// swad_indicators.c: indicators of courses

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

#include <linux/stddef.h>	// For NULL
#include <stdio.h>		// For fprintf
#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_action.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_indicator.h"
#include "swad_parameter.h"
#include "swad_theme.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;
extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];

/*****************************************************************************/
/*************************** Internal constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Internal types ******************************/
/*****************************************************************************/

typedef enum
  {
   Ind_INDICATORS_BRIEF,
   Ind_INDICATORS_FULL,
  } Ind_IndicatorsLayout_t;

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

static long Ind_GetParamNumIndicators (void);
static unsigned Ind_GetTableOfCourses (MYSQL_RES **mysql_res);
static bool Ind_GetIfShowBigList (unsigned NumCrss);
static void Ind_PutButtonToConfirmIWantToSeeBigList (unsigned NumCrss);
static void Ind_ShowTableOfCoursesWithIndicators (Ind_IndicatorsLayout_t IndicatorsLayout,unsigned NumCrss,MYSQL_RES *mysql_res);
static unsigned long Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_FileBrowser_t FileBrowser,long CrsCod);

/*****************************************************************************/
/******************* Request showing statistics of courses *******************/
/*****************************************************************************/

void Ind_ReqIndicatorsCourses (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Scope;
   extern const char *Txt_Types_of_degree;
   extern const char *Txt_only_if_the_scope_is_X;
   extern const char *Txt_Department;
   extern const char *Txt_No_of_indicators;
   extern const char *Txt_Any_number;
   extern const char *Txt_Update_indicators;
   extern const char *Txt_Indicators_of_courses;
   extern const char *Txt_Show_more_details;
   MYSQL_RES *mysql_res;
   unsigned Ind;
   unsigned NumCrss;

   /***** Form to update indicators *****/
   /* Start form */
   Act_FormStart (ActReqStaCrs);
   fprintf (Gbl.F.Out,"<div align=\"center\">"
	              "<table cellspacing=\"0\" cellpadding=\"2\">");

   /* Compute stats for anywhere, centre, degree or course? */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"right\" valign=\"middle\" class=\"%s\">%s:</td>"
                      "<td align=\"left\" valign=\"middle\">",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Scope);
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_PLATFORM    |
	               1 << Sco_SCOPE_COUNTRY     |
		       1 << Sco_SCOPE_INSTITUTION |
		       1 << Sco_SCOPE_CENTRE      |
		       1 << Sco_SCOPE_DEGREE      |
		       1 << Sco_SCOPE_COURSE;
   Gbl.Scope.Default = Sco_SCOPE_COURSE;
   Sco_GetScope ();
   Sco_PutSelectorScope (false);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /* Compute stats for a type of degree */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"right\" valign=\"top\" class=\"%s\">%s:</td>"
                      "<td align=\"left\" valign=\"top\" class=\"DAT\">",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Types_of_degree);
   Deg_WriteSelectorDegTypes ();
   fprintf (Gbl.F.Out," (");
   fprintf (Gbl.F.Out,Txt_only_if_the_scope_is_X,
            Cfg_PLATFORM_SHORT_NAME);
   fprintf (Gbl.F.Out,")</td>"
	              "</tr>");

   /* Compute stats for courses with teachers belonging to any department or to a particular departament? */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"right\" valign=\"top\" class=\"%s\">%s:</td>"
                      "<td align=\"left\" valign=\"top\">",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Department);
   Dpt_WriteSelectorDepartment (-1L);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /* Show only courses with a numer of indicators */
   Gbl.Stat.NumIndicators = Ind_GetParamNumIndicators ();
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"right\" valign=\"top\" class=\"%s\">%s:</td>"
                      "<td align=\"left\" valign=\"top\">"
                      "<select name=\"Indicators\">",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_No_of_indicators);
   fprintf (Gbl.F.Out,"<option value=\"-1\"");
   if (Gbl.Stat.NumIndicators < 0)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",Txt_Any_number);
   for (Ind = 0;
	Ind <= Ind_NUM_INDICATORS;
	Ind++)
     {
      fprintf (Gbl.F.Out,"<option");
      if ((long) Ind == Gbl.Stat.NumIndicators)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%u</option>",Ind);
     }
   fprintf (Gbl.F.Out,"</select>");

   /* Form end */
   fprintf (Gbl.F.Out,"</td>"
	              "</table>"
	              "</div>");
   Lay_PutSendButton (Txt_Update_indicators);
   fprintf (Gbl.F.Out,"</form>");

   /***** Get courses from database *****/
   NumCrss = Ind_GetTableOfCourses (&mysql_res);
   if (Ind_GetIfShowBigList (NumCrss))
     {
      /***** Show the stats of courses *****/
      /* Start table */
      Lay_StartRoundFrameTable10 (NULL,0,Txt_Indicators_of_courses);
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td align=\"center\">");

      /* Show table */
      Ind_ShowTableOfCoursesWithIndicators (Ind_INDICATORS_BRIEF,NumCrss,mysql_res);

      /* Button to show more details */
      Act_FormStart (ActSeeAllStaCrs);
      Sco_PutParamScope (Gbl.Scope.Current);
      Par_PutHiddenParamLong ("OthDegTypCod",Gbl.Stat.DegTypCod);
      Par_PutHiddenParamLong ("DptCod",Gbl.Stat.DptCod);
      Par_PutHiddenParamLong ("Indicators",Gbl.Stat.NumIndicators);
      Lay_PutSendButton (Txt_Show_more_details);
      fprintf (Gbl.F.Out,"</form>");

      /* End table */
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");
      Lay_EndRoundFrameTable10 ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*********************** Show statistics of courses **************************/
