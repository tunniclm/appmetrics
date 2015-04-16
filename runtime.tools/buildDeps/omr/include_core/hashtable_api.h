/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef hashtable_api_h
#define hashtable_api_h

/**
* @file hashtable_api.h
* @brief Public API for the HASHTABLE module.
*
* This file contains public function prototypes and
* type definitions for the HASHTABLE module.
*
*/

#include "j9hashtable.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------- hashtable.c ---------------- */

/**
* @brief
* @param *table
* @param *entry
* @return void *
*/
void *
hashTableAdd(J9HashTable *table, void *entry);


/**
* @brief
* @param *handle
* @return uintptr_t
*/
uintptr_t
hashTableDoRemove(J9HashTableState *handle);


/**
* @brief
* @param *table
* @return void
*/
void
hashTableDumpDistribution(J9HashTable *table);


/**
* @brief
* @param *table
* @param *entry
* @return void *
*/
void *
hashTableFind(J9HashTable *table, void *entry);


/**
* @brief
* @param *table
* @param doFn
* @param *opaque
* @return void
*/
void
hashTableForEachDo(J9HashTable *table, J9HashTableDoFn doFn, void *opaque);


/**
* @brief
* @param *table
* @return void
*/
void
hashTableFree(J9HashTable *table);


/**
* @brief
* @param *table
* @return uint32_t
*/
uint32_t
hashTableGetCount(J9HashTable *table);


/**
* @brief
* @param *portLibrary
* @param *tableName
* @param tableSize
* @param entrySize
* @param entryAlignment
* @param hashFn
* @param hashEqualFn
* @param printFn
* @param *functionUserData
* @return J9HashTable *
*/
J9HashTable *
hashTableNew(
	J9PortLibrary *portLibrary,
	const char *tableName,
	uint32_t tableSize,
	uint32_t entrySize,
	uint32_t entryAlignment,
	uint32_t flags,
	uint32_t memoryCategory,
	J9HashTableHashFn hashFn, 
	J9HashTableEqualFn hashEqualFn,
	J9HashTablePrintFn printFn,
	void *functionUserData);

/**
* @param portLibrary  The port library
* @param tableName   A string giving the name of the table
* @param tableSize   Initial number of hash table nodes (if zero, use a suitable default)
* @param entrySize   Size of the user-data for each node
* @param flags	Optional flags for extra options
* @param memoryCategory  memory category for which memory allocated by hashtable should use
* @param listToTreeThreshold  The threshold after which list chains are transformed into AVL trees
* @param hashFn  Mandatory hashing function ptr
* @param printFn  Optional node-print function ptr
* @param searchComparator  Mandatory comparison function required for tree comparison
* @param userData  Optional userData ptr to be passed to hashFn and hashEqualFn
* @return  An initialized hash table
*/
J9HashTable *
collisionResilientHashTableNew(
	J9PortLibrary *portLibrary,
	const char *tableName,
	uint32_t tableSize,
	uint32_t entrySize,
	uint32_t flags,
	uint32_t memoryCategory,
	uint32_t listToTreeThreshold,
	J9HashTableHashFn hashFn,
	J9HashTableComparatorFn comparatorFn,
	J9HashTablePrintFn printFn,
	void *functionUserData);

/**
* @brief
* @param *handle
* @return void  *
*/
void  *
hashTableNextDo(J9HashTableState *handle);


/**
* @brief
* @param *table
* @return void
*/
void
hashTableRehash(J9HashTable *table);


/**
* @brief
* @param *table
* @param *entry
* @return uint32_t
*/
uint32_t
hashTableRemove(J9HashTable *table, void *entry);


/**
* @brief
* @param *table
* @param *handle
* @return void *
*/
void *
hashTableStartDo(J9HashTable *table,  J9HashTableState *handle);



#ifdef __cplusplus
}
#endif

#endif /* hashtable_api_h */

