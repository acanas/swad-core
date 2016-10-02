// swad_report.c: report on my use of the platform

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

// #include <stdbool.h>		// For boolean type
// #include <stdio.h>		// For sprintf
// #include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_profile.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Rep_MAX_BAR_WIDTH 90	// Maximum width of graphic bar
// #define Rep_BLOCK "&boxH;"	// HTML code for a block in graphic bar
// #define Rep_BLOCK "&blk12;"	// HTML code for a block in graphic bar
// #define Rep_BLOCK "&block;"	// HTML code for a block in graphic bar
// #define Rep_BLOCK "&equiv;"	// HTML code for a block in graphic bar
// #define Rep_BLOCK "&bull;"	// HTML code for a block in graphic bar
#define Rep_BLOCK "&squf;"	// HTML code for a block in graphic bar

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

typedef enum
  {
   Rep_SEE,
   Rep_PRINT,
  } Rep_SeeOrPrint_t;

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Rep_ShowOrPrintMyUsageReport (Rep_SeeOrPrint_t SeeOrPrint);
static void Rep_PutIconToPrintMyUsageReport (void);

static unsigned long Rep_GetMaxHitsPerYear (const char *BrowserTimeZone,
                                            time_t FirstClickTimeUTC);
static void Rep_GetAndWriteCurrentCrssOfAUsr (const struct UsrData *UsrDat,Rol_Role_t Role,
                                              const char *BrowserTimeZone,
                                              time_t FirstClickTimeUTC,
                                              struct tm *tm_FirstClickTime,
                                              unsigned long MaxHitsPerYear);
static void Rep_GetAndWriteHistoricCrssOfAUsr (const struct UsrData *UsrDat,Rol_Role_t Role,
					       const char *BrowserTimeZone,
                                               time_t FirstClickTimeUTC,
                                               struct tm *tm_FirstClickTime,
                                               unsigned long MaxHitsPerYear);
static void Rep_WriteRowCrsData (long CrsCod,Rol_Role_t Role,
                                 const char *BrowserTimeZone,
                                 time_t FirstClickTimeUTC,
                                 struct tm *tm_FirstClickTime,
                                 unsigned long MaxHitsPerYear);

static void Rep_ShowMyHitsPerYear (bool AnyCourse,long CrsCod,Rol_Role_t Role,
                                   const char *BrowserTimeZone,
                                   time_t FirstClickTimeUTC,
                                   struct tm *tm_FirstClickTime,
                                   unsigned long MaxHitsPerYear);
// static void Rep_ShowMyHitsPerMonth (time_t FirstClickTimeUTC,struct tm *tm_FirstClickTime);
static void Rep_DrawBarNumHits (float HitsNum,float HitsMax,
                                unsigned MaxBarWidth);

/*****************************************************************************/
/********* Show my usage report (report on my use of the platform) ***********/
/*****************************************************************************/

void Rep_ShowMyUsageReport (void)
  {
   Rep_ShowOrPrintMyUsageReport (false);
  }

void Rep_PrintMyUsageReport (void)
  {
   Rep_ShowOrPrintMyUsageReport (true);
  }

