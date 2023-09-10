// swad_program_database.c: course program, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
#include "swad_program_resource.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************** Create a new program item into database *******************/
/*****************************************************************************/

long Prg_DB_InsertItem (const struct Prg_Item *Item,const char *Txt)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new program item",
			        "INSERT INTO prg_items"
			        " (CrsCod,ItmInd,Level,UsrCod,"
				  "StartTime,EndTime,"
				  "Title,Txt)"
			        " VALUES"
			        " (%ld,%u,%u,%ld,"
				  "FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
				  "'%s','%s')",
			        Gbl.Hierarchy.Crs.CrsCod,
			        Item->Hierarchy.ItmInd,
			        Item->Hierarchy.Level,
			        Gbl.Usrs.Me.UsrDat.UsrCod,
			        Item->TimeUTC[Dat_STR_TIME],
			        Item->TimeUTC[Dat_END_TIME],
			        Item->Title,
			        Txt);
  }

/*****************************************************************************/
/******************** Update an existing program item ************************/
/*****************************************************************************/

void Prg_DB_UpdateItem (const struct Prg_Item *Item,const char *Txt)
  {
   DB_QueryUPDATE ("can not update program item",
		   "UPDATE prg_items"
		     " SET StartTime=FROM_UNIXTIME(%ld),"
		          "EndTime=FROM_UNIXTIME(%ld),"
		          "Title='%s',"
		          "Txt='%s'"
		   " WHERE ItmCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
                   Item->TimeUTC[Dat_STR_TIME],
                   Item->TimeUTC[Dat_END_TIME],
                   Item->Title,
                   Txt,
                   Item->Hierarchy.ItmCod,
                   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/************************* Hide/unhide a program item ************************/
/*****************************************************************************/

void Prg_DB_HideOrUnhideItem (long ItmCod,
			      HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   DB_QueryUPDATE ("can not hide/unhide program item",
		   "UPDATE prg_items"
		     " SET Hidden='%c'"
		   " WHERE ItmCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
		   HiddenOrVisible == HidVis_HIDDEN ? 'Y' :
						      'N',
		   ItmCod,
                   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/********************* Change index of a set in an exam **********************/
/*****************************************************************************/

void Prg_DB_UpdateIndexRange (long Diff,long Begin,long End)
  {
   DB_QueryUPDATE ("can not exchange indexes of items",
		   "UPDATE prg_items"
		     " SET ItmInd=-ItmInd+%ld"
		   " WHERE CrsCod=%ld"
		     " AND ItmInd>=%ld"
		     " AND ItmInd<=%ld",
		   Diff,
		   Gbl.Hierarchy.Crs.CrsCod,
		   Begin,
		   End);
  }

/*****************************************************************************/
/************ Lock table to make the exchange of items atomic ****************/
/*****************************************************************************/

void Prg_DB_LockTableItems (void)
  {
   DB_Query ("can not lock table",
	     "LOCK TABLES prg_items WRITE");
   DB_SetThereAreLockedTables ();
  }

/*****************************************************************************/
/************ Move down all indexes of after last child of parent ************/
/*****************************************************************************/

void Prg_DB_MoveDownItems (unsigned Index)
  {
   DB_QueryUPDATE ("can not move down items",
		   "UPDATE prg_items"
		     " SET ItmInd=ItmInd+1"
		   " WHERE CrsCod=%ld"
		     " AND ItmInd>=%u"
		   " ORDER BY ItmInd DESC",	// Necessary to not create duplicate key (CrsCod,ItmInd)
		   Gbl.Hierarchy.Crs.CrsCod,
		   Index);
  }

/*****************************************************************************/
/**************** Move item and its children to left or right ****************/
/*****************************************************************************/

void Prg_DB_MoveLeftRightItemRange (const struct Prg_ItemRange *ToMove,
                                    Prg_MoveLeftRight_t LeftRight)
  {
   static const char IncDec[Prg_NUM_MOVEMENTS_LEFT_RIGHT] =
     {
      [Prg_MOVE_LEFT ] = '-',
      [Prg_MOVE_RIGHT] = '+',
     };

   DB_QueryUPDATE ("can not move items",
		   "UPDATE prg_items"
		     " SET Level=Level%c1"
		   " WHERE CrsCod=%ld"
		     " AND ItmInd>=%u"
		     " AND ItmInd<=%u",
		   IncDec[LeftRight],
		   Gbl.Hierarchy.Crs.CrsCod,
		   ToMove->Begin,
		   ToMove->End);
  }

/*****************************************************************************/
/******************* Get list of program items from database *****************/
/*****************************************************************************/

unsigned Prg_DB_GetListItems (MYSQL_RES **mysql_res)
  {
   static const char *HiddenSubQuery[Rol_NUM_ROLES] =
     {
      [Rol_UNK    ] = " AND Hidden='N'",
      [Rol_GST    ] = " AND Hidden='N'",
      [Rol_USR    ] = " AND Hidden='N'",
      [Rol_STD    ] = " AND Hidden='N'",
      [Rol_NET    ] = " AND Hidden='N'",
      [Rol_TCH    ] = "",
      [Rol_DEG_ADM] = " AND Hidden='N'",
      [Rol_CTR_ADM] = " AND Hidden='N'",
      [Rol_INS_ADM] = " AND Hidden='N'",
      [Rol_SYS_ADM] = "",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get program items",
		   "SELECT ItmCod,"	// row[0]
			  "ItmInd,"	// row[1]
			  "Level,"	// row[2]
			  "Hidden"	// row[3]
		    " FROM prg_items"
		   " WHERE CrsCod=%ld"
		       "%s"
		   " ORDER BY ItmInd",
		   Gbl.Hierarchy.Crs.CrsCod,
		   HiddenSubQuery[Gbl.Usrs.Me.Role.Logged]);
  }

/*****************************************************************************/
/****************** Get program item data using its code *********************/
/*****************************************************************************/

unsigned Prg_DB_GetItemDataByCod (MYSQL_RES **mysql_res,long ItmCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get program item data",
		   "SELECT ItmCod,"					// row[0]
			  "ItmInd,"					// row[1]
			  "Level,"					// row[2]
			  "Hidden,"					// row[3]
			  "UsrCod,"					// row[4]
			  "UNIX_TIMESTAMP(StartTime),"			// row[5]
			  "UNIX_TIMESTAMP(EndTime),"			// row[6]
			  "NOW() BETWEEN StartTime AND EndTime,"	// row[7]
			  "Title"					// row[8]
		    " FROM prg_items"
		   " WHERE ItmCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
		   ItmCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/******************* Get program item text from database *********************/
/*****************************************************************************/

void Prg_DB_GetItemTxt (long ItmCod,char Txt[Cns_MAX_BYTES_TEXT + 1])
  {
   DB_QuerySELECTString (Txt,Cns_MAX_BYTES_TEXT,"can not get program item text",
		         "SELECT Txt"
			  " FROM prg_items"
		         " WHERE ItmCod=%ld"
			   " AND CrsCod=%ld",	// Extra check
		         ItmCod,
		         Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/****************** Get number of courses with program items *****************/
/*****************************************************************************/
// Returns the number of courses with program items
// in this location (all the platform, current degree or current course)

unsigned Prg_DB_GetNumCoursesWithItems (HieLvl_Level_t Level)
  {
   /***** Get number of courses with program items from database *****/
   switch (Level)
     {
      case HieLvl_SYS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with program items",
                        "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM prg_items"
			" WHERE CrsCod>0");
       case HieLvl_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT prg_items.CrsCod)"
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "prg_items"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Cty.CtyCod);
       case HieLvl_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT prg_items.CrsCod)"
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "prg_items"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
      case HieLvl_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT prg_items.CrsCod)"
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "prg_items"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
      case HieLvl_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT prg_items.CrsCod)"
			  " FROM crs_courses,"
			        "prg_items"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Deg.DegCod);
      case HieLvl_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT CrsCod)"
			  " FROM prg_items"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
      default:
	 return 0;
     }
  }

