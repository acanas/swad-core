// swad_room_database.c: classrooms, laboratories, offices or other rooms in a center, operations with database

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

#include "swad_database.h"
#include "swad_global.h"
#include "swad_room.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

const char *Roo_TypesDB[Roo_NUM_TYPES] =
  {
   [Roo_NO_TYPE       ] = "no_type",
   [Roo_ADMINISTRATION] = "administration",
   [Roo_AUDITORIUM    ] = "auditorium",
   [Roo_CAFETERIA     ] = "cafeteria",
   [Roo_CANTEEN       ] = "canteen",
   [Roo_CLASSROOM     ] = "classroom",
   [Roo_CONCIERGE     ] = "concierge",
   [Roo_CORRIDOR      ] = "corridor",
   [Roo_GYM           ] = "gym",
   [Roo_HALL          ] = "hall",
   [Roo_KINDERGARTEN  ] = "kindergarten",
   [Roo_LABORATORY    ] = "laboratory",
   [Roo_LIBRARY       ] = "library",
   [Roo_OFFICE        ] = "office",
   [Roo_OUTDOORS      ] = "outdoors",
   [Roo_PARKING       ] = "parking",
   [Roo_PAVILION      ] = "pavilion",
   [Roo_ROOM          ] = "room",
   [Roo_SECRETARIAT   ] = "secretariat",
   [Roo_SEMINAR       ] = "seminar",
   [Roo_SHOP          ] = "shop",
   [Roo_STORE         ] = "store",
   [Roo_TOILETS       ] = "toilets",
   [Roo_VIRTUAL       ] = "virtual",
   [Roo_YARD          ] = "yard",
  };

/*****************************************************************************/
/**************************** Create a new room ******************************/
/*****************************************************************************/

long Roo_DB_CreateRoom (long CtrCod,const struct Roo_Room *Room)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create room",
			        "INSERT INTO roo_rooms"
			        " (CtrCod,BldCod,Floor,Type,"
			          "ShortName,FullName,Capacity)"
			        " VALUES"
			        " (%ld,%ld,%d,'%s',"
			          "'%s','%s',%u)",
			        CtrCod,
			        Room->BldCod,
			        Room->Floor,
			        Roo_TypesDB[Room->Type],
			        Room->ShrtName,
			        Room->FullName,
			        Room->Capacity);
  }

/*****************************************************************************/
/**************************** Create a new room ******************************/
/*****************************************************************************/

void Roo_DB_CreateMACAddress (long RooCod,unsigned long long MACnum)
  {
   DB_QueryINSERT ("can not create MAC address",
		   "INSERT INTO roo_macs"
		   " (RooCod,MAC)"
		   " VALUES"
		   " (%ld,%llu)",
		   RooCod,
		   MACnum);
  }

/*****************************************************************************/
/**** Update the table of rooms-MACs changing the old MAC for the new one ****/
/*****************************************************************************/

void Roo_DB_UpdateMACAddress (long RooCod,unsigned long long NewMACnum)
  {
   DB_QueryREPLACE ("can not change MAC address",
		    "REPLACE INTO roo_macs"
		    " (RooCod,MAC)"
		    " VALUES"
		    " (%ld,%llu)",
		    RooCod,
		    NewMACnum);
  }

/*****************************************************************************/
/**** Update the table of rooms changing the old building for the new one ****/
/*****************************************************************************/

void Roo_DB_UpdateRoomBuilding (long RooCod,long NewBldCod)
  {
   DB_QueryUPDATE ("can not update the building of a room",
		   "UPDATE roo_rooms"
		     " SET BldCod=%ld"
		   " WHERE RooCod=%ld",
		   NewBldCod,
		   RooCod);
  }

/*****************************************************************************/
/***** Update the table of rooms changing the old floor for the new one ******/
/*****************************************************************************/

void Roo_DB_UpdateRoomFloor (long RooCod,int NewFloor)
  {
   DB_QueryUPDATE ("can not update the capacity of a room",
		   "UPDATE roo_rooms"
		     " SET Floor=%d"
		   " WHERE RooCod=%ld",
		   NewFloor,
		   RooCod);
  }

/*****************************************************************************/
/****** Update the table of rooms changing the old type for the new one ******/
/*****************************************************************************/

void Roo_DB_UpdateRoomType (long RooCod,Roo_RoomType_t NewType)
  {
   DB_QueryUPDATE ("can not update the type of a room",
		   "UPDATE roo_rooms"
		     " SET Type='%s'"
		   " WHERE RooCod=%ld",
		   Roo_TypesDB[NewType],
		   RooCod);
  }

