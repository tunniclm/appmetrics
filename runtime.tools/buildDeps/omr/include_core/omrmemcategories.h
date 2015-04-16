/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2010, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef OMRMEMCATEGORIES_H
#define OMRMEMCATEGORIES_H

/*
 * To add a new category:
 * - Add a new #define to the end of this list
 * - Add the corresponding entries in VM_Sidecar/j9vm/j9memcategories.c
 *
 * Don't delete old categories. Only add new categories at the
 * end of the range. OMR uses the high categories from 0x80000000
 * upwards internally to allow language implementors to use
 * 0x0 and upwards.
 */
/* Special memory category for memory allocated for unknown categories */
#define J9MEM_CATEGORY_UNKNOWN 0x80000000
/* Special memory category for memory allocated for the port library itself */
#define J9MEM_CATEGORY_PORT_LIBRARY 0x80000001
#define J9MEM_CATEGORY_VM 0x80000002
#define J9MEM_CATEGORY_MM 0x80000003
#define J9MEM_CATEGORY_THREADS 0x80000004
#define J9MEM_CATEGORY_THREADS_RUNTIME_STACK 0x80000005
#define J9MEM_CATEGORY_THREADS_NATIVE_STACK 0x80000006
#define J9MEM_CATEGORY_TRACE 0x80000007
#define J9MEM_CATEGORY_OMRTI 0x80000008

/* Special memory category for *unused* sections of regions allocated for <32bit allocations on 64 bit.
 * The used sections will be accounted for under the categories they are used by. */
#define J9MEM_CATEGORY_PORT_LIBRARY_UNUSED_ALLOCATE32_REGIONS 0x80000009
#define J9MEM_CATEGORY_MM_RUNTIME_HEAP 0x8000000A

#define J9MEM_CATEGORY_JIT 0x8000000B
#define J9MEM_CATEGORY_JIT_CODE_CACHE 0x8000000C
#define J9MEM_CATEGORY_JIT_DATA_CACHE 0x8000000D

#if defined(J9VM_THR_FORK_SUPPORT)
#define J9MEM_CATEGORY_OSMUTEXES 0x8000000E
#define J9MEM_CATEGORY_OSCONDVARS 0x8000000F
#endif /* defined(J9VM_THR_FORK_SUPPORT) */

/* Helper macro to convert the category codes to indices starting from 0 */
#define J9MEM_LANGUAGE_CATEGORY_LIMIT 0x7FFFFFFF
#define J9MEM_OMR_CATEGORY_INDEX_FROM_CODE(code) (((uint32_t)0x7FFFFFFF) & (code))

#define J9MEM_CATEGORY_NO_CHILDREN(description, code) static J9MemCategory _j9mem_category_##code = {description, code, 0, 0, 0, 0, 0, NULL}
#define J9MEM_CATEGORY_1_CHILD(description, code, c1) static uint32_t _j9mem_##code##_child_categories[] = {c1}; static J9MemCategory _j9mem_category_##code = {description, code, 0, 0, 0, 0, 1, _j9mem_##code##_child_categories}
#define J9MEM_CATEGORY_2_CHILDREN(description, code, c1, c2) static uint32_t _j9mem_##code##_child_categories[] = {c1, c2}; static J9MemCategory _j9mem_category_##code = {description, code, 0, 0, 0, 0, 2, _j9mem_##code##_child_categories}
#define J9MEM_CATEGORY_3_CHILDREN(description, code, c1, c2, c3) static uint32_t _j9mem_##code##_child_categories[] = {c1, c2, c3}; static J9MemCategory _j9mem_category_##code = {description, code, 0, 0, 0, 0, 3, _j9mem_##code##_child_categories}
#define J9MEM_CATEGORY_4_CHILDREN(description, code, c1, c2, c3, c4) static uint32_t _j9mem_##code##_child_categories[] = {c1, c2, c3, c4}; static J9MemCategory _j9mem_category_##code = {description, code, 0, 0, 0, 0, 4, _j9mem_##code##_child_categories}
#define J9MEM_CATEGORY_5_CHILDREN(description, code, c1, c2, c3, c4, c5) static uint32_t _j9mem_##code##_child_categories[] = {c1, c2, c3, c4, c5}; static J9MemCategory _j9mem_category_##code = {description, code, 0, 0, 0, 0, 5, _j9mem_##code##_child_categories}
#define J9MEM_CATEGORY_6_CHILDREN(description, code, c1, c2, c3, c4, c5, c6) static uint32_t _j9mem_##code##_child_categories[] = {c1, c2, c3, c4, c5, c6}; static J9MemCategory _j9mem_category_##code = {description, code, 0, 0, 0, 0, 6, _j9mem_##code##_child_categories}
#define J9MEM_CATEGORY_7_CHILDREN(description, code, c1, c2, c3, c4, c5, c6, c7) static uint32_t _j9mem_##code##_child_categories[] = {c1, c2, c3, c4, c5, c6, c7}; static J9MemCategory _j9mem_category_##code = {description, code, 0, 0, 0, 0, 7, _j9mem_##code##_child_categories}

#define CATEGORY_TABLE_ENTRY(name) &_j9mem_category_##name

#endif /* OMRMEMCATEGORIES_H */
