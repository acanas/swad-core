// swad_message_database.c: messages between users, operations with database

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
// #include <linux/limits.h>	// For PATH_MAX
// #include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
// #include <stdlib.h>		// For free
#include <string.h>		// For string functions
// #include <time.h>		// For time

// #include "swad_action.h"
// #include "swad_box.h"
// #include "swad_config.h"
// #include "swad_course.h"
#include "swad_database.h"
#include "swad_error.h"
// #include "swad_figure.h"
// #include "swad_form.h"
// #include "swad_forum.h"
#include "swad_global.h"
// #include "swad_group.h"
// #include "swad_hierarchy.h"
// #include "swad_hierarchy_level.h"
// #include "swad_HTML.h"
// #include "swad_ID.h"
#include "swad_message.h"
#include "swad_message_database.h"
// #include "swad_notification.h"
// #include "swad_pagination.h"
// #include "swad_parameter.h"
// #include "swad_photo.h"
// #include "swad_profile.h"
// #include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** Private types ******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Expand a sent message **************************/
/*****************************************************************************/

void Msg_DB_ExpandSntMsg (long MsgCod)
  {
   /***** Expand message in sent message table *****/
   DB_QueryUPDATE ("can not expand a sent message",
		   "UPDATE msg_snt"
		     " SET Expanded='Y'"
		   " WHERE MsgCod=%ld"
		     " AND UsrCod=%ld",
                   MsgCod,
                   Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Contract all my other messages in sent message table *****/
   DB_QueryUPDATE ("can not contract a sent message",
		   "UPDATE msg_snt"
		     " SET Expanded='N'"
		   " WHERE UsrCod=%ld"
		     " AND MsgCod<>%ld",
                   Gbl.Usrs.Me.UsrDat.UsrCod,
                   MsgCod);
  }

/*****************************************************************************/
/************************* Expand a received message *************************/
/*****************************************************************************/

void Msg_DB_ExpandRcvMsg (long MsgCod)
  {
   /***** Expand message in received message table and mark it as read by me *****/
   DB_QueryUPDATE ("can not expand a received message",
		   "UPDATE msg_rcv"
		     " SET Open='Y',"
		          "Expanded='Y'"
		   " WHERE MsgCod=%ld"
		     " AND UsrCod=%ld",
                   MsgCod,
                   Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Contract all my other messages in received message table *****/
   DB_QueryUPDATE ("can not contract a received message",
		   "UPDATE msg_rcv"
		     " SET Expanded='N'"
		   " WHERE UsrCod=%ld"
		     " AND MsgCod<>%ld",
                   Gbl.Usrs.Me.UsrDat.UsrCod,
                   MsgCod);
  }

/*****************************************************************************/
/************************** Contract a sent message **************************/
/*****************************************************************************/

void Msg_DB_ContractSntMsg (long MsgCod)
  {
   /***** Contract message in sent message table *****/
   DB_QueryUPDATE ("can not contract a sent message",
		   "UPDATE msg_snt"
		     " SET Expanded='N'"
		   " WHERE MsgCod=%ld"
		     " AND UsrCod=%ld",
                   MsgCod,
                   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************************ Contract a received message ************************/
/*****************************************************************************/

void Msg_DB_ContractRcvMsg (long MsgCod)
  {
   /***** Contract message in received message table *****/
   DB_QueryUPDATE ("can not contract a received message",
		   "UPDATE msg_rcv"
		     " SET Expanded='N'"
		   " WHERE MsgCod=%ld"
		     " AND UsrCod=%ld",
                   MsgCod,
                   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************************* Make "from"/"to" subquery *************************/
/*****************************************************************************/

void Msg_DB_MakeFilterFromToSubquery (const struct Msg_Messages *Messages,
                                      char FilterFromToSubquery[Msg_MAX_BYTES_MESSAGES_QUERY + 1])
  {
   const char *Ptr;
   char SearchWord[Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1];

   /***** Split "from"/"to" string into words *****/
   if (Messages->FilterFromTo[0])
     {
      Ptr = Messages->FilterFromTo;
      Str_Copy (FilterFromToSubquery,
                " AND CONCAT(usr_data.FirstName,' ',usr_data.Surname1,' ',usr_data.Surname2) LIKE '",
                Msg_MAX_BYTES_MESSAGES_QUERY);
      while (*Ptr)
        {
         Str_GetNextStringUntilSpace (&Ptr,SearchWord,Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME);
         if (strlen (FilterFromToSubquery) + strlen (SearchWord) + 512 >
             Msg_MAX_BYTES_MESSAGES_QUERY)	// Prevent string overflow
            break;
         Str_Concat (FilterFromToSubquery,"%",Msg_MAX_BYTES_MESSAGES_QUERY);
         Str_Concat (FilterFromToSubquery,SearchWord,Msg_MAX_BYTES_MESSAGES_QUERY);
        }
      Str_Concat (FilterFromToSubquery,"%'",Msg_MAX_BYTES_MESSAGES_QUERY);
     }
   else
      FilterFromToSubquery[0] = '\0';
  }

/*****************************************************************************/
/********* Generate a query to select messages received or sent **************/
/*****************************************************************************/

unsigned Msg_DB_GetSntOrRcvMsgs (MYSQL_RES **mysql_res,
                                 const struct Msg_Messages *Messages,
			         long UsrCod,
			         const char *FilterFromToSubquery)
  {
   char *SubQuery;
   const char *StrUnreadMsg;
   unsigned NumMsgs;

   if (Messages->FilterCrsCod > 0)	// If origin course selected
      switch (Messages->TypeOfMessages)
        {
         case Msg_RECEIVED:
            StrUnreadMsg = (Messages->ShowOnlyUnreadMsgs ? " AND msg_rcv.Open='N'" :
        	                                           "");
            if (FilterFromToSubquery[0])
              {
               if (asprintf (&SubQuery,"(SELECT msg_rcv.MsgCod"
				         " FROM msg_rcv,"
				               "msg_snt,"
				               "usr_data"
				        " WHERE msg_rcv.UsrCod=%ld%s"
				          " AND msg_rcv.MsgCod=msg_snt.MsgCod"
				          " AND msg_snt.CrsCod=%ld"
				          " AND msg_snt.UsrCod=usr_data.UsrCod%s)"
				       " UNION "
				       "(SELECT msg_rcv.MsgCod"
				         " FROM msg_rcv,"
				               "msg_snt_deleted,"
				               "usr_data"
				        " WHERE msg_rcv.UsrCod=%ld%s"
				          " AND msg_rcv.MsgCod=msg_snt_deleted.MsgCod"
				          " AND msg_snt_deleted.CrsCod=%ld"
				          " AND msg_snt_deleted.UsrCod=usr_data.UsrCod%s)",
			     UsrCod,StrUnreadMsg,Messages->FilterCrsCod,FilterFromToSubquery,
			     UsrCod,StrUnreadMsg,Messages->FilterCrsCod,FilterFromToSubquery) < 0)
                  Err_NotEnoughMemoryExit ();
              }
            else
              {
               if (asprintf (&SubQuery,"(SELECT msg_rcv.MsgCod"
				         " FROM msg_rcv,"
				               "msg_snt"
				        " WHERE msg_rcv.UsrCod=%ld"
				          "%s"
				          " AND msg_rcv.MsgCod=msg_snt.MsgCod"
				          " AND msg_snt.CrsCod=%ld)"
				       " UNION "
				       "(SELECT msg_rcv.MsgCod"
				         " FROM msg_rcv,"
				               "msg_snt_deleted"
				        " WHERE msg_rcv.UsrCod=%ld"
				          "%s"
				          " AND msg_rcv.MsgCod=msg_snt_deleted.MsgCod"
				          " AND msg_snt_deleted.CrsCod=%ld)",
			     UsrCod,StrUnreadMsg,Messages->FilterCrsCod,
			     UsrCod,StrUnreadMsg,Messages->FilterCrsCod) < 0)
                  Err_NotEnoughMemoryExit ();
              }
            break;
         case Msg_SENT:
            if (FilterFromToSubquery[0])
              {
               if (asprintf (&SubQuery,"(SELECT DISTINCT msg_snt.MsgCod"
				         " FROM msg_snt,"
				               "msg_rcv,"
				               "usr_data"
				        " WHERE msg_snt.UsrCod=%ld"
				          " AND msg_snt.CrsCod=%ld"
				          " AND msg_snt.MsgCod=msg_rcv.MsgCod"
				          " AND msg_rcv.UsrCod=usr_data.UsrCod"
				          "%s)"
				       " UNION "
				       "(SELECT DISTINCT msg_snt.MsgCod"
				         " FROM msg_snt,"
				               "msg_rcv_deleted,"
				               "usr_data"
				        " WHERE msg_snt.UsrCod=%ld"
				          " AND msg_snt.CrsCod=%ld"
				          " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
				          " AND msg_rcv_deleted.UsrCod=usr_data.UsrCod"
				          "%s)",
			     UsrCod,Messages->FilterCrsCod,FilterFromToSubquery,
			     UsrCod,Messages->FilterCrsCod,FilterFromToSubquery) < 0)
                  Err_NotEnoughMemoryExit ();
              }
            else
              {
               if (asprintf (&SubQuery,"SELECT MsgCod"
				        " FROM msg_snt"
				       " WHERE UsrCod=%ld"
				         " AND CrsCod=%ld",
			     UsrCod,Messages->FilterCrsCod) < 0)
                  Err_NotEnoughMemoryExit ();
              }
            break;
         default: // Not aplicable here
            break;
        }
   else	// If no origin course selected
      switch (Messages->TypeOfMessages)
        {
         case Msg_RECEIVED:
            if (FilterFromToSubquery[0])
              {
               StrUnreadMsg = (Messages->ShowOnlyUnreadMsgs ? " AND msg_rcv.Open='N'" :
        	                                              "");
               if (asprintf (&SubQuery,"(SELECT msg_rcv.MsgCod"
				         " FROM msg_rcv,"
				               "msg_snt,"
				               "usr_data"
				        " WHERE msg_rcv.UsrCod=%ld"
				          "%s"
				          " AND msg_rcv.MsgCod=msg_snt.MsgCod"
				          " AND msg_snt.UsrCod=usr_data.UsrCod"
				          "%s)"
				       " UNION "
				       "(SELECT msg_rcv.MsgCod"
				         " FROM msg_rcv,"
				               "msg_snt_deleted,"
				               "usr_data"
				        " WHERE msg_rcv.UsrCod=%ld"
				          "%s"
				          " AND msg_rcv.MsgCod=msg_snt_deleted.MsgCod"
				          " AND msg_snt_deleted.UsrCod=usr_data.UsrCod"
				          "%s)",
			     UsrCod,StrUnreadMsg,FilterFromToSubquery,
			     UsrCod,StrUnreadMsg,FilterFromToSubquery) < 0)
                  Err_NotEnoughMemoryExit ();
              }
            else
              {
               StrUnreadMsg = (Messages->ShowOnlyUnreadMsgs ? " AND Open='N'" :
        	                                              "");
               if (asprintf (&SubQuery,"SELECT MsgCod"
				        " FROM msg_rcv"
				       " WHERE UsrCod=%ld"
				          "%s",
			     UsrCod,StrUnreadMsg) < 0)
                  Err_NotEnoughMemoryExit ();
              }
            break;
         case Msg_SENT:
            if (FilterFromToSubquery[0])
              {
               if (asprintf (&SubQuery,"(SELECT msg_snt.MsgCod"
				         " FROM msg_snt,"
				               "msg_rcv,"
				               "usr_data"
				        " WHERE msg_snt.UsrCod=%ld"
				          " AND msg_snt.MsgCod=msg_rcv.MsgCod"
				          " AND msg_rcv.UsrCod=usr_data.UsrCod%s)"
				        " UNION "
				       "(SELECT msg_snt.MsgCod"
				         " FROM msg_snt,"
				               "msg_rcv_deleted,"
				               "usr_data"
				        " WHERE msg_snt.UsrCod=%ld"
				          " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
				          " AND msg_rcv_deleted.UsrCod=usr_data.UsrCod%s)",
			     UsrCod,FilterFromToSubquery,
			     UsrCod,FilterFromToSubquery) < 0)
                  Err_NotEnoughMemoryExit ();
              }
            else
              {
               if (asprintf (&SubQuery,"SELECT MsgCod"
				        " FROM msg_snt"
				       " WHERE UsrCod=%ld",
                             UsrCod) < 0)
                  Err_NotEnoughMemoryExit ();
              }
            break;
         default: // Not aplicable here
            break;
        }

   if (Messages->FilterContent[0])
      /* Match against the content written in filter form */
      NumMsgs = (unsigned)
      DB_QuerySELECT (mysql_res,"can not get messages",
		      "SELECT MsgCod"
		       " FROM msg_content"
		      " WHERE MsgCod IN"
			    " (SELECT MsgCod"
			       " FROM (%s) AS M)"
		        " AND MATCH (Subject,Content) AGAINST ('%s')"
		      " ORDER BY MsgCod DESC",	// End the query ordering the result from most recent message to oldest
		      SubQuery,
		      Messages->FilterContent);
   else
      NumMsgs = (unsigned)
      DB_QuerySELECT (mysql_res,"can not get messages",
		      "%s"
		      " ORDER BY MsgCod DESC",	// End the query ordering the result from most recent message to oldest
		      SubQuery);

   /***** Free memory used for subquery *****/
   free (SubQuery);

   return NumMsgs;
  }

/*****************************************************************************/
/****************** Get subject and content of a message *********************/
/*****************************************************************************/

unsigned Msg_DB_GetSubjectAndContent (MYSQL_RES **mysql_res,long MsgCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get message content",
		   "SELECT Subject,"	// row[0]
			  "Content"	// row[1]
		    " FROM msg_content"
		   " WHERE MsgCod=%ld",
		   MsgCod);
  }

/*****************************************************************************/
/************************ Get the subject of a message ***********************/
/*****************************************************************************/

void Msg_DB_GetMsgSubject (long MsgCod,char Subject[Cns_MAX_BYTES_SUBJECT + 1])
  {
   /***** Get subject of message from database *****/
   DB_QuerySELECTString (Subject,Cns_MAX_BYTES_SUBJECT,
                         "can not get the subject of a message",
			 "SELECT Subject"		// row[0]
			  " FROM msg_content"
			 " WHERE MsgCod=%ld",
			 MsgCod);
  }

/*****************************************************************************/
/********************** Get if a sent message is expanded ********************/
/*****************************************************************************/

bool Msg_DB_GetStatusOfSntMsg (long MsgCod)
  {
   char StrExpanded[1 + 1];

   /***** Get if sent message has been expanded from database *****/
   DB_QuerySELECTString (StrExpanded,1,
                         "can not get if a sent message has been expanded",
		         "SELECT Expanded"		// row[0]
			  " FROM msg_snt"
		         " WHERE MsgCod=%ld"
			   " AND UsrCod=%ld",
		         MsgCod,
		         Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Get if message is expanded *****/
   return (StrExpanded[0] == 'Y');
  }

/*****************************************************************************/
/********* Get if a received message has been open/replied/expanded **********/
/*****************************************************************************/

void Msg_DB_GetStatusOfRcvMsg (long MsgCod,
                               bool *Open,bool *Replied,bool *Expanded)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get if received message has been open/replied/expanded from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get if a received message"
				  " has been replied/expanded",
		       "SELECT Open,"		// row[0]
			      "Replied,"		// row[1]
			      "Expanded"		// row[2]
			" FROM msg_rcv"
		       " WHERE MsgCod=%ld"
			 " AND UsrCod=%ld",
		       MsgCod,
		       Gbl.Usrs.Me.UsrDat.UsrCod) != 1)
      Err_ShowErrorAndExit ("Error when getting if a received message has been replied/expanded.");

   /***** Get number of rows *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get if message has been read by me (row[0]),
              if message has been replied (row[1]), and
              if message is expanded (row[2]) *****/
   *Open     = (row[0][0] == 'Y');
   *Replied  = (row[1][0] == 'Y');
   *Expanded = (row[2][0] == 'Y');

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Get number of user I have banned **********************/
/*****************************************************************************/

unsigned Msg_DB_GetNumUsrsBannedByMe (void)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users you have banned",
		  "SELECT COUNT(*)"
		   " FROM msg_banned"
		  " WHERE ToUsrCod=%ld",
		  Gbl.Usrs.Me.UsrDat.UsrCod);
  }
