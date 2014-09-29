/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef pool_api_h
#define pool_api_h

#include "j9nongenerated.h"

#if (defined(HARDHAT) && defined(ARMGNU))
#define MIN_GRANULARITY		sizeof(long long)	/* default structure alignment */
#define MALLOC_ALIGNMENT	sizeof(UDATA)	/* alignment enforced by malloc() */
#else
#define MIN_GRANULARITY		sizeof(UDATA)
#define MALLOC_ALIGNMENT	sizeof(UDATA)
#endif

/* read this if a port library call becomes available that gives out the OS page size */
#if 0
#define OS_PAGE_SIZE		(EsGetAddressSpacePageSize())
#else
#ifdef J9VM_SIZE_SMALL_RAM
#define OS_PAGE_SIZE 1024
#else
#define OS_PAGE_SIZE		4096
#endif
#endif

/**
* @file pool_api.h
* @brief Public API for the POOL module.
*
* This file contains public function prototypes and
* type definitions for the POOL module.
*
*/

#include "j9comp.h"
#include "j9pool.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------- pool.c ---------------- */


/**
* @brief
* @param aPool
* @return void
*/
void 
pool_clear(J9Pool * aPool);


/**
* @brief
* @param aPool
* @param aFunction
* @param userData
* @return void
*/
void 
pool_do(J9Pool *aPool, void (*aFunction) (void *anElement, void *userData), void *userData);

/**
* @brief
* @param aPool
* @return void
*/
void 
pool_kill(J9Pool *aPool);

/**
* @brief
* @param structSize
* @param minNumberElements
* @param elementAlignment
* @param poolFlags
* @param[in] creatorCallSite location of the function creating the pool
* @param[in] memoryCategory memory category
* @param void*(*memAlloc)(void*,U_32)
* @param void(*memFree)(void*,void*)
* @param userData
* @return J9Pool*
*/
J9Pool* 
pool_new(UDATA structSize,
		 UDATA minNumberElements,
		 UDATA elementAlignment,
		 UDATA poolFlags,
		 const char* poolCreatorCallsite,
		 U_32 memoryCategory,
		 j9memAlloc_fptr_t memAlloc,
		 j9memFree_fptr_t memFree,
		 void * userData);

/**
* @brief
* @param aPool
* @return void *
*/
void * 
pool_newElement(J9Pool * aPool);


/**
* @brief
* @param *lastHandle
* @return void*
*/
void* 
pool_nextDo(pool_state *lastHandle);


/**
* @brief
* @param *aPool
* @return UDATA
*/
UDATA 
pool_numElements(J9Pool *aPool);


/**
* @brief
* @param *aPool
* @param *anElement
* @return void
*/
void 
pool_removeElement(J9Pool *aPool, void *anElement);


/**
* @brief
* @param *aPool
* @return J9PoolPuddle
*/
J9PoolPuddle* 
poolPuddle_new(J9Pool *aPool);

/**
* @brief
* @param *aPool
* @param *lastHandle
* @return void*
*/
void* 
pool_startDo(J9Pool *aPool, pool_state *lastHandle);

/**
* @brief
* @param *aPool
* @param *currentPuddle
* @param *lastHandle
* @param followNextPointers
* @return void*
*/
void* 
poolPuddle_startDo(J9Pool *aPool, J9PoolPuddle *currentPuddle, pool_state *lastHandle, UDATA followNextPointers);

/* ---------------- pool_cap.c ---------------- */

/**
* @brief
* @param *aPool
* @return UDATA
*/
UDATA 
pool_capacity(J9Pool *aPool);


/**
* @brief
* @param *aPool
* @param newCapacity
* @return UDATA
*/
UDATA 
pool_ensureCapacity(J9Pool *aPool, UDATA newCapacity);


/**
* @brief
* @param *aPool
* @return UDATA
*/
UDATA 
pool_includesElement(J9Pool *aPool, void *anElement);


void*
pool_portLibAlloc(J9PortLibrary *portLibrary, U_32 size, const char *callSite, U_32 memoryCategory, U_32 type, U_32 *doInit);

void
pool_portLibFree(J9PortLibrary *portLibrary, void *address, U_32 type);

#if defined(J9VM_ENV_DATA64)

/**
 * @brief
 * @param *portLibrary
 * @param *doInit
 * @param size
 * @param *callSite
 * @return void *
 */
void*
pool_portLibAlloc32(J9PortLibrary *portLibrary, U_32 size, const char *callSite, U_32 memoryCategory, U_32 type, U_32 *doInit);


/**
 * @brief
 * @param *portLibrary
 * @return void
 */
void
pool_portLibFree32(J9PortLibrary *portLibrary, void *address, U_32 type);

#endif

#ifdef __cplusplus
}
#endif

#endif /* pool_api_h */
