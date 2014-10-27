 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_monitoring_agent__threads_threadpool_h
#define ibmras_monitoring_agent__threads_threadpool_h

#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/port/Semaphore.h"
#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/monitoring/agent/threads/WorkerThreadControl.h"

namespace ibmras {
namespace monitoring {
namespace agent {
namespace threads {

/* a pool of worker threads */
class ThreadPool {
public:
	ThreadPool(uint32 size, uint32 srccount);
	void setPullSource(uint32 index, pullsource* src);
	void startAll();			/* start all threads in this pool */
	void stopAll();				/* stop all threads in this pool */
	void process(bool immediate);				/* process queue entries */
	~ThreadPool();
private:
	uint32 size;
	WorkerThreadControl* threads;			/* worker threads */
	PullSourceCounter* counters;	/* count down timers for pulling data */
	uint32 counterSize;				/* number of countdown counters */
};

}
}
}
}	/* end namespace threads */

#endif /* ibmras_monitoring_agent__threads_threadpool_h */
