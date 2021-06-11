// swad_admin_database.c: administrators operations with database

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

#include "swad_database.h"
#include "swad_admin.h"
#include "swad_admin_database.h"
// #include "swad_error.h"
// #include "swad_global.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************** Private global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/** Insert/replace user as administrator of an institution, center or degree */
/*****************************************************************************/

void Adm_DB_InsertAdmin (long UsrCod,HieLvl_Level_t Scope,long Cod)
  {
   DB_QueryREPLACE ("can not create administrator",
		    "REPLACE INTO usr_admins"
		    " (UsrCod,Scope,Cod)"
		    " VALUES"
		    " (%ld,'%s',%ld)",
		    UsrCod,
		    Sco_GetDBStrFromScope (Scope),
		    Cod);
  }

/*****************************************************************************/
/***** Remove user as administrator of an institution, center or degree ******/
/*****************************************************************************/

void Adm_DB_RemAdmin (long UsrCod,HieLvl_Level_t Scope,long Cod)
  {
   DB_QueryDELETE ("can not remove an administrator",
		   "DELETE FROM usr_admins"
		   " WHERE UsrCod=%ld"
		     " AND Scope='%s'"
		     " AND Cod=%ld",
		   UsrCod,
		   Sco_GetDBStrFromScope (Scope),
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

void Adm_DB_RemAdmins (HieLvl_Level_t Scope,long Cod)
  {
   DB_QueryDELETE ("can not remove administrators",
		   "DELETE FROM usr_admins"
		   " WHERE Scope='%s'"
		     " AND Cod=%ld",
                   Sco_GetDBStrFromScope (Scope),
                   Cod);
  }
