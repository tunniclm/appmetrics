 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


/*
 * Wrapper classes for converting agent classes to JS
 */

#include "wrapper.h"

using namespace v8;

namespace monitoring {

/* static allocations */
template <class sourcetype>
Persistent<Function> NodePlugin<sourcetype>::constructor;

template <class sourcetype>
void NodePlugin<sourcetype>::Init(v8::Handle<v8::Object> exports) {
	/* constructor template */
	Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
	tpl->SetClassName(String::NewSymbol("Plugin"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	/* JS prototype */
	tpl->PrototypeTemplate()->SetAccessor(String::New("provid"), provid);
	tpl->PrototypeTemplate()->SetAccessor(String::New("sourceid"), sourceid);
	tpl->PrototypeTemplate()->SetAccessor(String::New("description"), description);
	tpl->PrototypeTemplate()->SetAccessor(String::New("uniqueID"), uniqueID);
	constructor = Persistent<Function>::New(tpl->GetFunction());
	exports->Set(String::NewSymbol("Plugin"), constructor);
}

template <class sourcetype>
void NodePlugin<sourcetype>::setWrapped(ibmras::monitoring::Plugin* p, ibmras::monitoring::agent::DataSource<sourcetype>* src) {
	plugin = p;
	this->src = src;
}

template <class sourcetype>
v8::Handle<v8::Value> NodePlugin<sourcetype>::provid(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
	HandleScope scope;
	NodePlugin<sourcetype>* plugin = ObjectWrap::Unwrap<NodePlugin<sourcetype> >(info.This());
	return scope.Close(Int32::NewFromUnsigned(plugin->getSrc()->getProvID()));
}

template <class sourcetype>
Handle<Value> NodePlugin<sourcetype>::sourceid(Local<String> property, const AccessorInfo &info) {
	HandleScope scope;
	NodePlugin* plugin = ObjectWrap::Unwrap<NodePlugin>(info.This());
	return scope.Close(Int32::NewFromUnsigned(plugin->getSrc()->getSourceID()));
}

template <class sourcetype>
Handle<Value> NodePlugin<sourcetype>::description(Local<String> property, const AccessorInfo &info) {
	HandleScope scope;
	NodePlugin<sourcetype>* plugin = ObjectWrap::Unwrap<NodePlugin<sourcetype> >(info.This());
	return scope.Close(String::New(plugin->getSrc()->getDescription()));
}

template <class sourcetype>
Handle<Value> NodePlugin<sourcetype>::uniqueID(Local<String> property, const AccessorInfo &info) {
	HandleScope scope;
	NodePlugin<sourcetype>* plugin = ObjectWrap::Unwrap<NodePlugin<sourcetype> >(info.This());
	return scope.Close(String::New(plugin->getSrc()->getUniqueID().c_str()));
}


template <class sourcetype>
Handle<Value> NodePlugin<sourcetype>::New(const Arguments& args) {
	HandleScope scope;

	if (args.IsConstructCall()) {   // Invoked as constructor: `new Plugin(...)`
		NodePlugin<sourcetype>* plugin = new NodePlugin<sourcetype>;
		plugin->Wrap(args.This());
		return args.This();
	} else {  // Invoked as plain function `Plugin(...)`, turn into construct call.
		const int argc = 1;
		Local<Value> argv[argc] = { args[0] };
		return scope.Close(constructor->NewInstance(argc, argv));
	}
}

template <class sourcetype>
Local<Object> NodePlugin<sourcetype>::NewInstance() {
	HandleScope scope;

	const unsigned argc = 0;
	Local<Object> instance = constructor->NewInstance(argc, NULL);

	return scope.Close(instance);
}

template class NodePlugin<pullsource>;
template class NodePlugin<pushsource>;

}


