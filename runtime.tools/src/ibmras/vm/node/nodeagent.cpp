 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include "node.h"
#include "nan.h"
#include "uv.h"
#include "ibmras/monitoring/AgentExtensions.h"
#include "ibmras/common/logging.h"
#include "ibmras/common/Logger.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/PropertiesFile.h"
#include <string>
#include <cstring>
#include <fstream>
#include <cstdlib>

#if defined(_WINDOWS)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

using namespace v8;

static std::string* appDir;
static std::string* hcDir;
static bool running = false;

IBMRAS_DEFINE_LOGGER("node");

static std::string ToStdString(Local<String> s) {
	char *buf = new char[s->Length() + 1];
	s->WriteUtf8(buf); // (FLORINCR) not sure if this or just Write()
	std::string result(buf);
	delete[] buf;
	return result;
}

#if defined(_WINDOWS)
//	std::cout << "Test empty: " << port_dirname("") << std::endl;
//	std::cout << "Test /: " << port_dirname("/") << std::endl;
//	std::cout << "Test a: " << port_dirname("a") << std::endl;
//	std::cout << "Test a/b: " << port_dirname("a/b") << std::endl;
//	std::cout << "Test /a/b: " << port_dirname("/a/b") << std::endl;
//	std::cout << "Test //a/b: " << port_dirname("//a/b") << std::endl;
//	std::cout << "Test /a//b: " << port_dirname("/a//b") << std::endl;
//	std::cout << "Test ./: " << port_dirname("./") << std::endl;
//	std::cout << "Test a/b/: " << port_dirname("a/b/") << std::endl;
static std::string port_dirname(const std::string& filename) {
	if (filename.length() == 0) return std::string(".");
	
	// Check for and ignore trailing slashes
	size_t lastpos = filename.length() - 1;
	while (lastpos > 0 && (filename[lastpos] == '/' || filename[lastpos] == '\\')) {
		lastpos--;
	}
	
	std::size_t slashpos = filename.rfind("/", lastpos);
	std::size_t bslashpos = filename.rfind("\\", lastpos);
	if (slashpos == std::string::npos && bslashpos == std::string::npos) {
		// No slashes
		return std::string(".");
	} else {
		// 
		std::size_t pos;
		if (slashpos != std::string::npos) pos = slashpos;
		else if (bslashpos != std::string::npos) pos = bslashpos;
		else pos = (slashpos > bslashpos) ? slashpos : bslashpos;
		if (pos == 0) {
			return filename.substr(0, 1);
		} else {
			// Remove trailing slashes
			size_t endpos = pos;
			while (endpos > 0 && (filename[endpos] == '/' || filename[endpos] == '\\')) {
				endpos--;
			}
			return filename.substr(0, endpos + 1);
		}
	}
}
#else
#include <libgen.h>
static std::string port_dirname(const std::string& filename) {
	char *fname = new char[filename.length() + 1];
	std::strcpy(fname, filename.c_str());
	std::string result(dirname(fname));
	delete[] fname;
	return result;
}
#endif

static std::string fileJoin(const std::string& path, const std::string& filename) {
#if defined(_WINDOWS)
	static const std::string fileSeparator("\\");
#else
	static const std::string fileSeparator("/");
#endif
	return path + fileSeparator + filename;
}

static bool FileExists(const std::string& filename) {
	std::ifstream f(filename.c_str());
	if (f.good()) {
		f.close();
		return true;
	} else {
		f.close();
		return false;
	}
}

static ibmras::common::PropertiesFile* LoadPropertiesFile(const std::string& filename, const char* desc) {
	ibmras::common::PropertiesFile* props = NULL;
	IBMRAS_LOG_1(debug, "Attempting to load properties from %s", desc); 
	if (FileExists(filename)) {
		IBMRAS_LOG_1(debug, "Loading properties from file '%s'", filename.c_str());
	 	props = new ibmras::common::PropertiesFile;
	 	props->load(filename);
	} else {
		IBMRAS_LOG_1(debug, "Properties file '%s' not found", filename.c_str());
	}
	return props;
}

static std::string* GetModuleDir(Handle<Object> module) {
	std::string moduleFilename(ToStdString(module->Get(NanNew<String>("filename"))->ToString()));
	return new std::string(port_dirname(moduleFilename));
}

static Local<Object> GetProcessObject() {
	return NanGetCurrentContext()->Global()->Get(NanNew<String>("process"))->ToObject();
}

static std::string* FindAppDir() {
	Handle<Value> mainModule = GetProcessObject()->Get(NanNew<String>("mainModule"));
	if (!mainModule->IsUndefined()) {
		return GetModuleDir(mainModule->ToObject());
	}
	return NULL;
}

