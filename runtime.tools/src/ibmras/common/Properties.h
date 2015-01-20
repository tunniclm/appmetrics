 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_common_properties_h
#define ibmras_common_properties_h
#include "ibmras/export.h"
#include <stdlib.h>
#include <istream>
#include <string>
#include <list>
#include <map>

namespace ibmras {
namespace common {

class DECL Properties {
public:
	virtual std::string get(const std::string &key, const std::string &defaultValue = "");
	virtual bool exists(const std::string &key);
	virtual void put(const std::string &key, const std::string &value);
	virtual void add(const Properties &p);
	virtual void add(const std::string &propString);
	virtual std::list<std::string> getKeys(const std::string& prefix = "");
	virtual std::string toString();

	virtual ~Properties() {}
protected:
	std::map<std::string, std::string> props;

};
/* end class Properties */
}
} /* end namespace RASCommon */

#endif /* ibmras_common_properties_h */
