 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
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
