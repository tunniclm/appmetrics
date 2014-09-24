#ifndef JMX_H_
#define JMX_H_

/*
 * These declarations are in a separate file to prevent circular inclusion
 * by the JMXSourceManager and specific JMX data provider plugins.
 */

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jmx {

DECL JavaVM* getJVM();
DECL void setJVM(JavaVM* vm);

DECL ibmras::monitoring::Plugin* getPlugin();

}
}
}
}

#endif /* JMX_H_ */
