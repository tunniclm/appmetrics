#define _XOPEN_SOURCE_EXTENDED 1
#include <unistd.h>

#include "ibmras/common/port/ThreadData.h"

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
