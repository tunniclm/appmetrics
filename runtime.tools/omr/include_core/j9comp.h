/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef j9comp_h
#define j9comp_h

#include <stddef.h>
#include "j9cfg.h"
#if defined(__cplusplus) && (defined(__xlC__) || defined(J9ZOS390))
#include <builtins.h>
#endif

/*
USE_PROTOTYPES:			Use full ANSI prototypes.

CLOCK_PRIMS:					We want the timer/clock prims to be used

LITTLE_ENDIAN:				This is for the intel machines or other
											little endian processors. Defaults to big endian.

NO_LVALUE_CASTING:	This is for compilers that don't like the left side
											of assigns to be cast.  It hacks around to do the
											right thing.

ATOMIC_FLOAT_ACCESS:	For the hp720 so that float operations will work.

LINKED_USER_PRIMITIVES:	Indicates that user primitives are statically linked
													with the VM executeable.

OLD_SPACE_SIZE_DIFF:	The 68k uses a different amount of old space.
											This "legitimizes" the change.

SIMPLE_SIGNAL:		For machines that don't use real signals in C.
									(eg: PC, 68k)

OS_NAME_LOOKUP:		Use nlist to lookup user primitive addresses.

SYS_FLOAT:	For the MPW C compiler on MACintosh. Most of the math functions
						there return extended type which has 80 or 96 bits depending on 68881 option.
						On All other platforms it is double

FLOAT_EXTENDED: If defined, the type name for extended precision floats.

PLATFORM_IS_ASCII: Must be defined if the platform is ASCII

EXE_EXTENSION_CHAR: the executable has a delimiter that we want to stop at as part of argv[0].

*/


/* Linux ANSI compiler (gcc) */
#ifdef LINUX

/* NOTE: Linux supports different processors -- do not assume 386 */

#if defined(J9HAMMER) || defined(S39064) || defined(LINUXPPC64)
#define DATA_TYPES_DEFINED
typedef unsigned long int		UDATA;					/* 64bits */
typedef unsigned long int	U_64;
typedef unsigned int				U_32;
typedef unsigned short			U_16;
typedef unsigned char			U_8;
typedef signed long int			IDATA;					/* 64bits */
typedef long int					I_64;
typedef signed int				I_32;
typedef signed short				I_16;
typedef signed char				I_8;
typedef U_32						BOOLEAN;

/* LinuxPPC64 is like AIX64 so we need direct function pointers */
#if defined(LINUXPPC64)
#if defined(J9VM_ENV_LITTLE_ENDIAN)
/* LINUXPPC64LE has a new ABI that uses direct functions, not function descriptors */
#define TOC_UNWRAP_ADDRESS(wrappedPointer) ((void *) (wrappedPointer))
extern UDATA getTOC();
#define TOC_STORE_TOC(dest,wrappedPointer) ((dest) = getTOC())
#else /* J9VM_ENV_LITTLE_ENDIAN */
#define TOC_UNWRAP_ADDRESS(wrappedPointer) (((void **)(wrappedPointer))[0])
#define TOC_STORE_TOC(dest,wrappedPointer) (dest = (((UDATA*)(wrappedPointer))[1]))
#endif /* J9VM_ENV_LITTLE_ENDIAN */
#endif /* LINUXPPC64 */

#else
typedef long long I_64;
typedef unsigned long long U_64;
#endif

typedef double SYS_FLOAT;

#define J9CONST64(x) x##LL
#define J9CONST_I64(x) x##LL
#define J9CONST_U64(x) x##ULL

#define NO_LVALUE_CASTING
#define FLOAT_EXTENDED	long double
#define PLATFORM_IS_ASCII
#define PLATFORM_LINE_DELIMITER	"\012"
#define DIR_SEPARATOR '/'
#define DIR_SEPARATOR_STR "/"

/* no priorities on Linux */
#define J9_PRIORITY_MAP {0,0,0,0,0,0,0,0,0,0,0,0}


#if (defined(LINUXPPC) && !defined(LINUXPPC64)) || defined(S390) || defined(J9HAMMER)
#define VA_PTR(valist) ((va_list*)&valist[0])
#endif

#if (defined(HARDHAT) && defined(ARMGNU))
#define ATOMIC_LONG_ACCESS
#endif

