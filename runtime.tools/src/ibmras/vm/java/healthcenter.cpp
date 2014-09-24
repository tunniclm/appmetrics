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
#include "ibmras/vm/java/healthcenter.h"
#include "ibmras/common/Properties.h"

struct __jdata;

#if defined(WINDOWS)
#include <winsock2.h>
#else /* Unix platforms */
#define _OE_SOCKETS
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include "jvmti.h"
#include "jni.h"
#include "ibmjvmti.h"
#include "jni.h"
#include "jniport.h"
#include "ibmras/monitoring/plugins/jmx/JMX.h"
#include "ibmras/monitoring/plugins/jni/Facade.h"

#ifdef __cplusplus
extern "C" {
#endif
#undef com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_OK
#define com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_OK 0L
#undef com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_ERR
#define com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_ERR -1L
#undef com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_ALREADY_LOADED
#define com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_ALREADY_LOADED -2L
#undef com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_JVMTI_ERR
#define com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_JVMTI_ERR -3L
#undef com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR
#define com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR -4L
#undef com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_EXC
#define com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_EXC -5L
#ifdef __cplusplus
}
#endif

/*########################################################################################################################*/
/*########################################################################################################################*/
/*########################################################################################################################*/

#if defined(WINDOWS)
#include <windows.h>
#include <winbase.h>
#include <psapi.h>
#pragma comment(lib,"psapi.lib")
#endif /* defined(WIN32) || defined(WIN64) */


static const char* HEALTHCENTER_PROPERTIES_PREFIX = "com.ibm.java.diagnostics.healthcenter.";

jint launchMBean(char *options);
void launchAgent(const std::string &options);
std::string agentOptions;
ibmras::common::Properties hcprops;
static JavaVM *theVM;
jvmFunctions tDPP;

jvmtiEnv *pti = NULL;
JNIEnv* env = NULL;
jclass javaHCLaunchMBean = NULL;
static jmethodID mainMethod = NULL;
static jobjectArray applicationArgs = NULL;
static jstring applicationArg0 = NULL;
static jstring applicationArg1 = NULL;
static int processID = 0;
static char args0[20];



typedef struct __jdata jdata_t;

/* ensure common reporting of JNI version required */
#define JNI_VERSION JNI_VERSION_1_4

jint agentStart(JavaVM *vm, char *options, void *reserved, int onAttach);

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
    std::cout << "> Agent_OnAttach";

    if (env != NULL) {
    	IBMRAS_DEBUG(fine, "Agent_OnAttach: env not NULL, no need to call agentStart");
    	env = NULL;
    	javaHCLaunchMBean = NULL;
    	mainMethod = NULL;
   		rc = vm->GetEnv((void **)&env, JNI_VERSION);
   	    if (rc < 0 || NULL == env)
   	    {
   	        IBMRAS_LOG(warning, "Agent_OnAttach: GetEnv failed");
   	        return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_JVMTI_ERR;
   	    }
    	rc = launchMBean(options);
    } else {
    	IBMRAS_DEBUG(fine, "Agent_OnAttach: env is NULL, calling agentStart");
    	rc = agentStart(vm, options, reserved, 1);
    	std::cout << "> agentStart called";
    	launchAgent(options);
    }

    IBMRAS_DEBUG_1(debug, "< Agent_OnAttach. rc=%d", rc);
    std::cout << "< Agent_OnAttach. rc= " << rc;
    return rc;
}

