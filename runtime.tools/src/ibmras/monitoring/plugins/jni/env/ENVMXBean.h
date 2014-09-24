#ifndef ENVMXBEAN_H_
#define ENVMXBEAN_H_

#include "ibmras/monitoring/plugins/jni/CFacade.h"
#include "ibmras/vm/java/healthcenter.h"
#include "ibmras/common/util/memUtils.h"

namespace ibmras {
namespace monitoring {
namespace plugins{
namespace jni {
namespace env {

class ENVPullSource : public PullSource {
public:
	pullsource* getDescriptor();
	uint32 getSourceID();
	monitordata* sourceData(jvmFunctions* tdpp, JNIEnv* Env);
	~ENVPullSource(){};
};

}	/* end namespace env */
}	/* end namespace jmx */
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */

#endif /*ENVMXBEAN_H_*/
