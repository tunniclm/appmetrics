#ifndef TRACEDATAPROVIDER_H_
#define TRACEDATAPROVIDER_H_

#include "ibmras/monitoring/Plugin.h"
#include "omr.h"
#include "omragent.h"
#include "ibmras/vm/omr/healthcenter.h"
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(WINDOWS)
#include <winsock2.h>
#define JLONG_FMT_STR "%I64d"
#else /* Unix platforms */
#define _OE_SOCKETS
#define JLONG_FMT_STR "%lld"
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

struct __traceBuffer {
	long size;
	unsigned char *buffer;
	struct __traceBuffer *next;
};
typedef struct __traceBuffer TRACEBUFFER;

struct __traceBufferQueue {
	TRACEBUFFER *head;
	TRACEBUFFER *tail;
};
typedef struct __traceBufferQueue TRACEBUFFERQUEUE;

struct __traceData {
	TRACEBUFFERQUEUE traceBufferQueue;
	TRACEBUFFERQUEUE freeBufferQueue;
	int droppedBufferCount;
	bool badMaxSizeWarningShown;
	I_32 traceBufferSize;
};
typedef struct __traceData TRACEDATA;



namespace plugins {
	namespace omr {
		namespace trace {

			class TraceDataProvider : ibmras::monitoring::Plugin {
			public:
				TraceDataProvider(omrRunTimeProviderParameters tDPP);
				virtual ~TraceDataProvider() {};  //destructor
			private:

			};

		} /* end namespace trace */
	} /* end namespace omr */
} /* end namespace plugins */

#endif /* TRACEDATAPROVIDER_H_ */