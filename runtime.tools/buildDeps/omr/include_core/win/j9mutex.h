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

/* windows.h defined uintptr_t.  Ignore its definition */
#define UDATA UDATA_win32_
#include <windows.h>
#undef UDATA	/* this is safe because our UDATA is a typedef, not a macro */

typedef CRITICAL_SECTION MUTEX;

/* MUTEX_INIT */

#define MUTEX_INIT(mutex) (InitializeCriticalSection(&(mutex)), 1)

/* MUTEX_DESTROY */

#define MUTEX_DESTROY(mutex) DeleteCriticalSection(&(mutex))

/* MUTEX_ENTER */

#define MUTEX_ENTER(mutex) EnterCriticalSection(&(mutex))

/*
 *  MUTEX_TRY_ENTER 
 *  returns 0 on success
 *  Beware: you may not have support for TryEnterCriticalSection 
 */
#define MUTEX_TRY_ENTER(mutex) (!(TryEnterCriticalSection(&(mutex))))

/* MUTEX_EXIT */

#define MUTEX_EXIT(mutex) LeaveCriticalSection(&(mutex))




#endif     /* j9mutex_h */

