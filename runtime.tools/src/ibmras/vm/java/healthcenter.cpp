/*
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools for Java\u2122 - Health Center
 * ? Copyright IBM Corp. 2008, 2014
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ibmras/common/logging.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/monitoring/plugins/j9/trace/TraceDataProvider.h"
#include "ibmras/monitoring/plugins/j9/methods/MethodLookupProvider.h"
#include "ibmras/monitoring/connector/jmx/JMXConnectorPlugin.h"
#include "ibmras/monitoring/connector/headless/HLConnectorPlugin.h"
#include "ibmras/monitoring/plugins/jni/JNIReceiver.h"
#include "ibmras/vm/java/healthcenter.h"
#include "ibmras/common/Properties.h"
#include "ibmras/common/util/strUtils.h"

struct __jdata;


#include "jvmti.h"
#include "jni.h"
#include "ibmjvmti.h"
#include "jni.h"
#include "jniport.h"
#include "ibmras/monitoring/plugins/jmx/JMX.h"
#include "ibmras/monitoring/plugins/jni/Facade.h"

/*########################################################################################################################*/
/*########################################################################################################################*/
/*########################################################################################################################*/


static const char* HEALTHCENTER_PROPERTIES_PREFIX = "com.ibm.java.diagnostics.healthcenter.";

void launchAgent(const std::string &options);
std::string agentOptions;
ibmras::common::Properties hcprops;
static JavaVM *theVM;
jvmFunctions tDPP;

jvmtiEnv *pti = NULL;

typedef struct __jdata jdata_t;

/* ensure common reporting of JNI version required */
#define JNI_VERSION JNI_VERSION_1_4

jint agentStart(JavaVM *vm, char *options, void *reserved, int onAttach);

static bool agentStarted = false;

IBMRAS_DEFINE_LOGGER("J9VM");

ibmras::monitoring::agent::Agent* agent;

/* ======================= */
/* Agent control functions */
/* ======================= */
/******************************/
JNIEXPORT void JNICALL
cbVMInit(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread) {

	launchAgent(agentOptions);

}
/******************************/
JNIEXPORT void JNICALL
cbVMDeath(jvmtiEnv *jvmti_env, JNIEnv* jni_env) {
	IBMRAS_DEBUG(debug, "VmDeath event");

	agent->stop();
	agent->shutdown();
}
/******************************/
JNIEXPORT void JNICALL
Agent_OnUnload(JavaVM *vm) {

	IBMRAS_DEBUG(debug, "OnUnload");
}

/******************************/
JNIEXPORT jint JNICALL
Agent_OnAttach(JavaVM *vm, char *options, void *reserved) {

	jint rc = 0;
	IBMRAS_DEBUG(debug, "> Agent_OnAttach");
	if (!agentStarted) {

		agentStarted = true;
		rc = agentStart(vm, options, reserved, 1);
		launchAgent(options);

		return 0;
	} IBMRAS_DEBUG_1(debug, "< Agent_OnAttach. rc=%d", rc);
	return rc;
}

/******************************/
JNIEXPORT jint JNICALL
Agent_OnLoad(JavaVM *vm, char *options, void *reserved) {
	IBMRAS_DEBUG(debug, "OnLoad");
	agentStarted = true;
	jint rc = 0;

	rc = agentStart(vm, options, reserved, 0);
	IBMRAS_DEBUG_1(debug, "< Agent_OnLoad. rc=%d", rc);
	return rc;
}

