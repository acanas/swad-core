// swad_notice.c: notices (yellow notes)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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
#include <stddef.h>		// For NULL
#include <stdio.h>		// For sscanf, asprintf, etc.
#include <stdlib.h>		// For exit, system, malloc, calloc, free, etc.
#include <string.h>

#include "swad_box.h"
#include "swad_database.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_RSS.h"
#include "swad_timeline.h"
#include "swad_timeline_note.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Not_MAX_CHARS_ON_NOTICE	40	// Maximum number of characters in notices (when not expanded)

static const unsigned Not_ContainerWidth[Not_NUM_TYPES_LISTING] =
  {
   [Not_LIST_BRIEF_NOTICES] = 148 + 50,
   [Not_LIST_FULL_NOTICES ] = 500 + 50,
  };

static const unsigned Not_MaxCharsURLOnScreen[Not_NUM_TYPES_LISTING] =
  {
   [Not_LIST_BRIEF_NOTICES] = 15,
   [Not_LIST_FULL_NOTICES ] = 50,
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static bool Not_CheckIfICanEditNotices (void);
static void Not_PutIconsListNotices (__attribute__((unused)) void *Args);
static void Not_PutIconToAddNewNotice (void);
static void Not_PutButtonToAddNewNotice (void);
static void Not_GetDataAndShowNotice (long NotCod);
static void Not_DrawANotice (Not_Listing_t TypeNoticesListing,
                             long NotCod,bool Highlight,
                             time_t TimeUTC,
                             const char *Content,
                             long UsrCod,
                             Not_Status_t Status);
static long Not_InsertNoticeInDB (const char *Content);
static void Not_UpdateNumUsrsNotifiedByEMailAboutNotice (long NotCod,unsigned NumUsrsToBeNotifiedByEMail);
static void Not_PutParams (void *NotCod);
static long Not_GetParamNotCod (void);

/*****************************************************************************/
/***************************** Write a new notice ****************************/
/*****************************************************************************/

void Not_ShowFormNotice (void)
  {
   extern const char *Hlp_COMMUNICATION_Notices;
   extern const char *Txt_The_notice_will_appear_as_a_yellow_note_;
   extern const char *Txt_New_notice;
   extern const char *Txt_Create_notice;

   /***** Help message *****/
   Ale_ShowAlert (Ale_INFO,Txt_The_notice_will_appear_as_a_yellow_note_,
                  Gbl.Hierarchy.Crs.FullName);

   /***** Begin form *****/
   Frm_StartForm (ActRcvNot);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_New_notice,
                 NULL,NULL,
                 Hlp_COMMUNICATION_Notices,Box_NOT_CLOSABLE);

   /***** Message body *****/
   HTM_TEXTAREA_Begin ("name=\"Content\" cols=\"30\" rows=\"10\""
	               " autofocus=\"autofocus\" required=\"required\"");
   HTM_TEXTAREA_End ();

   /***** Send button and end box *****/
   Box_BoxWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_notice);

   /***** End form *****/
   Frm_EndForm ();

   /***** Show all notices *****/
   Not_ShowNotices (Not_LIST_FULL_NOTICES,
	            -1L);	// No notice highlighted
  }

/*****************************************************************************/
/******* Receive a new notice from a form and store it in database ***********/
/*****************************************************************************/

void Not_ReceiveNotice (void)
  {
   extern const char *Txt_Notice_created;
   long NotCod;
   unsigned NumUsrsToBeNotifiedByEMail;
   char Content[Cns_MAX_BYTES_TEXT + 1];

   /***** Get the text of the notice *****/
   Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_TEXT,
                              Str_TO_RIGOROUS_HTML,true);

   /***** Create a new notice in database *****/
   NotCod = Not_InsertNoticeInDB (Content);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.Hierarchy.Crs);

   /***** Write message of success *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Notice_created);

   /***** Notify by email about the new notice *****/
   if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_NOTICE,NotCod)))
      Not_UpdateNumUsrsNotifiedByEMailAboutNotice (NotCod,NumUsrsToBeNotifiedByEMail);

   /***** Create a new social note about the new notice *****/
   TL_Not_StoreAndPublishNote (TL_NOTE_NOTICE,NotCod);

   /***** Set notice to be highlighted *****/
   Gbl.Crs.Notices.HighlightNotCod = NotCod;
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
				Gbl.Hierarchy.Crs.CrsCod,
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
   Ale_ShowAlert (Ale_SUCCESS,Txt_Notice_removed);

   /***** List remaining notices *****/
   Not_ListFullNotices ();
  }

