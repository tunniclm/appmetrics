/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef j9nongenerated_h
#define j9nongenerated_h

#include "j9comp.h"
#include "j9port.h"

typedef int32_t J9SRP;
#ifndef NNSRP_GET
#define NNSRP_GET(field, type) ((type) (((uint8_t *) &(field)) + (J9SRP)(field)))
#endif
#ifndef SRP_GET
#define SRP_GET(field, type) ((type) ((field) ? NNSRP_GET(field, type) : NULL))
#endif
#define NNSRP_SET(field, value) (field) = (J9SRP) (((uint8_t *) (value)) - (uint8_t *) &(field))
#define SRP_SET(field, value) (field) = (J9SRP) ((value) ? (((uint8_t *) (value)) - (uint8_t *) &(field)) : 0)
#define SRP_SET_TO_NULL(field) (field) = 0
#ifndef SRP_PTR_GET
#define SRP_PTR_GET(ptr, type) SRP_GET(*((J9SRP *) (ptr)), type)
#endif
#define SRP_PTR_SET(ptr, value) SRP_SET(*((J9SRP *) (ptr)), (value))
#define SRP_PTR_SET_TO_NULL(ptr) SRP_SET_TO_NULL(*((J9SRP *) (ptr)))
#define NNSRP_PTR_GET(ptr, type) NNSRP_GET(*((J9SRP *) (ptr)), type)
#define NNSRP_PTR_SET(ptr, value) NNSRP_SET(*((J9SRP *) (ptr)), (value))


typedef intptr_t J9WSRP;
#ifndef NNWSRP_GET
#define NNWSRP_GET(field, type) ((type) (((uint8_t *) &(field)) + (J9WSRP)(field)))
#endif
#ifndef WSRP_GET
#define WSRP_GET(field, type) ((type) ((field) ? NNWSRP_GET(field, type) : NULL))
#endif
#define NNWSRP_SET(field, value) (field) = (J9WSRP) (((uint8_t *) (value)) - (uint8_t *) &(field))
#define WSRP_SET(field, value) (field) = (J9WSRP) ((value) ? (((uint8_t *) (value)) - (uint8_t *) &(field)) : 0)
#define WSRP_SET_TO_NULL(field) (field) = 0
#ifndef WSRP_PTR_GET
#define WSRP_PTR_GET(ptr, type) WSRP_GET(*((J9WSRP *) (ptr)), type)
#endif
#define WSRP_PTR_SET(ptr, value) WSRP_SET(*((J9WSRP *) (ptr)), (value))
#define WSRP_PTR_SET_TO_NULL(ptr) SRP_SET_TO_NULL(*((J9WSRP *) (ptr)))
#define NNWSRP_PTR_GET(ptr, type) NNWSRP_GET(*((J9WSRP *) (ptr)), type)
#define NNWSRP_PTR_SET(ptr, value) NNWSRP_SET(*((J9WSRP *) (ptr)), (value))

/* NOTE: Use j9generated.h unless you receive different guidance */

typedef struct J9AVLTreeNode {
    J9WSRP leftChild;
    J9WSRP rightChild;
} J9AVLTreeNode;

typedef struct J9AVLTree {
    intptr_t  (*insertionComparator)(struct J9AVLTree *tree, struct J9AVLTreeNode *insertNode, struct J9AVLTreeNode *walkNode) ;
    intptr_t  (*searchComparator)(struct J9AVLTree *tree, uintptr_t searchValue, struct J9AVLTreeNode *node) ;
    void  (*genericActionHook)(struct J9AVLTree *tree, struct J9AVLTreeNode *node, uintptr_t action) ;
    uintptr_t  (*performNodeAction)(struct J9AVLTree *tree, struct J9AVLTreeNode *node, uintptr_t action, void* userData) ;
    uintptr_t flags;
    struct J9AVLTreeNode* rootNode;
    struct J9PortLibrary* portLibrary;
    void* userData;
} J9AVLTree;

#define J9AVLTREENODE_LEFTCHILD(base) AVL_SRP_GETNODE((base)->leftChild)
#define J9AVLTREENODE_RIGHTCHILD(base) AVL_SRP_GETNODE((base)->rightChild)

