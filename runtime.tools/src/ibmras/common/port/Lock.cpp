/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#if defined(_ZOS)
#define _OPEN_THREADS
#endif

#include "ibmras/common/port/Lock.h"
#include "ibmras/common/logging.h"

#if defined(WINDOWS)
#include <process.h>
#include <windows.h>
#else
#include "pthread.h"
#endif

namespace ibmras{
namespace common{
namespace port {

namespace locking {
IBMRAS_DEFINE_LOGGER("locking")
}
using namespace locking;

Lock::Lock() {
#if defined(WINDOWS)
	lock = new CRITICAL_SECTION;		/* create a new lock fpr this class */
	CRITICAL_SECTION* c = reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<uintptr_t>(lock));
	InitializeCriticalSection(c);
#else
	lock = new pthread_mutex_t;		/* create a new lock fpr this class */
	pthread_mutex_t* mutex = reinterpret_cast<pthread_mutex_t*>(lock);
	pthread_mutex_init(mutex, NULL);
#endif
}

/* acquire a pthread mutex */
int Lock::acquire() {
	if(lock) {
#if defined(WINDOWS)
		CRITICAL_SECTION* c = reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<uintptr_t>(lock));
		EnterCriticalSection(c);
		return 0;
#else
		return pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(lock));
#endif

	} else {
		IBMRAS_DEBUG(warning, "Attempted to acquire a previously failed lock");
		return LOCK_FAIL;
	}
}

/* release the mutex */
int Lock::release() {
	if(lock) {
#if defined(WINDOWS)
		CRITICAL_SECTION* c = reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<uintptr_t>(lock));
		LeaveCriticalSection(c);
		return 0;
#else
		return pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(lock));
#endif
	} else {
		IBMRAS_DEBUG(warning, "Attempted to release a previously failed lock");
		return LOCK_FAIL;
	}
}

void Lock::destroy() {
	if(lock) {
#if defined(WINDOWS)
		CRITICAL_SECTION* c = reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<uintptr_t>(lock));
		DeleteCriticalSection(c);
#else
		pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t*>(lock));
#endif
		lock = NULL;
	}
}

bool Lock::isDestroyed() {
	return lock == NULL;
}

Lock::~Lock() {
	destroy();
}

}
}
}

