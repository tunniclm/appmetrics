/*
 * common.h
 *
 *  Created on: 14 Feb 2014
 *      Author: adam
 */

#ifndef ibmras_common_common_h
#define ibmras_common_common_h

#include <sstream>
namespace ibmras {
namespace common {

template <class T>
std::string itoa(T t) {
	std::stringstream s;
	s << t;
	return s.str();
}

}
}


#endif /* ibmras_common_common_h */
