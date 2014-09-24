#ifndef FACADE_H_
#define FACADE_H_

#include "ibmras/monitoring/plugins/j9/trace/TraceDataProvider.h"

/*
 * These declarations are in a separate file to prevent circular inclusion
 * by the JMX SourceManager and specific JMX data provider plugins.
 */

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jni {

DECL JavaVM* getJVM();
DECL void setTDPP(jvmFunctions* tdpp);

DECL jvmFunctions* getTDPP();

DECL ibmras::monitoring::Plugin* getPlugin();

}
}
}
}

#endif /* FACADE_H_ */
