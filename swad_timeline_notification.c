// swad_timeline_notification.c: social timeline notifications

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <string.h>		// For string functions

#include "swad_global.h"
#include "swad_notification_database.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_note.h"
#include "swad_timeline_notification.h"
#include "swad_timeline_post.h"
#include "swad_timeline_publication.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*********** Create a notification for the author of a post/comment **********/
/*****************************************************************************/

void TmlNtf_CreateNotifToAuthor (long AuthorCod,long PubCod,
                                 Ntf_NotifyEvent_t NotifyEvent)
  {
   extern Ntf_Status_t Ntf_Status[Ntf_NUM_NOTIFY_BY_EMAIL];
   struct Usr_Data UsrDat;
   Ntf_NotifyByEmail_t NotifyByEmail;
   long HieCods[Hie_NUM_LEVELS];

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   UsrDat.UsrCod = AuthorCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
                                                Usr_DONT_GET_PREFS,
                                                Usr_DONT_GET_ROLE_IN_CRS) == Exi_EXISTS)
      /***** Create notification for the author of the post.
	     If this author wants to receive notifications by email,
	     activate the sending of a notification *****/
      if ((UsrDat.NtfEvents.CreateNotif & (1 << NotifyEvent)))	// Create notification?
	{
	 NotifyByEmail = (UsrDat.NtfEvents.SendEmail & (1 << NotifyEvent)) ? Ntf_NOTIFY_BY_EMAIL :
									     Ntf_DONT_NOTIFY_BY_EMAIL;;
	 HieCods[Hie_INS] = Gbl.Hierarchy.Node[Hie_INS].HieCod;
	 HieCods[Hie_CTR] = Gbl.Hierarchy.Node[Hie_CTR].HieCod;
	 HieCods[Hie_DEG] = Gbl.Hierarchy.Node[Hie_DEG].HieCod;
	 HieCods[Hie_CRS] = Gbl.Hierarchy.Node[Hie_CRS].HieCod;
	 Ntf_DB_StoreNotifyEventToUsr (NotifyEvent,UsrDat.UsrCod,PubCod,
				       Ntf_Status[NotifyByEmail],HieCods);
	}

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/***************** Get notification of a new publication *********************/
/*****************************************************************************/

void TmlNtf_GetNotifPublication (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                 char **ContentStr,
                                 long PubCod,Ntf_GetContent_t GetContent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct TmlPub_Publication Pub;
   struct TmlNot_Note Not;
   struct TmlPst_Content Content;
   size_t Length;
   bool ContentCopied = false;

   /***** Return nothing on error *****/
   Pub.Type = TmlPub_UNKNOWN;
   SummaryStr[0]  = '\0';	// Return nothing on error
   Content.Txt[0] = '\0';

   /***** Get summary and content from post from database *****/
   if (Tml_DB_GetPubDataByCod (PubCod,&mysql_res) == Exi_EXISTS)
      /* Get data of publication from row */
      TmlPub_GetPubDataFromRow (mysql_res,&Pub);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Get summary and content *****/
   switch (Pub.Type)
     {
      case TmlPub_UNKNOWN:
	 break;
      case TmlPub_ORIGINAL_NOTE:
      case TmlPub_SHARED_NOTE:
	 /* Get data of note */
	 Not.NotCod = Pub.NotCod;
	 TmlNot_GetNoteDataByCod (&Not);

	 if (Not.Type == TmlNot_POST)
	   {
	    /***** Get post from database *****/
            if (Tml_DB_GetPostDataByCod (Not.Cod,&mysql_res) == Exi_EXISTS)
	      {
	       /* Get row */
	       row = mysql_fetch_row (mysql_res);

	       /* Get only text content (row[0]) */
	       Str_Copy (Content.Txt,row[0],sizeof (Content.Txt) - 1);
	      }

	    /***** Free structure that stores the query result *****/
            DB_FreeMySQLResult (&mysql_res);

	    /***** Copy content string *****/
	    if (GetContent == Ntf_GET_CONTENT)
	      {
	       Length = strlen (Content.Txt);
	       if ((*ContentStr = malloc (Length + 1)) != NULL)
		 {
		  Str_Copy (*ContentStr,Content.Txt,Length);
		  ContentCopied = true;
		 }
	      }

	    /***** Copy summary string *****/
	    Str_LimitLengthHTMLStr (Content.Txt,Ntf_MAX_CHARS_SUMMARY);
	    Str_Copy (SummaryStr,Content.Txt,Ntf_MAX_BYTES_SUMMARY);
	   }
	 else
	    TmlNot_GetNoteSummary (&Not,SummaryStr);
	 break;
      case TmlPub_COMMENT_TO_NOTE:
	 /***** Get content of comment from database *****/
	 if (Tml_DB_GetCommDataByCod (Pub.PubCod,&mysql_res) == Exi_EXISTS)
	   {
	    /* Get row */
	    row = mysql_fetch_row (mysql_res);

	    /* Get only text content (row[4]) */
	    Str_Copy (Content.Txt,row[4],sizeof (Content.Txt) - 1);
	   }

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);

	 /***** Copy content string *****/
	 if (GetContent)
	   {
	    Length = strlen (Content.Txt);
	    if ((*ContentStr = malloc (Length + 1)) != NULL)
	      {
	       Str_Copy (*ContentStr,Content.Txt,Length);
	       ContentCopied = true;
	      }
	   }

	 /***** Copy summary string *****/
	 Str_LimitLengthHTMLStr (Content.Txt,Ntf_MAX_CHARS_SUMMARY);
	 Str_Copy (SummaryStr,Content.Txt,Ntf_MAX_BYTES_SUMMARY);
	 break;
     }

   /***** Create empty content string if nothing copied *****/
   if (GetContent && !ContentCopied)
      if ((*ContentStr = malloc (1)) != NULL)
         (*ContentStr)[0] = '\0';
  }

/*****************************************************************************/
/************ Mark all my notifications about timeline as seen ***************/
/*****************************************************************************/
// Must be executed as a priori function

void TmlNtf_MarkMyNotifAsSeen (void)
  {
   Ntf_DB_MarkNotifsAsSeen (Ntf_EVENT_TML_COMMENT);
   Ntf_DB_MarkNotifsAsSeen (Ntf_EVENT_TML_FAV    );
   Ntf_DB_MarkNotifsAsSeen (Ntf_EVENT_TML_SHARE  );
   Ntf_DB_MarkNotifsAsSeen (Ntf_EVENT_TML_MENTION);
  }
