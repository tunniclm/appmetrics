/*
 * OStreamConnector.cpp
 *
 *  Created on: 24 Feb 2014
 *      Author: robbins
 */
#include "ibmras/monitoring/connector/ostream/OStreamConnector.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/common/logging.h"

#if defined(_WINDOWS)
#define OSTREAMCONNECTOR_DECL __declspec(dllexport)	/* required for DLLs to export the plugin functions */
#else
#define OSTREAMCONNECTOR_DECL
#endif

namespace OStream {

IBMRAS_DEFINE_LOGGER("ostream");

OStreamConnector::OStreamConnector(std::ostream &outputStream) :
		output(outputStream) {
}

int OStreamConnector::sendMessage(const std::string &sourceId, uint32 size,
		void *data) {

	char* charData = reinterpret_cast<char*>(data);
	uint32 i;
	output << "----------------------------------------------------------------------------------------------------------\n";
	output << sourceId << "\n";
	output << "----------------------------------------------------------------------------------------------------------\n";
	for (i = 0; i < size; i++) {
		output.put(charData[i]);
	}
	output << "\n----------------------------------------------------------------------------------------------------------\n";
	return i;
}

void OStreamConnector::registerReceiver(ibmras::monitoring::connector::Receiver *receiver) {

}

int OStreamConnector::start() {
	IBMRAS_DEBUG(info, "Starting ostream connector");
	return 0;
}

int OStreamConnector::stop() {
	IBMRAS_DEBUG(info, "Stopping ostream connector");
	return 0;
}

OStreamConnector::~OStreamConnector() {
}

} /* end namespace monitoring */

extern "C" {
OSTREAMCONNECTOR_DECL int ibmras_monitoring_plugin_start() {
	return 0;
}

OSTREAMCONNECTOR_DECL int ibmras_monitoring_plugin_stop() {
	return 0;
}

OSTREAMCONNECTOR_DECL void* ibmras_monitoring_getConnector(const char* properties) {
	return new OStream::OStreamConnector(std::cout);
}
}
