#include "ibmras/monitoring/agent/Bucket.h"
#include "ibmras/common/common.h"
#include "ibmras/common/logging.h"
#include <sstream>

namespace ibmras {
namespace monitoring {
namespace agent {
namespace bucket {
IBMRAS_DEFINE_LOGGER("Bucket")
;
}
using namespace bucket;

Bucket::Bucket(uint32 provID, uint32 sourceID, uint32 capacity,
		const std::string &uniqueID) {
	this->provID = provID;
	this->sourceID = sourceID;
	this->capacity = capacity;
	this->uniqueID = uniqueID;
	count = 0;
	size = 0;
	head = NULL;
	tail = NULL;
	masterID = 1;
	lock = new ibmras::common::port::Lock;
	IBMRAS_DEBUG_1(fine, "Bucket created for : %s", uniqueID.c_str());
}

Bucket::BucketData::~BucketData() {
	if (entry) {
		delete entry;
	}
}


void Bucket::spill(uint32 entrysize) {
	BucketData* bdata; /* used to manage the bucket data */
	uint32 i = 0;
	IBMRAS_DEBUG_3(debug, "Bucket %s [%d:%d] capacity reached, spilling contents", uniqueID.c_str(), provID,
			sourceID);
	/* no room left so need to free up items from the bucket */
	IBMRAS_DEBUG_3(debug, "Bucket %s size %d capacity %d", uniqueID.c_str(), size, capacity);

	IBMRAS_DEBUG_4(debug, "Bucket %s head %p tail %p  count %d" , uniqueID.c_str(), head, tail, count);
	if (!lock->acquire()) {
		if (!lock->isDestroyed()) {
			BucketData *cursor = head;
			BucketData *prev = NULL;
			while ((entrysize > (capacity - size)) && (cursor != NULL)) {
				if (!cursor->entry->persistentData) {
					bdata = cursor;
					size -= bdata->entry->size;
					count--;
					i++;
					delete bdata;
					if (prev == NULL) {
						head = cursor->next;
					} else {
						prev->next = cursor->next;
					}
				} else {
					prev = cursor;
					IBMRAS_DEBUG_2(debug, "Bucket %s: skipping persistent entry %d", uniqueID.c_str(), cursor->id);
				}
				cursor = cursor->next;
			}
			if (!head) {
				tail = NULL; /* emptied the queue so there is no tail now either */
			}
			lock->release();
		}
	} IBMRAS_DEBUG_1(info, "Removed %d entries from the bucket", i); IBMRAS_DEBUG_4(debug, "Bucket stats [%d:%d] : count = %d, size = %d", provID,
			sourceID, count, size);
}

bool Bucket::add(BucketDataQueueEntry* entry, ibmras::monitoring::connector::Connector &con) {
	BucketData* bdata; /* used to manage the bucket data */
	if ((entry->provID != provID) || (entry->sourceID != sourceID)) {
		IBMRAS_DEBUG_4(info,
				"Wrong data sent to bucket : received %d:%d, expected %d, %d",
				entry->provID, entry->sourceID, provID, sourceID);
		return false; /* data not added as provider and source IDs do not match */
	}
	if (entry->size > capacity) {
		IBMRAS_DEBUG_2(info, "Data not added as the size was %d, but capacity is %d",
				entry->size, capacity);
		return false; /* data is larger than capacity */
	}
	if (entry->size > (capacity - size)) {
		spill(entry->size);
	}
	bdata = new BucketData;
	bdata->id = masterID++;
	bdata->entry = entry;
	bdata->next = NULL;
	if (!lock->acquire()) {
		if (!lock->isDestroyed()) {
			con.sendMessage(uniqueID, entry->size,
					entry->data->ptr());

			if (tail) {
				tail->next = bdata; /* add new entry to tail */
				tail = bdata; /* make a new tail */
			} else {
				head = bdata;
				tail = bdata;
			}
			count++;
			size += entry->size;
			lock->release();
		}
	} IBMRAS_DEBUG_4(debug,
			"Bucket data [%s] : data size = %d, bucket size = %d, count = %d",
			uniqueID.c_str(), entry->size, size, count);
	return true; /* data added to bucket */
}

uint32 Bucket::getNextData(uint32 id, int32 &dataSize, void* &data) {
	uint32 returnId = id;
	if (!lock->acquire()) {
		if (!lock->isDestroyed()) {
			uint32 requestedSize = dataSize;
			dataSize = 0;

			BucketData* current = head;
			while (current) {
				if (current->id > id) {

					// Calculate size to return
					BucketData* dataToSend = current;
					uint32 bufferSize = 0;
					if (requestedSize == 0) {
						bufferSize = current->entry->size;
					} else {

						while (dataToSend) {
							bufferSize += dataToSend->entry->size;
							if (requestedSize > 0
									&& bufferSize > requestedSize) {
								break;
							}
							dataToSend = dataToSend->next;
						}

					}
					// Allocate buffer
					char* buffer = new char[bufferSize];
					data = buffer;

					dataToSend = current;
					while (dataToSend) {
						if ((dataToSend->entry->size + dataSize) > bufferSize) {
							break;

						}
						// copy data to buffer
						char* dataPtr = dataToSend->entry->data->ptr();
						memcpy(buffer + dataSize, dataPtr,
								dataToSend->entry->size);
						dataSize += dataToSend->entry->size;
						returnId = dataToSend->id;
						dataToSend = dataToSend->next;
					}

					break;
				}
				current = current->next;
			}
			lock->release();
		}
	}

	return returnId;
}

void Bucket::republish(const std::string &topicPrefix,
		ibmras::monitoring::connector::Connector &con) {
	IBMRAS_DEBUG(fine, "in Bucket::republish()");
	if (!lock->acquire()) {
		if (!lock->isDestroyed()) {

			std::string messageTopic = topicPrefix + uniqueID;
			BucketData* current = head;
			while (current) {
					IBMRAS_DEBUG_2(fine, "publishing message to %s of %d bytes",
							uniqueID.c_str(), current->entry->size);
					con.sendMessage(messageTopic, current->entry->size,
							current->entry->data->ptr());
				current = current->next;
			}
			con.sendMessage(messageTopic, 0, NULL);
			lock->release();
		}
	}
}

std::string Bucket::toString() {
	std::stringstream str;
	str << "Bucket [" << common::itoa(provID) << ":" << common::itoa(sourceID)
			<< "], capacity = " << common::itoa(capacity) << ", count = "
			<< common::itoa(count) << ", used = " << common::itoa(size)
			<< std::endl;
	return str.str();
}

uint32 Bucket::getProvID() {
	return provID;
}

uint32 Bucket::getSourceID() {
	return sourceID;
}

std::string Bucket::getUniqueID() {
	return uniqueID;
}

}
}
} /* end namespace agent */
