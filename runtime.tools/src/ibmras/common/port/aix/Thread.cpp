/*
 * thread.cpp
 *
 */


/*
 * Functions that control thread behaviour
 */

#include <pthread.h>
#include <sys/time.h>
#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/port/Lock.h"
#include "ibmras/common/port/Semaphore.h"
#include "ibmras/common/logging.h"
#include <semaphore.h>
#include <errno.h>
#include <sched.h>
#include <unistd.h>

namespace ibmras{
namespace common{
namespace port {

IBMRAS_DEFINE_LOGGER("Port");

void* wrapper(void *params) {
	IBMRAS_DEBUG(fine,"in thread.cpp->wrapper");
	ThreadData* data = reinterpret_cast<ThreadData*>(params);
	return data->getCallback()(data);
}

uintptr_t createThread(ThreadData* data) {
	IBMRAS_DEBUG(fine,"in thread.cpp->createThread");
	pthread_t thread;

	return pthread_create(&thread, NULL, wrapper, data);
}

void exitThread(void *val) {
	pthread_exit(NULL);
}

void plsleep(uint32 seconds) {
	sleep(seconds);
}

Lock::Lock() {
	lock = new pthread_mutex_t;		/* create a new lock fpr this class */

	pthread_mutex_init(reinterpret_cast<pthread_mutex_t*>(lock), NULL);

	if(!lock) {
		IBMRAS_DEBUG(warning,"Failed to create lock");
		lock = NULL;				/* reset lock so that we won't try and release it when the class is destroyed */
	}
}

/* acquire a pthread mutex */
int Lock::acquire() {
	if(lock) {

		return pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(lock));

	} else {
		IBMRAS_DEBUG(warning,"Attempted to acquire a previously failed lock");
		return LOCK_FAIL;
	}
}

/* release the mutex */
int Lock::release() {
	if(lock) {
		return pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(lock));
	} else {
		IBMRAS_DEBUG(warning,"Attempted to release a previously failed lock");
		return LOCK_FAIL;
	}
}

void Lock::destroy() {
	if(lock) {
		pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t*>(lock));
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
	handle = new sem_t;
	IBMRAS_DEBUG(fine,"in thread.cpp creating CreateSemaphoreA");
	int result;
	result = sem_init(reinterpret_cast<sem_t*>(handle), 0, initial);
	if(result) {
		IBMRAS_DEBUG(warning,"Failed to create semaphore : error code %d", result);
		handle = NULL;
	}
}

void Semaphore::inc() {
	IBMRAS_DEBUG(fine,"Incrementing semaphore ticket count");
	if(handle) {
		sem_post(reinterpret_cast<sem_t*>(handle));
	}
}

bool Semaphore::wait(uint32 timeout) {
	int result;
	struct timespec t;
	while(!handle) {
		plsleep(timeout);		/* wait for the semaphore to be established */
	}
	clock_gettime(CLOCK_REALTIME, &t);
	t.tv_sec++;		/* configure the sleep interval */
	result = sem_timedwait(reinterpret_cast<sem_t*>(handle), &t);
	if(!result) {
		return true;
	}
	return (errno != ETIMEDOUT);
}

Semaphore::~Semaphore() {
	sem_destroy(reinterpret_cast<sem_t*>(handle));
	delete (sem_t*)handle;
}
}
}
}		/* end namespace port */


