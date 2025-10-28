// swad_browser_size.c: file browser size

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <dirent.h>		// For scandir, etc.
#include <stdlib.h>		// For free
#include <string.h>		// For string functions
#include <sys/stat.h>		// For lstat

#include "swad_alert.h"
#include "swad_browser_database.h"
#include "swad_browser_size.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

/* All quotas must be multiple of 1 GiB (Gibibyte)*/
#define BrwSiz_GiB (1024ULL * 1024ULL * 1024ULL)

/* Maximum quotas for each type of file browser */
#define BrwSiz_MAX_QUOTA_DOCUM_INS	(64ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_DOCUM_INS	5000
#define BrwSiz_MAX_FOLDS_DOCUM_INS	1000

#define BrwSiz_MAX_QUOTA_DOCUM_CTR	(64ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_DOCUM_CTR	5000
#define BrwSiz_MAX_FOLDS_DOCUM_CTR	1000

#define BrwSiz_MAX_QUOTA_DOCUM_DEG	(64ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_DOCUM_DEG	5000
#define BrwSiz_MAX_FOLDS_DOCUM_DEG	1000

#define BrwSiz_MAX_QUOTA_DOCUM_CRS	(64ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_DOCUM_CRS	5000
#define BrwSiz_MAX_FOLDS_DOCUM_CRS	1000

#define BrwSiz_MAX_QUOTA_DOCUM_GRP	( 1ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_DOCUM_GRP	1000
#define BrwSiz_MAX_FOLDS_DOCUM_GRP	500

#define BrwSiz_MAX_QUOTA_TEACH_CRS	BrwSiz_MAX_QUOTA_DOCUM_CRS
#define BrwSiz_MAX_FILES_TEACH_CRS	BrwSiz_MAX_FILES_DOCUM_CRS
#define BrwSiz_MAX_FOLDS_TEACH_CRS	BrwSiz_MAX_FOLDS_DOCUM_CRS

#define BrwSiz_MAX_QUOTA_TEACH_GRP	BrwSiz_MAX_QUOTA_DOCUM_GRP
#define BrwSiz_MAX_FILES_TEACH_GRP	BrwSiz_MAX_FILES_DOCUM_GRP
#define BrwSiz_MAX_FOLDS_TEACH_GRP	BrwSiz_MAX_FOLDS_DOCUM_GRP

#define BrwSiz_MAX_QUOTA_SHARE_INS	(64ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_SHARE_INS	5000
#define BrwSiz_MAX_FOLDS_SHARE_INS	1000	// Many, because every student can create his own directories

#define BrwSiz_MAX_QUOTA_SHARE_CTR	(64ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_SHARE_CTR	5000
#define BrwSiz_MAX_FOLDS_SHARE_CTR	1000	// Many, because every student can create his own directories

#define BrwSiz_MAX_QUOTA_SHARE_DEG	(64ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_SHARE_DEG	5000
#define BrwSiz_MAX_FOLDS_SHARE_DEG	1000	// Many, because every student can create his own directories

#define BrwSiz_MAX_QUOTA_SHARE_CRS	(64ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_SHARE_CRS	5000
#define BrwSiz_MAX_FOLDS_SHARE_CRS	1000	// Many, because every student can create his own directories

#define BrwSiz_MAX_QUOTA_SHARE_GRP	( 1ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_SHARE_GRP	1000
#define BrwSiz_MAX_FOLDS_SHARE_GRP	500	// Many, because every student can create his own directories

#define BrwSiz_MAX_QUOTA_ASSIG_PER_STD	( 2ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_ASSIG_PER_STD	500
#define BrwSiz_MAX_FOLDS_ASSIG_PER_STD	50

#define BrwSiz_MAX_QUOTA_WORKS_PER_STD	( 2ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_WORKS_PER_STD	500
#define BrwSiz_MAX_FOLDS_WORKS_PER_STD	50

#define BrwSiz_MAX_QUOTA_DOC_PRJ	( 1ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_DOC_PRJ	500
#define BrwSiz_MAX_FOLDS_DOC_PRJ	50

#define BrwSiz_MAX_QUOTA_ASS_PRJ	( 1ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_ASS_PRJ	200
#define BrwSiz_MAX_FOLDS_ASS_PRJ	20

#define BrwSiz_MAX_QUOTA_MARKS_CRS	( 1ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_MARKS_CRS	500
#define BrwSiz_MAX_FOLDS_MARKS_CRS	50

