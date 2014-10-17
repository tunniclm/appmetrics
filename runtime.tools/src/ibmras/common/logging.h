/*
 * logging.h
 *
 *  Created on: 14 Aug 2014
 *      Author: robbins
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

#if defined(_ZOS)
#pragma convlit(suspend)
#endif

/* Define logging macros */
#define IBMRAS_LOG(LOGLEVEL, MSG) {if (logger->level >= ibmras::common::logging::LOGLEVEL) { logger->LOGLEVEL(MSG);}}
#define IBMRAS_LOG_1(LOGLEVEL, MSG, INSERT1) {if (logger->level >= ibmras::common::logging::LOGLEVEL) { logger->LOGLEVEL(MSG, INSERT1);}}
#define IBMRAS_LOG_2(LOGLEVEL, MSG, INSERT1, INSERT2) {if (logger->level >= ibmras::common::logging::LOGLEVEL) { logger->LOGLEVEL(MSG, INSERT1, INSERT2);}}
#define IBMRAS_LOG_3(LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3) {if (logger->level >= ibmras::common::logging::LOGLEVEL) { logger->LOGLEVEL(MSG, INSERT1, INSERT2, INSERT3);}}
#define IBMRAS_LOG_4(LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3, INSERT4) {if (logger->level >= ibmras::common::logging::LOGLEVEL) { logger->LOGLEVEL(MSG, INSERT1, INSERT2, INSERT3, INSERT4);}}

/* Define debug logging macros */
#if defined(IBMRAS_DEBUG_LOGGING)
#define IBMRAS_DEBUG(LOGLEVEL, MSG) {IBMRAS_LOG(LOGLEVEL, MSG);}
#define IBMRAS_DEBUG_1(LOGLEVEL, MSG, INSERT1) {IBMRAS_LOG_1(LOGLEVEL, MSG, INSERT1);}
#define IBMRAS_DEBUG_2(LOGLEVEL, MSG, INSERT1, INSERT2) {IBMRAS_LOG_2(LOGLEVEL, MSG, INSERT1, INSERT2);}
#define IBMRAS_DEBUG_3(LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3) {IBMRAS_LOG_3(LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3);}
#define IBMRAS_DEBUG_4(LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3, INSERT4) {IBMRAS_LOG_4(LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3, INSERT4);}
#else
#define IBMRAS_DEBUG(LOGLEVEL, MSG)
#define IBMRAS_DEBUG_1(LOGLEVEL, MSG, INSERT1)
#define IBMRAS_DEBUG_2(LOGLEVEL, MSG, INSERT1, INSERT2)
#define IBMRAS_DEBUG_3(LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3)
#define IBMRAS_DEBUG_4(LOGLEVEL, MSG, INSERT1, INSERT2, INSERT3, INSERT4)

#endif /* IBMRAS_DEBUG_LOGGING */

#if defined(_ZOS)
#pragma convlit(resume)
#endif

#endif /* ibmras_common_logging_h */
