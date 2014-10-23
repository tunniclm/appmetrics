#include "ibmras/monitoring/plugins/jni/env/ENVMXBean.h"
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

using namespace ibmras::monitoring::plugins::jni;

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jni {
namespace env {

IBMRAS_DEFINE_LOGGER("DataProviderSources");


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

void ENVPullSource::publishConfig() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();

	ibmras::monitoring::connector::ConnectorManager *conMan =
			agent->getConnectionManager();

	std::string msg = "environment_subsystem=on";
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
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */
