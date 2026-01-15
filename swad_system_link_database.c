// swad_system_link_database.c: system links, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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
#include "swad_system_link_database.h"

/*****************************************************************************/
/**************************** Create a new link ******************************/
/*****************************************************************************/

void SysLnk_DB_CreateLink (const struct SysLnk_Link *Lnk)
  {
   DB_QueryINSERT ("can not create system link",
		   "INSERT INTO lnk_links"
		   " (ShortName,FullName,WWW)"
		   " VALUES"
		   " ('%s','%s','%s')",
                   Lnk->ShrtName,
                   Lnk->FullName,
                   Lnk->WWW);
  }

/*****************************************************************************/
/************ Update link name in table of institutional links ***************/
/*****************************************************************************/

void SysLnk_DB_UpdateLnkName (long LnkCod,const char *FldName,const char *NewLnkName)
  {
   DB_QueryUPDATE ("can not update the name of a system link",
		   "UPDATE lnk_links"
		     " SET %s='%s'"
		   " WHERE LnkCod=%ld",
	           FldName,NewLnkName,
	           LnkCod);
  }

/*****************************************************************************/
/****************** Update link changing old WWW by new WWW ******************/
/*****************************************************************************/

void SysLnk_DB_UpdateLnkWWW (long LnkCod,const char NewWWW[WWW_MAX_BYTES_WWW + 1])
  {
   DB_QueryUPDATE ("can not update the web of a system link",
		   "UPDATE lnk_links"
		     " SET WWW='%s'"
		   " WHERE LnkCod=%ld",
		   NewWWW,
		   LnkCod);
  }

/*****************************************************************************/
/************************** Get institutional links **************************/
/*****************************************************************************/

unsigned SysLnk_DB_GetLinks (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get institutional links",
		   "SELECT LnkCod,"	// row[0]
			  "ShortName,"	// row[1]
			  "FullName,"	// row[2]
			  "WWW"		// row[3]
		    " FROM lnk_links"
		" ORDER BY ShortName");
  }

/*****************************************************************************/
/**************************** Get link full name *****************************/
/*****************************************************************************/

Exi_Exist_t SysLnk_DB_GetLinkDataByCod (MYSQL_RES **mysql_res,long LnkCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get data of a system link",
			 "SELECT LnkCod,"	// row[0]
				"ShortName,"	// row[1]
				"FullName,"	// row[2]
				"WWW"		// row[3]
			  " FROM lnk_links"
			 " WHERE LnkCod=%ld",
			 LnkCod);
  }

/*****************************************************************************/
/********************** Check if the name of link exists *********************/
/*****************************************************************************/

Exi_Exist_t SysLnk_DB_CheckIfLinkNameExists (const char *FldName,const char *Name,long Cod,
					     __attribute__((unused)) long PrtCod,
					     __attribute__((unused)) unsigned Year)
  {
   return
   DB_QueryEXISTS ("can not check if the name of a system link already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM lnk_links"
		    " WHERE %s='%s'"
		      " AND LnkCod<>%ld)",
		   FldName,Name,
		   Cod);
  }

/*****************************************************************************/
/******************************* Remove a link *******************************/
/*****************************************************************************/

void SysLnk_DB_RemoveLink (long LnkCod)
  {
   DB_QueryDELETE ("can not remove a system link",
		   "DELETE FROM lnk_links"
		   " WHERE LnkCod=%ld",
		   LnkCod);
  }
