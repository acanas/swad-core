// swad_browser.c: file browsers

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

#define _GNU_SOURCE 		// For asprintf
#include <dirent.h>		// For scandir, etc.
#include <errno.h>		// For errno
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For exit, system, free, etc
#include <string.h>		// For string functions
#include <sys/types.h>		// For lstat, time_t
#include <sys/stat.h>		// For lstat
#include <time.h>		// For time
#include <unistd.h>		// For access, lstat, getpid, chdir, symlink

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_assignment.h"
#include "swad_box.h"
#include "swad_browser.h"
#include "swad_browser_database.h"
#include "swad_browser_size.h"
#include "swad_config.h"
#include "swad_course_database.h"
#include "swad_database.h"
#include "swad_enrolment_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_file_extension.h"
#include "swad_file_MIME.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_group_database.h"
#include "swad_hidden_visible.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_logo.h"
#include "swad_mark.h"
#include "swad_mark_database.h"
#include "swad_notification.h"
#include "swad_notification_database.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"
#include "swad_profile.h"
#include "swad_profile_database.h"
#include "swad_project.h"
#include "swad_project_database.h"
#include "swad_resource.h"
#include "swad_role.h"
#include "swad_setting.h"
#include "swad_setting_database.h"
#include "swad_string.h"
#include "swad_timeline.h"
#include "swad_timeline_note.h"
#include "swad_user_database.h"
#include "swad_zip.h"

/*****************************************************************************/
/******************** Global variables from other modules ********************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Private types *******************************/
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
/***************************** Public constants ******************************/
/*****************************************************************************/

// Names of root folders
const char *Brw_RootFolderInternalNames[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = "",
   [Brw_SHOW_DOC_CRS] = Brw_INTERNAL_NAME_ROOT_FOLDER_DOWNLOAD,
   [Brw_SHOW_MRK_CRS] = Brw_INTERNAL_NAME_ROOT_FOLDER_MARKS,
   [Brw_ADMI_DOC_CRS] = Brw_INTERNAL_NAME_ROOT_FOLDER_DOWNLOAD,
   [Brw_ADMI_SHR_CRS] = Brw_INTERNAL_NAME_ROOT_FOLDER_SHARED,
   [Brw_ADMI_SHR_GRP] = Brw_INTERNAL_NAME_ROOT_FOLDER_SHARED,
   [Brw_ADMI_WRK_USR] = Brw_INTERNAL_NAME_ROOT_FOLDER_WORKS,
   [Brw_ADMI_WRK_CRS] = Brw_INTERNAL_NAME_ROOT_FOLDER_WORKS,
   [Brw_ADMI_MRK_CRS] = Brw_INTERNAL_NAME_ROOT_FOLDER_MARKS,
   [Brw_ADMI_BRF_USR] = Brw_INTERNAL_NAME_ROOT_FOLDER_BRIEF,
   [Brw_SHOW_DOC_GRP] = Brw_INTERNAL_NAME_ROOT_FOLDER_DOWNLOAD,
   [Brw_ADMI_DOC_GRP] = Brw_INTERNAL_NAME_ROOT_FOLDER_DOWNLOAD,
   [Brw_SHOW_MRK_GRP] = Brw_INTERNAL_NAME_ROOT_FOLDER_MARKS,
   [Brw_ADMI_MRK_GRP] = Brw_INTERNAL_NAME_ROOT_FOLDER_MARKS,
   [Brw_ADMI_ASG_USR] = Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
   [Brw_ADMI_ASG_CRS] = Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
   [Brw_SHOW_DOC_DEG] = Brw_INTERNAL_NAME_ROOT_FOLDER_DOCUMENTS,
   [Brw_ADMI_DOC_DEG] = Brw_INTERNAL_NAME_ROOT_FOLDER_DOCUMENTS,
   [Brw_SHOW_DOC_CTR] = Brw_INTERNAL_NAME_ROOT_FOLDER_DOCUMENTS,
   [Brw_ADMI_DOC_CTR] = Brw_INTERNAL_NAME_ROOT_FOLDER_DOCUMENTS,
   [Brw_SHOW_DOC_INS] = Brw_INTERNAL_NAME_ROOT_FOLDER_DOCUMENTS,
   [Brw_ADMI_DOC_INS] = Brw_INTERNAL_NAME_ROOT_FOLDER_DOCUMENTS,
   [Brw_ADMI_SHR_DEG] = Brw_INTERNAL_NAME_ROOT_FOLDER_SHARED_FILES,
   [Brw_ADMI_SHR_CTR] = Brw_INTERNAL_NAME_ROOT_FOLDER_SHARED_FILES,
   [Brw_ADMI_SHR_INS] = Brw_INTERNAL_NAME_ROOT_FOLDER_SHARED_FILES,
   [Brw_ADMI_TCH_CRS] = Brw_INTERNAL_NAME_ROOT_FOLDER_TEACHERS,
   [Brw_ADMI_TCH_GRP] = Brw_INTERNAL_NAME_ROOT_FOLDER_TEACHERS,
   [Brw_ADMI_DOC_PRJ] = Brw_INTERNAL_NAME_ROOT_FOLDER_PROJECT_DOCUMENTS,
   [Brw_ADMI_ASS_PRJ] = Brw_INTERNAL_NAME_ROOT_FOLDER_PROJECT_ASSESSMENT,
  };

bool Brw_TypeIsCrsBrw[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_SHOW_DOC_CRS] = true,
   [Brw_ADMI_DOC_CRS] = true,
   [Brw_ADMI_TCH_CRS] = true,
   [Brw_ADMI_SHR_CRS] = true,
   [Brw_SHOW_MRK_CRS] = true,
   [Brw_ADMI_MRK_CRS] = true,
  };
bool Brw_TypeIsGrpBrw[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_SHOW_DOC_GRP] = true,
   [Brw_ADMI_DOC_GRP] = true,
   [Brw_ADMI_TCH_GRP] = true,
   [Brw_ADMI_SHR_GRP] = true,
   [Brw_SHOW_MRK_GRP] = true,
   [Brw_ADMI_MRK_GRP] = true,
  };
bool Brw_TypeIsSeeDoc[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_SHOW_DOC_INS] = true,
   [Brw_SHOW_DOC_CTR] = true,
   [Brw_SHOW_DOC_DEG] = true,
   [Brw_SHOW_DOC_CRS] = true,
   [Brw_SHOW_DOC_GRP] = true,
  };
bool Brw_TypeIsAdmDoc[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_ADMI_DOC_INS] = true,
   [Brw_ADMI_DOC_CTR] = true,
   [Brw_ADMI_DOC_DEG] = true,
   [Brw_ADMI_DOC_CRS] = true,
   [Brw_ADMI_DOC_GRP] = true,
  };
bool Brw_TypeIsAdmSha[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_ADMI_SHR_INS] = true,
   [Brw_ADMI_SHR_CTR] = true,
   [Brw_ADMI_SHR_DEG] = true,
   [Brw_ADMI_SHR_CRS] = true,
   [Brw_ADMI_SHR_GRP] = true,
  };
bool Brw_TypeIsAdmAsg[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_ADMI_ASG_USR] = true,
   [Brw_ADMI_ASG_CRS] = true,
  };
bool Brw_TypeIsAdmUsrAsgWrk[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_ADMI_ASG_USR] = true,
   [Brw_ADMI_WRK_USR] = true,
  };
bool Brw_TypeIsAdmCrsAsgWrk[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_ADMI_ASG_CRS] = true,
   [Brw_ADMI_WRK_CRS] = true,
  };
bool Brw_TypeIsSeeMrk[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_SHOW_MRK_CRS] = true,
   [Brw_SHOW_MRK_GRP] = true,
  };
bool Brw_TypeIsAdmMrk[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_ADMI_MRK_CRS] = true,
   [Brw_ADMI_MRK_GRP] = true,
  };
bool Brw_TypeIsAdmBrf[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_ADMI_BRF_USR] = true,
  };
bool Brw_TypeIsAdmPrj[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_ADMI_DOC_PRJ] = true,
   [Brw_ADMI_ASS_PRJ] = true,
  };

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const char *Brw_FileTypeParName[Brw_NUM_FILE_TYPES] =
  {
   [Brw_IS_UNKNOWN] = "BrwFFL",
   [Brw_IS_FILE   ] = "BrwFil",	// Do not use Fil_NAME_OF_PARAM_FILENAME_ORG
   [Brw_IS_FOLDER ] = "BrwFol",
   [Brw_IS_LINK   ] = "BrwLnk",
  };

// Browsers viewable shown in search for documents
static const Brw_FileBrowser_t Brw_FileBrowserForFoundDocs[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = Brw_UNKNOWN,
   [Brw_SHOW_DOC_CRS] = Brw_SHOW_DOC_CRS,
   [Brw_SHOW_MRK_CRS] = Brw_SHOW_MRK_CRS,
   [Brw_ADMI_DOC_CRS] = Brw_SHOW_DOC_CRS,
   [Brw_ADMI_SHR_CRS] = Brw_ADMI_SHR_CRS,
   [Brw_ADMI_SHR_GRP] = Brw_ADMI_SHR_GRP,
   [Brw_ADMI_WRK_USR] = Brw_ADMI_WRK_USR,
   [Brw_ADMI_WRK_CRS] = Brw_ADMI_WRK_USR,
   [Brw_ADMI_MRK_CRS] = Brw_ADMI_MRK_CRS,
   [Brw_ADMI_BRF_USR] = Brw_ADMI_BRF_USR,
   [Brw_SHOW_DOC_GRP] = Brw_SHOW_DOC_GRP,
   [Brw_ADMI_DOC_GRP] = Brw_SHOW_DOC_GRP,
   [Brw_SHOW_MRK_GRP] = Brw_SHOW_MRK_GRP,
   [Brw_ADMI_MRK_GRP] = Brw_SHOW_MRK_GRP,
   [Brw_ADMI_ASG_USR] = Brw_ADMI_ASG_USR,
   [Brw_ADMI_ASG_CRS] = Brw_ADMI_ASG_USR,
   [Brw_SHOW_DOC_DEG] = Brw_SHOW_DOC_DEG,
   [Brw_ADMI_DOC_DEG] = Brw_SHOW_DOC_DEG,
   [Brw_SHOW_DOC_CTR] = Brw_SHOW_DOC_CTR,
   [Brw_ADMI_DOC_CTR] = Brw_SHOW_DOC_CTR,
   [Brw_SHOW_DOC_INS] = Brw_SHOW_DOC_INS,
   [Brw_ADMI_DOC_INS] = Brw_SHOW_DOC_INS,
   [Brw_ADMI_SHR_DEG] = Brw_ADMI_SHR_DEG,
   [Brw_ADMI_SHR_CTR] = Brw_ADMI_SHR_CTR,
   [Brw_ADMI_SHR_INS] = Brw_ADMI_SHR_INS,
   [Brw_ADMI_TCH_CRS] = Brw_ADMI_TCH_CRS,
   [Brw_ADMI_TCH_GRP] = Brw_ADMI_TCH_GRP,
   [Brw_ADMI_DOC_PRJ] = Brw_ADMI_DOC_PRJ,
   [Brw_ADMI_ASS_PRJ] = Brw_ADMI_ASS_PRJ,
  };

// Browsers types for database "clipboard" table
static const Brw_FileBrowser_t Brw_FileBrowserForDB_clipboard[Brw_NUM_TYPES_FILE_BROWSER] =
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

static const Act_Action_t Brw_ActSeeAdm[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActSeeDocCrs,
   [Brw_SHOW_MRK_CRS] = ActSeeMrkCrs,
   [Brw_ADMI_DOC_CRS] = ActAdmDocCrs,
   [Brw_ADMI_SHR_CRS] = ActAdmShaCrs,
   [Brw_ADMI_SHR_GRP] = ActAdmShaGrp,
   [Brw_ADMI_WRK_USR] = ActAdmAsgWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActAdmAsgWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActAdmMrkCrs,
   [Brw_ADMI_BRF_USR] = ActAdmBrf,
   [Brw_SHOW_DOC_GRP] = ActSeeDocGrp,
   [Brw_ADMI_DOC_GRP] = ActAdmDocGrp,
   [Brw_SHOW_MRK_GRP] = ActSeeMrkGrp,
   [Brw_ADMI_MRK_GRP] = ActAdmMrkGrp,
   [Brw_ADMI_ASG_USR] = ActAdmAsgWrkUsr,
   [Brw_ADMI_ASG_CRS] = ActAdmAsgWrkCrs,
   [Brw_SHOW_DOC_DEG] = ActSeeDocDeg,
   [Brw_ADMI_DOC_DEG] = ActAdmDocDeg,
   [Brw_SHOW_DOC_CTR] = ActSeeDocCtr,
   [Brw_ADMI_DOC_CTR] = ActAdmDocCtr,
   [Brw_SHOW_DOC_INS] = ActSeeDocIns,
   [Brw_ADMI_DOC_INS] = ActAdmDocIns,
   [Brw_ADMI_SHR_DEG] = ActAdmShaDeg,
   [Brw_ADMI_SHR_CTR] = ActAdmShaCtr,
   [Brw_ADMI_SHR_INS] = ActAdmShaIns,
   [Brw_ADMI_TCH_CRS] = ActAdmTchCrs,
   [Brw_ADMI_TCH_GRP] = ActAdmTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActAdmDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActAdmAssPrj,
  };

static const Act_Action_t Brw_ActFromSeeToAdm[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActAdmDocCrs,
   [Brw_SHOW_MRK_CRS] = ActAdmMrkCrs,
   [Brw_ADMI_DOC_CRS] = ActUnk,
   [Brw_ADMI_SHR_CRS] = ActUnk,
   [Brw_ADMI_SHR_GRP] = ActUnk,
   [Brw_ADMI_WRK_USR] = ActUnk,
   [Brw_ADMI_WRK_CRS] = ActUnk,
   [Brw_ADMI_MRK_CRS] = ActUnk,
   [Brw_ADMI_BRF_USR] = ActUnk,
   [Brw_SHOW_DOC_GRP] = ActAdmDocGrp,
   [Brw_ADMI_DOC_GRP] = ActUnk,
   [Brw_SHOW_MRK_GRP] = ActAdmMrkGrp,
   [Brw_ADMI_MRK_GRP] = ActUnk,
   [Brw_ADMI_ASG_USR] = ActUnk,
   [Brw_ADMI_ASG_CRS] = ActUnk,
   [Brw_SHOW_DOC_DEG] = ActAdmDocDeg,
   [Brw_ADMI_DOC_DEG] = ActUnk,
   [Brw_SHOW_DOC_CTR] = ActAdmDocCtr,
   [Brw_ADMI_DOC_CTR] = ActUnk,
   [Brw_SHOW_DOC_INS] = ActAdmDocIns,
   [Brw_ADMI_DOC_INS] = ActUnk,
   [Brw_ADMI_SHR_DEG] = ActUnk,
   [Brw_ADMI_SHR_CTR] = ActUnk,
   [Brw_ADMI_SHR_INS] = ActUnk,
   [Brw_ADMI_TCH_CRS] = ActUnk,
   [Brw_ADMI_TCH_GRP] = ActUnk,
   [Brw_ADMI_DOC_PRJ] = ActUnk,
   [Brw_ADMI_ASS_PRJ] = ActUnk,
  };

static const Act_Action_t Brw_ActFromAdmToSee[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActUnk,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActSeeDocCrs,
   [Brw_ADMI_SHR_CRS] = ActUnk,
   [Brw_ADMI_SHR_GRP] = ActUnk,
   [Brw_ADMI_WRK_USR] = ActUnk,
   [Brw_ADMI_WRK_CRS] = ActUnk,
   [Brw_ADMI_MRK_CRS] = ActSeeMrkCrs,
   [Brw_ADMI_BRF_USR] = ActUnk,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActSeeDocGrp,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActSeeMrkGrp,
   [Brw_ADMI_ASG_USR] = ActUnk,
   [Brw_ADMI_ASG_CRS] = ActUnk,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActSeeDocDeg,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActSeeDocCtr,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActSeeDocIns,
   [Brw_ADMI_SHR_DEG] = ActUnk,
   [Brw_ADMI_SHR_CTR] = ActUnk,
   [Brw_ADMI_SHR_INS] = ActUnk,
   [Brw_ADMI_TCH_CRS] = ActUnk,
   [Brw_ADMI_TCH_GRP] = ActUnk,
   [Brw_ADMI_DOC_PRJ] = ActUnk,
   [Brw_ADMI_ASS_PRJ] = ActUnk,
  };

static const Act_Action_t Brw_ActChgZone[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActChgToSeeDocCrs,
   [Brw_SHOW_MRK_CRS] = ActChgToSeeMrk,
   [Brw_ADMI_DOC_CRS] = ActChgToAdmDocCrs,
   [Brw_ADMI_SHR_CRS] = ActChgToAdmSha,
   [Brw_ADMI_SHR_GRP] = ActChgToAdmSha,
   [Brw_ADMI_WRK_USR] = ActUnk,
   [Brw_ADMI_WRK_CRS] = ActUnk,
   [Brw_ADMI_MRK_CRS] = ActChgToAdmMrk,
   [Brw_ADMI_BRF_USR] = ActUnk,
   [Brw_SHOW_DOC_GRP] = ActChgToSeeDocCrs,
   [Brw_ADMI_DOC_GRP] = ActChgToAdmDocCrs,
   [Brw_SHOW_MRK_GRP] = ActChgToSeeMrk,
   [Brw_ADMI_MRK_GRP] = ActChgToAdmMrk,
   [Brw_ADMI_ASG_USR] = ActUnk,
   [Brw_ADMI_ASG_CRS] = ActUnk,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActUnk,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActUnk,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActUnk,
   [Brw_ADMI_SHR_DEG] = ActUnk,
   [Brw_ADMI_SHR_CTR] = ActUnk,
   [Brw_ADMI_SHR_INS] = ActUnk,
   [Brw_ADMI_TCH_CRS] = ActChgToAdmTch,
   [Brw_ADMI_TCH_GRP] = ActChgToAdmTch,
   [Brw_ADMI_DOC_PRJ] = ActUnk,
   [Brw_ADMI_ASS_PRJ] = ActUnk,
  };

static const Act_Action_t Brw_ActReqDatFile[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActReqDatSeeDocCrs,
   [Brw_SHOW_MRK_CRS] = ActReqDatSeeMrkCrs,
   [Brw_ADMI_DOC_CRS] = ActReqDatAdmDocCrs,
   [Brw_ADMI_SHR_CRS] = ActReqDatShaCrs,
   [Brw_ADMI_SHR_GRP] = ActReqDatShaGrp,
   [Brw_ADMI_WRK_USR] = ActReqDatWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActReqDatWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActReqDatAdmMrkCrs,
   [Brw_ADMI_BRF_USR] = ActReqDatBrf,
   [Brw_SHOW_DOC_GRP] = ActReqDatSeeDocGrp,
   [Brw_ADMI_DOC_GRP] = ActReqDatAdmDocGrp,
   [Brw_SHOW_MRK_GRP] = ActReqDatSeeMrkGrp,
   [Brw_ADMI_MRK_GRP] = ActReqDatAdmMrkGrp,
   [Brw_ADMI_ASG_USR] = ActReqDatAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActReqDatAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActReqDatSeeDocDeg,
   [Brw_ADMI_DOC_DEG] = ActReqDatAdmDocDeg,
   [Brw_SHOW_DOC_CTR] = ActReqDatSeeDocCtr,
   [Brw_ADMI_DOC_CTR] = ActReqDatAdmDocCtr,
   [Brw_SHOW_DOC_INS] = ActReqDatSeeDocIns,
   [Brw_ADMI_DOC_INS] = ActReqDatAdmDocIns,
   [Brw_ADMI_SHR_DEG] = ActReqDatShaDeg,
   [Brw_ADMI_SHR_CTR] = ActReqDatShaCtr,
   [Brw_ADMI_SHR_INS] = ActReqDatShaIns,
   [Brw_ADMI_TCH_CRS] = ActReqDatTchCrs,
   [Brw_ADMI_TCH_GRP] = ActReqDatTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActReqDatDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActReqDatAssPrj,
  };

static const Act_Action_t Brw_ActReqLnk[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActReqLnkSeeDocCrs,
   [Brw_SHOW_MRK_CRS] = ActReqLnkSeeMrkCrs,
   [Brw_ADMI_DOC_CRS] = ActReqLnkAdmDocCrs,
   [Brw_ADMI_SHR_CRS] = ActUnk,
   [Brw_ADMI_SHR_GRP] = ActUnk,
   [Brw_ADMI_WRK_USR] = ActUnk,
   [Brw_ADMI_WRK_CRS] = ActUnk,
   [Brw_ADMI_MRK_CRS] = ActReqLnkAdmMrkCrs,
   [Brw_ADMI_BRF_USR] = ActUnk,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActUnk,
   [Brw_SHOW_MRK_GRP] = ActUnk,	// ActReqLnkSeeMrkGrp,
   [Brw_ADMI_MRK_GRP] = ActUnk,	// ActReqLnkAdmMrkGrp,
   [Brw_ADMI_ASG_USR] = ActUnk,
   [Brw_ADMI_ASG_CRS] = ActUnk,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActUnk,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActUnk,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActUnk,
   [Brw_ADMI_SHR_DEG] = ActUnk,
   [Brw_ADMI_SHR_CTR] = ActUnk,
   [Brw_ADMI_SHR_INS] = ActUnk,
   [Brw_ADMI_TCH_CRS] = ActUnk,
   [Brw_ADMI_TCH_GRP] = ActUnk,
   [Brw_ADMI_DOC_PRJ] = ActUnk,
   [Brw_ADMI_ASS_PRJ] = ActUnk,
  };

static const Act_Action_t Brw_ActDowFile[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActDowSeeDocCrs,
   [Brw_SHOW_MRK_CRS] = ActSeeMyMrkCrs,
   [Brw_ADMI_DOC_CRS] = ActDowAdmDocCrs,
   [Brw_ADMI_SHR_CRS] = ActDowShaCrs,
   [Brw_ADMI_SHR_GRP] = ActDowShaGrp,
   [Brw_ADMI_WRK_USR] = ActDowWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActDowWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActDowAdmMrkCrs,
   [Brw_ADMI_BRF_USR] = ActDowBrf,
   [Brw_SHOW_DOC_GRP] = ActDowSeeDocGrp,
   [Brw_ADMI_DOC_GRP] = ActDowAdmDocGrp,
   [Brw_SHOW_MRK_GRP] = ActSeeMyMrkGrp,
   [Brw_ADMI_MRK_GRP] = ActDowAdmMrkGrp,
   [Brw_ADMI_ASG_USR] = ActDowAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActDowAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActDowSeeDocDeg,
   [Brw_ADMI_DOC_DEG] = ActDowAdmDocDeg,
   [Brw_SHOW_DOC_CTR] = ActDowSeeDocCtr,
   [Brw_ADMI_DOC_CTR] = ActDowAdmDocCtr,
   [Brw_SHOW_DOC_INS] = ActDowSeeDocIns,
   [Brw_ADMI_DOC_INS] = ActDowAdmDocIns,
   [Brw_ADMI_SHR_DEG] = ActDowShaDeg,
   [Brw_ADMI_SHR_CTR] = ActDowShaCtr,
   [Brw_ADMI_SHR_INS] = ActDowShaIns,
   [Brw_ADMI_TCH_CRS] = ActDowTchCrs,
   [Brw_ADMI_TCH_GRP] = ActDowTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActDowDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActDowAssPrj,
  };

static const Act_Action_t Brw_ActAskRemoveFile[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActUnk,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActReqRemFilDocCrs,
   [Brw_ADMI_SHR_CRS] = ActReqRemFilShaCrs,
   [Brw_ADMI_SHR_GRP] = ActReqRemFilShaGrp,
   [Brw_ADMI_WRK_USR] = ActReqRemFilWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActReqRemFilWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActReqRemFilMrkCrs,
   [Brw_ADMI_BRF_USR] = ActReqRemFilBrf,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActReqRemFilDocGrp,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActReqRemFilMrkGrp,
   [Brw_ADMI_ASG_USR] = ActReqRemFilAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActReqRemFilAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActReqRemFilDocDeg,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActReqRemFilDocCtr,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActReqRemFilDocIns,
   [Brw_ADMI_SHR_DEG] = ActReqRemFilShaDeg,
   [Brw_ADMI_SHR_CTR] = ActReqRemFilShaCtr,
   [Brw_ADMI_SHR_INS] = ActReqRemFilShaIns,
   [Brw_ADMI_TCH_CRS] = ActReqRemFilTchCrs,
   [Brw_ADMI_TCH_GRP] = ActReqRemFilTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActReqRemFilDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActReqRemFilAssPrj,
  };

static const Act_Action_t Brw_ActRemoveFile[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActUnk,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActRemFilDocCrs,
   [Brw_ADMI_SHR_CRS] = ActRemFilShaCrs,
   [Brw_ADMI_SHR_GRP] = ActRemFilShaGrp,
   [Brw_ADMI_WRK_USR] = ActRemFilWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActRemFilWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActRemFilMrkCrs,
   [Brw_ADMI_BRF_USR] = ActRemFilBrf,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActRemFilDocGrp,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActRemFilMrkGrp,
   [Brw_ADMI_ASG_USR] = ActRemFilAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActRemFilAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActRemFilDocDeg,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActRemFilDocCtr,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActRemFilDocIns,
   [Brw_ADMI_SHR_DEG] = ActRemFilShaDeg,
   [Brw_ADMI_SHR_CTR] = ActRemFilShaCtr,
   [Brw_ADMI_SHR_INS] = ActRemFilShaIns,
   [Brw_ADMI_TCH_CRS] = ActRemFilTchCrs,
   [Brw_ADMI_TCH_GRP] = ActRemFilTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActRemFilDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActRemFilAssPrj,
  };

static const Act_Action_t Brw_ActRemoveFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActUnk,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActRemFolDocCrs,
   [Brw_ADMI_SHR_CRS] = ActRemFolShaCrs,
   [Brw_ADMI_SHR_GRP] = ActRemFolShaGrp,
   [Brw_ADMI_WRK_USR] = ActRemFolWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActRemFolWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActRemFolMrkCrs,
   [Brw_ADMI_BRF_USR] = ActRemFolBrf,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActRemFolDocGrp,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActRemFolMrkGrp,
   [Brw_ADMI_ASG_USR] = ActRemFolAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActRemFolAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActRemFolDocDeg,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActRemFolDocCtr,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActRemFolDocIns,
   [Brw_ADMI_SHR_DEG] = ActRemFolShaDeg,
   [Brw_ADMI_SHR_CTR] = ActRemFolShaCtr,
   [Brw_ADMI_SHR_INS] = ActRemFolShaIns,
   [Brw_ADMI_TCH_CRS] = ActRemFolTchCrs,
   [Brw_ADMI_TCH_GRP] = ActRemFolTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActRemFolDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActRemFolAssPrj,
  };

static const Act_Action_t Brw_ActRemoveFolderNotEmpty[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActUnk,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActRemTreDocCrs,
   [Brw_ADMI_SHR_CRS] = ActRemTreShaCrs,
   [Brw_ADMI_SHR_GRP] = ActRemTreShaGrp,
   [Brw_ADMI_WRK_USR] = ActRemTreWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActRemTreWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActRemTreMrkCrs,
   [Brw_ADMI_BRF_USR] = ActRemTreBrf,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActRemTreDocGrp,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActRemTreMrkGrp,
   [Brw_ADMI_ASG_USR] = ActRemTreAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActRemTreAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActRemTreDocDeg,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActRemTreDocCtr,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActRemTreDocIns,
   [Brw_ADMI_SHR_DEG] = ActRemTreShaDeg,
   [Brw_ADMI_SHR_CTR] = ActRemTreShaCtr,
   [Brw_ADMI_SHR_INS] = ActRemTreShaIns,
   [Brw_ADMI_TCH_CRS] = ActRemTreTchCrs,
   [Brw_ADMI_TCH_GRP] = ActRemTreTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActRemTreDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActRemTreAssPrj,
  };

static const Act_Action_t Brw_ActCopy[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActUnk,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActCopDocCrs,
   [Brw_ADMI_SHR_CRS] = ActCopShaCrs,
   [Brw_ADMI_SHR_GRP] = ActCopShaGrp,
   [Brw_ADMI_WRK_USR] = ActCopWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActCopWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActCopMrkCrs,
   [Brw_ADMI_BRF_USR] = ActCopBrf,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActCopDocGrp,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActCopMrkGrp,
   [Brw_ADMI_ASG_USR] = ActCopAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActCopAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActCopDocDeg,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActCopDocCtr,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActCopDocIns,
   [Brw_ADMI_SHR_DEG] = ActCopShaDeg,
   [Brw_ADMI_SHR_CTR] = ActCopShaCtr,
   [Brw_ADMI_SHR_INS] = ActCopShaIns,
   [Brw_ADMI_TCH_CRS] = ActCopTchCrs,
   [Brw_ADMI_TCH_GRP] = ActCopTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActCopDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActCopAssPrj,
  };

static const Act_Action_t Brw_ActPaste[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActUnk,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActPasDocCrs,
   [Brw_ADMI_SHR_CRS] = ActPasShaCrs,
   [Brw_ADMI_SHR_GRP] = ActPasShaGrp,
   [Brw_ADMI_WRK_USR] = ActPasWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActPasWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActPasMrkCrs,
   [Brw_ADMI_BRF_USR] = ActPasBrf,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActPasDocGrp,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActPasMrkGrp,
   [Brw_ADMI_ASG_USR] = ActPasAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActPasAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActPasDocDeg,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActPasDocCtr,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActPasDocIns,
   [Brw_ADMI_SHR_DEG] = ActPasShaDeg,
   [Brw_ADMI_SHR_CTR] = ActPasShaCtr,
   [Brw_ADMI_SHR_INS] = ActPasShaIns,
   [Brw_ADMI_TCH_CRS] = ActPasTchCrs,
   [Brw_ADMI_TCH_GRP] = ActPasTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActPasDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActPasAssPrj,
  };

static const Act_Action_t Brw_ActFormCreate[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActUnk,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActFrmCreDocCrs,
   [Brw_ADMI_SHR_CRS] = ActFrmCreShaCrs,
   [Brw_ADMI_SHR_GRP] = ActFrmCreShaGrp,
   [Brw_ADMI_WRK_USR] = ActFrmCreWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActFrmCreWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActFrmCreMrkCrs,
   [Brw_ADMI_BRF_USR] = ActFrmCreBrf,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActFrmCreDocGrp,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActFrmCreMrkGrp,
   [Brw_ADMI_ASG_USR] = ActFrmCreAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActFrmCreAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActFrmCreDocDeg,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActFrmCreDocCtr,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActFrmCreDocIns,
   [Brw_ADMI_SHR_DEG] = ActFrmCreShaDeg,
   [Brw_ADMI_SHR_CTR] = ActFrmCreShaCtr,
   [Brw_ADMI_SHR_INS] = ActFrmCreShaIns,
   [Brw_ADMI_TCH_CRS] = ActFrmCreTchCrs,
   [Brw_ADMI_TCH_GRP] = ActFrmCreTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActFrmCreDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActFrmCreAssPrj,
  };

static const Act_Action_t Brw_ActCreateFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActUnk,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActCreFolDocCrs,
   [Brw_ADMI_SHR_CRS] = ActCreFolShaCrs,
   [Brw_ADMI_SHR_GRP] = ActCreFolShaGrp,
   [Brw_ADMI_WRK_USR] = ActCreFolWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActCreFolWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActCreFolMrkCrs,
   [Brw_ADMI_BRF_USR] = ActCreFolBrf,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActCreFolDocGrp,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActCreFolMrkGrp,
   [Brw_ADMI_ASG_USR] = ActCreFolAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActCreFolAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActCreFolDocDeg,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActCreFolDocCtr,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActCreFolDocIns,
   [Brw_ADMI_SHR_DEG] = ActCreFolShaDeg,
   [Brw_ADMI_SHR_CTR] = ActCreFolShaCtr,
   [Brw_ADMI_SHR_INS] = ActCreFolShaIns,
   [Brw_ADMI_TCH_CRS] = ActCreFolTchCrs,
   [Brw_ADMI_TCH_GRP] = ActCreFolTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActCreFolDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActCreFolAssPrj,
  };

static const Act_Action_t Brw_ActCreateLink[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActUnk,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActCreLnkDocCrs,
   [Brw_ADMI_SHR_CRS] = ActCreLnkShaCrs,
   [Brw_ADMI_SHR_GRP] = ActCreLnkShaGrp,
   [Brw_ADMI_WRK_USR] = ActCreLnkWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActCreLnkWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActUnk,
   [Brw_ADMI_BRF_USR] = ActCreLnkBrf,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActCreLnkDocGrp,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActUnk,
   [Brw_ADMI_ASG_USR] = ActCreLnkAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActCreLnkAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActCreLnkDocDeg,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActCreLnkDocCtr,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActCreLnkDocIns,
   [Brw_ADMI_SHR_DEG] = ActCreLnkShaDeg,
   [Brw_ADMI_SHR_CTR] = ActCreLnkShaCtr,
   [Brw_ADMI_SHR_INS] = ActCreLnkShaIns,
   [Brw_ADMI_TCH_CRS] = ActCreLnkTchCrs,
   [Brw_ADMI_TCH_GRP] = ActCreLnkTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActCreLnkDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActCreLnkAssPrj,
  };

static const Act_Action_t Brw_ActRenameFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActUnk,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActRenFolDocCrs,
   [Brw_ADMI_SHR_CRS] = ActRenFolShaCrs,
   [Brw_ADMI_SHR_GRP] = ActRenFolShaGrp,
   [Brw_ADMI_WRK_USR] = ActRenFolWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActRenFolWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActRenFolMrkCrs,
   [Brw_ADMI_BRF_USR] = ActRenFolBrf,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActRenFolDocGrp,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActRenFolMrkGrp,
   [Brw_ADMI_ASG_USR] = ActRenFolAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActRenFolAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActRenFolDocDeg,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActRenFolDocCtr,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActRenFolDocIns,
   [Brw_ADMI_SHR_DEG] = ActRenFolShaDeg,
   [Brw_ADMI_SHR_CTR] = ActRenFolShaCtr,
   [Brw_ADMI_SHR_INS] = ActRenFolShaIns,
   [Brw_ADMI_TCH_CRS] = ActRenFolTchCrs,
   [Brw_ADMI_TCH_GRP] = ActRenFolTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActRenFolDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActRenFolAssPrj,
  };

static const Act_Action_t Brw_ActUploadFileDropzone[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActUnk,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActRcvFilDocCrsDZ,
   [Brw_ADMI_SHR_CRS] = ActRcvFilShaCrsDZ,
   [Brw_ADMI_SHR_GRP] = ActRcvFilShaGrpDZ,
   [Brw_ADMI_WRK_USR] = ActRcvFilWrkUsrDZ,
   [Brw_ADMI_WRK_CRS] = ActRcvFilWrkCrsDZ,
   [Brw_ADMI_MRK_CRS] = ActRcvFilMrkCrsDZ,
   [Brw_ADMI_BRF_USR] = ActRcvFilBrfDZ,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActRcvFilDocGrpDZ,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActRcvFilMrkGrpDZ,
   [Brw_ADMI_ASG_USR] = ActRcvFilAsgUsrDZ,
   [Brw_ADMI_ASG_CRS] = ActRcvFilAsgCrsDZ,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActRcvFilDocDegDZ,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActRcvFilDocCtrDZ,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActRcvFilDocInsDZ,
   [Brw_ADMI_SHR_DEG] = ActRcvFilShaDegDZ,
   [Brw_ADMI_SHR_CTR] = ActRcvFilShaCtrDZ,
   [Brw_ADMI_SHR_INS] = ActRcvFilShaInsDZ,
   [Brw_ADMI_TCH_CRS] = ActRcvFilTchCrsDZ,
   [Brw_ADMI_TCH_GRP] = ActRcvFilTchGrpDZ,
   [Brw_ADMI_DOC_PRJ] = ActRcvFilDocPrjDZ,
   [Brw_ADMI_ASS_PRJ] = ActRcvFilAssPrjDZ,
  };