/*****************************************************************************/
/******************** Update room name in table of rooms *********************/
/*****************************************************************************/

void Roo_DB_UpdateRoomName (long RooCod,
                            const char *FldName,const char *NewRoomName)
  {
   DB_QueryUPDATE ("can not update the name of a room",
		   "UPDATE roo_rooms"
		     " SET %s='%s'"
		   " WHERE RooCod=%ld",
		   FldName,NewRoomName,
		   RooCod);
  }

/*****************************************************************************/
/**** Update the table of rooms changing the old capacity for the new one ****/
/*****************************************************************************/

void Roo_DB_UpdateRoomCapacity (long RooCod,unsigned NewCapacity)
  {
   DB_QueryUPDATE ("can not update the capacity of a room",
		   "UPDATE roo_rooms"
		     " SET Capacity=%u"
		   " WHERE RooCod=%ld",
		   NewCapacity,
		   RooCod);
  }

/*****************************************************************************/
/****************************** List all rooms *******************************/
/*****************************************************************************/

unsigned Roo_DB_GetListRooms (MYSQL_RES **mysql_res,
                              long CtrCod,
                              Roo_WhichData_t WhichData,
                              Roo_Order_t SelectedOrder)
  {
   static const char *OrderBySubQuery[Roo_NUM_ORDERS] =
     {
      [Roo_ORDER_BY_BUILDING ] = "bld_buildings.ShortName,"
	                         "roo_rooms.Floor,"
	                         "roo_rooms.ShortName",
      [Roo_ORDER_BY_FLOOR    ] = "roo_rooms.Floor,"
	                         "bld_buildings.ShortName,"
	                         "roo_rooms.ShortName",
      [Roo_ORDER_BY_TYPE     ] = "roo_rooms.Type,"
	                         "bld_buildings.ShortName,"
	                         "roo_rooms.Floor,"
	                         "roo_rooms.ShortName",
      [Roo_ORDER_BY_SHRT_NAME] = "roo_rooms.ShortName,"
	                         "roo_rooms.FullName",
      [Roo_ORDER_BY_FULL_NAME] = "roo_rooms.FullName,"
	                         "roo_rooms.ShortName",
      [Roo_ORDER_BY_CAPACITY ] = "roo_rooms.Capacity DESC,"
	                         "bld_buildings.ShortName,"
	                         "roo_rooms.Floor,"
	                         "roo_rooms.ShortName",
     };

   /***** Get rooms from database *****/
   switch (WhichData)
     {
      case Roo_ALL_DATA:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get rooms",
		         "SELECT roo_rooms.RooCod,"		// row[0]
			        "roo_rooms.BldCod,"		// row[1]
			        "bld_buildings.ShortName,"	// row[2]
			        "roo_rooms.Floor,"		// row[3]
			        "roo_rooms.Type,"		// row[4]
			        "roo_rooms.ShortName,"		// row[5]
			        "roo_rooms.FullName,"		// row[6]
			        "roo_rooms.Capacity"		// row[7]
			  " FROM roo_rooms"
			  " LEFT JOIN bld_buildings"
			    " ON roo_rooms.BldCod=bld_buildings.BldCod"
		         " WHERE roo_rooms.CtrCod=%ld"
		      " ORDER BY %s",
		         CtrCod,
		         OrderBySubQuery[SelectedOrder]);
      case Roo_ONLY_SHRT_NAME:
      default:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get rooms",
		         "SELECT roo_rooms.RooCod,"		// row[0]
			        "roo_rooms.ShortName"		// row[1]
			  " FROM roo_rooms LEFT JOIN bld_buildings"
			    " ON roo_rooms.BldCod=bld_buildings.BldCod"
		         " WHERE roo_rooms.CtrCod=%ld"
		      " ORDER BY %s",
		         CtrCod,
		         OrderBySubQuery[Roo_ORDER_DEFAULT]);
     }
  }

/*****************************************************************************/
/********************** Check if the name of room exists *********************/
/*****************************************************************************/

bool Roo_DB_CheckIfRoomNameExists (const char *FldName,const char *Name,
				   long Cod,long PrtCod,
				   __attribute__((unused)) unsigned Year)
  {
   return
   DB_QueryEXISTS ("can not check if the name of a room already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM roo_rooms"
		    " WHERE CtrCod=%ld"
		      " AND %s='%s'"
		      " AND RooCod<>%ld)",
		   PrtCod,
		   FldName,Name,
		   Cod);
  }

