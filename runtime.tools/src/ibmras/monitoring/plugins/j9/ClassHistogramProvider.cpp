/*
 * MethodLookupProvider.cpp
 *
 *  Created on: 21 July 2014
 *      Author: stalleyj
 */

#include "ibmras/monitoring/Plugin.h"
#include "ibmras/vm/java/healthcenter.h"
#include "ibmras/monitoring/plugins/j9/ClassHistogramProvider.h"
#include "ibmjvmti.h"
#include "jni.h"
#include "jvmti.h"
#include "ibmras/common/util/strUtils.h"
#include "ibmras/common/logging.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <ctime>

#if defined(WINDOWS)
#include <windows.h>
#include <Psapi.h>
#include <WinBase.h>
#endif

#if defined(LINUX)
#include <sys/time.h>
#endif

#if defined(_ZOS)
#include <sys/time.h>
#endif

#if defined (WINDOWS)
    #define JLONG_FMT_STR "%I64d"
#else
    #define JLONG_FMT_STR "%lld"
#endif

jlong * classSizes;
jlong * classCounts;

static jvmtiIterationControl JNICALL updateClassTotals(jlong class_tag, jlong size, jlong* tag_ptr, void* user_data)
{
    jint *totalHeapUsedValue;
    totalHeapUsedValue = (jint *)user_data;
    (*totalHeapUsedValue)+=size;
    classSizes[class_tag]+=size;
    classCounts[class_tag] ++;
    return JVMTI_ITERATION_CONTINUE;
}



namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace classhistogram {

PUSH_CALLBACK sendClassHistogramData;
uint32 ClassHistogramProvider::providerID = 0;
IBMRAS_DEFINE_LOGGER("ClassHistogram");

int startReceiver() {
	return 0;
}

int stopReceiver() {
	return 0;
}

pushsource* ClassHistogramProvider::registerPushSource(
		void (*callback)(monitordata* data), uint32 provID) {
	pushsource *src = new pushsource();
	src->header.name = "classhistogram";
	src->header.description =
			"Provides the class histogram data when requested by the client";
	src->header.sourceID = 0;
	src->next = NULL;
	src->header.capacity = 1048576; /* 1MB bucket capacity */
	ClassHistogramProvider::providerID = provID;
	ibmras::monitoring::plugins::j9::classhistogram::sendClassHistogramData = callback;

	return src;
}

ClassHistogramProvider::ClassHistogramProvider(jvmFunctions functions) {
	vmFunctions = functions;
	name = "ClassHistogramProvider";
	pull = NULL;
	push = registerPushSource;
	start = plugins::j9::classhistogram::startReceiver;
	stop = plugins::j9::classhistogram::stopReceiver;
	type = ibmras::monitoring::plugin::data
			| ibmras::monitoring::plugin::receiver;
	recvfactory = (RECEIVER_FACTORY) ClassHistogramProvider::getInstance;
	confactory = NULL;
}

ClassHistogramProvider::~ClassHistogramProvider() {
}

ClassHistogramProvider* instance = NULL;

ClassHistogramProvider* ClassHistogramProvider::getInstance(
		jvmFunctions functions) {
	if (!instance) {
		instance = new ClassHistogramProvider(functions);
	}
	return instance;
}

void* ClassHistogramProvider::getInstance() {
	if (!instance) {
		return NULL;
	}
	return instance;
}

void ClassHistogramProvider::receiveMessage(const std::string &id, uint32 size,
		void *data) {
	// Send the initial empty dictionary
	if (id == "ClassHistogramSource") {

		std::string data = createHistogramReport();
		monitordata *mdata = generateData(0, data.c_str(),
				data.length());
		sendClassHistogramData(mdata);
	}
}


monitordata* ClassHistogramProvider::generateData(uint32 sourceID,
		const char *dataToSend, int size) {
	monitordata* data = new monitordata;
	data->provID = ClassHistogramProvider::providerID;
	data->data = dataToSend;
	if (data->data == NULL) {
		data->size = 0;
	} else {
		data->size = strlen(data->data);
	}
	data->sourceID = sourceID;
	data->persistent = false;
	return data;
}


std::string ClassHistogramProvider::createHistogramReport()
{
    char buffer[1000];
    std::stringstream report;
    int heapUsed=0;
    int rc;
    jclass *classes = NULL;
    jint count;
    int i;
    char* repfmt = "@@chd@@,%s,"JLONG_FMT_STR","JLONG_FMT_STR"\n";
    char** classNameArray = NULL;
    char ** classHistLineArray = NULL;

	JNIEnv * env;
	vmFunctions.theVM->AttachCurrentThread((void **) &env, NULL);

    /* Get all the loaded classes */
    rc = vmFunctions.pti->GetLoadedClasses(&count, &classes);
    if ( rc != JVMTI_ERROR_NONE )
    {
    	IBMRAS_DEBUG_1(debug, "cannot GetLoadedClasses: rc = %d", rc);
        goto cleanup;
    }

    classNameArray = (char**)hc_alloc((count) * sizeof(char*));
    if(NULL == classNameArray) {
    	goto cleanup;
    }

    classSizes = (jlong*)hc_alloc(count * sizeof(jlong));
    if(NULL == classSizes) {
    	goto cleanup;
    }

    classCounts = (jlong*)hc_alloc(count * sizeof(jlong));
    if(NULL == classCounts) {
    	goto cleanup;
    }

    classHistLineArray = (char**)hc_alloc((count+1) * sizeof(char*));
    if(NULL == classHistLineArray) {
    	goto cleanup;
    }

    for ( i = 0 ; i < count ; i++ )
    {
        char* signature = NULL;
        char* generic = NULL;

        rc = vmFunctions.pti->SetTag(classes[i], i);
        if ( rc != JVMTI_ERROR_NONE )
        {
        	IBMRAS_DEBUG_1(debug, "cannot setTags on classes: rc = %d", rc);
            goto cleanup;
        }

        rc = vmFunctions.pti->GetClassSignature(classes[i], &signature, &generic);
        if ( rc != JVMTI_ERROR_NONE )
        {
        	IBMRAS_DEBUG_1(debug, "cannot get class signatures: rc = %d", rc);
            goto cleanup;
        }

        classNameArray[i] = (char *)hc_alloc(strlen(signature)+1);
    	if(NULL == classNameArray[i]) {
    		goto cleanup;
	    }

        strcpy(classNameArray[i],signature);
        //force2Native(classNameArray[i]);

        if (signature)
        {
            hc_dealloc((unsigned char**)&signature);
        }
        if (generic)
        {
            hc_dealloc((unsigned char**)&generic);
        }
    }

    /* This returns the JVMTI_ERROR_UNATTACHED_THREAD */
    rc = vmFunctions.pti->IterateOverHeap(JVMTI_HEAP_OBJECT_EITHER,&updateClassTotals,&heapUsed);

    if (rc != JVMTI_ERROR_NONE)
    {
    	IBMRAS_DEBUG_1(debug, "problem iterating over heap, error %d\n",rc);
        goto cleanup;
    }

    sprintf(buffer,"heapused,%d\n", heapUsed);
    report << buffer;
    for (i=0; i < count; i++)
    {
        sprintf(buffer,repfmt,classNameArray[i], classSizes[i], classCounts[i]);
        report << buffer;
    }

	unsigned long long millisecondsSinceEpoch;
#if defined(WINDOWS)

		SYSTEMTIME st;
		GetSystemTime(&st);

		millisecondsSinceEpoch = time(NULL)*1000+st.wMilliseconds;

#else
		struct timeval tv;
		gettimeofday(&tv, NULL);

		millisecondsSinceEpoch = (unsigned long long) (tv.tv_sec) * 1000
				+ (unsigned long long) (tv.tv_usec) / 1000;
#endif


cleanup:
	vmFunctions.theVM->DetachCurrentThread();
    hc_dealloc((unsigned char**)&classes);
    hc_dealloc((unsigned char**)&classCounts);
    hc_dealloc((unsigned char**)&classSizes);
    hc_dealloc((unsigned char**)&classNameArray);
    hc_dealloc((unsigned char**)&classHistLineArray);

    std::stringstream reportdata;

    reportdata << "classhistogramreport," << "reportTime," << millisecondsSinceEpoch << "\n" << report.str();

    return reportdata.str();
}


unsigned char* ClassHistogramProvider::hc_alloc(int size)
{
    jvmtiError rc;
    unsigned char* buffer = NULL;

    rc = vmFunctions.pti->Allocate(size, (unsigned char**)&buffer);
    if (rc != JVMTI_ERROR_NONE)
    {
    	IBMRAS_DEBUG_1(debug, "OutOfMem : hc_alloc failed to allocate %d bytes.", size);
        return NULL ;
    } else
    {
//    	IBMRAS_DEBUG_2(fine, "hc_alloc: allocated %d bytes at %p", size, buffer);
        memset(buffer, 0, size);
        return buffer;
    }
}

/********************************************************************************/
/* Our own function to perform dealloction of memory via jvmti and check return */

void ClassHistogramProvider::hc_dealloc(unsigned char** buffer)
{
    jvmtiError rc;

    if (*buffer == NULL)
    {
    	IBMRAS_DEBUG(fine, "hc_dealloc called with null pointer");
        return;
    }
    rc = vmFunctions.pti->Deallocate(*buffer);
    if (rc != JVMTI_ERROR_NONE)
    {
        IBMRAS_DEBUG_1(debug, "hc_dealloc failed to deallocate. rc=%d", rc);
    } else
    {
        *buffer = NULL;
    }
}

/******************************/
//void
//ClassHistogramProvider::force2Native(char * str)
//{
//    char *p = str;
//
//    if ( NULL != str )
//    {
//        while ( 0 != *p )
//        {
//            if ( 0 != ( 0x80 & *p ) )
//            {
//                p = NULL;
//                break;
//            }
//            p++;
//        }
//#ifdef _ZOS
//        if ( NULL != p )
//        {
//            __atoe(str);
//        }
//#endif
//    }
//}


}
}
}
}
} /* end namespace methods */