/******************************/
JNIEXPORT jint JNICALL
Agent_OnLoad(JavaVM *vm, char *options, void *reserved) {
	IBMRAS_DEBUG(debug, "OnLoad");
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

	IBMRAS_LOG(none, "Health Center agent build: " __DATE__ " " __TIME__);

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


void getHCProperties() {

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

	jobjectArray hcprops = (jobjectArray)ourEnv->CallStaticObjectMethod(hcoptsClass,
			getPropertiesMethod, NULL);

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

jint
launchMBean(char* options)
{
    IBMRAS_DEBUG(debug, "> launchMBean");

#ifdef _ZOS
#pragma convlit(resume)
#endif
    if (NULL == javaHCLaunchMBean ) {
    	javaHCLaunchMBean = env->FindClass("com/ibm/java/diagnostics/healthcenter/agent/mbean/HCLaunchMBean");
#ifdef _ZOS
#pragma convlit(suspend)
#endif
    	if (ExceptionCheck(env) || NULL == javaHCLaunchMBean)
    	{
    		IBMRAS_LOG(warning, "launchMBean couldn't find com.ibm.java.diagnostics.healthcenter/agent/mbean/HCLaunchMBean class. Agent not started.");
    		return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR;
    	}
    }

#ifdef _ZOS
#pragma convlit(resume)
#endif
    if (NULL == mainMethod) {
    	mainMethod = env->GetStaticMethodID(javaHCLaunchMBean, "main", "([Ljava/lang/String;)V");
#ifdef _ZOS
#pragma convlit(suspend)
#endif
    	if (ExceptionCheck(env) || NULL == mainMethod)
    	{
    		IBMRAS_LOG(warning, "launchMBean couldn't find main method in HCLaunchMBean class. Agent not started.");
        	return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR;
    	}
    }

#ifdef _ZOS
#pragma convlit(resume)
#endif
    applicationArgs = env->NewObjectArray(2, env->FindClass("java/lang/String"), NULL);
#ifdef _ZOS
#pragma convlit(suspend)
#endif
    /* should throw OOM or come back null */
    if (ExceptionCheck(env) || NULL == applicationArgs)
    {
    	IBMRAS_LOG(warning, "launchMBean couldn't create object array. Agent not started.");
       	return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR;
    }

    processID= ibmras::common::port::getProcessId();
    sprintf(args0, "%d", processID);

#ifdef _ZOS
    __etoa(args0);
#endif

    applicationArg0 = env->NewStringUTF(args0);
    /* should throw OOM or come back null */
    if (ExceptionCheck(env) || NULL == applicationArg0)
    {
    	IBMRAS_LOG(warning, "launchMBean couldn't create jstring for main args. Agent not started.");
       	return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR;
    }

    env->SetObjectArrayElement(applicationArgs, 0, applicationArg0);
    if (ExceptionCheck(env))
    {
    	IBMRAS_LOG(warning,"launchMBean couldn't set object array element for main args. Agent not started.");
        return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR;
    }

    applicationArg1 = env->NewStringUTF(options);
    /* should throw OOM or come back null */
    if (ExceptionCheck(env) || NULL == applicationArg1)
    {
    	IBMRAS_LOG(warning,"launchMBean couldn't create jstring for main args. Agent not started.");
       	return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR;
    }

    /* Also pass our options to the launcher. */
    env->SetObjectArrayElement(applicationArgs, 1, applicationArg1);
    if (ExceptionCheck(env))
    {
    	IBMRAS_LOG(warning,"launchMBean couldn't set object array element for main args. Agent not started.");
       	return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR;
    }

    env->CallStaticVoidMethod(javaHCLaunchMBean, mainMethod, applicationArgs);
    /* could throw an exception (doesn't say it'll return null) */
    if (ExceptionCheck(env))
    {
    	IBMRAS_LOG(warning,"launchMBean couldn't run main on com/ibm/java/diagnostics/healthcenter/agent/mbean/HCLaunchMBean class. Agent not started.");
        return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_EXC;
    }

    /* Everything OK, return OK */
    IBMRAS_DEBUG(debug, "< launchMBean");
    return JNI_OK;
}

/**
 * launch agent code
 */
void launchAgent(const std::string &options) {

	agent = ibmras::monitoring::agent::Agent::getInstance();

	getHCProperties();
	agent->setLogLevels();
	IBMRAS_DEBUG(debug, "in agent launch agent");

	IBMRAS_DEBUG_1(fine, "options %s", options.c_str());
	agent->setSearchPath(options);

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
			ibmras::monitoring::connector::jmx::JMXConnectorPlugin::getInstance(theVM,
					agentOptions));
	agent->addPlugin(
			ibmras::monitoring::connector::headless::HLConnectorPlugin::getInstance(theVM,
					agentOptions));

////	 //The next call invoked the setJVM function on the JMX plugin
	ibmras::monitoring::plugins::jmx::setJVM(tDPP.theVM);
////	// We now register the plugin with the agent
	agent->addPlugin(ibmras::monitoring::plugins::jmx::getPlugin());

	ibmras::monitoring::plugins::jni::setTDPP(&tDPP);
	//We now register the plugin with the agent
	agent->addPlugin(ibmras::monitoring::plugins::jni::getPlugin());

	agent->init();
	agent->start();

}

