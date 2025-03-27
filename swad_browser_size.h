// swad_browser_size.h: file browser size

#ifndef _SWAD_BRW_SIZ
#define _SWAD_BRW_SIZ
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

#include "swad_browser.h"

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct BrwSiz_SizeOfFileZone
  {
   int NumCrss;	// -1 stands for not aplicable
   int NumGrps;	// -1 stands for not aplicable
   int NumUsrs;	// -1 stands for not aplicable
   unsigned MaxLevels;
   unsigned long NumFolders;
   unsigned long NumFiles;
   unsigned long long int Size;	// Total size in bytes
  };

struct BrwSiz_BrowserSize
  {
   unsigned long MaxFiles;
   unsigned long MaxFolds;
   unsigned long long int MaxQuota;
   unsigned NumLevls;
   unsigned long NumFolds;
   unsigned long NumFiles;
   unsigned long long int TotalSiz;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

struct BrwSiz_BrowserSize *BrwSiz_GetSize (void);

void BrwSiz_SetAndCheckQuota (struct BrwSiz_BrowserSize *Size);
void BrwSiz_SetMaxQuota (struct BrwSiz_BrowserSize *Size);

bool BrwSiz_CheckIfQuotaExceded (const struct BrwSiz_BrowserSize *Size);

void BrwSiz_ResetFileBrowserSize (struct BrwSiz_BrowserSize *Size);
void BrwSiz_CalcSizeOfDir (struct BrwSiz_BrowserSize *Size,char *Path);

void BrwSiz_GetSizeOfFileZone (Hie_Level_t HieLvl,Brw_FileBrowser_t FileBrowser,
                               struct BrwSiz_SizeOfFileZone *SizeOfFileZone);

void BrwSiz_ShowAndStoreSizeOfFileBrowser (const struct BrwSiz_BrowserSize *Size);

#endif
