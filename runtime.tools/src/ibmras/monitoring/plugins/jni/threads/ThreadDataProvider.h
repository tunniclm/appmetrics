#ifndef THREADDATAPROVIDER_H_
#define THREADDATAPROVIDER_H_

#include "ibmras/monitoring/plugins/jni/CFacade.h"

namespace ibmras {
namespace monitoring {
namespace plugins{
namespace jni {
namespace threads {

class TDPullSource : public PullSource {
public:
	pullsource* getDescriptor();
	uint32 getSourceID();
	monitordata* sourceData(jvmFunctions* tdpp, JNIEnv* env);
	~TDPullSource(){};
	bool isEnabled();
	static void setState(std::string state);
	void publishConfig();
};

}	/* end namespace env */
}	/* end namespace jmx */
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */

#endif /*THREADDATAPROVIDER_H_*/
