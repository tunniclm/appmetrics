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
	handle = NULL;
	type = ibmras::monitoring::plugin::receiver;
	recvfactory = (RECEIVER_FACTORY) ibmras_getSystemReceiver;
	confactory = NULL;
}

SystemReceiver::~SystemReceiver() {
}

void SystemReceiver::receiveMessage(const std::string &id, uint32 size,
		void *data) {

	// If the topic is "datasources" it means we have had a request
	// to send back the source names and config (one for each bucket) to the client
	if (id == "datasources") {
		if(size <= 0 || data == NULL) {
			return;
		}

		std::string topic((char*)data, size);
		topic += "/datasource";


		ibmras::monitoring::agent::Agent* agent =
				ibmras::monitoring::agent::Agent::getInstance();

		ibmras::monitoring::connector::ConnectorManager *conMan =
				agent->getConnectionManager();

		ibmras::monitoring::agent::BucketList* buckets = agent->getBucketList();

		std::vector < std::string > ids = buckets->getIDs();
		std::string config;

		for (uint32 i = 0; i < ids.size(); i++) {
			ibmras::monitoring::agent::Bucket* bucket = buckets->findBucket(
					ids[i]);

			ibmras::monitoring::agent::DataSource<pullsource> *source =
					agent->getPullSource(bucket->getUniqueID());

			if (source == NULL) {
				ibmras::monitoring::agent::DataSource<pushsource> *pushSource =
						agent->getPushSource(bucket->getUniqueID());
				if (pushSource != NULL) {
					config = pushSource->getConfig();
				}
			} else {
				config = source->getConfig();
			}

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
		ibmras::monitoring::agent::Agent* agent =
				ibmras::monitoring::agent::Agent::getInstance();
		agent->republish(topic);
	}
}

}
}
} /* end namespace agent */

void* ibmras_getSystemReceiver() {
	return new ibmras::monitoring::agent::SystemReceiver();
}

