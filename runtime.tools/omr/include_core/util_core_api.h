/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef util_core_api_h
#define util_core_api_h

/**
* @file util_core_api.h
* @brief Public API for the util_core module.
*
* This file contains public function prototypes and
* type definitions for the util_core module.
*
*/

#include "j9comp.h"
#include "j9port.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(J9ZOS390)
#pragma map(getdsa, "GETDSA")
/* ----------------- j9getdsa.s ---------------- */
/**
 * Returns the caa and dsa respectively on z/OS
 */
void *getdsa(void);
#endif

/* ---------------- cas8help.s ---------------- */

#if !defined(J9VM_ENV_DATA64) && (defined(AIXPPC) || defined(LINUXPPC))

/**
 * @brief Perform a compare and swap of a 64-bit value on a 32-bit system.
 *
 * @param[in] addr  The address of the 8-aligned memory address
 * @param[in] compareLo  Low part of compare value
 * @param[in] compareHi  High part of compare value
 * @param[in] swapLo  Low  part of swap value
 * @param[in] swapHi  High part of swap value
 * @return  The old value read from addr
 */
U_64
J9CAS8Helper(volatile U_64 * addr, U_32 compareLo, U_32 compareHi, U_32 swapLo, U_32 swapHi);

#endif /* !J9VM_ENV_DATA64 && (AIXPPC || LINUXPPC) */

/* AtomicFunctions.cpp */
#ifndef _J9VMATOMICFUNCTIONS_
#define _J9VMATOMICFUNCTIONS_
extern J9_CFUNC void  issueReadBarrier (void);
extern J9_CFUNC UDATA  compareAndSwapUDATA (UDATA *location, UDATA oldValue, UDATA newValue, UDATA* spinlock);
extern J9_CFUNC UDATA  compareAndSwapUDATANoSpinlock(UDATA *location, UDATA oldValue, UDATA newValue);
extern J9_CFUNC U_32  compareAndSwapU32 (U_32 *location, U_32 oldValue, U_32 newValue, UDATA* spinlock);
extern J9_CFUNC U_32  compareAndSwapU32NoSpinlock(U_32 *location, U_32 oldValue, U_32 newValue);
extern J9_CFUNC void  issueReadWriteBarrier (void);
extern J9_CFUNC void  issueWriteBarrier (void);
#endif /* _J9VMATOMICFUNCTIONS_ */

/* ---------------- utf8decode.c ---------------- */

/**
* @brief
* @param input
* @param result
* @return U_32
*/
U_32
decodeUTF8Char(const U_8* input, U_16* result);


/**
* @brief
* @param input
* @param result
* @param bytesRemaining
* @return U_32
*/
U_32
decodeUTF8CharN(const U_8* input, U_16* result, UDATA bytesRemaining);


/* ---------------- utf8encode.c ---------------- */

/**
* @brief
* @param unicode
* @param result
* @return U_32
*/
U_32
encodeUTF8Char(UDATA unicode, U_8* result);


/**
* @brief
* @param unicode
* @param result
* @param bytesRemaining
* @return U_32
*/
U_32
encodeUTF8CharN(UDATA unicode, U_8* result, U_32 bytesRemaining);



/* ---------------- xml.c ---------------- */

/**
 * Escapes a string for use with XML.
 * @param portLibrary[in]
 * @param outBuf[out] A buffer in which to place the escaped string.
 * @param outBufLen[in] The length of the output buffer.
 * @param string[in] The string that should be escaped.
 * @param StringLen[in] The length of the string excluding any null termination.
 * @return The number of characters of the input string that were processed before filling the output buffer.
 */
UDATA
escapeXMLString(J9PortLibrary* portLibrary, char *outBuf, UDATA outBufLen, const char *string, UDATA stringLen);



/* ----------------- primeNumberHelper.c ---------------- */
/*
 * PRIMENUMBERHELPER_OUTOFRANGE is used when primeNumberHelper functions are being used
 * by a number that is not in the supported range of primeNumberHelper.
 * For such cases, these functions return PRIMENUMBERHELPER_OUTOFRANGE.
 */
#define PRIMENUMBERHELPER_OUTOFRANGE 1
/**
 * @brief
 * @param number
 * @return UDATA
 */
UDATA findLargestPrimeLessThanOrEqualTo(UDATA number);

/**
 * @brief
 * @param number
 * @return UDATA
 */
UDATA findSmallestPrimeGreaterThanOrEqualTo(UDATA number);

/**
 * @brief
 * @param void
 * @return UDATA
 */
UDATA getSupportedBiggestNumberByPrimeNumberHelper(void);

#if defined(WIN32) && !defined(WIN32_IBMC)

/* ---------------- j9getdbghelp.c ---------------- */

/**
* @brief Load the version of dbghelp.dll that shipped with the JRE. If we can't find the shipped version, try to find it somewhere else.
* @return A handle to dbghelp.dll if we were able to find one, NULL otherwise.
*/
UDATA j9getdbghelp_loadDLL(void);

