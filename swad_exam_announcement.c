// swad_exam_announcement.c: exam announcements

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_exam_announcement.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_logo.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_RSS.h"
#include "swad_string.h"
#include "swad_timeline.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static struct ExaAnn_ExamAnnouncements *ExaAnn_GetGlobalExamAnns (void);

static long ExaAnn_GetParamsExamAnn (struct ExaAnn_ExamAnnouncements *ExamAnns);

static void ExaAnn_AllocMemExamAnn (struct ExaAnn_ExamAnnouncements *ExamAnns);
static void ExaAnn_FreeMemExamAnn (struct ExaAnn_ExamAnnouncements *ExamAnns);

static void ExaAnn_UpdateNumUsrsNotifiedByEMailAboutExamAnn (long ExaCod,unsigned NumUsrsToBeNotifiedByEMail);

static void ExaAnn_GetExaCodToHighlight (struct ExaAnn_ExamAnnouncements *ExamAnns);
static void ExaAnn_GetDateToHighlight (struct ExaAnn_ExamAnnouncements *ExamAnns);

static void ExaAnn_ListExamAnns (struct ExaAnn_ExamAnnouncements *ExamAnns,
                                 ExaAnn_TypeViewExamAnnouncement_t TypeViewExamAnnouncement);
static void ExaAnn_PutIconToCreateNewExamAnn (__attribute__((unused)) void *Args);
static void ExaAnn_PutButtonToCreateNewExamAnn (void);

static long ExaAnn_AddExamAnnToDB (const struct ExaAnn_ExamAnnouncements *ExamAnns);
static void ExaAnn_ModifyExamAnnInDB (const struct ExaAnn_ExamAnnouncements *ExamAnns,
                                      long ExaCod);
static void ExaAnn_GetDataExamAnnFromDB (struct ExaAnn_ExamAnnouncements *ExamAnns,
                                         long ExaCod);
static void ExaAnn_ShowExamAnn (struct ExaAnn_ExamAnnouncements *ExamAnns,
                                long ExaCod,
				ExaAnn_TypeViewExamAnnouncement_t TypeViewExamAnnouncement,
				bool HighLight);
static void ExaAnn_PutIconsExamAnn (void *ExamAnns);
static void ExaAnn_PutParamExaCodToEdit (void *ExaCod);
static long ExaAnn_GetParamExaCod (void);

static void ExaAnn_GetNotifContentExamAnn (const struct ExaAnn_ExamAnnouncements *ExamAnns,
                                           char **ContentStr);

/*****************************************************************************/
/******************* Get global exam announcements context *******************/
/*****************************************************************************/

static struct ExaAnn_ExamAnnouncements *ExaAnn_GetGlobalExamAnns (void)
  {
   static struct ExaAnn_ExamAnnouncements ExaAnn_GlobalExamAnns;	// Used to preserve information between priori and posteriori functions

   return &ExaAnn_GlobalExamAnns;
  }

/*****************************************************************************/
/********************** Reset exam announcements context *********************/
/*****************************************************************************/

void ExaAnn_ResetExamAnns (struct ExaAnn_ExamAnnouncements *ExamAnns)
  {
   ExamAnns->NumExaAnns       = 0;
   ExamAnns->Lst              = NULL;
   ExamAnns->NewExaCod        = -1L;
   ExamAnns->HighlightExaCod  = -1L;
   ExamAnns->HighlightDate[0] = '\0';	// No exam announcements highlighted
   ExamAnns->ExaCod           = -1L;
   ExamAnns->Anchor           = NULL;

   ExamAnns->ExamAnn.CrsCod = -1L;
   ExamAnns->ExamAnn.Status = ExaAnn_STATUS_DEFAULT;
   ExamAnns->ExamAnn.CrsFullName[0] = '\0';
   ExamAnns->ExamAnn.Year = 0;
   ExamAnns->ExamAnn.Session[0] = '\0';
   Dat_ResetDate (&ExamAnns->ExamAnn.CallDate);
   Dat_ResetDate (&ExamAnns->ExamAnn.ExamDate);
   Dat_ResetHour (&ExamAnns->ExamAnn.StartTime);
   Dat_ResetHour (&ExamAnns->ExamAnn.Duration);
   ExamAnns->ExamAnn.Place            = NULL;
   ExamAnns->ExamAnn.Mode             = NULL;
   ExamAnns->ExamAnn.Structure        = NULL;
   ExamAnns->ExamAnn.DocRequired      = NULL;
   ExamAnns->ExamAnn.MatRequired      = NULL;
   ExamAnns->ExamAnn.MatAllowed       = NULL;
   ExamAnns->ExamAnn.OtherInfo        = NULL;
  }

/*****************************************************************************/
/********************** Form to edit an exam announcement ********************/
/*****************************************************************************/

void ExaAnn_PutFrmEditAExamAnn (void)
  {
   struct ExaAnn_ExamAnnouncements ExamAnns;
   long ExaCod;

   /***** Reset exam announcements context *****/
   ExaAnn_ResetExamAnns (&ExamAnns);

   /***** Allocate memory for the exam announcement *****/
   ExaAnn_AllocMemExamAnn (&ExamAnns);

   /***** Get the code of the exam announcement *****/
   ExaCod = ExaAnn_GetParamsExamAnn (&ExamAnns);

   if (ExaCod > 0)	// -1 indicates that this is a new exam announcement
      /***** Read exam announcement from the database *****/
      ExaAnn_GetDataExamAnnFromDB (&ExamAnns,ExaCod);

   /***** Show exam announcement *****/
   ExaAnn_ShowExamAnn (&ExamAnns,ExaCod,ExaAnn_FORM_VIEW,
		       false);	// Don't highlight

   /***** Free memory of the exam announcement *****/
   ExaAnn_FreeMemExamAnn (&ExamAnns);
  }

/*****************************************************************************/
/**************** Get parameters of an exam announcement *********************/
/*****************************************************************************/

static long ExaAnn_GetParamsExamAnn (struct ExaAnn_ExamAnnouncements *ExamAnns)
  {
   long ExaCod;

   /***** Get the code of the exam announcement *****/
   ExaCod = ExaAnn_GetParamExaCod ();

   /***** Get the name of the course (it is allowed to be different from the official name of the course) *****/
   Par_GetParToText ("CrsName",ExamAnns->ExamAnn.CrsFullName,Hie_MAX_BYTES_FULL_NAME);
   // If the parameter is not present or is empty, initialize the string to the full name of the current course
   if (!ExamAnns->ExamAnn.CrsFullName[0])
      Str_Copy (ExamAnns->ExamAnn.CrsFullName,Gbl.Hierarchy.Crs.FullName,
                Hie_MAX_BYTES_FULL_NAME);

   /***** Get the year *****/
   ExamAnns->ExamAnn.Year = (unsigned)
			    Par_GetParToUnsignedLong ("Year",
						      0,	// N.A.
						      Deg_MAX_YEARS_PER_DEGREE,
						      (unsigned long) Gbl.Hierarchy.Crs.Year);

   /***** Get the type of exam announcement *****/
   Par_GetParToText ("ExamSession",ExamAnns->ExamAnn.Session,ExaAnn_MAX_BYTES_SESSION);

   /***** Get the date of the exam *****/
   Dat_GetDateFromForm ("ExamDay","ExamMonth","ExamYear",
                        &ExamAnns->ExamAnn.ExamDate.Day,
                        &ExamAnns->ExamAnn.ExamDate.Month,
                        &ExamAnns->ExamAnn.ExamDate.Year);
   if (ExamAnns->ExamAnn.ExamDate.Day   == 0 ||
       ExamAnns->ExamAnn.ExamDate.Month == 0 ||
       ExamAnns->ExamAnn.ExamDate.Year  == 0)
     {
      ExamAnns->ExamAnn.ExamDate.Day   = Gbl.Now.Date.Day;
      ExamAnns->ExamAnn.ExamDate.Month = Gbl.Now.Date.Month;
      ExamAnns->ExamAnn.ExamDate.Year  = Gbl.Now.Date.Year;
     }

   /***** Get the hour of the exam *****/
   ExamAnns->ExamAnn.StartTime.Hour   = (unsigned) Par_GetParToUnsignedLong ("ExamHour",
                                                                             0,23,0);
   ExamAnns->ExamAnn.StartTime.Minute = (unsigned) Par_GetParToUnsignedLong ("ExamMinute",
                                                                             0,59,0);

   /***** Get the duration of the exam *****/
   ExamAnns->ExamAnn.Duration.Hour    = (unsigned) Par_GetParToUnsignedLong ("DurationHour",
                                                                             0,23,0);
   ExamAnns->ExamAnn.Duration.Minute  = (unsigned) Par_GetParToUnsignedLong ("DurationMinute",
                                                                             0,59,0);

   /***** Get the place where the exam will happen *****/
   Par_GetParToHTML ("Place",ExamAnns->ExamAnn.Place,Cns_MAX_BYTES_TEXT);

   /***** Get the modality of exam *****/
   Par_GetParToHTML ("ExamMode",ExamAnns->ExamAnn.Mode,Cns_MAX_BYTES_TEXT);

   /***** Get the structure of exam *****/
   Par_GetParToHTML ("Structure",ExamAnns->ExamAnn.Structure,Cns_MAX_BYTES_TEXT);

   /***** Get the mandatory documentation *****/
   Par_GetParToHTML ("DocRequired",ExamAnns->ExamAnn.DocRequired,Cns_MAX_BYTES_TEXT);

   /***** Get the mandatory material *****/
   Par_GetParToHTML ("MatRequired",ExamAnns->ExamAnn.MatRequired,Cns_MAX_BYTES_TEXT);

   /***** Get the allowed material *****/
   Par_GetParToHTML ("MatAllowed",ExamAnns->ExamAnn.MatAllowed,Cns_MAX_BYTES_TEXT);

   /***** Get other information *****/
   Par_GetParToHTML ("OtherInfo",ExamAnns->ExamAnn.OtherInfo,Cns_MAX_BYTES_TEXT);

   return ExaCod;
  }

