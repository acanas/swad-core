// swad_group_database.c: types of groups and groups operations with database

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For exit, system, malloc, free, rand, etc.

#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_group_database.h"

/*****************************************************************************/
/************* External global variables from others modules *****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*********** Lock tables to make the registration in groups atomic ***********/
/*****************************************************************************/

void Grp_DB_LockTables (void)
  {
   DB_Query ("can not lock tables to change user's groups",
	     "LOCK TABLES "
	          "grp_types WRITE,"
	          "grp_groups WRITE,"
	          "grp_users WRITE,"
	          "crs_users READ,"
	          "crs_user_settings READ,"
	          "roo_rooms READ");
   DB_SetThereAreLockedTables ();
  }

/*****************************************************************************/
/************************** Create a new group type **************************/
/*****************************************************************************/

long Grp_DB_CreateGroupType (const struct GroupType *GrpTyp)
  {
   /***** Create a new group type *****/
   return
   DB_QueryINSERTandReturnCode ("can not create type of group",
				"INSERT INTO grp_types"
				" (CrsCod,GrpTypName,"
				  "Mandatory,Multiple,MustBeOpened,OpenTime)"
				" VALUES"
				" (%ld,'%s',"
				  "'%c','%c','%c',FROM_UNIXTIME(%ld))",
				Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				GrpTyp->Name,
				GrpTyp->Enrolment.OptionalMandatory == Grp_MANDATORY ? 'Y' :
										       'N',
				GrpTyp->Enrolment.SingleMultiple == Grp_MULTIPLE ? 'Y' :
										   'N',
				GrpTyp->MustBeOpened == Grp_MUST_BE_OPENED ? 'Y' :
						                             'N',
				(long) GrpTyp->OpenTimeUTC);
  }

/*****************************************************************************/
/***************************** Create a new group ****************************/
/*****************************************************************************/

void Grp_DB_CreateGroup (const struct GroupData *GrpDat)
  {
   /***** Create a new group *****/
   DB_QueryINSERT ("can not create group",
		   "INSERT INTO grp_groups"
		   " (GrpTypCod,GrpName,RooCod,MaxStudents,Open,FileZones)"
		   " VALUES"
		   " (%ld,'%s',%ld,%u,'N','N')",
	           GrpDat->GrpTyp.GrpTypCod,
	           GrpDat->Grp.Name,
	           GrpDat->Grp.Room.RooCod,
	           GrpDat->Grp.MaxStds);
  }

/*****************************************************************************/
/******************** Get group type title from database *********************/
/*****************************************************************************/

void Grp_DB_GetGrpTypTitle (long GrpTypCod,char *Title,size_t TitleSize)
  {
   DB_QuerySELECTString (Title,TitleSize,"can not get group type title",
		         "SELECT GrpTypName"
			  " FROM grp_types"
		         " WHERE GrpTypCod=%ld"
			   " AND CrsCod=%ld",	// Extra check
		         GrpTypCod,
		         Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/******************* Get data of a group type from its code ******************/
/*****************************************************************************/

unsigned Grp_DB_GetGroupTypeData (MYSQL_RES **mysql_res,long GrpTypCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get type of group",
		       "SELECT GrpTypName,"			// row[0]
			      "Mandatory,"			// row[1]
			      "Multiple,"			// row[2]
			      "MustBeOpened,"			// row[3]
			      "UNIX_TIMESTAMP(OpenTime)"	// row[4]
			" FROM grp_types"
		       " WHERE GrpTypCod=%ld"
		         " AND CrsCod=%ld",	// Extra check
		       GrpTypCod,
		       Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/************* Check if a group type has multiple enrolment *****************/
/*****************************************************************************/

unsigned Grp_DB_GetSingleMultiple (MYSQL_RES **mysql_res,long GrpTypCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get if type of group has multiple enrolment",
		   "SELECT Multiple"	// row[0]
		    " FROM grp_types"
		   " WHERE GrpTypCod=%ld",
		   GrpTypCod);
  }

/*****************************************************************************/
/******************** Check if a group has file zones ************************/
/*****************************************************************************/

unsigned Grp_DB_GetFileZones (MYSQL_RES **mysql_res,long GrpCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get if group has file zones",
		   "SELECT FileZones"	// row[0]
		    " FROM grp_groups"
		   " WHERE GrpCod=%ld",
		   GrpCod);
  }

/*****************************************************************************/
/********************** Get data of a group from its code ********************/
/*****************************************************************************/

unsigned Grp_DB_GetGroupDataByCod (MYSQL_RES **mysql_res,long GrpCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of a group",
		   "SELECT grp_groups.GrpTypCod,"	// row[0]
			  "grp_types.CrsCod,"		// row[1]
			  "grp_types.GrpTypName,"	// row[2]
			  "grp_types.Multiple,"		// row[3]
			  "grp_groups.GrpName,"		// row[4]
			  "grp_groups.RooCod,"		// row[5]
			  "roo_rooms.ShortName,"	// row[6]
			  "grp_groups.MaxStudents,"	// row[7]
			  "grp_groups.Open,"		// row[8]
			  "grp_groups.FileZones"	// row[9]
		    " FROM (grp_groups,"
			   "grp_types)"
		    " LEFT JOIN roo_rooms"
		      " ON grp_groups.RooCod=roo_rooms.RooCod"
		   " WHERE grp_groups.GrpCod=%ld"
		     " AND grp_groups.GrpTypCod=grp_types.GrpTypCod",
		   GrpCod);
  }

