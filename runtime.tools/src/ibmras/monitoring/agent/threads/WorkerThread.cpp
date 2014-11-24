 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#include "ibmras/monitoring/agent/threads/WorkerThread.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/common/logging.h"

namespace ibmras {
namespace monitoring {
namespace agent {
namespace threads {

extern IBMRAS_DECLARE_LOGGER;


WorkerThread::WorkerThread(pullsource* pullSource) : semaphore(0, 1), data(threadEntry), countdown(0) {
	source = pullSource;
	running = false;
	stopped = true;
	data.setArgs(this);
}


void WorkerThread::start() {
	IBMRAS_DEBUG_1(fine, "Starting worker thread for %s", source->header.name);
	running = true;
	stopped = false;
	ibmras::common::port::createThread(&data);
}

void WorkerThread::stop() {
	running = false;
	semaphore.inc();
	IBMRAS_DEBUG_1(debug, "Worker thread for %s stopped", source->header.name)
}


void* WorkerThread::threadEntry(ibmras::common::port::ThreadData* data) {
	((WorkerThread*) data->getArgs())->processLoop();
	ibmras::common::port::exitThread(NULL);
	return NULL;
}

void WorkerThread::process(bool immediate) {
	if ((immediate && countdown > 120) || (countdown == 0)) {
		semaphore.inc();
		countdown = source->pullInterval;
	} else {
		countdown--;
	}
}

bool WorkerThread::isStopped() {
	return stopped;
}

void* WorkerThread::processLoop() {
	IBMRAS_DEBUG_1(finest, "Worker thread started for %s", source->header.name);
	Agent* agent = Agent::getInstance();
	while (running) {
		if (semaphore.wait(1) && running) {
			IBMRAS_DEBUG_1(fine, "Pulling data from source %s", source->header.name);
			monitordata* data = source->callback();
			if (data != NULL) {
				if (data->size > 0) {
					IBMRAS_DEBUG_2(finest, "%d bytes of data pulled from source %s", data->size, source->header.name);
					agent->addData(data); /* put pulled data on queue for processing */
				}
				source->complete(data);
			}
		}
	}

	source->complete(NULL);
	stopped = true;
	IBMRAS_DEBUG_1(finest, "Worker thread for %s exiting process loop", source->header.name);
	return NULL;
}


}
}
}
} /* end of namespace threads */

