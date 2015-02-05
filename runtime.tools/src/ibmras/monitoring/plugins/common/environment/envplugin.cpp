 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#define _XOPEN_SOURCE_EXTENDED 1 //This macro makes zOS' unistd.h expose gethostname().

#include "ibmras/monitoring/AgentExtensions.h"
#include "ibmras/common/types.h"
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#if defined(_LINUX) || defined(_AIX)
#include <sys/utsname.h> // uname()
#include <sys/sysinfo.h> // get_nprocs()
#include <unistd.h> // gethostname()
#endif
#if defined(_AIX)
#include_next </usr/include/sys/systemcfg.h>
#include <procinfo.h>
#include <sys/types.h>
#endif
#ifdef _WINDOWS
#include "windows.h"
#define HOST_NAME_MAX 256
#endif
#ifdef _ZOS
#define HOST_NAME_MAX 256
#endif



#if defined (_LINUX) || defined (_AIX) || defined (_ZOS)
extern "C" char **environ; // use GetEnvironmentStrings() on Windows (maybe getenv() on POSIX?)
#endif

#if defined(_WINDOWS)
#define ENVPLUGIN_DECL __declspec(dllexport)	/* required for DLLs to export the plugin functions */
#else
#define ENVPLUGIN_DECL
#endif

template <class T>
std::string itoa(T t);

static void initStaticInfo();

#define DEFAULT_BUCKET_CAPACITY 1024*10

namespace envplugin {
agentCoreFunctions aCF;
}

namespace plugin {
	uint32 provid = 0;
	std::string arch; 
	std::string osName;
	std::string osVersion;
	std::string nprocs;
	std::string pid;
	std::string commandLine;

	std::string agentVersion;
	std::string agentNativeBuildDate;

}

using namespace ibmras::common::logging;

static char* NewCString(const std::string& s) {
	char *result = new char[s.length() + 1];
	std::strcpy(result, s.c_str());
	return result;
}

void AppendEnvVars(std::stringstream &ss) {
	bool hostnameDefined = false;
	int i = 0;
	while (environ[i] != NULL) {
		ss << "environment." << environ[i] << '\n';
		if (std::strncmp("HOSTNAME=", environ[i], std::strlen("HOSTNAME=")) == 0) {
			hostnameDefined = true;
		}
		i++;
	}
	if (!hostnameDefined) {
		char hostname[HOST_NAME_MAX + 1];
		if (gethostname(hostname, HOST_NAME_MAX) == 0) {
			ss  << "environment.HOSTNAME=" << hostname << '\n'; 
		}
	}
}

void AppendSystemInfo(std::stringstream &ss) {
	ss << "os.arch="     << plugin::arch             << '\n'; // eg "amd64"
	ss << "os.name="     << plugin::osName           << '\n'; // eg "Windows 7"
	ss << "os.version="  << plugin::osVersion        << '\n'; // eg "6.1 build 7601 Service Pack 1"
	ss << "pid="         << plugin::pid              << '\n'; // eg "12345"
	ss << "native.library.date=" << plugin::agentNativeBuildDate << '\n'; // eg "Oct 10 2014 11:44:56"
	ss << "jar.version=" << plugin::agentVersion     << '\n'; // eg "3.0.0.20141030"
	ss << "number.of.processors=" << plugin::nprocs  << '\n'; // eg 8
	ss << "command.line=" << plugin::commandLine     << '\n';
}

monitordata* OnRequestData() {
	monitordata *data = new monitordata;
	data->provID = plugin::provid;
	data->sourceID = 0;
	
	std::stringstream contentss;
	contentss << "#EnvironmentSource\n";
	AppendEnvVars(contentss);
	AppendSystemInfo(contentss);
	
	std::string content = contentss.str();
	data->size = static_cast<uint32>(content.length()); // should data->size be a size_t?
	data->data = NewCString(content);
	data->persistent = false;

	return data;
}

void OnComplete(monitordata* data) {
	if (data != NULL) {
		if (data->data != NULL) {
			delete[] data->data;
		}
		delete data;
	}
}

pullsource* createPullSource(uint32 srcid, const char* name) {
	pullsource *src = new pullsource();
	src->header.name = name;
	std::string desc("Description for ");
	desc.append(name);
	src->header.description = NewCString(desc);
	src->header.sourceID = srcid;
	src->next = NULL;
	src->header.capacity = DEFAULT_BUCKET_CAPACITY;
	src->callback = OnRequestData;
	src->complete = OnComplete;
	src->pullInterval = 20*60;
	return src;
}

