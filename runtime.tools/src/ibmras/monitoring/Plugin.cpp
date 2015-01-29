 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#if defined(_ZOS)
#define _UNIX03_SOURCE
#endif

#include "ibmras/monitoring/Plugin.h"
#include "ibmras/common/logging.h"

#include <stdlib.h>

#if defined(WINDOWS)
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#else
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#endif

namespace ibmras {
namespace monitoring {

IBMRAS_DEFINE_LOGGER("Plugin")
;

/* these names are produced by gcc and may not be the same for all compilers */
const char* SYM_INIT = "ibmras_monitoring_plugin_init";
const char* SYM_REGISTER_PUSH_SOURCE = "ibmras_monitoring_registerPushSource";
const char* SYM_REGISTER_PULL_SOURCE = "ibmras_monitoring_registerPullSource";
const char* SYM_STOP = "ibmras_monitoring_plugin_stop";
const char* SYM_START = "ibmras_monitoring_plugin_start";
const char* SYM_CONNECTOR_FACTORY = "ibmras_monitoring_getConnector";
const char* SYM_RECEIVER_FACTORY = "ibmras_monitoring_getReceiver";
const char* SYM_VERSION = "ibmras_monitoring_getVersion";

Plugin::Plugin() :
		name(""), init(NULL), push(NULL), pull(NULL), start(NULL), stop(NULL), confactory(
				NULL), recvfactory(NULL), type(0), version(0), getVersion(NULL) {
}

std::vector<Plugin*> Plugin::scan(const std::string& dir) {

	std::vector<Plugin*> plugins;

	IBMRAS_DEBUG_1(fine, "Processing plugin path: %s", dir.c_str());

#if defined(WINDOWS)
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	HINSTANCE handle;

	TCHAR* path = (TCHAR*) dir.c_str(); /* cast to the unicode or ascii version */

	size_t length_of_arg;
	StringCchLength(path, MAX_PATH, &length_of_arg);
	if (length_of_arg > (MAX_PATH - 3)) {
		IBMRAS_DEBUG(fine, "The path is too long");
		return plugins;
	}

	StringCchCopy(szDir, MAX_PATH, path);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*.dll"));

	IBMRAS_DEBUG_1(finest, "Scanning %s", szDir);

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind) {
		IBMRAS_DEBUG(warning, "Unable to access the contents");
		return plugins;
	}

	do {
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			StringCchCopy(szDir, MAX_PATH, path);
			StringCchCat(szDir, MAX_PATH, TEXT("\\"));
			StringCchCat(szDir, MAX_PATH, ffd.cFileName);

			Plugin *plugin = processLibrary(szDir);
			if (plugin != NULL) {
				plugins.push_back(plugin);
			}
		}
	}while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();

	if (dwError != ERROR_NO_MORE_FILES) {
		IBMRAS_DEBUG(fine, "Error while traversing directory");
	}

	FindClose(hFind);

#else

	struct dirent *entry;

	DIR *dp = opendir(dir.c_str());

	if (dp == NULL) {
		IBMRAS_DEBUG_1(fine, "Warning, unable to open directory %s", dir.c_str());
		return plugins;
	}

	while ((entry = readdir(dp)) != NULL) {
		if (entry->d_name[0] != '.') {

			std::string filePath = dir;
			filePath += "/";
			filePath += entry->d_name;

			Plugin *plugin = processLibrary(filePath);
			if (plugin != NULL) {
				IBMRAS_LOG_2(info, "%s, version %s", (plugin->name).c_str(), (plugin->getVersion()));
				plugins.push_back(plugin);
			}

		}
	}
	closedir(dp);

#endif

	return plugins;

}

Plugin* Plugin::processLibrary(const std::string &filePath) {

	Plugin* plugin = NULL;
	IBMRAS_DEBUG_1(fine, "Processing plugin library: %s", filePath.c_str());

	ibmras::common::util::LibraryUtils::Handle handle =
			ibmras::common::util::LibraryUtils::openLibrary(filePath.c_str());
	if (handle.isValid()) {

		void* init = ibmras::common::util::LibraryUtils::getSymbol(handle,
				SYM_INIT);
		void* push = ibmras::common::util::LibraryUtils::getSymbol(handle,
				SYM_REGISTER_PUSH_SOURCE);
		void* pull = ibmras::common::util::LibraryUtils::getSymbol(handle,
				SYM_REGISTER_PULL_SOURCE);
		void* start = ibmras::common::util::LibraryUtils::getSymbol(handle,
				SYM_START);
		void* stop = ibmras::common::util::LibraryUtils::getSymbol(handle,
				SYM_STOP);
		void* getVersion = ibmras::common::util::LibraryUtils::getSymbol(handle,
				SYM_VERSION);
		void* connectorFactory = ibmras::common::util::LibraryUtils::getSymbol(
				handle, SYM_CONNECTOR_FACTORY);
		void* receiverFactory = ibmras::common::util::LibraryUtils::getSymbol(
				handle, SYM_RECEIVER_FACTORY);

		IBMRAS_DEBUG_3(fine, "Library %s: start=%p stop=%p", filePath.c_str(), start, stop);

		/* External plugins MUST implement both start, stop and getVersion */
		if (start && stop && getVersion) {
			plugin = new Plugin;

			plugin->name = filePath;
			plugin->handle = handle;

			plugin->init =reinterpret_cast<PLUGIN_INITIALIZE>(init);

			plugin->pull = reinterpret_cast<pullsource* (*)(agentCoreFunctions, uint32)>(pull);

			plugin->push = reinterpret_cast<pushsource* (*)(agentCoreFunctions, uint32)>(push);

			plugin->stop = reinterpret_cast<int (*)()>(stop);

			plugin->start = reinterpret_cast<int (*)()>(start);

			plugin->getVersion = reinterpret_cast<const char* (*)()>(getVersion);

			plugin->confactory = reinterpret_cast<CONNECTOR_FACTORY>(connectorFactory);

			plugin->recvfactory = reinterpret_cast<RECEIVER_FACTORY>(receiverFactory);

			plugin->setType();
		} else {
			/* not a plugin so close the handle	*/
			ibmras::common::util::LibraryUtils::closeLibrary(handle);
		}
	} else {
#if defined(WINDOWS)

#else
		IBMRAS_DEBUG_2(fine, "Not valid handler for library candidate %s. \ndlerror output: \"%s\"", filePath.c_str(), dlerror());
#endif
	}
	return plugin;
}

void Plugin::unload() {
	if (handle.isValid()) {
		ibmras::common::util::LibraryUtils::closeLibrary(handle);
	}
}

void Plugin::setType() {
	type = plugin::none;
	if (pull || push) {
		type = plugin::data;
	}
	if (confactory) {
		type = type | plugin::connector;
	}
	if (recvfactory) {
		type = type | plugin::receiver;
	}
}

}
}