/*****************************************************************************/
/******************** Check if a group exists in database ********************/
/*****************************************************************************/

bool Grp_DB_CheckIfGrpExists (long GrpCod)
  {
   return
   DB_QueryEXISTS ("can not check if a group exists",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM grp_groups"
		    " WHERE GrpCod=%ld)",
		   GrpCod);
  }

/*****************************************************************************/
/******************* Check if a group belongs to a course ********************/
/*****************************************************************************/

bool Grp_DB_CheckIfGrpBelongsToCrs (long GrpCod,long CrsCod)
  {
   return
   DB_QueryEXISTS ("can not check if a group belongs to a course",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM grp_groups,"
			   "grp_types"
		    " WHERE grp_groups.GrpCod=%ld"
		      " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
		      " AND grp_types.CrsCod=%ld)",
		   GrpCod,CrsCod);
  }

/*****************************************************************************/
/******************* Check if name of group type exists **********************/
/*****************************************************************************/

bool Grp_DB_CheckIfGrpTypNameExistsInCurrentCrs (const char *GrpTypName,long GrpTypCod)
  {
   return
   DB_QueryEXISTS ("can not check if the name of type of group already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM grp_types"
		    " WHERE CrsCod=%ld"
		      " AND GrpTypName='%s'"
		      " AND GrpTypCod<>%ld)",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   GrpTypName,
		   GrpTypCod);
  }

/*****************************************************************************/
/************************ Check if name of group exists **********************/
/*****************************************************************************/

bool Grp_DB_CheckIfGrpNameExistsForGrpTyp (long GrpTypCod,const char *GrpName,long GrpCod)
  {
   return
   DB_QueryEXISTS ("can not check if the name of group already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM grp_groups"
		    " WHERE GrpTypCod=%ld"
		      " AND GrpName='%s'"
		      " AND GrpCod<>%ld)",
		   GrpTypCod,
		   GrpName,
		   GrpCod);
  }

/*****************************************************************************/
/********************* Count number of users in a group **********************/
/*****************************************************************************/

unsigned Grp_DB_CountNumUsrsInGrp (Rol_Role_t Role,long GrpCod)
  {
   /***** Get number of students in a group from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users in a group",
		  "SELECT COUNT(*)"
		   " FROM grp_users,"
			 "grp_groups,"
			 "grp_types,"
			 "crs_users"
		  " WHERE grp_users.GrpCod=%ld"
		    " AND grp_users.GrpCod=grp_groups.GrpCod"
		    " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
		    " AND grp_types.CrsCod=crs_users.CrsCod"
		    " AND grp_users.UsrCod=crs_users.UsrCod"
		    " AND crs_users.Role=%u",
		  GrpCod,
		  (unsigned) Role);
  }

/*****************************************************************************/
/*** Count # of users of current course not belonging to groups of a type ****/
/*****************************************************************************/

unsigned Grp_DB_CountNumUsrsInNoGrpsOfType (Rol_Role_t Role,long GrpTypCod)
  {
   /***** Get number of users not belonging to groups of a type ******/
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of users"
		  " not belonging to groups of a type",
		  "SELECT COUNT(UsrCod)"
		   " FROM crs_users"
		  " WHERE CrsCod=%ld"
		    " AND Role=%u"
		    " AND UsrCod NOT IN"
		        " (SELECT DISTINCT "
		                 "grp_users.UsrCod"
			   " FROM grp_groups,"
			         "grp_users"
			  " WHERE grp_groups.GrpTypCod=%ld"
			    " AND grp_groups.GrpCod=grp_users.GrpCod)",
		  Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		  (unsigned) Role,
		  GrpTypCod);
  }

/*****************************************************************************/
/*********** Get the user's code of a random student from a group ************/
/*****************************************************************************/
// Returns user's code or -1 if no user found

long Grp_DB_GetRamdomStdFromGrp (long GrpCod)
  {
   return DB_QuerySELECTCode ("can not get a random student from a group",
			      "SELECT grp_users.UsrCod"
			       " FROM grp_users,"
				     "crs_users"
			      " WHERE grp_users.GrpCod=%ld"
			        " AND grp_users.UsrCod=crs_users.UsrCod"
			        " AND crs_users.Role=%u"
			   " ORDER BY RAND()"
			      " LIMIT 1",
			      GrpCod,
			      (unsigned) Rol_STD);
  }

/*****************************************************************************/
/******** Get all user codes belonging to the current group, except me *******/
/*****************************************************************************/

