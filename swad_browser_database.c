// swad_browser_database.c: file browsers operations with database

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

#include <mysql/mysql.h>	// To access MySQL databases
#include <string.h>		// For string functions

#include "swad_browser.h"
#include "swad_browser_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_project.h"

/*****************************************************************************/
/******************** Global variables from other modules ********************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

// Browsers types for database "files" and "brw_sizes" tables
const Brw_FileBrowser_t Brw_DB_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = Brw_UNKNOWN,
   [Brw_SHOW_DOC_CRS] = Brw_ADMI_DOC_CRS,
   [Brw_SHOW_MRK_CRS] = Brw_ADMI_MRK_CRS,
   [Brw_ADMI_DOC_CRS] = Brw_ADMI_DOC_CRS,
   [Brw_ADMI_SHR_CRS] = Brw_ADMI_SHR_CRS,
   [Brw_ADMI_SHR_GRP] = Brw_ADMI_SHR_GRP,
   [Brw_ADMI_WRK_USR] = Brw_ADMI_WRK_USR,
   [Brw_ADMI_WRK_CRS] = Brw_ADMI_WRK_USR,
   [Brw_ADMI_MRK_CRS] = Brw_ADMI_MRK_CRS,
   [Brw_ADMI_BRF_USR] = Brw_ADMI_BRF_USR,
   [Brw_SHOW_DOC_GRP] = Brw_ADMI_DOC_GRP,
   [Brw_ADMI_DOC_GRP] = Brw_ADMI_DOC_GRP,
   [Brw_SHOW_MRK_GRP] = Brw_ADMI_MRK_GRP,
   [Brw_ADMI_MRK_GRP] = Brw_ADMI_MRK_GRP,
   [Brw_ADMI_ASG_USR] = Brw_ADMI_ASG_USR,
   [Brw_ADMI_ASG_CRS] = Brw_ADMI_ASG_USR,
   [Brw_SHOW_DOC_DEG] = Brw_ADMI_DOC_DEG,
   [Brw_ADMI_DOC_DEG] = Brw_ADMI_DOC_DEG,
   [Brw_SHOW_DOC_CTR] = Brw_ADMI_DOC_CTR,
   [Brw_ADMI_DOC_CTR] = Brw_ADMI_DOC_CTR,
   [Brw_SHOW_DOC_INS] = Brw_ADMI_DOC_INS,
   [Brw_ADMI_DOC_INS] = Brw_ADMI_DOC_INS,
   [Brw_ADMI_SHR_DEG] = Brw_ADMI_SHR_DEG,
   [Brw_ADMI_SHR_CTR] = Brw_ADMI_SHR_CTR,
   [Brw_ADMI_SHR_INS] = Brw_ADMI_SHR_INS,
   [Brw_ADMI_TCH_CRS] = Brw_ADMI_TCH_CRS,
   [Brw_ADMI_TCH_GRP] = Brw_ADMI_TCH_GRP,
   [Brw_ADMI_DOC_PRJ] = Brw_ADMI_DOC_PRJ,
   [Brw_ADMI_ASS_PRJ] = Brw_ADMI_ASS_PRJ,
  };

// Browsers types for database "brw_last" table
// Assignments and works are stored as one in brw_last...
// ...because a user views them at the same time
static const Brw_FileBrowser_t Brw_DB_FileBrowserForDB_file_browser_last[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = Brw_UNKNOWN,
   [Brw_SHOW_DOC_CRS] = Brw_ADMI_DOC_CRS,
   [Brw_SHOW_MRK_CRS] = Brw_ADMI_MRK_CRS,
   [Brw_ADMI_DOC_CRS] = Brw_ADMI_DOC_CRS,
   [Brw_ADMI_SHR_CRS] = Brw_ADMI_SHR_CRS,
   [Brw_ADMI_SHR_GRP] = Brw_ADMI_SHR_GRP,
   [Brw_ADMI_WRK_USR] = Brw_ADMI_ASG_USR,
   [Brw_ADMI_WRK_CRS] = Brw_ADMI_ASG_CRS,
   [Brw_ADMI_MRK_CRS] = Brw_ADMI_MRK_CRS,
   [Brw_ADMI_BRF_USR] = Brw_ADMI_BRF_USR,
   [Brw_SHOW_DOC_GRP] = Brw_ADMI_DOC_GRP,
   [Brw_ADMI_DOC_GRP] = Brw_ADMI_DOC_GRP,
   [Brw_SHOW_MRK_GRP] = Brw_ADMI_MRK_GRP,
   [Brw_ADMI_MRK_GRP] = Brw_ADMI_MRK_GRP,
   [Brw_ADMI_ASG_USR] = Brw_ADMI_ASG_USR,
   [Brw_ADMI_ASG_CRS] = Brw_ADMI_ASG_CRS,
   [Brw_SHOW_DOC_DEG] = Brw_ADMI_DOC_DEG,
   [Brw_ADMI_DOC_DEG] = Brw_ADMI_DOC_DEG,
   [Brw_SHOW_DOC_CTR] = Brw_ADMI_DOC_CTR,
   [Brw_ADMI_DOC_CTR] = Brw_ADMI_DOC_CTR,
   [Brw_SHOW_DOC_INS] = Brw_ADMI_DOC_INS,
   [Brw_ADMI_DOC_INS] = Brw_ADMI_DOC_INS,
   [Brw_ADMI_SHR_DEG] = Brw_ADMI_SHR_DEG,
   [Brw_ADMI_SHR_CTR] = Brw_ADMI_SHR_CTR,
   [Brw_ADMI_SHR_INS] = Brw_ADMI_SHR_INS,
   [Brw_ADMI_TCH_CRS] = Brw_ADMI_TCH_CRS,
   [Brw_ADMI_TCH_GRP] = Brw_ADMI_TCH_GRP,
   [Brw_ADMI_DOC_PRJ] = Brw_ADMI_DOC_PRJ,
   [Brw_ADMI_ASS_PRJ] = Brw_ADMI_ASS_PRJ,
  };

// Browsers types for database "expanded_folders" table
static const Brw_FileBrowser_t Brw_DB_FileBrowserForDB_expanded_folders[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = Brw_UNKNOWN,
   [Brw_SHOW_DOC_CRS] = Brw_ADMI_DOC_CRS,
   [Brw_SHOW_MRK_CRS] = Brw_ADMI_MRK_CRS,
   [Brw_ADMI_DOC_CRS] = Brw_ADMI_DOC_CRS,
   [Brw_ADMI_SHR_CRS] = Brw_ADMI_SHR_CRS,
   [Brw_ADMI_SHR_GRP] = Brw_ADMI_SHR_GRP,
   [Brw_ADMI_WRK_USR] = Brw_ADMI_WRK_USR,
   [Brw_ADMI_WRK_CRS] = Brw_ADMI_WRK_CRS,
   [Brw_ADMI_MRK_CRS] = Brw_ADMI_MRK_CRS,
   [Brw_ADMI_BRF_USR] = Brw_ADMI_BRF_USR,
   [Brw_SHOW_DOC_GRP] = Brw_ADMI_DOC_GRP,
   [Brw_ADMI_DOC_GRP] = Brw_ADMI_DOC_GRP,
   [Brw_SHOW_MRK_GRP] = Brw_ADMI_MRK_GRP,
   [Brw_ADMI_MRK_GRP] = Brw_ADMI_MRK_GRP,
   [Brw_ADMI_ASG_USR] = Brw_ADMI_ASG_USR,
   [Brw_ADMI_ASG_CRS] = Brw_ADMI_ASG_CRS,
   [Brw_SHOW_DOC_DEG] = Brw_ADMI_DOC_DEG,
   [Brw_ADMI_DOC_DEG] = Brw_ADMI_DOC_DEG,
   [Brw_SHOW_DOC_CTR] = Brw_ADMI_DOC_CTR,
   [Brw_ADMI_DOC_CTR] = Brw_ADMI_DOC_CTR,
   [Brw_SHOW_DOC_INS] = Brw_ADMI_DOC_INS,
   [Brw_ADMI_DOC_INS] = Brw_ADMI_DOC_INS,
   [Brw_ADMI_SHR_DEG] = Brw_ADMI_SHR_DEG,
   [Brw_ADMI_SHR_CTR] = Brw_ADMI_SHR_CTR,
   [Brw_ADMI_SHR_INS] = Brw_ADMI_SHR_INS,
   [Brw_ADMI_TCH_CRS] = Brw_ADMI_TCH_CRS,
   [Brw_ADMI_TCH_GRP] = Brw_ADMI_TCH_GRP,
   [Brw_ADMI_DOC_PRJ] = Brw_ADMI_DOC_PRJ,
   [Brw_ADMI_ASS_PRJ] = Brw_ADMI_ASS_PRJ,
  };

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

/*****************************************************************************/
/**************** Add a path of file/folder to the database ******************/
/*****************************************************************************/

long Brw_DB_AddPath (long PublisherUsrCod,Brw_FileType_t FileType,
                     const char *FullPathInTree,bool IsPublic,Brw_License_t License)
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFileBrowser ();

   /***** Add path to the database *****/
   return
   DB_QueryINSERTandReturnCode ("can not add path to database",
				"INSERT INTO brw_files"
				" (FileBrowser,Cod,ZoneUsrCod,PublisherUsrCod,"
				  "FileType,Path,Hidden,Public,License)"
				" VALUES"
				" (%u,%ld,%ld,%ld,"
				  "%u,'%s','N','%c',%u)",
				(unsigned) Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type],
				Cod,
				ZoneUsrCod,
				PublisherUsrCod,
				(unsigned) FileType,
				FullPathInTree,
				IsPublic ? 'Y' :
					   'N',
				(unsigned) License);
  }

/*****************************************************************************/
/*************** Rename a file or folder in table of files *******************/
/*****************************************************************************/

void Brw_DB_RenameOneFolder (const char OldPath[PATH_MAX + 1],
                             const char NewPath[PATH_MAX + 1])
  {
   /***** Update file or folder in table of common files *****/
   DB_QueryUPDATE ("can not update folder name in a common zone",
		   "UPDATE brw_files"
		     " SET Path='%s'"
		   " WHERE FileBrowser=%u"
		     " AND Cod=%ld"
		     " AND ZoneUsrCod=%ld"
		     " AND Path='%s'",
		   NewPath,
		   (unsigned) Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type],
		   Brw_GetCodForFileBrowser (),
		   Brw_GetZoneUsrCodForFileBrowser (),
		   OldPath);
  }

/*****************************************************************************/
/************** Rename children of a folder in table of files ****************/
/*****************************************************************************/

void Brw_DB_RenameChildrenFilesOrFolders (const char OldPath[PATH_MAX + 1],
                                          const char NewPath[PATH_MAX + 1])
  {
   extern const Brw_FileBrowser_t Brw_DB_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   unsigned StartFinalSubpathNotChanged = strlen (OldPath) + 2;

   /***** Update children of a folder in table of files *****/
   DB_QueryUPDATE ("can not rename file or folder names in a common zone",
		   "UPDATE brw_files"
		     " SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
		   " WHERE FileBrowser=%u"
		     " AND Cod=%ld"
		     " AND ZoneUsrCod=%ld"
		     " AND Path LIKE '%s/%%'",
	           NewPath,StartFinalSubpathNotChanged,
	           (unsigned) Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type],
	           Brw_GetCodForFileBrowser (),
	           Brw_GetZoneUsrCodForFileBrowser (),
	           OldPath);
  }

/*****************************************************************************/
/*********************** Get file code using its path ************************/
/*****************************************************************************/
// Path is the full path in tree
// Example: descarga/folder/file.pdf

long Brw_DB_GetFilCodByPath (const char *Path,bool OnlyIfPublic)
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFileBrowser ();

   return DB_QuerySELECTCode ("can not get file code",
			      "SELECT FilCod"
			       " FROM brw_files"
			      " WHERE FileBrowser=%u"
			        " AND Cod=%ld"
			        " AND ZoneUsrCod=%ld"
			        " AND Path='%s'"
			        "%s"
			      " ORDER BY FilCod DESC"	// Due to errors, there could be old entries for the same path.
			      " LIMIT 1",		// Select the most recent entry.
			      (unsigned) Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type],
			      Cod,
			      ZoneUsrCod,
			      Path,
			      OnlyIfPublic ? " AND Public='Y'" :
					     "");
  }

/*****************************************************************************/
/********************* Get file metadata using its path **********************/
/*****************************************************************************/
// This function only gets metadata stored in table files,
// does not get size, time, numviews...

unsigned Brw_DB_GetFileMetadataByPath (MYSQL_RES **mysql_res,const char *Path)
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFileBrowser ();

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get file metadata",
		   "SELECT FilCod,"		// row[0]
			  "FileBrowser,"	// row[1]
			  "Cod,"		// row[2]
			  "ZoneUsrCod,"		// row[3]
			  "PublisherUsrCod,"	// row[4]
			  "FileType,"		// row[5]
			  "Path,"		// row[6]
			  "Hidden,"		// row[7]
			  "Public,"		// row[8]
			  "License"		// row[9]
		    " FROM brw_files"
		   " WHERE FileBrowser=%u"
		     " AND Cod=%ld"
		     " AND ZoneUsrCod=%ld"
		     " AND Path='%s'"
		   " ORDER BY FilCod DESC"	// Due to errors, there could be old entries for the same path.
		   " LIMIT 1",			// Select the most recent entry.
		   (unsigned) Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type],
		   Cod,
		   ZoneUsrCod,
		   Path);
  }

/*****************************************************************************/
/********************* Get file metadata using its code **********************/
/*****************************************************************************/
// This function only gets metadata stored in table files,
// does not get size, time, numviews...

unsigned Brw_DB_GetFileMetadataByCod (MYSQL_RES **mysql_res,long FilCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get file metadata",
		   "SELECT FilCod,"		// row[0]
			  "FileBrowser,"	// row[1]
			  "Cod,"		// row[2]
			  "ZoneUsrCod,"		// row[3]
			  "PublisherUsrCod,"	// row[4]
			  "FileType,"		// row[5]
			  "Path,"		// row[6]
			  "Hidden,"		// row[7]
			  "Public,"		// row[8]
			  "License"		// row[9]
		    " FROM brw_files"
		   " WHERE FilCod=%ld",
		   FilCod);
  }

/*****************************************************************************/
/************************ Get file path using its code ***********************/
/*****************************************************************************/

void Brw_DB_GetPathByCod (long FilCod,char *Title,size_t TitleSize)
  {
   DB_QuerySELECTString (Title,TitleSize,"can not get path",
			 "SELECT Path"		// row[0]
			  " FROM brw_files"
			 " WHERE FilCod=%ld",
			 FilCod);
  }

/*****************************************************************************/
/************************ Get the publisher of a subtree *********************/
/*****************************************************************************/

long Brw_DB_GetPublisherOfSubtree (const char *Path)
  {
   /***** Get all common files that are equal to full path (including filename)
	  or that are under that full path from database *****/
   return DB_QuerySELECTCode ("can not get publishers of files",
			      "SELECT DISTINCT "
			             "PublisherUsrCod"
			       " FROM brw_files"
			      " WHERE FileBrowser=%u"
			        " AND Cod=%ld"
			        " AND (Path='%s'"
				     " OR"
				     " Path LIKE '%s/%%')",
			      (unsigned) Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type],
			      Brw_GetCodForFileBrowser (),
			      Path,
			      Path);
  }

/*****************************************************************************/
/************ Get current number of files published by a user ****************/
/*****************************************************************************/

unsigned Brw_DB_GetNumFilesUsr (long UsrCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of files from a user",
		  "SELECT COUNT(*)"
		   " FROM brw_files"
		  " WHERE PublisherUsrCod=%ld"
		    " AND FileType IN (%u,%u)",
		  UsrCod,
		  (unsigned) Brw_IS_FILE,
		  (unsigned) Brw_IS_UNKNOWN);	// Unknown entries are counted as files
  }