static const Act_Action_t Brw_ActUploadFileClassic[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActUnk,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActRcvFilDocCrsCla,
   [Brw_ADMI_SHR_CRS] = ActRcvFilShaCrsCla,
   [Brw_ADMI_SHR_GRP] = ActRcvFilShaGrpCla,
   [Brw_ADMI_WRK_USR] = ActRcvFilWrkUsrCla,
   [Brw_ADMI_WRK_CRS] = ActRcvFilWrkCrsCla,
   [Brw_ADMI_MRK_CRS] = ActRcvFilMrkCrsCla,
   [Brw_ADMI_BRF_USR] = ActRcvFilBrfCla,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActRcvFilDocGrpCla,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActRcvFilMrkGrpCla,
   [Brw_ADMI_ASG_USR] = ActRcvFilAsgUsrCla,
   [Brw_ADMI_ASG_CRS] = ActRcvFilAsgCrsCla,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActRcvFilDocDegCla,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActRcvFilDocCtrCla,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActRcvFilDocInsCla,
   [Brw_ADMI_SHR_DEG] = ActRcvFilShaDegCla,
   [Brw_ADMI_SHR_CTR] = ActRcvFilShaCtrCla,
   [Brw_ADMI_SHR_INS] = ActRcvFilShaInsCla,
   [Brw_ADMI_TCH_CRS] = ActRcvFilTchCrsCla,
   [Brw_ADMI_TCH_GRP] = ActRcvFilTchGrpCla,
   [Brw_ADMI_DOC_PRJ] = ActRcvFilDocPrjCla,
   [Brw_ADMI_ASS_PRJ] = ActRcvFilAssPrjCla,
  };

static const Act_Action_t Brw_ActRefreshAfterUploadFiles[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActUnk,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActAdmDocCrs,
   [Brw_ADMI_SHR_CRS] = ActAdmShaCrs,
   [Brw_ADMI_SHR_GRP] = ActAdmShaGrp,
   [Brw_ADMI_WRK_USR] = ActAdmAsgWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActAdmAsgWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActAdmMrkCrs,
   [Brw_ADMI_BRF_USR] = ActAdmBrf,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActAdmDocGrp,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActAdmMrkGrp,
   [Brw_ADMI_ASG_USR] = ActAdmAsgWrkUsr,
   [Brw_ADMI_ASG_CRS] = ActAdmAsgWrkCrs,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActAdmDocDeg,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActAdmDocCtr,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActAdmDocIns,
   [Brw_ADMI_SHR_DEG] = ActAdmShaDeg,
   [Brw_ADMI_SHR_CTR] = ActAdmShaCtr,
   [Brw_ADMI_SHR_INS] = ActAdmShaIns,
   [Brw_ADMI_TCH_CRS] = ActAdmTchCrs,
   [Brw_ADMI_TCH_GRP] = ActAdmTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActAdmDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActAdmAssPrj,
  };

static const Act_Action_t Brw_ActExpandFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActExpSeeDocCrs,
   [Brw_SHOW_MRK_CRS] = ActExpSeeMrkCrs,
   [Brw_ADMI_DOC_CRS] = ActExpAdmDocCrs,
   [Brw_ADMI_SHR_CRS] = ActExpShaCrs,
   [Brw_ADMI_SHR_GRP] = ActExpShaGrp,
   [Brw_ADMI_WRK_USR] = ActExpWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActExpWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActExpAdmMrkCrs,
   [Brw_ADMI_BRF_USR] = ActExpBrf,
   [Brw_SHOW_DOC_GRP] = ActExpSeeDocGrp,
   [Brw_ADMI_DOC_GRP] = ActExpAdmDocGrp,
   [Brw_SHOW_MRK_GRP] = ActExpSeeMrkGrp,
   [Brw_ADMI_MRK_GRP] = ActExpAdmMrkGrp,
   [Brw_ADMI_ASG_USR] = ActExpAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActExpAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActExpSeeDocDeg,
   [Brw_ADMI_DOC_DEG] = ActExpAdmDocDeg,
   [Brw_SHOW_DOC_CTR] = ActExpSeeDocCtr,
   [Brw_ADMI_DOC_CTR] = ActExpAdmDocCtr,
   [Brw_SHOW_DOC_INS] = ActExpSeeDocIns,
   [Brw_ADMI_DOC_INS] = ActExpAdmDocIns,
   [Brw_ADMI_SHR_DEG] = ActExpShaDeg,
   [Brw_ADMI_SHR_CTR] = ActExpShaCtr,
   [Brw_ADMI_SHR_INS] = ActExpShaIns,
   [Brw_ADMI_TCH_CRS] = ActExpTchCrs,
   [Brw_ADMI_TCH_GRP] = ActExpTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActExpDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActExpAssPrj,
  };

static const Act_Action_t Brw_ActContractFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActConSeeDocCrs,
   [Brw_SHOW_MRK_CRS] = ActConSeeMrkCrs,
   [Brw_ADMI_DOC_CRS] = ActConAdmDocCrs,
   [Brw_ADMI_SHR_CRS] = ActConShaCrs,
   [Brw_ADMI_SHR_GRP] = ActConShaGrp,
   [Brw_ADMI_WRK_USR] = ActConWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActConWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActConAdmMrkCrs,
   [Brw_ADMI_BRF_USR] = ActConBrf,
   [Brw_SHOW_DOC_GRP] = ActConSeeDocGrp,
   [Brw_ADMI_DOC_GRP] = ActConAdmDocGrp,
   [Brw_SHOW_MRK_GRP] = ActConSeeMrkGrp,
   [Brw_ADMI_MRK_GRP] = ActConAdmMrkGrp,
   [Brw_ADMI_ASG_USR] = ActConAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActConAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActConSeeDocDeg,
   [Brw_ADMI_DOC_DEG] = ActConAdmDocDeg,
   [Brw_SHOW_DOC_CTR] = ActConSeeDocCtr,
   [Brw_ADMI_DOC_CTR] = ActConAdmDocCtr,
   [Brw_SHOW_DOC_INS] = ActConSeeDocIns,
   [Brw_ADMI_DOC_INS] = ActConAdmDocIns,
   [Brw_ADMI_SHR_DEG] = ActConShaDeg,
   [Brw_ADMI_SHR_CTR] = ActConShaCtr,
   [Brw_ADMI_SHR_INS] = ActConShaIns,
   [Brw_ADMI_TCH_CRS] = ActConTchCrs,
   [Brw_ADMI_TCH_GRP] = ActConTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActConDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActConAssPrj,
  };

static const Act_Action_t Brw_ActRecDatFile[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActUnk,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActChgDatAdmDocCrs,
   [Brw_ADMI_SHR_CRS] = ActChgDatShaCrs,
   [Brw_ADMI_SHR_GRP] = ActChgDatShaGrp,
   [Brw_ADMI_WRK_USR] = ActChgDatWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActChgDatWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActChgDatAdmMrkCrs,
   [Brw_ADMI_BRF_USR] = ActChgDatBrf,
   [Brw_SHOW_DOC_GRP] = ActUnk,
   [Brw_ADMI_DOC_GRP] = ActChgDatAdmDocGrp,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActChgDatAdmMrkGrp,
   [Brw_ADMI_ASG_USR] = ActChgDatAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActChgDatAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActUnk,
   [Brw_ADMI_DOC_DEG] = ActChgDatAdmDocDeg,
   [Brw_SHOW_DOC_CTR] = ActUnk,
   [Brw_ADMI_DOC_CTR] = ActChgDatAdmDocCtr,
   [Brw_SHOW_DOC_INS] = ActUnk,
   [Brw_ADMI_DOC_INS] = ActChgDatAdmDocIns,
   [Brw_ADMI_SHR_DEG] = ActChgDatShaDeg,
   [Brw_ADMI_SHR_CTR] = ActChgDatShaCtr,
   [Brw_ADMI_SHR_INS] = ActChgDatShaIns,
   [Brw_ADMI_TCH_CRS] = ActChgDatTchCrs,
   [Brw_ADMI_TCH_GRP] = ActChgDatTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActChgDatDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActChgDatAssPrj,
  };

static const Act_Action_t Brw_ActZIPFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_UNKNOWN     ] = ActUnk,
   [Brw_SHOW_DOC_CRS] = ActZIPSeeDocCrs,
   [Brw_SHOW_MRK_CRS] = ActUnk,
   [Brw_ADMI_DOC_CRS] = ActZIPAdmDocCrs,
   [Brw_ADMI_SHR_CRS] = ActZIPShaCrs,
   [Brw_ADMI_SHR_GRP] = ActZIPShaGrp,
   [Brw_ADMI_WRK_USR] = ActZIPWrkUsr,
   [Brw_ADMI_WRK_CRS] = ActZIPWrkCrs,
   [Brw_ADMI_MRK_CRS] = ActZIPAdmMrkCrs,
   [Brw_ADMI_BRF_USR] = ActZIPBrf,
   [Brw_SHOW_DOC_GRP] = ActZIPSeeDocGrp,
   [Brw_ADMI_DOC_GRP] = ActZIPAdmDocGrp,
   [Brw_SHOW_MRK_GRP] = ActUnk,
   [Brw_ADMI_MRK_GRP] = ActZIPAdmMrkGrp,
   [Brw_ADMI_ASG_USR] = ActZIPAsgUsr,
   [Brw_ADMI_ASG_CRS] = ActZIPAsgCrs,
   [Brw_SHOW_DOC_DEG] = ActZIPSeeDocDeg,
   [Brw_ADMI_DOC_DEG] = ActZIPAdmDocDeg,
   [Brw_SHOW_DOC_CTR] = ActZIPSeeDocCtr,
   [Brw_ADMI_DOC_CTR] = ActZIPAdmDocCtr,
   [Brw_SHOW_DOC_INS] = ActZIPSeeDocIns,
   [Brw_ADMI_DOC_INS] = ActZIPAdmDocIns,
   [Brw_ADMI_SHR_DEG] = ActZIPShaDeg,
   [Brw_ADMI_SHR_CTR] = ActZIPShaCtr,
   [Brw_ADMI_SHR_INS] = ActZIPShaIns,
   [Brw_ADMI_TCH_CRS] = ActZIPTchCrs,
   [Brw_ADMI_TCH_GRP] = ActZIPTchGrp,
   [Brw_ADMI_DOC_PRJ] = ActZIPDocPrj,
   [Brw_ADMI_ASS_PRJ] = ActZIPAssPrj,
  };

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

bool Brw_ICanEditFileOrFolder;	// Can I modify (remove, rename, create inside, etc.) a file or folder?

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static long Brw_GetGrpSettings (void);
static void Brw_GetDataCurrentGrp (void);

static void Brw_GetParsPathInTreeAndFileName (void);
static void Brw_SetPathFileBrowser (void);
static void Brw_CreateFoldersAssignmentsIfNotExist (long ZoneUsrCod);

static void Brw_AskEditWorksCrsInternal (__attribute__((unused)) void *Args);
static void Brw_ShowFileBrowsersAsgWrkCrs (void);
static void Brw_ShowFileBrowsersAsgWrkUsr (void);

static void Brw_FormToChangeCrsGrpZone (void);
static void Brw_GetSelectedGroupData (struct GroupData *GrpDat,bool AbortOnError);
static void Brw_ShowDataOwnerAsgWrk (struct Usr_Data *UsrDat);
static void Brw_ShowFileBrowserOrWorksInternal (__attribute__((unused)) void *Args);
static void Brw_ShowFileBrowser (void);
static void Brw_PutIconsFileBrowser (__attribute__((unused)) void *Args);
static void Brw_PutIconShowFigure (__attribute__((unused)) void *Args);
static void Brw_WriteTopBeforeShowingFileBrowser (void);
static void Brw_UpdateLastAccess (void);
static void Brw_WriteSubtitleOfFileBrowser (void);
static void Brw_InitHiddenLevels (void);

static void Brw_PutCheckboxFullTree (void);
static void Brw_PutParsFullTree (void);
static bool Brw_GetFullTreeFromForm (void);

static void Brw_GetAndUpdateDateLastAccFileBrowser (void);
static long Brw_GetGrpLastAccZone (const char *FieldNameDB);

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
                                       HidVis_HiddenOrVisible_t HiddenOrVisible);
static void Brw_PutIconToContractFolder (const char *FileBrowserId,const char *RowId,
                                         HidVis_HiddenOrVisible_t HiddenOrVisible);

static void Brw_PutIconHideUnhide (const char *Anchor,
                                   HidVis_HiddenOrVisible_t HiddenOrVisible);
static bool Brw_CheckIfAnyHigherLevelIsHidden (unsigned CurrentLevel);

static void Brw_PutIconFolder (unsigned Level,
                               const char *FileBrowserId,const char *RowId,
                               Brw_IconTree_t IconSubtree);
static void Brw_PutIconFolderWithoutPlus (const char *FileBrowserId,const char *RowId,
			                  bool Open,HidVis_HiddenOrVisible_t HiddenOrVisible);
static void Brw_PutIconFolderWithPlus (const char *FileBrowserId,const char *RowId,
				       bool Open,HidVis_HiddenOrVisible_t HiddenOrVisible);

static void Brw_PutIconNewFileOrFolder (void);
static void Brw_PutIconFileWithLinkToViewMetadata (const struct Brw_FileMetadata *FileMetadata);

static void Brw_PutButtonToDownloadZIPOfAFolder (void);

static void Brw_WriteFileName (unsigned Level,bool IsPublic,
			       const char *TxtStyle,const char *InputStyle);
static void Brw_GetFileNameToShowDependingOnLevel (Brw_FileBrowser_t FileBrowser,
                                                   unsigned Level,
                                                   Brw_FileType_t FileType,
                                                   const char *FileName,
                                                   char *FileNameToShow);
static void Brw_GetFileNameToShow (Brw_FileType_t FileType,
                                   const char FileName[NAME_MAX + 1],
                                   char FileNameToShow[NAME_MAX + 1]);
static void Brw_WriteFileSizeAndDate (struct Brw_FileMetadata *FileMetadata,
				      const char *TxtStyle);
static void Brw_WriteFileOrFolderPublisher (unsigned Level,long UsrCod);
static void Brw_AskConfirmRemoveFolderNotEmpty (void);

static void Brw_WriteCurrentClipboard (void);

static bool Brw_GetMyClipboard (void);
static bool Brw_CheckIfClipboardIsInThisTree (void);

static void Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (const char Path[PATH_MAX + 1]);
static void Brw_RemThisFolderAndUpdOtherFoldersFromExpandedFolders (const char Path[PATH_MAX + 1]);

static void Brw_PasteClipboard (struct BrwSiz_BrowserSize *Size);
static unsigned Brw_NumLevelsInPath (const char Path[PATH_MAX + 1]);
static bool Brw_PasteTreeIntoFolder (struct BrwSiz_BrowserSize *Size,
                                     unsigned LevelOrg,
                                     const char PathOrg[PATH_MAX + 1],
                                     const char PathDstInTree[PATH_MAX + 1],
                                     struct Brw_NumObjects *Pasted,
                                     long *FirstFilCod);
static void Brw_PutFormToCreateAFolder (const char FileNameToShow[NAME_MAX + 1]);
static void Brw_PutFormToUploadFilesUsingDropzone (const char *FileNameToShow);
static void Brw_PutFormToUploadOneFileClassic (const char *FileNameToShow);
static void Brw_PutFormToPasteAFileOrFolder (const char *FileNameToShow);
static void Brw_PutFormToCreateALink (const char *FileNameToShow);
static bool Brw_RcvFileInFileBrw (struct BrwSiz_BrowserSize *Size,
                                  Brw_UploadType_t UploadType);
static bool Brw_CheckIfUploadIsAllowed (const char *FileType);

static void Brw_PutIconToGetLinkToFile (void *FileMetadata);
static void Brw_PutParsToGetLinkToFile (void *FileMetadata);

static bool Brw_CheckIfICanEditFileMetadata (long IAmTheOwner);
static bool Brw_CheckIfIAmOwnerOfFile (long PublisherUsrCod);
static void Brw_WriteBigLinkToDownloadFile (const char *URL,
                                            struct Brw_FileMetadata *FileMetadata,
                                            const char *FileNameToShow);
static void Brw_WriteSmallLinkToDownloadFile (const char *URL,
	                                      struct Brw_FileMetadata *FileMetadata,
                                              const char *FileNameToShow);
static bool Brw_GetParPublicFile (void);
static Brw_License_t Brw_GetParLicense (void);

static void Brw_ResetFileMetadata (struct Brw_FileMetadata *FileMetadata);
static void Brw_GetFileMetadataFromRow (MYSQL_RES *mysql_res,
                                        struct Brw_FileMetadata *FileMetadata);
static void Brw_GetFileViewsFromLoggedUsrs (struct Brw_FileMetadata *FileMetadata);
static unsigned Brw_GetFileViewsFromMe (long FilCod);

static void Brw_RemoveOneFileOrFolderFromDB (const char Path[PATH_MAX + 1]);
static void Brw_RemoveChildrenOfFolderFromDB (const char Path[PATH_MAX + 1]);

static void Brw_SetIfICanEditFileOrFolder (bool Value);
static bool Brw_GetIfICanEditFileOrFolder (void);
static bool Brw_CheckIfICanEditFileOrFolder (unsigned Level);

static bool Brw_CheckIfICanCreateIntoFolder (unsigned Level);
static bool Brw_CheckIfICanModifySharedFileOrFolder (void);
static bool Brw_CheckIfICanModifyPrivateFileOrFolder (void);
static bool Brw_CheckIfICanModifyPrjDocFileOrFolder (void);
static bool Brw_CheckIfICanModifyPrjAssFileOrFolder (void);

static void Brw_WriteRowDocData (unsigned *NumDocsNotHidden,MYSQL_ROW row);

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

static void Brw_WriteStatsFileZonesTableHead1 (void);
static void Brw_WriteStatsFileZonesTableHead2 (void);
static void Brw_WriteStatsFileZonesTableHead3 (void);
static void Brw_WriteRowStatsFileBrowsers1 (const char *NameOfFileZones,
					    Brw_FileBrowser_t FileZone,
                                            struct BrwSiz_SizeOfFileZone *SizeOfFileZone);
static void Brw_WriteRowStatsFileBrowsers2 (const char *NameOfFileZones,
					    Brw_FileBrowser_t FileZone,
                                            struct BrwSiz_SizeOfFileZone *SizeOfFileZone);
static void Brw_WriteRowStatsFileBrowsers3 (const char *NameOfFileZones,
					    Brw_FileBrowser_t FileZone,
                                            struct BrwSiz_SizeOfFileZone *SizeOfFileZone);

static void Brw_GetNumberOfOERs (Brw_License_t License,
                                 unsigned long NumFiles[2]);

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
      case ActUnhDocIns:
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

      /***** Documents of center *****/
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
      case ActUnhDocCtr:
      case ActHidDocCtr:
      case ActReqDatAdmDocCtr:
      case ActChgDatAdmDocCtr:
      case ActDowAdmDocCtr:
	 Gbl.FileBrowser.Type = Brw_ADMI_DOC_CTR;
         break;

      /***** Shared files of center *****/
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
      case ActUnhDocDeg:
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
      case ActReqLnkSeeDocCrs:
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
      case ActUnhDocCrs:
      case ActHidDocCrs:
      case ActReqDatAdmDocCrs:
      case ActChgDatAdmDocCrs:
      case ActReqLnkAdmDocCrs:
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
      case ActUnhDocGrp:
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
      case ActSeeOnePrj:
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
      case ActChgPrjSco:
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
	       Err_WrongRoleExit ();
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
      case ActReqLnkSeeMrkCrs:
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
      case ActUnhMrkCrs:
      case ActHidMrkCrs:
      case ActReqDatAdmMrkCrs:
      case ActChgDatAdmMrkCrs:
      case ActReqLnkAdmMrkCrs:
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
      case ActUnhMrkGrp:
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
         Err_WrongFileBrowserExit ();
         break;
     }

   /***** Get the path in the file browser and the name of the file or folder *****/
   Brw_GetParsPathInTreeAndFileName ();
   Brw_SetFullPathInTree ();

   /***** Get other parameters *****/
   if (Brw_TypeIsAdmPrj[Gbl.FileBrowser.Type])
      /* Get project code */
      Prj_SetPrjCod (ParCod_GetPar (ParCod_Prj));
   else if (Brw_TypeIsAdmCrsAsgWrk[Gbl.FileBrowser.Type])
     {
      /* Get lists of the selected users if not already got */
      Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
      /* Get user whose folder will be used to make any operation */
      Usr_GetParOtherUsrCodEncryptedAndGetListIDs ();
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
	 Par_GetParText ("NewFolderName",Gbl.FileBrowser.NewFilFolLnkName,NAME_MAX);
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
	 Par_GetParText ("NewLinkName",Gbl.FileBrowser.NewFilFolLnkName,NAME_MAX);
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
	    Gbl.FileBrowser.ShowOnlyPublicFiles = !Gbl.Usrs.Me.IBelongToCurrent[Hie_INS];
	    break;
	 case Brw_SHOW_DOC_CTR:
	 case Brw_ADMI_DOC_CTR:
	 case Brw_ADMI_SHR_CTR:
	    Gbl.FileBrowser.ShowOnlyPublicFiles = !Gbl.Usrs.Me.IBelongToCurrent[Hie_CTR];
	    break;
	 case Brw_SHOW_DOC_DEG:
	 case Brw_ADMI_DOC_DEG:
	 case Brw_ADMI_SHR_DEG:
	    Gbl.FileBrowser.ShowOnlyPublicFiles = !Gbl.Usrs.Me.IBelongToCurrent[Hie_DEG];
	    break;
	 case Brw_SHOW_DOC_CRS:
	 case Brw_ADMI_DOC_CRS:
	 case Brw_ADMI_SHR_CRS:
	    Gbl.FileBrowser.ShowOnlyPublicFiles = !Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS];
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

   if ((GrpCod = ParCod_GetPar (ParCod_Grp)) > 0)
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
      Grp_GetGroupDataByCod (&GrpDat);

      switch (Gbl.Action.Act)
	{
	 case ActSeeAdmDocCrsGrp:	// Access to see/admin a documents zone from menu

	 case ActChgToSeeDocCrs:	// Access to see a documents zone
	 case ActSeeDocGrp:		// Access to see a documents zone

	 case ActChgToAdmDocCrs:	// Access to admin a documents zone
	 case ActAdmDocGrp:		// Access to admin a documents zone

	 case ActAdmTchCrsGrp:		// Access to admin a teachers' zone from menu
	 case ActChgToAdmTch:		// Access to admin a teachers' zone
	 case ActAdmTchGrp:		// Access to admin a teachers' zone

	 case ActAdmShaCrsGrp:		// Access to admin a shared zone from menu
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
	       Err_ShowErrorAndExit ("The group has no file zones.");
	    else if (!Grp_GetIfIBelongToGrp (Gbl.Crs.Grps.GrpCod))
	       Err_ShowErrorAndExit ("You don't have access to the group.");
	    break;
	}

      /***** Get data of the current group *****/
      if (Gbl.Crs.Grps.GrpCod > 0)
	{
	 Gbl.Crs.Grps.GrpTyp.GrpTypCod         = GrpDat.GrpTypCod;
	 Str_Copy (Gbl.Crs.Grps.GrpTyp.GrpTypName,GrpDat.GrpTypName,
	           sizeof (Gbl.Crs.Grps.GrpTyp.GrpTypName) - 1);
	 Str_Copy (Gbl.Crs.Grps.GrpName,GrpDat.GrpName,
	           sizeof (Gbl.Crs.Grps.GrpName) - 1);
	 Gbl.Crs.Grps.MaxStudents              = GrpDat.MaxStudents;
	 Gbl.Crs.Grps.Open                     = GrpDat.Open;
	 Gbl.Crs.Grps.FileZones                = GrpDat.FileZones;
	 Gbl.Crs.Grps.GrpTyp.MultipleEnrolment = GrpDat.MultipleEnrolment;
	}
     }
  }

/*****************************************************************************/
/**************** Write parameters related with file browser *****************/
/*****************************************************************************/

void Brw_PutImplicitParsFileBrowser (void *FilFolLnk)
  {
   if (FilFolLnk)
      Brw_PutParsFileBrowser (((struct Brw_FilFolLnk *) FilFolLnk)->Path,
			      ((struct Brw_FilFolLnk *) FilFolLnk)->Name,
			      ((struct Brw_FilFolLnk *) FilFolLnk)->Type,
			      -1L);	// Not used
  }

void Brw_PutParsFileBrowser (const char *PathInTree,const char *FilFolLnkName,
                             Brw_FileType_t FileType,long FilCod)
  {
   if (Brw_TypeIsGrpBrw[Gbl.FileBrowser.Type])		// This file browser needs specify a group
      /***** Group code *****/
      ParCod_PutPar (ParCod_Grp,Gbl.Crs.Grps.GrpCod);
   else if (Brw_TypeIsAdmPrj[Gbl.FileBrowser.Type])	// This file browser needs specify a project
      /***** Project code *****/
      ParCod_PutPar (ParCod_Prj,Prj_GetPrjCod ());
   else if (Brw_TypeIsAdmCrsAsgWrk[Gbl.FileBrowser.Type])
     {
      /***** Users selected *****/
      Usr_PutParSelectedUsrsCods (&Gbl.Usrs.Selected);
      Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
     }

   /***** If full tree selected? *****/
   Brw_PutParFullTreeIfSelected (&Gbl.FileBrowser.FullTree);

   /***** Path and file *****/
   if (PathInTree)
      Par_PutParString (NULL,"Path",PathInTree);
   if (FilFolLnkName)
      Par_PutParString (NULL,Brw_FileTypeParName[FileType],FilFolLnkName);
   ParCod_PutPar (ParCod_Fil,FilCod);
  }

/*****************************************************************************/
/************** Get parameters path and file in file browser *****************/
/*****************************************************************************/

