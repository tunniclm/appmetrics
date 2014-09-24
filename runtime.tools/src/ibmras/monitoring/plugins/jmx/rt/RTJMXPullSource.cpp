/*
 * oslinux.cpp
 *
 *  Created on: 3 Apr 2014
 *      Author: adam
 */

/*
 * Linux specific OS routines
 */

#include "ibmras/monitoring/plugins/jmx/rt/RTJMXPullSource.h"
#include "ibmras/monitoring/plugins/jmx/JMXSourceManager.h"
#include "ibmras/monitoring/plugins/jmx/JMXUtility.h"
#include "ibmras/common/logging.h"
#include <cstring>

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jmx {
namespace rt {

IBMRAS_DEFINE_LOGGER("JMXSources");

/* need to be in own namespace so that top level callbacks work with other MX bean data providers */
RTJMXPullSource* src = NULL;

JMXPullSource* getRTPullSource() {
	if(!src) {
		src = new RTJMXPullSource;
	}
	return src;
}

monitordata* callback() {
	return src->JMXPullSource::generateData();
}

uint32 RTJMXPullSource::getSourceID() {
	return CPU;
}

pullsource* RTJMXPullSource::getDescriptor() {
	pullsource *src = new pullsource();
	src->header.name = "RT";
	src->header.description = "Runtime information";
	src->header.sourceID = RT;
	src->header.capacity = JMXSourceManager::DEFAULT_CAPACITY;
	src->header.config = "runtime_subsystem=on";
	src->next = NULL;
	src->callback = callback;
	//src->complete = getCallbackComplete();
	src->complete = ibmras::monitoring::plugins::jmx::complete;	/* use default clean up */
	src->pullInterval = 120;
	return src;
}


monitordata* RTJMXPullSource::generateData(JNIEnv* env, jclass* mgtBean) {
	IBMRAS_DEBUG(debug, "Generating JMX Runtime data");
	monitordata* data = new monitordata;
	data->persistent = false;
	data->provID = getProvID();
	data->sourceID = RT;

	jobject mgt = getMXBean(env, mgtBean, "Runtime");
	if(mgt) {
		IBMRAS_DEBUG(debug, "Invoking getClassPath");
		char* cp = getString(env, &mgt, "java/lang/management/RuntimeMXBean", "getClassPath");
		if(cp) {
			data->size = strlen(cp);
			data->data = cp;
			return data;
		}
	}
	data->size = 0;
	data->data = NULL;
	return data;
}

}	/* end namespace rt */
}	/* end namespace jmx */
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */
