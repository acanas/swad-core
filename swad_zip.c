// swad_zip.c: compress files in file browsers

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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
#include <linux/limits.h>	// For PATH_MAX
#include <stdlib.h>		// For system...
#include <string.h>		// For strcpy...
#include <sys/stat.h>		// For mkdir...
#include <sys/types.h>		// For mkdir...
#include <unistd.h>		// For chdir...

#include "swad_config.h"
#include "swad_global.h"
#include "swad_file_browser.h"
#include "swad_ID.h"
#include "swad_parameter.h"
#include "swad_string.h"
#include "swad_theme.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal constants ****************************/
/*****************************************************************************/

#define ZIP_MiB (1024ULL*1024ULL)
#define ZIP_MAX_SIZE_UNCOMPRESSED (500ULL*ZIP_MiB)

const Act_Action_t ZIP_ActZIPFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActZIPSeeDocCrs,	// Brw_SHOW_DOCUM_CRS
   ActUnk,		// Brw_SHOW_MARKS_CRS
   ActZIPAdmDocCrs,	// Brw_ADMI_DOCUM_CRS
   ActZIPComCrs,	// Brw_ADMI_SHARE_CRS
   ActZIPComGrp,	// Brw_ADMI_SHARE_GRP
   ActZIPWrkUsr,	// Brw_ADMI_WORKS_USR
   ActZIPWrkCrs,	// Brw_ADMI_WORKS_CRS
   ActZIPAdmMrkCrs,	// Brw_ADMI_MARKS_CRS
   ActZIPBrf,		// Brw_ADMI_BRIEF_USR
   ActZIPSeeDocGrp,	// Brw_SHOW_DOCUM_GRP
   ActZIPAdmDocGrp,	// Brw_ADMI_DOCUM_GRP
   ActUnk,		// Brw_SHOW_MARKS_GRP
   ActZIPAdmMrkGrp,	// Brw_ADMI_MARKS_GRP
   ActZIPAsgUsr,	// Brw_ADMI_ASSIG_USR
   ActZIPAsgCrs,	// Brw_ADMI_ASSIG_CRS
   ActZIPSeeDocDeg,	// Brw_SHOW_DOCUM_DEG
   ActZIPAdmDocDeg,	// Brw_ADMI_DOCUM_DEG
   ActZIPSeeDocCtr,	// Brw_SHOW_DOCUM_CTR
   ActZIPAdmDocCtr,	// Brw_ADMI_DOCUM_CTR
   ActZIPSeeDocIns,	// Brw_SHOW_DOCUM_INS
   ActZIPAdmDocIns,	// Brw_ADMI_DOCUM_INS
   ActZIPComDeg,	// Brw_ADMI_SHARE_DEG
   ActZIPComCtr,	// Brw_ADMI_SHARE_CTR
   ActZIPComIns,	// Brw_ADMI_SHARE_INS
  };

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;
// extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

static void ZIP_CompressFolderIntoZIP (void);
static unsigned long long ZIP_CloneDir (const char *Path,const char *PathClone,const char *PathInTree);
static void ZIP_ShowLinkToDownloadZIP (const char *FileName,const char *URL,
                                       off_t FileSize,unsigned long long UncompressedSize);

/*****************************************************************************/
/********* Put button to create ZIP file of assignments and works ************/
/*****************************************************************************/

