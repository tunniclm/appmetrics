/*
 * ThreadData.cpp
 *
 *  Created on: 26 Feb 2014
 *      Author: adam
 */

/*
 * Implementation of common port library classes
 */

#include "ibmras/common/port/ThreadData.h"


namespace ibmras {

namespace common {

namespace port {


ThreadData::ThreadData(THREAD_CALLBACK callback) {
	this->callback = callback;
	handle = NULL;
	args = NULL;
}

void ThreadData::setArgs(void* args) {
	this->args = args;
}

void* ThreadData::getArgs() {
	return args;
}

THREAD_CALLBACK ThreadData::getCallback() {
	return callback;
}

}
}
} /* end of namespace port */