/****************************/
jint agentStart(JavaVM *vm, char *options, void *reserved, int onAttach) {
	int res;
	jvmtiCapabilities cap;
	jvmtiEventCallbacks cb;

	jint rc, i, j;

	jint xcnt;
	jvmtiExtensionFunctionInfo * exfn;
	jvmtiExtensionEventInfo * exev;

	jvmtiExtensionFunctionInfo * fi;
	jvmtiExtensionEventInfo * ei;
	jvmtiParamInfo * pi;

	static int fInitialized = 0;

	theVM = vm;
	tDPP.theVM = vm;
	agentOptions = options;

	res = vm->GetEnv((void **) &pti, JVMTI_VERSION_1);

	/*----------- Add thread capabilities ----------------------*/

	(void) memset(&cap, 0, sizeof(cap/*jvmtiCapabilities*/));

	/*--------------------------------------
	 Manage Extension Functions
	 --------------------------------------*/

	rc = pti->GetExtensionFunctions(&xcnt, &exfn);

	if (JVMTI_ERROR_NONE != rc) {
		IBMRAS_DEBUG_1(debug, "GetExtensionFunctions: rc = %d", rc);
	}

	/* Cleanup after GetExtensionFunctions while extracting information */

	tDPP.setTraceOption = 0;
	tDPP.jvmtiRegisterTraceSubscriber = 0;
	tDPP.jvmtiDeregisterTraceSubscriber = 0;
	tDPP.jvmtiGetTraceMetadata = 0;
	tDPP.jvmtiGetMethodAndClassNames = 0;
	tDPP.jvmtiFlushTraceData = 0;
	tDPP.getJ9method = 0;
	tDPP.pti = pti;
	IBMRAS_DEBUG(debug, "before launchagent 2");

	fi = exfn;
	for (i = 0; i < xcnt; i++) {
		if (0 == strcmp(fi->id, COM_IBM_REGISTER_TRACE_SUBSCRIBER)) {
			tDPP.jvmtiRegisterTraceSubscriber = fi->func;
		} else if (0 == strcmp(fi->id, COM_IBM_DEREGISTER_TRACE_SUBSCRIBER)) {
			tDPP.jvmtiDeregisterTraceSubscriber = fi->func;
		} else if (0 == strcmp(fi->id, COM_IBM_GET_TRACE_METADATA)) {
			tDPP.jvmtiGetTraceMetadata = fi->func;
		} else if (0 == strcmp(fi->id, COM_IBM_QUERY_VM_DUMP)) {
			tDPP.jvmtiQueryVmDump = fi->func;
		} else if (0 == strcmp(fi->id, COM_IBM_GET_MEMORY_CATEGORIES)) {
			tDPP.jvmtiGetMemoryCategories = fi->func;
		} else if (0 == strcmp(fi->id, COM_IBM_GET_METHOD_AND_CLASS_NAMES)) {
			tDPP.jvmtiGetMethodAndClassNames = fi->func;
		} else if (0 == strcmp(fi->id, COM_IBM_FLUSH_TRACE_DATA)) {
			tDPP.jvmtiFlushTraceData = fi->func;
		} else if (0 == strcmp(fi->id, COM_IBM_GET_J9METHOD)) {
			tDPP.getJ9method = fi->func; /* j9Method ID lookup*/
		} else if (0 == strcmp(fi->id, COM_IBM_SET_VM_TRACE)) {
			tDPP.setTraceOption = fi->func;
		} else if (0 == strcmp(fi->id, COM_IBM_SET_VM_JLM_DUMP)) {
			tDPP.dumpVMLockMonitor = fi->func;
		} else if (0 == strcmp(fi->id, COM_IBM_SET_VM_JLM)) {
			tDPP.setVMLockMonitor = fi->func;
		}
		/* Cleanup */

		pi = fi->params;

		for (j = 0; j < fi->param_count; j++) {
			pti->Deallocate((unsigned char*) pi->name);

			pi++;
		}
		pti->Deallocate((unsigned char*) fi->id);
		pti->Deallocate((unsigned char*) fi->short_description);
		pti->Deallocate((unsigned char *) fi->params);
		pti->Deallocate((unsigned char *) fi->errors);

		fi++;
	}
	pti->Deallocate((unsigned char *) exfn);

	/*--------------------------------------
	 Manage Extension Events
	 -------------------------------------*/

	rc = pti->GetExtensionEvents(&xcnt, &exev);

	/* Cleanup after GetExtensionEvents while extracting information */

	ei = exev;

	for (i = 0; i < xcnt; i++) {

		/* Cleanup */

		pi = ei->params;

		for (j = 0; j < ei->param_count; j++) {
			pti->Deallocate((unsigned char*) pi->name);

			pi++;
		}
		pti->Deallocate((unsigned char*) ei->id);
		pti->Deallocate((unsigned char*) ei->short_description);
		pti->Deallocate((unsigned char *) ei->params);

		ei++;
	}
	pti->Deallocate((unsigned char *) exev);

	memset(&cb, 0, sizeof(cb));

	cb.VMInit = cbVMInit;
	cb.VMDeath = cbVMDeath;

	res = pti->SetEventCallbacks(&cb, sizeof(cb));
	res = pti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_INIT,
			NULL);
	res = pti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_DEATH,
			NULL);

