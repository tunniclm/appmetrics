/*
 * MQTTConnector.h
 *
 *  Created on: 27 Feb 2014
 *      Author: robbins
 */

#ifndef ibmras_monitoring_connector_mqtt_mqttconnector_h
#define ibmras_monitoring_connector_mqtt_mqttconnector_h

#include "ibmras/monitoring/connector/Connector.h"

extern "C" {
#include "MQTTAsync.h"
}

namespace ibmras {
namespace monitoring {
namespace connector {
namespace mqttcon {

class MQTTConnector: public ibmras::monitoring::connector::Connector {
public:

	MQTTConnector(const std::string &host, const std::string &port,
			const std::string &user, const std::string &pass,
			const std::string &topicNamespace, const std::string &applicationId);

	int sendMessage(const std::string &sourceId, uint32 size, void *data);

	void registerReceiver(ibmras::monitoring::connector::Receiver *receiver);
	ibmras::monitoring::connector::Receiver* returnReceiver();

	int start();
	int stop();

	virtual ~MQTTConnector();
	std::string getID() {
		return "MQTTConnector";
	}
private:
	void createClient(const std::string &id);
	int connect();

	int handleReceivedmessage(char *topicName, int topicLen,
			MQTTAsync_message *message);
	static int messageReceived(void *context, char *topicName, int topicLen,
			MQTTAsync_message *message);

	void handleOnConnect(MQTTAsync_successData* response);
	static void onConnect(void* context, MQTTAsync_successData* response);
	static void onFailure(void* context, MQTTAsync_failureData* response);

	void sendIdentityMessage();

	std::string brokerHost;
	std::string brokerPort;
	std::string brokerUser;
	std::string brokerPass;

	MQTTAsync mqttClient;
	ibmras::monitoring::connector::Receiver *receiver;

	std::string rootTopic;
	std::string agentTopic;
	std::string agentIdMessage;

	std::string willTopic;
	std::string willMessage;
};

} /* end namespace mqttcon */
}
}
}
#endif /* ibmras_monitoring_connector_mqtt_mqttconnector_h */
