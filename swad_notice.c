// swad_notice.c: notices (yellow notes)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
#include <stdlib.h>		// For free
#include <string.h>

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hidden_visible.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_notice.h"
#include "swad_notice_database.h"
#include "swad_notification.h"
#include "swad_notification_database.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_RSS.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Not_MAX_CHARS_ON_NOTICE	40	// Maximum number of characters in notices (when not expanded)

static unsigned Not_MaxCharsURLOnScreen[Not_NUM_TYPES_LISTING] =
  {
   [Not_LIST_BRIEF_NOTICES] = 15,
   [Not_LIST_FULL_NOTICES ] = 50,
  };

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/* Notice code of the notice to be highlighted */
static long Not_HighlightNotCod = -1L;	// No notice highlighted

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Not_SetHighlightNotCod (long NotCod);
static long Not_GetHighlightNotCod (void);

static void Not_HideUnhideNotice (HidVis_HiddenOrVisible_t HiddenOrVisible);

static void Not_PutLinkToRSSFile (void);

static Usr_Can_t Not_CheckIfICanEditNotices (void);
static void Not_PutIconsListNotices (__attribute__((unused)) void *Args);
static void Not_PutIconToAddNewNotice (void);
static void Not_GetDataAndShowNotice (long NotCod);
static void Not_GetNoticeDataFromRow (MYSQL_RES *mysql_res,
                                      struct Not_Notice *Notice,
                                      Not_Listing_t TypeNoticesListing);
static void Not_DrawANotice (Not_Listing_t TypeNoticesListing,
                             struct Not_Notice *Notice,
                             Lay_Highlight_t Highlight);

static void Not_PutParNotCod (void *NotCod);

/*****************************************************************************/
/*********** Set/get notice code of the notice to be highlighted *************/
/*****************************************************************************/

static void Not_SetHighlightNotCod (long NotCod)
  {
   Not_HighlightNotCod = NotCod;
  }

static long Not_GetHighlightNotCod (void)
  {
   return Not_HighlightNotCod;
  }

/*****************************************************************************/
/***************************** Write a new notice ****************************/
/*****************************************************************************/

void Not_ShowFormNotice (void)
  {
   extern const char *Hlp_COMMUNICATION_Notices;
   extern const char *Txt_The_notice_will_appear_as_a_yellow_note_;
   extern const char *Txt_Notice;

   /***** Help message *****/
   Ale_ShowAlert (Ale_INFO,Txt_The_notice_will_appear_as_a_yellow_note_,
                  Gbl.Hierarchy.Node[Hie_CRS].FullName);

   /***** Begin form *****/
   Frm_BeginForm (ActNewNot);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Notice,NULL,NULL,
		    Hlp_COMMUNICATION_Notices,Box_NOT_CLOSABLE);

	 /***** Message body *****/
	 HTM_TEXTAREA_Begin (HTM_REQUIRED | HTM_AUTOFOCUS,
			     "name=\"Content\" cols=\"30\" rows=\"10\""
			     " class=\"INPUT_%s\"",The_GetSuffix ());
	 HTM_TEXTAREA_End ();

      /***** Send button and end box *****/
      Box_BoxWithButtonEnd (Btn_CREATE);

   /***** End form *****/
   Frm_EndForm ();

   /***** Show all notices *****/
   HTM_BR ();
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
                              Str_TO_RIGOROUS_HTML,Str_REMOVE_SPACES);

   /***** Create a new notice in database *****/
   NotCod = Not_DB_InsertNotice (Content);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.Hierarchy.Node[Hie_CRS]);

   /***** Write message of success *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Notice_created);

   /***** Notify by email about the new notice *****/
   if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_NOTICE,NotCod)))
      Not_DB_UpdateNumUsrsNotifiedByEMailAboutNotice (NotCod,NumUsrsToBeNotifiedByEMail);

   /***** Create a new social note about the new notice *****/
   TmlNot_StoreAndPublishNote (TmlNot_NOTICE,NotCod);

   /***** Set notice to be highlighted *****/
   Not_SetHighlightNotCod (NotCod);
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
	            Not_GetHighlightNotCod ());	// Highlight notice
  }

