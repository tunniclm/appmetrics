 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_monitoring_agent_pullsourcecounter_h
#define ibmras_monitoring_agent_pullsourcecounter_h

#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/port/Semaphore.h"
#include "ibmras/monitoring/Monitoring.h"

namespace ibmras {
namespace monitoring {
namespace agent {

class PullSourceCounter {
public:
	PullSourceCounter(uint32 interval = 0, PULL_CALLBACK callback = NULL, PULL_CALLBACK_COMPLETE complete = NULL);
	PullSourceCounter operator--(int x);
	monitordata* getData();
	void init(uint32 interval, PULL_CALLBACK callback, PULL_CALLBACK_COMPLETE complete);
	bool isQueued();
	void setQueued(bool value);
	bool isExpired();
	void expire();
	void reset();				/* reset the counter */

	PULL_CALLBACK_COMPLETE complete;
private:
	uint32 interval;
	uint32 current;
	uint32 missed;
	PULL_CALLBACK callback;
	bool queued;
};

}
}
}	/* end namespace agent */

#endif /* ibmras_monitoring_agent_pullsourcecounter_h */
