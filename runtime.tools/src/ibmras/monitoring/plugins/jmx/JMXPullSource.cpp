/*
 * plugin.cpp
 *
 *  Created on: 31 Jan 2014
 *      Author: adam
 */

#include "ibmras/monitoring/plugins/jmx/JMXPullSource.h"
#include "ibmras/monitoring/plugins/jmx/JMXUtility.h"
#include "ibmras/common/logging.h"
#include <cstring>
#include <stdlib.h>


namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jmx {

JavaVM* vm = NULL;

monitordata* JMXPullSource::generateError(char* msg) {
	monitordata* data = new monitordata;
	data->provID = getProvID();
	data->sourceID = getSourceID();
	data->size = strlen(msg);
	data->data = msg;
	data->persistent = false;
	return data;
}


monitordata* JMXPullSource::generateData() {
	JNIEnv* env;
	jint result = vm->AttachCurrentThread((void**)&env, NULL);
	if (result != JNI_OK) {
		IBMRAS_DEBUG(warning,  "Cannot get JMX factory as environment is not set");
		return NULL;
	}
	jclass clazz = env->FindClass("java/lang/management/ManagementFactory");
	if (!clazz) {
		IBMRAS_DEBUG(warning,  "!Failed to find ManagementFactory class");
		return NULL;
	}
	IBMRAS_DEBUG(debug,  "Found management class");
	monitordata* data = generateData(env, &clazz);
	vm->DetachCurrentThread();		/* call complete, detach the thread */
	return data;
}

JMXPullSource::~JMXPullSource() {
	if(vm) {
		vm->DetachCurrentThread();		/* call complete, detach the thread */
	}
}


}	/* end namespace jmx */
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */


