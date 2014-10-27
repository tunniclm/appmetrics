 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include "ibmras/monitoring/agent/BucketDataQueueEntry.h"
#include "ibmras/common/Logger.h"

namespace ibmras {
namespace monitoring {
namespace agent {

BucketDataQueueEntry::BucketDataQueueEntry(monitordata* data) {
	this->persistentData = data->persistent;
	this->provID = data->provID;
	this->sourceID = data->sourceID;
	this->size = data->size;
	if (size > 0 && data->data != NULL) {
		this->data = new ibmras::common::alloc<char>(size); /* char array of the data to store */
		this->data->copyfrom(data->data);
	}
	next = NULL;
}

BucketDataQueueEntry::~BucketDataQueueEntry() {
	if (data) {
		delete data;
	}
}

}
}
} /* end namespace agent */