static void Brw_GetParsPathInTreeAndFileName (void)
  {
   const char *Ptr;
   Brw_FileType_t FileType;
   char FileNameToShow[NAME_MAX + 1];

   /***** Get the path inside the tree
          (this path does not include
           the name of the file or folder at the end) *****/
   Par_GetParText ("Path",Gbl.FileBrowser.FilFolLnk.Path,PATH_MAX);

   /* Check if path contains ".." */
   if (strstr (Gbl.FileBrowser.FilFolLnk.Path,".."))	// ".." is not allowed in path
      Err_ShowErrorAndExit ("Wrong path.");

   /***** Get the name of the file, folder or link *****/
   Gbl.FileBrowser.FilFolLnk.Type = Brw_IS_UNKNOWN;
   for (FileType  = (Brw_FileType_t) 0;
	FileType <= (Brw_FileType_t) (Brw_NUM_FILE_TYPES - 1);
	FileType++)
      // File names with heading and trailing spaces are allowed
      if (Par_GetParAndChangeFormat (Brw_FileTypeParName[FileType],
                                     Gbl.FileBrowser.FilFolLnk.Name,
                                     NAME_MAX,Str_TO_TEXT,
                                     Str_DONT_REMOVE_SPACES))
	{
	 Gbl.FileBrowser.FilFolLnk.Type = FileType;

	 /* Check if filename contains ".." */
	 Brw_GetFileNameToShow (Gbl.FileBrowser.FilFolLnk.Type,
	                        Gbl.FileBrowser.FilFolLnk.Name,
                                FileNameToShow);
	 if (strstr (FileNameToShow,".."))	// ".." is not allowed in filename
	    Err_ShowErrorAndExit ("Wrong file name.");
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
   if (Gbl.FileBrowser.Level && Brw_TypeIsAdmAsg[Gbl.FileBrowser.Type])
     {
      Asg_SetFolder (&Gbl.FileBrowser.Asg,Gbl.FileBrowser.Level);
      Asg_GetAssignmentDataByFolder (&Gbl.FileBrowser.Asg);
     }
  }

/*****************************************************************************/
/************************* Initialize file browser ***************************/
/*****************************************************************************/
// Gbl.FileBrowser.Type must be set to a valid file browser

void Brw_InitializeFileBrowser (void)
  {
   struct BrwSiz_BrowserSize *Size = BrwSiz_GetSize ();

   /***** Construct the relative path to the folder of file browser *****/
   Brw_SetPathFileBrowser ();

   /***** Other initializations *****/
   BrwSiz_ResetFileBrowserSize (Size);
  }

/*****************************************************************************/
/********* Construct the paths to the top folders of file browser ************/
/*****************************************************************************/

static void Brw_SetPathFileBrowser (void)
  {
   long PrjCod;
   char Path[PATH_MAX * 2 + 128];

   /***** Reset paths. An empty path means that
          we don't have to create that directory *****/
   Gbl.FileBrowser.Path.AboveRootFolder[0] = '\0';
   Gbl.FileBrowser.Path.RootFolder[0] = '\0';

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
	 snprintf (Path,sizeof (Path),"%s/%02u",
		   Cfg_PATH_INS_PRIVATE,
		   (unsigned) (Gbl.Hierarchy.Node[Hie_INS].HieCod % 100));
	 Fil_CreateDirIfNotExists (Path);

	 /* Create path to the current institution */
	 snprintf (Gbl.FileBrowser.Path.AboveRootFolder,
	           sizeof (Gbl.FileBrowser.Path.AboveRootFolder),
	           "%s/%02u/%u",
		   Cfg_PATH_INS_PRIVATE,
		   (unsigned) (Gbl.Hierarchy.Node[Hie_INS].HieCod % 100),
		   (unsigned)  Gbl.Hierarchy.Node[Hie_INS].HieCod);
         break;
      case Brw_SHOW_DOC_CTR:
      case Brw_ADMI_DOC_CTR:
      case Brw_ADMI_SHR_CTR:
	 /* Create a directory for centers */
	 Fil_CreateDirIfNotExists (Cfg_PATH_CTR_PRIVATE);

	 /* Create a directory for all centers which codes end in
	    center-code mod 100 */
	 snprintf (Path,sizeof (Path),"%s/%02u",
		   Cfg_PATH_CTR_PRIVATE,
		   (unsigned) (Gbl.Hierarchy.Node[Hie_CTR].HieCod % 100));
	 Fil_CreateDirIfNotExists (Path);

	 /* Create path to the current center */
	 snprintf (Gbl.FileBrowser.Path.AboveRootFolder,
	           sizeof (Gbl.FileBrowser.Path.AboveRootFolder),
	           "%s/%02u/%u",
		   Cfg_PATH_CTR_PRIVATE,
		   (unsigned) (Gbl.Hierarchy.Node[Hie_CTR].HieCod % 100),
		   (unsigned)  Gbl.Hierarchy.Node[Hie_CTR].HieCod);
	 break;
      case Brw_SHOW_DOC_DEG:
      case Brw_ADMI_DOC_DEG:
      case Brw_ADMI_SHR_DEG:
	 /* Create a directory for degrees */
	 Fil_CreateDirIfNotExists (Cfg_PATH_DEG_PRIVATE);

	 /* Create a directory for all degrees which codes end in
	    degree-code mod 100 */
	 snprintf (Path,sizeof (Path),"%s/%02u",
		   Cfg_PATH_DEG_PRIVATE,
		   (unsigned) (Gbl.Hierarchy.Node[Hie_DEG].HieCod % 100));
	 Fil_CreateDirIfNotExists (Path);

         /* Create path to the current degree */
	 snprintf (Gbl.FileBrowser.Path.AboveRootFolder,
	           sizeof (Gbl.FileBrowser.Path.AboveRootFolder),
	           "%s/%02u/%u",
		   Cfg_PATH_DEG_PRIVATE,
		   (unsigned) (Gbl.Hierarchy.Node[Hie_DEG].HieCod % 100),
		   (unsigned)  Gbl.Hierarchy.Node[Hie_DEG].HieCod);
	 break;
      case Brw_SHOW_DOC_CRS:
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_TCH_CRS:
      case Brw_ADMI_SHR_CRS:
      case Brw_SHOW_MRK_CRS:
      case Brw_ADMI_MRK_CRS:
         /* Create path to the current course */
         Str_Copy (Gbl.FileBrowser.Path.AboveRootFolder,Gbl.Crs.Path.AbsPriv,
                   sizeof (Gbl.FileBrowser.Path.AboveRootFolder) - 1);
	 break;
      case Brw_SHOW_DOC_GRP:
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_GRP:
      case Brw_ADMI_SHR_GRP:
      case Brw_SHOW_MRK_GRP:
      case Brw_ADMI_MRK_GRP:
	 /* Create a directory for groups inside the current course */
         snprintf (Path,sizeof (Path),"%s/%s",
                   Gbl.Crs.Path.AbsPriv,Cfg_FOLDER_GRP);
         Fil_CreateDirIfNotExists (Path);

         /* Create path to this group */
         snprintf (Path,sizeof (Path),"%s/%s/%ld",
                   Gbl.Crs.Path.AbsPriv,Cfg_FOLDER_GRP,
                   Gbl.Crs.Grps.GrpCod);
         Str_Copy (Gbl.FileBrowser.Path.AboveRootFolder,Path,
                   sizeof (Gbl.FileBrowser.Path.AboveRootFolder) - 1);
	 break;
      case Brw_ADMI_ASG_USR:
      case Brw_ADMI_WRK_USR:
	 /* Create a directory for me inside the current course */
         snprintf (Path,sizeof (Path),"%s/%s",
                   Gbl.Crs.Path.AbsPriv,Cfg_FOLDER_USR);
         Fil_CreateDirIfNotExists (Path);

	 /* Create a directory for all users whose codes end in
	    my-user-code mod 100 */
         snprintf (Path,sizeof (Path),"%s/%s/%02u",
                   Gbl.Crs.Path.AbsPriv,Cfg_FOLDER_USR,
                   (unsigned) (Gbl.Usrs.Me.UsrDat.UsrCod % 100));
         Fil_CreateDirIfNotExists (Path);

         /* Create path to me */
         snprintf (Path,sizeof (Path),"%s/%s/%02u/%ld",
                   Gbl.Crs.Path.AbsPriv,Cfg_FOLDER_USR,
                   (unsigned) (Gbl.Usrs.Me.UsrDat.UsrCod % 100),
                   Gbl.Usrs.Me.UsrDat.UsrCod);
         Str_Copy (Gbl.FileBrowser.Path.AboveRootFolder,Path,
                   sizeof (Gbl.FileBrowser.Path.AboveRootFolder) - 1);
         break;
      case Brw_ADMI_ASG_CRS:
      case Brw_ADMI_WRK_CRS:
         if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
           {
	    /* Create a directory for this user inside the current course */
            snprintf (Path,sizeof (Path),"%s/%s",
        	      Gbl.Crs.Path.AbsPriv,Cfg_FOLDER_USR);
            Fil_CreateDirIfNotExists (Path);

	    /* Create a directory for all users whose codes end in
	       user-code mod 100 */
	    snprintf (Path,sizeof (Path),"%s/%s/%02u",
		      Gbl.Crs.Path.AbsPriv,Cfg_FOLDER_USR,
		      (unsigned) (Gbl.Usrs.Other.UsrDat.UsrCod % 100));
	    Fil_CreateDirIfNotExists (Path);

            /* Create path to user */
            snprintf (Path,sizeof (Path),"%s/%s/%02u/%ld",
        	      Gbl.Crs.Path.AbsPriv,Cfg_FOLDER_USR,
                      (unsigned) (Gbl.Usrs.Other.UsrDat.UsrCod % 100),
        	      Gbl.Usrs.Other.UsrDat.UsrCod);
            Str_Copy (Gbl.FileBrowser.Path.AboveRootFolder,Path,
                      sizeof (Gbl.FileBrowser.Path.AboveRootFolder) - 1);
           }
         break;
      case Brw_ADMI_DOC_PRJ:
      case Brw_ADMI_ASS_PRJ:
	 PrjCod = Prj_GetPrjCod ();

	 /* Create a directory for projects inside the current course */
         snprintf (Path,sizeof (Path),"%s/%s",
                   Gbl.Crs.Path.AbsPriv,Cfg_FOLDER_PRJ);
         Fil_CreateDirIfNotExists (Path);

	 /* Create a directory for all projects which codes end in
	    project-code mod 100 */
	 snprintf (Path,sizeof (Path),"%s/%s/%02u",
		   Gbl.Crs.Path.AbsPriv,Cfg_FOLDER_PRJ,
                   (unsigned) (PrjCod % 100));
	 Fil_CreateDirIfNotExists (Path);

         /* Create path to the current project */
         snprintf (Path,sizeof (Path),"%s/%s/%02u/%ld",
                   Gbl.Crs.Path.AbsPriv,Cfg_FOLDER_PRJ,
                   (unsigned) (PrjCod % 100),
                   PrjCod);
         Str_Copy (Gbl.FileBrowser.Path.AboveRootFolder,Path,
                   sizeof (Gbl.FileBrowser.Path.AboveRootFolder) - 1);
	 break;
      case Brw_ADMI_BRF_USR:
         Str_Copy (Gbl.FileBrowser.Path.AboveRootFolder,Gbl.Usrs.Me.PathDir,
                   sizeof (Gbl.FileBrowser.Path.AboveRootFolder) - 1);
	 break;
      default:
	 return;
     }

   /***** Create directories that not exist *****/
   if (Gbl.FileBrowser.Path.AboveRootFolder[0])
     {
      Fil_CreateDirIfNotExists (Gbl.FileBrowser.Path.AboveRootFolder);
      snprintf (Path,sizeof (Path),"%s/%s",
                Gbl.FileBrowser.Path.AboveRootFolder,
                Brw_RootFolderInternalNames[Gbl.FileBrowser.Type]);
      Str_Copy (Gbl.FileBrowser.Path.RootFolder,Path,
                sizeof (Gbl.FileBrowser.Path.RootFolder) - 1);
      Fil_CreateDirIfNotExists (Gbl.FileBrowser.Path.RootFolder);

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
   unsigned NumUsrs;
   unsigned NumUsr;
   long UsrCod;
   char PathFolder[PATH_MAX * 2 + 128];
   bool FolderExists = false;

   /***** Get all users belonging to current course *****/
   NumUsrs = Enr_DB_GetUsrsFromCurrentCrs (&mysql_res);

   /***** Check folders *****/
   for (NumUsr = 0;
	NumUsr < NumUsrs && !FolderExists;
	NumUsr++)
     {
      /* Get next user */
      UsrCod = DB_GetNextCode (mysql_res);

      /* Check if folder exists */
      snprintf (PathFolder,sizeof (PathFolder),"%s/usr/%02u/%ld/%s/%s",
                Gbl.Crs.Path.AbsPriv,
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
   unsigned NumFolders;
   unsigned NumFolder;
   char PathFolderAsg[PATH_MAX + 1 + PATH_MAX + 1];

   /***** Get assignment folders from database *****/
   NumFolders = Brw_DB_GetFoldersAssignments (&mysql_res,ZoneUsrCod);

   /***** Create one folder for each assignment *****/
   for (NumFolder = 0;
	NumFolder < NumFolders;
	NumFolder++)
     {
      /* Get next assignment with folder */
      row = mysql_fetch_row (mysql_res);
      if (row)
	 if (row[0])	// Not necessary, because folder name is checked in query to be not empty
	   {
	    /* Create folder if not exists */
	    snprintf (PathFolderAsg,sizeof (PathFolderAsg),"%s/%s",
		      Gbl.FileBrowser.Path.RootFolder,row[0]);
	    Fil_CreateDirIfNotExists (PathFolderAsg);
	   }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****** Update folders of assignments if exist for all users in course *******/
/*****************************************************************************/
// Folders are in level 1, just under root folder

bool Brw_UpdateFoldersAssigmentsIfExistForAllUsrs (const char *OldFolderName,
                                                   const char *NewFolderName)
  {
   extern const char *Txt_Can_not_rename_a_folder_of_assignment;
   extern const char *Txt_Users;
   extern const char *Txt_Folders_renamed;
   extern const char *Txt_Folders_not_renamed;
   MYSQL_RES *mysql_res;
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

   /***** Get all users belonging to current course *****/
   NumUsrs = Enr_DB_GetUsrsFromCurrentCrs (&mysql_res);

   /***** Check if there exist folders with the new name *****/
   for (NumUsr = 0;
	NumUsr < NumUsrs && RenamingIsPossible;
	NumUsr++)
     {
      /* Get next user */
      UsrCod = DB_GetNextCode (mysql_res);

      /* Rename folder if exists */
      snprintf (PathOldFolder,sizeof (PathOldFolder),"%s/usr/%02u/%ld/%s/%s",
                Gbl.Crs.Path.AbsPriv,
                (unsigned) (UsrCod % 100),
                UsrCod,	// User's code
                Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
                OldFolderName);
      snprintf (PathNewFolder,sizeof (PathNewFolder),"%s/usr/%02u/%ld/%s/%s",
                Gbl.Crs.Path.AbsPriv,
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
	 UsrCod = DB_GetNextCode (mysql_res);

         /* Rename folder if exists */
         snprintf (PathOldFolder,sizeof (PathOldFolder),"%s/usr/%02u/%ld/%s/%s",
                   Gbl.Crs.Path.AbsPriv,
                   (unsigned) (UsrCod % 100),
                   UsrCod,	// User's code
                   Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
                   OldFolderName);
         if (Fil_CheckIfPathExists (PathOldFolder))
           {
            snprintf (PathNewFolder,sizeof (PathNewFolder),"%s/usr/%02u/%ld/%s/%s",
                      Gbl.Crs.Path.AbsPriv,
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
               Brw_DB_RemoveAffectedClipboards (Brw_ADMI_ASG_USR,UsrCod,-1L);
               Brw_DB_RemoveAffectedClipboards (Brw_ADMI_ASG_CRS,-1L,UsrCod);

               /* Rename affected expanded folders */
               snprintf (OldPath,sizeof (OldPath),"%s/%s",
			 Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,OldFolderName);
               snprintf (NewPath,sizeof (NewPath),"%s/%s",
			 Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,NewFolderName);
               Brw_DB_RenameAffectedExpandedFolders (Brw_ADMI_ASG_USR,UsrCod,-1L,
        	                                     OldPath,NewPath);
               Brw_DB_RenameAffectedExpandedFolders (Brw_ADMI_ASG_CRS,-1L,UsrCod,
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
/******* Remove folders of assignments if exist for all users in course ******/
/*****************************************************************************/
// Folders are in level 1, just under root folder

void Brw_RemoveFoldersAssignmentsIfExistForAllUsrs (const char *FolderName)
  {
   MYSQL_RES *mysql_res;
   unsigned NumUsrs;
   unsigned NumUsr;
   long UsrCod;
   char PathFolder[PATH_MAX * 2 + 128];

   /***** Get all users belonging to current course *****/
   NumUsrs = Enr_DB_GetUsrsFromCurrentCrs (&mysql_res);

   /***** Remove folders *****/
   for (NumUsr = 0;
	NumUsr < NumUsrs;
	NumUsr++)
     {
      /* Get next user */
      UsrCod = DB_GetNextCode (mysql_res);

      /* Remove tree if exists */
      snprintf (PathFolder,sizeof (PathFolder),"%s/usr/%02u/%ld/%s/%s",
                Gbl.Crs.Path.AbsPriv,
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
/************** Request edition of works of users of the course **************/
/*****************************************************************************/

void Brw_AskEditWorksCrs (void)
  {
   Brw_AskEditWorksCrsInternal (NULL);
  }

static void Brw_AskEditWorksCrsInternal (__attribute__((unused)) void *Args)
  {
   extern const char *Hlp_FILES_Homework_for_teachers;
   extern const char *Txt_Assignments_and_other_works;
   extern const char *Txt_View_homework;

   /***** List users to select some of them *****/
   Usr_PutFormToSelectUsrsToGoToAct (&Gbl.Usrs.Selected,
				     ActAdmAsgWrkCrs,
				     NULL,NULL,
				     Txt_Assignments_and_other_works,
				     Hlp_FILES_Homework_for_teachers,
				     Txt_View_homework,
				     false);	// Do not put form with date range
  }

/*****************************************************************************/
/*********** Show normal file browser (not for assignments-works) ************/
/*****************************************************************************/

void Brw_ShowFileBrowserNormal (void)
  {
   /***** Write top before showing file browser *****/
   Brw_WriteTopBeforeShowingFileBrowser ();

   /****** Show the file browser *****/
   Brw_ShowFileBrowser ();

   /***** Put legal notice *****/
   Brw_PutLegalNotice ();
  }

/*****************************************************************************/
/************* Show file browser with the documents of a project *************/
/*****************************************************************************/

void Brw_ShowFileBrowserProject (long PrjCod)
  {
   extern const char *Txt_Documents;

   /***** Begin fieldset *****/
   HTM_FIELDSET_Begin (NULL);
      HTM_LEGEND (Txt_Documents);

      Brw_WriteTopBeforeShowingFileBrowser ();

      if (Prj_CheckIfICanViewProjectDocuments (PrjCod))
	{
	 /***** Show the tree with the project documents *****/
	 Gbl.FileBrowser.Type = Brw_ADMI_DOC_PRJ;
	 Brw_InitializeFileBrowser ();
	 Brw_ShowFileBrowser ();
	}

      if (Prj_CheckIfICanViewProjectAssessment (PrjCod))
	{
	 /***** Show the tree with the project assessment *****/
	 Gbl.FileBrowser.Type = Brw_ADMI_ASS_PRJ;
	 Brw_InitializeFileBrowser ();
	 Brw_ShowFileBrowser ();
	}

      /***** Put legal notice *****/
      Brw_PutLegalNotice ();

   /***** End fieldset *****/
   HTM_FIELDSET_End ();
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
   Box_BoxBegin (Txt_Assignments_and_other_works,Brw_PutIconShowFigure,NULL,
		 Hlp_FILES_Homework_for_teachers,Box_NOT_CLOSABLE);

      /***** List the assignments and works of the selected users *****/
      Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
      while (*Ptr)
	{
	 Par_GetNextStrUntilSeparParMult (&Ptr,Gbl.Usrs.Other.UsrDat.EnUsrCod,
					    Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
	 Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
						      Usr_DONT_GET_PREFS,
						      Usr_GET_ROLE_IN_CRS))
	    if (Usr_CheckIfICanViewAsgWrk (&Gbl.Usrs.Other.UsrDat))
	      {
	       Gbl.Usrs.Other.UsrDat.Accepted =
	       Enr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);

	       /***** Owner of the works *****/
	       Brw_ShowDataOwnerAsgWrk (&Gbl.Usrs.Other.UsrDat);

	       /***** File browsers *****/
	       HTM_DIV_Begin ("class=\"BROWSER_ASG_WRK\"");

		  /* File browser with the assignments */
		  Gbl.FileBrowser.Type = Brw_ADMI_ASG_CRS;
		  Brw_InitializeFileBrowser ();
		  Brw_ShowFileBrowser ();

		  /* File browser with the works */
		  Gbl.FileBrowser.Type = Brw_ADMI_WRK_CRS;
		  Brw_InitializeFileBrowser ();
		  Brw_ShowFileBrowser ();

	       HTM_DIV_End ();

	       HTM_BR ();
	       HTM_BR ();
	      }
	}

   /***** End table and box *****/
   Box_BoxEnd ();

   /***** Put legal notice *****/
   Brw_PutLegalNotice ();
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

   /***** Put legal notice *****/
   Brw_PutLegalNotice ();
  }

/*****************************************************************************/
/******************** Put legal notice about files hosted ********************/
/*****************************************************************************/

void Brw_PutLegalNotice (void)
  {
   extern const char *Txt_Disclaimer_the_files_hosted_here_;

   /***** Legal notice *****/
   Ale_ShowAlert (Ale_INFO,Txt_Disclaimer_the_files_hosted_here_,
                  Cfg_PLATFORM_SHORT_NAME,
                  Cfg_PLATFORM_RESPONSIBLE_EMAIL);
  }

/*****************************************************************************/
/**************** Form to change file zone (course or group) *****************/
/*****************************************************************************/

static void Brw_FormToChangeCrsGrpZone (void)
  {
   extern const char *Par_CodeStr[];
   struct ListCodGrps LstMyGrps;
   unsigned NumGrp;
   struct GroupData GrpDat;

   /***** Get list of groups to show *****/
   if (Gbl.Crs.Grps.NumGrps)	// This course has groups?
      /* Get list of group with file zones which I belong to */
      Grp_GetLstCodGrpsWithFileZonesIBelong (&LstMyGrps);

   /***** Begin form *****/
   Frm_BeginForm (Brw_ActChgZone[Gbl.FileBrowser.Type]);
      Brw_PutParFullTreeIfSelected (&Gbl.FileBrowser.FullTree);

      /***** List start *****/
      HTM_UL_Begin ("class=\"LIST_LEFT\"");

	 /***** Select the complete course, not a group *****/
	 HTM_LI_Begin ("class=\"%s\"",
	               Brw_TypeIsCrsBrw[Gbl.FileBrowser.Type] ? "BROWSER_TITLE" :
						                "BROWSER_TITLE_LIGHT");
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_RADIO (Par_CodeStr[ParCod_Grp],HTM_SUBMIT_ON_CLICK,
				"value=\"-1\"%s",
				Brw_TypeIsCrsBrw[Gbl.FileBrowser.Type] ? " checked=\"checked\"" :
									 "");
	       HTM_Txt (Gbl.Hierarchy.Node[Hie_CRS].FullName);
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
	       Grp_GetGroupDataByCod (&GrpDat);

	       /* Select this group */
	       HTM_LI_Begin ("class=\"%s\"",
			     (Brw_TypeIsGrpBrw[Gbl.FileBrowser.Type] &&
			      GrpDat.GrpCod == Gbl.Crs.Grps.GrpCod) ? "BROWSER_TITLE" :
								      "BROWSER_TITLE_LIGHT");
		  HTM_IMG (Cfg_URL_ICON_PUBLIC,
			   NumGrp < LstMyGrps.NumGrps - 1 ? "submid20x20.gif" :
							    "subend20x20.gif",
			   NULL,
			   "class=\"ICO25x25\" style=\"margin-left:6px;\"");
		  HTM_LABEL_Begin (NULL);
		     HTM_INPUT_RADIO (Par_CodeStr[ParCod_Grp],HTM_SUBMIT_ON_CLICK,
				      "value=\"%ld\"%s",
				      GrpDat.GrpCod,
				      (Brw_TypeIsGrpBrw[Gbl.FileBrowser.Type] &&
				       GrpDat.GrpCod == Gbl.Crs.Grps.GrpCod) ? " checked=\"checked\"" :
					                                       "");
		     HTM_TxtF ("%s&nbsp;%s",GrpDat.GrpTypName,GrpDat.GrpName);
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
      Grp_GetGroupDataByCod (GrpDat);

      /***** For security, check if group file zones are enabled,
             and check if I belongs to the selected group *****/
      if (!GrpDat->FileZones)
        {
         if (AbortOnError)
            Err_ShowErrorAndExit ("The file browser is disabled.");
         GrpDat->GrpCod = -1L;
        }
      else if (!Grp_GetIfIBelongToGrp (GrpDat->GrpCod))
        {
         if (AbortOnError)
            Err_NoPermissionExit ();
         GrpDat->GrpCod = -1L;
        }
     }
  }

/*****************************************************************************/
/******** Show a row with the data of the owner of assignments/works *********/
/*****************************************************************************/

static void Brw_ShowDataOwnerAsgWrk (struct Usr_Data *UsrDat)
  {
   extern const char *Txt_View_record_for_this_course;
   static const Act_Action_t NextAction[Rol_NUM_ROLES] =
     {
      [Rol_STD] = ActSeeRecOneStd,
      [Rol_NET] = ActSeeRecOneTch,
      [Rol_TCH] = ActSeeRecOneTch,
     };
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC60x80",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE60x80",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO60x80",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR60x80",
     };

   /***** Show user's photo *****/
   HTM_DIV_Begin ("class=\"OWNER_WORKS_PHOTO\"");
      Pho_ShowUsrPhotoIfAllowed (UsrDat,
                                 ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
   HTM_DIV_End ();

   /***** Begin form to send a message to this user *****/
   HTM_DIV_Begin ("class=\"OWNER_WORKS_DATA MSG_AUT_%s\"",The_GetSuffix ());

      if (!NextAction[UsrDat->Roles.InCurrentCrs])
	 Err_WrongRoleExit ();
      Frm_BeginForm (NextAction[UsrDat->Roles.InCurrentCrs]);
	 Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);

	 /***** Show user's ID *****/
	 ID_WriteUsrIDs (UsrDat,NULL);

	 /***** Show user's name *****/
	 HTM_BR ();

	 HTM_BUTTON_Submit_Begin (Txt_View_record_for_this_course,
				  "class=\"BT_LINK\"");
	    HTM_Txt (UsrDat->Surname1);
	    if (UsrDat->Surname2[0])
	       HTM_TxtF ("&nbsp;%s",UsrDat->Surname2);
	    if (UsrDat->FrstName[0])
	       HTM_TxtF (", %s",UsrDat->FrstName);
	 HTM_BUTTON_End ();

	 /***** Show user's email *****/
	 if (UsrDat->Email[0])
	   {
	    HTM_BR ();
	    HTM_A_Begin ("href=\"mailto:%s\" target=\"_blank\""
			 " class=\"MSG_AUT_%s\"",
			 UsrDat->Email,The_GetSuffix ());
	       HTM_Txt (UsrDat->Email);
	    HTM_A_End ();
	   }

      Frm_EndForm ();

   HTM_DIV_End ();
  }

/*****************************************************************************/
/******* Get and check list of selected users, and show users' works  ********/
/*****************************************************************************/

void Brw_GetSelectedUsrsAndShowWorks (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (&Gbl.Usrs.Selected,
				  Brw_ShowFileBrowserOrWorksInternal,NULL,	// when user(s) selected
                                  Brw_AskEditWorksCrsInternal,NULL);		// when no user selected
  }

/*****************************************************************************/
/******************** Show a file browser or users' works  *******************/
/*****************************************************************************/

void Brw_ShowFileBrowserOrWorks (void)
  {
   Brw_ShowFileBrowserOrWorksInternal (NULL);
  }

static void Brw_ShowFileBrowserOrWorksInternal (__attribute__((unused)) void *Args)
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
   if (Brw_TypeIsAdmUsrAsgWrk[Gbl.FileBrowser.Type])
      Brw_ShowFileBrowsersAsgWrkUsr ();
   else if (Brw_TypeIsAdmCrsAsgWrk[Gbl.FileBrowser.Type])
      Brw_ShowFileBrowsersAsgWrkCrs ();
   else if (Brw_TypeIsAdmPrj[Gbl.FileBrowser.Type])
      Prj_ShowOneProject ();
   else
      Brw_ShowFileBrowserNormal ();
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
   extern const char *Hlp_FILES_Marks;
   extern const char *Hlp_FILES_Briefcase;

   extern const char *Txt_Documents_area;
   extern const char *Txt_Documents_management_area;
   extern const char *Txt_Teachers_files_area;
   extern const char *Txt_Shared_files_area;
   extern const char *Txt_Marks_area;
   extern const char *Txt_Marks_management_area;
   extern const char *Txt_Assignments_area;
   extern const char *Txt_Works_area;
   extern const char *Txt_Temporary_private_storage_area;
   extern const char *Txt_Files_of_marks_must_contain_a_table_in_HTML_format_;

   static const char **Brw_TitleOfFileBrowser[Brw_NUM_TYPES_FILE_BROWSER] =
     {
      [Brw_UNKNOWN     ] = NULL,
      [Brw_SHOW_DOC_CRS] = &Txt_Documents_area,
      [Brw_SHOW_MRK_CRS] = &Txt_Marks_area,
      [Brw_ADMI_DOC_CRS] = &Txt_Documents_management_area,
      [Brw_ADMI_SHR_CRS] = &Txt_Shared_files_area,
      [Brw_ADMI_SHR_GRP] = &Txt_Shared_files_area,
      [Brw_ADMI_WRK_USR] = &Txt_Works_area,
      [Brw_ADMI_WRK_CRS] = NULL,
      [Brw_ADMI_MRK_CRS] = &Txt_Marks_management_area,
      [Brw_ADMI_BRF_USR] = &Txt_Temporary_private_storage_area,
      [Brw_SHOW_DOC_GRP] = &Txt_Documents_area,
      [Brw_ADMI_DOC_GRP] = &Txt_Documents_management_area,
      [Brw_SHOW_MRK_GRP] = &Txt_Marks_area,
      [Brw_ADMI_MRK_GRP] = &Txt_Marks_management_area,
      [Brw_ADMI_ASG_USR] = &Txt_Assignments_area,
      [Brw_ADMI_ASG_CRS] = NULL,
      [Brw_SHOW_DOC_DEG] = &Txt_Documents_area,
      [Brw_ADMI_DOC_DEG] = &Txt_Documents_management_area,
      [Brw_SHOW_DOC_CTR] = &Txt_Documents_area,
      [Brw_ADMI_DOC_CTR] = &Txt_Documents_management_area,
      [Brw_SHOW_DOC_INS] = &Txt_Documents_area,
      [Brw_ADMI_DOC_INS] = &Txt_Documents_management_area,
      [Brw_ADMI_SHR_DEG] = &Txt_Shared_files_area,
      [Brw_ADMI_SHR_CTR] = &Txt_Shared_files_area,
      [Brw_ADMI_SHR_INS] = &Txt_Shared_files_area,
      [Brw_ADMI_TCH_CRS] = &Txt_Teachers_files_area,
      [Brw_ADMI_TCH_GRP] = &Txt_Teachers_files_area,
      [Brw_ADMI_DOC_PRJ] = NULL,
      [Brw_ADMI_ASS_PRJ] = NULL,
     };
   static const char **Brw_HelpOfFileBrowser[Brw_NUM_TYPES_FILE_BROWSER] =
     {
      [Brw_UNKNOWN     ] = NULL,
      [Brw_SHOW_DOC_CRS] = &Hlp_FILES_Documents,
      [Brw_SHOW_MRK_CRS] = &Hlp_FILES_Marks,
      [Brw_ADMI_DOC_CRS] = &Hlp_FILES_Documents,
      [Brw_ADMI_SHR_CRS] = &Hlp_FILES_Shared,
      [Brw_ADMI_SHR_GRP] = &Hlp_FILES_Shared,
      [Brw_ADMI_WRK_USR] = &Hlp_FILES_Homework_for_students,
      [Brw_ADMI_WRK_CRS] = NULL,
      [Brw_ADMI_MRK_CRS] = &Hlp_FILES_Marks,
      [Brw_ADMI_BRF_USR] = &Hlp_FILES_Briefcase,
      [Brw_SHOW_DOC_GRP] = &Hlp_FILES_Documents,
      [Brw_ADMI_DOC_GRP] = &Hlp_FILES_Documents,
      [Brw_SHOW_MRK_GRP] = &Hlp_FILES_Marks,
      [Brw_ADMI_MRK_GRP] = &Hlp_FILES_Marks,
      [Brw_ADMI_ASG_USR] = &Hlp_FILES_Homework_for_students,
      [Brw_ADMI_ASG_CRS] = NULL,
      [Brw_SHOW_DOC_DEG] = &Hlp_FILES_Documents,
      [Brw_ADMI_DOC_DEG] = &Hlp_FILES_Documents,
      [Brw_SHOW_DOC_CTR] = &Hlp_FILES_Documents,
      [Brw_ADMI_DOC_CTR] = &Hlp_FILES_Documents,
      [Brw_SHOW_DOC_INS] = &Hlp_FILES_Documents,
      [Brw_ADMI_DOC_INS] = &Hlp_FILES_Documents,
      [Brw_ADMI_SHR_DEG] = &Hlp_FILES_Shared,
      [Brw_ADMI_SHR_CTR] = &Hlp_FILES_Shared,
      [Brw_ADMI_SHR_INS] = &Hlp_FILES_Shared,
      [Brw_ADMI_TCH_CRS] = &Hlp_FILES_Private,
      [Brw_ADMI_TCH_GRP] = &Hlp_FILES_Private,
      [Brw_ADMI_DOC_PRJ] = NULL,
      [Brw_ADMI_ASS_PRJ] = NULL,
     };
   struct Brw_NumObjects Removed;
   char FileBrowserSectionId[32];
   struct BrwSiz_BrowserSize *Size = BrwSiz_GetSize ();

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
   if (Brw_CheckIfFileBrowserIsEditable (Gbl.FileBrowser.Type))
      BrwSiz_SetAndCheckQuota (Size);

   /***** Check if the clipboard is in this tree *****/
   Gbl.FileBrowser.Clipboard.IsThisTree = Brw_CheckIfClipboardIsInThisTree ();

   /***** Begin box *****/
   Gbl.FileBrowser.Id++;
   snprintf (FileBrowserSectionId,sizeof (FileBrowserSectionId),
             "file_browser_%u",Gbl.FileBrowser.Id);
   HTM_SECTION_Begin (FileBrowserSectionId);

      if (Brw_TitleOfFileBrowser[Gbl.FileBrowser.Type])
	 Box_BoxBegin (*Brw_TitleOfFileBrowser[Gbl.FileBrowser.Type],
		       Brw_PutIconsFileBrowser,NULL,
		       *Brw_HelpOfFileBrowser[Gbl.FileBrowser.Type],Box_NOT_CLOSABLE);

      /***** Subtitle *****/
      Brw_WriteSubtitleOfFileBrowser ();

      /***** List recursively the directory *****/
      HTM_TABLE_Begin ("TBL_SCROLL");
	 Str_Copy (Gbl.FileBrowser.FilFolLnk.Path,
		   Brw_RootFolderInternalNames[Gbl.FileBrowser.Type],
		   sizeof (Gbl.FileBrowser.FilFolLnk.Path) - 1);
	 Str_Copy (Gbl.FileBrowser.FilFolLnk.Name,".",
		   sizeof (Gbl.FileBrowser.FilFolLnk.Name) - 1);
	 Brw_SetFullPathInTree ();
	 Gbl.FileBrowser.FilFolLnk.Type = Brw_IS_FOLDER;
	 if (Brw_WriteRowFileBrowser (0,"1",
				      false,	// Tree not contracted
				      Brw_ICON_TREE_NOTHING))
	    Brw_ListDir (1,"1",
			 false,	// Tree not contracted
			 Gbl.FileBrowser.Path.RootFolder,
			 Brw_RootFolderInternalNames[Gbl.FileBrowser.Type]);
      HTM_TABLE_End ();

      /***** Show and store number of documents found *****/
      BrwSiz_ShowAndStoreSizeOfFileBrowser (Size);

      /***** End box *****/
      if (Brw_TitleOfFileBrowser[Gbl.FileBrowser.Type])
         Box_BoxEnd ();

   HTM_SECTION_End ();

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
  }

/*****************************************************************************/
/******************* Put contextual icons in file browser ********************/
/*****************************************************************************/

static void Brw_PutIconsFileBrowser (__attribute__((unused)) void *Args)
  {
   enum
     {
      Brw_ICON_NONE,
      Brw_ICON_VIEW,
      Brw_ICON_EDIT
     } IconViewEdit = Brw_ICON_NONE;

   /***** Set contextual icon in box *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_SHOW_DOC_INS:
	 if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
	    IconViewEdit = Brw_ICON_EDIT;
	 break;
      case Brw_ADMI_DOC_INS:
	 if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
	    IconViewEdit = Brw_ICON_VIEW;
	 break;
      case Brw_SHOW_DOC_CTR:
	 if (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM)
	    IconViewEdit = Brw_ICON_EDIT;
	 break;
      case Brw_ADMI_DOC_CTR:
	 if (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM)
	    IconViewEdit = Brw_ICON_VIEW;
	 break;
      case Brw_SHOW_DOC_DEG:
	 if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
	    IconViewEdit = Brw_ICON_EDIT;
	 break;
      case Brw_ADMI_DOC_DEG:
	 if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
	    IconViewEdit = Brw_ICON_VIEW;
	 break;
      case Brw_SHOW_DOC_CRS:
      case Brw_SHOW_DOC_GRP:
      case Brw_SHOW_MRK_CRS:
      case Brw_SHOW_MRK_GRP:
	 if (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
	     Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	    IconViewEdit = Brw_ICON_EDIT;
	 break;
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_MRK_CRS:
      case Brw_ADMI_MRK_GRP:
	 if (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
	     Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	    IconViewEdit = Brw_ICON_VIEW;
	 break;
      default:
	 break;
     }

   /***** Put icon to view / edit file browser *****/
   switch (IconViewEdit)
     {
      case Brw_ICON_NONE:
	 break;
      case Brw_ICON_VIEW:
	 Ico_PutContextualIconToView (Brw_ActFromAdmToSee[Gbl.FileBrowser.Type],NULL,
				      Brw_PutParFullTreeIfSelected,&Gbl.FileBrowser.FullTree);
	 break;
      case Brw_ICON_EDIT:
	 Ico_PutContextualIconToEdit (Brw_ActFromSeeToAdm[Gbl.FileBrowser.Type],NULL,
				      Brw_PutParFullTreeIfSelected,&Gbl.FileBrowser.FullTree);
	 break;
     }

   /***** Put icon to get resource link *****/
   if (Brw_ActReqLnk[Gbl.FileBrowser.Type] != ActUnk &&
       Rsc_CheckIfICanGetLink ())
      Ico_PutContextualIconToGetLink (Brw_ActReqLnk[Gbl.FileBrowser.Type],NULL,
				      NULL,NULL);

   /***** Put icon to show a figure *****/
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_ADMI_WRK_CRS:
      case Brw_ADMI_ASG_CRS:
      case Brw_ADMI_DOC_PRJ:
      case Brw_ADMI_ASS_PRJ:
	 break;
      default:
	 Brw_PutIconShowFigure (NULL);
	 break;
     }
  }

static void Brw_PutIconShowFigure (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_FOLDERS_AND_FILES);
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
      if (Brw_TypeIsAdmBrf[Gbl.FileBrowser.Type])
	{
	 if (Gbl.Action.Act != ActReqRemOldBrf)
	    Brw_PutLinkToAskRemOldFiles ();	// Remove old files
	}
      else if (Brw_TypeIsAdmCrsAsgWrk[Gbl.FileBrowser.Type])
	{
	 if (!Gbl.FileBrowser.ZIP.CreateZIP)
	    ZIP_PutLinkToCreateZIPAsgWrk ();	// Create a zip file with the...
						//... works of the selected users
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
   if (Brw_CheckIfFileBrowserIsEditable (Gbl.FileBrowser.Type))
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
            Set_DB_UpdateGrpMyLastAccZone ("LastDowGrpCod",-1L);
	 break;
      case Brw_SHOW_DOC_GRP:
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_GRP:
         if (Gbl.Action.Act == ActChgToSeeDocCrs ||
             Gbl.Action.Act == ActChgToAdmDocCrs ||
             Gbl.Action.Act == ActChgToAdmTch)	// Update group of last access to a documents/teachers zone only when user changes zone
            Set_DB_UpdateGrpMyLastAccZone ("LastDowGrpCod",Gbl.Crs.Grps.GrpCod);
         break;
      case Brw_ADMI_SHR_CRS:
         if (Gbl.Action.Act == ActChgToAdmSha) 	// Update group of last access to a shared files zone only when user changes zone
            Set_DB_UpdateGrpMyLastAccZone ("LastComGrpCod",-1L);
	 break;
      case Brw_ADMI_SHR_GRP:
         if (Gbl.Action.Act == ActChgToAdmSha) 	// Update group of last access to a shared files zone only when user changes zone
            Set_DB_UpdateGrpMyLastAccZone ("LastComGrpCod",Gbl.Crs.Grps.GrpCod);
	 break;
      case Brw_SHOW_MRK_CRS:
      case Brw_ADMI_MRK_CRS:
         if (Gbl.Action.Act == ActChgToSeeMrk ||
             Gbl.Action.Act == ActChgToAdmMrk)	// Update group of last access to a marks zone only when user changes zone
            Set_DB_UpdateGrpMyLastAccZone ("LastAssGrpCod",-1L);
	 break;
      case Brw_SHOW_MRK_GRP:
      case Brw_ADMI_MRK_GRP:
         if (Gbl.Action.Act == ActChgToSeeMrk ||
             Gbl.Action.Act == ActChgToAdmMrk)	// Update group of last access to a marks zone only when user changes zone
            Set_DB_UpdateGrpMyLastAccZone ("LastAssGrpCod",Gbl.Crs.Grps.GrpCod);
	 break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/*********************** Write title of a file browser ***********************/
/*****************************************************************************/

static void Brw_WriteSubtitleOfFileBrowser (void)
  {
   extern const char *Txt_accessible_for_reading_and_writing_by_administrators_of_the_center;
   extern const char *Txt_accessible_for_reading_and_writing_by_administrators_of_the_degree;
   extern const char *Txt_accessible_for_reading_and_writing_by_administrators_of_the_institution;
   extern const char *Txt_accessible_for_reading_and_writing_by_project_members;
   extern const char *Txt_accessible_for_reading_and_writing_by_project_tutors_and_evaluators;
   extern const char *Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_center;
   extern const char *Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_degree;
   extern const char *Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_institution;
   extern const char *Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_course;
   extern const char *Txt_accessible_for_reading_and_writing_by_students_of_the_group_and_teachers_of_the_course;
   extern const char *Txt_accessible_for_reading_and_writing_by_teachers_of_the_course;
   extern const char *Txt_accessible_for_reading_and_writing_by_you_and_the_course_teachers;
   extern const char *Txt_accessible_only_for_reading_by_the_student_and_the_course_teachers;
   extern const char *Txt_accessible_only_for_reading_by_students_and_teachers_of_the_institution;
   extern const char *Txt_accessible_only_for_reading_by_students_and_teachers_of_the_center;
   extern const char *Txt_accessible_only_for_reading_by_students_and_teachers_of_the_degree;
   extern const char *Txt_accessible_only_for_reading_by_students_and_teachers_of_the_course;
   extern const char *Txt_accessible_only_for_reading_by_students_of_the_group_and_teachers_of_the_course;
   extern const char *Txt_nobody_else_can_access_this_content;
   static const char **Brw_SubtitleOfFileBrowser[Brw_NUM_TYPES_FILE_BROWSER] =
     {
      [Brw_UNKNOWN     ] = NULL,
      [Brw_SHOW_DOC_CRS] = &Txt_accessible_only_for_reading_by_students_and_teachers_of_the_course,
      [Brw_SHOW_MRK_CRS] = &Txt_accessible_only_for_reading_by_the_student_and_the_course_teachers,
      [Brw_ADMI_DOC_CRS] = &Txt_accessible_for_reading_and_writing_by_teachers_of_the_course,
      [Brw_ADMI_SHR_CRS] = &Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_course,
      [Brw_ADMI_SHR_GRP] = &Txt_accessible_for_reading_and_writing_by_students_of_the_group_and_teachers_of_the_course,
      [Brw_ADMI_WRK_USR] = &Txt_accessible_for_reading_and_writing_by_you_and_the_course_teachers,
      [Brw_ADMI_WRK_CRS] = NULL,
      [Brw_ADMI_MRK_CRS] = &Txt_accessible_for_reading_and_writing_by_teachers_of_the_course,
      [Brw_ADMI_BRF_USR] = &Txt_nobody_else_can_access_this_content,
      [Brw_SHOW_DOC_GRP] = &Txt_accessible_only_for_reading_by_students_of_the_group_and_teachers_of_the_course,
      [Brw_ADMI_DOC_GRP] = &Txt_accessible_for_reading_and_writing_by_teachers_of_the_course,
      [Brw_SHOW_MRK_GRP] = &Txt_accessible_only_for_reading_by_the_student_and_the_course_teachers,
      [Brw_ADMI_MRK_GRP] = &Txt_accessible_for_reading_and_writing_by_teachers_of_the_course,
      [Brw_ADMI_ASG_USR] = &Txt_accessible_for_reading_and_writing_by_you_and_the_course_teachers,
      [Brw_ADMI_ASG_CRS] = NULL,
      [Brw_SHOW_DOC_DEG] = &Txt_accessible_only_for_reading_by_students_and_teachers_of_the_degree,
      [Brw_ADMI_DOC_DEG] = &Txt_accessible_for_reading_and_writing_by_administrators_of_the_degree,
      [Brw_SHOW_DOC_CTR] = &Txt_accessible_only_for_reading_by_students_and_teachers_of_the_center,
      [Brw_ADMI_DOC_CTR] = &Txt_accessible_for_reading_and_writing_by_administrators_of_the_center,
      [Brw_SHOW_DOC_INS] = &Txt_accessible_only_for_reading_by_students_and_teachers_of_the_institution,
      [Brw_ADMI_DOC_INS] = &Txt_accessible_for_reading_and_writing_by_administrators_of_the_institution,
      [Brw_ADMI_SHR_DEG] = &Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_degree,
      [Brw_ADMI_SHR_CTR] = &Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_center,
      [Brw_ADMI_SHR_INS] = &Txt_accessible_for_reading_and_writing_by_students_and_teachers_of_the_institution,
      [Brw_ADMI_TCH_CRS] = &Txt_accessible_for_reading_and_writing_by_teachers_of_the_course,
      [Brw_ADMI_TCH_GRP] = &Txt_accessible_for_reading_and_writing_by_teachers_of_the_course,
      [Brw_ADMI_DOC_PRJ] = &Txt_accessible_for_reading_and_writing_by_project_members,
      [Brw_ADMI_ASS_PRJ] = &Txt_accessible_for_reading_and_writing_by_project_tutors_and_evaluators,
     };

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
   /*
   Subtitle = NULL;
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_ADMI_WRK_CRS:
      case Brw_ADMI_ASG_CRS:
         Subtitle = Gbl.Usrs.Other.UsrDat.FullName;
	 break;
      default:
	 if (Brw_SubtitleOfFileBrowser[Gbl.FileBrowser.Type])
	    Subtitle = *Brw_SubtitleOfFileBrowser[Gbl.FileBrowser.Type];
         break;
     }
   */
   if (Brw_SubtitleOfFileBrowser[Gbl.FileBrowser.Type])
     {
      HTM_DIV_Begin ("class=\"BROWSER_SUBTITLE\"");
	 HTM_Txt (*Brw_SubtitleOfFileBrowser[Gbl.FileBrowser.Type]);
      HTM_DIV_End ();
     }
  }

/*****************************************************************************/
/************ Initialize hidden levels of file browser to false **************/
/*****************************************************************************/

static void Brw_InitHiddenLevels (void)
  {
   unsigned Level;

   for (Level  = 0;
	Level <= BrwSiz_MAX_DIR_LEVELS;
	Level++)
      Gbl.FileBrowser.HiddenLevels[Level] = false;
  }

/*****************************************************************************/
/************** Write a form to select whether show full tree ****************/
/*****************************************************************************/

static void Brw_PutCheckboxFullTree (void)
  {
   extern const char *Txt_Show_all_files;

   Lay_PutContextualCheckbox (Brw_ActSeeAdm[Gbl.FileBrowser.Type],
                              Brw_PutParsFullTree,
                              "FullTree",
                              Gbl.FileBrowser.FullTree,false,
                              Txt_Show_all_files,Txt_Show_all_files);
  }

static void Brw_PutParsFullTree (void)
  {
   if (Brw_TypeIsGrpBrw[Gbl.FileBrowser.Type])
      ParCod_PutPar (ParCod_Grp,Gbl.Crs.Grps.GrpCod);
   else if (Brw_TypeIsAdmPrj[Gbl.FileBrowser.Type])	// This file browser needs specify a project
      ParCod_PutPar (ParCod_Prj,Prj_GetPrjCod ());
   else if (Brw_TypeIsAdmCrsAsgWrk[Gbl.FileBrowser.Type])
      Usr_PutParSelectedUsrsCods (&Gbl.Usrs.Selected);
  }

/*****************************************************************************/
/********* Put hidden parameter "full tree" if full tree is selected *********/
/*****************************************************************************/

void Brw_PutParFullTreeIfSelected (void *FullTree)
  {
   if (FullTree)
      if (*((bool *) FullTree))	// Put hidden parameter only if full tree selected
	 Par_PutParChar ("FullTree",'Y');
  }

/*****************************************************************************/
/******************* Get whether to show full tree from form *****************/
/*****************************************************************************/

static bool Brw_GetFullTreeFromForm (void)
  {
   return Par_GetParBool ("FullTree");
  }

/*****************************************************************************/
/******** Create a temporary public directory used to download files *********/
/*****************************************************************************/

void Brw_CreateDirDownloadTmp (void)
  {
   static unsigned NumDir = 0;	// When this function is called several times in the same execution of the program, each time a new directory is created
				// This happens when the trees of assignments and works of several users are being listed
   char PathUniqueDirL[PATH_MAX + 1];
   char PathUniqueDirR[PATH_MAX + 1 + NAME_MAX + 1];
   const char *UniqueNameEncrypted = Cry_GetUniqueNameEncrypted ();

   /* Example: /var/www/html/swad/tmp/SS/ujCNWsy4ZOdmgMKYBe0sKPAJu6szaZOQlIlJs_QIY */

   /***** If the public directory does not exist, create it *****/
   Fil_CreateDirIfNotExists (Cfg_PATH_FILE_BROWSER_TMP_PUBLIC);

   /***** Unique temporary directory.
          Important: number of directories inside a directory
          is limited to 32K in Linux ==> create directories in two levels *****/
   /* 1. Build the name of the directory, splitted in two parts: */
   /* 1a: 2 leftmost chars */
   Gbl.FileBrowser.TmpPubDir.L[0] = UniqueNameEncrypted[0];
   Gbl.FileBrowser.TmpPubDir.L[1] = UniqueNameEncrypted[1];
   Gbl.FileBrowser.TmpPubDir.L[2] = '\0';
   /* 1b: rest of chars */
   if (NumDir)
      snprintf (Gbl.FileBrowser.TmpPubDir.R,sizeof (Gbl.FileBrowser.TmpPubDir.R),
	        "%s_%u",&UniqueNameEncrypted[2],NumDir);
   else
      Str_Copy (Gbl.FileBrowser.TmpPubDir.R,&UniqueNameEncrypted[2],
                sizeof (Gbl.FileBrowser.TmpPubDir.R) - 1);

   /* 2. Create the left directory */
   snprintf (PathUniqueDirL,sizeof (PathUniqueDirL),"%s/%s",
             Cfg_PATH_FILE_BROWSER_TMP_PUBLIC,Gbl.FileBrowser.TmpPubDir.L);
   Fil_CreateDirIfNotExists (PathUniqueDirL);

   /* 3. Create the right directory inside the left one */
   snprintf (PathUniqueDirR,sizeof (PathUniqueDirR),"%s/%s",
             PathUniqueDirL,Gbl.FileBrowser.TmpPubDir.R);
   if (mkdir (PathUniqueDirR,(mode_t) 0xFFF))
      Err_ShowErrorAndExit ("Can not create a temporary folder for download.");

   /* 4. Increase number of directory for next call */
   NumDir++;
  }

/*****************************************************************************/
/* Get and update the date of my last access to file browser in this course **/
/*****************************************************************************/

static void Brw_GetAndUpdateDateLastAccFileBrowser (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;

   /***** Get date of last accesss to a file browser from database *****/
   NumRows = Brw_DB_GetDateMyLastAccFileBrowser (&mysql_res);

   if (NumRows == 0)	// May be an administrator not belonging to this course
      Gbl.Usrs.Me.TimeLastAccToThisFileBrowser = LONG_MAX;	// Initialize to a big value in order to show files as old
   else if (NumRows == 1)
     {
      /* Get the date of the last access to file browser (row[0]) */
      row = mysql_fetch_row (mysql_res);
      if (row[0] == NULL)
         Gbl.Usrs.Me.TimeLastAccToThisFileBrowser = 0;
      else if (sscanf (row[0],"%ld",&Gbl.Usrs.Me.TimeLastAccToThisFileBrowser) != 1)
         Err_ShowErrorAndExit ("Error when reading date-time of last access to a file browser.");
     }
   else
      Err_ShowErrorAndExit ("Error when getting date-time of last access to a file browser.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Update date of my last access to file browser in this course *****/
   Brw_DB_UpdateDateMyLastAccFileBrowser ();
  }

/*****************************************************************************/
/************* Get the group of my last access to a file browser *************/
/*****************************************************************************/

long Brw_GetGrpLastAccZone (const char *FieldNameDB)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrps;
   long GrpCod = -1L;

   /***** Get the group of my last access to a file browser *****/
   NumGrps = Brw_DB_GetGrpLastAccFileBrowser (&mysql_res,FieldNameDB);

   if (NumGrps == 0)	// May be an administrator not belonging to this course
      GrpCod = -1L;
   else if (NumGrps == 1)
     {
      /* Get the group code (row[0]) */
      row = mysql_fetch_row (mysql_res);
      if (row[0])
        {
         if (sscanf (row[0],"%ld",&GrpCod) != 1)
            Err_ShowErrorAndExit ("Error when reading the group of your last access to a file browser.");
        }
      else
         GrpCod = -1L;
     }
   else
      Err_ShowErrorAndExit ("Error when getting the group of your last access to a file browser.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Check if group exists (it's possible that this group has been removed after my last access to it) *****/
   if (GrpCod >= 0)
      if (Grp_DB_CheckIfGrpExists (GrpCod))
         /* Check if I belong to this group (it's possible that I have been removed from this group after my last access to it) */
         if (Grp_GetIfIBelongToGrp (GrpCod))
            return GrpCod;

   return -1L;	// To indicate course zone
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
	    snprintf (PathFileRel       ,sizeof (PathFileRel       ),"%s/%s",
		      Path      ,FileList[NumFile]->d_name);
	    snprintf (PathFileInExplTree,sizeof (PathFileInExplTree),"%s/%s",
		      PathInTree,FileList[NumFile]->d_name);

	    Str_Copy (Gbl.FileBrowser.FilFolLnk.Path,PathInTree,
		      sizeof (Gbl.FileBrowser.FilFolLnk.Path) - 1);
	    Str_Copy (Gbl.FileBrowser.FilFolLnk.Name,FileList[NumFile]->d_name,
		      sizeof (Gbl.FileBrowser.FilFolLnk.Name) - 1);
	    Brw_SetFullPathInTree ();

	    /***** Add number of row to parent row id *****/
	    NumRow++;
	    snprintf (RowId,sizeof (RowId),"%s_%u",ParentRowId,NumRow);

	    /***** Get file or folder status *****/
	    if (lstat (PathFileRel,&FileStatus))	// On success ==> 0 is returned
	       Err_ShowErrorAndExit ("Can not get information about a file or folder.");
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
			IconSubtree = Brw_DB_GetIfContractedOrExpandedFolder (Gbl.FileBrowser.FilFolLnk.Full) == ConExp_EXPANDED ? Brw_ICON_TREE_CONTRACT :
												                       Brw_ICON_TREE_EXPAND;
		     for (NumFileInSubdir = 0;
			  NumFileInSubdir < NumFilesInSubdir;
			  NumFileInSubdir++)
			free (SubdirFileList[NumFileInSubdir]);
		     free (SubdirFileList);
		    }
		  else
		     Err_ShowErrorAndExit ("Error while scanning directory.");
		 }

	       /***** Write a row for the subdirectory *****/
	       if (Brw_WriteRowFileBrowser (Level,RowId,
	                                    TreeContracted,
	                                    IconSubtree))
		  if (Level < BrwSiz_MAX_DIR_LEVELS)
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
      Err_ShowErrorAndExit ("Error while scanning directory.");
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
   HidVis_HiddenOrVisible_t HiddenOrVisible = HidVis_VISIBLE;
   bool RowSetAsPublic = false;
   bool LightStyle = false;
   bool IsRecent = false;
   struct Brw_FileMetadata FileMetadata;
   char FileBrowserId[32];
   char TxtStyle[64];
   char *InputStyle;

   /***** Initializations *****/
   Gbl.FileBrowser.Clipboard.IsThisFile = false;
   snprintf (FileBrowserId,sizeof (FileBrowserId),"fil_brw_%u",
	     Gbl.FileBrowser.Id);

   /***** Is this row hidden or visible? *****/
   if (Brw_TypeIsSeeDoc[Gbl.FileBrowser.Type] ||
       Brw_TypeIsSeeMrk[Gbl.FileBrowser.Type] ||
       Brw_TypeIsAdmDoc[Gbl.FileBrowser.Type] ||
       Brw_TypeIsAdmMrk[Gbl.FileBrowser.Type])
     {
      HiddenOrVisible = Brw_CheckIfFileOrFolderIsSetAsHiddenInDB (Gbl.FileBrowser.FilFolLnk.Type,
                                                                  Gbl.FileBrowser.FilFolLnk.Full);
      if (HiddenOrVisible == HidVis_HIDDEN &&
	  (Brw_TypeIsSeeDoc[Gbl.FileBrowser.Type] ||
	   Brw_TypeIsSeeMrk[Gbl.FileBrowser.Type]) &&
	  Level)	// Don't return on level 0
         return false;

      if (Brw_TypeIsAdmDoc[Gbl.FileBrowser.Type] ||
	  Brw_TypeIsAdmMrk[Gbl.FileBrowser.Type])
        {
	 if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER)
	    Gbl.FileBrowser.HiddenLevels[Level] = (HiddenOrVisible == HidVis_HIDDEN);
	 switch (HiddenOrVisible)
	   {
	    case HidVis_HIDDEN:	// this row is marked as hidden
               LightStyle = true;
	       break;
	    case HidVis_VISIBLE:	// this row is not marked as hidden
               LightStyle = Brw_CheckIfAnyHigherLevelIsHidden (Level);
	       break;
	   }
        }
     }

   /***** Get file metadata *****/
   Brw_GetFileMetadataByPath (&FileMetadata);
   Brw_GetFileTypeSizeAndDate (&FileMetadata);
   if (FileMetadata.FilCod <= 0)	// No entry for this file in database table of files
      /* Add entry to the table of files/folders */
      FileMetadata.FilCod = Brw_DB_AddPath (-1L,FileMetadata.FilFolLnk.Type,
                                             Gbl.FileBrowser.FilFolLnk.Full,false,Brw_LICENSE_DEFAULT);

   /***** Is this row public or private? *****/
   if (Brw_TypeIsSeeDoc[Gbl.FileBrowser.Type] ||
       Brw_TypeIsAdmDoc[Gbl.FileBrowser.Type] ||
       Brw_TypeIsAdmSha[Gbl.FileBrowser.Type])
     {
      RowSetAsPublic = (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER) ? Brw_DB_GetIfFolderHasPublicFiles (Gbl.FileBrowser.FilFolLnk.Full) :
	                                                                   FileMetadata.IsPublic;
      if (Gbl.FileBrowser.ShowOnlyPublicFiles && !RowSetAsPublic)
         return false;
     }

   /***** Check if is a recent file or folder *****/
   // If less than a week since last modify ==> indicate the file is recent by writting its name in green
   if (Dat_GetStartExecutionTimeUTC () < FileMetadata.Time + (7L * 24L * 60L * 60L))
      IsRecent = true;

   /* Style of the text in this row */
   snprintf (TxtStyle,sizeof (TxtStyle),
             "%s_%s",
             Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER ||
             !IsRecent ? (LightStyle ? "LST_HID" :
        			       "LST") :
        	         (LightStyle ? "LST_REC_HID" :
        	        	       "LST_REC"),
	     The_GetSuffix ());
   InputStyle = (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER ||
	         !IsRecent) ? (LightStyle ? "LST_EDIT_HID" :
					    "LST_EDIT") :
			      (LightStyle ? "LST_EDIT_REC_HID" :
					    "LST_EDIT_REC");

   /***** Get data of assignment using the name of the folder *****/
   if (Brw_TypeIsAdmAsg[Gbl.FileBrowser.Type] && Level == 1)	// Main folder of the assignment
     {
      Asg_SetFolder (&Gbl.FileBrowser.Asg,Level);
      Asg_GetAssignmentDataByFolder (&Gbl.FileBrowser.Asg);
      // The data of this assignment remains in Gbl.FileBrowser.Asg
      // for all subsequent rows with Level > 1 (files or folders inside this folder),
      // and they are overwritten on the next row with level == 1 (next assignment)
     }

   /***** Begin this row *****/
   if (asprintf (&Anchor,"fil_brw_%u_%s",
		 Gbl.FileBrowser.Id,RowId) < 0)
      Err_NotEnoughMemoryExit ();
   switch (IconThisRow)
     {
      case Brw_ICON_TREE_NOTHING:
	 if (TreeContracted)	// This row is inside a contracted subtree
            HTM_TR_Begin ("id=\"%s\""
        	          " style=\"display:none;\"",
        	          Anchor);
	 else
            HTM_TR_Begin ("id=\"%s\"",
                          Anchor);
	 break;
      case Brw_ICON_TREE_EXPAND:
	 if (TreeContracted)	// This row is inside a contracted subtree
            HTM_TR_Begin ("id=\"%s\" data-folder=\"contracted\""
        	          " style=\"display:none;\"",
        	          Anchor);
	 else
            HTM_TR_Begin ("id=\"%s\" data-folder=\"contracted\"",
                          Anchor);
	 break;
      case Brw_ICON_TREE_CONTRACT:
	 if (TreeContracted)	// This row is inside a contracted subtree
            HTM_TR_Begin ("id=\"%s\" data-folder=\"expanded\""
        	          " style=\"display:none;\"",
        	          Anchor);
	 else
            HTM_TR_Begin ("id=\"%s\" data-folder=\"expanded\"",
                          Anchor);
	 break;
     }

   /****** If current action allows file administration... ******/
   Brw_SetIfICanEditFileOrFolder (false);
   if (Brw_CheckIfFileBrowserIsEditable (Gbl.FileBrowser.Type) &&
       !Gbl.FileBrowser.ShowOnlyPublicFiles)
     {
      if (Gbl.FileBrowser.Clipboard.IsThisTree &&
	  Level != 0)	// Never copy root folder
	 // If path in the clipboard is equal to complete path in tree...
	 // ...or is the start of complete path in tree...
         if (Str_Path1BeginsByPath2 (Gbl.FileBrowser.FilFolLnk.Full,
                                     Gbl.FileBrowser.Clipboard.FilFolLnk.Full))
            Gbl.FileBrowser.Clipboard.IsThisFile = true;

      /* Check if I can modify (remove, rename, etc.) this file or folder */
      Brw_SetIfICanEditFileOrFolder (Brw_CheckIfICanEditFileOrFolder (Level));

      /* Icon to remove folder, file or link */
      Brw_PutIconRemove ();

      /* Icon to copy */
      Brw_PutIconCopy ();

      /* Icon to paste */
      Brw_PutIconPaste (Level);
     }

   /***** Indentation depending on level, icon, and file/folder name *****/
   /* Begin column */
   HTM_TD_Begin ("class=\"NO_BR LM %s\" style=\"width:99%%;\"",
                 The_GetColorRows ());

      HTM_TABLE_Begin (NULL);
	 HTM_TR_Begin (NULL);

	    /* Indent depending on level */
	    if (Level)
	       Brw_IndentAndWriteIconExpandContract (Level,FileBrowserId,RowId,IconThisRow);

	    /* Put icon to hide/unhide file or folder */
	    if (Brw_TypeIsAdmDoc[Gbl.FileBrowser.Type] ||
		Brw_TypeIsAdmMrk[Gbl.FileBrowser.Type])
	       Brw_PutIconHideUnhide (Anchor,HiddenOrVisible);

	    /***** File or folder icon *****/
	    switch (Gbl.FileBrowser.FilFolLnk.Type)
	      {
	       case Brw_IS_FILE:
	       case Brw_IS_LINK:
		  /* Icon with file type or link */
		  Brw_PutIconFileWithLinkToViewMetadata (&FileMetadata);
		  break;
	       case Brw_IS_FOLDER:
		  /* Icon with folder */
		  Brw_PutIconFolder (Level,FileBrowserId,RowId,IconThisRow);
		  break;
	       default:
		  break;
	      }

	    /* Check if is a new file or folder */
	    // If our last access was before the last modify ==>
	    // indicate the file is new by putting a blinking star
	    if (Gbl.Usrs.Me.TimeLastAccToThisFileBrowser < FileMetadata.Time)
	       Brw_PutIconNewFileOrFolder ();

	    /* File or folder name */
	    Brw_WriteFileName (Level,FileMetadata.IsPublic,TxtStyle,InputStyle);

	 HTM_TR_End ();
      HTM_TABLE_End ();

   /* End column */
   HTM_TD_End ();

   if (Brw_TypeIsAdmMrk[Gbl.FileBrowser.Type])
      /***** Header and footer rows *****/
      Mrk_GetAndWriteNumRowsHeaderAndFooter ();
   else if (Brw_TypeIsAdmAsg[Gbl.FileBrowser.Type] && Level == 1)
      /***** Start and end dates of assignment *****/
      Asg_WriteDatesAssignment (&Gbl.FileBrowser.Asg);
   else
      /***** File date and size *****/
      Brw_WriteFileSizeAndDate (&FileMetadata,TxtStyle);

   if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER)
     {
      /***** Put icon to download ZIP of folder *****/
      HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
	 if (Gbl.Usrs.Me.Role.Logged >= Rol_STD &&	// Only ZIP folders if I am student, teacher...
	     !Brw_TypeIsSeeMrk[Gbl.FileBrowser.Type] &&	// Do not ZIP folders when showing marks
	     !(Brw_TypeIsSeeDoc[Gbl.FileBrowser.Type] && HiddenOrVisible == HidVis_HIDDEN))	// When seeing docs, if folder is not hidden (this could happen for Level == 0)
	    Brw_PutButtonToDownloadZIPOfAFolder ();
      HTM_TD_End ();
     }
   else	// File or link
      /***** User who created the file or folder *****/
      Brw_WriteFileOrFolderPublisher (Level,FileMetadata.PublisherUsrCod);

   /***** End this row *****/
   free (Anchor);
   HTM_TR_End ();

   The_ChangeRowColor ();

   if (HiddenOrVisible == HidVis_HIDDEN &&
       (Brw_TypeIsSeeDoc[Gbl.FileBrowser.Type] ||
	Brw_TypeIsSeeMrk[Gbl.FileBrowser.Type]))
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
      snprintf (FullPath,sizeof (FullPath),"%s/%s",
		Gbl.FileBrowser.FilFolLnk.Path,Gbl.FileBrowser.FilFolLnk.Name);
      Str_Copy (Gbl.FileBrowser.FilFolLnk.Full,FullPath,
	        sizeof (Gbl.FileBrowser.FilFolLnk.Full) - 1);
     }
   else	// It's the root folder
      Str_Copy (Gbl.FileBrowser.FilFolLnk.Full,Gbl.FileBrowser.FilFolLnk.Path,
	        sizeof (Gbl.FileBrowser.FilFolLnk.Full) - 1);
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
       Brw_TypeIsAdmMrk[Gbl.FileBrowser.Type])
      return false;

   /**** If I can not create elements into this folder... *****/
   if (!Brw_CheckIfICanCreateIntoFolder (Level))
      return false;	// Pasting into top level of assignments is forbidden

   /**** If we are in the same tree of the clipboard... *****/
   if (Gbl.FileBrowser.Clipboard.IsThisTree)	// We are in the same tree of the clipboard ==>
						// we can paste or not depending on the subtree
     {
      /***** Construct the name of the file or folder destination *****/
      snprintf (PathDstWithFile,sizeof (PathDstWithFile),"%s/%s",
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
   HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());

      if (Brw_GetIfICanEditFileOrFolder ())	// Can I remove this?
	 switch (Gbl.FileBrowser.FilFolLnk.Type)
	   {
	    case Brw_IS_FILE:
	    case Brw_IS_LINK:
	       /***** Form to remove a file or link *****/
	       Ico_PutContextualIconToRemove (Brw_ActAskRemoveFile[Gbl.FileBrowser.Type],NULL,
					      Brw_PutImplicitParsFileBrowser,&Gbl.FileBrowser.FilFolLnk);
	       break;
	    case Brw_IS_FOLDER:
	       /***** Form to remove a folder *****/
	       Ico_PutContextualIconToRemove (Brw_ActRemoveFolder[Gbl.FileBrowser.Type],NULL,
					      Brw_PutImplicitParsFileBrowser,&Gbl.FileBrowser.FilFolLnk);
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
   HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());

      /***** Form to copy into the clipboard *****/
      Ico_PutContextualIconToCopy (Brw_ActCopy[Gbl.FileBrowser.Type],
				   Brw_PutImplicitParsFileBrowser,&Gbl.FileBrowser.FilFolLnk);

   HTM_TD_End ();
  }

/*****************************************************************************/
/************* Write link and icon to paste a file or a folder ***************/
/*****************************************************************************/

static void Brw_PutIconPaste (unsigned Level)
  {
   extern const char *Txt_Copy_not_allowed;

   HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());

      if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER)	// Can't paste in a file or link
	{
	 /* Icon to paste */
	 if (Brw_CheckIfCanPasteIn (Level))
	   {
	    /***** Form to paste the content of the clipboard *****/
	    Ico_PutContextualIconToPaste (Brw_ActPaste[Gbl.FileBrowser.Type],
					  Brw_PutImplicitParsFileBrowser,&Gbl.FileBrowser.FilFolLnk);
	   }
	 else
	    /* Icon to paste inactive */
	    Ico_PutIconOff ("paste.svg",Ico_BLACK,Txt_Copy_not_allowed);
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
	    HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
	       switch (IconThisRow)
		 {
		  case Brw_ICON_TREE_NOTHING:
		     Ico_PutIcon ("tr16x16.gif",Ico_UNCHANGED,"","ICO20x20");
		     break;
		  case Brw_ICON_TREE_EXPAND:
		     /***** Visible icon to expand folder *****/
		     Brw_PutIconToExpandFolder (FileBrowserId,RowId,
						HidVis_VISIBLE);

		     /***** Hidden icon to contract folder *****/
		     Brw_PutIconToContractFolder (FileBrowserId,RowId,
						  HidVis_HIDDEN);
		     break;
		  case Brw_ICON_TREE_CONTRACT:
		     /***** Hidden icon to expand folder *****/
		     Brw_PutIconToExpandFolder (FileBrowserId,RowId,
						HidVis_HIDDEN);

		     /***** Visible icon to contract folder *****/
		     Brw_PutIconToContractFolder (FileBrowserId,RowId,
						  HidVis_VISIBLE);
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
      HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
	 Ico_PutIcon ("tr16x16.gif",Ico_UNCHANGED,"","ICO20x20");
      HTM_TD_End ();
     }
  }

/*****************************************************************************/
/************************ Put icon to expand a folder ************************/
/*****************************************************************************/

static void Brw_PutIconToExpandFolder (const char *FileBrowserId,const char *RowId,
                                       HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   char JavaScriptFuncToExpandFolder[256 + Brw_MAX_ROW_ID];

   /***** Begin container *****/
   switch (HiddenOrVisible)
     {
      case HidVis_HIDDEN:
	 HTM_DIV_Begin ("id=\"expand_%s_%s\" style=\"display:none;\"",
			FileBrowserId,RowId);
	 break;
      case HidVis_VISIBLE:
	 HTM_DIV_Begin ("id=\"expand_%s_%s\"",
			FileBrowserId,RowId);
	 break;
     }

   /***** Form and icon *****/
   snprintf (JavaScriptFuncToExpandFolder,sizeof (JavaScriptFuncToExpandFolder),
	     "ExpandFolder('%s_%s')",
	     FileBrowserId,RowId);
   Frm_BeginFormAnchorOnSubmit (Brw_ActExpandFolder[Gbl.FileBrowser.Type],
				FileBrowserId,
				JavaScriptFuncToExpandFolder);	// JavaScript function to unhide rows
      Brw_PutImplicitParsFileBrowser (&Gbl.FileBrowser.FilFolLnk);
      Ico_PutIconLink ("caret-right.svg",Ico_BLACK,
                       Brw_ActExpandFolder[Gbl.FileBrowser.Type]);
   Frm_EndForm ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*********************** Put icon to contract a folder ***********************/
/*****************************************************************************/

static void Brw_PutIconToContractFolder (const char *FileBrowserId,const char *RowId,
                                         HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   char JavaScriptFuncToContractFolder[256 + Brw_MAX_ROW_ID];

   /***** Begin container *****/
   switch (HiddenOrVisible)
     {
      case HidVis_HIDDEN:
	 HTM_DIV_Begin ("id=\"contract_%s_%s\" style=\"display:none;\"",
			FileBrowserId,RowId);
	 break;
      case HidVis_VISIBLE:
	 HTM_DIV_Begin ("id=\"contract_%s_%s\"",
			FileBrowserId,RowId);
	 break;
     }

   /***** Form and icon *****/
   snprintf (JavaScriptFuncToContractFolder,sizeof (JavaScriptFuncToContractFolder),
	     "ContractFolder('%s_%s')",
	     FileBrowserId,RowId);
   Frm_BeginFormAnchorOnSubmit (Brw_ActContractFolder[Gbl.FileBrowser.Type],
				FileBrowserId,
				JavaScriptFuncToContractFolder);	// JavaScript function to hide rows
      Brw_PutImplicitParsFileBrowser (&Gbl.FileBrowser.FilFolLnk);
      Ico_PutIconLink ("caret-down.svg",Ico_BLACK,
                       Brw_ActContractFolder[Gbl.FileBrowser.Type]);
   Frm_EndForm ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************* Put link and icon to hide/unhide file or folder ***************/
/*****************************************************************************/

static void Brw_PutIconHideUnhide (const char *Anchor,
                                   HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   static const Act_Action_t Brw_ActHideUnhide[Brw_NUM_TYPES_FILE_BROWSER][HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [Brw_UNKNOWN     ] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_SHOW_DOC_CRS] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_SHOW_MRK_CRS] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_DOC_CRS] = {[HidVis_HIDDEN] = ActUnhDocCrs	,[HidVis_VISIBLE] = ActHidDocCrs	},
      [Brw_ADMI_SHR_CRS] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_SHR_GRP] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_WRK_USR] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_WRK_CRS] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_MRK_CRS] = {[HidVis_HIDDEN] = ActUnhMrkCrs	,[HidVis_VISIBLE] = ActHidMrkCrs	},
      [Brw_ADMI_BRF_USR] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_SHOW_DOC_GRP] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_DOC_GRP] = {[HidVis_HIDDEN] = ActUnhDocGrp	,[HidVis_VISIBLE] = ActHidDocGrp	},
      [Brw_SHOW_MRK_GRP] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_MRK_GRP] = {[HidVis_HIDDEN] = ActUnhMrkGrp	,[HidVis_VISIBLE] = ActHidMrkGrp	},
      [Brw_ADMI_ASG_USR] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_ASG_CRS] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_SHOW_DOC_DEG] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_DOC_DEG] = {[HidVis_HIDDEN] = ActUnhDocDeg	,[HidVis_VISIBLE] = ActHidDocDeg	},
      [Brw_SHOW_DOC_CTR] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_DOC_CTR] = {[HidVis_HIDDEN] = ActUnhDocCtr	,[HidVis_VISIBLE] = ActHidDocCtr	},
      [Brw_SHOW_DOC_INS] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_DOC_INS] = {[HidVis_HIDDEN] = ActUnhDocIns	,[HidVis_VISIBLE] = ActHidDocIns	},
      [Brw_ADMI_SHR_DEG] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_SHR_CTR] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_SHR_INS] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_TCH_CRS] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_TCH_GRP] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_DOC_PRJ] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
      [Brw_ADMI_ASS_PRJ] = {[HidVis_HIDDEN] = ActUnk		,[HidVis_VISIBLE] = ActUnk		},
     };

   HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
      Ico_PutContextualIconToHideUnhide (Brw_ActHideUnhide[Gbl.FileBrowser.Type],Anchor,
					 Brw_PutImplicitParsFileBrowser,&Gbl.FileBrowser.FilFolLnk,
					 HiddenOrVisible);
   HTM_TD_End ();
  }

