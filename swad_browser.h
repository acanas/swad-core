// swad_browser.h: file browsers

#ifndef _SWAD_BRW
#define _SWAD_BRW
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <linux/limits.h>	// For PATH_MAX

#include "swad_constant.h"
#include "swad_course.h"
#include "swad_form.h"
#include "swad_group.h"
#include "swad_notification.h"
#include "swad_private_public.h"

/*****************************************************************************/
/************************ Public types and constants *************************/
/*****************************************************************************/

#define Brw_MAX_CHARS_FOLDER	30		// 30
#define Brw_MAX_BYTES_FOLDER	NAME_MAX	// 255

#define Brw_NUM_TYPES_FILE_BROWSER 29
// The following types are stored in several database tables as numeric fields,
// so don't change numbers!
typedef enum
  {
   Brw_UNKNOWN      =  0,
   Brw_SHOW_DOC_CRS =  1,
   Brw_SHOW_MRK_CRS =  2,
   Brw_ADMI_DOC_CRS =  3,
   Brw_ADMI_SHR_CRS =  4,
   Brw_ADMI_SHR_GRP =  5,
   Brw_ADMI_WRK_USR =  6,
   Brw_ADMI_WRK_CRS =  7,
   Brw_ADMI_MRK_CRS =  8,
   Brw_ADMI_BRF_USR =  9,
   Brw_SHOW_DOC_GRP = 10,
   Brw_ADMI_DOC_GRP = 11,
   Brw_SHOW_MRK_GRP = 12,
   Brw_ADMI_MRK_GRP = 13,
   Brw_ADMI_ASG_USR = 14,
   Brw_ADMI_ASG_CRS = 15,
   Brw_SHOW_DOC_DEG = 16,
   Brw_ADMI_DOC_DEG = 17,
   Brw_SHOW_DOC_CTR = 18,
   Brw_ADMI_DOC_CTR = 19,
   Brw_SHOW_DOC_INS = 20,
   Brw_ADMI_DOC_INS = 21,
   Brw_ADMI_SHR_DEG = 22,
   Brw_ADMI_SHR_CTR = 23,
   Brw_ADMI_SHR_INS = 24,
   Brw_ADMI_TCH_CRS = 25,
   Brw_ADMI_TCH_GRP = 26,
   Brw_ADMI_DOC_PRJ = 27,
   Brw_ADMI_ASS_PRJ = 28,
  } Brw_FileBrowser_t;

// The following types are stored in files and clipboard tables as numeric fields, so don't change numbers!
#define Brw_NUM_FILE_TYPES	4
typedef enum
  {
   Brw_IS_UNKNOWN	= 0,
   Brw_IS_FILE		= 1,
   Brw_IS_FOLDER	= 2,
   Brw_IS_LINK		= 3,
  } Brw_FileType_t;

#define Brw_NUM_UPLOAD_TYPES 2
typedef enum
  {
   Brw_CLASSIC_UPLOAD,
   Brw_DROPZONE_UPLOAD,
  } Brw_UploadType_t;

#define Brw_NUM_ONLY_PUBLIC_FILES 2
typedef enum
  {
   Brw_ANY_FILE,
   Brw_ONLY_PUBLIC_FILES,
  } Brw_OnlyPublicFiles_t;

#define Brw_NUM_LICENSES 8
typedef enum	// Don't change these values! They are stored in database as numbers
  {
   Brw_LICENSE_UNKNOWN             = 0,	// Unknown license / other license
   Brw_LICENSE_ALL_RIGHTS_RESERVED = 1,	// All Rights Reserved
   Brw_LICENSE_CC_BY               = 2,	// Creative Commons Attribution License
   Brw_LICENSE_CC_BY_SA            = 3,	// Creative Commons Attribution-ShareAlike License
   Brw_LICENSE_CC_BY_ND            = 4,	// Creative Commons Attribution-NoDerivs License
   Brw_LICENSE_CC_BY_NC            = 5,	// Creative Commons Attribution-NonCommercial License
   Brw_LICENSE_CC_BY_NC_SA         = 6,	// Creative Commons Attribution-NonCommercial-ShareAlike License
   Brw_LICENSE_CC_BY_NC_ND         = 7,	// Creative Commons Attribution-NonCommercial-NoDerivs License
  } Brw_License_t;
