/*
 * ConfigurationConnector.h
 *
 *  Created on: 7 Oct 2014
 *      Author: robbins
 */

#ifndef CONFIGURATIONCONNECTOR_H_
#define CONFIGURATIONCONNECTOR_H_


#include "ibmras/monitoring/connector/Connector.h"
#include "ibmras/common/Properties.h"

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

};

} /* namespace connector */
} /* namespace monitoring */
} /* namespace ibmras */
#endif /* CONFIGURATIONCONNECTOR_H_ */