#if defined(__GNUC__)
#define VMINLINE_ALWAYS inline __attribute((always_inline))
/* If -O0 is in effect, define VMINLINE to be empty */
#if !defined(__OPTIMIZE__)
#define VMINLINE
#endif

#elif defined(__xlC__)
/*
 * xlC11 C++ compiler reportedly supports attributes before function names, but we've only tested xlC12.
 * The C compiler doesn't support it.
 */
#if defined(__cplusplus) && (__xlC__ >= 0xc00)
#define VMINLINE_ALWAYS inline __attribute__((always_inline))
#endif
#endif /* __xlC__ */

#ifndef VMINLINE_ALWAYS
#define VMINLINE_ALWAYS inline
#endif

#define HAS_BUILTIN_EXPECT

#endif /* LINUX */


/* MVS compiler */
#ifdef MVS

#define DATA_TYPES_DEFINED
typedef unsigned int				UDATA;
typedef unsigned long long	U_64;
typedef unsigned int				U_32;
typedef unsigned short			U_16;
typedef unsigned char			U_8;
typedef signed int				IDATA;
typedef signed long long		I_64;
typedef signed int				I_32;
typedef signed short				I_16;
typedef signed char				I_8;
typedef I_32						BOOLEAN;
typedef double 					SYS_FLOAT;
typedef long double				FLOAT_EXTENDED;

#define J9CONST64(x) x##LL
#define J9CONST_I64(x) x##LL
#define J9CONST_U64(x) x##ULL

#define NO_LVALUE_CASTING
#define PLATFORM_LINE_DELIMITER	"\025"
#define DIR_SEPARATOR '.'
#define DIR_SEPARATOR_STR "."

#include "esmap.h"

#endif /* MVS */


#define GLOBAL_DATA(symbol) ((void*)&(symbol))
#define GLOBAL_TABLE(symbol) GLOBAL_DATA(symbol)


/* RS6000 */

/* The AIX platform has the define AIXPPC and RS6000,
	this means AIXPPC inherits from the RS6000.*/

#if defined(RS6000)

#define DATA_TYPES_DEFINED

/* long is 32 bits on AIX32, and 64 bits on AIX64 */
typedef unsigned long UDATA;
typedef signed long IDATA;

#if defined(PPC64)
typedef unsigned long U_64;
typedef long I_64;
#else
typedef unsigned long long U_64;
typedef long long I_64;
#endif

typedef unsigned int U_32;
typedef unsigned short U_16;
typedef unsigned char U_8;
typedef signed int I_32;
typedef signed short I_16;
typedef signed char I_8;
typedef U_32 BOOLEAN;
typedef double SYS_FLOAT;

#define J9CONST64(x) x##LL
#define J9CONST_I64(x) x##LL
#define J9CONST_U64(x) x##ULL

#define NO_LVALUE_CASTING
#define PLATFORM_LINE_DELIMITER	"\012"
#define DIR_SEPARATOR '/'
#define DIR_SEPARATOR_STR "/"

#define TOC_UNWRAP_ADDRESS(wrappedPointer) (((void **)(wrappedPointer))[0])
#define TOC_STORE_TOC(dest,wrappedPointer) (dest = (((UDATA*)(wrappedPointer))[1]))

/*
 * Have to have priorities between 40 and 60 inclusive for AIX >=5.3
 * AIX 5.2 ignores them
 */

#if (defined(J9OS_I5) && !defined(J9OS_I5_V5R4)) /* i5/OS V6R1 or newer */
#define J9_PRIORITY_MAP  { 55,56,57,58,59,60,60,60,60,60,60,60 }
#define J9_PRIORITY_MAP_ALT  { 54,55,55,56,56,57,57,58,58,59,59,60 }

#elif (defined(J9OS_I5) && defined(J9OS_I5_V5R4)) /* i5/OS V5R4 */
#define J9_PRIORITY_MAP  { 54,55,55,56,56,57,57,58,58,59,59,60 }
#define J9_PRIORITY_MAP_ALT  { 55,56,57,58,59,60,60,60,60,60,60,60 }

#else /* not i5/OS */
#define J9_PRIORITY_MAP  { 40,41,43,45,47,49,51,53,55,57,59,60 }
#endif /* J9OS_I5 checks */


