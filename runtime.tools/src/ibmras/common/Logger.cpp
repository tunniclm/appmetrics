/*
 * Logger.cpp
 *
 *  Created on: 23 Jun 2014
 *      Author: robbins
 */

#if defined (_ZOS)
#define _ISOC99_SOURCE
#endif
#include <iostream>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <sstream>

#include "ibmras/common/Logger.h"
#include "ibmras/common/Memory.h"

#if defined(_WINDOWS)
#define VPRINT vsnprintf_s
#else
#define VPRINT vsnprintf
#endif


namespace ibmras {
namespace common {

Logger::Logger(const std::string &name, MSG_HANDLER h) :  level(logging::none), component(name), handler(h) {
}

Logger::~Logger() {
}


/*
 * Define a logger using a macro as it is not possible to pass a variable parameter
 * list onto another function - so the macro prevents code duplication.
 */
#define STR_(x) #x
#define STR(x) STR_(x)

#define LOGGER(lvl) void Logger::lvl(const char* format, ...) \
{\
	std::stringstream str; \
		str << component << " (";\
		str << STR(logging::lvl);\
		str << ") : ";\
		va_list messages;\
		va_start(messages, format);\
		char buffer[1024]; \
		int result = VPRINT(buffer, 1024, format, messages);\
		if(result > 0) {\
			str << buffer;\
		} else {\
			str << "(warning) failed to write replacements for :" << format;\
		}\
		str << std::endl;\
		std::string msg = str.str(); \
		handler(msg.c_str(), ibmras::common::logging::lvl, this);\
}


LOGGER(none)
LOGGER(info)
LOGGER(fine)
LOGGER(finest)
LOGGER(debug)
LOGGER(warning)

}

}
