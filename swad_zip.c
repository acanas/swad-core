// swad_zip.c: compress files in file browsers

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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
#include <errno.h>		// For errno
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
#define ZIP_MAX_SIZE_UNCOMPRESSED (1024ULL*ZIP_MiB)

const Act_Action_t ZIP_ActZIPFolder[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   ActUnk,		// Brw_UNKNOWN
   ActZIPSeeDocCrs,	// Brw_SHOW_DOCUM_CRS
   ActUnk,		// Brw_SHOW_MARKS_CRS
   ActZIPAdmDocCrs,	// Brw_ADMI_DOCUM_CRS
   ActZIPShaCrs,	// Brw_ADMI_SHARE_CRS
   ActZIPShaGrp,	// Brw_ADMI_SHARE_GRP
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
   ActZIPShaDeg,	// Brw_ADMI_SHARE_DEG
   ActZIPShaCtr,	// Brw_ADMI_SHARE_CTR
   ActZIPShaIns,	// Brw_ADMI_SHARE_INS
   ActZIPTchCrs,	// Brw_ADMI_TEACH_CRS
   ActZIPTchGrp,	// Brw_ADMI_TEACH_GRP
  };

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

static void ZIP_PutLinkToCreateZIPAsgWrkParams (void);

static void ZIP_CreateTmpDirForCompression (void);
static void ZIP_CreateDirCompressionUsr (struct UsrData *UsrDat);

static void ZIP_CompressFolderIntoZIP (void);
static unsigned long long ZIP_CloneDir (const char *Path,const char *PathClone,const char *PathInTree);
static void ZIP_ShowLinkToDownloadZIP (const char *FileName,const char *URL,
                                       off_t FileSize,unsigned long long UncompressedSize);

/*****************************************************************************/
/*********** Put link to create ZIP file of assignments and works ************/
/*****************************************************************************/

void ZIP_PutLinkToCreateZIPAsgWrk (void)
  {
   extern const char *Txt_Create_ZIP_file;

   Lay_PutContextualLink (ActAdmAsgWrkCrs,ZIP_PutLinkToCreateZIPAsgWrkParams,
                          "download64x64.png",
                          Txt_Create_ZIP_file,Txt_Create_ZIP_file,
		          NULL);
  }

static void ZIP_PutLinkToCreateZIPAsgWrkParams (void)
  {
   Usr_PutHiddenParUsrCodAll (ActAdmAsgWrkCrs,Gbl.Usrs.Select.All);
   if (Gbl.FileBrowser.FullTree)
      Par_PutHiddenParamChar ("FullTree",'Y');
   Par_PutHiddenParamChar ("CreateZIP",'Y');
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
   struct UsrData UsrDat;
   const char *Ptr;
   char StrZip[100+PATH_MAX*2+1];
   char Path[PATH_MAX+1];
   int Result;
   char FileNameZIP[NAME_MAX+1];
   char PathFileZIP[PATH_MAX+1];
   struct stat FileStatus;
   char URLWithSpaces[PATH_MAX+1];
   char URL[PATH_MAX+1];

   /***** Create zip file
	  with the assignments and works
	  of the selected users *****/
   /* Create temporary directory
      for the compression of assignments and works */
   ZIP_CreateTmpDirForCompression ();

   /* Initialize structure with user's data */
   Usr_UsrDataConstructor (&UsrDat);

   /* Create temporary directory for each selected user
      inside the directory used for compression */
   Ptr = Gbl.Usrs.Select.All;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDat.EncryptedUsrCod,
					 Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);


      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))	// Get user's data from database
	 if (Usr_CheckIfUsrBelongsToCrs (UsrDat.UsrCod,
					 Gbl.CurrentCrs.Crs.CrsCod,
					 false))
	    ZIP_CreateDirCompressionUsr (&UsrDat);
     }

   /* Free memory used for user's data */
   Usr_UsrDataDestructor (&UsrDat);

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
      if (lstat (PathFileZIP,&FileStatus))	// On success ==> 0 is returned
	 Lay_ShowErrorAndExit ("Can not get information about a file or folder.");
      else
	{
	 /***** Create URL pointing to ZIP file *****/
	 sprintf (URLWithSpaces,"%s/%s/%s/%s",
		  Cfg_URL_SWAD_PUBLIC,
		  Cfg_FOLDER_FILE_BROWSER_TMP,
		  Gbl.FileBrowser.TmpPubDir,
		  FileNameZIP);
	 Str_CopyStrChangingSpaces (URLWithSpaces,URL,PATH_MAX);	// In HTML, URL must have no spaces

	 /****** Link to download file *****/
	 ZIP_ShowLinkToDownloadZIP (FileNameZIP,URL,FileStatus.st_size,0);
	}
     }
   else
      Lay_ShowErrorAndExit ("Can not compress files into zip file.");

   /***** Remove the directory of compression *****/
   Fil_RemoveTree (Path);
  }

