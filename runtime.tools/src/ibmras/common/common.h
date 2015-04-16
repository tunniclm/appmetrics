 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
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