unsigned Grp_DB_GetUsrsFromCurrentGrpExceptMe (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users from current group",
		   "SELECT UsrCod"
		    " FROM grp_users"
		   " WHERE GrpCod=%ld"
		     " AND UsrCod<>%ld",
		   Grp_GetGrpCod (),
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/****** Get all teacher codes belonging to the current group, except me ******/
/*****************************************************************************/

unsigned Grp_DB_GetTchsFromCurrentGrpExceptMe (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get teachers from current group",
		   "SELECT grp_users.UsrCod"
		    " FROM grp_users,"
			  "grp_groups,"
			  "grp_types,"
			  "crs_users"
		   " WHERE grp_users.GrpCod=%ld"
		     " AND grp_users.UsrCod<>%ld"
		     " AND grp_users.GrpCod=grp_groups.GrpCod"
		     " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
		     " AND grp_types.CrsCod=crs_users.CrsCod"
		     " AND crs_users.Role=%u",	// Teachers only
		   Grp_GetGrpCod (),
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) Rol_TCH);
  }

/*****************************************************************************/
/********* Check if I belong to any groups of a given type I belong **********/
/*****************************************************************************/

Usr_Belong_t Grp_DB_CheckIfIBelongToGrpsOfType (long GrpTypCod)
  {
   return
   DB_QueryEXISTS ("can not check if you belong to a group type",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM grp_groups,"
			   "grp_users"
		    " WHERE grp_groups.GrpTypCod=%ld"
		      " AND grp_groups.GrpCod=grp_users.GrpCod"
		      " AND grp_users.UsrCod=%ld)",	// I belong
		   GrpTypCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod) ? Usr_BELONG :
						Usr_DONT_BELONG;
  }

/*****************************************************************************/
/*********************** Check if I belong to a group ************************/
/*****************************************************************************/

Usr_Belong_t Grp_DB_CheckIfIBelongToGrp (long GrpCod)
  {
   return
   DB_QueryEXISTS ("can not check if you belong to a group",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM grp_users"
		    " WHERE GrpCod=%ld"
		      " AND UsrCod=%ld)",	// I belong
		   GrpCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod) ? Usr_BELONG :
						Usr_DONT_BELONG;
  }

/*****************************************************************************/
/***** Check if a user belongs to any of my groups in the current course *****/
/*****************************************************************************/

bool Grp_DB_CheckIfUsrSharesAnyOfMyGrpsInCurrentCrs (long UsrCod)
  {
   return
   DB_QueryEXISTS ("can not check if a user shares any group in the current course with you",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM grp_users"
		    " WHERE UsrCod=%ld"
		      " AND GrpCod IN"
			  " (SELECT grp_users.GrpCod"
			     " FROM grp_users,"
				   "grp_groups,"
				   "grp_types"
			    " WHERE grp_users.UsrCod=%ld"
			      " AND grp_users.GrpCod=grp_groups.GrpCod"
			      " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
			      " AND grp_types.CrsCod=%ld))",
		   UsrCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/***************** Get group types with groups in a course *******************/
/*****************************************************************************/

unsigned Grp_DB_GetGrpTypesWithGrpsInCrs (MYSQL_RES **mysql_res,long CrsCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get types of group of a course",
		   "SELECT grp_types.GrpTypCod,"			// row[0]
			  "grp_types.GrpTypName,"			// row[1]
			  "grp_types.Mandatory,"			// row[2]
			  "grp_types.Multiple,"				// row[3]
			  "grp_types.MustBeOpened,"			// row[4]
			  "UNIX_TIMESTAMP(grp_types.OpenTime),"		// row[5]
			  "COUNT(grp_groups.GrpCod)"			// row[6]
		    " FROM grp_types,"
			  "grp_groups"
		   " WHERE grp_types.CrsCod=%ld"
		     " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
		" GROUP BY grp_types.GrpTypCod"
		" ORDER BY grp_types.GrpTypName",
		   CrsCod);
  }

/*****************************************************************************/
/************ Get group types with or without groups in a course *************/
/*****************************************************************************/

unsigned Grp_DB_GetAllGrpTypesInCrs (MYSQL_RES **mysql_res,long CrsCod)
  {
   // The tables in the second part of the UNION requires ALIAS in order to LOCK TABLES when registering in groups
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get types of group of a course",
		   "(SELECT grp_types.GrpTypCod,"			// row[0]
			   "grp_types.GrpTypName AS GrpTypName,"	// row[1]
			   "grp_types.Mandatory,"			// row[2]
			   "grp_types.Multiple,"			// row[3]
			   "grp_types.MustBeOpened,"			// row[4]
			   "UNIX_TIMESTAMP(grp_types.OpenTime),"	// row[5]
			   "COUNT(grp_groups.GrpCod)"			// row[6]
		     " FROM grp_types,"
			   "grp_groups"
		    " WHERE grp_types.CrsCod=%ld"
		      " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
		 " GROUP BY grp_types.GrpTypCod)"
		   " UNION "
		   "(SELECT GrpTypCod,"					// row[0]
			   "GrpTypName,"				// row[1]
			   "Mandatory,"					// row[2]
			   "Multiple,"					// row[3]
			   "MustBeOpened,"				// row[4]
			   "UNIX_TIMESTAMP(OpenTime),"			// row[5]
			   "0"
		     " FROM grp_types"
		    " WHERE CrsCod=%ld"
		      " AND GrpTypCod NOT IN"
			  " (SELECT GrpTypCod"
			     " FROM grp_groups))"
		 " ORDER BY GrpTypName",
		   CrsCod,
		   CrsCod);
  }