/*****************************************************************************/
/********* Check if any level higher than the current one is hidden **********/
/*****************************************************************************/

static bool Brw_CheckIfAnyHigherLevelIsHidden (unsigned CurrentLevel)
  {
   unsigned Level;

   for (Level = 0;
	Level < CurrentLevel;
	Level++)
      if (Gbl.FileBrowser.HiddenLevels[Level])	// Hidden
         return true;

   return false;	// None is hidden. All are visible.
  }

/*****************************************************************************/
/** Write link e icon to upload or paste files, or to create folder or link **/
/*****************************************************************************/

static void Brw_PutIconFolder (unsigned Level,
                               const char *FileBrowserId,const char *RowId,
                               Brw_IconTree_t IconSubtree)
  {
   bool ICanCreate;

   /***** Begin cell *****/
   HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());

      /***** Put icon to create a new file or folder *****/
      if ((ICanCreate = Brw_CheckIfICanCreateIntoFolder (Level)))	// I can create a new file or folder
	{
	 if (IconSubtree == Brw_ICON_TREE_EXPAND)
	   {
	    /***** Visible icon with folder closed *****/
	    Brw_PutIconFolderWithPlus (FileBrowserId,RowId,
				       false,		// Closed
				       HidVis_VISIBLE);	// Visible

	    /***** Hidden icon with folder open *****/
	    Brw_PutIconFolderWithPlus (FileBrowserId,RowId,
				       true,		// Open
				       HidVis_HIDDEN);	// Hidden
	   }
	 else
	   {
	    /***** Hidden icon with folder closed *****/
	    Brw_PutIconFolderWithPlus (FileBrowserId,RowId,
				       false,		// Closed
				       HidVis_HIDDEN);	// Hidden

	    /***** Visible icon with folder open *****/
	    Brw_PutIconFolderWithPlus (FileBrowserId,RowId,
				       true,		// Open
				       HidVis_VISIBLE);	// Visible
	   }
	}
      else	// I can't create a new file or folder
	{
	 if (IconSubtree == Brw_ICON_TREE_EXPAND)
	   {
	    /***** Visible icon with folder closed *****/
	    Brw_PutIconFolderWithoutPlus (FileBrowserId,RowId,
					  false,	// Closed
					  HidVis_VISIBLE);

	    /***** Hidden icon with folder open *****/
	    Brw_PutIconFolderWithoutPlus (FileBrowserId,RowId,
					  true,	// Open
					  HidVis_HIDDEN);
	   }
	 else
	   {
	    /***** Hidden icon with folder closed *****/
	    Brw_PutIconFolderWithoutPlus (FileBrowserId,RowId,
					  false,	// Closed
					  HidVis_HIDDEN);

	    /***** Visible icon with folder open *****/
	    Brw_PutIconFolderWithoutPlus (FileBrowserId,RowId,
					  true,	// Open
					  HidVis_VISIBLE);
	   }
	}

   /***** End cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/******** Put icon to expand a folder without form to create inside **********/
/*****************************************************************************/

