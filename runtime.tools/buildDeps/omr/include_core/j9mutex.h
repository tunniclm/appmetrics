/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

/* TODO: remove once include paths are sorted out */

#include "j9comp.h"

#if defined(WIN32)
#include "win/j9mutex.h"
#else
#include "unix/j9mutex.h"
#endif
