// swad_file.c: files

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <ctype.h>		// For isprint, isspace, etc.
#include <dirent.h>		// For scandir, etc.
#include <errno.h>		// For errno
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For FILE,fprintf
#include <stdlib.h>		// For exit, system, free, etc.
#include <string.h>		// For string functions
#include <sys/stat.h>		// For mkdir
#include <sys/types.h>		// For mkdir
#include <unistd.h>		// For unlink

#include "swad_config.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_file.h"
#include "swad_file_database.h"
#include "swad_parameter.h"
#include "swad_string.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Fil_NUM_BYTES_PER_CHUNK 4096

static struct
  {
   char FileName[PATH_MAX + 1];
  } Fil_HTMLOutput;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

static FILE *Fil_QueryFile = NULL;	// Temporary file to save stdin
static FILE *Fil_Out = NULL;		// Temporary file to save output to be written to stdout

/*****************************************************************************/
/***************************** Get query file ********************************/
/*****************************************************************************/

FILE *Fil_GetQueryFile (void)
  {
   return Fil_QueryFile;
  }

/*****************************************************************************/
/*************************** Set/Get output file *****************************/
/*****************************************************************************/

void Fil_SetOutputFileToStdout (void)
  {
   Fil_Out = stdout;
  }

FILE *Fil_GetOutputFile (void)
  {
   return Fil_Out;
  }

/*****************************************************************************/
/******** Create HTML output file for the web page sent by this CGI **********/
/*****************************************************************************/

void Fil_CreateFileForHTMLOutput (void)
  {
   /***** Check if exists the directory for HTML output. If not exists, create it *****/
   Fil_CreateDirIfNotExists (Cfg_PATH_OUT_PRIVATE);

   /***** Create a unique name for the file *****/
   snprintf (Fil_HTMLOutput.FileName,sizeof (Fil_HTMLOutput.FileName),
             "%s/%s.html",Cfg_PATH_OUT_PRIVATE,Cry_GetUniqueNameEncrypted ());

   /***** Open file for writing and reading *****/
   if ((Fil_Out = fopen (Fil_HTMLOutput.FileName,"w+t")) == NULL)
     {
      Fil_SetOutputFileToStdout ();
      Err_ShowErrorAndExit ("Can not create output file.");
     }
  }

/*****************************************************************************/
/****************** Close and remove the HTML output file ********************/
/*****************************************************************************/

void Fil_CloseAndRemoveFileForHTMLOutput (void)
  {
   if (Fil_Out)
     {
      fclose (Fil_Out);
      unlink (Fil_HTMLOutput.FileName);
     }
   Fil_SetOutputFileToStdout ();
  }

/*****************************************************************************/
/********** Open temporary file and write on it reading from stdin ***********/
/*****************************************************************************/

Err_SuccessOrError_t Fil_ReadStdinIntoTmpFile (void)
  {
   extern const char *Txt_UPLOAD_FILE_File_too_large_maximum_X_MiB_NO_HTML;
   extern const char *Txt_UPLOAD_FILE_Upload_time_too_long_maximum_X_minutes_NO_HTML;
   unsigned long long TmpFileSize;
   bool FileIsTooBig = false;
   bool TimeExceeded = false;

   if ((Fil_QueryFile = tmpfile ()) == NULL)
     {
      Fil_EndOfReadingStdin ();
      Err_ShowErrorAndExit ("Can not create temporary file.");
     }
   for (TmpFileSize = 0;
	!feof (stdin) && !FileIsTooBig && !TimeExceeded;
	TmpFileSize++)
      if (TmpFileSize < Fil_MAX_FILE_SIZE)
        {
         if (!(TmpFileSize % (64ULL * 1024ULL)))	// Check timeout from time to time
            if (time (NULL) - Dat_GetStartExecutionTimeUTC () >= Cfg_TIME_TO_ABORT_FILE_UPLOAD)
               TimeExceeded = true;
         fputc (fgetc (stdin),Fil_QueryFile);
        }
      else
         FileIsTooBig = true;
   if (FileIsTooBig || TimeExceeded)
     {
      Fil_EndOfReadingStdin ();  // If stdin were not fully read, there will be problems with buffers

      /* Begin HTTP response */
      fprintf (stdout,"Content-type: text/plain; charset=windows-1252\n");

      /* Status code and message */
      fprintf (stdout,"Status: 501 Not Implemented\r\n\r\n");
      if (FileIsTooBig)
         fprintf (stdout,Txt_UPLOAD_FILE_File_too_large_maximum_X_MiB_NO_HTML,
                  (unsigned long) (Fil_MAX_FILE_SIZE / (1024ULL * 1024ULL)));
      else
         fprintf (stdout,Txt_UPLOAD_FILE_Upload_time_too_long_maximum_X_minutes_NO_HTML,
                  (unsigned long) (Cfg_TIME_TO_ABORT_FILE_UPLOAD / 60UL));
      fprintf (stdout,"\n");

      /* Don't write HTML at all */
      Gbl.Layout.HTMLStartWritten =
      Gbl.Layout.DivsEndWritten   =
      Gbl.Layout.HTMLEndWritten   = true;

      return Err_ERROR;
     }
   rewind (Fil_QueryFile);

   return Err_SUCCESS;
  }

