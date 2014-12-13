// swad_course.c: edition of courses

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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

#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <limits.h>		// For maximum values
#include <stdlib.h>		// For getenv, etc.
#include <string.h>		// For string functions

#include "swad_course.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_degree.h"
#include "swad_enrollment.h"
#include "swad_exam.h"
#include "swad_global.h"
#include "swad_indicator.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_RSS.h"
#include "swad_theme.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;
extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];

/*****************************************************************************/
/*************************** Public constants ********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private types *********************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

#define Crs_MAX_LENGTH_FULL_NAME_COURSE_ON_LIST_OF_MY_COURSES	60

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Crs_Configuration (bool PrintView);
static void Crs_PutFormToConfigLogIn (bool IsForm);

static void Crs_WriteListMyCoursesToSelectOne (void);

static void Crs_GetListCoursesInDegree (Crs_WhatCourses_t WhatCourses);
static void Crs_ListCourses (void);
static void Crs_EditCourses (void);
static void Crs_ListCoursesForSeeing (void);
static void Crs_ListCoursesForEdition (void);
static bool Crs_CheckIfICanEdit (struct Course *Crs);
static Crs_StatusTxt_t Crs_GetStatusTxtFromStatusBits (Crs_Status_t Status);
static Crs_Status_t Crs_GetStatusBitsFromStatusTxt (Crs_StatusTxt_t StatusTxt);
static void Crs_PutFormToCreateCourse (void);
static void Crs_PutHeadCoursesForSeeing (void);
static void Crs_PutHeadCoursesForEdition (void);
static void Crs_RecFormRequestOrCreateCrs (unsigned Status);
static void Crs_GetParamsNewCourse (struct Course *Crs);
static bool Crs_CheckIfCourseNameExistsInCourses (long DegCod,unsigned Year,const char *FieldName,const char *Name,long CrsCod);
static void Crs_CreateCourse (struct Course *Crs,unsigned Status);
static void Crs_GetDataOfCourseFromRow (struct Course *Crs,MYSQL_ROW row);
static void Crs_EmptyCourseCompletely (long CrsCod);
static bool Crs_RenameCourse (struct Course *Crs,Cns_ShortOrFullName_t ShortOrFullName);
static void Crs_PutLinkToGoToCrs (struct Course *Crs);
static void Usr_PutFormToSearchCourses (void);

static void Crs_PutParamOtherCrsCod (long CrsCod);
static long Crs_GetParamOtherCrsCod (void);

static void Crs_WriteRowCrsData (unsigned NumCrs,MYSQL_ROW row,bool WriteColumnAccepted);

/*****************************************************************************/
/***************** Show introduction to the current course *******************/
/*****************************************************************************/

void Crs_ShowIntroduction (void)
  {
   /***** Course configuration *****/
   fprintf (Gbl.F.Out,"<div align=\"center\" style=\"margin-bottom:20px;\">");
   Crs_Configuration (false);
   fprintf (Gbl.F.Out,"</div>");

   /***** Course introduction *****/
   Inf_ShowInfo ();
  }

/*****************************************************************************/
/***************** Print configuration of the current course *****************/
/*****************************************************************************/

void Crs_PrintConfiguration (void)
  {
   Crs_Configuration (true);
  }

/*****************************************************************************/
/***************** Configuration of the current course ***********************/
/*****************************************************************************/

static void Crs_Configuration (bool PrintView)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_This_course_fulfills_X_out_of_Y_indicators_;
   extern const char *Txt_TABS_FULL_TXT[Act_NUM_TABS];
   extern const char *Txt_MENU_NO_BR[Act_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];
   extern const char *Txt_Show_more_details;
   extern const char *Txt_NO;
   extern const char *Txt_Course;
   extern const char *Txt_Short_Name;
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_YEAR_OF_DEGREE[1+Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_Semester;
   extern const char *Txt_SEMESTER_OF_YEAR[1+2];
   extern const char *Txt_Institutional_code;
   extern const char *Txt_Internal_code;
   extern const char *Txt_Shortcut_to_this_course;
   extern const char *Txt_QR_code;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Indicators;
   extern const char *Txt_of_PART_OF_A_TOTAL;
   extern const char *Txt_Save;
   Act_Action_t Superaction;
   unsigned Year;
   unsigned Semester;
   struct Ind_IndicatorsCrs Indicators;
   bool IsForm = (!PrintView && Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_TEACHER);
   bool PutLink = !PrintView && Gbl.CurrentDeg.Deg.WWW[0];

   /***** Messages and links above the frame *****/
   if (!PrintView)
     {
      /* Get indicators and show warning */
      Ind_GetIndicatorsCrs (Gbl.CurrentCrs.Crs.CrsCod,&Indicators);
      if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER &&
	  Indicators.CountIndicators < Ind_NUM_INDICATORS)
	{
	 /* Warning alert */
	 Superaction = Act_Actions[ActSeeAllStaCrs].SuperAction;
	 sprintf (Gbl.Message,Txt_This_course_fulfills_X_out_of_Y_indicators_,
		  Indicators.CountIndicators,Ind_NUM_INDICATORS,
		  Ind_NUM_INDICATORS,
		  Cfg_HTTPS_URL_SWAD_CGI,Gbl.CurrentCrs.Crs.CrsCod,Act_Actions[ActSeeAllStaCrs].ActCod,
		  Txt_TABS_FULL_TXT[Act_Actions[Superaction].Tab],
		  Txt_MENU_NO_BR[Act_Actions[Superaction].Tab][Act_Actions[Superaction].IndexInMenu],
		  Txt_Show_more_details,
		  Txt_NO);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	}

       /* Link to print view */
      fprintf (Gbl.F.Out,"<div align=\"center\">");
      Lay_PutLinkToPrintView1 (ActPrnCrsInf);
      Lay_PutLinkToPrintView2 ();
      fprintf (Gbl.F.Out,"</div>");

      /* Start form */
      if (IsForm)
         Act_FormStart (ActChgCrsLog);
     }

   /***** Start frame *****/
   Lay_StartRoundFrameTable10 (NULL,2,NULL);

   /***** Title *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td colspan=\"2\" align=\"center\" class=\"TITLE_LOCATION\">");
   if (PutLink)
      fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\""
	                 " class=\"TITLE_LOCATION\" title=\"%s\">",
	       Gbl.CurrentDeg.Deg.WWW,
	       Gbl.CurrentDeg.Deg.FullName);
   fprintf (Gbl.F.Out,"<img src=\"%s/%s/%s64x64.gif\""
	              " alt=\"%s\" class=\"ICON64x64\" />",
	    Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_DEGREES,
	    Gbl.CurrentDeg.Deg.Logo,
	    Gbl.CurrentDeg.Deg.ShortName);
   if (PutLink)
      fprintf (Gbl.F.Out,"</a>");
   fprintf (Gbl.F.Out,"<br />%s"
                      "</td>"
		      "</tr>",
            Gbl.CurrentCrs.Crs.FullName);

   /***** Course full name *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"right\" valign=\"middle\" class=\"%s\">%s:</td>"
                      "<td align=\"left\" valign=\"middle\" class=\"DAT_N\">%s</td>"
                      "</tr>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Course,
            Gbl.CurrentCrs.Crs.FullName);

   /***** Course short name *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"right\" valign=\"middle\" class=\"%s\">%s:</td>"
                      "<td align=\"left\" valign=\"middle\" class=\"DAT\">%s</td>"
                      "</tr>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Short_Name,
            Gbl.CurrentCrs.Crs.ShortName);

   /***** Course year *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"right\" valign=\"middle\" class=\"%s\">%s:</td>"
                      "<td align=\"left\" valign=\"middle\" class=\"DAT\">",
           The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Year_OF_A_DEGREE);
   if (IsForm)
     {
      fprintf (Gbl.F.Out,"<select name=\"OthCrsYear\">");
      for (Year = 0;
	   Year <= Gbl.CurrentDeg.Deg.LastYear;
           Year++)
         if (Deg_CheckIfYearIsValidInDeg (Year,&Gbl.CurrentDeg.Deg))
            fprintf (Gbl.F.Out,"<option value=\"%u\"%s>%s</option>",
                     Year,
                     Year == Gbl.CurrentCrs.Crs.Year ? " selected=\"selected\"" :
                	                               "",
                     Txt_YEAR_OF_DEGREE[Year]);
      fprintf (Gbl.F.Out,"</select>");
     }
   else
      fprintf (Gbl.F.Out,"%s",Txt_YEAR_OF_DEGREE[Gbl.CurrentCrs.Crs.Year]);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Course semester *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"right\" valign=\"middle\" class=\"%s\">%s:</td>"
                      "<td align=\"left\" valign=\"middle\" class=\"DAT\">",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Semester);
   if (IsForm)
     {
      fprintf (Gbl.F.Out,"<select name=\"OthCrsSem\">");
      for (Semester = 0;
	   Semester <= 2;
	   Semester++)
         fprintf (Gbl.F.Out,"<option value=\"%u\"%s>%s</option>",
                  Semester,
                  Semester == Gbl.CurrentCrs.Crs.Semester ? " selected=\"selected\"" :
                	                                    "",
                  Txt_SEMESTER_OF_YEAR[Semester]);
      fprintf (Gbl.F.Out,"</select>");
     }
   else
      fprintf (Gbl.F.Out,"%s",Txt_SEMESTER_OF_YEAR[Gbl.CurrentCrs.Crs.Semester]);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   if (!PrintView)
     {
      /***** Institutional code of the course *****/
      fprintf (Gbl.F.Out,"<tr>"
                         "<td align=\"right\" valign=\"middle\" class=\"%s\">%s:</td>"
                         "<td align=\"left\" valign=\"middle\" class=\"DAT\">",
              The_ClassFormul[Gbl.Prefs.Theme],
              Txt_Institutional_code);
      if (IsForm)
         fprintf (Gbl.F.Out,"<input type=\"text\" name=\"InsCrsCod\" size=\"%u\" maxlength=\"%u\" value=\"%s\" />",
                  Crs_LENGTH_INSTITUTIONAL_CRS_COD,
                  Crs_LENGTH_INSTITUTIONAL_CRS_COD,
               Gbl.CurrentCrs.Crs.InstitutionalCrsCod);
      else
         fprintf (Gbl.F.Out,"%s",Gbl.CurrentCrs.Crs.InstitutionalCrsCod);
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");

      /***** Internal code of the course *****/
      fprintf (Gbl.F.Out,"<tr>"
                         "<td align=\"right\" valign=\"middle\" class=\"%s\">%s:</td>"
                         "<td align=\"left\" valign=\"middle\" class=\"DAT\">%ld</td>"
                         "</tr>",
              The_ClassFormul[Gbl.Prefs.Theme],Txt_Internal_code,
              Gbl.CurrentCrs.Crs.CrsCod);
     }

   /***** Link to the course *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"right\" valign=\"middle\" class=\"%s\">%s:</td>"
                      "<td align=\"left\" valign=\"middle\" class=\"DAT\">"
                      "<a href=\"%s/?CrsCod=%ld\" class=\"DAT\" target=\"_blank\">%s/?CrsCod=%ld</a>"
                      "</td>"
                      "</tr>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Shortcut_to_this_course,
            Cfg_HTTPS_URL_SWAD_CGI,Gbl.CurrentCrs.Crs.CrsCod,
            Cfg_HTTPS_URL_SWAD_CGI,Gbl.CurrentCrs.Crs.CrsCod);

   if (PrintView)
     {
      /***** QR code with link to the course *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td align=\"right\" valign=\"middle\" class=\"%s\">%s:</td>"
			 "<td align=\"left\" valign=\"middle\" class=\"DAT\">",
	       The_ClassFormul[Gbl.Prefs.Theme],
	       Txt_QR_code);
      QR_LinkToCourse (200);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }
   else
     {
      /***** Choice whether a course allows direct log in *****/
      if (Cfg_EXTERNAL_LOGIN_SERVICE_SHORT_NAME[0])	// If external login service exists
	 Crs_PutFormToConfigLogIn (IsForm);

      /***** Number of teachers *****/
      fprintf (Gbl.F.Out,"<tr>"
                         "<td align=\"right\" valign=\"middle\" class=\"%s\">%s:</td>"
                         "<td align=\"left\" valign=\"middle\" class=\"DAT\">%u</td>"
                         "</tr>",
               The_ClassFormul[Gbl.Prefs.Theme],
               Txt_ROLES_PLURAL_Abc[Rol_ROLE_TEACHER][Usr_SEX_UNKNOWN],Gbl.CurrentCrs.Crs.NumTchs);

      /***** Number of students *****/
      fprintf (Gbl.F.Out,"<tr>"
                         "<td align=\"right\" valign=\"middle\" class=\"%s\">%s:</td>"
                         "<td align=\"left\" valign=\"middle\" class=\"DAT\">%u</td>"
                         "</tr>",
               The_ClassFormul[Gbl.Prefs.Theme],
               Txt_ROLES_PLURAL_Abc[Rol_ROLE_STUDENT][Usr_SEX_UNKNOWN],Gbl.CurrentCrs.Crs.NumStds);

      /***** Indicators *****/
      fprintf (Gbl.F.Out,"<tr>"
                         "<td align=\"right\" valign=\"middle\" class=\"%s\">%s:</td>"
                         "<td align=\"left\" valign=\"middle\" class=\"DAT\">"
                         "<a href=\"%s/?CrsCod=%ld&amp;ActCod=%ld\" target=\"_blank\" class=\"DAT\">%u %s %u "
                         "<img src=\"%s/%s16x16.gif\" alt=\"\""
                         " class=\"ICON16x16\" style=\"vertical-align:top;\"/>"
                         "</a>"
                         "</td>"
                         "</tr>",
               The_ClassFormul[Gbl.Prefs.Theme],Txt_Indicators,
               Cfg_HTTPS_URL_SWAD_CGI,Gbl.CurrentCrs.Crs.CrsCod,Act_Actions[ActReqStaCrs].ActCod,
               Indicators.CountIndicators,Txt_of_PART_OF_A_TOTAL,Ind_NUM_INDICATORS,
               Gbl.Prefs.IconsURL,
               (Indicators.CountIndicators == Ind_NUM_INDICATORS) ? "ok_green" :
        	                                                    "warning");
     }

   /***** End of the frame *****/
   Lay_EndRoundFrameTable10 ();

   /***** End form *****/
   if (IsForm)
     {
      Lay_PutSendButton (Txt_Save);
      fprintf (Gbl.F.Out,"</form>");
     }
  }

/*****************************************************************************/
/********* Put form to choice whether a course allows direct log in **********/
/*****************************************************************************/

