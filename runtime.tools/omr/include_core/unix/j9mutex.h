/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2001, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/
#ifndef j9mutex_h
#define j9mutex_h

#include <pthread.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef pthread_mutex_t MUTEX;


/* MUTEX_INIT */

#define MUTEX_INIT(mutex) (pthread_mutex_init(&(mutex), NULL) == 0)

/* MUTEX_DESTROY */

#define MUTEX_DESTROY(mutex) pthread_mutex_destroy(&(mutex))

/* MUTEX_ENTER */

#define MUTEX_ENTER(mutex) pthread_mutex_lock(&(mutex))

/*
 *  MUTEX_TRY_ENTER 
 *  returns 0 on success
 */

#define MUTEX_TRY_ENTER(mutex) pthread_mutex_trylock(&(mutex))

/* MUTEX_EXIT */

#define MUTEX_EXIT(mutex) pthread_mutex_unlock(&(mutex))

#ifdef __cplusplus
}
#endif


#endif     /* j9mutex_h */

