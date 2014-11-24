 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef SYSTEMRECEIVER_H_
#define SYSTEMRECEIVER_H_

#include "ibmras/monitoring/connector/Receiver.h"
#include "ibmras/monitoring/Plugin.h"

extern "C" DECL void* ibmras_getSystemReceiver();

namespace ibmras{
namespace monitoring {
namespace agent {

class SystemReceiver: public ibmras::monitoring::connector::Receiver, public ibmras::monitoring::Plugin {
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