static ibmras::common::PropertiesFile* LoadProperties() {
	ibmras::common::PropertiesFile* props = NULL;
	
	// Load from application directory, if possible
	if (appDir != NULL) {
		std::string propFilename(fileJoin(*appDir, std::string("healthcenter.properties")));
		props = LoadPropertiesFile(propFilename, "application directory");
	} else {
		IBMRAS_LOG(debug, "Cannot load properties from application directory, main module not defined");
	}
		
	// Load from current working directory, if possible
	if (props == NULL) {
		std::string propFilename("healthcenter.properties");
		props = LoadPropertiesFile(propFilename, "current working directory");
	}
	
	// Load from module directory
	if (props == NULL && hcDir != NULL) {
		std::string propFilename(fileJoin(*hcDir, std::string("healthcenter.properties")));
		props = LoadPropertiesFile(propFilename, "healthcenter directory");
	}

	return props;
}

NAN_METHOD(Start) {
	NanScope();
	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();
	if (!running) {
		running = true;

		agent->init();
	
		// Force MQTT on for now
		agent->setAgentProperty("mqtt", "on");

		agent->start();
	}
	NanReturnUndefined();
}

NAN_METHOD(Stop) {
	NanScope();
	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();
	if (running) {
		running = false;
		agent->stop();
		agent->shutdown();
	}
	NanReturnUndefined();
}

NAN_METHOD(spath) {
	NanScope();
	Local<String> value = args[0]->ToString();
	
	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();
	agent->setAgentProperty("plugin.path", ToStdString(value));

	NanReturnUndefined();
}

NAN_METHOD(setLogLevel) {
	NanScope();
	Local<Number> level = Local<Number>::Cast(args[0]);
	ibmras::common::logging::Level lvl = static_cast<ibmras::common::logging::Level>(level->Int32Value());
	ibmras::common::LogManager::getInstance()->setLevel(lvl); 
	NanReturnUndefined();
}

struct MessageData {
    const std::string* source;
    void* data;
    unsigned int size;
};

struct Listener {
    Persistent<Function> callback;
};


Listener* listener;

static void cleanupData(uv_handle_t *handle) {
    MessageData* payload = static_cast<MessageData*>(handle->data);
	free(payload->data);
    delete payload;
    delete handle;
}

static void EmitMessage(uv_async_t *handle, int status) {
    HandleScope scope;

    MessageData* payload = static_cast<MessageData*>(handle->data);

    TryCatch try_catch;
    const unsigned argc = 2;
    Local<Value> argv[argc];
    const char * source = (*payload->source).c_str();

    node::Buffer *message = node::Buffer::New(payload->size);
    memcpy(node::Buffer::Data(message), payload->data, payload->size);

    Local<Object> globalObj = Context::GetCurrent()->Global();
    Local<Function> bufferConstructor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));
    Handle<Value> constructorArgs[3] = { message->handle_, Integer::New(payload->size), Integer::New(0) };
    Local<Object> buffer = bufferConstructor->NewInstance(3, constructorArgs);

   	argv[0] = Local<Value>::New(String::New(source));
    argv[1] = buffer;

    listener->callback->Call(Context::GetCurrent()->Global(), argc, argv);
    if (try_catch.HasCaught()) {
       	node::FatalException(try_catch);
    }

    uv_close((uv_handle_t*) handle, cleanupData);
}

void SendData(const std::string &sourceId, unsigned int size, void *data) {
    uv_async_t *async = new uv_async_t;
    uv_async_init(uv_default_loop(), async, EmitMessage);

    MessageData* payload = new MessageData();
	/* 
	 * Make a copies of data and source as they will be freed when this function returns 
	 */
	void* dataCopy = malloc(size);
    memcpy(dataCopy, data, size);
	payload->source = new std::string(sourceId);
    payload->data = dataCopy;
	payload->size = size;

    async->data = payload;
    uv_async_send(async);
}

#if defined(_WINDOWS)
void* getApiFunc(std::string pluginPath, std::string funcName) {
	std::string apiPlugin = fileJoin(pluginPath, "apiplugin.dll");
	HMODULE handle = LoadLibrary(apiPlugin.c_str());
	if (handle == NULL) { 
		std::cerr << "API Connector Listener: failed to open apiplugin.dll \n";
		return NULL;
	}
	FARPROC apiFunc = GetProcAddress(handle, const_cast<char *>(funcName.c_str()));
	if (apiFunc == NULL) {
		std::cerr << "API Connector Listener: cannot find symbol '" << funcName << " in apiplugin.dll \n";
		return NULL;
	}
	return (void*) apiFunc;
}
#else
void* getApiFunc(std::string pluginPath, std::string funcName) {
	std::string apiPlugin = fileJoin(pluginPath, "libapiplugin.so");
	void* handle = dlopen(apiPlugin.c_str(), RTLD_LAZY);
    if (!handle) {
    	std::cerr << "API Connector Listener: failed to open libapiplugin.so: " << dlerror() << "\n";
    	return NULL;
    }
	void* apiFunc = dlsym(handle, funcName.c_str());
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
       	std::cerr << "API Connector Listener: cannot find symbol '" << funcName << "' in libapiplugin.so: " << dlsym_error <<
       		'\n';
       	dlclose(handle);
       	return NULL;
    }
	return apiFunc;
}
#endif

