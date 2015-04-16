/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
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

IBMRAS_DEFINE_LOGGER("Threads")
;

ThreadPool::ThreadPool() {
}

void ThreadPool::addPullSource(pullsource* src) {
	threads.push_back(new WorkerThread(src));
}

void ThreadPool::startAll() {
	IBMRAS_DEBUG(info, "Starting thread pool");
	for (uint32 i = 0; i < threads.size(); i++) {
		threads[i]->start();
	}
}

void ThreadPool::stopAll() {
	IBMRAS_DEBUG(info, "Stopping thread pool");
	for (uint32 i = 0; i < threads.size(); i++) {
		threads[i]->stop();
	}
	uint32 stoppedCount = 0;
	uint32 maxWait = 5;
	while ((stoppedCount < threads.size()) && (maxWait > 0)) {
		stoppedCount = 0;
		for (uint32 i = 0; i < threads.size(); i++) {
			if (threads[i]->isStopped()) {
				stoppedCount++;
			}
		}

		if (stoppedCount == threads.size()) {
			break;
		}

		IBMRAS_DEBUG_1(debug, "Waiting for %d worker threads to stop", threads.size() - stoppedCount);
		ibmras::common::port::sleep(1);
		maxWait--;
	}
}

ThreadPool::~ThreadPool() {
	for (uint32 i = 0; i < threads.size(); i++) {
		// Only delete threads that are stopped
		// unlikely leak but prevents an abort
		if (threads[i]->isStopped()) {
			delete threads[i];
		}
	}
}

void ThreadPool::process(bool immediate) {
	IBMRAS_DEBUG(finest, "Processing pull sources");
	for (uint32 i = 0; i < threads.size(); i++) {
		threads[i]->process(immediate);
	}
}

}
}
}
} /* end of namespace threads */

