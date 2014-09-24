/*
 * LegacyData.h
 *
 *  Created on: 21 May 2014
 *      Author: adam
 */

#ifndef LEGACYDATA_H_
#define LEGACYDATA_H_

#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/common.h"

namespace ibmras {
namespace common {
namespace data {
namespace legacy {

/*
 * Superclass for all legacy data values. Forces specific subclasses to implement
 * the toString() method which is invoked when sending data to the client.
 */
class LegacyDataValue {
public:
	virtual std::string toString() = 0;		/* string representation of the data to be sent back to the client */
	virtual ~LegacyDataValue();
};

/*
 * Template class to handle the different types of numeric data that can be returned
 * int, long, double etc. etc.
 */
template<class V>
class LegacyDataNumeric : public LegacyDataValue {
public:
	LegacyDataNumeric(V value) { this->value = value; }
	std::string toString() {
		return ibmras::common::itoa(value);
	}
private:
	V value;
};

/*
 * Legacy data is that defined as being used by HC versions earlier than 3.0.
 *
 * The format expected is <EYECATCHER><DELIM><TIMESTAMP><DELIM> followed by delimeted data
 */

class LegacyData {
public:
	LegacyData(const char* eyecatcher, uint64 timestamp);
	void add(LegacyDataValue* value);
	char* getData();
	virtual ~LegacyData();
private:
	uint64 timestamp;
	std::string* eyecatcher;
	std::vector<LegacyDataValue*>* data;
};

} /* namespace legacy */
} /* namespace data */
} /* namespace common */
} /* namespace ibmras */
#endif /* LEGACYDATA_H_ */
