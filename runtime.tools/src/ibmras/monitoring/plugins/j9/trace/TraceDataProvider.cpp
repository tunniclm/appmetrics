/*
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools for Java\u2122 - Health Center
 * Ã¯Â¿Â½ Copyright IBM Corp. 2008, 2014
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#include "ibmras/monitoring/plugins/j9/trace/TraceDataProvider.h"
#include "ibmras/monitoring/agent/Agent.h"

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <map>

#include "ibmras/common/logging.h"


struct __jdata;



namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace trace {

uint32 provID;
PUSH_CALLBACK sendDataToAgent;
IBMRAS_DEFINE_LOGGER("TraceDataProvider");
jvmFunctions vmData;
char *traceMetadata = NULL;
int headerSize = 0;
int DEFAULT_MAX_CIRCULAR_BUFFER_SIZE = 4000000;
int initialisedTraceBuffers = 0;
TRACEDATA traceData;
uint buffersDroppedBeforeFirstConnection = 0;
uint buffersDropped = 0;
uint buffersNotDropped = 0;
int firstConnectionMade = 0;
int countDroppedBuffers = 0;
bool running = false;
std::map<std::string, std::string> config;


std::string profiling[]={"j9jit.15","j9jit.16", "j9jit.17", "j9jit.18",""};
std::string gc[] = {"j9mm.1", "j9mm.2", "j9mm.50", "j9mm.51",
		"j9mm.52", "j9mm.53", "j9mm.54", "j9mm.55", "j9mm.56","j9mm.57",
		"j9mm.58", "j9mm.59", "j9mm.60", "j9mm.64","j9mm.65", "j9mm.68",
		"j9mm.69", "j9mm.71", "j9mm.72","j9mm.73", "j9mm.74", "j9mm.75",
		"j9mm.85", "j9mm.86","j9mm.90", "j9mm.91", "j9mm.94", "j9mm.131",
		"j9mm.132","j9mm.133", "j9mm.134", "j9mm.135", "j9mm.136", "j9mm.137",
		"j9mm.138", "j9mm.139", "j9mm.231", "j9mm.234", "j9mm.279","j9mm.280",
		"j9mm.281", "j9mm.282", "j9jit.2","j9jit.4", "j9jit.5", "j9jit.6",
		"j9jit.9","j9jit.11", "j9jit.12", "j9jit.18", "j9mm.383", "j9mm.384",
		"j9mm.345", "j9mm.346", "j9mm.347", "j9mm.348", "j9mm.463","j9mm.464",
		"j9mm.467", "j9mm.468", "j9mm.469", "j9mm.470","j9mm.560","j9mm.474",
		"j9mm.475", "j9mm.395", "j9mm.285", "j9mm.286",""};
std::string classes[] =  { "j9bcu.1", "j9shr.51", "j9bcverify.14", "j9bcverify.18",""};
std::string jit[] = { "j9jit.1", "j9jit.20", "j9jit.21", "j9jit.22", "j9jit.23", "j9jit.24",
		"j9jit.28", "j9jit.29" ,""};
std::string io[] = {"IO.100", "IO.101", "IO.102",
				"IO.103", "IO.104", "IO.105", "IO.106", "IO.107", "IO.108",
				"JAVA.315", "JAVA.316", "JAVA.317", "JAVA.318", "JAVA.319",
				"JAVA.320", "JAVA.321", "JAVA.322", "JAVA.323",
				"j9scar.35", "j9scar.36", "j9scar.37", "j9scar.38",
				"j9scar.136", "j9scar.137", "j9scar.138", "j9scar.139", "j9scar.140" ,""};

struct Wrapper
{
    std::string points[100];
};

/**
 * the agent calls registerPushSource to find out which data sources we have
 * and will provide to us a provID which we use for the callbacks.
 *
 * ProvID value needs to be store
 *
 * also given a callback pointer which is a pointer to a function that we
 * need to push the data to.  All data will be pushed into the same function regardless
 * of which data provider it comes from.  for us though in this provider, its invisible
 * that this happens as it may not scale.  but we just use the callback as given anyway
 */
