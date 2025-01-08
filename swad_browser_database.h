// swad_browser_database.h: file browsers operations with database

#ifndef _SWAD_BROWSER_DATABASE
#define _SWAD_BROWSER_DATABASE
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_browser.h"
#include "swad_browser_size.h"
#include "swad_contracted_expanded.h"
#include "swad_search.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//---------------------------------- Files ------------------------------------
long Brw_DB_AddPath (long PublisherUsrCod,Brw_FileType_t FileType,
                     const char *FullPathInTree,bool IsPublic,Brw_License_t License);
void Brw_DB_RenameOneFolder (const char OldPath[PATH_MAX + 1],
                             const char NewPath[PATH_MAX + 1]);
void Brw_DB_RenameChildrenFilesOrFolders (const char OldPath[PATH_MAX + 1],
                                          const char NewPath[PATH_MAX + 1]);
long Brw_DB_GetFilCodByPath (const char *Path,bool OnlyIfPublic);
unsigned Brw_DB_GetFileMetadataByPath (MYSQL_RES **mysql_res,const char *Path);
unsigned Brw_DB_GetFileMetadataByCod (MYSQL_RES **mysql_res,long FilCod);
void Brw_DB_GetPathByCod (long FilCod,char *Title,size_t TitleSize);
long Brw_DB_GetPublisherOfSubtree (const char *Path);
unsigned Brw_DB_GetNumFilesUsr (long UsrCod);
unsigned Brw_DB_GetNumFilesInDocumZonesOfCrs (long CrsCod);
unsigned Brw_DB_GetNumFilesInShareZonesOfCrs (long CrsCod);
unsigned Brw_DB_GetNumFilesInAssigZonesOfCrs (long CrsCod);
unsigned Brw_DB_GetNumFilesInWorksZonesOfCrs (long CrsCod);
void Brw_DB_RemoveOneFileOrFolder (const char Path[PATH_MAX + 1]);
void Brw_DB_RemoveChildrenOfFolder (const char Path[PATH_MAX + 1]);
void Brw_DB_RemoveInsFiles (long InsCod);
void Brw_DB_RemoveCtrFiles (long CtrCod);
void Brw_DB_RemoveDegFiles (long DegCod);
void Brw_DB_RemoveCrsFiles (long CrsCod);
void Brw_DB_RemoveGrpFiles (long GrpCod);
void Brw_DB_RemovePrjFiles (long PrjCod);
void Brw_DB_RemoveSomeInfoAboutCrsUsrFiles (long UsrCod,long CrsCod);
void Brw_DB_RemoveWrkFiles (long CrsCod,long UsrCod);
void Brw_DB_RemoveUsrFiles (long UsrCod);

//------------------------------ Public files ---------------------------------
void Brw_DB_ChangeFilePublic (const struct Brw_FileMetadata *FileMetadata,
                              bool IsPublic,Brw_License_t License);
bool Brw_DB_GetIfFolderHasPublicFiles (const char Path[PATH_MAX + 1]);
unsigned Brw_DB_GetNumPublicFilesUsr (long UsrCod);
unsigned Brw_DB_GetNumberOfPublicFiles (MYSQL_RES **mysql_res,Brw_License_t License);
unsigned Brw_DB_SearchPublicFiles (MYSQL_RES **mysql_res,
                                   const char *RangeQuery,
                                   const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1]);
unsigned Brw_DB_SearchFilesInMyCrss (MYSQL_RES **mysql_res,
                                     const char *RangeQuery,
                                     const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1]);
unsigned Brw_DB_SearchMyFiles (MYSQL_RES **mysql_res,
                               const char *RangeQuery,
                               const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1]);

//------------------------------- Assignments ---------------------------------
unsigned Brw_DB_GetFoldersAssignments (MYSQL_RES **mysql_res,long ZoneUsrCod);

//--------------------- My last access to file browsers -----------------------
void Brw_DB_UpdateDateMyLastAccFileBrowser (void);
unsigned Brw_DB_GetDateMyLastAccFileBrowser (MYSQL_RES **mysql_res);
unsigned Brw_DB_GetGrpLastAccFileBrowser (MYSQL_RES **mysql_res,const char *FieldNameDB);

//-------------------------------- File views ---------------------------------
void Brw_DB_UpdateFileViews (unsigned NumViews,long FilCod);
unsigned Brw_DB_GetFileViewsFromMe (MYSQL_RES **mysql_res,long FilCod);
unsigned Brw_DB_GetFileViewsFromLoggedUsrs (MYSQL_RES **mysql_res,long FilCod);
unsigned Brw_DB_GetFileViewsFromNonLoggedUsrs (long FilCod);
unsigned Brw_DB_GetNumFileViewsUsr (long UsrCod);

//------------------------------- Hidden files --------------------------------
void Brw_DB_HideOrUnhideFileOrFolder (const char Path[PATH_MAX + 1],
				      HidVis_HiddenOrVisible_t HiddenOrVisible);
HidVis_HiddenOrVisible_t Brw_DB_CheckIfFileOrFolderIsHiddenUsingPath (MYSQL_RES **mysql_res,
								      const char *Path);
HidVis_HiddenOrVisible_t Brw_DB_CheckIfFileOrFolderIsHiddenOrVisibleUsingMetadata (const struct Brw_FileMetadata *FileMetadata);

//---------------------------- Expanded folders -------------------------------
void Brw_DB_InsertFolderInExpandedFolders (const char Path[PATH_MAX + 1]);
void Brw_DB_UpdateClickTimeOfThisFileBrowserInExpandedFolders (void);
ConExp_ContractedOrExpanded_t Brw_DB_GetIfContractedOrExpandedFolder (const char Path[PATH_MAX + 1]);
void Brw_DB_RemoveFolderFromExpandedFolders (const char Path[PATH_MAX + 1]);
void Brw_DB_RemoveAffectedExpandedFolders (const char Path[PATH_MAX + 1]);
void Brw_DB_RenameAffectedExpandedFolders (Brw_FileBrowser_t FileBrowser,
                                           long MyUsrCod,long WorksUsrCod,
                                           const char *OldPath,const char *NewPath);

//------------------------------- Cliboards -----------------------------------
void Brw_DB_AddPathToClipboards (void);
void Brw_DB_UpdatePathInClipboard (void);
unsigned Brw_DB_GetMyClipboard (MYSQL_RES **mysql_res);
void Brw_DB_RemoveExpiredClipboards (void);
void Brw_DB_RemoveAffectedClipboards (Brw_FileBrowser_t FileBrowser,
                                      long MyUsrCod,long WorksUsrCod);

//-------------------------- Size of file zones -------------------------------
void Brw_DB_StoreSizeOfFileBrowser (const struct BrwSiz_BrowserSize *Size);
void Brw_DB_GetSizeOfFileBrowser (MYSQL_RES **mysql_res,
			          Brw_FileBrowser_t FileBrowser);

#endif
