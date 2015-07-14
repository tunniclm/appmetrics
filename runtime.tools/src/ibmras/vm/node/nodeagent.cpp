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
#include "ibmras/monitoring/Typesdef.h"
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

#define PROPERTIES_FILE "appmetrics.properties"

namespace funcs {
	void (*pushData)(std::string&);
	void (*sendControl)(std::string&, unsigned int, void*);
	void (*registerListener)(void (*)(const std::string&, unsigned int, void*));
}

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
		std::string propFilename(fileJoin(*appDir, std::string(PROPERTIES_FILE)));
		props = LoadPropertiesFile(propFilename, "application directory");
	} else {
		IBMRAS_LOG(debug, "Cannot load properties from application directory, main module not defined");
	}
		
	// Load from current working directory, if possible
	if (props == NULL) {
		std::string propFilename(PROPERTIES_FILE);
		props = LoadPropertiesFile(propFilename, "current working directory");
	}
	
	// Load from module directory
	if (props == NULL && hcDir != NULL) {
		std::string propFilename(fileJoin(*hcDir, std::string(PROPERTIES_FILE)));
		props = LoadPropertiesFile(propFilename, "appmetrics directory");
	}

	return props;
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
#if defined(_AIX)
	std::string libname = "libapiplugin.a";
#else
	std::string libname = "libapiplugin.so";
#endif
	std::string apiPlugin = fileJoin(pluginPath, libname);
	void* handle = dlopen(apiPlugin.c_str(), RTLD_LAZY);
    if (!handle) {
		std::cerr << "API Connector Listener: failed to open " << libname << ": " << dlerror() << "\n";
    	return NULL;
    }
	void* apiFunc = dlsym(handle, funcName.c_str());
	if (!apiFunc) {
       	std::cerr << "API Connector Listener: cannot find symbol '" << funcName << "' in " << libname << ": " << dlerror() << "\n";
       	dlclose(handle);
       	return NULL;
	}
	return apiFunc;
}
#endif

static void initApiFuncs() {
  	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();
  	std::string pluginPath = agent->getAgentProperty("plugin.path");
  	
	funcs::pushData = (void (*)(std::string&)) getApiFunc(pluginPath, std::string("pushData"));
  	funcs::sendControl = (void (*)(std::string&, unsigned int, void*)) getApiFunc(pluginPath, std::string("sendControl"));
	funcs::registerListener = (void (*)(void (*func)(const std::string&, unsigned int, void*))) getApiFunc(pluginPath, std::string("registerListener"));
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
	initApiFuncs();
	
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
    NanCallback *callback;
};


Listener* listener;

static void cleanupData(uv_handle_t *handle) {
    MessageData* payload = static_cast<MessageData*>(handle->data);
	free(payload->data);
    delete payload;
    delete handle;
}

static void EmitMessage(uv_async_t *handle, int status) {

    MessageData* payload = static_cast<MessageData*>(handle->data);

    TryCatch try_catch;
    const unsigned argc = 2;
    Local<Value> argv[argc];
    const char * source = (*payload->source).c_str();

	Local<Object> buffer = NanNewBufferHandle((char*)payload->data, payload->size);
   	argv[0] = NanNew<String>(source);
    argv[1] = buffer;

    listener->callback->Call(argc, argv);
    if (try_catch.HasCaught()) {
       	node::FatalException(try_catch);
    }

    uv_close((uv_handle_t*) handle, cleanupData);
}

void SendData(const std::string &sourceId, unsigned int size, void *data) {
    uv_async_t *async = new uv_async_t;
    uv_async_init(uv_default_loop(), async, (uv_async_cb)EmitMessage);

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
		return NanThrowError("First argument must a event name string");
	}
	if (args[1]->IsString()) {
		String::Utf8Value str(args[1]->ToString());
                char *c_arg = *str;
                contentss << c_arg;
	} else {
		/*
		 *  Error handling as we don't have a valid parameter
		 */
		return NanThrowError("Second argument must be a JSON string or a comma separated list of key value pairs");
	}
  	contentss << '\n';
  	std::string content = contentss.str();

  	funcs::pushData(content);
  	NanReturnUndefined();
}

NAN_METHOD(sendControlCommand) {
  	NanScope();

  	if (args[0]->IsString() && args[1]->IsString()) {
    	String::Utf8Value topicArg(args[0]->ToString());
    	String::Utf8Value commandArg(args[1]->ToString());
    	std::string topic = std::string(*topicArg);
    	std::string command = std::string(*commandArg); 
    	unsigned int length = command.length();
   		funcs::sendControl(topic, length, (void*)command.c_str());
  	} else {
    	return NanThrowError("Arguments must be strings containing the plugin name and control command");
  	}

  	NanReturnUndefined();
}


