 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_common_types_h
#define ibmras_common_types_h

#ifndef NULL
#define NULL 0
#endif

#if defined(_WINDOWS)
#include "ibmras/common/port/windows/types.h"

#elif defined(_LINUX)
#include "ibmras/common/port/linux/types.h"

#elif defined(_AIX)
#include "ibmras/common/port/aix/types.h"

#elif defined(_ZOS)
#include "ibmras/common/port/zos/types.h"
#endif


#endif /* ibmras_common_types_h */
