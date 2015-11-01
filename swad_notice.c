// swad_notice.c: notices (yellow notes)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For exit, system, malloc, calloc, free, etc.
#include <string.h>

#include "swad_database.h"
#include "swad_global.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_RSS.h"

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
   125,	// Not_LIST_BRIEF_NOTICES
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

static void Not_DrawANotice (Not_Listing_t TypeNoticesListing,
                             long NotCod,
                             time_t TimeUTC,
                             const char *Content,
                             long UsrCod,
                             Not_Status_t NoticeStatus,
                             bool ICanEditNotices);
static long Not_InsertNoticeInDB (const char *Content);
static void Not_UpdateNumUsrsNotifiedByEMailAboutNotice (long NotCod,unsigned NumUsrsToBeNotifiedByEMail);
static void Not_PutHiddenParamNotCod (long NotCod);
static long Not_GetParamNotCod (void);

/*****************************************************************************/
/***************************** Write a new notice ****************************/
/*****************************************************************************/

void Not_ShowFormNotice (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_The_notice_you_enter_here_will_appear_as_a_yellow_note_;
   extern const char *Txt_New_notice;
   extern const char *Txt_MSG_Message;
   extern const char *Txt_Create_notice;

   sprintf (Gbl.Message,Txt_The_notice_you_enter_here_will_appear_as_a_yellow_note_,
            Gbl.CurrentCrs.Crs.FullName);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);

   /***** Start form *****/
   Act_FormStart (ActRcvNot);

   /***** Start frame *****/
   Lay_StartRoundFrameTable (NULL,2,Txt_New_notice);

   /***** Message body *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s RIGHT_TOP\">"
                      "%s: "
                      "</td>"
                      "<td class=\"LEFT_TOP\">"
                      "<textarea name=\"Content\" cols=\"30\" rows=\"10\">"
                      "</textarea>"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_MSG_Message);

   /***** Button to create notice and end frame *****/
   Lay_EndRoundFrameTableWithButton (Lay_CREATE_BUTTON,Txt_Create_notice);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/******* Receive a new notice from a form and store it in database ***********/
/*****************************************************************************/

void Not_ReceiveNotice (void)
  {
   extern const char *Txt_Notice_created;
   long NotCod;
   unsigned NumUsrsToBeNotifiedByEMail;
   char Content[Cns_MAX_BYTES_TEXT+1];

   /***** Get the text of the notice *****/
   Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_TEXT,
                              Str_TO_RIGOROUS_HTML,true);

   /***** Create a new notice in database *****/
   NotCod = Not_InsertNoticeInDB (Content);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.CurrentCrs.Crs);

   /***** Write message of success *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_Notice_created);

   /***** Notify by e-mail about the new notice *****/
   if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_NOTICE,NotCod)))
      Not_UpdateNumUsrsNotifiedByEMailAboutNotice (NotCod,NumUsrsToBeNotifiedByEMail);
   Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);
  }

/*****************************************************************************/
/******************* Insert a notice in the table of notices *****************/
/*****************************************************************************/
// Return the code of the new inserted notice

static long Not_InsertNoticeInDB (const char *Content)
  {
   char Query[256+Cns_MAX_BYTES_TEXT];

   /***** Insert notice in the database *****/
   sprintf (Query,"INSERT INTO notices (CrsCod,UsrCod,CreatTime,Content,Status)"
                  " VALUES ('%ld','%ld',NOW(),'%s','%u')",
            Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod,Content,(unsigned) Not_ACTIVE_NOTICE);
   return DB_QueryINSERTandReturnCode (Query,"can not create notice");
  }

/*****************************************************************************/
/*********** Update number of users notified in table of notices *************/
/*****************************************************************************/

static void Not_UpdateNumUsrsNotifiedByEMailAboutNotice (long NotCod,unsigned NumUsrsToBeNotifiedByEMail)
  {
   char Query[512];

   /***** Update number of users notified *****/
   sprintf (Query,"UPDATE notices SET NumNotif='%u' WHERE NotCod='%ld'",
            NumUsrsToBeNotifiedByEMail,NotCod);
   DB_QueryUPDATE (Query,"can not update the number of notifications of a notice");
  }

/*****************************************************************************/
/******************************* List notices ********************************/
/*****************************************************************************/