typedef struct J9MemorySegment {
	J9AVLTreeNode parentAVLTreeNode;
    uintptr_t type;
    uintptr_t size;
    uint8_t* baseAddress;
    uint8_t* heapBase;
    uint8_t* heapTop;
    uint8_t* heapAlloc;
    struct J9MemorySegment* nextSegment;
    struct J9MemorySegment* previousSegment;
    struct J9MemorySegmentList* memorySegmentList;
    uintptr_t unused1;
    struct J9ClassLoader* classLoader;
    void* memorySpace;
    struct J9MemorySegment* nextSegmentInClassLoader;
    struct J9PortVmemIdentifier vmemIdentifier;
} J9MemorySegment;

#define MEMORY_TYPE_OLD  1
#define MEMORY_TYPE_NEW_RAM  10
#define MEMORY_TYPE_SCOPED  0x2000
#define MEMORY_TYPE_ALLOCATED  64
#define MEMORY_TYPE_IMMORTAL  0x1000
#define MEMORY_TYPE_DEBUG_INFO  0x200
#define MEMORY_TYPE_BASETYPE_ROM_CLASS  0x200000
#define MEMORY_TYPE_DYNAMIC_LOADED_CLASSES  0x20040
#define MEMORY_TYPE_NEW  2
#define MEMORY_TYPE_DISCARDABLE  0x80
#define MEMORY_TYPE_NUMA  0x4000
#define MEMORY_TYPE_ROM_CLASS  0x20000
#define MEMORY_TYPE_UNCOMMITTED  0x800
#define MEMORY_TYPE_FROM_JXE  0x4000000
#define MEMORY_TYPE_OLD_ROM  5
#define MEMORY_TYPE_SHARED_META  0x8000000
/* MEMORY_TYPE_VIRTUAL is expected to be used along with other types like MEMORY_TYPE_JIT_SCRATCH_SPACE
 * or MEMORY_TYPE_JIT_PERSISTENT to allocate virtual memory instead of malloc'ed memory.
 */
#define MEMORY_TYPE_VIRTUAL  0x400
/* MEMORY_TYPE_FIXED_RAM_CLASS is virtually allocated, setting MEMORY_TYPE_VIRTUAL is not required. */
#define MEMORY_TYPE_FIXED_RAM_CLASS  0x8000
#define MEMORY_TYPE_RAM_CLASS  0x10000
#define MEMORY_TYPE_IGC_SCAN_QUEUE  0x400000
#define MEMORY_TYPE_RAM  8
#define MEMORY_TYPE_FIXED  16
#define MEMORY_TYPE_JIT_SCRATCH_SPACE  0x1000000
#define MEMORY_TYPE_FIXED_RAM  24
#define MEMORY_TYPE_OLD_RAM  9
/* MEMORY_TYPE_CODE is used for virtually allocated JIT code segments, setting MEMORY_TYPE_VIRTUAL is not required. */
#define MEMORY_TYPE_CODE  32
#define MEMORY_TYPE_ROM  4
#define MEMORY_TYPE_CLASS_FILE_BYTES  0x40000
#define MEMORY_TYPE_UNDEAD_CLASS  0x80000
#define MEMORY_TYPE_JIT_PERSISTENT  0x800000
#define MEMORY_TYPE_FIXEDSIZE  0x100
#define MEMORY_TYPE_DEFAULT  0x2000000

#define J9MEMORYSEGMENT_LEFTCHILD(base) AVL_SRP_GETNODE((base)->parentAVLTreeNode.leftChild)
#define J9MEMORYSEGMENT_RIGHTCHILD(base) AVL_SRP_GETNODE((base)->parentAVLTreeNode.rightChild)

struct J9Pool;
typedef struct J9MemorySegmentList {
    struct J9Pool* segmentPool;
    struct J9MemorySegment* nextSegment;
    uintptr_t totalSegmentSize;
    j9thread_monitor_t segmentMutex;
    struct J9AVLTree avlTreeData;
    uintptr_t flags;
} J9MemorySegmentList;

/* NOTE: JIT HashTable (and walk state) likely need to be pushed into the JIT side of jvm/jit compilation */
typedef struct J9JITHashTable {
	J9AVLTreeNode parentAVLTreeNode;
    uintptr_t* buckets;
    uintptr_t start;
    uintptr_t end;
    uintptr_t flags;
    uintptr_t* methodStoreStart;
    uintptr_t* methodStoreEnd;
    uintptr_t* currentAllocate;
} J9JITHashTable;

#define JIT_HASH_IN_DATA_CACHE  1

#define J9JITHASHTABLE_LEFTCHILD(base) AVL_SRP_GETNODE((base)->parentAVLTreeNode.leftChild)
#define J9JITHASHTABLE_RIGHTCHILD(base) AVL_SRP_GETNODE((base)->parentAVLTreeNode.rightChild)

