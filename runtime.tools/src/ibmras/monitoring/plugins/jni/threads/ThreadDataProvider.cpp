#include "ibmras/monitoring/plugins/jni/threads/ThreadDataProvider.h"
#include <cstring>
#include "ibmras/common/util/memUtils.h"
#include <iostream>

using namespace ibmras::monitoring::plugins::jni;

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jni {
namespace threads {

 TDPullSource* src = NULL;

 PullSource* getTDPullSource() {
		if(!src) {
			src = new TDPullSource;
		}
		return src;

 }

 monitordata* callback() {
 	return src->PullSource::generateData();
 }

 uint32 TDPullSource::getSourceID() {
 	return TD;
 }

 pullsource* TDPullSource::getDescriptor() {

 	pullsource* src= new pullsource();
 	src->header.name = "threads";
 	src->header.description = "Thread information";
 	src->header.sourceID = TD;
 	src->header.capacity = 32 * 1024;
 	src->header.config = "threads_subsystem=on";
 	src->next = NULL;
 	src->callback = callback;
 	src->complete = ibmras::monitoring::plugins::jni::complete;
 	src->pullInterval = 30;

 	return src;
 }

 monitordata* TDPullSource::sourceData(jvmFunctions* tdpp, JNIEnv* env){
	 monitordata* data = new monitordata;
	 data->persistent = false;
	 data->provID = getProvID();
	 data->sourceID = TD;

	 std::string cp = getString(env,"runtime/tools/java/dataproviders/threads/ThreadDataProviderJNI", "getJMXData", "()Ljava/lang/String;");
	 int len = cp.length();
	 char* sval = reinterpret_cast<char*>(hc_alloc(len+1));
	 if(sval) {
		 strcpy(sval, cp.c_str());
	 }
	 if(sval){
		data->size = len;
		data->data = sval;
		return data;
	 }

	 data->size = 0;
	 data->data = NULL;
	 return data;

 }

} /* End of namespace threads */
} /* End of namespace jni */
} /* End of namespace plugins */
} /* End of namespace monitoring */
} /* End of namespace ibmras */

