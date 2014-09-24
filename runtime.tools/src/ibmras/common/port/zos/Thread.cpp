/*
 * thread.cpp
 *
 */


/*
 * Functions that control thread behaviour
 */

#pragma longname
#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1
#define _OPEN_THREADS
#include <sys/types.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/port/Lock.h"
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
	sleep(seconds); /* configure the sleep interval */
}

Lock::Lock() {
	lock = new pthread_mutex_t;		/* create a new lock fpr this class */
	pthread_mutex_t* mutex = reinterpret_cast<pthread_mutex_t*>(lock);
	pthread_mutex_init(mutex, NULL);
	if(!lock) {
		IBMRAS_DEBUG(warning, "Failed to create lock");
		lock = NULL;				/* reset lock so that we won't try and release it when the class is destroyed */
	} else {
		IBMRAS_DEBUG_1(finest, "Mutex created %p", lock);
	}
}

/* aquire a pthread mutex */
int Lock::acquire() {

	if(lock) {
		return pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(lock));
	} else {
		IBMRAS_DEBUG(warning, "Attempted to acquire a previously failed lock");
		return LOCK_FAIL;
	}
}

/* release the mutex */
int Lock::release() {

	if(lock) {
		return pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(lock));
	} else {
		IBMRAS_DEBUG(warning, "Attempted to release a previously failed lock");
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

int initsem(key_t key, int nsems)
{
   int semid;

    semid = semget(key, nsems, IPC_CREAT | IPC_EXCL | 0666);
     if (semid == -1 && errno == EEXIST) {
         semid = semget(key, nsems, 0);
         if (semid < 0)
         {
                 return semid;
         }

    } else if(semid == -1) {
        return semid;
    }

    return semid;
}

int sem_initialize(int *semid, int value)
{
 int ret = semctl(*semid,0,SETVAL,value);
 return ret;
}

int sem_init(int *sem,int pshared,unsigned int value)
{
  int ret = -1;
  key_t key = 5600;
  time_t seconds;
  seconds = time(NULL);
  if(value > INT_MAX){
     errno = EINVAL;
     return ret;
   }
  key = seconds;
  if((*sem = initsem(key,1)) == -1){
       return -1;
  }
  else
     ret = sem_initialize(sem,value);
  if(ret == -1)
  {
     return -1;
  }
  return ret;
}

int sem_destroy(int *semid)
{
  int ret = semctl(*semid,0,IPC_RMID);
  if(ret == -1)
  {
    return -1;
   }
  return ret;
}

int sem_post(int *semid)
{
        struct sembuf sb;
        sb.sem_num=0;
        sb.sem_op=1;
        sb.sem_flg=0;
        if (semop(*semid, &sb, 1) == -1) {
             return -1;
        }
        return 0;
}

int sem_wait(int *semid)
{
        struct sembuf sb;
        sb.sem_num=0;
        sb.sem_op=-1;
        sb.sem_flg=0;
        if (semop(*semid, &sb, 1) == -1) {
           return -1;
        }
        return 0;
}

Semaphore::Semaphore(uint32 initial, uint32 max) {
	handle = new int*;
	int result;
	result = sem_init(reinterpret_cast<int*>(handle), 0, initial);
	if(result) {
		IBMRAS_DEBUG_1(warning, "Failed to create semaphore : error code %d", result);
		handle = NULL;
	}
}

void Semaphore::inc() {
	IBMRAS_DEBUG(finest, "Incrementing semaphore ticket count");
	if(handle) {
		sem_post(reinterpret_cast<int*>(handle));
	}
}

bool Semaphore::wait(uint32 timeout) {
	int result;
	struct timespec t;
	while(!handle) {
		sleep(timeout);		/* wait for the semaphore to be established */
	}

	struct rusage *r = NULL;

	getrusage(RUSAGE_SELF, r);

	t.tv_sec = r->ru_utime.tv_sec;
	t.tv_sec++;		/* configure the sleep interval */
	result = sem_wait(reinterpret_cast<int*>(handle));
	//result = sem_timedwait(reinterpret_cast<sem_t*>(handle), &t);
	if(!result) {
		return true;
	}
	return (errno != ETIMEDOUT);
}

Semaphore::~Semaphore() {
	sem_destroy(reinterpret_cast<int*>(handle));
	delete (int*)handle;
}

}		/* end namespace port */

}
}