/*****************************************************************************/
/************ End the reading of all characters coming from stdin ************/
/*****************************************************************************/

void Fil_EndOfReadingStdin (void)
  {
   while (!feof (stdin))
      fgetc (stdin);
  }

/*****************************************************************************/
/********         Start the reception of data from a file            *********/
/******** once the rest of parameters from the form have been readed *********/
/*****************************************************************************/
/*
-----------------------------7d113610948
Content-Disposition: form-data; name="Param1"

2000-2001
-----------------------------7d113610948
Content-Disposition: form-data; name="Param2"

Estructura de los computadores II
-----------------------------7d113610948
Content-Disposition: form-data; name="Param3"

../swad/fichas.htm
-----------------------------7d113610948
Content-Disposition: form-data; name="Param4"

../public_html/docencia/ecii/ecii-log.html
-----------------------------7d113610948
Content-Disposition: form-data; name="Param5"

L384261
-----------------------------7d113610948
Content-Disposition: form-data; name="Param6"

13282
-----------------------------7d113610948
Content-Disposition: form-data; name="Param7"

/~acanas/docencia/fotos
-----------------------------7d113610948
Content-Disposition: form-data; name="Archivo"; filename="D:\Usr\Antonio\Docencia\Estruct\Notas\Notas 2000-2001.ec\fotos\R157550.jpg"
Content-Type: image/pjpeg

000000  FFD8FFE0 00104A46 49460001 01010048  ÿØÿà··JFIF·····H
000010  00480000 FFDB0043 00030202 03020203  ·H··ÿÛ·C········
000020  03030304 03030405 08050504 04050A07  ················
etc, etc.
*/
struct Par_Param *Fil_StartReceptionOfFile (const char *ParFile,
                                            char *FileName,char *MIMEType)
  {
   struct Par_Param *Par;
   FILE *QueryFile = Fil_GetQueryFile ();

   /***** Set default values *****/
   FileName[0] = 0;

   /***** Get parameter *****/
   Par_GetPar (Par_PARAM_SINGLE,ParFile,NULL,Fil_MAX_FILE_SIZE,&Par);

   /***** Get filename *****/
   /* Check if filename exists */
   if (Par->FileName.Start == 0 ||
       Par->FileName.Length == 0)
     {
      FileName[0] = MIMEType[0] = '\0';
      return Par;
     }
   if (Par->FileName.Length > PATH_MAX)
      Err_ShowErrorAndExit ("Error while getting filename.");

   /* Copy filename */
   fseek (QueryFile,Par->FileName.Start,SEEK_SET);
   if (fread (FileName,sizeof (char),Par->FileName.Length,QueryFile) !=
       Par->FileName.Length)
      Err_ShowErrorAndExit ("Error while getting filename.");
   FileName[Par->FileName.Length] = '\0';

   /***** Get MIME type *****/
   /* Check if MIME type exists */
   if (Par->ContentType.Start == 0 ||
       Par->ContentType.Length == 0 ||
       Par->ContentType.Length > Brw_MAX_BYTES_MIME_TYPE)
      Err_ShowErrorAndExit ("Error while getting content type.");

   /* Copy MIME type */
   fseek (QueryFile,Par->ContentType.Start,SEEK_SET);
   if (fread (MIMEType,sizeof (char),Par->ContentType.Length,QueryFile) !=
       Par->ContentType.Length)
      Err_ShowErrorAndExit ("Error while getting content type.");
   MIMEType[Par->ContentType.Length] = '\0';

   return Par;
  }

