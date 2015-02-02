 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include "ibmras/monitoring/plugins/j9/jni/env/ENVMXBean.h"
#include <cstring>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include "ibmras/common/util/strUtils.h"
#include "ibmras/common/logging.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/monitoring/plugins/j9/Util.h"
#include "ibmras/common/port/Process.h"
#include "ibmras/common/MemoryManager.h"

using namespace ibmras::monitoring::plugins::j9::jni;

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace jni {
namespace env {

IBMRAS_DEFINE_LOGGER("DataProviderSources");


const std::string reportDumpOptions(jvmFunctions* tdpp);

ENVPullSource* src = NULL;

PullSource* getENVPullSource(uint32 id){
	if(!src) {
		src = new ENVPullSource(id);
	}
	return src;
}

monitordata* callback() {
	return src->generateData();
}

 void complete(monitordata *mdata) {
	src->pullComplete(mdata);
}

ENVPullSource::ENVPullSource(uint32 id) : PullSource(id, "Health Center (environment)"){
}

void ENVPullSource::publishConfig() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();

	ibmras::monitoring::connector::ConnectorManager *conMan =
			agent->getConnectionManager();

	std::string msg = "environment_subsystem=on\ncapability.dump.heap=on\ncapability.dump.system=on\ncapability.dump.java=on";
	conMan->sendMessage("configuration/environment", msg.length(),
			(void*) msg.c_str());
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
	src->next = NULL;
	src->callback = callback;
	src->complete = complete;
	src->pullInterval = 1200;

	return src;
}

monitordata* ENVPullSource::sourceData(jvmFunctions* tdpp, JNIEnv* env) {

	IBMRAS_DEBUG(debug, "Generating Environment data");
	monitordata* data = new monitordata;
	data->persistent = false;
	data->provID = getProvID();
	data->sourceID = ENV;
#if defined(_ZOS)
#pragma convert("ISO8859-1")
#endif
	std::string cp = getString(env, "com/ibm/java/diagnostics/healthcenter/agent/dataproviders/environment/EnvironmentDataProvider", "getJMXData", "()Ljava/lang/String;");
#if defined(_ZOS)
#pragma convert(pop)
	char* envString = ibmras::common::util::createNativeString(cp.c_str());
#else
	const char* envString = cp.c_str();
#endif

	std::stringstream ss;

	if (envString) {
		ss << envString;
		ss<<"\n";
	}
#if defined(_ZOS)
	ibmras::common::memory::deallocate((unsigned char**)&envString);
#endif

	ss << "native.library.date=" << ibmras::monitoring::agent::Agent::getBuildDate() << "\n";
	ss << "pid=" << ibmras::common::port::getProcessId() << "\n";

	const std::string vmd = reportDumpOptions(tdpp);
	ss<<vmd;

	std::string envdata = ss.str();

	char* asciiEnv = ibmras::common::util::createAsciiString(envdata.c_str());

	if(asciiEnv) {
		IBMRAS_DEBUG_1(debug, "Showing first 100 characters of environment: %.100s",envdata.c_str());
		data->size = strlen(asciiEnv);
		data->data = asciiEnv;
	} else {
		data->size = 0;
		data->data = NULL;
	}
	return data;

}


/**
 	 Formats the string coming from queryVmDump so it can be recognised by the parser on the client side
 	 when appended to the environment data returned from the jni call.
 	 @param tdpp jvmFunctions structure to allow access to jvmtiQueryVmDump function pointer.
 	 @return A formatted string starting with a tag recognisable by the client and the options separated by
 	 the @@@ marker, as it was originally done when generating the environment data in Java
 */
const std::string reportDumpOptions(jvmFunctions* tdpp) {

	const::std::string options = ibmras::monitoring::plugins::j9::Util::queryVmDump(tdpp->jvmtiQueryVmDump, tdpp->pti);
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
}	/* end namespace j9 */
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */