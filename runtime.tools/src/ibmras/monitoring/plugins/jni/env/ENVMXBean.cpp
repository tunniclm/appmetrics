#include "ibmras/monitoring/plugins/jni/env/ENVMXBean.h"
#include <cstring>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include "ibmras/common/util/strUtils.h"
#include "ibmras/common/logging.h"
#include "ibmras/monitoring/agent/Agent.h"

using namespace ibmras::monitoring::plugins::jni;

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jni {
namespace env {

IBMRAS_DEFINE_LOGGER("DataProviderSources");

std::string queryVmDump(jvmtiExtensionFunction jvmtiQueryVmDump, jvmtiEnv* pti);
const std::string reportDumpOptions(jvmFunctions* tdpp);

ENVPullSource* src = NULL;

PullSource* getENVPullSource(){
	if(!src) {
		src = new ENVPullSource;
	}
	return src;
}

monitordata* callback() {
	return src->PullSource::generateData();
}

uint32 ENVPullSource::getSourceID() {
	return ENV;
}

pullsource* ENVPullSource::getDescriptor() {

	pullsource* src= new pullsource();
	src->header.name = "environment";
	src->header.description = "Environment information";
	src->header.sourceID = ENV;
	src->header.capacity = 32 * 1024;
	src->header.config = "environment_subsystem=on";
	src->next = NULL;
	src->callback = callback;
	src->complete = ibmras::monitoring::plugins::jni::complete;
	src->pullInterval = 1200;

	return src;
}

monitordata* ENVPullSource::sourceData(jvmFunctions* tdpp, JNIEnv* env) {

	IBMRAS_DEBUG(debug, "Generating Environment data");
	monitordata* data = new monitordata;
	data->persistent = false;
	data->provID = getProvID();
	data->sourceID = ENV;

	std::string cp = getString(env, "com/ibm/java/diagnostics/healthcenter/agent/dataproviders/environment/EnvironmentDataProvider", "getJMXData", "()Ljava/lang/String;");
	std::stringstream ss;
	ss << cp;
	ss<<"\n";


	ss << "native.library.date=" << ibmras::monitoring::agent::Agent::getBuildDate() << "\n";
	ss << "pid=" << ibmras::common::port::getProcessId() << "\n";

	const std::string vmd = reportDumpOptions(tdpp);
	ss<<vmd;

	std::string envdata = ss.str();
	jsize len = envdata.length();

	char* sval = reinterpret_cast<char*>(hc_alloc(len+1));
	if(sval) {
		strcpy(sval,envdata.c_str());
		IBMRAS_DEBUG_1(debug, "Showing first 100 characters of environment: %.100s",sval);
		data->size = len;
		data->data = sval;
	} else {
		data->size = 0;
		data->data = NULL;
	}
	return data;

}

/**
 	Returns VM Dump options
 	@param jvmtiQueryVmDump Function pointer to the jvmti function naming the parameter
 	@param pti Pointer to jvmti environment.
 	@return A char* which contains the options OR an empty string if there is an error.
 */

std::string queryVmDump(jvmtiExtensionFunction jvmtiQueryVmDump, jvmtiEnv* pti)
{
	IBMRAS_DEBUG(debug, "> queryVmDump");
    jvmtiError  rc;
    char *buffer = NULL;
    jint buffer_size;
    std::string options;

    if (jvmtiQueryVmDump == 0)
    {
    	IBMRAS_DEBUG(debug, "No jvmtiQueryVmDump extension function");
        return "";
    }


    buffer_size = 1024;
    rc = pti->Allocate(buffer_size, (unsigned char**)&buffer);
    if (buffer == NULL)
    {
    	IBMRAS_DEBUG(debug, "< queryVmDump failed to allocate buffer");
        return "";
    }

    /* call jvmti function */
    rc = (jvmtiQueryVmDump)(pti, buffer_size, buffer, &buffer_size);
    if (rc == JVMTI_ERROR_ILLEGAL_ARGUMENT)
    {
    	IBMRAS_DEBUG(debug, "Buffer smaller than expected");
        /* allocate buffer of correct size */
    	pti->Deallocate((unsigned char*)buffer);
        pti->Allocate(buffer_size+100, (unsigned char**)&buffer);
        if (buffer == NULL)
        {
        	IBMRAS_DEBUG(debug, "< queryVmDump failed to reallocate buffer");
            return "";
        }
        rc = (jvmtiQueryVmDump)(pti, buffer_size, buffer, &buffer_size);
    }

    if (rc == JVMTI_ERROR_NONE)
    {
    	options = buffer;

    } else
    {
    	IBMRAS_DEBUG(debug, "RC_ERROR_2");
    }

    pti->Deallocate((unsigned char*)buffer);

    IBMRAS_DEBUG(debug, "< queryVmDump");
    return options;
}


/**
 	 Formats the string coming from queryVmDump so it can be recognised by the parser on the client side
 	 when appended to the environment data returned from the jni call.
 	 @param tdpp jvmFunctions structure to allow access to jvmtiQueryVmDump function pointer.
 	 @return A formatted string starting with a tag recognisable by the client and the options separated by
 	 the @@@ marker, as it was originally done when generating the environment data in Java
 */
const std::string reportDumpOptions(jvmFunctions* tdpp) {

	const::std::string options = queryVmDump(tdpp->jvmtiQueryVmDump, tdpp->pti);
	std::vector<std::string> sv = ibmras::common::util::split(options, '\n');
	std::stringstream ss;

	ss << "dump.options=";
	int i = 0;
	for(std::vector<std::string>::iterator it = sv.begin(); it != sv.end(); ++it){
		if(i != 0){
			ss << "@@@";
		}
		ss << *it;
		i++;
	}
	ss << '\n';

	return ss.str();
}


}	/* end namespace env */
}	/* end namespace jni */
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */
