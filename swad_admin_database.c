// swad_admin_database.c: administrators operations with database

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_admin.h"
#include "swad_admin_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/** Insert/replace user as administrator of an institution, center or degree */
/*****************************************************************************/

void Adm_DB_InsertAdmin (long UsrCod,HieLvl_Level_t Level,long Cod)
  {
   DB_QueryREPLACE ("can not create administrator",
		    "REPLACE INTO usr_admins"
		    " (UsrCod,Scope,Cod)"
		    " VALUES"
		    " (%ld,'%s',%ld)",
		    UsrCod,
		    Hie_GetDBStrFromLevel (Level),
		    Cod);
  }

/*****************************************************************************/
/********** Get all admin codes above the current course, except me **********/
/*****************************************************************************/

unsigned Adm_DB_GetAdmsCurrentScopeExceptMe (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get admins from current scope",
		   "SELECT UsrCod"
		    " FROM usr_admins"
		   " WHERE (Scope='%s'"
			  " OR (Scope='%s' AND Cod=%ld)"
			  " OR (Scope='%s' AND Cod=%ld)"
			  " OR (Scope='%s' AND Cod=%ld))"
		     " AND UsrCod<>%ld",
		   Hie_GetDBStrFromLevel (HieLvl_SYS),
		   Hie_GetDBStrFromLevel (HieLvl_INS),Gbl.Hierarchy.Ins.Cod,
		   Hie_GetDBStrFromLevel (HieLvl_CTR),Gbl.Hierarchy.Ctr.Cod,
		   Hie_GetDBStrFromLevel (HieLvl_DEG),Gbl.Hierarchy.Deg.Cod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/***** Check if a user is an administrator of a degree/center/institution ****/
/*****************************************************************************/

bool Adm_DB_CheckIfUsrIsAdm (long UsrCod,HieLvl_Level_t Level,long Cod)
  {
   return
   DB_QueryEXISTS ("can not check if a user is administrator",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM usr_admins"
		    " WHERE UsrCod=%ld"
		      " AND Scope='%s'"
		      " AND Cod=%ld)",
		   UsrCod,
		   Hie_GetDBStrFromLevel (Level),
		   Cod);
  }

/*****************************************************************************/
/************** Check if a user is a superuser (global admin) ****************/
/*****************************************************************************/

bool Adm_DB_CheckIfUsrIsSuperuser (long UsrCod)
  {
   return
   DB_QueryEXISTS ("can not check if a user is superuser",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM usr_admins"
		    " WHERE UsrCod=%ld"
		      " AND Scope='%s')",
		   UsrCod,
		   Hie_GetDBStrFromLevel (HieLvl_SYS));
   }

/*****************************************************************************/
/***** Remove user as administrator of an institution, center or degree ******/
/*****************************************************************************/

void Adm_DB_RemAdmin (long UsrCod,HieLvl_Level_t Level,long Cod)
  {
   DB_QueryDELETE ("can not remove an administrator",
		   "DELETE FROM usr_admins"
		   " WHERE UsrCod=%ld"
		     " AND Scope='%s'"
		     " AND Cod=%ld",
		   UsrCod,
		   Hie_GetDBStrFromLevel (Level),
		   Cod);
  }

/*****************************************************************************/
/******* Remove user as administrator of any degree/center/institution *******/
/*****************************************************************************/

void Adm_DB_RemUsrAsAdmin (long UsrCod)
  {
   DB_QueryDELETE ("can not remove a user as administrator",
		   "DELETE FROM usr_admins"
		   " WHERE UsrCod=%ld",
                   UsrCod);
  }

/*****************************************************************************/
/********* Remove administrators of an institution, center or degree *********/
/*****************************************************************************/

void Adm_DB_RemAdmins (HieLvl_Level_t Level,long Cod)
  {
   DB_QueryDELETE ("can not remove administrators",
		   "DELETE FROM usr_admins"
		   " WHERE Scope='%s'"
		     " AND Cod=%ld",
                   Hie_GetDBStrFromLevel (Level),
                   Cod);
  }
