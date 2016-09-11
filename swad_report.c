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

static void Rep_GetAndWriteCrssOfAUsr (const struct UsrData *UsrDat,Rol_Role_t Role);
static void Rep_WriteRowCrsData (MYSQL_ROW row);

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
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_ID;
   extern const char *Txt_Email;
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
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
   extern const char *Txt_USER_in_COURSE;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_courses;
   extern const char *Txt_teachers_ABBREVIATION;
   extern const char *Txt_students_ABBREVIATION;
   unsigned NumID;
   char CtyName[Cty_MAX_BYTES_COUNTRY_NAME+1];
   struct Institution Ins;
   struct UsrFigures UsrFigures;
   struct tm FirstClickTime;
   unsigned NumFiles;
   unsigned NumPublicFiles;
   Rol_Role_t Role;
   unsigned NumCrss;

   /***** Start frame and table *****/
   Lay_StartRoundFrame ("100%",Txt_Report_of_use_of_the_platform,
                        SeeOrPrint == Rep_SEE ? Rep_PutIconToPrintMyUsageReport :
                                                NULL);
   fprintf (Gbl.F.Out,"<ul class=\"LEFT_MIDDLE\">");

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
      fprintf (Gbl.F.Out," <span class=\"%s\">%s</span>",
	       Gbl.Usrs.Me.UsrDat.IDs.List[NumID].Confirmed ? "USR_ID_C" :
						              "USR_ID_NC",
               Gbl.Usrs.Me.UsrDat.IDs.List[NumID].ID);
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

   /***** Get figures *****/
   Prf_GetUsrFigures (Gbl.Usrs.Me.UsrDat.UsrCod,&UsrFigures);

   /***** Time since first click *****/
   fprintf (Gbl.F.Out,"<li>%s: ",Txt_From_TIME);
   if (UsrFigures.FirstClickTimeUTC)
     {
      if ((gmtime_r (&UsrFigures.FirstClickTimeUTC,&FirstClickTime)) != NULL)
	{
	 fprintf (Gbl.F.Out,"%04d-%02d-%02d %02d:%02d:%02d UTC",
                  1900 + FirstClickTime.tm_year,	// year
                  1 + FirstClickTime.tm_mon,		// month
                  FirstClickTime.tm_mday,		// day of the month
                  FirstClickTime.tm_hour,		// hours
                  FirstClickTime.tm_min,		// minutes
		  FirstClickTime.tm_sec);		// seconds
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

   /***** Number of courses in which the user is student/teacher *****/
   for (Role  = Rol_STUDENT;
	Role <= Rol_TEACHER;
	Role++)
     {
      NumCrss = Usr_GetNumCrssOfUsrWithARole (Gbl.Usrs.Me.UsrDat.UsrCod,Role);
      sprintf (Gbl.Title,Txt_USER_in_COURSE,Txt_ROLES_SINGUL_Abc[Role][Gbl.Usrs.Me.UsrDat.Sex]);
      fprintf (Gbl.F.Out,"<li>%s: %u %s",
	       Gbl.Title,
	       NumCrss,
	       Txt_courses);
      if (NumCrss)
	{
	 fprintf (Gbl.F.Out," (%u %s / %u %s)",
		  Usr_GetNumUsrsInCrssOfAUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Rol_TEACHER,Rol_TEACHER),
		  Txt_teachers_ABBREVIATION,
		  Usr_GetNumUsrsInCrssOfAUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Rol_TEACHER,Rol_STUDENT),
		  Txt_students_ABBREVIATION);

         /* List my courses with this role */
	 Rep_GetAndWriteCrssOfAUsr (&Gbl.Usrs.Me.UsrDat,Role);
	}
      fprintf (Gbl.F.Out,"</li>");
     }

   fprintf (Gbl.F.Out,"</ul>");

   /***** List my courses *****/
   Crs_GetAndWriteCrssOfAUsr (&Gbl.Usrs.Me.UsrDat,Rol_TEACHER);
   Crs_GetAndWriteCrssOfAUsr (&Gbl.Usrs.Me.UsrDat,Rol_STUDENT);

   /***** End table and frame *****/
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
/************************** Write courses of a user **************************/
/*****************************************************************************/