#if defined(__xlC__)
/*
 * xlC11 C++ compiler reportedly supports attributes before function names, but we've only tested xlC12.
 * The C compiler doesn't support it.
 */
#if defined(__cplusplus) && (__xlC__ >= 0xc00)
#define VMINLINE_ALWAYS inline __attribute__((always_inline))
#endif
#endif /* __xlC__ */

/* XLC doesn't support __attribute before a function name and doesn't support inline in the stdc89 language level */
#ifndef VMINLINE_ALWAYS
#define VMINLINE_ALWAYS __inline__
#endif

#define HAS_BUILTIN_EXPECT

#endif /* defined(RS6000) */


/* Win32 - Windows 3.1 & NT using Win32 */

#ifdef WIN32

#ifdef J9HAMMER
/* NOTE: Windows now supports different processors -- do not assume 386 only */

#define DATA_TYPES_DEFINED
typedef unsigned __int64	UDATA;					/* 64bits */
typedef unsigned __int64	U_64;
typedef unsigned __int32	U_32;
typedef unsigned __int16	U_16;
typedef unsigned __int8		U_8;
typedef signed __int64		IDATA;					/* 64bits */
typedef signed __int64		I_64;
typedef signed __int32		I_32;
typedef signed __int16		I_16;
typedef signed __int8			I_8;

/* temp hack -- don't typedef BOOLEAN since it's already def'ed on Win32 */
#define BOOLEAN UDATA

#else

typedef __int64					I_64 ;
typedef unsigned __int64	U_64 ;
#endif /* J9HAMMER */

typedef double 					SYS_FLOAT;

#define NO_LVALUE_CASTING
#define EXE_EXTENSION_CHAR	'.'

#define DIR_SEPARATOR '\\'
#define DIR_SEPARATOR_STR "\\"

#if defined(J9VM_SIZE_SMALL_OS_STACK)
#define UNICODE_BUFFER_SIZE 128
#else
#define UNICODE_BUFFER_SIZE EsMaxPath
#endif
#define OS_ENCODING_CODE_PAGE CP_UTF8
#define OS_ENCODING_MB_FLAGS 0
#define OS_ENCODING_WC_FLAGS 0

#define J9_PRIORITY_MAP {	\
	THREAD_PRIORITY_IDLE,							/* 0 */\
	THREAD_PRIORITY_LOWEST,					/* 1 */\
	THREAD_PRIORITY_BELOW_NORMAL,	/* 2 */\
	THREAD_PRIORITY_BELOW_NORMAL,	/* 3 */\
	THREAD_PRIORITY_BELOW_NORMAL,	/* 4 */\
	THREAD_PRIORITY_NORMAL,						/* 5 */\
	THREAD_PRIORITY_ABOVE_NORMAL,		/* 6 */\
	THREAD_PRIORITY_ABOVE_NORMAL,		/* 7 */\
	THREAD_PRIORITY_ABOVE_NORMAL,		/* 8 */\
	THREAD_PRIORITY_ABOVE_NORMAL,		/* 9 */\
	THREAD_PRIORITY_HIGHEST,					/*10 */\
	THREAD_PRIORITY_TIME_CRITICAL			/*11 */}

#if defined(__GNUC__)
#define VMINLINE_ALWAYS inline __attribute((always_inline))
/* If -O0 is in effect, define VMINLINE to be empty */
#if !defined(__OPTIMIZE__)
#define VMINLINE
#endif
#define HAS_BUILTIN_EXPECT
#else
/* Only for use on static functions */
#define VMINLINE_ALWAYS __forceinline
#endif

#endif /* WIN32 */

/* ZOS390 */

#if defined(J9ZOS390)

#define DATA_TYPES_DEFINED
typedef unsigned long			UDATA;
typedef unsigned int   		U_32;
typedef unsigned short		U_16;
typedef unsigned char		U_8;
typedef signed long			IDATA;
typedef signed int			I_32;
typedef signed short			I_16;
typedef signed char			I_8;
typedef unsigned int			BOOLEAN;
#if defined (J9VM_ENV_DATA64)
typedef unsigned long U_64;
typedef long I_64;
#else
typedef signed long long	I_64;
typedef unsigned long long	U_64;
#endif

typedef double				SYS_FLOAT;

