/*
 * threadpool.cpp
 *
 *  Created on: 25 Feb 2014
 *      Author: adam
 */

/*
 * Thread pool and associated worker threads
 */

#include "ibmras/monitoring/agent/threads/ThreadPool.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/common/logging.h"

namespace ibmras {
namespace monitoring {
namespace agent {
namespace threads {

IBMRAS_DEFINE_LOGGER("Threads");

ThreadPool::ThreadPool(uint32 size, uint32 srccount) {
	IBMRAS_DEBUG_2(fine,  "Creating pull source thread pool [%d] for [%d] sources", size, srccount);
	threads = new WorkerThreadControl[size];
	this->size = size;
	counterSize = srccount;
	counters = new PullSourceCounter[counterSize];
}

void ThreadPool::setPullSource(uint32 index, pullsource* src) {
	IBMRAS_DEBUG_3(finest,  "Setting pull source %s[%d] with interval of %d seconds",src->header.name, index, src->pullInterval);
	counters[index].init(src->pullInterval, src->callback, src->complete);
}

void ThreadPool::stopAll() {
	IBMRAS_DEBUG(info,  "Stopping thread pool");
	for(uint32 i = 0; i < size; i++) {
		threads[i].getWorker()->stop();
	}
}

void ThreadPool::startAll() {
	IBMRAS_DEBUG(info,  "Starting thread pool");
	for(uint32 i = 0; i < size; i++) {
		threads[i].getWorker()->start(threads[i].getSemaphore());
	}
}

ThreadPool::~ThreadPool() {
	delete[] threads;
}

void ThreadPool::process() {
	IBMRAS_DEBUG(finest,  "Processing pull sources");
	for(uint32 i = 0; i < counterSize; i++) {
		counters[i]--;			/* decrement counters */
		if(!counters[i].isQueued() && counters[i].isExpired()) {
			/* counter has expired so need to schedule on first free thread */
			for(uint32 j = 0; j < size; j++) {
				if(!threads[j].getWorker()->isBusy()) {
					IBMRAS_DEBUG_2(fine,  "Scheduling pull source [%d] for processing on thread [%d]", i, j);
					counters[i].setQueued(true);
					threads[j].getWorker()->setSource(&counters[i]);
					threads[j].signal();
					break;			/* scheduled so stop looking for a free thread */
				}
			}
		}
	}
}

}
}
}
}	/* end of namespace threads */

