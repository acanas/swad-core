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
#include "swad_global.h"
#include "swad_resource.h"

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
