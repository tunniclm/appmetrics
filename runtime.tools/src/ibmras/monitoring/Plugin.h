/*
 * Plugin.h
 *
 *  Created on: 21 Feb 2014
 *      Author: adam
 */

#ifndef ibmras_monitoring_plugin_h
#define ibmras_monitoring_plugin_h

#include "ibmras/monitoring/Monitoring.h"
#include <vector>
#include <string>

#include "ibmras/common/util/LibraryUtils.h"

namespace ibmras {
namespace monitoring {

namespace plugin {

enum PluginType {none = 0, data = 1, connector = 2, receiver = 4};

} /* end namespace Plugin */


/*
 * Defines a plugin that implements either push and/or pull source functions
 */
class Plugin {
public:
	Plugin();
	virtual ~Plugin() {}

	void unload();

	static std::vector<Plugin*> scan(const std::string &dir); /* scan a directory and return a list of candidate plugins */
	static Plugin* processLibrary(const std::string &filePath);

	std::string name;										/* name of the library - typically this is the full path */

	int (*init)(const char *properties);				/* Plugin inialization method */
	pushsource* (*push)(void (*callback)(monitordata*), uint32);	/* push source function pointer or NULL */
	pullsource* (*pull)(uint32);							/* pull source function pointer or NULL */
	int (*start)(void);									/* start function to begin data production */
	int (*stop)(void);										/* stop function to end data production */
	CONNECTOR_FACTORY confactory;							/* Connector factory */
	RECEIVER_FACTORY recvfactory;                           /* Receiver factory */
	ibmras::common::util::LibraryUtils::Handle handle;	/* handle to be used when closing the dynamically loaded plugin */
	int type;

private:
	void setType();
};



}
}

#endif /* ibmras_monitoring_plugin_h */
