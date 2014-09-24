#include "ibmras/common/port/ThreadData.h"
#include "ibmras/monitoring/Monitoring.h"
#include <string.h>
#include "ibmras/common/logging.h"
#include "ibmras/vm/omr/healthcenter.h"
#include "ibmras/monitoring/plugins/omr/CpuDataProvider.h"
#include "omragent.h"
#include "stdlib.h"
#include <sys/time.h>


#define DEFAULT_CAPACITY 1024	//Capacity of the bucket that will host the pushsource

namespace plugins {
namespace omr {
namespace cpu {

IBMRAS_DEFINE_LOGGER("Cpu");

ibmras::common::port::Lock* lock = new ibmras::common::port::Lock;

uint32 localprovid = 0;
uint32 srcid = 0;

omrRunTimeProviderParameters vmData;

monitordata* CpuDataProvider::generateData(uint32 srcid) {
	IBMRAS_DEBUG(debug, "generateData start\n");
	monitordata* data = new monitordata;
	data->persistent = false;
	data->provID = plugins::omr::cpu::localprovid;

	data->data = getCpuData();
	if (data->data == NULL) {
		data->size = 0;
	} else {
		data->size = strlen(data->data);
	}

	data->sourceID = plugins::omr::cpu::srcid;
	return data;
}

monitordata* CpuDataProvider::pullCallback() {
	/**
	 * This method is called by the agent to get the information from the pullsource,
	 * in this case the mock data is produced from the "generateData" method.
	 */
	IBMRAS_DEBUG(debug, "pullCallback start\n");
	plugins::omr::cpu::lock->acquire();
	IBMRAS_DEBUG(debug, "Generating data for pull from agent");
	monitordata* data = generateData(srcid);
	plugins::omr::cpu::lock->release();
	return data;
}



void CpuDataProvider::pullComplete(monitordata* data) {
	/**
	 * This method is a callback "destructor" for the resources acquired by the class
	 */
	if (data->data != NULL) {
		delete data->data;
	}
	delete data;
}




pullsource* CpuDataProvider::registerPullSource(uint32 provID) {

	IBMRAS_DEBUG(info, "Registering pull sources");
	pullsource *src = new pullsource();
	src->header.name = "cpu";
	src->header.description = ("This returns the CPU data");
	src->header.sourceID = srcid;
	src->header.capacity = (DEFAULT_CAPACITY);
	src->next = NULL;
	src->callback = pullCallback;
	src->complete = pullComplete;
	src->pullInterval = 2;	/* space pull intervals apart for successive calls */
	localprovid = provID;
	IBMRAS_DEBUG(debug, "registerPullSource end\n");
	return src;
}

CpuDataProvider::CpuDataProvider(
		omrRunTimeProviderParameters oRTPP) {
	IBMRAS_DEBUG(debug, "CpuDataProvider constructor\n");
	vmData = oRTPP;
	name = "cpu";
	pull = registerPullSource;
	start = cpustart;
	stop = cpustop;
	push = NULL;
	handle = NULL;
	type = ibmras::monitoring::plugin::data;
	confactory = NULL;
	recvfactory = NULL;
}


int CpuDataProvider::cpustart() {

	/**
	 * This method is exposed and will be called by the agent when starting all the plugins, anything
	 * required to start the plugin has to be added here. In this case, there is just a pullsource, it
	 * does not require any kind of initialization.
	 */
	return 0;
}

int CpuDataProvider::cpustop() {
	/**
	 * The stop method will be called by the agent on shutdown, here is where any cleanup has to be done
	 */
	return 0;

}

/* ====================================== */
/*  getCpuData functions                  */
/* ====================================== */


char* CpuDataProvider::getCpuData()
{
    int err;
    double processCpuLoad = 0;
    double systemCpuLoad = 0;
    char * report;
	report = new char[100];
	int rc;
	OMR_VMThread thrSlot;
	OMR_VMThread *vmThread = &thrSlot;
	// format is startcpu@#timestamp@#processcpu@#systemcpu
	char* cpuFormatString = "startCPU@#%llu@#%f@#%f\n";
	unsigned long long millisecondsSinceEpoch;

    IBMRAS_DEBUG(debug, "getCpuData start\n");

    err = vmData.omrti->BindCurrentThread(vmData.theVm, "getCpuData", &vmThread);

	if (OMR_ERROR_NONE != err) {
		IBMRAS_DEBUG(debug, "getCpuData exit as unable to bindCurrentThread");
		return NULL;
	}

    /* Get the process cpu load */
	rc = vmData.omrti->GetProcessCpuLoad(vmThread, &processCpuLoad);
    if (OMR_ERROR_NONE != rc)
    {
    	IBMRAS_DEBUG_1(debug, "Problem calling GetProcessCpuLoad: %d", rc);
       	goto cleanup;
    }
    /* Get the system cpu load */
	rc = vmData.omrti->GetSystemCpuLoad(vmThread, &systemCpuLoad);
	if (OMR_ERROR_NONE != rc)
	{
		IBMRAS_DEBUG_1(debug, "Problem calling GetSystemCpuLoad: %d", rc);
		goto cleanup;
	}


#if defined(WINDOWS)
	// work out how to get windows time
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);

	millisecondsSinceEpoch =
	    (unsigned long long)(tv.tv_sec) * 1000 +
	    (unsigned long long)(tv.tv_usec) / 1000;
#endif

	sprintf(report,cpuFormatString,millisecondsSinceEpoch,processCpuLoad,systemCpuLoad);
	IBMRAS_DEBUG_1(debug, "%s", report);

    cleanup:
    IBMRAS_DEBUG(debug, "in cleanup block\n");
	if (OMR_ERROR_NONE == err) {
		// TODO - add this call back in when omr have fixed the unbind problem
//    	err = vmData.omrti->UnbindCurrentThread(vmThread);
	}

	if (OMR_ERROR_NONE != err) {
		IBMRAS_DEBUG(debug, "getCpuData exit as unable to unbindCurrentThread");
		return NULL;
	}

	if (OMR_ERROR_NONE != rc) {
		IBMRAS_DEBUG(debug, "getCpuData exit as unable to get all the data");
		return NULL;
	}

	IBMRAS_DEBUG(debug, "getCpuData exit");
    return report;
}

}
}
}/*end of namespace plugins::omr::cpu*/

