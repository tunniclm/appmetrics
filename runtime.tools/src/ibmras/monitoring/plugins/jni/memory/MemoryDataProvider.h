#ifndef MEMORYDATAPROVIDER_H_
#define MEMORYDATAPROVIDER_H_

#include "ibmras/monitoring/plugins/jni/CFacade.h"
#include "ibmras/vm/java/healthcenter.h"

namespace ibmras {
namespace monitoring {
namespace plugins{
namespace jni {
namespace memory {

class MEMPullSource : public PullSource {
public:
	pullsource* getDescriptor();
	uint32 getSourceID();
	monitordata* sourceData(jvmFunctions* tdpp, JNIEnv* env);
	~MEMPullSource(){};
	bool isEnabled();
	static void setState(std::string state);
};

}	/* end namespace env */
}	/* end namespace jmx */
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */

#endif /*MEMORYDATAPROVIDER_H_*/