#define BrwSiz_MAX_QUOTA_MARKS_GRP	( 1ULL*BrwSiz_GiB)
#define BrwSiz_MAX_FILES_MARKS_GRP	200
#define BrwSiz_MAX_FOLDS_MARKS_GRP	20

static unsigned long long BrwSiz_MAX_QUOTA_BRIEF[Rol_NUM_ROLES] =	// MaxRole is used
  {
   [Rol_STD] =	32ULL*BrwSiz_GiB,
   [Rol_NET] =	32ULL*BrwSiz_GiB,
   [Rol_TCH] =	64ULL*BrwSiz_GiB,
  };
#define BrwSiz_MAX_FILES_BRIEF	5000
#define BrwSiz_MAX_FOLDS_BRIEF	1000

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

struct BrwSiz_BrowserSize Brw_Size;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void BrwSiz_CalcSizeOfDirRecursive (struct BrwSiz_BrowserSize *Size,
                                           unsigned Level,char *Path);

/*****************************************************************************/
/********************* Get pointer to browser size struct ********************/
/*****************************************************************************/

struct BrwSiz_BrowserSize *BrwSiz_GetSize (void)
  {
   return &Brw_Size;
  }

/*****************************************************************************/
/*** Initialize maximum quota of current file browser and check if exceded ***/
/*****************************************************************************/

void BrwSiz_SetAndCheckQuota (struct BrwSiz_BrowserSize *Size)
  {
   extern const char *Txt_Quota_exceeded;

   /***** Check the quota *****/
   BrwSiz_SetMaxQuota (Size);
   BrwSiz_CalcSizeOfDir (Size,Gbl.FileBrowser.Path.RootFolder);
   if (BrwSiz_CheckQuota (Size) == Err_ERROR)
      Ale_ShowAlert (Ale_WARNING,Txt_Quota_exceeded);
  }

/*****************************************************************************/
/************ Initialize maximum quota of current file browser ***************/
/*****************************************************************************/

