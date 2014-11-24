 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_monitoring_connector_configurationconnector_h
#define ibmras_monitoring_connector_configurationconnector_h


#include "ibmras/monitoring/connector/Connector.h"
#include "ibmras/common/Properties.h"
#include "ibmras/common/port/Lock.h"

namespace ibmras {
namespace monitoring {
namespace connector {

class DECL ConfigurationConnector : public ibmras::monitoring::connector::Connector {
public:
	ConfigurationConnector();
	virtual ~ConfigurationConnector();

	std::string getConfig(const std::string &name);

	int sendMessage(const std::string &sourceId, uint32 size, void *data);

	std::string getID() {
		return "ConfigurationConnector";
	}
private:
	ibmras::common::Properties config;
	ibmras::common::port::Lock configLock;

};

} /* namespace connector */
} /* namespace monitoring */
} /* namespace ibmras */
#endif /* ibmras_monitoring_connector_configurationconnector_h */