/**
* @brief Get a previously loaded version of dbghelp.dll that shipped with the JRE.
* @return A handle to dbghelp.dll if we were able to find a previously loaded version, NULL otherwise.
*/
UDATA j9getdbghelp_getDLL(void);

/**
* @brief Free the supplied version of dbgHelpDLL
* @param dbgHelpDLL
* @return 0 if the library was freed, non-zero otherwise.
*/
void j9getdbghelp_freeDLL(UDATA dbgHelpDLL);

#endif  /* defined(WIN32) && !defined(WIN32_IBMC) */


/* ---------------- stricmp.c ---------------- */

/*
 * Converts characters to lowercase.
 * This is intended only to be used for command line processing.
 * Although currently implemented to do ASCII conversion,
 * it may be changed in the future if additional behavior is required.
 * This is used to address locale issues such as in Turkish where
 * the uppercase of 'i' does not equal 'I'.
 *
 *
 * @param c input character
 * @return lowercase version or returns input if a valid ASCII
 *         character wasn't given.
 */
int
j9_cmdla_tolower(int c);

/*
 * Converts ASCII character to lowercase.
 * This is used to address locale issues such as in Turkish where
 * the uppercase of 'i' does not equal 'I'.
 *
 * @param c input character
 * @return lowercase version or returns input if a valid ASCII
 *         character wasn't given.
 */
int
j9_ascii_tolower(int c);

/*
 * Converts characters to uppercase.
 * This is intended only to be used for command line processing.
 * Although currently implemented to do ASCII conversion,
 * it may be changed in the future if additional behavior is required.
 * This is used to address locale issues such as in Turkish where
 * the uppercase of 'i' does not equal 'I'.
 *
 * @param c input character
 * @return uppercase version or returns input if a valid ASCII
 *         character wasn't given.
 */
int
j9_cmdla_toupper(int c);

/*
 * Converts ASCII character to uppercase.
 * This is used to address locale issues such as in Turkish where
 * the uppercase of 'i' does not equal 'I'.
 *
 * @param c input character
 * @return uppercase version or returns input if a valid ASCII
 *         character wasn't given.
 */
int
j9_ascii_toupper(int c);

/*
 * Compare the bytes of two strings, ignoring case. Intended to be used
 * with command line arguments.
 * This is used to address locale issues such as in Turkish where
 * the uppercase of 'i' does not equal 'I'.
 *
 * @param *s1 string1
 * @param *s2 string2
 * @return a positive integer if s1 is greater, zero if the strings
 *         are equal, or a negative value if s1 is less.
 */
int
j9_cmdla_stricmp(const char *s1, const char *s2);

/*
 * Compare the bytes of two strings, ignoring case. Intended to be used
 * with command line arguments.
 * This is used to address locale issues such as in Turkish where
 * the uppercase of 'i' does not equal 'I'.
 *
 * @param *s1 string1
 * @param *s2 string2
 * @param length of string
 * @return a positive integer if s1 is greater, zero if the strings
 *         are equal, or a negative value if s1 is less.
 */
int
j9_cmdla_strnicmp(const char *s1, const char *s2, size_t length);


/* ---------------- shchelp.c ---------------- */

UDATA
getValuesFromShcFilePrefix(struct J9PortLibrary* portLibrary, const char* filename, J9PortShcVersion* versionData);

U_32
getShcModlevelForJCL(UDATA j2seVersion);

U_32
getJCLForShcModlevel(UDATA modlevel);

UDATA
isCompatibleShcFilePrefix(J9PortLibrary* portlib, U_32 j2seVersion, U_32 feature, const char* filename);

void
getStringForShcModlevel(J9PortLibrary* portlib, U_32 modlevel, char* buffer);

void
getStringForShcAddrmode(J9PortLibrary* portlib, U_32 addrmode, char* buffer);

UDATA
isCacheFileName(J9PortLibrary* portlib, const char* nameToTest, UDATA expectPersistent, const char* optionalExtraID);


/* ---------------- argscan.c ---------------- */

/**
 * @brief
 * @param portLibrary
 * @param input
 * @return char *
 */
char * trim(J9PortLibrary* portLibrary, char * input);

/**
* @brief
* @param portLibrary
* @param module
* @param *scan_start
* @return void
*/
void scan_failed(J9PortLibrary * portLibrary, const char* module, const char *scan_start);


/**
* @brief
* @param portLibrary
* @param module
* @param *scan_start
* @return void
*/
void scan_failed_incompatible(J9PortLibrary * portLibrary, char* module, char *scan_start);


/**
* @brief
* @param portLibrary
* @param module
* @param *scan_start
* @return void
*/
void scan_failed_unsupported(J9PortLibrary * portLibrary, char* module, char *scan_start);


/**
* @brief
* @param **scan_start
* @param result
* @return UDATA
*/
UDATA scan_hex(char **scan_start, UDATA* result);


/**
* @brief
* @param **scan_start
* @param uppercaseFalg
* @param result
* @return UDATA
*/
UDATA scan_hex_caseflag(char **scan_start, BOOLEAN uppercaseAllowed, UDATA* result);

/**
* @brief
* @param **scan_start
* @param *result
* @return UDATA
*/
UDATA scan_idata(char **scan_start, IDATA *result);


