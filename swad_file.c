// swad_file.c: files

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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
#include <linux/stddef.h>	// For NULL
#include <stdio.h>		// For FILE,fprintf
#include <stdlib.h>		// For exit, system, malloc, calloc, free, etc.
#include <string.h>		// For string functions
#include <sys/stat.h>		// For mkdir
#include <sys/types.h>		// For mkdir
#include <unistd.h>		// For unlink

#include "swad_config.h"
#include "swad_global.h"
#include "swad_file.h"
#include "swad_string.h"

#include "swad_database.h"	// Only for debug purposes. Remove this line!

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/******** Create HTML output file for the web page sent by this CGI **********/
/*****************************************************************************/

void Fil_CreateFileForHTMLOutput (void)
  {
   char PathHTMLOutputPriv[PATH_MAX+1];

   /***** Check if exists the directory for HTML output. If not exists, create it *****/
   sprintf (PathHTMLOutputPriv,"%s/%s",Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_OUT);
   Fil_CreateDirIfNotExists (PathHTMLOutputPriv);

   /***** Remove old files *****/
   Fil_RemoveOldTmpFiles (PathHTMLOutputPriv,Cfg_TIME_TO_DELETE_HTML_OUTPUT,false);

   /***** Create a unique name for the file *****/
   sprintf (Gbl.HTMLOutput.FileName,"%s/%s.html",
            PathHTMLOutputPriv,Gbl.UniqueNameEncrypted);

   /***** Open file for writing and reading *****/
   if ((Gbl.F.Out = fopen (Gbl.HTMLOutput.FileName,"w+t")) == NULL)
     {
      Gbl.F.Out = stdout;
      Lay_ShowErrorAndExit ("Can not create output file.");
     }
  }

/*****************************************************************************/
/****************** Close and remove the HTML output file ********************/
/*****************************************************************************/

void Fil_CloseAndRemoveFileForHTMLOutput (void)
  {
   if (Gbl.F.Out)
     {
      fclose (Gbl.F.Out);
      unlink (Gbl.HTMLOutput.FileName);
     }
   Gbl.F.Out = stdout;
  }

/*****************************************************************************/
/********** Open temporary file and write on it reading from stdin ***********/
/*****************************************************************************/
// On error, Gbl.Message will contain feedback

bool Fil_ReadStdinIntoTmpFile (void)
  {
   extern const char *Txt_UPLOAD_FILE_File_too_large_maximum_X_MiB_NO_HTML;
   extern const char *Txt_UPLOAD_FILE_Upload_time_too_long_maximum_X_minutes_NO_HTML;
   unsigned long long TmpFileSize;
   bool FileIsTooBig = false;
   bool TimeExceeded = false;

   if ((Gbl.F.Tmp = tmpfile ()) == NULL)
     {
      Fil_EndOfReadingStdin ();
      Lay_ShowErrorAndExit ("Can not create temporary file.");
     }
   for (TmpFileSize = 0;
	!feof (stdin) && !FileIsTooBig && !TimeExceeded;
	TmpFileSize++)
      if (TmpFileSize < Fil_MAX_FILE_SIZE)
        {
         if (!(TmpFileSize % (64ULL*1024ULL)))	// Check timeout from time to time
            if (time (NULL) - Gbl.StartExecutionTimeUTC >= Cfg_TIME_TO_ABORT_FILE_UPLOAD)
               TimeExceeded = true;
         fputc (fgetc (stdin),Gbl.F.Tmp);
        }
      else
         FileIsTooBig = true;
   if (FileIsTooBig || TimeExceeded)
     {
      Fil_EndOfReadingStdin ();  // If stdin were not fully read, there will be problems with buffers
      if (FileIsTooBig)
         sprintf (Gbl.Message,Txt_UPLOAD_FILE_File_too_large_maximum_X_MiB_NO_HTML,
                  (unsigned long) (Fil_MAX_FILE_SIZE / (1024ULL * 1024ULL)));
      else
         sprintf (Gbl.Message,Txt_UPLOAD_FILE_Upload_time_too_long_maximum_X_minutes_NO_HTML,
                  (unsigned long) (Cfg_TIME_TO_ABORT_FILE_UPLOAD / 60UL));

      /* Don't write HTML at all */
      Gbl.Layout.HTMLStartWritten =
      Gbl.Layout.DivsEndWritten   =
      Gbl.Layout.HTMLEndWritten   = true;

      /* Start HTTP response */
      fprintf (stdout,"Content-type: text/plain; charset=windows-1252\n");

      /* Status code and message */
      fprintf (stdout,"Status: 501 Not Implemented\r\n\r\n"
		      "%s\n",
	       Gbl.Message);
      return false;
     }
   rewind (Gbl.F.Tmp);

   return true;
  }

