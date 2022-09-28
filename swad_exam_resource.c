// swad_exam_resource.c: links to exams as program resources

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
#include "swad_error.h"
#include "swad_exam.h"
#include "swad_exam_database.h"
#include "swad_exam_resource.h"
#include "swad_form.h"
#include "swad_program_database.h"

/*****************************************************************************/
/***************************** Get link to exam ******************************/
/*****************************************************************************/

void ExaRsc_GetLinkToExam (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   struct Exa_Exams Exams;
   long ExaCod;
   char Title[Exa_MAX_BYTES_TITLE + 1];

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Err_WrongExamExit ();
   ExaCod = Exams.ExaCod;

   /***** Get exam title *****/
   ExaRsc_GetTitleFromExaCod (ExaCod,Title,sizeof (Title) - 1);

   /***** Copy link to exam into resource clipboard *****/
   Prg_DB_CopyToClipboard (PrgRsc_EXAM,ExaCod);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Title);

   /***** Show exams again *****/
   Exa_ListAllExams (&Exams);
  }

/*****************************************************************************/
/*********************** Write exam in course program ************************/
/*****************************************************************************/

void ExaRsc_WriteExamInCrsProgram (long ExaCod,bool PutFormToGo,
                                   const char *Icon,const char *IconTitle)
  {
   extern const char *Txt_Actions[Act_NUM_ACTIONS];
   Act_Action_t NextAction;
   char Title[Exa_MAX_BYTES_TITLE + 1];

   /***** Get exam title *****/
   ExaRsc_GetTitleFromExaCod (ExaCod,Title,sizeof (Title) - 1);

   /***** Begin form to go to exam *****/
   if (PutFormToGo)
     {
      NextAction = (ExaCod > 0)	? ActSeeExa :	// Exam specified
				  ActSeeAllExa;	// All exams
      Frm_BeginForm (NextAction);
         Exa_PutParamExamCod (ExaCod);
	 HTM_BUTTON_Submit_Begin (Txt_Actions[NextAction],
	                          "class=\"LM BT_LINK PRG_LNK_%s\"",
	                          The_GetSuffix ());
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
/*********************** Get exam title from exam code ***********************/
/*****************************************************************************/

void ExaRsc_GetTitleFromExaCod (long ExaCod,char *Title,size_t TitleSize)
  {
   extern const char *Txt_Exams;
   char TitleFromDB[Exa_MAX_BYTES_TITLE + 1];

   if (ExaCod > 0)
     {
      /***** Get exam title *****/
      Exa_DB_GetExamTitle (ExaCod,TitleFromDB);
      Str_Copy (Title,TitleFromDB,TitleSize);
     }
   else
      /***** Generic title for all exams *****/
      Str_Copy (Title,Txt_Exams,TitleSize);
  }
