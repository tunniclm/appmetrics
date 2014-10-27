/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#include <stdint.h>
#include <limits.h>
#include <inttypes.h>

#ifndef ibmras_common_port_aix_types_h
#define ibmras_common_port_aix_types_h

    typedef signed int          INT;
    typedef unsigned int        UINT;
    typedef signed int          INT32;
    #ifndef UINT32
        typedef unsigned int    UINT32;
    #endif

    typedef signed int          int32;
    typedef unsigned int        uint32;

    typedef signed int          int_t;
    typedef unsigned int        uint_t;

    #if (__WORDSIZE == 64)
       #define _P64         "l"
    #else
       #define _P64         "ll"
    #endif

    typedef int64_t             INT64;
#ifndef UINT64
    typedef uint64_t            UINT64;
#endif

    typedef int64_t             int64;
    typedef uint64_t            uint64;


#endif /* ibmras_common_port_aix_types_h */