static void Rep_ShowOrPrintMyUsageReport (Rep_SeeOrPrint_t SeeOrPrint)
  {
   extern const char *Txt_Report_of_use_of_the_platform;
   extern const char *Txt_Personal_information;
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_ID;
   extern const char *Txt_Email;
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Figures;
   extern const char *Txt_From_TIME;
   extern const char *Txt_day;
   extern const char *Txt_days;
   extern const char *Txt_Clicks;
   extern const char *Txt_Files_uploaded;
   extern const char *Txt_file;
   extern const char *Txt_files;
   extern const char *Txt_public_FILES;
   extern const char *Txt_Downloads;
   extern const char *Txt_download;
   extern const char *Txt_downloads;
   extern const char *Txt_Forum_posts;
   extern const char *Txt_post;
   extern const char *Txt_posts;
   extern const char *Txt_Messages_sent;
   extern const char *Txt_message;
   extern const char *Txt_messages;
   extern const char *Txt_Courses;
   extern const char *Txt_Hits;
   char BrowserTimeZone[Dat_MAX_BYTES_TIME_ZONE+1];
   unsigned NumID;
   char CtyName[Cty_MAX_BYTES_COUNTRY_NAME+1];
   struct Institution Ins;
   struct UsrFigures UsrFigures;
   struct tm tm_FirstClickTime;
   unsigned NumFiles;
   unsigned NumPublicFiles;
   Rol_Role_t Role;
   unsigned long MaxHitsPerYear;

   /***** Get client time zone *****/
   Dat_GetBrowserTimeZone (BrowserTimeZone);

   /***** Start frame *****/
   if (SeeOrPrint == Rep_SEE)
      Lay_StartRoundFrame (NULL,Txt_Report_of_use_of_the_platform,
			   Rep_PutIconToPrintMyUsageReport);
   fprintf (Gbl.F.Out,"<div class=\"LEFT_TOP\" style=\"margin:10px;\">");

   /***** Personal information *****/
   fprintf (Gbl.F.Out,"<h2>%s</h2>"
	              "<ul>",
	    Txt_Personal_information);

   /***** User's name *****/
   fprintf (Gbl.F.Out,"<li>%s: <strong>%s</strong></li>",
            Txt_User[Gbl.Usrs.Me.UsrDat.Sex],
            Gbl.Usrs.Me.UsrDat.FullName);

   /***** User's ID *****/
   fprintf (Gbl.F.Out,"<li>%s:",
            Txt_ID);
   for (NumID = 0;
	NumID < Gbl.Usrs.Me.UsrDat.IDs.Num;
	NumID++)
     {
      if (NumID)
	 fprintf (Gbl.F.Out,",");
      fprintf (Gbl.F.Out," %s",Gbl.Usrs.Me.UsrDat.IDs.List[NumID].ID);
     }
   fprintf (Gbl.F.Out,"</li>");

   /***** User's e-mail *****/
   fprintf (Gbl.F.Out,"<li>%s: %s</li>",
            Txt_Email,
            Gbl.Usrs.Me.UsrDat.Email);

   /***** User's country *****/
   Cty_GetCountryName (Gbl.Usrs.Me.UsrDat.CtyCod,CtyName);
   fprintf (Gbl.F.Out,"<li>%s: %s</li>",
            Txt_Country,
            CtyName);

   /***** User's institution *****/
   Ins.InsCod = Gbl.Usrs.Me.UsrDat.InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);
   fprintf (Gbl.F.Out,"<li>%s: %s</li>",
            Txt_Institution,
            Ins.FullName);

   fprintf (Gbl.F.Out,"</ul>");

   /***** Figures *****/
   fprintf (Gbl.F.Out,"<h2>%s</h2>"
	              "<ul>",
	    Txt_Figures);

   /***** Get figures *****/
   Prf_GetUsrFigures (Gbl.Usrs.Me.UsrDat.UsrCod,&UsrFigures);

   /***** Time since first click *****/
   fprintf (Gbl.F.Out,"<li>%s: ",Txt_From_TIME);
   if (UsrFigures.FirstClickTimeUTC)
     {
      if ((gmtime_r (&UsrFigures.FirstClickTimeUTC,&tm_FirstClickTime)) != NULL)
	{
	 fprintf (Gbl.F.Out,"%04d-%02d-%02d %02d:%02d:%02d UTC",
                  1900 + tm_FirstClickTime.tm_year,	// year
                  1 + tm_FirstClickTime.tm_mon,		// month
                  tm_FirstClickTime.tm_mday,		// day of the month
                  tm_FirstClickTime.tm_hour,		// hours
                  tm_FirstClickTime.tm_min,		// minutes
		  tm_FirstClickTime.tm_sec);		// seconds
	 if (UsrFigures.NumDays > 0)
	    fprintf (Gbl.F.Out," (%d %s)",
		     UsrFigures.NumDays,
		     (UsrFigures.NumDays == 1) ? Txt_day :
						 Txt_days);
	}
     }
   else	// Time of first click is unknown
      fprintf (Gbl.F.Out,"?");
   fprintf (Gbl.F.Out,"</li>");

   /***** Number of clicks *****/
   fprintf (Gbl.F.Out,"<li>%s: ",Txt_Clicks);
   if (UsrFigures.NumClicks >= 0)
     {
      fprintf (Gbl.F.Out,"%ld",UsrFigures.NumClicks);
      if (UsrFigures.NumDays > 0)
	{
	 fprintf (Gbl.F.Out," (");
	 Str_WriteFloatNum ((float) UsrFigures.NumClicks /
			    (float) UsrFigures.NumDays);
	 fprintf (Gbl.F.Out," / %s)",Txt_day);
	}
     }
   else	// Number of clicks is unknown
      fprintf (Gbl.F.Out,"?");
   fprintf (Gbl.F.Out,"</li>");

   /***** Number of files currently published *****/
   if ((NumFiles = Brw_GetNumFilesUsr (Gbl.Usrs.Me.UsrDat.UsrCod)))
      NumPublicFiles = Brw_GetNumPublicFilesUsr (Gbl.Usrs.Me.UsrDat.UsrCod);
   else
      NumPublicFiles = 0;
   fprintf (Gbl.F.Out,"<li>"
		      "%s: %u %s (%u %s)"
		      "</li>",
	    Txt_Files_uploaded,
	    NumFiles,
	    (NumFiles == 1) ? Txt_file :
		              Txt_files,
	    NumPublicFiles,Txt_public_FILES);

   /***** Number of file views *****/
   fprintf (Gbl.F.Out,"<li>%s: ",Txt_Downloads);
   if (UsrFigures.NumFileViews >= 0)
     {
      fprintf (Gbl.F.Out,"%ld %s",
               UsrFigures.NumFileViews,
	       (UsrFigures.NumFileViews == 1) ? Txt_download :
						Txt_downloads);
      if (UsrFigures.NumDays > 0)
	{
	 fprintf (Gbl.F.Out," (");
	 Str_WriteFloatNum ((float) UsrFigures.NumFileViews /
			    (float) UsrFigures.NumDays);
	 fprintf (Gbl.F.Out," / %s)",Txt_day);
	}
     }
   else	// Number of file views is unknown
      fprintf (Gbl.F.Out,"?");
   fprintf (Gbl.F.Out,"</li>");

   /***** Number of posts in forums *****/
   fprintf (Gbl.F.Out,"<li>%s: ",Txt_Forum_posts);
   if (UsrFigures.NumForPst >= 0)
     {
      fprintf (Gbl.F.Out,"%ld %s",
	       UsrFigures.NumForPst,
	       (UsrFigures.NumForPst == 1) ? Txt_post :
					     Txt_posts);
      if (UsrFigures.NumDays > 0)
	{
	 fprintf (Gbl.F.Out," (");
	 Str_WriteFloatNum ((float) UsrFigures.NumForPst /
			    (float) UsrFigures.NumDays);
	 fprintf (Gbl.F.Out," / %s)",Txt_day);
	}
     }
   else	// Number of forum posts is unknown
      fprintf (Gbl.F.Out,"?");
   fprintf (Gbl.F.Out,"</li>");

   /***** Number of messages sent *****/
   fprintf (Gbl.F.Out,"<li>%s: ",Txt_Messages_sent);
   if (UsrFigures.NumMsgSnt >= 0)
     {
      fprintf (Gbl.F.Out,"%ld %s",
	       UsrFigures.NumMsgSnt,
	       (UsrFigures.NumMsgSnt == 1) ? Txt_message :
					     Txt_messages);
      if (UsrFigures.NumDays > 0)
	{
	 fprintf (Gbl.F.Out," (");
	 Str_WriteFloatNum ((float) UsrFigures.NumMsgSnt /
			    (float) UsrFigures.NumDays);
	 fprintf (Gbl.F.Out," / %s)",Txt_day);
	}
     }
   else	// Number of messages sent is unknown
      fprintf (Gbl.F.Out,"?");
   fprintf (Gbl.F.Out,"</li>");

   fprintf (Gbl.F.Out,"</ul>");

   /***** Current courses *****/
   fprintf (Gbl.F.Out,"<h2>%s (actuales)</h2>"	// TODO: Need translation!!!
	              "<ul>",
	    Txt_Courses);

   /* Number of courses in which the user is student/teacher */
   MaxHitsPerYear = Rep_GetMaxHitsPerYear (BrowserTimeZone,UsrFigures.FirstClickTimeUTC);
   for (Role  = Rol_STUDENT;
	Role <= Rol_TEACHER;
	Role++)
      /* List my courses with this role */
      Rep_GetAndWriteCurrentCrssOfAUsr (&Gbl.Usrs.Me.UsrDat,Role,
					BrowserTimeZone,
					UsrFigures.FirstClickTimeUTC,&tm_FirstClickTime,
					MaxHitsPerYear);

   fprintf (Gbl.F.Out,"</ul>");

   /***** Historic courses *****/
   fprintf (Gbl.F.Out,"<h2>%s (hist&oacute;rico)</h2>"	// TODO: Need translation!!!
	              "<ul>",
	    Txt_Courses);

   /* Number of courses in which the user clicked as student/teacher */
   for (Role  = Rol_STUDENT;
	Role <= Rol_TEACHER;
	Role++)
      /* List my courses with this role */
      Rep_GetAndWriteHistoricCrssOfAUsr (&Gbl.Usrs.Me.UsrDat,Role,
					 BrowserTimeZone,
					 UsrFigures.FirstClickTimeUTC,&tm_FirstClickTime,
					 MaxHitsPerYear);

   fprintf (Gbl.F.Out,"</ul>");

   /***** Global hits *****/
   fprintf (Gbl.F.Out,"<h2>%s</h2>",Txt_Hits);
   Rep_ShowMyHitsPerYear (true,-1L,Rol_UNKNOWN,
                          BrowserTimeZone,
                          UsrFigures.FirstClickTimeUTC,
                          &tm_FirstClickTime,
                          0);
   // fprintf (Gbl.F.Out,"<br />");
   // Rep_ShowMyHitsPerMonth (UsrFigures.FirstClickTimeUTC,&tm_FirstClickTime);

   /***** End frame *****/
   fprintf (Gbl.F.Out,"</div>");
   if (SeeOrPrint == Rep_SEE)
      Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/********************* Put icon to print my usage report *********************/
