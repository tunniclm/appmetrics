 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
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
	int load(const std::string &inputFile);

protected:

};
/* end class PropertiesFile */
}
} /* end namespace RASCommon */

#endif /* ibmras_common_propertiesfile_h */
