/*
 * JMXConnector.h
 *
 *  Created on: 4 Jul 2014
 *      Author: robbins
 */

#ifndef ibmras_monitoring_agent_connector_jmxconnector_h
#define ibmras_monitoring_agent_connector_jmxconnector_h

#include "ibmras/monitoring/connector/Connector.h"

#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/common/Logger.h"

#include "jni.h"

namespace ibmras {
namespace monitoring {
namespace connector {
namespace jmx {

class JMXConnector: public ibmras::monitoring::connector::Connector {
public:
	virtual std::string getID() { return "JMX Connector"; }

	JMXConnector(JavaVM *theVM, const std::string &options, const char* properties);
	virtual ~JMXConnector();

	virtual int start();
	virtual int stop();

protected:

private:
	int launchMBean();

	JavaVM *vm;
	std::string agentOptions;


};

} /* namespace jmx */
} /* namespace connector */
} /* namespace monitoring */
} /* namespace ibmras */

#endif /* ibmras_monitoring_agent_connector_jmxconnector_h */
