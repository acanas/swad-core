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
   100,	// Not_LIST_BRIEF_NOTICES
   400,	// Not_LIST_FULL_NOTICES
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
                             bool ICanEditNotices,
                             long NotCod,
                             const char *DateTime,
                             const char *Content,
                             long UsrCod,
                             Not_Status_t NoticeStatus);
static long Not_InsertNoticeInDB (const char *Content);
static void Not_UpdateNumUsrsNotifiedByEMailAboutNotice (long NotCod,unsigned NumUsrsToBeNotifiedByEMail);
static void Not_PutHiddenParamNotCod (long NotCod);
static long Not_GetParamNotCod (void);

/*****************************************************************************/
/***************************** Write a new notice ****************************/
/*****************************************************************************/

void Not_ShowFormNotice (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_The_notice_you_enter_here_will_appear_as_a_yellow_note_;
   extern const char *Txt_New_notice;
   extern const char *Txt_MSG_Message;
   extern const char *Txt_Create_notice;

   sprintf (Gbl.Message,Txt_The_notice_you_enter_here_will_appear_as_a_yellow_note_,
            Gbl.CurrentCrs.Crs.FullName);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);

   /***** Form start *****/
   Act_FormStart (ActRcvNot);

   /***** Start frame *****/
   Lay_StartRoundFrameTable10 (NULL,2,Txt_New_notice);

   /***** Message body *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s\" style=\"text-align:right;"
                      " vertical-align:top;\">"
                      "%s: "
                      "</td>"
                      "<td style=\"text-align:left;\">"
                      "<textarea name=\"Content\" cols=\"30\" rows=\"10\"></textarea>"
                      "</td>"
                      "</tr>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_MSG_Message);

   /***** Button to create announcement *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" style=\"text-align:center;\">");
   Lay_PutCreateButton (Txt_Create_notice);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** End frame *****/
   Lay_EndRoundFrameTable10 ();

   /***** Form end *****/
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
   extern const char *Txt_No_notices;
   extern const char *Txt_New_notice;
   extern const char *Txt_All_notices;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char StrWidth[10+2+1];
   char PathRelRSSFile[PATH_MAX+1];
   long NotCod;
   unsigned long NumNot,NumNotices;
   char Content[Cns_MAX_BYTES_TEXT+1];
   long UsrCod;
   unsigned UnsignedNum;
   Not_Status_t NoticeStatus;
   bool ICanEditNotices;

   /***** A course must be selected (Gbl.CurrentCrs.Crs.CrsCod > 0) *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
     {
      ICanEditNotices = (
                         TypeNoticesListing == Not_LIST_FULL_NOTICES &&
                         (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER ||
                         Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)
                        );


      if (ICanEditNotices)
	{
         fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
	 Act_FormStart (ActWriNot);
         Act_PutContextualLink ("new",Txt_New_notice,Txt_New_notice,Txt_New_notice);
	 fprintf (Gbl.F.Out,"</div>");
	}

      /***** Show highlighted notice *****/
      if (TypeNoticesListing == Not_LIST_FULL_NOTICES &&
	  Gbl.CurrentCrs.Notices.HighlightNotCod > 0)
	{
	 sprintf (Query,"SELECT DATE_FORMAT(CreatTime,'%%Y%%m%%d%%H%%i') AS F,UsrCod,Content,Status"
			" FROM notices"
			" WHERE NotCod='%ld' AND CrsCod='%ld'",
		  Gbl.CurrentCrs.Notices.HighlightNotCod,
		  Gbl.CurrentCrs.Crs.CrsCod);
         if (DB_QuerySELECT (Query,&mysql_res,"can not get notice from database"))
           {
	    row = mysql_fetch_row (mysql_res);

	    /* Get user code */
	    UsrCod = Str_ConvertStrCodToLongCod (row[1]);

	    /* Get the content and insert links */
	    strncpy (Content,row[2],Cns_MAX_BYTES_TEXT);
	    Str_InsertLinkInURLs (Content,Cns_MAX_BYTES_TEXT,
		                  Not_MaxCharsURLOnScreen[TypeNoticesListing]);
	    if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
	       Str_LimitLengthHTMLStr (Content,Not_MAX_CHARS_ON_NOTICE);

	    /* Get status of the notice */
	    NoticeStatus = Not_OBSOLETE_NOTICE;
	    if (sscanf (row[3],"%u",&UnsignedNum) == 1)
	       if (UnsignedNum < Not_NUM_STATUS)
		 NoticeStatus = (Not_Status_t) UnsignedNum;

	    /* Draw the notice */
	    Not_DrawANotice (TypeNoticesListing,ICanEditNotices,Gbl.CurrentCrs.Notices.HighlightNotCod,row[0],Content,UsrCod,NoticeStatus);
           }

	 /* Free structure that stores the query result */
	 DB_FreeMySQLResult (&mysql_res);
	}

      /***** Get notices from database *****/
      switch (TypeNoticesListing)
	{
	 case Not_LIST_BRIEF_NOTICES:
	    sprintf (Query,"SELECT NotCod,DATE_FORMAT(CreatTime,'%%Y%%m%%d%%H%%i') AS F,UsrCod,Content,Status"
			   " FROM notices"
			   " WHERE CrsCod='%ld' AND Status='%u'"
			   " ORDER BY CreatTime DESC",
		     Gbl.CurrentCrs.Crs.CrsCod,
		     (unsigned) Not_ACTIVE_NOTICE);
	    break;
	 case Not_LIST_FULL_NOTICES:
	    sprintf (Query,"SELECT NotCod,DATE_FORMAT(CreatTime,'%%Y%%m%%d%%H%%i') AS F,UsrCod,Content,Status"
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
	    sprintf (StrWidth,"%upx",
		     Not_ContainerWidth[Not_LIST_FULL_NOTICES] + 40);
            Lay_StartRoundFrameTable10 (StrWidth,2,Txt_All_notices);
            fprintf (Gbl.F.Out,"<tr>"
        	               "<td style=\"text-align:center;\">");
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
	 fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">"
			    "<a href=\"");
	 RSS_WriteRSSLink (Gbl.F.Out,Gbl.CurrentCrs.Crs.CrsCod);
	 fprintf (Gbl.F.Out,"\" target=\"_blank\">"
			    "<img src=\"%s/rss16x16.gif\""
			    " alt=\"RSS\" title=\"RSS\" class=\"ICON16x16\" />"
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

	 /* Get notice code */
	 if (sscanf (row[0],"%ld",&NotCod) != 1)
	    Lay_ShowErrorAndExit ("Wrong code of notice.");

	 /* Get user code */
	 UsrCod = Str_ConvertStrCodToLongCod (row[2]);

	 /* Get the content and insert links */
	 strncpy (Content,row[3],Cns_MAX_BYTES_TEXT);
	 Str_InsertLinkInURLs (Content,Cns_MAX_BYTES_TEXT,
	                       Not_MaxCharsURLOnScreen[TypeNoticesListing]);
	 if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
            Str_LimitLengthHTMLStr (Content,Not_MAX_CHARS_ON_NOTICE);

	 /* Get status of the notice */
	 NoticeStatus = Not_OBSOLETE_NOTICE;
	 if (sscanf (row[4],"%u",&UnsignedNum) == 1)
	    if (UnsignedNum < Not_NUM_STATUS)
	      NoticeStatus = (Not_Status_t) UnsignedNum;

	 /* Draw the notice */
	 Not_DrawANotice (TypeNoticesListing,ICanEditNotices,NotCod,row[1],Content,UsrCod,NoticeStatus);
	}

      if (TypeNoticesListing == Not_LIST_FULL_NOTICES && NumNotices)
	{
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");
	 Lay_EndRoundFrameTable10 ();
	}

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
                             bool ICanEditNotices,
                             long NotCod,
                             const char *DateTime,
                             const char *Content,
                             long UsrCod,
                             Not_Status_t NoticeStatus)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_NOTICE_Active_SINGULAR;
   extern const char *Txt_NOTICE_Active_Mark_as_obsolete;
   extern const char *Txt_NOTICE_Obsolete_SINGULAR;
   extern const char *Txt_NOTICE_Obsolete_Mark_as_active;
   extern const char *Txt_See_full_notice;
   extern const char *Txt_Remove;
   extern const char *Txt_Remove_notice;
   struct UsrData UsrDat;

   /***** Start yellow note *****/
   fprintf (Gbl.F.Out,"<div class=\"NOTICE_CONTAINER\" style=\"width:%upx;\">",
	    Not_ContainerWidth[TypeNoticesListing]);

   /***** Write the date in the top part of the yellow note *****/
   /* Write symbol to indicate if notice is obsolete or active */
   if (TypeNoticesListing == Not_LIST_FULL_NOTICES)
     {
      if (ICanEditNotices)	// Put form to change the state of the notice
         switch (NoticeStatus)
           {
            case Not_ACTIVE_NOTICE:
               Act_FormStart (ActHidNot);
               Not_PutHiddenParamNotCod (NotCod);
               fprintf (Gbl.F.Out,"<input type=\"image\""
        	                  " src=\"%s/visible_on16x16.gif\" alt=\"%s\""
        	                  " title=\"%s\" class=\"ICON16x16\" />",
                        Gbl.Prefs.IconsURL,
                        Txt_NOTICE_Active_Mark_as_obsolete,
                        Txt_NOTICE_Active_Mark_as_obsolete);
               Act_FormEnd ();
               break;
            case Not_OBSOLETE_NOTICE:
               Act_FormStart (ActRevNot);
               Not_PutHiddenParamNotCod (NotCod);
               fprintf (Gbl.F.Out,"<input type=\"image\""
        	                  " src=\"%s/hidden_on16x16.gif\" alt=\"%s\""
        	                  " title=\"%s\" class=\"ICON16x16\" />",
                        Gbl.Prefs.IconsURL,
                        Txt_NOTICE_Obsolete_Mark_as_active,
                        Txt_NOTICE_Obsolete_Mark_as_active);
               Act_FormEnd ();
               break;
           }
      else			// Don't put form to change the state of the notice
         switch (NoticeStatus)
           {
            case Not_ACTIVE_NOTICE:
               fprintf (Gbl.F.Out,"<span title=\"%s\">"
                                  "<img src=\"%s/visible_off16x16.gif\""
                                  " alt=\"%s\" class=\"ICON16x16\" />"
                                  "</span>",
                        Txt_NOTICE_Active_SINGULAR,
                        Gbl.Prefs.IconsURL,
                        Txt_NOTICE_Active_SINGULAR);
               break;
            case Not_OBSOLETE_NOTICE:
               fprintf (Gbl.F.Out,"<span title=\"%s\">"
                                  "<img src=\"%s/hidden_off16x16.gif\""
                                  " alt=\"%s\" class=\"ICON16x16\" />"
                                  "</span>",
                        Txt_NOTICE_Obsolete_SINGULAR,
                        Gbl.Prefs.IconsURL,
                        Txt_NOTICE_Obsolete_SINGULAR);
               break;
           }
     }

   /* Write the date (DateTime is in YYYYMMDDHHMM format) */
   fprintf (Gbl.F.Out,"<div class=\"NOTICE_DATE\">");
   if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
     {
      /* Form to view full notice */
      Act_FormStart (ActShoNot);
      Not_PutHiddenParamNotCod (NotCod);
      Act_LinkFormSubmit (Txt_See_full_notice,"NOTICE_DATE");
     }
   Dat_WriteDate (DateTime);
   fprintf (Gbl.F.Out,"&nbsp;");
   Dat_WriteHourMinute (&DateTime[8]);
   if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
     {
      fprintf (Gbl.F.Out,"</a>");
      Act_FormEnd ();
     }
   fprintf (Gbl.F.Out,"</div>");

   /***** Write the content of the notice *****/
   fprintf (Gbl.F.Out,"<div class=\"NOTICE_TEXT\">%s",
            Content);
   if (TypeNoticesListing == Not_LIST_BRIEF_NOTICES)
     {
      fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
      /* Form to view full notice */
      Act_FormStart (ActShoNot);
      Not_PutHiddenParamNotCod (NotCod);
      Act_LinkFormSubmit (Txt_See_full_notice,The_ClassFormul[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"<img src=\"%s/ellipsis32x32.gif\""
	                 " alt=\"%s\" class=\"ICON32x32\" />"
	                 "</a>",
	       Gbl.Prefs.IconsURL,
	       Txt_See_full_notice);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");
     }
   fprintf (Gbl.F.Out,"</div>");

   /***** Write the author *****/
   fprintf (Gbl.F.Out,"<div class=\"NOTICE_AUTHOR\">");
   Usr_UsrDataConstructor (&UsrDat);
   UsrDat.UsrCod = UsrCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat)) // Get from the database the data of the autor
      Usr_RestrictLengthAndWriteName (&UsrDat,18);
   Usr_UsrDataDestructor (&UsrDat);
   fprintf (Gbl.F.Out,"</div>");

   /***** Write form to delete this notice *****/
   if (ICanEditNotices)
     {
      fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:center;\">",
               The_ClassFormul[Gbl.Prefs.Theme]);

      /* Form to delete notice */
      Act_FormStart (ActRemNot);
      Not_PutHiddenParamNotCod (NotCod);
      Act_LinkFormSubmit (Txt_Remove_notice,The_ClassFormul[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"<img src=\"%s/delon16x16.gif\""
	                 " alt=\"%s\" class=\"ICON16x16\" />"
                         " %s</a>",
               Gbl.Prefs.IconsURL,
               Txt_Remove_notice,
               Txt_Remove);
      Act_FormEnd ();

      fprintf (Gbl.F.Out,"</div>");
     }

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
