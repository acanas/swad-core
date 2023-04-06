// swad_survey_resource.c: links to surveys as program resources

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

void SvyRsc_GetLinkToSurvey (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   struct Svy_Surveys Surveys;
   char Title[Svy_MAX_BYTES_SURVEY_TITLE + 1];

   /***** Reset surveys context *****/
   Svy_ResetSurveys (&Surveys);

   /***** Get survey code *****/
   Surveys.Svy.SvyCod = ParCod_GetPar (ParCod_Svy);

   /***** Get survey title *****/
   SvyRsc_GetTitleFromSvyCod (Surveys.Svy.SvyCod,Title,sizeof (Title) - 1);

   /***** Copy link to survey into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_SURVEY,Surveys.Svy.SvyCod);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Title);

   /***** Show surveys again *****/
   Svy_ListAllSurveys (&Surveys);
  }

/*****************************************************************************/
/********************* Get survey title from survey code *********************/
/*****************************************************************************/
// The trailing null character is not counted in TitleSize

void SvyRsc_GetTitleFromSvyCod (long SvyCod,char *Title,size_t TitleSize)
  {
   extern const char *Txt_Surveys;
   char TitleFromDB[Svy_MAX_BYTES_SURVEY_TITLE + 1];

   if (SvyCod > 0)
     {
      /***** Get survey title *****/
      Svy_DB_GetSurveyTitle (SvyCod,TitleFromDB);
      Str_Copy (Title,TitleFromDB,TitleSize);
     }
   else
      /***** Generic title for all surveys *****/
      Str_Copy (Title,Txt_Surveys,TitleSize);
  }
