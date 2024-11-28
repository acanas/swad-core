// swad_tree_database.c: course tree, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Ca�as Vargas

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
#include "swad_tree_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

const char *Tre_DB_Types[Inf_NUM_TYPES] =
  {
   [Inf_UNKNOWN_TYPE	] = NULL,
   [Inf_INFORMATION	] = "inf",
   [Inf_PROGRAM		] = "prg",
   [Inf_TEACH_GUIDE	] = "gui",
   [Inf_SYLLABUS_LEC	] = "lec",
   [Inf_SYLLABUS_PRA	] = "pra",
   [Inf_BIBLIOGRAPHY	] = "bib",
   [Inf_FAQ		] = "faq",
   [Inf_LINKS		] = "lnk",
   [Inf_ASSESSMENT	] = "ass",
  };

/*****************************************************************************/
/******************* Create a new tree node into database ********************/
/*****************************************************************************/

long Tre_DB_InsertNode (const struct Tre_Node *Node,const char *Txt)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new tree node",
			        "INSERT INTO tre_nodes"
			        " (CrsCod,Type,NodInd,Level,UsrCod,"
				  "StartTime,EndTime,"
				  "Title,Txt)"
			        " VALUES"
			        " (%ld,'%s',%u,%u,%ld,"
				  "FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
				  "'%s','%s')",
			        Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				Tre_DB_Types[Node->InfoType],
			        Node->Hierarchy.NodInd,
			        Node->Hierarchy.Level,
			        Gbl.Usrs.Me.UsrDat.UsrCod,
			        Node->TimeUTC[Dat_STR_TIME],
			        Node->TimeUTC[Dat_END_TIME],
			        Node->Title,
			        Txt);
  }

/*****************************************************************************/
/********************** Update an existing tree node *************************/
/*****************************************************************************/

void Tre_DB_UpdateNode (const struct Tre_Node *Node,const char *Txt)
  {
   DB_QueryUPDATE ("can not update tree node",
		   "UPDATE tre_nodes"
		     " SET StartTime=FROM_UNIXTIME(%ld),"
		          "EndTime=FROM_UNIXTIME(%ld),"
		          "Title='%s',"
		          "Txt='%s'"
		   " WHERE NodCod=%ld"
		     " AND CrsCod=%ld"	// Extra check
		     " AND Type='%s'",	// Extra check
                   Node->TimeUTC[Dat_STR_TIME],
                   Node->TimeUTC[Dat_END_TIME],
                   Node->Title,
                   Txt,
                   Node->Hierarchy.NodCod,
                   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Node->InfoType]);
  }

/*****************************************************************************/
/*************************** Hide/unhide a tree node *************************/
/*****************************************************************************/

void Tre_DB_HideOrUnhideNode (const struct Tre_Node *Node,
			      HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char HidVis_YN[HidVis_NUM_HIDDEN_VISIBLE];

   DB_QueryUPDATE ("can not hide/unhide tree node",
		   "UPDATE tre_nodes"
		     " SET Hidden='%c'"
		   " WHERE NodCod=%ld"
		     " AND CrsCod=%ld"	// Extra check
		     " AND Type='%s'",	// Extra check
		   HidVis_YN[HiddenOrVisible],
		   Node->Hierarchy.NodCod,
                   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Node->InfoType]);
  }

/*****************************************************************************/
/********************* Change index of a node in a tree **********************/
/*****************************************************************************/

void Tre_DB_UpdateIndexRange (Inf_Type_t InfoType,long Diff,long Begin,long End)
  {
   DB_QueryUPDATE ("can not exchange indexes of tree nodes",
		   "UPDATE tre_nodes"
		     " SET NodInd=-NodInd+%ld"
		   " WHERE CrsCod=%ld"
		     " AND Type='%s'"
		     " AND NodInd>=%ld"
		     " AND NodInd<=%ld",
		   Diff,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[InfoType],
		   Begin,
		   End);
  }

/*****************************************************************************/
/************ Lock table to make the exchange of nodes atomic ****************/
/*****************************************************************************/

void Tre_DB_LockTableNodes (void)
  {
   DB_Query ("can not lock table",
	     "LOCK TABLES tre_nodes WRITE");
   DB_SetThereAreLockedTables ();
  }

/*****************************************************************************/
/************ Move down all indexes of after last child of parent ************/
/*****************************************************************************/

void Tre_DB_MoveDownNodes (Inf_Type_t InfoType,unsigned Index)
  {
   DB_QueryUPDATE ("can not move down tree nodes",
		   "UPDATE tre_nodes"
		     " SET NodInd=NodInd+1"
		   " WHERE CrsCod=%ld"
		     " AND Type='%s'"
		     " AND NodInd>=%u"
		" ORDER BY NodInd DESC",	// Necessary to not create duplicate key (CrsCod,NodInd)
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[InfoType],
		   Index);
  }

