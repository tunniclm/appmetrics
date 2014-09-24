/*
 * datasource.h
 *
 *  Created on: 25 Feb 2014
 *      Author: adam
 */

#ifndef ibmras_monitoring_agent_datasourcelist_h
#define ibmras_monitoring_agent_datasourcelist_h

#include "ibmras/common/port/ThreadData.h"
#include "ibmras/monitoring/agent/Bucket.h"
#include "ibmras/common/common.h"
#include <sstream>

/*
 * Internal representation of data sources e.g. push or pull sources.
 * Because this is a template class, all of the definition needs to go in the
 * header file.
 */

namespace ibmras {
namespace monitoring {
namespace agent {

/*
 * Q : will data sources reference buckets, or will buckets reference data sources ?
 */
template <class T>
class DataSourceList {
public:
	void add(uint32 provID, T *src, std::string providerName);
	DataSourceList() {head = NULL; size = 0;}
	uint32 getSize();
	std::string toString();
	std::vector<Bucket*> getBuckets();
	DataSource<T>* getItem(uint32 index);
private:
	DataSource<T> *head;
	uint32 size;
};


/*
 * Add all push/pull sources from a particular provider to the master data source list
 */
template <class T>
void DataSourceList<T>::add(uint32 provID, T *src, std::string providerName) {
	DataSource<T> *dsrc = NULL;
	DataSource<T> *insertAt = NULL;
	while(src) {
		size++;
		dsrc = new DataSource<T>(provID, src, providerName);
		if(!insertAt) {
			if(head) {		/* items are already in the list */
				insertAt = head;
				while(insertAt->next) {
					insertAt = insertAt->next;
				}
			} else {
				head = dsrc;
				insertAt = head;	/* nothing in list at the moment so this is the new head */
				src = src->next;
				continue;
			}
		}
		insertAt->next = dsrc;
		insertAt = dsrc;
		src = src->next;
	}
}

template <class T>
uint32 DataSourceList<T>::getSize() {
	return size;
}

template <class T>
std::string DataSourceList<T>::toString() {
	DataSource<T> *src = head;
	std::stringstream str;
	str << "Data source list : size = " << common::itoa(getSize()) << std::endl;
	while(src) {
		str << src->toString();
		src = src->next;
	}
	return str.str();
}

template <class T>
std::vector<Bucket*> DataSourceList<T>::getBuckets() {
	std::vector<Bucket*> buckets;
	DataSource<T> *src = head;
	while(src) {
		Bucket* bucket = new Bucket(src->getProvID(), src->getSourceID(), src->getCapacity(),src->getUniqueID());
		buckets.push_back(bucket);
		src = src->next;
	}
	return buckets;
}


/* could improve this by remembering the last index/value as this is likely used from an iterator */
template <class T>
DataSource<T>* DataSourceList<T>::getItem(uint32 index) {
	uint32 count = 0;
	DataSource<T> *src = head;
	while(src && (count++ < index)) {
		src = src->next;
	}
	return src;
}


}
}
} /* end namespace agent */

#endif /* ibmras_monitoring_agent_datasourcelist_h */
