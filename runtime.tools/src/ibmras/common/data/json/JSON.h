/*
 * JSON.h
 *
 *  Created on: 23 May 2014
 *      Author: adam
 */

#ifndef JSON_H_
#define JSON_H_

#include "ibmras/common/port/ThreadData.h"
#include "ibmras/common/common.h"

/*
 * Header file for working with JSON data builder
 */

/*
 * A stat used for JSON formatting of the data. It consists of a string representation
 * of a name value pair.
 *
 * This code has not yet been tested
 */

class JSONStat {
public:
	JSONStat(const char* name) { this->name = name; value = NULL; }
	void setValue(char* value) { this->value = value;}
	void setValue(double value) { this->value = ibmras::common::itoa(value); };
	const char* getName();
	char* getValue();
private:
	const char* name;
	char* value;
};

/*
 * The container for one or more data statistics
 */

class JSONStats {
public:
	JSONStats(uint32 max);
	const char* JSON();
	~JSONStats();
protected:
	JSONStat** stats;
    std::string* data;
    std::string* json;
    uint32 count;
    uint32 max;
};


#endif /* JSON_H_ */
