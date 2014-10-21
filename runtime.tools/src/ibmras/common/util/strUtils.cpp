/*
 * strUtils.cpp
 *
 *  Created on: 8 Aug 2014
 *      Author: Admin
 */

#include "ibmras/common/util/strUtils.h"
#include "ibmras/common/util/memUtils.h"
#include <sstream>
#include "ibmras/common/Logger.h"
#include <cstring>
#include <stdlib.h>
#include <string>
#include <stdint.h>


#if defined(WINDOWS)
#include <windows.h>
#include <intrin.h>
#include <winbase.h>
#endif


namespace ibmras {
namespace common {
namespace util {

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

bool endsWith(const std::string& str, const std::string& suffix) {
	return (str.length() >= suffix.length() && (0 == str.compare(str.length() - suffix.length(), suffix.length(), suffix)));
}

bool startsWith(const std::string& str, const std::string& prefix) {
	return (str.length() >= prefix.length() && (0 == str.compare(0, prefix.length(), prefix)));
}

bool equalsIgnoreCase(const std::string& s1, const std::string& s2) {
	if (s1.length() != s2.length()) {
		return false;
	}
	for(std::string::size_type i = 0; i < s1.size(); ++i) {
	    if (toupper(s1[i]) !=  toupper(s2[i]) ) {
	    	return false;
	    }
	}

	return true;
}


}/*end of namespace util*/
}/*end of namespace common*/
} /*end of namespace ibmras*/
