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

#include <ctime>

#include "ibmras/common/Logger.h"
#include "ibmras/common/Memory.h"

#if defined(_WINDOWS)
#define VPRINT vsnprintf_s
#else
#define VPRINT vsnprintf
#endif

namespace ibmras {
namespace common {

Logger::Logger(const std::string &name, MSG_HANDLER h) :
		level(logging::none), debugLevel(logging::none), component(name), handler(h) {
}

Logger::~Logger() {
}

void Logger::header(std::stringstream &str, logging::Level lev, bool debug) {
	std::time_t time = std::time(NULL);
	char buffer[100];
	if (std::strftime(buffer, sizeof(buffer), "%c", std::localtime(&time))) {
		str << '[' << buffer << ']';
	}
	str << " com.ibm.diagnostics.healthcenter." << component;

	if (debug) {
		str << ".debug";
	}

	switch (lev) {
	case logging::info:
		str << " INFO: ";
		break;
	case logging::warning:
		str << " WARNING: ";
		break;
	case logging::fine:
		str << " FINE: ";
		break;
	case logging::finest:
		str << " FINEST: ";
		break;
	case logging::debug:
		str << " DEBUG: ";
		break;
	default:
		str << " ";
		break;
	}
}

void Logger::log(logging::Level lev, const char* format, ...) {
	std::stringstream str;
	header(str, lev);
	va_list messages;
	va_start(messages, format);
	char buffer[1024];
	int result = VPRINT(buffer, 1024, format, messages);
	if (result > 0) {
		str << buffer;
	} else {
		str << "(warning) failed to write replacements for :" << format;
	}
	str << std::endl;
	std::string msg = str.str();
	handler(msg.c_str(), lev, this);
}

void Logger::debug(logging::Level lev, const char* format, ...) {
	std::stringstream str;
	header(str, lev, true);
	va_list messages;
	va_start(messages, format);
	char buffer[1024];
	int result = VPRINT(buffer, 1024, format, messages);
	if (result > 0) {
		str << buffer;
	} else {
		str << "(warning) failed to write replacements for :" << format;
	}
	str << std::endl;
	std::string msg = str.str();
	handler(msg.c_str(), lev, this);
}

}

}
