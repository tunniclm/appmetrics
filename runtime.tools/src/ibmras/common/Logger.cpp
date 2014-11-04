/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
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
#if defined(_ZOS)
#include <unistd.h>
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
#if defined(_ZOS)
#pragma convlit(suspend)
#endif
	if (std::strftime(buffer, sizeof(buffer), "%c", std::localtime(&time))) {
#if defined(_ZOS)
#pragma convlit(resume)
		__etoa(buffer);
#endif
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
	va_end(messages);
	if (result > 0) {
		str << buffer;
	} else {
		str << "(warning) failed to write replacements for :" << format;
	}
	str << std::endl;
	std::string msg = str.str();
#if defined(_ZOS) 
    char * z_str = new char [msg.length()+1];
	if (z_str) {
		std::strcpy (z_str, msg.c_str());
		__a2e_s(z_str);
		handler(z_str, lev, this);
		delete[] z_str;
	}
#else
	handler(msg.c_str(), lev, this);
#endif

}

void Logger::debug(logging::Level lev, const char* format, ...) {
	std::stringstream str;
	header(str, lev, true);
	va_list messages;
	va_start(messages, format);
	char buffer[1024];
	int result = VPRINT(buffer, 1024, format, messages);
	va_end(messages);
	if (result > 0) {
		str << buffer;
	} else {
		str << "(warning) failed to write replacements for :" << format;
	}
	str << std::endl;
	std::string msg = str.str();
#if defined(_ZOS) 
    char * z_str = new char [msg.length()+1];
	if (z_str) {
		std::strcpy (z_str, msg.c_str());
		__a2e_s(z_str);
		handler(z_str, lev, this);
		delete[] z_str;
	}
#else
	handler(msg.c_str(), lev, this);
#endif
}

}

}
