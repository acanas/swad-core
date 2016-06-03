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
static void Exa_ModifyExamAnnouncementInDB (long ExaCod);
static void Exa_GetDataExamAnnouncementFromDB (long ExaCod);
static void Exa_ShowExamAnnouncement (long ExaCod,Exa_TypeViewExamAnnouncement_t TypeViewExamAnnouncement);
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
   Exa_ShowExamAnnouncement (ExaCod,Exa_FORM_VIEW);

   /***** Free memory of the exam announcement *****/
   Exa_FreeMemExamAnnouncement ();
  }

/*****************************************************************************/
/**************** Get parameters of an exam announcement *********************/
/*****************************************************************************/

static long Exa_GetParamsExamAnnouncement (void)
  {
   char UnsignedStr[10+1];
   long ExaCod;

   /***** Get the code of the exam announcement *****/
   ExaCod = Exa_GetParamExaCod ();

   /***** Get the name of the course (it is allowed to be different from the official name of the course) *****/
   Par_GetParToText ("CrsName",Gbl.ExamAnnouncements.ExaDat.CrsFullName,Cns_MAX_BYTES_STRING);
   // If the parameter is not present or is empty, initialize the string to the full name of the current course
   if (!Gbl.ExamAnnouncements.ExaDat.CrsFullName[0])
      strcpy (Gbl.ExamAnnouncements.ExaDat.CrsFullName,Gbl.CurrentCrs.Crs.FullName);

   /***** Get the year *****/
   Par_GetParToText ("Year",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&Gbl.ExamAnnouncements.ExaDat.Year) != 1)
      Gbl.ExamAnnouncements.ExaDat.Year = Gbl.CurrentCrs.Crs.Year;
   if (Gbl.ExamAnnouncements.ExaDat.Year > Deg_MAX_YEARS_PER_DEGREE)
      Gbl.ExamAnnouncements.ExaDat.Year = Gbl.CurrentCrs.Crs.Year;

   /***** Get the type of exam announcement *****/
   Par_GetParToText ("ExamSession",Gbl.ExamAnnouncements.ExaDat.Session,Cns_MAX_BYTES_STRING);

   /***** Get the data of the exam *****/
   Dat_GetDateFromForm ("ExamDay","ExamMonth","ExamYear",
                        &Gbl.ExamAnnouncements.ExaDat.ExamDate.Day,
                        &Gbl.ExamAnnouncements.ExaDat.ExamDate.Month,
                        &Gbl.ExamAnnouncements.ExaDat.ExamDate.Year);
   if (Gbl.ExamAnnouncements.ExaDat.ExamDate.Day   == 0 ||
       Gbl.ExamAnnouncements.ExaDat.ExamDate.Month == 0 ||
       Gbl.ExamAnnouncements.ExaDat.ExamDate.Year  == 0)
     {
      Gbl.ExamAnnouncements.ExaDat.ExamDate.Day   = Gbl.Now.Date.Day;
      Gbl.ExamAnnouncements.ExaDat.ExamDate.Month = Gbl.Now.Date.Month;
      Gbl.ExamAnnouncements.ExaDat.ExamDate.Year  = Gbl.Now.Date.Year;
     }

   /***** Get the hour of the exam *****/
   Par_GetParToText ("ExamHour",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&Gbl.ExamAnnouncements.ExaDat.StartTime.Hour) != 1)
      Gbl.ExamAnnouncements.ExaDat.StartTime.Hour = 0;
   if (Gbl.ExamAnnouncements.ExaDat.StartTime.Hour > 23)
      Gbl.ExamAnnouncements.ExaDat.StartTime.Hour = 0;
   Par_GetParToText ("ExamMinute",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&Gbl.ExamAnnouncements.ExaDat.StartTime.Minute) != 1)
      Gbl.ExamAnnouncements.ExaDat.StartTime.Minute = 0;
   if (Gbl.ExamAnnouncements.ExaDat.StartTime.Minute > 59)
      Gbl.ExamAnnouncements.ExaDat.StartTime.Minute = 0;

   /***** Get the duration of the exam *****/
   Par_GetParToText ("DurationHour",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&Gbl.ExamAnnouncements.ExaDat.Duration.Hour) != 1)
      Gbl.ExamAnnouncements.ExaDat.Duration.Hour = 0;
   if (Gbl.ExamAnnouncements.ExaDat.Duration.Hour > 23)
      Gbl.ExamAnnouncements.ExaDat.Duration.Hour = 0;
   Par_GetParToText ("DurationMinute",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&Gbl.ExamAnnouncements.ExaDat.Duration.Minute) != 1)
      Gbl.ExamAnnouncements.ExaDat.Duration.Minute = 0;
   if (Gbl.ExamAnnouncements.ExaDat.Duration.Minute > 59)
      Gbl.ExamAnnouncements.ExaDat.Duration.Minute = 0;

   /***** Get the place where the exam will happen *****/
   Par_GetParToHTML ("Place",Gbl.ExamAnnouncements.ExaDat.Place,Cns_MAX_BYTES_TEXT);

   /***** Get the modality of exam *****/
   Par_GetParToHTML ("ExamMode",Gbl.ExamAnnouncements.ExaDat.Mode,Cns_MAX_BYTES_TEXT);

   /***** Get the structure of exam *****/
   Par_GetParToHTML ("Structure",Gbl.ExamAnnouncements.ExaDat.Structure,Cns_MAX_BYTES_TEXT);

   /***** Get the mandatory documentation *****/
   Par_GetParToHTML ("DocRequired",Gbl.ExamAnnouncements.ExaDat.DocRequired,Cns_MAX_BYTES_TEXT);

   /***** Get the mandatory material *****/
   Par_GetParToHTML ("MatRequired",Gbl.ExamAnnouncements.ExaDat.MatRequired,Cns_MAX_BYTES_TEXT);

   /***** Get the allowed material *****/
   Par_GetParToHTML ("MatAllowed",Gbl.ExamAnnouncements.ExaDat.MatAllowed,Cns_MAX_BYTES_TEXT);

   /***** Get other information *****/
   Par_GetParToHTML ("OtherInfo",Gbl.ExamAnnouncements.ExaDat.OtherInfo,Cns_MAX_BYTES_TEXT);

   return ExaCod;
  }

