 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#if defined(_WINDOWS)
	#include <basetsd.h>
	#include <limits.h>
#else
	#include <stdint.h>
	#include <limits.h>
	#include <inttypes.h>
#endif

#ifndef NULL
#define NULL 0
#endif

typedef signed int INT;
typedef unsigned int UINT;
typedef signed int INT32;
#ifndef UINT32
typedef unsigned int UINT32;
#endif
typedef unsigned int uint;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed int int_t;
typedef unsigned int uint_t;

#if defined(_WINDOWS)
typedef signed int int32_t;
typedef unsigned int uint32_t;

typedef signed __int64 INT64;
typedef unsigned __int64 UINT64;

typedef signed __int64 int64;
typedef unsigned __int64 uint64;

typedef signed __int64 int64_t;
typedef unsigned __int64 uint64_t;

#define _P64        "I64"
#else
#if (__WORDSIZE == 64)
#define _P64         "l"
#else
#define _P64         "ll"
#endif

typedef int64_t INT64;
#ifndef UINT64
typedef uint64_t UINT64;
#endif

typedef int64_t int64;
typedef uint64_t uint64;

#endif
