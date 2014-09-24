/*
 * export.h
 *
 *  Created on: 28 Aug 2014
 *      Author: robbins
 */

#ifndef ibmras_export_h
#define ibmras_export_h


#if defined(_WINDOWS)
#if defined(EXPORT)
#define DECL __declspec(dllexport)	/* required for DLLs to export the plugin functions */
#else
#define DECL __declspec(dllimport)
#endif
#endif

/* provide a default definition of DECL of the platform does not define one */
#ifndef DECL
#define DECL
#endif


#endif /* ibmras_export_h */
