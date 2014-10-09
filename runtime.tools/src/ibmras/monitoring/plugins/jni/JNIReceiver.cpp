#include "ibmras/monitoring/connector/Receiver.h"
#include "ibmras/monitoring/Plugin.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/monitoring/plugins/jni/JNIReceiver.h"
#include "ibmras/monitoring/plugins/jni/locking/LockingDataProvider.h"
#include "ibmras/monitoring/plugins/jni/memory/MemoryDataProvider.h"
#include "ibmras/monitoring/plugins/jni/memorycounter/MemoryCounterDataProvider.h"
#include "ibmras/monitoring/plugins/jni/threads/ThreadDataProvider.h"
#include "ibmras/monitoring/plugins/j9/DumpHandler.h"
#include "ibmras/monitoring/plugins/jmx/os/OSJMXPullSource.h"
#include "ibmras/common/util/strUtils.h"

#include <iostream>

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jni {

int startReceiver() {
	return 0;
}

int stopReceiver() {
	return 0;
}

JNIReceiver::JNIReceiver() {
	name = "JNI receiver";
	pull = NULL;
	push = NULL;
	start = ibmras::monitoring::plugins::jni::startReceiver;
	stop = ibmras::monitoring::plugins::jni::stopReceiver;
	type = ibmras::monitoring::plugin::receiver;
	recvfactory = (RECEIVER_FACTORY) ibmras_getJNIReceiver;
	confactory = NULL;
}

JNIReceiver::~JNIReceiver() {
}

void JNIReceiver::receiveMessage(const std::string &id, uint32 size,
		void *data) {

	std::string message((const char*) data, size);

	if (id == "locking") {
		std::size_t found = message.find(',');
		std::string command = message.substr(0, found);
		std::string rest = message.substr(found + 1);

		ibmras::monitoring::plugins::jni::locking::JLAPullSource::setState(
				command);
	} else if (id == "memory") {
		std::size_t found = message.find(',');
		std::string command = message.substr(0, found);
		std::string rest = message.substr(found + 1);

		ibmras::monitoring::plugins::jni::memory::MEMPullSource::setState(
				command);
	} else if (id == "threads") {
		std::size_t found = message.find(',');
		std::string command = message.substr(0, found);
		std::string rest = message.substr(found + 1);

		ibmras::monitoring::plugins::jni::threads::TDPullSource::setState(
				command);
	} else if (id == "memorycounters") {
		std::size_t found = message.find(',');
		std::string command = message.substr(0, found);
		std::string rest = message.substr(found + 1);

		ibmras::monitoring::plugins::jni::memorycounter::MCPullSource::setState(
				command);
	} else if (id == "cpu") {
		std::size_t found = message.find(',');
		std::string command = message.substr(0, found);
		std::string rest = message.substr(found + 1);

		ibmras::monitoring::plugins::jmx::os::OSJMXPullSource::setState(
				command);
	} else if (id == "environment") {
		std::size_t found = message.find(',');
		std::string command = message.substr(0, found);
		std::string rest = message.substr(found + 1);
		std::vector < std::string > parameters = ibmras::common::util::split(
				rest, ',');

		if (ibmras::common::util::equalsIgnoreCase(command, "set")) {
			ibmras::monitoring::plugins::j9::DumpHandler::requestDumps (parameters);
		}
	}
}

}
}
}
}

void* ibmras_getJNIReceiver() {
	return new ibmras::monitoring::plugins::jni::JNIReceiver();
}