static void Rep_GetAndWriteCrssOfAUsr (const struct UsrData *UsrDat,Rol_Role_t Role)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;

   /***** Get courses of a user from database *****/
   sprintf (Query,"SELECT degrees.DegCod,courses.CrsCod,degrees.ShortName,degrees.FullName,"
                  "courses.Year,courses.FullName,centres.ShortName"
                  " FROM crs_usr,courses,degrees,centres"
                  " WHERE crs_usr.UsrCod='%ld'"
                  " AND crs_usr.Role='%u'"
                  " AND crs_usr.CrsCod=courses.CrsCod"
                  " AND courses.DegCod=degrees.DegCod"
                  " AND degrees.CtrCod=centres.CtrCod"
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

         /* Write data of this course */
         Rep_WriteRowCrsData (row);
        }

      /* End table */
      fprintf (Gbl.F.Out,"</ol>");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Write the data of a course (result of a query) ***************/
/*****************************************************************************/

static void Rep_WriteRowCrsData (MYSQL_ROW row)
  {
   extern const char *Txt_Go_to_X;
   extern const char *Txt_YEAR_OF_DEGREE[1+Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_teachers_ABBREVIATION;
   extern const char *Txt_students_ABBREVIATION;
   struct Degree Deg;
   long CrsCod;
   unsigned NumTchs;
   unsigned NumStds;

   /*
   SELECT degrees.DegCod	0
	  courses.CrsCod	1
	  degrees.ShortName	2
	  degrees.FullName	3
	  courses.Year		4
	  courses.FullName	5
	  centres.ShortName	6
   */

   /***** Get degree code (row[0]) *****/
   if ((Deg.DegCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
      Lay_ShowErrorAndExit ("Wrong code of degree.");
   if (!Deg_GetDataOfDegreeByCod (&Deg))
      Lay_ShowErrorAndExit ("Degree not found.");

   /***** Get course code (row[1]) *****/
   if ((CrsCod = Str_ConvertStrCodToLongCod (row[1])) < 0)
      Lay_ShowErrorAndExit ("Wrong code of course.");

   /***** Get number of teachers and students in this course *****/
   NumTchs = Usr_GetNumUsrsInCrs (Rol_TEACHER,CrsCod);
   NumStds = Usr_GetNumUsrsInCrs (Rol_STUDENT,CrsCod);

   /***** Start row *****/
   fprintf (Gbl.F.Out,"<li>");

   /***** Write course full name (row[5]) *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT_N LEFT_TOP\">");
   Act_FormGoToStart (ActSeeCrsInf);
   Crs_PutParamCrsCod (CrsCod);
   sprintf (Gbl.Title,Txt_Go_to_X,row[6]);
   Act_LinkFormSubmit (Gbl.Title,"DAT_N",NULL);
   fprintf (Gbl.F.Out,"%s</a>",row[5]);
   Act_FormEnd ();

   /***** Write year (row[4]) *****/
   fprintf (Gbl.F.Out," - %s",
            Txt_YEAR_OF_DEGREE[Deg_ConvStrToYear (row[4])]);

   /***** Write degree short name (row[2])
          and centre short name (row[6]) *****/
   Act_FormGoToStart (ActSeeDegInf);
   Deg_PutParamDegCod (Deg.DegCod);
   sprintf (Gbl.Title,Txt_Go_to_X,row[2]);
   Act_LinkFormSubmit (Gbl.Title,"DAT_NOBR_N",NULL);
   fprintf (Gbl.F.Out," %s, %s"
                      "</a>",
            row[2],row[6]);
   Act_FormEnd ();

   /***** Write number of teachers / students in course *****/
   fprintf (Gbl.F.Out," (%u %s / %u %s)"
	              "</li>",
            NumTchs,Txt_teachers_ABBREVIATION,
            NumStds,Txt_students_ABBREVIATION);
  }