/*****************************************************************************/
/*********** Get group types in current course that must be opened ***********/
/*****************************************************************************/

unsigned Grp_DB_GetGrpTypesInCurrentCrsToBeOpened (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get the types of group to be opened",
		   "SELECT GrpTypCod"
		    " FROM grp_types"
		   " WHERE CrsCod=%ld"
		     " AND MustBeOpened='Y'"
		     " AND OpenTime<=NOW()",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/*********** Query the number of groups that hay in this course **************/
/*****************************************************************************/

unsigned Grp_DB_CountNumGrpsInCurrentCrs (void)
  {
   /***** Get number of group in current course from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of groups in this course",
		  "SELECT COUNT(*)"
		   " FROM grp_types,"
			 "grp_groups"
		  " WHERE grp_types.CrsCod=%ld"
		    " AND grp_types.GrpTypCod=grp_groups.GrpTypCod",
		  Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/******************** Get groups in a course ********************/
/*****************************************************************************/

unsigned Grp_DB_GetGrpsInCrs (MYSQL_RES **mysql_res,long CrsCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get user's groups",
		   "SELECT grp_types.GrpTypCod,"	// row[0]
			  "grp_types.GrpTypName,"	// row[1]
			  "grp_groups.GrpCod,"		// row[2]
			  "grp_groups.GrpName,"		// row[3]
			  "grp_groups.MaxStudents,"	// row[4]
			  "grp_groups.Open,	"	// row[5]
			  "grp_groups.FileZones"	// row[6]
		    " FROM grp_types,"
			  "grp_groups"
		   " WHERE grp_types.CrsCod=%d"
		     " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
		" ORDER BY grp_types.GrpTypName,"
			  "grp_groups.GrpName",
		   CrsCod);
  }

/*****************************************************************************/
/******************** Get groups of a type in this course ********************/
/*****************************************************************************/

unsigned Grp_DB_GetGrpsOfType (MYSQL_RES **mysql_res,long GrpTypCod)
  {
   // Don't use INNER JOIN because there are groups without assigned room
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get groups of a type",
		   "SELECT grp_groups.GrpCod,"		// row[0]
			  "grp_groups.GrpName,"		// row[1]
			  "grp_groups.RooCod,"		// row[2]
			  "roo_rooms.ShortName,"	// row[3]
			  "grp_groups.MaxStudents,"	// row[4]
			  "grp_groups.Open,"		// row[5]
			  "grp_groups.FileZones"	// row[6]
		    " FROM grp_groups"
		    " LEFT JOIN roo_rooms"
		      " ON grp_groups.RooCod=roo_rooms.RooCod"
		   " WHERE grp_groups.GrpTypCod=%ld"
		" ORDER BY grp_groups.GrpName",
		   GrpTypCod);
  }

/*****************************************************************************/
/*********** Get the list of group codes of any type            **************/
/*********** to which a user belongs to (in the current course) **************/
/*****************************************************************************/

unsigned Grp_DB_GetLstCodGrpsOfAnyTypeInCurrentCrsUsrBelongs (MYSQL_RES **mysql_res,
							      long UsrCod,
							      Grp_ClosedOpenGrps_t ClosedOpenGroups)
  {
   static const char *ClosedOpenGroupsTxt[Grp_NUM_CLOSED_OPEN_GROUPS] =
     {
      [Grp_ONLY_CLOSED_GROUPS    ] = " AND grp_groups.Open='N'",
      [Grp_CLOSED_AND_OPEN_GROUPS] = "",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get the groups which a user belongs to",
		   "SELECT grp_groups.GrpCod"	// row[0]
		    " FROM grp_types,"
			  "grp_groups,"
			  "grp_users"
		   " WHERE grp_types.CrsCod=%ld"
		     " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
		     "%s"
		     " AND grp_groups.GrpCod=grp_users.GrpCod"
		     " AND grp_users.UsrCod=%ld"
		" ORDER BY grp_types.GrpTypName,"
			  "grp_groups.GrpName",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   ClosedOpenGroupsTxt[ClosedOpenGroups],
		   UsrCod);
  }

/*****************************************************************************/
/************ Get the list of group codes of a type              *************/
/************ to which a user belongs to (in the current course) *************/
/*****************************************************************************/

unsigned Grp_DB_GetLstCodGrpsOfATypeInCurrentCrsUsrBelongs (MYSQL_RES **mysql_res,long UsrCod,long GrpTypCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get the groups which a user belongs to",
		   "SELECT grp_groups.GrpCod"	// row[0]
		    " FROM grp_types,"
			  "grp_groups,"
			  "grp_users"
		   " WHERE grp_types.CrsCod=%ld"
		     " AND grp_types.GrpTypCod=%ld"
		     " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
		     " AND grp_groups.GrpCod=grp_users.GrpCod"
		     " AND grp_users.UsrCod=%ld"
	        " ORDER BY grp_groups.GrpName",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   GrpTypCod,
		   UsrCod);
  }