/*****************************************************************************/
/* Allocate memory for those parameters of an exam anno. with a lot of text **/
/*****************************************************************************/

static void Exa_AllocMemExamAnnouncement (void)
  {
   if ((Gbl.ExamAnnouncements.ExaDat.Place       = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnnouncements.ExaDat.Mode        = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnnouncements.ExaDat.Structure   = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnnouncements.ExaDat.DocRequired = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnnouncements.ExaDat.MatRequired = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnnouncements.ExaDat.MatAllowed  = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnnouncements.ExaDat.OtherInfo   = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");
  }

/*****************************************************************************/
/* Free memory of those parameters of an exam announcem. with a lot of text **/
/*****************************************************************************/

void Exa_FreeMemExamAnnouncement (void)
  {
   if (Gbl.ExamAnnouncements.ExaDat.Place)
     {
      free ((void *) Gbl.ExamAnnouncements.ExaDat.Place);
      Gbl.ExamAnnouncements.ExaDat.Place = NULL;
     }
   if (Gbl.ExamAnnouncements.ExaDat.Mode)
     {
      free ((void *) Gbl.ExamAnnouncements.ExaDat.Mode);
      Gbl.ExamAnnouncements.ExaDat.Mode = NULL;
     }
   if (Gbl.ExamAnnouncements.ExaDat.Structure)
     {
      free ((void *) Gbl.ExamAnnouncements.ExaDat.Structure);
      Gbl.ExamAnnouncements.ExaDat.Structure = NULL;
     }
   if (Gbl.ExamAnnouncements.ExaDat.DocRequired)
     {
      free ((void *) Gbl.ExamAnnouncements.ExaDat.DocRequired);
      Gbl.ExamAnnouncements.ExaDat.DocRequired = NULL;
     }
   if (Gbl.ExamAnnouncements.ExaDat.MatRequired)
     {
      free ((void *) Gbl.ExamAnnouncements.ExaDat.MatRequired);
      Gbl.ExamAnnouncements.ExaDat.MatRequired = NULL;
     }
   if (Gbl.ExamAnnouncements.ExaDat.MatAllowed)
     {
      free ((void *) Gbl.ExamAnnouncements.ExaDat.MatAllowed);
      Gbl.ExamAnnouncements.ExaDat.MatAllowed = NULL;
     }
   if (Gbl.ExamAnnouncements.ExaDat.OtherInfo)
     {
      free ((void *) Gbl.ExamAnnouncements.ExaDat.OtherInfo);
      Gbl.ExamAnnouncements.ExaDat.OtherInfo = NULL;
     }
  }

/*****************************************************************************/
/************************ Receive an exam announcement ***********************/
/*****************************************************************************/

void Exa_ReceiveExamAnnouncement (void)
  {
   extern const char *Txt_Created_new_announcement_of_exam;
   extern const char *Txt_The_announcement_of_exam_has_been_successfully_updated;
   long ExaCod;
   bool NewExamAnnouncement;
   unsigned NumUsrsToBeNotifiedByEMail;
   struct SocialPublishing SocPub;

   /***** Allocate memory for the exam announcement *****/
   Exa_AllocMemExamAnnouncement ();

   /***** Get parameters of the exam announcement *****/
   ExaCod = Exa_GetParamsExamAnnouncement ();
   NewExamAnnouncement = (ExaCod == -1L);	// -1 indicates that it is a new exam announcement

   /***** Add the exam announcement to the database and read it again from the database *****/
   if (NewExamAnnouncement)
      ExaCod = Exa_AddExamAnnouncementToDB ();
   else
      Exa_ModifyExamAnnouncementInDB (ExaCod);

   /***** Free memory of the exam announcement *****/
   Exa_FreeMemExamAnnouncement ();

   /***** Show message *****/
   Lay_ShowAlert (Lay_SUCCESS,NewExamAnnouncement ? Txt_Created_new_announcement_of_exam :
                                                    Txt_The_announcement_of_exam_has_been_successfully_updated);

   /***** Notify by e-mail about the new exam announcement *****/
   if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_EXAM_ANNOUNCEMENT,ExaCod)))
      Exa_UpdateNumUsrsNotifiedByEMailAboutExamAnnouncement (ExaCod,NumUsrsToBeNotifiedByEMail);
   Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);

   /***** Create a new social note about the new exam announcement *****/
   Soc_StoreAndPublishSocialNote (Soc_NOTE_EXAM_ANNOUNCEMENT,ExaCod,&SocPub);

   /***** Show exam announcement *****/
   Exa_ListExamAnnouncementsEdit ();

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.CurrentCrs.Crs);
  }

/*****************************************************************************/
/***** Update number of users notified in table of exam announcements ********/
/*****************************************************************************/

