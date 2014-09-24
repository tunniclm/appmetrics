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

	void none(const char* format, ...); /* variable number of parameters should be string messages */
	void info(const char* format, ...); /* variable number of parameters should be string messages */
	void debug(const char* format, ...); /* variable number of parameters should be string messages */
	void fine(const char* format, ...);
	void finest(const char* format, ...);
	void warning(const char* format, ...);
	logging::Level level; /* level that the logger is operating at */
	std::string component;
private:
	MSG_HANDLER handler;
};

} /* namespace common */
} /* namespace ibmras */
#endif /* ibmras_common_logger_h */
