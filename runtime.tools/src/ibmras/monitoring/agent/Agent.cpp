 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


/*
 * Main agent functionality
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>

#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/common/logging.h"
#include "ibmras/common/Memory.h"
#include "ibmras/monitoring/agent/threads/ThreadPool.h"
#include "ibmras/common/PropertiesFile.h"
#include "ibmras/common/LogManager.h"
#include "ibmras/monitoring/agent/SystemReceiver.h"

#define AGENT(func) ibmras::monitoring::agent::Agent::getInstance()->func();
#define AGENTP(func,param) ibmras::monitoring::agent::Agent::getInstance()->func(param);


#if defined(WINDOWS)
const char PATHSEPARATOR = '\\';
const char* LIBPREFIX = "";
const char* LIBSUFFIX = ".dll";
#else
const char PATHSEPARATOR = '/';
const char* LIBPREFIX = "lib";
const char* LIBSUFFIX = ".so";
#endif

namespace ibmras {
namespace monitoring {
namespace agent {

#define AGENT_VERSION "99.99.99.29991231"

static const char* PROPERTIES_PREFIX = "com.ibm.diagnostics.healthcenter.";
static const char* HEARTBEAT_TOPIC = "heartbeat";

bool running = false;
bool updateNow = false;

Agent* Agent::instance = new Agent;

IBMRAS_DEFINE_LOGGER("Agent")
;

Agent::Agent() {
	activeThreadCount = 0;
}

void Agent::setLogOutput(ibmras::common::LOCAL_LOGGER_CALLBACK func) {
	ibmras::common::LogManager::localLogFunc = func;
}

std::string Agent::getBuildDate() {
	return __DATE__ " " __TIME__;
}

std::string Agent::getVersion() {
	return AGENT_VERSION;
}

void Agent::setLogLevels() {

	std::string loggingPropertyPrefix = PROPERTIES_PREFIX;
	loggingPropertyPrefix +="logging.";

	ibmras::common::LogManager* logMan = ibmras::common::LogManager::getInstance();
	std::list<std::string> keys = properties.getKeys(loggingPropertyPrefix);
	for (std::list<std::string>::iterator i = keys.begin(); i != keys.end();
			++i) {
		std::string component = i->substr(loggingPropertyPrefix.length());
		if (component.length() > 0) {
			std::string value = properties.get(*i);
			ibmras::common::logging::Level lev = ibmras::common::logging::none;
			if (value.compare("warning") == 0) {
				lev = ibmras::common::logging::warning;
			} else if (value.compare("info") == 0) {
				lev = ibmras::common::logging::info;
			} else if (value.compare("fine") == 0) {
				lev = ibmras::common::logging::fine;
			} else if (value.compare("finest") == 0) {
				lev = ibmras::common::logging::finest;
			} else if (value.compare("debug") == 0) {
				lev = ibmras::common::logging::debug;
			} else {
				lev = ibmras::common::logging::none;
			}
			logMan->setLevel(component, lev);
		}
	}

}

void Agent::setLocalLog(bool local) {
	ibmras::common::LogManager::getInstance()->localLog = local;
}

DataSourceList<pullsource> Agent::getPullSources() {
	return pullSourceList;
}

DataSourceList<pushsource> Agent::getPushSources() {
	return pushSourceList;
}

DataSource<pullsource>* Agent::getPullSource(std::string uniqueID) {
	Agent* agent = Agent::getInstance();
	uint32 pullcount = agent->getPullSources().getSize();
	for (uint32 i = 0; i < pullcount; i++) {
		DataSource<pullsource> *dsrc = agent->getPullSources().getItem(i);
		if (dsrc->getUniqueID().compare(uniqueID) == 0) {
			return dsrc;
		}
	}
	return NULL;
}

DataSource<pushsource>* Agent::getPushSource(std::string uniqueID) {
	Agent* agent = Agent::getInstance();
	uint32 pushcount = agent->getPushSources().getSize();
	for (uint32 i = 0; i < pushcount; i++) {
		DataSource<pushsource> *dsrc = agent->getPushSources().getItem(i);
		if (dsrc->getUniqueID().compare(uniqueID) == 0) {
			return dsrc;
		}
	}
	return NULL;
}

void callback(monitordata* data) {
	Agent* agent = Agent::getInstance();
	agent->addData(data);
}

/* thread entry point for publishing data from buckets to the registered connector */
void* processPublishLoop(ibmras::common::port::ThreadData* param) {
	IBMRAS_DEBUG(info, "Starting agent publishing loop");
	Agent* agent = Agent::getInstance();
	int count = 0;
	while (running) {
		ibmras::common::port::sleep(1);
		agent->publish();

		// Send heartbeat ping every 20 seconds
		if (++count > 20) {
			count = 0;
			agent->getConnectionManager()->sendMessage(HEARTBEAT_TOPIC, 0, NULL);
		}

	}
	IBMRAS_DEBUG(info, "Exiting agent publishing loop");
	agent->threadStop();
	return NULL;
}


