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

void* WorkerThread::threadEntry(ibmras::common::port::ThreadData* data) {
	((WorkerThread*) data->getArgs())->osentry(data);
	return NULL;
}

void WorkerThread::start(ibmras::common::port::Semaphore* semaphore) {
	IBMRAS_DEBUG(fine, "Starting worker thread");
	busy = false;
	running = true;
	this->semaphore = semaphore;
	acquired = false;
	data = new ibmras::common::port::ThreadData(WorkerThread::threadEntry);
	data->setArgs(this);
	ibmras::common::port::createThread(data);
}

void* WorkerThread::osentry(ibmras::common::port::ThreadData* args) {
	IBMRAS_DEBUG_1(finest, "Worker thread 0x%p started by OS", this);
	Agent* agent = Agent::getInstance();
	while (running) {
		acquired = semaphore->wait(1); /* wait for 1 second for semaphore */
		if (acquired && running) {
			IBMRAS_DEBUG_1(fine, "Pulling data from source (tid = 0x%p)", this);
			monitordata* data = source->getData();
			if (data != NULL) {
				if (data->size > 0) {
					IBMRAS_DEBUG_1(finest, "%d bytes of data pulled from source", data->size);
					agent->addData(data); /* put pulled data on queue for processing */
				}
				source->complete(data);
			}
			busy = false; /* thread no longer busy */
			source->setQueued(false); /* pull source no longer queued */
			source->reset();
			IBMRAS_DEBUG_1(fine, "Worker thread 0x%p available", this);
		}
	}
	return NULL;
}

void WorkerThread::stop() {
	running = false;
}

void WorkerThread::setSource(PullSourceCounter* source) {
	busy = true;
	this->source = source;
}

bool WorkerThread::isBusy() {
	return busy;
}

}
}
}
} /* end of namespace threads */