/*****************************************************************************/
/************* Get room data from database using the room code ***************/
/*****************************************************************************/

unsigned Roo_DB_GetRoomDataByCod (MYSQL_RES **mysql_res,long RooCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of a room",
		   "SELECT roo_rooms.RooCod,"		// row[0]
			  "roo_rooms.BldCod,"		// row[1]
			  "bld_buildings.ShortName,"	// row[2]
			  "roo_rooms.Floor,"		// row[3]
			  "roo_rooms.Type,"		// row[4]
			  "roo_rooms.ShortName,"	// row[5]
			  "roo_rooms.FullName,"		// row[6]
			  "roo_rooms.Capacity"		// row[7]
		    " FROM roo_rooms LEFT JOIN bld_buildings"
		      " ON roo_rooms.BldCod=bld_buildings.BldCod"
		   " WHERE roo_rooms.RooCod=%ld",
		   RooCod);
  }

/*****************************************************************************/
/***************** Get the MAC addresses associated to a room ****************/
/*****************************************************************************/

unsigned Roo_DB_GetMACAddresses (MYSQL_RES **mysql_res,long RooCod)
  {
   /***** Get MAC addresses from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get MAC addresses",
		   "SELECT MAC"	// row[0]
		    " FROM roo_macs"
		   " WHERE RooCod=%ld"
		" ORDER BY MAC",
		   RooCod);
  }

/*****************************************************************************/
/********************************* Remove room *******************************/
/*****************************************************************************/

void Roo_DB_RemoveRoom (long RooCod)
  {
   DB_QueryDELETE ("can not remove a room",
		   "DELETE FROM roo_rooms"
		   " WHERE RooCod=%ld",
		   RooCod);
  }

/*****************************************************************************/
/********************************* Remove room *******************************/
/*****************************************************************************/

void Roo_DB_RemoveMACAddress (long RooCod,unsigned long long MACnum)
  {
   DB_QueryDELETE ("can not remove MAC address",
		   "DELETE FROM roo_macs"
		   " WHERE RooCod=%ld"
		     " AND MAC=%llu",
		   RooCod,
		   MACnum);
  }

/*****************************************************************************/
/********************* Update rooms assigned to a building *******************/
/*****************************************************************************/

void Roo_DB_RemoveBuildingFromRooms (long BldCod)
  {
   DB_QueryUPDATE ("can not update building of rooms",
		   "UPDATE roo_rooms"
		     " SET BldCod=0"	// 0 means another building
		   " WHERE BldCod=%ld",
		   BldCod);
  }

/*****************************************************************************/
/********************** Remove all rooms in a center *************************/
/*****************************************************************************/

void Roo_DB_RemoveAllRoomsInCtr (long CtrCod)
  {
   /***** Remove all rooms in center *****/
   DB_QueryDELETE ("can not remove rooms",
		   "DELETE FROM roo_rooms"
                   " WHERE CtrCod=%ld",
		   CtrCod);
  }


/*****************************************************************************/
/**** Check in (send user's current location by inserting pair user-room) ****/
/*****************************************************************************/

long Roo_DB_CheckIn (long RooCod)
  {
   /***** Check in (insert pair user-room) in the database *****/
   /* Get the code of the inserted item */
   return
   DB_QueryINSERTandReturnCode ("can not save current location",
				"INSERT INTO roo_check_in"
				" (UsrCod,RooCod,CheckInTime)"
				" SELECT %ld,"
				        "RooCod,"
				        "NOW()"
			 	  " FROM roo_rooms"
			 	 " WHERE RooCod=%ld",	// Check that room exists
				Gbl.Usrs.Me.UsrDat.UsrCod,
				RooCod);
  }

/*****************************************************************************/
/********************** Check if I can see user's location *******************/
/*****************************************************************************/

Usr_Can_t Roo_DB_CheckIfICanSeeUsrLocation (long UsrCod)
  {
   /*
   I can only consult the location of another user
   if the intersection of the centers of our courses is not empty.
   The other user does not have to share any course with me,
   but at least some course of each one has to share center.
   */
   return
   DB_QueryEXISTS ("can not check if you can see user location",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM (SELECT DISTINCT "
				   "deg_degrees.CtrCod"
			     " FROM crs_users,"
				   "crs_courses,"
				   "deg_degrees"
			    " WHERE crs_users.UsrCod=%ld"
			      " AND crs_users.CrsCod=crs_courses.CrsCod"
			      " AND crs_courses.DegCod=deg_degrees.DegCod) AS C1,"	// centers of my courses
			   "(SELECT DISTINCT "
				   "deg_degrees.CtrCod"
			     " FROM crs_users,"
				   "crs_courses,"
				   "deg_degrees"
			    " WHERE crs_users.UsrCod=%ld"
			      " AND crs_users.CrsCod=crs_courses.CrsCod"
			      " AND crs_courses.DegCod=deg_degrees.DegCod) AS C2"	// centers of user's courses
			    " WHERE C1.CtrCod=C2.CtrCod)",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   UsrCod) ? Usr_CAN :
			     Usr_CAN_NOT;
  }

/*****************************************************************************/
/***************** Check in (send user's current location) *******************/
/*****************************************************************************/

unsigned Roo_DB_GetUsrLastLocation (MYSQL_RES **mysql_res,long UsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get matches",
		   "SELECT ins_instits.InsCod,"				// row[ 0]
			  "ins_instits.ShortName,"			// row[ 1]
			  "ins_instits.FullName,"			// row[ 2]
			  "ctr_centers.CtrCod,"				// row[ 3]
			  "ctr_centers.ShortName,"			// row[ 4]
			  "ctr_centers.FullName,"			// row[ 5]
			  "bld_buildings.BldCod,"			// row[ 6]
			  "bld_buildings.ShortName,"			// row[ 7]
			  "bld_buildings.FullName,"			// row[ 8]
			  "roo_rooms.Floor,"				// row[ 9]
			  "roo_rooms.RooCod,"				// row[10]
			  "roo_rooms.ShortName,"			// row[11]
			  "roo_rooms.FullName,"				// row[12]
			  "UNIX_TIMESTAMP(roo_check_in.CheckInTime)"	// row[13]
		    " FROM roo_check_in,"
			  "roo_rooms,"
			  "bld_buildings,"
			  "ctr_centers,"
			  "ins_instits"
		  " WHERE roo_check_in.UsrCod=%ld"
		    " AND roo_check_in.ChkCod="
			 "(SELECT ChkCod"
			   " FROM roo_check_in"
			  " WHERE UsrCod=%ld"
		       " ORDER BY ChkCod DESC"
			  " LIMIT 1)"	// Faster than SELECT MAX
		    " AND roo_check_in.RooCod=roo_rooms.RooCod"
		    " AND roo_rooms.BldCod=bld_buildings.BldCod"
		    " AND bld_buildings.CtrCod=ctr_centers.CtrCod"
		    " AND ctr_centers.InsCod=ins_instits.InsCod",
		   UsrCod,
		   UsrCod);
  }

/*****************************************************************************/
/**************** Get location associated to a MAC address *******************/
/*****************************************************************************/

unsigned Roo_DB_GetLocationByMAC (MYSQL_RES **mysql_res,unsigned long long MACnum)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get matches",
		   "SELECT ins_instits.InsCod,"		// row[ 0]
			  "ins_instits.ShortName,"	// row[ 1]
			  "ins_instits.FullName,"	// row[ 2]
			  "ctr_centers.CtrCod,"		// row[ 3]
			  "ctr_centers.ShortName,"	// row[ 4]
			  "ctr_centers.FullName,"	// row[ 5]
			  "bld_buildings.BldCod,"	// row[ 6]
			  "bld_buildings.ShortName,"	// row[ 7]
			  "bld_buildings.FullName,"	// row[ 8]
			  "roo_rooms.Floor,"		// row[ 9]
			  "roo_rooms.RooCod,"		// row[10]
			  "roo_rooms.ShortName,"	// row[11]
			  "roo_rooms.FullName"		// row[12]
		    " FROM roo_macs,"
			  "roo_rooms,"
			  "bld_buildings,"
			  "ctr_centers,"
			  "ins_instits"
		   " WHERE roo_macs.MAC=%llu"
		     " AND roo_macs.RooCod=roo_rooms.RooCod"
		     " AND roo_rooms.BldCod=bld_buildings.BldCod"
		     " AND bld_buildings.CtrCod=ctr_centers.CtrCod"
		     " AND ctr_centers.InsCod=ins_instits.InsCod"
		" ORDER BY roo_rooms.Capacity DESC,"	// Get the biggest room
			  "roo_rooms.ShortName"
		   " LIMIT 1",
		   MACnum);
  }
