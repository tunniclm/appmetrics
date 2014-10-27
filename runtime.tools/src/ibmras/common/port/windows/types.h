 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#ifndef ibmras_common_port_windows_types_h
#define ibmras_common_port_windows_types_h

#include <basetsd.h>
#include <limits.h>

typedef signed int          INT;
typedef unsigned int        UINT;
typedef signed int          INT32;
typedef unsigned int        UINT32;

typedef unsigned int        uint;
typedef signed int          int32;
typedef unsigned int        uint32;

typedef signed int          int_t;
typedef unsigned int        uint_t;
typedef signed int          int32_t;
typedef unsigned int        uint32_t;

typedef signed __int64      INT64;
typedef unsigned __int64    UINT64;

typedef signed __int64      int64;
typedef unsigned __int64    uint64;

typedef signed __int64      int64_t;
typedef unsigned __int64    uint64_t;

#define _P64        "I64"

#endif /* ibmras_common_port_windows_types_h */
