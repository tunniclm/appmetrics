/*
 * Receiver.h
 *
 *  Created on: 27 Mar 2014
 *      Author: robbins
 */

#ifndef ibmras_monitoring_connector_receiver_h
#define ibmras_monitoring_connector_receiver_h

#include "ibmras/common/port/ThreadData.h"

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
