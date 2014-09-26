/*
 * HLConnectorPlugin.h
 *
 *  Created on: 21 Aug 2014
 *      Author: Admin
 */

#ifndef ibmras_monitoring_connector_headless_hlconnectorplugin_h
#define ibmras_monitoring_connector_headless_hlconnectorplugin_h

#include <ibmras/monitoring/Plugin.h>
#include "ibmras/monitoring/connector/headless/HLConnector.h"
#include "jni.h"

namespace ibmras {
namespace monitoring {
namespace connector {
namespace headless {

class HLConnectorPlugin: public ibmras::monitoring::Plugin {
public:
	static HLConnectorPlugin* getInstance(JavaVM* theVM);
	virtual ~HLConnectorPlugin();

protected:
	HLConnectorPlugin(JavaVM* theVM);

private:

	static void* getConnector(const char* properties);

	HLConnector* getHLConnector();
	JavaVM* vm;

};


} /*end namespace headless*/
} /*end namespace connector*/
} /*end namespace monitoring*/
} /*end namespace ibmras*/



#endif /* ibmras_monitoring_connector_headless_hlconnectorplugin_h */