/*****************************************************************************/
/********** End the reading of all the characters coming from stdin **********/
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
void Fil_StartReceptionOfFile (char *SrcFileName,char *MIMEType)
  {
   char *Ptr;
   int Ch,i;

   /* At this point, a heading has been read from Gbl.F.Tmp
      with all the variables passed by form */
   rewind (Gbl.F.Tmp);
   if (!Str_FindStrInFile (Gbl.F.Tmp,Fil_NAME_OF_PARAM_FILENAME_ORG,Str_NO_SKIP_HTML_COMMENTS))
     {
      sprintf (Gbl.Message,"Error uploading file: parameter <strong>%s</strong> not found.",
               Fil_NAME_OF_PARAM_FILENAME_ORG);
      Lay_ShowErrorAndExit (Gbl.Message);
     }

   /* Go to the name of the source file */
   if (!Str_FindStrInFile (Gbl.F.Tmp,"filename=\"",Str_NO_SKIP_HTML_COMMENTS))
      Lay_ShowErrorAndExit ("Error uploading file: parameter &quot;filename&quot; not found.");

   /* Get the name of the source file */
   Ptr = SrcFileName;
   while ((Ch = fgetc (Gbl.F.Tmp)) != (int) '\"')
      *Ptr++ = Ch;
   *Ptr = '\0';

   /* Get and check the type of data */
   if (!Str_FindStrInFile (Gbl.F.Tmp,"Content-Type:",Str_NO_SKIP_HTML_COMMENTS))
      Lay_ShowErrorAndExit ("Error uploading file: &quot;Content-Type&quot; not found.");

   /* Skip spaces and get the type of file */
   while (isspace (Ch = fgetc (Gbl.F.Tmp)));
   for (i=0, Ptr = MIMEType;
	!isspace (Ch) && i < Brw_MAX_BYTES_MIME_TYPE;
	i++)
     {
      *Ptr++ = Str_ConvertToLowerLetter ((char) Ch);
      Ch = fgetc (Gbl.F.Tmp);
     }
   *Ptr = '\0';
  }

/*****************************************************************************/
/****************** End the reception of data of a file **********************/
/*****************************************************************************/

bool Fil_EndReceptionOfFile (char *FileNameDataTmp)
  {
   extern const char *Txt_UPLOAD_FILE_File_too_large_maximum_X_MiB_NO_HTML;
   FILE *FileDataTmp;
   int Result;  // Result of the reception of the file

   /***** Open a new file temporary *****/
   if ((FileDataTmp = fopen (FileNameDataTmp,"wb")) == NULL)
      Lay_ShowErrorAndExit ("Can not open temporary file.");

   /***** Skip carriage returns, spaces, etc. *****/
   Str_SkipSpacesInFile (Gbl.F.Tmp);

   /* At this point, the data of the file begin */

   /***** Write the file *****/
   Result = Str_ReceiveFileUntilDelimitStr (Gbl.F.Tmp,FileDataTmp,(char *) NULL,
	                                    Gbl.DelimiterStringIncludingInitialRet,
	                                    Fil_MAX_FILE_SIZE);
   fclose (FileDataTmp);
   if (Result != 1)
     /* 0 ==> File too large; -1 ==> Unfinished transmission */
     {
      unlink (FileNameDataTmp);
      sprintf (Gbl.Message,Txt_UPLOAD_FILE_File_too_large_maximum_X_MiB_NO_HTML,
               (unsigned long) (Fil_MAX_FILE_SIZE / (1024ULL * 1024ULL)));
      return false;	// Error
     }
   return true;		// Success
  }