/*****************************************************************************/
/*********** Get the number of files in document zones of a course ***********/
/*****************************************************************************/

unsigned Brw_DB_GetNumFilesInDocumZonesOfCrs (long CrsCod)
  {
   extern const Brw_FileBrowser_t Brw_DB_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];

   /***** Get number of files in document zones of a course from database *****/
   return DB_QuerySELECTUnsigned ("can not get the number of files",
				  "SELECT"
				  " (SELECT COALESCE(SUM(NumFiles),0)"
				     " FROM brw_sizes"
				    " WHERE FileBrowser=%u AND Cod=%ld) +"
				  " (SELECT COALESCE(SUM(brw_sizes.NumFiles),0)"
				     " FROM grp_types,"
					   "grp_groups,"
					   "brw_sizes"
				    " WHERE grp_types.CrsCod=%ld"
				      " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
				      " AND brw_sizes.FileBrowser=%u"
				      " AND brw_sizes.Cod=grp_groups.GrpCod)",
				  (unsigned) Brw_DB_FileBrowserForDB_files[Brw_ADMI_DOC_CRS],
				  CrsCod,
				  CrsCod,
				  (unsigned) Brw_DB_FileBrowserForDB_files[Brw_ADMI_DOC_GRP]);
  }

/*****************************************************************************/
/*********** Get the number of files in shared zones of a course ***********/
/*****************************************************************************/

unsigned Brw_DB_GetNumFilesInShareZonesOfCrs (long CrsCod)
  {
   extern const Brw_FileBrowser_t Brw_DB_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];

   /***** Get number of files in document zones of a course from database *****/
   return DB_QuerySELECTUnsigned ("can not get the number of files",
				  "SELECT"
				  " (SELECT COALESCE(SUM(NumFiles),0)"
				     " FROM brw_sizes"
				    " WHERE FileBrowser=%u AND Cod=%ld) +"
				  " (SELECT COALESCE(SUM(brw_sizes.NumFiles),0)"
				     " FROM grp_types,"
					   "grp_groups,"
					   "brw_sizes"
				    " WHERE grp_types.CrsCod=%ld"
				      " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
				      " AND brw_sizes.FileBrowser=%u"
				      " AND brw_sizes.Cod=grp_groups.GrpCod)",
				  (unsigned) Brw_DB_FileBrowserForDB_files[Brw_ADMI_SHR_CRS],
				  CrsCod,
				  CrsCod,
				  (unsigned) Brw_DB_FileBrowserForDB_files[Brw_ADMI_SHR_GRP]);
  }

/*****************************************************************************/
/********* Get the number of files in assignment zones of a course ***********/
/*****************************************************************************/

unsigned Brw_DB_GetNumFilesInAssigZonesOfCrs (long CrsCod)
  {
   extern const Brw_FileBrowser_t Brw_DB_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];

   /***** Get number of files in document zones of a course from database *****/
   return DB_QuerySELECTUnsigned ("can not get the number of files",
				  "SELECT COALESCE(SUM(NumFiles),0)"
				   " FROM brw_sizes"
				  " WHERE FileBrowser=%u"
				    " AND Cod=%ld",
				  (unsigned) Brw_DB_FileBrowserForDB_files[Brw_ADMI_ASG_USR],
				  CrsCod);
  }

/*****************************************************************************/
/************* Get the number of files in works zones of a course ************/
/*****************************************************************************/

unsigned Brw_DB_GetNumFilesInWorksZonesOfCrs (long CrsCod)
  {
   extern const Brw_FileBrowser_t Brw_DB_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];

   /***** Get number of files in document zones of a course from database *****/
   return DB_QuerySELECTUnsigned ("can not get the number of files",
				  "SELECT COALESCE(SUM(NumFiles),0)"
				   " FROM brw_sizes"
				  " WHERE FileBrowser=%u"
				    " AND Cod=%ld",
				  (unsigned) Brw_DB_FileBrowserForDB_files[Brw_ADMI_WRK_USR],
				  CrsCod);
  }

/*****************************************************************************/
/**************** Remove a file or folder from the database ******************/
/*****************************************************************************/

void Brw_DB_RemoveOneFileOrFolder (const char Path[PATH_MAX + 1])
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFileBrowser ();
   Brw_FileBrowser_t FileBrowser = Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type];

   /***** Remove from database the entries that store the marks properties *****/
   if (FileBrowser == Brw_ADMI_MRK_CRS ||
       FileBrowser == Brw_ADMI_MRK_GRP)
      DB_QueryDELETE ("can not remove properties of marks from database",
		      "DELETE FROM mrk_marks"
		      " USING brw_files,"
		             "mrk_marks"
		      " WHERE brw_files.FileBrowser=%u"
		        " AND brw_files.Cod=%ld"
		        " AND brw_files.Path='%s'"
		        " AND brw_files.FilCod=mrk_marks.FilCod",
	              (unsigned) FileBrowser,
	              Cod,
	              Path);

   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views from database",
		  "DELETE FROM brw_views"
		  " USING brw_files,"
		         "brw_views"
		  " WHERE brw_files.FileBrowser=%u"
		    " AND brw_files.Cod=%ld"
		    " AND brw_files.ZoneUsrCod=%ld"
		    " AND brw_files.Path='%s'"
		    " AND brw_files.FilCod=brw_views.FilCod",
	          (unsigned) FileBrowser,
	          Cod,
	          ZoneUsrCod,
	          Path);

   /***** Remove from database the entry that stores the data of a file *****/
   DB_QueryDELETE ("can not remove path from database",
		   "DELETE FROM brw_files"
		   " WHERE FileBrowser=%u"
		     " AND Cod=%ld"
		     " AND ZoneUsrCod=%ld"
		     " AND Path='%s'",
	           (unsigned) FileBrowser,
	           Cod,
	           ZoneUsrCod,
	           Path);
  }

/*****************************************************************************/
/************** Remove children of a folder from the database ****************/
/*****************************************************************************/

void Brw_DB_RemoveChildrenOfFolder (const char Path[PATH_MAX + 1])
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFileBrowser ();
   Brw_FileBrowser_t FileBrowser = Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type];

   /***** Remove from database the entries that store the marks properties *****/
   if (FileBrowser == Brw_ADMI_MRK_CRS ||
       FileBrowser == Brw_ADMI_MRK_GRP)
      DB_QueryDELETE ("can not remove properties of marks from database",
		      "DELETE FROM mrk_marks"
		      " USING brw_files,"
		             "mrk_marks"
		      " WHERE brw_files.FileBrowser=%u"
		        " AND brw_files.Cod=%ld"
		        " AND brw_files.Path LIKE '%s/%%'"
		        " AND brw_files.FilCod=mrk_marks.FilCod",
	              (unsigned) FileBrowser,
	              Cod,
	              Path);

   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views from database",
		  "DELETE FROM brw_views"
		  " USING brw_files,"
		         "brw_views"
		  " WHERE brw_files.FileBrowser=%u"
		    " AND brw_files.Cod=%ld"
		    " AND brw_files.ZoneUsrCod=%ld"
		    " AND brw_files.Path LIKE '%s/%%'"
		    " AND brw_files.FilCod=brw_views.FilCod",
                  (unsigned) FileBrowser,
                  Cod,
                  ZoneUsrCod,
                  Path);

   /***** Remove from database the entries that store the data of files *****/
   DB_QueryDELETE ("can not remove paths from database",
		   "DELETE FROM brw_files"
		   " WHERE FileBrowser=%u"
		     " AND Cod=%ld"
		     " AND ZoneUsrCod=%ld"
		     " AND Path LIKE '%s/%%'",
                   (unsigned) FileBrowser,
                   Cod,
                   ZoneUsrCod,
                   Path);
  }

/*****************************************************************************/
/******** Remove files related to an institution from the database ***********/
/*****************************************************************************/

void Brw_DB_RemoveInsFiles (long InsCod)
  {
   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views to files of an institution",
		   "DELETE FROM brw_views"
		   " USING brw_files,"
		          "brw_views"
		   " WHERE brw_files.FileBrowser IN (%u,%u)"
		     " AND brw_files.Cod=%ld"
		     " AND brw_files.FilCod=brw_views.FilCod",
	           (unsigned) Brw_ADMI_DOC_INS,
	           (unsigned) Brw_ADMI_SHR_INS,
	           InsCod);

   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders of an institution",
		   "DELETE LOW_PRIORITY FROM brw_expanded"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_INS,
	           (unsigned) Brw_ADMI_SHR_INS,
	           InsCod);

   /***** Remove from database the entries that store clipboards *****/
   DB_QueryDELETE ("can not remove clipboards"
		   " related to files of an institution",
		   "DELETE FROM brw_clipboards"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_INS,
	           (unsigned) Brw_ADMI_SHR_INS,
	           InsCod);

   /***** Remove from database the entries that store
          the last time users visited file zones *****/
   DB_QueryDELETE ("can not remove file last visits"
		   " to files of an institution",
		   "DELETE FROM brw_last"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_INS,
	           (unsigned) Brw_ADMI_SHR_INS,
	           InsCod);

   /***** Remove from database the entries that store
          the sizes of the file zones *****/
   DB_QueryDELETE ("can not remove sizes of file zones of an institution",
		   "DELETE FROM brw_sizes"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_INS,
	           (unsigned) Brw_ADMI_SHR_INS,
	           InsCod);

   /***** Remove from database the entries that store the data files *****/
   DB_QueryDELETE ("can not remove files of an institution",
		   "DELETE FROM brw_files"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_INS,
	           (unsigned) Brw_ADMI_SHR_INS,
	           InsCod);
  }

/*****************************************************************************/
/************ Remove files related to a center from the database *************/
/*****************************************************************************/

void Brw_DB_RemoveCtrFiles (long CtrCod)
  {
   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views to files of a center",
		   "DELETE FROM brw_views"
		   " USING brw_files,"
		          "brw_views"
		   " WHERE brw_files.FileBrowser IN (%u,%u)"
		     " AND brw_files.Cod=%ld"
		     " AND brw_files.FilCod=brw_views.FilCod",
	           (unsigned) Brw_ADMI_DOC_CTR,
	           (unsigned) Brw_ADMI_SHR_CTR,
	           CtrCod);

   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders of a center",
		   "DELETE LOW_PRIORITY FROM brw_expanded"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_CTR,
	           (unsigned) Brw_ADMI_SHR_CTR,
	           CtrCod);

   /***** Remove from database the entries that store clipboards *****/
   DB_QueryDELETE ("can not remove clipboards related to files of a center",
		   "DELETE FROM brw_clipboards"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_CTR,
	           (unsigned) Brw_ADMI_SHR_CTR,
	           CtrCod);

   /***** Remove from database the entries that store the last time users visited file zones *****/
   DB_QueryDELETE ("can not remove file last visits to files of a center",
		   "DELETE FROM brw_last"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_CTR,
	           (unsigned) Brw_ADMI_SHR_CTR,
	           CtrCod);

   /***** Remove from database the entries that store the sizes of the file zones *****/
   DB_QueryDELETE ("can not remove sizes of file zones of a center",
		   "DELETE FROM brw_sizes"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_CTR,
	           (unsigned) Brw_ADMI_SHR_CTR,
	           CtrCod);

   /***** Remove from database the entries that store the data files *****/
   DB_QueryDELETE ("can not remove files of a center",
		   "DELETE FROM brw_files"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_CTR,
	           (unsigned) Brw_ADMI_SHR_CTR,
	           CtrCod);
  }

/*****************************************************************************/
/************ Remove files related to a degree from the database *************/
/*****************************************************************************/

void Brw_DB_RemoveDegFiles (long DegCod)
  {
   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views to files of a degree",
		   "DELETE FROM brw_views"
		   " USING brw_files,"
		          "brw_views"
		   " WHERE brw_files.FileBrowser IN (%u,%u)"
		     " AND brw_files.Cod=%ld"
		     " AND brw_files.FilCod=brw_views.FilCod",
	           (unsigned) Brw_ADMI_DOC_DEG,
	           (unsigned) Brw_ADMI_SHR_DEG,
	           DegCod);

   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders of a degree",
		   "DELETE LOW_PRIORITY FROM brw_expanded"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_DEG,
	           (unsigned) Brw_ADMI_SHR_DEG,
	           DegCod);

   /***** Remove from database the entries that store clipboards *****/
   DB_QueryDELETE ("can not remove clipboards related to files of a degree",
		   "DELETE FROM brw_clipboards"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_DEG,
	           (unsigned) Brw_ADMI_SHR_DEG,
	           DegCod);

   /***** Remove from database the entries that store the last time users visited file zones *****/
   DB_QueryDELETE ("can not remove file last visits to files of a degree",
		   "DELETE FROM brw_last"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_DEG,
	           (unsigned) Brw_ADMI_SHR_DEG,
	           DegCod);

   /***** Remove from database the entries that store the sizes of the file zones *****/
   DB_QueryDELETE ("can not remove sizes of file zones of a degree",
		   "DELETE FROM brw_sizes"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_DEG,
	           (unsigned) Brw_ADMI_SHR_DEG,
	           DegCod);

   /***** Remove from database the entries that store the data files *****/
   DB_QueryDELETE ("can not remove files of a degree",
		   "DELETE FROM brw_files"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_DEG,
	           (unsigned) Brw_ADMI_SHR_DEG,
	           DegCod);
  }

/*****************************************************************************/
/************ Remove files related to a course from the database *************/
/*****************************************************************************/
/* Remove information related to files in course,
   including groups and projects,
   so this function must be called
   before removing groups and projects */

