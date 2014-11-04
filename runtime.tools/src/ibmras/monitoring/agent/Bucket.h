 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#ifndef ibmras_monitoring_bucket_h
#define ibmras_monitoring_bucket_h

#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/port/Lock.h"
#include "ibmras/monitoring/connector/Connector.h"
#include "ibmras/common/Memory.h"
#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/monitoring/agent/BucketDataQueueEntry.h"

namespace ibmras {
namespace monitoring {
namespace agent {

/* a bucket holds a set of monitor data */
class DECL Bucket {

public:
	Bucket(uint32 provID, uint32 sourceID, uint32 capacity, const std::string& uniqueID);
	bool add(monitordata* entry);			/* adds monitor data to the bucket */
	std::string toString();			/* debug / log string version */
	uint32 getProvID();
	uint32 getSourceID();
	std::string getUniqueID();
	void publish(ibmras::monitoring::connector::Connector &con);				/* publish bucket contents to the connector manager */
	uint32 getNextData(uint32 id, int32 &size,	void* &data, uint32 &droppedCount);
	uint32 getNextPersistentData(uint32 id, uint32 &size, void* &data);
	void republish(const std::string &topicPrefix, ibmras::monitoring::connector::Connector &con);
private:
	bool spill(uint32 size);	/* spill bucket contents until there is the requested space */

	/* bucket data builds on the monitor data to add control meta-data. It also removes any
	 * unnecessary or repeated data elements
	 */
	class BucketData {
	public:
		virtual ~BucketData();
		uint32 id;				/* used by clients to request ranges of data */
		BucketDataQueueEntry* entry;	/* data on the queue */
		BucketData* next;		/* next item in the bucket or null if this is the last item */
	};
	uint32 provID;
	uint32 sourceID;
	std::string uniqueID;  /*Name of the uniqueID plugin providing the name */
	BucketData* head;		/* when a bucket over flows then items are removed from the head */
	BucketData* tail;		/* when items are added to the bucket they are added to the tail */
	uint32 lastPublish;/* the last entry published */
	uint32 capacity;		/* maximum capacity for the bucket in bytes */
	uint32 size;			/* current size of the bucket */
	uint32 count;			/* number of items in the bucket */
	uint32 masterID;		/* master ID for items placed in the bucket */
	ibmras::common::port::Lock* lock;		/* lock to prevent spills whilst publishing/sending */
};


}
}
} /* end namespace agent */

#endif /* ibmras_monitoring_bucket_h */