void BrwSiz_SetMaxQuota (struct BrwSiz_BrowserSize *Size)
  {
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_SHOW_DOC_INS:
      case Brw_ADMI_DOC_INS:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_DOCUM_INS;
         Size->MaxFiles = BrwSiz_MAX_FILES_DOCUM_INS;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_DOCUM_INS;
         break;
      case Brw_ADMI_SHR_INS:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_SHARE_INS;
         Size->MaxFiles = BrwSiz_MAX_FILES_SHARE_INS;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_SHARE_INS;
	 break;
      case Brw_SHOW_DOC_CTR:
      case Brw_ADMI_DOC_CTR:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_DOCUM_CTR;
         Size->MaxFiles = BrwSiz_MAX_FILES_DOCUM_CTR;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_DOCUM_CTR;
         break;
      case Brw_ADMI_SHR_CTR:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_SHARE_CTR;
         Size->MaxFiles = BrwSiz_MAX_FILES_SHARE_CTR;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_SHARE_CTR;
	 break;
      case Brw_SHOW_DOC_DEG:
      case Brw_ADMI_DOC_DEG:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_DOCUM_DEG;
         Size->MaxFiles = BrwSiz_MAX_FILES_DOCUM_DEG;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_DOCUM_DEG;
         break;
      case Brw_ADMI_SHR_DEG:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_SHARE_DEG;
         Size->MaxFiles = BrwSiz_MAX_FILES_SHARE_DEG;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_SHARE_DEG;
	 break;
      case Brw_SHOW_DOC_CRS:
      case Brw_ADMI_DOC_CRS:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_DOCUM_CRS;
         Size->MaxFiles = BrwSiz_MAX_FILES_DOCUM_CRS;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_DOCUM_CRS;
	 break;
      case Brw_SHOW_DOC_GRP:
      case Brw_ADMI_DOC_GRP:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_DOCUM_GRP;
         Size->MaxFiles = BrwSiz_MAX_FILES_DOCUM_GRP;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_DOCUM_GRP;
	 break;
      case Brw_ADMI_TCH_CRS:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_TEACH_CRS;
         Size->MaxFiles = BrwSiz_MAX_FILES_TEACH_CRS;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_TEACH_CRS;
	 break;
      case Brw_ADMI_TCH_GRP:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_TEACH_GRP;
         Size->MaxFiles = BrwSiz_MAX_FILES_TEACH_GRP;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_TEACH_GRP;
	 break;
      case Brw_ADMI_SHR_CRS:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_SHARE_CRS;
         Size->MaxFiles = BrwSiz_MAX_FILES_SHARE_CRS;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_SHARE_CRS;
	 break;
      case Brw_ADMI_SHR_GRP:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_SHARE_GRP;
         Size->MaxFiles = BrwSiz_MAX_FILES_SHARE_GRP;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_SHARE_GRP;
	 break;
      case Brw_ADMI_ASG_USR:
      case Brw_ADMI_ASG_CRS:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_ASSIG_PER_STD;
         Size->MaxFiles = BrwSiz_MAX_FILES_ASSIG_PER_STD;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_ASSIG_PER_STD;
	 break;
      case Brw_ADMI_WRK_USR:
      case Brw_ADMI_WRK_CRS:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_WORKS_PER_STD;
         Size->MaxFiles = BrwSiz_MAX_FILES_WORKS_PER_STD;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_WORKS_PER_STD;
	 break;
      case Brw_ADMI_DOC_PRJ:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_DOC_PRJ;
         Size->MaxFiles = BrwSiz_MAX_FILES_DOC_PRJ;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_DOC_PRJ;
	 break;
      case Brw_ADMI_ASS_PRJ:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_ASS_PRJ;
         Size->MaxFiles = BrwSiz_MAX_FILES_ASS_PRJ;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_ASS_PRJ;
	 break;
      case Brw_SHOW_MRK_CRS:
      case Brw_ADMI_MRK_CRS:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_MARKS_CRS;
         Size->MaxFiles = BrwSiz_MAX_FILES_MARKS_CRS;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_MARKS_CRS;
	 break;
      case Brw_SHOW_MRK_GRP:
      case Brw_ADMI_MRK_GRP:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_MARKS_GRP;
         Size->MaxFiles = BrwSiz_MAX_FILES_MARKS_GRP;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_MARKS_GRP;
	 break;
      case Brw_ADMI_BRF_USR:
	 Size->MaxQuota = BrwSiz_MAX_QUOTA_BRIEF[Gbl.Usrs.Me.Role.Max];
         Size->MaxFiles = BrwSiz_MAX_FILES_BRIEF;
         Size->MaxFolds = BrwSiz_MAX_FOLDS_BRIEF;
	 break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/********************** Check if quota has been exceeded *********************/
/*****************************************************************************/

Err_SuccessOrError_t BrwSiz_CheckQuota (const struct BrwSiz_BrowserSize *Size)
  {
   return Size->NumLevls > BrwSiz_MAX_DIR_LEVELS ||
          Size->NumFolds > Size->MaxFolds ||
          Size->NumFiles > Size->MaxFiles ||
          Size->TotalSiz > Size->MaxQuota ? Err_ERROR :		// Quota exceeded
        				    Err_SUCCESS;	// Quota not exceeded
  }

/*****************************************************************************/
/********************* Reset the size of a file browser **********************/
/*****************************************************************************/

void BrwSiz_ResetFileBrowserSize (struct BrwSiz_BrowserSize *Size)
  {
   Size->NumLevls = 0;
   Size->NumFolds =
   Size->NumFiles = 0L;
   Size->TotalSiz = 0ULL;
  }

/*****************************************************************************/
/********************** Compute the size of a directory **********************/
/*****************************************************************************/

void BrwSiz_CalcSizeOfDir (struct BrwSiz_BrowserSize *Size,char *Path)
  {
   BrwSiz_ResetFileBrowserSize (Size);
   BrwSiz_CalcSizeOfDirRecursive (Size,1,Path);
  }

/*****************************************************************************/
/**************** Compute the size of a directory recursively ****************/
/*****************************************************************************/

static void BrwSiz_CalcSizeOfDirRecursive (struct BrwSiz_BrowserSize *Size,
                                           unsigned Level,char *Path)
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
	    if (Level > Size->NumLevls)
	       Size->NumLevls++;

	    /* Update counters depending on whether it's a directory or a regular file */
	    snprintf (PathFileRel,sizeof (PathFileRel),"%s/%s",
		      Path,FileList[NumFile]->d_name);
	    if (lstat (PathFileRel,&FileStatus))	// On success ==> 0 is returned
	       Err_ShowErrorAndExit ("Can not get information about a file or folder.");
	    else if (S_ISDIR (FileStatus.st_mode))		// It's a directory
	      {
	       Size->NumFolds++;
	       Size->TotalSiz += (unsigned long long) FileStatus.st_size;
	       BrwSiz_CalcSizeOfDirRecursive (Size,Level + 1,PathFileRel);
	      }
	    else if (S_ISREG (FileStatus.st_mode))		// It's a regular file
	      {
	       Size->NumFiles++;
	       Size->TotalSiz += (unsigned long long) FileStatus.st_size;
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
/**************** Get the size of a file zone from database ******************/
/*****************************************************************************/

void BrwSiz_GetSizeOfFileZone (Hie_Level_t HieLvl,Brw_FileBrowser_t FileBrowser,
                               struct BrwSiz_SizeOfFileZone *SizeOfFileZone)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get the size of a file browser *****/
   /* Query database */
   Brw_DB_GetSizeOfFileBrowser (&mysql_res,HieLvl,FileBrowser);

   /* Get row */
   row = mysql_fetch_row (mysql_res);

   /* Reset default values to zero */
   SizeOfFileZone->NumCrss    =
   SizeOfFileZone->NumUsrs    = 0;
   SizeOfFileZone->MaxLevels  = 0;
   SizeOfFileZone->NumFolders =
   SizeOfFileZone->NumFiles   = 0;
   SizeOfFileZone->Size       = 0;

   /* Get number of courses (row[0]) */
   if (row[0])
      if (sscanf (row[0],"%d",&(SizeOfFileZone->NumCrss)) != 1)
         Err_ShowErrorAndExit ("Error when getting number of courses.");

   /* Get number of groups (row[1]) */
   if (row[1])
      if (sscanf (row[1],"%d",&(SizeOfFileZone->NumGrps)) != 1)
         Err_ShowErrorAndExit ("Error when getting number of groups.");

   /* Get number of users (row[2]) */
   if (row[2])
      if (sscanf (row[2],"%d",&(SizeOfFileZone->NumUsrs)) != 1)
         Err_ShowErrorAndExit ("Error when getting number of users.");

   /* Get maximum number of levels (row[3]) */
   if (row[3])
      if (sscanf (row[3],"%u",&(SizeOfFileZone->MaxLevels)) != 1)
         Err_ShowErrorAndExit ("Error when getting maximum number of levels.");

   /* Get number of folders (row[4]) */
   if (row[4])
      if (sscanf (row[4],"%lu",&(SizeOfFileZone->NumFolders)) != 1)
         Err_ShowErrorAndExit ("Error when getting number of folders.");

   /* Get number of files (row[5]) */
   if (row[5])
      if (sscanf (row[5],"%lu",&(SizeOfFileZone->NumFiles)) != 1)
         Err_ShowErrorAndExit ("Error when getting number of files.");

   /* Get total size (row[6]) */
   if (row[6])
      if (sscanf (row[6],"%llu",&(SizeOfFileZone->Size)) != 1)
         Err_ShowErrorAndExit ("Error when getting toal size.");

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************* Show size of a file browser ***********************/
/*****************************************************************************/

void BrwSiz_ShowAndStoreSizeOfFileBrowser (const struct BrwSiz_BrowserSize *Size)
  {
   extern const char *Txt_level;
   extern const char *Txt_levels;
   extern const char *Txt_folder;
   extern const char *Txt_folders;
   extern const char *Txt_file;
   extern const char *Txt_files;
   extern const char *Txt_of_PART_OF_A_TOTAL;
   char FileSizeStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];

   HTM_DIV_Begin ("class=\"CM DAT_%s\"",The_GetSuffix ());

      if (Brw_CheckIfFileBrowserIsEditable (Gbl.FileBrowser.Type) == Usr_CAN)
	{
	 Fil_WriteFileSizeFull ((double) Size->TotalSiz,FileSizeStr);
	 HTM_UnsignedTxt (Size->NumLevls,Txt_level ,Txt_levels ); HTM_Semicolon (); HTM_SP ();
	 HTM_UnsignedTxt (Size->NumFolds,Txt_folder,Txt_folders); HTM_Semicolon (); HTM_SP ();
	 HTM_UnsignedTxt (Size->NumFiles,Txt_file  ,Txt_files  ); HTM_BR ();
	 HTM_Txt (FileSizeStr);

	 if (Size->MaxQuota)
	   {
	    Fil_WriteFileSizeBrief ((double) Size->MaxQuota,FileSizeStr);
	    HTM_SP ();
	    HTM_OpenParenthesis ();
	       HTM_Double1Decimal (100.0 * ((double) Size->TotalSiz /
					    (double) Size->MaxQuota));
	       HTM_Percent (); HTM_SP ();
	       HTM_Txt (Txt_of_PART_OF_A_TOTAL); HTM_SP (); HTM_Txt (FileSizeStr);
	    HTM_CloseParenthesis ();
	   }

	 Brw_DB_StoreSizeOfFileBrowser (Size);
	}

   HTM_DIV_End ();
  }
