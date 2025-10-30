// swad_attendance_database.c: control of attendance operations with database

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
/********************************** Headers **********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <mysql/mysql.h>	// To access MySQL databases
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_attendance.h"
#include "swad_attendance_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_group_database.h"
#include "swad_hierarchy_type.h"
#include "swad_user_database.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Public variables ****************************/
/*****************************************************************************/

unsigned (*Att_DB_GetListAttEvents[Grp_NUM_MY_ALL_GROUPS]) (MYSQL_RES **mysql_res,
							   Dat_StartEndTime_t SelectedOrder,
							   Att_OrderNewestOldest_t OrderNewestOldest) =
 {
  [Grp_MY_GROUPS ] = Att_DB_GetListEventsMyGrps,
  [Grp_ALL_GROUPS] = Att_DB_GetListEventsAllGrps,
 };

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

static const char *Att_DB_HiddenSubQuery[Rol_NUM_ROLES] =
  {
   [Rol_UNK    ] = " AND Hidden='N'",
   [Rol_GST    ] = " AND Hidden='N'",
   [Rol_USR    ] = " AND Hidden='N'",
   [Rol_STD    ] = " AND Hidden='N'",
   [Rol_NET    ] = " AND Hidden='N'",
   [Rol_TCH    ] = "",
   [Rol_DEG_ADM] = " AND Hidden='N'",
   [Rol_CTR_ADM] = " AND Hidden='N'",
   [Rol_INS_ADM] = " AND Hidden='N'",
   [Rol_SYS_ADM] = "",
  };

static const char *Att_DB_OrderBySubQuery[Dat_NUM_START_END_TIME][Att_NUM_ORDERS_NEWEST_OLDEST] =
  {
   [Dat_STR_TIME][Att_NEWEST_FIRST] = "StartTime DESC,"
	                              "EndTime DESC,"
	                              "Title DESC",
   [Dat_STR_TIME][Att_OLDEST_FIRST] = "StartTime,"
	                              "EndTime,"
	                              "Title",
   [Dat_END_TIME][Att_NEWEST_FIRST] = "EndTime DESC,"
	                              "StartTime DESC,"
	                              "Title DESC",
   [Dat_END_TIME][Att_OLDEST_FIRST] = "EndTime,"
	                              "StartTime,"
	                              "Title",
  };

/*****************************************************************************/
/**************** Get list of attendance events in my groups *****************/
/*****************************************************************************/

unsigned Att_DB_GetListEventsMyGrps (MYSQL_RES **mysql_res,
                                     Dat_StartEndTime_t SelectedOrder,
                                     Att_OrderNewestOldest_t OrderNewestOldest)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get attendance events",
		   "SELECT AttCod"
		    " FROM att_events"
		   " WHERE CrsCod=%ld"
		     "%s"
		     " AND (AttCod NOT IN"
			  " (SELECT AttCod"
			   " FROM att_groups)"
			  " OR"
			  " AttCod IN"
			  " (SELECT att_groups.AttCod"
			     " FROM grp_users,"
				   "att_groups"
			    " WHERE grp_users.UsrCod=%ld"
			      " AND att_groups.GrpCod=grp_users.GrpCod))"
		" ORDER BY %s",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Att_DB_HiddenSubQuery[Gbl.Usrs.Me.Role.Logged],
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Att_DB_OrderBySubQuery[SelectedOrder][OrderNewestOldest]);
  }

/*****************************************************************************/
/********************* Get list of all attendance events *********************/
/*****************************************************************************/

unsigned Att_DB_GetListEventsAllGrps (MYSQL_RES **mysql_res,
                                      Dat_StartEndTime_t SelectedOrder,
                                      Att_OrderNewestOldest_t OrderNewestOldest)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get attendance events",
		   "SELECT AttCod"
		    " FROM att_events"
		   " WHERE CrsCod=%ld%s"
		" ORDER BY %s",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Att_DB_HiddenSubQuery[Gbl.Usrs.Me.Role.Logged],
		   Att_DB_OrderBySubQuery[SelectedOrder][OrderNewestOldest]);
  }

/*****************************************************************************/
/********************* Get list of all attendance events *********************/
/*****************************************************************************/

