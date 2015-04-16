 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_monitoring_connector_ostreamconnector_h
#define ibmras_monitoring_connector_ostreamconnector_h

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

#endif /* ibmras_monitoring_connector_ostreamconnector_h */
