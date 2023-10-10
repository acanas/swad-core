// swad_name.c: short and full names

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

const char *Nam_ParShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES] =
  {
   [Nam_SHRT_NAME] = "ShortName",
   [Nam_FULL_NAME] = "FullName",
  };
const char *Nam_FldShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES] =
  {
   [Nam_SHRT_NAME] = "ShortName",
   [Nam_FULL_NAME] = "FullName",
  };
unsigned Nam_MaxCharsShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES] =
  {
   [Nam_SHRT_NAME] = Nam_MAX_CHARS_SHRT_NAME,
   [Nam_FULL_NAME] = Nam_MAX_CHARS_FULL_NAME,
  };
unsigned Nam_MaxBytesShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES] =
  {
   [Nam_SHRT_NAME] = Nam_MAX_BYTES_SHRT_NAME,
   [Nam_FULL_NAME] = Nam_MAX_BYTES_FULL_NAME,
  };
const char *Nam_ClassShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES] =
  {
   [Nam_SHRT_NAME] = "INPUT_SHORT_NAME",
   [Nam_FULL_NAME] = "INPUT_FULL_NAME",
  };

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

/*****************************************************************************/
/************************** Public global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************ Get short name and full name ***********************/
/*****************************************************************************/

void Nam_GetParsShrtAndFullName (char *Names[Nam_NUM_SHRT_FULL_NAMES])
  {
   Nam_ShrtOrFullName_t ShrtOrFullName;

   for (ShrtOrFullName  = Nam_SHRT_NAME;
	ShrtOrFullName <= Nam_FULL_NAME;
	ShrtOrFullName++)
      Nam_GetParShrtOrFullName (ShrtOrFullName,Names[ShrtOrFullName]);
  }

void Nam_GetParShrtOrFullName (Nam_ShrtOrFullName_t ShrtOrFullName,char *Name)
  {
   extern const char *Nam_ParShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES];
   extern unsigned Nam_MaxBytesShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES];

   Par_GetParText (Nam_ParShrtOrFullName[ShrtOrFullName],Name,
		   Nam_MaxBytesShrtOrFullName[ShrtOrFullName]);
  }

/*****************************************************************************/
/************************ Write short and full names *************************/
/*****************************************************************************/

void Nam_NewShortAndFullNames (const char *Names[Nam_NUM_SHRT_FULL_NAMES])
  {
   extern const char *Nam_ParShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES];
   extern unsigned Nam_MaxCharsShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES];
   extern const char *Nam_ClassShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES];
   Nam_ShrtOrFullName_t ShrtOrFullName;

   for (ShrtOrFullName  = Nam_SHRT_NAME;
	ShrtOrFullName <= Nam_FULL_NAME;
	ShrtOrFullName++)
     {
      HTM_TD_Begin ("class=\"CM\"");
	 HTM_INPUT_TEXT (Nam_ParShrtOrFullName[ShrtOrFullName],
			 Nam_MaxCharsShrtOrFullName[ShrtOrFullName],
			 Names[ShrtOrFullName],
			 HTM_DONT_SUBMIT_ON_CHANGE,
			 "class=\"%s INPUT_%s\""
			 " required=\"required\"",
			 Nam_ClassShrtOrFullName[ShrtOrFullName],
			 The_GetSuffix ());
      HTM_TD_End ();
     }
  }

void Nam_ExistingShortAndFullNames (Act_Action_t ActionRename[Nam_NUM_SHRT_FULL_NAMES],
			            ParCod_Param_t ParCod,long Cod,
			            const char *Names[Nam_NUM_SHRT_FULL_NAMES],
			            bool PutForm)
  {
   extern const char *Nam_ParShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES];
   extern unsigned Nam_MaxCharsShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES];
   extern const char *Nam_ClassShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES];
   Nam_ShrtOrFullName_t ShrtOrFullName;

   for (ShrtOrFullName  = Nam_SHRT_NAME;
	ShrtOrFullName <= Nam_FULL_NAME;
	ShrtOrFullName++)
     {
      HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	 if (PutForm)
	   {
	    Frm_BeginForm (ActionRename[ShrtOrFullName]);
	       ParCod_PutPar (ParCod,Cod);
	       HTM_INPUT_TEXT (Nam_ParShrtOrFullName[ShrtOrFullName],
			       Nam_MaxCharsShrtOrFullName[ShrtOrFullName],
			       Names[ShrtOrFullName],
			       HTM_SUBMIT_ON_CHANGE,
			       "class=\"%s INPUT_%s\""
			       " required=\"required\"",
			       Nam_ClassShrtOrFullName[ShrtOrFullName],
			       The_GetSuffix ());
	    Frm_EndForm ();
	   }
	 else
	    HTM_Txt (Names[ShrtOrFullName]);
      HTM_TD_End ();
     }
  }

/*****************************************************************************/
/****************** Check if a short or full name exists *********************/
/*****************************************************************************/

bool Nam_CheckIfNameExists (bool (*FuncToCheck) (const char *FldName,const char *Name,
					         long Cod,long PrtCod,unsigned Year),
		            char *Names[Nam_NUM_SHRT_FULL_NAMES],
			    long Cod,long PrtCod,unsigned Year)
  {
   extern const char *Nam_FldShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES];
   extern const char *Txt_X_already_exists;
   Nam_ShrtOrFullName_t ShrtOrFullName;

   for (ShrtOrFullName  = Nam_SHRT_NAME;
	ShrtOrFullName <= Nam_FULL_NAME;
	ShrtOrFullName++)
      if (FuncToCheck (Nam_FldShrtOrFullName[ShrtOrFullName],
		       Names[ShrtOrFullName],Cod,PrtCod,Year))
	{
	 Ale_CreateAlert (Ale_WARNING,NULL,Txt_X_already_exists,
			  Names[ShrtOrFullName]);
	 return true;	// Exists
	}

   return false;	// Does not exist
  }
