/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

/*
 * Functions that control thread behaviour
 */

#include "pthread.h"
#include "time.h"
#include <semaphore.h>
#include <errno.h>

#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/port/Semaphore.h"
#include "ibmras/common/logging.h"


namespace ibmras{
namespace common{

namespace port {

IBMRAS_DEFINE_LOGGER("Port");

void* wrapper(void *params) {
	ThreadData* data = reinterpret_cast<ThreadData*>(params);
	return data->getCallback()(data);
}

uintptr_t createThread(ThreadData* data) {
	pthread_t thread;
	return pthread_create(&thread, NULL,wrapper, data);
}

void exitThread(void *val) {
	pthread_exit(NULL);
}

void sleep(uint32 seconds) {
	struct timespec t, tleft;
	t.tv_sec = seconds;		/* configure the sleep interval */
	t.tv_nsec = 0;
	nanosleep(&t, &tleft);
}


Semaphore::Semaphore(uint32 initial, uint32 max) {
	handle = new sem_t;
	int result;
	result = sem_init(reinterpret_cast<sem_t*>(handle), 0, initial);
	if(result) {
		IBMRAS_DEBUG_1(warning, "Failed to create semaphore : error code %d", result);
		handle = NULL;
	}
}

void Semaphore::inc() {
	IBMRAS_DEBUG(finest, "Incrementing semaphore ticket count");
	if(handle) {
		sem_post(reinterpret_cast<sem_t*>(handle));
	}
}

bool Semaphore::wait(uint32 timeout) {
	int result;
	struct timespec t;
	while(!handle) {
		sleep(timeout);		/* wait for the semaphore to be established */
	}
	clock_gettime(CLOCK_REALTIME, &t);
	t.tv_sec++;		/* configure the sleep interval */
	IBMRAS_DEBUG(finest, "semaphore wait");
	result = sem_timedwait(reinterpret_cast<sem_t*>(handle), &t);
	if(!result) {
		IBMRAS_DEBUG(finest, "semaphore posted");
		return true;
	}

	IBMRAS_DEBUG(finest, "semaphore timeout");
	return (errno != ETIMEDOUT);
}

Semaphore::~Semaphore() {
	sem_destroy(reinterpret_cast<sem_t*>(handle));
	delete (sem_t*)handle;
}

}		/* end namespace port */

}
}