/*****************************************************************************/
/************************ Get number of program items ************************/
/*****************************************************************************/
// Returns the number of program items in a hierarchy scope

unsigned Prg_DB_GetNumItems (HieLvl_Level_t Level)
  {
   switch (Level)
     {
      case HieLvl_SYS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of program items",
                         "SELECT COUNT(*)"
			  " FROM prg_items"
			 " WHERE CrsCod>0");
      case HieLvl_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of program items",
                         "SELECT COUNT(*)"
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "prg_items"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Cty.CtyCod);
      case HieLvl_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of program items",
                         "SELECT COUNT(*)"
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "prg_items"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
      case HieLvl_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of program items",
                         "SELECT COUNT(*)"
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "prg_items"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
      case HieLvl_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of program items",
                         "SELECT COUNT(*)"
			  " FROM crs_courses,"
			        "prg_items"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Deg.DegCod);
      case HieLvl_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of program items",
                         "SELECT COUNT(*)"
			  " FROM prg_items"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
      default:
         Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/******************* Remove a program item and its children ******************/
/*****************************************************************************/

void Prg_DB_RemoveItemRange (const struct Prg_ItemRange *ToRemove)
  {
   DB_QueryDELETE ("can not remove program item",
		   "DELETE FROM prg_items"
		   " WHERE CrsCod=%ld"
		     " AND ItmInd>=%u"
		     " AND ItmInd<=%u",
                   Gbl.Hierarchy.Crs.CrsCod,
		   ToRemove->Begin,
		   ToRemove->End);
  }