/*****************************************************************************/
/**************** Get groups with file zones which I belong ******************/
/*****************************************************************************/

unsigned Grp_DB_GetLstCodGrpsWithFileZonesInCurrentCrsIBelong (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get the groups which you belong to",
		   "SELECT grp_groups.GrpCod"	// row[0]
		    " FROM grp_types,"
			  "grp_groups,"
			  "grp_users"
		   " WHERE grp_types.CrsCod=%ld"
		     " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
		     " AND grp_groups.FileZones='Y'"
		     " AND grp_groups.GrpCod=grp_users.GrpCod"
		     " AND grp_users.UsrCod=%ld"
		" ORDER BY grp_types.GrpTypName,"
			  "grp_groups.GrpName",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********** Query names of groups of a type which user belongs to ************/
/*****************************************************************************/

unsigned Grp_DB_GetNamesGrpsUsrBelongsTo (MYSQL_RES **mysql_res,
                                          long UsrCod,long GrpTypCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get names of groups a user belongs to",
		   "SELECT grp_groups.GrpName"	// row[0]
		    " FROM grp_groups,"
			  "grp_users"
		   " WHERE grp_groups.GrpTypCod=%ld"
		     " AND grp_groups.GrpCod=grp_users.GrpCod"
		     " AND grp_users.UsrCod=%ld"
		" ORDER BY grp_groups.GrpName",
		   GrpTypCod,
		   UsrCod);
  }

/*****************************************************************************/
/********************** Get the type of group of a group *********************/
/*****************************************************************************/

long Grp_DB_GetGrpTypeFromGrp (long GrpCod)
  {
   long GrpTypCod;

   /***** Get group type of a group from database *****/
   GrpTypCod = DB_QuerySELECTCode ("can not get the type of a group",
				   "SELECT GrpTypCod"
				    " FROM grp_groups"
				   " WHERE GrpCod=%ld",
				   GrpCod);
   if (GrpTypCod <= 0)
      Err_WrongGrpTypExit ();

   return GrpTypCod;
  }

/*****************************************************************************/
/*********** Check if an assignment, attendance event, survey,    ************/
/*********** exam session or match is associated to a given group ************/
/*****************************************************************************/

bool Grp_DB_CheckIfAssociatedToGrp (const char *Table,const char *Field,
                                    long Cod,long GrpCod)
  {
   /***** Trivial check: Cod <= 0 means
 			 new item, assignment, event, survey, exam event or match *****/
   if (Cod <= 0)
      return false;

   return
   DB_QueryEXISTS ("can not check if associated to a group",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM %s"
		    " WHERE %s=%ld"
		      " AND GrpCod=%ld)",
		   Table,
		   Field,Cod,
		   GrpCod);
  }

/*****************************************************************************/
/************ Check if an assignment, attendance event, survey, **************/
/************ exam session or match is associated to any group  **************/
/*****************************************************************************/

bool Grp_DB_CheckIfAssociatedToGrps (const char *Table,const char *Field,long Cod)
  {
   /***** Trivial check *****/
   if (Cod <= 0)	// Assignment, attendance event, survey, exam event or match code
      return false;

   /***** Check if an assignment, attendance event, survey,
          exam session or match is associated to any group *****/
   return
   DB_QueryEXISTS ("can not check if associated to groups",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM %s"
		    " WHERE %s=%ld)",
		   Table,
		   Field,Cod);
  }

/*****************************************************************************/
/************ Change the mandatory enrolment of a type of group **************/
/*****************************************************************************/

void Grp_DB_ChangeOptionalMandatory (long GrpTypCod,
				     Grp_OptionalMandatory_t NewOptionalMandatory)
  {
   DB_QueryUPDATE ("can not update enrolment type of a type of group",
		   "UPDATE grp_types"
		     " SET Mandatory='%c'"
		   " WHERE GrpTypCod=%ld",
		   (NewOptionalMandatory == Grp_MANDATORY) ? 'Y' :
							     'N',
		   GrpTypCod);
  }

/*****************************************************************************/
/************* Change the multiple enrolment of a type of group **************/
/*****************************************************************************/

void Grp_DB_ChangeSingleMultiple (long GrpTypCod,
                                  Grp_SingleMultiple_t NewSingleMultiple)
  {
   DB_QueryUPDATE ("can not update enrolment type of a type of group",
		   "UPDATE grp_types"
		     " SET Multiple='%c'"
		   " WHERE GrpTypCod=%ld",
		   (NewSingleMultiple == Grp_MULTIPLE) ? 'Y' :
							 'N',
		   GrpTypCod);
  }

/*****************************************************************************/
/*************** Change the opening time of a type of group ******************/
/*****************************************************************************/

