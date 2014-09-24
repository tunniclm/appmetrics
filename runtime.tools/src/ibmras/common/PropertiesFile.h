/*
 * PropertiesFile.h
 *
 *  Created on: 26 Mar 2014
 *      Author: robbins
 */

#ifndef ibmras_common_propertiesfile_h
#define ibmras_common_propertiesfile_h

#include <stdlib.h>
#include <istream>
#include "ibmras/common/Properties.h"

namespace ibmras {
namespace common {

class DECL PropertiesFile: public Properties {
public:
	void load(const std::string &inputFile);

protected:

};
/* end class PropertiesFile */
}
} /* end namespace RASCommon */

#endif /* ibmras_common_propertiesfile_h */
