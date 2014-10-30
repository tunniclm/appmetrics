 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include <stdio.h>
#include "ibmras/common/logging.h"
#include "ibmras/common/port/ThreadData.h"
#include "ibmras/monitoring/Plugin.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/monitoring/plugins/omr/TraceDataProvider.h"
#include "ibmras/monitoring/plugins/omr/MemoryCountersDataProvider.h"
#include "ibmras/monitoring/plugins/omr/CpuDataProvider.h"
#include "ibmras/monitoring/plugins/omr/NativeMemoryDataProvider.h"
#include "ibmras/monitoring/plugins/omr/MethodLookupProvider.h"
#include "ibmras/vm/omr/healthcenter.h"
#include "ibmras/common/PropertiesFile.h"

#if defined(WINDOWS)
const char PATHSEPARATOR = '\\';
#else
const char PATHSEPARATOR = '/';
#endif

int headerSize = 0;

void launchAgent(char const *options);

void killAgent();

IBMRAS_DEFINE_LOGGER("omr");

ibmras::monitoring::agent::Agent* agent;

omrRunTimeProviderParameters omrParams;

omr_error_t OMRAgent_OnLoad(OMR_TI const *ti, OMR_VM *vm, char const *options) {
	omr_error_t rc = OMR_ERROR_NONE;

	IBMRAS_DEBUG(debug,  "OMRAgent_OnLoad enter");

	omrParams.theVm = vm;
	omrParams.omrti = ti;
	launchAgent(options);

	IBMRAS_DEBUG(debug,  "OMRAgent_OnLoad exit");

	return rc;
}

omr_error_t OMRAgent_OnUnload(OMR_TI const *ti, OMR_VM *vm) {
	IBMRAS_DEBUG(debug,  "OMRAgent_OnUnload enter");

	PORT_ACCESS_FROM_OMRVM(vm);

	killAgent();

}

void launchAgent(char const *options) {

	agent = ibmras::monitoring::agent::Agent::getInstance();

	IBMRAS_DEBUG(debug, "launchAgent enter");

	// Add MQTT Connector plugin
	// TODO load SSL or plain
	std::string agentLibPath =
			ibmras::common::util::LibraryUtils::getLibraryDir(
					"healthcenter.dll", (void*) launchAgent);
	agent->addPlugin(agentLibPath + PATHSEPARATOR + "plugins", "hcmqtt");

	if (options == NULL) {
		IBMRAS_LOG(warning, "No properties file specified");
	} else {
		ibmras::common::PropertiesFile props;
		if (props.load(options)) {
			agent->setProperties(props);
		} else {
			IBMRAS_LOG_1(warning, "Properties could not be loaded from %s", options);
		}
	}


	agent->setLogLevels();

	IBMRAS_LOG_1(info, "Health Center %s", agent->getVersion().c_str());

	if (omrParams.omrti == NULL) {
		IBMRAS_DEBUG(warning,  "omrParams.omrti is null");
	}

	agent->addPlugin((ibmras::monitoring::Plugin*)plugins::omr::trace::TraceDataProvider::getInstance(omrParams));
	agent->addPlugin(
			(ibmras::monitoring::Plugin*)plugins::omr::memorycounters::MemoryCountersDataProvider::getInstance(omrParams));
	agent->addPlugin(
			(ibmras::monitoring::Plugin*)plugins::omr::cpu::CpuDataProvider::getInstance(omrParams));
	agent->addPlugin((ibmras::monitoring::Plugin*)plugins::omr::nativememory::NativeMemoryDataProvider::getInstance(omrParams));
	agent->addPlugin((ibmras::monitoring::Plugin*)plugins::omr::methods::MethodLookupProvider::getInstance(omrParams));


	agent->init();
	agent->start();
	IBMRAS_DEBUG(debug, "launchAgent exit");

}

void killAgent() {

	agent = ibmras::monitoring::agent::Agent::getInstance();
	agent->stop();
	agent->shutdown();

}