NAN_METHOD(nativeEmit) {
	NanScope();

	std::stringstream contentss;
	if (args[0]->IsString()) {
 		String::Utf8Value str(args[0]->ToString());
    	char *c_arg = *str;
    	contentss << c_arg << ":";
	} else {
		/*
		 *  Error handling as we don't have a valid parameter
		 */
		return ThrowException(Exception::TypeError(
            String::New("First argument must a event name string")));
	}
	if (args[1]->IsString()) {
		String::Utf8Value str(args[1]->ToString());
                char *c_arg = *str;
                contentss << c_arg;
	} else {
		/*
		 *  Error handling as we don't have a valid parameter
		 */
		return ThrowException(Exception::TypeError(
            String::New("Second argument must be a JSON string or a comma separated list of key value pairs")));
	}
  	contentss << '\n';
  	std::string content = contentss.str();
  	/*
  	 * Lookup of pushData call should be done in init and cached
  	 */
  	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();
  	std::string pluginPath = agent->getAgentProperty("plugin.path");
  	std::string funcName = std::string("pushData");
  	void (*pushData)(std::string&) = (void (*)(std::string&)) getApiFunc(pluginPath, funcName);	
  	pushData(content);
  	NanReturnUndefined();
}

NAN_METHOD(sendControlCommand) {
  	NanScope();

  	/* lookup the sendControl function - should be done once in an init() function */
  	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();
  	std::string pluginPath = agent->getAgentProperty("plugin.path");
  	std::string funcName = std::string("sendControl");
  	void (*sendControl)(std::string&, unsigned int, void*) = (void (*)(std::string&, unsigned int, void*)) getApiFunc(pluginPath, funcName);

  	if (args[0]->IsString() && args[1]->IsString()) {
    	String::Utf8Value topicArg(args[0]->ToString());
    	String::Utf8Value commandArg(args[1]->ToString());
    	std::string topic = std::string(*topicArg);
    	std::string command = std::string(*commandArg); 
    	unsigned int length = command.length();
   		sendControl(topic, length, (void*)command.c_str());
  	} else {
    	return ThrowException(Exception::TypeError(
        	String::New("Arguments must be strings containing the plugin name and control command")));
  	}

  	NanReturnUndefined();
}


NAN_METHOD(localConnect) {
    NanScope();
    if (!args[0]->IsFunction()) {
	    return ThrowException(Exception::TypeError(
    		String::New("First argument must be a callback function")));
    }
    Local<Function> callback = Local<Function>::Cast(args[0]);

    listener = new Listener();
    listener->callback = Persistent<Function>::New(callback);

	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();
	std::string pluginPath = agent->getAgentProperty("plugin.path");
	std::string funcName = std::string("registerListener");
	void (*registerListener)(void (*)(const std::string&, unsigned int, void*)) = (void (*)(void (*func)(const std::string&, unsigned int, void*))) getApiFunc(pluginPath, funcName);

	if (registerListener == NULL) {
		NanReturnUndefined();
	}

    void (*func)(const std::string&, unsigned int, void*);
    func = &SendData;
    registerListener(func);

    NanReturnUndefined();
}


void Init(Handle<Object> exports, Handle<Object> module) {
//	monitoring::NodePlugin<pullsource>::Init(exports);
//	monitoring::NodePlugin<pushsource>::Init(exports);

	exports->Set(NanNew<String>("start"), NanNew<FunctionTemplate>(Start)->GetFunction());
	exports->Set(NanNew<String>("spath"), NanNew<FunctionTemplate>(spath)->GetFunction());
	exports->Set(NanNew<String>("stop"), NanNew<FunctionTemplate>(Stop)->GetFunction());
	exports->Set(NanNew<String>("setLogLevel"), NanNew<FunctionTemplate>(setLogLevel)->GetFunction());
    exports->Set(NanNew<String>("localConnect"), NanNew<FunctionTemplate>(localConnect)->GetFunction());
	exports->Set(NanNew<String>("nativeEmit"), NanNew<FunctionTemplate>(nativeEmit)->GetFunction());
	exports->Set(NanNew<String>("sendControlCommand"), NanNew<FunctionTemplate>(sendControlCommand)->GetFunction());

	// Defaults
	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();
	agent->setAgentProperty("plugin.path", "./plugins");

	if (std::getenv("IBM_HC_NODE_EARLY_LOGGING") != NULL) {
		ibmras::common::LogManager::getInstance()->setLevel("node", "debug");
	}

	appDir = FindAppDir();
	hcDir = GetModuleDir(module);
	ibmras::common::PropertiesFile* props = LoadProperties();
	if (props != NULL) {
		agent->setProperties(*props);
		delete props;
	}
	agent->setProperty("agent.version", agent->getVersion());
	agent->setProperty("agent.native.build.date", agent->getBuildDate());
	agent->setLogLevels();

	IBMRAS_LOG_1(info, "Health Center %s", agent->getVersion().c_str());
}

NODE_MODULE(healthcenter, Init)
