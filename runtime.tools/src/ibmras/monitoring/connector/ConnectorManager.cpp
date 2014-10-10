/*
 * ConnectorManager.cpp
 *
 *  Created on: 27 Mar 2014
 *      Author: robbins
 */

#include "ibmras/monitoring/connector/ConnectorManager.h"
#include "ibmras/common/logging.h"

namespace ibmras {
namespace monitoring {
namespace connector {

ConnectorManager::ConnectorManager() {

}

ConnectorManager::~ConnectorManager() {
	removeAllConnectors();
}

void ConnectorManager::addConnector(Connector *connector) {
	connectors.insert(connector);
}

void ConnectorManager::removeConnector(Connector *connector) {
	connectors.erase(connector);
}

void ConnectorManager::removeAllConnectors() {
	connectors.clear();
}

void ConnectorManager::addReceiver(Receiver *receiver) {
	receivers.insert(receiver);
 }

void ConnectorManager::removeReceiver(Receiver *receiver) {
	receivers.erase(receiver);
}

void ConnectorManager::removeAllReceivers() {
	receivers.clear();
}

void ConnectorManager::receiveMessage(const std::string &id, uint32 size,
		void *data) {
	for (std::set<Receiver*>::iterator it = receivers.begin();
			it != receivers.end(); ++it) {
		if (*it) {
			(*it)->receiveMessage(id, size, data);
		}
	}
}

int ConnectorManager::sendMessage(const std::string &sourceId, uint32 size,
		void *data) {
	int count = 0;
	try {
		sendLock.acquire();

		for (std::set<Connector*>::iterator it = connectors.begin();
				it != connectors.end(); ++it) {
			if ((*it)->sendMessage(sourceId, size, data) > 0) {
				count++;
			}
		}
	} catch (...) {
	}
	sendLock.release();
	return count;
}

Connector* ConnectorManager::getConnector(const std::string &id) {

	for (std::set<Connector*>::iterator it = connectors.begin();
			it != connectors.end(); ++it) {
		if (!(*it)->getID().compare(id)) {
			return *it;
		}
	}
	return NULL;
}

int ConnectorManager::start() {
	int rc = 0;
	for (std::set<Connector*>::iterator it = connectors.begin();
			it != connectors.end(); ++it) {
		rc += (*it)->start();
	}
	return rc;
}

int ConnectorManager::stop() {
	int rc = 0;
	for (std::set<Connector*>::iterator it = connectors.begin();
			it != connectors.end(); ++it) {
		rc += (*it)->stop();
	}
	return rc;
}
}
}
} /* namespace monitoring */
