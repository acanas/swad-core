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

#include <linux/stddef.h>	// For NULL
#include <stdio.h>		// For sscanf, etc.
#include <stdlib.h>		// For exit, system, malloc, calloc, free, etc.
#include <string.h>		// For string functions

#include "swad_config.h"
#include "swad_database.h"
#include "swad_exam.h"
#include "swad_global.h"
#include "swad_logo.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_RSS.h"
#include "swad_social.h"
#include "swad_string.h"

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
static void Exa_ListExamAnnouncementsEdit (void);
static void Exa_ListExamAnnouncements (Exa_TypeViewExamAnnouncement_t TypeViewExamAnnouncement);
static void Exa_PutIconToCreateNewExamAnnouncement (void);
static void Exa_PutButtonToCreateNewExamAnnouncement (void);

static long Exa_AddExamAnnouncementToDB (void);
static void Exa_ModifyExamAnnouncementInDB (void);
static void Exa_GetDataExamAnnouncementFromDB (void);
static void Exa_ShowExamAnnouncement (Exa_TypeViewExamAnnouncement_t TypeViewExamAnnouncement);
static void Exa_PutIconsExamAnnouncement (void);
static void Exa_PutParamExaCodToEdit (void);
static long Exa_GetParamExaCod (void);

static void Exa_GetNotifContentExamAnnouncement (char **ContentStr);

/*****************************************************************************/
/********************** Form to edit an exam announcement ********************/
/*****************************************************************************/

void Exa_PutFrmEditAExamAnnouncement (void)
  {
   /***** Allocate memory for the exam announcement *****/
   Exa_AllocMemExamAnnouncement ();

   /***** Get the code of the exam announcement *****/
   Gbl.ExamAnns.ExaDat.ExaCod = Exa_GetParamsExamAnnouncement ();

   if (Gbl.ExamAnns.ExaDat.ExaCod > 0)	// -1 indicates that this is a new exam announcement
      /***** Read exam announcement from the database *****/
      Exa_GetDataExamAnnouncementFromDB ();

   /***** Show exam announcement *****/
   Exa_ShowExamAnnouncement (Exa_FORM_VIEW);

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
      Str_Copy (Gbl.ExamAnns.ExaDat.CrsFullName,Gbl.CurrentCrs.Crs.FullName,
                Hie_MAX_BYTES_FULL_NAME);

   /***** Get the year *****/
   Gbl.ExamAnns.ExaDat.Year = (unsigned)
	                      Par_GetParToUnsignedLong ("Year",
                                                        0,	// N.A.
                                                        Deg_MAX_YEARS_PER_DEGREE,
                                                        (unsigned long) Gbl.CurrentCrs.Crs.Year);

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
   if ((Gbl.ExamAnns.ExaDat.Place       = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnns.ExaDat.Mode        = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnns.ExaDat.Structure   = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnns.ExaDat.DocRequired = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnns.ExaDat.MatRequired = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnns.ExaDat.MatAllowed  = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnns.ExaDat.OtherInfo   = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");
  }

/*****************************************************************************/
/* Free memory of those parameters of an exam announcem. with a lot of text **/
/*****************************************************************************/

void Exa_FreeMemExamAnnouncement (void)
  {
   if (Gbl.ExamAnns.ExaDat.Place)
     {
      free ((void *) Gbl.ExamAnns.ExaDat.Place);
      Gbl.ExamAnns.ExaDat.Place = NULL;
     }
   if (Gbl.ExamAnns.ExaDat.Mode)
     {
      free ((void *) Gbl.ExamAnns.ExaDat.Mode);
      Gbl.ExamAnns.ExaDat.Mode = NULL;
     }
   if (Gbl.ExamAnns.ExaDat.Structure)
     {
      free ((void *) Gbl.ExamAnns.ExaDat.Structure);
      Gbl.ExamAnns.ExaDat.Structure = NULL;
     }
   if (Gbl.ExamAnns.ExaDat.DocRequired)
     {
      free ((void *) Gbl.ExamAnns.ExaDat.DocRequired);
      Gbl.ExamAnns.ExaDat.DocRequired = NULL;
     }
   if (Gbl.ExamAnns.ExaDat.MatRequired)
     {
      free ((void *) Gbl.ExamAnns.ExaDat.MatRequired);
      Gbl.ExamAnns.ExaDat.MatRequired = NULL;
     }
   if (Gbl.ExamAnns.ExaDat.MatAllowed)
     {
      free ((void *) Gbl.ExamAnns.ExaDat.MatAllowed);
      Gbl.ExamAnns.ExaDat.MatAllowed = NULL;
     }
   if (Gbl.ExamAnns.ExaDat.OtherInfo)
     {
      free ((void *) Gbl.ExamAnns.ExaDat.OtherInfo);
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
   /***** Allocate memory for the exam announcement *****/
   Exa_AllocMemExamAnnouncement ();

   /***** Get parameters of the exam announcement *****/
   Gbl.ExamAnns.ExaDat.ExaCod = Exa_GetParamsExamAnnouncement ();
   Gbl.ExamAnns.NewExamAnnouncement = (Gbl.ExamAnns.ExaDat.ExaCod < 0);

   /***** Add the exam announcement to the database and read it again from the database *****/
   if (Gbl.ExamAnns.NewExamAnnouncement)
      Gbl.ExamAnns.ExaDat.ExaCod = Exa_AddExamAnnouncementToDB ();
   else
      Exa_ModifyExamAnnouncementInDB ();

   /***** Free memory of the exam announcement *****/
   Exa_FreeMemExamAnnouncement ();
  }

void Exa_ReceiveExamAnnouncement2 (void)
  {
   extern const char *Txt_Created_new_announcement_of_exam;
   extern const char *Txt_The_announcement_of_exam_has_been_successfully_updated;
   unsigned NumUsrsToBeNotifiedByEMail;
   struct SocialPublishing SocPub;

   /***** Show message *****/
   Lay_ShowAlert (Lay_SUCCESS,
                  Gbl.ExamAnns.NewExamAnnouncement ? Txt_Created_new_announcement_of_exam :
                                                              Txt_The_announcement_of_exam_has_been_successfully_updated);

   /***** Notify by email about the new exam announcement *****/
   if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_EXAM_ANNOUNCEMENT,Gbl.ExamAnns.ExaDat.ExaCod)))
      Exa_UpdateNumUsrsNotifiedByEMailAboutExamAnnouncement (Gbl.ExamAnns.ExaDat.ExaCod,NumUsrsToBeNotifiedByEMail);
   Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);

   /***** Create a new social note about the new exam announcement *****/
   Soc_StoreAndPublishSocialNote (Soc_NOTE_EXAM_ANNOUNCEMENT,Gbl.ExamAnns.ExaDat.ExaCod,&SocPub);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.CurrentCrs.Crs);

   /***** Show exam announcement *****/
   Exa_ListExamAnnouncementsEdit ();
  }

