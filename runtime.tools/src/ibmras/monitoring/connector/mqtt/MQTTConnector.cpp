/*
 * MQTTConnector.cpp
 *
 *  Created on: 27 Feb 2014
 *      Author: robbins
 */
#if defined(_ZOS)
#define  _XOPEN_SOURCE_EXTENDED 1
#endif

#include "ibmras/monitoring/connector/mqtt/MQTTConnector.h"
#include "ibmras/common/logging.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/common/PropertiesFile.h"
#include "ibmras/common/util/sysUtils.h"

#define AGENT_TOPIC_PREFIX "ibm/healthcenter"
#define CLIENT_IDENTIFY_TOPIC AGENT_TOPIC_PREFIX  "/identify"
#define CLIENT_IDENTITY_TOPIC AGENT_TOPIC_PREFIX  "/id"
#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT "1883"

#if defined(_WINDOWS)
#define MQTT_DECL __declspec(dllexport)	/* required for DLLs to export the plugin functions */
#else
#define MQTT_DECL
#endif

namespace ibmras {
namespace monitoring {
namespace connector {
namespace mqttcon {

IBMRAS_DEFINE_LOGGER("MQTTCon")
;

MQTTConnector::MQTTConnector(const std::string &host, const std::string &port,
		const std::string &user, const std::string &pass,
		const std::string &topicNamespace, const std::string &applicationId) :
		brokerHost(host), brokerPort(port), brokerUser(user), brokerPass(pass), mqttClient(
				NULL) {

	int processId = ibmras::common::port::getProcessId();
	unsigned long long time = ibmras::common::util::getMilliseconds();
	srand((unsigned int) time);

	std::stringstream clientIdStream;
	clientIdStream << "agent_" << rand();
	std::string clientId = clientIdStream.str();

	std::string namespacePrefix = topicNamespace;
	if (topicNamespace.length() > 0
			&& topicNamespace[topicNamespace.length() - 1] != '/') {
		namespacePrefix += '/';
	}

	std::stringstream rootTopicStream;
	rootTopicStream << namespacePrefix << AGENT_TOPIC_PREFIX << "/" << clientId;
	rootTopic = rootTopicStream.str();

	std::stringstream agentTopicStream;
	agentTopicStream << namespacePrefix << AGENT_TOPIC_PREFIX << "/agent/"
			<< clientId << "/";
	agentTopic = agentTopicStream.str();

	std::stringstream agentIdMessageStream;
	std::string applicationIdentifier = applicationId;
	if (applicationId.length() == 0) {
		applicationIdentifier = ibmras::common::port::getHostName() + ":";
		applicationIdentifier += ibmras::common::itoa(processId);
	}
	agentIdMessageStream << rootTopic << "\n" << applicationIdentifier;
	agentIdMessage = agentIdMessageStream.str();

	willTopic = rootTopic + "/will";
	willMessage = agentIdMessage;
	createClient(clientId);

	IBMRAS_DEBUG_1(fine, "MQTTConnector: creating client: %s", clientId.c_str());
}

MQTTConnector::~MQTTConnector() {
	if (mqttClient != NULL) {
		MQTTAsync_disconnect(mqttClient, NULL);
		MQTTAsync_destroy(&mqttClient);
	}
}

void MQTTConnector::createClient(const std::string &clientId) {
	if (mqttClient == NULL) {
		std::string address("tcp://");
		address += brokerHost;
		address += ":";
		address += brokerPort;

		char *addStr = new char[address.length() + 1];
		strcpy(addStr, address.c_str());

		int rc = MQTTAsync_create(&mqttClient, addStr, clientId.c_str(),
				MQTTCLIENT_PERSISTENCE_NONE, NULL);

		if (addStr != NULL) {
			delete[] addStr;
		}

		if (rc != MQTTASYNC_SUCCESS) {
			IBMRAS_DEBUG_1(fine, "MQTTConnector: client create failed: %d", rc);
		} else {
			rc = MQTTAsync_setCallbacks(mqttClient, this, NULL, messageReceived,
					NULL);
			if (rc != MQTTASYNC_SUCCESS) {
				IBMRAS_DEBUG_1(fine, "MQTTConnector: setCallbacks failed: %d", rc);
			}
		}

	}
}

int MQTTConnector::connect() {
	IBMRAS_DEBUG(fine, "MQTTConnector: connecting");
	int rc = MQTTASYNC_FAILURE;
	if (mqttClient != NULL) {
		if (MQTTAsync_isConnected(mqttClient)) {
			return MQTTASYNC_SUCCESS;
		}

		MQTTAsync_connectOptions connOpts = MQTTAsync_connectOptions_initializer;
		MQTTAsync_willOptions willOpts = MQTTAsync_willOptions_initializer;
		willOpts.message = willMessage.c_str();
		willOpts.topicName = willTopic.c_str();

		connOpts.cleansession = 1;
		connOpts.keepAliveInterval = 20;
		connOpts.onSuccess = onConnect;
		connOpts.onFailure = onFailure;
		connOpts.context = this;
		connOpts.will = &willOpts;

		if (brokerUser != "") {
			connOpts.username = strdup(brokerUser.c_str());
		}
		if (brokerPass != "") {
			connOpts.password = strdup(brokerPass.c_str());
		}

		rc = MQTTAsync_connect(mqttClient, &connOpts);
		if (rc != MQTTASYNC_SUCCESS) {
			IBMRAS_DEBUG_1(warning, "MQTTAsync_connect failed. rc=%d", rc);
		}
	}
	return rc;
}

void MQTTConnector::onConnect(void* context, MQTTAsync_successData* response) {
	((MQTTConnector*) context)->handleOnConnect(response);
}

void MQTTConnector::onFailure(void* context, MQTTAsync_failureData* response) {
	IBMRAS_DEBUG_2(warning, "MQTTAsync_connect failed. rc: %d reason: %s", response->code, response->message);
}


void MQTTConnector::handleOnConnect(MQTTAsync_successData* response) {
	char *topic = new char[agentTopic.length() + 2];
	sprintf(topic, "%s#", agentTopic.c_str());
	IBMRAS_DEBUG_1(debug, "MQTTAsync_subscribe to %s", topic);
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	opts.context = this;
	int rc = MQTTAsync_subscribe(mqttClient, topic, 1, &opts);
	if (rc != MQTTASYNC_SUCCESS) {
		IBMRAS_DEBUG_2(warning, "MQTTAsync_subscribe to %s failed. rc=%d", topic, rc);
	}
	delete[] topic;

	char identifyTopic[] = CLIENT_IDENTIFY_TOPIC;
	IBMRAS_DEBUG_1(debug, "MQTTAsync_subscribe to %s", identifyTopic);
	rc = MQTTAsync_subscribe(mqttClient, identifyTopic, 1, &opts);
	if (rc != MQTTASYNC_SUCCESS) {
		IBMRAS_DEBUG_2(warning, "MQTTAsync_subscribe to %s failed. rc=%d", CLIENT_IDENTIFY_TOPIC, rc);
	}

	sendIdentityMessage();
}

int MQTTConnector::sendMessage(const std::string &sourceId, uint32 size,
		void *data) {
	IBMRAS_DEBUG_3(fine, "Sending message : topic %s : data %p : length %d", sourceId.c_str(), data, size);

	if (mqttClient == NULL || !MQTTAsync_isConnected(mqttClient)) {
		return -1;
	}

	/* topic = <clientId>/sourceId */
	char *topic = new char[rootTopic.length() + 1 + sourceId.length() + 1];
	sprintf(topic, "%s/%s", rootTopic.c_str(), sourceId.c_str());

	//	MQTTAsync_deliveryToken token;
	MQTTAsync_send(mqttClient, topic, size, data, 1, 0, NULL);

	delete[] topic;

	return size;
}

int MQTTConnector::messageReceived(void *context, char *topicName, int topicLen,
		MQTTAsync_message *message) {
	return ((MQTTConnector*) context)->handleReceivedmessage(topicName,
			topicLen, message);
}

int MQTTConnector::handleReceivedmessage(char *topicName, int topicLen,
		MQTTAsync_message *message) {

	IBMRAS_DEBUG_1(debug, "MQTT message received for %s", topicName);
	std::string topic(topicName);

	if (topic == CLIENT_IDENTIFY_TOPIC) {
		sendIdentityMessage();
	}
	if (receiver != NULL) {
		if (topic.find(agentTopic) == 0) {
			topic = topic.substr(agentTopic.length());
			IBMRAS_DEBUG_1(debug, "forwarding message %s", topic.c_str());
		}
		receiver->receiveMessage(topic, message->payloadlen, message->payload);
	}

	MQTTAsync_freeMessage(&message);
	MQTTAsync_free(topicName);
	return true;
}

void MQTTConnector::registerReceiver(
		ibmras::monitoring::connector::Receiver *receiver) {
	IBMRAS_DEBUG(debug, "registerReceiver");
	this->receiver = receiver;
}

ibmras::monitoring::connector::Receiver* MQTTConnector::returnReceiver() {
	return receiver;
}

int MQTTConnector::start() {
	IBMRAS_DEBUG(debug, "start");
	return connect();
}

int MQTTConnector::stop() {
	IBMRAS_DEBUG(debug, "stop");

	// Send will message before our clean termination
	char* message = new char[willMessage.length() + 1];
	strcpy(message, willMessage.c_str());
	MQTTAsync_send(mqttClient, willTopic.c_str(), strlen(message), message, 1, 0, NULL);
	delete[] message;

	if (mqttClient != NULL) {
		return MQTTAsync_disconnect(mqttClient, NULL);
	}
	return -1;
}

void MQTTConnector::sendIdentityMessage() {
	IBMRAS_DEBUG_1(debug, "sending identity message: %s", agentIdMessage.c_str());
	char topic[] = CLIENT_IDENTITY_TOPIC;
	char* idMessage = new char[agentIdMessage.length() + 1];
	strcpy(idMessage, agentIdMessage.c_str());
	MQTTAsync_send(mqttClient, topic, strlen(idMessage), idMessage, 1, 0, NULL);
	delete [] idMessage;
}

}
}
}
} /* end mqttcon monitoring */

