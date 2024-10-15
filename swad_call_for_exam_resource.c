// swad_call_for_exam_resource.c: links to calls for exams as program resources

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_call_for_exam.h"
#include "swad_call_for_exam_resource.h"
#include "swad_error.h"
#include "swad_parameter_code.h"
#include "swad_resource_database.h"

/*****************************************************************************/
/************************ Get link to call for exam **************************/
/*****************************************************************************/

void CfeRsc_GetLinkToCallForExam (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_Calls_for_exams;
   struct Cfe_CallsForExams *CallsForExams = Cfe_GetGlobalCallsForExams ();
   char SessionAndDate[Cfe_MAX_BYTES_SESSION_AND_DATE];

   /***** Reset calls for exams context *****/
   Cfe_ResetCallsForExams (CallsForExams);

   /***** Get the code of the call for exam *****/
   CallsForExams->ExaCod = ParCod_GetPar (ParCod_Exa);

   /***** Get call for exam title *****/
   if (CallsForExams->ExaCod > 0)
     {
      /* Get data of call for exam */
      Cfe_AllocMemCallForExam (CallsForExams);
      Cfe_GetCallForExamDataByCod (CallsForExams,CallsForExams->ExaCod);

      /* Session and date of the exam */
      Cfe_BuildSessionAndDate (CallsForExams,SessionAndDate);

      /* Free memory of the call for exam */
      Cfe_FreeMemCallForExam (CallsForExams);
     }

   /***** Copy link to call for exam into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_CALL_FOR_EXAM,CallsForExams->ExaCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
		  CallsForExams->ExaCod > 0 ? SessionAndDate :
					      Txt_Calls_for_exams);

   /***** Set exam to be highlighted *****/
   CallsForExams->HighlightExaCod = CallsForExams->ExaCod;

   /***** Show again the list of calls for exams *****/
   Cfe_ListCallsForExamsEdit ();
  }

/*****************************************************************************/
/************** Get call for exam text from call for exam code ***************/
/*****************************************************************************/
// The trailing null character is not counted in TitleSize

void CfeRsc_GetCallForExamTitle (long ExaCod,char *Title,size_t TitleSize)
  {
   struct Cfe_CallsForExams CallsForExams;
   char SessionAndDate[Cfe_MAX_BYTES_SESSION_AND_DATE];

   /***** Reset calls for exams context *****/
   Cfe_ResetCallsForExams (&CallsForExams);

   /***** Get data of call for exam *****/
   Cfe_AllocMemCallForExam (&CallsForExams);
   Cfe_GetCallForExamDataByCod (&CallsForExams,ExaCod);

   /***** Session and date of the exam *****/
   Cfe_BuildSessionAndDate (&CallsForExams,SessionAndDate);
   Str_Copy (Title,SessionAndDate,TitleSize);

   /***** Free memory of the call for exam *****/
   Cfe_FreeMemCallForExam (&CallsForExams);
  }