void Not_ListNotices (void)
  {
   Not_ShowNotices (Not_LIST_FULL_NOTICES);
  }

/*****************************************************************************/
/***************** Mark as hidden a notice that was active *******************/
/*****************************************************************************/

void Not_HideActiveNotice (void)
  {
   char Query[256];
   long NotCod;

   /***** Get the code of the notice to hide *****/
   NotCod = Not_GetParamNotCod ();

   /***** Set notice as hidden *****/
   sprintf (Query,"UPDATE notices SET Status='%u'"
                  " WHERE NotCod='%ld' AND CrsCod='%ld'",
            (unsigned) Not_OBSOLETE_NOTICE,
            NotCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not hide notice");

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.CurrentCrs.Crs);
  }

/*****************************************************************************/
/****************** Mark as active a notice that was hidden ******************/
/*****************************************************************************/

void Not_RevealHiddenNotice (void)
  {
   char Query[256];
   long NotCod;

   /***** Get the code of the notice to show *****/
   NotCod = Not_GetParamNotCod ();

   /***** Set notice as shown *****/
   sprintf (Query,"UPDATE notices SET Status='%u'"
                  " WHERE NotCod='%ld' AND CrsCod='%ld'",
            (unsigned) Not_ACTIVE_NOTICE,
            NotCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not show notice");

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.CurrentCrs.Crs);
  }

/*****************************************************************************/
/******************************* Delete a notice *****************************/
/*****************************************************************************/

