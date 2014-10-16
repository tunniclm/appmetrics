#ifndef ibmras_monitoring_plugins_j9_dumphandler_h
#define ibmras_monitoring_plugins_j9_dumphandler_h

#include "ibmras/monitoring/connector/Receiver.h"
#include "ibmras/monitoring/Plugin.h"

namespace ibmras{
namespace monitoring {
namespace plugins {
namespace j9 {

class DumpHandler: public ibmras::monitoring::Plugin {
public:
	DumpHandler(jvmFunctions functions);
	virtual ~DumpHandler();
	int startHandler();
	int stopHandler();
	static uint32 providerID;
	static DumpHandler* getInstance(jvmFunctions functions);
	static DumpHandler* getInstance();
	static void requestDumps(const std::vector<std::string> &dumpRequests);
	void triggerDumps(const std::vector<std::string> &dumpRequests);
private:
	jvmFunctions vmFunctions;

};

}
}
}
}/* namespace monitoring */
#endif /* ibmras_monitoring_plugins_j9_dumphandler_h */
