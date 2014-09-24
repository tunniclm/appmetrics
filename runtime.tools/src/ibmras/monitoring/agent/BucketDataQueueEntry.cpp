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
	this->data = new ibmras::common::alloc<char>(size); /* char array of the data to store */
	next = NULL;
}

BucketDataQueueEntry::~BucketDataQueueEntry(){
	if(data){
		delete data;
	}
}

}
}
} /* end namespace agent */
