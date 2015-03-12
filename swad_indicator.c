// swad_indicators.c: indicators of courses

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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
   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\" style=\"margin:0 auto;\">");

   /* Compute stats for anywhere, centre, degree or course? */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s\""
                      " style=\"text-align:right; vertical-align:middle;\">"
                      "%s:"
                      "</td>"
                      "<td style=\"text-align:left; vertical-align:middle;\">",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Scope);
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS    |
	               1 << Sco_SCOPE_CTY     |
		       1 << Sco_SCOPE_INS |
		       1 << Sco_SCOPE_CTR      |
		       1 << Sco_SCOPE_DEG      |
		       1 << Sco_SCOPE_CRS;
   Gbl.Scope.Default = Sco_SCOPE_CRS;
   Sco_GetScope ();
   Sco_PutSelectorScope (false);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /* Compute stats for a type of degree */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s\""
                      " style=\"text-align:right; vertical-align:top;\">"
                      "%s:"
                      "</td>"
                      "<td class=\"DAT\""
                      " style=\"text-align:left; vertical-align:top;\">",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Types_of_degree);
   Deg_WriteSelectorDegTypes ();
   fprintf (Gbl.F.Out," (");
   fprintf (Gbl.F.Out,Txt_only_if_the_scope_is_X,
            Cfg_PLATFORM_SHORT_NAME);
   fprintf (Gbl.F.Out,")</td>"
	              "</tr>");

   /* Compute stats for courses with teachers belonging to any department or to a particular departament? */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s\""
                      " style=\"text-align:right; vertical-align:top;\">"
                      "%s:"
                      "</td>"
                      "<td style=\"text-align:left; vertical-align:top;\">",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Department);
   Dpt_WriteSelectorDepartment (-1L);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /* Show only courses with a numer of indicators */
   Gbl.Stat.NumIndicators = Ind_GetParamNumIndicators ();
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s\" style=\"text-align:right; vertical-align:top;\">"
                      "%s:"
                      "</td>"
                      "<td style=\"text-align:left; vertical-align:top;\">"
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
	              "</table>");
   Lay_PutSendButton (Txt_Update_indicators);
   Act_FormEnd ();

   /***** Get courses from database *****/
   NumCrss = Ind_GetTableOfCourses (&mysql_res);
   if (Ind_GetIfShowBigList (NumCrss))
     {
      /***** Show the stats of courses *****/
      /* Start table */
      Lay_StartRoundFrameTable10 (NULL,0,Txt_Indicators_of_courses);
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td style=\"text-align:center;\">");

      /* Show table */
      Ind_ShowTableOfCoursesWithIndicators (Ind_INDICATORS_BRIEF,NumCrss,mysql_res);

      /* Button to show more details */
      Act_FormStart (ActSeeAllStaCrs);
      Sco_PutParamScope (Gbl.Scope.Current);
      Par_PutHiddenParamLong ("OthDegTypCod",Gbl.Stat.DegTypCod);
      Par_PutHiddenParamLong ("DptCod",Gbl.Stat.DptCod);
      Par_PutHiddenParamLong ("Indicators",Gbl.Stat.NumIndicators);
      Lay_PutSendButton (Txt_Show_more_details);
      Act_FormEnd ();

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
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS    |
	               1 << Sco_SCOPE_CTY     |
		       1 << Sco_SCOPE_INS |
		       1 << Sco_SCOPE_CTR      |
		       1 << Sco_SCOPE_DEG      |
		       1 << Sco_SCOPE_CRS;
   Gbl.Scope.Default = Sco_SCOPE_CRS;
   Sco_GetScope ();

   /***** Get degree type code *****/
   Gbl.Stat.DegTypCod = (Gbl.Scope.Current == Sco_SCOPE_SYS) ? Deg_GetParamOtherDegTypCod () :
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
      case Sco_SCOPE_SYS:
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
      case Sco_SCOPE_INS:
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
      case Sco_SCOPE_CTR:
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
      case Sco_SCOPE_DEG:
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
      case Sco_SCOPE_CRS:
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

   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");

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
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");
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
   extern const char *Txt_No_of_files_in_SHARE_zones;
   extern const char *Txt_No_of_files_in_DOCUM_zones;
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
   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_1\" style=\"border-spacing:1px;"
	              " margin-left:auto; margin-right:auto;"
	              " background-color:#4D88A1;\">");

   /***** Write table heading *****/
   switch (IndicatorsLayout)
     {
      case Ind_INDICATORS_BRIEF:
         fprintf (Gbl.F.Out,"<tr>"
                            "<td rowspan=\"3\" class=\"TIT_TBL\""
                            " style=\"text-align:left; background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td rowspan=\"3\" class=\"TIT_TBL\""
                            " style=\"text-align:left; background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td rowspan=\"3\" class=\"TIT_TBL\""
                            " style=\"text-align:left; background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td rowspan=\"3\" class=\"TIT_TBL\""
                            " style=\"text-align:left; background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td colspan=\"11\" class=\"TIT_TBL\""
                            " style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "</tr>"
                            "<tr>"
                            "<td rowspan=\"2\" class=\"TIT_TBL\""
                            " style=\"text-align:center; vertical-align:top;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td colspan=\"2\" class=\"TIT_TBL\""
                            " style=\"text-align:center; vertical-align:top;"
                            " background-color:%s;\">"
                            "(A) %s"
                            "</td>"
                            "<td colspan=\"2\" class=\"TIT_TBL\""
                            " style=\"text-align:center; vertical-align:top;"
                            " background-color:%s;\">"
                            "(B) %s"
                            "</td>"
                            "<td colspan=\"2\" class=\"TIT_TBL\""
                            " style=\"text-align:center; vertical-align:top;"
                            " background-color:%s;\">"
                            "(C) %s"
                            "</td>"
                            "<td colspan=\"2\" class=\"TIT_TBL\""
                            " style=\"text-align:center; vertical-align:top;"
                            " background-color:%s;\">"
                            "(D) %s"
                            "</td>"
                            "<td colspan=\"2\" class=\"TIT_TBL\""
                            " style=\"text-align:center; vertical-align:top;"
                            " background-color:%s;\">"
                            "(E) %s"
                            "</td>"
                            "</tr>"
                            "<tr>"
                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"

                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"

                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"

                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"

                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
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
                            "<td rowspan=\"3\" class=\"TIT_TBL\""
                            " style=\"text-align:left; background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td rowspan=\"3\" class=\"TIT_TBL\""
                            " style=\"text-align:left; background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td rowspan=\"3\" class=\"TIT_TBL\""
                            " style=\"text-align:left; background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td rowspan=\"3\" class=\"TIT_TBL\""
                            " style=\"text-align:left; background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td rowspan=\"3\" class=\"TIT_TBL\""
                            " style=\"text-align:left; background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td rowspan=\"3\" class=\"TIT_TBL\""
                            " style=\"text-align:left; background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td colspan=\"24\" class=\"TIT_TBL\""
                            " style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "</tr>"
                            "<tr>"
                            "<td rowspan=\"2\" class=\"TIT_TBL\""
                            " style=\"text-align:center; vertical-align:top;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td colspan=\"5\" class=\"TIT_TBL\""
                            " style=\"text-align:center; vertical-align:top;"
                            " background-color:%s;\">"
                            "(A) %s"
                            "</td>"
                            "<td colspan=\"5\" class=\"TIT_TBL\""
                            " style=\"text-align:center; vertical-align:top;"
                            " background-color:%s;\">"
                            "(B) %s"
                            "</td>"
                            "<td colspan=\"5\" class=\"TIT_TBL\""
                            " style=\"text-align:center; vertical-align:top;"
                            " background-color:%s;\">"
                            "(C) %s"
                            "</td>"
                            "<td colspan=\"4\" class=\"TIT_TBL\""
                            " style=\"text-align:center; vertical-align:top;"
                            " background-color:%s;\">"
                            "(D) %s"
                            "</td>"
                            "<td colspan=\"4\" class=\"TIT_TBL\""
                            " style=\"text-align:center; vertical-align:top;"
                            " background-color:%s;\">"
                            "(E) %s"
                            "</td>"
                            "</tr>"
                            "<tr>"
                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:left;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:left;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:left;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"

                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:right;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:right;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:right;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"

                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:right;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:right;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:right;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"

                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:right;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:right;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"

                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:center;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:left;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
                            "<td class=\"TIT_TBL\" style=\"text-align:left;"
                            " background-color:%s;\">"
                            "%s"
                            "</td>"
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
                  Gbl.ColorRows[0],Txt_No_of_files_in_DOCUM_zones,
                  Gbl.ColorRows[0],Txt_No_of_files_in_SHARE_zones,

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
                                  "<td class=\"%s\" style=\"text-align:left;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:left;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:left;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"DAT_SMALL\""
                                  " style=\"text-align:left;"
                                  " background-color:%s;\">"
                                  "<a href=\"%s/?crs=%ld&amp;act=%ld\" target=\"_blank\">"
                                  "%s/?crs=%ld&amp;act=%ld"
                                  "</a>"
                                  "</td>"

                                  "<td class=\"%s\" style=\"text-align:right;"
                                  " background-color:%s;\">"
                                  "%u"
                                  "</td>"

                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"

                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"

                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"

                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"

                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "</tr>",
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        row[0],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        row[1],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        row[3],
                        Gbl.ColorRows[Gbl.RowEvenOdd],Cfg_HTTPS_URL_SWAD_CGI,CrsCod,Act_Actions[ActReqStaCrs].ActCod,
                                                      Cfg_HTTPS_URL_SWAD_CGI,CrsCod,Act_Actions[ActReqStaCrs].ActCod,

                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.CountIndicators,

                        "DAT_SMALL_GREEN",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereIsSyllabus ? Txt_YES :
                                                     "",
                        "DAT_SMALL_RED",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereIsSyllabus ? "" :
                                                     Txt_NO,

                        "DAT_SMALL_GREEN",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereAreAssignments ? Txt_YES :
                                                         "",
                        "DAT_SMALL_RED",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereAreAssignments ? "" :
                                                         Txt_NO,

                        "DAT_SMALL_GREEN",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereIsOnlineTutoring ? Txt_YES :
                                                           "",
                        "DAT_SMALL_RED",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereIsOnlineTutoring ? "" :
                                                           Txt_NO,

                        "DAT_SMALL_GREEN",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereAreMaterials ? Txt_YES :
                                                       "",
                        "DAT_SMALL_RED",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereAreMaterials ? "" :
                                                       Txt_NO,

                        "DAT_SMALL_GREEN",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereIsAssessment ? Txt_YES :
                                                       "",
                        "DAT_SMALL_RED",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereIsAssessment ? "" :
                                                       Txt_NO);
               break;
            case Ind_INDICATORS_FULL:
               /* Get number of users */
               NumStds = Usr_GetNumUsrsInCrs (Rol_ROLE_STUDENT,CrsCod);
               NumTchs = Usr_GetNumUsrsInCrs (Rol_ROLE_TEACHER,CrsCod);

               fprintf (Gbl.F.Out,"<tr>"
                                  "<td class=\"%s\" style=\"text-align:left;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:left;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:left;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"DAT_SMALL\""
                                  " style=\"text-align:left;"
                                  " background-color:%s;\">"
                                  "<a href=\"%s/?crs=%ld&amp;act=%ld\" target=\"_blank\">"
                                  "%s/?crs=%ld&amp;act=%ld"
                                  "</a>"
                                  "</td>"

                                  "<td class=\"%s\" style=\"text-align:right;"
                                  " background-color:%s;\">"
                                  "%u"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:right;"
                                  " background-color:%s;\">"
                                  "%u"
                                  "</td>"

                                  "<td class=\"%s\" style=\"text-align:right;"
                                  " background-color:%s;\">"
                                  "%u"
                                  "</td>"

                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:left;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:left;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:left;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"

                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:right;"
                                  " background-color:%s;\">"
                                  "%u"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:right;"
                                  " background-color:%s;\">"
                                  "%lu"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:right;"
                                  " background-color:%s;\">"
                                  "%lu"
                                  "</td>"

                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:right;"
                                  " background-color:%s;\">"
                                  "%u"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:right;"
                                  " background-color:%s;\">"
                                  "%u"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:right;"
                                  " background-color:%s;\">"
                                  "%u"
                                  "</td>"

                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:right;"
                                  " background-color:%s;\">"
                                  "%lu"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:right;"
                                  " background-color:%s;\">"
                                  "%lu"
                                  "</td>"

                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:center;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:left;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s\" style=\"text-align:left;"
                                  " background-color:%s;\">"
                                  "%s"
                                  "</td>"
                                  "</tr>",
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        row[0],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        row[1],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        row[3],
                        Gbl.ColorRows[Gbl.RowEvenOdd],Cfg_HTTPS_URL_SWAD_CGI,CrsCod,Act_Actions[ActReqStaCrs].ActCod,
                                                      Cfg_HTTPS_URL_SWAD_CGI,CrsCod,Act_Actions[ActReqStaCrs].ActCod,

                        NumTchs != 0 ? "DAT_SMALL_GREEN" :
                                       "DAT_SMALL_RED",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        NumTchs,
                        NumStds != 0 ? "DAT_SMALL_GREEN" :
                                       "DAT_SMALL_RED",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        NumStds,

                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.CountIndicators,

                        "DAT_SMALL_GREEN",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereIsSyllabus ? Txt_YES :
                                                     "",
                        "DAT_SMALL_RED",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereIsSyllabus ? "" :
                                                     Txt_NO,
                        (Indicators.SyllabusLecSrc != Inf_INFO_SRC_NONE) ? "DAT_SMALL_GREEN" :
                                                                           "DAT_SMALL_RED",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Txt_INFO_SRC_SHORT_TEXT[Indicators.SyllabusLecSrc],
                        (Indicators.SyllabusPraSrc != Inf_INFO_SRC_NONE) ? "DAT_SMALL_GREEN" :
                                                                           "DAT_SMALL_RED",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Txt_INFO_SRC_SHORT_TEXT[Indicators.SyllabusPraSrc],
                        (Indicators.TeachingGuideSrc != Inf_INFO_SRC_NONE) ? "DAT_SMALL_GREEN" :
                                                                             "DAT_SMALL_RED",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Txt_INFO_SRC_SHORT_TEXT[Indicators.TeachingGuideSrc],

                        "DAT_SMALL_GREEN",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereAreAssignments ? Txt_YES :
                                                         "",
                        "DAT_SMALL_RED",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereAreAssignments ? "" :
                                                         Txt_NO,
                        (Indicators.NumAssignments != 0) ? "DAT_SMALL_GREEN" :
                                                           "DAT_SMALL_RED",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.NumAssignments,
                        (Indicators.NumFilesAssignments != 0) ? "DAT_SMALL_GREEN" :
                                                                "DAT_SMALL_RED",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.NumFilesAssignments,
                        (Indicators.NumFilesWorks != 0) ? "DAT_SMALL_GREEN" :
                                                          "DAT_SMALL_RED",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.NumFilesWorks,

                        "DAT_SMALL_GREEN",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereIsOnlineTutoring ? Txt_YES :
                                                           "",
                        "DAT_SMALL_RED",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereIsOnlineTutoring ? "" :
                                                           Txt_NO,
                        (Indicators.NumThreads != 0) ? "DAT_SMALL_GREEN" :
                                                       "DAT_SMALL_RED",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.NumThreads,
                        (Indicators.NumPosts != 0) ? "DAT_SMALL_GREEN" :
                                                     "DAT_SMALL_RED",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.NumPosts,
                        (Indicators.NumMsgsSentByTchs != 0) ? "DAT_SMALL_GREEN" :
                                                              "DAT_SMALL_RED",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.NumMsgsSentByTchs,

                        "DAT_SMALL_GREEN",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereAreMaterials ? Txt_YES :
                                                       "",
                        "DAT_SMALL_RED",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereAreMaterials ? "" :
                                                       Txt_NO,
                        (Indicators.NumFilesInDownloadZones != 0) ? "DAT_SMALL_GREEN" :
                                                                    "DAT_SMALL_RED",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.NumFilesInDownloadZones,
                        (Indicators.NumFilesInCommonZones != 0) ? "DAT_SMALL_GREEN" :
                                                                  "DAT_SMALL_RED",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.NumFilesInCommonZones,

                        "DAT_SMALL_GREEN",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereIsAssessment ? Txt_YES :
                                                       "",
                        "DAT_SMALL_RED",Gbl.ColorRows[Gbl.RowEvenOdd],
                        Indicators.ThereIsAssessment ? "" :
                                                       Txt_NO,
                        (Indicators.AssessmentSrc != Inf_INFO_SRC_NONE) ? "DAT_SMALL_GREEN" :
                                                                          "DAT_SMALL_RED",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Txt_INFO_SRC_SHORT_TEXT[Indicators.AssessmentSrc],
                        (Indicators.TeachingGuideSrc != Inf_INFO_SRC_NONE) ? "DAT_SMALL_GREEN" :
                                                                             "DAT_SMALL_RED",
                        Gbl.ColorRows[Gbl.RowEvenOdd],
                        Txt_INFO_SRC_SHORT_TEXT[Indicators.TeachingGuideSrc]);
               break;
              }
        }
     }

   /***** End table *****/
   fprintf (Gbl.F.Out,"</table>");

   /***** Write number of courses with each number of indicators valid *****/
   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\""
	              " style=\"margin-left:auto; margin-right:auto;\">"
                      "<tr>"
                      "<th class=\"TIT_TBL\" style=\"text-align:right;\">"
                      "%s"
                      "</th>"
                      "<th colspan=\"2\" class=\"TIT_TBL\""
                      " style=\"text-align:right;\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_No_of_indicators,
            Txt_Courses);
   for (Ind = 0;
	Ind <= Ind_NUM_INDICATORS;
	Ind++)
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"DAT\" style=\"text-align:right;\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT\" style=\"text-align:right;\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT\" style=\"text-align:right;\">"
                         "(%.1f%%)"
                         "</td>"
                         "</tr>",
               Ind,NumCrssWithIndicatorYes[Ind],
               NumCrss ? (float) NumCrssWithIndicatorYes[Ind] * 100.0 / (float) NumCrss :
        	         0.0);
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"DAT_N\" style=\"text-align:right;"
                      " border-style:solid none none none; border-width:1px;\">"
                      "%s"
                      "</td>"
                      "<td class=\"DAT_N\" style=\"text-align:right;"
                      " border-style:solid none none none; border-width:1px;\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT_N\" style=\"text-align:right;"
                      " border-style:solid none none none; border-width:1px;\">"
                      "(%.1f%%)"
                      "</td>"
                      "</tr>"
                      "</table>",
            Txt_Total,NumCrss,100.0);
  }

