 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_monitoring_plugins_jmx_os_osjmxpullsource_h
#define ibmras_monitoring_plugins_jmx_os_osjmxpullsource_h

#include "ibmras/monitoring/plugins/jmx/JMXPullSource.h"

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jmx {
namespace os {

/*
 * PullSource for data that will be retrieved from the OperatingSystemMXBean
 */

class OSJMXPullSource : public JMXPullSource {
public:
	pullsource* getDescriptor();
	uint32 getSourceID();
	monitordata* generateData(JNIEnv* env, jclass* mgtBean);
	~OSJMXPullSource(){};
	bool isEnabled();
	static void setState(const std::string &newState);
	void publishConfig();
};


}	/* end namespace os */
}	/* end namespace jmx */
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */


#endif /* ibmras_monitoring_plugins_jmx_os_osjmxpullsource_h */
