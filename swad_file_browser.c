// swad_file_browser.c: file browsers

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#include <dirent.h>		// For scandir, etc.
#include <errno.h>		// For errno
#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For exit, system, malloc, free, etc
#include <string.h>		// For string functions
#include <time.h>		// For time
#include <sys/types.h>		// For lstat, time_t
#include <sys/stat.h>		// For lstat
#include <unistd.h>		// For access, lstat, getpid, chdir, symlink

#include "swad_config.h"
#include "swad_database.h"
#include "swad_file_browser.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_logo.h"
#include "swad_mark.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_string.h"
#include "swad_zip.h"

/*****************************************************************************/
/******************** Global variables from other modules ********************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

typedef enum
  {
   Brw_EXPAND_TREE_NOTHING,
   Brw_EXPAND_TREE_PLUS,
   Brw_EXPAND_TREE_MINUS,
  } Brw_ExpandTree_t;

typedef enum
  {
   Brw_SHOW,
   Brw_ADMIN,
  } Brw_ShowOrAdmin_t;

/*****************************************************************************/
/**************************** Internal constants *****************************/
/*****************************************************************************/

const char *Brw_FileTypeParamName[Brw_NUM_FILE_TYPES] =
  {
   "BrwFFL",	// Brw_IS_UNKNOWN
   "BrwFil",	// Brw_IS_FILE	- Do not use Fil_NAME_OF_PARAM_FILENAME_ORG
   "BrwFol",	// Brw_IS_FOLDER
   "BrwLnk",	// Brw_IS_LINK
  };

static long Brw_FileBrowserForDB[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   Brw_FILE_BRW_UNKNOWN,		// Brw_FILE_BRW_UNKNOWN             =  0,
   Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS,	// Brw_FILE_BRW_SEE_DOCUMENTS_CRS   =  1,
   Brw_FILE_BRW_ADMIN_MARKS_CRS,	// Brw_FILE_BRW_SEE_MARKS_CRS       =  2,
   Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS =  3,
   Brw_FILE_BRW_COMMON_CRS,		// Brw_FILE_BRW_COMMON_CRS          =  4,
   Brw_FILE_BRW_COMMON_GRP,		// Brw_FILE_BRW_COMMON_GRP          =  5,
   Brw_FILE_BRW_WORKS_USR,		// Brw_FILE_BRW_WORKS_USR           =  6,
   Brw_FILE_BRW_WORKS_USR,		// Brw_FILE_BRW_WORKS_CRS           =  7,
   Brw_FILE_BRW_ADMIN_MARKS_CRS,	// Brw_FILE_BRW_ADMIN_MARKS_CRS     =  8,
   Brw_FILE_BRW_BRIEFCASE_USR,		// Brw_FILE_BRW_BRIEFCASE_USR       =  9,
   Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP,	// Brw_FILE_BRW_SEE_DOCUMENTS_GRP   = 10,
   Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP = 11,
   Brw_FILE_BRW_ADMIN_MARKS_GRP,	// Brw_FILE_BRW_SEE_MARKS_GRP       = 12,
   Brw_FILE_BRW_ADMIN_MARKS_GRP,	// Brw_FILE_BRW_ADMIN_MARKS_GRP     = 13,
   Brw_FILE_BRW_ASSIGNMENTS_USR,	// Brw_FILE_BRW_ASSIGNMENTS_USR     = 14,
   Brw_FILE_BRW_ASSIGNMENTS_USR,	// Brw_FILE_BRW_ASSIGNMENTS_CRS     = 15,
  };
/*
const char *Brw_Licenses_DB[Brw_NUM_LICENSES] =
  {
   "all_rights_reserved",	// All Rights Reserved
   "cc_by",			// CC Attribution License
   "cc_by_sa",			// CC Attribution-ShareAlike License
   "cc_by_nd",			// CC Attribution-NoDerivs License
   "cc_by_nc",			// CC Attribution-NonCommercial License
   "cc_by_nc_sa",		// CC Attribution-NonCommercial-ShareAlike License
   "cc_by_nc_nd",		// CC Attribution-NonCommercial-NoDerivs License
  };
*/

// Brosers types for expanded folders
const Brw_FileBrowser_t Brw_FileBrowserForExpandedFolders[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   Brw_FILE_BRW_UNKNOWN,		// Brw_FILE_BRW_UNKNOWN
   Brw_FILE_BRW_SEE_DOCUMENTS_CRS,	// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   Brw_FILE_BRW_SEE_MARKS_CRS,		// Brw_FILE_BRW_SEE_MARKS_CRS
   Brw_FILE_BRW_SEE_DOCUMENTS_CRS,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   Brw_FILE_BRW_COMMON_CRS,		// Brw_FILE_BRW_COMMON_CRS
   Brw_FILE_BRW_COMMON_GRP,		// Brw_FILE_BRW_COMMON_GRP
   Brw_FILE_BRW_WORKS_USR,		// Brw_FILE_BRW_WORKS_USR
   Brw_FILE_BRW_WORKS_CRS,		// Brw_FILE_BRW_WORKS_CRS
   Brw_FILE_BRW_SEE_MARKS_CRS,		// Brw_FILE_BRW_ADMIN_MARKS_CRS
   Brw_FILE_BRW_BRIEFCASE_USR,		// Brw_FILE_BRW_BRIEFCASE_USR
   Brw_FILE_BRW_SEE_DOCUMENTS_GRP,	// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   Brw_FILE_BRW_SEE_DOCUMENTS_GRP,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   Brw_FILE_BRW_SEE_MARKS_GRP,		// Brw_FILE_BRW_SEE_MARKS_GRP
   Brw_FILE_BRW_SEE_MARKS_GRP,		// Brw_FILE_BRW_ADMIN_MARKS_GRP
   Brw_FILE_BRW_ASSIGNMENTS_USR,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   Brw_FILE_BRW_ASSIGNMENTS_CRS,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };

// Internal names of root folders
const char *Brw_RootFolderInternalNames[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   "",						// Brw_FILE_BRW_UNKNOWN
   Brw_INTERNAL_NAME_ROOT_FOLDER_DOWNLOAD,	// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   Brw_INTERNAL_NAME_ROOT_FOLDER_MARKS,		// Brw_FILE_BRW_SEE_MARKS_CRS
   Brw_INTERNAL_NAME_ROOT_FOLDER_DOWNLOAD,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   Brw_INTERNAL_NAME_ROOT_FOLDER_COMMON,	// Brw_FILE_BRW_COMMON_CRS
   Brw_INTERNAL_NAME_ROOT_FOLDER_COMMON,	// Brw_FILE_BRW_COMMON_GRP
   Brw_INTERNAL_NAME_ROOT_FOLDER_WORKS,		// Brw_FILE_BRW_WORKS_USR
   Brw_INTERNAL_NAME_ROOT_FOLDER_WORKS,		// Brw_FILE_BRW_WORKS_CRS
   Brw_INTERNAL_NAME_ROOT_FOLDER_MARKS,		// Brw_FILE_BRW_ADMIN_MARKS_CRS
   Brw_INTERNAL_NAME_ROOT_FOLDER_BRIEFCASE,	// Brw_FILE_BRW_BRIEFCASE_USR
   Brw_INTERNAL_NAME_ROOT_FOLDER_DOWNLOAD,	// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   Brw_INTERNAL_NAME_ROOT_FOLDER_DOWNLOAD,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   Brw_INTERNAL_NAME_ROOT_FOLDER_MARKS,		// Brw_FILE_BRW_SEE_MARKS_GRP
   Brw_INTERNAL_NAME_ROOT_FOLDER_MARKS,		// Brw_FILE_BRW_ADMIN_MARKS_GRP
   Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };

// Number of columns of a file browser
const unsigned Brw_NumColumnsInExpTree[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   5,			// Brw_FILE_BRW_UNKNOWN
   5,			// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   4,			// Brw_FILE_BRW_SEE_MARKS_CRS
   8,			// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   8,			// Brw_FILE_BRW_COMMON_CRS
   8,			// Brw_FILE_BRW_COMMON_GRP
   8,			// Brw_FILE_BRW_WORKS_USR
   8,			// Brw_FILE_BRW_WORKS_CRS
  10,			// Brw_FILE_BRW_ADMIN_MARKS_CRS
   8,			// Brw_FILE_BRW_BRIEFCASE_USR
   5,			// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   8,			// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   4,			// Brw_FILE_BRW_SEE_MARKS_GRP
  10,			// Brw_FILE_BRW_ADMIN_MARKS_GRP
   8,			// Brw_FILE_BRW_ASSIGNMENTS_USR
   8,			// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const bool Brw_FileBrowserIsEditable[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   false,		// Brw_FILE_BRW_UNKNOWN
   false,		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   false,		// Brw_FILE_BRW_SEE_MARKS_CRS
   true,		// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   true,		// Brw_FILE_BRW_COMMON_CRS
   true,		// Brw_FILE_BRW_COMMON_GRP
   true,		// Brw_FILE_BRW_WORKS_USR
   true,		// Brw_FILE_BRW_WORKS_CRS
   true,		// Brw_FILE_BRW_ADMIN_MARKS_CRS
   true,		// Brw_FILE_BRW_BRIEFCASE_USR
   false,		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   true,		// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   false,		// Brw_FILE_BRW_SEE_MARKS_GRP
   true,		// Brw_FILE_BRW_ADMIN_MARKS_GRP
   true,		// Brw_FILE_BRW_ASSIGNMENTS_USR
   true,		// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActAskRemoveFile[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_CRS
   ActReqRemFilDocCrs,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActReqRemFilComCrs,	// Brw_FILE_BRW_COMMON_CRS
   ActReqRemFilComGrp,	// Brw_FILE_BRW_COMMON_GRP
   ActReqRemFilWrkUsr,	// Brw_FILE_BRW_WORKS_USR
   ActReqRemFilWrkCrs,	// Brw_FILE_BRW_WORKS_CRS
   ActReqRemFilMrkCrs,	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActReqRemFilBrf,	// Brw_FILE_BRW_BRIEFCASE_USR
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActReqRemFilDocGrp,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_GRP
   ActReqRemFilMrkGrp,	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActReqRemFilAsgUsr,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActReqRemFilAsgCrs,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActRemoveFile[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_CRS
   ActRemFilDocCrs,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActRemFilComCrs,	// Brw_FILE_BRW_COMMON_CRS
   ActRemFilComGrp,	// Brw_FILE_BRW_COMMON_GRP
   ActRemFilWrkUsr,	// Brw_FILE_BRW_WORKS_USR
   ActRemFilWrkCrs,	// Brw_FILE_BRW_WORKS_CRS
   ActRemFilMrkCrs,	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActRemFilBrf,	// Brw_FILE_BRW_BRIEFCASE_USR
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActRemFilDocGrp,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_GRP
   ActRemFilMrkGrp,	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActRemFilAsgUsr,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActRemFilAsgCrs,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActRemoveFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_CRS
   ActRemFolDocCrs,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActRemFolComCrs,	// Brw_FILE_BRW_COMMON_CRS
   ActRemFolComGrp,	// Brw_FILE_BRW_COMMON_GRP
   ActRemFolWrkUsr,	// Brw_FILE_BRW_WORKS_USR
   ActRemFolWrkCrs,	// Brw_FILE_BRW_WORKS_CRS
   ActRemFolMrkCrs,	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActRemFolBrf,	// Brw_FILE_BRW_BRIEFCASE_USR
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActRemFolDocGrp,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_GRP
   ActRemFolMrkGrp,	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActRemFolAsgUsr,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActRemFolAsgCrs,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActRemoveFolderNotEmpty[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_CRS
   ActRemTreDocCrs,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActRemTreComCrs,	// Brw_FILE_BRW_COMMON_CRS
   ActRemTreComGrp,	// Brw_FILE_BRW_COMMON_GRP
   ActRemTreWrkUsr,	// Brw_FILE_BRW_WORKS_USR
   ActRemTreWrkCrs,	// Brw_FILE_BRW_WORKS_CRS
   ActRemTreMrkCrs,	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActRemTreBrf,	// Brw_FILE_BRW_BRIEFCASE_USR
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActRemTreDocGrp,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_GRP
   ActRemTreMrkGrp,	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActRemTreAsgUsr,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActRemTreAsgCrs,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActCopy[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_CRS
   ActCopDocCrs,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActCopComCrs,	// Brw_FILE_BRW_COMMON_CRS
   ActCopComGrp,	// Brw_FILE_BRW_COMMON_GRP
   ActCopWrkUsr,	// Brw_FILE_BRW_WORKS_USR
   ActCopWrkCrs,	// Brw_FILE_BRW_WORKS_CRS
   ActCopMrkCrs,	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActCopBrf,		// Brw_FILE_BRW_BRIEFCASE_USR
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActCopDocGrp,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_GRP
   ActCopMrkGrp,	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActCopAsgUsr,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActCopAsgCrs,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActPaste[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_CRS
   ActPasDocCrs,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActPasComCrs,	// Brw_FILE_BRW_COMMON_CRS
   ActPasComGrp,	// Brw_FILE_BRW_COMMON_GRP
   ActPasWrkUsr,	// Brw_FILE_BRW_WORKS_USR
   ActPasWrkCrs,	// Brw_FILE_BRW_WORKS_CRS
   ActPasMrkCrs,	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActPasBrf,		// Brw_FILE_BRW_BRIEFCASE_USR
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActPasDocGrp,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_GRP
   ActPasMrkGrp,	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActPasAsgUsr,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActPasAsgCrs,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActFormCreate[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_CRS
   ActFrmCreDocCrs,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActFrmCreComCrs,	// Brw_FILE_BRW_COMMON_CRS
   ActFrmCreComGrp,	// Brw_FILE_BRW_COMMON_GRP
   ActFrmCreWrkUsr,	// Brw_FILE_BRW_WORKS_USR
   ActFrmCreWrkCrs,	// Brw_FILE_BRW_WORKS_CRS
   ActFrmCreMrkCrs,	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActFrmCreBrf,	// Brw_FILE_BRW_BRIEFCASE_USR
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActFrmCreDocGrp,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_GRP
   ActFrmCreMrkGrp,	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActFrmCreAsgUsr,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActFrmCreAsgCrs,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActCreateFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_CRS
   ActCreFolDocCrs,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActCreFolComCrs,	// Brw_FILE_BRW_COMMON_CRS
   ActCreFolComGrp,	// Brw_FILE_BRW_COMMON_GRP
   ActCreFolWrkUsr,	// Brw_FILE_BRW_WORKS_USR
   ActCreFolWrkCrs,	// Brw_FILE_BRW_WORKS_CRS
   ActCreFolMrkCrs,	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActCreFolBrf,	// Brw_FILE_BRW_BRIEFCASE_USR
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActCreFolDocGrp,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_GRP
   ActCreFolMrkGrp,	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActCreFolAsgUsr,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActCreFolAsgCrs,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActCreateLink[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_CRS
   ActCreLnkDocCrs,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActCreLnkComCrs,	// Brw_FILE_BRW_COMMON_CRS
   ActCreLnkComGrp,	// Brw_FILE_BRW_COMMON_GRP
   ActCreLnkWrkUsr,	// Brw_FILE_BRW_WORKS_USR
   ActCreLnkWrkCrs,	// Brw_FILE_BRW_WORKS_CRS
   ActUnk,		// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActCreLnkBrf,	// Brw_FILE_BRW_BRIEFCASE_USR
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActCreLnkDocGrp,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_GRP
   ActUnk,		// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActCreLnkAsgUsr,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActCreLnkAsgCrs,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActRenameFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_CRS
   ActRenFolDocCrs,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActRenFolComCrs,	// Brw_FILE_BRW_COMMON_CRS
   ActRenFolComGrp,	// Brw_FILE_BRW_COMMON_GRP
   ActRenFolWrkUsr,	// Brw_FILE_BRW_WORKS_USR
   ActRenFolWrkCrs,	// Brw_FILE_BRW_WORKS_CRS
   ActRenFolMrkCrs,	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActRenFolBrf,	// Brw_FILE_BRW_BRIEFCASE_USR
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActRenFolDocGrp,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_GRP
   ActRenFolMrkGrp,	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActRenFolAsgUsr,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActRenFolAsgCrs,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActUploadFileDropzone[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_CRS
   ActRcvFilDocCrsDZ,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActRcvFilComCrsDZ,	// Brw_FILE_BRW_COMMON_CRS
   ActRcvFilComGrpDZ,	// Brw_FILE_BRW_COMMON_GRP
   ActRcvFilWrkUsrDZ,	// Brw_FILE_BRW_WORKS_USR
   ActRcvFilWrkCrsDZ,	// Brw_FILE_BRW_WORKS_CRS
   ActRcvFilMrkCrsDZ,	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActRcvFilBrfDZ,	// Brw_FILE_BRW_BRIEFCASE_USR
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActRcvFilDocGrpDZ,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_GRP
   ActRcvFilMrkGrpDZ,	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActRcvFilAsgUsrDZ,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActRcvFilAsgCrsDZ,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActUploadFileClassic[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_CRS
   ActRcvFilDocCrsCla,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActRcvFilComCrsCla,	// Brw_FILE_BRW_COMMON_CRS
   ActRcvFilComGrpCla,	// Brw_FILE_BRW_COMMON_GRP
   ActRcvFilWrkUsrCla,	// Brw_FILE_BRW_WORKS_USR
   ActRcvFilWrkCrsCla,	// Brw_FILE_BRW_WORKS_CRS
   ActRcvFilMrkCrsCla,	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActRcvFilBrfCla,	// Brw_FILE_BRW_BRIEFCASE_USR
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActRcvFilDocGrpCla,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_GRP
   ActRcvFilMrkGrpCla,	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActRcvFilAsgUsrCla,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActRcvFilAsgCrsCla,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActRefreshAfterUploadFiles[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_CRS
   ActAdmDocCrs,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActAdmComCrs,	// Brw_FILE_BRW_COMMON_CRS
   ActAdmComGrp,	// Brw_FILE_BRW_COMMON_GRP
   ActAdmAsgWrkUsr,	// Brw_FILE_BRW_WORKS_USR
   ActAdmAsgWrkCrs,	// Brw_FILE_BRW_WORKS_CRS
   ActAdmMrkCrs,	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActAdmBrf,		// Brw_FILE_BRW_BRIEFCASE_USR
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActAdmDocGrp,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_GRP
   ActAdmMrkGrp,	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActAdmAsgWrkUsr,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActAdmAsgWrkCrs,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActExpandFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActExpSeeDocCrs,	// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActExpSeeMrkCrs,	// Brw_FILE_BRW_SEE_MARKS_CRS
   ActExpAdmDocCrs,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActExpComCrs,	// Brw_FILE_BRW_COMMON_CRS
   ActExpComGrp,	// Brw_FILE_BRW_COMMON_GRP
   ActExpWrkUsr,	// Brw_FILE_BRW_WORKS_USR
   ActExpWrkCrs,	// Brw_FILE_BRW_WORKS_CRS
   ActExpAdmMrkCrs,	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActExpBrf,		// Brw_FILE_BRW_BRIEFCASE_USR
   ActExpSeeDocGrp,	// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActExpAdmDocGrp,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActExpSeeMrkGrp,	// Brw_FILE_BRW_SEE_MARKS_GRP
   ActExpAdmMrkGrp,	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActExpAsgUsr,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActExpAsgCrs,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActContractFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActConSeeDocCrs,	// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActConSeeMrkCrs,	// Brw_FILE_BRW_SEE_MARKS_CRS
   ActConAdmDocCrs,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActConComCrs,	// Brw_FILE_BRW_COMMON_CRS
   ActConComGrp,	// Brw_FILE_BRW_COMMON_GRP
   ActConWrkUsr,	// Brw_FILE_BRW_WORKS_USR
   ActConWrkCrs,	// Brw_FILE_BRW_WORKS_CRS
   ActConAdmMrkCrs,	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActConBrf,		// Brw_FILE_BRW_BRIEFCASE_USR
   ActConSeeDocGrp,	// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActConAdmDocGrp,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActConSeeMrkGrp,	// Brw_FILE_BRW_SEE_MARKS_GRP
   ActConAdmMrkGrp,	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActConAsgUsr,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActConAsgCrs,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };
const Act_Action_t Brw_ActRecDatFile[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_FILE_BRW_UNKNOWN
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_CRS
   ActChgDatAdmDocCrs,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   ActChgDatComCrs,	// Brw_FILE_BRW_COMMON_CRS
   ActChgDatComGrp,	// Brw_FILE_BRW_COMMON_GRP
   ActChgDatWrkUsr,	// Brw_FILE_BRW_WORKS_USR
   ActChgDatWrkCrs,	// Brw_FILE_BRW_WORKS_CRS
   ActChgDatAdmMrkCrs,	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   ActChgDatBrf,	// Brw_FILE_BRW_BRIEFCASE_USR
   ActUnk,		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   ActChgDatAdmDocGrp,	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   ActUnk,		// Brw_FILE_BRW_SEE_MARKS_GRP
   ActChgDatAdmMrkGrp,	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   ActChgDatAsgUsr,	// Brw_FILE_BRW_ASSIGNMENTS_USR
   ActChgDatAsgCrs,	// Brw_FILE_BRW_ASSIGNMENTS_CRS
  };

/* All quotas must be multiple of 1 GiB (Gibibyte)*/
#define Brw_GiB (1024ULL*1024ULL*1024ULL)

/* Maximum quotas for each type of file browser */
#define Brw_MAX_QUOTA_DOWNLOAD_CRS		(64ULL*Brw_GiB)
#define Brw_MAX_FILES_DOWNLOAD_CRS		5000
#define Brw_MAX_FOLDERS_DOWNLOAD_CRS		1000

#define Brw_MAX_QUOTA_DOWNLOAD_GRP		( 1ULL*Brw_GiB)
#define Brw_MAX_FILES_DOWNLOAD_GRP		1000
#define Brw_MAX_FOLDERS_DOWNLOAD_GRP		500

#define Brw_MAX_QUOTA_COMMON_CRS		(64ULL*Brw_GiB)
#define Brw_MAX_FILES_COMMON_CRS		5000
#define Brw_MAX_FOLDERS_COMMON_CRS		1000	// Many, because every student can create his own directories

#define Brw_MAX_QUOTA_COMMON_GRP		( 1ULL*Brw_GiB)
#define Brw_MAX_FILES_COMMON_GRP		1000
#define Brw_MAX_FOLDERS_COMMON_GRP		500	// Many, because every student can create his own directories

#define Brw_MAX_QUOTA_ASSIGNMENTS_PER_STD	( 2ULL*Brw_GiB)
#define Brw_MAX_FILES_ASSIGNMENTS_PER_STD	500
#define Brw_MAX_FOLDERS_ASSIGNMENTS_PER_STD	50

#define Brw_MAX_QUOTA_WORKS_PER_STD		( 2ULL*Brw_GiB)
#define Brw_MAX_FILES_WORKS_PER_STD		500
#define Brw_MAX_FOLDERS_WORKS_PER_STD		50

#define Brw_MAX_QUOTA_GRADES_CRS		( 1ULL*Brw_GiB)
#define Brw_MAX_FILES_GRADES_CRS		500
#define Brw_MAX_FOLDERS_GRADES_CRS		50

#define Brw_MAX_QUOTA_GRADES_GRP		( 1ULL*Brw_GiB)
#define Brw_MAX_FILES_GRADES_GRP		200
#define Brw_MAX_FOLDERS_GRADES_GRP		20

const unsigned long long Brw_MAX_QUOTAS_BRIEFCASE[Rol_NUM_ROLES] =	// MaxRole is used
  {
	            0,	// Rol_ROLE_UNKNOWN
	            0,	// Rol_ROLE_GUEST
	            0,	// Rol_ROLE_VISITOR
	32ULL*Brw_GiB,	// Rol_ROLE_STUDENT
	64ULL*Brw_GiB,	// Rol_ROLE_TEACHER
	            0,	// Rol_ROLE_DEG_ADMIN
	            0,	// Rol_ROLE_CTR_ADMIN
	            0,	// Rol_ROLE_INS_ADMIN
	            0,	// Rol_ROLE_SUPERUSER
  };
#define Brw_MAX_FILES_BRIEFCASE			5000
#define Brw_MAX_FOLDERS_BRIEFCASE		1000

/* Extensions allowed for uploaded files */
const char *Brw_FileExtensionsAllowed[] =
  {
   "3gp"  ,	// Video Android mobile
   "7z"   ,
   "asm"  ,
   "avi"  ,
   "bas"  ,
   "bat"  ,
   "bbl"  ,
   "bib"  ,
   "bin"  ,
   "bmp"  ,
   "bz2"  ,
   "c"    ,
   "cc"   ,
   "cdr"  ,
   "com"  ,
   "cpp"  ,
   "css"  ,
   "csv"  ,
   "dmg"  ,
   "doc"  ,
   "docx" ,
   "dotm" ,
   "dwd"  ,
   "eps"  ,
   "exe"  ,
   "fdf"  ,
   "flv"  ,
   "gif"  ,
   "gz"   ,
   "h"    ,
   "hex"  ,
   "htm"  ,
   "html" ,
   "img"  ,
   "java" ,
   "jpg"  ,
   "jpeg" ,
   "latex",
   "m"    ,
   "mdb"  ,
   "mht"  ,
   "mhtml",
   "mid"  ,
   "mov"  ,
   "mp3"  ,
   "mp4"  ,
   "mpeg" ,
   "mpg"  ,
   "mpp"  ,
   "mus"  ,
   "nb"   ,
   "odb"  ,
   "odc"  ,
   "odf"  ,
   "odg"  ,
   "odi"  ,
   "odm"  ,
   "odp"  ,
   "ods"  ,
   "odt"  ,
   "otg"  ,
   "otp"  ,
   "ots"  ,
   "ott"  ,
   "pas"  ,
   "pl"   ,
   "pdf"  ,
   "png"  ,
   "pps"  ,
   "ppsx" ,
   "ppt"  ,
   "pptx" ,
   "ps"   ,
   "pss"  ,
   "qt"   ,
   "r"    ,
   "ram"  ,
   "rar"  ,
   "raw"  ,
   "rdata",
   "rm"   ,
   "rp"   ,	// Axure, http://www.axure.com/
   "rtf"  ,
   "s"    ,
   "sav"  ,	// SPSS Data File
   "sbs"  ,
   "sf3"  ,
   "sgp"  ,
   "spp"  ,
   "spo"  ,
   "sps"  ,
   "swf"  ,
   "sws"  ,
   "tar"  ,
   "tex"  ,
   "tgz"  ,
   "tif"  ,
   "txt"  ,
   "voc"  ,
   "vp"   ,	// Justinmind, http://www.justinmind.com/
   "wav"  ,
   "wma"  ,
   "wmv"  ,
   "wxm"  ,
   "wxmx" ,
   "xls"  ,
   "xlsx" ,
   "zip"  ,
  };
const unsigned Brw_NUM_FILE_EXT_ALLOWED = sizeof (Brw_FileExtensionsAllowed) / sizeof (Brw_FileExtensionsAllowed[0]);

/* MIME types allowed for uploades files */
const char *Brw_MIMETypesAllowed[] =
  {
   "application/",			//
   "application/acrobat",		// PDF
   "application/arj",			// compressed archive arj
   "application/binary",		//
   "application/bzip2",			// Bzip 2 UNIX Compressed File
   "application/cdr",			// Corel Draw (CDR)
   "application/coreldraw",		// Corel Draw (CDR)
   "application/css-stylesheet",	// Hypertext Cascading Style Sheet
   "application/csv",			// CSV, Comma Separated Values
   "application/data",			//
   "application/download",		// zip files in Firefox caused by an error?
   "application/excel",			// Microsoft Excel xls
   "application/finale",		// Finale .mus
   "application/force-download",	// RAR uploaded from Firefox
   "application/futuresplash",		// Flash
   "application/gzip",			// GNU ZIP gz, gzip
   "application/gzip-compressed",	// GNU ZIP gz, gzip
   "application/gzipped",		// GNU ZIP gz, gzip
   "application/msaccess",		// Microsoft Access mdb
   "application/msexcel",		// Microsoft Excel xla, xls, xlt, xlw
   "application/mspowerpoint",		// Microsoft PowerPoint pot, pps, ppt
   "application/mathematica",		// Mathematica
   "application/matlab",		// Matlab
   "application/mfile",			// Matlab
   "application/mpp",			// Microsoft Project mpp
   "application/msproj",		// Microsoft Project mpp
   "application/msproject",		// Microsoft Project mpp
   "application/msword",		// Microsoft Word doc, word, w6w
   "application/mswrite",		// Microsoft Write wri
   "application/octet",			// uninterpreted binary bin
   "application/octet-binary",
   "application/octetstream",		// uninterpreted binary bin
   "application/octet-stream",		// uninterpreted binary bin
   "application/pdf",			// Adobe Acrobat pdf
   "application/postscript",		// PostScript ai, eps, ps
   "application/powerpoint",		// Microsoft PowerPoint pot, pps, ppt
   "application/rar",			// RAR
   "application/rtf",			// RTF
   "application/self-extracting",	// Compressed file, self-extracting
   "application/stream",		// PDF in Mac?
   "application/unknown",
   "application/vnd.fdf",		// Forms Data Format
   "application/vnd.msexcel",		// Microsoft Excel .xls
   "application/vnd.ms-excel",		// Microsoft Excel .xls
   "application/vnd.ms-powerpoint",	// Microsoft PowerPoint .ppt or .pps
   "application/vnd.ms-project",	// Microsoft Project .mpp
   "application/vnd.ms-word",		// Microsoft Word .doc
   "application/vnd.ms-word.template.macroenabled.12",		// Microsoft Word template .dotm
   "application/vnd.oasis.opendocument.text",			// OpenOffice Text 			.odt
   "application/vnd.oasis.opendocument.spreadsheet",		// OpenOffice Hoja of cálculo 		.ods
   "application/vnd.oasis.opendocument.presentation",		// OpenOffice Presentación 		.odp
   "application/vnd.oasis.opendocument.graphics",		// OpenOffice Dibujo 			.odg
   "application/vnd.oasis.opendocument.chart",			// OpenOffice Gráfica 			.odc
   "application/vnd.oasis.opendocument.formula",		// OpenOffice Fórmula matemática 	.odf
   "application/vnd.oasis.opendocument.database",		// OpenOffice database 			.odb
   "application/vnd.oasis.opendocument.image",			// OpenOffice Imagen 			.odi
   "application/vnd.oasis.opendocument.text-master",		// OpenOffice Documento maestro 	.odm
   "application/vnd.oasis.opendocument.text-template",		// OpenOffice Text 			.ott
   "application/vnd.oasis.opendocument.spreadsheet-template",	// OpenOffice Hoja of cálculo 		.ots
   "application/vnd.oasis.opendocument.presentation-template",	// OpenOffice Presentación 		.otp
   "application/vnd.oasis.opendocument.graphics-template",	// OpenOffice Dibujo 			.otg
   "application/vnd.openxmlformats-officedocument.presentationml.presentation",	// Power Point Microsoft Office Open XML Format Presentation Slide Show .pptx
   "application/vnd.openxmlformats-officedocument.presentationml.slideshow",	// Power Point Microsoft Office Open XML Format Presentation Slide Show .ppsx
   "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",		// Excel Microsoft Office Open XML Format Spreadsheet .xlsx
   "application/vnd.openxmlformats-officedocument.wordprocessingml.document",	// Word Microsoft Office Open XML Format Document .docx
   "application/vnd.pdf",		// PDF
   "application/x-7z-compressed",	// 7 zip compressed file
   "application/x-bz2",			// Bzip 2 UNIX Compressed File
   "application/x-bzip",		// Bzip 2 UNIX Compressed File
   "application/x-cdr",			// Corel Draw (CDR)
   "application/x-compress",		// GNU ZIP gz, gzip
   "application/x-compressed",		// GNU ZIP gz, gzip, Bzip 2 UNIX Compressed File
   "application/x-compressed-tar",	// TGZ
   "application/x-coreldraw",		// Corel Draw (CDR)
   "application/x-dos_ms_project",	// Microsoft Project mpp
   "application/x-download",		// RAR
   "application/x-file-download",	// PDF
   "application/x-forcedownload",	// PDF
   "application/x-gtar",		// GNU tar gtar
   "application/x-gunzip",		// GNU ZIP gz, gzip
   "application/x-gzip",		// GNU ZIP gz, gzip
   "application/x-latex",	   	// LateX latex (LateX)
   "application/x-midi",		// MIDI mid
   "application/x-msdos-program",	// MSDOS program
   "application/x-msdownload",		// dll, exe
   "application/x-mspowerpoint",	// Microsoft PowerPoint pot, pps, ppt
   "application/x-msproject",		// Microsoft Project mpp
   "application/x-ms-project",		// Microsoft Project mpp
   "application/x-msword",		// PDF?
   "application/x-mswrite",		// PDF
   "application/x-rar",			// .rar
   "application/x-shockwave-flash",		// Flash
   "application/x-shockwave-flash2-preview",	// Flash
   "application/x-unknown",		//  Unknown file type
   "application/x-vnd.oasis.opendocument.chart",		// OpenOffice Gráfica 			.odc
   "application/x-vnd.oasis.opendocument.database",		// OpenOffice database 			.odb
   "application/x-vnd.oasis.opendocument.formula",		// OpenOffice Fórmula matemática 	.odf
   "application/x-vnd.oasis.opendocument.graphics",		// OpenOffice Dibujo 			.odg
   "application/x-vnd.oasis.opendocument.graphics-template",	// OpenOffice Dibujo 			.otg
   "application/x-vnd.oasis.opendocument.image",		// OpenOffice Imagen 			.odi
   "application/x-vnd.oasis.opendocument.presentation",		// OpenOffice Presentación 		.odp
   "application/x-vnd.oasis.opendocument.presentation-template",// OpenOffice Presentación 		.otp
   "application/x-vnd.oasis.opendocument.spreadsheet",		// OpenOffice Hoja of cálculo 		.ods
   "application/x-vnd.oasis.opendocument.spreadsheet-template",	// OpenOffice Hoja of cálculo 		.ots
   "application/x-vnd.oasis.opendocument.text",			// OpenOffice Text 			.odt
   "application/x-vnd.oasis.opendocument.text-master",		// OpenOffice Documento maestro 	.odm
   "application/x-vnd.oasis.opendocument.text-template",	// OpenOffice Text			.ott
   "application/x-pdf",			// PDF
   "application/x-shockwave-flash",	// Macromedia Shockwave swf
   "application/x-spss",		// SPSS File sav spp sbs sps spo
   "application/x-rar-compressed",	// RAR archive rar
   "application/x-tar",			// 4.3BSD tar format tar
   "application/x-tex",			// TeX tex (LateX)
   "application/x-tgz",			// TGZ
   "application/x-troff",		// .s assembler source file
   "application/x-zip",			// ZIP archive zip
   "application/x-zip-compressed",	// ZIP archive zip
   "application/zip",			// ZIP archive zip
   "audio/basic",			// BASIC audio (u-law) au, snd
   "audio/mp4",				// MPEG-4
   "audio/mpeg",			// MP3
   "audio/midi",			// MIDI mid, midi
   "audio/x-aiff",			// AIFF audio aif, aifc, aiff
   "audio/x-mpeg",			// MPEG audio mp3
   "audio/x-ms-wma",			// WMA (Windows Media Audio File)
   "audio/x-pn-realaudio",		// RealAudio ra, ram
   "audio/x-pn-realaudio-plugin",	// RealAudio plug-in rpm
   "audio/x-voice",			// Voice voc
   "audio/x-wav",			// Microsoft Windows WAVE audio wav
   "binary/octet-stream",		// uninterpreted binary bin
   "document/unknown",			// Some bowsers send this (?)
   "file/unknown",			// Some bowsers send this (?)
   "gzip/document",			// GNU ZIP gz, gzip
   "image/bmp",				// Bitmap bmp
   "image/cdr",				// Corel Draw (CDR)
   "image/gif",				// GIF image gif
   "image/jpeg",			// JPEG image jpe, jpeg, jpg
   "image/pdf",				// PDF
   "image/pjpeg",			// JPEG image jpe, jpeg, jpg
   "image/pict",			// Macintosh PICT pict
   "image/png",				// Portable Network Graphic png
   "image/tiff",			// TIFF image tif, tiff
   "image/vnd.rn-realflash",		// Flash
   "image/x-cdr",			// Corel Draw (CDR)
   "image/x-cmu-raster",		// CMU raster ras
   "image/x-eps",			// Imagen postcript
   "image/x-png",			// Portable Network Graphic png
   "image/x-portable-anymap",		// PBM Anymap format pnm
   "image/x-portable-bitmap",		// PBM Bitmap format pbm
   "image/x-portable-graymap",		// PBM Graymap format pgm
   "image/x-portable-pixmap",		// PBM Pixmap format ppm
   "image/x-rgb",			// RGB image rgb
   "image/x-xbitmap",			// X Bitmap xbm
   "image/x-xpixmap",			// X Pixmap xpm
   "image/x-xwindowdump",		// X BrowserWindow System dump xwd
   "message/rfc822",			// Files .mht and .mhtml
   "mime/pdf",				// Adobe Acrobat pdf
   "multipart/x-gzip",			// GNU ZIP archive gzip
   "multipart/x-zip",			// PKZIP archive zip
   "octet/pdf",				// PDF
   "text/anytext",			// CSV, Comma Separated Values?
   "text/comma-separated-values",	// CSV, Comma Separated Values
   "text/css",				// Hypertext Cascading Style Sheet
   "text/csv",				// CSV, Comma Separated Values
   "text/html",				// HTML htm, html, php
   "text/pdf",				// PDF
   "text/plain",			// plain text C, cc, h, txt. BAS
   "text/richtext",			// RTF
   "text/xml",				//
   "text/x-chdr",			// Source code in C
   "text/x-csrc",			// Source code in C
   "text/x-c++src",			// Source code in C++
   "text/x-latex",			// LateX
   "text/x-pdf",			// PDF
   "video/3gpp",			// Video Android mobile
   "video/avi",				// AVI
   "video/mp4",				// MPEG-4
   "video/mpeg",			// MPEG video mpe, mpeg, mpg
   "video/msvideo",			// Microsoft Windows video avi
   "video/quicktime",			// QuickTime video mov, qt
   "video/unknown",			// ?
   "video/x-ms-asf",			// WMA (Windows Media Audio File)
   "video/x-ms-wmv",			// WMV (Windows Media File)
   "video/x-msvideo",			// AVI
   "x-world/x-vrml",			// VRML Worlds wrl
   "x-java",				// Source code in Java
   "zz-application/zz-winassoc-cdr",	// Corel Draw (CDR)
   "zz-application/zz-winassoc-mpp"	// Microsoft Project mpp
  };

const unsigned Brw_NUM_MIME_TYPES_ALLOWED = sizeof (Brw_MIMETypesAllowed) / sizeof (Brw_MIMETypesAllowed[0]);

/*****************************************************************************/
/*************************** Internal prototypes *****************************/
/*****************************************************************************/

static long Brw_GetGrpSettings (void);
static void Brw_GetDataCurrentGrp (void);
static void Brw_GetParamsPathInTreeAndFileName (void);
static void Brw_SetPathFileBrowser (void);
static void Brw_CreateFoldersAssignmentsIfNotExist (void);
static void Brw_SetAndCheckQuota (void);
static void Brw_SetMaxQuota (void);
static bool Brw_CheckIfQuotaExceded (void);

static void Brw_ShowFileBrowserNormal (void);
static void Brw_ShowFileBrowsersAsgWrkCrs (void);
static void Brw_ShowFileBrowsersAsgWrkUsr (void);

static void Brw_FormToChangeZone (void);
static void Brw_GetSelectedGroupData (struct GroupData *GrpDat,bool AbortOnError);
static void Brw_ShowDataOwnerAsgWrk (struct UsrData *UsrDat);
static void Brw_ShowFileBrowser (void);
static void Brw_WriteTopBeforeShowingFileBrowser (void);
static void Brw_UpdateLastAccess (void);
static void Brw_UpdateGrpLastAccZone (const char *FieldNameDB,long GrpCod);
static void Brw_WriteSubtitleOfFileBrowser (void);
static void Brw_InitHiddenLevels (void);
static void Brw_ShowSizeOfFileTree (void);
static void Brw_StoreSizeOfFileTreeInDB (void);
static void Brw_PutFormToShowOrAdmin (Brw_ShowOrAdmin_t ShowOrAdmin,
                                      Act_Action_t Action);
static void Brw_WriteFormFullTree (void);
static bool Brw_GetFullTreeFromForm (void);
static void Brw_GetAndUpdateDateLastAccFileBrowser (const char *FieldNameDB);
static long Brw_GetGrpLastAccZone (const char *FieldNameDB);
static void Brw_ResetFileBrowserSize (void);
static void Brw_CalcSizeOfDirRecursive (unsigned Level,char *Path);
static void Brw_ListDir (unsigned Level,const char *Path,const char *PathInTree);
static bool Brw_WriteRowFileBrowser (unsigned Level,
                                     Brw_FileType_t FileType,Brw_ExpandTree_t ExpandTree,
                                     const char *PathInTree,const char *FileName);
static void Brw_PutIconsRemoveCopyPaste (unsigned Level,Brw_FileType_t FileType,
                                         const char *PathInTree,const char *FileName,const char *FileNameToShow);
static bool Brw_CheckIfCanPasteIn (unsigned Level);
static void Brw_PutIconRemoveFile (Brw_FileType_t FileType,
                                   const char *PathInTree,const char *FileName,const char *FileNameToShow);
static void Brw_PutIconRemoveDir (const char *PathInTree,const char *FileName,const char *FileNameToShow);
static void Brw_PutIconCopy (Brw_FileType_t FileType,
                             const char *PathInTree,const char *FileName,const char *FileNameShow);
static void Brw_PutIconPasteOn (const char *PathInTree,const char *FileName,const char *FileNameToShow);
static void Brw_PutIconPasteOff (void);
static void Brw_IndentAndWriteIconExpandContract (unsigned Level,Brw_ExpandTree_t ExpandTree,
                                                  const char *PathInTree,const char *FileName,const char *FileNameToShow);
static void Brw_IndentDependingOnLevel (unsigned Level);
static void Brw_PutIconShow (unsigned Level,Brw_FileType_t FileType,
                             const char *PathInTree,const char *FileName,const char *FileNameToShow);
static void Brw_PutIconHide (unsigned Level,Brw_FileType_t FileType,
                             const char *PathInTree,const char *FileName,const char *FileNameToShow);
static bool Brw_CheckIfAnyUpperLevelIsHidden (unsigned CurrentLevel);
static void Brw_PutIconFolder (unsigned Level,Brw_ExpandTree_t ExpandTree,
                               const char *PathInTree,const char *FileName,const char *FileNameToShow);
static void Brw_PutIconNewFileOrFolder (void);
static void Brw_PutIconFileWithLinkToViewMetadata (unsigned Size,Brw_FileType_t FileType,
                                                   const char *PathInTree,const char *FileName,const char *FileNameToShow);
static void Brw_PutIconFile (unsigned Size,Brw_FileType_t FileType,const char *FileName);
static void Brw_WriteFileName (unsigned Level,bool IsPublic,Brw_FileType_t FileType,
                               const char *PathInTree,const char *FileName,const char *FileNameToShow);
static void Brw_GetFileNameToShow (Brw_FileBrowser_t FileBrowser,unsigned Level,Brw_FileType_t FileType,
                                   const char *FileName,char *FileNameToShow);
static void Brw_LimitLengthFileNameToShow (Brw_FileType_t FileType,const char *FileName,char *FileNameToShow);
static void Brw_CreateTmpLinkToDownloadFileBrowser (const char *FullPathIncludingFile,const char *FileName);
static void Brw_WriteDatesAssignment (void);
static void Brw_WriteFileSizeAndDate (Brw_FileType_t FileType,struct FileMetadata *FileMetadata);
static void Brw_WriteFileOrFolderPublisher (unsigned Level,unsigned long UsrCod);
static void Brw_AskConfirmRemoveFolderNotEmpty (void);
static inline void Brw_GetAndWriteClipboard (void);
static void Brw_WriteCurrentClipboard (void);
static bool Brw_GetMyClipboard (void);
static bool Brw_CheckIfClipboardIsInThisTree (void);
static void Brw_AddPathToClipboards (Brw_FileType_t FileType,const char *Path);
static void Brw_UpdatePathInClipboard (Brw_FileType_t FileType,const char *Path);
static void Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (const char *Path);
static void Brw_RemThisFolderAndUpdOtherFoldersFromExpandedFolders (const char *Path);
static void Brw_InsertFolderInExpandedFolders (const char *Path);
static void Brw_UpdateClickTimeOfThisFileBrowserInExpandedFolders (void);
static void Brw_RemoveFolderFromExpandedFolders (const char *Path);
static void Brw_RemoveExpiredClipboards (void);
static void Brw_RemoveAffectedClipboards (Brw_FileBrowser_t FileBrowser,long MyUsrCod,long WorksUsrCod);
static void Brw_RemoveAffectedExpandedFolders (const char *Path);
static void Brw_RenameAffectedExpandedFolders (Brw_FileBrowser_t FileBrowser,long MyUsrCod,long WorksUsrCod,const char *OldPath,const char *NewPath);
static void Brw_PasteClipboard (void);
static unsigned Brw_NumLevelsInPath (const char *Path);
static bool Brw_PasteTreeIntoFolder (const char *PathOrg,const char *PathDstInTree,
                                     unsigned *NumFilesPasted,
                                     unsigned *NumFoldsPasted,
                                     unsigned *NumLinksPasted,
                                     long *FirstFilCod);
static void Brw_PutFormToCreateAFolder (const char *FileNameToShow);
static void Brw_PutFormToUploadFilesUsingDropzone (const char *FileNameToShow);
static void Brw_PutFormToUploadOneFileClassic (const char *FileNameToShow);
static void Brw_PutFormToPasteAFileOrFolder (const char *FileNameToShow);
static void Brw_PutFormToCreateALink (const char *FileNameToShow);
static bool Brw_RcvFileInFileBrw (Brw_UploadType_t UploadType);
static bool Brw_CheckIfUploadIsAllowed (const char *FileType);

static bool Brw_CheckIfICanEditFileMetadata (long PublisherUsrCod);
static void Brw_WriteBigLinkToDownloadFile (const char *URL,Brw_FileType_t FileType,
                                            const char *FileNameToShow);
static void Brw_WriteSmallLinkToDownloadFile (const char *URL,Brw_FileType_t FileType,
                                              const char *FileNameToShow);
static bool Brw_GetParamPublicFile (void);
static Brw_License_t Brw_GetParLicense (void);
static void Brw_GetFileViewsFromLoggedUsrs (struct FileMetadata *FileMetadata);
static void Brw_GetFileViewsFromNonLoggedUsrs (struct FileMetadata *FileMetadata);
static unsigned Brw_GetFileViewsFromMe (long FilCod);
static void Brw_UpdateFileViews (unsigned NumViews,long FilCod);
static bool Brw_GetIfFolderHasPublicFiles (const char *Path);
static bool Brw_GetIfExpandedTree (const char *Path);

static void Brw_ChangeFileOrFolderHiddenInDB (const char *Path,bool IsHidden);

static void Brw_ChangeFilePublicInDB (long PublisherUsrCod,const char *Path,
                                      bool IsPublic,Brw_License_t License);

static long Brw_GetCrsCod (void);
static long Brw_GetGrpCod (void);
static long Brw_GetZoneUsrCod (void);

static void Brw_RemoveOneFileOrFolderFromDB (const char *Path);
static void Brw_RemoveChildrenOfFolderFromDB (const char *Path);
static void Brw_RenameOneFolderInDB (const char *OldPath,const char *NewPath);
static void Brw_RenameChildrenFilesOrFoldersInDB (const char *OldPath,const char *NewPath);
static bool Brw_CheckIfICanEditFileOrFolder (unsigned Level);
static bool Brw_CheckIfICanCreateIntoFolder (unsigned Level);
static bool Brw_CheckIfIHavePermissionFileOrFolderCommon (long GrpCod);

static void Brw_WriteRowDocData (unsigned *NumDocsNotHidden,MYSQL_ROW row);

/*****************************************************************************/
/***************** Get parameters related to file browser ********************/
/*****************************************************************************/

void Brw_GetParAndInitFileBrowser (void)
  {
   /***** If a group is selected, get its data *****/
   if ((Gbl.CurrentCrs.Grps.GrpCod = Brw_GetGrpSettings ()) > 0)
      Brw_GetDataCurrentGrp ();

   /***** Get type of file browser *****/
   switch (Gbl.CurrentAct)
     {
      case ActSeeAdmDoc:	// Access to a documents zone from menu
         /* Set file browser type acording to last group accessed */
	 switch (Gbl.Usrs.Me.LoggedRole)
	   {
	    case Rol_ROLE_TEACHER:
	    case Rol_ROLE_SUPERUSER:
	       /* These roles can edit documents of course/groups */
	       Gbl.FileBrowser.Type = (Gbl.CurrentCrs.Grps.GrpCod > 0) ? Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP :
								         Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS;
	       break;
	    default:
	       /* The rest of roles can not edit documentos of course/groups */
	       Gbl.FileBrowser.Type = (Gbl.CurrentCrs.Grps.GrpCod > 0) ? Brw_FILE_BRW_SEE_DOCUMENTS_GRP :
								         Brw_FILE_BRW_SEE_DOCUMENTS_CRS;
	       break;
	   }
         break;
      case ActChgToSeeDoc:	// Access to see a documents zone
         /* Set file browser type acording to last group accessed */
         Gbl.FileBrowser.Type = (Gbl.CurrentCrs.Grps.GrpCod > 0) ? Brw_FILE_BRW_SEE_DOCUMENTS_GRP :
                                                                   Brw_FILE_BRW_SEE_DOCUMENTS_CRS;
         break;
      case ActSeeDocCrs:
      case ActExpSeeDocCrs:
      case ActConSeeDocCrs:
      case ActZIPSeeDocCrs:
      case ActReqDatSeeDocCrs:
      case ActDowSeeDocCrs:
	 Gbl.FileBrowser.Type = Brw_FILE_BRW_SEE_DOCUMENTS_CRS;
         break;
      case ActSeeDocGrp:
      case ActExpSeeDocGrp:
      case ActConSeeDocGrp:
      case ActZIPSeeDocGrp:
      case ActReqDatSeeDocGrp:
      case ActDowSeeDocGrp:
	 Gbl.FileBrowser.Type = Brw_FILE_BRW_SEE_DOCUMENTS_GRP;
         break;
      case ActChgToAdmDoc:	// Access to admin a documents zone
         /* Set file browser type acording to last group accessed */
         Gbl.FileBrowser.Type = (Gbl.CurrentCrs.Grps.GrpCod > 0) ? Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP :
                                                                   Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS;
         break;
      case ActAdmDocCrs:
      case ActReqRemFilDocCrs:
      case ActRemFilDocCrs:
      case ActRemFolDocCrs:
      case ActCopDocCrs:
      case ActPasDocCrs:
      case ActRemTreDocCrs:
      case ActFrmCreDocCrs:
      case ActCreFolDocCrs:
      case ActCreLnkDocCrs:
      case ActRenFolDocCrs:
      case ActRcvFilDocCrsDZ:	case ActRcvFilDocCrsCla:
      case ActExpAdmDocCrs:
      case ActConAdmDocCrs:
      case ActZIPAdmDocCrs:
      case ActShoDocCrs:
      case ActHidDocCrs:
      case ActReqDatAdmDocCrs:
      case ActChgDatAdmDocCrs:
      case ActDowAdmDocCrs:
	 Gbl.FileBrowser.Type = Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS;
         break;
      case ActAdmDocGrp:
      case ActReqRemFilDocGrp:
      case ActRemFilDocGrp:
      case ActRemFolDocGrp:
      case ActCopDocGrp:
      case ActPasDocGrp:
      case ActRemTreDocGrp:
      case ActFrmCreDocGrp:
      case ActCreFolDocGrp:
      case ActCreLnkDocGrp:
      case ActRenFolDocGrp:
      case ActRcvFilDocGrpDZ:	case ActRcvFilDocGrpCla:
      case ActExpAdmDocGrp:
      case ActConAdmDocGrp:
      case ActZIPAdmDocGrp:
      case ActShoDocGrp:
      case ActHidDocGrp:
      case ActReqDatAdmDocGrp:
      case ActChgDatAdmDocGrp:
      case ActDowAdmDocGrp:
	 Gbl.FileBrowser.Type = Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP;
         break;
      case ActAdmCom:
      case ActChgToAdmCom:	// Access to a shared zone from menu
         /* Set file browser type acording to last group accessed */
         Gbl.FileBrowser.Type = (Gbl.CurrentCrs.Grps.GrpCod > 0) ? Brw_FILE_BRW_COMMON_GRP :
                                                                   Brw_FILE_BRW_COMMON_CRS;
         break;
      case ActAdmComCrs:
      case ActReqRemFilComCrs:
      case ActRemFilComCrs:
      case ActRemFolComCrs:
      case ActCopComCrs:
      case ActPasComCrs:
      case ActRemTreComCrs:
      case ActFrmCreComCrs:
      case ActCreFolComCrs:
      case ActCreLnkComCrs:
      case ActRenFolComCrs:
      case ActRcvFilComCrsDZ:	case ActRcvFilComCrsCla:
      case ActExpComCrs:
      case ActConComCrs:
      case ActZIPComCrs:
      case ActReqDatComCrs:
      case ActChgDatComCrs:
      case ActDowComCrs:
         Gbl.FileBrowser.Type = Brw_FILE_BRW_COMMON_CRS;
         break;
      case ActAdmComGrp:
      case ActReqRemFilComGrp:
      case ActRemFilComGrp:
      case ActRemFolComGrp:
      case ActCopComGrp:
      case ActPasComGrp:
      case ActRemTreComGrp:
      case ActFrmCreComGrp:
      case ActCreFolComGrp:
      case ActCreLnkComGrp:
      case ActRenFolComGrp:
      case ActRcvFilComGrpDZ:	case ActRcvFilComGrpCla:
      case ActExpComGrp:
      case ActConComGrp:
      case ActZIPComGrp:
      case ActReqDatComGrp:
      case ActChgDatComGrp:
      case ActDowComGrp:
         Gbl.FileBrowser.Type = Brw_FILE_BRW_COMMON_GRP;
         break;
      case ActReqRemFilAsgUsr:
      case ActRemFilAsgUsr:
      case ActRemFolAsgUsr:
      case ActCopAsgUsr:
      case ActPasAsgUsr:
      case ActRemTreAsgUsr:
      case ActFrmCreAsgUsr:
      case ActCreFolAsgUsr:
      case ActCreLnkAsgUsr:
      case ActRenFolAsgUsr:
      case ActRcvFilAsgUsrDZ:	case ActRcvFilAsgUsrCla:
      case ActExpAsgUsr:
      case ActConAsgUsr:
      case ActZIPAsgUsr:
      case ActReqDatAsgUsr:
      case ActChgDatAsgUsr:
      case ActDowAsgUsr:
         Gbl.FileBrowser.Type = Brw_FILE_BRW_ASSIGNMENTS_USR;
         break;
      case ActAdmAsgWrkCrs:
      case ActReqRemFilAsgCrs:
      case ActRemFilAsgCrs:
      case ActRemFolAsgCrs:
      case ActCopAsgCrs:
      case ActPasAsgCrs:
      case ActRemTreAsgCrs:
      case ActFrmCreAsgCrs:
      case ActCreFolAsgCrs:
      case ActCreLnkAsgCrs:
      case ActRenFolAsgCrs:
      case ActRcvFilAsgCrsDZ:	case ActRcvFilAsgCrsCla:
      case ActExpAsgCrs:
      case ActConAsgCrs:
      case ActZIPAsgCrs:
      case ActReqDatAsgCrs:
      case ActChgDatAsgCrs:
      case ActDowAsgCrs:
         Gbl.FileBrowser.Type = Brw_FILE_BRW_ASSIGNMENTS_CRS;
         break;
      case ActAdmAsgWrkUsr:
      case ActReqRemFilWrkUsr:
      case ActRemFilWrkUsr:
      case ActRemFolWrkUsr:
      case ActCopWrkUsr:
      case ActPasWrkUsr:
      case ActRemTreWrkUsr:
      case ActFrmCreWrkUsr:
      case ActCreFolWrkUsr:
      case ActCreLnkWrkUsr:
      case ActRenFolWrkUsr:
      case ActRcvFilWrkUsrDZ:	case ActRcvFilWrkUsrCla:
      case ActExpWrkUsr:
      case ActConWrkUsr:
      case ActZIPWrkUsr:
      case ActReqDatWrkUsr:
      case ActChgDatWrkUsr:
      case ActDowWrkUsr:
         Gbl.FileBrowser.Type = Brw_FILE_BRW_WORKS_USR;
         break;
      case ActReqAsgWrkCrs:
      case ActReqRemFilWrkCrs:
      case ActRemFilWrkCrs:
      case ActRemFolWrkCrs:
      case ActCopWrkCrs:
      case ActPasWrkCrs:
      case ActRemTreWrkCrs:
      case ActFrmCreWrkCrs:
      case ActCreFolWrkCrs:
      case ActCreLnkWrkCrs:
      case ActRenFolWrkCrs:
      case ActRcvFilWrkCrsDZ:	case ActRcvFilWrkCrsCla:
      case ActExpWrkCrs:
      case ActConWrkCrs:
      case ActZIPWrkCrs:
      case ActReqDatWrkCrs:
      case ActChgDatWrkCrs:
      case ActDowWrkCrs:
         Gbl.FileBrowser.Type = Brw_FILE_BRW_WORKS_CRS;
         break;
      case ActSeeAdmMrk:	// Access to a marks zone from menu
         /* Set file browser type acording to last group accessed */
         Gbl.FileBrowser.Type = (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_STUDENT) ?
				(Gbl.CurrentCrs.Grps.GrpCod > 0 ? Brw_FILE_BRW_SEE_MARKS_GRP :
								  Brw_FILE_BRW_SEE_MARKS_CRS) :
				(Gbl.CurrentCrs.Grps.GrpCod > 0 ? Brw_FILE_BRW_ADMIN_MARKS_GRP :
								  Brw_FILE_BRW_ADMIN_MARKS_CRS);
         break;
      case ActChgToSeeMrk:	// Access to see a marks zone
         /* Set file browser type acording to last group accessed */
         Gbl.FileBrowser.Type = (Gbl.CurrentCrs.Grps.GrpCod > 0) ? Brw_FILE_BRW_SEE_MARKS_GRP :
                                                                   Brw_FILE_BRW_SEE_MARKS_CRS;
         break;
      case ActSeeMrkCrs:
      case ActExpSeeMrkCrs:
      case ActConSeeMrkCrs:
      case ActReqDatSeeMrkCrs:
      case ActSeeMyMrkCrs:
         Gbl.FileBrowser.Type = Brw_FILE_BRW_SEE_MARKS_CRS;
         break;
      case ActSeeMrkGrp:
      case ActExpSeeMrkGrp:
      case ActConSeeMrkGrp:
      case ActReqDatSeeMrkGrp:
      case ActSeeMyMrkGrp:
         Gbl.FileBrowser.Type = Brw_FILE_BRW_SEE_MARKS_GRP;
         break;
      case ActChgToAdmMrk:	// Access to admin a marks zone
         /* Set file browser type acording to last group accessed */
         Gbl.FileBrowser.Type = (Gbl.CurrentCrs.Grps.GrpCod > 0) ? Brw_FILE_BRW_ADMIN_MARKS_GRP :
                                                                   Brw_FILE_BRW_ADMIN_MARKS_CRS;
         break;
      case ActAdmMrkCrs:
      case ActReqRemFilMrkCrs:
      case ActRemFilMrkCrs:
      case ActRemFolMrkCrs:
      case ActCopMrkCrs:
      case ActPasMrkCrs:
      case ActRemTreMrkCrs:
      case ActFrmCreMrkCrs:
      case ActCreFolMrkCrs:
      case ActRenFolMrkCrs:
      case ActRcvFilMrkCrsDZ:	case ActRcvFilMrkCrsCla:
      case ActExpAdmMrkCrs:
      case ActConAdmMrkCrs:
      case ActZIPAdmMrkCrs:
      case ActShoMrkCrs:
      case ActHidMrkCrs:
      case ActReqDatAdmMrkCrs:
      case ActChgDatAdmMrkCrs:
      case ActDowAdmMrkCrs:
      case ActChgNumRowHeaCrs:
      case ActChgNumRowFooCrs:
         Gbl.FileBrowser.Type = Brw_FILE_BRW_ADMIN_MARKS_CRS;
         break;
      case ActAdmMrkGrp:
      case ActReqRemFilMrkGrp:
      case ActRemFilMrkGrp:
      case ActRemFolMrkGrp:
      case ActCopMrkGrp:
      case ActPasMrkGrp:
      case ActRemTreMrkGrp:
      case ActFrmCreMrkGrp:
      case ActCreFolMrkGrp:
      case ActRenFolMrkGrp:
      case ActRcvFilMrkGrpDZ:	case ActRcvFilMrkGrpCla:
      case ActExpAdmMrkGrp:
      case ActConAdmMrkGrp:
      case ActZIPAdmMrkGrp:
      case ActShoMrkGrp:
      case ActHidMrkGrp:
      case ActReqDatAdmMrkGrp:
      case ActChgDatAdmMrkGrp:
      case ActDowAdmMrkGrp:
      case ActChgNumRowHeaGrp:
      case ActChgNumRowFooGrp:
         Gbl.FileBrowser.Type = Brw_FILE_BRW_ADMIN_MARKS_GRP;
         break;
      case ActAdmBrf:
      case ActReqRemFilBrf:
      case ActRemFilBrf:
      case ActRemFolBrf:
      case ActCopBrf:
      case ActPasBrf:
      case ActRemTreBrf:
      case ActFrmCreBrf:
      case ActCreFolBrf:
      case ActCreLnkBrf:
      case ActRenFolBrf:
      case ActRcvFilBrfDZ:	case ActRcvFilBrfCla:
      case ActExpBrf:
      case ActConBrf:
      case ActZIPBrf:
      case ActReqDatBrf:
      case ActChgDatBrf:
      case ActDowBrf:
         Gbl.FileBrowser.Type = Brw_FILE_BRW_BRIEFCASE_USR;
         break;
      default:
         Lay_ShowErrorAndExit ("The type of file browser can not be determined.");
         break;
     }

   /***** Get the path in the file browser and the name of the file or folder *****/
   Brw_GetParamsPathInTreeAndFileName ();
   Brw_SetFullPathInTree (Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,
	                  Gbl.FileBrowser.FilFolLnkName);

   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         /* Get list of the selected users's IDs */
         Usr_GetListSelectedUsrs ();
         /* Get user whose folder will be used to make any operation */
         Usr_GetParamOtherUsrCodEncrypted ();
         /* Get whether we must create the zip file or not */
         Gbl.FileBrowser.ZIP.CreateZIP = ZIP_GetCreateZIPFromForm ();
         break;
      default:
         break;
     }

   switch (Gbl.CurrentAct)
     {
      case ActCreFolDocCrs:	case ActRenFolDocCrs:
      case ActCreFolDocGrp:	case ActRenFolDocGrp:
      case ActCreFolComCrs:	case ActRenFolComCrs:
      case ActCreFolComGrp:	case ActRenFolComGrp:
      case ActCreFolMrkCrs:	case ActRenFolMrkCrs:
      case ActCreFolMrkGrp:	case ActRenFolMrkGrp:
      case ActCreFolAsgCrs:	case ActRenFolAsgCrs:
      case ActCreFolWrkCrs:	case ActRenFolWrkCrs:
      case ActCreFolAsgUsr:	case ActRenFolAsgUsr:
      case ActCreFolWrkUsr:	case ActRenFolWrkUsr:
      case ActCreFolBrf:	case ActRenFolBrf:
	 /* Get the name of the new folder */
	 Par_GetParToText ("NewFolderName",Gbl.FileBrowser.NewFilFolLnkName,NAME_MAX);
	 break;
      case ActCreLnkDocCrs:
      case ActCreLnkDocGrp:
      case ActCreLnkComCrs:
      case ActCreLnkComGrp:
      case ActCreLnkAsgCrs:
      case ActCreLnkWrkCrs:
      case ActCreLnkAsgUsr:
      case ActCreLnkWrkUsr:
      case ActCreLnkBrf:
	 /* Get the new link */
	 Par_GetParToText ("NewLink",Gbl.FileBrowser.NewFilFolLnkName,NAME_MAX);
	 break;
      default:
	 break;
     }

   /***** Get whether to show full tree *****/
   // If I belong to the current course or I am superuser, or file browser is briefcase ==> get whether show full tree from form
   // Else ==> show full tree (only public files)
   Gbl.FileBrowser.ShowOnlyPublicFiles = (Gbl.FileBrowser.Type != Brw_FILE_BRW_BRIEFCASE_USR &&
                                          !Gbl.Usrs.Me.IHaveAccessToCurrentCrs);
   Gbl.FileBrowser.FullTree = Gbl.FileBrowser.ShowOnlyPublicFiles ? true :
	                                                            Brw_GetFullTreeFromForm ();

   /***** Initialize file browser *****/
   Brw_InitializeFileBrowser ();
  }

/*****************************************************************************/
/******* Get group code from last access to current zone or from form ********/
/*****************************************************************************/

static long Brw_GetGrpSettings (void)
  {
   char LongStr[1+10+1];
   long GrpCod;

   /***** Get parameter with group code *****/
   if (Par_GetParToText ("GrpCod",LongStr,1+10))
     {
      if ((GrpCod = Str_ConvertStrCodToLongCod (LongStr)) <= 0)
         GrpCod = -1L;
      return GrpCod;
     }
   else	// Parameter GrpCod not found!
      /***** Try to get group code from database *****/
      switch (Gbl.CurrentAct)
	{
	 case ActSeeAdmDoc:
	 case ActSeeDocGrp:
	 case ActAdmDocGrp:	// Access to a documents zone from menu
	    return Brw_GetGrpLastAccZone ("LastDowGrpCod");
	 case ActAdmCom:
	 case ActAdmComGrp:	// Access to a shared documents zone from menu
	    return Brw_GetGrpLastAccZone ("LastComGrpCod");
	 case ActSeeAdmMrk:
	 case ActSeeMrkGrp:
	 case ActAdmMrkGrp:	// Access to a marks zone from menu
	    return Brw_GetGrpLastAccZone ("LastAssGrpCod");
	 default:
	    return -1L;
	}
  }

/*****************************************************************************/
/******************* If a group is selected, get its data ********************/
/*****************************************************************************/

static void Brw_GetDataCurrentGrp (void)
  {
   struct GroupData GrpDat;

   if (Gbl.CurrentCrs.Grps.GrpCod > 0)
     {
      GrpDat.GrpCod = Gbl.CurrentCrs.Grps.GrpCod;
      Grp_GetDataOfGroupByCod (&GrpDat);

      switch (Gbl.CurrentAct)
	{
	 case ActSeeAdmDoc:	// Access to see/admin a documents zone from menu
	 case ActChgToSeeDoc:	// Access to see a documents zone
	 case ActSeeDocGrp:	// Access to see a documents zone

	 case ActChgToAdmDoc:	// Access to admin a documents zone
	 case ActAdmDocGrp:	// Access to admin a documents zone

	 case ActChgToAdmCom:	// Access to admin a common zone
	 case ActAdmComGrp:	// Access to admin a common zone

	 case ActSeeAdmMrk:	// Access to see/admin a marks zone from menu
	 case ActChgToSeeMrk:	// Access to see a marks zone
	 case ActSeeMrkGrp:	// Access to see a marks zone

	 case ActChgToAdmMrk:	// Access to admin a marks zone
	 case ActAdmMrkGrp:	// Access to admin a marks zone
	    /***** For security, check if group file zones are enabled,
		   and check if I belongs to the selected group *****/
	    if (!GrpDat.FileZones || !Grp_GetIfIBelongToGrp (Gbl.CurrentCrs.Grps.GrpCod))
	       Gbl.CurrentCrs.Grps.GrpCod = -1L;	// Go to course zone
	    break;
	 default:
	    /***** For security, check if group file zones are enabled,
		   and check if I belongs to the selected group *****/
	    if (!GrpDat.FileZones)
	       Lay_ShowErrorAndExit ("The group has no file zones.");
	    else if (!Grp_GetIfIBelongToGrp (Gbl.CurrentCrs.Grps.GrpCod))
	       Lay_ShowErrorAndExit ("You don't have access to the group.");
	    break;
	}

      /***** Get data of the current group *****/
      if (Gbl.CurrentCrs.Grps.GrpCod > 0)
	{
	 Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod           = GrpDat.GrpTypCod;
	 strcpy (Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName  , GrpDat.GrpTypName);
	 strcpy (Gbl.CurrentCrs.Grps.GrpName            , GrpDat.GrpName);
	 Gbl.CurrentCrs.Grps.MaxStudents                = GrpDat.MaxStudents;
	 Gbl.CurrentCrs.Grps.Open                       = GrpDat.Open;
	 Gbl.CurrentCrs.Grps.FileZones                  = GrpDat.FileZones;
	 Gbl.CurrentCrs.Grps.GrpTyp.MultipleEnrollment  = GrpDat.MultipleEnrollment;
	}
     }
  }

/*****************************************************************************/
/** Put hidden params. with the path in the tree and the name of file/folder */
/*****************************************************************************/

void Brw_PutParamsPathAndFile (Brw_FileType_t FileType,const char *PathInTree,const char *FileFolderName)
  {
   Par_PutHiddenParamString ("Path",PathInTree);
   Par_PutHiddenParamString (Brw_FileTypeParamName[FileType],FileFolderName);
  }

/*****************************************************************************/
/************** Get parameters path and file in file browser *****************/
/*****************************************************************************/

static void Brw_GetParamsPathInTreeAndFileName (void)
  {
   const char *Ptr;
   unsigned i;
   Brw_FileType_t FileType;

   /***** Get the path inside the tree (this path does not include the name of the file or folder at the end) *****/
   Par_GetParToText ("Path",Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,PATH_MAX);
   if (strstr (Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,".."))	// ".." is not allowed in the path
      Lay_ShowErrorAndExit ("Wrong path.");

   /***** Get the name of the file, folder or link *****/
   Gbl.FileBrowser.FileType = Brw_IS_UNKNOWN;
   for (FileType = (Brw_FileType_t) 0;
	FileType < Brw_NUM_FILE_TYPES;
	FileType++)
      // File names with heading and trailing spaces are allowed
      if (Par_GetParAndChangeFormat (Brw_FileTypeParamName[FileType],
                                     Gbl.FileBrowser.FilFolLnkName,
                                     NAME_MAX,Str_TO_TEXT,false))
	{
	 if (strstr (Gbl.FileBrowser.FilFolLnkName,".."))	// ".." is not allowed in the path
	    Lay_ShowErrorAndExit ("Wrong path.");

	 Gbl.FileBrowser.FileType = FileType;
	 break;
	}

   /***** Set level of this file or folder inside file browser *****/
   if (!strcmp (Gbl.FileBrowser.FilFolLnkName,"."))
      Gbl.FileBrowser.Level = 0;
   else
     {
      // Level == number-of-slashes-in-path-except-file-or-folder + 1
      Gbl.FileBrowser.Level = 1;
      for (Ptr = Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder;
	   *Ptr;
	   Ptr++)
         if (*Ptr == '/')
            Gbl.FileBrowser.Level++;
     }

   /***** Get data of assignment *****/
   if (Gbl.FileBrowser.Level &&
       (Gbl.FileBrowser.Type == Brw_FILE_BRW_ASSIGNMENTS_USR ||
        Gbl.FileBrowser.Type == Brw_FILE_BRW_ASSIGNMENTS_CRS))
     {
      if (Gbl.FileBrowser.Level == 1)
        {
         // We are in this case: assignments/assignment-folder
         strncpy (Gbl.FileBrowser.Asg.Folder,Gbl.FileBrowser.FilFolLnkName,Asg_MAX_LENGTH_FOLDER);
         Gbl.FileBrowser.Asg.Folder[Asg_MAX_LENGTH_FOLDER] = '\0';
        }
      else
        {
         // We are in this case: assignments/assignment-folder/rest-of-path
         for (Ptr = Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder;
              *Ptr && *Ptr != '/';
              Ptr++);	// Go to first '/'
         if (*Ptr == '/')
            Ptr++;	// Skip '/'
         for (i = 0;
              i < Asg_MAX_LENGTH_FOLDER && *Ptr && *Ptr != '/';
              i++, Ptr++)
            Gbl.FileBrowser.Asg.Folder[i] = *Ptr;	// Copy assignment folder
         Gbl.FileBrowser.Asg.Folder[i] = '\0';
        }
      Asg_GetDataOfAssignmentByFolder (&Gbl.FileBrowser.Asg);
     }
  }

/*****************************************************************************/
/************************* Initialize file browser ***************************/
/*****************************************************************************/
// Gbl.FileBrowser.Type must be set to a valid file browser

void Brw_InitializeFileBrowser (void)
  {
   /***** Construct the relative path to the folder of file browser *****/
   Brw_SetPathFileBrowser ();

   /***** Other initializations *****/
   Brw_ResetFileBrowserSize ();
  }

/*****************************************************************************/
/********* Construct the paths to the top folders of file browser ************/
/*****************************************************************************/

static void Brw_SetPathFileBrowser (void)
  {
   char Path[PATH_MAX+1];

   /***** Reset paths. An empty path means that
          we don't have to create that directory *****/
   Gbl.FileBrowser.Priv.PathAboveRootFolder[0] = '\0';
   Gbl.FileBrowser.Priv.PathRootFolder[0] = '\0';

   /***** Set paths depending on file browser *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
      case Brw_FILE_BRW_COMMON_CRS:
      case Brw_FILE_BRW_SEE_MARKS_CRS:
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
         strcpy (Gbl.FileBrowser.Priv.PathAboveRootFolder,
                 Gbl.CurrentCrs.PathPriv);
	 break;
      case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_SEE_MARKS_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         sprintf (Path,"%s/grp",
                  Gbl.CurrentCrs.PathPriv);
         Fil_CreateDirIfNotExists (Path);

         sprintf (Gbl.FileBrowser.Priv.PathAboveRootFolder,"%s/grp/%ld",
                  Gbl.CurrentCrs.PathPriv,
                  Gbl.CurrentCrs.Grps.GrpCod);
	 break;
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_WORKS_USR:
         sprintf (Path,"%s/usr",
                  Gbl.CurrentCrs.PathPriv);
         Fil_CreateDirIfNotExists (Path);
         sprintf (Path,"%s/usr/%02u",
                  Gbl.CurrentCrs.PathPriv,
                  (unsigned) (Gbl.Usrs.Me.UsrDat.UsrCod % 100));
         Fil_CreateDirIfNotExists (Path);

         sprintf (Gbl.FileBrowser.Priv.PathAboveRootFolder,"%s/usr/%02u/%ld",
                  Gbl.CurrentCrs.PathPriv,
                  (unsigned) (Gbl.Usrs.Me.UsrDat.UsrCod % 100),
                  Gbl.Usrs.Me.UsrDat.UsrCod);
	 break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
           {
            sprintf (Path,"%s/usr",
        	     Gbl.CurrentCrs.PathPriv);
            Fil_CreateDirIfNotExists (Path);
	    sprintf (Path,"%s/usr/%02u",
		     Gbl.CurrentCrs.PathPriv,
		     (unsigned) (Gbl.Usrs.Other.UsrDat.UsrCod % 100));
	    Fil_CreateDirIfNotExists (Path);

            sprintf (Gbl.FileBrowser.Priv.PathAboveRootFolder,"%s/usr/%02u/%ld",
        	     Gbl.CurrentCrs.PathPriv,
                     (unsigned) (Gbl.Usrs.Other.UsrDat.UsrCod % 100),
        	     Gbl.Usrs.Other.UsrDat.UsrCod);
           }
         break;
      case Brw_FILE_BRW_BRIEFCASE_USR:
         strcpy (Gbl.FileBrowser.Priv.PathAboveRootFolder,
                 Gbl.Usrs.Me.PathDir);
	 break;
      default:
	 return;
     }

   /***** Create directories that not exist *****/
   if (Gbl.FileBrowser.Priv.PathAboveRootFolder[0])
     {
      Fil_CreateDirIfNotExists (Gbl.FileBrowser.Priv.PathAboveRootFolder);
      sprintf (Gbl.FileBrowser.Priv.PathRootFolder,"%s/%s",
               Gbl.FileBrowser.Priv.PathAboveRootFolder,
               Brw_RootFolderInternalNames[Gbl.FileBrowser.Type]);
      Fil_CreateDirIfNotExists (Gbl.FileBrowser.Priv.PathRootFolder);

      /***** If file browser is for assignments,
             create folders of assignments if not exist *****/
      if (Gbl.FileBrowser.Type == Brw_FILE_BRW_ASSIGNMENTS_USR ||
	  Gbl.FileBrowser.Type == Brw_FILE_BRW_ASSIGNMENTS_CRS)
	 Brw_CreateFoldersAssignmentsIfNotExist ();
     }
  }

/*****************************************************************************/
/****** Check if exists a folder of assignments for any user in course *******/
/*****************************************************************************/
// Folders are in level 1, just under root folder

bool Brw_CheckIfExistsFolderAssigmentForAnyUsr (const char *FolderName)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   long UsrCod;
   char PathFolder[PATH_MAX+1];
   bool FolderExists = false;

   /***** Get all the users belonging to current course from database *****/
   sprintf (Query,"SELECT UsrCod FROM crs_usr WHERE CrsCod='%ld'",
            Gbl.CurrentCrs.Crs.CrsCod);
   NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get users from current course");

   /***** Check folders *****/
   for (NumUsr = 0;
	NumUsr < NumUsrs && !FolderExists;
	NumUsr++)
     {
      /* Get next user */
      row = mysql_fetch_row (mysql_res);
      UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Check if folder exists */
      sprintf (PathFolder,"%s/usr/%02u/%ld/%s/%s",
               Gbl.CurrentCrs.PathPriv,
               (unsigned) (UsrCod % 100),
               UsrCod,	// User's code
               Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
               FolderName);
      FolderExists = Fil_CheckIfPathExists (PathFolder);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return FolderExists;
  }

/*****************************************************************************/
/********* Create folders of assignments if not exist for one user ***********/
/*****************************************************************************/
// Folders are created in level 1, just under root folder
// Create a folder of and assignment when:
// 1. The assignment is visible (not hidden)
// 2. ...and the folder name is not empty (the teacher has set that the user must send work(s) for that assignment)
// 3. ...and the assignment is open (StartTime <= now <= EndTime)
// 4. ...the assignment is not restricted to groups or (if restricted to groups) I belong to any of the groups

static void Brw_CreateFoldersAssignmentsIfNotExist (void)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows,NumRow;
   char PathFolderAsg[PATH_MAX+1];

   /***** Get assignment folders from database *****/
   sprintf (Query,"SELECT Folder FROM assignments"
                  " WHERE CrsCod='%ld' AND Hidden='N' AND Folder<>'' AND StartTime<=NOW() AND EndTime>=NOW()"
                  " AND (AsgCod NOT IN (SELECT AsgCod FROM asg_grp) OR"
                  " AsgCod IN (SELECT asg_grp.AsgCod FROM asg_grp,crs_grp_usr"
                  " WHERE crs_grp_usr.UsrCod='%ld' AND asg_grp.GrpCod=crs_grp_usr.GrpCod))",
            Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get folders of assignments");

   /***** Create one folder for each assignment *****/
   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      /* Get next assignment with folder */
      row = mysql_fetch_row (mysql_res);

      /* Create folder if not exists */
      sprintf (PathFolderAsg,"%s/%s",Gbl.FileBrowser.Priv.PathRootFolder,row[0]);
      Fil_CreateDirIfNotExists (PathFolderAsg);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**** Update folders of assignments if exist for all the users in course *****/
/*****************************************************************************/
// Folders are in level 1, just under root folder

bool Brw_UpdateFoldersAssigmentsIfExistForAllUsrs (const char *OldFolderName,const char *NewFolderName)
  {
   extern const char *Txt_Can_not_rename_a_folder_of_assignment;
   extern const char *Txt_Users;
   extern const char *Txt_Folders_renamed;
   extern const char *Txt_Folders_not_renamed;
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   long UsrCod;
   char OldPath[PATH_MAX+1];
   char NewPath[PATH_MAX+1];
   char PathOldFolder[PATH_MAX+1];
   char PathNewFolder[PATH_MAX+1];
   bool RenamingIsPossible = true;
   unsigned NumUsrsError = 0;
   unsigned NumUsrsSuccess = 0;

   /***** Get all the users belonging to current course from database *****/
   sprintf (Query,"SELECT UsrCod FROM crs_usr WHERE CrsCod='%ld'",
            Gbl.CurrentCrs.Crs.CrsCod);
   NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get users from current course");

   /***** Check if there exist folders with the new name *****/
   for (NumUsr = 0;
	NumUsr < NumUsrs && RenamingIsPossible;
	NumUsr++)
     {
      /* Get next user */
      row = mysql_fetch_row (mysql_res);
      UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Rename folder if exists */
      sprintf (PathOldFolder,"%s/usr/%02u/%ld/%s/%s",
               Gbl.CurrentCrs.PathPriv,
               (unsigned) (UsrCod % 100),
               UsrCod,	// User's code
               Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
               OldFolderName);
      sprintf (PathNewFolder,"%s/usr/%02u/%ld/%s/%s",
               Gbl.CurrentCrs.PathPriv,
               (unsigned) (UsrCod % 100),
               UsrCod,	// User's code
               Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
               NewFolderName);
      if (Fil_CheckIfPathExists (PathOldFolder) &&
          Fil_CheckIfPathExists (PathNewFolder))
         RenamingIsPossible = false;
     }

   /***** Rename folder for each user *****/
   if (RenamingIsPossible)
     {
      mysql_data_seek (mysql_res,0);
      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
        {
	 /* Get next user */
	 row = mysql_fetch_row (mysql_res);
	 UsrCod = Str_ConvertStrCodToLongCod (row[0]);

         /* Rename folder if exists */
         sprintf (PathOldFolder,"%s/usr/%02u/%ld/%s/%s",
                  Gbl.CurrentCrs.PathPriv,
                  (unsigned) (UsrCod % 100),
                  UsrCod,	// User's code
                  Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
                  OldFolderName);
         if (Fil_CheckIfPathExists (PathOldFolder))
           {
            sprintf (PathNewFolder,"%s/usr/%02u/%ld/%s/%s",
                     Gbl.CurrentCrs.PathPriv,
		     (unsigned) (UsrCod % 100),
		     UsrCod,	// User's code
                     Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
                     NewFolderName);
            if (rename (PathOldFolder,PathNewFolder))
	      {
               Lay_ShowAlert (Lay_ERROR,Txt_Can_not_rename_a_folder_of_assignment);
               NumUsrsError++;
	      }
            else
              {
               /* Remove affected clipboards */
               Brw_RemoveAffectedClipboards (Brw_FILE_BRW_ASSIGNMENTS_USR,UsrCod,-1L);
               Brw_RemoveAffectedClipboards (Brw_FILE_BRW_ASSIGNMENTS_CRS,-1L,UsrCod);

               /* Rename affected expanded folders */
               sprintf (OldPath,"%s/%s",Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,OldFolderName);
               sprintf (NewPath,"%s/%s",Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,NewFolderName);
               Brw_RenameAffectedExpandedFolders (Brw_FILE_BRW_ASSIGNMENTS_USR,UsrCod,-1L,OldPath,NewPath);
               Brw_RenameAffectedExpandedFolders (Brw_FILE_BRW_ASSIGNMENTS_CRS,-1L,UsrCod,OldPath,NewPath);

               NumUsrsSuccess++;
              }
           }
        }

      /***** Summary message *****/
      sprintf (Gbl.Message,"%s: %u<br />"
                           "%s: %u<br />"
                           "%s: %u.",
               Txt_Users,NumUsrs,
               Txt_Folders_renamed,NumUsrsSuccess,
               Txt_Folders_not_renamed,NumUsrsError);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);
     }
   else
      /***** Warning message *****/
      Lay_ShowAlert (Lay_WARNING,Txt_Can_not_rename_a_folder_of_assignment);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return RenamingIsPossible;
  }

/*****************************************************************************/
/**** Remove folders of assignments if exist for all the users in course *****/
/*****************************************************************************/
// Folders are in level 1, just under root folder

void Brw_RemoveFoldersAssignmentsIfExistForAllUsrs (const char *FolderName)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   long UsrCod;
   char PathFolder[PATH_MAX+1];

   /***** Get all the users belonging to current course from database *****/
   sprintf (Query,"SELECT UsrCod FROM crs_usr WHERE CrsCod='%ld'",
            Gbl.CurrentCrs.Crs.CrsCod);
   NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get users from current course");

   /***** Remove folders *****/
   for (NumUsr = 0;
	NumUsr < NumUsrs;
	NumUsr++)
     {
      /* Get next user */
      row = mysql_fetch_row (mysql_res);
      UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Remove tree if exists */
      sprintf (PathFolder,"%s/usr/%02u/%ld/%s/%s",
               Gbl.CurrentCrs.PathPriv,
               (unsigned) (UsrCod % 100),
               UsrCod,	// User's code
               Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
               FolderName);
      Brw_RemoveTree (PathFolder);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*** Initialize maximum quota of current file browser and check if exceded ***/
/*****************************************************************************/

static void Brw_SetAndCheckQuota (void)
  {
   extern const char *Txt_Quota_exceeded;

   /***** Check the quota *****/
   Brw_SetMaxQuota ();
   Brw_CalcSizeOfDir (Gbl.FileBrowser.Priv.PathRootFolder);
   if (Brw_CheckIfQuotaExceded ())
      Lay_ShowAlert (Lay_WARNING,Txt_Quota_exceeded);
  }

/*****************************************************************************/
/************ Initialize maximum quota of current file browser ***************/
/*****************************************************************************/

static void Brw_SetMaxQuota (void)
  {
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_DOWNLOAD_CRS;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_DOWNLOAD_CRS;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDERS_DOWNLOAD_CRS;
	 break;
      case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_DOWNLOAD_GRP;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_DOWNLOAD_GRP;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDERS_DOWNLOAD_GRP;
	 break;
      case Brw_FILE_BRW_COMMON_CRS:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_COMMON_CRS;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_COMMON_CRS;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDERS_COMMON_CRS;
	 break;
      case Brw_FILE_BRW_COMMON_GRP:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_COMMON_GRP;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_COMMON_GRP;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDERS_COMMON_GRP;
	 break;
      case Brw_FILE_BRW_SEE_MARKS_CRS:
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_GRADES_CRS;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_GRADES_CRS;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDERS_GRADES_CRS;
	 break;
      case Brw_FILE_BRW_SEE_MARKS_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_GRADES_GRP;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_GRADES_GRP;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDERS_GRADES_GRP;
	 break;
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_ASSIGNMENTS_PER_STD;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_ASSIGNMENTS_PER_STD;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDERS_ASSIGNMENTS_PER_STD;
	 break;
      case Brw_FILE_BRW_WORKS_USR:
      case Brw_FILE_BRW_WORKS_CRS:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_WORKS_PER_STD;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_WORKS_PER_STD;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDERS_WORKS_PER_STD;
	 break;
      case Brw_FILE_BRW_BRIEFCASE_USR:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTAS_BRIEFCASE[Gbl.Usrs.Me.MaxRole];
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_BRIEFCASE;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDERS_BRIEFCASE;
	 break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/********************** Check if quota has been exceeded *********************/
/*****************************************************************************/

static bool Brw_CheckIfQuotaExceded (void)
  {
   return (Gbl.FileBrowser.Size.NumLevls > Brw_MAX_DIR_LEVELS ||
           Gbl.FileBrowser.Size.NumFolds > Gbl.FileBrowser.Size.MaxFolds ||
           Gbl.FileBrowser.Size.NumFiles > Gbl.FileBrowser.Size.MaxFiles ||
           Gbl.FileBrowser.Size.TotalSiz > Gbl.FileBrowser.Size.MaxQuota);
  }

/*****************************************************************************/
/************** Request edition of works of users of the course **************/
/*****************************************************************************/

void Brw_AskEditWorksCrs (void)
  {
   extern const char *Txt_View_works;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get and update type of list, number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Show form to select the groups *****/
   Grp_ShowFormToSelectSeveralGroups (ActReqAsgWrkCrs);

   /***** Form to select type of list used for select several users *****/
   Usr_ShowFormsToSelectUsrListType (ActReqAsgWrkCrs);

   /***** Get and order lists of users from this course *****/
   Usr_GetUsrsLst (Rol_ROLE_TEACHER,Sco_SCOPE_COURSE,NULL,false);
   Usr_GetUsrsLst (Rol_ROLE_STUDENT,Sco_SCOPE_COURSE,NULL,false);

   if (Gbl.Usrs.LstTchs.NumUsrs ||
       Gbl.Usrs.LstStds.NumUsrs)
     {
      if (Usr_GetIfShowBigList (Gbl.Usrs.LstTchs.NumUsrs +
	                        Gbl.Usrs.LstStds.NumUsrs))
        {
         /***** Draw class photos to select users *****/
         /* Form start */
         fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
         Act_FormStart (ActAdmAsgWrkCrs);
         Grp_PutParamsCodGrps ();
         Par_PutHiddenParamChar ("FullTree",'Y');	// By default, show all files

         /* Put list of users to select some of them */
         Lay_StartRoundFrameTable10 (NULL,0,NULL);
         Usr_ListUsersToSelect (Rol_ROLE_TEACHER);
         Usr_ListUsersToSelect (Rol_ROLE_STUDENT);
         Lay_EndRoundFrameTable10 ();

         /* Button to send the form */
         Lay_PutSendButton (Txt_View_works);
         fprintf (Gbl.F.Out,"</form>"
	                    "</div>");
        }
     }
   else
      Usr_ShowWarningNoUsersFound (Rol_ROLE_UNKNOWN);

   /***** Free memory for users' list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstTchs);
   Usr_FreeUsrsList (&Gbl.Usrs.LstStds);

   /***** Free the memory used by the list of users *****/
   Usr_FreeListsEncryptedUsrCods ();

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/*********** Show normal file browser (not for assignments-works) ************/
/*****************************************************************************/

static void Brw_ShowFileBrowserNormal (void)
  {
   /***** Write top before showing file browser *****/
   Brw_WriteTopBeforeShowingFileBrowser ();

   /****** Show the file browser *****/
   Brw_ShowFileBrowser ();
  }

/*****************************************************************************/
/*** Show file browsers with works files of several users of current course **/
/*****************************************************************************/

static void Brw_ShowFileBrowsersAsgWrkCrs (void)
  {
   extern const char *Txt_You_must_select_one_ore_more_users;
   const char *Ptr;
   struct UsrData UsrDat;

   /***** Check the number of users whose works will be shown *****/
   if (Usr_CountNumUsrsInEncryptedList ())				// If some users are selected...
     {
      if (Gbl.FileBrowser.ZIP.CreateZIP)
	{
	 /***** Create zip file with the assignments and works of the selected users *****/
	 /* Create temporary directory for the compression of assignments and works */
	 ZIP_CreateTmpDirForCompression ();

	 /* Initialize structure with user's data */
	 Usr_UsrDataConstructor (&UsrDat);

	 /* Create temporary directory for each selected user inside the directory used for compression */
	 Ptr = Gbl.Usrs.Select.All;
	 while (*Ptr)
	   {
	    Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDat.EncryptedUsrCod,Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64);
	    Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))               // Get user's data from database
	       if (Usr_CheckIfUsrBelongsToCrs (UsrDat.UsrCod,Gbl.CurrentCrs.Crs.CrsCod))
		  ZIP_CreateDirCompressionUsr (&UsrDat);
	   }

	 /* Free memory used for user's data */
	 Usr_UsrDataDestructor (&UsrDat);

	 /* Create the zip file and put a link to download it */
         ZIP_CreateZIPAsgWrk ();
	}
      else
	 /***** Button to create a zip file with all the works of the selected users *****/
	 ZIP_PutButtonToCreateZIPAsgWrk ();

      /***** Write top before showing file browser *****/
      Brw_WriteTopBeforeShowingFileBrowser ();

      /***** Header of the table with the list of users *****/
      Lay_StartRoundFrameTable10 (NULL,0,NULL);

      /***** List the assignments and works of the selected users *****/
      Ptr = Gbl.Usrs.Select.All;
      while (*Ptr)
	{
	 Par_GetNextStrUntilSeparParamMult (&Ptr,Gbl.Usrs.Other.UsrDat.EncryptedUsrCod,Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64);
	 Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))               // Get of the database the data of the user
	    if (Usr_CheckIfUsrBelongsToCrs (Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.CurrentCrs.Crs.CrsCod))
	      {
	       /***** Show a row with the data of the owner of the works *****/
	       fprintf (Gbl.F.Out,"<tr>");
	       Brw_ShowDataOwnerAsgWrk (&Gbl.Usrs.Other.UsrDat);

	       fprintf (Gbl.F.Out,"<td style=\"text-align:left;"
		                  " vertical-align:top;\">");

	       /***** Show the tree with the assignments *****/
	       Gbl.FileBrowser.Type = Brw_FILE_BRW_ASSIGNMENTS_CRS;
	       Brw_InitializeFileBrowser ();
	       Brw_ShowFileBrowser ();

	       /***** Show the tree with the works *****/
	       Gbl.FileBrowser.Type = Brw_FILE_BRW_WORKS_CRS;
	       Brw_InitializeFileBrowser ();
	       Brw_ShowFileBrowser ();

	       fprintf (Gbl.F.Out,"</td>"
		                  "</tr>");
	      }
	}

      /***** End of the table *****/
      Lay_EndRoundFrameTable10 ();
     }
   else	// If no users are selected...
     {
      // ...write warning alert
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_select_one_ore_more_users);
      // ...and show again the form
      Brw_AskEditWorksCrs ();
     }

   /***** Free the memory used for the list of users *****/
   Usr_FreeListsEncryptedUsrCods ();
  }

/*****************************************************************************/
/************ Show file browsers with works files of one user ****************/
/*****************************************************************************/

static void Brw_ShowFileBrowsersAsgWrkUsr (void)
  {
   /***** Write top before showing file browser *****/
   Brw_WriteTopBeforeShowingFileBrowser ();

   /***** Show the tree with the assignments *****/
   Gbl.FileBrowser.Type = Brw_FILE_BRW_ASSIGNMENTS_USR;
   Brw_InitializeFileBrowser ();
   Brw_ShowFileBrowser ();

   /***** Show the tree with the works *****/
   Gbl.FileBrowser.Type = Brw_FILE_BRW_WORKS_USR;
   Brw_InitializeFileBrowser ();
   Brw_ShowFileBrowser ();
  }

/*****************************************************************************/
/*************************** Form to change file zone ************************/
/*****************************************************************************/

static void Brw_FormToChangeZone (void)
  {
   struct ListCodGrps LstMyGrps;
   unsigned NumGrp;
   struct GroupData GrpDat;
   bool IsCourseZone = Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_DOCUMENTS_CRS   ||
                       Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS ||
                       Gbl.FileBrowser.Type == Brw_FILE_BRW_COMMON_CRS          ||
                       Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_MARKS_CRS       ||
                       Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_CRS;
   bool IsGroupZone  = Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_DOCUMENTS_GRP   ||
                       Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP ||
                       Gbl.FileBrowser.Type == Brw_FILE_BRW_COMMON_GRP          ||
                       Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_MARKS_GRP       ||
                       Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_GRP;

   /***** Get list of groups to show *****/
   if (Gbl.CurrentCrs.Grps.NumGrps)	// This course has groups?
      /* Get list of group with file zones which I belong to */
      Grp_GetLstCodGrpsWithFileZonesIBelong (&LstMyGrps);

   /***** Form start *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
      case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
         Act_FormStart (ActChgToSeeDoc);
         break;
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         Act_FormStart (ActChgToAdmDoc);
         break;
      case Brw_FILE_BRW_COMMON_CRS:
      case Brw_FILE_BRW_COMMON_GRP:
         Act_FormStart (ActChgToAdmCom);
         break;
      case Brw_FILE_BRW_SEE_MARKS_CRS:
      case Brw_FILE_BRW_SEE_MARKS_GRP:
         Act_FormStart (ActChgToSeeMrk);
         break;
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         Act_FormStart (ActChgToAdmMrk);
         break;
      default:
         break;
     }

   /***** List start *****/
   fprintf (Gbl.F.Out,"<ul style=\"list-style-type:none;"
	              " padding-top:0; margin-top:0; text-align:left;\">");

   /***** Select the complete course, not a group *****/
   fprintf (Gbl.F.Out,"<li class=\"%s\">"
                      "<input type=\"radio\" name=\"GrpCod\" value=\"-1\"",
            IsCourseZone ? "BROWSER_TITLE" :
                           "BROWSER_TITLE_LIGHT");
   if (IsCourseZone)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   fprintf (Gbl.F.Out," onclick=\"javascript:document.getElementById('%s').submit();\" />"
                      "%s"
                      "</li>",
            Gbl.FormId,
            Gbl.CurrentCrs.Crs.FullName);

   /***** List my groups for unique selection *****/
   if (Gbl.CurrentCrs.Grps.NumGrps)	// This course has groups?
     {
      for (NumGrp = 0;
	   NumGrp < LstMyGrps.NumGrps;
	   NumGrp++)
        {
         /* Get next group */
         GrpDat.GrpCod = LstMyGrps.GrpCod[NumGrp];
         Grp_GetDataOfGroupByCod (&GrpDat);

         /* Select this group */
         fprintf (Gbl.F.Out,"<li class=\"%s\">"
                            "<img src=\"%s/%s20x20.gif\""
                            " style=\"width:20px; height:20px; vertical-align:top; margin-left:5px;\" />"
	                    "<input type=\"radio\" name=\"GrpCod\" value=\"%ld\"",
                  (IsGroupZone &&
                   GrpDat.GrpCod == Gbl.CurrentCrs.Grps.GrpCod) ? "BROWSER_TITLE" :
                                                                  "BROWSER_TITLE_LIGHT",
                  Gbl.Prefs.IconsURL,
                  NumGrp < LstMyGrps.NumGrps - 1 ? "submid" :
                	                           "subend",
	          GrpDat.GrpCod);
	 if (IsGroupZone && GrpDat.GrpCod == Gbl.CurrentCrs.Grps.GrpCod)
	    fprintf (Gbl.F.Out," checked=\"checked\"");
	 fprintf (Gbl.F.Out," onclick=\"javascript:document.getElementById('%s').submit();\" />"
			    "%s %s"
			    "</li>",
		  Gbl.FormId,
                  GrpDat.GrpTypName,GrpDat.GrpName);
        }

      /***** Free memory with the list of groups I belong to *****/
      Grp_FreeListCodGrp (&LstMyGrps);
     }

   /***** End list and form *****/
   fprintf (Gbl.F.Out,"</ul>"
                      "</form>");
  }

/*****************************************************************************/
/*** Get the data of the selected group in order to enter its common zone ****/
/*****************************************************************************/

static void Brw_GetSelectedGroupData (struct GroupData *GrpDat,bool AbortOnError)
  {
   if (GrpDat->GrpCod > 0)
     {
      /***** Get the data of the selected group *****/
      Grp_GetDataOfGroupByCod (GrpDat);

      /***** For security, check if group file zones are enabled,
             and check if I belongs to the selected group *****/
      if (!GrpDat->FileZones)
        {
         if (AbortOnError)
            Lay_ShowErrorAndExit ("The file browser is disabled.");
         GrpDat->GrpCod = -1L;
        }
      else if (!Grp_GetIfIBelongToGrp (GrpDat->GrpCod))
        {
         if (AbortOnError)
            Lay_ShowErrorAndExit ("You don't have permission to access this group.");
         GrpDat->GrpCod = -1L;
        }
     }
  }

/*****************************************************************************/
/******** Show a row with the data of the owner of assignments/works *********/
/*****************************************************************************/

static void Brw_ShowDataOwnerAsgWrk (struct UsrData *UsrDat)
  {
   extern const char *Txt_Write_a_message_to_X;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];

   /***** Show user's photo *****/
   fprintf (Gbl.F.Out,"<td style=\"width:80px;"
	              " text-align:left; vertical-align:top;\">");
   ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                	                NULL,
                     "PHOTO75x100",true);
   fprintf (Gbl.F.Out,"</td>");

   /***** Start form to send a message to this user *****/
   fprintf (Gbl.F.Out,"<td class=\"MSG_AUT\" style=\"width:160px;"
	              " text-align:left; vertical-align:top;\">");
   Act_FormStart (UsrDat->RoleInCurrentCrsDB == Rol_ROLE_STUDENT ? ActSeeRecOneStd :
	                                                           ActSeeRecOneTch);
   Usr_PutParamOtherUsrCodEncrypted (UsrDat->EncryptedUsrCod);

   /***** Show user's ID *****/
   ID_WriteUsrIDs (UsrDat,
                   UsrDat->RoleInCurrentCrsDB == Rol_ROLE_TEACHER ? ID_ICanSeeTeacherID (UsrDat) :
                                                                    (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_TEACHER));

   /***** Show user's name *****/
   fprintf (Gbl.F.Out,"<br />");
   sprintf (Gbl.Title,Txt_Write_a_message_to_X,
            UsrDat->FullName);
   Act_LinkFormSubmit (Gbl.Title,"MSG_AUT");
   fprintf (Gbl.F.Out,"%s",UsrDat->Surname1);
   if (UsrDat->Surname2[0])
      fprintf (Gbl.F.Out," %s",UsrDat->Surname2);
   if (UsrDat->FirstName[0])
      fprintf (Gbl.F.Out,", %s",UsrDat->FirstName);
   fprintf (Gbl.F.Out,"</a>");

   /***** Show user's e-mail *****/
   if (UsrDat->Email[0])
     {
      fprintf (Gbl.F.Out,"<br />"
	                 "<a href=\"mailto:%s\" target=\"_blank\" class=\"MSG_AUT\">",
               UsrDat->Email);
      Str_LimitLengthHTMLStr (UsrDat->Email,25);
      fprintf (Gbl.F.Out,"%s</a>",UsrDat->Email);
     }
   fprintf (Gbl.F.Out,"</form>"
	              "</td>");
  }

/*****************************************************************************/
/******************* Show a file browser or students' works  *****************/
/*****************************************************************************/

void Brw_ShowFileBrowserOrWorks (void)
  {
   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Show the file browser or works *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/******************* Show a file browser or users' works  ********************/
/*****************************************************************************/

void Brw_ShowAgainFileBrowserOrWorks (void)
  {
   extern const char *Txt_Files_of_marks_must_contain_a_table_in_HTML_format_;
   extern const char *Txt_Disclaimer_the_files_hosted_here_;

   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_WORKS_USR:
         Brw_ShowFileBrowsersAsgWrkUsr ();
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         Brw_ShowFileBrowsersAsgWrkCrs ();
         break;
      default:
         Brw_ShowFileBrowserNormal ();
         break;
     }

   /***** Help *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         Lay_ShowAlert (Lay_INFO,Txt_Files_of_marks_must_contain_a_table_in_HTML_format_);
         break;
      default:
         break;
     }

   /***** Legal notice *****/
   sprintf (Gbl.Message,Txt_Disclaimer_the_files_hosted_here_,
            Cfg_PLATFORM_SHORT_NAME,
            Cfg_PLATFORM_RESPONSIBLE_E_MAIL);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);
  }

/*****************************************************************************/
/**************************** Show a file browser ****************************/
/*****************************************************************************/

static void Brw_ShowFileBrowser (void)
  {
   extern const char *Txt_Documents_zone;
   extern const char *Txt_Documents_management_zone;
   extern const char *Txt_Shared_files_area;
   extern const char *Txt_Marks_zone;
   extern const char *Txt_Marks_management_zone;
   extern const char *Txt_Assignments_zone;
   extern const char *Txt_Works_zone;
   extern const char *Txt_Private_storage_zone;
   const char *Brw_TitleOfFileBrowser[Brw_NUM_TYPES_FILE_BROWSER];

   Brw_TitleOfFileBrowser[Brw_FILE_BRW_UNKNOWN            ] = NULL;				// Brw_FILE_BRW_UNKNOWN
   Brw_TitleOfFileBrowser[Brw_FILE_BRW_SEE_DOCUMENTS_CRS  ] = Txt_Documents_zone;		// Brw_FILE_BRW_SEE_DOCUMENTS_CRS
   Brw_TitleOfFileBrowser[Brw_FILE_BRW_SEE_MARKS_CRS      ] = Txt_Marks_zone;			// Brw_FILE_BRW_SEE_MARKS_CRS
   Brw_TitleOfFileBrowser[Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS] = Txt_Documents_management_zone;	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
   Brw_TitleOfFileBrowser[Brw_FILE_BRW_COMMON_CRS         ] = Txt_Shared_files_area;		// Brw_FILE_BRW_COMMON_CRS
   Brw_TitleOfFileBrowser[Brw_FILE_BRW_COMMON_GRP         ] = Txt_Shared_files_area;		// Brw_FILE_BRW_COMMON_GRP
   Brw_TitleOfFileBrowser[Brw_FILE_BRW_WORKS_USR          ] = Txt_Works_zone;			// Brw_FILE_BRW_WORKS_USR
   Brw_TitleOfFileBrowser[Brw_FILE_BRW_WORKS_CRS          ] = Txt_Works_zone;			// Brw_FILE_BRW_WORKS_CRS
   Brw_TitleOfFileBrowser[Brw_FILE_BRW_ADMIN_MARKS_CRS    ] = Txt_Marks_management_zone;	// Brw_FILE_BRW_ADMIN_MARKS_CRS
   Brw_TitleOfFileBrowser[Brw_FILE_BRW_BRIEFCASE_USR      ] = Txt_Private_storage_zone;		// Brw_FILE_BRW_BRIEFCASE_USR
   Brw_TitleOfFileBrowser[Brw_FILE_BRW_SEE_DOCUMENTS_GRP  ] = Txt_Documents_zone;		// Brw_FILE_BRW_SEE_DOCUMENTS_GRP
   Brw_TitleOfFileBrowser[Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP] = Txt_Documents_management_zone;	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
   Brw_TitleOfFileBrowser[Brw_FILE_BRW_SEE_MARKS_GRP      ] = Txt_Marks_zone;			// Brw_FILE_BRW_SEE_MARKS_GRP
   Brw_TitleOfFileBrowser[Brw_FILE_BRW_ADMIN_MARKS_GRP    ] = Txt_Marks_management_zone;	// Brw_FILE_BRW_ADMIN_MARKS_GRP
   Brw_TitleOfFileBrowser[Brw_FILE_BRW_ASSIGNMENTS_USR    ] = Txt_Assignments_zone;		// Brw_FILE_BRW_ASSIGNMENTS_USR
   Brw_TitleOfFileBrowser[Brw_FILE_BRW_ASSIGNMENTS_CRS    ] = Txt_Assignments_zone;		// Brw_FILE_BRW_ASSIGNMENTS_CRS

   /***** Check if the maximum quota has been exceeded *****/
   if (Brw_FileBrowserIsEditable[Gbl.FileBrowser.Type])
      Brw_SetAndCheckQuota ();

   /***** Check if the clipboard is in this tree *****/
   Gbl.FileBrowser.Clipboard.IsThisTree = Brw_CheckIfClipboardIsInThisTree ();

   /***** Start of table *****/
   Lay_StartRoundFrameTable10 (NULL,0,Brw_TitleOfFileBrowser[Gbl.FileBrowser.Type]);

   /***** Title *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td colspan=\"%u\" style=\"text-align:center;\">",
            Brw_NumColumnsInExpTree[Gbl.FileBrowser.Type]);
   Brw_WriteSubtitleOfFileBrowser ();
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");

   /***** Show and store number of documents found *****/
   if (Brw_FileBrowserIsEditable[Gbl.FileBrowser.Type])
     {
      Brw_ShowSizeOfFileTree ();
      Brw_StoreSizeOfFileTreeInDB ();
     }

   /***** List recursively the directory *****/
   Brw_SetFullPathInTree (Brw_RootFolderInternalNames[Gbl.FileBrowser.Type],".");
   if (Brw_WriteRowFileBrowser (0,Brw_IS_FOLDER,Brw_EXPAND_TREE_NOTHING,Brw_RootFolderInternalNames[Gbl.FileBrowser.Type],"."))
      Brw_ListDir (1,Gbl.FileBrowser.Priv.PathRootFolder,Brw_RootFolderInternalNames[Gbl.FileBrowser.Type]);

   /***** End of table *****/
   if (Gbl.FileBrowser.Type == Brw_FILE_BRW_ASSIGNMENTS_CRS ||
       Gbl.FileBrowser.Type == Brw_FILE_BRW_WORKS_CRS)
      Lay_EndRoundFrameTable10 ();
   else
      Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/********************** Write title of a file browser ************************/
/*****************************************************************************/

static void Brw_WriteTopBeforeShowingFileBrowser (void)
  {
   /***** Update last access to this file browser *****/
   Brw_UpdateLastAccess ();

   /***** Write form to edit documents *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER ||
       Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
      switch (Gbl.FileBrowser.Type)
	{
	 case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
            Brw_PutFormToShowOrAdmin (Brw_ADMIN,ActAdmDocCrs);
	    break;
	 case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
            Brw_PutFormToShowOrAdmin (Brw_ADMIN,ActAdmDocGrp);
	    break;
	 case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
            Brw_PutFormToShowOrAdmin (Brw_SHOW,ActSeeDocCrs);
	    break;
	 case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
            Brw_PutFormToShowOrAdmin (Brw_SHOW,ActSeeDocGrp);
	    break;
	 case Brw_FILE_BRW_SEE_MARKS_CRS:
            Brw_PutFormToShowOrAdmin (Brw_ADMIN,ActAdmMrkCrs);
            break;
         case Brw_FILE_BRW_SEE_MARKS_GRP:
            Brw_PutFormToShowOrAdmin (Brw_ADMIN,ActAdmMrkGrp);
            break;
	 case Brw_FILE_BRW_ADMIN_MARKS_CRS:
            Brw_PutFormToShowOrAdmin (Brw_SHOW,ActSeeMrkCrs);
            break;
         case Brw_FILE_BRW_ADMIN_MARKS_GRP:
            Brw_PutFormToShowOrAdmin (Brw_SHOW,ActSeeMrkGrp);
            break;
	 default:
	    break;
	}

   /***** Initialize hidden levels *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
      case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
      case Brw_FILE_BRW_SEE_MARKS_CRS:
      case Brw_FILE_BRW_SEE_MARKS_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         Brw_InitHiddenLevels ();
	 break;
      default:
	 break;
     }

   /***** Write form to show the full tree *****/
   Brw_WriteFormFullTree ();

   /***** If browser is editable, get and write current clipboard *****/
   if (Brw_FileBrowserIsEditable[Gbl.FileBrowser.Type])
      Brw_GetAndWriteClipboard ();
  }

/*****************************************************************************/
/******************* Update last access to a file browser ********************/
/*****************************************************************************/

static void Brw_UpdateLastAccess (void)
  {
   /***** Get and update date and hour of last access to works in the course *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
         Brw_GetAndUpdateDateLastAccFileBrowser ("LastAccDownloadCrs");
         if (Gbl.CurrentAct == ActChgToSeeDoc ||
             Gbl.CurrentAct == ActChgToAdmDoc)// Update group of last access to a documents zone only when user changes zone
            Brw_UpdateGrpLastAccZone ("LastDowGrpCod",-1L);
	 break;
      case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         Brw_GetAndUpdateDateLastAccFileBrowser ("LastAccDownloadGrp");
         if (Gbl.CurrentAct == ActChgToSeeDoc ||
             Gbl.CurrentAct == ActChgToAdmDoc)// Update group of last access to a documents zone only when user changes zone
            Brw_UpdateGrpLastAccZone ("LastDowGrpCod",Gbl.CurrentCrs.Grps.GrpCod);
         break;
      case Brw_FILE_BRW_COMMON_CRS:
         Brw_GetAndUpdateDateLastAccFileBrowser ("LastAccCommonCrs");
         if (Gbl.CurrentAct == ActChgToAdmCom) 	// Update group of last access to a shared files zone only when user changes zone
            Brw_UpdateGrpLastAccZone ("LastComGrpCod",-1L);
	 break;
      case Brw_FILE_BRW_COMMON_GRP:
         Brw_GetAndUpdateDateLastAccFileBrowser ("LastAccCommonGrp");
         if (Gbl.CurrentAct == ActChgToAdmCom) 	// Update group of last access to a shared files zone only when user changes zone
            Brw_UpdateGrpLastAccZone ("LastComGrpCod",Gbl.CurrentCrs.Grps.GrpCod);
	 break;
      case Brw_FILE_BRW_SEE_MARKS_CRS:
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
         Brw_GetAndUpdateDateLastAccFileBrowser ("LastAccMarksCrs");
         if (Gbl.CurrentAct == ActChgToSeeMrk ||
             Gbl.CurrentAct == ActChgToAdmMrk)	// Update group of last access to a marks zone only when user changes zone
            Brw_UpdateGrpLastAccZone ("LastAssGrpCod",-1L);
	 break;
      case Brw_FILE_BRW_SEE_MARKS_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         Brw_GetAndUpdateDateLastAccFileBrowser ("LastAccMarksGrp");
         if (Gbl.CurrentAct == ActChgToSeeMrk ||
             Gbl.CurrentAct == ActChgToAdmMrk)	// Update group of last access to a marks zone only when user changes zone
            Brw_UpdateGrpLastAccZone ("LastAssGrpCod",Gbl.CurrentCrs.Grps.GrpCod);
	 break;
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_WORKS_USR:
         Brw_GetAndUpdateDateLastAccFileBrowser ("LastAccMyWorks");
	 break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         Brw_GetAndUpdateDateLastAccFileBrowser ("LastAccCrsWorks");
	 break;
      case Brw_FILE_BRW_BRIEFCASE_USR:
         Brw_GetAndUpdateDateLastAccFileBrowser ("LastAccBriefcase");
	 break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/*********** Update the group of my last access to a common zone *************/
/*****************************************************************************/

static void Brw_UpdateGrpLastAccZone (const char *FieldNameDB,long GrpCod)
  {
   char Query[512];

   /***** Update the group of my last access to a common zone *****/
   sprintf (Query,"UPDATE crs_usr SET %s='%ld'"
                  " WHERE CrsCod='%ld' AND UsrCod='%ld'",
            FieldNameDB,GrpCod,
            Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update the group of the last access to a file browser");
  }

/*****************************************************************************/
/*********************** Write title of a file browser ***********************/
/*****************************************************************************/

static void Brw_WriteSubtitleOfFileBrowser (void)
  {
   extern const char *Txt_accessible_only_for_reading_by_students_and_teachers_of_the_course;
   extern const char *Txt_accessible_only_for_reading_by_students_of_the_group_and_teachers_of_the_course;
   extern const char *Txt_accessible_for_reading_and_writing_by_teachers_of_the_course;
   extern const char *Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_course;
   extern const char *Txt_accessible_for_reading_and_writing_by_students_of_the_group_and_teachers_of_the_course;
   extern const char *Txt_accessible_for_reading_and_writing_by_you_and_the_teachers_of_the_course;
   extern const char *Txt_accessible_only_for_reading_by_you_and_the_teachers_of_the_course;
   extern const char *Txt_the_marks_of_a_student_chosen_at_random_;
   extern const char *Txt_nobody_else_can_access_this_content;
   char Subtitle[1024];

   fprintf (Gbl.F.Out,"<div style=\"margin:0 auto; min-width:600px;\">");
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
      case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_CRS:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_SEE_MARKS_CRS:
      case Brw_FILE_BRW_SEE_MARKS_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         Brw_FormToChangeZone ();
	 break;
      default:
         break;
     }

   /***** Write subtitle *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
         sprintf (Subtitle,"(%s)",
                  Txt_accessible_only_for_reading_by_students_and_teachers_of_the_course);
	 break;
      case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
         sprintf (Subtitle,"(%s)",
                  Txt_accessible_only_for_reading_by_students_of_the_group_and_teachers_of_the_course);
	 break;
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
         sprintf (Subtitle,"(%s)",
                  Txt_accessible_for_reading_and_writing_by_teachers_of_the_course);
	 break;
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         sprintf (Subtitle,"(%s)",
                  Txt_accessible_for_reading_and_writing_by_teachers_of_the_course);
	 break;
      case Brw_FILE_BRW_COMMON_CRS:
         sprintf (Subtitle,"(%s)",
                  Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_course);
	 break;
      case Brw_FILE_BRW_COMMON_GRP:
         sprintf (Subtitle,"(%s)",
                  Txt_accessible_for_reading_and_writing_by_students_of_the_group_and_teachers_of_the_course);
	 break;
      case Brw_FILE_BRW_SEE_MARKS_CRS:
      case Brw_FILE_BRW_SEE_MARKS_GRP:
         if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_STUDENT)
            sprintf (Subtitle,"(%s)",
                     Txt_accessible_only_for_reading_by_you_and_the_teachers_of_the_course);
	 else
            sprintf (Subtitle,"(%s)",
                     Txt_the_marks_of_a_student_chosen_at_random_);
 	 break;
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         sprintf (Subtitle,"(%s)",
                  Txt_accessible_for_reading_and_writing_by_teachers_of_the_course);
	 break;
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_WORKS_USR:
         sprintf (Subtitle,"%s<br />(%s)",
                  Gbl.Usrs.Me.UsrDat.FullName,
                  Txt_accessible_for_reading_and_writing_by_you_and_the_teachers_of_the_course);
	 break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         sprintf (Subtitle,"%s",
                  Gbl.Usrs.Other.UsrDat.FullName);
	 break;
      case Brw_FILE_BRW_BRIEFCASE_USR:
         sprintf (Subtitle,"%s<br />(%s)",
                  Gbl.Usrs.Me.UsrDat.FullName,
                  Txt_nobody_else_can_access_this_content);
	 break;
      case Brw_FILE_BRW_UNKNOWN:
         return;
     }
   if (Subtitle[0])
      fprintf (Gbl.F.Out,"<span class=\"BROWSER_SUBTITLE\">%s</span>",Subtitle);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************ Initialize hidden levels of download tree to false *************/
/*****************************************************************************/

static void Brw_InitHiddenLevels (void)
  {
   unsigned Level;

   for (Level = 0;
	Level <= Brw_MAX_DIR_LEVELS;
	Level++)
      Gbl.FileBrowser.HiddenLevels[Level] = false;
  }

/*****************************************************************************/
/************************* Show size of a file browser ***********************/
/*****************************************************************************/

static void Brw_ShowSizeOfFileTree (void)
  {
   extern const char *Txt_level;
   extern const char *Txt_levels;
   extern const char *Txt_folder;
   extern const char *Txt_folders;
   extern const char *Txt_file;
   extern const char *Txt_files;
   extern const char *Txt_of_PART_OF_A_TOTAL;

   fprintf (Gbl.F.Out,"<tr>"
                      "<td colspan=\"%u\" class=\"DAT\""
                      " style=\"text-align:center;\">"
                      "%u %s; %lu %s; %lu %s; ",
            Brw_NumColumnsInExpTree[Gbl.FileBrowser.Type],
            Gbl.FileBrowser.Size.NumLevls,
            Gbl.FileBrowser.Size.NumLevls == 1 ? Txt_level :
        	                                 Txt_levels ,
            Gbl.FileBrowser.Size.NumFolds,
            Gbl.FileBrowser.Size.NumFolds == 1 ? Txt_folder :
        	                                 Txt_folders,
            Gbl.FileBrowser.Size.NumFiles,
            Gbl.FileBrowser.Size.NumFiles == 1 ? Txt_file :
        	                                 Txt_files);
   Str_WriteSizeInBytesFull ((double) Gbl.FileBrowser.Size.TotalSiz);
   if (Gbl.FileBrowser.Size.MaxQuota)
     {
      fprintf (Gbl.F.Out," (%.1f%% %s ",
	       100.0 * ((double) Gbl.FileBrowser.Size.TotalSiz / (double) Gbl.FileBrowser.Size.MaxQuota),
	       Txt_of_PART_OF_A_TOTAL);
      Str_WriteSizeInBytesBrief ((double) Gbl.FileBrowser.Size.MaxQuota);
      fprintf (Gbl.F.Out,")");
     }
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
  }

/*****************************************************************************/
/****************** Store size of a file browser in database *****************/
/*****************************************************************************/

static void Brw_StoreSizeOfFileTreeInDB (void)
  {
   char Query[512];
   long CrsCod = -1L;
   long GrpCod = -1L;
   long UsrCod = -1L;
   Brw_FileBrowser_t FileBrowser = Gbl.FileBrowser.Type;

   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
      case Brw_FILE_BRW_COMMON_CRS:
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
         CrsCod = Gbl.CurrentCrs.Crs.CrsCod;
         break;
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         CrsCod = Gbl.CurrentCrs.Crs.CrsCod;
         GrpCod = Gbl.CurrentCrs.Grps.GrpCod;
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_WORKS_USR:
         CrsCod = Gbl.CurrentCrs.Crs.CrsCod;
         UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
         CrsCod = Gbl.CurrentCrs.Crs.CrsCod;
         UsrCod = Gbl.Usrs.Other.UsrDat.UsrCod;
         FileBrowser = Brw_FILE_BRW_ASSIGNMENTS_USR;
         break;
      case Brw_FILE_BRW_WORKS_CRS:
         CrsCod = Gbl.CurrentCrs.Crs.CrsCod;
         UsrCod = Gbl.Usrs.Other.UsrDat.UsrCod;
         FileBrowser = Brw_FILE_BRW_WORKS_USR;
         break;
      case Brw_FILE_BRW_BRIEFCASE_USR:
         UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
         break;
      default: // Not aplicable here
         break;
     }

   /***** Update size of the file browser in database *****/
   sprintf (Query,"REPLACE INTO file_browser_size"
                  " (FileBrowser,CrsCod,GrpCod,UsrCod,NumLevels,NumFolders,NumFiles,TotalSize)"
                  " VALUES ('%u','%ld','%ld','%ld','%u','%lu','%lu','%llu')",
            (unsigned) FileBrowser,CrsCod,GrpCod,UsrCod,
            Gbl.FileBrowser.Size.NumLevls,Gbl.FileBrowser.Size.NumFolds,Gbl.FileBrowser.Size.NumFiles,Gbl.FileBrowser.Size.TotalSiz);
   DB_QueryREPLACE (Query,"can not store the size of a file browser");
  }

/*****************************************************************************/
/**************** Remove size of a file browser from database ****************/
/*****************************************************************************/

void Brw_RemoveSizeOfFileTreeFromDB (long CrsCod,long GrpCod,long UsrCod)
  {
   char Query[512];

   /***** Remove size of the file browser from database *****/
   if (CrsCod > 0 && GrpCod == -1L && UsrCod == -1L)	// All the zones of the course
      sprintf (Query,"DELETE FROM file_browser_size WHERE CrsCod='%ld'",
               CrsCod);
   else if (CrsCod > 0 && UsrCod > 0)			// Works of a student in the course
      sprintf (Query,"DELETE FROM file_browser_size WHERE CrsCod='%ld' AND UsrCod='%ld'",
               CrsCod,UsrCod);
   else if (GrpCod > 0)					// All the zones of the group
      sprintf (Query,"DELETE FROM file_browser_size WHERE GrpCod='%ld'",
               GrpCod);
   else if (UsrCod > 0)					// All the zones of the user
      sprintf (Query,"DELETE FROM file_browser_size WHERE UsrCod='%ld'",
               UsrCod);
   DB_QueryDELETE (Query,"can not remove the size of a file browser");
  }

/*****************************************************************************/
/**************** Remove a file or folder from the database ******************/
/*****************************************************************************/

void Brw_RemoveFilesFromDB (long CrsCod,long GrpCod,long UsrCod)
  {
   char Query1[512];
   char Query2[512];

   /***** Remove size of the file browser from database *****/
   if (CrsCod > 0 && GrpCod <= 0 && UsrCod <= 0)	// All the zones of the course
     {
      sprintf (Query1,"DELETE FROM file_view USING file_view,files"
                     " WHERE files.CrsCod='%ld'"
                     " AND files.FilCod=file_view.FilCod",
               CrsCod);
      sprintf (Query2,"DELETE FROM files WHERE CrsCod='%ld'",
               CrsCod);
     }
   else if (CrsCod > 0 && UsrCod > 0)			// Works of a student in the course
     {
      sprintf (Query1,"DELETE FROM file_view USING file_view,files"
                     " WHERE files.CrsCod='%ld' AND files.ZoneUsrCod='%ld'"
                     " AND files.FilCod=file_view.FilCod",
               CrsCod,UsrCod);
      sprintf (Query2,"DELETE FROM files WHERE CrsCod='%ld' AND ZoneUsrCod='%ld'",
               CrsCod,UsrCod);
     }
   else if (GrpCod > 0)					// All the zones of the group
     {
      sprintf (Query1,"DELETE FROM file_view USING file_view,files"
                     " WHERE files.GrpCod='%ld'"
                     " AND files.FilCod=file_view.FilCod",
               GrpCod);
      sprintf (Query2,"DELETE FROM files WHERE GrpCod='%ld'",
               GrpCod);
     }
   else if (UsrCod > 0)					// All the zones of the user
     {
      sprintf (Query1,"DELETE FROM file_view USING file_view,files"
                     " WHERE files.ZoneUsrCod='%ld'"
                     " AND files.FilCod=file_view.FilCod",
               UsrCod);
      sprintf (Query2,"DELETE FROM files WHERE ZoneUsrCod='%ld'",
               UsrCod);
     }

   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE (Query1,"can not remove files");

   /***** Remove from database the entry that stores the data of a file *****/
   DB_QueryDELETE (Query2,"can not remove files");
  }

/*****************************************************************************/
/******************* Write a form to go to show documents ********************/
/*****************************************************************************/

static void Brw_PutFormToShowOrAdmin (Brw_ShowOrAdmin_t ShowOrAdmin,
                                      Act_Action_t Action)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_View_as_a_student;
   extern const char *Txt_Edit;

   fprintf (Gbl.F.Out,"<div style=\"text-align:center; margin-bottom:10px;\">");
   Act_FormStart (Action);
   if (Gbl.FileBrowser.FullTree)
      Par_PutHiddenParamChar ("FullTree",'Y');
   // It's not necessary to put a parameter with the group code...
   // ...because it is stored in database
   switch (ShowOrAdmin)
     {
      case Brw_SHOW:
	 Act_LinkFormSubmit (Txt_View_as_a_student,The_ClassFormul[Gbl.Prefs.Theme]);
	 Lay_PutSendIcon ("visible_on",
	                  Txt_View_as_a_student,
	                  Txt_View_as_a_student);
	 break;
      case Brw_ADMIN:
	 Act_LinkFormSubmit (Txt_Edit,The_ClassFormul[Gbl.Prefs.Theme]);
	 Lay_PutSendIcon ("edit",
	                  Txt_Edit,
	                  Txt_Edit);
	 break;
     }
   fprintf (Gbl.F.Out,"</form>"
	              "</div>");
  }

/*****************************************************************************/
/************** Write a form to select whether show full tree ****************/
/*****************************************************************************/

static void Brw_WriteFormFullTree (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Show_all_files;

   /***** Start form depending on type of tree *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\""
	              " style=\"text-align:center; vertical-align:middle;\">",
	    The_ClassFormul[Gbl.Prefs.Theme]);
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
	 Act_FormStart (ActSeeDocCrs);
         Grp_PutParamGrpCod (-1L);
	 break;
      case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
	 Act_FormStart (ActSeeDocGrp);
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
	 break;
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
	 Act_FormStart (ActAdmDocCrs);
         Grp_PutParamGrpCod (-1L);
	 break;
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
	 Act_FormStart (ActAdmDocGrp);
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
	 break;
      case Brw_FILE_BRW_COMMON_CRS:
	 Act_FormStart (ActAdmComCrs);
         Grp_PutParamGrpCod (-1L);
	 break;
      case Brw_FILE_BRW_COMMON_GRP:
	 Act_FormStart (ActAdmComGrp);
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
	 break;
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
	 Act_FormStart (ActAdmAsgWrkUsr);
	 break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
	 Act_FormStart (ActAdmAsgWrkCrs);
	 Usr_PutHiddenParUsrCodAll (ActAdmAsgWrkCrs,Gbl.Usrs.Select.All);
	 break;
      case Brw_FILE_BRW_WORKS_USR:
	 Act_FormStart (ActAdmAsgWrkUsr);
	 break;
      case Brw_FILE_BRW_WORKS_CRS:
	 Act_FormStart (ActAdmAsgWrkCrs);
	 Usr_PutHiddenParUsrCodAll (ActAdmAsgWrkCrs,Gbl.Usrs.Select.All);
	 break;
      case Brw_FILE_BRW_SEE_MARKS_CRS:
	 Act_FormStart (ActSeeMrkCrs);
         Grp_PutParamGrpCod (-1L);
	 break;
      case Brw_FILE_BRW_SEE_MARKS_GRP:
	 Act_FormStart (ActSeeMrkGrp);
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
	 break;
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
	 Act_FormStart (ActAdmMrkCrs);
         Grp_PutParamGrpCod (-1L);
	 break;
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
	 Act_FormStart (ActAdmMrkGrp);
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
	 break;
      case Brw_FILE_BRW_BRIEFCASE_USR:
	 Act_FormStart (ActAdmBrf);
	 break;
      default:
	 break;
     }

   /***** End form *****/
   fprintf (Gbl.F.Out,"<input type=\"checkbox\" name=\"FullTree\" value=\"Y\"");
   if (Gbl.FileBrowser.FullTree)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   fprintf (Gbl.F.Out," onclick=\"javascript:document.getElementById('%s').submit();\" />"
                      " %s"
	              "</form>"
	              "</div>",
            Gbl.FormId,
            Txt_Show_all_files);
  }

/*****************************************************************************/
/******************* Get whether to show full tree from form *****************/
/*****************************************************************************/

static bool Brw_GetFullTreeFromForm (void)
  {
   char YN[1+1];

   Par_GetParToText ("FullTree",YN,1);
   return (Str_ConvertToUpperLetter (YN[0]) == 'Y');
  }

/*****************************************************************************/
/******** Create a temporary public directory used to download files *********/
/*****************************************************************************/

void Brw_CreateDirDownloadTmp (void)
  {
   static unsigned NumDir = 0;	// When this function is called several times in the same execution of the program, each time a new directory is created
				// This happens when the trees of assignments and works of several users are being listed
   char PathFileBrowserTmpPubl[PATH_MAX+1];
   char PathPubDirTmp[PATH_MAX+1];

   /* Example: /var/www/html/swad/tmp/SSujCNWsy4ZOdmgMKYBe0sKPAJu6szaZOQlIlJs_QIY */

   /***** If the public directory does not exist, create it *****/
   sprintf (PathFileBrowserTmpPubl,"%s/%s",
            Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_FILE_BROWSER_TMP);
   Fil_CreateDirIfNotExists (PathFileBrowserTmpPubl);

   /***** First of all, we remove the oldest temporary directories.
	  Such temporary directories have been created by me or by other users.
	  This is a bit sloppy, but they must be removed by someone.
	  Here "oldest" means more than x time from their creation *****/
   if (NumDir == 0)	// Only in the first call to this function
      Fil_RemoveOldTmpFiles (PathFileBrowserTmpPubl,Cfg_TIME_TO_DELETE_BROWSER_TMP_FILES,false);

   /***** Create a new temporary directory.
          Important: number of directories inside a directory is limited to 32K in Linux *****/
   if (NumDir)
      sprintf (Gbl.FileBrowser.TmpPubDir,"%s_%u",Gbl.UniqueNameEncrypted,NumDir);
   else
      strcpy (Gbl.FileBrowser.TmpPubDir,Gbl.UniqueNameEncrypted);
   sprintf (PathPubDirTmp,"%s/%s",PathFileBrowserTmpPubl,Gbl.FileBrowser.TmpPubDir);
   if (mkdir (PathPubDirTmp,(mode_t) 0xFFF))
      Lay_ShowErrorAndExit ("Can not create a temporary folder for download.");
   NumDir++;
  }

/*****************************************************************************/
/* Get and update the date of my last access to file browser in this course **/
/*****************************************************************************/

static void Brw_GetAndUpdateDateLastAccFileBrowser (const char *FieldNameDB)
  {
   char Query1[256];
   char Query2[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get date of last accesss to a file browser from database *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
      case Brw_FILE_BRW_COMMON_CRS:
      case Brw_FILE_BRW_SEE_MARKS_CRS:
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_USR:
      case Brw_FILE_BRW_WORKS_CRS:
         sprintf (Query1,"SELECT UNIX_TIMESTAMP(%s) FROM crs_usr"
                         " WHERE CrsCod='%ld' AND UsrCod='%ld'",
                  FieldNameDB,
                  Gbl.CurrentCrs.Crs.CrsCod,
                  Gbl.Usrs.Me.UsrDat.UsrCod);
         sprintf (Query2,"UPDATE crs_usr SET %s=NOW()"
                         " WHERE CrsCod='%ld' AND UsrCod='%ld'",
                  FieldNameDB,
                  Gbl.CurrentCrs.Crs.CrsCod,
                  Gbl.Usrs.Me.UsrDat.UsrCod);
	 break;
      case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_SEE_MARKS_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         sprintf (Query1,"SELECT UNIX_TIMESTAMP(%s) FROM crs_grp_usr"
                         " WHERE GrpCod='%ld' AND UsrCod='%ld'",
                  FieldNameDB,
                  Gbl.CurrentCrs.Grps.GrpCod,
                  Gbl.Usrs.Me.UsrDat.UsrCod);
         sprintf (Query2,"UPDATE crs_grp_usr SET %s=NOW()"
                         " WHERE GrpCod='%ld' AND UsrCod='%ld'",
                  FieldNameDB,
                  Gbl.CurrentCrs.Grps.GrpCod,
                  Gbl.Usrs.Me.UsrDat.UsrCod);
	 break;
      case Brw_FILE_BRW_BRIEFCASE_USR:
	 // FieldNameDB -> "LastAccBriefcase"
         sprintf (Query1,"SELECT UNIX_TIMESTAMP(%s) FROM usr_last"
                         " WHERE UsrCod='%ld'",
                  FieldNameDB,Gbl.Usrs.Me.UsrDat.UsrCod);
         sprintf (Query2,"UPDATE usr_last SET %s=NOW()"
                         " WHERE UsrCod='%ld'",
                  FieldNameDB,Gbl.Usrs.Me.UsrDat.UsrCod);
	 break;
      default:
	 return;
     }
   NumRows = DB_QuerySELECT (Query1,&mysql_res,"can not get date-time of last access to a file browser");

   if (NumRows == 0)	// May be an administrator not belonging to this course
      Gbl.Usrs.Me.TimeLastAccToThisFileBrowser = LONG_MAX;	// Initialize to a big value in order to show files as old
   else if (NumRows == 1)
     {
      /* Get the date of the last access to file browser (row[0]) */
      row = mysql_fetch_row (mysql_res);
      if (row[0] == NULL)
         Gbl.Usrs.Me.TimeLastAccToThisFileBrowser = 0;
      else if (sscanf (row[0],"%lu",&Gbl.Usrs.Me.TimeLastAccToThisFileBrowser) != 1)
         Lay_ShowErrorAndExit ("Error when reading date-time of last access to a file browser.");
     }
   else
      Lay_ShowErrorAndExit ("Error when getting date-time of last access to a file browser.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Update date of my last access to file browser in this course *****/
   DB_QueryUPDATE (Query2,"can not update date of last access to a file browser");
  }

/*****************************************************************************/
/************* Get the group of my last access to a common zone **************/
/*****************************************************************************/

static long Brw_GetGrpLastAccZone (const char *FieldNameDB)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   long GrpCod = -1L;

   /***** Get the group of my last access to a common zone from database *****/
   sprintf (Query,"SELECT %s FROM crs_usr"
                  " WHERE CrsCod='%ld' AND UsrCod='%ld'",
            FieldNameDB,
            Gbl.CurrentCrs.Crs.CrsCod,
            Gbl.Usrs.Me.UsrDat.UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get the group of your last access to a file browser");

   if (NumRows == 0)	// May be an administrator not belonging to this course
      GrpCod = -1L;
   else if (NumRows == 1)
     {
      /* Get the group code (row[0]) */
      row = mysql_fetch_row (mysql_res);
      if (row[0])
        {
         if (sscanf (row[0],"%ld",&GrpCod) != 1)
            Lay_ShowErrorAndExit ("Error when reading the group of your last access to a file browser.");
        }
      else
         GrpCod = -1L;
     }
   else
      Lay_ShowErrorAndExit ("Error when getting the group of your last access to a file browser.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Check if group exists (it's possible that this group has been removed after my last access to it) *****/
   if (GrpCod >= 0)
      if (Grp_CheckIfGroupExists (GrpCod))
         /* Check if I belong to this group (it's possible that I have been removed from this group after my last access to it) */
         if (Grp_GetIfIBelongToGrp (GrpCod))
            return GrpCod;

   return -1L;	// To indicate course zone
  }

/*****************************************************************************/
/********************* Reset the size of a file browser **********************/
/*****************************************************************************/

static void Brw_ResetFileBrowserSize (void)
  {
   Gbl.FileBrowser.Size.NumLevls = 0;
   Gbl.FileBrowser.Size.NumFolds =
   Gbl.FileBrowser.Size.NumFiles = 0L;
   Gbl.FileBrowser.Size.TotalSiz = 0ULL;
  }

/*****************************************************************************/
/********************** Compute the size of a directory **********************/
/*****************************************************************************/

void Brw_CalcSizeOfDir (char *Path)
  {
   Brw_ResetFileBrowserSize ();
   Brw_CalcSizeOfDirRecursive (1,Path);
  }

/*****************************************************************************/
/**************** Compute the size of a directory recursively ****************/
/*****************************************************************************/

static void Brw_CalcSizeOfDirRecursive (unsigned Level,char *Path)
  {
   struct dirent **FileList;
   int NumFileInThisDir;
   int NumFilesInThisDir;
   char PathFileRel[PATH_MAX+1];
   struct stat FileStatus;

   /***** Scan the directory *****/
   NumFilesInThisDir = scandir (Path,&FileList,NULL,NULL);

   /***** Compute recursively the total number and size of the files and folders *****/
   for (NumFileInThisDir = 0;
	NumFileInThisDir < NumFilesInThisDir;
	NumFileInThisDir++)
      if (strcmp (FileList[NumFileInThisDir]->d_name,".") &&
          strcmp (FileList[NumFileInThisDir]->d_name,".."))	// Skip directories "." and ".."
	{
         /* There are files in this directory ==> update level */
         if (Level > Gbl.FileBrowser.Size.NumLevls)
            Gbl.FileBrowser.Size.NumLevls++;

         /* Update counters depending on whether it's a directory or a regular file */
	 sprintf (PathFileRel,"%s/%s",Path,FileList[NumFileInThisDir]->d_name);
	 lstat (PathFileRel,&FileStatus);
	 if (S_ISDIR (FileStatus.st_mode))		// It's a directory
	   {
	    Gbl.FileBrowser.Size.NumFolds++;
	    Gbl.FileBrowser.Size.TotalSiz += (unsigned long long) FileStatus.st_size;
	    Brw_CalcSizeOfDirRecursive (Level+1,PathFileRel);
	   }
	 else if (S_ISREG (FileStatus.st_mode))		// It's a regular file
	   {
	    Gbl.FileBrowser.Size.NumFiles++;
	    Gbl.FileBrowser.Size.TotalSiz += (unsigned long long) FileStatus.st_size;
	   }
	}
  }

/*****************************************************************************/
/************************ List a directory recursively ***********************/
/*****************************************************************************/

static void Brw_ListDir (unsigned Level,const char *Path,const char *PathInTree)
  {
   struct dirent **DirFileList;
   struct dirent **SubdirFileList;
   int NumFileInThisDir;
   int NumFilesInThisDir;
   int NumFilesInThisSubdir;
   char PathFileRel[PATH_MAX+1];
   char PathFileInExplTree[PATH_MAX+1];
   struct stat FileStatus;
   Brw_ExpandTree_t ExpandTree;

   /***** Scan directory *****/
   NumFilesInThisDir = scandir (Path,&DirFileList,NULL,alphasort);

   /***** List files *****/
   for (NumFileInThisDir = 0;
	NumFileInThisDir < NumFilesInThisDir;
	NumFileInThisDir++)
      if (strcmp (DirFileList[NumFileInThisDir]->d_name,".") &&
          strcmp (DirFileList[NumFileInThisDir]->d_name,".."))	// Skip directories "." and ".."
        {
	 sprintf (PathFileRel       ,"%s/%s",Path      ,DirFileList[NumFileInThisDir]->d_name);
	 sprintf (PathFileInExplTree,"%s/%s",PathInTree,DirFileList[NumFileInThisDir]->d_name);

	 lstat (PathFileRel,&FileStatus);

         /***** Construct the full path of the file or folder *****/
         Brw_SetFullPathInTree (PathInTree,DirFileList[NumFileInThisDir]->d_name);

	 if (S_ISDIR (FileStatus.st_mode))	// It's a directory
	   {
            if (Gbl.FileBrowser.FullTree)
               ExpandTree = Brw_EXPAND_TREE_NOTHING;
            else
              {
               /***** Check if this subdirectory have files or folders in it *****/
               if ((NumFilesInThisSubdir = scandir (PathFileRel,&SubdirFileList,NULL,NULL)) <= 2)
                  ExpandTree = Brw_EXPAND_TREE_NOTHING;
               else
                  /***** Check if the tree starting at this subdirectory must be expanded *****/
                  ExpandTree = Brw_GetIfExpandedTree (Gbl.FileBrowser.Priv.FullPathInTree) ? Brw_EXPAND_TREE_MINUS :
                	                                                                     Brw_EXPAND_TREE_PLUS;
              }

            /***** Write a row for the subdirectory *****/
	    if (Brw_WriteRowFileBrowser (Level,Brw_IS_FOLDER,ExpandTree,PathInTree,DirFileList[NumFileInThisDir]->d_name))
               if (ExpandTree == Brw_EXPAND_TREE_MINUS ||
                   ExpandTree == Brw_EXPAND_TREE_NOTHING)
                  if (Level < Brw_MAX_DIR_LEVELS)
                     /* List subtree starting at this this directory */
                     Brw_ListDir (Level+1,PathFileRel,PathFileInExplTree);
	   }
	 else if (S_ISREG (FileStatus.st_mode))	// It's a regular file
	    Brw_WriteRowFileBrowser (Level,
	                             Str_FileIs (DirFileList[NumFileInThisDir]->d_name,"url") ? Brw_IS_LINK :
		                                                                                Brw_IS_FILE,
		                     Brw_EXPAND_TREE_NOTHING,PathInTree,DirFileList[NumFileInThisDir]->d_name);
	}
  }

/*****************************************************************************/
/*********************** Write a row of a file browser ***********************/
/*****************************************************************************/
// If it is the first row (root folder), always show it
// If it is not the first row, it is shown or not depending on whether it is hidden or not
// If the row is visible, return true. If it is hidden, return false

static bool Brw_WriteRowFileBrowser (unsigned Level,
                                     Brw_FileType_t FileType,Brw_ExpandTree_t ExpandTree,
                                     const char *PathInTree,const char *FileName)
  {
   bool RowSetAsHidden = false;
   bool RowSetAsPublic = false;
   bool LightStyle = false;
   bool IsRecent = false;
   struct FileMetadata FileMetadata;
   char FileNameToShow[NAME_MAX+1];
   bool SeeDocsZone     = Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_DOCUMENTS_CRS ||
                          Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_DOCUMENTS_GRP;
   bool AdminDocsZone   = Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS ||
                          Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP;
   bool CommonZone      = Gbl.FileBrowser.Type == Brw_FILE_BRW_COMMON_CRS ||
                          Gbl.FileBrowser.Type == Brw_FILE_BRW_COMMON_GRP;
   bool AssignmentsZone = Gbl.FileBrowser.Type == Brw_FILE_BRW_ASSIGNMENTS_USR ||
                          Gbl.FileBrowser.Type == Brw_FILE_BRW_ASSIGNMENTS_CRS;
   bool SeeMarks        = Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_MARKS_CRS ||
                          Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_MARKS_GRP;
   bool AdminMarks      = Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_CRS ||
                          Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_GRP;

   Gbl.FileBrowser.Clipboard.IsThisFile = false;

   /***** Is this row hidden or visible? *****/
   if (SeeDocsZone || AdminDocsZone ||
       SeeMarks    || AdminMarks)
     {
      RowSetAsHidden = Brw_CheckIfFileOrFolderIsSetAsHiddenInDB (FileType,
                                                                 Gbl.FileBrowser.Priv.FullPathInTree);
      if (RowSetAsHidden && Level && (SeeDocsZone || SeeMarks))
         return false;
      if (AdminDocsZone || AdminMarks)
        {
         if (RowSetAsHidden)	// this row is marked as hidden
           {
            if (FileType == Brw_IS_FOLDER)
               Gbl.FileBrowser.HiddenLevels[Level] = true;
            LightStyle = true;
           }
         else			// this row is not marked as hidden
           {
            if (FileType == Brw_IS_FOLDER)
               Gbl.FileBrowser.HiddenLevels[Level] = false;
            LightStyle = Brw_CheckIfAnyUpperLevelIsHidden (Level);
           }
        }
     }

   /***** Get file metadata *****/
   Brw_GetFileMetadataByPath (&FileMetadata);
   Brw_GetFileSizeAndDate (&FileMetadata);
   if (FileMetadata.FilCod <= 0)	// No entry for this file in database table of files
      /* Add entry to the table of files/folders */
      FileMetadata.FilCod = Brw_AddPathToDB (-1L,FileMetadata.FileType,
                                             Gbl.FileBrowser.Priv.FullPathInTree,false,Brw_LICENSE_DEFAULT);

   /***** Is this row public or private? *****/
   if (SeeDocsZone || AdminDocsZone || CommonZone)
     {
      RowSetAsPublic = (FileType == Brw_IS_FOLDER) ? Brw_GetIfFolderHasPublicFiles (Gbl.FileBrowser.Priv.FullPathInTree) :
	                                             FileMetadata.IsPublic;
      if (Gbl.FileBrowser.ShowOnlyPublicFiles && !RowSetAsPublic)
         return false;
     }

   /***** Check if is a recent file or folder *****/
   // If less than a week since last modify ==> indicate the file is recent by writting its name in green
   if (Gbl.TimeStartExecution < FileMetadata.Time + (7L*24L*60L*60L))
      IsRecent = true;

   /* Style of the text in this row */
   Gbl.FileBrowser.TxtStyle   = (LightStyle ? (FileType == Brw_IS_FOLDER || !IsRecent ? "LST_HID" :
	                                                                                "LST_REC_HID") :
                                              (FileType == Brw_IS_FOLDER || !IsRecent ? "LST" :
                                        	                                        "LST_REC"));
   Gbl.FileBrowser.InputStyle = (LightStyle ? (FileType == Brw_IS_FOLDER || !IsRecent ? "LST_EDIT_HID" :
	                                                                                "LST_EDIT_REC_HID") :
                                              (FileType == Brw_IS_FOLDER || !IsRecent ? "LST_EDIT" :
                                        	                                        "LST_EDIT_REC"));

   /***** Get data of assignment using the name of the folder *****/
   if (AssignmentsZone && Level == 1)
     {
      strncpy (Gbl.FileBrowser.Asg.Folder,FileName,Asg_MAX_LENGTH_FOLDER);
      Gbl.FileBrowser.Asg.Folder[Asg_MAX_LENGTH_FOLDER] = '\0';
      Asg_GetDataOfAssignmentByFolder (&Gbl.FileBrowser.Asg);
      // The data of this assignment remains in Gbl.FileBrowser.Asg
      // for all subsequent rows with Level > 1 (files or folders inside this folder),
      // and they are overwritten on the next row with level == 1 (next assignment)
     }

   /***** Get the name of the file to show *****/
   Brw_GetFileNameToShow (Gbl.FileBrowser.Type,Level,FileType,
                          FileName,FileNameToShow);

   /***** Start this row *****/
   fprintf (Gbl.F.Out,"<tr>");

   /****** If current action allows file administration... ******/
   Gbl.FileBrowser.ICanRemoveFileOrFolder =
   Gbl.FileBrowser.ICanEditFileOrFolder   = false;
   if (Brw_FileBrowserIsEditable[Gbl.FileBrowser.Type] &&
       !Gbl.FileBrowser.ShowOnlyPublicFiles)
     {
      if (Gbl.FileBrowser.Clipboard.IsThisTree)
	 // If path in the clipboard is equal to complete path in tree...
	 // ...or is the start of complete path in tree...
         if (Str_Path1BeginsByPath2 (Gbl.FileBrowser.Priv.FullPathInTree,Gbl.FileBrowser.Clipboard.Path))
            Gbl.FileBrowser.Clipboard.IsThisFile = true;

      /* Check if I can modify (remove, rename, etc.) this file or folder */
      Gbl.FileBrowser.ICanEditFileOrFolder   = Brw_CheckIfICanEditFileOrFolder (Level);
      Gbl.FileBrowser.ICanRemoveFileOrFolder = (Gbl.FileBrowser.Type == Brw_FILE_BRW_BRIEFCASE_USR &&
	                                        Level != 0) ? true :
	                                                      Gbl.FileBrowser.ICanEditFileOrFolder;

      /* Put icons to remove, copy and paste */
      Brw_PutIconsRemoveCopyPaste (Level,FileType,PathInTree,FileName,FileNameToShow);
     }

   /***** Indentation depending on level, icon, and file/folder name *****/
   /* Start of the column */
   fprintf (Gbl.F.Out,"<td class=\"NO_BR\" style=\"width:99%%;"
	              " text-align:left; vertical-align:top;"
	              " background-color:%s;\">"
                      "<table>"
                      "<tr>",
            Gbl.ColorRows[Gbl.RowEvenOdd]);

   /* Indent depending on level */
   if (Level)
      Brw_IndentAndWriteIconExpandContract (Level,ExpandTree,
                                            PathInTree,FileName,FileNameToShow);

   /* Put icon to show/hide file or folder */
   if (AdminDocsZone || AdminMarks)
     {
      if (RowSetAsHidden)	// this row is marked as hidden
         Brw_PutIconShow (Level,FileType,
                          PathInTree,FileName,FileNameToShow);
      else			// this row is not marked as hidden
         Brw_PutIconHide (Level,FileType,
                          PathInTree,FileName,FileNameToShow);
     }

   /***** File or folder icon *****/
   if (FileType == Brw_IS_FOLDER)
      /* Icon with folder */
      Brw_PutIconFolder (Level,ExpandTree,
                         PathInTree,FileName,FileNameToShow);
   else
     {
      /* Icon with file type or link */
      fprintf (Gbl.F.Out,"<td class=\"BM%d\">",Gbl.RowEvenOdd);
      Brw_PutIconFileWithLinkToViewMetadata (16,FileType,PathInTree,FileName,FileNameToShow);
      fprintf (Gbl.F.Out,"</td>");
     }

   /* Check if is a new file or folder */
   // If our last access was before the last modify ==> indicate the file is new by putting a blinking star
   if (Gbl.Usrs.Me.TimeLastAccToThisFileBrowser < FileMetadata.Time)
      Brw_PutIconNewFileOrFolder ();

   /* File or folder name */
   Brw_WriteFileName (Level,FileMetadata.IsPublic,FileType,
                      PathInTree,FileName,FileNameToShow);

   /* End of the column */
   fprintf (Gbl.F.Out,"</tr>"
	              "</table>"
	              "</td>");

   /***** Put icon to download ZIP of folder *****/
   if (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_STUDENT &&	// Only ZIP folders if I am student, teacher...
       !SeeMarks)					// Do not ZIP folders when seeing marks
     {
      fprintf (Gbl.F.Out,"<td class=\"BM%d\">",Gbl.RowEvenOdd);
      if (FileType == Brw_IS_FOLDER &&	// If it is a folder
	  !(SeeDocsZone && RowSetAsHidden))	// When seeing docs, if folder is not hidden (this could happen for Level == 0)
	 ZIP_PutButtonToDownloadZIPOfAFolder (PathInTree,FileName);
      fprintf (Gbl.F.Out,"</td>");
     }

   if (AssignmentsZone && Level == 1)
      /***** Start and end dates of assignment *****/
      Brw_WriteDatesAssignment ();
   else
     {
      /***** User who created the file or folder *****/
      Brw_WriteFileOrFolderPublisher (Level,FileMetadata.PublisherUsrCod);

      if (AdminMarks)
         /***** Header and footer rows *****/
         Mrk_GetAndWriteNumRowsHeaderAndFooter (FileType,PathInTree,FileName);

      /***** File date and size *****/
      Brw_WriteFileSizeAndDate (FileType,&FileMetadata);
     }

   /***** End this row *****/
   fprintf (Gbl.F.Out,"</tr>");

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;

   if (RowSetAsHidden && (SeeDocsZone || SeeMarks))
      return false;
   return true;
  }

/*****************************************************************************/
/*************** Construct full path in tree of file browser *****************/
/*****************************************************************************/
// If, for example, PathInTreeUntilFileOrFolder is "descarga/teoria" and FilFolLnkName is "tema1.pdf"
// then Gbl.FileBrowser.Priv.FullPathInTree will be "descarga/teoria/tema1.pdf"
// If FilFolLnkName is ".", then Gbl.FileBrowser.Priv.FullPathInTree will be equal to PathInTreeUntilFileOrFolder

void Brw_SetFullPathInTree (const char *PathInTreeUntilFileOrFolder,const char *FilFolLnkName)
  {
   if (!PathInTreeUntilFileOrFolder[0])
      Gbl.FileBrowser.Priv.FullPathInTree[0] = '\0';
   else if (strcmp (FilFolLnkName,"."))
      sprintf (Gbl.FileBrowser.Priv.FullPathInTree,"%s/%s",
	       PathInTreeUntilFileOrFolder,FilFolLnkName);
   else	// It's the root folder
      strcpy (Gbl.FileBrowser.Priv.FullPathInTree,
	      PathInTreeUntilFileOrFolder);
  }

/*****************************************************************************/
/****************** Put icons to remove, copy and paste **********************/
/*****************************************************************************/

static void Brw_PutIconsRemoveCopyPaste (unsigned Level,Brw_FileType_t FileType,
                                         const char *PathInTree,const char *FileName,const char *FileNameToShow)
  {
   /***** Icon to remove folder, file or link *****/
   if (FileType == Brw_IS_FOLDER)
      /* Icon to remove a folder */
      Brw_PutIconRemoveDir (PathInTree,FileName,FileNameToShow);
   else	// File or link
      /* Icon to remove a file or link */
      Brw_PutIconRemoveFile (FileType,PathInTree,FileName,FileNameToShow);

   /***** Icon to copy *****/
   Brw_PutIconCopy (FileType,PathInTree,FileName,FileNameToShow);

   /***** Icon to paste *****/
   if (FileType == Brw_IS_FOLDER)
     {
      if (Brw_CheckIfCanPasteIn (Level))
         /* Icon to paste active */
         Brw_PutIconPasteOn (PathInTree,FileName,FileNameToShow);
      else
         /* Icon to paste inactive */
         Brw_PutIconPasteOff ();
     }
   else	// File or link. Can't paste in a file or link.
      fprintf (Gbl.F.Out,"<td class=\"BM%d\"></td>",Gbl.RowEvenOdd);
  }

/*****************************************************************************/
/******* Check if the clipboard can be pasted into the current folder ********/
/*****************************************************************************/
// Return true if Gbl.FileBrowser.Clipboard.Path can be pasted into Gbl.FileBrowser.Priv.FullPathInTree

static bool Brw_CheckIfCanPasteIn (unsigned Level)
  {
   char PathDstWithFile[PATH_MAX+1];

   /* If there is nothing in clipboard... */
   if (Gbl.FileBrowser.Clipboard.FileBrowser == Brw_FILE_BRW_UNKNOWN)
      return false;

   /* Do not paste a link in marks... */
   if (Gbl.FileBrowser.Clipboard.FileType == Brw_IS_LINK &&
       (Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_CRS ||
        Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_GRP))
      return false;

   if (!Brw_CheckIfICanCreateIntoFolder (Level))
      return false;	// Pasting into top level of assignments is forbidden

   if (Gbl.FileBrowser.Clipboard.IsThisTree)	// We are in the same tree of the clipboard ==> we can paste or not depending on the subtree
     {
      /***** Construct the name of the file or folder destination *****/
      sprintf (PathDstWithFile,"%s/%s",Gbl.FileBrowser.Priv.FullPathInTree,Gbl.FileBrowser.Clipboard.FileName);

      return !Str_Path1BeginsByPath2 (PathDstWithFile,Gbl.FileBrowser.Clipboard.Path);
     }

   return true;	// I can paste
  }

/*****************************************************************************/
/******************* Write link e icon to remove a file **********************/
/*****************************************************************************/
// FileType can be Brw_IS_FILE or Brw_IS_LINK

static void Brw_PutIconRemoveFile (Brw_FileType_t FileType,
                                   const char *PathInTree,const char *FileName,const char *FileNameToShow)
  {
   extern const char *Txt_Remove_FILE_OR_LINK_X;

   fprintf (Gbl.F.Out,"<td class=\"BM%d\">",Gbl.RowEvenOdd);

   if (Gbl.FileBrowser.ICanRemoveFileOrFolder)	// Can I remove this file?
     {
      /***** Form to remove a file *****/
      Act_FormStart (Brw_ActAskRemoveFile[Gbl.FileBrowser.Type]);
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         case Brw_FILE_BRW_COMMON_GRP:
         case Brw_FILE_BRW_ADMIN_MARKS_GRP:
            Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
            break;
         case Brw_FILE_BRW_ASSIGNMENTS_CRS:
         case Brw_FILE_BRW_WORKS_CRS:
            Usr_PutHiddenParUsrCodAll (Brw_ActAskRemoveFile[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
            Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
            break;
         default:
            break;
        }
      Brw_ParamListFiles (FileType,PathInTree,FileName);
      sprintf (Gbl.Title,Txt_Remove_FILE_OR_LINK_X,FileNameToShow);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/delon16x16.gif\""
	                 " alt=\"%s\" title=\"%s\" class=\"ICON16x16B\" />"
	                 "</form>",
               Gbl.Prefs.IconsURL,
               Gbl.Title,
               Gbl.Title);
     }
   else
      fprintf (Gbl.F.Out,"<img src=\"%s/deloff16x16.gif\" alt=\"\""
	                 " class=\"ICON16x16B\" />",
               Gbl.Prefs.IconsURL);
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/****************** Write link and icon to remove a folder *******************/
/*****************************************************************************/

static void Brw_PutIconRemoveDir (const char *PathInTree,const char *FileName,const char *FileNameToShow)
  {
   extern const char *Txt_Remove_folder_X;

   fprintf (Gbl.F.Out,"<td class=\"BM%d\">",Gbl.RowEvenOdd);

   if (Gbl.FileBrowser.ICanRemoveFileOrFolder)	// Can I remove this folder?
     {
      /***** Form to remove a folder *****/
      Act_FormStart (Brw_ActRemoveFolder[Gbl.FileBrowser.Type]);
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         case Brw_FILE_BRW_COMMON_GRP:
         case Brw_FILE_BRW_ADMIN_MARKS_GRP:
            Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
            break;
         case Brw_FILE_BRW_ASSIGNMENTS_CRS:
         case Brw_FILE_BRW_WORKS_CRS:
            Usr_PutHiddenParUsrCodAll (Brw_ActRemoveFolder[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
            Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
            break;
         default:
            break;
        }
      Brw_ParamListFiles (Brw_IS_FOLDER,PathInTree,FileName);
      sprintf (Gbl.Title,Txt_Remove_folder_X,FileNameToShow);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/delon16x16.gif\""
	                 " alt=\"%s\" title=\"%s\" class=\"ICON16x16B\" />"
	                 "</form>",
               Gbl.Prefs.IconsURL,
               Gbl.Title,
               Gbl.Title);
     }
   else
      fprintf (Gbl.F.Out,"<img src=\"%s/deloff16x16.gif\" alt=\"\""
	                 " class=\"ICON16x16B\" />",
               Gbl.Prefs.IconsURL);
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/************** Write link e icon to copy a file o a folder ******************/
/*****************************************************************************/

static void Brw_PutIconCopy (Brw_FileType_t FileType,
                             const char *PathInTree,const char *FileName,const char *FileNameToShow)
  {
   extern const char *Txt_Copy_FOLDER_FILE_OR_LINK_X;

   fprintf (Gbl.F.Out,"<td class=\"BM%d\">",Gbl.RowEvenOdd);

   if (Gbl.FileBrowser.ICanEditFileOrFolder)
     {
      /***** Form to copy into the clipboard *****/
      Act_FormStart (Brw_ActCopy[Gbl.FileBrowser.Type]);
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         case Brw_FILE_BRW_COMMON_GRP:
         case Brw_FILE_BRW_ADMIN_MARKS_GRP:
            Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
            break;
         case Brw_FILE_BRW_ASSIGNMENTS_CRS:
         case Brw_FILE_BRW_WORKS_CRS:
            Usr_PutHiddenParUsrCodAll (Brw_ActCopy[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
            Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
            break;
         default:
            break;
        }
      Brw_ParamListFiles (FileType,PathInTree,FileName);
      sprintf (Gbl.Title,Txt_Copy_FOLDER_FILE_OR_LINK_X,FileNameToShow);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/copy_on16x16.gif\""
	                 " alt=\"%s\" title=\"%s\" class=\"ICON16x16B\" />"
	                 "</form>",
               Gbl.Prefs.IconsURL,
               Gbl.Title,
               Gbl.Title);
     }
   else
      fprintf (Gbl.F.Out,"<img src=\"%s/copy_off16x16.gif\" alt=\"\""
	                 " class=\"ICON16x16B\" />",
               Gbl.Prefs.IconsURL);
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/************** Write link e icon to paste a file or a folder ****************/
/*****************************************************************************/

static void Brw_PutIconPasteOn (const char *PathInTree,const char *FileName,const char *FileNameToShow)
  {
   extern const char *Txt_Paste_in_X;

   fprintf (Gbl.F.Out,"<td class=\"BM%d\">",Gbl.RowEvenOdd);

   /***** Form to paste the content of the clipboard *****/
   Act_FormStart (Brw_ActPaste[Gbl.FileBrowser.Type]);
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         Usr_PutHiddenParUsrCodAll (Brw_ActPaste[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
         Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
         break;
      default:
         break;
     }
   Brw_ParamListFiles (Brw_IS_FOLDER,PathInTree,FileName);
   sprintf (Gbl.Title,Txt_Paste_in_X,FileNameToShow);
   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/paste_on16x16.gif\""
	              " alt=\"%s\" title=\"%s\" class=\"ICON16x16B\" />"
	              "</form>"
	              "</td>",
            Gbl.Prefs.IconsURL,
            Gbl.Title,
            Gbl.Title);
  }

/*****************************************************************************/
/**************** Write link e icon to paste a file o a folder ***************/
/*****************************************************************************/

static void Brw_PutIconPasteOff (void)
  {
   fprintf (Gbl.F.Out,"<td class=\"BM%d\">"
	              "<img src=\"%s/paste_off16x16.gif\""
	              " alt=\"\" class=\"ICON16x16B\" />"
	              "</td>",
            Gbl.RowEvenOdd,Gbl.Prefs.IconsURL);
  }

/*****************************************************************************/
/*************** Indent and write icon to expand/contract folder *************/
/*****************************************************************************/

static void Brw_IndentAndWriteIconExpandContract (unsigned Level,Brw_ExpandTree_t ExpandTree,
                                                  const char *PathInTree,const char *FileName,const char *FileNameToShow)
  {
   extern const char *Txt_Expand_FOLDER_X;
   extern const char *Txt_Contract_FOLDER_X;

   fprintf (Gbl.F.Out,"<td style=\"text-align:left;\">"
	              "<table>"
	              "<tr>");
   Brw_IndentDependingOnLevel (Level);
   fprintf (Gbl.F.Out,"<td class=\"BM%d\">",Gbl.RowEvenOdd);

   switch (ExpandTree)
     {
      case Brw_EXPAND_TREE_NOTHING:
         fprintf (Gbl.F.Out,"<img src=\"%s/tr16x16.gif\" alt=\"\""
                            " class=\"ICON16x16B\" />",
                  Gbl.Prefs.IconsURL);
         break;
      case Brw_EXPAND_TREE_PLUS:
         /***** Form to expand folder *****/
         Act_FormStart (Brw_ActExpandFolder[Gbl.FileBrowser.Type]);
         switch (Gbl.FileBrowser.Type)
           {
            case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
            case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
            case Brw_FILE_BRW_COMMON_GRP:
            case Brw_FILE_BRW_SEE_MARKS_GRP:
            case Brw_FILE_BRW_ADMIN_MARKS_GRP:
               Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
               break;
            case Brw_FILE_BRW_ASSIGNMENTS_CRS:
            case Brw_FILE_BRW_WORKS_CRS:
               Usr_PutHiddenParUsrCodAll (Brw_ActExpandFolder[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
               Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
               break;
            default:
               break;
           }
         Brw_ParamListFiles (Brw_IS_FOLDER,PathInTree,FileName);
         sprintf (Gbl.Title,Txt_Expand_FOLDER_X,FileNameToShow);
         fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/expand16x16.gif\""
                            " alt=\"%s\" title=\"%s\" class=\"ICON16x16B\" />"
                            "</form>",
                  Gbl.Prefs.IconsURL,
                  Gbl.Title,
                  Gbl.Title);
         break;
      case Brw_EXPAND_TREE_MINUS:
         /***** Form to contract folder *****/
         Act_FormStart (Brw_ActContractFolder[Gbl.FileBrowser.Type]);
         switch (Gbl.FileBrowser.Type)
           {
            case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
            case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
            case Brw_FILE_BRW_COMMON_GRP:
            case Brw_FILE_BRW_SEE_MARKS_GRP:
            case Brw_FILE_BRW_ADMIN_MARKS_GRP:
               Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
               break;
            case Brw_FILE_BRW_ASSIGNMENTS_CRS:
            case Brw_FILE_BRW_WORKS_CRS:
               Usr_PutHiddenParUsrCodAll (Brw_ActContractFolder[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
               Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
               break;
            default:
               break;
           }
         Brw_ParamListFiles (Brw_IS_FOLDER,PathInTree,FileName);
         sprintf (Gbl.Title,Txt_Contract_FOLDER_X,FileNameToShow);
         fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/contract16x16.gif\""
                            " alt=\"%s\" title=\"%s\" class=\"ICON16x16B\" />"
                            "</form>",
                  Gbl.Prefs.IconsURL,
                  Gbl.Title,
                  Gbl.Title);
         break;
     }

   fprintf (Gbl.F.Out,"</td>"
		      "</tr>"
		      "</table>"
                      "</td>");
  }

/*****************************************************************************/
/************************* Indent depending on level *************************/
/*****************************************************************************/

static void Brw_IndentDependingOnLevel (unsigned Level)
  {
   unsigned i;

   for (i = 1;
	i < Level;
	i++)
      fprintf (Gbl.F.Out,"<td style=\"width:16px;\">"
	                 "<img src=\"%s/tr16x16.gif\" alt=\"\""
	                 " class=\"ICON16x16B\" />"
	                 "</td>",
               Gbl.Prefs.IconsURL);
  }

/*****************************************************************************/
/****************** Put link and icon to show file or folder *****************/
/*****************************************************************************/

static void Brw_PutIconShow (unsigned Level,Brw_FileType_t FileType,
                             const char *PathInTree,const char *FileName,const char *FileNameToShow)
  {
   extern const char *Txt_Show_FOLDER_FILE_OR_LINK_X;

   fprintf (Gbl.F.Out,"<td class=\"BM%d\">",Gbl.RowEvenOdd);
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
         Act_FormStart (ActShoDocCrs);
         break;
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         Act_FormStart (ActShoDocGrp);
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
         break;
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
         Act_FormStart (ActShoMrkCrs);
         break;
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         Act_FormStart (ActShoMrkGrp);
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
         break;
      default:
         break;
     }
   Brw_ParamListFiles (FileType,PathInTree,FileName);
   sprintf (Gbl.Title,Txt_Show_FOLDER_FILE_OR_LINK_X,FileNameToShow);
   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/hidden_%s16x16.gif\""
	              " alt=\"%s\" title=\"%s\" class=\"ICON16x16B\" />"
	              "</form>"
	              "</td>",
            Gbl.Prefs.IconsURL,
            Brw_CheckIfAnyUpperLevelIsHidden (Level) ? "off" :
        	                                       "on",
            Gbl.Title,
            Gbl.Title);
  }

/*****************************************************************************/
/****************** Put link and icon to hide file or folder *****************/
/*****************************************************************************/

static void Brw_PutIconHide (unsigned Level,Brw_FileType_t FileType,
                             const char *PathInTree,const char *FileName,const char *FileNameToShow)
  {
   extern const char *Txt_Hide_FOLDER_FILE_OR_LINK_X;

   fprintf (Gbl.F.Out,"<td class=\"BM%d\">",Gbl.RowEvenOdd);
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
         Act_FormStart (ActHidDocCrs);
         break;
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         Act_FormStart (ActHidDocGrp);
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
         break;
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
         Act_FormStart (ActHidMrkCrs);
         break;
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         Act_FormStart (ActHidMrkGrp);
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
         break;
      default:
         break;
     }
   Brw_ParamListFiles (FileType,PathInTree,FileName);
   sprintf (Gbl.Title,Txt_Hide_FOLDER_FILE_OR_LINK_X,FileNameToShow);
   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/visible_%s16x16.gif\""
	              " alt=\"%s\" title=\"%s\" class=\"ICON16x16B\" />"
	              "</form>"
	              "</td>",
            Gbl.Prefs.IconsURL,
            Brw_CheckIfAnyUpperLevelIsHidden (Level) ? "off" :
        	                                       "on",
            Gbl.Title,
            Gbl.Title);
  }

/*****************************************************************************/
/**** Check if any level of folders superior to the current one is hidden ****/
/*****************************************************************************/

static bool Brw_CheckIfAnyUpperLevelIsHidden (unsigned CurrentLevel)
  {
   unsigned N;

   for (N = 0;
	N < CurrentLevel;
	N++)
      if (Gbl.FileBrowser.HiddenLevels[N])
         return true;

   return false;
  }

/*****************************************************************************/
/** Write link e icon to upload or paste files, or to create folder or link **/
/*****************************************************************************/

static void Brw_PutIconFolder (unsigned Level,Brw_ExpandTree_t ExpandTree,
                               const char *PathInTree,const char *FileName,const char *FileNameToShow)
  {
   extern const char *Txt_Upload_file_or_create_folder_in_FOLDER;
   bool ICanCreate;

   /***** Start cell *****/
   fprintf (Gbl.F.Out,"<td style=\"width:%upx; text-align:left;\">",
            Level * 16);

   /***** Put icon *****/
   if ((ICanCreate = Brw_CheckIfICanCreateIntoFolder (Level)))	// I can create a new file or folder
     {
      /***** Form to create a new file or folder *****/
      Act_FormStart (Brw_ActFormCreate[Gbl.FileBrowser.Type]);
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         case Brw_FILE_BRW_COMMON_GRP:
         case Brw_FILE_BRW_ADMIN_MARKS_GRP:
            Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
            break;
         case Brw_FILE_BRW_ASSIGNMENTS_CRS:
         case Brw_FILE_BRW_WORKS_CRS:
	    Usr_PutHiddenParUsrCodAll (Brw_ActFormCreate[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
            Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
            break;
         default:
            break;
        }
      Brw_ParamListFiles (Brw_IS_FOLDER,PathInTree,FileName);
      sprintf (Gbl.Title,Txt_Upload_file_or_create_folder_in_FOLDER,FileNameToShow);
      fprintf (Gbl.F.Out,"<input type=\"image\""
	                 " src=\"%s/folder-%s-plus16x16.gif\" alt=\"%s\""
	                 " title=\"%s\" class=\"ICON16x16B\" />"
	                 "</form>",
               Gbl.Prefs.IconsURL,
               (ExpandTree == Brw_EXPAND_TREE_PLUS) ? "closed" :
        	                                      "open",
               Gbl.Title,
               Gbl.Title);
     }
   else	// I can't create a new file or folder
      fprintf (Gbl.F.Out,"<img src=\"%s/folder-%s16x16.gif\" alt=\"\""
	                 " class=\"ICON16x16B\" />",
               Gbl.Prefs.IconsURL,
               (ExpandTree == Brw_EXPAND_TREE_PLUS) ? "closed" :
        	                                      "open");

   /***** End cell *****/
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/**************** Write icon to indicate that the file is new ****************/
/*****************************************************************************/

static void Brw_PutIconNewFileOrFolder (void)
  {
   extern const char *Txt_New_FILE_OR_FOLDER;

   /***** Icon that indicates new file *****/
   fprintf (Gbl.F.Out,"<td class=\"BM%d\">"
	              "<img src=\"%s/star16x16.gif\""
	              " alt=\"%s\" title=\"%s\" class=\"ICON16x16B\" />"
	              "</td>",
            Gbl.RowEvenOdd,Gbl.Prefs.IconsURL,
            Txt_New_FILE_OR_FOLDER,Txt_New_FILE_OR_FOLDER);
  }

/*****************************************************************************/
/***************************** Put icon of a file ****************************/
/*****************************************************************************/
// FileType can be Brw_IS_FILE or Brw_IS_LINK

static void Brw_PutIconFileWithLinkToViewMetadata (unsigned Size,Brw_FileType_t FileType,
                                                   const char *PathInTree,const char *FileName,const char *FileNameToShow)
  {
   extern const char *Txt_View_data_of_FILE_OR_LINK_X;

   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
	 Act_FormStart (ActReqDatSeeDocCrs);
	 break;
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
	 Act_FormStart (ActReqDatAdmDocCrs);
	 break;
      case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
	 Act_FormStart (ActReqDatSeeDocGrp);
	 Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
	 break;
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
	 Act_FormStart (ActReqDatAdmDocGrp);
	 Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
	 break;
      case Brw_FILE_BRW_COMMON_CRS:
	 Act_FormStart (ActReqDatComCrs);
	 break;
      case Brw_FILE_BRW_COMMON_GRP:
	 Act_FormStart (ActReqDatComGrp);
	 Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
	 break;
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
	 Act_FormStart (ActReqDatAsgUsr);
	 break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
	 Act_FormStart (ActReqDatAsgCrs);
	 Usr_PutHiddenParUsrCodAll (ActReqDatAsgCrs,Gbl.Usrs.Select.All);
	 Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
	 break;
      case Brw_FILE_BRW_WORKS_USR:
	 Act_FormStart (ActReqDatWrkUsr);
	 break;
      case Brw_FILE_BRW_WORKS_CRS:
	 Act_FormStart (ActReqDatWrkCrs);
	 Usr_PutHiddenParUsrCodAll (ActReqDatWrkCrs,Gbl.Usrs.Select.All);
	 Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
	 break;
      case Brw_FILE_BRW_SEE_MARKS_CRS:
	 Act_FormStart (ActReqDatSeeMrkCrs);
	 break;
      case Brw_FILE_BRW_SEE_MARKS_GRP:
	 Act_FormStart (ActReqDatSeeMrkGrp);
	 Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
	 break;
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
	 Act_FormStart (ActReqDatAdmMrkCrs);
	 break;
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
	 Act_FormStart (ActReqDatAdmMrkGrp);
	 Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
	 break;
      case Brw_FILE_BRW_BRIEFCASE_USR:
	 Act_FormStart (ActReqDatBrf);
	 break;
      default:
	 break;
     }
   Brw_ParamListFiles (FileType,PathInTree,FileName);

   /***** Name and link of the file or folder *****/
   sprintf (Gbl.Title,Txt_View_data_of_FILE_OR_LINK_X,
	    FileNameToShow);

   /* Link to the form and to the file */
   fprintf (Gbl.F.Out,"<a href=\"javascript:document.getElementById('%s').submit();\""
		      " title=\"%s\" class=\"%s\">",
	    Gbl.FormId,
	    Gbl.Title,Gbl.FileBrowser.TxtStyle);

   /***** Icon depending on the file extension *****/
   Brw_PutIconFile (Size,FileType,FileName);

   /* End of the link and of the form */
   fprintf (Gbl.F.Out,"</a>"
                      "</form>");
  }

/*****************************************************************************/
/***************************** Put icon of a file ****************************/
/*****************************************************************************/

static void Brw_PutIconFile (unsigned Size,Brw_FileType_t FileType,const char *FileName)
  {
   extern const char *Txt_Link;
   extern const char *Txt_X_file;
   unsigned DocType;
   bool NotFound;

   /***** Icon depending on the file extension *****/
   fprintf (Gbl.F.Out,"<img src=\"%s/%s%ux%u/",
            Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_FILEXT,
            Size,Size);
   if (FileType == Brw_IS_LINK)
      fprintf (Gbl.F.Out,"url%ux%u.gif\" alt=\"%s\"",
	       Size,Size,Txt_Link);
   else	// FileType == Brw_IS_FILE
     {
      for (DocType = 0, NotFound = true;
	   DocType < Brw_NUM_FILE_EXT_ALLOWED && NotFound;
	   DocType++)
	 if (Str_FileIs (FileName,Brw_FileExtensionsAllowed[DocType]))
	   {
	    fprintf (Gbl.F.Out,"%s%ux%u.gif\" alt=\"",
		     Brw_FileExtensionsAllowed[DocType],
		     Size,Size);
	    fprintf (Gbl.F.Out,Txt_X_file,Brw_FileExtensionsAllowed[DocType]);
	    fprintf (Gbl.F.Out,"\"");
	    NotFound = false;
	   }
      if (NotFound)
	 fprintf (Gbl.F.Out,"xxx%ux%u.gif\" alt=\"\"",
	          Size,Size);
     }
   fprintf (Gbl.F.Out,(Size == 16) ? " class=\"ICON16x16B\"/>" :
	                             " class=\"ICON32x32\"/>");
  }

/*****************************************************************************/
/********** Write central part with the name of a file or folder *************/
/*****************************************************************************/

static void Brw_WriteFileName (unsigned Level,bool IsPublic,Brw_FileType_t FileType,
                               const char *PathInTree,const char *FileName,const char *FileNameToShow)
  {
   extern const char *Txt_Check_marks_in_file_X;
   extern const char *Txt_Download_FILE_OR_LINK_X;
   extern const char *Txt_Public_open_educational_resource_OER_for_everyone;

   /***** Name and link of the folder, file or link *****/
   if (FileType == Brw_IS_FOLDER)
     {
      /***** Start of cell *****/
      fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"width:99%%;"
	                 " text-align:left; vertical-align:middle;",
               Gbl.FileBrowser.TxtStyle);
      if (Gbl.FileBrowser.Clipboard.IsThisFile)
         fprintf (Gbl.F.Out," background-color:%s;",LIGHT_GREEN);
      fprintf (Gbl.F.Out,"\">");

      /***** Form to rename folder *****/
      if (Gbl.FileBrowser.ICanEditFileOrFolder)	// Can I rename this folder?
	{
         Act_FormStart (Brw_ActRenameFolder[Gbl.FileBrowser.Type]);
         switch (Gbl.FileBrowser.Type)
           {
            case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
            case Brw_FILE_BRW_COMMON_GRP:
            case Brw_FILE_BRW_ADMIN_MARKS_GRP:
               Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
               break;
            case Brw_FILE_BRW_ASSIGNMENTS_CRS:
            case Brw_FILE_BRW_WORKS_CRS:
	       Usr_PutHiddenParUsrCodAll (Brw_ActRenameFolder[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
               Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
               break;
            default:
               break;
           }
	 Brw_ParamListFiles (Brw_IS_FOLDER,PathInTree,FileName);
	}

      /***** Write name of the folder *****/
      fprintf (Gbl.F.Out,"&nbsp;");
      if (Gbl.FileBrowser.ICanEditFileOrFolder)	// Can I rename this folder?
      	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"NewFolderName\""
      	                    " size=\"40\" maxlength=\"40\" value=\"%s\""
                            " class=\"%s\" style=\"background-color:%s\""
                            " onchange=\"javascript:document.getElementById('%s').submit();\" />"
                            "</form>",
		  FileName,Gbl.FileBrowser.InputStyle,
                  Gbl.FileBrowser.Clipboard.IsThisFile ? LIGHT_GREEN :
                	                                 Gbl.ColorRows[Gbl.RowEvenOdd],
                  Gbl.FormId);
      else
        {
         if ((Level == 1) &&
             (Gbl.FileBrowser.Type == Brw_FILE_BRW_ASSIGNMENTS_USR ||
              Gbl.FileBrowser.Type == Brw_FILE_BRW_ASSIGNMENTS_CRS))
            fprintf (Gbl.F.Out,"<span title=\"%s\">",
                     Gbl.FileBrowser.Asg.Title);
         fprintf (Gbl.F.Out,"<strong>%s</strong>&nbsp;",FileNameToShow);
         if ((Level == 1) &&
             (Gbl.FileBrowser.Type == Brw_FILE_BRW_ASSIGNMENTS_USR ||
              Gbl.FileBrowser.Type == Brw_FILE_BRW_ASSIGNMENTS_CRS))
            fprintf (Gbl.F.Out,"</span>");
        }

      /***** End of cell *****/
      fprintf (Gbl.F.Out,"</td>");
     }
   else	// File or link
     {
      fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"width:99%%;"
	                 " text-align:left; vertical-align:middle;",
               Gbl.FileBrowser.TxtStyle);
      if (Gbl.FileBrowser.Clipboard.IsThisFile)
         fprintf (Gbl.F.Out," background-color:%s;",LIGHT_GREEN);
      fprintf (Gbl.F.Out,"\">&nbsp;");

      switch (Gbl.FileBrowser.Type)
        {
         case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
            Act_FormStart (ActDowSeeDocCrs);
            break;
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
            Act_FormStart (ActDowAdmDocCrs);
            break;
         case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
            Act_FormStart (ActDowSeeDocGrp);
            Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
            break;
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
            Act_FormStart (ActDowAdmDocGrp);
            Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
            break;
         case Brw_FILE_BRW_COMMON_CRS:
            Act_FormStart (ActDowComCrs);
            break;
         case Brw_FILE_BRW_COMMON_GRP:
            Act_FormStart (ActDowComGrp);
            Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
            break;
         case Brw_FILE_BRW_ASSIGNMENTS_USR:
            Act_FormStart (ActDowAsgUsr);
            break;
         case Brw_FILE_BRW_ASSIGNMENTS_CRS:
            Act_FormStart (ActDowAsgCrs);
	    Usr_PutHiddenParUsrCodAll (ActDowAsgCrs,Gbl.Usrs.Select.All);
            Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
            break;
         case Brw_FILE_BRW_WORKS_USR:
            Act_FormStart (ActDowWrkUsr);
            break;
         case Brw_FILE_BRW_WORKS_CRS:
            Act_FormStart (ActDowWrkCrs);
	    Usr_PutHiddenParUsrCodAll (ActDowWrkCrs,Gbl.Usrs.Select.All);
            Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
            break;
         case Brw_FILE_BRW_SEE_MARKS_CRS:
	    Act_FormStart (ActSeeMyMrkCrs);
            break;
         case Brw_FILE_BRW_SEE_MARKS_GRP:
            Act_FormStart (ActSeeMyMrkGrp);
            Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
            break;
         case Brw_FILE_BRW_ADMIN_MARKS_CRS:
            Act_FormStart (ActDowAdmMrkCrs);
            break;
         case Brw_FILE_BRW_ADMIN_MARKS_GRP:
            Act_FormStart (ActDowAdmMrkGrp);
            Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
            break;
         case Brw_FILE_BRW_BRIEFCASE_USR:
            Act_FormStart (ActDowBrf);
            break;
         default:
            break;
        }
      Brw_ParamListFiles (FileType,PathInTree,FileName);

      /* Link to the form and to the file */
      sprintf (Gbl.Title,(Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_MARKS_CRS ||
	                  Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_MARKS_GRP) ? Txt_Check_marks_in_file_X :
	                	                                                Txt_Download_FILE_OR_LINK_X,
	       FileNameToShow);
      fprintf (Gbl.F.Out,"<a href=\"javascript:document.getElementById('%s').submit();\""
			 " title=\"%s\" class=\"%s\">"
			 "%s"
			 "</a>"
			 "</form>",
	       Gbl.FormId,
	       Gbl.Title,Gbl.FileBrowser.TxtStyle,
	       FileNameToShow);

      /* Put icon to make public/private file */
      if (IsPublic)
         fprintf (Gbl.F.Out,"&nbsp;<img src=\"%s/open_on16x16.gif\""
                            " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />",
                  Gbl.Prefs.IconsURL,
                  Txt_Public_open_educational_resource_OER_for_everyone,
                  Txt_Public_open_educational_resource_OER_for_everyone);

      fprintf (Gbl.F.Out,"</td>");
     }
  }

/*****************************************************************************/
/*********************** Which filename must be shown? ***********************/
/*****************************************************************************/

static void Brw_GetFileNameToShow (Brw_FileBrowser_t FileBrowser,unsigned Level,Brw_FileType_t FileType,
                                   const char *FileName,char *FileNameToShow)
  {
   extern const char *Txt_ROOT_FOLDER_EXTERNAL_NAMES[Brw_NUM_TYPES_FILE_BROWSER];

   Brw_LimitLengthFileNameToShow (FileType,
                                  Level ? FileName :
					  Txt_ROOT_FOLDER_EXTERNAL_NAMES[FileBrowser],
				  FileNameToShow);
  }

/*****************************************************************************/
/*************** Limit the length of the filename to be shown ****************/
/*****************************************************************************/
// FileNameToShow must have at least NAME_MAX+1 bytes

static void Brw_LimitLengthFileNameToShow (Brw_FileType_t FileType,const char *FileName,char *FileNameToShow)
  {
   size_t NumCharsToCopy;

   /***** Limit length of the name of the file *****/
   NumCharsToCopy = strlen (FileName);
   if (FileType == Brw_IS_LINK)	// It's a link (URL inside a .url file)
      NumCharsToCopy -= 4;	// Remove .url
   if (NumCharsToCopy > NAME_MAX)
      NumCharsToCopy = NAME_MAX;
   strncpy (FileNameToShow,FileName,NumCharsToCopy);
   FileNameToShow[NumCharsToCopy] = '\0';
   Str_LimitLengthHTMLStr (FileNameToShow,60);
  }

/*****************************************************************************/
/****** Create a link in public temporary directory to download a file *******/
/*****************************************************************************/

static void Brw_CreateTmpLinkToDownloadFileBrowser (const char *FullPathIncludingFile,const char *FileName)
  {
   char Link[PATH_MAX+1];

   /***** Create, into temporary download directory, a symbolic link to file *****/
   sprintf (Link,"%s/%s/%s/%s",
            Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_FILE_BROWSER_TMP,
            Gbl.FileBrowser.TmpPubDir,FileName);
   if (symlink (FullPathIncludingFile,Link) != 0)
      Lay_ShowErrorAndExit ("Can not create temporary link for download.");
  }

/*****************************************************************************/
/***************** Write parameters of a row of file list ********************/
/*****************************************************************************/

void Brw_ParamListFiles (Brw_FileType_t FileType,const char *PathInTree,const char *FileName)
  {
   Brw_PutParamsPathAndFile (FileType,PathInTree,FileName);
   if (Gbl.FileBrowser.FullTree)
      Par_PutHiddenParamChar ("FullTree",'Y');
  }

/*****************************************************************************/
/************ Write start and end dates of a folder of assignment ************/
/*****************************************************************************/

static void Brw_WriteDatesAssignment (void)
  {
   extern const char *Txt_unknown_assignment;

   fprintf (Gbl.F.Out,"<td colspan=\"4\" class=\"ASG_LST_DATE_GREEN\""
	              " style=\"text-align:right; vertical-align:middle;"
	              " background-color:%s;\">",
               Gbl.ColorRows[Gbl.RowEvenOdd]);

   if (Gbl.FileBrowser.Asg.AsgCod > 0)
     {
      fprintf (Gbl.F.Out,"<table>"
	                 "<tr>");

      /***** Write start date *****/
      fprintf (Gbl.F.Out,"<td class=\"%s\""
	                 " style=\"text-align:right; vertical-align:middle;\">"
                         "&nbsp;%02u/%02u/%02u&nbsp;%02u:%02u"
                         "</td>",
               Gbl.FileBrowser.Asg.Open ? "ASG_LST_DATE_GREEN" :
                                          "ASG_LST_DATE_RED",
               Gbl.FileBrowser.Asg.DateTimes[Asg_START_TIME].Date.Day,
               Gbl.FileBrowser.Asg.DateTimes[Asg_START_TIME].Date.Month,
	       Gbl.FileBrowser.Asg.DateTimes[Asg_START_TIME].Date.Year % 100,
	       Gbl.FileBrowser.Asg.DateTimes[Asg_START_TIME].Time.Hour,
	       Gbl.FileBrowser.Asg.DateTimes[Asg_START_TIME].Time.Minute);

      /***** Arrow *****/
      fprintf (Gbl.F.Out,"<td style=\"width:16px;"
	                 " text-align:right; vertical-align:middle;\">"
                         "<img src=\"%s/arrow%s16x12.gif\" alt=\"\""
                         " class=\"ICON16x16B\" />"
                         "</td>",
               Gbl.Prefs.IconsURL,
               Gbl.FileBrowser.Asg.Open ? "green" :
        	                          "red");

      /***** Write start date *****/
      fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:right;\">"
                         "%02u/%02u/%02u&nbsp;%02u:%02u"
                         "</td>",
               Gbl.FileBrowser.Asg.Open ? "ASG_LST_DATE_GREEN" :
                                          "ASG_LST_DATE_RED",
               Gbl.FileBrowser.Asg.DateTimes[Asg_END_TIME  ].Date.Day,
               Gbl.FileBrowser.Asg.DateTimes[Asg_END_TIME  ].Date.Month,
               Gbl.FileBrowser.Asg.DateTimes[Asg_END_TIME  ].Date.Year % 100,
	       Gbl.FileBrowser.Asg.DateTimes[Asg_END_TIME  ].Time.Hour,
	       Gbl.FileBrowser.Asg.DateTimes[Asg_END_TIME  ].Time.Minute);

      fprintf (Gbl.F.Out,"</tr>"
	                 "</table>");
     }
   else
      fprintf (Gbl.F.Out,"&nbsp;(%s)",
               Txt_unknown_assignment);
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/****************** Write size and date of a file or folder ******************/
/*****************************************************************************/

static void Brw_WriteFileSizeAndDate (Brw_FileType_t FileType,struct FileMetadata *FileMetadata)
  {
   /***** Write the file size *****/
   fprintf (Gbl.F.Out,"<td class=\"%s\""
	              " style=\"text-align:right; background-color:%s;\">"
	              "&nbsp;",
            Gbl.FileBrowser.TxtStyle,Gbl.ColorRows[Gbl.RowEvenOdd]);
   if (FileType == Brw_IS_FILE)
      Str_WriteSizeInBytesBrief ((double) FileMetadata->Size);
   fprintf (Gbl.F.Out,"</td>");

   /***** Write the date *****/
   fprintf (Gbl.F.Out,"<td class=\"%s\""
	              " style=\"text-align:right; background-color:%s;\">"
	              "&nbsp;",
            Gbl.FileBrowser.TxtStyle,Gbl.ColorRows[Gbl.RowEvenOdd]);
   if (FileType == Brw_IS_FILE ||
       FileType == Brw_IS_LINK)
     {
      Dat_GetLocalTimeFromClock (&(FileMetadata->Time));
      Dat_WriteDateFromtblock ();
     }
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/************** Write the user who published the file or folder **************/
/*****************************************************************************/

static void Brw_WriteFileOrFolderPublisher (unsigned Level,unsigned long UsrCod)
  {
   bool ShowUsr = false;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];
   struct UsrData UsrDat;

   if (Level && UsrCod > 0)
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Get data of file/folder publisher *****/
      UsrDat.UsrCod = UsrCod;
      ShowUsr = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);	// Get user's data from database
     }

   fprintf (Gbl.F.Out,"<td class=\"BM%d\">",Gbl.RowEvenOdd);
   if (ShowUsr)
     {
      /***** Show photo *****/
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                                            NULL,
                        "PHOTO12x16B",true);
     }
   else
      fprintf (Gbl.F.Out,"<img src=\"%s/usr_bl.jpg\" alt=\"\""
	                 " class=\"PHOTO12x16B\" />",
               Gbl.Prefs.IconsURL);
   fprintf (Gbl.F.Out,"</td>");

   if (Level && UsrCod > 0)
      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************** Ask for confirmation of removing a file or link **************/
/*****************************************************************************/

void Brw_AskRemFileFromTree (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_FILE_OR_LINK_X;
   extern const char *Txt_Remove_file;
   extern const char *Txt_Remove_link;
   extern const char *Txt_You_can_not_remove_this_file_or_link;
   char FileNameToShow[NAME_MAX+1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Button of confirmation of removing *****/
   if (Brw_CheckIfICanEditFileOrFolder (Gbl.FileBrowser.Level))	// Can I remove this file?
     {
      /***** Form to ask for confirmation to remove a file *****/
      Act_FormStart (Brw_ActRemoveFile[Gbl.FileBrowser.Type]);
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         case Brw_FILE_BRW_COMMON_GRP:
         case Brw_FILE_BRW_ADMIN_MARKS_GRP:
            Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
            break;
         case Brw_FILE_BRW_ASSIGNMENTS_CRS:
         case Brw_FILE_BRW_WORKS_CRS:
            Usr_PutHiddenParUsrCodAll (Brw_ActRemoveFile[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
            Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
            break;
         default:
            break;
        }
      Brw_ParamListFiles (Gbl.FileBrowser.FileType,Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,Gbl.FileBrowser.FilFolLnkName);

      /* Show question */
      Brw_GetFileNameToShow (Gbl.FileBrowser.FileType,Gbl.FileBrowser.Level,Gbl.FileBrowser.FileType,
                             Gbl.FileBrowser.FilFolLnkName,FileNameToShow);
      sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_FILE_OR_LINK_X,
               FileNameToShow);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
      fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">"
                         "<input type=\"submit\" value=\"%s\" />"
                         "</div>"
                         "</form>",
               Gbl.FileBrowser.FileType == Brw_IS_FILE ? Txt_Remove_file :
        	                                         Txt_Remove_link);
     }
   else
      Lay_ShowErrorAndExit (Txt_You_can_not_remove_this_file_or_link);

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/************************ Remove a file of a file browser ********************/
/*****************************************************************************/

void Brw_RemFileFromTree (void)
  {
   extern const char *Txt_FILE_X_removed;
   extern const char *Txt_You_can_not_remove_this_file_or_link;
   char Path[PATH_MAX+1];
   struct stat FileStatus;
   char FileNameToShow[NAME_MAX+1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   if (Brw_CheckIfICanEditFileOrFolder (Gbl.FileBrowser.Level))	// Can I remove this file?
     {
      sprintf (Path,"%s/%s",Gbl.FileBrowser.Priv.PathAboveRootFolder,Gbl.FileBrowser.Priv.FullPathInTree);

      /***** Check if is a file/link or a folder *****/
      lstat (Path,&FileStatus);
      if (S_ISREG (FileStatus.st_mode))		// It's a file or a link
        {
	 /***** Name of the file/link to be shown *****/
	 Brw_LimitLengthFileNameToShow (Str_FileIs (Gbl.FileBrowser.FilFolLnkName,"url") ? Brw_IS_LINK :
											   Brw_IS_FILE,
					Gbl.FileBrowser.FilFolLnkName,FileNameToShow);

	 if (unlink (Path))
            Lay_ShowErrorAndExit ("Can not remove file / link.");

         /* If a file is removed,
            it is necessary to remove it from the database */
         Brw_RemoveOneFileOrFolderFromDB (Gbl.FileBrowser.Priv.FullPathInTree);

         /* Remove affected clipboards */
         Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Other.UsrDat.UsrCod);

	 /* Message of confirmation of removing */
	 sprintf (Gbl.Message,Txt_FILE_X_removed,FileNameToShow);
         Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
        }
      else		// File / link not found
         Lay_ShowErrorAndExit ("File / link not found.");
     }
   else
      Lay_ShowErrorAndExit (Txt_You_can_not_remove_this_file_or_link);

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/******************* Remove a folder of a file browser ***********************/
/*****************************************************************************/

void Brw_RemFolderFromTree (void)
  {
   extern const char *Txt_Folder_X_removed;
   extern const char *Txt_You_can_not_remove_this_folder;
   char Path[PATH_MAX+1];
   struct stat FileStatus;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   if (Brw_CheckIfICanEditFileOrFolder (Gbl.FileBrowser.Level))	// Can I remove this folder?
     {
      sprintf (Path,"%s/%s",Gbl.FileBrowser.Priv.PathAboveRootFolder,Gbl.FileBrowser.Priv.FullPathInTree);

      /***** Check if it's a file or a folder *****/
      lstat (Path,&FileStatus);
      if (S_ISDIR (FileStatus.st_mode))		// It's a directory
         if (rmdir (Path))
           {
	    if (errno == ENOTEMPTY)	// The directory is not empty
	      {
	       Brw_AskConfirmRemoveFolderNotEmpty ();
	       Gbl.Message[0] = '\0';
	      }
	    else	// The directory is empty
               Lay_ShowErrorAndExit ("Can not remove folder.");
           }
         else
           {
            /* If a folder is removed,
               it is necessary to remove it from the database */
            Brw_RemoveOneFileOrFolderFromDB (Gbl.FileBrowser.Priv.FullPathInTree);

            /* Remove affected clipboards */
            Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Other.UsrDat.UsrCod);

            /* Remove affected expanded folders */
            Brw_RemoveAffectedExpandedFolders (Gbl.FileBrowser.Priv.FullPathInTree);

            /* Message of confirmation of successfull removing */
	    sprintf (Gbl.Message,Txt_Folder_X_removed,
                     Gbl.FileBrowser.FilFolLnkName);
            Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
           }
      else		// Folder not found
         Lay_ShowErrorAndExit ("Folder not found.");
     }
   else
      Lay_ShowErrorAndExit (Txt_You_can_not_remove_this_folder);

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/*********** Ask for confirmation of removing of a folder no empty ***********/
/*****************************************************************************/

static void Brw_AskConfirmRemoveFolderNotEmpty (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_folder_X;
   extern const char *Txt_Remove_folder;

   /***** Form to remove a not empty folder *****/
   Act_FormStart (Brw_ActRemoveFolderNotEmpty[Gbl.FileBrowser.Type]);
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         Usr_PutHiddenParUsrCodAll (Brw_ActRemoveFolderNotEmpty[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
         Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
         break;
      default:
         break;
     }
   Brw_ParamListFiles (Brw_IS_FOLDER,Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,Gbl.FileBrowser.FilFolLnkName);
   sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_folder_X,
            Gbl.FileBrowser.FilFolLnkName);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);
   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">"
                      "<input type=\"submit\" value=\"%s\" />"
                      "</div>"
                      "</form>",
            Txt_Remove_folder);
  }

/*****************************************************************************/
/********* Complete removing of a not empty folder in a file browser *********/
/*****************************************************************************/

void Brw_RemSubtreeInFileBrowser (void)
  {
   extern const char *Txt_Folder_X_and_all_its_contents_removed;
   char Path[PATH_MAX+1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   if (Brw_CheckIfICanEditFileOrFolder (Gbl.FileBrowser.Level))	// Can I remove this subtree?
     {
      sprintf (Path,"%s/%s",Gbl.FileBrowser.Priv.PathAboveRootFolder,Gbl.FileBrowser.Priv.FullPathInTree);

      /***** Remove the whole tree *****/
      Brw_RemoveTree (Path);

      /* If a folder is removed,
         it is necessary to remove it from the database and all the files o folders under that folder */
      Brw_RemoveOneFileOrFolderFromDB (Gbl.FileBrowser.Priv.FullPathInTree);
      Brw_RemoveChildrenOfFolderFromDB (Gbl.FileBrowser.Priv.FullPathInTree);

      /* Remove affected clipboards */
      Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Other.UsrDat.UsrCod);

      /* Remove affected expanded folders */
      Brw_RemoveAffectedExpandedFolders (Gbl.FileBrowser.Priv.FullPathInTree);

      /***** Write message of confirmation *****/
      sprintf (Gbl.Message,Txt_Folder_X_and_all_its_contents_removed,
               Gbl.FileBrowser.FilFolLnkName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/************************* Remove a folder recursively ***********************/
/*****************************************************************************/
// If the tree of directories and files exists, remove it

void Brw_RemoveTree (const char *Path)
  {
   struct stat FileStatus;
   struct dirent **FileList;
   int NumFile,NumFiles;
   char PathFileRel[PATH_MAX+1];
   bool Error;

   if (Fil_CheckIfPathExists (Path))
     {
      lstat (Path,&FileStatus);
      if (S_ISDIR (FileStatus.st_mode))		// It's a directory
	{
	 if (rmdir (Path))
	   {
	    Error = false;
	    if (errno == ENOTEMPTY)
	      {
	       /***** Remove each directory and file under this directory *****/
	       /* Scan the directory */
	       NumFiles = scandir (Path,&FileList,NULL,NULL);

	       /* Remove recursively all the directories and files */
	       for (NumFile = 0;
		    NumFile < NumFiles;
		    NumFile++)
		  if (strcmp (FileList[NumFile]->d_name,".") &&
                      strcmp (FileList[NumFile]->d_name,".."))	// Skip directories "." and ".."
		    {
		     sprintf (PathFileRel,"%s/%s",Path,FileList[NumFile]->d_name);
		     Brw_RemoveTree (PathFileRel);
		    }

	       /***** Remove of new the directory, now empty *****/
	       if (rmdir (Path))
		  Error = true;
	      }
	    else
	       Error = true;
	    if (Error)
	      {
	       sprintf (Gbl.Message,"Can not remove folder %s.",Path);
	       Lay_ShowErrorAndExit (Gbl.Message);
	      }
	   }
	}
      else					// It's a file
	 if (unlink (Path))
	    Lay_ShowErrorAndExit ("Can not remove file.");
     }
  }

/*****************************************************************************/
/********************* Expand a folder in a file browser *********************/
/*****************************************************************************/

void Brw_ExpandFileTree (void)
  {
   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Add path to table of expanded folders *****/
   Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.Priv.FullPathInTree);

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/******************* Contract a folder in a file browser *********************/
/*****************************************************************************/

void Brw_ContractFileTree (void)
  {
   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Remove path where the user has clicked from table of expanded folders *****/
   Brw_RemThisFolderAndUpdOtherFoldersFromExpandedFolders (Gbl.FileBrowser.Priv.FullPathInTree);

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/****** Copy the path of a file/folder of a file browser in clipboard ********/
/*****************************************************************************/

void Brw_CopyFromFileBrowser (void)
  {
   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Remove old clipboards (from all users) *****/
   Brw_RemoveExpiredClipboards ();   // Someone must do this work. Let's do it whenever a user click in a copy button

   /***** Put the path in the clipboard *****/
   if (Brw_GetMyClipboard ())
      Brw_UpdatePathInClipboard (Gbl.FileBrowser.FileType,
                                 Gbl.FileBrowser.Priv.FullPathInTree);
   else
      Brw_AddPathToClipboards (Gbl.FileBrowser.FileType,
                               Gbl.FileBrowser.Priv.FullPathInTree);

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/**************** Get the path of my clipboard and write it ******************/
/*****************************************************************************/

static inline void Brw_GetAndWriteClipboard (void)
  {
   if (Brw_GetMyClipboard ())
      Brw_WriteCurrentClipboard ();
  }

/*****************************************************************************/
/********* Write a title with the content of the current clipboard ***********/
/*****************************************************************************/

static void Brw_WriteCurrentClipboard (void)
  {
   extern const char *Txt_Copy_source;
   extern const char *Txt_documents_management_zone;
   extern const char *Txt_common_zone;
   extern const char *Txt_assignments_zone;
   extern const char *Txt_works_zone;
   extern const char *Txt_marks_management_zone;
   extern const char *Txt_private_storage_zone;
   extern const char *Txt_course;
   extern const char *Txt_group;
   extern const char *Txt_user[Usr_NUM_SEXS];
   extern const char *Txt_all_files;
   extern const char *Txt_file_folder;
   extern const char *Txt_file;
   extern const char *Txt_folder;
   extern const char *Txt_link;
   struct Course Crs;
   struct GroupData GrpDat;
   struct UsrData UsrDat;
   unsigned LevelClipboard;
   const char *Ptr;
   char FileNameToShow[NAME_MAX+1];

   if (Gbl.FileBrowser.Clipboard.FileBrowser != Brw_FILE_BRW_BRIEFCASE_USR)
     {
      Crs.CrsCod = Gbl.FileBrowser.Clipboard.CrsCod;
      Crs_GetDataOfCourseByCod (&Crs);
     }

   fprintf (Gbl.F.Out,"<div class=\"DAT\" style=\"text-align:center;\">"
	              "%s: ",
            Txt_Copy_source);
   switch (Gbl.FileBrowser.Clipboard.FileBrowser)
     {
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
         fprintf (Gbl.F.Out,"%s, %s <strong>%s</strong>",
                  Txt_documents_management_zone,
                  Txt_course,Crs.ShortName);
         break;
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.GrpCod;
         Grp_GetDataOfGroupByCod (&GrpDat);
         fprintf (Gbl.F.Out,"%s, %s <strong>%s</strong>, %s <strong>%s %s</strong>",
                  Txt_documents_management_zone,
                  Txt_course,Crs.ShortName,
                  Txt_group,GrpDat.GrpTypName,GrpDat.GrpName);
         break;
      case Brw_FILE_BRW_COMMON_CRS:
         fprintf (Gbl.F.Out,"%s, %s <strong>%s</strong>",
                  Txt_common_zone,
                  Txt_course,Crs.ShortName);
         break;
      case Brw_FILE_BRW_COMMON_GRP:
         GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.GrpCod;
         Grp_GetDataOfGroupByCod (&GrpDat);
         fprintf (Gbl.F.Out,"%s, %s <strong>%s</strong>, %s <strong>%s %s</strong>",
                  Txt_common_zone,
                  Txt_course,Crs.ShortName,
                  Txt_group,GrpDat.GrpTypName,GrpDat.GrpName);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
         fprintf (Gbl.F.Out,"%s, %s <strong>%s</strong>, %s <strong>%s</strong>",
                  Txt_assignments_zone,
                  Txt_course,Crs.ShortName,
                  Txt_user[Gbl.Usrs.Me.UsrDat.Sex],Gbl.Usrs.Me.UsrDat.FullName);
         break;
      case Brw_FILE_BRW_WORKS_USR:
         fprintf (Gbl.F.Out,"%s, %s <strong>%s</strong>, %s <strong>%s</strong>",
                  Txt_works_zone,
                  Txt_course,Crs.ShortName,
                  Txt_user[Gbl.Usrs.Me.UsrDat.Sex],Gbl.Usrs.Me.UsrDat.FullName);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
         Usr_UsrDataConstructor (&UsrDat);
         UsrDat.UsrCod = Gbl.FileBrowser.Clipboard.WorksUsrCod;
         Usr_GetAllUsrDataFromUsrCod (&UsrDat);
         fprintf (Gbl.F.Out,"%s, %s <strong>%s</strong>, %s <strong>%s</strong>",
                  Txt_assignments_zone,
                  Txt_course,Crs.ShortName,
                  Txt_user[UsrDat.Sex],UsrDat.FullName);
         Usr_UsrDataDestructor (&UsrDat);
         break;
      case Brw_FILE_BRW_WORKS_CRS:
         Usr_UsrDataConstructor (&UsrDat);
         UsrDat.UsrCod = Gbl.FileBrowser.Clipboard.WorksUsrCod;
         Usr_GetAllUsrDataFromUsrCod (&UsrDat);
         fprintf (Gbl.F.Out,"%s, %s <strong>%s</strong>, %s <strong>%s</strong>",
                  Txt_works_zone,
                  Txt_course,Crs.ShortName,
                  Txt_user[UsrDat.Sex],UsrDat.FullName);
         Usr_UsrDataDestructor (&UsrDat);
         break;
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
         fprintf (Gbl.F.Out,"%s, %s <strong>%s</strong>",
                  Txt_marks_management_zone,
                  Txt_course,Crs.ShortName);
         break;
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.GrpCod;
         Grp_GetDataOfGroupByCod (&GrpDat);
         fprintf (Gbl.F.Out,"%s, %s <strong>%s</strong>, %s <strong>%s %s</strong>",
                  Txt_marks_management_zone,
                  Txt_course,Crs.ShortName,
                  Txt_group,GrpDat.GrpTypName,GrpDat.GrpName);
         break;
      case Brw_FILE_BRW_BRIEFCASE_USR:
         fprintf (Gbl.F.Out,"%s, %s <strong>%s</strong>",
                  Txt_private_storage_zone,
                  Txt_user[Gbl.Usrs.Me.UsrDat.Sex],Gbl.Usrs.Me.UsrDat.FullName);
         break;
      default:
         break;
     }
   fprintf (Gbl.F.Out,", ");

   // LevelClipboard == number-of-slashes-in-full-path-including-file-or-folder
   for (LevelClipboard = 0, Ptr = Gbl.FileBrowser.Clipboard.Path;
	*Ptr;
	Ptr++)
      if (*Ptr == '/')
         LevelClipboard++;
   if (LevelClipboard)					// Is not the root folder?
     {
      Brw_GetFileNameToShow (Gbl.FileBrowser.Clipboard.FileBrowser,LevelClipboard,Gbl.FileBrowser.Clipboard.FileType,
                             Gbl.FileBrowser.Clipboard.FileName,FileNameToShow);
      switch (Gbl.FileBrowser.Clipboard.FileType)
        {
	 case Brw_IS_UNKNOWN:
	    fprintf (Gbl.F.Out,"%s",Txt_file_folder);
	    break;
	 case Brw_IS_FILE:
	    fprintf (Gbl.F.Out,"%s",Txt_file);
	    break;
	 case Brw_IS_FOLDER:
	    fprintf (Gbl.F.Out,"%s",Txt_folder);
	    break;
	 case Brw_IS_LINK:
	    fprintf (Gbl.F.Out,"%s",Txt_link);
	    break;
        }
      fprintf (Gbl.F.Out," <strong>%s</strong>",
               FileNameToShow);				// It's not the root folder
     }
   else
      fprintf (Gbl.F.Out,"%s",Txt_all_files);		// It's the root folder
   fprintf (Gbl.F.Out,".</div>");
  }

/*****************************************************************************/
/********************** Get data of my current clipboard *********************/
/*****************************************************************************/
// Returns true if something found
// Returns false and void data if nothing found

static bool Brw_GetMyClipboard (void)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char PathUntilFileName[PATH_MAX+1];
   unsigned NumRows;
   unsigned UnsignedNum;

   /***** Get my current clipboard from database *****/
   sprintf (Query,"SELECT FileBrowser,CrsCod,GrpCod,WorksUsrCod,FileType,Path"
	          " FROM clipboard WHERE UsrCod='%ld'",
            Gbl.Usrs.Me.UsrDat.UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get source of copy from clipboard");

   /***** Depending on the number of rows of the result... *****/
   if (NumRows == 0)
     {
      /***** Clear clipboard data *****/
      Gbl.FileBrowser.Clipboard.FileBrowser = Brw_FILE_BRW_UNKNOWN;
      Gbl.FileBrowser.Clipboard.CrsCod      = -1L;
      Gbl.FileBrowser.Clipboard.GrpCod      = -1L;
      Gbl.FileBrowser.Clipboard.WorksUsrCod = -1L;
      Gbl.FileBrowser.Clipboard.FileType    = Brw_IS_UNKNOWN;
      Gbl.FileBrowser.Clipboard.Path[0]     = '\0';
      Gbl.FileBrowser.Clipboard.FileName[0] = '\0';
     }
   else if (NumRows == 1)
     {
      /***** Get clipboard data *****/
      row = mysql_fetch_row (mysql_res);

      /* Get file browser type (row[0]) */
      if (sscanf (row[0],"%u",&UnsignedNum) == 1)
        {
         Gbl.FileBrowser.Clipboard.FileBrowser = (Brw_FileBrowser_t) UnsignedNum;

         /* Get course code (row[1]) */
         Gbl.FileBrowser.Clipboard.CrsCod = Str_ConvertStrCodToLongCod (row[1]);

         /* Get group code (row[2]) */
         if (sscanf (row[2],"%ld",&Gbl.FileBrowser.Clipboard.GrpCod) != 1)
            Gbl.FileBrowser.Clipboard.GrpCod = -1L;

         /* Get works user's code (row[3]) */
         Gbl.FileBrowser.Clipboard.WorksUsrCod = Str_ConvertStrCodToLongCod (row[3]);

         /* Get file type (row[4]) */
         Gbl.FileBrowser.Clipboard.FileType = Brw_IS_UNKNOWN;	// default
         if (sscanf (row[4],"%u",&UnsignedNum) == 1)
            if (UnsignedNum < Brw_NUM_FILE_TYPES)
               Gbl.FileBrowser.Clipboard.FileType = (Brw_FileType_t) UnsignedNum;

         /* Get file path (row[5]) */
         strcpy (Gbl.FileBrowser.Clipboard.Path,row[5]);
         Str_SplitFullPathIntoPathAndFileName (Gbl.FileBrowser.Clipboard.Path,
                                               PathUntilFileName,
                                               Gbl.FileBrowser.Clipboard.FileName);
        }
      else
        {
         Gbl.FileBrowser.Clipboard.FileBrowser = Brw_FILE_BRW_UNKNOWN;
         Gbl.FileBrowser.Clipboard.CrsCod      = -1L;
         Gbl.FileBrowser.Clipboard.GrpCod      = -1L;
         Gbl.FileBrowser.Clipboard.WorksUsrCod = -1L;
         Gbl.FileBrowser.Clipboard.FileType    = Brw_IS_UNKNOWN;
         Gbl.FileBrowser.Clipboard.Path[0]     = '\0';
         Gbl.FileBrowser.Clipboard.FileName[0] = '\0';
        }
     }
   else
      Lay_ShowErrorAndExit ("Error when getting source of copy.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return (bool) (NumRows != 0);
  }

/*****************************************************************************/
/********* Check if the clipboard is in the current file browser *************/
/*****************************************************************************/
// Asigna false o true a Gbl.FileBrowser.Clipboard.IsThisTree

static bool Brw_CheckIfClipboardIsInThisTree (void)
  {
   bool IsThisTree = false;

   if (Gbl.FileBrowser.Clipboard.FileBrowser == Gbl.FileBrowser.Type)
     {
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         case Brw_FILE_BRW_COMMON_CRS:
         case Brw_FILE_BRW_COMMON_GRP:
         case Brw_FILE_BRW_ADMIN_MARKS_CRS:
         case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         case Brw_FILE_BRW_ASSIGNMENTS_USR:
         case Brw_FILE_BRW_WORKS_USR:
         case Brw_FILE_BRW_ASSIGNMENTS_CRS:
         case Brw_FILE_BRW_WORKS_CRS:
            if (Gbl.FileBrowser.Clipboard.CrsCod == Gbl.CurrentCrs.Crs.CrsCod)
               IsThisTree = true;		// We are in the course of the clipboard
            break;
         case Brw_FILE_BRW_BRIEFCASE_USR:
            IsThisTree = true;
            break;
         default:
            break;
        }
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         case Brw_FILE_BRW_COMMON_GRP:
         case Brw_FILE_BRW_ADMIN_MARKS_GRP:
            if (Gbl.FileBrowser.Clipboard.GrpCod != Gbl.CurrentCrs.Grps.GrpCod)
               IsThisTree = false;	// We are not in the group zone of the clipboard
            break;
         case Brw_FILE_BRW_ASSIGNMENTS_CRS:
         case Brw_FILE_BRW_WORKS_CRS:
            if (Gbl.FileBrowser.Clipboard.WorksUsrCod != Gbl.Usrs.Other.UsrDat.UsrCod)
               IsThisTree = false;	// We are not in the student's works of the clipboard
            break;
         default:
            break;
        }
     }
   return IsThisTree;
  }

/*****************************************************************************/
/***************************** Add path to clipboards ************************/
/*****************************************************************************/

static void Brw_AddPathToClipboards (Brw_FileType_t FileType,const char *Path)
  {
   char Query[512+PATH_MAX];

   /***** Add path to clipboards *****/
   sprintf (Query,"INSERT INTO clipboard (UsrCod,FileBrowser,CrsCod,GrpCod,WorksUsrCod,FileType,Path)"
                  " VALUES ('%ld','%u','%ld','%ld','%ld','%u','%s')",
            Gbl.Usrs.Me.UsrDat.UsrCod,
            (unsigned) Gbl.FileBrowser.Type,
            (Gbl.FileBrowser.Type == Brw_FILE_BRW_BRIEFCASE_USR) ? -1L :
        	                                                   Gbl.CurrentCrs.Crs.CrsCod,
            (Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP ||
             Gbl.FileBrowser.Type == Brw_FILE_BRW_COMMON_GRP ||
             Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_GRP) ? Gbl.CurrentCrs.Grps.GrpCod :
        	                                                     -1L,
            (Gbl.FileBrowser.Type == Brw_FILE_BRW_ASSIGNMENTS_CRS ||
             Gbl.FileBrowser.Type == Brw_FILE_BRW_WORKS_CRS) ? Gbl.Usrs.Other.UsrDat.UsrCod :
        	                                               -1L,
            (unsigned) FileType,
            Path);
   DB_QueryINSERT (Query,"can not add source of copy to clipboard");
  }

/*****************************************************************************/
/************************** Update path in my clipboard **********************/
/*****************************************************************************/

static void Brw_UpdatePathInClipboard (Brw_FileType_t FileType,const char *Path)
  {
   char Query[512+PATH_MAX];

   /***** Update path in my clipboard *****/
   sprintf (Query,"UPDATE clipboard"
                  " SET FileBrowser='%u',CrsCod='%ld',GrpCod='%ld',WorksUsrCod='%ld',FileType='%u',Path='%s'"
                  " WHERE UsrCod='%ld'",
	    (unsigned) Gbl.FileBrowser.Type,
            (Gbl.FileBrowser.Type == Brw_FILE_BRW_BRIEFCASE_USR) ? -1L :
        	                                                   Gbl.CurrentCrs.Crs.CrsCod,
            (Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP ||
             Gbl.FileBrowser.Type == Brw_FILE_BRW_COMMON_GRP ||
             Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_GRP) ? Gbl.CurrentCrs.Grps.GrpCod :
        	                                                     -1L,
            (Gbl.FileBrowser.Type == Brw_FILE_BRW_ASSIGNMENTS_CRS ||
             Gbl.FileBrowser.Type == Brw_FILE_BRW_WORKS_CRS) ? Gbl.Usrs.Other.UsrDat.UsrCod :
        	                                               -1L,
            (unsigned) FileType,
            Path,
            Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update source of copy in clipboard");
  }

/*****************************************************************************/
/***** Insert folders until specified folder to table of expanded folders ****/
/***** and update click time of the other folders in the expl. tree       ****/
/*****************************************************************************/
// Important: parameter Path must end in a folder, not in a file

static void Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (const char *Path)
  {
   char *Ptr;
   char CopyOfPath[PATH_MAX+1];
   /* Example:
      Path = root_folder/folder1/folder2/folder3
      1. Try to insert CopyOfPath = "root_folder/folder1/folder2/folder3"
      2. Try to insert CopyOfPath = "root_folder/folder1/folder2"
      3. Try to insert CopyOfPath = "root_folder/folder1"
      Only insert paths with '/', so "root_folder" is not inserted
   */

   // if (strcmp (Path,Brw_RootFolderInternalNames[Gbl.FileBrowser.Type]))	// Don't insert root folder

   /***** Make a copy to keep Path unchanged *****/
   strcpy (CopyOfPath,Path);

   /***** Insert paths in table of expanded folders if they are not yet there *****/
   do
     {
      if ((Ptr = strrchr (CopyOfPath,'/')))	// If '/' found (backwards from the end)
	{
	 if (!Brw_GetIfExpandedTree (CopyOfPath))
	    Brw_InsertFolderInExpandedFolders (CopyOfPath);
	 // Now Ptr points to the last '/' in SubPath
	 *Ptr = '\0';	// Substitute '/' for '\0' to shorten CopyOfPath
	}
     }
   while (Ptr);

   /***** Update paths of the current file browser in table of expanded folders *****/
   Brw_UpdateClickTimeOfThisFileBrowserInExpandedFolders ();
  }

/*****************************************************************************/
/******* Remove specified folder from table of expanded folders       ********/
/******* and update click time of the other folders in the expl. tree ********/
/*****************************************************************************/

static void Brw_RemThisFolderAndUpdOtherFoldersFromExpandedFolders (const char *Path)
  {
   /***** Remove Path from expanded folders table *****/
   Brw_RemoveFolderFromExpandedFolders (Path);

   /***** Update paths of the current file browser in table of expanded folders *****/
   Brw_UpdateClickTimeOfThisFileBrowserInExpandedFolders ();
  }

/*****************************************************************************/
/************************* Insert path in expanded folders *******************/
/*****************************************************************************/

static void Brw_InsertFolderInExpandedFolders (const char *Path)
  {
   char Query[512+PATH_MAX];

   /***** Add path to expanded folders *****/
   // Path must be stored with final '/'
   sprintf (Query,"INSERT INTO expanded_folders (UsrCod,FileBrowser,"
	          "CrsCod,GrpCod,WorksUsrCod,Path,ClickTime)"
                  " VALUES ('%ld','%u','%ld','%ld','%ld','%s/',NOW())",
            Gbl.Usrs.Me.UsrDat.UsrCod,
            (unsigned) Brw_FileBrowserForExpandedFolders[Gbl.FileBrowser.Type],
            (Gbl.FileBrowser.Type == Brw_FILE_BRW_BRIEFCASE_USR) ? -1L :
        	                                                   Gbl.CurrentCrs.Crs.CrsCod,
            (Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_DOCUMENTS_GRP   ||
             Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP ||
             Gbl.FileBrowser.Type == Brw_FILE_BRW_COMMON_GRP          ||
             Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_MARKS_GRP       ||
             Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_GRP) ? Gbl.CurrentCrs.Grps.GrpCod :
        	                                                     -1L,
            (Gbl.FileBrowser.Type == Brw_FILE_BRW_ASSIGNMENTS_CRS ||
             Gbl.FileBrowser.Type == Brw_FILE_BRW_WORKS_CRS) ? Gbl.Usrs.Other.UsrDat.UsrCod :
        	                                               -1L,
            Path);
   DB_QueryINSERT (Query,"can not expand the content of a folder");
  }

/*****************************************************************************/
/******* Update paths of the current file browser in expanded folders ********/
/*****************************************************************************/

static void Brw_UpdateClickTimeOfThisFileBrowserInExpandedFolders (void)
  {
   char Query[512+PATH_MAX];
   Brw_FileBrowser_t FileBrowserForExpandedFolders = Brw_FileBrowserForExpandedFolders[Gbl.FileBrowser.Type];

   /***** Update path in table of expanded folders *****/
   switch (FileBrowserForExpandedFolders)
     {
      case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
      case Brw_FILE_BRW_COMMON_CRS:
      case Brw_FILE_BRW_SEE_MARKS_CRS:
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_WORKS_USR:
         sprintf (Query,"UPDATE expanded_folders SET ClickTime=NOW()"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'"
                        " AND CrsCod='%ld'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  (unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_SEE_MARKS_GRP:
         sprintf (Query,"UPDATE expanded_folders SET ClickTime=NOW()"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'"
                        " AND CrsCod='%ld' AND GrpCod='%ld'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  (unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,
                  Gbl.CurrentCrs.Grps.GrpCod);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         sprintf (Query,"UPDATE expanded_folders SET ClickTime=NOW()"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'"
                        " AND CrsCod='%ld' AND WorksUsrCod='%ld'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  (unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,
                  Gbl.Usrs.Other.UsrDat.UsrCod);
         break;
      case Brw_FILE_BRW_BRIEFCASE_USR:
         sprintf (Query,"UPDATE expanded_folders SET ClickTime=NOW()"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  (unsigned) FileBrowserForExpandedFolders);
         break;
      default:
         break;
     }
   DB_QueryUPDATE (Query,"can not update expanded folder");
  }

/*****************************************************************************/
/********************** Remove path from expanded folders ********************/
/*****************************************************************************/

static void Brw_RemoveFolderFromExpandedFolders (const char *Path)
  {
   char Query[512+PATH_MAX];
   Brw_FileBrowser_t FileBrowserForExpandedFolders = Brw_FileBrowserForExpandedFolders[Gbl.FileBrowser.Type];

   /***** Remove expanded folders associated to a file browser from a course or from a user *****/
   switch (FileBrowserForExpandedFolders)
     {
      case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
      case Brw_FILE_BRW_COMMON_CRS:
      case Brw_FILE_BRW_SEE_MARKS_CRS:
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_WORKS_USR:
         sprintf (Query,"DELETE FROM expanded_folders"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'"
                        " AND CrsCod='%ld' AND Path='%s/'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  (unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,Path);
         break;
      case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_SEE_MARKS_GRP:
         sprintf (Query,"DELETE FROM expanded_folders"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'"
                        " AND CrsCod='%ld' AND GrpCod='%ld' AND Path='%s/'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  (unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,Gbl.CurrentCrs.Grps.GrpCod,Path);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         sprintf (Query,"DELETE FROM expanded_folders"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'"
                        " AND CrsCod='%ld' AND WorksUsrCod='%ld' AND Path='%s/'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,(unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Other.UsrDat.UsrCod,Path);
         break;
      case Brw_FILE_BRW_BRIEFCASE_USR:
         sprintf (Query,"DELETE FROM expanded_folders"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u' AND Path='%s/'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,(unsigned) FileBrowserForExpandedFolders,
                  Path);
         break;
      default:
         break;
     }
   DB_QueryDELETE (Query,"can not contract the content of a folder");
  }

/*****************************************************************************/
/************************* Remove clipboard of a user ************************/
/*****************************************************************************/

void Brw_RemoveUsrClipboard (long UsrCod)
  {
   char Query[512];

   /***** Remove clipboard of specified user *****/
   sprintf (Query,"DELETE FROM clipboard WHERE UsrCod='%ld'",UsrCod);
   DB_QueryDELETE (Query,"can not remove source of copy for a user");
  }

/*****************************************************************************/
/************************ Remove clipboards of a group ***********************/
/*****************************************************************************/

void Brw_RemoveGrpClipboards (long GrpCod)
  {
   char Query[512];

   /***** Remove clipboard of specified user *****/
   sprintf (Query,"DELETE FROM clipboard WHERE GrpCod='%ld'",GrpCod);
   DB_QueryDELETE (Query,"can not remove source of copy for a group");
  }

/*****************************************************************************/
/****************** Remove clipboards of a user and a coruse *****************/
/*****************************************************************************/

void Brw_RemoveUsrClipboardInCrs (long UsrCod,long CrsCod)
  {
   char Query[512];

   /***** Remove clipboard of specified user *****/
   sprintf (Query,"DELETE FROM clipboard WHERE UsrCod='%ld' AND CrsCod='%ld'",
            UsrCod,CrsCod);
   DB_QueryDELETE (Query,"can not remove source of copy for a user and a course");
  }

/*****************************************************************************/
/******************* Remove all expanded folders of a user *******************/
/*****************************************************************************/

void Brw_RemoveUsrExpandedFolders (long UsrCod)
  {
   char Query[512];

   /***** Remove expanded folders of specified user *****/
   sprintf (Query,"DELETE FROM expanded_folders WHERE UsrCod='%ld'",UsrCod);
   DB_QueryDELETE (Query,"can not remove expanded folders for a user");
  }

/*****************************************************************************/
/********** Remove all expanded folders of a user in this course *************/
/*****************************************************************************/

void Brw_RemoveUsrExpandedFoldersInCrs (long UsrCod,long CrsCod)
  {
   char Query[512];

   /***** Remove expanded folders of specified user *****/
   sprintf (Query,"DELETE FROM expanded_folders"
                  " WHERE UsrCod='%ld' AND CrsCod='%ld'",
            UsrCod,CrsCod);
   DB_QueryDELETE (Query,"can not remove expanded folders for a user in a course");
  }

/*****************************************************************************/
/****************** Remove expired clipboards (from all users) ***************/
/*****************************************************************************/

static void Brw_RemoveExpiredClipboards (void)
  {
   char Query[512];

   /***** Remove all expired clipboards *****/
   sprintf (Query,"DELETE LOW_PRIORITY FROM clipboard"
                  " WHERE UNIX_TIMESTAMP() > UNIX_TIMESTAMP(CopyTime)+%ld",
            Cfg_TIME_TO_DELETE_BROWSER_CLIPBOARD);
   DB_QueryDELETE (Query,"can not remove old paths from clipboard");
  }

/*****************************************************************************/
/************* Remove expired expanded folders (from all users) **************/
/*****************************************************************************/

void Brw_RemoveExpiredExpandedFolders (void)
  {
   char Query[512];

   /***** Remove all expired clipboards *****/
   sprintf (Query,"DELETE LOW_PRIORITY FROM expanded_folders"
                  " WHERE UNIX_TIMESTAMP() > UNIX_TIMESTAMP(ClickTime)+%ld",
            Cfg_TIME_TO_DELETE_BROWSER_EXPANDED_FOLDERS);
   DB_QueryDELETE (Query,"can not remove old expanded folders");
  }

/*****************************************************************************/
/********* Remove clipboards with paths from a course or from a user *********/
/*****************************************************************************/

static void Brw_RemoveAffectedClipboards (Brw_FileBrowser_t FileBrowser,long MyUsrCod,long WorksUsrCod)
  {
   char Query[512];

   /***** Remove clipboards associated to a file browser
          from a course or from a user *****/
   switch (FileBrowser)
     {
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
      case Brw_FILE_BRW_COMMON_CRS:
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
         sprintf (Query,"DELETE FROM clipboard"
                        " WHERE FileBrowser='%u' AND CrsCod='%ld'",
                  (unsigned) FileBrowser,Gbl.CurrentCrs.Crs.CrsCod);
         break;
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         sprintf (Query,"DELETE FROM clipboard"
                        " WHERE FileBrowser='%u' AND CrsCod='%ld' AND GrpCod='%ld'",
                  (unsigned) FileBrowser,Gbl.CurrentCrs.Crs.CrsCod,Gbl.CurrentCrs.Grps.GrpCod);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_WORKS_USR:
         sprintf (Query,"DELETE FROM clipboard"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u' AND CrsCod='%ld'",
                  MyUsrCod,(unsigned) FileBrowser,Gbl.CurrentCrs.Crs.CrsCod);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         sprintf (Query,"DELETE FROM clipboard"
                        " WHERE FileBrowser='%u' AND CrsCod='%ld' AND WorksUsrCod='%ld'",
                  (unsigned) FileBrowser,Gbl.CurrentCrs.Crs.CrsCod,WorksUsrCod);
         break;
      case Brw_FILE_BRW_BRIEFCASE_USR:
         sprintf (Query,"DELETE FROM clipboard"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'",
                  MyUsrCod,(unsigned) FileBrowser);
         break;
      default:
         break;
     }
   DB_QueryDELETE (Query,"can not remove source of copy");
  }

/*****************************************************************************/
/***** Remove expanded folders with paths from a course or from a user *******/
/*****************************************************************************/

static void Brw_RemoveAffectedExpandedFolders (const char *Path)
  {
   char Query[512+PATH_MAX];
   Brw_FileBrowser_t FileBrowserForExpandedFolders = Brw_FileBrowserForExpandedFolders[Gbl.FileBrowser.Type];

   /***** Remove expanded folders associated to a file browser from a course or from a user *****/
   switch (FileBrowserForExpandedFolders)
     {
      case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
      case Brw_FILE_BRW_COMMON_CRS:
      case Brw_FILE_BRW_SEE_MARKS_CRS:
         sprintf (Query,"DELETE FROM expanded_folders"
                        " WHERE FileBrowser='%u'"
                        " AND CrsCod='%ld' AND Path LIKE '%s/%%'",
                  (unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,Path);
         break;
      case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_SEE_MARKS_GRP:
         sprintf (Query,"DELETE FROM expanded_folders"
                        " WHERE FileBrowser='%u'"
                        " AND CrsCod='%ld' AND GrpCod='%ld' AND Path LIKE '%s/%%'",
                  (unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,Gbl.CurrentCrs.Grps.GrpCod,Path);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_WORKS_USR:
         sprintf (Query,"DELETE FROM expanded_folders"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'"
                        " AND CrsCod='%ld' AND Path LIKE '%s/%%'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  (unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,Path);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         sprintf (Query,"DELETE FROM expanded_folders"
                        " WHERE FileBrowser='%u'"
                        " AND CrsCod='%ld' AND WorksUsrCod='%ld' AND Path LIKE '%s/%%'",
                  (unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Other.UsrDat.UsrCod,Path);
         break;
      case Brw_FILE_BRW_BRIEFCASE_USR:
         sprintf (Query,"DELETE FROM expanded_folders"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'"
                        " AND Path LIKE '%s/%%'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  (unsigned) FileBrowserForExpandedFolders,
                  Path);
         break;
      default:
         break;
     }
   DB_QueryDELETE (Query,"can not remove expanded folders");
  }

/*****************************************************************************/
/***** Remove expanded folders with paths from a course or from a user *******/
/*****************************************************************************/

static void Brw_RenameAffectedExpandedFolders (Brw_FileBrowser_t FileBrowser,long MyUsrCod,long WorksUsrCod,const char *OldPath,const char *NewPath)
  {
   char Query[512+PATH_MAX*2];
   Brw_FileBrowser_t FileBrowserForExpandedFolders = Brw_FileBrowserForExpandedFolders[FileBrowser];
   unsigned StartFinalSubpathNotChanged = strlen (OldPath) + 2;

   /***** Update expanded folders associated to a file browser from a course or from a user *****/
   switch (FileBrowserForExpandedFolders)
     {
      case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
      case Brw_FILE_BRW_COMMON_CRS:
      case Brw_FILE_BRW_SEE_MARKS_CRS:
         sprintf (Query,"UPDATE expanded_folders SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
                        " WHERE FileBrowser='%u'"
                        " AND CrsCod='%ld' AND Path LIKE '%s/%%'",
                  NewPath,StartFinalSubpathNotChanged,
                  (unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,OldPath);
         break;
      case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_SEE_MARKS_GRP:
         sprintf (Query,"UPDATE expanded_folders SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
                        " WHERE FileBrowser='%u'"
                        " AND CrsCod='%ld' AND GrpCod='%ld' AND Path LIKE '%s/%%'",
                  NewPath,StartFinalSubpathNotChanged,
                  (unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,Gbl.CurrentCrs.Grps.GrpCod,OldPath);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_WORKS_USR:
         sprintf (Query,"UPDATE expanded_folders SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'"
                        " AND CrsCod='%ld' AND Path LIKE '%s/%%'",
                  NewPath,StartFinalSubpathNotChanged,
                  MyUsrCod,(unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,OldPath);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         sprintf (Query,"UPDATE expanded_folders SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
                        " WHERE FileBrowser='%u'"
                        " AND CrsCod='%ld' AND WorksUsrCod='%ld' AND Path LIKE '%s/%%'",
                  NewPath,StartFinalSubpathNotChanged,
                  (unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,WorksUsrCod,OldPath);
         break;
      case Brw_FILE_BRW_BRIEFCASE_USR:
         sprintf (Query,"UPDATE expanded_folders SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'"
                        " AND Path LIKE '%s/%%'",
                  NewPath,StartFinalSubpathNotChanged,MyUsrCod,
                  (unsigned) FileBrowserForExpandedFolders,
                  OldPath);
         break;
      default:
         break;
     }
   DB_QueryUPDATE (Query,"can not remove expanded folders");
  }

/*****************************************************************************/
/**** Paste the arch/carp indicado in the portapapelesde in file browser *****/
/*****************************************************************************/

void Brw_PasteIntoFileBrowser (void)
  {
   extern const char *Txt_Nothing_has_been_pasted_because_the_clipboard_is_empty_;
   struct GroupData GrpDat;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   if (Brw_GetMyClipboard ())
     {
      if (Gbl.FileBrowser.Clipboard.GrpCod > 0)	// Clipboard in a group zone
        {
         GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.GrpCod;
	 Brw_GetSelectedGroupData (&GrpDat,true);
        }

      /***** Write the origin of the copy *****/
      Brw_WriteCurrentClipboard ();

      /***** Copy files recursively *****/
      Brw_PasteClipboard ();

      /***** Remove the affected clipboards *****/
      Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Other.UsrDat.UsrCod);
     }
   else
      /***** Write message ******/
      Lay_ShowAlert (Lay_WARNING,Txt_Nothing_has_been_pasted_because_the_clipboard_is_empty_);

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/****** Paste all the content of the clipboard in the current location *******/
/*****************************************************************************/
// Source:
//	Type of file browser:		Gbl.FileBrowser.Clipboard.FileBrowser
//	Possible course:		Gbl.FileBrowser.Clipboard.CrsCod
//	Possible student in works:	Gbl.FileBrowser.Clipboard.WorksUsrCod
//	Path (file or folder):		Gbl.FileBrowser.Clipboard.Path
// Destination:
//	Type of file browser:		Gbl.FileBrowser.Type
//	Possible course:		Gbl.CurrentCrs.Crs.CrsCod
//	Possible student in works:	Gbl.Usrs.Other.UsrDat.UsrCod
//	Path (should be a folder):	Gbl.FileBrowser.Priv.FullPathInTree
// Returns the number of files pasted

static void Brw_PasteClipboard (void)
  {
   extern const char *Txt_The_copy_has_been_successful;
   extern const char *Txt_Files_copied;
   extern const char *Txt_Folders_copied;
   extern const char *Txt_Links_copied;
   extern const char *Txt_You_can_not_paste_file_or_folder_here;
   struct Course Crs;
   char PathCrsOrg[PATH_MAX+1];
   char PathOrg[PATH_MAX+1];
   unsigned NumFilesPasted = 0;
   unsigned NumFoldsPasted = 0;
   unsigned NumLinksPasted = 0;
   long FirstFilCod = -1L;	// First file code of the first file or link pasted. Important: initialize here to -1L
   struct FileMetadata FileMetadata;
   unsigned NumUsrsToBeNotifiedByEMail;

   Gbl.FileBrowser.Clipboard.IsThisTree = Brw_CheckIfClipboardIsInThisTree ();
   if (Brw_CheckIfCanPasteIn (Gbl.FileBrowser.Level))
     {
      /***** Construct the relative path of the origin file or folder *****/
      switch (Gbl.FileBrowser.Clipboard.FileBrowser)
        {
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         case Brw_FILE_BRW_COMMON_CRS:
         case Brw_FILE_BRW_COMMON_GRP:
         case Brw_FILE_BRW_ADMIN_MARKS_CRS:
         case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         case Brw_FILE_BRW_ASSIGNMENTS_USR:
         case Brw_FILE_BRW_WORKS_USR:
         case Brw_FILE_BRW_ASSIGNMENTS_CRS:
         case Brw_FILE_BRW_WORKS_CRS:
            Crs.CrsCod = Gbl.FileBrowser.Clipboard.CrsCod;
            if (Crs_GetDataOfCourseByCod (&Crs))
               sprintf (PathCrsOrg,"%s/%s/%ld",
                        Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_CRS,Crs.CrsCod);
            else
               Lay_ShowErrorAndExit ("The course of copy source does not exist.");
            break;
         default:
            break;
        }
      switch (Gbl.FileBrowser.Clipboard.FileBrowser)
        {
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
         case Brw_FILE_BRW_COMMON_CRS:
         case Brw_FILE_BRW_ADMIN_MARKS_CRS:
            sprintf (PathOrg,"%s/%s",
        	     PathCrsOrg,
        	     Gbl.FileBrowser.Clipboard.Path);
            break;
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         case Brw_FILE_BRW_COMMON_GRP:
         case Brw_FILE_BRW_ADMIN_MARKS_GRP:
            sprintf (PathOrg,"%s/grp/%ld/%s",
        	     PathCrsOrg,
        	     Gbl.FileBrowser.Clipboard.GrpCod,
        	     Gbl.FileBrowser.Clipboard.Path);
            break;
         case Brw_FILE_BRW_ASSIGNMENTS_USR:
         case Brw_FILE_BRW_WORKS_USR:
            sprintf (PathOrg,"%s/usr/%02u/%ld/%s",
        	     PathCrsOrg,
        	     (unsigned) (Gbl.Usrs.Me.UsrDat.UsrCod % 100),
        	     Gbl.Usrs.Me.UsrDat.UsrCod,
        	     Gbl.FileBrowser.Clipboard.Path);
            break;
         case Brw_FILE_BRW_ASSIGNMENTS_CRS:
         case Brw_FILE_BRW_WORKS_CRS:
            sprintf (PathOrg,"%s/usr/%02u/%ld/%s",
        	     PathCrsOrg,
        	     (unsigned) (Gbl.FileBrowser.Clipboard.WorksUsrCod % 100),
        	     Gbl.FileBrowser.Clipboard.WorksUsrCod,
        	     Gbl.FileBrowser.Clipboard.Path);
            break;
         case Brw_FILE_BRW_BRIEFCASE_USR:
            sprintf (PathOrg,"%s/%s",
        	     Gbl.Usrs.Me.PathDir,
        	     Gbl.FileBrowser.Clipboard.Path);
            break;
         default:
            break;
        }

      /***** Paste tree (path in clipboard) into folder *****/
      Brw_CalcSizeOfDir (Gbl.FileBrowser.Priv.PathRootFolder);
      Brw_SetMaxQuota ();
      if (Brw_PasteTreeIntoFolder (PathOrg,Gbl.FileBrowser.Priv.FullPathInTree,
	                           &NumFilesPasted,&NumFoldsPasted,&NumLinksPasted,
	                           &FirstFilCod))
        {
         /***** Write message of success *****/
         sprintf (Gbl.Message,"%s<br />"
                              "%s: %u<br />"
                              "%s: %u<br />"
                              "%s: %u",
                  Txt_The_copy_has_been_successful,
                  Txt_Files_copied  ,NumFilesPasted,
                  Txt_Folders_copied,NumFoldsPasted,
                  Txt_Links_copied  ,NumLinksPasted);
         Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

         /***** Notify new files *****/
	 if (NumFilesPasted ||
	     NumLinksPasted)
	   {
	    FileMetadata.FilCod = FirstFilCod;
            Brw_GetFileMetadataByCod (&FileMetadata);

	    /* Notify only is destination folder is visible */
	    if (!Brw_CheckIfFileOrFolderIsHidden (&FileMetadata))
	       switch (Gbl.FileBrowser.Type)
		 {
		  case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
		  case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
		     NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_DOCUMENT_FILE,FirstFilCod);
		     Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);
		     break;
		  case Brw_FILE_BRW_COMMON_CRS:
		  case Brw_FILE_BRW_COMMON_GRP:
		     NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_SHARED_FILE,FirstFilCod);
		     Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);
		     break;
		  case Brw_FILE_BRW_ADMIN_MARKS_CRS:
		  case Brw_FILE_BRW_ADMIN_MARKS_GRP:
		     NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_MARKS_FILE,FirstFilCod);
		     Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);
		     break;
		  default:
		     break;
		 }
	   }
        }

      /***** Add path where new tree is pasted to table of expanded folders *****/
      Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.Priv.FullPathInTree);
     }
   else
      Lay_ShowErrorAndExit (Txt_You_can_not_paste_file_or_folder_here);	// It's difficult, but not impossible that a user sees this message
  }

/*****************************************************************************/
/****************** Compute number of levels in a path ***********************/
/*****************************************************************************/

static unsigned Brw_NumLevelsInPath (const char *Path)
  {
   unsigned NumLevls = 0;

   while (*Path)
      if (*Path++ == '/')
         NumLevls++;

   return NumLevls;
  }

/*****************************************************************************/
/*********** Copy a source file o tree in the destination folder *************/
/*****************************************************************************/
// Return true if the copy has been made successfully, and false if not

static bool Brw_PasteTreeIntoFolder (const char *PathOrg,const char *PathDstInTree,
                                     unsigned *NumFilesPasted,
                                     unsigned *NumFoldsPasted,
                                     unsigned *NumLinksPasted,
                                     long *FirstFilCod)
  {
   extern const char *Txt_The_copy_has_stopped_when_trying_to_paste_the_file_X_because_it_would_exceed_the_disk_quota;
   extern const char *Txt_The_copy_has_stopped_when_trying_to_paste_the_folder_X_because_it_would_exceed_the_disk_quota;
   extern const char *Txt_The_copy_has_stopped_when_trying_to_paste_the_link_X_because_it_would_exceed_the_disk_quota;

   extern const char *Txt_The_copy_has_stopped_when_trying_to_paste_the_file_X_because_it_would_exceed_the_maximum_allowed_number_of_levels;
   extern const char *Txt_The_copy_has_stopped_when_trying_to_paste_the_folder_X_because_it_would_exceed_the_maximum_allowed_number_of_levels;
   extern const char *Txt_The_copy_has_stopped_when_trying_to_paste_the_link_X_because_it_would_exceed_the_maximum_allowed_number_of_levels;

   extern const char *Txt_The_copy_has_stopped_when_trying_to_paste_the_file_X_because_there_is_already_an_object_with_that_name;
   extern const char *Txt_The_copy_has_stopped_when_trying_to_paste_the_link_X_because_there_is_already_an_object_with_that_name;

   extern const char *Txt_The_copy_has_stopped_when_trying_to_paste_the_file_X_because_you_can_not_paste_a_file_here_of_a_type_other_than_HTML;
   Brw_FileType_t FileType;
   char PathUntilFileNameOrg[PATH_MAX+1];
   char FileNameOrg[NAME_MAX+1];
   char FileNameToShow[NAME_MAX+1];
   char PathInFolderOrg[PATH_MAX+1];
   char PathDstInTreeWithFile[PATH_MAX+1];
   char PathDstWithFile[PATH_MAX+1];
   struct stat FileStatus;
   struct dirent **FileList;
   int NumFileInThisDir;
   int NumFilesInThisDir;
   unsigned NumLevls;
   long FilCod;	// File code of the file pasted

   /***** Get the name (only the name) of the origin file or folder *****/
   Str_SplitFullPathIntoPathAndFileName (PathOrg,
	                                 PathUntilFileNameOrg,
	                                 FileNameOrg);

   /***** Is it a file or a folder? *****/
   lstat (PathOrg,&FileStatus);
   if (S_ISDIR (FileStatus.st_mode))		// It's a directory
      FileType = Brw_IS_FOLDER;
   else if (S_ISREG (FileStatus.st_mode))	// It's a regular file
      FileType = Str_FileIs (FileNameOrg,"url") ? Brw_IS_LINK :	// It's a link (URL inside a .url file)
	                                          Brw_IS_FILE;	// It's a file
   else
      FileType = Brw_IS_UNKNOWN;

   /***** Name of the file/folder/link to be shown ****/
   Brw_LimitLengthFileNameToShow (FileType,FileNameOrg,FileNameToShow);

   /***** Construct the name of the destination file or folder *****/
   sprintf (PathDstInTreeWithFile,"%s/%s",PathDstInTree,FileNameOrg);

   /***** Construct the relative path of the destination file or folder *****/
   sprintf (PathDstWithFile,"%s/%s",Gbl.FileBrowser.Priv.PathAboveRootFolder,PathDstInTreeWithFile);

   /***** Update and check number of levels *****/
   // The number of levels is counted starting on the root folder raíz, not included.
   // Example:	If PathDstInTreeWithFile is "maletin/1/2/3/4/FileNameOrg", then NumLevls=5
   if ((NumLevls = Brw_NumLevelsInPath (PathDstInTreeWithFile)) > Gbl.FileBrowser.Size.NumLevls)
      Gbl.FileBrowser.Size.NumLevls = NumLevls;
   if (Brw_CheckIfQuotaExceded ())
     {
      switch (FileType)
        {
	 case Brw_IS_FILE:
	    sprintf (Gbl.Message,Txt_The_copy_has_stopped_when_trying_to_paste_the_file_X_because_it_would_exceed_the_maximum_allowed_number_of_levels,
		     FileNameToShow);
	    break;
	 case Brw_IS_FOLDER:
	    sprintf (Gbl.Message,Txt_The_copy_has_stopped_when_trying_to_paste_the_folder_X_because_it_would_exceed_the_maximum_allowed_number_of_levels,
		     FileNameToShow);
	    break;
	 case Brw_IS_LINK:
	    sprintf (Gbl.Message,Txt_The_copy_has_stopped_when_trying_to_paste_the_link_X_because_it_would_exceed_the_maximum_allowed_number_of_levels,
		     FileNameToShow);
	    break;
	 default:
            Lay_ShowErrorAndExit ("Can not paste unknown file type.");
        }
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
      return false;
     }

   /***** Copy file or folder *****/
   if (FileType == Brw_IS_FILE ||
       FileType == Brw_IS_LINK)	// It's a regular file
     {
      /***** Check if exists the destination file */
      if (Fil_CheckIfPathExists (PathDstWithFile))
        {
         sprintf (Gbl.Message,
                  FileType == Brw_IS_FILE ? Txt_The_copy_has_stopped_when_trying_to_paste_the_file_X_because_there_is_already_an_object_with_that_name :
                	                    Txt_The_copy_has_stopped_when_trying_to_paste_the_link_X_because_there_is_already_an_object_with_that_name,
                  FileNameToShow);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
         return false;
        }

      /***** If the target file browser is that of marks, only HTML files are allowed *****/
      if (Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_CRS ||
          Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_GRP)
         /* Check extension of the file */
         if (!Str_FileIsHTML (FileNameOrg))
           {
            sprintf (Gbl.Message,Txt_The_copy_has_stopped_when_trying_to_paste_the_file_X_because_you_can_not_paste_a_file_here_of_a_type_other_than_HTML,
                     FileNameToShow);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
            return false;
           }

      /***** Update and check the quota before copying the file *****/
      Gbl.FileBrowser.Size.NumFiles++;
      Gbl.FileBrowser.Size.TotalSiz += (unsigned long long) FileStatus.st_size;
      if (Brw_CheckIfQuotaExceded ())
        {
         sprintf (Gbl.Message,FileType == Brw_IS_FILE ? Txt_The_copy_has_stopped_when_trying_to_paste_the_file_X_because_it_would_exceed_the_disk_quota :
                                                        Txt_The_copy_has_stopped_when_trying_to_paste_the_link_X_because_it_would_exceed_the_disk_quota,
                  FileNameToShow);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
         return false;
        }

      /***** Quota will not be exceeded ==> copy the origin file to the destination file *****/
      Fil_FastCopyOfFiles (PathOrg,PathDstWithFile);

      /***** Add entry to the table of files/folders *****/
      FilCod = Brw_AddPathToDB (Gbl.Usrs.Me.UsrDat.UsrCod,FileType,
                                PathDstInTreeWithFile,false,Brw_LICENSE_DEFAULT);
      if (*FirstFilCod <= 0)
	 *FirstFilCod = FilCod;

      if (FileType == Brw_IS_FILE)
	 (*NumFilesPasted)++;
      else // FileType == Brw_IS_LINK
	 (*NumLinksPasted)++;
     }
   else if (FileType == Brw_IS_FOLDER)	// It's a directory
     {
      /***** Scan the source directory *****/
      NumFilesInThisDir = scandir (PathOrg,&FileList,NULL,alphasort);

      /***** Create the folder in the destination *****/
      if (!Fil_CheckIfPathExists (PathDstWithFile))	// If already exists, don't overwrite
        {
	 /* The directory does not exist ==> create it.
            First, update and check the quota */
	 Gbl.FileBrowser.Size.NumFolds++;
	 Gbl.FileBrowser.Size.TotalSiz += (unsigned long long) FileStatus.st_size;
         if (Brw_CheckIfQuotaExceded ())
           {
            sprintf (Gbl.Message,Txt_The_copy_has_stopped_when_trying_to_paste_the_folder_X_because_it_would_exceed_the_disk_quota,
                     FileNameToShow);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
            return false;
           }
	 /* The quota will not be exceded ==> create the directory */
         if (mkdir (PathDstWithFile,(mode_t) 0xFFF) != 0)
            Lay_ShowErrorAndExit ("Can not create folder.");

         /* Add entry to the table of files/folders */
         Brw_AddPathToDB (Gbl.Usrs.Me.UsrDat.UsrCod,FileType,
                          PathDstInTreeWithFile,false,Brw_LICENSE_DEFAULT);
        }

      /***** Copy each of the files and folders from the origin to the destination *****/
      for (NumFileInThisDir = 0;
	   NumFileInThisDir < NumFilesInThisDir;
	   NumFileInThisDir++)
         if (strcmp (FileList[NumFileInThisDir]->d_name,".") &&
             strcmp (FileList[NumFileInThisDir]->d_name,".."))	// Skip directories "." and ".."
	   {
	    sprintf (PathInFolderOrg,"%s/%s",PathOrg,FileList[NumFileInThisDir]->d_name);
            if (!Brw_PasteTreeIntoFolder (PathInFolderOrg,PathDstInTreeWithFile,
        	                          NumFilesPasted,NumFoldsPasted,NumLinksPasted,
        	                          FirstFilCod))
               return false;
	   }

      (*NumFoldsPasted)++;
     }
   return true;
  }

/*****************************************************************************/
/************** Form to add a file or folder to a file browser ***************/
/*****************************************************************************/

void Brw_ShowFormFileBrowser (void)
  {
   extern const char *Txt_You_can_not_create_folders_files_or_links_here;
   char FileNameToShow[NAME_MAX+1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Check if creating a new folder or file is allowed *****/
   if (Brw_CheckIfICanCreateIntoFolder (Gbl.FileBrowser.Level))
     {
      /***** Name of the folder to be shown ****/
      Brw_GetFileNameToShow (Gbl.FileBrowser.Type,Gbl.FileBrowser.Level,Gbl.FileBrowser.FileType,
                             Gbl.FileBrowser.FilFolLnkName,FileNameToShow);

      /***** 1. Form to create a new folder *****/
      Brw_PutFormToCreateAFolder (FileNameToShow);

      /***** 2. Form to send a file *****/
      Brw_PutFormToUploadFilesUsingDropzone (FileNameToShow);

      /***** 3. Form to send a file *****/
      Brw_PutFormToUploadOneFileClassic (FileNameToShow);

      /***** 4. Form to paste the content of the clipboard *****/
      if (Brw_GetMyClipboard ())
        {
         /***** Check if we can paste in this folder *****/
         Gbl.FileBrowser.Clipboard.IsThisTree = Brw_CheckIfClipboardIsInThisTree ();
         if (Brw_CheckIfCanPasteIn (Gbl.FileBrowser.Level))
            Brw_PutFormToPasteAFileOrFolder (FileNameToShow);
        }

      /***** 5. Form to create a link *****/
      if (Gbl.FileBrowser.Type != Brw_FILE_BRW_ADMIN_MARKS_CRS &&
	  Gbl.FileBrowser.Type != Brw_FILE_BRW_ADMIN_MARKS_GRP)	// Do not create links in marks
         Brw_PutFormToCreateALink (FileNameToShow);
     }
   else
     {
      Lay_ShowErrorAndExit (Txt_You_can_not_create_folders_files_or_links_here);	// It's difficult, but not impossible that a user sees this message

      /***** Show again file browser *****/
      Brw_ShowAgainFileBrowserOrWorks ();
     }
  }

/*****************************************************************************/
/************* Put form to create a new folder in a file browser *************/
/*****************************************************************************/

static void Brw_PutFormToCreateAFolder (const char *FileNameToShow)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Create_folder;
   extern const char *Txt_You_can_create_a_new_folder_inside_the_folder_X;
   extern const char *Txt_Folder;

   /***** Start frame *****/
   Lay_StartRoundFrameTable10 (NULL,0,Txt_Create_folder);
   fprintf (Gbl.F.Out,"<tr>"
		      "<td style=\"text-align:center;\">");
   sprintf (Gbl.Message,Txt_You_can_create_a_new_folder_inside_the_folder_X,
	    FileNameToShow);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);

   Act_FormStart (Brw_ActCreateFolder[Gbl.FileBrowser.Type]);
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         Usr_PutHiddenParUsrCodAll (Brw_ActCreateFolder[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
         Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
         break;
      default:
         break;
     }
   Brw_ParamListFiles (Brw_IS_FOLDER,Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,Gbl.FileBrowser.FilFolLnkName);

   /* Folder */
   fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">"
                      "<tr>"
                      "<td class=\"%s\""
                      " style=\"width:30%%; text-align:right;\">"
                      "%s:"
                      "</td>"
                      "<td style=\"width:70%%; text-align:left;\">"
                      "<input type=\"text\" name=\"NewFolderName\" size=\"32\" maxlength=\"40\" value=\"\" />"
                      "</td>"
                      "</tr>"
                      "</table>",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Folder);

   /* Button to send */
   Lay_PutSendButton (Txt_Create_folder);
   fprintf (Gbl.F.Out,"</form>");

   /***** End frame *****/
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/*** Put form to upload several files to a file browser using Dropzone.js ****/
/*****************************************************************************/

static void Brw_PutFormToUploadFilesUsingDropzone (const char *FileNameToShow)
  {
   extern const char *Txt_Upload_files;
   extern const char *Txt_or_you_can_upload_new_files_to_the_folder_X;
   extern const char *Txt_Select_one_or_more_files_from_your_computer_or_drag_and_drop_here;
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *Txt_FILE_UPLOAD_Done;
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];

   /***** Start frame *****/
   fprintf (Gbl.F.Out,"<div id=\"dropzone-upload\">");
   Lay_StartRoundFrameTable10 ("100%",0,Txt_Upload_files);
   fprintf (Gbl.F.Out,"<tr>"
		      "<td style=\"text-align:center;\">");
   sprintf (Gbl.Message,Txt_or_you_can_upload_new_files_to_the_folder_X,
	    FileNameToShow);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);

   /***** Form to upload files using the library Dropzone.js *****/
   // Use min-height:100px; or other number to stablish the height
   Gbl.NumForm++;
   fprintf (Gbl.F.Out,"<form method=\"post\" action=\"%s/%s\""
                      " class=\"dropzone\""
                      " enctype=\"multipart/form-data\""
                      " id=\"my-awesome-dropzone\""
                      " style=\"display:inline-block; width:100%%;"
                      " background:url('%s/upload320x320.gif') no-repeat center;\">",
            Cfg_HTTPS_URL_SWAD_CGI,
            Txt_STR_LANG_ID[Gbl.Prefs.Language],
            Gbl.Prefs.IconsURL);
   Par_PutHiddenParamLong ("ActCod",Act_Actions[Brw_ActUploadFileDropzone[Gbl.FileBrowser.Type]].ActCod);
   Par_PutHiddenParamString ("IdSes",Gbl.Session.Id);
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         Usr_PutHiddenParUsrCodAll (Brw_ActUploadFileDropzone[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
         Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
         break;
      default:
         break;
     }
   Brw_ParamListFiles (Brw_IS_FOLDER,Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,Gbl.FileBrowser.FilFolLnkName);

   fprintf (Gbl.F.Out,"<div class=\"dz-message\">"
		      "<span class=\"DAT_LIGHT\">%s</div>"
		      "</div>"
                      "</form>",
            Txt_Select_one_or_more_files_from_your_computer_or_drag_and_drop_here);

   /***** Put button to refresh file browser after upload *****/
   Act_FormStart (Brw_ActRefreshAfterUploadFiles[Gbl.FileBrowser.Type]);
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         Usr_PutHiddenParUsrCodAll (Brw_ActRefreshAfterUploadFiles[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
         Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
         break;
      default:
         break;
     }
   if (Gbl.FileBrowser.FullTree)
      Par_PutHiddenParamChar ("FullTree",'Y');

   /* Button to send */
   Lay_PutSendButton (Txt_FILE_UPLOAD_Done);
   fprintf (Gbl.F.Out,"</form>");

   /***** End frame *****/
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");
   Lay_EndRoundFrameTable10 ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/*** Put form to upload several files to a file browser using Dropzone.js ****/
/*****************************************************************************/

static void Brw_PutFormToUploadOneFileClassic (const char *FileNameToShow)
  {
   extern const char *Txt_Upload_file;
   extern const char *Txt_or_you_can_upload_a_new_file_to_the_folder_X;

   /***** Start frame *****/
   fprintf (Gbl.F.Out,"<div id=\"classic-upload\" style='display:none;'>");
   Lay_StartRoundFrameTable10 (NULL,0,Txt_Upload_file);
   fprintf (Gbl.F.Out,"<tr>"
		      "<td style=\"text-align:center;\">");
   sprintf (Gbl.Message,Txt_or_you_can_upload_a_new_file_to_the_folder_X,
	    FileNameToShow);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);

   /***** Form to upload one files using the classic way *****/
   Act_FormStart (Brw_ActUploadFileClassic[Gbl.FileBrowser.Type]);
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         Usr_PutHiddenParUsrCodAll (Brw_ActUploadFileClassic[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
         Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
         break;
      default:
         break;
     }
   Brw_ParamListFiles (Brw_IS_FOLDER,Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,Gbl.FileBrowser.FilFolLnkName);
   fprintf (Gbl.F.Out,"<input type=\"file\" name=\"%s\" size=\"50\" maxlength=\"100\" value=\"\" />",
            Fil_NAME_OF_PARAM_FILENAME_ORG);

   /* Button to send */
   Lay_PutSendButton (Txt_Upload_file);
   fprintf (Gbl.F.Out,"</form>");

   /***** End frame *****/
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");
   Lay_EndRoundFrameTable10 ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********* Put form to paste a file or a folder into a file browser **********/
/*****************************************************************************/

static void Brw_PutFormToPasteAFileOrFolder (const char *FileNameToShow)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Paste;
   extern const char *Txt_or_you_can_make_a_file_copy_to_the_folder_X;

   /***** Start frame *****/
   Lay_StartRoundFrameTable10 (NULL,0,Txt_Paste);
   fprintf (Gbl.F.Out,"<tr>"
		      "<td style=\"text-align:center;\">");
   sprintf (Gbl.Message,Txt_or_you_can_make_a_file_copy_to_the_folder_X,
	    FileNameToShow);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);

   Act_FormStart (Brw_ActPaste[Gbl.FileBrowser.Type]);
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         Usr_PutHiddenParUsrCodAll (Brw_ActPaste[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
         Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
         break;
      default:
         break;
     }
   Brw_ParamListFiles (Brw_IS_FOLDER,Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,Gbl.FileBrowser.FilFolLnkName);

   /* Button to send */
   Lay_PutSendButton (Txt_Paste);
   fprintf (Gbl.F.Out,"</form>");

   /***** End frame *****/
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/************** Put form to create a new link in a file browser **************/
/*****************************************************************************/

static void Brw_PutFormToCreateALink (const char *FileNameToShow)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Create_link;
   extern const char *Txt_or_you_can_create_a_new_link_inside_the_folder_X;
   extern const char *Txt_URL;

   /***** Start frame *****/
   Lay_StartRoundFrameTable10 (NULL,0,Txt_Create_link);
   fprintf (Gbl.F.Out,"<tr>"
		      "<td style=\"text-align:center;\">");
   sprintf (Gbl.Message,Txt_or_you_can_create_a_new_link_inside_the_folder_X,
	    FileNameToShow);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);

   Act_FormStart (Brw_ActCreateLink[Gbl.FileBrowser.Type]);
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         Usr_PutHiddenParUsrCodAll (Brw_ActCreateLink[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
         Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
         break;
      default:
         break;
     }
   Brw_ParamListFiles (Brw_IS_FOLDER,Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,Gbl.FileBrowser.FilFolLnkName);

   /* URL */
   fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">"
                      "<tr>"
                      "<td class=\"%s\""
                      " style=\"width:30%%; text-align:right;\">"
                      "%s:"
                      "</td>"
                      "<td style=\"width:70%%; text-align:left;\">"
                      "<input type=\"text\" name=\"NewLink\" size=\"80\" maxlength=\"%u\" value=\"\" />"
                      "</td>"
                      "</tr>"
                      "</table>",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_URL,PATH_MAX);

   /* Button to send */
   Lay_PutSendButton (Txt_Create_link);
   fprintf (Gbl.F.Out,"</form>");

   /***** End frame *****/
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/*********** Receive the name of a new folder in a file browser **************/
/*****************************************************************************/

void Brw_RecFolderFileBrowser (void)
  {
   extern const char *Txt_Can_not_create_the_folder_X_because_it_would_exceed_the_disk_quota;
   extern const char *Txt_Can_not_create_the_folder_X_because_there_is_already_a_folder_or_a_file_with_that_name;
   extern const char *Txt_The_folder_X_has_been_created_inside_the_folder_Y;
   extern const char *Txt_You_can_not_create_folders_here;
   char Path[PATH_MAX+1];
   char PathCompleteInTreeIncludingFolder[PATH_MAX+1];
   char FileNameToShow[NAME_MAX+1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Check if creating a new folder is allowed *****/
   if (Brw_CheckIfICanCreateIntoFolder (Gbl.FileBrowser.Level))
     {
      if (Str_ConvertFilFolLnkNameToValid (Gbl.FileBrowser.NewFilFolLnkName))
        {
         /* In Gbl.FileBrowser.NewFilFolLnkName is the name of the new folder */
         sprintf (Path,"%s/%s",Gbl.FileBrowser.Priv.PathAboveRootFolder,Gbl.FileBrowser.Priv.FullPathInTree);

         if (strlen (Path) + 1 + strlen (Gbl.FileBrowser.NewFilFolLnkName) > PATH_MAX)
	    Lay_ShowErrorAndExit ("Path is too long.");
         strcat (Path,"/");
         strcat (Path,Gbl.FileBrowser.NewFilFolLnkName);

         /* Create the new directory */
         if (mkdir (Path,(mode_t) 0xFFF) == 0)
	   {
	    /* Check if quota has been exceeded */
	    Brw_CalcSizeOfDir (Gbl.FileBrowser.Priv.PathRootFolder);
	    Brw_SetMaxQuota ();
            if (Brw_CheckIfQuotaExceded ())
	      {
	       Brw_RemoveTree (Path);
               sprintf (Gbl.Message,Txt_Can_not_create_the_folder_X_because_it_would_exceed_the_disk_quota,
                        Gbl.FileBrowser.NewFilFolLnkName);
               Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	      }
	    else
              {
               /* Remove affected clipboards */
               Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Other.UsrDat.UsrCod);

               /* Add path where new file is created to table of expanded folders */
               Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.Priv.FullPathInTree);

               /* Add entry to the table of files/folders */
               sprintf (PathCompleteInTreeIncludingFolder,"%s/%s",Gbl.FileBrowser.Priv.FullPathInTree,Gbl.FileBrowser.NewFilFolLnkName);
               Brw_AddPathToDB (Gbl.Usrs.Me.UsrDat.UsrCod,Brw_IS_FOLDER,
                                PathCompleteInTreeIncludingFolder,false,Brw_LICENSE_DEFAULT);

	       /* The folder has been created sucessfully */
               Brw_GetFileNameToShow (Gbl.FileBrowser.Type,Gbl.FileBrowser.Level,Brw_IS_FOLDER,
                                      Gbl.FileBrowser.FilFolLnkName,FileNameToShow);
	       sprintf (Gbl.Message,Txt_The_folder_X_has_been_created_inside_the_folder_Y,
		        Gbl.FileBrowser.NewFilFolLnkName,FileNameToShow);
               Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
              }
	   }
         else
	   {
	    switch (errno)
	      {
	       case EEXIST:
                  sprintf (Gbl.Message,Txt_Can_not_create_the_folder_X_because_there_is_already_a_folder_or_a_file_with_that_name,
		   	   Gbl.FileBrowser.NewFilFolLnkName);
                  Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	          break;
	       case EACCES:
	          Lay_ShowErrorAndExit ("Write forbidden.");
	          break;
	       default:
	          Lay_ShowErrorAndExit ("Can not create folder.");
	          break;
	      }
	   }
        }
      else	// Folder name not valid
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
   else
      Lay_ShowErrorAndExit (Txt_You_can_not_create_folders_here);	// It's difficult, but not impossible that a user sees this message

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/**************** Rename an existing folder in a file browser ****************/
/*****************************************************************************/

void Brw_RenFolderFileBrowser (void)
  {
   extern const char *Txt_The_folder_name_X_has_changed_to_Y;
   extern const char *Txt_The_folder_name_X_has_not_changed;
   extern const char *Txt_The_folder_name_X_has_not_changed_because_there_is_already_a_folder_or_a_file_with_the_name_Y;
   extern const char *Txt_You_can_not_rename_this_folder;
   char OldPathInTree[PATH_MAX+1];
   char NewPathInTree[PATH_MAX+1];
   char OldPath[PATH_MAX+1];
   char NewPath[PATH_MAX+1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   if (Brw_CheckIfICanEditFileOrFolder (Gbl.FileBrowser.Level))	// Can I rename this folder?
     {
      if (Str_ConvertFilFolLnkNameToValid (Gbl.FileBrowser.NewFilFolLnkName))
        {
         if (strcmp (Gbl.FileBrowser.FilFolLnkName,Gbl.FileBrowser.NewFilFolLnkName))	// The name has changed
           {
            /* Gbl.FileBrowser.FilFolLnkName holds the new name of the folder */
            sprintf (OldPathInTree,"%s/%s",Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,Gbl.FileBrowser.FilFolLnkName);
            sprintf (OldPath,"%s/%s",Gbl.FileBrowser.Priv.PathAboveRootFolder,OldPathInTree);

            /* Gbl.FileBrowser.NewFilFolLnkName holds the new name of the folder */
            if (strlen (Gbl.FileBrowser.Priv.PathAboveRootFolder)+1+
                strlen (Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder)+1+
                strlen (Gbl.FileBrowser.NewFilFolLnkName) > PATH_MAX)
	       Lay_ShowErrorAndExit ("Path is too long.");
            sprintf (NewPathInTree,"%s/%s",Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,Gbl.FileBrowser.NewFilFolLnkName);
            sprintf (NewPath,"%s/%s",Gbl.FileBrowser.Priv.PathAboveRootFolder,NewPathInTree);

            /* We should check here that a folder with the same name does not exist.
	       but we leave this work to the system */

            /* Rename the directory. If a empty folder existed with the name new, overwrite it! */
            if (rename (OldPath,NewPath) == 0)
              {
	       /* If a folder is renamed,
                  it is necessary to rename all the entries in the tables of files
                  that belong to the subtree starting at that folder */
               Brw_RenameOneFolderInDB (OldPathInTree,
        	                        NewPathInTree);
               Brw_RenameChildrenFilesOrFoldersInDB (OldPathInTree,
        	                                     NewPathInTree);

               /* Remove affected clipboards */
               Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
        	                             Gbl.Usrs.Me.UsrDat.UsrCod,
        	                             Gbl.Usrs.Other.UsrDat.UsrCod);

               /* Remove affected expanded folders */
               Brw_RenameAffectedExpandedFolders (Gbl.FileBrowser.Type,
        	                                  Gbl.Usrs.Me.UsrDat.UsrCod,
        	                                  Gbl.Usrs.Other.UsrDat.UsrCod,
        	                                  OldPathInTree,
        	                                  NewPathInTree);

               /* Write message of confirmation */
	       sprintf (Gbl.Message,Txt_The_folder_name_X_has_changed_to_Y,
                        Gbl.FileBrowser.FilFolLnkName,
                        Gbl.FileBrowser.NewFilFolLnkName);
               Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
              }
            else
	      {
	       switch (errno)
	         {
	          case ENOTEMPTY:
	          case EEXIST:
	          case ENOTDIR:
	             sprintf (Gbl.Message,Txt_The_folder_name_X_has_not_changed_because_there_is_already_a_folder_or_a_file_with_the_name_Y,
			      Gbl.FileBrowser.FilFolLnkName,Gbl.FileBrowser.NewFilFolLnkName);
	             break;
	          case EACCES:
	             Lay_ShowErrorAndExit ("Write forbidden.");
	             break;
	          default:
	             Lay_ShowErrorAndExit ("Can not rename folder.");
	             break;
	         }
               Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	      }
           }
         else	// Names are equal. This may happens if we have press INTRO without changing the name
           {
	    sprintf (Gbl.Message,Txt_The_folder_name_X_has_not_changed,
                     Gbl.FileBrowser.FilFolLnkName);
            Lay_ShowAlert (Lay_INFO,Gbl.Message);
           }
        }
      else	// Folder name not valid
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
   else
      Lay_ShowErrorAndExit (Txt_You_can_not_rename_this_folder);

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/****** Receive a new file in a file browser unsigned Dropzone.js ************/
/*****************************************************************************/

void Brw_RcvFileInFileBrwDropzone (void)
  {
   bool UploadSucessful;

   /***** Receive file *****/
   UploadSucessful = Brw_RcvFileInFileBrw (Brw_DROPZONE_UPLOAD);

   /***** When a file is uploaded, the HTTP response
	  is a code status and a message for Dropzone.js *****/
   /* Don't write HTML at all */
   Gbl.Layout.HTMLStartWritten =
   Gbl.Layout.TablEndWritten   =
   Gbl.Layout.HTMLEndWritten   = true;

   /* Start HTTP response */
   fprintf (stdout,"Content-type: text/plain; charset=windows-1252\n");

   /* Status code and message */
   if (UploadSucessful)
      fprintf (stdout,"Status: 200\r\n\r\n");
   else
      fprintf (stdout,"Status: 501 Not Implemented\r\n\r\n"
		      "%s\n",
	       Gbl.Message);
  }

/*****************************************************************************/
/******** Receive a new file in a file browser using the classic way *********/
/*****************************************************************************/

void Brw_RcvFileInFileBrwClassic (void)
  {
   /***** Receive file and show feedback message *****/
   if (!Brw_RcvFileInFileBrw (Brw_CLASSIC_UPLOAD))
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/****************** Receive a new file in a file browser *********************/
/*****************************************************************************/

static bool Brw_RcvFileInFileBrw (Brw_UploadType_t UploadType)
  {
   extern const char *Txt_UPLOAD_FILE_X_file_already_exists_NO_HTML;
   extern const char *Txt_UPLOAD_FILE_could_not_create_file_NO_HTML;
   extern const char *Txt_UPLOAD_FILE_X_quota_exceeded_NO_HTML;
   extern const char *Txt_The_file_X_has_been_placed_inside_the_folder_Y;
   extern const char *Txt_UPLOAD_FILE_You_must_specify_the_file_NO_HTML;
   extern const char *Txt_UPLOAD_FILE_Forbidden_NO_HTML;
   char SrcFileName[PATH_MAX+1];
   char PathUntilFileName[PATH_MAX+1];
   char Path[PATH_MAX+1];
   char PathTmp[PATH_MAX+1];
   char PathCompleteInTreeIncludingFile[PATH_MAX+1];
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE+1];
   bool AdminMarks;
   bool FileIsValid = true;
   long FilCod = -1L;	// Code of new file in database
   struct FileMetadata FileMetadata;
   struct MarksProperties Marks;
   unsigned NumUsrsToBeNotifiedByEMail;
   char FileNameToShow[NAME_MAX+1];
   bool UploadSucessful = false;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();
   AdminMarks = Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_CRS ||
                Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_GRP;

   /***** Check if creating a new file is allowed *****/
   if (Brw_CheckIfICanCreateIntoFolder (Gbl.FileBrowser.Level))
     {
      /***** First, we save in disk the file from stdin (really from Gbl.F.Tmp) *****/
      Fil_StartReceptionOfFile (SrcFileName,MIMEType);

      /***** Get filename from path *****/
      // Spaces at start or end are allowed
      Str_SplitFullPathIntoPathAndFileName (SrcFileName,
	                                    PathUntilFileName,
	                                    Gbl.FileBrowser.NewFilFolLnkName);
      if (Gbl.FileBrowser.NewFilFolLnkName[0])
        {
         /***** Check if uploading this kind of file is allowed *****/
	 if (Brw_CheckIfUploadIsAllowed (MIMEType))	// Gbl.Message contains feedback text
           {
            if (Str_ConvertFilFolLnkNameToValid (Gbl.FileBrowser.NewFilFolLnkName))	// Gbl.Message contains feedback text
              {
               /* Gbl.FileBrowser.NewFilFolLnkName holds the name of the new file */
               sprintf (Path,"%s/%s",Gbl.FileBrowser.Priv.PathAboveRootFolder,Gbl.FileBrowser.Priv.FullPathInTree);
               if (strlen (Path) + 1 + strlen (Gbl.FileBrowser.NewFilFolLnkName) + strlen (".tmp") > PATH_MAX)
	          Lay_ShowErrorAndExit ("Path is too long.");
               strcat (Path,"/");
               strcat (Path,Gbl.FileBrowser.NewFilFolLnkName);

               /* Check if the destination file exists */
               if (Fil_CheckIfPathExists (Path))
                 {
                  sprintf (Gbl.Message,Txt_UPLOAD_FILE_X_file_already_exists_NO_HTML,
                           Gbl.FileBrowser.NewFilFolLnkName);
                 }
               else	// Destination file does not exist
                 {
                  /* End receiving the file */
                  sprintf (PathTmp,"%s.tmp",Path);
                  FileIsValid = Fil_EndReceptionOfFile (PathTmp);	// Gbl.Message contains feedback text

                  /* Check if the content of the file of marks is valid */
                  if (FileIsValid)
                     if (AdminMarks)
                        if (!Mrk_CheckFileOfMarks (PathTmp,&Marks))	// Gbl.Message contains feedback text
                           FileIsValid = false;

                  if (FileIsValid)
                    {
                     /* Rename the temporary */
                     if (rename (PathTmp,Path))
	               {
	                Brw_RemoveTree (PathTmp);
                        sprintf (Gbl.Message,Txt_UPLOAD_FILE_could_not_create_file_NO_HTML,
                                 Gbl.FileBrowser.NewFilFolLnkName);
	               }
                     else
	               {
	                /* Check if quota has been exceeded */
	                Brw_CalcSizeOfDir (Gbl.FileBrowser.Priv.PathRootFolder);
	                Brw_SetMaxQuota ();
                        if (Brw_CheckIfQuotaExceded ())
	                  {
	                   Brw_RemoveTree (Path);
	                   sprintf (Gbl.Message,Txt_UPLOAD_FILE_X_quota_exceeded_NO_HTML,
		                    Gbl.FileBrowser.NewFilFolLnkName);
	                  }
	                else
                          {
                           /* Remove affected clipboards */
                           Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Other.UsrDat.UsrCod);

                           /* Add path where new file is created to table of expanded folders */
                           Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.Priv.FullPathInTree);

                           /* Add entry to the table of files/folders */
                           sprintf (PathCompleteInTreeIncludingFile,"%s/%s",Gbl.FileBrowser.Priv.FullPathInTree,Gbl.FileBrowser.NewFilFolLnkName);
                           FilCod = Brw_AddPathToDB (Gbl.Usrs.Me.UsrDat.UsrCod,Brw_IS_FILE,
                                                     PathCompleteInTreeIncludingFile,false,Brw_LICENSE_DEFAULT);

                           /* Show message of confirmation */
                           if (UploadType == Brw_CLASSIC_UPLOAD)
                             {
			      Brw_GetFileNameToShow (Gbl.FileBrowser.Type,Gbl.FileBrowser.Level,Brw_IS_FOLDER,
			                             Gbl.FileBrowser.FilFolLnkName,FileNameToShow);
			      sprintf (Gbl.Message,Txt_The_file_X_has_been_placed_inside_the_folder_Y,
			               Gbl.FileBrowser.NewFilFolLnkName,
			               FileNameToShow);
			      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
                             }
			   UploadSucessful = true;

                           FileMetadata.FilCod = FilCod;
                           Brw_GetFileMetadataByCod (&FileMetadata);

                           /* Add a new entry of marks into database */
			   if (AdminMarks)
                              Mrk_AddMarksToDB (FileMetadata.FilCod,&Marks);

                           /* Notify new file */
			   if (!Brw_CheckIfFileOrFolderIsHidden (&FileMetadata))
			      switch (Gbl.FileBrowser.Type)
				{
				 case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
				 case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
				    NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_DOCUMENT_FILE,FilCod);
                                    if (UploadType == Brw_CLASSIC_UPLOAD)
				       Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);
				    break;
				 case Brw_FILE_BRW_COMMON_CRS:
				 case Brw_FILE_BRW_COMMON_GRP:
				    NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_SHARED_FILE,FilCod);
                                    if (UploadType == Brw_CLASSIC_UPLOAD)
				       Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);
				    break;
				 case Brw_FILE_BRW_ADMIN_MARKS_CRS:
				 case Brw_FILE_BRW_ADMIN_MARKS_GRP:
				    NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_MARKS_FILE,FilCod);
                                    if (UploadType == Brw_CLASSIC_UPLOAD)
				       Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);
				    break;
				 default:
				    break;
				}
                          }
                       }
	            }
                  else	// Error in file reception. File probably too big
	             Brw_RemoveTree (PathTmp);
                 }
              }
           }
        }
      else	// Empty filename
         strcpy (Gbl.Message,Txt_UPLOAD_FILE_You_must_specify_the_file_NO_HTML);
     }
   else		// I do not have permission to create files here
      strcpy (Gbl.Message,Txt_UPLOAD_FILE_Forbidden_NO_HTML);

   return UploadSucessful;
  }

/*****************************************************************************/
/******************* Receive a new link in a file browser ********************/
/*****************************************************************************/

void Brw_RecLinkFileBrowser (void)
  {
   extern const char *Txt_Can_not_create_the_link_X_because_there_is_already_a_folder_or_a_link_with_that_name;
   extern const char *Txt_Can_not_create_the_link_X_because_it_would_exceed_the_disk_quota;
   extern const char *Txt_The_link_X_has_been_placed_inside_the_folder_Y;
   extern const char *Txt_UPLOAD_FILE_Invalid_name;
   extern const char *Txt_You_can_not_create_links_here;
   char URLWithoutEndingSlash[NAME_MAX+1];	// TODO: It should be PATH_MAX
   size_t LengthURL;
   char URLUntilLastFilename[PATH_MAX+1];
   char LastFilenameInURL[NAME_MAX+1];
   char Path[PATH_MAX+1];
   FILE *FileURL;
   char PathCompleteInTreeIncludingFile[PATH_MAX+1];
   long FilCod = -1L;	// Code of new file in database
   char FileNameToShow[NAME_MAX+1];
   struct FileMetadata FileMetadata;
   unsigned NumUsrsToBeNotifiedByEMail;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Check if creating a new link is allowed *****/
   if (Brw_CheckIfICanCreateIntoFolder (Gbl.FileBrowser.Level))
     {
      /***** Create a new file to store URL ****/
      /*
         Gbl.FileBrowser.NewFilFolLnkName holds the URL of the new link
         Example:
         URL: http://www.intel.es/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-manual-325462.pdf
         File in swad: 64-ia-32-architectures-software-developer-manual-325462.pdf.url
      */
      if ((LengthURL = strlen (Gbl.FileBrowser.NewFilFolLnkName)))
	{
	 strncpy (URLWithoutEndingSlash,Gbl.FileBrowser.NewFilFolLnkName,NAME_MAX);
	 URLWithoutEndingSlash[NAME_MAX] = '\0';

         /* Remove possible final '/' from URL */
	 if (URLWithoutEndingSlash[LengthURL - 1] == '/')
            URLWithoutEndingSlash[LengthURL - 1] = '\0';

         /* Get the last name in URL-without-ending-slash */
	 Str_SplitFullPathIntoPathAndFileName (URLWithoutEndingSlash,
					       URLUntilLastFilename,
					       LastFilenameInURL);

	 /* Convert the last name in URL to a valid filename */
	 if (Str_ConvertFilFolLnkNameToValid (LastFilenameInURL))	// Gbl.Message contains feedback text
	   {
	    /* The name of the file with the link will be the LastFilenameInURL.url */
	    sprintf (Path,"%s/%s",Gbl.FileBrowser.Priv.PathAboveRootFolder,Gbl.FileBrowser.Priv.FullPathInTree);
	    if (strlen (Path) + 1 + strlen (LastFilenameInURL) + strlen (".url") > PATH_MAX)
	       Lay_ShowErrorAndExit ("Path is too long.");
	    strcat (Path,"/");
	    strcat (Path,LastFilenameInURL);
	    strcat (Path,".url");

	    /* Check if the URL file exists */
	    if (Fil_CheckIfPathExists (Path))
	      {
	       sprintf (Gbl.Message,Txt_Can_not_create_the_link_X_because_there_is_already_a_folder_or_a_link_with_that_name,
			LastFilenameInURL);
	       Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	      }
	    else	// URL file does not exist
	      {
	       /***** Create the new file with the URL *****/
	       if ((FileURL = fopen (Path,"wb")) != NULL)
		 {
		  /* Write URL */
		  fprintf (FileURL,"%s",Gbl.FileBrowser.NewFilFolLnkName);

		  /* Close file */
		  fclose (FileURL);

		  /* Check if quota has been exceeded */
		  Brw_CalcSizeOfDir (Gbl.FileBrowser.Priv.PathRootFolder);
		  Brw_SetMaxQuota ();
		  if (Brw_CheckIfQuotaExceded ())
		    {
		     Brw_RemoveTree (Path);
		     sprintf (Gbl.Message,Txt_Can_not_create_the_link_X_because_it_would_exceed_the_disk_quota,
			      LastFilenameInURL);
		     Lay_ShowAlert (Lay_WARNING,Gbl.Message);
		    }
		  else
		    {
		     /* Remove affected clipboards */
		     Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Other.UsrDat.UsrCod);

		     /* Add path where new file is created to table of expanded folders */
		     Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.Priv.FullPathInTree);

		     /* Add entry to the table of files/folders */
		     sprintf (PathCompleteInTreeIncludingFile,"%s/%s.url",Gbl.FileBrowser.Priv.FullPathInTree,LastFilenameInURL);
		     FilCod = Brw_AddPathToDB (Gbl.Usrs.Me.UsrDat.UsrCod,Brw_IS_LINK,
					       PathCompleteInTreeIncludingFile,false,Brw_LICENSE_DEFAULT);

		     /* Show message of confirmation */
		     Brw_GetFileNameToShow (Gbl.FileBrowser.Type,Gbl.FileBrowser.Level,Brw_IS_FOLDER,
					    Gbl.FileBrowser.FilFolLnkName,FileNameToShow);
		     sprintf (Gbl.Message,Txt_The_link_X_has_been_placed_inside_the_folder_Y,
			      LastFilenameInURL,FileNameToShow);
		     Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

		     FileMetadata.FilCod = FilCod;
		     Brw_GetFileMetadataByCod (&FileMetadata);

		     /* Notify new file */
		     if (!Brw_CheckIfFileOrFolderIsHidden (&FileMetadata))
			switch (Gbl.FileBrowser.Type)
			  {
			   case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
			   case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
			      NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_DOCUMENT_FILE,FilCod);
			      Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);
			      break;
			   case Brw_FILE_BRW_COMMON_CRS:
			   case Brw_FILE_BRW_COMMON_GRP:
			      NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_SHARED_FILE,FilCod);
			      Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);
			      break;
			   case Brw_FILE_BRW_ADMIN_MARKS_CRS:
			   case Brw_FILE_BRW_ADMIN_MARKS_GRP:
			      NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_MARKS_FILE,FilCod);
			      Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);
			      break;
			   default:
			      break;
			  }
		    }
		 }
	      }
	   }
	 else	// Link name not valid
	    Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	}
      else	// Link name not valid
	 Lay_ShowAlert (Lay_WARNING,Txt_UPLOAD_FILE_Invalid_name);
     }
   else
      Lay_ShowErrorAndExit (Txt_You_can_not_create_links_here);	// It's difficult, but not impossible that a user sees this message

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/****************** Check if it is allowed to upload a file ******************/
/*****************************************************************************/
// Returns true if file type is allowed
// Returns false if MIME type or extension are not allowed
// On error, Gbl.Message will contain feedback text

static bool Brw_CheckIfUploadIsAllowed (const char *MIMEType)
  {
   extern const char *Txt_UPLOAD_FILE_X_MIME_type_Y_not_allowed_NO_HTML;
   extern const char *Txt_UPLOAD_FILE_X_not_HTML_NO_HTML;
   extern const char *Txt_UPLOAD_FILE_X_extension_not_allowed_NO_HTML;
   unsigned Type;
   bool MIMETypeIsAllowed = false;
   bool ExtensionIsAllowed = false;

   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ADMIN_MARKS_CRS:
      case Brw_FILE_BRW_ADMIN_MARKS_GRP:
	 /* Check file extension */
	 if ((ExtensionIsAllowed = Str_FileIsHTML (Gbl.FileBrowser.NewFilFolLnkName)))
	   {
	    /* Check MIME type*/
	    if (strcmp (MIMEType,"text/html") &&
		strcmp (MIMEType,"text/plain") &&
		strcmp (MIMEType,"application/octet-stream"))	// MIME type forbidden
	      {
	       sprintf (Gbl.Message,Txt_UPLOAD_FILE_X_MIME_type_Y_not_allowed_NO_HTML,
			Gbl.FileBrowser.NewFilFolLnkName,MIMEType);
	       return false;
	      }
	   }
	 else
	   {
	    sprintf (Gbl.Message,Txt_UPLOAD_FILE_X_not_HTML_NO_HTML,
		     Gbl.FileBrowser.NewFilFolLnkName);
	    return false;
	   }
	 break;
      default:
	 /* Check the file extension */
	 for (Type = 0;
	      Type < Brw_NUM_FILE_EXT_ALLOWED;
	      Type++)
	    if (Str_FileIs (Gbl.FileBrowser.NewFilFolLnkName,Brw_FileExtensionsAllowed[Type]))
	      {
	       ExtensionIsAllowed = true;
	       break;
	      }
	 if (ExtensionIsAllowed)
	   {
	    /* Check type MIME */
	    for (Type = 0;
		 Type < Brw_NUM_MIME_TYPES_ALLOWED;
		 Type++)
	       if (!strcmp (MIMEType,Brw_MIMETypesAllowed[Type]))
		 {
		  MIMETypeIsAllowed = true;
		  break;
		 }
	    if (!MIMETypeIsAllowed)
	      {
	       sprintf (Gbl.Message,Txt_UPLOAD_FILE_X_MIME_type_Y_not_allowed_NO_HTML,
			Gbl.FileBrowser.NewFilFolLnkName,MIMEType);
	       return false;
	      }
	   }
	 else
	   {
	    sprintf (Gbl.Message,Txt_UPLOAD_FILE_X_extension_not_allowed_NO_HTML,
		     Gbl.FileBrowser.NewFilFolLnkName);
	    return false;
	   }
	 break;
     }

   return true;
  }

/*****************************************************************************/
/******************* Show file or folder in a file browser *******************/
/*****************************************************************************/

void Brw_SetDocumentAsVisible (void)
  {
   extern const char *Txt_FILE_FOLDER_OR_LINK_X_is_now_visible;
   char FileNameToShow[NAME_MAX+1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Change file to visible *****/
   if (Brw_CheckIfFileOrFolderIsSetAsHiddenInDB (Gbl.FileBrowser.FileType,
                                                 Gbl.FileBrowser.Priv.FullPathInTree))
      Brw_ChangeFileOrFolderHiddenInDB (Gbl.FileBrowser.Priv.FullPathInTree,false);

   /***** Remove the affected clipboards *****/
   Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Write message of confirmation *****/
   Brw_GetFileNameToShow (Gbl.FileBrowser.Type,Gbl.FileBrowser.Level,Gbl.FileBrowser.FileType,
                          Gbl.FileBrowser.FilFolLnkName,FileNameToShow);
   sprintf (Gbl.Message,Txt_FILE_FOLDER_OR_LINK_X_is_now_visible,
            FileNameToShow);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/***************** Hide file or folder in a file browser *********************/
/*****************************************************************************/

void Brw_SetDocumentAsHidden (void)
  {
   extern const char *Txt_FILE_FOLDER_OR_LINK_X_is_now_hidden;
   char FileNameToShow[NAME_MAX+1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** If the file or folder is not already set as hidden in database,
          set it as hidden *****/
   if (!Brw_CheckIfFileOrFolderIsSetAsHiddenInDB (Gbl.FileBrowser.FileType,
                                                  Gbl.FileBrowser.Priv.FullPathInTree))
      Brw_ChangeFileOrFolderHiddenInDB (Gbl.FileBrowser.Priv.FullPathInTree,true);

   /***** Remove the affected clipboards *****/
   Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Write confirmation message *****/
   Brw_GetFileNameToShow (Gbl.FileBrowser.Type,Gbl.FileBrowser.Level,Gbl.FileBrowser.FileType,
                          Gbl.FileBrowser.FilFolLnkName,FileNameToShow);
   sprintf (Gbl.Message,Txt_FILE_FOLDER_OR_LINK_X_is_now_hidden,FileNameToShow);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/** Check if a file / folder from the documents zone is set as hidden in DB **/
/*****************************************************************************/

bool Brw_CheckIfFileOrFolderIsSetAsHiddenInDB (Brw_FileType_t FileType,const char *Path)
  {
   char Query[512+PATH_MAX];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool IsHidden = false;

   /***** Get if a file or folder is hidden from database *****/
   sprintf (Query,"SELECT Hidden FROM files"
                  " WHERE CrsCod='%ld' AND GrpCod='%ld' AND ZoneUsrCod='%ld'"
                  " AND FileBrowser='%u'"
                  " AND Path='%s'",
            Brw_GetCrsCod (),
            Brw_GetGrpCod (),
            Brw_GetZoneUsrCod (),
            (unsigned) Brw_FileBrowserForDB[Gbl.FileBrowser.Type],
            Path);
   if (DB_QuerySELECT (Query,&mysql_res,"can not check if a file is hidden"))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* File is hidden? (row[0]) */
      IsHidden = (Str_ConvertToUpperLetter (row[0][0]) == 'Y');
     }
   else
      Brw_AddPathToDB (-1L,FileType,
                       Gbl.FileBrowser.Priv.FullPathInTree,false,Brw_LICENSE_DEFAULT);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return IsHidden;
  }

/*****************************************************************************/
/******** Check if a file / folder from the documents zone is hidden *********/
/*****************************************************************************/

bool Brw_CheckIfFileOrFolderIsHidden (struct FileMetadata *FileMetadata)
  {
   char Query[512+PATH_MAX];

   /***** Get if a file or folder is under a hidden folder from database *****/
   /*
      The argument Path passed to this function is hidden if:
      1) the argument Path is exactly the same as a path stored in database
         or
      2) the argument Path begins by 'x/', where x is a path stored in database
   */
   sprintf (Query,"SELECT COUNT(*) FROM files"
                  " WHERE CrsCod='%ld' AND GrpCod='%ld' AND ZoneUsrCod='%ld'"
                  " AND FileBrowser='%u' AND Hidden='Y'"
                  " AND (Path='%s' OR LOCATE(CONCAT(Path,'/'),'%s')=1)",
            FileMetadata->CrsCod,
            FileMetadata->GrpCod,
            FileMetadata->ZoneUsrCod,
            FileMetadata->FileBrowser,
            FileMetadata->Path,
            FileMetadata->Path);

   return (DB_QueryCOUNT (Query,"can not check if a file or folder is hidden") != 0);
  }

/*****************************************************************************/
/***************** Show metadata of a file in a file browser *****************/
/*****************************************************************************/

void Brw_ShowFileMetadata (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_The_file_of_folder_no_longer_exists_or_is_now_hidden;
   extern const char *Txt_Filename;
   extern const char *Txt_File_size;
   extern const char *Txt_Uploaded_by;
   extern const char *Txt_ROLES_SINGULAR_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Date_of_creation;
   extern const char *Txt_Availability;
   extern const char *Txt_Private_available_to_certain_users_identified;
   extern const char *Txt_Public_open_educational_resource_OER_for_everyone;
   extern const char *Txt_License;
   extern const char *Txt_LICENSES[Brw_NUM_LICENSES];
   extern const char *Txt_My_views;
   extern const char *Txt_Identified_views;
   extern const char *Txt_Public_views;
   extern const char *Txt_user[Usr_NUM_SEXS];
   extern const char *Txt_users;
   extern const char *Txt_Save_file_properties;
   struct FileMetadata FileMetadata;
   struct UsrData PublisherUsrDat;
   char FileNameToShow[NAME_MAX+1];
   char URL[PATH_MAX+1];
   bool Found;
   bool IsHidden = false;
   bool ICanView;
   bool ICanEdit;
   bool ICanChangePublic = false;
   bool ICanChangeLicense = false;
   bool FileHasPublisher;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];
   Brw_License_t License;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get file metadata *****/
   Brw_GetFileMetadataByPath (&FileMetadata);
   Found = Brw_GetFileSizeAndDate (&FileMetadata);

   if (Found)
     {
      if (FileMetadata.FilCod <= 0)	// No entry for this file in database table of files
	{
	 /*  entry to the table of files/folders */
	 FileMetadata.FilCod = Brw_AddPathToDB (-1L,FileMetadata.FileType,
	                                        Gbl.FileBrowser.Priv.FullPathInTree,false,Brw_LICENSE_DEFAULT);
	 Brw_GetFileMetadataByCod (&FileMetadata);
	}

      /***** Check if I can view this file.
	     It could be marked as hidden by teachers *****/
      if (Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_DOCUMENTS_CRS ||
	  Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_DOCUMENTS_GRP)
	 IsHidden = Brw_CheckIfFileOrFolderIsHidden (&FileMetadata);
     }

   ICanView = Found && (!IsHidden || Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_TEACHER);
   if (ICanView)
     {
      if (FileMetadata.FileType == Brw_IS_FILE ||
	  FileMetadata.FileType == Brw_IS_LINK)
	{
	 /***** Update number of views *****/
	 Brw_GetAndUpdateFileViews (&FileMetadata);

	 /***** Get data of file/folder publisher *****/
	 if (FileMetadata.PublisherUsrCod > 0)
	   {
	    /***** Initialize structure with publisher's data *****/
	    Usr_UsrDataConstructor (&PublisherUsrDat);

	    PublisherUsrDat.UsrCod = FileMetadata.PublisherUsrCod;
	    FileHasPublisher = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&PublisherUsrDat);
	   }
	 else
	    FileHasPublisher = false;	// Get user's data from database

	 /***** Get link to download the file *****/
	 if (Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_MARKS_CRS ||
	     Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_MARKS_GRP)
	    URL[0] = '\0';
	 else
	    Brw_GetLinkToDownloadFile (Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,
				       Gbl.FileBrowser.FilFolLnkName,
				       URL);

	 /***** Can I edit the properties of the file? *****/
	 ICanEdit = Brw_CheckIfICanEditFileMetadata (FileMetadata.PublisherUsrCod);

	 /***** Name of the file/link to be shown *****/
	 Brw_LimitLengthFileNameToShow (FileMetadata.FileType,
	                                Gbl.FileBrowser.FilFolLnkName,FileNameToShow);

	 /***** Start frame *****/
	 Lay_StartRoundFrameTable10Shadow (NULL,0);
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td style=\"text-align:center;\">");

	 /***** Start form to update the metadata of a file *****/
	 if (ICanEdit)	// I can edit file properties
	   {
	    Act_FormStart (Brw_ActRecDatFile[Gbl.FileBrowser.Type]);
	    switch (Gbl.FileBrowser.Type)
	      {
	       case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
	       case Brw_FILE_BRW_COMMON_CRS:
		  ICanChangePublic  = true;
		  ICanChangeLicense = true;
		  break;
	       case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
	       case Brw_FILE_BRW_COMMON_GRP:
		  Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
		  ICanChangePublic  = false;	// A file in group zones can not be public...
		  ICanChangeLicense = true;	// ...but I can change its license
		  break;
	       case Brw_FILE_BRW_ASSIGNMENTS_USR:
	       case Brw_FILE_BRW_WORKS_USR:
		  ICanChangePublic  = false;	// A file in assignments or works zones can not be public...
		  ICanChangeLicense = true;	// ...but I can change its license
		  break;
	       case Brw_FILE_BRW_ASSIGNMENTS_CRS:
	       case Brw_FILE_BRW_WORKS_CRS:
		  Usr_PutHiddenParUsrCodAll (Brw_ActRecDatFile[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
		  Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
		  ICanChangePublic  = false;	// A file in assignments or works zones can not be public...
		  ICanChangeLicense = true;	// ...but I can change its license
		  break;
	       case Brw_FILE_BRW_ADMIN_MARKS_GRP:
		  Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
		  ICanChangePublic  = false;
		  ICanChangeLicense = false;
		  break;
	       default:
		  ICanChangePublic  = false;
		  ICanChangeLicense = false;
		  break;
	      }
	    Brw_ParamListFiles (FileMetadata.FileType,
	                        Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,
				Gbl.FileBrowser.FilFolLnkName);
	   }

	 /***** Start table *****/
	 fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\">");

	 /***** Link to download the file *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td colspan=\"2\" class=\"FILENAME\""
			    " style=\"text-align:center;"
			    " vertical-align:middle;\">");
	 Brw_WriteBigLinkToDownloadFile (URL,FileMetadata.FileType,
	                                 FileNameToShow);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /***** Filename *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">",
		  The_ClassFormul[Gbl.Prefs.Theme],Txt_Filename);
	 Brw_WriteSmallLinkToDownloadFile (URL,FileMetadata.FileType,FileNameToShow);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /***** Publisher's data *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">",
		  The_ClassFormul[Gbl.Prefs.Theme],Txt_Uploaded_by);
	 if (FileHasPublisher)
	   {
	    /* Show photo */
	    ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&PublisherUsrDat,PhotoURL);
	    Pho_ShowUsrPhoto (&PublisherUsrDat,ShowPhoto ? PhotoURL :
	                	                           NULL,
	                      "PHOTO12x16",true);

	    /* Write name */
	    fprintf (Gbl.F.Out,"%s",
		     PublisherUsrDat.FullName);
	   }
	 else
	    /* Unknown publisher */
	    fprintf (Gbl.F.Out,"%s",Txt_ROLES_SINGULAR_Abc[Rol_ROLE_UNKNOWN][Usr_SEX_UNKNOWN]);
	 fprintf (Gbl.F.Out,"</td>"
	                    "</tr>");

	 /***** Free memory used for publisher's data *****/
	 if (FileMetadata.PublisherUsrCod > 0)
	    Usr_UsrDataDestructor (&PublisherUsrDat);

	 /***** Write the file size *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">",
		  The_ClassFormul[Gbl.Prefs.Theme],Txt_File_size);
	 Str_WriteSizeInBytesFull ((double) FileMetadata.Size);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /***** Write the date *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">",
		  The_ClassFormul[Gbl.Prefs.Theme],Txt_Date_of_creation);
	 Dat_GetLocalTimeFromClock (&(FileMetadata.Time));
	 Dat_WriteDateTimeFromtblock ();
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /***** Private or public? *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">",
		  The_ClassFormul[Gbl.Prefs.Theme],Txt_Availability);
	 if (ICanChangePublic)	// I can change file to public
	   {
	    fprintf (Gbl.F.Out,"<select name=\"PublicFile\">");

	    fprintf (Gbl.F.Out,"<option value=\"N\"");
	    if (!FileMetadata.IsPublic)
	       fprintf (Gbl.F.Out," selected=\"selected\"");
	    fprintf (Gbl.F.Out,">%s</option>",Txt_Private_available_to_certain_users_identified);

	    fprintf (Gbl.F.Out,"<option value=\"Y\"");
	    if (FileMetadata.IsPublic)
	       fprintf (Gbl.F.Out," selected=\"selected\"");
	    fprintf (Gbl.F.Out,">%s</option>",Txt_Public_open_educational_resource_OER_for_everyone);

	    fprintf (Gbl.F.Out,"</select>");
	   }
	 else		// I can not edit file properties
	    fprintf (Gbl.F.Out,"%s",
	             FileMetadata.IsPublic ? Txt_Public_open_educational_resource_OER_for_everyone :
					     Txt_Private_available_to_certain_users_identified);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /***** License *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">",
		  The_ClassFormul[Gbl.Prefs.Theme],Txt_License);
	 if (ICanChangeLicense)	// I can edit file properties
	   {
	    fprintf (Gbl.F.Out,"<select name=\"License\">");

	    for (License = 0;
		 License < Brw_NUM_LICENSES;
		 License++)
	      {
	       fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) License);
	       if (License == FileMetadata.License)
		  fprintf (Gbl.F.Out," selected=\"selected\"");
	       fprintf (Gbl.F.Out,">%s</option>",Txt_LICENSES[License]);
	      }
	    fprintf (Gbl.F.Out,"</select>");
	   }
	 else		// I can not edit file properties
	    fprintf (Gbl.F.Out,"%s",Txt_LICENSES[FileMetadata.License]);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /***** Write my number of views *****/
	 if (Gbl.Usrs.Me.Logged)
	    fprintf (Gbl.F.Out,"<tr>"
			       "<td class=\"%s\" style=\"text-align:right;"
			       " vertical-align:middle;\">"
			       "%s:"
			       "</td>"
			       "<td class=\"DAT\" style=\"text-align:left;"
			       " vertical-align:middle;\">"
			       "%u"
			       "</td>"
			       "</tr>",
		     The_ClassFormul[Gbl.Prefs.Theme],Txt_My_views,
		     FileMetadata.NumMyViews);

	 /***** Write number of identificated views *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">"
			    "%u (%u %s)"
			    "</td>"
			    "</tr>",
		  The_ClassFormul[Gbl.Prefs.Theme],Txt_Identified_views,
		  FileMetadata.NumViewsFromLoggedUsrs,
		  FileMetadata.NumLoggedUsrs,
		  (FileMetadata.NumLoggedUsrs == 1) ? Txt_user[Usr_SEX_UNKNOWN] :
			                              Txt_users);

	 /***** Write number of public views *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassFormul[Gbl.Prefs.Theme],Txt_Public_views,
		  FileMetadata.NumPublicViews);

	 /***** End form and table *****/
	 fprintf (Gbl.F.Out,"</table>");

	 if (ICanEdit)	// I can edit file properties
	   {
	    Lay_PutSendButton (Txt_Save_file_properties);
	    fprintf (Gbl.F.Out,"</form>");
	   }

	 /***** End frame *****/
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");
	 Lay_EndRoundFrameTable10 ();

	 /***** Mark possible notifications as seen *****/
	 switch (Gbl.FileBrowser.Type)
	   {
	    case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
	    case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
	    case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
	    case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
	       Ntf_SetNotifAsSeen (Ntf_EVENT_DOCUMENT_FILE,
				   FileMetadata.FilCod,
				   Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    case Brw_FILE_BRW_COMMON_CRS:
	    case Brw_FILE_BRW_COMMON_GRP:
	       Ntf_SetNotifAsSeen (Ntf_EVENT_SHARED_FILE,
				   FileMetadata.FilCod,
				   Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    case Brw_FILE_BRW_SEE_MARKS_CRS:
	    case Brw_FILE_BRW_SEE_MARKS_GRP:
	    case Brw_FILE_BRW_ADMIN_MARKS_CRS:
	    case Brw_FILE_BRW_ADMIN_MARKS_GRP:
	       Ntf_SetNotifAsSeen (Ntf_EVENT_MARKS_FILE,
				   FileMetadata.FilCod,
				   Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    default:
	       break;
	   }
	}

      /***** Add paths until file to table of expanded folders *****/
      Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder);
     }
   else	// !ICanView
     {
      /***** Mark possible notifications about non visible file as removed *****/
      switch (Gbl.FileBrowser.Type)
	{
	 case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
	 case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
	 case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
	 case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
	    Ntf_SetNotifAsRemoved (Ntf_EVENT_DOCUMENT_FILE,
				   FileMetadata.FilCod);
	    break;
	 case Brw_FILE_BRW_COMMON_CRS:
	 case Brw_FILE_BRW_COMMON_GRP:
	    Ntf_SetNotifAsRemoved (Ntf_EVENT_SHARED_FILE,
				   FileMetadata.FilCod);
	    break;
	 case Brw_FILE_BRW_SEE_MARKS_CRS:
	 case Brw_FILE_BRW_SEE_MARKS_GRP:
	 case Brw_FILE_BRW_ADMIN_MARKS_CRS:
	 case Brw_FILE_BRW_ADMIN_MARKS_GRP:
	    Ntf_SetNotifAsRemoved (Ntf_EVENT_MARKS_FILE,
				   FileMetadata.FilCod);
	    break;
	 default:
	    break;
	}

      Lay_ShowAlert (Lay_WARNING,Txt_The_file_of_folder_no_longer_exists_or_is_now_hidden);
     }

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/******************* Download a file from a file browser *********************/
/*****************************************************************************/

void Brw_DownloadFile (void)
  {
   extern const char *Txt_The_file_of_folder_no_longer_exists_or_is_now_hidden;
   struct FileMetadata FileMetadata;
   char URL[PATH_MAX+1];
   bool Found;
   bool IsHidden = false;
   bool ICanView;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get file metadata *****/
   Brw_GetFileMetadataByPath (&FileMetadata);
   Found = Brw_GetFileSizeAndDate (&FileMetadata);

   if (Found)
     {
      if (FileMetadata.FilCod <= 0)	// No entry for this file in database table of files
	{
	 /* Add entry to the table of files/folders */
	 FileMetadata.FilCod = Brw_AddPathToDB (-1L,FileMetadata.FileType,
	                                        Gbl.FileBrowser.Priv.FullPathInTree,false,Brw_LICENSE_DEFAULT);
	 Brw_GetFileMetadataByCod (&FileMetadata);
	}

      /***** Check if I can view this file.
	     It could be marked as hidden by teachers *****/
      if (Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_DOCUMENTS_CRS ||
	  Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_DOCUMENTS_GRP)
	 IsHidden = Brw_CheckIfFileOrFolderIsHidden (&FileMetadata);
     }

   ICanView = Found &&
	      (!IsHidden || Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_TEACHER);
   if (ICanView)
     {
      if (FileMetadata.FileType == Brw_IS_FILE ||
	  FileMetadata.FileType == Brw_IS_LINK)
	{
	 /***** Update number of views *****/
	 Brw_GetAndUpdateFileViews (&FileMetadata);

	 /***** Get link to download the file *****/
	 if (Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_MARKS_CRS ||
	     Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_MARKS_GRP)
	    URL[0] = '\0';
	 else
	    Brw_GetLinkToDownloadFile (Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,
				       Gbl.FileBrowser.FilFolLnkName,
				       URL);

	 /***** Mark possible notifications as seen *****/
	 switch (Gbl.FileBrowser.Type)
	   {
	    case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
	    case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
	    case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
	    case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
	       Ntf_SetNotifAsSeen (Ntf_EVENT_DOCUMENT_FILE,
				   FileMetadata.FilCod,
				   Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    case Brw_FILE_BRW_COMMON_CRS:
	    case Brw_FILE_BRW_COMMON_GRP:
	       Ntf_SetNotifAsSeen (Ntf_EVENT_SHARED_FILE,
				   FileMetadata.FilCod,
				   Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    case Brw_FILE_BRW_SEE_MARKS_CRS:
	    case Brw_FILE_BRW_SEE_MARKS_GRP:
	    case Brw_FILE_BRW_ADMIN_MARKS_CRS:
	    case Brw_FILE_BRW_ADMIN_MARKS_GRP:
	       Ntf_SetNotifAsSeen (Ntf_EVENT_MARKS_FILE,
				   FileMetadata.FilCod,
				   Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    default:
	       break;
	   }
	}

      /***** Add paths until file to table of expanded folders *****/
      Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder);

      /***** Download the file *****/
      fprintf (stdout,"Location: %s\n\n",URL);
      Gbl.Layout.HTMLStartWritten =
      Gbl.Layout.TablEndWritten   =
      Gbl.Layout.HTMLEndWritten   = true;	// Don't write HTML at all
     }
   else	// !ICanView
     {
      /***** Mark possible notifications about non visible file as removed *****/
      switch (Gbl.FileBrowser.Type)
	{
	 case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
	 case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
	 case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
	 case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
	    Ntf_SetNotifAsRemoved (Ntf_EVENT_DOCUMENT_FILE,
				   FileMetadata.FilCod);
	    break;
	 case Brw_FILE_BRW_COMMON_CRS:
	 case Brw_FILE_BRW_COMMON_GRP:
	    Ntf_SetNotifAsRemoved (Ntf_EVENT_SHARED_FILE,
				   FileMetadata.FilCod);
	    break;
	 case Brw_FILE_BRW_SEE_MARKS_CRS:
	 case Brw_FILE_BRW_SEE_MARKS_GRP:
	 case Brw_FILE_BRW_ADMIN_MARKS_CRS:
	 case Brw_FILE_BRW_ADMIN_MARKS_GRP:
	    Ntf_SetNotifAsRemoved (Ntf_EVENT_MARKS_FILE,
				   FileMetadata.FilCod);
	    break;
	 default:
	    break;
	}

      Lay_ShowAlert (Lay_WARNING,Txt_The_file_of_folder_no_longer_exists_or_is_now_hidden);

      /***** Show again the file browser *****/
      Brw_ShowAgainFileBrowserOrWorks ();
     }
  }

/*****************************************************************************/
/*********** Check if I have permission to change file metadata **************/
/*****************************************************************************/

static bool Brw_CheckIfICanEditFileMetadata (long PublisherUsrCod)
  {
   long ZoneUsrCod;

   switch (Gbl.CurrentAct)	// Only in actions where edition is allowed
     {
      case ActReqDatAdmDocCrs:		case ActChgDatAdmDocCrs:
      case ActReqDatAdmDocGrp:		case ActChgDatAdmDocGrp:
      case ActReqDatComCrs:		case ActChgDatComCrs:
      case ActReqDatComGrp:		case ActChgDatComGrp:
      case ActReqDatAsgUsr:		case ActChgDatAsgUsr:
      case ActReqDatAsgCrs:		case ActChgDatAsgCrs:
      case ActReqDatWrkCrs:		case ActChgDatWrkCrs:
      case ActReqDatWrkUsr:		case ActChgDatWrkUsr:
      case ActReqDatAdmMrkCrs:		case ActChgDatAdmMrkCrs:
      case ActReqDatAdmMrkGrp:		case ActChgDatAdmMrkGrp:
      case ActReqDatBrf:		case ActChgDatBrf:
	 if (Gbl.Usrs.Me.UsrDat.UsrCod > 0)						// I am logged
	   {
	    if (PublisherUsrCod > 0)							// The file has publisher
	      {
	       if (PublisherUsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)			// I am the publisher
		  return true;
	      }
	    else									// The file has no publisher
	      {
	       ZoneUsrCod = Brw_GetZoneUsrCod ();
	       if ((ZoneUsrCod <= 0 && Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER) ||	// It's a zone without owner and I am a teacher (I may be the future owner)
		   ZoneUsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)				// I am the owner
		  return true;
	      }
	   }
         break;
      default:
         break;
     }

   return false;
  }

/*****************************************************************************/
/****************** Write link to download a file or link ********************/
/*****************************************************************************/
// FileType can be Brw_IS_FILE or Brw_IS_LINK

static void Brw_WriteBigLinkToDownloadFile (const char *URL,Brw_FileType_t FileType,
                                            const char *FileNameToShow)
  {
   extern const char *Txt_Check_marks_in_file_X;
   extern const char *Txt_Download;

   /***** On the screen a link will be shown to download the file *****/
   if (Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_MARKS_CRS ||
       Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_MARKS_GRP)
     {
      /* Form to see marks */
      switch (Gbl.FileBrowser.Type)
	{
	 case Brw_FILE_BRW_SEE_MARKS_CRS:
	    Act_FormStart (ActSeeMyMrkCrs);
	    break;
	 case Brw_FILE_BRW_SEE_MARKS_GRP:
	    Act_FormStart (ActSeeMyMrkGrp);
            Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
	    break;
	 default:	// Not aplicable here
	    break;
	}
      Brw_ParamListFiles (FileType,
                          Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,
			  Gbl.FileBrowser.FilFolLnkName);

      /* Link begin */
      sprintf (Gbl.Title,Txt_Check_marks_in_file_X,FileNameToShow);
      Act_LinkFormSubmit (Gbl.Title,"FILENAME");
      Brw_PutIconFile (32,FileType,Gbl.FileBrowser.FilFolLnkName);

      /* Name of the file of marks, link end and form end */
      fprintf (Gbl.F.Out,"&nbsp;%s&nbsp;"
			 "<img src=\"%s/grades32x32.gif\" alt=\"%s\" title=\"%s\""
			 " class=\"ICON32x32\" /></a>"
			 "</form>",
	       FileNameToShow,Gbl.Prefs.IconsURL,Gbl.Title,Gbl.Title);
     }
   else
     {
      /* Put anchor and filename */
      fprintf (Gbl.F.Out,"<a href=\"%s\" class=\"FILENAME\""
                         " title=\"%s\" target=\"_blank\">",
	       URL,
	       (FileType == Brw_IS_LINK) ? URL :	// If it's a link, show full URL in title
		                           FileNameToShow);
      Brw_PutIconFile (32,FileType,Gbl.FileBrowser.FilFolLnkName);
      fprintf (Gbl.F.Out,"&nbsp;%s&nbsp;"
			 "<img src=\"%s/%s/download64x64.gif\""
			 " alt=\"%s\" title=\"%s\""
			 " class=\"ICON32x32\">"
			 "</a>",
	       FileNameToShow,
	       Gbl.Prefs.PathIconSet,Cfg_ICON_ACTION,
	       Txt_Download,Txt_Download);
     }
  }

/*****************************************************************************/
/*********************** Write link to download a file ***********************/
/*****************************************************************************/

static void Brw_WriteSmallLinkToDownloadFile (const char *URL,Brw_FileType_t FileType,
                                              const char *FileNameToShow)
  {
   extern const char *Txt_Check_marks_in_file_X;

   /***** On the screen a link will be shown to download the file *****/
   if (Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_MARKS_CRS ||
       Gbl.FileBrowser.Type == Brw_FILE_BRW_SEE_MARKS_GRP)
     {
      /* Form to see marks */
      switch (Gbl.FileBrowser.Type)
	{
	 case Brw_FILE_BRW_SEE_MARKS_CRS:
	    Act_FormStart (ActSeeMyMrkCrs);
	    break;
	 case Brw_FILE_BRW_SEE_MARKS_GRP:
	    Act_FormStart (ActSeeMyMrkGrp);
            Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
	    break;
	 default:	// Not aplicable here
	    break;
	}
      Brw_ParamListFiles (FileType,
                          Gbl.FileBrowser.Priv.PathInTreeExceptFileOrFolder,
			  Gbl.FileBrowser.FilFolLnkName);

      /* Link begin */
      sprintf (Gbl.Title,Txt_Check_marks_in_file_X,FileNameToShow);
      Act_LinkFormSubmit (Gbl.Title,"DAT");

      /* Name of the file of marks, link end and form end */
      fprintf (Gbl.F.Out,"%s</a>"
			 "</form>",
	       FileNameToShow);
     }
   else
      /* Put anchor and filename */
      fprintf (Gbl.F.Out,"<a href=\"%s\" class=\"DAT\" title=\"%s\" target=\"_blank\">%s</a>",
	       URL,FileNameToShow,FileNameToShow);
  }

/*****************************************************************************/
/************************ Get link to download a file ************************/
/*****************************************************************************/

void Brw_GetLinkToDownloadFile (const char *PathInTree,const char *FileName,char *URL)
  {
   char FullPathIncludingFile[PATH_MAX+1];
   FILE *FileURL;
   char URLWithSpaces[PATH_MAX+1];

   /***** Construct absolute path to file in the private directory *****/
   sprintf (FullPathIncludingFile,"%s/%s/%s",
	    Gbl.FileBrowser.Priv.PathAboveRootFolder,
	    PathInTree,FileName);

   if (Str_FileIs (FileName,"url"))	// It's a link (URL inside a .url file)
     {
      /***** Open .url file *****/
      if ((FileURL = fopen (FullPathIncludingFile,"rb")))
	{
	 if (fgets (URLWithSpaces,PATH_MAX,FileURL) == NULL)
	    URLWithSpaces[0] = '\0';
	 /* File is not longer needed  ==> close it */
	 fclose (FileURL);
	}
     }
   else
     {
      /***** Create a temporary public directory used to download files *****/
      Brw_CreateDirDownloadTmp ();

      /***** Create symbolic link from temporary public directory to private file in order to gain access to it for downloading *****/
      Brw_CreateTmpLinkToDownloadFileBrowser (FullPathIncludingFile,FileName);

      /***** Create URL pointing to symbolic link *****/
      sprintf (URLWithSpaces,"%s/%s/%s/%s",
	       Cfg_HTTPS_URL_SWAD_PUBLIC,Cfg_FOLDER_FILE_BROWSER_TMP,
	       Gbl.FileBrowser.TmpPubDir,
	       FileName);
     }

   Str_CopyStrChangingSpaces (URLWithSpaces,URL,PATH_MAX);	// In HTML, URL must have no spaces
  }

/*****************************************************************************/
/**************** Change metadata of a file in a file browser ****************/
/*****************************************************************************/

void Brw_ChgFileMetadata (void)
  {
   extern const char *Txt_The_properties_of_file_X_have_been_saved;
   extern const char *Txt_You_dont_have_permission_to_change_the_properties_of_file_X;
   struct FileMetadata FileMetadata;
   bool PublicFile;
   Brw_License_t License;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get file metadata from database *****/
   Brw_GetFileMetadataByPath (&FileMetadata);
   Brw_GetFileSizeAndDate (&FileMetadata);

   /***** Check if I can change file metadata *****/
   if (Brw_CheckIfICanEditFileMetadata (FileMetadata.PublisherUsrCod))
     {
      /***** Get the new file privacity and license from form *****/
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
         case Brw_FILE_BRW_COMMON_CRS:
            PublicFile = Brw_GetParamPublicFile ();
            License = Brw_GetParLicense ();
            break;
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
         case Brw_FILE_BRW_COMMON_GRP:
         case Brw_FILE_BRW_ASSIGNMENTS_USR:
         case Brw_FILE_BRW_ASSIGNMENTS_CRS:
         case Brw_FILE_BRW_WORKS_USR:
         case Brw_FILE_BRW_WORKS_CRS:
         case Brw_FILE_BRW_BRIEFCASE_USR:
            PublicFile = false;	// Files in these zones can not be public
            License = Brw_GetParLicense ();
            break;
         default:
            PublicFile = false;	// Files in other zones can not be public
            License = Brw_LICENSE_DEFAULT;
            break;
        }

      /***** Change file metadata *****/
      if (FileMetadata.FilCod > 0)	// Entry exists in database
         Brw_ChangeFilePublicInDB (Gbl.Usrs.Me.UsrDat.UsrCod,
                                   Gbl.FileBrowser.Priv.FullPathInTree,
                                   PublicFile,License);
      else				// No entry in database
         Brw_AddPathToDB (Gbl.Usrs.Me.UsrDat.UsrCod,FileMetadata.FileType,
                          Gbl.FileBrowser.Priv.FullPathInTree,
                          PublicFile,License);

      /***** Remove the affected clipboards *****/
      Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
	                            Gbl.Usrs.Me.UsrDat.UsrCod,
	                            Gbl.Usrs.Other.UsrDat.UsrCod);

      /***** Write message of confirmation *****/
      sprintf (Gbl.Message,Txt_The_properties_of_file_X_have_been_saved,
               Gbl.FileBrowser.FilFolLnkName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
   else
     {
      /***** Write message of error *****/
      sprintf (Gbl.Message,Txt_You_dont_have_permission_to_change_the_properties_of_file_X,
               Gbl.FileBrowser.FilFolLnkName);
      Lay_ShowAlert (Lay_ERROR,Gbl.Message);
     }

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/*********** Get parameter with public / private file from form *************/
/*****************************************************************************/

static bool Brw_GetParamPublicFile (void)
  {
   char YN[1+1];

   Par_GetParToText ("PublicFile",YN,1);
   return (Str_ConvertToUpperLetter (YN[0]) == 'Y');
  }

/*****************************************************************************/
/******************** Get parameter with file license ***********************/
/*****************************************************************************/

static Brw_License_t Brw_GetParLicense (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   /* Get file license from form */
   Par_GetParToText ("License",UnsignedStr,10);

   if (sscanf (UnsignedStr,"%u",&UnsignedNum) != 1)
      return Brw_LICENSE_UNKNOWN;

   if (UnsignedNum < Brw_NUM_LICENSES)
      return (Brw_License_t) UnsignedNum;

   return Brw_LICENSE_UNKNOWN;
  }

/*****************************************************************************/
/*********************** Get file code using its path ************************/
/*****************************************************************************/
// Path if the full path in tree
// Example: descarga/folder/file.pdf

long Brw_GetFilCodByPath (const char *Path)
  {
   char Query[256+PATH_MAX];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long FilCod;

   /***** Get metadata of a file from database *****/
   sprintf (Query,"SELECT FilCod FROM files"
                  " WHERE CrsCod='%ld' AND GrpCod='%ld' AND ZoneUsrCod='%ld'"
                  " AND FileBrowser='%u' AND Path='%s'",
            Brw_GetCrsCod (),
            Brw_GetGrpCod (),
            Brw_GetZoneUsrCod (),
            (unsigned) Brw_FileBrowserForDB[Gbl.FileBrowser.Type],
            Path);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get file code"))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get file code (row[0]) */
      FilCod = Str_ConvertStrCodToLongCod (row[0]);
     }
   else
      FilCod = -1L;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return FilCod;
  }

/*****************************************************************************/
/********************* Get file metadata using its path **********************/
/*****************************************************************************/
// This function only gets metadata stored in table files,
// does not get size, time, numviews...

void Brw_GetFileMetadataByPath (struct FileMetadata *FileMetadata)
  {
   char Query[512+PATH_MAX];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;

   /***** Get metadata of a file from database *****/
   sprintf (Query,"SELECT FilCod,CrsCod,GrpCod,ZoneUsrCod,FileBrowser,"
	          "PublisherUsrCod,FileType,Path,Hidden,Public,License"
	          " FROM files"
                  " WHERE CrsCod='%ld' AND GrpCod='%ld' AND ZoneUsrCod='%ld'"
                  " AND FileBrowser='%u' AND Path='%s'",
            Brw_GetCrsCod (),
            Brw_GetGrpCod (),
            Brw_GetZoneUsrCod (),
            (unsigned) Brw_FileBrowserForDB[Gbl.FileBrowser.Type],
            Gbl.FileBrowser.Priv.FullPathInTree);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get file metadata"))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get file code (row[0]) */
      FileMetadata->FilCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get course code (row[1]) */
      FileMetadata->CrsCod = Str_ConvertStrCodToLongCod (row[1]);

      /* Get group code (row[2]) */
      FileMetadata->GrpCod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get the user's code of the owner of a zone of files (row[3]) */
      FileMetadata->ZoneUsrCod = Str_ConvertStrCodToLongCod (row[3]);

      /* Get file browser type in database (row[4]) */
      FileMetadata->FileBrowser = Brw_FILE_BRW_UNKNOWN;
      if (sscanf (row[4],"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < Brw_NUM_TYPES_FILE_BROWSER)
            FileMetadata->FileBrowser = (Brw_FileBrowser_t) UnsignedNum;

      /* Get publisher's code (row[5]) */
      FileMetadata->PublisherUsrCod = Str_ConvertStrCodToLongCod (row[5]);

      /* Get file type (row[6]) */
      FileMetadata->FileType = Brw_IS_UNKNOWN;	// default
      if (sscanf (row[6],"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < Brw_NUM_FILE_TYPES)
	    FileMetadata->FileType = (Brw_FileType_t) UnsignedNum;

      /* Get path (row[7]) */
      strncpy (FileMetadata->Path,row[7],PATH_MAX);
      FileMetadata->Path[PATH_MAX] = '\0';

      /* File is hidden? (row[8]) */
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
            FileMetadata->IsHidden = (Str_ConvertToUpperLetter (row[8][0]) == 'Y');
            break;
         default:
            FileMetadata->IsHidden = false;
            break;
        }

      /* Is a public file? (row[9]) */
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
         case Brw_FILE_BRW_COMMON_CRS:
            FileMetadata->IsPublic = (Str_ConvertToUpperLetter (row[9][0]) == 'Y');
            break;
         default:
            FileMetadata->IsPublic = false;
            break;
        }

      /* Get license (row[10]) */
      FileMetadata->License = Brw_LICENSE_UNKNOWN;
      if (sscanf (row[10],"%u",&UnsignedNum) == 1)
         if (UnsignedNum < Brw_NUM_LICENSES)
            FileMetadata->License = (Brw_License_t) UnsignedNum;
     }
   else
     {
      FileMetadata->FilCod = -1L;
      FileMetadata->CrsCod = -1L;
      FileMetadata->GrpCod = -1L;
      FileMetadata->ZoneUsrCod = -1L;
      FileMetadata->FileBrowser = Brw_FILE_BRW_UNKNOWN;
      FileMetadata->PublisherUsrCod = -1L;
      FileMetadata->FileType = Brw_IS_UNKNOWN;
      FileMetadata->Path[0] = '\0';
      FileMetadata->IsHidden = false;
      FileMetadata->IsPublic = false;
      FileMetadata->License = Brw_LICENSE_DEFAULT;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Fill some values with 0 (unused at this moment) *****/
   FileMetadata->Size = (off_t) 0;
   FileMetadata->Time = (time_t) 0;
   FileMetadata->NumMyViews             =
   FileMetadata->NumPublicViews         =
   FileMetadata->NumViewsFromLoggedUsrs =
   FileMetadata->NumLoggedUsrs          = 0;
  }

/*****************************************************************************/
/********************* Get file metadata using its code **********************/
/*****************************************************************************/
// FileMetadata.FilCod must be filled
// This function only gets metadata stored in table files,
// does not get size, time, numviews...

void Brw_GetFileMetadataByCod (struct FileMetadata *FileMetadata)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;

   /***** Get metadata of a file from database *****/
   sprintf (Query,"SELECT FilCod,CrsCod,GrpCod,ZoneUsrCod,FileBrowser,"
	          "PublisherUsrCod,FileType,Path,Hidden,Public,License"
	          " FROM files"
                  " WHERE FilCod='%ld'",
            FileMetadata->FilCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get file metadata"))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get file code (row[0]) */
      FileMetadata->FilCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get course code (row[1]) */
      FileMetadata->CrsCod = Str_ConvertStrCodToLongCod (row[1]);

      /* Get group code (row[2]) */
      FileMetadata->GrpCod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get the user's code of the owner of a zone of files (row[3]) */
      FileMetadata->ZoneUsrCod = Str_ConvertStrCodToLongCod (row[3]);

      /* Get file browser type in database (row[4]) */
      FileMetadata->FileBrowser = Brw_FILE_BRW_UNKNOWN;
      if (sscanf (row[4],"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < Brw_NUM_TYPES_FILE_BROWSER)
            FileMetadata->FileBrowser = (Brw_FileBrowser_t) UnsignedNum;

      /* Get publisher's code (row[5]) */
      FileMetadata->PublisherUsrCod = Str_ConvertStrCodToLongCod (row[5]);

      /* Get file type (row[6]) */
      FileMetadata->FileType = Brw_IS_UNKNOWN;	// default
      if (sscanf (row[6],"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < Brw_NUM_FILE_TYPES)
	    FileMetadata->FileType = (Brw_FileType_t) UnsignedNum;

      /* Get path (row[7]) */
      strncpy (FileMetadata->Path,row[7],PATH_MAX);
      FileMetadata->Path[PATH_MAX] = '\0';

      /* Is a hidden file? (row[8]) */
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
            FileMetadata->IsHidden = (Str_ConvertToUpperLetter (row[8][0]) == 'Y');
            break;
         default:
            FileMetadata->IsHidden = false;
            break;
        }

      /* Is a public file? (row[9]) */
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
         case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
         case Brw_FILE_BRW_COMMON_CRS:
            FileMetadata->IsPublic = (Str_ConvertToUpperLetter (row[9][0]) == 'Y');
            break;
         default:
            FileMetadata->IsPublic = false;
            break;
        }

      /* Get license (row[10]) */
      FileMetadata->License = Brw_LICENSE_UNKNOWN;
      if (sscanf (row[10],"%u",&UnsignedNum) == 1)
         if (UnsignedNum < Brw_NUM_LICENSES)
            FileMetadata->License = (Brw_License_t) UnsignedNum;
     }
   else
     {
      FileMetadata->FilCod = -1L;
      FileMetadata->CrsCod = -1L;
      FileMetadata->GrpCod = -1L;
      FileMetadata->ZoneUsrCod = -1L;
      FileMetadata->FileBrowser = Brw_FILE_BRW_UNKNOWN;
      FileMetadata->PublisherUsrCod = -1L;
      FileMetadata->FileType = Brw_IS_UNKNOWN;
      FileMetadata->Path[0] = '\0';
      FileMetadata->IsHidden = false;
      FileMetadata->IsPublic = false;
      FileMetadata->License = Brw_LICENSE_DEFAULT;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Fill some values with 0 (unused at this moment) *****/
   FileMetadata->Size = (off_t) 0;
   FileMetadata->Time = (time_t) 0;
   FileMetadata->NumMyViews             =
   FileMetadata->NumPublicViews         =
   FileMetadata->NumViewsFromLoggedUsrs =
   FileMetadata->NumLoggedUsrs          = 0;
  }

/*****************************************************************************/
/*************************** Get file size and date **************************/
/*****************************************************************************/
// Return true if file exists

bool Brw_GetFileSizeAndDate (struct FileMetadata *FileMetadata)
  {
   char Path[PATH_MAX+1];
   struct stat FileStatus;

   sprintf (Path,"%s/%s",Gbl.FileBrowser.Priv.PathAboveRootFolder,
	                 Gbl.FileBrowser.Priv.FullPathInTree);
   if (lstat (Path,&FileStatus))
     {
      // Error on lstat
      FileMetadata->FileType = Brw_IS_UNKNOWN;
      FileMetadata->Size = (off_t) 0;
      FileMetadata->Time = (time_t) 0;
      return false;
     }
   else
     {
      if (S_ISDIR (FileStatus.st_mode))
	 FileMetadata->FileType = Brw_IS_FOLDER;
      else if (S_ISREG (FileStatus.st_mode))
         FileMetadata->FileType = Str_FileIs (Gbl.FileBrowser.Priv.FullPathInTree,"url") ? Brw_IS_LINK :
                                                                                           Brw_IS_FILE;
      else
	 FileMetadata->FileType = Brw_IS_UNKNOWN;
      FileMetadata->Size = FileStatus.st_size;
      FileMetadata->Time = FileStatus.st_mtime;
      return true;
     }
  }

/*****************************************************************************/
/*********************** Get and update views of a file **********************/
/*****************************************************************************/
/*
   Input:  FileMetadata->FilCod
   Output: FileMetadata->NumMyViews
           FileMetadata->NumPublicViews
           FileMetadata->NumViewsFromLoggedUsrs
           FileMetadata->NumLoggedUsrs
*/
void Brw_GetAndUpdateFileViews (struct FileMetadata *FileMetadata)
  {
   if (FileMetadata->FilCod > 0)
     {
      /***** Get file views from logged users *****/
      Brw_GetFileViewsFromLoggedUsrs (FileMetadata);

      /***** Get file views from non logged users *****/
      Brw_GetFileViewsFromNonLoggedUsrs (FileMetadata);

      /***** Get number of my views *****/
      if (Gbl.Usrs.Me.Logged)
        {
         if (FileMetadata->NumViewsFromLoggedUsrs)
            FileMetadata->NumMyViews = Brw_GetFileViewsFromMe (FileMetadata->FilCod);
         else
            FileMetadata->NumMyViews = 0;
        }
      else
         FileMetadata->NumMyViews = FileMetadata->NumPublicViews;

      /***** Update number of my views (if I am not logged, UsrCod == -1L) *****/
      Brw_UpdateFileViews (FileMetadata->NumMyViews,FileMetadata->FilCod);
     }
   else
      FileMetadata->NumMyViews             =
      FileMetadata->NumPublicViews         =
      FileMetadata->NumViewsFromLoggedUsrs =
      FileMetadata->NumLoggedUsrs          = 0;
  }

/*****************************************************************************/
/************************** Update my views of a file ************************/
/*****************************************************************************/

void Brw_UpdateMyFileViews (long FilCod)
  {
   /***** Update number of my views *****/
   Brw_UpdateFileViews (Brw_GetFileViewsFromMe (FilCod),FilCod);
  }

/*****************************************************************************/
/******************** Get file views from logged users ***********************/
/*****************************************************************************/
/*
   Input:  FileMetadata->FilCod
   Output: FileMetadata->NumViewsFromLoggedUsrs
           FileMetadata->NumLoggedUsrs
*/
static void Brw_GetFileViewsFromLoggedUsrs (struct FileMetadata *FileMetadata)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get number total of views from logged users *****/
   sprintf (Query,"SELECT COUNT(DISTINCT UsrCod),SUM(NumViews)"
		  " FROM file_view"
		  " WHERE FilCod='%ld' AND UsrCod>'0'",
	    FileMetadata->FilCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get number of views of a file from logged users"))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get number of distinct users (row[0]) */
      if (sscanf (row[0],"%u",&(FileMetadata->NumLoggedUsrs)) != 1)
	 FileMetadata->NumLoggedUsrs = 0;

      /* Get number of views (row[1]) */
      if (row[1])
	{
	 if (sscanf (row[1],"%u",&(FileMetadata->NumViewsFromLoggedUsrs)) != 1)
	    FileMetadata->NumViewsFromLoggedUsrs = 0;
	}
      else
	 FileMetadata->NumViewsFromLoggedUsrs = 0;
     }
   else
      FileMetadata->NumViewsFromLoggedUsrs = FileMetadata->NumLoggedUsrs = 0;

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Get file views from non logged users *********************/
/*****************************************************************************/
/*
   Input:  FileMetadata->FilCod
   Output: FileMetadata->NumPublicViews
*/
static void Brw_GetFileViewsFromNonLoggedUsrs (struct FileMetadata *FileMetadata)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get number of public views *****/
   sprintf (Query,"SELECT SUM(NumViews) FROM file_view"
		  " WHERE FilCod='%ld' AND UsrCod<='0'",
	    FileMetadata->FilCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get number of public views of a file"))
     {
      /* Get number of public views */
      row = mysql_fetch_row (mysql_res);
      if (row[0])
	{
	 if (sscanf (row[0],"%u",&(FileMetadata->NumPublicViews)) != 1)
	    FileMetadata->NumPublicViews = 0;
	}
      else
	 FileMetadata->NumPublicViews = 0;
     }
   else
      FileMetadata->NumPublicViews = 0;

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************** Get file views from me ***************************/
/*****************************************************************************/

static unsigned Brw_GetFileViewsFromMe (long FilCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumMyViews;

   /***** Get number of my views *****/
   sprintf (Query,"SELECT NumViews FROM file_view"
		  " WHERE FilCod='%ld' AND UsrCod='%ld'",
	    FilCod,Gbl.Usrs.Me.UsrDat.UsrCod);

   if (DB_QuerySELECT (Query,&mysql_res,"can not get my number of views of a file"))
     {
      /* Get number of my views */
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%u",&NumMyViews) != 1)
	 NumMyViews = 0;
     }
   else
      NumMyViews = 0;

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   return NumMyViews;
  }

/*****************************************************************************/
/*************************** Update file views *******************************/
/*****************************************************************************/

static void Brw_UpdateFileViews (unsigned NumViews,long FilCod)
  {
   char Query[256];

   if (NumViews)
     {
      /* Update number of views in database */
      sprintf (Query,"UPDATE file_view SET NumViews=NumViews+1"
		     " WHERE FilCod='%ld' AND UsrCod='%ld'",
	       FilCod,Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryUPDATE (Query,"can not update number of views of a file");
     }
   else	// NumViews == 0
     {
      /* Insert number of views in database */
      sprintf (Query,"INSERT INTO file_view"
		     " (FilCod,UsrCod,NumViews)"
		     " VALUES ('%ld','%ld','1')",
	       FilCod,Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryINSERT (Query,"can not insert number of views of a file");
     }
  }

/*****************************************************************************/
/*********** Check if a folder contains file(s) marked as public *************/
/*****************************************************************************/

static bool Brw_GetIfFolderHasPublicFiles (const char *Path)
  {
   char Query[512+PATH_MAX];

   /***** Get if a file or folder is public from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM files"
                  " WHERE CrsCod='%ld' AND GrpCod='%ld' AND ZoneUsrCod='%ld'"
                  " AND FileBrowser='%u' AND Path LIKE '%s/%%' AND Public='Y'",
            Brw_GetCrsCod (),
            Brw_GetGrpCod (),
            Brw_GetZoneUsrCod (),
            (unsigned) Brw_FileBrowserForDB[Gbl.FileBrowser.Type],
            Path);
   return (DB_QueryCOUNT (Query,"can not check if a folder contains public files") != 0);
  }

/*****************************************************************************/
/************* Check if a folder from a file browser is expanded *************/
/*****************************************************************************/

static bool Brw_GetIfExpandedTree (const char *Path)
  {
   char Query[512+PATH_MAX];
   Brw_FileBrowser_t FileBrowserForExpandedFolders = Brw_FileBrowserForExpandedFolders[Gbl.FileBrowser.Type];

   /***** Get if a folder is expanded from database *****/
   switch (FileBrowserForExpandedFolders)
     {
      case Brw_FILE_BRW_SEE_DOCUMENTS_CRS:
      case Brw_FILE_BRW_COMMON_CRS:
      case Brw_FILE_BRW_SEE_MARKS_CRS:
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_WORKS_USR:
         sprintf (Query,"SELECT COUNT(*) FROM expanded_folders"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'"
                        " AND CrsCod='%ld'"
                        " AND Path='%s/'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  (unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,
                  Path);
         break;
      case Brw_FILE_BRW_SEE_DOCUMENTS_GRP:
      case Brw_FILE_BRW_COMMON_GRP:
      case Brw_FILE_BRW_SEE_MARKS_GRP:
         sprintf (Query,"SELECT COUNT(*) FROM expanded_folders"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'"
                        " AND CrsCod='%ld' AND GrpCod='%ld'"
                        " AND Path='%s/'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  (unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,
                  Gbl.CurrentCrs.Grps.GrpCod,
                  Path);
         break;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         sprintf (Query,"SELECT COUNT(*) FROM expanded_folders"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'"
                        " AND CrsCod='%ld' AND WorksUsrCod='%ld'"
                        " AND Path='%s/'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  (unsigned) FileBrowserForExpandedFolders,
                  Gbl.CurrentCrs.Crs.CrsCod,
                  Gbl.Usrs.Other.UsrDat.UsrCod,
                  Path);
         break;
      case Brw_FILE_BRW_BRIEFCASE_USR:
         sprintf (Query,"SELECT COUNT(*) FROM expanded_folders"
                        " WHERE UsrCod='%ld' AND FileBrowser='%u'"
                        " AND Path='%s/'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  (unsigned) FileBrowserForExpandedFolders,
                  Path);
         break;
      default:
         break;
     }
   return (DB_QueryCOUNT (Query,"can not get check if a folder is expanded") != 0);
  }

/*****************************************************************************/
/***************** Change hiddeness of file in the database ******************/
/*****************************************************************************/

static void Brw_ChangeFileOrFolderHiddenInDB (const char *Path,bool IsHidden)
  {
   char Query[256+PATH_MAX];

   /***** Mark file as hidden in database *****/
   sprintf (Query,"UPDATE files SET Hidden='%c'"
                  " WHERE CrsCod='%ld' AND GrpCod='%ld' AND ZoneUsrCod='%ld'"
                  " AND FileBrowser='%u' AND Path='%s'",
            IsHidden ? 'Y' :
        	       'N',
            Brw_GetCrsCod (),
            Brw_GetGrpCod (),
            Brw_GetZoneUsrCod (),
            (unsigned) Brw_FileBrowserForDB[Gbl.FileBrowser.Type],
            Path);
   DB_QueryUPDATE (Query,"can not change status of a file in database");
  }

/*****************************************************************************/
/******* Change publisher, public and license of file in the database ********/
/*****************************************************************************/

static void Brw_ChangeFilePublicInDB (long PublisherUsrCod,const char *Path,
                                      bool IsPublic,Brw_License_t License)
  {
   char Query[256+PATH_MAX];

   /***** Change publisher, public and license of file in database *****/
   sprintf (Query,"UPDATE files SET PublisherUsrCod='%ld',Public='%c',License='%u'"
                  " WHERE CrsCod='%ld' AND GrpCod='%ld' AND ZoneUsrCod='%ld'"
                  " AND FileBrowser='%u' AND Path='%s'",
            PublisherUsrCod,
            IsPublic ? 'Y' :
        	       'N',
            (unsigned) License,
            Brw_GetCrsCod (),
            Brw_GetGrpCod (),
            Brw_GetZoneUsrCod (),
            (unsigned) Brw_FileBrowserForDB[Gbl.FileBrowser.Type],
            Path);
   DB_QueryUPDATE (Query,"can not change metadata of a file in database");
  }

/*****************************************************************************/
/****************** Get the course code of a zone of files *******************/
/*****************************************************************************/

static long Brw_GetCrsCod (void)
  {
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_BRIEFCASE_USR:
         return -1L;
      default:
         return Gbl.CurrentCrs.Crs.CrsCod;
     }
  }

/*****************************************************************************/
/******************* Get the group code of a zone of files *******************/
/*****************************************************************************/

static long Brw_GetGrpCod (void)
  {
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_BRIEFCASE_USR:
         return -1L;
      default:
         return Gbl.CurrentCrs.Grps.GrpCod;
     }
  }

/*****************************************************************************/
/********** Get the user's code of the owner of a zone of files **************/
/*****************************************************************************/

static long Brw_GetZoneUsrCod (void)
  {
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_WORKS_USR:
      case Brw_FILE_BRW_BRIEFCASE_USR:
         return Gbl.Usrs.Me.UsrDat.UsrCod;
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
      case Brw_FILE_BRW_WORKS_CRS:
         return Gbl.Usrs.Other.UsrDat.UsrCod;
      default:
         return -1L;
     }
  }

/*****************************************************************************/
/**************** Add a path of file/folder to the database ******************/
/*****************************************************************************/

long Brw_AddPathToDB (long PublisherUsrCod,Brw_FileType_t FileType,
                      const char *Path,bool IsPublic,Brw_License_t License)
  {
   char Query[512+PATH_MAX];

   /***** Add path to the database *****/
   sprintf (Query,"INSERT INTO files (CrsCod,GrpCod,ZoneUsrCod,FileBrowser,"
	          "PublisherUsrCod,FileType,Path,Hidden,Public,License)"
                  " VALUES ('%ld','%ld','%ld','%u',"
                  "'%ld','%u','%s','N','%c','%u')",
            Brw_GetCrsCod (),
            Brw_GetGrpCod (),
            Brw_GetZoneUsrCod (),
            (unsigned) Brw_FileBrowserForDB[Gbl.FileBrowser.Type],
            PublisherUsrCod,
            (unsigned) FileType,
            Path,
            IsPublic ? 'Y' :
        	       'N',
            (unsigned) License);
   return DB_QueryINSERTandReturnCode (Query,"can not add path to database");
  }

/*****************************************************************************/
/**************** Remove a file or folder from the database ******************/
/*****************************************************************************/

static void Brw_RemoveOneFileOrFolderFromDB (const char *Path)
  {
   char Query[512+PATH_MAX];
   long CrsCod = Brw_GetCrsCod ();
   long GrpCod = Brw_GetGrpCod ();
   long ZoneUsrCod = Brw_GetZoneUsrCod ();
   Brw_FileBrowser_t FileBrowser = Brw_FileBrowserForDB[Gbl.FileBrowser.Type];

   /***** Set possible notifications as removed.
          Important: do this before removing from files *****/
   Ntf_SetNotifOneFileAsRemoved (CrsCod,GrpCod,FileBrowser,Path);

   /***** Remove from database the entries that store the marks properties *****/
   if (Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_CRS ||
       Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_GRP)
     {
      sprintf (Query,"DELETE FROM marks_properties USING files,marks_properties"
		     " WHERE files.CrsCod='%ld' AND files.GrpCod='%ld'"
		     " AND files.FileBrowser='%u' AND files.Path='%s'"
		     " AND files.FilCod=marks_properties.FilCod",
	       CrsCod,
	       GrpCod,
	       (unsigned) FileBrowser,Path);
      DB_QueryDELETE (Query,"can not remove properties of marks from database");
     }

   /***** Remove from database the entries that store the file views *****/
   sprintf (Query,"DELETE FROM file_view USING file_view,files"
	          " WHERE files.CrsCod='%ld' AND files.GrpCod='%ld' AND files.ZoneUsrCod='%ld'"
	          " AND files.FileBrowser='%u' AND files.Path='%s'"
	          " AND files.FilCod=file_view.FilCod",
            CrsCod,
            GrpCod,
            ZoneUsrCod,
            (unsigned) FileBrowser,Path);
   DB_QueryDELETE (Query,"can not remove file views from database");

   /***** Remove from database the entry that stores the data of a file *****/
   sprintf (Query,"DELETE FROM files"
                  " WHERE CrsCod='%ld' AND GrpCod='%ld' AND ZoneUsrCod='%ld'"
                  " AND FileBrowser='%u' AND Path='%s'",
            CrsCod,
            GrpCod,
            ZoneUsrCod,
            (unsigned) FileBrowser,Path);
   DB_QueryDELETE (Query,"can not remove path from database");
  }

/*****************************************************************************/
/************** Remove children of a folder from the database ****************/
/*****************************************************************************/

static void Brw_RemoveChildrenOfFolderFromDB (const char *Path)
  {
   char Query[512+PATH_MAX];
   long CrsCod = Brw_GetCrsCod ();
   long GrpCod = Brw_GetGrpCod ();
   long ZoneUsrCod = Brw_GetZoneUsrCod ();
   Brw_FileBrowser_t FileBrowser = Brw_FileBrowserForDB[Gbl.FileBrowser.Type];

   /***** Set possible notifications as removed.
          Important: do this before removing from files *****/
   Ntf_SetNotifChildrenOfFolderAsRemoved (CrsCod,GrpCod,FileBrowser,Path);

   /***** Remove from database the entries that store the marks properties *****/
   if (Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_CRS ||
       Gbl.FileBrowser.Type == Brw_FILE_BRW_ADMIN_MARKS_GRP)
     {
      sprintf (Query,"DELETE FROM marks_properties USING files,marks_properties"
		     " WHERE files.CrsCod='%ld' AND files.GrpCod='%ld'"
		     " AND files.FileBrowser='%u' AND files.Path LIKE '%s/%%'"
		     " AND files.FilCod=marks_properties.FilCod",
	       CrsCod,
	       GrpCod,
	       FileBrowser,Path);
      DB_QueryDELETE (Query,"can not remove properties of marks from database");
     }

   /***** Remove from database the entries that store the file views *****/
   sprintf (Query,"DELETE FROM file_view USING file_view,files"
                  " WHERE files.CrsCod='%ld' AND files.GrpCod='%ld' AND files.ZoneUsrCod='%ld'"
                  " AND files.FileBrowser='%u' AND files.Path LIKE '%s/%%'"
	          " AND files.FilCod=file_view.FilCod",
            CrsCod,
            GrpCod,
            ZoneUsrCod,
            FileBrowser,Path);
   DB_QueryDELETE (Query,"can not remove file views from database");

   /***** Remove from database the entries that store the data of files *****/
   sprintf (Query,"DELETE FROM files"
                  " WHERE CrsCod='%ld' AND GrpCod='%ld' AND ZoneUsrCod='%ld'"
                  " AND FileBrowser='%u' AND Path LIKE '%s/%%'",
            CrsCod,
            GrpCod,
            ZoneUsrCod,
            FileBrowser,Path);
   DB_QueryDELETE (Query,"can not remove paths from database");
  }

/*****************************************************************************/
/*************** Rename a file or folder in table of files *******************/
/*****************************************************************************/

static void Brw_RenameOneFolderInDB (const char *OldPath,const char *NewPath)
  {
   char Query[512+PATH_MAX*2];

   /***** Update file or folder in table of common files *****/
   sprintf (Query,"UPDATE files SET Path='%s'"
                  " WHERE CrsCod='%ld' AND GrpCod='%ld' AND ZoneUsrCod='%ld' AND FileBrowser='%u' AND Path='%s'",
            NewPath,
            Brw_GetCrsCod (),
            Brw_GetGrpCod (),
            Brw_GetZoneUsrCod (),
            (unsigned) Brw_FileBrowserForDB[Gbl.FileBrowser.Type],
            OldPath);
   DB_QueryUPDATE (Query,"can not update folder name in a common zone");
  }

/*****************************************************************************/
/************** Rename children of a folder in table of files ****************/
/*****************************************************************************/

static void Brw_RenameChildrenFilesOrFoldersInDB (const char *OldPath,const char *NewPath)
  {
   char Query[512+PATH_MAX*2];
   unsigned StartFinalSubpathNotChanged = strlen (OldPath) + 2;

   /***** Update children of a folder in table of files *****/
   sprintf (Query,"UPDATE files SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
                  " WHERE CrsCod='%ld' AND GrpCod='%ld' AND ZoneUsrCod='%ld'"
                  " AND FileBrowser='%u' AND Path LIKE '%s/%%'",
            NewPath,StartFinalSubpathNotChanged,
            Brw_GetCrsCod (),
            Brw_GetGrpCod (),
            Brw_GetZoneUsrCod (),
            (unsigned) Brw_FileBrowserForDB[Gbl.FileBrowser.Type],
            OldPath);
   DB_QueryUPDATE (Query,"can not rename file or folder names in a common zone");
  }

/*****************************************************************************/
/********** Check if I have permission to modify a file or folder ************/
/*****************************************************************************/

static bool Brw_CheckIfICanEditFileOrFolder (unsigned Level)
  {
   /***** I must be student, teacher, admin or superuser to edit *****/
   if (Gbl.Usrs.Me.MaxRole < Rol_ROLE_STUDENT)
      return false;

   /***** Set depending on browser, level, logged role... *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_COMMON_CRS:
         // Check if I am the publisher of the folder
         return (Level ? Brw_CheckIfIHavePermissionFileOrFolderCommon (-1L) :
                         false);
      case Brw_FILE_BRW_COMMON_GRP:
         // Check if I am the publisher of the folder
         return (Level ? Brw_CheckIfIHavePermissionFileOrFolderCommon (Gbl.CurrentCrs.Grps.GrpCod) :
                         false);
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
         return (Level != 0 &&
                 (Gbl.FileBrowser.Asg.AsgCod < 0 ||	// If folder does not correspond to any assignment
                  (Level > 1 &&
                   !Gbl.FileBrowser.Asg.Hidden &&	// If assignment is visible (not hidden)
                   Gbl.FileBrowser.Asg.ICanDo &&	// If I can do this assignment
                   (Gbl.FileBrowser.Asg.Open || Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_TEACHER))));
      default:
         return (Level != 0 &&
                 Brw_FileBrowserIsEditable[Gbl.FileBrowser.Type]);
     }
  }

/*****************************************************************************/
/**** Check if I have permission to create a file or folder into a folder ****/
/*****************************************************************************/

static bool Brw_CheckIfICanCreateIntoFolder (unsigned Level)
  {
   /***** I must be student, teacher, admin or superuser to edit *****/
   if (Gbl.Usrs.Me.MaxRole < Rol_ROLE_STUDENT)
      return false;

   /***** Have I premission to create/paste a new file or folder into the folder? *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_FILE_BRW_ASSIGNMENTS_USR:
      case Brw_FILE_BRW_ASSIGNMENTS_CRS:
         return (Level != 0 &&
                 (Gbl.FileBrowser.Asg.AsgCod < 0 ||	// If folder does not correspond to any assignment
                  (!Gbl.FileBrowser.Asg.Hidden &&	// If assignment is visible (not hidden)
                   Gbl.FileBrowser.Asg.ICanDo &&	// If I can do this assignment
                   (Gbl.FileBrowser.Asg.Open ||
                    Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_TEACHER))));
      default:
         return Brw_FileBrowserIsEditable[Gbl.FileBrowser.Type];
     }
  }

/*****************************************************************************/
/********** Check if I have permission to modify a file or folder ************/
/********** in the current common zone                            ************/
/*****************************************************************************/
// Returns true if the current user can remove or rename Gbl.FileBrowser.Priv.FullPathInTree, and false if he have not permission
// A user can remove or rename a file if he's the publisher
// A user can remove or rename a folder if he's the unique publisher of all the files and folders in the subtree starting there

static bool Brw_CheckIfIHavePermissionFileOrFolderCommon (long GrpCod)	// Use GrpCod = -1L to select commons files for the whole course
  {
   char Query[512+PATH_MAX*2];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   long PublisherUsrCod = -1L;

   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_STUDENT:	// If I am a student, I can modify the file/folder if I am the publisher
         /***** Get all the distinct publishers of files starting by Gbl.FileBrowser.Priv.FullPathInTree from database *****/
         sprintf (Query,"SELECT DISTINCT(PublisherUsrCod) FROM files"
                        " WHERE CrsCod='%ld' AND GrpCod='%ld' AND FileBrowser='%u'"
                        " AND (Path='%s' OR Path LIKE '%s/%%')",
                  Gbl.CurrentCrs.Crs.CrsCod,
                  GrpCod,
                  (unsigned) Gbl.FileBrowser.Type,
                  Gbl.FileBrowser.Priv.FullPathInTree,
                  Gbl.FileBrowser.Priv.FullPathInTree);
         NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get publishers of files");

         /***** Check all common files that are equal to Gbl.FileBrowser.Priv.FullPathInTree
                or that are under the folder Gbl.FileBrowser.Priv.FullPathInTree *****/
         if (NumRows == 1)	// Get the publisher of the file(s)
           {
            row = mysql_fetch_row (mysql_res);
            PublisherUsrCod = Str_ConvertStrCodToLongCod (row[0]);
           }

         /***** Free structure that stores the query result *****/
         DB_FreeMySQLResult (&mysql_res);

         return (Gbl.Usrs.Me.UsrDat.UsrCod == PublisherUsrCod);	// Am I the publisher of subtree?
      case Rol_ROLE_TEACHER:
      case Rol_ROLE_DEG_ADMIN:
      case Rol_ROLE_SUPERUSER:
         return true;
      default:
         return false;
     }
  }

/*****************************************************************************/
/************ Remove common zones of all the groups of a type ***************/
/*****************************************************************************/

void Brw_RemoveZonesOfGroupsOfType (long GrpTypCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow,NumRows;
   struct GroupData GrpDat;

   /***** Query database *****/
   if ((NumRows = Grp_GetGrpsOfType (GrpTypCod,&mysql_res)))	// If there exists groups...
      for (NumRow = 0;
	   NumRow < NumRows;
	   NumRow++)
        {
	 /* Get next group */
	 row = mysql_fetch_row (mysql_res);

         /* Group code is in row[0] */
         if (sscanf (row[0],"%ld",&(GrpDat.GrpCod)) != 1)
            Lay_ShowErrorAndExit ("Wrong group code.");

         /* Get name and type of the group from database */
         Grp_GetDataOfGroupByCod (&GrpDat);

         /* Remove file zones of this group */
         Brw_RemoveGrpZonesVerbose (&GrpDat);
	}

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Remove file zones of a group and show message ****************/
/*****************************************************************************/

void Brw_RemoveGrpZonesVerbose (struct GroupData *GrpDat)
  {
   extern const char *Txt_File_zones_of_the_group_X_Y_removed;

   /***** Remove group zones and clipboards *****/
   Brw_RemoveGrpZones (Gbl.CurrentCrs.Crs.CrsCod,GrpDat->GrpCod);

   /***** Print message *****/
   sprintf (Gbl.Message,Txt_File_zones_of_the_group_X_Y_removed,
            GrpDat->GrpTypName,GrpDat->GrpName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/********************** Remove file zones of a group ************************/
/*****************************************************************************/

void Brw_RemoveGrpZones (long CrsCod,long GrpCod)
  {
   char PathGrpFileZones[PATH_MAX+1];

   /***** Remove group zones *****/
   sprintf (PathGrpFileZones,"%s/%s/%ld/grp/%ld",
            Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_CRS,CrsCod,GrpCod);
   Brw_RemoveTree (PathGrpFileZones);

   /***** Set notifications about files in this group zone as removed *****/
   Ntf_SetNotifFilesInGroupAsRemoved (CrsCod,GrpCod);

   /***** Remove files in the course from database *****/
   Brw_RemoveFilesFromDB (CrsCod,GrpCod,-1L);

   /***** Remove size of file zones in the group from database *****/
   Brw_RemoveSizeOfFileTreeFromDB (CrsCod,GrpCod,-1L);

   /***** Remove clipboard entries in this group *****/
   Brw_RemoveGrpClipboards (GrpCod);
  }

/*****************************************************************************/
/**************** Remove the works of a user in a course ********************/
/*****************************************************************************/

void Brw_RemoveUsrWorksInCrs (struct UsrData *UsrDat,struct Course *Crs,Cns_QuietOrVerbose_t QuietOrVerbose)
  {
   extern const char *Txt_Works_of_X_in_Y_removed;
   char PathUsrInCrs[PATH_MAX+1];

   /***** Remove the folder for this user inside the course *****/
   sprintf (PathUsrInCrs,"%s/%s/%ld/usr/%02u/%ld",
            Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_CRS,Crs->CrsCod,
            (unsigned) (UsrDat->UsrCod % 100),UsrDat->UsrCod);
   Brw_RemoveTree (PathUsrInCrs);
   // If this was the last user in his/her subfolder ==> the subfolder will be empty

   /***** Remove files in the course from database *****/
   Brw_RemoveFilesFromDB (Crs->CrsCod,-1L,UsrDat->UsrCod);

   /***** Remove size of file zone of the course from database *****/
   Brw_RemoveSizeOfFileTreeFromDB (Crs->CrsCod,-1L,UsrDat->UsrCod);

   /***** Write message *****/
   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_Works_of_X_in_Y_removed,
               UsrDat->FullName,Crs->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
  }

/*****************************************************************************/
/************* Remove the works of a user in all of his courses *************/
/*****************************************************************************/

void Brw_RemoveUsrWorksInAllCrss (struct UsrData *UsrDat,Cns_QuietOrVerbose_t QuietOrVerbose)
  {
   extern const char *Txt_The_works_of_X_have_been_removed_in_a_total_of_Y_of_his_her_Z_courses;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow,NumRows;
   unsigned NumCrssWorksRemoved = 0;
   struct Course Crs;

   /***** Query database *****/
   if ((NumRows = Usr_GetCrssFromUsr (UsrDat->UsrCod,-1L,&mysql_res)) > 0) // If courses found
     {
      /***** Remove the zone of works of the user in the courses he/she belongs to *****/
      for (NumRow = 0;
	   NumRow < NumRows;
	   NumRow++)
	{
	 /* Get the next course */
	 row = mysql_fetch_row (mysql_res);
         Crs.CrsCod = Str_ConvertStrCodToLongCod (row[0]);
         /* Get data of course */
         Crs_GetDataOfCourseByCod (&Crs);
         Brw_RemoveUsrWorksInCrs (UsrDat,&Crs,QuietOrVerbose);
         NumCrssWorksRemoved++;
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Write message *****/
   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_The_works_of_X_have_been_removed_in_a_total_of_Y_of_his_her_Z_courses,
               UsrDat->FullName,NumCrssWorksRemoved,(unsigned) NumRows);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
  }

/*****************************************************************************/
/*********** Put a document or a shared file into a notification *************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error

void Brw_GetNotifDocOrSharedFile (char *SummaryStr,char **ContentStr,
                                  long FilCod,unsigned MaxChars,bool GetContent)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char FullPathInTreeFromDB[PATH_MAX+1];
   char PathUntilFileName[PATH_MAX+1];

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Get subject of message from database *****/
   sprintf (Query,"SELECT Path FROM files WHERE FilCod='%ld'",FilCod);
   if (!mysql_query (&Gbl.mysql,Query))
      if ((mysql_res = mysql_store_result (&Gbl.mysql)) != NULL)
        {
         /***** Result should have a unique row *****/
         if (mysql_num_rows (mysql_res) == 1)
           {
            /***** Get data of this file *****/
            row = mysql_fetch_row (mysql_res);

            /* Path (row[0]) */
            strncpy (FullPathInTreeFromDB,row[0],PATH_MAX);
            FullPathInTreeFromDB[PATH_MAX] = '\0';
            Str_SplitFullPathIntoPathAndFileName (FullPathInTreeFromDB,
        	                                  PathUntilFileName,
        	                                  SummaryStr);
            if (MaxChars)
               Str_LimitLengthHTMLStr (SummaryStr,MaxChars);

            if (GetContent)
              {	// TODO: Put file metadata into content string
	       if ((*ContentStr = (char *) malloc (strlen (FullPathInTreeFromDB)+1)))
		  strcpy (*ContentStr,FullPathInTreeFromDB);
              }
           }

         mysql_free_result (mysql_res);
        }
  }

/*****************************************************************************/
/**************************** List documents found ***************************/
/*****************************************************************************/
// Returns the number of documents found

unsigned Brw_ListDocsFound (const char *Query,const char *Title)
  {
   extern const char *Txt_document;
   extern const char *Txt_documents;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;
   extern const char *Txt_File_zone;
   extern const char *Txt_Document;
   extern const char *Txt_document_hidden;
   extern const char *Txt_documents_hidden;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumDocs;
   unsigned NumDoc;
   unsigned NumDocsNotHidden = 0;
   unsigned NumDocsHidden;

   /***** Query database *****/
   if ((NumDocs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get files")))
     {
      /***** Write heading *****/
      /* Table start */
      Lay_StartRoundFrameTable10 (NULL,2,Title);

      /* Write header with number of documents found */
      fprintf (Gbl.F.Out,"<tr>"
			 "<td colspan=\"5\" class=\"TIT_TBL\""
			 " style=\"text-align:center;\">");
      if (NumDocs == 1)
	 fprintf (Gbl.F.Out,"1 %s",Txt_document);
      else
	 fprintf (Gbl.F.Out,"%u %s",NumDocs,Txt_documents);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");

      /* Heading row */
      fprintf (Gbl.F.Out,"<tr>"
			 "<th class=\"BM\"></th>"
			 "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
			 "%s"
			 "</th>"
			 "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
			 "%s"
			 "</th>"
			 "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
			 "%s"
			 "</th>"
			 "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
			 "%s"
			 "</th>"
			 "</tr>",
	       Txt_Degree,
	       Txt_Course,
	       Txt_File_zone,
	       Txt_Document);

      /***** List documents found *****/
      for (NumDoc = 1;
	   NumDoc <= NumDocs;
	   NumDoc++)
	{
	 /* Get next course */
	 row = mysql_fetch_row (mysql_res);

	 /* Write data of this course */
	 Brw_WriteRowDocData (&NumDocsNotHidden,row);
	}

      /***** Write footer *****/
      /* Number of documents not hidden found */
      fprintf (Gbl.F.Out,"<tr>"
			 "<td colspan=\"5\" class=\"TIT_TBL\""
			 " style=\"text-align:center;\">"
			 "(");
      NumDocsHidden = NumDocs - NumDocsNotHidden;
      if (NumDocsHidden == 1)
	 fprintf (Gbl.F.Out,"1 %s",Txt_document_hidden);
      else
	 fprintf (Gbl.F.Out,"%u %s",NumDocsHidden,Txt_documents_hidden);
      fprintf (Gbl.F.Out,")"
	                 "</td>"
			 "</tr>");

      /* Table end */
      Lay_EndRoundFrameTable10 ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumDocs;
  }

/*****************************************************************************/
/************ Write the data of a document (result of a query) ***************/
/*****************************************************************************/

static void Brw_WriteRowDocData (unsigned *NumDocsNotHidden,MYSQL_ROW row)
  {
   extern const char *Txt_Documents_zone;
   extern const char *Txt_Shared_files_area;
   extern const char *Txt_Assignments_zone;
   extern const char *Txt_Works_zone;
   extern const char *Txt_Marks_zone;
   extern const char *Txt_Private_storage_zone;
   extern const char *Txt_Go_to_X;
   struct FileMetadata FileMetadata;
   long DegCod;
   const char *BgColor;
   const char *Title;
   Act_Action_t Action = ActUnk;				// Initialized to avoid warning
   char PathUntilFileName[PATH_MAX+1];
   char FileName[NAME_MAX+1];
   char FileNameToShow[NAME_MAX+1];

   /***** Get file code (row[0]) and metadata *****/
   FileMetadata.FilCod = Str_ConvertStrCodToLongCod (row[0]);
   Brw_GetFileMetadataByCod (&FileMetadata);

   if (!Brw_CheckIfFileOrFolderIsHidden (&FileMetadata))
     {
      BgColor = (FileMetadata.CrsCod > 0 &&
	         FileMetadata.CrsCod == Gbl.CurrentCrs.Crs.CrsCod) ? VERY_LIGHT_BLUE :
							             Gbl.ColorRows[Gbl.RowEvenOdd];

      /***** Get degree code (row[2]) *****/
      DegCod = Str_ConvertStrCodToLongCod (row[2]);

      /***** Write number of document in this search *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"DAT\" style=\"text-align:right;"
	                 " vertical-align:top; background-color:%s;\">"
	                 "%u"
	                 "</td>",
	       BgColor,++(*NumDocsNotHidden));

      /***** Write degree logo (row[3]), degree short name (row[4]) and centre short name (row[5]) *****/
      fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:left;"
	                 " vertical-align:top; background-color:%s;\">",
	       BgColor);
      if (DegCod > 0)
	{
         Act_FormGoToStart (ActSeeDegInf);
         Deg_PutParamDegCod (DegCod);
         sprintf (Gbl.Title,Txt_Go_to_X,row[5]);
         Act_LinkFormSubmit (Gbl.Title,"DAT");
         Log_DrawLogo (Sco_SCOPE_DEGREE,DegCod,row[4],16,"vertical-align:top;");
	 fprintf (Gbl.F.Out,"&nbsp;%s (%s)</a>"
			    "</form>",
		  row[4],row[5]);
	}
      fprintf (Gbl.F.Out,"</td>");

      /***** Write course short name (row[6]) *****/
      fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:left;"
	                 " vertical-align:top; background-color:%s;\">",
	       BgColor);
      if (FileMetadata.CrsCod > 0)
	{
	 Act_FormGoToStart (ActSeeCrsInf);
	 Crs_PutParamCrsCod (FileMetadata.CrsCod);
	 sprintf (Gbl.Title,Txt_Go_to_X,row[6]);
	 Act_LinkFormSubmit (Gbl.Title,"DAT");
	 fprintf (Gbl.F.Out,"%s</a>"
	                    "</form>",
		  row[6]);
	}
      fprintf (Gbl.F.Out,"</td>");

      /***** Write file zone *****/
      switch (FileMetadata.FileBrowser)
	{
	 case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
	 case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
	    Title = Txt_Documents_zone;
	    break;
	 case Brw_FILE_BRW_COMMON_CRS:
	 case Brw_FILE_BRW_COMMON_GRP:
	    Title = Txt_Shared_files_area;
	    break;
	 case Brw_FILE_BRW_ASSIGNMENTS_USR:
	    Title = Txt_Assignments_zone;
	    break;
	 case Brw_FILE_BRW_WORKS_USR:
	    Title = Txt_Works_zone;
	    break;
	 case Brw_FILE_BRW_ADMIN_MARKS_CRS:
	 case Brw_FILE_BRW_ADMIN_MARKS_GRP:
	    Title = Txt_Marks_zone;
	    break;
	 case Brw_FILE_BRW_BRIEFCASE_USR:
	    Title = Txt_Private_storage_zone;
	    break;
	 default:
	    Title = "";
	    break;
	}
      fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:left;"
	                 " vertical-align:top; background-color:%s;\">"
	                 "%s"
	                 "</td>",
               BgColor,Title);

      /***** Get the name of the file to show *****/
      Str_SplitFullPathIntoPathAndFileName (FileMetadata.Path,
					    PathUntilFileName,
					    FileName);
      Brw_LimitLengthFileNameToShow (FileMetadata.FileType,FileName,FileNameToShow);

      /***** Write file name using path (row[1]) *****/
      fprintf (Gbl.F.Out,"<td class=\"DAT_N\" style=\"text-align:left;"
	                 " vertical-align:top; background-color:%s;\">",
	       BgColor);

      /* Form start */
      switch (FileMetadata.FileBrowser)
	{
	 case Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS:
	    Action = ActReqDatSeeDocCrs;
	    break;
	 case Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP:
	    Action = ActReqDatSeeDocGrp;
	    break;
	 case Brw_FILE_BRW_COMMON_CRS:
	    Action = ActReqDatComCrs;
	    break;
	 case Brw_FILE_BRW_COMMON_GRP:
	    Action = ActReqDatComGrp;
	    break;
	 case Brw_FILE_BRW_WORKS_USR:
	    Action = ActReqDatWrkUsr;
	    break;
	 case Brw_FILE_BRW_ASSIGNMENTS_USR:
	    Action = ActReqDatAsgUsr;
	    break;
	 case Brw_FILE_BRW_ADMIN_MARKS_CRS:
	    Action = ActReqDatAdmMrkCrs;
	    break;
	 case Brw_FILE_BRW_ADMIN_MARKS_GRP:
	    Action = ActReqDatAdmMrkGrp;
	    break;
	 case Brw_FILE_BRW_BRIEFCASE_USR:
	    Action = ActReqDatBrf;
	    break;
	 default:
	    break;
	}
      if (FileMetadata.CrsCod > 0 &&
	  FileMetadata.CrsCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
	{
         Act_FormGoToStart (Action);	// Go to another course
	 Crs_PutParamCrsCod (FileMetadata.CrsCod);
	}
      else
         Act_FormStart (Action);
      if (FileMetadata.GrpCod > 0)
	 Grp_PutParamGrpCod (FileMetadata.GrpCod);
      Brw_PutParamsPathAndFile (FileMetadata.FileType,PathUntilFileName,FileName);

      Act_LinkFormSubmit (FileNameToShow,"DAT_N");

      /* File or folder icon */
      if (FileMetadata.FileType == Brw_IS_FOLDER)
	 /* Icon with folder */
	 fprintf (Gbl.F.Out,"<img src=\"%s/folder-closed16x16.gif\""
			    " alt=\"\" class=\"ICON16x16B\" />",
		  Gbl.Prefs.IconsURL);
      else
	 /* Icon with file type or link */
	 Brw_PutIconFile (16,FileMetadata.FileType,FileName);

      /* File name and end of form */
      fprintf (Gbl.F.Out,"%s"
	                 "</a>"
	                 "</form>"
	                 "</td>"
	                 "</tr>",
	       FileNameToShow);

      Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
     }
  }