/**
* @brief
* @param portLibrary
* @param **scan_start
* @param delimiter
* @return char *
*/
char *scan_to_delim(J9PortLibrary * portLibrary, char **scan_start, char delimiter);


/**
* @brief
* @param **scan_start
* @param result
* @return UDATA
*/
UDATA scan_udata(char **scan_start, UDATA* result);


/**
* @brief
* @param **scan_start
* @param result
* @return UDATA
*/
UDATA scan_u64(char **scan_start, U_64* result);


/**
* @brief
* @param **scan_start
* @param result
* @return U_32
*/
UDATA scan_u32(char **scan_start, U_32* result);


/**
* @brief
* @param **scan_start
* @param *search_string
* @return UDATA
*/
UDATA try_scan(char **scan_start, const char *search_string);

#if defined(J9ZOS390)
/* ---------------- getstoragekey.c ---------------- */

/**
 * Get the storage key for current process on z/OS.
 *
 * @return The current z/OS storage protection key.
 *
 */

UDATA getStorageKey(void);

#endif /*if defined(J9ZOS390)*/

/**
 * Returns a string representing the type of page indicated by the given pageFlags.
 * Useful when printing page type.
 *
 * @param[in] pageFlags indicates type of the page.
 *
 * @return pointer to string representing the page type.
 */
const char *
getPageTypeString(UDATA pageFlags);

/**
 * Returns a string (with a leading space) representing the type of page indicated by the given pageFlags.
 * Useful when printing page type.
 *
 * @param[in] pageFlags indicates type of the page.
 *
 * @return pointer to string (with a leading space) representing the page type.
 */
const char *
getPageTypeStringWithLeadingSpace(UDATA pageFlags);


/* ---------------- gettimebase.c ---------------- */

/**
* @brief
* @param void
* @return U_64
*/
U_64
getTimebase(void);

/* ---------------- j9memclr.c ---------------- */

/**
* @brief
* @param void
* @return UDATA
*/
UDATA getCacheLineSize(void);


/**
* @brief
* @param *ptr
* @param length
* @return void
*/
void J9ZeroMemory(void *ptr, UDATA length);


/**
* @brief
* @param *dest
* @param value
* @param size
* @return void
*/
void j9memset(void * dest, IDATA value, UDATA size);

/* ---------------- archinfo.c ---------------- */
/**
 * @brief
 * @return I_32
 */

#define G5                              (9672)  /* Not Supported as of Java 1.5 */
#define MULTIPRISE7000                  (7060)  /* Not Supported as of Java 1.5 */
#define FREEWAY                         (2064)
#define Z800                            (2066)
#define MIRAGE                          (1090)
#define TREX                            (2084)
#define Z890                            (2086)
#define GOLDEN_EAGLE                    (2094)
#define DANU_GA2                        (2094)  /* type doesn't change from GOLDEN_EAGLE */
#define Z9BC                            (2096)
#define Z10                             (2097)
#define Z10BC                           (2098)  /* zMR */

I_32 get390zLinuxMachineType(void);

/* ---------------- thrname_core.c ---------------- */

struct OMR_VMThread;

/**
 * Lock and get the thread name. Must be paired with releaseOMRVMThreadName().
 * 
 * @param[in] vmThread The vmthread.
 * @return The thread name.
 */
char *getOMRVMThreadName(struct OMR_VMThread *vmThread);

/**
 * Try to lock and get the thread name. Fails if locking requires blocking.
 * If this call succeeds, it must be paired with releaseOMRVMThreadName().
 * Don't call releaseOMRVMThreadName() if this call failed.
 * 
 * @param[in] vmThread The vmthread.
 * @return NULL if we failed to get the lock, a non-NULL thread name if we succeeded.
 */
char *tryGetOMRVMThreadName(struct OMR_VMThread *vmThread);

/**
 * Unlock the thread name that was obtained using getOMRVMThreadName() or tryGetOMRVMThreadName().
 * @param[in] vmThread The vmthread.
 */
void releaseOMRVMThreadName(struct OMR_VMThread *vmThread);

/**
 * Lock the thread name, set it, and then unlock it.
 * The thread name may be freed by the VM.
 *
 * @param[in] currentThread The current vmthread.
 * @param[in] vmThread The vmthread whose name should be set.
 * @param[in] name The new thread name.
 * @param[in] nameIsStatic If non-zero, this indicates that the VM should not free the thread name.
 */
void setOMRVMThreadNameWithFlag(struct OMR_VMThread *currentThread, struct OMR_VMThread *vmThread, char *name, U_8 nameIsStatic);

/**
 * Set the thread name without locking it.
 *
 * @param[in] vmThread The vmthread whose name should be set.
 * @param[in] name The new thread name.
 * @param[in] nameIsStatic If non-zero, this indicates that the VM should not free the thread name.
 */
void setOMRVMThreadNameWithFlagNoLock(struct OMR_VMThread *vmThread, char *name, U_8 nameIsStatic);

#ifdef __cplusplus
}
#endif

#endif /* util_core_api_h */

