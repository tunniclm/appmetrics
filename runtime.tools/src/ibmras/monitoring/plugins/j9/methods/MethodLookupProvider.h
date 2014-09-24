/*
 * MethodLookupProvider.h
 *
 *  Created on: 21 July 2014
 *      Author: stalleyj
 */

#ifndef ibmras_monitoring_plugins_j9_methods
#define ibmras_monitoring_plugins_j9_methods

#include "ibmras/monitoring/connector/Receiver.h"
#include "ibmras/monitoring/Plugin.h"

namespace ibmras{
namespace monitoring {
namespace plugins {
namespace j9 {
namespace methods {



class MethodLookupProvider: public ibmras::monitoring::connector::Receiver, public ibmras::monitoring::Plugin {
public:
	MethodLookupProvider(jvmFunctions functions);
	virtual ~MethodLookupProvider();
	int startReceiver();
	int stopReceiver();
	void receiveMessage(const std::string &id, uint32 size, void *data);
	static pushsource* registerPushSource(void (*callback)(monitordata* data), uint32 provID);
	static uint32 providerID;
	static MethodLookupProvider* getInstance(jvmFunctions functions);
	static void* getInstance();
	void getMethodIDs(std::vector<std::string> &parameters);
	static void sendMethodDictionary();
private:
	jvmFunctions vmFunctions;

	static monitordata* generateData(uint32 sourceID, const char *dataToSend, int size);
};
}
}
}
}
}/* namespace monitoring */
#endif /* ibmras_monitoring_plugins_j9_methods */
