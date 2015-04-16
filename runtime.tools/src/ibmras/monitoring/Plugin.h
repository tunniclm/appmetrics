 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_monitoring_plugin_h
#define ibmras_monitoring_plugin_h

#include "ibmras/monitoring/AgentExtensions.h"
#include "ibmras/common/types.h"
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
	const char* version;

	int (*init)(const char *properties);				/* Plugin inialization method */
	pushsource* (*push)(agentCoreFunctions, uint32);	/* push source function pointer or NULL */
	pullsource* (*pull)(agentCoreFunctions, uint32);							/* pull source function pointer or NULL */
	int (*start)(void);									/* start function to begin data production */
	int (*stop)(void);										/* stop function to end data production */
	const char* (*getVersion)(void);								/* returns plugin version, used to enforce versioning */
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
