// swad_notification_database.c: notifications operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_notification_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************** Store a notify event to one user into database ***************/
/*****************************************************************************/

void Ntf_DB_StoreNotifyEventToUsr (Ntf_NotifyEvent_t NotifyEvent,
                                   long ToUsrCod,long Cod,Ntf_Status_t Status,
                                   long InsCod,long CtrCod,long DegCod,long CrsCod)
  {
   DB_QueryINSERT ("can not create new notification event",
		   "INSERT INTO ntf_notifications"
		   " (NotifyEvent,ToUsrCod,FromUsrCod,"
		     "InsCod,CtrCod,DegCod,CrsCod,Cod,TimeNotif,Status)"
		   " VALUES"
		   " (%u,%ld,%ld,"
		     "%ld,%ld,%ld,%ld,%ld,NOW(),%u)",
	           (unsigned) NotifyEvent,
		   ToUsrCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
	           InsCod,
	           CtrCod,
	           DegCod,
	           CrsCod,
	           Cod,
	           (unsigned) Status);
  }

/*****************************************************************************/
/***************** Update my last access to notifications ********************/
/*****************************************************************************/

void Ntf_DB_UpdateMyLastAccessToNotifications (void)
  {
   DB_QueryUPDATE ("can not update last access to notifications",
		   "UPDATE usr_last"
		     " SET LastAccNotif=NOW()"
		   " WHERE UsrCod=%ld",
                   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/******************** Mark all my notifications as seen **********************/
/*****************************************************************************/

void Ntf_DB_MarkAllMyNotifAsSeen (void)
  {
   DB_QueryUPDATE ("can not set notification(s) as seen",
		   "UPDATE ntf_notifications"
		     " SET Status=(Status | %u)"
		   " WHERE ToUsrCod=%ld",
	           (unsigned) Ntf_STATUS_BIT_READ,
	           Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************ Mark all pending notifications of a user as 'sent' *************/
/*****************************************************************************/

void Ntf_DB_MarkPendingNtfsAsSent (long ToUsrCod)
  {
   DB_QueryUPDATE ("can not set pending notifications of a user as sent",
		   "UPDATE ntf_notifications"
		     " SET Status=(Status | %u)"
		   " WHERE ToUsrCod=%ld"
		     " AND (Status & %u)<>0"
		     " AND (Status & %u)=0"
		     " AND (Status & %u)=0",
		   (unsigned) Ntf_STATUS_BIT_SENT,
		   ToUsrCod,
		   (unsigned) Ntf_STATUS_BIT_EMAIL,
		   (unsigned) Ntf_STATUS_BIT_SENT,
		   (unsigned) (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED));
  }

/*****************************************************************************/
/**************** Set one possible notification as seen by me ****************/
/*****************************************************************************/

void Ntf_DB_MarkNotifAsSeenUsingNtfCod (long NtfCod)
  {
   DB_QueryUPDATE ("can not set notification as seen",
		   "UPDATE ntf_notifications"
		     " SET Status=(Status | %u)"
		   " WHERE NtfCod=%ld"
		     " AND ToUsrCod=%ld",
		   (unsigned) Ntf_STATUS_BIT_READ,
		   NtfCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/**************** Set one possible notification as seen by me ****************/
/*****************************************************************************/

void Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_NotifyEvent_t NotifyEvent,long Cod)
  {
   /***** Trivial check: if no code specified, nothing to do *****/
   if (Cod <= 0)	// If the user code is specified
      return;

   /***** Set notification as seen by me *****/
   DB_QueryUPDATE ("can not set notification(s) as seen",
		   "UPDATE ntf_notifications"
		     " SET Status=(Status | %u)"
		   " WHERE ToUsrCod=%ld"
		     " AND NotifyEvent=%u"
		     " AND Cod=%ld",
		   (unsigned) Ntf_STATUS_BIT_READ,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) NotifyEvent,
		   Cod);
  }

/*****************************************************************************/
/** Set all notifications of this type in the current course as seen by me ***/
/*****************************************************************************/

void Ntf_DB_MarkNotifsInCrsAsSeen (Ntf_NotifyEvent_t NotifyEvent)
  {
   DB_QueryUPDATE ("can not set notification(s) as seen",
		   "UPDATE ntf_notifications"
		     " SET Status=(Status | %u)"
		   " WHERE ToUsrCod=%ld"
		     " AND NotifyEvent=%u"
		     " AND CrsCod=%ld",
		   (unsigned) Ntf_STATUS_BIT_READ,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) NotifyEvent,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/************* Set all notifications of this type as seen by me **************/
/*****************************************************************************/

void Ntf_DB_MarkNotifsAsSeen (Ntf_NotifyEvent_t NotifyEvent)
  {
   DB_QueryUPDATE ("can not set notification(s) as seen",
		   "UPDATE ntf_notifications"
		     " SET Status=(Status | %u)"
		   " WHERE ToUsrCod=%ld"
		     " AND NotifyEvent=%u",
		   (unsigned) Ntf_STATUS_BIT_READ,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) NotifyEvent);
  }

/*****************************************************************************/
/******************** Set possible notification as removed *******************/
/*****************************************************************************/

void Ntf_DB_MarkNotifToOneUsrAsRemoved (Ntf_NotifyEvent_t NotifyEvent,long Cod,long ToUsrCod)
  {
   /***** Set notification as removed *****/
   if (Cod > 0)	// Set only one notification as removed
      DB_QueryUPDATE ("can not set notification(s) as removed",
		      "UPDATE ntf_notifications"
		        " SET Status=(Status | %u)"
		      " WHERE ToUsrCod=%ld"
		        " AND NotifyEvent=%u"
		        " AND Cod=%ld",
	              (unsigned) Ntf_STATUS_BIT_REMOVED,
	              ToUsrCod,
	              (unsigned) NotifyEvent,
		      Cod);
   else		// Set all notifications of this type,
		// in the current course for the user, as removed
      DB_QueryUPDATE ("can not set notification(s) as removed",
		      "UPDATE ntf_notifications"
		        " SET Status=(Status | %u)"
		      " WHERE ToUsrCod=%ld"
		        " AND NotifyEvent=%u"
		        " AND CrsCod=%ld",
	              (unsigned) Ntf_STATUS_BIT_REMOVED,
	              ToUsrCod,
	              (unsigned) NotifyEvent,
		      Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/*********** Set possible notifications from a course as removed *************/
/*****************************************************************************/
// This function should be called when a course is removed
// because notifications from this course will not be available after course removing.
// However, notifications about new messages should not be removed
// because the messages will remain available

void Ntf_DB_MarkNotifInCrsAsRemoved (long ToUsrCod,long CrsCod)
  {
   /***** Set all notifications from the course as removed,
          except notifications about new messages *****/
   if (ToUsrCod > 0)	// If the user code is specified
      DB_QueryUPDATE ("can not set notification(s) as removed",
		      "UPDATE ntf_notifications"
		        " SET Status=(Status | %u)"
		      " WHERE ToUsrCod=%ld"
		        " AND CrsCod=%ld"
		        " AND NotifyEvent<>%u",	// messages will remain available
	              (unsigned) Ntf_STATUS_BIT_REMOVED,
	              ToUsrCod,
	              CrsCod,
	              (unsigned) Ntf_EVENT_MESSAGE);
   else			// User code not specified ==> any user
      DB_QueryUPDATE ("can not set notification(s) as removed",
		      "UPDATE ntf_notifications"
		        " SET Status=(Status | %u)"
		      " WHERE CrsCod=%ld"
		        " AND NotifyEvent<>%u",	// messages will remain available
	              (unsigned) Ntf_STATUS_BIT_REMOVED,
	              CrsCod,
	              (unsigned) Ntf_EVENT_MESSAGE);
  }

/*****************************************************************************/
/******************* Set possible notifications as removed *******************/
/*****************************************************************************/

void Ntf_DB_MarkNotifAsRemoved (Ntf_NotifyEvent_t NotifyEvent,long Cod)
  {
   DB_QueryUPDATE ("can not set notification(s) as removed",
		   "UPDATE ntf_notifications"
		     " SET Status=(Status | %u)"
		   " WHERE NotifyEvent=%u"
		     " AND Cod=%ld",
	           (unsigned) Ntf_STATUS_BIT_REMOVED,
	           (unsigned) NotifyEvent,
	           Cod);
  }

/*****************************************************************************/
/******* Set all possible notifications of files in a group as removed *******/
/*****************************************************************************/

void Ntf_DB_MarkNotifFilesInGroupAsRemoved (long GrpCod)
  {
   DB_QueryUPDATE ("can not set notification(s) as removed",
		   "UPDATE ntf_notifications"
		     " SET Status=(Status | %u)"
		   " WHERE NotifyEvent IN (%u,%u,%u,%u)"
		     " AND Cod IN"
			 " (SELECT FilCod"
			    " FROM brw_files"
			   " WHERE FileBrowser IN (%u,%u,%u,%u)"
			     " AND Cod=%ld)",
	           (unsigned) Ntf_STATUS_BIT_REMOVED,
	           (unsigned) Ntf_EVENT_DOCUMENT_FILE,
	           (unsigned) Ntf_EVENT_TEACHERS_FILE,
	           (unsigned) Ntf_EVENT_SHARED_FILE,
	           (unsigned) Ntf_EVENT_MARKS_FILE,
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           GrpCod);
  }

/*****************************************************************************/
/************ Set possible notifications of children as removed **************/
/*****************************************************************************/

void Ntf_DB_MarkNotifChildrenOfFolderAsRemoved (Ntf_NotifyEvent_t NotifyEvent,
                                                Brw_FileBrowser_t FileBrowser,
                                                long Cod,const char *Path)
  {
   DB_QueryUPDATE ("can not set notification(s) as removed",
		   "UPDATE ntf_notifications"
		     " SET Status=(Status | %u)"
		   " WHERE NotifyEvent=%u"
		     " AND Cod IN"
			 " (SELECT FilCod"
			    " FROM brw_files"
			   " WHERE FileBrowser=%u"
			     " AND Cod=%ld"
			     " AND Path LIKE '%s/%%')",
		   (unsigned) Ntf_STATUS_BIT_REMOVED,
		   (unsigned) NotifyEvent,
		   (unsigned) FileBrowser,
		   Cod,
		   Path);
  }

/*****************************************************************************/
/******************** Update number of notify emails sent ********************/
/*****************************************************************************/

void Ntf_DB_UpdateNumNotifSent (long DegCod,long CrsCod,
                                Ntf_NotifyEvent_t NotifyEvent,
                                unsigned NumEvents,unsigned NumMails)
  {
   DB_QueryREPLACE ("can not update the number of sent notifications",
		    "REPLACE INTO sta_notifications"
		    " (DegCod,CrsCod,NotifyEvent,NumEvents,NumMails)"
		    " VALUES"
		    " (%ld,%ld,%u,%u,%u)",
	            DegCod,
	            CrsCod,
	            (unsigned) NotifyEvent,
	            NumEvents,
	            NumMails);
  }
/*****************************************************************************/
/************* Get number of events notified and emails sent *****************/
/*****************************************************************************/

unsigned Ntf_DB_GetNumNotifSent (MYSQL_RES **mysql_res,
                                 long DegCod,long CrsCod,
                                 Ntf_NotifyEvent_t NotifyEvent)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get number of notifications"
			     " sent by email",
		   "SELECT NumEvents,"	// row[0]
			  "NumMails"	// row[1]
		    " FROM sta_notifications"
		   " WHERE DegCod=%ld"
		     " AND CrsCod=%ld"
		     " AND NotifyEvent=%u",
		   DegCod,
		   CrsCod,
		   (unsigned) NotifyEvent);
  }

/*****************************************************************************/
/*************************** Get my notifications ***************************/
/*****************************************************************************/

unsigned Ntf_DB_GetMyNotifications (MYSQL_RES **mysql_res,bool AllNotifications)
  {
   char SubQuery[128];

   /***** Get my notifications from database *****/
   if (AllNotifications)
      SubQuery[0] = '\0';
   else
      sprintf (SubQuery," AND (Status&%u)=0",
               Ntf_STATUS_BIT_READ |
               Ntf_STATUS_BIT_REMOVED);

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get your notifications",
		   "SELECT NotifyEvent,"		// row[0]
			  "FromUsrCod,"			// row[1]
			  "InsCod,"			// row[2]
			  "CtrCod,"			// row[3]
			  "DegCod,"			// row[4]
			  "CrsCod,"			// row[5]
			  "Cod,"			// row[6]
			  "UNIX_TIMESTAMP(TimeNotif),"	// row[7]
			  "Status"			// row[8]
		    " FROM ntf_notifications"
		   " WHERE ToUsrCod=%ld"
		       "%s"
		" ORDER BY TimeNotif DESC",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   SubQuery);
  }


/*****************************************************************************/
/************************* Get my recent notifications ***********************/
/*****************************************************************************/

unsigned Ntf_DB_GetMyRecentNotifications (MYSQL_RES **mysql_res,time_t BeginTime)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get user's notifications",
		   "SELECT NtfCod,"			// row[0]
			  "NotifyEvent,"		// row[1]
			  "UNIX_TIMESTAMP(TimeNotif),"	// row[2]
			  "FromUsrCod,"			// row[3]
			  "InsCod,"			// row[4]
			  "CtrCod,"			// row[5]
			  "DegCod,"			// row[6]
			  "CrsCod,"			// row[7]
			  "Cod,"			// row[8]
			  "Status"			// row[9]
		    " FROM ntf_notifications"
		   " WHERE ToUsrCod=%ld"
		     " AND TimeNotif>=FROM_UNIXTIME(%ld)"
		" ORDER BY TimeNotif DESC",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (long) BeginTime);
  }

/*****************************************************************************/
/******************* Get pending notifications to a user *********************/
/*****************************************************************************/

unsigned Ntf_DB_GetPendingNtfsToUsr (MYSQL_RES **mysql_res,long ToUsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get user's pending notifications",
		   "SELECT NotifyEvent,"	// row[0]
			  "FromUsrCod,"		// row[1]
			  "InsCod,"		// row[2]
			  "CtrCod,"		// row[3]
			  "DegCod,"		// row[4]
			  "CrsCod,"		// row[5]
			  "Cod"			// row[6]
		    " FROM ntf_notifications"
		   " WHERE ToUsrCod=%ld"
		     " AND (Status & %u)<>0"
		     " AND (Status & %u)=0"
		     " AND (Status & %u)=0"
		" ORDER BY TimeNotif,"
			  "NotifyEvent",
		   ToUsrCod,
		   (unsigned) Ntf_STATUS_BIT_EMAIL,
		   (unsigned) Ntf_STATUS_BIT_SENT,
		   (unsigned) (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED));
  }

