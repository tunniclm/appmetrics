/*
	(c) Copyright IBM Corp. 1998, ${uma.year}  All Rights Reserved
	C language defines for included modules.

	File generated in stream: ${uma.buildinfo.stream}
*/


#ifndef J9CFG_H
#define J9CFG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "j9importstrings.h"

#define J9_COPYRIGHT_STRING "(c) Copyright IBM Corp. 1991, ${uma.year} All Rights Reserved"
#define J9_COPYRIGHT_DYNAMIC_CURRENT_YEAR_STRING "(c) Copyright IBM Corp. 1991, %s All Rights Reserved"
#define J9_COPYRIGHT_CURRENT_YEAR_STRING "${uma.year}"
#define ${uma.spec.properties.threadLibraryDefine.value} 1

#define EsVersionMajor ${uma.buildinfo.version.major}
#define EsVersionMinor ${uma.buildinfo.version.minor}0
#define EsVersionString "${uma.buildinfo.version.major}.${uma.buildinfo.version.minor}"
#define EsExtraVersionString ""

/*  Note: The following defines record flags used to build VM.  */
/*  Changing them here does not remove the feature and may cause linking problems. */

<#list uma.spec.flags as flag>
<#if flag.enabled && flag.cname_defined>
#define ${flag.cname}
</#if>
</#list>

/* flags NOT used by this VM.  */
<#list uma.spec.flags as flag>
<#if !flag.enabled && flag.cname_defined>
#undef ${flag.cname}
</#if>
</#list>

#ifdef __cplusplus
}
#endif

#endif /* J9CFG_H */