/*****************************************************************************/
/************************* Get highlighted notice code ***********************/
/*****************************************************************************/

void Not_GetParHighlightNotCod (void)
  {
   Not_SetHighlightNotCod (ParCod_GetPar (ParCod_Not));
  }

/*****************************************************************************/
/**************************** Hide/unhide a notice ***************************/
/*****************************************************************************/

void Not_HideNotice (void)
  {
   Not_HideUnhideNotice (HidVis_HIDDEN);
  }

void Not_UnhideNotice (void)
  {
   Not_HideUnhideNotice (HidVis_VISIBLE);
  }

static void Not_HideUnhideNotice (HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   long NotCod;

   /***** Get the code of the notice to hide/unhide *****/
   NotCod = ParCod_GetAndCheckPar (ParCod_Not);

   /***** Set notice as hidden/visible *****/
   Not_DB_ChangeNoticeStatus (NotCod,HiddenOrVisible);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.Hierarchy.Node[Hie_CRS]);

   /***** Set notice to be highlighted *****/
   Not_SetHighlightNotCod (NotCod);
  }

/*****************************************************************************/
/********************* Request the removal of a notice ***********************/
/*****************************************************************************/

void Not_ReqRemNotice (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_following_notice;
   long NotCod;

   /***** Get the code of the notice to remove *****/
   NotCod = ParCod_GetAndCheckPar (ParCod_Not);

   /***** Show question and button to remove this notice *****/
   /* Begin alert */
   Ale_ShowAlertAndButtonBegin (Ale_QUESTION,
				Txt_Do_you_really_want_to_remove_the_following_notice);

      /* Show notice */
      Not_GetDataAndShowNotice (NotCod);

   /* End alert */
   Ale_ShowAlertAndButtonEnd (ActRemNot,NULL,NULL,
                              Not_PutParNotCod,&NotCod,
			      Btn_REMOVE);

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
   NotCod = ParCod_GetAndCheckPar (ParCod_Not);

   /***** Remove notice *****/
   /* Copy notice to table of deleted notices */
   Not_DB_CopyNoticeToDeleted (NotCod);

   /* Remove notice */
   Not_DB_RemoveNotice (NotCod);

   /***** Mark possible notifications as removed *****/
   Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_NOTICE,NotCod);

   /***** Mark possible social note as unavailable *****/
   Tml_DB_MarkNoteAsUnavailable (TmlNot_NOTICE,NotCod);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.Hierarchy.Node[Hie_CRS]);
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
   // char StrWidth[Cns_MAX_DIGITS_UINT + 2 + 1];
   struct Not_Notice Notice;
   unsigned NumNot;
   unsigned NumNotices = 0;	// Initialized to avoid warning

   /***** Trivial check *****/
   if (Gbl.Hierarchy.HieLvl != Hie_CRS)	// No course selected
      return;

   switch (TypeNoticesListing)
     {
      case Not_LIST_BRIEF_NOTICES:
	 /***** Get notices from database *****/
	 NumNotices = Not_DB_GetActiveNotices (&mysql_res,
					       Gbl.Hierarchy.Node[Hie_CRS].HieCod);
	 break;
      case Not_LIST_FULL_NOTICES:
	 /***** Get notices from database *****/
	 NumNotices = Not_DB_GetAllNotices (&mysql_res);

	 /***** Begin box *****/
	 Box_BoxBegin (Txt_Notices,Not_PutIconsListNotices,NULL,
		       Hlp_COMMUNICATION_Notices,Box_NOT_CLOSABLE);
	    if (!NumNotices)
	       Ale_ShowAlert (Ale_INFO,Txt_No_notices);
	 break;
     }

   /***** Show notices *****/
   for (NumNot = 0;
	NumNot < NumNotices;
	NumNot++)
     {
      /* Get notice data */
      Not_GetNoticeDataFromRow (mysql_res,&Notice,TypeNoticesListing);

      /* Draw notice */
      Not_DrawANotice (TypeNoticesListing,&Notice,
		       (Notice.NotCod == HighlightNotCod) ? Lay_HIGHLIGHT :
							    Lay_NO_HIGHLIGHT);
     }

   switch (TypeNoticesListing)
     {
      case Not_LIST_BRIEF_NOTICES:
	 /***** Link to RSS file *****/
	 Not_PutLinkToRSSFile ();
	 break;
      case Not_LIST_FULL_NOTICES:
	 /***** End box *****/
	 Box_BoxEnd ();
	 break;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Mark possible notification as seen *****/
   Ntf_DB_MarkNotifsInCrsAsSeen (Ntf_EVENT_NOTICE);
  }

