/*
 * memory.h
 *
 *  Created on: 18 Feb 2014
 *      Author: adam
 */

#ifndef MEMORY_H_
#define MEMORY_H_

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


#endif /* MEMORY_H_ */
