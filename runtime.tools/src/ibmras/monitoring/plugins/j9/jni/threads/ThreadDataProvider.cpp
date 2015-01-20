 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include "ibmras/monitoring/plugins/j9/jni/threads/ThreadDataProvider.h"
#include "ibmras/monitoring/agent/Agent.h"

#include <cstring>
#include "ibmras/common/util/memUtils.h"
#include "ibmras/common/util/strUtils.h"
#include <iostream>

using namespace ibmras::monitoring::plugins::j9::jni;

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace jni {
namespace threads {

TDPullSource* src = NULL;
bool enabled = true;

PullSource* getTDPullSource(uint32 id) {
	if (!src) {
		src = new TDPullSource(id);
	}
	return src;
}

bool TDPullSource::isEnabled() {
	return enabled;
}

TDPullSource::TDPullSource(uint32 id) : PullSource(id, "Health Center (threads)") {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	std::string enableProp = agent->getAgentProperty("data.threads");
	if (enableProp == "on" || enableProp == "") {
		enabled = true;
	} else {
		enabled = false;
	}
}

void TDPullSource::publishConfig() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();

	ibmras::monitoring::connector::ConnectorManager *conMan =
			agent->getConnectionManager();

	std::string msg = "threads_subsystem=";
	if (isEnabled()) {
		msg += "on";
	} else {
		msg += "off";
	}

	conMan->sendMessage("configuration/threads", msg.length(),
			(void*) msg.c_str());
}

void TDPullSource::setState(const std::string &newState) {
	enabled = ibmras::common::util::equalsIgnoreCase(newState, "on");
	if (src) {
		src->publishConfig();
	}
}

monitordata* callback() {
	return src->generateData();
}

void complete(monitordata *mdata) {
	src->pullComplete(mdata);
}

uint32 TDPullSource::getSourceID() {
	return TD;
}

pullsource* TDPullSource::getDescriptor() {

	pullsource* src = new pullsource();
	src->header.name = "threads";
	src->header.description = "Thread information";
	src->header.sourceID = TD;
	src->header.capacity = 32 * 1024;
	src->next = NULL;
	src->callback = callback;
	src->complete = complete;
	src->pullInterval = 30;

	return src;
}

monitordata* TDPullSource::sourceData(jvmFunctions* tdpp, JNIEnv* env) {
	monitordata* data = new monitordata;
	data->size = 0;
	data->data = NULL;

	if (isEnabled()) {
		data->persistent = false;
		data->provID = getProvID();
		data->sourceID = TD;
#if defined(_ZOS)
#pragma convert("ISO8859-1")
#endif
		std::string cp =
				getString(env,
						"com/ibm/java/diagnostics/healthcenter/agent/dataproviders/threads/ThreadDataProvider",
						"getJMXData", "()Ljava/lang/String;");
#if defined(_ZOS)
#pragma convert(pop)
#endif
		int len = cp.length();
		char* sval = reinterpret_cast<char*>(hc_alloc(len + 1));
		if (sval) {
			strcpy(sval, cp.c_str());
		}
		if (sval) {
			data->size = len;
			data->data = sval;
		}
	}
	return data;
}

} /* End of namespace threads */
} /* End of namespace jni */
} /* End of namespace j9 */
} /* End of namespace plugins */
} /* End of namespace monitoring */
} /* End of namespace ibmras */