void Brw_DB_RemoveCrsFiles (long CrsCod)
  {
   char SubqueryGrp[256];
   char SubqueryPrj[128];

   /***** Build subquery for groups *****/
   sprintf (SubqueryGrp,"(SELECT grp_groups.GrpCod"
	                  " FROM grp_types,"
	                        "grp_groups"
		         " WHERE grp_types.CrsCod=%ld"
		           " AND grp_types.GrpTypCod=grp_groups.GrpTypCod)",
            CrsCod);

   /***** Build subquery for projects *****/
   sprintf (SubqueryPrj,"(SELECT PrjCod"
	                  " FROM prj_projects"
	                 " WHERE CrsCod=%ld)",
            CrsCod);

   /***** Remove format of files of marks *****/
   DB_QueryDELETE ("can not remove the properties of marks"
		   " associated to a course",
		   "DELETE FROM mrk_marks"
		   " USING brw_files,"
		          "mrk_marks"
		   " WHERE brw_files.FileBrowser=%u"
		     " AND brw_files.Cod=%ld"
		     " AND brw_files.FilCod=mrk_marks.FilCod",
	           (unsigned) Brw_ADMI_MRK_CRS,
	           CrsCod);

   /***** Remove from database the entries that store the file views *****/
   /* Remove from course file zones */
   DB_QueryDELETE ("can not remove file views to files of a course",
		   "DELETE FROM brw_views"
		   " USING brw_files,"
		          "brw_views"
		   " WHERE brw_files.FileBrowser IN (%u,%u,%u,%u,%u,%u)"
		     " AND brw_files.Cod=%ld"
		     " AND brw_files.FilCod=brw_views.FilCod",
	           (unsigned) Brw_ADMI_DOC_CRS,
	           (unsigned) Brw_ADMI_TCH_CRS,
	           (unsigned) Brw_ADMI_SHR_CRS,
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_WRK_USR,
	           (unsigned) Brw_ADMI_MRK_CRS,
	           CrsCod);

   /* Remove from group file zones */
   DB_QueryDELETE ("can not remove file views to files of a course",
		   "DELETE FROM brw_views"
		   " USING brw_files,"
		          "brw_views"
		   " WHERE brw_files.FileBrowser IN (%u,%u,%u,%u)"
		     " AND brw_files.Cod IN %s"
		     " AND brw_files.FilCod=brw_views.FilCod",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           SubqueryGrp);

   /* Remove from project file zones */
   DB_QueryDELETE ("can not remove file views to files of a course",
		   "DELETE FROM brw_views"
		   " USING brw_files,"
		          "brw_views"
		   " WHERE brw_files.FileBrowser IN (%u,%u)"
		     " AND brw_files.Cod IN %s"
		     " AND brw_files.FilCod=brw_views.FilCod",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           SubqueryPrj);

   /***** Remove from database expanded folders *****/
   /* Remove from course file zones */
   DB_QueryDELETE ("can not remove expanded folders of a course",
		   "DELETE LOW_PRIORITY FROM brw_expanded"
		   " WHERE FileBrowser IN (%u,%u,%u,%u,%u,%u,%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_CRS,
	           (unsigned) Brw_ADMI_TCH_CRS,
	           (unsigned) Brw_ADMI_SHR_CRS,
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_ASG_CRS,
	           (unsigned) Brw_ADMI_WRK_USR,
	           (unsigned) Brw_ADMI_WRK_CRS,
	           (unsigned) Brw_ADMI_MRK_CRS,
	           CrsCod);

   /* Remove from group file zones */
   DB_QueryDELETE ("can not remove expanded folders of a course",
		   "DELETE LOW_PRIORITY FROM brw_expanded"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		     " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           SubqueryGrp);

   /* Remove from project file zones */
   DB_QueryDELETE ("can not remove expanded folders of a course",
		   "DELETE LOW_PRIORITY FROM brw_expanded"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           SubqueryPrj);

   /***** Remove from database the entries that store clipboards *****/
   /* Remove from course file zones */
   DB_QueryDELETE ("can not remove clipboards related to files of a course",
		   "DELETE FROM brw_clipboards"
		   " WHERE FileBrowser IN (%u,%u,%u,%u,%u,%u,%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_CRS,
	           (unsigned) Brw_ADMI_TCH_CRS,
	           (unsigned) Brw_ADMI_SHR_CRS,
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_ASG_CRS,
	           (unsigned) Brw_ADMI_WRK_USR,
	           (unsigned) Brw_ADMI_WRK_CRS,
	           (unsigned) Brw_ADMI_MRK_CRS,
	           CrsCod);

   /* Remove from group file zones */
   DB_QueryDELETE ("can not remove clipboards related to files of a course",
		   "DELETE FROM brw_clipboards"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		     " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           SubqueryGrp);

   /* Remove from project file zones */
   DB_QueryDELETE ("can not remove clipboards related to files of a course",
		   "DELETE FROM brw_clipboards"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           SubqueryPrj);

   /***** Remove from database the entries that store the last time users visited file zones *****/
   // Assignments and works are stored as one in brw_last...
   // ...because a user views them at the same time
   /* Remove from course file zones */
   DB_QueryDELETE ("can not remove file last visits to files of a course",
		   "DELETE FROM brw_last"
		   " WHERE FileBrowser IN (%u,%u,%u,%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_CRS,
	           (unsigned) Brw_ADMI_TCH_CRS,
	           (unsigned) Brw_ADMI_SHR_CRS,
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_MRK_CRS,
	           CrsCod);

   /* Remove from group file zones */
    DB_QueryDELETE ("can not remove file last visits to files of a course",
		    "DELETE FROM brw_last"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		     " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           SubqueryGrp);

   /* Remove from project file zones */
   DB_QueryDELETE ("can not remove file last visits to files of a course",
		   "DELETE FROM brw_last"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           SubqueryPrj);

   /***** Remove from database the entries that store the sizes of the file zones *****/
   /* Remove from course file zones */
   DB_QueryDELETE ("can not remove sizes of file zones of a course",
		   "DELETE FROM brw_sizes"
		   " WHERE FileBrowser IN (%u,%u,%u,%u,%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_CRS,
	           (unsigned) Brw_ADMI_TCH_CRS,
	           (unsigned) Brw_ADMI_SHR_CRS,
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_WRK_USR,
	           (unsigned) Brw_ADMI_MRK_CRS,
	           CrsCod);

   /* Remove from group file zones */
   DB_QueryDELETE ("can not remove sizes of file zones of a course",
		   "DELETE FROM brw_sizes"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		     " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           SubqueryGrp);

   /* Remove from project file zones */
   DB_QueryDELETE ("can not remove sizes of file zones of a course",
		   "DELETE FROM brw_sizes"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           SubqueryPrj);

   /***** Remove from database the entries that store the data files *****/
   /* Remove from course file zones */
   DB_QueryDELETE ("can not remove files of a course",
		   "DELETE FROM brw_files"
		   " WHERE FileBrowser IN (%u,%u,%u,%u,%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_CRS,
	           (unsigned) Brw_ADMI_TCH_CRS,
	           (unsigned) Brw_ADMI_SHR_CRS,
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_WRK_USR,
	           (unsigned) Brw_ADMI_MRK_CRS,
	           CrsCod);

   /* Remove from group file zones */
   DB_QueryDELETE ("can not remove files of a course",
		   "DELETE FROM brw_files"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		     " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           SubqueryGrp);

   /* Remove from project file zones */
   DB_QueryDELETE ("can not remove files of a course",
		   "DELETE FROM brw_files"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           SubqueryPrj);
  }

/*****************************************************************************/
/************ Remove files related to a group from the database **************/
/*****************************************************************************/

void Brw_DB_RemoveGrpFiles (long GrpCod)
  {
   /***** Remove format of files of marks *****/
   DB_QueryDELETE ("can not remove the properties of marks"
		   " associated to a group",
		   "DELETE FROM mrk_marks"
		   " USING brw_files,"
		          "mrk_marks"
		   " WHERE brw_files.FileBrowser=%u"
		     " AND brw_files.Cod=%ld"
		     " AND brw_files.FilCod=mrk_marks.FilCod",
	           (unsigned) Brw_ADMI_MRK_GRP,
	           GrpCod);

   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views to files of a group",
		   "DELETE FROM brw_views"
		   " USING brw_files,"
		          "brw_views"
		   " WHERE brw_files.FileBrowser IN (%u,%u,%u,%u)"
		     " AND brw_files.Cod=%ld"
		     " AND brw_files.FilCod=brw_views.FilCod",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           GrpCod);

   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders of a group",
		   "DELETE LOW_PRIORITY FROM brw_expanded"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		   " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           GrpCod);

   /***** Remove from database the entries that store clipboards *****/
   DB_QueryDELETE ("can not remove clipboards related to files of a group",
		   "DELETE FROM brw_clipboards"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           GrpCod);

   /***** Remove from database the entries that store the last time users visited file zones *****/
   DB_QueryDELETE ("can not remove file last visits to files of a group",
		   "DELETE FROM brw_last"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		   " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           GrpCod);

   /***** Remove from database the entries that store the sizes of the file zones *****/
   DB_QueryDELETE ("can not remove sizes of file zones of a group",
		   "DELETE FROM brw_sizes"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		   " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           GrpCod);

   /***** Remove from database the entries that store the data files *****/
   DB_QueryDELETE ("can not remove files of a group",
		   "DELETE FROM brw_files"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           GrpCod);
  }

/*****************************************************************************/
/*********** Remove files related to a project from the database *************/
/*****************************************************************************/

void Brw_DB_RemovePrjFiles (long PrjCod)
  {
   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views to files of a project",
		   "DELETE FROM brw_views"
		   " USING brw_files,"
		          "brw_views"
		   " WHERE brw_files.FileBrowser IN (%u,%u)"
		     " AND brw_files.Cod=%ld"
		     " AND brw_files.FilCod=brw_views.FilCod",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           PrjCod);

   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders of a project",
		   "DELETE LOW_PRIORITY FROM brw_expanded"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           PrjCod);

   /***** Remove from database the entries that store clipboards *****/
   DB_QueryDELETE ("can not remove clipboards related to files of a project",
		   "DELETE FROM brw_clipboards"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           PrjCod);

   /***** Remove from database the entries that store the last time users visited file zones *****/
   DB_QueryDELETE ("can not remove file last visits to files of a project",
		   "DELETE FROM brw_last"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           PrjCod);

   /***** Remove from database the entries that store the sizes of the file zones *****/
   DB_QueryDELETE ("can not remove sizes of file zones of a project",
		   "DELETE FROM brw_sizes"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           PrjCod);

   /***** Remove from database the entries that store the data files *****/
   DB_QueryDELETE ("can not remove files of a project",
		   "DELETE FROM brw_files"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           PrjCod);
  }

/*****************************************************************************/
/* Remove some info about files related to a course and a user from database */
/*****************************************************************************/

void Brw_DB_RemoveSomeInfoAboutCrsUsrFiles (long UsrCod,long CrsCod)
  {
   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders for a user in a course",
		   "DELETE LOW_PRIORITY FROM brw_expanded"
		   " WHERE UsrCod=%ld"
		     " AND ("
			    "(FileBrowser IN (%u,%u,%u,%u,%u,%u,%u,%u)"
			    " AND Cod=%ld)"
			    " OR "
			    "(FileBrowser IN (%u,%u,%u,%u)"
			    " AND Cod IN"
			    " (SELECT grp_groups.GrpCod"
			       " FROM grp_types,"
			             "grp_groups"
			      " WHERE grp_types.CrsCod=%ld"
			        " AND grp_types.GrpTypCod=grp_groups.GrpTypCod))"
		          ")",
	           UsrCod,
	           (unsigned) Brw_ADMI_DOC_CRS,
	           (unsigned) Brw_ADMI_TCH_CRS,
	           (unsigned) Brw_ADMI_SHR_CRS,
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_ASG_CRS,
	           (unsigned) Brw_ADMI_WRK_USR,
	           (unsigned) Brw_ADMI_WRK_CRS,
	           (unsigned) Brw_ADMI_MRK_CRS,
	           CrsCod,
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           CrsCod);

   /***** Remove from database the entries that store clipboards *****/
   DB_QueryDELETE ("can not remove source of copy for a user in a course",
		   "DELETE FROM brw_clipboards"
		   " WHERE UsrCod=%ld"
		     " AND ("
			    "(FileBrowser IN (%u,%u,%u,%u,%u,%u,%u,%u)"
			    " AND Cod=%ld)"
			    " OR "
			    "(FileBrowser IN (%u,%u,%u,%u)"
			    " AND Cod IN"
			    " (SELECT grp_groups.GrpCod"
			       " FROM grp_types,"
			             "grp_groups"
			      " WHERE grp_types.CrsCod=%ld"
			        " AND grp_types.GrpTypCod=grp_groups.GrpTypCod))"
		          ")",
	           UsrCod,
	           (unsigned) Brw_ADMI_DOC_CRS,
	           (unsigned) Brw_ADMI_TCH_CRS,
	           (unsigned) Brw_ADMI_SHR_CRS,
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_ASG_CRS,
	           (unsigned) Brw_ADMI_WRK_USR,
	           (unsigned) Brw_ADMI_WRK_CRS,
	           (unsigned) Brw_ADMI_MRK_CRS,
	           CrsCod,
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           CrsCod);

   /***** Remove from database the entries that store the last time user visited file zones *****/
   // Assignments and works are stored as one in brw_last...
   // ...because a user views them at the same time
   DB_QueryDELETE ("can not remove file last visits to files of a course"
		   " from a user",
		   "DELETE FROM brw_last"
		   " WHERE UsrCod=%ld"
		     " AND ("
			    "(FileBrowser IN (%u,%u,%u,%u,%u)"
			    " AND Cod=%ld)"
			    " OR "
			    "(FileBrowser IN (%u,%u,%u,%u)"
			    " AND Cod IN"
			    " (SELECT grp_groups.GrpCod"
			       " FROM grp_types,"
			             "grp_groups"
			      " WHERE grp_types.CrsCod=%ld"
			        " AND grp_types.GrpTypCod=grp_groups.GrpTypCod))"
		          ")",
	           UsrCod,
	           (unsigned) Brw_ADMI_DOC_CRS,
	           (unsigned) Brw_ADMI_TCH_CRS,
	           (unsigned) Brw_ADMI_SHR_CRS,
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_MRK_CRS,
	           CrsCod,
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           CrsCod);
  }

/*****************************************************************************/
/*************** Remove user's works in a course from database ***************/
/*****************************************************************************/

void Brw_DB_RemoveWrkFiles (long CrsCod,long UsrCod)
  {
   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views",
		   "DELETE FROM brw_views"
		   " USING brw_files,"
		          "brw_views"
		   " WHERE brw_files.FileBrowser IN (%u,%u)"
		     " AND brw_files.Cod=%ld"
		     " AND brw_files.ZoneUsrCod=%ld"
		     " AND brw_files.FilCod=brw_views.FilCod",
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_WRK_USR,
	           CrsCod,UsrCod);

   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders of a group",
		   "DELETE LOW_PRIORITY FROM brw_expanded"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld"
		     " AND WorksUsrCod=%ld",
	           (unsigned) Brw_ADMI_ASG_CRS,
	           (unsigned) Brw_ADMI_WRK_CRS,
	           CrsCod,UsrCod);

   /***** Remove from database the entries that store clipboards *****/
   DB_QueryDELETE ("can not remove clipboards",
		   "DELETE FROM brw_clipboards"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld"
		     " AND WorksUsrCod=%ld",
	           (unsigned) Brw_ADMI_ASG_CRS,
	           (unsigned) Brw_ADMI_WRK_CRS,
	           CrsCod,UsrCod);

   /***** Remove from database the entries that store the sizes of the file zones *****/
   DB_QueryDELETE ("can not remove file browser sizes",
		   "DELETE FROM brw_sizes"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld"
		     " AND ZoneUsrCod=%ld",
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_WRK_USR,
	           CrsCod,UsrCod);

   /***** Remove from database the entries that store the data files *****/
   DB_QueryDELETE ("can not remove files",
		   "DELETE FROM brw_files"
		   " WHERE FileBrowser IN (%u,%u)"
		     " AND Cod=%ld"
		     " AND ZoneUsrCod=%ld",
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_WRK_USR,
	           CrsCod,UsrCod);
  }

/*****************************************************************************/
/************* Remove files related to a user from the database **************/
/*****************************************************************************/

void Brw_DB_RemoveUsrFiles (long UsrCod)
  {
   /***** Remove from database the entries that store the file views *****/
   // User is not removed from brw_views table,
   // in order to take into account his/her views
   DB_QueryDELETE ("can not remove file views to files of a user",
		   "DELETE FROM brw_views"
		   " USING brw_files,"
		          "brw_views"
		   " WHERE brw_files.ZoneUsrCod=%ld"
		     " AND brw_files.FilCod=brw_views.FilCod",
	           UsrCod);

   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders for a user",
		   "DELETE LOW_PRIORITY FROM brw_expanded"
		   " WHERE UsrCod=%ld",
	           UsrCod);

   /***** Remove from database the entries that store clipboards *****/
   DB_QueryDELETE ("can not remove user's clipboards",
		   "DELETE FROM brw_clipboards"
		   " WHERE UsrCod=%ld",	// User's clipboard
	           UsrCod);

   /***** Remove from database the entries that store the last time users visited file zones *****/
   DB_QueryDELETE ("can not remove user's last visits to file zones",
		   "DELETE FROM brw_last"
		   " WHERE UsrCod=%ld",	// User's last visits to all zones
	           UsrCod);

   /***** Remove from database the entries that store the sizes of the file zones *****/
   DB_QueryDELETE ("can not remove sizes of user's file zones",
		   "DELETE FROM brw_sizes"
		   " WHERE ZoneUsrCod=%ld",
	           UsrCod);

   /***** Remove from database the entries that store the data files *****/
   DB_QueryDELETE ("can not remove files in user's file zones",
		   "DELETE FROM brw_files"
		   " WHERE ZoneUsrCod=%ld",
	           UsrCod);
  }

/*****************************************************************************/
/************ Change public and license of file in the database **************/
/*****************************************************************************/

