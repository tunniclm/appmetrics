#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include "node.h"
#include "uv.h"
#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/common/logging.h"
#include "ibmras/common/Logger.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/PropertiesFile.h"
#include <fstream>
#include <cstdlib>

using namespace v8;

IBMRAS_DEFINE_LOGGER("NodeAgent");

Handle<Value> Start(const Arguments& args) {
  HandleScope scope;
	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();
	agent->init();

	// Force MQTT on for now
	agent->setAgentProperty("mqtt", "on");

	agent->start();
	return scope.Close(Undefined());
}

Handle<Value> Stop(const Arguments& args) {
  HandleScope scope;
	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();
	agent->stop();
	agent->shutdown();
	return scope.Close(Undefined());
}

Handle<Value> spath(const Arguments& args) {
  HandleScope scope;
	Local<String> value = args[0]->ToString();
	String *path = *value;
	int size = path->Length() + 1;
	char* buffer = new char[size];
	path->WriteAscii(buffer,0 , path->Length());
	buffer[path->Length()] = '\0';
	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();
	agent->setAgentProperty("plugin.path", buffer);
return scope.Close(Undefined());
}

Handle<Value> setLogLevel(const Arguments& args) {
	HandleScope scope;
	Local<Number> level = Local<Number>::Cast(args[0]);
	ibmras::common::logging::Level lvl = static_cast<ibmras::common::logging::Level>(level->Int32Value());
	ibmras::common::LogManager::getInstance()->setLevel(lvl); 
	return scope.Close(Undefined());
}

static std::string ToStdString(Local<String> s) {
	char *buf = new char[s->Length() + 1];
	s->WriteAscii(buf);
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

static bool file_exists(const std::string& filename) {
	std::ifstream f(filename.c_str());
	if (f.good()) {
		f.close();
		return true;
	} else {
		f.close();
		return false;
	}
}

static Local<Object> GetProcessObject() {
	return Context::GetCurrent()->Global()->Get(String::New("process"))->ToObject();
}

static ibmras::common::PropertiesFile* LoadPropertiesFile(const std::string& filename, const char* desc) {
	ibmras::common::PropertiesFile* props = NULL;
	IBMRAS_LOG_1(debug, "Attempting to load properties from %s", desc); 
	if (file_exists(filename)) {
		IBMRAS_LOG_1(debug, "Loading properties from file '%s'", filename.c_str());
	 	props = new ibmras::common::PropertiesFile;
	 	props->load(filename);
	} else {
		IBMRAS_LOG_1(debug, "Properties file '%s' not found", filename.c_str());
	}
	return props;
}

static ibmras::common::PropertiesFile* LoadProperties(Handle<Object> module) {
#if defined(_WINDOWS)
	static const std::string file_separator("\\");
#else
	static const std::string file_separator("/");
#endif
	ibmras::common::PropertiesFile* props = NULL;
	
	// Load from application directory, if possible
	Handle<Value> mainModule = GetProcessObject()->Get(String::New("mainModule"));
	if (!mainModule->IsUndefined()) {
		std::string mainModuleFilename(ToStdString(mainModule->ToObject()->Get(String::New("filename"))->ToString()));
		std::string mainModuleDirname(port_dirname(mainModuleFilename));
		std::string propFilename(mainModuleDirname + file_separator + std::string("healthcenter.properties"));
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
	if (props == NULL) {
		std::string moduleFilename(ToStdString(module->Get(String::New("filename"))->ToString()));
		std::string moduleDirname(port_dirname(moduleFilename));
		std::string propFilename(moduleDirname + file_separator + std::string("healthcenter.properties"));
		props = LoadPropertiesFile(propFilename, "healthcenter directory");
	}
	
	return props;
}

void Init(Handle<Object> exports, Handle<Object> module) {
//	monitoring::NodePlugin<pullsource>::Init(exports);
//	monitoring::NodePlugin<pushsource>::Init(exports);

	exports->Set(String::NewSymbol("start"), FunctionTemplate::New(Start)->GetFunction());
	exports->Set(String::NewSymbol("spath"), FunctionTemplate::New(spath)->GetFunction());
	exports->Set(String::NewSymbol("stop"), FunctionTemplate::New(Stop)->GetFunction());

	exports->Set(String::NewSymbol("setLogLevel"), FunctionTemplate::New(setLogLevel)->GetFunction());

	// Defaults
	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();
	agent->setAgentProperty("plugin.path", "./plugins");

	if (std::getenv("IBM_HC_NODEAGENT_EARLY_LOGGING") != NULL) {
		ibmras::common::LogManager::getInstance()->setLevel("Node", "debug");
	}

	ibmras::common::PropertiesFile* props = LoadProperties(module);
	if (props != NULL) {
		agent->setProperties(*props);
		delete props;
	}	
	agent->setLogLevels();
}

NODE_MODULE(healthcenter, Init)
