/*
 *
 *  Created on: 13 Feb 2014
 *      Author: adam
 */

#ifndef ibmras_monitoring_agent_monitordataqueue_h
#define ibmras_monitoring_agent_monitordataqueue_h

/*
 * Header file for the buckets that are used by the agent for holding data
 * pushed or pulled from plugins. Buckets are managed and will not grow
 * beyond a pre-set maximum size.
 */

#include "ibmras/common/port/ThreadData.h"
#include "ibmras/monitoring/Monitoring.h"		/* uses the monitor data structure as what is stored in the bucket */
#include "ibmras/monitoring/agent/BucketList.h"
#include "ibmras/monitoring/agent/BucketDataQueueEntry.h"


#define DEFAULT_CAPACITY 1048576	/* default bucket capacity = 1MB */

namespace ibmras {
namespace monitoring {
namespace agent {
/* holds the data pushed from plugins. This queue will potentially have many threads adding
 * data at the same time.
 */
class MonitorDataQueue {
public:
	void add(monitordata* data);
	MonitorDataQueue();
	uint32 getSize();
	void drain(BucketList& buckets);			/* drain the monitor queue into buckets */
	void shutdown(BucketList& buckets);		/* shutdown the queue and don't accept any more entries */
private:
	uint32 highwater;
	BucketDataQueueEntry* drainto;
	BucketDataQueueEntry* head;
	BucketDataQueueEntry* tail;
	uint32 count;
	ibmras::common::port::Lock* dataqLock;
};
}
}

} 	/* end of namespace agent */

#endif /* ibmras_monitoring_agent_monitordataqueue_h */