void Brw_DB_ChangeFilePublic (const struct Brw_FileMetadata *FileMetadata,
                              bool IsPublic,Brw_License_t License)
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFileBrowser ();

   /***** Trivial check *****/
   if (FileMetadata->FilCod <= 0)
      return;

   /***** Change publisher, public and license of file in database *****/
   DB_QueryUPDATE ("can not change metadata of a file in database",
		   "UPDATE brw_files"
		     " SET Public='%c',"
		          "License=%u"
		   " WHERE FileBrowser=%u"
		     " AND Cod=%ld"
		     " AND ZoneUsrCod=%ld"
		     " AND FilCod=%ld"
		     " AND Path='%s'",
	           IsPublic ? 'Y' :
			      'N',
	           (unsigned) License,
	           (unsigned) Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type],
	           Cod,
	           ZoneUsrCod,
	           FileMetadata->FilCod,
	           FileMetadata->FilFolLnk.Full);
  }

/*****************************************************************************/
/*********** Check if a folder contains file(s) marked as public *************/
/*****************************************************************************/

bool Brw_DB_GetIfFolderHasPublicFiles (const char Path[PATH_MAX + 1])
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFileBrowser ();

   return
   DB_QueryEXISTS ("can not check if a folder contains public files",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM brw_files"
		    " WHERE FileBrowser=%u"
		      " AND Cod=%ld"
		      " AND ZoneUsrCod=%ld"
		      " AND Path LIKE '%s/%%'"
		      " AND Public='Y')",
		   (unsigned) Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type],
		   Cod,
		   ZoneUsrCod,
		   Path);
  }

/*****************************************************************************/
/********** Get current number of public files published by a user ***********/
/*****************************************************************************/

unsigned Brw_DB_GetNumPublicFilesUsr (long UsrCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of public files from a user",
		  "SELECT COUNT(*)"
		   " FROM brw_files"
		  " WHERE PublisherUsrCod=%ld"
		    " AND FileType IN (%u,%u)"
		    " AND Public='Y'",
		  UsrCod,
		  (unsigned) Brw_IS_FILE,
		  (unsigned) Brw_IS_UNKNOWN);	// Unknown entries are counted as files
  }

/*****************************************************************************/
/***************** Get number of OERs depending on license *******************/
/*****************************************************************************/

unsigned Brw_DB_GetNumberOfPublicFiles (MYSQL_RES **mysql_res,Brw_License_t License)
  {
   switch (Gbl.Scope.Current)
     {
      case HieLvl_SYS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of OERs",
			 "SELECT Public,"		// row[0]
			        "COUNT(*)"		// row[1]
			  " FROM brw_files"
			 " WHERE License=%u"
		      " GROUP BY Public",
			 (unsigned) License);
      case HieLvl_CTY:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of OERs",
			 "SELECT brw_files.Public,"	// row[0]
			        "COUNT(*)"		// row[1]
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "brw_files"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=brw_files.Cod"
			   " AND brw_files.FileBrowser IN (%u,%u)"
			   " AND brw_files.License=%u"
		      " GROUP BY brw_files.Public",
			 Gbl.Hierarchy.Node[HieLvl_CTY].Cod,
			 (unsigned) Brw_ADMI_DOC_CRS,
			 (unsigned) Brw_ADMI_SHR_CRS,
			 (unsigned) License);
      case HieLvl_INS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of OERs",
			 "SELECT brw_files.Public,"	// row[0]
			        "COUNT(*)"		// row[1]
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "brw_files"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=brw_files.Cod"
			   " AND brw_files.FileBrowser IN (%u,%u)"
			   " AND brw_files.License=%u"
		      " GROUP BY brw_files.Public",
			 Gbl.Hierarchy.Node[HieLvl_INS].Cod,
			 (unsigned) Brw_ADMI_DOC_CRS,
			 (unsigned) Brw_ADMI_SHR_CRS,
			 (unsigned) License);
      case HieLvl_CTR:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of OERs",
			 "SELECT brw_files.Public,"	// row[0]
			        "COUNT(*)"		// row[1]
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "brw_files"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=brw_files.Cod"
			   " AND brw_files.FileBrowser IN (%u,%u)"
			   " AND brw_files.License=%u"
		      " GROUP BY brw_files.Public",
			 Gbl.Hierarchy.Node[HieLvl_CTR].Cod,
			 (unsigned) Brw_ADMI_DOC_CRS,
			 (unsigned) Brw_ADMI_SHR_CRS,
			 (unsigned) License);
      case HieLvl_DEG:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of OERs",
			 "SELECT brw_files.Public,"	// row[0]
			        "COUNT(*)"		// row[1]
			  " FROM crs_courses,"
			        "brw_files"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=brw_files.Cod"
			   " AND brw_files.FileBrowser IN (%u,%u)"
			   " AND brw_files.License=%u"
		      " GROUP BY brw_files.Public",
			 Gbl.Hierarchy.Node[HieLvl_DEG].Cod,
			 (unsigned) Brw_ADMI_DOC_CRS,
			 (unsigned) Brw_ADMI_SHR_CRS,
			 (unsigned) License);
      case HieLvl_CRS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of OERs",
			 "SELECT Public,"		// row[0]
			        "COUNT(*)"		// row[1]
			  " FROM brw_files"
			 " WHERE Cod=%ld"
			   " AND FileBrowser IN (%u,%u)"
			   " AND License=%u"
		      " GROUP BY Public",
			 Gbl.Hierarchy.Node[HieLvl_CRS].Cod,
			 (unsigned) Brw_ADMI_DOC_CRS,
			 (unsigned) Brw_ADMI_SHR_CRS,
			 (unsigned) License);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/********************** Search open documents in database ********************/
/*****************************************************************************/
// Returns number of documents found

unsigned Brw_DB_SearchPublicFiles (MYSQL_RES **mysql_res,
                                   const char *RangeQuery,
                                   const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1])
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get files",
		   "SELECT *"
		    " FROM "
			  // Institution
			  "(SELECT brw_files.FilCod,"						// row[ 0]
				  "SUBSTRING(brw_files.Path,"
					    "LOCATE('/',brw_files.Path)) AS PathFromRoot,"	// row[ 1]
				  "ins_instits.InsCod,"						// row[ 2]
				  "ins_instits.ShortName AS InsShortName,"			// row[ 3]
				  "-1 AS CtrCod,"						// row[ 4]
				  "'' AS CtrShortName,"						// row[ 5]
				  "-1 AS DegCod,"						// row[ 6]
				  "'' AS DegShortName,"						// row[ 7]
				  "-1 AS CrsCod,"						// row[ 8]
				  "'' AS CrsShortName,"						// row[ 9]
				  "-1 AS GrpCod"						// row[10]
			    " FROM brw_files,"
				  "crs_courses,"
				  "deg_degrees,"
				  "ctr_centers,"
				  "ins_instits,"
				  "cty_countrs"
			   " WHERE brw_files.Public='Y'"
			     " AND %s"
			     " AND brw_files.FileBrowser IN (%u,%u)"
			     " AND brw_files.Cod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
			     "%s"
			   " UNION "
			   // Center -------------------------------------
			   "SELECT brw_files.FilCod,"						// row[ 0]
				  "SUBSTRING(brw_files.Path,"
					    "LOCATE('/',brw_files.Path)) AS PathFromRoot,"	// row[ 1]
				  "ins_instits.InsCod,"						// row[ 2]
				  "ins_instits.ShortName AS InsShortName,"			// row[ 3]
				  "ctr_centers.CtrCod,"						// row[ 4]
				  "ctr_centers.ShortName AS CtrShortName,"			// row[ 5]
				  "-1 AS DegCod,"						// row[ 6]
				  "'' AS DegShortName,"						// row[ 7]
				  "-1 AS CrsCod,"						// row[ 8]
				  "'' AS CrsShortName,"						// row[ 9]
				  "-1 AS GrpCod"						// row[10]
			    " FROM brw_files,"
				  "crs_courses,"
				  "deg_degrees,"
				  "ctr_centers,"
				  "ins_instits,"
				  "cty_countrs"
			   " WHERE brw_files.Public='Y' AND %s"
			     " AND brw_files.FileBrowser IN (%u,%u)"
			     " AND brw_files.Cod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
			     "%s"
			   " UNION "
			   // Degree -------------------------------------
			   "SELECT brw_files.FilCod,"						// row[ 0]
				  "SUBSTRING(brw_files.Path,"
					    "LOCATE('/',brw_files.Path)) AS PathFromRoot,"	// row[ 1]
				  "ins_instits.InsCod,"						// row[ 2]
				  "ins_instits.ShortName AS InsShortName,"			// row[ 3]
				  "ctr_centers.CtrCod,"						// row[ 4]
				  "ctr_centers.ShortName AS CtrShortName,"			// row[ 5]
				  "deg_degrees.DegCod,"						// row[ 6]
				  "deg_degrees.ShortName AS DegShortName,"			// row[ 7]
				  "-1 AS CrsCod,"						// row[ 8]
				  "'' AS CrsShortName,"						// row[ 9]
				  "-1 AS GrpCod"						// row[10]
			    " FROM brw_files,"
				  "crs_courses,"
				  "deg_degrees,"
				  "ctr_centers,"
				  "ins_instits,"
				  "cty_countrs"
			   " WHERE brw_files.Public='Y' AND %s"
			     " AND brw_files.FileBrowser IN (%u,%u)"
			     " AND brw_files.Cod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
			     "%s"
			   " UNION "
			   // Course -------------------------------------
			   "SELECT brw_files.FilCod,"						// row[ 0]
				  "SUBSTRING(brw_files.Path,"
					    "LOCATE('/',brw_files.Path)) AS PathFromRoot,"	// row[ 1]
				  "ins_instits.InsCod,"						// row[ 2]
				  "ins_instits.ShortName AS InsShortName,"			// row[ 3]
				  "ctr_centers.CtrCod,"						// row[ 4]
				  "ctr_centers.ShortName AS CtrShortName,"			// row[ 5]
				  "deg_degrees.DegCod,"						// row[ 6]
				  "deg_degrees.ShortName AS DegShortName,"			// row[ 7]
				  "crs_courses.CrsCod,"						// row[ 8]
				  "crs_courses.ShortName AS CrsShortName,"			// row[ 9]
				  "-1 AS GrpCod"						// row[10]
			    " FROM brw_files,"
				  "crs_courses,"
				  "deg_degrees,"
				  "ctr_centers,"
				  "ins_instits,"
				  "cty_countrs"
			   " WHERE brw_files.Public='Y' AND %s"
			     " AND brw_files.FileBrowser IN (%u,%u)"
			     " AND brw_files.Cod=crs_courses.CrsCod"
			     " AND crs_courses.DegCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
			     "%s"
			  ") AS selected_files"
		   " WHERE PathFromRoot<>''"
		   " ORDER BY InsShortName,"
			     "CtrShortName,"
			     "DegShortName,"
			     "CrsShortName,"
			     "PathFromRoot",
		   SearchQuery,
		   (unsigned) Brw_ADMI_DOC_INS,
		   (unsigned) Brw_ADMI_SHR_INS,
		   RangeQuery,
		   SearchQuery,
		   (unsigned) Brw_ADMI_DOC_CTR,
		   (unsigned) Brw_ADMI_SHR_CTR,
		   RangeQuery,
		   SearchQuery,
		   (unsigned) Brw_ADMI_DOC_DEG,
		   (unsigned) Brw_ADMI_SHR_DEG,
		   RangeQuery,
		   SearchQuery,
		   (unsigned) Brw_ADMI_DOC_CRS,
		   (unsigned) Brw_ADMI_SHR_CRS,
		   RangeQuery);
  }

/*****************************************************************************/
/**************** Search documents in my courses in database *****************/
/*****************************************************************************/

unsigned Brw_DB_SearchFilesInMyCrss (MYSQL_RES **mysql_res,
                                     const char *RangeQuery,
                                     const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1])
  {
   unsigned NumFiles;

   /***** Create temporary tables with codes of files in documents and shared areas accessible by me.
	  It is necessary to speed up the second query *****/
   DB_DropTmpTable ("my_files_crs");
   DB_DropTmpTable ("my_files_grp");

   DB_CreateTmpTable ("CREATE TEMPORARY TABLE my_files_crs"
		      " (FilCod INT NOT NULL,"
		      "UNIQUE INDEX(FilCod))"
		      " ENGINE=MEMORY"
		      " SELECT brw_files.FilCod"
		        " FROM crs_users,"
			      "brw_files"
		       " WHERE crs_users.UsrCod=%ld"
		         " AND crs_users.CrsCod=brw_files.Cod"
		         " AND brw_files.FileBrowser IN (%u,%u,%u,%u)",
		      Gbl.Usrs.Me.UsrDat.UsrCod,
		      (unsigned) Brw_ADMI_DOC_CRS,
		      (unsigned) Brw_ADMI_TCH_CRS,
		      (unsigned) Brw_ADMI_SHR_CRS,
		      (unsigned) Brw_ADMI_MRK_CRS);

   DB_CreateTmpTable ("CREATE TEMPORARY TABLE my_files_grp"
		      " (FilCod INT NOT NULL,"
		        "UNIQUE INDEX(FilCod))"
		      " ENGINE=MEMORY"
		      " SELECT brw_files.FilCod"
		        " FROM grp_users,"
			      "brw_files"
		       " WHERE grp_users.UsrCod=%ld"
		         " AND grp_users.GrpCod=brw_files.Cod"
		         " AND brw_files.FileBrowser IN (%u,%u,%u,%u)",
		      Gbl.Usrs.Me.UsrDat.UsrCod,
		      (unsigned) Brw_ADMI_DOC_GRP,
		      (unsigned) Brw_ADMI_TCH_GRP,
		      (unsigned) Brw_ADMI_SHR_GRP,
		      (unsigned) Brw_ADMI_MRK_GRP);

   /***** Build the query *****/
   NumFiles = (unsigned)
   DB_QuerySELECT (mysql_res,"can not get files",
		   "SELECT *"
		    " FROM ("
			   // Files in course zones ----------------------
			   "SELECT brw_files.FilCod,"					// row[ 0]
				  "SUBSTRING(brw_files.Path,"
					    "LOCATE('/',"
						   "brw_files.Path)) AS PathFromRoot,"	// row[ 1]
				  "ins_instits.InsCod,"					// row[ 2]
				  "ins_instits.ShortName AS InsShortName,"		// row[ 3]
				  "ctr_centers.CtrCod,"					// row[ 4]
				  "ctr_centers.ShortName AS CtrShortName,"		// row[ 5]
				  "deg_degrees.DegCod,"					// row[ 6]
				  "deg_degrees.ShortName AS DegShortName,"		// row[ 7]
				   "crs_courses.CrsCod,"				// row[ 8]
				  "crs_courses.ShortName AS CrsShortName,"		// row[ 9]
				  "-1 AS GrpCod"					// row[10]
			    " FROM brw_files,"
				  "crs_courses,"
				  "deg_degrees,"
				  "ctr_centers,"
				  "ins_instits,"
				  "cty_countrs"
			   " WHERE brw_files.FilCod IN"
				 " (SELECT FilCod"
				    " FROM my_files_crs)"
			     " AND %s"
			     " AND brw_files.FileBrowser IN (%u,%u,%u,%u)"
			     " AND brw_files.Cod=crs_courses.CrsCod"
			     " AND crs_courses.DegCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
			     "%s"
			   " UNION "
			   // Files in group zones -----------------------
			   "SELECT brw_files.FilCod,"					// row[ 0]
				  "SUBSTRING(brw_files.Path,"
					    "LOCATE('/',"
						   "brw_files.Path)) AS PathFromRoot,"	// row[ 1]
				  "ins_instits.InsCod,"					// row[ 2]
				  "ins_instits.ShortName AS InsShortName,"		// row[ 3]
				  "ctr_centers.CtrCod,"					// row[ 4]
				  "ctr_centers.ShortName AS CtrShortName,"		// row[ 5]
				  "deg_degrees.DegCod,"					// row[ 6]
				  "deg_degrees.ShortName AS DegShortName,"		// row[ 7]
				  "crs_courses.CrsCod,"					// row[ 8]
				  "crs_courses.ShortName AS CrsShortName,"		// row[ 9]
				  "grp_groups.GrpCod"					// row[10]
			    " FROM brw_files,"
				  "grp_groups,"
				  "grp_types,"
				  "crs_courses,"
				  "deg_degrees,"
				  "ctr_centers,"
				  "ins_instits,"
				  "cty_countrs"
			   " WHERE brw_files.FilCod IN"
				 " (SELECT FilCod"
				    " FROM my_files_grp)"
			     " AND %s"
			     " AND brw_files.FileBrowser IN (%u,%u,%u,%u)"
			     " AND brw_files.Cod=grp_groups.GrpCod"
			     " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
			     " AND grp_types.CrsCod=crs_courses.CrsCod"
			     " AND crs_courses.DegCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
			     "%s"
			  ") AS selected_files"
		   " WHERE PathFromRoot<>''"
		   " ORDER BY InsShortName,"
			     "CtrShortName,"
			     "DegShortName,"
			     "CrsShortName,"
			     "PathFromRoot",
		   SearchQuery,
		   (unsigned) Brw_ADMI_DOC_CRS,
		   (unsigned) Brw_ADMI_TCH_CRS,
		   (unsigned) Brw_ADMI_SHR_CRS,
		   (unsigned) Brw_ADMI_MRK_CRS,
		   RangeQuery,
		   SearchQuery,
		   (unsigned) Brw_ADMI_DOC_GRP,
		   (unsigned) Brw_ADMI_TCH_GRP,
		   (unsigned) Brw_ADMI_SHR_GRP,
		   (unsigned) Brw_ADMI_MRK_GRP,
		   RangeQuery);

   /***** Drop temporary tables *****/
   DB_DropTmpTable ("my_files_crs");
   DB_DropTmpTable ("my_files_grp");

   return NumFiles;
  }

