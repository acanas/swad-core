// swad_timeline_notification.c: social timeline notifications

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include <stdbool.h>		// For boolean type
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
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/*********** Create a notification for the author of a post/comment **********/
/*****************************************************************************/

void Tml_Ntf_CreateNotifToAuthor (long AuthorCod,long PubCod,
                                  Ntf_NotifyEvent_t NotifyEvent)
  {
   struct UsrData UsrDat;
   bool CreateNotif;
   bool NotifyByEmail;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   UsrDat.UsrCod = AuthorCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
                                                Usr_DONT_GET_PREFS,
                                                Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
     {
      /***** This fav must be notified by email? *****/
      CreateNotif = (UsrDat.NtfEvents.CreateNotif & (1 << NotifyEvent));
      NotifyByEmail = CreateNotif &&
		      (UsrDat.NtfEvents.SendEmail & (1 << NotifyEvent));

      /***** Create notification for the author of the post.
	     If this author wants to receive notifications by email,
	     activate the sending of a notification *****/
      if (CreateNotif)
	 Ntf_DB_StoreNotifyEventToOneUser (NotifyEvent,&UsrDat,PubCod,
					(Ntf_Status_t) (NotifyByEmail ? Ntf_STATUS_BIT_EMAIL :
									0),
					Gbl.Hierarchy.Ins.InsCod,
					Gbl.Hierarchy.Ctr.CtrCod,
					Gbl.Hierarchy.Deg.DegCod,
					Gbl.Hierarchy.Crs.CrsCod);
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/***************** Get notification of a new publication *********************/
/*****************************************************************************/

void Tml_Ntf_GetNotifPublication (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                  char **ContentStr,
                                  long PubCod,bool GetContent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct Tml_Pub_Publication Pub;
   struct Tml_Not_Note Not;
   struct Tml_Pst_Content Content;
   size_t Length;
   bool ContentCopied = false;

   /***** Return nothing on error *****/
   Pub.PubType = Tml_Pub_UNKNOWN;
   SummaryStr[0]  = '\0';	// Return nothing on error
   Content.Txt[0] = '\0';

   /***** Get summary and content from post from database *****/
   if (Tml_DB_GetDataOfPubByCod (PubCod,&mysql_res) == 1)   // Result should have a unique row
      /* Get data of publication from row */
      Tml_Pub_GetDataOfPubFromNextRow (mysql_res,&Pub);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Get summary and content *****/
   switch (Pub.PubType)
     {
      case Tml_Pub_UNKNOWN:
	 break;
      case Tml_Pub_ORIGINAL_NOTE:
      case Tml_Pub_SHARED_NOTE:
	 /* Get data of note */
	 Not.NotCod = Pub.NotCod;
	 Tml_Not_GetDataOfNoteByCod (&Not);

	 if (Not.NoteType == TL_NOTE_POST)
	   {
	    /***** Get post from database *****/
            if (Tml_DB_GetPostByCod (Not.Cod,&mysql_res) == 1)   // Result should have a unique row
	      {
	       /* Get row */
	       row = mysql_fetch_row (mysql_res);

	       /* Get only text content (row[0]) */
	       Str_Copy (Content.Txt,row[0],sizeof (Content.Txt) - 1);
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
	   }
	 else
	    Tml_Not_GetNoteSummary (&Not,SummaryStr);
	 break;
      case Tml_Pub_COMMENT_TO_NOTE:
	 /***** Get content of comment from database *****/
	 if (Tml_DB_GetDataOfCommByCod (Pub.PubCod,&mysql_res) == 1)   // Result should have a unique row
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

void Tml_Ntf_MarkMyNotifAsSeen (void)
  {
   Ntf_DB_MarkNotifAsSeen (Ntf_EVENT_TL_COMMENT,-1L,-1L,Gbl.Usrs.Me.UsrDat.UsrCod);
   Ntf_DB_MarkNotifAsSeen (Ntf_EVENT_TL_FAV    ,-1L,-1L,Gbl.Usrs.Me.UsrDat.UsrCod);
   Ntf_DB_MarkNotifAsSeen (Ntf_EVENT_TL_SHARE  ,-1L,-1L,Gbl.Usrs.Me.UsrDat.UsrCod);
   Ntf_DB_MarkNotifAsSeen (Ntf_EVENT_TL_MENTION,-1L,-1L,Gbl.Usrs.Me.UsrDat.UsrCod);
  }
