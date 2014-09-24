/*
 * threadpool.h
 *
 *  Created on: 25 Feb 2014
 *      Author: adam
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
	void reset();				/* reset the counter */
private:
	uint32 interval;
	uint32 current;
	uint32 missed;
	PULL_CALLBACK callback;
	PULL_CALLBACK_COMPLETE complete;
	bool queued;
};

}
}
}	/* end namespace agent */

#endif /* ibmras_monitoring_agent_pullsourcecounter_h */
