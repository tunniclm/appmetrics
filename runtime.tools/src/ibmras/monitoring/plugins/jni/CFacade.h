 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#ifndef ibmras_monitoring_plugins_jni_cfacade_h
#define ibmras_monitoring_plugins_jni_cfacade_h

#include <jni.h>
#include <jvmti.h>
#include "ibmras/common/port/ThreadData.h"
#include "ibmras/monitoring/Plugin.h"
#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/monitoring/plugins/jni/Facade.h"
#include "ibmras/vm/java/healthcenter.h"
#include "ibmras/common/util/memUtils.h"
#include "ibmras/common/Logger.h"

#include <cstdio>
#include <string>

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jni {

enum PullSourceIDs {
	ENV, TD, MEM, MC, JLA, PULL_COUNT
};


extern void complete(monitordata* data);

extern uint64_t tm_stt;
#define JVMPI_MONITOR_JAVA              0x01
#define JVMPI_MONITOR_RAW               0x02
#define PITRACE_ERROR					-1

std::string getString(JNIEnv* env, const char* cname, const char* mname, const char* signature);


/*
 * This is the pure virtual class that specific pull sources need to inherit from.
 * When the agent calls back for data, you need to attach the current agent thread to
 * the JVM using the JNI->attachThread call otherwise you cannot see the running Java code.
 * This is why all agent calls come into the superclass generateData() method, where this
 * attachment is performed before calling the subclass specific sourceData(JNIEnv*) method.
 *
 */

class PullSource {
public:
	PullSource(uint32 provID, const std::string& providerName);
	virtual ~PullSource();										/* desctructor for clean up operations */

	virtual pullsource* getDescriptor() {return NULL;};			/* descriptor for this pull source */
	uint32 getProvID() {return provID; }						/* JMX provider ID */
	virtual uint32 getSourceID() = 0;							/* source ID, overridden by the subclass, and corresponds to the enum entries */
	monitordata* generateData();								/* where the agent will call into */
	void pullComplete(monitordata* mdata);
	monitordata* generateError(char* msg);						/* Wrap an error message to send back */
	virtual void publishConfig() = 0;							/* sub classes must implement to publish their configuration details */
protected:
	virtual monitordata* sourceData(jvmFunctions* tdpp, JNIEnv* env) = 0;		/* sub classes must implement this to get the right JNI env to use to retrieve data */
private:
	uint32 provID;					/* provider ID assigned by the agent */
	JNIEnv* env;
	std::string name;

};

/*
 * The SourceManager provides the registration point for the agent.
 * It also controls stopping and starting any configured pull sources.
 */

class SourceManager {
public:
	SourceManager();
	int start();
	int stop();
	pullsource* registerPullSource(uint32 provID);
	uint32 getProvID();
private:
	uint32 provid;
	bool running;
	PullSource** pullsources;
};
/***
 * UTILITY FUNCTIONS
 */

//MEMORY ALLOCATION FUNCTIONS
unsigned char* hc_alloc(int size);
void hc_dealloc(unsigned char** buffer);
void* hc_realloc_ptr_array(char** source[], int currentSize, int newSize);
void dealloc_report_lines(char *lines[], int count);
//----------------------------
//STRING MANIPULATION
char* dupJavaStr(const char * jnm);
void force2Native(const char * str);
char* join_strings(char *strings[], int count);
void native2Java(char * str);
//----------------------------
//DECL jvmFunctions getTDPP();
int ExceptionCheck(JNIEnv *env);
unsigned int dump_read_u4(char * dp);
void dump_read(void * dp, void * res, int size);
UINT64 read_cycles_on_processor(int cpu);
uint64_t readCyclesOnProcessor(int cpu);
uint64_t getPlatformCycles(void);
void getCycles(uint64_t * t);
#if defined(_ZOS)
extern "C" {
#endif
int qcmp_jlm(const void * p1, const void * p2);
#if defined(_ZOS)
}
#endif


struct __jdata
{
    double miss;
    double util;
    double averht;
    int gets;
    int rec;
    int nrec;
    int fast;
    int slow;
    int tier2;
    int tier3;
    char * mnm;
    unsigned int htlo;
    unsigned int hthi;
    double dhtm;
};

union rdval
{
    struct
    {
#if defined __BIG_ENDIAN
        uint32_t hval;
        uint32_t lval;
#else  /* kernel code, little endian */
#if defined __LITTLE_ENDIAN
        uint32_t lval;
        uint32_t hval;
#else
#error "Undetermined endian!"
#endif
#endif

    } sval;
    uint64_t cval;
};


typedef struct __jdata jdata_t;




}//eon
}
}
}//eon

#endif /* ibmras_monitoring_plugins_jni_cfacade_h */