/*****************************************************************************/
/********************** Calculate indicators of a course *********************/
/*****************************************************************************/

void Ind_GetIndicatorsCrs (long CrsCod,struct Ind_IndicatorsCrs *Indicators)
  {
   bool MustBeRead;	// Not used
   Indicators->CountIndicators = 0;

   /* Get whether download zones are empty or not */
   Indicators->NumFilesInDownloadZonesCrs = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_ADMI_DOCUM_CRS,CrsCod);
   Indicators->NumFilesInDownloadZonesGrp = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_ADMI_DOCUM_GRP,CrsCod);
   Indicators->NumFilesInDownloadZones    = Indicators->NumFilesInDownloadZonesCrs + Indicators->NumFilesInDownloadZonesGrp;
   Indicators->NumFilesInCommonZonesCrs   = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_ADMI_SHARE_CRS,CrsCod);
   Indicators->NumFilesInCommonZonesGrp   = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_ADMI_SHARE_GRP,CrsCod);
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
   Indicators->NumFilesAssignments = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_ADMI_ASSIG_USR,CrsCod);
   Indicators->NumFilesWorks       = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_ADMI_WORKS_USR      ,CrsCod);
   Indicators->ThereAreAssignments = (Indicators->NumAssignments      != 0) ||
                                     (Indicators->NumFilesAssignments != 0) ||
                                     (Indicators->NumFilesWorks       != 0);
   if (Indicators->ThereAreAssignments)
      Indicators->CountIndicators++;

   /* Indicator #3: information about online tutoring */
   Indicators->NumThreads = For_GetNumTotalThrsInForumsOfType (For_FORUM_COURSE_USRS,-1L,-1L,-1L,-1L,CrsCod);
   Indicators->NumPosts   = For_GetNumTotalPstsInForumsOfType (For_FORUM_COURSE_USRS,-1L,-1L,-1L,-1L,CrsCod,&(Indicators->NumUsrsToBeNotifiedByEMail));
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
   extern const Brw_FileBrowser_t Brw_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumFiles = 0UL;

   /***** Get number of files in a file browser from database *****/
   sprintf (Query,"SELECT SUM(NumFiles)"
                  " FROM file_browser_size"
                  " WHERE FileBrowser='%u' AND Cod='%ld'",
            (unsigned) Brw_FileBrowserForDB_files[FileBrowser],CrsCod);
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
