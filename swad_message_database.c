// swad_message_database.c: messages between users, operations with database

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
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_error.h"
#include "swad_exist.h"
#include "swad_global.h"
#include "swad_message.h"
#include "swad_message_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************* Insert message subject and content in the database ************/
/*****************************************************************************/
// Return the code of the new inserted message

long Msg_DB_CreateNewMsg (const char *Subject,const char *Content,long MedCod)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create message",
				"INSERT INTO msg_content"
				" (Subject,Content,MedCod)"
				" VALUES"
				" ('%s','%s',%ld)",
				Subject,
				Content,
				MedCod);
  }

/*****************************************************************************/
/************* Insert a message in the table of sent messages ****************/
/*****************************************************************************/

void Msg_DB_CreateSntMsg (long MsgCod,long CrsCod)
  {
   DB_QueryINSERT ("can not create sent message",
		   "INSERT INTO msg_snt"
		   " (MsgCod,CrsCod,UsrCod,Expanded,CreatTime)"
		   " VALUES"
		   " (%ld,%ld,%ld,'N',NOW())",
	           MsgCod,
	           CrsCod,
	           Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*********** Insert a message in the table of received messages **************/
/*****************************************************************************/

void Msg_DB_CreateRcvMsg (long MsgCod,long UsrCod,bool NotifyByEmail)
  {
   DB_QueryINSERT ("can not create received message",
		   "INSERT INTO msg_rcv"
		   " (MsgCod,UsrCod,Notified,Open,Replied,Expanded)"
		   " VALUES"
		   " (%ld,%ld,'%c','N','N','N')",
	           MsgCod,
	           UsrCod,
	           NotifyByEmail ? 'Y' :
			           'N');
  }

/*****************************************************************************/
/******** Update received message by setting Replied field to true ***********/
/*****************************************************************************/

void Msg_DB_SetRcvMsgAsReplied (long MsgCod)
  {
   DB_QueryUPDATE ("can not update a received message",
		   "UPDATE msg_rcv"
		     " SET Replied='Y'"
		   " WHERE MsgCod=%ld"
		     " AND UsrCod=%ld",
                   MsgCod,
                   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

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
/********************** Mark a received message as open **********************/
/*****************************************************************************/

void Msg_DB_SetRcvMsgAsOpen (long MsgCod,long UsrCod)
  {
   /***** Mark message as read by user *****/
   DB_QueryUPDATE ("can not mark a received message as open",
		   "UPDATE msg_rcv"
		     " SET Open='Y'"
		   " WHERE MsgCod=%ld"
		     " AND UsrCod=%ld",
                   MsgCod,
                   UsrCod);
  }

/*****************************************************************************/
/*************** Get dictinct courses in my received messages ****************/
/*****************************************************************************/

unsigned Msg_DB_GetDistinctCrssInMyRcvMsgs (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get distinct courses in your messages",
		   "SELECT DISTINCT "
		          "crs_courses.CrsCod,"		// row[0]
			  "crs_courses.ShortName"	// row[1]
		    " FROM msg_rcv,"
			  "msg_snt,"
			  "crs_courses"
		   " WHERE msg_rcv.UsrCod=%ld"
		     " AND msg_rcv.MsgCod=msg_snt.MsgCod"
		     " AND msg_snt.CrsCod=crs_courses.CrsCod"
		" ORDER BY crs_courses.ShortName",
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/***************** Get dictinct courses in my sent messages ******************/
/*****************************************************************************/

unsigned Msg_DB_GetDistinctCrssInMySntMsgs (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get distinct courses in your messages",
		   "SELECT DISTINCT "
		          "crs_courses.CrsCod,"		// row[0]
			  "crs_courses.ShortName"	// row[1]
		    " FROM msg_snt,"
			  "crs_courses"
		   " WHERE msg_snt.UsrCod=%ld"
		     " AND msg_snt.CrsCod=crs_courses.CrsCod"
		" ORDER BY crs_courses.ShortName",
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************************* Make "from"/"to" subquery *************************/
/*****************************************************************************/

void Msg_DB_MakeFilterFromToSubquery (const struct Msg_Messages *Messages,
                                      char FilterFromToSubquery[Msg_DB_MAX_BYTES_MESSAGES_QUERY + 1])
  {
   const char *Ptr;
   char SearchWord[Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1];

   /***** Split "from"/"to" string into words *****/
   if (Messages->FilterFromTo[0])
     {
      Str_Copy (FilterFromToSubquery,
                " AND CONCAT(usr_data.FirstName,' ',usr_data.Surname1,' ',usr_data.Surname2) LIKE '",
                Msg_DB_MAX_BYTES_MESSAGES_QUERY);
      for (Ptr = Messages->FilterFromTo;
           *Ptr;
          )
        {
         Str_GetNextStringUntilSpace (&Ptr,SearchWord,Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME);
         if (strlen (FilterFromToSubquery) + strlen (SearchWord) + 512 >
             Msg_DB_MAX_BYTES_MESSAGES_QUERY)	// Prevent string overflow
            break;
         Str_Concat (FilterFromToSubquery,"%",Msg_DB_MAX_BYTES_MESSAGES_QUERY);
         Str_Concat (FilterFromToSubquery,SearchWord,Msg_DB_MAX_BYTES_MESSAGES_QUERY);
        }
      Str_Concat (FilterFromToSubquery,"%'",Msg_DB_MAX_BYTES_MESSAGES_QUERY);
     }
   else
      FilterFromToSubquery[0] = '\0';
  }

/*****************************************************************************/
/******** Get number of received messages that haven't been read by me *******/
/*****************************************************************************/

unsigned Msg_DB_GetNumUnreadMsgs (const struct Msg_Messages *Messages,
                                  const char FilterFromToSubquery[Msg_DB_MAX_BYTES_MESSAGES_QUERY + 1])
  {
   char *SubQuery;
   unsigned NumMsgs;

   /***** Get number of unread messages from database *****/
   if (Messages->FilterCrsCod > 0)	// If origin course selected
     {
      if (FilterFromToSubquery[0])
	{
         if (asprintf (&SubQuery,"SELECT msg_rcv.MsgCod"
        	                  " FROM msg_rcv,"
        	                        "msg_snt,"
        	                        "usr_data"
				 " WHERE msg_rcv.UsrCod=%ld"
				   " AND msg_rcv.Open='N'"
				   " AND msg_rcv.MsgCod=msg_snt.MsgCod"
				   " AND msg_snt.CrsCod=%ld"
				   " AND msg_snt.UsrCod=usr_data.UsrCod%s",
		       Gbl.Usrs.Me.UsrDat.UsrCod,
		       Messages->FilterCrsCod,
		       FilterFromToSubquery) < 0)
            Err_NotEnoughMemoryExit ();
	}
      else
        {
         if (asprintf (&SubQuery,"SELECT msg_rcv.MsgCod"
        	                  " FROM msg_rcv,"
        	                        "msg_snt"
				 " WHERE msg_rcv.UsrCod=%ld"
				   " AND msg_rcv.Open='N'"
				   " AND msg_rcv.MsgCod=msg_snt.MsgCod"
				   " AND msg_snt.CrsCod=%ld",
			Gbl.Usrs.Me.UsrDat.UsrCod,
			Messages->FilterCrsCod) < 0)
            Err_NotEnoughMemoryExit ();
        }
     }
   else	// If no origin course selected
     {
      if (FilterFromToSubquery[0])
	{
         if (asprintf (&SubQuery,"SELECT msg_rcv.MsgCod"
        	                  " FROM msg_rcv,"
        	                        "msg_snt,"
        	                        "usr_data"
				 " WHERE msg_rcv.UsrCod=%ld"
				   " AND msg_rcv.Open='N'"
				   " AND msg_rcv.MsgCod=msg_snt.MsgCod"
				   " AND msg_snt.UsrCod=usr_data.UsrCod%s",
			Gbl.Usrs.Me.UsrDat.UsrCod,
			FilterFromToSubquery) < 0)
	    Err_NotEnoughMemoryExit ();
        }
      else
	{
         if (asprintf (&SubQuery,"SELECT MsgCod"
        	                  " FROM msg_rcv"
			         " WHERE UsrCod=%ld"
			           " AND Open='N'",
		        Gbl.Usrs.Me.UsrDat.UsrCod) < 0)
	    Err_NotEnoughMemoryExit ();
        }
     }

   if (Messages->FilterContent[0])
      NumMsgs = (unsigned)
      DB_QueryCOUNT ("can not get number of unread messages",
		     "SELECT COUNT(*)"
		      " FROM msg_content"
		     " WHERE MsgCod IN (%s)"
		       " AND MATCH (Subject,Content) AGAINST ('%s')",
		     SubQuery,
		     Messages->FilterContent);
   else
      NumMsgs = (unsigned)
      DB_QueryCOUNT ("can not get number of unread messages",
		     "SELECT COUNT(*)"
		      " FROM (%s) AS T",
		     SubQuery);

   free (SubQuery);

   return NumMsgs;
  }

/*****************************************************************************/
/********* Generate a query to select messages received or sent **************/
/*****************************************************************************/

unsigned Msg_DB_GetSntOrRcvMsgs (MYSQL_RES **mysql_res,
                                 const struct Msg_Messages *Messages,
			         long UsrCod,
			         const char FilterFromToSubquery[Msg_DB_MAX_BYTES_MESSAGES_QUERY + 1])
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
               if (asprintf (&SubQuery,"(SELECT DISTINCT "
        		                       "msg_snt.MsgCod"
				         " FROM msg_snt,"
				               "msg_rcv,"
				               "usr_data"
				        " WHERE msg_snt.UsrCod=%ld"
				          " AND msg_snt.CrsCod=%ld"
				          " AND msg_snt.MsgCod=msg_rcv.MsgCod"
				          " AND msg_rcv.UsrCod=usr_data.UsrCod"
				            "%s)"
				        " UNION "
				       "(SELECT DISTINCT "
				               "msg_snt.MsgCod"
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

Exi_Exist_t Msg_DB_GetSubjectAndContent (MYSQL_RES **mysql_res,long MsgCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get message content",
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
/*************** Get content and optional image of a message *****************/
/*****************************************************************************/

Exi_Exist_t Msg_DB_GetMsgContent (MYSQL_RES **mysql_res,long MsgCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get the content of a message",
			 "SELECT Content,"	// row[0]
				"MedCod"	// row[1]
			  " FROM msg_content"
			 " WHERE MsgCod=%ld",
			 MsgCod);
  }

/*****************************************************************************/
/***************************** Get data of a message *************************/
/*****************************************************************************/

unsigned Msg_DB_GetMsgSntData (MYSQL_RES **mysql_res,long MsgCod,bool *Deleted)
  {
   unsigned NumRows;

   /***** Get data of message from table msg_snt *****/
   *Deleted = false;
   NumRows = (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of a message",
		   "SELECT CrsCod,"				// row[0]
			  "UsrCod,"				// row[1]
			  "UNIX_TIMESTAMP(CreatTime)"		// row[2]
		    " FROM msg_snt"
		   " WHERE MsgCod=%ld",
		   MsgCod);

   if (NumRows == 0)   // If not result ==> sent message is deleted
     {
      /***** Get data of message from table msg_snt_deleted *****/
      NumRows = (unsigned)
      DB_QuerySELECT (mysql_res,"can not get data of a message",
		      "SELECT CrsCod,"				// row[0]
			     "UsrCod,"				// row[1]
			     "UNIX_TIMESTAMP(CreatTime)"	// row[2]
		       " FROM msg_snt_deleted"
		      " WHERE MsgCod=%ld",
		      MsgCod);

      *Deleted = true;
     }

   return NumRows;
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

void Msg_DB_GetStatusOfRcvMsg (long MsgCod,CloOpe_ClosedOrOpen_t *Open,
			       bool *Replied,bool *Expanded)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get if received message has been open/replied/expanded from database *****/
   if (DB_QuerySELECTunique (&mysql_res,"can not get if a received message"
					" has been replied/expanded",
			     "SELECT Open,"	// row[0]
				    "Replied,"	// row[1]
				    "Expanded"	// row[2]
			      " FROM msg_rcv"
			     " WHERE MsgCod=%ld"
			       " AND UsrCod=%ld",
			     MsgCod,
			     Gbl.Usrs.Me.UsrDat.UsrCod) == Exi_DOES_NOT_EXIST)
      Err_ShowErrorAndExit ("Error when getting if a received message has been replied/expanded.");

   /***** Get number of rows *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get if message has been read by me (row[0]),
              if message has been replied (row[1]), and
              if message is expanded (row[2]) *****/
   *Open     = CloOpe_GetOpenFromYN (row[0][0]);
   *Replied  = (row[1][0] == 'Y');
   *Expanded = (row[2][0] == 'Y');

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************* Get the sender of a message ***********************/
/*****************************************************************************/

long Msg_DB_GetSender (long MsgCod)
  {
   return
   DB_QuerySELECTCode ("can not get sender",
		       "SELECT UsrCod"
		        " FROM msg_snt"
		       " WHERE MsgCod=%ld"
		       " UNION "
		       "SELECT UsrCod"
		        " FROM msg_snt_deleted"
		       " WHERE MsgCod=%ld",
		       MsgCod,
		       MsgCod);
  }

/*****************************************************************************/
/************** Check if a message has been really received by me ************/
/*****************************************************************************/

bool Msg_DB_CheckIfMsgHasBeenReceivedByMe (long MsgCod)
  {
   return
   DB_QueryEXISTS ("can not check original message",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM msg_rcv"
		    " WHERE UsrCod=%ld"
		      " AND MsgCod=%ld"
		   " UNION"
		   " SELECT *"
		     " FROM msg_rcv_deleted"
		    " WHERE UsrCod=%ld"
		      " AND MsgCod=%ld)",
		   Gbl.Usrs.Me.UsrDat.UsrCod,MsgCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,MsgCod) == Exi_EXISTS;
  }

/*****************************************************************************/
/***************************** Get recipients' codes *************************/
/*****************************************************************************/

#define Msg_DB_MAX_BYTES_QUERY_RECIPIENTS (16 * 1024 - 1)

unsigned Msg_DB_GetRecipientsCods (MYSQL_RES **mysql_res,
                                   long ReplyUsrCod,const char *ListRecipients)
  {
   char *Query = NULL;
   const char *Ptr;
   char Nick[Nck_MAX_BYTES_NICK_WITH_ARROBA + 1];
   bool FirstNickname = true;
   bool ThereAreNicknames = false;
   unsigned NumUsrs = 0;

   /***** Allocate space for query *****/
   if ((Query = malloc (Msg_DB_MAX_BYTES_QUERY_RECIPIENTS + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Build query for recipients from database *****/
   if (ReplyUsrCod > 0)
      snprintf (Query,Msg_DB_MAX_BYTES_QUERY_RECIPIENTS + 1,
	        "SELECT UsrCod"
	         " FROM usr_data"
	        " WHERE UsrCod=%ld",
	        ReplyUsrCod);
   else
      Query[0] = '\0';

   /***** Loop over recipients' nicknames building query *****/
   for (Ptr = ListRecipients;
        *Ptr;
       )
     {
      /* Find next string in text until comma (leading and trailing spaces are removed) */
      Str_GetNextStringUntilComma (&Ptr,Nick,sizeof (Nick) - 1);	// With leading arrobas

      /* Check if string is a valid nickname */
      if (Nck_CheckIfNickWithArrIsValid (Nick) == Err_SUCCESS)	// String is a nickname (with leading arrobas)?
	{
         Str_RemoveLeadingArrobas (Nick);

	 /* Check for overflow in query */
	 if (strlen (Query) + Nck_MAX_BYTES_NICK_WITHOUT_ARROBA + 32 >
	     Msg_DB_MAX_BYTES_QUERY_RECIPIENTS)
            Err_NotEnoughMemoryExit ();

	 /* Add this nickname to query */
	 if (FirstNickname)
	   {
	    if (ReplyUsrCod > 0)
	       Str_Concat (Query," UNION ",Msg_DB_MAX_BYTES_QUERY_RECIPIENTS);
	    Str_Concat (Query,"SELECT UsrCod"
		               " FROM usr_nicknames"
			      " WHERE Nickname IN ('",
			Msg_DB_MAX_BYTES_QUERY_RECIPIENTS);
	    FirstNickname = false;
	    ThereAreNicknames = true;
	   }
	 else
	    Str_Concat (Query,",'",Msg_DB_MAX_BYTES_QUERY_RECIPIENTS);
	 Str_Concat (Query,Nick,Msg_DB_MAX_BYTES_QUERY_RECIPIENTS);	// Leading arrobas already removed
	 Str_Concat (Query,"'",Msg_DB_MAX_BYTES_QUERY_RECIPIENTS);
	}
     }
   if (ThereAreNicknames)
      Str_Concat (Query,")",Msg_DB_MAX_BYTES_QUERY_RECIPIENTS);

   if (ReplyUsrCod > 0 || ThereAreNicknames)	// There are a recipient to reply or nicknames in list
     {
      /***** Get users *****/
      NumUsrs = (unsigned)
      DB_QuerySELECT (mysql_res,"can not get users",
		      "%s",
	              Query);
     }

   /***** Free space for query *****/
   free (Query);

   return NumUsrs;
  }

/*****************************************************************************/
/************ Get number of recipients of a message from database ************/
/*****************************************************************************/

unsigned Msg_DB_GetNumRecipients (long MsgCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of recipients",
		  "SELECT "
		  "(SELECT COUNT(*)"
		    " FROM msg_rcv"
		   " WHERE MsgCod=%ld)"
		  " + "
		  "(SELECT COUNT(*)"
		    " FROM msg_rcv_deleted"
		   " WHERE MsgCod=%ld)",
		  MsgCod,
		  MsgCod);
  }

/*****************************************************************************/
/*************** Get known recipients of a message from database *************/
/*****************************************************************************/

unsigned Msg_DB_GetKnownRecipients (MYSQL_RES **mysql_res,long MsgCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get recipients of a message",
		   "(SELECT msg_rcv.UsrCod,"		// row[0]
			   "'N',"			// row[1]
			   "msg_rcv.Open,"		// row[2]
			   "usr_data.Surname1 AS S1,"	// row[3]
			   "usr_data.Surname2 AS S2,"	// row[4]
			   "usr_data.FirstName AS FN"	// row[5]
		     " FROM msg_rcv,"
		 	   "usr_data"
		    " WHERE msg_rcv.MsgCod=%ld"
		      " AND msg_rcv.UsrCod=usr_data.UsrCod)"
		    " UNION "
		   "(SELECT msg_rcv_deleted.UsrCod,"	// row[0]
			   "'Y',"			// row[1]
			   "msg_rcv_deleted.Open,"	// row[2]
			   "usr_data.Surname1 AS S1,"	// row[3]
			   "usr_data.Surname2 AS S2,"	// row[4]
			   "usr_data.FirstName AS FN"	// row[5]
		     " FROM msg_rcv_deleted,"
			   "usr_data"
		    " WHERE msg_rcv_deleted.MsgCod=%ld"
		      " AND msg_rcv_deleted.UsrCod=usr_data.UsrCod)"
		 " ORDER BY S1,"
			   "S2,"
			   "FN",
		   MsgCod,
		   MsgCod);
  }

/*****************************************************************************/
/******** Get the number of unique messages sent from this location **********/
/******** (all the platform, current degree or current course)      **********/
/*****************************************************************************/

unsigned Msg_DB_GetNumSntMsgs (Hie_Level_t HieLvl,Msg_Status_t MsgStatus)
  {
   static const char *Table[Msg_NUM_STATUS] =
     {
      [Msg_STATUS_ALL     ] = "msg_snt",
      [Msg_STATUS_DELETED ] = "msg_snt_deleted",
      [Msg_STATUS_NOTIFIED] = "msg_snt",
     };

   /***** Get the number of messages sent from this location
          (all the platform, current degree or current course) from database *****/
   switch (HieLvl)
     {
      case Hie_SYS:
	 return (unsigned) DB_GetNumRowsTable (Table[MsgStatus]);
      case Hie_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of sent messages",
		        "SELECT COUNT(*)"
		         " FROM ins_instits,"
		               "ctr_centers,"
		               "deg_degrees,"
		               "crs_courses,"
		               "%s"
		        " WHERE ins_instits.CtyCod=%ld"
		          " AND ins_instits.InsCod=ctr_centers.InsCod"
		          " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		          " AND deg_degrees.DegCod=crs_courses.DegCod"
		          " AND crs_courses.CrsCod=%s.CrsCod",
		        Table[MsgStatus],
		        Gbl.Hierarchy.Node[Hie_CTY].HieCod,
		        Table[MsgStatus]);
      case Hie_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of sent messages",
		        "SELECT COUNT(*)"
		         " FROM ctr_centers,"
		               "deg_degrees,"
		               "crs_courses,"
		               "%s"
		        " WHERE ctr_centers.InsCod=%ld"
		          " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		          " AND deg_degrees.DegCod=crs_courses.DegCod"
		          " AND crs_courses.CrsCod=%s.CrsCod",
		        Table[MsgStatus],
		        Gbl.Hierarchy.Node[Hie_INS].HieCod,
		        Table[MsgStatus]);
      case Hie_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of sent messages",
		        "SELECT COUNT(*)"
		         " FROM deg_degrees,"
		               "crs_courses,"
		               "%s"
		        " WHERE deg_degrees.CtrCod=%ld"
		          " AND deg_degrees.DegCod=crs_courses.DegCod"
		          " AND crs_courses.CrsCod=%s.CrsCod",
		        Table[MsgStatus],
		        Gbl.Hierarchy.Node[Hie_CTR].HieCod,
		        Table[MsgStatus]);
      case Hie_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of sent messages",
		        "SELECT COUNT(*)"
		         " FROM crs_courses,"
		               "%s"
		        " WHERE crs_courses.DegCod=%ld"
		          " AND crs_courses.CrsCod=%s.CrsCod",
		        Table[MsgStatus],
		        Gbl.Hierarchy.Node[Hie_DEG].HieCod,
		        Table[MsgStatus]);
      case Hie_CRS:
         return (unsigned)
	 DB_QueryCOUNT ("can not get number of sent messages",
		        "SELECT COUNT(*)"
		         " FROM %s"
		        " WHERE CrsCod=%ld",
		        Table[MsgStatus],
		        Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      default:
	 return 0;
     }
  }

/*****************************************************************************/
/****** Get the number of unique messages received from this location ********/
/****** (all the platform, current degree or current course)          ********/
/*****************************************************************************/

unsigned Msg_DB_GetNumRcvMsgs (Hie_Level_t HieLvl,Msg_Status_t MsgStatus)
  {
   static const char *Table[Msg_NUM_STATUS] =
     {
      [Msg_STATUS_ALL     ] = "msg_rcv",
      [Msg_STATUS_DELETED ] = "msg_rcv_deleted",
      [Msg_STATUS_NOTIFIED] = "msg_rcv",	// Not used
     };

   /***** Get the number of unique messages sent from this location
          (all the platform, current degree or current course) from database *****/
   switch (MsgStatus)
     {
      case Msg_STATUS_ALL:
      case Msg_STATUS_DELETED:
         switch (HieLvl)
           {
            case Hie_SYS:
               return (unsigned) DB_GetNumRowsTable (Table[MsgStatus]);
            case Hie_CTY:
               return (unsigned)
               DB_QueryCOUNT ("can not get number of received messages",
			      "SELECT COUNT(*)"
			       " FROM ins_instits,"
			             "ctr_centers,"
			             "deg_degrees,"
			             "crs_courses,"
			             "%s,"
			             "msg_snt"
			      " WHERE ins_instits.CtyCod=%ld"
			        " AND ins_instits.InsCod=ctr_centers.InsCod"
			        " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			        " AND deg_degrees.DegCod=crs_courses.DegCod"
			        " AND crs_courses.CrsCod=msg_snt.CrsCod"
			        " AND msg_snt.MsgCod=%s.MsgCod",
			      Table[MsgStatus],
			      Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			      Table[MsgStatus]);
            case Hie_INS:
               return (unsigned)
               DB_QueryCOUNT ("can not get number of received messages",
			      "SELECT COUNT(*)"
			       " FROM ctr_centers,"
			             "deg_degrees,"
			             "crs_courses,"
			             "%s,"
			             "msg_snt"
			      " WHERE ctr_centers.InsCod=%ld"
			        " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			        " AND deg_degrees.DegCod=crs_courses.DegCod"
			        " AND crs_courses.CrsCod=msg_snt.CrsCod"
			        " AND msg_snt.MsgCod=%s.MsgCod",
			      Table[MsgStatus],
			      Gbl.Hierarchy.Node[Hie_INS].HieCod,
			      Table[MsgStatus]);
            case Hie_CTR:
               return (unsigned)
               DB_QueryCOUNT ("can not get number of received messages",
			      "SELECT COUNT(*)"
			       " FROM deg_degrees,"
			             "crs_courses,"
			             "%s,"
			             "msg_snt"
			      " WHERE deg_degrees.CtrCod=%ld"
			        " AND deg_degrees.DegCod=crs_courses.DegCod"
			        " AND crs_courses.CrsCod=msg_snt.CrsCod"
			        " AND msg_snt.MsgCod=%s.MsgCod",
			      Table[MsgStatus],
			      Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			      Table[MsgStatus]);
            case Hie_DEG:
               return (unsigned)
               DB_QueryCOUNT ("can not get number of received messages",
			      "SELECT COUNT(*)"
			       " FROM crs_courses,"
			             "%s,"
			             "msg_snt"
			      " WHERE crs_courses.DegCod=%ld"
			        " AND crs_courses.CrsCod=msg_snt.CrsCod"
			        " AND msg_snt.MsgCod=%s.MsgCod",
			      Table[MsgStatus],
			      Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			      Table[MsgStatus]);
            case Hie_CRS:
               return (unsigned)
               DB_QueryCOUNT ("can not get number of received messages",
			      "SELECT COUNT(*)"
			       " FROM msg_snt,"
			             "%s"
			      " WHERE msg_snt.CrsCod=%ld"
			        " AND msg_snt.MsgCod=%s.MsgCod",
			      Table[MsgStatus],
			      Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			      Table[MsgStatus]);
            case Hie_UNK:
	    default:
	       return 0;
           }
         return 0;
      case Msg_STATUS_NOTIFIED:
         switch (HieLvl)
           {
            case Hie_SYS:
               return (unsigned)
               DB_QueryCOUNT ("can not get number of received messages",
			      "SELECT "
			      "(SELECT COUNT(*)"
			        " FROM msg_rcv"
			       " WHERE Notified='Y')"
			      " + "
			      "(SELECT COUNT(*)"
			        " FROM msg_rcv_deleted"
			       " WHERE Notified='Y')");
            case Hie_CTY:
               return (unsigned)
               DB_QueryCOUNT ("can not get number of received messages",
			      "SELECT "
			      "(SELECT COUNT(*)"
			        " FROM ins_instits,"
			              "ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "msg_snt,"
			              "msg_rcv"
			       " WHERE ins_instits.CtyCod=%ld"
			         " AND ins_instits.InsCod=ctr_centers.InsCod"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=msg_snt.CrsCod"
			         " AND msg_snt.MsgCod=msg_rcv.MsgCod"
			         " AND msg_rcv.Notified='Y')"
			      " + "
			      "(SELECT COUNT(*)"
			        " FROM ins_instits,"
			              "ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "msg_snt,"
			              "msg_rcv_deleted"
			       " WHERE ins_instits.CtyCod=%ld"
			         " AND ins_instits.InsCod=ctr_centers.InsCod"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=msg_snt.CrsCod"
			         " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
			         " AND msg_rcv_deleted.Notified='Y')",
			      Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			      Gbl.Hierarchy.Node[Hie_CTY].HieCod);
            case Hie_INS:
               return (unsigned)
               DB_QueryCOUNT ("can not get number of received messages",
			      "SELECT "
			      "(SELECT COUNT(*)"
			        " FROM ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "msg_snt,"
			              "msg_rcv"
			       " WHERE ctr_centers.InsCod=%ld"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=msg_snt.CrsCod"
			         " AND msg_snt.MsgCod=msg_rcv.MsgCod"
			         " AND msg_rcv.Notified='Y')"
			      " + "
			      "(SELECT COUNT(*)"
			        " FROM ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "msg_snt,"
			              "msg_rcv_deleted"
			       " WHERE ctr_centers.InsCod=%ld"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=msg_snt.CrsCod"
			         " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
			         " AND msg_rcv_deleted.Notified='Y')",
			      Gbl.Hierarchy.Node[Hie_INS].HieCod,
			      Gbl.Hierarchy.Node[Hie_INS].HieCod);
            case Hie_CTR:
               return (unsigned)
               DB_QueryCOUNT ("can not get number of received messages",
			      "SELECT "
			      "(SELECT COUNT(*)"
			        " FROM deg_degrees,"
			              "crs_courses,"
			              "msg_snt,"
			              "msg_rcv"
			       " WHERE deg_degrees.CtrCod=%ld"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=msg_snt.CrsCod"
			         " AND msg_snt.MsgCod=msg_rcv.MsgCod"
			         " AND msg_rcv.Notified='Y')"
			      " + "
			      "(SELECT COUNT(*)"
			        " FROM deg_degrees,"
			              "crs_courses,"
			              "msg_snt,"
			              "msg_rcv_deleted"
			       " WHERE deg_degrees.CtrCod=%ld"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=msg_snt.CrsCod"
			         " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
			         " AND msg_rcv_deleted.Notified='Y')",
			      Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			      Gbl.Hierarchy.Node[Hie_CTR].HieCod);
            case Hie_DEG:
               return (unsigned)
               DB_QueryCOUNT ("can not get number of received messages",
			      "SELECT "
			      "(SELECT COUNT(*)"
			        " FROM crs_courses,"
			              "msg_snt,"
			              "msg_rcv"
			       " WHERE crs_courses.DegCod=%ld"
			         " AND crs_courses.CrsCod=msg_snt.CrsCod"
			         " AND msg_snt.MsgCod=msg_rcv.MsgCod"
			         " AND msg_rcv.Notified='Y')"
			      " + "
			      "(SELECT COUNT(*)"
			        " FROM crs_courses,"
			              "msg_snt,"
			              "msg_rcv_deleted"
			       " WHERE crs_courses.DegCod=%ld"
			         " AND crs_courses.CrsCod=msg_snt.CrsCod"
			         " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
			         " AND msg_rcv_deleted.Notified='Y')",
			      Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			      Gbl.Hierarchy.Node[Hie_DEG].HieCod);
            case Hie_CRS:
               return (unsigned)
               DB_QueryCOUNT ("can not get number of received messages",
			      "SELECT "
			      "(SELECT COUNT(*)"
			        " FROM msg_snt,"
			              "msg_rcv"
			       " WHERE msg_snt.CrsCod=%ld"
			         " AND msg_snt.MsgCod=msg_rcv.MsgCod"
			         " AND msg_rcv.Notified='Y')"
			      " + "
			      "(SELECT COUNT(*)"
			        " FROM msg_snt,"
			              "msg_rcv_deleted"
			       " WHERE msg_snt.CrsCod=%ld"
			         " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
			         " AND msg_rcv_deleted.Notified='Y')",
			      Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			      Gbl.Hierarchy.Node[Hie_CRS].HieCod);
            case Hie_UNK:
	    default:
	       return 0;
           }
         return 0;
      default:
         return 0;
     }
  }

/*****************************************************************************/
/**** Get the number of unique messages sent by any teacher from a course ****/
/*****************************************************************************/

unsigned Msg_DB_GetNumMsgsSentByTchsCrs (long CrsCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of messages sent by teachers",
		  "SELECT COUNT(*)"
		   " FROM msg_snt,"
			 "crs_users"
		  " WHERE msg_snt.CrsCod=%ld"
		    " AND crs_users.CrsCod=%ld"
		    " AND crs_users.Role IN (%u,%u)"
		    " AND msg_snt.UsrCod=crs_users.UsrCod",
		  CrsCod,
		  CrsCod,
		  (unsigned) Rol_NET,	// Non-editing teacher
		  (unsigned) Rol_TCH);	// Teacher
  }

/*****************************************************************************/
/************** Get the number of unique messages sent by a user *************/
/*****************************************************************************/

unsigned Msg_DB_GetNumMsgsSentByUsr (long UsrCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of messages sent by a user",
		  "SELECT"
		  " (SELECT COUNT(*)"
		     " FROM msg_snt"
		    " WHERE UsrCod=%ld)"
		  " +"
		  " (SELECT COUNT(*)"
		     " FROM msg_snt_deleted"
		    " WHERE UsrCod=%ld)",
		  UsrCod,
		  UsrCod);
  }

/*****************************************************************************/
/****************** Remove message from received messages ********************/
/*****************************************************************************/

void Msg_DB_RemoveRcvMsg (long MsgCod,long UsrCod)
  {
   /***** Step 1: Move message content from msg_content to msg_content_deleted
		  if pointed only by one unique recipient and none sender *****/
   /*
                                                 msg_rcv
                       msg_content              ________
     msg_snt          ______________   Only    |________|
    ________         |______________|  one     |________|
   |________|        |______________|  message |________|
   |________|  \ /   |______________|  received|________|
   |________|---x--->|____MsgCod____|<-points--|_UsrCod_|
   |________|  / \   |______________|  here    |________|
   |________|        |______________|          |________|
                     |______________|          |________|
                                               |________|
   */
   /* Insert message content into msg_content_deleted */
   DB_QueryINSERT ("can not remove the content of a message",
		   "INSERT IGNORE INTO msg_content_deleted"
		   " (MsgCod,Subject,Content,MedCod)"
		   " SELECT MsgCod,"
			   "Subject,"
			   "Content,"
			   "MedCod"
		    " FROM msg_content"
		   " WHERE MsgCod=%ld"
		     " AND MsgCod IN"
			 " (SELECT MsgCod"
			    " FROM (SELECT MsgCod,"
					  "COUNT(*) AS N"
				    " FROM msg_rcv"
				   " WHERE MsgCod=%ld"
				     " AND UsrCod=%ld"
				" GROUP BY MsgCod"
				  " HAVING N=1) AS msg_rcv_unique)"
		     " AND MsgCod NOT IN"
			 " (SELECT MsgCod"
			    " FROM msg_snt)",
		   MsgCod,
                   MsgCod,UsrCod);

   /* Delete message from msg_content */
   DB_QueryDELETE ("can not remove the content of a message",
		   "DELETE FROM msg_content"
		   " WHERE MsgCod=%ld"
		     " AND MsgCod IN"
			 " (SELECT MsgCod"
			    " FROM (SELECT MsgCod,"
					  "COUNT(*) AS N"
				    " FROM msg_rcv"
				   " WHERE MsgCod=%ld"
				     " AND UsrCod=%ld"
				" GROUP BY MsgCod"
				  " HAVING N=1) AS msg_rcv_unique)"
		     " AND MsgCod NOT IN"
			 " (SELECT MsgCod"
			    " FROM msg_snt)",
		   MsgCod,
                   MsgCod,UsrCod);

   /***** Step 2: Move message from msg_rcv to msg_rcv_deleted *****/
   /* Insert message into msg_rcv_deleted */
   DB_QueryINSERT ("can not remove a received message",
		   "INSERT IGNORE INTO msg_rcv_deleted"
		   " (MsgCod,UsrCod,Notified,Open,Replied)"
		   " SELECT MsgCod,"
		           "UsrCod,"
		           "Notified,"
		           "Open,"
		           "Replied"
		     " FROM msg_rcv"
		    " WHERE MsgCod=%ld"
		      " AND UsrCod=%ld",
                   MsgCod,
                   UsrCod);

   /* Delete message from msg_rcv */
   DB_QueryDELETE ("can not remove a received message",
		   "DELETE FROM msg_rcv"
		   " WHERE MsgCod=%ld"
		     " AND UsrCod=%ld",
                   MsgCod,
                   UsrCod);
  }

/*****************************************************************************/
/******************* Remove message from sent messages ***********************/
/*****************************************************************************/

void Msg_DB_RemoveSntMsg (long MsgCod)
  {
   /***** Step 1: Move message content from msg_content to msg_content_deleted
		  if it is not in table of received *****/
   /*
                                                 msg_rcv
                        msg_content             ________
     msg_snt          ______________           |________|
    ________   There |______________|          |________|
   |________|  can   |______________|          |________|
   |________|  never |______________|    \ /   |________|
   |_UsrCod_|--be--->|____MsgCod____|<----x----|________|
   |________|  more  |______________|    / \   |________|
   |________|  than  |______________|          |________|
               one   |______________|          |________|
               here                            |________|

   */
   /* Insert message content into msg_content_deleted */
   DB_QueryINSERT ("can not remove the content of a message",
		   "INSERT IGNORE INTO msg_content_deleted"
		   " (MsgCod,Subject,Content,MedCod)"
		   " SELECT MsgCod,"
			   "Subject,"
			   "Content,"
			   "MedCod"
		    " FROM msg_content"
		   " WHERE MsgCod=%ld"
		     " AND MsgCod NOT IN"
		         " (SELECT MsgCod"
		            " FROM msg_rcv)",
                   MsgCod);

   /* Delete message from msg_content */
   DB_QueryDELETE ("can not remove the content of a message",
		   "DELETE FROM msg_content"
		   " WHERE MsgCod=%ld"
		     " AND MsgCod NOT IN"
		         " (SELECT MsgCod"
		            " FROM msg_rcv)",
                   MsgCod);

   /***** Step 2: Move message from msg_snt to msg_snt_deleted *****/
   /* Insert message into msg_snt_deleted */
   DB_QueryINSERT ("can not remove sent messages",
		   "INSERT IGNORE INTO msg_snt_deleted"
		   " (MsgCod,CrsCod,UsrCod,CreatTime)"
		   " SELECT MsgCod,"
		           "CrsCod,"
		           "UsrCod,"
		           "CreatTime"
		     " FROM msg_snt"
		    " WHERE MsgCod=%ld",
                   MsgCod);

   /* Delete message from msg_snt */
   DB_QueryDELETE ("can not remove a sent message",
		   "DELETE FROM msg_snt"
		   " WHERE MsgCod=%ld",
		   MsgCod);
  }

/*****************************************************************************/
/*************** Remove all received or sent messages of a user **************/
/*****************************************************************************/

void Msg_DB_RemoveAllRecAndSntMsgsUsr (long UsrCod)
  {
   /***** Step 1.1: Move messages contents from msg_content to msg_content_deleted
		    for messages pointed only by one unique recipient and none sender *****/
   /*
                                                 msg_rcv
                       msg_content              ________
     msg_snt          ______________   Only    |________|
    ________         |______________|  one     |________|
   |________|        |______________|  message |________|
   |________|  \ /   |______________|  received|________|
   |________|---x--->|____MsgCod____|<-points--|_UsrCod_|
   |________|  / \   |______________|  here    |________|
   |________|        |______________|          |________|
                     |______________|          |________|
                                               |________|
   */
   /* Insert messages contents into msg_content_deleted */
   DB_QueryINSERT ("can not remove the content of a message",
		   "INSERT IGNORE INTO msg_content_deleted"
		   " (MsgCod,Subject,Content,MedCod)"
		   " SELECT MsgCod,"
			   "Subject,"
			   "Content,"
			   "MedCod"
		    " FROM msg_content"
		   " WHERE MsgCod IN"
			 " (SELECT MsgCod"
			    " FROM (SELECT MsgCod,"
					  "COUNT(*) AS N"
				    " FROM msg_rcv"
				   " WHERE MsgCod IN"
					 " (SELECT MsgCod"
					    " FROM msg_rcv"
					   " WHERE UsrCod=%ld)"
				" GROUP BY MsgCod"
				  " HAVING N=1) AS msg_rcv_unique)"
		     " AND MsgCod NOT IN"
			 " (SELECT MsgCod"
			    " FROM msg_snt)",
                   UsrCod);

   /* Delete messages from msg_content */
   DB_QueryDELETE ("can not remove the content of a message",
		   "DELETE FROM msg_content"
		   " WHERE MsgCod IN"
			 " (SELECT MsgCod"
			    " FROM (SELECT MsgCod,"
					  "COUNT(*) AS N"
				    " FROM msg_rcv"
				   " WHERE MsgCod IN"
					 " (SELECT MsgCod"
					    " FROM msg_rcv"
					   " WHERE UsrCod=%ld)"
				" GROUP BY MsgCod"
				  " HAVING N=1) AS msg_rcv_unique)"
		     " AND MsgCod NOT IN"
			 " (SELECT MsgCod"
			    " FROM msg_snt)",
                   UsrCod);

   /***** Step 1.2: Move messages from msg_rcv to msg_rcv_deleted *****/
   /* Insert messages into msg_rcv_deleted */
   DB_QueryINSERT ("can not remove received messages",
		   "INSERT IGNORE INTO msg_rcv_deleted"
		   " (MsgCod,UsrCod,Notified,Open,Replied)"
		   " SELECT MsgCod,"
		           "UsrCod,"
		           "Notified,"
		           "Open,"
		           "Replied"
		     " FROM msg_rcv"
		    " WHERE UsrCod=%ld",
                   UsrCod);

   /* Delete messages from msg_rcv */
   DB_QueryDELETE ("can not remove received messages",
		   "DELETE FROM msg_rcv"
		   " WHERE UsrCod=%ld",
		   UsrCod);

   /***** Step 2.1: Move message contents from msg_rcv to msg_rcv_deleted
		    for messages pointed only by one unique sender and no recipient *****/
   /*
                                                 msg_rcv
                        msg_content             ________
     msg_snt          ______________           |________|
    ________   There |______________|          |________|
   |________|  can   |______________|          |________|
   |________|  never |______________|    \ /   |________|
   |_UsrCod_|--be--->|____MsgCod____|<----x----|________|
   |________|  more  |______________|    / \   |________|
   |________|  than  |______________|          |________|
               one   |______________|          |________|
               here                            |________|

   */
   /* Insert messages contents into msg_content_deleted */
   DB_QueryINSERT ("can not remove the content of a message",
		   "INSERT IGNORE INTO msg_content_deleted"
		   " (MsgCod,Subject,Content,MedCod)"
		   " SELECT MsgCod,"
			   "Subject,"
			   "Content,"
			   "MedCod"
		    " FROM msg_content"
		   " WHERE MsgCod IN"
			 " (SELECT MsgCod"
			    " FROM msg_snt"
			   " WHERE UsrCod=%ld)"
		     " AND MsgCod NOT IN"
		         " (SELECT MsgCod"
		            " FROM msg_rcv)",
                   UsrCod);

   /* Delete messages from msg_content */
   DB_QueryDELETE ("can not remove the content of a message",
		   "DELETE FROM msg_content"
		   " WHERE MsgCod IN"
			 " (SELECT MsgCod"
			    " FROM msg_snt"
			   " WHERE UsrCod=%ld)"
		     " AND MsgCod NOT IN"
		         " (SELECT MsgCod"
		            " FROM msg_rcv)",
                   UsrCod);

   /***** Step 2.2: Move messages from msg_snt to msg_snt_deleted *****/
   /* Insert messages into msg_snt_deleted */
   DB_QueryINSERT ("can not remove sent messages",
		   "INSERT IGNORE INTO msg_snt_deleted"
		   " (MsgCod,CrsCod,UsrCod,CreatTime)"
		   " SELECT MsgCod,"
		           "CrsCod,"
		           "UsrCod,"
		           "CreatTime"
		     " FROM msg_snt"
		    " WHERE UsrCod=%ld",
                   UsrCod);

   /* Delete messages from msg_snt */
   DB_QueryDELETE ("can not remove sent messages",
		   "DELETE FROM msg_snt"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/***** Insert pair (sender's code - my code) in table of banned senders ******/
/*****************************************************************************/

void Msg_DB_CreateUsrsPairIntoBanned (long FromUsrCod,long ToUsrCod)
  {
   DB_QueryREPLACE ("can not ban sender",
		    "REPLACE INTO msg_banned"
		    " (FromUsrCod,ToUsrCod)"
		    " VALUES"
		    " (%ld,%ld)",
                    FromUsrCod,
                    ToUsrCod);
  }

/*****************************************************************************/
/******************** Get number of users I have banned **********************/
/*****************************************************************************/

unsigned Msg_DB_GetNumUsrsBannedBy (long UsrCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users banned",
		  "SELECT COUNT(*)"
		   " FROM msg_banned"
		  " WHERE ToUsrCod=%ld",
		  UsrCod);
  }

/*****************************************************************************/
/************************* Get users I have banned ***************************/
/*****************************************************************************/

unsigned Msg_DB_GetUsrsBannedBy (MYSQL_RES **mysql_res,long UsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get banned users",
		   "SELECT msg_banned.FromUsrCod"
		    " FROM msg_banned,"
			  "usr_data"
		   " WHERE msg_banned.ToUsrCod=%ld"
		     " AND msg_banned.FromUsrCod=usr_data.UsrCod"
	        " ORDER BY usr_data.Surname1,"
			  "usr_data.Surname2,"
			  "usr_data.FirstName",
		   UsrCod);
  }

/*****************************************************************************/
/**************** Chech if a user is banned by another user ******************/
/*****************************************************************************/

bool Msg_DB_CheckIfUsrIsBanned (long FromUsrCod,long ToUsrCod)
  {
   return
   DB_QueryEXISTS ("can not check if a user is banned",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM msg_banned"
		    " WHERE FromUsrCod=%ld"
		      " AND ToUsrCod=%ld)",
		   FromUsrCod,
		   ToUsrCod) == Exi_EXISTS;
  }

/*****************************************************************************/
/**** Remove pair (sender's code - recipient's code) from table of banned ****/
/*****************************************************************************/

void Msg_DB_RemoveUsrsPairFromBanned (long FromUsrCod,long ToUsrCod)
  {
   DB_QueryDELETE ("can not unban sender",
		   "DELETE FROM msg_banned"
		   " WHERE FromUsrCod=%ld"
		     " AND ToUsrCod=%ld",
                   FromUsrCod,
                   ToUsrCod);
  }

/*****************************************************************************/
/**************** Remove user from table of banned senders *******************/
/*****************************************************************************/

void Msg_DB_RemoveUsrFromBanned (long UsrCod)
  {
   DB_QueryDELETE ("can not remove user from table of banned users",
		   "DELETE FROM msg_banned"
		   " WHERE FromUsrCod=%ld"
		      " OR ToUsrCod=%ld",
                   UsrCod,
                   UsrCod);
  }
