/*
 * plugin.cpp
 *
 *  Created on: 16 Jul 2014
 *      Author: Mike Tunnicliffe
 */

#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/common/Logger.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <string>
// #include <thread>
#ifdef _LINUX
#include <sys/utsname.h> // uname()
#include <sys/sysinfo.h> // get_nprocs()
#include <unistd.h> // gethostname()
#endif
#ifdef _WINDOWS
#include "windows.h"
#include "VersionHelpers.h"
#define HOST_NAME_MAX 256
#endif
#include "ibmras/common/logging.h"

#ifdef _LINUX
extern "C" char **environ; // use GetEnvironmentStrings() on Windows (maybe getenv() on POSIX?)
#endif

#if defined(_WINDOWS)
#define ENVPLUGIN_DECL __declspec(dllexport)	/* required for DLLs to export the plugin functions */
#else
#define ENVPLUGIN_DECL
#endif

static void initStaticInfo();

#define DEFAULT_CAPACITY 1024*10

IBMRAS_DEFINE_LOGGER("EnvironmentPlugin");

namespace plugin {
	uint32 provid = 0;
	std::string arch; 
	std::string osName;
	std::string osVersion;
	std::string nprocs;
	std::string pid;
}

void AppendEnvVars(std::string &s) {
	bool hostnameDefined = false;
	int i = 0;
	while (environ[i] != NULL) {
		if (i > 0) s += '\n';
		s += "environment.";
		s += environ[i];
		if (strncmp("HOSTNAME=", environ[i], strlen("HOSTNAME=")) == 0) {
			hostnameDefined = true;
		}
		i++;
	}
	if (!hostnameDefined) {
		char hostname[HOST_NAME_MAX + 1];
		if (gethostname(hostname, HOST_NAME_MAX) == 0) {
			s += '\n'; s += "environment.HOSTNAME="; s += hostname; 
		}
	}
}

void AppendSystemInfo(std::string &s) {
	s += "system.os.arch=" + plugin::arch + '\n'; // eg amd64
	s += "system.os.name=" + plugin::osName + '\n'; // eg Windows 7
	s += "system.os.version=" + plugin::osVersion + '\n'; // eg 6.1 build 7601 Service Pack 1
	s += "number.of.processors=" + plugin::nprocs + '\n'; // eg 8
	s += "pid=" + plugin::pid; // + '\n';
}

// Do something akin to C++11 std::to_string()
std::string ToString(unsigned value) {
	char buf[32]; // unsigned can have max 10 digits, so should not overrun
	std::sprintf(buf, "%d", value);
	return std::string(buf);
}

monitordata* OnRequestData() {
	monitordata *data = new monitordata;
	data->provID = plugin::provid;
	data->sourceID = 0;
	std::string content("#EnvironmentSource\n");
	AppendEnvVars(content);
	content += '\n';
	AppendSystemInfo(content);
	data->size = content.length();
	data->data = strdup(content.c_str());
	data->persistent = false;
	
	return data;
}

void OnComplete(monitordata* data) {
	delete[] data->data;
	delete data;
}

pullsource* createPullSource(uint32 srcid, const char* name) {
	pullsource *src = new pullsource();
	src->header.name = name;
	std::string desc("Description for ");
	desc.append(name);
	src->header.description = desc.c_str();
	src->header.sourceID = srcid;
	src->next = NULL;
	src->header.capacity = DEFAULT_CAPACITY;
	src->callback = OnRequestData;
	src->complete = OnComplete;
	src->pullInterval=20*60;
	return src;
}

extern "C" {
ENVPLUGIN_DECL pullsource* ibmras_monitoring_registerPullSource(uint32 provID) {
	IBMRAS_DEBUG(info,  "Registering pull sources");
	pullsource *head = createPullSource(0, "environment_os");
	plugin::provid = provID;
	return head;
}

ENVPLUGIN_DECL int ibmras_monitoring_plugin_start() {
	IBMRAS_DEBUG(info,  "Starting");
	initStaticInfo(); // See below for platform-specific implementation, protected by ifdefs
	return 0;
}

ENVPLUGIN_DECL int ibmras_monitoring_plugin_stop() {
	return 0;
}
}

/*
 * Architectures
 */
static const char* GetArchitecture() {
/* probably need to replace these with the real predefined macros, or else bake them into our builds */
/*#ifdef _ARM_
	return "arm";
#elif _PPC_
	return "ppc";
#elif _MIPS_
	return "mips";
#elif _X86_
	return "x86";
#elif _AMD64_
	return "x86_64";
#elif _S390_
	return "s390";
#elif _S390X_
	return "s390x";
#else*/
	return "unknown";
/*#endif*/
}
/* 
 * Linux 
 */
#ifdef _LINUX
static void initStaticInfo() {
	struct utsname sysinfo;
	int rc = uname(&sysinfo);
	if (rc >= 0) {
		plugin::arch = std::string(sysinfo.machine);
		plugin::osName = std::string(sysinfo.sysname);
		plugin::osVersion = std::string(sysinfo.release) + std::string(sysinfo.version);
	} else {
		plugin::arch = GetArchitecture();
		plugin::osName = "Linux"; // this fallback may need to change if this function is made more general (eg POSIX rather than just Linux)
		plugin::osVersion = "";
	}
	plugin::nprocs = ToString(get_nprocs());
	plugin::pid = ToString(getpid());
}
#endif
 
/*
 * Windows
 */
#ifdef _WINDOWS
static const char* GetWindowsVersion() {
	// FIXME? Should do a much more thorough job here using GetVersionEx() etc -- see OSVERSIONINFOEX structure Remarks section on MSDN for details
	if (IsWindowsVersionOrGreater(6, 3, 1)) return ""; // One more than the latest we know (6.3.0 is Windows 8.1 -- see OSVERSIONINFOEX structure on MSDN)
	if (IsWindowsServer()) {
		return "Server"; // This is why we need to use GetVersionEx() ... helper fns are not good with mappings for server versions
	} else {
		if (IsWindows8Point1OrGreater()) return "8.1";
		if (IsWindows8OrGreater()) return "8";
		if (IsWindows7SP1OrGreater()) return "7 SP1";
		if (IsWindows7OrGreater()) return "7";
		if (IsWindowsVistaSP2OrGreater()) return "Vista SP2";
		if (IsWindowsVistaSP1OrGreater()) return "Vista SP1";
		if (IsWindowsVistaOrGreater()) return "Vista";
	}
	return "unknown";
}

static void initStaticInfo() {
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	switch (sysinfo.wProcessorArchitecture) {
	case PROCESSOR_ARCHITECTURE_AMD64: plugin::arch = "x86_64"; break;
	case PROCESSOR_ARCHITECTURE_ARM: plugin::arch = "arm"; break;
	case PROCESSOR_ARCHITECTURE_IA64: plugin::arch = "itanium"; break;
	case PROCESSOR_ARCHITECTURE_INTEL: plugin::arch = "x86"; break;
	default: plugin::arch = GetArchitecture(); break;
	}
	plugin::osName = "Windows";
	plugin::osVersion = GetWindowsVersion();
	plugin::nprocs = ToString(sysinfo.dwNumberOfProcessors).c_str(); /* convert DWORD to char* -- will this need a new ToString() ? */
	plugin::pid = ToString(GetCurrentProcessId()).c_str(); /* convert DWORD to char* -- will this need a new ToString() ? */	
}
#endif
