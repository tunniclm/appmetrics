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

#define J9SRP int32_t
#define J9WSRP intptr_t

#include "j9comp.h"
#include "j9port.h"

typedef void* (*j9memAlloc_fptr_t)(void*,uint32_t, const char *, uint32_t, uint32_t, uint32_t*);
typedef void (*j9memFree_fptr_t)(void*,void *, uint32_t);

#define POOL_FOR_PORT(portLib) (j9memAlloc_fptr_t)pool_portLibAlloc, (j9memFree_fptr_t)pool_portLibFree, portLib
#if defined(J9VM_ENV_DATA64)
#define POOL_FOR_PORT_PUDDLE32(portLib) (j9memAlloc_fptr_t)pool_portLibAlloc32, (j9memFree_fptr_t)pool_portLibFree32, portLib
#else
#define POOL_FOR_PORT_PUDDLE32(portLib) NULL, NULL, NULL
#endif

typedef struct J9PoolPuddleList {
    uintptr_t numElements;
    J9WSRP nextPuddle;
    J9WSRP nextAvailablePuddle;
} J9PoolPuddleList;

typedef struct J9PoolPuddle {
    uintptr_t usedElements;
    J9SRP firstElementAddress;
    J9SRP firstFreeSlot;
    J9WSRP prevPuddle;
    J9WSRP nextPuddle;
    J9WSRP prevAvailablePuddle;
    J9WSRP nextAvailablePuddle;
    uintptr_t userData;
    uintptr_t flags;
} J9PoolPuddle;


#define PUDDLE_KILLED  4
#define PUDDLE_ACTIVE  2

typedef struct J9Pool {
    uintptr_t elementSize;
    uintptr_t elementsPerPuddle;
    uintptr_t puddleAllocSize;
    J9WSRP puddleList;
    void*  ( *memAlloc)(void* userData, uint32_t byteAmount, const char* callsite, uint32_t memoryCategory, uint32_t type, uint32_t* doInit) ;
    void  ( *memFree)(void* userData, void* ptr, uint32_t type) ;
    const char* poolCreatorCallsite;
    void* userData;
    uint16_t alignment;
    uint16_t flags;
    uint32_t memoryCategory;
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
    uintptr_t leftToDo;
    struct J9Pool* thePool;
    struct J9PoolPuddle* currentPuddle;
    int32_t lastSlot;
    uintptr_t flags;
} J9PoolState;


#define POOLSTATE_FOLLOW_NEXT_POINTERS  1

#define pool_state J9PoolState
#undef J9SRP
#undef J9WSRP

#ifdef __cplusplus
}
#endif

#endif /* J9POOL_H */
