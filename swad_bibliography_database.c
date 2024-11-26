// swad_bibliography_database.c: bibliographic references, operations with database

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

long Bib_DB_CreateBibRef (const struct Tre_Node *Node)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new bibliographic reference",
				"INSERT INTO crs_bibliography"
				" (NodCod,BibInd,Hidden,Title,Description,WWW)"
				" SELECT %ld,COALESCE(MAX(t2.BibInd),0)+1,'N','%s','%s','%s'"
				  " FROM crs_bibliography AS t2"
				 " WHERE t2.NodCod=%ld",
				Node->Hierarchy.NodCod,
				Node->Lnk.Title,
				Node->Lnk.Description,
				Node->Lnk.WWW,
				Node->Hierarchy.NodCod);
  }

/*****************************************************************************/
/******** Get list of node bibliographic references from database ************/
/*****************************************************************************/

unsigned Bib_DB_GetListBibRefs (MYSQL_RES **mysql_res,long NodCod,
                                bool ShowHiddenBibRefs)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];
   static const char *HiddenSubQuery[2] =
     {
      [false] = " AND crs_bibliography.Hidden='N'",
      [true ] = "",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get node bibliographic references",
		   "SELECT crs_bibliography.NodCod,"		// row[0]
			  "crs_bibliography.BibCod,"		// row[1]
                          "crs_bibliography.BibInd,"		// row[2]
			  "crs_bibliography.Hidden,"		// row[3]
			  "crs_bibliography.Title,"		// row[4]
			  "crs_bibliography.Description,"	// row[5]
			  "crs_bibliography.WWW"		// row[6]
		    " FROM crs_bibliography,"
		          "tre_nodes"
		   " WHERE crs_bibliography.NodCod=%ld"
		       "%s"
		     " AND crs_bibliography.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'"		// Extra check
		" ORDER BY crs_bibliography.BibInd",
		   NodCod,
		   HiddenSubQuery[ShowHiddenBibRefs],
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_LINKS]);
  }

/*****************************************************************************/
/************* Get bibliographic reference data using its code ***************/
/*****************************************************************************/

unsigned Bib_DB_GetBibRefDataByCod (MYSQL_RES **mysql_res,long BibCod)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get node bibliographic reference data",
		   "SELECT crs_bibliography.NodCod,"		// row[0]
			  "crs_bibliography.BibCod,"		// row[1]
                          "crs_bibliography.BibInd,"		// row[2]
			  "crs_bibliography.Hidden,"		// row[3]
			  "crs_bibliography.Title,"		// row[4]
			  "crs_bibliography.Description,"	// row[5]
			  "crs_bibliography.WWW"		// row[6]
		    " FROM crs_bibliography,"
		          "tre_nodes"
		   " WHERE crs_bibliography.BibCod=%ld"
		     " AND crs_bibliography.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   BibCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_LINKS]);
  }

/*****************************************************************************/
/****** Get the bibliographic reference index before/after a given one *******/
/*****************************************************************************/

unsigned Bib_DB_GetBibIndBefore (const struct Tre_Node *Node)
  {
   return
   DB_QuerySELECTUnsigned ("can not get the bibliographic reference before",
			   "SELECT COALESCE(MAX(BibInd),0)"
			    " FROM crs_bibliography"
			   " WHERE NodCod=%ld"
			     " AND BibInd<%u",
			   Node->Hierarchy.NodCod,
			   Node->SpcItem.Ind);
  }

unsigned Bib_DB_GetBibIndAfter (const struct Tre_Node *Node)
  {
   return
   DB_QuerySELECTUnsigned ("can not get the bibliographic reference after",
			   "SELECT COALESCE(MIN(BibInd),0)"
			    " FROM crs_bibliography"
			   " WHERE NodCod=%ld"
			     " AND BibInd>%u",
			   Node->Hierarchy.NodCod,
			   Node->SpcItem.Ind);
  }

/*****************************************************************************/
/************ Get bibliographic reference code                  **************/
/************ given node code and bibliographic reference index **************/
/*****************************************************************************/

long Bib_DB_GetBibCodFromBibInd (long NodCod,unsigned BibInd)
  {
   /***** Trivial check: bibliographic reference index should be > 0 *****/
   if (BibInd == 0)
      return -1L;

   /***** Get bibliographic reference code
          given node code and bibliographic reference index *****/
   return DB_QuerySELECTCode ("can not get bibliographic reference code",
			      "SELECT BibCod"
			       " FROM crs_bibliography"
			      " WHERE NodCod=%ld"
				" AND BibInd=%u",
			      NodCod,BibInd);
  }

/*****************************************************************************/
/********************* Remove a bibliographic reference **********************/
/*****************************************************************************/

void Bib_DB_RemoveBibRef (const struct Tre_Node *Node)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryDELETE ("can not remove bibliographic reference",
		   "DELETE FROM crs_bibliography"
		   " USING crs_bibliography,"
		          "tre_nodes"
		   " WHERE crs_bibliography.BibCod=%ld"
		     " AND crs_bibliography.NodCod=%ld"
                     " AND crs_bibliography.NodCod=tre_nodes.NodCod"
                     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   Node->SpcItem.Cod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_LINKS]);
  }

/*****************************************************************************/
/***************** Hide/unhide a node bibliographic reference ****************/
/*****************************************************************************/

void Bib_DB_HideOrUnhideBibRef (const struct Tre_Node *Node,
			         HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char HidVis_YN[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryUPDATE ("can not hide/unhide node bibliographic reference",
		   "UPDATE crs_bibliography,"
		          "tre_nodes"
		     " SET crs_bibliography.Hidden='%c'"
		   " WHERE crs_bibliography.BibCod=%ld"
		     " AND crs_bibliography.NodCod=%ld"
		     " AND crs_bibliography.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   HidVis_YN[HiddenOrVisible],
		   Node->SpcItem.Cod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_LINKS]);
  }

/*****************************************************************************/
/**** Lock table to make the exchange of bibliographic reference atomic ******/
/*****************************************************************************/

void Bib_DB_LockTableBibRefs (void)
  {
   DB_Query ("can not lock tables",
	     "LOCK TABLES crs_bibliography WRITE,"
			 "tre_nodes READ");
   DB_SetThereAreLockedTables ();
  }

/*****************************************************************************/
/******** Update the index of a bibliographic reference given its code *******/
/*****************************************************************************/

void Bib_DB_UpdateBibInd (const struct Tre_Node *Node,long BibCod,int BibInd)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryUPDATE ("can not update index of bibliographic reference",
		   "UPDATE crs_bibliography,"
		          "tre_nodes"
		     " SET crs_bibliography.BibInd=%d"
		   " WHERE crs_bibliography.BibCod=%ld"
		     " AND crs_bibliography.NodCod=%ld"
		     " AND crs_bibliography.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   BibInd,
		   BibCod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_LINKS]);
  }

/*****************************************************************************/
/*************** Update bibliographic reference given its code ***************/
/*****************************************************************************/

void Bib_DB_UpdateBibRef (const struct Tre_Node *Node)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryUPDATE ("can not update bibliographic reference",
		   "UPDATE crs_bibliography,"
		          "tre_nodes"
		     " SET crs_bibliography.Title='%s',"
		          "crs_bibliography.Description='%s',"
		          "crs_bibliography.WWW='%s'"
		   " WHERE crs_bibliography.BibCod=%ld"
		     " AND crs_bibliography.NodCod=%ld"
		     " AND crs_bibliography.NodCod=tre_nodes.NodCod"
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
