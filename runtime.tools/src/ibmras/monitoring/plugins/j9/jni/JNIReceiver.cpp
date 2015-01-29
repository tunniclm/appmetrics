 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include "ibmras/monitoring/connector/Receiver.h"
#include "ibmras/monitoring/Plugin.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/monitoring/plugins/j9/jni/JNIReceiver.h"
#include "ibmras/monitoring/plugins/j9/jni/locking/LockingDataProvider.h"
#include "ibmras/monitoring/plugins/j9/jni/memory/MemoryDataProvider.h"
#include "ibmras/monitoring/plugins/j9/jni/memorycounter/MemoryCounterDataProvider.h"
#include "ibmras/monitoring/plugins/j9/jni/threads/ThreadDataProvider.h"
#include "ibmras/monitoring/plugins/j9/DumpHandler.h"
#include "ibmras/monitoring/plugins/j9/jmx/os/OSJMXPullSource.h"
#include "ibmras/common/util/strUtils.h"

#include <iostream>

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace jni {

const char* JNIRecVersion = "99.99.99";

int startReceiver() {
	return 0;
}

int stopReceiver() {
	return 0;
}

const char* getVersionJNIRec() {
	return JNIRecVersion;
}

JNIReceiver::JNIReceiver() {
	name = "JNI receiver";
	pull = NULL;
	push = NULL;
	start = ibmras::monitoring::plugins::j9::jni::startReceiver;
	stop = ibmras::monitoring::plugins::j9::jni::stopReceiver;
	getVersion = getVersionJNIRec;
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

		ibmras::monitoring::plugins::j9::jni::locking::JLAPullSource::setState(
				command);
	} else if (id == "memory") {
		std::size_t found = message.find(',');
		std::string command = message.substr(0, found);
		std::string rest = message.substr(found + 1);

		ibmras::monitoring::plugins::j9::jni::memory::MEMPullSource::setState(
				command);
	} else if (id == "threads") {
		std::size_t found = message.find(',');
		std::string command = message.substr(0, found);
		std::string rest = message.substr(found + 1);

		ibmras::monitoring::plugins::j9::jni::threads::TDPullSource::setState(
				command);
	} else if (id == "memorycounters") {
		std::size_t found = message.find(',');
		std::string command = message.substr(0, found);
		std::string rest = message.substr(found + 1);

		ibmras::monitoring::plugins::j9::jni::memorycounter::MCPullSource::setState(
				command);
	} else if (id == "cpu") {
		std::size_t found = message.find(',');
		std::string command = message.substr(0, found);
		std::string rest = message.substr(found + 1);

		ibmras::monitoring::plugins::j9::jmx::os::OSJMXPullSource::setState(
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
}

void* ibmras_getJNIReceiver() {
	return new ibmras::monitoring::plugins::j9::jni::JNIReceiver();
}