unsigned Att_DB_GetAllEventsData (MYSQL_RES **mysql_res,long HieCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get attendance events",
		   "SELECT AttCod,"					// row[0]
			  "CrsCod,"					// row[1]
			  "Hidden,"					// row[2]
			  "UsrCod,"					// row[3]
			  "UNIX_TIMESTAMP(StartTime) AS ST,"		// row[4]
			  "UNIX_TIMESTAMP(EndTime) AS ET,"		// row[5]
			  "NOW() BETWEEN StartTime AND EndTime,"	// row[6]
			  "CommentTchVisible,"				// row[7]
			  "Title,"					// row[8]
			  "Txt"						// row[9]
		    " FROM att_events"
		   " WHERE CrsCod=%d"
		" ORDER BY ST DESC,"
			  "ET DESC,"
			  "Title DESC",
		   HieCod);
  }

/*****************************************************************************/
/**************** Get attendance event data using its code *******************/
/*****************************************************************************/

Exi_Exist_t Att_DB_GetEventDataByCod (MYSQL_RES **mysql_res,long AttCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get attendance event data",
			 "SELECT AttCod,"				// row[0]
				"CrsCod,"				// row[1]
				"Hidden,"				// row[2]
				"UsrCod,"				// row[3]
				"UNIX_TIMESTAMP(StartTime),"		// row[4]
				"UNIX_TIMESTAMP(EndTime),"		// row[5]
				"NOW() BETWEEN StartTime AND EndTime,"	// row[6]
				"CommentTchVisible,"			// row[7]
				"Title"					// row[8]
			  " FROM att_events"
			 " WHERE AttCod=%ld",
			 AttCod);
  }

/*****************************************************************************/
/***************** Get attendance event title from database ******************/
/*****************************************************************************/