#define Brw_LICENSE_DEFAULT Brw_LICENSE_ALL_RIGHTS_RESERVED

typedef enum
  {
   Brw_HEADER = 0,
   Brw_FOOTER = 1,
  } Brw_HeadOrFoot_t;

struct Brw_FilFolLnk
  {
   char Full[PATH_MAX + 1];	// Full path = Path/Name
   char Path[PATH_MAX + 1];	// Path in tree, without ending '/', until file, folder or link name
   char Name[NAME_MAX + 1];	// File, folder or link name
   Brw_FileType_t Type;
  };

struct Brw_FileMetadata
  {
   long FilCod;
   Brw_FileBrowser_t FileBrowser;
   long Cod;	// Code of institution, center, degree, course or group
   long ZoneUsrCod;
   long PublisherUsrCod;
   struct Brw_FilFolLnk FilFolLnk;
   HidVis_HiddenOrVisible_t Hidden;
   PriPub_PrivateOrPublic_t Public;
   Brw_License_t License;
   off_t Size;
   time_t Time;
   unsigned NumMyViews;
   unsigned NumPublicViews;
   unsigned NumViewsFromLoggedUsrs;
   unsigned NumLoggedUsrs;
  };

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define BrwSiz_MAX_DIR_LEVELS	10	// Maximum number of subdirectory levels in file browsers

#define Brw_MAX_ROW_ID	((1 + BrwSiz_MAX_DIR_LEVELS) * (10 + 1))

#define Brw_MAX_BYTES_MIME_TYPE	(128 - 1)	// 127: maximum size in bytes of "image/jpeg", "text/html", etc.

#define Brw_MAX_CHARS_LICENSE	(128 - 1)	// 127
#define Brw_MAX_BYTES_LICENSE	((Brw_MAX_CHARS_LICENSE + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Brw_INTERNAL_NAME_ROOT_FOLDER_DOCUMENTS			"doc"
#define Brw_INTERNAL_NAME_ROOT_FOLDER_SHARED_FILES		"sha"
#define Brw_INTERNAL_NAME_ROOT_FOLDER_DOWNLOAD			"descarga"		// TODO: It should be Brw_INTERNAL_NAME_ROOT_FOLDER_DOCUMENTS
#define Brw_INTERNAL_NAME_ROOT_FOLDER_TEACHERS			"tch"
#define Brw_INTERNAL_NAME_ROOT_FOLDER_SHARED			"comun"			// TODO: It should be "sha"
#define Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS		"actividades"		// TODO: It should be "asg"
#define Brw_INTERNAL_NAME_ROOT_FOLDER_WORKS			"trabajos"		// TODO: It should be "wrk"
#define Brw_INTERNAL_NAME_ROOT_FOLDER_PROJECT_DOCUMENTS		"doc"
#define Brw_INTERNAL_NAME_ROOT_FOLDER_PROJECT_ASSESSMENT	"ass"
#define Brw_INTERNAL_NAME_ROOT_FOLDER_MARKS			"calificaciones"	// TODO: It should be "mrk"
#define Brw_INTERNAL_NAME_ROOT_FOLDER_BRIEF			"maletin"		// TODO: It should be "brf"

#define Brw_MIN_MONTHS_TO_REMOVE_OLD_FILES	3		// 3 months
#define Brw_DEF_MONTHS_TO_REMOVE_OLD_FILES	6		// 6 months
#define Brw_MAX_MONTHS_IN_BRIEFCASE		(1 * 12)	// 1 year

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Brw_SetGrpCod (long GrpCod);
long Brw_GetGrpCod (void);

void Brw_GetParAndInitFileBrowser (void);
void Brw_InitializeFileBrowser (void);
Exi_Exist_t Brw_CheckIfExistsFolderAssigmentForAnyUsr (const char *FolderName);
Err_SuccessOrError_t Brw_UpdateFoldersAssigmentsIfExistForAllUsrs (const char *OldFolderName,
								   const char *NewFolderName);
void Brw_RemoveFoldersAssignmentsIfExistForAllUsrs (const char *FolderName);

void Brw_GetSelectedUsrsAndShowWorks (void);
void Brw_ShowFileBrowserOrWorks (void);
void Brw_ShowAgainFileBrowserOrWorks (void);

