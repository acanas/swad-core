// swad_RSS.c: RSS feeds

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <string.h>

#include "swad_autolink.h"
#include "swad_call_for_exam_database.h"
#include "swad_changelog.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_notice.h"
#include "swad_notice_database.h"
#include "swad_RSS.h"

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void RSS_WriteNotices (FILE *FileRSS,struct Hie_Node *Crs);
static void RSS_WriteCallsForExams (FILE *FileRSS,struct Hie_Node *Crs);

/*****************************************************************************/
/******* Update RSS archive with active notices in the current course ********/
/*****************************************************************************/

void RSS_UpdateRSSFileForACrs (struct Hie_Node *Crs)
  {
   char PathRelPublRSSDir[PATH_MAX + 1];
   char PathRelPublRSSFile[PATH_MAX + 1 + NAME_MAX + 1];
   FILE *FileRSS;
   char RSSLink[WWW_MAX_BYTES_WWW + 1];
   struct tm *tm;
   time_t t = Dat_GetStartExecutionTimeUTC ();

   /***** Create RSS directory if not exists *****/
   snprintf (PathRelPublRSSDir,sizeof (PathRelPublRSSDir),"%s/%ld/%s",
	     Cfg_PATH_CRS_PUBLIC,Crs->HieCod,Cfg_RSS_FOLDER);
   Fil_CreateDirIfNotExists (PathRelPublRSSDir);

   /***** Create RSS file *****/
   snprintf (PathRelPublRSSFile,sizeof (PathRelPublRSSFile),"%s/%s",
	     PathRelPublRSSDir,Cfg_RSS_FILE);
   if ((FileRSS = fopen (PathRelPublRSSFile,"wb")) == NULL)
      Err_ShowErrorAndExit ("Can not create RSS file.");

   /***** Write RSS header *****/
   fprintf (FileRSS,"<?xml version=\"1.0\" encoding=\"windows-1252\"?>\n"
                    "<rss version=\"2.0\" xmlns:atom=\"http://www.w3.org/2005/Atom\">\n");

   /***** Write channel header *****/
   fprintf (FileRSS,"<channel>\n");

   fprintf (FileRSS,"<atom:link href=\"");
   RSS_BuildRSSLink (RSSLink,Crs->HieCod);
   fprintf (FileRSS,"%s",RSSLink);
   fprintf (FileRSS,"\" rel=\"self\" type=\"application/rss+xml\" />\n");

   fprintf (FileRSS,"<title>%s: %s</title>\n",
            Cfg_PLATFORM_SHORT_NAME,Crs->ShrtName);
   fprintf (FileRSS,"<link>%s/?crs=%ld</link>\n",
            Cfg_URL_SWAD_CGI,Crs->HieCod);
   fprintf (FileRSS,"<description>%s</description>\n",
            Crs->FullName);
   fprintf (FileRSS,"<language>%s</language>\n",
            Cfg_RSS_LANGUAGE);
   fprintf (FileRSS,"<webMaster>%s (%s)</webMaster>\n",
            Cfg_PLATFORM_RESPONSIBLE_EMAIL,Cfg_PLATFORM_RESPONSIBLE_NAME);

   fprintf (FileRSS,"<image>\n");
   fprintf (FileRSS,"<url>%s/swad112x32.png</url>\n",
            Cfg_URL_ICON_PUBLIC);
   fprintf (FileRSS,"<title>%s: %s</title>\n",
            Cfg_PLATFORM_SHORT_NAME,Crs->ShrtName);
   fprintf (FileRSS,"<link>%s/?crs=%ld</link>\n",
            Cfg_URL_SWAD_CGI,Crs->HieCod);
   fprintf (FileRSS,"<width>112</width>\n"
                    "<height>32</height>\n"
                    "</image>\n");

   // All date-times in RSS conform to the Date and Time Specification of RFC 822, with the exception that the year may be expressed with two characters or four characters (four preferred)
   fprintf (FileRSS,"<pubDate>");	
   tm = gmtime (&t);
   Dat_WriteRFC822DateFromTM (FileRSS,tm);
   fprintf (FileRSS,"</pubDate>\n");

   fprintf (FileRSS,"<lastBuildDate>");	
   tm = gmtime (&t);
   Dat_WriteRFC822DateFromTM (FileRSS,tm);
   fprintf (FileRSS,"</lastBuildDate>\n");

   fprintf (FileRSS,"<generator>%s</generator>\n",
            Log_PLATFORM_VERSION);
   fprintf (FileRSS,"<docs>http://www.rssboard.org/rss-specification</docs>\n");

   /***** Write notices as RSS items *****/
   RSS_WriteNotices (FileRSS,Crs);

   /***** Write exam announcements as RSS items *****/
   RSS_WriteCallsForExams (FileRSS,Crs);

   /***** Write channel footer *****/
   fprintf (FileRSS,"</channel>\n");

   /***** Write RSS footer *****/
   fprintf (FileRSS,"</rss>\n");

   /***** Close RSS file *****/
   fclose (FileRSS);
  }

/*****************************************************************************/
/************* Write notices of a course as items of RSS file ****************/
/*****************************************************************************/

