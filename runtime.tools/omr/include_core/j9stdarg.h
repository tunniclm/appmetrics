/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef j9stdarg_h
#define j9stdarg_h

#include <stdarg.h>
#include <string.h>	

#include "j9comp.h"

#if defined(va_copy)
#define COPY_VA_LIST(new,old) va_copy(new,  old)
#else
#define COPY_VA_LIST(new,old) memcpy(VA_PTR(new), VA_PTR(old), sizeof(va_list))
#endif

#if defined(va_copy)
#define END_VA_LIST_COPY(args) va_end(args)
#else
#define END_VA_LIST_COPY(args)
#endif

#endif     /* j9stdarg_h */