pushsource* registerPushSource(void (*callback)(monitordata* data),
		uint32 provID) {
	pushsource *src = new pushsource();
	src->header.name = "trace";
	src->header.description = "Data provided by jvmti trace engine";
	/*
	 * the sourceID is for the plugin to manage. If we have more than 1 source
	 * then we need to give each source (in this plugin) a different number
	 */
	src->header.sourceID = 0;
	src->next = NULL;
	src->header.capacity = 1048576; /* 1MB bucket capacity */
	src->header.config = "gc_subsystem=on\nprofiling_subsystem=on\nclasses_subsystem=on\nio_subsystem=on\njit_subsystem=on";

	ibmras::monitoring::plugins::j9::trace::provID = provID;
	ibmras::monitoring::plugins::j9::trace::sendDataToAgent = callback;
	return src;
}

/**
 * The start() method starts the plugin and is the method called from the setup ibmras::monitoring::Plugin* getPlugin()
 * function above
 */
int Tracestart() {

	IBMRAS_DEBUG(debug,  "Tracestart enter");

	int result = 0;
	int bufferSize = 0;
	int rc = 0;
	char* tempMeta;
	int tempHeaderSize = 0;
	long maxCircularBufferSize = DEFAULT_MAX_CIRCULAR_BUFFER_SIZE;
	/* this is the eye catcher that tells the health center client trace parser
	 * that this is a header record */
	char METADATA_EYE_CATCHER[] = { 'H', 'C', 'T', 'H' };

	initializeTraceUserData();

	/* get the trace header data from the vm */
	if (vmData.jvmtiGetTraceMetadata != 0) {
		rc = vmData.jvmtiGetTraceMetadata(vmData.pti, &tempMeta, &tempHeaderSize);
	}

	if (tempMeta == NULL || rc != JVMTI_ERROR_NONE) {
		IBMRAS_DEBUG(warning,  "failed to get trace header");
		return -1;
	}

	jlong traceHeaderLength = (jlong) tempHeaderSize;
	traceHeaderLength = htonjl(traceHeaderLength);

	// the size of the trace buffer is at position 17 in the array
	bufferSize = *((int*) &tempMeta[16]);

	headerSize = sizeof(METADATA_EYE_CATCHER) + sizeof(traceHeaderLength)
			+ tempHeaderSize;

	traceMetadata = (char*) malloc(headerSize);
	/* set everything to 0 */
	memset(traceMetadata, 0, (headerSize));

	/* now build up the traceMetaData structure starting with the eyecatcher */
	memcpy(traceMetadata, METADATA_EYE_CATCHER, sizeof(METADATA_EYE_CATCHER));

	/* now add the record of the length of the trace header */
	memcpy(traceMetadata + sizeof(METADATA_EYE_CATCHER), &traceHeaderLength,
			sizeof(traceHeaderLength));

	/* now add the trace header data itself */
	memcpy(
			traceMetadata + sizeof(METADATA_EYE_CATCHER)
					+ sizeof(traceHeaderLength), tempMeta, tempHeaderSize);

	sendTraceHeader(true);
	/* start the trace subscriber */
	startTraceSubscriber(maxCircularBufferSize, bufferSize);

	/* start a loop that will publish the data coming back from the trace subscriber */
	ibmras::common::port::ThreadData* data =
			new ibmras::common::port::ThreadData(processLoop);
	result = ibmras::common::port::createThread(data);
	if (result) {
		IBMRAS_DEBUG(debug,  "processLoop thread failed to start");
		// do something clever as it failed to start
	} else {
		IBMRAS_DEBUG(debug,  "processLoop thread started ok");
	}


	/* turn off all trace to start with */
	vmData.setTraceOption(vmData.pti, "none=all,maximal=mt");

	/* now enable HC specific trace */
	enableTracePoints(gc);
	enableTracePoints(profiling);
	enableTracePoints(classes);
	enableTracePoints(jit);
	enableTracePoints(io);
	config["gc_subsystem"]="on";
	config["profiling_subsystem"]="on";
	config["classes_subsystem"]="on";
	config["jit_subsystem"]="on";
	config["io_subsystem"]="on";

	IBMRAS_DEBUG(debug,  "Tracestart exit");
	return 0;
}

int Tracestop() {
	running = false;
	return 0;

}

TraceDataProvider* instance = NULL;
TraceDataProvider* TraceDataProvider::getInstance(jvmFunctions tDPP) {
	if (!instance) {
		instance = new TraceDataProvider(tDPP);
	}
	return instance;
}

TraceDataProvider* TraceDataProvider::getInstance() {
	if (!instance) {
		return NULL;
	}
	return instance;
}


