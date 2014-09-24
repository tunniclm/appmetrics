/*
 * Properties.cpp
 *
 *  Created on: 26 Mar 2014
 *      Author: robbins
 */

#include "ibmras/common/Properties.h"
#include <sstream>

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

std::string Properties::toString() {
	std::stringstream ss;
	ss << "Properties: " << std::endl;
	for (std::map<std::string, std::string>::iterator propsiter = props.begin();
			propsiter != props.end(); ++propsiter) {
		ss << propsiter->first << "=" << propsiter->second << std::endl;
	}
	return ss.str();
}

}
}

