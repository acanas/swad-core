// swad_convert_photos.c
// Created on: 25/11/2013
// Author: Antonio Cañas Vargas
// Compile with: gcc -Wall -O1 swad_convert_photos.c -o swad_convert_photos -lmysqlclient -L/usr/lib64/mysql

#include "swad_ID.h"

#include <mysql/mysql.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DATABASE_HOST		"swad.ugr.es"
#define DATABASE_USER		"swad"
#define DATABASE_PASSWORD	"********"
#define DATABASE_DBNAME		"swad"
#define PATH_SWAD_PRIVATE	"/var/www/swad"
#define PATH_SWAD_PUBLIC	"/var/www/html/swad"
#define FOLDER_PHOTO		"photo"
#define WEB_USER		"apache"

void ExecuteCommand (const char *Command)
  {
   printf ("%s\n",Command);

   if (system (Command) < 0)
     {
      fprintf (stderr,"Error when executing command: %s\n",Command);
      exit (1);
     }

  }

int CheckIfPathExists (const char *Path)
  {
   return access (Path,F_OK) ? 0 :
	                       1;
  }

int main (void)
  {
   char *Query = "SELECT UsrCod,UsrID FROM usr_IDs ORDER BY UsrCod";
   MYSQL mysql;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;
   unsigned NumPhotos = 0;
   unsigned i;
   long UsrCod;
   char UsrID[ID_MAX_BYTES_USR_ID + 1];
   char OldPathPhoto[PATH_MAX + 1];
   char Command[1024 + PATH_MAX * 2];

   snprintf (Command,sizeof (Command),
	     "mv %s/%s %s/%s_backup",
	     PATH_SWAD_PRIVATE,FOLDER_PHOTO,
	     PATH_SWAD_PRIVATE,FOLDER_PHOTO);
   ExecuteCommand (Command);
   snprintf (Command,sizeof (Command),
	     "mkdir %s/%s",
	     PATH_SWAD_PRIVATE,FOLDER_PHOTO);
   ExecuteCommand (Command);
   for (i=0; i<100; i++)
     {
      snprintf (Command,sizeof (Command),
	        "mkdir %s/%s/%02u",
	        PATH_SWAD_PRIVATE,FOLDER_PHOTO,i);
      ExecuteCommand (Command);
     }

   if (mysql_init (&mysql) == NULL)
     {
      fprintf (stderr,"Can not init MySQL.");
      return 1;
     }
   if (!mysql_real_connect (&mysql,DATABASE_HOST,DATABASE_USER,DATABASE_PASSWORD,DATABASE_DBNAME,0,NULL,0))
     {
      fprintf (stderr,"Can not connect to database");
      return 2;
     }
   if (mysql_query (&mysql,Query))
     {
      fprintf (stderr,"%s",mysql_error (&mysql));
      return 3;
     }
   if ((mysql_res = mysql_store_result (&mysql)) == NULL)
     {
      fprintf (stderr,"%s",mysql_error (&mysql));
      return 4;
     }
   if ((NumRows = (unsigned) mysql_num_rows (mysql_res)))
     {
      for (i=0; i<NumRows; i++)
	{
	 row = mysql_fetch_row (mysql_res);
	 if (row[0])
	    if (row[0][0])
	       if (sscanf (row[0],"%ld",&UsrCod) == 1)	// UsrCod
		  if (row[1])
		    {
                     Str_Copy (UsrID,row[1],
                               ID_MAX_BYTES_USR_ID);	// UsrID
                     snprintf (OldPathPhoto,sizeof (OldPathPhoto),
                	       "%s/%s_backup/%s_original.jpg",
                	       PATH_SWAD_PRIVATE,FOLDER_PHOTO,UsrID);
                     if (CheckIfPathExists (OldPathPhoto))
                       {
                        snprintf (Command,sizeof (Command),
                                  "cp -a %s %s/%s/%02u/%ld_original.jpg",
                	          OldPathPhoto,
                	          PATH_SWAD_PRIVATE,FOLDER_PHOTO,(unsigned) (UsrCod % 100),UsrCod);
                        ExecuteCommand (Command);
                       }

                     snprintf (OldPathPhoto,sizeof (OldPathPhoto),
                	       "%s/%s_backup/%s.jpg",
                	       PATH_SWAD_PRIVATE,FOLDER_PHOTO,UsrID);
                     if (CheckIfPathExists (OldPathPhoto))
                       {
                        snprintf (Command,sizeof (Command),
                                  "cp -a %s %s/%s/%02u/%ld.jpg",
                	          OldPathPhoto,
                	          PATH_SWAD_PRIVATE,FOLDER_PHOTO,(unsigned) (UsrCod % 100),UsrCod);
                        ExecuteCommand (Command);
                        NumPhotos++;
                       }
		    }
	}
     }
   mysql_free_result (mysql_res);
   snprintf (Command,sizeof (Command),
	     "chown -R %s:%s %s/%s",
	     WEB_USER,WEB_USER,
	     PATH_SWAD_PRIVATE,FOLDER_PHOTO);
   ExecuteCommand (Command);

   snprintf (Command,sizeof (Command),
	     "rm -f %s/%s/*.jpg",
	     PATH_SWAD_PUBLIC,FOLDER_PHOTO);
   ExecuteCommand (Command);

   printf ("# Rows : %u\n"
	   "# Photos: %u\n",NumRows,NumPhotos);
   return 0;
  }
