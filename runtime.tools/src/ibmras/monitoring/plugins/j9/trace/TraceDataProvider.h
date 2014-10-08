#ifndef ibmras_monitoring_plugins_j9_trace_tracedataprovider_h
#define ibmras_monitoring_plugins_j9_trace_tracedataprovider_h

#include <map>
#include "jvmti.h"
#include "ibmjvmti.h"
#include "jni.h"
#include "ibmras/monitoring/Plugin.h"
#include "ibmras/vm/java/healthcenter.h"
#include "ibmras/monitoring/plugins/j9/trace/TraceReceiver.h"



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

void handleCommand(const std::string &command, const std::vector<std::string> &parameters);
void publishConfig();
void* processLoop(ibmras::common::port::ThreadData* param);
void enableTracePoints(const char* tracePoints[]);
void disableTracePoints(const char* tracePoints[]);
void enableTracePoint(const std::string &tp);
void disableTracePoint(const std::string &tp);
void enableGCTracePoint(const std::string &tp);
void enableNormalTracePoint(const std::string &tp);
void disableExceptionTracePoint(const std::string &tp);
void enableExceptionTracePoint(const std::string &tp);
void disableNormalTracePoint(const std::string &tp);
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
int registerVerboseGCSubscriber(std::string fileName);
jvmtiError verboseGCSubscriber(jvmtiEnv *env, const char *record, jlong length, void *userData);
jvmtiError verboseGCAlarm(jvmtiEnv *env, void *subscriptionID, void *userData);
void handleVerboseGCSetting(std::string value);
std::string getWriteableDirectory();
std::string getString(JNIEnv* env, const std::string& cname, const std::string& mname, const std::string& signature, jstring args);


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
