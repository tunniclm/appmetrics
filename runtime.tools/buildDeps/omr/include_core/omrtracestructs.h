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
#ifndef OMR_TRACE_STRUCTS_H
#define OMR_TRACE_STRUCTS_H

#include <limits.h>

#include "j9comp.h"
#include "omr.h"
#include "ute_module.h"
#include "ute_core.h"

#ifdef  __cplusplus
extern "C" {
#endif
#if 0 /* taken from ute_core.h */
/*
 * =============================================================================
 *  Constants
 * =============================================================================
 */
#define UT_VERSION                    5
#define UT_MODIFICATION               0

#define UT_APPLICATION_TRACE_MODULE 99


/*
 * =============================================================================
 *   Forward declarations for opaque types
 * =============================================================================
 */
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
	int32_t                  dataLength;
	volatile utsSubscriberCallback subscriber;
	volatile utsSubscriberAlarmCallback alarm;
	void                   *userData;
	/* internal fields - callers shouldn't modify these. */
	struct UtThreadData          **thr; /* unused */
	int32_t                  threadPriority; /* unused */
	int32_t                 threadAttach; /* unused */
	struct UtSubscription         *next;
	struct UtSubscription         *prev;
	struct subscription     *queueSubscription; /* unused */
	volatile uint32_t state;
} UtSubscription;

/*
 * =============================================================================
 *  UT data header
 * =============================================================================
 */
typedef struct UtDataHeader {
	char eyecatcher[4];
	int32_t length;
	int32_t version;
	int32_t modification;
} UtDataHeader;


#endif /* taken from ute_core.h */

#ifdef  __cplusplus
}
#endif
#endif /* !OMR_TRACE_STRUCTS_H */
