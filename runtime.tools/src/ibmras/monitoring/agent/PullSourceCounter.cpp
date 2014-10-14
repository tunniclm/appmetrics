/*
 * PullSourceCounter.cpp
 *
 *  Created on: 25 Feb 2014
 *      Author: adam
 */

#include "ibmras/monitoring/agent/PullSourceCounter.h"
#include "ibmras/monitoring/agent/Agent.h"

namespace ibmras {
namespace monitoring {
namespace agent {

PullSourceCounter::PullSourceCounter(uint32 interval, PULL_CALLBACK callback, PULL_CALLBACK_COMPLETE complete) {
	this->interval = interval;
	this->callback = callback;
	this->complete = complete;
	current = interval;
	missed = 0;
	queued = false;
}

bool PullSourceCounter::isQueued() {
	return queued;
}

void PullSourceCounter::setQueued(bool value) {
	queued = value;
}

bool PullSourceCounter::isExpired() {
	return current == 0;		/* counter has expired when the current value = 0 */
}

void PullSourceCounter::reset() {
	current = interval;
}

PullSourceCounter PullSourceCounter::operator --(int x) {
	if(current) {
		current--;
	} else {
		missed++;		/* indicate that we have missed a callback */
	}
	return *this;
}

monitordata* PullSourceCounter::getData() {
	monitordata* result = callback();
	monitordata* copy = new monitordata;		/* where we are going to copy the pull data to */
	copy->provID = result->provID;
	copy->sourceID = result->sourceID;
	copy->size = result->size;
	copy->persistent = result->persistent;
	char* buffer = new char[copy->size];
	memcpy(buffer, result->data, copy->size);
	copy->data = buffer;
	current = interval;		/* reset the countdown timer */
	missed = 0;				/* reset missed counter */
	complete(result);
	return copy;
}

void PullSourceCounter::init(uint32 value, PULL_CALLBACK callback, PULL_CALLBACK_COMPLETE complete) {
	interval = value;
	this->callback = callback;
	this->complete = complete;
	current = 0;
}


}
}
}	/* end of namespace agent */

