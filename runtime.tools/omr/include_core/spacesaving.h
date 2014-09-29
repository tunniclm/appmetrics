/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2001, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/
#if !defined(SPACESAVING_H_)
#define SPACESAVING_H_
#include "ranking.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	J9Ranking * ranking;
	J9PortLibrary * portLib;
} J9SpaceSaving;

J9SpaceSaving * spaceSavingNew(J9PortLibrary *portLibrary, U_32 size);
void spaceSavingFree(J9SpaceSaving * spaceSaving);
void spaceSavingUpdate(J9SpaceSaving * spaceSaving, void *data, UDATA count);
void spaceSavingClear(J9SpaceSaving * spaceSaving);
void * spaceSavingGetKthMostFreq(J9SpaceSaving * spaceSaving, UDATA k);
UDATA spaceSavingGetKthMostFreqCount(J9SpaceSaving * spaceSaving, UDATA k);
UDATA spaceSavingGetCurSize(J9SpaceSaving * spaceSaving);

#ifdef __cplusplus
}
#endif


#endif