/*****************************************************************************/

void Ind_ShowIndicatorsCourses (void)
  {
   MYSQL_RES *mysql_res;
   unsigned NumCrss;

   /***** Get users range for statistics of courses *****/
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_PLATFORM    |
	               1 << Sco_SCOPE_COUNTRY     |
		       1 << Sco_SCOPE_INSTITUTION |
		       1 << Sco_SCOPE_CENTRE      |
		       1 << Sco_SCOPE_DEGREE      |
		       1 << Sco_SCOPE_COURSE;
   Gbl.Scope.Default = Sco_SCOPE_COURSE;
   Sco_GetScope ();

   /***** Get degree type code *****/
   Gbl.Stat.DegTypCod = (Gbl.Scope.Current == Sco_SCOPE_PLATFORM) ? Deg_GetParamOtherDegTypCod () :
                                                                    -1L;

   /***** Get department code *****/
   Gbl.Stat.DptCod = Dpt_GetParamDptCod ();

   /***** Get number of indicators *****/
   Gbl.Stat.NumIndicators = Ind_GetParamNumIndicators ();

   /***** Get courses from database *****/
   NumCrss = Ind_GetTableOfCourses (&mysql_res);

   /***** Show the stats of courses *****/
   Ind_ShowTableOfCoursesWithIndicators (Ind_INDICATORS_FULL,NumCrss,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************** Get parameter with the number of indicators *****************/
/*****************************************************************************/

static long Ind_GetParamNumIndicators (void)
  {
   char LongStr[1+10+1];

   /***** Get parameter with code of department *****/
   Par_GetParToText ("Indicators",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/******************* Build query to get table of courses *********************/
/*****************************************************************************/
// Return the number of courses found

static unsigned Ind_GetTableOfCourses (MYSQL_RES **mysql_res)
  {
   char Query[1024];

   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_PLATFORM:
         if (Gbl.Stat.DptCod > 0)
           {
            if (Gbl.Stat.DegTypCod > 0)
               sprintf (Query,"SELECT DISTINCTROW degrees.FullName,courses.FullName,courses.CrsCod,courses.InsCrsCod"
                              " FROM degrees,courses,crs_usr,usr_data"
                              " WHERE degrees.DegTypCod='%ld'"
                              " AND degrees.DegCod=courses.DegCod"
                              " AND courses.CrsCod=crs_usr.CrsCod"
                              " AND crs_usr.Role='%u'"
                              " AND crs_usr.UsrCod=usr_data.UsrCod"
                              " AND usr_data.DptCod='%ld'"
                              " ORDER BY degrees.FullName,courses.FullName",
                        Gbl.Stat.DegTypCod,
                        (unsigned) Rol_ROLE_TEACHER,
                        Gbl.Stat.DptCod);
            else
               sprintf (Query,"SELECT DISTINCTROW degrees.FullName,courses.FullName,courses.CrsCod,courses.InsCrsCod"
                              " FROM degrees,courses,crs_usr,usr_data"
                              " WHERE degrees.DegCod=courses.DegCod"
                              " AND courses.CrsCod=crs_usr.CrsCod"
                              " AND crs_usr.Role='%u'"
                              " AND crs_usr.UsrCod=usr_data.UsrCod"
                              " AND usr_data.DptCod='%ld'"
                              " ORDER BY degrees.FullName,courses.FullName",
                        (unsigned) Rol_ROLE_TEACHER,
                        Gbl.Stat.DptCod);
           }
         else
           {
            if (Gbl.Stat.DegTypCod > 0)
               sprintf (Query,"SELECT degrees.FullName,courses.FullName,courses.CrsCod,courses.InsCrsCod"
                              " FROM degrees,courses"
                              " WHERE degrees.DegTypCod='%ld'"
                              " AND degrees.DegCod=courses.DegCod"
                              " ORDER BY degrees.FullName,courses.FullName",
                        Gbl.Stat.DegTypCod);
            else
               sprintf (Query,"SELECT degrees.FullName,courses.FullName,courses.CrsCod,courses.InsCrsCod"
                              " FROM degrees,courses"
                              " WHERE degrees.DegCod=courses.DegCod"
                              " ORDER BY degrees.FullName,courses.FullName");
           }
         break;
      case Sco_SCOPE_INSTITUTION:
         if (Gbl.Stat.DptCod > 0)
            sprintf (Query,"SELECT DISTINCTROW degrees.FullName,courses.FullName,courses.CrsCod,courses.InsCrsCod"
                           " FROM centres,degrees,courses,crs_usr,usr_data"
                           " WHERE centres.InsCod='%ld'"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.Role='%u'"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.DptCod='%ld'"
                           " ORDER BY degrees.FullName,courses.FullName",
                     Gbl.CurrentIns.Ins.InsCod,
                     (unsigned) Rol_ROLE_TEACHER,
                     Gbl.Stat.DptCod);
         else
            sprintf (Query,"SELECT degrees.FullName,courses.FullName,courses.CrsCod,courses.InsCrsCod"
                           " FROM centres,degrees,courses"
                           " WHERE centres.InsCod='%ld'"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " ORDER BY degrees.FullName,courses.FullName",
                     Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CENTRE:
         if (Gbl.Stat.DptCod > 0)
            sprintf (Query,"SELECT DISTINCTROW degrees.FullName,courses.FullName,courses.CrsCod,courses.InsCrsCod"
                           " FROM degrees,courses,crs_usr,usr_data"
                           " WHERE degrees.CtrCod='%ld'"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.Role='%u'"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.DptCod='%ld'"
                           " ORDER BY degrees.FullName,courses.FullName",
                     Gbl.CurrentCtr.Ctr.CtrCod,
                     (unsigned) Rol_ROLE_TEACHER,
                     Gbl.Stat.DptCod);
         else
            sprintf (Query,"SELECT degrees.FullName,courses.FullName,courses.CrsCod,courses.InsCrsCod"
                           " FROM degrees,courses"
                           " WHERE degrees.CtrCod='%ld'"
                           " AND degrees.DegCod=courses.DegCod"
                           " ORDER BY degrees.FullName,courses.FullName",
                     Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEGREE:
         if (Gbl.Stat.DptCod > 0)
            sprintf (Query,"SELECT DISTINCTROW degrees.FullName,courses.FullName,courses.CrsCod,courses.InsCrsCod"
                           " FROM degrees,courses,crs_usr,usr_data"
                           " WHERE degrees.DegCod='%ld'"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.Role='%u'"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.DptCod='%ld'"
                           " ORDER BY degrees.FullName,courses.FullName",
                     Gbl.CurrentDeg.Deg.DegCod,
                     (unsigned) Rol_ROLE_TEACHER,
                     Gbl.Stat.DptCod);
         else
            sprintf (Query,"SELECT degrees.FullName,courses.FullName,courses.CrsCod,courses.InsCrsCod"
                           " FROM degrees,courses"
                           " WHERE degrees.DegCod='%ld'"
                           " AND degrees.DegCod=courses.DegCod"
                           " ORDER BY degrees.FullName,courses.FullName",
                     Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_COURSE:
         if (Gbl.Stat.DptCod > 0)
            sprintf (Query,"SELECT DISTINCTROW degrees.FullName,courses.FullName,courses.CrsCod,courses.InsCrsCod"
                           " FROM degrees,courses,crs_usr,usr_data"
                           " WHERE courses.CrsCod='%ld'"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.CrsCod='%ld'"
                           " AND crs_usr.Role='%u'"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.DptCod='%ld'"
                           " ORDER BY degrees.FullName,courses.FullName",
                     Gbl.CurrentCrs.Crs.CrsCod,
                     Gbl.CurrentCrs.Crs.CrsCod,
                     (unsigned) Rol_ROLE_TEACHER,
                     Gbl.Stat.DptCod);
         else
            sprintf (Query,"SELECT degrees.FullName,courses.FullName,courses.CrsCod,courses.InsCrsCod"
                           " FROM degrees,courses"
                           " WHERE courses.CrsCod='%ld'"
                           " AND degrees.DegCod=courses.DegCod"
                           " ORDER BY degrees.FullName,courses.FullName",
                     Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }

   return (unsigned) DB_QuerySELECT (Query,mysql_res,"can not get courses");
  }

/*****************************************************************************/
/******* Show form to confirm that I want to see a big list of courses *******/
/*****************************************************************************/

static bool Ind_GetIfShowBigList (unsigned NumCrss)
  {
   bool ShowBigList;
   char YN[1+1];

   /***** If list of courses is too big... *****/
   if (NumCrss <= Cfg_MIN_NUM_COURSES_TO_CONFIRM_SHOW_BIG_LIST)
      return true;	// List is not too big ==> show it

   /***** Get parameter with user's confirmation to see a big list of courses *****/
   Par_GetParToText ("ShowBigList",YN,1);
   if (!(ShowBigList = (Str_ConvertToUpperLetter (YN[0]) == 'Y')))
      Ind_PutButtonToConfirmIWantToSeeBigList (NumCrss);

   return ShowBigList;
  }

/*****************************************************************************/
/****** Show form to confirm that I want to see a big list of courses ********/
/*****************************************************************************/

static void Ind_PutButtonToConfirmIWantToSeeBigList (unsigned NumCrss)
  {
   extern const char *Txt_The_list_of_X_courses_is_too_large_to_be_displayed;
   extern const char *Txt_Show_anyway;

   fprintf (Gbl.F.Out,"<div align=\"center\">");

   /***** Show warning *****/
   sprintf (Gbl.Message,Txt_The_list_of_X_courses_is_too_large_to_be_displayed,
            NumCrss);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);

   /***** Put form to confirm that I want to see the big list *****/
   Act_FormStart (Gbl.CurrentAct);
   Sco_PutParamScope (Gbl.Scope.Current);
   Par_PutHiddenParamLong ("OthDegTypCod",Gbl.Stat.DegTypCod);
   Par_PutHiddenParamLong ("DptCod",Gbl.Stat.DptCod);
   Par_PutHiddenParamLong ("Indicators",Gbl.Stat.NumIndicators);
   Par_PutHiddenParamChar ("ShowBigList",'Y');

   /***** Send button *****/
   Lay_PutSendButton (Txt_Show_anyway);
   fprintf (Gbl.F.Out,"</form>"
	              "</div>");
  }

/*****************************************************************************/
/****************** Get and show total number of courses *********************/
/*****************************************************************************/

static void Ind_ShowTableOfCoursesWithIndicators (Ind_IndicatorsLayout_t IndicatorsLayout,unsigned NumCrss,MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Degree;
   extern const char *Txt_Course;
   extern const char *Txt_Institutional_BR_code;
   extern const char *Txt_Web_page_of_the_course;
   extern const char *Txt_Teachers_ABBREVIATION;
   extern const char *Txt_Students_ABBREVIATION;
   extern const char *Txt_Indicators;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Syllabus_of_the_course;
   extern const char *Txt_INFO_TITLE[Inf_NUM_INFO_TYPES];
   extern const char *Txt_No_of_files_in_common_zones;
   extern const char *Txt_No_of_files_in_documents_zones;
   extern const char *Txt_Guided_academic_assignments;
   extern const char *Txt_Assignments;
   extern const char *Txt_Files_assignments;
   extern const char *Txt_Files_works;
   extern const char *Txt_Online_tutoring;
   extern const char *Txt_Forum_threads;
   extern const char *Txt_Forum_posts;
   extern const char *Txt_Messages_sent_by_teachers;
   extern const char *Txt_Materials;
   extern const char *Txt_Assessment_criteria;
   extern const char *Txt_YES;
   extern const char *Txt_NO;
   extern const char *Txt_INFO_SRC_SHORT_TEXT[Inf_NUM_INFO_SOURCES];
   extern const char *Txt_No_of_indicators;
   extern const char *Txt_Courses;
   extern const char *Txt_Total;
   MYSQL_ROW row;
   unsigned NumCrs;
   long CrsCod;
   unsigned NumCrssWithIndicatorYes[1+Ind_NUM_INDICATORS];
   unsigned Ind;
   unsigned NumTchs;
   unsigned NumStds;
   struct Ind_IndicatorsCrs Indicators;

   /***** Table start *****/
   fprintf (Gbl.F.Out,"<table bgcolor=\"#4D88A1\" cellspacing=\"1\" cellpadding=\"1\">");

   /***** Write table heading *****/
   switch (IndicatorsLayout)
     {
      case Ind_INDICATORS_BRIEF:
         fprintf (Gbl.F.Out,"<tr>"
                            "<td bgcolor=\"%s\" align=\"left\" class=\"TIT_TBL\" rowspan=\"3\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"left\" class=\"TIT_TBL\" rowspan=\"3\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"left\" class=\"TIT_TBL\" rowspan=\"3\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"left\" class=\"TIT_TBL\" rowspan=\"3\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\" colspan=\"11\">%s</td>"
                            "</tr>"
                            "<tr>"
                            "<td bgcolor=\"%s\" align=\"center\" valign=\"top\" class=\"TIT_TBL\" rowspan=\"2\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" valign=\"top\" class=\"TIT_TBL\" colspan=\"2\">(A) %s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" valign=\"top\" class=\"TIT_TBL\" colspan=\"2\">(B) %s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" valign=\"top\" class=\"TIT_TBL\" colspan=\"2\">(C) %s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" valign=\"top\" class=\"TIT_TBL\" colspan=\"2\">(D) %s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" valign=\"top\" class=\"TIT_TBL\" colspan=\"2\">(E) %s</td>"
                            "</tr>"
                            "<tr>"
                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"

                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"

                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"

                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"

                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "</tr>",
                  Gbl.ColorRows[0],Txt_Degree,
                  Gbl.ColorRows[0],Txt_Course,
                  Gbl.ColorRows[0],Txt_Institutional_BR_code,
                  Gbl.ColorRows[0],Txt_Web_page_of_the_course,
                  Gbl.ColorRows[0],Txt_Indicators,

                  Gbl.ColorRows[0],Txt_No_INDEX,
                  Gbl.ColorRows[0],Txt_Syllabus_of_the_course,
                  Gbl.ColorRows[0],Txt_Guided_academic_assignments,
                  Gbl.ColorRows[0],Txt_Online_tutoring,
                  Gbl.ColorRows[0],Txt_Materials,
                  Gbl.ColorRows[0],Txt_Assessment_criteria,

                  Gbl.ColorRows[0],Txt_YES,
                  Gbl.ColorRows[0],Txt_NO,

                  Gbl.ColorRows[0],Txt_YES,
                  Gbl.ColorRows[0],Txt_NO,

                  Gbl.ColorRows[0],Txt_YES,
                  Gbl.ColorRows[0],Txt_NO,

                  Gbl.ColorRows[0],Txt_YES,
                  Gbl.ColorRows[0],Txt_NO,

                  Gbl.ColorRows[0],Txt_YES,
                  Gbl.ColorRows[0],Txt_NO);
         break;
      case Ind_INDICATORS_FULL:
         fprintf (Gbl.F.Out,"<tr>"
                            "<td bgcolor=\"%s\" align=\"left\" class=\"TIT_TBL\" rowspan=\"3\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"left\" class=\"TIT_TBL\" rowspan=\"3\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"left\" class=\"TIT_TBL\" rowspan=\"3\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"left\" class=\"TIT_TBL\" rowspan=\"3\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"left\" class=\"TIT_TBL\" rowspan=\"3\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"left\" class=\"TIT_TBL\" rowspan=\"3\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\" colspan=\"24\">%s</td>"
                            "</tr>"
                            "<tr>"
                            "<td bgcolor=\"%s\" align=\"center\" valign=\"top\" class=\"TIT_TBL\" rowspan=\"2\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" valign=\"top\" class=\"TIT_TBL\" colspan=\"5\">(A) %s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" valign=\"top\" class=\"TIT_TBL\" colspan=\"5\">(B) %s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" valign=\"top\" class=\"TIT_TBL\" colspan=\"5\">(C) %s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" valign=\"top\" class=\"TIT_TBL\" colspan=\"4\">(D) %s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" valign=\"top\" class=\"TIT_TBL\" colspan=\"4\">(E) %s</td>"
                            "</tr>"
                            "<tr>"
                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"left\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"left\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"left\" class=\"TIT_TBL\">%s</td>"

                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"right\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"right\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"right\" class=\"TIT_TBL\">%s</td>"

                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"right\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"right\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"right\" class=\"TIT_TBL\">%s</td>"

                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"right\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"right\" class=\"TIT_TBL\">%s</td>"

                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"center\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"left\" class=\"TIT_TBL\">%s</td>"
                            "<td bgcolor=\"%s\" align=\"left\" class=\"TIT_TBL\">%s</td>"
                            "</tr>",
                  Gbl.ColorRows[0],Txt_Degree,
                  Gbl.ColorRows[0],Txt_Course,
                  Gbl.ColorRows[0],Txt_Institutional_BR_code,
                  Gbl.ColorRows[0],Txt_Web_page_of_the_course,
                  Gbl.ColorRows[0],Txt_Teachers_ABBREVIATION,
                  Gbl.ColorRows[0],Txt_Students_ABBREVIATION,
                  Gbl.ColorRows[0],Txt_Indicators,

                  Gbl.ColorRows[0],Txt_No_INDEX,
                  Gbl.ColorRows[0],Txt_Syllabus_of_the_course,
                  Gbl.ColorRows[0],Txt_Guided_academic_assignments,
                  Gbl.ColorRows[0],Txt_Online_tutoring,
                  Gbl.ColorRows[0],Txt_Materials,
                  Gbl.ColorRows[0],Txt_Assessment_criteria,

                  Gbl.ColorRows[0],Txt_YES,
                  Gbl.ColorRows[0],Txt_NO,
                  Gbl.ColorRows[0],Txt_INFO_TITLE[Inf_LECTURES],
                  Gbl.ColorRows[0],Txt_INFO_TITLE[Inf_PRACTICALS],
                  Gbl.ColorRows[0],Txt_INFO_TITLE[Inf_TEACHING_GUIDE],

                  Gbl.ColorRows[0],Txt_YES,
                  Gbl.ColorRows[0],Txt_NO,
                  Gbl.ColorRows[0],Txt_Assignments,
                  Gbl.ColorRows[0],Txt_Files_assignments,
                  Gbl.ColorRows[0],Txt_Files_works,

                  Gbl.ColorRows[0],Txt_YES,
                  Gbl.ColorRows[0],Txt_NO,
                  Gbl.ColorRows[0],Txt_Forum_threads,
                  Gbl.ColorRows[0],Txt_Forum_posts,
                  Gbl.ColorRows[0],Txt_Messages_sent_by_teachers,

                  Gbl.ColorRows[0],Txt_YES,
                  Gbl.ColorRows[0],Txt_NO,
                  Gbl.ColorRows[0],Txt_No_of_files_in_documents_zones,
                  Gbl.ColorRows[0],Txt_No_of_files_in_common_zones,

                  Gbl.ColorRows[0],Txt_YES,
                  Gbl.ColorRows[0],Txt_NO,
                  Gbl.ColorRows[0],Txt_INFO_TITLE[Inf_ASSESSMENT],
                  Gbl.ColorRows[0],Txt_INFO_TITLE[Inf_TEACHING_GUIDE]);
      break;
     }

   /***** Reset counters of courses with each number of indicators *****/
   for (Ind = 0;
	Ind <= Ind_NUM_INDICATORS;
	Ind++)
      NumCrssWithIndicatorYes[Ind] = 0;

   /***** List courses *****/
   for (Gbl.RowEvenOdd = 1, NumCrs = 0;
	NumCrs < NumCrss;
	NumCrs++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
     {
      /* Get next course */
      row = mysql_fetch_row (mysql_res);

      /* Get course code (row[2]) */
      if ((CrsCod = Str_ConvertStrCodToLongCod (row[2])) < 0)
         Lay_ShowErrorAndExit ("Wrong code of course.");

      /* Get indicators of this course */
      Ind_GetIndicatorsCrs (CrsCod,&Indicators);
      NumCrssWithIndicatorYes[Indicators.CountIndicators]++;

      if (Gbl.Stat.NumIndicators < 0 ||		// -1 means any number of indicators
          Gbl.Stat.NumIndicators == (long) Indicators.CountIndicators)
        {
         /* Write a row for this course */
         switch (IndicatorsLayout)
           {
            case Ind_INDICATORS_BRIEF:
               fprintf (Gbl.F.Out,"<tr>"
                                  "<td bgcolor=\"%s\" align=\"left\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"left\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"left\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"left\" class=\"DAT_SMALL\"><a href=\"%s/?CrsCod=%ld&amp;ActCod=%ld\" target=\"_blank\">%s/?CrsCod=%ld&amp;ActCod=%ld</a></td>"

                                  "<td bgcolor=\"%s\" align=\"right\" class=\"%s\">%u</td>"

                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"

                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"

                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"

                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"

                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "</tr>",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        row[0],
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        row[1],
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        row[3],
                        Gbl.ColorRows[Gbl.RowEvenOdd],Cfg_HTTPS_URL_SWAD_CGI,CrsCod,Act_Actions[ActReqStaCrs].ActCod,
                                                      Cfg_HTTPS_URL_SWAD_CGI,CrsCod,Act_Actions[ActReqStaCrs].ActCod,

                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Indicators.CountIndicators,

                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_GREEN",
                        Indicators.ThereIsSyllabus ? Txt_YES :
                                                     "",
                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_RED",
                        Indicators.ThereIsSyllabus ? "" :
                                                     Txt_NO,

                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_GREEN",
                        Indicators.ThereAreAssignments ? Txt_YES :
                                                         "",
                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_RED",
                        Indicators.ThereAreAssignments ? "" :
                                                         Txt_NO,

                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_GREEN",
                        Indicators.ThereIsOnlineTutoring ? Txt_YES :
                                                           "",
                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_RED"  ,
                        Indicators.ThereIsOnlineTutoring ? "" :
                                                           Txt_NO,

                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_GREEN",
                        Indicators.ThereAreMaterials ? Txt_YES :
                                                       "",
                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_RED"  ,
                        Indicators.ThereAreMaterials ? "" :
                                                       Txt_NO,

                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_GREEN",
                        Indicators.ThereIsAssessment ? Txt_YES :
                                                       "",
                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_RED"  ,
                        Indicators.ThereIsAssessment ? "" :
                                                       Txt_NO);
               break;
            case Ind_INDICATORS_FULL:
               /* Get number of users */
               NumStds = Usr_GetNumUsrsInCrs (Rol_ROLE_STUDENT,CrsCod);
               NumTchs = Usr_GetNumUsrsInCrs (Rol_ROLE_TEACHER,CrsCod);

               fprintf (Gbl.F.Out,"<tr>"
                                  "<td bgcolor=\"%s\" align=\"left\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"left\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"left\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"left\" class=\"DAT_SMALL\"><a href=\"%s/?CrsCod=%ld&amp;ActCod=%ld\" target=\"_blank\">%s/?CrsCod=%ld&amp;ActCod=%ld</a></td>"

                                  "<td bgcolor=\"%s\" align=\"right\" class=\"%s\">%u</td>"
                                  "<td bgcolor=\"%s\" align=\"right\" class=\"%s\">%u</td>"

                                  "<td bgcolor=\"%s\" align=\"right\" class=\"%s\">%u</td>"

                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"left\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"left\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"left\" class=\"%s\">%s</td>"

                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"right\" class=\"%s\">%u</td>"
                                  "<td bgcolor=\"%s\" align=\"right\" class=\"%s\">%lu</td>"
                                  "<td bgcolor=\"%s\" align=\"right\" class=\"%s\">%lu</td>"

                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"right\" class=\"%s\">%u</td>"
                                  "<td bgcolor=\"%s\" align=\"right\" class=\"%s\">%u</td>"
                                  "<td bgcolor=\"%s\" align=\"right\" class=\"%s\">%u</td>"

                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"right\" class=\"%s\">%lu</td>"
                                  "<td bgcolor=\"%s\" align=\"right\" class=\"%s\">%lu</td>"

                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"center\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"left\" class=\"%s\">%s</td>"
                                  "<td bgcolor=\"%s\" align=\"left\" class=\"%s\">%s</td>"
                                  "</tr>",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        row[0],
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        row[1],
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        row[3],
                        Gbl.ColorRows[Gbl.RowEvenOdd],Cfg_HTTPS_URL_SWAD_CGI,CrsCod,Act_Actions[ActReqStaCrs].ActCod,
                                                      Cfg_HTTPS_URL_SWAD_CGI,CrsCod,Act_Actions[ActReqStaCrs].ActCod,

                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        NumTchs != 0 ? "DAT_SMALL_GREEN" :
                                       "DAT_SMALL_RED",
                        NumTchs,
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        NumStds != 0 ? "DAT_SMALL_GREEN" :
                                       "DAT_SMALL_RED",
                        NumStds,

                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Indicators.CountIndicators,

                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_GREEN",
                        Indicators.ThereIsSyllabus ? Txt_YES :
                                                     "",
                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_RED",
                        Indicators.ThereIsSyllabus ? "" :
                                                     Txt_NO,
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        (Indicators.SyllabusLecSrc != Inf_INFO_SRC_NONE) ? "DAT_SMALL_GREEN" :
                                                                           "DAT_SMALL_RED",
                        Txt_INFO_SRC_SHORT_TEXT[Indicators.SyllabusLecSrc],
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        (Indicators.SyllabusPraSrc != Inf_INFO_SRC_NONE) ? "DAT_SMALL_GREEN" :
                                                                           "DAT_SMALL_RED",
                        Txt_INFO_SRC_SHORT_TEXT[Indicators.SyllabusPraSrc],
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        (Indicators.TeachingGuideSrc != Inf_INFO_SRC_NONE) ? "DAT_SMALL_GREEN" :
                                                                             "DAT_SMALL_RED",
                        Txt_INFO_SRC_SHORT_TEXT[Indicators.TeachingGuideSrc],

                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_GREEN",
                        Indicators.ThereAreAssignments ? Txt_YES :
                                                         "",
                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_RED",
                        Indicators.ThereAreAssignments ? "" :
                                                         Txt_NO,
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        (Indicators.NumAssignments != 0) ? "DAT_SMALL_GREEN" :
                                                           "DAT_SMALL_RED",
                        Indicators.NumAssignments,
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        (Indicators.NumFilesAssignments != 0) ? "DAT_SMALL_GREEN" :
                                                                "DAT_SMALL_RED",
                        Indicators.NumFilesAssignments,
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        (Indicators.NumFilesWorks != 0) ? "DAT_SMALL_GREEN" :
                                                          "DAT_SMALL_RED",
                        Indicators.NumFilesWorks,

                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_GREEN",
                        Indicators.ThereIsOnlineTutoring ? Txt_YES :
                                                           "",
                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_RED",
                        Indicators.ThereIsOnlineTutoring ? "" :
                                                           Txt_NO,
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        (Indicators.NumThreads != 0) ? "DAT_SMALL_GREEN" :
                                                       "DAT_SMALL_RED",
                        Indicators.NumThreads,
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        (Indicators.NumPosts != 0) ? "DAT_SMALL_GREEN" :
                                                     "DAT_SMALL_RED",
                        Indicators.NumPosts,
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        (Indicators.NumMsgsSentByTchs != 0) ? "DAT_SMALL_GREEN" :
                                                              "DAT_SMALL_RED",
                        Indicators.NumMsgsSentByTchs,

                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_GREEN",
                        Indicators.ThereAreMaterials ? Txt_YES :
                                                       "",
                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_RED",
                        Indicators.ThereAreMaterials ? "" :
                                                       Txt_NO,
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        (Indicators.NumFilesInDownloadZones != 0) ? "DAT_SMALL_GREEN" :
                                                                    "DAT_SMALL_RED",
                        Indicators.NumFilesInDownloadZones,
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        (Indicators.NumFilesInCommonZones != 0) ? "DAT_SMALL_GREEN" :
                                                                  "DAT_SMALL_RED",
                        Indicators.NumFilesInCommonZones,

                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_GREEN",
                        Indicators.ThereIsAssessment ? Txt_YES :
                                                       "",
                        Gbl.ColorRows[Gbl.RowEvenOdd],"DAT_SMALL_RED",
                        Indicators.ThereIsAssessment ? "" :
                                                       Txt_NO,
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        (Indicators.AssessmentSrc != Inf_INFO_SRC_NONE) ? "DAT_SMALL_GREEN" :
                                                                          "DAT_SMALL_RED",
                        Txt_INFO_SRC_SHORT_TEXT[Indicators.AssessmentSrc],
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        (Indicators.TeachingGuideSrc != Inf_INFO_SRC_NONE) ? "DAT_SMALL_GREEN" :
                                                                             "DAT_SMALL_RED",
                        Txt_INFO_SRC_SHORT_TEXT[Indicators.TeachingGuideSrc]);
               break;
              }
        }
     }

   /***** End table *****/
   fprintf (Gbl.F.Out,"</table>");

   /***** Write number of courses with each number of indicators valid *****/
   fprintf (Gbl.F.Out,"<table><tr>"
                      "<th align=\"right\" class=\"TIT_TBL\">%s</th>"
                      "<th colspan=\"2\" align=\"right\" class=\"TIT_TBL\">%s</th>"
                      "</tr>",
            Txt_No_of_indicators,
            Txt_Courses);
   for (Ind = 0;
	Ind <= Ind_NUM_INDICATORS;
	Ind++)
      fprintf (Gbl.F.Out,"<tr>"
                         "<td align=\"right\" class=\"DAT\">%u</td>"
                         "<td align=\"right\" class=\"DAT\">%u</td>"
                         "<td align=\"right\" class=\"DAT\">(%.1f%%)</td>"
                         "</tr>",
               Ind,NumCrssWithIndicatorYes[Ind],
               NumCrss ? (float) NumCrssWithIndicatorYes[Ind] * 100.0 / (float) NumCrss :
        	         0.0);
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"right\" class=\"DAT_N\" style=\"border-style:solid none none none;border-width:1px;\">%s</td>"
                      "<td align=\"right\" class=\"DAT_N\" style=\"border-style:solid none none none;border-width:1px;\">%u</td>"
                      "<td align=\"right\" class=\"DAT_N\" style=\"border-style:solid none none none;border-width:1px;\">(%.1f%%)</td>"
                      "</tr>",
            Txt_Total,NumCrss,100.0);
   fprintf (Gbl.F.Out,"</table>");
  }

/*****************************************************************************/
/********************** Calculate indicators of a course *********************/
/*****************************************************************************/

void Ind_GetIndicatorsCrs (long CrsCod,struct Ind_IndicatorsCrs *Indicators)
  {
   bool MustBeRead;	// Not used
   Indicators->CountIndicators = 0;

   /* Get whether download zones are empty or not */
   Indicators->NumFilesInDownloadZonesCrs = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS,CrsCod);
   Indicators->NumFilesInDownloadZonesGrp = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP,CrsCod);
   Indicators->NumFilesInDownloadZones    = Indicators->NumFilesInDownloadZonesCrs + Indicators->NumFilesInDownloadZonesGrp;
   Indicators->NumFilesInCommonZonesCrs   = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_FILE_BRW_COMMON_CRS,CrsCod);
   Indicators->NumFilesInCommonZonesGrp   = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_FILE_BRW_COMMON_GRP,CrsCod);
   Indicators->NumFilesInCommonZones      = Indicators->NumFilesInCommonZonesCrs + Indicators->NumFilesInCommonZonesGrp;

   /* Indicator #1: information about syllabus */
   Inf_GetInfoSrcFromDB (CrsCod,Inf_LECTURES      ,&(Indicators->SyllabusLecSrc  ),&MustBeRead);
   Inf_GetInfoSrcFromDB (CrsCod,Inf_PRACTICALS    ,&(Indicators->SyllabusPraSrc  ),&MustBeRead);
   Inf_GetInfoSrcFromDB (CrsCod,Inf_TEACHING_GUIDE,&(Indicators->TeachingGuideSrc),&MustBeRead);
   Indicators->ThereIsSyllabus = (Indicators->SyllabusLecSrc   != Inf_INFO_SRC_NONE) ||
                                 (Indicators->SyllabusPraSrc   != Inf_INFO_SRC_NONE) ||
                                 (Indicators->TeachingGuideSrc != Inf_INFO_SRC_NONE);
   if (Indicators->ThereIsSyllabus)
      Indicators->CountIndicators++;

   /* Indicator #2: information about assignments */
   Indicators->NumAssignments = Asg_GetNumAssignmentsInCrs (CrsCod);
   Indicators->NumFilesAssignments = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_FILE_BRW_ASSIGNMENTS_USR,CrsCod);
   Indicators->NumFilesWorks       = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_FILE_BRW_WORKS_USR      ,CrsCod);
   Indicators->ThereAreAssignments = (Indicators->NumAssignments      != 0) ||
                                     (Indicators->NumFilesAssignments != 0) ||
                                     (Indicators->NumFilesWorks       != 0);
   if (Indicators->ThereAreAssignments)
      Indicators->CountIndicators++;

   /* Indicator #3: information about online tutoring */
   Indicators->NumThreads = For_GetNumTotalThrsInForumsOfType (For_FORUM_COURSE_USRS,-1L,-1L,-1L,CrsCod);
   Indicators->NumPosts   = For_GetNumTotalPstsInForumsOfType (For_FORUM_COURSE_USRS,-1L,-1L,-1L,CrsCod,&(Indicators->NumUsrsToBeNotifiedByEMail));
   Indicators->NumMsgsSentByTchs = Msg_GetNumMsgsSentByTchsCrs (CrsCod);
   Indicators->ThereIsOnlineTutoring = (Indicators->NumThreads        != 0) ||
	                               (Indicators->NumPosts          != 0) ||
	                               (Indicators->NumMsgsSentByTchs != 0);
   if (Indicators->ThereIsOnlineTutoring)
      Indicators->CountIndicators++;

   /* Indicator #4: information about materials */
   Indicators->ThereAreMaterials = (Indicators->NumFilesInDownloadZones != 0) ||
                                   (Indicators->NumFilesInCommonZones   != 0);
   if (Indicators->ThereAreMaterials)
      Indicators->CountIndicators++;

   /* Indicator #5: information about assessment */
   Inf_GetInfoSrcFromDB (CrsCod,Inf_ASSESSMENT,&(Indicators->AssessmentSrc),&MustBeRead);
   Indicators->ThereIsAssessment = (Indicators->AssessmentSrc    != Inf_INFO_SRC_NONE) ||
                                   (Indicators->TeachingGuideSrc != Inf_INFO_SRC_NONE);
   if (Indicators->ThereIsAssessment)
      Indicators->CountIndicators++;

   /* All the indicators are OK? */
   Indicators->CoursePartiallyOK = Indicators->CountIndicators >= 1 &&
	                           Indicators->CountIndicators < Ind_NUM_INDICATORS;
   Indicators->CourseAllOK       = Indicators->CountIndicators == Ind_NUM_INDICATORS;
  }

/*****************************************************************************/
/******* Get the number of files of a course file zone from database *********/
/*****************************************************************************/

static unsigned long Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_FileBrowser_t FileBrowser,long CrsCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumFiles = 0UL;

   /***** Get number of files in a file browser from database *****/
   sprintf (Query,"SELECT SUM(NumFiles)"
                  " FROM file_browser_size"
                  " WHERE FileBrowser='%u' AND CrsCod='%ld'",
            (unsigned) FileBrowser,CrsCod);
   DB_QuerySELECT (Query,&mysql_res,"can not get the number of files in a file browser");

   /***** Get row *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get number of files (row[0]) *****/
   if (row[0])
      if (sscanf (row[0],"%lu",&NumFiles) != 1)
         Lay_ShowErrorAndExit ("Error when getting the number of files in a file browser.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumFiles;
  }
