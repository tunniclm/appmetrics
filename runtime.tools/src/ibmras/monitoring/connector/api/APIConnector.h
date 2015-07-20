/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#ifndef ibmras_monitoring_connector_apiconnector_h
#define ibmras_monitoring_connector_apiconnector_h

#include "ibmras/monitoring/connector/Connector.h"
#include "ibmras/monitoring/AgentExtensions.h"
#include "ibmras/monitoring/Typesdef.h"

#include <cstring>

#if defined(_WINDOWS)
#define APICONNECTORPLUGIN_DECL __declspec(dllexport)   /* required for DLLs to export the plugin functions */
#else
#define APICONNECTORPLUGIN_DECL
#endif

namespace APIConnector {

extern "C" {
APICONNECTORPLUGIN_DECL void registerListener(void(*)(const std::string&, unsigned int, void*));
APICONNECTORPLUGIN_DECL void deregisterListener();
APICONNECTORPLUGIN_DECL void sendControl(const std::string, unsigned int length, void* message);
}

class APIConnector: public ibmras::monitoring::connector::Connector {
public:

    APIConnector();

    std::string getID() { return "APIConnector"; }

    int sendMessage(const std::string &sourceId, uint32 size, void *data);

	void registerReceiver(ibmras::monitoring::connector::Receiver *receiver);
    void deregisterReceiver();
       
	int start();
    int stop();

    ~APIConnector();

private:

};

/* end class Connector */

namespace plugin {
        agentCoreFunctions api;
        uint32 provid;
	ibmras::monitoring::connector::Receiver *receiver;
}


} /* end APIConnector monitoring */

#endif /* ibmras_monitoring_connector_apiconnector_h */

