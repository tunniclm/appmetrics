/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include "ibmras/monitoring/connector/ConnectorManager.h"
#include "ibmras/common/logging.h"

namespace ibmras {
namespace monitoring {
namespace connector {

ConnectorManager::ConnectorManager() :
		running(false), threadData(processThread) {
	threadData.setArgs(this);
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
	if (running && !receiveLock.acquire() && !receiveLock.isDestroyed()) {
		ReceivedMessage msg(id, size, data);
		receiveQueue.push(msg);
		receiveLock.release();
	}
}

void ConnectorManager::processMessage(const std::string &id, uint32 size,
		void *data) {
	ReceivedMessage msg(id, size, data);
	if (!receiveLock.acquire() && !receiveLock.isDestroyed()) {
		processReceivedMessage(msg);
		receiveLock.release();
	}
}

void ConnectorManager::processReceivedMessage(const ReceivedMessage& msg) {
	for (std::set<Receiver*>::iterator it = receivers.begin();
			it != receivers.end(); ++it) {
		if (*it) {
			(*it)->receiveMessage(msg.getId(),
					msg.getMessage().size(),
					(void*) msg.getMessage().c_str());
		}
	}
}

void* ConnectorManager::processThread(ibmras::common::port::ThreadData *td) {
	ConnectorManager* conMan = (ConnectorManager*) td->getArgs();
	if (conMan) {
		conMan->processReceivedMessages();
	}

	return NULL;
}

void ConnectorManager::processReceivedMessages() {
	while (running) {

		if (!receiveLock.acquire() && !receiveLock.isDestroyed()) {
			while (!receiveQueue.empty()) {
				ReceivedMessage msg = receiveQueue.front();
				receiveQueue.pop();
				processReceivedMessage(msg);
			}
			receiveLock.release();
		}
		ibmras::common::port::sleep(1);
	}
}

int ConnectorManager::sendMessage(const std::string &sourceId, uint32 size,
		void *data) {
	int count = 0;
	if (running && !sendLock.acquire()) {
		try {

			for (std::set<Connector*>::iterator it = connectors.begin();
					it != connectors.end(); ++it) {
				if ((*it)->sendMessage(sourceId, size, data) > 0) {
					count++;
				}
			}

		} catch (...) {
		}
		sendLock.release();
	}

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
	if (running) {
		return 0;
	}

	running = true;
	ibmras::common::port::createThread(&threadData);

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
	running = false;
	return rc;
}

ConnectorManager::ReceivedMessage::ReceivedMessage(const std::string& id,
		uint32 size, void* data) {
	this->id = id;
	if (size > 0 && data != NULL) {
		message = std::string((const char*) data, size);
	} else {
		data = NULL;
	}
}

}
}
} /* namespace monitoring */

