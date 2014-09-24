
#include "ibmras/common/port/ThreadData.h"
#include "ibmras/export.h"

#include <winsock2.h>
#include <process.h>

namespace ibmras {
namespace common {
namespace port {

DECL int getProcessId() {
	return _getpid();
}

DECL std::string getHostName() {
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
