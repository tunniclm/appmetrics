/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef LIBHLP_H
#define LIBHLP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "j9port.h"

#define J9CMDLINE_OK 0
#define J9CMDLINE_PARSE_ERROR 1
#define J9CMDLINE_OUT_OF_MEMORY 2
#define J9CMDLINE_GENERIC_ERROR 3

struct j9cmdlineOptions {
	int argc;
	char** argv;
	char** envp;
	J9PortLibrary* portLibrary;
	BOOLEAN shutdownPortLib;
};

#ifdef __cplusplus
}
#endif

#endif /* LIBHLP_H */
