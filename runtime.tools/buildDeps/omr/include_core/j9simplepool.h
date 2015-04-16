/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef J9SIMPLEPOOL_H
#define J9SIMPLEPOOL_H

#ifdef __cplusplus
extern "C" {
#endif


/*		DO NOT DIRECTLY INCLUDE THIS FILE 	*/
/*		Include simplepool_api.h instead					*/


#include "j9nongenerated.h"

typedef struct J9SimplePoolFreeList {
    J9SRP next;
    J9SRP simplePool;
} J9SimplePoolFreeList;

typedef struct J9SimplePool {
    uint32_t numElements;
    uint32_t elementSize;
    J9SRP freeList;
    J9SRP firstFreeSlot;
    J9SRP blockEnd;
    uint32_t flags;
} J9SimplePool;

#undef J9SRP

#ifdef __cplusplus
}
#endif

#endif /* J9SIMPLEPOOL_H */
