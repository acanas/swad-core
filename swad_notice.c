// swad_notice.c: notices (yellow notes)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For exit, system, malloc, calloc, free, etc.
#include <string.h>

#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_RSS.h"
#include "swad_social.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Not_MAX_CHARS_ON_NOTICE	40	// Maximum number of characters in notices (when not expanded)

const unsigned Not_ContainerWidth[Not_NUM_TYPES_LISTING] =
  {
   148,	// Not_LIST_BRIEF_NOTICES
   500,	// Not_LIST_FULL_NOTICES
  };

const unsigned Not_MaxCharsURLOnScreen[Not_NUM_TYPES_LISTING] =
  {
    15,	// Not_LIST_BRIEF_NOTICES
    50,	// Not_LIST_FULL_NOTICES
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static bool Not_CheckIfICanEditNotices (void);
static void Not_PutIconsListNotices (void);
static void Not_PutIconToAddNewNotice (void);
static void Not_PutButtonToAddNewNotice (void);
static void Not_GetDataAndShowNotice (long NotCod);
static void Not_DrawANotice (Not_Listing_t TypeNoticesListing,
                             long NotCod,
                             time_t TimeUTC,
                             const char *Content,
                             long UsrCod,
                             Not_Status_t Status);
static long Not_InsertNoticeInDB (const char *Content);
static void Not_UpdateNumUsrsNotifiedByEMailAboutNotice (long NotCod,unsigned NumUsrsToBeNotifiedByEMail);
static void Not_PutParams (void);
static long Not_GetParamNotCod (void);

/*****************************************************************************/
/***************************** Write a new notice ****************************/
/*****************************************************************************/

void Not_ShowFormNotice (void)
  {
   extern const char *Hlp_MESSAGES_Notices;
   extern const char *Txt_The_notice_will_appear_as_a_yellow_note_;
   extern const char *Txt_New_notice;
   extern const char *Txt_Create_notice;

   /***** Help message *****/
   snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	     Txt_The_notice_will_appear_as_a_yellow_note_,
             Gbl.CurrentCrs.Crs.FullName);
   Ale_ShowA_old (Ale_INFO,Gbl.Alert.Txt);

   /***** Start form *****/
   Frm_StartForm (ActRcvNot);

   /***** Start box *****/
   Box_StartBox (NULL,Txt_New_notice,NULL,
                 Hlp_MESSAGES_Notices,Box_NOT_CLOSABLE);

   /***** Message body *****/
   fprintf (Gbl.F.Out,"<textarea name=\"Content\" cols=\"30\" rows=\"10\""
	              " autofocus=\"autofocus\" required=\"required\">"
                      "</textarea>");

   /***** Send button and end box *****/
   Box_EndBoxWithButton (Btn_CREATE_BUTTON,Txt_Create_notice);

   /***** End form *****/
   Frm_EndForm ();

   /***** Show all notices *****/
   Not_ShowNotices (Not_LIST_FULL_NOTICES);
  }

/*****************************************************************************/
/******* Receive a new notice from a form and store it in database ***********/
/*****************************************************************************/

void Not_ReceiveNotice (void)
  {
   extern const char *Txt_Notice_created;
   long NotCod;
   unsigned NumUsrsToBeNotifiedByEMail;
   struct SocialPublishing SocPub;
   char Content[Cns_MAX_BYTES_TEXT + 1];

   /***** Get the text of the notice *****/
   Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_TEXT,
                              Str_TO_RIGOROUS_HTML,true);

   /***** Create a new notice in database *****/
   NotCod = Not_InsertNoticeInDB (Content);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.CurrentCrs.Crs);

   /***** Write message of success *****/
   Ale_ShowA_old (Ale_SUCCESS,Txt_Notice_created);

   /***** Notify by email about the new notice *****/
   if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_NOTICE,NotCod)))
      Not_UpdateNumUsrsNotifiedByEMailAboutNotice (NotCod,NumUsrsToBeNotifiedByEMail);

   /***** Create a new social note about the new notice *****/
   Soc_StoreAndPublishSocialNote (Soc_NOTE_NOTICE,NotCod,&SocPub);
  }