TraceReceiver* TraceDataProvider::getTraceReceiver() {
	if (traceReceiver == NULL) {
		traceReceiver = new TraceReceiver();
	}
	return traceReceiver;
}

void* TraceDataProvider::getReceiver() {
	return getInstance()->getTraceReceiver();
}

TraceDataProvider::TraceDataProvider(jvmFunctions tDPP) {
	traceReceiver = NULL;
	vmData = tDPP;
	name = "Trace data provider";
	pull = NULL;
	push = registerPushSource;
	start = Tracestart;
	stop = Tracestop;
	handle = NULL;
	type = ibmras::monitoring::plugin::data | ibmras::monitoring::plugin::receiver;
	recvfactory = (RECEIVER_FACTORY)TraceDataProvider::getReceiver;
	confactory = NULL;
}

void enableTracePoints(std::string s[]) {

	IBMRAS_DEBUG(debug,  "start of turning on tracepoints");

	for(int i=0;!s[i].empty(); i++){
		enableTracePoint(s[i]);
	}

	IBMRAS_DEBUG(debug,  "end of turning on tracepoints");
}

void disableTracePoints(std::string s[]) {

	IBMRAS_DEBUG(debug,  "start of turning off tracepoints");

	for (int i=0;!s[i].empty(); i++) {
		disableTracePoint(s[i]);
	}
	IBMRAS_DEBUG(debug,  "end of turning off tracepoints");
}

void controlSubsystem(std::string command, std::string points[]) {
	if (command == "off") {
		disableTracePoints(points);
	} else if (command == "on") {
		enableTracePoints(points);
	}
}

void controlTracePoints(std::string command, std::string subsystem) {
	JNIEnv * env;
	vmData.theVM->AttachCurrentThread((void **) &env, NULL);

	if (subsystem == "gc_subsystem") {
		controlSubsystem(command, gc);
	} else if (subsystem == "profiling_subsystem") {
		controlSubsystem(command, profiling);
	} else if (subsystem == "classes_subsystem") {
		controlSubsystem(command, classes);
	} else if (subsystem == "jit_subsystem") {
		controlSubsystem(command, jit);
	} else if (subsystem == "io_subsystem") {
		controlSubsystem(command, io);
	}
	//update the config info
	config[subsystem] = command;
	publishConfig();

	vmData.theVM->DetachCurrentThread();
}


void enableTracePoint(std::string tp) {
	if (tp.find("j9mm.")!=std::string::npos) {
		enableGCTracePoint(tp);
	} else {
		enableNormalTracePoint(tp);
	}
}

void disableTracePoint(std::string tp) {
	if (tp.find("j9mm.")!=std::string::npos) {
		disableExceptionTracePoint(tp);
	} else {
		disableNormalTracePoint(tp);
	}
}

void enableGCTracePoint(std::string tp){
	disableNormalTracePoint(tp);
	enableExceptionTracePoint(tp);
}

void enableNormalTracePoint(std::string tp) {
	std::string command = "maximal=tpnid{" + tp + "}";
	vmData.setTraceOption(vmData.pti, command.c_str());
}


void disableExceptionTracePoint(std::string tp) {
	int rc = 0;
	std::string command = "exception=!tpnid{" + tp +"}";
	rc = vmData.setTraceOption(vmData.pti, command.c_str());
}

void enableExceptionTracePoint(std::string tp) {
	IBMRAS_DEBUG(debug,  "in enableExceptionTracePoint");

	std::string command = "exception=tpnid{" + tp + "}";
	vmData.setTraceOption(vmData.pti, command.c_str());
}

void disableNormalTracePoint(std::string tp) {
	int rc = 0;
	std::string command = "maximal=!tpnid{" + tp +"}";
	rc = vmData.setTraceOption(vmData.pti, command.c_str());
}

/*
 * Converts a jlong from host to network byte order (big endian)
 */
jlong htonjl(jlong l) {
	jint byteOrderTest = 0x01020304;
	if (byteOrderTest == htonl(byteOrderTest)) {
		/* big endian */
		return l;
	} else {
		jint hi = (jint)(l >> 32);
		jint lo = (jint)(l & 0xffffffff);
		jlong convhi = htonl(hi);
		jlong convlo = htonl(lo);
		/* little endian */
		return (jlong)((convlo << 32) | (convhi & 0xffffffff));
	}
}

