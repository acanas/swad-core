// swad_name.h: short and full names

#ifndef _SWAD_NAM
#define _SWAD_NAM
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*****************************************************************************/
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_action_list.h"
#include "swad_form.h"
#include "swad_parameter_code.h"
#include "swad_string.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Nam_MAX_CHARS_SHRT_NAME	(32 - 1)	// 31
#define Nam_MAX_BYTES_SHRT_NAME	((Nam_MAX_CHARS_SHRT_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 511

#define Nam_MAX_CHARS_FULL_NAME	(128 - 1)	// 127
#define Nam_MAX_BYTES_FULL_NAME	((Nam_MAX_CHARS_FULL_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

// Related with names of centers, degrees, courses,...
#define Nam_NUM_SHRT_FULL_NAMES 2
typedef enum
  {
   Nam_SHRT_NAME,
   Nam_FULL_NAME,
  } Nam_ShrtOrFullName_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Nam_GetParsShrtAndFullName (char *Names[Nam_NUM_SHRT_FULL_NAMES]);
void Nam_GetParShrtOrFullName (Nam_ShrtOrFullName_t ShrtOrFull,char *Name);

void Nam_NewShortAndFullNames (const char *Names[Nam_NUM_SHRT_FULL_NAMES]);
void Nam_ExistingShortAndFullNames (Act_Action_t ActionRename[Nam_NUM_SHRT_FULL_NAMES],
				    ParCod_Param_t ParCod,long Cod,
				    const char *Names[Nam_NUM_SHRT_FULL_NAMES],
				    Frm_PutForm_t PutForm);

bool Nam_CheckIfNameExists (bool (*FuncToCheck) (const char *FldName,const char *Name,
					         long Cod,long PrtCod,unsigned Year),
		            const char *Names[Nam_NUM_SHRT_FULL_NAMES],
			    long Cod,long PrtCod,unsigned Year);

#endif
