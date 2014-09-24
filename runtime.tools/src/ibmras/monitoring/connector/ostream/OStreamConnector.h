/*
 * OStreamConnector.h
 *
 *  Created on: 24 Feb 2014
 *      Author: robbins
 */

#ifndef OSTREAMCONNECTOR_H_
#define OSTREAMCONNECTOR_H_

#include "ibmras/monitoring/connector/Connector.h"
#include <iostream>

namespace OStream {

class OStreamConnector: public ibmras::monitoring::connector::Connector {
public:

	OStreamConnector();
	OStreamConnector(std::ostream &outputStream);

	std::string getID() { return "OStreamConnector"; }

	int sendMessage(const std::string &sourceId, uint32 size, void *data);

	void registerReceiver(ibmras::monitoring::connector::Receiver *receiver);

	int start();
	int stop();

	~OStreamConnector();
private:
	std::ostream & output;

};

/* end class Connector */

} /* end OStream monitoring */

#endif /* OSTREAMCONNECTOR_H_ */
