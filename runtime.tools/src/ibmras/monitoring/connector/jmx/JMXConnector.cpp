/*
 * JMXConnector.cpp
 *
 *  Created on: 4 Jul 2014
 *      Author: robbins
 */

#include "ibmras/monitoring/connector/jmx/JMXConnector.h"
#include "ibmras/common/logging.h"


#include "jvmti.h"
#include "jni.h"
#include "ibmjvmti.h"
#include "jniport.h"

/*
 * This is compiled with convlit(ISO8859-1) so we need to suspend conversion for literals
 * passed to OS functions such as printf when on zOS
 * We suspend conversion here and put a resume/suspend pair aroun literals that need to
 * be in ISO8859-1 encoding
 */
#ifdef _ZOS
#pragma convlit(suspend)
#include <unistd.h>
#endif

/* ensure common reporting of JNI version required */
#define JNI_VERSION JNI_VERSION_1_4

static jclass javaHCLaunchMBean = NULL;
static jmethodID mainMethod = NULL;
static jobjectArray applicationArgs = NULL;
static jstring applicationArg0 = NULL;
static jstring applicationArg1 = NULL;
static int processID = 0;
static char args0[20];

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

namespace ibmras {
namespace monitoring {
namespace connector {
namespace jmx {

IBMRAS_DEFINE_LOGGER("JMXConnector");

JMXConnector::JMXConnector(JavaVM *theVM, const std::string &options,
		const char* properties) :
		vm(theVM), agentOptions(options) {
}

JMXConnector::~JMXConnector() {
}

int JMXConnector::start() {

	int rc = launchMBean();

	return rc;
}

int JMXConnector::stop() {
	int rc = 0;

	return rc;
}

/*****************************
 * Check & clear JNI Exception
 * Return 1 (true) if there was an Exception
 * return 0 (false) otherwise.
 */
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

/******************************/
int JMXConnector::launchMBean() {
	IBMRAS_DEBUG(debug, "> launchMBean");

	JNIEnv *env = NULL;
	javaHCLaunchMBean = NULL;
	mainMethod = NULL;
	IBMRAS_DEBUG_1(debug, "VM structure: %p", vm);
	jint rc = vm->GetEnv((void **) &env, JNI_VERSION);
	if (rc < 0 || NULL == env) {
		IBMRAS_DEBUG(warning, "Agent_OnAttach: GetEnv failed");
		return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_JVMTI_ERR;
	}

#ifdef _ZOS
#pragma convlit(resume)
#endif
	if (NULL == javaHCLaunchMBean) {
		javaHCLaunchMBean =
				env->FindClass(
						"com/ibm/java/diagnostics/healthcenter/agent/mbean/HCLaunchMBean");
#ifdef _ZOS
#pragma convlit(suspend)
#endif
		if (ExceptionCheck(env) || NULL == javaHCLaunchMBean) {
			IBMRAS_LOG(warning,
					"launchMBean couldn't find com.ibm.java.diagnostics.healthcenter/agent/mbean/HCLaunchMBean class. Agent not started.");
			return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR;
		}
	}

#ifdef _ZOS
#pragma convlit(resume)
#endif
	if (NULL == mainMethod) {
		mainMethod = env->GetStaticMethodID(javaHCLaunchMBean, "main",
				"([Ljava/lang/String;)V");
#ifdef _ZOS
#pragma convlit(suspend)
#endif
		if (ExceptionCheck(env) || NULL == mainMethod) {
			IBMRAS_LOG(warning,
					"launchMBean couldn't find main method in HCLaunchMBean class. Agent not started.");
			return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR;
		}
	}

#ifdef _ZOS
#pragma convlit(resume)
#endif
	applicationArgs = env->NewObjectArray(2, env->FindClass("java/lang/String"),
			NULL);
#ifdef _ZOS
#pragma convlit(suspend)
#endif
	/* should throw OOM or come back null */
	if (ExceptionCheck(env) || NULL == applicationArgs) {
		IBMRAS_LOG(warning,
				"launchMBean couldn't create object array. Agent not started.");
		return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR;
	}

	processID = ibmras::common::port::getProcessId();
	sprintf(args0, "%d", processID);

#ifdef _ZOS
	__etoa(args0);
#endif

	applicationArg0 = env->NewStringUTF(args0);
	/* should throw OOM or come back null */
	if (ExceptionCheck(env) || NULL == applicationArg0) {
		IBMRAS_LOG(warning,
				"launchMBean couldn't create jstring for main args. Agent not started.");
		return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR;
	}

	env->SetObjectArrayElement(applicationArgs, 0, applicationArg0);
	if (ExceptionCheck(env)) {
		IBMRAS_LOG(warning,
				"launchMBean couldn't set object array element for main args. Agent not started.");
		return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR;
	}

	applicationArg1 = env->NewStringUTF(agentOptions.c_str());
	/* should throw OOM or come back null */
	if (ExceptionCheck(env) || NULL == applicationArg1) {
		IBMRAS_LOG(warning,
				"launchMBean couldn't create jstring for main args. Agent not started.");
		return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR;
	}

	/* Also pass our options to the launcher. */
	env->SetObjectArrayElement(applicationArgs, 1, applicationArg1);
	if (ExceptionCheck(env)) {
		IBMRAS_LOG(warning,
				"launchMBean couldn't set object array element for main args. Agent not started.");
		return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_ERR;
	}

	env->CallStaticVoidMethod(javaHCLaunchMBean, mainMethod, applicationArgs);
	/* could throw an exception (doesn't say it'll return null) */
	if (ExceptionCheck(env)) {
		IBMRAS_LOG(warning,
				"launchMBean couldn't run main on com/ibm/java/diagnostics/healthcenter/agent/mbean/HCLaunchMBean class. Agent not started.");
		return com_ibm_java_diagnostics_healthcenter_agent_lateattach_AttachAgent_attachAgent_MBEAN_EXC;
	}

	/* Everything OK, return OK */
	IBMRAS_LOG(debug, "< launchMBean");

	return JNI_OK;
}

} /* namespace jmx */
} /* namespace connector */
} /* namespace monitoring */
} /* namespace ibmras */

