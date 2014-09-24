/*
 * ThreadPool.h
 *
 *  Created on: 25 Feb 2014
 *      Author: adam
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
	void process();				/* process queue entries */
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
