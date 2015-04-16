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
#include "ibmras/monitoring/plugins/j9/DumpHandler.h"
#include "ibmras/monitoring/plugins/j9/jmx/os/OSJMXPullSource.h"
#include "ibmras/common/util/strUtils.h"

#include <iostream>

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace jni {

const char* JNIRecVersion = "1.0";

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

	if (id == "locking") {
		std::string message((const char*) data, size);
		std::size_t found = message.find(',');
		std::string command = message.substr(0, found);
		std::string rest = message.substr(found + 1);

		ibmras::monitoring::plugins::j9::jni::locking::JLAPullSource::setState(
				command);
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