/*****************************************************************************/
/************* Get the number of (all) my unseen notifications ***************/
/*****************************************************************************/

unsigned Ntf_DB_GetNumAllMyUnseenNtfs (void)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of unseen notifications",
		  "SELECT COUNT(*)"
		   " FROM ntf_notifications"
		  " WHERE ToUsrCod=%ld"
		    " AND (Status & %u)=0",
		  Gbl.Usrs.Me.UsrDat.UsrCod,
		  (unsigned) (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED));
  }

/*****************************************************************************/
/************** Get the number of my new unseen notifications ****************/
/*****************************************************************************/

unsigned Ntf_DB_GetNumMyNewUnseenNtfs (void)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of unseen notifications",
		  "SELECT COUNT(*)"
		   " FROM ntf_notifications"
		  " WHERE ToUsrCod=%ld"
		    " AND (Status & %u)=0"
		    " AND TimeNotif>FROM_UNIXTIME(%ld)",
		  Gbl.Usrs.Me.UsrDat.UsrCod,
		  (unsigned) (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED),
		  Gbl.Usrs.Me.UsrLast.LastAccNotif);
  }

/*****************************************************************************/
/********************** Get users who must be notified ***********************/
/*****************************************************************************/

unsigned Ntf_DB_GetUsrsWhoMustBeNotified (MYSQL_RES **mysql_res)
  {
   //  (Status & Ntf_STATUS_BIT_EMAIL) &&
   // !(Status & Ntf_STATUS_BIT_SENT) &&
   // !(Status & (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED))
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users who must be notified",
		   "SELECT DISTINCT "
		          "ToUsrCod"
		    " FROM ntf_notifications"
		   " WHERE TimeNotif<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)"
		     " AND (Status & %u)<>0"
		     " AND (Status & %u)=0"
		     " AND (Status & %u)=0",
		   Cfg_TIME_TO_SEND_PENDING_NOTIF,
		   (unsigned) Ntf_STATUS_BIT_EMAIL,
		   (unsigned) Ntf_STATUS_BIT_SENT,
		   (unsigned) (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED));
  }