/*****************************************************************************/
/********* Create temporary directory to put the works to compress ***********/
/*****************************************************************************/

static void ZIP_CreateTmpDirForCompression (void)
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

static void ZIP_CreateDirCompressionUsr (struct UsrData *UsrDat)
  {
   char FullNameAndUsrID[(Usr_MAX_BYTES_NAME+1)*3+
                         ID_MAX_LENGTH_USR_ID+1+
                         10+1];
   char PathFolderUsrInsideCrs[PATH_MAX+1];
   char LinkTmpUsr[PATH_MAX+1];
   char Link[PATH_MAX+1];
   unsigned NumTry;
   bool Success;

   /***** Create a link in the tree of compression
          with a name that identifies the owner
          of the assignments and works *****/
   /* Create link name for this user */
   strcpy (FullNameAndUsrID,UsrDat->Surname1);
   if (UsrDat->Surname1[0] &&
       UsrDat->Surname2[0])
      strcat (FullNameAndUsrID,"_");	// Separation between surname 1 and surname 2
   strcat (FullNameAndUsrID,UsrDat->Surname2);
   if ((UsrDat->Surname1[0] ||
	UsrDat->Surname2[0]) &&
       UsrDat->FirstName[0])
      strcat (FullNameAndUsrID,"_");	// Separation between surnames and first name
   strcat (FullNameAndUsrID,UsrDat->FirstName);
   if ((UsrDat->Surname1[0] ||
	UsrDat->Surname2[0] ||
	UsrDat->FirstName[0]) &&
       UsrDat->IDs.Num)
      strcat (FullNameAndUsrID,"-");	// Separation between name and ID
   Str_LimitLengthHTMLStr (FullNameAndUsrID,50);
   if (UsrDat->IDs.Num)	// If this user has at least one ID
      strcat (FullNameAndUsrID,UsrDat->IDs.List[0].ID);	// First user's ID
   Str_ConvertToValidFileName (FullNameAndUsrID);

   /* Create path to folder and link */
   sprintf (PathFolderUsrInsideCrs,"%s/usr/%02u/%ld",
	    Gbl.CurrentCrs.PathPriv,
	    (unsigned) (UsrDat->UsrCod % 100),
	    UsrDat->UsrCod);
   sprintf (LinkTmpUsr,"%s/%s/%s/%s",
	    Cfg_PATH_SWAD_PRIVATE,
	    Cfg_FOLDER_ZIP,
	    Gbl.FileBrowser.ZIP.TmpDir,
	    FullNameAndUsrID);

   /* Try to create a link named LinkTmpUsr to PathFolderUsrInsideCrs */
   if (symlink (PathFolderUsrInsideCrs,LinkTmpUsr) != 0)
     {
      for (Success = false, NumTry = 2;
	   !Success && errno == EEXIST && NumTry<=1000;
	   NumTry++)
	{
	 // Link exists ==> a former user share the same name and ID
	 // (probably a unique user has created two or more accounts)
	 sprintf (Link,"%s-%u",LinkTmpUsr,NumTry);
	 if (symlink (PathFolderUsrInsideCrs,Link) == 0)
	    Success = true;
	}

      if (!Success)
	 Lay_ShowErrorAndExit ("Can not create temporary link for compression.");
     }
  }