/*****************************************************************************/
/******************************* List notices ********************************/
/*****************************************************************************/

void Not_ListFullNotices (void)
  {
   /***** Show all notices *****/
   Not_ShowNotices (Not_LIST_FULL_NOTICES,
	            Gbl.Crs.Notices.HighlightNotCod);	// Highlight notice
  }

/*****************************************************************************/
/************************* Get highlighted notice code ***********************/
/*****************************************************************************/

void Not_GetHighLightedNotCod (void)
  {
   /***** Get notice to be highlighted *****/
   Gbl.Crs.Notices.HighlightNotCod = Not_GetParamNotCod ();
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
	           NotCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.Hierarchy.Crs);

   /***** Set notice to be highlighted *****/
   Gbl.Crs.Notices.HighlightNotCod = NotCod;
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
	           NotCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.Hierarchy.Crs);

   /***** Set notice to be highlighted *****/
   Gbl.Crs.Notices.HighlightNotCod = NotCod;
  }

/*****************************************************************************/
/********************* Request the removal of a notice ***********************/
/*****************************************************************************/

void Not_RequestRemNotice (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_following_notice;
   extern const char *Txt_Remove;
   long NotCod;

   /***** Get the code of the notice to remove *****/
   NotCod = Not_GetParamNotCod ();

   /***** Show question and button to remove this notice *****/
   /* Start alert */
   Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_following_notice);

   /* Show notice */
   Not_GetDataAndShowNotice (NotCod);

   /* End alert */
   Ale_ShowAlertAndButton2 (ActRemNot,NULL,NULL,
                            Not_PutParams,&NotCod,
			    Btn_REMOVE_BUTTON,Txt_Remove);

   /***** Show all notices *****/
   Not_ShowNotices (Not_LIST_FULL_NOTICES,
	            NotCod);	// Highlight notice to be removed
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
                   NotCod,Gbl.Hierarchy.Crs.CrsCod);

   /* Remove notice */
   DB_QueryDELETE ("can not remove notice",
		   "DELETE FROM notices"
		   " WHERE NotCod=%ld AND CrsCod=%ld",
                   NotCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Mark possible notifications as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_NOTICE,NotCod);

   /***** Mark possible social note as unavailable *****/
   TL_Not_MarkNoteAsUnavailable (TL_NOTE_NOTICE,NotCod);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.Hierarchy.Crs);
  }

/*****************************************************************************/
/***************************** Show the notices ******************************/
/*****************************************************************************/