/*****************************************************************************/

static void Rep_PutIconToPrintMyUsageReport (void)
  {
   extern const char *Txt_Print;

   Lay_PutContextualLink (ActPrnMyUsgRep,NULL,
                          "print64x64.png",
                          Txt_Print,NULL,
		          NULL);
  }


/*****************************************************************************/
/************ Get the maximum number of hits per course-year-role ************/
/*****************************************************************************/
// Return the maximum number of hits per year

static unsigned long Rep_GetMaxHitsPerYear (const char *BrowserTimeZone,
                                            time_t FirstClickTimeUTC)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long MaxHitsPerYear = 0;

   sprintf (Query,"SELECT MAX(N) FROM (SELECT "
	          "CrsCod,"
	          "YEAR(CONVERT_TZ(ClickTime,@@session.time_zone,'%s')) AS Year,"
	          "Role,"
	          "COUNT(*) AS N"
	          " FROM log_full"
	          " WHERE ClickTime>=FROM_UNIXTIME('%ld')"
	          " AND UsrCod='%ld'"
	          " GROUP BY CrsCod,Year,Role)"
	          " AS hits_per_crs_year",
	    BrowserTimeZone,
            (long) FirstClickTimeUTC,
	    Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QuerySELECT (Query,&mysql_res,"can not get last question index");

   /***** Get number of users *****/
   row = mysql_fetch_row (mysql_res);
   if (row[0])	// There are questions
      if (sscanf (row[0],"%lu",&MaxHitsPerYear) != 1)
         Lay_ShowErrorAndExit ("Error when getting maximum hits.");

   return MaxHitsPerYear;
  }

