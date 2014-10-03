/*
 * strUtils.h
 *
 *  Created on: 8 Aug 2014
 *      Author: Admin
 */

#include <string>
#include <vector>
#include "ibmras/export.h"

#ifndef STRUTILS_H_
#define STRUTILS_H_

namespace ibmras {
namespace common {
namespace util {

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
bool endsWith(const std::string& str, const std::string& suffix);
DECL bool equalsIgnoreCase(const std::string& s1, const std::string& s2);

}/*end of namespace util*/
}/*end of namespace common*/
} /*end of namespace ibmras*/




#endif /* STRUTILS_H_ */
