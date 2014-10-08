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

static const std::string CONFIGURATION_SUFFIX("Configuration");

ConfigurationConnector::ConfigurationConnector() {
}

ConfigurationConnector::~ConfigurationConnector() {
}

std::string ConfigurationConnector::getConfig(const std::string& name) {
	return config.get(name);
}

int ConfigurationConnector::sendMessage(const std::string& sourceId,
		uint32 size, void* data) {

	if (ibmras::common::util::endsWith(sourceId, CONFIGURATION_SUFFIX)) {
		std::string configName = sourceId;
		if (ibmras::common::util::equalsIgnoreCase(sourceId, "ClassHistogramSourceConfiguration")) {
			configName = "classhistogram";
		} else if (ibmras::common::util::equalsIgnoreCase(sourceId,"cpusourceconfiguration")) {
			configName = "cpu";
		} else if (ibmras::common::util::equalsIgnoreCase(sourceId,"environmentsourceconfiguration")) {
			configName = "environment";
		} else if (ibmras::common::util::equalsIgnoreCase(sourceId,"jlasourceconfiguration")) {
			configName = "locking";
		} else if (ibmras::common::util::equalsIgnoreCase(sourceId,"memorysourceconfiguration")) {
			configName = "memory";
		} else if (ibmras::common::util::equalsIgnoreCase(sourceId,"memorycounterssourceconfiguration")) {
			configName = "memorycounters";
		} else if (ibmras::common::util::equalsIgnoreCase(sourceId,"methoddictionarysourceconfiguration")) {
			configName = "methoddictionary";
		} else if (ibmras::common::util::equalsIgnoreCase(sourceId,"threadssourceconfiguration")) {
			configName = "threads";
		} else if (ibmras::common::util::equalsIgnoreCase(sourceId,"tracesubscribersourceconfiguration")) {
			configName = "trace";
		}

		std::string configString((char*)data, size);
		config.put(configName, configString);
	}
	return 0;
}

} /* namespace connector */
} /* namespace monitoring */
} /* namespace ibmras */