void ZIP_PutButtonToCreateZIPAsgWrk (void)
  {
   extern const char *Txt_Create_ZIP_file;

   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Act_FormStart (ActAdmAsgWrkCrs);
   Usr_PutHiddenParUsrCodAll (ActAdmAsgWrkCrs,Gbl.Usrs.Select.All);
   if (Gbl.FileBrowser.FullTree)
      Par_PutHiddenParamChar ("FullTree",'Y');
   Par_PutHiddenParamChar ("CreateZIP",'Y');
   Act_PutContextualLink ("download",Txt_Create_ZIP_file,Txt_Create_ZIP_file,Txt_Create_ZIP_file);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/****************** Get whether to create ZIP file from form *****************/
/*****************************************************************************/

bool ZIP_GetCreateZIPFromForm (void)
  {
   char YN[1+1];

   Par_GetParToText ("CreateZIP",YN,1);
   return (Str_ConvertToUpperLetter (YN[0]) == 'Y');
  }

/*****************************************************************************/
/*************** Create the zip file with assignments and works **************/
/*************** and put a link to download it                  **************/
/*****************************************************************************/

void ZIP_CreateZIPAsgWrk (void)
  {
   extern const char *Txt_works_ZIP_FILE_NAME;
   char StrZip[100+PATH_MAX*2+1];
   char Path[PATH_MAX+1];
   int Result;
   char FileNameZIP[NAME_MAX+1];
   char PathFileZIP[PATH_MAX+1];
   struct stat FileStatus;
   char URLWithSpaces[PATH_MAX+1];
   char URL[PATH_MAX+1];

   /***** Create a temporary public directory
          used to download the zip file *****/
   Brw_CreateDirDownloadTmp ();

   /***** Relative path of the directory with the works to compress *****/
   sprintf (Path,"%s/%s/%s",
	    Cfg_PATH_SWAD_PRIVATE,
	    Cfg_FOLDER_ZIP,
	    Gbl.FileBrowser.ZIP.TmpDir);

   /***** Change to directory of the assignments and works
          in order to start the path in the zip file from there *****/
   if (chdir (Path))
      Lay_ShowErrorAndExit ("Can not change to temporary folder for compression.");

   /***** Create public zip file with the assignment and works *****/
   sprintf (FileNameZIP,"%s.zip",Txt_works_ZIP_FILE_NAME);
   sprintf (PathFileZIP,"%s/%s/%s/%s",
	    Cfg_PATH_SWAD_PUBLIC,
            Cfg_FOLDER_FILE_BROWSER_TMP,
            Gbl.FileBrowser.TmpPubDir,
            FileNameZIP);
   sprintf (StrZip,"nice -n 19 zip -q -r '%s' *",
            PathFileZIP);
   Result = system (StrZip);

   /***** Return to the CGI directory *****/
   if (chdir (Cfg_PATH_CGI_BIN))
      Lay_ShowErrorAndExit ("Can not change to cgi-bin folder.");

   /***** If the zip command has been sucessful, write the link to zip file *****/
   if (Result == 0)
     {
      /***** Get file size *****/
      lstat (PathFileZIP,&FileStatus);

      /***** Create URL pointing to ZIP file *****/
      sprintf (URLWithSpaces,"%s/%s/%s/%s",
	       Cfg_HTTPS_URL_SWAD_PUBLIC,
               Cfg_FOLDER_FILE_BROWSER_TMP,
               Gbl.FileBrowser.TmpPubDir,
               FileNameZIP);
      Str_CopyStrChangingSpaces (URLWithSpaces,URL,PATH_MAX);	// In HTML, URL must have no spaces

      /****** Link to download file *****/
      ZIP_ShowLinkToDownloadZIP (FileNameZIP,URL,FileStatus.st_size,0);
     }
   else
      Lay_ShowErrorAndExit ("Can not compress files into zip file.");

   /***** Remove the directory of compression *****/
   Brw_RemoveTree (Path);
  }

/*****************************************************************************/
/********* Create temporary directory to put the works to compress ***********/
/*****************************************************************************/

void ZIP_CreateTmpDirForCompression (void)
  {
   char PathZipPriv[PATH_MAX+1];
   char PathDirTmp[PATH_MAX+1];

   /***** If the private directory does not exist, create it *****/
   sprintf (PathZipPriv,"%s/%s",Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_ZIP);
   Fil_CreateDirIfNotExists (PathZipPriv);

   /***** First of all, we remove the oldest temporary directories.
          Such temporary directories have been created by me or by other users.
          This is a bit sloppy, but they must be removed by someone.
          Here "oldest" means more than x time from their creation *****/
   Fil_RemoveOldTmpFiles (PathZipPriv,Cfg_TIME_TO_DELETE_BROWSER_ZIP_FILES,false);

   /***** Create a new temporary directory *****/
   strcpy (Gbl.FileBrowser.ZIP.TmpDir,Gbl.UniqueNameEncrypted);
   sprintf (PathDirTmp,"%s/%s",PathZipPriv,Gbl.FileBrowser.ZIP.TmpDir);
   if (mkdir (PathDirTmp,(mode_t) 0xFFF))
      Lay_ShowErrorAndExit ("Can not create temporary folder for compression.");
  }

/*****************************************************************************/
/**************** Create link to a user's works zone        ******************/
/**************** in the temporary directory of compression ******************/
/*****************************************************************************/

void ZIP_CreateDirCompressionUsr (struct UsrData *UsrDat)
  {
   char FullNameAndUsrID[(Usr_MAX_BYTES_NAME+1)*3+ID_MAX_LENGTH_USR_ID+1];
   char PathFolderUsrInsideCrs[PATH_MAX+1];
   char LinkRelTmpUsr[PATH_MAX+1];

   /***** Create a link in the tree of compression
          with a name that identifies the owner
          of the assignments and works *****/
   sprintf (FullNameAndUsrID,"%s%s%s%s%s%s",
	    UsrDat->Surname1 ,
	    UsrDat->Surname1[0]  ? "_" :
		                   "",
	    UsrDat->Surname2 ,
	    UsrDat->Surname2[0]  ? "_" :
		                   "",
   	    UsrDat->FirstName,
   	    UsrDat->FirstName[0] ? "-" :
   		                   "");
   Str_LimitLengthHTMLStr (FullNameAndUsrID,50);
   strcat (FullNameAndUsrID,UsrDat->IDs.List[0].ID);	// TODO: What user's ID from the list?
   Str_ConvertToValidFileName (FullNameAndUsrID);

   sprintf (PathFolderUsrInsideCrs,"%s/usr/%02u/%ld",
	    Gbl.CurrentCrs.PathPriv,
	    (unsigned) (UsrDat->UsrCod % 100),
	    UsrDat->UsrCod);
   sprintf (LinkRelTmpUsr,"%s/%s/%s/%s",
	    Cfg_PATH_SWAD_PRIVATE,
	    Cfg_FOLDER_ZIP,
	    Gbl.FileBrowser.ZIP.TmpDir,
	    FullNameAndUsrID);
   if (symlink (PathFolderUsrInsideCrs,LinkRelTmpUsr) != 0)
      Lay_ShowErrorAndExit ("Can not create temporary folder for compression.");
  }

/*****************************************************************************/
/***************** Put button to create ZIP file of a folder *****************/
/*****************************************************************************/

void ZIP_PutButtonToDownloadZIPOfAFolder (const char *PathInTree,const char *FileName)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Create_ZIP_file;

   Act_FormStart (ZIP_ActZIPFolder[Gbl.FileBrowser.Type]);
   switch (Gbl.FileBrowser.Type)
     {
      case Brw_SHOW_DOCUM_GRP:
      case Brw_ADMI_DOCUM_GRP:
      case Brw_ADMI_SHARE_GRP:
      case Brw_SHOW_MARKS_GRP:
      case Brw_ADMI_MARKS_GRP:
	 Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
	 break;
      case Brw_ADMI_ASSIG_CRS:
      case Brw_ADMI_WORKS_CRS:
	 Usr_PutHiddenParUsrCodAll (ZIP_ActZIPFolder[Gbl.FileBrowser.Type],Gbl.Usrs.Select.All);
	 Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
	 break;
      default:
	 break;
     }
   Brw_ParamListFiles (Brw_IS_FOLDER,PathInTree,FileName);
   Act_LinkFormSubmit (Txt_Create_ZIP_file,The_ClassFormul[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"<img src=\"%s/download16x16.gif\" alt=\"%s\""
	              " title=\"%s\" class=\"ICON16x16B\" />"
		      "</a>",
	 Gbl.Prefs.IconsURL,Txt_Create_ZIP_file,Txt_Create_ZIP_file);
   Act_FormEnd ();
  }

/*****************************************************************************/
/********************* Compress a folder into ZIP file ***********************/
/*****************************************************************************/

void ZIP_CompressFileTree (void)
  {
   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Compress folder into ZIP *****/
   ZIP_CompressFolderIntoZIP ();

   /***** Show again file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/*************** Create the zip file with assignments and works **************/
/*************** and put a link to download it                  **************/
/*****************************************************************************/

static void ZIP_CompressFolderIntoZIP (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_ROOT_FOLDER_EXTERNAL_NAMES[Brw_NUM_TYPES_FILE_BROWSER];
   extern const char *Txt_The_folder_is_empty;
   extern const char *Txt_The_contents_of_the_folder_are_too_big;
   unsigned long long UncompressedSize;
   char StrZip[100+PATH_MAX*2+1];
   char Path[PATH_MAX+1];
   char PathCompression[PATH_MAX+1];
   int Result;
   char FileNameZIP[NAME_MAX+1];
   char PathFileZIP[PATH_MAX+1];
   struct stat FileStatus;
   char URLWithSpaces[PATH_MAX+1];
   char URL[PATH_MAX+1];

   /***** Create temporary private directory
          for the compression of folder *****/
   ZIP_CreateTmpDirForCompression ();

   /***** Create a temporary public directory
          used to download the zip file *****/
   Brw_CreateDirDownloadTmp ();

   /***** Create a copy of the directory to compress *****/
   sprintf (Path,"%s/%s",
	    Gbl.FileBrowser.Priv.PathAboveRootFolder,
	    Gbl.FileBrowser.Priv.FullPathInTree);
   sprintf (PathCompression,"%s/%s/%s",
	    Cfg_PATH_SWAD_PRIVATE,
	    Cfg_FOLDER_ZIP,
	    Gbl.FileBrowser.ZIP.TmpDir);	// Example: /var/www/swad/zip/<temporary_dir>

   UncompressedSize = ZIP_CloneDir (Path,PathCompression,Gbl.FileBrowser.Priv.FullPathInTree);

   if (UncompressedSize == 0)					// Nothing to compress
      Lay_ShowAlert (Lay_WARNING,Txt_The_folder_is_empty);
   else if (UncompressedSize > ZIP_MAX_SIZE_UNCOMPRESSED)	// Uncompressed size is too big
      Lay_ShowAlert (Lay_WARNING,Txt_The_contents_of_the_folder_are_too_big);
   else
     {
      /***** Change to directory of the clone folder
	     in order to start the path in the zip file from there *****/
      if (chdir (PathCompression))
	 Lay_ShowErrorAndExit ("Can not change to temporary folder for compression.");

      /***** Create public zip file with the assignment and works *****/
      sprintf (FileNameZIP,"%s.zip",strcmp (Gbl.FileBrowser.FilFolLnkName,".") ? Gbl.FileBrowser.FilFolLnkName :
										    Txt_ROOT_FOLDER_EXTERNAL_NAMES[Gbl.FileBrowser.Type]);
      sprintf (PathFileZIP,"%s/%s/%s/%s",
	       Cfg_PATH_SWAD_PUBLIC,
	       Cfg_FOLDER_FILE_BROWSER_TMP,
	       Gbl.FileBrowser.TmpPubDir,
	       FileNameZIP);
      sprintf (StrZip,"nice -n 19 zip -q -5 -r '%s' *",
	       PathFileZIP);
      Result = system (StrZip);

      /***** Return to the CGI directory *****/
      if (chdir (Cfg_PATH_CGI_BIN))
	 Lay_ShowErrorAndExit ("Can not change to cgi-bin folder.");

      /***** If the zip command has been sucessful, write the link to zip file *****/
      if (Result == 0)
	{
	 /***** Get file size *****/
	 lstat (PathFileZIP,&FileStatus);

	 /***** Create URL pointing to ZIP file *****/
	 sprintf (URLWithSpaces,"%s/%s/%s/%s",
		  Cfg_HTTPS_URL_SWAD_PUBLIC,
		  Cfg_FOLDER_FILE_BROWSER_TMP,
		  Gbl.FileBrowser.TmpPubDir,
		  FileNameZIP);
	 Str_CopyStrChangingSpaces (URLWithSpaces,URL,PATH_MAX);	// In HTML, URL must have no spaces

	 /****** Link to download file *****/
	 ZIP_ShowLinkToDownloadZIP (FileNameZIP,URL,FileStatus.st_size,UncompressedSize);
	}
      else
	 Lay_ShowErrorAndExit ("Can not compress files into zip file.");
     }

   /***** Remove the directory of compression *****/
   Brw_RemoveTree (PathCompression);
  }

/*****************************************************************************/
/*************** Clone a directory recursively to compress it ****************/
/*****************************************************************************/

/* Example:
 *
 * Example starting directory with document files: /var/www/swad/crs/1000/descarga/lectures/lecture_1
 * We want to compress all files inside lecture_1 into a ZIP file
 * Path = /var/www/swad/crs/1000/descarga/lectures/lecture_1
 * PathClone = /var/www/swad/zip/<unique_dir>
 * PathInTree = "descarga/lectures/lecture_1"

 * Example directory inside starting directory with document files: /var/www/swad/crs/1000/descarga/lectures/lecture_1/slides
 * Path = /var/www/swad/crs/1000/descarga/lectures/lecture_1/slides
 * PathClone: /var/www/swad/zip/<unique_dir>/slides
 * PathInTree = "descarga/lectures/lecture_1/slides
 */
// Return: full size of directory contents

static unsigned long long ZIP_CloneDir (const char *Path,const char *PathClone,const char *PathInTree)
  {
   struct dirent **DirFileList;
   int NumFileInThisDir;
   int NumFilesInThisDir;
   char PathFile[PATH_MAX+1];
   char PathFileClone[PATH_MAX+1];
   char PathFileInTree[PATH_MAX+1];
   struct stat FileStatus;
   Brw_FileType_t FileType;
   bool Hidden;
   bool SeeDocsZone = Gbl.FileBrowser.Type == Brw_SHOW_DOCUM_INS ||
                      Gbl.FileBrowser.Type == Brw_SHOW_DOCUM_CTR ||
                      Gbl.FileBrowser.Type == Brw_SHOW_DOCUM_DEG ||
                      Gbl.FileBrowser.Type == Brw_SHOW_DOCUM_CRS ||
                      Gbl.FileBrowser.Type == Brw_SHOW_DOCUM_GRP;
   bool SeeMarks    = Gbl.FileBrowser.Type == Brw_SHOW_MARKS_CRS ||
                      Gbl.FileBrowser.Type == Brw_SHOW_MARKS_GRP;
   unsigned long long FullSize = 0;

   /***** Scan directory *****/
   NumFilesInThisDir = scandir (Path,&DirFileList,NULL,alphasort);

   /***** List files *****/
   for (NumFileInThisDir = 0;
	NumFileInThisDir < NumFilesInThisDir;
	NumFileInThisDir++)
      if (strcmp (DirFileList[NumFileInThisDir]->d_name,".") &&
          strcmp (DirFileList[NumFileInThisDir]->d_name,".."))	// Skip directories "." and ".."
        {
	 sprintf (PathFileInTree,"%s/%s",PathInTree,DirFileList[NumFileInThisDir]->d_name);
	 sprintf (PathFile,"%s/%s",
		  Path,DirFileList[NumFileInThisDir]->d_name);
	 sprintf (PathFileClone,"%s/%s",
		  PathClone,DirFileList[NumFileInThisDir]->d_name);

	 lstat (PathFile,&FileStatus);
	 if (S_ISDIR (FileStatus.st_mode))		// It's a directory
	    FileType = Brw_IS_FOLDER;
	 else if (S_ISREG (FileStatus.st_mode))	// It's a regular file
	    FileType = Str_FileIs (DirFileList[NumFileInThisDir]->d_name,"url") ? Brw_IS_LINK :	// It's a link (URL inside a .url file)
							                          Brw_IS_FILE;	// It's a file
	 else
	    FileType = Brw_IS_UNKNOWN;

	 Hidden = (SeeDocsZone || SeeMarks) ? Brw_CheckIfFileOrFolderIsSetAsHiddenInDB (FileType,PathFileInTree) :
	                                      false;

         if (!Hidden)	// If file/folder is not hidden
           {
	    if (FileType == Brw_IS_FOLDER)	// It's a directory
	      {
	       FullSize += (unsigned long long) FileStatus.st_size;

	       /***** Create clone of subdirectory *****/
	       if (mkdir (PathFileClone,(mode_t) 0xFFF))
		  Lay_ShowErrorAndExit ("Can not create temporary folder for compression.");

	       /***** Clone subtree starting at this this directory *****/
	       FullSize += ZIP_CloneDir (PathFile,PathFileClone,PathFileInTree);
	      }
	    else if (FileType == Brw_IS_FILE ||
		     FileType == Brw_IS_LINK)	// It's a regular file
	      {
	       FullSize += (unsigned long long) FileStatus.st_size;

	       /***** Create a symbolic link (clone) to original file *****/
	       if (symlink (PathFile,PathFileClone) != 0)
		  Lay_ShowErrorAndExit ("Can not create temporary link for compression.");

	       /***** Update number of my views of this file *****/
	       Brw_UpdateMyFileViews (Brw_GetFilCodByPath (PathFileInTree));
	      }
           }
	}

   return FullSize;
  }

/*****************************************************************************/
/********************* Show link to download a ZIP file **********************/
/*****************************************************************************/

static void ZIP_ShowLinkToDownloadZIP (const char *FileName,const char *URL,
                                       off_t FileSize,unsigned long long UncompressedSize)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_ZIP_file;
   extern const char *Txt_Download;
   extern const char *Txt_Filename;
   extern const char *Txt_File_size;
   extern const char *Txt_FILE_uncompressed;
   char FileNameShort[NAME_MAX+1];

   /***** Limit length of the name of the file *****/
   strncpy (FileNameShort,FileName,NAME_MAX);
   FileNameShort[NAME_MAX] = '\0';
   Str_LimitLengthHTMLStr (FileNameShort,50);

   /***** Start frame *****/
   Lay_StartRoundFrameTable10Shadow (NULL,0);
   fprintf (Gbl.F.Out,"<tr>"
		      "<td style=\"text-align:center;\">"
                      "<table class=\"CELLS_PAD_2\">");

   /***** Link to download the file *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td colspan=\"2\"class=\"FILENAME\""
		      " style=\"text-align:center; vertical-align:middle;\">"
                      "<a href=\"%s\" class=\"FILENAME\" title=\"%s\" target=\"_blank\">"
                      "<img src=\"%s/%s32x32/zip32x32.gif\" alt=\"%s\""
                      " class=\"ICON32x32\" />"
                      "&nbsp;%s&nbsp;"
		      "<img src=\"%s/down32x32.gif\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON32x32\" />"
		      "</a>"
		      "</td>"
		      "</tr>",
            URL,FileName,
            Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_FILEXT,
            Txt_ZIP_file,
	    FileNameShort,
	    Gbl.Prefs.IconsURL,
	    Txt_Download,Txt_Download);

   /***** Filename *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s\" style=\"text-align:right;"
		      " vertical-align:middle;\">"
		      "%s:"
		      "</td>"
		      "<td class=\"DAT\" style=\"text-align:left;"
		      " vertical-align:middle;\">"
		      "<a href=\"%s\" class=\"DAT\" title=\"%s\" target=\"_blank\">%s</a>"
		      "</td>"
		      "</tr>",
	    The_ClassFormul[Gbl.Prefs.Theme],Txt_Filename,
	    URL,FileName,FileName);

   /***** Write the file size *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s\" style=\"text-align:right;"
		      " vertical-align:middle;\">"
		      "%s:"
		      "</td>"
		      "<td class=\"DAT\" style=\"text-align:left;"
		      " vertical-align:middle;\">",
	    The_ClassFormul[Gbl.Prefs.Theme],Txt_File_size);
   Str_WriteSizeInBytesFull ((double) FileSize);
   if (UncompressedSize)
     {
      fprintf (Gbl.F.Out," (");
      Str_WriteSizeInBytesFull ((double) UncompressedSize);
      fprintf (Gbl.F.Out," %s)",Txt_FILE_uncompressed);
     }
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   /***** End frame *****/
   fprintf (Gbl.F.Out,"</table>"
                      "</td>"
		      "</tr>");
   Lay_EndRoundFrameTable10 ();
  }
