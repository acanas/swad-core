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
				" (NodCod,LnkInd,Hidden,Title,Description,WWW)"
				" SELECT %ld,COALESCE(MAX(t2.LnkInd),0)+1,'N','%s','%s','%s'"
				  " FROM crs_links AS t2"
				 " WHERE t2.NodCod=%ld",
				Node->Hierarchy.NodCod,
				Node->Lnk.Title,
				Node->Lnk.Description,
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
			  "crs_links.LnkCod,"		// row[1]
                          "crs_links.LnkInd,"		// row[2]
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
		" ORDER BY crs_links.LnkInd",
		   NodCod,
		   HiddenSubQuery[ShowHiddenCrsLinks],
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_LINKS]);
  }

/*****************************************************************************/
/******************* Get course link data using its code *********************/
/*****************************************************************************/

unsigned Lnk_DB_GetCrsLinkDataByCod (MYSQL_RES **mysql_res,long LnkCod)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get node course link data",
		   "SELECT crs_links.NodCod,"		// row[0]
			  "crs_links.LnkCod,"		// row[1]
                          "crs_links.LnkInd,"		// row[2]
			  "crs_links.Hidden,"		// row[3]
			  "crs_links.Title,"		// row[4]
			  "crs_links.Description,"	// row[5]
			  "crs_links.WWW"		// row[6]
		    " FROM crs_links,"
		          "tre_nodes"
		   " WHERE crs_links.LnkCod=%ld"
		     " AND crs_links.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   LnkCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_LINKS]);
  }

/*****************************************************************************/
/*********** Get the course link index before/after a given one **************/
/*****************************************************************************/

unsigned Lnk_DB_GetLnkIndBefore (const struct Tre_Node *Node)
  {
   return
   DB_QuerySELECTUnsigned ("can not get the course link before",
			   "SELECT COALESCE(MAX(LnkInd),0)"
			    " FROM crs_links"
			   " WHERE NodCod=%ld"
			     " AND LnkInd<%u",
			   Node->Hierarchy.NodCod,
			   Node->SpcItem.Ind);
  }

unsigned Lnk_DB_GetLnkIndAfter (const struct Tre_Node *Node)
  {
   return
   DB_QuerySELECTUnsigned ("can not get the course link after",
			   "SELECT COALESCE(MIN(LnkInd),0)"
			    " FROM crs_links"
			   " WHERE NodCod=%ld"
			     " AND LnkInd>%u",
			   Node->Hierarchy.NodCod,
			   Node->SpcItem.Ind);
  }

/*****************************************************************************/
/******** Get course link code given node code and course link index *********/
/*****************************************************************************/

long Lnk_DB_GetLnkCodFromLnkInd (long NodCod,unsigned LnkInd)
  {
   /***** Trivial check: course link index should be > 0 *****/
   if (LnkInd == 0)
      return -1L;

   /***** Get course link code given node code and course link index *****/
   return DB_QuerySELECTCode ("can not get course link code",
			      "SELECT LnkCod"
			       " FROM crs_links"
			      " WHERE NodCod=%ld"
				" AND LnkInd=%u",
			      NodCod,LnkInd);
  }

/*****************************************************************************/
/*************************** Remove a course link ****************************/
/*****************************************************************************/

void Lnk_DB_RemoveCrsLink (const struct Tre_Node *Node)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryDELETE ("can not remove course link",
		   "DELETE FROM crs_links"
		   " USING crs_links,"
		          "tre_nodes"
		   " WHERE crs_links.LnkCod=%ld"
		     " AND crs_links.NodCod=%ld"
                     " AND crs_links.NodCod=tre_nodes.NodCod"
                     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   Node->SpcItem.Cod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_LINKS]);
  }

/*****************************************************************************/
/*********************** Hide/unhide a node course link **********************/
/*****************************************************************************/

void Lnk_DB_HideOrUnhideCrsLink (const struct Tre_Node *Node,
			         HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char HidVis_YN[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryUPDATE ("can not hide/unhide node course link",
		   "UPDATE crs_links,"
		          "tre_nodes"
		     " SET crs_links.Hidden='%c'"
		   " WHERE crs_links.LnkCod=%ld"
		     " AND crs_links.NodCod=%ld"
		     " AND crs_links.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   HidVis_YN[HiddenOrVisible],
		   Node->SpcItem.Cod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_LINKS]);
  }

/*****************************************************************************/
/********** Lock table to make the exchange of course link atomic ************/
/*****************************************************************************/

void Lnk_DB_LockTableCrsLinks (void)
  {
   DB_Query ("can not lock tables",
	     "LOCK TABLES crs_links WRITE,"
			 "tre_nodes READ");
   DB_SetThereAreLockedTables ();
  }

/*****************************************************************************/
/*********** Update the index of a course link given its code ****************/
/*****************************************************************************/

void Lnk_DB_UpdateLnkInd (const struct Tre_Node *Node,long LnkCod,int LnkInd)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryUPDATE ("can not update index of course link",
		   "UPDATE crs_links,"
		          "tre_nodes"
		     " SET crs_links.LnkInd=%d"
		   " WHERE crs_links.LnkCod=%ld"
		     " AND crs_links.NodCod=%ld"
		     " AND crs_links.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   LnkInd,
		   LnkCod,
		   Node->Hierarchy.NodCod,
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
		   " WHERE crs_links.LnkCod=%ld"
		     " AND crs_links.NodCod=%ld"
		     " AND crs_links.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   Node->Lnk.Title,
		   Node->Lnk.Description,
		   Node->Lnk.WWW,
		   Node->SpcItem.Cod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_LINKS]);
  }
