 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibhmras_common_util_libraryutils_h
#define ibhmras_common_util_libraryutils_h

#if defined(WINDOWS)
#include <windows.h>
#else
#endif

#include <string>

namespace ibmras {
namespace common {
namespace util {

class LibraryUtils {

public:
	class Handle {
	public:
#if defined(WINDOWS)
		typedef HINSTANCE handle_type;
#else
		typedef void* handle_type;
#endif

		Handle() :
				handle(NULL) {
		}

		bool isValid() {return (handle != NULL); }
		handle_type handle;
	};

	static void* getSymbol(Handle libHandle, const std::string& symbol);
	static Handle openLibrary(const std::string &lib);
	static void closeLibrary(Handle libHandle);

	static std::string getLibraryDir(const std::string &library, const void* func);
	static std::string getLibraryLocation(const void* func);
	static std::string getLibraryLocation(const std::string &library);

};

}
}
}

#endif /* ibhmras_common_util_libraryutils_h */
