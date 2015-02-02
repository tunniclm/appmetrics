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

void Init(Handle<Object> exports, Handle<Object> module) {
//	monitoring::NodePlugin<pullsource>::Init(exports);
//	monitoring::NodePlugin<pushsource>::Init(exports);

	exports->Set(NanNew<String>("start"), NanNew<FunctionTemplate>(Start)->GetFunction());
	exports->Set(NanNew<String>("spath"), NanNew<FunctionTemplate>(spath)->GetFunction());
	exports->Set(NanNew<String>("stop"), NanNew<FunctionTemplate>(Stop)->GetFunction());
	exports->Set(NanNew<String>("setLogLevel"), NanNew<FunctionTemplate>(setLogLevel)->GetFunction());

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
	agent->setAgentProperty("agent.version", agent->getVersion());
	agent->setAgentProperty("agent.native.build.date", agent->getBuildDate());
	agent->setLogLevels();

	IBMRAS_LOG_1(info, "Health Center %s", agent->getVersion().c_str());
}

NODE_MODULE(healthcenter, Init)
