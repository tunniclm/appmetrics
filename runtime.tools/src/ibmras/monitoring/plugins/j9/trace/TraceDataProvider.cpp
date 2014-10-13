/*
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools for Java\u2122 - Health Center
 * (C) Copyright IBM Corp. 2008, 2014
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include "ibmras/monitoring/plugins/j9/trace/TraceDataProvider.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/monitoring/plugins/j9/Util.h"

#if defined(WINDOWS)
//#include <winsock2.h>
#define JLONG_FMT_STR "%I64d"
#else /* Unix platforms */
#define _OE_SOCKETS
#define JLONG_FMT_STR "%lld"
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

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
#include "ibmras/common/util/strUtils.h"

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace trace {

uint32 provID;
PUSH_CALLBACK sendDataToAgent;
IBMRAS_DEFINE_LOGGER("TraceDataProvider")
;
jvmFunctions vmData;
char *traceMetadata = NULL;
int headerSize = 0;
int DEFAULT_MAX_CIRCULAR_BUFFER_SIZE = 4000000;
bool running = false;

std::map<std::string, std::string> config;

FILE *vgcFile = NULL;
void *vgcsubscriptionID = NULL;

static const char* SUBSYSTEM = "_subsystem";
static const std::string CAPABILITY_PREFIX = "capability."; //$NON-NLS-1$
static const char* VERBOSE_GC_AVAILABLE = "verbose.gc.available"; //$NON-NLS-1$
static const char* ALLOCATION_SAMPLING_AVAILABLE =
		"allocation.sampling.available"; //$NON-NLS-1$
static const char* ALLOCATION_THRESHOLD_AVAILABLE =
		"allocation.threshold.available"; //$NON-NLS-1$


static const char* LOW_ALLOCATION_THRESHOLD = "lowallocationthreshold"; //$NON-NLS-1$
static const char* HIGH_ALLOCATION_THRESHOLD = "highallocationthreshold"; //$NON-NLS-1$
static const char* STACKTRACEDEPTH = "stacktracedepth"; //$NON-NLS-1$
static const char* TRIGGER_STACK_TRACE_ON = "stacktrace.on"; //$NON-NLS-1$
static const char* TRIGGER_STACK_TRACE_OFF = "stacktrace.off"; //$NON-NLS-1$
static const char* STACK_TRACE_TRIGGER_SUFFIX = "_stacktrace.trigger"; //$NON-NLS-1$
static const char* SET_ALLOCATION_THRESHOLD_TRACEPOINT = "j9mm.231"; //$NON-NLS-1$
static const char* ALLOCATION_THRESHOLD_TRACEPOINT = "j9mm.234"; //$NON-NLS-1$
#if defined(_64BIT)
static const char* MAX_THRESHOLD_VALUE = "18446744073709551615"; //$NON-NLS-1$
#else
static const char* MAX_THRESHOLD_VALUE = "4294967295"; //$NON-NLS-1$
#endif

static const char* ALLOCATION_EVENT_DUMP_SILENT = "silent:"; //$NON-NLS-1$
static const char* ALLOCATION_EVENT_DUMP_OPTIONS = "events=allocation,filter=#"; //$NON-NLS-1$
static const char* ALLOCATION_FILTER_SIZE_SEPARATOR = ".."; //$NON-NLS-1$

bool allocationThresholdInitialized = false;

std::string stackTraceDepth;
bool stackTraceDepthSet = false;

static const char* VERBOSE_GC = "verbose.gc"; //$NON-NLS-1$

static const char* profiling[] = { "j9jit.15", "j9jit.16", "j9jit.17",
		"j9jit.18", "" };
static const char* gc[] = { "j9mm.1", "j9mm.2", "j9mm.50", "j9mm.51", "j9mm.52",
		"j9mm.53", "j9mm.54", "j9mm.55", "j9mm.56", "j9mm.57", "j9mm.58",
		"j9mm.59", "j9mm.60", "j9mm.64", "j9mm.65", "j9mm.68", "j9mm.69",
		"j9mm.71", "j9mm.72", "j9mm.73", "j9mm.74", "j9mm.75", "j9mm.85",
		"j9mm.86", "j9mm.90", "j9mm.91", "j9mm.94", "j9mm.131", "j9mm.132",
		"j9mm.133", "j9mm.134", "j9mm.135", "j9mm.136", "j9mm.137", "j9mm.138",
		"j9mm.139", "j9mm.231", "j9mm.234", "j9mm.279", "j9mm.280", "j9mm.281",
		"j9mm.282", "j9jit.2", "j9jit.4", "j9jit.5", "j9jit.6", "j9jit.9",
		"j9jit.11", "j9jit.12", "j9jit.18", "j9mm.383", "j9mm.384", "j9mm.345",
		"j9mm.346", "j9mm.347", "j9mm.348", "j9mm.463", "j9mm.464", "j9mm.467",
		"j9mm.468", "j9mm.469", "j9mm.470", "j9mm.560", "j9mm.474", "j9mm.475",
		"j9mm.395", "j9mm.285", "j9mm.286", "" };
static const char* classes[] = { "j9bcu.1", "j9shr.51", "j9bcverify.14",
		"j9bcverify.18", "" };
static const char* jit[] = { "j9jit.1", "j9jit.20", "j9jit.21", "j9jit.22",
		"j9jit.23", "j9jit.24", "j9jit.28", "j9jit.29", "" };
static const char* io[] =
		{ "IO.100", "IO.101", "IO.102", "IO.103", "IO.104", "IO.105", "IO.106",
				"IO.107", "IO.108", "JAVA.315", "JAVA.316", "JAVA.317",
				"JAVA.318", "JAVA.319", "JAVA.320", "JAVA.321", "JAVA.322",
				"JAVA.323", "j9scar.35", "j9scar.36", "j9scar.37", "j9scar.38",
				"j9scar.136", "j9scar.137", "j9scar.138", "j9scar.139",
				"j9scar.140", "" };
static const char* DUMP_POINTS[] = { "j9dmp.4", "j9dmp.7", "j9dmp.9",
		"j9dmp.10", "" };

std::string getConfigString() {
	std::stringstream str;
	for (std::map<std::string, std::string>::iterator propsiter =
			config.begin(); propsiter != config.end(); ++propsiter) {
		str << propsiter->first << "=" << propsiter->second << std::endl;
	}
	return str.str();
}

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

	ibmras::monitoring::plugins::j9::trace::provID = provID;
	ibmras::monitoring::plugins::j9::trace::sendDataToAgent = callback;
	return src;
}

