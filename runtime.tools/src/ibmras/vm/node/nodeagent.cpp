#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include "node.h"
#include "uv.h"
#include "ibmras/common/Logger.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/common/port/ThreadData.h"
#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/common/PropertiesFile.h"
#include "wrapper.h"

using namespace v8;

Persistent<Function> cbfn;
ibmras::common::port::Lock* dataqLock = new ibmras::common::port::Lock;
bool agentRunning = false;
uv_timer_t* timer = new uv_timer_t;

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

Handle<Value> plugins(Local<String> property, const v8::AccessorInfo &info) {
//Handle<Value> plugins(const Arguments& args) {
	std::cerr << "Building list of plugins\n";
	HandleScope scope;
	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();
	ibmras::monitoring::agent::DataSourceList<pullsource> pullList = agent->getPullSources();
	ibmras::monitoring::agent::DataSourceList<pushsource> pushList = agent->getPushSources();
	uint32 pullcount = pullList.getSize();
	uint32 pushcount = pushList.getSize();
	Local<Array> plugins = Array::New((int)(pullcount + pushcount));
	for(uint32 i = 0; i < pullcount; i++) {
		ibmras::monitoring::agent::DataSource<pullsource>* dsrc = pullList.getItem(i);
		std::cerr << "Plugin(Pull) :: " << dsrc->getProviderName() << "\n";
		Local<Object> obj = monitoring::NodePlugin<pullsource>::NewInstance();
		monitoring::NodePlugin<pullsource>* plugin = node::ObjectWrap::Unwrap<monitoring::NodePlugin<pullsource> >(obj);
		plugin->setWrapped(NULL, dsrc);
		plugins->Set(i, obj);
	}
	for(uint32 i = 0; i < pushcount; i++) {
		ibmras::monitoring::agent::DataSource<pushsource>* dsrc = pushList.getItem(i);
		std::cerr << "Plugin(Push) :: " << dsrc->getProviderName() << "\n";
		Local<Object> obj = monitoring::NodePlugin<pushsource>::NewInstance();
		monitoring::NodePlugin<pushsource>* plugin = node::ObjectWrap::Unwrap<monitoring::NodePlugin<pushsource> >(obj);
		plugin->setWrapped(NULL, dsrc);
		plugins->Set(i+pullcount, obj);
	}

	//return scope.Close(MyObject::NewInstance(args));
	return scope.Close(plugins);
}


void Init(Handle<Object> exports, Handle<Object> module) {
	monitoring::NodePlugin<pullsource>::Init(exports);
	monitoring::NodePlugin<pushsource>::Init(exports);

	exports->Set(String::NewSymbol("start"), FunctionTemplate::New(Start)->GetFunction());
	exports->Set(String::NewSymbol("spath"), FunctionTemplate::New(spath)->GetFunction());
	exports->Set(String::NewSymbol("stop"), FunctionTemplate::New(Stop)->GetFunction());

	exports->SetAccessor(String::New("plugins"), plugins, NULL);

	exports->Set(String::NewSymbol("setLogLevel"), FunctionTemplate::New(setLogLevel)->GetFunction());

	// Defaults
	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();
	agent->setAgentProperty("plugin.path", "./plugins");
	
	// Load healthcenter.properties
	ibmras::common::PropertiesFile props;
	props.load("healthcenter.properties");
	agent->setProperties(props);
	agent->setLogLevels();
}

NODE_MODULE(healthcenter, Init)