/*****************************************************************************/
/****************************** Link to RSS file *****************************/
/*****************************************************************************/

static void Not_PutLinkToRSSFile (void)
  {
   char PathRelRSSFile[PATH_MAX + 1];
   char RSSLink[WWW_MAX_BYTES_WWW + 1];

   /***** Create RSS file if not exists *****/
   snprintf (PathRelRSSFile,sizeof (PathRelRSSFile),"%s/%ld/%s/%s",
	     Cfg_PATH_CRS_PUBLIC,
	     Gbl.Hierarchy.Node[Hie_CRS].HieCod,Cfg_RSS_FOLDER,Cfg_RSS_FILE);
   if (Fil_CheckIfPathExists (PathRelRSSFile) == Exi_DOES_NOT_EXIST)
      RSS_UpdateRSSFileForACrs (&Gbl.Hierarchy.Node[Hie_CRS]);

   /***** Put a link to the RSS file *****/
   HTM_DIV_Begin ("class=\"CM\"");
      RSS_BuildRSSLink (RSSLink,Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      HTM_A_Begin ("href=\"%s\" target=\"_blank\"",RSSLink);
	 Ico_PutIcon ("rss-square.svg",Ico_BLACK,"RSS","ICO16x16");
      HTM_A_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*********************** Check if I can edit notices *************************/
/*****************************************************************************/

static Usr_Can_t Not_CheckIfICanEditNotices (void)
  {
   return (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
	   Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM) ? Usr_CAN :
						     Usr_CAN_NOT;
  }

/*****************************************************************************/
/****************** Put contextual icons in list of notices ******************/
/*****************************************************************************/

static void Not_PutIconsListNotices (__attribute__((unused)) void *Args)
  {
   /***** Put icon to add a new notice *****/
   if (Not_CheckIfICanEditNotices () == Usr_CAN)
      Not_PutIconToAddNewNotice ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_NOTICES);
  }

/*****************************************************************************/
/*********************** Put icon to add a new notice ************************/
/*****************************************************************************/

static void Not_PutIconToAddNewNotice (void)
  {
   Ico_PutContextualIconToAdd (ActWriNot,NULL,NULL,NULL);
  }

/*****************************************************************************/
/******************** Get data of a notice and show it ***********************/
/*****************************************************************************/

static void Not_GetDataAndShowNotice (long NotCod)
  {
   MYSQL_RES *mysql_res;
   struct Not_Notice Notice;

   /***** Get notice data from database *****/
   if (Not_DB_GetNoticeData (&mysql_res,NotCod) == Exi_EXISTS)
     {
      Not_GetNoticeDataFromRow (mysql_res,&Notice,Not_LIST_FULL_NOTICES);

      /***** Draw the notice *****/
      Not_DrawANotice (Not_LIST_FULL_NOTICES,&Notice,Lay_NO_HIGHLIGHT);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Get notice data from database row ***********************/
/*****************************************************************************/

static void Not_GetNoticeDataFromRow (MYSQL_RES *mysql_res,
                                      struct Not_Notice *Notice,
                                      Not_Listing_t TypeNoticesListing)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);
   /*
   NotCod				row[0]
   UNIX_TIMESTAMP(CreatTime) AS F	row[1]
   UsrCod				row[2]
   Content				row[3]
   Status				row[4]
   */
   /***** Get notice code (row[0]) *****/
   if (sscanf (row[0],"%ld",&Notice->NotCod) != 1)
      Err_WrongNoticeExit ();

   /***** Get creation time (row[1] holds the UTC date-time) *****/
   Notice->CreatTime = Dat_GetUNIXTimeFromStr (row[1]);

   /***** Get user code (row[2]) *****/
   Notice->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get the content (row[3]) and insert links *****/
   Str_Copy (Notice->Content,row[3],sizeof (Notice->Content) - 1);

   /* Inserting links is incompatible with limiting the length
      ==> don't insert links when limiting length */
   switch (TypeNoticesListing)
     {
      case Not_LIST_BRIEF_NOTICES:
	 Str_LimitLengthHTMLStr (Notice->Content,Not_MAX_CHARS_ON_NOTICE);
	 break;
      case Not_LIST_FULL_NOTICES:
	 ALn_InsertLinks (Notice->Content,Cns_MAX_BYTES_TEXT,
			  Not_MaxCharsURLOnScreen[Not_LIST_FULL_NOTICES]);
	 break;
     }

   /***** Get status of the notice (row[4]) *****/
   Notice->HiddenOrVisible = HidVis_GetHiddenFrom01 (row[4][0]);	// 1 ==> hidden
  }

/*****************************************************************************/
/********************* Draw a notice as a yellow note ************************/
/*****************************************************************************/

static void Not_DrawANotice (Not_Listing_t TypeNoticesListing,
                             struct Not_Notice *Notice,
                             Lay_Highlight_t Highlight)
  {
   extern const char *Txt_See_full_notice;
   static Act_Action_t ActionHideUnhide[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] = ActUnhNot,	// Hidden ==> action to unhide
      [HidVis_VISIBLE] = ActHidNot,	// Visible ==> action to hide
     };
   static const char *ContainerClass[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] = "NOTICE_BOX LIGHT",
      [HidVis_VISIBLE] = "NOTICE_BOX",
     };
   static const char *ContainerWidthClass[Not_NUM_TYPES_LISTING] =
     {
      [Not_LIST_BRIEF_NOTICES] = "NOTICE_BOX_NARROW",
      [Not_LIST_FULL_NOTICES ] = "NOTICE_BOX_WIDE",
     };
   static const char *ContainerHighLightClass[Lay_NUM_HIGHLIGHT] =
     {
      [Lay_NO_HIGHLIGHT] = "",
      [Lay_HIGHLIGHT   ] = " NOTICE_HIGHLIGHT",
     };
   static unsigned UniqueId = 0;
   char *Id;
   struct Usr_Data UsrDat;
   char *Anchor = NULL;

   /***** Build anchor string *****/
   Frm_SetAnchorStr (Notice->NotCod,&Anchor);

   /***** Begin article for this notice *****/
   if (TypeNoticesListing == Not_LIST_FULL_NOTICES)
     {
      HTM_ARTICLE_Begin (Anchor);
         HTM_DIV_Begin ("class=\"NOTICE_CONT%s\"",
			ContainerHighLightClass[Highlight]);
     }

   /***** Begin yellow note *****/
   HTM_DIV_Begin ("class=\"%s %s\"",
	          ContainerClass[Notice->HiddenOrVisible],
	          ContainerWidthClass[TypeNoticesListing]);

      /***** Write the date in the top part of the yellow note *****/
      /* Write symbol to indicate if notice is obsolete or active */
      if (TypeNoticesListing == Not_LIST_FULL_NOTICES)
	 if (Not_CheckIfICanEditNotices () == Usr_CAN)
	   {
	    /***** Icon to remove announcement *****/
	    Ico_PutContextualIconToRemove (ActReqRemNot,NULL,
					   Not_PutParNotCod,&Notice->NotCod);

	    /***** Icon to change the status of the notice *****/
	    Ico_PutContextualIconToHideUnhide (ActionHideUnhide,NULL,	// TODO: Put anchor
				               Not_PutParNotCod,&Notice->NotCod,
				               Notice->HiddenOrVisible);
	   }

      /* Write the date */
      UniqueId++;
      HTM_DIV_Begin ("class=\"NOTICE_DATE NOTICE_DATE_%s RT\"",The_GetSuffix ());
	 if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
	   {
	    /* Form to view full notice */
	    Frm_BeginFormAnchor (ActSeeOneNot,Anchor);
	       ParCod_PutPar (ParCod_Not,Notice->NotCod);
	       HTM_BUTTON_Submit_Begin (Txt_See_full_notice,NULL,
	                                "class=\"RT BT_LINK\"");
	   }
	 if (asprintf (&Id,"not_date_%u",UniqueId) < 0)
	    Err_NotEnoughMemoryExit ();
	 HTM_SPAN_Begin ("id=\"%s\"",Id);
	 HTM_SPAN_End ();
	 if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
	   {
	       HTM_BUTTON_End ();
	    Frm_EndForm ();
	   }
	 Dat_WriteLocalDateHMSFromUTC (Id,Notice->CreatTime,
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       Dat_WRITE_TODAY |
				       Dat_WRITE_DATE_ON_SAME_DAY |
				       Dat_WRITE_HOUR |
				       Dat_WRITE_MINUTE);
	 free (Id);
      HTM_DIV_End ();

      /***** Write the content of the notice *****/
      if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
	{
	 HTM_DIV_Begin ("class=\"NOTICE_TEXT_BRIEF NOTICE_TEXT_%s\"",
                        The_GetSuffix ());
	    HTM_Txt (Notice->Content);
	 HTM_DIV_End ();

	 /* Put form to view full notice */
	 HTM_DIV_Begin ("class=\"CM\"");
	    Lay_PutContextualLinkOnlyIcon (ActSeeOneNot,Anchor,
					   Not_PutParNotCod,&Notice->NotCod,
					   "ellipsis-h.svg",Ico_BLACK);
	 HTM_DIV_End ();
	}
      else
	{
         HTM_DIV_Begin ("class=\"NOTICE_TEXT NOTICE_TEXT_%s\"",The_GetSuffix ());
            HTM_Txt (Notice->Content);
	 HTM_DIV_End ();
	}

      /***** Write the author *****/
      HTM_DIV_Begin ("class=\"NOTICE_AUTHOR NOTICE_AUTHOR_%s\"",The_GetSuffix ());	// Limited width
	 Usr_UsrDataConstructor (&UsrDat);
	 UsrDat.UsrCod = Notice->UsrCod;
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,	// Get author's data from database
						      Usr_DONT_GET_PREFS,
						      Usr_DONT_GET_ROLE_IN_CRS) == Exi_EXISTS)
	    Usr_WriteFirstNameBRSurnames (&UsrDat);
	 Usr_UsrDataDestructor (&UsrDat);
      HTM_DIV_End ();

   /***** End yellow note *****/
   HTM_DIV_End ();

   /***** End article for this notice *****/
   if (TypeNoticesListing == Not_LIST_FULL_NOTICES)
     {
 	 HTM_DIV_End ();
      HTM_ARTICLE_End ();
     }

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (&Anchor);
  }