void Not_DeleteNotice (void)
  {
   char Query[512];
   long NotCod;

   /***** Get the code of the notice to delete *****/
   NotCod = Not_GetParamNotCod ();

   /***** Remove notice *****/
   /* Copy notice to table of deleted notices */
   sprintf (Query,"INSERT IGNORE INTO notices_deleted (NotCod,CrsCod,UsrCod,CreatTime,Content,NumNotif)"
                  " SELECT NotCod,CrsCod,UsrCod,CreatTime,Content,NumNotif"
                  " FROM notices"
                  " WHERE NotCod='%ld' AND CrsCod='%ld'",
            NotCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryINSERT (Query,"can not remove notice");

   /* Remove notice */
   sprintf (Query,"DELETE FROM notices"
	          " WHERE NotCod='%ld' AND CrsCod='%ld'",
            NotCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryDELETE (Query,"can not remove notice");

   /***** Mark possible notifications as removed *****/
   Ntf_SetNotifAsRemoved (Ntf_EVENT_NOTICE,NotCod);

   /***** Update RSS of current course *****/
   RSS_UpdateRSSFileForACrs (&Gbl.CurrentCrs.Crs);
  }

/*****************************************************************************/
/**************************** Show (expand) a notice *************************/
/*****************************************************************************/

void Not_ShowANotice (void)
  {
   /***** Get the code of the notice to highlight *****/
   Gbl.CurrentCrs.Notices.HighlightNotCod = Not_GetParamNotCod ();
  }

/*****************************************************************************/
/***************************** Show the notices ******************************/
/*****************************************************************************/

void Not_ShowNotices (Not_Listing_t TypeNoticesListing)
  {
   extern const char *Txt_New_notice;
   extern const char *Txt_All_notices;
   extern const char *Txt_No_notices;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char StrWidth[10+2+1];
   char PathRelRSSFile[PATH_MAX+1];
   long NotCod;
   unsigned long NumNot;
   unsigned long NumNotices;
   char Content[Cns_MAX_BYTES_TEXT+1];
   time_t TimeUTC;
   long UsrCod;
   unsigned UnsignedNum;
   Not_Status_t NoticeStatus;
   bool ICanEditNotices;

   /***** A course must be selected (Gbl.CurrentCrs.Crs.CrsCod > 0) *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
     {
      ICanEditNotices = (TypeNoticesListing == Not_LIST_FULL_NOTICES &&
                         (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER ||
                          Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM));

      if (ICanEditNotices)
	{
         fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
         Act_PutContextualLink (ActWriNot,NULL,"new",Txt_New_notice);
	 fprintf (Gbl.F.Out,"</div>");
	}

      /***** Show highlighted notice *****/
      if (TypeNoticesListing == Not_LIST_FULL_NOTICES &&
	  Gbl.CurrentCrs.Notices.HighlightNotCod > 0)
	{
	 sprintf (Query,"SELECT UNIX_TIMESTAMP(CreatTime) AS F,UsrCod,Content,Status"
			" FROM notices"
			" WHERE NotCod='%ld' AND CrsCod='%ld'",
		  Gbl.CurrentCrs.Notices.HighlightNotCod,
		  Gbl.CurrentCrs.Crs.CrsCod);
         if (DB_QuerySELECT (Query,&mysql_res,"can not get notice from database"))
           {
	    row = mysql_fetch_row (mysql_res);

	    /* Get creation time (row[0] holds the UTC date-time) */
	    TimeUTC = Dat_GetUNIXTimeFromStr (row[0]);

	    /* Get user code (row[1]) */
	    UsrCod = Str_ConvertStrCodToLongCod (row[1]);

	    /* Get the content (row[2]) and insert links*/
	    strncpy (Content,row[2],Cns_MAX_BYTES_TEXT);
	    Str_InsertLinkInURLs (Content,Cns_MAX_BYTES_TEXT,
		                  Not_MaxCharsURLOnScreen[TypeNoticesListing]);
	    if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
	       Str_LimitLengthHTMLStr (Content,Not_MAX_CHARS_ON_NOTICE);

	    /* Get status of the notice (row[3]) */
	    NoticeStatus = Not_OBSOLETE_NOTICE;
	    if (sscanf (row[3],"%u",&UnsignedNum) == 1)
	       if (UnsignedNum < Not_NUM_STATUS)
		 NoticeStatus = (Not_Status_t) UnsignedNum;

	    /* Draw the notice */
	    Not_DrawANotice (TypeNoticesListing,
	                     Gbl.CurrentCrs.Notices.HighlightNotCod,
	                     TimeUTC,Content,UsrCod,NoticeStatus,
	                     ICanEditNotices);
           }

	 /* Free structure that stores the query result */
	 DB_FreeMySQLResult (&mysql_res);
	}

      /***** Get notices from database *****/
      switch (TypeNoticesListing)
	{
	 case Not_LIST_BRIEF_NOTICES:
	    sprintf (Query,"SELECT NotCod,UNIX_TIMESTAMP(CreatTime) AS F,UsrCod,Content,Status"
			   " FROM notices"
			   " WHERE CrsCod='%ld' AND Status='%u'"
			   " ORDER BY CreatTime DESC",
		     Gbl.CurrentCrs.Crs.CrsCod,
		     (unsigned) Not_ACTIVE_NOTICE);
	    break;
	 case Not_LIST_FULL_NOTICES:
	    sprintf (Query,"SELECT NotCod,UNIX_TIMESTAMP(CreatTime) AS F,UsrCod,Content,Status"
			   " FROM notices"
			   " WHERE CrsCod='%ld'"
			   " ORDER BY CreatTime DESC",
		     Gbl.CurrentCrs.Crs.CrsCod);
	    break;
	}
      NumNotices = DB_QuerySELECT (Query,&mysql_res,"can not get notices from database");

      if (TypeNoticesListing == Not_LIST_FULL_NOTICES)
	{
	 if (NumNotices)
	   {
            /***** Start frame *****/
	    sprintf (StrWidth,"%upx",
		     Not_ContainerWidth[Not_LIST_FULL_NOTICES] + 50);
            Lay_StartRoundFrame (StrWidth,Txt_All_notices);
	   }
	 else
	    Lay_ShowAlert (Lay_INFO,Txt_No_notices);
	}

      /***** Link to RSS file *****/
      if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
	{
	 /* Create RSS file if not exists */
	 sprintf (PathRelRSSFile,"%s/%s/%ld/%s/%s",
		  Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CRS,Gbl.CurrentCrs.Crs.CrsCod,Cfg_RSS_FOLDER,Cfg_RSS_FILE);
	 if (!Fil_CheckIfPathExists (PathRelRSSFile))
	    RSS_UpdateRSSFileForACrs (&Gbl.CurrentCrs.Crs);

	 /* Put a link to the RSS file */
	 fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">"
			    "<a href=\"");
	 RSS_WriteRSSLink (Gbl.F.Out,Gbl.CurrentCrs.Crs.CrsCod);
	 fprintf (Gbl.F.Out,"\" target=\"_blank\">"
			    "<img src=\"%s/rss16x16.gif\""
			    " alt=\"RSS\" title=\"RSS\""
			    " class=\"ICON16x16\" />"
			    "</a>"
			    "</div>",
	          Gbl.Prefs.IconsURL);
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
	 strncpy (Content,row[3],Cns_MAX_BYTES_TEXT);
	 Str_InsertLinkInURLs (Content,Cns_MAX_BYTES_TEXT,
	                       Not_MaxCharsURLOnScreen[TypeNoticesListing]);
	 if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
            Str_LimitLengthHTMLStr (Content,Not_MAX_CHARS_ON_NOTICE);

	 /* Get status of the notice (row[4]) */
	 NoticeStatus = Not_OBSOLETE_NOTICE;
	 if (sscanf (row[4],"%u",&UnsignedNum) == 1)
	    if (UnsignedNum < Not_NUM_STATUS)
	      NoticeStatus = (Not_Status_t) UnsignedNum;

	 /* Draw the notice */
	 Not_DrawANotice (TypeNoticesListing,
	                  NotCod,
	                  TimeUTC,Content,UsrCod,NoticeStatus,
	                  ICanEditNotices);
	}

      if (TypeNoticesListing == Not_LIST_FULL_NOTICES && NumNotices)
         /***** End frame *****/
	 Lay_EndRoundFrame ();

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Mark possible notification as seen *****/
      Ntf_SetNotifAsSeen (Ntf_EVENT_NOTICE,
	                  -1L,
	                  Gbl.Usrs.Me.UsrDat.UsrCod);
     }
  }