/*****************************************************************************/
/***************** Put button to create ZIP file of a folder *****************/
/*****************************************************************************/

void ZIP_PutButtonToDownloadZIPOfAFolder (const char *PathInTree,const char *FileName)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Create_ZIP_file;

   Act_FormStart (ZIP_ActZIPFolder[Gbl.FileBrowser.Type]);
   Brw_PutParamsFileBrowser (ZIP_ActZIPFolder[Gbl.FileBrowser.Type],
                             PathInTree,FileName,
                             Brw_IS_FOLDER,-1L);
   Act_LinkFormSubmit (Txt_Create_ZIP_file,The_ClassForm[Gbl.Prefs.Theme],NULL);
   fprintf (Gbl.F.Out,"<img src=\"%s/download64x64.png\""
	              " alt=\"%s\" title=\"%s\""
	              " class=\"ICO20x20B\" />"
		      "</a>",
	 Gbl.Prefs.IconsURL,
	 Txt_Create_ZIP_file,
	 Txt_Create_ZIP_file);
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
	 if (lstat (PathFileZIP,&FileStatus))	// On success ==> 0 is returned
	    Lay_ShowErrorAndExit ("Can not get information about a file or folder.");
	 else
	   {
	    /***** Create URL pointing to ZIP file *****/
	    sprintf (URLWithSpaces,"%s/%s/%s/%s",
		     Cfg_URL_SWAD_PUBLIC,
		     Cfg_FOLDER_FILE_BROWSER_TMP,
		     Gbl.FileBrowser.TmpPubDir,
		     FileNameZIP);
	    Str_CopyStrChangingSpaces (URLWithSpaces,URL,PATH_MAX);	// In HTML, URL must have no spaces

	    /****** Link to download file *****/
	    ZIP_ShowLinkToDownloadZIP (FileNameZIP,URL,FileStatus.st_size,UncompressedSize);
	   }
	}
      else
	 Lay_ShowErrorAndExit ("Can not compress files into zip file.");
     }

   /***** Remove the directory of compression *****/
   Fil_RemoveTree (PathCompression);
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
   struct dirent **FileList;
   int NumFile;
   int NumFiles;
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
   if ((NumFiles = scandir (Path,&FileList,NULL,alphasort)) >= 0)	// No error
     {
      /***** List files *****/
      for (NumFile = 0;
	   NumFile < NumFiles;
	   NumFile++)
	 if (strcmp (FileList[NumFile]->d_name,".") &&
	     strcmp (FileList[NumFile]->d_name,".."))	// Skip directories "." and ".."
	   {
	    sprintf (PathFileInTree,"%s/%s",
	             PathInTree,FileList[NumFile]->d_name);
	    sprintf (PathFile,"%s/%s",
		     Path,FileList[NumFile]->d_name);
	    sprintf (PathFileClone,"%s/%s",
		     PathClone,FileList[NumFile]->d_name);

	    FileType = Brw_IS_UNKNOWN;
	    if (lstat (PathFile,&FileStatus))	// On success ==> 0 is returned
	       Lay_ShowErrorAndExit ("Can not get information about a file or folder.");
	    else if (S_ISDIR (FileStatus.st_mode))		// It's a directory
	       FileType = Brw_IS_FOLDER;
	    else if (S_ISREG (FileStatus.st_mode))	// It's a regular file
	       FileType = Str_FileIs (FileList[NumFile]->d_name,"url") ? Brw_IS_LINK :	// It's a link (URL inside a .url file)
									 Brw_IS_FILE;	// It's a file

	    Hidden = (SeeDocsZone || SeeMarks) ? Brw_CheckIfFileOrFolderIsSetAsHiddenInDB (FileType,PathFileInTree) :
						 false;

	    if (!Hidden)	// If file/folder is not hidden
	      {
	       if (FileType == Brw_IS_FOLDER)	// It's a directory
		 {
		  FullSize += (unsigned long long) FileStatus.st_size;

		  /***** Create clone of subdirectory *****/
		  if (mkdir (PathFileClone,(mode_t) 0xFFF))
		     Lay_ShowErrorAndExit ("Can not create temporary subfolder for compression.");

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
		  Brw_UpdateMyFileViews (Brw_GetFilCodByPath (PathFileInTree,false));	// Any file, public or not
		 }
	      }
	   }
     }
   else
      Lay_ShowErrorAndExit ("Error while scanning directory.");

   return FullSize;
  }