bool gcTracepointAvailableInThisVM(int tpNumber) {
	/*
	 * gc tracepoints j9mm.131 - j9mm.139 aren't available in Java 5 before SR9
	 */
	if (tpNumber <= 139 && tpNumber >= 131) {
		if (Util::getJavaLevel() == 5) {
			if (Util::getServiceRefreshNumber() < 9) {
				return false;
			}
		}
		// trace point j9mm.383 and j9mm.384 only available in the 2.6VM onwards
	} else if (tpNumber == 383 || tpNumber == 384 || tpNumber == 395
			|| (tpNumber >= 345 && tpNumber <= 348)
			|| (tpNumber >= 463 && tpNumber <= 475)) {
		if (!Util::is26VMOrLater()) {
			return false;
		}
	}
	if (tpNumber == 560) {
		if (((Util::getJavaLevel() < 7) && (!Util::is26VMOrLater()))
				|| ((Util::getJavaLevel() == 7)
						&& (Util::getServiceRefreshNumber() < 4)
						&& (!Util::is27VMOrLater()))) {
			return false;
		}
	}
	return true;
}

bool j9ShrTracePointAvailableInThisVM() {
	/*
	 * j9shr 1326 tracepoint is not available in Java 6 before SR8, and the trace API is broken
	 * therefore we don't get a return code to find out whether we managed to enable it or not.
	 * Let's parse the java.runtime.version
	 */
	if (Util::getJavaLevel() == 5) {
		return false;
	}

	if (Util::getJavaLevel() == 6) {
		/*
		 * If it's between -1 and 8 (-1 would be given for GA and if we couldn't work it out)
		 */
		if (Util::getServiceRefreshNumber() < 8) {
			return false;
		}
	}
	return true;
}

bool profilingTracepointAvailableInThisVM() {
	/*
	 * profiling tracepoints aren't available in Java 5 before SR8, and the trace API is broken
	 * therefore we don't get a return code to find out whether we managed to enable it or not.
	 * Let's parse the java.runtime.version - if it contains "dev" we're on a Java 5 build, the
	 * SR number will be in the string in the format (SR#).
	 */
	if (Util::getJavaLevel() == 5) {
		/*
		 * If it's between -1 and 8 (-1 would be given for GA and if we couldn't work it out)
		 */
		if (Util::getServiceRefreshNumber() < 8) {
			return false;
		}
	}
	// TODO should it check for SR6? Or maybe for Java 6 return codes work?
	return true;
}

bool isDumpStartedTPAvailabledInVM() {
	/*
	 * j9dmp.7 tracepoint isn't available in Java 5 before SR10 or Java 6 before SR 5
	 */
	if (Util::getJavaLevel() == 5) {
		if (Util::getServiceRefreshNumber() < 10) {
			return false;
		}
	}
	if (Util::getJavaLevel() == 6) {
		if (Util::getServiceRefreshNumber() < 5) {
			return false;
		}
	}
	return true;
}

bool isDumpTPavailable(const std::string &tpNumber) {
	if (tpNumber == "4") {
		return true;
	}
	if (tpNumber == "7") {
		return isDumpStartedTPAvailabledInVM();
	}
	// J9dmp 9 and 10 are only available in 26 vm
	if (tpNumber == "9" || tpNumber == "10") {
		return (Util::j9DmpTrcAvailable());
	}
	return false;
}

