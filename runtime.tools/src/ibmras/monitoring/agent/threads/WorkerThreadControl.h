/*
 * WorkerThreadControl.h
 *
 *  Created on: 25 Feb 2014
 *      Author: adam
 */

#ifndef ibmras_monitoring_agent_threads_workerthreadcontrol_h
#define ibmras_monitoring_agent_threads_workerthreadcontrol_h

#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/port/Semaphore.h"
#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/monitoring/agent/threads/WorkerThread.h"

namespace ibmras {
namespace monitoring {
namespace agent {
namespace threads {

class WorkerThreadControl {
public:
	WorkerThreadControl();
	WorkerThread* getWorker();
	ibmras::common::port::Semaphore* getSemaphore();
	void signal();
private:
	WorkerThread* worker;
	ibmras::common::port::Semaphore* semaphore;
};


}
}
}
}	/* end namespace threads */

#endif /* ibmras_monitoring_agent_threads_workerthreadcontrol_h */
