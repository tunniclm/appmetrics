/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include "ibmras/common/data/json/JSON.h"


JSONStats::JSONStats(uint32 max) {
	stats = new JSONStat*[max];
	data = NULL;
	json = NULL;
	count = 0;
	this->max = max;
}

const char* JSONStats::JSON() {
	JSONStat* stat = stats[0];
	json = new std::string;
	json->append("{ \n\"count\" : ");
	json->append(ibmras::common::itoa(count++));
	json->append(",\n");
	for(uint32 i = 0, j = max - 1; i < max; stat++, i++, j--) {
		json->append("\"");
		json->append(stat->getName());
		json->append("\" : ");
		if(!i) json->append("\"");
		char* value = stat->getValue();
		if(value) {
			json->append(value);
		} else {
			json->append("0");		/* no data available for that stat */
		}
		if(!i) json->append("\"");
		if(j) {
			json->append(",\n");
		} else {
			json->append("\n");
		}
	}
	json->append(" }\n");
	char* result = new char[json->length() + 1];
	*(result + json->length()) = 0;		/* null terminate the string */
	json->copy(result, json->length());
	delete json;
	json = NULL;
	return result;
}

JSONStats::~JSONStats() {
	if(data) {
		delete[] data;
	}
	if(json) {
		delete json;
	}
}

const char* JSONStat::getName() {
	return name;
}

char* JSONStat::getValue() {
	return value;
}