static void Crs_PutFormToConfigLogIn (bool IsForm)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Students_authentication;
   extern const char *Txt_STUDENTS_must_enter_through_X;
   extern const char *Txt_STUDENTS_may_enter_directly;

   /***** Can students enter directly? *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td align=\"right\" valign=\"middle\" class=\"%s\">%s:</td>"
		      "<td align=\"left\" valign=\"middle\">",
	    The_ClassFormul[Gbl.Prefs.Theme],Txt_Students_authentication);

   fprintf (Gbl.F.Out,"<input type=\"radio\" name=\"AllowDirectLogIn\" value=\"N\"");
   if (!Gbl.CurrentDegTyp.DegTyp.AllowDirectLogIn &&
       !Gbl.CurrentCrs.Crs.AllowDirectLogIn)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   if (!IsForm || Gbl.CurrentDegTyp.DegTyp.AllowDirectLogIn)
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out," /><span class=\"DAT\">");
   fprintf (Gbl.F.Out,Txt_STUDENTS_must_enter_through_X,
	    Cfg_EXTERNAL_LOGIN_SERVICE_SHORT_NAME);
   fprintf (Gbl.F.Out,"<br /></span>");

   fprintf (Gbl.F.Out,"<input type=\"radio\" name=\"AllowDirectLogIn\" value=\"Y\"");
   if (Gbl.CurrentDegTyp.DegTyp.AllowDirectLogIn ||
       Gbl.CurrentCrs.Crs.AllowDirectLogIn)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   if (!IsForm || Gbl.CurrentDegTyp.DegTyp.AllowDirectLogIn)
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out," /><span class=\"DAT\">%s</span>",
	    Txt_STUDENTS_may_enter_directly);

   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");
  }

/*****************************************************************************/
/******************** Change the configuration of a course *******************/
/*****************************************************************************/

