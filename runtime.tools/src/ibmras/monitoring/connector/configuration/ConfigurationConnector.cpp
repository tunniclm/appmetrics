/*
 * ConfigurationConnector.cpp
 *
 *  Created on: 7 Oct 2014
 *      Author: robbins
 */

#include "ibmras/monitoring/connector/configuration/ConfigurationConnector.h"
#include "ibmras/common/util/strUtils.h"

namespace ibmras {
namespace monitoring {
namespace connector {

static const std::string CONFIGURATION_SUFFIX("Configuration");

ConfigurationConnector::ConfigurationConnector() {
}

ConfigurationConnector::~ConfigurationConnector() {
}

std::string ConfigurationConnector::getConfig(const std::string& name) {
	return config.get(name + CONFIGURATION_SUFFIX);
}

int ConfigurationConnector::sendMessage(const std::string& sourceId,
		uint32 size, void* data) {
	if (ibmras::common::util::endsWith(sourceId, CONFIGURATION_SUFFIX)) {
		std::string configString((char*)data, size);
		config.put(sourceId, configString);
	}
	return 0;
}

} /* namespace connector */
} /* namespace monitoring */
} /* namespace ibmras */
