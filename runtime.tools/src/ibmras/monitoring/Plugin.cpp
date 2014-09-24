/*
 * Plugin.cpp
 *
 *  Created on: 27 Aug 2014
 *      Author: robbins
 */

#if defined(_ZOS)
#define _UNIX03_SOURCE
#endif

#include "ibmras/monitoring/Plugin.h"
#include "ibmras/common/logging.h"
#include "ibmras/common/util/FileUtils.h"

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

IBMRAS_DEFINE_LOGGER("Plugin");

/* these names are produced by gcc and may not be the same for all compilers */
const char* SYM_REGISTER_PUSH_SOURCE = "ibmras_monitoring_registerPushSource";
const char* SYM_REGISTER_PULL_SOURCE = "ibmras_monitoring_registerPullSource";
const char* SYM_STOP = "ibmras_monitoring_plugin_stop";
const char* SYM_START = "ibmras_monitoring_plugin_start";
const char* SYM_CONNECTOR_FACTORY = "ibmras_monitoring_getConnector";
const char* SYM_RECEIVER_FACTORY = "ibmras_monitoring_getReceiver";

Plugin::Plugin() :
		name(""), push(NULL), pull(NULL), start(NULL), stop(NULL), confactory(
				NULL), recvfactory(NULL), handle(NULL), type(0) {
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
		IBMRAS_DEBUG(fine,  "The path is too long");
		return plugins;
	}

	StringCchCopy(szDir, MAX_PATH, path);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*.dll"));

	IBMRAS_DEBUG_1(finest,  "Scanning %s", szDir);

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind) {
		IBMRAS_DEBUG(warning,  "Unable to access the contents");
		return plugins;
	}

	do {
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			StringCchCopy(szDir, MAX_PATH, path);
			StringCchCat(szDir, MAX_PATH, TEXT("\\"));
			StringCchCat(szDir, MAX_PATH, ffd.cFileName);
			IBMRAS_DEBUG_1(finest,  "Scanning library : %s", ffd.cFileName);
			handle = LoadLibrary(szDir);
			if (handle) {
				Plugin* plugin = buildPlugin(handle, szDir);
				if (plugin != NULL) {
					plugins.push_back(plugin);
				} else {
					FreeLibrary(handle);
				}
			}
		} else {
			IBMRAS_DEBUG_1(finest,  "Skipping : %s", ffd.cFileName);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();

	if (dwError != ERROR_NO_MORE_FILES) {
		IBMRAS_DEBUG(fine,  "Error while traversing directory");
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

			IBMRAS_DEBUG_1(fine, "Processing plugin library: %s", filePath.c_str());

			void *handle = dlopen(filePath.c_str(), RTLD_LAZY);
			if (handle) {
				Plugin* plugin = buildPlugin(handle, filePath);
				if (plugin != NULL) {
					plugins.push_back(plugin);
				} else {
					/* not a plugin so close the handle	*/
					dlclose(handle);
				}
			}
		}
	}
	closedir(dp);

#endif

	return plugins;

}

Plugin* Plugin::buildPlugin(void* libHandle, const std::string &filePath) {
	Plugin* plugin = NULL;

	void* push = ibmras::common::util::FileUtils::getSymbol(libHandle,
			SYM_REGISTER_PUSH_SOURCE);
	void* pull = ibmras::common::util::FileUtils::getSymbol(libHandle,
			SYM_REGISTER_PULL_SOURCE);
	void* start = ibmras::common::util::FileUtils::getSymbol(libHandle,
			SYM_START);
	void* stop = ibmras::common::util::FileUtils::getSymbol(libHandle,
			SYM_STOP);
	void* connectorFactory = ibmras::common::util::FileUtils::getSymbol(
			libHandle, SYM_CONNECTOR_FACTORY);
	void* receiverFactory = ibmras::common::util::FileUtils::getSymbol(
			libHandle, SYM_RECEIVER_FACTORY);

	IBMRAS_DEBUG_3(fine, "Library %s: start=%p stop=%p", filePath.c_str(), start, stop);

	/* External plugins MUST implement both start and stop */
	if (start && stop) {
		plugin = new Plugin;

		plugin->name = filePath;
		plugin->handle = libHandle;

		plugin->pull = reinterpret_cast<pullsource* (*)(uint32)>(pull);
		plugin->push = reinterpret_cast<pushsource* (*)(void (*)(monitordata*), uint32)>(push);
		plugin->stop = reinterpret_cast<int (*)()>(stop);
		plugin->start = reinterpret_cast<int (*)()>(start);
		plugin->confactory = reinterpret_cast<CONNECTOR_FACTORY>(connectorFactory);
		plugin->recvfactory = reinterpret_cast<RECEIVER_FACTORY>(receiverFactory);

		plugin->setType();
	}

	return plugin;
}

void Plugin::unload() {
	if (handle) {
#if defined(WINDOWS)
		FreeLibrary((HINSTANCE)handle);
#else
		dlclose(handle);
#endif
		handle = NULL;
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

