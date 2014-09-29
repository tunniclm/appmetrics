/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef j9avlflags_h
#define j9avlflags_h

/* DO NOT DIRECTLY INCLUDE THIS FILE! */
/* Include avl_api.h instead */

#define AVL_BALANCEMASK 0x3
#define AVL_BALANCED 0
#define AVL_LEFTHEAVY 1
#define AVL_RIGHTHEAVY 2

#define AVL_GETNODE(x) ((J9AVLTreeNode *)((UDATA)(x) & (~(UDATA)AVL_BALANCEMASK)))
#define AVL_SETNODE(x, node) ((x) = (J9AVLTreeNode *)(((UDATA)(x) & AVL_BALANCEMASK) | (UDATA)(node)))

#define AVL_GETBALANCE(x) ((UDATA)((x)->leftChild) & (AVL_BALANCEMASK))
#define AVL_SETBALANCE(x, bal) (((x)->leftChild) = (J9WSRP)(((UDATA)((x)->leftChild) & (~(UDATA)AVL_BALANCEMASK)) | (bal)))

#define AVL_SRP_SETNODE(field, value) (field) = (J9WSRP)(((UDATA)field & AVL_BALANCEMASK) | ((value) ? (UDATA)(((U_8 *)(value)) - (U_8 *)&(field)) : 0))
#define AVL_NNSRP_SETNODE(field, value) (field) = (J9WSRP)(((UDATA)field & AVL_BALANCEMASK) | (UDATA)(((U_8 *)(value)) - (U_8 *)&(field)))

#define AVL_SRP_SET_TO_NULL(field) (field) = (J9WSRP)(((UDATA)field & AVL_BALANCEMASK))

#define AVL_SRP_PTR_SETNODE(ptr, value) AVL_SRP_SETNODE(*((J9WSRP *) (ptr)), (value))
#define AVL_NNSRP_PTR_SETNODE(ptr, value) AVL_NNSRP_SETNODE(*((J9WSRP *) (ptr)), (value))

#define AVL_SRP_GETNODE(node) ((J9AVLTreeNode *)(AVL_GETNODE(node) ? ((((U_8 *) &(node)) + (J9WSRP)(AVL_GETNODE(node)))) : NULL))
#define AVL_NNSRP_GETNODE(node) ((J9AVLTreeNode *) (((U_8 *) &(node)) + (J9WSRP)(AVL_GETNODE(node))))

#define J9AVLTREE_ACTION_INSERT  1
#define J9AVLTREE_ACTION_INSERT_EXISTS  2
#define J9AVLTREE_ACTION_REMOVE  3
#define J9AVLTREE_ACTION_REMOVE_NOT_IN_TREE  4
#define J9AVLTREE_ACTION_SINGLE_ROTATE  5
#define J9AVLTREE_ACTION_DOUBLE_ROTATE  6
#define J9AVLTREE_ACTION_REPLACE_REMOVED_PARENT  7

#define J9AVLTREE_IS_SHARED_TREE  1
#define J9AVLTREE_SHARED_TREE_INITIALIZED  2
#define J9AVLTREE_DISABLE_SHARED_TREE_UPDATES  4
#define J9AVLTREE_TEST_INTERNAVL  8
#define J9AVLTREE_DO_VERIFY_TREE_STRUCT_AND_ACCESS  16

#endif /* j9avlflags_h */
