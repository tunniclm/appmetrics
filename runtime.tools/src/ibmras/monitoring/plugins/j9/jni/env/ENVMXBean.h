 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#ifndef ibmras_monitoring_plugins_j9_jni_envmxbean_h
#define ibmras_monitoring_plugins_j9_jni_envmxbean_h

#include "ibmras/monitoring/plugins/j9/jni/CFacade.h"
#include "ibmras/vm/java/healthcenter.h"
#include "ibmras/common/util/memUtils.h"

namespace ibmras {
namespace monitoring {
namespace plugins{
namespace j9 {
namespace jni {
namespace env {

class ENVPullSource : public PullSource {
public:
	ENVPullSource(uint32 id);
	pullsource* getDescriptor();
	uint32 getSourceID();
	monitordata* sourceData(jvmFunctions* tdpp, JNIEnv* Env);
	~ENVPullSource(){};
	void publishConfig();
};

}	/* end namespace env */
}	/* end namespace jmx */
}	/* end namespace j9 */
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */

#endif /*ibmras_monitoring_plugins_j9_jni_envmxbean_h*/
