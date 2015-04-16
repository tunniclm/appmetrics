/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#if !defined(J9GCCONSTS_H_)
#define J9GCCONSTS_H_

#define J9_GC_POLICY_BALANCED 0x4
#define J9_GC_POLICY_GENCON 0x3
#define J9_GC_POLICY_ILLEGAL 0x0
#define J9_GC_POLICY_METRONOME 0x5
#define J9_GC_POLICY_OPTAVGPAUSE 0x2
#define J9_GC_POLICY_OPTTHRUPUT 0x1

#define J9_GC_CYCLE_TYPE_DEFAULT     0
#define J9_GC_CYCLE_TYPE_GLOBAL      1
#define J9_GC_CYCLE_TYPE_SCAVENGE    2

#define J9_GC_ALLOCATE_OBJECT_NON_INSTRUMENTABLE 0x0
#define J9_GC_ALLOCATE_OBJECT_INSTRUMENTABLE 0x1
#define J9_GC_ALLOCATE_OBJECT_TENURED 0x2
#define J9_GC_ALLOCATE_OBJECT_PACKED_HEADER 0x4
#define J9_GC_ALLOCATE_OBJECT_HASHED 0x8
#define J9_GC_ALLOCATE_OBJECT_NON_ZERO_TLH 0x10

#define J9_GC_ALLOCATION_TYPE_ILLEGAL 0x0
#define J9_GC_ALLOCATION_TYPE_TLH 0x1
#define J9_GC_ALLOCATION_TYPE_SEGREGATED 0x2
#define J9_GC_ALLOCATION_TYPE_COUNT 0x3

/* Object-model related constants moved from builder (J9VMConstantValue.st / j9generated.h)
 * These replace the following constants from builder (J9VMConstantValue.st / j9consts.h):
 * #define J9_OBJECT_HEADER_AGE_MAX 0xE
 * #define J9_OBJECT_HEADER_AGE_MIN 0x1
 * Note that some related defines remain, in both j9generated.h and j9consts.h, e.g.:
 * J9_OBJECT_HEADER_AGE_DEFAULT		/ OBJECT_HEADER_AGE_DEFAULT
 * J9_OBJECT_HEADER_INDEXABLE		/ OBJECT_HEADER_INDEXABLE
 * J9_OBJECT_HEADER_SHAPE_MASK		/ OBJECT_HEADER_SHAPE_MASK
 * J9_OBJECT_HEADER_SHAPE_MASK		/ OBJECT_HEADER_SHAPE_MASK
 * J9_OBJECT_HEADER_STACK_ALLOCATED	/ OBJECT_HEADER_STACK_ALLOCATED
 * */
#define OBJECT_HEADER_AGE_MIN  1
#define OBJECT_HEADER_AGE_MAX  14

#define J9_MU_WALK_DEBUGGER_REFS 0x1
#define J9_MU_WALK_DEBUGGER_CLASS_REFS 0x2
#define J9_MU_WALK_ALL 0x3
#define J9_MU_WALK_OBJECT_BASE 0x4
#define J9_MU_WALK_SKIP_JVMTI_TAG_TABLES 0x20
#define J9_MU_WALK_TRACK_VISIBLE_FRAME_DEPTH 0x40
#define J9_MU_WALK_IGNORE_CLASS_SLOTS 0x80
#define J9_MU_WALK_NEW_AND_REMEMBERED_ONLY 0x10
#define J9_MU_WALK_IGNORE_NULL_ARRAYLET_LEAF 0x100
#define J9_MU_WALK_PREINDEX_INTERFACE_FIELDS 0x200

#endif /* J9GCCONSTS_H_ */
