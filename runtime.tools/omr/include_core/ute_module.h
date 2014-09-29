/*
*	(c) Copyright IBM Corp. 1991, 2006 All Rights Reserved
*/

/*
 * =============================================================================
 * IBM Confidential
 * OCO Source Materials
 *
 * IBM Universal Trace Engine v1.0
 * (C) Copyright IBM Corp. 1998, 2004.
 *
 * The source code for this program is not published or otherwise divested of
 * its trade secrets, irrespective of what has been deposited with the U.S.
 * Copyright office.
 * =============================================================================
 */
/*
 * Define the constants and structures used by the generated trace code to create
 * trace points for each module.
 */
#ifndef _IBM_UTE_MODULE_H
#define _IBM_UTE_MODULE_H

#define UTE_VERSION_1_1                0x7E000101

#include <stdio.h>

#if defined(LINUX)
#include <unistd.h>
#endif /* defined(LINUX) */

#include "j9comp.h"


#if defined(J9VM_RAS_TDF_TRACE)
	#define UT_TRACE_ENABLED_IN_BUILD
#endif /* defined(J9VM_RAS_TDF_TRACE) */

#ifdef  __cplusplus
extern "C" {
#endif

/*
 * =============================================================================
 *   Constants
 * =============================================================================
 */

#define UT_SPECIAL_ASSERTION 0x00400000

/*
 * =============================================================================
 *   Forward declarations
 * =============================================================================
 */

struct  UtServerInterface;
typedef struct UtServerInterface UtServerInterface;

struct  UtClientInterface;
typedef struct  UtClientInterface UtClientInterface;

struct  UtModuleInterface;
typedef struct UtModuleInterface UtModuleInterface;

/*
 * =============================================================================
 *   The combined UT server/client/module interface
 * =============================================================================
 */

/*
 * UtClientInterface is obsolete. It is included only to maintain binary compatibility
 * with existing binaries that access UtInterface.module (e.g. class library).
 */
typedef struct  UtInterface {
	UtServerInterface *server;
	UtClientInterface *client_unused;
	UtModuleInterface *module;
} UtInterface;

typedef struct UtGroupDetails{
	char                  *groupName;
	I_32                 count;
	I_32                *tpids;
	struct UtGroupDetails *next;
} UtGroupDetails;

typedef struct UtTraceVersionInfo {
	I_32								 		traceVersion;
} UtTraceVersionInfo;

/*
 * =============================================================================
 *  UT module info
 * =============================================================================
 */

typedef struct UtModuleInfo {
	char              	*name;
	I_32             	namelength;
	I_32             	count;
	I_32             	moduleId;
	unsigned char     	*active;
	UtModuleInterface 	*intf;
	char              	*properties;
	UtTraceVersionInfo	*traceVersionInfo;
	char              	*formatStringsFileName;
	unsigned char     	*levels;
	UtGroupDetails    	*groupDetails;
	struct UtModuleInfo	*next;
	struct UtModuleInfo	*containerModule;
	U_32				referenceCount;
/* Fields added at version 8. Legacy binaries may use the interface
 * so we must check the traceVersionInfo structure before 
 */
	I_32				isAuxiliary;
} UtModuleInfo;

#define TRACE_VERSION_WITH_AUXILIARY 8
#define MODULE_IS_AUXILIARY(x) ( x->traceVersionInfo->traceVersion >= TRACE_VERSION_WITH_AUXILIARY && x->isAuxiliary )

/*
 * =============================================================================
 *   The module interface for indirect calls into UT
 * =============================================================================
 */

struct  UtModuleInterface {
	void  (*Trace)(void *env, UtModuleInfo *modInfo, U_32 traceId, const char *spec, ...);
	void  (*TraceMem)(void *env, UtModuleInfo *modInfo, U_32 traceId, UDATA length, void *mem);
	void  (*TraceState)(void *env, UtModuleInfo *modInfo, U_32 traceId, const char *, ...);
	void  (*TraceInit)(void *env, UtModuleInfo *mod);
	void  (*TraceTerm)(void *env, UtModuleInfo *mod);
};

#ifdef  __cplusplus
}
#endif

#endif /* !_IBM_UTE_MODULE_H */
