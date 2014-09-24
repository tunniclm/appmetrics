/*
 * HLConnectorPlugin.cpp
 *
 *  Created on: 21 Aug 2014
 *      Author: Admin
 */

#include "ibmras/monitoring/connector/headless/HLConnectorPlugin.h"
//#include <cstdio>
#include <cstdlib>

namespace ibmras {
namespace monitoring {
namespace connector {
namespace headless {

HLConnectorPlugin* instance = NULL;
static HLConnector* hlConnector = NULL;

HLConnectorPlugin::HLConnectorPlugin(JavaVM* theVM, const std::string &options) : agentOptions(options), vm(theVM) {
	name = "Headless Connector";
	type = ibmras::monitoring::plugin::connector;
	start = NULL;
	stop = NULL;
	confactory = getConnector;
	pull = NULL;
	push = NULL;
	handle = NULL;
	recvfactory = NULL;
}

HLConnectorPlugin::~HLConnectorPlugin(){}

HLConnectorPlugin* HLConnectorPlugin::getInstance(JavaVM* theVM, const std::string &options) {
	if(!instance) {
		instance = new HLConnectorPlugin(theVM, options);
	}

	return instance;
}


void* HLConnectorPlugin::getConnector(const char* properties) {
	if (instance == NULL) {
		return NULL;
	}
	return instance->getHLConnector(properties);
}

HLConnector* HLConnectorPlugin::getHLConnector(const char* properties) {
	if(!hlConnector) {
		//hlConnector = new HLConnector(agentOptions, properties);
		hlConnector = HLConnector::getInstance(vm, agentOptions, properties);
	}

	return hlConnector;
}




} /*end namespace headless*/
} /*end namespace connector*/
} /*end namespace monitoring*/
} /*end namespace ibmras*/