/*****************************************************************************/
/********************* Show link to download a ZIP file **********************/
/*****************************************************************************/

static void ZIP_ShowLinkToDownloadZIP (const char *FileName,const char *URL,
                                       off_t FileSize,unsigned long long UncompressedSize)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_ZIP_file;
   extern const char *Txt_Download;
   extern const char *Txt_Filename;
   extern const char *Txt_File_size;
   extern const char *Txt_FILE_uncompressed;
   char FileNameShort[NAME_MAX+1];
   char FileSizeStr[Fil_MAX_BYTES_FILE_SIZE_STRING];

   /***** Limit length of the name of the file *****/
   strncpy (FileNameShort,FileName,NAME_MAX);
   FileNameShort[NAME_MAX] = '\0';
   Str_LimitLengthHTMLStr (FileNameShort,50);

   /***** Start frame *****/
   Lay_StartRoundFrameTableShadow (NULL,NULL,NULL,NULL,2);

   /***** Link to download the file *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td colspan=\"2\" class=\"FILENAME CENTER_MIDDLE\">"
                      "<a href=\"%s\" class=\"FILENAME\" title=\"%s\" target=\"_blank\">"
                      "<img src=\"%s/%s32x32/zip32x32.gif\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO40x40\" />"
                      "&nbsp;%s&nbsp;"
		      "<img src=\"%s/download64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO40x40\" />"
		      "</a>"
		      "</td>"
		      "</tr>",
            URL,FileName,
            Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_FILEXT,
            Txt_ZIP_file,
            Txt_ZIP_file,
	    FileNameShort,
	    Gbl.Prefs.IconsURL,
	    Txt_Download,
	    Txt_Download);

   /***** Filename *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s RIGHT_MIDDLE\">"
		      "%s:"
		      "</td>"
		      "<td class=\"DAT LEFT_MIDDLE\">"
		      "<a href=\"%s\" class=\"DAT\" title=\"%s\" target=\"_blank\">%s</a>"
		      "</td>"
		      "</tr>",
	    The_ClassForm[Gbl.Prefs.Theme],Txt_Filename,
	    URL,FileName,FileName);

   /***** Write the file size *****/
   Fil_WriteFileSizeFull ((double) FileSize,FileSizeStr);
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s RIGHT_MIDDLE\">"
		      "%s:"
		      "</td>"
		      "<td class=\"DAT LEFT_MIDDLE\">"
		      "%s",
	    The_ClassForm[Gbl.Prefs.Theme],
	    Txt_File_size,
	    FileSizeStr);
   if (UncompressedSize)
     {
      Fil_WriteFileSizeFull ((double) UncompressedSize,FileSizeStr);
      fprintf (Gbl.F.Out," (%s %s)",
               FileSizeStr,Txt_FILE_uncompressed);
     }
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   /***** End frame *****/
   Lay_EndRoundFrameTable ();
  }