monitordata* generateData(uint32 sourceID, char *dataToSend, int size) {
	monitordata* data = new monitordata;
	data->provID = provID;
	data->data = dataToSend;
	data->size = size;
	data->sourceID = sourceID;
	data->persistent = false;
	return data;
}

TRACEBUFFER *
allocateTraceBuffer(jvmtiEnv *jvmtienv, jlong length) {
	TRACEBUFFER *buffer = NULL;
	jvmtiError rc;
	rc = jvmtienv->Allocate(sizeof(TRACEBUFFER), (unsigned char**) &buffer);
	if (rc != JVMTI_ERROR_NONE) {
		IBMRAS_DEBUG_2(debug,
				"allocateTraceBuffer: unable to allocate %ld bytes for trace buffer wrapper (rc=%d).",
				sizeof(TRACEBUFFER), rc);
	} else {
		buffer->buffer = NULL;
		buffer->next = NULL;
		rc = jvmtienv->Allocate(length, &(buffer->buffer));
		if (rc == JVMTI_ERROR_NONE) {
			buffer->size = length;
		} else {
			IBMRAS_DEBUG_2(debug,
					"allocateTraceBuffer unable to allocate " JLONG_FMT_STR " bytes for trace buffer (rc=%d).",
					length, rc);
			if (JVMTI_ERROR_NONE
					!= jvmtienv->Deallocate((unsigned char*) buffer)) {
				IBMRAS_DEBUG(debug,
						"allocateTraceBuffer unable to deallocate memory.");
			}
			buffer = NULL;
		}
	}
	return buffer;
}
TRACEBUFFER *
allocateTraceBuffers(jvmtiEnv *jvmtienv, jlong maxBufferSize, jint bufferSize) {
	TRACEBUFFER *buffers = NULL;
	int i, numberOfBuffers;
	jlong wrappedBufferLength = 4 + sizeof(jlong) + bufferSize;

	//IBMRAS_DEBUG(debug,  "> allocateTraceBuffers");
	assert(wrappedBufferLength > 0);
	numberOfBuffers = (maxBufferSize / wrappedBufferLength);

	/* We need at least one buffer to correctly function */
	if (numberOfBuffers <= 0) {
		numberOfBuffers = 1;
	}
	for (i = 0; i < numberOfBuffers; i++) {
		TRACEBUFFER *newBuffer = allocateTraceBuffer(jvmtienv,
				wrappedBufferLength);
		if (newBuffer == NULL) {
			continue;
		}
		/* Chain allocated buffers to the new one */
		if (buffers != NULL) {
			newBuffer->next = buffers;
		}
		buffers = newBuffer;
	}
	return buffers;
}


bool startTraceSubscriber(long maxCircularBufferSize, int traceBufferSize) {
	IBMRAS_DEBUG(debug,  "> startTraceSubscriber");
	if (vmData.jvmtiGetTraceMetadata != 0 && vmData.jvmtiRegisterTraceSubscriber != 0) {
		void *subscriptionID;
		TRACEBUFFER* traceBufferChain;

		if (initialisedTraceBuffers == 0) {
			traceBufferChain = allocateTraceBuffers(vmData.pti, maxCircularBufferSize,
					traceBufferSize);
			initialisedTraceBuffers = 1;
			IBMRAS_DEBUG(debug,
					"Have initialised TBs and set flag to 1 (won't create TBs again).");
		} else {
			IBMRAS_DEBUG(debug,
					"Have NOT initialised TBs, flag was already set to 1.");
			return true;
		}

		/* We need at least one buffer to use the subscriber */
		if (traceBufferChain == NULL) {
			IBMRAS_DEBUG(debug,
					"startTraceSubscriber unable to allocate buffer memory.");
			return false;
		} else {
			jvmtiError rc;

			traceData.traceBufferSize = traceBufferSize;

			/* Make the buffers available to the subscriber */
			IBMRAS_DEBUG(debug,  "startTraceSubscriber before >RawMonitorEnter ");
			rc = vmData.pti->RawMonitorEnter(traceData.monitor);

			if (JVMTI_ERROR_NONE == rc) {
				queuePut(&(traceData.freeBufferQueue), traceBufferChain);

				rc = vmData.pti->RawMonitorExit(traceData.monitor);
				if (rc != JVMTI_ERROR_NONE) {
					IBMRAS_DEBUG(debug,
							"startTraceSubscriber unable to exit raw monitor 1.");
					return false;
				}
			} else {
				IBMRAS_DEBUG(debug,
						"startTraceSubscriber unable to enter raw monitor 2.");
				return false;
			}
		}
		int rc;
		rc = vmData.jvmtiRegisterTraceSubscriber(vmData.pti, "Health Center trace subscriber",
				traceSubscriber, NULL, &traceData, &subscriptionID);
		IBMRAS_DEBUG_1(debug,  "return code from jvmtiRegisterTraceSubscriber %d", rc);
		if (JVMTI_ERROR_NONE == rc) {
			IBMRAS_DEBUG(debug,
					"startTraceSubscriber registered to jvmtiRegisterTraceSubscriber");
			return true;
		} else {
			IBMRAS_DEBUG(debug,
					"startTraceSubscriber unable to register to jvmtiRegisterTraceSubscriber");
			return false;
		}
	} else {
		IBMRAS_DEBUG(debug,
				"startTraceSubscriber failed to start");
		return false;
	}
}

