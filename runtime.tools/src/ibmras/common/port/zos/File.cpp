/*
 * file.cpp
 *
 */

/*
 * Functions for working with files
 */

#define _UNIX03_SOURCE
#include <dlfcn.h>
#include <dirent.h>
#include <sys/types.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "ibmras/common/port/ThreadData.h"
#include "ibmras/monitoring/Plugin.h"
#include "ibmras/common/logging.h"

namespace ibmras {
namespace common {
namespace port {

extern IBMRAS_DECLARE_LOGGER;		/* use the logger defined in the Agent */

/* these names are produced by gcc and may not be the same for all compilers */
const char* csyms[] = {"registerPushSource", "registerPullSource", "stop", "start"};		/* C language function symbols */
#define SYM_REGISTER_PUSH_SOURCE 0
#define SYM_REGISTER_PULL_SOURCE 1
#define SYM_STOP 2
#define SYM_START 3

/*
 * Get a symbol from a function, either in C or C++
 */
void* getSymbol(void* handle, int index) {
	void* symbol;
	symbol = dlsym(handle, csyms[index]);
	return symbol;
}

std::vector<Plugin*>* scan(char* dir) {

	int count, i;
	struct dirent *files;
	void *handle;
	std::vector<Plugin*>* plugins = new std::vector<Plugin*>;
	char filePath[_POSIX_PATH_MAX];
	void *push, *pull;


	if(!dir){
		dir = "./"; //If the dir weren't specified in command line it looks for the .so in the current dir
	}

	DIR *dp;

	if((dp = opendir(dir)) == NULL){
				IBMRAS_DEBUG_1(fine,  "Warning, unable to open directory %s", dir);
				return plugins;
	}
	else {
	   while ((files = readdir(dp)) != NULL) {
	     if (files->d_name[0] != '.') {
		strcpy(filePath, dir);
		strcat(filePath, "/");
		strcat(filePath, files->d_name);

		handle = dlopen(filePath, RTLD_LAZY);
		if(handle) {
			push = getSymbol(handle, SYM_REGISTER_PUSH_SOURCE);
			pull = getSymbol(handle, SYM_REGISTER_PULL_SOURCE);
			IBMRAS_DEBUG_1(info,  "%s", filePath);
			IBMRAS_DEBUG_2(info,  "push (%p) pull (%p)", push, pull);
			if(push || pull) {
				Plugin* plugin = new Plugin;
				plugin->name = filePath;
				plugin->handle = handle;
				plugin->pull = (pullsource* (*)(uint32))(pull);
				plugin->push = (pushsource* (*)(void (*)(monitordata*), uint32))(push);
				plugin->stop = (int (*)())(getSymbol(handle, SYM_STOP));
				plugin->start = (int (*)())(getSymbol(handle, SYM_START));
				IBMRAS_DEBUG_2(info,  "start (%p) stop (%p)", plugin->start, plugin->stop);
				plugins->push_back(plugin);

			} else {
				/* not a plugin so close the handle	*/
				dlclose(handle);
			}
		}
	}

	   }

	}

	free(files);

	return plugins;
}

}/* end namespace port */

}

}