using namespace ibmras::monitoring::connector::jmx;

extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_ibm_java_diagnostics_healthcenter_agent_mbean_HealthCenter_getProviders(
		JNIEnv * jni_env, jobject obj) {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	ibmras::monitoring::agent::BucketList* buckets = agent->getBucketList();
	std::vector<std::string> ids = buckets->getIDs();

	jclass stringClass = jni_env->FindClass("java/lang/String");
	jobjectArray stringArray = jni_env->NewObjectArray(ids.size(), stringClass,
			0);
	for (uint32 i = 0; i < ids.size(); ++i) {
		jstring javaString = jni_env->NewStringUTF(ids[i].c_str());
		jni_env->SetObjectArrayElement(stringArray, i, javaString);
	}
	return stringArray;

}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_ibm_java_diagnostics_healthcenter_agent_dataproviders_MonitoringDataProvider_getData(
		JNIEnv * jni_env, jobject obj, jstring name, jint requestedSize, jintArray requestedId) {
	const char* bucketName = jni_env->GetStringUTFChars(name, NULL);

	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	ibmras::monitoring::agent::BucketList* buckets = agent->getBucketList();
	ibmras::monitoring::agent::Bucket* bucket = buckets->findBucket(bucketName);

	if (bucket == NULL) {
		return NULL;
	}
	jni_env->ReleaseStringUTFChars(name, bucketName);

	signed char* data = NULL;
	int32 size = requestedSize;

	jint * retID = jni_env->GetIntArrayElements(requestedId,NULL);
	uint32 id = retID[0];

	id = bucket->getNextData(id, size, (void*&) data);
	if (size == 0) {
		return NULL;
	}

	retID[0] = id;
	jni_env->ReleaseIntArrayElements( requestedId, retID, 0);

	jbyteArray buffersByteArray = jni_env->NewByteArray(size);
	jni_env->SetByteArrayRegion(buffersByteArray, 0, (int) size, data);

	if (data) {
		delete[] data;
	}

	return buffersByteArray;

}


extern "C" JNIEXPORT void JNICALL
Java_com_ibm_java_diagnostics_healthcenter_agent_dataproviders_MonitoringDataProvider_sendMessage(
		JNIEnv * jni_env, jobject obj, jstring topic, jstring message) {
	const char* subject = jni_env->GetStringUTFChars(topic, NULL);
	const char* msg = jni_env->GetStringUTFChars(message, NULL);

	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	ibmras::monitoring::connector::ConnectorManager *conMan = agent->getConnectionManager();
	conMan->receiveMessage(subject, strlen(msg), (void*)msg);

	jni_env->ReleaseStringUTFChars(topic, subject);
	jni_env->ReleaseStringUTFChars(message, msg);

}

extern "C" JNIEXPORT jstring JNICALL
Java_com_ibm_java_diagnostics_healthcenter_agent_dataproviders_MonitoringDataProvider_getSourceDetails(
		JNIEnv * jni_env, jobject obj, jstring name) {

	const char* bucketName = jni_env->GetStringUTFChars(name, NULL);
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	ibmras::monitoring::agent::BucketList* buckets = agent->getBucketList();
	ibmras::monitoring::agent::Bucket* bucket = buckets->findBucket(bucketName);

	jni_env->ReleaseStringUTFChars(name, bucketName);

	if (bucket == NULL) {
		return NULL;
	}

	ibmras::monitoring::agent::DataSource<pullsource> *source = agent->getPullSource(bucket->getUniqueID());

	if (source == NULL) {
		ibmras::monitoring::agent::DataSource<pushsource> *pushSource = agent->getPushSource(bucket->getUniqueID());
		if (pushSource == NULL) {
			return jni_env->NewStringUTF("DID NOT FIND IT");
		} else {
			std::string config = pushSource->getConfig();
			jstring javaString = jni_env->NewStringUTF(config.c_str());
			return javaString;
		}
	} else {
		std::string config = source->getConfig();
		jstring javaString = jni_env->NewStringUTF(config.c_str());
		return javaString;
	}
}