/*****************************************************************************/
/********************** Search my documents in database **********************/
/*****************************************************************************/
// Returns number of documents found

unsigned Brw_DB_SearchMyFiles (MYSQL_RES **mysql_res,
                               const char *RangeQuery,
                               const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1])
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get files",
		   "SELECT *"
		    " FROM ("
			   // Institution --------------------------------
			   "SELECT brw_files.FilCod,"					// row[ 0]
				  "SUBSTRING(brw_files.Path,"
					    "LOCATE('/',"
						   "brw_files.Path)) AS PathFromRoot,"	// row[ 1]
				  "ins_instits.InsCod,"					// row[ 2]
				  "ins_instits.ShortName AS InsShortName,"		// row[ 3]
				  "-1 AS CtrCod,"					// row[ 4]
				  "'' AS CtrShortName,"					// row[ 5]
				  "-1 AS DegCod,"					// row[ 6]
				  "'' AS DegShortName,"					// row[ 7]
				  "-1 AS CrsCod,"					// row[ 8]
				  "'' AS CrsShortName,"					// row[ 9]
				  "-1 AS GrpCod"					// row[10]
			    " FROM brw_files,"
				  "crs_courses,"
				  "deg_degrees,"
				  "ctr_centers,"
				  "ins_instits,"
				  "cty_countrs"
			   " WHERE brw_files.PublisherUsrCod=%ld"
			     " AND %s"
			     " AND brw_files.FileBrowser IN (%u,%u)"
			     " AND brw_files.Cod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
			     "%s"
			   " UNION "
			   // Center -------------------------------------
			   "SELECT brw_files.FilCod,"					// row[ 0]
				  "SUBSTRING(brw_files.Path,"
					    "LOCATE('/',"
						   "brw_files.Path)) AS PathFromRoot,"	// row[ 1]
				  "ins_instits.InsCod,"					// row[ 2]
				  "ins_instits.ShortName AS InsShortName,"		// row[ 3]
				  "ctr_centers.CtrCod,"					// row[ 4]
				  "ctr_centers.ShortName AS CtrShortName,"		// row[ 5]
				  "-1 AS DegCod,"					// row[ 6]
				  "'' AS DegShortName,"					// row[ 7]
				  "-1 AS CrsCod,"					// row[ 8]
				  "'' AS CrsShortName,"					// row[ 9]
				  "-1 AS GrpCod"					// row[10]
			    " FROM brw_files,"
				  "crs_courses,"
				  "deg_degrees,"
				  "ctr_centers,"
				  "ins_instits,"
				  "cty_countrs"
			   " WHERE brw_files.PublisherUsrCod=%ld AND %s"
			     " AND brw_files.FileBrowser IN (%u,%u)"
			     " AND brw_files.Cod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
			     "%s"
			   " UNION "
			   // Degree -------------------------------------
			   "SELECT brw_files.FilCod,"					// row[ 0]
				  "SUBSTRING(brw_files.Path,"
					    "LOCATE('/',"
						   "brw_files.Path)) AS PathFromRoot,"	// row[ 1]
				  "ins_instits.InsCod,"					// row[ 2]
				  "ins_instits.ShortName AS InsShortName,"		// row[ 3]
				  "ctr_centers.CtrCod,"					// row[ 4]
				  "ctr_centers.ShortName AS CtrShortName,"		// row[ 5]
				  "deg_degrees.DegCod,"					// row[ 6]
				  "deg_degrees.ShortName AS DegShortName,"		// row[ 7]
				  "-1 AS CrsCod,"					// row[ 8]
				  "'' AS CrsShortName,"					// row[ 9]
				  "-1 AS GrpCod"					// row[10]
			    " FROM brw_files,"
				  "crs_courses,"
				  "deg_degrees,"
				  "ctr_centers,"
				  "ins_instits,"
				  "cty_countrs"
			   " WHERE brw_files.PublisherUsrCod=%ld"
			     " AND %s"
			     " AND brw_files.FileBrowser IN (%u,%u)"
			     " AND brw_files.Cod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
			     "%s"
			   " UNION "
			   // Course -------------------------------------
			   "SELECT brw_files.FilCod,"					// row[ 0]
				  "SUBSTRING(brw_files.Path,"
					    "LOCATE('/',"
						   "brw_files.Path)) AS PathFromRoot,"	// row[ 1]
				  "ins_instits.InsCod,"					// row[ 2]
				  "ins_instits.ShortName AS InsShortName,"		// row[ 3]
				  "ctr_centers.CtrCod,"					// row[ 4]
				  "ctr_centers.ShortName AS CtrShortName,"		// row[ 5]
				  "deg_degrees.DegCod,"					// row[ 6]
				  "deg_degrees.ShortName AS DegShortName,"		// row[ 7]
				  "crs_courses.CrsCod,"					// row[ 8]
				  "crs_courses.ShortName AS CrsShortName,"		// row[ 9]
				  "-1 AS GrpCod"					// row[10]
			    " FROM brw_files,"
				  "crs_courses,"
				  "deg_degrees,"
				  "ctr_centers,"
				  "ins_instits,"
				  "cty_countrs"
			   " WHERE brw_files.PublisherUsrCod=%ld"
			     " AND %s"
			     " AND brw_files.FileBrowser IN (%u,%u,%u,%u)"
			     " AND brw_files.Cod=crs_courses.CrsCod"
			     " AND crs_courses.DegCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
			     "%s"
			   " UNION "
			   // Group --------------------------------------
			   "SELECT brw_files.FilCod,"					// row[ 0]
				  "SUBSTRING(brw_files.Path,"
					    "LOCATE('/',"
						   "brw_files.Path)) AS PathFromRoot,"	// row[ 1]
				  "ins_instits.InsCod,"					// row[ 2]
				  "ins_instits.ShortName AS InsShortName,"		// row[ 3]
				  "ctr_centers.CtrCod,"					// row[ 4]
				  "ctr_centers.ShortName AS CtrShortName,"		// row[ 5]
				  "deg_degrees.DegCod,"					// row[ 6]
				  "deg_degrees.ShortName AS DegShortName,"		// row[ 7]
				  "crs_courses.CrsCod,"					// row[ 8]
				  "crs_courses.ShortName AS CrsShortName,"		// row[ 9]
				  "grp_groups.GrpCod"					// row[10]
			    " FROM brw_files,"
				  "grp_groups,"
				  "grp_types,"
				  "crs_courses,"
				  "deg_degrees,"
				  "ctr_centers,"
				  "ins_instits,"
				  "cty_countrs"
			   " WHERE brw_files.PublisherUsrCod=%ld"
			     " AND %s"
			     " AND brw_files.FileBrowser IN (%u,%u,%u,%u)"
			     " AND brw_files.Cod=grp_groups.GrpCod"
			     " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
			     " AND grp_types.CrsCod=crs_courses.CrsCod"
			     " AND crs_courses.DegCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
			     "%s"
			   " UNION "
			   // Briefcase ----------------------------------
			   "SELECT brw_files.FilCod,"					// row[ 0]
				  "SUBSTRING(brw_files.Path,"
					    "LOCATE('/',"
						   "brw_files.Path)) AS PathFromRoot,"	// row[ 1]
				  "-1 AS InsCod,"					// row[ 2]
				  "'' AS InsShortName,"					// row[ 3]
				  "-1 AS CtrCod,"					// row[ 4]
				  "'' AS CtrShortName,"					// row[ 5]
				  "-1 AS DegCod,"					// row[ 6]
				  "'' AS DegShortName,"					// row[ 7]
				  "-1 AS CrsCod,"					// row[ 8]
				  "'' AS CrsShortName,"					// row[ 9]
				  "-1 AS GrpCod"					// row[10]
			    " FROM brw_files"
			   " WHERE brw_files.PublisherUsrCod=%ld"
			     " AND %s"
			     " AND brw_files.FileBrowser=%u"
			  ") AS selected_files"
		   " WHERE PathFromRoot<>''"
		   " ORDER BY InsShortName,"
			     "CtrShortName,"
			     "DegShortName,"
			     "CrsShortName,"
			     "PathFromRoot",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   SearchQuery,
		   (unsigned) Brw_ADMI_DOC_INS,
		   (unsigned) Brw_ADMI_SHR_INS,
		   RangeQuery,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   SearchQuery,
		   (unsigned) Brw_ADMI_DOC_CTR,
		   (unsigned) Brw_ADMI_SHR_CTR,
		   RangeQuery,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   SearchQuery,
		   (unsigned) Brw_ADMI_DOC_DEG,
		   (unsigned) Brw_ADMI_SHR_DEG,
		   RangeQuery,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   SearchQuery,
		   (unsigned) Brw_ADMI_DOC_CRS,
		   (unsigned) Brw_ADMI_TCH_CRS,
		   (unsigned) Brw_ADMI_SHR_CRS,
		   (unsigned) Brw_ADMI_MRK_CRS,
		   RangeQuery,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   SearchQuery,
		   (unsigned) Brw_ADMI_DOC_GRP,
		   (unsigned) Brw_ADMI_TCH_GRP,
		   (unsigned) Brw_ADMI_SHR_GRP,
		   (unsigned) Brw_ADMI_MRK_GRP,
		   RangeQuery,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   SearchQuery,
		   (unsigned) Brw_ADMI_BRF_USR);
  }

/*****************************************************************************/
/*********************** Get folders of assignments **************************/
/*****************************************************************************/
// Get folder of an assignment when:
// 1. The assignment is visible (not hidden)
// 2. ...and the folder name is not empty (the teacher has set that the user must send work(s) for that assignment)
// 3. ...the assignment is not restricted to groups or (if restricted to groups), the owner of zone belong to any of the groups

unsigned Brw_DB_GetFoldersAssignments (MYSQL_RES **mysql_res,long ZoneUsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get folders of assignments",
		   "SELECT Folder"		// row[0]
		    " FROM asg_assignments"
		   " WHERE CrsCod=%ld"
		     " AND Hidden='N'"
		     " AND Folder<>''"
		     " AND ("
			   "AsgCod NOT IN"
			   " (SELECT AsgCod"
			      " FROM asg_groups)"
			   " OR "
			   "AsgCod IN"
			   " (SELECT asg_groups.AsgCod"
			      " FROM grp_users,"
				    "asg_groups"
			     " WHERE grp_users.UsrCod=%ld"
			       " AND asg_groups.GrpCod=grp_users.GrpCod)"
			  ")",
		   Gbl.Hierarchy.Node[HieLvl_CRS].Cod,
		   ZoneUsrCod);
  }

/*****************************************************************************/
/************ Update the date of my last access to file browser **************/
/*****************************************************************************/

void Brw_DB_UpdateDateMyLastAccFileBrowser (void)
  {
   long Cod = Brw_GetCodForFileBrowser ();

   DB_QueryREPLACE ("can not update date of last access to a file browser",
		    "REPLACE INTO brw_last"
		    " (UsrCod,FileBrowser,Cod,LastClick)"
		    " VALUES"
		    " (%ld,%u,%ld,NOW())",
	            Gbl.Usrs.Me.UsrDat.UsrCod,
	            (unsigned) Brw_DB_FileBrowserForDB_file_browser_last[Gbl.FileBrowser.Type],
	            Cod);
  }

/*****************************************************************************/
/************** Get the date of my last access to file browser ***************/
/*****************************************************************************/

unsigned Brw_DB_GetDateMyLastAccFileBrowser (MYSQL_RES **mysql_res)
  {
   long Cod = Brw_GetCodForFileBrowser ();

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get date-time"
			      " of last access to a file browser",
		   "SELECT UNIX_TIMESTAMP(LastClick)"	// row[0]
		    " FROM brw_last"
		   " WHERE UsrCod=%ld"
		     " AND FileBrowser=%u"
		     " AND Cod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) Brw_DB_FileBrowserForDB_file_browser_last[Gbl.FileBrowser.Type],
		   Cod);
  }

/*****************************************************************************/
/**************** Get the group of my last access to a zone ******************/
/*****************************************************************************/

unsigned Brw_DB_GetGrpLastAccFileBrowser (MYSQL_RES **mysql_res,const char *FieldNameDB)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get the group of your last access"
			      " to a file browser",
		   "SELECT %s"	// row[0]
		    " FROM crs_user_settings"
		   " WHERE UsrCod=%ld"
		     " AND CrsCod=%ld",
		   FieldNameDB,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Node[HieLvl_CRS].Cod);
  }

/*****************************************************************************/
/*************************** Update file views *******************************/
/*****************************************************************************/