/*****************************************************************************/
/******************* Insert a notice in the table of notices *****************/
/*****************************************************************************/
// Return the code of the new inserted notice

static long Not_InsertNoticeInDB (const char *Content)
  {
   /***** Insert notice in the database *****/
   return
   DB_QueryINSERTandReturnCode ("can not create notice",
				"INSERT INTO notices"
				" (CrsCod,UsrCod,CreatTime,Content,Status)"
				" VALUES"
				" (%ld,%ld,NOW(),'%s',%u)",
				Gbl.CurrentCrs.Crs.CrsCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Content,(unsigned) Not_ACTIVE_NOTICE);
  }

/*****************************************************************************/
/*********** Update number of users notified in table of notices *************/
/*****************************************************************************/

static void Not_UpdateNumUsrsNotifiedByEMailAboutNotice (long NotCod,unsigned NumUsrsToBeNotifiedByEMail)
  {
   /***** Update number of users notified *****/
   DB_QueryUPDATE ("can not update the number of notifications of a notice",
		   "UPDATE notices SET NumNotif=%u WHERE NotCod=%ld",
	           NumUsrsToBeNotifiedByEMail,NotCod);
  }

/*****************************************************************************/
/******************* List notices after removing one of them *****************/
/*****************************************************************************/

void Not_ListNoticesAfterRemoval (void)
  {
   extern const char *Txt_Notice_removed;

   /***** Message of success *****/
   Ale_ShowA_old (Ale_SUCCESS,Txt_Notice_removed);

   /***** List remaining notices *****/
   Not_ListFullNotices ();
  }

/*****************************************************************************/
/******************************* List notices ********************************/
/*****************************************************************************/

void Not_ListFullNotices (void)
  {
   /***** Show highlighted notice *****/
   if (Gbl.CurrentCrs.Notices.HighlightNotCod > 0)
      Not_GetDataAndShowNotice (Gbl.CurrentCrs.Notices.HighlightNotCod);

   /***** Show all notices *****/
   Not_ShowNotices (Not_LIST_FULL_NOTICES);
  }

/*****************************************************************************/
/***************** Mark as hidden a notice that was active *******************/
/*****************************************************************************/

void Not_HideActiveNotice (void)
  {
   long NotCod;

   /***** Get the code of the notice to hide *****/
   NotCod = Not_GetParamNotCod ();

   /***** Set notice as hidden *****/
   DB_QueryUPDATE ("can not hide notice",
		   "UPDATE notices SET Status=%u"
		   " WHERE NotCod=%ld AND CrsCod=%ld",
	           (unsigned) Not_OBSOLETE_NOTICE,
	           NotCod,Gbl.CurrentCrs.Crs.CrsCod);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.CurrentCrs.Crs);
  }

/*****************************************************************************/
/****************** Mark as active a notice that was hidden ******************/
/*****************************************************************************/

void Not_RevealHiddenNotice (void)
  {
   long NotCod;

   /***** Get the code of the notice to reveal *****/
   NotCod = Not_GetParamNotCod ();

   /***** Set notice as active *****/
   DB_QueryUPDATE ("can not reveal notice",
		   "UPDATE notices SET Status=%u"
		   " WHERE NotCod=%ld AND CrsCod=%ld",
	           (unsigned) Not_ACTIVE_NOTICE,
	           NotCod,Gbl.CurrentCrs.Crs.CrsCod);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.CurrentCrs.Crs);
  }

/*****************************************************************************/
/********************* Request the removal of a notice ***********************/
/*****************************************************************************/

