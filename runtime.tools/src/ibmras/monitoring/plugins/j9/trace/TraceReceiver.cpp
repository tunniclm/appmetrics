/*
 * TraceReceiver.cpp
 *
 *  Created on: 7 July 2014
 *      Author: stalleyj
 */
#include <iostream>
#include <sstream>
#include "ibmras/monitoring/plugins/j9/trace/TraceReceiver.h"
#include "ibmras/monitoring/plugins/j9/trace/TraceDataProvider.h"
#include "ibmras/monitoring/connector/Receiver.h"
#include "ibmras/monitoring/Plugin.h"
#include "ibmras/common/util/strUtils.h"

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace trace {

int startReceiver() {
	return 0;
}

int stopReceiver() {
	return 0;
}

TraceReceiver::TraceReceiver() {
}

TraceReceiver::~TraceReceiver() {
}

void TraceReceiver::receiveMessage(const std::string &id, uint32 size,
		void *data) {

	std::string message((const char*) data, size);

	if ((id == "trace") || (id == "TRACESubscriberSourceConfiguration")) {
		if (message == "header") {
			ibmras::monitoring::plugins::j9::trace::sendTraceHeader(false);
		} else {
			std::size_t found = message.find(',');
			if (found != std::string::npos) {
				std::string command = message.substr(0, found);
				std::string rest = message.substr(found + 1);
				std::vector < std::string > parameters =
						ibmras::common::util::split(rest, ',');
				for (std::vector<std::string>::iterator it = parameters.begin();
						it != parameters.end(); ++it) {
					ibmras::monitoring::plugins::j9::trace::controlTracePoints(
							command, *it);
				}
			}
		}
	}
}

}
}
}
}
} /* end namespace agent */
