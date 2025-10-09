// swad_resource_database.c: resources, operations with database

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

#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_resource.h"
#include "swad_resource_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

const char *Rsc_DB_Types[Rsc_NUM_TYPES] =
  {
   [Rsc_NONE		] = "non",
   [Rsc_INFORMATION	] = "inf",
   [Rsc_TEACH_GUIDE	] = "gui",
   [Rsc_LECTURES	] = "lec",
   [Rsc_PRACTICALS	] = "pra",
   [Rsc_BIBLIOGRAPHY	] = "bib",
   [Rsc_FAQ		] = "faq",
   [Rsc_LINKS		] = "lnk",
   [Rsc_ASSESSMENT	] = "ass",
   [Rsc_TIMETABLE	] = "tmt",
   [Rsc_ASSIGNMENT	] = "asg",
   [Rsc_PROJECT		] = "prj",
   [Rsc_CALL_FOR_EXAM	] = "cfe",
   [Rsc_TEST		] = "tst",
   [Rsc_EXAM		] = "exa",
   [Rsc_GAME		] = "gam",
   [Rsc_RUBRIC		] = "rub",
   [Rsc_DOCUMENT	] = "doc",
   [Rsc_MARKS		] = "mrk",
   [Rsc_GROUPS		] = "grp",
   [Rsc_TEACHER		] = "tch",
   [Rsc_ATT_EVENT	] = "att",
   [Rsc_FORUM_THREAD	] = "for",
   [Rsc_SURVEY		] = "svy",
  };

/*****************************************************************************/
/********************** Copy link to resource into clipboard *****************/
/*****************************************************************************/

void Rsc_DB_CopyToClipboard (Rsc_Type_t Type,long Cod)
  {
   DB_QueryREPLACE ("can not copy link to resource clipboard",
		    "REPLACE INTO rsc_clipboards"
		    " (UsrCod,CrsCod,Type,Cod,CopyTime)"
		    " VALUES"
		    " (%ld,%ld,'%s',%ld,NOW())",
		    Gbl.Usrs.Me.UsrDat.UsrCod,
		    Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		    Rsc_DB_Types[Type],
		    Cod);
  }

/*****************************************************************************/
/******************** Get number of resources in clipboard *******************/
/*****************************************************************************/

unsigned Rsc_DB_GetNumResourcesInClipboard (void)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get resources",
		  "SELECT COUNT(*)"
		   " FROM rsc_clipboards"
		  " WHERE UsrCod=%ld"
		    " AND CrsCod=%ld",
		  Gbl.Usrs.Me.UsrDat.UsrCod,
		  Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/****************** Get all resources from resource clipboard ****************/
/*****************************************************************************/