/*****************************************************************************/
/******************* Remove all program items in a course ********************/
/*****************************************************************************/

void Prg_DB_RemoveCrsItems (long CrsCod)
  {
   DB_QueryDELETE ("can not remove all program items in a course",
		   "DELETE FROM prg_items"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/************************** Create a new resource ****************************/
/*****************************************************************************/

long Prg_DB_CreateResource (const struct Prg_Item *Item)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new resource",
				"INSERT INTO prg_resources"
				" (ItmCod,RscInd,Hidden,Title)"
				" SELECT %ld,COALESCE(MAX(t2.RscInd),0)+1,'N','%s'"
				  " FROM prg_resources AS t2"
				 " WHERE t2.ItmCod=%ld",
				Item->Hierarchy.ItmCod,
				Item->Resource.Title,
				Item->Hierarchy.ItmCod);
  }

/*****************************************************************************/
/**************************** Update resource title **************************/
/*****************************************************************************/

void Prg_DB_UpdateResourceTitle (long ItmCod,long RscCod,
                                 const char NewTitle[Rsc_MAX_BYTES_RESOURCE_TITLE + 1])
  {
   DB_QueryUPDATE ("can not update the title of a resource",
		   "UPDATE prg_resources,"
		          "prg_items"
		     " SET prg_resources.Title='%s'"
		   " WHERE prg_resources.RscCod=%ld"
		     " AND prg_resources.ItmCod=%ld"
		     " AND prg_resources.ItmCod=prg_items.ItmCod"
		     " AND prg_items.CrsCod=%ld",
	           NewTitle,
	           RscCod,
	           ItmCod,
	           Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/****************** Get list of item resources from database *****************/
/*****************************************************************************/

unsigned Prg_DB_GetListResources (MYSQL_RES **mysql_res,long ItmCod,
                                  bool ShowHiddenResources)
  {
   static const char *HiddenSubQuery[2] =
     {
      [false] = " AND prg_resources.Hidden='N'",
      [true ] = "",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get item resources",
		   "SELECT prg_resources.ItmCod,"	// row[0]
			  "prg_resources.RscCod,"	// row[1]
                          "prg_resources.RscInd,"	// row[2]
			  "prg_resources.Hidden,"	// row[3]
			  "prg_resources.Type,"		// row[4]
			  "prg_resources.Cod,"		// row[5]
			  "prg_resources.Title"		// row[6]
		    " FROM prg_resources,"
		          "prg_items"
		   " WHERE prg_resources.ItmCod=%ld"
		       "%s"
		     " AND prg_resources.ItmCod=prg_items.ItmCod"
		     " AND prg_items.CrsCod=%ld"	// Extra check
		   " ORDER BY prg_resources.RscInd",
		   ItmCod,
		   HiddenSubQuery[ShowHiddenResources],
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/****************** Get item resource data using its code ********************/
/*****************************************************************************/

unsigned Prg_DB_GetResourceDataByCod (MYSQL_RES **mysql_res,long RscCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get item resource data",
		   "SELECT prg_resources.ItmCod,"	// row[0]
			  "prg_resources.RscCod,"	// row[1]
                          "prg_resources.RscInd,"	// row[2]
			  "prg_resources.Hidden,"	// row[3]
			  "prg_resources.Type,"		// row[4]
			  "prg_resources.Cod,"		// row[5]
			  "prg_resources.Title"		// row[6]
		    " FROM prg_resources,"
		          "prg_items"
		   " WHERE prg_resources.RscCod=%ld"
		     " AND prg_resources.ItmCod=prg_items.ItmCod"
		     " AND prg_items.CrsCod=%ld",	// Extra check
		   RscCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/************* Get the resource index before/after a given one ***************/
/*****************************************************************************/

unsigned Prg_DB_GetRscIndBefore (long ItmCod,unsigned RscInd)
  {
   return
   DB_QuerySELECTUnsigned ("can not get the resource before",
			   "SELECT COALESCE(MAX(RscInd),0)"
			    " FROM prg_resources"
			   " WHERE ItmCod=%ld"
			     " AND RscInd<%u",
			   ItmCod,RscInd);
  }

unsigned Prg_DB_GetRscIndAfter (long ItmCod,unsigned RscInd)
  {
   return
   DB_QuerySELECTUnsigned ("can not get the resource after",
			   "SELECT COALESCE(MIN(RscInd),0)"
			    " FROM prg_resources"
			   " WHERE ItmCod=%ld"
			     " AND RscInd>%u",
			   ItmCod,RscInd);
  }

/*****************************************************************************/
/*********** Get resource code given item code and resource index ************/
/*****************************************************************************/

long Prg_DB_GetRscCodFromRscInd (long ItmCod,unsigned RscInd)
  {
   /***** Trivial check: resource index should be > 0 *****/
   if (RscInd == 0)
      return -1L;

   /***** Get resource code given item code and resource index *****/
   return DB_QuerySELECTCode ("can not get resource code",
			      "SELECT RscCod"
			       " FROM prg_resources"
			      " WHERE ItmCod=%ld"
				" AND RscInd=%u",
			      ItmCod,RscInd);
  }

/*****************************************************************************/
/************************** Remove an item resource **************************/
/*****************************************************************************/

void Prg_DB_RemoveResource (const struct Prg_Item *Item)
  {
   DB_QueryDELETE ("can not remove item resource",
		   "DELETE FROM prg_resources"
		   " USING prg_resources,"
		          "prg_items"
		   " WHERE prg_resources.RscCod=%ld"
		     " AND prg_resources.ItmCod=%ld"
                     " AND prg_resources.ItmCod=prg_items.ItmCod"
                     " AND prg_items.CrsCod=%ld",	// Extra check
		   Item->Resource.Hierarchy.RscCod,
		   Item->Hierarchy.ItmCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/************************ Hide/unhide an item resource ***********************/
/*****************************************************************************/

void Prg_DB_HideOrUnhideResource (long RscCod,
				  HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   DB_QueryUPDATE ("can not hide/unhide item resource",
		   "UPDATE prg_resources"
		     " SET Hidden='%c'"
		   " WHERE RscCod=%ld",
		   HiddenOrVisible == HidVis_HIDDEN ? 'Y' :
						      'N',
		   RscCod);
  }

/*****************************************************************************/
/********** Lock table to make the exchange of resources atomic **************/
/*****************************************************************************/

void Prg_DB_LockTableResources (void)
  {
   DB_Query ("can not lock table",
	     "LOCK TABLES prg_resources WRITE");
   DB_SetThereAreLockedTables ();
  }

/*****************************************************************************/
/************* Update the index of a resource given its code *****************/
/*****************************************************************************/

void Prg_DB_UpdateRscInd (long RscCod,int RscInd)
  {
   DB_QueryUPDATE ("can not update index of resource",
		   "UPDATE prg_resources"
		     " SET RscInd=%d"
		   " WHERE RscCod=%ld",
		   RscInd,
		   RscCod);
  }

/*****************************************************************************/
/************* Update the link of a resource given its code *****************/
/*****************************************************************************/

void Prg_DB_UpdateRscLink (const struct Prg_Item *Item)
  {
   extern const char *Rsc_ResourceTypesDB[Rsc_NUM_TYPES];

   DB_QueryUPDATE ("can not update link of resource",
		   "UPDATE prg_resources"
		     " SET Type='%s',"
		          "Cod=%ld"
		   " WHERE RscCod=%ld",
		   Rsc_ResourceTypesDB[Item->Resource.Link.Type],
		   Item->Resource.Link.Cod,
		   Item->Resource.Hierarchy.RscCod);
  }

/*****************************************************************************/
/************************ Insert item in expanded items **********************/
/*****************************************************************************/

void Prg_DB_InsertItemInExpandedItems (long ItmCod)
  {
   DB_QueryREPLACE ("can not expand item",
		   "REPLACE INTO prg_expanded"
		   " (UsrCod,ItmCod,ClickTime)"
		   " VALUES"
		   " (%ld,%ld,NOW())",
	           Gbl.Usrs.Me.UsrDat.UsrCod,
	           ItmCod);
  }

/*****************************************************************************/
/******************** Check if a program item is expanded ********************/
/*****************************************************************************/

bool Prg_DB_GetIfExpandedItem (long ItmCod)
  {
   return
   DB_QueryEXISTS ("can not check if an item is expanded",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM prg_expanded"
		    " WHERE UsrCod=%ld"
		      " AND ItmCod=%ld)",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   ItmCod);
  }

/*****************************************************************************/
/********************** Remove item from expanded items **********************/
/*****************************************************************************/

void Prg_DB_RemoveItemFromExpandedItems (long ItmCod)
  {
   DB_QueryDELETE ("can not contract item",
		   "DELETE FROM prg_expanded"
		   " WHERE UsrCod=%ld"
		     " AND ItmCod=%ld",
	           Gbl.Usrs.Me.UsrDat.UsrCod,
		   ItmCod);
  }