static void RSS_WriteNotices (FILE *FileRSS,struct Hie_Node *Crs)
  {
   extern const char *Txt_Notice;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct Usr_Data UsrDat;
   struct tm *tm;
   time_t CreatTimeUTC;
   long NotCod;
   unsigned NumNotices;
   unsigned NumNot;
   char Content[Cns_MAX_BYTES_TEXT + 1];

   /***** Write items with active notices *****/
   if ((NumNotices = Not_DB_GetActiveNotices (&mysql_res,Crs->HieCod)))
     {
      Usr_UsrDataConstructor (&UsrDat);

      for (NumNot = 0;
	   NumNot < NumNotices;
	   NumNot++)
        {
         /***** Get data of the notice *****/
         row = mysql_fetch_row (mysql_res);

         /* Get notice code (row[0]) */
         if (sscanf (row[0],"%ld",&NotCod) != 1)
            Err_WrongNoticeExit ();

         /* Get UTC date-time of publication (row[1]) */
         CreatTimeUTC = 0L;
         if (row[1])
            sscanf (row[1],"%ld",&CreatTimeUTC);

         /* Get author (row[2]) */
         UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[2]);
         Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,	// Get author's data from database
                                                  Usr_DONT_GET_PREFS,
                                                  Usr_DONT_GET_ROLE_IN_CRS);

         /***** Write item with notice *****/
         fprintf (FileRSS,"<item>\n");

         /* Write title (first characters) of the notice */
         Str_Copy (Content,row[3],sizeof (Content) - 1);
         Str_LimitLengthHTMLStr (Content,40);	// Remove when notice has a Subject
         fprintf (FileRSS,"<title>%s: ",Txt_Notice);
         Str_FilePrintStrChangingBRToRetAndNBSPToSpace (FileRSS,Content);
         fprintf (FileRSS,"</title>\n");

         /* Write link to the notice */
         fprintf (FileRSS,"<link>%s/?crs=%ld</link>\n",
                  Cfg_URL_SWAD_CGI,Crs->HieCod);

         /* Write full content of the notice */
         Str_Copy (Content,row[3],sizeof (Content) - 1);
         ALn_InsertLinks (Content,Cns_MAX_BYTES_TEXT,40);
         fprintf (FileRSS,"<description><![CDATA[<p><em>%s %s %s:</em></p><p>%s</p>]]></description>\n",
                  UsrDat.FrstName,UsrDat.Surname1,UsrDat.Surname2,Content);

         /* Write author */
         if (UsrDat.Email[0])
            fprintf (FileRSS,"<author>%s (%s %s %s)</author>\n",
                     UsrDat.Email,UsrDat.FrstName,UsrDat.Surname1,UsrDat.Surname2);

         /* Write unique string for this item */
         fprintf (FileRSS,"<guid isPermaLink=\"false\">%s, course #%ld, notice #%ld</guid>\n",
                  Cfg_URL_SWAD_CGI,Crs->HieCod,NotCod);

         /* Write publication date */
         fprintf (FileRSS,"<pubDate>");	
         tm = gmtime (&CreatTimeUTC);
         Dat_WriteRFC822DateFromTM (FileRSS,tm);
         fprintf (FileRSS,"</pubDate>\n");

         fprintf (FileRSS,"</item>\n");
        }

      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********** Write calls for exams of a course as items of RSS file ***********/
/*****************************************************************************/

static void RSS_WriteCallsForExams (FILE *FileRSS,struct Hie_Node *Crs)
  {
   extern const char *Txt_Exam;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct Usr_Data UsrDat;
   struct tm *tm;
   time_t CallTimeUTC;
   long ExaCod;
   unsigned NumExams;
   unsigned NumExa;

   if (DB_CheckIfDatabaseIsOpen ())
     {
      /***** Write exam announcements (only future exams) in current course *****/
      if ((NumExams = Cfe_DB_GetFutureCallsForExamsInCurrentCrs (&mysql_res)))
	{
	 Usr_UsrDataConstructor (&UsrDat);

	 for (NumExa = 0;
	      NumExa < NumExams;
	      NumExa++)
	   {
	    /***** Get data of the call for exam *****/
	    row = mysql_fetch_row (mysql_res);

	    /* Get call for exam code (row[0]) */
	    if (sscanf (row[0],"%ld",&ExaCod) != 1)
	       Err_WrongCallForExamExit ();

	    /* Get UTC date-time of publication (row[1]) */
	    CallTimeUTC = 0L;
	    if (row[1])
	       sscanf (row[1],"%ld",&CallTimeUTC);

	    /***** Write item with exam announcement *****/
	    fprintf (FileRSS,"<item>\n");

	    /* Write title (exam date) */
	    fprintf (FileRSS,"<title>%s: %s</title>\n",Txt_Exam,row[2]);

	    /* Write link to the notice */
	    fprintf (FileRSS,"<link>%s/?crs=%ld</link>\n",
		     Cfg_URL_SWAD_CGI,Crs->HieCod);

	    /* Write full content of the exam announcement (row[2]) */
	    fprintf (FileRSS,"<description><![CDATA[<p><em>Fecha examen: %s</em></p>]]></description>\n",	// TODO: Need translation!
		     row[2]);

	    /* Write unique string for this item */
	    fprintf (FileRSS,"<guid isPermaLink=\"false\">%s, course #%ld, exam #%ld</guid>\n",
		     Cfg_URL_SWAD_CGI,Crs->HieCod,ExaCod);

	    /* Write publication date */
	    fprintf (FileRSS,"<pubDate>");
	    tm = gmtime (&CallTimeUTC);
	    Dat_WriteRFC822DateFromTM (FileRSS,tm);
	    fprintf (FileRSS,"</pubDate>\n");

	    fprintf (FileRSS,"</item>\n");
	   }
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/********* Write URL to RSS archive with active notices in a course **********/
/*****************************************************************************/

void RSS_BuildRSSLink (char RSSLink[WWW_MAX_BYTES_WWW + 1],long CrsCod)
  {
   snprintf (RSSLink,WWW_MAX_BYTES_WWW + 1,"%s/%ld/%s/%s",
             Cfg_URL_CRS_PUBLIC,CrsCod,Cfg_RSS_FOLDER,Cfg_RSS_FILE);
  }
