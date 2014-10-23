/*
 * Logger.h
 *
 *  Created on: 23 Jun 2014
 *      Author: robbins
 */

#ifndef ibmras_common_logger_h
#define ibmras_common_logger_h

#include "ibmras/export.h"

#include <string>

namespace ibmras {
namespace common {

namespace logging {
/*
 * Enumeration levels to set for the logger
 */
enum Level {
	/* log levels are ranked with debug being the most verbose */
	none, warning, info, fine, finest, debug
};

}

class DECL Logger;

typedef void (*MSG_HANDLER)(const std::string &msg, logging::Level Level, Logger* logger); /* common message processing */

class DECL Logger {
public:
	Logger(const std::string &name, MSG_HANDLER h);
	virtual ~Logger();

	void log(logging::Level lev, const char* format, ...); /* variable number of parameters should be string messages */
	void debug(logging::Level lev, const char* format, ...); /* variable number of parameters should be string messages */

	logging::Level level; /* level that the logger is operating at */
	logging::Level debugLevel; /* level that the logger is operating at */
	std::string component;

private:
	MSG_HANDLER handler;

	void header(std::stringstream &str, logging::Level lev, bool debug=false);
};

} /* namespace common */
} /* namespace ibmras */
#endif /* ibmras_common_logger_h */
