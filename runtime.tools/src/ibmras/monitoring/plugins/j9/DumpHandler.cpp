#include "ibmras/monitoring/Plugin.h"
#include "ibmras/vm/java/healthcenter.h"
#include "ibmras/monitoring/plugins/j9/DumpHandler.h"
#include "ibmjvmti.h"
#include "jni.h"
#include "jvmti.h"
#include "ibmras/common/util/strUtils.h"
#include "ibmras/common/logging.h"

#include "ibmras/monitoring/agent/Agent.h"


#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include<stdlib.h>

#define JNI_VERSION JNI_VERSION_1_4

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {

uint32 DumpHandler::providerID = 0;

int startHandler() {
	return 0;
}

int stopHandler() {
	return 0;
}

DumpHandler::DumpHandler(jvmFunctions functions) {
	vmFunctions = functions;
	name = "Dump Handler";
	pull = NULL;
	push = NULL;
	start = ibmras::monitoring::plugins::j9::startHandler;
	stop = ibmras::monitoring::plugins::j9::stopHandler;
	type = ibmras::monitoring::plugin::none;
	recvfactory = NULL;
	confactory = NULL;
}

DumpHandler::~DumpHandler() {
}

DumpHandler* instance = NULL;

DumpHandler* DumpHandler::getInstance(jvmFunctions functions) {
	if (!instance) {
		instance = new DumpHandler(functions);
	}
	return instance;
}

DumpHandler* DumpHandler::getInstance() {
	if (!instance) {
		return NULL;
	}
	return instance;
}

void DumpHandler::requestDumps(const std::vector<std::string> &dumpRequests) {
	if (!ibmras::monitoring::agent::Agent::getInstance()->readOnly()) {
		getInstance()->triggerDumps(dumpRequests);
	}
}

void DumpHandler::triggerDumps(const std::vector<std::string> &dumpRequests) {
	JNIEnv * env;

	vmFunctions.theVM->AttachCurrentThread((void **) &env, NULL);

	for (std::vector<std::string>::const_iterator it = dumpRequests.begin();
			it != dumpRequests.end(); ++it) {
		const std::string parameter = (*it);
		std::vector < std::string > parts = ibmras::common::util::split(
				parameter, '=');

		// strip dump from end of parameter javadump, systemdump or heapdump
		if (ibmras::common::util::equalsIgnoreCase(parts[1], "true")) {

			std::string type = parts[0].substr(0, (parts[0].length()-4));

			if (vmFunctions.jvmtiTriggerVmDump != 0) {
				jvmtiError error;

				char * dumpType = new char[type.length()+1];
				strcpy(dumpType, type.c_str());
				error = vmFunctions.jvmtiTriggerVmDump(vmFunctions.pti, dumpType);
				delete[] dumpType;
			}
		}

	}

	cleanup: vmFunctions.theVM->DetachCurrentThread();
}

}
}
}
} /* end namespace methods */

