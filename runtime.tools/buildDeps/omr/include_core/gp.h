/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef gp_h
#define gp_h

struct J9SigContext;
#ifdef WIN32_IBMC
typedef uintptr_t (* protected_fn)(void*);
typedef void (* handler_fn)(uintptr_t gpType, void* gpInfo, void* userData, struct J9SigContext *gpContext);
#else
typedef uintptr_t (*protected_fn)(void*);
typedef void (*handler_fn)(uintptr_t gpType, void* gpInfo, void* userData, struct J9SigContext *gpContext);
#endif

#define J9PrimErrGPF 0
#define J9PrimErrGPFInvalidRead 1
#define J9PrimErrGPFInvalidWrite 2
#define J9PrimErrGPFInvalidInstruction 3
#define J9PrimErrGPFFloat 4

#endif     /* gp_h */

