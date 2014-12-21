// swad_file.h: files

#ifndef _SWAD_FIL
#define _SWAD_FIL
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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

#include <stdio.h>	// For FILE

#include "swad_bool.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Fil_NAME_OF_PARAM_FILENAME_ORG "file"

// Maximum allowed size (in bytes) of a file when uploading it.
// Must be < 2 GiB, because off_t type is of type long int
#define Fil_MAX_FILE_SIZE (1536ULL*1024ULL*1024ULL)	// 1.5 GiB

// Global files
struct Files
  {
   FILE *Out;		// File with the HTML output of this CGI
   FILE *Tmp;		// Temporary file to save stdin
   FILE *XML;		// XML file for syllabus, for directory tree
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Fil_CreateFileForHTMLOutput (void);
void Fil_CloseAndRemoveFileForHTMLOutput (void);
bool Fil_ReadStdinIntoTmpFile (void);
void Fil_EndOfReadingStdin (void);
void Fil_StartReceptionOfFile (char *SrcFileName,char *MIMEType);
bool Fil_EndReceptionOfFile (char *FileNameDataTmp);
void Fil_CreateUpdateFile  (const char *CurrentName,const char *ExtensionOldName,char *OldName,char *NewName,FILE **NewFile);
void Fil_CloseUpdateFile (const char *CurrentName,const char *OldName,const char *NewName,FILE *NewFile);
bool Fil_RenameFileOrDir (const char *PathOld,const char *PathNew);
bool Fil_CheckIfPathExists (const char *Path);
void Fil_CreateDirIfNotExists (const char *Path);
void Fil_RemoveOldTmpFiles (const char *Path,time_t TimeToRemove,bool RemoveDirectory);
void Fil_FastCopyOfFiles (const char *PathSrc,const char *PathTgt);
void Fil_FastCopyOfOpenFiles (FILE *FileSrc,FILE *FileTgt);
void Fil_CloseXMLFile (void);

#endif
