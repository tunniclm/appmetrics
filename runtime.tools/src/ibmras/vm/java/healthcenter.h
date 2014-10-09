/*
 * healthcenter.h
 *
 *  Created on: 22 July 2014
 *      Author: stalleyj
 */

#ifndef HEALTHCENTER_H_
#define HEALTHCENTER_H_

#include "jvmti.h"

struct jvmFunctions {
	jvmtiExtensionFunction setTraceOption;
	jvmtiExtensionFunction jvmtiRegisterTraceSubscriber;
	jvmtiExtensionFunction jvmtiDeregisterTraceSubscriber;
	jvmtiExtensionFunction jvmtiGetTraceMetadata;
	jvmtiExtensionFunction jvmtiGetMemoryCategories;
	jvmtiExtensionFunction jvmtiGetMethodAndClassNames;
	jvmtiExtensionFunction jvmtiFlushTraceData;
	jvmtiExtensionFunction getJ9method;
	jvmtiExtensionFunction jvmtiSetVmDump;
	jvmtiExtensionFunction jvmtiQueryVmDump;
	jvmtiExtensionFunction jvmtiResetVmDump;
	jvmtiExtensionFunction dumpVMLockMonitor;
	jvmtiExtensionFunction setVMLockMonitor;
	jvmtiExtensionFunction verboseGCsubscribe;
	jvmtiExtensionFunction verboseGCunsubscribe;
	jvmtiExtensionFunction jvmtiTriggerVmDump;
	
	jvmtiEnv *pti;
	JavaVM *theVM;
};

#endif /* HEALTHCENTER_H_ */
