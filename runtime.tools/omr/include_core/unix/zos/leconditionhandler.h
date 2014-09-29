/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef leconditionhandler_h
#define leconditionhandler_h

#include <leawi.h>
#include <setjmp.h>
#include <ceeedcct.h>
#include "edcwccwi.h"
#include "j9port.h"

typedef struct J9ZOSLEConditionHandlerRecord {
	struct J9PortLibrary *portLibrary;
	j9sig_handler_fn handler;
	void *handler_arg;
	sigjmp_buf returnBuf;
	struct __jumpinfo farJumpInfo;
	U_32 flags;
	U_32 recursiveCheck; /* if this is set to 1, the handler corresponding to this record has been invoked recursively */
} J9ZOSLEConditionHandlerRecord;

#endif /* leconditionhandler_h */