void Grp_DB_ChangeOpeningTime (long GrpTypCod,
                               Grp_MustBeOpened_t MustBeOpened,time_t OpenTimeUTC)
  {
   DB_QueryUPDATE ("can not update enrolment type of a type of group",
		   "UPDATE grp_types"
		     " SET MustBeOpened='%c',"
		          "OpenTime=FROM_UNIXTIME(%ld)"
		   " WHERE GrpTypCod=%ld",
                   MustBeOpened == Grp_MUST_BE_OPENED ? 'Y' :
                				        'N',
                   (long) OpenTimeUTC,
                   GrpTypCod);
  }

/*****************************************************************************/
/******************** Set type of group to not be opened *********************/
/*****************************************************************************/

void Grp_DB_ClearMustBeOpened (long GrpTypCod)
  {
   DB_QueryUPDATE ("can not update the opening of a type of group",
		   "UPDATE grp_types"
		     " SET MustBeOpened='N'"
		   " WHERE GrpTypCod=%ld",
		   GrpTypCod);
  }

/*****************************************************************************/
/********************** Open all closed groups of a type *********************/
/*****************************************************************************/

void Grp_DB_OpenGrpsOfType (long GrpTypCod)
  {
   DB_QueryUPDATE ("can not open groups of a type",
		   "UPDATE grp_groups"
		     " SET Open='Y'"
		   " WHERE GrpTypCod=%ld"
		     " AND Open='N'",
		   GrpTypCod);
  }

/*****************************************************************************/
/**************************** Open a closed group ****************************/
/*****************************************************************************/

void Grp_DB_OpenGrp (long GrpCod)
  {
   DB_QueryUPDATE ("can not open a group",
		   "UPDATE grp_groups"
		     " SET Open='Y'"
		   " WHERE GrpCod=%ld"
		     " AND Open='N'",
                   GrpCod);
  }

/*****************************************************************************/
/**************************** Close an open group ****************************/
/*****************************************************************************/

void Grp_DB_CloseGrp (long GrpCod)
  {
   DB_QueryUPDATE ("can not close a group",
		   "UPDATE grp_groups"
		     " SET Open='N'"
		   " WHERE GrpCod=%ld"
		     " AND Open='Y'",
                   GrpCod);
  }

/*****************************************************************************/
/************************ Enable file zones of a group ***********************/
/*****************************************************************************/

void Grp_DB_EnableFileZonesGrp (long GrpCod)
  {
   DB_QueryUPDATE ("can not enable file zones of a group",
		   "UPDATE grp_groups"
		     " SET FileZones='Y'"
		   " WHERE GrpCod=%ld"
		     " AND FileZones='N'",
                   GrpCod);
  }

/*****************************************************************************/
/************************ Disable file zones of a group **********************/
/*****************************************************************************/

void Grp_DB_DisableFileZonesGrp (long GrpCod)
  {
   DB_QueryUPDATE ("can not disable file zones of a group",
		   "UPDATE grp_groups"
		     " SET FileZones='N'"
		   " WHERE GrpCod=%ld"
		     " AND FileZones='Y'",
                   GrpCod);
  }

/*****************************************************************************/
/************************ Change group type of a group ***********************/
/*****************************************************************************/

void Grp_DB_ChangeGrpTypOfGrp (long GrpCod,long NewGrpTypCod)
  {
   DB_QueryUPDATE ("can not update the type of a group",
		   "UPDATE grp_groups"
		     " SET GrpTypCod=%ld"
		   " WHERE GrpCod=%ld",
		   NewGrpTypCod,
		   GrpCod);
  }

/*****************************************************************************/
/*************************** Change room of a group **************************/
/*****************************************************************************/

void Grp_DB_ChangeRoomOfGrp (long GrpCod,long NewRooCod)
  {
   DB_QueryUPDATE ("can not update the room of a group",
		   "UPDATE grp_groups"
		     " SET RooCod=%ld"
		   " WHERE GrpCod=%ld",
		   NewRooCod,
		   GrpCod);
  }

/*****************************************************************************/
/********************* Update groups assigned to a room **********************/
/*****************************************************************************/

void Grp_DB_ResetRoomInGrps (long RooCod)
  {
   DB_QueryUPDATE ("can not update room in groups",
		   "UPDATE grp_groups"
		     " SET RooCod=0"	// 0 means another room
		   " WHERE RooCod=%ld",
		   RooCod);
  }

/*****************************************************************************/
/******************* Change maximum of students in a group *******************/
/*****************************************************************************/

void Grp_DB_ChangeMaxStdsOfGrp (long GrpCod,unsigned NewMaxStds)
  {
   DB_QueryUPDATE ("can not update the maximum number of students"
		   " in a group",
		   "UPDATE grp_groups"
		     " SET MaxStudents=%u"
		   " WHERE GrpCod=%ld",
		   NewMaxStds,
		   GrpCod);
  }

/*****************************************************************************/
/**** Get if any group in group-type/this-course is open and has vacants *****/
/*****************************************************************************/
// If GrpTypCod >  0 ==> restrict to the given group type, mandatory or not
// If GrpTypCod <= 0 ==> all mandatory group types in the current course

