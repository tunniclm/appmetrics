 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_monitoring_connector_receiver_h
#define ibmras_monitoring_connector_receiver_h

#include <string>
#include <ibmras/common/types.h>

namespace ibmras{
namespace monitoring {
namespace connector {

class Receiver {
public:
	virtual void receiveMessage(const std::string &id, uint32 size,
			void *data) = 0;

	virtual ~Receiver() {
	}

protected:
	Receiver() {
	}
};

}
}
} /* namespace connector */
#endif /* ibmras_monitoring_connector_receiver_h */
