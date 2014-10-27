 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#ifndef ibmras_monitoring_plugins_jni_jnireceiver_h
#define ibmras_monitoring_plugins_jni_jnireceiver_h

#include "ibmras/monitoring/connector/Receiver.h"
#include "ibmras/monitoring/Plugin.h"

extern "C" DECL void* ibmras_getJNIReceiver();

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jni {

class JNIReceiver: public ibmras::monitoring::connector::Receiver, ibmras::monitoring::Plugin {
public:
	JNIReceiver();
	virtual ~JNIReceiver();
	int startReceiver();
	int stopReceiver();
	void receiveMessage(const std::string &id, uint32 size, void *data);
};
}
}
}
}

#endif /* ibmras_monitoring_plugins_jni_jnireceiver_h */
