// swad_agenda_database.c: user's agenda (personal organizer) operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_agenda.h"
#include "swad_agenda_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_hierarchy_level.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Get list of agenda events *************************/
/*****************************************************************************/

#define Agd_MAX_BYTES_SUBQUERY 128

unsigned Agd_DB_GetListEvents (MYSQL_RES **mysql_res,
                               const struct Agd_Agenda *Agenda,
                               Agd_AgendaType_t AgendaType)
  {
   char *UsrSubQuery;
   char Past__FutureEventsSubQuery[Agd_MAX_BYTES_SUBQUERY + 1];
   char PrivatPublicEventsSubQuery[Agd_MAX_BYTES_SUBQUERY + 1];
   char HiddenVisiblEventsSubQuery[Agd_MAX_BYTES_SUBQUERY + 1];
   static const char *OrderBySubQuery[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = "StartTime,"
	               "EndTime,"
	               "Event,"
	               "Location",
      [Dat_END_TIME] = "EndTime,"
	               "StartTime,"
	               "Event,"
	               "Location",
     };
   unsigned NumEvents;

   /***** Get list of events from database *****/
   /* Build events subqueries */
   switch (AgendaType)
     {
      case Agd_MY_AGENDA_TODAY:
      case Agd_MY_AGENDA:
	 if (asprintf (&UsrSubQuery,"UsrCod=%ld",
		       Gbl.Usrs.Me.UsrDat.UsrCod) < 0)
	    Err_NotEnoughMemoryExit ();
	 if (AgendaType == Agd_MY_AGENDA_TODAY)
	    Str_Copy (Past__FutureEventsSubQuery,
		      " AND DATE(StartTime)<=CURDATE()"
		      " AND DATE(EndTime)>=CURDATE()",
		      sizeof (Past__FutureEventsSubQuery) - 1);	// Today events
	 else
	    switch (Agenda->Past__FutureEvents)
	      {
	       case (1 << Agd_PAST___EVENTS):
		  Str_Copy (Past__FutureEventsSubQuery,
			    " AND DATE(StartTime)<=CURDATE()",
			    sizeof (Past__FutureEventsSubQuery) - 1);	// Past and today events
		  break;
	       case (1 << Agd_FUTURE_EVENTS):
		  Str_Copy (Past__FutureEventsSubQuery,
			    " AND DATE(EndTime)>=CURDATE()",
			    sizeof (Past__FutureEventsSubQuery) - 1);	// Today and future events
		  break;
	       default:
		  Past__FutureEventsSubQuery[0] = '\0';	// All events
		  break;
	      }
	 switch (Agenda->PrivatPublicEvents)
	   {
	    case (1 << Agd_PRIVAT_EVENTS):
	       Str_Copy (PrivatPublicEventsSubQuery," AND Public='N'",
			 sizeof (PrivatPublicEventsSubQuery) - 1);	// Private events
	       break;
	    case (1 << Agd_PUBLIC_EVENTS):
	       Str_Copy (PrivatPublicEventsSubQuery," AND Public='Y'",
			 sizeof (PrivatPublicEventsSubQuery) - 1);	// Public events
	       break;
	    default:
	       PrivatPublicEventsSubQuery[0] = '\0';	// All events
	       break;
	   }
	 switch (Agenda->HiddenVisiblEvents)
	   {
	    case (1 << HidVis_HIDDEN):
	       Str_Copy (HiddenVisiblEventsSubQuery," AND Hidden='Y'",
			 sizeof (HiddenVisiblEventsSubQuery) - 1);	// Hidden events
	       break;
	    case (1 << HidVis_VISIBLE):
	       Str_Copy (HiddenVisiblEventsSubQuery," AND Hidden='N'",
			 sizeof (HiddenVisiblEventsSubQuery) - 1);	// Visible events
	       break;
	    default:
	       HiddenVisiblEventsSubQuery[0] = '\0';	// All events
	       break;
	   }
	 break;
      case Agd_ANOTHER_AGENDA_TODAY:
      case Agd_ANOTHER_AGENDA:
	 if (asprintf (&UsrSubQuery,"UsrCod=%ld",
	               Gbl.Usrs.Other.UsrDat.UsrCod) < 0)
	    Err_NotEnoughMemoryExit ();
	 if (AgendaType == Agd_ANOTHER_AGENDA_TODAY)
	    Str_Copy (Past__FutureEventsSubQuery,
		      " AND DATE(StartTime)<=CURDATE()"
		      " AND DATE(EndTime)>=CURDATE()",
		      sizeof (Past__FutureEventsSubQuery) - 1);		// Today events
	 else
	    Str_Copy (Past__FutureEventsSubQuery,
		      " AND DATE(EndTime)>=CURDATE()",
		      sizeof (Past__FutureEventsSubQuery) - 1);		// Today and future events
	 Str_Copy (PrivatPublicEventsSubQuery," AND Public='Y'",
	           sizeof (PrivatPublicEventsSubQuery) - 1);		// Public events
	 Str_Copy (HiddenVisiblEventsSubQuery," AND Hidden='N'",
	           sizeof (HiddenVisiblEventsSubQuery) - 1);		// Visible events
     }

   /* Make query */
   NumEvents = (unsigned)
   DB_QuerySELECT (mysql_res,"can not get agenda events",
		  "SELECT AgdCod"	// row[0]
		   " FROM agd_agendas"
		  " WHERE %s%s%s%s"
		  " ORDER BY %s",
		  UsrSubQuery,
		  Past__FutureEventsSubQuery,
		  PrivatPublicEventsSubQuery,
		  HiddenVisiblEventsSubQuery,
		  OrderBySubQuery[Agenda->SelectedOrder]);

   /* Free allocated memory for subquery */
   free (UsrSubQuery);

   return NumEvents;
  }

