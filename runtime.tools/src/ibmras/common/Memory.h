 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_common_memory_h
#define ibmras_common_memory_h

/*
 * Memory management functionality
 */

#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include "ibmras/common/types.h"

namespace ibmras {
namespace common {

template<typename T>
class alloc {
public:
	alloc(uint32 size);
	virtual ~alloc();
	void copyfrom(const T* src);
	T* ptr();
private:
	T* p;
	uint32 size;
};

template<typename T>
alloc<T>::alloc(uint32 size) {
	this->size = size * sizeof(T);
	p = (T*) malloc(this->size);
}

template<typename T>
alloc<T>::~alloc() {
	if(p) {
		free(p);
	}
}

template<typename T>
T* alloc<T>::ptr() {
	return p;
}

template<typename T>
void alloc<T>::copyfrom(const T* src) {
	memcpy(p, src, size);
}
}
} /* end of namespace port */


#endif /* ibmras_common_memory_h */
