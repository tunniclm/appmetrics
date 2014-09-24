#include "ibmras/monitoring/agent/BucketList.h"
#include "ibmras/common/logging.h"
#include <sstream>

namespace ibmras {
namespace monitoring {
namespace agent {

namespace bucket {
extern IBMRAS_DECLARE_LOGGER;
}
using namespace bucket;





bool BucketList::add(Bucket* bucket) {
	IBMRAS_DEBUG(fine,  "BucketList::add(Bucket* bucket)  adding a bucket");
	buckets.push_back(bucket);
	return true;
}

std::string BucketList::toString() {
	std::stringstream str;
	str << "Bucket list : start" << std::endl;
	for (std::vector<Bucket*>::iterator i = buckets.begin(); i != buckets.end();
			++i) {
		str << (*i)->toString();
	}
	return str.str();
}

bool BucketList::add(std::vector<Bucket*> buckets) {
	IBMRAS_DEBUG(fine,
			"BucketList::add(std::vector<Bucket*> buckets)  adding a bucket");
	bool result = true;
	for (uint32 i = 0; i < buckets.size(); i++) {
		add(buckets[i]);
	}
	return result; /* cumulative result of all additions */
}

Bucket* BucketList::findBucket(uint32 provID, uint32 sourceID) {
	for (uint32 i = 0; i < buckets.size(); i++) {
		Bucket* b = buckets[i];
		if ((b->getProvID() == provID) && (b->getSourceID() == sourceID)) {
			return b; /* found a matching bucket */
		}
	}
	return NULL; /* did not find a matching bucket */
}

Bucket* BucketList::findBucket(const std::string &uniqueID) {
	for (uint32 i = 0; i < buckets.size(); i++) {
		Bucket* b = buckets[i];

		if (uniqueID.compare((b->getUniqueID())) == 0) {
			return b; /* found a matching bucket */
		}
	}
	return NULL; /* did not find a matching bucket */
}

void BucketList::publish(ibmras::monitoring::connector::Connector &con) {
	for (uint32 i = 0; i < buckets.size(); i++) {
		Bucket* b = buckets[i];
		b->publish(con);
	}
}

void BucketList::republish(const std::string &prefix, ibmras::monitoring::connector::Connector &con) {
	for (uint32 i = 0; i < buckets.size(); i++) {
		Bucket* b = buckets[i];
		b->republish(prefix, con);
	}
}

bool BucketList::addData(BucketDataQueueEntry* data) {
	Bucket* b = findBucket(data->provID, data->sourceID);
	if (b) {
		b->add(data); /* found a matching bucket so add the data*/
		return true;
	} else {
		IBMRAS_DEBUG_2(warning,  "Attempted to add data to missing bucket [%d:%d]",
				data->provID, data->sourceID);
	}
	return false;
}

std::vector<std::string> BucketList::getIDs() {
	std::vector<std::string> ids;

	for (std::vector<Bucket*>::iterator i = buckets.begin(); i != buckets.end();
			++i) {
		ids.push_back(((*i)->getUniqueID()));
	}

	return ids;
}

}
}
} /* end namespace agent */
