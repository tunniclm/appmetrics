 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#include "PropertiesFile.h"

#include <fstream>
#include <string>

namespace ibmras {
namespace common {



int PropertiesFile::load(const std::string &inputFile) {
	std::ifstream in_file(inputFile.c_str());
	if (in_file.fail()) {
		return -1;
	}
	std::string line;
	while(std::getline(in_file, line)) {
		if (line.find('#') == 0) {
			continue;
		}
		// trim line ending
		if (line.length() > 0 && line.at(line.length() - 1) == '\r') {
			line.erase(line.length() - 1);
		}
		size_t pos = line.find('=');
		if ((pos != std::string::npos) && (pos < line.size())) {
			put(line.substr(0, pos), line.substr(pos + 1));
		}
	}

}


}
} /* end namespace monitoring */


