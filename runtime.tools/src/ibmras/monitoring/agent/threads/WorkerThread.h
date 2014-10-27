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
#include "ibmras/monitoring/agent/PullSourceCounter.h"

namespace ibmras {
namespace monitoring {
namespace agent {
namespace threads {

class WorkerThread {
public:
	void start(ibmras::common::port::Semaphore* semaphore);				/* start this worker thread taking from the queue */
	void stop();				/* stop this thread from taking any more entries */
	void setSource(PullSourceCounter* source);	/* set the next source to pull from */
	bool isBusy();				/* indicates if the thread is processing a request */
	static void* threadEntry(ibmras::common::port::ThreadData* data);
private:
	void* osentry(ibmras::common::port::ThreadData* data);				/* entry function for OS to call back into */
	bool running;
	uintptr_t osthread;				/* underlying OS thread backing this worker */
	ibmras::common::port::Semaphore* semaphore;		/* sempahore to control data processing */
	bool acquired;					/* true if the semaphore was acquired, false if timeout */
	PullSourceCounter* source;		/* source to pull data from */
	ibmras::common::port::ThreadData* data;
	bool busy;						/* indicates if a counter has been queued for processing */
};

}
}
}
}	/* end namespace threads */

#endif /* ibmras_monitoring_agent_threads_workerthread_h */
