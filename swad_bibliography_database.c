// swad_bibliography_database.c: bibliographic references, operations with database

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_bibliography.h"
#include "swad_bibliography_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************* Create a new bibliographic reference ********************/
/*****************************************************************************/

long Bib_DB_CreateRef (const struct Tre_Node *Node)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new bibliographic reference",
				"INSERT INTO crs_bibliography"
				" (NodCod,ItmInd,Hidden,"
				  "Authors,Title,Source,Publisher,Date,Id,URL)"
				" SELECT %ld,COALESCE(MAX(t2.ItmInd),0)+1,'N',"
				        "'%s','%s','%s','%s','%s','%s','%s'"
				  " FROM crs_bibliography AS t2"
				 " WHERE t2.NodCod=%ld",
				Node->Hierarchy.NodCod,
				Node->Item.Bib.Fields[Bib_AUTHORS	],
				Node->Item.Bib.Fields[Bib_TITLE		],
				Node->Item.Bib.Fields[Bib_SOURCE	],
				Node->Item.Bib.Fields[Bib_PUBLISHER	],
				Node->Item.Bib.Fields[Bib_DATE		],
				Node->Item.Bib.Fields[Bib_ID		],
				Node->Item.Bib.URL,
				Node->Hierarchy.NodCod);
  }

/*****************************************************************************/
/******** Get list of node bibliographic references from database ************/
/*****************************************************************************/

unsigned Bib_DB_GetListRefs (MYSQL_RES **mysql_res,
			     long NodCod,bool ShowHiddenBibRefs)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];
   static const char *HiddenSubQuery[2] =
     {
      [false] = " AND crs_bibliography.Hidden='N'",
      [true ] = "",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get node bibliographic references",
		   "SELECT crs_bibliography.NodCod,"	// row[ 0]
			  "crs_bibliography.ItmCod,"	// row[ 1]
                          "crs_bibliography.ItmInd,"	// row[ 2]
			  "crs_bibliography.Hidden,"	// row[ 3]
			  "crs_bibliography.Authors,"	// row[ 4]
			  "crs_bibliography.Title,"	// row[ 5]
			  "crs_bibliography.Source,"	// row[ 6]
			  "crs_bibliography.Publisher,"	// row[ 7]
			  "crs_bibliography.Date,"	// row[ 8]
			  "crs_bibliography.Id,"	// row[ 9]
			  "crs_bibliography.URL"	// row[10]
		    " FROM crs_bibliography,"
		          "tre_nodes"
		   " WHERE crs_bibliography.NodCod=%ld"
		       "%s"
		     " AND crs_bibliography.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'"		// Extra check
		" ORDER BY crs_bibliography.ItmInd",
		   NodCod,
		   HiddenSubQuery[ShowHiddenBibRefs],
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_BIBLIOGRAPHY]);
  }

/*****************************************************************************/
/************* Get bibliographic reference data using its code ***************/
/*****************************************************************************/

Exi_Exist_t Bib_DB_GetRefDataByCod (MYSQL_RES **mysql_res,long ItmCod)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   return
   DB_QuerySELECTunique (mysql_res,"can not get node bibliographic reference data",
			 "SELECT crs_bibliography.NodCod,"	// row[ 0]
				"crs_bibliography.ItmCod,"	// row[ 1]
				"crs_bibliography.ItmInd,"	// row[ 2]
				"crs_bibliography.Hidden,"	// row[ 3]
				"crs_bibliography.Authors,"	// row[ 4]
				"crs_bibliography.Title,"	// row[ 5]
				"crs_bibliography.Source,"	// row[ 6]
				"crs_bibliography.Publisher,"	// row[ 7]
				"crs_bibliography.Date,"	// row[ 8]
				"crs_bibliography.Id,"	// row[ 9]
				"crs_bibliography.URL"	// row[10]
			  " FROM crs_bibliography,"
				"tre_nodes"
			 " WHERE crs_bibliography.ItmCod=%ld"
			   " AND crs_bibliography.NodCod=tre_nodes.NodCod"
			   " AND tre_nodes.CrsCod=%ld"	// Extra check
			   " AND tre_nodes.Type='%s'",	// Extra check
			 ItmCod,
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			 Tre_DB_Types[Inf_BIBLIOGRAPHY]);
  }

/*****************************************************************************/
/*************** Update bibliographic reference given its code ***************/
/*****************************************************************************/

void Bib_DB_UpdateRef (const struct Tre_Node *Node)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryUPDATE ("can not update bibliographic reference",
		   "UPDATE crs_bibliography,"
		          "tre_nodes"
		     " SET crs_bibliography.Authors='%s',"
			  "crs_bibliography.Title='%s',"
			  "crs_bibliography.Source='%s',"
			  "crs_bibliography.Publisher='%s',"
			  "crs_bibliography.Date='%s',"
			  "crs_bibliography.Id='%s',"
			  "crs_bibliography.URL='%s'"
		   " WHERE crs_bibliography.ItmCod=%ld"
		     " AND crs_bibliography.NodCod=%ld"
		     " AND crs_bibliography.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   Node->Item.Bib.Fields[Bib_AUTHORS	],
		   Node->Item.Bib.Fields[Bib_TITLE	],
		   Node->Item.Bib.Fields[Bib_SOURCE	],
		   Node->Item.Bib.Fields[Bib_PUBLISHER	],
		   Node->Item.Bib.Fields[Bib_DATE	],
		   Node->Item.Bib.Fields[Bib_ID		],
		   Node->Item.Bib.URL,
		   Node->Item.Cod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_BIBLIOGRAPHY]);
  }
