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
