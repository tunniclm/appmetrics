/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include "ibmras/monitoring/Plugin.h"
#include "ibmras/vm/java/healthcenter.h"
#include "ibmras/monitoring/plugins/j9/methods/MethodLookupProvider.h"
#include "ibmjvmti.h"
#include "jni.h"
#include "jvmti.h"
#include "ibmras/common/util/strUtils.h"
#include "ibmras/common/logging.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <stdlib.h>

#define JNI_VERSION JNI_VERSION_1_4

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace methods {

IBMRAS_DEFINE_LOGGER("methodlookup")
;

MethodLookupProvider* MethodLookupProvider::instance = NULL;

MethodLookupProvider::MethodLookupProvider(jvmFunctions functions) :
		providerID(0), sendHeader(true), initialHeaderSent(false) {
	vmFunctions = functions;
	name = "Method Lookup";
	pull = registerPullSource;
	type = ibmras::monitoring::plugin::data
			| ibmras::monitoring::plugin::receiver;
	recvfactory = (RECEIVER_FACTORY) getReceiver;
}

MethodLookupProvider::~MethodLookupProvider() {
}

pullsource* MethodLookupProvider::registerPullSource(uint32 provID) {
	instance->providerID = provID;

	pullsource *src = new pullsource();
	src->header.name = "methoddictionary";
	src->header.description =
			"Method lookup data which maps hex value to method data";
	src->header.sourceID = 0;
	src->header.capacity = 1048576; /* 1MB bucket capacity */

	src->callback = getData;
	src->complete = complete;
	src->pullInterval = 5;
	src->next = NULL;

	return src;
}

MethodLookupProvider* MethodLookupProvider::getInstance(
		jvmFunctions functions) {
	if (!instance) {
		instance = new MethodLookupProvider(functions);
	}
	return instance;
}

void* MethodLookupProvider::getReceiver() {
	if (!instance) {
		return NULL;
	}
	return instance;
}

void MethodLookupProvider::receiveMessage(const std::string &id, uint32 size,
		void *data) {

	if (!methodSetLock.acquire() && !methodSetLock.isDestroyed()) {

		IBMRAS_DEBUG(debug, "receiveMessage got lock");

		IBMRAS_DEBUG(debug, "processing received message");
		if (id == "methoddictionary") {
			if (size == 0 || data == NULL) {
				sendHeader = true;
			} else {
				std::string message((const char*) data, size);
				std::size_t found = message.find(',');
				if (found != std::string::npos) {
					std::string command = message.substr(0, found);
					std::string rest = message.substr(found + 1);
					std::vector<std::string> parameters =
							ibmras::common::util::split(rest, ',');

					if (parameters.size() > 0) {

						char * pEnd;
						for (std::vector<std::string>::iterator it =
								parameters.begin(); it != parameters.end();
								++it) {
							methodsToLookup.insert(
									(void*) strtol((*it).c_str(), &pEnd, 16));
						}
					} else {
						sendHeader = true;
					}
				}
			}
		} else if (id == "headless") {
			getAllMethodIDs();
		}
		IBMRAS_DEBUG(debug, "receiveMessage releasing lock");
		methodSetLock.release();
		IBMRAS_DEBUG(debug, "receiveMessage lock released");
	}

}

void MethodLookupProvider::getAllMethodIDs() {

	IBMRAS_DEBUG(debug, "in getAllMethodIDs");
	if (!vmFunctions.getJ9method) {
		return;
	}

	JNIEnv *env;
	vmFunctions.theVM->AttachCurrentThread((void **) &env, NULL);

	jclass *classes = NULL;
	jint count = 0;
	jvmtiError err = vmFunctions.pti->GetLoadedClasses(&count, &classes);

	for (int i = 0; i < count; i++) {
		jint methodcount;

		jmethodID *mids = NULL;
		err = vmFunctions.pti->GetClassMethods(classes[i], &methodcount, &mids);

		if (err == JVMTI_ERROR_NONE) {
			jmethodID *midPtr = mids;

			for (int j = 0; j < methodcount; j++) {
				/* Convert the object pointer to a j9methodPtr as that */
				/* is what health center uses to lookup missing method names */

				void *j9method_ptr;
				err = vmFunctions.getJ9method(vmFunctions.pti, *midPtr,
						&j9method_ptr);
				if (err == JVMTI_ERROR_NONE) {
					methodsToLookup.insert(j9method_ptr);
				} else {
					IBMRAS_DEBUG(fine, "getJ9method failed");
				}
				midPtr++;
			}
			env->DeleteLocalRef(classes[i]);
		}
		hc_dealloc((unsigned char**) &mids);
	}

	hc_dealloc((unsigned char**) &classes);
	vmFunctions.theVM->DetachCurrentThread();
}

monitordata* MethodLookupProvider::getData() {
	return instance->getMethodData();
}

monitordata* MethodLookupProvider::getMethodData() {
	IBMRAS_DEBUG(debug, "in getMethodData");
	JNIEnv *env;
	void *ramMethods = NULL;
	void **ramMethodsPtr = NULL;
	void *descriptorBuffer = NULL;
	unsigned char *stringBytes = NULL;
	std::stringstream ss;

	if (sendHeader) {
		sendHeader = false;
		ss << "#MethodDictionarySource\n";

		IBMRAS_DEBUG(debug, "header added");
		if (!initialHeaderSent) {
			IBMRAS_DEBUG(debug, "Sending persistent header");
			std::string mdheader = ss.str();
			initialHeaderSent = true;
			return generateData(0, mdheader.c_str(), mdheader.size(), true );
		}
	}


	vmFunctions.theVM->AttachCurrentThread((void **) &env, NULL);
	if (!methodSetLock.acquire() && !methodSetLock.isDestroyed()) {

		IBMRAS_DEBUG(debug, "getMethodData got lock");
		int numberOfMethods = methodsToLookup.size();

		IBMRAS_DEBUG_1(debug, "%d methods to lookup", numberOfMethods);

		if (vmFunctions.jvmtiGetMethodAndClassNames != 0
				&& numberOfMethods > 0) {
			jvmtiError error;

			/* Allocate memory for the method identifiers */
			ramMethods = (char**) hc_alloc(sizeof(void*) * numberOfMethods);
			if (ramMethods == NULL) {
				goto cleanup;
			}

			descriptorBuffer = (unsigned char**) hc_alloc(
					sizeof(jvmtiExtensionRamMethodData) * numberOfMethods);
			if (descriptorBuffer == NULL) {
				goto cleanup;
			}

			/* Set up the array of method identifier */
			ramMethodsPtr = (void**) ramMethods;
			int i = 0;
			for (std::set<void*>::iterator it = methodsToLookup.begin();
					it != methodsToLookup.end(); ++it) {
				ramMethodsPtr[i] = (*it);
				i++;
			}

			methodsToLookup.clear();

			/**
			 * Queries the VM for the method and class names for one or more method identifiers. The results
			 * of calling this method are only valid if isSupported() returns true.
			 *
			 * The methodIds, results, classNameOffsets and methodNameOffsets should all be large enough to
			 * contain numberOfMethods elements. For these arrays, data at any given index will correspond
			 * to the the data in the other arrays at the same index.
			 *
			 * For each method identifier at index i in the first numberOfMethod elements in methodIds, this
			 * function will set results[i] to be:
			 *
			 * 0 if, and only if, the class and method names were found and written into the stringData
			 * array. In this case, classNameOffsets[i] will point to the offset into stringData where the
			 * class name starts and methodNameOffsets[i] will point to the offset into stringData where the
			 * method name starts.
			 *
			 * 23 if, and only if, methodIds[i] is not a valid method identifier.
			 *
			 * 110 if, and only if, methodIds[i] is a valid method identifier but there is not enough
			 * remaining space in stringData to write the method and class names.
			 *
			 * @param methodIds
			 *            An array of longs representing the method identifiers to lookup. This array should
			 *            be large enough to contain numberOfMethods elements.
			 * @param results
			 *            An array of integers that should be large enough to contain numberOfMethods
			 *            elements.
			 * @param classNameOffsets
			 *            An array of integers that should be large enough to contain numberOfMethods
			 *            elements.
			 * @param methodNameOffsets
			 *            An array of integers that should be large enough to contain numberOfMethods
			 *            elements.
			 * @param numberOfMethods
			 *            The number of methods to lookup.
			 * @param stringData
			 *            A byte array to contain the returned method and class names in UTF8.
			 */
			int stringBytesLength = 200000;

			stringBytes = hc_alloc(sizeof(char) * stringBytesLength);
			if (stringBytes == NULL) {
				goto cleanup;
			};

			error = vmFunctions.jvmtiGetMethodAndClassNames(vmFunctions.pti,
					ramMethods, numberOfMethods, descriptorBuffer, stringBytes,
					&stringBytesLength);
			if (error == JVMTI_ERROR_NONE) {
				jvmtiExtensionRamMethodData * descriptors =
						(jvmtiExtensionRamMethodData *) descriptorBuffer;

				int j = 0;
				for (int k = 0; k < numberOfMethods; k++) {
					if (descriptors[j].reasonCode == JVMTI_ERROR_NONE) {

						std::stringstream ss2;
						ss2 << ramMethodsPtr[j];
						std::string method = ss2.str();
						if (ibmras::common::util::startsWith(method, "0x")) {
							method = method.substr(2);
						}
						ss << method << "=" << (char*) descriptors[j].className
								<< "." << (char*) descriptors[j].methodName
								<< "\n";
					} else if (descriptors[j].reasonCode
							== JVMTI_ERROR_OUT_OF_MEMORY) {
						methodsToLookup.insert(ramMethodsPtr[j]);
					}
					j++;
				}

			}
		}
	}

	cleanup:

	IBMRAS_DEBUG(debug, "getMethodData releasing lock");
	methodSetLock.release();


	IBMRAS_DEBUG(debug, "getMethodData lock released");

	hc_dealloc((unsigned char**) &stringBytes);
	hc_dealloc((unsigned char**) &ramMethods);
	hc_dealloc((unsigned char**) &descriptorBuffer);

	vmFunctions.theVM->DetachCurrentThread();

	monitordata *mdata = NULL;
	std::string datastring = ss.str();
	if (datastring.length() > 0) {
		mdata = generateData(0, datastring.c_str(), datastring.length(), false);
		IBMRAS_DEBUG_2(debug, " provider %d size %d", mdata->provID, mdata->size);
	}
	return mdata;
}

void MethodLookupProvider::complete(monitordata *mdata) {
	IBMRAS_DEBUG(debug, "complete")
	if (mdata->data) {
		delete mdata->data;
	}
	delete mdata;
}

monitordata* MethodLookupProvider::generateData(uint32 sourceID,
		const char *dataToSend, int size, bool persistentData) {
	monitordata* data = new monitordata;
	data->provID = providerID;
	if (dataToSend && size > 0) {
		char *buffer = new char[size];
		memcpy(buffer, dataToSend, size);
	data->data = buffer;
	} else {
		data->data = NULL;
	}
	data->size = size;
	data->sourceID = sourceID;
	data->persistent = persistentData;
	return data;
}

/*******************************
 * MEMORY MANAGEMENT FUNCTIONS *
 *******************************/

unsigned char* MethodLookupProvider::hc_alloc(int size) {
	jvmtiError rc;
	void* buffer = NULL;
	rc = (vmFunctions.pti)->Allocate(size, (unsigned char**) &buffer);
	if (rc != JVMTI_ERROR_NONE) {
		//fprintf(stderr,"OutOfMem : hc_alloc failed to allocate %d bytes.", size);
		return NULL;
	} else {
		//fprintf(stderr,"hc_alloc: allocated %d bytes at %p", size, buffer);
		memset(buffer, 0, size);
		return (unsigned char*) buffer;
	}

}

void MethodLookupProvider::hc_dealloc(unsigned char** buffer) {
	jvmtiError rc;

	if (*buffer == NULL) {
		//fprintf(stderr,"hc_dealloc buffer == NULL");
		return;
	}
	rc = (vmFunctions.pti)->Deallocate(*buffer);
	if (rc != JVMTI_ERROR_NONE) {
		//fprintf(stderr,"hc_dealloc failed to deallocate. rc=%d", rc);
	} else {
		*buffer = NULL;
	}
}

}
}
}
}
} /* end namespace methods */