/*****************************************************************************/
/******************* Get summary and content for a notice ********************/
/*****************************************************************************/

void Not_GetSummaryAndContentNotice (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                     char **ContentStr,
                                     long NotCod,Ntf_GetContent_t GetContent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   size_t Length;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Get content of message from database *****/
   if (Not_DB_ContentNotice (&mysql_res,NotCod) == Exi_EXISTS)
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
	 Str_Copy (SummaryStr,row[0],Ntf_MAX_BYTES_SUMMARY);

      /***** Copy content *****/
      if (GetContent == Ntf_GET_CONTENT)
	{
	 Length = strlen (row[0]);
	 if ((*ContentStr = malloc (Length + 1)) == NULL)
            Err_NotEnoughMemoryExit ();
	 Str_Copy (*ContentStr,row[0],Length);
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

unsigned Not_GetNumNotices (Hie_Level_t HieLvl,HidVis_HiddenOrVisible_t HiddenOrVisible,
			    unsigned *NumNotif)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumNotices;

   /***** Get number of notices from database *****/
   if (Not_DB_GetNumNotices (&mysql_res,HieLvl,HiddenOrVisible) == 1)
     {
      /***** Get number of notices *****/
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%u",&NumNotices) != 1)
	 Err_ShowErrorAndExit ("Error when getting number of notices.");

      /***** Get number of notifications by email *****/
      if (row[1])
	{
	 if (sscanf (row[1],"%u",NumNotif) != 1)
	    Err_ShowErrorAndExit ("Error when getting number of notifications of notices.");
	}
      else
	 *NumNotif = 0;
     }
   else
     {
      NumNotices = 0;
      *NumNotif = 0;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumNotices;
  }

/*****************************************************************************/
/************************ Get number of deleted notices **********************/
/*****************************************************************************/
// Returns the number of deleted notices
// sent from this location (all the platform, current degree or current course)

unsigned Not_GetNumNoticesDeleted (Hie_Level_t HieLvl,unsigned *NumNotif)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumNotices;

   /***** Get number of notices from database *****/
   if (Not_DB_GetNumNoticesDeleted (&mysql_res,HieLvl) == 1)
     {
      /***** Get number of notices *****/
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%u",&NumNotices) != 1)
	 Err_ShowErrorAndExit ("Error when getting number of deleted notices.");

      /***** Get number of notifications by email *****/
      if (row[1])
	{
	 if (sscanf (row[1],"%u",NumNotif) != 1)
	    Err_ShowErrorAndExit ("Error when getting number of notifications of deleted notices.");
	}
      else
	 *NumNotif = 0;
     }
   else
     {
      NumNotices = 0;
      *NumNotif = 0;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumNotices;
  }

/*****************************************************************************/
/*************** Put parameter with the code of a notice *********************/
/*****************************************************************************/

static void Not_PutParNotCod (void *NotCod)
  {
   if (NotCod)
      ParCod_PutPar (ParCod_Not,*((long *) NotCod));
  }

/*****************************************************************************/
/************************** Show figures about notices ***********************/
/*****************************************************************************/

void Not_GetAndShowNoticesStats (Hie_Level_t HieLvl)
  {
   extern const char *Hlp_ANALYTICS_Figures_notices;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_NOTICE_Active_BR_notices;
   extern const char *Txt_NOTICE_Obsolete_BR_notices;
   extern const char *Txt_NOTICE_Deleted_BR_notices;
   extern const char *Txt_Total;
   extern const char *Txt_Number_of_BR_notifications;
   HidVis_HiddenOrVisible_t HiddenOrVisible;
   unsigned NumNotices[HidVis_NUM_HIDDEN_VISIBLE];
   unsigned NumNoticesDeleted;
   unsigned NumTotalNotices = 0;
   unsigned NumNotif;
   unsigned NumTotalNotifications = 0;

   /***** Get the number of notices active and obsolete *****/
   for (HiddenOrVisible  = (HidVis_HiddenOrVisible_t) 0;
	HiddenOrVisible <= (HidVis_HiddenOrVisible_t) (HidVis_NUM_HIDDEN_VISIBLE - 1);
	HiddenOrVisible++)
     {
      NumNotices[HiddenOrVisible] = Not_GetNumNotices (HieLvl,HiddenOrVisible,&NumNotif);
      NumTotalNotices += NumNotices[HiddenOrVisible];
      NumTotalNotifications += NumNotif;
     }
   NumNoticesDeleted = Not_GetNumNoticesDeleted (HieLvl,&NumNotif);
   NumTotalNotices += NumNoticesDeleted;
   NumTotalNotifications += NumNotif;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_NOTICES],NULL,NULL,
                      Hlp_ANALYTICS_Figures_notices,Box_NOT_CLOSABLE,2);

      /***** Write table heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_NOTICE_Active_BR_notices  ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_NOTICE_Obsolete_BR_notices,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_NOTICE_Deleted_BR_notices ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Total                     ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_BR_notifications,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Write number of notices *****/
      HTM_TR_Begin (NULL);
	 HTM_TD_Unsigned (NumNotices[HidVis_VISIBLE]);
	 HTM_TD_Unsigned (NumNotices[HidVis_HIDDEN ]);
	 HTM_TD_Unsigned (NumNoticesDeleted);
	 HTM_TD_Unsigned (NumTotalNotices);
	 HTM_TD_Unsigned (NumTotalNotifications);
      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
