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
JMXConnectorPlugin* JMXConnectorPlugin::getInstance(JavaVM *theVM) {
	if (!instance) {
		if (theVM) {
			instance = new JMXConnectorPlugin(theVM);
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
	return instance->getJMXConnector();
}

JMXConnector* JMXConnectorPlugin::getJMXConnector() {
	if (!jmxConnector) {
		jmxConnector = new JMXConnector(vm);
	}
	return jmxConnector;
}

JMXConnectorPlugin::JMXConnectorPlugin(JavaVM *theVM) : jmxConnector(NULL), vm(theVM) {
	name = "JMX Connector";
	type = ibmras::monitoring::plugin::connector;
	start = startPlugin;
	stop = stopPlugin;
	confactory = getConnector;
	pull = NULL;
	push = NULL;
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
