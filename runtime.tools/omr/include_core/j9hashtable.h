/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef J9HASHTABLE_H
#define J9HASHTABLE_H

#ifdef __cplusplus
extern "C" {
#endif


/* DO NOT DIRECTLY INCLUDE THIS FILE! */
/* Include hashtable_api.h instead */


#include "j9avl.h"
#include "j9comp.h"
#include "j9port.h"
#include "pool_api.h"

/**
 * Hash table flags
 */
#define J9HASH_TABLE_DO_NOT_GROW	0x00000001	/*!< Do not grow & rehash the table while set */
#define J9HASH_TABLE_COLLISION_RESILIENT	0x00000002	/*!< Use hash table using avl trees for collision resolution instead of lists */
#define J9HASH_TABLE_ALLOCATE_ELEMENTS_USING_MALLOC32	0x00000004	/*!< Allocate table elements using the malloc32 function */
#define J9HASH_TABLE_ALLOW_SIZE_OPTIMIZATION	0x00000008	/*!< Allow space optimized hashTable, some functions not supported */
#define J9HASH_TABLE_DO_NOT_REHASH	0x00000010	/*!< Do not rehash the table while set */

#define J9HASH_TABLE_AVL_TREE_TAG_BIT ((UDATA)0x00000001) /*!< Bit to indicate that hastable slot contains a pointer to an AVL tree */

/**
 * Hash Table state constants for iteration
 */
#define J9HASH_TABLE_ITERATE_STATE_LIST_NODES 0
#define J9HASH_TABLE_ITERATE_STATE_TREE_NODES 1
#define J9HASH_TABLE_ITERATE_STATE_FINISHED  2

/**
 * Macros for getting at data directly from AVLTreeNodes
 */
#define AVL_NODE_TO_DATA(p) ((void *)((U_8 *)(p) + sizeof(J9AVLTreeNode)))
#define AVL_DATA_TO_NODE(p) (((J9AVLTreeNode *)((U_8 *)(p) - sizeof(J9AVLTreeNode))))

/**
 * Hash table flag macros
 */
#define hashTableCanGrow(table) (((table)->flags & J9HASH_TABLE_DO_NOT_GROW) ? 0 : 1)
#define hashTableCanRehash(table) (((table)->flags & J9HASH_TABLE_DO_NOT_REHASH) ? 0 : 1)
#define hashTableSetFlag(table,flag) ((table)->flags |= (flag))
#define hashTableResetFlag(table,flag) ((table)->flags &= ~(flag))

/**
* Hash table state queries
*/
#define hashTableIsSpaceOptimized(table) (NULL == table->listNodePool)


struct J9HashTable; /* Forward struct declaration */
struct J9AVLTreeNode; /* Forward struct declaration */
typedef UDATA (*J9HashTableHashFn) (void *entry, void *userData); /* Forward struct declaration */
typedef UDATA (*J9HashTableEqualFn) (void *leftEntry, void *rightEntry, void *userData); /* Forward struct declaration */
typedef IDATA  (*J9HashTableComparatorFn)(struct J9AVLTree *tree, struct J9AVLTreeNode *leftNode, struct J9AVLTreeNode *rightNode); /* Forward struct declaration */
typedef void (*J9HashTablePrintFn) (J9PortLibrary *portLibrary, void *entry, void *userData); /* Forward struct declaration */
typedef UDATA (*J9HashTableDoFn) (void *entry, void *userData); /* Forward struct declaration */
typedef struct J9HashTable {
    const char* tableName;
    U_32 tableSize;
    U_32 numberOfNodes;
    U_32 numberOfTreeNodes;
    U_32 entrySize;
    U_32 listNodeSize;
    U_32 treeNodeSize;
    U_32 nodeAlignment;
    U_32 flags;
    U_32 memoryCategory;
    U_32 listToTreeThreshold;
    void** nodes;
    struct J9Pool* listNodePool;
    struct J9Pool* treeNodePool;
    struct J9Pool* treePool;
    struct J9AVLTree* avlTreeTemplate;
    UDATA  ( *hashFn)(void *key, void *userData) ;
    UDATA  ( *hashEqualFn)(void *leftKey, void *rightKey, void *userData) ;
    void  ( *printFn)(J9PortLibrary *portLibrary, void *key, void *userData) ;
    struct J9PortLibrary * portLibrary;
    void* equalFnUserData;
    void* hashFnUserData;
    struct J9HashTable* previous;
} J9HashTable;

typedef struct J9HashTableState {
    struct J9HashTable* table;
    U_32 bucketIndex;
    U_32 didDeleteCurrentNode;
    void** pointerToCurrentNode;
    UDATA iterateState;
    struct J9PoolState poolState;
} J9HashTableState;

#ifdef __cplusplus
}
#endif

#endif /* J9HASHTABLE_H */