typedef struct J9JITHashTableWalkState {
    struct J9JITHashTable* table;
    uintptr_t index;
    uintptr_t* bucket;
} J9JITHashTableWalkState;

/* NOTE: J9MEMAVLTreeNode should be pushed into a memcheck specific definition */
typedef struct J9MEMAVLTreeNode {
	J9AVLTreeNode parentAVLTreeNode;
    const char* callSite;
    struct J9MemoryCheckStats* stats;
    struct J9MemoryCheckStats* prevStats;
} J9MEMAVLTreeNode;

#define J9MEMAVLTREENODE_LEFTCHILD(base) AVL_SRP_GETNODE((base)->parentAVLTreeNode.leftChild)
#define J9MEMAVLTREENODE_RIGHTCHILD(base) AVL_SRP_GETNODE((base)->parentAVLTreeNode.rightChild)

#define J9POOLPUDDLE_FIRSTFREESLOT(parm) SRP_GET((parm)->firstFreeSlot, uintptr_t*)
#define J9POOLPUDDLE_FIRSTELEMENTADDRESS(parm) NNSRP_GET((parm)->firstElementAddress, void*)
#define J9POOLPUDDLE_PREVPUDDLE(parm) WSRP_GET((parm)->prevPuddle, J9PoolPuddle*)
#define J9POOLPUDDLE_NEXTPUDDLE(parm) WSRP_GET((parm)->nextPuddle, J9PoolPuddle*)
#define J9POOLPUDDLE_NEXTAVAILABLEPUDDLE(parm) WSRP_GET((parm)->nextAvailablePuddle, J9PoolPuddle*)
#define J9POOLPUDDLE_PREVAVAILABLEPUDDLE(parm) WSRP_GET((parm)->prevAvailablePuddle, J9PoolPuddle*)
#define J9POOL_PUDDLELIST(pool) NNWSRP_GET((pool)->puddleList, J9PoolPuddleList*)
#define J9POOLPUDDLELIST_NEXTPUDDLE(parm) NNWSRP_GET((parm)->nextPuddle, J9PoolPuddle*)
#define J9POOLPUDDLELIST_NEXTAVAILABLEPUDDLE(parm) WSRP_GET((parm)->nextAvailablePuddle, J9PoolPuddle*)

typedef struct J9CmdLineMapping {
    char* j9Name;
    char* mapName;
    uintptr_t flags;
} J9CmdLineMapping;

typedef struct J9CmdLineOption {
    struct J9CmdLineMapping* mapping;
    uintptr_t flags;
    char* fromEnvVar;
} J9CmdLineOption;

/*TODO Temporary duplicate of J9VMGCSublistFragment from j9generated.h */

typedef struct J9VMGC_SublistFragment {
    uintptr_t* fragmentCurrent;
    uintptr_t* fragmentTop;
    uintptr_t fragmentSize;
    void* parentList;
    uintptr_t deferredFlushID;
    uintptr_t count;
} J9VMGC_SublistFragment;

#define J9SIZEOF_J9VMGC_SublistFragment 48


#define J9MMCONSTANT_IMPLICIT_GC_DEFAULT  0
#define J9MMCONSTANT_EXPLICIT_GC_NOT_AGGRESSIVE  1
#define J9MMCONSTANT_EXPLICIT_GC_RASDUMP_COMPACT  2
#define J9MMCONSTANT_EXPLICIT_GC_EXCLUSIVE_VMACCESS_ALREADY_ACQUIRED  2
#define J9MMCONSTANT_EXPLICIT_GC_SYSTEM_GC  3
#define J9MMCONSTANT_EXPLICIT_GC_NATIVE_OUT_OF_MEMORY  4
#define J9MMCONSTANT_IMPLICIT_GC_AGGRESSIVE  5
#define J9MMCONSTANT_IMPLICIT_GC_PERCOLATE  6
#define J9MMCONSTANT_IMPLICIT_GC_PERCOLATE_AGGRESSIVE  7
#define J9MMCONSTANT_IMPLICIT_GC_EXCESSIVE  8
#define J9MMCONSTANT_IMPLICIT_GC_PERCOLATE_UNLOADING_CLASSES  9
#define J9MMCONSTANT_IMPLICIT_GC_PERCOLATE_CRITICAL_REGIONS  10

typedef struct J9MemorySpaceDescription {
    uintptr_t oldSpaceSize;
    uintptr_t newSpaceSize;
} J9MemorySpaceDescription;

#endif /* j9nongenerated_h */
