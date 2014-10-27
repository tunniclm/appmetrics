 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_monitoring_connector_connector_h
#define ibmras_monitoring_connector_connector_h

#include "ibmras/monitoring/connector/Receiver.h"


namespace ibmras{
namespace monitoring {
namespace connector {

class Connector {
public:
	virtual std::string getID() { return "Connector"; };			/* unique ID for the connector */
	virtual int sendMessage(const std::string &sourceId, uint32 size,
			void *data) {return 0;};

	virtual void registerReceiver(Receiver *receiver) {};

	virtual int start() {return 0;};
	virtual int stop() {return 0;};

	virtual ~Connector() {};

protected:
	Connector() {
	}
	;

};
/* end class Connector */

}
}
} /* end namespace connector */

#endif /* ibmras_monitoring_connector_connector_h */
