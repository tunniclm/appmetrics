/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include "ibmras/monitoring/plugins/j9/jni/CFacade.h"
#include "ibmras/common/logging.h"
#include "ibmras/common/MemoryManager.h"
#include "ibmras/common/util/memUtils.h"
#include "ibmras/common/util/strUtils.h"
#include "ibmras/vm/java/healthcenter.h"
#include <cstring>
#include <stdlib.h>
#include <string>
#include <stdint.h>

#if defined(WINDOWS)
#include <windows.h>
#include <intrin.h>
#include <winbase.h>
#else /* Unix platforms */
#define _OE_SOCKETS

#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#if defined(_ZOS)
#include <unistd.h>
#endif

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace jni {

uint64_t tm_stt;
const char* JNIdpVersion ="99.99.99";

#if defined(WINDOWS)
uint64_t rdtsc()
{
	return __rdtsc();
}
#endif

jvmFunctions* jvmF;
JavaVM* vm = NULL;
SourceManager* mgr = new SourceManager;

namespace env {
PullSource* getENVPullSource(uint32 id);
}

namespace threads {
PullSource* getTDPullSource(uint32 id);
}

namespace memory {
PullSource* getMEMPullSource(uint32 id);
}

namespace memorycounter {
PullSource* getMCPullSource(uint32 id);
}

namespace locking {
PullSource* getJLAPullSource(uint32 id);
}

#if defined(_ZOS)
#else
extern "C" {
#endif

DECL pullsource* registerPullSourcejni(agentCoreFunctions aCF,uint32 provID) {
	return ibmras::monitoring::plugins::j9::jni::mgr->registerPullSource(provID);
}

DECL int startjni() {
	return ibmras::monitoring::plugins::j9::jni::mgr->start();
}

DECL int stopjni() {
	return ibmras::monitoring::plugins::j9::jni::mgr->stop();
}

DECL const char* getVersionJNI() {
	return JNIdpVersion;
}

#if defined(_ZOS)
#else
}
#endif

IBMRAS_DEFINE_LOGGER("DataProviderSources")
;

SourceManager::SourceManager() {
	running = false;
	provid = -1;
	pullsources = NULL;
	vm = NULL;
}

pullsource* SourceManager::registerPullSource(uint32 provID) {
	IBMRAS_DEBUG(fine, "Registering pull sources");
	provid = provID;
	pullsources = new PullSource*[PULL_COUNT];

	pullsources[ENV] = ibmras::monitoring::plugins::j9::jni::env::getENVPullSource(
			provid);
	pullsource* src = pullsources[ENV]->getDescriptor();
	pullsource* curr = src;

	pullsources[TD] =
			ibmras::monitoring::plugins::j9::jni::threads::getTDPullSource(provid);
	curr->next = pullsources[TD]->getDescriptor();
	curr = curr->next;

	pullsources[MEM] =
			ibmras::monitoring::plugins::j9::jni::memory::getMEMPullSource(provid);
	curr->next = pullsources[MEM]->getDescriptor();
	curr = curr->next;

	pullsources[MC] =
			ibmras::monitoring::plugins::j9::jni::memorycounter::getMCPullSource(
					provid);
	curr->next = pullsources[MC]->getDescriptor();
	curr = curr->next;

	pullsources[JLA] =
			ibmras::monitoring::plugins::j9::jni::locking::getJLAPullSource(provid);
	curr->next = pullsources[JLA]->getDescriptor();
	curr = curr->next;

	return src;
}

int SourceManager::start() {
	IBMRAS_DEBUG(info, "Starting");
	// Ask each source to publish it's config
	for (uint32 i = 0; i < PULL_COUNT; i++) {
		PullSource* p = pullsources[i];
		if (p) {
			p->publishConfig();
		}
	}

#ifndef ORACLE
	tm_stt = read_cycles_on_processor(0);
	//getTDPP().setVMLockMonitor(getTDPP().pti, 1);
	jvmF->setVMLockMonitor(jvmF->pti, 1);
#endif

	return 0;
}

int SourceManager::stop() {
	IBMRAS_DEBUG(info, "Stopping");
	for (uint32 i = 0; i < PULL_COUNT; i++) {
		PullSource* p = pullsources[i];
		if (p) {
			delete p;
		}
	}
	return 0;
}

uint32 SourceManager::getProvID() {
	return provid;
}

DECL void setTDPP(jvmFunctions* tdpp) {
	ibmras::monitoring::plugins::j9::jni::vm = tdpp->theVM;
	ibmras::monitoring::plugins::j9::jni::jvmF = tdpp;
}

DECL JavaVM* getJVM() {
	return vm;
}

DECL jvmFunctions* getTDPP() {
	return jvmF;
}

monitordata* PullSource::generateError(char* msg) {
	monitordata* data = new monitordata;
	data->persistent = false;
	data->provID = getProvID();
	data->sourceID = getSourceID();
	data->size = strlen(msg);
	data->data = msg;
	return data;
}

std::string getString(JNIEnv* env, const char* cname, const char* mname,
		const char* signature) {

	IBMRAS_DEBUG(debug, ">>getString");

	IBMRAS_DEBUG(debug, "Retrieving class");
	jclass clazz = env->FindClass(cname);
	if (!clazz) {
		IBMRAS_DEBUG(warning, "Failed to find class");
		return "";
	}

	jmethodID method = env->GetStaticMethodID(clazz, mname, signature);
	if (!method) {
		IBMRAS_DEBUG(warning, "Failed to get %s method ID");
		return "";
	}

	jstring jobj = (jstring) env->CallStaticObjectMethod(clazz, method, NULL);

	const char* value = env->GetStringUTFChars(jobj, NULL);
	if (env->ExceptionOccurred()) {
		env->ExceptionDescribe();
	}

	std::string sval(value);

	env->ReleaseStringUTFChars(jobj, value);
	env->DeleteLocalRef(jobj);

	IBMRAS_DEBUG(debug, "<<getString");
	return sval;
}

monitordata* PullSource::generateData() {

	IBMRAS_DEBUG(debug, ">>CFacade's generateData()");

	if (!env) {
		JavaVMAttachArgs threadArgs;

		memset(&threadArgs, 0, sizeof(threadArgs));
		threadArgs.version = JNI_VERSION_1_4;
		threadArgs.name = ibmras::common::util::createAsciiString(name.c_str());
		threadArgs.group = NULL;
		IBMRAS_DEBUG_1(debug, "Attaching thread %s", name.c_str());
		jint errcode = vm->AttachCurrentThreadAsDaemon((void **) &env, &threadArgs);
		ibmras::common::memory::deallocate((unsigned char**)&threadArgs.name);
		if (errcode != JNI_OK) {
			IBMRAS_DEBUG_1(debug, "AttachCurrentThreadAsDaemon failed %d", errcode);
			return NULL;
		}

		IBMRAS_DEBUG_1(debug, "Attached thread %s", name.c_str());
	}

	monitordata* data = sourceData(jvmF, env);
	IBMRAS_DEBUG(debug, "<<CFacade's generateData() [DATA]");

	return data;
}

void PullSource::pullComplete(monitordata* mdata) {
	if (mdata) {
		hc_dealloc((unsigned char**) (&(mdata->data)));
		delete mdata;
	} else {
		if (env) {
			IBMRAS_DEBUG_1(debug, "Detaching thread %s", name.c_str());
			vm->DetachCurrentThread();
			env = NULL;
		}
	}
}

PullSource::PullSource(uint32 id, const std::string& providerName) :
		provID(id), env(NULL), name(providerName) {
}

PullSource::~PullSource() {
}

DECL ibmras::monitoring::Plugin* getPlugin() {
	ibmras::monitoring::Plugin* plugin = new ibmras::monitoring::Plugin;
	plugin->name = "JNI data providers";
	plugin->pull = ibmras::monitoring::plugins::j9::jni::registerPullSourcejni;
	plugin->push = NULL;
	plugin->getVersion = ibmras::monitoring::plugins::j9::jni::getVersionJNI;
	plugin->start = ibmras::monitoring::plugins::j9::jni::startjni;
	plugin->stop = ibmras::monitoring::plugins::j9::jni::stopjni;
	plugin->type = ibmras::monitoring::plugin::data;
	plugin->recvfactory = NULL;
	plugin->confactory = NULL;
	return plugin;
}

/*******************************************************************************************
 * UTILITY FUNCTIONS
 */

int ExceptionCheck(JNIEnv *env) {
	if (env->ExceptionCheck()) {
		IBMRAS_DEBUG(warning, "JNI exception:");
		env->ExceptionDescribe();
		env->ExceptionClear();
		return 1;
	} else {
		return 0;
	}
}

void dump_read(void * dp, void * res, int size) {
	memcpy(res, dp, size);
	return;
}

unsigned int dump_read_u4(char * dp) {
	unsigned int temp;

	dump_read((void *) dp, (void *) &temp, 4);
	temp = ntohl(temp);
	return temp;
}

UINT64 read_cycles_on_processor(int cpu) {
	UINT64 cycles = 0;

#ifdef _ZOS
	__stck( (unsigned long long *)&cycles); /* Use unaltered time for JLM */
#else
#if defined (_LINUX) && !defined(_PPC) && !defined(_S390)
	cycles = readCyclesOnProcessor(cpu);
#else
	getCycles(&cycles);
#endif
#endif

	return cycles;
}

#if defined (_LINUX) && !defined(_PPC) && !defined(_S390)
/*
 ReadCyclesOnProcessor()
 ************************
 Read cycles on a specified processor
 */

uint64_t readCyclesOnProcessor(int cpu) {
	cpu_set_t prev_mask; /* current processor affinity mask */
	cpu_set_t new_mask; /* mask to attach to a processor */
	uint64_t mycycles;

	/* get affinity mask for the current process */
	/* the call for this depends on which version of glibc we are using */
//#ifdef _NEW_GLIBC
	int sched_get_return_code = sched_getaffinity(0, sizeof(cpu_set_t),
			&prev_mask);
//#else
//    int sched_get_return_code = sched_getaffinity(0, &prev_mask);
//#endif

	if (sched_get_return_code != 0) {
		printf("sched_getaffinity failed\n");
		return PITRACE_ERROR;
	}
	/* switch & read cycles      */

	CPU_ZERO(&new_mask);
	CPU_SET(cpu, &new_mask);

//#ifdef _NEW_GLIBC
	int sched_set_return_code = sched_setaffinity(0, sizeof(cpu_set_t),
			&new_mask);
//#else
//    int sched_set_return_code = sched_setaffinity(0, &new_mask);
//#endif

	if (sched_set_return_code != 0) {
		printf("sched_setaffinity failed\n");
		return PITRACE_ERROR;
	}

	sleep(0);

	mycycles = getPlatformCycles();

	/* restore previous mask */

//#ifdef _NEW_GLIBC
	int sched_reset_return_code = sched_setaffinity(0, sizeof(cpu_set_t),
			&prev_mask);
//#else
//    int sched_reset_return_code = sched_setaffinity(0, &prev_mask);
//#endif

	if (sched_reset_return_code != 0) {
		printf("sched_setaffinity failed\n");
		return PITRACE_ERROR;
	}

	sleep(0);

	return mycycles;
}
#endif

/*
 _GetCycles()
 **************
 This is an architecture specific routine.
 */
uint64_t getPlatformCycles(void) {
	union rdval time;
#ifdef CONFIG_IA64
	int result = 0;
#endif

#if defined(_WINDOWS)
	return rdtsc();
#endif

#if defined (_SOLARIS)
	int result = 0;
#endif

#if defined (_HPIA)
	int result = 0;
#endif

#if defined (_HPRISC)
	int result = 0;
#endif

#if defined(_LINUX) && !defined(_PPC) && !defined(_S390)
	uint32_t lo, hi;
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
	return (uint64_t) hi << 32 | lo;
#endif

#if defined(CONFIG_S390) || defined(CONFIG_S390X) || defined(_S390)
	__asm__ __volatile__("la     1,%0\n stck    0(1)":"=m"(time.cval)
			::"cc", "1");
#endif

#if defined(_PPC) || defined(CONFIG_PPC64) || defined (_AIX)
	uint32_t temp1 = 1;

	time.sval.hval = 2;
	while (temp1 != time.sval.hval)
	{
		__asm__ __volatile__("mftbu %0":"=r"(temp1));
		__asm__ __volatile__("mftb  %0":"=r"(time.sval.lval));
		__asm__ __volatile__("mftbu %0":"=r"(time.sval.hval));
	}
#endif

#ifdef CONFIG_IA64
	__asm__ __volatile__("mov %0=ar.itc":"=r"(time.cval)::"memory");
#ifdef CONFIG_ITANIUM
	while (__builtin_expect((__s32) result == -1, 0))
	__asm__ __volatile__("mov %0=ar.itc":"=r"(time.cval)::"memory");
#endif
#endif
	return time.cval;
}

/******************************/
void getCycles(uint64_t * t) {
	*t = getPlatformCycles();
}

int qcmp_jlm(const void * p1, const void * p2) {
	jdata_t * r;
	jdata_t * s;

	r = *(jdata_t **) p1;
	s = *(jdata_t **) p2;

	if (r->gets > s->gets) {
		return -1;
	}
	if (r->gets < s->gets) {
		return 1;
	}

	return 0;
}

/*******************************
 * MEMORY MANAGEMENT FUNCTIONS *
 *******************************/

unsigned char* hc_alloc(int size) {
	return ibmras::common::memory::allocate(size);
}

void hc_dealloc(unsigned char** buffer) {
	ibmras::common::memory::deallocate(buffer);
}

/**********
 * utility function to free pointers in an array that have been malloced
 **********/
void dealloc_report_lines(char *lines[], int count) {
	int i;
	//IBMRAS_DEBUG(debug,  "> dealloc_report_lines");

	if (lines != NULL) {
		/* Need to free the memory for our array */
		for (i = 0; i < count; i++) {
			hc_dealloc((unsigned char**) &lines[i]);
		}
	}
	//IBMRAS_DEBUG(debug,  "< dealloc_report_lines");
}

/* Our own function to perform realloc of memory via jvmti and check return */
void* hc_realloc_ptr_array(char** source[], int currentSize, int newSize) {
	char** buffer = NULL;
	int i;

	if (currentSize >= newSize) {
		return *source;
	}

	buffer = (char**) hc_alloc(newSize * sizeof(char*));
	if (buffer != NULL) {
		for (i = 0; i < currentSize; i++) {
			buffer[i] = (*source)[i];
		}
	}
	hc_dealloc((unsigned char**) source);
	*source = buffer;
	return buffer;
}

/*********************************
 * STRING MANIPULATION FUNCTIONS *
 *********************************/

void force2Native(const char * str) {
	const char *p = str;

	if (NULL != str) {
		while (0 != *p) {
			if (0 != (0x80 & *p)) {
				p = NULL;
				break;
			}
			p++;
		}
#ifdef _ZOS
		if ( NULL != p )
		{
			__atoe( (char *)str);
		}
#endif
	}
}

/******************************
 * returns a valid string (might be "NULL")
 * or an actual NULL if out of memory
 */
char * dupJavaStr(const char * jnm) {
	char* cp = NULL;
	if (NULL != jnm) {
		cp = (char*) ibmras::monitoring::plugins::j9::jni::hc_alloc(
				strlen(jnm) + 1);
		if (NULL == cp) {
			return NULL;
		} else {
			/* jnm is valid, so is cp */
			strcpy(cp, jnm);
			force2Native(cp);
		}
	} else {
		/* malloc enough space for the word null */
		cp = (char*) ibmras::monitoring::plugins::j9::jni::hc_alloc(
				sizeof(char) * 4 + 1);
		if (NULL == cp) {
			return NULL;
		}
		strcpy(cp, "NULL");
	}
	return cp;
}

/************
 * Function to join an array of strings                            *
 * this function allocates memory that must be freed by the caller *
 ************/
char* join_strings(char *strings[], int count) {
	char* str = NULL; /* Pointer to the joined strings  */
	size_t total_length = 0; /* Total length of joined strings */
	int i = 0; /* Loop counter                   */

	/* Find total length of joined strings */
	for (i = 0; i < count; i++) {
		if (strings[i] != NULL) {
			total_length += strlen(strings[i]);
		}
	}
	++total_length; /* For joined string terminator */

	str = (char*) ibmras::monitoring::plugins::j9::jni::hc_alloc(total_length); /* Allocate memory for joined strings */
	if (NULL == str) {
		return NULL;
	}
	str[0] = '\0'; /* Empty string we can append to      */

	/* Append all the strings */
	for (i = 0; i < count; i++) {
		if (strings[i] != NULL) {
			strcat(str, strings[i]);
		}
	}

	return str;
}


}
}
}
}
}/*eo namespaces*/
