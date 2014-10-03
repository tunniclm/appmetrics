/*
 * agent.h
 *
 *  Created on: 4 Feb 2014
 *      Author: adam
 */

#ifndef ibmras_monitoring_agent_agent_h
#define ibmras_monitoring_agent_agent_h

#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/monitoring/agent/DataSource.h"
#include "ibmras/monitoring/agent/DataSourceList.h"
#include "ibmras/monitoring/connector/Connector.h"
#include "ibmras/monitoring/agent/BucketList.h"
#include "ibmras/monitoring/agent/BucketDataQueueEntry.h"
#include "ibmras/monitoring/agent/MonitorDataQueue.h"
#include "ibmras/monitoring/connector/ConnectorManager.h"
#include "ibmras/monitoring/Plugin.h"
#include "ibmras/common/Logger.h"
#include "ibmras/common/LogManager.h"
#include "ibmras/common/Properties.h"

#include <string>
/*
 * Definitions for the internal workings of the agent
 *
 */


namespace ibmras {
namespace monitoring {
namespace agent {

class DECL Agent {
public:
	static Agent* getInstance();					/* return the singleton instance of the agent */
	static std::string getBuildDate();
	void init();							/* invoke to start the agent initialisation lifecycle event */
	void start();						/* invoke to start the agent start lifecycle event */
	void stop();							/* invoke to start the agent stop lifecycle event */
	void shutdown();						/* invoke to shutdown the agent, it cannot be restarted after this */


	void addConnector(ibmras::monitoring::connector::Connector* con);
	void removeConnector(ibmras::monitoring::connector::Connector* con);
	DataSourceList<pullsource> getPullSources();
	DataSourceList<pushsource> getPushSources();
	DataSource<pullsource>* getPullSource(std::string uniqueID);
	DataSource<pushsource>* getPushSource(std::string uniqueID);
	BucketList* getBucketList();

	ibmras::monitoring::connector::Connector* getConnector(const std::string &id);
	ibmras::monitoring::connector::ConnectorManager* getConnectionManager();
	bool addData(monitordata* data);

	void publish();							/* publish messages to connectors */
	void republish(const std::string &prefix); /* republish history */
	void drain();							/* drain any plugin data into the buckets */
	void threadStop();						/* fired when an agent processing thread stops */
	void setLogOutput(ibmras::common::LOCAL_LOGGER_CALLBACK func);
	void setLogLevels();
	void setLocalLog(bool local);
	void addPlugin(ibmras::monitoring::Plugin* plugin);	/* manually add a plugin to the agent */
	void addPlugin(const std::string &dir, const std::string library);	/* manually add a plugin to the agent */

	ibmras::common::Properties getProperties();
	void setProperties(const ibmras::common::Properties &props);
	void setProperty(const std::string &prop, const std::string &value);
	std::string getProperty(const std::string &prop);

	std::string getAgentPropertyPrefix();
	std::string getAgentProperty(const std::string &agentProp);
	void setAgentProperty(const std::string &agentProp, const std::string &value);

private:
	Agent();					/* private constructor */
	static Agent* instance;		/* singleton instance */
	void addPushSource(std::vector<ibmras::monitoring::Plugin*>::iterator i, uint32 provID);
	void addPullSource(std::vector<ibmras::monitoring::Plugin*>::iterator i, uint32 provID);
	void addSystemPlugins(); /* adds agent internal / system push or pull sources */
	void createBuckets();
	void startPlugins(); 		/* call the start method on each plugin */
	void stopPlugins(); 		/* call the stop method on each plugin */
	void startConnectors(); 		/* initialise the connectors */
	void startReceivers(); 		/* initialise any receivers */
	BucketList bucketList;

	ibmras::monitoring::connector::ConnectorManager connectionManager;
	DataSourceList<pushsource> pushSourceList;
	DataSourceList<pullsource> pullSourceList;
	std::vector<ibmras::monitoring::Plugin*> plugins;
	MonitorDataQueue dataq;
	uint32 activeThreadCount;		/* number of active threads */
	
	ibmras::common::Properties properties;

};
}
}
} /* end namespace agent */


#endif /* ibmras_monitoring_agent_agent_h */
