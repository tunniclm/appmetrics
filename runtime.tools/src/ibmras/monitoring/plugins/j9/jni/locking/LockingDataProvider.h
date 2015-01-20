 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_monitoring_plugins_j9_jni_locking_lockingdataprovider_h
#define ibmras_monitoring_plugins_j9_jni_locking_lockingdataprovider_h

#include "ibmras/monitoring/plugins/j9/jni/CFacade.h"
#include "ibmras/common/util/memUtils.h"

namespace ibmras {
namespace monitoring {
namespace plugins{
namespace j9 {
namespace jni {
namespace locking {

class JLAPullSource : public PullSource {
public:
	JLAPullSource(uint32 id);
	pullsource* getDescriptor();
	uint32 getSourceID();
	monitordata* sourceData(jvmFunctions* tdpp, JNIEnv* env);
	~JLAPullSource(){};
	bool isEnabled();
	static void setState(const std::string &state);
	void publishConfig();
};

}	/* end namespace locking */
}	/* end namespace jni */
}	/* end namespace j9 */
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */

#endif /* LOCKINGDATAPROVIDER_H_ */