extern "C" {

#if defined(WINDOWS)
#else
void MQTTAsync_init();
#endif

MQTT_DECL int ibmras_monitoring_plugin_start() {
	return 0;
}

MQTT_DECL int ibmras_monitoring_plugin_stop() {
	return 0;
}

bool mqttInitialized = false;

MQTT_DECL void* ibmras_monitoring_getConnector(const char* properties) {

	if (!mqttInitialized) {
#if defined(WINDOWS)
#else
		MQTTAsync_init();
#endif
		mqttInitialized = true;
	}

	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();

	std::string brokerHost = agent->getAgentProperty("mqtt.broker.host");
	if (!brokerHost.compare("")) {
		brokerHost = DEFAULT_HOST;
	}

	std::string brokerPort = agent->getAgentProperty("mqtt.broker.port");
	if (!brokerPort.compare("")) {
		brokerPort = DEFAULT_PORT;
	}
	std::string brokerUser = agent->getAgentProperty("mqtt.broker.user");
	std::string brokerPass = agent->getAgentProperty("mqtt.broker.pass");
	std::string topcNamespace = agent->getAgentProperty("mqtt.topic.namespace");
	std::string applicationId = agent->getAgentProperty("mqtt.application.id");

	return new ibmras::monitoring::connector::mqttcon::MQTTConnector(brokerHost,
			brokerPort, brokerUser, brokerPass, topcNamespace, applicationId);
}
}