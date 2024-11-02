// swad_program_database.c: course program, operations with database

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

#include "swad_error.h"
#include "swad_global.h"
#include "swad_program.h"
#include "swad_program_database.h"
#include "swad_program_resource.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************** Create a new resource ****************************/
/*****************************************************************************/

long Prg_DB_CreateResource (const struct Tre_Node *Node)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new resource",
				"INSERT INTO prg_resources"
				" (NodCod,RscInd,Hidden,Title)"
				" SELECT %ld,COALESCE(MAX(t2.RscInd),0)+1,'N','%s'"
				  " FROM prg_resources AS t2"
				 " WHERE t2.NodCod=%ld",
				Node->Hierarchy.NodCod,
				Node->Resource.Title,
				Node->Hierarchy.NodCod);
  }

/*****************************************************************************/
/**************************** Update resource title **************************/
/*****************************************************************************/

void Prg_DB_UpdateResourceTitle (long NodCod,long RscCod,
                                 const char NewTitle[Rsc_MAX_BYTES_RESOURCE_TITLE + 1])
  {
   extern const char *Tre_DB_Types[Tre_NUM_TYPES];

   DB_QueryUPDATE ("can not update the title of a resource",
		   "UPDATE prg_resources,"
		          "tre_nodes"
		     " SET prg_resources.Title='%s'"
		   " WHERE prg_resources.RscCod=%ld"
		     " AND prg_resources.NodCod=%ld"
		     " AND prg_resources.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
	           NewTitle,
	           RscCod,
	           NodCod,
	           Gbl.Hierarchy.Node[Hie_CRS].HieCod,
	           Tre_DB_Types[Tre_PROGRAM]);
  }

/*****************************************************************************/
/****************** Get list of node resources from database *****************/
/*****************************************************************************/

unsigned Prg_DB_GetListResources (MYSQL_RES **mysql_res,long NodCod,
                                  bool ShowHiddenResources)
  {
   extern const char *Tre_DB_Types[Tre_NUM_TYPES];
   static const char *HiddenSubQuery[2] =
     {
      [false] = " AND prg_resources.Hidden='N'",
      [true ] = "",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get node resources",
		   "SELECT prg_resources.NodCod,"	// row[0]
			  "prg_resources.RscCod,"	// row[1]
                          "prg_resources.RscInd,"	// row[2]
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
		   " ORDER BY prg_resources.RscInd",
		   NodCod,
		   HiddenSubQuery[ShowHiddenResources],
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Tre_PROGRAM]);
  }

/*****************************************************************************/
/******************** Get resource data using its code ***********************/
/*****************************************************************************/

unsigned Prg_DB_GetResourceDataByCod (MYSQL_RES **mysql_res,long RscCod)
  {
   extern const char *Tre_DB_Types[Tre_NUM_TYPES];

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get node resource data",
		   "SELECT prg_resources.NodCod,"	// row[0]
			  "prg_resources.RscCod,"	// row[1]
                          "prg_resources.RscInd,"	// row[2]
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
		   Tre_DB_Types[Tre_PROGRAM]);
  }

/*****************************************************************************/
/************* Get the resource index before/after a given one ***************/
/*****************************************************************************/

unsigned Prg_DB_GetRscIndBefore (const struct Tre_Node *Node)
  {
   return
   DB_QuerySELECTUnsigned ("can not get the resource before",
			   "SELECT COALESCE(MAX(RscInd),0)"
			    " FROM prg_resources"
			   " WHERE NodCod=%ld"
			     " AND RscInd<%u",
			   Node->Hierarchy.NodCod,
			   Node->Resource.Hierarchy.RscInd);
  }

unsigned Prg_DB_GetRscIndAfter (const struct Tre_Node *Node)
  {
   return
   DB_QuerySELECTUnsigned ("can not get the resource after",
			   "SELECT COALESCE(MIN(RscInd),0)"
			    " FROM prg_resources"
			   " WHERE NodCod=%ld"
			     " AND RscInd>%u",
			   Node->Hierarchy.NodCod,
			   Node->Resource.Hierarchy.RscInd);
  }

/*****************************************************************************/
/*********** Get resource code given node code and resource index ************/
/*****************************************************************************/

long Prg_DB_GetRscCodFromRscInd (long NodCod,unsigned RscInd)
  {
   /***** Trivial check: resource index should be > 0 *****/
   if (RscInd == 0)
      return -1L;

   /***** Get resource code given node code and resource index *****/
   return DB_QuerySELECTCode ("can not get resource code",
			      "SELECT RscCod"
			       " FROM prg_resources"
			      " WHERE NodCod=%ld"
				" AND RscInd=%u",
			      NodCod,RscInd);
  }

/*****************************************************************************/
/************************** Remove a note resource ***************************/
/*****************************************************************************/

void Prg_DB_RemoveResource (const struct Tre_Node *Node)
  {
   extern const char *Tre_DB_Types[Tre_NUM_TYPES];

   DB_QueryDELETE ("can not remove node resource",
		   "DELETE FROM prg_resources"
		   " USING prg_resources,"
		          "tre_nodes"
		   " WHERE prg_resources.RscCod=%ld"
		     " AND prg_resources.NodCod=%ld"
                     " AND prg_resources.NodCod=tre_nodes.NodCod"
                     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   Node->Resource.Hierarchy.RscCod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Tre_PROGRAM]);
  }

/*****************************************************************************/
/************************ Hide/unhide a node resource ************************/
/*****************************************************************************/

void Prg_DB_HideOrUnhideResource (const struct Tre_Node *Node,
				  HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char HidVis_YN[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Tre_DB_Types[Tre_NUM_TYPES];

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
		   Node->Resource.Hierarchy.RscCod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Tre_PROGRAM]);
  }

/*****************************************************************************/
/********** Lock table to make the exchange of resources atomic **************/
/*****************************************************************************/

void Prg_DB_LockTableResources (void)
  {
   DB_Query ("can not lock tables",
	     "LOCK TABLES prg_resources WRITE,"
			 "tre_nodes READ");
   DB_SetThereAreLockedTables ();
  }

/*****************************************************************************/
/************* Update the index of a resource given its code *****************/
/*****************************************************************************/

void Prg_DB_UpdateRscInd (const struct Tre_Node *Node,long RscCod,int RscInd)
  {
   extern const char *Tre_DB_Types[Tre_NUM_TYPES];

   DB_QueryUPDATE ("can not update index of resource",
		   "UPDATE prg_resources,"
		          "tre_nodes"
		     " SET prg_resources.RscInd=%d"
		   " WHERE prg_resources.RscCod=%ld"
		     " AND prg_resources.NodCod=%ld"
		     " AND prg_resources.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   RscInd,
		   RscCod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Tre_PROGRAM]);
  }

/*****************************************************************************/
/************* Update the link of a resource given its code *****************/
/*****************************************************************************/

void Prg_DB_UpdateRscLink (const struct Tre_Node *Node)
  {
   extern const char *Rsc_DB_Types[Rsc_NUM_TYPES];
   extern const char *Tre_DB_Types[Tre_NUM_TYPES];

   DB_QueryUPDATE ("can not update link of resource",
		   "UPDATE prg_resources,"
		          "tre_nodes"
		     " SET prg_resources.Type='%s',"
		          "prg_resources.Cod=%ld"
		   " WHERE prg_resources.RscCod=%ld"
		     " AND prg_resources.NodCod=%ld"
		     " AND prg_resources.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   Rsc_DB_Types[Node->Resource.Link.Type],
		   Node->Resource.Link.Cod,
		   Node->Resource.Hierarchy.RscCod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Tre_PROGRAM]);
  }