void Not_ShowNotices (Not_Listing_t TypeNoticesListing,long HighlightNotCod)
  {
   extern const char *Hlp_COMMUNICATION_Notices;
   extern const char *Txt_Notices;
   extern const char *Txt_No_notices;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char StrWidth[Cns_MAX_DECIMAL_DIGITS_UINT + 2 + 1];
   char PathRelRSSFile[PATH_MAX + 1];
   long NotCod;
   unsigned long NumNot;
   unsigned long NumNotices = 0;	// Initialized to avoid warning
   char Content[Cns_MAX_BYTES_TEXT + 1];
   time_t TimeUTC;
   long UsrCod;
   unsigned UnsignedNum;
   Not_Status_t Status;
   char RSSLink[Cns_MAX_BYTES_WWW + 1];

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Level != Hie_Lvl_CRS)	// No course selected
      return;

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
				      Gbl.Hierarchy.Crs.CrsCod,
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
				      Gbl.Hierarchy.Crs.CrsCod);
	 break;
     }

   if (TypeNoticesListing == Not_LIST_FULL_NOTICES)
     {
      /***** Begin box *****/
      snprintf (StrWidth,sizeof (StrWidth),
		"%upx",
		Not_ContainerWidth[Not_LIST_FULL_NOTICES] + 50);
      Box_BoxBegin (StrWidth,Txt_Notices,
		    Not_PutIconsListNotices,NULL,
		    Hlp_COMMUNICATION_Notices,Box_NOT_CLOSABLE);
      if (!NumNotices)
	 Ale_ShowAlert (Ale_INFO,Txt_No_notices);
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

      /* Inserting links is incompatible with limiting the length
	 ==> don't insert links when limiting length */
      switch (TypeNoticesListing)
        {
	 case Not_LIST_BRIEF_NOTICES:
	    Str_LimitLengthHTMLStr (Content,Not_MAX_CHARS_ON_NOTICE);
	    break;
	 case Not_LIST_FULL_NOTICES:
	    Str_InsertLinks (Content,Cns_MAX_BYTES_TEXT,
			     Not_MaxCharsURLOnScreen[TypeNoticesListing]);
	    break;
        }

      /* Get status of the notice (row[4]) */
      Status = Not_OBSOLETE_NOTICE;
      if (sscanf (row[4],"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < Not_NUM_STATUS)
	   Status = (Not_Status_t) UnsignedNum;

      /* Draw the notice */
      Not_DrawANotice (TypeNoticesListing,
		       NotCod,
		       (NotCod == HighlightNotCod),	// Highlighted?
		       TimeUTC,Content,UsrCod,Status);
     }

   switch (TypeNoticesListing)
     {
      case Not_LIST_BRIEF_NOTICES:
	 /***** Link to RSS file *****/
	 /* Create RSS file if not exists */
	 snprintf (PathRelRSSFile,sizeof (PathRelRSSFile),
		   "%s/%ld/%s/%s",
		   Cfg_PATH_CRS_PUBLIC,
		   Gbl.Hierarchy.Crs.CrsCod,Cfg_RSS_FOLDER,Cfg_RSS_FILE);
	 if (!Fil_CheckIfPathExists (PathRelRSSFile))
	    RSS_UpdateRSSFileForACrs (&Gbl.Hierarchy.Crs);

	 /* Put a link to the RSS file */
	 HTM_DIV_Begin ("class=\"CM\"");
	 RSS_BuildRSSLink (RSSLink,Gbl.Hierarchy.Crs.CrsCod);
	 HTM_A_Begin ("href=\"%s\" target=\"_blank\"",RSSLink);
	 Ico_PutIcon ("rss-square.svg","RSS","ICO16x16");
	 HTM_A_End ();
	 HTM_DIV_End ();
	 break;
      case Not_LIST_FULL_NOTICES:
	 /***** Button to add new notice *****/
	 if (Not_CheckIfICanEditNotices ())
	    Not_PutButtonToAddNewNotice ();

	 /***** End box *****/
	 Box_BoxEnd ();
	 break;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Mark possible notification as seen *****/
   Ntf_MarkNotifAsSeen (Ntf_EVENT_NOTICE,
			-1L,Gbl.Hierarchy.Crs.CrsCod,
			Gbl.Usrs.Me.UsrDat.UsrCod);
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

static void Not_PutIconsListNotices (__attribute__((unused)) void *Args)
  {
   /***** Put icon to add a new notice *****/
   if (Not_CheckIfICanEditNotices ())
      Not_PutIconToAddNewNotice ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_NOTICES);
  }

/*****************************************************************************/
/*********************** Put icon to add a new notice ************************/
/*****************************************************************************/

static void Not_PutIconToAddNewNotice (void)
  {
   extern const char *Txt_New_notice;

   Ico_PutContextualIconToAdd (ActWriNot,NULL,
                               NULL,NULL,
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
		       NotCod,Gbl.Hierarchy.Crs.CrsCod))
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
		       false,	// Not highlighted
		       TimeUTC,Content,UsrCod,Status);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Draw a notice as a yellow note ************************/
/*****************************************************************************/

static void Not_DrawANotice (Not_Listing_t TypeNoticesListing,
                             long NotCod,bool Highlight,
                             time_t TimeUTC,
                             const char *Content,
                             long UsrCod,
                             Not_Status_t Status)
  {
   extern const char *Txt_See_full_notice;
   static const char *ContainerClass[Not_NUM_STATUS] =
     {
      [Not_ACTIVE_NOTICE  ] = "NOTICE_CONT_ACTIVE",
      [Not_OBSOLETE_NOTICE] = "NOTICE_CONT_OBSOLETE",
     };
   static const char *ContainerWidthClass[Not_NUM_TYPES_LISTING] =
     {
      [Not_LIST_BRIEF_NOTICES] = "NOTICE_CONT_NARROW",
      [Not_LIST_FULL_NOTICES ] = "NOTICE_CONT_WIDE",
     };
   static const char *DateClass[Not_NUM_STATUS] =
     {
      [Not_ACTIVE_NOTICE  ] = "NOTICE_DATE_ACTIVE",
      [Not_OBSOLETE_NOTICE] = "NOTICE_DATE_OBSOLETE",
     };
   static const char *TextClass[Not_NUM_STATUS] =
     {
      [Not_ACTIVE_NOTICE  ] = "NOTICE_TEXT_ACTIVE",
      [Not_OBSOLETE_NOTICE] = "NOTICE_TEXT_OBSOLETE",
     };
   static const char *AuthorClass[Not_NUM_STATUS] =
     {
      [Not_ACTIVE_NOTICE  ] = "NOTICE_AUTHOR_ACTIVE",
      [Not_OBSOLETE_NOTICE] = "NOTICE_AUTHOR_OBSOLETE",
     };
   static unsigned UniqueId = 0;
   char *Id;
   struct UsrData UsrDat;
   char *Anchor = NULL;

   /***** Build anchor string *****/
   Frm_SetAnchorStr (NotCod,&Anchor);

   /***** Begin article for this notice *****/
   if (TypeNoticesListing == Not_LIST_FULL_NOTICES)
     {
      HTM_ARTICLE_Begin (Anchor);
      if (Highlight)
	 HTM_DIV_Begin ("class=\"NOTICE_HIGHLIGHT\"");
     }

   /***** Start yellow note *****/
   HTM_DIV_Begin ("class=\"%s %s\"",
	          ContainerClass[Status],ContainerWidthClass[TypeNoticesListing]);

   /***** Write the date in the top part of the yellow note *****/
   /* Write symbol to indicate if notice is obsolete or active */
   if (TypeNoticesListing == Not_LIST_FULL_NOTICES)
      if (Not_CheckIfICanEditNotices ())
	{
	 /***** Put form to remove announcement *****/
         Ico_PutContextualIconToRemove (ActReqRemNot,NULL,
                                        Not_PutParams,&NotCod);

	 /***** Put form to change the status of the notice *****/
         switch (Status)
           {
            case Not_ACTIVE_NOTICE:
	       Ico_PutContextualIconToHide (ActHidNot,NULL,
	                                    Not_PutParams,&NotCod);
               break;
            case Not_OBSOLETE_NOTICE:
	       Ico_PutContextualIconToUnhide (ActRevNot,NULL,
	                                      Not_PutParams,&NotCod);
               break;
           }
         Frm_EndForm ();
   	}

   /* Write the date */
   UniqueId++;
   HTM_DIV_Begin ("class=\"%s\"",DateClass[Status]);
   if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
     {
      /* Form to view full notice */
      Frm_StartFormAnchor (ActSeeOneNot,Anchor);
      Not_PutHiddenParamNotCod (NotCod);
      HTM_BUTTON_SUBMIT_Begin (Txt_See_full_notice,"BT_LINK RT",NULL);
     }
   if (asprintf (&Id,"not_date_%u",UniqueId) < 0)
      Lay_NotEnoughMemoryExit ();
   HTM_SPAN_Begin ("id=\"%s\"",Id);
   HTM_SPAN_End ();
   if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
     {
      HTM_BUTTON_End ();
      Frm_EndForm ();
     }
   Dat_WriteLocalDateHMSFromUTC (Id,TimeUTC,
				 Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				 true,true,false,0x6);
   free (Id);
   HTM_DIV_End ();

   /***** Write the content of the notice *****/
   if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
     {
      HTM_DIV_Begin ("class=\"NOTICE_TEXT_BRIEF\"");
      HTM_Txt (Content);
      HTM_DIV_End ();

      /* Put form to view full notice */
      HTM_DIV_Begin ("class=\"CM\"");
      Lay_PutContextualLinkOnlyIcon (ActSeeOneNot,Anchor,
                                     Not_PutParams,&NotCod,
				     "ellipsis-h.svg",
				     Txt_See_full_notice);
      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"%s\"",TextClass[Status]);
      HTM_Txt (Content);
      HTM_DIV_End ();
     }

   /***** Write the author *****/
   HTM_DIV_Begin ("class=\"NOTICE_AUTHOR %s\"",	// Limited width
                  AuthorClass[Status]);
   Usr_UsrDataConstructor (&UsrDat);
   UsrDat.UsrCod = UsrCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS)) // Get from the database the data of the autor
      Usr_WriteFirstNameBRSurnames (&UsrDat);
   Usr_UsrDataDestructor (&UsrDat);
   HTM_DIV_End ();

   /***** End yellow note *****/
   HTM_DIV_End ();

   /***** End article for this notice *****/
   if (TypeNoticesListing == Not_LIST_FULL_NOTICES)
     {
      if (Highlight)
	 HTM_DIV_End ();
      HTM_ARTICLE_End ();
     }

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);
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
	 /* Inserting links is incompatible with limiting the length
	    ==> don't insert links */
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