static void Exa_UpdateNumUsrsNotifiedByEMailAboutExamAnnouncement (long ExaCod,unsigned NumUsrsToBeNotifiedByEMail)
  {
   char Query[512];

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
   long ExaCod;

   /***** Allocate memory for the exam announcement *****/
   Exa_AllocMemExamAnnouncement ();

   /***** Get the code of the exam announcement *****/
   if ((ExaCod = Exa_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Read exam announcement from the database *****/
   Exa_GetDataExamAnnouncementFromDB (ExaCod);

   /***** Show exam announcement *****/
   Exa_ShowExamAnnouncement (ExaCod,Exa_PRINT_VIEW);

   /***** Free memory of the exam announcement *****/
   Exa_FreeMemExamAnnouncement ();
  }

/*****************************************************************************/
/************************ Remove an exam announcement ************************/
/*****************************************************************************/

void Exa_RemoveExamAnnouncement (void)
  {
   extern const char *Txt_Announcement_of_exam_removed;
   char Query[1024];
   long ExaCod;

   /***** Get the code of the exam announcement *****/
   if ((ExaCod = Exa_GetParamExaCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Mark the exam announcement as deleted in the database *****/
   sprintf (Query,"UPDATE exam_announcements SET Status='%u'"
                  " WHERE ExaCod='%ld' AND CrsCod='%ld'",
            (unsigned) Exa_DELETED_EXAM_ANNOUNCEMENT,ExaCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not remove an exam announcement");

   /***** Mark possible notifications as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_EXAM_ANNOUNCEMENT,ExaCod);

   /***** Mark possible social note as unavailable *****/
   Soc_MarkSocialNoteAsUnavailableUsingNoteTypeAndCod (Soc_NOTE_EXAM_ANNOUNCEMENT,ExaCod);

   /***** Write message *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_Announcement_of_exam_removed);

   /***** List again all the remaining exam announcements *****/
   Exa_ListExamAnnouncementsEdit ();

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.CurrentCrs.Crs);
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
   Gbl.ExamAnnouncements.HighlightExaCod = Exa_GetParamExaCod ();
  }

/*****************************************************************************/
/*********** Get date of exam announcements to show highlighted **************/
/*****************************************************************************/

void Exa_GetDateToHighlight (void)
  {
   /***** Get the date (in YYYY-MM-DD format)
          of the exam announcements to highlight *****/
   Par_GetParToText ("Date",Gbl.ExamAnnouncements.HighlightDate,4+1+2+1+2);
  }

/*****************************************************************************/
/******************** List all the exam announcements ************************/
/*****************************************************************************/

static void Exa_ListExamAnnouncements (Exa_TypeViewExamAnnouncement_t TypeViewExamAnnouncement)
  {
   extern const char *Txt_All_announcements_of_exam;
   extern const char *Txt_Announcements_of_exam;
   extern const char *Txt_No_announcements_of_exams_of_X;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumExaAnn;
   unsigned long NumExaAnns;
   long ExaCod;
   bool ICanEdit = (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER ||
		    Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);

   /***** Show one highlighted exam announcement *****/
   if (Gbl.ExamAnnouncements.HighlightExaCod > 0)
     {
      /***** Get one exam announcement from database *****/
      sprintf (Query,"SELECT ExaCod"
		     " FROM exam_announcements"
		     " WHERE ExaCod='%ld' AND CrsCod='%ld' AND Status<>'%u'",
	       Gbl.ExamAnnouncements.HighlightExaCod,
	       Gbl.CurrentCrs.Crs.CrsCod,
	       (unsigned) Exa_DELETED_EXAM_ANNOUNCEMENT);
      NumExaAnns = DB_QuerySELECT (Query,&mysql_res,"can not get exam announcements in this course for listing");

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
	 Exa_ShowExamAnnouncement (ExaCod,TypeViewExamAnnouncement);

	 /***** Free memory of the exam announcement *****/
	 Exa_FreeMemExamAnnouncement ();
	}
     }

   /***** Show highlighted exam announcements of a date *****/
   if (Gbl.ExamAnnouncements.HighlightDate[0])
     {
      /***** Get exam announcements (the most recent first)
	     in current course for a date from database *****/
      sprintf (Query,"SELECT ExaCod"
		     " FROM exam_announcements"
		     " WHERE CrsCod='%ld' AND Status<>'%u'"
		     " AND DATE(ExamDate)='%s'"
		     " ORDER BY ExamDate DESC",
	       Gbl.CurrentCrs.Crs.CrsCod,
	       (unsigned) Exa_DELETED_EXAM_ANNOUNCEMENT,
	       Gbl.ExamAnnouncements.HighlightDate);
      NumExaAnns = DB_QuerySELECT (Query,&mysql_res,"can not get exam announcements in this course for listing");

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
	 Exa_ShowExamAnnouncement (ExaCod,TypeViewExamAnnouncement);

	 /***** Free memory of the exam announcement *****/
	 Exa_FreeMemExamAnnouncement ();
	}
     }

   /***** Get exam announcements (the most recent first)
          in current course from database *****/
   sprintf (Query,"SELECT ExaCod"
	          " FROM exam_announcements"
                  " WHERE CrsCod='%ld' AND Status<>'%u'"
                  " ORDER BY ExamDate DESC",
            Gbl.CurrentCrs.Crs.CrsCod,
            (unsigned) Exa_DELETED_EXAM_ANNOUNCEMENT);
   NumExaAnns = DB_QuerySELECT (Query,&mysql_res,"can not get exam announcements in this course for listing");

   /***** Start frame *****/
   Lay_StartRoundFrame (NULL,
                        (Gbl.ExamAnnouncements.HighlightExaCod > 0 ||
			 Gbl.ExamAnnouncements.HighlightDate[0]) ? Txt_All_announcements_of_exam :
								   Txt_Announcements_of_exam,
			ICanEdit ? Exa_PutIconToCreateNewExamAnnouncement :
				   NULL);

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

      if (sscanf (row[0],"%ld",&ExaCod) != 1)
         Lay_ShowErrorAndExit ("Wrong code of exam announcement.");

      /***** Allocate memory for the exam announcement *****/
      Exa_AllocMemExamAnnouncement ();

      /***** Read the data of the exam announcement *****/
      Exa_GetDataExamAnnouncementFromDB (ExaCod);

      /***** Show exam announcement *****/
      Exa_ShowExamAnnouncement (ExaCod,TypeViewExamAnnouncement);

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

   Lay_PutContextualLink (ActEdiExaAnn,NULL,"plus64x64.png",Txt_New_announcement_OF_EXAM,NULL);
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
   if ((Query = malloc (512+2*Cns_MAX_BYTES_STRING+7*Cns_MAX_BYTES_TEXT)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to query database.");
   sprintf (Query,"INSERT INTO exam_announcements "
	          "(CrsCod,Status,NumNotif,CrsFullName,Year,ExamSession,"
	          "CallDate,ExamDate,Duration,Place,"
                  "ExamMode,Structure,DocRequired,MatRequired,MatAllowed,OtherInfo)"
                  " VALUES "
                  "('%ld','%u','0','%s','%u','%s',"
                  "NOW(),'%04u-%02u-%02u %02u:%02u:00','%02u:%02u:00','%s',"
                  "'%s','%s','%s','%s','%s','%s')",
            Gbl.CurrentCrs.Crs.CrsCod,
            (unsigned) Exa_ACTIVE_EXAM_ANNOUNCEMENT,
            Gbl.ExamAnnouncements.ExaDat.CrsFullName,
            Gbl.ExamAnnouncements.ExaDat.Year,
            Gbl.ExamAnnouncements.ExaDat.Session,
            Gbl.ExamAnnouncements.ExaDat.ExamDate.Year,
            Gbl.ExamAnnouncements.ExaDat.ExamDate.Month,
            Gbl.ExamAnnouncements.ExaDat.ExamDate.Day,
            Gbl.ExamAnnouncements.ExaDat.StartTime.Hour,
            Gbl.ExamAnnouncements.ExaDat.StartTime.Minute,
	    Gbl.ExamAnnouncements.ExaDat.Duration.Hour,
	    Gbl.ExamAnnouncements.ExaDat.Duration.Minute,
            Gbl.ExamAnnouncements.ExaDat.Place,
	    Gbl.ExamAnnouncements.ExaDat.Mode,
	    Gbl.ExamAnnouncements.ExaDat.Structure,
            Gbl.ExamAnnouncements.ExaDat.DocRequired,
            Gbl.ExamAnnouncements.ExaDat.MatRequired,
            Gbl.ExamAnnouncements.ExaDat.MatAllowed,
            Gbl.ExamAnnouncements.ExaDat.OtherInfo);
   ExaCod = DB_QueryINSERTandReturnCode (Query,"can not create a new exam announcement");
   free ((void *) Query);

   return ExaCod;
  }

/*****************************************************************************/
/*************** Modify an exam announcement in the database *****************/
/*****************************************************************************/

static void Exa_ModifyExamAnnouncementInDB (long ExaCod)
  {
   char *Query;

   /***** Modify exam announcement *****/
   if ((Query = malloc (512+2*Cns_MAX_BYTES_STRING+7*Cns_MAX_BYTES_TEXT)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to query database.");
   sprintf (Query,"UPDATE exam_announcements"
                  " SET CrsFullName='%s',Year='%u',ExamSession='%s',"
                  "ExamDate='%04u-%02u-%02u %02u:%02u:00',"
                  "Duration='%02u:%02u:00',"
                  "Place='%s',ExamMode='%s',Structure='%s',"
                  "DocRequired='%s',MatRequired='%s',MatAllowed='%s',OtherInfo='%s'" \
                  " WHERE ExaCod='%ld'",
            Gbl.ExamAnnouncements.ExaDat.CrsFullName,
            Gbl.ExamAnnouncements.ExaDat.Year,
            Gbl.ExamAnnouncements.ExaDat.Session,
            Gbl.ExamAnnouncements.ExaDat.ExamDate.Year,
            Gbl.ExamAnnouncements.ExaDat.ExamDate.Month,
            Gbl.ExamAnnouncements.ExaDat.ExamDate.Day,
            Gbl.ExamAnnouncements.ExaDat.StartTime.Hour,
            Gbl.ExamAnnouncements.ExaDat.StartTime.Minute,
	    Gbl.ExamAnnouncements.ExaDat.Duration.Hour,
	    Gbl.ExamAnnouncements.ExaDat.Duration.Minute,
	    Gbl.ExamAnnouncements.ExaDat.Place,
            Gbl.ExamAnnouncements.ExaDat.Mode,
            Gbl.ExamAnnouncements.ExaDat.Structure,
            Gbl.ExamAnnouncements.ExaDat.DocRequired,
            Gbl.ExamAnnouncements.ExaDat.MatRequired,
            Gbl.ExamAnnouncements.ExaDat.MatAllowed,
            Gbl.ExamAnnouncements.ExaDat.OtherInfo,
            ExaCod);
   DB_QueryUPDATE (Query,"can not update an exam announcement");
   free ((void *) Query);
  }

/*****************************************************************************/
/******* Create a list with the dates of all the exam announcements **********/
/*****************************************************************************/

void Exa_CreateListOfExamAnnouncements (void)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumExaAnn;
   unsigned long NumExaAnns;

   if (Gbl.DB.DatabaseIsOpen)
     {
      /***** Get exam dates (no matter in what order)
             in current course from database *****/
      sprintf (Query,"SELECT DISTINCT(DATE(ExamDate))"
	             " FROM exam_announcements" \
		     " WHERE CrsCod='%ld' AND Status<>'%u'",
	       Gbl.CurrentCrs.Crs.CrsCod,
	       (unsigned) Exa_DELETED_EXAM_ANNOUNCEMENT);
      NumExaAnns = DB_QuerySELECT (Query,&mysql_res,"can not get exam announcements in this course");

      /***** The result of the query may be empty *****/
      Gbl.ExamAnnouncements.Lst = NULL;
      Gbl.ExamAnnouncements.NumExaAnns = 0;
      if (NumExaAnns)
	{
	 /***** Allocate memory for the list *****/
	 if ((Gbl.ExamAnnouncements.Lst = (struct Date *) calloc (NumExaAnns,sizeof (struct Date))) == NULL)
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
			&Gbl.ExamAnnouncements.Lst[Gbl.ExamAnnouncements.NumExaAnns].Year,
			&Gbl.ExamAnnouncements.Lst[Gbl.ExamAnnouncements.NumExaAnns].Month,
			&Gbl.ExamAnnouncements.Lst[Gbl.ExamAnnouncements.NumExaAnns].Day) != 3)
	       Lay_ShowErrorAndExit ("Wrong date of exam.");

	    /***** Increment number of elements in list *****/
	    Gbl.ExamAnnouncements.NumExaAnns++;
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
   if (Gbl.ExamAnnouncements.Lst)
     {
      free ((void *) Gbl.ExamAnnouncements.Lst);
      Gbl.ExamAnnouncements.Lst = NULL;
      Gbl.ExamAnnouncements.NumExaAnns = 0;
     }
  }

/*****************************************************************************/
/******** Read the data of an exam announcement from the database ************/
/*****************************************************************************/

static void Exa_GetDataExamAnnouncementFromDB (long ExaCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumExaAnns;
   unsigned Hour;
   unsigned Minute;
   unsigned Second;

   /***** Get data of an exam announcement from database *****/
   sprintf (Query,"SELECT CrsCod,CrsFullName,Year,ExamSession,"
	          "CallDate,ExamDate,Duration,Place,ExamMode,"
	          "Structure,DocRequired,MatRequired,MatAllowed,OtherInfo"
                  " FROM exam_announcements WHERE ExaCod='%ld'",
           ExaCod);
   NumExaAnns = DB_QuerySELECT (Query,&mysql_res,"can not get data of an exam announcement");

   /***** The result of the query must have one row *****/
   if (NumExaAnns != 1)
      Lay_ShowErrorAndExit ("Error when getting data of an exam announcement.");

   /***** Get the data of the exam announcement *****/
   row = mysql_fetch_row (mysql_res);

   /* Code of the course in which the exam announcement is inserted (row[0]) */
   Gbl.ExamAnnouncements.ExaDat.CrsCod = Str_ConvertStrCodToLongCod (row[0]);

   /* Name of the course (row[1]) */
   strcpy (Gbl.ExamAnnouncements.ExaDat.CrsFullName,row[1]);

   /* Year (row[2]) */
   if (sscanf (row[2],"%u",&Gbl.ExamAnnouncements.ExaDat.Year) != 1)
      Lay_ShowErrorAndExit ("Wrong year.");

   /* Exam session (row[3]) */
   strcpy (Gbl.ExamAnnouncements.ExaDat.Session,row[3]);

   /* Date of exam announcement (row[4]) */
   if (sscanf (row[4],"%04u-%02u-%02u %02u:%02u:%02u",
               &Gbl.ExamAnnouncements.ExaDat.CallDate.Year,
               &Gbl.ExamAnnouncements.ExaDat.CallDate.Month,
               &Gbl.ExamAnnouncements.ExaDat.CallDate.Day,
               &Hour,&Minute,&Second) != 6)
      Lay_ShowErrorAndExit ("Wrong date of exam announcement.");

   /* Date of exam (row[5]) */
   if (sscanf (row[5],"%04u-%02u-%02u %02u:%02u:%02u",
   	       &Gbl.ExamAnnouncements.ExaDat.ExamDate.Year,&Gbl.ExamAnnouncements.ExaDat.ExamDate.Month,&Gbl.ExamAnnouncements.ExaDat.ExamDate.Day,
               &Gbl.ExamAnnouncements.ExaDat.StartTime.Hour,&Gbl.ExamAnnouncements.ExaDat.StartTime.Minute,&Second) != 6)
      Lay_ShowErrorAndExit ("Wrong date of exam.");

   /* Approximate duration (row[6]) */
   if (sscanf (row[6],"%02u:%02u:%02u",&Gbl.ExamAnnouncements.ExaDat.Duration.Hour,&Gbl.ExamAnnouncements.ExaDat.Duration.Minute,&Second) != 3)
      Lay_ShowErrorAndExit ("Wrong duration of exam.");

   /* Place (row[7]) */
   strncpy (Gbl.ExamAnnouncements.ExaDat.Place,row[7],Cns_MAX_BYTES_TEXT);
   Gbl.ExamAnnouncements.ExaDat.Place[Cns_MAX_BYTES_TEXT] = '\0';

   /* Exam mode (row[8]) */
   strncpy (Gbl.ExamAnnouncements.ExaDat.Mode,row[8],Cns_MAX_BYTES_TEXT);
   Gbl.ExamAnnouncements.ExaDat.Mode[Cns_MAX_BYTES_TEXT] = '\0';

   /* Structure (row[9]) */
   strncpy (Gbl.ExamAnnouncements.ExaDat.Structure,row[9],Cns_MAX_BYTES_TEXT);
   Gbl.ExamAnnouncements.ExaDat.Structure[Cns_MAX_BYTES_TEXT] = '\0';

   /* Documentation required (row[10]) */
   strncpy (Gbl.ExamAnnouncements.ExaDat.DocRequired,row[10],Cns_MAX_BYTES_TEXT);
   Gbl.ExamAnnouncements.ExaDat.DocRequired[Cns_MAX_BYTES_TEXT] = '\0';

   /* Material required (row[11]) */
   strncpy (Gbl.ExamAnnouncements.ExaDat.MatRequired,row[11],Cns_MAX_BYTES_TEXT);
   Gbl.ExamAnnouncements.ExaDat.MatRequired[Cns_MAX_BYTES_TEXT] = '\0';

   /* Material allowed (row[12]) */
   strncpy (Gbl.ExamAnnouncements.ExaDat.MatAllowed,row[12],Cns_MAX_BYTES_TEXT);
   Gbl.ExamAnnouncements.ExaDat.MatAllowed[Cns_MAX_BYTES_TEXT] = '\0';

   /* Other information for students (row[13]) */
   strncpy (Gbl.ExamAnnouncements.ExaDat.OtherInfo,row[13],Cns_MAX_BYTES_TEXT);
   Gbl.ExamAnnouncements.ExaDat.OtherInfo[Cns_MAX_BYTES_TEXT] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Show a form with the data of an exam announcement **************/
/*****************************************************************************/

static void Exa_ShowExamAnnouncement (long ExaCod,Exa_TypeViewExamAnnouncement_t TypeViewExamAnnouncement)
  {
   extern const char *Txt_YEAR_OF_DEGREE[1+Deg_MAX_YEARS_PER_DEGREE];
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_EXAM_ANNOUNCEMENT;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Course;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Year;
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
   struct Institution Ins;
   char StrExamDate[Cns_MAX_LENGTH_DATE+1];
   unsigned Year,Hour,Minute;

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
   Gbl.ExamAnnouncements.ExaCodToEdit = ExaCod;	// Used as parameter in contextual links
   Lay_StartRoundFrame ("625px",NULL,
                        TypeViewExamAnnouncement == Exa_NORMAL_VIEW ? Exa_PutIconsExamAnnouncement :
                                                                      NULL);

   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      /***** Start form *****/
      Act_FormStart (ActRcvExaAnn);
      if (ExaCod > 0)
	 Exa_PutHiddenParamExaCod (ExaCod);
     }

   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\""
                      " style=\"width:100%%;"
                      " padding:25px 25px 125px 25px;\">");      // Bottom space used for signatures

   /***** Institution logo *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td colspan=\"2\" class=\"CENTER_MIDDLE\">");
   if (TypeViewExamAnnouncement == Exa_PRINT_VIEW)
      fprintf (Gbl.F.Out,"<span class=\"%s\">",StyleTitle);
   else
      fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"%s\">",
               Ins.WWW,StyleTitle);
   Log_DrawLogo (Sco_SCOPE_INS,Ins.InsCod,Ins.FullName,64,NULL,true);
   fprintf (Gbl.F.Out,"<br />%s%s" \
                      "</td>" \
	              "</tr>",
            Ins.FullName,
            TypeViewExamAnnouncement == Exa_PRINT_VIEW ? "</span>" :
 	                                                 "</a>");

   /***** Degree *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td colspan=\"2\" class=\"%s CENTER_MIDDLE\">",
	    StyleTitle);
   if (TypeViewExamAnnouncement == Exa_NORMAL_VIEW)
      fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"%s\">",
               Gbl.CurrentDeg.Deg.WWW,StyleTitle);
   fprintf (Gbl.F.Out,"%s",Gbl.CurrentDeg.Deg.FullName);
   if (TypeViewExamAnnouncement == Exa_NORMAL_VIEW)
      fprintf (Gbl.F.Out,"</a>");
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Title *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td colspan=\"2\" class=\"%s CENTER_MIDDLE\">"
	              "&nbsp;<br />"
	              "<strong>%s</strong>"
	              "</td>" \
	              "</tr>",
            StyleNormal,Txt_EXAM_ANNOUNCEMENT);
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td colspan=\"2\" class=\"%s LEFT_MIDDLE\">"
	              "&nbsp;"
	              "</td>" \
	              "</tr>",
	    StyleNormal);

   /***** Name of the course *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Course,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"CrsName\""
	                 " size=\"30\" maxlength=\"%u\" value=\"%s\" />",
               Cns_MAX_LENGTH_STRING,Gbl.ExamAnnouncements.ExaDat.CrsFullName);
     }
   else
      fprintf (Gbl.F.Out,"<strong>%s</strong>",Gbl.ExamAnnouncements.ExaDat.CrsFullName);
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Year (N.A., 1º, 2º, 3º, 4º, 5º...) *****/
   fprintf (Gbl.F.Out,"<tr>" \
                      "<td class=\"%s RIGHT_TOP\">"
                      "%s:"
                      "</td>" \
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Year,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      fprintf (Gbl.F.Out,"<select name=\"Year\">");
      for (Year = 0;
	   Year <= Deg_MAX_YEARS_PER_DEGREE;
	   Year++)
        {
	 fprintf (Gbl.F.Out,"<option");
	 if (Gbl.ExamAnnouncements.ExaDat.Year == Year)
            fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out," value=\"%u\">%s</option>",Year,Txt_YEAR_OF_DEGREE[Year]);
	}
      fprintf (Gbl.F.Out,"</select>");
     }
   else
      fprintf (Gbl.F.Out,"%s",Txt_YEAR_OF_DEGREE[Gbl.ExamAnnouncements.ExaDat.Year]);
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Exam session *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Session,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ExamSession\""
	                 " size=\"30\" maxlength=\"%u\" value=\"%s\" />",
               Cns_MAX_LENGTH_STRING,Gbl.ExamAnnouncements.ExaDat.Session);
   else
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncements.ExaDat.Session);
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Date of the exam *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Exam_date);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\">");
      Dat_WriteFormDate (Gbl.ExamAnnouncements.ExaDat.ExamDate.Year < Gbl.Now.Date.Year ? Gbl.ExamAnnouncements.ExaDat.ExamDate.Year :
                                                                                          Gbl.Now.Date.Year,
                         Gbl.Now.Date.Year + 1,"Exam",
                         &(Gbl.ExamAnnouncements.ExaDat.ExamDate),
                         false,false);
      fprintf (Gbl.F.Out,"</td>");
     }
   else
     {
      Dat_ConvDateToDateStr (&Gbl.ExamAnnouncements.ExaDat.ExamDate,StrExamDate);
      fprintf (Gbl.F.Out,"<td class=\"%s LEFT_TOP\">"
	                 "%s"
	                 "</td>",
               StyleNormal,StrExamDate);
     }
   fprintf (Gbl.F.Out,"</tr>");

   /***** Start time *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Start_time,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      fprintf (Gbl.F.Out,"<select name=\"ExamHour\"><option value=\"0\"");
      if (Gbl.ExamAnnouncements.ExaDat.StartTime.Hour == 0)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">-</option>");
      for (Hour = 7;
	   Hour <= 22;
	   Hour++)
        {
	 fprintf (Gbl.F.Out,"<option value=\"%u\"",Hour);
	 if (Gbl.ExamAnnouncements.ExaDat.StartTime.Hour == Hour)
            fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out,">%02u %s</option>",
                  Hour,Txt_hours_ABBREVIATION);
	}
      fprintf (Gbl.F.Out,"</select>" \
	                 "<select name=\"ExamMinute\">");
      for (Minute = 0;
	   Minute <= 59;
	   Minute++)
        {
	 fprintf (Gbl.F.Out,"<option value=\"%u\"",Minute);
	 if (Gbl.ExamAnnouncements.ExaDat.StartTime.Minute == Minute)
            fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out,">%02u &#39;</option>",Minute);
	}
      fprintf (Gbl.F.Out,"</select>");
     }
   else if (Gbl.ExamAnnouncements.ExaDat.StartTime.Hour)
      fprintf (Gbl.F.Out,"%2u:%02u",
               Gbl.ExamAnnouncements.ExaDat.StartTime.Hour,
               Gbl.ExamAnnouncements.ExaDat.StartTime.Minute);
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Approximate duration of the exam *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s LEFT_TOP\">",
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
	 if (Gbl.ExamAnnouncements.ExaDat.Duration.Hour == Hour)
            fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out,">%02u %s</option>",
                  Hour,Txt_hours_ABBREVIATION);
	}
      fprintf (Gbl.F.Out,"</select><select name=\"DurationMinute\">");
      for (Minute = 0;
	   Minute <= 59;
	   Minute++)
        {
	 fprintf (Gbl.F.Out,"<option value=\"%u\"",Minute);
	 if (Gbl.ExamAnnouncements.ExaDat.Duration.Minute == Minute)
            fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out,">%02u &#39;</option>",Minute);
	}
      fprintf (Gbl.F.Out,"</select>");
     }
   else if (Gbl.ExamAnnouncements.ExaDat.Duration.Hour ||
            Gbl.ExamAnnouncements.ExaDat.Duration.Minute)
     {
      if (Gbl.ExamAnnouncements.ExaDat.Duration.Hour)
        {
         if (Gbl.ExamAnnouncements.ExaDat.Duration.Minute)
            fprintf (Gbl.F.Out,"%u %s %u &#39;",
                     Gbl.ExamAnnouncements.ExaDat.Duration.Hour,
                     Txt_hours_ABBREVIATION,
                     Gbl.ExamAnnouncements.ExaDat.Duration.Minute);
         else
           {
            if (Gbl.ExamAnnouncements.ExaDat.Duration.Hour == 1)
               fprintf (Gbl.F.Out,"1 %s",Txt_hour);
            else
               fprintf (Gbl.F.Out,"%u %s",Gbl.ExamAnnouncements.ExaDat.Duration.Hour,Txt_hours);
           }
        }
      else if (Gbl.ExamAnnouncements.ExaDat.Duration.Minute)
        {
         if (Gbl.ExamAnnouncements.ExaDat.Duration.Minute == 1)
            fprintf (Gbl.F.Out,"1 %s",Txt_minute);
         else
            fprintf (Gbl.F.Out,"%u %s",Gbl.ExamAnnouncements.ExaDat.Duration.Minute,Txt_minutes);
        }
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Place where the exam will be made *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Place_of_exam,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea name=\"Place\" cols=\"40\" rows=\"4\">%s</textarea>",
               Gbl.ExamAnnouncements.ExaDat.Place);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnnouncements.ExaDat.Place,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncements.ExaDat.Place);
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Exam mode *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Mode,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea name=\"ExamMode\" cols=\"40\" rows=\"2\">%s</textarea>",
               Gbl.ExamAnnouncements.ExaDat.Mode);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnnouncements.ExaDat.Mode,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncements.ExaDat.Mode);
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Structure of the exam *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Structure_of_the_exam,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea name=\"Structure\" cols=\"40\" rows=\"8\">%s</textarea>",
               Gbl.ExamAnnouncements.ExaDat.Structure);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnnouncements.ExaDat.Structure,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncements.ExaDat.Structure);
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Documentation required *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Documentation_required,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea name=\"DocRequired\" cols=\"40\" rows=\"2\">%s</textarea>",
               Gbl.ExamAnnouncements.ExaDat.DocRequired);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnnouncements.ExaDat.DocRequired,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncements.ExaDat.DocRequired);
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Material required *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Material_required,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea name=\"MatRequired\" cols=\"40\" rows=\"4\">%s</textarea>",
               Gbl.ExamAnnouncements.ExaDat.MatRequired);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnnouncements.ExaDat.MatRequired,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncements.ExaDat.MatRequired);
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Material allowed *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,
            Txt_EXAM_ANNOUNCEMENT_Material_allowed,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea name=\"MatAllowed\" cols=\"40\" rows=\"4\">%s</textarea>",
               Gbl.ExamAnnouncements.ExaDat.MatAllowed);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnnouncements.ExaDat.MatAllowed,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncements.ExaDat.MatAllowed);
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Other information to students *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s LEFT_TOP\">",
            StyleForm,Txt_EXAM_ANNOUNCEMENT_Other_information,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea name=\"OtherInfo\" cols=\"40\" rows=\"5\">%s</textarea>",
               Gbl.ExamAnnouncements.ExaDat.OtherInfo);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnnouncements.ExaDat.OtherInfo,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncements.ExaDat.OtherInfo);
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>" \
                      "</table>");

   /***** End frame *****/
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      Lay_EndRoundFrameWithButton ((ExaCod > 0) ? Lay_CONFIRM_BUTTON :
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
   extern const char *Txt_Edit;
   extern const char *Txt_Print;

   if (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER ||
       Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
     {
      /***** Link to remove this exam announcement *****/
      Lay_PutContextualLink (ActRemExaAnn,Exa_PutParamExaCodToEdit,"remove-on64x64.png",
			     Txt_Remove,NULL);

      /***** Link to edit this exam announcement *****/
      Lay_PutContextualLink (ActEdiExaAnn,Exa_PutParamExaCodToEdit,"edit64x64.png",
			     Txt_Edit,NULL);
     }

   /***** Link to print view *****/
   Lay_PutContextualLink (ActPrnExaAnn,Exa_PutParamExaCodToEdit,"print64x64.png",
			  Txt_Print,NULL);
  }

/*****************************************************************************/
/*************** Param with the code of an exam announcement *****************/
/*****************************************************************************/

static void Exa_PutParamExaCodToEdit (void)
  {
   Exa_PutHiddenParamExaCod (Gbl.ExamAnnouncements.ExaCodToEdit);
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
   char LongStr[1+10+1];	// String that holds the exam announcement code
   long ExaCod;

   /* Get notice code */
   Par_GetParToText ("ExaCod",LongStr,1+10);
   if (sscanf (LongStr,"%ld",&ExaCod) != 1)
      ExaCod = -1L;

   return ExaCod;
  }

/*****************************************************************************/
/************ Get summary and content about an exam announcement *************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error
// MaxChars must be > 3+(2+Cns_MAX_LENGTH_DATE+6)

void Exa_GetSummaryAndContentExamAnnouncement (char *SummaryStr,char **ContentStr,
                                               long ExaCod,unsigned MaxChars,bool GetContent)
  {
   extern const char *Txt_hours_ABBREVIATION;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Allocate memory for the exam announcement *****/
   Exa_AllocMemExamAnnouncement ();

   /***** Get data of an exam announcement from database *****/
   Exa_GetDataExamAnnouncementFromDB (ExaCod);

   /***** Content *****/
   if (GetContent)
      Exa_GetNotifContentExamAnnouncement (ContentStr);

   /***** Summary *****/
   /* Name of the course */
   if (MaxChars)
      Str_LimitLengthHTMLStr (Gbl.ExamAnnouncements.ExaDat.CrsFullName,
                              MaxChars-(2+Cns_MAX_LENGTH_DATE+6));

   /* Date of exam */
   sprintf (SummaryStr,"%s, %04u-%02u-%02u %2u:%02u",
            Gbl.ExamAnnouncements.ExaDat.CrsFullName,
            Gbl.ExamAnnouncements.ExaDat.ExamDate.Year,
            Gbl.ExamAnnouncements.ExaDat.ExamDate.Month,
            Gbl.ExamAnnouncements.ExaDat.ExamDate.Day,
            Gbl.ExamAnnouncements.ExaDat.StartTime.Hour,
            Gbl.ExamAnnouncements.ExaDat.StartTime.Minute);

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
   extern const char *Txt_YEAR_OF_DEGREE[1+Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_EXAM_ANNOUNCEMENT_Course;
   extern const char *Txt_EXAM_ANNOUNCEMENT_Year;
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
   struct Institution Ins;
   char StrExamDate[Cns_MAX_LENGTH_DATE+1];

   if ((*ContentStr = (char *) malloc (Cns_MAX_BYTES_TEXT*8)) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for notification content.");
   (*ContentStr)[0] = '\0';	// Return nothing on error

   /***** Get data of course *****/
   Crs.CrsCod = Gbl.ExamAnnouncements.ExaDat.CrsCod;
   Crs_GetDataOfCourseByCod (&Crs);

   /***** Get data of degree *****/
   Deg.DegCod = Crs.DegCod;
   Deg_GetDataOfDegreeByCod (&Deg);

   /***** Get data of institution *****/
   Ins.InsCod = Deg_GetInsCodOfDegreeByCod (Deg.DegCod);
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);

   Dat_ConvDateToDateStr (&Gbl.ExamAnnouncements.ExaDat.ExamDate,StrExamDate);

   /***** Institution *****/
   sprintf (*ContentStr,"%s: %s<br />" \
                        "%s: %s<br />" \
                        "%s: %s<br />" \
                        "%s: %s<br />" \
                        "%s: %s<br />" \
                        "%s: %s<br />" \
                        "%s: %2u:%02u %s<br />" \
                        "%s: %2u:%02u %s<br />" \
                        "%s: %s<br />" \
                        "%s: %s<br />" \
                        "%s: %s<br />" \
                        "%s: %s<br />" \
                        "%s: %s<br />" \
                        "%s: %s<br />" \
                        "%s: %s",
            Txt_Institution,Ins.FullName,
            Txt_Degree,Deg.FullName,
            Txt_EXAM_ANNOUNCEMENT_Course,Gbl.ExamAnnouncements.ExaDat.CrsFullName,
            Txt_EXAM_ANNOUNCEMENT_Year,Txt_YEAR_OF_DEGREE[Gbl.ExamAnnouncements.ExaDat.Year],
            Txt_EXAM_ANNOUNCEMENT_Session,Gbl.ExamAnnouncements.ExaDat.Session,
            Txt_EXAM_ANNOUNCEMENT_Exam_date,StrExamDate,
            Txt_EXAM_ANNOUNCEMENT_Start_time,Gbl.ExamAnnouncements.ExaDat.StartTime.Hour,
                                             Gbl.ExamAnnouncements.ExaDat.StartTime.Minute,
            Txt_hours_ABBREVIATION,
            Txt_EXAM_ANNOUNCEMENT_Approximate_duration,Gbl.ExamAnnouncements.ExaDat.Duration.Hour,
                                                       Gbl.ExamAnnouncements.ExaDat.Duration.Minute,
            Txt_hours_ABBREVIATION,
            Txt_EXAM_ANNOUNCEMENT_Place_of_exam,Gbl.ExamAnnouncements.ExaDat.Place,
            Txt_EXAM_ANNOUNCEMENT_Mode,Gbl.ExamAnnouncements.ExaDat.Mode,
            Txt_EXAM_ANNOUNCEMENT_Structure_of_the_exam,Gbl.ExamAnnouncements.ExaDat.Structure,
            Txt_EXAM_ANNOUNCEMENT_Documentation_required,Gbl.ExamAnnouncements.ExaDat.DocRequired,
            Txt_EXAM_ANNOUNCEMENT_Material_required,Gbl.ExamAnnouncements.ExaDat.MatRequired,
            Txt_EXAM_ANNOUNCEMENT_Material_allowed,Gbl.ExamAnnouncements.ExaDat.MatAllowed,
            Txt_EXAM_ANNOUNCEMENT_Other_information,Gbl.ExamAnnouncements.ExaDat.OtherInfo);
  }
