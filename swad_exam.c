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
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_RSS.h"
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
static void Exa_ListExamAnnouncements (Exa_tTypeViewExamAnnouncement_t TypeViewExamAnnouncement);
static long Exa_AddExamAnnouncementToDB (void);
static void Exa_ModifyExamAnnouncementInDB (long ExaCod);
static void Exa_GetDataExamAnnouncementFromDB (long ExaCod);
static void Exa_ShowExamAnnouncement (long ExaCod,Exa_tTypeViewExamAnnouncement_t TypeViewExamAnnouncement);
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

   if (ExaCod >= 0)	// -1 indicates that this is a new exam announcement
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
   char LongStr[1+10+1];
   long ExaCod;

   /***** Get the code of the exam announcement *****/
   Par_GetParToText ("ExaCod",LongStr,1+10);
   if (sscanf (LongStr,"%ld",&ExaCod) != 1)
      ExaCod = -1;

   /***** Get the name of the course (it is allowed to be different from the official name of the course) *****/
   Par_GetParToText ("CrsName",Gbl.ExamAnnouncement.CrsFullName,Cns_MAX_BYTES_STRING);
   // If the parameter is not present or is empty, initialize the string to the full name of the current course
   if (!Gbl.ExamAnnouncement.CrsFullName[0])
      strcpy (Gbl.ExamAnnouncement.CrsFullName,Gbl.CurrentCrs.Crs.FullName);

   /***** Get the year *****/
   Par_GetParToText ("Year",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&Gbl.ExamAnnouncement.Year) != 1)
      Gbl.ExamAnnouncement.Year = Gbl.CurrentCrs.Crs.Year;
   if (Gbl.ExamAnnouncement.Year > Deg_MAX_YEARS_PER_DEGREE)
      Gbl.ExamAnnouncement.Year = Gbl.CurrentCrs.Crs.Year;

   /***** Get the type of exam announcement *****/
   Par_GetParToText ("ExamSession",Gbl.ExamAnnouncement.Session,Cns_MAX_BYTES_STRING);

   /***** Get the data of the exam *****/
   Dat_GetDateFromForm ("ExamDay","ExamMonth","ExamYear",&Gbl.ExamAnnouncement.ExamDate.Day,&Gbl.ExamAnnouncement.ExamDate.Month,&Gbl.ExamAnnouncement.ExamDate.Year);
   if (Gbl.ExamAnnouncement.ExamDate.Day   == 0 ||
       Gbl.ExamAnnouncement.ExamDate.Month == 0 ||
       Gbl.ExamAnnouncement.ExamDate.Year  == 0)
     {
      Gbl.ExamAnnouncement.ExamDate.Day   = Gbl.Now.Date.Day;
      Gbl.ExamAnnouncement.ExamDate.Month = Gbl.Now.Date.Month;
      Gbl.ExamAnnouncement.ExamDate.Year  = Gbl.Now.Date.Year;
     }

   /***** Get the hour of the exam *****/
   Par_GetParToText ("ExamHour",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&Gbl.ExamAnnouncement.StartTime.Hour) != 1)
      Gbl.ExamAnnouncement.StartTime.Hour = 0;
   if (Gbl.ExamAnnouncement.StartTime.Hour > 23)
      Gbl.ExamAnnouncement.StartTime.Hour = 0;
   Par_GetParToText ("ExamMinute",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&Gbl.ExamAnnouncement.StartTime.Minute) != 1)
      Gbl.ExamAnnouncement.StartTime.Minute = 0;
   if (Gbl.ExamAnnouncement.StartTime.Minute > 59)
      Gbl.ExamAnnouncement.StartTime.Minute = 0;

   /***** Get the duration of the exam *****/
   Par_GetParToText ("DurationHour",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&Gbl.ExamAnnouncement.Duration.Hour) != 1)
      Gbl.ExamAnnouncement.Duration.Hour = 0;
   if (Gbl.ExamAnnouncement.Duration.Hour > 23)
      Gbl.ExamAnnouncement.Duration.Hour = 0;
   Par_GetParToText ("DurationMinute",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&Gbl.ExamAnnouncement.Duration.Minute) != 1)
      Gbl.ExamAnnouncement.Duration.Minute = 0;
   if (Gbl.ExamAnnouncement.Duration.Minute > 59)
      Gbl.ExamAnnouncement.Duration.Minute = 0;

   /***** Get the place where the exam will happen *****/
   Par_GetParToHTML ("Place",Gbl.ExamAnnouncement.Place,Cns_MAX_BYTES_TEXT);

   /***** Get the modality of exam *****/
   Par_GetParToHTML ("ExamMode",Gbl.ExamAnnouncement.Mode,Cns_MAX_BYTES_TEXT);

   /***** Get the structure of exam *****/
   Par_GetParToHTML ("Structure",Gbl.ExamAnnouncement.Structure,Cns_MAX_BYTES_TEXT);

   /***** Get the mandatory documentation *****/
   Par_GetParToHTML ("DocRequired",Gbl.ExamAnnouncement.DocRequired,Cns_MAX_BYTES_TEXT);

   /***** Get the mandatory material *****/
   Par_GetParToHTML ("MatRequired",Gbl.ExamAnnouncement.MatRequired,Cns_MAX_BYTES_TEXT);

   /***** Get the allowed material *****/
   Par_GetParToHTML ("MatAllowed",Gbl.ExamAnnouncement.MatAllowed,Cns_MAX_BYTES_TEXT);

   /***** Get other information *****/
   Par_GetParToHTML ("OtherInfo",Gbl.ExamAnnouncement.OtherInfo,Cns_MAX_BYTES_TEXT);

   return ExaCod;
  }

