/*
 * TraceReceiver.h
 *
 *  Created on: 7 July 2014
 *      Author: stalleyj
 */

#ifndef ibmras_monitoring_plugins_j9_trace_tracereceiver_h
#define ibmras_monitoring_plugins_j9_trace_tracereceiver_h

#include "ibmras/monitoring/connector/Receiver.h"
#include "ibmras/monitoring/Plugin.h"

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace trace {

class TraceReceiver: public ibmras::monitoring::connector::Receiver, ibmras::monitoring::Plugin {
public:
	TraceReceiver();
	virtual ~TraceReceiver();
	int startReceiver();
	int stopReceiver();
	void receiveMessage(const std::string &id, uint32 size, void *data);
};

}
}
}
}
}/* namespace trace */
#endif /* ibmras_monitoring_plugins_j9_trace_tracereceiver_h */
