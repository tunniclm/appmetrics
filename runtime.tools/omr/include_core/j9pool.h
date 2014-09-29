/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

/**
 * @file
 * @ingroup Pool
 * @brief Pool Header
 */

#ifndef J9POOL_H
#define J9POOL_H

#ifdef __cplusplus
extern "C" {
#endif


/*		DO NOT DIRECTLY INCLUDE THIS FILE 	*/
/*		Include pool_api.h instead					*/

#define J9SRP I_32
#define J9WSRP IDATA

#include "j9comp.h"
#include "j9port.h"

typedef void* (*j9memAlloc_fptr_t)(void*,U_32, const char *, U_32, U_32, U_32*);
typedef void (*j9memFree_fptr_t)(void*,void *, U_32);

#define POOL_FOR_PORT(portLib) (j9memAlloc_fptr_t)pool_portLibAlloc, (j9memFree_fptr_t)pool_portLibFree, portLib
#if defined(J9VM_ENV_DATA64)
#define POOL_FOR_PORT_PUDDLE32(portLib) (j9memAlloc_fptr_t)pool_portLibAlloc32, (j9memFree_fptr_t)pool_portLibFree32, portLib
#else
#define POOL_FOR_PORT_PUDDLE32(portLib) NULL, NULL, NULL
#endif

typedef struct J9PoolPuddleList {
    UDATA numElements;
    J9WSRP nextPuddle;
    J9WSRP nextAvailablePuddle;
} J9PoolPuddleList;

typedef struct J9PoolPuddle {
    UDATA usedElements;
    J9SRP firstElementAddress;
    J9SRP firstFreeSlot;
    J9WSRP prevPuddle;
    J9WSRP nextPuddle;
    J9WSRP prevAvailablePuddle;
    J9WSRP nextAvailablePuddle;
    UDATA userData;
    UDATA flags;
} J9PoolPuddle;


#define PUDDLE_KILLED  4
#define PUDDLE_ACTIVE  2

typedef struct J9Pool {
    UDATA elementSize;
    UDATA elementsPerPuddle;
    UDATA puddleAllocSize;
    J9WSRP puddleList;
    void*  ( *memAlloc)(void* userData, U_32 byteAmount, const char* callsite, U_32 memoryCategory, U_32 type, U_32* doInit) ;
    void  ( *memFree)(void* userData, void* ptr, U_32 type) ;
    const char* poolCreatorCallsite;
    void* userData;
    U_16 alignment;
    U_16 flags;
    U_32 memoryCategory;
} J9Pool;


#define POOL_NO_ZERO  8
#define POOL_ROUND_TO_PAGE_SIZE  16
#define POOL_USES_HOLES  32
#define POOL_NEVER_FREE_PUDDLES  2
#define POOL_ALLOC_TYPE_PUDDLE  1
#define POOL_ALWAYS_KEEP_SORTED  4
#define POOL_ALLOC_TYPE_PUDDLE_LIST  2
#define POOL_ALLOC_TYPE_POOL  0

typedef struct J9PoolState {
    UDATA leftToDo;
    struct J9Pool* thePool;
    struct J9PoolPuddle* currentPuddle;
    I_32 lastSlot;
    UDATA flags;
} J9PoolState;


#define POOLSTATE_FOLLOW_NEXT_POINTERS  1

#define pool_state J9PoolState
#undef J9SRP
#undef J9WSRP

#ifdef __cplusplus
}
#endif

#endif /* J9POOL_H */