/*****************************************************************************/
/************* Create names and new file to update a existing file ***********/
/*****************************************************************************/
// CurrentName does not change
// OldName is created
// NewName is created

void Fil_CreateUpdateFile (const char *CurrentName,const char *ExtensionOldName,char *OldName,char *NewName,FILE **NewFile)
  {
   int LongRaizFich = Str_GetLengthRootFileName (CurrentName);

   strncpy (NewName,CurrentName,LongRaizFich);
   NewName[LongRaizFich] = '\0';
   sprintf (OldName,"%s%s",NewName,ExtensionOldName);
   strcat (NewName,".new");

   /* The new file shouldn't exist. If it exists is due to any error when running this CGI formerly
      and the file was not renamed successfully. In this case, remove it! */
   if (Fil_CheckIfPathExists (NewName))
      unlink (NewName);

   /* Open the new file */
   if ((*NewFile = fopen (NewName,"wb")) == NULL)
     {
      sprintf (Gbl.Message,"Can not create file <strong>%s</strong>.",NewName);
      Lay_ShowErrorAndExit (Gbl.Message);
     }
  }

/*****************************************************************************/
/****************** Close and rename files related to an update **************/
/*****************************************************************************/

void Fil_CloseUpdateFile (const char *CurrentName,const char *OldName,const char *NewName,FILE *NewFile)
  {
   /* Close file */
   fclose (NewFile);

   /* Rename the old file and the new file */
   if (rename (CurrentName,OldName)) // mv CurrentName OldName Ej: mv file.html file.old
     {
      sprintf (Gbl.Message,"Can not rename the file <strong>%s</strong> as <strong>%s</strong>.",
               CurrentName,OldName);
      Lay_ShowErrorAndExit (Gbl.Message);
     }
   if (rename (NewName,CurrentName)) // mv NewName CurrentName Ej: mv file.new file.html
     {
      sprintf (Gbl.Message,"Can not rename the file <strong>%s</strong> as <strong>%s</strong>.",
               NewName,CurrentName);
      Lay_ShowErrorAndExit (Gbl.Message);
     }
  }

/*****************************************************************************/
/*********************** Rename a file or directory **************************/
/*****************************************************************************/

bool Fil_RenameFileOrDir (const char *PathOld,const char *PathNew)
  {
   extern const char *Txt_There_is_already_a_non_empty_folder_named_X;
   extern const char *Txt_There_is_already_a_file_named_X;

   /* Rename the file or directory */
   if (rename (PathOld,PathNew) == 0)
      return true;
   else
     {
      switch (errno)
        {
	 case ENOTEMPTY:
	 case EEXIST:
	    sprintf (Gbl.Message,Txt_There_is_already_a_non_empty_folder_named_X,
	             PathNew);
	    break;
	 case ENOTDIR:
	    sprintf (Gbl.Message,Txt_There_is_already_a_file_named_X,
	             PathNew);
	    break;
	 case EACCES:
	    Lay_ShowErrorAndExit ("Write is forbidden.");
	    break;
         default:
	    Lay_ShowErrorAndExit ("Can not rename file or folder.");
	    break;
	}
      return false;
     }
  }

/*****************************************************************************/
/***************** Check if existe a file or directory ***********************/
/*****************************************************************************/
/* Return true if exists and false if not exists */

bool Fil_CheckIfPathExists (const char *Path)
  {
   return access (Path,F_OK) ? false :
	                       true;
  }