void Not_RequestRemNotice (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_following_notice;
   extern const char *Txt_Remove;

   /***** Get the code of the notice to remove *****/
   Gbl.CurrentCrs.Notices.NotCod = Not_GetParamNotCod ();

   /***** Show question and button to remove this notice *****/
   /* Start alert */
   Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_following_notice);

   /* Show notice */
   Not_GetDataAndShowNotice (Gbl.CurrentCrs.Notices.NotCod);

   /* End alert */
   Ale_ShowAlertAndButton2 (ActRemNot,NULL,NULL,Not_PutParams,
			    Btn_REMOVE_BUTTON,Txt_Remove);

   /***** Show all notices *****/
   Not_ShowNotices (Not_LIST_FULL_NOTICES);
  }

/*****************************************************************************/
/******************************* Remove a notice *****************************/
/*****************************************************************************/

void Not_RemoveNotice (void)
  {
   long NotCod;

   /***** Get the code of the notice to remove *****/
   NotCod = Not_GetParamNotCod ();

   /***** Remove notice *****/
   /* Copy notice to table of deleted notices */
   DB_QueryINSERT ("can not remove notice",
		   "INSERT IGNORE INTO notices_deleted"
		   " (NotCod,CrsCod,UsrCod,CreatTime,Content,NumNotif)"
		   " SELECT NotCod,CrsCod,UsrCod,CreatTime,Content,NumNotif"
		   " FROM notices"
		   " WHERE NotCod=%ld AND CrsCod=%ld",
                   NotCod,Gbl.CurrentCrs.Crs.CrsCod);

   /* Remove notice */
   DB_QueryDELETE ("can not remove notice",
		   "DELETE FROM notices"
		   " WHERE NotCod=%ld AND CrsCod=%ld",
                   NotCod,Gbl.CurrentCrs.Crs.CrsCod);

   /***** Mark possible notifications as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_NOTICE,NotCod);

   /***** Mark possible social note as unavailable *****/
   Soc_MarkSocialNoteAsUnavailableUsingNoteTypeAndCod (Soc_NOTE_NOTICE,NotCod);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.CurrentCrs.Crs);
  }

/*****************************************************************************/
/********************* Get notice to show highlighted ************************/
/*****************************************************************************/

void Not_GetNotCodToHighlight (void)
  {
   /***** Get the code of the notice to highlight *****/
   Gbl.CurrentCrs.Notices.HighlightNotCod = Not_GetParamNotCod ();
  }

/*****************************************************************************/
/***************************** Show the notices ******************************/
/*****************************************************************************/

