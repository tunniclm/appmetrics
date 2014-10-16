/*
 * ThreadData.h
 *
 *  Created on: 31 Jan 2014
 *      Author: adam
 */

#ifndef ibmras_common_port_threaddata_h
#define ibmras_common_port_threaddata_h

#include <vector>
#include <string>
#include "ibmras/common/types.h"
#include "ibmras/export.h"


namespace ibmras {
namespace common {
namespace port {


class ThreadData;											/* forward declaration of ThreadData class */
typedef void* (*THREAD_CALLBACK)(ThreadData*);				/* shortcut definition for the thread  callback */

/* provides the encapsulation of different thread semantics for each platform */
class ThreadData {
public:
	ThreadData(THREAD_CALLBACK callback);
	~ThreadData(){}
	void setArgs(void* args);
	void* getArgs();
	THREAD_CALLBACK getCallback();
private:
	uintptr_t handle;								/* handle to underlying OS thread */
	THREAD_CALLBACK callback;						/* callback to make */
	void* args;
};

uintptr_t createThread(ThreadData *data);			/* create a thread and start it with specified callback and args */
void exitThread(void *val);							/* exit current thread with an optional return value */
void sleep(uint32 seconds);							/* sleep the current thread */

}
}
}	/*end of namespace port */

#endif /* ibmras_common_port_threaddata_h */