/*****************************************************************************/
/************************** Write courses of a user **************************/
/*****************************************************************************/

static void Rep_GetAndWriteCurrentCrssOfAUsr (const struct UsrData *UsrDat,Rol_Role_t Role,
					      const char *BrowserTimeZone,
                                              time_t FirstClickTimeUTC,
                                              struct tm *tm_FirstClickTime,
                                              unsigned long MaxHitsPerYear)
  {
   extern const char *Txt_USER_in_COURSE;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_course;
   extern const char *Txt_courses;
   extern const char *Txt_teachers_ABBREVIATION;
   extern const char *Txt_students_ABBREVIATION;
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   long CrsCod;

   NumCrss = Usr_GetNumCrssOfUsrWithARole (UsrDat->UsrCod,Role);
   sprintf (Gbl.Title,Txt_USER_in_COURSE,Txt_ROLES_SINGUL_Abc[Role][Gbl.Usrs.Me.UsrDat.Sex]);
   fprintf (Gbl.F.Out,"<li>%s %u %s",
	    Gbl.Title,
	    NumCrss,
	    NumCrss == 1 ? Txt_course :
			   Txt_courses);
   if (NumCrss)
     {
      fprintf (Gbl.F.Out," (%u %s / %u %s)",
	       Usr_GetNumUsrsInCrssOfAUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Role,Rol_TEACHER),
	       Txt_teachers_ABBREVIATION,
	       Usr_GetNumUsrsInCrssOfAUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Role,Rol_STUDENT),
	       Txt_students_ABBREVIATION);

      /***** Get courses of a user from database *****/
      sprintf (Query,"SELECT courses.CrsCod,degrees.FullName,courses.Year,courses.FullName"
		     " FROM crs_usr,courses,degrees"
		     " WHERE crs_usr.UsrCod='%ld'"
		     " AND crs_usr.Role='%u'"
		     " AND crs_usr.CrsCod=courses.CrsCod"
		     " AND courses.DegCod=degrees.DegCod"
		     " ORDER BY degrees.FullName,courses.Year,courses.FullName",
	       UsrDat->UsrCod,(unsigned) Role);

      /***** List the courses (one row per course) *****/
      if ((NumCrss = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get courses of a user")))
	{
	 /* Heading row */
	 fprintf (Gbl.F.Out,"<ol>");

	 /* Write courses */
	 for (NumCrs = 1;
	      NumCrs <= NumCrss;
	      NumCrs++)
	   {
	    /* Get next course */
	    row = mysql_fetch_row (mysql_res);

	    /* Get course code (row[0]) */
	    CrsCod = Str_ConvertStrCodToLongCod (row[0]);

	    /* Write data of this course */
	    Rep_WriteRowCrsData (CrsCod,Role,
				 BrowserTimeZone,FirstClickTimeUTC,tm_FirstClickTime,
				 MaxHitsPerYear);
	   }

	 /* End table */
	 fprintf (Gbl.F.Out,"</ol>");
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   fprintf (Gbl.F.Out,"</li>");
  }

/*****************************************************************************/
/************************** Write courses of a user **************************/
/*****************************************************************************/

static void Rep_GetAndWriteHistoricCrssOfAUsr (const struct UsrData *UsrDat,Rol_Role_t Role,
					       const char *BrowserTimeZone,
                                               time_t FirstClickTimeUTC,
                                               struct tm *tm_FirstClickTime,
                                               unsigned long MaxHitsPerYear)
  {
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   long CrsCod;

   /***** Get courses of a user from database *****/
   sprintf (Query,"SELECT CrsCod,"
	          "COUNT(*) AS N"
	          " FROM log_full"
	          " WHERE UsrCod='%ld' AND Role='%u'"
                  " GROUP BY CrsCod ORDER BY N DESC",
            UsrDat->UsrCod,(unsigned) Role);

   /***** List the courses (one row per course) *****/
   if ((NumCrss = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get courses of a user")))
     {
      /* Heading row */
      sprintf (Gbl.Title,"Accesos como %s",	// TODO: Need translation
               Txt_ROLES_SINGUL_abc[Role][Gbl.Usrs.Me.UsrDat.Sex]);
      fprintf (Gbl.F.Out,"<li>%s:"
	                 "<ol>",
	       Gbl.Title);

      /* Write courses */
      for (NumCrs = 1;
	   NumCrs <= NumCrss;
	   NumCrs++)
        {
         /* Get next course */
         row = mysql_fetch_row (mysql_res);

	 /* Get course code (row[0]) */
	 CrsCod = Str_ConvertStrCodToLongCod (row[0]);

         /* Write data of this course */
         Rep_WriteRowCrsData (CrsCod,Role,
                              BrowserTimeZone,FirstClickTimeUTC,tm_FirstClickTime,
                              MaxHitsPerYear);
        }

      /* End of list */
      fprintf (Gbl.F.Out,"</ol>"
	                 "</li>");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Write the data of a course (result of a query) ***************/
/*****************************************************************************/

static void Rep_WriteRowCrsData (long CrsCod,Rol_Role_t Role,
                                 const char *BrowserTimeZone,
                                 time_t FirstClickTimeUTC,
                                 struct tm *tm_FirstClickTime,
                                 unsigned long MaxHitsPerYear)
  {
   extern const char *Txt_YEAR_OF_DEGREE[1+Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_teachers_ABBREVIATION;
   extern const char *Txt_students_ABBREVIATION;
   struct Course Crs;
   struct Degree Deg;

   /***** Get course data *****/
   Crs.CrsCod = CrsCod;
   Crs_GetDataOfCourseByCod (&Crs);

   /***** Get degree data *****/
   Deg.DegCod = Crs.DegCod;
   Deg_GetDataOfDegreeByCod (&Deg);

   /***** Start row *****/
   fprintf (Gbl.F.Out,"<li>");

   if (CrsCod > 0)	// CrsCod > 0 in log ==> course selected
     {
      if (Crs.CrsCod > 0)	// Course exists
	{
	 /***** Write course full name *****/
	 fprintf (Gbl.F.Out,"<strong>%s</strong> -",Crs.FullName);

	 /***** Write year *****/
	 if (Crs.Year)
	    fprintf (Gbl.F.Out," %s",Txt_YEAR_OF_DEGREE[Crs.Year]);

	 /***** Write degree full name *****/
	 fprintf (Gbl.F.Out," %s",Deg.FullName);

	 /***** Write number of teachers / students in course *****/
	 fprintf (Gbl.F.Out," (%u %s / %u %s)",
		  Usr_GetNumUsrsInCrs (Rol_TEACHER,Crs.CrsCod),Txt_teachers_ABBREVIATION,
		  Usr_GetNumUsrsInCrs (Rol_STUDENT,Crs.CrsCod),Txt_students_ABBREVIATION);
	}
      else
         fprintf (Gbl.F.Out,"(asignatura desconocida/eliminada)");	// TODO: Need translation
     }
   else	// CrsCod <= 0 in log ==> no course selected
      fprintf (Gbl.F.Out,"(asignatura no seleccionada)");	// TODO: Need translation

   /***** Write hits per year for this course *****/
   fprintf (Gbl.F.Out,"<br />");
   Rep_ShowMyHitsPerYear (false,CrsCod,Role,
                          BrowserTimeZone,FirstClickTimeUTC,tm_FirstClickTime,
                          MaxHitsPerYear);

   fprintf (Gbl.F.Out,"</li>");
  }

/*****************************************************************************/
/********************** Write my hits grouped by years ***********************/
/*****************************************************************************/

static void Rep_ShowMyHitsPerYear (bool AnyCourse,long CrsCod,Rol_Role_t Role,
                                   const char *BrowserTimeZone,
                                   time_t FirstClickTimeUTC,
                                   struct tm *tm_FirstClickTime,
                                   unsigned long MaxHitsPerYear)
  {
   char Query[1024];
   char SubQueryCrs[128];
   char SubQueryRol[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned long NumRow;
   unsigned ReadYear;
   unsigned LastYear;
   unsigned Year;
   struct Sta_Hits Hits;

   /***** Make the query *****/
   if (AnyCourse)
      SubQueryCrs[0] = '\0';
   else
      sprintf (SubQueryCrs," AND CrsCod='%ld'",CrsCod);

   if (Role == Rol_UNKNOWN)	// Here Rol_UNKNOWN means any role
      SubQueryRol[0] = '\0';
   else
      sprintf (SubQueryRol," AND Role='%u'",(unsigned) Role);

   sprintf (Query,"SELECT SQL_NO_CACHE "
		  "YEAR(CONVERT_TZ(ClickTime,@@session.time_zone,'%s')) AS Year,"
		  "COUNT(*) FROM log_full"
                  " WHERE ClickTime>=FROM_UNIXTIME('%ld')"
		  " AND UsrCod='%ld'%s%s"
		  " GROUP BY Year DESC",
	    BrowserTimeZone,
            (long) FirstClickTimeUTC,
	    Gbl.Usrs.Me.UsrDat.UsrCod,
	    SubQueryCrs,
	    SubQueryRol);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get clicks");

   /***** Initialize first year *****/
   Gbl.DateRange.DateIni.Date.Year = 1900 + tm_FirstClickTime->tm_year;

   /***** Initialize LastYear *****/
   LastYear = Gbl.Now.Date.Year;

   /***** Set maximum number of hits per year *****/
   if (MaxHitsPerYear)
      /* Set maximum number of hits per year from parameter */
      Hits.Max = (float) MaxHitsPerYear;
   else
     {
      /* Compute maximum number of hits per year */
      Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,1,1);
      mysql_data_seek (mysql_res,0);
     }

   /***** Write rows *****/
   for (NumRow = 1;
	NumRow <= NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get the year (in row[0] is the date in YYYY format) */
      if (sscanf (row[0],"%04u",&ReadYear) != 1)
	 Lay_ShowErrorAndExit ("Wrong date.");

      /* Get number hits (in row[1]) */
      Hits.Num = Str_GetFloatNumFromStr (row[1]);

      for (Year = LastYear;
	   Year >= ReadYear;
	   Year--)
        {
         /* Write the year */
         fprintf (Gbl.F.Out,"%04u ",Year);

         /* Draw bar proportional to number of hits */
         Rep_DrawBarNumHits (Year == LastYear ? Hits.Num :
                        	                0.0,
                             Hits.Max,Rep_MAX_BAR_WIDTH);
        }
      LastYear = Year;
     }

   /***** Finally, show the oldest years without clicks *****/
   for (Year = LastYear;
        Year >= Gbl.DateRange.DateIni.Date.Year;
        Year--)
     {
      /* Write the year */
      fprintf (Gbl.F.Out,"%04u ",Year);

      /* Draw bar proportional to number of hits */
      Rep_DrawBarNumHits (0.0,Hits.Max,Rep_MAX_BAR_WIDTH);
     }
  }

/*****************************************************************************/
/********************* Write my hits grouped by months ***********************/
/*****************************************************************************/
/*
static void Rep_ShowMyHitsPerMonth (time_t FirstClickTimeUTC,struct tm *tm_FirstClickTime)
  {
   char BrowserTimeZone[Dat_MAX_BYTES_TIME_ZONE+1];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned long NumRow;
   struct Date ReadDate;
   struct Date LastDate;
   struct Date Date;
   unsigned M;
   unsigned NumMonthsBetweenLastDateAndCurrentDate;
   struct Sta_Hits Hits;

   ***** Get client time zone *****
   Dat_GetBrowserTimeZone (BrowserTimeZone);

   ***** Make the query *****
   sprintf (Query,"SELECT SQL_NO_CACHE "
		  "DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%Y%%m') AS Month,"
		  "COUNT(*) FROM log_full"
                  " WHERE ClickTime>=FROM_UNIXTIME('%ld')"
		  " AND UsrCod='%ld'"
		  " GROUP BY Month DESC",
	    BrowserTimeZone,
            (long) FirstClickTimeUTC,
	    Gbl.Usrs.Me.UsrDat.UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get clicks");

   ***** Initialize first date *****
   Gbl.DateRange.DateIni.Date.Year  = 1900 + tm_FirstClickTime->tm_year;
   Gbl.DateRange.DateIni.Date.Month = 1 + tm_FirstClickTime->tm_mon;
   Gbl.DateRange.DateIni.Date.Day   = tm_FirstClickTime->tm_mday;

   ***** Initialize LastDate *****
   Dat_AssignDate (&LastDate,&Gbl.Now.Date);

   ***** Compute maximum number of hits per month *****
   Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,1,1);

   ***** Write rows *****
   mysql_data_seek (mysql_res,0);
   for (NumRow = 1;
	NumRow <= NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      * Get the year and the month (in row[0] is the date in YYYYMM format) *
      if (sscanf (row[0],"%04u%02u",&ReadDate.Year,&ReadDate.Month) != 2)
	 Lay_ShowErrorAndExit ("Wrong date.");

      * Get number hits (in row[1]) *
      Hits.Num = Str_GetFloatNumFromStr (row[1]);

      Dat_AssignDate (&Date,&LastDate);
      NumMonthsBetweenLastDateAndCurrentDate = Dat_GetNumMonthsBetweenDates (&ReadDate,&LastDate);
      for (M = 1;
	   M <= NumMonthsBetweenLastDateAndCurrentDate;
	   M++)
        {
         * Write the month *
         fprintf (Gbl.F.Out,"%04u-%02u ",Date.Year,Date.Month);

         * Draw bar proportional to number of hits *
         Rep_DrawBarNumHits (M == NumMonthsBetweenLastDateAndCurrentDate ? Hits.Num :
                        	                                           0.0,
                             Hits.Max,Rep_MAX_BAR_WIDTH);

         * Decrease month *
         Dat_GetMonthBefore (&Date,&Date);
        }
      Dat_AssignDate (&LastDate,&Date);
     }

  ***** Finally, show the oldest months without clicks *****
  NumMonthsBetweenLastDateAndCurrentDate = Dat_GetNumMonthsBetweenDates (&Gbl.DateRange.DateIni.Date,&LastDate);
  for (M = 1;
       M <= NumMonthsBetweenLastDateAndCurrentDate;
       M++)
    {
     * Write the month *
     fprintf (Gbl.F.Out,"%04u-%02u ",Date.Year,Date.Month);

     * Draw bar proportional to number of hits *
     Rep_DrawBarNumHits (0.0,Hits.Max,Rep_MAX_BAR_WIDTH);

     * Decrease month *
     Dat_GetMonthBefore (&Date,&Date);
    }
  }
*/
/*****************************************************************************/
/********************* Draw a bar with the number of hits ********************/
/*****************************************************************************/

static void Rep_DrawBarNumHits (float HitsNum,float HitsMax,
                                unsigned MaxBarWidth)
  {
   unsigned BarWidth;
   unsigned i;

   if (HitsNum != 0.0)
     {
      /***** Draw bar with a with proportional to the number of hits *****/
      BarWidth = (unsigned) (((HitsNum * (float) MaxBarWidth) / HitsMax) + 0.5);
      for (i = 0;
	   i < BarWidth;
	   i++)
         fprintf (Gbl.F.Out,Rep_BLOCK);

      /***** Write the number of hits *****/
      fprintf (Gbl.F.Out," ");
      Str_WriteFloatNum (HitsNum);
     }
   else
      /***** Write the number of clicks *****/
      fprintf (Gbl.F.Out,"0");

   fprintf (Gbl.F.Out,"<br />");
  }
