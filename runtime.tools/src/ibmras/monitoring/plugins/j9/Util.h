 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
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