unsigned Rsc_DB_GetClipboard (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get clipboard",
		   "SELECT Type,"	// row[0]
			  "Cod"		// row[1]
		    " FROM rsc_clipboards"
		   " WHERE UsrCod=%ld"
		     " AND CrsCod=%ld"
		" ORDER BY CopyTime",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/**************** Remove all resources from resource clipboard ***************/
/*****************************************************************************/

void Rsc_DB_RemoveClipboard (void)
  {
   DB_QueryDELETE ("can not remove clipboard",
		   "DELETE FROM rsc_clipboards"
		   " WHERE UsrCod=%ld"
		     " AND CrsCod=%ld",
 		   Gbl.Usrs.Me.UsrDat.UsrCod,
                   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/*************************** Remove link from clipboard **********************/
/*****************************************************************************/

void Rsc_DB_RemoveLinkFromClipboard (struct Rsc_Link *Link)
  {
   DB_QueryDELETE ("can not remove link from clipboard",
		   "DELETE FROM rsc_clipboards"
		   " WHERE UsrCod=%ld"
		     " AND CrsCod=%ld"
		     " AND Type='%s'"
		     " AND Cod=%ld",
 		   Gbl.Usrs.Me.UsrDat.UsrCod,
                   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Rsc_DB_Types[Link->Type],
		   Link->Cod);
  }

/*****************************************************************************/
/************************** Create a new resource ****************************/
/*****************************************************************************/

long Rsc_DB_CreateRsc (const struct Tre_Node *Node)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new resource",
				"INSERT INTO prg_resources"
				" (NodCod,ItmInd,Hidden,Type,Cod,Title)"
				" SELECT %ld,COALESCE(MAX(t2.ItmInd),0)+1,'N','%s',%ld,'%s'"
				  " FROM prg_resources AS t2"
				 " WHERE t2.NodCod=%ld",
				Node->Hierarchy.NodCod,
			        Rsc_DB_Types[Node->Item.Rsc.Link.Type],
			        Node->Item.Rsc.Link.Cod,
				Node->Item.Rsc.Title,
				Node->Hierarchy.NodCod);
  }

/*****************************************************************************/
/****************** Get list of node resources from database *****************/
/*****************************************************************************/

unsigned Rsc_DB_GetListRscs (MYSQL_RES **mysql_res,long NodCod,
                             Lay_Show_t ShowHiddenResources)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];
   static const char *HiddenSubQuery[Lay_NUM_SHOW] =
     {
      [Lay_DONT_SHOW] = " AND prg_resources.Hidden='N'",
      [Lay_SHOW     ] = "",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get node resources",
		   "SELECT prg_resources.NodCod,"	// row[0]
			  "prg_resources.ItmCod,"	// row[1]
                          "prg_resources.ItmInd,"	// row[2]
			  "prg_resources.Hidden,"	// row[3]
			  "prg_resources.Type,"		// row[4]
			  "prg_resources.Cod,"		// row[5]
			  "prg_resources.Title"		// row[6]
		    " FROM prg_resources,"
		          "tre_nodes"
		   " WHERE prg_resources.NodCod=%ld"
		       "%s"
		     " AND prg_resources.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'"		// Extra check
		" ORDER BY prg_resources.ItmInd",
		   NodCod,
		   HiddenSubQuery[ShowHiddenResources],
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_PROGRAM]);
  }

/*****************************************************************************/
/******************** Get resource data using its code ***********************/
/*****************************************************************************/

Exi_Exist_t Rsc_DB_GetRscDataByCod (MYSQL_RES **mysql_res,long ItmCod)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   return
   DB_QuerySELECTunique (mysql_res,"can not get node resource data",
			 "SELECT prg_resources.NodCod,"	// row[0]
				"prg_resources.ItmCod,"	// row[1]
				"prg_resources.ItmInd,"	// row[2]
				"prg_resources.Hidden,"	// row[3]
				"prg_resources.Type,"	// row[4]
				"prg_resources.Cod,"	// row[5]
				"prg_resources.Title"	// row[6]
			  " FROM prg_resources,"
				"tre_nodes"
			 " WHERE prg_resources.ItmCod=%ld"
			   " AND prg_resources.NodCod=tre_nodes.NodCod"
			   " AND tre_nodes.CrsCod=%ld"	// Extra check
			   " AND tre_nodes.Type='%s'",	// Extra check
			 ItmCod,
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			 Tre_DB_Types[Inf_PROGRAM]);
  }

/*****************************************************************************/
/************************ Update resource given its code *********************/
/*****************************************************************************/

void Rsc_DB_UpdateRsc (const struct Tre_Node *Node)
  {
   extern const char *Rsc_DB_Types[Rsc_NUM_TYPES];
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryUPDATE ("can not update resource",
		   "UPDATE prg_resources,"
		          "tre_nodes"
		     " SET prg_resources.Type='%s',"
		          "prg_resources.Cod=%ld,"
			  "prg_resources.Title='%s'"
		   " WHERE prg_resources.ItmCod=%ld"
		     " AND prg_resources.NodCod=%ld"
		     " AND prg_resources.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   Rsc_DB_Types[Node->Item.Rsc.Link.Type],
		   Node->Item.Rsc.Link.Cod,
		   Node->Item.Rsc.Title,
		   Node->Item.Cod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_PROGRAM]);
  }