static void Brw_PutIconFolderWithoutPlus (const char *FileBrowserId,const char *RowId,
			                  bool Open,HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *Txt_Folder;
   extern const char *HidVis_ShownStyle[HidVis_NUM_HIDDEN_VISIBLE];

   /***** Begin container *****/
   HTM_DIV_Begin ("id=\"folder_%s_%s_%s\" class=\"%s\"%s",
		  Open ? "open" :
			 "closed",
		  FileBrowserId,RowId,
                  The_GetColorRows (),
		  HidVis_ShownStyle[HiddenOrVisible]);

      /***** Icon *****/
      Ico_PutIcon (Open ? "folder-open-yellow.png" :
			  "folder-yellow.png",
		   Ico_UNCHANGED,
		   Txt_Folder,"CONTEXT_OPT CONTEXT_ICO16x16");

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Put icon to expand a folder with form to create inside ***********/
/*****************************************************************************/

static void Brw_PutIconFolderWithPlus (const char *FileBrowserId,const char *RowId,
				       bool Open,HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *HidVis_ShownStyle[HidVis_NUM_HIDDEN_VISIBLE];

   /***** Begin container *****/
   HTM_DIV_Begin ("id=\"folder_%s_%s_%s\" class=\"%s\"%s",
		  Open ? "open" :
			 "closed",
		  FileBrowserId,RowId,
		  The_GetColorRows (),
		  HidVis_ShownStyle[HiddenOrVisible]);

      /***** Form and icon *****/
      Ico_PutContextualIconToCreateInFolder (Brw_ActFormCreate[Gbl.FileBrowser.Type],
					     Brw_PutImplicitParsFileBrowser,&Gbl.FileBrowser.FilFolLnk,
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
   HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
      Ico_PutIcon ("star16x16.gif",Ico_UNCHANGED,Txt_New_FILE_OR_FOLDER,"ICO20x20");
   HTM_TD_End ();
  }

/*****************************************************************************/
/***************************** Put icon of a file ****************************/
/*****************************************************************************/

static void Brw_PutIconFileWithLinkToViewMetadata (const struct Brw_FileMetadata *FileMetadata)
  {
   /***** Begin cell *****/
   HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());

      /***** Begin form *****/
      Frm_BeginForm (Brw_ActReqDatFile[Gbl.FileBrowser.Type]);
	 Brw_PutParsFileBrowser (NULL,			// Not used
				 NULL,			// Not used
				 Brw_IS_UNKNOWN,	// Not used
				 FileMetadata->FilCod);

	 /***** Icon depending on the file extension *****/
	 if (FileMetadata->FilFolLnk.Type == Brw_IS_FILE)
	    Brw_PutIconFile (FileMetadata->FilFolLnk.Name,
			     "CONTEXT_OPT ICO_HIGHLIGHT CONTEXT_ICO16x16",
			     Frm_PUT_FORM);	// Put link to view metadata
	 else
	    Ico_PutIconLink ("up-right-from-square.svg",Ico_BLACK,
	                     Brw_ActReqDatFile[Gbl.FileBrowser.Type]);


      /***** End form *****/
      Frm_EndForm ();

   /***** End cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/***************************** Put icon of a file ****************************/
/*****************************************************************************/

void Brw_PutIconFile (const char *FileName,
		      const char *Class,Frm_PutForm_t PutFormToGo)
  {
   extern const unsigned Ext_NUM_FILE_EXT_ALLOWED;
   extern const char *Ext_FileExtensionsAllowed[];
   extern const char *Txt_X_file;
   char *URL;
   char *Icon;
   char *Title;
   unsigned DocType;
   bool NotFound;

   if (asprintf (&URL,"%s32x32",
		 CfG_URL_ICON_FILEXT_PUBLIC) < 0)
      Err_NotEnoughMemoryExit ();
   for (DocType = 0, NotFound = true;
	DocType < Ext_NUM_FILE_EXT_ALLOWED && NotFound;
	DocType++)
      if (Str_FileIs (FileName,Ext_FileExtensionsAllowed[DocType]))
	{
	 if (asprintf (&Icon,"%s32x32.gif",
		       Ext_FileExtensionsAllowed[DocType]) < 0)
	    Err_NotEnoughMemoryExit ();
	 if (asprintf (&Title,Txt_X_file,
		       Ext_FileExtensionsAllowed[DocType]) < 0)
	    Err_NotEnoughMemoryExit ();
	 NotFound = false;
	}
   if (NotFound)
     {
      if (asprintf (&Icon,"xxx32x32.gif") < 0)
	 Err_NotEnoughMemoryExit ();
      if (asprintf (&Title,"%s","") < 0)
	 Err_NotEnoughMemoryExit ();
     }

   switch (PutFormToGo)
     {
      case Frm_DONT_PUT_FORM:
	 HTM_IMG (URL,Icon,Title,"class=\"%s\"",Class);
	 break;
      case Frm_PUT_FORM:
         HTM_INPUT_IMAGE (URL,Icon,Title,"class=\"%s\"",Class);
	 break;
     }
   free (Title);
   free (Icon);
   free (URL);
  }

/*****************************************************************************/
/***************** Put button to create ZIP file of a folder *****************/
/*****************************************************************************/

static void Brw_PutButtonToDownloadZIPOfAFolder (void)
  {
   /***** Form to zip and download folder *****/
   Ico_PutContextualIconToZIP (Brw_ActZIPFolder[Gbl.FileBrowser.Type],
			       Brw_PutImplicitParsFileBrowser,&Gbl.FileBrowser.FilFolLnk);
  }

/*****************************************************************************/
/********** Write central part with the name of a file or folder *************/
/*****************************************************************************/

static void Brw_WriteFileName (unsigned Level,bool IsPublic,
			       const char *TxtStyle,const char *InputStyle)
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
   switch (Gbl.FileBrowser.FilFolLnk.Type)
     {
      case Brw_IS_FOLDER:
	 /***** Begin cell *****/
	 HTM_TD_Begin (Gbl.FileBrowser.Clipboard.IsThisFile ? "class=\"%s LM LIGHT_GREEN\" style=\"width:99%%;\"" :
							      "class=\"%s LM\" style=\"width:99%%;\"",
		       TxtStyle);

	    HTM_DIV_Begin ("class=\"FILENAME\"");

	       HTM_NBSP ();

	       if (Brw_GetIfICanEditFileOrFolder ())	// Can I rename this folder?
		 {
	          /***** Form to rename folder *****/
		  Frm_BeginForm (Brw_ActRenameFolder[Gbl.FileBrowser.Type]);
		     Brw_PutImplicitParsFileBrowser (&Gbl.FileBrowser.FilFolLnk);
		     HTM_INPUT_TEXT ("NewFolderName",Brw_MAX_CHARS_FOLDER,Gbl.FileBrowser.FilFolLnk.Name,
				     HTM_SUBMIT_ON_CHANGE,
				     "class=\"LST_EDIT %s_%s %s\"",
				     InputStyle,
				     The_GetSuffix (),
				     Gbl.FileBrowser.Clipboard.IsThisFile ? "LIGHT_GREEN" :
									    The_GetColorRows ());
		  Frm_EndForm ();
		 }
	       else
		 {
	          /***** Write name of the folder *****/
		  if (Level == 1 && Brw_TypeIsAdmAsg[Gbl.FileBrowser.Type])
		     HTM_SPAN_Begin ("title=\"%s\"",Gbl.FileBrowser.Asg.Title);

		  HTM_STRONG_Begin ();
		     HTM_Txt (FileNameToShow);
		  HTM_STRONG_End ();
		  HTM_NBSP ();

		  if (Level == 1 && Brw_TypeIsAdmAsg[Gbl.FileBrowser.Type])
		     HTM_SPAN_End ();
		 }

	    /***** End cell *****/
	    HTM_DIV_End ();

	 HTM_TD_End ();
         break;
      case Brw_IS_FILE:
      case Brw_IS_LINK:
	 HTM_TD_Begin (Gbl.FileBrowser.Clipboard.IsThisFile ? "class=\"%s LM LIGHT_GREEN\" style=\"width:99%%;\"" :
							      "class=\"%s LM\" style=\"width:99%%;\"",
		       TxtStyle);

	    HTM_NBSP ();

	    Frm_BeginForm (Brw_ActDowFile[Gbl.FileBrowser.Type]);
	       Brw_PutImplicitParsFileBrowser (&Gbl.FileBrowser.FilFolLnk);

	       /* Link to the form and to the file */
	       HTM_BUTTON_Submit_Begin (Brw_TypeIsSeeMrk[Gbl.FileBrowser.Type] ? Txt_Check_marks_in_the_file :
										 Txt_Download,
					"class=\"LM BT_LINK FILENAME\"");
		  HTM_Txt (FileNameToShow);
	       HTM_BUTTON_End ();

	    Frm_EndForm ();

	    /* Put icon to make public/private file */
	    if (IsPublic)
	       Ico_PutIconOff ("unlock.svg",Ico_GREEN,
			       Txt_Public_open_educational_resource_OER_for_everyone);

	 HTM_TD_End ();
	 break;
      default:
	 break;
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
   Str_Copy (FileNameToShow,FileName,NAME_MAX);

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
   snprintf (Link,sizeof (Link),"%s/%s/%s/%s",
             Cfg_PATH_FILE_BROWSER_TMP_PUBLIC,
             Gbl.FileBrowser.TmpPubDir.L,
             Gbl.FileBrowser.TmpPubDir.R,
	     FileName);
   if (symlink (FullPathIncludingFile,Link) != 0)
      Err_ShowErrorAndExit ("Can not create temporary link.");
  }

/*****************************************************************************/
/****************** Write size and date of a file or folder ******************/
/*****************************************************************************/

static void Brw_WriteFileSizeAndDate (struct Brw_FileMetadata *FileMetadata,
				      const char *TxtStyle)
  {
   static unsigned UniqueId = 0;
   char *Id;
   char FileSizeStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];

   /***** Write the file size *****/
   if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FILE)
      Fil_WriteFileSizeBrief ((double) FileMetadata->Size,FileSizeStr);
   else
      FileSizeStr[0] = '\0';
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtStyle,The_GetColorRows ());
      HTM_TxtF ("&nbsp;%s",FileSizeStr);
   HTM_TD_End ();

   /***** Write the date *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtStyle,The_GetColorRows ());
      HTM_NBSP ();
      if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FILE ||
	  Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_LINK)
	{
	 UniqueId++;
	 if (asprintf (&Id,"filedate%u",UniqueId) < 0)
	    Err_NotEnoughMemoryExit ();
	 HTM_SPAN_Begin ("id=\"%s\"",Id);
	 HTM_SPAN_End ();
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

static void Brw_WriteFileOrFolderPublisher (unsigned Level,long UsrCod)
  {
   extern const char *Txt_Unknown_or_without_photo;
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC15x20B",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE15x20B",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO15x20B",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR15x20B",
     };
   bool ShowUsr = false;
   struct Usr_Data UsrDat;

   if (Level && UsrCod > 0)
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Get data of file/folder publisher *****/
      UsrDat.UsrCod = UsrCod;
      ShowUsr = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
                                                         Usr_DONT_GET_PREFS,
                                                         Usr_DONT_GET_ROLE_IN_CRS);
     }

   HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());

      if (ShowUsr)
	 /***** Show photo *****/
	 Pho_ShowUsrPhotoIfAllowed (&UsrDat,
	                            ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
      else
	 Ico_PutIcon ("usr_bl.jpg",Ico_UNCHANGED,Txt_Unknown_or_without_photo,
	              ClassPhoto[Gbl.Prefs.PhotoShape]);

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
   extern const char *Txt_You_can_not_remove_this_file_or_link;
   char FileNameToShow[NAME_MAX + 1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Button of confirmation of removing *****/
   if (Brw_CheckIfICanEditFileOrFolder (Gbl.FileBrowser.Level))	// Can I remove this file?
     {
      /***** Show question and button to remove file/link *****/
      Brw_GetFileNameToShowDependingOnLevel (Gbl.FileBrowser.Type,
                                             Gbl.FileBrowser.Level,
                                             Gbl.FileBrowser.FilFolLnk.Type,
                                             Gbl.FileBrowser.FilFolLnk.Name,
                                             FileNameToShow);
      Ale_ShowAlertRemove (Brw_ActRemoveFile[Gbl.FileBrowser.Type],NULL,
			   Brw_PutImplicitParsFileBrowser,&Gbl.FileBrowser.FilFolLnk,
			   Txt_Do_you_really_want_to_remove_FILE_OR_LINK_X,
                           FileNameToShow);
     }
   else
      Err_ShowErrorAndExit (Txt_You_can_not_remove_this_file_or_link);

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
      snprintf (Path,sizeof (Path),"%s/%s",
	        Gbl.FileBrowser.Path.AboveRootFolder,
	        Gbl.FileBrowser.FilFolLnk.Full);

      /***** Check if is a file/link or a folder *****/
      if (lstat (Path,&FileStatus))	// On success ==> 0 is returned
	 Err_ShowErrorAndExit ("Can not get information about a file or folder.");
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
	 Brw_DB_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
				          Gbl.Usrs.Me.UsrDat.UsrCod,
				          Gbl.Usrs.Other.UsrDat.UsrCod);

	 /* Message of confirmation of removing */
         Ale_ShowAlert (Ale_SUCCESS,Txt_FILE_X_removed,
		        FileNameToShow);
        }
      else		// File / link not found
         Err_FileFolderNotFoundExit ();
     }
   else
      Err_ShowErrorAndExit (Txt_You_can_not_remove_this_file_or_link);

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
      snprintf (Path,sizeof (Path),"%s/%s",
	        Gbl.FileBrowser.Path.AboveRootFolder,
	        Gbl.FileBrowser.FilFolLnk.Full);

      /***** Check if it's a file or a folder *****/
      if (lstat (Path,&FileStatus))	// On success ==> 0 is returned
	 Err_ShowErrorAndExit ("Can not get information about a file or folder.");
      else if (S_ISDIR (FileStatus.st_mode))		// It's a directory
	 if (Brw_RemoveFolderFromDiskAndDB (Path,
                                            Gbl.FileBrowser.FilFolLnk.Full))
           {
	    if (errno == ENOTEMPTY)	// The directory is not empty
	       Brw_AskConfirmRemoveFolderNotEmpty ();
	    else	// The directory is empty
               Err_ShowErrorAndExit ("Can not remove folder.");
           }
         else
           {
            /* Remove affected clipboards */
            Brw_DB_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
                                             Gbl.Usrs.Me.UsrDat.UsrCod,
                                             Gbl.Usrs.Other.UsrDat.UsrCod);

            /* Message of confirmation of successfull removing */
            Ale_ShowAlert (Ale_SUCCESS,Txt_Folder_X_removed,
                           Gbl.FileBrowser.FilFolLnk.Name);
           }
      else		// Folder not found
         Err_FileFolderNotFoundExit ();
     }
   else
      Err_ShowErrorAndExit (Txt_You_can_not_remove_this_folder);

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/*********** Ask for confirmation of removing of a folder no empty ***********/
/*****************************************************************************/

static void Brw_AskConfirmRemoveFolderNotEmpty (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_folder_X;

   /***** Show question and button to remove not empty folder *****/
   Ale_ShowAlertRemove (Brw_ActRemoveFolderNotEmpty[Gbl.FileBrowser.Type],NULL,
			Brw_PutImplicitParsFileBrowser,&Gbl.FileBrowser.FilFolLnk,
			Txt_Do_you_really_want_to_remove_the_folder_X,
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
      snprintf (Path,sizeof (Path),"%s/%s",
	        Gbl.FileBrowser.Path.AboveRootFolder,
	        Gbl.FileBrowser.FilFolLnk.Full);

      /***** Remove the whole tree *****/
      Fil_RemoveTree (Path);

      /* If a folder is removed,
         it is necessary to remove it from the database
         and all files or folders under that folder */
      Brw_RemoveOneFileOrFolderFromDB (Gbl.FileBrowser.FilFolLnk.Full);
      Brw_RemoveChildrenOfFolderFromDB (Gbl.FileBrowser.FilFolLnk.Full);

      /* Remove affected clipboards */
      Brw_DB_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
				       Gbl.Usrs.Me.UsrDat.UsrCod,
				       Gbl.Usrs.Other.UsrDat.UsrCod);

      /* Remove affected expanded folders */
      Brw_DB_RemoveAffectedExpandedFolders (Gbl.FileBrowser.FilFolLnk.Full);

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
   Brw_DB_RemoveExpiredClipboards ();   // Someone must do this work. Let's do it whenever a user click in a copy button

   /***** Put the path in the clipboard *****/
   if (Brw_GetMyClipboard ())
      Brw_DB_UpdatePathInClipboard ();
   else
      Brw_DB_AddPathToClipboards ();

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/********* Write a title with the content of the current clipboard ***********/
/*****************************************************************************/

static void Brw_WriteCurrentClipboard (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
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
   extern const char *Txt_HIERARCHY_SINGUL_abc[Hie_NUM_LEVELS];
   extern const char *Txt_group;
   extern const char *Txt_project;
   extern const char *Txt_user[Usr_NUM_SEXS];
   extern const char *Txt_file_folder;
   extern const char *Txt_file;
   extern const char *Txt_folder;
   extern const char *Txt_link;
   extern const char *Txt_all_files_inside_the_root_folder;
   struct Hie_Node Hie[Hie_NUM_LEVELS];
   struct GroupData GrpDat;
   struct Prj_Project Prj;
   struct Usr_Data UsrDat;
   char TxtClipboardZone[1024 +
			 Nam_MAX_BYTES_SHRT_NAME +
			 Grp_MAX_BYTES_GROUP_TYPE_NAME +
			 Grp_MAX_BYTES_GROUP_NAME +
			 Usr_MAX_BYTES_FULL_NAME +
			 Prj_MAX_BYTES_TITLE];
   char FileNameToShow[NAME_MAX + 1];
   static const char **TxtFileType[Brw_NUM_FILE_TYPES] =
     {
      [Brw_IS_UNKNOWN] = &Txt_file_folder,
      [Brw_IS_FILE   ] = &Txt_file,
      [Brw_IS_FOLDER ] = &Txt_folder,
      [Brw_IS_LINK   ] = &Txt_link,
     };

   switch (Gbl.FileBrowser.Clipboard.FileBrowser)
     {
      case Brw_ADMI_DOC_INS:
	 Hie[Hie_INS].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
	 Hie_GetDataByCod[Hie_INS] (&Hie[Hie_INS]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_documents_management_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_INS],Hie[Hie_INS].ShrtName);
         break;
      case Brw_ADMI_SHR_INS:
	 Hie[Hie_INS].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
	 Hie_GetDataByCod[Hie_INS] (&Hie[Hie_INS]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_shared_files_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_INS],Hie[Hie_INS].ShrtName);
         break;
      case Brw_ADMI_DOC_CTR:
	 Hie[Hie_CTR].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
	 Hie_GetDataByCod[Hie_CTR] (&Hie[Hie_CTR]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_documents_management_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_CTR],Hie[Hie_CTR].ShrtName);
         break;
      case Brw_ADMI_SHR_CTR:
	 Hie[Hie_CTR].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
	 Hie_GetDataByCod[Hie_CTR] (&Hie[Hie_CTR]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_shared_files_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_CTR],Hie[Hie_CTR].ShrtName);
         break;
      case Brw_ADMI_DOC_DEG:
	 Hie[Hie_DEG].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
	 Hie_GetDataByCod[Hie_DEG] (&Hie[Hie_DEG]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_documents_management_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_DEG],Hie[Hie_DEG].ShrtName);
         break;
      case Brw_ADMI_SHR_DEG:
	 Hie[Hie_DEG].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
	 Hie_GetDataByCod[Hie_DEG] (&Hie[Hie_DEG]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_shared_files_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_DEG],Hie[Hie_DEG].ShrtName);
         break;
      case Brw_ADMI_DOC_CRS:
	 Hie[Hie_CRS].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
	 Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_documents_management_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_CRS],Hie[Hie_CRS].ShrtName);
         break;
      case Brw_ADMI_DOC_GRP:
         GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.HieCod;
         Grp_GetGroupDataByCod (&GrpDat);
         Hie[Hie_CRS].HieCod = GrpDat.CrsCod;
	 Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s %s</strong>",
                   Txt_documents_management_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_CRS],Hie[Hie_CRS].ShrtName,
                   Txt_group,GrpDat.GrpTypName,GrpDat.GrpName);
         break;
      case Brw_ADMI_TCH_CRS:
	 Hie[Hie_CRS].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
	 Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_teachers_files_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_CRS],Hie[Hie_CRS].ShrtName);
         break;
      case Brw_ADMI_TCH_GRP:
         GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.HieCod;
         Grp_GetGroupDataByCod (&GrpDat);
         Hie[Hie_CRS].HieCod = GrpDat.CrsCod;
	 Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s %s</strong>",
                   Txt_teachers_files_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_CRS],Hie[Hie_CRS].ShrtName,
                   Txt_group,GrpDat.GrpTypName,GrpDat.GrpName);
         break;
      case Brw_ADMI_SHR_CRS:
	 Hie[Hie_CRS].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
	 Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_shared_files_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_CRS],Hie[Hie_CRS].ShrtName);
         break;
      case Brw_ADMI_SHR_GRP:
         GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.HieCod;
         Grp_GetGroupDataByCod (&GrpDat);
         Hie[Hie_CRS].HieCod = GrpDat.CrsCod;
	 Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s %s</strong>",
                   Txt_shared_files_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_CRS],Hie[Hie_CRS].ShrtName,
                   Txt_group,GrpDat.GrpTypName,GrpDat.GrpName);
         break;
      case Brw_ADMI_ASG_USR:
	 Hie[Hie_CRS].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
	 Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s</strong>",
                   Txt_assignments_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_CRS],Hie[Hie_CRS].ShrtName,
                   Txt_user[Gbl.Usrs.Me.UsrDat.Sex],Gbl.Usrs.Me.UsrDat.FullName);
         break;
      case Brw_ADMI_WRK_USR:
	 Hie[Hie_CRS].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
	 Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s</strong>",
                   Txt_works_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_CRS],Hie[Hie_CRS].ShrtName,
                   Txt_user[Gbl.Usrs.Me.UsrDat.Sex],Gbl.Usrs.Me.UsrDat.FullName);
         break;
      case Brw_ADMI_ASG_CRS:
	 Hie[Hie_CRS].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
	 Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]);
         Usr_UsrDataConstructor (&UsrDat);
         UsrDat.UsrCod = Gbl.FileBrowser.Clipboard.WorksUsrCod;
         Usr_GetAllUsrDataFromUsrCod (&UsrDat,
                                      Usr_DONT_GET_PREFS,
                                      Usr_DONT_GET_ROLE_IN_CRS);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s</strong>",
                   Txt_assignments_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_CRS],Hie[Hie_CRS].ShrtName,
                   Txt_user[UsrDat.Sex],UsrDat.FullName);
         Usr_UsrDataDestructor (&UsrDat);
         break;
      case Brw_ADMI_WRK_CRS:
	 Hie[Hie_CRS].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
	 Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]);
         Usr_UsrDataConstructor (&UsrDat);
         UsrDat.UsrCod = Gbl.FileBrowser.Clipboard.WorksUsrCod;
         Usr_GetAllUsrDataFromUsrCod (&UsrDat,
                                      Usr_DONT_GET_PREFS,
                                      Usr_DONT_GET_ROLE_IN_CRS);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s</strong>",
                   Txt_works_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_CRS],Hie[Hie_CRS].ShrtName,
                   Txt_user[UsrDat.Sex],UsrDat.FullName);
         Usr_UsrDataDestructor (&UsrDat);
         break;
      case Brw_ADMI_DOC_PRJ:
      case Brw_ADMI_ASS_PRJ:
	 Prj_AllocMemProject (&Prj);
         Prj.PrjCod = Gbl.FileBrowser.Clipboard.HieCod;
         Prj_GetProjectDataByCod (&Prj);
         Hie[Hie_CRS].HieCod = Prj.CrsCod;
	 Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s</strong>",
                   Gbl.FileBrowser.Clipboard.FileBrowser == Brw_ADMI_DOC_PRJ ? Txt_project_documents :
                                                                               Txt_project_assessment,
                   Txt_HIERARCHY_SINGUL_abc[Hie_CRS],Hie[Hie_CRS].ShrtName,
                   Txt_project,Prj.Title);
         Prj_FreeMemProject (&Prj);
         break;
      case Brw_ADMI_MRK_CRS:
	 Hie[Hie_CRS].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
	 Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>",
                   Txt_marks_management_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_CRS],Hie[Hie_CRS].ShrtName);
         break;
      case Brw_ADMI_MRK_GRP:
         GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.HieCod;
         Grp_GetGroupDataByCod (&GrpDat);
         Hie[Hie_CRS].HieCod = GrpDat.CrsCod;
	 Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]);
         snprintf (TxtClipboardZone,sizeof (TxtClipboardZone),
                   "%s, %s <strong>%s</strong>, %s <strong>%s %s</strong>",
                   Txt_marks_management_area,
                   Txt_HIERARCHY_SINGUL_abc[Hie_CRS],Hie[Hie_CRS].ShrtName,
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
                     *TxtFileType[Gbl.FileBrowser.Clipboard.FilFolLnk.Type],
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
   Gbl.FileBrowser.Clipboard.FileBrowser       = Brw_UNKNOWN;
   Gbl.FileBrowser.Clipboard.HieCod               = -1L;
   Gbl.FileBrowser.Clipboard.WorksUsrCod       = -1L;
   Gbl.FileBrowser.Clipboard.FilFolLnk.Type    = Brw_IS_UNKNOWN;
   Gbl.FileBrowser.Clipboard.FilFolLnk.Full[0] = '\0';
   Gbl.FileBrowser.Clipboard.FilFolLnk.Name[0] = '\0';
   Gbl.FileBrowser.Clipboard.Level             = 0;

   /***** Get my current clipboard from database *****/
   if ((NumRows = Brw_DB_GetMyClipboard (&mysql_res)) == 1)
     {
      /***** Get clipboard data *****/
      row = mysql_fetch_row (mysql_res);

      /* Get file browser type (row[0]) */
      if (sscanf (row[0],"%u",&UnsignedNum) == 1)
        {
         Gbl.FileBrowser.Clipboard.FileBrowser = (Brw_FileBrowser_t) UnsignedNum;

         /* Get institution/center/degree/course/group code (row[1]) */
         Gbl.FileBrowser.Clipboard.HieCod = Str_ConvertStrCodToLongCod (row[1]);

         /* Get works user's code (row[2]) */
         Gbl.FileBrowser.Clipboard.WorksUsrCod = Str_ConvertStrCodToLongCod (row[2]);

         /* Get file type (row[3]) */
         Gbl.FileBrowser.Clipboard.FilFolLnk.Type = Brw_IS_UNKNOWN;	// default
         if (sscanf (row[3],"%u",&UnsignedNum) == 1)
            if (UnsignedNum < Brw_NUM_FILE_TYPES)
               Gbl.FileBrowser.Clipboard.FilFolLnk.Type = (Brw_FileType_t) UnsignedNum;

         /* Get file path (row[4]) */
         Str_Copy (Gbl.FileBrowser.Clipboard.FilFolLnk.Full,row[4],
                   sizeof (Gbl.FileBrowser.Clipboard.FilFolLnk.Full) - 1);
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
      Err_WrongCopySrcExit ();

   return NumRows == 1;
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
            if (Gbl.FileBrowser.Clipboard.HieCod == Gbl.Hierarchy.Node[Hie_INS].HieCod)
               return true;		// I am in the institution of the clipboard
            break;
	 case Brw_ADMI_DOC_CTR:
         case Brw_ADMI_SHR_CTR:
            if (Gbl.FileBrowser.Clipboard.HieCod == Gbl.Hierarchy.Node[Hie_CTR].HieCod)
               return true;		// I am in the center of the clipboard
            break;
	 case Brw_ADMI_DOC_DEG:
         case Brw_ADMI_SHR_DEG:
            if (Gbl.FileBrowser.Clipboard.HieCod == Gbl.Hierarchy.Node[Hie_DEG].HieCod)
               return true;		// I am in the degree of the clipboard
            break;
         case Brw_ADMI_DOC_CRS:
         case Brw_ADMI_TCH_CRS:
         case Brw_ADMI_SHR_CRS:
         case Brw_ADMI_MRK_CRS:
         case Brw_ADMI_ASG_USR:
         case Brw_ADMI_WRK_USR:
            if (Gbl.FileBrowser.Clipboard.HieCod == Gbl.Hierarchy.Node[Hie_CRS].HieCod)
               return true;		// I am in the course of the clipboard
            break;
	 case Brw_ADMI_ASG_CRS:
	 case Brw_ADMI_WRK_CRS:
            if (Gbl.FileBrowser.Clipboard.HieCod == Gbl.Hierarchy.Node[Hie_CRS].HieCod &&
	        Gbl.FileBrowser.Clipboard.WorksUsrCod == Gbl.Usrs.Other.UsrDat.UsrCod)
               return true;		// I am in the course of the clipboard
					// I am in the student's works of the clipboard
	    break;
	 case Brw_ADMI_DOC_GRP:
	 case Brw_ADMI_TCH_GRP:
	 case Brw_ADMI_SHR_GRP:
	 case Brw_ADMI_MRK_GRP:
            if (Gbl.FileBrowser.Clipboard.HieCod == Gbl.Crs.Grps.GrpCod)
               return true;		// I am in the group of the clipboard
            break;
	 case Brw_ADMI_DOC_PRJ:
	 case Brw_ADMI_ASS_PRJ:
            if (Gbl.FileBrowser.Clipboard.HieCod == Prj_GetPrjCod ())
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
/* Get code of institution, degree, course, group depending on file browser **/
/*****************************************************************************/

long Brw_GetCodForFileBrowser (void)
  {
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_SHOW_DOC_INS:
      case Brw_ADMI_DOC_INS:
      case Brw_ADMI_SHR_INS:
	 return Gbl.Hierarchy.Node[Hie_INS].HieCod;
      case Brw_SHOW_DOC_CTR:
      case Brw_ADMI_DOC_CTR:
      case Brw_ADMI_SHR_CTR:
	 return Gbl.Hierarchy.Node[Hie_CTR].HieCod;
      case Brw_SHOW_DOC_DEG:
      case Brw_ADMI_DOC_DEG:
      case Brw_ADMI_SHR_DEG:
	 return Gbl.Hierarchy.Node[Hie_DEG].HieCod;
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
	 return Gbl.Hierarchy.Node[Hie_CRS].HieCod;
      case Brw_SHOW_DOC_GRP:
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_GRP:
      case Brw_ADMI_SHR_GRP:
      case Brw_SHOW_MRK_GRP:
      case Brw_ADMI_MRK_GRP:
	 return Gbl.Crs.Grps.GrpCod;
      case Brw_ADMI_DOC_PRJ:
      case Brw_ADMI_ASS_PRJ:
	 return Prj_GetPrjCod ();
      case Brw_ADMI_BRF_USR:
	 return -1L;
      default:
	 return -1L;
     }
  }

/*****************************************************************************/
/******** Get code of user in assignment / works for expanded folders ********/
/*****************************************************************************/

long Brw_GetZoneUsrCodForFileBrowser (void)
  {
   if (Brw_TypeIsAdmBrf[Gbl.FileBrowser.Type])		// My briefcase
      return Gbl.Usrs.Me.UsrDat.UsrCod;

   if (Brw_TypeIsAdmUsrAsgWrk[Gbl.FileBrowser.Type])	// My assignments or works
      return Gbl.Usrs.Me.UsrDat.UsrCod;

   if (Brw_TypeIsAdmCrsAsgWrk[Gbl.FileBrowser.Type])	// Course assignments or works
      return Gbl.Usrs.Other.UsrDat.UsrCod;

   return -1L;
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
   Str_Copy (CopyOfPath,Path,sizeof (CopyOfPath) - 1);

   /***** Insert paths in table of expanded folders if they are not yet there *****/
   do
     {
      if ((Ptr = strrchr (CopyOfPath,'/')))	// If '/' found (backwards from the end)
	{
	 if (Brw_DB_GetIfContractedOrExpandedFolder (CopyOfPath) == ConExp_CONTRACTED)
	    Brw_DB_InsertFolderInExpandedFolders (CopyOfPath);
	 // Now Ptr points to the last '/' in SubPath
	 *Ptr = '\0';	// Substitute '/' for '\0' to shorten CopyOfPath
	}
     }
   while (Ptr);

   /***** Update paths of the current file browser in table of expanded folders *****/
   Brw_DB_UpdateClickTimeOfThisFileBrowserInExpandedFolders ();
  }

/*****************************************************************************/
/******* Remove specified folder from table of expanded folders       ********/
/******* and update click time of the other folders in the expl. tree ********/
/*****************************************************************************/

static void Brw_RemThisFolderAndUpdOtherFoldersFromExpandedFolders (const char Path[PATH_MAX + 1])
  {
   /***** Remove Path from expanded folders table *****/
   Brw_DB_RemoveFolderFromExpandedFolders (Path);

   /***** Update paths of the current file browser in table of expanded folders *****/
   Brw_DB_UpdateClickTimeOfThisFileBrowserInExpandedFolders ();
  }

/*****************************************************************************/
/**** Paste the arch/carp indicado in the portapapelesde in file browser *****/
/*****************************************************************************/

void Brw_PasteIntoFileBrowser (void)
  {
   extern const char *Txt_Nothing_has_been_pasted_because_the_clipboard_is_empty_;
   struct GroupData GrpDat;
   struct BrwSiz_BrowserSize *Size = BrwSiz_GetSize ();

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
	    GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.HieCod;
	    Brw_GetSelectedGroupData (&GrpDat,true);
	    break;
         default:
            break;
        }

      /***** Copy files recursively *****/
      Brw_PasteClipboard (Size);

      /***** Remove the affected clipboards *****/
      Brw_DB_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
				       Gbl.Usrs.Me.UsrDat.UsrCod,
				       Gbl.Usrs.Other.UsrDat.UsrCod);
     }
   else
      /***** Write message ******/
      Ale_ShowAlert (Ale_WARNING,Txt_Nothing_has_been_pasted_because_the_clipboard_is_empty_);

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/************* Paste all clipboard content at current location ***************/
/*****************************************************************************/
// Source:
//	Type of file browser:		Gbl.FileBrowser.Clipboard.FileBrowser
//	Possible institution:		Gbl.FileBrowser.Clipboard.InsCod
//	Possible center:		Gbl.FileBrowser.Clipboard.CtrCod
//	Possible degree:		Gbl.FileBrowser.Clipboard.DegCod
//	Possible course:		Gbl.FileBrowser.Clipboard.CrsCod
//	Possible student in works:	Gbl.FileBrowser.Clipboard.WorksUsrCod
//	Path (file or folder):		Gbl.FileBrowser.Clipboard.FilFolLnk.Full
// Destination:
//	Type of file browser:		Gbl.FileBrowser.Type
//	Possible institution:		Gbl.Hierarchy.Node[Hie_INS].InsCod
//	Possible center:		Gbl.Hierarchy.Node[Hie_CTR].CtrCod
//	Possible degree:		Gbl.Hierarchy.Node[Hie_DEG].DegCod
//	Possible course:		Gbl.Hierarchy.Node[Hie_CRS].CrsCod
//	Possible student in works:	Gbl.Usrs.Other.UsrDat.UsrCod
//	Path (should be a folder):	Gbl.FileBrowser.FilFolLnk.Full
// Returns the number of files pasted

