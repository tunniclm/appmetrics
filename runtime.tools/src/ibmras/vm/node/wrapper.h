/*
 * wrapper.h
 *
 *  Created on: 7 Mar 2014
 *      Author: adam
 */

#ifndef ibmras_vm_node_wrapper_h
#define ibmras_vm_node_wrapper_h

#ifndef BUILDING_NODE_EXTENSION
 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#define BUILDING_NODE_EXTENSION
#endif

#include "node.h"
#include "ibmras/common/port/ThreadData.h"
#include "ibmras/monitoring/Plugin.h"
#include "ibmras/monitoring/agent/DataSource.h"
#include "ibmras/monitoring/AgentExtensions.h"

using namespace v8;

namespace monitoring {
	template <class sourcetype>
	class NodePlugin : public node::ObjectWrap {
	public:
		static void Init(v8::Handle<v8::Object> exports);
		static v8::Local<v8::Object> NewInstance();
		static v8::Handle<v8::Value> provid(v8::Local<v8::String> property, const v8::AccessorInfo &info);
		static Handle<Value> sourceid(Local<String> property, const AccessorInfo &info);
		static Handle<Value> description(Local<String> property, const AccessorInfo &info);
		static Handle<Value> uniqueID(Local<String> property, const AccessorInfo &info);
		void setWrapped(ibmras::monitoring::Plugin* p, ibmras::monitoring::agent::DataSource<sourcetype> *src);		/* set the wrapped plugin */
		ibmras::monitoring::agent::DataSource<sourcetype>* getSrc() { return src; } ;
	private:
		static v8::Handle<v8::Value> New(const v8::Arguments& args);
		static v8::Persistent<v8::Function> constructor;
		ibmras::monitoring::Plugin* plugin;		/* wrapped plugin from the port library */
		ibmras::monitoring::agent::DataSource<sourcetype> *src;
	};
}


#endif /* ibmras_vm_node_wrapper_h */
