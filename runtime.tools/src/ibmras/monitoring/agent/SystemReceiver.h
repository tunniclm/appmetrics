/*
 * SystemReceiver.h
 *
 *  Created on: 10 June 2014
 *      Author: stalleyj
 */

#ifndef SYSTEMRECEIVER_H_
#define SYSTEMRECEIVER_H_

#include "ibmras/monitoring/connector/Receiver.h"
#include "ibmras/monitoring/Plugin.h"

extern "C" DECL void* ibmras_getSystemReceiver();

namespace ibmras{
namespace monitoring {
namespace agent {

class SystemReceiver: public ibmras::monitoring::connector::Receiver, ibmras::monitoring::Plugin {
public:
	SystemReceiver();
	virtual ~SystemReceiver();
	int startReceiver();
	int stopReceiver();
	void receiveMessage(const std::string &id, uint32 size, void *data);
};
}
}
} /* namespace monitoring */
#endif /* SYSTEMRECEIVER_H_ */
