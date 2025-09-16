// swad_mark_database.c: marks operations with database

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_browser.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_mark_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const char *Mrk_DB_HeadOrFootStr[2] =	// Names of columns in database, so don't change!
  {
   "Header",
   "Footer",
  };

/*****************************************************************************/
/****************** Add a new entry of marks into database *******************/
/*****************************************************************************/

void Mrk_DB_AddMarks (long FilCod,const struct Mrk_Properties *Marks)
  {
   DB_QueryINSERT ("can not add properties of marks to database",
		   "INSERT INTO mrk_marks"
		   " (FilCod,%s,%s)"
		   " VALUES"
		   " (%ld,%u,%u)",
	           Mrk_DB_HeadOrFootStr[Brw_HEADER],
	           Mrk_DB_HeadOrFootStr[Brw_FOOTER],
	           FilCod,
	           Marks->Header,
	           Marks->Footer);
  }

/*****************************************************************************/
/***** Change the number of rows of header or footer of a file of marks ******/
/*****************************************************************************/

void Mrk_DB_ChangeNumRowsHeaderOrFooter (Brw_HeadOrFoot_t HeaderOrFooter,unsigned NumRows)
  {
   extern const Brw_FileBrowser_t Brw_DB_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];

   DB_QueryUPDATE ("can not update properties of marks",
		   "UPDATE mrk_marks,"
			  "brw_files"
		     " SET mrk_marks.%s=%u"
		   " WHERE brw_files.FileBrowser=%u"
		     " AND brw_files.Cod=%ld"
		     " AND brw_files.Path='%s'"
		     " AND brw_files.FilCod=mrk_marks.FilCod",
		   Mrk_DB_HeadOrFootStr[HeaderOrFooter],NumRows,
		   (unsigned) Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type],
		   Brw_GetCodForFileBrowser (Gbl.FileBrowser.Type),
		   Gbl.FileBrowser.FilFolLnk.Full);
  }

/*****************************************************************************/
/*********************** Get marks data from database ************************/
/*****************************************************************************/

Exi_Exist_t Mrk_DB_GetMarksDataByCod (MYSQL_RES **mysql_res,long MrkCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get the number of rows"
				   " in header and footer",
			 "SELECT brw_files.FileBrowser,"	// row[0]
				"brw_files.Cod,"		// row[1]
				"brw_files.Path,"		// row[2]
				"mrk_marks.Header,"		// row[3]
				"mrk_marks.Footer"		// row[4]
			  " FROM brw_files,"
				"mrk_marks"
			 " WHERE brw_files.FilCod=%ld"
			   " AND brw_files.FilCod=mrk_marks.FilCod",
			  MrkCod);
  }

/*****************************************************************************/
/******** Get number of rows of header and of footer of a file of marks ******/
/*****************************************************************************/

Exi_Exist_t Mrk_DB_GetNumRowsHeaderAndFooter (MYSQL_RES **mysql_res)
  {
   extern const Brw_FileBrowser_t Brw_DB_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];

   /***** Get number of rows of header and footer from database *****/
   /* There should be a single file in database.
      If, due to an error, there is more than one file,
      get the number of rows of the more recent file. */
   return
   DB_QuerySELECTunique (mysql_res,"can not get the number of rows"
				   " in header and footer",
			 "SELECT mrk_marks.%s,"	// row[0]
				"mrk_marks.%s"	// row[1]
			  " FROM brw_files,"
				"mrk_marks"
			 " WHERE brw_files.FileBrowser=%u"
			   " AND brw_files.Cod=%ld"
			   " AND brw_files.Path='%s'"
			   " AND brw_files.FilCod=mrk_marks.FilCod"
		      " ORDER BY brw_files.FilCod DESC"
			 " LIMIT 1",	// On duplicate entries, get the more recent
			 Mrk_DB_HeadOrFootStr[Brw_HEADER],
			 Mrk_DB_HeadOrFootStr[Brw_FOOTER],
			 (unsigned) Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type],
			 Brw_GetCodForFileBrowser (Gbl.FileBrowser.Type),
			 Gbl.FileBrowser.FilFolLnk.Full);
  }
