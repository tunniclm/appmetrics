 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#include "ibmras/common/Properties.h"
#include <sstream>
#include <vector>
#include "ibmras/common/util/strUtils.h"


namespace ibmras {
namespace common {

std::string Properties::get(const std::string &key,
		const std::string &defaultValue) {
	std::map<std::string, std::string>::iterator propsiter;

	propsiter = props.find(key);
	if (propsiter == props.end()) {
		return defaultValue;
	} else {
		return propsiter->second;
	}
}

void Properties::put(const std::string &key, const std::string &value) {
	props[key] = value;
}

void Properties::add(const Properties &p) {
	for (std::map<std::string, std::string>::const_iterator propsiter = p.props.begin();
			propsiter != p.props.end(); ++propsiter) {
		put(propsiter->first, propsiter->second);
	}
}

bool Properties::exists(const std::string& key) {
	if (props.find(key) == props.end()) {
		return false;
	}
	return true;
}

std::list<std::string> Properties::getKeys(const std::string& prefix) {

	std::list<std::string> keys;

	for (std::map<std::string, std::string>::iterator propsiter = props.begin();
			propsiter != props.end(); ++propsiter) {
		if (propsiter->first.compare(0, prefix.length(), prefix) == 0) {
			keys.push_back(propsiter->first);
		}
	}

	return keys;
}

void Properties::add(const std::string& propString) {
	std::vector<std::string> stringProps = ibmras::common::util::split(propString, '\n');
	for (std::vector<std::string>::iterator it = stringProps.begin(); it != stringProps.end(); ++it ) {
		std::vector<std::string> propPair = ibmras::common::util::split((*it), '=');
		if (propPair.size() == 2) {
			put(propPair[0], propPair[1]);
		}
	}

}

std::string Properties::toString() {
	std::stringstream ss;
	for (std::map<std::string, std::string>::iterator propsiter = props.begin();
			propsiter != props.end(); ++propsiter) {
		ss << propsiter->first << "=" << propsiter->second << '\n';
	}
	return ss.str();
}

}
}