static void Brw_PasteClipboard (struct BrwSiz_BrowserSize *Size)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_The_copy_has_been_successful;
   extern const char *Txt_Files_copied;
   extern const char *Txt_Links_copied;
   extern const char *Txt_Folders_copied;
   extern const char *Txt_You_can_not_paste_file_or_folder_here;
   struct Hie_Node Hie[Hie_NUM_LEVELS];
   struct GroupData GrpDat;
   struct Usr_Data UsrDat;
   long PrjCod;
   char PathOrg[PATH_MAX + NAME_MAX + PATH_MAX + 128];
   struct Brw_NumObjects Pasted;
   long FirstFilCod = -1L;	// First file code of the first file or link pasted. Important: initialize here to -1L
   struct Brw_FileMetadata FileMetadata;

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
            Hie[Hie_INS].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
            if (Hie_GetDataByCod[Hie_INS] (&Hie[Hie_INS]))
	       snprintf (PathOrg,sizeof (PathOrg),"%s/%02u/%u/%s",
		         Cfg_PATH_INS_PRIVATE,
		         (unsigned) (Hie[Hie_INS].HieCod % 100),
		         (unsigned) Hie[Hie_INS].HieCod,
			 Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
            else
               Err_WrongCopySrcExit ();
            break;
         case Brw_ADMI_DOC_CTR:
         case Brw_ADMI_SHR_CTR:
            Hie[Hie_CTR].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
            if (Hie_GetDataByCod[Hie_CTR] (&Hie[Hie_CTR]))
	       snprintf (PathOrg,sizeof (PathOrg),"%s/%02u/%u/%s",
		         Cfg_PATH_CTR_PRIVATE,
		         (unsigned) (Hie[Hie_CTR].HieCod % 100),
		         (unsigned) Hie[Hie_CTR].HieCod,
			 Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
            else
               Err_WrongCopySrcExit ();
            break;
         case Brw_ADMI_DOC_DEG:
         case Brw_ADMI_SHR_DEG:
            Hie[Hie_DEG].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
            if (Hie_GetDataByCod[Hie_DEG] (&Hie[Hie_DEG]))
	       snprintf (PathOrg,sizeof (PathOrg),"%s/%02u/%u/%s",
		         Cfg_PATH_DEG_PRIVATE,
		         (unsigned) (Hie[Hie_DEG].HieCod % 100),
		         (unsigned) Hie[Hie_DEG].HieCod,
			 Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
            else
               Err_WrongCopySrcExit ();
            break;
         case Brw_ADMI_DOC_CRS:
         case Brw_ADMI_TCH_CRS:
         case Brw_ADMI_SHR_CRS:
         case Brw_ADMI_MRK_CRS:
            Hie[Hie_CRS].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
            if (Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]))
	       snprintf (PathOrg,sizeof (PathOrg),"%s/%ld/%s",
                         Cfg_PATH_CRS_PRIVATE,Hie[Hie_CRS].HieCod,
			 Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
            else
               Err_WrongCopySrcExit ();
            break;
         case Brw_ADMI_DOC_GRP:
         case Brw_ADMI_TCH_GRP:
         case Brw_ADMI_SHR_GRP:
         case Brw_ADMI_MRK_GRP:
	    GrpDat.GrpCod = Gbl.FileBrowser.Clipboard.HieCod;
	    Grp_GetGroupDataByCod (&GrpDat);
	    Hie[Hie_CRS].HieCod = GrpDat.CrsCod;
            if (Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]))
	       snprintf (PathOrg,sizeof (PathOrg),"%s/%ld/%s/%ld/%s",
                         Cfg_PATH_CRS_PRIVATE,Hie[Hie_CRS].HieCod,Cfg_FOLDER_GRP,
			 GrpDat.GrpCod,
			 Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
            else
               Err_WrongCopySrcExit ();
            break;
         case Brw_ADMI_ASG_CRS:
         case Brw_ADMI_WRK_CRS:
            Hie[Hie_CRS].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
            if (Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]))
              {
               Usr_UsrDataConstructor (&UsrDat);
               if (Usr_DB_ChkIfUsrCodExists (Gbl.FileBrowser.Clipboard.WorksUsrCod))

	       UsrDat.UsrCod = Gbl.FileBrowser.Clipboard.WorksUsrCod;
	       Usr_GetAllUsrDataFromUsrCod (&UsrDat,
	                                    Usr_DONT_GET_PREFS,
	                                    Usr_DONT_GET_ROLE_IN_CRS);	// Check that user exists
	       snprintf (PathOrg,sizeof (PathOrg),"%s/%ld/%s/%02u/%ld/%s",
                         Cfg_PATH_CRS_PRIVATE,Hie[Hie_CRS].HieCod,Cfg_FOLDER_USR,
			 (unsigned) (Gbl.FileBrowser.Clipboard.WorksUsrCod % 100),
			 Gbl.FileBrowser.Clipboard.WorksUsrCod,
			 Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
	       Usr_UsrDataDestructor (&UsrDat);
              }
            else
               Err_WrongCopySrcExit ();
            break;
         case Brw_ADMI_ASG_USR:
         case Brw_ADMI_WRK_USR:
            Hie[Hie_CRS].HieCod = Gbl.FileBrowser.Clipboard.HieCod;
            if (Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]))
	       snprintf (PathOrg,sizeof (PathOrg),"%s/%ld/%s/%02u/%ld/%s",
                         Cfg_PATH_CRS_PRIVATE,Hie[Hie_CRS].HieCod,Cfg_FOLDER_USR,
			 (unsigned) (Gbl.Usrs.Me.UsrDat.UsrCod % 100),
			 Gbl.Usrs.Me.UsrDat.UsrCod,
			 Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
            else
               Err_WrongCopySrcExit ();
            break;
         case Brw_ADMI_DOC_PRJ:
         case Brw_ADMI_ASS_PRJ:
            PrjCod = Gbl.FileBrowser.Clipboard.HieCod;
            Hie[Hie_CRS].HieCod = Prj_DB_GetCrsOfPrj (PrjCod);
	    if (Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]))
	       snprintf (PathOrg,sizeof (PathOrg),"%s/%ld/%s/%02u/%ld/%s",
			Cfg_PATH_CRS_PRIVATE,Hie[Hie_CRS].HieCod,Cfg_FOLDER_PRJ,
			(unsigned) (PrjCod % 100),
			PrjCod,
			Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
	    else
	       Err_WrongCopySrcExit ();
            break;
         case Brw_ADMI_BRF_USR:
            snprintf (PathOrg,sizeof (PathOrg),"%s/%s",
        	      Gbl.Usrs.Me.PathDir,
        	      Gbl.FileBrowser.Clipboard.FilFolLnk.Full);
            break;
         default:
            Err_WrongFileBrowserExit ();
            break;
        }

      /***** Paste tree (path in clipboard) into folder *****/
      BrwSiz_CalcSizeOfDir (Size,Gbl.FileBrowser.Path.RootFolder);
      BrwSiz_SetMaxQuota (Size);
      if (Brw_PasteTreeIntoFolder (Size,
	                           Gbl.FileBrowser.Clipboard.Level,
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
	    if (!Brw_DB_CheckIfFileOrFolderIsSetAsHiddenUsingMetadata (&FileMetadata))
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
      Err_ShowErrorAndExit (Txt_You_can_not_paste_file_or_folder_here);	// It's difficult, but not impossible that a user sees this message
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

static bool Brw_PasteTreeIntoFolder (struct BrwSiz_BrowserSize *Size,
                                     unsigned LevelOrg,
                                     const char PathOrg[PATH_MAX + 1],
                                     const char PathDstInTree[PATH_MAX + 1],
                                     struct Brw_NumObjects *Pasted,
                                     long *FirstFilCod)
  {
   extern const char *Txt_The_copy_has_stopped_when_trying_to_paste_X_because_it_would_exceed_the_disk_quota;
   extern const char *Txt_The_copy_has_stopped_when_trying_to_paste_X_because_it_would_exceed_the_maximum_allowed_number_of_levels;
   extern const char *Txt_The_copy_has_stopped_when_trying_to_paste_X_because_there_is_already_an_object_with_that_name;
   extern const char *Txt_The_copy_has_stopped_when_trying_to_paste_X_because_you_can_not_paste_a_file_here_of_a_type_other_than_HTML;
   Brw_FileType_t FileType;
   char PathUntilFileNameOrg[PATH_MAX + 1];
   char FileNameOrg[NAME_MAX + 1];
   char FileNameToShow[NAME_MAX + 1];
   char PathInFolderOrg[PATH_MAX + 1];
   char PathDstInTreeWithFile[PATH_MAX + 1 + NAME_MAX + 1];
   char PathDstWithFile[PATH_MAX + 1 + PATH_MAX + 1 + NAME_MAX + 1];
   struct stat FileStatus;
   struct dirent **FileList;
   struct Mrk_Properties Marks;
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
      Err_ShowErrorAndExit ("Can not get information about a file or folder.");
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
                sizeof (PathDstInTreeWithFile) - 1);
   else			// Origin of copy is a file or folder inside the root folder
			// for example "sha/folder1/file1"
      snprintf (PathDstInTreeWithFile,sizeof (PathDstInTreeWithFile),"%s/%s",
	        PathDstInTree,FileNameOrg);

   /***** Construct the relative path of the destination file or folder *****/
   snprintf (PathDstWithFile,sizeof (PathDstWithFile),"%s/%s",
	     Gbl.FileBrowser.Path.AboveRootFolder,
	     PathDstInTreeWithFile);

   /***** Update and check number of levels *****/
   // The number of levels is counted starting on the root folder raíz, not included.
   // Example:	If PathDstInTreeWithFile is "root-folder/1/2/3/4/FileNameOrg", then NumLevls=5
   if ((NumLevls = Brw_NumLevelsInPath (PathDstInTreeWithFile)) > Size->NumLevls)
      Size->NumLevls = NumLevls;

   if (BrwSiz_CheckIfQuotaExceded (Size))
     {
      Ale_ShowAlert (Ale_WARNING,Txt_The_copy_has_stopped_when_trying_to_paste_X_because_it_would_exceed_the_maximum_allowed_number_of_levels,
		     FileNameToShow);
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
	    Ale_ShowAlert (Ale_WARNING,Txt_The_copy_has_stopped_when_trying_to_paste_X_because_there_is_already_an_object_with_that_name,
		           FileNameToShow);
	    CopyIsGoingSuccessful = false;
	   }
	 else	// Destination file does not exist
	   {
	    /***** If the target file browser is that of marks, only HTML files are allowed *****/
	    if (Brw_TypeIsAdmMrk[Gbl.FileBrowser.Type])
	      {
	       /* Check extension of the file */
	       if (Str_FileIsHTML (FileNameOrg))
		  Mrk_CheckFileOfMarks (PathOrg,&Marks);
	       else
		 {
		  Ale_ShowAlert (Ale_WARNING,Txt_The_copy_has_stopped_when_trying_to_paste_X_because_you_can_not_paste_a_file_here_of_a_type_other_than_HTML,
			         FileNameToShow);
	          CopyIsGoingSuccessful = false;
		 }
	      }

            if (CopyIsGoingSuccessful)
              {
	       /***** Update and check the quota before copying the file *****/
	       Size->NumFiles++;
	       Size->TotalSiz += (unsigned long long) FileStatus.st_size;
	       if (BrwSiz_CheckIfQuotaExceded (Size))
		 {
		  Ale_ShowAlert (Ale_WARNING,Txt_The_copy_has_stopped_when_trying_to_paste_X_because_it_would_exceed_the_disk_quota,
			         FileNameToShow);
		  CopyIsGoingSuccessful = false;
		 }
	       else	// Quota not exceeded
		 {
		  /***** Quota will not be exceeded ==> copy the origin file to the destination file *****/
		  Fil_FastCopyOfFiles (PathOrg,PathDstWithFile);

		  /***** Add entry to the table of files/folders *****/
		  FilCod = Brw_DB_AddPath (Gbl.Usrs.Me.UsrDat.UsrCod,FileType,
					   PathDstInTreeWithFile,false,Brw_LICENSE_DEFAULT);
		  if (*FirstFilCod <= 0)
		     *FirstFilCod = FilCod;

		  /* Add a new entry of marks into database */
		  if (Brw_TypeIsAdmMrk[Gbl.FileBrowser.Type])
		     Mrk_DB_AddMarks (FilCod,&Marks);

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
	       Size->NumFolds++;
	       Size->TotalSiz += (unsigned long long) FileStatus.st_size;
	       if (BrwSiz_CheckIfQuotaExceded (Size))
		 {
		  Ale_ShowAlert (Ale_WARNING,Txt_The_copy_has_stopped_when_trying_to_paste_X_because_it_would_exceed_the_disk_quota,
			         FileNameToShow);
		  CopyIsGoingSuccessful = false;
		 }
	       else	// Quota not exceded
		 {
		  /* Create directory */
		  if (mkdir (PathDstWithFile,(mode_t) 0xFFF) != 0)
		     Err_ShowErrorAndExit ("Can not create folder.");

		  /* Add entry to the table of files/folders */
		  Brw_DB_AddPath (Gbl.Usrs.Me.UsrDat.UsrCod,FileType,
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
		  snprintf (PathInFolderOrg,sizeof (PathInFolderOrg),"%s/%s",
			    PathOrg,FileList[NumFile]->d_name);
		  /* Recursive call to this function */
		  if (!Brw_PasteTreeIntoFolder (Size,
		                                LevelOrg + 1,
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
	    Err_ShowErrorAndExit ("Error while scanning directory.");

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
      Err_ShowErrorAndExit (Txt_You_can_not_create_folders_files_or_links_here);	// It's difficult, but not impossible that a user sees this message

      /***** Show again file browser *****/
      Brw_ShowAgainFileBrowserOrWorks ();
     }
  }

/*****************************************************************************/
/************* Put form to create a new folder in a file browser *************/
/*****************************************************************************/

static void Brw_PutFormToCreateAFolder (const char FileNameToShow[NAME_MAX + 1])
  {
   extern const char *Txt_Create_folder;
   extern const char *Txt_You_can_create_a_new_folder_inside_the_folder_X;
   extern const char *Txt_Folder;
   extern const char *Txt_Create;

   /***** Begin form *****/
   Frm_BeginForm (Brw_ActCreateFolder[Gbl.FileBrowser.Type]);
      Brw_PutImplicitParsFileBrowser (&Gbl.FileBrowser.FilFolLnk);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Create_folder,NULL,NULL,NULL,Box_NOT_CLOSABLE);

         /* Alert */
	 Ale_ShowAlert (Ale_INFO,Txt_You_can_create_a_new_folder_inside_the_folder_X,
			FileNameToShow);

	 /* Folder */
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_TxtColonNBSP (Txt_Folder);
	    HTM_INPUT_TEXT ("NewFolderName",Brw_MAX_CHARS_FOLDER,"",
			    HTM_DONT_SUBMIT_ON_CHANGE,
			    "size=\"30\" class=\"INPUT_%s\""
			    " required=\"required\"",
			    The_GetSuffix ());
	 HTM_LABEL_End ();

      /***** Send button and end box *****/
      Box_BoxWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create);

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
      Box_BoxBegin (Txt_Upload_files,NULL,NULL,NULL,Box_NOT_CLOSABLE);

	 /***** Help message *****/
	 Ale_ShowAlert (Ale_INFO,Txt_or_you_can_upload_new_files_to_the_folder_X,
			FileNameToShow);

	 /***** Form to upload files using the library Dropzone.js *****/
	 // Use min-height:125px; or other number to stablish the height?
	 HTM_TxtF ("<form method=\"post\" action=\"%s/%s\""
		   " class=\"dropzone\""
		   " enctype=\"multipart/form-data\""
		   " id=\"my-awesome-dropzone\""
		   " style=\"display:inline-block; width:100%%;"
		   " background:url('%s/upload320x320.gif') no-repeat center;\">",
		   Cfg_URL_SWAD_CGI,
		   Lan_STR_LANG_ID[Gbl.Prefs.Language],
		   Cfg_URL_ICON_PUBLIC);
	 Par_PutParLong (NULL,"act",Act_GetActCod (Brw_ActUploadFileDropzone[Gbl.FileBrowser.Type]));
	 Par_PutParString (NULL,"ses",Gbl.Session.Id);
	 Brw_PutImplicitParsFileBrowser (&Gbl.FileBrowser.FilFolLnk);

	 HTM_DIV_Begin ("class=\"dz-message\"");
	    HTM_SPAN_Begin ("class=\"DAT_LIGHT_%s\"",
	                    The_GetSuffix ());
	       HTM_Txt (Txt_Select_one_or_more_files_from_your_computer_or_drag_and_drop_here);
	    HTM_SPAN_End ();
	 HTM_DIV_End ();

	 HTM_Txt ("</form>");

	 /***** Put button to refresh file browser after upload *****/
	 Frm_BeginForm (Brw_ActRefreshAfterUploadFiles[Gbl.FileBrowser.Type]);
	    Brw_PutParsFileBrowser (NULL,		// Not used
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
      Box_BoxBegin (Txt_Upload_file,NULL,NULL,NULL,Box_NOT_CLOSABLE);

	 /***** Help message *****/
	 Ale_ShowAlert (Ale_INFO,Txt_or_you_can_upload_a_new_file_to_the_folder_X,
			FileNameToShow);

	 /***** Form to upload one files using the classic way *****/
	 Frm_BeginForm (Brw_ActUploadFileClassic[Gbl.FileBrowser.Type]);
	    Brw_PutImplicitParsFileBrowser (&Gbl.FileBrowser.FilFolLnk);

	    /* File */
	    HTM_INPUT_FILE (Fil_NAME_OF_PARAM_FILENAME_ORG,"*",
			    HTM_DONT_SUBMIT_ON_CHANGE,
			    NULL);

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
   Frm_BeginForm (Brw_ActPaste[Gbl.FileBrowser.Type]);
      Brw_PutImplicitParsFileBrowser (&Gbl.FileBrowser.FilFolLnk);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Paste,NULL,NULL,NULL,Box_NOT_CLOSABLE);

	 /***** Help message *****/
	 Ale_ShowAlert (Ale_INFO,Txt_or_you_can_make_a_file_copy_to_the_folder_X,
			FileNameToShow);

      /***** Send button and end box *****/
      Box_BoxWithButtonEnd (Btn_CREATE_BUTTON,Txt_Paste);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/************** Put form to create a new link in a file browser **************/
/*****************************************************************************/

static void Brw_PutFormToCreateALink (const char *FileNameToShow)
  {
   extern const char *Txt_Create_link;
   extern const char *Txt_or_you_can_create_a_new_link_inside_the_folder_X;
   extern const char *Txt_URL;
   extern const char *Txt_Save_as;
   extern const char *Txt_optional;
   extern const char *Txt_Create;
   char *Label;

   /***** Begin form *****/
   Frm_BeginForm (Brw_ActCreateLink[Gbl.FileBrowser.Type]);
      Brw_PutImplicitParsFileBrowser (&Gbl.FileBrowser.FilFolLnk);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Create_link,NULL,NULL,NULL,Box_NOT_CLOSABLE);

      /***** Help message *****/
      Ale_ShowAlert (Ale_INFO,Txt_or_you_can_create_a_new_link_inside_the_folder_X,
		     FileNameToShow);

      /***** URL *****/
      HTM_TABLE_Begin (NULL);

	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT","NewLinkURL",Txt_URL);

	    /* Data */
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_INPUT_URL ("NewLinkURL","",HTM_DONT_SUBMIT_ON_CHANGE,
			      "size=\"30\" class=\"INPUT_%s\""
			      " required=\"required\"",
			      The_GetSuffix ());
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Link name *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    if (asprintf (&Label,"%s&nbsp;(%s):&nbsp;",Txt_Save_as,Txt_optional) < 0)
	       Err_NotEnoughMemoryExit ();
	    Frm_LabelColumn ("RT","NewLinkName",Label);
	    free (Label);

	    /* Data */
	    HTM_TD_Begin ("class=\"LM\"");
	       HTM_INPUT_TEXT ("NewLinkName",Brw_MAX_CHARS_FOLDER,"",
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "id=\"NewLinkName\" size=\"30\""
			       " class=\"INPUT_%s\"",
			       The_GetSuffix ());
	    HTM_TD_End ();

	 HTM_TR_End ();

      HTM_TABLE_End ();

      /***** Send button and end box *****/
      Box_BoxWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create);

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
   struct BrwSiz_BrowserSize *Size = BrwSiz_GetSize ();

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Check if creating a new folder is allowed *****/
   if (Brw_CheckIfICanCreateIntoFolder (Gbl.FileBrowser.Level))
     {
      if (Str_ConvertFilFolLnkNameToValid (Gbl.FileBrowser.NewFilFolLnkName))
        {
         /* In Gbl.FileBrowser.NewFilFolLnkName is the name of the new folder */
         snprintf (Path,sizeof (Path),"%s/%s",
        	   Gbl.FileBrowser.Path.AboveRootFolder,
		   Gbl.FileBrowser.FilFolLnk.Full);

         if (strlen (Path) + 1 + strlen (Gbl.FileBrowser.NewFilFolLnkName) > PATH_MAX)
	    Err_ShowErrorAndExit ("Path is too long.");
         Str_Concat (Path,"/",sizeof (Path) - 1);
         Str_Concat (Path,Gbl.FileBrowser.NewFilFolLnkName,sizeof (Path) - 1);

         /* Create the new directory */
         if (mkdir (Path,(mode_t) 0xFFF) == 0)
	   {
	    /* Check if quota has been exceeded */
	    BrwSiz_CalcSizeOfDir (Size,Gbl.FileBrowser.Path.RootFolder);
	    BrwSiz_SetMaxQuota (Size);
            if (BrwSiz_CheckIfQuotaExceded (Size))
	      {
	       Fil_RemoveTree (Path);
               Ale_ShowAlert (Ale_WARNING,Txt_Can_not_create_the_folder_X_because_it_would_exceed_the_disk_quota,
                              Gbl.FileBrowser.NewFilFolLnkName);
	      }
	    else
              {
               /* Remove affected clipboards */
               Brw_DB_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
        				        Gbl.Usrs.Me.UsrDat.UsrCod,
					        Gbl.Usrs.Other.UsrDat.UsrCod);

               /* Add path where new file is created to table of expanded folders */
               Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.FilFolLnk.Full);

               /* Add entry to the table of files/folders */
               snprintf (PathCompleteInTreeIncludingFolder,
                         sizeof (PathCompleteInTreeIncludingFolder),
			 "%s/%s",
			 Gbl.FileBrowser.FilFolLnk.Full,
			 Gbl.FileBrowser.NewFilFolLnkName);
               Brw_DB_AddPath (Gbl.Usrs.Me.UsrDat.UsrCod,Brw_IS_FOLDER,
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
	          Err_ShowErrorAndExit ("Write forbidden.");
	          break;
	       default:
	          Err_ShowErrorAndExit ("Can not create folder.");
	          break;
	      }
	   }
        }
      else	// Folder name not valid
         Ale_ShowAlerts (NULL);
     }
   else
      Err_ShowErrorAndExit (Txt_You_can_not_create_folders_here);	// It's difficult, but not impossible that a user sees this message

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/**************** Rename an existing folder in a file browser ****************/
/*****************************************************************************/

void Brw_RenFolderFileBrowser (void)
  {
   extern const char *Txt_The_folder_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   extern const char *Txt_The_folder_name_X_has_not_changed_because_there_is_already_a_folder_or_a_file_with_the_name_Y;
   extern const char *Txt_You_can_not_rename_this_folder;
   char OldPathInTree[PATH_MAX + 1 + NAME_MAX + 1];
   char NewPathInTree[PATH_MAX + 1 + NAME_MAX + 1];
   char OldPath[PATH_MAX + 1 + PATH_MAX + 1 + NAME_MAX + 1];
   char NewPath[PATH_MAX + 1 + PATH_MAX + 1 + NAME_MAX + 1];

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   if (Brw_CheckIfICanEditFileOrFolder (Gbl.FileBrowser.Level))	// Can I rename this folder?
     {
      if (Str_ConvertFilFolLnkNameToValid (Gbl.FileBrowser.NewFilFolLnkName))
        {
         if (strcmp (Gbl.FileBrowser.FilFolLnk.Name,Gbl.FileBrowser.NewFilFolLnkName))	// The name has changed
           {
            /* Gbl.FileBrowser.FilFolLnk.Name holds the new name of the folder */
            snprintf (OldPathInTree,sizeof (OldPathInTree),"%s/%s",
        	      Gbl.FileBrowser.FilFolLnk.Path,
		      Gbl.FileBrowser.FilFolLnk.Name);
            snprintf (OldPath,sizeof (OldPath),"%s/%s",
        	      Gbl.FileBrowser.Path.AboveRootFolder,OldPathInTree);

            /* Gbl.FileBrowser.NewFilFolLnkName holds the new name of the folder */
            if (strlen (Gbl.FileBrowser.Path.AboveRootFolder) + 1 +
                strlen (Gbl.FileBrowser.FilFolLnk.Path) + 1 +
                strlen (Gbl.FileBrowser.NewFilFolLnkName) > PATH_MAX)
	       Err_ShowErrorAndExit ("Path is too long.");
            snprintf (NewPathInTree,sizeof (NewPathInTree),"%s/%s",
        	      Gbl.FileBrowser.FilFolLnk.Path,
		      Gbl.FileBrowser.NewFilFolLnkName);
            snprintf (NewPath,sizeof (NewPath),"%s/%s",
        	      Gbl.FileBrowser.Path.AboveRootFolder,NewPathInTree);

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
	             Err_ShowErrorAndExit ("Write forbidden.");
	             break;
	          default:
	             Err_ShowErrorAndExit ("Can not rename folder.");
	             break;
	         }
	      }
            else				// Success
              {
	       /* If a folder is renamed,
                  it is necessary to rename all entries in the tables of files
                  that belong to the subtree starting at that folder */
               Brw_DB_RenameOneFolder (OldPathInTree,
        	                       NewPathInTree);
               Brw_DB_RenameChildrenFilesOrFolders (OldPathInTree,
        	                                    NewPathInTree);

               /* Remove affected clipboards */
               Brw_DB_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
        	                                Gbl.Usrs.Me.UsrDat.UsrCod,
        	                                Gbl.Usrs.Other.UsrDat.UsrCod);

               /* Remove affected expanded folders */
               Brw_DB_RenameAffectedExpandedFolders (Gbl.FileBrowser.Type,
        	                                     Gbl.Usrs.Me.UsrDat.UsrCod,
        	                                     Gbl.Usrs.Other.UsrDat.UsrCod,
        	                                     OldPathInTree,
        	                                     NewPathInTree);

               /* Write message of confirmation */
               Ale_ShowAlert (Ale_SUCCESS,Txt_The_folder_X_has_been_renamed_as_Y,
                              Gbl.FileBrowser.FilFolLnk.Name,
                              Gbl.FileBrowser.NewFilFolLnkName);
              }

           }
         else	// Names are equal.
	        // This may happens if we have press...
		// ...INTRO without changing the name
            Ale_ShowAlert (Ale_INFO,Txt_The_name_X_has_not_changed,
                           Gbl.FileBrowser.FilFolLnk.Name);
        }
      else	// Folder name not valid
         Ale_ShowAlerts (NULL);
     }
   else
      Err_ShowErrorAndExit (Txt_You_can_not_rename_this_folder);

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/****** Receive a new file in a file browser unsigned Dropzone.js ************/
/*****************************************************************************/

void Brw_RcvFileInFileBrwDropzone (void)
  {
   bool UploadSucessful;
   struct BrwSiz_BrowserSize *Size = BrwSiz_GetSize ();

   /***** Receive file *****/
   UploadSucessful = Brw_RcvFileInFileBrw (Size,Brw_DROPZONE_UPLOAD);

   /***** When a file is uploaded, the HTTP response
	  is a code status and a message for Dropzone.js *****/
   /* Don't write HTML at all */
   Gbl.Layout.HTMLStartWritten =
   Gbl.Layout.DivsEndWritten   =
   Gbl.Layout.HTMLEndWritten   = true;

   /* Begin HTTP response */
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
   struct BrwSiz_BrowserSize *Size = BrwSiz_GetSize ();

   /***** Receive file and show feedback message *****/
   Brw_RcvFileInFileBrw (Size,Brw_CLASSIC_UPLOAD);

   /***** Show possible alert *****/
   Ale_ShowAlerts (NULL);

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/****************** Receive a new file in a file browser *********************/
/*****************************************************************************/

static bool Brw_RcvFileInFileBrw (struct BrwSiz_BrowserSize *Size,
                                  Brw_UploadType_t UploadType)
  {
   extern const char *Txt_UPLOAD_FILE_X_file_already_exists_NO_HTML;
   extern const char *Txt_UPLOAD_FILE_could_not_create_file_NO_HTML;
   extern const char *Txt_UPLOAD_FILE_X_quota_exceeded_NO_HTML;
   extern const char *Txt_The_file_X_has_been_placed_inside_the_folder_Y;
   extern const char *Txt_UPLOAD_FILE_You_must_specify_the_file_NO_HTML;
   extern const char *Txt_UPLOAD_FILE_Forbidden_NO_HTML;
   struct Par_Param *Par;
   char SrcFileName[PATH_MAX + 1];
   char PathUntilFileName[PATH_MAX + 1];
   char Path[PATH_MAX + 1 + PATH_MAX + 1];
   char PathTmp[PATH_MAX + 1 + PATH_MAX + 4 + 1];
   char PathCompleteInTreeIncludingFile[PATH_MAX + 1 + NAME_MAX + 1];
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE + 1];
   bool FileIsValid = true;
   long FilCod = -1L;	// Code of new file in database
   struct Brw_FileMetadata FileMetadata;
   struct Mrk_Properties Marks;
   char FileNameToShow[NAME_MAX + 1];
   bool UploadSucessful = false;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Check if creating a new file is allowed *****/
   if (Brw_CheckIfICanCreateIntoFolder (Gbl.FileBrowser.Level))
     {
      /***** First, we save in disk the file received *****/
      Par = Fil_StartReceptionOfFile (Fil_NAME_OF_PARAM_FILENAME_ORG,
                                      SrcFileName,MIMEType);

      /***** Get filename from path *****/
      // Spaces at start or end are allowed
      Str_SplitFullPathIntoPathAndFileName (SrcFileName,
	                                    PathUntilFileName,
	                                    Gbl.FileBrowser.NewFilFolLnkName);
      if (Gbl.FileBrowser.NewFilFolLnkName[0])
        {
         /***** Check if uploading this kind of file is allowed *****/
	 if (Brw_CheckIfUploadIsAllowed (MIMEType))
           {
            if (Str_ConvertFilFolLnkNameToValid (Gbl.FileBrowser.NewFilFolLnkName))
              {
               /* Gbl.FileBrowser.NewFilFolLnkName holds the name of the new file */
               snprintf (Path,sizeof (Path),"%s/%s",
                         Gbl.FileBrowser.Path.AboveRootFolder,
                         Gbl.FileBrowser.FilFolLnk.Full);
               if (strlen (Path) + 1 +
        	   strlen (Gbl.FileBrowser.NewFilFolLnkName) +
		   strlen (".tmp") > PATH_MAX)
	          Err_ShowErrorAndExit ("Path is too long.");
               Str_Concat (Path,"/",sizeof (Path) - 1);
               Str_Concat (Path,Gbl.FileBrowser.NewFilFolLnkName,sizeof (Path) - 1);

               /* Check if the destination file exists */
               if (Fil_CheckIfPathExists (Path))
        	  Ale_CreateAlert (Ale_WARNING,NULL,
        		           Txt_UPLOAD_FILE_X_file_already_exists_NO_HTML,
                                   Gbl.FileBrowser.NewFilFolLnkName);
               else	// Destination file does not exist
                 {
                  /* End receiving the file */
                  snprintf (PathTmp,sizeof (PathTmp),"%s.tmp",Path);
                  FileIsValid = Fil_EndReceptionOfFile (PathTmp,Par);

                  /* Check if the content of the file of marks is valid */
                  if (FileIsValid)
                     if (Brw_TypeIsAdmMrk[Gbl.FileBrowser.Type])
                        if (!Mrk_CheckFileOfMarks (PathTmp,&Marks))
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
	                BrwSiz_CalcSizeOfDir (Size,Gbl.FileBrowser.Path.RootFolder);
	                BrwSiz_SetMaxQuota (Size);
                        if (BrwSiz_CheckIfQuotaExceded (Size))
	                  {
	                   Fil_RemoveTree (Path);
        	           Ale_CreateAlert (Ale_WARNING,NULL,
        	        	            Txt_UPLOAD_FILE_X_quota_exceeded_NO_HTML,
		                            Gbl.FileBrowser.NewFilFolLnkName);
	                  }
	                else
                          {
                           /* Remove affected clipboards */
                           Brw_DB_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
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
                           FilCod = Brw_DB_AddPath (Gbl.Usrs.Me.UsrDat.UsrCod,Brw_IS_FILE,
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
			   if (Brw_TypeIsAdmMrk[Gbl.FileBrowser.Type])
                              Mrk_DB_AddMarks (FileMetadata.FilCod,&Marks);

                           /* Notify new file */
			   if (!Brw_DB_CheckIfFileOrFolderIsSetAsHiddenUsingMetadata (&FileMetadata))
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
   char PathCompleteInTreeIncludingFile[PATH_MAX + 1 + NAME_MAX + 4 + 1];
   long FilCod = -1L;	// Code of new file in database
   char FileNameToShow[NAME_MAX + 1];
   struct Brw_FileMetadata FileMetadata;
   struct BrwSiz_BrowserSize *Size = BrwSiz_GetSize ();

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Check if creating a new link is allowed *****/
   if (Brw_CheckIfICanCreateIntoFolder (Gbl.FileBrowser.Level))
     {
      /***** Create a new file to store URL ****/
      Par_GetParText ("NewLinkURL",URL,PATH_MAX);
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
	              sizeof (URLWithoutEndingSlash) - 1);
	 else
	    /*
	    Gbl.FileBrowser.NewFilFolLnkName is empty
	    URL holds the URL given by me in the form
	    Example:
	    URL: http://www.intel.es/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-manual-325462.pdf
	    File in swad: 64-ia-32-architectures-software-developer-manual-325462.pdf.url
	    */
	    Str_Copy (URLWithoutEndingSlash,URL,
	              sizeof (URLWithoutEndingSlash) - 1);

	 /* Remove possible final '/' from URL */
	 if (URLWithoutEndingSlash[LengthURL - 1] == '/')
	    URLWithoutEndingSlash[LengthURL - 1] = '\0';

	 /* Get the last name in URL-without-ending-slash */
	 Str_SplitFullPathIntoPathAndFileName (URLWithoutEndingSlash,
					       URLUntilLastFilename,
					       FileName);

	 /* Convert the last name in URL to a valid filename */
	 if (Str_ConvertFilFolLnkNameToValid (FileName))
	   {
	    /* The name of the file with the link will be the FileName.url */
	    snprintf (Path,sizeof (Path),"%s/%s",
		      Gbl.FileBrowser.Path.AboveRootFolder,
		      Gbl.FileBrowser.FilFolLnk.Full);
	    if (strlen (Path) + 1 + strlen (FileName) + strlen (".url") > PATH_MAX)
	       Err_ShowErrorAndExit ("Path is too long.");
	    Str_Concat (Path,"/",sizeof (Path) - 1);
	    Str_Concat (Path,FileName,sizeof (Path) - 1);
	    Str_Concat (Path,".url",sizeof (Path) - 1);

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
		  BrwSiz_CalcSizeOfDir (Size,Gbl.FileBrowser.Path.RootFolder);
		  BrwSiz_SetMaxQuota (Size);
		  if (BrwSiz_CheckIfQuotaExceded (Size))
		    {
		     Fil_RemoveTree (Path);
		     Ale_ShowAlert (Ale_WARNING,Txt_Can_not_create_the_link_X_because_it_would_exceed_the_disk_quota,
			            FileName);
		    }
		  else
		    {
		     /* Remove affected clipboards */
		     Brw_DB_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
						      Gbl.Usrs.Me.UsrDat.UsrCod,
						      Gbl.Usrs.Other.UsrDat.UsrCod);

		     /* Add path where new file is created to table of expanded folders */
		     Brw_InsFoldersInPathAndUpdOtherFoldersInExpandedFolders (Gbl.FileBrowser.FilFolLnk.Full);

		     /* Add entry to the table of files/folders */
		     snprintf (PathCompleteInTreeIncludingFile,
			       sizeof (PathCompleteInTreeIncludingFile),
			       "%s/%s.url",
			       Gbl.FileBrowser.FilFolLnk.Full,FileName);
		     FilCod = Brw_DB_AddPath (Gbl.Usrs.Me.UsrDat.UsrCod,Brw_IS_LINK,
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
		     if (!Brw_DB_CheckIfFileOrFolderIsSetAsHiddenUsingMetadata (&FileMetadata))
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
      Err_ShowErrorAndExit (Txt_You_can_not_create_links_here);	// It's difficult, but not impossible that a user sees this message

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
                                                 Gbl.FileBrowser.FilFolLnk.Full) == HidVis_HIDDEN)
      Brw_DB_HideOrUnhideFileOrFolder (Gbl.FileBrowser.FilFolLnk.Full,HidVis_VISIBLE);

   /***** Remove the affected clipboards *****/
   Brw_DB_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
				    Gbl.Usrs.Me.UsrDat.UsrCod,
				    Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Show again the file browser *****/
   Brw_ShowFileBrowserNormal ();
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
   if (Brw_CheckIfFileOrFolderIsSetAsHiddenInDB (Gbl.FileBrowser.FilFolLnk.Type,
                                                 Gbl.FileBrowser.FilFolLnk.Full) == HidVis_VISIBLE)
      Brw_DB_HideOrUnhideFileOrFolder (Gbl.FileBrowser.FilFolLnk.Full,HidVis_HIDDEN);

   /***** Remove the affected clipboards *****/
   Brw_DB_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
				    Gbl.Usrs.Me.UsrDat.UsrCod,
				    Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Show again the file browser *****/
   Brw_ShowFileBrowserNormal ();
  }

/*****************************************************************************/
/** Check if a file / folder from the documents zone is set as hidden in DB **/
/*****************************************************************************/

HidVis_HiddenOrVisible_t Brw_CheckIfFileOrFolderIsSetAsHiddenInDB (Brw_FileType_t FileType,
                                                                   const char *Path)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   HidVis_HiddenOrVisible_t HiddenOrVisible;

   /***** Get if a file or folder is hidden from database *****/
   if (Brw_DB_CheckIfFileOrFolderIsSetAsHiddenUsingPath (&mysql_res,Path))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* File is hidden? (row[0]) */
      HiddenOrVisible = HidVid_GetHiddenOrVisible (row[0][0]);
     }
   else
     {
      Brw_DB_AddPath (-1L,FileType,
                       Gbl.FileBrowser.FilFolLnk.Full,false,Brw_LICENSE_DEFAULT);
      HiddenOrVisible = HidVis_VISIBLE;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return HiddenOrVisible;
  }

/*****************************************************************************/
/***************** Show metadata of a file in a file browser *****************/
/*****************************************************************************/

void Brw_ShowFileMetadata (void)
  {
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
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC15x20",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE15x20",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO15x20",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR15x20",
     };
   struct Brw_FileMetadata FileMetadata;
   struct Usr_Data PublisherUsrDat;
   char FileNameToShow[NAME_MAX + 1];
   char URL[PATH_MAX + 1];
   char FileSizeStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];
   bool Found;
   bool ICanView = false;
   bool IAmTheOwner;
   bool ICanEdit;
   bool ICanChangePublic = false;
   bool FileHasPublisher;
   Brw_License_t License;
   unsigned LicenseUnsigned;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get file metadata *****/
   FileMetadata.FilCod = ParCod_GetAndCheckPar (ParCod_Fil);
   Brw_GetFileMetadataByCod (&FileMetadata);
   Found = Brw_GetFileTypeSizeAndDate (&FileMetadata);

   if (Found)
     {
      if (FileMetadata.FilCod <= 0)	// No entry for this file in database table of files
	 /* Add entry to the table of files/folders */
	 FileMetadata.FilCod = Brw_DB_AddPath (-1L,FileMetadata.FilFolLnk.Type,
	                                        FileMetadata.FilFolLnk.Full,
	                                        false,Brw_LICENSE_DEFAULT);

      /***** Check if I can view this file.
	     It could be marked as hidden or in a hidden folder *****/
      ICanView = true;
      switch (Gbl.FileBrowser.Type)
	{
	 case Brw_SHOW_DOC_INS:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_INS_ADM)
	       ICanView = !Brw_DB_CheckIfFileOrFolderIsSetAsHiddenUsingMetadata (&FileMetadata);
            break;
	 case Brw_SHOW_DOC_CTR:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_CTR_ADM)
	       ICanView = !Brw_DB_CheckIfFileOrFolderIsSetAsHiddenUsingMetadata (&FileMetadata);
            break;
	 case Brw_SHOW_DOC_DEG:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_DEG_ADM)
	       ICanView = !Brw_DB_CheckIfFileOrFolderIsSetAsHiddenUsingMetadata (&FileMetadata);
            break;
	 case Brw_SHOW_DOC_CRS:
	 case Brw_SHOW_DOC_GRP:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_TCH)
               ICanView = !Brw_DB_CheckIfFileOrFolderIsSetAsHiddenUsingMetadata (&FileMetadata);
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
	    FileHasPublisher = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&PublisherUsrDat,
	                                                                Usr_DONT_GET_PREFS,
	                                                                Usr_DONT_GET_ROLE_IN_CRS);
	   }
	 else
	    FileHasPublisher = false;	// Get user's data from database

	 /***** Get link to download the file *****/
	 if (Brw_TypeIsSeeMrk[Gbl.FileBrowser.Type])
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

         /***** Begin box *****/
	 // Put icon to get link?
	 if (Brw_ActReqLnk[Gbl.FileBrowser.Type] != ActUnk &&
	     (FileMetadata.FilFolLnk.Type == Brw_IS_FILE ||	// Only files or links
	      FileMetadata.FilFolLnk.Type == Brw_IS_LINK) &&
	     Rsc_CheckIfICanGetLink ())
	    Box_BoxShadowBegin (NULL,Brw_PutIconToGetLinkToFile,&FileMetadata,
				NULL);
	 else
	    Box_BoxShadowBegin (NULL,NULL,NULL,
			        NULL);


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

	    Frm_BeginForm (Brw_ActRecDatFile[Gbl.FileBrowser.Type]);
	       Brw_PutParsFileBrowser (NULL,		// Not used
				       NULL,		// Not used
				       Brw_IS_UNKNOWN,	// Not used
				       FileMetadata.FilCod);
	   }

         /***** Begin table *****/
         HTM_TABLE_BeginWidePadding (2);

	    /***** Link to download the file *****/
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("colspan=\"2\" class=\"CM\"");
		  Brw_WriteBigLinkToDownloadFile (URL,&FileMetadata,FileNameToShow);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** Filename *****/
	    HTM_TR_Begin (NULL);

	       Frm_LabelColumn ("RT",NULL,Txt_Filename);

	       HTM_TD_Begin ("class=\"LB DAT_STRONG_%s\"",
			     The_GetSuffix ());
		  Brw_WriteSmallLinkToDownloadFile (URL,&FileMetadata,FileNameToShow);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** Publisher's data *****/
	    HTM_TR_Begin (NULL);

	       Frm_LabelColumn ("RT",NULL,Txt_Uploaded_by);

	       HTM_TD_Begin ("class=\"LB DAT_STRONG_%s\"",
			     The_GetSuffix ());
		  if (FileHasPublisher)
		    {
		     /* Show photo */
		     Pho_ShowUsrPhotoIfAllowed (&PublisherUsrDat,
						ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);

		     /* Write name */
		     HTM_NBSP ();
		     HTM_Txt (PublisherUsrDat.FullName);
		    }
		  else
		     /* Unknown publisher */
		     HTM_Txt (Txt_ROLES_SINGUL_Abc[Rol_UNK][Usr_SEX_UNKNOWN]);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** Free memory used for publisher's data *****/
	    if (FileMetadata.PublisherUsrCod > 0)
	       Usr_UsrDataDestructor (&PublisherUsrDat);

	    /***** Write the file size *****/
	    Fil_WriteFileSizeFull ((double) FileMetadata.Size,FileSizeStr);
	    HTM_TR_Begin (NULL);

	       Frm_LabelColumn ("RT",NULL,Txt_File_size);

	       HTM_TD_Begin ("class=\"LB DAT_STRONG_%s\"",
			     The_GetSuffix ());
		  HTM_Txt (FileSizeStr);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** Write the date *****/
	    HTM_TR_Begin (NULL);

	       Frm_LabelColumn ("RT",NULL,Txt_Date_of_creation);

	       HTM_TD_Begin ("id=\"filedate\" class=\"LB DAT_STRONG_%s\"",
			     The_GetSuffix ());
		  Dat_WriteLocalDateHMSFromUTC ("filedate",FileMetadata.Time,
						Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
						true,true,true,0x7);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** Private or public? *****/
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("RT",ICanChangePublic ? "PublicFile" :
							NULL,
				Txt_Availability);

	       /* Data */
	       HTM_TD_Begin ("class=\"LT DAT_STRONG_%s\"",
			     The_GetSuffix ());
		  if (ICanChangePublic)	// I can change file to public
		    {
		     HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,NULL,
				       "id=\"PublicFile\" name=\"PublicFile\" class=\"PUBLIC_FILE\"");
			HTM_OPTION (HTM_Type_STRING,"N",
				    FileMetadata.IsPublic ? HTM_OPTION_UNSELECTED :
							    HTM_OPTION_SELECTED,
				    HTM_OPTION_ENABLED,
				    "%s",Txt_Private_available_to_certain_users_identified);
			HTM_OPTION (HTM_Type_STRING,"Y",
				    FileMetadata.IsPublic ? HTM_OPTION_SELECTED :
							    HTM_OPTION_UNSELECTED,
				    HTM_OPTION_ENABLED,
				    "%s",Txt_Public_open_educational_resource_OER_for_everyone);
		     HTM_SELECT_End ();
		    }
		  else		// I can not edit file properties
		     HTM_Txt (FileMetadata.IsPublic ? Txt_Public_open_educational_resource_OER_for_everyone :
						      Txt_Private_available_to_certain_users_identified);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** License *****/
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("RT",ICanEdit ? "License" :
						NULL,
				Txt_License);

	       /* Data */
	       HTM_TD_Begin ("class=\"LT DAT_STRONG_%s\"",
			     The_GetSuffix ());
		  if (ICanEdit)	// I can edit file properties
		    {
		     HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,NULL,
				       "id=\"License\" name=\"License\" class=\"LICENSE\"");
			for (License  = (Brw_License_t) 0;
			     License <= (Brw_License_t) (Brw_NUM_LICENSES - 1);
			     License++)
			  {
			   LicenseUnsigned = (unsigned) License;
			   HTM_OPTION (HTM_Type_UNSIGNED,&LicenseUnsigned,
				       License == FileMetadata.License ? HTM_OPTION_SELECTED :
									 HTM_OPTION_UNSELECTED,
				       HTM_OPTION_ENABLED,
				       "%s",Txt_LICENSES[License]);
			  }
		     HTM_SELECT_End ();
		    }
		  else		// I can not edit file properties
		     HTM_Txt (Txt_LICENSES[FileMetadata.License]);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** Write my number of views *****/
	    if (Gbl.Usrs.Me.Logged)
	      {
	       HTM_TR_Begin (NULL);

		  Frm_LabelColumn ("RT",NULL,Txt_My_views);

		  HTM_TD_Begin ("class=\"LB DAT_STRONG_%s\"",
				The_GetSuffix ());
		     HTM_Unsigned (FileMetadata.NumMyViews);
		  HTM_TD_End ();

	       HTM_TR_End ();
	      }

	    /***** Write number of identificated views *****/
	    HTM_TR_Begin (NULL);

	       Frm_LabelColumn ("RT",NULL,Txt_Identified_views);

	       HTM_TD_Begin ("class=\"LB DAT_STRONG_%s\"",
			     The_GetSuffix ());
		  HTM_TxtF ("%u&nbsp;",FileMetadata.NumViewsFromLoggedUsrs);
		  HTM_TxtF ("(%u %s)",
			    FileMetadata.NumLoggedUsrs,
			    FileMetadata.NumLoggedUsrs == 1 ? Txt_user[Usr_SEX_UNKNOWN] :
							      Txt_users[Usr_SEX_UNKNOWN]);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** Write number of public views *****/
	    HTM_TR_Begin (NULL);

	       Frm_LabelColumn ("RT",NULL,Txt_Public_views);

	       HTM_TD_Begin ("class=\"LB DAT_STRONG_%s\"",
			     The_GetSuffix ());
		  HTM_Unsigned (FileMetadata.NumPublicViews);
	       HTM_TD_End ();

	    HTM_TR_End ();

	 /***** End table *****/
         HTM_TABLE_End ();

	 /***** End form *****/
	 if (ICanEdit)	// I can edit file properties
	   {
	       Btn_PutButton (Btn_CONFIRM_BUTTON,Txt_Save_file_properties);
	    Frm_EndForm ();
	   }

	 /***** End box *****/
	 Box_BoxEnd ();

	 /***** Mark possible notifications as seen *****/
	 switch (Gbl.FileBrowser.Type)
	   {
	    case Brw_SHOW_DOC_CRS:
	    case Brw_SHOW_DOC_GRP:
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_DOC_GRP:
	       Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_DOCUMENT_FILE,
	                                       FileMetadata.FilCod);
	       break;
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_TCH_GRP:
	       Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_TEACHERS_FILE,
	                                       FileMetadata.FilCod);
	       break;
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_SHR_GRP:
	       Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_SHARED_FILE,
	                                       FileMetadata.FilCod);
	       break;
	    case Brw_SHOW_MRK_CRS:
	    case Brw_SHOW_MRK_GRP:
	    case Brw_ADMI_MRK_CRS:
	    case Brw_ADMI_MRK_GRP:
	       Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_MARKS_FILE,
	                                       FileMetadata.FilCod);
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
	    Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_DOCUMENT_FILE,
				       FileMetadata.FilCod);
	    break;
	 case Brw_ADMI_TCH_CRS:
	 case Brw_ADMI_TCH_GRP:
	    Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_TEACHERS_FILE,
				       FileMetadata.FilCod);
	    break;
	 case Brw_ADMI_SHR_CRS:
	 case Brw_ADMI_SHR_GRP:
	    Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_SHARED_FILE,
				       FileMetadata.FilCod);
	    break;
	 case Brw_SHOW_MRK_CRS:
	 case Brw_SHOW_MRK_GRP:
	 case Brw_ADMI_MRK_CRS:
	 case Brw_ADMI_MRK_GRP:
	    Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_MARKS_FILE,
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
/*********************** Put icon to get link to file ************************/
/*****************************************************************************/

