/*
 * FileUtils.cpp
 *
 *  Created on: 28 Aug 2014
 *      Author: robbins
 */
#if defined(_ZOS)
#define _UNIX03_SOURCE
#endif

#include "ibmras/common/util/FileUtils.h"

#if defined(WINDOWS)
#include <windows.h>
#include <tchar.h>
#else
#include <dlfcn.h>
#endif

void* ibmras::common::util::FileUtils::getSymbol(void *libHandle, const std::string& symbolName) {

	void *symbol = NULL;

#if defined(WINDOWS)
	HINSTANCE handle = (HINSTANCE)libHandle;
	symbol = (void*) GetProcAddress(handle, symbolName.c_str());
#else
	/* Unix platforms */
	symbol = dlsym(libHandle, symbolName.c_str());
#endif

	return symbol;

}



