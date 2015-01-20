/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include <string>
#include <unistd.h>

namespace ibmras {
namespace common {
namespace port {

int getProcessId() {
	return getpid();
}

std::string getHostName() {
	char buffer[256];
	if (gethostname(buffer, 255) == 0) {
		return buffer;
	} else {
		return "unknown";
	}
}


}
}
} /* end namespace port */