#define J9CONST64(x) x##LL
#define J9CONST_I64(x) x##LL
#define J9CONST_U64(x) x##ULL

#define NO_LVALUE_CASTING
#define PLATFORM_LINE_DELIMITER	"\012"
#define DIR_SEPARATOR '/'
#define DIR_SEPARATOR_STR "/"

#define VA_PTR(valist) ((va_list*)&valist[0])

typedef struct {
#if !defined(J9VM_ENV_DATA64)
	char stuff[16];
#endif
	char *ada;
	void *rawFnAddress;
} J9FunctionDescriptor_T;

#define TOC_UNWRAP_ADDRESS(wrappedPointer) (((J9FunctionDescriptor_T *)(UDATA)(wrappedPointer))->rawFnAddress)


#if defined(__cplusplus)

#if defined(__MVS__) && defined(__COMPILER_VER__)
#if (__COMPILER_VER__ >= 0x410D0000)
#define VMINLINE_ALWAYS inline __attribute__((always_inline))
#endif
#endif /* __MVS__ && __COMPILER_VER__ */

#ifndef VMINLINE_ALWAYS
#define VMINLINE_ALWAYS inline
#endif

#endif /* __cplusplus */

#endif /* J9ZOS390 */


/* Provide some reasonable defaults for the VM "types":

	UDATA			unsigned data, can be used as an integer or pointer storage.
	IDATA			signed data, can be used as an integer or pointer storage.
	U_64 / I_64	unsigned/signed 64 bits.
	U_32 / I_32	unsigned/signed 32 bits.
	U_16 / I_16	unsigned/signed 16 bits.
	U_8 / I_8		unsigned/signed 8 bits (bytes -- not to be confused with char)
	BOOLEAN	something that can be zero or non-zero.

*/

#ifndef	DATA_TYPES_DEFINED

typedef unsigned int			UDATA;
typedef unsigned int			U_32;
typedef unsigned short		U_16;
typedef unsigned char		U_8;
/* no generic U_64 or I_64 */

typedef int					IDATA;
typedef int					I_32;
typedef short				I_16;
typedef signed char				I_8;    /* jdk Bugzilla 117041 : pb : Incorrectly defined I_8 type */


/* temp hack -- don't typedef BOOLEAN since it's already def'ed on Win32 */
#define BOOLEAN UDATA


#endif

#ifndef J9CONST64
#define J9CONST64(x) x##L
#endif

#ifndef J9CONST_I64
#define J9CONST_I64(x) x##LL
#endif

#ifndef J9CONST_U64
#define J9CONST_U64(x) x##ULL
#endif


/*
 * MIN and MAX data types
 */
#define U_8_MAX ((U_8)-1)
#define I_8_MIN ((I_8)1 << ((sizeof(I_8) * 8) - 1))
#define I_8_MAX ((I_8)((U_8)I_8_MIN - 1))

#define U_16_MAX ((U_16)-1)
#define I_16_MIN ((I_16)1 << ((sizeof(I_16) * 8) - 1))
#define I_16_MAX ((I_16)((U_16)I_16_MIN - 1))

#define U_32_MAX ((U_32)-1)
#define I_32_MIN ((I_32)1 << ((sizeof(I_32) * 8) - 1))
#define I_32_MAX ((I_32)((U_32)I_32_MIN - 1))

#define U_64_MAX ((U_64)-1)
#define I_64_MIN ((I_64)1 << ((sizeof(I_64) * 8) - 1))
#define I_64_MAX ((I_64)((U_64)I_64_MIN - 1))

#define UDATA_MAX ((UDATA)-1)
#define IDATA_MIN ((IDATA)1 << ((sizeof(IDATA) * 8) - 1))
#define IDATA_MAX ((IDATA)((UDATA)IDATA_MIN - 1))

#ifndef J9_DEFAULT_SCHED
/* by default, pthreads platforms use the SCHED_OTHER thread scheduling policy */
#define J9_DEFAULT_SCHED SCHED_OTHER
#endif

#ifndef J9_PRIORITY_MAP
/* if no priority map if provided, priorities will be determined algorithmically */
#endif

#ifndef	FALSE
#define	FALSE		((BOOLEAN) 0)

