/*
 * Properties.h
 *
 *  Created on: 26 Mar 2014
 *      Author: robbins
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