void Crs_ChangeCourseConfig (void)
  {
   extern const char *Txt_The_configuration_of_the_course_X_has_been_updated;
   char Query[512];
   char YearStr[2+1];
   char YN[1+1];
   char SemesterStr[1+1];

   /***** Get parameters from form *****/
   /* Get institutional code */
   Par_GetParToText ("InsCrsCod",Gbl.CurrentCrs.Crs.InstitutionalCrsCod,Crs_LENGTH_INSTITUTIONAL_CRS_COD);

   /* Get year */
   Par_GetParToText ("OthCrsYear",YearStr,2);
   Gbl.CurrentCrs.Crs.Year = Deg_ConvStrToYear (YearStr);

   /* Get semester */
   Par_GetParToText ("OthCrsSem",SemesterStr,1);
   Gbl.CurrentCrs.Crs.Semester = Deg_ConvStrToSemester (SemesterStr);

   /* Get whether this course allows direct log in or not */
   Par_GetParToText ("AllowDirectLogIn",YN,1);
   Gbl.CurrentCrs.Crs.AllowDirectLogIn = (Str_ConvertToUpperLetter (YN[0]) == 'Y');

   /***** Update table of degree types *****/
   sprintf (Query,"UPDATE courses SET InsCrsCod='%s',Year='%u',Semester='%u',AllowDirectLogIn='%c'"
                  " WHERE CrsCod='%ld'",
            Gbl.CurrentCrs.Crs.InstitutionalCrsCod,
            Gbl.CurrentCrs.Crs.Year,
            Gbl.CurrentCrs.Crs.Semester,
            Gbl.CurrentCrs.Crs.AllowDirectLogIn ? 'Y' :
        	                                  'N',
            Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not update the type of log in for a course");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_The_configuration_of_the_course_X_has_been_updated,
            Gbl.CurrentCrs.Crs.ShortName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show the form again *****/
   Crs_ShowIntroduction ();
  }

/*****************************************************************************/
/************************ Write menu with my courses *************************/
/*****************************************************************************/

static void Crs_WriteListMyCoursesToSelectOne (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_My_courses;
   extern const char *Txt_Go_to_X;
   unsigned NumMyCrs;
   struct Degree Deg;
   struct Course Crs;
   char CourseFullName[Crs_MAX_LENGTH_COURSE_FULL_NAME+1];	// Full name of course
   char PathRelRSSFile[PATH_MAX+1];

   /***** Start form *****/
   Lay_StartRoundFrameTable10 (NULL,0,Txt_My_courses);
   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"left\">"
                      "<ul style=\"list-style-type:none; padding:0; margin:0;\">");

   /***** Write an option for each of my courses *****/
   for (NumMyCrs = 0;
	NumMyCrs < Gbl.Usrs.Me.MyCourses.Num;
	NumMyCrs++)
     {
      /* Get data of this course */
      Crs.CrsCod = Gbl.Usrs.Me.MyCourses.Crss[NumMyCrs].CrsCod;
      Crs_GetDataOfCourseByCod (&Crs);
      Deg.DegCod = Crs.DegCod;
      Deg_GetDataOfDegreeByCod (&Deg);

      strcpy (CourseFullName,Crs.FullName);
      Str_LimitLengthHTMLStr (CourseFullName,Crs_MAX_LENGTH_FULL_NAME_COURSE_ON_LIST_OF_MY_COURSES);

      /* Start list entry */
      fprintf (Gbl.F.Out,"<li");
      if (Gbl.Usrs.Me.MyCourses.Crss[NumMyCrs].CrsCod == Gbl.CurrentCrs.Crs.CrsCod)
         fprintf (Gbl.F.Out," style=\"background-color:%s;\"",VERY_LIGHT_BLUE);
      fprintf (Gbl.F.Out," class=\"%s\">",The_ClassFormul[Gbl.Prefs.Theme]);

      /* Put form to go to one of my courses */
      Act_FormGoToStart (ActSeeCrsInf);
      Crs_PutParamCrsCod (Crs.CrsCod);
      sprintf (Gbl.Title,Txt_Go_to_X,Crs.FullName);
      Act_LinkFormSubmit (Gbl.Title,The_ClassFormul[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"<img src=\"%s/%s/%s16x16.gif\" alt=\"%s\""
	                 " class=\"ICON16x16\""
	                 " style=\"vertical-align:middle;\" />"
                         " %s &gt; %s</a>"
                         "</form>",
               Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_DEGREES,Deg.Logo,Deg.ShortName,
               Deg.ShortName,CourseFullName);

      /* Write link to RSS file */
      sprintf (PathRelRSSFile,"%s/%s/%ld/%s/%s",
               Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CRS,Crs.CrsCod,Cfg_RSS_FOLDER,Cfg_RSS_FILE);
      if (!Fil_CheckIfPathExists (PathRelRSSFile))
         RSS_UpdateRSSFileForACrs (&Crs);
      fprintf (Gbl.F.Out," <a href=\"");
      RSS_WriteRSSLink (Gbl.F.Out,Crs.CrsCod);
      fprintf (Gbl.F.Out,"\" target=\"_blank\">"
	                 "<img src=\"%s/rss16x16.gif\" alt=\"RSS\""
	                 " class=\"ICON16x16\""
	                 " style=\"vertical-align:middle;\" />"
	                 "</a>",
               Gbl.Prefs.IconsURL);

      /* End list entry */
      fprintf (Gbl.F.Out,"</li>");
     }

   fprintf (Gbl.F.Out,"</ul>"
	              "</td>"
	              "</tr>");
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/*********************** Get total number of courses *************************/
/*****************************************************************************/

unsigned Crs_GetNumCrssTotal (void)
  {
   char Query[256];

   /***** Get total number of courses from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM courses");
   return (unsigned) DB_QueryCOUNT (Query,"can not get the total number of courses");
  }

/*****************************************************************************/
/****************** Get number of courses in a country ***********************/
/*****************************************************************************/

unsigned Crs_GetNumCrssInCty (long CtyCod)
  {
   char Query[512];

   /***** Get number of courses in a country from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM institutions,centres,degrees,courses"
	          " WHERE institutions.CtyCod='%ld'"
	          " AND institutions.InsCod=centres.InsCod"
	          " AND centres.CtrCod=degrees.CtrCod"
	          " AND degrees.DegCod=courses.DegCod",
	    CtyCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of courses in a country");
  }

/*****************************************************************************/
/**************** Get number of courses in an institution ********************/
/*****************************************************************************/

unsigned Crs_GetNumCrssInIns (long InsCod)
  {
   char Query[512];

   /***** Get number of courses in a degree from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM centres,degrees,courses"
	          " WHERE centres.InsCod='%ld'"
	          " AND centres.CtrCod=degrees.CtrCod"
	          " AND degrees.DegCod=courses.DegCod",
	    InsCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of courses in an institution");
  }

/*****************************************************************************/
/******************** Get number of courses in a centre **********************/
/*****************************************************************************/

unsigned Crs_GetNumCrssInCtr (long CtrCod)
  {
   char Query[512];

   /***** Get number of courses in a degree from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM degrees,courses"
	          " WHERE degrees.CtrCod='%ld' AND degrees.DegCod=courses.DegCod",
	    CtrCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of courses in a centre");
  }

/*****************************************************************************/
/******************** Get number of courses in a degree **********************/
/*****************************************************************************/

unsigned Crs_GetNumCrssInDeg (long DegCod)
  {
   char Query[256];

   /***** Get number of courses in a degree from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM courses"
	          " WHERE DegCod='%ld'",
	    DegCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of courses in a degree");
  }

/*****************************************************************************/
/********************* Get number of courses with users **********************/
/*****************************************************************************/

unsigned Crs_GetNumCrssWithUsrs (Rol_Role_t Role,const char *SubQuery)
  {
   char Query[512];

   /***** Get number of degrees with users from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT courses.CrsCod)"
                  " FROM institutions,centres,degrees,courses,crs_usr"
                  " WHERE %sinstitutions.InsCod=centres.InsCod"
                  " AND centres.CtrCod=degrees.CtrCod"
                  " AND degrees.DegCod=courses.DegCod"
                  " AND courses.CrsCod=crs_usr.CrsCod"
                  " AND crs_usr.Role='%u'",
            SubQuery,(unsigned) Role);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of courses with users");
  }

/*****************************************************************************/
/************************** Show courses of a degree *************************/
/*****************************************************************************/

void Crs_ShowCrssOfCurrentDeg (void)
  {
   if (Gbl.CurrentDeg.Deg.DegCod > 0)
     {
      /***** Get list of courses in this degree *****/
      Crs_GetListCoursesInDegree (Crs_ALL_COURSES_EXCEPT_REMOVED);

      /***** Write menu to select country, institution, centre and degree *****/
      Deg_WriteMenuAllCourses (ActSeeIns,ActSeeCtr,ActSeeDeg,ActSeeCrs);

      /***** Put link (form) to edit courses in current degree *****/
      if (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_GUEST)
         Lay_PutFormToEdit (ActEdiCrs);

      /***** Show list of courses *****/
      Crs_ListCourses ();

      /***** Free list of courses in this degree *****/
      Crs_FreeListCoursesInDegree (&Gbl.CurrentDeg.Deg);
     }
  }

/*****************************************************************************/
/************************ Request edition of courses *************************/
/*****************************************************************************/

void Crs_ReqEditCourses (void)
  {
   if (Gbl.CurrentDeg.Deg.DegCod > 0)
     {
      /***** Get list of courses in this degree *****/
      Crs_GetListCoursesInDegree (Crs_ALL_COURSES_EXCEPT_REMOVED);

      /***** Get list of degrees administrated by me *****/
      Deg_GetListDegsAdminByMe ();

      /***** Put form to edit courses *****/
      Crs_EditCourses ();

      /***** Free list of courses in this degree *****/
      Crs_FreeListCoursesInDegree (&Gbl.CurrentDeg.Deg);

      /***** Free list of degrees administrated by me *****/
      Deg_FreeListMyAdminDegs ();
     }
  }

/*****************************************************************************/
/**************** Create a list with courses in this degree ******************/
/*****************************************************************************/

static void Crs_GetListCoursesInDegree (Crs_WhatCourses_t WhatCourses)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumCrs;
   struct Course *Crs;

   /***** Get courses of a degree from database *****/
   switch (WhatCourses)
     {
      case Crs_ACTIVE_COURSES:
         sprintf (Query,"SELECT CrsCod,DegCod,Year,Semester,InsCrsCod,AllowDirectLogIn,Status,RequesterUsrCod,ShortName,FullName"
                        " FROM courses WHERE DegCod='%ld' AND Status=0"
                        " ORDER BY Year,ShortName",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Crs_ALL_COURSES_EXCEPT_REMOVED:
         sprintf (Query,"SELECT CrsCod,DegCod,Year,Semester,InsCrsCod,AllowDirectLogIn,Status,RequesterUsrCod,ShortName,FullName"
                        " FROM courses WHERE DegCod='%ld' AND (Status & %u)=0"
                        " ORDER BY Year,ShortName",
                  Gbl.CurrentDeg.Deg.DegCod,
                  (unsigned) Crs_STATUS_BIT_REMOVED);
         break;
      default:
	 break;
     }
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get the courses of a degree");

   if (NumRows) // Courses found...
     {
      // NumRows should be equal to Deg->NumCourses
      Gbl.CurrentDeg.Deg.NumCourses = (unsigned) NumRows;

      /***** Create list with courses in degree *****/
      if ((Gbl.CurrentDeg.Deg.LstCrss = (struct Course *) calloc (NumRows,sizeof (struct Course))) == NULL)
          Lay_ShowErrorAndExit ("Not enough memory to store the courses of a degree.");

      /***** Get the courses in degree *****/
      for (NumCrs = 0;
	   NumCrs < Gbl.CurrentDeg.Deg.NumCourses;
	   NumCrs++)
        {
         Crs = &(Gbl.CurrentDeg.Deg.LstCrss[NumCrs]);

         /* Get next course */
         row = mysql_fetch_row (mysql_res);
         Crs_GetDataOfCourseFromRow (Crs,row);
        }
     }
   else
      Gbl.CurrentDeg.Deg.NumCourses = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Free list of courses in this degree *******************/
/*****************************************************************************/

void Crs_FreeListCoursesInDegree (struct Degree *Deg)
  {
   if (Deg->LstCrss)
     {
      /***** Free memory used by the list of courses in degree *****/
      free ((void *) Deg->LstCrss);
      Deg->LstCrss = NULL;
      Deg->NumCourses = 0;
     }
  }

/*****************************************************************************/
/********************** Write selector of my coursess ************************/
/*****************************************************************************/

void Crs_WriteSelectorMyCourses (void)
  {
   extern const char *Txt_No_COURSE_SELECTED;
   static const unsigned SelectorWidth[Lay_NUM_LAYOUTS] =
     {
      140,	// Lay_LAYOUT_DESKTOP
      240,	// Lay_LAYOUT_MOBILE
     };
   unsigned NumMyCrs;
   bool IBelongToCurrentCrs = false;
   long CrsCod;
   long DegCod;
   long LastDegCod;
   char CrsShortName[Crs_MAX_LENGTH_COURSE_SHORT_NAME+1];
   char DegShortName[Deg_MAX_LENGTH_DEGREE_SHORT_NAME+1];

   /***** Fill the list with the courses I belong to, if not filled *****/
   if (Gbl.Usrs.Me.Logged)
      Usr_GetMyCourses ();

   /***** Start form *****/
   Act_FormGoToStart (Gbl.Usrs.Me.MyCourses.Num ? ActSeeCrsInf :
                                                  ActSysReqSch);

   /***** Start of selector of courses *****/
   fprintf (Gbl.F.Out,"<select name=\"CrsCod\" style=\"width:%upx;margin:1px;\""
                      " onchange=\"javascript:document.getElementById('%s').submit();\">",
            SelectorWidth[Gbl.Prefs.Layout],Gbl.FormId);

   if (Gbl.Usrs.Me.MyCourses.Num)
     {
      /***** Write an option for each of my courses *****/
      for (NumMyCrs = 0, LastDegCod = -1L;
           NumMyCrs < Gbl.Usrs.Me.MyCourses.Num;
           NumMyCrs++)
        {
	 CrsCod = Gbl.Usrs.Me.MyCourses.Crss[NumMyCrs].CrsCod;
	 DegCod = Gbl.Usrs.Me.MyCourses.Crss[NumMyCrs].DegCod;

         Crs_GetShortNamesByCod (CrsCod,CrsShortName,DegShortName);

	 if (DegCod != LastDegCod)
	   {
	    fprintf (Gbl.F.Out,"<option value=\"-1\" disabled=\"disabled\">--- %s ---</option>",
		     DegShortName);
	    LastDegCod = DegCod;
	   }

         fprintf (Gbl.F.Out,"<option value=\"%ld\"",
                  Gbl.Usrs.Me.MyCourses.Crss[NumMyCrs].CrsCod);
         if (Gbl.CurrentCrs.Crs.CrsCod > 0 &&
             CrsCod == Gbl.CurrentCrs.Crs.CrsCod)
           {
            fprintf (Gbl.F.Out," selected=\"selected\"");
            IBelongToCurrentCrs = true;
           }
         fprintf (Gbl.F.Out,">%s</option>",CrsShortName);
        }
     }

   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
     {
      if (!IBelongToCurrentCrs)
	{
         /***** Blank option to separate *****/
	 if (Gbl.Usrs.Me.MyCourses.Num)
            fprintf (Gbl.F.Out,"<option value=\"-1\" disabled=\"disabled\">------------</option>");

         /***** Write an option with the current course *****/
         fprintf (Gbl.F.Out,"<option value=\"%ld\" selected=\"selected\">%s</option>",
                  Gbl.CurrentCrs.Crs.CrsCod,
                  Gbl.CurrentCrs.Crs.ShortName);
	}
     }
   else
      /***** Write an option with no course selected *****/
      fprintf (Gbl.F.Out,"<option value=\"-1\" disabled=\"disabled\" selected=\"selected\">%s</option>",
               Txt_No_COURSE_SELECTED);

   /***** End form *****/
   fprintf (Gbl.F.Out,"</select>"
	              "</form>");
  }

/*****************************************************************************/
/************************* List courses in this degree ***********************/
/*****************************************************************************/

static void Crs_ListCourses (void)
  {
   extern const char *Txt_No_courses_have_been_created_in_this_degree;

   if (Gbl.CurrentDeg.Deg.NumCourses)
      Crs_ListCoursesForSeeing ();
   else
      Lay_ShowAlert (Lay_INFO,Txt_No_courses_have_been_created_in_this_degree);
  }

/*****************************************************************************/
/****************** Put forms to edit courses in this degree *****************/
/*****************************************************************************/

static void Crs_EditCourses (void)
  {
   extern const char *Txt_No_courses_have_been_created_in_this_degree;

   /***** Help message *****/
   if (!Gbl.CurrentDeg.Deg.NumCourses)	// There aren't courses
      Lay_ShowAlert (Lay_INFO,Txt_No_courses_have_been_created_in_this_degree);

   /***** Put a form to create or request a new course *****/
   Crs_PutFormToCreateCourse ();

   /***** Forms to edit current courses *****/
   if (Gbl.CurrentDeg.Deg.NumCourses)
      Crs_ListCoursesForEdition ();
  }

/*****************************************************************************/
/********************* List current courses for edition **********************/
/*****************************************************************************/

static void Crs_ListCoursesForSeeing (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Courses_of_DEGREE_X;
   extern const char *Txt_COURSE_With_users;
   extern const char *Txt_COURSE_Without_users;
   extern const char *Txt_YEAR_OF_DEGREE[1+Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_SEMESTER_OF_YEAR[1+2];
   extern const char *Txt_Go_to_X;
   extern const char *Txt_COURSE_STATUS[Crs_NUM_STATUS_TXT];
   struct Course *Crs;
   unsigned Year;
   unsigned NumCrs;
   const char *TxtClass;
   const char *BgColor;
   Crs_StatusTxt_t StatusTxt;

   /***** Write heading *****/
   sprintf (Gbl.Message,Txt_Courses_of_DEGREE_X,Gbl.CurrentDeg.Deg.ShortName);
   Lay_StartRoundFrameTable10 (NULL,2,Gbl.Message);
   Crs_PutHeadCoursesForSeeing ();

   /***** List the courses *****/
   for (Year = 0;
	Year <= Deg_MAX_YEARS_PER_DEGREE;
	Year++)
     {
      for (NumCrs = 0;
	   NumCrs < Gbl.CurrentDeg.Deg.NumCourses;
	   NumCrs++)
        {
         Crs = &(Gbl.CurrentDeg.Deg.LstCrss[NumCrs]);
         if (Crs->Year == Year)
           {
            TxtClass = (Crs->Status & Crs_STATUS_BIT_PENDING) ? "DAT_LIGHT" :
        	                                                "DAT";
            BgColor = (Crs->CrsCod == Gbl.CurrentCrs.Crs.CrsCod) ? VERY_LIGHT_BLUE :
        	                                                   Gbl.ColorRows[Gbl.RowEvenOdd];

            /* Put green tip if course has users */
            fprintf (Gbl.F.Out,"<tr>"
                               "<td align=\"center\" bgcolor=\"%s\">"
                               "<img src=\"%s/%s16x16.gif\""
                               " alt=\"\" title=\"%s\" class=\"ICON16x16\" />"
                               "</td>",
                     BgColor,
                     Gbl.Prefs.IconsURL,
                     Crs->NumUsrs ? "ok_green" :
                	            "tr",
                     Crs->NumUsrs ? Txt_COURSE_With_users :
                                    Txt_COURSE_Without_users);

            /* Institutional code of the course */
            fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\" class=\"%s\" bgcolor=\"%s\">%s</td>",
                     TxtClass,BgColor,Crs->InstitutionalCrsCod);

            /* Course year */
            fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\" class=\"%s\" bgcolor=\"%s\">%s</td>",
                     TxtClass,BgColor,Txt_YEAR_OF_DEGREE[Crs->Year]);

            /* Course semester */
            fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\" class=\"%s\" bgcolor=\"%s\">%s</td>",
                     TxtClass,BgColor,Txt_SEMESTER_OF_YEAR[Crs->Semester]);

            /* Course full name */
            fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"middle\" class=\"%s\" bgcolor=\"%s\">",
                     TxtClass,BgColor);
            Act_FormGoToStart (ActSeeCrsInf);
            Crs_PutParamCrsCod (Crs->CrsCod);
            sprintf (Gbl.Title,Txt_Go_to_X,Crs->FullName);
            Act_LinkFormSubmit (Gbl.Title,TxtClass);
            fprintf (Gbl.F.Out,"%s</a>"
        	               "</form>"
        	               "</td>",
        	     Crs->FullName);

            /* Current number of students in this course */
            fprintf (Gbl.F.Out,"<td align=\"right\" valign=\"middle\" class=\"%s\" bgcolor=\"%s\">%u</td>",
                     TxtClass,BgColor,Crs->NumStds);

            /* Current number of teachers in this course */
            fprintf (Gbl.F.Out,"<td align=\"right\" valign=\"middle\" class=\"%s\" bgcolor=\"%s\">%u</td>",
                     TxtClass,BgColor,Crs->NumTchs);

            /* Course status */
            StatusTxt = Crs_GetStatusTxtFromStatusBits (Crs->Status);
            fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"middle\" class=\"%s\" bgcolor=\"%s\">%s</td>"
                               "</tr>",
                     TxtClass,BgColor,Txt_COURSE_STATUS[StatusTxt]);
           }
        }
      Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
     }

   /***** Table end *****/
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/********************* List current courses for edition **********************/
/*****************************************************************************/

static void Crs_ListCoursesForEdition (void)
  {
   extern const char *Txt_Courses_of_DEGREE_X;
   extern const char *Txt_Remove_course;
   extern const char *Txt_YEAR_OF_DEGREE[1+Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_SEMESTER_OF_YEAR[1+2];
   extern const char *Txt_COURSE_STATUS[Crs_NUM_STATUS_TXT];
   struct Course *Crs;
   unsigned Year;
   unsigned YearAux;
   unsigned Semester;
   unsigned NumDeg;
   unsigned NumCrs;
   struct UsrData UsrDat;
   bool ICanEdit;
   Crs_StatusTxt_t StatusTxt;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Write heading *****/
   sprintf (Gbl.Message,Txt_Courses_of_DEGREE_X,Gbl.CurrentDeg.Deg.ShortName);
   Lay_StartRoundFrameTable10 (NULL,2,Gbl.Message);
   Crs_PutHeadCoursesForEdition ();

   /***** List the courses *****/
   for (Year = 0;
	Year <= Deg_MAX_YEARS_PER_DEGREE;
	Year++)
      for (NumCrs = 0;
	   NumCrs < Gbl.CurrentDeg.Deg.NumCourses;
	   NumCrs++)
        {
         Crs = &(Gbl.CurrentDeg.Deg.LstCrss[NumCrs]);
         if (Crs->Year == Year)
           {
            ICanEdit = Crs_CheckIfICanEdit (Crs);

            /* Put icon to remove course */
            fprintf (Gbl.F.Out,"<tr>"
                               "<td class=\"BM\">");
            if (Crs->NumUsrs ||	// Course has users ==> deletion forbidden
                !ICanEdit)
               fprintf (Gbl.F.Out,"<img src=\"%s/deloff16x16.gif\""
        	                  " alt=\"\" class=\"ICON16x16\" />",
                        Gbl.Prefs.IconsURL);
            else	// Crs->NumUsrs == 0 && ICanEdit
              {
               Act_FormStart (ActRemCrs);
               Crs_PutParamOtherCrsCod (Crs->CrsCod);
               fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/delon16x16.gif\""
        	                  " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />"
        	                  "</form>",
                        Gbl.Prefs.IconsURL,
                        Txt_Remove_course,
                        Txt_Remove_course);
              }
            fprintf (Gbl.F.Out,"</td>");

            /* Course code */
            fprintf (Gbl.F.Out,"<td align=\"right\" valign=\"middle\" class=\"DAT\">%ld</td>",
                     Crs->CrsCod);

            /* Institutional code of the course */
            fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\" class=\"DAT\">");
            if (ICanEdit)
              {
               Act_FormStart (ActChgInsCrsCod);
               Crs_PutParamOtherCrsCod (Crs->CrsCod);
               fprintf (Gbl.F.Out,"<input type=\"text\" name=\"InsCrsCod\" size=\"%u\" maxlength=\"%u\" value=\"%s\""
                                  " onchange=\"javascript:document.getElementById('%s').submit();\" />"
                                  "</form>",
                        Crs_LENGTH_INSTITUTIONAL_CRS_COD,
                        Crs_LENGTH_INSTITUTIONAL_CRS_COD,
                        Crs->InstitutionalCrsCod,
                        Gbl.FormId);
              }
            else
               fprintf (Gbl.F.Out,"%s",Crs->InstitutionalCrsCod);
            fprintf (Gbl.F.Out,"</td>");

            /* Degree */
            fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"middle\" class=\"DAT\">");
            if (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_DEG_ADMIN)
              {
               Act_FormStart (ActChgCrsDeg);
               Crs_PutParamOtherCrsCod (Crs->CrsCod);
               fprintf (Gbl.F.Out,"<select name=\"OthDegCod\" style=\"width:80px;\""
        	                  " onchange=\"javascript:document.getElementById('%s').submit();\">",
                        Gbl.FormId);
               for (NumDeg = 0;
        	    NumDeg < Gbl.Usrs.Me.MyAdminDegs.Num;
        	    NumDeg++)
                  fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s</option>",
                           Gbl.Usrs.Me.MyAdminDegs.Lst[NumDeg].DegCod,
                           Gbl.Usrs.Me.MyAdminDegs.Lst[NumDeg].DegCod == Gbl.CurrentDeg.Deg.DegCod ? " selected=\"selected\"" :
                        	                                                                     "",
                           Gbl.Usrs.Me.MyAdminDegs.Lst[NumDeg].ShortName);
               fprintf (Gbl.F.Out,"</select>"
                                  "</form>");
              }
            else
               fprintf (Gbl.F.Out,"%s",Gbl.CurrentDeg.Deg.ShortName);
            fprintf (Gbl.F.Out,"</td>");

            /* Course year */
            fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\" class=\"DAT\">");
            if (ICanEdit)
              {
               Act_FormStart (ActChgCrsYea);
               Crs_PutParamOtherCrsCod (Crs->CrsCod);
               fprintf (Gbl.F.Out,"<select name=\"OthCrsYear\" style=\"width:40px;\""
        	                  " onchange=\"javascript:document.getElementById('%s').submit();\">",
                        Gbl.FormId);
               for (YearAux = 0;
        	    YearAux <= Deg_MAX_YEARS_PER_DEGREE;
        	    YearAux++)	// All the years are permitted because it's possible to move this course to another degree (with other active years)
                  fprintf (Gbl.F.Out,"<option value=\"%u\"%s>%s</option>",
                           YearAux,
                           YearAux == Crs->Year ? " selected=\"selected\"" :
                        	                  "",
                           Txt_YEAR_OF_DEGREE[YearAux]);
               fprintf (Gbl.F.Out,"</select></form>");
              }
            else
               fprintf (Gbl.F.Out,"%s",Txt_YEAR_OF_DEGREE[Crs->Year]);
            fprintf (Gbl.F.Out,"</td>");

            /* Course semester */
            fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\" class=\"DAT\">");
            if (ICanEdit)
              {
               Act_FormStart (ActChgCrsSem);
               Crs_PutParamOtherCrsCod (Crs->CrsCod);
               fprintf (Gbl.F.Out,"<select name=\"OthCrsSem\""
                                  " onchange=\"javascript:document.getElementById('%s').submit();\">",
                        Gbl.FormId);
               for (Semester = 0;
        	    Semester <= 2;
        	    Semester++)
                 {
                  fprintf (Gbl.F.Out,"<option value=\"%u\"",Semester);
                  if (Semester == Crs->Semester)
                     fprintf (Gbl.F.Out," selected=\"selected\"");
                  fprintf (Gbl.F.Out,">%s</option>",
                           Txt_SEMESTER_OF_YEAR[Semester]);
                 }
               fprintf (Gbl.F.Out,"</select></form>");
              }
            else
               fprintf (Gbl.F.Out,"%s",Txt_SEMESTER_OF_YEAR[Crs->Semester]);
            fprintf (Gbl.F.Out,"</td>");

            /* Course short name */
            fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"middle\" class=\"DAT\">");
            if (ICanEdit)
              {
               Act_FormStart (ActRenCrsSho);
               Crs_PutParamOtherCrsCod (Crs->CrsCod);
               fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\" size=\"10\" maxlength=\"%u\" value=\"%s\""
                                  " onchange=\"javascript:document.getElementById('%s').submit();\" />"
                                  "</form>",
                        Crs_MAX_LENGTH_COURSE_SHORT_NAME,Crs->ShortName,
                        Gbl.FormId);
              }
            else
               fprintf (Gbl.F.Out,"%s",Crs->ShortName);
            fprintf (Gbl.F.Out,"</td>");

            /* Course full name */
            fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"middle\" class=\"DAT\">");
            if (ICanEdit)
              {
               Act_FormStart (ActRenCrsFul);
               Crs_PutParamOtherCrsCod (Crs->CrsCod);
               fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\" size=\"20\" maxlength=\"%u\" value=\"%s\""
                                  " onchange=\"javascript:document.getElementById('%s').submit();\" />"
                                  "</form>",
                        Crs_MAX_LENGTH_COURSE_FULL_NAME,Crs->FullName,
                        Gbl.FormId);
              }
            else
               fprintf (Gbl.F.Out,"%s",Crs->FullName);
            fprintf (Gbl.F.Out,"</td>");

            /* Current number of students in this course */
            fprintf (Gbl.F.Out,"<td align=\"right\" valign=\"middle\" class=\"DAT\">%u</td>",
                     Crs->NumStds);

            /* Current number of teachers in this course */
            fprintf (Gbl.F.Out,"<td align=\"right\" valign=\"middle\" class=\"DAT\">%u</td>",
                     Crs->NumTchs);

            /* Course status */
            StatusTxt = Crs_GetStatusTxtFromStatusBits (Crs->Status);
            fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"middle\" class=\"DAT\">");
            if (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_DEG_ADMIN &&
        	StatusTxt == Crs_STATUS_PENDING)
              {
               Act_FormStart (ActChgCrsSta);
               Crs_PutParamOtherCrsCod (Crs->CrsCod);
               fprintf (Gbl.F.Out,"<select name=\"Status\" style=\"width:80px;\""
        	                  " onchange=\"javascript:document.getElementById('%s').submit();\">"
                                  "<option value=\"%u\" selected=\"selected\">%s</option>"
                                  "<option value=\"%u\">%s</option>"
                                  "</select>"
                                  "</form>",
                        Gbl.FormId,
                        (unsigned) Crs_GetStatusBitsFromStatusTxt (Crs_STATUS_PENDING),
                        Txt_COURSE_STATUS[Crs_STATUS_PENDING],
                        (unsigned) Crs_GetStatusBitsFromStatusTxt (Crs_STATUS_ACTIVE),
                        Txt_COURSE_STATUS[Crs_STATUS_ACTIVE]);
              }
            else
               fprintf (Gbl.F.Out,"%s",Txt_COURSE_STATUS[StatusTxt]);
            fprintf (Gbl.F.Out,"</td>");

            /* Course requester */
            UsrDat.UsrCod = Crs->RequesterUsrCod;
            Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);
            fprintf (Gbl.F.Out,"<td align=\"left\" width=\"100\" valign=\"top\">"
                               "<table width=\"100\" cellspacing=\"0\" cellpadding=\"2\">"
                               "<tr>");
            Msg_WriteMsgAuthor (&UsrDat,80,10,"DAT",true,NULL);
            fprintf (Gbl.F.Out,"</tr>"
        	               "</table>"
        	               "</td>"
        	               "</tr>");
           }
        }

   /***** End table *****/
   Lay_EndRoundFrameTable10 ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************** Check if I can edit, remove, etc. a course *******************/