/*****************************************************************************/
/********************* Draw a notice as a yellow note ************************/
/*****************************************************************************/

static void Not_DrawANotice (Not_Listing_t TypeNoticesListing,
                             long NotCod,
                             time_t TimeUTC,
                             const char *Content,
                             long UsrCod,
                             Not_Status_t NoticeStatus,
                             bool ICanEditNotices)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_NOTICE_Active_SINGULAR;
   extern const char *Txt_NOTICE_Active_Mark_as_obsolete;
   extern const char *Txt_NOTICE_Obsolete_SINGULAR;
   extern const char *Txt_NOTICE_Obsolete_Mark_as_active;
   extern const char *Txt_See_full_notice;
   extern const char *Txt_Remove;
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

   /***** Start yellow note *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"width:%upx;\">",
	    ContainerClass[NoticeStatus],
	    Not_ContainerWidth[TypeNoticesListing]);

   /***** Write the date in the top part of the yellow note *****/
   /* Write symbol to indicate if notice is obsolete or active */
   if (TypeNoticesListing == Not_LIST_FULL_NOTICES)
     {
      if (ICanEditNotices)
	{
	 /* Form to remove notice */
	 Act_FormStart (ActRemNot);
	 Not_PutHiddenParamNotCod (NotCod);
	 fprintf (Gbl.F.Out,"<div class=\"CONTEXT_OPT ICON_HIGHLIGHT\">"
        	            "<input type=\"image\""
	                    " src=\"%s/delon16x16.gif\""
			    " alt=\"%s\" title=\"%s\""
			    " class=\"ICON16x16\" />"
        	            "</div>",
		  Gbl.Prefs.IconsURL,
		  Txt_Remove,
		  Txt_Remove);
	 Act_FormEnd ();

	 /* Put form to change the state of the notice */
         switch (NoticeStatus)
           {
            case Not_ACTIVE_NOTICE:
               Act_FormStart (ActHidNot);
               Not_PutHiddenParamNotCod (NotCod);
               fprintf (Gbl.F.Out,"<div class=\"CONTEXT_OPT ICON_HIGHLIGHT\">"
        	                  "<input type=\"image\""
        	                  " src=\"%s/visible_on16x16.gif\""
        	                  " alt=%s\" title=\"%s\""
        	                  " class=\"ICON16x16\" />"
        	                  "</div>",
                        Gbl.Prefs.IconsURL,
                        Txt_NOTICE_Active_Mark_as_obsolete,
                        Txt_NOTICE_Active_Mark_as_obsolete);
               break;
            case Not_OBSOLETE_NOTICE:
               Act_FormStart (ActRevNot);
               Not_PutHiddenParamNotCod (NotCod);
               fprintf (Gbl.F.Out,"<div class=\"CONTEXT_OPT ICON_HIGHLIGHT\">"
        	                  "<input type=\"image\""
        	                  " src=\"%s/hidden_on16x16.gif\""
        	                  " alt=\"%s\" title=\"%s\""
        	                  " class=\"ICON16x16\" />"
        	                  "</div>",
                        Gbl.Prefs.IconsURL,
                        Txt_NOTICE_Obsolete_Mark_as_active,
                        Txt_NOTICE_Obsolete_Mark_as_active);
               break;
           }
         Act_FormEnd ();
   	}
      else	// Don't put forms
	 /* Status of the notice */
         switch (NoticeStatus)
           {
            case Not_ACTIVE_NOTICE:
               fprintf (Gbl.F.Out,"<span title=\"%s\">"
                                  "<img src=\"%s/visible_off16x16.gif\""
                                  " alt=\"%s\" title=\"%s\""
                                  " class=\"ICON16x16\" />"
                                  "</span>",
                        Txt_NOTICE_Active_SINGULAR,
                        Gbl.Prefs.IconsURL,
                        Txt_NOTICE_Active_SINGULAR,
                        Txt_NOTICE_Active_SINGULAR);
               break;
            case Not_OBSOLETE_NOTICE:
               fprintf (Gbl.F.Out,"<span title=\"%s\">"
                                  "<img src=\"%s/hidden_off16x16.gif\""
                                  " alt=\"%s\" title=\"%s\""
                                  " class=\"ICON16x16\" />"
                                  "</span>",
                        Txt_NOTICE_Obsolete_SINGULAR,
                        Gbl.Prefs.IconsURL,
                        Txt_NOTICE_Obsolete_SINGULAR,
                        Txt_NOTICE_Obsolete_SINGULAR);
               break;
           }
     }

   /* Write the date */
   UniqueId++;
   fprintf (Gbl.F.Out,"<div class=\"%s\">",
            DateClass[NoticeStatus]);
   if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
     {
      /* Form to view full notice */
      Act_FormStart (ActShoNot);
      Not_PutHiddenParamNotCod (NotCod);
      Act_LinkFormSubmit (Txt_See_full_notice,DateClass[NoticeStatus]);
     }
   fprintf (Gbl.F.Out,"<span id=\"notice_date_%u\"></span>",
            UniqueId);
   if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
     {
      fprintf (Gbl.F.Out,"</a>");
      Act_FormEnd ();
     }
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
                      "writeLocalDateTimeFromUTC('notice_date_%u',%ld,'<br />');"
                      "</script>"
	              "</div>",
	    UniqueId,(long) TimeUTC);

   /***** Write the content of the notice *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s",
            TextClass[NoticeStatus],Content);
   if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
     {
      fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");
      /* Form to view full notice */
      Act_FormStart (ActShoNot);
      Not_PutHiddenParamNotCod (NotCod);
      Act_LinkFormSubmit (Txt_See_full_notice,The_ClassForm[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"<img src=\"%s/ellipsis32x32.gif\""
	                 " alt=\"%s\" title=\"%s\""
	                 " class=\"ICON32x32\" />"
	                 "</a>",
	       Gbl.Prefs.IconsURL,
	       Txt_See_full_notice,
	       Txt_See_full_notice);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");
     }
   fprintf (Gbl.F.Out,"</div>");

   /***** Write the author *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\">",
            AuthorClass[NoticeStatus]);
   Usr_UsrDataConstructor (&UsrDat);
   UsrDat.UsrCod = UsrCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat)) // Get from the database the data of the autor
      Usr_RestrictLengthAndWriteName (&UsrDat,18);
   Usr_UsrDataDestructor (&UsrDat);
   fprintf (Gbl.F.Out,"</div>");

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********************* Write first characters of a notice  *******************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error

void Not_GetNotifNotice (char *SummaryStr,char **ContentStr,long NotCod,unsigned MaxChars,bool GetContent)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Get subject of message from database *****/
   sprintf (Query,"SELECT Content FROM notices WHERE NotCod='%ld'",
            NotCod);
   if (!mysql_query (&Gbl.mysql,Query))
      if ((mysql_res = mysql_store_result (&Gbl.mysql)) != NULL)
        {
         /***** Result should have a unique row *****/
         if (mysql_num_rows (mysql_res) == 1)
           {
            /***** Get sumary / content *****/
            row = mysql_fetch_row (mysql_res);

            /***** Copy summary *****/
            strcpy (SummaryStr,row[0]);
            if (MaxChars)
               Str_LimitLengthHTMLStr (SummaryStr,MaxChars);

            /***** Copy content *****/
            if (GetContent)
              {
               if ((*ContentStr = (char *) malloc (strlen (row[0])+1)) == NULL)
                  Lay_ShowErrorAndExit ("Error allocating memory for notification content.");
               strcpy (*ContentStr,row[0]);
              }
           }
         mysql_free_result (mysql_res);
        }
  }

