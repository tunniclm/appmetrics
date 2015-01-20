 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#include "nodeconnector.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/common/logging.h"
#include <cstdio>

#if defined(_WINDOWS)
#define NODECONNECTOR_DECL __declspec(dllexport)	/* required for DLLs to export the plugin functions */
#else
#define NODECONNECTOR_DECL
#endif

using namespace v8;
using namespace ibmras::monitoring::agent;

namespace nodecon {

IBMRAS_DEFINE_LOGGER("NodeCon");
NodeConnector* connector = new NodeConnector;

void NodeConnector::publish(HandleScope* scope) {
	if (head == NULL) return;
	Callback* current = head;
	Callback* next = NULL;
	while(current) {	/* remove any old callbacks before publishing */
		if(current->remove) {
			if(current->hasCompleteFn) {
				current->complete->Call(Context::GetCurrent()->Global(), 0, NULL);
			}
			next = current->next;
			if(current == head) {
				head = next;
			}
			delete current;
			current = next;
		} else {
			Bucket* bucket = buckets->findBucket(current->provid, current->srcid);
			if(bucket) {
				bucket->publish(*current);
			}
			current = current->next;
		}
	}
}

/* this will execute inside an already created scope */
int Callback::sendMessage(const std::string &sourceId, uint32 size, void *data) {
	const unsigned count = 1;			/* argument return count */
	Local<Value> v = Local<Value>::New(String::New(reinterpret_cast<const char *>(data), size));
	Local<Value> args[count] = { v };
	cbfn->Call(Context::GetCurrent()->Global(), count, args);
	remove = true;		/* this callback is now eligible for removal */
	return 0;
}

int NodeConnector::sendMessage(const std::string &sourceId, uint32 size, void *data) {
	Bucket* bucket = buckets->findBucket(sourceId);
	if(bucket && size > 0 && data != NULL) {
		monitordata* md = new monitordata;	/* copy into a monitor data structure so that we can add to a bucket */
		md->persistent = false;
		md->provID = bucket->getProvID();
		md->sourceID = bucket->getSourceID();
		md->size = size;
		md->data = data;
//		BucketDataQueueEntry* entry = new BucketDataQueueEntry(md);
//		bucket->add(entry);
	}
	return 0;
}

v8::Handle<v8::Value> NodeConnector::version(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
	return v8::String::New("1.0");
}

void NodeConnector::registerListener(const std::string &sourceId, Local<Function> fn) {
	uint32 count = 0;
	Bucket* bucket = buckets->findBucket(sourceId);
	if(!bucket) {
		/* cannot locate the bucket for the specified source ID */
		IBMRAS_DEBUG_1(warning,  "Attempt to register for an unknown source [%s]", sourceId.c_str());
		return;
	}
	Callback* cb = new Callback;
	cb->provid = bucket->getProvID();
	cb->srcid = bucket->getSourceID();
	cb->cbfn = Persistent<Function>::New(fn);
	cb->hasCompleteFn = false;
	cb->next = NULL;
	cb->remove = false;
	Callback* insert = head;
	if(insert) {
		while(insert->next) {
			insert = insert->next;
			count++;
		}
		insert->next = cb;		/* add to the end of the list */
	} else {
		head = cb;				/* nothing in the list so start with this entry */
	}
	count++;
	IBMRAS_DEBUG_2(warning,  "Registered new listener (%s): count = %d", sourceId.c_str(), count);
}

void NodeConnector::onComplete(uint32 provid, uint32 srcid, Local<Function> fn) {
	Callback* cb = head;
	while(cb) {
		if((cb->provid == provid) && (cb->srcid == srcid)) {
			cb->complete = Persistent<Function>::New(fn);
			cb->hasCompleteFn = true;
			return;		/* found the callback to add an oncomplete call to */
		}
		cb = cb->next;
	}
	IBMRAS_DEBUG_2(warning,  "Failed to set oncomplete [%d:%d]", provid, srcid);
}

NodeConnector::NodeConnector() {
	head = NULL;		/* clear out linked list of callbacks */
	buckets = NULL;
}

void NodeConnector::registerReceiver(Receiver *receiver) {
	this->receiver = receiver;
}

int NodeConnector::start() {
	IBMRAS_DEBUG(info,  "Starting node.js connector, registering against all data sources");
	buckets = new BucketList;		/* create buckets, one for each data source */
	Agent* agent = Agent::getInstance();
	buckets->add(agent->getPushSources().getBuckets());
	buckets->add(agent->getPullSources().getBuckets());
	agent->addConnector(this);		/* subscribe for all notification events */
	return 0;
}

int NodeConnector::stop() {
	return 0;
}

void Callback::registerReceiver(Receiver *receiver) {

}

int Callback::start() {
	return 0;
}

int Callback::stop() {
	return 0;
}

NodeConnector* NodeConnector::getInstance() {
	return connector;
}

}	/* end of namespace monitoring */

extern "C" {
NODECONNECTOR_DECL int ibmras_monitoring_plugin_start() {
	return 0;
}

NODECONNECTOR_DECL int ibmras_monitoring_plugin_stop() {
	return 0;
}

NODECONNECTOR_DECL void* ibmras_monitoring_getConnector(const char* properties) {
	return nodecon::connector;
}
}