jvmtiError traceSubscriber(jvmtiEnv *pti, void *record, jlong length,void *userData) {
	TRACEDATA *data = (TRACEDATA *) userData;
	TRACEBUFFER *buffer = NULL;

	jvmtiError rc;
	rc = pti->RawMonitorEnter(data->monitor);
	if (JVMTI_ERROR_NONE == rc) {
		/* Get a free buffer to copy the record into */
		buffer = queueGet(&(data->freeBufferQueue), 1);
		if (buffer == NULL) {
			/* Drop the oldest buffer */
			buffer = queueGet(&(data->traceBufferQueue), 1);
			data->droppedBufferCount++;
			if (countDroppedBuffers != 0) {
				buffersDropped++;
			}
		} else {
			if (countDroppedBuffers != 0) {
				buffersNotDropped++;
			}

		}

		rc = pti->RawMonitorExit(data->monitor);
		if (rc != JVMTI_ERROR_NONE) {
			IBMRAS_DEBUG(debug,
					"traceSubscriber unable to exit raw monitor to obtain free buffer.");
		}
	} else {
		IBMRAS_DEBUG(debug,
				"traceSubscriber unable to enter raw monitor to obtain free buffer.");
	}

	/* Copy and queue the trace buffer */
	if (buffer != NULL) {
		jlong payLoadLength = length;
		assert(buffer->size == (length + 4 + sizeof(jlong)));

		/* Write eye catcher */
		strcpy((char*) buffer->buffer, "HCTB");
		/* Convert payload length to network byte order */
		payLoadLength = htonjl(payLoadLength);

		/* Write length of trace buffer */
		memcpy(buffer->buffer + 4, (char*) &payLoadLength, sizeof(jlong));

		/* Copy the trace buffer */
		memcpy(buffer->buffer + 4 + sizeof(jlong), record, length);

		/* Queue the copied trace buffer */
		rc = pti->RawMonitorEnter(data->monitor);
		if (JVMTI_ERROR_NONE == rc) {
			queuePut(&(data->traceBufferQueue), buffer);

			rc = pti->RawMonitorExit(data->monitor);
			if (rc != JVMTI_ERROR_NONE) {
				IBMRAS_DEBUG(debug,
						"traceSubscriber unable to exit raw monitor to queue copied buffer.");
			}
		} else {
			IBMRAS_DEBUG(debug,
					"traceSubscriber unable to enter raw monitor to queue copied buffer.");
		}
	} else {
		IBMRAS_DEBUG(debug,
				"traceSubscriber unable to obtain memory to copy trace buffer.");
	}
	return JVMTI_ERROR_NONE;
}
void queuePut(TRACEBUFFERQUEUE *queue, TRACEBUFFER *buffer) {
	assert(queue != NULL);
	if (buffer == NULL) {
		return;
	}
	/* Append the buffer to the tail of the queue */
	if (queue->tail == NULL) {
		queue->head = buffer;
	} else {
		queue->tail->next = buffer;
	}
	queue->tail = buffer;
	while (queue->tail->next != NULL) {
		queue->tail = queue->tail->next;
	}
}