/*****************************************************************************/
/****************** End the reception of data of a file **********************/
/*****************************************************************************/

Err_SuccessOrError_t Fil_EndReceptionOfFile (char *FileNameDataTmp,
					     struct Par_Param *Param)
  {
   FILE *FileDataTmp;
   unsigned char Bytes[Fil_NUM_BYTES_PER_CHUNK];
   size_t RemainingBytesToCopy;
   size_t BytesToCopy;
   FILE *QueryFile = Fil_GetQueryFile ();

   /***** Open destination file *****/
   if ((FileDataTmp = fopen (FileNameDataTmp,"wb")) == NULL)
      Err_ShowErrorAndExit ("Can not open temporary file.");

   /***** Copy file *****/
   /* Go to start of source */
   if (Param->Value.Start == 0)
      Err_ShowErrorAndExit ("Error while copying file.");
   fseek (QueryFile,Param->Value.Start,SEEK_SET);

   /* Copy part of query file to FileDataTmp */
   for (RemainingBytesToCopy  = Param->Value.Length;
	RemainingBytesToCopy;
	RemainingBytesToCopy -= BytesToCopy)
     {
      BytesToCopy = (RemainingBytesToCopy >= Fil_NUM_BYTES_PER_CHUNK) ? Fil_NUM_BYTES_PER_CHUNK :
	                                                                RemainingBytesToCopy;
      if (fread (Bytes,1,BytesToCopy,QueryFile) != BytesToCopy)
	{
         fclose (FileDataTmp);
	 return Err_ERROR;
	}
      if (fwrite (Bytes,sizeof (Bytes[0]),BytesToCopy,FileDataTmp) != BytesToCopy)
	{
         fclose (FileDataTmp);
	 return Err_ERROR;
	}
     }

   /***** Close destination file *****/
   fclose (FileDataTmp);

   return Err_SUCCESS;
  }

/*****************************************************************************/
/************* Create names and new file to update a existing file ***********/
/*****************************************************************************/
// CurrentName does not change
// OldName is created
// NewName is created

void Fil_CreateUpdateFile (const char CurrentName[PATH_MAX + 1],
                           const char *ExtensionOldName,
                           char OldName[PATH_MAX + 1],
                           char NewName[PATH_MAX + 1],
                           FILE **NewFile)
  {
   size_t LengthFileRoot = Str_GetLengthRootFileName (CurrentName);
   char ErrorMsg[128 + PATH_MAX];

   Str_Copy (NewName,CurrentName,PATH_MAX);
   NewName[LengthFileRoot] = '\0';
   snprintf (OldName,PATH_MAX + 1,"%s%s",NewName,ExtensionOldName);
   Str_Concat (NewName,".new",PATH_MAX);

   /* The new file shouldn't exist. If it exists is due to any error when running this CGI formerly
      and the file was not renamed successfully. In this case, remove it! */
   if (Fil_CheckIfPathExists (NewName) == Exi_EXISTS)
      unlink (NewName);

   /* Open the new file */
   if ((*NewFile = fopen (NewName,"wb")) == NULL)
     {
      snprintf (ErrorMsg,sizeof (ErrorMsg),
                "Can not create file <strong>%s</strong>.",NewName);
      Err_ShowErrorAndExit (ErrorMsg);
     }
  }

/*****************************************************************************/
/****************** Close and rename files related to an update **************/
/*****************************************************************************/

