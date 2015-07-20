 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef AGENTEXTENSIONRECEIVER_H_
#define AGENTEXTENSIONRECEIVER_H_

#include "ibmras/export.h"
#include "ibmras/monitoring/connector/Receiver.h"
#include "ibmras/monitoring/AgentExtensions.h"

namespace ibmras {
namespace monitoring {

class DECL AgentExtensionReceiver : public ibmras::monitoring::connector::Receiver {
public:
	AgentExtensionReceiver(RECEIVE_MESSAGE cb) : 
			receiveMessageCallback(cb) {
	}
	virtual ~AgentExtensionReceiver() {}
	void receiveMessage(const std::string &id, uint32 size, void *data) {
		if (receiveMessageCallback) {
			receiveMessageCallback(id.c_str(), size, data);
		}
	}
	
private:
	RECEIVE_MESSAGE receiveMessageCallback;
};
	
}
}

#endif /* AGENTEXTENSIONRECEIVER_H_ */