/******************************/
TRACEBUFFER *
queueGet(TRACEBUFFERQUEUE *queue, int maxNumberOfItems) {
	TRACEBUFFER *firstItemToReturn = NULL;
	TRACEBUFFER *lastItemToReturn = NULL;
	int items = 0;

	assert(queue != NULL);

	if (maxNumberOfItems <= 0 || queue->head == NULL) {
		return NULL;
	}
	firstItemToReturn = queue->head;
	lastItemToReturn = firstItemToReturn;
	items = 1;
	while ((items < maxNumberOfItems) && (lastItemToReturn->next != NULL)) {
		lastItemToReturn = lastItemToReturn->next;
		items++;
	}
	queue->head = lastItemToReturn->next;
	if (queue->tail == lastItemToReturn) {
		queue->tail = NULL;
	}
	lastItemToReturn->next = NULL;
	return firstItemToReturn;
}

/******************************/
void freeTraceBuffer(jvmtiEnv *jvmtienv, TRACEBUFFER *buffer) {
	IBMRAS_DEBUG(debug,  "> freeTraceBuffer");
	if (buffer != NULL) {
		jvmtiError rc;
		rc = jvmtienv->Deallocate(buffer->buffer);
		if (rc != JVMTI_ERROR_NONE) {
			IBMRAS_DEBUG(debug,
					"freeTraceBuffer unable to deallocate memory for trace buffer.");
		} else {
			buffer->buffer = NULL;
			buffer->size = 0;
		}
		rc = jvmtienv->Deallocate((unsigned char *) buffer);
		if (rc != JVMTI_ERROR_NONE) {
			IBMRAS_DEBUG(debug,  "freeTraceBuffer error deallocating memory.");
		}
	}
	IBMRAS_DEBUG(debug,  "< freeTraceBuffer");
}

void* processLoop(ibmras::common::port::ThreadData* param) {
	running = true;
	int maxSendBytes = 100000;
	IBMRAS_DEBUG(info,  "Starting tracesubscriber process loop");
	JNIEnv * env;

	vmData.theVM->AttachCurrentThread((void **) &env, NULL);
	IBMRAS_DEBUG(info,  "tracesubscriber process loop 1");
	while (running) {
		// acquire lock, if !running break
		IBMRAS_DEBUG(info,  "tracesubscriber process loop 2");
		ibmras::common::port::sleep(2);
		if (!running) {
			break;
		}
		IBMRAS_DEBUG(info,  "tracesubscriber process loop 3");
		sendTraceBuffers(maxSendBytes);
		// release lock so that stop() can set it needed
	}
	IBMRAS_DEBUG(info,  "tracesubscriber process loop 4");
	vmData.theVM->DetachCurrentThread();
	IBMRAS_DEBUG(info,  "Exiting tracesubscriber process loop");
	return NULL;
}

void initializeTraceUserData() {
	IBMRAS_DEBUG(debug,  "initializeTraceUserData enter");

	if(vmData.pti == NULL){
		IBMRAS_DEBUG(debug,  "vmData.pti == NULL");
	}

	if (JVMTI_ERROR_NONE == vmData.pti->CreateRawMonitor(
					"Health Center trace buffer queue monitor",
					&(traceData.monitor))) {
		IBMRAS_DEBUG(debug,  "initializeTraceUserData 1");
		traceData.droppedBufferCount = 0;
		traceData.badMaxSizeWarningShown = JNI_FALSE;
		traceData.traceBufferSize = 0;
		traceData.traceBufferQueue.head = NULL;
		traceData.traceBufferQueue.tail = NULL;
		traceData.freeBufferQueue.head = NULL;
		traceData.freeBufferQueue.tail = NULL;
	} else {
		IBMRAS_DEBUG(debug,  "initializeTraceUserData unable to create raw monitor.");
	}
	IBMRAS_DEBUG(debug,  "initializeTraceUserData exit");

}


monitordata* generateTraceHeader() {
	return generateData(0, traceMetadata, headerSize);
}

/******************************/
void publishConfig() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();

	ibmras::monitoring::connector::ConnectorManager *conMan =
					agent->getConnectionManager();

	std::stringstream str;

	for (std::map<std::string, std::string>::iterator propsiter = config.begin();
			propsiter != config.end(); ++propsiter) {
		str << propsiter->first << "=" << propsiter->second << std::endl;
	}

	std::string msg = str.str();
	conMan->sendMessage("TRACESubscriberSourceConfiguration", msg.length(), (void*) msg.c_str());
}

