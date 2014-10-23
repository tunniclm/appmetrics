/*
 * ConfigurationConnector.cpp
 *
 *  Created on: 7 Oct 2014
 *      Author: robbins
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
