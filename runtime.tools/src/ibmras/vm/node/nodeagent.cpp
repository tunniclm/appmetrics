#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include "ibmras/common/Logger.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/common/port/ThreadData.h"
#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/common/PropertiesFile.h"
#include "node.h"
#include "uv.h"
#include "ibmras/vm/node/nodeconnector.h"
#include "wrapper.h"

using namespace v8;

Persistent<Function> cbfn;
ibmras::common::port::Lock* dataqLock = new ibmras::common::port::Lock;
bool agentRunning = false;
nodecon::NodeConnector* connector;
uv_timer_t* timer = new uv_timer_t;

void timercb(uv_timer_s* data, int status) {
	HandleScope scope;
	connector->publish(&scope);
	scope.Close(Undefined());
}

Handle<Value> Start(const Arguments& args) {
  HandleScope scope;
	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();
	agent->init();

	// Force MQTT on for now
	agent->setAgentProperty("mqtt", "on");

	agent->start();
	connector = reinterpret_cast<nodecon::NodeConnector*>(agent->getConnector("NodeConnector"));
	if(!connector) {
		std::cerr << "!!!!!!!!!!NULL CONNECTOR!!!!!!!!!!!!!\n";
	} else {
		uv_timer_init(uv_default_loop(), timer);
		uv_unref((uv_handle_t*) timer); // don't prevent event loop exit
		std::cerr << "############Starting UV timer\n";
		uv_timer_start(timer, timercb, 1000, 1000);
	}
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

Handle<Value> addListener(const Arguments& args) {
  HandleScope scope;
	Local<Function> cb = Local<Function>::Cast(args[1]);
	Local<String> id = Local<String>::Cast(args[0]);
	char* buffer = new char[id->Length() + 1];
	id->WriteAscii(buffer, 0, id->Length(), 0);
	buffer[id->Length()] = '\0';
	std::string* str = new std::string(buffer);
	connector->registerListener(*str, cb);
	delete[] buffer;
	//connector.setCallback(Persistent<Function>::New(cb));
/*
  Local<Function> cb = Local<Function>::Cast(args[0]);
  const unsigned argc = 1;
  Local<Value> argv[argc] = { Local<Value>::New(String::New("hello world")) };
  cb->Call(Context::GetCurrent()->Global(), argc, argv);
*/
  return scope.Close(Undefined());
}

Handle<Value> setLogLevel(const Arguments& args) {
	HandleScope scope;
	Local<Number> level = Local<Number>::Cast(args[0]);
	ibmras::common::logging::Level lvl = static_cast<ibmras::common::logging::Level>(level->Int32Value());
	ibmras::common::LogManager::getInstance()->setLevel(lvl); 
	return scope.Close(Undefined());
}


Handle<Value> addOnComplete(const Arguments& args) {
  HandleScope scope;
	Local<Function> cb = Local<Function>::Cast(args[2]);
	Local<Number> provid = Local<Number>::Cast(args[0]);
	Local<Number> srcid = Local<Number>::Cast(args[1]);
	connector->onComplete(provid->Int32Value(), srcid->Int32Value(), cb);
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

	exports->Set(String::NewSymbol("addlistener"), FunctionTemplate::New(addListener)->GetFunction());
	exports->Set(String::NewSymbol("addOnComplete"), FunctionTemplate::New(addOnComplete)->GetFunction());
	exports->Set(String::NewSymbol("start"), FunctionTemplate::New(Start)->GetFunction());
	exports->Set(String::NewSymbol("spath"), FunctionTemplate::New(spath)->GetFunction());
	exports->Set(String::NewSymbol("stop"), FunctionTemplate::New(Stop)->GetFunction());

	exports->SetAccessor(String::New("version"), nodecon::NodeConnector::version, NULL);
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