/*****************************************************************************/
/* Allocate memory for those parameters of an exam anno. with a lot of text **/
/*****************************************************************************/

static void ExaAnn_AllocMemExamAnn (struct ExaAnn_ExamAnnouncements *ExamAnns)
  {
   if ((ExamAnns->ExamAnn.Place       = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   if ((ExamAnns->ExamAnn.Mode        = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   if ((ExamAnns->ExamAnn.Structure   = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   if ((ExamAnns->ExamAnn.DocRequired = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   if ((ExamAnns->ExamAnn.MatRequired = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   if ((ExamAnns->ExamAnn.MatAllowed  = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   if ((ExamAnns->ExamAnn.OtherInfo   = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();
  }

/*****************************************************************************/
/* Free memory of those parameters of an exam announcem. with a lot of text **/
/*****************************************************************************/

static void ExaAnn_FreeMemExamAnn (struct ExaAnn_ExamAnnouncements *ExamAnns)
  {
   if (ExamAnns->ExamAnn.Place)
     {
      free (ExamAnns->ExamAnn.Place);
      ExamAnns->ExamAnn.Place = NULL;
     }
   if (ExamAnns->ExamAnn.Mode)
     {
      free (ExamAnns->ExamAnn.Mode);
      ExamAnns->ExamAnn.Mode = NULL;
     }
   if (ExamAnns->ExamAnn.Structure)
     {
      free (ExamAnns->ExamAnn.Structure);
      ExamAnns->ExamAnn.Structure = NULL;
     }
   if (ExamAnns->ExamAnn.DocRequired)
     {
      free (ExamAnns->ExamAnn.DocRequired);
      ExamAnns->ExamAnn.DocRequired = NULL;
     }
   if (ExamAnns->ExamAnn.MatRequired)
     {
      free (ExamAnns->ExamAnn.MatRequired);
      ExamAnns->ExamAnn.MatRequired = NULL;
     }
   if (ExamAnns->ExamAnn.MatAllowed)
     {
      free (ExamAnns->ExamAnn.MatAllowed);
      ExamAnns->ExamAnn.MatAllowed = NULL;
     }
   if (ExamAnns->ExamAnn.OtherInfo)
     {
      free (ExamAnns->ExamAnn.OtherInfo);
      ExamAnns->ExamAnn.OtherInfo = NULL;
     }
  }

/*****************************************************************************/
/************************ Receive an exam announcement ***********************/
/*****************************************************************************/
// This function is splitted into a-priori and a-posteriori functions
// in order to view updated links in month of left column

void ExaAnn_ReceiveExamAnn1 (void)
  {
   extern const char *Txt_Created_new_announcement_of_exam;
   extern const char *Txt_The_announcement_of_exam_has_been_successfully_updated;
   struct ExaAnn_ExamAnnouncements *ExamAnns = ExaAnn_GetGlobalExamAnns ();
   long ExaCod;
   bool NewExamAnnouncement;
   char *Anchor = NULL;

   /***** Reset exam announcements context *****/
   ExaAnn_ResetExamAnns (ExamAnns);

   /***** Allocate memory for the exam announcement *****/
   ExaAnn_AllocMemExamAnn (ExamAnns);

   /***** Get parameters of the exam announcement *****/
   ExaCod = ExaAnn_GetParamsExamAnn (ExamAnns);
   NewExamAnnouncement = (ExaCod < 0);

   /***** Add the exam announcement to the database and read it again from the database *****/
   if (NewExamAnnouncement)
      ExamAnns->NewExaCod = ExaCod = ExaAnn_AddExamAnnToDB (ExamAnns);
   else
      ExaAnn_ModifyExamAnnInDB (ExamAnns,ExaCod);

   /***** Free memory of the exam announcement *****/
   ExaAnn_FreeMemExamAnn (ExamAnns);

   /***** Create alert to show the change made *****/
   Frm_SetAnchorStr (ExaCod,&Anchor);
   Ale_CreateAlert (Ale_SUCCESS,Anchor,
                    NewExamAnnouncement ? Txt_Created_new_announcement_of_exam :
                                          Txt_The_announcement_of_exam_has_been_successfully_updated);
   Frm_FreeAnchorStr (Anchor);

   /***** Set exam to be highlighted *****/
   ExamAnns->HighlightExaCod = ExaCod;
  }

void ExaAnn_ReceiveExamAnn2 (void)
  {
   struct ExaAnn_ExamAnnouncements *ExamAnns = ExaAnn_GetGlobalExamAnns ();
   unsigned NumUsrsToBeNotifiedByEMail;

   /***** Notify by email about the new exam announcement *****/
   if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_EXAM_ANNOUNCEMENT,ExamAnns->HighlightExaCod)))
      ExaAnn_UpdateNumUsrsNotifiedByEMailAboutExamAnn (ExamAnns->HighlightExaCod,NumUsrsToBeNotifiedByEMail);

   /***** Create a new social note about the new exam announcement *****/
   TL_StoreAndPublishNote (TL_NOTE_EXAM_ANNOUNCEMENT,ExamAnns->HighlightExaCod);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.Hierarchy.Crs);

   /***** Show exam announcements *****/
   ExaAnn_ListExamAnnsEdit ();
  }

/*****************************************************************************/
/***** Update number of users notified in table of exam announcements ********/
/*****************************************************************************/

static void ExaAnn_UpdateNumUsrsNotifiedByEMailAboutExamAnn (long ExaCod,unsigned NumUsrsToBeNotifiedByEMail)
  {
   /***** Update number of users notified *****/
   DB_QueryUPDATE ("can not update the number of notifications"
		   " of an exam announcement",
		   "UPDATE exam_announcements SET NumNotif=NumNotif+%u"
		   " WHERE ExaCod=%ld",
                   NumUsrsToBeNotifiedByEMail,ExaCod);
  }

/*****************************************************************************/
/************************* Print an exam announcement ************************/
/*****************************************************************************/

void ExaAnn_PrintExamAnn (void)
  {
   struct ExaAnn_ExamAnnouncements ExamAnns;
   long ExaCod;

   /***** Reset exam announcements context *****/
   ExaAnn_ResetExamAnns (&ExamAnns);

   /***** Allocate memory for the exam announcement *****/
   ExaAnn_AllocMemExamAnn (&ExamAnns);

   /***** Get the code of the exam announcement *****/
   if ((ExaCod = ExaAnn_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Read exam announcement from the database *****/
   ExaAnn_GetDataExamAnnFromDB (&ExamAnns,ExaCod);

   /***** Show exam announcement *****/
   ExaAnn_ShowExamAnn (&ExamAnns,ExaCod,ExaAnn_PRINT_VIEW,
			     false);	// Don't highlight

   /***** Free memory of the exam announcement *****/
   ExaAnn_FreeMemExamAnn (&ExamAnns);
  }

/*****************************************************************************/
/************************ Remove an exam announcement ************************/
/*****************************************************************************/

void ExaAnn_ReqRemoveExamAnn (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_following_announcement_of_exam;
   extern const char *Txt_Remove;
   struct ExaAnn_ExamAnnouncements ExamAnns;
   long ExaCod;

   /***** Reset exam announcements context *****/
   ExaAnn_ResetExamAnns (&ExamAnns);

   /***** Get the code of the exam announcement *****/
   if ((ExaCod = ExaAnn_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Show question and button to remove exam announcement *****/
   /* Start alert */
   Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_following_announcement_of_exam);

   /* Show announcement */
   ExaAnn_AllocMemExamAnn (&ExamAnns);
   ExaAnn_GetDataExamAnnFromDB (&ExamAnns,ExaCod);
   ExaAnn_ShowExamAnn (&ExamAnns,ExaCod,ExaAnn_NORMAL_VIEW,
			     false);	// Don't highlight
   ExaAnn_FreeMemExamAnn (&ExamAnns);

   /* End alert */

   Ale_ShowAlertAndButton2 (ActRemExaAnn,NULL,NULL,
                            ExaAnn_PutParamExaCodToEdit,&ExamAnns.ExaCod,
			    Btn_REMOVE_BUTTON,Txt_Remove);
  }

/*****************************************************************************/
/************************ Remove an exam announcement ************************/
/*****************************************************************************/
// This function is splitted into a-priori and a-posteriori functions
// in order to view updated links in month of left column

void ExaAnn_RemoveExamAnn1 (void)
  {
   struct ExaAnn_ExamAnnouncements *ExamAnns = ExaAnn_GetGlobalExamAnns ();
   long ExaCod;

   /***** Reset exam announcements context *****/
   ExaAnn_ResetExamAnns (ExamAnns);

   /***** Get the code of the exam announcement *****/
   if ((ExaCod = ExaAnn_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Mark the exam announcement as deleted in the database *****/
   DB_QueryUPDATE ("can not remove exam announcement",
		   "UPDATE exam_announcements SET Status=%u"
		   " WHERE ExaCod=%ld AND CrsCod=%ld",
                   (unsigned) ExaAnn_DELETED_EXAM_ANNOUNCEMENT,
                   ExaCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Mark possible notifications as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_EXAM_ANNOUNCEMENT,ExaCod);

   /***** Mark possible social note as unavailable *****/
   TL_MarkNoteAsUnavailableUsingNoteTypeAndCod (TL_NOTE_EXAM_ANNOUNCEMENT,ExaCod);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.Hierarchy.Crs);
  }

void ExaAnn_RemoveExamAnn2 (void)
  {
   extern const char *Txt_Announcement_of_exam_removed;

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Announcement_of_exam_removed);

   /***** List again all the remaining exam announcements *****/
   ExaAnn_ListExamAnnsEdit ();
  }

/*****************************************************************************/
/************************ Hide an exam announcement **************************/
/*****************************************************************************/
// This function is splitted into a-priori and a-posteriori functions
// in order to view updated links in month of left column

void ExaAnn_HideExamAnn (void)
  {
   struct ExaAnn_ExamAnnouncements *ExamAnns = ExaAnn_GetGlobalExamAnns ();
   long ExaCod;

   /***** Reset exam announcements context *****/
   ExaAnn_ResetExamAnns (ExamAnns);

   /***** Get the code of the exam announcement *****/
   if ((ExaCod = ExaAnn_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Mark the exam announcement as hidden in the database *****/
   DB_QueryUPDATE ("can not hide exam announcement",
		   "UPDATE exam_announcements SET Status=%u"
		   " WHERE ExaCod=%ld AND CrsCod=%ld",
                   (unsigned) ExaAnn_HIDDEN_EXAM_ANNOUNCEMENT,
                   ExaCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Set exam to be highlighted *****/
   ExamAnns->HighlightExaCod = ExaCod;
  }

/*****************************************************************************/
/************************ Unhide an exam announcement ************************/
/*****************************************************************************/
// This function is splitted into a-priori and a-posteriori functions
// in order to view updated links in month of left column

void ExaAnn_UnhideExamAnn (void)
  {
   struct ExaAnn_ExamAnnouncements *ExamAnns = ExaAnn_GetGlobalExamAnns ();
   long ExaCod;

   /***** Reset exam announcements context *****/
   ExaAnn_ResetExamAnns (ExamAnns);

   /***** Get the code of the exam announcement *****/
   if ((ExaCod = ExaAnn_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Mark the exam announcement as visible in the database *****/
   DB_QueryUPDATE ("can not unhide exam announcement",
		   "UPDATE exam_announcements SET Status=%u"
		   " WHERE ExaCod=%ld AND CrsCod=%ld",
                   (unsigned) ExaAnn_VISIBLE_EXAM_ANNOUNCEMENT,
                   ExaCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Set exam to be highlighted *****/
   ExamAnns->HighlightExaCod = ExaCod;
  }

/*****************************************************************************/
/*************** List all the exam announcements to see them *****************/
/*****************************************************************************/

void ExaAnn_ListExamAnnsSee (void)
  {
   struct ExaAnn_ExamAnnouncements ExamAnns;

   /***** Reset exam announcements context *****/
   ExaAnn_ResetExamAnns (&ExamAnns);

   /***** List all exam announcements *****/
   ExaAnn_ListExamAnns (&ExamAnns,ExaAnn_NORMAL_VIEW);

   /***** Mark possible notifications as seen *****/
   Ntf_MarkNotifAsSeen (Ntf_EVENT_EXAM_ANNOUNCEMENT,
	                -1L,Gbl.Hierarchy.Crs.CrsCod,
	                Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********** List all the exam announcements to edit or remove them ***********/
/*****************************************************************************/

void ExaAnn_ListExamAnnsEdit (void)
  {
   struct ExaAnn_ExamAnnouncements *ExamAnns = ExaAnn_GetGlobalExamAnns ();

   ExaAnn_ListExamAnns (ExamAnns,ExaAnn_NORMAL_VIEW);
  }

/*****************************************************************************/
/********** List exam announcement given an exam announcement code ***********/
/*****************************************************************************/

void ExaAnn_ListExamAnnsCod (void)
  {
   struct ExaAnn_ExamAnnouncements ExamAnns;

   /***** Reset exam announcements context *****/
   ExaAnn_ResetExamAnns (&ExamAnns);

   /***** Get exam announcement code *****/
   ExaAnn_GetExaCodToHighlight (&ExamAnns);

   /***** List all exam announcements *****/
   ExaAnn_ListExamAnns (&ExamAnns,ExaAnn_NORMAL_VIEW);
  }

/*****************************************************************************/
/***************** List exam announcements on a given date *******************/
/*****************************************************************************/

void ExaAnn_ListExamAnnsDay (void)
  {
   struct ExaAnn_ExamAnnouncements ExamAnns;

   /***** Reset exam announcements context *****/
   ExaAnn_ResetExamAnns (&ExamAnns);

   /***** Get date *****/
   ExaAnn_GetDateToHighlight (&ExamAnns);

   /***** List all exam announcements *****/
   ExaAnn_ListExamAnns (&ExamAnns,ExaAnn_NORMAL_VIEW);
  }

/*****************************************************************************/
/*********** Get date of exam announcements to show highlighted **************/
/*****************************************************************************/

static void ExaAnn_GetExaCodToHighlight (struct ExaAnn_ExamAnnouncements *ExamAnns)
  {
   /***** Get the exam announcement code
          of the exam announcement to highlight *****/
   ExamAnns->HighlightExaCod = ExaAnn_GetParamExaCod ();
  }

/*****************************************************************************/
/*********** Get date of exam announcements to show highlighted **************/
/*****************************************************************************/

static void ExaAnn_GetDateToHighlight (struct ExaAnn_ExamAnnouncements *ExamAnns)
  {
   /***** Get the date (in YYYYMMDD format)
          of the exam announcements to highlight *****/
   Par_GetParToText ("Date",ExamAnns->HighlightDate,4 + 2 + 2);
  }

/*****************************************************************************/
/******************** List all the exam announcements ************************/
/*****************************************************************************/

static void ExaAnn_ListExamAnns (struct ExaAnn_ExamAnnouncements *ExamAnns,
                                 ExaAnn_TypeViewExamAnnouncement_t TypeViewExamAnnouncement)
  {
   extern const char *Hlp_ASSESSMENT_Announcements;
   extern const char *Txt_Announcements_of_exams;
   extern const char *Txt_No_announcements_of_exams_of_X;
   char SubQueryStatus[64];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumExaAnn;
   unsigned long NumExaAnns;
   long ExaCod;
   bool HighLight;
   bool ICanEdit = (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
		    Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);

   /***** Build subquery about status depending on my role *****/
   if (ICanEdit)
      sprintf (SubQueryStatus,"Status<>%u",
	       (unsigned) ExaAnn_DELETED_EXAM_ANNOUNCEMENT);
   else
      sprintf (SubQueryStatus,"Status=%u",
	       (unsigned) ExaAnn_VISIBLE_EXAM_ANNOUNCEMENT);

   /***** Get exam announcements (the most recent first)
          in current course from database *****/
   NumExaAnns = DB_QuerySELECT (&mysql_res,"can not get exam announcements"
	                                   " in this course for listing",
				"SELECT ExaCod"
				" FROM exam_announcements"
				" WHERE CrsCod=%ld AND %s"
				" ORDER BY ExamDate DESC",
				Gbl.Hierarchy.Crs.CrsCod,SubQueryStatus);

   /***** Begin box *****/
   if (ICanEdit)
      Box_BoxBegin (NULL,Txt_Announcements_of_exams,
		    ExaAnn_PutIconToCreateNewExamAnn,NULL,
		    Hlp_ASSESSMENT_Announcements,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,Txt_Announcements_of_exams,
		    NULL,NULL,
		    Hlp_ASSESSMENT_Announcements,Box_NOT_CLOSABLE);

   /***** The result of the query may be empty *****/
   if (!NumExaAnns)
      Ale_ShowAlert (Ale_INFO,Txt_No_announcements_of_exams_of_X,
                     Gbl.Hierarchy.Crs.FullName);

   /***** List the existing exam announcements *****/
   for (NumExaAnn = 0;
	NumExaAnn < NumExaAnns;
	NumExaAnn++)
     {
      /***** Get the code of the exam announcement (row[0]) *****/
      row = mysql_fetch_row (mysql_res);

      if (sscanf (row[0],"%ld",&ExaCod) != 1)
         Lay_ShowErrorAndExit ("Wrong code of exam announcement.");

      /***** Allocate memory for the exam announcement *****/
      ExaAnn_AllocMemExamAnn (ExamAnns);

      /***** Read the data of the exam announcement *****/
      ExaAnn_GetDataExamAnnFromDB (ExamAnns,ExaCod);

      /***** Show exam announcement *****/
      HighLight = false;
      if (ExaCod == ExamAnns->HighlightExaCod)
	 HighLight = true;
      else if (ExamAnns->HighlightDate[0])
        {
	 if (!strcmp (ExamAnns->ExamAnn.ExamDate.YYYYMMDD,
	              ExamAnns->HighlightDate))
	    HighLight = true;
        }
      ExaAnn_ShowExamAnn (ExamAnns,ExaCod,TypeViewExamAnnouncement,
	                        HighLight);

      /***** Free memory of the exam announcement *****/
      ExaAnn_FreeMemExamAnn (ExamAnns);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Button to create a new exam announcement *****/
   if (ICanEdit)
      ExaAnn_PutButtonToCreateNewExamAnn ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/***************** Put icon to create a new exam announcement ****************/
/*****************************************************************************/

static void ExaAnn_PutIconToCreateNewExamAnn (__attribute__((unused)) void *Args)
  {
   extern const char *Txt_New_announcement_OF_EXAM;

   Ico_PutContextualIconToAdd (ActEdiExaAnn,NULL,
			       NULL,NULL,
			       Txt_New_announcement_OF_EXAM);
  }

/*****************************************************************************/
/**************** Put button to create a new exam announcement ***************/
/*****************************************************************************/

static void ExaAnn_PutButtonToCreateNewExamAnn (void)
  {
   extern const char *Txt_New_announcement_OF_EXAM;

   Frm_StartForm (ActEdiExaAnn);
   Btn_PutConfirmButton (Txt_New_announcement_OF_EXAM);
   Frm_EndForm ();
  }

/*****************************************************************************/
/****************** Add an exam announcement to the database *****************/
/*****************************************************************************/
// Return the code of the exam announcement just added

static long ExaAnn_AddExamAnnToDB (const struct ExaAnn_ExamAnnouncements *ExamAnns)
  {
   long ExaCod;

   /***** Add exam announcement *****/
   ExaCod =
   DB_QueryINSERTandReturnCode ("can not create a new exam announcement",
				"INSERT INTO exam_announcements "
				"(CrsCod,Status,NumNotif,CrsFullName,Year,ExamSession,"
				"CallDate,ExamDate,Duration,"
				"Place,ExamMode,Structure,DocRequired,MatRequired,MatAllowed,OtherInfo)"
				" VALUES "
				"(%ld,%u,0,'%s',%u,'%s',"
				"NOW(),'%04u-%02u-%02u %02u:%02u:00','%02u:%02u:00','%s',"
				"'%s','%s','%s','%s','%s','%s')",
				Gbl.Hierarchy.Crs.CrsCod,
				(unsigned) ExaAnn_VISIBLE_EXAM_ANNOUNCEMENT,
				ExamAnns->ExamAnn.CrsFullName,
				ExamAnns->ExamAnn.Year,
				ExamAnns->ExamAnn.Session,
				ExamAnns->ExamAnn.ExamDate.Year,
				ExamAnns->ExamAnn.ExamDate.Month,
				ExamAnns->ExamAnn.ExamDate.Day,
				ExamAnns->ExamAnn.StartTime.Hour,
				ExamAnns->ExamAnn.StartTime.Minute,
				ExamAnns->ExamAnn.Duration.Hour,
				ExamAnns->ExamAnn.Duration.Minute,
				ExamAnns->ExamAnn.Place,
				ExamAnns->ExamAnn.Mode,
				ExamAnns->ExamAnn.Structure,
				ExamAnns->ExamAnn.DocRequired,
				ExamAnns->ExamAnn.MatRequired,
				ExamAnns->ExamAnn.MatAllowed,
				ExamAnns->ExamAnn.OtherInfo);

   return ExaCod;
  }

/*****************************************************************************/
/*************** Modify an exam announcement in the database *****************/
/*****************************************************************************/

static void ExaAnn_ModifyExamAnnInDB (const struct ExaAnn_ExamAnnouncements *ExamAnns,
                                      long ExaCod)
  {
   /***** Modify exam announcement *****/
   DB_QueryUPDATE ("can not update an exam announcement",
		   "UPDATE exam_announcements"
		   " SET CrsFullName='%s',Year=%u,ExamSession='%s',"
		   "ExamDate='%04u-%02u-%02u %02u:%02u:00',"
		   "Duration='%02u:%02u:00',"
		   "Place='%s',ExamMode='%s',Structure='%s',"
		   "DocRequired='%s',MatRequired='%s',MatAllowed='%s',OtherInfo='%s'"
		   " WHERE ExaCod=%ld",
	           ExamAnns->ExamAnn.CrsFullName,
	           ExamAnns->ExamAnn.Year,
	           ExamAnns->ExamAnn.Session,
	           ExamAnns->ExamAnn.ExamDate.Year,
	           ExamAnns->ExamAnn.ExamDate.Month,
	           ExamAnns->ExamAnn.ExamDate.Day,
	           ExamAnns->ExamAnn.StartTime.Hour,
	           ExamAnns->ExamAnn.StartTime.Minute,
	           ExamAnns->ExamAnn.Duration.Hour,
	           ExamAnns->ExamAnn.Duration.Minute,
	           ExamAnns->ExamAnn.Place,
	           ExamAnns->ExamAnn.Mode,
	           ExamAnns->ExamAnn.Structure,
	           ExamAnns->ExamAnn.DocRequired,
	           ExamAnns->ExamAnn.MatRequired,
	           ExamAnns->ExamAnn.MatAllowed,
	           ExamAnns->ExamAnn.OtherInfo,
	           ExaCod);
  }

/*****************************************************************************/
/******* Create a list with the dates of all the exam announcements **********/
/*****************************************************************************/

void ExaAnn_CreateListExamAnns (struct ExaAnn_ExamAnnouncements *ExamAnns)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumExaAnn;
   unsigned long NumExaAnns;

   if (Gbl.DB.DatabaseIsOpen)
     {
      /***** Get exam dates (ordered from more recent to older)
             of visible exam announcements
             in current course from database *****/
      NumExaAnns = DB_QuerySELECT (&mysql_res,"can not get exam announcements"
	                                      " in this course",
				   "SELECT ExaCod,DATE(ExamDate)"
				   " FROM exam_announcements"
				   " WHERE CrsCod=%ld AND Status=%u"
				   " ORDER BY ExamDate DESC",
				   Gbl.Hierarchy.Crs.CrsCod,
				   (unsigned) ExaAnn_VISIBLE_EXAM_ANNOUNCEMENT);

      /***** The result of the query may be empty *****/
      ExamAnns->Lst = NULL;
      ExamAnns->NumExaAnns = 0;
      if (NumExaAnns)
	{
	 /***** Allocate memory for the list *****/
	 if ((ExamAnns->Lst = (struct ExaAnn_ExamCodeAndDate *) calloc (NumExaAnns,sizeof (struct ExaAnn_ExamCodeAndDate))) == NULL)
	    Lay_NotEnoughMemoryExit ();

	 /***** Get the dates of the existing exam announcements *****/
	 for (NumExaAnn = 0;
	      NumExaAnn < NumExaAnns;
	      NumExaAnn++)
	   {
	    /***** Get next exam announcement *****/
	    row = mysql_fetch_row (mysql_res);

	    /* Get exam code (row[0]) */
	    ExamAnns->Lst[ExamAnns->NumExaAnns].ExaCod = Str_ConvertStrCodToLongCod (row[0]);

	    /* Read the date of the exam (row[1]) */
	    if (sscanf (row[1],"%04u-%02u-%02u",
			&ExamAnns->Lst[ExamAnns->NumExaAnns].ExamDate.Year,
			&ExamAnns->Lst[ExamAnns->NumExaAnns].ExamDate.Month,
			&ExamAnns->Lst[ExamAnns->NumExaAnns].ExamDate.Day) != 3)
	       Lay_ShowErrorAndExit ("Wrong date of exam.");

	    /***** Increment number of elements in list *****/
	    ExamAnns->NumExaAnns++;
	   }
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/***************** Free list of dates of exam announcements ******************/
/*****************************************************************************/

void ExaAnn_FreeListExamAnns (struct ExaAnn_ExamAnnouncements *ExamAnns)
  {
   if (ExamAnns->Lst)
     {
      free (ExamAnns->Lst);
      ExamAnns->Lst = NULL;
      ExamAnns->NumExaAnns = 0;
     }
  }

/*****************************************************************************/
/******** Read the data of an exam announcement from the database ************/
/*****************************************************************************/

static void ExaAnn_GetDataExamAnnFromDB (struct ExaAnn_ExamAnnouncements *ExamAnns,
                                         long ExaCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumExaAnns;
   unsigned UnsignedNum;
   unsigned Hour;
   unsigned Minute;
   unsigned Second;

   /***** Get data of an exam announcement from database *****/
   NumExaAnns = DB_QuerySELECT (&mysql_res,"can not get data"
					   " of an exam announcement",
	                        "SELECT CrsCod,Status,CrsFullName,Year,ExamSession,"
				"CallDate,ExamDate,Duration,Place,ExamMode,"
				"Structure,DocRequired,MatRequired,MatAllowed,OtherInfo"
				" FROM exam_announcements WHERE ExaCod=%ld",
				ExaCod);

   /***** The result of the query must have one row *****/
   if (NumExaAnns != 1)
      Lay_ShowErrorAndExit ("Error when getting data of an exam announcement.");

   /***** Get the data of the exam announcement *****/
   row = mysql_fetch_row (mysql_res);

   /* Code of the course in which the exam announcement is inserted (row[0]) */
   ExamAnns->ExamAnn.CrsCod = Str_ConvertStrCodToLongCod (row[0]);

   /* Status of the exam announcement (row[1]) */
   if (sscanf (row[1],"%u",&UnsignedNum) != 1)
      Lay_ShowErrorAndExit ("Wrong status.");
   if (UnsignedNum >= ExaAnn_NUM_STATUS)
      Lay_ShowErrorAndExit ("Wrong status.");
   ExamAnns->ExamAnn.Status = (ExaAnn_Status_t) UnsignedNum;

   /* Name of the course (row[2]) */
   Str_Copy (ExamAnns->ExamAnn.CrsFullName,row[2],
             Hie_MAX_BYTES_FULL_NAME);

   /* Year (row[3]) */
   if (sscanf (row[3],"%u",&ExamAnns->ExamAnn.Year) != 1)
      Lay_ShowErrorAndExit ("Wrong year.");

   /* Exam session (row[4]) */
   Str_Copy (ExamAnns->ExamAnn.Session,row[4],
             ExaAnn_MAX_BYTES_SESSION);

   /* Date of exam announcement (row[5]) */
   if (sscanf (row[5],"%04u-%02u-%02u %02u:%02u:%02u",
               &ExamAnns->ExamAnn.CallDate.Year,
               &ExamAnns->ExamAnn.CallDate.Month,
               &ExamAnns->ExamAnn.CallDate.Day,
               &Hour,
	       &Minute,
	       &Second) != 6)
      Lay_ShowErrorAndExit ("Wrong date of exam announcement.");

   /* Date of exam (row[6]) */
   if (sscanf (row[6],"%04u-%02u-%02u %02u:%02u:%02u",
   	       &ExamAnns->ExamAnn.ExamDate.Year,
	       &ExamAnns->ExamAnn.ExamDate.Month,
	       &ExamAnns->ExamAnn.ExamDate.Day,
               &ExamAnns->ExamAnn.StartTime.Hour,
	       &ExamAnns->ExamAnn.StartTime.Minute,
	       &Second) != 6)
      Lay_ShowErrorAndExit ("Wrong date of exam.");
   snprintf (ExamAnns->ExamAnn.ExamDate.YYYYMMDD,sizeof (ExamAnns->ExamAnn.ExamDate.YYYYMMDD),
	     "%04u%02u%02u",
             ExamAnns->ExamAnn.ExamDate.Year,
	     ExamAnns->ExamAnn.ExamDate.Month,
	     ExamAnns->ExamAnn.ExamDate.Day);

   /* Approximate duration (row[7]) */
   if (sscanf (row[7],"%02u:%02u:%02u",&ExamAnns->ExamAnn.Duration.Hour,&ExamAnns->ExamAnn.Duration.Minute,&Second) != 3)
      Lay_ShowErrorAndExit ("Wrong duration of exam.");

   /* Place (row[8]) */
   Str_Copy (ExamAnns->ExamAnn.Place,row[8],
             Cns_MAX_BYTES_TEXT);

   /* Exam mode (row[9]) */
   Str_Copy (ExamAnns->ExamAnn.Mode,row[9],
             Cns_MAX_BYTES_TEXT);

   /* Structure (row[10]) */
   Str_Copy (ExamAnns->ExamAnn.Structure,row[10],
             Cns_MAX_BYTES_TEXT);

   /* Documentation required (row[11]) */
   Str_Copy (ExamAnns->ExamAnn.DocRequired,row[11],
             Cns_MAX_BYTES_TEXT);

   /* Material required (row[12]) */
   Str_Copy (ExamAnns->ExamAnn.MatRequired,row[12],
             Cns_MAX_BYTES_TEXT);

   /* Material allowed (row[13]) */
   Str_Copy (ExamAnns->ExamAnn.MatAllowed,row[13],
             Cns_MAX_BYTES_TEXT);

   /* Other information for students (row[14]) */
   Str_Copy (ExamAnns->ExamAnn.OtherInfo,row[14],
             Cns_MAX_BYTES_TEXT);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Show a form with the data of an exam announcement **************/
/*****************************************************************************/

static void ExaAnn_ShowExamAnn (struct ExaAnn_ExamAnnouncements *ExamAnns,
                                long ExaCod,
				ExaAnn_TypeViewExamAnnouncement_t TypeViewExamAnnouncement,
				bool HighLight)
  {
   extern const char *Hlp_ASSESSMENT_Announcements_new_announcement;
   extern const char *Hlp_ASSESSMENT_Announcements_edit_announcement;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_EXAM_ANNOUNCEMENT;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Course;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Year_or_semester;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Session;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Exam_date;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Start_time;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Approximate_duration;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Place_of_exam;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Mode;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Structure_of_the_exam;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Documentation_required;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Material_required;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Material_allowed;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Other_information;
   extern const char *Txt_hours_ABBREVIATION;
   extern const char *Txt_hour;
   extern const char *Txt_hours;
   extern const char *Txt_minute;
   extern const char *Txt_minutes;
   extern const char *Txt_Publish_announcement_OF_EXAM;
   struct Instit Ins;
   char StrExamDate[Cns_MAX_BYTES_DATE + 1];
   unsigned Year;
   unsigned Hour;
   unsigned Minute;
   char *Anchor = NULL;
   const char *Width;
   void (*FunctionToDrawContextualIcons) (void *Args);
   const char *HelpLink;
   static const char *ClassExaAnnouncement[ExaAnn_NUM_VIEWS][ExaAnn_NUM_STATUS] =
     {
      [ExaAnn_NORMAL_VIEW][ExaAnn_VISIBLE_EXAM_ANNOUNCEMENT] = "EXA_ANN_VISIBLE",
      [ExaAnn_NORMAL_VIEW][ExaAnn_HIDDEN_EXAM_ANNOUNCEMENT ] = "EXA_ANN_HIDDEN",
      [ExaAnn_NORMAL_VIEW][ExaAnn_DELETED_EXAM_ANNOUNCEMENT] = NULL,	// Not applicable here

      [ExaAnn_PRINT_VIEW ][ExaAnn_VISIBLE_EXAM_ANNOUNCEMENT] = "EXA_ANN_VISIBLE",
      [ExaAnn_PRINT_VIEW ][ExaAnn_HIDDEN_EXAM_ANNOUNCEMENT ] = "EXA_ANN_VISIBLE",
      [ExaAnn_PRINT_VIEW ][ExaAnn_DELETED_EXAM_ANNOUNCEMENT] = NULL,	// Not applicable here

      [ExaAnn_FORM_VIEW  ][ExaAnn_VISIBLE_EXAM_ANNOUNCEMENT] = "EXA_ANN_VISIBLE",
      [ExaAnn_FORM_VIEW  ][ExaAnn_HIDDEN_EXAM_ANNOUNCEMENT ] = "EXA_ANN_VISIBLE",
      [ExaAnn_FORM_VIEW  ][ExaAnn_DELETED_EXAM_ANNOUNCEMENT] = NULL,	// Not applicable here
     };

   /***** Get data of institution of this degree *****/
   Ins.InsCod = Gbl.Hierarchy.Ins.InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins);

   /***** Build anchor string *****/
   Frm_SetAnchorStr (ExaCod,&Anchor);

   /***** Begin article *****/
   if (TypeViewExamAnnouncement == ExaAnn_NORMAL_VIEW)
      HTM_ARTICLE_Begin (Anchor);

   /***** Begin box *****/
   Width = "625px";
   ExamAnns->Anchor = Anchor;	// Used to put contextual icons
   ExamAnns->ExaCod = ExaCod;	// Used to put contextual icons
   FunctionToDrawContextualIcons = TypeViewExamAnnouncement == ExaAnn_NORMAL_VIEW ? ExaAnn_PutIconsExamAnn :
									            NULL;
   HelpLink = TypeViewExamAnnouncement == ExaAnn_FORM_VIEW ? ((ExaCod > 0) ? Hlp_ASSESSMENT_Announcements_edit_announcement :
									     Hlp_ASSESSMENT_Announcements_new_announcement) :
						             NULL;
   if (HighLight)
     {
      /* Show pending alerts */
      Ale_ShowAlerts (Anchor);

      /* Start highlighted box */
      Box_BoxShadowBegin (Width,NULL,
                          FunctionToDrawContextualIcons,ExamAnns,
                          HelpLink);
     }
   else	// Don't highlight
      /* Start normal box */
      Box_BoxBegin (Width,NULL,
                    FunctionToDrawContextualIcons,ExamAnns,
                    HelpLink,Box_NOT_CLOSABLE);

   if (TypeViewExamAnnouncement == ExaAnn_FORM_VIEW)
     {
      /***** Begin form *****/
      Frm_StartFormAnchor (ActRcvExaAnn,Anchor);
      if (ExaCod > 0)	// Existing announcement of exam
         ExaAnn_PutHiddenParamExaCod (ExaCod);
     }

   /***** Begin table *****/
   HTM_TABLE_Begin ("%s CELLS_PAD_2",
                    ClassExaAnnouncement[TypeViewExamAnnouncement][ExamAnns->ExamAnn.Status]);

   /***** Institution logo *****/
   HTM_TR_Begin (NULL);
   HTM_TD_Begin ("colspan=\"2\" class=\"CM\"");
   if (TypeViewExamAnnouncement == ExaAnn_PRINT_VIEW)
      HTM_SPAN_Begin ("class=\"EXAM_TIT\"");
   else
      HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"EXAM_TIT\"",
                   Ins.WWW);
   Lgo_DrawLogo (Hie_INS,Ins.InsCod,Ins.FullName,64,NULL,true);
   HTM_BR ();
   HTM_Txt (Ins.FullName);
   if (TypeViewExamAnnouncement == ExaAnn_PRINT_VIEW)
      HTM_SPAN_End ();
   else
      HTM_A_End ();
   HTM_TD_End ();
   HTM_TR_End ();

   /***** Degree *****/
   HTM_TR_Begin (NULL);
   HTM_TD_Begin ("colspan=\"2\" class=\"EXAM_TIT CM\"");
   if (TypeViewExamAnnouncement == ExaAnn_NORMAL_VIEW)
      HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"EXAM_TIT\"",
                   Gbl.Hierarchy.Deg.WWW);
   HTM_Txt (Gbl.Hierarchy.Deg.FullName);
   if (TypeViewExamAnnouncement == ExaAnn_NORMAL_VIEW)
      HTM_A_End ();
   HTM_TD_End ();
   HTM_TR_End ();

   /***** Title *****/
   HTM_TR_Begin (NULL);
   HTM_TD_Begin ("colspan=\"2\" class=\"EXAM CM\"");
   HTM_NBSP ();
   HTM_BR ();
   HTM_STRONG_Begin ();
   HTM_Txt (Txt_EXAM_ANNOUNCEMENT);
   HTM_STRONG_End ();
   HTM_TD_End ();
   HTM_TR_End ();

   HTM_TR_Begin (NULL);
   HTM_TD_Begin ("colspan=\"2\" class=\"EXAM LM\"");
   HTM_NBSP ();
   HTM_TD_End ();
   HTM_TR_End ();

   /***** Name of the course *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == ExaAnn_FORM_VIEW ? "CrsName" :
			                                           NULL,
		    Txt_EXAM_ANNOUNCEMENT_Course);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LB\"");
   if (TypeViewExamAnnouncement == ExaAnn_FORM_VIEW)
      HTM_INPUT_TEXT ("CrsName",Hie_MAX_CHARS_FULL_NAME,ExamAnns->ExamAnn.CrsFullName,
                      HTM_DONT_SUBMIT_ON_CHANGE,
		      "id=\"CrsName\" size=\"30\"");
   else
     {
      HTM_STRONG_Begin ();
      HTM_Txt (ExamAnns->ExamAnn.CrsFullName);
      HTM_STRONG_End ();
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Year/semester (N.A., 1º, 2º, 3º, 4º, 5º...) *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == ExaAnn_FORM_VIEW ? "Year" :
			                                        NULL,
		    Txt_EXAM_ANNOUNCEMENT_Year_or_semester);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LB\"");
   if (TypeViewExamAnnouncement == ExaAnn_FORM_VIEW)
     {
      HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
			"id=\"Year\" name=\"Year\"");
      for (Year = 0;
	   Year <= Deg_MAX_YEARS_PER_DEGREE;
	   Year++)
	 HTM_OPTION (HTM_Type_UNSIGNED,&Year,
		     ExamAnns->ExamAnn.Year == Year,false,
		     "%s",Txt_YEAR_OF_DEGREE[Year]);
      HTM_SELECT_End ();
     }
   else
      HTM_Txt (Txt_YEAR_OF_DEGREE[ExamAnns->ExamAnn.Year]);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Exam session *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == ExaAnn_FORM_VIEW ? "ExamSession" :
			                                           NULL,
		    Txt_EXAM_ANNOUNCEMENT_Session);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LB\"");
   if (TypeViewExamAnnouncement == ExaAnn_FORM_VIEW)
      HTM_INPUT_TEXT ("ExamSession",ExaAnn_MAX_CHARS_SESSION,ExamAnns->ExamAnn.Session,
                      HTM_DONT_SUBMIT_ON_CHANGE,
		      "id=\"ExamSession\" size=\"30\"");
   else
      HTM_Txt (ExamAnns->ExamAnn.Session);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Date of the exam *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
                    TypeViewExamAnnouncement == ExaAnn_FORM_VIEW ? "ExamYear" :
	                                                           NULL,
	            Txt_EXAM_ANNOUNCEMENT_Exam_date);

   /* Data */
   if (TypeViewExamAnnouncement == ExaAnn_FORM_VIEW)
     {
      HTM_TD_Begin ("class=\"LB\"");
      Dat_WriteFormDate (ExamAnns->ExamAnn.ExamDate.Year < Gbl.Now.Date.Year ? ExamAnns->ExamAnn.ExamDate.Year :
                                                                               Gbl.Now.Date.Year,
                         Gbl.Now.Date.Year + 1,"Exam",
                         &(ExamAnns->ExamAnn.ExamDate),
                         false,false);
      HTM_TD_End ();
     }
   else
     {
      Dat_ConvDateToDateStr (&ExamAnns->ExamAnn.ExamDate,
                             StrExamDate);
      HTM_TD_Begin ("class=\"EXAM LB\"");
      HTM_Txt (StrExamDate);
      HTM_TD_End ();
     }
   HTM_TR_End ();

   /***** Start time *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
                    TypeViewExamAnnouncement == ExaAnn_FORM_VIEW ? "ExamHour" :
	                                                           NULL,
	            Txt_EXAM_ANNOUNCEMENT_Start_time);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LB\"");
   if (TypeViewExamAnnouncement == ExaAnn_FORM_VIEW)
     {
      HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
			"id=\"ExamHour\" name=\"ExamHour\"");
      HTM_OPTION (HTM_Type_STRING,"0",
		  ExamAnns->ExamAnn.StartTime.Hour == 0,false,
		  "-");
      for (Hour = 7;
	   Hour <= 22;
	   Hour++)
	 HTM_OPTION (HTM_Type_UNSIGNED,&Hour,
		     ExamAnns->ExamAnn.StartTime.Hour == Hour,false,
		     "%02u %s",Hour,Txt_hours_ABBREVIATION);
      HTM_SELECT_End ();

      HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
			"name=\"ExamMinute\"");
      for (Minute = 0;
	   Minute <= 59;
	   Minute++)
	 HTM_OPTION (HTM_Type_UNSIGNED,&Minute,
		     ExamAnns->ExamAnn.StartTime.Minute == Minute,false,
		     "%02u &prime;",Minute);
      HTM_SELECT_End ();
     }
   else if (ExamAnns->ExamAnn.StartTime.Hour)
      HTM_TxtF ("%2u:%02u",ExamAnns->ExamAnn.StartTime.Hour,
                           ExamAnns->ExamAnn.StartTime.Minute);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Approximate duration of the exam *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
                    TypeViewExamAnnouncement == ExaAnn_FORM_VIEW ? "DurationHour" :
	                                                           NULL,
	            Txt_EXAM_ANNOUNCEMENT_Approximate_duration);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LB\"");
   if (TypeViewExamAnnouncement == ExaAnn_FORM_VIEW)
     {
      HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
			"id=\"DurationHour\" name=\"DurationHour\"");
      for (Hour = 0;
	   Hour <= 8;
	   Hour++)
	 HTM_OPTION (HTM_Type_UNSIGNED,&Hour,
		     ExamAnns->ExamAnn.Duration.Hour == Hour,false,
		     "%02u %s",Hour,Txt_hours_ABBREVIATION);
      HTM_SELECT_End ();

      HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
			"name=\"DurationMinute\"");
      for (Minute = 0;
	   Minute <= 59;
	   Minute++)
	 HTM_OPTION (HTM_Type_UNSIGNED,&Minute,
		     ExamAnns->ExamAnn.Duration.Minute == Minute,false,
		     "%02u &prime;",Minute);
      HTM_SELECT_End ();
     }
   else if (ExamAnns->ExamAnn.Duration.Hour ||
            ExamAnns->ExamAnn.Duration.Minute)
     {
      if (ExamAnns->ExamAnn.Duration.Hour)
        {
         if (ExamAnns->ExamAnn.Duration.Minute)
            HTM_TxtF ("%u%s %u&prime;",ExamAnns->ExamAnn.Duration.Hour,
                                       Txt_hours_ABBREVIATION,
                                       ExamAnns->ExamAnn.Duration.Minute);
         else
            HTM_TxtF ("%u&nbsp;%s",ExamAnns->ExamAnn.Duration.Hour,
				   ExamAnns->ExamAnn.Duration.Hour == 1 ? Txt_hour :
					                                  Txt_hours);
        }
      else if (ExamAnns->ExamAnn.Duration.Minute)
        {
         HTM_TxtF ("%u&nbsp;%s",ExamAnns->ExamAnn.Duration.Minute,
			        ExamAnns->ExamAnn.Duration.Minute == 1 ? Txt_minute :
				                                         Txt_minutes);
        }
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Place where the exam will be made *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == ExaAnn_FORM_VIEW ? "Place" :
			                                        NULL,
		    Txt_EXAM_ANNOUNCEMENT_Place_of_exam);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LB\"");
   if (TypeViewExamAnnouncement == ExaAnn_FORM_VIEW)
     {
      HTM_TEXTAREA_Begin ("id=\"Place\" name=\"Place\" cols=\"40\" rows=\"4\"");
      HTM_Txt (ExamAnns->ExamAnn.Place);
      HTM_TEXTAREA_End ();
     }
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        ExamAnns->ExamAnn.Place,
                        Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (ExamAnns->ExamAnn.Place);
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Exam mode *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == ExaAnn_FORM_VIEW ? "ExamMode" :
			                                           NULL,
		    Txt_EXAM_ANNOUNCEMENT_Mode);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LB\"");
   if (TypeViewExamAnnouncement == ExaAnn_FORM_VIEW)
     {
      HTM_TEXTAREA_Begin ("id=\"ExamMode\" name=\"ExamMode\" cols=\"40\" rows=\"2\"");
      HTM_Txt (ExamAnns->ExamAnn.Mode);
      HTM_TEXTAREA_End ();
     }
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        ExamAnns->ExamAnn.Mode,
                        Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (ExamAnns->ExamAnn.Mode);
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Structure of the exam *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == ExaAnn_FORM_VIEW ? "Structure" :
			                                        NULL,
		    Txt_EXAM_ANNOUNCEMENT_Structure_of_the_exam);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LB\"");
   if (TypeViewExamAnnouncement == ExaAnn_FORM_VIEW)
     {
      HTM_TEXTAREA_Begin ("id=\"Structure\" name=\"Structure\" cols=\"40\" rows=\"8\"");
      HTM_Txt (ExamAnns->ExamAnn.Structure);
      HTM_TEXTAREA_End ();
     }
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        ExamAnns->ExamAnn.Structure,
                        Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (ExamAnns->ExamAnn.Structure);
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Documentation required *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == ExaAnn_FORM_VIEW ? "DocRequired" :
			                                        NULL,
		    Txt_EXAM_ANNOUNCEMENT_Documentation_required);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LB\"");
   if (TypeViewExamAnnouncement == ExaAnn_FORM_VIEW)
     {
      HTM_TEXTAREA_Begin ("id=\"DocRequired\" name=\"DocRequired\" cols=\"40\" rows=\"2\"");
      HTM_Txt (ExamAnns->ExamAnn.DocRequired);
      HTM_TEXTAREA_End ();
     }
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        ExamAnns->ExamAnn.DocRequired,
                        Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (ExamAnns->ExamAnn.DocRequired);
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Material required *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == ExaAnn_FORM_VIEW ? "MatRequired" :
			                                           NULL,
		    Txt_EXAM_ANNOUNCEMENT_Material_required);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LB\"");
   if (TypeViewExamAnnouncement == ExaAnn_FORM_VIEW)
     {
      HTM_TEXTAREA_Begin ("id=\"MatRequired\" name=\"MatRequired\" cols=\"40\" rows=\"4\"");
      HTM_Txt (ExamAnns->ExamAnn.MatRequired);
      HTM_TEXTAREA_End ();
     }
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        ExamAnns->ExamAnn.MatRequired,
                        Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (ExamAnns->ExamAnn.MatRequired);
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Material allowed *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == ExaAnn_FORM_VIEW ? "MatAllowed" :
			                                           NULL,
		    Txt_EXAM_ANNOUNCEMENT_Material_allowed);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LB\"");
   if (TypeViewExamAnnouncement == ExaAnn_FORM_VIEW)
     {
      HTM_TEXTAREA_Begin ("id=\"MatAllowed\" name=\"MatAllowed\" cols=\"40\" rows=\"4\"");
      HTM_Txt (ExamAnns->ExamAnn.MatAllowed);
      HTM_TEXTAREA_End ();
     }
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        ExamAnns->ExamAnn.MatAllowed,
                        Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (ExamAnns->ExamAnn.MatAllowed);
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Other information to students *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == ExaAnn_FORM_VIEW ? "OtherInfo" :
			                                        NULL,
		    Txt_EXAM_ANNOUNCEMENT_Other_information);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LB\"");
   if (TypeViewExamAnnouncement == ExaAnn_FORM_VIEW)
     {
      HTM_TEXTAREA_Begin ("id=\"OtherInfo\" name=\"OtherInfo\" cols=\"40\" rows=\"5\"");
      HTM_Txt (ExamAnns->ExamAnn.OtherInfo);
      HTM_TEXTAREA_End ();
     }
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        ExamAnns->ExamAnn.OtherInfo,
                        Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (ExamAnns->ExamAnn.OtherInfo);
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   if (TypeViewExamAnnouncement == ExaAnn_FORM_VIEW)
      Box_BoxTableWithButtonEnd ((ExaCod > 0) ? Btn_CONFIRM_BUTTON :
	                                        Btn_CREATE_BUTTON,
	                         Txt_Publish_announcement_OF_EXAM);
   else
      Box_BoxTableEnd ();

   /***** Show QR code *****/
   if (TypeViewExamAnnouncement == ExaAnn_PRINT_VIEW)
      QR_ExamAnnnouncement ();

   /***** End article *****/
   if (TypeViewExamAnnouncement == ExaAnn_NORMAL_VIEW)
      HTM_ARTICLE_End ();

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);
  }

/*****************************************************************************/
/********* Put icons to remove / edit / print an exam announcement ***********/
/*****************************************************************************/

static void ExaAnn_PutIconsExamAnn (void *ExamAnns)
  {
   if (ExamAnns)
     {
      if (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
	  Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	{
	 /***** Link to remove this exam announcement *****/
	 Ico_PutContextualIconToRemove (ActReqRemExaAnn,NULL,
					ExaAnn_PutParamExaCodToEdit,&((struct ExaAnn_ExamAnnouncements *) ExamAnns)->ExaCod);

	 /***** Put form to hide/show exam announement *****/
	 switch (((struct ExaAnn_ExamAnnouncements *) ExamAnns)->ExamAnn.Status)
	   {
	    case ExaAnn_VISIBLE_EXAM_ANNOUNCEMENT:
	       Ico_PutContextualIconToHide (ActHidExaAnn,((struct ExaAnn_ExamAnnouncements *) ExamAnns)->Anchor,
					    ExaAnn_PutParamExaCodToEdit,&((struct ExaAnn_ExamAnnouncements *) ExamAnns)->ExaCod);
	       break;
	    case ExaAnn_HIDDEN_EXAM_ANNOUNCEMENT:
	       Ico_PutContextualIconToUnhide (ActUnhExaAnn,((struct ExaAnn_ExamAnnouncements *) ExamAnns)->Anchor,
					      ExaAnn_PutParamExaCodToEdit,&((struct ExaAnn_ExamAnnouncements *) ExamAnns)->ExaCod);
	       break;
	    case ExaAnn_DELETED_EXAM_ANNOUNCEMENT:	// Not applicable here
	       break;
	   }

	 /***** Link to edit this exam announcement *****/
	 Ico_PutContextualIconToEdit (ActEdiExaAnn,NULL,
				      ExaAnn_PutParamExaCodToEdit,&((struct ExaAnn_ExamAnnouncements *) ExamAnns)->ExaCod);
	}

      /***** Link to print view *****/
      Ico_PutContextualIconToPrint (ActPrnExaAnn,
				    ExaAnn_PutParamExaCodToEdit,&((struct ExaAnn_ExamAnnouncements *) ExamAnns)->ExaCod);
     }
  }

/*****************************************************************************/
/*************** Param with the code of an exam announcement *****************/
/*****************************************************************************/

static void ExaAnn_PutParamExaCodToEdit (void *ExaCod)
  {
   if (ExaCod)
      ExaAnn_PutHiddenParamExaCod (*((long *) ExaCod));
  }

void ExaAnn_PutHiddenParamExaCod (long ExaCod)
  {
   Par_PutHiddenParamLong (NULL,"ExaCod",ExaCod);
  }

/*****************************************************************************/
/********** Get parameter with the code of an exam announcement **************/
/*****************************************************************************/

static long ExaAnn_GetParamExaCod (void)
  {
   /* Get notice code */
   return Par_GetParToLong ("ExaCod");
  }

/*****************************************************************************/
/************ Get summary and content about an exam announcement *************/
/*****************************************************************************/

void ExaAnn_GetSummaryAndContentExamAnn (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                         char **ContentStr,
                                         long ExaCod,bool GetContent)
  {
   extern const char *Txt_hours_ABBREVIATION;
   struct ExaAnn_ExamAnnouncements ExamAnns;
   char CrsNameAndDate[Hie_MAX_BYTES_FULL_NAME + (2 + Cns_MAX_BYTES_DATE + 7) + 1];
   char StrExamDate[Cns_MAX_BYTES_DATE + 1];

   /***** Reset exam announcements context *****/
   ExaAnn_ResetExamAnns (&ExamAnns);

   /***** Initializations *****/
   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Allocate memory for the exam announcement *****/
   ExaAnn_AllocMemExamAnn (&ExamAnns);

   /***** Get data of an exam announcement from database *****/
   ExaAnn_GetDataExamAnnFromDB (&ExamAnns,ExaCod);

   /***** Content *****/
   if (GetContent)
      ExaAnn_GetNotifContentExamAnn (&ExamAnns,ContentStr);

   /***** Summary *****/
   /* Name of the course and date of exam */
   Dat_ConvDateToDateStr (&ExamAnns.ExamAnn.ExamDate,StrExamDate);
   snprintf (CrsNameAndDate,sizeof (CrsNameAndDate),
	     "%s, %s, %2u:%02u",
             ExamAnns.ExamAnn.CrsFullName,
             StrExamDate,
             ExamAnns.ExamAnn.StartTime.Hour,
             ExamAnns.ExamAnn.StartTime.Minute);
   Str_Copy (SummaryStr,CrsNameAndDate,
             Ntf_MAX_BYTES_SUMMARY);

   /***** Free memory of the exam announcement *****/
   ExaAnn_FreeMemExamAnn (&ExamAnns);
  }

/*****************************************************************************/
/************ Show a form with the data of an exam announcement **************/
/*****************************************************************************/

static void ExaAnn_GetNotifContentExamAnn (const struct ExaAnn_ExamAnnouncements *ExamAnns,
                                           char **ContentStr)
  {
   extern const char *Txt_Institution;
   extern const char *Txt_Degree;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_EXAM_ANNOUNCEMENT_Course;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Year_or_semester;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Session;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Exam_date;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Start_time;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Approximate_duration;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Place_of_exam;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Mode;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Structure_of_the_exam;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Documentation_required;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Material_required;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Material_allowed;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Other_information;
   extern const char *Txt_hours_ABBREVIATION;
   struct Course Crs;
   struct Degree Deg;
   struct Instit Ins;
   char StrExamDate[Cns_MAX_BYTES_DATE + 1];

   /***** Get data of course *****/
   Crs.CrsCod = ExamAnns->ExamAnn.CrsCod;
   Crs_GetDataOfCourseByCod (&Crs);

   /***** Get data of degree *****/
   Deg.DegCod = Crs.DegCod;
   Deg_GetDataOfDegreeByCod (&Deg);

   /***** Get data of institution *****/
   Ins.InsCod = Deg_GetInsCodOfDegreeByCod (Deg.DegCod);
   Ins_GetDataOfInstitutionByCod (&Ins);

   /***** Convert struct date to a date string *****/
   Dat_ConvDateToDateStr (&ExamAnns->ExamAnn.ExamDate,StrExamDate);

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
                 Txt_Institution,Ins.FullName,
                 Txt_Degree,Deg.FullName,
                 Txt_EXAM_ANNOUNCEMENT_Course,ExamAnns->ExamAnn.CrsFullName,
                 Txt_EXAM_ANNOUNCEMENT_Year_or_semester,Txt_YEAR_OF_DEGREE[ExamAnns->ExamAnn.Year],
                 Txt_EXAM_ANNOUNCEMENT_Session,ExamAnns->ExamAnn.Session,
                 Txt_EXAM_ANNOUNCEMENT_Exam_date,StrExamDate,
                 Txt_EXAM_ANNOUNCEMENT_Start_time,ExamAnns->ExamAnn.StartTime.Hour,
                                                  ExamAnns->ExamAnn.StartTime.Minute,
                 Txt_hours_ABBREVIATION,
                 Txt_EXAM_ANNOUNCEMENT_Approximate_duration,ExamAnns->ExamAnn.Duration.Hour,
                                                            ExamAnns->ExamAnn.Duration.Minute,
                 Txt_hours_ABBREVIATION,
                 Txt_EXAM_ANNOUNCEMENT_Place_of_exam,ExamAnns->ExamAnn.Place,
                 Txt_EXAM_ANNOUNCEMENT_Mode,ExamAnns->ExamAnn.Mode,
                 Txt_EXAM_ANNOUNCEMENT_Structure_of_the_exam,ExamAnns->ExamAnn.Structure,
                 Txt_EXAM_ANNOUNCEMENT_Documentation_required,ExamAnns->ExamAnn.DocRequired,
                 Txt_EXAM_ANNOUNCEMENT_Material_required,ExamAnns->ExamAnn.MatRequired,
                 Txt_EXAM_ANNOUNCEMENT_Material_allowed,ExamAnns->ExamAnn.MatAllowed,
                 Txt_EXAM_ANNOUNCEMENT_Other_information,ExamAnns->ExamAnn.OtherInfo) < 0)
      Lay_NotEnoughMemoryExit ();
  }
