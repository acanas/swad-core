// swad_resource_database.c: resources, operations with database

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

long Rsc_DB_CreateResource (const struct Tre_Node *Node)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new resource",
				"INSERT INTO prg_resources"
				" (NodCod,ItmInd,Hidden,Type,Cod,Title)"
				" SELECT %ld,COALESCE(MAX(t2.ItmInd),0)+1,'N','%s',%ld,'%s'"
				  " FROM prg_resources AS t2"
				 " WHERE t2.NodCod=%ld",
				Node->Hierarchy.NodCod,
			        Rsc_DB_Types[Node->Resource.Link.Type],
			        Node->Resource.Link.Cod,
				Node->Resource.Title,
				Node->Hierarchy.NodCod);
  }

/*****************************************************************************/
/****************** Get list of node resources from database *****************/
/*****************************************************************************/

unsigned Rsc_DB_GetListResources (MYSQL_RES **mysql_res,long NodCod,
                                  bool ShowHiddenResources)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];
   static const char *HiddenSubQuery[2] =
     {
      [false] = " AND prg_resources.Hidden='N'",
      [true ] = "",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get node resources",
		   "SELECT prg_resources.NodCod,"	// row[0]
			  "prg_resources.RscCod,"	// row[1]
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

unsigned Rsc_DB_GetResourceDataByCod (MYSQL_RES **mysql_res,long RscCod)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get node resource data",
		   "SELECT prg_resources.NodCod,"	// row[0]
			  "prg_resources.RscCod,"	// row[1]
                          "prg_resources.ItmInd,"	// row[2]
			  "prg_resources.Hidden,"	// row[3]
			  "prg_resources.Type,"		// row[4]
			  "prg_resources.Cod,"		// row[5]
			  "prg_resources.Title"		// row[6]
		    " FROM prg_resources,"
		          "tre_nodes"
		   " WHERE prg_resources.RscCod=%ld"
		     " AND prg_resources.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   RscCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_PROGRAM]);
  }

/*****************************************************************************/
/*********** Get resource code given node code and resource index ************/
/*****************************************************************************/

long Rsc_DB_GetRscCodFromRscInd (long NodCod,unsigned ItmInd)
  {
   /***** Trivial check: resource index should be > 0 *****/
   if (ItmInd == 0)
      return -1L;

   /***** Get resource code given node code and resource index *****/
   return DB_QuerySELECTCode ("can not get resource code",
			      "SELECT RscCod"
			       " FROM prg_resources"
			      " WHERE NodCod=%ld"
				" AND ItmInd=%u",
			      NodCod,ItmInd);
  }

/*****************************************************************************/
/************************** Remove a note resource ***************************/
/*****************************************************************************/

void Rsc_DB_RemoveResource (const struct Tre_Node *Node)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryDELETE ("can not remove node resource",
		   "DELETE FROM prg_resources"
		   " USING prg_resources,"
		          "tre_nodes"
		   " WHERE prg_resources.RscCod=%ld"
		     " AND prg_resources.NodCod=%ld"
                     " AND prg_resources.NodCod=tre_nodes.NodCod"
                     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   Node->SpcItem.Cod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_PROGRAM]);
  }

/*****************************************************************************/
/************************ Hide/unhide a node resource ************************/
/*****************************************************************************/

void Rsc_DB_HideOrUnhideResource (const struct Tre_Node *Node,
				  HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char HidVis_YN[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryUPDATE ("can not hide/unhide node resource",
		   "UPDATE prg_resources,"
		          "tre_nodes"
		     " SET prg_resources.Hidden='%c'"
		   " WHERE prg_resources.RscCod=%ld"
		     " AND prg_resources.NodCod=%ld"
		     " AND prg_resources.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   HidVis_YN[HiddenOrVisible],
		   Node->SpcItem.Cod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_PROGRAM]);
  }

/*****************************************************************************/
/********** Lock table to make the exchange of resources atomic **************/
/*****************************************************************************/

void Rsc_DB_LockTableResources (void)
  {
   DB_Query ("can not lock tables",
	     "LOCK TABLES prg_resources WRITE,"
			 "tre_nodes READ");
   DB_SetThereAreLockedTables ();
  }

/*****************************************************************************/
/************* Update the index of a resource given its code *****************/
/*****************************************************************************/

void Rsc_DB_UpdateRscInd (const struct Tre_Node *Node,long RscCod,int ItmInd)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryUPDATE ("can not update index of resource",
		   "UPDATE prg_resources,"
		          "tre_nodes"
		     " SET prg_resources.ItmInd=%d"
		   " WHERE prg_resources.RscCod=%ld"
		     " AND prg_resources.NodCod=%ld"
		     " AND prg_resources.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   ItmInd,
		   RscCod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_PROGRAM]);
  }

/*****************************************************************************/
/************************ Update resource given its code *********************/
/*****************************************************************************/

void Rsc_DB_UpdateResource (const struct Tre_Node *Node)
  {
   extern const char *Rsc_DB_Types[Rsc_NUM_TYPES];
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryUPDATE ("can not update resource",
		   "UPDATE prg_resources,"
		          "tre_nodes"
		     " SET prg_resources.Type='%s',"
		          "prg_resources.Cod=%ld,"
			  "prg_resources.Title='%s'"
		   " WHERE prg_resources.RscCod=%ld"
		     " AND prg_resources.NodCod=%ld"
		     " AND prg_resources.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   Rsc_DB_Types[Node->Resource.Link.Type],
		   Node->Resource.Link.Cod,
		   Node->Resource.Title,
		   Node->SpcItem.Cod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_PROGRAM]);
  }
