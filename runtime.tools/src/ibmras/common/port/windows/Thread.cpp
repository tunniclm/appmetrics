/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

/*
 * Functions that control thread behaviour
 */

#include "process.h"
#include "windows.h"
#include "stdio.h"
#include "ibmras/common/port/ThreadData.h"
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
	return 1;
}


void exitThread(void *val) {
	_endthread();
}

void sleep(uint32 seconds) {
	Sleep(1000 * seconds);
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