NAN_METHOD(localConnect) {
    NanScope();
    if (!args[0]->IsFunction()) {
	    return NanThrowError("First argument must be a callback function");
    }
    NanCallback *callback = new NanCallback(args[0].As<Function>());

    listener = new Listener();
    listener->callback = callback;

    void (*func)(const std::string&, unsigned int, void*);
    func = &SendData;
    funcs::registerListener(func);

    NanReturnUndefined();
}

// Unfortunately native modules don't get a reference
// to require.cache as this happens in Module._compile()
// and native modules aren't compiled, they are loaded
// directly by NativeModule.require() (in Module._load())
// So we need to get it from Module._cache instead (by
// executing require('module')._cache)
static Local<Object> GetRequireCache(Handle<Object> module) {
	NanEscapableScope();
	Handle<Value> args[] = { NanNew<String>("module") };
	Local<Value> m = module->Get(NanNew<String>("require"))->ToObject()->CallAsFunction(NanGetCurrentContext()->Global(), 1, args);
	Local<Object> cache = m->ToObject()->Get(NanNew<String>("_cache"))->ToObject();
	return NanEscapeScope(cache);
}

// Check whether the filepath given looks like it's a file in the
// appmetrics npm module directory. Here we are checking it ends
// with appmetrics/somefile, but perhaps node_modules/appmetrics/somefile
// would be more accurate?
static bool IsAppMetricsFile(std::string expected, std::string potentialMatch) {
	std::string endsWithPosix = "appmetrics/" + expected;
	std::string endsWithWindows = "appmetrics\\" + expected;

	int startAt = potentialMatch.length() - endsWithPosix.length();
	if (startAt >= 0 && potentialMatch.compare(startAt, endsWithPosix.length(), endsWithPosix) == 0) {
		return true;
	}

	startAt = potentialMatch.length() - endsWithWindows.length();
	if (startAt >= 0 && potentialMatch.compare(startAt, endsWithWindows.length(), endsWithWindows) == 0) {
		return true;
	}
	
	return false;
}

// Check if this appmetrics agent native module is loaded via the node-hc command.
// This is actually checking if this module has appmetrics/launcher.js as it's grandparent.
// For reference:
// A locally loaded module would have ancestry like:
//   ...
//   ^-- some_module_that_does_require('appmetrics') (grandparent)
//       ^--- .../node_modules/appmetrics/index.js (parent)
//            ^-- .../node_modules/appmetrics/appmetrics.node (this)
//
// A globally loaded module would have ancestry like:
//   .../node_modules/appmetrics/launcher.js (grandparent)
//   ^--- .../node_modules/appmetrics/index.js (parent)
//        ^-- .../node_modules/appmetrics/appmetrics.node (this)
//
static bool IsGlobalAgent(Handle<Object> module) {
	NanScope();
	Local<Value> parent = module->Get(NanNew<String>("parent"));
	if (parent->IsObject()) {
		Local<Value> filename = parent->ToObject()->Get(NanNew<String>("filename"));
		if (filename->IsString() && IsAppMetricsFile("index.js", ToStdString(filename->ToString()))) {
			Local<Value> grandparent = parent->ToObject()->Get(NanNew<String>("parent"));
			Local<Value> gpfilename = grandparent->ToObject()->Get(NanNew<String>("filename"));
			if (gpfilename->IsString() && IsAppMetricsFile("launcher.js", ToStdString(gpfilename->ToString()))) {
				return true;
			}
		}
	}
	return false;
}

// Check if a global appmetrics agent module is already loaded.
// This is actually searching the module cache for a module with filepath 
// ending .../appmetrics/launcher.js
static bool IsGlobalAgentAlreadyLoaded(Handle<Object> module) {
	NanScope();
	Local<Object> cache = GetRequireCache(module);
	Local<Array> props = cache->GetOwnPropertyNames();
	if (props->Length() > 0) {
		for (uint32_t i=0; i<props->Length(); i++) {
			Local<Value> entry = props->Get(i);
			if (entry->IsString() && IsAppMetricsFile("launcher.js", ToStdString(entry->ToString()))) {
				return true;
			}
		}
	}
	
	return false;	
}

void Init(Handle<Object> exports, Handle<Object> module) {
//	monitoring::NodePlugin<pullsource>::Init(exports);
//	monitoring::NodePlugin<pushsource>::Init(exports);

	if (!IsGlobalAgent(module) && IsGlobalAgentAlreadyLoaded(module)) {
		NanThrowError("Conflicting appmetrics module was already loaded by node-hc. Try running with node instead.");
		return;
	}

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

	IBMRAS_LOG_1(info, "Node Application Metrics %s", agent->getVersion().c_str());
}

NODE_MODULE(appmetrics, Init)