//	res = vm->GetEnv((void **) &env, JNI_VERSION);
//	if (res < 0) {
//		return JNI_ERR;
//	}

	if (rc == JNI_OK) {
		fInitialized = 1;
	}

	IBMRAS_DEBUG_1(debug, "< agentstart rc=%d", rc);
	return rc;
}

int ExceptionCheck(JNIEnv *env) {
	if (env->ExceptionCheck()) {
		IBMRAS_DEBUG(debug, "JNI exception:");
		env->ExceptionDescribe();
		env->ExceptionClear();
		return 1;
	} else {
		return 0;
	}
}


void getHCProperties(const std::string &options) {

	JNIEnv *ourEnv = NULL;

	jint rc = theVM->GetEnv((void **) &ourEnv, JNI_VERSION);
	if (rc < 0 || NULL == ourEnv) {
		IBMRAS_DEBUG(warning, "getEnv failed");
		return;
	}

	jclass hcoptsClass =
			ourEnv->FindClass(
					"com/ibm/java/diagnostics/healthcenter/agent/mbean/HealthCenterOptionHandler");
	if (ExceptionCheck(ourEnv) || hcoptsClass == NULL) {
		IBMRAS_DEBUG(warning, "could not find HealthCenterOptionHandler")
		return;
	}

	jmethodID getPropertiesMethod = ourEnv->GetStaticMethodID(hcoptsClass,
			"getProperties", "([Ljava/lang/String;)[Ljava/lang/String;");
	if (ExceptionCheck(ourEnv) || getPropertiesMethod == NULL) {
		IBMRAS_DEBUG(warning, "could not find getProperties method")
		return;
	}

	std::stringstream ss;
	ss << ibmras::common::port::getProcessId();
	std::string pid = ss.str();
	jobjectArray applicationArgs = NULL;

	jstring pidArg = ourEnv->NewStringUTF(pid.c_str());
	if (!ExceptionCheck(ourEnv)) {
		jstring opts = ourEnv->NewStringUTF(options.c_str());
		if (!ExceptionCheck(ourEnv)) {
			applicationArgs = ourEnv->NewObjectArray(2,
					ourEnv->FindClass("java/lang/String"), NULL);
			if (!ExceptionCheck(ourEnv)) {
				ourEnv->SetObjectArrayElement(applicationArgs, 0, pidArg);
				if (!ExceptionCheck(ourEnv)) {
					ourEnv->SetObjectArrayElement(applicationArgs, 1, opts);
					if (ExceptionCheck(ourEnv)) {
						applicationArgs = NULL;
					}
				} else {
					applicationArgs = NULL;
				}
			}
		}
	}


	jobjectArray hcprops = (jobjectArray)ourEnv->CallStaticObjectMethod(hcoptsClass,
			getPropertiesMethod, applicationArgs);

	if (ExceptionCheck(ourEnv) || hcprops == NULL) {
		IBMRAS_DEBUG(warning, "No healthcenter.properties found")
		return;
	}

	jsize numProps = ourEnv->GetArrayLength(hcprops);

	ibmras::common::Properties theProps;

	for( jsize i = 0; i < numProps; ++i )
	{
		jstring line = (jstring) ourEnv->GetObjectArrayElement(hcprops, i );
		const char* lineChars = ourEnv->GetStringUTFChars(line, false);

		std::string lineStr(lineChars);
		size_t pos = lineStr.find('=');
		if ((pos != std::string::npos) && (pos < lineStr.size())) {
			std::string key (lineStr.substr(0, pos));
			std::string value (lineStr.substr(pos + 1));
			theProps.put(key, value);

		}

		ourEnv->ReleaseStringUTFChars(line, lineChars);

	}

	std::string agentPropertyPrefix = agent->getAgentPropertyPrefix();
	std::list<std::string> hcPropKeys = theProps.getKeys(HEALTHCENTER_PROPERTIES_PREFIX);
	for (std::list<std::string>::iterator i = hcPropKeys.begin(); i != hcPropKeys.end();
			++i) {
		std::string key = i->substr(strlen(HEALTHCENTER_PROPERTIES_PREFIX));
		if (key.length() > 0) {
			std::string newKey =  agentPropertyPrefix + key;
			if (!theProps.exists(newKey)) {
				theProps.put(newKey, theProps.get(*i));
			}
		}
	}

	agent->setProperties(theProps);
}