/*****************************************************************************/
/*********************** Get event data using its code ***********************/
/*****************************************************************************/

unsigned Agd_DB_GetEventDataByCod (MYSQL_RES **mysql_res,
                                   const struct Agd_Event *AgdEvent)
  {
   /***** Get data of event from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get agenda event data",
		   "SELECT AgdCod,"			// row[0]
			  "Public,"			// row[1]
			  "Hidden,"			// row[2]
			  "UNIX_TIMESTAMP(StartTime),"	// row[3]
			  "UNIX_TIMESTAMP(EndTime),"	// row[4]
			  "NOW()>EndTime,"		// row[5]	Past event?
			  "NOW()<StartTime,"		// row[6]	Future event?
			  "Event,"			// row[7]
			  "Location"			// row[8]
		    " FROM agd_agendas"
		   " WHERE AgdCod=%ld"
		     " AND UsrCod=%ld",
		   AgdEvent->AgdCod,
		   AgdEvent->UsrCod);
  }

/*****************************************************************************/
/*********************** Get event text from database ************************/
/*****************************************************************************/

void Agd_DB_GetEventTxt (const struct Agd_Event *AgdEvent,
                         char Txt[Cns_MAX_BYTES_TEXT + 1])
  {
   DB_QuerySELECTString (Txt,Cns_MAX_BYTES_TEXT,"can not get event text",
		         "SELECT Txt"
			  " FROM agd_agendas"
		         " WHERE AgdCod=%ld"
			   " AND UsrCod=%ld",
		         AgdEvent->AgdCod,
		         AgdEvent->UsrCod);
  }

/*****************************************************************************/
/************************* Remove event from database ************************/
/*****************************************************************************/

void Agd_DB_RemoveEvent (const struct Agd_Event *AgdEvent)
  {
   DB_QueryDELETE ("can not remove event",
		   "DELETE FROM agd_agendas"
		   " WHERE AgdCod=%ld"
		     " AND UsrCod=%ld",
                   AgdEvent->AgdCod,
                   AgdEvent->UsrCod);
  }

/*****************************************************************************/
/************************** Create a new event *******************************/
/*****************************************************************************/

long Agd_DB_CreateEvent (const struct Agd_Event *AgdEvent,const char *Txt)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new event",
				"INSERT INTO agd_agendas"
				" (UsrCod,StartTime,EndTime,"
				  "Event,Location,Txt)"
				" VALUES"
				" (%ld,FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
				  "'%s','%s','%s')",
				AgdEvent->UsrCod,
				AgdEvent->TimeUTC[Dat_STR_TIME],
				AgdEvent->TimeUTC[Dat_END_TIME],
				AgdEvent->Title,
				AgdEvent->Location,
				Txt);
  }

/*****************************************************************************/
/************************ Update an existing event ***************************/
/*****************************************************************************/

