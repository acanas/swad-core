// swad_browser_database.c: file browsers operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

// #define _GNU_SOURCE 		// For asprintf
// #include <dirent.h>		// For scandir, etc.
// #include <errno.h>		// For errno
// #include <linux/limits.h>	// For PATH_MAX
#include <mysql/mysql.h>	// To access MySQL databases
// #include <stddef.h>		// For NULL
// #include <stdio.h>		// For asprintf
// #include <stdlib.h>		// For exit, system, free, etc
// #include <string.h>		// For string functions
// #include <sys/types.h>		// For lstat, time_t
// #include <sys/stat.h>		// For lstat
// #include <time.h>		// For time
// #include <unistd.h>		// For access, lstat, getpid, chdir, symlink

// #include "swad_box.h"
// #include "swad_browser.h"
#include "swad_browser_database.h"
// #include "swad_config.h"
#include "swad_database.h"
#include "swad_error.h"
// #include "swad_figure.h"
// #include "swad_file_extension.h"
// #include "swad_file_MIME.h"
// #include "swad_form.h"
#include "swad_global.h"
// #include "swad_group_database.h"
// #include "swad_hierarchy.h"
// #include "swad_hierarchy_level.h"
// #include "swad_HTML.h"
// #include "swad_ID.h"
// #include "swad_logo.h"
// #include "swad_mark.h"
// #include "swad_notification.h"
// #include "swad_parameter.h"
// #include "swad_photo.h"
// #include "swad_profile.h"
// #include "swad_project.h"
// #include "swad_role.h"
// #include "swad_setting.h"
// #include "swad_string.h"
// #include "swad_timeline.h"
// #include "swad_timeline_note.h"
// #include "swad_zip.h"

/*****************************************************************************/
/******************** Global variables from other modules ********************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

/*****************************************************************************/
/*********************** Get the size of a file zone *************************/
/*****************************************************************************/

void Brw_DB_GetSizeOfFileZone (MYSQL_RES **mysql_res,
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
			       Gbl.Hierarchy.Cty.CtyCod,
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       Gbl.Hierarchy.Cty.CtyCod,
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
			       Gbl.Hierarchy.Cty.CtyCod,
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
			       Gbl.Hierarchy.Cty.CtyCod,
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
			       Gbl.Hierarchy.Cty.CtyCod,
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
			       Gbl.Hierarchy.Cty.CtyCod,
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
			       Gbl.Hierarchy.Ins.InsCod,
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       Gbl.Hierarchy.Ins.InsCod,
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
			       Gbl.Hierarchy.Ins.InsCod,
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
			       Gbl.Hierarchy.Ins.InsCod,
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
			       Gbl.Hierarchy.Ins.InsCod,
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
			       Gbl.Hierarchy.Ins.InsCod,
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
			       Gbl.Hierarchy.Ctr.CtrCod,
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       Gbl.Hierarchy.Ctr.CtrCod,
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
			       Gbl.Hierarchy.Ctr.CtrCod,
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
			       Gbl.Hierarchy.Ctr.CtrCod,
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
			       Gbl.Hierarchy.Ctr.CtrCod,
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
			       Gbl.Hierarchy.Ctr.CtrCod,
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
			       Gbl.Hierarchy.Deg.DegCod,
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       Gbl.Hierarchy.Deg.DegCod,
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
			       Gbl.Hierarchy.Deg.DegCod,
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
			       Gbl.Hierarchy.Deg.DegCod,
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
			       Gbl.Hierarchy.Deg.DegCod,
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
			       Gbl.Hierarchy.Deg.DegCod,
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
			       Gbl.Hierarchy.Crs.CrsCod,
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       Gbl.Hierarchy.Crs.CrsCod,
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
			       Gbl.Hierarchy.Crs.CrsCod,
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
			       Gbl.Hierarchy.Crs.CrsCod,
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
			       Gbl.Hierarchy.Crs.CrsCod,
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
			       Gbl.Hierarchy.Crs.CrsCod,
			       (unsigned) FileBrowser);
	       break;
	    default:
	       Err_WrongFileBrowserExit ();
	       break;
	   }
         break;
      default:
	 Err_WrongScopeExit ();
	 break;
     }
  }

/*****************************************************************************/
/***************** Get number of OERs depending on license *******************/
/*****************************************************************************/

unsigned Brw_DB_GetNumberOfOERs (MYSQL_RES **mysql_res,Brw_License_t License)
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
			 Gbl.Hierarchy.Cty.CtyCod,
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
			 Gbl.Hierarchy.Ins.InsCod,
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
			 Gbl.Hierarchy.Ctr.CtrCod,
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
			 Gbl.Hierarchy.Deg.DegCod,
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
			 Gbl.Hierarchy.Crs.CrsCod,
			 (unsigned) Brw_ADMI_DOC_CRS,
			 (unsigned) Brw_ADMI_SHR_CRS,
			 (unsigned) License);
      default:
	 Err_WrongScopeExit ();
	 return 0;	// Not reached
     }
  }