extern "C" {
ENVPLUGIN_DECL pullsource* ibmras_monitoring_registerPullSource(agentCoreFunctions aCF, uint32 provID) {
	envplugin::aCF = aCF;

	plugin::agentVersion = std::string(envplugin::aCF.getProperty("agent.version"));
	plugin::agentNativeBuildDate = std::string(envplugin::aCF.getProperty("agent.native.build.date"));

	envplugin::aCF.logMessage(debug, "[environment_os] Registering pull source");
	pullsource *head = createPullSource(0, "environment_os");
	plugin::provid = provID;
	return head;
}

ENVPLUGIN_DECL int ibmras_monitoring_plugin_init(const char* properties) {	
	return 0;
}

ENVPLUGIN_DECL int ibmras_monitoring_plugin_start() {
	envplugin::aCF.logMessage(fine, "[environment_os] Starting");
	initStaticInfo(); // See below for platform-specific implementation, protected by ifdefs
	return 0;
}

ENVPLUGIN_DECL int ibmras_monitoring_plugin_stop() {
	envplugin::aCF.logMessage(fine, "[environment_os] Stopping");
	return 0;
}

ENVPLUGIN_DECL const char* ibmras_monitoring_getVersion() {
	return "1.0";
}
}

/* 
 * Linux
 */
#if defined (_LINUX)
static std::string GetCommandLine() {
	std::stringstream filenamess;
	filenamess << "/proc/" << getpid() << "/cmdline";
	std::string filename = filenamess.str();
	
	std::ifstream filestream(filename.c_str());

	if (!filestream.is_open()) {

		std::stringstream envss;
		envss << "Failed to open " << filename.c_str();

		return "";
	}

    std::istreambuf_iterator<char> begin(filestream), end;
    std::string cmdline(begin, end);
    filestream.close();
	
	for (unsigned i=0; i < cmdline.length(); i++) {
		if (cmdline[i] == '\0') {
			cmdline[i] = ' ';
		}
	}
	return cmdline;	
}

static void initStaticInfo() {
	struct utsname sysinfo;
	int rc = uname(&sysinfo);
	if (rc >= 0) {
		plugin::arch = std::string(sysinfo.machine);
		plugin::osName = std::string(sysinfo.sysname);
		plugin::osVersion = std::string(sysinfo.release) + std::string(sysinfo.version);
	} else {
		plugin::arch = "unknown"; // could fallback to compile-time information
		plugin::osName = "Linux";
		plugin::osVersion = "";
	}

	plugin::nprocs = itoa(get_nprocs());
	plugin::pid = itoa(getpid());
	plugin::commandLine = GetCommandLine();	
}

#endif

/* 
 * AIX 
 */
#if defined (_AIX)

static std::string GetCommandLine() {
	struct procsinfo proc;
	char procargs[512]; // Is this a decent length? Should we heap allocate and expand?
	
	proc.pi_pid = getpid();
	int rc = getargs(&proc, sizeof(proc), procargs, sizeof(procargs));
	if (rc < 0) {



		std::stringstream envss;
		envss << "Failed to get command line " << errno;
		envplugin::aCF.logMessage(debug, envss.str().c_str());

		return std::string();
	}
	std::stringstream cmdliness;
	char *current = procargs;
	int written = 0;
	while (std::strlen(current) > 0) {
		if (written++ > 0) cmdliness << ' ';
		cmdliness << current;
		current = current + std::strlen(current) + 1;
	}
	return cmdliness.str();
}

static void initStaticInfo() {
	struct utsname sysinfo;
	int rc = uname(&sysinfo);
	if (rc >= 0) {
		uint64_t architecture = getsystemcfg(SC_ARCH);
		uint64_t width = getsystemcfg(SC_WIDTH);
		
		std::string bits = (width == 32) ? "32" : 
		                   (width == 64) ? "64" : 
		                   "";
		plugin::arch = (architecture == POWER_PC) ? "ppc" : "";
		if (plugin::arch != "") {
			plugin::arch += bits;
		} else {
			plugin::arch = std::string(sysinfo.machine);
		}
		plugin::osName = std::string(sysinfo.sysname);
		plugin::osVersion = std::string(sysinfo.release) + std::string(sysinfo.version);
	} else {
		plugin::arch = "unknown"; // could fallback to compile-time information
		plugin::osName = "AIX";
		plugin::osVersion = "";
	}
	// might be _SC_NPROCESSORS_ONLN -https://www.ibm.com/developerworks/community/forums/html/topic?id=77777777-0000-0000-0000-000014250083
	plugin::nprocs = itoa(sysconf(_SC_NPROCESSORS_CONF));
	plugin::pid = itoa(getpid());
	plugin::commandLine = GetCommandLine();	
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
	default: 
		plugin::arch = "unknown"; // could fallback to compile-time information 
		break;
	}
	plugin::osName = GetWindowsMajorVersion();
	plugin::osVersion = GetWindowsBuild();
	plugin::nprocs = itoa(sysinfo.dwNumberOfProcessors);
	plugin::pid = itoa(GetCurrentProcessId());
	plugin::commandLine = std::string(GetCommandLine());	
}
#endif

template <class T>
std::string itoa(T t) {
	std::stringstream s;
	s << t;
	return s.str();
}
