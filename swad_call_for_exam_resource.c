// swad_call_for_exam_resource.c: links to calls for exams as program resources

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2022 Antonio Cañas Vargas

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
#include "swad_call_for_exam.h"
#include "swad_call_for_exam_resource.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_program_database.h"

/*****************************************************************************/
/************************ Get link to call for exam **************************/
/*****************************************************************************/

void Cfe_GetLinkToCallForExam (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   struct Cfe_CallsForExams *CallsForExams = Cfe_GetGlobalCallsForExams ();
   long ExaCod;
   char Title[Cfe_MAX_BYTES_SESSION_AND_DATE];

   /***** Get the code of the call for exam *****/
   ExaCod = Cfe_GetParamExaCod ();

   /***** Get session and date of the exam *****/
   CfeRsc_GetTitleFromExaCod (ExaCod,Title,sizeof (Title) - 1);

   /***** Copy link to call for exam into resource clipboard *****/
   Prg_DB_CopyToClipboard (PrgRsc_CALL_FOR_EXAM,ExaCod);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
		  Title);

   /***** Set exam to be highlighted *****/
   Cfe_ResetCallsForExams (CallsForExams);
   CallsForExams->HighlightExaCod = ExaCod;

   /***** Show again the list of calls for exams *****/
   Cfe_ListCallsForExamsEdit ();
  }

/*****************************************************************************/
/******************* Write call for exam in course program *******************/
/*****************************************************************************/

void CfeRsc_WriteCallForExamInCrsProgram (long ExaCod,bool PutFormToGo,
                                          const char *Icon,const char *IconTitle)
  {
   extern const char *Txt_Actions[Act_NUM_ACTIONS];
   Act_Action_t NextAction;
   char Title[Cfe_MAX_BYTES_SESSION_AND_DATE];
   char *Anchor = NULL;

   /***** Get session and date of the exam *****/
   CfeRsc_GetTitleFromExaCod (ExaCod,Title,sizeof (Title) - 1);

   /***** Begin form to download file *****/
   if (PutFormToGo)
     {
      /* Build anchor string */
      Frm_SetAnchorStr (ExaCod,&Anchor);

      /* Begin form */
      NextAction = (ExaCod > 0)	? ActSeeOneCfe :	// Call for exam specified
				  ActSeeAllCfe;		// All calls for exams
      Frm_BeginFormAnchor (NextAction,Anchor);
         Cfe_PutHiddenParamExaCod (ExaCod);
	 HTM_BUTTON_Submit_Begin (Txt_Actions[NextAction],
	                          "class=\"LM BT_LINK PRG_LNK_%s\"",
	                          The_GetSuffix ());

      /* Free anchor string */
      Frm_FreeAnchorStr (Anchor);
     }

   /***** Icon depending on type ******/
   if (PutFormToGo)
      Ico_PutIconLink (Icon,Ico_BLACK,NextAction);
   else
      Ico_PutIconOn (Icon,Ico_BLACK,IconTitle);

   /***** Write Name of the course and date of exam *****/
   HTM_Txt (Title);

   /***** End form to download file *****/
   if (PutFormToGo)
     {
      /* End form */
         HTM_BUTTON_End ();

      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/************** Get call for exam text from call for exam code ***************/
/*****************************************************************************/
// The trailing null character is not counted in TitleSize

void CfeRsc_GetTitleFromExaCod (long ExaCod,char *Title,size_t TitleSize)
  {
   extern const char *Txt_Call_for_exam;
   extern const char *Txt_Calls_for_exams;
   struct Cfe_CallsForExams CallsForExams;
   char SessionAndDate[Cfe_MAX_BYTES_SESSION_AND_DATE];

   if (ExaCod > 0)
     {
      /***** Reset calls for exams context *****/
      Cfe_ResetCallsForExams (&CallsForExams);

      /***** Get data of call for exam *****/
      Cfe_AllocMemCallForExam (&CallsForExams);
      Cfe_GetDataCallForExamFromDB (&CallsForExams,ExaCod);

      /***** Session and date of the exam *****/
      Cfe_BuildSessionAndDate (&CallsForExams,SessionAndDate);
      snprintf (Title,TitleSize,"%s: %s",Txt_Call_for_exam,SessionAndDate);

      /***** Free memory of the call for exam *****/
      Cfe_FreeMemCallForExam (&CallsForExams);
     }
   else
      /***** Generic title for all calls for exams *****/
      Str_Copy (Title,Txt_Calls_for_exams,TitleSize);
  }
