// swad_file_browser.c: file browsers

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <dirent.h>		// For scandir, etc.
#include <errno.h>		// For errno
#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For exit, system, malloc, free, etc
#include <string.h>		// For string functions
#include <time.h>		// For time
#include <sys/types.h>		// For lstat, time_t
#include <sys/stat.h>		// For lstat
#include <unistd.h>		// For access, lstat, getpid, chdir, symlink

#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_file_browser.h"
#include "swad_file_extension.h"
#include "swad_file_MIME.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_logo.h"
#include "swad_mark.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_profile.h"
#include "swad_role.h"
#include "swad_string.h"
#include "swad_timeline.h"
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
   Brw_ICON_TREE_NOTHING,	// No icon to expand/contract a subtree
   Brw_ICON_TREE_EXPAND,	// Icon to expand a contracted subtree
   Brw_ICON_TREE_CONTRACT,	// Icon to contract a expanded subtree
  } Brw_IconTree_t;

struct Brw_NumObjects
  {
   unsigned NumFiles;
   unsigned NumFolds;
   unsigned NumLinks;
  };

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

// Browsers types for database "files" and "file_browser_size" tables
const Brw_FileBrowser_t Brw_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   Brw_UNKNOWN,		// Brw_UNKNOWN        =  0
   Brw_ADMI_DOC_CRS,	// Brw_SHOW_DOC_CRS =  1
   Brw_ADMI_MRK_CRS,	// Brw_SHOW_MRK_CRS =  2
   Brw_ADMI_DOC_CRS,	// Brw_ADMI_DOC_CRS =  3
   Brw_ADMI_SHR_CRS,	// Brw_ADMI_SHR_CRS =  4
   Brw_ADMI_SHR_GRP,	// Brw_ADMI_SHR_GRP =  5
   Brw_ADMI_WRK_USR,	// Brw_ADMI_WRK_USR =  6
   Brw_ADMI_WRK_USR,	// Brw_ADMI_WRK_CRS =  7
   Brw_ADMI_MRK_CRS,	// Brw_ADMI_MRK_CRS =  8
   Brw_ADMI_BRF_USR,	// Brw_ADMI_BRF_USR =  9
   Brw_ADMI_DOC_GRP,	// Brw_SHOW_DOC_GRP = 10
   Brw_ADMI_DOC_GRP,	// Brw_ADMI_DOC_GRP = 11
   Brw_ADMI_MRK_GRP,	// Brw_SHOW_MRK_GRP = 12
   Brw_ADMI_MRK_GRP,	// Brw_ADMI_MRK_GRP = 13
   Brw_ADMI_ASG_USR,	// Brw_ADMI_ASG_USR = 14
   Brw_ADMI_ASG_USR,	// Brw_ADMI_ASG_CRS = 15
   Brw_ADMI_DOC_DEG,	// Brw_SHOW_DOC_DEG = 16
   Brw_ADMI_DOC_DEG,	// Brw_ADMI_DOC_DEG = 17
   Brw_ADMI_DOC_CTR,	// Brw_SHOW_DOC_CTR = 18
   Brw_ADMI_DOC_CTR,	// Brw_ADMI_DOC_CTR = 19
   Brw_ADMI_DOC_INS,	// Brw_SHOW_DOC_INS = 20
   Brw_ADMI_DOC_INS,	// Brw_ADMI_DOC_INS = 21
   Brw_ADMI_SHR_DEG,	// Brw_ADMI_SHR_DEG = 22
   Brw_ADMI_SHR_CTR,	// Brw_ADMI_SHR_CTR = 23
   Brw_ADMI_SHR_INS,	// Brw_ADMI_SHR_INS = 24
   Brw_ADMI_TCH_CRS,	// Brw_ADMI_TCH_CRS = 25
   Brw_ADMI_TCH_GRP,	// Brw_ADMI_TCH_GRP = 26
   Brw_ADMI_DOC_PRJ,	// Brw_ADMI_DOC_PRJ = 27
   Brw_ADMI_ASS_PRJ,	// Brw_ADMI_ASS_PRJ = 28
  };
// Browsers viewable shown in search for documents
const Brw_FileBrowser_t Brw_FileBrowserForFoundDocs[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   Brw_UNKNOWN,		// Brw_UNKNOWN        =  0
   Brw_SHOW_DOC_CRS,	// Brw_SHOW_DOC_CRS =  1
   Brw_SHOW_MRK_CRS,	// Brw_SHOW_MRK_CRS =  2
   Brw_SHOW_DOC_CRS,	// Brw_ADMI_DOC_CRS =  3
   Brw_ADMI_SHR_CRS,	// Brw_ADMI_SHR_CRS =  4
   Brw_ADMI_SHR_GRP,	// Brw_ADMI_SHR_GRP =  5
   Brw_ADMI_WRK_USR,	// Brw_ADMI_WRK_USR =  6
   Brw_ADMI_WRK_USR,	// Brw_ADMI_WRK_CRS =  7
   Brw_ADMI_MRK_CRS,	// Brw_ADMI_MRK_CRS =  8
   Brw_ADMI_BRF_USR,	// Brw_ADMI_BRF_USR =  9
   Brw_SHOW_DOC_GRP,	// Brw_SHOW_DOC_GRP = 10
   Brw_SHOW_DOC_GRP,	// Brw_ADMI_DOC_GRP = 11
   Brw_SHOW_MRK_GRP,	// Brw_SHOW_MRK_GRP = 12
   Brw_SHOW_MRK_GRP,	// Brw_ADMI_MRK_GRP = 13
   Brw_ADMI_ASG_USR,	// Brw_ADMI_ASG_USR = 14
   Brw_ADMI_ASG_USR,	// Brw_ADMI_ASG_CRS = 15
   Brw_SHOW_DOC_DEG,	// Brw_SHOW_DOC_DEG = 16
   Brw_SHOW_DOC_DEG,	// Brw_ADMI_DOC_DEG = 17
   Brw_SHOW_DOC_CTR,	// Brw_SHOW_DOC_CTR = 18
   Brw_SHOW_DOC_CTR,	// Brw_ADMI_DOC_CTR = 19
   Brw_SHOW_DOC_INS,	// Brw_SHOW_DOC_INS = 20
   Brw_SHOW_DOC_INS,	// Brw_ADMI_DOC_INS = 21
   Brw_ADMI_SHR_DEG,	// Brw_ADMI_SHR_DEG = 22
   Brw_ADMI_SHR_CTR,	// Brw_ADMI_SHR_CTR = 23
   Brw_ADMI_SHR_INS,	// Brw_ADMI_SHR_INS = 24
   Brw_ADMI_TCH_CRS,	// Brw_ADMI_TCH_CRS = 25
   Brw_ADMI_TCH_GRP,	// Brw_ADMI_TCH_GRP = 26
   Brw_ADMI_DOC_PRJ,	// Brw_ADMI_DOC_PRJ = 27
   Brw_ADMI_ASS_PRJ,	// Brw_ADMI_ASS_PRJ = 28
  };
// Browsers types for database "clipboard" table
static const Brw_FileBrowser_t Brw_FileBrowserForDB_clipboard[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   Brw_UNKNOWN,		// Brw_UNKNOWN        =  0
   Brw_ADMI_DOC_CRS,	// Brw_SHOW_DOC_CRS =  1
   Brw_ADMI_MRK_CRS,	// Brw_SHOW_MRK_CRS =  2
   Brw_ADMI_DOC_CRS,	// Brw_ADMI_DOC_CRS =  3
   Brw_ADMI_SHR_CRS,	// Brw_ADMI_SHR_CRS =  4
   Brw_ADMI_SHR_GRP,	// Brw_ADMI_SHR_GRP =  5
   Brw_ADMI_WRK_USR,	// Brw_ADMI_WRK_USR =  6
   Brw_ADMI_WRK_CRS,	// Brw_ADMI_WRK_CRS =  7
   Brw_ADMI_MRK_CRS,	// Brw_ADMI_MRK_CRS =  8
   Brw_ADMI_BRF_USR,	// Brw_ADMI_BRF_USR =  9
   Brw_ADMI_DOC_GRP,	// Brw_SHOW_DOC_GRP = 10
   Brw_ADMI_DOC_GRP,	// Brw_ADMI_DOC_GRP = 11
   Brw_ADMI_MRK_GRP,	// Brw_SHOW_MRK_GRP = 12
   Brw_ADMI_MRK_GRP,	// Brw_ADMI_MRK_GRP = 13
   Brw_ADMI_ASG_USR,	// Brw_ADMI_ASG_USR = 14
   Brw_ADMI_ASG_CRS,	// Brw_ADMI_ASG_CRS = 15
   Brw_ADMI_DOC_DEG,	// Brw_SHOW_DOC_DEG = 16
   Brw_ADMI_DOC_DEG,	// Brw_ADMI_DOC_DEG = 17
   Brw_ADMI_DOC_CTR,	// Brw_SHOW_DOC_CTR = 18
   Brw_ADMI_DOC_CTR,	// Brw_ADMI_DOC_CTR = 19
   Brw_ADMI_DOC_INS,	// Brw_SHOW_DOC_INS = 20
   Brw_ADMI_DOC_INS,	// Brw_ADMI_DOC_INS = 21
   Brw_ADMI_SHR_DEG,	// Brw_ADMI_SHR_DEG = 22
   Brw_ADMI_SHR_CTR,	// Brw_ADMI_SHR_CTR = 23
   Brw_ADMI_SHR_INS,	// Brw_ADMI_SHR_INS = 24
   Brw_ADMI_TCH_CRS,	// Brw_ADMI_TCH_CRS = 25
   Brw_ADMI_TCH_GRP,	// Brw_ADMI_TCH_GRP = 26
   Brw_ADMI_DOC_PRJ,	// Brw_ADMI_DOC_PRJ = 27
   Brw_ADMI_ASS_PRJ,	// Brw_ADMI_ASS_PRJ = 28
  };
// Browsers types for database "expanded_folders" table
static const Brw_FileBrowser_t Brw_FileBrowserForDB_expanded_folders[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   Brw_UNKNOWN,		// Brw_UNKNOWN        =  0
   Brw_ADMI_DOC_CRS,	// Brw_SHOW_DOC_CRS =  1
   Brw_ADMI_MRK_CRS,	// Brw_SHOW_MRK_CRS =  2
   Brw_ADMI_DOC_CRS,	// Brw_ADMI_DOC_CRS =  3
   Brw_ADMI_SHR_CRS,	// Brw_ADMI_SHR_CRS =  4
   Brw_ADMI_SHR_GRP,	// Brw_ADMI_SHR_GRP =  5
   Brw_ADMI_WRK_USR,	// Brw_ADMI_WRK_USR =  6
   Brw_ADMI_WRK_CRS,	// Brw_ADMI_WRK_CRS =  7
   Brw_ADMI_MRK_CRS,	// Brw_ADMI_MRK_CRS =  8
   Brw_ADMI_BRF_USR,	// Brw_ADMI_BRF_USR =  9
   Brw_ADMI_DOC_GRP,	// Brw_SHOW_DOC_GRP = 10
   Brw_ADMI_DOC_GRP,	// Brw_ADMI_DOC_GRP = 11
   Brw_ADMI_MRK_GRP,	// Brw_SHOW_MRK_GRP = 12
   Brw_ADMI_MRK_GRP,	// Brw_ADMI_MRK_GRP = 13
   Brw_ADMI_ASG_USR,	// Brw_ADMI_ASG_USR = 14
   Brw_ADMI_ASG_CRS,	// Brw_ADMI_ASG_CRS = 15
   Brw_ADMI_DOC_DEG,	// Brw_SHOW_DOC_DEG = 16
   Brw_ADMI_DOC_DEG,	// Brw_ADMI_DOC_DEG = 17
   Brw_ADMI_DOC_CTR,	// Brw_SHOW_DOC_CTR = 18
   Brw_ADMI_DOC_CTR,	// Brw_ADMI_DOC_CTR = 19
   Brw_ADMI_DOC_INS,	// Brw_SHOW_DOC_INS = 20
   Brw_ADMI_DOC_INS,	// Brw_ADMI_DOC_INS = 21
   Brw_ADMI_SHR_DEG,	// Brw_ADMI_SHR_DEG = 22
   Brw_ADMI_SHR_CTR,	// Brw_ADMI_SHR_CTR = 23
   Brw_ADMI_SHR_INS,	// Brw_ADMI_SHR_INS = 24
   Brw_ADMI_TCH_CRS,	// Brw_ADMI_TCH_CRS = 25
   Brw_ADMI_TCH_GRP,	// Brw_ADMI_TCH_GRP = 26
   Brw_ADMI_DOC_PRJ,	// Brw_ADMI_DOC_PRJ = 27
   Brw_ADMI_ASS_PRJ,	// Brw_ADMI_ASS_PRJ = 28
  };
// Browsers types for database "file_browser_last" table
// Assignments and works are stored as one in file_browser_last...
// ...because a user views them at the same time
static const Brw_FileBrowser_t Brw_FileBrowserForDB_file_browser_last[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   Brw_UNKNOWN,		// Brw_UNKNOWN        =  0
   Brw_ADMI_DOC_CRS,	// Brw_SHOW_DOC_CRS =  1
   Brw_ADMI_MRK_CRS,	// Brw_SHOW_MRK_CRS =  2
   Brw_ADMI_DOC_CRS,	// Brw_ADMI_DOC_CRS =  3
   Brw_ADMI_SHR_CRS,	// Brw_ADMI_SHR_CRS =  4
   Brw_ADMI_SHR_GRP,	// Brw_ADMI_SHR_GRP =  5
   Brw_ADMI_ASG_USR,	// Brw_ADMI_WRK_USR =  6
   Brw_ADMI_ASG_CRS,	// Brw_ADMI_WRK_CRS =  7
   Brw_ADMI_MRK_CRS,	// Brw_ADMI_MRK_CRS =  8
   Brw_ADMI_BRF_USR,	// Brw_ADMI_BRF_USR =  9
   Brw_ADMI_DOC_GRP,	// Brw_SHOW_DOC_GRP = 10
   Brw_ADMI_DOC_GRP,	// Brw_ADMI_DOC_GRP = 11
   Brw_ADMI_MRK_GRP,	// Brw_SHOW_MRK_GRP = 12
   Brw_ADMI_MRK_GRP,	// Brw_ADMI_MRK_GRP = 13
   Brw_ADMI_ASG_USR,	// Brw_ADMI_ASG_USR = 14
   Brw_ADMI_ASG_CRS,	// Brw_ADMI_ASG_CRS = 15
   Brw_ADMI_DOC_DEG,	// Brw_SHOW_DOC_DEG = 16
   Brw_ADMI_DOC_DEG,	// Brw_ADMI_DOC_DEG = 17
   Brw_ADMI_DOC_CTR,	// Brw_SHOW_DOC_CTR = 18
   Brw_ADMI_DOC_CTR,	// Brw_ADMI_DOC_CTR = 19
   Brw_ADMI_DOC_INS,	// Brw_SHOW_DOC_INS = 20
   Brw_ADMI_DOC_INS,	// Brw_ADMI_DOC_INS = 21
   Brw_ADMI_SHR_DEG,	// Brw_ADMI_SHR_DEG = 22
   Brw_ADMI_SHR_CTR,	// Brw_ADMI_SHR_CTR = 23
   Brw_ADMI_SHR_INS,	// Brw_ADMI_SHR_INS = 24
   Brw_ADMI_TCH_CRS,	// Brw_ADMI_TCH_CRS = 25
   Brw_ADMI_TCH_GRP,	// Brw_ADMI_TCH_GRP = 26
   Brw_ADMI_DOC_PRJ,	// Brw_ADMI_DOC_PRJ = 27
   Brw_ADMI_ASS_PRJ,	// Brw_ADMI_ASS_PRJ = 28
  };

// Internal names of root folders
const char *Brw_RootFolderInternalNames[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   "",							// Brw_UNKNOWN
   Brw_INTERNAL_NAME_ROOT_FOLDER_DOWNLOAD,		// Brw_SHOW_DOC_CRS
   Brw_INTERNAL_NAME_ROOT_FOLDER_MARKS,			// Brw_SHOW_MRK_CRS
   Brw_INTERNAL_NAME_ROOT_FOLDER_DOWNLOAD,		// Brw_ADMI_DOC_CRS
   Brw_INTERNAL_NAME_ROOT_FOLDER_SHARED,		// Brw_ADMI_SHR_CRS
   Brw_INTERNAL_NAME_ROOT_FOLDER_SHARED,		// Brw_ADMI_SHR_GRP
   Brw_INTERNAL_NAME_ROOT_FOLDER_WORKS,			// Brw_ADMI_WRK_USR
   Brw_INTERNAL_NAME_ROOT_FOLDER_WORKS,			// Brw_ADMI_WRK_CRS
   Brw_INTERNAL_NAME_ROOT_FOLDER_MARKS,			// Brw_ADMI_MRK_CRS
   Brw_INTERNAL_NAME_ROOT_FOLDER_BRIEF,			// Brw_ADMI_BRF_USR
   Brw_INTERNAL_NAME_ROOT_FOLDER_DOWNLOAD,		// Brw_SHOW_DOC_GRP
   Brw_INTERNAL_NAME_ROOT_FOLDER_DOWNLOAD,		// Brw_ADMI_DOC_GRP
   Brw_INTERNAL_NAME_ROOT_FOLDER_MARKS,			// Brw_SHOW_MRK_GRP
   Brw_INTERNAL_NAME_ROOT_FOLDER_MARKS,			// Brw_ADMI_MRK_GRP
   Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,		// Brw_ADMI_ASG_USR
   Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,		// Brw_ADMI_ASG_CRS
   Brw_INTERNAL_NAME_ROOT_FOLDER_DOCUMENTS,		// Brw_SHOW_DOC_DEG
   Brw_INTERNAL_NAME_ROOT_FOLDER_DOCUMENTS,		// Brw_ADMI_DOC_DEG
   Brw_INTERNAL_NAME_ROOT_FOLDER_DOCUMENTS,		// Brw_SHOW_DOC_CTR
   Brw_INTERNAL_NAME_ROOT_FOLDER_DOCUMENTS,		// Brw_ADMI_DOC_CTR
   Brw_INTERNAL_NAME_ROOT_FOLDER_DOCUMENTS,		// Brw_SHOW_DOC_INS
   Brw_INTERNAL_NAME_ROOT_FOLDER_DOCUMENTS,		// Brw_ADMI_DOC_INS
   Brw_INTERNAL_NAME_ROOT_FOLDER_SHARED_FILES,		// Brw_ADMI_SHR_DEG
   Brw_INTERNAL_NAME_ROOT_FOLDER_SHARED_FILES,		// Brw_ADMI_SHR_CTR
   Brw_INTERNAL_NAME_ROOT_FOLDER_SHARED_FILES,		// Brw_ADMI_SHR_INS
   Brw_INTERNAL_NAME_ROOT_FOLDER_TEACHERS,		// Brw_ADMI_TCH_CRS
   Brw_INTERNAL_NAME_ROOT_FOLDER_TEACHERS,		// Brw_ADMI_TCH_GRP
   Brw_INTERNAL_NAME_ROOT_FOLDER_PROJECT_DOCUMENTS,	// Brw_ADMI_DOC_PRJ
   Brw_INTERNAL_NAME_ROOT_FOLDER_PROJECT_ASSESSMENT,	// Brw_ADMI_ASS_PRJ
  };

static const bool Brw_FileBrowserIsEditable[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   false,	// Brw_UNKNOWN
   false,	// Brw_SHOW_DOC_CRS
   false,	// Brw_SHOW_MRK_CRS
   true,	// Brw_ADMI_DOC_CRS
   true,	// Brw_ADMI_SHR_CRS
   true,	// Brw_ADMI_SHR_GRP
   true,	// Brw_ADMI_WRK_USR
   true,	// Brw_ADMI_WRK_CRS
   true,	// Brw_ADMI_MRK_CRS
   true,	// Brw_ADMI_BRF_USR
   false,	// Brw_SHOW_DOC_GRP
   true,	// Brw_ADMI_DOC_GRP
   false,	// Brw_SHOW_MRK_GRP
   true,	// Brw_ADMI_MRK_GRP
   true,	// Brw_ADMI_ASG_USR
   true,	// Brw_ADMI_ASG_CRS
   false,	// Brw_SHOW_DOC_DEG
   true,	// Brw_ADMI_DOC_DEG
   false,	// Brw_SHOW_DOC_CTR
   true,	// Brw_ADMI_DOC_CTR
   false,	// Brw_SHOW_DOC_INS
   true,	// Brw_ADMI_DOC_INS
   true,	// Brw_ADMI_SHR_DEG
   true,	// Brw_ADMI_SHR_CTR
   true,	// Brw_ADMI_SHR_INS
   true,	// Brw_ADMI_TCH_CRS
   true,	// Brw_ADMI_TCH_GRP
   true,	// Brw_ADMI_DOC_PRJ
   true,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActSeeAdm[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActSeeDocCrs,	// Brw_SHOW_DOC_CRS
   ActSeeMrkCrs,	// Brw_SHOW_MRK_CRS
   ActAdmDocCrs,	// Brw_ADMI_DOC_CRS
   ActAdmShaCrs,	// Brw_ADMI_SHR_CRS
   ActAdmShaGrp,	// Brw_ADMI_SHR_GRP
   ActAdmAsgWrkUsr,	// Brw_ADMI_WRK_USR
   ActAdmAsgWrkCrs,	// Brw_ADMI_WRK_CRS
   ActAdmMrkCrs,	// Brw_ADMI_MRK_CRS
   ActAdmBrf,		// Brw_ADMI_BRF_USR
   ActSeeDocGrp,	// Brw_SHOW_DOC_GRP
   ActAdmDocGrp,	// Brw_ADMI_DOC_GRP
   ActSeeMrkGrp,	// Brw_SHOW_MRK_GRP
   ActAdmMrkGrp,	// Brw_ADMI_MRK_GRP
   ActAdmAsgWrkUsr,	// Brw_ADMI_ASG_USR
   ActAdmAsgWrkCrs,	// Brw_ADMI_ASG_CRS
   ActSeeDocDeg,	// Brw_SHOW_DOC_DEG
   ActAdmDocDeg,	// Brw_ADMI_DOC_DEG
   ActSeeDocCtr,	// Brw_SHOW_DOC_CTR
   ActAdmDocCtr,	// Brw_ADMI_DOC_CTR
   ActSeeDocIns,	// Brw_SHOW_DOC_INS
   ActAdmDocIns,	// Brw_ADMI_DOC_INS
   ActAdmShaDeg,	// Brw_ADMI_SHR_DEG
   ActAdmShaCtr,	// Brw_ADMI_SHR_CTR
   ActAdmShaIns,	// Brw_ADMI_SHR_INS
   ActAdmTchCrs,	// Brw_ADMI_TCH_CRS
   ActAdmTchGrp,	// Brw_ADMI_TCH_GRP
   ActAdmDocPrj,	// Brw_ADMI_DOC_PRJ
   ActAdmAssPrj,	// Brw_ADMI_ASS_PRJ
  };

static const Act_Action_t Brw_ActFromSeeToAdm[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActAdmDocCrs,	// Brw_SHOW_DOC_CRS
   ActAdmMrkCrs,	// Brw_SHOW_MRK_CRS
   ActUnk,		// Brw_ADMI_DOC_CRS
   ActUnk,		// Brw_ADMI_SHR_CRS
   ActUnk,		// Brw_ADMI_SHR_GRP
   ActUnk,		// Brw_ADMI_WRK_USR
   ActUnk,		// Brw_ADMI_WRK_CRS
   ActUnk,		// Brw_ADMI_MRK_CRS
   ActUnk,		// Brw_ADMI_BRF_USR
   ActAdmDocGrp,	// Brw_SHOW_DOC_GRP
   ActUnk,		// Brw_ADMI_DOC_GRP
   ActAdmMrkGrp,	// Brw_SHOW_MRK_GRP
   ActUnk,		// Brw_ADMI_MRK_GRP
   ActUnk,		// Brw_ADMI_ASG_USR
   ActUnk,		// Brw_ADMI_ASG_CRS
   ActAdmDocDeg,	// Brw_SHOW_DOC_DEG
   ActUnk,		// Brw_ADMI_DOC_DEG
   ActAdmDocCtr,	// Brw_SHOW_DOC_CTR
   ActUnk,		// Brw_ADMI_DOC_CTR
   ActAdmDocIns,	// Brw_SHOW_DOC_INS
   ActUnk,		// Brw_ADMI_DOC_INS
   ActUnk,		// Brw_ADMI_SHR_DEG
   ActUnk,		// Brw_ADMI_SHR_CTR
   ActUnk,		// Brw_ADMI_SHR_INS
   ActUnk,		// Brw_ADMI_TCH_CRS
   ActUnk,		// Brw_ADMI_TCH_GRP
   ActUnk,		// Brw_ADMI_DOC_PRJ
   ActUnk,		// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActFromAdmToSee[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActSeeDocCrs,	// Brw_ADMI_DOC_CRS
   ActUnk,		// Brw_ADMI_SHR_CRS
   ActUnk,		// Brw_ADMI_SHR_GRP
   ActUnk,		// Brw_ADMI_WRK_USR
   ActUnk,		// Brw_ADMI_WRK_CRS
   ActSeeMrkCrs,	// Brw_ADMI_MRK_CRS
   ActUnk,		// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActSeeDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActSeeMrkGrp,	// Brw_ADMI_MRK_GRP
   ActUnk,		// Brw_ADMI_ASG_USR
   ActUnk,		// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActSeeDocDeg,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActSeeDocCtr,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActSeeDocIns,	// Brw_ADMI_DOC_INS
   ActUnk,		// Brw_ADMI_SHR_DEG
   ActUnk,		// Brw_ADMI_SHR_CTR
   ActUnk,		// Brw_ADMI_SHR_INS
   ActUnk,		// Brw_ADMI_TCH_CRS
   ActUnk,		// Brw_ADMI_TCH_GRP
   ActUnk,		// Brw_ADMI_DOC_PRJ
   ActUnk,		// Brw_ADMI_ASS_PRJ
  };

static const Act_Action_t Brw_ActChgZone[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActChgToSeeDocCrs,	// Brw_SHOW_DOC_CRS
   ActChgToSeeMrk,	// Brw_SHOW_MRK_CRS
   ActChgToAdmDocCrs,	// Brw_ADMI_DOC_CRS
   ActChgToAdmSha,	// Brw_ADMI_SHR_CRS
   ActChgToAdmSha,	// Brw_ADMI_SHR_GRP
   ActUnk,		// Brw_ADMI_WRK_USR
   ActUnk,		// Brw_ADMI_WRK_CRS
   ActChgToAdmMrk,	// Brw_ADMI_MRK_CRS
   ActUnk,		// Brw_ADMI_BRF_USR
   ActChgToSeeDocCrs,	// Brw_SHOW_DOC_GRP
   ActChgToAdmDocCrs,	// Brw_ADMI_DOC_GRP
   ActChgToSeeMrk,	// Brw_SHOW_MRK_GRP
   ActChgToAdmMrk,	// Brw_ADMI_MRK_GRP
   ActUnk,		// Brw_ADMI_ASG_USR
   ActUnk,		// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActUnk,		// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActUnk,		// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActUnk,		// Brw_ADMI_DOC_INS
   ActUnk,		// Brw_ADMI_SHR_DEG
   ActUnk,		// Brw_ADMI_SHR_CTR
   ActUnk,		// Brw_ADMI_SHR_INS
   ActChgToAdmTch,	// Brw_ADMI_TCH_CRS
   ActChgToAdmTch,	// Brw_ADMI_TCH_GRP
   ActUnk,		// Brw_ADMI_DOC_PRJ
   ActUnk,		// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActShow[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActShoDocCrs,	// Brw_ADMI_DOC_CRS
   ActUnk,		// Brw_ADMI_SHR_CRS
   ActUnk,		// Brw_ADMI_SHR_GRP
   ActUnk,		// Brw_ADMI_WRK_USR
   ActUnk,		// Brw_ADMI_WRK_CRS
   ActShoMrkCrs,	// Brw_ADMI_MRK_CRS
   ActUnk,		// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActShoDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActShoMrkGrp,	// Brw_ADMI_MRK_GRP
   ActUnk,		// Brw_ADMI_ASG_USR
   ActUnk,		// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActShoDocDeg,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActShoDocCtr,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActShoDocIns,	// Brw_ADMI_DOC_INS
   ActUnk,		// Brw_ADMI_SHR_DEG
   ActUnk,		// Brw_ADMI_SHR_CTR
   ActUnk,		// Brw_ADMI_SHR_INS
   ActUnk,		// Brw_ADMI_TCH_CRS
   ActUnk,		// Brw_ADMI_TCH_GRP
   ActUnk,		// Brw_ADMI_DOC_PRJ
   ActUnk,		// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActHide[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActHidDocCrs,	// Brw_ADMI_DOC_CRS
   ActUnk,		// Brw_ADMI_SHR_CRS
   ActUnk,		// Brw_ADMI_SHR_GRP
   ActUnk,		// Brw_ADMI_WRK_USR
   ActUnk,		// Brw_ADMI_WRK_CRS
   ActHidMrkCrs,	// Brw_ADMI_MRK_CRS
   ActUnk,		// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActHidDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActHidMrkGrp,	// Brw_ADMI_MRK_GRP
   ActUnk,		// Brw_ADMI_ASG_USR
   ActUnk,		// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActHidDocDeg,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActHidDocCtr,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActHidDocIns,	// Brw_ADMI_DOC_INS
   ActUnk,		// Brw_ADMI_SHR_DEG
   ActUnk,		// Brw_ADMI_SHR_CTR
   ActUnk,		// Brw_ADMI_SHR_INS
   ActUnk,		// Brw_ADMI_TCH_CRS
   ActUnk,		// Brw_ADMI_TCH_GRP
   ActUnk,		// Brw_ADMI_DOC_PRJ
   ActUnk,		// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActReqDatFile[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActReqDatSeeDocCrs,	// Brw_SHOW_DOC_CRS
   ActReqDatSeeMrkCrs,	// Brw_SHOW_MRK_CRS
   ActReqDatAdmDocCrs,	// Brw_ADMI_DOC_CRS
   ActReqDatShaCrs,	// Brw_ADMI_SHR_CRS
   ActReqDatShaGrp,	// Brw_ADMI_SHR_GRP
   ActReqDatWrkUsr,	// Brw_ADMI_WRK_USR
   ActReqDatWrkCrs,	// Brw_ADMI_WRK_CRS
   ActReqDatAdmMrkCrs,	// Brw_ADMI_MRK_CRS
   ActReqDatBrf,	// Brw_ADMI_BRF_USR
   ActReqDatSeeDocGrp,	// Brw_SHOW_DOC_GRP
   ActReqDatAdmDocGrp,	// Brw_ADMI_DOC_GRP
   ActReqDatSeeMrkGrp,	// Brw_SHOW_MRK_GRP
   ActReqDatAdmMrkGrp,	// Brw_ADMI_MRK_GRP
   ActReqDatAsgUsr,	// Brw_ADMI_ASG_USR
   ActReqDatAsgCrs,	// Brw_ADMI_ASG_CRS
   ActReqDatSeeDocDeg,	// Brw_SHOW_DOC_DEG
   ActReqDatAdmDocDeg,	// Brw_ADMI_DOC_DEG
   ActReqDatSeeDocCtr,	// Brw_SHOW_DOC_CTR
   ActReqDatAdmDocCtr,	// Brw_ADMI_DOC_CTR
   ActReqDatSeeDocIns,	// Brw_SHOW_DOC_INS
   ActReqDatAdmDocIns,	// Brw_ADMI_DOC_INS
   ActReqDatShaDeg,	// Brw_ADMI_SHR_DEG
   ActReqDatShaCtr,	// Brw_ADMI_SHR_CTR
   ActReqDatShaIns,	// Brw_ADMI_SHR_INS
   ActReqDatTchCrs,	// Brw_ADMI_TCH_CRS
   ActReqDatTchGrp,	// Brw_ADMI_TCH_GRP
   ActReqDatDocPrj,	// Brw_ADMI_DOC_PRJ
   ActReqDatAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActDowFile[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActDowSeeDocCrs,	// Brw_SHOW_DOC_CRS
   ActSeeMyMrkCrs,	// Brw_SHOW_MRK_CRS
   ActDowAdmDocCrs,	// Brw_ADMI_DOC_CRS
   ActDowShaCrs,	// Brw_ADMI_SHR_CRS
   ActDowShaGrp,	// Brw_ADMI_SHR_GRP
   ActDowWrkUsr,	// Brw_ADMI_WRK_USR
   ActDowWrkCrs,	// Brw_ADMI_WRK_CRS
   ActDowAdmMrkCrs,	// Brw_ADMI_MRK_CRS
   ActDowBrf,		// Brw_ADMI_BRF_USR
   ActDowSeeDocGrp,	// Brw_SHOW_DOC_GRP
   ActDowAdmDocGrp,	// Brw_ADMI_DOC_GRP
   ActSeeMyMrkGrp,	// Brw_SHOW_MRK_GRP
   ActDowAdmMrkGrp,	// Brw_ADMI_MRK_GRP
   ActDowAsgUsr,	// Brw_ADMI_ASG_USR
   ActDowAsgCrs,	// Brw_ADMI_ASG_CRS
   ActDowSeeDocDeg,	// Brw_SHOW_DOC_DEG
   ActDowAdmDocDeg,	// Brw_ADMI_DOC_DEG
   ActDowSeeDocCtr,	// Brw_SHOW_DOC_CTR
   ActDowAdmDocCtr,	// Brw_ADMI_DOC_CTR
   ActDowSeeDocIns,	// Brw_SHOW_DOC_INS
   ActDowAdmDocIns,	// Brw_ADMI_DOC_INS
   ActDowShaDeg,	// Brw_ADMI_SHR_DEG
   ActDowShaCtr,	// Brw_ADMI_SHR_CTR
   ActDowShaIns,	// Brw_ADMI_SHR_INS
   ActDowTchCrs,	// Brw_ADMI_TCH_CRS
   ActDowTchGrp,	// Brw_ADMI_TCH_GRP
   ActDowDocPrj,	// Brw_ADMI_DOC_PRJ
   ActDowAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActAskRemoveFile[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActReqRemFilDocCrs,	// Brw_ADMI_DOC_CRS
   ActReqRemFilShaCrs,	// Brw_ADMI_SHR_CRS
   ActReqRemFilShaGrp,	// Brw_ADMI_SHR_GRP
   ActReqRemFilWrkUsr,	// Brw_ADMI_WRK_USR
   ActReqRemFilWrkCrs,	// Brw_ADMI_WRK_CRS
   ActReqRemFilMrkCrs,	// Brw_ADMI_MRK_CRS
   ActReqRemFilBrf,	// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActReqRemFilDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActReqRemFilMrkGrp,	// Brw_ADMI_MRK_GRP
   ActReqRemFilAsgUsr,	// Brw_ADMI_ASG_USR
   ActReqRemFilAsgCrs,	// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActReqRemFilDocDeg,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActReqRemFilDocCtr,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActReqRemFilDocIns,	// Brw_ADMI_DOC_INS
   ActReqRemFilShaDeg,	// Brw_ADMI_SHR_DEG
   ActReqRemFilShaCtr,	// Brw_ADMI_SHR_CTR
   ActReqRemFilShaIns,	// Brw_ADMI_SHR_INS
   ActReqRemFilTchCrs,	// Brw_ADMI_TCH_CRS
   ActReqRemFilTchGrp,	// Brw_ADMI_TCH_GRP
   ActReqRemFilDocPrj,	// Brw_ADMI_DOC_PRJ
   ActReqRemFilAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActRemoveFile[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActRemFilDocCrs,	// Brw_ADMI_DOC_CRS
   ActRemFilShaCrs,	// Brw_ADMI_SHR_CRS
   ActRemFilShaGrp,	// Brw_ADMI_SHR_GRP
   ActRemFilWrkUsr,	// Brw_ADMI_WRK_USR
   ActRemFilWrkCrs,	// Brw_ADMI_WRK_CRS
   ActRemFilMrkCrs,	// Brw_ADMI_MRK_CRS
   ActRemFilBrf,	// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActRemFilDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActRemFilMrkGrp,	// Brw_ADMI_MRK_GRP
   ActRemFilAsgUsr,	// Brw_ADMI_ASG_USR
   ActRemFilAsgCrs,	// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActRemFilDocDeg,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActRemFilDocCtr,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActRemFilDocIns,	// Brw_ADMI_DOC_INS
   ActRemFilShaDeg,	// Brw_ADMI_SHR_DEG
   ActRemFilShaCtr,	// Brw_ADMI_SHR_CTR
   ActRemFilShaIns,	// Brw_ADMI_SHR_INS
   ActRemFilTchCrs,	// Brw_ADMI_TCH_CRS
   ActRemFilTchGrp,	// Brw_ADMI_TCH_GRP
   ActRemFilDocPrj,	// Brw_ADMI_DOC_PRJ
   ActRemFilAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActRemoveFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActRemFolDocCrs,	// Brw_ADMI_DOC_CRS
   ActRemFolShaCrs,	// Brw_ADMI_SHR_CRS
   ActRemFolShaGrp,	// Brw_ADMI_SHR_GRP
   ActRemFolWrkUsr,	// Brw_ADMI_WRK_USR
   ActRemFolWrkCrs,	// Brw_ADMI_WRK_CRS
   ActRemFolMrkCrs,	// Brw_ADMI_MRK_CRS
   ActRemFolBrf,	// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActRemFolDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActRemFolMrkGrp,	// Brw_ADMI_MRK_GRP
   ActRemFolAsgUsr,	// Brw_ADMI_ASG_USR
   ActRemFolAsgCrs,	// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActRemFolDocDeg,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActRemFolDocCtr,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActRemFolDocIns,	// Brw_ADMI_DOC_INS
   ActRemFolShaDeg,	// Brw_ADMI_SHR_DEG
   ActRemFolShaCtr,	// Brw_ADMI_SHR_CTR
   ActRemFolShaIns,	// Brw_ADMI_SHR_INS
   ActRemFolTchCrs,	// Brw_ADMI_TCH_CRS
   ActRemFolTchGrp,	// Brw_ADMI_TCH_GRP
   ActRemFolDocPrj,	// Brw_ADMI_DOC_PRJ
   ActRemFolAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActRemoveFolderNotEmpty[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActRemTreDocCrs,	// Brw_ADMI_DOC_CRS
   ActRemTreShaCrs,	// Brw_ADMI_SHR_CRS
   ActRemTreShaGrp,	// Brw_ADMI_SHR_GRP
   ActRemTreWrkUsr,	// Brw_ADMI_WRK_USR
   ActRemTreWrkCrs,	// Brw_ADMI_WRK_CRS
   ActRemTreMrkCrs,	// Brw_ADMI_MRK_CRS
   ActRemTreBrf,	// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActRemTreDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActRemTreMrkGrp,	// Brw_ADMI_MRK_GRP
   ActRemTreAsgUsr,	// Brw_ADMI_ASG_USR
   ActRemTreAsgCrs,	// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActRemTreDocDeg,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActRemTreDocCtr,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActRemTreDocIns,	// Brw_ADMI_DOC_INS
   ActRemTreShaDeg,	// Brw_ADMI_SHR_DEG
   ActRemTreShaCtr,	// Brw_ADMI_SHR_CTR
   ActRemTreShaIns,	// Brw_ADMI_SHR_INS
   ActRemTreTchCrs,	// Brw_ADMI_TCH_CRS
   ActRemTreTchGrp,	// Brw_ADMI_TCH_GRP
   ActRemTreDocPrj,	// Brw_ADMI_DOC_PRJ
   ActRemTreAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActCopy[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActCopDocCrs,	// Brw_ADMI_DOC_CRS
   ActCopShaCrs,	// Brw_ADMI_SHR_CRS
   ActCopShaGrp,	// Brw_ADMI_SHR_GRP
   ActCopWrkUsr,	// Brw_ADMI_WRK_USR
   ActCopWrkCrs,	// Brw_ADMI_WRK_CRS
   ActCopMrkCrs,	// Brw_ADMI_MRK_CRS
   ActCopBrf,		// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActCopDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActCopMrkGrp,	// Brw_ADMI_MRK_GRP
   ActCopAsgUsr,	// Brw_ADMI_ASG_USR
   ActCopAsgCrs,	// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActCopDocDeg,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActCopDocCtr,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActCopDocIns,	// Brw_ADMI_DOC_INS
   ActCopShaDeg,	// Brw_ADMI_SHR_DEG
   ActCopShaCtr,	// Brw_ADMI_SHR_CTR
   ActCopShaIns,	// Brw_ADMI_SHR_INS
   ActCopTchCrs,	// Brw_ADMI_TCH_CRS
   ActCopTchGrp,	// Brw_ADMI_TCH_GRP
   ActCopDocPrj,	// Brw_ADMI_DOC_PRJ
   ActCopAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActPaste[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActPasDocCrs,	// Brw_ADMI_DOC_CRS
   ActPasShaCrs,	// Brw_ADMI_SHR_CRS
   ActPasShaGrp,	// Brw_ADMI_SHR_GRP
   ActPasWrkUsr,	// Brw_ADMI_WRK_USR
   ActPasWrkCrs,	// Brw_ADMI_WRK_CRS
   ActPasMrkCrs,	// Brw_ADMI_MRK_CRS
   ActPasBrf,		// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActPasDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActPasMrkGrp,	// Brw_ADMI_MRK_GRP
   ActPasAsgUsr,	// Brw_ADMI_ASG_USR
   ActPasAsgCrs,	// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActPasDocDeg,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActPasDocCtr,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActPasDocIns,	// Brw_ADMI_DOC_INS
   ActPasShaDeg,	// Brw_ADMI_SHR_DEG
   ActPasShaCtr,	// Brw_ADMI_SHR_CTR
   ActPasShaIns,	// Brw_ADMI_SHR_INS
   ActPasTchCrs,	// Brw_ADMI_TCH_CRS
   ActPasTchGrp,	// Brw_ADMI_TCH_GRP
   ActPasDocPrj,	// Brw_ADMI_DOC_PRJ
   ActPasAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActFormCreate[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActFrmCreDocCrs,	// Brw_ADMI_DOC_CRS
   ActFrmCreShaCrs,	// Brw_ADMI_SHR_CRS
   ActFrmCreShaGrp,	// Brw_ADMI_SHR_GRP
   ActFrmCreWrkUsr,	// Brw_ADMI_WRK_USR
   ActFrmCreWrkCrs,	// Brw_ADMI_WRK_CRS
   ActFrmCreMrkCrs,	// Brw_ADMI_MRK_CRS
   ActFrmCreBrf,	// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActFrmCreDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActFrmCreMrkGrp,	// Brw_ADMI_MRK_GRP
   ActFrmCreAsgUsr,	// Brw_ADMI_ASG_USR
   ActFrmCreAsgCrs,	// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActFrmCreDocDeg,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActFrmCreDocCtr,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActFrmCreDocIns,	// Brw_ADMI_DOC_INS
   ActFrmCreShaDeg,	// Brw_ADMI_SHR_DEG
   ActFrmCreShaCtr,	// Brw_ADMI_SHR_CTR
   ActFrmCreShaIns,	// Brw_ADMI_SHR_INS
   ActFrmCreTchCrs,	// Brw_ADMI_TCH_CRS
   ActFrmCreTchGrp,	// Brw_ADMI_TCH_GRP
   ActFrmCreDocPrj,	// Brw_ADMI_DOC_PRJ
   ActFrmCreAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActCreateFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActCreFolDocCrs,	// Brw_ADMI_DOC_CRS
   ActCreFolShaCrs,	// Brw_ADMI_SHR_CRS
   ActCreFolShaGrp,	// Brw_ADMI_SHR_GRP
   ActCreFolWrkUsr,	// Brw_ADMI_WRK_USR
   ActCreFolWrkCrs,	// Brw_ADMI_WRK_CRS
   ActCreFolMrkCrs,	// Brw_ADMI_MRK_CRS
   ActCreFolBrf,	// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActCreFolDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActCreFolMrkGrp,	// Brw_ADMI_MRK_GRP
   ActCreFolAsgUsr,	// Brw_ADMI_ASG_USR
   ActCreFolAsgCrs,	// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActCreFolDocDeg,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActCreFolDocCtr,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActCreFolDocIns,	// Brw_ADMI_DOC_INS
   ActCreFolShaDeg,	// Brw_ADMI_SHR_DEG
   ActCreFolShaCtr,	// Brw_ADMI_SHR_CTR
   ActCreFolShaIns,	// Brw_ADMI_SHR_INS
   ActCreFolTchCrs,	// Brw_ADMI_TCH_CRS
   ActCreFolTchGrp,	// Brw_ADMI_TCH_GRP
   ActCreFolDocPrj,	// Brw_ADMI_DOC_PRJ
   ActCreFolAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActCreateLink[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActCreLnkDocCrs,	// Brw_ADMI_DOC_CRS
   ActCreLnkShaCrs,	// Brw_ADMI_SHR_CRS
   ActCreLnkShaGrp,	// Brw_ADMI_SHR_GRP
   ActCreLnkWrkUsr,	// Brw_ADMI_WRK_USR
   ActCreLnkWrkCrs,	// Brw_ADMI_WRK_CRS
   ActUnk,		// Brw_ADMI_MRK_CRS
   ActCreLnkBrf,	// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActCreLnkDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActUnk,		// Brw_ADMI_MRK_GRP
   ActCreLnkAsgUsr,	// Brw_ADMI_ASG_USR
   ActCreLnkAsgCrs,	// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActCreLnkDocDeg,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActCreLnkDocCtr,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActCreLnkDocIns,	// Brw_ADMI_DOC_INS
   ActCreLnkShaDeg,	// Brw_ADMI_SHR_DEG
   ActCreLnkShaCtr,	// Brw_ADMI_SHR_CTR
   ActCreLnkShaIns,	// Brw_ADMI_SHR_INS
   ActCreLnkTchCrs,	// Brw_ADMI_TCH_CRS
   ActCreLnkTchGrp,	// Brw_ADMI_TCH_GRP
   ActCreLnkDocPrj,	// Brw_ADMI_DOC_PRJ
   ActCreLnkAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActRenameFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActRenFolDocCrs,	// Brw_ADMI_DOC_CRS
   ActRenFolShaCrs,	// Brw_ADMI_SHR_CRS
   ActRenFolShaGrp,	// Brw_ADMI_SHR_GRP
   ActRenFolWrkUsr,	// Brw_ADMI_WRK_USR
   ActRenFolWrkCrs,	// Brw_ADMI_WRK_CRS
   ActRenFolMrkCrs,	// Brw_ADMI_MRK_CRS
   ActRenFolBrf,	// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActRenFolDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActRenFolMrkGrp,	// Brw_ADMI_MRK_GRP
   ActRenFolAsgUsr,	// Brw_ADMI_ASG_USR
   ActRenFolAsgCrs,	// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActRenFolDocDeg,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActRenFolDocCtr,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActRenFolDocIns,	// Brw_ADMI_DOC_INS
   ActRenFolShaDeg,	// Brw_ADMI_SHR_DEG
   ActRenFolShaCtr,	// Brw_ADMI_SHR_CTR
   ActRenFolShaIns,	// Brw_ADMI_SHR_INS
   ActRenFolTchCrs,	// Brw_ADMI_TCH_CRS
   ActRenFolTchGrp,	// Brw_ADMI_TCH_GRP
   ActRenFolDocPrj,	// Brw_ADMI_DOC_PRJ
   ActRenFolAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActUploadFileDropzone[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActRcvFilDocCrsDZ,	// Brw_ADMI_DOC_CRS
   ActRcvFilShaCrsDZ,	// Brw_ADMI_SHR_CRS
   ActRcvFilShaGrpDZ,	// Brw_ADMI_SHR_GRP
   ActRcvFilWrkUsrDZ,	// Brw_ADMI_WRK_USR
   ActRcvFilWrkCrsDZ,	// Brw_ADMI_WRK_CRS
   ActRcvFilMrkCrsDZ,	// Brw_ADMI_MRK_CRS
   ActRcvFilBrfDZ,	// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActRcvFilDocGrpDZ,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActRcvFilMrkGrpDZ,	// Brw_ADMI_MRK_GRP
   ActRcvFilAsgUsrDZ,	// Brw_ADMI_ASG_USR
   ActRcvFilAsgCrsDZ,	// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActRcvFilDocDegDZ,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActRcvFilDocCtrDZ,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActRcvFilDocInsDZ,	// Brw_ADMI_DOC_INS
   ActRcvFilShaDegDZ,	// Brw_ADMI_SHR_DEG
   ActRcvFilShaCtrDZ,	// Brw_ADMI_SHR_CTR
   ActRcvFilShaInsDZ,	// Brw_ADMI_SHR_INS
   ActRcvFilTchCrsDZ,	// Brw_ADMI_TCH_CRS
   ActRcvFilTchGrpDZ,	// Brw_ADMI_TCH_GRP
   ActRcvFilDocPrjDZ,	// Brw_ADMI_DOC_PRJ
   ActRcvFilAssPrjDZ,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActUploadFileClassic[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActRcvFilDocCrsCla,	// Brw_ADMI_DOC_CRS
   ActRcvFilShaCrsCla,	// Brw_ADMI_SHR_CRS
   ActRcvFilShaGrpCla,	// Brw_ADMI_SHR_GRP
   ActRcvFilWrkUsrCla,	// Brw_ADMI_WRK_USR
   ActRcvFilWrkCrsCla,	// Brw_ADMI_WRK_CRS
   ActRcvFilMrkCrsCla,	// Brw_ADMI_MRK_CRS
   ActRcvFilBrfCla,	// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActRcvFilDocGrpCla,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActRcvFilMrkGrpCla,	// Brw_ADMI_MRK_GRP
   ActRcvFilAsgUsrCla,	// Brw_ADMI_ASG_USR
   ActRcvFilAsgCrsCla,	// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActRcvFilDocDegCla,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActRcvFilDocCtrCla,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActRcvFilDocInsCla,	// Brw_ADMI_DOC_INS
   ActRcvFilShaDegCla,	// Brw_ADMI_SHR_DEG
   ActRcvFilShaCtrCla,	// Brw_ADMI_SHR_CTR
   ActRcvFilShaInsCla,	// Brw_ADMI_SHR_INS
   ActRcvFilTchCrsCla,	// Brw_ADMI_TCH_CRS
   ActRcvFilTchGrpCla,	// Brw_ADMI_TCH_GRP
   ActRcvFilDocPrjCla,	// Brw_ADMI_DOC_PRJ
   ActRcvFilAssPrjCla,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActRefreshAfterUploadFiles[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActAdmDocCrs,	// Brw_ADMI_DOC_CRS
   ActAdmShaCrs,	// Brw_ADMI_SHR_CRS
   ActAdmShaGrp,	// Brw_ADMI_SHR_GRP
   ActAdmAsgWrkUsr,	// Brw_ADMI_WRK_USR
   ActAdmAsgWrkCrs,	// Brw_ADMI_WRK_CRS
   ActAdmMrkCrs,	// Brw_ADMI_MRK_CRS
   ActAdmBrf,		// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActAdmDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActAdmMrkGrp,	// Brw_ADMI_MRK_GRP
   ActAdmAsgWrkUsr,	// Brw_ADMI_ASG_USR
   ActAdmAsgWrkCrs,	// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActAdmDocDeg,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActAdmDocCtr,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActAdmDocIns,	// Brw_ADMI_DOC_INS
   ActAdmShaDeg,	// Brw_ADMI_SHR_DEG
   ActAdmShaCtr,	// Brw_ADMI_SHR_CTR
   ActAdmShaIns,	// Brw_ADMI_SHR_INS
   ActAdmTchCrs,	// Brw_ADMI_TCH_CRS
   ActAdmTchGrp,	// Brw_ADMI_TCH_GRP
   ActAdmDocPrj,	// Brw_ADMI_DOC_PRJ
   ActAdmAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActExpandFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActExpSeeDocCrs,	// Brw_SHOW_DOC_CRS
   ActExpSeeMrkCrs,	// Brw_SHOW_MRK_CRS
   ActExpAdmDocCrs,	// Brw_ADMI_DOC_CRS
   ActExpShaCrs,	// Brw_ADMI_SHR_CRS
   ActExpShaGrp,	// Brw_ADMI_SHR_GRP
   ActExpWrkUsr,	// Brw_ADMI_WRK_USR
   ActExpWrkCrs,	// Brw_ADMI_WRK_CRS
   ActExpAdmMrkCrs,	// Brw_ADMI_MRK_CRS
   ActExpBrf,		// Brw_ADMI_BRF_USR
   ActExpSeeDocGrp,	// Brw_SHOW_DOC_GRP
   ActExpAdmDocGrp,	// Brw_ADMI_DOC_GRP
   ActExpSeeMrkGrp,	// Brw_SHOW_MRK_GRP
   ActExpAdmMrkGrp,	// Brw_ADMI_MRK_GRP
   ActExpAsgUsr,	// Brw_ADMI_ASG_USR
   ActExpAsgCrs,	// Brw_ADMI_ASG_CRS
   ActExpSeeDocDeg,	// Brw_SHOW_DOC_DEG
   ActExpAdmDocDeg,	// Brw_ADMI_DOC_DEG
   ActExpSeeDocCtr,	// Brw_SHOW_DOC_CTR
   ActExpAdmDocCtr,	// Brw_ADMI_DOC_CTR
   ActExpSeeDocIns,	// Brw_SHOW_DOC_INS
   ActExpAdmDocIns,	// Brw_ADMI_DOC_INS
   ActExpShaDeg,	// Brw_ADMI_SHR_DEG
   ActExpShaCtr,	// Brw_ADMI_SHR_CTR
   ActExpShaIns,	// Brw_ADMI_SHR_INS
   ActExpTchCrs,	// Brw_ADMI_TCH_CRS
   ActExpTchGrp,	// Brw_ADMI_TCH_GRP
   ActExpDocPrj,	// Brw_ADMI_DOC_PRJ
   ActExpAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActContractFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActConSeeDocCrs,	// Brw_SHOW_DOC_CRS
   ActConSeeMrkCrs,	// Brw_SHOW_MRK_CRS
   ActConAdmDocCrs,	// Brw_ADMI_DOC_CRS
   ActConShaCrs,	// Brw_ADMI_SHR_CRS
   ActConShaGrp,	// Brw_ADMI_SHR_GRP
   ActConWrkUsr,	// Brw_ADMI_WRK_USR
   ActConWrkCrs,	// Brw_ADMI_WRK_CRS
   ActConAdmMrkCrs,	// Brw_ADMI_MRK_CRS
   ActConBrf,		// Brw_ADMI_BRF_USR
   ActConSeeDocGrp,	// Brw_SHOW_DOC_GRP
   ActConAdmDocGrp,	// Brw_ADMI_DOC_GRP
   ActConSeeMrkGrp,	// Brw_SHOW_MRK_GRP
   ActConAdmMrkGrp,	// Brw_ADMI_MRK_GRP
   ActConAsgUsr,	// Brw_ADMI_ASG_USR
   ActConAsgCrs,	// Brw_ADMI_ASG_CRS
   ActConSeeDocDeg,	// Brw_SHOW_DOC_DEG
   ActConAdmDocDeg,	// Brw_ADMI_DOC_DEG
   ActConSeeDocCtr,	// Brw_SHOW_DOC_CTR
   ActConAdmDocCtr,	// Brw_ADMI_DOC_CTR
   ActConSeeDocIns,	// Brw_SHOW_DOC_INS
   ActConAdmDocIns,	// Brw_ADMI_DOC_INS
   ActConShaDeg,	// Brw_ADMI_SHR_DEG
   ActConShaCtr,	// Brw_ADMI_SHR_CTR
   ActConShaIns,	// Brw_ADMI_SHR_INS
   ActConTchCrs,	// Brw_ADMI_TCH_CRS
   ActConTchGrp,	// Brw_ADMI_TCH_GRP
   ActConDocPrj,	// Brw_ADMI_DOC_PRJ
   ActConAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActRecDatFile[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActUnk,		// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActChgDatAdmDocCrs,	// Brw_ADMI_DOC_CRS
   ActChgDatShaCrs,	// Brw_ADMI_SHR_CRS
   ActChgDatShaGrp,	// Brw_ADMI_SHR_GRP
   ActChgDatWrkUsr,	// Brw_ADMI_WRK_USR
   ActChgDatWrkCrs,	// Brw_ADMI_WRK_CRS
   ActChgDatAdmMrkCrs,	// Brw_ADMI_MRK_CRS
   ActChgDatBrf,	// Brw_ADMI_BRF_USR
   ActUnk,		// Brw_SHOW_DOC_GRP
   ActChgDatAdmDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActChgDatAdmMrkGrp,	// Brw_ADMI_MRK_GRP
   ActChgDatAsgUsr,	// Brw_ADMI_ASG_USR
   ActChgDatAsgCrs,	// Brw_ADMI_ASG_CRS
   ActUnk,		// Brw_SHOW_DOC_DEG
   ActChgDatAdmDocDeg,	// Brw_ADMI_DOC_DEG
   ActUnk,		// Brw_SHOW_DOC_CTR
   ActChgDatAdmDocCtr,	// Brw_ADMI_DOC_CTR
   ActUnk,		// Brw_SHOW_DOC_INS
   ActChgDatAdmDocIns,	// Brw_ADMI_DOC_INS
   ActChgDatShaDeg,	// Brw_ADMI_SHR_DEG
   ActChgDatShaCtr,	// Brw_ADMI_SHR_CTR
   ActChgDatShaIns,	// Brw_ADMI_SHR_INS
   ActChgDatTchCrs,	// Brw_ADMI_TCH_CRS
   ActChgDatTchGrp,	// Brw_ADMI_TCH_GRP
   ActChgDatDocPrj,	// Brw_ADMI_DOC_PRJ
   ActChgDatAssPrj,	// Brw_ADMI_ASS_PRJ
  };
static const Act_Action_t Brw_ActZIPFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActZIPSeeDocCrs,	// Brw_SHOW_DOC_CRS
   ActUnk,		// Brw_SHOW_MRK_CRS
   ActZIPAdmDocCrs,	// Brw_ADMI_DOC_CRS
   ActZIPShaCrs,	// Brw_ADMI_SHR_CRS
   ActZIPShaGrp,	// Brw_ADMI_SHR_GRP
   ActZIPWrkUsr,	// Brw_ADMI_WRK_USR
   ActZIPWrkCrs,	// Brw_ADMI_WRK_CRS
   ActZIPAdmMrkCrs,	// Brw_ADMI_MRK_CRS
   ActZIPBrf,		// Brw_ADMI_BRF_USR
   ActZIPSeeDocGrp,	// Brw_SHOW_DOC_GRP
   ActZIPAdmDocGrp,	// Brw_ADMI_DOC_GRP
   ActUnk,		// Brw_SHOW_MRK_GRP
   ActZIPAdmMrkGrp,	// Brw_ADMI_MRK_GRP
   ActZIPAsgUsr,	// Brw_ADMI_ASG_USR
   ActZIPAsgCrs,	// Brw_ADMI_ASG_CRS
   ActZIPSeeDocDeg,	// Brw_SHOW_DOC_DEG
   ActZIPAdmDocDeg,	// Brw_ADMI_DOC_DEG
   ActZIPSeeDocCtr,	// Brw_SHOW_DOC_CTR
   ActZIPAdmDocCtr,	// Brw_ADMI_DOC_CTR
   ActZIPSeeDocIns,	// Brw_SHOW_DOC_INS
   ActZIPAdmDocIns,	// Brw_ADMI_DOC_INS
   ActZIPShaDeg,	// Brw_ADMI_SHR_DEG
   ActZIPShaCtr,	// Brw_ADMI_SHR_CTR
   ActZIPShaIns,	// Brw_ADMI_SHR_INS
   ActZIPTchCrs,	// Brw_ADMI_TCH_CRS
   ActZIPTchGrp,	// Brw_ADMI_TCH_GRP
   ActZIPDocPrj,	// Brw_ADMI_DOC_PRJ
   ActZIPAssPrj,	// Brw_ADMI_ASS_PRJ
  };


/* All quotas must be multiple of 1 GiB (Gibibyte)*/
#define Brw_GiB (1024ULL * 1024ULL * 1024ULL)

/* Maximum quotas for each type of file browser */
#define Brw_MAX_QUOTA_DOCUM_INS		(64ULL*Brw_GiB)
#define Brw_MAX_FILES_DOCUM_INS		5000
#define Brw_MAX_FOLDS_DOCUM_INS		1000

#define Brw_MAX_QUOTA_DOCUM_CTR		(64ULL*Brw_GiB)
#define Brw_MAX_FILES_DOCUM_CTR		5000
#define Brw_MAX_FOLDS_DOCUM_CTR		1000

#define Brw_MAX_QUOTA_DOCUM_DEG		(64ULL*Brw_GiB)
#define Brw_MAX_FILES_DOCUM_DEG		5000
#define Brw_MAX_FOLDS_DOCUM_DEG		1000

#define Brw_MAX_QUOTA_DOCUM_CRS		(64ULL*Brw_GiB)
#define Brw_MAX_FILES_DOCUM_CRS		5000
#define Brw_MAX_FOLDS_DOCUM_CRS		1000

#define Brw_MAX_QUOTA_DOCUM_GRP		( 1ULL*Brw_GiB)
#define Brw_MAX_FILES_DOCUM_GRP		1000
#define Brw_MAX_FOLDS_DOCUM_GRP		500

#define Brw_MAX_QUOTA_TEACH_CRS		Brw_MAX_QUOTA_DOCUM_CRS
#define Brw_MAX_FILES_TEACH_CRS		Brw_MAX_FILES_DOCUM_CRS
#define Brw_MAX_FOLDS_TEACH_CRS		Brw_MAX_FOLDS_DOCUM_CRS

#define Brw_MAX_QUOTA_TEACH_GRP		Brw_MAX_QUOTA_DOCUM_GRP
#define Brw_MAX_FILES_TEACH_GRP		Brw_MAX_FILES_DOCUM_GRP
#define Brw_MAX_FOLDS_TEACH_GRP		Brw_MAX_FOLDS_DOCUM_GRP

#define Brw_MAX_QUOTA_SHARE_INS		(64ULL*Brw_GiB)
#define Brw_MAX_FILES_SHARE_INS		5000
#define Brw_MAX_FOLDS_SHARE_INS		1000	// Many, because every student can create his own directories

#define Brw_MAX_QUOTA_SHARE_CTR		(64ULL*Brw_GiB)
#define Brw_MAX_FILES_SHARE_CTR		5000
#define Brw_MAX_FOLDS_SHARE_CTR		1000	// Many, because every student can create his own directories

#define Brw_MAX_QUOTA_SHARE_DEG		(64ULL*Brw_GiB)
#define Brw_MAX_FILES_SHARE_DEG		5000
#define Brw_MAX_FOLDS_SHARE_DEG		1000	// Many, because every student can create his own directories

#define Brw_MAX_QUOTA_SHARE_CRS		(64ULL*Brw_GiB)
#define Brw_MAX_FILES_SHARE_CRS		5000
#define Brw_MAX_FOLDS_SHARE_CRS		1000	// Many, because every student can create his own directories

#define Brw_MAX_QUOTA_SHARE_GRP		( 1ULL*Brw_GiB)
#define Brw_MAX_FILES_SHARE_GRP		1000
#define Brw_MAX_FOLDS_SHARE_GRP		500	// Many, because every student can create his own directories

#define Brw_MAX_QUOTA_ASSIG_PER_STD	( 2ULL*Brw_GiB)
#define Brw_MAX_FILES_ASSIG_PER_STD	500
#define Brw_MAX_FOLDS_ASSIG_PER_STD	50

#define Brw_MAX_QUOTA_WORKS_PER_STD	( 2ULL*Brw_GiB)
#define Brw_MAX_FILES_WORKS_PER_STD	500
#define Brw_MAX_FOLDS_WORKS_PER_STD	50

#define Brw_MAX_QUOTA_DOC_PRJ		( 1ULL*Brw_GiB)
#define Brw_MAX_FILES_DOC_PRJ		500
#define Brw_MAX_FOLDS_DOC_PRJ		50

#define Brw_MAX_QUOTA_ASS_PRJ		( 1ULL*Brw_GiB)
#define Brw_MAX_FILES_ASS_PRJ		200
#define Brw_MAX_FOLDS_ASS_PRJ		20

#define Brw_MAX_QUOTA_MARKS_CRS		( 1ULL*Brw_GiB)
#define Brw_MAX_FILES_MARKS_CRS		500
#define Brw_MAX_FOLDS_MARKS_CRS		50

#define Brw_MAX_QUOTA_MARKS_GRP		( 1ULL*Brw_GiB)
#define Brw_MAX_FILES_MARKS_GRP		200
#define Brw_MAX_FOLDS_MARKS_GRP		20

const unsigned long long Brw_MAX_QUOTA_BRIEF[Rol_NUM_ROLES] =	// MaxRole is used
  {
	            0,	// Rol_UNK
	            0,	// Rol_GST
	            0,	// Rol_USR
	32ULL*Brw_GiB,	// Rol_STD
	32ULL*Brw_GiB,	// Rol_NET
	64ULL*Brw_GiB,	// Rol_TCH
	            0,	// Rol_DEG_ADM
	            0,	// Rol_CTR_ADM
	            0,	// Rol_INS_ADM
	            0,	// Rol_SYS_ADM
  };
#define Brw_MAX_FILES_BRIEF	5000
#define Brw_MAX_FOLDS_BRIEF	1000

/*****************************************************************************/
/**************************** Internal variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/*************************** Internal prototypes *****************************/
/*****************************************************************************/

static long Brw_GetGrpSettings (void);
static void Brw_GetDataCurrentGrp (void);

static void Brw_PutParamsFileBrowser (const char *PathInTree,const char *FileFolderName,
                                      Brw_FileType_t FileType,long FilCod);
static void Brw_GetParamsPathInTreeAndFileName (void);
static void Brw_SetPathFileBrowser (void);
static void Brw_CreateFoldersAssignmentsIfNotExist (long ZoneUsrCod);
static void Brw_SetAndCheckQuota (void);
static void Brw_SetMaxQuota (void);
static bool Brw_CheckIfQuotaExceded (void);

static void Brw_ShowFileBrowserNormal (void);
static void Brw_ShowFileBrowsersAsgWrkCrs (void);
static void Brw_ShowFileBrowsersAsgWrkUsr (void);

static void Brw_FormToChangeCrsGrpZone (void);
static void Brw_GetSelectedGroupData (struct GroupData *GrpDat,bool AbortOnError);
static void Brw_ShowDataOwnerAsgWrk (struct UsrData *UsrDat);
static void Brw_ShowFileBrowser (void);
static void Brw_PutIconsFileBrowser (void);
static void Brw_PutIconShowFigure (void);
static void Brw_PutButtonToShowEdit (void);
static void Brw_WriteTopBeforeShowingFileBrowser (void);
static void Brw_UpdateLastAccess (void);
static void Brw_UpdateGrpLastAccZone (const char *FieldNameDB,long GrpCod);
static void Brw_WriteSubtitleOfFileBrowser (void);
static void Brw_InitHiddenLevels (void);
static void Brw_ShowAndStoreSizeOfFileTree (void);
static void Brw_StoreSizeOfFileTreeInDB (void);

static void Brw_PutCheckboxFullTree (void);
static void Brw_PutParamsFullTree (void);
static bool Brw_GetFullTreeFromForm (void);

static bool Brw_GetIfGroupFileBrowser (void);
static bool Brw_GetIfProjectFileBrowser (void);
static bool Brw_GetIfBriefcaseFileBrowser (void);
static bool Brw_GetIfUsrAssigWorksFileBrowser (void);
static bool Brw_GetIfCrsAssigWorksFileBrowser (void);

static void Brw_GetAndUpdateDateLastAccFileBrowser (void);
static long Brw_GetGrpLastAccZone (const char *FieldNameDB);
static void Brw_ResetFileBrowserSize (void);
static void Brw_CalcSizeOfDirRecursive (unsigned Level,char *Path);
static void Brw_ListDir (unsigned Level,const char *RowId,
                         bool TreeContracted,
                         const char Path[PATH_MAX + 1],
                         const char PathInTree[PATH_MAX + 1]);
static bool Brw_WriteRowFileBrowser (unsigned Level,const char *RowId,
                                     bool TreeContracted,
                                     Brw_IconTree_t IconThisRow);
static bool Brw_CheckIfCanPasteIn (unsigned Level);
static void Brw_PutIconRemove (void);
static void Brw_PutIconCopy (void);
static void Brw_PutIconPaste (unsigned Level);
static void Brw_IndentAndWriteIconExpandContract (unsigned Level,
                                                  const char *FileBrowserId,const char *RowId,
                                                  Brw_IconTree_t IconThisRow);
static void Brw_IndentDependingOnLevel (unsigned Level);
static void Brw_PutIconToExpandFolder (const char *FileBrowserId,const char *RowId,
                                       bool Hidden);
static void Brw_PutIconToContractFolder (const char *FileBrowserId,const char *RowId,
                                         bool Hidden);

static void Brw_PutIconShow (const char *Anchor);
static void Brw_PutIconHide (const char *Anchor);
static bool Brw_CheckIfAnyUpperLevelIsHidden (unsigned CurrentLevel);

static void Brw_PutIconFolder (unsigned Level,
                               const char *FileBrowserId,const char *RowId,
                               Brw_IconTree_t IconSubtree);
static void Brw_PutIconFolderWithoutPlus (const char *FileBrowserId,const char *RowId,
			                  bool Open,bool Hidden);
static void Brw_PutIconFolderWithPlus (const char *FileBrowserId,const char *RowId,
				       bool Open,bool Hidden);

static void Brw_PutIconNewFileOrFolder (void);
static void Brw_PutIconFileWithLinkToViewMetadata (unsigned Size,
                                                   struct FileMetadata *FileMetadata);
static void Brw_PutIconFile (unsigned Size,Brw_FileType_t FileType,const char *FileName);

static void Brw_PutButtonToDownloadZIPOfAFolder (void);

static void Brw_WriteFileName (unsigned Level,bool IsPublic);
static void Brw_GetFileNameToShowDependingOnLevel (Brw_FileBrowser_t FileBrowser,
                                                   unsigned Level,
                                                   Brw_FileType_t FileType,
                                                   const char *FileName,
                                                   char *FileNameToShow);
static void Brw_GetFileNameToShow (Brw_FileType_t FileType,
                                   const char FileName[NAME_MAX + 1],
                                   char FileNameToShow[NAME_MAX + 1]);
static void Brw_WriteDatesAssignment (void);
static void Brw_WriteFileSizeAndDate (struct FileMetadata *FileMetadata);
static void Brw_WriteFileOrFolderPublisher (unsigned Level,unsigned long UsrCod);
static void Brw_AskConfirmRemoveFolderNotEmpty (void);

static void Brw_WriteCurrentClipboard (void);

static bool Brw_GetMyClipboard (void);
static bool Brw_CheckIfClipboardIsInThisTree (void);
static void Brw_AddPathToClipboards (void);
static void Brw_UpdatePathInClipboard (void);
static long Brw_GetCodForClipboard (void);
static long Brw_GetWorksUsrCodForClipboard (void);

static void Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (const char Path[PATH_MAX + 1]);
static void Brw_RemThisFolderAndUpdOtherFoldersFromExpandedFolders (const char Path[PATH_MAX + 1]);
static void Brw_InsertFolderInExpandedFolders (const char Path[PATH_MAX + 1]);
static void Brw_UpdateClickTimeOfThisFileBrowserInExpandedFolders (void);
static void Brw_RemoveFolderFromExpandedFolders (const char Path[PATH_MAX + 1]);
static void Brw_RemoveAffectedExpandedFolders (const char Path[PATH_MAX + 1]);
static void Brw_RenameAffectedExpandedFolders (Brw_FileBrowser_t FileBrowser,
                                               long MyUsrCod,long WorksUsrCod,
                                               const char *OldPath,const char *NewPath);
static bool Brw_GetIfExpandedTree (const char Path[PATH_MAX + 1]);
static long Brw_GetCodForExpandedFolders (void);
static long Brw_GetWorksUsrCodForExpandedFolders (void);

static void Brw_RemoveExpiredClipboards (void);
static void Brw_RemoveAffectedClipboards (Brw_FileBrowser_t FileBrowser,
                                          long MyUsrCod,long WorksUsrCod);
static void Brw_PasteClipboard (void);
static unsigned Brw_NumLevelsInPath (const char Path[PATH_MAX + 1]);
static bool Brw_PasteTreeIntoFolder (unsigned Level,
                                     const char PathOrg[PATH_MAX + 1],
                                     const char PathDstInTree[PATH_MAX + 1],
                                     struct Brw_NumObjects *Pasted,
                                     long *FirstFilCod);
static void Brw_PutFormToCreateAFolder (const char FileNameToShow[NAME_MAX + 1]);
static void Brw_PutFormToUploadFilesUsingDropzone (const char *FileNameToShow);
static void Brw_PutFormToUploadOneFileClassic (const char *FileNameToShow);
static void Brw_PutFormToPasteAFileOrFolder (const char *FileNameToShow);
static void Brw_PutFormToCreateALink (const char *FileNameToShow);
static bool Brw_RcvFileInFileBrw (Brw_UploadType_t UploadType);
static bool Brw_CheckIfUploadIsAllowed (const char *FileType);

static bool Brw_CheckIfICanEditFileMetadata (long IAmTheOwner);
static bool Brw_CheckIfIAmOwnerOfFile (long PublisherUsrCod);
static void Brw_WriteBigLinkToDownloadFile (const char *URL,
                                            struct FileMetadata *FileMetadata,
                                            const char *FileNameToShow);
static void Brw_WriteSmallLinkToDownloadFile (const char *URL,
	                                      struct FileMetadata *FileMetadata,
                                              const char *FileNameToShow);
static bool Brw_GetParamPublicFile (void);
static Brw_License_t Brw_GetParLicense (void);
static void Brw_GetFileViewsFromLoggedUsrs (struct FileMetadata *FileMetadata);
static void Brw_GetFileViewsFromNonLoggedUsrs (struct FileMetadata *FileMetadata);
static unsigned Brw_GetFileViewsFromMe (long FilCod);
static void Brw_UpdateFileViews (unsigned NumViews,long FilCod);
static bool Brw_GetIfFolderHasPublicFiles (const char Path[PATH_MAX + 1]);

static void Brw_ChangeFileOrFolderHiddenInDB (const char Path[PATH_MAX + 1],bool IsHidden);

static void Brw_ChangeFilePublicInDB (long PublisherUsrCod,const char *Path,
                                      bool IsPublic,Brw_License_t License);

static long Brw_GetZoneUsrCodForFiles (void);

static void Brw_RemoveOneFileOrFolderFromDB (const char Path[PATH_MAX + 1]);
static void Brw_RemoveChildrenOfFolderFromDB (const char Path[PATH_MAX + 1]);
static void Brw_RenameOneFolderInDB (const char OldPath[PATH_MAX + 1],
                                     const char NewPath[PATH_MAX + 1]);
static void Brw_RenameChildrenFilesOrFoldersInDB (const char OldPath[PATH_MAX + 1],
                                                  const char NewPath[PATH_MAX + 1]);
static bool Brw_CheckIfICanEditFileOrFolder (unsigned Level);
static bool Brw_CheckIfICanCreateIntoFolder (unsigned Level);
static bool Brw_CheckIfICanModifySharedFileOrFolder (void);
static bool Brw_CheckIfICanModifyPrivateFileOrFolder (void);
static bool Brw_CheckIfICanViewProjectDocuments (unsigned MyRolesInProject);
static bool Brw_CheckIfICanViewProjectAssessment (unsigned MyRolesInProject);
static bool Brw_CheckIfICanModifyPrjDocFileOrFolder (void);
static bool Brw_CheckIfICanModifyPrjAssFileOrFolder (void);
static long Brw_GetPublisherOfSubtree (void);

static void Brw_WriteRowDocData (unsigned long *NumDocsNotHidden,MYSQL_ROW row);

static void Brw_PutLinkToAskRemOldFiles (void);
static void Brw_RemoveOldFilesInBrowser (unsigned Months,struct Brw_NumObjects *Removed);
static void Brw_ScanDirRemovingOldFiles (unsigned Level,
                                         const char Path[PATH_MAX + 1],
                                         const char PathInTree[PATH_MAX + 1],
                                         time_t TimeRemoveFilesOlder,
                                         struct Brw_NumObjects *Removed);

static void Brw_RemoveFileFromDiskAndDB (const char Path[PATH_MAX + 1],
                                         const char FullPathInTree[PATH_MAX + 1]);
static int Brw_RemoveFolderFromDiskAndDB (const char Path[PATH_MAX + 1],
                                          const char FullPathInTree[PATH_MAX + 1]);

/*****************************************************************************/
/***************** Get parameters related to file browser ********************/
/*****************************************************************************/

void Brw_GetParAndInitFileBrowser (void)
  {
   /***** If a group is selected, get its data *****/
   if ((Gbl.Crs.Grps.GrpCod = Brw_GetGrpSettings ()) > 0)
      Brw_GetDataCurrentGrp ();

   /***** Get type of file browser *****/
   switch (Gbl.Action.Act)
     {
      /***** Documents of institution *****/
      case ActSeeAdmDocIns:	// Access to a documents zone from menu
      case ActChgToSeeDocIns:	// Access to see a documents zone
      case ActSeeDocIns:
      case ActExpSeeDocIns:
      case ActConSeeDocIns:
      case ActZIPSeeDocIns:
      case ActReqDatSeeDocIns:
      case ActDowSeeDocIns:
	 Gbl.FileBrowser.Type = Brw_SHOW_DOC_INS;
         break;
      case ActChgToAdmDocIns:	// Access to admin a documents zone
      case ActAdmDocIns:
      case ActReqRemFilDocIns:
      case ActRemFilDocIns:
      case ActRemFolDocIns:
      case ActCopDocIns:
      case ActPasDocIns:
      case ActRemTreDocIns:
      case ActFrmCreDocIns:
      case ActCreFolDocIns:
      case ActCreLnkDocIns:
      case ActRenFolDocIns:
      case ActRcvFilDocInsDZ:
      case ActRcvFilDocInsCla:
      case ActExpAdmDocIns:
      case ActConAdmDocIns:
      case ActZIPAdmDocIns:
      case ActShoDocIns:
      case ActHidDocIns:
      case ActReqDatAdmDocIns:
      case ActChgDatAdmDocIns:
      case ActDowAdmDocIns:
	 Gbl.FileBrowser.Type = Brw_ADMI_DOC_INS;
         break;

      /***** Shared files of institution *****/
      case ActAdmShaIns:
      case ActReqRemFilShaIns:
      case ActRemFilShaIns:
      case ActRemFolShaIns:
      case ActCopShaIns:
      case ActPasShaIns:
      case ActRemTreShaIns:
      case ActFrmCreShaIns:
      case ActCreFolShaIns:
      case ActCreLnkShaIns:
      case ActRenFolShaIns:
      case ActRcvFilShaInsDZ:
      case ActRcvFilShaInsCla:
      case ActExpShaIns:
      case ActConShaIns:
      case ActZIPShaIns:
      case ActReqDatShaIns:
      case ActChgDatShaIns:
      case ActDowShaIns:
         Gbl.FileBrowser.Type = Brw_ADMI_SHR_INS;
         break;

      /***** Documents of centre *****/
      case ActSeeAdmDocCtr:	// Access to a documents zone from menu
      case ActChgToSeeDocCtr:	// Access to see a documents zone
      case ActSeeDocCtr:
      case ActExpSeeDocCtr:
      case ActConSeeDocCtr:
      case ActZIPSeeDocCtr:
      case ActReqDatSeeDocCtr:
      case ActDowSeeDocCtr:
	 Gbl.FileBrowser.Type = Brw_SHOW_DOC_CTR;
         break;
      case ActChgToAdmDocCtr:	// Access to admin a documents zone
      case ActAdmDocCtr:
      case ActReqRemFilDocCtr:
      case ActRemFilDocCtr:
      case ActRemFolDocCtr:
      case ActCopDocCtr:
      case ActPasDocCtr:
      case ActRemTreDocCtr:
      case ActFrmCreDocCtr:
      case ActCreFolDocCtr:
      case ActCreLnkDocCtr:
      case ActRenFolDocCtr:
      case ActRcvFilDocCtrDZ:
      case ActRcvFilDocCtrCla:
      case ActExpAdmDocCtr:
      case ActConAdmDocCtr:
      case ActZIPAdmDocCtr:
      case ActShoDocCtr:
      case ActHidDocCtr:
      case ActReqDatAdmDocCtr:
      case ActChgDatAdmDocCtr:
      case ActDowAdmDocCtr:
	 Gbl.FileBrowser.Type = Brw_ADMI_DOC_CTR;
         break;

      /***** Shared files of centre *****/
      case ActAdmShaCtr:
      case ActReqRemFilShaCtr:
      case ActRemFilShaCtr:
      case ActRemFolShaCtr:
      case ActCopShaCtr:
      case ActPasShaCtr:
      case ActRemTreShaCtr:
      case ActFrmCreShaCtr:
      case ActCreFolShaCtr:
      case ActCreLnkShaCtr:
      case ActRenFolShaCtr:
      case ActRcvFilShaCtrDZ:
      case ActRcvFilShaCtrCla:
      case ActExpShaCtr:
      case ActConShaCtr:
      case ActZIPShaCtr:
      case ActReqDatShaCtr:
      case ActChgDatShaCtr:
      case ActDowShaCtr:
         Gbl.FileBrowser.Type = Brw_ADMI_SHR_CTR;
         break;

      /***** Documents of degree *****/
      case ActSeeAdmDocDeg:	// Access to a documents zone from menu
      case ActChgToSeeDocDeg:	// Access to see a documents zone
      case ActSeeDocDeg:
      case ActExpSeeDocDeg:
      case ActConSeeDocDeg:
      case ActZIPSeeDocDeg:
      case ActReqDatSeeDocDeg:
      case ActDowSeeDocDeg:
	 Gbl.FileBrowser.Type = Brw_SHOW_DOC_DEG;
         break;
      case ActChgToAdmDocDeg:	// Access to admin a documents zone
      case ActAdmDocDeg:
      case ActReqRemFilDocDeg:
      case ActRemFilDocDeg:
      case ActRemFolDocDeg:
      case ActCopDocDeg:
      case ActPasDocDeg:
      case ActRemTreDocDeg:
      case ActFrmCreDocDeg:
      case ActCreFolDocDeg:
      case ActCreLnkDocDeg:
      case ActRenFolDocDeg:
      case ActRcvFilDocDegDZ:
      case ActRcvFilDocDegCla:
      case ActExpAdmDocDeg:
      case ActConAdmDocDeg:
      case ActZIPAdmDocDeg:
      case ActShoDocDeg:
      case ActHidDocDeg:
      case ActReqDatAdmDocDeg:
      case ActChgDatAdmDocDeg:
      case ActDowAdmDocDeg:
	 Gbl.FileBrowser.Type = Brw_ADMI_DOC_DEG;
         break;

      /***** Shared files of degree *****/
      case ActAdmShaDeg:
      case ActReqRemFilShaDeg:
      case ActRemFilShaDeg:
      case ActRemFolShaDeg:
      case ActCopShaDeg:
      case ActPasShaDeg:
      case ActRemTreShaDeg:
      case ActFrmCreShaDeg:
      case ActCreFolShaDeg:
      case ActCreLnkShaDeg:
      case ActRenFolShaDeg:
      case ActRcvFilShaDegDZ:
      case ActRcvFilShaDegCla:
      case ActExpShaDeg:
      case ActConShaDeg:
      case ActZIPShaDeg:
      case ActReqDatShaDeg:
      case ActChgDatShaDeg:
      case ActDowShaDeg:
         Gbl.FileBrowser.Type = Brw_ADMI_SHR_DEG;
         break;

      /***** Documents of course/group *****/
      case ActSeeAdmDocCrsGrp:	// Access to a documents zone from menu
      case ActChgToSeeDocCrs:	// Access to see a documents zone
         /* Set file browser type acording to last group accessed */
         Gbl.FileBrowser.Type = (Gbl.Crs.Grps.GrpCod > 0) ? Brw_SHOW_DOC_GRP :
                                                            Brw_SHOW_DOC_CRS;
         break;
      case ActSeeDocCrs:
      case ActExpSeeDocCrs:
      case ActConSeeDocCrs:
      case ActZIPSeeDocCrs:
      case ActReqDatSeeDocCrs:
      case ActDowSeeDocCrs:
	 Gbl.FileBrowser.Type = Brw_SHOW_DOC_CRS;
         break;
      case ActSeeDocGrp:
      case ActExpSeeDocGrp:
      case ActConSeeDocGrp:
      case ActZIPSeeDocGrp:
      case ActReqDatSeeDocGrp:
      case ActDowSeeDocGrp:
	 Gbl.FileBrowser.Type = Brw_SHOW_DOC_GRP;
         break;
      case ActChgToAdmDocCrs:	// Access to admin a documents zone
         /* Set file browser type acording to last group accessed */
         Gbl.FileBrowser.Type = (Gbl.Crs.Grps.GrpCod > 0) ? Brw_ADMI_DOC_GRP :
                                                            Brw_ADMI_DOC_CRS;
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
      case ActRcvFilDocCrsDZ:
      case ActRcvFilDocCrsCla:
      case ActExpAdmDocCrs:
      case ActConAdmDocCrs:
      case ActZIPAdmDocCrs:
      case ActShoDocCrs:
      case ActHidDocCrs:
      case ActReqDatAdmDocCrs:
      case ActChgDatAdmDocCrs:
      case ActDowAdmDocCrs:
	 Gbl.FileBrowser.Type = Brw_ADMI_DOC_CRS;
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
      case ActRcvFilDocGrpDZ:
      case ActRcvFilDocGrpCla:
      case ActExpAdmDocGrp:
      case ActConAdmDocGrp:
      case ActZIPAdmDocGrp:
      case ActShoDocGrp:
      case ActHidDocGrp:
      case ActReqDatAdmDocGrp:
      case ActChgDatAdmDocGrp:
      case ActDowAdmDocGrp:
	 Gbl.FileBrowser.Type = Brw_ADMI_DOC_GRP;
         break;

      /***** Teachers' private files of course/group *****/
      case ActAdmTchCrsGrp:
      case ActChgToAdmTch:	// Access to a teachers zone from menu
         /* Set file browser type acording to last group accessed */
         Gbl.FileBrowser.Type = (Gbl.Crs.Grps.GrpCod > 0) ? Brw_ADMI_TCH_GRP :
                                                            Brw_ADMI_TCH_CRS;
         break;
      case ActAdmTchCrs:
      case ActReqRemFilTchCrs:
      case ActRemFilTchCrs:
      case ActRemFolTchCrs:
      case ActCopTchCrs:
      case ActPasTchCrs:
      case ActRemTreTchCrs:
      case ActFrmCreTchCrs:
      case ActCreFolTchCrs:
      case ActCreLnkTchCrs:
      case ActRenFolTchCrs:
      case ActRcvFilTchCrsDZ:
      case ActRcvFilTchCrsCla:
      case ActExpTchCrs:
      case ActConTchCrs:
      case ActZIPTchCrs:
      case ActReqDatTchCrs:
      case ActChgDatTchCrs:
      case ActDowTchCrs:
         Gbl.FileBrowser.Type = Brw_ADMI_TCH_CRS;
         break;
      case ActAdmTchGrp:
      case ActReqRemFilTchGrp:
      case ActRemFilTchGrp:
      case ActRemFolTchGrp:
      case ActCopTchGrp:
      case ActPasTchGrp:
      case ActRemTreTchGrp:
      case ActFrmCreTchGrp:
      case ActCreFolTchGrp:
      case ActCreLnkTchGrp:
      case ActRenFolTchGrp:
      case ActRcvFilTchGrpDZ:
      case ActRcvFilTchGrpCla:
      case ActExpTchGrp:
      case ActConTchGrp:
      case ActZIPTchGrp:
      case ActReqDatTchGrp:
      case ActChgDatTchGrp:
      case ActDowTchGrp:
         Gbl.FileBrowser.Type = Brw_ADMI_TCH_GRP;
         break;

      /***** Shared files of course/group *****/
      case ActAdmShaCrsGrp:
      case ActChgToAdmSha:	// Access to a shared zone from menu
         /* Set file browser type acording to last group accessed */
         Gbl.FileBrowser.Type = (Gbl.Crs.Grps.GrpCod > 0) ? Brw_ADMI_SHR_GRP :
                                                            Brw_ADMI_SHR_CRS;
         break;
      case ActAdmShaCrs:
      case ActReqRemFilShaCrs:
      case ActRemFilShaCrs:
      case ActRemFolShaCrs:
      case ActCopShaCrs:
      case ActPasShaCrs:
      case ActRemTreShaCrs:
      case ActFrmCreShaCrs:
      case ActCreFolShaCrs:
      case ActCreLnkShaCrs:
      case ActRenFolShaCrs:
      case ActRcvFilShaCrsDZ:
      case ActRcvFilShaCrsCla:
      case ActExpShaCrs:
      case ActConShaCrs:
      case ActZIPShaCrs:
      case ActReqDatShaCrs:
      case ActChgDatShaCrs:
      case ActDowShaCrs:
         Gbl.FileBrowser.Type = Brw_ADMI_SHR_CRS;
         break;
      case ActAdmShaGrp:
      case ActReqRemFilShaGrp:
      case ActRemFilShaGrp:
      case ActRemFolShaGrp:
      case ActCopShaGrp:
      case ActPasShaGrp:
      case ActRemTreShaGrp:
      case ActFrmCreShaGrp:
      case ActCreFolShaGrp:
      case ActCreLnkShaGrp:
      case ActRenFolShaGrp:
      case ActRcvFilShaGrpDZ:
      case ActRcvFilShaGrpCla:
      case ActExpShaGrp:
      case ActConShaGrp:
      case ActZIPShaGrp:
      case ActReqDatShaGrp:
      case ActChgDatShaGrp:
      case ActDowShaGrp:
         Gbl.FileBrowser.Type = Brw_ADMI_SHR_GRP;
         break;

      /***** My assignments *****/
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
      case ActRcvFilAsgUsrDZ:
      case ActRcvFilAsgUsrCla:
      case ActExpAsgUsr:
      case ActConAsgUsr:
      case ActZIPAsgUsr:
      case ActReqDatAsgUsr:
      case ActChgDatAsgUsr:
      case ActDowAsgUsr:
         Gbl.FileBrowser.Type = Brw_ADMI_ASG_USR;
         break;

      /***** Another users' assignments *****/
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
      case ActRcvFilAsgCrsDZ:
      case ActRcvFilAsgCrsCla:
      case ActExpAsgCrs:
      case ActConAsgCrs:
      case ActZIPAsgCrs:
      case ActReqDatAsgCrs:
      case ActChgDatAsgCrs:
      case ActDowAsgCrs:
         Gbl.FileBrowser.Type = Brw_ADMI_ASG_CRS;
         break;

      /***** My works *****/
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
      case ActRcvFilWrkUsrDZ:
      case ActRcvFilWrkUsrCla:
      case ActExpWrkUsr:
      case ActConWrkUsr:
      case ActZIPWrkUsr:
      case ActReqDatWrkUsr:
      case ActChgDatWrkUsr:
      case ActDowWrkUsr:
         Gbl.FileBrowser.Type = Brw_ADMI_WRK_USR;
         break;

      /***** Another users' works *****/
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
      case ActRcvFilWrkCrsDZ:
      case ActRcvFilWrkCrsCla:
      case ActExpWrkCrs:
      case ActConWrkCrs:
      case ActZIPWrkCrs:
      case ActReqDatWrkCrs:
      case ActChgDatWrkCrs:
      case ActDowWrkCrs:
         Gbl.FileBrowser.Type = Brw_ADMI_WRK_CRS;
         break;

      /***** Documents in project *****/
      case ActAdmDocPrj:
      case ActReqRemFilDocPrj:
      case ActRemFilDocPrj:
      case ActRemFolDocPrj:
      case ActCopDocPrj:
      case ActPasDocPrj:
      case ActRemTreDocPrj:
      case ActFrmCreDocPrj:
      case ActCreFolDocPrj:
      case ActCreLnkDocPrj:
      case ActRenFolDocPrj:
      case ActRcvFilDocPrjDZ:
      case ActRcvFilDocPrjCla:
      case ActExpDocPrj:
      case ActConDocPrj:
      case ActZIPDocPrj:
      case ActReqDatDocPrj:
      case ActChgDatDocPrj:
      case ActDowDocPrj:
         Gbl.FileBrowser.Type = Brw_ADMI_DOC_PRJ;
         break;

      /***** Assessment of project *****/
      case ActAdmAssPrj:
      case ActReqRemFilAssPrj:
      case ActRemFilAssPrj:
      case ActRemFolAssPrj:
      case ActCopAssPrj:
      case ActPasAssPrj:
      case ActRemTreAssPrj:
      case ActFrmCreAssPrj:
      case ActCreFolAssPrj:
      case ActCreLnkAssPrj:
      case ActRenFolAssPrj:
      case ActRcvFilAssPrjDZ:
      case ActRcvFilAssPrjCla:
      case ActExpAssPrj:
      case ActConAssPrj:
      case ActZIPAssPrj:
      case ActReqDatAssPrj:
      case ActChgDatAssPrj:
      case ActDowAssPrj:
         Gbl.FileBrowser.Type = Brw_ADMI_ASS_PRJ;
         break;

      /***** Marks *****/
      case ActSeeAdmMrk:	// Access to a marks zone from menu
         /* Set file browser type acording to last group accessed */
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_STD:
	    case Rol_NET:
	       Gbl.FileBrowser.Type = (Gbl.Crs.Grps.GrpCod > 0) ? Brw_SHOW_MRK_GRP :
								  Brw_SHOW_MRK_CRS;
	       break;
	    case Rol_TCH:
	    case Rol_SYS_ADM:
	       Gbl.FileBrowser.Type = (Gbl.Crs.Grps.GrpCod > 0) ? Brw_ADMI_MRK_GRP :
								  Brw_ADMI_MRK_CRS;
	       break;
	    default:
	       Rol_WrongRoleExit ();
	       break;
	   }
         break;
      case ActChgToSeeMrk:	// Access to see a marks zone
         /* Set file browser type acording to last group accessed */
         Gbl.FileBrowser.Type = (Gbl.Crs.Grps.GrpCod > 0) ? Brw_SHOW_MRK_GRP :
                                                            Brw_SHOW_MRK_CRS;
         break;
      case ActSeeMrkCrs:
      case ActExpSeeMrkCrs:
      case ActConSeeMrkCrs:
      case ActReqDatSeeMrkCrs:
      case ActSeeMyMrkCrs:
         Gbl.FileBrowser.Type = Brw_SHOW_MRK_CRS;
         break;
      case ActSeeMrkGrp:
      case ActExpSeeMrkGrp:
      case ActConSeeMrkGrp:
      case ActReqDatSeeMrkGrp:
      case ActSeeMyMrkGrp:
         Gbl.FileBrowser.Type = Brw_SHOW_MRK_GRP;
         break;
      case ActChgToAdmMrk:	// Access to admin a marks zone
         /* Set file browser type acording to last group accessed */
         Gbl.FileBrowser.Type = (Gbl.Crs.Grps.GrpCod > 0) ? Brw_ADMI_MRK_GRP :
                                                            Brw_ADMI_MRK_CRS;
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
      case ActRcvFilMrkCrsDZ:
      case ActRcvFilMrkCrsCla:
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
         Gbl.FileBrowser.Type = Brw_ADMI_MRK_CRS;
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
      case ActRcvFilMrkGrpDZ:
      case ActRcvFilMrkGrpCla:
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
         Gbl.FileBrowser.Type = Brw_ADMI_MRK_GRP;
         break;

      /***** Briefcase *****/
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
      case ActRcvFilBrfDZ:
      case ActRcvFilBrfCla:
      case ActExpBrf:
      case ActConBrf:
      case ActZIPBrf:
      case ActReqDatBrf:
      case ActChgDatBrf:
      case ActDowBrf:
      case ActReqRemOldBrf:	// Ask for removing old files in briefcase
      case ActRemOldBrf:	// Remove old files in briefcase
         Gbl.FileBrowser.Type = Brw_ADMI_BRF_USR;
         break;
      default:
         Lay_ShowErrorAndExit ("The type of file browser can not be determined.");
         break;
     }

   /***** Get the path in the file browser and the name of the file or folder *****/
   Brw_GetParamsPathInTreeAndFileName ();
   Brw_SetFullPathInTree ();

   /***** Get other parameters *****/
   if (Brw_GetIfProjectFileBrowser ())
      /* Get project code */
      Gbl.Prjs.PrjCod = Prj_GetParamPrjCod ();
   else if (Brw_GetIfCrsAssigWorksFileBrowser ())
     {
      /* Get lists of the selected users if not already got */
      Usr_GetListsSelectedUsrsCods ();
      /* Get user whose folder will be used to make any operation */
      Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();
      /* Get whether we must create the zip file or not */
      Gbl.FileBrowser.ZIP.CreateZIP = ZIP_GetCreateZIPFromForm ();
     }

   switch (Gbl.Action.Act)
     {
      case ActCreFolDocIns:	case ActRenFolDocIns:
      case ActCreFolShaIns:	case ActRenFolShaIns:

      case ActCreFolDocCtr:	case ActRenFolDocCtr:
      case ActCreFolShaCtr:	case ActRenFolShaCtr:

      case ActCreFolDocDeg:	case ActRenFolDocDeg:
      case ActCreFolShaDeg:	case ActRenFolShaDeg:

      case ActCreFolDocCrs:	case ActRenFolDocCrs:
      case ActCreFolDocGrp:	case ActRenFolDocGrp:

      case ActCreFolTchCrs:	case ActRenFolTchCrs:
      case ActCreFolTchGrp:	case ActRenFolTchGrp:

      case ActCreFolShaCrs:	case ActRenFolShaCrs:
      case ActCreFolShaGrp:	case ActRenFolShaGrp:

      case ActCreFolMrkCrs:	case ActRenFolMrkCrs:
      case ActCreFolMrkGrp:	case ActRenFolMrkGrp:

      case ActCreFolAsgCrs:	case ActRenFolAsgCrs:
      case ActCreFolWrkCrs:	case ActRenFolWrkCrs:
      case ActCreFolAsgUsr:	case ActRenFolAsgUsr:
      case ActCreFolWrkUsr:	case ActRenFolWrkUsr:

      case ActCreFolDocPrj:	case ActRenFolDocPrj:
      case ActCreFolAssPrj:	case ActRenFolAssPrj:

      case ActCreFolBrf:	case ActRenFolBrf:
	 /* Get the name of the new folder */
	 Par_GetParToText ("NewFolderName",Gbl.FileBrowser.NewFilFolLnkName,NAME_MAX);
	 break;
      case ActCreLnkDocIns:
      case ActCreLnkShaIns:
      case ActCreLnkDocCtr:
      case ActCreLnkShaCtr:
      case ActCreLnkDocDeg:
      case ActCreLnkShaDeg:
      case ActCreLnkDocCrs:      case ActCreLnkDocGrp:
      case ActCreLnkTchCrs:      case ActCreLnkTchGrp:
      case ActCreLnkShaCrs:      case ActCreLnkShaGrp:
      case ActCreLnkAsgCrs:
      case ActCreLnkWrkCrs:
      case ActCreLnkAsgUsr:
      case ActCreLnkWrkUsr:
      case ActCreLnkDocPrj:
      case ActCreLnkBrf:
	 /* Get the name of the new link */
	 Par_GetParToText ("NewLinkName",Gbl.FileBrowser.NewFilFolLnkName,NAME_MAX);
	 break;
      default:
	 break;
     }

   /***** Get whether to show full tree *****/
   // If I belong to the current course or I am superuser, or file browser is briefcase ==> get whether show full tree from form
   // Else ==> show full tree (only public files)
   Gbl.FileBrowser.ShowOnlyPublicFiles = false;
   if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM)
      switch (Gbl.FileBrowser.Type)
	{
	 case Brw_SHOW_DOC_INS:
	 case Brw_ADMI_DOC_INS:
	 case Brw_ADMI_SHR_INS:
	    Gbl.FileBrowser.ShowOnlyPublicFiles = !Gbl.Usrs.Me.IBelongToCurrentIns;
	    break;
	 case Brw_SHOW_DOC_CTR:
	 case Brw_ADMI_DOC_CTR:
	 case Brw_ADMI_SHR_CTR:
	    Gbl.FileBrowser.ShowOnlyPublicFiles = !Gbl.Usrs.Me.IBelongToCurrentCtr;
	    break;
	 case Brw_SHOW_DOC_DEG:
	 case Brw_ADMI_DOC_DEG:
	 case Brw_ADMI_SHR_DEG:
	    Gbl.FileBrowser.ShowOnlyPublicFiles = !Gbl.Usrs.Me.IBelongToCurrentDeg;
	    break;
	 case Brw_SHOW_DOC_CRS:
	 case Brw_ADMI_DOC_CRS:
	 case Brw_ADMI_SHR_CRS:
	    Gbl.FileBrowser.ShowOnlyPublicFiles = !Gbl.Usrs.Me.IBelongToCurrentCrs;
	    break;
	 default:
	    break;
	}
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
   long GrpCod;

   if ((GrpCod = Par_GetParToLong ("GrpCod")) > 0)
      return GrpCod;
   else
      /***** Try to get group code from database *****/
      switch (Gbl.Action.Act)
	{
	 case ActSeeAdmDocCrsGrp:	// Access to a documents zone from menu
	 case ActSeeDocGrp:
	 case ActAdmDocGrp:
	 case ActAdmTchCrsGrp:		// Access to a documents zone from menu
	 case ActAdmTchGrp:
	    return Brw_GetGrpLastAccZone ("LastDowGrpCod");
	 case ActAdmShaCrsGrp:		// Access to a shared documents zone from menu
	 case ActAdmShaGrp:
	    return Brw_GetGrpLastAccZone ("LastComGrpCod");
	 case ActSeeAdmMrk:		// Access to a marks zone from menu
	 case ActSeeMrkGrp:
	 case ActAdmMrkGrp:
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

   if (Gbl.Crs.Grps.GrpCod > 0)
     {
      GrpDat.GrpCod = Gbl.Crs.Grps.GrpCod;
      Grp_GetDataOfGroupByCod (&GrpDat);

      switch (Gbl.Action.Act)
	{
	 case ActSeeAdmDocCrsGrp:	// Access to see/admin a documents zone from menu
	 case ActChgToSeeDocCrs:	// Access to see a documents zone
	 case ActSeeDocGrp:		// Access to see a documents zone

	 case ActChgToAdmDocCrs:	// Access to admin a documents zone
	 case ActAdmDocGrp:		// Access to admin a documents zone

	 case ActChgToAdmTch:		// Access to admin a teachers' zone
	 case ActAdmTchGrp:		// Access to admin a teachers' zone

	 case ActChgToAdmSha:		// Access to admin a shared zone
	 case ActAdmShaGrp:		// Access to admin a shared zone

	 case ActSeeAdmMrk:		// Access to see/admin a marks zone from menu
	 case ActChgToSeeMrk:		// Access to see a marks zone
	 case ActSeeMrkGrp:		// Access to see a marks zone

	 case ActChgToAdmMrk:		// Access to admin a marks zone
	 case ActAdmMrkGrp:		// Access to admin a marks zone
	    /***** For security, check if group file zones are enabled,
		   and check if I belongs to the selected group *****/
	    if (GrpDat.FileZones)
	      {
	       if (!Grp_GetIfIBelongToGrp (Gbl.Crs.Grps.GrpCod))
	          Gbl.Crs.Grps.GrpCod = -1L;	// Go to course zone
	      }
	    else
	       Gbl.Crs.Grps.GrpCod = -1L;	// Go to course zone
	    break;
	 default:
	    /***** For security, check if group file zones are enabled,
		   and check if I belongs to the selected group *****/
	    if (!GrpDat.FileZones)
	       Lay_ShowErrorAndExit ("The group has no file zones.");
	    else if (!Grp_GetIfIBelongToGrp (Gbl.Crs.Grps.GrpCod))
	       Lay_ShowErrorAndExit ("You don't have access to the group.");
	    break;
	}

      /***** Get data of the current group *****/
      if (Gbl.Crs.Grps.GrpCod > 0)
	{
	 Gbl.Crs.Grps.GrpTyp.GrpTypCod         = GrpDat.GrpTypCod;
	 Str_Copy (Gbl.Crs.Grps.GrpTyp.GrpTypName,GrpDat.GrpTypName,
	           Grp_MAX_BYTES_GROUP_TYPE_NAME);
	 Str_Copy (Gbl.Crs.Grps.GrpName,GrpDat.GrpName,
	           Grp_MAX_BYTES_GROUP_NAME);
	 Gbl.Crs.Grps.MaxStudents              = GrpDat.MaxStudents;
	 Gbl.Crs.Grps.Open                     = GrpDat.Open;
	 Gbl.Crs.Grps.FileZones                = GrpDat.FileZones;
	 Gbl.Crs.Grps.GrpTyp.MultipleEnrolment = GrpDat.MultipleEnrolment;
	}
     }
  }

/*****************************************************************************/
/**************** Write hidden parameter with code of file *******************/
/*****************************************************************************/

void Brw_PutHiddenParamFilCod (long FilCod)
  {
   Par_PutHiddenParamLong (NULL,"FilCod",FilCod);
  }

/*****************************************************************************/
/********************* Get parameter with code of file ***********************/
/*****************************************************************************/

long Brw_GetParamFilCod (void)
  {
   /***** Get code of file *****/
   return Par_GetParToLong ("FilCod");
  }

/*****************************************************************************/
/**************** Write parameters related with file browser *****************/
/*****************************************************************************/

void Brw_PutImplicitParamsFileBrowser (void)
  {
   Brw_PutParamsFileBrowser (Gbl.FileBrowser.FilFolLnk.Path,
			     Gbl.FileBrowser.FilFolLnk.Name,
			     Gbl.FileBrowser.FilFolLnk.Type,
			     -1L);	// Not used
  }

static void Brw_PutParamsFileBrowser (const char *PathInTree,const char *FilFolLnkName,
                                      Brw_FileType_t FileType,long FilCod)
  {
   if (Brw_GetIfGroupFileBrowser ())		// This file browser needs specify a group
      /***** Group code *****/
      Grp_PutParamGrpCod (Gbl.Crs.Grps.GrpCod);
   else if (Brw_GetIfProjectFileBrowser ())	// This file browser needs specify a project
      /***** Project code *****/
      Prj_PutParamPrjCod (Gbl.Prjs.PrjCod);
   else if (Brw_GetIfCrsAssigWorksFileBrowser ())
     {
      /***** Users selected *****/
      Usr_PutHiddenParSelectedUsrsCods ();
      Usr_PutParamOtherUsrCodEncrypted ();
     }

   /***** If full tree selected? *****/
   Brw_PutHiddenParamFullTreeIfSelected ();

   /***** Path and file *****/
   if (PathInTree)
      Par_PutHiddenParamString (NULL,"Path",PathInTree);
   if (FilFolLnkName)
      Par_PutHiddenParamString (NULL,Brw_FileTypeParamName[FileType],FilFolLnkName);
   if (FilCod > 0)
      Brw_PutHiddenParamFilCod (FilCod);
  }

/*****************************************************************************/
/************** Get parameters path and file in file browser *****************/
/*****************************************************************************/

static void Brw_GetParamsPathInTreeAndFileName (void)
  {
   const char *Ptr;
   unsigned i;
   Brw_FileType_t FileType;
   char FileNameToShow[NAME_MAX + 1];

   /***** Get the path inside the tree
          (this path does not include
           the name of the file or folder at the end) *****/
   Par_GetParToText ("Path",Gbl.FileBrowser.FilFolLnk.Path,PATH_MAX);

   /* Check if path contains ".." */
   if (strstr (Gbl.FileBrowser.FilFolLnk.Path,".."))	// ".." is not allowed in path
      Lay_ShowErrorAndExit ("Wrong path.");

   /***** Get the name of the file, folder or link *****/
   Gbl.FileBrowser.FilFolLnk.Type = Brw_IS_UNKNOWN;
   for (FileType = (Brw_FileType_t) 0;
	FileType < Brw_NUM_FILE_TYPES;
	FileType++)
      // File names with heading and trailing spaces are allowed
      if (Par_GetParAndChangeFormat (Brw_FileTypeParamName[FileType],
                                     Gbl.FileBrowser.FilFolLnk.Name,
                                     NAME_MAX,Str_TO_TEXT,false))
	{
	 Gbl.FileBrowser.FilFolLnk.Type = FileType;

	 /* Check if filename contains ".." */
	 Brw_GetFileNameToShow (Gbl.FileBrowser.FilFolLnk.Type,
	                        Gbl.FileBrowser.FilFolLnk.Name,
                                FileNameToShow);
	 if (strstr (FileNameToShow,".."))	// ".." is not allowed in filename
	    Lay_ShowErrorAndExit ("Wrong file name.");
	 break;
	}

   /***** Set level of this file or folder inside file browser *****/
   if (!strcmp (Gbl.FileBrowser.FilFolLnk.Name,"."))
      Gbl.FileBrowser.Level = 0;
   else
     {
      // Level == number-of-slashes-in-path-except-file-or-folder + 1
      Gbl.FileBrowser.Level = 1;
      for (Ptr = Gbl.FileBrowser.FilFolLnk.Path;
	   *Ptr;
	   Ptr++)
         if (*Ptr == '/')
            Gbl.FileBrowser.Level++;
     }

   /***** Get data of assignment *****/
   if (Gbl.FileBrowser.Level &&
       (Gbl.FileBrowser.Type == Brw_ADMI_ASG_USR ||
        Gbl.FileBrowser.Type == Brw_ADMI_ASG_CRS))
     {
      if (Gbl.FileBrowser.Level == 1)
         // We are in this case: assignments/assignment-folder
         Str_Copy (Gbl.FileBrowser.Asg.Folder,Gbl.FileBrowser.FilFolLnk.Name,
                   Brw_MAX_BYTES_FOLDER);
      else
        {
         // We are in this case: assignments/assignment-folder/rest-of-path
         for (Ptr = Gbl.FileBrowser.FilFolLnk.Path;
              *Ptr && *Ptr != '/';
              Ptr++);	// Go to first '/'
         if (*Ptr == '/')
            Ptr++;	// Skip '/'
         for (i = 0;
              i < Brw_MAX_BYTES_FOLDER && *Ptr && *Ptr != '/';
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
   char Path[PATH_MAX * 2 + 128];

   /***** Reset paths. An empty path means that
          we don't have to create that directory *****/
   Gbl.FileBrowser.Priv.PathAboveRootFolder[0] = '\0';
   Gbl.FileBrowser.Priv.PathRootFolder[0] = '\0';

   /***** Set paths depending on file browser *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_SHOW_DOC_INS:
      case Brw_ADMI_DOC_INS:
      case Brw_ADMI_SHR_INS:
	 /* Create a directory for institutions */
	 Fil_CreateDirIfNotExists (Cfg_PATH_INS_PRIVATE);

	 /* Create a directory for all institutions which codes end in
	    institution-code mod 100 */
	 snprintf (Path,sizeof (Path),
	           "%s/%02u",
		   Cfg_PATH_INS_PRIVATE,
		   (unsigned) (Gbl.Hierarchy.Ins.InsCod % 100));
	 Fil_CreateDirIfNotExists (Path);

	 /* Create path to the current institution */
	 snprintf (Gbl.FileBrowser.Priv.PathAboveRootFolder,
	           sizeof (Gbl.FileBrowser.Priv.PathAboveRootFolder),
	           "%s/%02u/%u",
		   Cfg_PATH_INS_PRIVATE,
		   (unsigned) (Gbl.Hierarchy.Ins.InsCod % 100),
		   (unsigned) Gbl.Hierarchy.Ins.InsCod);
         break;
      case Brw_SHOW_DOC_CTR:
      case Brw_ADMI_DOC_CTR:
      case Brw_ADMI_SHR_CTR:
	 /* Create a directory for centres */
	 Fil_CreateDirIfNotExists (Cfg_PATH_CTR_PRIVATE);

	 /* Create a directory for all centres which codes end in
	    centre-code mod 100 */
	 snprintf (Path,sizeof (Path),
	           "%s/%02u",
		   Cfg_PATH_CTR_PRIVATE,
		   (unsigned) (Gbl.Hierarchy.Ctr.CtrCod % 100));
	 Fil_CreateDirIfNotExists (Path);

	 /* Create path to the current centre */
	 snprintf (Gbl.FileBrowser.Priv.PathAboveRootFolder,
	           sizeof (Gbl.FileBrowser.Priv.PathAboveRootFolder),
	           "%s/%02u/%u",
		   Cfg_PATH_CTR_PRIVATE,
		   (unsigned) (Gbl.Hierarchy.Ctr.CtrCod % 100),
		   (unsigned) Gbl.Hierarchy.Ctr.CtrCod);
	 break;
      case Brw_SHOW_DOC_DEG:
      case Brw_ADMI_DOC_DEG:
      case Brw_ADMI_SHR_DEG:
	 /* Create a directory for degrees */
	 Fil_CreateDirIfNotExists (Cfg_PATH_DEG_PRIVATE);

	 /* Create a directory for all degrees which codes end in
	    degree-code mod 100 */
	 snprintf (Path,sizeof (Path),
	           "%s/%02u",
		   Cfg_PATH_DEG_PRIVATE,
		   (unsigned) (Gbl.Hierarchy.Deg.DegCod % 100));
	 Fil_CreateDirIfNotExists (Path);

         /* Create path to the current degree */
	 snprintf (Gbl.FileBrowser.Priv.PathAboveRootFolder,
	           sizeof (Gbl.FileBrowser.Priv.PathAboveRootFolder),
	           "%s/%02u/%u",
		   Cfg_PATH_DEG_PRIVATE,
		   (unsigned) (Gbl.Hierarchy.Deg.DegCod % 100),
		   (unsigned) Gbl.Hierarchy.Deg.DegCod);
	 break;
      case Brw_SHOW_DOC_CRS:
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_TCH_CRS:
      case Brw_ADMI_SHR_CRS:
      case Brw_SHOW_MRK_CRS:
      case Brw_ADMI_MRK_CRS:
         /* Create path to the current course */
         Str_Copy (Gbl.FileBrowser.Priv.PathAboveRootFolder,
                   Gbl.Crs.PathPriv,
                   PATH_MAX);
	 break;
      case Brw_SHOW_DOC_GRP:
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_GRP:
      case Brw_ADMI_SHR_GRP:
      case Brw_SHOW_MRK_GRP:
      case Brw_ADMI_MRK_GRP:
	 /* Create a directory for groups inside the current course */
         snprintf (Path,sizeof (Path),
                   "%s/%s",
                   Gbl.Crs.PathPriv,Cfg_FOLDER_GRP);
         Fil_CreateDirIfNotExists (Path);

         /* Create path to this group */
         snprintf (Path,sizeof (Path),
                   "%s/%s/%ld",
                   Gbl.Crs.PathPriv,Cfg_FOLDER_GRP,
                   Gbl.Crs.Grps.GrpCod);
         Str_Copy (Gbl.FileBrowser.Priv.PathAboveRootFolder,
        	   Path,
                   PATH_MAX);
	 break;
      case Brw_ADMI_ASG_USR:
      case Brw_ADMI_WRK_USR:
	 /* Create a directory for me inside the current course */
         snprintf (Path,sizeof (Path),
                   "%s/%s",
                   Gbl.Crs.PathPriv,Cfg_FOLDER_USR);
         Fil_CreateDirIfNotExists (Path);

	 /* Create a directory for all users whose codes end in
	    my-user-code mod 100 */
         snprintf (Path,sizeof (Path),
                   "%s/%s/%02u",
                   Gbl.Crs.PathPriv,Cfg_FOLDER_USR,
                   (unsigned) (Gbl.Usrs.Me.UsrDat.UsrCod % 100));
         Fil_CreateDirIfNotExists (Path);

         /* Create path to me */
         snprintf (Path,sizeof (Path),
                   "%s/%s/%02u/%ld",
                   Gbl.Crs.PathPriv,Cfg_FOLDER_USR,
                   (unsigned) (Gbl.Usrs.Me.UsrDat.UsrCod % 100),
                   Gbl.Usrs.Me.UsrDat.UsrCod);
         Str_Copy (Gbl.FileBrowser.Priv.PathAboveRootFolder,
        	   Path,
                   PATH_MAX);
         break;
      case Brw_ADMI_ASG_CRS:
      case Brw_ADMI_WRK_CRS:
         if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
           {
	    /* Create a directory for this user inside the current course */
            snprintf (Path,sizeof (Path),
        	      "%s/%s",
        	      Gbl.Crs.PathPriv,Cfg_FOLDER_USR);
            Fil_CreateDirIfNotExists (Path);

	    /* Create a directory for all users whose codes end in
	       user-code mod 100 */
	    snprintf (Path,sizeof (Path),
		      "%s/%s/%02u",
		      Gbl.Crs.PathPriv,Cfg_FOLDER_USR,
		      (unsigned) (Gbl.Usrs.Other.UsrDat.UsrCod % 100));
	    Fil_CreateDirIfNotExists (Path);

            /* Create path to user */
            snprintf (Path,sizeof (Path),
        	      "%s/%s/%02u/%ld",
        	      Gbl.Crs.PathPriv,Cfg_FOLDER_USR,
                      (unsigned) (Gbl.Usrs.Other.UsrDat.UsrCod % 100),
        	      Gbl.Usrs.Other.UsrDat.UsrCod);
            Str_Copy (Gbl.FileBrowser.Priv.PathAboveRootFolder,
           	      Path,
                      PATH_MAX);
           }
         break;
      case Brw_ADMI_DOC_PRJ:
      case Brw_ADMI_ASS_PRJ:
	 /* Create a directory for projects inside the current course */
         snprintf (Path,sizeof (Path),
                   "%s/%s",
                   Gbl.Crs.PathPriv,Cfg_FOLDER_PRJ);
         Fil_CreateDirIfNotExists (Path);

	 /* Create a directory for all projects which codes end in
	    project-code mod 100 */
	 snprintf (Path,sizeof (Path),
	           "%s/%s/%02u",
		   Gbl.Crs.PathPriv,Cfg_FOLDER_PRJ,
                   (unsigned) (Gbl.Prjs.PrjCod % 100));
	 Fil_CreateDirIfNotExists (Path);

         /* Create path to the current project */
         snprintf (Path,sizeof (Path),
                   "%s/%s/%02u/%ld",
                   Gbl.Crs.PathPriv,Cfg_FOLDER_PRJ,
                   (unsigned) (Gbl.Prjs.PrjCod % 100),
                   Gbl.Prjs.PrjCod);
         Str_Copy (Gbl.FileBrowser.Priv.PathAboveRootFolder,
        	   Path,
                   PATH_MAX);
	 break;
      case Brw_ADMI_BRF_USR:
         Str_Copy (Gbl.FileBrowser.Priv.PathAboveRootFolder,
                   Gbl.Usrs.Me.PathDir,
                   PATH_MAX);
	 break;
      default:
	 return;
     }

   /***** Create directories that not exist *****/
   if (Gbl.FileBrowser.Priv.PathAboveRootFolder[0])
     {
      Fil_CreateDirIfNotExists (Gbl.FileBrowser.Priv.PathAboveRootFolder);
      snprintf (Path,sizeof (Path),
	        "%s/%s",
                Gbl.FileBrowser.Priv.PathAboveRootFolder,
                Brw_RootFolderInternalNames[Gbl.FileBrowser.Type]);
      Str_Copy (Gbl.FileBrowser.Priv.PathRootFolder,
     	        Path,
                PATH_MAX);
      Fil_CreateDirIfNotExists (Gbl.FileBrowser.Priv.PathRootFolder);

      /***** If file browser is for assignments,
             create folders of assignments if not exist *****/
      if (Gbl.FileBrowser.Type == Brw_ADMI_ASG_USR)
	 Brw_CreateFoldersAssignmentsIfNotExist (Gbl.Usrs.Me.UsrDat.UsrCod);
      else if (Gbl.FileBrowser.Type == Brw_ADMI_ASG_CRS)
	 Brw_CreateFoldersAssignmentsIfNotExist (Gbl.Usrs.Other.UsrDat.UsrCod);
     }
  }

/*****************************************************************************/
/****** Check if exists a folder of assignments for any user in course *******/
/*****************************************************************************/
// Folders are in level 1, just under root folder

bool Brw_CheckIfExistsFolderAssigmentForAnyUsr (const char *FolderName)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   long UsrCod;
   char PathFolder[PATH_MAX * 2 + 128];
   bool FolderExists = false;

   /***** Get all the users belonging to current course from database *****/
   NumUsrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get users"
						   " from current course",
				        "SELECT UsrCod FROM crs_usr"
				        " WHERE CrsCod=%ld",
					Gbl.Hierarchy.Crs.CrsCod);

   /***** Check folders *****/
   for (NumUsr = 0;
	NumUsr < NumUsrs && !FolderExists;
	NumUsr++)
     {
      /* Get next user */
      row = mysql_fetch_row (mysql_res);
      UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Check if folder exists */
      snprintf (PathFolder,sizeof (PathFolder),
	        "%s/usr/%02u/%ld/%s/%s",
                Gbl.Crs.PathPriv,
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
// Create a folder of an assignment when:
// 1. The assignment is visible (not hidden)
// 2. ...and the folder name is not empty (the teacher has set that the user must send work(s) for that assignment)
// 3. ...the assignment is not restricted to groups or (if restricted to groups), the owner of zone belong to any of the groups

static void Brw_CreateFoldersAssignmentsIfNotExist (long ZoneUsrCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned long NumRow;
   char PathFolderAsg[PATH_MAX + 1 + PATH_MAX + 1];

   /***** Get assignment folders from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get folders of assignments",
			     "SELECT Folder FROM assignments"
			     " WHERE CrsCod=%ld AND Hidden='N' AND Folder<>''"
			     " AND (AsgCod NOT IN (SELECT AsgCod FROM asg_grp) OR"
			     " AsgCod IN (SELECT asg_grp.AsgCod FROM asg_grp,crs_grp_usr"
			     " WHERE crs_grp_usr.UsrCod=%ld"
			     " AND asg_grp.GrpCod=crs_grp_usr.GrpCod))",
			     Gbl.Hierarchy.Crs.CrsCod,ZoneUsrCod);

   /***** Create one folder for each assignment *****/
   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      /* Get next assignment with folder */
      row = mysql_fetch_row (mysql_res);
      if (row)
	 if (row[0])	// Not necessary, because folder name is checked in query to be not empty
	   {
	    /* Create folder if not exists */
	    snprintf (PathFolderAsg,sizeof (PathFolderAsg),
		      "%s/%s",
		      Gbl.FileBrowser.Priv.PathRootFolder,row[0]);
	    Fil_CreateDirIfNotExists (PathFolderAsg);
	   }
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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   long UsrCod;
   char OldPath[PATH_MAX + 1];
   char NewPath[PATH_MAX + 1];
   char PathOldFolder[PATH_MAX * 2 + 128];
   char PathNewFolder[PATH_MAX * 2 + 128];
   bool RenamingIsPossible = true;
   unsigned NumUsrsError = 0;
   unsigned NumUsrsSuccess = 0;

   /***** Get all the users belonging to current course from database *****/
   NumUsrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get users"
						   " from current course",
				        "SELECT UsrCod FROM crs_usr"
				        " WHERE CrsCod=%ld",
					Gbl.Hierarchy.Crs.CrsCod);

   /***** Check if there exist folders with the new name *****/
   for (NumUsr = 0;
	NumUsr < NumUsrs && RenamingIsPossible;
	NumUsr++)
     {
      /* Get next user */
      row = mysql_fetch_row (mysql_res);
      UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Rename folder if exists */
      snprintf (PathOldFolder,sizeof (PathOldFolder),
	        "%s/usr/%02u/%ld/%s/%s",
                Gbl.Crs.PathPriv,
                (unsigned) (UsrCod % 100),
                UsrCod,	// User's code
                Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
                OldFolderName);
      snprintf (PathNewFolder,sizeof (PathNewFolder),
	        "%s/usr/%02u/%ld/%s/%s",
                Gbl.Crs.PathPriv,
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
         snprintf (PathOldFolder,sizeof (PathOldFolder),
                   "%s/usr/%02u/%ld/%s/%s",
                   Gbl.Crs.PathPriv,
                   (unsigned) (UsrCod % 100),
                   UsrCod,	// User's code
                   Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
                   OldFolderName);
         if (Fil_CheckIfPathExists (PathOldFolder))
           {
            snprintf (PathNewFolder,sizeof (PathNewFolder),
        	      "%s/usr/%02u/%ld/%s/%s",
                      Gbl.Crs.PathPriv,
	 	      (unsigned) (UsrCod % 100),
 		      UsrCod,	// User's code
                      Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
                      NewFolderName);
            if (rename (PathOldFolder,PathNewFolder))	// Fail
	      {
               Ale_ShowAlert (Ale_ERROR,Txt_Can_not_rename_a_folder_of_assignment);
               NumUsrsError++;
	      }
            else					// Success
              {
               /* Remove affected clipboards */
               Brw_RemoveAffectedClipboards (Brw_ADMI_ASG_USR,UsrCod,-1L);
               Brw_RemoveAffectedClipboards (Brw_ADMI_ASG_CRS,-1L,UsrCod);

               /* Rename affected expanded folders */
               snprintf (OldPath,sizeof (OldPath),
        	         "%s/%s",
			 Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,OldFolderName);
               snprintf (NewPath,sizeof (NewPath),
        	         "%s/%s",
			 Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,NewFolderName);
               Brw_RenameAffectedExpandedFolders (Brw_ADMI_ASG_USR,UsrCod,-1L,
        	                                  OldPath,NewPath);
               Brw_RenameAffectedExpandedFolders (Brw_ADMI_ASG_CRS,-1L,UsrCod,
        	                                  OldPath,NewPath);

               NumUsrsSuccess++;
              }
           }
        }

      /***** Summary message *****/
      Ale_ShowAlert (Ale_INFO,"%s: %u<br />"
                              "%s: %u<br />"
                              "%s: %u.",
                     Txt_Users,NumUsrs,
                     Txt_Folders_renamed,NumUsrsSuccess,
                     Txt_Folders_not_renamed,NumUsrsError);
     }
   else
      /***** Warning message *****/
      Ale_ShowAlert (Ale_WARNING,Txt_Can_not_rename_a_folder_of_assignment);

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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   long UsrCod;
   char PathFolder[PATH_MAX * 2 + 128];

   /***** Get all the users belonging to current course from database *****/
   NumUsrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get users"
						   " from current course",
				        "SELECT UsrCod FROM crs_usr"
				        " WHERE CrsCod=%ld",
					Gbl.Hierarchy.Crs.CrsCod);

   /***** Remove folders *****/
   for (NumUsr = 0;
	NumUsr < NumUsrs;
	NumUsr++)
     {
      /* Get next user */
      row = mysql_fetch_row (mysql_res);
      UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Remove tree if exists */
      snprintf (PathFolder,sizeof (PathFolder),
	        "%s/usr/%02u/%ld/%s/%s",
                Gbl.Crs.PathPriv,
                (unsigned) (UsrCod % 100),
                UsrCod,	// User's code
                Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
                FolderName);
      Fil_RemoveTree (PathFolder);
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
      Ale_ShowAlert (Ale_WARNING,Txt_Quota_exceeded);
  }

/*****************************************************************************/
/************ Initialize maximum quota of current file browser ***************/
/*****************************************************************************/

static void Brw_SetMaxQuota (void)
  {
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_SHOW_DOC_INS:
      case Brw_ADMI_DOC_INS:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_DOCUM_INS;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_DOCUM_INS;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_DOCUM_INS;
         break;
      case Brw_ADMI_SHR_INS:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_SHARE_INS;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_SHARE_INS;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_SHARE_INS;
	 break;
      case Brw_SHOW_DOC_CTR:
      case Brw_ADMI_DOC_CTR:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_DOCUM_CTR;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_DOCUM_CTR;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_DOCUM_CTR;
         break;
      case Brw_ADMI_SHR_CTR:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_SHARE_CTR;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_SHARE_CTR;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_SHARE_CTR;
	 break;
      case Brw_SHOW_DOC_DEG:
      case Brw_ADMI_DOC_DEG:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_DOCUM_DEG;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_DOCUM_DEG;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_DOCUM_DEG;
         break;
      case Brw_ADMI_SHR_DEG:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_SHARE_DEG;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_SHARE_DEG;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_SHARE_DEG;
	 break;
      case Brw_SHOW_DOC_CRS:
      case Brw_ADMI_DOC_CRS:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_DOCUM_CRS;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_DOCUM_CRS;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_DOCUM_CRS;
	 break;
      case Brw_SHOW_DOC_GRP:
      case Brw_ADMI_DOC_GRP:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_DOCUM_GRP;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_DOCUM_GRP;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_DOCUM_GRP;
	 break;
      case Brw_ADMI_TCH_CRS:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_TEACH_CRS;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_TEACH_CRS;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_TEACH_CRS;
	 break;
      case Brw_ADMI_TCH_GRP:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_TEACH_GRP;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_TEACH_GRP;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_TEACH_GRP;
	 break;
      case Brw_ADMI_SHR_CRS:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_SHARE_CRS;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_SHARE_CRS;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_SHARE_CRS;
	 break;
      case Brw_ADMI_SHR_GRP:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_SHARE_GRP;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_SHARE_GRP;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_SHARE_GRP;
	 break;
      case Brw_ADMI_ASG_USR:
      case Brw_ADMI_ASG_CRS:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_ASSIG_PER_STD;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_ASSIG_PER_STD;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_ASSIG_PER_STD;
	 break;
      case Brw_ADMI_WRK_USR:
      case Brw_ADMI_WRK_CRS:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_WORKS_PER_STD;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_WORKS_PER_STD;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_WORKS_PER_STD;
	 break;
      case Brw_ADMI_DOC_PRJ:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_DOC_PRJ;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_DOC_PRJ;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_DOC_PRJ;
	 break;
      case Brw_ADMI_ASS_PRJ:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_ASS_PRJ;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_ASS_PRJ;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_ASS_PRJ;
	 break;
      case Brw_SHOW_MRK_CRS:
      case Brw_ADMI_MRK_CRS:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_MARKS_CRS;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_MARKS_CRS;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_MARKS_CRS;
	 break;
      case Brw_SHOW_MRK_GRP:
      case Brw_ADMI_MRK_GRP:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_MARKS_GRP;
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_MARKS_GRP;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_MARKS_GRP;
	 break;
      case Brw_ADMI_BRF_USR:
	 Gbl.FileBrowser.Size.MaxQuota = Brw_MAX_QUOTA_BRIEF[Gbl.Usrs.Me.Role.Max];
         Gbl.FileBrowser.Size.MaxFiles = Brw_MAX_FILES_BRIEF;
         Gbl.FileBrowser.Size.MaxFolds = Brw_MAX_FOLDS_BRIEF;
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
   extern const char *Hlp_FILES_Homework_for_teachers;
   extern const char *Txt_Assignments_and_other_works;
   extern const char *Txt_View_homework;

   /***** List users to select some of them *****/
   Usr_PutFormToSelectUsrsToGoToAct (ActAdmAsgWrkCrs,NULL,
				     Txt_Assignments_and_other_works,
	                             Hlp_FILES_Homework_for_teachers,
	                             Txt_View_homework);
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
/************* Show file browser with the documents of a project *************/
/*****************************************************************************/

static void Brw_ShowFileBrowserProject (void)
  {
   extern const char *Hlp_ASSESSMENT_Projects;
   struct Project Prj;
   unsigned MyRolesInProject;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get project data *****/
   Prj.PrjCod = Prj_GetParamPrjCod ();
   Prj_GetDataOfProjectByCod (&Prj);
   Gbl.Prjs.PrjCod = Prj.PrjCod;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Prj.Title,NULL,
		 Hlp_ASSESSMENT_Projects,Box_NOT_CLOSABLE);

   /***** Show the project *****/
   Prj_ShowOneUniqueProject (&Prj);

   /***** Show project file browsers *****/
   MyRolesInProject = Prj_GetMyRolesInProject (Gbl.Prjs.PrjCod);
   if (Prj_CheckIfICanViewProjectFiles (MyRolesInProject))
     {
      Brw_WriteTopBeforeShowingFileBrowser ();

      if (Brw_CheckIfICanViewProjectDocuments (MyRolesInProject))
	{
	 /***** Show the tree with the project documents *****/
	 Gbl.FileBrowser.Type = Brw_ADMI_DOC_PRJ;
	 Brw_InitializeFileBrowser ();
	 Brw_ShowFileBrowser ();
	}

      if (Brw_CheckIfICanViewProjectAssessment (MyRolesInProject))
	{
	 /***** Show the tree with the project assessment *****/
	 Gbl.FileBrowser.Type = Brw_ADMI_ASS_PRJ;
	 Brw_InitializeFileBrowser ();
	 Brw_ShowFileBrowser ();
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,"You have no access to project files.");

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);
  }

/*****************************************************************************/
/*** Show file browsers with works files of several users of current course **/
/*****************************************************************************/

static void Brw_ShowFileBrowsersAsgWrkCrs (void)
  {
   extern const char *Hlp_FILES_Homework_for_teachers;
   extern const char *Txt_Assignments_and_other_works;
   const char *Ptr;

   /***** Create the zip file and put a link to download it *****/
   if (Gbl.FileBrowser.ZIP.CreateZIP)
      ZIP_CreateZIPAsgWrk ();

   /***** Write top before showing file browser *****/
   Brw_WriteTopBeforeShowingFileBrowser ();

   /***** Begin box and table *****/
   Box_StartBoxTable ("100%",Txt_Assignments_and_other_works,
		      Brw_PutIconShowFigure,
		      Hlp_FILES_Homework_for_teachers,Box_NOT_CLOSABLE,0);

   /***** List the assignments and works of the selected users *****/
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,Gbl.Usrs.Other.UsrDat.EncryptedUsrCod,
					 Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))	// Get of the database the data of the user
	 if (Usr_CheckIfICanViewAsgWrk (&Gbl.Usrs.Other.UsrDat))
	   {
	    /***** Show a row with the data of the owner of the works *****/
	    HTM_TR_Begin (NULL);
	    Brw_ShowDataOwnerAsgWrk (&Gbl.Usrs.Other.UsrDat);

	    HTM_TD_Begin ("class=\"LT\"");

	    /***** Show the tree with the assignments *****/
	    Gbl.FileBrowser.Type = Brw_ADMI_ASG_CRS;
	    Brw_InitializeFileBrowser ();
	    Brw_ShowFileBrowser ();

	    /***** Show the tree with the works *****/
	    Gbl.FileBrowser.Type = Brw_ADMI_WRK_CRS;
	    Brw_InitializeFileBrowser ();
	    Brw_ShowFileBrowser ();

	    HTM_TD_End ();
	    HTM_TR_End ();
	   }
     }

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/************ Show file browsers with works files of one user ****************/
/*****************************************************************************/

static void Brw_ShowFileBrowsersAsgWrkUsr (void)
  {
   /***** Write top before showing file browser *****/
   Brw_WriteTopBeforeShowingFileBrowser ();

   /***** Show the tree with the assignments *****/
   Gbl.FileBrowser.Type = Brw_ADMI_ASG_USR;
   Brw_InitializeFileBrowser ();
   Brw_ShowFileBrowser ();

   /***** Show the tree with the works *****/
   Gbl.FileBrowser.Type = Brw_ADMI_WRK_USR;
   Brw_InitializeFileBrowser ();
   Brw_ShowFileBrowser ();
  }

/*****************************************************************************/
/**************** Form to change file zone (course or group) *****************/
/*****************************************************************************/

static void Brw_FormToChangeCrsGrpZone (void)
  {
   struct ListCodGrps LstMyGrps;
   unsigned NumGrp;
   struct GroupData GrpDat;
   bool IsCourseZone = Gbl.FileBrowser.Type == Brw_SHOW_DOC_CRS ||
                       Gbl.FileBrowser.Type == Brw_ADMI_DOC_CRS ||
                       Gbl.FileBrowser.Type == Brw_ADMI_TCH_CRS ||
                       Gbl.FileBrowser.Type == Brw_ADMI_SHR_CRS ||
                       Gbl.FileBrowser.Type == Brw_SHOW_MRK_CRS ||
                       Gbl.FileBrowser.Type == Brw_ADMI_MRK_CRS;
   bool IsGroupZone  = Gbl.FileBrowser.Type == Brw_SHOW_DOC_GRP ||
                       Gbl.FileBrowser.Type == Brw_ADMI_DOC_GRP ||
                       Gbl.FileBrowser.Type == Brw_ADMI_TCH_GRP ||
                       Gbl.FileBrowser.Type == Brw_ADMI_SHR_GRP ||
                       Gbl.FileBrowser.Type == Brw_SHOW_MRK_GRP ||
                       Gbl.FileBrowser.Type == Brw_ADMI_MRK_GRP;

   /***** Get list of groups to show *****/
   if (Gbl.Crs.Grps.NumGrps)	// This course has groups?
      /* Get list of group with file zones which I belong to */
      Grp_GetLstCodGrpsWithFileZonesIBelong (&LstMyGrps);

   /***** Begin form *****/
   Frm_StartForm (Brw_ActChgZone[Gbl.FileBrowser.Type]);
   Brw_PutHiddenParamFullTreeIfSelected ();

   /***** List start *****/
   HTM_UL_Begin ("class=\"LIST_LEFT\"");

   /***** Select the complete course, not a group *****/
   HTM_LI_Begin ("class=\"%s\"",IsCourseZone ? "BROWSER_TITLE" :
                                               "BROWSER_TITLE_LIGHT");
   HTM_LABEL_Begin (NULL);
   HTM_INPUT_RADIO ("GrpCod",true,
		    "value=\"-1\"%s",
		    IsCourseZone ? " checked=\"checked\"" : "");
   fprintf (Gbl.F.Out,"%s",Gbl.Hierarchy.Crs.FullName);
   HTM_LABEL_End ();
   HTM_LI_End ();

   /***** List my groups for unique selection *****/
   if (Gbl.Crs.Grps.NumGrps)	// This course has groups?
     {
      for (NumGrp = 0;
	   NumGrp < LstMyGrps.NumGrps;
	   NumGrp++)
        {
         /* Get next group */
         GrpDat.GrpCod = LstMyGrps.GrpCods[NumGrp];
         Grp_GetDataOfGroupByCod (&GrpDat);

         /* Select this group */
         HTM_LI_Begin ("class=\"%s\"",
		       (IsGroupZone &&
                        GrpDat.GrpCod == Gbl.Crs.Grps.GrpCod) ? "BROWSER_TITLE" :
                                                                "BROWSER_TITLE_LIGHT");
         HTM_IMG (Cfg_URL_ICON_PUBLIC,
		  NumGrp < LstMyGrps.NumGrps - 1 ? "submid20x20.gif" :
                	                           "subend20x20.gif",
		  NULL,
	          "class=\"ICO25x25\" style=\"margin-left:6px;\"");
         HTM_LABEL_Begin (NULL);
	 HTM_INPUT_RADIO ("GrpCod",true,
			  "value=\"%ld\"%s",
			  GrpDat.GrpCod,
			  (IsGroupZone &&
			   GrpDat.GrpCod == Gbl.Crs.Grps.GrpCod) ? " checked=\"checked\"" : "");
	 fprintf (Gbl.F.Out,"%s %s",GrpDat.GrpTypName,GrpDat.GrpName);
	 HTM_LABEL_End ();
	 HTM_LI_End ();
        }

      /***** Free memory with the list of groups I belong to *****/
      Grp_FreeListCodGrp (&LstMyGrps);
     }

   /***** End list and form *****/
   HTM_UL_End ();
   Frm_EndForm ();
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
            Lay_NoPermissionExit ();
         GrpDat->GrpCod = -1L;
        }
     }
  }

/*****************************************************************************/
/******** Show a row with the data of the owner of assignments/works *********/
/*****************************************************************************/

static void Brw_ShowDataOwnerAsgWrk (struct UsrData *UsrDat)
  {
   extern const char *Txt_View_record_for_this_course;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   Act_Action_t NextAction;

   /***** Show user's photo *****/
   HTM_TD_Begin ("class=\"OWNER_WORKS_PHOTO\"");
   ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                	                NULL,
                     "PHOTO60x80",Pho_ZOOM,false);
   HTM_TD_End ();

   /***** Begin form to send a message to this user *****/
   HTM_TD_Begin ("class=\"LT\"");

   HTM_DIV_Begin ("class=\"OWNER_WORKS_DATA AUTHOR_TXT\"");

   switch (UsrDat->Roles.InCurrentCrs.Role)
     {
      case Rol_STD:
	 NextAction = ActSeeRecOneStd;
	 break;
      case Rol_NET:
      case Rol_TCH:
	 NextAction = ActSeeRecOneTch;
	 break;
      default:
	 NextAction = ActUnk;
	 Rol_WrongRoleExit ();
	 break;
     }
   Frm_StartForm (NextAction);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);

   /***** Show user's ID *****/
   ID_WriteUsrIDs (UsrDat,NULL);

   /***** Show user's name *****/
   fprintf (Gbl.F.Out,"<br />");

   Frm_LinkFormSubmit (Txt_View_record_for_this_course,"AUTHOR_TXT",NULL);
   fprintf (Gbl.F.Out,"%s",UsrDat->Surname1);
   if (UsrDat->Surname2[0])
      fprintf (Gbl.F.Out," %s",UsrDat->Surname2);
   if (UsrDat->FirstName[0])
      fprintf (Gbl.F.Out,", %s",UsrDat->FirstName);
   Frm_LinkFormEnd ();

   /***** Show user's email *****/
   if (UsrDat->Email[0])
     {
      fprintf (Gbl.F.Out,"<br />");
      HTM_A_Begin ("href=\"mailto:%s\" target=\"_blank\" class=\"AUTHOR_TXT\"",
	           UsrDat->Email);
      fprintf (Gbl.F.Out,"%s",UsrDat->Email);
      HTM_A_End ();
     }
   Frm_EndForm ();

   HTM_DIV_End ();


   HTM_TD_End ();
  }

/*****************************************************************************/
/******* Get and check list of selected users, and show users' works  ********/
/*****************************************************************************/

void Brw_GetSelectedUsrsAndShowWorks (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (Brw_ShowFileBrowserOrWorks,	// when user(s) selected
                                  Brw_AskEditWorksCrs);		// when no user selected
  }

/*****************************************************************************/
/******************** Show a file browser or users' works  *******************/
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

   if (Brw_GetIfUsrAssigWorksFileBrowser ())
      Brw_ShowFileBrowsersAsgWrkUsr ();
   else if (Brw_GetIfCrsAssigWorksFileBrowser ())
      Brw_ShowFileBrowsersAsgWrkCrs ();
   else if (Brw_GetIfProjectFileBrowser ())
      Brw_ShowFileBrowserProject ();
   else
      Brw_ShowFileBrowserNormal ();

   /***** Help *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_ADMI_MRK_CRS:
      case Brw_ADMI_MRK_GRP:
         Ale_ShowAlert (Ale_INFO,Txt_Files_of_marks_must_contain_a_table_in_HTML_format_);
         break;
      default:
         break;
     }

   /***** Legal notice *****/
   Ale_ShowAlert (Ale_INFO,Txt_Disclaimer_the_files_hosted_here_,
                  Cfg_PLATFORM_SHORT_NAME,
                  Cfg_PLATFORM_RESPONSIBLE_EMAIL);
  }

/*****************************************************************************/
/**************************** Show a file browser ****************************/
/*****************************************************************************/

static void Brw_ShowFileBrowser (void)
  {
   extern const char *Hlp_FILES_Documents;
   extern const char *Hlp_FILES_Shared;
   extern const char *Hlp_FILES_Documents;
   extern const char *Hlp_FILES_Shared;
   extern const char *Hlp_FILES_Documents;
   extern const char *Hlp_FILES_Shared;
   extern const char *Hlp_FILES_Documents;
   extern const char *Hlp_FILES_Private;
   extern const char *Hlp_FILES_Shared;
   extern const char *Hlp_FILES_Homework_for_students;
   extern const char *Hlp_FILES_Homework_for_teachers;
   extern const char *Hlp_FILES_Marks;
   extern const char *Hlp_FILES_Briefcase;
   extern const char *Hlp_ASSESSMENT_Projects;

   extern const char *Txt_Documents_area;
   extern const char *Txt_Documents_management_area;
   extern const char *Txt_Teachers_files_area;
   extern const char *Txt_Shared_files_area;
   extern const char *Txt_Marks_area;
   extern const char *Txt_Marks_management_area;
   extern const char *Txt_Assignments_area;
   extern const char *Txt_Works_area;
   extern const char *Txt_Temporary_private_storage_area;
   extern const char *Txt_Project_documents;
   extern const char *Txt_Project_assessment;

   const char *Brw_TitleOfFileBrowser[Brw_NUM_TYPES_FILE_BROWSER];
   const char *Brw_HelpOfFileBrowser[Brw_NUM_TYPES_FILE_BROWSER];
   struct Brw_NumObjects Removed;
   char FileBrowserSectionId[32];
   bool IAmTeacherOrSysAdm = Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
	                     Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM;

   /***** Set title of file browser *****/
   Brw_TitleOfFileBrowser[Brw_UNKNOWN     ] = NULL;					// Brw_UNKNOWN
   Brw_TitleOfFileBrowser[Brw_SHOW_DOC_CRS] = Txt_Documents_area;			// Brw_SHOW_DOC_CRS
   Brw_TitleOfFileBrowser[Brw_SHOW_MRK_CRS] = Txt_Marks_area;				// Brw_SHOW_MRK_CRS
   Brw_TitleOfFileBrowser[Brw_ADMI_DOC_CRS] = Txt_Documents_management_area;		// Brw_ADMI_DOC_CRS
   Brw_TitleOfFileBrowser[Brw_ADMI_SHR_CRS] = Txt_Shared_files_area;			// Brw_ADMI_SHR_CRS
   Brw_TitleOfFileBrowser[Brw_ADMI_SHR_GRP] = Txt_Shared_files_area;			// Brw_ADMI_SHR_GRP
   Brw_TitleOfFileBrowser[Brw_ADMI_WRK_USR] = Txt_Works_area;				// Brw_ADMI_WRK_USR
   Brw_TitleOfFileBrowser[Brw_ADMI_WRK_CRS] = Txt_Works_area;				// Brw_ADMI_WRK_CRS
   Brw_TitleOfFileBrowser[Brw_ADMI_MRK_CRS] = Txt_Marks_management_area;		// Brw_ADMI_MRK_CRS
   Brw_TitleOfFileBrowser[Brw_ADMI_BRF_USR] = Txt_Temporary_private_storage_area;	// Brw_ADMI_BRF_USR
   Brw_TitleOfFileBrowser[Brw_SHOW_DOC_GRP] = Txt_Documents_area;			// Brw_SHOW_DOC_GRP
   Brw_TitleOfFileBrowser[Brw_ADMI_DOC_GRP] = Txt_Documents_management_area;		// Brw_ADMI_DOC_GRP
   Brw_TitleOfFileBrowser[Brw_SHOW_MRK_GRP] = Txt_Marks_area;				// Brw_SHOW_MRK_GRP
   Brw_TitleOfFileBrowser[Brw_ADMI_MRK_GRP] = Txt_Marks_management_area;		// Brw_ADMI_MRK_GRP
   Brw_TitleOfFileBrowser[Brw_ADMI_ASG_USR] = Txt_Assignments_area;			// Brw_ADMI_ASG_USR
   Brw_TitleOfFileBrowser[Brw_ADMI_ASG_CRS] = Txt_Assignments_area;			// Brw_ADMI_ASG_CRS
   Brw_TitleOfFileBrowser[Brw_SHOW_DOC_DEG] = Txt_Documents_area;			// Brw_SHOW_DOC_DEG
   Brw_TitleOfFileBrowser[Brw_ADMI_DOC_DEG] = Txt_Documents_management_area;		// Brw_ADMI_DOC_DEG
   Brw_TitleOfFileBrowser[Brw_SHOW_DOC_CTR] = Txt_Documents_area;			// Brw_SHOW_DOC_CTR
   Brw_TitleOfFileBrowser[Brw_ADMI_DOC_CTR] = Txt_Documents_management_area;		// Brw_ADMI_DOC_CTR
   Brw_TitleOfFileBrowser[Brw_SHOW_DOC_INS] = Txt_Documents_area;			// Brw_SHOW_DOC_INS
   Brw_TitleOfFileBrowser[Brw_ADMI_DOC_INS] = Txt_Documents_management_area;		// Brw_ADMI_DOC_INS
   Brw_TitleOfFileBrowser[Brw_ADMI_SHR_DEG] = Txt_Shared_files_area;			// Brw_ADMI_SHR_DEG
   Brw_TitleOfFileBrowser[Brw_ADMI_SHR_CTR] = Txt_Shared_files_area;			// Brw_ADMI_SHR_CTR
   Brw_TitleOfFileBrowser[Brw_ADMI_SHR_INS] = Txt_Shared_files_area;			// Brw_ADMI_SHR_INS
   Brw_TitleOfFileBrowser[Brw_ADMI_TCH_CRS] = Txt_Teachers_files_area;			// Brw_ADMI_TCH_CRS
   Brw_TitleOfFileBrowser[Brw_ADMI_TCH_GRP] = Txt_Teachers_files_area;			// Brw_ADMI_TCH_GRP
   Brw_TitleOfFileBrowser[Brw_ADMI_DOC_PRJ] = Txt_Project_documents;			// Brw_ADMI_DOC_PRJ
   Brw_TitleOfFileBrowser[Brw_ADMI_ASS_PRJ] = Txt_Project_assessment;			// Brw_ADMI_ASS_PRJ

   /***** Set help link of file browser *****/
   Brw_HelpOfFileBrowser[Brw_UNKNOWN     ] = NULL;				// Brw_UNKNOWN
   Brw_HelpOfFileBrowser[Brw_SHOW_DOC_CRS] = Hlp_FILES_Documents;		// Brw_SHOW_DOC_CRS
   Brw_HelpOfFileBrowser[Brw_SHOW_MRK_CRS] = Hlp_FILES_Marks;			// Brw_SHOW_MRK_CRS
   Brw_HelpOfFileBrowser[Brw_ADMI_DOC_CRS] = Hlp_FILES_Documents;		// Brw_ADMI_DOC_CRS
   Brw_HelpOfFileBrowser[Brw_ADMI_SHR_CRS] = Hlp_FILES_Shared;			// Brw_ADMI_SHR_CRS
   Brw_HelpOfFileBrowser[Brw_ADMI_SHR_GRP] = Hlp_FILES_Shared;			// Brw_ADMI_SHR_GRP
   Brw_HelpOfFileBrowser[Brw_ADMI_WRK_USR] = Hlp_FILES_Homework_for_students;	// Brw_ADMI_WRK_USR
   Brw_HelpOfFileBrowser[Brw_ADMI_WRK_CRS] = Hlp_FILES_Homework_for_teachers;	// Brw_ADMI_WRK_CRS
   Brw_HelpOfFileBrowser[Brw_ADMI_MRK_CRS] = Hlp_FILES_Marks;			// Brw_ADMI_MRK_CRS
   Brw_HelpOfFileBrowser[Brw_ADMI_BRF_USR] = Hlp_FILES_Briefcase;		// Brw_ADMI_BRF_USR
   Brw_HelpOfFileBrowser[Brw_SHOW_DOC_GRP] = Hlp_FILES_Documents;		// Brw_SHOW_DOC_GRP
   Brw_HelpOfFileBrowser[Brw_ADMI_DOC_GRP] = Hlp_FILES_Documents;		// Brw_ADMI_DOC_GRP
   Brw_HelpOfFileBrowser[Brw_SHOW_MRK_GRP] = Hlp_FILES_Marks;			// Brw_SHOW_MRK_GRP
   Brw_HelpOfFileBrowser[Brw_ADMI_MRK_GRP] = Hlp_FILES_Marks;			// Brw_ADMI_MRK_GRP
   Brw_HelpOfFileBrowser[Brw_ADMI_ASG_USR] = Hlp_FILES_Homework_for_students;	// Brw_ADMI_ASG_USR
   Brw_HelpOfFileBrowser[Brw_ADMI_ASG_CRS] = Hlp_FILES_Homework_for_teachers;	// Brw_ADMI_ASG_CRS
   Brw_HelpOfFileBrowser[Brw_SHOW_DOC_DEG] = Hlp_FILES_Documents;		// Brw_SHOW_DOC_DEG
   Brw_HelpOfFileBrowser[Brw_ADMI_DOC_DEG] = Hlp_FILES_Documents;		// Brw_ADMI_DOC_DEG
   Brw_HelpOfFileBrowser[Brw_SHOW_DOC_CTR] = Hlp_FILES_Documents;		// Brw_SHOW_DOC_CTR
   Brw_HelpOfFileBrowser[Brw_ADMI_DOC_CTR] = Hlp_FILES_Documents;		// Brw_ADMI_DOC_CTR
   Brw_HelpOfFileBrowser[Brw_SHOW_DOC_INS] = Hlp_FILES_Documents;		// Brw_SHOW_DOC_INS
   Brw_HelpOfFileBrowser[Brw_ADMI_DOC_INS] = Hlp_FILES_Documents;		// Brw_ADMI_DOC_INS
   Brw_HelpOfFileBrowser[Brw_ADMI_SHR_DEG] = Hlp_FILES_Shared;			// Brw_ADMI_SHR_DEG
   Brw_HelpOfFileBrowser[Brw_ADMI_SHR_CTR] = Hlp_FILES_Shared;			// Brw_ADMI_SHR_CTR
   Brw_HelpOfFileBrowser[Brw_ADMI_SHR_INS] = Hlp_FILES_Shared;			// Brw_ADMI_SHR_INS
   Brw_HelpOfFileBrowser[Brw_ADMI_TCH_CRS] = Hlp_FILES_Private;			// Brw_ADMI_TCH_CRS
   Brw_HelpOfFileBrowser[Brw_ADMI_TCH_GRP] = Hlp_FILES_Private;			// Brw_ADMI_TCH_GRP
   Brw_HelpOfFileBrowser[Brw_ADMI_DOC_PRJ] = Hlp_ASSESSMENT_Projects;		// Brw_ADMI_DOC_PRJ
   Brw_HelpOfFileBrowser[Brw_ADMI_ASS_PRJ] = Hlp_ASSESSMENT_Projects;		// Brw_ADMI_ASS_PRJ

   /***** Set contextual icon in box *****/
   Gbl.FileBrowser.IconViewEdit = Brw_ICON_NONE;
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_SHOW_DOC_INS:
	 if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
	    Gbl.FileBrowser.IconViewEdit = Brw_ICON_EDIT;
	 break;
      case Brw_ADMI_DOC_INS:
	 if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
	    Gbl.FileBrowser.IconViewEdit = Brw_ICON_VIEW;
	 break;
      case Brw_SHOW_DOC_CTR:
	 if (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM)
	    Gbl.FileBrowser.IconViewEdit = Brw_ICON_EDIT;
	 break;
      case Brw_ADMI_DOC_CTR:
	 if (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM)
	    Gbl.FileBrowser.IconViewEdit = Brw_ICON_VIEW;
	 break;
      case Brw_SHOW_DOC_DEG:
	 if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
	    Gbl.FileBrowser.IconViewEdit = Brw_ICON_EDIT;
	 break;
      case Brw_ADMI_DOC_DEG:
	 if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
	    Gbl.FileBrowser.IconViewEdit = Brw_ICON_VIEW;
	 break;
      case Brw_SHOW_DOC_CRS:
      case Brw_SHOW_DOC_GRP:
      case Brw_SHOW_MRK_CRS:
      case Brw_SHOW_MRK_GRP:
	 if (IAmTeacherOrSysAdm)
	    Gbl.FileBrowser.IconViewEdit = Brw_ICON_EDIT;
	 break;
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_MRK_CRS:
      case Brw_ADMI_MRK_GRP:
	 if (IAmTeacherOrSysAdm)
	    Gbl.FileBrowser.IconViewEdit = Brw_ICON_VIEW;
	 break;
      default:
	 break;
     }

   /***** Every time user clicks in menu option to view
          his/her (temporary) briefcase ==> remove old files *****/
   if (Gbl.Action.Act == ActAdmBrf)
      /* There are two reasons to not remove old files on any action in briefcase:
         1) To avoid wasting time unnecessarily
         2) To allow copying of files from briefcase,
            because the clipboard is deleted every time file browswer changes
      */
      Brw_RemoveOldFilesInBrowser (Brw_MAX_MONTHS_IN_BRIEFCASE,
                                   &Removed);	// Not used here

   /***** Check if the maximum quota has been exceeded *****/
   if (Brw_FileBrowserIsEditable[Gbl.FileBrowser.Type])
      Brw_SetAndCheckQuota ();

   /***** Check if the clipboard is in this tree *****/
   Gbl.FileBrowser.Clipboard.IsThisTree = Brw_CheckIfClipboardIsInThisTree ();

   /***** Begin box *****/
   Gbl.FileBrowser.Id++;
   snprintf (FileBrowserSectionId,sizeof (FileBrowserSectionId),
	     "file_browser_%u",
	     Gbl.FileBrowser.Id);
   HTM_SECTION_Begin (FileBrowserSectionId);
   Box_BoxBegin ("100%",Brw_TitleOfFileBrowser[Gbl.FileBrowser.Type],
                 Brw_PutIconsFileBrowser,
                 Brw_HelpOfFileBrowser[Gbl.FileBrowser.Type],Box_NOT_CLOSABLE);

   /***** Subtitle *****/
   Brw_WriteSubtitleOfFileBrowser ();

   /***** List recursively the directory *****/
   HTM_TABLE_Begin ("BROWSER_TABLE");
   Str_Copy (Gbl.FileBrowser.FilFolLnk.Path,Brw_RootFolderInternalNames[Gbl.FileBrowser.Type],
	     PATH_MAX);
   Str_Copy (Gbl.FileBrowser.FilFolLnk.Name,".",
	     NAME_MAX);
   Brw_SetFullPathInTree ();
   Gbl.FileBrowser.FilFolLnk.Type = Brw_IS_FOLDER;
   if (Brw_WriteRowFileBrowser (0,"1",
                                false,	// Tree not contracted
                                Brw_ICON_TREE_NOTHING))
      Brw_ListDir (1,"1",
                   false,	// Tree not contracted
                   Gbl.FileBrowser.Priv.PathRootFolder,
                   Brw_RootFolderInternalNames[Gbl.FileBrowser.Type]);
   HTM_TABLE_End ();

   /***** Show and store number of documents found *****/
   Brw_ShowAndStoreSizeOfFileTree ();

   /***** Put button to show / edit *****/
   Brw_PutButtonToShowEdit ();

   /***** End box *****/
   Box_BoxEnd ();
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/******************* Put contextual icons in file browser ********************/
/*****************************************************************************/

static void Brw_PutIconsFileBrowser (void)
  {
   /***** Put icon to view / edit file browser *****/
   switch (Gbl.FileBrowser.IconViewEdit)
     {
      case Brw_ICON_NONE:
	 break;
      case Brw_ICON_VIEW:
	 Ico_PutContextualIconToView (Brw_ActFromAdmToSee[Gbl.FileBrowser.Type],
	                              Brw_PutHiddenParamFullTreeIfSelected);
	 break;
      case Brw_ICON_EDIT:
         Ico_PutContextualIconToEdit (Brw_ActFromSeeToAdm[Gbl.FileBrowser.Type],
                                      Brw_PutHiddenParamFullTreeIfSelected);
	 break;
     }

   /***** Put icon to show a figure *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_ADMI_WRK_CRS:
      case Brw_ADMI_ASG_CRS:
      case Brw_ADMI_DOC_PRJ:
      case Brw_ADMI_ASS_PRJ:
	 break;
      default:
         Brw_PutIconShowFigure ();
         break;
     }
  }

static void Brw_PutIconShowFigure (void)
  {
   Gbl.Figures.FigureType = Fig_FOLDERS_AND_FILES;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/***************** Put button to view / edit file browser ********************/
/*****************************************************************************/

static void Brw_PutButtonToShowEdit (void)
  {
   extern const char *Txt_View;
   extern const char *Txt_Edit;

   switch (Gbl.FileBrowser.IconViewEdit)
     {
      case Brw_ICON_NONE:
	 break;
      case Brw_ICON_VIEW:
         if (Brw_ActFromAdmToSee[Gbl.FileBrowser.Type] != ActUnk)
           {
	    Frm_StartForm (Brw_ActFromAdmToSee[Gbl.FileBrowser.Type]);
	    Brw_PutHiddenParamFullTreeIfSelected ();
	    Btn_PutConfirmButton (Txt_View);
            Frm_EndForm ();
           }
	 break;
      case Brw_ICON_EDIT:
         if (Brw_ActFromSeeToAdm[Gbl.FileBrowser.Type] != ActUnk)
           {
	    Frm_StartForm (Brw_ActFromSeeToAdm[Gbl.FileBrowser.Type]);
	    Brw_PutHiddenParamFullTreeIfSelected ();
	    Btn_PutConfirmButton (Txt_Edit);
            Frm_EndForm ();
           }
	 break;
     }
  }

/*****************************************************************************/
/********************** Write title of a file browser ************************/
/*****************************************************************************/

static void Brw_WriteTopBeforeShowingFileBrowser (void)
  {
   /***** Update last access to this file browser *****/
   Brw_UpdateLastAccess ();

   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
   Brw_PutCheckboxFullTree ();	// Checkbox to show the full tree
   if (Brw_GetIfBriefcaseFileBrowser ())
     {
      if (Gbl.Action.Act != ActReqRemOldBrf)
	 Brw_PutLinkToAskRemOldFiles ();	// Remove old files
     }
   else if (Brw_GetIfCrsAssigWorksFileBrowser ())
     {
      if (!Gbl.FileBrowser.ZIP.CreateZIP)
	 ZIP_PutLinkToCreateZIPAsgWrk ();	// Create a zip file with the
						// works of the selected users
     }
   Mnu_ContextMenuEnd ();

   /***** Initialize hidden levels *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_SHOW_DOC_INS:
      case Brw_ADMI_DOC_INS:
      case Brw_SHOW_DOC_CTR:
      case Brw_ADMI_DOC_CTR:
      case Brw_SHOW_DOC_DEG:
      case Brw_ADMI_DOC_DEG:
      case Brw_SHOW_DOC_CRS:
      case Brw_ADMI_DOC_CRS:
      case Brw_SHOW_DOC_GRP:
      case Brw_ADMI_DOC_GRP:
      case Brw_SHOW_MRK_CRS:
      case Brw_ADMI_MRK_CRS:
      case Brw_SHOW_MRK_GRP:
      case Brw_ADMI_MRK_GRP:
         Brw_InitHiddenLevels ();
	 break;
      default:
	 break;
     }

   /***** If browser is editable, get and write current clipboard *****/
   if (Brw_FileBrowserIsEditable[Gbl.FileBrowser.Type])
      if (Brw_GetMyClipboard ())
	 Brw_WriteCurrentClipboard ();
  }

/*****************************************************************************/
/******************* Update last access to a file browser ********************/
/*****************************************************************************/

static void Brw_UpdateLastAccess (void)
  {
   /***** Get and update date and hour of last access to file browser *****/
   Brw_GetAndUpdateDateLastAccFileBrowser ();
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_SHOW_DOC_CRS:
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_TCH_CRS:
         if (Gbl.Action.Act == ActChgToSeeDocCrs ||
             Gbl.Action.Act == ActChgToAdmDocCrs ||
             Gbl.Action.Act == ActChgToAdmTch)	// Update group of last access to a documents/teachers zone only when user changes zone
            Brw_UpdateGrpLastAccZone ("LastDowGrpCod",-1L);
	 break;
      case Brw_SHOW_DOC_GRP:
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_GRP:
         if (Gbl.Action.Act == ActChgToSeeDocCrs ||
             Gbl.Action.Act == ActChgToAdmDocCrs ||
             Gbl.Action.Act == ActChgToAdmTch)	// Update group of last access to a documents/teachers zone only when user changes zone
            Brw_UpdateGrpLastAccZone ("LastDowGrpCod",Gbl.Crs.Grps.GrpCod);
         break;
      case Brw_ADMI_SHR_CRS:
         if (Gbl.Action.Act == ActChgToAdmSha) 	// Update group of last access to a shared files zone only when user changes zone
            Brw_UpdateGrpLastAccZone ("LastComGrpCod",-1L);
	 break;
      case Brw_ADMI_SHR_GRP:
         if (Gbl.Action.Act == ActChgToAdmSha) 	// Update group of last access to a shared files zone only when user changes zone
            Brw_UpdateGrpLastAccZone ("LastComGrpCod",Gbl.Crs.Grps.GrpCod);
	 break;
      case Brw_SHOW_MRK_CRS:
      case Brw_ADMI_MRK_CRS:
         if (Gbl.Action.Act == ActChgToSeeMrk ||
             Gbl.Action.Act == ActChgToAdmMrk)	// Update group of last access to a marks zone only when user changes zone
            Brw_UpdateGrpLastAccZone ("LastAssGrpCod",-1L);
	 break;
      case Brw_SHOW_MRK_GRP:
      case Brw_ADMI_MRK_GRP:
         if (Gbl.Action.Act == ActChgToSeeMrk ||
             Gbl.Action.Act == ActChgToAdmMrk)	// Update group of last access to a marks zone only when user changes zone
            Brw_UpdateGrpLastAccZone ("LastAssGrpCod",Gbl.Crs.Grps.GrpCod);
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
   /***** Update the group of my last access to a common zone *****/
   DB_QueryUPDATE ("can not update the group of the last access to a file browser",
		   "UPDATE crs_usr SET %s=%ld"
		   " WHERE CrsCod=%ld AND UsrCod=%ld",
                   FieldNameDB,GrpCod,
                   Gbl.Hierarchy.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*********************** Write title of a file browser ***********************/
/*****************************************************************************/

static void Brw_WriteSubtitleOfFileBrowser (void)
  {
   extern const char *Txt_accessible_only_for_reading_by_students_and_teachers_of_the_institution;
   extern const char *Txt_accessible_for_reading_and_writing_by_administrators_of_the_institution;
   extern const char *Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_institution;
   extern const char *Txt_accessible_only_for_reading_by_students_and_teachers_of_the_centre;
   extern const char *Txt_accessible_for_reading_and_writing_by_administrators_of_the_centre;
   extern const char *Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_centre;
   extern const char *Txt_accessible_only_for_reading_by_students_and_teachers_of_the_degree;
   extern const char *Txt_accessible_for_reading_and_writing_by_administrators_of_the_degree;
   extern const char *Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_degree;
   extern const char *Txt_accessible_only_for_reading_by_students_and_teachers_of_the_course;
   extern const char *Txt_accessible_only_for_reading_by_students_of_the_group_and_teachers_of_the_course;
   extern const char *Txt_accessible_for_reading_and_writing_by_teachers_of_the_course;
   extern const char *Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_course;
   extern const char *Txt_accessible_for_reading_and_writing_by_students_of_the_group_and_teachers_of_the_course;
   extern const char *Txt_accessible_for_reading_and_writing_by_you_and_the_teachers_of_the_course;
   extern const char *Txt_accessible_only_for_reading_by_you_and_the_teachers_of_the_course;
   extern const char *Txt_the_marks_of_a_student_chosen_at_random_;
   extern const char *Txt_accessible_for_reading_and_writing_by_project_members;
   extern const char *Txt_accessible_for_reading_and_writing_by_project_tutors_and_evaluators;
   extern const char *Txt_nobody_else_can_access_this_content;
   char Subtitle[1024 + Usr_MAX_BYTES_FULL_NAME];

   /***** Form to change zone (course and group browsers) *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_SHOW_DOC_CRS:
      case Brw_ADMI_DOC_CRS:
      case Brw_SHOW_DOC_GRP:
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_CRS:
      case Brw_ADMI_TCH_GRP:
      case Brw_ADMI_SHR_CRS:
      case Brw_ADMI_SHR_GRP:
      case Brw_SHOW_MRK_CRS:
      case Brw_ADMI_MRK_CRS:
      case Brw_SHOW_MRK_GRP:
      case Brw_ADMI_MRK_GRP:
         Brw_FormToChangeCrsGrpZone ();
	 break;
      default:
         break;
     }

   /***** Write subtitle *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_SHOW_DOC_INS:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_only_for_reading_by_students_and_teachers_of_the_institution);
	 break;
      case Brw_ADMI_DOC_INS:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_for_reading_and_writing_by_administrators_of_the_institution);
	 break;
      case Brw_ADMI_SHR_INS:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_institution);
	 break;
      case Brw_SHOW_DOC_CTR:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_only_for_reading_by_students_and_teachers_of_the_centre);
	 break;
      case Brw_ADMI_DOC_CTR:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_for_reading_and_writing_by_administrators_of_the_centre);
	 break;
      case Brw_ADMI_SHR_CTR:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_centre);
	 break;
      case Brw_SHOW_DOC_DEG:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_only_for_reading_by_students_and_teachers_of_the_degree);
	 break;
      case Brw_ADMI_DOC_DEG:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_for_reading_and_writing_by_administrators_of_the_degree);
	 break;
      case Brw_ADMI_SHR_DEG:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_degree);
	 break;
      case Brw_SHOW_DOC_CRS:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_only_for_reading_by_students_and_teachers_of_the_course);
	 break;
      case Brw_SHOW_DOC_GRP:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_only_for_reading_by_students_of_the_group_and_teachers_of_the_course);
	 break;
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_CRS:
      case Brw_ADMI_TCH_GRP:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_for_reading_and_writing_by_teachers_of_the_course);
	 break;
      case Brw_ADMI_SHR_CRS:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_course);
	 break;
      case Brw_ADMI_SHR_GRP:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_for_reading_and_writing_by_students_of_the_group_and_teachers_of_the_course);
	 break;
      case Brw_SHOW_MRK_CRS:
      case Brw_SHOW_MRK_GRP:
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_STD:
	       snprintf (Subtitle,sizeof (Subtitle),
                         "(%s)",
			 Txt_accessible_only_for_reading_by_you_and_the_teachers_of_the_course);
	       break;
	    case Rol_NET:
	    case Rol_TCH:
	    case Rol_SYS_ADM:
	       snprintf (Subtitle,sizeof (Subtitle),
                         "(%s)",
			 Txt_the_marks_of_a_student_chosen_at_random_);
	       break;
	    default:
	       Subtitle[0] = '\0';
	       Rol_WrongRoleExit ();
	       break;
	   }
 	 break;
      case Brw_ADMI_MRK_CRS:
      case Brw_ADMI_MRK_GRP:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_for_reading_and_writing_by_teachers_of_the_course);
	 break;
      case Brw_ADMI_ASG_USR:
      case Brw_ADMI_WRK_USR:
         snprintf (Subtitle,sizeof (Subtitle),
                   "%s<br />(%s)",
                   Gbl.Usrs.Me.UsrDat.FullName,
                   Txt_accessible_for_reading_and_writing_by_you_and_the_teachers_of_the_course);
	 break;
      case Brw_ADMI_ASG_CRS:
      case Brw_ADMI_WRK_CRS:
         snprintf (Subtitle,sizeof (Subtitle),
                   "%s",
                   Gbl.Usrs.Other.UsrDat.FullName);
	 break;
      case Brw_ADMI_DOC_PRJ:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_for_reading_and_writing_by_project_members);
	 break;
      case Brw_ADMI_ASS_PRJ:
         snprintf (Subtitle,sizeof (Subtitle),
                   "(%s)",
                   Txt_accessible_for_reading_and_writing_by_project_tutors_and_evaluators);
	 break;
      case Brw_ADMI_BRF_USR:
         snprintf (Subtitle,sizeof (Subtitle),
                   "%s<br />(%s)",
                   Gbl.Usrs.Me.UsrDat.FullName,
                   Txt_nobody_else_can_access_this_content);
	 break;
      case Brw_UNKNOWN:
         return;
     }
   if (Subtitle[0])
     {
      HTM_DIV_Begin ("class=\"BROWSER_SUBTITLE\"");
      fprintf (Gbl.F.Out,"%s",Subtitle);
      HTM_DIV_End ();
     }
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

static void Brw_ShowAndStoreSizeOfFileTree (void)
  {
   extern const char *Txt_level;
   extern const char *Txt_levels;
   extern const char *Txt_folder;
   extern const char *Txt_folders;
   extern const char *Txt_file;
   extern const char *Txt_files;
   extern const char *Txt_of_PART_OF_A_TOTAL;
   char FileSizeStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];

   HTM_DIV_Begin ("class=\"DAT CM\"");

   if (Brw_FileBrowserIsEditable[Gbl.FileBrowser.Type])
     {
      Fil_WriteFileSizeFull ((double) Gbl.FileBrowser.Size.TotalSiz,FileSizeStr);
      fprintf (Gbl.F.Out,"%u %s; %lu %s; %lu %s; %s",
	       Gbl.FileBrowser.Size.NumLevls,
	       Gbl.FileBrowser.Size.NumLevls == 1 ? Txt_level :
						    Txt_levels ,
	       Gbl.FileBrowser.Size.NumFolds,
	       Gbl.FileBrowser.Size.NumFolds == 1 ? Txt_folder :
						    Txt_folders,
	       Gbl.FileBrowser.Size.NumFiles,
	       Gbl.FileBrowser.Size.NumFiles == 1 ? Txt_file :
						    Txt_files,
	       FileSizeStr);

      if (Gbl.FileBrowser.Size.MaxQuota)
	{
	 Fil_WriteFileSizeBrief ((double) Gbl.FileBrowser.Size.MaxQuota,FileSizeStr);
	 fprintf (Gbl.F.Out," (%.1f%% %s %s)",
		  100.0 * ((double) Gbl.FileBrowser.Size.TotalSiz /
			   (double) Gbl.FileBrowser.Size.MaxQuota),
		  Txt_of_PART_OF_A_TOTAL,
		  FileSizeStr);
	}

      Brw_StoreSizeOfFileTreeInDB ();
     }
   else
     fprintf (Gbl.F.Out,"&nbsp;");	// Blank to occupy the same space as the text for the browser size

   HTM_DIV_End ();
  }

/*****************************************************************************/
/****************** Store size of a file browser in database *****************/
/*****************************************************************************/

static void Brw_StoreSizeOfFileTreeInDB (void)
  {
   long Cod = Brw_GetCodForFiles ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFiles ();

   /***** Update size of the file browser in database *****/
   DB_QueryREPLACE ("can not store the size of a file browser",
		    "REPLACE INTO file_browser_size"
		    " (FileBrowser,Cod,ZoneUsrCod,"
		    "NumLevels,NumFolders,NumFiles,TotalSize)"
		    " VALUES"
		    " (%u,%ld,%ld,"
		    "%u,'%lu','%lu','%llu')",
	            (unsigned) Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type],
		    Cod,ZoneUsrCod,
	            Gbl.FileBrowser.Size.NumLevls,
	            Gbl.FileBrowser.Size.NumFolds,
	            Gbl.FileBrowser.Size.NumFiles,
	            Gbl.FileBrowser.Size.TotalSiz);
  }

/*****************************************************************************/
/******** Remove files related to an institution from the database ***********/
/*****************************************************************************/

void Brw_RemoveInsFilesFromDB (long InsCod)
  {
   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views to files of an institution",
		   "DELETE FROM file_view USING file_view,files"
		   " WHERE files.FileBrowser IN (%u,%u) AND files.Cod=%ld"
		   " AND files.FilCod=file_view.FilCod",
	           (unsigned) Brw_ADMI_DOC_INS,
	           (unsigned) Brw_ADMI_SHR_INS,
	           InsCod);

   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders of an institution",
		   "DELETE LOW_PRIORITY FROM expanded_folders"
		   " WHERE FileBrowser IN (%u,%u) AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_INS,
	           (unsigned) Brw_ADMI_SHR_INS,
	           InsCod);

   /***** Remove from database the entries that store clipboards *****/
   DB_QueryDELETE ("can not remove clipboards"
		   " related to files of an institution",
		   "DELETE FROM clipboard"
		   " WHERE FileBrowser IN (%u,%u) AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_INS,
	           (unsigned) Brw_ADMI_SHR_INS,
	           InsCod);

   /***** Remove from database the entries that store
          the last time users visited file zones *****/
   DB_QueryDELETE ("can not remove file last visits"
		   " to files of an institution",
		   "DELETE FROM file_browser_last"
		   " WHERE FileBrowser IN (%u,%u) AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_INS,
	           (unsigned) Brw_ADMI_SHR_INS,
	           InsCod);

   /***** Remove from database the entries that store
          the sizes of the file zones *****/
   DB_QueryDELETE ("can not remove sizes of file zones of an institution",
		   "DELETE FROM file_browser_size"
		   " WHERE FileBrowser IN (%u,%u) AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_INS,
	           (unsigned) Brw_ADMI_SHR_INS,
	           InsCod);

   /***** Remove from database the entries that store the data files *****/
   DB_QueryDELETE ("can not remove files of an institution",
		   "DELETE FROM files"
		   " WHERE FileBrowser IN (%u,%u) AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_INS,
	           (unsigned) Brw_ADMI_SHR_INS,
	           InsCod);
  }

/*****************************************************************************/
/************ Remove files related to a centre from the database *************/
/*****************************************************************************/

void Brw_RemoveCtrFilesFromDB (long CtrCod)
  {
   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views to files of a centre",
		   "DELETE FROM file_view USING file_view,files"
		   " WHERE files.FileBrowser IN (%u,%u) AND files.Cod=%ld"
		   " AND files.FilCod=file_view.FilCod",
	           (unsigned) Brw_ADMI_DOC_CTR,
	           (unsigned) Brw_ADMI_SHR_CTR,
	           CtrCod);

   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders of a centre",
		   "DELETE LOW_PRIORITY FROM expanded_folders"
		   " WHERE FileBrowser IN (%u,%u) AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_CTR,
	           (unsigned) Brw_ADMI_SHR_CTR,
	           CtrCod);

   /***** Remove from database the entries that store clipboards *****/
   DB_QueryDELETE ("can not remove clipboards related to files of a centre",
		   "DELETE FROM clipboard"
		   " WHERE FileBrowser IN (%u,%u) AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_CTR,
	           (unsigned) Brw_ADMI_SHR_CTR,
	           CtrCod);

   /***** Remove from database the entries that store the last time users visited file zones *****/
   DB_QueryDELETE ("can not remove file last visits to files of a centre",
		   "DELETE FROM file_browser_last"
		   " WHERE FileBrowser IN (%u,%u) AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_CTR,
	           (unsigned) Brw_ADMI_SHR_CTR,
	           CtrCod);

   /***** Remove from database the entries that store the sizes of the file zones *****/
   DB_QueryDELETE ("can not remove sizes of file zones of a centre",
		   "DELETE FROM file_browser_size"
		   " WHERE FileBrowser IN (%u,%u) AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_CTR,
	           (unsigned) Brw_ADMI_SHR_CTR,
	           CtrCod);

   /***** Remove from database the entries that store the data files *****/
   DB_QueryDELETE ("can not remove files of a centre",
		   "DELETE FROM files"
		   " WHERE FileBrowser IN (%u,%u) AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_CTR,
	           (unsigned) Brw_ADMI_SHR_CTR,
	           CtrCod);
  }

/*****************************************************************************/
/************ Remove files related to a degree from the database *************/
/*****************************************************************************/

void Brw_RemoveDegFilesFromDB (long DegCod)
  {
   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views to files of a degree",
		   "DELETE FROM file_view USING file_view,files"
		   " WHERE files.FileBrowser IN (%u,%u) AND files.Cod=%ld"
		   " AND files.FilCod=file_view.FilCod",
	           (unsigned) Brw_ADMI_DOC_DEG,
	           (unsigned) Brw_ADMI_SHR_DEG,
	           DegCod);

   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders of a degree",
		   "DELETE LOW_PRIORITY FROM expanded_folders"
		   " WHERE FileBrowser IN (%u,%u) AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_DEG,
	           (unsigned) Brw_ADMI_SHR_DEG,
	           DegCod);

   /***** Remove from database the entries that store clipboards *****/
   DB_QueryDELETE ("can not remove clipboards related to files of a degree",
		   "DELETE FROM clipboard"
		   " WHERE FileBrowser IN (%u,%u) AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_DEG,
	           (unsigned) Brw_ADMI_SHR_DEG,
	           DegCod);

   /***** Remove from database the entries that store the last time users visited file zones *****/
   DB_QueryDELETE ("can not remove file last visits to files of a degree",
		   "DELETE FROM file_browser_last"
		   " WHERE FileBrowser IN (%u,%u) AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_DEG,
	           (unsigned) Brw_ADMI_SHR_DEG,
	           DegCod);

   /***** Remove from database the entries that store the sizes of the file zones *****/
   DB_QueryDELETE ("can not remove sizes of file zones of a degree",
		   "DELETE FROM file_browser_size"
		   " WHERE FileBrowser IN (%u,%u) AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_DEG,
	           (unsigned) Brw_ADMI_SHR_DEG,
	           DegCod);

   /***** Remove from database the entries that store the data files *****/
   DB_QueryDELETE ("can not remove files of a degree",
		   "DELETE FROM files"
		   " WHERE FileBrowser IN (%u,%u) AND Cod=%ld",
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

void Brw_RemoveCrsFilesFromDB (long CrsCod)
  {
   char SubqueryGrp[256];
   char SubqueryPrj[128];

   /***** Build subquery for groups *****/
   sprintf (SubqueryGrp,"(SELECT crs_grp.GrpCod FROM crs_grp_types,crs_grp"
		        " WHERE crs_grp_types.CrsCod=%ld"
		        " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod)",
            CrsCod);

   /***** Build subquery for projects *****/
   sprintf (SubqueryPrj,"(SELECT PrjCod FROM projects WHERE CrsCod=%ld)",
            CrsCod);

   /***** Remove format of files of marks *****/
   DB_QueryDELETE ("can not remove the properties of marks"
		   " associated to a course",
		   "DELETE FROM marks_properties USING files,marks_properties"
		   " WHERE files.FileBrowser=%u"
		   " AND files.Cod=%ld"
		   " AND files.FilCod=marks_properties.FilCod",
	           (unsigned) Brw_ADMI_MRK_CRS,
	           CrsCod);

   /***** Remove from database the entries that store the file views *****/
   /* Remove from course file zones */
   DB_QueryDELETE ("can not remove file views to files of a course",
		   "DELETE FROM file_view USING file_view,files"
		   " WHERE files.FileBrowser IN (%u,%u,%u,%u,%u,%u)"
		   " AND files.Cod=%ld"
		   " AND files.FilCod=file_view.FilCod",
	           (unsigned) Brw_ADMI_DOC_CRS,
	           (unsigned) Brw_ADMI_TCH_CRS,
	           (unsigned) Brw_ADMI_SHR_CRS,
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_WRK_USR,
	           (unsigned) Brw_ADMI_MRK_CRS,
	           CrsCod);

   /* Remove from group file zones */
   DB_QueryDELETE ("can not remove file views to files of a course",
		   "DELETE FROM file_view USING file_view,files"
		   " WHERE files.FileBrowser IN (%u,%u,%u,%u)"
		   " AND files.Cod IN %s"
		   " AND files.FilCod=file_view.FilCod",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           SubqueryGrp);

   /* Remove from project file zones */
   DB_QueryDELETE ("can not remove file views to files of a course",
		   "DELETE FROM file_view USING file_view,files"
		   " WHERE files.FileBrowser IN (%u,%u)"
		   " AND files.Cod IN %s"
		   " AND files.FilCod=file_view.FilCod",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           SubqueryPrj);

   /***** Remove from database expanded folders *****/
   /* Remove from course file zones */
   DB_QueryDELETE ("can not remove expanded folders of a course",
		   "DELETE LOW_PRIORITY FROM expanded_folders"
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
		   "DELETE LOW_PRIORITY FROM expanded_folders"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		   " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           SubqueryGrp);

   /* Remove from project file zones */
   DB_QueryDELETE ("can not remove expanded folders of a course",
		   "DELETE LOW_PRIORITY FROM expanded_folders"
		   " WHERE FileBrowser IN (%u,%u)"
		   " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           SubqueryPrj);

   /***** Remove from database the entries that store clipboards *****/
   /* Remove from course file zones */
   DB_QueryDELETE ("can not remove clipboards related to files of a course",
		   "DELETE FROM clipboard"
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
		   "DELETE FROM clipboard"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		   " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           SubqueryGrp);

   /* Remove from project file zones */
   DB_QueryDELETE ("can not remove clipboards related to files of a course",
		   "DELETE FROM clipboard"
		   " WHERE FileBrowser IN (%u,%u)"
		   " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           SubqueryPrj);

   /***** Remove from database the entries that store the last time users visited file zones *****/
   // Assignments and works are stored as one in file_browser_last...
   // ...because a user views them at the same time
   /* Remove from course file zones */
   DB_QueryDELETE ("can not remove file last visits to files of a course",
		   "DELETE FROM file_browser_last"
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
		    "DELETE FROM file_browser_last"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		   " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           SubqueryGrp);

   /* Remove from project file zones */
   DB_QueryDELETE ("can not remove file last visits to files of a course",
		   "DELETE FROM file_browser_last"
		   " WHERE FileBrowser IN (%u,%u)"
		   " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           SubqueryPrj);

   /***** Remove from database the entries that store the sizes of the file zones *****/
   /* Remove from course file zones */
   DB_QueryDELETE ("can not remove sizes of file zones of a course",
		   "DELETE FROM file_browser_size"
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
		   "DELETE FROM file_browser_size"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		   " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           SubqueryGrp);

   /* Remove from project file zones */
   DB_QueryDELETE ("can not remove sizes of file zones of a course",
		   "DELETE FROM file_browser_size"
		   " WHERE FileBrowser IN (%u,%u)"
		   " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           SubqueryPrj);

   /***** Remove from database the entries that store the data files *****/
   /* Remove from course file zones */
   DB_QueryDELETE ("can not remove files of a course",
		   "DELETE FROM files"
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
		   "DELETE FROM files"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		   " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           SubqueryGrp);

   /* Remove from project file zones */
   DB_QueryDELETE ("can not remove files of a course",
		   "DELETE FROM files"
		   " WHERE FileBrowser IN (%u,%u)"
		   " AND Cod IN %s",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           SubqueryPrj);
  }

/*****************************************************************************/
/************ Remove files related to a group from the database **************/
/*****************************************************************************/

void Brw_RemoveGrpFilesFromDB (long GrpCod)
  {
   /***** Remove format of files of marks *****/
   DB_QueryDELETE ("can not remove the properties of marks"
		   " associated to a group",
		   "DELETE FROM marks_properties USING files,marks_properties"
		   " WHERE files.FileBrowser=%u"
		   " AND files.Cod=%ld"
		   " AND files.FilCod=marks_properties.FilCod",
	           (unsigned) Brw_ADMI_MRK_GRP,
	           GrpCod);

   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views to files of a group",
		   "DELETE FROM file_view USING file_view,files"
		   " WHERE files.FileBrowser IN (%u,%u,%u,%u)"
		   " AND files.Cod=%ld"
		   " AND files.FilCod=file_view.FilCod",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           GrpCod);

   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders of a group",
		   "DELETE LOW_PRIORITY FROM expanded_folders"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		   " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           GrpCod);

   /***** Remove from database the entries that store clipboards *****/
   DB_QueryDELETE ("can not remove clipboards related to files of a group",
		   "DELETE FROM clipboard"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		   " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           GrpCod);

   /***** Remove from database the entries that store the last time users visited file zones *****/
   DB_QueryDELETE ("can not remove file last visits to files of a group",
		   "DELETE FROM file_browser_last"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		   " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           GrpCod);

   /***** Remove from database the entries that store the sizes of the file zones *****/
   DB_QueryDELETE ("can not remove sizes of file zones of a group",
		   "DELETE FROM file_browser_size"
		   " WHERE FileBrowser IN (%u,%u,%u,%u)"
		   " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           GrpCod);

   /***** Remove from database the entries that store the data files *****/
   DB_QueryDELETE ("can not remove files of a group",
		   "DELETE FROM files"
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

void Brw_RemovePrjFilesFromDB (long PrjCod)
  {
   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views to files of a project",
		   "DELETE FROM file_view USING file_view,files"
		   " WHERE files.FileBrowser IN (%u,%u)"
		   " AND files.Cod=%ld"
		   " AND files.FilCod=file_view.FilCod",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           PrjCod);

   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders of a project",
		   "DELETE LOW_PRIORITY FROM expanded_folders"
		   " WHERE FileBrowser IN (%u,%u)"
		   " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           PrjCod);

   /***** Remove from database the entries that store clipboards *****/
   DB_QueryDELETE ("can not remove clipboards related to files of a project",
		   "DELETE FROM clipboard"
		   " WHERE FileBrowser IN (%u,%u)"
		   " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           PrjCod);

   /***** Remove from database the entries that store the last time users visited file zones *****/
   DB_QueryDELETE ("can not remove file last visits to files of a project",
		   "DELETE FROM file_browser_last"
		   " WHERE FileBrowser IN (%u,%u)"
		   " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           PrjCod);

   /***** Remove from database the entries that store the sizes of the file zones *****/
   DB_QueryDELETE ("can not remove sizes of file zones of a project",
		   "DELETE FROM file_browser_size"
		   " WHERE FileBrowser IN (%u,%u)"
		   " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           PrjCod);

   /***** Remove from database the entries that store the data files *****/
   DB_QueryDELETE ("can not remove files of a project",
		   "DELETE FROM files"
		   " WHERE FileBrowser IN (%u,%u)"
		   " AND Cod=%ld",
	           (unsigned) Brw_ADMI_DOC_PRJ,
	           (unsigned) Brw_ADMI_ASS_PRJ,
	           PrjCod);
  }

/*****************************************************************************/
/* Remove some info about files related to a course and a user from database */
/*****************************************************************************/

void Brw_RemoveSomeInfoAboutCrsUsrFilesFromDB (long UsrCod,long CrsCod)
  {
   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders for a user in a course",
		   "DELETE LOW_PRIORITY FROM expanded_folders"
		   " WHERE UsrCod=%ld AND ("
		   "(FileBrowser IN (%u,%u,%u,%u,%u,%u,%u,%u)"
		   " AND Cod=%ld)"
		   " OR "
		   "(FileBrowser IN (%u,%u,%u,%u)"
		   " AND Cod IN"
		   " (SELECT crs_grp.GrpCod FROM crs_grp_types,crs_grp"
		   " WHERE crs_grp_types.CrsCod=%ld"
		   " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod))"
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
		   "DELETE FROM clipboard"
		   " WHERE UsrCod=%ld AND ("
		   "(FileBrowser IN (%u,%u,%u,%u,%u,%u,%u,%u)"
		   " AND Cod=%ld)"
		   " OR "
		   "(FileBrowser IN (%u,%u,%u,%u)"
		   " AND Cod IN"
		   " (SELECT crs_grp.GrpCod FROM crs_grp_types,crs_grp"
		   " WHERE crs_grp_types.CrsCod=%ld"
		   " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod))"
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
   // Assignments and works are stored as one in file_browser_last...
   // ...because a user views them at the same time
   DB_QueryDELETE ("can not remove file last visits to files of a course"
		   " from a user",
		   "DELETE FROM file_browser_last"
		   " WHERE UsrCod=%ld AND ("
		   "(FileBrowser IN (%u,%u,%u,%u,%u)"
		   " AND Cod=%ld)"
		   " OR "
		   "(FileBrowser IN (%u,%u,%u,%u)"
		   " AND Cod IN"
		   " (SELECT crs_grp.GrpCod FROM crs_grp_types,crs_grp"
		   " WHERE crs_grp_types.CrsCod=%ld"
		   " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod))"
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

void Brw_RemoveWrkFilesFromDB (long CrsCod,long UsrCod)
  {
   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views",
		   "DELETE FROM file_view USING file_view,files"
		   " WHERE files.FileBrowser IN (%u,%u)"
		   " AND files.Cod=%ld AND files.ZoneUsrCod=%ld"
		   " AND files.FilCod=file_view.FilCod",
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_WRK_USR,
	           CrsCod,UsrCod);

   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders of a group",
		   "DELETE LOW_PRIORITY FROM expanded_folders"
		   " WHERE FileBrowser IN (%u,%u)"
		   " AND Cod=%ld AND WorksUsrCod=%ld",
	           (unsigned) Brw_ADMI_ASG_CRS,
	           (unsigned) Brw_ADMI_WRK_CRS,
	           CrsCod,UsrCod);

   /***** Remove from database the entries that store clipboards *****/
   DB_QueryDELETE ("can not remove clipboards",
		   "DELETE FROM clipboard"
		   " WHERE FileBrowser IN (%u,%u)"
		   " AND Cod=%ld AND WorksUsrCod=%ld",
	           (unsigned) Brw_ADMI_ASG_CRS,
	           (unsigned) Brw_ADMI_WRK_CRS,
	           CrsCod,UsrCod);

   /***** Remove from database the entries that store the sizes of the file zones *****/
   DB_QueryDELETE ("can not remove file browser sizes",
		   "DELETE FROM file_browser_size"
		   " WHERE FileBrowser IN (%u,%u)"
		   " AND Cod=%ld AND ZoneUsrCod=%ld",
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_WRK_USR,
	           CrsCod,UsrCod);

   /***** Remove from database the entries that store the data files *****/
   DB_QueryDELETE ("can not remove files",
		   "DELETE FROM files"
		   " WHERE FileBrowser IN (%u,%u)"
		   " AND Cod=%ld AND ZoneUsrCod=%ld",
	           (unsigned) Brw_ADMI_ASG_USR,
	           (unsigned) Brw_ADMI_WRK_USR,
	           CrsCod,UsrCod);
  }

/*****************************************************************************/
/************* Remove files related to a user from the database **************/
/*****************************************************************************/

void Brw_RemoveUsrFilesFromDB (long UsrCod)
  {
   /***** Remove from database the entries that store the file views *****/
   // User is not removed from file_view table,
   // in order to take into account his/her views
   DB_QueryDELETE ("can not remove file views to files of a user",
		   "DELETE FROM file_view USING file_view,files"
		   " WHERE files.ZoneUsrCod=%ld"
		   " AND files.FilCod=file_view.FilCod",
	           UsrCod);

   /***** Remove from database expanded folders *****/
   DB_QueryDELETE ("can not remove expanded folders for a user",
		   "DELETE LOW_PRIORITY FROM expanded_folders"
		   " WHERE UsrCod=%ld",
	           UsrCod);

   /***** Remove from database the entries that store clipboards *****/
   DB_QueryDELETE ("can not remove user's clipboards",
		   "DELETE FROM clipboard"
		   " WHERE UsrCod=%ld",	// User's clipboard
	           UsrCod);

   /***** Remove from database the entries that store the last time users visited file zones *****/
   DB_QueryDELETE ("can not remove user's last visits to file zones",
		   "DELETE FROM file_browser_last"
		   " WHERE UsrCod=%ld",	// User's last visits to all zones
	           UsrCod);

   /***** Remove from database the entries that store the sizes of the file zones *****/
   DB_QueryDELETE ("can not remove sizes of user's file zones",
		   "DELETE FROM file_browser_size"
		   " WHERE ZoneUsrCod=%ld",
	           UsrCod);

   /***** Remove from database the entries that store the data files *****/
   DB_QueryDELETE ("can not remove files in user's file zones",
		   "DELETE FROM files"
		   " WHERE ZoneUsrCod=%ld",
	           UsrCod);
  }

/*****************************************************************************/
/************** Write a form to select whether show full tree ****************/
/*****************************************************************************/

static void Brw_PutCheckboxFullTree (void)
  {
   extern const char *Txt_Show_all_files;

   Lay_PutContextualCheckbox (Brw_ActSeeAdm[Gbl.FileBrowser.Type],
                              Brw_PutParamsFullTree,
                              "FullTree",
                              Gbl.FileBrowser.FullTree,false,
                              Txt_Show_all_files,Txt_Show_all_files);
  }

static void Brw_PutParamsFullTree (void)
  {
   if (Brw_GetIfGroupFileBrowser ())
      Grp_PutParamGrpCod (Gbl.Crs.Grps.GrpCod);
   else if (Brw_GetIfProjectFileBrowser ())	// This file browser needs specify a project
      Prj_PutParamPrjCod (Gbl.Prjs.PrjCod);
   else if (Brw_GetIfCrsAssigWorksFileBrowser ())
      Usr_PutHiddenParSelectedUsrsCods ();
  }

/*****************************************************************************/
/********* Put hidden parameter "full tree" if full tree is selected *********/
/*****************************************************************************/

void Brw_PutHiddenParamFullTreeIfSelected (void)
  {
   if (Gbl.FileBrowser.FullTree)
      Par_PutHiddenParamChar ("FullTree",'Y');
   // if not full tree selected ==> do not put hidden parameter
  }

/*****************************************************************************/
/******************* Get whether to show full tree from form *****************/
/*****************************************************************************/

static bool Brw_GetFullTreeFromForm (void)
  {
   return Par_GetParToBool ("FullTree");
  }

/*****************************************************************************/
/********* Get if the current file browser is a group file browser ***********/
/*****************************************************************************/

static bool Brw_GetIfGroupFileBrowser (void)
  {
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_SHOW_DOC_GRP:
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_GRP:
      case Brw_ADMI_SHR_GRP:
      case Brw_SHOW_MRK_GRP:
      case Brw_ADMI_MRK_GRP:
         return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/******** Get if the current file browser is a project file browser **********/
/*****************************************************************************/

static bool Brw_GetIfProjectFileBrowser (void)
  {
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_ADMI_DOC_PRJ:	// Project documents
      case Brw_ADMI_ASS_PRJ:	// Project assessment
         return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/****** Get if the current file browser is course assignments or works *******/
/*****************************************************************************/

static bool Brw_GetIfBriefcaseFileBrowser (void)
  {
   return (Gbl.FileBrowser.Type == Brw_ADMI_BRF_USR);
  }

/*****************************************************************************/
/****** Get if the current file browser is course assignments or works *******/
/*****************************************************************************/

static bool Brw_GetIfUsrAssigWorksFileBrowser (void)
  {
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_ADMI_ASG_USR:	// My assignments
      case Brw_ADMI_WRK_USR:	// My works
         return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/****** Get if the current file browser is course assignments or works *******/
/*****************************************************************************/

static bool Brw_GetIfCrsAssigWorksFileBrowser (void)
  {
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_ADMI_ASG_CRS:	// Course assignments
      case Brw_ADMI_WRK_CRS:	// Course works
         return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/******** Create a temporary public directory used to download files *********/
/*****************************************************************************/

void Brw_CreateDirDownloadTmp (void)
  {
   static unsigned NumDir = 0;	// When this function is called several times in the same execution of the program, each time a new directory is created
				// This happens when the trees of assignments and works of several users are being listed
   char PathUniqueDirL[PATH_MAX + 1];
   char PathUniqueDirR[PATH_MAX + 1];

   /* Example: /var/www/html/swad/tmp/SS/ujCNWsy4ZOdmgMKYBe0sKPAJu6szaZOQlIlJs_QIY */

   /***** If the public directory does not exist, create it *****/
   Fil_CreateDirIfNotExists (Cfg_PATH_FILE_BROWSER_TMP_PUBLIC);

   /***** Unique temporary directory.
          Important: number of directories inside a directory
          is limited to 32K in Linux ==> create directories in two levels *****/
   /* 1. Build the name of the directory, splitted in two parts: */
   /* 1a: 2 leftmost chars */
   Gbl.FileBrowser.TmpPubDir.L[0] = Gbl.UniqueNameEncrypted[0];
   Gbl.FileBrowser.TmpPubDir.L[1] = Gbl.UniqueNameEncrypted[1];
   Gbl.FileBrowser.TmpPubDir.L[2] = '\0';
   /* 1b: rest of chars */
   if (NumDir)
      snprintf (Gbl.FileBrowser.TmpPubDir.R,sizeof (Gbl.FileBrowser.TmpPubDir.R),
	        "%s_%u",
		&Gbl.UniqueNameEncrypted[2],NumDir);
   else
      Str_Copy (Gbl.FileBrowser.TmpPubDir.R,&Gbl.UniqueNameEncrypted[2],
                NAME_MAX);

   /* 2. Create the left directory */
   snprintf (PathUniqueDirL,sizeof (PathUniqueDirL),
	     "%s/%s",
             Cfg_PATH_FILE_BROWSER_TMP_PUBLIC,Gbl.FileBrowser.TmpPubDir.L);
   Fil_CreateDirIfNotExists (PathUniqueDirL);

   /* 3. Create the right directory inside the left one */
   snprintf (PathUniqueDirR,sizeof (PathUniqueDirR),
	     "%s/%s",
             PathUniqueDirL,Gbl.FileBrowser.TmpPubDir.R);
   if (mkdir (PathUniqueDirR,(mode_t) 0xFFF))
      Lay_ShowErrorAndExit ("Can not create a temporary folder for download.");

   /* 4. Increase number of directory for next call */
   NumDir++;
  }

/*****************************************************************************/
/* Get and update the date of my last access to file browser in this course **/
/*****************************************************************************/

static void Brw_GetAndUpdateDateLastAccFileBrowser (void)
  {
   long Cod;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get date of last accesss to a file browser from database *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_SHOW_DOC_INS:
      case Brw_ADMI_DOC_INS:
      case Brw_ADMI_SHR_INS:
	 Cod = Gbl.Hierarchy.Ins.InsCod;
         break;
      case Brw_SHOW_DOC_CTR:
      case Brw_ADMI_DOC_CTR:
      case Brw_ADMI_SHR_CTR:
	 Cod = Gbl.Hierarchy.Ctr.CtrCod;
         break;
      case Brw_SHOW_DOC_DEG:
      case Brw_ADMI_DOC_DEG:
      case Brw_ADMI_SHR_DEG:
	 Cod = Gbl.Hierarchy.Deg.DegCod;
         break;
      case Brw_SHOW_DOC_CRS:
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_TCH_CRS:
      case Brw_ADMI_SHR_CRS:
      case Brw_SHOW_MRK_CRS:
      case Brw_ADMI_MRK_CRS:
      case Brw_ADMI_ASG_USR:
      case Brw_ADMI_ASG_CRS:
      case Brw_ADMI_WRK_USR:
      case Brw_ADMI_WRK_CRS:
	 Cod = Gbl.Hierarchy.Crs.CrsCod;
         break;
      case Brw_SHOW_DOC_GRP:
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_GRP:
      case Brw_ADMI_SHR_GRP:
      case Brw_SHOW_MRK_GRP:
      case Brw_ADMI_MRK_GRP:
	 Cod = Gbl.Crs.Grps.GrpCod;
         break;
      case Brw_ADMI_DOC_PRJ:
      case Brw_ADMI_ASS_PRJ:
	 Cod = Gbl.Prjs.PrjCod;
         break;
      case Brw_ADMI_BRF_USR:
	 Cod = -1L;
	 break;
      default:
	 return;
     }
   NumRows = DB_QuerySELECT (&mysql_res,"can not get date-time"
					" of last access to a file browser",
			     "SELECT UNIX_TIMESTAMP(LastClick)"
			     " FROM file_browser_last"
			     " WHERE UsrCod=%ld AND FileBrowser=%u AND Cod=%ld",
			     Gbl.Usrs.Me.UsrDat.UsrCod,
			     (unsigned) Brw_FileBrowserForDB_file_browser_last[Gbl.FileBrowser.Type],
			     Cod);

   if (NumRows == 0)	// May be an administrator not belonging to this course
      Gbl.Usrs.Me.TimeLastAccToThisFileBrowser = LONG_MAX;	// Initialize to a big value in order to show files as old
   else if (NumRows == 1)
     {
      /* Get the date of the last access to file browser (row[0]) */
      row = mysql_fetch_row (mysql_res);
      if (row[0] == NULL)
         Gbl.Usrs.Me.TimeLastAccToThisFileBrowser = 0;
      else if (sscanf (row[0],"%ld",&Gbl.Usrs.Me.TimeLastAccToThisFileBrowser) != 1)
         Lay_ShowErrorAndExit ("Error when reading date-time of last access to a file browser.");
     }
   else
      Lay_ShowErrorAndExit ("Error when getting date-time of last access to a file browser.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Update date of my last access to file browser in this course *****/
   DB_QueryREPLACE ("can not update date of last access to a file browser",
		    "REPLACE INTO file_browser_last"
		    " (UsrCod,FileBrowser,Cod,LastClick)"
		    " VALUES"
		    " (%ld,%u,%ld,NOW())",
	            Gbl.Usrs.Me.UsrDat.UsrCod,
	            (unsigned) Brw_FileBrowserForDB_file_browser_last[Gbl.FileBrowser.Type],
	            Cod);
  }

/*****************************************************************************/
/************* Get the group of my last access to a common zone **************/
/*****************************************************************************/

static long Brw_GetGrpLastAccZone (const char *FieldNameDB)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   long GrpCod = -1L;

   /***** Get the group of my last access to a common zone from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get the group"
					" of your last access"
					" to a file browser",
			     "SELECT %s FROM crs_usr"
			     " WHERE CrsCod=%ld AND UsrCod=%ld",
			     FieldNameDB,
			     Gbl.Hierarchy.Crs.CrsCod,
			     Gbl.Usrs.Me.UsrDat.UsrCod);

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
   int NumFile;
   int NumFiles;
   char PathFileRel[PATH_MAX + 1];
   struct stat FileStatus;

   /***** Scan the directory *****/
   if ((NumFiles = scandir (Path,&FileList,NULL,NULL)) >= 0)	// No error
     {
      /***** Compute recursively the total number and size of the files and folders *****/
      for (NumFile = 0;
	   NumFile < NumFiles;
	   NumFile++)
	{
	 if (strcmp (FileList[NumFile]->d_name,".") &&
	     strcmp (FileList[NumFile]->d_name,".."))	// Skip directories "." and ".."
	   {
	    /* There are files in this directory ==> update level */
	    if (Level > Gbl.FileBrowser.Size.NumLevls)
	       Gbl.FileBrowser.Size.NumLevls++;

	    /* Update counters depending on whether it's a directory or a regular file */
	    snprintf (PathFileRel,sizeof (PathFileRel),
		      "%s/%s",
		      Path,FileList[NumFile]->d_name);
	    if (lstat (PathFileRel,&FileStatus))	// On success ==> 0 is returned
	       Lay_ShowErrorAndExit ("Can not get information about a file or folder.");
	    else if (S_ISDIR (FileStatus.st_mode))		// It's a directory
	      {
	       Gbl.FileBrowser.Size.NumFolds++;
	       Gbl.FileBrowser.Size.TotalSiz += (unsigned long long) FileStatus.st_size;
	       Brw_CalcSizeOfDirRecursive (Level + 1,PathFileRel);
	      }
	    else if (S_ISREG (FileStatus.st_mode))		// It's a regular file
	      {
	       Gbl.FileBrowser.Size.NumFiles++;
	       Gbl.FileBrowser.Size.TotalSiz += (unsigned long long) FileStatus.st_size;
	      }
	   }
	 free (FileList[NumFile]);
	}
      free (FileList);
     }
   else
      Lay_ShowErrorAndExit ("Error while scanning directory.");
  }

/*****************************************************************************/
/************************ List a directory recursively ***********************/
/*****************************************************************************/

static void Brw_ListDir (unsigned Level,const char *ParentRowId,
                         bool TreeContracted,
                         const char Path[PATH_MAX + 1],
                         const char PathInTree[PATH_MAX + 1])
  {
   struct dirent **FileList;
   struct dirent **SubdirFileList;
   int NumFile;
   int NumFiles;
   int NumFileInSubdir;
   int NumFilesInSubdir;
   unsigned NumRow;
   char RowId[Brw_MAX_ROW_ID + 1];
   char PathFileRel[PATH_MAX + 1];
   char PathFileInExplTree[PATH_MAX + 1];
   struct stat FileStatus;
   Brw_IconTree_t IconSubtree = Brw_ICON_TREE_NOTHING;	// Initialized to avoid warning

   /***** Scan directory *****/
   if ((NumFiles = scandir (Path,&FileList,NULL,alphasort)) >= 0)	// No error
     {
      /***** List files *****/
      for (NumFile = 0, NumRow = 0;
	   NumFile < NumFiles;
	   NumFile++)
	{
	 if (strcmp (FileList[NumFile]->d_name,".") &&
	     strcmp (FileList[NumFile]->d_name,".."))	// Skip directories "." and ".."
	   {
	    /***** Construct the full path of the file or folder *****/
	    snprintf (PathFileRel,sizeof (PathFileRel),
		      "%s/%s",
		      Path      ,FileList[NumFile]->d_name);
	    snprintf (PathFileInExplTree,sizeof (PathFileInExplTree),
		      "%s/%s",
		      PathInTree,FileList[NumFile]->d_name);

	    Str_Copy (Gbl.FileBrowser.FilFolLnk.Path,PathInTree,
		      PATH_MAX);
	    Str_Copy (Gbl.FileBrowser.FilFolLnk.Name,FileList[NumFile]->d_name,
		      NAME_MAX);
	    Brw_SetFullPathInTree ();

	    /***** Add number of row to parent row id *****/
	    NumRow++;
	    snprintf (RowId,sizeof (RowId),
		      "%s_%u",
		      ParentRowId,NumRow);

	    /***** Get file or folder status *****/
	    if (lstat (PathFileRel,&FileStatus))	// On success ==> 0 is returned
	       Lay_ShowErrorAndExit ("Can not get information about a file or folder.");
	    else if (S_ISDIR (FileStatus.st_mode))	// It's a directory
	      {
	       Gbl.FileBrowser.FilFolLnk.Type = Brw_IS_FOLDER;

	       if (Gbl.FileBrowser.FullTree)
		  IconSubtree = Brw_ICON_TREE_NOTHING;
	       else
		 {
		  /***** Check if this subdirectory has files or folders in it *****/
		  if ((NumFilesInSubdir = scandir (PathFileRel,&SubdirFileList,NULL,NULL)) >= 0)	// No error
		    {
		     if (NumFilesInSubdir <= 2)
			IconSubtree = Brw_ICON_TREE_NOTHING;
		     else
			/***** Check if the tree starting at this subdirectory must be expanded *****/
			IconSubtree = Brw_GetIfExpandedTree (Gbl.FileBrowser.FilFolLnk.Full) ? Brw_ICON_TREE_CONTRACT :
												    Brw_ICON_TREE_EXPAND;
		     for (NumFileInSubdir = 0;
			  NumFileInSubdir < NumFilesInSubdir;
			  NumFileInSubdir++)
			free (SubdirFileList[NumFileInSubdir]);
		     free (SubdirFileList);
		    }
		  else
		     Lay_ShowErrorAndExit ("Error while scanning directory.");
		 }

	       /***** Write a row for the subdirectory *****/
	       if (Brw_WriteRowFileBrowser (Level,RowId,
	                                    TreeContracted,
	                                    IconSubtree))
		  if (Level < Brw_MAX_DIR_LEVELS)
		     /* List subtree starting at this this directory */
		     Brw_ListDir (Level + 1,RowId,
		                  TreeContracted || IconSubtree == Brw_ICON_TREE_EXPAND,
				  PathFileRel,PathFileInExplTree);
	      }
	    else if (S_ISREG (FileStatus.st_mode))	// It's a regular file
	      {
	       Gbl.FileBrowser.FilFolLnk.Type = Str_FileIs (Gbl.FileBrowser.FilFolLnk.Name,"url") ? Brw_IS_LINK :
					                                                            Brw_IS_FILE;
	       Brw_WriteRowFileBrowser (Level,RowId,
	                                TreeContracted,
	                                Brw_ICON_TREE_NOTHING);
	      }
	   }
	 free (FileList[NumFile]);
	}
      free (FileList);
     }
   else
      Lay_ShowErrorAndExit ("Error while scanning directory.");
  }

/*****************************************************************************/
/*********************** Write a row of a file browser ***********************/
/*****************************************************************************/
// If it is the first row (root folder), always show it
// If it is not the first row, it is shown or not depending on whether it is hidden or not
// If the row is visible, return true. If it is hidden, return false

static bool Brw_WriteRowFileBrowser (unsigned Level,const char *RowId,
                                     bool TreeContracted,
                                     Brw_IconTree_t IconThisRow)
  {
   char *Anchor;
   bool RowSetAsHidden = false;
   bool RowSetAsPublic = false;
   bool LightStyle = false;
   bool IsRecent = false;
   struct FileMetadata FileMetadata;
   char FileBrowserId[32];
   bool SeeDocsZone     = Gbl.FileBrowser.Type == Brw_SHOW_DOC_INS ||
	                  Gbl.FileBrowser.Type == Brw_SHOW_DOC_CTR ||
	                  Gbl.FileBrowser.Type == Brw_SHOW_DOC_DEG ||
	                  Gbl.FileBrowser.Type == Brw_SHOW_DOC_CRS ||
                          Gbl.FileBrowser.Type == Brw_SHOW_DOC_GRP;
   bool AdminDocsZone   = Gbl.FileBrowser.Type == Brw_ADMI_DOC_INS ||
                          Gbl.FileBrowser.Type == Brw_ADMI_DOC_CTR ||
                          Gbl.FileBrowser.Type == Brw_ADMI_DOC_DEG ||
                          Gbl.FileBrowser.Type == Brw_ADMI_DOC_CRS ||
                          Gbl.FileBrowser.Type == Brw_ADMI_DOC_GRP;
   bool SharedZone      = Gbl.FileBrowser.Type == Brw_ADMI_SHR_INS ||
                          Gbl.FileBrowser.Type == Brw_ADMI_SHR_CTR ||
                          Gbl.FileBrowser.Type == Brw_ADMI_SHR_DEG ||
                          Gbl.FileBrowser.Type == Brw_ADMI_SHR_CRS ||
                          Gbl.FileBrowser.Type == Brw_ADMI_SHR_GRP;
   bool AssignmentsZone = Gbl.FileBrowser.Type == Brw_ADMI_ASG_USR ||
                          Gbl.FileBrowser.Type == Brw_ADMI_ASG_CRS;
   bool SeeMarks        = Gbl.FileBrowser.Type == Brw_SHOW_MRK_CRS ||
                          Gbl.FileBrowser.Type == Brw_SHOW_MRK_GRP;
   bool AdminMarks      = Gbl.FileBrowser.Type == Brw_ADMI_MRK_CRS ||
                          Gbl.FileBrowser.Type == Brw_ADMI_MRK_GRP;

   /***** Initializations *****/
   Gbl.FileBrowser.Clipboard.IsThisFile = false;
   snprintf (FileBrowserId,sizeof (FileBrowserId),
	     "fil_brw_%u",
	     Gbl.FileBrowser.Id);

   /***** Is this row hidden or visible? *****/
   if (SeeDocsZone || AdminDocsZone ||
       SeeMarks    || AdminMarks)
     {
      RowSetAsHidden = Brw_CheckIfFileOrFolderIsSetAsHiddenInDB (Gbl.FileBrowser.FilFolLnk.Type,
                                                                 Gbl.FileBrowser.FilFolLnk.Full);
      if (RowSetAsHidden && Level && (SeeDocsZone || SeeMarks))
         return false;
      if (AdminDocsZone || AdminMarks)
        {
         if (RowSetAsHidden)	// this row is marked as hidden
           {
            if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER)
               Gbl.FileBrowser.HiddenLevels[Level] = true;
            LightStyle = true;
           }
         else			// this row is not marked as hidden
           {
            if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER)
               Gbl.FileBrowser.HiddenLevels[Level] = false;
            LightStyle = Brw_CheckIfAnyUpperLevelIsHidden (Level);
           }
        }
     }

   /***** Get file metadata *****/
   Brw_GetFileMetadataByPath (&FileMetadata);
   Brw_GetFileTypeSizeAndDate (&FileMetadata);
   if (FileMetadata.FilCod <= 0)	// No entry for this file in database table of files
      /* Add entry to the table of files/folders */
      FileMetadata.FilCod = Brw_AddPathToDB (-1L,FileMetadata.FilFolLnk.Type,
                                             Gbl.FileBrowser.FilFolLnk.Full,false,Brw_LICENSE_DEFAULT);

   /***** Is this row public or private? *****/
   if (SeeDocsZone || AdminDocsZone || SharedZone)
     {
      RowSetAsPublic = (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER) ? Brw_GetIfFolderHasPublicFiles (Gbl.FileBrowser.FilFolLnk.Full) :
	                                                             FileMetadata.IsPublic;
      if (Gbl.FileBrowser.ShowOnlyPublicFiles && !RowSetAsPublic)
         return false;
     }

   /***** Check if is a recent file or folder *****/
   // If less than a week since last modify ==> indicate the file is recent by writting its name in green
   if (Gbl.StartExecutionTimeUTC < FileMetadata.Time + (7L * 24L * 60L * 60L))
      IsRecent = true;

   /* Style of the text in this row */
   Gbl.FileBrowser.TxtStyle   = (LightStyle ? (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER || !IsRecent ? "LST_HID" :
	                                                                                                "LST_REC_HID") :
                                              (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER || !IsRecent ? "LST" :
                                        	                                                        "LST_REC"));
   Gbl.FileBrowser.InputStyle = (LightStyle ? (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER || !IsRecent ? "LST_EDIT_HID" :
	                                                                                                "LST_EDIT_REC_HID") :
                                              (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER || !IsRecent ? "LST_EDIT" :
                                        	                                                        "LST_EDIT_REC"));

   /***** Get data of assignment using the name of the folder *****/
   if (AssignmentsZone)
     {
      if (Level == 1)	// Main folder of the assignment
	{
	 Str_Copy (Gbl.FileBrowser.Asg.Folder,Gbl.FileBrowser.FilFolLnk.Name,
	           Brw_MAX_BYTES_FOLDER);
	 Asg_GetDataOfAssignmentByFolder (&Gbl.FileBrowser.Asg);
	 // The data of this assignment remains in Gbl.FileBrowser.Asg
	 // for all subsequent rows with Level > 1 (files or folders inside this folder),
	 // and they are overwritten on the next row with level == 1 (next assignment)
	}
     }
   else	// Not an assignment zone
      Gbl.FileBrowser.Asg.AsgCod = -1L;

   /***** Start this row *****/
   if (asprintf (&Anchor,"fil_brw_%u_%s",
		 Gbl.FileBrowser.Id,RowId) < 0)
      Lay_NotEnoughMemoryExit ();
   switch (IconThisRow)
     {
      case Brw_ICON_TREE_NOTHING:
	 if (TreeContracted)	// This row is inside a contracted subtree
            HTM_TR_Begin ("id=\"%s\" style=\"display:none;\"",Anchor);
	 else
            HTM_TR_Begin ("id=\"%s\"",Anchor);
	 break;
      case Brw_ICON_TREE_EXPAND:
	 if (TreeContracted)	// This row is inside a contracted subtree
            HTM_TR_Begin ("id=\"%s\" data-folder=\"contracted\" style=\"display:none;\"",Anchor);
	 else
            HTM_TR_Begin ("id=\"%s\" data-folder=\"contracted\"",Anchor);
	 break;
      case Brw_ICON_TREE_CONTRACT:
	 if (TreeContracted)	// This row is inside a contracted subtree
            HTM_TR_Begin ("id=\"%s\" data-folder=\"expanded\" style=\"display:none;\"",Anchor);
	 else
            HTM_TR_Begin ("id=\"%s\" data-folder=\"expanded\"",Anchor);
	 break;
     }

   /****** If current action allows file administration... ******/
   Gbl.FileBrowser.ICanEditFileOrFolder = false;
   if (Brw_FileBrowserIsEditable[Gbl.FileBrowser.Type] &&
       !Gbl.FileBrowser.ShowOnlyPublicFiles)
     {
      if (Gbl.FileBrowser.Clipboard.IsThisTree &&
	  Level != 0)	// Never copy root folder
	 // If path in the clipboard is equal to complete path in tree...
	 // ...or is the start of complete path in tree...
         if (Str_Path1BeginsByPath2 (Gbl.FileBrowser.FilFolLnk.Full,Gbl.FileBrowser.Clipboard.FilFolLnk.Full))
            Gbl.FileBrowser.Clipboard.IsThisFile = true;

      /* Check if I can modify (remove, rename, etc.) this file or folder */
      Gbl.FileBrowser.ICanEditFileOrFolder = Brw_CheckIfICanEditFileOrFolder (Level);

      /* Icon to remove folder, file or link */
      Brw_PutIconRemove ();

      /* Icon to copy */
      Brw_PutIconCopy ();

      /* Icon to paste */
      Brw_PutIconPaste (Level);
     }

   /***** Indentation depending on level, icon, and file/folder name *****/
   /* Start column */
   HTM_TD_Begin ("class=\"NO_BR LT COLOR%u\" style=\"width:99%%;\"",Gbl.RowEvenOdd);

   HTM_TABLE_Begin (NULL);
   HTM_TR_Begin (NULL);

   /* Indent depending on level */
   if (Level)
      Brw_IndentAndWriteIconExpandContract (Level,FileBrowserId,RowId,IconThisRow);

   /* Put icon to show/hide file or folder */
   if (AdminDocsZone || AdminMarks)
     {
      if (RowSetAsHidden)	// this row is marked as hidden
         Brw_PutIconShow (Anchor);
      else			// this row is not marked as hidden
         Brw_PutIconHide (Anchor);
     }

   /***** File or folder icon *****/
   if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER)
      /* Icon with folder */
      Brw_PutIconFolder (Level,FileBrowserId,RowId,IconThisRow);
   else	// File or link
     {
      /* Icon with file type or link */
      HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);
      Brw_PutIconFileWithLinkToViewMetadata (16,&FileMetadata);
      HTM_TD_End ();
     }

   /* Check if is a new file or folder */
   // If our last access was before the last modify ==> indicate the file is new by putting a blinking star
   if (Gbl.Usrs.Me.TimeLastAccToThisFileBrowser < FileMetadata.Time)
      Brw_PutIconNewFileOrFolder ();

   /* File or folder name */
   Brw_WriteFileName (Level,FileMetadata.IsPublic);

   /* End column */
   HTM_TR_End ();
   HTM_TABLE_End ();
   HTM_TD_End ();

   if (AdminMarks)
      /***** Header and footer rows *****/
      Mrk_GetAndWriteNumRowsHeaderAndFooter ();

   if (AssignmentsZone && Level == 1)
      /***** Start and end dates of assignment *****/
      Brw_WriteDatesAssignment ();
   else
      /***** File date and size *****/
      Brw_WriteFileSizeAndDate (&FileMetadata);

   if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER)
     {
      /***** Put icon to download ZIP of folder *****/
      HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);
      if (Gbl.Usrs.Me.Role.Logged >= Rol_STD &&	// Only ZIP folders if I am student, teacher...
	  !SeeMarks &&				// Do not ZIP folders when seeing marks
	  !(SeeDocsZone && RowSetAsHidden))	// When seeing docs, if folder is not hidden (this could happen for Level == 0)
	 Brw_PutButtonToDownloadZIPOfAFolder ();
      HTM_TD_End ();
     }
   else	// File or link
      /***** User who created the file or folder *****/
      Brw_WriteFileOrFolderPublisher (Level,FileMetadata.PublisherUsrCod);

   /***** End this row *****/
   free (Anchor);
   HTM_TR_End ();

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;

   if (RowSetAsHidden && (SeeDocsZone || SeeMarks))
      return false;
   return true;
  }

/*****************************************************************************/
/*************** Construct full path in tree of file browser *****************/
/*****************************************************************************/
// If, for example, Path is "descarga/teoria" and Name is "tema1.pdf"
// then FullPathInTree will be "descarga/teoria/tema1.pdf"
// If Name is ".", then FullPathInTree will be equal to Path

void Brw_SetFullPathInTree (void)
  {
   char FullPath[PATH_MAX + 1 + NAME_MAX + 1];

   if (!Gbl.FileBrowser.FilFolLnk.Path[0])
      Gbl.FileBrowser.FilFolLnk.Full[0] = '\0';
   else if (strcmp (Gbl.FileBrowser.FilFolLnk.Name,"."))
     {
      snprintf (FullPath,sizeof (FullPath),
	        "%s/%s",
		Gbl.FileBrowser.FilFolLnk.Path,Gbl.FileBrowser.FilFolLnk.Name);
      Str_Copy (Gbl.FileBrowser.FilFolLnk.Full,FullPath,
	        PATH_MAX);
     }
   else	// It's the root folder
      Str_Copy (Gbl.FileBrowser.FilFolLnk.Full,Gbl.FileBrowser.FilFolLnk.Path,
	        PATH_MAX);
  }

/*****************************************************************************/
/******* Check if the clipboard can be pasted into the current folder ********/
/*****************************************************************************/
// Return true if Gbl.FileBrowser.Clipboard.FilFolLnk.Full can be pasted into Gbl.FileBrowser.FilFolLnk.Full

static bool Brw_CheckIfCanPasteIn (unsigned Level)
  {
   char PathDstWithFile[PATH_MAX + 1 + NAME_MAX + 1];

   /***** If not in a folder... *****/
   if (Gbl.FileBrowser.FilFolLnk.Type != Brw_IS_FOLDER)
      return false;

   /***** If there is nothing in clipboard... *****/
   if (Gbl.FileBrowser.Clipboard.FileBrowser == Brw_UNKNOWN)
      return false;

   /***** Do not paste a link in marks... *****/
   if (Gbl.FileBrowser.Clipboard.FilFolLnk.Type == Brw_IS_LINK &&
       (Gbl.FileBrowser.Type == Brw_ADMI_MRK_CRS ||
        Gbl.FileBrowser.Type == Brw_ADMI_MRK_GRP))
      return false;

   /**** If I can not create elements into this folder... *****/
   if (!Brw_CheckIfICanCreateIntoFolder (Level))
      return false;	// Pasting into top level of assignments is forbidden

   /**** If we are in the same tree of the clipboard... *****/
   if (Gbl.FileBrowser.Clipboard.IsThisTree)	// We are in the same tree of the clipboard ==> we can paste or not depending on the subtree
     {
      /***** Construct the name of the file or folder destination *****/
      snprintf (PathDstWithFile,sizeof (PathDstWithFile),
	        "%s/%s",
	        Gbl.FileBrowser.FilFolLnk.Full,
	        Gbl.FileBrowser.Clipboard.FilFolLnk.Name);

      return !Str_Path1BeginsByPath2 (PathDstWithFile,
		                      Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
     }

   return true;	// I can paste
  }

/*****************************************************************************/
/********** Write link and icon to remove a file, link or folder *************/
/*****************************************************************************/

static void Brw_PutIconRemove (void)
  {
   HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);

   if (Gbl.FileBrowser.ICanEditFileOrFolder)	// Can I remove this?
      switch (Gbl.FileBrowser.FilFolLnk.Type)
	{
	 case Brw_IS_FILE:
	 case Brw_IS_LINK:
	    /***** Form to remove a file or link *****/
	    Ico_PutContextualIconToRemove (Brw_ActAskRemoveFile[Gbl.FileBrowser.Type],
					   Brw_PutImplicitParamsFileBrowser);
	    break;
	 case Brw_IS_FOLDER:
	    /***** Form to remove a folder *****/
	    Ico_PutContextualIconToRemove (Brw_ActRemoveFolder[Gbl.FileBrowser.Type],
					   Brw_PutImplicitParamsFileBrowser);
	    break;
	 default:
	    break;
	}
   else
      Ico_PutIconRemovalNotAllowed ();

   HTM_TD_End ();
  }

/*****************************************************************************/
/************* Write link and icon to copy a file o a folder *****************/
/*****************************************************************************/

static void Brw_PutIconCopy (void)
  {
   HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);

   /***** Form to copy into the clipboard *****/
   Ico_PutContextualIconToCopy (Brw_ActCopy[Gbl.FileBrowser.Type],
				Brw_PutImplicitParamsFileBrowser);

   HTM_TD_End ();
  }

/*****************************************************************************/
/************* Write link and icon to paste a file or a folder ***************/
/*****************************************************************************/

static void Brw_PutIconPaste (unsigned Level)
  {
   extern const char *Txt_Copy_not_allowed;

   HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);

   if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER)	// Can't paste in a file or link
     {
      /* Icon to paste */
      if (Brw_CheckIfCanPasteIn (Level))
	{
	 /***** Form to paste the content of the clipboard *****/
	 Ico_PutContextualIconToPaste (Brw_ActPaste[Gbl.FileBrowser.Type],
				       Brw_PutImplicitParamsFileBrowser);
	}
      else
	 /* Icon to paste inactive */
	 Ico_PutIconOff ("paste.svg",Txt_Copy_not_allowed);
     }

   HTM_TD_End ();
  }

/*****************************************************************************/
/*************** Indent and write icon to expand/contract folder *************/
/*****************************************************************************/

static void Brw_IndentAndWriteIconExpandContract (unsigned Level,
                                                  const char *FileBrowserId,const char *RowId,
                                                  Brw_IconTree_t IconThisRow)
  {
   HTM_TD_Begin ("class=\"LM\"");
   HTM_TABLE_Begin (NULL);
   HTM_TR_Begin (NULL);
   Brw_IndentDependingOnLevel (Level);

   /***** Icon to expand/contract *****/
   HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);
   switch (IconThisRow)
     {
      case Brw_ICON_TREE_NOTHING:
	 Ico_PutIcon ("tr16x16.gif","","ICO20x20");
	 break;
      case Brw_ICON_TREE_EXPAND:
	 /***** Visible icon to expand folder *****/
         Brw_PutIconToExpandFolder (FileBrowserId,RowId,
                                    false);	// Visible

	 /***** Hidden icon to contract folder *****/
         Brw_PutIconToContractFolder (FileBrowserId,RowId,
                                      true);	// Hidden
	 break;
      case Brw_ICON_TREE_CONTRACT:
	 /***** Hidden icon to expand folder *****/
         Brw_PutIconToExpandFolder (FileBrowserId,RowId,
                                    true);	// Hidden

	 /***** Visible icon to contract folder *****/
         Brw_PutIconToContractFolder (FileBrowserId,RowId,
                                      false);	// Visible
	 break;
     }
   HTM_TD_End ();

   HTM_TR_End ();
   HTM_TABLE_End ();
   HTM_TD_End ();
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
     {
      HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);
      Ico_PutIcon ("tr16x16.gif","","ICO20x20");
      HTM_TD_End ();
     }
  }

/*****************************************************************************/
/************************ Put icon to expand a folder ************************/
/*****************************************************************************/

static void Brw_PutIconToExpandFolder (const char *FileBrowserId,const char *RowId,
                                       bool Hidden)
  {
   extern const char *Txt_Expand;
   char JavaScriptFuncToExpandFolder[256 + Brw_MAX_ROW_ID];

   /***** Start container *****/
   if (Hidden)
      HTM_DIV_Begin ("id=\"expand_%s_%s\" style=\"display:none;\"",FileBrowserId,RowId);
   else
      HTM_DIV_Begin ("id=\"expand_%s_%s\"",FileBrowserId,RowId);

   /***** Form and icon *****/
   snprintf (JavaScriptFuncToExpandFolder,sizeof (JavaScriptFuncToExpandFolder),
	     "ExpandFolder('%s_%s')",
	     FileBrowserId,RowId);
   Frm_StartFormAnchorOnSubmit (Brw_ActExpandFolder[Gbl.FileBrowser.Type],
				FileBrowserId,
				JavaScriptFuncToExpandFolder);	// JavaScript function to unhide rows
   Brw_PutImplicitParamsFileBrowser ();
   Ico_PutIconLink ("caret-right.svg",Txt_Expand);
   Frm_EndForm ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*********************** Put icon to contract a folder ***********************/
/*****************************************************************************/

static void Brw_PutIconToContractFolder (const char *FileBrowserId,const char *RowId,
                                         bool Hidden)
  {
   extern const char *Txt_Contract;
   char JavaScriptFuncToContractFolder[256 + Brw_MAX_ROW_ID];

   /***** Start container *****/
   if (Hidden)
      HTM_DIV_Begin ("id=\"contract_%s_%s\" style=\"display:none;\"",FileBrowserId,RowId);
   else
      HTM_DIV_Begin ("id=\"contract_%s_%s\"",FileBrowserId,RowId);

   /***** Form and icon *****/
   snprintf (JavaScriptFuncToContractFolder,sizeof (JavaScriptFuncToContractFolder),
	     "ContractFolder('%s_%s')",
	     FileBrowserId,RowId);
   Frm_StartFormAnchorOnSubmit (Brw_ActContractFolder[Gbl.FileBrowser.Type],
				FileBrowserId,
				JavaScriptFuncToContractFolder);	// JavaScript function to hide rows
   Brw_PutImplicitParamsFileBrowser ();
   Ico_PutIconLink ("caret-down.svg",Txt_Contract);
   Frm_EndForm ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/****************** Put link and icon to show file or folder *****************/
/*****************************************************************************/

static void Brw_PutIconShow (const char *Anchor)
  {
   HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);
   Ico_PutContextualIconToUnhide (Brw_ActShow[Gbl.FileBrowser.Type],Anchor,
	                          Brw_PutImplicitParamsFileBrowser);
   HTM_TD_End ();
  }

/*****************************************************************************/
/****************** Put link and icon to hide file or folder *****************/
/*****************************************************************************/

static void Brw_PutIconHide (const char *Anchor)
  {
   HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);
   Ico_PutContextualIconToHide (Brw_ActHide[Gbl.FileBrowser.Type],Anchor,
	                        Brw_PutImplicitParamsFileBrowser);
   HTM_TD_End ();
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

static void Brw_PutIconFolder (unsigned Level,
                               const char *FileBrowserId,const char *RowId,
                               Brw_IconTree_t IconSubtree)
  {
   bool ICanCreate;

   /***** Start cell *****/
   HTM_TD_Begin ("class=\"LM\" style=\"width:%upx;\"",Level * 20);

   /***** Put icon to create a new file or folder *****/
   if ((ICanCreate = Brw_CheckIfICanCreateIntoFolder (Level)))	// I can create a new file or folder
     {
      if (IconSubtree == Brw_ICON_TREE_EXPAND)
	{
	 /***** Visible icon with folder closed *****/
	 Brw_PutIconFolderWithPlus (FileBrowserId,RowId,
				    false,	// Closed
				    false);	// Visible

	 /***** Hidden icon with folder open *****/
	 Brw_PutIconFolderWithPlus (FileBrowserId,RowId,
				    true,	// Open
				    true);	// Hidden
	}
      else
	{
	 /***** Hidden icon with folder closed *****/
	 Brw_PutIconFolderWithPlus (FileBrowserId,RowId,
				    false,	// Closed
				    true);	// Hidden

	 /***** Visible icon with folder open *****/
	 Brw_PutIconFolderWithPlus (FileBrowserId,RowId,
				    true,	// Open
				    false);	// Visible
	}
     }
   else	// I can't create a new file or folder
     {
      if (IconSubtree == Brw_ICON_TREE_EXPAND)
	{
	 /***** Visible icon with folder closed *****/
	 Brw_PutIconFolderWithoutPlus (FileBrowserId,RowId,
			               false,	// Closed
			               false);	// Visible

	 /***** Hidden icon with folder open *****/
	 Brw_PutIconFolderWithoutPlus (FileBrowserId,RowId,
			               true,	// Open
			               true);	// Hidden
	}
      else
	{
	 /***** Hidden icon with folder closed *****/
	 Brw_PutIconFolderWithoutPlus (FileBrowserId,RowId,
			               false,	// Closed
			               true);	// Hidden

	 /***** Visible icon with folder open *****/
	 Brw_PutIconFolderWithoutPlus (FileBrowserId,RowId,
			               true,	// Open
			               false);	// Visible
	}
     }

   /***** End cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/******** Put icon to expand a folder without form to create inside **********/
/*****************************************************************************/

static void Brw_PutIconFolderWithoutPlus (const char *FileBrowserId,const char *RowId,
			                  bool Open,bool Hidden)
  {
   extern const char *Txt_Folder;

   /***** Start container *****/
   if (Hidden)
      HTM_DIV_Begin ("id=\"folder_%s_%s_%s\" style=\"display:none;\"",
		     Open ? "open" :
			    "closed",
		     FileBrowserId,RowId);
   else
      HTM_DIV_Begin ("id=\"folder_%s_%s_%s\"",
		     Open ? "open" :
			    "closed",
		     FileBrowserId,RowId);

   /***** Icon *****/
   Ico_PutIcon (Open ? "folder-open-yellow.png" :
	               "folder-yellow.png",
		Txt_Folder,"CONTEXT_OPT CONTEXT_ICO_16x16");

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Put icon to expand a folder with form to create inside ***********/
/*****************************************************************************/

static void Brw_PutIconFolderWithPlus (const char *FileBrowserId,const char *RowId,
				       bool Open,bool Hidden)
  {
   /***** Start container *****/
   if (Hidden)
      HTM_DIV_Begin ("id=\"folder_%s_%s_%s\" style=\"display:none;\"",
		     Open ? "open" :
			    "closed",
		     FileBrowserId,RowId);
   else
      HTM_DIV_Begin ("id=\"folder_%s_%s_%s\"",
		     Open ? "open" :
			    "closed",
		     FileBrowserId,RowId);

   /***** Form and icon *****/
   Ico_PutContextualIconToCreateInFolder (Brw_ActFormCreate[Gbl.FileBrowser.Type],
					  Brw_PutImplicitParamsFileBrowser,
					  Open);

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**************** Write icon to indicate that the file is new ****************/
/*****************************************************************************/

static void Brw_PutIconNewFileOrFolder (void)
  {
   extern const char *Txt_New_FILE_OR_FOLDER;

   /***** Icon that indicates new file *****/
   HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);
   Ico_PutIcon ("star16x16.gif",Txt_New_FILE_OR_FOLDER,"ICO20x20");
   HTM_TD_End ();
  }

/*****************************************************************************/
/***************************** Put icon of a file ****************************/
/*****************************************************************************/
// FileType can be Brw_IS_FILE or Brw_IS_LINK

static void Brw_PutIconFileWithLinkToViewMetadata (unsigned Size,
                                                   struct FileMetadata *FileMetadata)
  {
   extern const char *Txt_View_data;

   /***** Begin form *****/
   Frm_StartForm (Brw_ActReqDatFile[Gbl.FileBrowser.Type]);
   Brw_PutParamsFileBrowser (NULL,		// Not used
			     NULL,		// Not used
			     Brw_IS_UNKNOWN,	// Not used
                             FileMetadata->FilCod);

   /***** Name and link of the file or folder *****/
   Frm_LinkFormSubmit (Txt_View_data,Gbl.FileBrowser.TxtStyle,NULL);

   /***** Icon depending on the file extension *****/
   Brw_PutIconFile (Size,FileMetadata->FilFolLnk.Type,FileMetadata->FilFolLnk.Name);

   /***** End link and form *****/
   Frm_LinkFormEnd ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/***************************** Put icon of a file ****************************/
/*****************************************************************************/

static void Brw_PutIconFile (unsigned Size,Brw_FileType_t FileType,const char *FileName)
  {
   extern const unsigned Ext_NUM_FILE_EXT_ALLOWED;
   extern const char *Ext_FileExtensionsAllowed[];
   extern const char *Txt_Link;
   extern const char *Txt_X_file;
   char *URL;
   char *Icon;
   char *Title;
   unsigned DocType;
   bool NotFound;

   /***** Icon depending on the file extension *****/
   if (FileType == Brw_IS_LINK)
      Ico_PutIcon ("link.svg",Txt_Link,(Size == 16) ? "CONTEXT_ICO_16x16" :
		                                      "ICO40x40");
   else	// FileType == Brw_IS_FILE
     {
      if (asprintf (&URL,"%s%ux%u",
		    CfG_URL_ICON_FILEXT_PUBLIC,
	            Size,Size) < 0)
	 Lay_NotEnoughMemoryExit ();
      for (DocType = 0, NotFound = true;
	   DocType < Ext_NUM_FILE_EXT_ALLOWED && NotFound;
	   DocType++)
	 if (Str_FileIs (FileName,Ext_FileExtensionsAllowed[DocType]))
	   {
	    if (asprintf (&Icon,"%s%ux%u.gif",
			  Ext_FileExtensionsAllowed[DocType],
		          Size,Size) < 0)
	       Lay_NotEnoughMemoryExit ();
	    if (asprintf (&Title,Txt_X_file,
			  Ext_FileExtensionsAllowed[DocType]) < 0)
	       Lay_NotEnoughMemoryExit ();
	    NotFound = false;
	   }
      if (NotFound)
	{
	 if (asprintf (&Icon,"xxx%ux%u.gif",Size,Size) < 0)
	    Lay_NotEnoughMemoryExit ();
	 if (asprintf (&Title,"%s","") < 0)
	    Lay_NotEnoughMemoryExit ();
	}
      HTM_IMG (URL,Icon,Title,
	       "class=\"CONTEXT_OPT ICO_HIGHLIGHT %s\"",
	       (Size == 16) ? "CONTEXT_ICO_16x16" :
			      "ICO40x40");
      free (Title);
      free (Icon);
      free (URL);
     }
  }

/*****************************************************************************/
/***************** Put button to create ZIP file of a folder *****************/
/*****************************************************************************/

static void Brw_PutButtonToDownloadZIPOfAFolder (void)
  {
   /***** Form to zip and download folder *****/
   Ico_PutContextualIconToZIP (Brw_ActZIPFolder[Gbl.FileBrowser.Type],
			       Brw_PutImplicitParamsFileBrowser);
  }

/*****************************************************************************/
/********** Write central part with the name of a file or folder *************/
/*****************************************************************************/

static void Brw_WriteFileName (unsigned Level,bool IsPublic)
  {
   extern const char *Txt_Check_marks_in_the_file;
   extern const char *Txt_Download;
   extern const char *Txt_Public_open_educational_resource_OER_for_everyone;
   char FileNameToShow[NAME_MAX + 1];

   /***** Get the name of the file to show *****/
   Brw_GetFileNameToShowDependingOnLevel (Gbl.FileBrowser.Type,
                                          Level,
                                          Gbl.FileBrowser.FilFolLnk.Type,
					  Gbl.FileBrowser.FilFolLnk.Name,
                                          FileNameToShow);

   /***** Name and link of the folder, file or link *****/
   if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER)
     {
      /***** Start cell *****/
      if (Gbl.FileBrowser.Clipboard.IsThisFile)
	 HTM_TD_Begin ("class=\"%s LM LIGHT_GREEN\" style=\"width:99%%;\"",
		       Gbl.FileBrowser.TxtStyle);
      else
	 HTM_TD_Begin ("class=\"%s LM\" style=\"width:99%%;\"",
		       Gbl.FileBrowser.TxtStyle);

      HTM_DIV_Begin ("class=\"FILENAME\"");

      /***** Form to rename folder *****/
      if (Gbl.FileBrowser.ICanEditFileOrFolder)	// Can I rename this folder?
	{
         Frm_StartForm (Brw_ActRenameFolder[Gbl.FileBrowser.Type]);
         Brw_PutImplicitParamsFileBrowser ();
	}

      /***** Write name of the folder *****/
      fprintf (Gbl.F.Out,"&nbsp;");
      if (Gbl.FileBrowser.ICanEditFileOrFolder)	// Can I rename this folder?
	{
	 HTM_INPUT_TEXT ("NewFolderName",Brw_MAX_CHARS_FOLDER,Gbl.FileBrowser.FilFolLnk.Name,true,
			 "class=\"%s %s\"",
			 Gbl.FileBrowser.InputStyle,
			 Gbl.FileBrowser.Clipboard.IsThisFile ? "LIGHT_GREEN" :
							        Gbl.ColorRows[Gbl.RowEvenOdd]);
         Frm_EndForm ();
        }
      else
        {
         if ((Level == 1) &&
             (Gbl.FileBrowser.Type == Brw_ADMI_ASG_USR ||
              Gbl.FileBrowser.Type == Brw_ADMI_ASG_CRS))
            fprintf (Gbl.F.Out,"<span title=\"%s\">",
                     Gbl.FileBrowser.Asg.Title);
         fprintf (Gbl.F.Out,"<strong>%s</strong>&nbsp;",FileNameToShow);
         if ((Level == 1) &&
             (Gbl.FileBrowser.Type == Brw_ADMI_ASG_USR ||
              Gbl.FileBrowser.Type == Brw_ADMI_ASG_CRS))
            fprintf (Gbl.F.Out,"</span>");
        }

      /***** End cell *****/
      HTM_DIV_End ();

      HTM_TD_End ();
     }
   else	// File or link
     {
      if (Gbl.FileBrowser.Clipboard.IsThisFile)
	 HTM_TD_Begin ("class=\"%s LM LIGHT_GREEN\" style=\"width:99%%;\"",
		       Gbl.FileBrowser.TxtStyle);
      else
	 HTM_TD_Begin ("class=\"%s LM\" style=\"width:99%%;\"",
		       Gbl.FileBrowser.TxtStyle);

      fprintf (Gbl.F.Out,"&nbsp;");
      HTM_DIV_Begin ("class=\"FILENAME\"");

      Frm_StartForm (Brw_ActDowFile[Gbl.FileBrowser.Type]);
      Brw_PutImplicitParamsFileBrowser ();

      /* Link to the form and to the file */
      Frm_LinkFormSubmit ((Gbl.FileBrowser.Type == Brw_SHOW_MRK_CRS ||
	                   Gbl.FileBrowser.Type == Brw_SHOW_MRK_GRP) ? Txt_Check_marks_in_the_file :
	                                                               Txt_Download,
			  Gbl.FileBrowser.TxtStyle,NULL);
      fprintf (Gbl.F.Out,"%s",FileNameToShow);
      Frm_LinkFormEnd ();
      Frm_EndForm ();

      /* Put icon to make public/private file */
      if (IsPublic)
         Ico_PutIconOff ("unlock.svg",
                         Txt_Public_open_educational_resource_OER_for_everyone);

      HTM_DIV_End ();
      HTM_TD_End ();
     }
  }

/*****************************************************************************/
/*********************** Which filename must be shown? ***********************/
/*****************************************************************************/

static void Brw_GetFileNameToShowDependingOnLevel (Brw_FileBrowser_t FileBrowser,
                                                   unsigned Level,
                                                   Brw_FileType_t FileType,
                                                   const char *FileName,
                                                   char *FileNameToShow)
  {
   extern const char *Txt_ROOT_FOLDER_EXTERNAL_NAMES[Brw_NUM_TYPES_FILE_BROWSER];

   Brw_GetFileNameToShow (FileType,
                          Level ? FileName :
				  Txt_ROOT_FOLDER_EXTERNAL_NAMES[FileBrowser],
			  FileNameToShow);
  }

static void Brw_GetFileNameToShow (Brw_FileType_t FileType,
                                   const char FileName[NAME_MAX + 1],
                                   char FileNameToShow[NAME_MAX + 1])
  {
   /***** Copy file name *****/
   Str_Copy (FileNameToShow,FileName,
             NAME_MAX);

   /***** Remove .url if link *****/
   if (FileType == Brw_IS_LINK)	// It's a link (URL inside a .url file)
      FileNameToShow[strlen (FileNameToShow) - 4] = '\0';	// Remove .url
  }

/*****************************************************************************/
/****** Create a link in public temporary directory to a private file ********/
/*****************************************************************************/

void Brw_CreateTmpPublicLinkToPrivateFile (const char *FullPathIncludingFile,
                                           const char *FileName)
  {
   char Link[PATH_MAX + 1];

   /***** Create, into temporary public directory, a symbolic link to file *****/
   snprintf (Link,sizeof (Link),
	     "%s/%s/%s/%s",
             Cfg_PATH_FILE_BROWSER_TMP_PUBLIC,
             Gbl.FileBrowser.TmpPubDir.L,
             Gbl.FileBrowser.TmpPubDir.R,
	     FileName);
   if (symlink (FullPathIncludingFile,Link) != 0)
      Lay_ShowErrorAndExit ("Can not create temporary link.");
  }

/*****************************************************************************/
/************ Write start and end dates of a folder of assignment ************/
/*****************************************************************************/

static void Brw_WriteDatesAssignment (void)
  {
   extern const char *Txt_unknown_assignment;
   static unsigned UniqueId = 0;
   char *Id;

   HTM_TD_Begin ("colspan=\"2\" class=\"%s RM COLOR%u\"",
		 Gbl.FileBrowser.Asg.Open ? "ASG_LST_DATE_GREEN" :
					    "ASG_LST_DATE_RED",
		 Gbl.RowEvenOdd);

   if (Gbl.FileBrowser.Asg.AsgCod > 0)
     {
      UniqueId++;

      /***** Write start date *****/
      if (asprintf (&Id,"asg_start_date_%u",UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      fprintf (Gbl.F.Out,"<span id=\"%s\">",Id);
      Dat_WriteLocalDateHMSFromUTC (Id,Gbl.FileBrowser.Asg.TimeUTC[Dat_START_TIME],
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				    true,true,false,0x7);
      fprintf (Gbl.F.Out,"</span>");
      free (Id);

      /***** Arrow *****/
      fprintf (Gbl.F.Out,"&rarr;");

      /***** Write end date *****/
      if (asprintf (&Id,"asg_end_date_%u",UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      fprintf (Gbl.F.Out,"<span id=\"%s\">",Id);
      Dat_WriteLocalDateHMSFromUTC (Id,Gbl.FileBrowser.Asg.TimeUTC[Dat_END_TIME],
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				    true,false,false,0x7);
      fprintf (Gbl.F.Out,"</span>");
      free (Id);
     }
   else
      fprintf (Gbl.F.Out,"&nbsp;(%s)",Txt_unknown_assignment);
   HTM_TD_End ();
  }

/*****************************************************************************/
/****************** Write size and date of a file or folder ******************/
/*****************************************************************************/

static void Brw_WriteFileSizeAndDate (struct FileMetadata *FileMetadata)
  {
   static unsigned UniqueId = 0;
   char *Id;
   char FileSizeStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];

   /***** Write the file size *****/
   if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FILE)
      Fil_WriteFileSizeBrief ((double) FileMetadata->Size,FileSizeStr);
   else
      FileSizeStr[0] = '\0';
   HTM_TD_Begin ("class=\"%s RM COLOR%u\"",
                 Gbl.FileBrowser.TxtStyle,Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"&nbsp;%s",FileSizeStr);
   HTM_TD_End ();

   /***** Write the date *****/
   HTM_TD_Begin ("class=\"%s RM COLOR%u\"",
                 Gbl.FileBrowser.TxtStyle,Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"&nbsp;");
   if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FILE ||
       Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_LINK)
     {
      UniqueId++;
      if (asprintf (&Id,"filedate%u",UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      fprintf (Gbl.F.Out,"<span id=\"%s\"></span>",Id);
      Dat_WriteLocalDateHMSFromUTC (Id,FileMetadata->Time,
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				    true,true,false,0x6);
      free (Id);
     }
   HTM_TD_End ();
  }

/*****************************************************************************/
/************** Write the user who published the file or folder **************/
/*****************************************************************************/

static void Brw_WriteFileOrFolderPublisher (unsigned Level,unsigned long UsrCod)
  {
   extern const char *Txt_Unknown_or_without_photo;
   bool ShowUsr = false;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   struct UsrData UsrDat;

   if (Level && UsrCod > 0)
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Get data of file/folder publisher *****/
      UsrDat.UsrCod = UsrCod;
      ShowUsr = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);	// Get user's data from database
     }

   HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);
   if (ShowUsr)
     {
      /***** Show photo *****/
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                                            NULL,
                        "PHOTO15x20B",Pho_ZOOM,false);
     }
   else
      Ico_PutIcon ("usr_bl.jpg",Txt_Unknown_or_without_photo,"PHOTO15x20B");

   HTM_TD_End ();

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
   char FileNameToShow[NAME_MAX + 1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Button of confirmation of removing *****/
   if (Brw_CheckIfICanEditFileOrFolder (Gbl.FileBrowser.Level))	// Can I remove this file?
     {
      /***** Show question and button to remove file/link *****/
      Brw_GetFileNameToShowDependingOnLevel (Gbl.FileBrowser.FilFolLnk.Type,
                                             Gbl.FileBrowser.Level,
                                             Gbl.FileBrowser.FilFolLnk.Type,
                                             Gbl.FileBrowser.FilFolLnk.Name,
                                             FileNameToShow);
      Ale_ShowAlertAndButton (Brw_ActRemoveFile[Gbl.FileBrowser.Type],NULL,NULL,
			      Brw_PutImplicitParamsFileBrowser,
                              Btn_REMOVE_BUTTON,
                              Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FILE ? Txt_Remove_file :
        	                                                        Txt_Remove_link,
			      Ale_QUESTION,Txt_Do_you_really_want_to_remove_FILE_OR_LINK_X,
                              FileNameToShow);
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
   char Path[PATH_MAX + 1 + PATH_MAX + 1];
   struct stat FileStatus;
   char FileNameToShow[NAME_MAX + 1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   if (Brw_CheckIfICanEditFileOrFolder (Gbl.FileBrowser.Level))	// Can I remove this file?
     {
      snprintf (Path,sizeof (Path),
	        "%s/%s",
	        Gbl.FileBrowser.Priv.PathAboveRootFolder,
	        Gbl.FileBrowser.FilFolLnk.Full);

      /***** Check if is a file/link or a folder *****/
      if (lstat (Path,&FileStatus))	// On success ==> 0 is returned
	 Lay_ShowErrorAndExit ("Can not get information about a file or folder.");
      else if (S_ISREG (FileStatus.st_mode))		// It's a file or a link
        {
	 /* Name of the file/link to be shown */
	 Brw_GetFileNameToShow (Str_FileIs (Gbl.FileBrowser.FilFolLnk.Name,"url") ? Brw_IS_LINK :
										    Brw_IS_FILE,
				Gbl.FileBrowser.FilFolLnk.Name,FileNameToShow);

	 /* Remove file/link from disk and database */
	 Brw_RemoveFileFromDiskAndDB (Path,
	                              Gbl.FileBrowser.FilFolLnk.Full);

	 /* Remove affected clipboards */
	 Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
				       Gbl.Usrs.Me.UsrDat.UsrCod,
				       Gbl.Usrs.Other.UsrDat.UsrCod);

	 /* Message of confirmation of removing */
         Ale_ShowAlert (Ale_SUCCESS,Txt_FILE_X_removed,
		        FileNameToShow);
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
   char Path[PATH_MAX + 1 + PATH_MAX + 1];
   struct stat FileStatus;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   if (Brw_CheckIfICanEditFileOrFolder (Gbl.FileBrowser.Level))	// Can I remove this folder?
     {
      snprintf (Path,sizeof (Path),
	        "%s/%s",
	        Gbl.FileBrowser.Priv.PathAboveRootFolder,
	        Gbl.FileBrowser.FilFolLnk.Full);

      /***** Check if it's a file or a folder *****/
      if (lstat (Path,&FileStatus))	// On success ==> 0 is returned
	 Lay_ShowErrorAndExit ("Can not get information about a file or folder.");
      else if (S_ISDIR (FileStatus.st_mode))		// It's a directory
	 if (Brw_RemoveFolderFromDiskAndDB (Path,
                                            Gbl.FileBrowser.FilFolLnk.Full))
           {
	    if (errno == ENOTEMPTY)	// The directory is not empty
	      {
	       Brw_AskConfirmRemoveFolderNotEmpty ();
	       // Gbl.Alert.Txt[0] = '\0';	// TODO: Remove this line
	      }
	    else	// The directory is empty
               Lay_ShowErrorAndExit ("Can not remove folder.");
           }
         else
           {
            /* Remove affected clipboards */
            Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
                                          Gbl.Usrs.Me.UsrDat.UsrCod,
                                          Gbl.Usrs.Other.UsrDat.UsrCod);

            /* Message of confirmation of successfull removing */
            Ale_ShowAlert (Ale_SUCCESS,Txt_Folder_X_removed,
                           Gbl.FileBrowser.FilFolLnk.Name);
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

   /***** Show question and button to remove not empty folder *****/
   Ale_ShowAlertAndButton (Brw_ActRemoveFolderNotEmpty[Gbl.FileBrowser.Type],NULL,NULL,
			   Brw_PutImplicitParamsFileBrowser,
			   Btn_REMOVE_BUTTON,Txt_Remove_folder,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_folder_X,
                           Gbl.FileBrowser.FilFolLnk.Name);
  }

/*****************************************************************************/
/********* Complete removing of a not empty folder in a file browser *********/
/*****************************************************************************/

void Brw_RemSubtreeInFileBrowser (void)
  {
   extern const char *Txt_Folder_X_and_all_its_contents_removed;
   char Path[PATH_MAX + 1 + PATH_MAX + 1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   if (Brw_CheckIfICanEditFileOrFolder (Gbl.FileBrowser.Level))	// Can I remove this subtree?
     {
      snprintf (Path,sizeof (Path),
	        "%s/%s",
	        Gbl.FileBrowser.Priv.PathAboveRootFolder,
	        Gbl.FileBrowser.FilFolLnk.Full);

      /***** Remove the whole tree *****/
      Fil_RemoveTree (Path);

      /* If a folder is removed,
         it is necessary to remove it from the database and all the files o folders under that folder */
      Brw_RemoveOneFileOrFolderFromDB (Gbl.FileBrowser.FilFolLnk.Full);
      Brw_RemoveChildrenOfFolderFromDB (Gbl.FileBrowser.FilFolLnk.Full);

      /* Remove affected clipboards */
      Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
				    Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Other.UsrDat.UsrCod);

      /* Remove affected expanded folders */
      Brw_RemoveAffectedExpandedFolders (Gbl.FileBrowser.FilFolLnk.Full);

      /***** Write message of confirmation *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_Folder_X_and_all_its_contents_removed,
                     Gbl.FileBrowser.FilFolLnk.Name);
     }

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/********************* Expand a folder in a file browser *********************/
/*****************************************************************************/
// A priori function. Changes database. No HTML output.

void Brw_ExpandFileTree (void)
  {
   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Add path to table of expanded folders *****/
   Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.FilFolLnk.Full);
  }

/*****************************************************************************/
/******************* Contract a folder in a file browser *********************/
/*****************************************************************************/
// A priori function. Changes database. No HTML output.

void Brw_ContractFileTree (void)
  {
   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Remove path where the user has clicked from table of expanded folders *****/
   Brw_RemThisFolderAndUpdOtherFoldersFromExpandedFolders (Gbl.FileBrowser.FilFolLnk.Full);
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
      Brw_UpdatePathInClipboard ();
   else
      Brw_AddPathToClipboards ();

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/********* Write a title with the content of the current clipboard ***********/
/*****************************************************************************/

static void Brw_WriteCurrentClipboard (void)
  {
   extern const char *Txt_Copy_source;
   extern const char *Txt_documents_management_area;
   extern const char *Txt_teachers_files_area;
   extern const char *Txt_shared_files_area;
   extern const char *Txt_assignments_area;
   extern const char *Txt_works_area;
   extern const char *Txt_project_documents;
   extern const char *Txt_project_assessment;
   extern const char *Txt_marks_management_area;
   extern const char *Txt_temporary_private_storage_area;
   extern const char *Txt_institution;
   extern const char *Txt_centre;
   extern const char *Txt_degree;
   extern const char *Txt_course;
   extern const char *Txt_group;
   extern const char *Txt_project;
   extern const char *Txt_user[Usr_NUM_SEXS];
   extern const char *Txt_file_folder;
   extern const char *Txt_file;
   extern const char *Txt_folder;
   extern const char *Txt_link;
   extern const char *Txt_all_files_inside_the_root_folder;
   struct Instit Ins;
   struct Centre Ctr;
   struct Degree Deg;
   struct Course Crs;
   struct GroupData GrpDat;
   struct Project Prj;
   struct UsrData UsrDat;
   char TxtClipboardZone[1024 +
			 Hie_MAX_BYTES_SHRT_NAME +
			 Grp_MAX_BYTES_GROUP_TYPE_NAME +
			 Grp_MAX_BYTES_GROUP_NAME +
			 Usr_MAX_BYTES_FULL_NAME +
			 Prj_MAX_BYTES_PROJECT_TITLE];
   char FileNameToShow[NAME_MAX + 1];
   const char *TxtFileType[Brw_NUM_FILE_TYPES] =
     {
      Txt_file_folder,	// Brw_IS_UNKNOWN
      Txt_file,		// Brw_IS_FILE
      Txt_folder,	// Brw_IS_FOLDER
      Txt_link,		// Brw_IS_LINK
     };

   switch (Gbl.FileBrowser.Clipboard.FileBrowser)
     {
      case Brw_ADMI_DOC_INS:
	 Ins.InsCod = Gbl.FileBrowser.Clipboard.Cod;
	 Ins_GetDataOfInstitutionByCod (&Ins,false);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_documents_management_area,
                   Txt_institution,Ins.ShrtName);
         break;
      case Brw_ADMI_SHR_INS:
	 Ins.InsCod = Gbl.FileBrowser.Clipboard.Cod;
	 Ins_GetDataOfInstitutionByCod (&Ins,false);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_shared_files_area,
                   Txt_institution,Ins.ShrtName);
         break;
      case Brw_ADMI_DOC_CTR:
	 Ctr.CtrCod = Gbl.FileBrowser.Clipboard.Cod;
	 Ctr_GetDataOfCentreByCod (&Ctr);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_documents_management_area,
                   Txt_centre,Ctr.ShrtName);
         break;
      case Brw_ADMI_SHR_CTR:
	 Ctr.CtrCod = Gbl.FileBrowser.Clipboard.Cod;
	 Ctr_GetDataOfCentreByCod (&Ctr);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_shared_files_area,
                   Txt_centre,Ctr.ShrtName);
         break;
      case Brw_ADMI_DOC_DEG:
	 Deg.DegCod = Gbl.FileBrowser.Clipboard.Cod;
	 Deg_GetDataOfDegreeByCod (&Deg);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_documents_management_area,
                   Txt_degree,Deg.ShrtName);
         break;
      case Brw_ADMI_SHR_DEG:
	 Deg.DegCod = Gbl.FileBrowser.Clipboard.Cod;
	 Deg_GetDataOfDegreeByCod (&Deg);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_shared_files_area,
                   Txt_degree,Deg.ShrtName);
         break;
      case Brw_ADMI_DOC_CRS:
	 Crs.CrsCod = Gbl.FileBrowser.Clipboard.Cod;
	 Crs_GetDataOfCourseByCod (&Crs);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_documents_management_area,
                   Txt_course,Crs.ShrtName);
         break;
      case Brw_ADMI_DOC_GRP:
         GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.Cod;
         Grp_GetDataOfGroupByCod (&GrpDat);
	 Crs.CrsCod = GrpDat.CrsCod;
	 Crs_GetDataOfCourseByCod (&Crs);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s %s</strong>",
                   Txt_documents_management_area,
                   Txt_course,Crs.ShrtName,
                   Txt_group,GrpDat.GrpTypName,GrpDat.GrpName);
         break;
      case Brw_ADMI_TCH_CRS:
	 Crs.CrsCod = Gbl.FileBrowser.Clipboard.Cod;
	 Crs_GetDataOfCourseByCod (&Crs);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_teachers_files_area,
                   Txt_course,Crs.ShrtName);
         break;
      case Brw_ADMI_TCH_GRP:
         GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.Cod;
         Grp_GetDataOfGroupByCod (&GrpDat);
	 Crs.CrsCod = GrpDat.CrsCod;
	 Crs_GetDataOfCourseByCod (&Crs);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s %s</strong>",
                   Txt_teachers_files_area,
                   Txt_course,Crs.ShrtName,
                   Txt_group,GrpDat.GrpTypName,GrpDat.GrpName);
         break;
      case Brw_ADMI_SHR_CRS:
	 Crs.CrsCod = Gbl.FileBrowser.Clipboard.Cod;
	 Crs_GetDataOfCourseByCod (&Crs);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_shared_files_area,
                   Txt_course,Crs.ShrtName);
         break;
      case Brw_ADMI_SHR_GRP:
         GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.Cod;
         Grp_GetDataOfGroupByCod (&GrpDat);
	 Crs.CrsCod = GrpDat.CrsCod;
	 Crs_GetDataOfCourseByCod (&Crs);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s %s</strong>",
                   Txt_shared_files_area,
                   Txt_course,Crs.ShrtName,
                   Txt_group,GrpDat.GrpTypName,GrpDat.GrpName);
         break;
      case Brw_ADMI_ASG_USR:
	 Crs.CrsCod = Gbl.FileBrowser.Clipboard.Cod;
	 Crs_GetDataOfCourseByCod (&Crs);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s</strong>",
                   Txt_assignments_area,
                   Txt_course,Crs.ShrtName,
                   Txt_user[Gbl.Usrs.Me.UsrDat.Sex],Gbl.Usrs.Me.UsrDat.FullName);
         break;
      case Brw_ADMI_WRK_USR:
	 Crs.CrsCod = Gbl.FileBrowser.Clipboard.Cod;
	 Crs_GetDataOfCourseByCod (&Crs);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s</strong>",
                   Txt_works_area,
                   Txt_course,Crs.ShrtName,
                   Txt_user[Gbl.Usrs.Me.UsrDat.Sex],Gbl.Usrs.Me.UsrDat.FullName);
         break;
      case Brw_ADMI_ASG_CRS:
	 Crs.CrsCod = Gbl.FileBrowser.Clipboard.Cod;
	 Crs_GetDataOfCourseByCod (&Crs);
         Usr_UsrDataConstructor (&UsrDat);
         UsrDat.UsrCod = Gbl.FileBrowser.Clipboard.WorksUsrCod;
         Usr_GetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s</strong>",
                   Txt_assignments_area,
                   Txt_course,Crs.ShrtName,
                   Txt_user[UsrDat.Sex],UsrDat.FullName);
         Usr_UsrDataDestructor (&UsrDat);
         break;
      case Brw_ADMI_WRK_CRS:
	 Crs.CrsCod = Gbl.FileBrowser.Clipboard.Cod;
	 Crs_GetDataOfCourseByCod (&Crs);
         Usr_UsrDataConstructor (&UsrDat);
         UsrDat.UsrCod = Gbl.FileBrowser.Clipboard.WorksUsrCod;
         Usr_GetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s</strong>",
                   Txt_works_area,
                   Txt_course,Crs.ShrtName,
                   Txt_user[UsrDat.Sex],UsrDat.FullName);
         Usr_UsrDataDestructor (&UsrDat);
         break;
      case Brw_ADMI_DOC_PRJ:
      case Brw_ADMI_ASS_PRJ:
	 Prj_AllocMemProject (&Prj);
         Prj.PrjCod = Gbl.FileBrowser.Clipboard.Cod;
         Prj_GetDataOfProjectByCod (&Prj);
	 Crs.CrsCod = Prj.CrsCod;
	 Crs_GetDataOfCourseByCod (&Crs);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s</strong>",
                   Gbl.FileBrowser.Clipboard.FileBrowser == Brw_ADMI_DOC_PRJ ? Txt_project_documents :
                                                                               Txt_project_assessment,
                   Txt_course,Crs.ShrtName,
                   Txt_project,Prj.Title);
         Prj_FreeMemProject (&Prj);
         break;
      case Brw_ADMI_MRK_CRS:
	 Crs.CrsCod = Gbl.FileBrowser.Clipboard.Cod;
	 Crs_GetDataOfCourseByCod (&Crs);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_marks_management_area,
                   Txt_course,Crs.ShrtName);
         break;
      case Brw_ADMI_MRK_GRP:
         GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.Cod;
         Grp_GetDataOfGroupByCod (&GrpDat);
	 Crs.CrsCod = GrpDat.CrsCod;
	 Crs_GetDataOfCourseByCod (&Crs);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s %s</strong>",
                   Txt_marks_management_area,
                   Txt_course,Crs.ShrtName,
                   Txt_group,GrpDat.GrpTypName,GrpDat.GrpName);
         break;
      case Brw_ADMI_BRF_USR:
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_temporary_private_storage_area,
                   Txt_user[Gbl.Usrs.Me.UsrDat.Sex],Gbl.Usrs.Me.UsrDat.FullName);
         break;
      default:
         break;
     }

   if (Gbl.FileBrowser.Clipboard.Level)		// Is the root folder?
     {
      // Not the root folder
      Brw_GetFileNameToShowDependingOnLevel (Gbl.FileBrowser.Clipboard.FileBrowser,
                                             Gbl.FileBrowser.Clipboard.Level,
                                             Gbl.FileBrowser.Clipboard.FilFolLnk.Type,
                                             Gbl.FileBrowser.Clipboard.FilFolLnk.Name,
                                             FileNameToShow);

      Ale_ShowAlert (Ale_CLIPBOARD,"%s: %s, %s <strong>%s</strong>.",
                     Txt_Copy_source,TxtClipboardZone,
                     TxtFileType[Gbl.FileBrowser.Clipboard.FilFolLnk.Type],
                     FileNameToShow);
     }
   else
      // The root folder
      Ale_ShowAlert (Ale_CLIPBOARD,"%s: %s, %s.",
                     Txt_Copy_source,TxtClipboardZone,
                     Txt_all_files_inside_the_root_folder);
  }

/*****************************************************************************/
/********************** Get data of my current clipboard *********************/
/*****************************************************************************/
// Returns true if something found
// Returns false and void data if nothing found

static bool Brw_GetMyClipboard (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;
   unsigned UnsignedNum;

   /***** Clear clipboard data *****/
   Gbl.FileBrowser.Clipboard.FileBrowser = Brw_UNKNOWN;
   Gbl.FileBrowser.Clipboard.Cod         = -1L;
   Gbl.FileBrowser.Clipboard.WorksUsrCod = -1L;
   Gbl.FileBrowser.Clipboard.FilFolLnk.Type    = Brw_IS_UNKNOWN;
   Gbl.FileBrowser.Clipboard.FilFolLnk.Full[0]     = '\0';
   Gbl.FileBrowser.Clipboard.FilFolLnk.Name[0] = '\0';
   Gbl.FileBrowser.Clipboard.Level       = 0;

   /***** Get my current clipboard from database *****/
   NumRows = (unsigned) DB_QuerySELECT (&mysql_res,"can not get source of copy"
						   " from clipboard",
					"SELECT FileBrowser,Cod,WorksUsrCod,"
					"FileType,Path"
					" FROM clipboard WHERE UsrCod=%ld",
					Gbl.Usrs.Me.UsrDat.UsrCod);

   if (NumRows == 1)
     {
      /***** Get clipboard data *****/
      row = mysql_fetch_row (mysql_res);

      /* Get file browser type (row[0]) */
      if (sscanf (row[0],"%u",&UnsignedNum) == 1)
        {
         Gbl.FileBrowser.Clipboard.FileBrowser = (Brw_FileBrowser_t) UnsignedNum;

         /* Get institution/centre/degree/course/group code (row[1]) */
         Gbl.FileBrowser.Clipboard.Cod = Str_ConvertStrCodToLongCod (row[1]);

         /* Get works user's code (row[2]) */
         Gbl.FileBrowser.Clipboard.WorksUsrCod = Str_ConvertStrCodToLongCod (row[2]);

         /* Get file type (row[3]) */
         Gbl.FileBrowser.Clipboard.FilFolLnk.Type = Brw_IS_UNKNOWN;	// default
         if (sscanf (row[3],"%u",&UnsignedNum) == 1)
            if (UnsignedNum < Brw_NUM_FILE_TYPES)
               Gbl.FileBrowser.Clipboard.FilFolLnk.Type = (Brw_FileType_t) UnsignedNum;

         /* Get file path (row[4]) */
         Str_Copy (Gbl.FileBrowser.Clipboard.FilFolLnk.Full,row[4],
                   PATH_MAX);
         Str_SplitFullPathIntoPathAndFileName (Gbl.FileBrowser.Clipboard.FilFolLnk.Full,
					       Gbl.FileBrowser.Clipboard.FilFolLnk.Path,
                                               Gbl.FileBrowser.Clipboard.FilFolLnk.Name);

         /* Set clipboard level
            (number of slashes in full path, including file or folder) */
         Gbl.FileBrowser.Clipboard.Level = Brw_NumLevelsInPath (Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (NumRows > 1)
      Lay_ShowErrorAndExit ("Error when getting source of copy.");

   return (bool) (NumRows == 1);
  }

/*****************************************************************************/
/********* Check if the clipboard is in the current file browser *************/
/*****************************************************************************/

static bool Brw_CheckIfClipboardIsInThisTree (void)
  {
   if (Gbl.FileBrowser.Clipboard.FileBrowser == Brw_FileBrowserForDB_clipboard[Gbl.FileBrowser.Type])
     {
      switch (Gbl.FileBrowser.Clipboard.FileBrowser)
        {
	 case Brw_ADMI_DOC_INS:
         case Brw_ADMI_SHR_INS:
            if (Gbl.FileBrowser.Clipboard.Cod == Gbl.Hierarchy.Ins.InsCod)
               return true;		// I am in the institution of the clipboard
            break;
	 case Brw_ADMI_DOC_CTR:
         case Brw_ADMI_SHR_CTR:
            if (Gbl.FileBrowser.Clipboard.Cod == Gbl.Hierarchy.Ctr.CtrCod)
               return true;		// I am in the centre of the clipboard
            break;
	 case Brw_ADMI_DOC_DEG:
         case Brw_ADMI_SHR_DEG:
            if (Gbl.FileBrowser.Clipboard.Cod == Gbl.Hierarchy.Deg.DegCod)
               return true;		// I am in the degree of the clipboard
            break;
         case Brw_ADMI_DOC_CRS:
         case Brw_ADMI_TCH_CRS:
         case Brw_ADMI_SHR_CRS:
         case Brw_ADMI_MRK_CRS:
         case Brw_ADMI_ASG_USR:
         case Brw_ADMI_WRK_USR:
            if (Gbl.FileBrowser.Clipboard.Cod == Gbl.Hierarchy.Crs.CrsCod)
               return true;		// I am in the course of the clipboard
            break;
	 case Brw_ADMI_ASG_CRS:
	 case Brw_ADMI_WRK_CRS:
            if (Gbl.FileBrowser.Clipboard.Cod == Gbl.Hierarchy.Crs.CrsCod &&
	        Gbl.FileBrowser.Clipboard.WorksUsrCod == Gbl.Usrs.Other.UsrDat.UsrCod)
               return true;		// I am in the course of the clipboard
					// I am in the student's works of the clipboard
	    break;
	 case Brw_ADMI_DOC_GRP:
	 case Brw_ADMI_TCH_GRP:
	 case Brw_ADMI_SHR_GRP:
	 case Brw_ADMI_MRK_GRP:
            if (Gbl.FileBrowser.Clipboard.Cod == Gbl.Crs.Grps.GrpCod)
               return true;		// I am in the group of the clipboard
            break;
	 case Brw_ADMI_DOC_PRJ:
	 case Brw_ADMI_ASS_PRJ:
            if (Gbl.FileBrowser.Clipboard.Cod == Gbl.Prjs.PrjCod)
               return true;		// I am in the project of the clipboard
	    break;
	 case Brw_ADMI_BRF_USR:
	    return true;
	 default:
	    break;
	}
     }
   return false;
  }

/*****************************************************************************/
/***************************** Add path to clipboards ************************/
/*****************************************************************************/

static void Brw_AddPathToClipboards (void)
  {
   long Cod = Brw_GetCodForClipboard ();
   long WorksUsrCod = Brw_GetWorksUsrCodForClipboard ();

   /***** Add path to clipboards *****/
   DB_QueryINSERT ("can not add source of copy to clipboard",
		   "INSERT INTO clipboard"
		   " (UsrCod,FileBrowser,Cod,WorksUsrCod,FileType,Path)"
		   " VALUES"
		   " (%ld,%u,%ld,%ld,%u,'%s')",
	           Gbl.Usrs.Me.UsrDat.UsrCod,(unsigned) Gbl.FileBrowser.Type,
	           Cod,WorksUsrCod,
	           (unsigned) Gbl.FileBrowser.FilFolLnk.Type,Gbl.FileBrowser.FilFolLnk.Full);
  }

/*****************************************************************************/
/************************** Update path in my clipboard **********************/
/*****************************************************************************/

static void Brw_UpdatePathInClipboard (void)
  {
   long Cod = Brw_GetCodForClipboard ();
   long WorksUsrCod = Brw_GetWorksUsrCodForClipboard ();

   /***** Update path in my clipboard *****/
   DB_QueryUPDATE ("can not update source of copy in clipboard",
		   "UPDATE clipboard SET FileBrowser=%u,"
		   "Cod=%ld,WorksUsrCod=%ld,"
		   "FileType=%u,Path='%s'"
		   " WHERE UsrCod=%ld",
	           (unsigned) Gbl.FileBrowser.Type,
	           Cod,WorksUsrCod,
	           (unsigned) Gbl.FileBrowser.FilFolLnk.Type,Gbl.FileBrowser.FilFolLnk.Full,
	           Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/**** Get code of institution, degree, course, group for expanded folders ****/
/*****************************************************************************/

static long Brw_GetCodForClipboard (void)
  {
   switch (Brw_FileBrowserForDB_clipboard[Gbl.FileBrowser.Type])
     {
      case Brw_ADMI_DOC_INS:
      case Brw_ADMI_SHR_INS:
	 return Gbl.Hierarchy.Ins.InsCod;
      case Brw_ADMI_DOC_CTR:
      case Brw_ADMI_SHR_CTR:
	 return Gbl.Hierarchy.Ctr.CtrCod;
      case Brw_ADMI_DOC_DEG:
      case Brw_ADMI_SHR_DEG:
	 return Gbl.Hierarchy.Deg.DegCod;
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_TCH_CRS:
      case Brw_ADMI_SHR_CRS:
      case Brw_ADMI_ASG_USR:
      case Brw_ADMI_ASG_CRS:
      case Brw_ADMI_WRK_USR:
      case Brw_ADMI_WRK_CRS:
      case Brw_ADMI_MRK_CRS:
	 return Gbl.Hierarchy.Crs.CrsCod;
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_GRP:
      case Brw_ADMI_SHR_GRP:
      case Brw_ADMI_MRK_GRP:
	 return Gbl.Crs.Grps.GrpCod;
      case Brw_ADMI_DOC_PRJ:
      case Brw_ADMI_ASS_PRJ:
	 return Gbl.Prjs.PrjCod;
      default:
         return -1L;
     }
  }

/*****************************************************************************/
/******** Get code of user in assignment / works for expanded folders ********/
/*****************************************************************************/

static long Brw_GetWorksUsrCodForClipboard (void)
  {
   switch (Brw_FileBrowserForDB_clipboard[Gbl.FileBrowser.Type])
     {
      case Brw_ADMI_ASG_CRS:
      case Brw_ADMI_WRK_CRS:
	 return Gbl.Usrs.Other.UsrDat.UsrCod;
      default:
         return -1L;
     }
  }

/*****************************************************************************/
/***** Insert folders until specified folder to table of expanded folders ****/
/***** and update click time of the other folders in the expl. tree       ****/
/*****************************************************************************/
// Important: parameter Path must end in a folder, not in a file

static void Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (const char Path[PATH_MAX + 1])
  {
   char *Ptr;
   char CopyOfPath[PATH_MAX + 1];
   /* Example:
      Path = root_folder/folder1/folder2/folder3
      1. Try to insert CopyOfPath = "root_folder/folder1/folder2/folder3"
      2. Try to insert CopyOfPath = "root_folder/folder1/folder2"
      3. Try to insert CopyOfPath = "root_folder/folder1"
      Only insert paths with '/', so "root_folder" is not inserted
   */

   /***** Make a copy to keep Path unchanged *****/
   Str_Copy (CopyOfPath,Path,
             PATH_MAX);

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

static void Brw_RemThisFolderAndUpdOtherFoldersFromExpandedFolders (const char Path[PATH_MAX + 1])
  {
   /***** Remove Path from expanded folders table *****/
   Brw_RemoveFolderFromExpandedFolders (Path);

   /***** Update paths of the current file browser in table of expanded folders *****/
   Brw_UpdateClickTimeOfThisFileBrowserInExpandedFolders ();
  }

/*****************************************************************************/
/************************* Insert path in expanded folders *******************/
/*****************************************************************************/

static void Brw_InsertFolderInExpandedFolders (const char Path[PATH_MAX + 1])
  {
   long Cod = Brw_GetCodForExpandedFolders ();
   long WorksUsrCod = Brw_GetWorksUsrCodForExpandedFolders ();

   /***** Update path time in table of expanded folders *****/
   // Path must be stored with final '/'
   DB_QueryINSERT ("can not expand the content of a folder",
		   "INSERT INTO expanded_folders"
		   " (UsrCod,FileBrowser,Cod,WorksUsrCod,Path,ClickTime)"
		   " VALUES"
		   " (%ld,%u,%ld,%ld,'%s/',NOW())",
	           Gbl.Usrs.Me.UsrDat.UsrCod,
	           (unsigned) Brw_FileBrowserForDB_expanded_folders[Gbl.FileBrowser.Type],
	           Cod,WorksUsrCod,
	           Path);
  }

/*****************************************************************************/
/******* Update paths of the current file browser in expanded folders ********/
/*****************************************************************************/

static void Brw_UpdateClickTimeOfThisFileBrowserInExpandedFolders (void)
  {
   long Cod = Brw_GetCodForExpandedFolders ();
   long WorksUsrCod = Brw_GetWorksUsrCodForExpandedFolders ();
   Brw_FileBrowser_t FileBrowserForExpandedFolders = Brw_FileBrowserForDB_expanded_folders[Gbl.FileBrowser.Type];

   /***** Update click time in table of expanded folders *****/
   if (Cod > 0)
     {
      if (WorksUsrCod > 0)
	 DB_QueryUPDATE ("can not update expanded folder",
			 "UPDATE expanded_folders SET ClickTime=NOW()"
			 " WHERE UsrCod=%ld AND FileBrowser=%u"
			 " AND Cod=%ld AND WorksUsrCod=%ld",
		         Gbl.Usrs.Me.UsrDat.UsrCod,
		         (unsigned) FileBrowserForExpandedFolders,
		         Cod,
		         WorksUsrCod);
      else
	 DB_QueryUPDATE ("can not update expanded folder",
			 "UPDATE expanded_folders SET ClickTime=NOW()"
			 " WHERE UsrCod=%ld AND FileBrowser=%u"
			 " AND Cod=%ld",
		         Gbl.Usrs.Me.UsrDat.UsrCod,
		         (unsigned) FileBrowserForExpandedFolders,
		         Cod);
     }
   else	// Briefcase
      DB_QueryUPDATE ("can not update expanded folder",
		      "UPDATE expanded_folders SET ClickTime=NOW()"
		      " WHERE UsrCod=%ld AND FileBrowser=%u",
	              Gbl.Usrs.Me.UsrDat.UsrCod,
	              (unsigned) FileBrowserForExpandedFolders);
  }

/*****************************************************************************/
/********************** Remove path from expanded folders ********************/
/*****************************************************************************/

static void Brw_RemoveFolderFromExpandedFolders (const char Path[PATH_MAX + 1])
  {
   long Cod = Brw_GetCodForExpandedFolders ();
   long WorksUsrCod = Brw_GetWorksUsrCodForExpandedFolders ();
   Brw_FileBrowser_t FileBrowserForExpandedFolders = Brw_FileBrowserForDB_expanded_folders[Gbl.FileBrowser.Type];

   /***** Remove expanded folders associated to a file browser *****/
   if (Cod > 0)
     {
      if (WorksUsrCod > 0)
         DB_QueryDELETE ("can not contract the content of a folder",
			 "DELETE FROM expanded_folders"
			 " WHERE UsrCod=%ld AND FileBrowser=%u"
			 " AND Cod=%ld AND WorksUsrCod=%ld AND Path='%s/'",
		         Gbl.Usrs.Me.UsrDat.UsrCod,(unsigned) FileBrowserForExpandedFolders,
		         Cod,WorksUsrCod,Path);
      else
	 DB_QueryDELETE ("can not contract the content of a folder",
		         "DELETE FROM expanded_folders"
			 " WHERE UsrCod=%ld AND FileBrowser=%u"
			 " AND Cod=%ld AND Path='%s/'",
		         Gbl.Usrs.Me.UsrDat.UsrCod,
		         (unsigned) FileBrowserForExpandedFolders,
		         Cod,Path);
     }
   else	// Briefcase
      DB_QueryDELETE ("can not contract the content of a folder",
		      "DELETE FROM expanded_folders"
		      " WHERE UsrCod=%ld AND FileBrowser=%u"
		      " AND Path='%s/'",
	              Gbl.Usrs.Me.UsrDat.UsrCod,(unsigned) FileBrowserForExpandedFolders,
	              Path);
  }

/*****************************************************************************/
/***** Remove expanded folders with paths from a course or from a user *******/
/*****************************************************************************/

static void Brw_RemoveAffectedExpandedFolders (const char Path[PATH_MAX + 1])
  {
   long Cod = Brw_GetCodForExpandedFolders ();
   long WorksUsrCod = Brw_GetWorksUsrCodForExpandedFolders ();
   Brw_FileBrowser_t FileBrowserForExpandedFolders = Brw_FileBrowserForDB_expanded_folders[Gbl.FileBrowser.Type];

   /***** Remove expanded folders associated to a file browser from a course or from a user *****/
   if (Cod > 0)
     {
      if (WorksUsrCod > 0)
         DB_QueryDELETE ("can not remove expanded folders",
			 "DELETE FROM expanded_folders"
			 " WHERE UsrCod=%ld AND FileBrowser=%u"
			 " AND Cod=%ld AND WorksUsrCod=%ld AND Path LIKE '%s/%%'",
		         Gbl.Usrs.Me.UsrDat.UsrCod,(unsigned) FileBrowserForExpandedFolders,
		         Cod,WorksUsrCod,Path);
      else
         DB_QueryDELETE ("can not remove expanded folders",
			 "DELETE FROM expanded_folders"
			 " WHERE UsrCod=%ld AND FileBrowser=%u"
			 " AND Cod=%ld AND Path LIKE '%s/%%'",
		         Gbl.Usrs.Me.UsrDat.UsrCod,
		         (unsigned) FileBrowserForExpandedFolders,
		         Cod,Path);
     }
   else	// Briefcase
      DB_QueryDELETE ("can not remove expanded folders",
		      "DELETE FROM expanded_folders"
		      " WHERE UsrCod=%ld AND FileBrowser=%u"
		      " AND Path LIKE '%s/%%'",
		      Gbl.Usrs.Me.UsrDat.UsrCod,
		      (unsigned) FileBrowserForExpandedFolders,
		      Path);
  }

/*****************************************************************************/
/***** Remove expanded folders with paths from a course or from a user *******/
/*****************************************************************************/

static void Brw_RenameAffectedExpandedFolders (Brw_FileBrowser_t FileBrowser,
                                               long MyUsrCod,long WorksUsrCod,
                                               const char *OldPath,const char *NewPath)
  {
   long Cod = Brw_GetCodForExpandedFolders ();
   Brw_FileBrowser_t FileBrowserForExpandedFolders = Brw_FileBrowserForDB_expanded_folders[FileBrowser];
   unsigned StartFinalSubpathNotChanged = strlen (OldPath) + 2;

   /***** Update expanded folders associated to a file browser from a course or from a user *****/
   if (Cod > 0)
     {
      if (MyUsrCod > 0)
	{
	 if (WorksUsrCod > 0)
	    DB_QueryUPDATE ("can not update expanded folders",
			    "UPDATE expanded_folders SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
			    " WHERE UsrCod=%ld AND FileBrowser=%u"
			    " AND Cod=%ld AND WorksUsrCod=%ld"
			    " AND Path LIKE '%s/%%'",
		            NewPath,StartFinalSubpathNotChanged,
		            MyUsrCod,(unsigned) FileBrowserForExpandedFolders,
		            Cod,WorksUsrCod,
		            OldPath);
	 else
	    DB_QueryUPDATE ("can not update expanded folders",
			    "UPDATE expanded_folders SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
			    " WHERE UsrCod=%ld AND FileBrowser=%u"
			    " AND Cod=%ld"
			    " AND Path LIKE '%s/%%'",
		            NewPath,StartFinalSubpathNotChanged,
		            MyUsrCod,(unsigned) FileBrowserForExpandedFolders,
		            Cod,
		            OldPath);
	}
      else	// MyUsrCod <= 0 means expanded folders for any user
	{
	 if (WorksUsrCod > 0)
	    DB_QueryUPDATE ("can not update expanded folders",
			    "UPDATE expanded_folders SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
			    " WHERE FileBrowser=%u AND Cod=%ld"
			    " AND WorksUsrCod=%ld"
			    " AND Path LIKE '%s/%%'",
		            NewPath,StartFinalSubpathNotChanged,
		            (unsigned) FileBrowserForExpandedFolders,Cod,
		            WorksUsrCod,
		            OldPath);
	 else
	    DB_QueryUPDATE ("can not update expanded folders",
			    "UPDATE expanded_folders SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
			    " WHERE FileBrowser=%u AND Cod=%ld"
			    " AND Path LIKE '%s/%%'",
			    NewPath,StartFinalSubpathNotChanged,
			    (unsigned) FileBrowserForExpandedFolders,Cod,
			    OldPath);
	}
     }
   else	// Briefcase
      DB_QueryUPDATE ("can not update expanded folders",
		      "UPDATE expanded_folders SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
		      " WHERE UsrCod=%ld AND FileBrowser=%u"
		      " AND Path LIKE '%s/%%'",
	              NewPath,StartFinalSubpathNotChanged,
	              MyUsrCod,
	              (unsigned) FileBrowserForExpandedFolders,
	              OldPath);
  }

/*****************************************************************************/
/************* Check if a folder from a file browser is expanded *************/
/*****************************************************************************/

static bool Brw_GetIfExpandedTree (const char Path[PATH_MAX + 1])
  {
   bool Expanded;
   long Cod = Brw_GetCodForExpandedFolders ();
   long WorksUsrCod = Brw_GetWorksUsrCodForExpandedFolders ();
   Brw_FileBrowser_t FileBrowserForExpandedFolders = Brw_FileBrowserForDB_expanded_folders[Gbl.FileBrowser.Type];

   /***** Get if a folder is expanded from database *****/
   if (Cod > 0)
     {
      if (WorksUsrCod > 0)
         Expanded = (DB_QueryCOUNT ("can not get check if a folder is expanded",
				    "SELECT COUNT(*) FROM expanded_folders"
				    " WHERE UsrCod=%ld AND FileBrowser=%u"
				    " AND Cod=%ld AND WorksUsrCod=%ld"
				    " AND Path='%s/'",
				    Gbl.Usrs.Me.UsrDat.UsrCod,
				    (unsigned) FileBrowserForExpandedFolders,
				    Cod,WorksUsrCod,
				    Path) != 0);
      else
         Expanded = (DB_QueryCOUNT ("can not get check if a folder is expanded",
				    "SELECT COUNT(*) FROM expanded_folders"
				    " WHERE UsrCod=%ld AND FileBrowser=%u"
				    " AND Cod=%ld"
				    " AND Path='%s/'",
				    Gbl.Usrs.Me.UsrDat.UsrCod,
				    (unsigned) FileBrowserForExpandedFolders,
				    Cod,
				    Path) != 0);
     }
   else	// Briefcase
      Expanded = (DB_QueryCOUNT ("can not get check if a folder is expanded",
				 "SELECT COUNT(*) FROM expanded_folders"
				 " WHERE UsrCod=%ld AND FileBrowser=%u"
				 " AND Path='%s/'",
				 Gbl.Usrs.Me.UsrDat.UsrCod,
				 (unsigned) FileBrowserForExpandedFolders,
				 Path) != 0);
   return Expanded;
  }

/*****************************************************************************/
/**** Get code of institution, degree, course, group for expanded folders ****/
/*****************************************************************************/

static long Brw_GetCodForExpandedFolders (void)
  {
   switch (Brw_FileBrowserForDB_expanded_folders[Gbl.FileBrowser.Type])
     {
      case Brw_ADMI_DOC_INS:
      case Brw_ADMI_SHR_INS:
	 return Gbl.Hierarchy.Ins.InsCod;
      case Brw_ADMI_DOC_CTR:
      case Brw_ADMI_SHR_CTR:
	 return Gbl.Hierarchy.Ctr.CtrCod;
      case Brw_ADMI_DOC_DEG:
      case Brw_ADMI_SHR_DEG:
	 return Gbl.Hierarchy.Deg.DegCod;
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_TCH_CRS:
      case Brw_ADMI_SHR_CRS:
      case Brw_ADMI_ASG_USR:
      case Brw_ADMI_ASG_CRS:
      case Brw_ADMI_WRK_USR:
      case Brw_ADMI_WRK_CRS:
      case Brw_ADMI_MRK_CRS:
	 return Gbl.Hierarchy.Crs.CrsCod;
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_GRP:
      case Brw_ADMI_SHR_GRP:
      case Brw_ADMI_MRK_GRP:
	 return Gbl.Crs.Grps.GrpCod;
      default:
         return -1L;
     }
  }

/*****************************************************************************/
/******** Get code of user in assignment / works for expanded folders ********/
/*****************************************************************************/

static long Brw_GetWorksUsrCodForExpandedFolders (void)
  {
   switch (Brw_FileBrowserForDB_expanded_folders[Gbl.FileBrowser.Type])
     {
      case Brw_ADMI_ASG_CRS:
      case Brw_ADMI_WRK_CRS:
	 return Gbl.Usrs.Other.UsrDat.UsrCod;
      default:
         return -1L;
     }
  }

/*****************************************************************************/
/************* Remove expired expanded folders (from all users) **************/
/*****************************************************************************/

void Brw_RemoveExpiredExpandedFolders (void)
  {
   /***** Remove all expired clipboards *****/
   DB_QueryDELETE ("can not remove old expanded folders",
		   "DELETE LOW_PRIORITY FROM expanded_folders"
		   " WHERE ClickTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Cfg_TIME_TO_DELETE_BROWSER_EXPANDED_FOLDERS);
  }

/*****************************************************************************/
/****************** Remove expired clipboards (from all users) ***************/
/*****************************************************************************/

static void Brw_RemoveExpiredClipboards (void)
  {
   /***** Remove all expired clipboards *****/
   DB_QueryDELETE ("can not remove old paths from clipboard",
		   "DELETE LOW_PRIORITY FROM clipboard"
		   " WHERE CopyTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Cfg_TIME_TO_DELETE_BROWSER_CLIPBOARD);
  }

/*****************************************************************************/
/********* Remove clipboards with paths from a course or from a user *********/
/*****************************************************************************/

static void Brw_RemoveAffectedClipboards (Brw_FileBrowser_t FileBrowser,
                                          long MyUsrCod,long WorksUsrCod)
  {
   /***** Remove clipboards associated to a file browser
          from a course or from a user *****/
   switch (FileBrowser)
     {
      case Brw_ADMI_DOC_INS:
      case Brw_ADMI_SHR_INS:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM clipboard"
			 " WHERE FileBrowser=%u AND Cod=%ld",
			 (unsigned) FileBrowser,
			 Gbl.Hierarchy.Ins.InsCod);
         break;
      case Brw_ADMI_DOC_CTR:
      case Brw_ADMI_SHR_CTR:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM clipboard"
			 " WHERE FileBrowser=%u AND Cod=%ld",
                         (unsigned) FileBrowser,
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Brw_ADMI_DOC_DEG:
      case Brw_ADMI_SHR_DEG:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM clipboard"
			 " WHERE FileBrowser=%u AND Cod=%ld",
                         (unsigned) FileBrowser,
                         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_TCH_CRS:
      case Brw_ADMI_SHR_CRS:
      case Brw_ADMI_MRK_CRS:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM clipboard"
			 " WHERE FileBrowser=%u AND Cod=%ld",
                         (unsigned) FileBrowser,
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_GRP:
      case Brw_ADMI_SHR_GRP:
      case Brw_ADMI_MRK_GRP:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM clipboard"
			 " WHERE FileBrowser=%u AND Cod=%ld",
                         (unsigned) FileBrowser,
                         Gbl.Crs.Grps.GrpCod);
         break;
      case Brw_ADMI_ASG_USR:
      case Brw_ADMI_WRK_USR:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM clipboard"
			 " WHERE UsrCod=%ld AND FileBrowser=%u AND Cod=%ld",
                         MyUsrCod,(unsigned) FileBrowser,
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      case Brw_ADMI_ASG_CRS:
      case Brw_ADMI_WRK_CRS:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM clipboard"
			 " WHERE FileBrowser=%u AND Cod=%ld AND WorksUsrCod=%ld",
                         (unsigned) FileBrowser,
                         Gbl.Hierarchy.Crs.CrsCod,WorksUsrCod);
         break;
      case Brw_ADMI_DOC_PRJ:
      case Brw_ADMI_ASS_PRJ:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM clipboard"
			 " WHERE FileBrowser=%u AND Cod=%ld",
                         (unsigned) FileBrowser,
                         Gbl.Prjs.PrjCod);
	 break;
      case Brw_ADMI_BRF_USR:
         DB_QueryDELETE ("can not remove source of copy",
			 "DELETE FROM clipboard"
			 " WHERE UsrCod=%ld AND FileBrowser=%u",
                         MyUsrCod,(unsigned) FileBrowser);
         break;
      default:
         break;
     }
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
      switch (Gbl.FileBrowser.Clipboard.FileBrowser)
        {
         case Brw_ADMI_DOC_GRP:
         case Brw_ADMI_TCH_GRP:
         case Brw_ADMI_SHR_GRP:
         case Brw_ADMI_MRK_GRP:	// Clipboard in a group zone
	    GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.Cod;
	    Brw_GetSelectedGroupData (&GrpDat,true);
	    break;
         default:
            break;
        }

      /***** Copy files recursively *****/
      Brw_PasteClipboard ();

      /***** Remove the affected clipboards *****/
      Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
				    Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Other.UsrDat.UsrCod);
     }
   else
      /***** Write message ******/
      Ale_ShowAlert (Ale_WARNING,Txt_Nothing_has_been_pasted_because_the_clipboard_is_empty_);

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/****** Paste all the content of the clipboard in the current location *******/
/*****************************************************************************/
// Source:
//	Type of file browser:		Gbl.FileBrowser.Clipboard.FileBrowser
//	Possible institution:		Gbl.FileBrowser.Clipboard.InsCod
//	Possible centre:		Gbl.FileBrowser.Clipboard.CtrCod
//	Possible degree:		Gbl.FileBrowser.Clipboard.DegCod
//	Possible course:		Gbl.FileBrowser.Clipboard.CrsCod
//	Possible student in works:	Gbl.FileBrowser.Clipboard.WorksUsrCod
//	Path (file or folder):		Gbl.FileBrowser.Clipboard.FilFolLnk.Full
// Destination:
//	Type of file browser:		Gbl.FileBrowser.Type
//	Possible institution:		Gbl.Hierarchy.Ins.InsCod
//	Possible centre:		Gbl.Hierarchy.Ctr.CtrCod
//	Possible degree:		Gbl.Hierarchy.Deg.DegCod
//	Possible course:		Gbl.Hierarchy.Crs.CrsCod
//	Possible student in works:	Gbl.Usrs.Other.UsrDat.UsrCod
//	Path (should be a folder):	Gbl.FileBrowser.FilFolLnk.Full
// Returns the number of files pasted

static void Brw_PasteClipboard (void)
  {
   extern const char *Txt_The_copy_has_been_successful;
   extern const char *Txt_Files_copied;
   extern const char *Txt_Links_copied;
   extern const char *Txt_Folders_copied;
   extern const char *Txt_You_can_not_paste_file_or_folder_here;
   struct Instit Ins;
   struct Centre Ctr;
   struct Degree Deg;
   struct Course Crs;
   struct GroupData GrpDat;
   struct UsrData UsrDat;
   long PrjCod;
   char PathOrg[PATH_MAX + NAME_MAX + PATH_MAX + 128];
   struct Brw_NumObjects Pasted;
   long FirstFilCod = -1L;	// First file code of the first file or link pasted. Important: initialize here to -1L
   struct FileMetadata FileMetadata;

   Pasted.NumFiles =
   Pasted.NumLinks =
   Pasted.NumFolds = 0;

   Gbl.FileBrowser.Clipboard.IsThisTree = Brw_CheckIfClipboardIsInThisTree ();
   if (Brw_CheckIfCanPasteIn (Gbl.FileBrowser.Level))
     {
      /***** Construct the relative path of the origin file or folder *****/
      switch (Gbl.FileBrowser.Clipboard.FileBrowser)
        {
         case Brw_ADMI_DOC_INS:
         case Brw_ADMI_SHR_INS:
            Ins.InsCod = Gbl.FileBrowser.Clipboard.Cod;
            if (Ins_GetDataOfInstitutionByCod (&Ins,false))
	       snprintf (PathOrg,sizeof (PathOrg),
		         "%s/%02u/%u/%s",
		         Cfg_PATH_INS_PRIVATE,
		         (unsigned) (Ins.InsCod % 100),
		         (unsigned) Ins.InsCod,
			 Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
            else
               Lay_ShowErrorAndExit ("The copy source does not exist.");
            break;
         case Brw_ADMI_DOC_CTR:
         case Brw_ADMI_SHR_CTR:
            Ctr.CtrCod = Gbl.FileBrowser.Clipboard.Cod;
            if (Ctr_GetDataOfCentreByCod (&Ctr))
	       snprintf (PathOrg,sizeof (PathOrg),
		         "%s/%02u/%u/%s",
		         Cfg_PATH_CTR_PRIVATE,
		         (unsigned) (Ctr.CtrCod % 100),
		         (unsigned) Ctr.CtrCod,
			 Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
            else
               Lay_ShowErrorAndExit ("The copy source does not exist.");
            break;
         case Brw_ADMI_DOC_DEG:
         case Brw_ADMI_SHR_DEG:
            Deg.DegCod = Gbl.FileBrowser.Clipboard.Cod;
            if (Deg_GetDataOfDegreeByCod (&Deg))
	       snprintf (PathOrg,sizeof (PathOrg),
		         "%s/%02u/%u/%s",
		         Cfg_PATH_DEG_PRIVATE,
		         (unsigned) (Deg.DegCod % 100),
		         (unsigned) Deg.DegCod,
			 Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
            else
               Lay_ShowErrorAndExit ("The copy source does not exist.");
            break;
         case Brw_ADMI_DOC_CRS:
         case Brw_ADMI_TCH_CRS:
         case Brw_ADMI_SHR_CRS:
         case Brw_ADMI_MRK_CRS:
            Crs.CrsCod = Gbl.FileBrowser.Clipboard.Cod;
            if (Crs_GetDataOfCourseByCod (&Crs))
	       snprintf (PathOrg,sizeof (PathOrg),
		         "%s/%ld/%s",
                         Cfg_PATH_CRS_PRIVATE,Crs.CrsCod,
			 Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
            else
               Lay_ShowErrorAndExit ("The copy source does not exist.");
            break;
         case Brw_ADMI_DOC_GRP:
         case Brw_ADMI_TCH_GRP:
         case Brw_ADMI_SHR_GRP:
         case Brw_ADMI_MRK_GRP:
	    GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.Cod;
	    Grp_GetDataOfGroupByCod (&GrpDat);
            Crs.CrsCod = GrpDat.CrsCod;
            if (Crs_GetDataOfCourseByCod (&Crs))
	       snprintf (PathOrg,sizeof (PathOrg),
		         "%s/%ld/%s/%ld/%s",
                         Cfg_PATH_CRS_PRIVATE,Crs.CrsCod,Cfg_FOLDER_GRP,
			 GrpDat.GrpCod,
			 Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
            else
               Lay_ShowErrorAndExit ("The copy source does not exist.");
            break;
         case Brw_ADMI_ASG_CRS:
         case Brw_ADMI_WRK_CRS:
            Crs.CrsCod = Gbl.FileBrowser.Clipboard.Cod;
            if (Crs_GetDataOfCourseByCod (&Crs))
              {
               Usr_UsrDataConstructor (&UsrDat);
	       UsrDat.UsrCod = Gbl.FileBrowser.Clipboard.WorksUsrCod;
	       Usr_GetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);	// Check that user exists
	       snprintf (PathOrg,sizeof (PathOrg),
		         "%s/%ld/%s/%02u/%ld/%s",
                         Cfg_PATH_CRS_PRIVATE,Crs.CrsCod,Cfg_FOLDER_USR,
			 (unsigned) (Gbl.FileBrowser.Clipboard.WorksUsrCod % 100),
			 Gbl.FileBrowser.Clipboard.WorksUsrCod,
			 Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
	       Usr_UsrDataDestructor (&UsrDat);
              }
            else
               Lay_ShowErrorAndExit ("The copy source does not exist.");
            break;
         case Brw_ADMI_ASG_USR:
         case Brw_ADMI_WRK_USR:
            Crs.CrsCod = Gbl.FileBrowser.Clipboard.Cod;
            if (Crs_GetDataOfCourseByCod (&Crs))
	       snprintf (PathOrg,sizeof (PathOrg),
		         "%s/%ld/%s/%02u/%ld/%s",
                         Cfg_PATH_CRS_PRIVATE,Crs.CrsCod,Cfg_FOLDER_USR,
			 (unsigned) (Gbl.Usrs.Me.UsrDat.UsrCod % 100),
			 Gbl.Usrs.Me.UsrDat.UsrCod,
			 Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
            else
               Lay_ShowErrorAndExit ("The copy source does not exist.");
            break;
         case Brw_ADMI_DOC_PRJ:
         case Brw_ADMI_ASS_PRJ:
            PrjCod = Gbl.FileBrowser.Clipboard.Cod;
	    Crs.CrsCod = Prj_GetCourseOfProject (PrjCod);
	    if (Crs_GetDataOfCourseByCod (&Crs))
	       snprintf (PathOrg,sizeof (PathOrg),
		         "%s/%ld/%s/%02u/%ld/%s",
			Cfg_PATH_CRS_PRIVATE,Crs.CrsCod,Cfg_FOLDER_PRJ,
			(unsigned) (PrjCod % 100),
			PrjCod,
			Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
	    else
	       Lay_ShowErrorAndExit ("The copy source does not exist.");
            break;
         case Brw_ADMI_BRF_USR:
            snprintf (PathOrg,sizeof (PathOrg),
        	      "%s/%s",
        	      Gbl.Usrs.Me.PathDir,
        	      Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
            break;
         default:
            Lay_ShowErrorAndExit ("Wrong file browser.");
            break;
        }

      /***** Paste tree (path in clipboard) into folder *****/
      Brw_CalcSizeOfDir (Gbl.FileBrowser.Priv.PathRootFolder);
      Brw_SetMaxQuota ();
      if (Brw_PasteTreeIntoFolder (Gbl.FileBrowser.Clipboard.Level,
	                           PathOrg,
                                   Gbl.FileBrowser.FilFolLnk.Full,
	                           &Pasted,
	                           &FirstFilCod))
        {
         /***** Write message of success *****/
         Ale_ShowAlert (Ale_SUCCESS,"%s<br />"
                                    "%s: %u<br />"
                                    "%s: %u<br />"
                                    "%s: %u",
                        Txt_The_copy_has_been_successful,
                        Txt_Files_copied  ,Pasted.NumFiles,
                        Txt_Links_copied  ,Pasted.NumLinks,
                        Txt_Folders_copied,Pasted.NumFolds);

         /***** Notify new files *****/
	 if (Pasted.NumFiles ||
	     Pasted.NumLinks)
	   {
	    FileMetadata.FilCod = FirstFilCod;
            Brw_GetFileMetadataByCod (&FileMetadata);

	    /* Notify only is destination folder is visible */
	    if (!Brw_CheckIfFileOrFolderIsHidden (&FileMetadata))
	       switch (Gbl.FileBrowser.Type)
		 {
		  case Brw_ADMI_DOC_CRS:
		  case Brw_ADMI_DOC_GRP:
		     Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_DOCUMENT_FILE,FirstFilCod);
		     break;
		  case Brw_ADMI_TCH_CRS:
		  case Brw_ADMI_TCH_GRP:
		     Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_TEACHERS_FILE,FirstFilCod);
		     break;
		  case Brw_ADMI_SHR_CRS:
		  case Brw_ADMI_SHR_GRP:
		     Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_SHARED_FILE,FirstFilCod);
		     break;
		  case Brw_ADMI_MRK_CRS:
		  case Brw_ADMI_MRK_GRP:
		     Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_MARKS_FILE,FirstFilCod);
		     break;
		  default:
		     break;
		 }
	   }
        }

      /***** Add path where new tree is pasted to table of expanded folders *****/
      Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.FilFolLnk.Full);
     }
   else
      Lay_ShowErrorAndExit (Txt_You_can_not_paste_file_or_folder_here);	// It's difficult, but not impossible that a user sees this message
  }

/*****************************************************************************/
/****************** Compute number of levels in a path ***********************/
/*****************************************************************************/

static unsigned Brw_NumLevelsInPath (const char Path[PATH_MAX + 1])
  {
   unsigned NumLevls = 0;

   while (*Path)
      if (*Path++ == '/')
         NumLevls++;

   return NumLevls;
  }

/*****************************************************************************/
/********** Copy a source file or tree in the destination folder *************/
/*****************************************************************************/
// Return true if the copy has been made successfully, and false if not

static bool Brw_PasteTreeIntoFolder (unsigned LevelOrg,
                                     const char PathOrg[PATH_MAX + 1],
                                     const char PathDstInTree[PATH_MAX + 1],
                                     struct Brw_NumObjects *Pasted,
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
   char PathUntilFileNameOrg[PATH_MAX + 1];
   char FileNameOrg[NAME_MAX + 1];
   char FileNameToShow[NAME_MAX + 1];
   char PathInFolderOrg[PATH_MAX + 1];
   char PathDstInTreeWithFile[PATH_MAX + 1 + NAME_MAX + 1];
   char PathDstWithFile[PATH_MAX + 1 + PATH_MAX + 1 + NAME_MAX + 1];
   struct stat FileStatus;
   struct dirent **FileList;
   bool AdminMarks;
   struct MarksProperties Marks;
   int NumFile;
   int NumFiles;
   unsigned NumLevls;
   long FilCod;	// File code of the file pasted
   bool CopyIsGoingSuccessful = true;

   /***** Get the name (only the name) of the origin file or folder *****/
   Str_SplitFullPathIntoPathAndFileName (PathOrg,
	                                 PathUntilFileNameOrg,
	                                 FileNameOrg);

   /***** Is it a file or a folder? *****/
   FileType = Brw_IS_UNKNOWN;
   if (lstat (PathOrg,&FileStatus))	// On success ==> 0 is returned
      Lay_ShowErrorAndExit ("Can not get information about a file or folder.");
   else if (S_ISDIR (FileStatus.st_mode))	// It's a directory
      FileType = Brw_IS_FOLDER;
   else if (S_ISREG (FileStatus.st_mode))	// It's a regular file
      FileType = Str_FileIs (FileNameOrg,"url") ? Brw_IS_LINK :	// It's a link (URL inside a .url file)
	                                          Brw_IS_FILE;	// It's a file

   /***** Name of the file/folder/link to be shown ****/
   Brw_GetFileNameToShow (FileType,FileNameOrg,FileNameToShow);

   /***** Construct the name of the destination file or folder *****/
   if (LevelOrg == 0)	// Origin of copy is the root folder,
			// for example "sha"
			// ==> do not copy the root folder itself into destination
      Str_Copy (PathDstInTreeWithFile,PathDstInTree,
                PATH_MAX);
   else			// Origin of copy is a file or folder inside the root folder
			// for example "sha/folder1/file1"
      snprintf (PathDstInTreeWithFile,sizeof (PathDstInTreeWithFile),
	        "%s/%s",
	        PathDstInTree,FileNameOrg);

   /***** Construct the relative path of the destination file or folder *****/
   snprintf (PathDstWithFile,sizeof (PathDstWithFile),
	     "%s/%s",
	     Gbl.FileBrowser.Priv.PathAboveRootFolder,
	     PathDstInTreeWithFile);

   /***** Update and check number of levels *****/
   // The number of levels is counted starting on the root folder raíz, not included.
   // Example:	If PathDstInTreeWithFile is "root-folder/1/2/3/4/FileNameOrg", then NumLevls=5
   if ((NumLevls = Brw_NumLevelsInPath (PathDstInTreeWithFile)) > Gbl.FileBrowser.Size.NumLevls)
      Gbl.FileBrowser.Size.NumLevls = NumLevls;
   if (Brw_CheckIfQuotaExceded ())
     {
      switch (FileType)
        {
	 case Brw_IS_FILE:
            Ale_ShowAlert (Ale_WARNING,Txt_The_copy_has_stopped_when_trying_to_paste_the_file_X_because_it_would_exceed_the_maximum_allowed_number_of_levels,
		           FileNameToShow);
	    break;
	 case Brw_IS_FOLDER:
            Ale_ShowAlert (Ale_WARNING,Txt_The_copy_has_stopped_when_trying_to_paste_the_folder_X_because_it_would_exceed_the_maximum_allowed_number_of_levels,
		           FileNameToShow);
	    break;
	 case Brw_IS_LINK:
            Ale_ShowAlert (Ale_WARNING,Txt_The_copy_has_stopped_when_trying_to_paste_the_link_X_because_it_would_exceed_the_maximum_allowed_number_of_levels,
		           FileNameToShow);
	    break;
	 default:
            Lay_ShowErrorAndExit ("Can not paste unknown file type.");
        }
      CopyIsGoingSuccessful = false;
     }
   else	// Quota not exceeded
     {
      /***** Copy file or folder *****/
      if (FileType == Brw_IS_FILE ||
	  FileType == Brw_IS_LINK)	// It's a regular file
	{
	 /***** Check if exists the destination file */
	 if (Fil_CheckIfPathExists (PathDstWithFile))
	   {
	    Ale_ShowAlert (Ale_WARNING,FileType == Brw_IS_FILE ? Txt_The_copy_has_stopped_when_trying_to_paste_the_file_X_because_there_is_already_an_object_with_that_name :
					                         Txt_The_copy_has_stopped_when_trying_to_paste_the_link_X_because_there_is_already_an_object_with_that_name,
		           FileNameToShow);
	    CopyIsGoingSuccessful = false;
	   }
	 else	// Destination file does not exist
	   {
	    /***** If the target file browser is that of marks, only HTML files are allowed *****/
	    AdminMarks = Gbl.FileBrowser.Type == Brw_ADMI_MRK_CRS ||
			 Gbl.FileBrowser.Type == Brw_ADMI_MRK_GRP;
	    if (AdminMarks)
	      {
	       /* Check extension of the file */
	       if (Str_FileIsHTML (FileNameOrg))
		  Mrk_CheckFileOfMarks (PathOrg,&Marks);	// Gbl.Alert.Txt contains feedback text
	       else
		 {
		  Ale_ShowAlert (Ale_WARNING,Txt_The_copy_has_stopped_when_trying_to_paste_the_file_X_because_you_can_not_paste_a_file_here_of_a_type_other_than_HTML,
			         FileNameToShow);
	          CopyIsGoingSuccessful = false;
		 }
	      }

            if (CopyIsGoingSuccessful)
              {
	       /***** Update and check the quota before copying the file *****/
	       Gbl.FileBrowser.Size.NumFiles++;
	       Gbl.FileBrowser.Size.TotalSiz += (unsigned long long) FileStatus.st_size;
	       if (Brw_CheckIfQuotaExceded ())
		 {
		  Ale_ShowAlert (Ale_WARNING,FileType == Brw_IS_FILE ? Txt_The_copy_has_stopped_when_trying_to_paste_the_file_X_because_it_would_exceed_the_disk_quota :
						                       Txt_The_copy_has_stopped_when_trying_to_paste_the_link_X_because_it_would_exceed_the_disk_quota,
			         FileNameToShow);
		  CopyIsGoingSuccessful = false;
		 }
	       else	// Quota not exceeded
		 {
		  /***** Quota will not be exceeded ==> copy the origin file to the destination file *****/
		  Fil_FastCopyOfFiles (PathOrg,PathDstWithFile);

		  /***** Add entry to the table of files/folders *****/
		  FilCod = Brw_AddPathToDB (Gbl.Usrs.Me.UsrDat.UsrCod,FileType,
					    PathDstInTreeWithFile,false,Brw_LICENSE_DEFAULT);
		  if (*FirstFilCod <= 0)
		     *FirstFilCod = FilCod;

		  /* Add a new entry of marks into database */
		  if (AdminMarks)
		     Mrk_AddMarksToDB (FilCod,&Marks);

		  if (FileType == Brw_IS_FILE)
		     (Pasted->NumFiles)++;
		  else // FileType == Brw_IS_LINK
		     (Pasted->NumLinks)++;
		 }
              }
	   }
	}
      else if (FileType == Brw_IS_FOLDER)	// It's a directory
	{
	 /***** Scan the source directory *****/
	 if ((NumFiles = scandir (PathOrg,&FileList,NULL,alphasort)) >= 0)	// No error
	   {
	    /***** Create the folder in the destination *****/
	    if (!Fil_CheckIfPathExists (PathDstWithFile))	// If already exists, don't overwrite
	      {
	       /* The directory does not exist ==> create it.
		  First, update and check the quota */
	       Gbl.FileBrowser.Size.NumFolds++;
	       Gbl.FileBrowser.Size.TotalSiz += (unsigned long long) FileStatus.st_size;
	       if (Brw_CheckIfQuotaExceded ())
		 {
		  Ale_ShowAlert (Ale_WARNING,Txt_The_copy_has_stopped_when_trying_to_paste_the_folder_X_because_it_would_exceed_the_disk_quota,
			         FileNameToShow);
		  CopyIsGoingSuccessful = false;
		 }
	       else	// Quota not exceded
		 {
		  /* Create directory */
		  if (mkdir (PathDstWithFile,(mode_t) 0xFFF) != 0)
		     Lay_ShowErrorAndExit ("Can not create folder.");

		  /* Add entry to the table of files/folders */
		  Brw_AddPathToDB (Gbl.Usrs.Me.UsrDat.UsrCod,FileType,
				   PathDstInTreeWithFile,false,Brw_LICENSE_DEFAULT);
		 }
	      }

	    /***** Copy each of the files and folders from the origin to the destination *****/
	    for (NumFile = 0;
		 NumFile < NumFiles;
		 NumFile++)
	      {
	       if (CopyIsGoingSuccessful &&
		   strcmp (FileList[NumFile]->d_name,".") &&
		   strcmp (FileList[NumFile]->d_name,".."))	// Skip directories "." and ".."
		 {
		  snprintf (PathInFolderOrg,sizeof (PathInFolderOrg),
			    "%s/%s",
			    PathOrg,FileList[NumFile]->d_name);
		  /* Recursive call to this function */
		  if (!Brw_PasteTreeIntoFolder (LevelOrg + 1,
			                        PathInFolderOrg,
		                                PathDstInTreeWithFile,
						Pasted,
						FirstFilCod))
		     CopyIsGoingSuccessful = false;
		 }
	       free (FileList[NumFile]);
	      }
	    free (FileList);
	   }
	 else
	    Lay_ShowErrorAndExit ("Error while scanning directory.");

	 if (CopyIsGoingSuccessful &&
	     LevelOrg != 0)	// When copying all files inside root folder,
				// do not count the root folder itself
	    (Pasted->NumFolds)++;
	}
     }

   return CopyIsGoingSuccessful;
  }

/*****************************************************************************/
/************** Form to add a file or folder to a file browser ***************/
/*****************************************************************************/

void Brw_ShowFormFileBrowser (void)
  {
   extern const char *Txt_You_can_not_create_folders_files_or_links_here;
   char FileNameToShow[NAME_MAX + 1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Check if creating a new folder or file is allowed *****/
   if (Brw_CheckIfICanCreateIntoFolder (Gbl.FileBrowser.Level))
     {
      /***** Name of the folder to be shown ****/
      Brw_GetFileNameToShowDependingOnLevel (Gbl.FileBrowser.Type,
                                             Gbl.FileBrowser.Level,
                                             Gbl.FileBrowser.FilFolLnk.Type,
                                             Gbl.FileBrowser.FilFolLnk.Name,
                                             FileNameToShow);

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
      if (Gbl.FileBrowser.Type != Brw_ADMI_MRK_CRS &&
	  Gbl.FileBrowser.Type != Brw_ADMI_MRK_GRP)	// Do not create links in marks
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

static void Brw_PutFormToCreateAFolder (const char FileNameToShow[NAME_MAX + 1])
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Create_folder;
   extern const char *Txt_You_can_create_a_new_folder_inside_the_folder_X;
   extern const char *Txt_Folder;

   /***** Begin form *****/
   Frm_StartForm (Brw_ActCreateFolder[Gbl.FileBrowser.Type]);
   Brw_PutImplicitParamsFileBrowser ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Create_folder,NULL,
                 NULL,Box_NOT_CLOSABLE);
   Ale_ShowAlert (Ale_INFO,Txt_You_can_create_a_new_folder_inside_the_folder_X,
	          FileNameToShow);

   /***** Folder *****/
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s: ",Txt_Folder);
   HTM_INPUT_TEXT ("NewFolderName",Brw_MAX_CHARS_FOLDER,"",false,
		   "size=\"30\" required=\"required\"");
   HTM_LABEL_End ();

   /***** Send button and end box *****/
   Box_EndBoxWithButton (Btn_CREATE_BUTTON,Txt_Create_folder);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/*** Put form to upload several files to a file browser using Dropzone.js ****/
/*****************************************************************************/

static void Brw_PutFormToUploadFilesUsingDropzone (const char *FileNameToShow)
  {
   extern const char *Txt_Upload_files;
   extern const char *Txt_or_you_can_upload_new_files_to_the_folder_X;
   extern const char *Txt_Select_one_or_more_files_from_your_computer_or_drag_and_drop_here;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_Done;

   /***** Begin box *****/
   HTM_DIV_Begin ("id=\"dropzone-upload\"");
   Box_BoxBegin ("95%",Txt_Upload_files,NULL,
                 NULL,Box_NOT_CLOSABLE);

   /***** Help message *****/
   Ale_ShowAlert (Ale_INFO,Txt_or_you_can_upload_new_files_to_the_folder_X,
	          FileNameToShow);

   /***** Form to upload files using the library Dropzone.js *****/
   // Use min-height:125px; or other number to stablish the height?
   Gbl.Form.Num++;
   fprintf (Gbl.F.Out,"<form method=\"post\" action=\"%s/%s\""
                      " class=\"dropzone\""
                      " enctype=\"multipart/form-data\""
                      " id=\"my-awesome-dropzone\""
                      " style=\"display:inline-block; width:100%%;"
                      " background:url('%s/upload320x320.gif') no-repeat center;\">",
            Cfg_URL_SWAD_CGI,
            Lan_STR_LANG_ID[Gbl.Prefs.Language],
            Cfg_URL_ICON_PUBLIC);
   Par_PutHiddenParamLong (NULL,"act",Act_GetActCod (Brw_ActUploadFileDropzone[Gbl.FileBrowser.Type]));
   Par_PutHiddenParamString (NULL,"ses",Gbl.Session.Id);
   Brw_PutImplicitParamsFileBrowser ();

   HTM_DIV_Begin ("class=\"dz-message\"");
   fprintf (Gbl.F.Out,"<span class=\"DAT_LIGHT\">%s</span>",
            Txt_Select_one_or_more_files_from_your_computer_or_drag_and_drop_here);
   HTM_DIV_End ();
   fprintf (Gbl.F.Out,"</form>");

   /***** Put button to refresh file browser after upload *****/
   Frm_StartForm (Brw_ActRefreshAfterUploadFiles[Gbl.FileBrowser.Type]);
   Brw_PutParamsFileBrowser (NULL,		// Not used
			     NULL,		// Not used
			     Brw_IS_UNKNOWN,	// Not used
			     -1L);		// Not used

   /***** Button to send *****/
   Btn_PutConfirmButton (Txt_Done);

   /***** End form *****/
   Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*** Put form to upload several files to a file browser using Dropzone.js ****/
/*****************************************************************************/

static void Brw_PutFormToUploadOneFileClassic (const char *FileNameToShow)
  {
   extern const char *Txt_Upload_file;
   extern const char *Txt_or_you_can_upload_a_new_file_to_the_folder_X;

   /***** Begin box *****/
   HTM_DIV_Begin ("id=\"classic-upload\" style=\"display:none;\"");
   Box_BoxBegin (NULL,Txt_Upload_file,NULL,
                 NULL,Box_NOT_CLOSABLE);

   /***** Help message *****/
   Ale_ShowAlert (Ale_INFO,Txt_or_you_can_upload_a_new_file_to_the_folder_X,
	          FileNameToShow);

   /***** Form to upload one files using the classic way *****/
   Frm_StartForm (Brw_ActUploadFileClassic[Gbl.FileBrowser.Type]);
   Brw_PutImplicitParamsFileBrowser ();
   HTM_INPUT_FILE ("*",false);

   /* Button to send */
   Btn_PutCreateButton (Txt_Upload_file);
   Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********* Put form to paste a file or a folder into a file browser **********/
/*****************************************************************************/

static void Brw_PutFormToPasteAFileOrFolder (const char *FileNameToShow)
  {
   extern const char *Txt_Paste;
   extern const char *Txt_or_you_can_make_a_file_copy_to_the_folder_X;

   /***** Begin form *****/
   Frm_StartForm (Brw_ActPaste[Gbl.FileBrowser.Type]);
   Brw_PutImplicitParamsFileBrowser ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Paste,NULL,
                 NULL,Box_NOT_CLOSABLE);

   /***** Help message *****/
   Ale_ShowAlert (Ale_INFO,Txt_or_you_can_make_a_file_copy_to_the_folder_X,
	          FileNameToShow);

   /***** Send button and end box *****/
   Box_EndBoxWithButton (Btn_CREATE_BUTTON,Txt_Paste);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/************** Put form to create a new link in a file browser **************/
/*****************************************************************************/

static void Brw_PutFormToCreateALink (const char *FileNameToShow)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Create_link;
   extern const char *Txt_or_you_can_create_a_new_link_inside_the_folder_X;
   extern const char *Txt_URL;
   extern const char *Txt_Save_as;
   extern const char *Txt_optional;

   /***** Begin form *****/
   Frm_StartForm (Brw_ActCreateLink[Gbl.FileBrowser.Type]);
   Brw_PutImplicitParamsFileBrowser ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Create_link,NULL,
                 NULL,Box_NOT_CLOSABLE);

   /***** Help message *****/
   Ale_ShowAlert (Ale_INFO,Txt_or_you_can_create_a_new_link_inside_the_folder_X,
	          FileNameToShow);

   /***** URL *****/
   HTM_TABLE_Begin (NULL);
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   HTM_LABEL_Begin ("for=\"NewLinkURL\" class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s:&nbsp;",Txt_URL);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_URL ("NewLinkURL","",false,
		  "size=\"30\" required=\"required\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Link name *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   HTM_LABEL_Begin ("for=\"NewLinkName\" class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s&nbsp;(%s):&nbsp;",Txt_Save_as,Txt_optional);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("NewLinkName",Brw_MAX_CHARS_FOLDER,"",false,
		   "size=\"30\"");
   HTM_TD_End ();

   HTM_TR_End ();
   HTM_TABLE_End ();

   /***** Send button and end box *****/
   Box_EndBoxWithButton (Btn_CREATE_BUTTON,Txt_Create_link);

   /***** End form *****/
   Frm_EndForm ();
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
   char Path[PATH_MAX + 1 + PATH_MAX + 1];
   char PathCompleteInTreeIncludingFolder[PATH_MAX + 1 + NAME_MAX + 1];
   char FileNameToShow[NAME_MAX + 1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Check if creating a new folder is allowed *****/
   if (Brw_CheckIfICanCreateIntoFolder (Gbl.FileBrowser.Level))
     {
      if (Str_ConvertFilFolLnkNameToValid (Gbl.FileBrowser.NewFilFolLnkName))
        {
         /* In Gbl.FileBrowser.NewFilFolLnkName is the name of the new folder */
         snprintf (Path,sizeof (Path),
                   "%s/%s",
        	   Gbl.FileBrowser.Priv.PathAboveRootFolder,
		   Gbl.FileBrowser.FilFolLnk.Full);

         if (strlen (Path) + 1 + strlen (Gbl.FileBrowser.NewFilFolLnkName) > PATH_MAX)
	    Lay_ShowErrorAndExit ("Path is too long.");
         Str_Concat (Path,"/",
                     PATH_MAX);
         Str_Concat (Path,Gbl.FileBrowser.NewFilFolLnkName,
                     PATH_MAX);

         /* Create the new directory */
         if (mkdir (Path,(mode_t) 0xFFF) == 0)
	   {
	    /* Check if quota has been exceeded */
	    Brw_CalcSizeOfDir (Gbl.FileBrowser.Priv.PathRootFolder);
	    Brw_SetMaxQuota ();
            if (Brw_CheckIfQuotaExceded ())
	      {
	       Fil_RemoveTree (Path);
               Ale_ShowAlert (Ale_WARNING,Txt_Can_not_create_the_folder_X_because_it_would_exceed_the_disk_quota,
                              Gbl.FileBrowser.NewFilFolLnkName);
	      }
	    else
              {
               /* Remove affected clipboards */
               Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
        				     Gbl.Usrs.Me.UsrDat.UsrCod,
					     Gbl.Usrs.Other.UsrDat.UsrCod);

               /* Add path where new file is created to table of expanded folders */
               Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.FilFolLnk.Full);

               /* Add entry to the table of files/folders */
               snprintf (PathCompleteInTreeIncludingFolder,sizeof (PathCompleteInTreeIncludingFolder),
			 "%s/%s",
			 Gbl.FileBrowser.FilFolLnk.Full,
			 Gbl.FileBrowser.NewFilFolLnkName);
               Brw_AddPathToDB (Gbl.Usrs.Me.UsrDat.UsrCod,Brw_IS_FOLDER,
                                PathCompleteInTreeIncludingFolder,false,Brw_LICENSE_DEFAULT);

	       /* The folder has been created sucessfully */
               Brw_GetFileNameToShowDependingOnLevel (Gbl.FileBrowser.Type,
                                                      Gbl.FileBrowser.Level,
                                                      Brw_IS_FOLDER,
                                                      Gbl.FileBrowser.FilFolLnk.Name,
                                                      FileNameToShow);
               Ale_ShowAlert (Ale_SUCCESS,Txt_The_folder_X_has_been_created_inside_the_folder_Y,
		              Gbl.FileBrowser.NewFilFolLnkName,FileNameToShow);
              }
	   }
         else
	   {
	    switch (errno)
	      {
	       case EEXIST:
                  Ale_ShowAlert (Ale_WARNING,Txt_Can_not_create_the_folder_X_because_there_is_already_a_folder_or_a_file_with_that_name,
		   	         Gbl.FileBrowser.NewFilFolLnkName);
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
         Ale_ShowAlerts (NULL);
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
   char OldPathInTree[PATH_MAX + 1 + NAME_MAX + 1];
   char NewPathInTree[PATH_MAX + 1 + NAME_MAX + 1];
   char OldPath[PATH_MAX + 1 + PATH_MAX + 1];
   char NewPath[PATH_MAX + 1 + PATH_MAX + 1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   if (Brw_CheckIfICanEditFileOrFolder (Gbl.FileBrowser.Level))	// Can I rename this folder?
     {
      if (Str_ConvertFilFolLnkNameToValid (Gbl.FileBrowser.NewFilFolLnkName))
        {
         if (strcmp (Gbl.FileBrowser.FilFolLnk.Name,Gbl.FileBrowser.NewFilFolLnkName))	// The name has changed
           {
            /* Gbl.FileBrowser.FilFolLnk.Name holds the new name of the folder */
            snprintf (OldPathInTree,sizeof (OldPathInTree),
        	      "%s/%s",
        	      Gbl.FileBrowser.FilFolLnk.Path,
		      Gbl.FileBrowser.FilFolLnk.Name);
            snprintf (OldPath,sizeof (OldPath),
        	      "%s/%s",
        	      Gbl.FileBrowser.Priv.PathAboveRootFolder,
		      OldPathInTree);

            /* Gbl.FileBrowser.NewFilFolLnkName holds the new name of the folder */
            if (strlen (Gbl.FileBrowser.Priv.PathAboveRootFolder) + 1 +
                strlen (Gbl.FileBrowser.FilFolLnk.Path) + 1 +
                strlen (Gbl.FileBrowser.NewFilFolLnkName) > PATH_MAX)
	       Lay_ShowErrorAndExit ("Path is too long.");
            snprintf (NewPathInTree,sizeof (NewPathInTree),
        	      "%s/%s",
        	      Gbl.FileBrowser.FilFolLnk.Path,
		      Gbl.FileBrowser.NewFilFolLnkName);
            snprintf (NewPath,sizeof (NewPath),
        	      "%s/%s",
        	      Gbl.FileBrowser.Priv.PathAboveRootFolder,
		      NewPathInTree);

            /* We should check here that a folder with the same name does not exist.
	       but we leave this work to the system */

            /* Rename the directory. If a empty folder existed with the name new, overwrite it! */
            if (rename (OldPath,NewPath))	// Fail
	      {
	       switch (errno)
	         {
	          case ENOTEMPTY:
	          case EEXIST:
	          case ENOTDIR:
                     Ale_ShowAlert (Ale_WARNING,Txt_The_folder_name_X_has_not_changed_because_there_is_already_a_folder_or_a_file_with_the_name_Y,
			            Gbl.FileBrowser.FilFolLnk.Name,Gbl.FileBrowser.NewFilFolLnkName);
	             break;
	          case EACCES:
	             Lay_ShowErrorAndExit ("Write forbidden.");
	             break;
	          default:
	             Lay_ShowErrorAndExit ("Can not rename folder.");
	             break;
	         }
	      }
            else				// Success
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
               Ale_ShowAlert (Ale_SUCCESS,Txt_The_folder_name_X_has_changed_to_Y,
                              Gbl.FileBrowser.FilFolLnk.Name,
                              Gbl.FileBrowser.NewFilFolLnkName);
              }

           }
         else	// Names are equal. This may happens if we have press INTRO without changing the name
            Ale_ShowAlert (Ale_INFO,Txt_The_folder_name_X_has_not_changed,
                           Gbl.FileBrowser.FilFolLnk.Name);
        }
      else	// Folder name not valid
         Ale_ShowAlerts (NULL);
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
   Gbl.Layout.DivsEndWritten   =
   Gbl.Layout.HTMLEndWritten   = true;

   /* Start HTTP response */
   fprintf (stdout,"Content-type: text/plain; charset=windows-1252\n");

   /* Status code and message */
   if (UploadSucessful)
      fprintf (stdout,"Status: 200\r\n\r\n");
   else
      fprintf (stdout,"Status: 501 Not Implemented\r\n\r\n"
		      "%s\n",
	       Ale_GetTextOfLastAlert ());
  }

/*****************************************************************************/
/******** Receive a new file in a file browser using the classic way *********/
/*****************************************************************************/

void Brw_RcvFileInFileBrwClassic (void)
  {
   /***** Receive file and show feedback message *****/
   Brw_RcvFileInFileBrw (Brw_CLASSIC_UPLOAD);

   /***** Show possible alert *****/
   Ale_ShowAlerts (NULL);

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
   struct Param *Param;
   char SrcFileName[PATH_MAX + 1];
   char PathUntilFileName[PATH_MAX + 1];
   char Path[PATH_MAX + 1 + PATH_MAX + 1];
   char PathTmp[PATH_MAX + 1];
   char PathCompleteInTreeIncludingFile[PATH_MAX + 1 + NAME_MAX + 1];
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE + 1];
   bool AdminMarks;
   bool FileIsValid = true;
   long FilCod = -1L;	// Code of new file in database
   struct FileMetadata FileMetadata;
   struct MarksProperties Marks;
   char FileNameToShow[NAME_MAX + 1];
   bool UploadSucessful = false;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();
   AdminMarks = Gbl.FileBrowser.Type == Brw_ADMI_MRK_CRS ||
                Gbl.FileBrowser.Type == Brw_ADMI_MRK_GRP;

   /***** Check if creating a new file is allowed *****/
   if (Brw_CheckIfICanCreateIntoFolder (Gbl.FileBrowser.Level))
     {
      /***** First, we save in disk the file from stdin (really from Gbl.F.Tmp) *****/
      Param = Fil_StartReceptionOfFile (Fil_NAME_OF_PARAM_FILENAME_ORG,
                                        SrcFileName,MIMEType);

      /***** Get filename from path *****/
      // Spaces at start or end are allowed
      Str_SplitFullPathIntoPathAndFileName (SrcFileName,
	                                    PathUntilFileName,
	                                    Gbl.FileBrowser.NewFilFolLnkName);
      if (Gbl.FileBrowser.NewFilFolLnkName[0])
        {
         /***** Check if uploading this kind of file is allowed *****/
	 if (Brw_CheckIfUploadIsAllowed (MIMEType))	// Gbl.Alert.Txt contains feedback text
           {
            if (Str_ConvertFilFolLnkNameToValid (Gbl.FileBrowser.NewFilFolLnkName))
              {
               /* Gbl.FileBrowser.NewFilFolLnkName holds the name of the new file */
               snprintf (Path,sizeof (Path),
        	         "%s/%s",
                         Gbl.FileBrowser.Priv.PathAboveRootFolder,
                         Gbl.FileBrowser.FilFolLnk.Full);
               if (strlen (Path) + 1 +
        	   strlen (Gbl.FileBrowser.NewFilFolLnkName) +
		   strlen (".tmp") > PATH_MAX)
	          Lay_ShowErrorAndExit ("Path is too long.");
               Str_Concat (Path,"/",
                           PATH_MAX);
               Str_Concat (Path,Gbl.FileBrowser.NewFilFolLnkName,
                           PATH_MAX);

               /* Check if the destination file exists */
               if (Fil_CheckIfPathExists (Path))
        	  Ale_CreateAlert (Ale_WARNING,NULL,
        		           Txt_UPLOAD_FILE_X_file_already_exists_NO_HTML,
                                   Gbl.FileBrowser.NewFilFolLnkName);
               else	// Destination file does not exist
                 {
                  /* End receiving the file */
                  snprintf (PathTmp,sizeof (PathTmp),
                	    "%s.tmp",
			    Path);
                  FileIsValid = Fil_EndReceptionOfFile (PathTmp,Param);	// Gbl.Alert.Txt contains feedback text

                  /* Check if the content of the file of marks is valid */
                  if (FileIsValid)
                     if (AdminMarks)
                        if (!Mrk_CheckFileOfMarks (PathTmp,&Marks))	// Gbl.Alert.Txt contains feedback text
                           FileIsValid = false;

                  if (FileIsValid)
                    {
                     /* Rename the temporary */
                     if (rename (PathTmp,Path))	// Fail
	               {
	                Fil_RemoveTree (PathTmp);
        	        Ale_CreateAlert (Ale_WARNING,NULL,
        	                         Txt_UPLOAD_FILE_could_not_create_file_NO_HTML,
                                         Gbl.FileBrowser.NewFilFolLnkName);
	               }
                     else			// Success
	               {
	                /* Check if quota has been exceeded */
	                Brw_CalcSizeOfDir (Gbl.FileBrowser.Priv.PathRootFolder);
	                Brw_SetMaxQuota ();
                        if (Brw_CheckIfQuotaExceded ())
	                  {
	                   Fil_RemoveTree (Path);
        	           Ale_CreateAlert (Ale_WARNING,NULL,
        	        	            Txt_UPLOAD_FILE_X_quota_exceeded_NO_HTML,
		                            Gbl.FileBrowser.NewFilFolLnkName);
	                  }
	                else
                          {
                           /* Remove affected clipboards */
                           Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
                        				 Gbl.Usrs.Me.UsrDat.UsrCod,
							 Gbl.Usrs.Other.UsrDat.UsrCod);

                           /* Add path where new file is created to table of expanded folders */
                           Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.FilFolLnk.Full);

                           /* Add entry to the table of files/folders */
                           snprintf (PathCompleteInTreeIncludingFile,
                        	     sizeof (PathCompleteInTreeIncludingFile),
                        	     "%s/%s",
                        	     Gbl.FileBrowser.FilFolLnk.Full,
				     Gbl.FileBrowser.NewFilFolLnkName);
                           FilCod = Brw_AddPathToDB (Gbl.Usrs.Me.UsrDat.UsrCod,Brw_IS_FILE,
                                                     PathCompleteInTreeIncludingFile,false,Brw_LICENSE_DEFAULT);

                           /* Show message of confirmation */
                           if (UploadType == Brw_CLASSIC_UPLOAD)
                             {
			      Brw_GetFileNameToShowDependingOnLevel (Gbl.FileBrowser.Type,
			                                             Gbl.FileBrowser.Level,
			                                             Brw_IS_FOLDER,
			                                             Gbl.FileBrowser.FilFolLnk.Name,
			                                             FileNameToShow);	// Folder name
        	              Ale_CreateAlert (Ale_SUCCESS,NULL,
        	        	               Txt_The_file_X_has_been_placed_inside_the_folder_Y,
			                       Gbl.FileBrowser.NewFilFolLnkName,
			                       FileNameToShow);
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
				 case Brw_ADMI_DOC_CRS:
				 case Brw_ADMI_DOC_GRP:
				    Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_DOCUMENT_FILE,FilCod);
				    break;
				 case Brw_ADMI_TCH_CRS:
				 case Brw_ADMI_TCH_GRP:
				    Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_TEACHERS_FILE,FilCod);
				    break;
				 case Brw_ADMI_SHR_CRS:
				 case Brw_ADMI_SHR_GRP:
				    Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_SHARED_FILE,FilCod);
				    break;
				 case Brw_ADMI_MRK_CRS:
				 case Brw_ADMI_MRK_GRP:
				    Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_MARKS_FILE,FilCod);
				    break;
				 default:
				    break;
				}
                          }
                       }
	            }
                  else	// Error in file reception. File probably too big
	             Fil_RemoveTree (PathTmp);
                 }
              }
           }
        }
      else	// Empty filename
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_UPLOAD_FILE_You_must_specify_the_file_NO_HTML);
     }
   else		// I do not have permission to create files here
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_UPLOAD_FILE_Forbidden_NO_HTML);

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
   extern const char *Txt_UPLOAD_FILE_Invalid_link;
   extern const char *Txt_You_can_not_create_links_here;
   char URL[PATH_MAX + 1];
   char URLWithoutEndingSlash[PATH_MAX + 1];
   size_t LengthURL;
   char URLUntilLastFilename[PATH_MAX + 1];
   char FileName[NAME_MAX + 1];
   char Path[PATH_MAX + 1 + PATH_MAX + 1];
   FILE *FileURL;
   char PathCompleteInTreeIncludingFile[PATH_MAX + 1 + NAME_MAX + 1];
   long FilCod = -1L;	// Code of new file in database
   char FileNameToShow[NAME_MAX + 1];
   struct FileMetadata FileMetadata;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Check if creating a new link is allowed *****/
   if (Brw_CheckIfICanCreateIntoFolder (Gbl.FileBrowser.Level))
     {
      /***** Create a new file to store URL ****/
      Par_GetParToText ("NewLinkURL",URL,PATH_MAX);
      if ((LengthURL = strlen (URL)))
	{
	 if (Gbl.FileBrowser.NewFilFolLnkName[0])
	    /*
	    Gbl.FileBrowser.NewFilFolLnkName holds the name given by me in the form
	    Example:
	    Name given by me: intel-architectures.pdf
	    File in swad: intel-architectures.pdf.url
	    */
	    Str_Copy (URLWithoutEndingSlash,Gbl.FileBrowser.NewFilFolLnkName,
	              PATH_MAX);
	 else
	    /*
	    Gbl.FileBrowser.NewFilFolLnkName is empty
	    URL holds the URL given by me in the form
	    Example:
	    URL: http://www.intel.es/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-manual-325462.pdf
	    File in swad: 64-ia-32-architectures-software-developer-manual-325462.pdf.url
	    */
	    Str_Copy (URLWithoutEndingSlash,URL,
	              PATH_MAX);

	 /* Remove possible final '/' from URL */
	 if (URLWithoutEndingSlash[LengthURL - 1] == '/')
	    URLWithoutEndingSlash[LengthURL - 1] = '\0';

	 /* Get the last name in URL-without-ending-slash */
	 Str_SplitFullPathIntoPathAndFileName (URLWithoutEndingSlash,
					       URLUntilLastFilename,
					       FileName);

	 /* Convert the last name in URL to a valid filename */
	 if (Str_ConvertFilFolLnkNameToValid (FileName))	// Gbl.Alert.Txt contains feedback text
	   {
	    /* The name of the file with the link will be the FileName.url */
	    snprintf (Path,sizeof (Path),
		      "%s/%s",
		      Gbl.FileBrowser.Priv.PathAboveRootFolder,
		      Gbl.FileBrowser.FilFolLnk.Full);
	    if (strlen (Path) + 1 + strlen (FileName) + strlen (".url") > PATH_MAX)
	       Lay_ShowErrorAndExit ("Path is too long.");
	    Str_Concat (Path,"/",
	                PATH_MAX);
	    Str_Concat (Path,FileName,
	                PATH_MAX);
	    Str_Concat (Path,".url",
	                PATH_MAX);

	    /* Check if the URL file exists */
	    if (Fil_CheckIfPathExists (Path))
	       Ale_ShowAlert (Ale_WARNING,Txt_Can_not_create_the_link_X_because_there_is_already_a_folder_or_a_link_with_that_name,
			      FileName);
	    else	// URL file does not exist
	      {
	       /***** Create the new file with the URL *****/
	       if ((FileURL = fopen (Path,"wb")) != NULL)
		 {
		  /* Write URL */
		  fprintf (FileURL,"%s",URL);

		  /* Close file */
		  fclose (FileURL);

		  /* Check if quota has been exceeded */
		  Brw_CalcSizeOfDir (Gbl.FileBrowser.Priv.PathRootFolder);
		  Brw_SetMaxQuota ();
		  if (Brw_CheckIfQuotaExceded ())
		    {
		     Fil_RemoveTree (Path);
		     Ale_ShowAlert (Ale_WARNING,Txt_Can_not_create_the_link_X_because_it_would_exceed_the_disk_quota,
			            FileName);
		    }
		  else
		    {
		     /* Remove affected clipboards */
		     Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
						   Gbl.Usrs.Me.UsrDat.UsrCod,
						   Gbl.Usrs.Other.UsrDat.UsrCod);

		     /* Add path where new file is created to table of expanded folders */
		     Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.FilFolLnk.Full);

		     /* Add entry to the table of files/folders */
		     snprintf (PathCompleteInTreeIncludingFile,
			       sizeof (PathCompleteInTreeIncludingFile),
			       "%s/%s.url",
			       Gbl.FileBrowser.FilFolLnk.Full,
			       FileName);
		     FilCod = Brw_AddPathToDB (Gbl.Usrs.Me.UsrDat.UsrCod,Brw_IS_LINK,
					       PathCompleteInTreeIncludingFile,false,Brw_LICENSE_DEFAULT);

		     /* Show message of confirmation */
		     Brw_GetFileNameToShowDependingOnLevel (Gbl.FileBrowser.Type,
		                                            Gbl.FileBrowser.Level,
		                                            Brw_IS_FOLDER,
					                    Gbl.FileBrowser.FilFolLnk.Name,
					                    FileNameToShow);	// Folder name
		     Ale_ShowAlert (Ale_SUCCESS,Txt_The_link_X_has_been_placed_inside_the_folder_Y,
			            FileName,FileNameToShow);

		     FileMetadata.FilCod = FilCod;
		     Brw_GetFileMetadataByCod (&FileMetadata);

		     /* Notify new file */
		     if (!Brw_CheckIfFileOrFolderIsHidden (&FileMetadata))
			switch (Gbl.FileBrowser.Type)
			  {
			   case Brw_ADMI_DOC_CRS:
			   case Brw_ADMI_DOC_GRP:
			      Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_DOCUMENT_FILE,FilCod);
			      break;
			   case Brw_ADMI_TCH_CRS:
			   case Brw_ADMI_TCH_GRP:
			      Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_TEACHERS_FILE,FilCod);
			      break;
			   case Brw_ADMI_SHR_CRS:
			   case Brw_ADMI_SHR_GRP:
			      Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_SHARED_FILE,FilCod);
			      break;
			   case Brw_ADMI_MRK_CRS:
			   case Brw_ADMI_MRK_GRP:
			      Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_MARKS_FILE,FilCod);
			      break;
			   default:
			      break;
			  }
		    }
		 }
	      }
	   }
	 else	// Link URL not valid
	    Ale_ShowAlert (Ale_WARNING,Txt_UPLOAD_FILE_Invalid_link);
	}
      else	// Link URL not valid
	 Ale_ShowAlert (Ale_WARNING,Txt_UPLOAD_FILE_Invalid_link);
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
// On error, delayed alerts will contain feedback text

static bool Brw_CheckIfUploadIsAllowed (const char *MIMEType)
  {
   extern const char *Txt_UPLOAD_FILE_X_MIME_type_Y_not_allowed_NO_HTML;
   extern const char *Txt_UPLOAD_FILE_X_not_HTML_NO_HTML;
   extern const char *Txt_UPLOAD_FILE_X_extension_not_allowed_NO_HTML;

   switch (Gbl.FileBrowser.Type)
     {
      case Brw_ADMI_MRK_CRS:
      case Brw_ADMI_MRK_GRP:
	 /* Check file extension */
	 if (!Str_FileIsHTML (Gbl.FileBrowser.NewFilFolLnkName))
	   {
	    Ale_CreateAlert (Ale_WARNING,NULL,
		             Txt_UPLOAD_FILE_X_not_HTML_NO_HTML,
		             Gbl.FileBrowser.NewFilFolLnkName);
	    return false;
	   }

	 /* Check MIME type*/
	 if (strcmp (MIMEType,"text/html"))
	    if (strcmp (MIMEType,"text/plain"))
	       if (strcmp (MIMEType,"application/octet-stream"))
		  if (strcmp (MIMEType,"application/octetstream"))
		     if (strcmp (MIMEType,"application/octet"))
		       {	// MIME type forbidden
			Ale_CreateAlert (Ale_WARNING,NULL,
		                         Txt_UPLOAD_FILE_X_MIME_type_Y_not_allowed_NO_HTML,
				         Gbl.FileBrowser.NewFilFolLnkName,MIMEType);
			return false;
		       }
	 break;
      default:
	 /* Check file extension */
	 if (!Ext_CheckIfFileExtensionIsAllowed (Gbl.FileBrowser.NewFilFolLnkName))
	   {
	    Ale_CreateAlert (Ale_WARNING,NULL,
			     Txt_UPLOAD_FILE_X_extension_not_allowed_NO_HTML,
		             Gbl.FileBrowser.NewFilFolLnkName);
	    return false;
	   }

	 /* Check MIME type*/
	 if (!MIM_CheckIfMIMETypeIsAllowed (MIMEType))
	   {
	    Ale_CreateAlert (Ale_WARNING,NULL,
			     Txt_UPLOAD_FILE_X_MIME_type_Y_not_allowed_NO_HTML,
		             Gbl.FileBrowser.NewFilFolLnkName,MIMEType);
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
   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Change file to visible *****/
   if (Brw_CheckIfFileOrFolderIsSetAsHiddenInDB (Gbl.FileBrowser.FilFolLnk.Type,
                                                 Gbl.FileBrowser.FilFolLnk.Full))
      Brw_ChangeFileOrFolderHiddenInDB (Gbl.FileBrowser.FilFolLnk.Full,false);

   /***** Remove the affected clipboards *****/
   Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
				 Gbl.Usrs.Me.UsrDat.UsrCod,
				 Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/***************** Hide file or folder in a file browser *********************/
/*****************************************************************************/

void Brw_SetDocumentAsHidden (void)
  {
   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** If the file or folder is not already set as hidden in database,
          set it as hidden *****/
   if (!Brw_CheckIfFileOrFolderIsSetAsHiddenInDB (Gbl.FileBrowser.FilFolLnk.Type,
                                                  Gbl.FileBrowser.FilFolLnk.Full))
      Brw_ChangeFileOrFolderHiddenInDB (Gbl.FileBrowser.FilFolLnk.Full,true);

   /***** Remove the affected clipboards *****/
   Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
				 Gbl.Usrs.Me.UsrDat.UsrCod,
				 Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/** Check if a file / folder from the documents zone is set as hidden in DB **/
/*****************************************************************************/

bool Brw_CheckIfFileOrFolderIsSetAsHiddenInDB (Brw_FileType_t FileType,const char *Path)
  {
   long Cod = Brw_GetCodForFiles ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFiles ();
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool IsHidden = false;

   /***** Get if a file or folder is hidden from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not check if a file is hidden",
		       "SELECT Hidden FROM files"
		       " WHERE FileBrowser=%u AND Cod=%ld AND ZoneUsrCod=%ld"
		       " AND Path='%s'",
		       (unsigned) Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type],
		       Cod,ZoneUsrCod,
		       Path))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* File is hidden? (row[0]) */
      IsHidden = (row[0][0] == 'Y');
     }
   else
      Brw_AddPathToDB (-1L,FileType,
                       Gbl.FileBrowser.FilFolLnk.Full,false,Brw_LICENSE_DEFAULT);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return IsHidden;
  }

/*****************************************************************************/
/******** Check if a file / folder from the documents zone is hidden *********/
/*****************************************************************************/

bool Brw_CheckIfFileOrFolderIsHidden (struct FileMetadata *FileMetadata)
  {
   /***** Get if a file or folder is under a hidden folder from database *****/
   /*
      The argument Path passed to this function is hidden if:
      1) the argument Path is exactly the same as a path stored in database
         or
      2) the argument Path begins by 'x/', where x is a path stored in database
   */
   return (DB_QueryCOUNT ("can not check if a file or folder is hidden",
			  "SELECT COUNT(*) FROM files"
			  " WHERE FileBrowser=%u AND Cod=%ld AND ZoneUsrCod=%ld"
			  " AND Hidden='Y'"
			  " AND (Path='%s' OR LOCATE(CONCAT(Path,'/'),'%s')=1)",
			  FileMetadata->FileBrowser,
			  FileMetadata->Cod,
			  FileMetadata->ZoneUsrCod,
			  FileMetadata->FilFolLnk.Full,
			  FileMetadata->FilFolLnk.Full) != 0);
  }

/*****************************************************************************/
/***************** Show metadata of a file in a file browser *****************/
/*****************************************************************************/

void Brw_ShowFileMetadata (void)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_The_file_of_folder_no_longer_exists_or_is_now_hidden;
   extern const char *Txt_Filename;
   extern const char *Txt_File_size;
   extern const char *Txt_Uploaded_by;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
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
   extern const char *Txt_users[Usr_NUM_SEXS];
   extern const char *Txt_Save_file_properties;
   struct FileMetadata FileMetadata;
   struct UsrData PublisherUsrDat;
   char FileNameToShow[NAME_MAX + 1];
   char URL[PATH_MAX + 1];
   char FileSizeStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];
   bool Found;
   bool ICanView = false;
   bool IAmTheOwner;
   bool ICanEdit;
   bool ICanChangePublic = false;
   bool FileHasPublisher;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   Brw_License_t License;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get file metadata *****/
   FileMetadata.FilCod = Brw_GetParamFilCod ();
   Brw_GetFileMetadataByCod (&FileMetadata);
   Found = Brw_GetFileTypeSizeAndDate (&FileMetadata);

   if (Found)
     {
      if (FileMetadata.FilCod <= 0)	// No entry for this file in database table of files
	 /* Add entry to the table of files/folders */
	 FileMetadata.FilCod = Brw_AddPathToDB (-1L,FileMetadata.FilFolLnk.Type,
	                                        FileMetadata.FilFolLnk.Full,
	                                        false,Brw_LICENSE_DEFAULT);

      /***** Check if I can view this file.
	     It could be marked as hidden or in a hidden folder *****/
      ICanView = true;
      switch (Gbl.FileBrowser.Type)
	{
	 case Brw_SHOW_DOC_INS:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_INS_ADM)
	       ICanView = !Brw_CheckIfFileOrFolderIsHidden (&FileMetadata);
            break;
	 case Brw_SHOW_DOC_CTR:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_CTR_ADM)
	       ICanView = !Brw_CheckIfFileOrFolderIsHidden (&FileMetadata);
            break;
	 case Brw_SHOW_DOC_DEG:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_DEG_ADM)
	       ICanView = !Brw_CheckIfFileOrFolderIsHidden (&FileMetadata);
            break;
	 case Brw_SHOW_DOC_CRS:
	 case Brw_SHOW_DOC_GRP:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_TCH)
               ICanView = !Brw_CheckIfFileOrFolderIsHidden (&FileMetadata);
	    break;
	 default:
	    break;
	}
     }

   if (ICanView)
     {
      if (FileMetadata.FilFolLnk.Type == Brw_IS_FILE ||
	  FileMetadata.FilFolLnk.Type == Brw_IS_LINK)
	{
	 /***** Update number of views *****/
	 Brw_GetAndUpdateFileViews (&FileMetadata);

	 /***** Get data of file/folder publisher *****/
	 if (FileMetadata.PublisherUsrCod > 0)
	   {
	    /***** Initialize structure with publisher's data *****/
	    Usr_UsrDataConstructor (&PublisherUsrDat);

	    PublisherUsrDat.UsrCod = FileMetadata.PublisherUsrCod;
	    FileHasPublisher = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&PublisherUsrDat,Usr_DONT_GET_PREFS);
	   }
	 else
	    FileHasPublisher = false;	// Get user's data from database

	 /***** Get link to download the file *****/
	 if (Gbl.FileBrowser.Type == Brw_SHOW_MRK_CRS ||
	     Gbl.FileBrowser.Type == Brw_SHOW_MRK_GRP)
	    URL[0] = '\0';
	 else
	    Brw_GetLinkToDownloadFile (FileMetadata.FilFolLnk.Path,
				       FileMetadata.FilFolLnk.Name,
				       URL);

	 /***** Can I edit the properties of the file? *****/
	 IAmTheOwner = Brw_CheckIfIAmOwnerOfFile (FileMetadata.PublisherUsrCod);
	 ICanEdit = Brw_CheckIfICanEditFileMetadata (IAmTheOwner);

	 /***** Name of the file/link to be shown *****/
	 Brw_GetFileNameToShow (FileMetadata.FilFolLnk.Type,
	                        FileMetadata.FilFolLnk.Name,
	                        FileNameToShow);

	 /***** Begin form to update the metadata of a file *****/
	 if (ICanEdit)	// I can edit file properties
	   {
	    /* Can the file be public? */
	    switch (Gbl.FileBrowser.Type)
	      {
	       case Brw_ADMI_DOC_INS:
	       case Brw_ADMI_SHR_INS:
	       case Brw_ADMI_DOC_CTR:
	       case Brw_ADMI_SHR_CTR:
	       case Brw_ADMI_DOC_DEG:
	       case Brw_ADMI_SHR_DEG:
	       case Brw_ADMI_DOC_CRS:
	       case Brw_ADMI_SHR_CRS:
		  ICanChangePublic = true;
		  break;
	       default:
		  ICanChangePublic = false;
		  break;
	      }

	    Frm_StartForm (Brw_ActRecDatFile[Gbl.FileBrowser.Type]);
	    Brw_PutImplicitParamsFileBrowser ();
	   }

         /***** Begin box and table *****/
	 Box_StartBoxTableShadow (NULL,NULL,NULL,NULL,2);

	 /***** Link to download the file *****/
	 HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("colspan=\"2\" class=\"FILENAME_TXT CM\"");
	 Brw_WriteBigLinkToDownloadFile (URL,&FileMetadata,FileNameToShow);
	 HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Filename *****/
	 HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
	 fprintf (Gbl.F.Out,"%s:",Txt_Filename);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"DAT LM\"");
	 Brw_WriteSmallLinkToDownloadFile (URL,&FileMetadata,FileNameToShow);
	 HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Publisher's data *****/
	 HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
	 fprintf (Gbl.F.Out,"%s:",Txt_Uploaded_by);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"DAT LM\"");
	 if (FileHasPublisher)
	   {
	    /* Show photo */
	    ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&PublisherUsrDat,PhotoURL);
	    Pho_ShowUsrPhoto (&PublisherUsrDat,ShowPhoto ? PhotoURL :
	                	                           NULL,
	                      "PHOTO15x20",Pho_ZOOM,false);

	    /* Write name */
	    fprintf (Gbl.F.Out,"%s",
		     PublisherUsrDat.FullName);
	   }
	 else
	    /* Unknown publisher */
	    fprintf (Gbl.F.Out,"%s",Txt_ROLES_SINGUL_Abc[Rol_UNK][Usr_SEX_UNKNOWN]);
	 HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Free memory used for publisher's data *****/
	 if (FileMetadata.PublisherUsrCod > 0)
	    Usr_UsrDataDestructor (&PublisherUsrDat);

	 /***** Write the file size *****/
	 Fil_WriteFileSizeFull ((double) FileMetadata.Size,FileSizeStr);
	 HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
	 fprintf (Gbl.F.Out,"%s:",Txt_File_size);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"DAT LM\"");
	 fprintf (Gbl.F.Out,"%s",FileSizeStr);
	 HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Write the date *****/
	 HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
	 fprintf (Gbl.F.Out,"%s:",Txt_Date_of_creation);
	 HTM_TD_End ();

	 HTM_TD_Begin ("id=\"filedate\" class=\"DAT LM\"");
	 Dat_WriteLocalDateHMSFromUTC ("filedate",FileMetadata.Time,
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				       true,true,true,0x7);
	 HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Private or public? *****/
	 HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"RM\"");
	 HTM_LABEL_Begin ("for=\"PublicFile\" class=\"%s\"",
		          The_ClassFormInBox[Gbl.Prefs.Theme]);
	 fprintf (Gbl.F.Out,"%s:",Txt_Availability);
	 HTM_LABEL_End ();
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"DAT LM\"");
	 if (ICanChangePublic)	// I can change file to public
	   {
	    HTM_SELECT_Begin (false,
			      "id=\"PublicFile\" name=\"PublicFile\"");

	    fprintf (Gbl.F.Out,"<option value=\"N\"");
	    if (!FileMetadata.IsPublic)
	       fprintf (Gbl.F.Out," selected=\"selected\"");
	    fprintf (Gbl.F.Out,">%s</option>",Txt_Private_available_to_certain_users_identified);

	    fprintf (Gbl.F.Out,"<option value=\"Y\"");
	    if (FileMetadata.IsPublic)
	       fprintf (Gbl.F.Out," selected=\"selected\"");
	    fprintf (Gbl.F.Out,">%s</option>",Txt_Public_open_educational_resource_OER_for_everyone);

	    HTM_SELECT_End ();
	   }
	 else		// I can not edit file properties
	    fprintf (Gbl.F.Out,"%s",
	             FileMetadata.IsPublic ? Txt_Public_open_educational_resource_OER_for_everyone :
					     Txt_Private_available_to_certain_users_identified);
	 HTM_TD_End ();

	 HTM_TR_End ();

	 /***** License *****/
	 HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"RM\"");
	 HTM_LABEL_Begin ("for=\"License\" class=\"%s\"",
		          The_ClassFormInBox[Gbl.Prefs.Theme]);
	 fprintf (Gbl.F.Out,"%s:",Txt_License);
	 HTM_LABEL_End ();
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"DAT LM\"");
	 if (ICanEdit)	// I can edit file properties
	   {
	    HTM_SELECT_Begin (false,
			      "id=\"License\" name=\"License\">");
	    for (License = 0;
		 License < Brw_NUM_LICENSES;
		 License++)
	      {
	       fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) License);
	       if (License == FileMetadata.License)
		  fprintf (Gbl.F.Out," selected=\"selected\"");
	       fprintf (Gbl.F.Out,">%s</option>",Txt_LICENSES[License]);
	      }
	    HTM_SELECT_End ();
	   }
	 else		// I can not edit file properties
	    fprintf (Gbl.F.Out,"%s",Txt_LICENSES[FileMetadata.License]);
	 HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Write my number of views *****/
	 if (Gbl.Usrs.Me.Logged)
	   {
	    HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
	    fprintf (Gbl.F.Out,"%s:",Txt_My_views);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"DAT LM\"");
	    fprintf (Gbl.F.Out,"%u",FileMetadata.NumMyViews);
	    HTM_TD_End ();

	    HTM_TR_End ();
	   }

	 /***** Write number of identificated views *****/
	 HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
	 fprintf (Gbl.F.Out,"%s:",Txt_Identified_views);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"DAT LM\"");
	 fprintf (Gbl.F.Out,"%u (%u %s)",
		  FileMetadata.NumViewsFromLoggedUsrs,
		  FileMetadata.NumLoggedUsrs,
		  (FileMetadata.NumLoggedUsrs == 1) ? Txt_user[Usr_SEX_UNKNOWN] :
			                              Txt_users[Usr_SEX_UNKNOWN]);
	 HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Write number of public views *****/
	 HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
	 fprintf (Gbl.F.Out,"%s:",Txt_Public_views);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"DAT LM\"");
	 fprintf (Gbl.F.Out,"%u",FileMetadata.NumPublicViews);
	 HTM_TD_End ();

	 HTM_TR_End ();

	 /***** End box *****/
	 if (ICanEdit)	// I can edit file properties
	   {
            /* End table, send button and end box */
	    Box_EndBoxTableWithButton (Btn_CONFIRM_BUTTON,Txt_Save_file_properties);

	    /* End form */
	    Frm_EndForm ();
	   }
	 else
            /* End table and box */
	    Box_EndBoxTable ();

	 /***** Mark possible notifications as seen *****/
	 switch (Gbl.FileBrowser.Type)
	   {
	    case Brw_SHOW_DOC_CRS:
	    case Brw_SHOW_DOC_GRP:
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_DOC_GRP:
	       Ntf_MarkNotifAsSeen (Ntf_EVENT_DOCUMENT_FILE,
				   FileMetadata.FilCod,Gbl.Hierarchy.Crs.CrsCod,
				   Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_TCH_GRP:
	       Ntf_MarkNotifAsSeen (Ntf_EVENT_TEACHERS_FILE,
				   FileMetadata.FilCod,Gbl.Hierarchy.Crs.CrsCod,
				   Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_SHR_GRP:
	       Ntf_MarkNotifAsSeen (Ntf_EVENT_SHARED_FILE,
				   FileMetadata.FilCod,Gbl.Hierarchy.Crs.CrsCod,
				   Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    case Brw_SHOW_MRK_CRS:
	    case Brw_SHOW_MRK_GRP:
	    case Brw_ADMI_MRK_CRS:
	    case Brw_ADMI_MRK_GRP:
	       Ntf_MarkNotifAsSeen (Ntf_EVENT_MARKS_FILE,
				   FileMetadata.FilCod,Gbl.Hierarchy.Crs.CrsCod,
				   Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    default:
	       break;
	   }
	}

      /***** Add paths until file to table of expanded folders *****/
      Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.FilFolLnk.Path);
     }
   else	// !ICanView
     {
      /***** Mark possible notifications about non visible file as removed *****/
      switch (Gbl.FileBrowser.Type)
	{
	 case Brw_SHOW_DOC_CRS:
	 case Brw_SHOW_DOC_GRP:
	 case Brw_ADMI_DOC_CRS:
	 case Brw_ADMI_DOC_GRP:
	    Ntf_MarkNotifAsRemoved (Ntf_EVENT_DOCUMENT_FILE,
				   FileMetadata.FilCod);
	    break;
	 case Brw_ADMI_TCH_CRS:
	 case Brw_ADMI_TCH_GRP:
	    Ntf_MarkNotifAsRemoved (Ntf_EVENT_TEACHERS_FILE,
				   FileMetadata.FilCod);
	    break;
	 case Brw_ADMI_SHR_CRS:
	 case Brw_ADMI_SHR_GRP:
	    Ntf_MarkNotifAsRemoved (Ntf_EVENT_SHARED_FILE,
				   FileMetadata.FilCod);
	    break;
	 case Brw_SHOW_MRK_CRS:
	 case Brw_SHOW_MRK_GRP:
	 case Brw_ADMI_MRK_CRS:
	 case Brw_ADMI_MRK_GRP:
	    Ntf_MarkNotifAsRemoved (Ntf_EVENT_MARKS_FILE,
				   FileMetadata.FilCod);
	    break;
	 default:
	    break;
	}

      Ale_ShowAlert (Ale_WARNING,Txt_The_file_of_folder_no_longer_exists_or_is_now_hidden);
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
   char URL[PATH_MAX + 1];
   bool Found;
   bool ICanView = false;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get file metadata *****/
   Brw_GetFileMetadataByPath (&FileMetadata);
   Found = Brw_GetFileTypeSizeAndDate (&FileMetadata);

   if (Found)
     {
      if (FileMetadata.FilCod <= 0)	// No entry for this file in database table of files
	 /* Add entry to the table of files/folders */
	 FileMetadata.FilCod = Brw_AddPathToDB (-1L,FileMetadata.FilFolLnk.Type,
	                                        Gbl.FileBrowser.FilFolLnk.Full,
	                                        false,Brw_LICENSE_DEFAULT);

      /***** Check if I can view this file.
	     It could be marked as hidden or in a hidden folder *****/
      ICanView = true;
      switch (Gbl.FileBrowser.Type)
	{
	 case Brw_SHOW_DOC_INS:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_INS_ADM)
	       ICanView = !Brw_CheckIfFileOrFolderIsHidden (&FileMetadata);
            break;
	 case Brw_SHOW_DOC_CTR:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_CTR_ADM)
	       ICanView = !Brw_CheckIfFileOrFolderIsHidden (&FileMetadata);
            break;
	 case Brw_SHOW_DOC_DEG:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_DEG_ADM)
	       ICanView = !Brw_CheckIfFileOrFolderIsHidden (&FileMetadata);
            break;
	 case Brw_SHOW_DOC_CRS:
	 case Brw_SHOW_DOC_GRP:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_TCH)
               ICanView = !Brw_CheckIfFileOrFolderIsHidden (&FileMetadata);
	    break;
	 default:
	    break;
	}
     }

   if (ICanView)
     {
      if (FileMetadata.FilFolLnk.Type == Brw_IS_FILE ||
	  FileMetadata.FilFolLnk.Type == Brw_IS_LINK)
	{
	 /***** Update number of views *****/
	 Brw_GetAndUpdateFileViews (&FileMetadata);

	 /***** Get link to download the file *****/
	 if (Gbl.FileBrowser.Type == Brw_SHOW_MRK_CRS ||
	     Gbl.FileBrowser.Type == Brw_SHOW_MRK_GRP)
	    URL[0] = '\0';
	 else
	    Brw_GetLinkToDownloadFile (Gbl.FileBrowser.FilFolLnk.Path,
				       Gbl.FileBrowser.FilFolLnk.Name,
				       URL);

	 /***** Mark possible notifications as seen *****/
	 switch (Gbl.FileBrowser.Type)
	   {
	    case Brw_SHOW_DOC_CRS:
	    case Brw_SHOW_DOC_GRP:
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_DOC_GRP:
	       Ntf_MarkNotifAsSeen (Ntf_EVENT_DOCUMENT_FILE,
				   FileMetadata.FilCod,Gbl.Hierarchy.Crs.CrsCod,
				   Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_TCH_GRP:
	       Ntf_MarkNotifAsSeen (Ntf_EVENT_TEACHERS_FILE,
				   FileMetadata.FilCod,Gbl.Hierarchy.Crs.CrsCod,
				   Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_SHR_GRP:
	       Ntf_MarkNotifAsSeen (Ntf_EVENT_SHARED_FILE,
				   FileMetadata.FilCod,Gbl.Hierarchy.Crs.CrsCod,
				   Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    case Brw_SHOW_MRK_CRS:
	    case Brw_SHOW_MRK_GRP:
	    case Brw_ADMI_MRK_CRS:
	    case Brw_ADMI_MRK_GRP:
	       Ntf_MarkNotifAsSeen (Ntf_EVENT_MARKS_FILE,
				   FileMetadata.FilCod,Gbl.Hierarchy.Crs.CrsCod,
				   Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    default:
	       break;
	   }
	}

      /***** Add paths until file to table of expanded folders *****/
      Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.FilFolLnk.Path);

      /***** Download the file *****/
      fprintf (stdout,"Location: %s\n\n",URL);
      // TODO: Put headers Content-type and Content-disposition:
      // See: http://stackoverflow.com/questions/381954/how-do-i-fix-firefox-trying-to-save-image-as-htm
      // http://elouai.com/force-download.php
      Gbl.Layout.HTMLStartWritten =
      Gbl.Layout.DivsEndWritten   =
      Gbl.Layout.HTMLEndWritten   = true;	// Don't write HTML at all
     }
   else	// !ICanView
     {
      /***** Mark possible notifications about non visible file as removed *****/
      switch (Gbl.FileBrowser.Type)
	{
	 case Brw_SHOW_DOC_CRS:
	 case Brw_SHOW_DOC_GRP:
	 case Brw_ADMI_DOC_CRS:
	 case Brw_ADMI_DOC_GRP:
	    Ntf_MarkNotifAsRemoved (Ntf_EVENT_DOCUMENT_FILE,
				    FileMetadata.FilCod);
	    break;
	 case Brw_ADMI_TCH_CRS:
	 case Brw_ADMI_TCH_GRP:
	    Ntf_MarkNotifAsRemoved (Ntf_EVENT_TEACHERS_FILE,
				    FileMetadata.FilCod);
	    break;
	 case Brw_ADMI_SHR_CRS:
	 case Brw_ADMI_SHR_GRP:
	    Ntf_MarkNotifAsRemoved (Ntf_EVENT_SHARED_FILE,
				    FileMetadata.FilCod);
	    break;
	 case Brw_SHOW_MRK_CRS:
	 case Brw_SHOW_MRK_GRP:
	 case Brw_ADMI_MRK_CRS:
	 case Brw_ADMI_MRK_GRP:
	    Ntf_MarkNotifAsRemoved (Ntf_EVENT_MARKS_FILE,
				    FileMetadata.FilCod);
	    break;
	 default:
	    break;
	}

      Ale_ShowAlert (Ale_WARNING,Txt_The_file_of_folder_no_longer_exists_or_is_now_hidden);

      /***** Show again the file browser *****/
      Brw_ShowAgainFileBrowserOrWorks ();
     }
  }

/*****************************************************************************/
/*********** Check if I have permission to change file metadata **************/
/*****************************************************************************/

static bool Brw_CheckIfICanEditFileMetadata (long IAmTheOwner)
  {
   switch (Gbl.Action.Act)	// Only in actions where edition is allowed
     {
      case ActReqDatAdmDocIns:		case ActChgDatAdmDocIns:
      case ActReqDatShaIns:		case ActChgDatShaIns:

      case ActReqDatAdmDocCtr:		case ActChgDatAdmDocCtr:
      case ActReqDatShaCtr:		case ActChgDatShaCtr:

      case ActReqDatAdmDocDeg:		case ActChgDatAdmDocDeg:
      case ActReqDatShaDeg:		case ActChgDatShaDeg:

      case ActReqDatAdmDocCrs:		case ActChgDatAdmDocCrs:
      case ActReqDatAdmDocGrp:		case ActChgDatAdmDocGrp:

      case ActReqDatTchCrs:		case ActChgDatTchCrs:
      case ActReqDatTchGrp:		case ActChgDatTchGrp:

      case ActReqDatShaCrs:		case ActChgDatShaCrs:
      case ActReqDatShaGrp:		case ActChgDatShaGrp:

      case ActReqDatAsgUsr:		case ActChgDatAsgUsr:
      case ActReqDatAsgCrs:		case ActChgDatAsgCrs:

      case ActReqDatWrkCrs:		case ActChgDatWrkCrs:
      case ActReqDatWrkUsr:		case ActChgDatWrkUsr:

      case ActReqDatBrf:		case ActChgDatBrf:
	 return IAmTheOwner;
      default:
         return false;
     }
  }

static bool Brw_CheckIfIAmOwnerOfFile (long PublisherUsrCod)
  {
   long ZoneUsrCod;

   if (Gbl.Usrs.Me.Logged)							// I am logged
     {
      if (PublisherUsrCod > 0)							// The file has publisher
	{
	 if (PublisherUsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)			// I am the publisher
	    return true;
	}
      else									// The file has no publisher
	{
	 ZoneUsrCod = Brw_GetZoneUsrCodForFiles ();
	 if ((ZoneUsrCod <= 0 && Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM) ||	// It's a zone without owner and I am a superuser (I may be the future owner)
	     ZoneUsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)				// I am the owner
	    return true;
	}
     }
   return false;
  }

/*****************************************************************************/
/****************** Write link to download a file or link ********************/
/*****************************************************************************/
// FileType can be Brw_IS_FILE or Brw_IS_LINK

static void Brw_WriteBigLinkToDownloadFile (const char *URL,
                                            struct FileMetadata *FileMetadata,
                                            const char *FileNameToShow)
  {
   extern const char *Txt_Check_marks_in_the_file;
   extern const char *Txt_Download;
   const char *Title;

   /***** On the screen a link will be shown to download the file *****/
   if (Gbl.FileBrowser.Type == Brw_SHOW_MRK_CRS ||
       Gbl.FileBrowser.Type == Brw_SHOW_MRK_GRP)
     {
      /* Form to see marks */
      Frm_StartForm (Gbl.FileBrowser.Type == Brw_SHOW_MRK_CRS ? ActSeeMyMrkCrs :
								ActSeeMyMrkGrp);
      Str_Copy (Gbl.FileBrowser.FilFolLnk.Path,FileMetadata->FilFolLnk.Path,
   	     PATH_MAX);
      Str_Copy (Gbl.FileBrowser.FilFolLnk.Name,FileMetadata->FilFolLnk.Name,
   	     NAME_MAX);
      Gbl.FileBrowser.FilFolLnk.Type = FileMetadata->FilFolLnk.Type;
      Brw_PutImplicitParamsFileBrowser ();

      /* Link begin */
      Frm_LinkFormSubmit (Txt_Check_marks_in_the_file,"FILENAME_TXT",NULL);
      Brw_PutIconFile (32,FileMetadata->FilFolLnk.Type,FileMetadata->FilFolLnk.Name);

      /* Name of the file of marks, link end and form end */
      fprintf (Gbl.F.Out,"&nbsp;%s&nbsp;",FileNameToShow);
      Ico_PutIcon ("grades32x32.gif",Txt_Check_marks_in_the_file,"ICO40x40");
      Frm_LinkFormEnd ();
      Frm_EndForm ();
     }
   else
     {
      Title = (FileMetadata->FilFolLnk.Type == Brw_IS_LINK) ? URL :	// If it's a link, show full URL in title
                                                        Txt_Download;

      /* Put anchor and filename */
      HTM_A_Begin ("href=\"%s\" class=\"FILENAME_TXT\" title=\"%s\" target=\"_blank\"",
	           URL,Title);
      Brw_PutIconFile (32,FileMetadata->FilFolLnk.Type,FileMetadata->FilFolLnk.Name);
      fprintf (Gbl.F.Out,"&nbsp;%s&nbsp;",FileNameToShow);
      Ico_PutIcon ("download.svg",Title,"ICO40x40");
      HTM_A_End ();
     }
  }

/*****************************************************************************/
/*********************** Write link to download a file ***********************/
/*****************************************************************************/

static void Brw_WriteSmallLinkToDownloadFile (const char *URL,
	                                      struct FileMetadata *FileMetadata,
                                              const char *FileNameToShow)
  {
   extern const char *Txt_Check_marks_in_the_file;

   /***** On the screen a link will be shown to download the file *****/
   if (Gbl.FileBrowser.Type == Brw_SHOW_MRK_CRS ||
       Gbl.FileBrowser.Type == Brw_SHOW_MRK_GRP)
     {
      /* Form to see marks */
      Frm_StartForm (Gbl.FileBrowser.Type == Brw_SHOW_MRK_CRS ? ActSeeMyMrkCrs :
								ActSeeMyMrkGrp);
      Str_Copy (Gbl.FileBrowser.FilFolLnk.Path,FileMetadata->FilFolLnk.Path,
   	     PATH_MAX);
      Str_Copy (Gbl.FileBrowser.FilFolLnk.Name,FileMetadata->FilFolLnk.Name,
   	     NAME_MAX);
      Gbl.FileBrowser.FilFolLnk.Type = FileMetadata->FilFolLnk.Type;
      Brw_PutImplicitParamsFileBrowser ();

      /* Link begin */
      Frm_LinkFormSubmit (Txt_Check_marks_in_the_file,"DAT",NULL);

      /* Name of the file of marks */
      fprintf (Gbl.F.Out,"%s",FileNameToShow);

      /* Link end and form end */
      Frm_LinkFormEnd ();
      Frm_EndForm ();
     }
   else
     {
      /* Put anchor and filename */
      HTM_A_Begin ("href=\"%s\" class=\"DAT\" title=\"%s\" target=\"_blank\"",
	           URL,FileNameToShow);
      fprintf (Gbl.F.Out,"%s",FileNameToShow);
      HTM_A_End ();
     }
  }

/*****************************************************************************/
/************************ Get link to download a file ************************/
/*****************************************************************************/

void Brw_GetLinkToDownloadFile (const char *PathInTree,const char *FileName,char *URL)
  {
   char FullPathIncludingFile[PATH_MAX + 1 + PATH_MAX + 1 + NAME_MAX + 1];
   FILE *FileURL;
   char URLWithSpaces[PATH_MAX + 1];

   /***** Construct absolute path to file in the private directory *****/
   snprintf (FullPathIncludingFile,sizeof (FullPathIncludingFile),
	     "%s/%s/%s",
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
      Brw_CreateTmpPublicLinkToPrivateFile (FullPathIncludingFile,FileName);

      /***** Create URL pointing to symbolic link *****/
      snprintf (URLWithSpaces,sizeof (URLWithSpaces),
	        "%s/%s/%s/%s",
	        Cfg_URL_FILE_BROWSER_TMP_PUBLIC,
	        Gbl.FileBrowser.TmpPubDir.L,
	        Gbl.FileBrowser.TmpPubDir.R,
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
   struct FileMetadata FileMetadata;
   bool IAmTheOwner;
   bool PublicFileBeforeEdition;
   bool PublicFileAfterEdition;
   Brw_License_t License;
   struct TL_Publication SocPub;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get file metadata from database *****/
   Brw_GetFileMetadataByPath (&FileMetadata);
   Brw_GetFileTypeSizeAndDate (&FileMetadata);

   /***** Check if I can change file metadata *****/
   IAmTheOwner = Brw_CheckIfIAmOwnerOfFile (FileMetadata.PublisherUsrCod);
   if (Brw_CheckIfICanEditFileMetadata (IAmTheOwner))
     {
      /* Check if the file was public before the edition */
      PublicFileBeforeEdition = FileMetadata.IsPublic;

      /***** Get the new file privacy and license from form *****/
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_ADMI_DOC_INS:
         case Brw_ADMI_SHR_INS:
         case Brw_ADMI_DOC_CTR:
         case Brw_ADMI_SHR_CTR:
         case Brw_ADMI_DOC_DEG:
         case Brw_ADMI_SHR_DEG:
         case Brw_ADMI_DOC_CRS:
         case Brw_ADMI_SHR_CRS:
            PublicFileAfterEdition = Brw_GetParamPublicFile ();
            License = Brw_GetParLicense ();
            break;
         case Brw_ADMI_DOC_GRP:
         case Brw_ADMI_TCH_CRS:
         case Brw_ADMI_TCH_GRP:
         case Brw_ADMI_SHR_GRP:
         case Brw_ADMI_ASG_USR:
         case Brw_ADMI_ASG_CRS:
         case Brw_ADMI_WRK_USR:
         case Brw_ADMI_WRK_CRS:
         case Brw_ADMI_DOC_PRJ:
         case Brw_ADMI_ASS_PRJ:
         case Brw_ADMI_BRF_USR:
            PublicFileAfterEdition = false;	// Files in these zones can not be public
            License = Brw_GetParLicense ();
            break;
         default:
            PublicFileAfterEdition = false;	// Files in other zones can not be public
            License = Brw_LICENSE_DEFAULT;
            break;
        }

      /***** Change file metadata *****/
      if (FileMetadata.FilCod > 0)	// Entry exists in database
         Brw_ChangeFilePublicInDB (Gbl.Usrs.Me.UsrDat.UsrCod,
                                   Gbl.FileBrowser.FilFolLnk.Full,
                                   PublicFileAfterEdition,License);
      else				// No entry in database
         FileMetadata.FilCod = Brw_AddPathToDB (Gbl.Usrs.Me.UsrDat.UsrCod,FileMetadata.FilFolLnk.Type,
                                                Gbl.FileBrowser.FilFolLnk.Full,
                                                PublicFileAfterEdition,License);

      /***** Remove the affected clipboards *****/
      Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
	                            Gbl.Usrs.Me.UsrDat.UsrCod,
	                            Gbl.Usrs.Other.UsrDat.UsrCod);

      /***** Insert file into public social activity *****/
      if (!PublicFileBeforeEdition &&
	   PublicFileAfterEdition)	// Only if file has changed from private to public
	 switch (Gbl.FileBrowser.Type)
	   {
	    case Brw_ADMI_DOC_INS:
	       TL_StoreAndPublishNote (TL_NOTE_INS_DOC_PUB_FILE,FileMetadata.FilCod,&SocPub);
	       break;
	    case Brw_ADMI_SHR_INS:
	       TL_StoreAndPublishNote (TL_NOTE_INS_SHA_PUB_FILE,FileMetadata.FilCod,&SocPub);
	       break;
	    case Brw_ADMI_DOC_CTR:
	       TL_StoreAndPublishNote (TL_NOTE_CTR_DOC_PUB_FILE,FileMetadata.FilCod,&SocPub);
	       break;
	    case Brw_ADMI_SHR_CTR:
	       TL_StoreAndPublishNote (TL_NOTE_CTR_SHA_PUB_FILE,FileMetadata.FilCod,&SocPub);
	       break;
	    case Brw_ADMI_DOC_DEG:
	       TL_StoreAndPublishNote (TL_NOTE_DEG_DOC_PUB_FILE,FileMetadata.FilCod,&SocPub);
	       break;
	    case Brw_ADMI_SHR_DEG:
	       TL_StoreAndPublishNote (TL_NOTE_DEG_SHA_PUB_FILE,FileMetadata.FilCod,&SocPub);
	       break;
	    case Brw_ADMI_DOC_CRS:
	       TL_StoreAndPublishNote (TL_NOTE_CRS_DOC_PUB_FILE,FileMetadata.FilCod,&SocPub);
	       break;
	    case Brw_ADMI_SHR_CRS:
	       TL_StoreAndPublishNote (TL_NOTE_CRS_SHA_PUB_FILE,FileMetadata.FilCod,&SocPub);
	       break;
	    default:
	       break;
	   }

      /***** Write sucess message *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_The_properties_of_file_X_have_been_saved,
                     Gbl.FileBrowser.FilFolLnk.Name);
     }
   else
      /***** Write error message and exit *****/
      Lay_NoPermissionExit ();

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/*********** Get parameter with public / private file from form *************/
/*****************************************************************************/

static bool Brw_GetParamPublicFile (void)
  {
   return Par_GetParToBool ("PublicFile");
  }

/*****************************************************************************/
/******************** Get parameter with file license ***********************/
/*****************************************************************************/

static Brw_License_t Brw_GetParLicense (void)
  {
   return (Brw_License_t)
	  Par_GetParToUnsignedLong ("License",
                                    0,
                                    Brw_NUM_LICENSES - 1,
                                    (unsigned long) Brw_LICENSE_UNKNOWN);
  }

/*****************************************************************************/
/*********************** Get file code using its path ************************/
/*****************************************************************************/
// Path is the full path in tree
// Example: descarga/folder/file.pdf

long Brw_GetFilCodByPath (const char *Path,bool OnlyIfPublic)
  {
   long Cod = Brw_GetCodForFiles ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFiles ();
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long FilCod;

   /***** Get code of a file from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get file code",
		       "SELECT FilCod FROM files"
		       " WHERE FileBrowser=%u AND Cod=%ld AND ZoneUsrCod=%ld"
		       " AND Path='%s'%s",
		       (unsigned) Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type],
		       Cod,ZoneUsrCod,
		       Path,
		       OnlyIfPublic ? " AND Public='Y'" :
				      ""))
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
   long Cod = Brw_GetCodForFiles ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFiles ();
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;

   /***** Get metadata of a file from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get file metadata",
		       "SELECT FilCod,FileBrowser,Cod,ZoneUsrCod,"
		       "PublisherUsrCod,FileType,Path,Hidden,Public,License"
		       " FROM files"
		       " WHERE FileBrowser=%u AND Cod=%ld AND ZoneUsrCod=%ld"
		       " AND Path='%s'",
		       (unsigned) Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type],
		       Cod,ZoneUsrCod,
		       Gbl.FileBrowser.FilFolLnk.Full))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get file code (row[0]) */
      FileMetadata->FilCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get file browser type in database (row[1]) */
      FileMetadata->FileBrowser = Brw_UNKNOWN;
      if (sscanf (row[1],"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < Brw_NUM_TYPES_FILE_BROWSER)
            FileMetadata->FileBrowser = (Brw_FileBrowser_t) UnsignedNum;

      /* Get institution/centre/degree/course/group code (row[2]) */
      FileMetadata->Cod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get the user's code of the owner of a zone of files (row[3]) */
      FileMetadata->ZoneUsrCod = Str_ConvertStrCodToLongCod (row[3]);

      /* Get publisher's code (row[4]) */
      FileMetadata->PublisherUsrCod = Str_ConvertStrCodToLongCod (row[4]);

      /* Get file type (row[5]) */
      FileMetadata->FilFolLnk.Type = Brw_IS_UNKNOWN;	// default
      if (sscanf (row[5],"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < Brw_NUM_FILE_TYPES)
	    FileMetadata->FilFolLnk.Type = (Brw_FileType_t) UnsignedNum;

      /* Get path (row[6]) */
      Str_Copy (FileMetadata->FilFolLnk.Full,row[6],
                PATH_MAX);
      Str_SplitFullPathIntoPathAndFileName (FileMetadata->FilFolLnk.Full,
					    FileMetadata->FilFolLnk.Path,
					    FileMetadata->FilFolLnk.Name);

      /* File is hidden? (row[7]) */
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_SHOW_DOC_INS:
         case Brw_ADMI_DOC_INS:
         case Brw_SHOW_DOC_CTR:
         case Brw_ADMI_DOC_CTR:
         case Brw_SHOW_DOC_DEG:
         case Brw_ADMI_DOC_DEG:
         case Brw_SHOW_DOC_CRS:
         case Brw_ADMI_DOC_CRS:
            FileMetadata->IsHidden = (row[7][0] == 'Y');
            break;
         default:
            FileMetadata->IsHidden = false;
            break;
        }

      /* Is a public file? (row[8]) */
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_SHOW_DOC_INS:
         case Brw_ADMI_DOC_INS:
         case Brw_ADMI_SHR_INS:
         case Brw_SHOW_DOC_CTR:
         case Brw_ADMI_DOC_CTR:
         case Brw_ADMI_SHR_CTR:
         case Brw_SHOW_DOC_DEG:
         case Brw_ADMI_DOC_DEG:
         case Brw_ADMI_SHR_DEG:
         case Brw_SHOW_DOC_CRS:
         case Brw_ADMI_DOC_CRS:
         case Brw_ADMI_SHR_CRS:
            FileMetadata->IsPublic = (row[8][0] == 'Y');
            break;
         default:
            FileMetadata->IsPublic = false;
            break;
        }

      /* Get license (row[9]) */
      FileMetadata->License = Brw_LICENSE_UNKNOWN;
      if (sscanf (row[9],"%u",&UnsignedNum) == 1)
         if (UnsignedNum < Brw_NUM_LICENSES)
            FileMetadata->License = (Brw_License_t) UnsignedNum;
     }
   else
     {
      FileMetadata->FilCod                      = -1L;
      FileMetadata->FileBrowser                 = Brw_UNKNOWN;
      FileMetadata->Cod                         = -1L;
      FileMetadata->ZoneUsrCod                  = -1L;
      FileMetadata->PublisherUsrCod             = -1L;
      FileMetadata->FilFolLnk.Type                    = Brw_IS_UNKNOWN;
      FileMetadata->FilFolLnk.Full[0]           = '\0';
      FileMetadata->FilFolLnk.Path[0] = '\0';
      FileMetadata->FilFolLnk.Name[0]            = '\0';
      FileMetadata->IsHidden                    = false;
      FileMetadata->IsPublic                    = false;
      FileMetadata->License                     = Brw_LICENSE_DEFAULT;
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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;

   /***** Get metadata of a file from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get file metadata",
		       "SELECT FilCod,FileBrowser,Cod,ZoneUsrCod,"
		       "PublisherUsrCod,FileType,Path,Hidden,Public,License"
		       " FROM files"
		       " WHERE FilCod=%ld",
		       FileMetadata->FilCod))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get file code (row[0]) */
      FileMetadata->FilCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get file browser type in database (row[1]) */
      FileMetadata->FileBrowser = Brw_UNKNOWN;
      if (sscanf (row[1],"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < Brw_NUM_TYPES_FILE_BROWSER)
            FileMetadata->FileBrowser = (Brw_FileBrowser_t) UnsignedNum;

      /* Get institution/centre/degree/course/group code (row[2]) */
      FileMetadata->Cod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get the user's code of the owner of a zone of files (row[3]) */
      FileMetadata->ZoneUsrCod = Str_ConvertStrCodToLongCod (row[3]);

      /* Get publisher's code (row[4]) */
      FileMetadata->PublisherUsrCod = Str_ConvertStrCodToLongCod (row[4]);

      /* Get file type (row[5]) */
      FileMetadata->FilFolLnk.Type = Brw_IS_UNKNOWN;	// default
      if (sscanf (row[5],"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < Brw_NUM_FILE_TYPES)
	    FileMetadata->FilFolLnk.Type = (Brw_FileType_t) UnsignedNum;

      /* Get path (row[6]) */
      Str_Copy (FileMetadata->FilFolLnk.Full,row[6],
                PATH_MAX);
      Str_SplitFullPathIntoPathAndFileName (FileMetadata->FilFolLnk.Full,
					    FileMetadata->FilFolLnk.Path,
					    FileMetadata->FilFolLnk.Name);

      /* Is a hidden file? (row[7]) */
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_SHOW_DOC_INS:
         case Brw_ADMI_DOC_INS:
         case Brw_SHOW_DOC_CTR:
         case Brw_ADMI_DOC_CTR:
         case Brw_SHOW_DOC_DEG:
         case Brw_ADMI_DOC_DEG:
         case Brw_SHOW_DOC_CRS:
         case Brw_ADMI_DOC_CRS:
            FileMetadata->IsHidden = (row[7][0] == 'Y');
            break;
         default:
            FileMetadata->IsHidden = false;
            break;
        }

      /* Is a public file? (row[8]) */
      switch (Gbl.FileBrowser.Type)
        {
         case Brw_SHOW_DOC_INS:
         case Brw_ADMI_DOC_INS:
         case Brw_ADMI_SHR_INS:
         case Brw_SHOW_DOC_CTR:
         case Brw_ADMI_DOC_CTR:
         case Brw_ADMI_SHR_CTR:
         case Brw_SHOW_DOC_DEG:
         case Brw_ADMI_DOC_DEG:
         case Brw_ADMI_SHR_DEG:
         case Brw_SHOW_DOC_CRS:
         case Brw_ADMI_DOC_CRS:
         case Brw_ADMI_SHR_CRS:
            FileMetadata->IsPublic = (row[8][0] == 'Y');
            break;
         default:
            FileMetadata->IsPublic = false;
            break;
        }

      /* Get license (row[9]) */
      FileMetadata->License = Brw_LICENSE_UNKNOWN;
      if (sscanf (row[9],"%u",&UnsignedNum) == 1)
         if (UnsignedNum < Brw_NUM_LICENSES)
            FileMetadata->License = (Brw_License_t) UnsignedNum;
     }
   else
     {
      FileMetadata->FilCod                      = -1L;
      FileMetadata->FileBrowser                 = Brw_UNKNOWN;
      FileMetadata->Cod                         = -1L;
      FileMetadata->ZoneUsrCod                  = -1L;
      FileMetadata->PublisherUsrCod             = -1L;
      FileMetadata->FilFolLnk.Type                    = Brw_IS_UNKNOWN;
      FileMetadata->FilFolLnk.Full[0]           = '\0';
      FileMetadata->FilFolLnk.Path[0] = '\0';
      FileMetadata->FilFolLnk.Name[0]            = '\0';
      FileMetadata->IsHidden                    = false;
      FileMetadata->IsPublic                    = false;
      FileMetadata->License                     = Brw_LICENSE_DEFAULT;
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
/********************** Get file type, size and date *************************/
/*****************************************************************************/
// Return true if file exists

bool Brw_GetFileTypeSizeAndDate (struct FileMetadata *FileMetadata)
  {
   char Path[PATH_MAX + 1 + PATH_MAX + 1];
   struct stat FileStatus;

   snprintf (Path,sizeof (Path),
	     "%s/%s",
	     Gbl.FileBrowser.Priv.PathAboveRootFolder,
	     FileMetadata->FilFolLnk.Full);
   if (lstat (Path,&FileStatus))	// On success ==> 0 is returned
     {
      // Error on lstat
      FileMetadata->FilFolLnk.Type = Brw_IS_UNKNOWN;
      FileMetadata->Size = (off_t) 0;
      FileMetadata->Time = (time_t) 0;
      return false;
     }
   else
     {
      if (S_ISDIR (FileStatus.st_mode))
	 FileMetadata->FilFolLnk.Type = Brw_IS_FOLDER;
      else if (S_ISREG (FileStatus.st_mode))
         FileMetadata->FilFolLnk.Type = Str_FileIs (FileMetadata->FilFolLnk.Full,"url") ? Brw_IS_LINK :
                                                                                          Brw_IS_FILE;
      else
	 FileMetadata->FilFolLnk.Type = Brw_IS_UNKNOWN;
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

      /***** Increment number of file views in my user's figures *****/
      if (Gbl.Usrs.Me.Logged)
         Prf_IncrementNumFileViewsUsr (Gbl.Usrs.Me.UsrDat.UsrCod);
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
/******************** Get number of file views from a user *******************/
/*****************************************************************************/

unsigned long Brw_GetNumFileViewsUsr (long UsrCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long FileViews;

   /***** Get number of filw views *****/
   if (DB_QuerySELECT (&mysql_res,"can not get number of file views",
	               "SELECT SUM(NumViews) FROM file_view WHERE UsrCod=%ld",
                       UsrCod))
     {
      /* Get number of file views */
      row = mysql_fetch_row (mysql_res);
      if (row[0])
	{
	 if (sscanf (row[0],"%lu",&FileViews) != 1)
	    FileViews = 0;
	}
      else
	 FileViews = 0;
     }
   else
      FileViews = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return FileViews;
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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get number total of views from logged users *****/
   if (DB_QuerySELECT (&mysql_res,"can not get number of views of a file"
				  " from logged users",
		       "SELECT COUNT(DISTINCT UsrCod),SUM(NumViews)"
		       " FROM file_view"
		       " WHERE FilCod=%ld AND UsrCod>0",
		       FileMetadata->FilCod))
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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get number of public views *****/
   if (DB_QuerySELECT (&mysql_res,"can not get number of public views"
				  " of a file",
		       "SELECT SUM(NumViews) FROM file_view"
		       " WHERE FilCod=%ld AND UsrCod<=0",
	               FileMetadata->FilCod))
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

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************** Get file views from me ***************************/
/*****************************************************************************/

static unsigned Brw_GetFileViewsFromMe (long FilCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumMyViews;

   /***** Get number of my views *****/
   if (DB_QuerySELECT (&mysql_res,"can not get your number of views of a file",
		       "SELECT NumViews FROM file_view"
		       " WHERE FilCod=%ld AND UsrCod=%ld",
		       FilCod,Gbl.Usrs.Me.UsrDat.UsrCod))
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
   if (NumViews)
      /* Update number of views in database */
      DB_QueryUPDATE ("can not update number of views of a file",
		      "UPDATE file_view SET NumViews=NumViews+1"
		      " WHERE FilCod=%ld AND UsrCod=%ld",
	              FilCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   else	// NumViews == 0
      /* Insert number of views in database */
      DB_QueryINSERT ("can not insert number of views of a file",
		      "INSERT INTO file_view"
		      " (FilCod,UsrCod,NumViews)"
		      " VALUES"
		      " (%ld,%ld,1)",
		      FilCod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*********** Check if a folder contains file(s) marked as public *************/
/*****************************************************************************/

static bool Brw_GetIfFolderHasPublicFiles (const char Path[PATH_MAX + 1])
  {
   long Cod = Brw_GetCodForFiles ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFiles ();

   /***** Get if a file or folder is public from database *****/
   return (DB_QueryCOUNT ("can not check if a folder contains public files",
			  "SELECT COUNT(*) FROM files"
			  " WHERE FileBrowser=%u AND Cod=%ld AND ZoneUsrCod=%ld"
			  " AND Path LIKE '%s/%%' AND Public='Y'",
			  (unsigned) Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type],
			  Cod,ZoneUsrCod,
			  Path) != 0);
  }

/*****************************************************************************/
/*********************** Get number of files from a user *********************/
/*****************************************************************************/

unsigned Brw_GetNumFilesUsr (long UsrCod)
  {
   /***** Get current number of files published by a user from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of files from a user",
			     "SELECT COUNT(*) FROM files"
			     " WHERE PublisherUsrCod=%ld"
			     " AND FileType IN (%u,%u)",
			     UsrCod,
			     (unsigned) Brw_IS_FILE,
			     (unsigned) Brw_IS_UNKNOWN);	// Unknown entries are counted as files
  }

/*****************************************************************************/
/******************* Get number of public files from a user ******************/
/*****************************************************************************/

unsigned Brw_GetNumPublicFilesUsr (long UsrCod)
  {
   /***** Get current number of public files published by a user from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of public files from a user",
			     "SELECT COUNT(*) FROM files"
			     " WHERE PublisherUsrCod=%ld"
			     " AND FileType IN (%u,%u)"
			     " AND Public='Y'",
			     UsrCod,
			     (unsigned) Brw_IS_FILE,
			     (unsigned) Brw_IS_UNKNOWN);	// Unknown entries are counted as files
  }

/*****************************************************************************/
/***************** Change hiddeness of file in the database ******************/
/*****************************************************************************/

static void Brw_ChangeFileOrFolderHiddenInDB (const char Path[PATH_MAX + 1],bool IsHidden)
  {
   long Cod = Brw_GetCodForFiles ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFiles ();

   /***** Mark file as hidden in database *****/
   DB_QueryUPDATE ("can not change status of a file in database",
		   "UPDATE files SET Hidden='%c'"
		   " WHERE FileBrowser=%u AND Cod=%ld AND ZoneUsrCod=%ld"
		   " AND Path='%s'",
	           IsHidden ? 'Y' :
			      'N',
	           (unsigned) Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type],
	           Cod,ZoneUsrCod,
	           Path);
  }

/*****************************************************************************/
/******* Change publisher, public and license of file in the database ********/
/*****************************************************************************/

static void Brw_ChangeFilePublicInDB (long PublisherUsrCod,const char *Path,
                                      bool IsPublic,Brw_License_t License)
  {
   long Cod = Brw_GetCodForFiles ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFiles ();

   /***** Change publisher, public and license of file in database *****/
   DB_QueryUPDATE ("can not change metadata of a file in database",
		   "UPDATE files SET PublisherUsrCod=%ld,Public='%c',License=%u"
		   " WHERE FileBrowser=%u AND Cod=%ld AND ZoneUsrCod=%ld"
		   " AND Path='%s'",
	           PublisherUsrCod,
	           IsPublic ? 'Y' :
			      'N',
	           (unsigned) License,
	           (unsigned) Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type],
	           Cod,ZoneUsrCod,
	           Path);
  }

/*****************************************************************************/
/**** Get code of institution, degree, course, group for expanded folders ****/
/*****************************************************************************/

long Brw_GetCodForFiles (void)
  {
   switch (Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type])
     {
      case Brw_ADMI_DOC_INS:
      case Brw_ADMI_SHR_INS:
	 return Gbl.Hierarchy.Ins.InsCod;
      case Brw_ADMI_DOC_CTR:
      case Brw_ADMI_SHR_CTR:
	 return Gbl.Hierarchy.Ctr.CtrCod;
      case Brw_ADMI_DOC_DEG:
      case Brw_ADMI_SHR_DEG:
	 return Gbl.Hierarchy.Deg.DegCod;
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_TCH_CRS:
      case Brw_ADMI_SHR_CRS:
      case Brw_ADMI_ASG_USR:
      case Brw_ADMI_WRK_USR:
      case Brw_ADMI_MRK_CRS:
	 return Gbl.Hierarchy.Crs.CrsCod;
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_GRP:
      case Brw_ADMI_SHR_GRP:
      case Brw_ADMI_MRK_GRP:
	 return Gbl.Crs.Grps.GrpCod;
      case Brw_ADMI_DOC_PRJ:
      case Brw_ADMI_ASS_PRJ:
	 return Gbl.Prjs.PrjCod;
      default:
         return -1L;
     }
  }

/*****************************************************************************/
/******** Get code of user in assignment / works for expanded folders ********/
/*****************************************************************************/

static long Brw_GetZoneUsrCodForFiles (void)
  {
   if (Brw_GetIfBriefcaseFileBrowser ())	// Briefcase
      return Gbl.Usrs.Me.UsrDat.UsrCod;

   if (Brw_GetIfUsrAssigWorksFileBrowser ())	// My assignments or works
      return Gbl.Usrs.Me.UsrDat.UsrCod;

   if (Brw_GetIfCrsAssigWorksFileBrowser ())	// Course assignments or works
      return Gbl.Usrs.Other.UsrDat.UsrCod;

   return -1L;
  }

/*****************************************************************************/
/******** Get code of user in assignment / works for expanded folders ********/
/*****************************************************************************/

void Brw_GetCrsGrpFromFileMetadata (Brw_FileBrowser_t FileBrowser,long Cod,
                                    long *InsCod,
                                    long *CtrCod,
                                    long *DegCod,
                                    long *CrsCod,
                                    long *GrpCod)
  {
   struct Centre Ctr;
   struct Degree Deg;
   struct Course Crs;
   struct GroupData GrpDat;

   switch (FileBrowser)
     {
      case Brw_ADMI_DOC_INS:
      case Brw_ADMI_SHR_INS:
	 /* Cod stores the institution code */
	 *GrpCod = -1L;
	 *CrsCod = -1L;
	 *DegCod = -1L;
	 *CtrCod = -1L;
	 *InsCod = Cod;
         break;
      case Brw_ADMI_DOC_CTR:
      case Brw_ADMI_SHR_CTR:
	 /* Cod stores the centre code */
	 *GrpCod = -1L;
	 *CrsCod = -1L;
	 *DegCod = -1L;
	 *CtrCod = Ctr.CtrCod = Cod;
	 Ctr_GetDataOfCentreByCod (&Ctr);
	 *InsCod = Ctr.InsCod;
         break;
      case Brw_ADMI_DOC_DEG:
      case Brw_ADMI_SHR_DEG:
	 /* Cod stores the degree code */
	 *GrpCod = -1L;
	 *CrsCod = -1L;
	 *DegCod = Deg.DegCod = Cod;
	 Deg_GetDataOfDegreeByCod (&Deg);
	 *CtrCod = Ctr.CtrCod = Deg.CtrCod;
	 Ctr_GetDataOfCentreByCod (&Ctr);
	 *InsCod = Ctr.InsCod;
         break;
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_TCH_CRS:
      case Brw_ADMI_SHR_CRS:
      case Brw_ADMI_ASG_USR:
      case Brw_ADMI_WRK_USR:
      case Brw_ADMI_MRK_CRS:
	 /* Cod stores the course code */
	 *GrpCod = -1L;
	 *CrsCod = Crs.CrsCod = Cod;
	 Crs_GetDataOfCourseByCod (&Crs);
	 *DegCod = Deg.DegCod = Crs.DegCod;
	 Deg_GetDataOfDegreeByCod (&Deg);
	 *CtrCod = Ctr.CtrCod = Deg.CtrCod;
	 Ctr_GetDataOfCentreByCod (&Ctr);
	 *InsCod = Ctr.InsCod;
	 break;
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_GRP:
      case Brw_ADMI_SHR_GRP:
      case Brw_ADMI_MRK_GRP:
	 /* Cod stores the group code */
	 *GrpCod = GrpDat.GrpCod = Cod;
	 Grp_GetDataOfGroupByCod (&GrpDat);
	 *CrsCod = Crs.CrsCod = GrpDat.CrsCod;
	 Crs_GetDataOfCourseByCod (&Crs);
	 *DegCod = Deg.DegCod = Crs.DegCod;
	 Deg_GetDataOfDegreeByCod (&Deg);
	 *CtrCod = Ctr.CtrCod = Deg.CtrCod;
	 Ctr_GetDataOfCentreByCod (&Ctr);
	 *InsCod = Ctr.InsCod;
	 break;
      case Brw_ADMI_DOC_PRJ:
      case Brw_ADMI_ASS_PRJ:
	 /* Cod stores the project code */
	 *GrpCod = -1L;
	 *CrsCod = Crs.CrsCod = Prj_GetCourseOfProject (Cod);
	 Crs_GetDataOfCourseByCod (&Crs);
	 *DegCod = Deg.DegCod = Crs.DegCod;
	 Deg_GetDataOfDegreeByCod (&Deg);
	 *CtrCod = Ctr.CtrCod = Deg.CtrCod;
	 Ctr_GetDataOfCentreByCod (&Ctr);
	 *InsCod = Ctr.InsCod;
	 break;
      default:
	 *GrpCod = -1L;
	 *CrsCod = -1L;
	 *DegCod = -1L;
	 *CtrCod = -1L;
	 *InsCod = -1L;
	 break;
     }
  }

/*****************************************************************************/
/**************** Add a path of file/folder to the database ******************/
/*****************************************************************************/

long Brw_AddPathToDB (long PublisherUsrCod,Brw_FileType_t FileType,
                      const char *FullPathInTree,bool IsPublic,Brw_License_t License)
  {
   long Cod = Brw_GetCodForFiles ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFiles ();

   /***** Add path to the database *****/
   return
   DB_QueryINSERTandReturnCode ("can not add path to database",
				"INSERT INTO files"
				" (FileBrowser,Cod,ZoneUsrCod,"
				"PublisherUsrCod,FileType,Path,Hidden,Public,License)"
				" VALUES"
				" (%u,%ld,%ld,"
				"%ld,%u,'%s','N','%c',%u)",
				(unsigned) Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type],
				Cod,ZoneUsrCod,
				PublisherUsrCod,
				(unsigned) FileType,
				FullPathInTree,
				IsPublic ? 'Y' :
					   'N',
				(unsigned) License);
  }

/*****************************************************************************/
/**************** Remove a file or folder from the database ******************/
/*****************************************************************************/

static void Brw_RemoveOneFileOrFolderFromDB (const char Path[PATH_MAX + 1])
  {
   long Cod = Brw_GetCodForFiles ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFiles ();
   Brw_FileBrowser_t FileBrowser = Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type];

   /***** Set possible notifications as removed.
          Set possible social note as unavailable.
          Important: do this before removing from files *****/
   Ntf_MarkNotifOneFileAsRemoved (Path);
   TL_MarkNoteOneFileAsUnavailable (Path);

   /***** Remove from database the entries that store the marks properties *****/
   if (FileBrowser == Brw_ADMI_MRK_CRS ||
       FileBrowser == Brw_ADMI_MRK_GRP)
      DB_QueryDELETE ("can not remove properties of marks from database",
		      "DELETE FROM marks_properties"
		      " USING files,marks_properties"
		      " WHERE files.FileBrowser=%u AND files.Cod=%ld"
		      " AND files.Path='%s'"
		      " AND files.FilCod=marks_properties.FilCod",
	              (unsigned) FileBrowser,Cod,Path);

   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views from database",
		  "DELETE FROM file_view USING file_view,files"
		  " WHERE files.FileBrowser=%u AND files.Cod=%ld"
		  " AND files.ZoneUsrCod=%ld"
		  " AND files.Path='%s'"
		  " AND files.FilCod=file_view.FilCod",
	          (unsigned) FileBrowser,Cod,ZoneUsrCod,Path);

   /***** Remove from database the entry that stores the data of a file *****/
   DB_QueryDELETE ("can not remove path from database",
		   "DELETE FROM files"
		   " WHERE FileBrowser=%u AND Cod=%ld AND ZoneUsrCod=%ld"
		   " AND Path='%s'",
	           (unsigned) FileBrowser,Cod,ZoneUsrCod,Path);
  }

/*****************************************************************************/
/************** Remove children of a folder from the database ****************/
/*****************************************************************************/

static void Brw_RemoveChildrenOfFolderFromDB (const char Path[PATH_MAX + 1])
  {
   long Cod = Brw_GetCodForFiles ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFiles ();
   Brw_FileBrowser_t FileBrowser = Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type];

   /***** Set possible notifications as removed.
          Set possible social notes as unavailable.
          Important: do this before removing from files *****/
   Ntf_MarkNotifChildrenOfFolderAsRemoved (Path);
   TL_MarkNotesChildrenOfFolderAsUnavailable (Path);

   /***** Remove from database the entries that store the marks properties *****/
   if (FileBrowser == Brw_ADMI_MRK_CRS ||
       FileBrowser == Brw_ADMI_MRK_GRP)
      DB_QueryDELETE ("can not remove properties of marks from database",
		      "DELETE FROM marks_properties"
		      " USING files,marks_properties"
		      " WHERE files.FileBrowser=%u AND files.Cod=%ld"
		      " AND files.Path LIKE '%s/%%'"
		      " AND files.FilCod=marks_properties.FilCod",
	              (unsigned) FileBrowser,Cod,Path);

   /***** Remove from database the entries that store the file views *****/
   DB_QueryDELETE ("can not remove file views from database",
		  "DELETE FROM file_view USING file_view,files"
		  " WHERE files.FileBrowser=%u AND files.Cod=%ld"
		  " AND files.ZoneUsrCod=%ld"
		  " AND files.Path LIKE '%s/%%'"
		  " AND files.FilCod=file_view.FilCod",
                  (unsigned) FileBrowser,Cod,ZoneUsrCod,Path);

   /***** Remove from database the entries that store the data of files *****/
   DB_QueryDELETE ("can not remove paths from database",
		   "DELETE FROM files"
		   " WHERE FileBrowser=%u AND Cod=%ld AND ZoneUsrCod=%ld"
		   " AND Path LIKE '%s/%%'",
                   (unsigned) FileBrowser,Cod,ZoneUsrCod,Path);
  }

/*****************************************************************************/
/*************** Rename a file or folder in table of files *******************/
/*****************************************************************************/

static void Brw_RenameOneFolderInDB (const char OldPath[PATH_MAX + 1],
                                     const char NewPath[PATH_MAX + 1])
  {
   long Cod = Brw_GetCodForFiles ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFiles ();

   /***** Update file or folder in table of common files *****/
   DB_QueryUPDATE ("can not update folder name in a common zone",
		   "UPDATE files SET Path='%s'"
		   " WHERE FileBrowser=%u AND Cod=%ld AND ZoneUsrCod=%ld AND Path='%s'",
		   NewPath,
		   (unsigned) Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type],
		   Cod,ZoneUsrCod,
		   OldPath);
  }

/*****************************************************************************/
/************** Rename children of a folder in table of files ****************/
/*****************************************************************************/

static void Brw_RenameChildrenFilesOrFoldersInDB (const char OldPath[PATH_MAX + 1],
                                                  const char NewPath[PATH_MAX + 1])
  {
   long Cod = Brw_GetCodForFiles ();
   long ZoneUsrCod = Brw_GetZoneUsrCodForFiles ();
   unsigned StartFinalSubpathNotChanged = strlen (OldPath) + 2;

   /***** Update children of a folder in table of files *****/
   DB_QueryUPDATE ("can not rename file or folder names in a common zone",
		   "UPDATE files SET Path=CONCAT('%s','/',SUBSTRING(Path,%u))"
		   " WHERE FileBrowser=%u AND Cod=%ld AND ZoneUsrCod=%ld"
		   " AND Path LIKE '%s/%%'",
	           NewPath,StartFinalSubpathNotChanged,
	           (unsigned) Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type],
	           Cod,ZoneUsrCod,
	           OldPath);
  }

/*****************************************************************************/
/********** Check if I have permission to modify a file or folder ************/
/*****************************************************************************/

static bool Brw_CheckIfICanEditFileOrFolder (unsigned Level)
  {
   /***** Level 0 (root folder) can not be removed/renamed *****/
   if (Level == 0)
      return false;

   /***** I must be student or a superior role to edit *****/
   if (Gbl.Usrs.Me.Role.Max < Rol_STD)
      return false;

   /***** Set depending on browser, level, logged role... *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_ADMI_DOC_CRS:
         return Gbl.Usrs.Me.Role.Logged >= Rol_TCH;
      case Brw_ADMI_DOC_GRP:
	 if (Gbl.Usrs.Me.Role.Logged == Rol_TCH)		// A teacher...
							// ...can edit only if he/she belongs to group
	    return Grp_GetIfIBelongToGrp (Gbl.Crs.Grps.GrpCod);
	 // An administrator can edit
         return (Gbl.Usrs.Me.Role.Logged > Rol_TCH);
      case Brw_ADMI_TCH_CRS:
      case Brw_ADMI_TCH_GRP:
         // Check if I am the publisher of the file/folder
         return Brw_CheckIfICanModifyPrivateFileOrFolder ();
      case Brw_ADMI_SHR_CRS:
      case Brw_ADMI_SHR_GRP:
         // Check if I am the publisher of the file/folder
         return Brw_CheckIfICanModifySharedFileOrFolder ();
      case Brw_ADMI_ASG_USR:
      case Brw_ADMI_ASG_CRS:
	 if (Gbl.FileBrowser.Asg.AsgCod <= 0)	// If folder does not correspond to any assignment
	    return true;			// Folder can be removed or renamed

	 if (Gbl.FileBrowser.Asg.Hidden)	// If assignment is hidden
	    return false;			// Do not edit anything in hidden assigments

	 if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER && // The main folder of an assignment
             Level == 1)
	    return false;			// Do not remove / rename main folder of assigment

	 if (!Gbl.FileBrowser.Asg.IBelongToCrsOrGrps)	// If I do not belong to course / groups of this assignment
	    return false; 				// I can not edit this assignment

	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_STD:			// Students...
	    case Rol_NET:			// ...and non-editing teachers...
	       return Gbl.FileBrowser.Asg.Open;	// ...can edit inside open assignments
	    case Rol_TCH:			// Teachers...
	       return true;			// ...can edit inside open or closed assignments
	    default:
	       break;
	   }
	 return false;
      case Brw_ADMI_DOC_PRJ:
         return Brw_CheckIfICanModifyPrjDocFileOrFolder ();
      case Brw_ADMI_ASS_PRJ:
         return Brw_CheckIfICanModifyPrjAssFileOrFolder ();
      default:
         return Brw_FileBrowserIsEditable[Gbl.FileBrowser.Type];
     }
   return false;
  }

/*****************************************************************************/
/**** Check if I have permission to create a file or folder into a folder ****/
/*****************************************************************************/

static bool Brw_CheckIfICanCreateIntoFolder (unsigned Level)
  {
   /***** If not in a folder... *****/
   if (Gbl.FileBrowser.FilFolLnk.Type != Brw_IS_FOLDER)
      return false;

   /***** I must be student, teacher, admin or superuser to edit *****/
   if (Gbl.Usrs.Me.Role.Max < Rol_STD)
      return false;

   /***** If maximum level is reached, I can not create/paste *****/
   if (Level >= Brw_MAX_DIR_LEVELS)
      return false;

   /***** Have I permission to create/paste a new file or folder into the folder? *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_ADMI_DOC_CRS:
         return Gbl.Usrs.Me.Role.Logged >= Rol_TCH;
      case Brw_ADMI_DOC_GRP:
	 if (Gbl.Usrs.Me.Role.Logged == Rol_TCH)		// A teacher
							// ...can create/paste only if he/she belongs to group
	    return Grp_GetIfIBelongToGrp (Gbl.Crs.Grps.GrpCod);
	 // An administrator can create/paste
         return (Gbl.Usrs.Me.Role.Logged > Rol_TCH);
      case Brw_ADMI_TCH_CRS:
         return Gbl.Usrs.Me.Role.Logged >= Rol_NET;
      case Brw_ADMI_TCH_GRP:
	 if (Gbl.Usrs.Me.Role.Logged == Rol_NET ||	// A non-editing teacher...
	     Gbl.Usrs.Me.Role.Logged == Rol_TCH)		// ...or a teacher
							// ...can create/paste only if he/she belongs to group
	    return Grp_GetIfIBelongToGrp (Gbl.Crs.Grps.GrpCod);
	 // An administrator can create/paste
         return (Gbl.Usrs.Me.Role.Logged > Rol_TCH);
      case Brw_ADMI_SHR_CRS:
         return Gbl.Usrs.Me.Role.Logged >= Rol_STD;
      case Brw_ADMI_SHR_GRP:
	 if (Gbl.Usrs.Me.Role.Logged >= Rol_STD &&	// A student, non-editing teacher...
	     Gbl.Usrs.Me.Role.Logged <= Rol_TCH)		// ...or a teacher
							// ...can create/paste only if he/she belongs to group
	    return Grp_GetIfIBelongToGrp (Gbl.Crs.Grps.GrpCod);
	 // An administrator can create/paste
         return Gbl.Usrs.Me.Role.Logged >= Rol_STD;
      case Brw_ADMI_ASG_USR:
      case Brw_ADMI_ASG_CRS:
	 if (Level == 0)	// If root folder
	    return false;	// Folders of assigments (level 1)
				// can only be created automatically

	 if (Gbl.FileBrowser.Asg.AsgCod <= 0)	// If folder does not correspond to any assignment
	    return false;			// Do not create anything out of assignments

	 if (Gbl.FileBrowser.Asg.Hidden)	// If assignment is hidden
	    return false;			// Do not create anything in hidden assigments

	 if (!Gbl.FileBrowser.Asg.IBelongToCrsOrGrps)	// If I do not belong to course / groups of this assignment
	    return false; 				// I can not create anything inside this assignment

	 switch (Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs.Role)
	   {
	    case Rol_STD:			// Students...
	    case Rol_NET:			// ...and non-editing teachers...
	       return Gbl.FileBrowser.Asg.Open;	// ...can create inside open assignments
	    case Rol_TCH:			// Teachers...
	       return true;			// ...can create inside open or closed assignments
	    default:
	       break;
	   }
	 return false;
      default:
         return Brw_FileBrowserIsEditable[Gbl.FileBrowser.Type];
     }
   return false;
  }

/*****************************************************************************/
/********** Check if I have permission to modify a file or folder ************/
/********** in the current shared or private zone                 ************/
/*****************************************************************************/
// Returns true if I can remove or rename Gbl.FileBrowser.FilFolLnk.Full, and false if I have not permission
// I can remove or rename a file if I am the publisher
// I can remove or rename a folder if I am the unique publisher of all the files and folders in the subtree starting there

static bool Brw_CheckIfICanModifySharedFileOrFolder (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:	// If I am a student or a non-editing teacher...
      case Rol_NET:	// ...I can modify the file/folder if I am the publisher
         return (Gbl.Usrs.Me.UsrDat.UsrCod == Brw_GetPublisherOfSubtree ());	// Am I the publisher of subtree?
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
   return false;
  }

static bool Brw_CheckIfICanModifyPrivateFileOrFolder (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:	// If I am a student or a non-editing teacher...
			// ...I can modify the file/folder if I am the publisher
         return (Gbl.Usrs.Me.UsrDat.UsrCod == Brw_GetPublisherOfSubtree ());	// Am I the publisher of subtree?
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
   return false;
  }

/*****************************************************************************/
/******** Check if I have permission to view project documents zone **********/
/*****************************************************************************/

static bool Brw_CheckIfICanViewProjectDocuments (unsigned MyRolesInProject)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 return (MyRolesInProject != 0);	// Am I a member?
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
   return false;
  }

/*****************************************************************************/
/******** Check if I have permission to view project assessment zone *********/
/*****************************************************************************/

static bool Brw_CheckIfICanViewProjectAssessment (unsigned MyRolesInProject)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 return ((MyRolesInProject & (1 << Prj_ROLE_TUT |		// Tutor...
	                              1 << Prj_ROLE_EVL)) != 0);	// ...or evaluator
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
   return false;
  }

/*****************************************************************************/
/********** Check if I have permission to modify a file or folder ************/
/********** in the current project documents zone                 ************/
/*****************************************************************************/
// Returns true if I can remove or rename Gbl.FileBrowser.FilFolLnk.Full, and false if I have not permission
// I can remove or rename a file if I am the publisher
// I can remove or rename a folder if I am the unique publisher of all the files and folders in the subtree starting there

static bool Brw_CheckIfICanModifyPrjDocFileOrFolder (void)
  {
   unsigned MyRolesInProject;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 MyRolesInProject = Prj_GetMyRolesInProject (Gbl.Prjs.PrjCod);
	 if (MyRolesInProject)	// I am a member
            return (Gbl.Usrs.Me.UsrDat.UsrCod == Brw_GetPublisherOfSubtree ());	// Am I the publisher of subtree?
	 return false;
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
   return false;
  }

/*****************************************************************************/
/********** Check if I have permission to modify a file or folder ************/
/********** in the current project assessment zone                ************/
/*****************************************************************************/
// Returns true if I can remove or rename Gbl.FileBrowser.FilFolLnk.Full, and false if I have not permission
// I can remove or rename a file if I am the publisher
// I can remove or rename a folder if I am the unique publisher of all the files and folders in the subtree starting there

static bool Brw_CheckIfICanModifyPrjAssFileOrFolder (void)
  {
   unsigned MyRolesInProject;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 MyRolesInProject = Prj_GetMyRolesInProject (Gbl.Prjs.PrjCod);
	 if ((MyRolesInProject & (1 << Prj_ROLE_TUT |	// Tutor...
	                          1 << Prj_ROLE_EVL)))	// ...or evaluator
            return (Gbl.Usrs.Me.UsrDat.UsrCod == Brw_GetPublisherOfSubtree ());	// Am I the publisher of subtree?
	 return false;
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
   return false;
  }

/*****************************************************************************/
/************************ Get the publisher of a subtree *********************/
/*****************************************************************************/

static long Brw_GetPublisherOfSubtree (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   long PublisherUsrCod;
   long Cod = Brw_GetCodForFiles ();

   /***** Get all common files that are equal to full path (including filename)
	  or that are under that full path from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get publishers of files",
			     "SELECT DISTINCT(PublisherUsrCod) FROM files"
			     " WHERE FileBrowser=%u AND Cod=%ld"
			     " AND (Path='%s' OR Path LIKE '%s/%%')",
			     (unsigned) Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type],
			     Cod,
			     Gbl.FileBrowser.FilFolLnk.Full,
			     Gbl.FileBrowser.FilFolLnk.Full);

   /***** Check all common files that are equal to full path (including filename)
	  or that are under that full path *****/
   if (NumRows == 1)	// Get the publisher of the file(s)
     {
      row = mysql_fetch_row (mysql_res);
      PublisherUsrCod = Str_ConvertStrCodToLongCod (row[0]);
     }
   else
      PublisherUsrCod = -1L;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return PublisherUsrCod;
  }

/*****************************************************************************/
/************* Remove common zones of all the groups of a type ***************/
/*****************************************************************************/

void Brw_RemoveZonesOfGroupsOfType (long GrpTypCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;
   long GrpCod;

   /***** Query database *****/
   if ((NumRows = Grp_GetGrpsOfType (GrpTypCod,&mysql_res)))	// If there exists groups...
      for (NumRow = 0;
	   NumRow < NumRows;
	   NumRow++)
        {
	 /* Get next group */
	 row = mysql_fetch_row (mysql_res);

         /* Group code is in row[0] */
         if (sscanf (row[0],"%ld",&GrpCod) != 1)
            Lay_ShowErrorAndExit ("Wrong group code.");

         /* Remove file zones of this group */
         Brw_RemoveGrpZones (Gbl.Hierarchy.Crs.CrsCod,GrpCod);
	}

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*********************** Remove file zones of a group ************************/
/*****************************************************************************/

void Brw_RemoveGrpZones (long CrsCod,long GrpCod)
  {
   char PathGrpFileZones[PATH_MAX + 1];

   /***** Set notifications about files in this group zone as removed *****/
   Ntf_MarkNotifFilesInGroupAsRemoved (GrpCod);

   /***** Remove files in the group from database *****/
   Brw_RemoveGrpFilesFromDB (GrpCod);

   /***** Remove group zones *****/
   snprintf (PathGrpFileZones,sizeof (PathGrpFileZones),
	     "%s/%ld/grp/%ld",
             Cfg_PATH_CRS_PRIVATE,CrsCod,GrpCod);
   Fil_RemoveTree (PathGrpFileZones);
  }

/*****************************************************************************/
/***************** Remove the works of a user in a course ********************/
/*****************************************************************************/

void Brw_RemoveUsrWorksInCrs (struct UsrData *UsrDat,struct Course *Crs)
  {
   char PathUsrInCrs[PATH_MAX + 1];

   /***** Remove user's works in the course from database *****/
   Brw_RemoveWrkFilesFromDB (Crs->CrsCod,UsrDat->UsrCod);

   /***** Remove the folder for this user inside the course *****/
   snprintf (PathUsrInCrs,sizeof (PathUsrInCrs),
	     "%s/%ld/usr/%02u/%ld",
             Cfg_PATH_CRS_PRIVATE,Crs->CrsCod,
             (unsigned) (UsrDat->UsrCod % 100),UsrDat->UsrCod);
   Fil_RemoveTree (PathUsrInCrs);
   // If this was the last user in his/her subfolder ==> the subfolder will be empty
  }

/*****************************************************************************/
/************* Remove the works of a user in all of his courses **************/
/*****************************************************************************/

void Brw_RemoveUsrWorksInAllCrss (struct UsrData *UsrDat)
  {
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
         Brw_RemoveUsrWorksInCrs (UsrDat,&Crs);
         NumCrssWorksRemoved++;
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Get summary and content of a file **********************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error

void Brw_GetSummaryAndContentOfFile (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                     char **ContentStr,
                                     long FilCod,bool GetContent)
  {
   extern const char *Txt_Filename;
   extern const char *Txt_Folder;
   extern const char *Txt_Uploaded_by;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   struct FileMetadata FileMetadata;
   bool FileHasPublisher;
   struct UsrData PublisherUsrDat;

   /***** Return nothing on error *****/
   SummaryStr[0] = '\0';	// Return nothing on error
   if (GetContent && ContentStr)
      *ContentStr = NULL;

   /***** Get file metadata *****/
   FileMetadata.FilCod = FilCod;
   Brw_GetFileMetadataByCod (&FileMetadata);

   /***** Copy file name into summary string *****/
   Str_Copy (SummaryStr,FileMetadata.FilFolLnk.Name,
             Ntf_MAX_BYTES_SUMMARY);

   /***** Copy some file metadata into content string *****/
   if (GetContent && ContentStr)
     {
      /* Get publisher */
      if (FileMetadata.PublisherUsrCod > 0)
	{
	 /* Initialize structure with publisher's data */
	 Usr_UsrDataConstructor (&PublisherUsrDat);
	 PublisherUsrDat.UsrCod = FileMetadata.PublisherUsrCod;
	 FileHasPublisher = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&PublisherUsrDat,Usr_DONT_GET_PREFS);
	}
      else
	 /* Unknown publisher */
	 FileHasPublisher = false;

      if (asprintf (ContentStr,"%s: %s<br />"	// File name
			       "%s: %s<br />"	// File path
			       "%s: %s",	// Publisher
		    Txt_Filename,FileMetadata.FilFolLnk.Name,
		    Txt_Folder,FileMetadata.FilFolLnk.Path,	// TODO: Fix bug: do not write internal name (for example "comun")
		    Txt_Uploaded_by,
		    FileHasPublisher ? PublisherUsrDat.FullName :
				       Txt_ROLES_SINGUL_Abc[Rol_UNK][Usr_SEX_UNKNOWN]) < 0)
	 Lay_NotEnoughMemoryExit ();

      /* Free memory used for publisher's data */
      if (FileMetadata.PublisherUsrCod > 0)
	 Usr_UsrDataDestructor (&PublisherUsrDat);
     }
  }

/*****************************************************************************/
/**************************** List documents found ***************************/
/*****************************************************************************/

void Brw_ListDocsFound (MYSQL_RES **mysql_res,unsigned long NumDocs,
			const char *TitleSingular,const char *TitlePlural)
  {
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;
   extern const char *Txt_File_zone;
   extern const char *Txt_Document;
   extern const char *Txt_hidden_document;
   extern const char *Txt_hidden_documents;
   MYSQL_ROW row;
   unsigned long NumDoc;
   unsigned long NumDocsNotHidden = 0;
   unsigned long NumDocsHidden;

   /***** Query database *****/
   if (NumDocs)
     {
      /***** Begin box and table *****/
      /* Number of documents found */
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        "%lu %s",
                NumDocs,(NumDocs == 1) ? TitleSingular :
        	                         TitlePlural);
      Box_StartBoxTable (NULL,Gbl.Title,NULL,
                         NULL,Box_NOT_CLOSABLE,2);

      /***** Write heading *****/
      HTM_TR_Begin (NULL);

      HTM_TH (1,1,"BM",NULL);
      HTM_TH (1,1,"LM",Txt_Institution);
      HTM_TH (1,1,"LM",Txt_Centre);
      HTM_TH (1,1,"LM",Txt_Degree);
      HTM_TH (1,1,"LM",Txt_Course);
      HTM_TH (1,1,"LM",Txt_File_zone);
      HTM_TH (1,1,"LM",Txt_Document);

      HTM_TR_End ();

      /***** List documents found *****/
      for (NumDoc = 1;
	   NumDoc <= NumDocs;
	   NumDoc++)
	{
	 /* Get next course */
	 row = mysql_fetch_row (*mysql_res);

	 /* Write data of this course */
	 Brw_WriteRowDocData (&NumDocsNotHidden,row);
	}

      /***** Write footer *****/
      HTM_TR_Begin (NULL);

      /* Number of documents not hidden found */
      HTM_TH_Begin (1,7,"CM");
      fprintf (Gbl.F.Out,"(");
      NumDocsHidden = NumDocs - NumDocsNotHidden;
      if (NumDocsHidden == 1)
	 fprintf (Gbl.F.Out,"1 %s",Txt_hidden_document);
      else
	 fprintf (Gbl.F.Out,"%lu %s",NumDocsHidden,Txt_hidden_documents);
      fprintf (Gbl.F.Out,")");
      HTM_TH_End ();

      HTM_TR_End ();

      /***** End table and box *****/
      Box_EndBoxTable ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/************ Write the data of a document (result of a query) ***************/
/*****************************************************************************/

static void Brw_WriteRowDocData (unsigned long *NumDocsNotHidden,MYSQL_ROW row)
  {
   extern const char *Txt_Documents_area;
   extern const char *Txt_Teachers_files_area;
   extern const char *Txt_Shared_files_area;
   extern const char *Txt_Assignments_area;
   extern const char *Txt_Works_area;
   extern const char *Txt_Project_documents;
   extern const char *Txt_Project_assessment;
   extern const char *Txt_Marks_area;
   extern const char *Txt_Temporary_private_storage_area;
   extern const char *Txt_Go_to_X;
   extern const char *Txt_Folder;
   struct FileMetadata FileMetadata;
   long InsCod;
   long CtrCod;
   long DegCod;
   long CrsCod;
   long GrpCod;
   Act_Action_t Action;
   const char *InsShortName;
   const char *CtrShortName;
   const char *DegShortName;
   const char *CrsShortName;
   const char *BgColor;
   const char *Title;
   char FileNameToShow[NAME_MAX + 1];
/*
   row[ 0] = FilCod
   row[ 1] = PathFromRoot
   row[ 2] = InsCod
   row[ 3] = InsShortName
   row[ 4] = CtrCod
   row[ 5] = CtrShortName
   row[ 6] = DegCod
   row[ 7] = DegShortName
   row[ 8] = CrsCod
   row[ 9] = CrsShortName
   row[10] = GrpCod
*/
   /***** Get file code (row[0]) and metadata *****/
   FileMetadata.FilCod = Str_ConvertStrCodToLongCod (row[0]);
   Brw_GetFileMetadataByCod (&FileMetadata);

   if (!Brw_CheckIfFileOrFolderIsHidden (&FileMetadata))
     {
      /***** Get institution code (row[2]) *****/
      InsCod = Str_ConvertStrCodToLongCod (row[2]);
      InsShortName = row[3];

      /***** Get centre code (row[4]) *****/
      CtrCod = Str_ConvertStrCodToLongCod (row[4]);
      CtrShortName = row[5];

      /***** Get degree code (row[6]) *****/
      DegCod = Str_ConvertStrCodToLongCod (row[6]);
      DegShortName = row[7];

      /***** Get course code (row[8]) *****/
      CrsCod = Str_ConvertStrCodToLongCod (row[8]);
      CrsShortName = row[9];

      /***** Get group code (row[8]) *****/
      GrpCod = Str_ConvertStrCodToLongCod (row[10]);

      /***** Set row color *****/
      BgColor = (CrsCod > 0 &&
	         CrsCod == Gbl.Hierarchy.Crs.CrsCod) ? "LIGHT_BLUE" :
                                                        Gbl.ColorRows[Gbl.RowEvenOdd];

      HTM_TR_Begin (NULL);

      /***** Write number of document in this search *****/
      HTM_TD_Begin ("class=\"DAT RT %s\"",BgColor);
      fprintf (Gbl.F.Out,"%lu",++(*NumDocsNotHidden));
      HTM_TD_End ();

      /***** Write institution logo, institution short name *****/
      HTM_TD_Begin ("class=\"DAT LT %s\"",BgColor);
      if (InsCod > 0)
	{
         Frm_StartFormGoTo (ActSeeInsInf);
         Deg_PutParamDegCod (InsCod);
         snprintf (Gbl.Title,sizeof (Gbl.Title),
                   Txt_Go_to_X,
		   InsShortName);
         Frm_LinkFormSubmit (Gbl.Title,"DAT",NULL);
         Log_DrawLogo (Hie_INS,InsCod,InsShortName,20,"CT",true);
	 fprintf (Gbl.F.Out,"&nbsp;%s",InsShortName);
	 Frm_LinkFormEnd ();
	 Frm_EndForm ();
	}
      HTM_TD_End ();

      /***** Write centre logo, centre short name *****/
      HTM_TD_Begin ("class=\"DAT LT %s\"",BgColor);
      if (CtrCod > 0)
	{
         Frm_StartFormGoTo (ActSeeCtrInf);
         Deg_PutParamDegCod (CtrCod);
         snprintf (Gbl.Title,sizeof (Gbl.Title),
                   Txt_Go_to_X,
		   CtrShortName);
         Frm_LinkFormSubmit (Gbl.Title,"DAT",NULL);
         Log_DrawLogo (Hie_CTR,CtrCod,CtrShortName,20,"CT",true);
	 fprintf (Gbl.F.Out,"&nbsp;%s",CtrShortName);
	 Frm_LinkFormEnd ();
	 Frm_EndForm ();
	}
      HTM_TD_End ();

      /***** Write degree logo, degree short name *****/
      HTM_TD_Begin ("class=\"DAT LT %s\"",BgColor);
      if (DegCod > 0)
	{
         Frm_StartFormGoTo (ActSeeDegInf);
         Deg_PutParamDegCod (DegCod);
         snprintf (Gbl.Title,sizeof (Gbl.Title),
                   Txt_Go_to_X,
		   DegShortName);
         Frm_LinkFormSubmit (Gbl.Title,"DAT",NULL);
         Log_DrawLogo (Hie_DEG,DegCod,DegShortName,20,"CT",true);
	 fprintf (Gbl.F.Out,"&nbsp;%s",DegShortName);
	 Frm_LinkFormEnd ();
	 Frm_EndForm ();
	}
      HTM_TD_End ();

      /***** Write course short name *****/
      HTM_TD_Begin ("class=\"DAT LT %s\"",BgColor);
      if (CrsCod > 0)
	{
	 Frm_StartFormGoTo (ActSeeCrsInf);
	 Crs_PutParamCrsCod (CrsCod);
	 snprintf (Gbl.Title,sizeof (Gbl.Title),
	           Txt_Go_to_X,
		   CrsShortName);
	 Frm_LinkFormSubmit (Gbl.Title,"DAT",NULL);
	 fprintf (Gbl.F.Out,"%s",CrsShortName);
	 Frm_LinkFormEnd ();
	 Frm_EndForm ();
	}
      HTM_TD_End ();

      /***** Write file zone *****/
      switch (FileMetadata.FileBrowser)
	{
	 case Brw_ADMI_DOC_INS:
	 case Brw_ADMI_DOC_CTR:
	 case Brw_ADMI_DOC_DEG:
	 case Brw_ADMI_DOC_CRS:
	 case Brw_ADMI_DOC_GRP:
	    Title = Txt_Documents_area;
	    break;
	 case Brw_ADMI_TCH_CRS:
	 case Brw_ADMI_TCH_GRP:
	    Title = Txt_Teachers_files_area;
	    break;
         case Brw_ADMI_SHR_INS:
         case Brw_ADMI_SHR_CTR:
         case Brw_ADMI_SHR_DEG:
	 case Brw_ADMI_SHR_CRS:
	 case Brw_ADMI_SHR_GRP:
	    Title = Txt_Shared_files_area;
	    break;
	 case Brw_ADMI_ASG_USR:
	    Title = Txt_Assignments_area;
	    break;
	 case Brw_ADMI_WRK_USR:
	    Title = Txt_Works_area;
	    break;
	 case Brw_ADMI_DOC_PRJ:
	    Title = Txt_Project_documents;
	    break;
	 case Brw_ADMI_ASS_PRJ:
	    Title = Txt_Project_assessment;
	    break;
	 case Brw_ADMI_MRK_CRS:
	 case Brw_ADMI_MRK_GRP:
	    Title = Txt_Marks_area;
	    break;
	 case Brw_ADMI_BRF_USR:
	    Title = Txt_Temporary_private_storage_area;
	    break;
	 default:
	    Title = "";
	    break;
	}

      HTM_TD_Begin ("class=\"DAT LT %s\"",BgColor);
      fprintf (Gbl.F.Out,"%s",Title);
      HTM_TD_End ();

      /***** Get the name of the file to show *****/
      Brw_GetFileNameToShow (FileMetadata.FilFolLnk.Type,
                             FileMetadata.FilFolLnk.Name,
                             FileNameToShow);

      /***** Write file name using path (row[1]) *****/
      HTM_TD_Begin ("class=\"DAT_N LT %s\"",BgColor);

      /* Begin form */
      Action = Brw_ActReqDatFile[Brw_FileBrowserForFoundDocs[FileMetadata.FileBrowser]];

      if (CrsCod > 0)
        {
	 Frm_StartFormGoTo (Action);
	 Crs_PutParamCrsCod (CrsCod);	// Go to course
	 if (GrpCod > 0)
	    Grp_PutParamGrpCod (GrpCod);
        }
      else if (DegCod > 0)
	{
	 Frm_StartFormGoTo (Action);
	 Deg_PutParamDegCod (DegCod);	// Go to degree
	}
      else if (CtrCod > 0)
	{
	 Frm_StartFormGoTo (Action);
	 Ctr_PutParamCtrCod (CtrCod);	// Go to centre
	}
      else if (InsCod > 0)
	{
	 Frm_StartFormGoTo (Action);
	 Ins_PutParamInsCod (InsCod);	// Go to institution
	}
      else
         Frm_StartForm (Action);

      /* Parameters to go to file / folder */
      if (FileMetadata.FilFolLnk.Type == Brw_IS_FOLDER)
	 Brw_PutImplicitParamsFileBrowser ();
      else
	 Brw_PutParamsFileBrowser (NULL,		// Not used
				   NULL,		// Not used
				   Brw_IS_UNKNOWN,	// Not used
				   FileMetadata.FilCod);

      /* File or folder icon */
      Frm_LinkFormSubmit (FileNameToShow,"DAT_N",NULL);
      if (FileMetadata.FilFolLnk.Type == Brw_IS_FOLDER)
	 /* Icon with folder */
         Ico_PutIcon ("folder-yellow.png",Txt_Folder,"CONTEXT_ICO_16x16");
      else
	 /* Icon with file type or link */
	 Brw_PutIconFile (16,FileMetadata.FilFolLnk.Type,FileMetadata.FilFolLnk.Name);
      fprintf (Gbl.F.Out,"&nbsp;%s",FileNameToShow);
      Frm_LinkFormEnd ();

      /* End form */
      Frm_EndForm ();

      HTM_TD_End ();
      HTM_TR_End ();

      Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
     }
  }

/*****************************************************************************/
/***************** Write a form (link) to remove old files *******************/
/*****************************************************************************/

static void Brw_PutLinkToAskRemOldFiles (void)
  {
   extern const char *Txt_Remove_old_files;

   Lay_PutContextualLinkIconText (ActReqRemOldBrf,NULL,
				  Brw_PutHiddenParamFullTreeIfSelected,
				  "trash.svg",
				  Txt_Remove_old_files);
  }

/*****************************************************************************/
/************** Write a form fo confirm removing of old files ****************/
/*****************************************************************************/

void Brw_AskRemoveOldFiles (void)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Remove_old_files;
   extern const char *Txt_Remove_files_older_than_PART_1_OF_2;
   extern const char *Txt_Remove_files_older_than_PART_2_OF_2;
   extern const char *Txt_Remove;
   unsigned Months;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Begin form *****/
   Frm_StartForm (ActRemOldBrf);
   Brw_PutHiddenParamFullTreeIfSelected ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Remove_old_files,NULL,
                 NULL,Box_NOT_CLOSABLE);

   /***** Form to request number of months (to remove files older) *****/
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s&nbsp;",Txt_Remove_files_older_than_PART_1_OF_2);
   HTM_SELECT_Begin (false,
		     "name=\"Months\"");
   for (Months  = Brw_MIN_MONTHS_TO_REMOVE_OLD_FILES;
        Months <= Brw_MAX_MONTHS_IN_BRIEFCASE;
        Months++)
     {
      fprintf (Gbl.F.Out,"<option");
      if (Months == Brw_DEF_MONTHS_TO_REMOVE_OLD_FILES)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%u</option>",Months);
     }
   HTM_SELECT_End ();
   fprintf (Gbl.F.Out,"&nbsp;");
   fprintf (Gbl.F.Out,Txt_Remove_files_older_than_PART_2_OF_2,
            Cfg_PLATFORM_SHORT_NAME);
   HTM_LABEL_End ();

   /***** Send button and end box *****/
   Box_EndBoxWithButton (Btn_REMOVE_BUTTON,Txt_Remove);

   /***** End form *****/
   Frm_EndForm ();

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/*********************** Remove old files in briefcase ***********************/
/*****************************************************************************/

void Brw_RemoveOldFilesBriefcase (void)
  {
   extern const char *Txt_Files_removed;
   extern const char *Txt_Links_removed;
   extern const char *Txt_Folders_removed;
   unsigned Months;
   struct Brw_NumObjects Removed;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   if (Brw_GetIfCrsAssigWorksFileBrowser ())
     {
      /***** Get parameter with number of months without access *****/
      Months = (unsigned)
	       Par_GetParToUnsignedLong ("Months",
                                         Brw_MIN_MONTHS_TO_REMOVE_OLD_FILES,
                                         Brw_MAX_MONTHS_IN_BRIEFCASE,
                                         Brw_DEF_MONTHS_TO_REMOVE_OLD_FILES);

      /***** Remove old files *****/
      Brw_RemoveOldFilesInBrowser (Months,&Removed);

      /***** Success message *****/
      Ale_ShowAlert (Ale_SUCCESS,"%s: %u<br />"
		                 "%s: %u<br />"
		                 "%s: %u",
	             Txt_Files_removed  ,Removed.NumFiles,
	             Txt_Links_removed  ,Removed.NumLinks,
	             Txt_Folders_removed,Removed.NumFolds);
     }

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/******************************* Remove old files ****************************/
/*****************************************************************************/

static void Brw_RemoveOldFilesInBrowser (unsigned Months,struct Brw_NumObjects *Removed)
  {
   time_t TimeRemoveFilesOlder;

   /***** Compute time in seconds
          (files older than this time will be removed) *****/
   TimeRemoveFilesOlder = Gbl.StartExecutionTimeUTC -
	                  (time_t) Months * Dat_SECONDS_IN_ONE_MONTH;

   /***** Remove old files recursively *****/
   Removed->NumFiles =
   Removed->NumLinks =
   Removed->NumFolds = 0;
   Brw_ScanDirRemovingOldFiles (1,Gbl.FileBrowser.Priv.PathRootFolder,
                                Brw_RootFolderInternalNames[Gbl.FileBrowser.Type],
                                TimeRemoveFilesOlder,Removed);

   /***** Remove affected clipboards *****/
   if (Removed->NumFiles ||
       Removed->NumLinks ||
       Removed->NumFolds)	// If anything has been changed
      Brw_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
				    Gbl.Usrs.Me.UsrDat.UsrCod,
				    Gbl.Usrs.Other.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************* Scan a directory recursively removing old files ***************/
/*****************************************************************************/

static void Brw_ScanDirRemovingOldFiles (unsigned Level,
                                         const char Path[PATH_MAX + 1],
                                         const char PathInTree[PATH_MAX + 1],
                                         time_t TimeRemoveFilesOlder,
                                         struct Brw_NumObjects *Removed)
  {
   struct dirent **FileList;
   int NumFile;
   int NumFiles;
   char PathFileRel[PATH_MAX + 1];
   char PathFileInExplTree[PATH_MAX + 1];
   struct stat FolderStatus;
   struct stat FileStatus;

   /***** Save folder status *****/
   // Folder st_mtime must be saved before remove files inside it
   // because st_mtime is updated by the deletion
   if (lstat (Path,&FolderStatus))	// On success ==> 0 is returned
      Lay_ShowErrorAndExit ("Can not get information about a file or folder.");
   /***** Scan directory *****/
   else if ((NumFiles = scandir (Path,&FileList,NULL,alphasort)) >= 0)	// No error
     {
      /***** Check file by file removing old files *****/
      for (NumFile = 0;
	   NumFile < NumFiles;
	   NumFile++)
	{
	 if (strcmp (FileList[NumFile]->d_name,".") &&
	     strcmp (FileList[NumFile]->d_name,".."))	// Skip directories "." and ".."
	   {
	    /***** Construct the full path of the file or folder *****/
	    snprintf (PathFileRel,sizeof (PathFileRel),
		      "%s/%s",
		      Path,FileList[NumFile]->d_name);
	    snprintf (PathFileInExplTree,sizeof (PathFileInExplTree),
		      "%s/%s",
		      PathInTree,FileList[NumFile]->d_name);

	    /***** Get file or folder status *****/
	    if (lstat (PathFileRel,&FileStatus))	// On success ==> 0 is returned
	       Lay_ShowErrorAndExit ("Can not get information about a file or folder.");
	    else if (S_ISDIR (FileStatus.st_mode))				// It's a folder
	       /* Scan subtree starting at this this directory recursively */
	       Brw_ScanDirRemovingOldFiles (Level + 1,PathFileRel,
					    PathFileInExplTree,
					    TimeRemoveFilesOlder,Removed);
	    else if (S_ISREG (FileStatus.st_mode) &&			// It's a regular file
		     FileStatus.st_mtime < TimeRemoveFilesOlder) 	// ..and it's old
	      {
	       /* Remove file/link from disk and database */
		Brw_RemoveFileFromDiskAndDB (PathFileRel,
					     PathFileInExplTree);

	       /* Update number of files/links removed */
	       if (Str_FileIs (PathFileRel,"url"))
		  (Removed->NumLinks)++;	// It's a link (URL inside a .url file)
	       else
		  (Removed->NumFiles)++;	// It's a file
	      }
	   }
	 free (FileList[NumFile]);
	}
      free (FileList);

      if (Level > 1)	// If not root folder
	{
	 if (NumFiles > 2)
	   {
	    /***** Rescan folder in order to count
	           the new number of files after deletion *****/
	    if ((NumFiles = scandir (Path,&FileList,NULL,alphasort)) >= 0)	// No error
	      {
	       /* Free list of files */
	       for (NumFile = 0;
		    NumFile < NumFiles;
		    NumFile++)
		  free (FileList[NumFile]);
	       free (FileList);
	      }
	    else
	       Lay_ShowErrorAndExit ("Error while scanning directory.");
	   }

	 if (NumFiles <= 2 &&					// It's an empty folder
	     FolderStatus.st_mtime < TimeRemoveFilesOlder)	//  ..and it was old before deletion
	   {
	    /* Remove folder from disk and database */
	    if (Brw_RemoveFolderFromDiskAndDB (Path,PathInTree))
	       Lay_ShowErrorAndExit ("Can not remove folder.");

	    /* Update number of files/links removed */
	    (Removed->NumFolds)++;
	   }
	}
     }
   else
      Lay_ShowErrorAndExit ("Error while scanning directory.");
  }

/*****************************************************************************/
/******************* Remove file/link from disk and database *****************/
/*****************************************************************************/

static void Brw_RemoveFileFromDiskAndDB (const char Path[PATH_MAX + 1],
                                         const char FullPathInTree[PATH_MAX + 1])
  {
   /***** Remove file from disk *****/
   if (unlink (Path))
      Lay_ShowErrorAndExit ("Can not remove file / link.");

   /***** If a file is removed,
          it is necessary to remove it from the database *****/
   Brw_RemoveOneFileOrFolderFromDB (FullPathInTree);
  }

/*****************************************************************************/
/******************** Remove folder from disk and database *******************/
/*****************************************************************************/
// Return the returned value of rmdir

static int Brw_RemoveFolderFromDiskAndDB (const char Path[PATH_MAX + 1],
                                          const char FullPathInTree[PATH_MAX + 1])
  {
   int Result;

   /***** Remove folder from disk *****/
   Result = rmdir (Path);	// On success, zero is returned.
				// On error, -1 is returned, and errno is set appropriately.
   if (!Result)	// Success
     {
      /***** If a folder is removed,
	     it is necessary to remove it from the database *****/
      Brw_RemoveOneFileOrFolderFromDB (FullPathInTree);

      /***** Remove affected expanded folders *****/
      Brw_RemoveAffectedExpandedFolders (FullPathInTree);
     }

   return Result;
  }

/*****************************************************************************/
/*********** Get action to expand folder for current file browser ************/
/*****************************************************************************/

Act_Action_t Brw_GetActionExpand (void)
  {
   return Brw_ActExpandFolder[Gbl.FileBrowser.Type];
  }

/*****************************************************************************/
/********** Get action to contract folder for current file browser ***********/
/*****************************************************************************/

Act_Action_t Brw_GetActionContract (void)
  {
   return Brw_ActContractFolder[Gbl.FileBrowser.Type];
  }
