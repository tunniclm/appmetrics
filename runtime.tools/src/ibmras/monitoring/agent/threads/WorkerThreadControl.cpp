/*
 * WorkerThreadControl.cpp
 *
 *  Created on: 25 Feb 2014
 *      Author: adam
 */

#include "ibmras/monitoring/agent/threads/WorkerThreadControl.h"
#include "ibmras/monitoring/agent/Agent.h"

namespace ibmras {
namespace monitoring {
namespace agent {
namespace threads {

WorkerThreadControl::WorkerThreadControl() {
	worker = new WorkerThread;
	semaphore = new ibmras::common::port::Semaphore(0, 1);
}

WorkerThread* WorkerThreadControl::getWorker() {
	return worker;
}

ibmras::common::port::Semaphore* WorkerThreadControl::getSemaphore() {
	return semaphore;
}

void WorkerThreadControl::signal() {
	semaphore->inc();
}

}
}
}
}	/* end of namespace threads */

