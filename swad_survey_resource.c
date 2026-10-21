// swad_survey_resource.c: links to surveys as program resources

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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
#include "swad_alert.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_parameter_code.h"
#include "swad_resource_database.h"
#include "swad_survey.h"
#include "swad_survey_database.h"
#include "swad_survey_resource.h"

/*****************************************************************************/
/**************************** Get link to survey *****************************/
/*****************************************************************************/

void SvyRsc_GetLinkToSvy (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_Surveys;
   struct Svy_Surveys Svys;

   /***** Reset surveys context *****/
   Svy_ResetSvys (&Svys);

   /***** Get survey code *****/
   Svys.Svy.SvyCod = ParCod_GetPar (ParCod_Svy);

   /***** Get survey data *****/
   if (Svys.Svy.SvyCod > 0)
      Svy_GetSvyDataByCod (&Svys.Svy);

   /***** Copy link to survey into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_SURVEY,Svys.Svy.SvyCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Svys.Svy.SvyCod > 0 ? Svys.Svy.Title :
   					Txt_Surveys);

   /***** Show surveys again *****/
   Svy_ListAllSvys (&Svys);
  }
