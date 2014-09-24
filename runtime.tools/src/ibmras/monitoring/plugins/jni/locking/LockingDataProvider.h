/*
 * LockingDataProvider.h
 *
 *  Created on: 1 Aug 2014
 *      Author: Admin
 */

#ifndef ibmras_monitoring_plugins_jni_locking_lockingdataprovider_h
#define ibmras_monitoring_plugins_jni_locking_lockingdataprovider_h

#include "ibmras/monitoring/plugins/jni/CFacade.h"
#include "ibmras/common/util/memUtils.h"

namespace ibmras {
namespace monitoring {
namespace plugins{
namespace jni {
namespace locking {

class JLAPullSource : public PullSource {
public:
	pullsource* getDescriptor();
	uint32 getSourceID();
	monitordata* sourceData(jvmFunctions* tdpp, JNIEnv* env);
	~JLAPullSource(){};
};

}	/* end namespace locking */
}	/* end namespace jni */
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */

#endif /* LOCKINGDATAPROVIDER_H_ */
