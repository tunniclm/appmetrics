/*
 * PropertiesFile.cpp
 *
 *  Created on: 26 Mar 2014
 *      Author: robbins
 */

#include "PropertiesFile.h"

#include <fstream>
#include <string>

namespace ibmras {
namespace common {



void PropertiesFile::load(const std::string &inputFile) {
	std::ifstream in_file(inputFile.c_str());
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