void Not_ShowNotices (Not_Listing_t TypeNoticesListing)
  {
   extern const char *Hlp_MESSAGES_Notices;
   extern const char *Txt_All_notices;
   extern const char *Txt_Notices;
   extern const char *Txt_No_notices;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char StrWidth[10 + 2 + 1];
   char PathRelRSSFile[PATH_MAX + 1];
   long NotCod;
   unsigned long NumNot;
   unsigned long NumNotices = 0;	// Initialized to avoid warning
   char Content[Cns_MAX_BYTES_TEXT + 1];
   time_t TimeUTC;
   long UsrCod;
   unsigned UnsignedNum;
   Not_Status_t Status;

   /***** A course must be selected (Gbl.CurrentCrs.Crs.CrsCod > 0) *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
     {
      /***** Get notices from database *****/
      switch (TypeNoticesListing)
	{
	 case Not_LIST_BRIEF_NOTICES:
	    NumNotices = DB_QuerySELECT (&mysql_res,"can not get notices from database",
					 "SELECT NotCod,"
					        "UNIX_TIMESTAMP(CreatTime) AS F,"
					        "UsrCod,"
					        "Content,"
					        "Status"
					 " FROM notices"
					 " WHERE CrsCod=%ld AND Status=%u"
					 " ORDER BY CreatTime DESC",
					 Gbl.CurrentCrs.Crs.CrsCod,
					 (unsigned) Not_ACTIVE_NOTICE);
	    break;
	 case Not_LIST_FULL_NOTICES:
	    NumNotices = DB_QuerySELECT (&mysql_res,"can not get notices from database",
					 "SELECT NotCod,"
					        "UNIX_TIMESTAMP(CreatTime) AS F,"
					        "UsrCod,"
					        "Content,"
					        "Status"
					 " FROM notices"
					 " WHERE CrsCod=%ld"
					 " ORDER BY CreatTime DESC",
					 Gbl.CurrentCrs.Crs.CrsCod);
	    break;
	}

      if (TypeNoticesListing == Not_LIST_FULL_NOTICES)
	{
	 /***** Start box *****/
	 snprintf (StrWidth,sizeof (StrWidth),
	           "%upx",
	           Not_ContainerWidth[Not_LIST_FULL_NOTICES] + 50);
	 Box_StartBox (StrWidth,
	               Gbl.CurrentCrs.Notices.HighlightNotCod > 0 ? Txt_All_notices :
	                	                                    Txt_Notices,
		       Not_PutIconsListNotices,
		       Hlp_MESSAGES_Notices,Box_NOT_CLOSABLE);
         if (!NumNotices)
	    Ale_ShowA_old (Ale_INFO,Txt_No_notices);
	}

      /***** Show the notices *****/
      for (NumNot = 0;
	   NumNot < NumNotices;
	   NumNot++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get notice code (row[0]) */
	 if (sscanf (row[0],"%ld",&NotCod) != 1)
	    Lay_ShowErrorAndExit ("Wrong code of notice.");

	 /* Get creation time (row[1] holds the UTC date-time) */
	 TimeUTC = Dat_GetUNIXTimeFromStr (row[1]);

	 /* Get user code (row[2]) */
	 UsrCod = Str_ConvertStrCodToLongCod (row[2]);

	 /* Get the content (row[3]) and insert links */
	 Str_Copy (Content,row[3],
	           Cns_MAX_BYTES_TEXT);
	 Str_InsertLinks (Content,Cns_MAX_BYTES_TEXT,
	                  Not_MaxCharsURLOnScreen[TypeNoticesListing]);
	 if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
            Str_LimitLengthHTMLStr (Content,Not_MAX_CHARS_ON_NOTICE);

	 /* Get status of the notice (row[4]) */
	 Status = Not_OBSOLETE_NOTICE;
	 if (sscanf (row[4],"%u",&UnsignedNum) == 1)
	    if (UnsignedNum < Not_NUM_STATUS)
	      Status = (Not_Status_t) UnsignedNum;

	 /* Draw the notice */
	 Not_DrawANotice (TypeNoticesListing,
	                  NotCod,
	                  TimeUTC,Content,UsrCod,Status);
	}

      switch (TypeNoticesListing)
        {
	 case Not_LIST_BRIEF_NOTICES:
            /***** Link to RSS file *****/
	    /* Create RSS file if not exists */
	    snprintf (PathRelRSSFile,sizeof (PathRelRSSFile),
		      "%s/%s/%ld/%s/%s",
		      Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CRS,
		      Gbl.CurrentCrs.Crs.CrsCod,Cfg_RSS_FOLDER,Cfg_RSS_FILE);
	    if (!Fil_CheckIfPathExists (PathRelRSSFile))
	       RSS_UpdateRSSFileForACrs (&Gbl.CurrentCrs.Crs);

	    /* Put a link to the RSS file */
	    fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">"
			       "<a href=\"");
	    RSS_WriteRSSLink (Gbl.F.Out,Gbl.CurrentCrs.Crs.CrsCod);
	    fprintf (Gbl.F.Out,"\" target=\"_blank\">"
			       "<img src=\"%s/rss-square.svg\""
			       " alt=\"RSS\" title=\"RSS\""
			       " class=\"ICO16x16\" />"
			       "</a>"
			       "</div>",
		     Gbl.Prefs.URLIcons);
	    break;
	 case Not_LIST_FULL_NOTICES:
	    /***** Button to add new notice *****/
	    if (Not_CheckIfICanEditNotices ())
	       Not_PutButtonToAddNewNotice ();

	    /***** End box *****/
	    Box_EndBox ();
	    break;
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Mark possible notification as seen *****/
      Ntf_MarkNotifAsSeen (Ntf_EVENT_NOTICE,
	                   -1L,Gbl.CurrentCrs.Crs.CrsCod,
	                   Gbl.Usrs.Me.UsrDat.UsrCod);
     }
  }