/*****************************************************************************/
/**************** Move node and its children to left or right ****************/
/*****************************************************************************/

void Tre_DB_MoveLeftRightNodeRange (Inf_Type_t InfoType,
				    const struct Tre_NodeRange *ToMove,
                                    Tre_MoveLeftRight_t LeftRight)
  {
   static const char IncDec[Tre_NUM_MOVEMENTS_LEFT_RIGHT] =
     {
      [Tre_MOVE_LEFT ] = '-',
      [Tre_MOVE_RIGHT] = '+',
     };

   DB_QueryUPDATE ("can not move tree nodes",
		   "UPDATE tre_nodes"
		     " SET Level=Level%c1"
		   " WHERE CrsCod=%ld"
		     " AND Type='%s'"
		     " AND NodInd>=%u"
		     " AND NodInd<=%u",
		   IncDec[LeftRight],
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[InfoType],
		   ToMove->Begin,
		   ToMove->End);
  }

/*****************************************************************************/
/******************** Get list of tree nodes from database *******************/
/*****************************************************************************/

unsigned Tre_DB_GetListNodes (Inf_Type_t InfoType,MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get tree nodes",
		   "SELECT NodCod,"	// row[0]
			  "NodInd,"	// row[1]
			  "Level,"	// row[2]
			  "Hidden"	// row[3]
		    " FROM tre_nodes"
		   " WHERE CrsCod=%ld"
		     " AND Type='%s'"
		" ORDER BY NodInd",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[InfoType]);
  }

/*****************************************************************************/
/******************** Get tree node data using its code **********************/
/*****************************************************************************/

unsigned Tre_DB_GetNodeDataByCod (const struct Tre_Node *Node,MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get tree node data",
		   "SELECT NodCod,"					// row[0]
			  "NodInd,"					// row[1]
			  "Level,"					// row[2]
			  "Hidden,"					// row[3]
			  "UsrCod,"					// row[4]
			  "UNIX_TIMESTAMP(StartTime),"			// row[5]
			  "UNIX_TIMESTAMP(EndTime),"			// row[6]
			  "NOW() BETWEEN StartTime AND EndTime,"	// row[7]
			  "Title"					// row[8]
		    " FROM tre_nodes"
		   " WHERE NodCod=%ld"
		     " AND CrsCod=%ld"	// Extra check
		     " AND Type='%s'",	// Extra check
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Node->InfoType]);
  }

/*****************************************************************************/
/********************* Get tree node text from database **********************/
/*****************************************************************************/

void Tre_DB_GetNodeTxt (const struct Tre_Node *Node,
			char Txt[Cns_MAX_BYTES_TEXT + 1])
  {
   DB_QuerySELECTString (Txt,Cns_MAX_BYTES_TEXT,"can not get tree node text",
		         "SELECT Txt"
			  " FROM tre_nodes"
		         " WHERE NodCod=%ld"
			   " AND CrsCod=%ld"	// Extra check
			   " AND Type='%s'",	// Extra check
		         Node->Hierarchy.NodCod,
		         Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		         Tre_DB_Types[Node->InfoType]);
  }

/*****************************************************************************/
/******************* Get number of courses with tree nodes *******************/
/*****************************************************************************/
// Returns the number of courses with tree nodes
// in this location (all the platform, current degree or current course)

unsigned Tre_DB_GetNumCoursesWithNodes (Inf_Type_t InfoType,Hie_Level_t Level)
  {
   /***** Get number of courses with tree nodes from database *****/
   switch (Level)
     {
      case Hie_SYS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with tree nodes",
                        "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM tre_nodes"
			" WHERE CrsCod>0"
			  " AND Type='%s'",
			Tre_DB_Types[InfoType]);
       case Hie_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with tree nodes",
                         "SELECT COUNT(DISTINCT tre_nodes.CrsCod)"
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "tre_nodes"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=tre_nodes.CrsCod"
			   " AND tre_nodes.Type='%s'",
                        Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			Tre_DB_Types[InfoType]);
       case Hie_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with tree nodes",
                         "SELECT COUNT(DISTINCT tre_nodes.CrsCod)"
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "tre_nodes"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=tre_nodes.CrsCod"
			   " AND tre_nodes.Type='%s'",
                        Gbl.Hierarchy.Node[Hie_INS].HieCod,
			Tre_DB_Types[InfoType]);
      case Hie_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with tree nodes",
                         "SELECT COUNT(DISTINCT tre_nodes.CrsCod)"
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "tre_nodes"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=tre_nodes.CrsCod"
			   " AND tre_nodes.Type='%s'",
                        Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			Tre_DB_Types[InfoType]);
      case Hie_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with tree nodes",
                         "SELECT COUNT(DISTINCT tre_nodes.CrsCod)"
			  " FROM crs_courses,"
			        "tre_nodes"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=tre_nodes.CrsCod"
			   " AND tre_nodes.Type='%s'",
                        Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			Tre_DB_Types[InfoType]);
      case Hie_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with tree nodes",
                         "SELECT COUNT(DISTINCT CrsCod)"
			  " FROM tre_nodes"
			 " WHERE CrsCod=%ld"
			   " AND Type='%s'",
                        Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			Tre_DB_Types[InfoType]);
      default:
	 return 0;
     }
  }

