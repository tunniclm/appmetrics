/*
*	(c) Copyright IBM Corp. 1991, 2006 All Rights Reserved
*/

/*
 * =============================================================================
 * IBM Confidential
 * OCO Source Materials
 *
 * IBM Universal Trace Engine v1.0
 * (C) Copyright IBM Corp. 1998, 2011.
 *
 * The source code for this program is not published or otherwise divested of
 * its trade secrets, irrespective of what has been deposited with the U.S.
 * Copyright office.
 * =============================================================================
 */
#ifndef _IBM_UTE_CORE_H
#define _IBM_UTE_CORE_H

#include <limits.h>

#include "j9comp.h"
#include "omr.h"
#include "ute_module.h"

#ifdef  __cplusplus
extern "C" {
#endif

/*
 * =============================================================================
 *  Constants
 * =============================================================================
 */
#define UT_VERSION                    5
#define UT_MODIFICATION               0
#define UT_TRACEFMT_VERSION_MOD       (float)1.2

#define UT_SUSPEND_GLOBAL             1
#define UT_SUSPEND_THREAD             2
#define UT_RESUME_GLOBAL              1
#define UT_RESUME_THREAD              2

#define UT_APPLICATION_TRACE_MODULE 99


/*
 * =============================================================================
 *   Forward declarations for opaque types
 * =============================================================================
 */
typedef void (*UtListenerWrapper)(void *userData, OMR_VMThread *env, void **thrLocal,
	const char *modName, U_32 traceId, const char * format, va_list varargs);

struct UtSubscription;

typedef omr_error_t (*utsSubscriberCallback)(struct UtSubscription *subscription);
typedef void (*utsSubscriberAlarmCallback)(struct UtSubscription *subscription);

/*
 * ======================================================================
 *  Trace buffer subscriber data
 * ======================================================================
 */
typedef struct UtSubscription {
	char                   *description;
	void                   *data;
	I_32                  dataLength;
	volatile utsSubscriberCallback subscriber;
	volatile utsSubscriberAlarmCallback alarm;
	void                   *userData;
	/* internal fields - callers shouldn't modify these. */
	struct UtThreadData          **thr;
	I_32                  threadPriority;
	I_32                 threadAttach;
	struct UtSubscription         *next;
	struct UtSubscription         *prev;
	struct subscription     *queueSubscription;
	j9thread_monitor_t shutdownCond;
	volatile BOOLEAN subscriberActive;
} UtSubscription;

/*
 * =============================================================================
 *  UT data header
 * =============================================================================
 */
typedef struct UtDataHeader {
	char eyecatcher[4];
	I_32 length;
	I_32 version;
	I_32 modification;
} UtDataHeader;

/*
 * =============================================================================
 *  UT thread data
 * =============================================================================
 */
/* If you modify this structure, please update dbgext_utthreaddata in dbgTrc.c to cope with the
 * modifications.
 */
typedef struct UtThreadData {
	UtDataHeader	header;
	const void		*id;						/* Thread identifier               */
	const void		*synonym1;					/* Alternative thread identifier   */
	const void		*synonym2;					/* And another thread identifier   */
	const char		*name;						/* Thread name                     */
	unsigned char 	currentOutputMask;			/* Bitmask containing the options  */
	/* for the tracepoint currently being formatted*/
	struct UtTraceBuffer	*trcBuf;					/* Trace buffer                    */
	void			*external;					/* External trace thread local ptr */
	I_32			suspendResume;				/* Suspend / resume count          */
	int				recursion;					/* Trace recursion indicator       */
	int				indent;						/* Iprint indentation count        */
} UtThreadData;

/*
 * =============================================================================
 *   The interface for calls out of UT to the client
 * =============================================================================
 */
#define UT_CLIENT_INTF_NAME "UTCI"
struct  UtClientInterface {
	UtDataHeader  header;
	/** DEAD STUB **/
};

/*
 * =============================================================================
 *   The server interface for calls into UT
 * =============================================================================
 */
#define UT_SERVER_INTF_NAME "UTSI"
/*
 * UtServerInterface is embedded in J9UtServerInterface.
 */
struct UtServerInterface {
	void   (*TraceVNoThread)(UtModuleInfo *modInfo, U_32 traceId, const char *spec, va_list varArgs);
	omr_error_t (*TraceSnap)(UtThreadData **thr, char *label, char **response);
	omr_error_t (*TraceSnapWithPriority)(UtThreadData **thr, char *label, I_32 snapPriority, char **response, I_32 sync);
	omr_error_t (*AddComponent)(UtModuleInfo *modInfo, const char **fmt);
	omr_error_t (*GetComponents)(UtThreadData **thr, char ***list, I_32 *number);
	omr_error_t (*GetComponent)(char *name, unsigned char **bitmap, I_32 *first, I_32 *last);
	I_32 (*Suspend)(UtThreadData **thr, I_32 type);
	I_32 (*Resume)(UtThreadData **thr, I_32 type);

	struct UtTracePointIterator * (*GetTracePointIteratorForBuffer)(UtThreadData **thr, const char *bufferName);
	char * (*FormatNextTracePoint)(struct UtTracePointIterator *iter, char *buffer, U_32 buffLen);
	omr_error_t (*FreeTracePointIterator)(UtThreadData **thr, struct UtTracePointIterator *iter);
	omr_error_t (*RegisterRecordSubscriber)(UtThreadData **thr, const char *description, utsSubscriberCallback func, utsSubscriberAlarmCallback alarm, void *userData, struct UtTraceBuffer *start, struct UtTraceBuffer *stop, UtSubscription **record, I_32 attach);
	omr_error_t (*DeregisterRecordSubscriber)(UtThreadData **thr, UtSubscription *subscriptionID);
	omr_error_t (*FlushTraceData)(UtThreadData **thr, struct UtTraceBuffer **first, struct UtTraceBuffer **last, I_32 pause);
	omr_error_t (*GetTraceMetadata)(void **data, I_32 *length);
	void (*DisableTrace)(void);
	void (*EnableTrace)(void);
	omr_error_t (*RegisterTracePointSubscriber)(UtThreadData **thr, char *description, utsSubscriberCallback callback, utsSubscriberAlarmCallback alarm, void *userData, UtSubscription **subscription);
	omr_error_t (*DeregisterTracePointSubscriber)(UtThreadData **thr, UtSubscription *subscription);
	omr_error_t (*TraceRegister)(UtThreadData **thr, UtListenerWrapper func, void *userData);
	omr_error_t (*TraceDeregister)(UtThreadData **thr, UtListenerWrapper func, void *userData);
	omr_error_t (*SetOptions)(UtThreadData **thr, const char *opts[]);
};


/*
 * =============================================================================
 * Language interface - embedded in UtGlobalData
 *
 * This interface supplies callback functions for language-specific behaviour.
 * A callback may be NULL. We will NULL-check each function pointer before invoking it.
 * =============================================================================
 */
typedef omr_error_t (*SetLanguageTraceOptionFunc)(const OMR_VM *omr_vm, const char *optName, const char* optValue, BOOLEAN atRuntime);
typedef void (*ReportCommandLineErrorFunc)(J9PortLibrary* portLibrary, const char* detailStr, va_list args);

typedef struct OMRTraceLanguageInterface {
	/**
	 * Attach the current thread to the language VM.
	 *
	 * The trace engine invokes this callback to attach internal threads to the VM.
	 * A sample implementation is omr_trc_defaultAttachCurrentThreadToLanguageVM().
	 * Attached threads will be detached using the DetachCurrentThreadFromLanguageVM() callback.
	 *
	 * @pre The current thread is completely unattached to the VM.
	 *
	 * @param[in] omrVM The OMR VM.
	 * @param[in] name The name of the current thread.
	 * @param[out] omrVMThread Location where a pointer to a new OMR VM thread should be stored. Must not be NULL.
	 *
	 * @return an OMR error code
	 */
	omr_error_t	(*AttachCurrentThreadToLanguageVM)(OMR_VM *omrVM, const char *name, OMR_VMThread **omrVMThread);

	/**
	 * Detach the current thread from the language VM.
	 *
	 * The trace engine invokes this callback to detach internal threads to the VM.
	 * A sample implementation is omr_trc_defaultDetachThreadFromLanguageVM().
	 * The current thread should have been attached using the AttachCurrentThreadToLanguageVM() callback.
	 *
	 * @pre The current thread is attached to the VM.
	 *
	 * @param[in,out] omrVMThread The OMR VM thread.
	 * @return an OMR error code
	 */
	omr_error_t (*DetachCurrentThreadFromLanguageVM)(OMR_VMThread *omrVMThread);

	/**
	 * Pass unhandled trace options for implementor to handle.
	 */
	SetLanguageTraceOptionFunc SetLanguageTraceOption;

	/**
	 * Report errors with trace options to the implementor's error stream.
	 */
	ReportCommandLineErrorFunc ReportCommandLineError;
} OMRTraceLanguageInterface;

#ifdef  __cplusplus
}
#endif
#endif /* !_IBM_UTE_CORE_H */
