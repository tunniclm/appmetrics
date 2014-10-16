/*
 * ConnectorManager.h
 *
 *  Created on: 27 Mar 2014
 *      Author: robbins
 */

#ifndef ibmras_monitoring_connector_connectormanager_h
#define ibmras_monitoring_connector_connectormanager_h

#include "ibmras/monitoring/connector/Connector.h"
#include "ibmras/monitoring/connector/Receiver.h"
#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/port/Lock.h"
#include <set>
#include <queue>

namespace ibmras{
namespace monitoring {
namespace connector {

class DECL ConnectorManager: public Connector, public Receiver {
public:
	ConnectorManager();
	virtual ~ConnectorManager();

	void addConnector(Connector *connector);
	void removeConnector(Connector *connector);
	void removeAllConnectors();
	Connector* getConnector(const std::string &id);
	
	void addReceiver(Receiver *receiver);
	void removeReceiver(Receiver *receiver);
	void removeAllReceivers();

	int sendMessage(const std::string &sourceId, uint32 size, void *data);

	virtual void receiveMessage(const std::string &id, uint32 size, void *data);

	int start();
	int stop();

private:
	bool running;
	ibmras::common::port::ThreadData threadData;

	class ReceivedMessage {
	public:
		ReceivedMessage(const std::string &id, uint32 size, void *data);
		virtual ~ReceivedMessage() {}

		const std::string& getId() const {
			return id;
		}

		const std::string& getMessage() const {
			return message;
		}

	private:
		std::string id;
		std::string message;
	};

	std::queue<ReceivedMessage> receiveQueue;
	ibmras::common::port::Lock receiveLock;
	ibmras::common::port::Lock sendLock;

	std::set<Connector*> connectors;
	std::set<Receiver*> receivers;

	void processReceivedMessages();
	static void* processThread(ibmras::common::port::ThreadData *td);
};
}
}
} /* namespace connector */
#endif /* ibmras_monitoring_connector_connectormanager_h */