/*****************************************************************************/
/************************** Get number of tree nodes *************************/
/*****************************************************************************/
// Returns the number of tree nodes in a hierarchy scope

unsigned Tre_DB_GetNumNodes (Inf_Type_t InfoType,Hie_Level_t Level)
  {
   switch (Level)
     {
      case Hie_SYS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of tree nodes",
                         "SELECT COUNT(*)"
			  " FROM tre_nodes"
			 " WHERE CrsCod>0"
			   " AND Type='%s'",
			Tre_DB_Types[InfoType]);
      case Hie_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of tree nodes",
                         "SELECT COUNT(*)"
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "tre_nodes"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=tre_nodes.CrsCod"
			   " AND tre_nodes.Type='%s'",
                        Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			Tre_DB_Types[InfoType]);
      case Hie_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of tree nodes",
                         "SELECT COUNT(*)"
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "tre_nodes"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=tre_nodes.CrsCod"
			   " AND tre_nodes.Type='%s'",
                        Gbl.Hierarchy.Node[Hie_INS].HieCod,
			Tre_DB_Types[InfoType]);
      case Hie_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of tree nodes",
                         "SELECT COUNT(*)"
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "tre_nodes"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=tre_nodes.CrsCod"
			   " AND tre_nodes.Type='%s'",
                        Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			Tre_DB_Types[InfoType]);
      case Hie_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of tree nodes",
                         "SELECT COUNT(*)"
			  " FROM crs_courses,"
			        "tre_nodes"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=tre_nodes.CrsCod"
			   " AND tre_nodes.Type='%s'",
                        Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			Tre_DB_Types[InfoType]);
      case Hie_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of tree nodes",
                         "SELECT COUNT(*)"
			  " FROM tre_nodes"
			 " WHERE CrsCod=%ld"
			   " AND Type='%s'",
                        Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			Tre_DB_Types[InfoType]);
      default:
         Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/******************** Remove a tree node and its children ********************/
/*****************************************************************************/

void Tre_DB_RemoveNodeRange (Inf_Type_t InfoType,
			     const struct Tre_NodeRange *ToRemove)
  {
   DB_QueryDELETE ("can not remove tree node range",
		   "DELETE FROM tre_nodes"
		   " WHERE CrsCod=%ld"
		     " AND Type='%s'"
		     " AND NodInd>=%u"
		     " AND NodInd<=%u",
                   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
                   Tre_DB_Types[InfoType],
		   ToRemove->Begin,
		   ToRemove->End);
  }

/*****************************************************************************/
/********************* Remove all tree nodes in a course *********************/
/*****************************************************************************/

void Tre_DB_RemoveCrsNodes (long CrsCod)
  {
   DB_QueryDELETE ("can not remove all tree nodes in a course",
		   "DELETE FROM tre_nodes"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/************************ Insert node in expanded nodes **********************/
/*****************************************************************************/

void Tre_DB_InsertNodeInExpandedNodes (long NodCod)
  {
   DB_QueryREPLACE ("can not expand tree node",
		   "REPLACE INTO tre_expanded"
		   " (UsrCod,NodCod,ClickTime)"
		   " VALUES"
		   " (%ld,%ld,NOW())",
	           Gbl.Usrs.Me.UsrDat.UsrCod,
	           NodCod);
  }

/*****************************************************************************/
/********************** Check if a tree node is expanded *********************/
/*****************************************************************************/

ConExp_ContractedOrExpanded_t Tre_DB_GetIfContractedOrExpandedNode (long NodCod)
  {
   return
   DB_QueryEXISTS ("can not check if a tree node is expanded",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM tre_expanded"
		    " WHERE UsrCod=%ld"
		      " AND NodCod=%ld)",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   NodCod) ? ConExp_EXPANDED :
		             ConExp_CONTRACTED;
  }

/*****************************************************************************/
/********************** Remove node from expanded nodes **********************/
/*****************************************************************************/

void Tre_DB_RemoveNodeFromExpandedNodes (long NodCod)
  {
   DB_QueryDELETE ("can not contract tree node",
		   "DELETE FROM tre_expanded"
		   " WHERE UsrCod=%ld"
		     " AND NodCod=%ld",
	           Gbl.Usrs.Me.UsrDat.UsrCod,
		   NodCod);
  }