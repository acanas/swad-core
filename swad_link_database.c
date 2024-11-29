// swad_link_database.c: course links, operations with database

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
#include "swad_link.h"
#include "swad_link_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Create a new course link **************************/
/*****************************************************************************/

long Lnk_DB_CreateCrsLink (const struct Tre_Node *Node)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new course link",
				"INSERT INTO crs_links"
				" (NodCod,ItmInd,Hidden,Title,Description,WWW)"
				" SELECT %ld,COALESCE(MAX(t2.ItmInd),0)+1,'N','%s','%s','%s'"
				  " FROM crs_links AS t2"
				 " WHERE t2.NodCod=%ld",
				Node->Hierarchy.NodCod,
				Node->Lnk.Fields[Lnk_TITLE	],
				Node->Lnk.Fields[Lnk_DESCRIPTION],
				Node->Lnk.WWW,
				Node->Hierarchy.NodCod);
  }

/*****************************************************************************/
/*************** Get list of node course links from database *****************/
/*****************************************************************************/

unsigned Lnk_DB_GetListCrsLinks (MYSQL_RES **mysql_res,long NodCod,
                                 bool ShowHiddenCrsLinks)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];
   static const char *HiddenSubQuery[2] =
     {
      [false] = " AND crs_links.Hidden='N'",
      [true ] = "",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get node course links",
		   "SELECT crs_links.NodCod,"		// row[0]
			  "crs_links.ItmCod,"		// row[1]
                          "crs_links.ItmInd,"		// row[2]
			  "crs_links.Hidden,"		// row[3]
			  "crs_links.Title,"		// row[4]
			  "crs_links.Description,"	// row[5]
			  "crs_links.WWW"		// row[6]
		    " FROM crs_links,"
		          "tre_nodes"
		   " WHERE crs_links.NodCod=%ld"
		       "%s"
		     " AND crs_links.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'"		// Extra check
		" ORDER BY crs_links.ItmInd",
		   NodCod,
		   HiddenSubQuery[ShowHiddenCrsLinks],
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_LINKS]);
  }

/*****************************************************************************/
/******************* Get course link data using its code *********************/
/*****************************************************************************/

unsigned Lnk_DB_GetCrsLinkDataByCod (MYSQL_RES **mysql_res,long ItmCod)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get node course link data",
		   "SELECT crs_links.NodCod,"		// row[0]
			  "crs_links.ItmCod,"		// row[1]
                          "crs_links.ItmInd,"		// row[2]
			  "crs_links.Hidden,"		// row[3]
			  "crs_links.Title,"		// row[4]
			  "crs_links.Description,"	// row[5]
			  "crs_links.WWW"		// row[6]
		    " FROM crs_links,"
		          "tre_nodes"
		   " WHERE crs_links.ItmCod=%ld"
		     " AND crs_links.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   ItmCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_LINKS]);
  }

/*****************************************************************************/
/********************* Update course link given its code *********************/
/*****************************************************************************/

void Lnk_DB_UpdateCrsLink (const struct Tre_Node *Node)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryUPDATE ("can not update course link",
		   "UPDATE crs_links,"
		          "tre_nodes"
		     " SET crs_links.Title='%s',"
		          "crs_links.Description='%s',"
		          "crs_links.WWW='%s'"
		   " WHERE crs_links.ItmCod=%ld"
		     " AND crs_links.NodCod=%ld"
		     " AND crs_links.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   Node->Lnk.Fields[Lnk_TITLE		],
		   Node->Lnk.Fields[Lnk_DESCRIPTION	],
		   Node->Lnk.WWW,
		   Node->SpcItem.Cod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_LINKS]);
  }