void Fil_CloseUpdateFile (const char CurrentName[PATH_MAX + 1],
                          const char OldName[PATH_MAX + 1],
			  const char NewName[PATH_MAX + 1],
			  FILE *NewFile)
  {
   char ErrorMsg[128 + 2 * PATH_MAX];

   /* Close file */
   fclose (NewFile);

   /* Rename the old file and the new file */
   if (rename (CurrentName,OldName)) // mv CurrentName OldName Ej: mv file.html file.old
     {
      snprintf (ErrorMsg,sizeof (ErrorMsg),
	        "Can not rename the file <strong>%s</strong> as <strong>%s</strong>.",
                CurrentName,OldName);
      Err_ShowErrorAndExit (ErrorMsg);
     }
   if (rename (NewName,CurrentName)) // mv NewName CurrentName Ej: mv file.new file.html
     {
      snprintf (ErrorMsg,sizeof (ErrorMsg),
	        "Can not rename the file <strong>%s</strong> as <strong>%s</strong>.",
                NewName,CurrentName);
      Err_ShowErrorAndExit (ErrorMsg);
     }
  }

/*****************************************************************************/
/***************** Check if existe a file or directory ***********************/
/*****************************************************************************/

Exi_Exist_t Fil_CheckIfPathExists (const char *Path)
  {
   /* access (): On success (all requested permissions granted, or mode is F_OK and
      the file exists), zero is returned.  On error (at least one bit in
      mode asked for a permission that is denied, or mode is F_OK and
      the file does not exist, or some other error occurred), -1 is
      returned, and errno is set to indicate the error. */
   // Important: access with a link returns
   // if exists the file pointed by the link, not the link itself
   return access (Path,F_OK) ? Exi_DOES_NOT_EXIST :
	                       Exi_EXISTS;
  }

/*****************************************************************************/
/********** Check if a directory exists. If not exists, create it! ***********/
/*****************************************************************************/

void Fil_CreateDirIfNotExists (const char *Path)
  {
   char ErrorMsg[128 + PATH_MAX];

   if (Fil_CheckIfPathExists (Path) == Exi_DOES_NOT_EXIST)
      if (mkdir (Path,(mode_t) 0xFFF))
        {
	 snprintf (ErrorMsg,sizeof (ErrorMsg),
	           "Can not create folder <strong>%s</strong>.",Path);
	 Err_ShowErrorAndExit (ErrorMsg);
        }
  }

/*****************************************************************************/
/************************ Remove a directory recursively *********************/
/*****************************************************************************/
// If the tree of directories and files exists, remove it

void Fil_RemoveTree (const char *Path)
  {
   struct stat FileStatus;
   struct dirent **FileList;
   int NumFile,NumFiles;
   char PathFileRel[PATH_MAX + 1];
   Err_SuccessOrError_t SuccessOrError;
   char ErrorMsg[128 + PATH_MAX];

   if (Fil_CheckIfPathExists (Path) == Exi_EXISTS)
     {
      if (lstat (Path,&FileStatus))	// On success ==> 0 is returned
	 Err_ShowErrorAndExit ("Can not get information about a file or folder.");
      else if (S_ISDIR (FileStatus.st_mode))		// It's a directory
	{
	 if (rmdir (Path))
	   {
	    SuccessOrError = Err_SUCCESS;
	    if (errno == ENOTEMPTY)
	      {
	       /***** Remove each directory and file under this directory *****/
	       /* Scan the directory */
	       if ((NumFiles = scandir (Path,&FileList,NULL,NULL)) >= 0)
		 {
		  /* Remove recursively all directories and files */
		  for (NumFile = 0;
		       NumFile < NumFiles;
		       NumFile++)
		    {
		     if (strcmp (FileList[NumFile]->d_name,".") &&
			 strcmp (FileList[NumFile]->d_name,".."))	// Skip directories "." and ".."
		       {
			snprintf (PathFileRel,sizeof (PathFileRel),"%s/%s",
				  Path,FileList[NumFile]->d_name);
			Fil_RemoveTree (PathFileRel);
		       }
		     free (FileList[NumFile]);
		    }
		  free (FileList);
		 }
	       else
		  Err_ShowErrorAndExit ("Error while scanning directory.");

	       /***** Remove the directory, now empty *****/
	       if (rmdir (Path))
		  SuccessOrError = Err_ERROR;
	      }
	    else
	       SuccessOrError = Err_ERROR;
	    if (SuccessOrError == Err_ERROR)
	      {
	       snprintf (ErrorMsg,sizeof (ErrorMsg),
	                 "Can not remove folder %s.",Path);
	       Err_ShowErrorAndExit (ErrorMsg);
	      }
	   }
	}
      else					// It's a file
	 if (unlink (Path))
	    Err_ShowErrorAndExit ("Can not remove file.");
     }
  }