static void Brw_PutIconToGetLinkToFile (void *FileMetadata)
  {
   Ico_PutContextualIconToGetLink (Brw_ActReqLnk[Gbl.FileBrowser.Type],NULL,
                                   Brw_PutParsToGetLinkToFile,FileMetadata);
  }

static void Brw_PutParsToGetLinkToFile (void *FileMetadata)
  {
   if (FileMetadata)
      Brw_PutParsFileBrowser (NULL,		// Not used
			      NULL,		// Not used
			      Brw_IS_UNKNOWN,	// Not used
			      ((struct Brw_FileMetadata *) FileMetadata)->FilCod);
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
   snprintf (FullPathIncludingFile,sizeof (FullPathIncludingFile),"%s/%s/%s",
	     Gbl.FileBrowser.Path.AboveRootFolder,PathInTree,FileName);

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
      snprintf (URLWithSpaces,sizeof (URLWithSpaces),"%s/%s/%s/%s",
	        Cfg_URL_FILE_BROWSER_TMP_PUBLIC,
	        Gbl.FileBrowser.TmpPubDir.L,
	        Gbl.FileBrowser.TmpPubDir.R,
	        FileName);
     }

   Str_CopyStrChangingSpaces (URLWithSpaces,URL,PATH_MAX);	// In HTML, URL must have no spaces
  }

/*****************************************************************************/
/******************* Download a file from a file browser *********************/
/*****************************************************************************/

void Brw_DownloadFile (void)
  {
   extern const char *Txt_The_file_of_folder_no_longer_exists_or_is_now_hidden;
   struct Brw_FileMetadata FileMetadata;
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
	 FileMetadata.FilCod = Brw_DB_AddPath (-1L,FileMetadata.FilFolLnk.Type,
	                                        Gbl.FileBrowser.FilFolLnk.Full,
	                                        false,Brw_LICENSE_DEFAULT);

      /***** Check if I can view this file.
	     It could be marked as hidden or in a hidden folder *****/
      ICanView = true;
      switch (Gbl.FileBrowser.Type)
	{
	 case Brw_SHOW_DOC_INS:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_INS_ADM)
	       ICanView = !Brw_DB_CheckIfFileOrFolderIsSetAsHiddenUsingMetadata (&FileMetadata);
            break;
	 case Brw_SHOW_DOC_CTR:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_CTR_ADM)
	       ICanView = !Brw_DB_CheckIfFileOrFolderIsSetAsHiddenUsingMetadata (&FileMetadata);
            break;
	 case Brw_SHOW_DOC_DEG:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_DEG_ADM)
	       ICanView = !Brw_DB_CheckIfFileOrFolderIsSetAsHiddenUsingMetadata (&FileMetadata);
            break;
	 case Brw_SHOW_DOC_CRS:
	 case Brw_SHOW_DOC_GRP:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_TCH)
               ICanView = !Brw_DB_CheckIfFileOrFolderIsSetAsHiddenUsingMetadata (&FileMetadata);
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
	 if (Brw_TypeIsSeeMrk[Gbl.FileBrowser.Type])
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
	       Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_DOCUMENT_FILE,
	                                       FileMetadata.FilCod);
	       break;
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_TCH_GRP:
	       Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_TEACHERS_FILE,
	                                       FileMetadata.FilCod);
	       break;
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_SHR_GRP:
	       Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_SHARED_FILE,
	                                       FileMetadata.FilCod);
	       break;
	    case Brw_SHOW_MRK_CRS:
	    case Brw_SHOW_MRK_GRP:
	    case Brw_ADMI_MRK_CRS:
	    case Brw_ADMI_MRK_GRP:
	       Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_MARKS_FILE,
	                                       FileMetadata.FilCod);
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
	    Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_DOCUMENT_FILE,
				       FileMetadata.FilCod);
	    break;
	 case Brw_ADMI_TCH_CRS:
	 case Brw_ADMI_TCH_GRP:
	    Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_TEACHERS_FILE,
				       FileMetadata.FilCod);
	    break;
	 case Brw_ADMI_SHR_CRS:
	 case Brw_ADMI_SHR_GRP:
	    Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_SHARED_FILE,
				       FileMetadata.FilCod);
	    break;
	 case Brw_SHOW_MRK_CRS:
	 case Brw_SHOW_MRK_GRP:
	 case Brw_ADMI_MRK_CRS:
	 case Brw_ADMI_MRK_GRP:
	    Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_MARKS_FILE,
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
	 ZoneUsrCod = Brw_GetZoneUsrCodForFileBrowser ();
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
                                            struct Brw_FileMetadata *FileMetadata,
                                            const char *FileNameToShow)
  {
   extern const char *Txt_Check_marks_in_the_file;
   extern const char *Txt_Download;
   extern const char *Txt_Link;
   const char *Title;

   /***** On the screen a link will be shown to download the file *****/
   if (Brw_TypeIsSeeMrk[Gbl.FileBrowser.Type])
     {
      /* Form to see marks */
      Frm_BeginForm (Gbl.FileBrowser.Type == Brw_SHOW_MRK_CRS ? ActSeeMyMrkCrs :
								ActSeeMyMrkGrp);

	 Str_Copy (Gbl.FileBrowser.FilFolLnk.Path,FileMetadata->FilFolLnk.Path,
		   sizeof (Gbl.FileBrowser.FilFolLnk.Path) - 1);
	 Str_Copy (Gbl.FileBrowser.FilFolLnk.Name,FileMetadata->FilFolLnk.Name,
		   sizeof (Gbl.FileBrowser.FilFolLnk.Name) - 1);
	 Gbl.FileBrowser.FilFolLnk.Type = FileMetadata->FilFolLnk.Type;
	 Brw_PutImplicitParsFileBrowser (&Gbl.FileBrowser.FilFolLnk);

	 /* Begin link */
	 HTM_BUTTON_Submit_Begin (Txt_Check_marks_in_the_file,
	                          "class=\"BT_LINK ICO_HIGHLIGHT FILENAME_BIG_%s\"",
	                          The_GetSuffix ());

	    if (FileMetadata->FilFolLnk.Type == Brw_IS_FILE)
	       Brw_PutIconFile (FileMetadata->FilFolLnk.Name,
				"ICO40x40",
				Frm_DONT_PUT_FORM);	// Don't put link to view metadata
	    else
	       Ico_PutIcon ("up-right-from-square.svg",Ico_BLACK,Txt_Link,"ICO40x40");

	    /* Name of the file of marks, link end and form end */
	    HTM_TxtF ("&nbsp;%s&nbsp;",FileNameToShow);
	    Ico_PutIcon ("list-alt.svg",Ico_UNCHANGED,Txt_Check_marks_in_the_file,"ICO40x40");

	 /* End link */
	 HTM_BUTTON_End ();

      Frm_EndForm ();
     }
   else
     {
      Title = (FileMetadata->FilFolLnk.Type == Brw_IS_LINK) ? URL :	// If it's a link, show full URL in title
                                                              Txt_Download;

      /* Put anchor and filename */
      HTM_A_Begin ("href=\"%s\" title=\"%s\" target=\"_blank\""
	           " class=\"FILENAME_BIG_%s\"",
	           URL,Title,The_GetSuffix ());
	 if (FileMetadata->FilFolLnk.Type == Brw_IS_FILE)
	    Brw_PutIconFile (FileMetadata->FilFolLnk.Name,
			     "ICO40x40",
			     Frm_DONT_PUT_FORM);	// Don't put link to view metadata
	 else
	    Ico_PutIcon ("up-right-from-square.svg",Ico_BLACK,Txt_Link,"ICO40x40");
	 HTM_TxtF ("&nbsp;%s&nbsp;",FileNameToShow);
	 Ico_PutIcon ("download.svg",Ico_BLACK,Title,"ICO40x40");
      HTM_A_End ();
     }
  }

/*****************************************************************************/
/*********************** Write link to download a file ***********************/
/*****************************************************************************/

static void Brw_WriteSmallLinkToDownloadFile (const char *URL,
	                                      struct Brw_FileMetadata *FileMetadata,
                                              const char *FileNameToShow)
  {
   extern const char *Txt_Check_marks_in_the_file;

   /***** On the screen a link will be shown to download the file *****/
   if (Brw_TypeIsSeeMrk[Gbl.FileBrowser.Type])
     {
      /* Form to see marks */
      Frm_BeginForm (Gbl.FileBrowser.Type == Brw_SHOW_MRK_CRS ? ActSeeMyMrkCrs :
								ActSeeMyMrkGrp);
	 Str_Copy (Gbl.FileBrowser.FilFolLnk.Path,FileMetadata->FilFolLnk.Path,
		   sizeof (Gbl.FileBrowser.FilFolLnk.Path) - 1);
	 Str_Copy (Gbl.FileBrowser.FilFolLnk.Name,FileMetadata->FilFolLnk.Name,
		   sizeof (Gbl.FileBrowser.FilFolLnk.Name) - 1);
	 Gbl.FileBrowser.FilFolLnk.Type = FileMetadata->FilFolLnk.Type;
	 Brw_PutImplicitParsFileBrowser (&Gbl.FileBrowser.FilFolLnk);

	 /* Begin link */
	 HTM_BUTTON_Submit_Begin (Txt_Check_marks_in_the_file,
	                          "class=\"BT_LINK\"");

	    /* Name of the file of marks */
	    HTM_Txt (FileNameToShow);

	 /* End link */
	 HTM_BUTTON_End ();

      /* End form */
      Frm_EndForm ();
     }
   else
     {
      /* Put anchor and filename */
      HTM_A_Begin ("href=\"%s\" class=\"DAT_STRONG_%s\""
	           " title=\"%s\" target=\"_blank\"",
	           URL,The_GetSuffix (),FileNameToShow);
	 HTM_Txt (FileNameToShow);
      HTM_A_End ();
     }
  }

/*****************************************************************************/
/**************** Change metadata of a file in a file browser ****************/
/*****************************************************************************/

