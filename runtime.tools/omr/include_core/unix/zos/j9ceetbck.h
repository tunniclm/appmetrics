/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef j9ceetbck_h
#define j9ceetbck_h

#include <leawi.h>
#include <setjmp.h>
#include <ceeedcct.h>
#include "edcwccwi.h"
#include "j9port.h"

#pragma linkage(ceetbck, OS_UPSTACK)
#pragma map(ceetbck, "CEETBCK")

#if	!defined(J9ZOS39064)
void ceetbck(
		_POINTER* dsaptr,							/* in */
		_INT4* dsa_format,							/* inout */
		_POINTER* caaptr,							/* in */
		_INT4* member_id,							/* out */
		char* program_unit_name,					/* out */
		_INT4* program_unit_name_length,			/* inout */
		_INT4* program_unit_address,				/* out */
		_INT4* call_instruction_address,			/* inout */
		char* entry_name,							/* out */
		_INT4* entry_name_length,					/* inout */
		_INT4* entry_address,						/* out */
		_INT4* callers_call_instruction_address,	/* out */
		_POINTER* callers_dsaptr,					/* out */
		_INT4* callers_dsa_format,					/* out */
		char* statement_id,							/* out */
		_INT4* statement_id_length,					/* inout */
		_POINTER* cibptr,							/* out */
		_INT4* main_program,						/* out */
		struct _FEEDBACK* fc);						/* out */
#endif

#endif /* j9ceetbck_h */
