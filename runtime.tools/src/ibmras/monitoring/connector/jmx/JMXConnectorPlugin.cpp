/*
 * JMXConnectorPlugin.cpp
 *
 *  Created on: 4 Jul 2014
 *      Author: robbins
 */

#include "ibmras/monitoring/connector/jmx/JMXConnectorPlugin.h"

namespace ibmras {
namespace monitoring {
namespace connector {
namespace jmx {


JMXConnectorPlugin* instance = NULL;
JMXConnectorPlugin* JMXConnectorPlugin::getInstance(JavaVM *theVM, const std::string &options) {
	if (!instance) {
		if (theVM) {
			instance = new JMXConnectorPlugin(theVM, options);
		}
	}
	return instance;
}

int JMXConnectorPlugin::startPlugin() {
	return 0;
}

int JMXConnectorPlugin::stopPlugin() {
	return 0;
}

void* JMXConnectorPlugin::getConnector(const char* properties) {
	if (instance == NULL) {
		return NULL;
	}
	return instance->getJMXConnector(properties);
}

JMXConnector* JMXConnectorPlugin::getJMXConnector(const char* properties) {
	if (!jmxConnector) {
		jmxConnector = new JMXConnector(vm, agentOptions, properties);
	}
	return jmxConnector;
}

JMXConnectorPlugin::JMXConnectorPlugin(JavaVM *theVM,  const std::string &options) : jmxConnector(NULL), vm(theVM), agentOptions(options) {
	name = "JMX Connector";
	type = ibmras::monitoring::plugin::connector;
	start = startPlugin;
	stop = stopPlugin;
	confactory = getConnector;
	pull = NULL;
	push = NULL;
	handle = NULL;
	recvfactory = NULL;
}


JMXConnectorPlugin::~JMXConnectorPlugin() {
	if (jmxConnector) {
		delete jmxConnector;
	}
}


} /* namespace jmx */
} /* namespace connector */
} /* namespace monitoring */
} /* namespace ibmras */