/*****************************************************************************/

static bool Crs_CheckIfICanEdit (struct Course *Crs)
  {
   return (bool) (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_DEG_ADMIN ||		// I am a degree administrator or higher
                  ((Crs->Status & Crs_STATUS_BIT_PENDING) != 0 &&		// Course is not yet activated
                   Gbl.Usrs.Me.UsrDat.UsrCod == Crs->RequesterUsrCod));		// I am the requester
  }

/*****************************************************************************/
/******************* Set StatusTxt depending on status bits ******************/
/*****************************************************************************/
// Crs_STATUS_UNKNOWN = 0	// Other
// Crs_STATUS_ACTIVE  = 1	// 00 (Status == 0)
// Crs_STATUS_PENDING = 2	// 01 (Status == Crs_STATUS_BIT_PENDING)
// Crs_STATUS_REMOVED = 3	// 1- (Status & Crs_STATUS_BIT_REMOVED)

static Crs_StatusTxt_t Crs_GetStatusTxtFromStatusBits (Crs_Status_t Status)
  {
   if (Status == 0)
      return Crs_STATUS_ACTIVE;
   if (Status == Crs_STATUS_BIT_PENDING)
      return Crs_STATUS_PENDING;
   if (Status & Crs_STATUS_BIT_REMOVED)
      return Crs_STATUS_REMOVED;
   return Crs_STATUS_UNKNOWN;
  }

/*****************************************************************************/
/******************* Set status bits depending on StatusTxt ******************/
/*****************************************************************************/
// Crs_STATUS_UNKNOWN = 0	// Other
// Crs_STATUS_ACTIVE  = 1	// 00 (Status == 0)
// Crs_STATUS_PENDING = 2	// 01 (Status == Crs_STATUS_BIT_PENDING)
// Crs_STATUS_REMOVED = 3	// 1- (Status & Crs_STATUS_BIT_REMOVED)

static Crs_Status_t Crs_GetStatusBitsFromStatusTxt (Crs_StatusTxt_t StatusTxt)
  {
   switch (StatusTxt)
     {
      case Crs_STATUS_UNKNOWN:
      case Crs_STATUS_ACTIVE:
	 return (Crs_Status_t) 0;
      case Crs_STATUS_PENDING:
	 return Crs_STATUS_BIT_PENDING;
      case Crs_STATUS_REMOVED:
	 return Crs_STATUS_BIT_REMOVED;
     }
   return (Crs_Status_t) 0;
  }

/*****************************************************************************/
/*********************** Put a form to create a new course *******************/
/*****************************************************************************/

static void Crs_PutFormToCreateCourse (void)
  {
   extern const char *Txt_New_course_of_DEGREE_X;
   extern const char *Txt_YEAR_OF_DEGREE[1+Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_SEMESTER_OF_YEAR[1+2];
   extern const char *Txt_COURSE_STATUS[Crs_NUM_STATUS_TXT];
   extern const char *Txt_Create_course;
   struct Course *Crs;
   unsigned Year;
   unsigned Semester;

   /***** Start form *****/
   if (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_DEG_ADMIN)
      Act_FormStart (ActNewCrs);
   else if (Gbl.Usrs.Me.MaxRole >= Rol_ROLE_GUEST)
      Act_FormStart (ActReqCrs);
   else
      Lay_ShowErrorAndExit ("You can not edit courses.");

   /***** Course data *****/
   Crs = &Gbl.Degs.EditingCrs;

   /***** Write heading *****/
   sprintf (Gbl.Message,Txt_New_course_of_DEGREE_X,Gbl.CurrentDeg.Deg.ShortName);
   Lay_StartRoundFrameTable10 (NULL,2,Gbl.Message);
   Crs_PutHeadCoursesForEdition ();

   /***** Disabled icon to remove course *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"BM\">"
                      "<img src=\"%s/deloff16x16.gif\""
		      " alt=\"\" class=\"ICON16x16\" />"
                      "</td>",
	    Gbl.Prefs.IconsURL);

   /***** Course code *****/
   fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\" class=\"DAT\"></td>");

   /***** Institutional code of the course *****/
   fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\">"
                      "<input type=\"text\" name=\"InsCrsCod\" size=\%u\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>",
            Crs_LENGTH_INSTITUTIONAL_CRS_COD,
            Crs_LENGTH_INSTITUTIONAL_CRS_COD,
            Crs->InstitutionalCrsCod);

   /***** Degree *****/
   fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\">"
                      "<select name=\"OthDegCod\" style=\"width:80px;\" disabled=\"disabled\">"
                      "<option value=\"%ld\">%s</option>"
                      "</select>"
                      "</td>",
            Gbl.CurrentDeg.Deg.DegCod,
            Gbl.CurrentDeg.Deg.ShortName);

   /***** Year *****/
   fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\">"
	              "<select name=\"OthCrsYear\" style=\"width:40px;\">");
   for (Year = 0;
	Year <= Gbl.CurrentDeg.Deg.LastYear;
        Year++)
      if (Deg_CheckIfYearIsValidInDeg (Year,&Gbl.CurrentDeg.Deg))
         fprintf (Gbl.F.Out,"<option value=\"%u\"%s>%s</option>",
                  Year,
                  Year == Crs->Year ? " selected=\"selected\"" :
                	              "",
                  Txt_YEAR_OF_DEGREE[Year]);
   fprintf (Gbl.F.Out,"</select>"
	              "</td>");

   /***** Semester *****/
   fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\">"
	              "<select name=\"OthCrsSem\">");
   for (Semester = 0;
	Semester <= 2;
	Semester++)
      fprintf (Gbl.F.Out,"<option value=\"%u\"%s>%s</option>",
               Semester,
               Semester == Crs->Semester ? " selected=\"selected\"" :
        	                           "",
               Txt_SEMESTER_OF_YEAR[Semester]);
   fprintf (Gbl.F.Out,"</select>"
                      "</td>");

   /***** Course short name *****/
   fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\">"
                      "<input type=\"text\" name=\"ShortName\" size=\"10\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>",
            Crs_MAX_LENGTH_COURSE_SHORT_NAME,Crs->ShortName);

   /***** Course full name *****/
   fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\">"
                      "<input type=\"text\" name=\"FullName\" size=\"20\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>",
            Crs_MAX_LENGTH_COURSE_FULL_NAME,Crs->FullName);

   /***** Current number of students in this course *****/
   fprintf (Gbl.F.Out,"<td align=\"right\" valign=\"middle\" class=\"DAT\">0</td>");

   /***** Current number of teachers in this course *****/
   fprintf (Gbl.F.Out,"<td align=\"right\" valign=\"middle\" class=\"DAT\">0</td>");

   /***** Course status *****/
   fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\" class=\"DAT\">%s</td>",
            Txt_COURSE_STATUS[Crs_STATUS_PENDING]);

   /***** Course requester *****/
   fprintf (Gbl.F.Out,"<td align=\"left\" width=\"100\" valign=\"top\">"
		      "<table width=\"100\" cellspacing=\"0\" cellpadding=\"2\">"
		      "<tr>");
   Msg_WriteMsgAuthor (&Gbl.Usrs.Me.UsrDat,80,10,"DAT",true,NULL);
   fprintf (Gbl.F.Out,"</tr>"
		      "</table>"
		      "</td>"
		      "</tr>");

   /***** Send button *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"center\" colspan=\"12\">"
                      "<input type=\"submit\" value=\"%s\" />"
	              "</td>"
	              "</tr>",
            Txt_Create_course);

   /***** End of frame *****/
   Lay_EndRoundFrameTable10 ();

   /***** End of form *****/
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/******************** Write header with fields of a course *******************/
/*****************************************************************************/

static void Crs_PutHeadCoursesForSeeing (void)
  {
   extern const char *Txt_Institutional_BR_code;
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_Semester_ABBREVIATION;
   extern const char *Txt_Course;
   extern const char *Txt_Students_ABBREVIATION;
   extern const char *Txt_Teachers_ABBREVIATION;
   extern const char *Txt_Status;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"BM\"></th>"
                      "<th align=\"center\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"center\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"center\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"left\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"right\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"right\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"left\" class=\"TIT_TBL\">%s</th>"
                      "</tr>",
            Txt_Institutional_BR_code,
            Txt_Year_OF_A_DEGREE,
            Txt_Semester_ABBREVIATION,
            Txt_Course,
            Txt_Students_ABBREVIATION,
            Txt_Teachers_ABBREVIATION,
            Txt_Status);
  }

/*****************************************************************************/
/******************** Write header with fields of a course *******************/
/*****************************************************************************/

static void Crs_PutHeadCoursesForEdition (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Institutional_BR_code;
   extern const char *Txt_Degree;
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_Semester_ABBREVIATION;
   extern const char *Txt_Short_Name;
   extern const char *Txt_Full_Name;
   extern const char *Txt_Students_ABBREVIATION;
   extern const char *Txt_Teachers_ABBREVIATION;
   extern const char *Txt_Status;
   extern const char *Txt_Requester;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"BM\"></th>"
                      "<th align=\"right\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"center\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"left\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"center\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"center\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"left\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"left\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"right\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"right\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"left\" class=\"TIT_TBL\">%s</th>"
                      "<th align=\"left\" class=\"TIT_TBL\">%s</th>"
                      "</tr>",
            Txt_Code,
            Txt_Institutional_BR_code,
            Txt_Degree,
            Txt_Year_OF_A_DEGREE,
            Txt_Semester_ABBREVIATION,
            Txt_Short_Name,
            Txt_Full_Name,
            Txt_Students_ABBREVIATION,
            Txt_Teachers_ABBREVIATION,
            Txt_Status,
            Txt_Requester);
  }

/*****************************************************************************/
/****************** Receive form to request a new course *********************/
/*****************************************************************************/