void* processPullSourceLoop(ibmras::common::port::ThreadData* data) {
	Agent* agent = Agent::getInstance();
	uint32 pullcount = agent->getPullSources().getSize();
	uint32 srccount = pullcount;
	for (uint32 i = 0; i < pullcount; i++) {
		DataSource<pullsource> *dsrc = agent->getPullSources().getItem(i);
		if (!(dsrc->getSource()->callback && dsrc->getSource()->complete)) {
			IBMRAS_DEBUG_1(warning, "Pull source %s disabled due to missing callback or complete function",
					dsrc->getUniqueID().c_str());
			srccount--; /* decrease number of valid sources */
		}
	}
	ibmras::monitoring::agent::threads::ThreadPool* pool =
			new ibmras::monitoring::agent::threads::ThreadPool(5, srccount); /* 5 pull source threads */
	for (uint32 i = 0; i < pullcount; i++) {
		DataSource<pullsource> *dsrc = agent->getPullSources().getItem(i);
		if (dsrc->getSource()->callback && dsrc->getSource()->complete) {
			pool->setPullSource(--srccount, dsrc->getSource());
			IBMRAS_DEBUG_2(debug, "pullsource %d is source %s", srccount, dsrc->getSource()->header.name);
		}
	}

	IBMRAS_DEBUG(info, "Starting agent process pull source loop");

	pool->startAll();
	while (running) {
		ibmras::common::port::sleep(1); /* polling interval for thread */
		pool->process(updateNow); /* process the pull sources */
		updateNow = false;
	}
	pool->stopAll();
	delete pool; /* clean up */
	IBMRAS_DEBUG(info, "Exiting agent process pull source loop");
	agent->threadStop();
	return NULL;
}

void Agent::immediateUpdate() {
	updateNow = true;
}

void Agent::publish() {
	bucketList.publish(connectionManager);
}

void Agent::republish(const std::string &topicPrefix) {
	bucketList.republish(topicPrefix, connectionManager);
}


void Agent::addPushSource(std::vector<ibmras::monitoring::Plugin*>::iterator i,
		uint32 provID) {
	if ((*i)->push) {
		pushsource *push = (*i)->push(callback, provID);
		if (push) {
			IBMRAS_DEBUG(debug, "Push sources were defined");
			pushSourceList.add(provID, push, (*i)->name);
			IBMRAS_DEBUG_1(debug, "Push source list size now : %d",
					pushSourceList.getSize());
			IBMRAS_DEBUG(debug, pushSourceList.toString().c_str());
		} else {
			IBMRAS_DEBUG(info, "No sources were defined !");
		}
	}
}

void Agent::addPullSource(std::vector<ibmras::monitoring::Plugin*>::iterator i,
		uint32 provID) {
	if ((*i)->pull) {
		pullsource *pull = (*i)->pull(provID);
		if (pull) {
			IBMRAS_DEBUG(debug, "Pull sources were defined");
			pullSourceList.add(provID, pull, (*i)->name);
			IBMRAS_DEBUG_1(info, "Pull source list size now : %d",
					pullSourceList.getSize());
			IBMRAS_DEBUG(debug, pullSourceList.toString().c_str());
		} else {
			IBMRAS_DEBUG(info, "No pull sources were defined !");
		}
	}
}

/*
 * Add a bucket to the overall list of data buckets
 */
void Agent::createBuckets() {
	IBMRAS_DEBUG(fine, "Creating buckets");
	bucketList.add(pushSourceList.getBuckets());
	bucketList.add(pullSourceList.getBuckets());
}

void Agent::addPlugin(ibmras::monitoring::Plugin* plugin) {
	if (plugin) {
		IBMRAS_DEBUG_1(info, "Adding plugin %s", plugin->name.c_str());IBMRAS_DEBUG_4(info, "Push source %p, Pull source %p, start %p, stop %p",
				plugin->push, plugin->pull, plugin->start, plugin->stop);
		plugins.push_back(plugin);
		IBMRAS_DEBUG(info, "Plugin added");
	} else {
		IBMRAS_DEBUG(warning, "Attempt to add null plugin");
	}
}

void Agent::addPlugin(const std::string &dir, const std::string library) {
	ibmras::monitoring::Plugin *plugin = ibmras::monitoring::Plugin::processLibrary(dir + PATHSEPARATOR + LIBPREFIX + library + LIBSUFFIX);
	if (plugin) {
		plugins.push_back(plugin);
	}
}


void Agent::addSystemPlugins() {
//	addPlugin(ibmras::common::LogManager::getPlugin());
	addPlugin(
			(ibmras::monitoring::Plugin*) new ibmras::monitoring::agent::SystemReceiver());
}

void Agent::addConnector(ibmras::monitoring::connector::Connector* con) {
	connectionManager.addConnector(con);
}
void Agent::removeConnector(ibmras::monitoring::connector::Connector* con) {
	connectionManager.removeConnector(con);
}

void Agent::init() {
	IBMRAS_DEBUG(info, "Agent initialisation : start");

	std::string searchPath = getAgentProperty("plugin.path");
	IBMRAS_DEBUG_1(debug, "Plugin search path : %s", searchPath.c_str());
	if (searchPath.size() > 0) {
		std::vector<ibmras::monitoring::Plugin*> found =
				ibmras::monitoring::Plugin::scan(searchPath);
		plugins.insert(plugins.begin(), found.begin(), found.end());
	}

	addSystemPlugins();

	std::string pluginProperties = properties.toString();

	IBMRAS_DEBUG_1(info, "%d plugins found", plugins.size());
	uint32 provID = 0;
	for (std::vector<ibmras::monitoring::Plugin*>::iterator i =
			plugins.begin(); i != plugins.end(); ++i, provID++) {
		IBMRAS_DEBUG_1(fine, "Library : %s", (*i)->name.c_str());
		if ((*i)->init) {
			(*i)->init(pluginProperties.c_str());
		}
		if ((*i)->type & ibmras::monitoring::plugin::data) {
			addPushSource(i, provID);
			addPullSource(i, provID);
		}
	}
	createBuckets();
	addConnector(&configConn);
	IBMRAS_DEBUG(finest, bucketList.toString().c_str());IBMRAS_DEBUG(info, "Agent initialisation : end");
}

std::string Agent::getConfig(const std::string& name) {
	return configConn.getConfig(name);
}

bool Agent::readOnly() {
	std::string readOnlyMode = getAgentProperty("readonly");
	if (!readOnlyMode.compare("on")) {
		return true;
	}
	return false;
}


void Agent::start() {
	int result = 0;

	IBMRAS_DEBUG(info, "Agent start : begin");

	/* Connectors must be started before the plugins start pushing data */
	IBMRAS_DEBUG(info, "Agent start : connectors");
	startConnectors();

	IBMRAS_DEBUG(info, "Agent start : receivers");
	startReceivers();

	IBMRAS_DEBUG(info, "Agent start : data providers");
	startPlugins();

	running = true; /* if any of the thread creation below fails then running will be set to false and started threads will exit */

	ibmras::common::port::ThreadData* data =
			new ibmras::common::port::ThreadData(processPullSourceLoop);
	result = ibmras::common::port::createThread(data);
	if (result) {
		running = false;
	} else {
		activeThreadCount++;
		data = new ibmras::common::port::ThreadData(processPublishLoop);
		result = ibmras::common::port::createThread(data);
		if (result) {
			running = false;
		} else {
			activeThreadCount++; /* should end up with three active threads */
		}
	}

	IBMRAS_DEBUG(info, "Agent start : finish");
}

void Agent::startPlugins() {
	for (std::vector<ibmras::monitoring::Plugin*>::iterator i =
			plugins.begin(); i != plugins.end(); ++i) {
		if ((*i)->start) {
			IBMRAS_DEBUG_1(info, "Invoking plugin start method %s",
					(*i)->name.c_str());
			(*i)->start();
		} else {
			IBMRAS_DEBUG_1(info, "Warning : no start method defined on %s",
					(*i)->name.c_str());
		}
	}
}
void Agent::stopPlugins() {
	for (std::vector<ibmras::monitoring::Plugin*>::iterator i =
			plugins.begin(); i != plugins.end(); ++i) {
		if ((*i)->stop) {
			IBMRAS_DEBUG_1(info, "Invoking plugin stop method %s",
					(*i)->name.c_str());
			(*i)->stop();
		} else {
			IBMRAS_DEBUG_1(info, "Warning : no stop method defined on %s",
					(*i)->name.c_str());
		}
	}
}

