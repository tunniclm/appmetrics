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

HLConnectorPlugin::HLConnectorPlugin(JavaVM* theVM) : vm(theVM) {
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

HLConnectorPlugin* HLConnectorPlugin::getInstance(JavaVM* theVM) {
	if(!instance) {
		instance = new HLConnectorPlugin(theVM);
	}
	return instance;
}


void* HLConnectorPlugin::getConnector(const char* properties) {
	if (instance == NULL) {
		return NULL;
	}
	return instance->getHLConnector();
}

HLConnector* HLConnectorPlugin::getHLConnector() {
	if(!hlConnector) {
		hlConnector = HLConnector::getInstance(vm);
	}

	return hlConnector;
}




} /*end namespace headless*/
} /*end namespace connector*/
} /*end namespace monitoring*/
} /*end namespace ibmras*/