/*****************************************************************************/
/********** Check if a directory exists. If not exists, create it! ***********/
/*****************************************************************************/

void Fil_CreateDirIfNotExists (const char *Path)
  {
   if (!Fil_CheckIfPathExists (Path))
      if (mkdir (Path,(mode_t) 0xFFF) != 0)
        {
	 sprintf (Gbl.Message,"Can not create folder <strong>%s</strong>.",Path);
	 Lay_ShowErrorAndExit (Gbl.Message);
        }
  }

/*****************************************************************************/
/********************* Remove old temporary directories **********************/
/*****************************************************************************/

void Fil_RemoveOldTmpFiles (const char *Path,time_t TimeToRemove,bool RemoveDirectory)
  {
   struct dirent **FileList;
   int NumFile;
   int NumFiles;
   char Path2[PATH_MAX+1];
   struct stat FileStatus;

   lstat (Path,&FileStatus);
   if (S_ISDIR (FileStatus.st_mode))		// It's a directory
     {
      /***** Scan the directory *****/
      if ((NumFiles = scandir (Path,&FileList,NULL,NULL)) >= 0)	// No error
	{
	 /* Loop over files */
	 for (NumFile = 0;
	      NumFile < NumFiles;
	      NumFile++)
	   {
	    if (strcmp (FileList[NumFile]->d_name,".") &&
		strcmp (FileList[NumFile]->d_name,".."))	// Skip directories "." and ".."
	      {
	       sprintf (Path2,"%s/%s",Path,FileList[NumFile]->d_name);
	       Fil_RemoveOldTmpFiles (Path2,TimeToRemove,true);	// Recursive call
	      }
	    free ((void *) FileList[NumFile]);
	   }
	 free ((void *) FileList);

	 if (RemoveDirectory)
	    /* Remove the directory itself */
	    if (FileStatus.st_mtime < Gbl.StartExecutionTimeUTC - TimeToRemove)
	       rmdir (Path);
	}
      else
	 Lay_ShowErrorAndExit ("Error while scanning directory.");
     }
   else
      if (FileStatus.st_mtime < Gbl.StartExecutionTimeUTC - TimeToRemove)
	 unlink (Path);
  }

/*****************************************************************************/
/**************************** Fast copy of files *****************************/
/*****************************************************************************/

void Fil_FastCopyOfFiles (const char *PathSrc,const char *PathTgt)
  {
   FILE *FileSrc,*FileTgt;

   /***** Open source file *****/
   if ((FileSrc = fopen (PathSrc,"rb")) == NULL)
      Lay_ShowErrorAndExit ("Can not open source file.");

   /***** Open destination file *****/
   if ((FileTgt = fopen (PathTgt,"wb")) == NULL)
      Lay_ShowErrorAndExit ("Can not open target file.");

   /***** Copy source file into destination file *****/
   Fil_FastCopyOfOpenFiles (FileSrc,FileTgt);

   /***** Close the files *****/
   fclose (FileTgt);
   fclose (FileSrc);
  }

/*****************************************************************************/
/************************* Fast copy of open files ***************************/
/*****************************************************************************/

#define NUM_BYTES_PER_COPY 4096

void Fil_FastCopyOfOpenFiles (FILE *FileSrc,FILE *FileTgt)
  {
   unsigned char Bytes[NUM_BYTES_PER_COPY];
   size_t NumBytesRead;

   while ((NumBytesRead = fread ((void *) Bytes,sizeof (Bytes[0]),(size_t) NUM_BYTES_PER_COPY,FileSrc)))
      fwrite ((void *) Bytes,sizeof (Bytes[0]),NumBytesRead,FileTgt);
  }

/*****************************************************************************/
/**************************** Close XML file *********************************/
/*****************************************************************************/

void Fil_CloseXMLFile (void)
  {
   if (Gbl.F.XML)
     {
      fclose (Gbl.F.XML);
      Gbl.F.XML = NULL;	// To indicate that it is not open
     }
  }
