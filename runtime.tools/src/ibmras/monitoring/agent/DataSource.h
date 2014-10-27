 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_monitoring_agent_datasource_h
#define ibmras_monitoring_agent_datasource_h

#include <sstream>
#include "ibmras/common/port/ThreadData.h"
#include "ibmras/monitoring/agent/Bucket.h"
#include "ibmras/common/common.h"

/*
 * Internal representation of data sources e.g. push or pull sources.
 * Because this is a template class, all of the definition needs to go in the
 * header file.
 */

namespace ibmras {
namespace monitoring {
namespace agent {

/* defines a data source */
template <class T>
class DataSource {
public:
	DataSource(uint32 provID, T *src, const std::string &providerName) {
		this->provID = provID;
		this->src = src;
		next = NULL;
		this->providerName = providerName;
		uniqueID = src->header.name;
	}
	DataSource *next;
	std::string toString();
	uint32 getProvID();
	uint32 getSourceID();
	uint32 getCapacity();
	std::string getProviderName();
	std::string getUniqueID();
	const char* getDescription();
	T* getSource();
private:
	uint32 provID;
	std::string providerName;
	std::string uniqueID;
	T *src;		/* source for this provider */
};

template <class T>
std::string DataSource<T>::toString() {
	std::stringstream str;
	str << src->header.name << " (id = " << common::itoa(provID) << ":" << common::itoa(src->header.sourceID) << ")\n";
	return str.str();
}

template <class T>
uint32 DataSource<T>::getProvID() {
	return provID;
}

template <class T>
std::string DataSource<T>::getProviderName() {
	return providerName;
}

template <class T>
std::string DataSource<T>::getUniqueID() {
	return  uniqueID;
}

template <class T>
uint32 DataSource<T>::getSourceID() {
	return src->header.sourceID;
}

template <class T>
uint32 DataSource<T>::getCapacity() {
	return src->header.capacity;
}

template <class T>
const char* DataSource<T>::getDescription() {
	return src->header.description;
}

template <class T>
T* DataSource<T>::getSource() {
	return src;
}

}
}
} /* end namespace agent */

#endif /* ibmras_monitoring_agent_datasource_h */
