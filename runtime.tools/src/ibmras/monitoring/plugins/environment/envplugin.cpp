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
#if defined (_LINUX) || defined (_AIX)
#include <sys/utsname.h> // uname()
#include <sys/sysinfo.h> // get_nprocs()
#include <unistd.h> // gethostname()
#endif
#ifdef _WINDOWS
#include "windows.h"
#define HOST_NAME_MAX 256
#endif
#include "ibmras/common/logging.h"

#if defined (_LINUX) || defined (_AIX)
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
	s += "pid=" + plugin::pid;
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
	content += '\n';
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
	src->header.description = strdup(desc.c_str());
	src->header.sourceID = srcid;
	src->header.config = "";
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
#if defined (_LINUX) || defined (_AIX)
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
	#if defined (_AIX)
		// might be _SC_NPROCESSORS_ONLN -https://www.ibm.com/developerworks/community/forums/html/topic?id=77777777-0000-0000-0000-000014250083
		plugin::nprocs = ToString(sysconf(_SC_NPROCESSORS_CONF));
	#else
		plugin::nprocs = ToString(get_nprocs());
	#endif
	plugin::pid = ToString(getpid());
}
#endif
 
/*
 * Windows
 */
#ifdef _WINDOWS
static const std::string GetWindowsMajorVersion() {
	OSVERSIONINFOEX versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	
	static const std::string defaultVersion = "Windows";
	
	if (!GetVersionEx((OSVERSIONINFO *) &versionInfo)) {
		return defaultVersion;
	}
	
	switch (versionInfo.dwPlatformId) {
	case VER_PLATFORM_WIN32s: return "Windows 3.1";
	case VER_PLATFORM_WIN32_WINDOWS:
		switch (versionInfo.dwMinorVersion) {
		case 0: return "Windows 95";
		case 90: return "Windows Me";
		default: return "Windows 98";
		}
		break; /* VER_PLATFORM_WIN32_WINDOWS */
		
	case VER_PLATFORM_WIN32_NT:
		if (versionInfo.dwMajorVersion < 5)  {
			return "Windows NT";
			
		} else if (versionInfo.dwMajorVersion == 5) {
			switch (versionInfo.dwMinorVersion) {
			case 0: return "Windows 2000";
			/* case 1: WinNT 5.1 => Windows XP. Handled by the default. */
			case 2:
				/* WinNT 5.2 can be either Win2003 Server or Workstation (e.g. XP64).
				 * Report workstation products as "Windows XP".
				 * See CMVC 89090 and CMVC 89127 */
				switch (versionInfo.wProductType) {
				case VER_NT_WORKSTATION: return "Windows XP";
				case VER_NT_DOMAIN_CONTROLLER:
				case VER_NT_SERVER:
				default: return "Windows Server 2003";
				}
			default: return "Windows XP";
			}
			
		} else if (versionInfo.dwMajorVersion == 6) {
			switch (versionInfo.wProductType) {
			case VER_NT_WORKSTATION:
				switch (versionInfo.dwMinorVersion) {
				case 0: return "Windows Vista";
				case 1: return "Windows 7";
				case 2: return "Windows 8";
				default: return defaultVersion;
				} /* VER_NT_WORKSTATION */
			default:
				switch (versionInfo.dwMinorVersion) {
				case 0: return "Windows Server 2008";
				case 1: return "Windows Server 2008 R2";
				case 2: return "Windows Server 8";
				default: return defaultVersion;
				}
			}
		} else {
			return defaultVersion;
		}
		break; /* VER_PLATFORM_WIN32_NT */
			
	default: return defaultVersion;
	}
}

static const std::string GetWindowsBuild() {
	OSVERSIONINFOW versionInfo;
	int len = sizeof("0123456789.0123456789 build 0123456789 ") + 1;
	char *buffer;
	int position;
	
	static const std::string defaultBuild = "";
	
	versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
	
	if (!GetVersionExW(&versionInfo)) {
		return defaultBuild;
	}

	if (NULL != versionInfo.szCSDVersion) {
		len += WideCharToMultiByte(CP_UTF8, 0, versionInfo.szCSDVersion, -1, NULL, 0, NULL, NULL);
	}
	buffer = new char[len];
	if (NULL == buffer) {
		return defaultBuild;
	}

	position = sprintf(buffer,"%d.%d build %d",
		versionInfo.dwMajorVersion,
		versionInfo.dwMinorVersion,
		versionInfo.dwBuildNumber & 0x0000FFFF);

	if ((NULL != versionInfo.szCSDVersion) && ('\0' != versionInfo.szCSDVersion[0])) {
		buffer[position++] = ' ';
		WideCharToMultiByte(CP_UTF8, 0, versionInfo.szCSDVersion, -1, &buffer[position], len - position - 1, NULL, NULL);
	}
	
	std::string version(buffer);
	delete[] buffer;
	return version;
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
	plugin::osName = GetWindowsMajorVersion();
	plugin::osVersion = GetWindowsBuild();
	plugin::nprocs = ToString(sysinfo.dwNumberOfProcessors); /* convert DWORD to char* -- will this need a new ToString() ? */
	plugin::pid = ToString(GetCurrentProcessId()); /* convert DWORD to char* -- will this need a new ToString() ? */	
}
#endif
