// swad_info_resource.c: links to info about course as program resources

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Ca�as Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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

#include "swad_alert.h"
#include "swad_course.h"
#include "swad_global.h"
#include "swad_info.h"
#include "swad_info_database.h"
#include "swad_parameter_code.h"
#include "swad_resource_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/********************** Get link to info about course ************************/
/*****************************************************************************/

void InfRsc_GetLinkToInfo (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   static Rsc_Type_t Types[Inf_NUM_TYPES] =
     {
      [Inf_INFORMATION	] = Rsc_INFORMATION,
      [Inf_TEACH_GUIDE	] = Rsc_TEACH_GUIDE,
      [Inf_LECTURES	] = Rsc_LECTURES,
      [Inf_PRACTICALS	] = Rsc_PRACTICALS,
      [Inf_BIBLIOGRAPHY	] = Rsc_BIBLIOGRAPHY,
      [Inf_FAQ		] = Rsc_FAQ,
      [Inf_LINKS	] = Rsc_LINKS,
      [Inf_ASSESSMENT	] = Rsc_ASSESSMENT,
     };
   static void (*FunctionToShowInfo[Inf_NUM_TYPES]) (void) =
     {
      [Inf_INFORMATION	] = Crs_ShowIntroduction,
      [Inf_TEACH_GUIDE	] = Inf_ShowInfo,
      [Inf_LECTURES	] = Inf_ShowInfo,
      [Inf_PRACTICALS	] = Inf_ShowInfo,
      [Inf_BIBLIOGRAPHY	] = Inf_ShowInfo,
      [Inf_FAQ		] = Inf_ShowInfo,
      [Inf_LINKS	] = Inf_ShowInfo,
      [Inf_ASSESSMENT	] = Inf_ShowInfo,
     };
   struct Syl_Syllabus Syllabus;

   /***** Reset syllabus context *****/
   Syl_ResetSyllabus (&Syllabus);

   /***** Set info type *****/
   Inf_AsignInfoType (&Gbl.Crs.Info,&Syllabus);

   /***** Copy link to exam into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Types[Gbl.Crs.Info.Type],-1L);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Txt_INFO_TITLE[Gbl.Crs.Info.Type]);

   /***** Show test again *****/
   FunctionToShowInfo[Gbl.Crs.Info.Type] ();
  }