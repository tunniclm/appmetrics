/*
 * bucket.cpp
 *
 *  Created on: 13 Feb 2014
 *      Author: adam
 */

#include "ibmras/monitoring/agent/MonitorDataQueue.h"
#include "ibmras/common/logging.h"

namespace ibmras {
namespace monitoring {
namespace agent {

extern IBMRAS_DECLARE_LOGGER;

MonitorDataQueue::MonitorDataQueue() {
	highwater = 0;
	drainto = NULL;
	head = NULL;
	tail = NULL;
	count = 0;
	dataqLock = new ibmras::common::port::Lock;
}

void MonitorDataQueue::add(monitordata* data) {
	if (data == NULL) {
		IBMRAS_DEBUG(info, "MonitorDataQueue::add entry data was NULL");
		return;
	}
	IBMRAS_DEBUG_1(info, "MonitorDataQueue::add entry data->size=%d",data->size);
	if (data != NULL & (data->size > 0 )) {
		BucketDataQueueEntry* entry = new BucketDataQueueEntry(data);
		entry->data->copyfrom(data->data);
		entry->next = NULL;
		if (!dataqLock->acquire()) {
			if (!dataqLock->isDestroyed()) {
				if (tail) {
					tail->next = entry;
					tail = entry;
				} else {
					head = tail = entry;
				}
				count++;
				dataqLock->release();
			}
		} else {
			IBMRAS_DEBUG(warning, "***Failed to acquire lock\n");
		}
		if (count > highwater) {
			highwater = count; /* record high water mark for the queue */
		}
		IBMRAS_DEBUG(info, "MonitorDataQueue::add exit\n");
	} else {
		IBMRAS_DEBUG(info, "MonitorDataQueue::add exit no data\n");
	}
}

uint32 MonitorDataQueue::getSize() {
	return count;
}

void MonitorDataQueue::drain(BucketList& buckets) {
	BucketDataQueueEntry* entry;
	IBMRAS_DEBUG(info, "Draining monitor queue");
	drainto = tail; /* drain the queue up to the current tail */
	while (head && (head != drainto)) {
		if (buckets.addData(head)) {
			entry = head;
			head = head->next;
			count--;
		} else {
			IBMRAS_DEBUG_2(info,
					"Warning : bucket not found for data from plugin %d:%d, data discarded",
					head->provID, head->sourceID);
		}
	}
}

void MonitorDataQueue::shutdown(BucketList& buckets) {
	IBMRAS_DEBUG(info, "Releasing locks");
	dataqLock->acquire();
	drain(buckets);
	dataqLock->destroy();
	if (count) {
		IBMRAS_DEBUG_1(info, "%d items were discarded from the monitoring queue", count);
	}
}

}
}
} /* end of namespace agent */

