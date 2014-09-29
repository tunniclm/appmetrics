
/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#if !defined(OMR_BASE_HPP_)
#define OMR_BASE_HPP_

#include "j9comp.h"

class OMR_Base
{
private:
protected:
public:
	void *operator new(size_t size, void *memoryPtr) { return memoryPtr; };

	/**
	 * Create OMR_Base object.
	 */
	OMR_Base() {}

};

#endif /* OMR_BASE_HPP_ */
