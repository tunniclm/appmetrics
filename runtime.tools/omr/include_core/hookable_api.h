/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef hookable_api_h
#define hookable_api_h

/**
* @file hookable_api.h
* @brief Public API for the HOOKABLE module.
*
* This file contains public function prototypes and
* type definitions for the HOOKABLE module.
*
*/

#include "j9comp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------- hookable.c ---------------- */

struct J9PortLibrary;
struct J9HookInterface;
/**
* @brief
* @param hookInterface
* @param portLib
* @param interfaceSize
* @return IDATA
*/
IDATA VMEXPORT 
J9HookInitializeInterface(struct J9HookInterface** hookInterface, struct J9PortLibrary* portLib, size_t interfaceSize);

#ifdef __cplusplus
}
#endif

#endif /* hookable_api_h */

