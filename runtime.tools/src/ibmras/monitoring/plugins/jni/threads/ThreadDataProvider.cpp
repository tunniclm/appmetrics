#include "ibmras/monitoring/plugins/jni/threads/ThreadDataProvider.h"
#include "ibmras/monitoring/agent/Agent.h"

#include <cstring>
#include "ibmras/common/util/memUtils.h"
#include "ibmras/common/util/strUtils.h"
#include <iostream>

using namespace ibmras::monitoring::plugins::jni;

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jni {
namespace threads {

TDPullSource* src = NULL;
bool enabled = true;

PullSource* getTDPullSource() {
	if (!src) {
		src = new TDPullSource;
	}
	return src;
}

bool TDPullSource::isEnabled() {
	return enabled;
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

	conMan->sendMessage("ThreadsSourceConfiguration", msg.length(),
			(void*) msg.c_str());
}

void TDPullSource::setState(std::string newState) {
	enabled = ibmras::common::util::equalsIgnoreCase(newState, "on");
	getTDPullSource()->publishConfig();
}

monitordata* callback() {
	return src->PullSource::generateData();
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
	src->complete = ibmras::monitoring::plugins::jni::complete;
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

		std::string cp =
				getString(env,
						"runtime/tools/java/dataproviders/threads/ThreadDataProviderJNI",
						"getJMXData", "()Ljava/lang/String;");
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
} /* End of namespace plugins */
} /* End of namespace monitoring */
} /* End of namespace ibmras */

