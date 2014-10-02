
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