bool Grp_DB_CheckIfAvailableGrpTyp (long GrpTypCod)
  {
   unsigned NumGrpTypes;
   char *SubQueryGrpTypes;

   if (GrpTypCod > 0)	// restrict to the given group type, mandatory or not
     {
      if (asprintf (&SubQueryGrpTypes,"grp_types.GrpTypCod=%ld",
	            GrpTypCod) < 0)
	 Err_NotEnoughMemoryExit ();
     }
   else			// all mandatory group types in the current course
     {
      if (asprintf (&SubQueryGrpTypes,"grp_types.CrsCod=%ld"
	                              " AND grp_types.Mandatory='Y'",
	            Gbl.Hierarchy.Node[Hie_CRS].HieCod) < 0)
	 Err_NotEnoughMemoryExit ();
     }

   /***** Get the number of types of group in this course
          with one or more open groups with vacants, from database *****/
   NumGrpTypes = (unsigned)
   DB_QueryCOUNT ("can not check if there has available mandatory group types",
		  "SELECT COUNT(GrpTypCod)"
		   " FROM ("
			   // Available mandatory groups with students
			   "SELECT GrpTypCod"
			    " FROM (SELECT grp_types.GrpTypCod AS GrpTypCod,"
					  "COUNT(*) AS NumStudents,"
					  "grp_groups.MaxStudents as MaxStudents"
				    " FROM grp_types,"
					  "grp_groups,"
					  "grp_users,"
					  "crs_users"
				   " WHERE %s"					// Which group types?
				     " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
				     " AND grp_groups.Open='Y'"			// Open
				     " AND grp_groups.MaxStudents>0"		// Admits students
				     " AND grp_types.CrsCod=crs_users.CrsCod"
				     " AND grp_groups.GrpCod=grp_users.GrpCod"
				     " AND grp_users.UsrCod=crs_users.UsrCod"
				     " AND crs_users.Role=%u"			// Student
				" GROUP BY grp_groups.GrpCod"
				  " HAVING NumStudents<MaxStudents"		// Not full
				  ") AS available_grp_types_with_stds"

			   " UNION "

			   // Available mandatory groups...
			   "SELECT grp_types.GrpTypCod AS GrpTypCod"
			    " FROM grp_types,"
				  "grp_groups"
			   " WHERE %s"						// Which group types?
			     " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
			     " AND grp_groups.Open='Y'"				// Open
			     " AND grp_groups.MaxStudents>0"			// Admits students
			   // ...without students
			     " AND grp_groups.GrpCod NOT IN"
				 " (SELECT grp_users.GrpCod"
				    " FROM crs_users,"
					  "grp_users"
				   " WHERE crs_users.CrsCod=%ld"
				     " AND crs_users.Role=%u"			// Student
				     " AND crs_users.UsrCod=grp_users.UsrCod)"

			   ") AS available_grp_types"

		  // ...to which I don't belong
		  " WHERE GrpTypCod NOT IN"
		        " (SELECT grp_types.GrpTypCod"
			   " FROM grp_types,"
				 "grp_groups,"
				 "grp_users"
			  " WHERE %s"						// Which group types?
			    " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
			    " AND grp_groups.Open='Y'"				// Open
			    " AND grp_groups.MaxStudents>0"			// Admits students
			    " AND grp_groups.GrpCod=grp_users.GrpCod"
			    " AND grp_users.UsrCod=%ld)",			// I belong

		  SubQueryGrpTypes,(unsigned) Rol_STD,
		  SubQueryGrpTypes,
		  Gbl.Hierarchy.Node[Hie_CRS].HieCod,(unsigned) Rol_STD,
		  SubQueryGrpTypes,Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Free allocated memory for subquery *****/
   free (SubQueryGrpTypes);

   return (NumGrpTypes != 0);
  }

/*****************************************************************************/
/*************************** Rename a group type *****************************/
/*****************************************************************************/

void Grp_DB_RenameGrpTyp (long GrpTypCod,
                          const char NewNameGrpTyp[Grp_MAX_BYTES_GROUP_TYPE_NAME + 1])
  {
   DB_QueryUPDATE ("can not update the type of a group",
		   "UPDATE grp_types"
		     " SET GrpTypName='%s'"
		   " WHERE GrpTypCod=%ld",
		   NewNameGrpTyp,
		   GrpTypCod);
  }

/*****************************************************************************/
/****************************** Rename a group *******************************/
/*****************************************************************************/

void Grp_DB_RenameGrp (long GrpCod,
                       const char NewNameGrp[Grp_MAX_BYTES_GROUP_NAME + 1])
  {
   DB_QueryUPDATE ("can not update the name of a group",
		   "UPDATE grp_groups"
		     " SET GrpName='%s'"
		   " WHERE GrpCod=%ld",
		   NewNameGrp,
		   GrpCod);
  }

/*****************************************************************************/
/*********************** Register a user in a group **************************/
/*****************************************************************************/

void Grp_DB_AddUsrToGrp (long UsrCod,long GrpCod)
  {
   DB_QueryINSERT ("can not add a user to a group",
		   "INSERT INTO grp_users"
		   " (GrpCod,UsrCod)"
		   " VALUES"
		   " (%ld,%ld)",
                   GrpCod,
                   UsrCod);
  }

/*****************************************************************************/
/************************* Remove a user from a group ************************/
/*****************************************************************************/

void Grp_DB_RemoveUsrFromGrp (long UsrCod,long GrpCod)
  {
   DB_QueryDELETE ("can not remove a user from a group",
		   "DELETE FROM grp_users"
		   " WHERE GrpCod=%ld"
		     " AND UsrCod=%ld",
                   GrpCod,
                   UsrCod);
  }

/*****************************************************************************/
/***************** Remove a user of all groups of a course *******************/
/*****************************************************************************/

void Grp_DB_RemUsrFromAllGrpsInCrs (long UsrCod,long CrsCod)
  {
   DB_QueryDELETE ("can not remove a user from all groups of a course",
		   "DELETE FROM grp_users"
		   " WHERE UsrCod=%ld"
		     " AND GrpCod IN"
		         " (SELECT grp_groups.GrpCod"
		            " FROM grp_types,"
		                  "grp_groups"
		           " WHERE grp_types.CrsCod=%ld"
		             " AND grp_types.GrpTypCod=grp_groups.GrpTypCod)",
                   UsrCod,
                   CrsCod);
  }

/*****************************************************************************/
/*********************** Remove a user from all groups ***********************/
/*****************************************************************************/

void Grp_DB_RemUsrFromAllGrps (long UsrCod)
  {
   DB_QueryDELETE ("can not remove a user from the groups he/she belongs to",
		   "DELETE FROM grp_users"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/******************* Remove all users from groups in a course ****************/
/*****************************************************************************/

void Grp_DB_RemoveUsrsFromGrpsOfCrs (long HieCod)
  {
   DB_QueryDELETE ("can not remove users from groups of a course",
		   "DELETE FROM grp_users"
		   " USING grp_types,"
			  "grp_groups,"
			  "grp_users"
		   " WHERE grp_types.CrsCod=%ld"
		     " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
		     " AND grp_groups.GrpCod=grp_users.GrpCod",
		   HieCod);
  }

/*****************************************************************************/
/************** Remove all users from groups of a given type *****************/
/*****************************************************************************/

void Grp_DB_RemoveUsrsFromGrpsOfType (long GrpTypCod)
  {
   DB_QueryDELETE ("can not remove users from all groups of a type",
		   "DELETE FROM grp_users"
		   " WHERE GrpCod IN"
		         " (SELECT GrpCod"
		            " FROM grp_groups"
		           " WHERE GrpTypCod=%ld)",
                   GrpTypCod);
  }

/*****************************************************************************/
/******************* Remove all users from a given group *********************/
/*****************************************************************************/

void Grp_DB_RemoveUsrsFromGrp (long GrpCod)
  {
   DB_QueryDELETE ("can not remove users from a group",
		   "DELETE FROM grp_users"
		   " WHERE GrpCod=%ld",
                   GrpCod);
  }

/*****************************************************************************/
/******************** Remove all group types in a course *********************/
/*****************************************************************************/

void Grp_DB_RemoveGrpTypesInCrs (long HieCod)
  {
   DB_QueryDELETE ("can not remove types of group of a course",
		   "DELETE FROM grp_types"
		   " WHERE CrsCod=%ld",
		   HieCod);
  }

/*****************************************************************************/
/*************************** Remove a group type *****************************/
/*****************************************************************************/

void Grp_DB_RemoveGrpType (long GrpTypCod)
  {
   DB_QueryDELETE ("can not remove a type of group",
		   "DELETE FROM grp_types"
		   " WHERE GrpTypCod=%ld",
                   GrpTypCod);
  }

/*****************************************************************************/
/*********************** Remove all groups in a course ***********************/
/*****************************************************************************/

void Grp_DB_RemoveGrpsInCrs (long CrsCod)
  {
   DB_QueryDELETE ("can not remove groups of a course",
		   "DELETE FROM grp_groups"
		   " USING grp_types,"
			  "grp_groups"
		   " WHERE grp_types.CrsCod=%ld"
		     " AND grp_types.GrpTypCod=grp_groups.GrpTypCod",
		   CrsCod);
  }

/*****************************************************************************/
/******************** Remove all groups of a given type **********************/
/*****************************************************************************/

void Grp_DB_RemoveGrpsOfType (long GrpTypCod)
  {
   DB_QueryDELETE ("can not remove groups of a type",
		   "DELETE FROM grp_groups"
		   " WHERE GrpTypCod=%ld",
                   GrpTypCod);
  }

/*****************************************************************************/
/****************************** Remove a group *******************************/
/*****************************************************************************/

void Grp_DB_RemoveGrp (long GrpCod)
  {
   DB_QueryDELETE ("can not remove a group",
		   "DELETE FROM grp_groups"
		   " WHERE GrpCod=%ld",
                   GrpCod);
  }
