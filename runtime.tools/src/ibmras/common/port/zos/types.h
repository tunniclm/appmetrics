/* types.h
 *
 *  Created on: 31 Jan 2014
 *      Author: adam
 */

#include <stdint.h>
#include <limits.h>
#include <inttypes.h>

#ifndef TYPES_H_
#define TYPES_H_

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


#endif /* TYPES_H_ */