void Agd_DB_UpdateEvent (const struct Agd_Event *AgdEvent,const char *Txt)
  {
   /***** Update the data of the event *****/
   DB_QueryUPDATE ("can not update event",
		   "UPDATE agd_agendas"
		     " SET StartTime=FROM_UNIXTIME(%ld),"
		          "EndTime=FROM_UNIXTIME(%ld),"
		          "Event='%s',"
		          "Location='%s',"
		          "Txt='%s'"
		   " WHERE AgdCod=%ld"
		     " AND UsrCod=%ld",
                   AgdEvent->TimeUTC[Dat_STR_TIME],
                   AgdEvent->TimeUTC[Dat_END_TIME],
                   AgdEvent->Title,
                   AgdEvent->Location,
                   Txt,
                   AgdEvent->AgdCod,
                   AgdEvent->UsrCod);
  }

/*****************************************************************************/
/*************************** Hide/unhide an event ****************************/
/*****************************************************************************/

void Agd_DB_HideOrUnhideEvent (long AgdCod,long UsrCod,bool Hide)
  {
   DB_QueryUPDATE ("can not hide/unhide event",
		   "UPDATE agd_agendas"
		     " SET Hidden='%c'"
		   " WHERE AgdCod=%ld"
		     " AND UsrCod=%ld",
		   Hide ? 'Y' :
			  'N',
		   AgdCod,
		   UsrCod);
  }

/*****************************************************************************/
/***************** Set an existing event as public/private *******************/
/*****************************************************************************/

void Agd_DB_MakeEventPublic (const struct Agd_Event *AgdEvent)
  {
   DB_QueryUPDATE ("can not make event public",
		   "UPDATE agd_agendas"
		     " SET Public='Y'"
		   " WHERE AgdCod=%ld"
		     " AND UsrCod=%ld",
                   AgdEvent->AgdCod,
                   AgdEvent->UsrCod);
  }

void Agd_DB_MakeEventPrivate (const struct Agd_Event *AgdEvent)
  {
   DB_QueryUPDATE ("can not make event private",
		   "UPDATE agd_agendas"
		     " SET Public='N'"
		   " WHERE AgdCod=%ld"
		     " AND UsrCod=%ld",
                   AgdEvent->AgdCod,
                   AgdEvent->UsrCod);
  }

/*****************************************************************************/
/************************ Remove all events of a user ************************/
/*****************************************************************************/

void Agd_DB_RemoveUsrEvents (long UsrCod)
  {
   DB_QueryDELETE ("can not remove all events of a user",
		   "DELETE FROM agd_agendas"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/********************* Get number of events from a user **********************/
/*****************************************************************************/

unsigned Agd_DB_GetNumEventsFromUsr (long UsrCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of events from user",
		  "SELECT COUNT(*)"
		   " FROM agd_agendas"
		  " WHERE UsrCod=%ld",
		  UsrCod);
  }

/*****************************************************************************/
/********************** Get number of users with events **********************/
/*****************************************************************************/
// Returns the number of users with events in a given hierarchy level

unsigned Agd_DB_GetNumUsrsWithEvents (HieLvl_Level_t Level)
  {
   /***** Get number of users with events from database *****/
   switch (Level)
     {
      case HieLvl_SYS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of users with events",
                        "SELECT COUNT(DISTINCT UsrCod)"
			 " FROM agd_agendas"
		        " WHERE UsrCod>0");
      case HieLvl_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of users with events",
                        "SELECT COUNT(DISTINCT agd_agendas.UsrCod)"
			 " FROM ins_instits,"
			       "ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "crs_users,"
			       "agd_agendas"
			" WHERE ins_instits.CtyCod=%ld"
			  " AND ins_instits.InsCod=ctr_centers.InsCod"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=crs_users.CrsCod"
			  " AND crs_users.UsrCod=agd_agendas.UsrCod",
		        Gbl.Hierarchy.Cty.CtyCod);
       case HieLvl_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of users with events",
                        "SELECT COUNT(DISTINCT agd_agendas.UsrCod)"
			 " FROM ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "crs_users,"
			       "agd_agendas"
			" WHERE ctr_centers.InsCod=%ld"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=crs_users.CrsCod"
			  " AND crs_users.UsrCod=agd_agendas.UsrCod",
                        Gbl.Hierarchy.Ins.InsCod);
      case HieLvl_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of users with events",
                        "SELECT COUNT(DISTINCT agd_agendas.UsrCod)"
			"  FROM deg_degrees,"
			       "crs_courses,"
			       "crs_users,"
			       "agd_agendas"
			" WHERE deg_degrees.CtrCod=%ld"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=crs_users.CrsCod"
			  " AND crs_users.UsrCod=agd_agendas.UsrCod",
                        Gbl.Hierarchy.Ctr.CtrCod);
      case HieLvl_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of users with events",
                        "SELECT COUNT(DISTINCT agd_agendas.UsrCod)"
			 " FROM crs_courses,"
			       "crs_users,"
			       "agd_agendas"
			" WHERE crs_courses.DegCod=%ld"
			  " AND crs_courses.CrsCod=crs_users.CrsCod"
			  " AND crs_users.UsrCod=agd_agendas.UsrCod",
                        Gbl.Hierarchy.Deg.DegCod);
      case HieLvl_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of users with events",
                        "SELECT COUNT(DISTINCT agd_agendas.UsrCod)"
			 " FROM crs_users,"
			       "agd_agendas"
			" WHERE crs_users.CrsCod=%ld"
			  " AND crs_users.UsrCod=agd_agendas.UsrCod",
                        Gbl.Hierarchy.Crs.CrsCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/*************************** Get number of events ****************************/
