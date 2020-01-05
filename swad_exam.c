// swad_exam.c: exam announcements

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    Copyright (C) 1999-2014
        SWAD core (1999-2014):
            Antonio Cañas Vargas
        Photo processing (2006-2014):
            Daniel J. Calandria Hernández
        Chat (2009-2014):
            Daniel J. Calandria Hernández

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

#define _GNU_SOURCE 		// For asprintf
#include <stddef.h>		// For NULL
#include <stdio.h>		// For sscanf, asprintf, etc.
#include <stdlib.h>		// For exit, system, malloc, calloc, free, etc.
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_exam.h"
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

static long Exa_GetParamsExamAnnouncement (void);
static void Exa_AllocMemExamAnnouncement (void);
static void Exa_UpdateNumUsrsNotifiedByEMailAboutExamAnnouncement (long ExaCod,unsigned NumUsrsToBeNotifiedByEMail);
static void Exa_ListExamAnnouncements (Exa_TypeViewExamAnnouncement_t TypeViewExamAnnouncement);
static void Exa_PutIconToCreateNewExamAnnouncement (void);
static void Exa_PutButtonToCreateNewExamAnnouncement (void);

static long Exa_AddExamAnnouncementToDB (void);
static void Exa_ModifyExamAnnouncementInDB (long ExaCod);
static void Exa_GetDataExamAnnouncementFromDB (long ExaCod);
static void Exa_ShowExamAnnouncement (long ExaCod,
				      Exa_TypeViewExamAnnouncement_t TypeViewExamAnnouncement,
				      bool HighLight);
static void Exa_PutIconsExamAnnouncement (void);
static void Exa_PutParamExaCodToEdit (void);
static long Exa_GetParamExaCod (void);

static void Exa_GetNotifContentExamAnnouncement (char **ContentStr);

/*****************************************************************************/
/********************** Form to edit an exam announcement ********************/
/*****************************************************************************/

void Exa_PutFrmEditAExamAnnouncement (void)
  {
   long ExaCod;

   /***** Allocate memory for the exam announcement *****/
   Exa_AllocMemExamAnnouncement ();

   /***** Get the code of the exam announcement *****/
   ExaCod = Exa_GetParamsExamAnnouncement ();

   if (ExaCod > 0)	// -1 indicates that this is a new exam announcement
      /***** Read exam announcement from the database *****/
      Exa_GetDataExamAnnouncementFromDB (ExaCod);

   /***** Show exam announcement *****/
   Exa_ShowExamAnnouncement (ExaCod,Exa_FORM_VIEW,
			     false);	// Don't highlight

   /***** Free memory of the exam announcement *****/
   Exa_FreeMemExamAnnouncement ();
  }

/*****************************************************************************/
/**************** Get parameters of an exam announcement *********************/
/*****************************************************************************/

static long Exa_GetParamsExamAnnouncement (void)
  {
   long ExaCod;

   /***** Get the code of the exam announcement *****/
   ExaCod = Exa_GetParamExaCod ();

   /***** Get the name of the course (it is allowed to be different from the official name of the course) *****/
   Par_GetParToText ("CrsName",Gbl.ExamAnns.ExaDat.CrsFullName,Hie_MAX_BYTES_FULL_NAME);
   // If the parameter is not present or is empty, initialize the string to the full name of the current course
   if (!Gbl.ExamAnns.ExaDat.CrsFullName[0])
      Str_Copy (Gbl.ExamAnns.ExaDat.CrsFullName,Gbl.Hierarchy.Crs.FullName,
                Hie_MAX_BYTES_FULL_NAME);

   /***** Get the year *****/
   Gbl.ExamAnns.ExaDat.Year = (unsigned)
	                      Par_GetParToUnsignedLong ("Year",
                                                        0,	// N.A.
                                                        Deg_MAX_YEARS_PER_DEGREE,
                                                        (unsigned long) Gbl.Hierarchy.Crs.Year);

   /***** Get the type of exam announcement *****/
   Par_GetParToText ("ExamSession",Gbl.ExamAnns.ExaDat.Session,Exa_MAX_BYTES_SESSION);

   /***** Get the data of the exam *****/
   Dat_GetDateFromForm ("ExamDay","ExamMonth","ExamYear",
                        &Gbl.ExamAnns.ExaDat.ExamDate.Day,
                        &Gbl.ExamAnns.ExaDat.ExamDate.Month,
                        &Gbl.ExamAnns.ExaDat.ExamDate.Year);
   if (Gbl.ExamAnns.ExaDat.ExamDate.Day   == 0 ||
       Gbl.ExamAnns.ExaDat.ExamDate.Month == 0 ||
       Gbl.ExamAnns.ExaDat.ExamDate.Year  == 0)
     {
      Gbl.ExamAnns.ExaDat.ExamDate.Day   = Gbl.Now.Date.Day;
      Gbl.ExamAnns.ExaDat.ExamDate.Month = Gbl.Now.Date.Month;
      Gbl.ExamAnns.ExaDat.ExamDate.Year  = Gbl.Now.Date.Year;
     }

   /***** Get the hour of the exam *****/
   Gbl.ExamAnns.ExaDat.StartTime.Hour   = (unsigned) Par_GetParToUnsignedLong ("ExamHour",
                                                                               0,23,0);
   Gbl.ExamAnns.ExaDat.StartTime.Minute = (unsigned) Par_GetParToUnsignedLong ("ExamMinute",
                                                                               0,59,0);

   /***** Get the duration of the exam *****/
   Gbl.ExamAnns.ExaDat.Duration.Hour    = (unsigned) Par_GetParToUnsignedLong ("DurationHour",
                                                                               0,23,0);
   Gbl.ExamAnns.ExaDat.Duration.Minute  = (unsigned) Par_GetParToUnsignedLong ("DurationMinute",
                                                                               0,59,0);

   /***** Get the place where the exam will happen *****/
   Par_GetParToHTML ("Place",Gbl.ExamAnns.ExaDat.Place,Cns_MAX_BYTES_TEXT);

   /***** Get the modality of exam *****/
   Par_GetParToHTML ("ExamMode",Gbl.ExamAnns.ExaDat.Mode,Cns_MAX_BYTES_TEXT);

   /***** Get the structure of exam *****/
   Par_GetParToHTML ("Structure",Gbl.ExamAnns.ExaDat.Structure,Cns_MAX_BYTES_TEXT);

   /***** Get the mandatory documentation *****/
   Par_GetParToHTML ("DocRequired",Gbl.ExamAnns.ExaDat.DocRequired,Cns_MAX_BYTES_TEXT);

   /***** Get the mandatory material *****/
   Par_GetParToHTML ("MatRequired",Gbl.ExamAnns.ExaDat.MatRequired,Cns_MAX_BYTES_TEXT);

   /***** Get the allowed material *****/
   Par_GetParToHTML ("MatAllowed",Gbl.ExamAnns.ExaDat.MatAllowed,Cns_MAX_BYTES_TEXT);

   /***** Get other information *****/
   Par_GetParToHTML ("OtherInfo",Gbl.ExamAnns.ExaDat.OtherInfo,Cns_MAX_BYTES_TEXT);

   return ExaCod;
  }

