// swad_institution_database.c: institutions operations with database

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

// #include <stdbool.h>		// For boolean type
// #include <stddef.h>		// For NULL
// #include <stdlib.h>		// For free
// #include <string.h>		// For string functions

// #include "swad_admin_database.h"
#include "swad_database.h"
// #include "swad_department.h"
// #include "swad_enrolment_database.h"
// #include "swad_error.h"
// #include "swad_figure.h"
// #include "swad_figure_cache.h"
// #include "swad_form.h"
// #include "swad_forum.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
// #include "swad_hierarchy_level.h"
// #include "swad_HTML.h"
#include "swad_institution.h"
#include "swad_institution_database.h"
// #include "swad_logo.h"
// #include "swad_message.h"
// #include "swad_place.h"
// #include "swad_survey.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************** Create a new institution *************************/
/*****************************************************************************/

long Ins_DB_CreateInstitution (const struct Ins_Instit *Ins,unsigned Status)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create institution",
				"INSERT INTO ins_instits"
				" (CtyCod,Status,RequesterUsrCod,"
				  "ShortName,FullName,WWW)"
				" VALUES"
				" (%ld,%u,%ld,"
				  "'%s','%s','%s')",
				Ins->CtyCod,
				Status,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Ins->ShrtName,
				Ins->FullName,
				Ins->WWW);
  }

/*****************************************************************************/
/****************** Update country in table of institutions ******************/
/*****************************************************************************/

void Ins_DB_UpdateInsCty (long InsCod,long CtyCod)
  {
   /***** Update country in table of institutions *****/
   DB_QueryUPDATE ("can not update the country of an institution",
		   "UPDATE ins_instits"
		     " SET CtyCod=%ld"
		   " WHERE InsCod=%ld",
                   CtyCod,
                   InsCod);
  }

/*****************************************************************************/
/**************** Update database changing old WWW by new WWW ****************/
/*****************************************************************************/

void Ins_DB_UpdateInsWWW (long InsCod,const char NewWWW[Cns_MAX_BYTES_WWW + 1])
  {
   /***** Update database changing old WWW by new WWW *****/
   DB_QueryUPDATE ("can not update the web of an institution",
		   "UPDATE ins_instits"
		     " SET WWW='%s'"
		   " WHERE InsCod=%ld",
	           NewWWW,
	           InsCod);
  }

/*****************************************************************************/
/*********** Get the short name of an institution from its code **************/
/*****************************************************************************/

void Ins_DB_GetShortNameOfInstitution (long InsCod,char ShrtName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1])
  {
   /***** Trivial check: institution code should be > 0 *****/
   if (InsCod <= 0)
     {
      ShrtName[0] = '\0';	// Empty name
      return;
     }

   /***** Get short name of institution from database *****/
   DB_QuerySELECTString (ShrtName,Cns_HIERARCHY_MAX_BYTES_SHRT_NAME,
			 "can not get the short name of an institution",
			 "SELECT ShortName"
			  " FROM ins_instits"
			 " WHERE InsCod=%ld",
			 InsCod);
  }

/*****************************************************************************/
/****** Check if the name of institution exists in the current country *******/
/*****************************************************************************/

bool Ins_DB_CheckIfInsNameExistsInCty (const char *FieldName,
                                       const char *Name,
				       long InsCod,
				       long CtyCod)
  {
   /***** Get number of institutions in current country with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of an institution"
			  " already existed",
			  "SELECT COUNT(*)"
			   " FROM ins_instits"
			  " WHERE CtyCod=%ld"
			    " AND %s='%s'"
			    " AND InsCod<>%ld",
			  CtyCod,FieldName,Name,InsCod) != 0);
  }

/*****************************************************************************/
/****************** Get number of institutions with centres ******************/
/*****************************************************************************/

unsigned Ins_DB_GetNumInssWithCtrs (HieLvl_Level_t Scope,long Cod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Scope,Cod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of institutions with centers",
		  "SELECT COUNT(DISTINCT ins_instits.InsCod)"
		   " FROM ins_instits,"
			 "ctr_centers"
		  " WHERE %sinstitutions.InsCod=ctr_centers.InsCod",
		  SubQuery);
  }

/*****************************************************************************/
/****************** Get number of institutions with degrees ******************/
/*****************************************************************************/

unsigned Ins_DB_GetNumInssWithDegs (HieLvl_Level_t Scope,long Cod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Scope,Cod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of institutions with degrees",
		  "SELECT COUNT(DISTINCT ins_instits.InsCod)"
		   " FROM ins_instits,"
			 "ctr_centers,"
			 "deg_degrees"
		  " WHERE %sinstitutions.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod",
		  SubQuery);
  }

/*****************************************************************************/
/****************** Get number of institutions with courses ******************/
/*****************************************************************************/

unsigned Ins_DB_GetNumInssWithCrss (HieLvl_Level_t Scope,long Cod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Scope,Cod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of institutions with courses",
		  "SELECT COUNT(DISTINCT ins_instits.InsCod)"
		   " FROM ins_instits,"
			 "ctr_centers,"
			 "deg_degrees,"
			 "crs_courses"
		  " WHERE %sinstitutions.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		    " AND deg_degrees.DegCod=crs_courses.DegCod",
		  SubQuery);
  }

/*****************************************************************************/
/************* Get current number of institutions with users *****************/
/*****************************************************************************/

unsigned Ins_DB_GetNumInnsWithUsrs (Rol_Role_t Role,
                                    HieLvl_Level_t Scope,long Cod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Scope,Cod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of institutions with users",
		  "SELECT COUNT(DISTINCT ins_instits.InsCod)"
		   " FROM ins_instits,"
			 "ctr_centers,"
			 "deg_degrees,"
			 "crs_courses,"
			 "crs_users"
		  " WHERE %s"
			 "ins_instits.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		    " AND deg_degrees.DegCod=crs_courses.DegCod"
		    " AND crs_courses.CrsCod=crs_users.CrsCod"
		    " AND crs_users.Role=%u",
		  SubQuery,(unsigned) Role);
  }
