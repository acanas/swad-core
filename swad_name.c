// swad_name.c: short and full names

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_alert.h"
#include "swad_form.h"
#include "swad_HTML.h"
#include "swad_name.h"
#include "swad_parameter.h"
#include "swad_theme.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

const char *Nam_Params[Nam_NUM_SHRT_FULL_NAMES] =
  {
   [Nam_SHRT_NAME] = "ShrtName",
   [Nam_FULL_NAME] = "FullName",
  };
const char *Nam_Fields[Nam_NUM_SHRT_FULL_NAMES] =
  {
   [Nam_SHRT_NAME] = "ShortName",	// TODO: Change to ShrtName in databse tables?
   [Nam_FULL_NAME] = "FullName",
  };
unsigned Nam_MaxChars[Nam_NUM_SHRT_FULL_NAMES] =
  {
   [Nam_SHRT_NAME] = Nam_MAX_CHARS_SHRT_NAME,
   [Nam_FULL_NAME] = Nam_MAX_CHARS_FULL_NAME,
  };
unsigned Nam_MaxBytes[Nam_NUM_SHRT_FULL_NAMES] =
  {
   [Nam_SHRT_NAME] = Nam_MAX_BYTES_SHRT_NAME,
   [Nam_FULL_NAME] = Nam_MAX_BYTES_FULL_NAME,
  };
const char *Nam_Classes[Nam_NUM_SHRT_FULL_NAMES] =
  {
   [Nam_SHRT_NAME] = "Frm_C2_INPUT",
   [Nam_FULL_NAME] = "Frm_C2_INPUT",
  };

/*****************************************************************************/
/************************ Get short name and full name ***********************/
/*****************************************************************************/

void Nam_GetParsShrtAndFullName (char *Names[Nam_NUM_SHRT_FULL_NAMES])
  {
   Nam_ShrtOrFullName_t ShrtOrFull;

   for (ShrtOrFull  = Nam_SHRT_NAME;
	ShrtOrFull <= Nam_FULL_NAME;
	ShrtOrFull++)
      Nam_GetParShrtOrFullName (ShrtOrFull,Names[ShrtOrFull]);
  }

void Nam_GetParShrtOrFullName (Nam_ShrtOrFullName_t ShrtOrFull,char *Name)
  {
   Par_GetParText (Nam_Params[ShrtOrFull],Name,Nam_MaxBytes[ShrtOrFull]);
  }

/*****************************************************************************/
/************************ Write short and full names *************************/
/*****************************************************************************/

void Nam_NewShortAndFullNames (const char *Names[Nam_NUM_SHRT_FULL_NAMES])
  {
   Nam_ShrtOrFullName_t ShrtOrFull;

   for (ShrtOrFull  = Nam_SHRT_NAME;
	ShrtOrFull <= Nam_FULL_NAME;
	ShrtOrFull++)
     {
      HTM_TD_Begin ("class=\"LM\"");
	 HTM_INPUT_TEXT (Nam_Params[ShrtOrFull],Nam_MaxChars[ShrtOrFull],
			 Names[ShrtOrFull],
			 HTM_DONT_SUBMIT_ON_CHANGE,
			 "class=\"%s INPUT_%s\" required=\"required\"",
			 Nam_Classes[ShrtOrFull],The_GetSuffix ());
      HTM_TD_End ();
     }
  }

void Nam_ExistingShortAndFullNames (Act_Action_t ActionRename[Nam_NUM_SHRT_FULL_NAMES],
			            ParCod_Param_t ParCod,long Cod,
			            const char *Names[Nam_NUM_SHRT_FULL_NAMES],
			            Frm_PutForm_t PutForm)
  {
   Nam_ShrtOrFullName_t ShrtOrFull;

   for (ShrtOrFull  = Nam_SHRT_NAME;
	ShrtOrFull <= Nam_FULL_NAME;
	ShrtOrFull++)
     {
      HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
         switch (PutForm)
           {
            case Frm_DONT_PUT_FORM:
               HTM_Txt (Names[ShrtOrFull]);
               break;
            case Frm_PUT_FORM:
	       Frm_BeginForm (ActionRename[ShrtOrFull]);
		  ParCod_PutPar (ParCod,Cod);
		  HTM_INPUT_TEXT (Nam_Params[ShrtOrFull],Nam_MaxChars[ShrtOrFull],
				  Names[ShrtOrFull],
				  HTM_SUBMIT_ON_CHANGE,
				  "class=\"%s INPUT_%s\" required=\"required\"",
				  Nam_Classes[ShrtOrFull],The_GetSuffix ());
	       Frm_EndForm ();
               break;
           }
      HTM_TD_End ();
     }
  }

/*****************************************************************************/
/****************** Check if a short or full name exists *********************/
/*****************************************************************************/

bool Nam_CheckIfNameExists (bool (*FuncToCheck) (const char *FldName,const char *Name,
					         long Cod,long PrtCod,unsigned Year),
		            const char *Names[Nam_NUM_SHRT_FULL_NAMES],
			    long Cod,long PrtCod,unsigned Year)
  {
   extern const char *Txt_X_already_exists;
   Nam_ShrtOrFullName_t ShrtOrFull;

   for (ShrtOrFull  = Nam_SHRT_NAME;
	ShrtOrFull <= Nam_FULL_NAME;
	ShrtOrFull++)
      if (FuncToCheck (Nam_Fields[ShrtOrFull],Names[ShrtOrFull],Cod,PrtCod,Year))
	{
	 Ale_CreateAlert (Ale_WARNING,NULL,Txt_X_already_exists,Names[ShrtOrFull]);
	 return true;	// Exists
	}

   return false;	// Does not exist
  }
