// swad_RSS.c: RSS feeds

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdio.h>		// For asprintf
#include <string.h>

#include "swad_changelog.h"
#include "swad_database.h"
#include "swad_exam.h"
#include "swad_global.h"
#include "swad_notice.h"
#include "swad_RSS.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void RSS_WriteNotices (FILE *FileRSS,struct Course *Crs);
static void RSS_WriteExamAnnouncements (FILE *FileRSS,struct Course *Crs);

/*****************************************************************************/
/******* Update RSS archive with active notices in the current course ********/
/*****************************************************************************/

void RSS_UpdateRSSFileForACrs (struct Course *Crs)
  {
   char PathRelPublRSSDir[PATH_MAX + 1];
   char PathRelPublRSSFile[PATH_MAX + 1];
   FILE *FileRSS;
   struct tm *tm;

   /***** Create RSS directory if not exists *****/
   snprintf (PathRelPublRSSDir,sizeof (PathRelPublRSSDir),
	     "%s/%s/%ld/%s",
	     Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CRS,Crs->CrsCod,Cfg_RSS_FOLDER);
   Fil_CreateDirIfNotExists (PathRelPublRSSDir);

   /***** Create RSS file *****/
   snprintf (PathRelPublRSSFile,sizeof (PathRelPublRSSFile),
	     "%s/%s",
	     PathRelPublRSSDir,Cfg_RSS_FILE);
   if ((FileRSS = fopen (PathRelPublRSSFile,"wb")) == NULL)
      Lay_ShowErrorAndExit ("Can not create RSS file.");

   /***** Write RSS header *****/
   fprintf (FileRSS,"<?xml version=\"1.0\" encoding=\"windows-1252\"?>\n");
   fprintf (FileRSS,"<rss version=\"2.0\" xmlns:atom=\"http://www.w3.org/2005/Atom\">\n");

   /***** Write channel header *****/
   fprintf (FileRSS,"<channel>\n");

   fprintf (FileRSS,"<atom:link href=\"");
   RSS_WriteRSSLink (FileRSS,Crs->CrsCod);
   fprintf (FileRSS,"\" rel=\"self\" type=\"application/rss+xml\" />\n");

   fprintf (FileRSS,"<title>%s: %s</title>\n",
            Cfg_PLATFORM_SHORT_NAME,Crs->ShrtName);
   fprintf (FileRSS,"<link>%s/?crs=%ld</link>\n",
            Cfg_URL_SWAD_CGI,Crs->CrsCod);
   fprintf (FileRSS,"<description>%s</description>\n",
            Crs->FullName);
   fprintf (FileRSS,"<language>%s</language>\n",
            Cfg_RSS_LANGUAGE);
   fprintf (FileRSS,"<webMaster>%s (%s)</webMaster>\n",
            Cfg_PLATFORM_RESPONSIBLE_EMAIL,Cfg_PLATFORM_RESPONSIBLE_NAME);

   fprintf (FileRSS,"<image>\n");
   fprintf (FileRSS,"<url>%s/swad112x32.png</url>\n",
            Gbl.Prefs.IconsURL);
   fprintf (FileRSS,"<title>%s: %s</title>\n",
            Cfg_PLATFORM_SHORT_NAME,Crs->ShrtName);
   fprintf (FileRSS,"<link>%s/?crs=%ld</link>\n",
            Cfg_URL_SWAD_CGI,Crs->CrsCod);
   fprintf (FileRSS,"<width>112</width>\n");
   fprintf (FileRSS,"<height>32</height>\n");
   fprintf (FileRSS,"</image>\n");

   // All date-times in RSS conform to the Date and Time Specification of RFC 822, with the exception that the year may be expressed with two characters or four characters (four preferred)
   fprintf (FileRSS,"<pubDate>");	
   tm = gmtime (&Gbl.StartExecutionTimeUTC);
   Dat_WriteRFC822DateFromTM (FileRSS,tm);
   fprintf (FileRSS,"</pubDate>\n");

   fprintf (FileRSS,"<lastBuildDate>");	
   tm = gmtime (&Gbl.StartExecutionTimeUTC);
   Dat_WriteRFC822DateFromTM (FileRSS,tm);
   fprintf (FileRSS,"</lastBuildDate>\n");

   fprintf (FileRSS,"<generator>%s</generator>\n",
            Log_PLATFORM_VERSION);
   fprintf (FileRSS,"<docs>http://www.rssboard.org/rss-specification</docs>\n");

   /***** Write notices as RSS items *****/
   RSS_WriteNotices (FileRSS,Crs);

   /***** Write exam announcements as RSS items *****/
   RSS_WriteExamAnnouncements (FileRSS,Crs);

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

static void RSS_WriteNotices (FILE *FileRSS,struct Course *Crs)
  {
   extern const char *Txt_Notice;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct UsrData UsrDat;
   struct tm *tm;
   time_t CreatTimeUTC;
   long NotCod;
   unsigned long NumNot,NumNotices;
   char Content[Cns_MAX_BYTES_TEXT + 1];

   /***** Get active notices in course *****/
   DB_BuildQuery ("SELECT NotCod,UNIX_TIMESTAMP(CreatTime) AS T,UsrCod,Content"
		  " FROM notices"
		  " WHERE CrsCod=%ld AND Status=%u"
		  " ORDER BY T DESC",
                  Crs->CrsCod,(unsigned) Not_ACTIVE_NOTICE);
   NumNotices = DB_QuerySELECT_new (&mysql_res,"can not get notices from database");

   /***** Write items with notices *****/
   if (NumNotices)
     {
      Usr_UsrDataConstructor (&UsrDat);

      for (NumNot = 0;
	   NumNot < NumNotices;
	   NumNot++)
        {
         /***** Get data of the notice *****/
         row = mysql_fetch_row (mysql_res);

         /* Get notice code */
         if (sscanf (row[0],"%ld",&NotCod) != 1)
            Lay_ShowErrorAndExit ("Wrong code of notice.");

         /* Get UTC date-time of publication */
         CreatTimeUTC = 0L;
         if (row[1])
            sscanf (row[1],"%ld",&CreatTimeUTC);

         /* Get author */
         UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[2]);
         Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat); // Get from the database the data of the autor

         /***** Write item with notice *****/
         fprintf (FileRSS,"<item>\n");

         /* Write title (first characters) of the notice */
         Str_Copy (Content,row[3],
                   Cns_MAX_BYTES_TEXT);
         Str_LimitLengthHTMLStr (Content,40);	// Remove when notice has a Subject
         fprintf (FileRSS,"<title>%s: ",Txt_Notice);
         Str_FilePrintStrChangingBRToRetAndNBSPToSpace (FileRSS,Content);
         fprintf (FileRSS,"</title>\n");

         /* Write link to the notice */
         fprintf (FileRSS,"<link>%s/?crs=%ld</link>\n",
                  Cfg_URL_SWAD_CGI,Crs->CrsCod);

         /* Write full content of the notice */
         Str_Copy (Content,row[3],
                   Cns_MAX_BYTES_TEXT);
         Str_InsertLinks (Content,Cns_MAX_BYTES_TEXT,40);
         fprintf (FileRSS,"<description><![CDATA[<p><em>%s %s %s:</em></p><p>%s</p>]]></description>\n",
                  UsrDat.FirstName,UsrDat.Surname1,UsrDat.Surname2,Content);

         /* Write author */
         if (UsrDat.Email[0])
            fprintf (FileRSS,"<author>%s (%s %s %s)</author>\n",
                     UsrDat.Email,UsrDat.FirstName,UsrDat.Surname1,UsrDat.Surname2);

         /* Write unique string for this item */
         fprintf (FileRSS,"<guid isPermaLink=\"false\">%s, course #%ld, notice #%ld</guid>\n",
                  Cfg_URL_SWAD_CGI,Crs->CrsCod,NotCod);

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
/******** Write exam announcements of a course as items of RSS file **********/
/*****************************************************************************/

static void RSS_WriteExamAnnouncements (FILE *FileRSS,struct Course *Crs)
  {
   extern const char *Txt_Exam;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct UsrData UsrDat;
   struct tm *tm;
   time_t CallTimeUTC;
   long ExaCod;
   unsigned long NumExa,NumExamAnnouncements;

   if (Gbl.DB.DatabaseIsOpen)
     {
      /***** Get exam announcements (only future exams) in current course from database *****/
      DB_BuildQuery ("SELECT ExaCod,UNIX_TIMESTAMP(CallDate) AS T,"
		     "DATE_FORMAT(ExamDate,'%%d/%%m/%%Y %%H:%%i')"
		     " FROM exam_announcements"
		     " WHERE CrsCod=%ld AND Status=%u AND ExamDate>=NOW()"
		     " ORDER BY T",
	             Gbl.CurrentCrs.Crs.CrsCod,
	             (unsigned) Exa_VISIBLE_EXAM_ANNOUNCEMENT);
      NumExamAnnouncements = DB_QuerySELECT_new (&mysql_res,"can not get exam announcements");

      /***** Write items with notices *****/
      if (NumExamAnnouncements)
	{
	 Usr_UsrDataConstructor (&UsrDat);

	 for (NumExa = 0;
	      NumExa < NumExamAnnouncements;
	      NumExa++)
	   {
	    /***** Get data of the exam announcement *****/
	    row = mysql_fetch_row (mysql_res);

	    /* Get exam announcement code */
	    if (sscanf (row[0],"%ld",&ExaCod) != 1)
	       Lay_ShowErrorAndExit ("Wrong code of exam announcement.");

	    /* Get UTC date-time of publication */
	    CallTimeUTC = 0L;
	    if (row[1])
	       sscanf (row[1],"%ld",&CallTimeUTC);

	    /***** Write item with exam announcement *****/
	    fprintf (FileRSS,"<item>\n");

	    /* Write title (exam date) */
	    fprintf (FileRSS,"<title>%s: %s</title>\n",Txt_Exam,row[2]);

	    /* Write link to the notice */
	    fprintf (FileRSS,"<link>%s/?crs=%ld</link>\n",
		     Cfg_URL_SWAD_CGI,Crs->CrsCod);

	    /* Write full content of the exam announcement */
	    fprintf (FileRSS,"<description><![CDATA[<p><em>Fecha examen: %s</em></p>]]></description>\n",
		     row[2]);

	    /* Write unique string for this item */
	    fprintf (FileRSS,"<guid isPermaLink=\"false\">%s, course #%ld, exam #%ld</guid>\n",
		     Cfg_URL_SWAD_CGI,Crs->CrsCod,ExaCod);

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

void RSS_WriteRSSLink (FILE *FileTgt,long CrsCod)
  {
   fprintf (FileTgt,"%s/%s/%ld/%s/%s",
            Cfg_URL_SWAD_PUBLIC,Cfg_FOLDER_CRS,CrsCod,Cfg_RSS_FOLDER,Cfg_RSS_FILE);
  }
