#ifndef MEMORYCOUNTERDATAPROVIDER_H_
#define MEMORYCOUNTERDATAPROVIDER_H_

#include "ibmras/monitoring/plugins/jni/CFacade.h"
#include "ibmras/common/util/memUtils.h"
#include "ibmras/vm/java/healthcenter.h"


namespace ibmras {
namespace monitoring {
namespace plugins{
namespace jni {
namespace memorycounter {

class MCPullSource : public PullSource {
public:
	pullsource* getDescriptor();
	uint32 getSourceID();
	monitordata* sourceData(jvmFunctions* tdpp, JNIEnv* env);
	~MCPullSource(){};
	bool isEnabled();
	static void setState(const std::string &newState);
	void publishConfig();
};

}	/* end namespace env */
}	/* end namespace jmx */
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */

#endif /*MEMORYDATAPROVIDER_H_*/
