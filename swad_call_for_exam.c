// swad_call_for_exam.c: calls for exams

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

#define _GNU_SOURCE 		// For asprintf
#include <stddef.h>		// For NULL
#include <stdio.h>		// For sscanf, asprintf, etc.
#include <stdlib.h>		// For exit, system, malloc, calloc, free, etc.
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_call_for_exam.h"
#include "swad_call_for_exam_database.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_degree_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hidden_visible.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_logo.h"
#include "swad_notification.h"
#include "swad_notification_database.h"
#include "swad_parameter.h"
#include "swad_program_database.h"
#include "swad_QR.h"
#include "swad_RSS.h"
#include "swad_string.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static long Cfe_GetParsCallsForExams (struct Cfe_CallsForExams *CallsForExams);

static void Cfe_GetExaCodToHighlight (struct Cfe_CallsForExams *CallsForExams);
static void Cfe_GetDateToHighlight (struct Cfe_CallsForExams *CallsForExams);

static void Cfe_ListCallsForExams (struct Cfe_CallsForExams *CallsForExams,
                                   Vie_ViewType_t ViewType);
static void Cfe_PutIconsCallsForExams (__attribute__((unused)) void *Args);

static void Cfe_ShowCallForExam (struct Cfe_CallsForExams *CallsForExams,
                                 long ExaCod,
			         Vie_ViewType_t ViewType,
			         bool HighLight);
static void Cfe_PutIconsCallForExam (void *CallsForExams);

static void Cfe_PutParExaCod (void *ExaCod);

static void Cfe_GetNotifContentCallForExam (const struct Cfe_CallsForExams *CallsForExams,
                                            char **ContentStr);

/*****************************************************************************/
/******************** Get global calls for exams context *********************/
/*****************************************************************************/

struct Cfe_CallsForExams *Cfe_GetGlobalCallsForExams (void)
  {
   static struct Cfe_CallsForExams Cfe_GlobalCallsForExams;	// Used to preserve information between priori and posteriori functions

   return &Cfe_GlobalCallsForExams;
  }

/*****************************************************************************/
/********************** Reset calls for exams context *********************/
/*****************************************************************************/

void Cfe_ResetCallsForExams (struct Cfe_CallsForExams *CallsForExams)
  {
   CallsForExams->NumCallsForExams = 0;
   CallsForExams->Lst              = NULL;
   CallsForExams->NewExaCod        = -1L;
   CallsForExams->HighlightExaCod  = -1L;
   CallsForExams->HighlightDate[0] = '\0';	// No calls for exams highlighted
   CallsForExams->ExaCod           = -1L;
   CallsForExams->Anchor           = NULL;

   CallsForExams->CallForExam.CrsCod = -1L;
   CallsForExams->CallForExam.Status = Cfe_STATUS_DEFAULT;
   CallsForExams->CallForExam.CrsFullName[0] = '\0';
   CallsForExams->CallForExam.Year = 0;
   CallsForExams->CallForExam.Session[0] = '\0';
   Dat_ResetDate (&CallsForExams->CallForExam.CallDate);
   Dat_ResetDate (&CallsForExams->CallForExam.ExamDate);
   Dat_ResetHour (&CallsForExams->CallForExam.StartTime);
   Dat_ResetHour (&CallsForExams->CallForExam.Duration);
   CallsForExams->CallForExam.Place       = NULL;
   CallsForExams->CallForExam.Mode        = NULL;
   CallsForExams->CallForExam.Structure   = NULL;
   CallsForExams->CallForExam.DocRequired = NULL;
   CallsForExams->CallForExam.MatRequired = NULL;
   CallsForExams->CallForExam.MatAllowed  = NULL;
   CallsForExams->CallForExam.OtherInfo   = NULL;
  }

/*****************************************************************************/
/************************ Form to edit a call for exam ***********************/
/*****************************************************************************/

void Cfe_PutFrmEditACallForExam (void)
  {
   struct Cfe_CallsForExams CallsForExams;
   long ExaCod;

   /***** Reset calls for exams context *****/
   Cfe_ResetCallsForExams (&CallsForExams);

   /***** Allocate memory for the call for exam *****/
   Cfe_AllocMemCallForExam (&CallsForExams);

   /***** Get the code of the call for exam *****/
   ExaCod = Cfe_GetParsCallsForExams (&CallsForExams);

   if (ExaCod > 0)	// -1 indicates that this is a new call for exam
      /***** Read call for exam from the database *****/
      Cfe_GetCallForExamDataByCod (&CallsForExams,ExaCod);

   /***** Show call for exam *****/
   Cfe_ShowCallForExam (&CallsForExams,ExaCod,Vie_EDIT,
		        false);	// Don't highlight

   /***** Free memory of the call for exam *****/
   Cfe_FreeMemCallForExam (&CallsForExams);
  }

/*****************************************************************************/
/****************** Get parameters of a call for exam ************************/
/*****************************************************************************/

static long Cfe_GetParsCallsForExams (struct Cfe_CallsForExams *CallsForExams)
  {
   long ExaCod;

   /***** Get the code of the call for exam *****/
   ExaCod = ParCod_GetPar (ParCod_Exa);

   /***** Get the name of the course (it is allowed to be different from the official name of the course) *****/
   Par_GetParText ("CrsName",CallsForExams->CallForExam.CrsFullName,Nam_MAX_BYTES_FULL_NAME);
   // If the parameter is not present or is empty, initialize the string to the full name of the current course
   if (!CallsForExams->CallForExam.CrsFullName[0])
      Str_Copy (CallsForExams->CallForExam.CrsFullName,
	        Gbl.Hierarchy.Node[Hie_CRS].FullName,
                sizeof (CallsForExams->CallForExam.CrsFullName) - 1);

   /***** Get the year *****/
   CallsForExams->CallForExam.Year = (unsigned)
   Par_GetParUnsignedLong ("Year",
			   0,	// N.A.
			   Deg_MAX_YEARS_PER_DEGREE,
			   (unsigned long) Gbl.Hierarchy.Node[Hie_CRS].Specific.Year);

   /***** Get the type of call for exam *****/
   Par_GetParText ("ExamSession",CallsForExams->CallForExam.Session,Cfe_MAX_BYTES_SESSION);

   /***** Get the date of the exam *****/
   Dat_GetDateFromForm ("ExamDay","ExamMonth","ExamYear",
                        &CallsForExams->CallForExam.ExamDate.Day,
                        &CallsForExams->CallForExam.ExamDate.Month,
                        &CallsForExams->CallForExam.ExamDate.Year);
   if (CallsForExams->CallForExam.ExamDate.Day   == 0 ||
       CallsForExams->CallForExam.ExamDate.Month == 0 ||
       CallsForExams->CallForExam.ExamDate.Year  == 0)
     {
      CallsForExams->CallForExam.ExamDate.Day   = Dat_GetCurrentDay ();
      CallsForExams->CallForExam.ExamDate.Month = Dat_GetCurrentMonth ();
      CallsForExams->CallForExam.ExamDate.Year  = Dat_GetCurrentYear ();
     }

   /***** Get the hour of the exam *****/
   CallsForExams->CallForExam.StartTime.Hour   = (unsigned) Par_GetParUnsignedLong ("ExamHour",
                                                                                    0,23,0);
   CallsForExams->CallForExam.StartTime.Minute = (unsigned) Par_GetParUnsignedLong ("ExamMinute",
                                                                                    0,59,0);

   /***** Get the duration of the exam *****/
   CallsForExams->CallForExam.Duration.Hour    = (unsigned) Par_GetParUnsignedLong ("DurationHour",
                                                                                    0,23,0);
   CallsForExams->CallForExam.Duration.Minute  = (unsigned) Par_GetParUnsignedLong ("DurationMinute",
                                                                                    0,59,0);

   /***** Get the place where the exam will happen, the modality of exam,
          the structure of exam, the mandatory documentation, the mandatory material,
          the allowed material and other information *****/
   Par_GetParHTML ("Place"      ,CallsForExams->CallForExam.Place      ,Cns_MAX_BYTES_TEXT);
   Par_GetParHTML ("ExamMode"   ,CallsForExams->CallForExam.Mode       ,Cns_MAX_BYTES_TEXT);
   Par_GetParHTML ("Structure"  ,CallsForExams->CallForExam.Structure  ,Cns_MAX_BYTES_TEXT);
   Par_GetParHTML ("DocRequired",CallsForExams->CallForExam.DocRequired,Cns_MAX_BYTES_TEXT);
   Par_GetParHTML ("MatRequired",CallsForExams->CallForExam.MatRequired,Cns_MAX_BYTES_TEXT);
   Par_GetParHTML ("MatAllowed" ,CallsForExams->CallForExam.MatAllowed ,Cns_MAX_BYTES_TEXT);
   Par_GetParHTML ("OtherInfo"  ,CallsForExams->CallForExam.OtherInfo  ,Cns_MAX_BYTES_TEXT);

   return ExaCod;
  }