void Crs_RecFormReqCrs (void)
  {
   Crs_RecFormRequestOrCreateCrs ((unsigned) Crs_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/******************* Receive form to create a new course *********************/
/*****************************************************************************/

void Crs_RecFormNewCrs (void)
  {
   Crs_RecFormRequestOrCreateCrs (0);
  }

/*****************************************************************************/
/************* Receive form to request or create a new course ****************/
/*****************************************************************************/

static void Crs_RecFormRequestOrCreateCrs (unsigned Status)
  {
   extern const char *Txt_The_course_X_already_exists;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_course;
   extern const char *Txt_The_year_X_is_not_enabled_on_the_degree_Y;
   extern const char *Txt_YEAR_OF_DEGREE[1+Deg_MAX_YEARS_PER_DEGREE];
   struct Course *Crs;
   struct Degree Deg;

   Crs = &Gbl.Degs.EditingCrs;

   /***** Get parameters from form *****/
   /* Set course degree */
   Deg.DegCod = Crs->DegCod = Gbl.CurrentDeg.Deg.DegCod;

   /* Get parameters of the new course */
   Crs_GetParamsNewCourse (Crs);

   /***** Check if year is correct *****/
   Deg_GetDataOfDegreeByCod (&Deg);
   if (Deg_CheckIfYearIsValidInDeg (Crs->Year,&Deg))	// If year is valid
     {
      if (Crs->ShortName[0] &&
	  Crs->FullName[0])	// If there's a course name
	{
	 /***** If name of course was in database... *****/
	 if (Crs_CheckIfCourseNameExistsInCourses (Crs->DegCod,Crs->Year,"ShortName",Crs->ShortName,-1L))
	   {
	    sprintf (Gbl.Message,Txt_The_course_X_already_exists,
		     Crs->ShortName);
	    Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	   }
	 else if (Crs_CheckIfCourseNameExistsInCourses (Crs->DegCod,Crs->Year,"FullName",Crs->FullName,-1L))
	   {
	    sprintf (Gbl.Message,Txt_The_course_X_already_exists,
		     Crs->FullName);
	    Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	   }
	 else	// Add new requested course to database
	    Crs_CreateCourse (Crs,Status);
	}
      else	// If there is not a course name
	{
	 sprintf (Gbl.Message,"%s",Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_course);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	}
     }
   else	// Year not valid
     {
      sprintf (Gbl.Message,Txt_The_year_X_is_not_enabled_on_the_degree_Y,
	       Txt_YEAR_OF_DEGREE[Crs->Year],Deg.FullName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }

   /***** Show the form again *****/
   Crs_ReqEditCourses ();
  }

/*****************************************************************************/
/************** Get the parameters of a new course from form *****************/
/*****************************************************************************/

static void Crs_GetParamsNewCourse (struct Course *Crs)
  {
   char YearStr[2+1];
   char SemesterStr[1+1];

   /***** Get parameters of the course from form *****/
   /* Get institutional code */
   Par_GetParToText ("InsCrsCod",Crs->InstitutionalCrsCod,Crs_LENGTH_INSTITUTIONAL_CRS_COD);

   /* Get year */
   Par_GetParToText ("OthCrsYear",YearStr,2);
   Crs->Year = Deg_ConvStrToYear (YearStr);

   /* Get semester */
   Par_GetParToText ("OthCrsSem",SemesterStr,1);
   Crs->Semester = Deg_ConvStrToSemester (SemesterStr);

   /* Get course short name */
   Par_GetParToText ("ShortName",Crs->ShortName,Crs_MAX_LENGTH_COURSE_SHORT_NAME);

   /* Get course full name */
   Par_GetParToText ("FullName",Crs->FullName,Crs_MAX_LENGTH_COURSE_FULL_NAME);
  }

/*****************************************************************************/
/********** Check if the name of course exists in existing courses ***********/
/*****************************************************************************/

static bool Crs_CheckIfCourseNameExistsInCourses (long DegCod,unsigned Year,const char *FieldName,const char *Name,long CrsCod)
  {
   char Query[512];

   /***** Get number of courses in a year of a degree and with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM courses"
                  " WHERE DegCod='%ld' AND Year='%u'"
                  " AND %s='%s' AND CrsCod<>'%ld'",
            DegCod,Year,FieldName,Name,CrsCod);
   return (DB_QueryCOUNT (Query,"can not check if the name of a course already existed") != 0);
  }

/*****************************************************************************/
/************* Add a new requested course to pending requests ****************/
/*****************************************************************************/

static void Crs_CreateCourse (struct Course *Crs,unsigned Status)
  {
   extern const char *Txt_Created_new_course_X;
   char Query[2048];

   /***** Insert new course into pending requests *****/
   sprintf (Query,"INSERT INTO courses (DegCod,Year,Semester,InsCrsCod,"
                  "AllowDirectLogIn,Status,RequesterUsrCod,ShortName,FullName)"
                  " VALUES ('%ld','%u','%u','%s','N','%u','%ld','%s','%s')",
            Crs->DegCod,Crs->Year,Crs->Semester,
            Crs->InstitutionalCrsCod,
            Status,
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Crs->ShortName,Crs->FullName);
   Crs->CrsCod = DB_QueryINSERTandReturnCode (Query,"can not create a new course");

   /***** Write success message *****/
   sprintf (Gbl.Message,Txt_Created_new_course_X,
            Crs->FullName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Put link to go to course created *****/
   Crs_PutLinkToGoToCrs (Crs);
  }

/*****************************************************************************/
/************************ Request removing of a course ***********************/
/*****************************************************************************/

void Crs_RemoveCourse (void)
  {
   extern const char *Txt_To_remove_a_course_you_must_first_remove_all_users_in_the_course;
   extern const char *Txt_Course_X_removed;
   extern const char *Txt_You_dont_have_permission_to_edit_this_course;
   struct Course Crs;

   /***** Get course code *****/
   if ((Crs.CrsCod = Crs_GetParamOtherCrsCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of course is missing.");

   /***** Get data of the course from database *****/
   Crs_GetDataOfCourseByCod (&Crs);

   if (Crs_CheckIfICanEdit (&Crs))
     {
      /***** Check if this course has users *****/
      if (Crs.NumUsrs)	// Course has users ==> don't remove
         Lay_ShowAlert (Lay_WARNING,Txt_To_remove_a_course_you_must_first_remove_all_users_in_the_course);
      else	// Course has no users ==> remove it
        {
         /***** Remove course *****/
         Crs_RemoveCourseCompletely (Crs.CrsCod);

         /***** Write message to show the change made *****/
         sprintf (Gbl.Message,Txt_Course_X_removed,
                  Crs.FullName);
         Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
        }
     }
   else
      Lay_ShowAlert (Lay_WARNING, Txt_You_dont_have_permission_to_edit_this_course);

   /***** Show the form again *****/
   Crs_ReqEditCourses ();
  }

/*****************************************************************************/
/********************* Get data of a course from its code ********************/
/*****************************************************************************/

bool Crs_GetDataOfCourseByCod (struct Course *Crs)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   bool CrsFound = false;

   if (Crs->CrsCod < 0)
     {
      Crs->CrsCod = -1L;
      Crs->DegCod = -1L;
      Crs->Year = 0;
      Crs->Semester = 0;
      Crs->AllowDirectLogIn = false;
      Crs->Status = (Crs_Status_t) 0;
      Crs->RequesterUsrCod = -1L;
      Crs->ShortName[0] = '\0';
      Crs->FullName[0] = '\0';
      Crs->NumStds = 0;
      Crs->NumTchs = 0;
      Crs->NumUsrs = 0;
      return false;
     }

   /***** Get data of a course from database *****/
   sprintf (Query,"SELECT CrsCod,DegCod,Year,Semester,InsCrsCod,AllowDirectLogIn,Status,RequesterUsrCod,ShortName,FullName"
                  " FROM courses WHERE CrsCod='%ld'",
            Crs->CrsCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get data of a course");

   if (NumRows == 1)		// Course found
     {
      /***** Get data of the course *****/
      row = mysql_fetch_row (mysql_res);
      Crs_GetDataOfCourseFromRow (Crs,row);

      CrsFound = true;
     }
   else if (NumRows == 0)	// Course not found
     {
      Crs->CrsCod = -1L;
      Crs->DegCod = -1L;
      Crs->Year = 0;
      Crs->Semester = 0;
      Crs->AllowDirectLogIn = false;
      Crs->Status = (Crs_Status_t) 0;
      Crs->RequesterUsrCod = -1L;
      Crs->ShortName[0] = '\0';
      Crs->FullName[0] = '\0';
      Crs->NumStds = 0;
      Crs->NumTchs = 0;
      Crs->NumUsrs = 0;
     }
   else if (NumRows > 1)	// Course duplicated
      Lay_ShowErrorAndExit ("Course is repeated in database.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return CrsFound;
  }

/*****************************************************************************/
/********** Get data of a course from a row resulting of a query *************/
/*****************************************************************************/

static void Crs_GetDataOfCourseFromRow (struct Course *Crs,MYSQL_ROW row)
  {
   /***** Get course code (row[0]) *****/
   if ((Crs->CrsCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
      Lay_ShowErrorAndExit ("Wrong code of course.");

   /***** Get code of degree (row[1]) *****/
   Crs->DegCod = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get year (row[2]) *****/
   Crs->Year = Deg_ConvStrToYear (row[2]);

   /***** Get semester (row[3]) *****/
   Crs->Semester = Deg_ConvStrToSemester (row[3]);

   /***** Get institutional course code (row[4]) *****/
   strncpy (Crs->InstitutionalCrsCod,row[4],Crs_LENGTH_INSTITUTIONAL_CRS_COD);
   Crs->InstitutionalCrsCod[Crs_LENGTH_INSTITUTIONAL_CRS_COD] = '\0';

   /***** Get whether this course allows direct log in or not (row[5]) *****/
   Crs->AllowDirectLogIn = (Str_ConvertToUpperLetter (row[5][0]) == 'Y');

   /***** Get course status (row[6]) *****/
   if (sscanf (row[6],"%u",&(Crs->Status)) != 1)
      Lay_ShowErrorAndExit ("Wrong course status.");

   /***** Get requester user'code (row[7]) *****/
   Crs->RequesterUsrCod = Str_ConvertStrCodToLongCod (row[7]);

   /***** Get the short name of the course (row[8]) *****/
   strncpy (Crs->ShortName,row[8],Crs_MAX_LENGTH_COURSE_SHORT_NAME);
   Crs->ShortName[Crs_MAX_LENGTH_COURSE_SHORT_NAME] = '\0';

   /***** Get the full name of the course (row[9]) *****/
   strncpy (Crs->FullName,row[9],Crs_MAX_LENGTH_COURSE_FULL_NAME);
   Crs->FullName[Crs_MAX_LENGTH_COURSE_FULL_NAME] = '\0';

   /***** Get number of students *****/
   Crs->NumStds = Usr_GetNumUsrsInCrs (Rol_ROLE_STUDENT,Crs->CrsCod);

   /***** Get number of teachers *****/
   Crs->NumTchs = Usr_GetNumUsrsInCrs (Rol_ROLE_TEACHER,Crs->CrsCod);

   Crs->NumUsrs = Crs->NumStds + Crs->NumTchs;
  }

/*****************************************************************************/
/******* Get the short names of degree and course from a course code *********/
/*****************************************************************************/

void Crs_GetShortNamesByCod (long CrsCod,char *CrsShortName,char *DegShortName)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   DegShortName[0] = CrsShortName[0] = '\0';

   if (CrsCod > 0)
     {
      /***** Get the short name of a degree from database *****/
      sprintf (Query,"SELECT courses.ShortName,degrees.ShortName"
		     " FROM courses,degrees"
		     " WHERE courses.CrsCod='%ld'"
		     " AND courses.DegCod=degrees.DegCod",
	       CrsCod);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get the short name of a course") == 1)
	{
	 /***** Get the short name of this course *****/
	 row = mysql_fetch_row (mysql_res);
	 strcpy (CrsShortName,row[0]);
	 strcpy (DegShortName,row[1]);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/****************************** Remove a course ******************************/
/*****************************************************************************/

void Crs_RemoveCourseCompletely (long CrsCod)
  {
   char Query[128];

   /***** Empty course *****/
   Crs_EmptyCourseCompletely (CrsCod);

   /***** Remove course from table of last accesses to courses in database *****/
   sprintf (Query,"DELETE FROM crs_last WHERE CrsCod='%ld'",CrsCod);
   DB_QueryDELETE (Query,"can not remove a course");

   /***** Remove course from table of courses in database *****/
   sprintf (Query,"DELETE FROM courses WHERE CrsCod='%ld'",CrsCod);
   DB_QueryDELETE (Query,"can not remove a course");
  }

/*****************************************************************************/
/********** Empty a course (remove all its information and users) ************/
/*****************************************************************************/
// Start removing less important things to more important things;
// so, in case of failure, important things can been removed in the future

static void Crs_EmptyCourseCompletely (long CrsCod)
  {
   struct Course Crs;
   char PathRelCrs[PATH_MAX+1];
   char Query[512];

   /***** Get course data *****/
   Crs.CrsCod = CrsCod;
   Crs_GetDataOfCourseByCod (&Crs);

   /***** Remove all the students in the course *****/
   Enr_RemAllStdsInCrs (&Crs);

   /***** Set all the notifications from the course as removed,
          except notifications about new messages *****/
   Ntf_SetNotifInCrsAsRemoved (CrsCod,-1L);

   /***** Remove information of the course ****/
   /* Remove timetable of the course */
   sprintf (Query,"DELETE FROM timetable_crs WHERE CrsCod='%ld'",CrsCod);
   DB_QueryDELETE (Query,"can not remove the timetable of a course");

   /* Remove other information of the course */
   sprintf (Query,"DELETE FROM crs_info_src WHERE CrsCod='%ld'",CrsCod);
   DB_QueryDELETE (Query,"can not remove info sources of a course");

   sprintf (Query,"DELETE FROM crs_info_txt WHERE CrsCod='%ld'",CrsCod);
   DB_QueryDELETE (Query,"can not remove info of a course");

   /***** Remove all test exams made in course *****/
   Tst_RemoveAllExamsMadeInCrs (CrsCod);

   /***** Remove exam announcements in the course *****/
   /* Mark all exam announcements in the course as deleted */
   sprintf (Query,"UPDATE exam_announcements SET Status='%u' WHERE CrsCod='%ld'",
            (unsigned) Exa_DELETED_EXAM_ANNOUNCEMENT,CrsCod);
   DB_QueryUPDATE (Query,"can not remove exam announcements of a course");

   /***** Remove course cards of the course *****/
   /* Remove content of course cards */
   sprintf (Query,"DELETE FROM crs_records USING crs_record_fields,crs_records"
                  " WHERE crs_record_fields.CrsCod='%ld' AND crs_record_fields.FieldCod=crs_records.FieldCod",CrsCod);
   DB_QueryDELETE (Query,"can not remove content of cards in a course");

   /* Remove definition of fields in course cards */
   sprintf (Query,"DELETE FROM crs_record_fields WHERE CrsCod='%ld'",CrsCod);
   DB_QueryDELETE (Query,"can not remove fields of cards in a course");

   /***** Remove assignments of the course *****/
   Asg_RemoveCrsAssignments (CrsCod);

   /***** Remove attendance events of the course *****/
   Att_RemoveCrsAttEvents (CrsCod);

   /***** Remove surveys of the course *****/
   Svy_RemoveCrsSurveys (CrsCod);

   /***** Remove notices in the course *****/
   /* Copy all notices from the course to table of deleted notices */
   sprintf (Query,"INSERT INTO notices_deleted (NotCod,CrsCod,UsrCod,CreatTime,Content,NumNotif)"
                  " SELECT NotCod,CrsCod,UsrCod,CreatTime,Content,NumNotif FROM notices"
                  " WHERE CrsCod='%ld'",CrsCod);
   DB_QueryINSERT (Query,"can not remove notices in a course");
   /* Remove all notices from the course */
   sprintf (Query,"DELETE FROM notices WHERE CrsCod='%ld'",CrsCod);
   DB_QueryDELETE (Query,"can not remove notices in a course");

   /***** Remove all the threads and posts in course forums *****/
   /* Remove disabled posts */
   sprintf (Query,"DELETE FROM forum_disabled_post USING forum_thread,forum_post,forum_disabled_post"
                  " WHERE (forum_thread.ForumType='%u' OR forum_thread.ForumType='%u') AND forum_thread.Location='%ld' AND forum_thread.ThrCod=forum_post.ThrCod AND forum_post.PstCod=forum_disabled_post.PstCod",
            For_FORUM_COURSE_USRS,For_FORUM_COURSE_TCHS,CrsCod);
   DB_QueryDELETE (Query,"can not remove disabled posts in forums of a course");

   /* Remove posts */
   sprintf (Query,"DELETE FROM forum_post USING forum_thread,forum_post"
                  " WHERE (forum_thread.ForumType='%u' OR forum_thread.ForumType='%u') AND forum_thread.Location='%ld' AND forum_thread.ThrCod=forum_post.ThrCod",
            For_FORUM_COURSE_USRS,For_FORUM_COURSE_TCHS,CrsCod);
   DB_QueryDELETE (Query,"can not remove posts in forums of a course");

   /* Remove threads read */
   sprintf (Query,"DELETE FROM forum_thr_read USING forum_thread,forum_thr_read"
                  " WHERE (forum_thread.ForumType='%u' OR forum_thread.ForumType='%u') AND forum_thread.Location='%ld' AND forum_thread.ThrCod=forum_thr_read.ThrCod",
            For_FORUM_COURSE_USRS,For_FORUM_COURSE_TCHS,CrsCod);
   DB_QueryDELETE (Query,"can not remove read threads in forums of a course");

   /* Remove threads */
   sprintf (Query,"DELETE FROM forum_thread"
                  " WHERE (forum_thread.ForumType='%u' OR forum_thread.ForumType='%u') AND Location='%ld'",
            For_FORUM_COURSE_USRS,For_FORUM_COURSE_TCHS,CrsCod);
   DB_QueryDELETE (Query,"can not remove threads in forums of a course");

   /***** Remove tests in the course *****/
   /* Remove tests status in the course */
   sprintf (Query,"DELETE FROM tst_status WHERE CrsCod='%ld'",CrsCod);
   DB_QueryDELETE (Query,"can not remove status of tests of a course");

   /* Remove test configuration of the course */
   sprintf (Query,"DELETE FROM tst_config WHERE CrsCod='%ld'",
	    CrsCod);
   DB_QueryDELETE (Query,"can not remove configuration of tests of a course");

   /* Remove associations between test questions and test tags in the course */
   sprintf (Query,"DELETE FROM tst_question_tags USING tst_questions,tst_question_tags"
                  " WHERE tst_questions.CrsCod='%ld'"
                  " AND tst_questions.QstCod=tst_question_tags.QstCod",
            CrsCod);
   DB_QueryDELETE (Query,"can not remove tags associated to questions of tests of a course");

   /* Remove test tags in the course */
   sprintf (Query,"DELETE FROM tst_tags WHERE CrsCod='%ld'",
	    CrsCod);
   DB_QueryDELETE (Query,"can not remove tags of test of a course");

   /* Remove test answers in the course */
   sprintf (Query,"DELETE FROM tst_answers USING tst_questions,tst_answers"
                  " WHERE tst_questions.CrsCod='%ld'"
                  " AND tst_questions.QstCod=tst_answers.QstCod",
            CrsCod);
   DB_QueryDELETE (Query,"can not remove answers of tests of a course");

   /* Remove test questions in the course */
   sprintf (Query,"DELETE FROM tst_questions WHERE CrsCod='%ld'",
	    CrsCod);
   DB_QueryDELETE (Query,"can not remove test questions of a course");

   /***** Remove groups in the course *****/
   /* Remove all the users in groups in the course */
   sprintf (Query,"DELETE FROM crs_grp_usr"
	          " USING crs_grp_types,crs_grp,crs_grp_usr"
                  " WHERE crs_grp_types.CrsCod='%ld'"
                  " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
                  " AND crs_grp.GrpCod=crs_grp_usr.GrpCod",
            CrsCod);
   DB_QueryDELETE (Query,"can not remove users from groups of a course");

   /* Remove all the groups in the course */
   sprintf (Query,"DELETE FROM crs_grp"
	          " USING crs_grp_types,crs_grp"
                  " WHERE crs_grp_types.CrsCod='%ld'"
                  " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod",
            CrsCod);
   DB_QueryDELETE (Query,"can not remove groups of a course");

   /* Remove all the group types in the course */
   sprintf (Query,"DELETE FROM crs_grp_types"
	          " WHERE CrsCod='%ld'",
	    CrsCod);
   DB_QueryDELETE (Query,"can not remove types of group of a course");

   /***** Remove users' requests for inscription in the course *****/
   sprintf (Query,"DELETE FROM crs_usr_requests WHERE CrsCod='%ld'",
	    CrsCod);
   DB_QueryDELETE (Query,"can not remove requests for inscription to a course");

   /***** Remove possible users remaining in the course (teachers) *****/
   sprintf (Query,"DELETE FROM crs_usr WHERE CrsCod='%ld'",
	    CrsCod);
   DB_QueryDELETE (Query,"can not remove users from a course");

   /***** Remove information related to files in course *****/
   /* Remove clipboards related to the course */
   sprintf (Query,"DELETE FROM clipboard WHERE CrsCod='%ld'",
	    CrsCod);
   DB_QueryDELETE (Query,"can not remove clipboards in a course");

   /* Remove expanded folders in the course */
   sprintf (Query,"DELETE FROM expanded_folders WHERE CrsCod='%ld'",
	    CrsCod);
   DB_QueryDELETE (Query,"can not remove expanded folders in a course");

   /* Remove format of files of marks of the course */
   sprintf (Query,"DELETE FROM marks_properties"
	          " USING files,marks_properties"
	          " WHERE files.CrsCod='%ld'"
	          " AND files.FilCod=marks_properties.FilCod",
	    CrsCod);
   DB_QueryDELETE (Query,"can not remove the properties of marks associated to a course");

   /* Remove files in the course from database */
   Brw_RemoveFilesFromDB (CrsCod,-1L,-1L);

   /* Remove size of file zones in the course from database */
   Brw_RemoveSizeOfFileTreeFromDB (CrsCod,-1L,-1L);

   /***** Remove directories of the course *****/
   sprintf (PathRelCrs,"%s/%s/%ld",
            Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_CRS,CrsCod);
   Brw_RemoveTree (PathRelCrs);
   sprintf (PathRelCrs,"%s/%s/%ld",
            Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CRS,CrsCod);
   Brw_RemoveTree (PathRelCrs);
  }

/*****************************************************************************/
/************** Change the institutional code of a course ********************/
/*****************************************************************************/

void Crs_ChangeInsCrsCod (void)
  {
   extern const char *Txt_The_institutional_code_of_the_course_X_has_changed_to_Y;
   extern const char *Txt_The_institutional_code_of_the_course_X_has_not_changed;
   extern const char *Txt_You_dont_have_permission_to_edit_this_course;
   char NewInstitutionalCrsCod[Crs_LENGTH_INSTITUTIONAL_CRS_COD+1];
   struct Course *Crs;

   Crs = &Gbl.Degs.EditingCrs;

   /***** Get parameters from form *****/
   /* Get course code */
   if ((Crs->CrsCod = Crs_GetParamOtherCrsCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of course is missing.");

   /* Get institutional code */
   Par_GetParToText ("InsCrsCod",NewInstitutionalCrsCod,Crs_LENGTH_INSTITUTIONAL_CRS_COD);

   /* Get data of the course */
   Crs_GetDataOfCourseByCod (Crs);

   if (Crs_CheckIfICanEdit (Crs))
     {
      /***** Change the institutional course code *****/
      if (strcmp (NewInstitutionalCrsCod,Crs->InstitutionalCrsCod))
        {
         Crs_UpdateCurrentInstitutionalCrsCod (Crs,NewInstitutionalCrsCod);
         sprintf (Gbl.Message,Txt_The_institutional_code_of_the_course_X_has_changed_to_Y,
                  Crs->ShortName,NewInstitutionalCrsCod);
         Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
        }
      else	// The same institutional code
        {
         sprintf (Gbl.Message,Txt_The_institutional_code_of_the_course_X_has_not_changed,
                  Crs->ShortName);
         Lay_ShowAlert (Lay_INFO,Gbl.Message);
        }

      /***** Put link to go to course changed *****/
      if (Crs->CrsCod != Gbl.CurrentCrs.Crs.CrsCod)	// If changing other course different than the current one...
         Crs_PutLinkToGoToCrs (Crs);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_You_dont_have_permission_to_edit_this_course);

   /***** Show the form again *****/
   Crs_ReqEditCourses ();
  }

/*****************************************************************************/
/****** Change the degree of a course (move course to another degree) ********/
/*****************************************************************************/

void Crs_ChangeCrsDegree (void)
  {
   extern const char *Txt_In_the_year_X_of_the_degree_Y_already_existed_a_course_with_the_name_Z;
   extern const char *Txt_YEAR_OF_DEGREE[1+Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_The_course_X_has_been_moved_to_the_degree_Y;
   extern const char *Txt_The_year_X_is_not_enabled_on_the_degree_Y;
   extern const char *Txt_You_dont_have_permission_to_move_courses_to_the_degree_X;
   struct Course *Crs;
   bool ICanChangeCrsToNewDeg;
   char Query[512];
   struct Degree NewDeg;

   Crs = &Gbl.Degs.EditingCrs;

   /***** Get parameters from form *****/
   /* Get course code */
   if ((Crs->CrsCod = Crs_GetParamOtherCrsCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of course is missing.");

   /* Get new degree code */
   if ((NewDeg.DegCod = Deg_GetParamOtherDegCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of degree is missing.");

   /* Check if I have permission to change course to this degree */
   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
      ICanChangeCrsToNewDeg = true;
   else	if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADMIN)
      ICanChangeCrsToNewDeg = Usr_CheckIfUsrIsAdmOfDeg (Gbl.Usrs.Me.UsrDat.UsrCod,NewDeg.DegCod);
   else
      ICanChangeCrsToNewDeg = false;

   /***** Get data of course and new degree *****/
   Crs_GetDataOfCourseByCod (Crs);
   Deg_GetDataOfDegreeByCod (&NewDeg);

   /***** If I have permission to change course to this new degree... *****/
   if (ICanChangeCrsToNewDeg)
     {
      /***** If new degree has current course year... *****/
      if (Deg_CheckIfYearIsValidInDeg (Crs->Year,&NewDeg))
        {
         /***** If name of course was in database in the new degree... *****/
         if (Crs_CheckIfCourseNameExistsInCourses (NewDeg.DegCod,Crs->Year,"ShortName",Crs->ShortName,-1L))
           {
            sprintf (Gbl.Message,Txt_In_the_year_X_of_the_degree_Y_already_existed_a_course_with_the_name_Z,
                     Txt_YEAR_OF_DEGREE[Crs->Year],NewDeg.FullName,Crs->ShortName);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
           }
         else if (Crs_CheckIfCourseNameExistsInCourses (NewDeg.DegCod,Crs->Year,"FullName",Crs->FullName,-1L))
           {
            sprintf (Gbl.Message,Txt_In_the_year_X_of_the_degree_Y_already_existed_a_course_with_the_name_Z,
                     Txt_YEAR_OF_DEGREE[Crs->Year],NewDeg.FullName,Crs->FullName);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
           }
         else	// Update degree in database
           {
            /***** Update degree in table of courses *****/
            sprintf (Query,"UPDATE courses SET DegCod='%ld' WHERE CrsCod='%ld'",
                     NewDeg.DegCod,Crs->CrsCod);
            DB_QueryUPDATE (Query,"can not move course to another degree");

            /***** Write message to show the change made *****/
            sprintf (Gbl.Message,Txt_The_course_X_has_been_moved_to_the_degree_Y,
                     Crs->FullName,NewDeg.FullName);
            Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

            /***** Put link to go to course changed *****/
            if (Crs->CrsCod != Gbl.CurrentCrs.Crs.CrsCod)	// If changing other course different than the current one...
               Crs_PutLinkToGoToCrs (Crs);
           }
        }
      else	// New degree has no current course year
        {
         sprintf (Gbl.Message,Txt_The_year_X_is_not_enabled_on_the_degree_Y,
                  Txt_YEAR_OF_DEGREE[Crs->Year],NewDeg.FullName);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }
     }
   else	// I have no permission to change course to this new degree
     {
      sprintf (Gbl.Message,Txt_You_dont_have_permission_to_move_courses_to_the_degree_X,
               NewDeg.FullName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }

   /***** Show the form again *****/
   Crs_ReqEditCourses ();
  }

/*****************************************************************************/
/************************ Change the year of a course ************************/
/*****************************************************************************/

void Crs_ChangeCrsYear (void)
  {
   extern const char *Txt_The_course_X_already_exists_in_year_Y;
   extern const char *Txt_YEAR_OF_DEGREE[1+Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_The_year_of_the_course_X_has_changed;
   extern const char *Txt_The_year_X_is_not_enabled_on_the_degree_Y;
   extern const char *Txt_You_dont_have_permission_to_edit_this_course;
   struct Course *Crs;
   struct Degree Deg;
   char Query[512];
   char YearStr[2+1];
   unsigned NewYear;

   Crs = &Gbl.Degs.EditingCrs;

   /***** Get parameters from form *****/
   /* Get course code */
   if ((Crs->CrsCod = Crs_GetParamOtherCrsCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of course is missing.");

   /* Get parameter with year */
   Par_GetParToText ("OthCrsYear",YearStr,2);
   NewYear = Deg_ConvStrToYear (YearStr);

   Crs_GetDataOfCourseByCod (Crs);

   if (Crs_CheckIfICanEdit (Crs))
     {
      Deg.DegCod = Crs->DegCod;
      Deg_GetDataOfDegreeByCod (&Deg);

      if (Deg_CheckIfYearIsValidInDeg (NewYear,&Deg))	// If year is valid
        {
         /***** If name of course was in database in the new year... *****/
         if (Crs_CheckIfCourseNameExistsInCourses (Crs->DegCod,NewYear,"ShortName",Crs->ShortName,-1L))
           {
            sprintf (Gbl.Message,Txt_The_course_X_already_exists_in_year_Y,
                     Crs->ShortName,Txt_YEAR_OF_DEGREE[NewYear]);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
           }
         else if (Crs_CheckIfCourseNameExistsInCourses (Crs->DegCod,NewYear,"FullName",Crs->FullName,-1L))
           {
            sprintf (Gbl.Message,Txt_The_course_X_already_exists_in_year_Y,
                     Crs->FullName,Txt_YEAR_OF_DEGREE[NewYear]);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
           }
         else	// Update year in database
           {
            /***** Update year in table of courses *****/
            sprintf (Query,"UPDATE courses SET Year='%u' WHERE CrsCod='%ld'",
                     NewYear,Crs->CrsCod);
            DB_QueryUPDATE (Query,"can not update the year of a course");

            Crs->Year = NewYear;

            /***** Write message to show the change made *****/
            sprintf (Gbl.Message,Txt_The_year_of_the_course_X_has_changed,
                     Crs->ShortName);
            Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

            /***** Put link to go to course changed *****/
            if (Crs->CrsCod != Gbl.CurrentCrs.Crs.CrsCod)	// If changing other course different than the current one...
               Crs_PutLinkToGoToCrs (Crs);
           }
        }
      else	// Year not valid
        {
         sprintf (Gbl.Message,Txt_The_year_X_is_not_enabled_on_the_degree_Y,
                  Txt_YEAR_OF_DEGREE[NewYear],Deg.FullName);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_You_dont_have_permission_to_edit_this_course);

   /***** Show the form again *****/
   Crs_ReqEditCourses ();
  }

/*****************************************************************************/
/********************** Change the semester of a course **********************/
/*****************************************************************************/

void Crs_ChangeCrsSemester (void)
  {
   extern const char *Txt_The_semester_of_the_course_X_has_changed;
   extern const char *Txt_You_dont_have_permission_to_edit_this_course;
   struct Course *Crs;
   char Query[512];
   char SemesterStr[1+1];
   unsigned NewSemester;

   Crs = &Gbl.Degs.EditingCrs;

   /***** Get parameters from form *****/
   /* Get course code */
   if ((Crs->CrsCod = Crs_GetParamOtherCrsCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of course is missing.");

   /* Get parameter with semester */
   Par_GetParToText ("OthCrsSem",SemesterStr,1);
   NewSemester = Deg_ConvStrToSemester (SemesterStr);

   /***** Get data of course *****/
   Crs_GetDataOfCourseByCod (Crs);

   if (Crs_CheckIfICanEdit (Crs))
     {
      /***** Update semester in table of courses *****/
      sprintf (Query,"UPDATE courses SET Semester='%u' WHERE CrsCod='%ld'",
               NewSemester,Crs->CrsCod);
      DB_QueryUPDATE (Query,"can not update the semester of a course");

      Crs->Semester = NewSemester;

      /***** Write message to show the change made *****/
      sprintf (Gbl.Message,Txt_The_semester_of_the_course_X_has_changed,
               Crs->ShortName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

      /***** Put link to go to course changed *****/
      if (Crs->CrsCod != Gbl.CurrentCrs.Crs.CrsCod)	// If changing other course different than the current one...
         Crs_PutLinkToGoToCrs (Crs);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_You_dont_have_permission_to_edit_this_course);

   /***** Show the form again *****/
   Crs_ReqEditCourses ();
  }

/*****************************************************************************/
/******** Change the institutional course code of the current course *********/
/*****************************************************************************/

void Crs_UpdateCurrentInstitutionalCrsCod (struct Course *Crs,const char *NewInstitutionalCrsCod)
  {
   char Query[512];

   strncpy (Gbl.CurrentCrs.Crs.InstitutionalCrsCod,NewInstitutionalCrsCod,Crs_LENGTH_INSTITUTIONAL_CRS_COD);
   Gbl.CurrentCrs.Crs.InstitutionalCrsCod[Crs_LENGTH_INSTITUTIONAL_CRS_COD] = '\0';

   /***** Update institutional course code in table of courses *****/
   sprintf (Query,"UPDATE courses SET InsCrsCod='%s' WHERE CrsCod='%ld'",
            NewInstitutionalCrsCod,Crs->CrsCod);
   DB_QueryUPDATE (Query,"can not update the institutional code of the current course");

   /***** Copy institutional course code *****/
   strncpy (Crs->InstitutionalCrsCod,NewInstitutionalCrsCod,Crs_LENGTH_INSTITUTIONAL_CRS_COD);
   Crs->InstitutionalCrsCod[Crs_LENGTH_INSTITUTIONAL_CRS_COD] = '\0';
  }

/*****************************************************************************/
/********************* Change the short name of a course *********************/
/*****************************************************************************/

void Crs_RenameCourseShort (void)
  {
   struct Course *Crs;

   Crs = &Gbl.Degs.EditingCrs;

   if (Crs_RenameCourse (Crs,Cns_SHORT_NAME))
      if (Crs->CrsCod == Gbl.CurrentCrs.Crs.CrsCod)	// If renaming current course...
         strcpy (Gbl.CurrentCrs.Crs.ShortName,Crs->ShortName);	// Overwrite current course name in order to show correctly in page title and heading
  }

/*****************************************************************************/
/********************* Change the full name of a course **********************/
/*****************************************************************************/

void Crs_RenameCourseFull (void)
  {
   struct Course *Crs;

   Crs = &Gbl.Degs.EditingCrs;

   if (Crs_RenameCourse (Crs,Cns_FULL_NAME))
      if (Crs->CrsCod == Gbl.CurrentCrs.Crs.CrsCod)	// If renaming current course...
         strcpy (Gbl.CurrentCrs.Crs.FullName,Crs->FullName);	// Overwrite current course name in order to show correctly in page title and heading
  }

/*****************************************************************************/
/************************ Change the name of a course ************************/
/*****************************************************************************/
// Returns true if the course is renamed
// Returns false if the course is not renamed

static bool Crs_RenameCourse (struct Course *Crs,Cns_ShortOrFullName_t ShortOrFullName)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_course_X_empty;
   extern const char *Txt_The_course_X_already_exists;
   extern const char *Txt_The_name_of_the_course_X_has_changed_to_Y;
   extern const char *Txt_The_name_of_the_course_X_has_not_changed;
   extern const char *Txt_You_dont_have_permission_to_edit_this_course;
   char Query[512];
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxLength = 0;		// Initialized to avoid warning
   char *CurrentCrsName = NULL;		// Initialized to avoid warning
   char NewCrsName[Crs_MAX_LENGTH_COURSE_FULL_NAME+1];
   bool CourseHasBeenRenamed = false;

   switch (ShortOrFullName)
     {
      case Cns_SHORT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxLength = Crs_MAX_LENGTH_COURSE_SHORT_NAME;
         CurrentCrsName = Crs->ShortName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxLength = Crs_MAX_LENGTH_COURSE_FULL_NAME;
         CurrentCrsName = Crs->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the course */
   if ((Crs->CrsCod = Crs_GetParamOtherCrsCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of course is missing.");

   /* Get the new name for the course */
   Par_GetParToText (ParamName,NewCrsName,MaxLength);

   /***** Get from the database the data of the degree *****/
   Crs_GetDataOfCourseByCod (Crs);

   if (Crs_CheckIfICanEdit (Crs))
     {
      /***** Check if new name is empty *****/
      if (!NewCrsName[0])
        {
         sprintf (Gbl.Message,Txt_You_can_not_leave_the_name_of_the_course_X_empty,
                  CurrentCrsName);
         Gbl.Error = true;
        }
      else
        {
         /***** Check if old and new names are the same (this happens when user press enter with no changes in the form) *****/
         if (strcmp (CurrentCrsName,NewCrsName))	// Different names
           {
            /***** If course was in database... *****/
            if (Crs_CheckIfCourseNameExistsInCourses (Crs->DegCod,Crs->Year,ParamName,NewCrsName,Crs->CrsCod))
              {
               sprintf (Gbl.Message,Txt_The_course_X_already_exists,
                        NewCrsName);
               Gbl.Error = true;
              }
            else
              {
               /* Update the table changing old name by new name */
               sprintf (Query,"UPDATE courses SET %s='%s' WHERE CrsCod='%ld'",
                        FieldName,NewCrsName,Crs->CrsCod);
               DB_QueryUPDATE (Query,"can not update the name of a course");

               /***** Write message to show the change made *****/
               sprintf (Gbl.Message,Txt_The_name_of_the_course_X_has_changed_to_Y,
                        CurrentCrsName,NewCrsName);

               CourseHasBeenRenamed = true;
              }
           }
         else	// The same name
            sprintf (Gbl.Message,Txt_The_name_of_the_course_X_has_not_changed,
                     CurrentCrsName);
        }
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_You_dont_have_permission_to_edit_this_course);

   return CourseHasBeenRenamed;
  }

/*****************************************************************************/
/*********************** Change the status of a course ***********************/
/*****************************************************************************/

void Crs_ChangeCrsStatus (void)
  {
   extern const char *Txt_The_status_of_the_course_X_has_changed;
   struct Course *Crs;
   char Query[256];
   char UnsignedNum[10+1];
   Crs_Status_t Status;
   Crs_StatusTxt_t StatusTxt;

   Crs = &Gbl.Degs.EditingCrs;

   /***** Get parameters from form *****/
   /* Get course code */
   if ((Crs->CrsCod = Crs_GetParamOtherCrsCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of course is missing.");

   /* Get parameter with status */
   Par_GetParToText ("Status",UnsignedNum,1);
   if (sscanf (UnsignedNum,"%u",&Status) != 1)
      Lay_ShowErrorAndExit ("Wrong status.");
   StatusTxt = Crs_GetStatusTxtFromStatusBits (Status);
   Status = Crs_GetStatusBitsFromStatusTxt (StatusTxt);	// New status

   /***** Get data of course *****/
   Crs_GetDataOfCourseByCod (Crs);

   /***** Update status in table of courses *****/
   sprintf (Query,"UPDATE courses SET Status='%u' WHERE CrsCod='%ld'",
            (unsigned) Status,Crs->CrsCod);
   DB_QueryUPDATE (Query,"can not update the status of a course");

   Crs->Status = Status;

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_The_status_of_the_course_X_has_changed,
            Crs->ShortName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Put link to go to course changed *****/
   if (Crs->CrsCod != Gbl.CurrentCrs.Crs.CrsCod)	// If changing other course different than the current one...
      Crs_PutLinkToGoToCrs (Crs);

   /***** Show the form again *****/
   Crs_ReqEditCourses ();
  }

/*****************************************************************************/
/************* Show message of success after changing a course ***************/
/*****************************************************************************/

void Crs_ContEditAfterChgCrs (void)
  {
   /***** Write error/success message showing the change made *****/
   Lay_ShowAlert (Gbl.Error ? Lay_WARNING :
	                      Lay_INFO,
	          Gbl.Message);

   /***** Put link to go to course changed *****/
   if (Gbl.Degs.EditingCrs.CrsCod != Gbl.CurrentCrs.Crs.CrsCod)	// If changing other course different than the current one...
      Crs_PutLinkToGoToCrs (&Gbl.Degs.EditingCrs);

   /***** Show the form again *****/
   Crs_ReqEditCourses ();
  }

/*****************************************************************************/
/******************** Put centered link to go to course **********************/
/*****************************************************************************/

static void Crs_PutLinkToGoToCrs (struct Course *Crs)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Go_to_X;

   fprintf (Gbl.F.Out,"<div align=\"center\" class=\"%s\">",
            The_ClassFormul[Gbl.Prefs.Theme]);
   Act_FormGoToStart (ActSeeCrsInf);
   Crs_PutParamCrsCod (Crs->CrsCod);
   sprintf (Gbl.Title,Txt_Go_to_X,Crs->ShortName);
   Act_LinkFormSubmit (Gbl.Title,The_ClassFormul[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s</a>"
	              "</form>"
	              "</div>",
	    Gbl.Title);
  }

/*****************************************************************************/
/************************* Select one of my courses **************************/
/*****************************************************************************/

void Crs_ReqSelectOneOfMyCourses (void)
  {
   extern const char *Txt_You_are_not_enrolled_in_any_course[Usr_NUM_SEXS];

   /***** Search / select more courses *****/
   fprintf (Gbl.F.Out,"<div align=\"center\">");
   Usr_PutFormToSearchCourses ();
   fprintf (Gbl.F.Out,"</div>");

   /***** Select one of my courses *****/
   /* Fill the list with the courses I belong to, if not filled */
   Usr_GetMyCourses ();

   if (Gbl.Usrs.Me.MyCourses.Num)
      Crs_WriteListMyCoursesToSelectOne ();
   else
      Lay_ShowAlert (Lay_INFO,Txt_You_are_not_enrolled_in_any_course[Gbl.Usrs.Me.UsrDat.Sex]);
  }

/*****************************************************************************/
/******************* Put a link (form) to search courses *********************/
/*****************************************************************************/

static void Usr_PutFormToSearchCourses (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Search_courses;

   /***** Put form to search / select courses *****/
   Act_FormStart ( Gbl.CurrentCrs.Crs.CrsCod > 0 ? ActCrsReqSch :
		  (Gbl.CurrentDeg.Deg.DegCod > 0 ? ActDegReqSch :
		  (Gbl.CurrentCtr.Ctr.CtrCod > 0 ? ActCtrReqSch :
		  (Gbl.CurrentIns.Ins.InsCod > 0 ? ActInsReqSch :
		  (Gbl.CurrentCty.Cty.CtyCod > 0 ? ActCtyReqSch :
						   ActSysReqSch)))));
   Sco_PutParamScope (Sco_SCOPE_PLATFORM);
   Par_PutHiddenParamUnsigned ("WhatToSearch",(unsigned) Sch_SEARCH_COURSES);
   Act_LinkFormSubmit (Txt_Search_courses,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("search",Txt_Search_courses,Txt_Search_courses);
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/**** Put a link (form) to remove all the students in the current course *****/
/*****************************************************************************/

void Usr_PutFormToSelectMyCourses (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_My_courses;

   /***** Put form to search / select courses *****/
   Act_FormStart (ActMyCrs);
   Act_LinkFormSubmit (Txt_My_courses,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("hierarchy",Txt_My_courses,Txt_My_courses);
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/******************** Write parameter with code of course ********************/
/*****************************************************************************/

void Crs_PutParamCrsCod (long CrsCod)
  {
   Par_PutHiddenParamLong ("CrsCod",CrsCod);
  }

/*****************************************************************************/
/******************** Write parameter with code of course ********************/
/*****************************************************************************/

static void Crs_PutParamOtherCrsCod (long CrsCod)
  {
   Par_PutHiddenParamLong ("OthCrsCod",CrsCod);
  }

/*****************************************************************************/
/********************* Get parameter with code of course *********************/
/*****************************************************************************/

static long Crs_GetParamOtherCrsCod (void)
  {
   char LongStr[1+10+1];

   /***** Get parameter with code of course *****/
   Par_GetParToText ("OthCrsCod",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/************************** Write courses of a user **************************/
/*****************************************************************************/

void Crs_GetAndWriteCrssOfAUsr (long UsrCod,Rol_Role_t Role)
  {
   extern const char *Txt_Degree;
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_Semester_ABBREVIATION;
   extern const char *Txt_Course;
   extern const char *Txt_Students_ABBREVIATION;
   extern const char *Txt_Teachers_ABBREVIATION;
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;

   /***** Get courses of a user from database *****/
   sprintf (Query,"SELECT degrees.DegCod,courses.CrsCod,degrees.Logo,degrees.ShortName,degrees.FullName,"
                  "courses.Year,courses.Semester,courses.FullName,centres.ShortName,crs_usr.Accepted"
                  " FROM crs_usr,courses,degrees,centres"
                  " WHERE crs_usr.UsrCod='%ld'"
                  " AND crs_usr.Role='%u'"
                  " AND crs_usr.CrsCod=courses.CrsCod"
                  " AND courses.DegCod=degrees.DegCod"
                  " AND degrees.CtrCod=centres.CtrCod"
                  " ORDER BY degrees.FullName,courses.Year,courses.Semester,courses.FullName",
            UsrCod,(unsigned) Role);

   /***** List the courses (one row per course) *****/
   if ((NumCrss = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get courses of a user")))
     {
      /* Heading row */
      fprintf (Gbl.F.Out,"<tr>"
                         "<th class=\"BM\"></th>"
                         "<th class=\"BM\"></th>"
                         "<th align=\"left\" class=\"TIT_TBL\">%s</th>"
                         "<th align=\"center\" class=\"TIT_TBL\">%s</th>"
                         "<th align=\"center\" class=\"TIT_TBL\">%s</th>"
                         "<th align=\"left\" class=\"TIT_TBL\">%s</th>"
                         "<th align=\"right\" class=\"TIT_TBL\">%s</th>"
                         "<th align=\"right\" class=\"TIT_TBL\">%s</th>"
                         "</tr>",
               Txt_Degree,
               Txt_Year_OF_A_DEGREE,
               Txt_Semester_ABBREVIATION,
               Txt_Course,
               Txt_Students_ABBREVIATION,
               Txt_Teachers_ABBREVIATION);

      /* Write courses */
      for (NumCrs = 1;
	   NumCrs <= NumCrss;
	   NumCrs++)
        {
         /* Get next course */
         row = mysql_fetch_row (mysql_res);

         /* Write data of this course */
         Crs_WriteRowCrsData (NumCrs,row,true);
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************************** List courses found ***************************/
/*****************************************************************************/
// Returns number of courses found

unsigned Crs_ListCrssFound (const char *Query)
  {
   extern const char *Txt_Courses;
   extern const char *Txt_course;
   extern const char *Txt_courses;
   extern const char *Txt_Degree;
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_Semester_ABBREVIATION;
   extern const char *Txt_Course;
   extern const char *Txt_Students_ABBREVIATION;
   extern const char *Txt_Teachers_ABBREVIATION;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;

   /***** Query database *****/
   NumCrss = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get courses");

   /***** List the courses (one row per course) *****/
   if (NumCrss)
     {
      /***** Write heading *****/
      Lay_StartRoundFrameTable10 (NULL,2,Txt_Courses);

      /* Number of courses found */
      fprintf (Gbl.F.Out,"<tr>"
			 "<td align=\"center\" class=\"TIT_TBL\" colspan=\"7\">");
      if (NumCrss == 1)
	 fprintf (Gbl.F.Out,"1 %s",Txt_course);
      else
	 fprintf (Gbl.F.Out,"%u %s",NumCrss,Txt_courses);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");

      /* Heading row */
      fprintf (Gbl.F.Out,"<tr>"
			 "<th class=\"BM\"></th>"
			 "<th align=\"left\" class=\"TIT_TBL\">%s</th>"
			 "<th align=\"center\" class=\"TIT_TBL\">%s</th>"
			 "<th align=\"center\" class=\"TIT_TBL\">%s</th>"
			 "<th align=\"left\" class=\"TIT_TBL\">%s</th>"
			 "<th align=\"right\" class=\"TIT_TBL\">%s</th>"
			 "<th align=\"right\" class=\"TIT_TBL\">%s</th>"
			 "</tr>",
	       Txt_Degree,
	       Txt_Year_OF_A_DEGREE,
	       Txt_Semester_ABBREVIATION,
	       Txt_Course,
	       Txt_Students_ABBREVIATION,
	       Txt_Teachers_ABBREVIATION);

      /* Write courses */
      for (NumCrs = 1;
	   NumCrs <= NumCrss;
	   NumCrs++)
	{
	 /* Get next course */
	 row = mysql_fetch_row (mysql_res);

	 /* Write data of this course */
	 Crs_WriteRowCrsData (NumCrs,row,false);
	}

      /***** Table end *****/
      Lay_EndRoundFrameTable10 ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCrss;
  }

/*****************************************************************************/
/************** Write the data of a course (result of a query) ***************/
/*****************************************************************************/

static void Crs_WriteRowCrsData (unsigned NumCrs,MYSQL_ROW row,bool WriteColumnAccepted)
  {
   extern const char *Txt_Enrollment_confirmed;
   extern const char *Txt_Enrollment_not_confirmed;
   extern const char *Txt_Go_to_X;
   extern const char *Txt_YEAR_OF_DEGREE[1+Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_SEMESTER_OF_YEAR[1+2];
   long DegCod;
   long CrsCod;
   unsigned NumStds;
   unsigned NumTchs;
   const char *Style;
   const char *StyleNoBR;
   const char *BgColor;
   bool Accepted;

   /***** Get degree code (row[0]) *****/
   if ((DegCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
      Lay_ShowErrorAndExit ("Wrong code of degree.");

   /***** Get course code (row[1]) *****/
   if ((CrsCod = Str_ConvertStrCodToLongCod (row[1])) < 0)
      Lay_ShowErrorAndExit ("Wrong code of course.");

   /***** Get number of students and teachers in this course *****/
   NumStds = Usr_GetNumUsrsInCrs (Rol_ROLE_STUDENT,CrsCod);
   NumTchs = Usr_GetNumUsrsInCrs (Rol_ROLE_TEACHER,CrsCod);
   if (NumStds + NumTchs)
     {
      Style = "DAT_N";
      StyleNoBR = "DAT_NOBR_N";
     }
   else
     {
      Style = "DAT";
      StyleNoBR = "DAT_NOBR";
     }
   BgColor = (CrsCod == Gbl.CurrentCrs.Crs.CrsCod) ? VERY_LIGHT_BLUE :
                                                     Gbl.ColorRows[Gbl.RowEvenOdd];

   /***** Start row *****/
   fprintf (Gbl.F.Out,"<tr>");

   /***** Teacher has accepted joining to this course/to any course in degree/to any course? *****/
   if (WriteColumnAccepted)
     {
      Accepted = (Str_ConvertToUpperLetter (row[9][0]) == 'Y');
      fprintf (Gbl.F.Out,"<td class=\"BT\" bgcolor=\"%s\">"
	                 "<img src=\"%s/%s16x16.gif\""
	                 " alt=\"\" title=\"%s\" class=\"ICON16x16\" />"
	                 "</td>",
               BgColor,
               Gbl.Prefs.IconsURL,
               Accepted ? "ok_on" :
        	          "tr",
               Accepted ? Txt_Enrollment_confirmed :
        	          Txt_Enrollment_not_confirmed);
     }

   /***** Write number of course in this search *****/
   fprintf (Gbl.F.Out,"<td align=\"right\" valign=\"top\" class=\"%s\" bgcolor=\"%s\">%u</td>",
            StyleNoBR,BgColor,NumCrs);

   /***** Write degree logo (row[2]), degree short name (row[3]) and centre short name (row[8]) *****/
   fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"top\" class=\"%s\" bgcolor=\"%s\">",
            StyleNoBR,BgColor);
   Act_FormGoToStart (ActSeeDegInf);
   Deg_PutParamDegCod (DegCod);
   sprintf (Gbl.Title,Txt_Go_to_X,row[4]);
   Act_LinkFormSubmit (Gbl.Title,StyleNoBR);
   fprintf (Gbl.F.Out,"<img src=\"%s/%s/%s16x16.gif\" alt=\"%s\""
                      " class=\"ICON16x16\" style=\"vertical-align:top;\" />"
                      "&nbsp;%s (%s)</a>"
                      "</form>"
                      "</td>",
            Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_DEGREES,row[2],
            row[3],row[3],row[8]);

   /***** Write year (row[5]) *****/
   fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"top\" class=\"%s\" bgcolor=\"%s\">%s</td>",
            Style,BgColor,Txt_YEAR_OF_DEGREE[Deg_ConvStrToYear (row[5])]);

   /***** Write semester (row[6]) *****/
   fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"top\" class=\"%s\" bgcolor=\"%s\">%s</td>",
            Style,BgColor,Txt_SEMESTER_OF_YEAR[Deg_ConvStrToSemester (row[6])]);

   /***** Write course full name (row[7]) *****/
   fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"top\" class=\"%s\" bgcolor=\"%s\">",
            Style,BgColor);
   Act_FormGoToStart (ActSeeCrsInf);
   Crs_PutParamCrsCod (CrsCod);
   sprintf (Gbl.Title,Txt_Go_to_X,row[7]);
   Act_LinkFormSubmit (Gbl.Title,Style);
   fprintf (Gbl.F.Out,"%s</a>"
	              "</form>"
	              "</td>",
	    row[7]);

   /***** Write number of students in course *****/
   fprintf (Gbl.F.Out,"<td align=\"right\" valign=\"top\" class=\"%s\" bgcolor=\"%s\">%u</td>",
            Style,BgColor,NumStds);

   /***** Write number of teachers in course *****/
   fprintf (Gbl.F.Out,"<td align=\"right\" valign=\"top\" class=\"%s\" bgcolor=\"%s\">%u</td>"
	              "</tr>",
            Style,BgColor,NumTchs);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/***************** Update my last click in current course ********************/
/*****************************************************************************/

void Crs_UpdateCrsLast (void)
  {
   char Query[256];

   if (Gbl.CurrentCrs.Crs.CrsCod > 0 &&
       Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_STUDENT)
     {
      /***** Update my last access to current course *****/
      sprintf (Query,"REPLACE INTO crs_last (CrsCod,LastTime)"
	             " VALUES ('%ld',NOW())",
	       Gbl.CurrentCrs.Crs.CrsCod);
      DB_QueryUPDATE (Query,"can not update last access to current course");
     }
  }

/*****************************************************************************/
/********************** Write form to remove old courses *********************/
/*****************************************************************************/

void Crs_AskRemoveOldCrss (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Eliminate_all_courses_whithout_users_PART_1_OF_2;
   extern const char *Txt_Eliminate_all_courses_whithout_users_PART_2_OF_2;
   extern const char *Txt_Eliminate;
   unsigned MonthsWithoutAccess = Crs_DEF_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS;
   unsigned i;

   /***** Form to request number of months without clicks *****/
   fprintf (Gbl.F.Out,"<div align=\"center\">");
   Act_FormStart (ActRemOldCrs);
   fprintf (Gbl.F.Out,"<span class=\"%s\">%s </span>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Eliminate_all_courses_whithout_users_PART_1_OF_2);
   fprintf (Gbl.F.Out,"<select name=\"Months\">");
   for (i  = Crs_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS;
        i <= Crs_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS;
        i++)
     {
      fprintf (Gbl.F.Out,"<option");
      if (i == MonthsWithoutAccess)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%u</option>",i);
     }
   fprintf (Gbl.F.Out,"</select>"
                      "<span class=\"%s\"> ",
            The_ClassFormul[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,Txt_Eliminate_all_courses_whithout_users_PART_2_OF_2,
            Cfg_PLATFORM_SHORT_NAME);
   fprintf (Gbl.F.Out,"</span>");

   /***** Send button*****/
   Lay_PutSendButton (Txt_Eliminate);
   fprintf (Gbl.F.Out,"</form>"
                      "</div>");
  }

/*****************************************************************************/
/**************************** Remove old courses *****************************/
/*****************************************************************************/

void Crs_RemoveOldCrss (void)
  {
   extern const char *Txt_Eliminating_X_courses_whithout_users_and_with_more_than_Y_months_without_access;
   extern const char *Txt_X_courses_have_been_eliminated;
   char UnsignedStr[10+1];
   unsigned MonthsWithoutAccess;
   unsigned long SecondsWithoutAccess;
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumCrs;
   unsigned long NumCrss;
   unsigned NumCrssRemoved = 0;
   long CrsCod;

   /***** Get parameter with number of months without access *****/
   Par_GetParToText ("Months",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&MonthsWithoutAccess) != 1)
      Lay_ShowErrorAndExit ("Number of months without clicks is missing.");
   if (MonthsWithoutAccess < Crs_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS ||
       MonthsWithoutAccess > Crs_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS)
      Lay_ShowErrorAndExit ("Wrong number of months without clicks.");
   SecondsWithoutAccess = (unsigned long) MonthsWithoutAccess * Dat_SECONDS_IN_ONE_MONTH;

   /***** Get old courses from database *****/
   sprintf (Query,"SELECT CrsCod FROM crs_last WHERE"
                  " UNIX_TIMESTAMP(LastTime) < UNIX_TIMESTAMP()-%lu"
                  " AND CrsCod NOT IN (SELECT DISTINCT CrsCod FROM crs_usr)",
            SecondsWithoutAccess);
   if ((NumCrss = DB_QuerySELECT (Query,&mysql_res,"can not get old users")))
     {
      sprintf (Gbl.Message,Txt_Eliminating_X_courses_whithout_users_and_with_more_than_Y_months_without_access,
               NumCrss,
               MonthsWithoutAccess,
               Cfg_PLATFORM_SHORT_NAME);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);

      /***** Remove courses *****/
      for (NumCrs = 0;
           NumCrs < NumCrss;
           NumCrs++)
        {
         row = mysql_fetch_row (mysql_res);
         CrsCod = Str_ConvertStrCodToLongCod (row[0]);
         Crs_RemoveCourseCompletely (CrsCod);
         NumCrssRemoved++;
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** Write end message *****/
   sprintf (Gbl.Message,Txt_X_courses_have_been_eliminated,
            NumCrssRemoved);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }
