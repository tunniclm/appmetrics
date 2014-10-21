/*
 * MethodLookupProvider.h
 *
 *  Created on: 21 July 2014
 *      Author: stalleyj
 */

#ifndef METHODLOOKUPPROVIDER_H_
#define METHODLOOKUPPROVIDER_H_

#include "ibmras/monitoring/connector/Receiver.h"
#include "ibmras/monitoring/Plugin.h"


namespace plugins {
namespace omr {
namespace methods {



class MethodLookupProvider: public ibmras::monitoring::connector::Receiver, public ibmras::monitoring::Plugin {
public:
	MethodLookupProvider(omrRunTimeProviderParameters functions);
	virtual ~MethodLookupProvider();
	int startReceiver();
	int stopReceiver();
	void receiveMessage(const std::string &id, uint32 size, void *data);
	static pushsource* registerPushSource(void (*callback)(monitordata* data), uint32 provID);
	static uint32 providerID;
	static MethodLookupProvider* getInstance(omrRunTimeProviderParameters oRTPP);
	static void* getInstance();
	void getMethodIDs(std::vector<std::string> &parameters);
	static void sendMethodDictionary();
private:
	omrRunTimeProviderParameters vmData;

	static monitordata* generateData(uint32 sourceID, const char *dataToSend, int size);
};
}
}
}/* namespace monitoring */
#endif /* METHODLOOKUPPROVIDER_H_ */
