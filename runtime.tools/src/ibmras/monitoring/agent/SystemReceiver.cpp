/*
 * SystemReceiver.cpp
 *
 *  Created on: 10 June 2014
 *      Author: stalleyj
 */

#include "ibmras/monitoring/agent/SystemReceiver.h"
#include "ibmras/monitoring/connector/Receiver.h"
#include "ibmras/monitoring/Plugin.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/monitoring/connector/configuration/ConfigurationConnector.h"
#include <iostream>

namespace ibmras {
namespace monitoring {
namespace agent {

int startReceiver() {
	return 0;
}

int stopReceiver() {
	return 0;
}

SystemReceiver::SystemReceiver() {
	name = "System receiver";
	pull = NULL;
	push = NULL;
	start = ibmras::monitoring::agent::startReceiver;
	stop = ibmras::monitoring::agent::stopReceiver;
	type = ibmras::monitoring::plugin::receiver;
	recvfactory = (RECEIVER_FACTORY) ibmras_getSystemReceiver;
	confactory = NULL;
}

SystemReceiver::~SystemReceiver() {
}

void SystemReceiver::receiveMessage(const std::string &id, uint32 size,
		void *data) {

	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();

	// If the topic is "datasources" it means we have had a request
	// to send back the source names and config (one for each bucket) to the client
	if (id == "datasources") {
		if(size <= 0 || data == NULL) {
			return;
		}
		std::string topic((char*)data, size);
		topic += "/datasource";

		ibmras::monitoring::connector::ConnectorManager *conMan =
				agent->getConnectionManager();

		ibmras::monitoring::agent::BucketList* buckets = agent->getBucketList();

		std::vector < std::string > ids = buckets->getIDs();

		for (uint32 i = 0; i < ids.size(); i++) {

			std::string config = agent->getConfig(ids[i]);

			std::stringstream str;
			str << ids[i];
			str << ",";
			str << config;
			str << std::endl;
			std::string msg = str.str();

			conMan->sendMessage(topic, msg.length(), (void*) msg.c_str());
		}
	} else if (id == "history") {
		std::string topic((char*) data, size);
		topic += "/history/";
		agent->republish(topic);
	} else if (id == "headless") {
		// force immediate update for pull sources
		agent->immediateUpdate();
	}
}

}
}
} /* end namespace agent */

void* ibmras_getSystemReceiver() {
	return new ibmras::monitoring::agent::SystemReceiver();
}