/*****************************************************************************/
/********************* Remove old temporary directories **********************/
/*****************************************************************************/

void Fil_RemoveOldTmpFiles (const char *Path,time_t TimeToRemove,
                            bool RemoveDirectory)
  {
   struct dirent **FileList;
   int NumFile;
   int NumFiles;
   char Path2[PATH_MAX + 1];
   struct stat FileStatus;

   /***** Check this path (file or directory)
          because it could have already been deleted *****/
   // Important: don't use access here to check if path exists
   // because access with a link returns if exists
   // the file pointed by the link, not the link itself
   if (!lstat (Path,&FileStatus))		// On success ==> 0 is returned
     {
      if (S_ISDIR (FileStatus.st_mode))		// It's a directory
	{
	 /***** Scan the directory and delete recursively *****/
	 if ((NumFiles = scandir (Path,&FileList,NULL,NULL)) >= 0)	// No error
	   {
	    /* Loop over files */
	    for (NumFile = 0;
		 NumFile < NumFiles;
		 NumFile++)
	      {
	       if (strcmp (FileList[NumFile]->d_name,".") &&
		   strcmp (FileList[NumFile]->d_name,".."))		// Skip directories "." and ".."
		 {
		  snprintf (Path2,sizeof (Path2),"%s/%s",
			    Path,FileList[NumFile]->d_name);
		  Fil_RemoveOldTmpFiles (Path2,TimeToRemove,true);	// Recursive call
		 }
	       free (FileList[NumFile]);
	      }
	    free (FileList);

	    if (RemoveDirectory)
	       /* Remove the directory itself */
	       if (FileStatus.st_mtime < Dat_GetStartExecutionTimeUTC () - TimeToRemove)
		  rmdir (Path);
	   }
	 else
	    Err_ShowErrorAndExit ("Error while scanning directory.");
	}
      else					// Not a directory
	 if (FileStatus.st_mtime < Dat_GetStartExecutionTimeUTC () - TimeToRemove)
	    unlink (Path);
     }
  }

/*****************************************************************************/
/**************************** Fast copy of files *****************************/
/*****************************************************************************/

void Fil_FastCopyOfFiles (const char *PathSrc,const char *PathTgt)
  {
   FILE *FileSrc;
   FILE *FileTgt;

   /***** Open source file *****/
   if ((FileSrc = fopen (PathSrc,"rb")) == NULL)
      Err_ShowErrorAndExit ("Can not open source file.");

   /***** Open destination file *****/
   if ((FileTgt = fopen (PathTgt,"wb")) == NULL)
      Err_ShowErrorAndExit ("Can not open target file.");

   /***** Copy source file into destination file *****/
   Fil_FastCopyOfOpenFiles (FileSrc,FileTgt);

   /***** Close the files *****/
   fclose (FileTgt);
   fclose (FileSrc);
  }

/*****************************************************************************/
/************************* Fast copy of open files ***************************/
/*****************************************************************************/

void Fil_FastCopyOfOpenFiles (FILE *FileSrc,FILE *FileTgt)
  {
   unsigned char Bytes[Fil_NUM_BYTES_PER_CHUNK];
   size_t NumBytesRead;

   while ((NumBytesRead = fread (Bytes,sizeof (Bytes[0]),
                                 (size_t) Fil_NUM_BYTES_PER_CHUNK,FileSrc)))
      fwrite (Bytes,sizeof (Bytes[0]),NumBytesRead,FileTgt);
  }

/*****************************************************************************/
/********** Write a quantity of bytes as bytes, KiB, MiB, GiB or TiB *********/
/*****************************************************************************/

#define Ki 1024.0
#define Mi 1048576.0
#define Gi 1073741824.0
#define Ti 1099511627776.0