void Brw_ChgFileMetadata (void)
  {
   extern const char *Txt_The_properties_of_file_X_have_been_saved;
   struct Brw_FileMetadata FileMetadata;
   bool Found;
   bool IAmTheOwner;
   bool PublicFileBeforeEdition;
   bool PublicFileAfterEdition;
   Brw_License_t License;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get file metadata *****/
   FileMetadata.FilCod = ParCod_GetAndCheckPar (ParCod_Fil);
   Brw_GetFileMetadataByCod (&FileMetadata);
   Found = Brw_GetFileTypeSizeAndDate (&FileMetadata);

   if (Found)
     {
      /***** Check if I can change file metadata *****/
      IAmTheOwner = Brw_CheckIfIAmOwnerOfFile (FileMetadata.PublisherUsrCod);
      if (!Brw_CheckIfICanEditFileMetadata (IAmTheOwner))
	 Err_NoPermissionExit ();

      /***** Check if the file was public before the edition *****/
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
	    PublicFileAfterEdition = Brw_GetParPublicFile ();
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
      Brw_DB_ChangeFilePublic (&FileMetadata,PublicFileAfterEdition,License);

      /***** Remove the affected clipboards *****/
      Brw_DB_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
				       Gbl.Usrs.Me.UsrDat.UsrCod,
				       Gbl.Usrs.Other.UsrDat.UsrCod);

      /***** Insert file into public social activity *****/
      if (!PublicFileBeforeEdition &&
	   PublicFileAfterEdition)	// Only if file has changed from private to public
	 switch (Gbl.FileBrowser.Type)
	   {
	    case Brw_ADMI_DOC_INS:
	       TmlNot_StoreAndPublishNote (TmlNot_INS_DOC_PUB_FILE,FileMetadata.FilCod);
	       break;
	    case Brw_ADMI_SHR_INS:
	       TmlNot_StoreAndPublishNote (TmlNot_INS_SHA_PUB_FILE,FileMetadata.FilCod);
	       break;
	    case Brw_ADMI_DOC_CTR:
	       TmlNot_StoreAndPublishNote (TmlNot_CTR_DOC_PUB_FILE,FileMetadata.FilCod);
	       break;
	    case Brw_ADMI_SHR_CTR:
	       TmlNot_StoreAndPublishNote (TmlNot_CTR_SHA_PUB_FILE,FileMetadata.FilCod);
	       break;
	    case Brw_ADMI_DOC_DEG:
	       TmlNot_StoreAndPublishNote (TmlNot_DEG_DOC_PUB_FILE,FileMetadata.FilCod);
	       break;
	    case Brw_ADMI_SHR_DEG:
	       TmlNot_StoreAndPublishNote (TmlNot_DEG_SHA_PUB_FILE,FileMetadata.FilCod);
	       break;
	    case Brw_ADMI_DOC_CRS:
	       TmlNot_StoreAndPublishNote (TmlNot_CRS_DOC_PUB_FILE,FileMetadata.FilCod);
	       break;
	    case Brw_ADMI_SHR_CRS:
	       TmlNot_StoreAndPublishNote (TmlNot_CRS_SHA_PUB_FILE,FileMetadata.FilCod);
	       break;
	    default:
	       break;
	   }

      /***** Write sucess message *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_The_properties_of_file_X_have_been_saved,
		     FileMetadata.FilFolLnk.Name);
     }

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/*********** Get parameter with public / private file from form *************/
/*****************************************************************************/

static bool Brw_GetParPublicFile (void)
  {
   return Par_GetParBool ("PublicFile");
  }

/*****************************************************************************/
/******************** Get parameter with file license ***********************/
/*****************************************************************************/

static Brw_License_t Brw_GetParLicense (void)
  {
   return (Brw_License_t)
	  Par_GetParUnsignedLong ("License",
                                  0,
                                  Brw_NUM_LICENSES - 1,
                                  (unsigned long) Brw_LICENSE_UNKNOWN);
  }

/*****************************************************************************/
/********************* Get file metadata using its path **********************/
/*****************************************************************************/
// This function only gets metadata stored in table files,
// does not get size, time, numviews...

void Brw_GetFileMetadataByPath (struct Brw_FileMetadata *FileMetadata)
  {
   MYSQL_RES *mysql_res;

   /***** Get metadata of a file from database *****/
   if (Brw_DB_GetFileMetadataByPath (&mysql_res,Gbl.FileBrowser.FilFolLnk.Full))
     {
      Brw_ResetFileMetadata (FileMetadata);
      Brw_GetFileMetadataFromRow (mysql_res,FileMetadata);
     }
   else
      Brw_ResetFileMetadata (FileMetadata);
  }

/*****************************************************************************/
/********************* Get file metadata using its code **********************/
/*****************************************************************************/
// FileMetadata.FilCod must be filled
// This function only gets metadata stored in table files,
// does not get size, time, numviews...

void Brw_GetFileMetadataByCod (struct Brw_FileMetadata *FileMetadata)
  {
   MYSQL_RES *mysql_res;

   /***** Get metadata of a file from database *****/
   if (Brw_DB_GetFileMetadataByCod (&mysql_res,FileMetadata->FilCod))
     {
      Brw_ResetFileMetadata (FileMetadata);
      Brw_GetFileMetadataFromRow (mysql_res,FileMetadata);
     }
   else
      Brw_ResetFileMetadata (FileMetadata);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************************** Reset file metadata *****************************/
/*****************************************************************************/

static void Brw_ResetFileMetadata (struct Brw_FileMetadata *FileMetadata)
  {
   FileMetadata->FilCod            = -1L;
   FileMetadata->FileBrowser       = Brw_UNKNOWN;
   FileMetadata->Cod               = -1L;
   FileMetadata->ZoneUsrCod        = -1L;
   FileMetadata->PublisherUsrCod   = -1L;
   FileMetadata->FilFolLnk.Type    = Brw_IS_UNKNOWN;
   FileMetadata->FilFolLnk.Full[0] = '\0';
   FileMetadata->FilFolLnk.Path[0] = '\0';
   FileMetadata->FilFolLnk.Name[0] = '\0';
   FileMetadata->IsHidden          = false;
   FileMetadata->IsPublic          = false;
   FileMetadata->License           = Brw_LICENSE_DEFAULT;
   FileMetadata->Size              = (off_t) 0;
   FileMetadata->Time              = (time_t) 0;
   FileMetadata->NumMyViews             =
   FileMetadata->NumPublicViews         =
   FileMetadata->NumViewsFromLoggedUsrs =
   FileMetadata->NumLoggedUsrs          = 0;
  }

/*****************************************************************************/
/********************* Get file metadata using its code **********************/
/*****************************************************************************/

static void Brw_GetFileMetadataFromRow (MYSQL_RES *mysql_res,
                                        struct Brw_FileMetadata *FileMetadata)
  {
   MYSQL_ROW row;
   unsigned UnsignedNum;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get file code (row[0]) *****/
   FileMetadata->FilCod = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get file browser type in database (row[1]) *****/
   FileMetadata->FileBrowser = Brw_UNKNOWN;
   if (sscanf (row[1],"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Brw_NUM_TYPES_FILE_BROWSER)
	 FileMetadata->FileBrowser = (Brw_FileBrowser_t) UnsignedNum;

   /***** Get institution/center/degree/course/group code (row[2]) *****/
   FileMetadata->Cod = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get the user's code of the owner of a zone of files (row[3]) *****/
   FileMetadata->ZoneUsrCod = Str_ConvertStrCodToLongCod (row[3]);

   /***** Get publisher's code (row[4]) *****/
   FileMetadata->PublisherUsrCod = Str_ConvertStrCodToLongCod (row[4]);

   /***** Get file type (row[5]) *****/
   FileMetadata->FilFolLnk.Type = Brw_IS_UNKNOWN;	// default
   if (sscanf (row[5],"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Brw_NUM_FILE_TYPES)
	 FileMetadata->FilFolLnk.Type = (Brw_FileType_t) UnsignedNum;

   /***** Get path (row[6]) *****/
   Str_Copy (FileMetadata->FilFolLnk.Full,row[6],
	     sizeof (FileMetadata->FilFolLnk.Full) - 1);
   Str_SplitFullPathIntoPathAndFileName (FileMetadata->FilFolLnk.Full,
					 FileMetadata->FilFolLnk.Path,
					 FileMetadata->FilFolLnk.Name);

   /***** File is hidden? (row[7]) *****/
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

   /***** Is a public file? (row[8]) *****/
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

   /***** Get license (row[9]) *****/
   FileMetadata->License = Brw_LICENSE_UNKNOWN;
   if (sscanf (row[9],"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Brw_NUM_LICENSES)
	 FileMetadata->License = (Brw_License_t) UnsignedNum;
  }

/*****************************************************************************/
/********************** Get file type, size and date *************************/
/*****************************************************************************/
// Return true if file exists

bool Brw_GetFileTypeSizeAndDate (struct Brw_FileMetadata *FileMetadata)
  {
   char Path[PATH_MAX + 1 + PATH_MAX + 1];
   struct stat FileStatus;

   snprintf (Path,sizeof (Path),"%s/%s",
	     Gbl.FileBrowser.Path.AboveRootFolder,
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
void Brw_GetAndUpdateFileViews (struct Brw_FileMetadata *FileMetadata)
  {
   if (FileMetadata->FilCod > 0)
     {
      /***** Get file views from logged users *****/
      Brw_GetFileViewsFromLoggedUsrs (FileMetadata);

      /***** Get file views from non logged users *****/
      FileMetadata->NumPublicViews = Brw_DB_GetFileViewsFromNonLoggedUsrs (FileMetadata->FilCod);

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

      /***** Update number of my views *****/
      Brw_DB_UpdateFileViews (FileMetadata->NumMyViews,FileMetadata->FilCod);

      /***** Increment number of file views in my user's figures *****/
      if (Gbl.Usrs.Me.Logged)
         Prf_DB_IncrementNumFileViewsUsr (Gbl.Usrs.Me.UsrDat.UsrCod);
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
   Brw_DB_UpdateFileViews (Brw_GetFileViewsFromMe (FilCod),FilCod);
  }

/*****************************************************************************/
/******************** Get file views from logged users ***********************/
/*****************************************************************************/
/*
   Input:  FileMetadata->FilCod
   Output: FileMetadata->NumViewsFromLoggedUsrs
           FileMetadata->NumLoggedUsrs
*/
static void Brw_GetFileViewsFromLoggedUsrs (struct Brw_FileMetadata *FileMetadata)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get number total of views from logged users *****/
   if (Brw_DB_GetFileViewsFromLoggedUsrs (&mysql_res,FileMetadata->FilCod))
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
/************************** Get file views from me ***************************/
/*****************************************************************************/

static unsigned Brw_GetFileViewsFromMe (long FilCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumMyViews;

   /***** Get number of my views *****/
   if (Brw_DB_GetFileViewsFromMe (&mysql_res,FilCod))
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
/******** Get code of user in assignment / works for expanded folders ********/
/*****************************************************************************/

void Brw_GetCrsGrpFromFileMetadata (Brw_FileBrowser_t FileBrowser,long Cod,
                                    long *InsCod,
                                    long *CtrCod,
                                    long *DegCod,
                                    long *CrsCod,
                                    long *GrpCod)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   struct Hie_Node Ctr;
   struct Hie_Node Deg;
   struct Hie_Node Crs;
   struct GroupData GrpDat;

   switch (FileBrowser)
     {
      case Brw_ADMI_DOC_INS:
      case Brw_ADMI_SHR_INS:
	 /* Cod stores the institution code */
	 *GrpCod =
	 *CrsCod =
	 *DegCod =
	 *CtrCod = -1L;
	 *InsCod = Cod;
         break;
      case Brw_ADMI_DOC_CTR:
      case Brw_ADMI_SHR_CTR:
	 /* Cod stores the center code */
	 *GrpCod =
	 *CrsCod =
	 *DegCod = -1L;
	 *CtrCod = Ctr.HieCod = Cod;
	 Hie_GetDataByCod[Hie_CTR] (&Ctr);
	 *InsCod = Ctr.PrtCod;
         break;
      case Brw_ADMI_DOC_DEG:
      case Brw_ADMI_SHR_DEG:
	 /* Cod stores the degree code */
	 *GrpCod =
	 *CrsCod = -1L;
	 *DegCod = Deg.HieCod = Cod;
	 Hie_GetDataByCod[Hie_DEG] (&Deg);
	 *CtrCod = Ctr.HieCod = Deg.PrtCod;
	 Hie_GetDataByCod[Hie_CTR] (&Ctr);
	 *InsCod = Ctr.PrtCod;
         break;
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_TCH_CRS:
      case Brw_ADMI_SHR_CRS:
      case Brw_ADMI_ASG_USR:
      case Brw_ADMI_WRK_USR:
      case Brw_ADMI_MRK_CRS:
	 /* Cod stores the course code */
	 *GrpCod = -1L;
	 *CrsCod = Crs.HieCod = Cod;
	 Hie_GetDataByCod[Hie_CRS] (&Crs);
	 *DegCod = Deg.HieCod = Crs.PrtCod;
	 Hie_GetDataByCod[Hie_DEG] (&Deg);
	 *CtrCod = Ctr.HieCod = Deg.PrtCod;
	 Hie_GetDataByCod[Hie_CTR] (&Ctr);
	 *InsCod = Ctr.PrtCod;
	 break;
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_GRP:
      case Brw_ADMI_SHR_GRP:
      case Brw_ADMI_MRK_GRP:
	 /* Cod stores the group code */
	 *GrpCod = GrpDat.GrpCod = Cod;
	 Grp_GetGroupDataByCod (&GrpDat);
	 *CrsCod = Crs.HieCod = GrpDat.CrsCod;
	 Hie_GetDataByCod[Hie_CRS] (&Crs);
	 *DegCod = Deg.HieCod = Crs.PrtCod;
	 Hie_GetDataByCod[Hie_DEG] (&Deg);
	 *CtrCod = Ctr.HieCod = Deg.PrtCod;
	 Hie_GetDataByCod[Hie_CTR] (&Ctr);
	 *InsCod = Ctr.PrtCod;
	 break;
      case Brw_ADMI_DOC_PRJ:
      case Brw_ADMI_ASS_PRJ:
	 /* Cod stores the project code */
	 *GrpCod = -1L;
	 *CrsCod = Crs.HieCod = Prj_DB_GetCrsOfPrj (Cod);
	 Hie_GetDataByCod[Hie_CRS] (&Crs);
	 *DegCod = Deg.HieCod = Crs.PrtCod;
	 Hie_GetDataByCod[Hie_DEG] (&Deg);
	 *CtrCod = Ctr.HieCod = Deg.PrtCod;
	 Hie_GetDataByCod[Hie_CTR] (&Ctr);
	 *InsCod = Ctr.PrtCod;
	 break;
      default:
	 *GrpCod =
	 *CrsCod =
	 *DegCod =
	 *CtrCod =
	 *InsCod = -1L;
	 break;
     }
  }

/*****************************************************************************/
/**************** Remove a file or folder from the database ******************/
/*****************************************************************************/

static void Brw_RemoveOneFileOrFolderFromDB (const char Path[PATH_MAX + 1])
  {
   /***** Set possible notifications as removed.
          Set possible social note as unavailable.
          Important: do this before removing from files *****/
   Ntf_MarkNotifOneFileAsRemoved (Path);
   TmlNot_MarkNoteOneFileAsUnavailable (Path);

   /***** Remove from database the entries that store
          the marks properties, file views and file data *****/
   Brw_DB_RemoveOneFileOrFolder (Path);
  }

/*****************************************************************************/
/*************** Remove children of a folder from the database ***************/
/*****************************************************************************/

static void Brw_RemoveChildrenOfFolderFromDB (const char Path[PATH_MAX + 1])
  {
   /***** Set possible notifications as removed.
          Set possible social notes as unavailable.
          Important: do this before removing from files *****/
   Ntf_MarkNotifChildrenOfFolderAsRemoved (Path);
   TmlNot_MarkNotesChildrenOfFolderAsUnavailable (Path);

   /***** Remove from database the entries that store
          the marks properties, file views and file data *****/
   Brw_DB_RemoveChildrenOfFolder (Path);
  }

/*****************************************************************************/
/********** Check if I have permission to modify a file or folder ************/
/*****************************************************************************/

static void Brw_SetIfICanEditFileOrFolder (bool Value)
  {
   Brw_ICanEditFileOrFolder = Value;
  }

static bool Brw_GetIfICanEditFileOrFolder (void)
  {
   return Brw_ICanEditFileOrFolder;
  }

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
	 if (Gbl.Usrs.Me.Role.Logged == Rol_TCH)	// A teacher...
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
	 if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER && // The main folder of an assignment
             Level == 1)
	    return false;			// Do not remove / rename main folder of assigment

	 return Asg_CheckIfICanCreateIntoAssigment (&Gbl.FileBrowser.Asg);
      case Brw_ADMI_DOC_PRJ:
         return Brw_CheckIfICanModifyPrjDocFileOrFolder ();
      case Brw_ADMI_ASS_PRJ:
         return Brw_CheckIfICanModifyPrjAssFileOrFolder ();
      default:
         return Brw_CheckIfFileBrowserIsEditable (Gbl.FileBrowser.Type);
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
   if (Level >= BrwSiz_MAX_DIR_LEVELS)
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
	 return Asg_CheckIfICanCreateIntoAssigment (&Gbl.FileBrowser.Asg);
      default:
         return Brw_CheckIfFileBrowserIsEditable (Gbl.FileBrowser.Type);
     }
   return false;
  }

/*****************************************************************************/
/********************* Check if file browser is editable *********************/
/*****************************************************************************/

bool Brw_CheckIfFileBrowserIsEditable (Brw_FileBrowser_t FileBrowser)
  {
   static const bool Brw_FileBrowserIsEditable[Brw_NUM_TYPES_FILE_BROWSER] =
     {
      [Brw_UNKNOWN     ] = false,
      [Brw_SHOW_DOC_CRS] = false,
      [Brw_SHOW_MRK_CRS] = false,
      [Brw_ADMI_DOC_CRS] = true,
      [Brw_ADMI_SHR_CRS] = true,
      [Brw_ADMI_SHR_GRP] = true,
      [Brw_ADMI_WRK_USR] = true,
      [Brw_ADMI_WRK_CRS] = true,
      [Brw_ADMI_MRK_CRS] = true,
      [Brw_ADMI_BRF_USR] = true,
      [Brw_SHOW_DOC_GRP] = false,
      [Brw_ADMI_DOC_GRP] = true,
      [Brw_SHOW_MRK_GRP] = false,
      [Brw_ADMI_MRK_GRP] = true,
      [Brw_ADMI_ASG_USR] = true,
      [Brw_ADMI_ASG_CRS] = true,
      [Brw_SHOW_DOC_DEG] = false,
      [Brw_ADMI_DOC_DEG] = true,
      [Brw_SHOW_DOC_CTR] = false,
      [Brw_ADMI_DOC_CTR] = true,
      [Brw_SHOW_DOC_INS] = false,
      [Brw_ADMI_DOC_INS] = true,
      [Brw_ADMI_SHR_DEG] = true,
      [Brw_ADMI_SHR_CTR] = true,
      [Brw_ADMI_SHR_INS] = true,
      [Brw_ADMI_TCH_CRS] = true,
      [Brw_ADMI_TCH_GRP] = true,
      [Brw_ADMI_DOC_PRJ] = true,
      [Brw_ADMI_ASS_PRJ] = true,
     };

   return Brw_FileBrowserIsEditable[FileBrowser];
  }

/*****************************************************************************/
/********** Check if I have permission to modify a file or folder ************/
/********** in the current shared or private zone                 ************/
/*****************************************************************************/
// Returns true if I can remove or rename Gbl.FileBrowser.FilFolLnk.Full, and false if I have not permission
// I can remove or rename a file if I am the publisher
// I can remove or rename a folder if I am the unique publisher of all files and folders in the subtree starting there

static bool Brw_CheckIfICanModifySharedFileOrFolder (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:	// If I am a student or a non-editing teacher...
      case Rol_NET:	// ...I can modify the file/folder if I am the publisher
         return (Gbl.Usrs.Me.UsrDat.UsrCod == Brw_DB_GetPublisherOfSubtree (Gbl.FileBrowser.FilFolLnk.Full));	// Am I the publisher of subtree?
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
         return (Gbl.Usrs.Me.UsrDat.UsrCod == Brw_DB_GetPublisherOfSubtree (Gbl.FileBrowser.FilFolLnk.Full));	// Am I the publisher of subtree?
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
/********** Check if I have permission to modify a file or folder ************/
/********** in the current project documents zone                 ************/
/*****************************************************************************/
// Returns true if I can remove or rename Gbl.FileBrowser.FilFolLnk.Full, and false if I have not permission
// I can remove or rename a file if I am the publisher
// I can remove or rename a folder if I am the unique publisher of all files and folders in the subtree starting there

static bool Brw_CheckIfICanModifyPrjDocFileOrFolder (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
	 if (Prj_GetMyRolesInProject (Prj_GetPrjCod ()))	// I am a member
            return (Gbl.Usrs.Me.UsrDat.UsrCod == Brw_DB_GetPublisherOfSubtree (Gbl.FileBrowser.FilFolLnk.Full));	// Am I the publisher of subtree?
	 return false;
      case Rol_TCH:	// Editing teachers in a course can access to all files
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
// I can remove or rename a folder if I am the unique publisher of all files and folders in the subtree starting there

static bool Brw_CheckIfICanModifyPrjAssFileOrFolder (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
	 if ((Prj_GetMyRolesInProject (Prj_GetPrjCod ()) & (1 << Prj_ROLE_TUT |		// Tutor...
	                                                    1 << Prj_ROLE_EVL)))	// ...or evaluator
            return (Gbl.Usrs.Me.UsrDat.UsrCod == Brw_DB_GetPublisherOfSubtree (Gbl.FileBrowser.FilFolLnk.Full));	// Am I the publisher of subtree?
	 return false;
      case Rol_TCH:	// Editing teachers in a course can access to all files
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
   return false;
  }

/*****************************************************************************/
/************* Remove common zones of all groups of a type ***************/
/*****************************************************************************/

void Brw_RemoveZonesOfGroupsOfType (long GrpTypCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumGrps;
   unsigned NumGrp;
   long GrpCod;

   /***** Query database *****/
   NumGrps = Grp_DB_GetGrpsOfType (&mysql_res,GrpTypCod);
   for (NumGrp = 0;
	NumGrp < NumGrps;
	NumGrp++)
     {
      /* Get next group */
      GrpCod = DB_GetNextCode (mysql_res);

      /* Remove file zones of this group */
      Brw_RemoveGrpZones (Gbl.Hierarchy.Node[Hie_CRS].HieCod,GrpCod);
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
   Ntf_DB_MarkNotifFilesInGroupAsRemoved (GrpCod);

   /***** Remove files in the group from database *****/
   Brw_DB_RemoveGrpFiles (GrpCod);

   /***** Remove group zones *****/
   snprintf (PathGrpFileZones,sizeof (PathGrpFileZones),"%s/%ld/grp/%ld",
             Cfg_PATH_CRS_PRIVATE,CrsCod,GrpCod);
   Fil_RemoveTree (PathGrpFileZones);
  }

/*****************************************************************************/
/***************** Remove the works of a user in a course ********************/
/*****************************************************************************/

void Brw_RemoveUsrWorksInCrs (struct Usr_Data *UsrDat,struct Hie_Node *Crs)
  {
   char PathUsrInCrs[PATH_MAX + 1];

   /***** Remove user's works in the course from database *****/
   Brw_DB_RemoveWrkFiles (Crs->HieCod,UsrDat->UsrCod);

   /***** Remove the folder for this user inside the course *****/
   snprintf (PathUsrInCrs,sizeof (PathUsrInCrs),"%s/%ld/usr/%02u/%ld",
             Cfg_PATH_CRS_PRIVATE,Crs->HieCod,
             (unsigned) (UsrDat->UsrCod % 100),UsrDat->UsrCod);
   Fil_RemoveTree (PathUsrInCrs);
   // If this was the last user in his/her subfolder ==> the subfolder will be empty
  }

/*****************************************************************************/
/************* Remove the works of a user in all of his courses **************/
/*****************************************************************************/

void Brw_RemoveUsrWorksInAllCrss (struct Usr_Data *UsrDat)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   unsigned NumCrssWorksRemoved = 0;
   struct Hie_Node Crs;

   /***** Query database *****/
   NumCrss = Crs_DB_GetCrssFromUsr (&mysql_res,UsrDat->UsrCod,-1L);

   /***** Remove the zone of works of the user in the courses he/she belongs to *****/
   for (NumCrs = 0;
	NumCrs < NumCrss;
	NumCrs++)
     {
      /* Get the next course */
      row = mysql_fetch_row (mysql_res);
      Crs.HieCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get data of course */
      Hie_GetDataByCod[Hie_CRS] (&Crs);
      Brw_RemoveUsrWorksInCrs (UsrDat,&Crs);
      NumCrssWorksRemoved++;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Get summary and content of a file **********************/
/*****************************************************************************/

void Brw_GetSummaryAndContentOfFile (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                     char **ContentStr,
                                     long FilCod,Ntf_GetContent_t GetContent)
  {
   extern const char *Txt_Filename;
   extern const char *Txt_Folder;
   extern const char *Txt_Uploaded_by;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   struct Brw_FileMetadata FileMetadata;
   bool FileHasPublisher;
   struct Usr_Data PublisherUsrDat;

   /***** Return nothing on error *****/
   SummaryStr[0] = '\0';	// Return nothing on error
   if (GetContent == Ntf_GET_CONTENT && ContentStr)
      *ContentStr = NULL;

   /***** Get file metadata *****/
   FileMetadata.FilCod = FilCod;
   Brw_GetFileMetadataByCod (&FileMetadata);

   /***** Copy file name into summary string *****/
   Str_Copy (SummaryStr,FileMetadata.FilFolLnk.Name,Ntf_MAX_BYTES_SUMMARY);

   /***** Copy some file metadata into content string *****/
   if (GetContent == Ntf_GET_CONTENT && ContentStr)
     {
      /* Get publisher */
      if (FileMetadata.PublisherUsrCod > 0)
	{
	 /* Initialize structure with publisher's data */
	 Usr_UsrDataConstructor (&PublisherUsrDat);
	 PublisherUsrDat.UsrCod = FileMetadata.PublisherUsrCod;
	 FileHasPublisher = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&PublisherUsrDat,
	                                                             Usr_DONT_GET_PREFS,
	                                                             Usr_DONT_GET_ROLE_IN_CRS);
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
	 Err_NotEnoughMemoryExit ();

      /* Free memory used for publisher's data */
      if (FileMetadata.PublisherUsrCod > 0)
	 Usr_UsrDataDestructor (&PublisherUsrDat);
     }
  }

/*****************************************************************************/
/**************************** List documents found ***************************/
/*****************************************************************************/

void Brw_ListDocsFound (MYSQL_RES **mysql_res,unsigned NumDocs,
			const char *TitleSingular,const char *TitlePlural)
  {
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_File_zone;
   extern const char *Txt_Document;
   extern const char *Txt_hidden_document;
   extern const char *Txt_hidden_documents;
   MYSQL_ROW row;
   char *Title;
   Hie_Level_t Level;
   unsigned NumDoc;
   unsigned NumDocsNotHidden = 0;
   unsigned NumDocsHidden;

   /***** Query database *****/
   if (NumDocs)
     {
      /***** Begin box and table *****/
      /* Number of documents found */
      if (asprintf (&Title,"%u %s",NumDocs,NumDocs == 1 ? TitleSingular :
							  TitlePlural) < 0)
	 Err_NotEnoughMemoryExit ();
      Box_BoxTableBegin (Title,NULL,NULL,NULL,Box_NOT_CLOSABLE,2);
      free (Title);

	 /***** Write heading *****/
	 HTM_TR_Begin (NULL);
	    HTM_TH_Span (NULL,HTM_HEAD_CENTER,1,1,"BT");
	    for (Level  = Hie_INS;
		 Level >= Hie_CRS;
		 Level++)
               HTM_TH (Txt_HIERARCHY_SINGUL_Abc[Level],HTM_HEAD_LEFT);
            HTM_TH (Txt_File_zone,HTM_HEAD_LEFT);
            HTM_TH (Txt_Document ,HTM_HEAD_LEFT);
	 HTM_TR_End ();

	 /***** List documents found *****/
	 for (NumDoc  = 1;
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
	    HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,7,NULL);
	       HTM_Txt ("(");
	       NumDocsHidden = NumDocs - NumDocsNotHidden;
	       HTM_TxtF ("%u %s",
			 NumDocsHidden,NumDocsHidden == 1 ? Txt_hidden_document :
							    Txt_hidden_documents);
	       HTM_Txt (")");
	    HTM_TH_End ();

	 HTM_TR_End ();

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/************ Write the data of a document (result of a query) ***************/
/*****************************************************************************/

static void Brw_WriteRowDocData (unsigned *NumDocsNotHidden,MYSQL_ROW row)
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
   extern const char *Txt_Folder;
   extern const char *Txt_Link;
   struct Brw_FileMetadata FileMetadata;
   struct Hie_Node Hie[Hie_NUM_LEVELS];
   long GrpCod;
   Act_Action_t Action;
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

   if (!Brw_DB_CheckIfFileOrFolderIsSetAsHiddenUsingMetadata (&FileMetadata))
     {
      /***** Get institution code (row[2]) *****/
      Hie[Hie_INS].HieCod = Str_ConvertStrCodToLongCod (row[2]);
      Str_Copy (Hie[Hie_INS].ShrtName,row[3],sizeof (Hie[Hie_INS].ShrtName) - 1);

      /***** Get center code (row[4]) *****/
      Hie[Hie_CTR].HieCod = Str_ConvertStrCodToLongCod (row[4]);
      Str_Copy (Hie[Hie_CTR].ShrtName,row[5],sizeof (Hie[Hie_CTR].ShrtName) - 1);

      /***** Get degree code (row[6]) *****/
      Hie[Hie_DEG].HieCod = Str_ConvertStrCodToLongCod (row[6]);
      Str_Copy (Hie[Hie_DEG].ShrtName,row[7],sizeof (Hie[Hie_DEG].ShrtName) - 1);

      /***** Get course code (row[8]) *****/
      Hie[Hie_CRS].HieCod = Str_ConvertStrCodToLongCod (row[8]);
      Str_Copy (Hie[Hie_CRS].ShrtName,row[9],sizeof (Hie[Hie_CRS].ShrtName) - 1);

      /***** Get group code (row[8]) *****/
      GrpCod = Str_ConvertStrCodToLongCod (row[10]);

      /***** Set row color *****/
      BgColor = (Hie[Hie_CRS].HieCod > 0 &&
	         Hie[Hie_CRS].HieCod == Gbl.Hierarchy.Node[Hie_CRS].HieCod) ? "BG_HIGHLIGHT" :
									      The_GetColorRows ();

      HTM_TR_Begin (NULL);

	 /***** Write number of document in this search *****/
	 HTM_TD_Begin ("class=\"RT DAT_%s %s\"",
	               The_GetSuffix (),BgColor);
	    HTM_Unsigned (++(*NumDocsNotHidden));
	 HTM_TD_End ();

	 /***** Write institution logo, institution short name *****/
	 HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
	               The_GetSuffix (),BgColor);
	    if (Hie[Hie_INS].HieCod > 0)
	      {
	       Frm_BeginFormGoTo (ActSeeInsInf);
		  ParCod_PutPar (ParCod_Ins,Hie[Hie_INS].HieCod);
		  HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Hie[Hie_INS].ShrtName),
		                           "class=\"LT BT_LINK\"");
		  Str_FreeGoToTitle ();
		     Lgo_DrawLogo (Hie_INS,&Hie[Hie_INS],"LT BT_LINK ICO20x20");
		     HTM_TxtF ("&nbsp;%s",Hie[Hie_INS].ShrtName);
		  HTM_BUTTON_End ();
	       Frm_EndForm ();
	      }
	 HTM_TD_End ();

	 /***** Write center logo, center short name *****/
	 HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
	               The_GetSuffix (),BgColor);
	    if (Hie[Hie_CTR].HieCod > 0)
	      {
	       Frm_BeginFormGoTo (ActSeeCtrInf);
		  ParCod_PutPar (ParCod_Ctr,Hie[Hie_CTR].HieCod);
		  HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Hie[Hie_CTR].ShrtName),
		                           "class=\"LT BT_LINK\"");
		  Str_FreeGoToTitle ();
		     Lgo_DrawLogo (Hie_CTR,&Hie[Hie_CTR],"LT ICO20x20");
		     HTM_TxtF ("&nbsp;%s",Hie[Hie_CTR].ShrtName);
		  HTM_BUTTON_End ();
	       Frm_EndForm ();
	      }
	 HTM_TD_End ();

	 /***** Write degree logo, degree short name *****/
	 HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
	               The_GetSuffix (),BgColor);
	    if (Hie[Hie_DEG].HieCod > 0)
	      {
	       Frm_BeginFormGoTo (ActSeeDegInf);
		  ParCod_PutPar (ParCod_Deg,Hie[Hie_DEG].HieCod);
		  HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Hie[Hie_DEG].ShrtName),
		                           "class=\"LT BT_LINK\"");
		  Str_FreeGoToTitle ();
		     Lgo_DrawLogo (Hie_DEG,&Hie[Hie_DEG],"LT ICO20x20");
		     HTM_TxtF ("&nbsp;%s",Hie[Hie_DEG].ShrtName);
		  HTM_BUTTON_End ();
	       Frm_EndForm ();
	      }
	 HTM_TD_End ();

	 /***** Write course short name *****/
	 HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
	               The_GetSuffix (),BgColor);
	    if (Hie[Hie_CRS].HieCod > 0)
	      {
	       Frm_BeginFormGoTo (ActSeeCrsInf);
		  ParCod_PutPar (ParCod_Crs,Hie[Hie_CRS].HieCod);
		  HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Hie[Hie_CRS].ShrtName),
		                           "class=\"LT BT_LINK\"");
		  Str_FreeGoToTitle ();
		     HTM_Txt (Hie[Hie_CRS].ShrtName);
		  HTM_BUTTON_End ();
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

	 HTM_TD_Begin ("class=\"LT DAT_%s %s\"",The_GetSuffix (),BgColor);
	    HTM_Txt (Title);
	 HTM_TD_End ();

	 /***** Get the name of the file to show *****/
	 Brw_GetFileNameToShow (FileMetadata.FilFolLnk.Type,
				FileMetadata.FilFolLnk.Name,
				FileNameToShow);

	 /***** Write file name using path (row[1]) *****/
	 HTM_TD_Begin ("class=\"LT DAT_STRONG_%s %s\"",The_GetSuffix (),BgColor);

	    /* Begin form */
	    Action = Brw_ActReqDatFile[Brw_FileBrowserForFoundDocs[FileMetadata.FileBrowser]];

	    if (Hie[Hie_CRS].HieCod > 0)
	      {
	       Frm_BeginFormGoTo (Action);
		  ParCod_PutPar (ParCod_Crs,Hie[Hie_CRS].HieCod);	// Go to course
		  if (GrpCod > 0)
		     ParCod_PutPar (ParCod_Grp,GrpCod);
	      }
	    else if (Hie[Hie_DEG].HieCod > 0)
	      {
	       Frm_BeginFormGoTo (Action);
		  ParCod_PutPar (ParCod_Deg,Hie[Hie_DEG].HieCod);	// Go to degree
	      }
	    else if (Hie[Hie_CTR].HieCod > 0)
	      {
	       Frm_BeginFormGoTo (Action);
		  ParCod_PutPar (ParCod_Ctr,Hie[Hie_CTR].HieCod);	// Go to center
	      }
	    else if (Hie[Hie_INS].HieCod > 0)
	      {
	       Frm_BeginFormGoTo (Action);
		  ParCod_PutPar (ParCod_Ins,Hie[Hie_INS].HieCod);	// Go to institution
	      }
	    else
	       Frm_BeginForm (Action);

	    /* Parameters to go to file / folder */
	    if (FileMetadata.FilFolLnk.Type == Brw_IS_FOLDER)
	       Brw_PutImplicitParsFileBrowser (&Gbl.FileBrowser.FilFolLnk);
	    else
	       Brw_PutParsFileBrowser (NULL,		// Not used
				       NULL,		// Not used
				       Brw_IS_UNKNOWN,	// Not used
				       FileMetadata.FilCod);

	    /* File or folder icon */
	    HTM_BUTTON_Submit_Begin (FileNameToShow,"class=\"LT BT_LINK\"");
	       switch (FileMetadata.FilFolLnk.Type)
	         {
		  case Brw_IS_FILE:
		     Brw_PutIconFile (FileMetadata.FilFolLnk.Name,
				      "CONTEXT_ICO16x16",
				      Frm_DONT_PUT_FORM);	// Don't put link to view metadata
		     break;
		  case Brw_IS_FOLDER:
		     Ico_PutIcon ("folder-yellow.png",Ico_UNCHANGED,
				  Txt_Folder,"CONTEXT_ICO16x16");
		     break;
		  case Brw_IS_LINK:
	             Ico_PutIcon ("up-right-from-square.svg",Ico_BLACK,
	                          Txt_Link,"CONTEXT_ICO16x16");
		     break;
		  default:
		     break;
	         }
	       HTM_TxtF ("&nbsp;%s",FileNameToShow);
	    HTM_BUTTON_End ();

	    /* End form */
	    Frm_EndForm ();

	 HTM_TD_End ();
      HTM_TR_End ();

      The_ChangeRowColor ();
     }
  }

/*****************************************************************************/
/***************** Write a form (link) to remove old files *******************/
/*****************************************************************************/

static void Brw_PutLinkToAskRemOldFiles (void)
  {
   extern const char *Txt_Remove_old_files;

   Lay_PutContextualLinkIconText (ActReqRemOldBrf,NULL,
				  Brw_PutParFullTreeIfSelected,&Gbl.FileBrowser.FullTree,
				  "trash.svg",Ico_RED,
				  Txt_Remove_old_files,NULL);
  }

/*****************************************************************************/
/************** Write a form fo confirm removing of old files ****************/
/*****************************************************************************/

void Brw_AskRemoveOldFilesBriefcase (void)
  {
   extern const char *Txt_Remove_old_files;
   extern const char *Txt_Remove_files_older_than_PART_1_OF_2;
   extern const char *Txt_Remove_files_older_than_PART_2_OF_2;
   unsigned Months;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Begin form *****/
   Frm_BeginForm (ActRemOldBrf);
      Brw_PutParFullTreeIfSelected (&Gbl.FileBrowser.FullTree);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Remove_old_files,NULL,NULL,NULL,Box_NOT_CLOSABLE);

	 /***** Form to request number of months (to remove files older) *****/
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_TxtF ("%s&nbsp;",Txt_Remove_files_older_than_PART_1_OF_2);
	    HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,NULL,
			      "name=\"Months\" class=\"INPUT_%s\"",
			      The_GetSuffix ());
	       for (Months  = Brw_MIN_MONTHS_TO_REMOVE_OLD_FILES;
		    Months <= Brw_MAX_MONTHS_IN_BRIEFCASE;
		    Months++)
		  HTM_OPTION (HTM_Type_UNSIGNED,&Months,
			      Months == Brw_DEF_MONTHS_TO_REMOVE_OLD_FILES ? HTM_OPTION_SELECTED :
									     HTM_OPTION_UNSELECTED,
			      HTM_OPTION_ENABLED,
			      "%u",Months);
	    HTM_SELECT_End ();
	    HTM_NBSP ();
	    HTM_TxtF (Txt_Remove_files_older_than_PART_2_OF_2,
		      Cfg_PLATFORM_SHORT_NAME);
	 HTM_LABEL_End ();

      /***** Send button and end box *****/
      Box_BoxWithButtonEnd (Btn_REMOVE_BUTTON,Act_GetActionText (ActRemOldBrf));

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

   if (Brw_TypeIsAdmCrsAsgWrk[Gbl.FileBrowser.Type])
     {
      /***** Get parameter with number of months without access *****/
      Months = (unsigned)
	       Par_GetParUnsignedLong ("Months",
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
   Brw_ShowFileBrowserNormal ();
  }

/*****************************************************************************/
/******************************* Remove old files ****************************/
/*****************************************************************************/

static void Brw_RemoveOldFilesInBrowser (unsigned Months,struct Brw_NumObjects *Removed)
  {
   time_t TimeRemoveFilesOlder;

   /***** Compute time in seconds
          (files older than this time will be removed) *****/
   TimeRemoveFilesOlder = Dat_GetStartExecutionTimeUTC () -
	                  (time_t) Months * Dat_SECONDS_IN_ONE_MONTH;

   /***** Remove old files recursively *****/
   Removed->NumFiles =
   Removed->NumLinks =
   Removed->NumFolds = 0;
   Brw_ScanDirRemovingOldFiles (1,Gbl.FileBrowser.Path.RootFolder,
                                Brw_RootFolderInternalNames[Gbl.FileBrowser.Type],
                                TimeRemoveFilesOlder,Removed);

   /***** Remove affected clipboards *****/
   if (Removed->NumFiles ||
       Removed->NumLinks ||
       Removed->NumFolds)	// If anything has been changed
      Brw_DB_RemoveAffectedClipboards (Gbl.FileBrowser.Type,
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
      Err_ShowErrorAndExit ("Can not get information about a file or folder.");
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
	    snprintf (PathFileRel,sizeof (PathFileRel),"%s/%s",
		      Path,FileList[NumFile]->d_name);
	    snprintf (PathFileInExplTree,sizeof (PathFileInExplTree),"%s/%s",
		      PathInTree,FileList[NumFile]->d_name);

	    /***** Get file or folder status *****/
	    if (lstat (PathFileRel,&FileStatus))	// On success ==> 0 is returned
	       Err_ShowErrorAndExit ("Can not get information about a file or folder.");
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
	       Err_ShowErrorAndExit ("Error while scanning directory.");
	   }

	 if (NumFiles <= 2 &&					// It's an empty folder
	     FolderStatus.st_mtime < TimeRemoveFilesOlder)	//  ..and it was old before deletion
	   {
	    /* Remove folder from disk and database */
	    if (Brw_RemoveFolderFromDiskAndDB (Path,PathInTree))
	       Err_ShowErrorAndExit ("Can not remove folder.");

	    /* Update number of files/links removed */
	    (Removed->NumFolds)++;
	   }
	}
     }
   else
      Err_ShowErrorAndExit ("Error while scanning directory.");
  }

/*****************************************************************************/
/******************* Remove file/link from disk and database *****************/
/*****************************************************************************/

static void Brw_RemoveFileFromDiskAndDB (const char Path[PATH_MAX + 1],
                                         const char FullPathInTree[PATH_MAX + 1])
  {
   /***** Remove file from disk *****/
   if (unlink (Path))
      Err_ShowErrorAndExit ("Can not remove file / link.");

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
      Brw_DB_RemoveAffectedExpandedFolders (FullPathInTree);
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


/*****************************************************************************/
/********************* Show stats about exploration trees ********************/
/*****************************************************************************/
// TODO: add links to statistic

void Brw_GetAndShowFileBrowsersStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_folders_and_files;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_STAT_COURSE_FILE_ZONES[Fig_NUM_STAT_CRS_FILE_ZONES];
   static const Brw_FileBrowser_t StatCrsFileZone[Fig_NUM_STAT_CRS_FILE_ZONES] =
     {
      Brw_ADMI_DOC_CRS,
      Brw_ADMI_DOC_GRP,
      Brw_ADMI_TCH_CRS,
      Brw_ADMI_TCH_GRP,
      Brw_ADMI_SHR_CRS,
      Brw_ADMI_SHR_GRP,
      Brw_ADMI_MRK_CRS,
      Brw_ADMI_MRK_GRP,
      Brw_ADMI_ASG_USR,
      Brw_ADMI_WRK_USR,
      Brw_UNKNOWN,
      Brw_ADMI_BRF_USR,
     };
   struct BrwSiz_SizeOfFileZone SizeOfFileZone[Fig_NUM_STAT_CRS_FILE_ZONES];
   unsigned NumStat;

   /***** Get sizes of all file zones *****/
   for (NumStat = 0;
	NumStat < Fig_NUM_STAT_CRS_FILE_ZONES;
	NumStat++)
      BrwSiz_GetSizeOfFileZone (StatCrsFileZone[NumStat],
				&SizeOfFileZone[NumStat]);

   /***** Begin box *****/
   Box_BoxBegin (Txt_FIGURE_TYPES[Fig_FOLDERS_AND_FILES],NULL,NULL,
                 Hlp_ANALYTICS_Figures_folders_and_files,Box_NOT_CLOSABLE);

      /***** Write sizes of all file zones *****/
      HTM_TABLE_BeginCenterPadding (2);
	 Brw_WriteStatsFileZonesTableHead1 ();
	 for (NumStat = 0;
	      NumStat < Fig_NUM_STAT_CRS_FILE_ZONES;
	      NumStat++)
	    Brw_WriteRowStatsFileBrowsers1 (Txt_STAT_COURSE_FILE_ZONES[NumStat],
					    StatCrsFileZone[NumStat],
					    &SizeOfFileZone[NumStat]);
      HTM_TABLE_End ();

      /***** Write sizes of all file zones per course *****/
      HTM_TABLE_BeginCenterPadding (2);
	 Brw_WriteStatsFileZonesTableHead2 ();
	 for (NumStat = 0;
	      NumStat < Fig_NUM_STAT_CRS_FILE_ZONES;
	      NumStat++)
	    Brw_WriteRowStatsFileBrowsers2 (Txt_STAT_COURSE_FILE_ZONES[NumStat],
					    StatCrsFileZone[NumStat],
					    &SizeOfFileZone[NumStat]);
      HTM_TABLE_End ();

      /***** Write sizes of all file zones per user *****/
      HTM_TABLE_BeginCenterPadding (2);
	 Brw_WriteStatsFileZonesTableHead3 ();
	 for (NumStat = 0;
	      NumStat < Fig_NUM_STAT_CRS_FILE_ZONES;
	      NumStat++)
	    Brw_WriteRowStatsFileBrowsers3 (Txt_STAT_COURSE_FILE_ZONES[NumStat],
					    StatCrsFileZone[NumStat],
					    &SizeOfFileZone[NumStat]);
      HTM_TABLE_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*************** Write table heading for stats of file zones *****************/
/*****************************************************************************/

static void Brw_WriteStatsFileZonesTableHead1 (void)
  {
   extern const char *Txt_File_zones;
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Groups;
   extern const char *Txt_Users;
   extern const char *Txt_Max_levels;
   extern const char *Txt_Folders;
   extern const char *Txt_Files;
   extern const char *Txt_Size;

   HTM_TR_Begin (NULL);
      HTM_TH (Txt_File_zones		       ,HTM_HEAD_LEFT);
      HTM_TH (Txt_HIERARCHY_PLURAL_Abc[Hie_CRS],HTM_HEAD_RIGHT);
      HTM_TH (Txt_Groups		       ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Users			       ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Max_levels		       ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Folders		       ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Files			       ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Size			       ,HTM_HEAD_RIGHT);
   HTM_TR_End ();
  }

static void Brw_WriteStatsFileZonesTableHead2 (void)
  {
   extern const char *Txt_File_zones;
   extern const char *Txt_Folders;
   extern const char *Txt_Files;
   extern const char *Txt_Size;
   extern const char *Txt_HIERARCHY_SINGUL_abc[Hie_NUM_LEVELS];

   HTM_TR_Begin (NULL);

      HTM_TH (Txt_File_zones,HTM_HEAD_LEFT);

      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 HTM_TxtF ("%s/",Txt_Folders);
	 HTM_BR ();
	 HTM_Txt (Txt_HIERARCHY_SINGUL_abc[Hie_CRS]);
      HTM_TH_End ();

      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 HTM_TxtF ("%s/",Txt_Files);
	 HTM_BR ();
	 HTM_Txt (Txt_HIERARCHY_SINGUL_abc[Hie_CRS]);
      HTM_TH_End ();

      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 HTM_TxtF ("%s/",Txt_Size);
	 HTM_BR ();
	 HTM_Txt (Txt_HIERARCHY_SINGUL_abc[Hie_CRS]);
      HTM_TH_End ();

   HTM_TR_End ();
  }

static void Brw_WriteStatsFileZonesTableHead3 (void)
  {
   extern const char *Txt_File_zones;
   extern const char *Txt_Folders;
   extern const char *Txt_Files;
   extern const char *Txt_Size;
   extern const char *Txt_user[Usr_NUM_SEXS];

   HTM_TR_Begin (NULL);

      HTM_TH (Txt_File_zones,HTM_HEAD_LEFT);

      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 HTM_TxtF ("%s/",Txt_Folders);
	 HTM_BR ();
	 HTM_Txt (Txt_user[Usr_SEX_UNKNOWN]);
      HTM_TH_End ();

      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 HTM_TxtF ("%s/",Txt_Files);
	 HTM_BR ();
	 HTM_Txt (Txt_user[Usr_SEX_UNKNOWN]);
      HTM_TH_End ();

      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 HTM_TxtF ("%s/",Txt_Size);
	 HTM_BR ();
	 HTM_Txt (Txt_user[Usr_SEX_UNKNOWN]);
      HTM_TH_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/*************** Write a row of stats of exploration trees *******************/
/*****************************************************************************/

static void Brw_WriteRowStatsFileBrowsers1 (const char *NameOfFileZones,
					    Brw_FileBrowser_t FileZone,
                                            struct BrwSiz_SizeOfFileZone *SizeOfFileZone)
  {
   char StrNumCrss[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   char StrNumGrps[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   char StrNumUsrs[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   char FileSizeStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];
   const char *Class = (FileZone == Brw_UNKNOWN) ? "LINE_TOP DAT_STRONG" :
	                                           "DAT";

   Fil_WriteFileSizeFull ((double) SizeOfFileZone->Size,FileSizeStr);

   if (SizeOfFileZone->NumCrss == -1)	// Not applicable
      Str_Copy (StrNumCrss,"-",sizeof (StrNumCrss) - 1);
   else
      snprintf (StrNumCrss,sizeof (StrNumCrss),"%d",
		SizeOfFileZone->NumCrss);

   if (SizeOfFileZone->NumGrps == -1)	// Not applicable
      Str_Copy (StrNumGrps,"-",sizeof (StrNumGrps) - 1);
   else
      snprintf (StrNumGrps,sizeof (StrNumGrps),"%d",
		SizeOfFileZone->NumGrps);

   if (SizeOfFileZone->NumUsrs == -1)	// Not applicable
      Str_Copy (StrNumUsrs,"-",sizeof (StrNumUsrs) - 1);
   else
      snprintf (StrNumUsrs,sizeof (StrNumUsrs),"%d",
		SizeOfFileZone->NumUsrs);

   HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"LM %s_%s\"",Class,The_GetSuffix ());
	 HTM_Txt (NameOfFileZones);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RM %s_%s\"",Class,The_GetSuffix ());
	 HTM_Txt (StrNumCrss);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RM %s_%s\"",Class,The_GetSuffix ());
	 HTM_Txt (StrNumGrps);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RM %s_%s\"",Class,The_GetSuffix ());
	 HTM_Txt (StrNumUsrs);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RM %s_%s\"",Class,The_GetSuffix ());
	 HTM_Unsigned (SizeOfFileZone->MaxLevels);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RM %s_%s\"",Class,The_GetSuffix ());
	 HTM_UnsignedLong (SizeOfFileZone->NumFolders);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RM %s_%s\"",Class,The_GetSuffix ());
	 HTM_UnsignedLong (SizeOfFileZone->NumFiles);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RM %s_%s\"",Class,The_GetSuffix ());
	 HTM_Txt (FileSizeStr);
      HTM_TD_End ();

   HTM_TR_End ();
  }

static void Brw_WriteRowStatsFileBrowsers2 (const char *NameOfFileZones,
					    Brw_FileBrowser_t FileZone,
                                            struct BrwSiz_SizeOfFileZone *SizeOfFileZone)
  {
   char StrNumFoldersPerCrs[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   char StrNumFilesPerCrs[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   char FileSizePerCrsStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];
   const char *Class = (FileZone == Brw_UNKNOWN) ? "LINE_TOP DAT_STRONG" :
						   "DAT";

   if (SizeOfFileZone->NumCrss == -1)	// Not applicable
     {
      Str_Copy (StrNumFoldersPerCrs,"-",sizeof (StrNumFoldersPerCrs) - 1);
      Str_Copy (StrNumFilesPerCrs  ,"-",sizeof (StrNumFilesPerCrs  ) - 1);
      Str_Copy (FileSizePerCrsStr  ,"-",sizeof (FileSizePerCrsStr  ) - 1);
     }
   else
     {
      snprintf (StrNumFoldersPerCrs,sizeof (StrNumFoldersPerCrs),"%.1f",
                SizeOfFileZone->NumCrss ? (double) SizeOfFileZone->NumFolders /
        	                          (double) SizeOfFileZone->NumCrss :
        	                          0.0);
      snprintf (StrNumFilesPerCrs,sizeof (StrNumFilesPerCrs),"%.1f",
                SizeOfFileZone->NumCrss ? (double) SizeOfFileZone->NumFiles /
        	                          (double) SizeOfFileZone->NumCrss :
        	                          0.0);
      Fil_WriteFileSizeFull (SizeOfFileZone->NumCrss ? (double) SizeOfFileZone->Size /
	                                               (double) SizeOfFileZone->NumCrss :
	                                               0.0,
	                     FileSizePerCrsStr);
     }

   HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"LM %s_%s\"",Class,The_GetSuffix ());
	 HTM_Txt (NameOfFileZones);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RM %s_%s\"",Class,The_GetSuffix ());
	 HTM_Txt (StrNumFoldersPerCrs);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RM %s_%s\"",Class,The_GetSuffix ());
	 HTM_Txt (StrNumFilesPerCrs);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RM %s_%s\"",Class,The_GetSuffix ());
	 HTM_Txt (FileSizePerCrsStr);
      HTM_TD_End ();

   HTM_TR_End ();
  }

static void Brw_WriteRowStatsFileBrowsers3 (const char *NameOfFileZones,
					    Brw_FileBrowser_t FileZone,
                                            struct BrwSiz_SizeOfFileZone *SizeOfFileZone)
  {
   char StrNumFoldersPerUsr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   char StrNumFilesPerUsr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   char FileSizePerUsrStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];
   const char *Class = (FileZone == Brw_UNKNOWN) ? "LINE_TOP DAT_STRONG" :
						   "DAT";

   if (SizeOfFileZone->NumUsrs == -1)	// Not applicable
     {
      Str_Copy (StrNumFoldersPerUsr,"-",sizeof (StrNumFoldersPerUsr) - 1);
      Str_Copy (StrNumFilesPerUsr  ,"-",sizeof (StrNumFilesPerUsr  ) - 1);
      Str_Copy (FileSizePerUsrStr  ,"-",sizeof (FileSizePerUsrStr  ) - 1);
     }
   else
     {
      snprintf (StrNumFoldersPerUsr,sizeof (StrNumFoldersPerUsr),"%.1f",
                SizeOfFileZone->NumUsrs ? (double) SizeOfFileZone->NumFolders /
        	                          (double) SizeOfFileZone->NumUsrs :
        	                          0.0);
      snprintf (StrNumFilesPerUsr,sizeof (StrNumFilesPerUsr),"%.1f",
                SizeOfFileZone->NumUsrs ? (double) SizeOfFileZone->NumFiles /
        	                          (double) SizeOfFileZone->NumUsrs :
        	                          0.0);
      Fil_WriteFileSizeFull (SizeOfFileZone->NumUsrs ? (double) SizeOfFileZone->Size /
	                                               (double) SizeOfFileZone->NumUsrs :
	                                               0.0,
	                     FileSizePerUsrStr);
     }

   HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"LM %s_%s\"",Class,The_GetSuffix ());
	 HTM_Txt (NameOfFileZones);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RM %s_%s\"",Class,The_GetSuffix ());
	 HTM_Txt (StrNumFoldersPerUsr);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RM %s_%s\"",Class,The_GetSuffix ());
	 HTM_Txt (StrNumFilesPerUsr);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RM %s_%s\"",Class,The_GetSuffix ());
	 HTM_Txt (FileSizePerUsrStr);
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************ Show stats about Open Educational Resources (OERs) *************/
/*****************************************************************************/

void Brw_GetAndShowOERsStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_open_educational_resources_oer;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_License;
   extern const char *Txt_Number_of_private_files;
   extern const char *Txt_Number_of_public_files;
   extern const char *Txt_LICENSES[Brw_NUM_LICENSES];
   Brw_License_t License;
   unsigned long NumFiles[2];

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_OER],NULL,NULL,
                      Hlp_ANALYTICS_Figures_open_educational_resources_oer,Box_NOT_CLOSABLE,2);

      /***** Write table heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_License                ,HTM_HEAD_LEFT);
	 HTM_TH (Txt_Number_of_private_files,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_public_files ,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      for (License  = (Brw_License_t) 0;
	   License <= (Brw_License_t) (Brw_NUM_LICENSES - 1);
	   License++)
	{
	 Brw_GetNumberOfOERs (License,NumFiles);

	 HTM_TR_Begin (NULL);
	    HTM_TD_Txt_Left (Txt_LICENSES[License]);
	    HTM_TD_UnsignedLong (NumFiles[0]);
	    HTM_TD_UnsignedLong (NumFiles[1]);
	 HTM_TR_End ();
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/**************** Get the size of a file zone from database ******************/
/*****************************************************************************/

static void Brw_GetNumberOfOERs (Brw_License_t License,
                                 unsigned long NumFiles[2])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows = 0;	// Initialized to avoid warning
   unsigned NumRow;
   unsigned Public;

   /***** Get the size of a file browser *****/
   /* Query database */
   NumRows = Brw_DB_GetNumberOfPublicFiles (&mysql_res,License);

   /* Reset values to zero */
   NumFiles[0] = NumFiles[1] = 0L;

   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get if public (row[0]) */
      Public = (row[0][0] == 'Y') ? 1 :
	                            0;

      /* Get number of files (row[1]) */
      if (sscanf (row[1],"%lu",&NumFiles[Public]) != 1)
         Err_ShowErrorAndExit ("Error when getting number of files.");
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }
