 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#include "ibmras/monitoring/plugins/j9/Util.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/common/logging.h"
#include "ibmras/common/util/strUtils.h"

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {

IBMRAS_DEFINE_LOGGER("J9Utils");

int Util::getJavaLevel() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	std::string level = agent->getAgentProperty("capability.JavaLevel");
	return atoi(level.c_str());
}

int Util::getServiceRefreshNumber() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	std::string level = agent->getAgentProperty(
			"capability.ServiceRefreshNumber");
	return atoi(level.c_str());
}

bool Util::is26VMOrLater() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	std::string level = agent->getAgentProperty("capability.is26VMOrLater");
	return (level == "true");
}

bool Util::is27VMOrLater() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	std::string level = agent->getAgentProperty("capability.is27VMOrLater");
	return (level == "true");
}

bool Util::vmHasLOATracePoints() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	std::string level = agent->getAgentProperty(
			"capability.vmHasLOATracePoints");
	return (level == "true");
}

bool Util::j9DmpTrcAvailable() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	std::string level = agent->getAgentProperty("capability.j9DmpTrcAvailable");
	return (level == "true");
}

bool Util::isRealTimeVM() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	std::string level = agent->getAgentProperty("capability.isRealTimeVM");
	return (level == "true");
}

std::string Util::getLowAllocationThreshold() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	return agent->getAgentProperty("allocation.threshold.low");
}

std::string Util::getHighAllocationThreshold() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	return agent->getAgentProperty("allocation.threshold.high");
}

/**
 Returns VM Dump options
 @param jvmtiQueryVmDump Function pointer to the jvmti function naming the parameter
 @param pti Pointer to jvmti environment.
 @return A char* which contains the options OR an empty string if there is an error.
 */
std::string Util::queryVmDump(jvmtiExtensionFunction jvmtiQueryVmDump,
		jvmtiEnv* pti) {
	IBMRAS_DEBUG(debug, "> queryVmDump");
	jvmtiError rc;
	char *buffer = NULL;
	jint buffer_size;
	std::string options;

	if (jvmtiQueryVmDump == 0) {
		IBMRAS_DEBUG(debug, "No jvmtiQueryVmDump extension function");
		return "";
	}

	buffer_size = 1024;
	rc = pti->Allocate(buffer_size, (unsigned char**) &buffer);
	if (buffer == NULL) {
		IBMRAS_DEBUG(debug, "< queryVmDump failed to allocate buffer");
		return "";
	}

	/* call jvmti function */
	rc = (jvmtiQueryVmDump)(pti, buffer_size, buffer, &buffer_size);
	if (rc == JVMTI_ERROR_ILLEGAL_ARGUMENT) {
		IBMRAS_DEBUG(debug, "Buffer smaller than expected");
		/* allocate buffer of correct size */
		pti->Deallocate((unsigned char*) buffer);
		pti->Allocate(buffer_size + 100, (unsigned char**) &buffer);
		if (buffer == NULL) {
			IBMRAS_DEBUG(debug, "< queryVmDump failed to reallocate buffer");
			return "";
		}
		rc = (jvmtiQueryVmDump)(pti, buffer_size, buffer, &buffer_size);
	}

	if (rc == JVMTI_ERROR_NONE) {
		ibmras::common::util::force2Native(buffer);
		options = buffer;

	} else {
		IBMRAS_DEBUG(debug, "RC_ERROR_2");
	}

	pti->Deallocate((unsigned char*) buffer);

	IBMRAS_DEBUG(debug, "< queryVmDump");
	return options;
}

} /* namespace j9 */
} /* namespace plugins */
} /* namespace monitoring */
} /* namespace ibmras */

