 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#ifndef ibmras_monitoring_bucketlist_h
#define ibmras_monitoring_bucketlist_h

#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/port/Lock.h"
#include "ibmras/monitoring/connector/Connector.h"
#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/monitoring/agent/Bucket.h"

namespace ibmras {
namespace monitoring {
namespace agent {


/* the list of all available buckets */
class DECL BucketList {
public:
	bool add(Bucket* bucket);					/* add a bucket to the list */
	bool add(std::vector<Bucket*> buckets);		/* add multiple buckets in one go */
	Bucket* findBucket(uint32 provID, uint32 sourceID);	/* find a bucket for a given provider */
	Bucket* findBucket(const std::string &uniqueID);
	void publish(ibmras::monitoring::connector::Connector &con); /* publish all bucket contents */
	void republish(const std::string &prefix, ibmras::monitoring::connector::Connector &con);
	bool addData(monitordata* data);
	std::vector<std::string> getIDs();
	std::string toString();						/* debug / log string version */
private:
	std::vector<Bucket*> buckets;				/* start of the list of buckets */
};
}
}
} /* end namespace agent */

#endif /* ibmras_monitoring_bucketlist_h */
