/*
 * ClassHistogrmProvider.h
 *
 */

#ifndef ibmras_monitoring_plugins_j9_classHistogram
#define ibmras_monitoring_plugins_j9_classHistogram

#include "ibmras/monitoring/connector/Receiver.h"
#include "ibmras/monitoring/Plugin.h"


namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace classhistogram {


class ClassHistogramProvider: public ibmras::monitoring::connector::Receiver, public ibmras::monitoring::Plugin {
public:
	ClassHistogramProvider(jvmFunctions functions);
	virtual ~ClassHistogramProvider();
	int startReceiver();
	int stopReceiver();
	void receiveMessage(const std::string &id, uint32 size, void *data);
	static pushsource* registerPushSource(void (*callback)(monitordata* data), uint32 provID);
	static uint32 providerID;
	static ClassHistogramProvider* getInstance(jvmFunctions functions);
	static void* getInstance();
private:
	jvmFunctions vmFunctions;

	static monitordata* generateData(uint32 sourceID, const char *dataToSend, int size);
	std::string createHistogramReport();
	unsigned char* hc_alloc(int size);
	void hc_dealloc(unsigned char** buffer);
	//void force2Native(char * str);
	//static jvmtiIterationControl JNICALL updateClassTotals(jlong class_tag, jlong size, jlong* tag_ptr, void* user_data);
};
}
}
}
}
}/* namespace monitoring */
#endif /* ibmras_monitoring_plugins_j9_classHistogram */
