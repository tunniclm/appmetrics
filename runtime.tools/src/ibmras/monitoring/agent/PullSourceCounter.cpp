/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include "ibmras/monitoring/agent/PullSourceCounter.h"
#include "ibmras/monitoring/agent/Agent.h"

namespace ibmras {
namespace monitoring {
namespace agent {

PullSourceCounter::PullSourceCounter(uint32 interval, PULL_CALLBACK callback,
		PULL_CALLBACK_COMPLETE complete) {
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
	return current == 0; /* counter has expired when the current value = 0 */
}

void PullSourceCounter::expire() {
	current = 0;
}

void PullSourceCounter::reset() {
	current = interval;
}

PullSourceCounter PullSourceCounter::operator --(int x) {
	if (current) {
		current--;
	} else {
		missed++; /* indicate that we have missed a callback */
	}
	return *this;
}

monitordata* PullSourceCounter::getData() {
	monitordata* result = callback();
	current = interval; /* reset the countdown timer */
	missed = 0; /* reset missed counter */
	return result;

}

void PullSourceCounter::init(uint32 value, PULL_CALLBACK callback,
		PULL_CALLBACK_COMPLETE complete) {
	interval = value;
	this->callback = callback;
	this->complete = complete;
	current = 0;
}

}
}
} /* end of namespace agent */

