 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#ifndef ibmras_monitoring_bucketdataqueueentry_h
#define ibmras_monitoring_bucketdataqueueentry_h

#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/port/Lock.h"
#include "ibmras/monitoring/connector/Connector.h"
#include "ibmras/common/Memory.h"
#include "ibmras/monitoring/Monitoring.h"

namespace ibmras {
namespace monitoring {
namespace agent {

class DECL BucketDataQueueEntry {
public:
	BucketDataQueueEntry(monitordata *);
	virtual ~BucketDataQueueEntry();
	uint32 provID;				/* provider ID, previously allocated during the source registration */
	uint32 sourceID;			/* source ID, previously supplied by the source during registration */
	uint32 size;				/* amount of data being provided */
	bool persistentData;
	ibmras::common::alloc<char> *data;	/* char array of the data to store */
	BucketDataQueueEntry* next;
};


}
}
} /* end namespace agent */

#endif /* ibmras_monitoring_bucketdataqueueentry_h */
