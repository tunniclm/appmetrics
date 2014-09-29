/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2001, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/
#ifndef j2sever_h
#define j2sever_h

/**
 * Constants for supported J2SE versions.
 * note: J2SE_15 needed for shared classes cache introspection but not supported by JVM.
 */
#define J2SE_15  0x1500
#define J2SE_16  0x1600
#define J2SE_17  0x1700
#define J2SE_18  0x1800

/**
 * Masks for extracting major and full versions.
 */
#define J2SE_VERSION_MASK 0xFF00
#define J2SE_RELEASE_MASK 0xFFF0
#define J2SE_SERVICE_RELEASE_MASK 0xFFFF

/**
 * Masks and constants for describing J2SE shapes.
 *  J2SE_SHAPE_SUN = Sun core libraries (aka IBM 'sidecar', Sun code + IBM kernel)
 *  J2SE_SHAPE_ORACLE = Pure Oracle code without any IBM modifications
 */
#define J2SE_SHAPE_SUN     		0x10000
#define J2SE_SHAPE_ORACLE 		0x80000
#define J2SE_SHAPE_MASK 		0xF0000
#define J2SE_SHAPE_SHIFT		16

/**
 * Masks and constants for describing J2SE shapes.
 */
#define J2SE_LAYOUT_VM_IN_SUBDIR 0x100000
#define J2SE_LAYOUT_MASK 		0xF00000
#define J2SE_LAYOUT(javaVM) 	((javaVM)->j2seVersion & J2SE_LAYOUT_MASK)

/**
 * Macro to extract the J2SE version given a J9JavaVM.
 */
#define J2SE_VERSION(javaVM) 	((javaVM)->j2seVersion & J2SE_SERVICE_RELEASE_MASK)

/**
 * Macro to extract J2SE version given a JNIEnv.
 */ 
#define J2SE_VERSION_FROM_ENV(env) J2SE_VERSION(((J9VMThread*)env)->javaVM)

/**
 * Macro to extract the shape portion of the J2SE flags.
 */
#define J2SE_SHAPE(javaVM) 	((javaVM)->j2seVersion & J2SE_SHAPE_MASK)

/**
 * Macro to extract J2SE shape given a JNIEnv.
 */ 
#define J2SE_SHAPE_FROM_ENV(env) J2SE_SHAPE(((J9VMThread*)env)->javaVM)

#endif     /* j2sever_h */