void Brw_PutParFullTreeIfSelected (void *ShowFullTree);

void Brw_CreateDirDownloadTmp (void);

void Brw_AskEditWorksCrs (void);

void Brw_ShowFileBrowserNormal (void);
void Brw_ShowFileBrowserProject (long PrjCod);

void Brw_PutLegalNotice (void);

void Brw_ReqRemFile (void);
void Brw_RemFile (void);
void Brw_RemFolder (void);
void Brw_ExpandFileTree (void);
void Brw_ContractFileTree (void);
void Brw_Copy (void);
void Brw_Paste (void);
void Brw_RemSubtree (void);
void Brw_ShowFormFileBrowser (void);
void Brw_CreateFolder (void);
void Brw_RenFolder (void);
void Brw_RcvFileDZ (void);
void Brw_RcvFileClassic (void);
void Brw_CreateLink (void);

void Brw_SetDocumentAsVisible (void);
void Brw_SetDocumentAsHidden (void);
HidVis_HiddenOrVisible_t Brw_CheckIfFileOrFolderIsHidden (Brw_FileType_t FileType,
                                                          const char *Path);
void Brw_ShowFileMetadata (void);

void Brw_GetLinkToDownloadFile (const char *PathInTree,const char *FileName,char *URL);
void Brw_DownloadFile (void);
void Brw_ChgFileMetadata (void);
void Brw_GetFileMetadataByPath (struct Brw_FileMetadata *FileMetadata);
void Brw_GetFileMetadataByCod (struct Brw_FileMetadata *FileMetadata);
Exi_Exist_t Brw_GetFileTypeSizeAndDate (struct Brw_FileMetadata *FileMetadata);
void Brw_GetAndUpdateFileViews (struct Brw_FileMetadata *FileMetadata);
void Brw_UpdateMyFileViews (long FilCod);

long Brw_GetZoneUsrCodForFileBrowser (void);
void Brw_GetCrsGrpFromFileMetadata (Brw_FileBrowser_t FileBrowser,long Cod,
                                    long *InsCod,
                                    long *CtrCod,
                                    long *DegCod,
                                    long *CrsCod,
                                    long *GrpCod);

bool Brw_CheckIfFileBrowserIsEditable (Brw_FileBrowser_t FileBrowser);

long Brw_GetCodForFileBrowser (Brw_FileBrowser_t FileBrowser);
long Brw_GetZoneUsrCodForFileBrowser (void);

void Brw_DB_RemoveExpiredExpandedFolders (void);

void Brw_SetFullPathInTree (void);

void Brw_PutIconFile (const char *FileName,
		      const char *Class,Frm_PutForm_t PutFormToGo);

//-----------------------------------------------------------------------------
void Brw_CreateTmpPublicLinkToPrivateFile (const char *FullPathIncludingFile,
                                           const char *FileName);

void Brw_PutImplicitParsFileBrowser (void *FilFolLnk);
void Brw_PutParsFileBrowser (const char *PathInTree,const char *FilFolLnkName,
                             Brw_FileType_t FileType,long FilCod);

void Brw_RemoveZonesOfGroupsOfType (long GrpTypCod);
void Brw_RemoveGrpZones (long HieCod,long GrpCod);

void Brw_RemoveUsrWorksInCrs (struct Usr_Data *UsrDat,struct Hie_Node *Crs);
void Brw_RemoveUsrWorksInAllCrss (struct Usr_Data *UsrDat);

void Brw_GetSummaryAndContentOfFile (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                     char **ContentStr,
                                     long FilCod,Ntf_GetContent_t GetContent);

void Brw_ListDocsFound (MYSQL_RES **mysql_res,unsigned NumDocs,
			const char *TitleSingular,const char *TitlePlural);

void Brw_AskRemoveOldFilesBriefcase (void);
void Brw_RemoveOldFilesBriefcase (void);

Act_Action_t Brw_GetActionExpand (void);
Act_Action_t Brw_GetActionContract (void);

//-------------------------------- Figures ------------------------------------
void Brw_GetAndShowFileBrowsersStats (Hie_Level_t HieLvl);
void Brw_GetAndShowOERsStats (Hie_Level_t HieLvl);

#endif