/*****************************************************************************/
// Returns the number of events in a given hierarchy level

unsigned Agd_DB_GetNumEvents (HieLvl_Level_t Level)
  {
   /***** Get number of events from database *****/
   switch (Level)
     {
      case HieLvl_SYS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of events",
                        "SELECT COUNT(*)"
			 " FROM agd_agendas"
			" WHERE UsrCod>0");
      case HieLvl_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of events",
                        "SELECT COUNT(*)"
			 " FROM (SELECT DISTINCT "
			               "crs_users.UsrCod"
                                 " FROM ins_instits,"
			               "ctr_centers,"
			               "deg_degrees,"
			               "crs_courses,"
			               "crs_users"
			        " WHERE ins_instits.CtyCod=%ld"
			          " AND ins_instits.InsCod=ctr_centers.InsCod"
			          " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			          " AND deg_degrees.DegCod=crs_courses.DegCod"
			          " AND crs_courses.CrsCod=crs_users.CrsCod) AS users,"
			       "agd_agendas"
			" WHERE users.UsrCod=agd_agendas.UsrCod",
                        Gbl.Hierarchy.Cty.CtyCod);
      case HieLvl_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of events",
                        "SELECT COUNT(*)"
			 " FROM (SELECT DISTINCT "
			               "crs_users.UsrCod"
                                 " FROM ctr_centers,"
			               "deg_degrees,"
			               "crs_courses,"
			               "crs_users"
			        " WHERE ctr_centers.InsCod=%ld"
			          " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			          " AND deg_degrees.DegCod=crs_courses.DegCod"
			          " AND crs_courses.CrsCod=crs_users.CrsCod) AS users,"
			       "agd_agendas"
			" WHERE users.UsrCod=agd_agendas.UsrCod",
                        Gbl.Hierarchy.Ins.InsCod);
      case HieLvl_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of events",
                        "SELECT COUNT(*)"
			 " FROM (SELECT DISTINCT "
			               "crs_users.UsrCod"
                                 " FROM deg_degrees,"
			               "crs_courses,"
			               "crs_users"
			        " WHERE deg_degrees.CtrCod=%ld"
			          " AND deg_degrees.DegCod=crs_courses.DegCod"
			          " AND crs_courses.CrsCod=crs_users.CrsCod) AS users,"
			       "agd_agendas"
			" WHERE users.UsrCod=agd_agendas.UsrCod",
                        Gbl.Hierarchy.Ctr.CtrCod);
      case HieLvl_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of events",
                        "SELECT COUNT(*)"
			 " FROM (SELECT DISTINCT "
			               "crs_users.UsrCod"
			         " FROM crs_courses,"
			               "crs_users"
			        " WHERE crs_courses.DegCod=%ld"
			          " AND crs_courses.CrsCod=crs_users.CrsCod) AS users,"
			       "agd_agendas"
			" WHERE users.UsrCod=agd_agendas.UsrCod",
                        Gbl.Hierarchy.Deg.DegCod);
      case HieLvl_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of events",
                        "SELECT COUNT(*)"
			 " FROM crs_users,"
			       "agd_agendas"
			" WHERE crs_users.CrsCod=%ld"
			  " AND crs_users.UsrCod=agd_agendas.UsrCod",
                        Gbl.Hierarchy.Crs.CrsCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }
