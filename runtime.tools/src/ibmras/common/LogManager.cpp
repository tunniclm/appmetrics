/*
 * logging.cpp
 *
 *  Created on: 5 Feb 2014
 *      Author: adam
 */

#include <iostream>
#include <cstdarg>
#include <cstdio>
#include <string>

#include "ibmras/common/LogManager.h"
#include "ibmras/common/Memory.h"

#if defined(_WINDOWS)
#define VPRINT vsprintf_s
#else
#define VPRINT vsprintf
#endif

extern "C" {

DECL void* ibmras_common_LogManager_getLogger(const char* name) {
	return (void*) ibmras::common::LogManager::getLogger(name);
}
}

namespace ibmras {
namespace common {

LogManager* LogManager::instance = NULL;

LOCAL_LOGGER_CALLBACK LogManager::localLogFunc = NULL;

LogManager::LogManager() :
		level(logging::none), localLog(true), lock(NULL) {
	/* do not create a lock in the constructor as it will create a loop with the logging in the port library */
}


void LogManager::processMsg(const std::string &msg) {

	if (localLog) {
		/* local logging is overriding */
		if (localLogFunc) {
			localLogFunc(msg);
		} else {
			std::cerr << msg;
		}
		return;
	}

}

void LogManager::msgHandler(const std::string &message, ibmras::common::logging::Level level,
		Logger* logger) {

	/* logger level has priority over log manager level (which should be considered a default level */
	if ((logger->level >= level) || (instance->level >= level)) {
		instance->processMsg(message);
	}
}

void LogManager::setLevel(logging::Level newlevel) {
	LogManager::level = newlevel;
}

void LogManager::setLevel(const std::string &name, logging::Level newlevel) {
	if (name.compare("global") == 0) {
		setLevel(newlevel);
	} else {
		Logger* logger = getLogger(name);
		logger->level = newlevel;
	}
}

LogManager* LogManager::getInstance() {
	if (!instance) {
		instance = new LogManager;
		instance->lock = new ibmras::common::port::Lock;
	}
	return instance;
}

Logger* LogManager::getLogger(const std::string &name) {
	LogManager* instance = getInstance();
	Logger* logger = instance->findLogger(name);
	if (!logger) { /* logger not found so need to create a new instance and return that */
		logger = new Logger(name, LogManager::msgHandler);
		instance->loggers.push_back(logger);
	}

	return logger;
}

Logger* LogManager::findLogger(const std::string &name) {

	for (std::vector<Logger*>::iterator i = loggers.begin(); i != loggers.end();
			++i) {
		if ((*i)->component == name) {
			return (*i);
		}
	}
	return NULL; /* no match found */
}


}
}