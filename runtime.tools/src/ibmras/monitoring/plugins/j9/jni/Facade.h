 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#ifndef ibmras_monitoring_plugins_j9_jni_facade_h
#define ibmras_monitoring_plugins_j9_jni_facade_h

#include "ibmras/monitoring/plugins/j9/trace/TraceDataProvider.h"

/*
 * These declarations are in a separate file to prevent circular inclusion
 * by the JMX SourceManager and specific JMX data provider plugins.
 */

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace jni {

DECL JavaVM* getJVM();
DECL void setTDPP(jvmFunctions* tdpp);

DECL jvmFunctions* getTDPP();

DECL ibmras::monitoring::Plugin* getPlugin();

}
}
}
}
}

#endif /* ibmras_monitoring_plugins_j9_jni_facade_h */