void sendTraceHeader(bool persistent) {
	monitordata* mdata = generateTraceHeader();
	mdata->persistent = persistent;
	sendDataToAgent(mdata);
	delete mdata;
}


int sendTraceBuffers(int maxSize) {
	IBMRAS_DEBUG(debug,  "> sendTraceBuffers");
	jvmtiError rc;
	int droppedBufferCount = 0;
	TRACEBUFFER *buffersToSend = NULL;
	TRACEBUFFER *traceBuffer = NULL;
	const char* droppedMsgEyeCatcher = "HCDB";

	long droppedBuffersMsgSize = 0;
	int bytesToSendSize = 0;

	/* if we're counting dropped buffers and we haven't already sent back any trace,
	 * record the number of buffers dropped.
	 */
	if (countDroppedBuffers != 0 && firstConnectionMade == 0) {
		buffersDroppedBeforeFirstConnection = buffersDropped;
		firstConnectionMade = 1;
	}

	rc = vmData.pti->RawMonitorEnter(traceData.monitor);
	if (JVMTI_ERROR_NONE == rc) {
		int numberOfBuffersToSend = (maxSize / traceData.traceBufferSize) + 1;
		droppedBufferCount = traceData.droppedBufferCount;
		buffersToSend = queueGet(&(traceData.traceBufferQueue),
				numberOfBuffersToSend);
		traceData.droppedBufferCount = 0;

		rc = vmData.pti->RawMonitorExit(traceData.monitor);
		if (rc != JVMTI_ERROR_NONE) {
			IBMRAS_DEBUG_1(debug,
					"sendTraceBuffers unable to exit raw monitor to get buffers rc code is %d.",
					rc);
		}
	} else {
		IBMRAS_DEBUG_1(debug,
				"sendTraceBuffers unable to enter raw monitor to get buffers rc code is %d.",
				rc);
	}

	/* Total up the number of bytes to send back */
	if (droppedBufferCount > 0) {
		droppedBuffersMsgSize = strlen(droppedMsgEyeCatcher) + sizeof(long)
				+ sizeof(int);
		bytesToSendSize += droppedBuffersMsgSize;
	}
	traceBuffer = buffersToSend;
	while (traceBuffer != NULL) {
		bytesToSendSize += traceBuffer->size;
		traceBuffer = traceBuffer->next;
	}

	int offset = 0;

	/* Add a dropped buffer message if required */
	if (droppedBufferCount > 0) {
		const char* droppedMsgEyeCatcher = "HCDB";
		char buffer[16];
		jlong length = sizeof(int);
		jint count = htonl(droppedBufferCount);

		/* Write eye catcher */
		strcpy(buffer, droppedMsgEyeCatcher);
		offset += strlen(droppedMsgEyeCatcher);

		/* Convert length to network byte order */
		length = htonjl(length);

		/* Write length of dropped buffer message body */
		memcpy(buffer + offset, (char*) &length, sizeof(jlong));
		offset += sizeof(jlong);

		/* Write out number of dropped buffers */
		memcpy(buffer + offset, (char*) &count, sizeof(int));

		monitordata *mdata = generateData(0, (char*) buffer, 16);
		sendDataToAgent(mdata);

		delete mdata;
	}

	/* Copy across items */
	traceBuffer = buffersToSend;
	while (traceBuffer != NULL) {
		IBMRAS_DEBUG(debug,  "sending tracebuffer");
		sendDataToAgent(
				generateData(0, (char*) traceBuffer->buffer,
						traceBuffer->size));
		traceBuffer = traceBuffer->next;
	}

	/* Return buffers to the free buffer queue for reuse */
	rc = vmData.pti->RawMonitorEnter(traceData.monitor);
	if (JVMTI_ERROR_NONE == rc) {
		queuePut(&(traceData.freeBufferQueue), buffersToSend);

		rc = vmData.pti->RawMonitorExit(traceData.monitor);
		if (rc != JVMTI_ERROR_NONE) {
			IBMRAS_DEBUG_1(debug,
					"sendTraceBuffers unable to exit raw monitor to queue buffers rc code is %d.",
					rc);
		}
	} else {
		IBMRAS_DEBUG_1(debug,
				"sendTraceBuffers unable to enter raw monitor to queue buffers rc code is %d.",
				rc);
	}
	IBMRAS_DEBUG(debug,  "< sendTraceBuffers");
	return 0;
}

}
}
}
}
} /* end of namespace*/

