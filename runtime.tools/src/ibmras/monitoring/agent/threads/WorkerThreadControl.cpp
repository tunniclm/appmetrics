 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
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