/*****************************************************************************/
/****************** Get number of notifications by email *********************/
/*****************************************************************************/

unsigned Ntf_DB_GetNumNotifs (MYSQL_RES **mysql_res,Ntf_NotifyEvent_t NotifyEvent)
  {
   switch (Gbl.Scope.Current)
     {
      case Hie_SYS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get the number"
				    " of notifications by email",
			 "SELECT SUM(NumEvents),"			// row[0]
				"SUM(NumMails)"				// row[1]
			  " FROM sta_notifications"
			 " WHERE NotifyEvent=%u",
			 (unsigned) NotifyEvent);
      case Hie_CTY:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get the number"
				    " of notifications by email",
			 "SELECT SUM(sta_notifications.NumEvents),"	// row[0]
				"SUM(sta_notifications.NumMails)"	// row[1]
			  " FROM ins_instits,"
				"ctr_centers,"
				"deg_degrees,"
				"sta_notifications"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=sta_notifications.DegCod"
			   " AND sta_notifications.NotifyEvent=%u",
			 Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			 (unsigned) NotifyEvent);
      case Hie_INS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get the number"
				    " of notifications by email",
			 "SELECT SUM(sta_notifications.NumEvents),"	// row[0]
				"SUM(sta_notifications.NumMails)"	// row[1]
			  " FROM ctr_centers,"
				"deg_degrees,"
				"sta_notifications"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=sta_notifications.DegCod"
			   " AND sta_notifications.NotifyEvent=%u",
			 Gbl.Hierarchy.Node[Hie_INS].HieCod,
			 (unsigned) NotifyEvent);
      case Hie_CTR:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get the number"
				    " of notifications by email",
			 "SELECT SUM(sta_notifications.NumEvents),"	// row[0]
				"SUM(sta_notifications.NumMails)"	// row[1]
			  " FROM deg_degrees,"
				"sta_notifications"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=sta_notifications.DegCod"
			   " AND sta_notifications.NotifyEvent=%u",
			 Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			 (unsigned) NotifyEvent);
      case Hie_DEG:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get the number"
				    " of notifications by email",
			 "SELECT SUM(NumEvents),"			// row[0]
				"SUM(NumMails)"				// row[1]
			  " FROM sta_notifications"
			 " WHERE DegCod=%ld"
			   " AND NotifyEvent=%u",
			 Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			 (unsigned) NotifyEvent);
      case Hie_CRS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get the number"
				    " of notifications by email",
			 "SELECT SUM(NumEvents),"			// row[0]
				"SUM(NumMails)"				// row[1]
			  " FROM sta_notifications"
			 " WHERE CrsCod=%ld"
			   " AND NotifyEvent=%u",
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			 (unsigned) NotifyEvent);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/**************** Remove all notifications made to a user ********************/
/*****************************************************************************/

void Ntf_DB_RemoveUsrNtfs (long ToUsrCod)
  {
   DB_QueryDELETE ("can not remove notifications of a user",
		   "DELETE LOW_PRIORITY FROM ntf_notifications"
		   " WHERE ToUsrCod=%ld",
                   ToUsrCod);
  }


/*****************************************************************************/
/************************* Delete old notifications **************************/
/*****************************************************************************/

void Ntf_DB_RemoveOldNtfs (void)
  {
   DB_QueryDELETE ("can not remove old notifications",
		   "DELETE LOW_PRIORITY FROM ntf_notifications"
		   " WHERE TimeNotif<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Cfg_TIME_TO_DELETE_OLD_NOTIF);
  }