BucketList* Agent::getBucketList() {
	return &bucketList;
}


void Agent::startReceivers() {
	for (std::vector<ibmras::monitoring::Plugin*>::iterator i =
			plugins.begin(); i != plugins.end(); ++i) {
		if ((*i)->type & ibmras::monitoring::plugin::receiver) {
			if ((*i)->recvfactory) {
				void* instance = (*i)->recvfactory();
				ibmras::monitoring::connector::Receiver* receiver =
						reinterpret_cast<ibmras::monitoring::connector::Receiver*>(instance);
				if (receiver) {
					IBMRAS_DEBUG_1(info, "Add receiver %s to connector manager",
							(*i)->name.c_str());
					connectionManager.addReceiver(receiver);
				}
			}
		}
	}
}

void Agent::startConnectors() {
	std::string connectorProperties = properties.toString();
	for (std::vector<ibmras::monitoring::Plugin*>::iterator i =
			plugins.begin(); i != plugins.end(); ++i) {
		IBMRAS_DEBUG_2(info, "Agent::startConnectors %s type is %d", (*i)->name.c_str(),
				(*i)->type);
		if ((*i)->type & ibmras::monitoring::plugin::connector) {
			IBMRAS_DEBUG(info, "it is a connector");
			if ((*i)->confactory) {
				IBMRAS_DEBUG_1(info, "Invoking factory method for %s",
						(*i)->name.c_str());
				void* instance = (*i)->confactory(connectorProperties.c_str());
				ibmras::monitoring::connector::Connector* con =
						reinterpret_cast<ibmras::monitoring::connector::Connector*>(instance);
				if (con) {
					IBMRAS_DEBUG(info, "Add connector to connector manager");
					connectionManager.addConnector(con);
					// Register the receiver with each connector
					con->registerReceiver(&connectionManager);
				}
			} else {
				IBMRAS_DEBUG_1(info, "Warning : no factory method defined on %s",
						(*i)->name.c_str());
			};
		}
	}
	connectionManager.start();
}

void Agent::stop() {
	IBMRAS_DEBUG(info, "Agent stop : begin");
	running = false;
	IBMRAS_DEBUG(fine, "Waiting for active threads to stop");
	while (activeThreadCount) {
		ibmras::common::port::sleep(1);
		IBMRAS_DEBUG_1(debug, "Checking thread count - current [%d]",
				activeThreadCount);
	}IBMRAS_DEBUG(fine, "All active threads now quit");

	connectionManager.removeAllReceivers();
	connectionManager.stop();
	stopPlugins();

	IBMRAS_DEBUG(info, "Agent stop : finish");
}

void Agent::shutdown() {
	std::string str;
	IBMRAS_DEBUG(info, "Agent shutdown : begin");

	IBMRAS_DEBUG(info, bucketList.toString().c_str());
	IBMRAS_DEBUG(info, "Agent shutdown : finish");
}

ibmras::monitoring::connector::Connector* Agent::getConnector(
		const std::string &id) {
	return connectionManager.getConnector(id);
}

ibmras::monitoring::connector::ConnectorManager* Agent::getConnectionManager() {
	return &connectionManager;
}


Agent* Agent::getInstance() {
	return instance;
}

bool Agent::addData(monitordata* data) {
	return bucketList.addData(data);
}

void Agent::threadStop() {
	activeThreadCount--;
	IBMRAS_DEBUG_1(debug, "Number of active threads %d", activeThreadCount);
}


void Agent::setProperties(const ibmras::common::Properties& props) {
	properties.add(props);
}

ibmras::common::Properties Agent::getProperties() {
	return properties;
}

void Agent::setProperty(const std::string& prop, const std::string& value) {
	properties.put(prop, value);
}

std::string Agent::getProperty(const std::string& prop) {
	return properties.get(prop);
}


bool Agent::propertyExists(const std::string& prop) {
	return properties.exists(prop);
}

std::string Agent::getAgentPropertyPrefix() {
	return PROPERTIES_PREFIX;
}

std::string Agent::getAgentProperty(const std::string& agentProp) {
	return getProperty(getAgentPropertyPrefix() + agentProp);
}

void Agent::setAgentProperty(const std::string& agentProp, const std::string& value) {
	setProperty(getAgentPropertyPrefix() + agentProp, value);
}

bool Agent::agentPropertyExists(const std::string& agentProp) {
	return propertyExists(getAgentPropertyPrefix() + agentProp);
}


}
}
} /* end namespace monitoring */