unsigned Not_GetNumNotices (Hie_Lvl_Level_t Scope,Not_Status_t Status,unsigned *NumNotif)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumNotices;

   /***** Get number of notices from database *****/
   switch (Scope)
     {
      case Hie_Lvl_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of notices",
			 "SELECT COUNT(*),SUM(NumNotif)"
			 " FROM notices"
			 " WHERE Status=%u",
                         Status);
         break;
      case Hie_Lvl_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of notices",
			 "SELECT COUNT(*),SUM(notices.NumNotif)"
			 " FROM institutions,centres,degrees,courses,notices"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=notices.CrsCod"
			 " AND notices.Status=%u",
                         Gbl.Hierarchy.Cty.CtyCod,
                         Status);
         break;
      case Hie_Lvl_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of notices",
			 "SELECT COUNT(*),SUM(notices.NumNotif)"
			 " FROM centres,degrees,courses,notices"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=notices.CrsCod"
			 " AND notices.Status=%u",
                         Gbl.Hierarchy.Ins.InsCod,
                         Status);
         break;
      case Hie_Lvl_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of notices",
			 "SELECT COUNT(*),SUM(notices.NumNotif)"
			 " FROM degrees,courses,notices"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=notices.CrsCod"
			 " AND notices.Status=%u",
                         Gbl.Hierarchy.Ctr.CtrCod,
                         Status);
         break;
      case Hie_Lvl_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of notices",
			 "SELECT COUNT(*),SUM(notices.NumNotif)"
			 " FROM courses,notices"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=notices.CrsCod"
			 " AND notices.Status=%u",
                         Gbl.Hierarchy.Deg.DegCod,
                         Status);
         break;
      case Hie_Lvl_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of notices",
			 "SELECT COUNT(*),SUM(NumNotif)"
			 " FROM notices"
			 " WHERE CrsCod=%ld"
			 " AND Status=%u",
                         Gbl.Hierarchy.Crs.CrsCod,
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

