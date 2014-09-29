/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef avl_api_h
#define avl_api_h

/**
* @file avl_api.h
* @brief Public API for the AVL module.
*
* This file contains public function prototypes and
* type definitions for the AVL module.
*
*/

#include "j9port.h"
#include "j9avldefines.h"
#include "j9avl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------- avlsup.c ---------------- */

/**
* @brief
* @param *tree
* @param *nodeToDelete
* @return J9AVLTreeNode *
*/
J9AVLTreeNode * 
avl_delete(J9AVLTree *tree, J9AVLTreeNode *nodeToDelete);


/**
* @brief
* @param *tree
* @param *nodeToInsert
* @return J9AVLTreeNode *
*/
J9AVLTreeNode * 
avl_insert(J9AVLTree *tree, J9AVLTreeNode *nodeToInsert);


/**
* @brief
* @param *tree
* @param searchValue
* @return J9AVLTreeNode *
*/
J9AVLTreeNode * 
avl_search(J9AVLTree *tree, UDATA searchValue);


#ifdef __cplusplus
}
#endif

#endif /* avl_api_h */
