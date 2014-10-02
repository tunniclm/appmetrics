/*
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 */

#if !defined(J9LIB_H)
#define J9LIB_H

#ifdef __cplusplus
extern"C"{
#endif

#define J9_DLL_VERSION_STRING "28"

#define J9_DEFAULT_JCL_DLL_NAME "jclscar_28"


/* We need certain defines... this is a big hack until we can do this another way.*/
#ifndef J9_VERBOSE_DLL_NAME
#define J9_VERBOSE_DLL_NAME "j9vrb"
#endif
#ifndef J9_IFA_DLL_NAME
#define J9_IFA_DLL_NAME "j9ifa"
#endif
#ifndef J9_JIT_DEBUG_DLL_NAME
#define J9_JIT_DEBUG_DLL_NAME "j9jitd28"
#endif
#ifndef J9_VM_DLL_NAME
#define J9_VM_DLL_NAME "j9vm28"
#endif
#ifndef J9_STRICT_MATH_DLL_NAME
#define J9_STRICT_MATH_DLL_NAME "j9fdm28"
#endif
#ifndef J9_SHARED_DLL_NAME
#define J9_SHARED_DLL_NAME "j9shr28"
#endif
#ifndef J9_VERIFY_DLL_NAME
#define J9_VERIFY_DLL_NAME "j9bcv28"
#endif
#ifndef J9_DEBUG_DLL_NAME
#define J9_DEBUG_DLL_NAME "j9dbg28"
#endif
#ifndef J9_GC_DLL_NAME
#define J9_GC_DLL_NAME "j9gc28"
#endif
#ifndef J9_HOOKABLE_DLL_NAME
#define J9_HOOKABLE_DLL_NAME "j9hookable28"
#endif
#ifndef J9_RAS_DUMP_DLL_NAME
#define J9_RAS_DUMP_DLL_NAME "j9dmp28"
#endif
#ifndef J9_DYNLOAD_DLL_NAME
#define J9_DYNLOAD_DLL_NAME "j9dyn28"
#endif
#ifndef J9_CHECK_JNI_DLL_NAME
#define J9_CHECK_JNI_DLL_NAME "j9jnichk28"
#endif
#ifndef J9_CHECK_VM_DLL_NAME
#define J9_CHECK_VM_DLL_NAME "j9vmchk28"
#endif
#ifndef J9_SIDECAR_DLL_NAME
#define J9_SIDECAR_DLL_NAME "jclscar_28"
#endif
#ifndef J9_THREAD_DLL_NAME
#define J9_THREAD_DLL_NAME "j9thr28"
#endif
#ifndef J9_GATEWAY_RESMAN_DLL_NAME
#define J9_GATEWAY_RESMAN_DLL_NAME "jclrm_28"
#endif
#ifndef J9_JIT_DLL_NAME
#define J9_JIT_DLL_NAME "j9jit28"
#endif
#ifndef J9_JVMTI_DLL_NAME
#define J9_JVMTI_DLL_NAME "j9jvmti28"
#endif
#ifndef J9_VERBOSE_DLL_NAME
#define J9_VERBOSE_DLL_NAME "j9vrb28"
#endif
#ifndef J9_PORT_DLL_NAME
#define J9_PORT_DLL_NAME "j9prt28"
#endif
#ifndef J9_MAX_DLL_NAME
#define J9_MAX_DLL_NAME "jclmax_28"
#endif
#ifndef J9_RAS_TRACE_DLL_NAME
#define J9_RAS_TRACE_DLL_NAME "j9trc28"
#endif
#ifndef J9_JEXTRACT_DLL_NAME
#define J9_JEXTRACT_DLL_NAME "j9jextract"
#endif
#ifndef J9_JCL_CLEAR_DLL_NAME
#define J9_JCL_CLEAR_DLL_NAME "jclclear_28"
#endif
#ifndef J9_JCL_HS60_DLL_NAME
#define J9_JCL_HS60_DLL_NAME "jclhs60_28"
#endif
#ifndef J9_ZIP_DLL_NAME
#define J9_ZIP_DLL_NAME "j9zlib28"
#endif

/* This is an old DLL that should be removed later. */
#define J9_AOT_DEBUG_DLL_NAME "bogus old dll"

#ifdef __cplusplus
}
#endif

#endif /* J9LIB_H */
