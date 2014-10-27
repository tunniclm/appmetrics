 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
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