/**
 * launch agent code
 */
void launchAgent(const std::string &options) {

	agent = ibmras::monitoring::agent::Agent::getInstance();
	agent->setAgentProperty("launch.options", options);

	getHCProperties(options);
	agent->setLogLevels();


	// Add MQTT Connector plugin
	// TODO load SSL or plain
	std::string agentLibPath =
			ibmras::common::util::LibraryUtils::getLibraryDir(
					"healthcenter.dll", (void*) launchAgent);
	agent->addPlugin(agentLibPath, "hcmqtt");


	// Set connector properties based on data.collection.level
	std::string dataCollectionLevel = agent->getAgentProperty("data.collection.level");
	if (ibmras::common::util::equalsIgnoreCase(dataCollectionLevel, "headless")) {
		agent->setAgentProperty("headless", "on");
		agent->setAgentProperty("mqtt", "off");
		agent->setAgentProperty("jmx", "off");
	} else {
		std::string jmx = agent->getAgentProperty("jmx");
		if (jmx == "") {
			agent->setAgentProperty("jmx", "on");
		}
	}

	IBMRAS_DEBUG(debug, "in agent launch agent");

	if (tDPP.pti == NULL) {
		IBMRAS_DEBUG(debug, "tDPP.pti is null");
	}

	agent->addPlugin(
			ibmras::monitoring::plugins::j9::trace::TraceDataProvider::getInstance(
					tDPP));
	agent->addPlugin(
			ibmras::monitoring::plugins::j9::methods::MethodLookupProvider::getInstance(
					tDPP));
	agent->addPlugin(
			ibmras::monitoring::connector::jmx::JMXConnectorPlugin::getInstance(theVM));
	agent->addPlugin(
			ibmras::monitoring::connector::headless::HLConnectorPlugin::getInstance(theVM));

////	 //The next call invoked the setJVM function on the JMX plugin
	ibmras::monitoring::plugins::jmx::setJVM(tDPP.theVM);
////	// We now register the plugin with the agent
	agent->addPlugin(ibmras::monitoring::plugins::jmx::getPlugin());

	ibmras::monitoring::plugins::jni::setTDPP(&tDPP);
	//We now register the plugin with the agent
	agent->addPlugin(ibmras::monitoring::plugins::jni::getPlugin());

	// Add the jni receiver
	agent->addPlugin((ibmras::monitoring::Plugin*) new ibmras::monitoring::plugins::jni::JNIReceiver());

	agent->init();
	agent->start();

}

