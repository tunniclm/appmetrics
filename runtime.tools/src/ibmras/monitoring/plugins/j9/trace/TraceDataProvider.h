#ifndef ibmras_monitoring_plugins_j9_trace_tracedataprovider_h
#define ibmras_monitoring_plugins_j9_trace_tracedataprovider_h

#include <map>
#include "jvmti.h"
#include "ibmjvmti.h"
#include "jni.h"
#include "ibmras/monitoring/Plugin.h"
#include "ibmras/vm/java/healthcenter.h"
#include "ibmras/monitoring/plugins/j9/trace/TraceReceiver.h"

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
	jlong size;
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
	jboolean badMaxSizeWarningShown;
	jint traceBufferSize;
	jrawMonitorID monitor;
};
typedef struct __traceData TRACEDATA;


namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace trace {

void controlTracePoints(std::string command, std::string subsystem);
void publishConfig();
void* processLoop(ibmras::common::port::ThreadData* param);
void enableTracePoints(std::string s[]);
void disableTracePoints(std::string s[]);
void enableTracePoint(std::string tp);
void disableTracePoint(std::string tp);
void enableGCTracePoint(std::string tp);
void enableNormalTracePoint(std::string tp);
void disableExceptionTracePoint(std::string tp);
void enableExceptionTracePoint(std::string tp);
void disableNormalTracePoint(std::string tp);
bool startTraceSubscriber(long maxCircularBufferSize, int traceBufferSize);
jlong htonjl(jlong l);
monitordata* generateData(uint32 sourceID, char *dataToSend, int size);
TRACEBUFFER *allocateTraceBuffers(jvmtiEnv *jvmtienv, jlong maxBufferSize, jint bufferSize);
TRACEBUFFER *allocateTraceBuffer(jvmtiEnv *jvmtienv, jlong length);
jvmtiError traceSubscriber(jvmtiEnv *pti, void *record, jlong length,void *userData);
void freeTraceBuffer(jvmtiEnv *jvmtienv, TRACEBUFFER *buffer);
TRACEBUFFER *queueGet(TRACEBUFFERQUEUE *queue, int maxNumberOfItems);
void queuePut(TRACEBUFFERQUEUE *queue, TRACEBUFFER *buffer);
void initializeTraceUserData();
int sendTraceBuffers(int maxSize);
void sendTraceHeader(bool persistent);

class TraceDataProvider : public ibmras::monitoring::Plugin {
public:
	static TraceDataProvider* getInstance(jvmFunctions tDPP);
	static TraceDataProvider* getInstance();
	TraceDataProvider(jvmFunctions tDPP);
	virtual ~TraceDataProvider(){};  //destructor
	static void* getReceiver();

private:
	TraceReceiver *traceReceiver;
	TraceReceiver* getTraceReceiver();
};


}	/* end namespace trace */
}	/* end namespace j9 */
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */


#endif /* ibmras_monitoring_plugins_j9_trace_tracedataprovider_h */