/*****************************************************************************/
/* Allocate memory for those parameters of an exam anno. with a lot of text **/
/*****************************************************************************/

static void Exa_AllocMemExamAnnouncement (void)
  {
   if ((Gbl.ExamAnns.ExaDat.Place       = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   if ((Gbl.ExamAnns.ExaDat.Mode        = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   if ((Gbl.ExamAnns.ExaDat.Structure   = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   if ((Gbl.ExamAnns.ExaDat.DocRequired = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   if ((Gbl.ExamAnns.ExaDat.MatRequired = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   if ((Gbl.ExamAnns.ExaDat.MatAllowed  = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   if ((Gbl.ExamAnns.ExaDat.OtherInfo   = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();
  }

/*****************************************************************************/
/* Free memory of those parameters of an exam announcem. with a lot of text **/
/*****************************************************************************/

void Exa_FreeMemExamAnnouncement (void)
  {
   if (Gbl.ExamAnns.ExaDat.Place)
     {
      free (Gbl.ExamAnns.ExaDat.Place);
      Gbl.ExamAnns.ExaDat.Place = NULL;
     }
   if (Gbl.ExamAnns.ExaDat.Mode)
     {
      free (Gbl.ExamAnns.ExaDat.Mode);
      Gbl.ExamAnns.ExaDat.Mode = NULL;
     }
   if (Gbl.ExamAnns.ExaDat.Structure)
     {
      free (Gbl.ExamAnns.ExaDat.Structure);
      Gbl.ExamAnns.ExaDat.Structure = NULL;
     }
   if (Gbl.ExamAnns.ExaDat.DocRequired)
     {
      free (Gbl.ExamAnns.ExaDat.DocRequired);
      Gbl.ExamAnns.ExaDat.DocRequired = NULL;
     }
   if (Gbl.ExamAnns.ExaDat.MatRequired)
     {
      free (Gbl.ExamAnns.ExaDat.MatRequired);
      Gbl.ExamAnns.ExaDat.MatRequired = NULL;
     }
   if (Gbl.ExamAnns.ExaDat.MatAllowed)
     {
      free (Gbl.ExamAnns.ExaDat.MatAllowed);
      Gbl.ExamAnns.ExaDat.MatAllowed = NULL;
     }
   if (Gbl.ExamAnns.ExaDat.OtherInfo)
     {
      free (Gbl.ExamAnns.ExaDat.OtherInfo);
      Gbl.ExamAnns.ExaDat.OtherInfo = NULL;
     }
  }

/*****************************************************************************/
/************************ Receive an exam announcement ***********************/
/*****************************************************************************/
// This function is splitted into a-priori and a-posteriori functions
// in order to view updated links in month of left column

void Exa_ReceiveExamAnnouncement1 (void)
  {
   extern const char *Txt_Created_new_announcement_of_exam;
   extern const char *Txt_The_announcement_of_exam_has_been_successfully_updated;
   long ExaCod;
   bool NewExamAnnouncement;
   char *Anchor = NULL;

   /***** Allocate memory for the exam announcement *****/
   Exa_AllocMemExamAnnouncement ();

   /***** Get parameters of the exam announcement *****/
   ExaCod = Exa_GetParamsExamAnnouncement ();
   NewExamAnnouncement = (ExaCod < 0);

   /***** Add the exam announcement to the database and read it again from the database *****/
   if (NewExamAnnouncement)
      Gbl.ExamAnns.NewExaCod = ExaCod = Exa_AddExamAnnouncementToDB ();
   else
      Exa_ModifyExamAnnouncementInDB (ExaCod);

   /***** Free memory of the exam announcement *****/
   Exa_FreeMemExamAnnouncement ();

   /***** Create alert to show the change made *****/
   Frm_SetAnchorStr (ExaCod,&Anchor);
   Ale_CreateAlert (Ale_SUCCESS,Anchor,
                    NewExamAnnouncement ? Txt_Created_new_announcement_of_exam :
                                          Txt_The_announcement_of_exam_has_been_successfully_updated);
   Frm_FreeAnchorStr (Anchor);

   /***** Set exam to be highlighted *****/
   Gbl.ExamAnns.HighlightExaCod = ExaCod;
  }

void Exa_ReceiveExamAnnouncement2 (void)
  {
   unsigned NumUsrsToBeNotifiedByEMail;
   struct TL_Publication SocPub;

   /***** Notify by email about the new exam announcement *****/
   if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_EXAM_ANNOUNCEMENT,Gbl.ExamAnns.HighlightExaCod)))
      Exa_UpdateNumUsrsNotifiedByEMailAboutExamAnnouncement (Gbl.ExamAnns.HighlightExaCod,NumUsrsToBeNotifiedByEMail);

   /***** Create a new social note about the new exam announcement *****/
   TL_StoreAndPublishNote (TL_NOTE_EXAM_ANNOUNCEMENT,Gbl.ExamAnns.HighlightExaCod,&SocPub);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.Hierarchy.Crs);

   /***** Show exam announcements *****/
   Exa_ListExamAnnouncementsEdit ();
  }

/*****************************************************************************/
/***** Update number of users notified in table of exam announcements ********/
/*****************************************************************************/

static void Exa_UpdateNumUsrsNotifiedByEMailAboutExamAnnouncement (long ExaCod,unsigned NumUsrsToBeNotifiedByEMail)
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

void Exa_PrintExamAnnouncement (void)
  {
   long ExaCod;

   /***** Allocate memory for the exam announcement *****/
   Exa_AllocMemExamAnnouncement ();

   /***** Get the code of the exam announcement *****/
   if ((ExaCod = Exa_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Read exam announcement from the database *****/
   Exa_GetDataExamAnnouncementFromDB (ExaCod);

   /***** Show exam announcement *****/
   Exa_ShowExamAnnouncement (ExaCod,Exa_PRINT_VIEW,
			     false);	// Don't highlight

   /***** Free memory of the exam announcement *****/
   Exa_FreeMemExamAnnouncement ();
  }

/*****************************************************************************/
/************************ Remove an exam announcement ************************/
/*****************************************************************************/

void Exa_ReqRemoveExamAnnouncement (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_following_announcement_of_exam;
   extern const char *Txt_Remove;
   long ExaCod;

   /***** Get the code of the exam announcement *****/
   if ((ExaCod = Exa_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Show question and button to remove exam announcement *****/
   /* Start alert */
   Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_following_announcement_of_exam);

   /* Show announcement */
   Exa_AllocMemExamAnnouncement ();
   Exa_GetDataExamAnnouncementFromDB (ExaCod);
   Exa_ShowExamAnnouncement (ExaCod,Exa_NORMAL_VIEW,
			     false);	// Don't highlight
   Exa_FreeMemExamAnnouncement ();

   /* End alert */

   Ale_ShowAlertAndButton2 (ActRemExaAnn,NULL,NULL,Exa_PutParamExaCodToEdit,
			    Btn_REMOVE_BUTTON,Txt_Remove);
  }

/*****************************************************************************/
/************************ Remove an exam announcement ************************/
/*****************************************************************************/
// This function is splitted into a-priori and a-posteriori functions
// in order to view updated links in month of left column

void Exa_RemoveExamAnnouncement1 (void)
  {
   long ExaCod;

   /***** Get the code of the exam announcement *****/
   if ((ExaCod = Exa_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Mark the exam announcement as deleted in the database *****/
   DB_QueryUPDATE ("can not remove exam announcement",
		   "UPDATE exam_announcements SET Status=%u"
		   " WHERE ExaCod=%ld AND CrsCod=%ld",
                   (unsigned) Exa_DELETED_EXAM_ANNOUNCEMENT,
                   ExaCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Mark possible notifications as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_EXAM_ANNOUNCEMENT,ExaCod);

   /***** Mark possible social note as unavailable *****/
   TL_MarkNoteAsUnavailableUsingNoteTypeAndCod (TL_NOTE_EXAM_ANNOUNCEMENT,ExaCod);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.Hierarchy.Crs);
  }

void Exa_RemoveExamAnnouncement2 (void)
  {
   extern const char *Txt_Announcement_of_exam_removed;

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Announcement_of_exam_removed);

   /***** List again all the remaining exam announcements *****/
   Exa_ListExamAnnouncementsEdit ();
  }

/*****************************************************************************/
/************************ Hide an exam announcement **************************/
/*****************************************************************************/
// This function is splitted into a-priori and a-posteriori functions
// in order to view updated links in month of left column

void Exa_HideExamAnnouncement (void)
  {
   long ExaCod;

   /***** Get the code of the exam announcement *****/
   if ((ExaCod = Exa_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Mark the exam announcement as hidden in the database *****/
   DB_QueryUPDATE ("can not hide exam announcement",
		   "UPDATE exam_announcements SET Status=%u"
		   " WHERE ExaCod=%ld AND CrsCod=%ld",
                   (unsigned) Exa_HIDDEN_EXAM_ANNOUNCEMENT,
                   ExaCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Set exam to be highlighted *****/
   Gbl.ExamAnns.HighlightExaCod = ExaCod;
  }

/*****************************************************************************/
/************************ Unhide an exam announcement ************************/
/*****************************************************************************/
// This function is splitted into a-priori and a-posteriori functions
// in order to view updated links in month of left column

void Exa_UnhideExamAnnouncement (void)
  {
   long ExaCod;

   /***** Get the code of the exam announcement *****/
   if ((ExaCod = Exa_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Mark the exam announcement as visible in the database *****/
   DB_QueryUPDATE ("can not unhide exam announcement",
		   "UPDATE exam_announcements SET Status=%u"
		   " WHERE ExaCod=%ld AND CrsCod=%ld",
                   (unsigned) Exa_VISIBLE_EXAM_ANNOUNCEMENT,
                   ExaCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Set exam to be highlighted *****/
   Gbl.ExamAnns.HighlightExaCod = ExaCod;
  }

/*****************************************************************************/
/*************** List all the exam announcements to see them *****************/
/*****************************************************************************/

void Exa_ListExamAnnouncementsSee (void)
  {
   /***** List all exam announcements *****/
   Exa_ListExamAnnouncements (Exa_NORMAL_VIEW);

   /***** Mark possible notifications as seen *****/
   Ntf_MarkNotifAsSeen (Ntf_EVENT_EXAM_ANNOUNCEMENT,
	                -1L,Gbl.Hierarchy.Crs.CrsCod,
	                Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********** List all the exam announcements to edit or remove them ***********/
/*****************************************************************************/

void Exa_ListExamAnnouncementsEdit (void)
  {
   Exa_ListExamAnnouncements (Exa_NORMAL_VIEW);
  }

/*****************************************************************************/
/*********** Get date of exam announcements to show highlighted **************/
/*****************************************************************************/

void Exa_GetExaCodToHighlight (void)
  {
   /***** Get the exam announcement code
          of the exam announcement to highlight *****/
   Gbl.ExamAnns.HighlightExaCod = Exa_GetParamExaCod ();
  }

/*****************************************************************************/
/*********** Get date of exam announcements to show highlighted **************/
/*****************************************************************************/

void Exa_GetDateToHighlight (void)
  {
   /***** Get the date (in YYYYMMDD format)
          of the exam announcements to highlight *****/
   Par_GetParToText ("Date",Gbl.ExamAnns.HighlightDate,4 + 2 + 2);
  }

/*****************************************************************************/
/******************** List all the exam announcements ************************/
/*****************************************************************************/

static void Exa_ListExamAnnouncements (Exa_TypeViewExamAnnouncement_t TypeViewExamAnnouncement)
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
	       (unsigned) Exa_DELETED_EXAM_ANNOUNCEMENT);
   else
      sprintf (SubQueryStatus,"Status=%u",
	       (unsigned) Exa_VISIBLE_EXAM_ANNOUNCEMENT);

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
   Box_BoxBegin (NULL,Txt_Announcements_of_exams,
		 ICanEdit ? Exa_PutIconToCreateNewExamAnnouncement :
			    NULL,
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
      Exa_AllocMemExamAnnouncement ();

      /***** Read the data of the exam announcement *****/
      Exa_GetDataExamAnnouncementFromDB (ExaCod);

      /***** Show exam announcement *****/
      HighLight = false;
      if (ExaCod == Gbl.ExamAnns.HighlightExaCod)
	 HighLight = true;
      else if (Gbl.ExamAnns.HighlightDate[0])
        {
	 if (!strcmp (Gbl.ExamAnns.ExaDat.ExamDate.YYYYMMDD,
	              Gbl.ExamAnns.HighlightDate))
	    HighLight = true;
        }
      Exa_ShowExamAnnouncement (ExaCod,TypeViewExamAnnouncement,
	                        HighLight);

      /***** Free memory of the exam announcement *****/
      Exa_FreeMemExamAnnouncement ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Button to create a new exam announcement *****/
   if (ICanEdit)
      Exa_PutButtonToCreateNewExamAnnouncement ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/***************** Put icon to create a new exam announcement ****************/
/*****************************************************************************/

static void Exa_PutIconToCreateNewExamAnnouncement (void)
  {
   extern const char *Txt_New_announcement_OF_EXAM;

   Ico_PutContextualIconToAdd (ActEdiExaAnn,NULL,NULL,
			       Txt_New_announcement_OF_EXAM);
  }

/*****************************************************************************/
/**************** Put button to create a new exam announcement ***************/
/*****************************************************************************/

static void Exa_PutButtonToCreateNewExamAnnouncement (void)
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

static long Exa_AddExamAnnouncementToDB (void)
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
				(unsigned) Exa_VISIBLE_EXAM_ANNOUNCEMENT,
				Gbl.ExamAnns.ExaDat.CrsFullName,
				Gbl.ExamAnns.ExaDat.Year,
				Gbl.ExamAnns.ExaDat.Session,
				Gbl.ExamAnns.ExaDat.ExamDate.Year,
				Gbl.ExamAnns.ExaDat.ExamDate.Month,
				Gbl.ExamAnns.ExaDat.ExamDate.Day,
				Gbl.ExamAnns.ExaDat.StartTime.Hour,
				Gbl.ExamAnns.ExaDat.StartTime.Minute,
				Gbl.ExamAnns.ExaDat.Duration.Hour,
				Gbl.ExamAnns.ExaDat.Duration.Minute,
				Gbl.ExamAnns.ExaDat.Place,
				Gbl.ExamAnns.ExaDat.Mode,
				Gbl.ExamAnns.ExaDat.Structure,
				Gbl.ExamAnns.ExaDat.DocRequired,
				Gbl.ExamAnns.ExaDat.MatRequired,
				Gbl.ExamAnns.ExaDat.MatAllowed,
				Gbl.ExamAnns.ExaDat.OtherInfo);

   return ExaCod;
  }

/*****************************************************************************/
/*************** Modify an exam announcement in the database *****************/
/*****************************************************************************/

static void Exa_ModifyExamAnnouncementInDB (long ExaCod)
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
	           Gbl.ExamAnns.ExaDat.CrsFullName,
	           Gbl.ExamAnns.ExaDat.Year,
	           Gbl.ExamAnns.ExaDat.Session,
	           Gbl.ExamAnns.ExaDat.ExamDate.Year,
	           Gbl.ExamAnns.ExaDat.ExamDate.Month,
	           Gbl.ExamAnns.ExaDat.ExamDate.Day,
	           Gbl.ExamAnns.ExaDat.StartTime.Hour,
	           Gbl.ExamAnns.ExaDat.StartTime.Minute,
	           Gbl.ExamAnns.ExaDat.Duration.Hour,
	           Gbl.ExamAnns.ExaDat.Duration.Minute,
	           Gbl.ExamAnns.ExaDat.Place,
	           Gbl.ExamAnns.ExaDat.Mode,
	           Gbl.ExamAnns.ExaDat.Structure,
	           Gbl.ExamAnns.ExaDat.DocRequired,
	           Gbl.ExamAnns.ExaDat.MatRequired,
	           Gbl.ExamAnns.ExaDat.MatAllowed,
	           Gbl.ExamAnns.ExaDat.OtherInfo,
	           ExaCod);
  }

/*****************************************************************************/
/******* Create a list with the dates of all the exam announcements **********/
/*****************************************************************************/

void Exa_CreateListDatesOfExamAnnouncements (void)
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
				   (unsigned) Exa_VISIBLE_EXAM_ANNOUNCEMENT);

      /***** The result of the query may be empty *****/
      Gbl.ExamAnns.Lst = NULL;
      Gbl.ExamAnns.NumExaAnns = 0;
      if (NumExaAnns)
	{
	 /***** Allocate memory for the list *****/
	 if ((Gbl.ExamAnns.Lst = (struct Exa_ExamCodeAndDate *) calloc (NumExaAnns,sizeof (struct Exa_ExamCodeAndDate))) == NULL)
	    Lay_NotEnoughMemoryExit ();

	 /***** Get the dates of the existing exam announcements *****/
	 for (NumExaAnn = 0;
	      NumExaAnn < NumExaAnns;
	      NumExaAnn++)
	   {
	    /***** Get next exam announcement *****/
	    row = mysql_fetch_row (mysql_res);

	    /* Get exam code (row[0]) */
	    Gbl.ExamAnns.Lst[Gbl.ExamAnns.NumExaAnns].ExaCod = Str_ConvertStrCodToLongCod (row[0]);

	    /* Read the date of the exam (row[1]) */
	    if (sscanf (row[1],"%04u-%02u-%02u",
			&Gbl.ExamAnns.Lst[Gbl.ExamAnns.NumExaAnns].ExamDate.Year,
			&Gbl.ExamAnns.Lst[Gbl.ExamAnns.NumExaAnns].ExamDate.Month,
			&Gbl.ExamAnns.Lst[Gbl.ExamAnns.NumExaAnns].ExamDate.Day) != 3)
	       Lay_ShowErrorAndExit ("Wrong date of exam.");

	    /***** Increment number of elements in list *****/
	    Gbl.ExamAnns.NumExaAnns++;
	   }
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/***************** Free list of dates of exam announcements ******************/
/*****************************************************************************/

void Exa_FreeListExamAnnouncements (void)
  {
   if (Gbl.ExamAnns.Lst)
     {
      free (Gbl.ExamAnns.Lst);
      Gbl.ExamAnns.Lst = NULL;
      Gbl.ExamAnns.NumExaAnns = 0;
     }
  }

/*****************************************************************************/
/******** Read the data of an exam announcement from the database ************/
/*****************************************************************************/

static void Exa_GetDataExamAnnouncementFromDB (long ExaCod)
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
   Gbl.ExamAnns.ExaDat.CrsCod = Str_ConvertStrCodToLongCod (row[0]);

   /* Status of the exam announcement (row[1]) */
   if (sscanf (row[1],"%u",&UnsignedNum) != 1)
      Lay_ShowErrorAndExit ("Wrong status.");
   if (UnsignedNum >= Exa_NUM_STATUS)
      Lay_ShowErrorAndExit ("Wrong status.");
   Gbl.ExamAnns.ExaDat.Status = (Exa_ExamAnnouncementStatus_t) UnsignedNum;

   /* Name of the course (row[2]) */
   Str_Copy (Gbl.ExamAnns.ExaDat.CrsFullName,row[2],
             Hie_MAX_BYTES_FULL_NAME);

   /* Year (row[3]) */
   if (sscanf (row[3],"%u",&Gbl.ExamAnns.ExaDat.Year) != 1)
      Lay_ShowErrorAndExit ("Wrong year.");

   /* Exam session (row[4]) */
   Str_Copy (Gbl.ExamAnns.ExaDat.Session,row[4],
             Exa_MAX_BYTES_SESSION);

   /* Date of exam announcement (row[5]) */
   if (sscanf (row[5],"%04u-%02u-%02u %02u:%02u:%02u",
               &Gbl.ExamAnns.ExaDat.CallDate.Year,
               &Gbl.ExamAnns.ExaDat.CallDate.Month,
               &Gbl.ExamAnns.ExaDat.CallDate.Day,
               &Hour,
	       &Minute,
	       &Second) != 6)
      Lay_ShowErrorAndExit ("Wrong date of exam announcement.");

   /* Date of exam (row[6]) */
   if (sscanf (row[6],"%04u-%02u-%02u %02u:%02u:%02u",
   	       &Gbl.ExamAnns.ExaDat.ExamDate.Year,
	       &Gbl.ExamAnns.ExaDat.ExamDate.Month,
	       &Gbl.ExamAnns.ExaDat.ExamDate.Day,
               &Gbl.ExamAnns.ExaDat.StartTime.Hour,
	       &Gbl.ExamAnns.ExaDat.StartTime.Minute,
	       &Second) != 6)
      Lay_ShowErrorAndExit ("Wrong date of exam.");
   snprintf (Gbl.ExamAnns.ExaDat.ExamDate.YYYYMMDD,sizeof (Gbl.ExamAnns.ExaDat.ExamDate.YYYYMMDD),
	     "%04u%02u%02u",
             Gbl.ExamAnns.ExaDat.ExamDate.Year,
	     Gbl.ExamAnns.ExaDat.ExamDate.Month,
	     Gbl.ExamAnns.ExaDat.ExamDate.Day);

   /* Approximate duration (row[7]) */
   if (sscanf (row[7],"%02u:%02u:%02u",&Gbl.ExamAnns.ExaDat.Duration.Hour,&Gbl.ExamAnns.ExaDat.Duration.Minute,&Second) != 3)
      Lay_ShowErrorAndExit ("Wrong duration of exam.");

   /* Place (row[8]) */
   Str_Copy (Gbl.ExamAnns.ExaDat.Place,row[8],
             Cns_MAX_BYTES_TEXT);

   /* Exam mode (row[9]) */
   Str_Copy (Gbl.ExamAnns.ExaDat.Mode,row[9],
             Cns_MAX_BYTES_TEXT);

   /* Structure (row[10]) */
   Str_Copy (Gbl.ExamAnns.ExaDat.Structure,row[10],
             Cns_MAX_BYTES_TEXT);

   /* Documentation required (row[11]) */
   Str_Copy (Gbl.ExamAnns.ExaDat.DocRequired,row[11],
             Cns_MAX_BYTES_TEXT);

   /* Material required (row[12]) */
   Str_Copy (Gbl.ExamAnns.ExaDat.MatRequired,row[12],
             Cns_MAX_BYTES_TEXT);

   /* Material allowed (row[13]) */
   Str_Copy (Gbl.ExamAnns.ExaDat.MatAllowed,row[13],
             Cns_MAX_BYTES_TEXT);

   /* Other information for students (row[14]) */
   Str_Copy (Gbl.ExamAnns.ExaDat.OtherInfo,row[14],
             Cns_MAX_BYTES_TEXT);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Show a form with the data of an exam announcement **************/
/*****************************************************************************/

static void Exa_ShowExamAnnouncement (long ExaCod,
				      Exa_TypeViewExamAnnouncement_t TypeViewExamAnnouncement,
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
   void (*FunctionToDrawContextualIcons) (void);
   const char *HelpLink;
   static const char *ClassExaAnnouncement[Exa_NUM_VIEWS][Exa_NUM_STATUS] =
     {
      [Exa_NORMAL_VIEW][Exa_VISIBLE_EXAM_ANNOUNCEMENT] = "EXA_ANN_VISIBLE",
      [Exa_NORMAL_VIEW][Exa_HIDDEN_EXAM_ANNOUNCEMENT ] = "EXA_ANN_HIDDEN",
      [Exa_NORMAL_VIEW][Exa_DELETED_EXAM_ANNOUNCEMENT] = NULL,	// Not applicable here

      [Exa_PRINT_VIEW ][Exa_VISIBLE_EXAM_ANNOUNCEMENT] = "EXA_ANN_VISIBLE",
      [Exa_PRINT_VIEW ][Exa_HIDDEN_EXAM_ANNOUNCEMENT ] = "EXA_ANN_VISIBLE",
      [Exa_PRINT_VIEW ][Exa_DELETED_EXAM_ANNOUNCEMENT] = NULL,	// Not applicable here

      [Exa_FORM_VIEW  ][Exa_VISIBLE_EXAM_ANNOUNCEMENT] = "EXA_ANN_VISIBLE",
      [Exa_FORM_VIEW  ][Exa_HIDDEN_EXAM_ANNOUNCEMENT ] = "EXA_ANN_VISIBLE",
      [Exa_FORM_VIEW  ][Exa_DELETED_EXAM_ANNOUNCEMENT] = NULL,	// Not applicable here
     };

   /***** Get data of institution of this degree *****/
   Ins.InsCod = Gbl.Hierarchy.Ins.InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);

   /***** Build anchor string *****/
   Frm_SetAnchorStr (ExaCod,&Anchor);

   /***** Begin article *****/
   if (TypeViewExamAnnouncement == Exa_NORMAL_VIEW)
      HTM_ARTICLE_Begin (Anchor);

   /***** Begin box *****/
   Width = "625px";
   Gbl.ExamAnns.Anchor = Anchor;	// Used to put contextual icons
   Gbl.ExamAnns.ExaCod = ExaCod;	// Used to put contextual icons
   FunctionToDrawContextualIcons = TypeViewExamAnnouncement == Exa_NORMAL_VIEW ? Exa_PutIconsExamAnnouncement :
									         NULL;
   HelpLink = TypeViewExamAnnouncement == Exa_FORM_VIEW ? ((ExaCod > 0) ? Hlp_ASSESSMENT_Announcements_edit_announcement :
									  Hlp_ASSESSMENT_Announcements_new_announcement) :
						          NULL;
   if (HighLight)
     {
      /* Show pending alerts */
      Ale_ShowAlerts (Anchor);

      /* Start highlighted box */
      Box_BoxShadowBegin (Width,NULL,FunctionToDrawContextualIcons,HelpLink);
     }
   else	// Don't highlight
      /* Start normal box */
      Box_BoxBegin (Width,NULL,FunctionToDrawContextualIcons,HelpLink,
		    Box_NOT_CLOSABLE);

   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      /***** Begin form *****/
      Frm_StartFormAnchor (ActRcvExaAnn,Anchor);
      if (ExaCod > 0)	// Existing announcement of exam
         Exa_PutHiddenParamExaCod (ExaCod);
     }

   /***** Begin table *****/
   HTM_TABLE_Begin ("%s CELLS_PAD_2",
                    ClassExaAnnouncement[TypeViewExamAnnouncement][Gbl.ExamAnns.ExaDat.Status]);

   /***** Institution logo *****/
   HTM_TR_Begin (NULL);
   HTM_TD_Begin ("colspan=\"2\" class=\"CM\"");
   if (TypeViewExamAnnouncement == Exa_PRINT_VIEW)
      HTM_SPAN_Begin ("class=\"EXAM_TIT\"");
   else
      HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"EXAM_TIT\"",
                   Ins.WWW);
   Lgo_DrawLogo (Hie_INS,Ins.InsCod,Ins.FullName,64,NULL,true);
   HTM_BR ();
   HTM_Txt (Ins.FullName);
   if (TypeViewExamAnnouncement == Exa_PRINT_VIEW)
      HTM_SPAN_End ();
   else
      HTM_A_End ();
   HTM_TD_End ();
   HTM_TR_End ();

   /***** Degree *****/
   HTM_TR_Begin (NULL);
   HTM_TD_Begin ("colspan=\"2\" class=\"EXAM_TIT CM\"");
   if (TypeViewExamAnnouncement == Exa_NORMAL_VIEW)
      HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"EXAM_TIT\"",
                   Gbl.Hierarchy.Deg.WWW);
   HTM_Txt (Gbl.Hierarchy.Deg.FullName);
   if (TypeViewExamAnnouncement == Exa_NORMAL_VIEW)
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
		    TypeViewExamAnnouncement == Exa_FORM_VIEW ? "CrsName" :
			                                        NULL,
		    Txt_EXAM_ANNOUNCEMENT_Course);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LT\"");
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      HTM_INPUT_TEXT ("CrsName",Hie_MAX_CHARS_FULL_NAME,Gbl.ExamAnns.ExaDat.CrsFullName,false,
		      "id=\"CrsName\" size=\"30\"");
   else
     {
      HTM_STRONG_Begin ();
      HTM_Txt (Gbl.ExamAnns.ExaDat.CrsFullName);
      HTM_STRONG_End ();
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Year/semester (N.A., 1º, 2º, 3º, 4º, 5º...) *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == Exa_FORM_VIEW ? "Year" :
			                                        NULL,
		    Txt_EXAM_ANNOUNCEMENT_Year_or_semester);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LT\"");
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      HTM_SELECT_Begin (false,
			"id=\"Year\" name=\"Year\"");
      for (Year = 0;
	   Year <= Deg_MAX_YEARS_PER_DEGREE;
	   Year++)
	 HTM_OPTION (HTM_Type_UNSIGNED,&Year,
		     Gbl.ExamAnns.ExaDat.Year == Year,false,
		     "%s",Txt_YEAR_OF_DEGREE[Year]);
      HTM_SELECT_End ();
     }
   else
      HTM_Txt (Txt_YEAR_OF_DEGREE[Gbl.ExamAnns.ExaDat.Year]);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Exam session *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == Exa_FORM_VIEW ? "ExamSession" :
			                                        NULL,
		    Txt_EXAM_ANNOUNCEMENT_Session);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LT\"");
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      HTM_INPUT_TEXT ("ExamSession",Exa_MAX_CHARS_SESSION,Gbl.ExamAnns.ExaDat.Session,false,
		      "id=\"ExamSession\" size=\"30\"");
   else
      HTM_Txt (Gbl.ExamAnns.ExaDat.Session);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Date of the exam *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_EXAM_ANNOUNCEMENT_Exam_date);

   /* Data */
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      HTM_TD_Begin ("class=\"LT\"");
      Dat_WriteFormDate (Gbl.ExamAnns.ExaDat.ExamDate.Year < Gbl.Now.Date.Year ? Gbl.ExamAnns.ExaDat.ExamDate.Year :
                                                                                 Gbl.Now.Date.Year,
                         Gbl.Now.Date.Year + 1,"Exam",
                         &(Gbl.ExamAnns.ExaDat.ExamDate),
                         false,false);
      HTM_TD_End ();
     }
   else
     {
      Dat_ConvDateToDateStr (&Gbl.ExamAnns.ExaDat.ExamDate,
                             StrExamDate);
      HTM_TD_Begin ("class=\"EXAM LT\"");
      HTM_Txt (StrExamDate);
      HTM_TD_End ();
     }
   HTM_TR_End ();

   /***** Start time *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_EXAM_ANNOUNCEMENT_Start_time);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LT\"");
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      HTM_SELECT_Begin (false,
			"name=\"ExamHour\"");
      HTM_OPTION (HTM_Type_STRING,"0",
		  Gbl.ExamAnns.ExaDat.StartTime.Hour == 0,false,
		  "-");
      for (Hour = 7;
	   Hour <= 22;
	   Hour++)
	 HTM_OPTION (HTM_Type_UNSIGNED,&Hour,
		     Gbl.ExamAnns.ExaDat.StartTime.Hour == Hour,false,
		     "%02u %s",Hour,Txt_hours_ABBREVIATION);
      HTM_SELECT_End ();

      HTM_SELECT_Begin (false,
			"name=\"ExamMinute\"");
      for (Minute = 0;
	   Minute <= 59;
	   Minute++)
	 HTM_OPTION (HTM_Type_UNSIGNED,&Minute,
		     Gbl.ExamAnns.ExaDat.StartTime.Minute == Minute,false,
		     "%02u &prime;",Minute);
      HTM_SELECT_End ();
     }
   else if (Gbl.ExamAnns.ExaDat.StartTime.Hour)
      HTM_TxtF ("%2u:%02u",Gbl.ExamAnns.ExaDat.StartTime.Hour,
                           Gbl.ExamAnns.ExaDat.StartTime.Minute);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Approximate duration of the exam *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_EXAM_ANNOUNCEMENT_Approximate_duration);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LT\"");
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      HTM_SELECT_Begin (false,
			"name=\"DurationHour\"");
      for (Hour = 0;
	   Hour <= 8;
	   Hour++)
	 HTM_OPTION (HTM_Type_UNSIGNED,&Hour,
		     Gbl.ExamAnns.ExaDat.Duration.Hour == Hour,false,
		     "%02u %s",Hour,Txt_hours_ABBREVIATION);
      HTM_SELECT_End ();

      HTM_SELECT_Begin (false,
			"name=\"DurationMinute\"");
      for (Minute = 0;
	   Minute <= 59;
	   Minute++)
	 HTM_OPTION (HTM_Type_UNSIGNED,&Minute,
		     Gbl.ExamAnns.ExaDat.Duration.Minute == Minute,false,
		     "%02u &prime;",Minute);
      HTM_SELECT_End ();
     }
   else if (Gbl.ExamAnns.ExaDat.Duration.Hour ||
            Gbl.ExamAnns.ExaDat.Duration.Minute)
     {
      if (Gbl.ExamAnns.ExaDat.Duration.Hour)
        {
         if (Gbl.ExamAnns.ExaDat.Duration.Minute)
            HTM_TxtF ("%u %s %u &prime;",Gbl.ExamAnns.ExaDat.Duration.Hour,
                                       Txt_hours_ABBREVIATION,
                                       Gbl.ExamAnns.ExaDat.Duration.Minute);
         else
            HTM_TxtF ("%u&nbsp;%s",Gbl.ExamAnns.ExaDat.Duration.Hour,
				   Gbl.ExamAnns.ExaDat.Duration.Hour == 1 ? Txt_hour :
					                                    Txt_hours);
        }
      else if (Gbl.ExamAnns.ExaDat.Duration.Minute)
        {
         HTM_TxtF ("%u&nbsp;%s",Gbl.ExamAnns.ExaDat.Duration.Minute,
			        Gbl.ExamAnns.ExaDat.Duration.Minute == 1 ? Txt_minute :
				                                           Txt_minutes);
        }
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Place where the exam will be made *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == Exa_FORM_VIEW ? "Place" :
			                                        NULL,
		    Txt_EXAM_ANNOUNCEMENT_Place_of_exam);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LT\"");
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      HTM_TEXTAREA_Begin ("id=\"Place\" name=\"Place\" cols=\"40\" rows=\"4\"");
      HTM_Txt (Gbl.ExamAnns.ExaDat.Place);
      HTM_TEXTAREA_End ();
     }
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnns.ExaDat.Place,
                        Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (Gbl.ExamAnns.ExaDat.Place);
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Exam mode *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == Exa_FORM_VIEW ? "ExamMode" :
			                                        NULL,
		    Txt_EXAM_ANNOUNCEMENT_Mode);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LT\"");
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      HTM_TEXTAREA_Begin ("id=\"ExamMode\" name=\"ExamMode\" cols=\"40\" rows=\"2\"");
      HTM_Txt (Gbl.ExamAnns.ExaDat.Mode);
      HTM_TEXTAREA_End ();
     }
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnns.ExaDat.Mode,
                        Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (Gbl.ExamAnns.ExaDat.Mode);
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Structure of the exam *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == Exa_FORM_VIEW ? "Structure" :
			                                        NULL,
		    Txt_EXAM_ANNOUNCEMENT_Structure_of_the_exam);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LT\"");
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      HTM_TEXTAREA_Begin ("id=\"Structure\" name=\"Structure\" cols=\"40\" rows=\"8\"");
      HTM_Txt (Gbl.ExamAnns.ExaDat.Structure);
      HTM_TEXTAREA_End ();
     }
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnns.ExaDat.Structure,
                        Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (Gbl.ExamAnns.ExaDat.Structure);
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Documentation required *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == Exa_FORM_VIEW ? "DocRequired" :
			                                        NULL,
		    Txt_EXAM_ANNOUNCEMENT_Documentation_required);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LT\"");
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      HTM_TEXTAREA_Begin ("id=\"DocRequired\" name=\"DocRequired\" cols=\"40\" rows=\"2\"");
      HTM_Txt (Gbl.ExamAnns.ExaDat.DocRequired);
      HTM_TEXTAREA_End ();
     }
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnns.ExaDat.DocRequired,
                        Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (Gbl.ExamAnns.ExaDat.DocRequired);
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Material required *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == Exa_FORM_VIEW ? "MatRequired" :
			                                        NULL,
		    Txt_EXAM_ANNOUNCEMENT_Material_required);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LT\"");
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      HTM_TEXTAREA_Begin ("id=\"MatRequired\" name=\"MatRequired\" cols=\"40\" rows=\"4\"");
      HTM_Txt (Gbl.ExamAnns.ExaDat.MatRequired);
      HTM_TEXTAREA_End ();
     }
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnns.ExaDat.MatRequired,
                        Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (Gbl.ExamAnns.ExaDat.MatRequired);
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Material allowed *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == Exa_FORM_VIEW ? "MatAllowed" :
			                                        NULL,
		    Txt_EXAM_ANNOUNCEMENT_Material_allowed);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LT\"");
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      HTM_TEXTAREA_Begin ("id=\"MatAllowed\" name=\"MatAllowed\" cols=\"40\" rows=\"4\"");
      HTM_Txt (Gbl.ExamAnns.ExaDat.MatAllowed);
      HTM_TEXTAREA_End ();
     }
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnns.ExaDat.MatAllowed,
                        Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (Gbl.ExamAnns.ExaDat.MatAllowed);
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Other information to students *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",
		    TypeViewExamAnnouncement == Exa_FORM_VIEW ? "OtherInfo" :
			                                        NULL,
		    Txt_EXAM_ANNOUNCEMENT_Other_information);

   /* Data */
   HTM_TD_Begin ("class=\"EXAM LT\"");
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      HTM_TEXTAREA_Begin ("id=\"OtherInfo\" name=\"OtherInfo\" cols=\"40\" rows=\"5\"");
      HTM_Txt (Gbl.ExamAnns.ExaDat.OtherInfo);
      HTM_TEXTAREA_End ();
     }
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnns.ExaDat.OtherInfo,
                        Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (Gbl.ExamAnns.ExaDat.OtherInfo);
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      Box_BoxTableWithButtonEnd ((ExaCod > 0) ? Btn_CONFIRM_BUTTON :
	                                        Btn_CREATE_BUTTON,
	                         Txt_Publish_announcement_OF_EXAM);
   else
      Box_BoxTableEnd ();

   /***** Show QR code *****/
   if (TypeViewExamAnnouncement == Exa_PRINT_VIEW)
      QR_ExamAnnnouncement ();

   /***** End article *****/
   if (TypeViewExamAnnouncement == Exa_NORMAL_VIEW)
      HTM_ARTICLE_End ();

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);
  }

/*****************************************************************************/
/********* Put icons to remove / edit / print an exam announcement ***********/
/*****************************************************************************/

static void Exa_PutIconsExamAnnouncement (void)
  {
   if (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
       Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
     {
      /***** Link to remove this exam announcement *****/
      Ico_PutContextualIconToRemove (ActReqRemExaAnn,Exa_PutParamExaCodToEdit);

      /***** Put form to hide/show exam announement *****/
      switch (Gbl.ExamAnns.ExaDat.Status)
        {
	 case Exa_VISIBLE_EXAM_ANNOUNCEMENT:
            Ico_PutContextualIconToHide (ActHidExaAnn,Gbl.ExamAnns.Anchor,Exa_PutParamExaCodToEdit);
	    break;
	 case Exa_HIDDEN_EXAM_ANNOUNCEMENT:
            Ico_PutContextualIconToUnhide (ActShoExaAnn,Gbl.ExamAnns.Anchor,Exa_PutParamExaCodToEdit);
	    break;
	 case Exa_DELETED_EXAM_ANNOUNCEMENT:	// Not applicable here
	    break;
        }

      /***** Link to edit this exam announcement *****/
      Ico_PutContextualIconToEdit (ActEdiExaAnn,Exa_PutParamExaCodToEdit);
     }

   /***** Link to print view *****/
   Ico_PutContextualIconToPrint (ActPrnExaAnn,Exa_PutParamExaCodToEdit);
  }

/*****************************************************************************/
/*************** Param with the code of an exam announcement *****************/
/*****************************************************************************/

static void Exa_PutParamExaCodToEdit (void)
  {
   Exa_PutHiddenParamExaCod (Gbl.ExamAnns.ExaCod);
  }

void Exa_PutHiddenParamExaCod (long ExaCod)
  {
   Par_PutHiddenParamLong (NULL,"ExaCod",ExaCod);
  }

/*****************************************************************************/
/********** Get parameter with the code of an exam announcement **************/
/*****************************************************************************/

static long Exa_GetParamExaCod (void)
  {
   /* Get notice code */
   return Par_GetParToLong ("ExaCod");
  }

/*****************************************************************************/
/************ Get summary and content about an exam announcement *************/
/*****************************************************************************/

void Exa_GetSummaryAndContentExamAnnouncement (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                               char **ContentStr,
                                               long ExaCod,bool GetContent)
  {
   extern const char *Txt_hours_ABBREVIATION;
   char CrsNameAndDate[Hie_MAX_BYTES_FULL_NAME + (2 + Cns_MAX_BYTES_DATE + 7) + 1];
   char StrExamDate[Cns_MAX_BYTES_DATE + 1];

   /***** Initializations *****/
   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Allocate memory for the exam announcement *****/
   Exa_AllocMemExamAnnouncement ();

   /***** Get data of an exam announcement from database *****/
   Exa_GetDataExamAnnouncementFromDB (ExaCod);

   /***** Content *****/
   if (GetContent)
      Exa_GetNotifContentExamAnnouncement (ContentStr);

   /***** Summary *****/
   /* Name of the course and date of exam */
   Dat_ConvDateToDateStr (&Gbl.ExamAnns.ExaDat.ExamDate,StrExamDate);
   snprintf (CrsNameAndDate,sizeof (CrsNameAndDate),
	     "%s, %s, %2u:%02u",
             Gbl.ExamAnns.ExaDat.CrsFullName,
             StrExamDate,
             Gbl.ExamAnns.ExaDat.StartTime.Hour,
             Gbl.ExamAnns.ExaDat.StartTime.Minute);
   Str_Copy (SummaryStr,CrsNameAndDate,
             Ntf_MAX_BYTES_SUMMARY);

   /***** Free memory of the exam announcement *****/
   Exa_FreeMemExamAnnouncement ();
  }

/*****************************************************************************/
/************ Show a form with the data of an exam announcement **************/
/*****************************************************************************/

static void Exa_GetNotifContentExamAnnouncement (char **ContentStr)
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
   Crs.CrsCod = Gbl.ExamAnns.ExaDat.CrsCod;
   Crs_GetDataOfCourseByCod (&Crs);

   /***** Get data of degree *****/
   Deg.DegCod = Crs.DegCod;
   Deg_GetDataOfDegreeByCod (&Deg);

   /***** Get data of institution *****/
   Ins.InsCod = Deg_GetInsCodOfDegreeByCod (Deg.DegCod);
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);

   /***** Convert struct date to a date string *****/
   Dat_ConvDateToDateStr (&Gbl.ExamAnns.ExaDat.ExamDate,StrExamDate);

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
                 Txt_EXAM_ANNOUNCEMENT_Course,Gbl.ExamAnns.ExaDat.CrsFullName,
                 Txt_EXAM_ANNOUNCEMENT_Year_or_semester,Txt_YEAR_OF_DEGREE[Gbl.ExamAnns.ExaDat.Year],
                 Txt_EXAM_ANNOUNCEMENT_Session,Gbl.ExamAnns.ExaDat.Session,
                 Txt_EXAM_ANNOUNCEMENT_Exam_date,StrExamDate,
                 Txt_EXAM_ANNOUNCEMENT_Start_time,Gbl.ExamAnns.ExaDat.StartTime.Hour,
                                                  Gbl.ExamAnns.ExaDat.StartTime.Minute,
                 Txt_hours_ABBREVIATION,
                 Txt_EXAM_ANNOUNCEMENT_Approximate_duration,Gbl.ExamAnns.ExaDat.Duration.Hour,
                                                            Gbl.ExamAnns.ExaDat.Duration.Minute,
                 Txt_hours_ABBREVIATION,
                 Txt_EXAM_ANNOUNCEMENT_Place_of_exam,Gbl.ExamAnns.ExaDat.Place,
                 Txt_EXAM_ANNOUNCEMENT_Mode,Gbl.ExamAnns.ExaDat.Mode,
                 Txt_EXAM_ANNOUNCEMENT_Structure_of_the_exam,Gbl.ExamAnns.ExaDat.Structure,
                 Txt_EXAM_ANNOUNCEMENT_Documentation_required,Gbl.ExamAnns.ExaDat.DocRequired,
                 Txt_EXAM_ANNOUNCEMENT_Material_required,Gbl.ExamAnns.ExaDat.MatRequired,
                 Txt_EXAM_ANNOUNCEMENT_Material_allowed,Gbl.ExamAnns.ExaDat.MatAllowed,
                 Txt_EXAM_ANNOUNCEMENT_Other_information,Gbl.ExamAnns.ExaDat.OtherInfo) < 0)
      Lay_NotEnoughMemoryExit ();
  }
