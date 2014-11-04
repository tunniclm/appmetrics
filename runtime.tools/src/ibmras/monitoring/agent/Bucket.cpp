 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

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
	masterID = 0;
	lock = new ibmras::common::port::Lock;
	lastPublish = 0;
	IBMRAS_DEBUG_1(fine, "Bucket created for : %s", uniqueID.c_str());
}

Bucket::BucketData::~BucketData() {
	if (entry) {
		delete entry;
	}
}

void Bucket::publish(ibmras::monitoring::connector::Connector &con) {
	if (!lock->acquire()) {
		if (!lock->isDestroyed()) {
			BucketData* current = head;
			while (current) {
				if ((current->id > lastPublish) || !lastPublish) {
					IBMRAS_DEBUG_2(fine, "publishing message to %s of %d bytes",
							uniqueID.c_str(), current->entry->size);
					con.sendMessage(uniqueID, current->entry->size,
							current->entry->data->ptr());
					lastPublish = current->id;
				}
				current = current->next;
			}
			lock->release();
		}
	}
}

bool Bucket::spill(uint32 entrysize) {

	BucketData* bdata; /* used to manage the bucket data */
	uint32 i = 0;

	BucketData *cursor = head;
	BucketData *prev = NULL;
	while ((entrysize > (capacity - size)) && (cursor != NULL) && (cursor->id <= lastPublish)) {
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
		}
		cursor = cursor->next;
	}
	if (!head) {
		tail = NULL; /* emptied the queue so there is no tail now either */
	}

	if (head && (entrysize > (capacity - size)) ) {
		// No room within capacity
		return false;
	}


	IBMRAS_DEBUG_1(debug, "Removed %d entries from the bucket", i);

	IBMRAS_DEBUG_4(debug, "Bucket stats [%d:%d] : count = %d, size = %d", provID,
			sourceID, count, size);


	return true;

}

bool Bucket::add(monitordata* data) {

	if ((data->provID != provID) || (data->sourceID != sourceID)) {
		IBMRAS_DEBUG_4(info,
				"Wrong data sent to bucket : received %d:%d, expected %d, %d",
				data->provID, data->sourceID, provID, sourceID);
		return false; /* data not added as provider and source IDs do not match */
	}
    bool added = false;

	if (!lock->acquire()) {
		if (!lock->isDestroyed()) {
			if (spill(data->size)) {
				BucketData* bdata = new BucketData;
				bdata->id = ++masterID;
				bdata->entry = new BucketDataQueueEntry(data);
				bdata->next = NULL;

				if (tail) {
					tail->next = bdata; /* add new entry to tail */
					tail = bdata; /* make a new tail */
				} else {
					head = bdata;
					tail = bdata;
				}
				count++;
				size += data->size;
				added = true;
			} else {
				IBMRAS_DEBUG_2(warning, "No room in bucket %s for data of size %d", uniqueID.c_str(), data->size);
			}
			lock->release();
		}
	}


	IBMRAS_DEBUG_4(debug,
			"Bucket data [%s] : data size = %d, bucket size = %d, count = %d",
			uniqueID.c_str(), data->size, size, count);
	return added; /* data added to bucket */
}

uint32 Bucket::getNextData(uint32 id, int32 &dataSize, void* &data,
		uint32 &droppedCount) {
	uint32 returnId = id;
	droppedCount = 0;
	if (!lock->acquire()) {
		if (!lock->isDestroyed()) {
			uint32 requestedSize = dataSize;
			dataSize = 0;

			BucketData* current = head;
			while (current) {
				if (current->id > id) {
					droppedCount = current->id - (id + 1);

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
							if (dataToSend->next) {
								droppedCount += (dataToSend->next->id
										- (dataToSend->id + 1));
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

/*
 * NOTE This method has NO locking as it is intended to be called by the thread that
 * already owns the bucket lock, ie from connectors called by the publish method
 */
uint32 Bucket::getNextPersistentData(uint32 id, uint32& dataSize, void*& data) {
	uint32 returnId = id;

	IBMRAS_DEBUG(debug, "in Bucket::getNextPersistentData()");

	IBMRAS_DEBUG(debug, "in Bucket::getNextPersistentData() lock acquired");
	dataSize = 0;
	data = NULL;


	BucketData* current = head;
	while (current && current->id <= lastPublish) {
		if (current->id > id && current->entry->persistentData) {
			IBMRAS_DEBUG_1(debug, "in Bucket::getNextPersistentData() persistent entry found id", current->id);
			// Allocate buffer
			dataSize = current->entry->size;
			data = new char[current->entry->size];

			// copy data to buffer
			memcpy(data, current->entry->data->ptr(), current->entry->size);
			returnId = current->id;
			break;
		}
		current = current->next;
	}
	lock->release();

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
				if ((current->id <= lastPublish)) {
					IBMRAS_DEBUG_2(fine, "publishing message to %s of %d bytes",
							uniqueID.c_str(), current->entry->size);
					con.sendMessage(messageTopic, current->entry->size,
							current->entry->data->ptr());
				}
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
