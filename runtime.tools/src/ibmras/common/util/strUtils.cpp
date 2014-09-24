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


}/*end of namespace util*/
}/*end of namespace common*/
} /*end of namespace ibmras*/
