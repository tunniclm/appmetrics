 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_monitoring_agent_threads_workerthread_h
#define ibmras_monitoring_agent_threads_workerthread_h

#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/port/Semaphore.h"
#include "ibmras/monitoring/Monitoring.h"

namespace ibmras {
namespace monitoring {
namespace agent {
namespace threads {

class WorkerThread {
public:
	WorkerThread(pullsource* source);
	void start();				/* start this worker thread taking from the queue */
	void stop();				/* stop this thread from taking any more entries */

	void process(bool immediate);
	bool isStopped();

	static void* threadEntry(ibmras::common::port::ThreadData* data);
private:
	void* processLoop();
	bool running;
	bool stopped;
	ibmras::common::port::Semaphore semaphore;		/* sempahore to control data processing */
	pullsource* source;		/* source to pull data from */
	ibmras::common::port::ThreadData data;
	int countdown;
};

}
}
}
}	/* end namespace threads */

#endif /* ibmras_monitoring_agent_threads_workerthread_h */
