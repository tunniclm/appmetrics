/*
 * LogManager.h
 *
 *  Created on: 5 Feb 2014
 *      Author: adam
 */

#ifndef ibmras_common_logmanager_h
#define ibmras_common_logmanager_h

#include <iostream>
#include <string>
#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/common/Logger.h"
#include "ibmras/common/port/Lock.h"

namespace ibmras {
	namespace common {

		typedef void* (*LOCAL_LOGGER_CALLBACK)(const std::string &msg); /* shortcut definition for the local log callback */

		/*
		 * Common logging functions
		 */
		class DECL LogManager {
		public:
			static LogManager* getInstance();

			logging::Level level; /* will default to 0 which is 'none' in the log level enum */
			bool localLog; /* setting this to true will push all output to local stderr */
			static LOCAL_LOGGER_CALLBACK localLogFunc; /* optional function to invoke for local callbacks */
			static void msgHandler(const std::string &msg, ibmras::common::logging::Level level, Logger* logger); /* common message processing */

			static Logger* getLogger(const std::string &name); /* return instance of the logger */

			void setLevel(logging::Level level); /* set the log level for all components */
			void setLevel(const std::string &name, logging::Level level); /* set the log level for a named component logger */
		protected:
			LogManager();

		private:
			static LogManager* instance;
			std::vector<Logger*> loggers;
			ibmras::common::port::Lock* lock; /* lock to prevent spills whilst publishing/sending */

			void processMsg(const std::string& msg); /* common message processing */

			Logger* findLogger(const std::string &name); /* find a named logger */
		};
	}
}
#endif /* ibmras_common_logmanager_h */