void Brw_DB_UpdateFileViews (unsigned NumViews,long FilCod)
  {
   if (NumViews)
      /* Update number of views in database */
      DB_QueryUPDATE ("can not update number of views of a file",
		      "UPDATE brw_views"
		        " SET NumViews=NumViews+1"
		      " WHERE FilCod=%ld"
		        " AND UsrCod=%ld",
	              FilCod,
	              Gbl.Usrs.Me.UsrDat.UsrCod);
   else	// NumViews == 0
      /* Insert number of views in database */
      DB_QueryINSERT ("can not insert number of views of a file",
		      "INSERT INTO brw_views"
		      " (FilCod,UsrCod,NumViews)"
		      " VALUES"
		      " (%ld,%ld,1)",
		      FilCod,
		      Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************************** Get file views from me ***************************/
/*****************************************************************************/

unsigned Brw_DB_GetFileViewsFromMe (MYSQL_RES **mysql_res,long FilCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get your number of views of a file",
		   "SELECT NumViews"	// row[0]
		    " FROM brw_views"
		   " WHERE FilCod=%ld"
		     " AND UsrCod=%ld",
		   FilCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/******************** Get file views from logged users ***********************/
/*****************************************************************************/

unsigned Brw_DB_GetFileViewsFromLoggedUsrs (MYSQL_RES **mysql_res,long FilCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get number of views of a file"
			     " from logged users",
		   "SELECT COUNT(DISTINCT UsrCod),"	// row[0]
			  "SUM(NumViews)"		// row[1]
		    " FROM brw_views"
		   " WHERE FilCod=%ld"
		     " AND UsrCod>0",
		   FilCod);
  }

/*****************************************************************************/
/******************** Get number of public views of a file *******************/
/*****************************************************************************/

unsigned Brw_DB_GetFileViewsFromNonLoggedUsrs (long FilCod)
  {
   return
   DB_QuerySELECTUnsigned ("can not get number of public views of a file",
			   "SELECT SUM(NumViews)"
			    " FROM brw_views"
			   " WHERE FilCod=%ld"
			     " AND UsrCod<=0",
			   FilCod);
  }

/*****************************************************************************/
/******************** Get number of file views from a user *******************/
/*****************************************************************************/

unsigned Brw_DB_GetNumFileViewsUsr (long UsrCod)
  {
   return
   DB_QuerySELECTUnsigned ("can not get number of file views",
			   "SELECT SUM(NumViews)"
			    " FROM brw_views"
			   " WHERE UsrCod=%ld",
			   UsrCod);
  }

/*****************************************************************************/
/************************ Hide/unhide file or folder *************************/
/*****************************************************************************/

void Brw_DB_HideOrUnhideFileOrFolder (const char Path[PATH_MAX + 1],
				      HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char HidVis_YN[HidVis_NUM_HIDDEN_VISIBLE];
   long Cod = Brw_GetCodForFileBrowser ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFileBrowser ();

   /***** Mark file as hidden/unhidden in database *****/
   DB_QueryUPDATE ("can not hide/unhide file/folder",
		   "UPDATE brw_files"
		     " SET Hidden='%c'"
		   " WHERE FileBrowser=%u"
		     " AND Cod=%ld"
		     " AND ZoneUsrCod=%ld"
		     " AND Path='%s'",
		   HidVis_YN[HiddenOrVisible],
	           (unsigned) Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type],
	           Cod,
	           ZoneUsrCod,
	           Path);
  }

/*****************************************************************************/
/************** Check if a file / folder from is set as hidden ***************/
/*****************************************************************************/

unsigned Brw_DB_CheckIfFileOrFolderIsSetAsHiddenUsingPath (MYSQL_RES **mysql_res,
                                                           const char *Path)
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFileBrowser ();

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not check if a file is hidden",
		   "SELECT Hidden"		// row[0]
		    " FROM brw_files"
		   " WHERE FileBrowser=%u"
		     " AND Cod=%ld"
		     " AND ZoneUsrCod=%ld"
		     " AND Path='%s'"
		   " ORDER BY FilCod DESC"	// Due to errors, there could be old entries for the same path.
		   " LIMIT 1",			// Select the most recent entry.
		   (unsigned) Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type],
		   Cod,
		   ZoneUsrCod,
		   Path);
  }

/*****************************************************************************/
/******** Check if a file / folder from the documents zone is hidden *********/
/*****************************************************************************/

bool Brw_DB_CheckIfFileOrFolderIsSetAsHiddenUsingMetadata (const struct Brw_FileMetadata *FileMetadata)
  {
   /***** Get if a file or folder is under a hidden folder from database *****/
   /*
      The argument Path passed to this function is hidden if:
      1) the argument Path is exactly the same as a path stored in database
         or
      2) the argument Path begins by 'x/', where x is a path stored in database
   */
   return
   DB_QueryEXISTS ("can not check if a file or folder is hidden",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM brw_files"
		    " WHERE FileBrowser=%u"
		      " AND Cod=%ld"
		      " AND ZoneUsrCod=%ld"
		      " AND Hidden='Y'"
		      " AND (Path='%s'"
			   " OR"
			   " LOCATE(CONCAT(Path,'/'),'%s')=1))",
		   FileMetadata->FileBrowser,
		   FileMetadata->Cod,
		   FileMetadata->ZoneUsrCod,
		   FileMetadata->FilFolLnk.Full,
		   FileMetadata->FilFolLnk.Full);
  }

/*****************************************************************************/
/************************* Insert path in expanded folders *******************/
/*****************************************************************************/

void Brw_DB_InsertFolderInExpandedFolders (const char Path[PATH_MAX + 1])
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long WorksUsrCod = Brw_GetZoneUsrCodForFileBrowser ();

   // Path must be stored with final '/'
   DB_QueryINSERT ("can not expand the content of a folder",
		   "INSERT INTO brw_expanded"
		   " (UsrCod,FileBrowser,Cod,WorksUsrCod,Path,ClickTime)"
		   " VALUES"
		   " (%ld,%u,%ld,%ld,'%s/',NOW())",
	           Gbl.Usrs.Me.UsrDat.UsrCod,
	           (unsigned) Brw_DB_FileBrowserForDB_expanded_folders[Gbl.FileBrowser.Type],
	           Cod,
	           WorksUsrCod,
	           Path);
  }

/*****************************************************************************/
/******* Update paths of the current file browser in expanded folders ********/
/*****************************************************************************/

void Brw_DB_UpdateClickTimeOfThisFileBrowserInExpandedFolders (void)
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long WorksUsrCod = Brw_GetZoneUsrCodForFileBrowser ();
   Brw_FileBrowser_t FileBrowserForExpandedFolders = Brw_DB_FileBrowserForDB_expanded_folders[Gbl.FileBrowser.Type];

   if (Cod > 0)
     {
      if (WorksUsrCod > 0)
	 DB_QueryUPDATE ("can not update expanded folder",
			 "UPDATE brw_expanded"
			   " SET ClickTime=NOW()"
			 " WHERE UsrCod=%ld"
			   " AND FileBrowser=%u"
			   " AND Cod=%ld"
			   " AND WorksUsrCod=%ld",
		         Gbl.Usrs.Me.UsrDat.UsrCod,
		         (unsigned) FileBrowserForExpandedFolders,
		         Cod,
		         WorksUsrCod);
      else
	 DB_QueryUPDATE ("can not update expanded folder",
			 "UPDATE brw_expanded"
			   " SET ClickTime=NOW()"
			 " WHERE UsrCod=%ld"
			   " AND FileBrowser=%u"
			   " AND Cod=%ld",
		         Gbl.Usrs.Me.UsrDat.UsrCod,
		         (unsigned) FileBrowserForExpandedFolders,
		         Cod);
     }
   else	// Briefcase
      DB_QueryUPDATE ("can not update expanded folder",
		      "UPDATE brw_expanded"
		        " SET ClickTime=NOW()"
		      " WHERE UsrCod=%ld"
		        " AND FileBrowser=%u",
	              Gbl.Usrs.Me.UsrDat.UsrCod,
	              (unsigned) FileBrowserForExpandedFolders);
  }

/*****************************************************************************/
/************* Check if a folder from a file browser is expanded *************/
/*****************************************************************************/

bool Brw_DB_GetIfExpandedFolder (const char Path[PATH_MAX + 1])
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long WorksUsrCod = Brw_GetZoneUsrCodForFileBrowser ();
   Brw_FileBrowser_t FileBrowserForExpandedFolders = Brw_DB_FileBrowserForDB_expanded_folders[Gbl.FileBrowser.Type];

   if (Cod > 0)
     {
      if (WorksUsrCod > 0)
	 return
	 DB_QueryEXISTS ("can not check if a folder is expanded",
			 "SELECT EXISTS"
			 "(SELECT *"
			   " FROM brw_expanded"
			  " WHERE UsrCod=%ld"
			    " AND FileBrowser=%u"
			    " AND Cod=%ld"
			    " AND WorksUsrCod=%ld"
			    " AND Path='%s/')",
			 Gbl.Usrs.Me.UsrDat.UsrCod,
			 (unsigned) FileBrowserForExpandedFolders,
			 Cod,
			 WorksUsrCod,
			 Path);
      else
	 return
	 DB_QueryEXISTS ("can not check if a folder is expanded",
			 "SELECT EXISTS"
			 "(SELECT *"
			   " FROM brw_expanded"
			  " WHERE UsrCod=%ld"
			    " AND FileBrowser=%u"
			    " AND Cod=%ld"
			    " AND Path='%s/')",
			 Gbl.Usrs.Me.UsrDat.UsrCod,
			 (unsigned) FileBrowserForExpandedFolders,
			 Cod,
			 Path);
     }
   else	// Briefcase
      return
      DB_QueryEXISTS ("can not check if a folder is expanded",
		      "SELECT EXISTS"
		      "(SELECT *"
			" FROM brw_expanded"
		       " WHERE UsrCod=%ld"
			 " AND FileBrowser=%u"
			 " AND Path='%s/')",
		      Gbl.Usrs.Me.UsrDat.UsrCod,
		      (unsigned) FileBrowserForExpandedFolders,
		      Path);
  }

/*****************************************************************************/
/********************** Remove path from expanded folders ********************/
/*****************************************************************************/

void Brw_DB_RemoveFolderFromExpandedFolders (const char Path[PATH_MAX + 1])
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long WorksUsrCod = Brw_GetZoneUsrCodForFileBrowser ();
   Brw_FileBrowser_t FileBrowserForExpandedFolders = Brw_DB_FileBrowserForDB_expanded_folders[Gbl.FileBrowser.Type];

   if (Cod > 0)
     {
      if (WorksUsrCod > 0)
         DB_QueryDELETE ("can not contract the content of a folder",
			 "DELETE FROM brw_expanded"
			 " WHERE UsrCod=%ld"
			   " AND FileBrowser=%u"
			   " AND Cod=%ld"
			   " AND WorksUsrCod=%ld"
			   " AND Path='%s/'",
		         Gbl.Usrs.Me.UsrDat.UsrCod,
		         (unsigned) FileBrowserForExpandedFolders,
		         Cod,WorksUsrCod,Path);
      else
	 DB_QueryDELETE ("can not contract the content of a folder",
		         "DELETE FROM brw_expanded"
			 " WHERE UsrCod=%ld"
			   " AND FileBrowser=%u"
			   " AND Cod=%ld"
			   " AND Path='%s/'",
		         Gbl.Usrs.Me.UsrDat.UsrCod,
		         (unsigned) FileBrowserForExpandedFolders,
		         Cod,Path);
     }
   else	// Briefcase
      DB_QueryDELETE ("can not contract the content of a folder",
		      "DELETE FROM brw_expanded"
		      " WHERE UsrCod=%ld"
		        " AND FileBrowser=%u"
		        " AND Path='%s/'",
	              Gbl.Usrs.Me.UsrDat.UsrCod,
	              (unsigned) FileBrowserForExpandedFolders,
	              Path);
  }

/*****************************************************************************/
/***** Remove expanded folders with paths from a course or from a user *******/
/*****************************************************************************/

void Brw_DB_RemoveAffectedExpandedFolders (const char Path[PATH_MAX + 1])
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long WorksUsrCod = Brw_GetZoneUsrCodForFileBrowser ();
   Brw_FileBrowser_t FileBrowserForExpandedFolders = Brw_DB_FileBrowserForDB_expanded_folders[Gbl.FileBrowser.Type];

   if (Cod > 0)
     {
      if (WorksUsrCod > 0)
         DB_QueryDELETE ("can not remove expanded folders",
			 "DELETE FROM brw_expanded"
			 " WHERE UsrCod=%ld"
			   " AND FileBrowser=%u"
			   " AND Cod=%ld"
			   " AND WorksUsrCod=%ld"
			   " AND Path LIKE '%s/%%'",
		         Gbl.Usrs.Me.UsrDat.UsrCod,
		         (unsigned) FileBrowserForExpandedFolders,
		         Cod,
		         WorksUsrCod,
		         Path);
      else
         DB_QueryDELETE ("can not remove expanded folders",
			 "DELETE FROM brw_expanded"
			 " WHERE UsrCod=%ld"
			   " AND FileBrowser=%u"
			   " AND Cod=%ld"
			   " AND Path LIKE '%s/%%'",
		         Gbl.Usrs.Me.UsrDat.UsrCod,
		         (unsigned) FileBrowserForExpandedFolders,
		         Cod,
		         Path);
     }
   else	// Briefcase
      DB_QueryDELETE ("can not remove expanded folders",
		      "DELETE FROM brw_expanded"
		      " WHERE UsrCod=%ld"
		        " AND FileBrowser=%u"
		        " AND Path LIKE '%s/%%'",
		      Gbl.Usrs.Me.UsrDat.UsrCod,
		      (unsigned) FileBrowserForExpandedFolders,
		      Path);
  }

/*****************************************************************************/
/***** Remove expanded folders with paths from a course or from a user *******/
/*****************************************************************************/

void Brw_DB_RenameAffectedExpandedFolders (Brw_FileBrowser_t FileBrowser,
                                           long MyUsrCod,long WorksUsrCod,
                                           const char *OldPath,const char *NewPath)
  {
   long Cod = Brw_GetCodForFileBrowser ();
   Brw_FileBrowser_t FileBrowserForExpandedFolders = Brw_DB_FileBrowserForDB_expanded_folders[FileBrowser];
   unsigned StartFinalSubpathNotChanged = strlen (OldPath) + 2;

   if (Cod > 0)
     {
      if (MyUsrCod > 0)
	{
	 if (WorksUsrCod > 0)
	    DB_QueryUPDATE ("can not update expanded folders",
			    "UPDATE brw_expanded"
			      " SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
			    " WHERE UsrCod=%ld"
			      " AND FileBrowser=%u"
			      " AND Cod=%ld"
			      " AND WorksUsrCod=%ld"
			      " AND Path LIKE '%s/%%'",
		            NewPath,StartFinalSubpathNotChanged,
		            MyUsrCod,
		            (unsigned) FileBrowserForExpandedFolders,
		            Cod,
		            WorksUsrCod,
		            OldPath);
	 else
	    DB_QueryUPDATE ("can not update expanded folders",
			    "UPDATE brw_expanded"
			      " SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
			    " WHERE UsrCod=%ld"
			      " AND FileBrowser=%u"
			      " AND Cod=%ld"
			      " AND Path LIKE '%s/%%'",
		            NewPath,StartFinalSubpathNotChanged,
		            MyUsrCod,
		            (unsigned) FileBrowserForExpandedFolders,
		            Cod,
		            OldPath);
	}
      else	// MyUsrCod <= 0 means expanded folders for any user
	{
	 if (WorksUsrCod > 0)
	    DB_QueryUPDATE ("can not update expanded folders",
			    "UPDATE brw_expanded"
			      " SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
			    " WHERE FileBrowser=%u"
			      " AND Cod=%ld"
			      " AND WorksUsrCod=%ld"
			      " AND Path LIKE '%s/%%'",
		            NewPath,StartFinalSubpathNotChanged,
		            (unsigned) FileBrowserForExpandedFolders,
		            Cod,
		            WorksUsrCod,
		            OldPath);
	 else
	    DB_QueryUPDATE ("can not update expanded folders",
			    "UPDATE brw_expanded"
			      " SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
			    " WHERE FileBrowser=%u"
			      " AND Cod=%ld"
			      " AND Path LIKE '%s/%%'",
			    NewPath,StartFinalSubpathNotChanged,
			    (unsigned) FileBrowserForExpandedFolders,
			    Cod,
			    OldPath);
	}
     }
   else	// Briefcase
      DB_QueryUPDATE ("can not update expanded folders",
		      "UPDATE brw_expanded"
		        " SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
		      " WHERE UsrCod=%ld"
		        " AND FileBrowser=%u"
		        " AND Path LIKE '%s/%%'",
	              NewPath,StartFinalSubpathNotChanged,
	              MyUsrCod,
	              (unsigned) FileBrowserForExpandedFolders,
	              OldPath);
  }

/*****************************************************************************/
/************* Remove expired expanded folders (from all users) **************/
/*****************************************************************************/

