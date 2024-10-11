// swad_info_database.c: info about course, operations with database

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

#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const char *Inf_DB_NamesForInfoType[Inf_NUM_TYPES] =
  {
   [Inf_INFORMATION	] = "intro",		// TODO: Change this to "inf"
   [Inf_TEACH_GUIDE	] = "description",	// TODO: Change this to "gui"
   [Inf_LECTURES	] = "theory",		// TODO: Change this to "lec"
   [Inf_PRACTICALS	] = "practices",	// TODO: Change this to "pra"
   [Inf_BIBLIOGRAPHY	] = "bibliography",	// TODO: Change this to "bib"
   [Inf_FAQ		] = "FAQ",		// TODO: Change this to "faq"
   [Inf_LINKS		] = "links",		// TODO: Change this to "lnk"
   [Inf_ASSESSMENT	] = "assessment",	// TODO: Change this to "ass"
  };

static const char *Inf_DB_NamesForInfoSrc[Inf_NUM_SOURCES] =
  {
   [Inf_NONE		] = "none",
   [Inf_EDITOR		] = "editor",
   [Inf_PLAIN_TEXT	] = "plain_text",
   [Inf_RICH_TEXT	] = "rich_text",
   [Inf_PAGE		] = "page",
   [Inf_URL		] = "URL",
  };

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

/*****************************************************************************/
/*** Convert a string with info type in database to a Inf_InfoType_t value ***/
/*****************************************************************************/

Inf_Type_t Inf_DB_ConvertFromStrDBToInfoType (const char *StrInfoTypeDB)
  {
   Inf_Type_t InfoType;

   for (InfoType  = (Inf_Type_t) 0;
	InfoType <= (Inf_Type_t) (Inf_NUM_TYPES - 1);
	InfoType++)
      if (!strcmp (StrInfoTypeDB,Inf_DB_NamesForInfoType[InfoType]))
         return InfoType;

   return (Inf_Type_t) 0;
  }

/*****************************************************************************/
/********* Set info source for a type of course info from database ***********/
/*****************************************************************************/

void Inf_DB_SetInfoSrc (Inf_Src_t InfoSrc)
  {
   /***** Get if info source is already stored in database *****/
   if (DB_QueryCOUNT ("can not get if info source is already stored in database",
		      "SELECT COUNT(*)"
		       " FROM crs_info_src"
		      " WHERE CrsCod=%ld"
		        " AND InfoType='%s'",
		      Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		      Inf_DB_NamesForInfoType[Gbl.Crs.Info.Type]))
      // Info is already stored in database, so update it
     {	// Update info source
      if (InfoSrc == Inf_NONE)
         DB_QueryUPDATE ("can not update info source",
			 "UPDATE crs_info_src"
			   " SET InfoSrc='%s',"
			        "MustBeRead='N'"
			 " WHERE CrsCod=%ld"
			   " AND InfoType='%s'",
                         Inf_DB_NamesForInfoSrc[Inf_NONE],
                         Gbl.Hierarchy.Node[Hie_CRS].HieCod,
                         Inf_DB_NamesForInfoType[Gbl.Crs.Info.Type]);
      else	// MustBeRead remains unchanged
         DB_QueryUPDATE ("can not update info source",
			 "UPDATE crs_info_src"
			   " SET InfoSrc='%s'"
		         " WHERE CrsCod=%ld"
		           " AND InfoType='%s'",
		         Inf_DB_NamesForInfoSrc[InfoSrc],
		         Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		         Inf_DB_NamesForInfoType[Gbl.Crs.Info.Type]);
     }
   else		// Info is not stored in database, so insert it
      DB_QueryINSERT ("can not insert info source",
		      "INSERT INTO crs_info_src"
		      " (CrsCod,InfoType,InfoSrc,MustBeRead)"
		      " VALUES"
		      " (%ld,'%s','%s','N')",
		      Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		      Inf_DB_NamesForInfoType[Gbl.Crs.Info.Type],
		      Inf_DB_NamesForInfoSrc[InfoSrc]);
  }

/*****************************************************************************/
/***************** Get info source for a type of course info *****************/
/*****************************************************************************/

unsigned Inf_DB_GetInfoSrc (MYSQL_RES **mysql_res,
                            long CrsCod,Inf_Type_t InfoType)
  {
   /***** Get info source for a specific type of info from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get info source",
		   "SELECT InfoSrc"	// row[0]
		    " FROM crs_info_src"
		   " WHERE CrsCod=%ld"
		     " AND InfoType='%s'",
		   CrsCod,
		   Inf_DB_NamesForInfoType[InfoType]);
  }

/*****************************************************************************/
/*** Get info source and wether info must be read for a type of course info **/
/*****************************************************************************/

unsigned Inf_DB_GetInfoSrcAndMustBeRead (MYSQL_RES **mysql_res,
                                         long CrsCod,Inf_Type_t InfoType)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get info source",
		   "SELECT InfoSrc,"	// row[0]
			  "MustBeRead"	// row[1]
		    " FROM crs_info_src"
		   " WHERE CrsCod=%ld"
		     " AND InfoType='%s'",
		   CrsCod,
		   Inf_DB_NamesForInfoType[InfoType]);
  }