bool JavaTracePointsAvailableInVM() {
	/*
	 * java.315-323 tracepoints aren't available in Java 5 before SR10 or Java 6 before SR 5
	 */
	if (Util::getJavaLevel() == 5) {
		if (Util::getServiceRefreshNumber() < 10) {
			return false;
		}
	}
	return true;
}

bool isOkConsideringRealtime(int tp) {
	bool answer = false;
	if (Util::isRealTimeVM()) {
		if (Util::is26VMOrLater()) {
			answer = (tp == 285 || tp == 286 || tp == 467 || tp == 468
					|| tp == 474 || tp == 475 || (tp >= 54 && tp <= 57));
		} else {
			// these are the old WRT trace points but are something else
			// in 2.6 vm that we don't want to enable
			answer = (tp >= 279 && tp <= 282);
		}
	} else {
		answer = !((tp == 285 || tp == 286 || tp == 467 || tp == 468)
				|| (tp >= 279 && tp <= 282));
	}
	return answer;
}

bool tracePointExistsInThisVM(const std::string &tp) {
	std::vector<std::string> tracePoint = ibmras::common::util::split(tp, '.');
	if (tracePoint.size() != 2) {
		return false;
	}

	std::string component = tracePoint[0];
	std::string number = tracePoint[1];

	// Do some extra checks

	bool isJITTracePoint = ((component == "j9jit")
			&& (number == "1" || number == "20" || number == "21"
					|| number == "22" || number == "23" || number == "24"
					|| number == "28" || number == "29"));

	bool jitOK = !isJITTracePoint || Util::is27VMOrLater();

	bool isj9ShrTracePoint = (tp == "j9shr.1326"); //$NON-NLS-1$
	bool isj9ShrOK = !isj9ShrTracePoint || j9ShrTracePointAvailableInThisVM();

	bool isProfilingTracePoint = ((component == "j9jit")
			&& (number == "15" || number == "16" || number == "17"
					|| number == "18"));

	bool isLOATracePoint = (tp == "j9mm.231" || tp == "j9mm.234");

	bool profilingOK = !isProfilingTracePoint
			|| profilingTracepointAvailableInThisVM();

	bool loaOK = !isLOATracePoint || Util::vmHasLOATracePoints();

	bool isDumpTracePointOK = ((component != "j9dmp") || isDumpTPavailable(tp));

	bool isJavaTracePoint = ((component == "java")
			&& (number == "315" || number == "316" || number == "317"
					|| number == "318" || number == "319" || number == "320"
					|| number == "321" || number == "322" || number == "323"));

	bool javaTracePointsOK = !isJavaTracePoint
			|| JavaTracePointsAvailableInVM();

	bool gcOK = true;
	bool realtimeOK = true;

	int tpNumber = atoi(number.c_str());

	if (component == "j9mm") {
		realtimeOK = isOkConsideringRealtime(tpNumber);

		// GC trace points that do not exist in earlier vm's
		gcOK = gcTracepointAvailableInThisVM(tpNumber);
	}

	// Also don't turn on j9vm.333 if the method dictionary is available
	bool methodDictionaryAvailable = false;
	if (tp == "j9vm.333") {
		if (vmData.jvmtiGetMethodAndClassNames) {
			methodDictionaryAvailable = true;
		}
	}

	bool tracePointExists = realtimeOK && profilingOK && loaOK
			&& !methodDictionaryAvailable && gcOK && isDumpTracePointOK
			&& javaTracePointsOK && jitOK && isj9ShrOK;

	return tracePointExists;
}

void setCapabilities() {
	std::string capability;
	if (vmData.verboseGCsubscribe) {
		config[CAPABILITY_PREFIX + VERBOSE_GC_AVAILABLE] = "on";
	}

	if (gcTracepointAvailableInThisVM(395)) {
		config[CAPABILITY_PREFIX + ALLOCATION_SAMPLING_AVAILABLE] = "on";
	}
	if (tracePointExistsInThisVM(ALLOCATION_THRESHOLD_TRACEPOINT)) {
		config[CAPABILITY_PREFIX + ALLOCATION_THRESHOLD_AVAILABLE] = "on";
	}
}

void setNoDynamicProperties() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	if (agent->agentPropertyExists("leave.dynamic.trace")) {
		vmData.setTraceOption(vmData.pti, "buffers=nodynamic");
	}
}

/**
 * The start() method starts the plugin and is the method called from the setup ibmras::monitoring::Plugin* getPlugin()
 * function above
 */
