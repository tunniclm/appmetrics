 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#include "ibmras/monitoring/connector/configuration/ConfigurationConnector.h"
#include "ibmras/common/util/strUtils.h"
#include <iostream>

namespace ibmras {
namespace monitoring {
namespace connector {

static const std::string CONFIGURATION_TOPIC("configuration/");

ConfigurationConnector::ConfigurationConnector() {
}

ConfigurationConnector::~ConfigurationConnector() {
}

std::string ConfigurationConnector::getConfig(const std::string& name) {
	return config.get(name);
}

int ConfigurationConnector::sendMessage(const std::string& sourceId,
		uint32 size, void* data) {

	if (ibmras::common::util::startsWith(sourceId, CONFIGURATION_TOPIC)) {
		std::string configName = sourceId.substr(CONFIGURATION_TOPIC.length());

		std::string configString((char*)data, size);
		config.put(configName, configString);
	}
	return 0;
}

} /* namespace connector */
} /* namespace monitoring */
} /* namespace ibmras */
