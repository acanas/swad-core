// swad_config.c: configuration

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include <stdio.h>		// For fseek, ftell...
#include <stdlib.h>		// For malloc and free
#include <string.h>		// For strcasecmp...

#include "swad_config.h"
#include "swad_error.h"
#include "swad_layout.h"
#include "swad_string.h"

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

static struct
  {
   char Database[Cfg_MAX_BYTES_DATABASE_PASSWORD + 1];
   char SMTP[Cfg_MAX_BYTES_SMTP_PASSWORD + 1];
  } Cfg_Passwords;

/*****************************************************************************/
/******************* Get SMTP password from config file **********************/
/*****************************************************************************/

#define Cfg_MAX_BYTES_STR 256

void Cfg_GetConfigFromFile (void)
  {
   FILE *FileCfg;
   size_t Length;
   char *Config;
   const char *Ptr;
   char Str[Cfg_MAX_BYTES_STR + 1];

   /***** Read config from file to string *****/
   /* Open config file */
   if ((FileCfg = fopen (Cfg_FILE_CONFIG,"rb")) == NULL)
      Err_FileFolderNotFoundExit ();

   /* Compute file size */
   fseek (FileCfg,0L,SEEK_END);
   Length = (size_t) ftell (FileCfg);
   fseek (FileCfg,0L,SEEK_SET);

   /* Allocate memory for buffer */
   if ((Config = malloc (Length + 1)) == NULL)
     {
      fclose (FileCfg);
      Err_NotEnoughMemoryExit ();
     }

   /* Copy file content into buffer */
   if (fread (Config,sizeof (char),Length,FileCfg) != Length)
     {
      fclose (FileCfg);
      Err_ShowErrorAndExit ("Can not read config.");
     }
   Config[Length] = '\0';

   /* Close config file */
   fclose (FileCfg);

   /***** Search passwords *****/
   Cfg_Passwords.Database[0] =
   Cfg_Passwords.SMTP[0]     = '\0';
   for (Ptr = Config;
        *Ptr;
       )
     {
      Str_GetNextStringUntilSpace (&Ptr,Str,Cfg_MAX_BYTES_STR);
      if (!strcasecmp (Str,"DATABASE_PASSWORD"))
          Str_GetNextStringUntilSpace (&Ptr,Cfg_Passwords.Database,Cfg_MAX_BYTES_DATABASE_PASSWORD);
      else if (!strcasecmp (Str,"SMTP_PASSWORD"))
          Str_GetNextStringUntilSpace (&Ptr,Cfg_Passwords.SMTP    ,Cfg_MAX_BYTES_SMTP_PASSWORD);
     }
   if (!Cfg_Passwords.Database[0] ||
       !Cfg_Passwords.SMTP[0])
      Err_ShowErrorAndExit ("Bad config format.");
  }

const char *Cfg_GetDatabasePassword (void)
  {
   return Cfg_Passwords.Database;
  }

const char *Cfg_GetSMTPPassword (void)
  {
   return Cfg_Passwords.SMTP;
  }
