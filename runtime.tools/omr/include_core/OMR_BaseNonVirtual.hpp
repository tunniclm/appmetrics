/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#if !defined(OMR_BASENONVIRTUAL_HPP_)
#define OMR_BASENONVIRTUAL_HPP_

#include "OMR_Base.hpp"

class OMR_BaseNonVirtual : public OMR_Base
{
private:
protected:
	/* Used by DDR to figure out runtime types, this is opt-in
	 * and has to be done by the constructor of each subclass.
	 * e.g. _typeId = __FUNCTION__;
	 */
	const char* _typeId;

public:

	/**
	 * Create OMR_BaseNonVirtual object.
	 */
	OMR_BaseNonVirtual()
	{
		_typeId = NULL; // If NULL DDR will print the static (compile-time) type.
	}
};

#endif /* OMR_BASENONVIRTUAL_HPP_ */
