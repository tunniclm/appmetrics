/*
 * nodeconnector.h
 *
 *  Created on: 6 Mar 2014
 *      Author: adam
 */

#ifndef NODECONNECTOR_H_
#define NODECONNECTOR_H_

#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include "node.h"
#include "ibmras/monitoring/connector/Connector.h"
#include "v8.h"
#include "ibmras/monitoring/agent/BucketList.h"

using namespace ibmras::monitoring::connector;

namespace nodecon {

class Callback : public Connector {
	int sendMessage(const std::string &sourceId, uint32 size, void *data);
	void registerReceiver(Receiver *receiver);
	int start();
	int stop();
	std::string getID() { return "JSClientCallback"; }
public:
	uint32 provid;
	uint32 srcid;
	v8::Persistent<v8::Function> cbfn;
	v8::Persistent<v8::Function> complete;
	Callback* next;
	bool remove;
	bool hasCompleteFn;
};

class DECL NodeConnector : public Connector {
public:
	static NodeConnector* getInstance();
	NodeConnector();
	int sendMessage(const std::string &sourceId, uint32 size, void *data);
	void registerReceiver(Receiver *receiver);
	int start();
	int stop();
	static v8::Handle<v8::Value> version(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	void registerListener(const std::string &sourceId, v8::Local<v8::Function> fn);
	void onComplete(uint32 provid, uint32 srcid, v8::Local<v8::Function> fn);
	void publish(v8::HandleScope* scope);
	std::string getID() { return "NodeConnector"; }
private:
	Callback* head;
	ibmras::monitoring::agent::BucketList* buckets;
	ibmras::monitoring::connector::Receiver* receiver;
};



}	/* end of namespace monitoring */


#endif /* NODECONNECTOR_H_ */
