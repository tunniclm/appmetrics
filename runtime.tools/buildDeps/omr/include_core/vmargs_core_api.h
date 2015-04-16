/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#include "j9nongenerated.h"
#include "util_core_api.h"
#include "j9port.h"
#include "pool_api.h"

#ifndef vmargs_core_api_h
#define vmargs_core_api_h

#if !defined(JNI_H)
typedef struct JavaVMOption {
	char *optionString;
	void *extraInfo;
} JavaVMOption;
#endif /* !defined(JNI_H) */

/* Temporary buffer for argument while constructing the list */
typedef struct J9JavaVMArgInfo {
	JavaVMOption vmOpt;
	J9CmdLineOption cmdLineOpt;
	struct J9JavaVMArgInfo *next; /* used to create linked list */
} J9JavaVMArgInfo;

typedef struct J9JavaVMArgInfoList {
	J9Pool *pool;
	J9JavaVMArgInfo *head; /* used to create linked list */
	J9JavaVMArgInfo *tail; /* used to create linked list */
} J9JavaVMArgInfoList;

#define CONSUMABLE_ARG 1
#define NOT_CONSUMABLE_ARG 2
#define ARG_CONSUMED 4
#define ARG_REQUIRES_LIBRARY 8
#define ARG_MEMORY_ALLOCATION 16

#define VMOPT_XOPTIONSFILE_EQUALS "-Xoptionsfile="

#define RC_FAILED -70
#define RC_MALFORMED -71

J9JavaVMArgInfo *
newJavaVMArgInfo(J9JavaVMArgInfoList *vmArgumentsList, char *optString, uintptr_t flags);

/*
 * Add a -Xoptionsfile argument, plus contents of specified file.
 * This allocates memory for the options string and contents of the file
 * @param portLib port library
 * @param xOptionsfileArg -Xoptionsfile=<path>
 * @param vmArgumentsList current list of arguments
 * @param verboseFlags set to VERBOSE_INIT for verbosity
 * @return 0 on success, negative value on failure
 */
intptr_t
addXOptionsFile(J9PortLibrary* portLib, const char *xOptionsfileArg, J9JavaVMArgInfoList *vmArgumentsList, uintptr_t verboseFlags);

/**
 * Scan an argument buffer containing multiple options to find the options and add them to the list.
 * The argument buffer must point to an allocated section of memory.  It will be freed if no valid arguments
 * other than -Xoptionsfile are found. Its contents may be changed.
 * @param portLib port library
 * @param argumentBuffer buffer holding the raw string
 * @param vmArgumentsList list of arguments so far.
 * @param verboseFlags turn on verbosity
 * @param parseOptionsFileFlag Set to false to ignore embedded -Xoptionsfile arguments
 * @return 0 on success, negative on error
 */
intptr_t
parseOptionsBuffer(J9PortLibrary* portLib, char* argumentBuffer, J9JavaVMArgInfoList *vmArgumentsList, uintptr_t verboseFlags, BOOLEAN parseOptionsFileFlag);

/**
 * Parses the Hypervisor Environment Variable string into a J9JavaVMArgInfoList list
 * removing leading and trailing quotes
 *
 * @param[in]	portLib 	The portLibary
 * @param[in]	argBuffer 	pointer to the env var ENVVAR_IBM_JAVA_HYPERVISOR_SETTINGS
 * @param[in]	hypervisorArgumentsList	pointer to the parsed option strings
 *
 * @return	0 on Success, Negative error code on failure
 */
intptr_t
parseHypervisorEnvVar(J9PortLibrary *portLib, char *argBuffer, J9JavaVMArgInfoList *hypervisorArgumentsList);

#endif /* vmargs_core_api_h */
