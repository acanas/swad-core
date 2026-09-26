// swad_browser.h: file browsers

#ifndef _SWAD_BRW
#define _SWAD_BRW
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include "swad_browser_type.h"
#include "swad_hierarchy_type.h"
#include "swad_notification_type.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//--------------------------------- Timing ------------------------------------
void Tim_StartPartialTiming (void);
long Tim_StopPartialTiming (void);
void Tim_StartGlobalTiming (void);
long Tim_StopGlobalTiming (void);

//--------------------------------- Browser -----------------------------------
void Brw_SetGrpCod (long GrpCod);
long Brw_GetGrpCod (void);

void Brw_GetParAndInitFileBrowser (struct Brw_FileBrowser *FileBrowser);
void Brw_InitializeFileBrowser (struct Brw_FileBrowser *FileBrowser);
Exi_Exist_t Brw_CheckIfExistsFolderAssigmentForAnyUsr (const char *FolderName);
Err_SuccessOrError_t Brw_UpdateFoldersAssigmentsIfExistForAllUsrs (const char *OldFolderName,
								   const char *NewFolderName);
void Brw_RemoveFoldersAssignmentsIfExistForAllUsrs (const char *FolderName);

void Brw_GetSelectedUsrsAndShowWorks (void);
void Brw_ShowFileBrowserOrWorks (void);
void Brw_ShowAgainFileBrowserOrWorks (struct Brw_FileBrowser *FileBrowser);

void Brw_PutParFullTreeIfSelected (void *ShowFullTree);

void Brw_CreateDirDownloadTmp (struct Brw_TmpPubDir *TmpPubDir);

void Brw_AskEditWorksCrs (void);

void Brw_ShowFileBrowserNormal (struct Brw_FileBrowser *FileBrowser);
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
HidVis_HiddenOrVisible_t Brw_CheckIfFileOrFolderIsHidden (const struct Brw_FileBrowser *FileBrowser);
void Brw_ShowFileMetadata (void);

void Brw_GetLinkToDownloadFile (struct Brw_FileBrowser *FileBrowser,char *URL);
void Brw_DownloadFile (void);
void Brw_ChgFileMetadata (void);
void Brw_GetFileMetadataByPath (struct Brw_FileBrowser *FileBrowser);
void Brw_GetFileMetadataByCod (struct Brw_FileMetadata *FileMetadata);
Exi_Exist_t Brw_GetFileTypeSizeAndDate (struct Brw_FileBrowser *FileBrowser);
void Brw_GetAndUpdateFileViews (struct Brw_FileMetadata *FileMetadata);
void Brw_UpdateMyFileViews (long FilCod);

long Brw_GetZoneUsrCodForFileBrowser (Brw_Zone_t Zone);
void Brw_GetCrsGrpFromFileMetadata (Brw_Zone_t Zone,long Cod,
                                    long HieCods[Hie_NUM_LEVELS],long *GrpCod);

Usr_Can_t Brw_CheckIfFileBrowserIsEditable (Brw_Zone_t Zone);

long Brw_GetCodForFileBrowser (Brw_Zone_t Zone);

void Brw_SetFullPathInTree (struct Brw_FilFolLnk *FilFolLnk);

void Brw_PutIconFile (const char *FileName,
		      const char *Class,Frm_PutForm_t PutFormToGo);

//-----------------------------------------------------------------------------
void Brw_CreateTmpPublicLinkToPrivateFile (const struct Brw_TmpPubDir *TmpPubDir,
					   const char *FullPathIncludingFile,
                                           const char *FileName);

void Brw_PutImplicitParsFileBrowser (void *Zone);
void Brw_PutParsFileBrowser (struct Brw_FileBrowser *FileBrowser,
			     const char *PathInTree,const char *FilFolLnkName,
                             Brw_FileType_t FileType,long FilCod);
void Brw_GetParsFilFolLnk (struct Brw_FileBrowser *FileBrowser);

void Brw_RemoveZonesOfGroupsOfType (long GrpTypCod);
void Brw_RemoveGrpZones (long HieCod,long GrpCod);

void Brw_RemoveUsrWorksInCrs (struct Usr_Data *UsrDat,struct Hie_Node *Crs);
void Brw_RemoveUsrWorksInAllCrss (struct Usr_Data *UsrDat);

void Brw_GetSummaryAndContentOfFile (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                     char **ContentStr,
                                     long FilCod,Ntf_GetContent_t GetContent);

void Brw_ListDocsFound (MYSQL_RES **mysql_res,
			unsigned NumDocs,
			const char *TitleSingular,const char *TitlePlural);

void Brw_AskRemoveOldFilesBriefcase (void);
void Brw_RemoveOldFilesBriefcase (void);

Act_Action_t Brw_GetActionExpand (Brw_Zone_t Zone);
Act_Action_t Brw_GetActionContract (Brw_Zone_t Zone);

//-------------------------------- Figures ------------------------------------
void Brw_GetAndShowFileBrowsersStats (Hie_Level_t HieLvl);
void Brw_GetAndShowOERsStats (Hie_Level_t HieLvl);

#endif
