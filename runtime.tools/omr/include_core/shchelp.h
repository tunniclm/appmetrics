/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2001, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#include "j9comp.h"

#ifndef SHCHELP_H
#define SHCHELP_H

#define J9SH_MODLEVEL_JAVA5 1
#define J9SH_MODLEVEL_JAVA6 2
#define J9SH_MODLEVEL_JAVA7 3
#define J9SH_MODLEVEL_JAVA8 4

/*	JVM feature bit flag(s)	*/
/*
 * if there is a need for double digit features such as 10
 * version string leng is going to increase by 1
 * J9SH_VERSION_STRING_LEN need to be changed accordingly *
 */
#define J9SH_FEATURE_MT 1

#define J9SH_ADDRMODE_32 32
#define J9SH_ADDRMODE_64 64

#ifdef J9VM_ENV_DATA64
#define J9SH_ADDRMODE J9SH_ADDRMODE_64
#else
#define J9SH_ADDRMODE J9SH_ADDRMODE_32
#endif

#define J9SH_VERSION_PREFIX_CHAR 'C'
#define J9SH_MODLEVEL_G07ANDLOWER_CHAR 'D'
#define J9SH_MODLEVEL_PREFIX_CHAR 'M'
#define J9SH_FEATURE_PREFIX_CHAR 'F'
#define J9SH_ADDRMODE_PREFIX_CHAR 'A'
#define J9SH_PERSISTENT_PREFIX_CHAR 'P'
#define J9SH_PREFIX_SEPARATOR_CHAR '_'
#if defined(J9VM_OPT_VIRT)
#define J9SH_TARGET_PREFIX_CHAR 'T'
#endif /* J9VM_OPT_VIRT */

#define J9SH_GENERATION_29		29
#define J9SH_GENERATION_07		7

#define J9SH_VERSION_STRING_G07ANDLOWER_SPEC "C%dD%dA%d"
#define J9SH_VERSION_STRING_G07TO29_SPEC "C%dM%dA%d"
#if defined(J9VM_OPT_VIRT)
/* Use this version string only when crossguest sharing has been
 * enabled; otherwise, fallback on the already existing one.
 */
#define J9SH_VERSION_STRING_SPEC_CG "C%dM%dA%dT%s"
#endif /* J9VM_OPT_VIRT */
#define J9SH_VERSION_STRING_SPEC "C%dM%dF%xA%d"
#define J9SH_VERSION_STRING_LEN 12
#define J9SH_VERSTRLEN_INCREASED_SINCEG29 2

typedef struct J9PortShcVersion {
    U_32 esVersionMajor;
    U_32 esVersionMinor;
    U_32 modlevel;
    U_32 addrmode;
    U_32 cacheType;
    U_32 feature;
} J9PortShcVersion;

#define J9PORT_SHR_CACHE_TYPE_PERSISTENT  1
#define J9PORT_SHR_CACHE_TYPE_NONPERSISTENT  2
#define J9PORT_SHR_CACHE_TYPE_VMEM  3
#define J9PORT_SHR_CACHE_TYPE_CROSSGUEST  4

#endif /* SHCHELP_H */