void Brw_DB_RemoveExpiredExpandedFolders (void)
  {
   DB_QueryDELETE ("can not remove old expanded folders",
		   "DELETE LOW_PRIORITY FROM brw_expanded"
		   " WHERE ClickTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Cfg_TIME_TO_DELETE_BROWSER_EXPANDED_FOLDERS);
  }

/*****************************************************************************/
/***************************** Add path to clipboards ************************/
/*****************************************************************************/

void Brw_DB_AddPathToClipboards (void)
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long WorksUsrCod = Brw_GetZoneUsrCodForFileBrowser ();

   /***** Add path to clipboards *****/
   DB_QueryINSERT ("can not add source of copy to clipboard",
		   "INSERT INTO brw_clipboards"
		   " (UsrCod,FileBrowser,Cod,WorksUsrCod,FileType,Path)"
		   " VALUES"
		   " (%ld,%u,%ld,%ld,%u,'%s')",
	           Gbl.Usrs.Me.UsrDat.UsrCod,
	           (unsigned) Gbl.FileBrowser.Type,
	           Cod,
	           WorksUsrCod,
	           (unsigned) Gbl.FileBrowser.FilFolLnk.Type,
	           Gbl.FileBrowser.FilFolLnk.Full);
  }

/*****************************************************************************/
/************************** Update path in my clipboard **********************/
/*****************************************************************************/

void Brw_DB_UpdatePathInClipboard (void)
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long WorksUsrCod = Brw_GetZoneUsrCodForFileBrowser ();

   /***** Update path in my clipboard *****/
   DB_QueryUPDATE ("can not update source of copy in clipboard",
		   "UPDATE brw_clipboards"
		     " SET FileBrowser=%u,"
		          "Cod=%ld,"
		          "WorksUsrCod=%ld,"
		          "FileType=%u,"
		          "Path='%s'"
		   " WHERE UsrCod=%ld",
	           (unsigned) Gbl.FileBrowser.Type,
	           Cod,
	           WorksUsrCod,
	           (unsigned) Gbl.FileBrowser.FilFolLnk.Type,
	           Gbl.FileBrowser.FilFolLnk.Full,
	           Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************** Get data of my current clipboard *********************/
/*****************************************************************************/

unsigned Brw_DB_GetMyClipboard (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get source of copy from clipboard",
		   "SELECT FileBrowser,"	// row[0]
		          "Cod,"		// row[1]
		          "WorksUsrCod,"	// row[2]
		          "FileType,"		// row[3]
		          "Path"		// row[4]
		    " FROM brw_clipboards"
		   " WHERE UsrCod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/****************** Remove expired clipboards (from all users) ***************/
/*****************************************************************************/

void Brw_DB_RemoveExpiredClipboards (void)
  {
   DB_QueryDELETE ("can not remove old paths from clipboard",
		   "DELETE LOW_PRIORITY FROM brw_clipboards"
		   " WHERE CopyTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Cfg_TIME_TO_DELETE_BROWSER_CLIPBOARD);
  }

/*****************************************************************************/
/********* Remove clipboards with paths from a course or from a user *********/
/*****************************************************************************/

void Brw_DB_RemoveAffectedClipboards (Brw_FileBrowser_t FileBrowser,
                                      long MyUsrCod,long WorksUsrCod)
  {
   switch (FileBrowser)
     {
      case Brw_ADMI_DOC_INS:
      case Brw_ADMI_SHR_INS:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM brw_clipboards"
			 " WHERE FileBrowser=%u"
			   " AND Cod=%ld",
			 (unsigned) FileBrowser,
			 Gbl.Hierarchy.Node[HieLvl_INS].Cod);
         break;
      case Brw_ADMI_DOC_CTR:
      case Brw_ADMI_SHR_CTR:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM brw_clipboards"
			 " WHERE FileBrowser=%u"
			   " AND Cod=%ld",
                         (unsigned) FileBrowser,
                         Gbl.Hierarchy.Node[HieLvl_CTR].Cod);
         break;
      case Brw_ADMI_DOC_DEG:
      case Brw_ADMI_SHR_DEG:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM brw_clipboards"
			 " WHERE FileBrowser=%u"
			   " AND Cod=%ld",
                         (unsigned) FileBrowser,
                         Gbl.Hierarchy.Node[HieLvl_DEG].Cod);
         break;
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_TCH_CRS:
      case Brw_ADMI_SHR_CRS:
      case Brw_ADMI_MRK_CRS:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM brw_clipboards"
			 " WHERE FileBrowser=%u"
			   " AND Cod=%ld",
                         (unsigned) FileBrowser,
                         Gbl.Hierarchy.Node[HieLvl_CRS].Cod);
         break;
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_GRP:
      case Brw_ADMI_SHR_GRP:
      case Brw_ADMI_MRK_GRP:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM brw_clipboards"
			 " WHERE FileBrowser=%u"
			   " AND Cod=%ld",
                         (unsigned) FileBrowser,
                         Gbl.Crs.Grps.GrpCod);
         break;
      case Brw_ADMI_ASG_USR:
      case Brw_ADMI_WRK_USR:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM brw_clipboards"
			 " WHERE UsrCod=%ld"
			   " AND FileBrowser=%u"
			   " AND Cod=%ld",
                         MyUsrCod,(unsigned) FileBrowser,
                         Gbl.Hierarchy.Node[HieLvl_CRS].Cod);
         break;
      case Brw_ADMI_ASG_CRS:
      case Brw_ADMI_WRK_CRS:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM brw_clipboards"
			 " WHERE FileBrowser=%u"
			   " AND Cod=%ld"
			   " AND WorksUsrCod=%ld",
                         (unsigned) FileBrowser,
                         Gbl.Hierarchy.Node[HieLvl_CRS].Cod,WorksUsrCod);
         break;
      case Brw_ADMI_DOC_PRJ:
      case Brw_ADMI_ASS_PRJ:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM brw_clipboards"
			 " WHERE FileBrowser=%u"
			   " AND Cod=%ld",
                         (unsigned) FileBrowser,
                         Prj_GetPrjCod ());
	 break;
      case Brw_ADMI_BRF_USR:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM brw_clipboards"
			 " WHERE UsrCod=%ld"
			   " AND FileBrowser=%u",
                         MyUsrCod,(unsigned) FileBrowser);
         break;
      default:
         break;
     }
  }

/*****************************************************************************/
/*********************** Store size of a file zone ***************************/
/*****************************************************************************/

void Brw_DB_StoreSizeOfFileBrowser (const struct BrwSiz_BrowserSize *Size)
  {
   long Cod = Brw_GetCodForFileBrowser ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFileBrowser ();

   DB_QueryREPLACE ("can not store the size of a file zone",
		    "REPLACE INTO brw_sizes"
		    " (FileBrowser,Cod,ZoneUsrCod,"
		      "NumLevels,NumFolders,NumFiles,TotalSize)"
		    " VALUES"
		    " (%u,%ld,%ld,"
		      "%u,'%lu','%lu','%llu')",
	            (unsigned) Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type],
		    Cod,ZoneUsrCod,
	            Size->NumLevls,
	            Size->NumFolds,
	            Size->NumFiles,
	            Size->TotalSiz);
  }

/*****************************************************************************/
/*********************** Get the size of a file zone *************************/
/*****************************************************************************/

