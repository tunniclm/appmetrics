/*
 * types.h
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
