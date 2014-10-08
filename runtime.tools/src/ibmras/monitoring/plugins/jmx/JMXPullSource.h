/*
 * JMXPullSource.h
 *
 *  Created on: 28 Feb 2014
 *      Author: adam
 */

#ifndef ibmras_monitoring_plugins_jmx_jmxpullsource_h
#define ibmras_monitoring_plugins_jmx_jmxpullsource_h

/* declarations for the plugin that will provide OS metrics */
#include <jni.h>
#include "ibmras/common/port/ThreadData.h"
#include "ibmras/monitoring/Plugin.h"
#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/monitoring/plugins/jmx/JMX.h"
#include <cstdio>
#include <string>

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jmx {

/*
 * This enum corresponds to the source IDs for each of the JMX data providers.
 */
enum PullSourceIDs {
	CPU, RT
};

/*
 * This is the pure virtual class that specific JMX pull sources need to inherit from.
 * When the agent calls back for data, you need to attach the current agent thread to
 * the JVM using the JNI->attachThread call otherwise you cannot see the running Java code.
 * This is why all agent calls come into the superclass generateData() method, where this
 * attachment is performed before calling the subclass specific generateData(JNIEnv*, jclass*) method.
 * It will also locate the ManagementFactory class (which is the second parameter). Because
 * the VM thread will change over time you cannot cache the JNIEnv or other values.
 */

class JMXPullSource {
public:
	virtual pullsource* getDescriptor() {return NULL;};			/* descriptor for this pull source */
	virtual ~JMXPullSource();										/* desctructor for clean up operations */
	uint32 getProvID() {return provID; }						/* JMX provider ID */
	virtual uint32 getSourceID() = 0;							/* source ID, overridden by the subclass, and corresponds to the enum entries */
	void setProvID(uint32 provID) { this->provID = provID; };	/* allows the prov ID to be set - remember this is allocated by the agent at startup */
	monitordata* generateData();								/* where the agent will call into */
	monitordata* generateError(char* msg);						/* Wrap an error message to send back */
	virtual void publishConfig() = 0;							/* sub classes must implement to publish their configuration details */
protected:
	virtual monitordata* generateData(JNIEnv* env, jclass* mgtBean) = 0 ;		/* sub classes must implement this to get the right JNI env to use to retrieve data */
private:
	JNIEnv* aquireMgtFactory();		/* get the Mangement Bean Factory */
	uint32 provID;					/* JMX provider ID assigned by the agent */

};


}	/* end namespace jmx */
}	/* end namespace plugins */
}	/* end namespace monitoring */
}	/* end namespace ibmras */


#endif /* ibmras_monitoring_plugins_jmx_jmxpullsource_h */