#ifndef TRUE
#define	TRUE		((BOOLEAN) (!FALSE))
#endif
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL    (0)
#else
#define NULL    ((void *)0)
#endif
#endif

#define USE_PROTOTYPES
#ifdef	USE_PROTOTYPES
#define	PROTOTYPE(x)	x
#define	VARARGS		, ...
#else
#define	PROTOTYPE(x)	()
#define	VARARGS
#endif

/* Macro used to mark all exported VM functions */
#define VMEXPORT

/* Assign the default line delimiter if it was not set */
#ifndef PLATFORM_LINE_DELIMITER
#define PLATFORM_LINE_DELIMITER	"\015\012"
#endif

/* Set the max path length if it was not set */
#ifndef MAX_IMAGE_PATH_LENGTH
#define MAX_IMAGE_PATH_LENGTH	(2048)
#endif

typedef	double	ESDOUBLE;
typedef	float		ESSINGLE;

typedef struct U_128 {
#if defined(J9VM_ENV_LITTLE_ENDIAN)
	U_64 low64;
	U_64 high64;
#else /* J9VM_ENV_LITTLE_ENDIAN */
	U_64 high64;
	U_64 low64;
#endif /* J9VM_ENV_LITTLE_ENDIAN */
} U_128;

/* helpers for U_64s */
#define CLEAR_U64(u64)  (u64 = (U_64)0)

#ifdef	J9VM_ENV_LITTLE_ENDIAN
#define	LOW_LONG(l)	(*((U_32 *) &(l)))
#define	HIGH_LONG(l)	(*(((U_32 *) &(l)) + 1))
#else
#define	HIGH_LONG(l)	(*((U_32 *) &(l)))
#define	LOW_LONG(l)	(*(((U_32 *) &(l)) + 1))
#endif

#define	I8(x)			((I_8) (x))
#define	I8P(x)			((I_8 *) (x))
#define	U16(x)			((U_16) (x))
#define	I16(x)			((I_16) (x))
#define	I16P(x)			((I_16 *) (x))
#define	U32(x)			((U_32) (x))
#define	I32(x)			((I_32) (x))
#define	I32P(x)			((I_32 *) (x))
#define	U16P(x)			((U_16 *) (x))
#define	U32P(x)			((U_32 *) (x))
#define	BYTEP(x)		((BYTE *) (x))

/* Test - was conflicting with OS2.h */
#define	ESCHAR(x)		((CHARACTER) (x))
#define	FLT(x)			((FLOAT) x)
#define	FLTP(x)			((FLOAT *) (x))

#ifdef	NO_LVALUE_CASTING
#define	LI8(x)			(*((I_8 *) &(x)))
#define	LI8P(x)			(*((I_8 **) &(x)))
#define	LU16(x)			(*((U_16 *) &(x)))
#define	LI16(x)			(*((I_16 *) &(x)))
#define	LU32(x)			(*((U_32 *) &(x)))
#define	LI32(x)			(*((I_32 *) &(x)))
#define	LI32P(x)		(*((I_32 **) &(x)))
#define	LU16P(x)		(*((U_16 **) &(x)))
#define	LU32P(x)		(*((U_32 **) &(x)))
#define	LBYTEP(x)		(*((BYTE **) &(x)))
#define	LCHAR(x)		(*((CHARACTER) &(x)))
#define	LFLT(x)			(*((FLOAT) &x))
#define	LFLTP(x)		(*((FLOAT *) &(x)))
#else
#define	LI8(x)			I8((x))
#define	LI8P(x)			I8P((x))
#define	LU16(x)			U16((x))
#define	LI16(x)			I16((x))
#define	LU32(x)			U32((x))
#define	LI32(x)			I32((x))
#define	LI32P(x)		I32P((x))
#define	LU16P(x)		U16P((x))
#define	LU32P(x)		U32P((x))
#define	LBYTEP(x)		BYTEP((x))
#define	LCHAR(x)		CHAR((x))
#define	LFLT(x)			FLT((x))
#define	LFLTP(x)		FLTP((x))
#endif

/* Macros for converting between words and longs and accessing bits */

#define	HIGH_WORD(x)	U16(U32((x)) >> 16)
#define	LOW_WORD(x)		U16(U32((x)) & 0xFFFF)
#define	MAKE_32(h, l)	((U32((h)) << 16) | U32((l)))
#define	MAKE_64(h, l)	((((I_64)(h)) << 32) | (l))

