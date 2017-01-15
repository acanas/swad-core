// swad_convert_works.c
// Created on: 28/11/2013
// Author: Antonio Cañas Vargas
// Compile with: gcc -Wall -O1 swad_convert_works.c -o swad_convert_works -lmysqlclient -L/usr/lib64/mysql

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
#define FOLDER_CRS		"crs"
#define FOLDER_USR		"usr"
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
   char Query[512];
   MYSQL mysql;
   MYSQL_RES *mysql_res_crs;
   MYSQL_RES *mysql_res_usr;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   unsigned NumUsrs;
   unsigned NumUsr;
   long CrsCod;
   long UsrCod;
   char UsrID[ID_MAX_LENGTH_USR_ID + 1];
   char OldPathUsrs[PATH_MAX + 1];
   char OldPathUsr[PATH_MAX + 1];
   char Path02u[PATH_MAX + 1];
   char Command[1024 + PATH_MAX * 2];

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

   sprintf (Query,"SELECT CrsCod FROM courses ORDER BY CrsCod");
   if (mysql_query (&mysql,Query))
     {
      fprintf (stderr,"%s",mysql_error (&mysql));
      return 3;
     }
   if ((mysql_res_crs = mysql_store_result (&mysql)) == NULL)
     {
      fprintf (stderr,"%s",mysql_error (&mysql));
      return 4;
     }
   if ((NumCrss = (unsigned) mysql_num_rows (mysql_res_crs)))
     {
      for (NumCrs=0; NumCrs<NumCrss; NumCrs++)
	{
	 row = mysql_fetch_row (mysql_res_crs);
	 if (row[0])
	    if (row[0][0])
	       if (sscanf (row[0],"%ld",&CrsCod) == 1)	// CrsCod
		 {
		  sprintf (OldPathUsrs,"%s/%s/%ld/%s",
			   PATH_SWAD_PRIVATE,FOLDER_CRS,CrsCod,FOLDER_USR);
		  if (CheckIfPathExists (OldPathUsrs))
		    {
		     sprintf (Command,"mv %s %s_backup",OldPathUsrs,OldPathUsrs);
		     ExecuteCommand (Command);

		     sprintf (Command,"mkdir %s",OldPathUsrs);
		     ExecuteCommand (Command);

		     sprintf (Query,"SELECT usr_IDs.UsrCod,usr_IDs.UsrID"
				    " FROM crs_usr,usr_IDs"
				    " WHERE crs_usr.CrsCod='%ld'"
				    " AND crs_usr.UsrCod=usr_IDs.UsrCod"
				    " ORDER BY usr_IDs.UsrCod",
			      CrsCod);

		     if (mysql_query (&mysql,Query))
		       {
			fprintf (stderr,"%s",mysql_error (&mysql));
			return 3;
		       }
		     if ((mysql_res_usr = mysql_store_result (&mysql)) == NULL)
		       {
			fprintf (stderr,"%s",mysql_error (&mysql));
			return 4;
		       }
		     if ((NumUsrs = (unsigned) mysql_num_rows (mysql_res_usr)))
		       {
			for (NumUsr=0; NumUsr<NumUsrs; NumUsr++)
			  {
			   row = mysql_fetch_row (mysql_res_usr);
			   if (row[0])
			      if (row[0][0])
				 if (sscanf (row[0],"%ld",&UsrCod) == 1)	// UsrCod
				    if (row[1])
				      {
				       Str_Copy (UsrID,row[1],ID_MAX_LENGTH_USR_ID);		// UsrID

				       sprintf (OldPathUsr,"%s_backup/%s",OldPathUsrs,UsrID);
				       if (CheckIfPathExists (OldPathUsr))
					 {
					  sprintf (Path02u,"%s/%02u",
						   OldPathUsrs,(unsigned) (UsrCod % 100));
					  if (!CheckIfPathExists (Path02u))
					    {
					     sprintf (Command,"mkdir %s",Path02u);
					     ExecuteCommand (Command);
					    }
					  sprintf (Command,"mv %s %s/%ld",OldPathUsr,Path02u,UsrCod);
					  ExecuteCommand (Command);
					 }
				      }
		          }
			mysql_free_result (mysql_res_usr);
		       }
		     sprintf (Command,"chown -R %s:%s %s",WEB_USER,WEB_USER,OldPathUsrs);
		     ExecuteCommand (Command);
/*
		     sprintf (Command,"rm -Rf %s_backup",OldPathUsrs);
		     ExecuteCommand (Command);
*/
		     printf ("\n");
		    }
		 }
	}
     }
   mysql_free_result (mysql_res_crs);

   printf ("# courses: %u\n",NumCrss);
   return 0;
  }
