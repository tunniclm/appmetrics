/*
 * thread.cpp
 *
 *  Created on: 17 Feb 2014
 *      Author: adam
 */


/*
 * Functions that control thread behaviour
 */

#include "process.h"
#include "windows.h"
#include "stdio.h"
#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/port/Lock.h"
#include "ibmras/common/port/Semaphore.h"
#include "ibmras/common/logging.h"

namespace ibmras{
namespace common{

namespace port {

IBMRAS_DEFINE_LOGGER("Port");

typedef void *(*callback) (void *);

/* wrapper function to convert from void* to void return types */
void wrapper(void *params) {
	IBMRAS_DEBUG(fine,  "in thread.cpp->wrapper");
	ThreadData* data = reinterpret_cast<ThreadData*>(params);
	data->getCallback()(data);
}


uintptr_t createThread(ThreadData* data) {
	uintptr_t result;
	IBMRAS_DEBUG(fine,  "in thread.cpp->createThread");
	result = _beginthread(wrapper, 0, data);
	if(result) {
		return 0;	/* works = handle to thread, so convert to NULL for consistent semantics */
	}
}


void exitThread(void *val) {
	_endthread();
}

void sleep(uint32 seconds) {
	Sleep(1000 * seconds);
}

Lock::Lock() {
	lock = new CRITICAL_SECTION;		/* create a new lock fpr this class */
	CRITICAL_SECTION* c = reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<uintptr_t>(lock));
	InitializeCriticalSection(c);
	if(!lock) {
		lock = NULL;				/* reset lock so that we won't try and release it when the class is destroyed */
	}
}

/* acquire a pthread mutex */
int Lock::acquire() {
	if(lock) {
		CRITICAL_SECTION* c = reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<uintptr_t>(lock));
		EnterCriticalSection(c);
		return 0;
	} else {
		IBMRAS_DEBUG(warning,  "Attempted to acquire a previously failed lock");
		return LOCK_FAIL;
	}
}

/* release the mutex */
int Lock::release() {
	if(lock) {
		CRITICAL_SECTION* c = reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<uintptr_t>(lock));
		LeaveCriticalSection(c);
		return 0;
	} else {
		IBMRAS_DEBUG(warning,  "Attempted to release a previously failed lock");
		return LOCK_FAIL;
	}
}

void Lock::destroy() {
	if(lock) {
		CRITICAL_SECTION* c = reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<uintptr_t>(lock));
		DeleteCriticalSection(c);
		lock = NULL;
	}
}

bool Lock::isDestroyed() {
	return lock == NULL;
}

Lock::~Lock() {
	destroy();
}

Semaphore::Semaphore(uint32 initial, uint32 max) {
	handle = new HANDLE;
	IBMRAS_DEBUG(fine,  "in thread.cpp creating CreateSemaphoreA");
	handle = CreateSemaphoreA(NULL, initial, max, NULL);
	if(handle == NULL) {
		IBMRAS_DEBUG_1(warning,  "Failed to create semaphore : error code %d", GetLastError());
		handle = NULL;
	}
}

void Semaphore::inc() {
	IBMRAS_DEBUG(finest,  "Incrementing semaphore ticket count");
	if(handle) {
		ReleaseSemaphore(handle,1,NULL);
	}
}

bool Semaphore::wait(uint32 timeout) {

	IBMRAS_DEBUG(finest,  "Semaphore::wait");
	DWORD retVal = WaitForSingleObject(handle, timeout * 1000);
	if ( !GetLastError()) {
		return (retVal == WAIT_OBJECT_0);
	}
	return false;

}

Semaphore::~Semaphore() {
	IBMRAS_DEBUG(finest,  "Semaphore::~Semaphore()");
	ReleaseSemaphore(handle,1,NULL);
	CloseHandle(handle);
}
}
}
}	/* end of namespace port */
