/*
 * oslinux.cpp
 *
 *  Created on: 3 Apr 2014
 *      Author: adam
 */

/*
 * Linux specific OS routines
 */

#include "ibmras/monitoring/plugins/jmx/os/OSJMXPullSource.h"
#include "ibmras/monitoring/plugins/jmx/JMXSourceManager.h"
#include "ibmras/monitoring/plugins/jmx/JMXUtility.h"
#include "ibmras/common/data/legacy/LegacyData.h"
#include <cstring>
#include "ibmras/common/logging.h"

using namespace ibmras::common::data::legacy;
using namespace ibmras::monitoring::plugins::jmx;

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jmx {
namespace os {

IBMRAS_DEFINE_LOGGER("JMXSources");

/* need to be in own namespace so that top level callbacks work with other MX bean data providers */
OSJMXPullSource* src = NULL;

JMXPullSource* getOSPullSource() {
	if(!src) {
		src = new OSJMXPullSource;
	}
	return src;
}

monitordata* callback() {
	return src->JMXPullSource::generateData();
}

uint32 OSJMXPullSource::getSourceID() {
	return CPU;
}

pullsource* OSJMXPullSource::getDescriptor() {
	pullsource *src = new pullsource();
	src->header.name = "cpu";
	src->header.description = "CPU usage";
	src->header.sourceID = CPU;
	src->header.capacity = JMXSourceManager::DEFAULT_CAPACITY;
	src->header.config = "cpu_subsystem=on";
	src->next = NULL;
	src->callback = callback;
	//src->complete = getCallbackComplete();
	src->complete = ibmras::monitoring::plugins::jmx::complete;	/* use default clean up */
	src->pullInterval = 2;
	return src;
}

monitordata* OSJMXPullSource::generateData(JNIEnv* env, jclass* mgtBean) {

	IBMRAS_DEBUG(debug, "Generating JMX CPU data");
	monitordata* data = new monitordata;
	data->persistent = false;
	data->provID = getProvID();
	data->sourceID = CPU;

	jobject mgt = getMXBean(env, mgtBean, "OperatingSystem");
	if(mgt) {
		IBMRAS_DEBUG(debug, "Getting timestamp");
		jlong tstamp = getTimestamp(env);
		IBMRAS_DEBUG(debug, "Invoking getSystemCpuLoad");
		jdouble systemCPULoad = getDouble(env, &mgt,  "com/ibm/lang/management/OperatingSystemMXBean", "getSystemCpuLoad");
		IBMRAS_DEBUG(debug, "Invoking getProcessCpuLoad : this is only available in Java 7 and later");
		jdouble processCPULoad = getDouble(env, &mgt, "com/ibm/lang/management/OperatingSystemMXBean", "getProcessCpuLoad");

		if (processCPULoad >= 0 || systemCPULoad >= 0) {
			IBMRAS_DEBUG(debug, "Constructing CPU data line");
			LegacyData* line = new LegacyData("startCPU", tstamp);
			LegacyDataNumeric<jdouble>* value = new LegacyDataNumeric<jdouble>(
					processCPULoad);
			line->add(value);
			value = new LegacyDataNumeric<jdouble>(systemCPULoad);
			line->add(value);
			char* sval = line->getData();
			IBMRAS_DEBUG_1(debug, "Got CPU data : %s", sval);
			data->size = strlen(sval);
			data->data = sval;
			delete line; /* deleteing the line should cascade delete the values it is currently holding */
			return data;
		}
	}
	data->size = 0;
	data->data = NULL;
	return data;
}


}	/* end namespace os */
}	/* end namespace jmx */
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */
