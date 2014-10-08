/*
 * Util.h
 *
 *  Created on: 8 Oct 2014
 *      Author: robbins
 */

#ifndef ibmras_monitoring_plugins_j9_Util_h
#define ibmras_monitoring_plugins_j9_Util_h

#include <jvmti.h>
#include <string>

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {

class Util {
public:
	static int getJavaLevel();
	static int getServiceRefreshNumber();
	static bool is26VMOrLater();
	static bool is27VMOrLater();
	static bool vmHasLOATracePoints();
	static bool j9DmpTrcAvailable();
	static bool isRealTimeVM();
	static std::string getLowAllocationThreshold();
	static std::string getHighAllocationThreshold();

	static std::string queryVmDump(jvmtiExtensionFunction jvmtiQueryVmDump, jvmtiEnv* pti);
};

} /* namespace j9 */
} /* namespace plugins */
} /* namespace monitoring */
} /* namespace ibmras */
#endif /* ibmras_monitoring_plugins_j9_Util_h */