void Brw_DB_GetSizeOfFileBrowser (MYSQL_RES **mysql_res,
                                  Brw_FileBrowser_t FileBrowser)
  {
   switch (Gbl.Scope.Current)
     {
      /* Scope = the whole platform */
      case HieLvl_SYS:
	 switch (FileBrowser)
	   {
	    case Brw_UNKNOWN:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT CrsCod),"			// row[0]
				      "COUNT(DISTINCT GrpCod)-1,"		// row[1]
				      "-1,"					// row[2]
				      "MAX(NumLevels),"				// row[3]
				      "SUM(NumFolders),"			// row[4]
				      "SUM(NumFiles),"				// row[5]
				      "SUM(TotalSize)"				// row[6]
			        " FROM "
	                       "("
	                       "SELECT Cod AS CrsCod,"
				      "-1 AS GrpCod,"
				      "NumLevels,"
				      "NumFolders,"
				      "NumFiles,"
				      "TotalSize"
			       " FROM brw_sizes"
			       " WHERE FileBrowser IN (%u,%u,%u,%u,%u,%u)"
	                       " UNION "
	                       "SELECT grp_types.CrsCod,"
				      "brw_sizes.Cod AS GrpCod,"
				      "brw_sizes.NumLevels,"
				      "brw_sizes.NumFolders,"
				      "brw_sizes.NumFiles,"
				      "brw_sizes.TotalSize"
			        " FROM grp_types,"
			              "grp_groups,"
			              "brw_sizes"
			       " WHERE grp_types.GrpTypCod=grp_groups.GrpTypCod"
			         " AND grp_groups.GrpCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser IN (%u,%u,%u,%u)"
			       ") AS sizes",
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       (unsigned) Brw_ADMI_DOC_GRP,
			       (unsigned) Brw_ADMI_TCH_GRP,
			       (unsigned) Brw_ADMI_SHR_GRP,
			       (unsigned) Brw_ADMI_MRK_GRP);
	       break;
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_MRK_CRS:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT Cod),"			// row[0]
				      "-1,"					// row[1]
				      "-1,"					// row[2]
				      "MAX(NumLevels),"				// row[3]
				      "SUM(NumFolders),"			// row[4]
				      "SUM(NumFiles),"				// row[5]
				      "SUM(TotalSize)"				// row[6]
			        " FROM brw_sizes"
			       " WHERE FileBrowser=%u",
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_DOC_GRP:
	    case Brw_ADMI_TCH_GRP:
	    case Brw_ADMI_SHR_GRP:
	    case Brw_ADMI_MRK_GRP:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT grp_types.CrsCod),"	// row[0]
				      "COUNT(DISTINCT brw_sizes.Cod),"		// row[1]
				      "-1,"					// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM grp_types,"
			              "grp_groups,"
			              "brw_sizes"
			       " WHERE grp_types.GrpTypCod=grp_groups.GrpTypCod"
			         " AND grp_groups.GrpCod=brw_sizes.Cod"
	                         " AND brw_sizes.FileBrowser=%u",
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_ASG_USR:
	    case Brw_ADMI_WRK_USR:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT Cod),"			// row[0]
				      "-1,"					// row[1]
				      "COUNT(DISTINCT ZoneUsrCod),"		// row[2]
				      "MAX(NumLevels),"				// row[3]
				      "SUM(NumFolders),"			// row[4]
				      "SUM(NumFiles),"				// row[5]
				      "SUM(TotalSize)"				// row[6]
			        " FROM brw_sizes"
			       " WHERE FileBrowser=%u",
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_BRF_USR:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT -1,"					// row[0]
				      "-1,"					// row[1]
				      "COUNT(DISTINCT ZoneUsrCod),"		// row[2]
				      "MAX(NumLevels),"				// row[3]
				      "SUM(NumFolders),"			// row[4]
				      "SUM(NumFiles),"				// row[5]
				      "SUM(TotalSize)"				// row[6]
			        " FROM brw_sizes"
			       " WHERE FileBrowser=%u",
			       (unsigned) FileBrowser);
	       break;
	    default:
	       Err_WrongFileBrowserExit ();
	       break;
	   }
         break;
      /* Scope = the current country */
      case HieLvl_CTY:
	 switch (FileBrowser)
	   {
	    case Brw_UNKNOWN:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT CrsCod),"			// row[0]
				      "COUNT(DISTINCT GrpCod)-1,"		// row[1]
				      "-1,"					// row[2]
				      "MAX(NumLevels),"				// row[3]
				      "SUM(NumFolders),"			// row[4]
				      "SUM(NumFiles),"				// row[5]
				      "SUM(TotalSize)"				// row[6]
			        " FROM "
	                       "("
	                       "SELECT brw_sizes.Cod AS CrsCod,"
				      "-1 AS GrpCod,"			// Course zones
				      "brw_sizes.NumLevels,"
				      "brw_sizes.NumFolders,"
				      "brw_sizes.NumFiles,"
				      "brw_sizes.TotalSize"
			        " FROM ins_instits,"
			              "ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "brw_sizes"
			       " WHERE ins_instits.CtyCod=%ld"
			         " AND ins_instits.InsCod=ctr_centers.InsCod"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser IN (%u,%u,%u,%u,%u,%u)"
	                       " UNION "
	                       "SELECT grp_types.CrsCod,"
				      "brw_sizes.Cod AS GrpCod,"	// Group zones
				      "brw_sizes.NumLevels,"
				      "brw_sizes.NumFolders,"
				      "brw_sizes.NumFiles,"
				      "brw_sizes.TotalSize"
			        " FROM ins_instits,"
			              "ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "grp_types,"
			              "grp_groups,"
			              "brw_sizes"
			       " WHERE ins_instits.CtyCod=%ld"
	                         " AND ins_instits.InsCod=ctr_centers.InsCod"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=grp_types.CrsCod"
			         " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
			         " AND grp_groups.GrpCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser IN (%u,%u,%u,%u)"
			       ") AS sizes",
			       Gbl.Hierarchy.Node[HieLvl_CTY].Cod,
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       Gbl.Hierarchy.Node[HieLvl_CTY].Cod,
			       (unsigned) Brw_ADMI_DOC_GRP,
			       (unsigned) Brw_ADMI_TCH_GRP,
			       (unsigned) Brw_ADMI_SHR_GRP,
			       (unsigned) Brw_ADMI_MRK_GRP);
	       break;
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_MRK_CRS:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT brw_sizes.Cod),"		// row[0]
				      "-1,"					// row[1]
				      "-1,"					// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM ins_instits,"
			              "ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "brw_sizes"
			       " WHERE ins_instits.CtyCod=%ld"
	                         " AND ins_instits.InsCod=ctr_centers.InsCod"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_CTY].Cod,
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_DOC_GRP:
	    case Brw_ADMI_TCH_GRP:
	    case Brw_ADMI_SHR_GRP:
	    case Brw_ADMI_MRK_GRP:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT grp_types.CrsCod),"	// row[0]
				      "COUNT(DISTINCT brw_sizes.Cod),"		// row[1]
				      "-1,"					// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM ins_instits,"
			              "ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "grp_types,"
			              "grp_groups,"
			              "brw_sizes"
			       " WHERE ins_instits.CtyCod=%ld"
	                         " AND ins_instits.InsCod=ctr_centers.InsCod"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=grp_types.CrsCod"
			         " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
			         " AND grp_groups.GrpCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_CTY].Cod,
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_ASG_USR:
	    case Brw_ADMI_WRK_USR:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT brw_sizes.Cod),"		// row[0]
				      "-1,"					// row[1]
				      "COUNT(DISTINCT brw_sizes.ZoneUsrCod),"	// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM ins_instits,"
			              "ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "brw_sizes"
			       " WHERE ins_instits.CtyCod=%ld"
	                         " AND ins_instits.InsCod=ctr_centers.InsCod"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=brw_sizes.Cod"
	                         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_CTY].Cod,
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_BRF_USR:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT -1,"					// row[0]
				      "-1,"					// row[1]
				      "COUNT(DISTINCT brw_sizes.ZoneUsrCod),"	// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM ins_instits,"
			              "ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "crs_users,"
			              "brw_sizes"
			       " WHERE ins_instits.CtyCod=%ld"
	                         " AND ins_instits.InsCod=ctr_centers.InsCod"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.UsrCod=brw_sizes.ZoneUsrCod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_CTY].Cod,
			       (unsigned) FileBrowser);
	       break;
	    default:
	       Err_WrongFileBrowserExit ();
	       break;
	   }
         break;
      /* Scope = the current institution */
      case HieLvl_INS:
	 switch (FileBrowser)
	   {
	    case Brw_UNKNOWN:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT CrsCod),"			// row[0]
				      "COUNT(DISTINCT GrpCod)-1,"		// row[1]
				      "-1,"					// row[2]
				      "MAX(NumLevels),"				// row[3]
				      "SUM(NumFolders),"			// row[4]
				      "SUM(NumFiles),"				// row[5]
				      "SUM(TotalSize)"				// row[6]
			        " FROM "
	                       "("
	                       "SELECT brw_sizes.Cod AS CrsCod,"
				      "-1 AS GrpCod,"			// Course zones
				      "brw_sizes.NumLevels,"
				      "brw_sizes.NumFolders,"
				      "brw_sizes.NumFiles,"
				      "brw_sizes.TotalSize"
			        " FROM ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "brw_sizes"
			       " WHERE ctr_centers.InsCod=%ld"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser IN (%u,%u,%u,%u,%u,%u)"
	                       " UNION "
	                       "SELECT grp_types.CrsCod,"
				      "brw_sizes.Cod AS GrpCod,"	// Group zones
				      "brw_sizes.NumLevels,"
				      "brw_sizes.NumFolders,"
				      "brw_sizes.NumFiles,"
				      "brw_sizes.TotalSize"
			        " FROM ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "grp_types,"
			              "grp_groups,"
			              "brw_sizes"
			       " WHERE ctr_centers.InsCod=%ld"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=grp_types.CrsCod"
			         " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
			         " AND grp_groups.GrpCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser IN (%u,%u,%u,%u)"
			       ") AS sizes",
			       Gbl.Hierarchy.Node[HieLvl_INS].Cod,
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       Gbl.Hierarchy.Node[HieLvl_INS].Cod,
			       (unsigned) Brw_ADMI_DOC_GRP,
			       (unsigned) Brw_ADMI_TCH_GRP,
			       (unsigned) Brw_ADMI_SHR_GRP,
			       (unsigned) Brw_ADMI_MRK_GRP);
	       break;
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_MRK_CRS:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT brw_sizes.Cod),"		// row[0]
				      "-1,"					// row[1]
				      "-1,"					// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "brw_sizes"
			       " WHERE ctr_centers.InsCod=%ld"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_INS].Cod,
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_DOC_GRP:
	    case Brw_ADMI_TCH_GRP:
	    case Brw_ADMI_SHR_GRP:
	    case Brw_ADMI_MRK_GRP:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT grp_types.CrsCod),"	// row[0]
				      "COUNT(DISTINCT brw_sizes.Cod),"		// row[1]
				      "-1,"					// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "grp_types,"
			              "grp_groups,"
			              "brw_sizes"
			       " WHERE ctr_centers.InsCod=%ld"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=grp_types.CrsCod"
			         " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
			         " AND grp_groups.GrpCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_INS].Cod,
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_ASG_USR:
	    case Brw_ADMI_WRK_USR:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT brw_sizes.Cod),"		// row[0]
				      "-1,"					// row[1]
				      "COUNT(DISTINCT brw_sizes.ZoneUsrCod),"	// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "brw_sizes"
			       " WHERE ctr_centers.InsCod=%ld"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=brw_sizes.Cod"
	                         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_INS].Cod,
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_BRF_USR:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT -1,"					// row[0]
				      "-1,"					// row[1]
				      "COUNT(DISTINCT brw_sizes.ZoneUsrCod),"	// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "crs_users,"
			              "brw_sizes"
			       " WHERE ctr_centers.InsCod=%ld"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.UsrCod=brw_sizes.ZoneUsrCod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_INS].Cod,
			       (unsigned) FileBrowser);
	       break;
	    default:
	       Err_WrongFileBrowserExit ();
	       break;
	   }
         break;
      /* Scope = the current center */
      case HieLvl_CTR:
	 switch (FileBrowser)
	   {
	    case Brw_UNKNOWN:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT CrsCod),"			// row[0]
				      "COUNT(DISTINCT GrpCod)-1,"		// row[1]
				      "-1,"					// row[2]
				      "MAX(NumLevels),"				// row[3]
				      "SUM(NumFolders),"			// row[4]
				      "SUM(NumFiles),"				// row[5]
				      "SUM(TotalSize)"				// row[6]
			        " FROM "
	                       "("
	                       "SELECT brw_sizes.Cod AS CrsCod,"
				      "-1 AS GrpCod,"			// Course zones
				      "brw_sizes.NumLevels,"
				      "brw_sizes.NumFolders,"
				      "brw_sizes.NumFiles,"
				      "brw_sizes.TotalSize"
			        " FROM deg_degrees,"
			              "crs_courses,"
			              "brw_sizes"
			       " WHERE deg_degrees.CtrCod=%ld"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser IN (%u,%u,%u,%u,%u,%u)"
	                       " UNION "
	                       "SELECT grp_types.CrsCod,"
				      "brw_sizes.Cod AS GrpCod,"	// Group zones
				      "brw_sizes.NumLevels,"
				      "brw_sizes.NumFolders,"
				      "brw_sizes.NumFiles,"
				      "brw_sizes.TotalSize"
			        " FROM deg_degrees,"
			              "crs_courses,"
			              "grp_types,"
			              "grp_groups,"
			              "brw_sizes"
			       " WHERE deg_degrees.CtrCod=%ld"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=grp_types.CrsCod"
			         " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
			         " AND grp_groups.GrpCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser IN (%u,%u,%u,%u)"
			       ") AS sizes",
			       Gbl.Hierarchy.Node[HieLvl_CTR].Cod,
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       Gbl.Hierarchy.Node[HieLvl_CTR].Cod,
			       (unsigned) Brw_ADMI_DOC_GRP,
			       (unsigned) Brw_ADMI_TCH_GRP,
			       (unsigned) Brw_ADMI_SHR_GRP,
			       (unsigned) Brw_ADMI_MRK_GRP);
	       break;
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_MRK_CRS:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT brw_sizes.Cod),"		// row[0]
				      "-1,"					// row[1]
				      "-1,"					// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM deg_degrees,"
			              "crs_courses,"
			              "brw_sizes"
			       " WHERE deg_degrees.CtrCod=%ld"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_CTR].Cod,
			       (unsigned) FileBrowser);
               break;
	    case Brw_ADMI_DOC_GRP:
	    case Brw_ADMI_TCH_GRP:
	    case Brw_ADMI_SHR_GRP:
	    case Brw_ADMI_MRK_GRP:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT grp_types.CrsCod),"
				      "COUNT(DISTINCT brw_sizes.Cod),"		// row[0]
				      "-1,"					// row[1]
				      "MAX(brw_sizes.NumLevels),"		// row[2]
				      "SUM(brw_sizes.NumFolders),"		// row[3]
				      "SUM(brw_sizes.NumFiles),"		// row[4]
				      "SUM(brw_sizes.TotalSize)"		// row[5]
			        " FROM deg_degrees,"
			              "crs_courses,"
			              "grp_types,"
			              "grp_groups,"
			              "brw_sizes"
			       " WHERE deg_degrees.CtrCod=%ld"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=grp_types.CrsCod"
			         " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
			         " AND grp_groups.GrpCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_CTR].Cod,
			       (unsigned) FileBrowser);
               break;
	    case Brw_ADMI_ASG_USR:
	    case Brw_ADMI_WRK_USR:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT brw_sizes.Cod),"		// row[0]
				      "-1,"					// row[1]
				      "COUNT(DISTINCT brw_sizes.ZoneUsrCod),"	// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM deg_degrees,"
			              "crs_courses,"
			              "brw_sizes"
			       " WHERE deg_degrees.CtrCod=%ld"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_CTR].Cod,
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_BRF_USR:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT -1,"					// row[0]
				      "-1,"					// row[1]
				      "COUNT(DISTINCT brw_sizes.ZoneUsrCod),"	// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM deg_degrees,"
			              "crs_courses,"
			              "crs_users,"
			              "brw_sizes"
			       " WHERE deg_degrees.CtrCod=%ld"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.UsrCod=brw_sizes.ZoneUsrCod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_CTR].Cod,
			       (unsigned) FileBrowser);
	       break;
	    default:
	       Err_WrongFileBrowserExit ();
	       break;
	   }
         break;
      /* Scope = the current degree */
      case HieLvl_DEG:
	 switch (FileBrowser)
	   {
	    case Brw_UNKNOWN:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT CrsCod),"			// row[0]
				      "COUNT(DISTINCT GrpCod)-1,"		// row[1]
				      "-1,"					// row[2]
				      "MAX(NumLevels),"				// row[3]
				      "SUM(NumFolders),"			// row[4]
				      "SUM(NumFiles),"				// row[5]
				      "SUM(TotalSize)"				// row[6]
			        " FROM "
	                       "("
	                       "SELECT brw_sizes.Cod AS CrsCod,"
				      "-1 AS GrpCod,"			// Course zones
				      "brw_sizes.NumLevels,"
				      "brw_sizes.NumFolders,"
				      "brw_sizes.NumFiles,"
				      "brw_sizes.TotalSize"
			        " FROM crs_courses,"
			              "brw_sizes"
			       " WHERE crs_courses.DegCod=%ld"
			         " AND crs_courses.CrsCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser IN (%u,%u,%u,%u,%u,%u)"
	                       " UNION "
	                       "SELECT grp_types.CrsCod,"
	                              "brw_sizes.Cod AS GrpCod,"	// Group zones
			              "brw_sizes.NumLevels,"
				      "brw_sizes.NumFolders,"
				      "brw_sizes.NumFiles,"
				      "brw_sizes.TotalSize"
			        " FROM crs_courses,"
			              "grp_types,"
			              "grp_groups,"
			              "brw_sizes"
			       " WHERE crs_courses.DegCod=%ld"
			         " AND crs_courses.CrsCod=grp_types.CrsCod"
			         " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
			         " AND grp_groups.GrpCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser IN (%u,%u,%u,%u)"
			       ") AS sizes",
			       Gbl.Hierarchy.Node[HieLvl_DEG].Cod,
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       Gbl.Hierarchy.Node[HieLvl_DEG].Cod,
			       (unsigned) Brw_ADMI_DOC_GRP,
			       (unsigned) Brw_ADMI_TCH_GRP,
			       (unsigned) Brw_ADMI_SHR_GRP,
			       (unsigned) Brw_ADMI_MRK_GRP);
	       break;
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_MRK_CRS:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT brw_sizes.Cod),"		// row[0]
				      "-1,"					// row[1]
				      "-1,"					// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM crs_courses,"
			              "brw_sizes"
			       " WHERE crs_courses.DegCod=%ld"
			         " AND crs_courses.CrsCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_DEG].Cod,
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_DOC_GRP:
	    case Brw_ADMI_TCH_GRP:
	    case Brw_ADMI_SHR_GRP:
	    case Brw_ADMI_MRK_GRP:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT grp_types.CrsCod),"	// row[0]
				      "COUNT(DISTINCT brw_sizes.Cod),"		// row[1]
				      "-1,"					// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM crs_courses,"
			              "grp_types,"
			              "grp_groups,"
			              "brw_sizes"
			       " WHERE crs_courses.DegCod=%ld"
			         " AND crs_courses.CrsCod=grp_types.CrsCod"
			         " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
			         " AND grp_groups.GrpCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_DEG].Cod,
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_ASG_USR:
	    case Brw_ADMI_WRK_USR:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT brw_sizes.Cod),"		// row[0]
				      "-1,"					// row[1]
				      "COUNT(DISTINCT brw_sizes.ZoneUsrCod),"	// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM crs_courses,"
			              "brw_sizes"
			       " WHERE crs_courses.DegCod=%ld"
			         " AND crs_courses.CrsCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_DEG].Cod,
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_BRF_USR:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT -1,"					// row[0]
				      "-1,"					// row[1]
				      "COUNT(DISTINCT brw_sizes.ZoneUsrCod),"	// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM crs_courses,"
			              "crs_users,"
			              "brw_sizes"
			       " WHERE crs_courses.DegCod=%ld"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.UsrCod=brw_sizes.ZoneUsrCod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_DEG].Cod,
			       (unsigned) FileBrowser);
	       break;
	    default:
	       Err_WrongFileBrowserExit ();
	       break;
	   }
         break;
      /* Scope = the current course */
      case HieLvl_CRS:
	 switch (FileBrowser)
	   {
	    case Brw_UNKNOWN:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT CrsCod),"			// row[0]
				      "COUNT(DISTINCT GrpCod)-1,"		// row[1]
				      "-1,"					// row[2]
				      "MAX(NumLevels),"				// row[3]
				      "SUM(NumFolders),"			// row[4]
				      "SUM(NumFiles),"				// row[5]
				      "SUM(TotalSize)"				// row[6]
			        " FROM "
	                       "("
	                       "SELECT Cod AS CrsCod,"
				      "-1 AS GrpCod,"			// Course zones
				      "NumLevels,"
				      "NumFolders,"
				      "NumFiles,"
				      "TotalSize"
			        " FROM brw_sizes"
			       " WHERE Cod=%ld"
			        " AND FileBrowser IN (%u,%u,%u,%u,%u,%u)"
	                       " UNION "
	                       "SELECT grp_types.CrsCod,"
				      "brw_sizes.Cod AS GrpCod,"	// Group zones
				      "brw_sizes.NumLevels,"
				      "brw_sizes.NumFolders,"
				      "brw_sizes.NumFiles,"
				      "brw_sizes.TotalSize"
			        " FROM grp_types,"
			              "grp_groups,"
			              "brw_sizes"
			       " WHERE grp_types.CrsCod=%ld"
			         " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
			         " AND grp_groups.GrpCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser IN (%u,%u,%u,%u)"
			       ") AS sizes",
			       Gbl.Hierarchy.Node[HieLvl_CRS].Cod,
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       Gbl.Hierarchy.Node[HieLvl_CRS].Cod,
			       (unsigned) Brw_ADMI_DOC_GRP,
			       (unsigned) Brw_ADMI_TCH_GRP,
			       (unsigned) Brw_ADMI_SHR_GRP,
			       (unsigned) Brw_ADMI_MRK_GRP);
	       break;
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_MRK_CRS:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT 1,"					// row[0]
				      "-1,"					// row[1]
				      "-1,"					// row[2]
				      "MAX(NumLevels),"				// row[3]
				      "SUM(NumFolders),"			// row[4]
				      "SUM(NumFiles),"				// row[5]
				      "SUM(TotalSize)"				// row[6]
			        " FROM brw_sizes"
			       " WHERE Cod=%ld"
			       " AND FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_CRS].Cod,
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_DOC_GRP:
	    case Brw_ADMI_TCH_GRP:
	    case Brw_ADMI_SHR_GRP:
	    case Brw_ADMI_MRK_GRP:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT grp_types.CrsCod),"	// row[0]
				      "COUNT(DISTINCT brw_sizes.Cod),"		// row[1]
				      "-1,"					// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM grp_types,"
			              "grp_groups,"
			              "brw_sizes"
			       " WHERE grp_types.CrsCod=%ld"
			         " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
			         " AND grp_groups.GrpCod=brw_sizes.Cod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_CRS].Cod,
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_ASG_USR:
	    case Brw_ADMI_WRK_USR:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT 1,"					// row[0]
				      "-1,"					// row[1]
				      "COUNT(DISTINCT ZoneUsrCod),"		// row[2]
				      "MAX(NumLevels),"				// row[3]
				      "SUM(NumFolders),"			// row[4]
				      "SUM(NumFiles),"				// row[5]
				      "SUM(TotalSize)"				// row[6]
			        " FROM brw_sizes"
			       " WHERE Cod=%ld"
			         " AND FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_CRS].Cod,
			       (unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_BRF_USR:
	       DB_QuerySELECT (mysql_res,"can not get size of a file browser",
			       "SELECT -1,"					// row[0]
				      "-1,"					// row[1]
				      "COUNT(DISTINCT brw_sizes.ZoneUsrCod),"	// row[2]
				      "MAX(brw_sizes.NumLevels),"		// row[3]
				      "SUM(brw_sizes.NumFolders),"		// row[4]
				      "SUM(brw_sizes.NumFiles),"		// row[5]
				      "SUM(brw_sizes.TotalSize)"		// row[6]
			        " FROM crs_users,"
			              "brw_sizes"
			       " WHERE crs_users.CrsCod=%ld"
			         " AND crs_users.UsrCod=brw_sizes.ZoneUsrCod"
			         " AND brw_sizes.FileBrowser=%u",
			       Gbl.Hierarchy.Node[HieLvl_CRS].Cod,
			       (unsigned) FileBrowser);
	       break;
	    default:
	       Err_WrongFileBrowserExit ();
	       break;
	   }
         break;
      default:
	 Err_WrongHierarchyLevelExit ();
	 break;
     }
  }
