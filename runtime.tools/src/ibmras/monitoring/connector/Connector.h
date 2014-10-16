/*
 * Connector.h
 *
 *  Created on: 21 Feb 2014
 *      Author: robbins
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