/*****************************************************************************/
/*************************** Get number of notices ***************************/
/*****************************************************************************/
// Returns the number of (active or obsolete) notices
// sent from this location (all the platform, current degree or current course)

unsigned Not_GetNumNotices (Sco_Scope_t Scope,Not_Status_t NoticeStatus,unsigned *NumNotif)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumNotices;

   /***** Get number of notices from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM notices"
                        " WHERE Status='%u'",
                        NoticeStatus);
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(*),SUM(notices.NumNotif)"
                        " FROM institutions,centres,degrees,courses,notices"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=notices.CrsCod"
                        " AND notices.Status='%u'",
                  Gbl.CurrentCty.Cty.CtyCod,
                  NoticeStatus);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(*),SUM(notices.NumNotif)"
                        " FROM centres,degrees,courses,notices"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=notices.CrsCod"
                        " AND notices.Status='%u'",
                  Gbl.CurrentIns.Ins.InsCod,
                  NoticeStatus);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(*),SUM(notices.NumNotif)"
                        " FROM degrees,courses,notices"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=notices.CrsCod"
                        " AND notices.Status='%u'",
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  NoticeStatus);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(*),SUM(notices.NumNotif)"
                        " FROM courses,notices"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=notices.CrsCod"
                        " AND notices.Status='%u'",
                  Gbl.CurrentDeg.Deg.DegCod,
                  NoticeStatus);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM notices"
                        " WHERE CrsCod='%ld'"
                        " AND Status='%u'",
                  Gbl.CurrentCrs.Crs.CrsCod,
                  NoticeStatus);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of notices");

   /***** Get number of notices *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumNotices) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of notices.");

   /***** Get number of notifications by e-mail *****/
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
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumNotices;

   /***** Get number of notices from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM notices_deleted");
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(*),SUM(notices_deleted.NumNotif)"
                        " FROM institutions,centres,degrees,courses,notices_deleted"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=notices_deleted.CrsCod",
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(*),SUM(notices_deleted.NumNotif)"
                        " FROM centres,degrees,courses,notices_deleted"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=notices_deleted.CrsCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(*),SUM(notices_deleted.NumNotif)"
                        " FROM degrees,courses,notices_deleted"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=notices_deleted.CrsCod",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(*),SUM(notices_deleted.NumNotif)"
                        " FROM courses,notices_deleted"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=notices_deleted.CrsCod",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM notices_deleted"
                        " WHERE CrsCod='%ld'",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of deleted notices");

   /***** Get number of notices *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumNotices) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of deleted notices.");

   /***** Get number of notifications by e-mail *****/
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

static void Not_PutHiddenParamNotCod (long NotCod)
  {
   Par_PutHiddenParamLong ("NotCod",NotCod);
  }

/*****************************************************************************/
/*************** Get parameter with the code of a notice *********************/
/*****************************************************************************/

static long Not_GetParamNotCod (void)
  {
   char LongStr[1+10+1];	// String that holds the notice code
   long NotCod;

   /* Get notice code */
   Par_GetParToText ("NotCod",LongStr,1+10);
   if (sscanf (LongStr,"%ld",&NotCod) != 1)
      Lay_ShowErrorAndExit ("Wrong code of notice.");

   return NotCod;
  }
