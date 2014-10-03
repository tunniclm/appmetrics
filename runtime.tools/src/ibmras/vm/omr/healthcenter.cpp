/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2014, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/
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

IBMRAS_DEFINE_LOGGER("OMR VM");

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

	agent->setLogLevels();
	IBMRAS_DEBUG(debug, "launchAgent enter");

	// Add MQTT Connector plugin
	// TODO load SSL or plain
	std::string agentLibPath =
			ibmras::common::util::LibraryUtils::getLibraryDir(
					"healthcenter.dll", (void*) launchAgent);
	agent->addPlugin(agentLibPath + PATHSEPARATOR + "plugins", "hcmqtt");



	ibmras::common::PropertiesFile props;
	props.load(options);
	agent->setProperties(props);
	if (omrParams.omrti == NULL) {
		IBMRAS_DEBUG(warning,  "omrParams.omrti is null");
	}

	agent->addPlugin(
			(ibmras::monitoring::Plugin*) new plugins::omr::trace::TraceDataProvider(omrParams));
	agent->addPlugin(
			(ibmras::monitoring::Plugin*) new plugins::omr::memorycounters::MemoryCountersDataProvider(omrParams));
	agent->addPlugin(
			(ibmras::monitoring::Plugin*) new plugins::omr::cpu::CpuDataProvider(omrParams));
	agent->addPlugin((ibmras::monitoring::Plugin*) new plugins::omr::nativememory::NativeMemoryDataProvider(omrParams));

	agent->init();
	agent->start();
	IBMRAS_DEBUG(debug, "launchAgent exit");

}

void killAgent() {

	agent = ibmras::monitoring::agent::Agent::getInstance();
	agent->stop();
	agent->shutdown();

}