int Tracestart() {

	IBMRAS_DEBUG(debug, "Tracestart enter");

	setNoDynamicProperties();

	setCapabilities();

	int bufferSize = 0;
	int rc = 0;
	char* tempMeta;
	int tempHeaderSize = 0;
	long maxCircularBufferSize = DEFAULT_MAX_CIRCULAR_BUFFER_SIZE;
	/* this is the eye catcher that tells the health center client trace parser
	 * that this is a header record */
	char METADATA_EYE_CATCHER[] = { 'H', 'C', 'T', 'H' };

	/* get the trace header data from the vm */
	if (vmData.jvmtiGetTraceMetadata != 0) {
		rc = vmData.jvmtiGetTraceMetadata(vmData.pti, &tempMeta,
				&tempHeaderSize);
	}

	if (tempMeta == NULL || rc != JVMTI_ERROR_NONE) {
		IBMRAS_DEBUG(warning, "failed to get trace header");
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


#if defined(_ZOS)
#pragma convert("ISO8859-1")
#endif
	/* turn off all trace to start with */
	vmData.setTraceOption(vmData.pti, "none=all,maximal=mt");
#if defined(_ZOS)
#pragma convert(pop)
#endif
	/* now enable HC specific trace */
	enableTracePoints(gc);
	enableTracePoints(profiling);
	enableTracePoints(classes);
	enableTracePoints(jit);
	enableTracePoints(io);
	config["gc_subsystem"] = "on";
	config["profiling_subsystem"] = "on";
	config["classes_subsystem"] = "on";
	config["jit_subsystem"] = "on";
	config["io_subsystem"] = "on";
	enableTracePoints(DUMP_POINTS);

	// Publish the initial configuration
	publishConfig();
	IBMRAS_DEBUG(debug, "Tracestart exit");
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
	type = ibmras::monitoring::plugin::data
			| ibmras::monitoring::plugin::receiver;
	recvfactory = (RECEIVER_FACTORY) TraceDataProvider::getReceiver;
	confactory = NULL;
}

void enableTracePoints(const char* tracePoints[]) {

	IBMRAS_DEBUG(debug, "start of turning on tracepoints");

	for (int i = 0; strlen(tracePoints[i]) > 0; i++) {
		enableTracePoint(tracePoints[i]);
	}

	IBMRAS_DEBUG(debug, "end of turning on tracepoints");
}

void disableTracePoints(const char* tracePoints[]) {

	IBMRAS_DEBUG(debug, "start of turning off tracepoints");

	for (int i = 0; strlen(tracePoints[i]) > 0; i++) {
		disableTracePoint(tracePoints[i]);
	} IBMRAS_DEBUG(debug, "end of turning off tracepoints");
}

void controlSubsystem(const std::string &command, const char* tracePoints[]) {
	if (ibmras::common::util::equalsIgnoreCase(command, "off")) {
		disableTracePoints(tracePoints);
	} else if (ibmras::common::util::equalsIgnoreCase(command, "on")) {
		enableTracePoints(tracePoints);
	}
}

void controlSubsystem(const std::string &command,
		const std::string& subsystem) {

	IBMRAS_DEBUG_2(debug, "processing subsystem command: %s %s", command.c_str(), subsystem.c_str());
	if (subsystem == "gc") {
		controlSubsystem(command, gc);
	} else if (subsystem == "profiling") {
		controlSubsystem(command, profiling);
	} else if (subsystem == "classes") {
		controlSubsystem(command, classes);
	} else if (subsystem == "jit") {
		controlSubsystem(command, jit);
	} else if (subsystem == "io") {
		controlSubsystem(command, io);
	} else {
		return;
	}
	//update the config info
	config[subsystem + SUBSYSTEM] = command;
}

std::string getAllocationThresholds() {
	std::string threshold;
	std::string dumpOptions = Util::queryVmDump(vmData.jvmtiQueryVmDump,
			vmData.pti);

	if (dumpOptions.length() > 0) {
		size_t index = dumpOptions.find(ALLOCATION_EVENT_DUMP_OPTIONS);
		if (index != std::string::npos) {
			std::string optionString = dumpOptions.substr(
					index + strlen(ALLOCATION_EVENT_DUMP_OPTIONS));
			threshold = optionString.substr(0, optionString.find(','));
		}
	}
	return threshold;
}

int setAllocationThresholds(const std::string &thresholds, bool force) {
	std::string currentThresholds = getAllocationThresholds();
	if (!force && currentThresholds.length() > 0
			&& currentThresholds == thresholds) {
		return 0;
	}

	std::string command = ALLOCATION_EVENT_DUMP_SILENT;
	command += ALLOCATION_EVENT_DUMP_OPTIONS;
	command += thresholds;

	vmData.jvmtiResetVmDump(vmData.pti);
	int rc = vmData.jvmtiSetVmDump(vmData.pti, command.c_str());

	if (rc != 0) {
		if (currentThresholds.length() > 0) {
			command = ALLOCATION_EVENT_DUMP_SILENT;
			command += ALLOCATION_EVENT_DUMP_OPTIONS;
			command += currentThresholds;

			vmData.jvmtiResetVmDump(vmData.pti);
			rc = vmData.jvmtiSetVmDump(vmData.pti, command.c_str());
		}
	}

	return rc;
}

void setAllocationThresholds(const std::string &low, const std::string& high) {

	std::string lowThreshold;
	std::string highThreshold;
	if (low.length() == 0) {
		lowThreshold = MAX_THRESHOLD_VALUE;
		highThreshold = MAX_THRESHOLD_VALUE;
	} else {
		lowThreshold = low;
		highThreshold = high;
	}

	std::string threshold = lowThreshold;
	if (highThreshold.length() > 0) {
		threshold += ALLOCATION_FILTER_SIZE_SEPARATOR;
		threshold += highThreshold;
	}
	setAllocationThresholds(threshold, false);
}

void resetAllocationThresholdsToCurrent() {
	std::string currentThresholds = getAllocationThresholds();
	if (currentThresholds.length() > 0) {
		setAllocationThresholds(currentThresholds, true);
	}
}

void enableAllocationThreshold() {
	if (allocationThresholdInitialized) {
		return;
	}

	std::string lowThreshold;
	std::string highThreshold;

	lowThreshold = Util::getLowAllocationThreshold();
	highThreshold = Util::getHighAllocationThreshold();

	if (lowThreshold.length() > 0) {
		setAllocationThresholds(lowThreshold, highThreshold);
	} else {
		// Force a tracepoint for setting the allocation threshold
		resetAllocationThresholdsToCurrent();
	}

	allocationThresholdInitialized = true;
}

void setStackDepth(const std::string &depth) {
	std::string traceCommand = "stackdepth=" + depth;
	vmData.setTraceOption(vmData.pti, traceCommand.c_str());
	stackTraceDepthSet = true;
}

void handleStackTraceTrigger(const std::string &command,
		const std::string& tracePoint) {
	if (!tracePointExistsInThisVM(tracePoint)) {
		return;
	}
	std::string traceCommand = "trigger=";
	if (ibmras::common::util::equalsIgnoreCase(command, "off")) {
		traceCommand += '!';
	}
	traceCommand += "tpnid{";
	traceCommand += tracePoint;
	traceCommand += ",jstacktrace}";
	if (!stackTraceDepthSet) {
		// set the default depth
		ibmras::monitoring::agent::Agent* agent =
				ibmras::monitoring::agent::Agent::getInstance();
		stackTraceDepth = agent->getAgentProperty("stack.trace.depth");
		if (stackTraceDepth.length() > 0) {
			setStackDepth(stackTraceDepth);
		}
	}
	vmData.setTraceOption(vmData.pti, traceCommand.c_str());
	config[tracePoint + STACK_TRACE_TRIGGER_SUFFIX] = command;
}

void handleSetCommand(const std::vector<std::string> &parameters) {
	IBMRAS_DEBUG(debug, "handleSetCommand");

	std::string lowAllocationThreshold;
	std::string highAllocationThreshold;

	for (std::vector<std::string>::const_iterator it = parameters.begin();
			it != parameters.end(); ++it) {
		IBMRAS_DEBUG_1(debug, "processing: set %s", (*it).c_str());
		const std::vector<std::string> items = ibmras::common::util::split(*it,
				'=');
		if (items.size() != 2) {
			continue;
		}

		IBMRAS_DEBUG_2(debug, "processing: set %s=%s", items[0].c_str(), items[1].c_str());


		if (ibmras::common::util::equalsIgnoreCase(items[0], STACKTRACEDEPTH)) {
			setStackDepth(items[1]);

		} else if (ibmras::common::util::equalsIgnoreCase(items[0],
				LOW_ALLOCATION_THRESHOLD)) {
			lowAllocationThreshold = items[1];

		} else if (ibmras::common::util::equalsIgnoreCase(items[0],
				HIGH_ALLOCATION_THRESHOLD)) {
			highAllocationThreshold = items[1];

		} else if (ibmras::common::util::equalsIgnoreCase(items[0],
				TRIGGER_STACK_TRACE_ON)) {
			handleStackTraceTrigger("on", items[1]);

		} else if (ibmras::common::util::equalsIgnoreCase(items[0],
				TRIGGER_STACK_TRACE_OFF)) {
			handleStackTraceTrigger("off", items[1]);

		} else if (ibmras::common::util::equalsIgnoreCase(items[0],
				VERBOSE_GC)) {
			if (ibmras::common::util::equalsIgnoreCase(items[1], "on")) {
				handleVerboseGCSetting("on");
			} else if (ibmras::common::util::equalsIgnoreCase(items[1],
					"off")) {
				handleVerboseGCSetting("off");
			}

		}
	}

	if (lowAllocationThreshold.length() > 0) {
		setAllocationThresholds(lowAllocationThreshold,
				highAllocationThreshold);
	}
}

void handleCommand(const std::string &command,
		const std::vector<std::string> &parameters) {

	JNIEnv * env;
	vmData.theVM->AttachCurrentThread((void **) &env, NULL);

	IBMRAS_DEBUG_1(fine, "command received: %s", command.c_str());

	if (ibmras::common::util::equalsIgnoreCase(command, "set")) {
		handleSetCommand(parameters);
	} else {
		for (std::vector<std::string>::const_iterator it = parameters.begin();
				it != parameters.end(); ++it) {
			const std::string parameter = (*it);
			IBMRAS_DEBUG_2(debug, "processing command: %s %s", command.c_str(), parameter.c_str());

			if (ibmras::common::util::endsWith(parameter, SUBSYSTEM)) {
				controlSubsystem(command,
						parameter.substr(0,
								parameter.length() - strlen(SUBSYSTEM)));

			} else if (ibmras::common::util::endsWith(parameter,
					STACK_TRACE_TRIGGER_SUFFIX)) {
				handleStackTraceTrigger(command,
						parameter.substr(0,
								parameter.length()
										- strlen(STACK_TRACE_TRIGGER_SUFFIX)));
			}
		}
	}
	publishConfig();
	vmData.theVM->DetachCurrentThread();
}

void enableTracePoint(const std::string &tp) {
	if (tracePointExistsInThisVM(tp)) {
		if (ibmras::common::util::startsWith(tp, "j9mm.")) {
			enableGCTracePoint(tp);
			if (tp == ALLOCATION_THRESHOLD_TRACEPOINT) {
				handleStackTraceTrigger("on", tp);
			} else if (tp == SET_ALLOCATION_THRESHOLD_TRACEPOINT) {
				enableAllocationThreshold();
			}
		} else {
			enableNormalTracePoint(tp);
		}

		config[tp] = "on";
	}
}

void disableTracePoint(const std::string &tp) {
	if (tracePointExistsInThisVM(tp)) {
		if (tp.find("j9mm.") != std::string::npos) {
			disableExceptionTracePoint(tp);
		} else {
			disableNormalTracePoint(tp);
		}
		config[tp] = "off";
	}
}

void enableGCTracePoint(const std::string &tp) {
	disableNormalTracePoint(tp);
	enableExceptionTracePoint(tp);
}

void enableNormalTracePoint(const std::string &tp) {
#if defined(_ZOS)
#pragma convert("ISO8859-1")
#endif
	std::string command = "maximal=tpnid{" + tp + "}";
	vmData.setTraceOption(vmData.pti, command.c_str());
}

void disableExceptionTracePoint(const std::string &tp) {
	int rc = 0;
	std::string command = "exception=!tpnid{" + tp + "}";
	rc = vmData.setTraceOption(vmData.pti, command.c_str());
}

void enableExceptionTracePoint(const std::string &tp) {
	IBMRAS_DEBUG(debug, "in enableExceptionTracePoint");

	std::string command = "exception=tpnid{" + tp + "}";
	vmData.setTraceOption(vmData.pti, command.c_str());
}

void disableNormalTracePoint(const std::string &tp) {
	int rc = 0;
	std::string command = "maximal=!tpnid{" + tp + "}";
	rc = vmData.setTraceOption(vmData.pti, command.c_str());
}
#if defined(_ZOS)
#pragma convert(pop)
#endif

/*
 * Converts a jlong from host to network byte order (big endian)
 */
jlong htonjl(jlong l) {
	jint byteOrderTest = 0x01020304;
	if (byteOrderTest == htonl(byteOrderTest)) {
		/* big endian */
		return l;
	} else {
		jint hi = (jint) (l >> 32);
		jint lo = (jint) (l & 0xffffffff);
		jlong convhi = htonl(hi);
		jlong convlo = htonl(lo);
		/* little endian */
		return (jlong) ((convlo << 32) | (convhi & 0xffffffff));
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

bool startTraceSubscriber(long maxCircularBufferSize, int traceBufferSize) {
	IBMRAS_DEBUG(debug, "> startTraceSubscriber");
	if (vmData.jvmtiGetTraceMetadata != 0
			&& vmData.jvmtiRegisterTraceSubscriber != 0) {
		void *subscriptionID;

		int rc;
#if defined(_ZOS)
#pragma convert("ISO8859-1")
#endif
		rc = vmData.jvmtiRegisterTraceSubscriber(vmData.pti,
				"Health Center trace subscriber", traceSubscriber, NULL,
				NULL, &subscriptionID);
#if defined(_ZOS)
#pragma convert(pop)
#endif
		IBMRAS_DEBUG_1(debug, "return code from jvmtiRegisterTraceSubscriber %d", rc);
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


jvmtiError traceSubscriber(jvmtiEnv *pti, void *record, jlong length,
		void *userData) {


	IBMRAS_DEBUG(debug, "entering trace subscriber callback");
	if (record == NULL || length == 0) {
		IBMRAS_DEBUG(debug, "exiting trace subscriber callback: no buffer");
		return JVMTI_ERROR_NONE;
	}

	jlong payLoadLength = length;

	unsigned char* buffer = new unsigned char[length + 4 + sizeof(jlong)];
	/* Write eye catcher */
#if defined(_ZOS)
#pragma convert("ISO8859-1")
#endif
	strcpy((char*) buffer, "HCTB");
#if defined(_ZOS)
#pragma convert(pop)
#endif
	/* Convert payload length to network byte order */
	payLoadLength = htonjl(payLoadLength);

	/* Write length of trace buffer */
	memcpy(buffer + 4, (char*) &payLoadLength, sizeof(jlong));

	/* Copy the trace buffer */
	memcpy(buffer + 4 + sizeof(jlong), record, length);

	monitordata* mdata = generateData(0, (char*) buffer,
			length+4+sizeof(jlong));
	sendDataToAgent(mdata);
	delete[] buffer;
	delete mdata;

	IBMRAS_DEBUG(debug, "exiting trace subscriber callback");

	return JVMTI_ERROR_NONE;
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

	std::string msg = getConfigString();
	IBMRAS_DEBUG_1(fine, "publishing config: %s", msg.c_str());

	conMan->sendMessage("TRACESubscriberSourceConfiguration", msg.length(),
			(void*) msg.c_str());
}

void sendTraceHeader(bool persistent) {
	monitordata* mdata = generateTraceHeader();
	mdata->persistent = persistent;
	sendDataToAgent(mdata);
	delete mdata;
}


jvmtiError verboseGCSubscriber(jvmtiEnv *env, const char *record, jlong length,
		void *userData) {
	IBMRAS_DEBUG(debug, "> verboseGCSubscriber");
	if (vgcFile != NULL) {
		fwrite(record, length, 1, vgcFile);
	} IBMRAS_DEBUG(debug, "< verboseGCSubscriber");
	return JVMTI_ERROR_NONE;
}

jvmtiError verboseGCAlarm(jvmtiEnv *env, void *subscriptionID, void *userData) {
	IBMRAS_DEBUG(debug, "> verboseGCAlarm");
	/* The subscriber callback failed */
	IBMRAS_DEBUG(debug, "< verboseGCAlarm");
	return JVMTI_ERROR_NONE;
}

int registerVerboseGCSubscriber(std::string fileName) {
	IBMRAS_DEBUG(debug, "> registerVerboseGCSubscriber");

	if (!vmData.verboseGCsubscribe) {
		IBMRAS_DEBUG(debug, "< registerVerboseGCSubscriber feature not available on this vm");
		return -1;
	}

	if (vgcFile != NULL || vgcsubscriptionID != NULL) {
		IBMRAS_DEBUG(debug, "< registerVerboseGCSubscriber agent already subscribed to verbosegc");
		return -1;
	}

	if (!fileName.length()) {
		IBMRAS_LOG(warning, "null file name for registerVerboseGCSubscriber");
		return -1;
	}

	vgcFile = fopen(fileName.c_str(), "w");

	if (vgcFile == NULL) {
		IBMRAS_LOG_1(warning, "Error opening a file for writing verbose gc. %s",
				fileName.c_str());
	} else {
		/* Register a new subscriber */
#ifdef _ZOS
#pragma convlit(resume)
#endif
		jvmtiError err = vmData.verboseGCsubscribe(vmData.pti,
				"Health Center verbose GC subscriber", verboseGCSubscriber,
				verboseGCAlarm, NULL, &vgcsubscriptionID);
#ifdef _ZOS
#pragma convlit(suspend)
#endif
		if (err != JVMTI_ERROR_NONE) {
			IBMRAS_LOG_1(warning, "verboseGCsubscribe failed: %i", err);
			fclose(vgcFile);
			IBMRAS_DEBUG(debug, "< registerVerboseGCSubscriber");
			return -1;
		} else {
			IBMRAS_LOG_1(info, "writing verbose gc data to %s",
					fileName.c_str());
		}
	}

	IBMRAS_DEBUG(debug, "< registerVerboseGCSubscriber");
	return 0;
}

int deregisterVerboseGCSubscriber() {
	jvmtiError err = JVMTI_ERROR_NONE;
	IBMRAS_DEBUG(debug, "> deregisterVerboseGCSubscriber");

	if (!vmData.verboseGCunsubscribe) {
		IBMRAS_DEBUG(debug,"< deregisterVerboseGCSubscriber feature not available on this vm");
		return -1;
	}

	/* Deregister the subscriber */
	err = vmData.verboseGCunsubscribe(vmData.pti, vgcsubscriptionID, NULL);
	vgcsubscriptionID = NULL;

	if (vgcFile != NULL) {
		fclose(vgcFile);
		vgcFile = NULL;
	}

	if (err != JVMTI_ERROR_NONE && err != JVMTI_ERROR_NOT_AVAILABLE) {
		IBMRAS_LOG_1(warning, "verboseGCunsubscribe failed: %i", err);
		return -1;
	}

	IBMRAS_DEBUG(debug, "< deregisterVerboseGCSubscriber");
	return 0;
}

std::string getWriteableDirectory() {

	JavaVMAttachArgs threadArgs;
	std::string dir = "";

	memset(&threadArgs, 0, sizeof(threadArgs));
	threadArgs.version = JNI_VERSION_1_6;
	threadArgs.name = (char *) "HLCThread";
	threadArgs.group = NULL;

	JNIEnv* env;

	std::vector<std::string> directories;

	IBMRAS_DEBUG(debug, "Attaching to thread");
	jint result =
			vmData.theVM ?
					vmData.theVM->AttachCurrentThread((void**) &env,
							(void*) &threadArgs) :
					-1;
	if (JNI_OK != result) {
		IBMRAS_DEBUG(warning, "Cannot set environment"); IBMRAS_DEBUG(debug, "<< Trace [NOATTACH]");
		return dir;
	}IBMRAS_DEBUG(info, "Environment set");

	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();

	std::string userDir = agent->getAgentProperty("output.directory");

	jstring dirJava = env->NewStringUTF(userDir.c_str());

	dir = getString(env, "runtime/tools/java/utils/FileUtils",
			"findWriteableDirectory", "(Ljava/lang/String;)Ljava/lang/String;",
			dirJava);

	return dir;
}

std::string getString(JNIEnv* env, const std::string& cname,
		const std::string& mname, const std::string& signature,
		jstring dirJava) {

	IBMRAS_DEBUG(debug, ">>getString");

	IBMRAS_DEBUG(debug, "Retrieving class");
	jclass clazz = env->FindClass(cname.c_str());
	if (!clazz) {
		IBMRAS_DEBUG_1(warning, "Failed to find %s class", cname.c_str());
		return "";
	} IBMRAS_DEBUG_1(debug, "Found %s class", cname.c_str());

	jmethodID method = env->GetStaticMethodID(clazz, mname.c_str(),
			signature.c_str());
	if (!method) {
		IBMRAS_DEBUG_1(warning, "Failed to get %s method ID", mname.c_str());
		return "";
	} IBMRAS_DEBUG_1(debug, "%s method loaded, calling thru JNI", mname.c_str());

	jstring jobj = (jstring) env->CallStaticObjectMethod(clazz, method,
			dirJava);

	IBMRAS_DEBUG_1(debug, "Back from env->GetStringUTFChars method with jobj = %s", jobj);

	IBMRAS_DEBUG_1(debug, "Back from %s method", mname.c_str());

	if (jobj) {
		const char* value = env->GetStringUTFChars(jobj, NULL);
		IBMRAS_DEBUG_1(debug, "Back from env->GetStringUTFChars method wit jobj = %s", jobj)
		std::string sval(value);
		env->ReleaseStringUTFChars(jobj, value);

		return sval;
	}

	IBMRAS_DEBUG(debug, "<<getString");

	return "";

}

void handleVerboseGCSetting(std::string value) {
	IBMRAS_DEBUG_1(debug, ">>> handleVerboseGCSetting(%s)", value.c_str());
	if (value == "on") {
		const std::string outputDirectory = getWriteableDirectory();
		if (!outputDirectory.length()) {
			IBMRAS_DEBUG(debug, "No writeable dir found");
			return;
		}

		std::stringstream vgcFileNamePrefix;
		vgcFileNamePrefix << "verbosegc_";
		vgcFileNamePrefix << ibmras::common::port::getProcessId() << "_";
		std::string vgcFileName = vgcFileNamePrefix.str();
		int suffix = 0;
		bool fileAvailable = false;
		std::fstream* vgcFile;
		while (!fileAvailable) {
			std::stringstream ss;
			ss << outputDirectory;
#if defined(WINDOWS)
			ss << "\\";
#else
			ss << "/";
#endif
			ss << vgcFileNamePrefix.str() << suffix++ << ".log";
			vgcFileName = ss.str();
			vgcFile = new std::fstream;

			vgcFile->open(vgcFileName.c_str(), std::ios::out | std::ios::app);

			if (vgcFile->good()) {
				fileAvailable = true;
				break;
			}
		}

		int err = 0;

		err = registerVerboseGCSubscriber(vgcFileName);
		if (err) {
			config[VERBOSE_GC] = "off";
			IBMRAS_DEBUG(debug, "Error in registerVerboseGCSubscriber(vgcFileName)");
		} else {
			config[VERBOSE_GC] = vgcFileName;
			IBMRAS_DEBUG(debug, "registerVerboseGCSubscriber(vgcFileName) success");
		}

	} else {
		int err = 0;
		err = deregisterVerboseGCSubscriber();
		if (err) {
			IBMRAS_DEBUG(debug, "Error in deregisterVerboseGCSubscriber(vgcFileName)");
		} else {
			IBMRAS_DEBUG(debug, "deregisterVerboseGCSubscriber(vgcFileName) success");
		}
		config[VERBOSE_GC] = "off";
	}
}


}
}
}
}
} /* end of namespace*/