/*****************************************************************************/
/* Allocate memory for those parameters of an exam anno. with a lot of text **/
/*****************************************************************************/

static void Exa_AllocMemExamAnnouncement (void)
  {
   if ((Gbl.ExamAnnouncement.Place       = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnnouncement.Mode        = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnnouncement.Structure   = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnnouncement.DocRequired = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnnouncement.MatRequired = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnnouncement.MatAllowed  = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");

   if ((Gbl.ExamAnnouncement.OtherInfo   = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store exam announcement.");
  }

/*****************************************************************************/
/* Free memory of those parameters of an exam announcem. with a lot of text **/
/*****************************************************************************/

void Exa_FreeMemExamAnnouncement (void)
  {
   if (Gbl.ExamAnnouncement.Place)
     {
      free ((void *) Gbl.ExamAnnouncement.Place);
      Gbl.ExamAnnouncement.Place = NULL;
     }
   if (Gbl.ExamAnnouncement.Mode)
     {
      free ((void *) Gbl.ExamAnnouncement.Mode);
      Gbl.ExamAnnouncement.Mode = NULL;
     }
   if (Gbl.ExamAnnouncement.Structure)
     {
      free ((void *) Gbl.ExamAnnouncement.Structure);
      Gbl.ExamAnnouncement.Structure = NULL;
     }
   if (Gbl.ExamAnnouncement.DocRequired)
     {
      free ((void *) Gbl.ExamAnnouncement.DocRequired);
      Gbl.ExamAnnouncement.DocRequired = NULL;
     }
   if (Gbl.ExamAnnouncement.MatRequired)
     {
      free ((void *) Gbl.ExamAnnouncement.MatRequired);
      Gbl.ExamAnnouncement.MatRequired = NULL;
     }
   if (Gbl.ExamAnnouncement.MatAllowed)
     {
      free ((void *) Gbl.ExamAnnouncement.MatAllowed);
      Gbl.ExamAnnouncement.MatAllowed = NULL;
     }
   if (Gbl.ExamAnnouncement.OtherInfo)
     {
      free ((void *) Gbl.ExamAnnouncement.OtherInfo);
      Gbl.ExamAnnouncement.OtherInfo = NULL;
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
   char LongStr[1+10+1];
   long ExaCod;

   /***** Allocate memory for the exam announcement *****/
   Exa_AllocMemExamAnnouncement ();

   /***** Get the code of the exam announcement *****/
   Par_GetParToText ("ExaCod",LongStr,1+10);
   if (sscanf (LongStr,"%ld",&ExaCod) != 1)
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
   char LongStr[1+10+1];
   long ExaCod;

   /***** Get the code of the exam announcement *****/
   Par_GetParToText ("ExaCod",LongStr,1+10);
   if (sscanf (LongStr,"%ld",&ExaCod) != 1)
      Lay_ShowErrorAndExit ("Code of exam announcement is missing.");

   /***** Mark the exam announcement as deleted in the database *****/
   sprintf (Query,"UPDATE exam_announcements SET Status='%u'"
                  " WHERE ExaCod='%ld' AND CrsCod='%ld'",
            (unsigned) Exa_DELETED_EXAM_ANNOUNCEMENT,ExaCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not remove an exam announcement");

   /***** Mark possible notifications as removed *****/
   Ntf_SetNotifAsRemoved (Ntf_EVENT_EXAM_ANNOUNCEMENT,ExaCod);

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
   Exa_ListExamAnnouncements (Exa_NORMAL_VIEW);

   /***** Mark possible notifications as seen *****/
   Ntf_SetNotifAsSeen (Ntf_EVENT_EXAM_ANNOUNCEMENT,
	               -1L,
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
/******************** List all the exam announcements ************************/
/*****************************************************************************/

static void Exa_ListExamAnnouncements (Exa_tTypeViewExamAnnouncement_t TypeViewExamAnnouncement)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_No_announcements_of_exams_of_X;
   extern const char *Txt_New_announcement_of_exam;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow,NumRows;
   long ExaCod;

   /***** Get exam announcements in current course from database *****/
   sprintf (Query,"SELECT ExaCod,ExamDate FROM exam_announcements"
                  " WHERE CrsCod='%ld' AND Status<>'%u' ORDER BY ExamDate",
            Gbl.CurrentCrs.Crs.CrsCod,(unsigned) Exa_DELETED_EXAM_ANNOUNCEMENT);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get exam announcements in this course for listing");

   /***** The result of the query may be empty *****/
   if (NumRows == 0)
     {
      sprintf (Gbl.Message,Txt_No_announcements_of_exams_of_X,
               Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);
     }

   /***** Create link for creation of a new exam announcement *****/
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_TEACHER:
      case Rol_ROLE_DEG_ADMIN:
      case Rol_ROLE_SUPERUSER:
         fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
         Act_FormStart (ActEdiExaAnn);
         Act_LinkFormSubmit (Txt_New_announcement_of_exam,The_ClassFormul[Gbl.Prefs.Theme]);
         Lay_PutSendIcon ("new",Txt_New_announcement_of_exam,Txt_New_announcement_of_exam);
         fprintf (Gbl.F.Out,"</form>"
	                    "</div>");
         break;
      default:
         break;
     }

   /***** List the existing exam announcements *****/
   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
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
      fprintf (Gbl.F.Out,"<br />");

      /***** Free memory of the exam announcement *****/
      Exa_FreeMemExamAnnouncement ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
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
   sprintf (Query,"INSERT INTO exam_announcements (CrsCod,Status,NumNotif,"
                  "CrsFullName,Year,ExamSession,CallDate,ExamDate,Duration,Place,"
                  "ExamMode,Structure,DocRequired,MatRequired,MatAllowed,OtherInfo)"
                  " VALUES ('%ld','%u','0','%s','%u','%s',NOW(),'%04u-%02u-%02u %02u:%02u:00','%02u:%02u:00','%s','%s','%s','%s','%s','%s','%s')",
            Gbl.CurrentCrs.Crs.CrsCod,(unsigned) Exa_ACTIVE_EXAM_ANNOUNCEMENT,
            Gbl.ExamAnnouncement.CrsFullName,Gbl.ExamAnnouncement.Year,Gbl.ExamAnnouncement.Session,
            Gbl.ExamAnnouncement.ExamDate.Year,Gbl.ExamAnnouncement.ExamDate.Month,Gbl.ExamAnnouncement.ExamDate.Day,
            Gbl.ExamAnnouncement.StartTime.Hour,Gbl.ExamAnnouncement.StartTime.Minute,
	    Gbl.ExamAnnouncement.Duration.Hour,Gbl.ExamAnnouncement.Duration.Minute,
            Gbl.ExamAnnouncement.Place,
	    Gbl.ExamAnnouncement.Mode,Gbl.ExamAnnouncement.Structure,
            Gbl.ExamAnnouncement.DocRequired,
            Gbl.ExamAnnouncement.MatRequired,Gbl.ExamAnnouncement.MatAllowed,
            Gbl.ExamAnnouncement.OtherInfo);
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
                  "ExamDate='%04u-%02u-%02u %02u:%02u:00',Duration='%02u:%02u:00',"
                  "Place='%s',ExamMode='%s',Structure='%s',DocRequired='%s',MatRequired='%s',MatAllowed='%s',OtherInfo='%s'" \
                  " WHERE ExaCod='%ld'",
            Gbl.ExamAnnouncement.CrsFullName,Gbl.ExamAnnouncement.Year,Gbl.ExamAnnouncement.Session,
            Gbl.ExamAnnouncement.ExamDate.Year,Gbl.ExamAnnouncement.ExamDate.Month,Gbl.ExamAnnouncement.ExamDate.Day,
            Gbl.ExamAnnouncement.StartTime.Hour,Gbl.ExamAnnouncement.StartTime.Minute,
	    Gbl.ExamAnnouncement.Duration.Hour,Gbl.ExamAnnouncement.Duration.Minute,
	    Gbl.ExamAnnouncement.Place,
            Gbl.ExamAnnouncement.Mode,Gbl.ExamAnnouncement.Structure,
            Gbl.ExamAnnouncement.DocRequired,
            Gbl.ExamAnnouncement.MatRequired,Gbl.ExamAnnouncement.MatAllowed,
            Gbl.ExamAnnouncement.OtherInfo,
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
   unsigned long NumRow,NumRows;
   long ExaCod;
   struct Date ExamDate;
   unsigned Hour,Minute,Second;

   if (Gbl.DB.DatabaseIsOpen)
     {
      /***** Get exam announcements in current course from database *****/
      sprintf (Query,"SELECT ExaCod,ExamDate FROM exam_announcements" \
		     " WHERE CrsCod='%ld' AND Status<>'%u' ORDER BY ExamDate",
	       Gbl.CurrentCrs.Crs.CrsCod,(unsigned) Exa_DELETED_EXAM_ANNOUNCEMENT);
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get exam announcements in this course");

      /***** The result of the query may be empty *****/
      Gbl.LstExamAnnouncements.Lst = NULL;
      Gbl.LstExamAnnouncements.NumExamAnnounc = 0;
      if (NumRows)
	{
	 /***** Allocate memory for the list *****/
	 if ((Gbl.LstExamAnnouncements.Lst = (struct Date *) calloc (NumRows,sizeof (struct Date))) == NULL)
	    Lay_ShowErrorAndExit ("Not enough memory to store dates of exam announcements.");

	 /***** Get the dates of the existing exam announcements *****/
	 for (NumRow = 0;
	      NumRow < NumRows;
	      NumRow++)
	   {
	    /***** Get the code of the exam announcement (row[0]) *****/
	    row = mysql_fetch_row (mysql_res);

	    if (sscanf (row[0],"%ld",&ExaCod) != 1)
	       Lay_ShowErrorAndExit ("Wrong code of exam announcement.");

	    /***** Read the date of the exam (row[1]) *****/
	    /* Date of exam (row[6]) */
	    if (sscanf (row[1],"%04u-%02u-%02u %02u:%02u:%02u",
			&ExamDate.Year,&ExamDate.Month,&ExamDate.Day,&Hour,&Minute,&Second) != 6)
	       Lay_ShowErrorAndExit ("Wrong date of exam.");

	    /***** Add exam announcement the list *****/
	    Gbl.LstExamAnnouncements.Lst[Gbl.LstExamAnnouncements.NumExamAnnounc].Year  = ExamDate.Year;
	    Gbl.LstExamAnnouncements.Lst[Gbl.LstExamAnnouncements.NumExamAnnounc].Month = ExamDate.Month;
	    Gbl.LstExamAnnouncements.Lst[Gbl.LstExamAnnouncements.NumExamAnnounc].Day   = ExamDate.Day;
	    Gbl.LstExamAnnouncements.NumExamAnnounc++;
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
   if (Gbl.LstExamAnnouncements.Lst)
     {
      free ((void *) Gbl.LstExamAnnouncements.Lst);
      Gbl.LstExamAnnouncements.Lst = NULL;
      Gbl.LstExamAnnouncements.NumExamAnnounc = 0;
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
   unsigned long NumRows;
   unsigned Hour,Minute,Second;

   /***** Get data of an exam announcement from database *****/
   sprintf (Query,"SELECT CrsCod,CrsFullName,Year,ExamSession,CallDate,ExamDate,Duration," \
                  "Place,ExamMode,Structure,DocRequired,MatRequired,MatAllowed,OtherInfo" \
                  " FROM exam_announcements WHERE ExaCod='%ld'",ExaCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get data of an exam announcement");

   /***** The result of the query must have one row *****/
   if (NumRows != 1)
      Lay_ShowErrorAndExit ("Error when getting data of an exam announcement.");

   /***** Get the data of the exam announcement *****/
   row = mysql_fetch_row (mysql_res);

   /* Code of the course in which the exam announcement is inserted (row[0]) */
   Gbl.ExamAnnouncement.CrsCod = Str_ConvertStrCodToLongCod (row[0]);

   /* Name of the course (row[1]) */
   strcpy (Gbl.ExamAnnouncement.CrsFullName,row[1]);

   /* Year (row[2]) */
   if (sscanf (row[2],"%u",&Gbl.ExamAnnouncement.Year) != 1)
      Lay_ShowErrorAndExit ("Wrong year.");

   /* Exam session (row[3]) */
   strcpy (Gbl.ExamAnnouncement.Session,row[3]);

   /* Date of exam announcement (row[4]) */
   if (sscanf (row[4],"%04u-%02u-%02u %02u:%02u:%02u",
               &Gbl.ExamAnnouncement.CallDate.Year,
               &Gbl.ExamAnnouncement.CallDate.Month,
               &Gbl.ExamAnnouncement.CallDate.Day,
               &Hour,&Minute,&Second) != 6)
      Lay_ShowErrorAndExit ("Wrong date of exam announcement.");

   /* Date of exam (row[5]) */
   if (sscanf (row[5],"%04u-%02u-%02u %02u:%02u:%02u",
   	       &Gbl.ExamAnnouncement.ExamDate.Year,&Gbl.ExamAnnouncement.ExamDate.Month,&Gbl.ExamAnnouncement.ExamDate.Day,
               &Gbl.ExamAnnouncement.StartTime.Hour,&Gbl.ExamAnnouncement.StartTime.Minute,&Second) != 6)
      Lay_ShowErrorAndExit ("Wrong date of exam.");

   /* Approximate duration (row[6]) */
   if (sscanf (row[6],"%02u:%02u:%02u",&Gbl.ExamAnnouncement.Duration.Hour,&Gbl.ExamAnnouncement.Duration.Minute,&Second) != 3)
      Lay_ShowErrorAndExit ("Wrong duration of exam.");

   /* Place (row[7]) */
   strncpy (Gbl.ExamAnnouncement.Place,row[7],Cns_MAX_BYTES_TEXT);
   Gbl.ExamAnnouncement.Place[Cns_MAX_BYTES_TEXT] = '\0';

   /* Exam mode (row[8]) */
   strncpy (Gbl.ExamAnnouncement.Mode,row[8],Cns_MAX_BYTES_TEXT);
   Gbl.ExamAnnouncement.Mode[Cns_MAX_BYTES_TEXT] = '\0';

   /* Structure (row[9]) */
   strncpy (Gbl.ExamAnnouncement.Structure,row[9],Cns_MAX_BYTES_TEXT);
   Gbl.ExamAnnouncement.Structure[Cns_MAX_BYTES_TEXT] = '\0';

   /* Documentation required (row[10]) */
   strncpy (Gbl.ExamAnnouncement.DocRequired,row[10],Cns_MAX_BYTES_TEXT);
   Gbl.ExamAnnouncement.DocRequired[Cns_MAX_BYTES_TEXT] = '\0';

   /* Material required (row[11]) */
   strncpy (Gbl.ExamAnnouncement.MatRequired,row[11],Cns_MAX_BYTES_TEXT);
   Gbl.ExamAnnouncement.MatRequired[Cns_MAX_BYTES_TEXT] = '\0';

   /* Material allowed (row[12]) */
   strncpy (Gbl.ExamAnnouncement.MatAllowed,row[12],Cns_MAX_BYTES_TEXT);
   Gbl.ExamAnnouncement.MatAllowed[Cns_MAX_BYTES_TEXT] = '\0';

   /* Other information for students (row[13]) */
   strncpy (Gbl.ExamAnnouncement.OtherInfo,row[13],Cns_MAX_BYTES_TEXT);
   Gbl.ExamAnnouncement.OtherInfo[Cns_MAX_BYTES_TEXT] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Show a form with the data of an exam announcement **************/
/*****************************************************************************/

static void Exa_ShowExamAnnouncement (long ExaCod,Exa_tTypeViewExamAnnouncement_t TypeViewExamAnnouncement)
  {
   extern const char *Txt_YEAR_OF_DEGREE[1+Deg_MAX_YEARS_PER_DEGREE];
   extern const char *The_ClassFormul[The_NUM_THEMES];
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
   extern const char *Txt_Edit_announcement_of_exam;
   extern const char *Txt_Edit;
   extern const char *Txt_Send_announcement_of_exam;
   extern const char *Txt_Remove_announcement_of_exam;
   extern const char *Txt_Remove;
   const char *StyleTitle  = "CONV_TIT";
   const char *StyleForm   = "CONV_NEG";
   const char *StyleNormal = "CONV";
   struct Institution Ins;
   char StrExamDate[Cns_MAX_LENGTH_DATE+1];
   unsigned Year,Hour,Minute;

   /***** Get data of institution of this degree *****/
   Ins.InsCod = Gbl.CurrentIns.Ins.InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);

   switch (TypeViewExamAnnouncement)
     {
      case Exa_NORMAL_VIEW:
         /***** Link to print view *****/
         fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
         Lay_PutLinkToPrintView1 (ActPrnExaAnn);
         Par_PutHiddenParamLong ("ExaCod",ExaCod);
         Lay_PutLinkToPrintView2 ();
         fprintf (Gbl.F.Out,"</div>");
	 break;
      case Exa_PRINT_VIEW:
         StyleTitle  = "CONV_TIT_IMPR";
         StyleForm   = "CONV_NEG_IMPR";
         StyleNormal = "CONV_IMPR";
         break;
      case Exa_FORM_VIEW:
         StyleForm = The_ClassFormul[Gbl.Prefs.Theme];
         /***** Start form *****/
         Act_FormStart (ActRcvExaAnn);
         if (ExaCod >= 0)
            Par_PutHiddenParamLong ("ExaCod",ExaCod);
         break;
     }

   /***** Start frame *****/
   Lay_StartRoundFrameTable10 ("500px",0,NULL);
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td style=\"text-align:center;\">" \
                      "<table style=\"width:100%%; padding:20px; border-spacing:3px;\">");

   /***** Institution logo *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td colspan=\"2\" style=\"text-align:center;\">");
   if (TypeViewExamAnnouncement == Exa_PRINT_VIEW)
      fprintf (Gbl.F.Out,"<span class=\"%s\">",StyleTitle);
   else
      fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"%s\">",
               Ins.WWW,StyleTitle);
   Ins_DrawInstitutionLogo (Ins.Logo,Ins.FullName,64,NULL);
   fprintf (Gbl.F.Out,"<br />%s%s" \
                      "</td>" \
	              "</tr>",
            Ins.FullName,
            TypeViewExamAnnouncement == Exa_PRINT_VIEW ? "</span>" :
 	                                                 "</a>");

   /***** Degree *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td colspan=\"2\" class=\"%s\""
	              " style=\"text-align:center;\">",
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
	              "<td colspan=\"2\" class=\"%s\""
	              " style=\"text-align:center;\">"
	              "&nbsp;<br />"
	              "<strong>%s</strong>"
	              "</td>" \
	              "</tr>",
            StyleNormal,Txt_EXAM_ANNOUNCEMENT);
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td colspan=\"2\" class=\"%s\""
	              " style=\"text-align:left;\">"
	              "&nbsp;"
	              "</td>" \
	              "</tr>",
	    StyleNormal);

   /***** Name of the course *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s\""
                      " style=\"text-align:left; vertical-align:top;\">",
            StyleForm,Txt_EXAM_ANNOUNCEMENT_Course,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"CrsName\" size=\"40\" maxlength=\"%u\" value=\"%s\" />",
               Cns_MAX_LENGTH_STRING,Gbl.ExamAnnouncement.CrsFullName);
     }
   else
      fprintf (Gbl.F.Out,"<strong>%s</strong>",Gbl.ExamAnnouncement.CrsFullName);
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Year (optional, 1º, 2º, 3º, 4º, 5º...) *****/
   fprintf (Gbl.F.Out,"<tr>" \
                      "<td class=\"%s\""
                      " style=\"text-align:right; vertical-align:top;\">"
                      "%s:"
                      "</td>" \
                      "<td class=\"%s\""
                      " style=\"text-align:left; vertical-align:top;\">",
            StyleForm,Txt_EXAM_ANNOUNCEMENT_Year,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      fprintf (Gbl.F.Out,"<select name=\"Year\">");
      for (Year = 0;
	   Year <= Deg_MAX_YEARS_PER_DEGREE;
	   Year++)
        {
	 fprintf (Gbl.F.Out,"<option");
	 if (Gbl.ExamAnnouncement.Year == Year)
            fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out," value=\"%u\">%s</option>",Year,Txt_YEAR_OF_DEGREE[Year]);
	}
      fprintf (Gbl.F.Out,"</select>");
     }
   else
      fprintf (Gbl.F.Out,"%s",Txt_YEAR_OF_DEGREE[Gbl.ExamAnnouncement.Year]);
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Exam session *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s\""
                      " style=\"text-align:left; vertical-align:top;\">",
            StyleForm,Txt_EXAM_ANNOUNCEMENT_Session,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ExamSession\" size=\"40\" maxlength=\"%u\" value=\"%s\" />",
               Cns_MAX_LENGTH_STRING,Gbl.ExamAnnouncement.Session);
   else
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncement.Session);
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Date of the exam *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "%s:"
	              "</td>",
            StyleForm,Txt_EXAM_ANNOUNCEMENT_Exam_date);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      fprintf (Gbl.F.Out,"<td style=\"text-align:left;\">");
      Dat_WriteFormDate (Gbl.ExamAnnouncement.ExamDate.Year < Gbl.Now.Date.Year ? Gbl.ExamAnnouncement.ExamDate.Year :
                                                                                  Gbl.Now.Date.Year,
                         Gbl.Now.Date.Year+1,
                         "ExamDay","ExamMonth","ExamYear",
                         &(Gbl.ExamAnnouncement.ExamDate),
                         false,false);
      fprintf (Gbl.F.Out,"</td>");
     }
   else
     {
      Dat_ConvDateToDateStr (&Gbl.ExamAnnouncement.ExamDate,StrExamDate);
      fprintf (Gbl.F.Out,"<td class=\"%s\""
	                 " style=\"text-align:left; vertical-align:top;\">"
	                 "%s"
	                 "</td>",
               StyleNormal,StrExamDate);
     }
   fprintf (Gbl.F.Out,"</tr>");

   /***** Start time *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s\""
                      " style=\"text-align:left; vertical-align:top;\">",
            StyleForm,Txt_EXAM_ANNOUNCEMENT_Start_time,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      fprintf (Gbl.F.Out,"<select name=\"ExamHour\"><option value=\"0\"");
      if (Gbl.ExamAnnouncement.StartTime.Hour == 0)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">-</option>");
      for (Hour = 7;
	   Hour <= 22;
	   Hour++)
        {
	 fprintf (Gbl.F.Out,"<option value=\"%u\"",Hour);
	 if (Gbl.ExamAnnouncement.StartTime.Hour == Hour)
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
	 if (Gbl.ExamAnnouncement.StartTime.Minute == Minute)
            fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out,">%02u &#39;</option>",Minute);
	}
      fprintf (Gbl.F.Out,"</select>");
     }
   else if (Gbl.ExamAnnouncement.StartTime.Hour)
      fprintf (Gbl.F.Out,"%2u:%02u %s",
               Gbl.ExamAnnouncement.StartTime.Hour,
               Gbl.ExamAnnouncement.StartTime.Minute,
               Txt_hours_ABBREVIATION);
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Approximate duration of the exam *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s\""
                      " style=\"text-align:left; vertical-align:top;\">",
            StyleForm,Txt_EXAM_ANNOUNCEMENT_Approximate_duration,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
     {
      fprintf (Gbl.F.Out,"<select name=\"DurationHour\">");
      for (Hour = 0;
	   Hour <= 8;
	   Hour++)
        {
	 fprintf (Gbl.F.Out,"<option value=\"%u\"",Hour);
	 if (Gbl.ExamAnnouncement.Duration.Hour == Hour)
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
	 if (Gbl.ExamAnnouncement.Duration.Minute == Minute)
            fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out,">%02u &#39;</option>",Minute);
	}
      fprintf (Gbl.F.Out,"</select>");
     }
   else if (Gbl.ExamAnnouncement.Duration.Hour ||
            Gbl.ExamAnnouncement.Duration.Minute)
     {
      if (Gbl.ExamAnnouncement.Duration.Hour)
        {
         if (Gbl.ExamAnnouncement.Duration.Minute)
            fprintf (Gbl.F.Out,"%u %s %u &#39;",
                     Gbl.ExamAnnouncement.Duration.Hour,
                     Txt_hours_ABBREVIATION,
                     Gbl.ExamAnnouncement.Duration.Minute);
         else
           {
            if (Gbl.ExamAnnouncement.Duration.Hour == 1)
               fprintf (Gbl.F.Out,"1 %s",Txt_hour);
            else
               fprintf (Gbl.F.Out,"%u %s",Gbl.ExamAnnouncement.Duration.Hour,Txt_hours);
           }
        }
      else if (Gbl.ExamAnnouncement.Duration.Minute)
        {
         if (Gbl.ExamAnnouncement.Duration.Minute == 1)
            fprintf (Gbl.F.Out,"1 %s",Txt_minute);
         else
            fprintf (Gbl.F.Out,"%u %s",Gbl.ExamAnnouncement.Duration.Minute,Txt_minutes);
        }
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Place where the exam will be made *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s\""
                      " style=\"text-align:left; vertical-align:top;\">",
            StyleForm,Txt_EXAM_ANNOUNCEMENT_Place_of_exam,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea name=\"Place\" cols=\"40\" rows=\"4\">%s</textarea>",
               Gbl.ExamAnnouncement.Place);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnnouncement.Place,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncement.Place);
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Exam mode *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s\""
                      " style=\"text-align:left; vertical-align:top;\">",
            StyleForm,Txt_EXAM_ANNOUNCEMENT_Mode,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea name=\"ExamMode\" cols=\"40\" rows=\"2\">%s</textarea>",
               Gbl.ExamAnnouncement.Mode);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnnouncement.Mode,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncement.Mode);
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Structure of the exam *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s\""
                      " style=\"text-align:left; vertical-align:top;\">",
            StyleForm,Txt_EXAM_ANNOUNCEMENT_Structure_of_the_exam,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea name=\"Structure\" cols=\"40\" rows=\"8\">%s</textarea>",
               Gbl.ExamAnnouncement.Structure);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnnouncement.Structure,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncement.Structure);
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Documentation required *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s\""
                      " style=\"text-align:left; vertical-align:top;\">",
            StyleForm,Txt_EXAM_ANNOUNCEMENT_Documentation_required,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea name=\"DocRequired\" cols=\"40\" rows=\"2\">%s</textarea>",
               Gbl.ExamAnnouncement.DocRequired);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnnouncement.DocRequired,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncement.DocRequired);
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Material required *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s\""
                      " style=\"text-align:left; vertical-align:top;\">",
            StyleForm,Txt_EXAM_ANNOUNCEMENT_Material_required,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea name=\"MatRequired\" cols=\"40\" rows=\"4\">%s</textarea>",
               Gbl.ExamAnnouncement.MatRequired);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnnouncement.MatRequired,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncement.MatRequired);
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Material allowed *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s\" style=\"text-align:right; vertical-align:top;\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s\""
                      " style=\"text-align:left; vertical-align:top;\">",
            StyleForm,Txt_EXAM_ANNOUNCEMENT_Material_allowed,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea name=\"MatAllowed\" cols=\"40\" rows=\"4\">%s</textarea>",
               Gbl.ExamAnnouncement.MatAllowed);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnnouncement.MatAllowed,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncement.MatAllowed);
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Other information to students *****/
   fprintf (Gbl.F.Out,"<tr>" \
	              "<td class=\"%s\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "%s:"
	              "</td>" \
                      "<td class=\"%s\""
                      " style=\"text-align:left; vertical-align:top;\">",
            StyleForm,Txt_EXAM_ANNOUNCEMENT_Other_information,
            StyleNormal);
   if (TypeViewExamAnnouncement == Exa_FORM_VIEW)
      fprintf (Gbl.F.Out,"<textarea name=\"OtherInfo\" cols=\"40\" rows=\"5\">%s</textarea>",
               Gbl.ExamAnnouncement.OtherInfo);
   else
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Gbl.ExamAnnouncement.OtherInfo,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      fprintf (Gbl.F.Out,"%s",Gbl.ExamAnnouncement.OtherInfo);
     }
   fprintf (Gbl.F.Out,"</td>" \
	              "</tr>");

   /***** Bottom space used for signatures, or links to edit / remove the exam announcement *****/
   switch (TypeViewExamAnnouncement)
     {
      case Exa_NORMAL_VIEW:
         switch (Gbl.Usrs.Me.LoggedRole)
           {
            case Rol_ROLE_TEACHER:
            case Rol_ROLE_DEG_ADMIN:
            case Rol_ROLE_SUPERUSER:
               /***** Create link to edit this exam announcement *****/
               fprintf (Gbl.F.Out,"<tr>" \
        	                  "<td style=\"text-align:left;\">");
               Act_FormStart (ActEdiExaAnn);
               Par_PutHiddenParamLong ("ExaCod",ExaCod);
               Act_LinkFormSubmit (Txt_Edit_announcement_of_exam,The_ClassFormul[Gbl.Prefs.Theme]);
               fprintf (Gbl.F.Out,"<img src=\"%s/edit16x16.gif\"" \
        	                  " alt=\"%s\" class=\"ICON16x16\" />" \
	   	                  " %s</a>",
                        Gbl.Prefs.IconsURL,Txt_Edit_announcement_of_exam,Txt_Edit);
               fprintf (Gbl.F.Out,"</form>" \
        	                  "</td>");

               /***** Create link to remove this exam announcement *****/
               fprintf (Gbl.F.Out,"<td style=\"text-align:right;\">");
               Act_FormStart (ActRemExaAnn);
               Par_PutHiddenParamLong ("ExaCod",ExaCod);
               Act_LinkFormSubmit (Txt_Remove_announcement_of_exam,The_ClassFormul[Gbl.Prefs.Theme]);
               fprintf (Gbl.F.Out,"<img src=\"%s/delon16x16.gif\"" \
        	                  " alt=\"%s\" class=\"ICON16x16\" />" \
   		                  " %s</a>",
   		        Gbl.Prefs.IconsURL,
   		        Txt_Remove_announcement_of_exam,
   		        Txt_Remove);
               fprintf (Gbl.F.Out,"</form>" \
        	                  "</td>" \
        	                  "</tr>");
               break;
            default:
               break;
           }
         break;
      case Exa_PRINT_VIEW:
         /* Bottom space used for signatures */
         fprintf (Gbl.F.Out,"<tr>" \
                            "<td colspan=\"2\" class=\"%s\""
                            " style=\"text-align:left;\">"
                            "&nbsp;<br />"
                            "&nbsp;<br />"
                            "&nbsp;<br />"
                            "&nbsp;<br />"
                            "&nbsp;"
                            "</td>" \
                            "</tr>",
                  StyleForm);
         break;
      default:
         break;
     }

   /***** End frame *****/
   fprintf (Gbl.F.Out,"</table>" \
	              "</td>" \
	              "</tr>");
   Lay_EndRoundFrameTable10 ();

   switch (TypeViewExamAnnouncement)
     {
      case Exa_NORMAL_VIEW:
	 break;
      case Exa_PRINT_VIEW:
         QR_ExamAnnnouncement ();
         break;
      case Exa_FORM_VIEW:
         Lay_PutSendButton (Txt_Send_announcement_of_exam);
         fprintf (Gbl.F.Out,"</form>");
         break;
     }
  }

/*****************************************************************************/
/*********** Get data for notification about an exam announcement ************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error
// NumChars must be > 3+Cns_MAX_LENGTH_DATE+9

void Exa_GetNotifExamAnnouncement (char *SummaryStr,char **ContentStr,long ExaCod,unsigned MaxChars,bool GetContent)
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
      Str_LimitLengthHTMLStr (Gbl.ExamAnnouncement.CrsFullName,MaxChars-(Cns_MAX_LENGTH_DATE+9));

   /* Date of exam */
   sprintf (SummaryStr,"%s, %02u/%02u/%04u, %2u:%02u %s",
            Gbl.ExamAnnouncement.CrsFullName,
            Gbl.ExamAnnouncement.ExamDate.Day,
            Gbl.ExamAnnouncement.ExamDate.Month,
            Gbl.ExamAnnouncement.ExamDate.Year,
            Gbl.ExamAnnouncement.StartTime.Hour,
            Gbl.ExamAnnouncement.StartTime.Minute,
            Txt_hours_ABBREVIATION);

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
   Crs.CrsCod = Gbl.ExamAnnouncement.CrsCod;
   Crs_GetDataOfCourseByCod (&Crs);

   /***** Get data of degree *****/
   Deg.DegCod = Crs.DegCod;
   Deg_GetDataOfDegreeByCod (&Deg);

   /***** Get data of institution *****/
   Ins.InsCod = Deg_GetInsCodOfDegreeByCod (Deg.DegCod);
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);

   Dat_ConvDateToDateStr (&Gbl.ExamAnnouncement.ExamDate,StrExamDate);

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
            Txt_EXAM_ANNOUNCEMENT_Course,Gbl.ExamAnnouncement.CrsFullName,
            Txt_EXAM_ANNOUNCEMENT_Year,Txt_YEAR_OF_DEGREE[Gbl.ExamAnnouncement.Year],
            Txt_EXAM_ANNOUNCEMENT_Session,Gbl.ExamAnnouncement.Session,
            Txt_EXAM_ANNOUNCEMENT_Exam_date,StrExamDate,
            Txt_EXAM_ANNOUNCEMENT_Start_time,Gbl.ExamAnnouncement.StartTime.Hour,
                                             Gbl.ExamAnnouncement.StartTime.Minute,
            Txt_hours_ABBREVIATION,
            Txt_EXAM_ANNOUNCEMENT_Approximate_duration,Gbl.ExamAnnouncement.Duration.Hour,
                                                       Gbl.ExamAnnouncement.Duration.Minute,
            Txt_hours_ABBREVIATION,
            Txt_EXAM_ANNOUNCEMENT_Place_of_exam,Gbl.ExamAnnouncement.Place,
            Txt_EXAM_ANNOUNCEMENT_Mode,Gbl.ExamAnnouncement.Mode,
            Txt_EXAM_ANNOUNCEMENT_Structure_of_the_exam,Gbl.ExamAnnouncement.Structure,
            Txt_EXAM_ANNOUNCEMENT_Documentation_required,Gbl.ExamAnnouncement.DocRequired,
            Txt_EXAM_ANNOUNCEMENT_Material_required,Gbl.ExamAnnouncement.MatRequired,
            Txt_EXAM_ANNOUNCEMENT_Material_allowed,Gbl.ExamAnnouncement.MatAllowed,
            Txt_EXAM_ANNOUNCEMENT_Other_information,Gbl.ExamAnnouncement.OtherInfo);
  }
