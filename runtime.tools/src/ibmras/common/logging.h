 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_common_logging_h
#define ibmras_common_logging_h

#include "ibmras/common/LogManager.h"
extern "C" {
	void* ibmras_common_LogManager_getLogger(const char* name);
}

#define IBMRAS_DECLARE_LOGGER ibmras::common::Logger* logger;
#define IBMRAS_ASSIGN_LOGGER(name) {logger = (ibmras::common::Logger*)ibmras_common_LogManager_getLogger( name );}
#define IBMRAS_DEFINE_LOGGER(name) ibmras::common::Logger* logger = (ibmras::common::Logger*)ibmras_common_LogManager_getLogger( name );

/* Define logging macros */
#define IBMRAS_LOG(LOGLEVEL, MSG) {if (logger->level >= ibmras::common::logging::LOGLEVEL) { logger->log(ibmras::common::logging::LOGLEVEL, MSG);}}
#define IBMRAS_LOG_1(LOGLEVEL, MSG, INSERT1) {if (logger->level >= ibmras::common::logging::LOGLEVEL) { logger->log(ibmras::common::logging::LOGLEVEL, MSG, INSERT1);}}
#define IBMRAS_LOG_2(LOGLEVEL, MSG, INSERT1, INSERT2) {if (logger->level >= ibmras::common::logging::LOGLEVEL) { logger->log(ibmras::common::logging::LOGLEVEL, MSG, INSERT1, INSERT2);}}
#define IBMRAS_LOG_3(LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3) {if (logger->level >= ibmras::common::logging::LOGLEVEL) { logger->log(ibmras::common::logging::LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3);}}
#define IBMRAS_LOG_4(LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3, INSERT4) {if (logger->level >= ibmras::common::logging::LOGLEVEL) { logger->log(ibmras::common::logging::LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3, INSERT4);}}

/* Define debug logging macros */
#if defined(IBMRAS_DEBUG_LOGGING)
#define IBMRAS_DEBUG(LOGLEVEL, MSG) {if (logger->debugLevel >= ibmras::common::logging::LOGLEVEL) { logger->debug(ibmras::common::logging::LOGLEVEL, MSG);}}
#define IBMRAS_DEBUG_1(LOGLEVEL, MSG, INSERT1) {if (logger->debugLevel >= ibmras::common::logging::LOGLEVEL) { logger->debug(ibmras::common::logging::LOGLEVEL, MSG, INSERT1);}}
#define IBMRAS_DEBUG_2(LOGLEVEL, MSG, INSERT1, INSERT2) {if (logger->debugLevel >= ibmras::common::logging::LOGLEVEL) { logger->debug(ibmras::common::logging::LOGLEVEL, MSG, INSERT1, INSERT2);}}
#define IBMRAS_DEBUG_3(LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3) {if (logger->debugLevel >= ibmras::common::logging::LOGLEVEL) { logger->debug(ibmras::common::logging::LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3);}}
#define IBMRAS_DEBUG_4(LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3, INSERT4) {if (logger->debugLevel >= ibmras::common::logging::LOGLEVEL) { logger->debug(ibmras::common::logging::LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3, INSERT4);}}
#else
#define IBMRAS_DEBUG(LOGLEVEL, MSG)
#define IBMRAS_DEBUG_1(LOGLEVEL, MSG, INSERT1)
#define IBMRAS_DEBUG_2(LOGLEVEL, MSG, INSERT1, INSERT2)
#define IBMRAS_DEBUG_3(LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3)
#define IBMRAS_DEBUG_4(LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3, INSERT4)

#endif /* IBMRAS_DEBUG_LOGGING */

#endif /* ibmras_common_logging_h */
