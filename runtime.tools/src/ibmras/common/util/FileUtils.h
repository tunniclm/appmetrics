/*
 * FileUtils.h
 *
 *  Created on: 27 Aug 2014
 *      Author: robbins
 */

#ifndef ibhmras_common_util_fileutils_h
#define ibhmras_common_util_fileutils_h

#include <string>

namespace ibmras {
namespace common {
namespace util {

class FileUtils {

public:

bool createDirectory(std::string& path);

};

}
}
}

#endif /* ibhmras_common_util_fileutils_h */