/*****************************************************************************/
/*********************** Check if I can edit notices *************************/
/*****************************************************************************/

static bool Not_CheckIfICanEditNotices (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
	          Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);
  }

/*****************************************************************************/
/****************** Put contextual icons in list of notices ******************/
/*****************************************************************************/

static void Not_PutIconsListNotices (void)
  {
   /***** Put icon to add a new notice *****/
   if (Not_CheckIfICanEditNotices ())
      Not_PutIconToAddNewNotice ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_NOTICES;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/*********************** Put icon to add a new notice ************************/
/*****************************************************************************/

static void Not_PutIconToAddNewNotice (void)
  {
   extern const char *Txt_New_notice;

   Ico_PutContextualIconToAdd (ActWriNot,NULL,NULL,
			       Txt_New_notice);
  }

/*****************************************************************************/
/********************** Put button to add a new notice ***********************/
/*****************************************************************************/

static void Not_PutButtonToAddNewNotice (void)
  {
   extern const char *Txt_New_notice;

   Frm_StartForm (ActWriNot);
   Btn_PutConfirmButton (Txt_New_notice);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Get data of a notice and show it ***********************/
/*****************************************************************************/

static void Not_GetDataAndShowNotice (long NotCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char Content[Cns_MAX_BYTES_TEXT + 1];
   time_t TimeUTC;
   long UsrCod;
   unsigned UnsignedNum;
   Not_Status_t Status;

   /***** Get notice data from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get notice from database",
		       "SELECT UNIX_TIMESTAMP(CreatTime) AS F,"
			      "UsrCod,"
			      "Content,"
			      "Status"
		       " FROM notices"
		       " WHERE NotCod=%ld AND CrsCod=%ld",
		       NotCod,Gbl.CurrentCrs.Crs.CrsCod))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get creation time (row[0] holds the UTC date-time) */
      TimeUTC = Dat_GetUNIXTimeFromStr (row[0]);

      /* Get user code (row[1]) */
      UsrCod = Str_ConvertStrCodToLongCod (row[1]);

      /* Get the content (row[2]) and insert links*/
      Str_Copy (Content,row[2],
                Cns_MAX_BYTES_TEXT);
      Str_InsertLinks (Content,Cns_MAX_BYTES_TEXT,
		       Not_MaxCharsURLOnScreen[Not_LIST_FULL_NOTICES]);

      /* Get status of the notice (row[3]) */
      Status = Not_OBSOLETE_NOTICE;
      if (sscanf (row[3],"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < Not_NUM_STATUS)
	   Status = (Not_Status_t) UnsignedNum;

      /***** Draw the notice *****/
      Not_DrawANotice (Not_LIST_FULL_NOTICES,
		       NotCod,
		       TimeUTC,Content,UsrCod,Status);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Draw a notice as a yellow note ************************/
/*****************************************************************************/

static void Not_DrawANotice (Not_Listing_t TypeNoticesListing,
                             long NotCod,
                             time_t TimeUTC,
                             const char *Content,
                             long UsrCod,
                             Not_Status_t Status)
  {
   extern const char *Txt_NOTICE_Active_SINGULAR;
   extern const char *Txt_NOTICE_Active_Mark_as_obsolete;
   extern const char *Txt_NOTICE_Obsolete_SINGULAR;
   extern const char *Txt_NOTICE_Obsolete_Mark_as_active;
   extern const char *Txt_See_full_notice;
   extern const char *Txt_Today;
   static const char *ContainerClass[Not_NUM_STATUS] =
     {
      "NOTICE_CONTAINER_ACTIVE",	// Not_ACTIVE_NOTICE
      "NOTICE_CONTAINER_OBSOLETE",	// Not_OBSOLETE_NOTICE
     };
   static const char *DateClass[Not_NUM_STATUS] =
     {
      "NOTICE_DATE_ACTIVE",		// Not_ACTIVE_NOTICE
      "NOTICE_DATE_OBSOLETE",		// Not_OBSOLETE_NOTICE
     };
   static const char *TextClass[Not_NUM_STATUS] =
     {
      "NOTICE_TEXT_ACTIVE",		// Not_ACTIVE_NOTICE
      "NOTICE_TEXT_OBSOLETE",		// Not_OBSOLETE_NOTICE
     };
   static const char *AuthorClass[Not_NUM_STATUS] =
     {
      "NOTICE_AUTHOR_ACTIVE",		// Not_ACTIVE_NOTICE
      "NOTICE_AUTHOR_OBSOLETE",		// Not_OBSOLETE_NOTICE
     };
   static unsigned UniqueId = 0;
   struct UsrData UsrDat;

   Gbl.CurrentCrs.Notices.NotCod = NotCod;	// Parameter for forms

   /***** Start yellow note *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"width:%upx;\">",
	    ContainerClass[Status],
	    Not_ContainerWidth[TypeNoticesListing]);

   /***** Write the date in the top part of the yellow note *****/
   /* Write symbol to indicate if notice is obsolete or active */
   if (TypeNoticesListing == Not_LIST_FULL_NOTICES)
     {
      if (Not_CheckIfICanEditNotices ())
	{
	 /***** Put form to remove announcement *****/
         Ico_PutContextualIconToRemove (ActReqRemNot,Not_PutParams);

	 /***** Put form to change the status of the notice *****/
         switch (Status)
           {
            case Not_ACTIVE_NOTICE:
	       Lay_PutContextualLinkOnlyIcon (ActHidNot,NULL,Not_PutParams,
					      "eye.svg",
					      Txt_NOTICE_Active_Mark_as_obsolete);
               break;
            case Not_OBSOLETE_NOTICE:
	       Lay_PutContextualLinkOnlyIcon (ActRevNot,NULL,Not_PutParams,
					      "eye-slash.svg",
					      Txt_NOTICE_Obsolete_Mark_as_active);
               break;
           }
         Frm_EndForm ();
   	}
      else	// Don't put forms
	 /* Status of the notice */
         switch (Status)
           {
            case Not_ACTIVE_NOTICE:
               fprintf (Gbl.F.Out,"<span title=\"%s\">"
                                  "<img src=\"%s/eye.svg\""
                                  " alt=\"%s\" title=\"%s\""
                                  " class=\"ICO_HIDDEN ICO16x16\" />"
                                  "</span>",
                        Txt_NOTICE_Active_SINGULAR,
                        Gbl.Prefs.URLIcons,
                        Txt_NOTICE_Active_SINGULAR,
                        Txt_NOTICE_Active_SINGULAR);
               break;
            case Not_OBSOLETE_NOTICE:
               fprintf (Gbl.F.Out,"<span title=\"%s\">"
                                  "<img src=\"%s/eye-slash.svg\""
                                  " alt=\"%s\" title=\"%s\""
                                  " class=\"ICO_HIDDEN ICO16x16\" />"
                                  "</span>",
                        Txt_NOTICE_Obsolete_SINGULAR,
                        Gbl.Prefs.URLIcons,
                        Txt_NOTICE_Obsolete_SINGULAR,
                        Txt_NOTICE_Obsolete_SINGULAR);
               break;
           }
     }

   /* Write the date */
   UniqueId++;
   fprintf (Gbl.F.Out,"<div class=\"%s\">",
            DateClass[Status]);
   if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
     {
      /* Form to view full notice */
      Frm_StartForm (ActSeeOneNot);
      Not_PutHiddenParamNotCod (NotCod);
      Frm_LinkFormSubmit (Txt_See_full_notice,DateClass[Status],NULL);
     }
   fprintf (Gbl.F.Out,"<span id=\"not_date_%u\"></span>",
            UniqueId);
   if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
     {
      fprintf (Gbl.F.Out,"</a>");
      Frm_EndForm ();
     }
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('not_date_%u',%ld,"
                      "%u,'<br />','%s',true,false,0x6);"
                      "</script>"
	              "</div>",
	    UniqueId,(long) TimeUTC,
	    (unsigned) Gbl.Prefs.DateFormat,Txt_Today);

   /***** Write the content of the notice *****/
   if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
     {
      fprintf (Gbl.F.Out,"<div class=\"NOTICE_TEXT_BRIEF\">%s</div>",Content);

      /* Put form to view full notice */
      fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");
      Lay_PutContextualLinkOnlyIcon (ActSeeOneNot,NULL,Not_PutParams,
				     "ellipsis-h.svg",
				     Txt_See_full_notice);
      fprintf (Gbl.F.Out,"</div>");
     }
   else
      fprintf (Gbl.F.Out,"<div class=\"%s\">%s</div>",
	       TextClass[Status],Content);

   /***** Write the author *****/
   fprintf (Gbl.F.Out,"<div class=\"NOTICE_AUTHOR %s\">",	// Limited width
            AuthorClass[Status]);
   Usr_UsrDataConstructor (&UsrDat);
   UsrDat.UsrCod = UsrCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat)) // Get from the database the data of the autor
      Usr_WriteFirstNameBRSurnames (&UsrDat);
   Usr_UsrDataDestructor (&UsrDat);
   fprintf (Gbl.F.Out,"</div>");

   /***** End yellow note *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/******************* Get summary and content for a notice ********************/
/*****************************************************************************/

void Not_GetSummaryAndContentNotice (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                     char **ContentStr,
                                     long NotCod,bool GetContent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   size_t Length;

   SummaryStr[0] = '\0';	// Return nothing on error
   // This function may be called inside a web service, so don't report error

   /***** Get subject of message from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get content of notice",
		       "SELECT Content FROM notices WHERE NotCod=%ld",
		       NotCod) == 1)	// Result should have a unique row
     {
      /***** Get sumary / content *****/
      row = mysql_fetch_row (mysql_res);

      /***** Copy summary *****/
      // TODO: Do only direct copy when a Subject of type VARCHAR(255) is available
      if (strlen (row[0]) > Ntf_MAX_BYTES_SUMMARY)
	{
	 strncpy (SummaryStr,row[0],
		  Ntf_MAX_BYTES_SUMMARY);
	 SummaryStr[Ntf_MAX_BYTES_SUMMARY] = '\0';
	}
      else
	 Str_Copy (SummaryStr,row[0],
		   Ntf_MAX_BYTES_SUMMARY);

      /***** Copy content *****/
      if (GetContent)
	{
	 Length = strlen (row[0]);
	 if ((*ContentStr = (char *) malloc (Length + 1)) == NULL)
	    Lay_ShowErrorAndExit ("Error allocating memory for notification content.");
	 Str_Copy (*ContentStr,row[0],
		   Length);
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************************** Get number of notices ***************************/
/*****************************************************************************/
// Returns the number of (active or obsolete) notices
// sent from this location (all the platform, current degree or current course)

unsigned Not_GetNumNotices (Sco_Scope_t Scope,Not_Status_t Status,unsigned *NumNotif)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumNotices;

   /***** Get number of notices from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of notices",
			 "SELECT COUNT(*),SUM(NumNotif)"
			 " FROM notices"
			 " WHERE Status=%u",
                         Status);
         break;
      case Sco_SCOPE_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of notices",
			 "SELECT COUNT(*),SUM(notices.NumNotif)"
			 " FROM institutions,centres,degrees,courses,notices"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=notices.CrsCod"
			 " AND notices.Status=%u",
                         Gbl.CurrentCty.Cty.CtyCod,
                         Status);
         break;
      case Sco_SCOPE_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of notices",
			 "SELECT COUNT(*),SUM(notices.NumNotif)"
			 " FROM centres,degrees,courses,notices"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=notices.CrsCod"
			 " AND notices.Status=%u",
                         Gbl.CurrentIns.Ins.InsCod,
                         Status);
         break;
      case Sco_SCOPE_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of notices",
			 "SELECT COUNT(*),SUM(notices.NumNotif)"
			 " FROM degrees,courses,notices"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=notices.CrsCod"
			 " AND notices.Status=%u",
                         Gbl.CurrentCtr.Ctr.CtrCod,
                         Status);
         break;
      case Sco_SCOPE_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of notices",
			 "SELECT COUNT(*),SUM(notices.NumNotif)"
			 " FROM courses,notices"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=notices.CrsCod"
			 " AND notices.Status=%u",
                         Gbl.CurrentDeg.Deg.DegCod,
                         Status);
         break;
      case Sco_SCOPE_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of notices",
			 "SELECT COUNT(*),SUM(NumNotif)"
			 " FROM notices"
			 " WHERE CrsCod=%ld"
			 " AND Status=%u",
                         Gbl.CurrentCrs.Crs.CrsCod,
                         Status);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of notices *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumNotices) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of notices.");

   /***** Get number of notifications by email *****/
   if (row[1])
     {
      if (sscanf (row[1],"%u",NumNotif) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of notifications of notices.");
     }
   else
      *NumNotif = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumNotices;
  }

/*****************************************************************************/
/************************ Get number of deleted notices **********************/
/*****************************************************************************/
// Returns the number of deleted notices
// sent from this location (all the platform, current degree or current course)

unsigned Not_GetNumNoticesDeleted (Sco_Scope_t Scope,unsigned *NumNotif)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumNotices;

   /***** Get number of notices from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of deleted notices",
			 "SELECT COUNT(*),SUM(NumNotif)"
			 " FROM notices_deleted");
         break;
      case Sco_SCOPE_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of deleted notices",
			 "SELECT COUNT(*),SUM(notices_deleted.NumNotif)"
			 " FROM institutions,centres,degrees,courses,notices_deleted"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=notices_deleted.CrsCod",
                         Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of deleted notices",
			 "SELECT COUNT(*),SUM(notices_deleted.NumNotif)"
			 " FROM centres,degrees,courses,notices_deleted"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=notices_deleted.CrsCod",
                         Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of deleted notices",
			 "SELECT COUNT(*),SUM(notices_deleted.NumNotif)"
			 " FROM degrees,courses,notices_deleted"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=notices_deleted.CrsCod",
                         Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of deleted notices",
			 "SELECT COUNT(*),SUM(notices_deleted.NumNotif)"
			 " FROM courses,notices_deleted"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=notices_deleted.CrsCod",
                         Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of deleted notices",
			 "SELECT COUNT(*),SUM(NumNotif)"
			 " FROM notices_deleted"
			 " WHERE CrsCod=%ld",
                         Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of notices *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumNotices) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of deleted notices.");

   /***** Get number of notifications by email *****/
   if (row[1])
     {
      if (sscanf (row[1],"%u",NumNotif) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of notifications of deleted notices.");
     }
   else
      *NumNotif = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumNotices;
  }

/*****************************************************************************/
/*************** Put parameter with the code of a notice *********************/
/*****************************************************************************/

static void Not_PutParams (void)
  {
   Not_PutHiddenParamNotCod (Gbl.CurrentCrs.Notices.NotCod);
  }

/*****************************************************************************/
/*************** Put parameter with the code of a notice *********************/
/*****************************************************************************/

void Not_PutHiddenParamNotCod (long NotCod)
  {
   Par_PutHiddenParamLong ("NotCod",NotCod);
  }

/*****************************************************************************/
/*************** Get parameter with the code of a notice *********************/
/*****************************************************************************/

static long Not_GetParamNotCod (void)
  {
   long NotCod;

   /***** Get notice code *****/
   if ((NotCod = Par_GetParToLong ("NotCod")) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of notice.");

   return NotCod;
  }