void Fil_WriteFileSizeBrief (double SizeInBytes,
                             char FileSizeStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1])
  {
   if (SizeInBytes < Ki)
      snprintf (FileSizeStr,Fil_MAX_BYTES_FILE_SIZE_STRING + 1,"%.0f&nbsp;B",
		SizeInBytes);
   else if (SizeInBytes < Mi)
      snprintf (FileSizeStr,Fil_MAX_BYTES_FILE_SIZE_STRING + 1,"%.0f&nbsp;KiB",
		SizeInBytes / Ki);
   else if (SizeInBytes < Gi)
      snprintf (FileSizeStr,Fil_MAX_BYTES_FILE_SIZE_STRING + 1,"%.0f&nbsp;MiB",
		SizeInBytes / Mi);
   else if (SizeInBytes < Ti)
      snprintf (FileSizeStr,Fil_MAX_BYTES_FILE_SIZE_STRING + 1,"%.0f&nbsp;GiB",
		SizeInBytes / Gi);
   else
      snprintf (FileSizeStr,Fil_MAX_BYTES_FILE_SIZE_STRING + 1,"%.0f&nbsp;TiB",
		SizeInBytes / Ti);
  }

void Fil_WriteFileSizeFull (double SizeInBytes,
                            char FileSizeStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1])
  {
   if (SizeInBytes < Ki)
      snprintf (FileSizeStr,Fil_MAX_BYTES_FILE_SIZE_STRING + 1,"%.0f&nbsp;B",
		SizeInBytes);
   else if (SizeInBytes < Mi)
      snprintf (FileSizeStr,Fil_MAX_BYTES_FILE_SIZE_STRING + 1,"%.1f&nbsp;KiB",
		SizeInBytes / Ki);
   else if (SizeInBytes < Gi)
      snprintf (FileSizeStr,Fil_MAX_BYTES_FILE_SIZE_STRING + 1,"%.1f&nbsp;MiB",
		SizeInBytes / Mi);
   else if (SizeInBytes < Ti)
      snprintf (FileSizeStr,Fil_MAX_BYTES_FILE_SIZE_STRING + 1,"%.1f&nbsp;GiB",
		SizeInBytes / Gi);
   else
      snprintf (FileSizeStr,Fil_MAX_BYTES_FILE_SIZE_STRING + 1,"%.1f&nbsp;TiB",
		SizeInBytes / Ti);
  }

/*****************************************************************************/
/********* Add public directory used to link private path to cache ***********/
/*****************************************************************************/

void Fil_AddPublicDirToCache (const char *FullPathPriv,
                              const char TmpPubDir[PATH_MAX + 1])
  {
   /***** Trivial check: if no current session, don't do anything *****/
   if (Gbl.Session.Status != Ses_OPEN)
      return;

   /***** Delete possible old entry *****/
   Fil_DB_RemovePublicDirFromCache (FullPathPriv);

   /***** Insert new entry *****/
   Fil_DB_AddPublicDirToCache (FullPathPriv,TmpPubDir);
  }

/*****************************************************************************/
/******** Get public directory used to link private path from cache **********/
/*****************************************************************************/

Exi_Exist_t Fil_GetPublicDirFromCache (const char *FullPathPriv,
				       char TmpPubDir[PATH_MAX + 1])
  {
   bool Cached;
   Exi_Exist_t TmpPubDirExists;
   char FullPathTmpPubDir[PATH_MAX + 1];

   /***** Reset temporary directory *****/
   TmpPubDir[0] = '\0';

   /***** Trivial check: if no current session, don't do anything *****/
   if (Gbl.Session.Status != Ses_OPEN)
      return Exi_DOES_NOT_EXIST;

   /***** Get temporary directory from cache *****/
   Fil_DB_GetPublicDirFromCache (FullPathPriv,TmpPubDir);
   Cached = (TmpPubDir[0] != '\0');

   /***** Check if temporary public directory exists *****/
   if (Cached)
     {
      /* If not exists (it could be deleted if its lifetime has expired)
	 ==> remove from cache */
      snprintf (FullPathTmpPubDir,sizeof (FullPathTmpPubDir),"%s/%s",
		Cfg_PATH_FILE_BROWSER_TMP_PUBLIC,TmpPubDir);
      TmpPubDirExists = Fil_CheckIfPathExists (FullPathTmpPubDir);
      if (TmpPubDirExists == Exi_DOES_NOT_EXIST)
	 Fil_DB_RemovePublicDirFromCache (FullPathPriv);
      return TmpPubDirExists;
     }

   return Exi_DOES_NOT_EXIST;
  }
