/*
 * JMXConnectorPlugin.h
 *
 *  Created on: 4 Jul 2014
 *      Author: robbins
 */

#ifndef ibmras_monitoring_connector_jmx_jmxconnectorplugin_h
#define ibmras_monitoring_connector_jmx_jmxconnectorplugin_h

#include "ibmras/monitoring/Plugin.h"
#include "ibmras/monitoring/connector/jmx/JMXConnector.h"

namespace ibmras {
namespace monitoring {
namespace connector {
namespace jmx {

class JMXConnectorPlugin: public ibmras::monitoring::Plugin {
public:
	static JMXConnectorPlugin* getInstance(JavaVM *theVM, const std::string &options);
	virtual ~JMXConnectorPlugin();

	static int startPlugin();
	static int stopPlugin();
	static void* getConnector(const char* properties);

protected:
	JMXConnectorPlugin(JavaVM *theVM, const std::string &options);

private:
	JMXConnector *jmxConnector;
	JMXConnector* getJMXConnector(const char* properties);

	JavaVM *vm;
	std::string agentOptions;
};

} /* namespace jmx */
} /* namespace connector */
} /* namespace monitoring */
} /* namespace ibmras */

#endif /* ibmras_monitoring_connector_jmx_jmxconnectorplugin_h */
