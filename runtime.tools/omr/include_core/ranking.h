/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2001, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/
#if !defined(RANKING_H_)
#define RANKING_H_
#include "hashtable_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rankTableEntry rankTableEntry;
typedef struct J9Ranking J9Ranking;
typedef struct hashTableEntry hashTableEntry;

struct J9Ranking
{
	U_32 size;
	U_32 curSize;
	rankTableEntry * rankTable;
	J9PortLibrary * portLib;
	J9HashTable * hashTable;
};

/*
 * Create new ranking data structure which organizes keys based on some count value.  The keys
 * are organized by some sorting based on their rank in such a way that we can access that key's count
 * in O(1) time.   Updates of count values are also quick (increments of count values should not require
 * much time).
 *
 * TODO: rewrite so we can accept something other than pointer values as keys
 *
 * @param portLibrary the port library
 * @param size number of entries to have in the ranking table
 * @return pointer to new ranking data structure
 */
J9Ranking* rankingNew(J9PortLibrary *portLibrary, U_32 size);
void rankingFree(J9Ranking * ranking);

/* resets all entries in the ranking data structure */
void rankingClear(J9Ranking * ranking);

/* Get the lowest count value in the data structure*/
UDATA rankingGetLowestCount(J9Ranking * ranking);

/* Replace the entry with the lowest count value with the given key and count value
 * @param key to replace lowest entry with
 * @param count count to replace lowest entry wtih
 */
void rankingUpdateLowest(J9Ranking * ranking, void * key, UDATA count);

/* Increment an entry by count
 * @param key the entry to increment
 * @param count how much to increment by
 * @return returns TRUE if an entry with the given key exists(who's count is then increment), false otherwise
 * */
UDATA rankingIncrementEntry(J9Ranking * ranking, void * key, UDATA count);

/* get the key at rank k
 * @param k the kth highest count we're enquiring about
 * @return the key at rank k
 */
void * rankingGetKthHighest(J9Ranking * ranking, UDATA k);

/* get the count at rank k
 * @param k the kth highest count we're enquiring about
 * @return the count at rank k
 */
UDATA rankingGetKthHighestCount(J9Ranking * ranking, UDATA k);

#ifdef __cplusplus
}
#endif

#endif