/*****************************************************************************/
/***** Update number of users notified in table of exam announcements ********/
/*****************************************************************************/

static void Exa_UpdateNumUsrsNotifiedByEMailAboutExamAnnouncement (long ExaCod,unsigned NumUsrsToBeNotifiedByEMail)
  {
   char Query[256];

   /***** Update number of users notified *****/
   sprintf (Query,"UPDATE exam_announcements SET NumNotif=NumNotif+'%u'"
                  " WHERE ExaCod='%ld'",
            NumUsrsToBeNotifiedByEMail,ExaCod);
   DB_QueryUPDATE (Query,"can not update the number of notifications of an exam announcement");
  }

/*****************************************************************************/
/************************* Print an exam announcement ************************/
/*****************************************************************************/

void Exa_PrintExamAnnouncement (void)
  {
   /***** Allocate memory for the exam announcement *****/
   Exa_AllocMemExamAnnouncement ();

   /***** Get the code of the exam announcement *****/
   if ((Gbl.ExamAnns.ExaDat.ExaCod = Exa_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Read exam announcement from the database *****/
   Exa_GetDataExamAnnouncementFromDB ();

   /***** Show exam announcement *****/
   Exa_ShowExamAnnouncement (Exa_PRINT_VIEW);

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

   /***** Get the code of the exam announcement *****/
   if ((Gbl.ExamAnns.ExaDat.ExaCod = Exa_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Message *****/
   Lay_ShowAlert (Lay_WARNING,Txt_Do_you_really_want_to_remove_the_following_announcement_of_exam);

   /***** View announcement *****/
   Exa_AllocMemExamAnnouncement ();
   Exa_GetDataExamAnnouncementFromDB ();
   Exa_ShowExamAnnouncement (Exa_NORMAL_VIEW);
   Exa_FreeMemExamAnnouncement ();

   /***** Button of confirmation of removing *****/
   Act_FormStart (ActRemExaAnn);
   Exa_PutParamExaCodToEdit ();
   Lay_PutRemoveButton (Txt_Remove);
   Act_FormEnd ();
  }

/*****************************************************************************/
/************************ Remove an exam announcement ************************/
/*****************************************************************************/
// This function is splitted into a-priori and a-posteriori functions
// in order to view updated links in month of left column

void Exa_RemoveExamAnnouncement1 (void)
  {
   char Query[256];
   long ExaCod;

   /***** Get the code of the exam announcement *****/
   if ((ExaCod = Exa_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Mark the exam announcement as deleted in the database *****/
   sprintf (Query,"UPDATE exam_announcements SET Status='%u'"
                  " WHERE ExaCod='%ld' AND CrsCod='%ld'",
            (unsigned) Exa_DELETED_EXAM_ANNOUNCEMENT,
            ExaCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not remove exam announcement");

   /***** Mark possible notifications as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_EXAM_ANNOUNCEMENT,ExaCod);

   /***** Mark possible social note as unavailable *****/
   Soc_MarkSocialNoteAsUnavailableUsingNoteTypeAndCod (Soc_NOTE_EXAM_ANNOUNCEMENT,ExaCod);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.CurrentCrs.Crs);
  }

void Exa_RemoveExamAnnouncement2 (void)
  {
   extern const char *Txt_Announcement_of_exam_removed;

   /***** Write message *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_Announcement_of_exam_removed);

   /***** List again all the remaining exam announcements *****/
   Exa_ListExamAnnouncementsEdit ();
  }

/*****************************************************************************/
/************************ Hide an exam announcement **************************/
/*****************************************************************************/
// This function is splitted into a-priori and a-posteriori functions
// in order to view updated links in month of left column

void Exa_HideExamAnnouncement1 (void)
  {
   char Query[256];
   long ExaCod;

   /***** Get the code of the exam announcement *****/
   if ((ExaCod = Exa_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Mark the exam announcement as hidden in the database *****/
   sprintf (Query,"UPDATE exam_announcements SET Status='%u'"
                  " WHERE ExaCod='%ld' AND CrsCod='%ld'",
            (unsigned) Exa_HIDDEN_EXAM_ANNOUNCEMENT,
            ExaCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not hide exam announcement");
  }

void Exa_HideExamAnnouncement2 (void)
  {
   extern const char *Txt_The_announcement_of_exam_is_now_hidden;

   /***** Write message to show the change made *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_The_announcement_of_exam_is_now_hidden);

   /***** Show exam announcements again *****/
   Exa_ListExamAnnouncementsEdit ();
  }

/*****************************************************************************/
/************************ Unhide an exam announcement ************************/
/*****************************************************************************/
// This function is splitted into a-priori and a-posteriori functions
// in order to view updated links in month of left column

void Exa_UnhideExamAnnouncement1 (void)
  {
   char Query[256];
   long ExaCod;

   /***** Get the code of the exam announcement *****/
   if ((ExaCod = Exa_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Mark the exam announcement as visible in the database *****/
   sprintf (Query,"UPDATE exam_announcements SET Status='%u'"
                  " WHERE ExaCod='%ld' AND CrsCod='%ld'",
            (unsigned) Exa_VISIBLE_EXAM_ANNOUNCEMENT,
            ExaCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not unhide exam announcement");
  }

void Exa_UnhideExamAnnouncement2 (void)
  {
   extern const char *Txt_The_announcement_of_exam_is_now_visible;

   /***** Write message to show the change made *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_The_announcement_of_exam_is_now_visible);

   /***** Show exam announcements again *****/
   Exa_ListExamAnnouncementsEdit ();
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
	                -1L,Gbl.CurrentCrs.Crs.CrsCod,
	                Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********** List all the exam announcements to edit or remove them ***********/
/*****************************************************************************/

static void Exa_ListExamAnnouncementsEdit (void)
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
   /***** Get the date (in YYYY-MM-DD format)
          of the exam announcements to highlight *****/
   Par_GetParToText ("Date",Gbl.ExamAnns.HighlightDate,4 + 1 + 2 + 1 + 2);
  }

/*****************************************************************************/
/******************** List all the exam announcements ************************/
/*****************************************************************************/

static void Exa_ListExamAnnouncements (Exa_TypeViewExamAnnouncement_t TypeViewExamAnnouncement)
  {
   extern const char *Hlp_ASSESSMENT_Announcements;
   extern const char *Txt_All_announcements_of_exams;
   extern const char *Txt_Announcements_of_exams;
   extern const char *Txt_No_announcements_of_exams_of_X;
   char Query[512];
   char SubQueryStatus[64];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumExaAnn;
   unsigned long NumExaAnns;
   bool ICanEdit = (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER ||
		    Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);

   /***** Build subquery about status depending on my role *****/
   if (ICanEdit)
      sprintf (SubQueryStatus,"Status<>'%u'",
	       (unsigned) Exa_DELETED_EXAM_ANNOUNCEMENT);
   else
      sprintf (SubQueryStatus,"Status='%u'",
	       (unsigned) Exa_VISIBLE_EXAM_ANNOUNCEMENT);

   /***** Show one highlighted exam announcement *****/
   if (Gbl.ExamAnns.HighlightExaCod > 0)
     {
      /***** Get one exam announcement from database *****/
      sprintf (Query,"SELECT ExaCod"
		     " FROM exam_announcements"
		     " WHERE ExaCod='%ld'"
		     " AND CrsCod='%ld' AND %s",
	       Gbl.ExamAnns.HighlightExaCod,
	       Gbl.CurrentCrs.Crs.CrsCod,SubQueryStatus);
      NumExaAnns = DB_QuerySELECT (Query,&mysql_res,"can not get exam announcements in this course for listing");

      /***** List the existing exam announcements *****/
      for (NumExaAnn = 0;
	   NumExaAnn < NumExaAnns;
	   NumExaAnn++)
	{
	 /***** Get the code of the exam announcement (row[0]) *****/
	 row = mysql_fetch_row (mysql_res);

	 if (sscanf (row[0],"%ld",&Gbl.ExamAnns.ExaDat.ExaCod) != 1)
	    Lay_ShowErrorAndExit ("Wrong code of exam announcement.");

	 /***** Allocate memory for the exam announcement *****/
	 Exa_AllocMemExamAnnouncement ();

	 /***** Read the data of the exam announcement *****/
	 Exa_GetDataExamAnnouncementFromDB ();

	 /***** Show exam announcement *****/
	 Exa_ShowExamAnnouncement (TypeViewExamAnnouncement);

	 /***** Free memory of the exam announcement *****/
	 Exa_FreeMemExamAnnouncement ();
	}
     }

   /***** Show highlighted exam announcements of a date *****/
   if (Gbl.ExamAnns.HighlightDate[0])
     {
      /***** Get exam announcements (the most recent first)
	     in current course for a date from database *****/
      sprintf (Query,"SELECT ExaCod"
		     " FROM exam_announcements"
		     " WHERE CrsCod='%ld' AND %s"
		     " AND DATE(ExamDate)='%s'"
		     " ORDER BY ExamDate DESC",
	       Gbl.CurrentCrs.Crs.CrsCod,SubQueryStatus,
	       Gbl.ExamAnns.HighlightDate);
      NumExaAnns = DB_QuerySELECT (Query,&mysql_res,"can not get exam announcements in this course for listing");

      /***** List the existing exam announcements *****/
      for (NumExaAnn = 0;
	   NumExaAnn < NumExaAnns;
	   NumExaAnn++)
	{
	 /***** Get the code of the exam announcement (row[0]) *****/
	 row = mysql_fetch_row (mysql_res);

	 if (sscanf (row[0],"%ld",&Gbl.ExamAnns.ExaDat.ExaCod) != 1)
	    Lay_ShowErrorAndExit ("Wrong code of exam announcement.");

	 /***** Allocate memory for the exam announcement *****/
	 Exa_AllocMemExamAnnouncement ();

	 /***** Read the data of the exam announcement *****/
	 Exa_GetDataExamAnnouncementFromDB ();

	 /***** Show exam announcement *****/
	 Exa_ShowExamAnnouncement (TypeViewExamAnnouncement);

	 /***** Free memory of the exam announcement *****/
	 Exa_FreeMemExamAnnouncement ();
	}
     }

   /***** Get exam announcements (the most recent first)
          in current course from database *****/
   sprintf (Query,"SELECT ExaCod"
	          " FROM exam_announcements"
                  " WHERE CrsCod='%ld' AND %s"
                  " ORDER BY ExamDate DESC",
            Gbl.CurrentCrs.Crs.CrsCod,SubQueryStatus);
   NumExaAnns = DB_QuerySELECT (Query,&mysql_res,"can not get exam announcements in this course for listing");

   /***** Start frame *****/
   Lay_StartRoundFrame (NULL,
                        (Gbl.ExamAnns.HighlightExaCod > 0 ||
			 Gbl.ExamAnns.HighlightDate[0]) ? Txt_All_announcements_of_exams :
								   Txt_Announcements_of_exams,
			ICanEdit ? Exa_PutIconToCreateNewExamAnnouncement :
				   NULL,
		        Hlp_ASSESSMENT_Announcements);

   /***** The result of the query may be empty *****/
   if (!NumExaAnns)
     {
      sprintf (Gbl.Message,Txt_No_announcements_of_exams_of_X,
               Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);
     }

   /***** List the existing exam announcements *****/
   for (NumExaAnn = 0;
	NumExaAnn < NumExaAnns;
	NumExaAnn++)
     {
      /***** Get the code of the exam announcement (row[0]) *****/
      row = mysql_fetch_row (mysql_res);

      if (sscanf (row[0],"%ld",&Gbl.ExamAnns.ExaDat.ExaCod) != 1)
         Lay_ShowErrorAndExit ("Wrong code of exam announcement.");

      /***** Allocate memory for the exam announcement *****/
      Exa_AllocMemExamAnnouncement ();

      /***** Read the data of the exam announcement *****/
      Exa_GetDataExamAnnouncementFromDB ();

      /***** Show exam announcement *****/
      Exa_ShowExamAnnouncement (TypeViewExamAnnouncement);

      /***** Free memory of the exam announcement *****/
      Exa_FreeMemExamAnnouncement ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Button to create a new assignment *****/
   if (ICanEdit)
      Exa_PutButtonToCreateNewExamAnnouncement ();

   /***** End frame *****/
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/***************** Put icon to create a new exam announcement ****************/
/*****************************************************************************/

static void Exa_PutIconToCreateNewExamAnnouncement (void)
  {
   extern const char *Txt_New_announcement_OF_EXAM;

   Lay_PutContextualLink (ActEdiExaAnn,NULL,
                          "plus64x64.png",
                          Txt_New_announcement_OF_EXAM,NULL,
                          NULL);
  }

/*****************************************************************************/
/**************** Put button to create a new exam announcement ***************/
/*****************************************************************************/

static void Exa_PutButtonToCreateNewExamAnnouncement (void)
  {
   extern const char *Txt_New_announcement_OF_EXAM;

   Act_FormStart (ActEdiExaAnn);
   Lay_PutConfirmButton (Txt_New_announcement_OF_EXAM);
   Act_FormEnd ();
  }

/*****************************************************************************/
/****************** Add an exam announcement to the database *****************/
/*****************************************************************************/
// Return the code of the exam announcement just added

static long Exa_AddExamAnnouncementToDB (void)
  {
   char *Query;
   long ExaCod;

   /***** Add exam announcement *****/
   if ((Query = malloc (512 +
                        Hie_MAX_BYTES_FULL_NAME +
                        Exa_MAX_BYTES_SESSION +
                        7 * Cns_MAX_BYTES_TEXT)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to query database.");
   sprintf (Query,"INSERT INTO exam_announcements "
	          "(CrsCod,Status,NumNotif,CrsFullName,Year,ExamSession,"
	          "CallDate,ExamDate,Duration,"
	          "Place,ExamMode,Structure,DocRequired,MatRequired,MatAllowed,OtherInfo)"
                  " VALUES "
                  "('%ld','%u','0','%s','%u','%s',"
                  "NOW(),'%04u-%02u-%02u %02u:%02u:00','%02u:%02u:00','%s',"
                  "'%s','%s','%s','%s','%s','%s')",
            Gbl.CurrentCrs.Crs.CrsCod,
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
   ExaCod = DB_QueryINSERTandReturnCode (Query,"can not create a new exam announcement");
   free ((void *) Query);

   return ExaCod;
  }

/*****************************************************************************/
/*************** Modify an exam announcement in the database *****************/
/*****************************************************************************/

static void Exa_ModifyExamAnnouncementInDB (void)
  {
   char *Query;

   /***** Modify exam announcement *****/
   if ((Query = malloc (512 +
                        Hie_MAX_BYTES_FULL_NAME +
                        Exa_MAX_BYTES_SESSION +
                        7 * Cns_MAX_BYTES_TEXT)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to query database.");
   sprintf (Query,"UPDATE exam_announcements"
                  " SET CrsFullName='%s',Year='%u',ExamSession='%s',"
                  "ExamDate='%04u-%02u-%02u %02u:%02u:00',"
                  "Duration='%02u:%02u:00',"
                  "Place='%s',ExamMode='%s',Structure='%s',"
                  "DocRequired='%s',MatRequired='%s',MatAllowed='%s',OtherInfo='%s'"
                  " WHERE ExaCod='%ld'",
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
            Gbl.ExamAnns.ExaDat.ExaCod);
   DB_QueryUPDATE (Query,"can not update an exam announcement");
   free ((void *) Query);
  }

/*****************************************************************************/
/******* Create a list with the dates of all the exam announcements **********/
/*****************************************************************************/

void Exa_CreateListDatesOfExamAnnouncements (void)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumExaAnn;
   unsigned long NumExaAnns;

   if (Gbl.DB.DatabaseIsOpen)
     {
      /***** Get exam dates (no matter in what order)
             of visible exam announcements
             in current course from database *****/
      sprintf (Query,"SELECT DISTINCT(DATE(ExamDate))"
	             " FROM exam_announcements"
		     " WHERE CrsCod='%ld' AND Status='%u'",
	       Gbl.CurrentCrs.Crs.CrsCod,
	       (unsigned) Exa_VISIBLE_EXAM_ANNOUNCEMENT);
      NumExaAnns = DB_QuerySELECT (Query,&mysql_res,"can not get exam announcements in this course");

      /***** The result of the query may be empty *****/
      Gbl.ExamAnns.Lst = NULL;
      Gbl.ExamAnns.NumExaAnns = 0;
      if (NumExaAnns)
	{
	 /***** Allocate memory for the list *****/
	 if ((Gbl.ExamAnns.Lst = (struct Date *) calloc (NumExaAnns,sizeof (struct Date))) == NULL)
	    Lay_ShowErrorAndExit ("Not enough memory to store dates of exam announcements.");

	 /***** Get the dates of the existing exam announcements *****/
	 for (NumExaAnn = 0;
	      NumExaAnn < NumExaAnns;
	      NumExaAnn++)
	   {
	    /***** Get next exam announcement *****/
	    row = mysql_fetch_row (mysql_res);

	    /* Read the date of the exam (row[0]) */
	    if (sscanf (row[0],"%04u-%02u-%02u",
			&Gbl.ExamAnns.Lst[Gbl.ExamAnns.NumExaAnns].Year,
			&Gbl.ExamAnns.Lst[Gbl.ExamAnns.NumExaAnns].Month,
			&Gbl.ExamAnns.Lst[Gbl.ExamAnns.NumExaAnns].Day) != 3)
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
      free ((void *) Gbl.ExamAnns.Lst);
      Gbl.ExamAnns.Lst = NULL;
      Gbl.ExamAnns.NumExaAnns = 0;
     }
  }

/*****************************************************************************/
/******** Read the data of an exam announcement from the database ************/
/*****************************************************************************/

static void Exa_GetDataExamAnnouncementFromDB (void)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumExaAnns;
   unsigned UnsignedNum;
   unsigned Hour;
   unsigned Minute;
   unsigned Second;

   /***** Get data of an exam announcement from database *****/
   sprintf (Query,"SELECT CrsCod,Status,CrsFullName,Year,ExamSession,"
	          "CallDate,ExamDate,Duration,Place,ExamMode,"
	          "Structure,DocRequired,MatRequired,MatAllowed,OtherInfo"
                  " FROM exam_announcements WHERE ExaCod='%ld'",
           Gbl.ExamAnns.ExaDat.ExaCod);
   NumExaAnns = DB_QuerySELECT (Query,&mysql_res,"can not get data of an exam announcement");

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
               &Hour,&Minute,&Second) != 6)
      Lay_ShowErrorAndExit ("Wrong date of exam announcement.");

   /* Date of exam (row[6]) */
   if (sscanf (row[6],"%04u-%02u-%02u %02u:%02u:%02u",
   	       &Gbl.ExamAnns.ExaDat.ExamDate.Year,&Gbl.ExamAnns.ExaDat.ExamDate.Month,&Gbl.ExamAnns.ExaDat.ExamDate.Day,
               &Gbl.ExamAnns.ExaDat.StartTime.Hour,&Gbl.ExamAnns.ExaDat.StartTime.Minute,&Second) != 6)
      Lay_ShowErrorAndExit ("Wrong date of exam.");

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

static void Exa_ShowExamAnnouncement (Exa_TypeViewExamAnnouncement_t TypeViewExamAnnouncement)
  {
   extern const char *Hlp_ASSESSMENT_Announcements_new_announcement;
   extern const char *Hlp_ASSESSMENT_Announcements_edit_announcement;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *The_ClassForm[The_NUM_THEMES];
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
   const char *StyleTitle  = "CONV_TIT";
   const char *StyleForm   = "CONV_NEG";
   const char *StyleNormal = "CONV";
   struct Instit Ins;
   char StrExamDate[Cns_MAX_BYTES_DATE + 1];
   unsigned Year,Hour,Minute;
   const char *ClassExaAnnouncement[Exa_NUM_VIEWS][Exa_NUM_STATUS] =
     {
	{	// Exa_NORMAL_VIEW
	 "EXA_ANN_VISIBLE",	// Exa_VISIBLE_EXAM_ANNOUNCEMENT
	 "EXA_ANN_HIDDEN",	// Exa_HIDDEN_EXAM_ANNOUNCEMENT
	 NULL,			// Exa_DELETED_EXAM_ANNOUNCEMENT, Not applicable here
	},
	{	// Exa_PRINT_VIEW
	 "EXA_ANN_VISIBLE",	// Exa_VISIBLE_EXAM_ANNOUNCEMENT
	 "EXA_ANN_VISIBLE",	// Exa_HIDDEN_EXAM_ANNOUNCEMENT
	 NULL,			// Exa_DELETED_EXAM_ANNOUNCEMENT, Not applicable here
	},
	{	// Exa_FORM_VIEW
	 "EXA_ANN_VISIBLE",	// Exa_VISIBLE_EXAM_ANNOUNCEMENT
	 "EXA_ANN_VISIBLE",	// Exa_HIDDEN_EXAM_ANNOUNCEMENT
	 NULL,			// Exa_DELETED_EXAM_ANNOUNCEMENT, Not applicable here
	},
     };

   /***** Get data of institution of this degree *****/
   Ins.InsCod = Gbl.CurrentIns.Ins.InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);

   switch (TypeViewExamAnnouncement)
     {
      case Exa_NORMAL_VIEW:
	 break;
      case Exa_PRINT_VIEW:
         StyleTitle  = "CONV_TIT_IMPR";
         StyleForm   = "CONV_NEG_IMPR";
         StyleNormal = "CONV_IMPR";
         break;
      case Exa_FORM_VIEW:
         StyleForm = The_ClassForm[Gbl.Prefs.Theme];
         break;
     }

   /***** Start frame *****/
   Lay_StartRoundFrame ("625px",NULL,
                        TypeViewExamAnnouncement == Exa_NORMAL_VIEW ? Exa_PutIconsExamAnnouncement :
                                                                      NULL,
                        TypeViewExamAnnouncement == Exa_FORM_VIEW ? ((Gbl.ExamAnns.ExaDat.ExaCod > 0) ? Hlp_ASSESSMENT_Announcements_edit_announcement :
                                                                                                                 Hlp_ASSESSMENT_Announcements_new_announcement) :
                                                                    NULL);

   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      /***** Start form *****/
      Act_FormStart (ActRcvExaAnn);
      if (Gbl.ExamAnns.ExaDat.ExaCod > 0)	// Existing announcement of exam
         Exa_PutParamExaCodToEdit ();
     }

   /***** Start table *****/
   fprintf (Gbl.F.Out,"<table class=\"%s CELLS_PAD_2\">",
            ClassExaAnnouncement[TypeViewExamAnnouncement][Gbl.ExamAnns.ExaDat.Status]);

   /***** Institution logo *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" class=\"CENTER_MIDDLE\">");
   if (TypeViewExamAnnouncement == Exa_PRINT_VIEW)
      fprintf (Gbl.F.Out,"<span class=\"%s\">",StyleTitle);
   else
      fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"%s\">",
               Ins.WWW,StyleTitle);
   Log_DrawLogo (Sco_SCOPE_INS,Ins.InsCod,Ins.FullName,64,NULL,true);
   fprintf (Gbl.F.Out,"<br />%s%s"
                      "</td>"
	              "</tr>",
            Ins.FullName,
            TypeViewExamAnnouncement == Exa_PRINT_VIEW ? "</span>" :
 	                                                 "</a>");

   /***** Degree *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" class=\"%s CENTER_MIDDLE\">",
	    StyleTitle);
   if (TypeViewExamAnnouncement == Exa_NORMAL_VIEW)
      fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"%s\">",
               Gbl.CurrentDeg.Deg.WWW,StyleTitle);
   fprintf (Gbl.F.Out,"%s",Gbl.CurrentDeg.Deg.FullName);
   if (TypeViewExamAnnouncement == Exa_NORMAL_VIEW)
      fprintf (Gbl.F.Out,"</a>");
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Title *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" class=\"%s CENTER_MIDDLE\">"
	              "&nbsp;<br />"
	              "<strong>%s</strong>"
	              "</td>"
	              "</tr>",
            StyleNormal,Txt_EXAM_ANNOUNCEMENT);
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" class=\"%s LEFT_MIDDLE\">"
	              "&nbsp;"
	              "</td>"
	              "</tr>",
	    StyleNormal);

   /***** Name of the course *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_BOTTOM\">"
	              "<label for=\"CrsName\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"%s LEFT_BOTTOM\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Course,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      fprintf (Gbl.F.Out,"<input type=\"text\" id=\"CrsName\" name=\"CrsName\""
	                 " size=\"30\" maxlength=\"%u\" value=\"%s\" />",
               Hie_MAX_CHARS_FULL_NAME,Gbl.ExamAnns.ExaDat.CrsFullName);
     }
   else
      fprintf (Gbl.F.Out,"<strong>%s</strong>",
               Gbl.ExamAnns.ExaDat.CrsFullName);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Year/semester (N.A., 1º, 2º, 3º, 4º, 5º...) *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_BOTTOM\">"
	              "<label for=\"Year\" class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"%s LEFT_BOTTOM\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Year_or_semester,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      fprintf (Gbl.F.Out,"<select id=\"Year\" name=\"Year\">");
      for (Year = 0;
	   Year <= Deg_MAX_YEARS_PER_DEGREE;
	   Year++)
        {
	 fprintf (Gbl.F.Out,"<option");
	 if (Gbl.ExamAnns.ExaDat.Year == Year)
            fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out," value=\"%u\">"
	                    "%s"
	                    "</option>",
	          Year,Txt_YEAR_OF_DEGREE[Year]);
	}
      fprintf (Gbl.F.Out,"</select>");
     }
   else
      fprintf (Gbl.F.Out,"%s",
               Txt_YEAR_OF_DEGREE[Gbl.ExamAnns.ExaDat.Year]);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Exam session *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_BOTTOM\">"
	              "<label for=\"ExamSession\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"%s LEFT_BOTTOM\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Session,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<input type=\"text\""
	                 " id=\"ExamSession\" name=\"ExamSession\""
	                 " size=\"30\" maxlength=\"%u\" value=\"%s\" />",
               Exa_MAX_CHARS_SESSION,Gbl.ExamAnns.ExaDat.Session);
   else
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnns.ExaDat.Session);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Date of the exam *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_BOTTOM\">"
	              "%s:"
	              "</td>",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Exam_date);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      fprintf (Gbl.F.Out,"<td class=\"LEFT_BOTTOM\">");
      Dat_WriteFormDate (Gbl.ExamAnns.ExaDat.ExamDate.Year < Gbl.Now.Date.Year ? Gbl.ExamAnns.ExaDat.ExamDate.Year :
                                                                                          Gbl.Now.Date.Year,
                         Gbl.Now.Date.Year + 1,"Exam",
                         &(Gbl.ExamAnns.ExaDat.ExamDate),
                         false,false);
      fprintf (Gbl.F.Out,"</td>");
     }
   else
     {
      Dat_ConvDateToDateStr (&Gbl.ExamAnns.ExaDat.ExamDate,
                             StrExamDate);
      fprintf (Gbl.F.Out,"<td class=\"%s LEFT_BOTTOM\">"
	                 "%s"
	                 "</td>",
               StyleNormal,StrExamDate);
     }
   fprintf (Gbl.F.Out,"</tr>");

   /***** Start time *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_BOTTOM\">"
	              "%s:"
	              "</td>"
                      "<td class=\"%s LEFT_BOTTOM\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Start_time,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      fprintf (Gbl.F.Out,"<select name=\"ExamHour\">"
	                 "<option value=\"0\"");
      if (Gbl.ExamAnns.ExaDat.StartTime.Hour == 0)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">-</option>");
      for (Hour = 7;
	   Hour <= 22;
	   Hour++)
        {
	 fprintf (Gbl.F.Out,"<option value=\"%u\"",Hour);
	 if (Gbl.ExamAnns.ExaDat.StartTime.Hour == Hour)
            fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out,">%02u %s</option>",
                  Hour,Txt_hours_ABBREVIATION);
	}
      fprintf (Gbl.F.Out,"</select>"
	                 "<select name=\"ExamMinute\">");
      for (Minute = 0;
	   Minute <= 59;
	   Minute++)
        {
	 fprintf (Gbl.F.Out,"<option value=\"%u\"",Minute);
	 if (Gbl.ExamAnns.ExaDat.StartTime.Minute == Minute)
            fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out,">%02u &#39;</option>",Minute);
	}
      fprintf (Gbl.F.Out,"</select>");
     }
   else if (Gbl.ExamAnns.ExaDat.StartTime.Hour)
      fprintf (Gbl.F.Out,"%2u:%02u",
               Gbl.ExamAnns.ExaDat.StartTime.Hour,
               Gbl.ExamAnns.ExaDat.StartTime.Minute);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Approximate duration of the exam *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_BOTTOM\">"
	              "%s:"
	              "</td>"
                      "<td class=\"%s LEFT_BOTTOM\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Approximate_duration,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      fprintf (Gbl.F.Out,"<select name=\"DurationHour\">");
      for (Hour = 0;
	   Hour <= 8;
	   Hour++)
        {
	 fprintf (Gbl.F.Out,"<option value=\"%u\"",Hour);
	 if (Gbl.ExamAnns.ExaDat.Duration.Hour == Hour)
            fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out,">%02u %s</option>",
                  Hour,Txt_hours_ABBREVIATION);
	}
      fprintf (Gbl.F.Out,"</select>"
	                 "<select name=\"DurationMinute\">");
      for (Minute = 0;
	   Minute <= 59;
	   Minute++)
        {
	 fprintf (Gbl.F.Out,"<option value=\"%u\"",Minute);
	 if (Gbl.ExamAnns.ExaDat.Duration.Minute == Minute)
            fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out,">%02u &#39;</option>",Minute);
	}
      fprintf (Gbl.F.Out,"</select>");
     }
   else if (Gbl.ExamAnns.ExaDat.Duration.Hour ||
            Gbl.ExamAnns.ExaDat.Duration.Minute)
     {
      if (Gbl.ExamAnns.ExaDat.Duration.Hour)
        {
         if (Gbl.ExamAnns.ExaDat.Duration.Minute)
            fprintf (Gbl.F.Out,"%u %s %u &#39;",
                     Gbl.ExamAnns.ExaDat.Duration.Hour,
                     Txt_hours_ABBREVIATION,
                     Gbl.ExamAnns.ExaDat.Duration.Minute);
         else
           {
            if (Gbl.ExamAnns.ExaDat.Duration.Hour == 1)
               fprintf (Gbl.F.Out,"1 %s",Txt_hour);
            else
               fprintf (Gbl.F.Out,"%u %s",
                        Gbl.ExamAnns.ExaDat.Duration.Hour,Txt_hours);
           }
        }
      else if (Gbl.ExamAnns.ExaDat.Duration.Minute)
        {
         if (Gbl.ExamAnns.ExaDat.Duration.Minute == 1)
            fprintf (Gbl.F.Out,"1 %s",Txt_minute);
         else
            fprintf (Gbl.F.Out,"%u %s",
                     Gbl.ExamAnns.ExaDat.Duration.Minute,Txt_minutes);
        }
     }
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Place where the exam will be made *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_TOP\">"
	              "<label for=\"Place\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Place_of_exam,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea id=\"Place\" name=\"Place\""
	                 " cols=\"40\" rows=\"4\">"
	                 "%s"
	                 "</textarea>",
               Gbl.ExamAnns.ExaDat.Place);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnns.ExaDat.Place,
                        Cns_MAX_BYTES_TEXT,false);
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnns.ExaDat.Place);
     }
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Exam mode *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_TOP\">"
	              "<label for=\"ExamMode\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Mode,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea id=\"ExamMode\" name=\"ExamMode\""
	                 " cols=\"40\" rows=\"2\">"
	                 "%s"
	                 "</textarea>",
               Gbl.ExamAnns.ExaDat.Mode);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnns.ExaDat.Mode,
                        Cns_MAX_BYTES_TEXT,false);
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnns.ExaDat.Mode);
     }
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Structure of the exam *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_TOP\">"
	              "<label for=\"Structure\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Structure_of_the_exam,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea id=\"Structure\" name=\"Structure\""
	                 " cols=\"40\" rows=\"8\">"
	                 "%s"
	                 "</textarea>",
               Gbl.ExamAnns.ExaDat.Structure);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnns.ExaDat.Structure,
                        Cns_MAX_BYTES_TEXT,false);
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnns.ExaDat.Structure);
     }
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Documentation required *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_TOP\">"
	              "<label for=\"DocRequired\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Documentation_required,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea id=\"DocRequired\" name=\"DocRequired\""
	                 " cols=\"40\" rows=\"2\">"
	                 "%s"
	                 "</textarea>",
               Gbl.ExamAnns.ExaDat.DocRequired);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnns.ExaDat.DocRequired,
                        Cns_MAX_BYTES_TEXT,false);
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnns.ExaDat.DocRequired);
     }
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Material required *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_TOP\">"
	              "<label for=\"MatRequired\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Material_required,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea id=\"MatRequired\" name=\"MatRequired\""
	                 " cols=\"40\" rows=\"4\">"
	                 "%s"
	                 "</textarea>",
               Gbl.ExamAnns.ExaDat.MatRequired);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnns.ExaDat.MatRequired,
                        Cns_MAX_BYTES_TEXT,false);
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnns.ExaDat.MatRequired);
     }
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Material allowed *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_TOP\">"
	              "<label for=\"MatAllowed\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Material_allowed,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea id=\"MatAllowed\" name=\"MatAllowed\""
	                 " cols=\"40\" rows=\"4\">"
	                 "%s"
	                 "</textarea>",
               Gbl.ExamAnns.ExaDat.MatAllowed);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnns.ExaDat.MatAllowed,
                        Cns_MAX_BYTES_TEXT,false);
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnns.ExaDat.MatAllowed);
     }
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Other information to students *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_TOP\">"
	              "<label for=\"OtherInfo\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,Txt_EXAM_ANNOUNCEMENT_Other_information,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea id=\"OtherInfo\" name=\"OtherInfo\""
	                 " cols=\"40\" rows=\"5\">"
	                 "%s"
	                 "</textarea>",
               Gbl.ExamAnns.ExaDat.OtherInfo);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnns.ExaDat.OtherInfo,
                        Cns_MAX_BYTES_TEXT,false);
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnns.ExaDat.OtherInfo);
     }
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** End table *****/
   fprintf (Gbl.F.Out,"</table>");

   /***** End frame *****/
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      Lay_EndRoundFrameWithButton ((Gbl.ExamAnns.ExaDat.ExaCod > 0) ? Lay_CONFIRM_BUTTON :
	                                                                       Lay_CREATE_BUTTON,
	                           Txt_Publish_announcement_OF_EXAM);
   else
      Lay_EndRoundFrame ();

   if (TypeViewExamAnnouncement == Exa_PRINT_VIEW)
      QR_ExamAnnnouncement ();
  }

/*****************************************************************************/
/********* Put icons to remove / edit / print an exam announcement ***********/
/*****************************************************************************/

static void Exa_PutIconsExamAnnouncement (void)
  {
   extern const char *Txt_Remove;
   extern const char *Txt_Show;
   extern const char *Txt_Hide;
   extern const char *Txt_Edit;
   extern const char *Txt_Print;

   if (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER ||
       Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
     {
      /***** Link to remove this exam announcement *****/
      Lay_PutContextualLink (ActReqRemExaAnn,Exa_PutParamExaCodToEdit,
                             "remove-on64x64.png",
			     Txt_Remove,NULL,
                             NULL);

      /***** Put form to hide/show exam announement *****/
      switch (Gbl.ExamAnns.ExaDat.Status)
        {
	 case Exa_VISIBLE_EXAM_ANNOUNCEMENT:
	    Lay_PutContextualLink (ActHidExaAnn,Exa_PutParamExaCodToEdit,
				   "eye-on64x64.png",
				   Txt_Hide,NULL,
				   NULL);
	    break;
	 case Exa_HIDDEN_EXAM_ANNOUNCEMENT:
	    Lay_PutContextualLink (ActShoExaAnn,Exa_PutParamExaCodToEdit,
				   "eye-slash-on64x64.png",
				   Txt_Show,NULL,
				   NULL);
	    break;
	 case Exa_DELETED_EXAM_ANNOUNCEMENT:	// Not applicable here
	    break;
        }

      /***** Link to edit this exam announcement *****/
      Lay_PutContextualLink (ActEdiExaAnn,Exa_PutParamExaCodToEdit,
                             "edit64x64.png",
			     Txt_Edit,NULL,
                             NULL);
     }

   /***** Link to print view *****/
   Lay_PutContextualLink (ActPrnExaAnn,Exa_PutParamExaCodToEdit,
                          "print64x64.png",
			  Txt_Print,NULL,
                          NULL);
  }

/*****************************************************************************/
/*************** Param with the code of an exam announcement *****************/
/*****************************************************************************/

static void Exa_PutParamExaCodToEdit (void)
  {
   Exa_PutHiddenParamExaCod (Gbl.ExamAnns.ExaDat.ExaCod);
  }

void Exa_PutHiddenParamExaCod (long ExaCod)
  {
   Par_PutHiddenParamLong ("ExaCod",ExaCod);
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
   char CrsNameAndDate[Hie_MAX_BYTES_FULL_NAME + (2 + Cns_MAX_BYTES_DATE + 6) + 1];

   /***** Initializations *****/
   Gbl.ExamAnns.ExaDat.ExaCod = ExaCod;
   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Allocate memory for the exam announcement *****/
   Exa_AllocMemExamAnnouncement ();

   /***** Get data of an exam announcement from database *****/
   Exa_GetDataExamAnnouncementFromDB ();

   /***** Content *****/
   if (GetContent)
      Exa_GetNotifContentExamAnnouncement (ContentStr);

   /***** Summary *****/
   /* Name of the course and date of exam */
   sprintf (CrsNameAndDate,"%s, %04u-%02u-%02u %2u:%02u",
            Gbl.ExamAnns.ExaDat.CrsFullName,
            Gbl.ExamAnns.ExaDat.ExamDate.Year,
            Gbl.ExamAnns.ExaDat.ExamDate.Month,
            Gbl.ExamAnns.ExaDat.ExamDate.Day,
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

   if ((*ContentStr = (char *) malloc (Cns_MAX_BYTES_TEXT * 8)) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for notification content.");
   (*ContentStr)[0] = '\0';	// Return nothing on error

   /***** Get data of course *****/
   Crs.CrsCod = Gbl.ExamAnns.ExaDat.CrsCod;
   Crs_GetDataOfCourseByCod (&Crs);

   /***** Get data of degree *****/
   Deg.DegCod = Crs.DegCod;
   Deg_GetDataOfDegreeByCod (&Deg);

   /***** Get data of institution *****/
   Ins.InsCod = Deg_GetInsCodOfDegreeByCod (Deg.DegCod);
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);

   Dat_ConvDateToDateStr (&Gbl.ExamAnns.ExaDat.ExamDate,StrExamDate);

   /***** Institution *****/
   sprintf (*ContentStr,"%s: %s<br />"
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
            Txt_EXAM_ANNOUNCEMENT_Other_information,Gbl.ExamAnns.ExaDat.OtherInfo);
  }
