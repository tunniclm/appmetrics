/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
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