#ifdef __cplusplus
#define J9_CFUNC "C"
#define J9_CDATA "C"
#else
#define J9_CFUNC
#define J9_CDATA
#endif

/* Macros for tagging functions which read/write the vm thread */

#define READSVMTHREAD
#define WRITESVMTHREAD
#define REQUIRESSTACKFRAME

/* macro for tagging functions which never return */
#ifdef __GNUC__
/* on GCC, we can actually pass this information on to the compiler */
#define OMRNORETURN __attribute__((noreturn))
#else
#define OMRNORETURN
#endif

/* on some systems (e.g. LinuxPPC) va_list is an array type.  This is probably in
 * violation of the ANSI C spec, but it's not entirely clear.  Because of this, we end
 * up with an undesired extra level of indirection if we take the address of a
 * va_list argument.
 *
 * To get it right ,always use the VA_PTR macro
 */
#ifndef VA_PTR
#define VA_PTR(valist) (&valist)
#endif

/* Macros used on RS6000 to manipulate wrapped function pointers */
#ifndef TOC_UNWRAP_ADDRESS
#define TOC_UNWRAP_ADDRESS(wrappedPointer) ((void *) (wrappedPointer))
#endif
#ifndef TOC_STORE_TOC
#define TOC_STORE_TOC(dest,wrappedPointer)
#endif

/* Macros for accessing I_64 values */
#ifdef ATOMIC_LONG_ACCESS
#define PTR_LONG_STORE(dstPtr, aLongPtr) ((*U32P(dstPtr) = *U32P(aLongPtr)), (*(U32P(dstPtr)+1) = *(U32P(aLongPtr)+1)))
#define PTR_LONG_VALUE(dstPtr, aLongPtr) ((*U32P(aLongPtr) = *U32P(dstPtr)), (*(U32P(aLongPtr)+1) = *(U32P(dstPtr)+1)))
#else
#define PTR_LONG_STORE(dstPtr, aLongPtr) (*(dstPtr) = *(aLongPtr))
#define PTR_LONG_VALUE(dstPtr, aLongPtr) (*(aLongPtr) = *(dstPtr))
#endif

/* Macro used when declaring tables which require relocations. */
#ifndef J9CONST_TABLE
#define J9CONST_TABLE const
#endif

/* ANSI qsort is not always available */
#ifndef J9_SORT
#define J9_SORT(base, nmemb, size, compare) qsort((base), (nmemb), (size), (compare))
#endif

/* For backwards compatibility. See Bugzilla 117585  */
/* By default order doubles in the native (i.e. big/little endian) ordering. */
#if defined(J9VM_ENV_PLATFORM_DOUBLE_ORDER)
#if !defined(J9_PLATFORM_DOUBLE_ORDER)
#define J9_PLATFORM_DOUBLE_ORDER
#endif
#endif

#if !defined(VMINLINE_ALWAYS)
#define VMINLINE_ALWAYS
#endif
#if !defined(VMINLINE)
#define VMINLINE VMINLINE_ALWAYS
#endif
#if defined(DEBUG)
#undef VMINLINE
#define VMINLINE
#endif

#if defined(HAS_BUILTIN_EXPECT)
#undef HAS_BUILTIN_EXPECT
#define J9_EXPECTED(e)		__builtin_expect(!!(e), 1)
#define J9_UNEXPECTED(e)	__builtin_expect(!!(e), 0)
#else
#define J9_EXPECTED(e) (e)
#define J9_UNEXPECTED(e) (e)
#endif

#if !defined(max)
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#if !defined(min)
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

/* Provide macros which can be used for bit testing */
#define J9_ARE_ANY_BITS_SET(value, bits) (0 != ((value) & (bits)))
#define J9_ARE_ALL_BITS_SET(value, bits) ((bits) == ((value) & (bits)))
#define J9_ARE_NO_BITS_SET(value, bits) (!J9_ARE_ANY_BITS_SET(value, bits))

/* Legacy defines - remove once code cleanup is complete */
#define J9VM_ENV_DIRECT_FUNCTION_POINTERS
#define J9VM_OPT_REMOVE_CONSTANT_POOL_SPLITTING

#endif /* escomp_h */