void Att_DB_GetEventTitle (long AttCod,char *Title,size_t TitleSize)
  {
   DB_QuerySELECTString (Title,TitleSize,"can not get attendance event title",
		         "SELECT Title"	// row[0]
			  " FROM att_events"
		         " WHERE AttCod=%ld"
		           " AND CrsCod=%ld",	// Extra check
			 AttCod,
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/***************** Get attendance event text from database *******************/
/*****************************************************************************/

void Att_DB_GetEventDescription (long AttCod,char Description[Cns_MAX_BYTES_TEXT + 1])
  {
   DB_QuerySELECTString (Description,Cns_MAX_BYTES_TEXT,"can not get attendance event text",
		         "SELECT Txt"	// row[0]
			  " FROM att_events"
		         " WHERE AttCod=%ld"
			   " AND CrsCod=%ld",	// Extra check
		         AttCod,
		         Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/***** Check if the title or the folder of an attendance event exists ********/
/*****************************************************************************/

Exi_Exist_t Att_DB_CheckIfSimilarEventExists (const char *Field,const char *Value,long AttCod)
  {
   return
   DB_QueryEXISTS ("can not check similar attendance events",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM att_events"
		    " WHERE CrsCod=%ld"
		      " AND %s='%s'"
		      " AND AttCod<>%ld)",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Field,Value,
		   AttCod);
  }

/*****************************************************************************/
/********************* Create a new attendance event *************************/
/*****************************************************************************/

long Att_DB_CreateEvent (const struct Att_Event *Event,const char *Description)
  {
   extern const char HidVis_Hidden_YN[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char HidVis_Visible_YN[HidVis_NUM_HIDDEN_VISIBLE];

   return
   DB_QueryINSERTandReturnCode ("can not create new attendance event",
				"INSERT INTO att_events"
				" (CrsCod,Hidden,UsrCod,"
				  "StartTime,EndTime,"
				  "CommentTchVisible,Title,Txt)"
				" VALUES"
				" (%ld,'%c',%ld,"
				  "FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
				  "'%c','%s','%s')",
				Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				HidVis_Hidden_YN[Event->Hidden],
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Event->TimeUTC[Dat_STR_TIME],
				Event->TimeUTC[Dat_END_TIME],
				HidVis_Visible_YN[Event->CommentTchVisible],
				Event->Title,
				Description);
  }

/*****************************************************************************/
/****************** Update the data of an attendance event *******************/
/*****************************************************************************/

void Att_DB_UpdateEvent (const struct Att_Event *Event,const char *Description)
  {
   extern const char HidVis_Hidden_YN[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char HidVis_Visible_YN[HidVis_NUM_HIDDEN_VISIBLE];

   DB_QueryUPDATE ("can not update attendance event",
		   "UPDATE att_events"
		     " SET Hidden='%c',"
		          "StartTime=FROM_UNIXTIME(%ld),"
		          "EndTime=FROM_UNIXTIME(%ld),"
		          "CommentTchVisible='%c',"
		          "Title='%s',"
		          "Txt='%s'"
		   " WHERE AttCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
		   HidVis_Hidden_YN[Event->Hidden],
                   Event->TimeUTC[Dat_STR_TIME],
                   Event->TimeUTC[Dat_END_TIME],
		   HidVis_Visible_YN[Event->CommentTchVisible],
                   Event->Title,
                   Description,
                   Event->AttCod,
                   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/********************** Hide/unhide an attendance event **********************/
/*****************************************************************************/

void Att_DB_HideOrUnhideEvent (long AttCod,
			       HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char HidVis_Hidden_YN[HidVis_NUM_HIDDEN_VISIBLE];

   DB_QueryUPDATE ("can not hide/unhide assignment",
		   "UPDATE att_events"
		     " SET Hidden='%c'"
		   " WHERE AttCod=%ld"
		     " AND CrsCod=%ld",
		   HidVis_Hidden_YN[HiddenOrVisible],
                   AttCod,
                   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/****************** Create group of an attendance event **********************/
/*****************************************************************************/

void Att_DB_CreateGroupAssociatedToEvent (long AttCod,long GrpCod)
  {
   DB_QueryINSERT ("can not associate a group to an attendance event",
		   "INSERT INTO att_groups"
		   " (AttCod,GrpCod)"
		   " VALUES"
		   " (%ld,%ld)",
		   AttCod,
		   GrpCod);
  }

/*****************************************************************************/
/************ Get group codes associated to an attendance event **************/
/*****************************************************************************/

unsigned Att_DB_GetGrpCodsAssociatedToEvent (MYSQL_RES **mysql_res,long AttCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get groups of an attendance event",
		   "SELECT GrpCod"	// row[0]
		    " FROM att_groups"
		   " WHERE AttCod=%ld",
		   AttCod);
  }

/*****************************************************************************/
/**************** Get groups associated to an attendance event ***************/
/*****************************************************************************/

unsigned Att_DB_GetGroupsAssociatedToEvent (MYSQL_RES **mysql_res,long AttCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get groups of an attendance event",
		   "SELECT grp_types.GrpTypName,"	// row[0]
			  "grp_groups.GrpName,"		// row[1]
			  "roo_rooms.ShortName"		// row[2]
		    " FROM (att_groups,"
			   "grp_groups,"
			   "grp_types)"
		    " LEFT JOIN roo_rooms"
		      " ON grp_groups.RooCod=roo_rooms.RooCod"
		   " WHERE att_groups.AttCod=%ld"
		     " AND att_groups.GrpCod=grp_groups.GrpCod"
		     " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
		" ORDER BY grp_types.GrpTypName,"
			  "grp_groups.GrpName",
		   AttCod);
  }

/*****************************************************************************/
/*************** Remove one group from all attendance events *****************/
/*****************************************************************************/

void Att_DB_RemoveGroup (long GrpCod)
  {
   /***** Remove group from all attendance events *****/
   DB_QueryDELETE ("can not remove group from the associations"
	           " between attendance events and groups",
		   "DELETE FROM att_groups"
		   " WHERE GrpCod=%ld",
		   GrpCod);
  }

/*****************************************************************************/
/********** Remove groups of one type from all attendance events *************/
/*****************************************************************************/

void Att_DB_RemoveGroupsOfType (long GrpTypCod)
  {
   /***** Remove group from all attendance events *****/
   DB_QueryDELETE ("can not remove groups of a type from the associations"
		   " between attendance events and groups",
		   "DELETE FROM att_groups"
		   " USING grp_groups,"
		          "att_groups"
		   " WHERE grp_groups.GrpTypCod=%ld"
		     " AND grp_groups.GrpCod=att_groups.GrpCod",
                   GrpTypCod);
  }

/*****************************************************************************/
/****************** Remove groups of an attendance event *********************/
/*****************************************************************************/

void Att_DB_RemoveGrpsAssociatedToAnEvent (long AttCod)
  {
   DB_QueryDELETE ("can not remove the groups"
		   " associated to an attendance event",
		   "DELETE FROM att_groups"
		   " WHERE AttCod=%ld",
		   AttCod);
  }

/*****************************************************************************/
/******* Get number of students from a list who attended to an event *********/
/*****************************************************************************/

unsigned Att_DB_GetNumStdsTotalWhoAreInEvent (long AttCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of students registered in an event",
		  "SELECT COUNT(*)"
		   " FROM att_users"
		  " WHERE AttCod=%ld"
		    " AND Present='Y'",
		  AttCod);
  }

/*****************************************************************************/
/********** Get number of users from a list in an attendance event ***********/
/*****************************************************************************/

unsigned Att_DB_GetNumStdsFromListWhoAreInEvent (long AttCod,const char *SubQueryUsrs)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of students from a list"
		  " who are registered in an event",
		  "SELECT COUNT(*)"
		   " FROM att_users"
		  " WHERE AttCod=%ld"
		    " AND UsrCod IN (%s)"
		    " AND Present='Y'",
		  AttCod,SubQueryUsrs);
  }

/*****************************************************************************/
/******************* Check if a user attended to an event ********************/
/*****************************************************************************/
// Return if user is in table

Exi_Exist_t Att_DB_CheckIfUsrExistsInTableAttUsr (long AttCod,long UsrCod,
					          Att_AbsentOrPresent_t *Present)
  {
   char StrPresent[1 + 1];

   /***** Check if a user is registered in an event in database *****/
   DB_QuerySELECTString (StrPresent,1,"can not check if a user"
	                              " is already registered in an event",
		         "SELECT Present"
			  " FROM att_users"
		         " WHERE AttCod=%ld"
			   " AND UsrCod=%ld",
		         AttCod,
		         UsrCod);
   if (StrPresent[0])
     {
      *Present = Att_GetPresentFromYN (StrPresent[0]);
      return Exi_EXISTS;	// User is in table
     }

   *Present = Att_ABSENT;
   return Exi_DOES_NOT_EXIST;	// User is not in table
  }

/*****************************************************************************/
/********** Get if a student attended to an event and get comments ***********/
/*****************************************************************************/

Exi_Exist_t Att_DB_GetPresentAndComments (MYSQL_RES **mysql_res,
					  long AttCod,long UsrCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get if a student"
				   " is already registered in an event",
			 "SELECT Present,"	// row[0]
				"CommentStd,"	// row[1]
				"CommentTch"	// row[2]
			  " FROM att_users"
			 " WHERE AttCod=%ld"
			   " AND UsrCod=%ld",
			 AttCod,
			 UsrCod);
  }

/*****************************************************************************/
/*********** Return a list with the users in an attendance event *************/
/*****************************************************************************/

unsigned Att_DB_GetListUsrsInEvent (MYSQL_RES **mysql_res,long AttCod)
  {
   char *SubQuery;
   unsigned NumUsrs;

   /***** Query list of attendance users *****/
   switch (Grp_DB_CheckIfAssociatedToGrps ("att_groups","AttCod",AttCod))
     {
      case Exi_EXISTS:
	 // Event for one or more groups
	 // Subquery: list of users in groups of this attendance event...
	 // ...who have no entry in attendance list of users
	 if (asprintf (&SubQuery,"SELECT DISTINCT "
					"grp_users.UsrCod AS UsrCod,"	// row[0]
					"'N' AS Present"		// row[1]
				  " FROM att_groups,"
					"grp_groups,"
					"grp_types,"
					"crs_users,"
					"grp_users"
				 " WHERE att_groups.AttCod=%ld"
				   " AND att_groups.GrpCod=grp_groups.GrpCod"
				   " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
				   " AND grp_types.CrsCod=crs_users.CrsCod"
				   " AND crs_users.Role=%u"
				   " AND crs_users.UsrCod=grp_users.UsrCod"
				   " AND grp_users.GrpCod=att_groups.GrpCod"
				   " AND grp_users.UsrCod NOT IN"
				       " (SELECT UsrCod"
					  " FROM att_users"
					 " WHERE AttCod=%ld)",
			AttCod,
			(unsigned) Rol_STD,
			AttCod) < 0)
	    Err_NotEnoughMemoryExit ();
         break;
      case Exi_DOES_NOT_EXIST:
      default:
	 // Event for the whole course
	 // Subquery: list of users in the course of this attendance event...
	 // ...who have no entry in attendance list of users
	 if (asprintf (&SubQuery,"SELECT crs_users.UsrCod AS UsrCod,"	// row[0]
					"'N' AS Present"			// row[1]
				  " FROM att_events,"
					"crs_users"
				 " WHERE att_events.AttCod=%ld"
				   " AND att_events.CrsCod=crs_users.CrsCod"
				   " AND crs_users.Role=%u"
				   " AND crs_users.UsrCod NOT IN"
				       " (SELECT UsrCod"
					  " FROM att_users"
					 " WHERE AttCod=%ld)",
			AttCod,
			(unsigned) Rol_STD,
			AttCod) < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
     }

   // Query: list of users in attendance list + rest of users (subquery)
   NumUsrs = (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users in an attendance event",
		   "SELECT u.UsrCod,"	// row[0]
			  "u.Present"	// row[1]
		    " FROM (SELECT UsrCod,"
				  "Present"
			    " FROM att_users"
			   " WHERE AttCod=%ld"
			   " UNION %s) AS u,"
			  "usr_data"
		   " WHERE u.UsrCod=usr_data.UsrCod"
		" ORDER BY usr_data.Surname1,"
			  "usr_data.Surname2,"
			  "usr_data.FirstName",
		   AttCod,
		   SubQuery);

   free (SubQuery);

   return NumUsrs;
  }

/*****************************************************************************/
/********* Register a user in an attendance event changing comments **********/
/*****************************************************************************/

void Att_DB_RegUsrInEventChangingComments (long AttCod,long UsrCod,
                                           Att_AbsentOrPresent_t Present,
                                           const char *CommentStd,
                                           const char *CommentTch)
  {
   static const char YN[Att_NUM_PRESENT] =
     {
      [Att_ABSENT ] = 'N',
      [Att_PRESENT] = 'Y',
     };

   /***** Register user as assistant to an event in database *****/
   DB_QueryREPLACE ("can not register user in an event",
		    "REPLACE INTO att_users"
		    " (AttCod,UsrCod,Present,CommentStd,CommentTch)"
		    " VALUES"
		    " (%ld,%ld,'%c','%s','%s')",
                    AttCod,UsrCod,YN[Present],CommentStd,CommentTch);
  }

/*****************************************************************************/
/**************** Set user as present in an attendance event *****************/
/*****************************************************************************/

void Att_DB_SetUsrAsPresent (long AttCod,long UsrCod)
  {
   DB_QueryUPDATE ("can not set user as present in an event",
		   "UPDATE att_users"
		     " SET Present='Y'"
		   " WHERE AttCod=%ld"
		     " AND UsrCod=%ld",
		   AttCod,
		   UsrCod);
  }

/*****************************************************************************/
/**************** Set users as present in an attendance event ****************/
/*****************************************************************************/

void Att_DB_SetUsrsAsPresent (long AttCod,const char *ListUsrs,
			      Att_SetOthersAsAbsent_t SetOthersAsAbsent)
  {
   const char *Ptr;
   char LongStr[Cns_MAX_DIGITS_LONG + 1];
   struct Usr_Data UsrDat;
   unsigned NumCodsInList;
   char *SubQueryAllUsrs = NULL;
   char SubQueryOneUsr[1 + Cns_MAX_DIGITS_LONG + 1];
   Att_AbsentOrPresent_t Present;
   size_t Length = 0;	// Initialized to avoid warning
   unsigned NumUsrsPresent = 0;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Build list of present users *****/
   if (SetOthersAsAbsent == Att_SET_OTHERS_AS_ABSENT)
     {
      /***** Count number of codes in list *****/
      for (Ptr = ListUsrs, NumCodsInList = 0;
	   *Ptr;
	   NumCodsInList++)
	 /* Find next string in text until comma (leading and trailing spaces are removed) */
	 Str_GetNextStringUntilComma (&Ptr,LongStr,Cns_MAX_DIGITS_LONG);

      /***** Allocate subquery used to mark not present users as absent *****/
      Length = 256 + NumCodsInList * (1 + Cns_MAX_DIGITS_LONG + 1) - 1;
      if ((SubQueryAllUsrs = malloc (Length + 1)) == NULL)
	 Err_NotEnoughMemoryExit ();
      SubQueryAllUsrs[0] = '\0';
     }

   for (Ptr = ListUsrs;
	*Ptr;
	)
     {
      /* Find next string in text until comma
         (leading and trailing spaces are removed) */
      Str_GetNextStringUntilComma (&Ptr,LongStr,Cns_MAX_DIGITS_LONG);
      if ((UsrDat.UsrCod = Str_ConvertStrCodToLongCod (LongStr)) > 0)
	 if (Usr_DB_ChkIfUsrCodExists (UsrDat.UsrCod) == Exi_EXISTS)
	    // The user must belong to course,
	    // but it's not necessary he/she belongs to groups associated to the event
	    if (Enr_CheckIfUsrBelongsToCurrentCrs (&UsrDat) == Usr_BELONG)
	      {
	       /* Mark user as present */
	       switch (Att_DB_CheckIfUsrExistsInTableAttUsr (AttCod,UsrDat.UsrCod,&Present))
		 {
		  case Exi_EXISTS:		// User is in table att_users
		     if (Present == Att_ABSENT)	// If already present ==> nothing to do
			/***** If user is in database as absent ==>
			       set user as present in database *****/
			Att_DB_SetUsrAsPresent (AttCod,UsrDat.UsrCod);
		     break;
		  case Exi_DOES_NOT_EXIST:	// User is not in table att_users
		  default:
		     Att_DB_RegUsrInEventChangingComments (AttCod,UsrDat.UsrCod,
							   Att_PRESENT,"","");
		     break;
		 }

	       /* Add this user to query used to mark not present users as absent */
	       if (SetOthersAsAbsent == Att_SET_OTHERS_AS_ABSENT)
		 {
		  if (!NumUsrsPresent)	// Begin building subquery
		     snprintf (SubQueryAllUsrs,Length," AND UsrCod NOT IN (%ld",
			       UsrDat.UsrCod);
		  else			// Continue building subquery
		    {
		     snprintf (SubQueryOneUsr,sizeof (SubQueryOneUsr),",%ld",
		               UsrDat.UsrCod);
		     Str_Concat (SubQueryAllUsrs,SubQueryOneUsr,Length);
		    }
		 }

	       NumUsrsPresent++;
	      }
     }

   if (NumUsrsPresent)			// End building subquery
      Str_Concat (SubQueryAllUsrs,")",Length);

   /***** Mark not present users as absent in table of users *****/
   if (SetOthersAsAbsent == Att_SET_OTHERS_AS_ABSENT)
     {
      DB_QueryUPDATE ("can not set other users as absent",
      		      "UPDATE att_users"
      		        " SET Present='N'"
		      " WHERE AttCod=%ld"
		          "%s",
		      AttCod,
		      SubQueryAllUsrs);

      /* Free memory for subquery string */
      free (SubQueryAllUsrs);
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/********************** Remove a user from an event **************************/
/*****************************************************************************/

void Att_DB_RemoveUsrFromEvent (long AttCod,long UsrCod)
  {
   DB_QueryDELETE ("can not remove student from an event",
		   "DELETE FROM att_users"
		   " WHERE AttCod=%ld"
		     " AND UsrCod=%ld",
                   AttCod,UsrCod);
  }

/*****************************************************************************/
/************ Remove users absent without comments from an event *************/
/*****************************************************************************/

void Att_DB_RemoveUsrsAbsentWithoutCommentsFromEvent (long AttCod)
  {
   /***** Clean table att_users *****/
   DB_QueryDELETE ("can not remove users absent"
	           " without comments from an event",
		   "DELETE FROM att_users"
		   " WHERE AttCod=%ld"
		     " AND Present='N'"
		     " AND CommentStd=''"
		     " AND CommentTch=''",
	           AttCod);
  }

/*****************************************************************************/
/*********** Remove all users registered in an attendance event **************/
/*****************************************************************************/

void Att_DB_RemoveAllUsrsFromAnEvent (long AttCod)
  {
   DB_QueryDELETE ("can not remove attendance event",
		   "DELETE FROM att_users"
		   " WHERE AttCod=%ld",
		   AttCod);
  }

/*****************************************************************************/
/*** Remove one user from all attendance events where he/she is registered ***/
/*****************************************************************************/

void Att_DB_RemoveUsrFromAllEvents (long UsrCod)
  {
   DB_QueryDELETE ("can not remove user from all attendance events",
		   "DELETE FROM att_users"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/************* Remove one student from all attendance events *****************/
/*****************************************************************************/

void Att_DB_RemoveUsrFromCrsEvents (long UsrCod,long HieCod)
  {
   DB_QueryDELETE ("can not remove user from attendance events of a course",
		   "DELETE FROM att_users"
		   " USING att_events,"
		          "att_users"
		   " WHERE att_events.CrsCod=%ld"
		     " AND att_events.AttCod=att_users.AttCod"
		     " AND att_users.UsrCod=%ld",
                   HieCod,UsrCod);
  }

/*****************************************************************************/
/*********************** Remove an attendance event **************************/
/*****************************************************************************/

void Att_DB_RemoveEventFromCurrentCrs (long AttCod)
  {
   DB_QueryDELETE ("can not remove attendance event",
		   "DELETE FROM att_events"
		   " WHERE AttCod=%ld AND CrsCod=%ld",
                   AttCod,Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/************* Remove users in all attendance events of a course *************/
/*****************************************************************************/

void Att_DB_RemoveUsrsFromCrsEvents (long HieCod)
  {
   DB_QueryDELETE ("can not remove users registered"
		   " in attendance events of a course",
		   "DELETE FROM att_users"
		   " USING att_events,"
		          "att_users"
		   " WHERE att_events.CrsCod=%ld"
		     " AND att_events.AttCod=att_users.AttCod",
                   HieCod);
  }

/*****************************************************************************/
/************ Remove groups in all attendance events of a course *************/
/*****************************************************************************/

void Att_DB_RemoveGrpsAssociatedToCrsEvents (long HieCod)
  {
   DB_QueryDELETE ("can not remove all groups associated"
		   " to attendance events of a course",
		   "DELETE FROM att_groups"
		   " USING att_events,"
		          "att_groups"
		   " WHERE att_events.CrsCod=%ld"
		     " AND att_events.AttCod=att_groups.AttCod",
                   HieCod);
  }

/*****************************************************************************/
/***************** Remove all attendance events of a course ******************/
/*****************************************************************************/

void Att_DB_RemoveCrsEvents (long HieCod)
  {
   DB_QueryDELETE ("can not remove all attendance events of a course",
		   "DELETE FROM att_events"
		   " WHERE CrsCod=%ld",
		   HieCod);
  }

/*****************************************************************************/
/*************** Get number of attendance events in a course *****************/
/*****************************************************************************/

unsigned Att_DB_GetNumEventsInCrs (long HieCod)
  {
   /***** Get number of attendance events in a course from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of attendance events in course",
		  "SELECT COUNT(*)"
		   " FROM att_events"
		  " WHERE CrsCod=%ld",
		  HieCod);
  }

/*****************************************************************************/
/*************** Get number of courses with attendance events ****************/
/*****************************************************************************/
// Returns the number of courses with attendance events
// in this location (all the platform, the current degree or the current course)

unsigned Att_DB_GetNumCoursesWithEvents (Hie_Level_t HieLvl)
  {
   switch (HieLvl)
     {
      case Hie_SYS:
         return DB_QueryCOUNT ("can not get number of courses with attendance events",
			       "SELECT COUNT(DISTINCT CrsCod)"
				" FROM att_events"
			       " WHERE CrsCod>0");
      case Hie_INS:
         return DB_QueryCOUNT ("can not get number of courses with attendance events",
			       "SELECT COUNT(DISTINCT att_events.CrsCod)"
				" FROM ctr_centers,"
				      "deg_degrees,"
				      "crs_courses,"
				      "att_events"
			       " WHERE ctr_centers.InsCod=%ld"
				 " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=att_events.CrsCod",
			       Gbl.Hierarchy.Node[Hie_INS].HieCod);
      case Hie_CTR:
         return DB_QueryCOUNT ("can not get number of courses with attendance events",
			       "SELECT COUNT(DISTINCT att_events.CrsCod)"
				" FROM deg_degrees,"
				      "crs_courses,"
				      "att_events"
			       " WHERE deg_degrees.CtrCod=%ld"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=att_events.CrsCod",
			       Gbl.Hierarchy.Node[Hie_CTR].HieCod);
      case Hie_DEG:
         return DB_QueryCOUNT ("can not get number of courses with attendance events",
			       "SELECT COUNT(DISTINCT att_events.CrsCod)"
				" FROM crs_courses,"
				      "att_events"
			       " WHERE crs_courses.DegCod=%ld"
				 " AND crs_courses.CrsCod=att_events.CrsCod",
			       Gbl.Hierarchy.Node[Hie_DEG].HieCod);
      case Hie_CRS:
         return DB_QueryCOUNT ("can not get number of courses with attendance events",
			       "SELECT COUNT(DISTINCT CrsCod)"
				" FROM att_events"
			       " WHERE CrsCod=%ld",
			       Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/********************* Get number of attendance events ***********************/
/*****************************************************************************/

Exi_Exist_t Att_DB_GetNumEvents (MYSQL_RES **mysql_res,Hie_Level_t HieLvl)
  {
   switch (HieLvl)
     {
      case Hie_SYS:
         return
         DB_QuerySELECTunique (mysql_res,"can not get number of attendance events",
			       "SELECT COUNT(*),"			// row[0]
				      "SUM(NumNotif)"			// row[1]
				" FROM att_events"
			       " WHERE CrsCod>0");
      case Hie_INS:
         return
         DB_QuerySELECTunique (mysql_res,"can not get number of attendance events",
			       "SELECT COUNT(*),"			// row[0]
				      "SUM(att_events.NumNotif)"	// row[1]
				" FROM ctr_centers,"
				      "deg_degrees,"
				      "crs_courses,"
				      "att_events"
			       " WHERE ctr_centers.InsCod=%ld"
				 " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=att_events.CrsCod",
			       Gbl.Hierarchy.Node[Hie_INS].HieCod);
      case Hie_CTR:
         return
         DB_QuerySELECTunique (mysql_res,"can not get number of attendance events",
			       "SELECT COUNT(*),"			// row[0]
				      "SUM(att_events.NumNotif)"	// row[1]
				" FROM deg_degrees,"
				      "crs_courses,"
				      "att_events"
			       " WHERE deg_degrees.CtrCod=%ld"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=att_events.CrsCod",
			       Gbl.Hierarchy.Node[Hie_CTR].HieCod);
      case Hie_DEG:
         return
         DB_QuerySELECTunique (mysql_res,"can not get number of attendance events",
			       "SELECT COUNT(*),"			// row[0]
				      "SUM(att_events.NumNotif)"	// row[1]
				" FROM crs_courses,"
				      "att_events"
			       " WHERE crs_courses.DegCod=%ld"
				 " AND crs_courses.CrsCod=att_events.CrsCod",
			       Gbl.Hierarchy.Node[Hie_DEG].HieCod);
      case Hie_CRS:
         return
         DB_QuerySELECTunique (mysql_res,"can not get number of attendance events",
			       "SELECT COUNT(*),"			// row[0]
				      "SUM(NumNotif)"			// row[1]
				" FROM att_events"
			       " WHERE CrsCod=%ld",
			       Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return Exi_DOES_NOT_EXIST;	// Not reached
     }
  }