/*****************************************************************************/
/* Allocate memory for those parameters of an exam anno. with a lot of text **/
/*****************************************************************************/

void Cfe_AllocMemCallForExam (struct Cfe_CallsForExams *CallsForExams)
  {
   if ((CallsForExams->CallForExam.Place       = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   if ((CallsForExams->CallForExam.Mode        = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   if ((CallsForExams->CallForExam.Structure   = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   if ((CallsForExams->CallForExam.DocRequired = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   if ((CallsForExams->CallForExam.MatRequired = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   if ((CallsForExams->CallForExam.MatAllowed  = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   if ((CallsForExams->CallForExam.OtherInfo   = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();
  }

/*****************************************************************************/
/* Free memory of those parameters of an exam announcem. with a lot of text **/
/*****************************************************************************/

void Cfe_FreeMemCallForExam (struct Cfe_CallsForExams *CallsForExams)
  {
   if (CallsForExams->CallForExam.Place)
     {
      free (CallsForExams->CallForExam.Place);
      CallsForExams->CallForExam.Place = NULL;
     }
   if (CallsForExams->CallForExam.Mode)
     {
      free (CallsForExams->CallForExam.Mode);
      CallsForExams->CallForExam.Mode = NULL;
     }
   if (CallsForExams->CallForExam.Structure)
     {
      free (CallsForExams->CallForExam.Structure);
      CallsForExams->CallForExam.Structure = NULL;
     }
   if (CallsForExams->CallForExam.DocRequired)
     {
      free (CallsForExams->CallForExam.DocRequired);
      CallsForExams->CallForExam.DocRequired = NULL;
     }
   if (CallsForExams->CallForExam.MatRequired)
     {
      free (CallsForExams->CallForExam.MatRequired);
      CallsForExams->CallForExam.MatRequired = NULL;
     }
   if (CallsForExams->CallForExam.MatAllowed)
     {
      free (CallsForExams->CallForExam.MatAllowed);
      CallsForExams->CallForExam.MatAllowed = NULL;
     }
   if (CallsForExams->CallForExam.OtherInfo)
     {
      free (CallsForExams->CallForExam.OtherInfo);
      CallsForExams->CallForExam.OtherInfo = NULL;
     }
  }

/*****************************************************************************/
/************************** Receive a call for exam **************************/
/*****************************************************************************/
// This function is splitted into a-priori and a-posteriori functions
// in order to view updated links in month of left column

void Cfe_ReceiveCallForExam1 (void)
  {
   extern const char *Txt_Created_new_call_for_exam;
   extern const char *Txt_The_call_for_exam_has_been_successfully_updated;
   struct Cfe_CallsForExams *CallsForExams = Cfe_GetGlobalCallsForExams ();
   long ExaCod;
   bool NewCallForExam;
   char *Anchor = NULL;

   /***** Reset calls for exams context *****/
   Cfe_ResetCallsForExams (CallsForExams);

   /***** Allocate memory for the call for exam *****/
   Cfe_AllocMemCallForExam (CallsForExams);

   /***** Get parameters of the call for exam *****/
   ExaCod = Cfe_GetParsCallsForExams (CallsForExams);
   NewCallForExam = (ExaCod < 0);

   /***** Add the call for exam to the database and read it again from the database *****/
   if (NewCallForExam)
      CallsForExams->NewExaCod = ExaCod = Cfe_DB_CreateCallForExam (&CallsForExams->CallForExam);
   else
      Cfe_DB_ModifyCallForExam (&CallsForExams->CallForExam,ExaCod);

   /***** Free memory of the call for exam *****/
   Cfe_FreeMemCallForExam (CallsForExams);

   /***** Create alert to show the change made *****/
   Frm_SetAnchorStr (ExaCod,&Anchor);
   Ale_CreateAlert (Ale_SUCCESS,Anchor,
                    NewCallForExam ? Txt_Created_new_call_for_exam :
                                     Txt_The_call_for_exam_has_been_successfully_updated);
   Frm_FreeAnchorStr (&Anchor);

   /***** Set exam to be highlighted *****/
   CallsForExams->HighlightExaCod = ExaCod;
  }

void Cfe_ReceiveCallForExam2 (void)
  {
   struct Cfe_CallsForExams *CallsForExams = Cfe_GetGlobalCallsForExams ();
   unsigned NumUsrsToBeNotifiedByEMail;

   /***** Notify by email about the new call for exam *****/
   if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_CALL_FOR_EXAM,CallsForExams->HighlightExaCod)))
      Cfe_DB_UpdateNumUsrsNotifiedByEMailAboutCallForExam (CallsForExams->HighlightExaCod,NumUsrsToBeNotifiedByEMail);

   /***** Create a new social note about the new call for exam *****/
   TmlNot_StoreAndPublishNote (TmlNot_CALL_FOR_EXAM,CallsForExams->HighlightExaCod);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.Hierarchy.Node[Hie_CRS]);

   /***** Show calls for exams *****/
   Cfe_ListCallsForExamsEdit ();
  }

/*****************************************************************************/
/*************************** Print a call for exam ***************************/
/*****************************************************************************/

void Cfe_PrintCallForExam (void)
  {
   struct Cfe_CallsForExams CallsForExams;
   long ExaCod;

   /***** Reset calls for exams context *****/
   Cfe_ResetCallsForExams (&CallsForExams);

   /***** Allocate memory for the call for exam *****/
   Cfe_AllocMemCallForExam (&CallsForExams);

   /***** Get the code of the call for exam *****/
   ExaCod = ParCod_GetAndCheckPar (ParCod_Exa);

   /***** Read call for exam from the database *****/
   Cfe_GetCallForExamDataByCod (&CallsForExams,ExaCod);

   /***** Show call for exam *****/
   Cfe_ShowCallForExam (&CallsForExams,ExaCod,Vie_PRINT,
			false);	// Don't highlight

   /***** Free memory of the call for exam *****/
   Cfe_FreeMemCallForExam (&CallsForExams);
  }

/*****************************************************************************/
/************************** Remove a call for exam ***************************/
/*****************************************************************************/

void Cfe_ReqRemCallForExam (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_following_call_for_exam;
   struct Cfe_CallsForExams CallsForExams;
   long ExaCod;

   /***** Reset calls for exams context *****/
   Cfe_ResetCallsForExams (&CallsForExams);

   /***** Get the code of the call for exam *****/
   ExaCod = ParCod_GetAndCheckPar (ParCod_Exa);

   /***** Show question and button to remove call for exam *****/
   /* Begin alert */
   Ale_ShowAlertAndButtonBegin (Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_following_call_for_exam);

      /* Show call for exam */
      Cfe_AllocMemCallForExam (&CallsForExams);
      Cfe_GetCallForExamDataByCod (&CallsForExams,ExaCod);
      Cfe_ShowCallForExam (&CallsForExams,ExaCod,Vie_VIEW,
			   false);	// Don't highlight
      Cfe_FreeMemCallForExam (&CallsForExams);

   /* End alert */
   Ale_ShowAlertAndButtonEnd (ActRemCfe,NULL,NULL,
                            Cfe_PutParExaCod,&CallsForExams.ExaCod,
			    Btn_REMOVE_BUTTON,Act_GetActionText (ActRemCfe));
  }

/*****************************************************************************/
/************************** Remove a call for exam ***************************/
/*****************************************************************************/
// This function is splitted into a-priori and a-posteriori functions
// in order to view updated links in month of left column

void Cfe_RemoveCallForExam1 (void)
  {
   struct Cfe_CallsForExams *CallsForExams = Cfe_GetGlobalCallsForExams ();
   long ExaCod;

   /***** Reset calls for exams context *****/
   Cfe_ResetCallsForExams (CallsForExams);

   /***** Get the code of the call for exam *****/
   ExaCod = ParCod_GetAndCheckPar (ParCod_Exa);

   /***** Mark the call for exam as deleted in the database *****/
   Cfe_DB_MarkACallForExamAsDeleted (ExaCod);

   /***** Mark possible notifications as removed *****/
   Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_CALL_FOR_EXAM,ExaCod);

   /***** Mark possible social note as unavailable *****/
   Tml_DB_MarkNoteAsUnavailable (TmlNot_CALL_FOR_EXAM,ExaCod);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.Hierarchy.Node[Hie_CRS]);
  }

void Cfe_RemoveCallForExam2 (void)
  {
   extern const char *Txt_Call_for_exam_removed;

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Call_for_exam_removed);

   /***** List again all the remaining calls for exams *****/
   Cfe_ListCallsForExamsEdit ();
  }

/*****************************************************************************/
/************************** Hide a call for exam *****************************/
/*****************************************************************************/
// This function is splitted into a-priori and a-posteriori functions
// in order to view updated links in month of left column

void Cfe_HideCallForExam (void)
  {
   struct Cfe_CallsForExams *CallsForExams = Cfe_GetGlobalCallsForExams ();
   long ExaCod;

   /***** Reset calls for exams context *****/
   Cfe_ResetCallsForExams (CallsForExams);

   /***** Get the code of the call for exam *****/
   ExaCod = ParCod_GetAndCheckPar (ParCod_Exa);

   /***** Mark the call for exam as hidden in the database *****/
   Cfe_DB_HideCallForExam (ExaCod);

   /***** Set exam to be highlighted *****/
   CallsForExams->HighlightExaCod = ExaCod;
  }

/*****************************************************************************/
/************************** Unhide a call for exam ***************************/
/*****************************************************************************/
// This function is splitted into a-priori and a-posteriori functions
// in order to view updated links in month of left column

void Cfe_UnhideCallForExam (void)
  {
   struct Cfe_CallsForExams *CallsForExams = Cfe_GetGlobalCallsForExams ();
   long ExaCod;

   /***** Reset calls for exams context *****/
   Cfe_ResetCallsForExams (CallsForExams);

   /***** Get the code of the call for exam *****/
   ExaCod = ParCod_GetAndCheckPar (ParCod_Exa);

   /***** Mark the call for exam as visible in the database *****/
   Cfe_DB_UnhideCallForExam (ExaCod);

   /***** Set exam to be highlighted *****/
   CallsForExams->HighlightExaCod = ExaCod;
  }

/*****************************************************************************/
/****************** List all calls for exams to see them *********************/
/*****************************************************************************/

void Cfe_ListCallsForExamsSee (void)
  {
   struct Cfe_CallsForExams CallsForExams;

   /***** Reset calls for exams context *****/
   Cfe_ResetCallsForExams (&CallsForExams);

   /***** List all calls for exams *****/
   Cfe_ListCallsForExams (&CallsForExams,Vie_VIEW);

   /***** Mark possible notifications as seen *****/
   Ntf_DB_MarkNotifsInCrsAsSeen (Ntf_EVENT_CALL_FOR_EXAM);
  }

/*****************************************************************************/
/************** List all calls for exams to edit or remove them **************/
/*****************************************************************************/

void Cfe_ListCallsForExamsEdit (void)
  {
   struct Cfe_CallsForExams *CallsForExams = Cfe_GetGlobalCallsForExams ();

   Cfe_ListCallsForExams (CallsForExams,Vie_VIEW);
  }

/*****************************************************************************/
/************** List call for exam given a call for exam code ****************/
/*****************************************************************************/

void Cfe_ListCallsForExamsCod (void)
  {
   struct Cfe_CallsForExams CallsForExams;

   /***** Reset calls for exams context *****/
   Cfe_ResetCallsForExams (&CallsForExams);

   /***** Get call for exam code *****/
   Cfe_GetExaCodToHighlight (&CallsForExams);

   /***** List all calls for exams *****/
   Cfe_ListCallsForExams (&CallsForExams,Vie_VIEW);
  }

/*****************************************************************************/
/****************** List calls for exams on a given date *********************/
/*****************************************************************************/

void Cfe_ListCallsForExamsDay (void)
  {
   struct Cfe_CallsForExams CallsForExams;

   /***** Reset calls for exams context *****/
   Cfe_ResetCallsForExams (&CallsForExams);

   /***** Get date *****/
   Cfe_GetDateToHighlight (&CallsForExams);

   /***** List all calls for exams *****/
   Cfe_ListCallsForExams (&CallsForExams,Vie_VIEW);
  }

/*****************************************************************************/
/************ Get date of calls for exams to show highlighted ****************/
/*****************************************************************************/

static void Cfe_GetExaCodToHighlight (struct Cfe_CallsForExams *CallsForExams)
  {
   /***** Get the call for exam code
          of the call for exam to highlight *****/
   CallsForExams->HighlightExaCod = ParCod_GetPar (ParCod_Exa);
  }

/*****************************************************************************/
/************ Get date of calls for exams to show highlighted ****************/
/*****************************************************************************/

static void Cfe_GetDateToHighlight (struct Cfe_CallsForExams *CallsForExams)
  {
   /***** Get the date (in YYYYMMDD format)
          of the calls for exams to highlight *****/
   Par_GetParText ("Date",CallsForExams->HighlightDate,4 + 2 + 2);
  }

/*****************************************************************************/
/*********************** List all calls for exams ****************************/
/*****************************************************************************/

static void Cfe_ListCallsForExams (struct Cfe_CallsForExams *CallsForExams,
                                   Vie_ViewType_t ViewType)
  {
   extern const char *Hlp_ASSESSMENT_Calls_for_exams;
   extern const char *Txt_Calls_for_exams;
   extern const char *Txt_No_calls_for_exams_of_X;
   MYSQL_RES *mysql_res;
   unsigned NumCalls;
   unsigned NumCall;
   long ExaCod;
   bool HighLight;

   /***** Get calls for exams (the most recent first)
          in current course from database *****/
   NumCalls = Cfe_DB_GetCallsForExamsInCurrentCrs (&mysql_res);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Calls_for_exams,Cfe_PutIconsCallsForExams,NULL,
		 Hlp_ASSESSMENT_Calls_for_exams,Box_NOT_CLOSABLE);

      /***** List the existing calls for exams *****/
      if (NumCalls)
	 for (NumCall = 0;
	      NumCall < NumCalls;
	      NumCall++)
	   {
	    /***** Get the code of the call for exam (row[0]) *****/
	    if ((ExaCod = DB_GetNextCode (mysql_res)) <= 0)
	       Err_WrongCallForExamExit ();

	    /***** Allocate memory for the call for exam *****/
	    Cfe_AllocMemCallForExam (CallsForExams);

	    /***** Read the data of the call for exam *****/
	    Cfe_GetCallForExamDataByCod (CallsForExams,ExaCod);

	    /***** Show call for exam *****/
	    HighLight = false;
	    if (ExaCod == CallsForExams->HighlightExaCod)
	       HighLight = true;
	    else if (CallsForExams->HighlightDate[0])
	      {
	       if (!strcmp (CallsForExams->CallForExam.ExamDate.YYYYMMDD,
			    CallsForExams->HighlightDate))
		  HighLight = true;
	      }
	    Cfe_ShowCallForExam (CallsForExams,ExaCod,ViewType,HighLight);

	    /***** Free memory of the call for exam *****/
	    Cfe_FreeMemCallForExam (CallsForExams);
	   }
      else
	 Ale_ShowAlert (Ale_INFO,Txt_No_calls_for_exams_of_X,
			Gbl.Hierarchy.Node[Hie_CRS].FullName);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*********************** Put icons in calls for exams ************************/
/*****************************************************************************/

static void Cfe_PutIconsCallsForExams (__attribute__((unused)) void *Args)
  {
   /***** Icon to create a new call for exam *****/
   if (Cfe_CheckIfICanEditCallsForExams () == Usr_CAN)
      Ico_PutContextualIconToAdd (ActEdiCfe,NULL,NULL,NULL);

   /***** Icon to get resource link *****/
   if (Rsc_CheckIfICanGetLink () == Usr_CAN)
      Ico_PutContextualIconToGetLink (ActReqLnkCfe,NULL,NULL,NULL);
  }

/*****************************************************************************/
/********** Create a list with the dates of all calls for exams **************/
/*****************************************************************************/

void Cfe_CreateListCallsForExams (struct Cfe_CallsForExams *CallsForExams)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumExaAnns;
   unsigned NumExaAnn;

   if (DB_CheckIfDatabaseIsOpen ())
     {
      /***** Get exam dates (ordered from more recent to older)
             of visible calls for exams
             in current course from database *****/
      NumExaAnns = Cfe_DB_GetVisibleCallsForExamsInCurrentCrs (&mysql_res);

      /***** The result of the query may be empty *****/
      CallsForExams->Lst = NULL;
      CallsForExams->NumCallsForExams = 0;
      if (NumExaAnns)
	{
	 /***** Allocate memory for the list *****/
	 if ((CallsForExams->Lst = calloc (NumExaAnns,sizeof (*CallsForExams->Lst))) == NULL)
	    Err_NotEnoughMemoryExit ();

	 /***** Get the dates of the existing calls for exams *****/
	 for (NumExaAnn = 0;
	      NumExaAnn < NumExaAnns;
	      NumExaAnn++)
	   {
	    /***** Get next call for exam *****/
	    row = mysql_fetch_row (mysql_res);

	    /* Get exam code (row[0]) */
	    CallsForExams->Lst[CallsForExams->NumCallsForExams].ExaCod = Str_ConvertStrCodToLongCod (row[0]);

	    /* Read the date of the exam (row[1]) */
	    if (sscanf (row[1],"%04u-%02u-%02u",
			&CallsForExams->Lst[CallsForExams->NumCallsForExams].ExamDate.Year,
			&CallsForExams->Lst[CallsForExams->NumCallsForExams].ExamDate.Month,
			&CallsForExams->Lst[CallsForExams->NumCallsForExams].ExamDate.Day) != 3)
	       Err_WrongCallForExamExit ();

	    /***** Increment number of elements in list *****/
	    CallsForExams->NumCallsForExams++;
	   }
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/****************** Free list of dates of calls for exams ********************/
/*****************************************************************************/

void Cfe_FreeListCallsForExams (struct Cfe_CallsForExams *CallsForExams)
  {
   if (CallsForExams->Lst)
     {
      free (CallsForExams->Lst);
      CallsForExams->Lst = NULL;
      CallsForExams->NumCallsForExams = 0;
     }
  }

/*****************************************************************************/
/*********** Read the data of a call for exam from the database **************/
/*****************************************************************************/

void Cfe_GetCallForExamDataByCod (struct Cfe_CallsForExams *CallsForExams,
                                  long ExaCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;
   unsigned Hour;
   unsigned Minute;
   unsigned Second;

   /***** Get data of a call for exam from database *****/
   if (Cfe_DB_GetCallForExamDataByCod (&mysql_res,ExaCod) != 1)
      Err_WrongCallForExamExit ();

   /***** Get the data of the call for exam *****/
   row = mysql_fetch_row (mysql_res);

   /* Code of the course in which the call for exam is inserted (row[0]) */
   CallsForExams->CallForExam.CrsCod = Str_ConvertStrCodToLongCod (row[0]);

   /* Status of the call for exam (row[1]) */
   if (sscanf (row[1],"%u",&UnsignedNum) != 1)
      Err_WrongCallForExamExit ();
   if (UnsignedNum >= Cfe_NUM_STATUS)
      Err_WrongCallForExamExit ();
   CallsForExams->CallForExam.Status = (Cfe_Status_t) UnsignedNum;

   /* Name of the course (row[2]) */
   Str_Copy (CallsForExams->CallForExam.CrsFullName,row[2],
             sizeof (CallsForExams->CallForExam.CrsFullName) - 1);

   /* Year (row[3]) */
   if (sscanf (row[3],"%u",&CallsForExams->CallForExam.Year) != 1)
      Err_WrongCallForExamExit ();

   /* Exam session (row[4]) */
   Str_Copy (CallsForExams->CallForExam.Session,row[4],
             sizeof (CallsForExams->CallForExam.Session) - 1);

   /* Date of call for exam (row[5]) */
   if (sscanf (row[5],"%04u-%02u-%02u %02u:%02u:%02u",
               &CallsForExams->CallForExam.CallDate.Year,
               &CallsForExams->CallForExam.CallDate.Month,
               &CallsForExams->CallForExam.CallDate.Day,
               &Hour,
	       &Minute,
	       &Second) != 6)
      Err_WrongCallForExamExit ();

   /* Date of exam (row[6]) */
   if (sscanf (row[6],"%04u-%02u-%02u %02u:%02u:%02u",
   	       &CallsForExams->CallForExam.ExamDate.Year,
	       &CallsForExams->CallForExam.ExamDate.Month,
	       &CallsForExams->CallForExam.ExamDate.Day,
               &CallsForExams->CallForExam.StartTime.Hour,
	       &CallsForExams->CallForExam.StartTime.Minute,
	       &Second) != 6)
      Err_WrongCallForExamExit ();
   snprintf (CallsForExams->CallForExam.ExamDate.YYYYMMDD,
             sizeof (CallsForExams->CallForExam.ExamDate.YYYYMMDD),"%04u%02u%02u",
             CallsForExams->CallForExam.ExamDate.Year,
	     CallsForExams->CallForExam.ExamDate.Month,
	     CallsForExams->CallForExam.ExamDate.Day);

   /* Approximate duration (row[7]) */
   if (sscanf (row[7],"%02u:%02u:%02u",
               &CallsForExams->CallForExam.Duration.Hour,
               &CallsForExams->CallForExam.Duration.Minute,
               &Second) != 3)
      Err_WrongCallForExamExit ();

   /* Place (row[8]), exam mode (row[9]), structure (row[10]),
      documentation required (row[11]), material required (row[12]),
      material allowed (row[13]) and other information for students (row[14]) */
   Str_Copy (CallsForExams->CallForExam.Place      ,row[ 8],Cns_MAX_BYTES_TEXT);
   Str_Copy (CallsForExams->CallForExam.Mode       ,row[ 9],Cns_MAX_BYTES_TEXT);
   Str_Copy (CallsForExams->CallForExam.Structure  ,row[10],Cns_MAX_BYTES_TEXT);
   Str_Copy (CallsForExams->CallForExam.DocRequired,row[11],Cns_MAX_BYTES_TEXT);
   Str_Copy (CallsForExams->CallForExam.MatRequired,row[12],Cns_MAX_BYTES_TEXT);
   Str_Copy (CallsForExams->CallForExam.MatAllowed ,row[13],Cns_MAX_BYTES_TEXT);
   Str_Copy (CallsForExams->CallForExam.OtherInfo  ,row[14],Cns_MAX_BYTES_TEXT);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Show a form with the data of a call for exam *****************/
/*****************************************************************************/

static void Cfe_ShowCallForExam (struct Cfe_CallsForExams *CallsForExams,
                                 long ExaCod,
				 Vie_ViewType_t ViewType,
				 bool HighLight)
  {
   extern unsigned Nam_MaxChars[Nam_NUM_SHRT_FULL_NAMES];
   extern const char *Hlp_ASSESSMENT_Calls_for_exams_new_call;
   extern const char *Hlp_ASSESSMENT_Calls_for_exams_edit_call;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_Call_for_exam;
   extern const char *Txt_CALL_FOR_EXAM_Course;
   extern const char *Txt_CALL_FOR_EXAM_Year_or_semester;
   extern const char *Txt_CALL_FOR_EXAM_Session;
   extern const char *Txt_CALL_FOR_EXAM_Exam_date;
   extern const char *Txt_CALL_FOR_EXAM_Start_time;
   extern const char *Txt_CALL_FOR_EXAM_Approximate_duration;
   extern const char *Txt_CALL_FOR_EXAM_Place_of_exam;
   extern const char *Txt_CALL_FOR_EXAM_Mode;
   extern const char *Txt_CALL_FOR_EXAM_Structure_of_the_exam;
   extern const char *Txt_CALL_FOR_EXAM_Documentation_required;
   extern const char *Txt_CALL_FOR_EXAM_Material_required;
   extern const char *Txt_CALL_FOR_EXAM_Material_allowed;
   extern const char *Txt_CALL_FOR_EXAM_Other_information;
   extern const char *Txt_hours_ABBREVIATION;
   extern const char *Txt_hour;
   extern const char *Txt_hours;
   extern const char *Txt_minute;
   extern const char *Txt_minutes;
   extern const char *Txt_Publish_call_FOR_EXAM;
   char StrExamDate[Cns_MAX_BYTES_DATE + 1];
   unsigned CurrentYear = Dat_GetCurrentYear ();
   unsigned Year;
   unsigned Hour;
   unsigned Minute;
   char *Anchor = NULL;
   void (*FunctionToDrawContextualIcons) (void *Args);
   const char *HelpLink;
   static const char *ClassCallForExam[Vie_NUM_VIEW_TYPES][Cfe_NUM_STATUS] =
     {
      [Vie_VIEW ][Cfe_VISIBLE_CALL_FOR_EXAM] = "CALL_FOR_EXAM_VISIBLE",
      [Vie_VIEW ][Cfe_HIDDEN_CALL_FOR_EXAM ] = "CALL_FOR_EXAM_HIDDEN",
      [Vie_VIEW ][Cfe_DELETED_CALL_FOR_EXAM] = NULL,	// Not applicable here

      [Vie_EDIT ][Cfe_VISIBLE_CALL_FOR_EXAM] = "CALL_FOR_EXAM_VISIBLE",
      [Vie_EDIT ][Cfe_HIDDEN_CALL_FOR_EXAM ] = "CALL_FOR_EXAM_VISIBLE",
      [Vie_EDIT ][Cfe_DELETED_CALL_FOR_EXAM] = NULL,	// Not applicable here

      [Vie_PRINT][Cfe_VISIBLE_CALL_FOR_EXAM] = "CALL_FOR_EXAM_VISIBLE",
      [Vie_PRINT][Cfe_HIDDEN_CALL_FOR_EXAM ] = "CALL_FOR_EXAM_VISIBLE",
      [Vie_PRINT][Cfe_DELETED_CALL_FOR_EXAM] = NULL,	// Not applicable here
     };

   /***** Build anchor string *****/
   Frm_SetAnchorStr (ExaCod,&Anchor);

   /***** Begin article *****/
   if (ViewType == Vie_VIEW)
      HTM_ARTICLE_Begin (Anchor);

   /***** Begin box *****/
   CallsForExams->Anchor = Anchor;	// Used to put contextual icons
   CallsForExams->ExaCod = ExaCod;	// Used to put contextual icons
   FunctionToDrawContextualIcons = ViewType == Vie_VIEW ? Cfe_PutIconsCallForExam :
							  NULL;
   HelpLink = ViewType == Vie_EDIT ? ((ExaCod > 0) ? Hlp_ASSESSMENT_Calls_for_exams_edit_call :
						     Hlp_ASSESSMENT_Calls_for_exams_new_call) :
				     NULL;
   if (HighLight)
     {
      /* Show pending alerts */
      Ale_ShowAlerts (Anchor);

      /* Begin highlighted box */
      Box_BoxShadowBegin (NULL,FunctionToDrawContextualIcons,CallsForExams,
                          HelpLink);
     }
   else	// Don't highlight
      /* Begin normal box */
      Box_BoxBegin (NULL,FunctionToDrawContextualIcons,CallsForExams,
                    HelpLink,Box_NOT_CLOSABLE);

   if (ViewType == Vie_EDIT)
     {
      /***** Begin form *****/
      Frm_BeginFormAnchor (ActRcvCfe,Anchor);
         ParCod_PutPar (ParCod_Exa,ExaCod);
     }

      /***** Begin table *****/
      HTM_TABLE_Begin ("%s CELLS_PAD_2",
		       ClassCallForExam[ViewType][CallsForExams->CallForExam.Status]);

	 /***** Institution logo *****/
	 HTM_TR_Begin (NULL);
	    HTM_TD_Begin ("colspan=\"2\" class=\"CM EXAM_TIT_%s\"",
	                  The_GetSuffix ());
	       if (ViewType == Vie_VIEW)
		  HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"EXAM_TIT_%s\"",
			       Gbl.Hierarchy.Node[Hie_INS].WWW,The_GetSuffix ());
	       Lgo_DrawLogo (Hie_INS,&Gbl.Hierarchy.Node[Hie_INS],"ICO64x64");
	       HTM_BR ();
	       HTM_Txt (Gbl.Hierarchy.Node[Hie_INS].FullName);
	       if (ViewType == Vie_VIEW)
		  HTM_A_End ();
	    HTM_TD_End ();
	 HTM_TR_End ();

	 /***** Degree *****/
	 HTM_TR_Begin (NULL);
	    HTM_TD_Begin ("colspan=\"2\" class=\"CM EXAM_TIT_%s\"",
	                  The_GetSuffix ());
	       if (ViewType == Vie_VIEW)
		  HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"EXAM_TIT_%s\"",
			       Gbl.Hierarchy.Node[Hie_DEG].WWW,
			       The_GetSuffix ());
	       HTM_Txt (Gbl.Hierarchy.Node[Hie_DEG].FullName);
	       if (ViewType == Vie_VIEW)
		  HTM_A_End ();
	    HTM_TD_End ();
	 HTM_TR_End ();

	 /***** Title *****/
	 HTM_TR_Begin (NULL);
	    HTM_TD_Begin ("colspan=\"2\" class=\"CM CALL_FOR_EXAM_TIT DAT_STRONG_%s\"",
	                  The_GetSuffix ());
	       HTM_NBSP ();
	       HTM_BR ();
		  HTM_TxtF (Txt_Call_for_exam);
	       HTM_BR ();
	       HTM_NBSP ();
	    HTM_TD_End ();
	 HTM_TR_End ();

	 /***** Name of the course *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT",
			     ViewType == Vie_EDIT ? "CrsName" :
						    NULL,
			     Txt_CALL_FOR_EXAM_Course);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
	       if (ViewType == Vie_EDIT)
		  HTM_INPUT_TEXT ("CrsName",Nam_MaxChars[Nam_FULL_NAME],
				  CallsForExams->CallForExam.CrsFullName,
				  HTM_ENABLED,HTM_DONT_SUBMIT_ON_CHANGE,
				  "id=\"CrsName\" size=\"30\""
				  " class=\"Frm_C2_INPUT INPUT_%s\"",
				  The_GetSuffix ());
	       else
		 {
		  HTM_STRONG_Begin ();
		     HTM_Txt (CallsForExams->CallForExam.CrsFullName);
		  HTM_STRONG_End ();
		 }
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Year/semester (N.A., 1º, 2º, 3º, 4º, 5º...) *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT",
			     ViewType == Vie_EDIT ? "Year" :
						    NULL,
			     Txt_CALL_FOR_EXAM_Year_or_semester);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
	       if (ViewType == Vie_EDIT)
		 {
		  HTM_SELECT_Begin (HTM_ENABLED,HTM_DONT_SUBMIT_ON_CHANGE,NULL,
				    "id=\"Year\" name=\"Year\""
				    " class=\"INPUT_%s\"",
				    The_GetSuffix ());
		     for (Year  = 0;
			  Year <= Deg_MAX_YEARS_PER_DEGREE;
			  Year++)
			HTM_OPTION (HTM_Type_UNSIGNED,&Year,
				    Year == CallsForExams->CallForExam.Year ? HTM_OPTION_SELECTED :
									      HTM_OPTION_UNSELECTED,
				    HTM_ENABLED,
				    "%s",Txt_YEAR_OF_DEGREE[Year]);
		  HTM_SELECT_End ();
		 }
	       else
		  HTM_Txt (Txt_YEAR_OF_DEGREE[CallsForExams->CallForExam.Year]);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Exam session *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT",
			     ViewType == Vie_EDIT ? "ExamSession" :
						    NULL,
			     Txt_CALL_FOR_EXAM_Session);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
	       if (ViewType == Vie_EDIT)
		  HTM_INPUT_TEXT ("ExamSession",Cfe_MAX_CHARS_SESSION,CallsForExams->CallForExam.Session,
				  HTM_ENABLED,HTM_DONT_SUBMIT_ON_CHANGE,
				  "id=\"ExamSession\" size=\"30\""
				  " class=\"Frm_C2_INPUT INPUT_%s\"",
				  The_GetSuffix ());
	       else
		  HTM_Txt (CallsForExams->CallForExam.Session);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Date of the exam *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT",
			     ViewType == Vie_EDIT ? "ExamYear" :
						    NULL,
			     Txt_CALL_FOR_EXAM_Exam_date);

	    /* Data */
	    if (ViewType == Vie_EDIT)
	      {
	       HTM_TD_Begin ("class=\"Frm_C2 LB\"");
		  Dat_WriteFormDate (CallsForExams->CallForExam.ExamDate.Year < CurrentYear ? CallsForExams->CallForExam.ExamDate.Year :
											      CurrentYear,
				     CurrentYear + 1,"Exam",
				     &(CallsForExams->CallForExam.ExamDate),
				     HTM_DONT_SUBMIT_ON_CHANGE,
				     false);
	       HTM_TD_End ();
	      }
	    else
	      {
	       Dat_ConvDateToDateStr (&CallsForExams->CallForExam.ExamDate,
				      StrExamDate);
	       HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
		  HTM_Txt (StrExamDate);
	       HTM_TD_End ();
	      }
	 HTM_TR_End ();

	 /***** Start time *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT",
			     ViewType == Vie_EDIT ? "ExamHour" :
						    NULL,
			     Txt_CALL_FOR_EXAM_Start_time);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
	       if (ViewType == Vie_EDIT)
		 {
		  HTM_SELECT_Begin (HTM_ENABLED,HTM_DONT_SUBMIT_ON_CHANGE,NULL,
				    "id=\"ExamHour\" name=\"ExamHour\""
				    " class=\"INPUT_%s\"",
				    The_GetSuffix ());
		     HTM_OPTION (HTM_Type_STRING,"0",
				 CallsForExams->CallForExam.StartTime.Hour == 0 ? HTM_OPTION_SELECTED :
										  HTM_OPTION_UNSELECTED,
				 HTM_ENABLED,
				 "-");
		     for (Hour  = 7;
			  Hour <= 22;
			  Hour++)
			HTM_OPTION (HTM_Type_UNSIGNED,&Hour,
				    Hour == CallsForExams->CallForExam.StartTime.Hour ? HTM_OPTION_SELECTED :
											HTM_OPTION_UNSELECTED,
				    HTM_ENABLED,
				    "%02u %s",Hour,Txt_hours_ABBREVIATION);
		  HTM_SELECT_End ();

		  HTM_SELECT_Begin (HTM_ENABLED,HTM_DONT_SUBMIT_ON_CHANGE,NULL,
				    "name=\"ExamMinute\" class=\"INPUT_%s\"",
				    The_GetSuffix ());
		     for (Minute  = 0;
			  Minute <= 59;
			  Minute++)
			HTM_OPTION (HTM_Type_UNSIGNED,&Minute,
				    Minute == CallsForExams->CallForExam.StartTime.Minute ? HTM_OPTION_SELECTED :
											    HTM_OPTION_UNSELECTED,
				    HTM_ENABLED,
				    "%02u &prime;",Minute);
		  HTM_SELECT_End ();
		 }
	       else if (CallsForExams->CallForExam.StartTime.Hour)
		  HTM_TxtF ("%2u:%02u",CallsForExams->CallForExam.StartTime.Hour,
				       CallsForExams->CallForExam.StartTime.Minute);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Approximate duration of the exam *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT",
			     ViewType == Vie_EDIT ? "DurationHour" :
						    NULL,
			     Txt_CALL_FOR_EXAM_Approximate_duration);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
	       if (ViewType == Vie_EDIT)
		 {
		  HTM_SELECT_Begin (HTM_ENABLED,HTM_DONT_SUBMIT_ON_CHANGE,NULL,
				    "id=\"DurationHour\" name=\"DurationHour\""
				    " class=\"INPUT_%s\"",
				    The_GetSuffix ());
		     for (Hour  = 0;
			  Hour <= 8;
			  Hour++)
			HTM_OPTION (HTM_Type_UNSIGNED,&Hour,
				    Hour == CallsForExams->CallForExam.Duration.Hour ? HTM_OPTION_SELECTED :
										       HTM_OPTION_UNSELECTED,
				    HTM_ENABLED,
				    "%02u %s",Hour,Txt_hours_ABBREVIATION);
		  HTM_SELECT_End ();

		  HTM_SELECT_Begin (HTM_ENABLED,HTM_DONT_SUBMIT_ON_CHANGE,NULL,
				    "name=\"DurationMinute\""
				    " class=\"INPUT_%s\"",
				    The_GetSuffix ());
		     for (Minute  = 0;
			  Minute <= 59;
			  Minute++)
			HTM_OPTION (HTM_Type_UNSIGNED,&Minute,
				    Minute == CallsForExams->CallForExam.Duration.Minute ? HTM_OPTION_SELECTED :
											   HTM_OPTION_UNSELECTED,
				    HTM_ENABLED,
				    "%02u &prime;",Minute);
		  HTM_SELECT_End ();
		 }
	       else if (CallsForExams->CallForExam.Duration.Hour ||
			CallsForExams->CallForExam.Duration.Minute)
		 {
		  if (CallsForExams->CallForExam.Duration.Hour)
		    {
		     if (CallsForExams->CallForExam.Duration.Minute)
			HTM_TxtF ("%u%s %u&prime;",CallsForExams->CallForExam.Duration.Hour,
						   Txt_hours_ABBREVIATION,
						   CallsForExams->CallForExam.Duration.Minute);
		     else
			HTM_TxtF ("%u&nbsp;%s",CallsForExams->CallForExam.Duration.Hour,
					       CallsForExams->CallForExam.Duration.Hour == 1 ? Txt_hour :
											       Txt_hours);
		    }
		  else if (CallsForExams->CallForExam.Duration.Minute)
		    {
		     HTM_TxtF ("%u&nbsp;%s",CallsForExams->CallForExam.Duration.Minute,
					    CallsForExams->CallForExam.Duration.Minute == 1 ? Txt_minute :
											      Txt_minutes);
		    }
		 }
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Place where the exam will be made *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT",
			     ViewType == Vie_EDIT ? "Place" :
						    NULL,
			     Txt_CALL_FOR_EXAM_Place_of_exam);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
	       if (ViewType == Vie_EDIT)
		 {
		  HTM_TEXTAREA_Begin (HTM_ENABLED,
				      "id=\"Place\" name=\"Place\" rows=\"4\""
			              " class=\"Frm_C2_INPUT INPUT_%s\"",
			              The_GetSuffix ());
		     HTM_Txt (CallsForExams->CallForExam.Place);
		  HTM_TEXTAREA_End ();
		 }
	       else
		 {
		  Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				    CallsForExams->CallForExam.Place,
				    Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
		  HTM_Txt (CallsForExams->CallForExam.Place);
		 }
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Exam mode *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT",
			     ViewType == Vie_EDIT ? "ExamMode" :
						    NULL,
			     Txt_CALL_FOR_EXAM_Mode);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
	    if (ViewType == Vie_EDIT)
	      {
	       HTM_TEXTAREA_Begin (HTM_ENABLED,
				   "id=\"ExamMode\" name=\"ExamMode\" rows=\"2\""
			           " class=\"Frm_C2_INPUT INPUT_%s\"",
			           The_GetSuffix ());
		  HTM_Txt (CallsForExams->CallForExam.Mode);
	       HTM_TEXTAREA_End ();
	      }
	    else
	      {
	       Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				 CallsForExams->CallForExam.Mode,
				 Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
	       HTM_Txt (CallsForExams->CallForExam.Mode);
	      }
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Structure of the exam *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT",
			     ViewType == Vie_EDIT ? "Structure" :
						    NULL,
			     Txt_CALL_FOR_EXAM_Structure_of_the_exam);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
	       if (ViewType == Vie_EDIT)
		 {
		  HTM_TEXTAREA_Begin (HTM_ENABLED,
				      "id=\"Structure\" name=\"Structure\""
				      " rows=\"8\""
				      " class=\"Frm_C2_INPUT INPUT_%s\"",
				      The_GetSuffix ());
		     HTM_Txt (CallsForExams->CallForExam.Structure);
		  HTM_TEXTAREA_End ();
		 }
	       else
		 {
		  Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				    CallsForExams->CallForExam.Structure,
				    Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
		  HTM_Txt (CallsForExams->CallForExam.Structure);
		 }
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Documentation required *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT",
			     ViewType == Vie_EDIT ? "DocRequired" :
						    NULL,
			     Txt_CALL_FOR_EXAM_Documentation_required);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
	       if (ViewType == Vie_EDIT)
		 {
		  HTM_TEXTAREA_Begin (HTM_ENABLED,
				      "id=\"DocRequired\" name=\"DocRequired\""
			              " rows=\"2\""
			              " class=\"Frm_C2_INPUT INPUT_%s\"",
			              The_GetSuffix ());
		     HTM_Txt (CallsForExams->CallForExam.DocRequired);
		  HTM_TEXTAREA_End ();
		 }
	       else
		 {
		  Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				    CallsForExams->CallForExam.DocRequired,
				    Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
		  HTM_Txt (CallsForExams->CallForExam.DocRequired);
		 }
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Material required *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT",
			     ViewType == Vie_EDIT ? "MatRequired" :
						    NULL,
			     Txt_CALL_FOR_EXAM_Material_required);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
	       if (ViewType == Vie_EDIT)
		 {
		  HTM_TEXTAREA_Begin (HTM_ENABLED,
				      "id=\"MatRequired\" name=\"MatRequired\""
				      " rows=\"4\""
				      " class=\"Frm_C2_INPUT INPUT_%s\"",
				      The_GetSuffix ());
		     HTM_Txt (CallsForExams->CallForExam.MatRequired);
		  HTM_TEXTAREA_End ();
		 }
	       else
		 {
		  Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				    CallsForExams->CallForExam.MatRequired,
				    Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
		  HTM_Txt (CallsForExams->CallForExam.MatRequired);
		 }
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Material allowed *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT",
			     ViewType == Vie_EDIT ? "MatAllowed" :
						    NULL,
			     Txt_CALL_FOR_EXAM_Material_allowed);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
	       if (ViewType == Vie_EDIT)
		 {
		  HTM_TEXTAREA_Begin (HTM_ENABLED,
				      "id=\"MatAllowed\" name=\"MatAllowed\""
			              " rows=\"4\""
			              " class=\"Frm_C2_INPUT INPUT_%s\"",
			              The_GetSuffix ());
		     HTM_Txt (CallsForExams->CallForExam.MatAllowed);
		  HTM_TEXTAREA_End ();
		 }
	       else
		 {
		  Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				    CallsForExams->CallForExam.MatAllowed,
				    Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
		  HTM_Txt (CallsForExams->CallForExam.MatAllowed);
		 }
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Other information to students *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT",
			     ViewType == Vie_EDIT ? "OtherInfo" :
						    NULL,
			     Txt_CALL_FOR_EXAM_Other_information);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
	       if (ViewType == Vie_EDIT)
		 {
		  HTM_TEXTAREA_Begin (HTM_ENABLED,
				      "id=\"OtherInfo\" name=\"OtherInfo\""
			              " rows=\"5\""
			              " class=\"Frm_C2_INPUT INPUT_%s\"",
			              The_GetSuffix ());
		     HTM_Txt (CallsForExams->CallForExam.OtherInfo);
		  HTM_TEXTAREA_End ();
		 }
	       else
		 {
		  Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				    CallsForExams->CallForExam.OtherInfo,
				    Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
		  HTM_Txt (CallsForExams->CallForExam.OtherInfo);
		 }
	    HTM_TD_End ();

	 HTM_TR_End ();

      /***** End table, send button and end box *****/
      if (ViewType == Vie_EDIT)
	 Box_BoxTableWithButtonEnd (ExaCod > 0 ? Btn_CONFIRM_BUTTON :
						 Btn_CREATE_BUTTON,
				    Txt_Publish_call_FOR_EXAM);
      else
	 Box_BoxTableEnd ();

      /***** Show QR code *****/
      if (ViewType == Vie_PRINT)
	 QR_ExamAnnnouncement ();

   /***** End article *****/
   if (ViewType == Vie_VIEW)
      HTM_ARTICLE_End ();

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (&Anchor);
  }

/*****************************************************************************/
/*********** Put icons to remove / edit / print a call for exam **************/
/*****************************************************************************/

static void Cfe_PutIconsCallForExam (void *CallsForExams)
  {
   static Act_Action_t ActionHideUnhide[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] = ActUnhCfe,	// Hidden ==> action to unhide
      [HidVis_VISIBLE] = ActHidCfe,	// Visible ==> action to hide
     };

   if (CallsForExams)
     {
      if (Cfe_CheckIfICanEditCallsForExams () == Usr_CAN)
	{
	 /***** Icon to remove call for exam *****/
	 Ico_PutContextualIconToRemove (ActReqRemCfe,NULL,
					Cfe_PutParExaCod,
					&((struct Cfe_CallsForExams *) CallsForExams)->ExaCod);

	 /***** Icon to hide/unhide call for exam *****/
	 switch (((struct Cfe_CallsForExams *) CallsForExams)->CallForExam.Status)
	   {
	    case Cfe_VISIBLE_CALL_FOR_EXAM:
	    case Cfe_HIDDEN_CALL_FOR_EXAM:
	       Ico_PutContextualIconToHideUnhide (ActionHideUnhide,((struct Cfe_CallsForExams *) CallsForExams)->Anchor,
					          Cfe_PutParExaCod,
					          &((struct Cfe_CallsForExams *) CallsForExams)->ExaCod,
					          ((struct Cfe_CallsForExams *) CallsForExams)->CallForExam.Status == Cfe_HIDDEN_CALL_FOR_EXAM ? HidVis_HIDDEN :
					        												 HidVis_VISIBLE);
	       break;
	    case Cfe_DELETED_CALL_FOR_EXAM:	// Not applicable here
	       break;
	   }

	 /***** Icon to edit call for exam *****/
	 Ico_PutContextualIconToEdit (ActEdiCfe,NULL,
				      Cfe_PutParExaCod,
				      &((struct Cfe_CallsForExams *) CallsForExams)->ExaCod);
	}

      /***** Link to print view *****/
      Ico_PutContextualIconToPrint (ActPrnCfe,
				    Cfe_PutParExaCod,
				    &((struct Cfe_CallsForExams *) CallsForExams)->ExaCod);

      /***** Link to get resource link *****/
      if (Rsc_CheckIfICanGetLink () == Usr_CAN)
	 Ico_PutContextualIconToGetLink (ActReqLnkCfe,NULL,
					 Cfe_PutParExaCod,
					 &((struct Cfe_CallsForExams *) CallsForExams)->ExaCod);
     }
  }

/*****************************************************************************/
/******************* Check if I can edit calls for exams *********************/
/*****************************************************************************/

Usr_Can_t Cfe_CheckIfICanEditCallsForExams (void)
  {
   static Usr_Can_t ICanEditCallsForExams[Rol_NUM_ROLES] =
     {
      [Rol_TCH    ] = Usr_CAN,
      [Rol_SYS_ADM] = Usr_CAN,
     };

   return ICanEditCallsForExams[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/***************** Param with the code of a call for exam ********************/
/*****************************************************************************/

static void Cfe_PutParExaCod (void *ExaCod)
  {
   if (ExaCod)
      ParCod_PutPar (ParCod_Exa,*((long *) ExaCod));
  }

/*****************************************************************************/
/************** Get summary and content about a call for exam ****************/
/*****************************************************************************/

void Cfe_GetSummaryAndContentCallForExam (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                          char **ContentStr,
                                          long ExaCod,Ntf_GetContent_t GetContent)
  {
   extern const char *Txt_hours_ABBREVIATION;
   struct Cfe_CallsForExams CallsForExams;
   char SessionAndDate[Cfe_MAX_BYTES_SESSION_AND_DATE];

   /***** Reset calls for exams context *****/
   Cfe_ResetCallsForExams (&CallsForExams);

   /***** Initializations *****/
   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Allocate memory for the call for exam *****/
   Cfe_AllocMemCallForExam (&CallsForExams);

   /***** Get data of a call for exam from database *****/
   Cfe_GetCallForExamDataByCod (&CallsForExams,ExaCod);

   /***** Content *****/
   if (GetContent == Ntf_GET_CONTENT)
      Cfe_GetNotifContentCallForExam (&CallsForExams,ContentStr);

   /***** Summary *****/
   /* Session and date of the exam */
   Cfe_BuildSessionAndDate (&CallsForExams,SessionAndDate);
   Str_Copy (SummaryStr,SessionAndDate,Ntf_MAX_BYTES_SUMMARY);

   /***** Free memory of the call for exam *****/
   Cfe_FreeMemCallForExam (&CallsForExams);
  }

/*****************************************************************************/
/************** Show a form with the data of a call for exam *****************/
/*****************************************************************************/

static void Cfe_GetNotifContentCallForExam (const struct Cfe_CallsForExams *CallsForExams,
                                            char **ContentStr)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_CALL_FOR_EXAM_Course;
   extern const char *Txt_CALL_FOR_EXAM_Year_or_semester;
   extern const char *Txt_CALL_FOR_EXAM_Session;
   extern const char *Txt_CALL_FOR_EXAM_Exam_date;
   extern const char *Txt_CALL_FOR_EXAM_Start_time;
   extern const char *Txt_CALL_FOR_EXAM_Approximate_duration;
   extern const char *Txt_CALL_FOR_EXAM_Place_of_exam;
   extern const char *Txt_CALL_FOR_EXAM_Mode;
   extern const char *Txt_CALL_FOR_EXAM_Structure_of_the_exam;
   extern const char *Txt_CALL_FOR_EXAM_Documentation_required;
   extern const char *Txt_CALL_FOR_EXAM_Material_required;
   extern const char *Txt_CALL_FOR_EXAM_Material_allowed;
   extern const char *Txt_CALL_FOR_EXAM_Other_information;
   extern const char *Txt_hours_ABBREVIATION;
   struct Hie_Node Hie[Hie_NUM_LEVELS];
   char StrExamDate[Cns_MAX_BYTES_DATE + 1];

   /***** Get data of course *****/
   Hie[Hie_CRS].HieCod = CallsForExams->CallForExam.CrsCod;
   Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]);

   /***** Get data of degree *****/
   Hie[Hie_DEG].HieCod = Hie[Hie_CRS].PrtCod;
   Hie_GetDataByCod[Hie_DEG] (&Hie[Hie_DEG]);

   /***** Get data of institution *****/
   Hie[Hie_INS].HieCod = Deg_DB_GetInsCodOfDegreeByCod (Hie[Hie_DEG].HieCod);
   Hie_GetDataByCod[Hie_INS] (&Hie[Hie_INS]);

   /***** Convert struct date to a date string *****/
   Dat_ConvDateToDateStr (&CallsForExams->CallForExam.ExamDate,StrExamDate);

   /***** Fill content string *****/
   if (asprintf (ContentStr,"%s: %s<br />"
                            "%s: %s<br />"
                            "%s: %s<br />"
                            "%s: %s<br />"
                            "%s: %s<br />"
                            "%s: %s<br />"
                            "%s: %2u:%02u %s<br />"
                            "%s: %2u:%02u %s<br />"
                            "%s: %s<br />"
                            "%s: %s<br />"
                            "%s: %s<br />"
                            "%s: %s<br />"
                            "%s: %s<br />"
                            "%s: %s<br />"
                            "%s: %s",
                 Txt_HIERARCHY_SINGUL_Abc[Hie_INS],Hie[Hie_INS].FullName,
                 Txt_HIERARCHY_SINGUL_Abc[Hie_DEG],Hie[Hie_DEG].FullName,
                 Txt_CALL_FOR_EXAM_Course,CallsForExams->CallForExam.CrsFullName,
                 Txt_CALL_FOR_EXAM_Year_or_semester,Txt_YEAR_OF_DEGREE[CallsForExams->CallForExam.Year],
                 Txt_CALL_FOR_EXAM_Session,CallsForExams->CallForExam.Session,
                 Txt_CALL_FOR_EXAM_Exam_date,StrExamDate,
                 Txt_CALL_FOR_EXAM_Start_time,CallsForExams->CallForExam.StartTime.Hour,
                                              CallsForExams->CallForExam.StartTime.Minute,
                 Txt_hours_ABBREVIATION,
                 Txt_CALL_FOR_EXAM_Approximate_duration,CallsForExams->CallForExam.Duration.Hour,
                                                        CallsForExams->CallForExam.Duration.Minute,
                 Txt_hours_ABBREVIATION,
                 Txt_CALL_FOR_EXAM_Place_of_exam,CallsForExams->CallForExam.Place,
                 Txt_CALL_FOR_EXAM_Mode,CallsForExams->CallForExam.Mode,
                 Txt_CALL_FOR_EXAM_Structure_of_the_exam,CallsForExams->CallForExam.Structure,
                 Txt_CALL_FOR_EXAM_Documentation_required,CallsForExams->CallForExam.DocRequired,
                 Txt_CALL_FOR_EXAM_Material_required,CallsForExams->CallForExam.MatRequired,
                 Txt_CALL_FOR_EXAM_Material_allowed,CallsForExams->CallForExam.MatAllowed,
                 Txt_CALL_FOR_EXAM_Other_information,CallsForExams->CallForExam.OtherInfo) < 0)
      Err_NotEnoughMemoryExit ();
  }

/*****************************************************************************/
/*********** Build string with session and date of a call for exam ***********/
/*****************************************************************************/

void Cfe_BuildSessionAndDate (const struct Cfe_CallsForExams *CallsForExams,
                              char SessionAndDate[Cfe_MAX_BYTES_SESSION_AND_DATE])
  {
   char StrExamDate[Cns_MAX_BYTES_DATE + 1];

   Dat_ConvDateToDateStr (&CallsForExams->CallForExam.ExamDate,StrExamDate);
   snprintf (SessionAndDate,Cfe_MAX_BYTES_SESSION_AND_DATE,"%s, %s, %2u:%02u",
             CallsForExams->CallForExam.Session,
             StrExamDate,
             CallsForExams->CallForExam.StartTime.Hour,
             CallsForExams->CallForExam.StartTime.Minute);
  }