/*****************************************************************************/
/** Convert a string with info source in database to a Inf_InfoSrc_t value ***/
/*****************************************************************************/

Inf_Src_t Inf_DB_ConvertFromStrDBToInfoSrc (const char *StrInfoSrcDB)
  {
   Inf_Src_t InfoSrc;

   for (InfoSrc  = (Inf_Src_t) 0;
	InfoSrc <= (Inf_Src_t) (Inf_NUM_SOURCES - 1);
	InfoSrc++)
      if (!strcmp (StrInfoSrcDB,Inf_DB_NamesForInfoSrc[InfoSrc]))
         return InfoSrc;

   return Inf_NONE;
  }

/*****************************************************************************/
/********** Set info text for a type of course info from database ************/
/*****************************************************************************/

void Inf_DB_SetInfoTxt (const char *InfoTxtHTML,const char *InfoTxtMD)
  {
   /***** Insert or replace info source for a specific type of course information *****/
   DB_QueryREPLACE ("can not update info text",
		    "REPLACE INTO crs_info_txt"
		    " (CrsCod,InfoType,InfoTxtHTML,InfoTxtMD)"
		    " VALUES"
		    " (%ld,'%s','%s','%s')",
		    Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		    Inf_DB_NamesForInfoType[Gbl.Crs.Info.Type],
		    InfoTxtHTML,
		    InfoTxtMD);
  }
/*****************************************************************************/
/********** Get info text for a type of course info from database ************/
/*****************************************************************************/

unsigned Inf_DB_GetInfoTxt (MYSQL_RES **mysql_res,
                            long CrsCod,Inf_Type_t InfoType)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get info text",
		   "SELECT InfoTxtHTML,"	// row[0]
			  "InfoTxtMD"		// row[1]
		    " FROM crs_info_txt"
		   " WHERE CrsCod=%ld"
		     " AND InfoType='%s'",
		   CrsCod,
		   Inf_DB_NamesForInfoType[InfoType]);
  }

/*****************************************************************************/
/***************** Set if students must read course info *********************/
/*****************************************************************************/

void Inf_DB_SetForceRead (bool MustBeRead)
  {
   DB_QueryUPDATE ("can not update if info must be read",
		   "UPDATE crs_info_src"
		     " SET MustBeRead='%c'"
		   " WHERE CrsCod=%ld"
		     " AND InfoType='%s'",
                   MustBeRead ? 'Y' :
        	                'N',
                   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Inf_DB_NamesForInfoType[Gbl.Crs.Info.Type]);
  }

/*****************************************************************************/
/********************* Set if I have read course info ************************/
/*****************************************************************************/

void Inf_DB_SetIHaveRead (bool IHaveRead)
  {
   if (IHaveRead)
      /***** Insert I have read course information *****/
      DB_QueryREPLACE ("can not set that I have read course info",
		       "REPLACE INTO crs_info_read"
		       " (UsrCod,CrsCod,InfoType)"
		       " VALUES"
		       " (%ld,%ld,'%s')",
                       Gbl.Usrs.Me.UsrDat.UsrCod,
                       Gbl.Hierarchy.Node[Hie_CRS].HieCod,
                       Inf_DB_NamesForInfoType[Gbl.Crs.Info.Type]);
   else
      /***** Remove I have read course information *****/
      DB_QueryDELETE ("can not set that I have not read course info",
		      "DELETE FROM crs_info_read"
		      " WHERE UsrCod=%ld"
		        " AND CrsCod=%ld"
		        " AND InfoType='%s'",
		      Gbl.Usrs.Me.UsrDat.UsrCod,
		      Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		      Inf_DB_NamesForInfoType[Gbl.Crs.Info.Type]);
  }

/*****************************************************************************/
/******************** Check I have read a course info ************************/
/*****************************************************************************/

bool Inf_DB_CheckIfIHaveReadInfo (void)
  {
   return
   DB_QueryEXISTS ("can not check if I have read course info",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM crs_info_read"
		    " WHERE UsrCod=%ld"
		      " AND CrsCod=%ld"
		      " AND InfoType='%s')",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Inf_DB_NamesForInfoType[Gbl.Crs.Info.Type]);
  }

/*****************************************************************************/
/******************* Get info types where I must read info *******************/
/*****************************************************************************/

unsigned Inf_DB_GetInfoTypesfIMustReadInfo (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get if you must read any course info",
		   "SELECT InfoType"		// row[0]
		    " FROM crs_info_src"
		   " WHERE CrsCod=%ld"
		     " AND MustBeRead='Y'"
		     " AND InfoType NOT IN"
		         " (SELECT InfoType"
			    " FROM crs_info_read"
			   " WHERE UsrCod=%ld"
			     " AND CrsCod=%ld)",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/********* Remove user's status about reading of course information **********/
/*****************************************************************************/

void Inf_DB_RemoveUsrFromCrsInfoRead (long UsrCod,long CrsCod)
  {
   DB_QueryDELETE ("can not set that I have not read course info",
		   "DELETE FROM crs_info_read"
		   " WHERE UsrCod=%ld"
		     " AND CrsCod=%ld",
                   UsrCod,CrsCod);
  }