unsigned Not_GetNumNoticesDeleted (Hie_Lvl_Level_t Scope,unsigned *NumNotif)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumNotices;

   /***** Get number of notices from database *****/
   switch (Scope)
     {
      case Hie_Lvl_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of deleted notices",
			 "SELECT COUNT(*),SUM(NumNotif)"
			 " FROM notices_deleted");
         break;
      case Hie_Lvl_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of deleted notices",
			 "SELECT COUNT(*),SUM(notices_deleted.NumNotif)"
			 " FROM institutions,centres,degrees,courses,notices_deleted"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=notices_deleted.CrsCod",
                         Gbl.Hierarchy.Cty.CtyCod);
         break;
      case Hie_Lvl_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of deleted notices",
			 "SELECT COUNT(*),SUM(notices_deleted.NumNotif)"
			 " FROM centres,degrees,courses,notices_deleted"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=notices_deleted.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_Lvl_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of deleted notices",
			 "SELECT COUNT(*),SUM(notices_deleted.NumNotif)"
			 " FROM degrees,courses,notices_deleted"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=notices_deleted.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_Lvl_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of deleted notices",
			 "SELECT COUNT(*),SUM(notices_deleted.NumNotif)"
			 " FROM courses,notices_deleted"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=notices_deleted.CrsCod",
                         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_Lvl_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of deleted notices",
			 "SELECT COUNT(*),SUM(NumNotif)"
			 " FROM notices_deleted"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
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

static void Not_PutParams (void *NotCod)
  {
   if (NotCod)
      Not_PutHiddenParamNotCod (*((long *) NotCod));
  }

/*****************************************************************************/
/*************** Put parameter with the code of a notice *********************/
/*****************************************************************************/

void Not_PutHiddenParamNotCod (long NotCod)
  {
   Par_PutHiddenParamLong (NULL,"NotCod",NotCod);
  }

/*****************************************************************************/
/*************** Get parameter with the code of a notice *********************/
/*****************************************************************************/

static long Not_GetParamNotCod (void)
  {
   /***** Get notice code *****/
   return Par_GetParToLong ("NotCod");
  }
