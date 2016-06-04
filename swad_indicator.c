// swad_indicators.c: indicators of courses

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

static void Ind_GetNumCoursesWithIndicators (unsigned NumCrssWithIndicatorYes[1+Ind_NUM_INDICATORS],
                                             unsigned NumCrss,MYSQL_RES *mysql_res);
static void Ind_ShowNumCoursesWithIndicators (unsigned NumCrssWithIndicatorYes[1+Ind_NUM_INDICATORS],
                                              unsigned NumCrss);
static void Ind_ShowTableOfCoursesWithIndicators (Ind_IndicatorsLayout_t IndicatorsLayout,
                                                  unsigned NumCrss,MYSQL_RES *mysql_res);
static unsigned long Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_FileBrowser_t FileBrowser,long CrsCod);

/*****************************************************************************/
/******************* Request showing statistics of courses *******************/
/*****************************************************************************/

void Ind_ReqIndicatorsCourses (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *The_ClassFormBold[The_NUM_THEMES];
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
   unsigned NumCrssWithIndicatorYes[1+Ind_NUM_INDICATORS];
   unsigned NumCrssToList;

   /***** Start frame *****/
   Lay_StartRoundFrame (NULL,Txt_Indicators_of_courses,NULL);

   /***** Form to update indicators *****/
   /* Start form */
   Act_FormStart (ActReqStaCrs);

   fprintf (Gbl.F.Out,"<table class=\"FRAME_TABLE CELLS_PAD_2\">"
                      "<tr>"
                      "<td class=\"RIGHT_MIDDLE\">"
                      "<label class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Scope);
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	               1 << Sco_SCOPE_CTY |
		       1 << Sco_SCOPE_INS |
		       1 << Sco_SCOPE_CTR |
		       1 << Sco_SCOPE_DEG |
		       1 << Sco_SCOPE_CRS;
   Gbl.Scope.Default = Sco_SCOPE_CRS;
   Sco_GetScope ();
   Sco_PutSelectorScope (false);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /* Compute stats for a type of degree */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_MIDDLE\">"
                      "<label class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"DAT LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Types_of_degree);
   DT_WriteSelectorDegreeTypes ();
   fprintf (Gbl.F.Out," (");
   fprintf (Gbl.F.Out,Txt_only_if_the_scope_is_X,
            Cfg_PLATFORM_SHORT_NAME);
   fprintf (Gbl.F.Out,")</td>"
	              "</tr>");

   /* Compute stats for courses with teachers belonging to any department or to a particular departament? */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_MIDDLE\">"
                      "<label class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Department);
   Dpt_WriteSelectorDepartment (-1L);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /* Show only courses with a number of indicators */
   Gbl.Stat.NumIndicators = Ind_GetParamNumIndicators ();
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_MIDDLE\">"
                      "<label class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<select name=\"Indicators\">",
            The_ClassForm[Gbl.Prefs.Theme],Txt_No_of_indicators);
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
   fprintf (Gbl.F.Out,"</select>"
	              "</td>"
	              "</tr>");

   /* Send button */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td colspan=\"2\" class=\"CENTER_MIDDLE\">");
   Act_LinkFormSubmitAnimated (Txt_Update_indicators,The_ClassFormBold[Gbl.Prefs.Theme]);
   Lay_PutCalculateIconWithText (Txt_Update_indicators,Txt_Update_indicators);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>"
	              "</table>");

   /* End form */
   Act_FormEnd ();

   /***** Get courses from database *****/
   // The result will contain courses with any number of indicators
   // If Gbl.Stat.NumIndicators <  0 ==> all courses in result will be listed
   // If Gbl.Stat.NumIndicators >= 0 ==> only those courses in result
   //                                    with Gbl.Stat.NumIndicators set to yes
   //                                    will be listed
   NumCrss = Ind_GetTableOfCourses (&mysql_res);

   /***** Get vector with numbers of courses with 0, 1, 2... indicators set to yes *****/
   Ind_GetNumCoursesWithIndicators (NumCrssWithIndicatorYes,NumCrss,mysql_res);

   /***** Show table with numbers of courses with 0, 1, 2... indicators set to yes *****/
   Ind_ShowNumCoursesWithIndicators (NumCrssWithIndicatorYes,NumCrss);

   if (Gbl.Stat.NumIndicators < 0)	// -1 means any number of indicators
      NumCrssToList = NumCrss;
   else
      NumCrssToList = NumCrssWithIndicatorYes[(unsigned) Gbl.Stat.NumIndicators];

   if (Ind_GetIfShowBigList (NumCrssToList))
     {
      /***** Show the stats of courses *****/
      /* Show table */
      Ind_ShowTableOfCoursesWithIndicators (Ind_INDICATORS_BRIEF,NumCrss,mysql_res);

      /* Button to show more details */
      Act_FormStart (ActSeeAllStaCrs);
      Sco_PutParamScope (Gbl.Scope.Current);
      Par_PutHiddenParamLong ("OthDegTypCod",Gbl.Stat.DegTypCod);
      Par_PutHiddenParamLong ("DptCod",Gbl.Stat.DptCod);
      Par_PutHiddenParamLong ("Indicators",Gbl.Stat.NumIndicators);
      Lay_PutConfirmButton (Txt_Show_more_details);
      Act_FormEnd ();
     }

   /***** End frame *****/
   Lay_EndRoundFrame ();

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
   unsigned NumCrssWithIndicatorYes[1+Ind_NUM_INDICATORS];

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
   Gbl.Stat.DegTypCod = (Gbl.Scope.Current == Sco_SCOPE_SYS) ? DT_GetParamOtherDegTypCod () :
                                                               -1L;

   /***** Get department code *****/
   Gbl.Stat.DptCod = Dpt_GetParamDptCod ();

   /***** Get number of indicators *****/
   Gbl.Stat.NumIndicators = Ind_GetParamNumIndicators ();

   /***** Get courses from database *****/
   NumCrss = Ind_GetTableOfCourses (&mysql_res);

   /***** Get vector with numbers of courses with 0, 1, 2... indicators set to yes *****/
   Ind_GetNumCoursesWithIndicators (NumCrssWithIndicatorYes,NumCrss,mysql_res);

   /***** Show table with numbers of courses with 0, 1, 2... indicators set to yes *****/
   Ind_ShowNumCoursesWithIndicators (NumCrssWithIndicatorYes,NumCrss);

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
                        (unsigned) Rol_TEACHER,
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
                        (unsigned) Rol_TEACHER,
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
      case Sco_SCOPE_CTY:
         if (Gbl.Stat.DptCod > 0)
            sprintf (Query,"SELECT DISTINCTROW degrees.FullName,courses.FullName,courses.CrsCod,courses.InsCrsCod"
                           " FROM institutions,centres,degrees,courses,crs_usr,usr_data"
                           " WHERE institutions.CtyCod='%ld'"
                           " AND institutions.InsCod=centres.InsCod"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.Role='%u'"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.DptCod='%ld'"
                           " ORDER BY degrees.FullName,courses.FullName",
                     Gbl.CurrentCty.Cty.CtyCod,
                     (unsigned) Rol_TEACHER,
                     Gbl.Stat.DptCod);
         else
            sprintf (Query,"SELECT degrees.FullName,courses.FullName,courses.CrsCod,courses.InsCrsCod"
                           " FROM institutions,centres,degrees,courses"
                          " WHERE institutions.CtyCod='%ld'"
                           " AND institutions.InsCod=centres.InsCod"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " ORDER BY degrees.FullName,courses.FullName",
                     Gbl.CurrentCty.Cty.CtyCod);
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
                     (unsigned) Rol_TEACHER,
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
                     (unsigned) Rol_TEACHER,
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
                     (unsigned) Rol_TEACHER,
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
                     (unsigned) Rol_TEACHER,
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

   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");

   /***** Show warning *****/
   sprintf (Gbl.Message,Txt_The_list_of_X_courses_is_too_large_to_be_displayed,
            NumCrss);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);

   /***** Put form to confirm that I want to see the big list *****/
   Act_FormStart (Gbl.Action.Act);
   Sco_PutParamScope (Gbl.Scope.Current);
   Par_PutHiddenParamLong ("OthDegTypCod",Gbl.Stat.DegTypCod);
   Par_PutHiddenParamLong ("DptCod",Gbl.Stat.DptCod);
   Par_PutHiddenParamLong ("Indicators",Gbl.Stat.NumIndicators);
   Par_PutHiddenParamChar ("ShowBigList",'Y');

   /***** Send button *****/
   Lay_PutConfirmButton (Txt_Show_anyway);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/** Get vector with numbers of courses with 0, 1, 2... indicators set to yes */
/*****************************************************************************/

static void Ind_GetNumCoursesWithIndicators (unsigned NumCrssWithIndicatorYes[1+Ind_NUM_INDICATORS],
                                             unsigned NumCrss,MYSQL_RES *mysql_res)
  {
   MYSQL_ROW row;
   unsigned NumCrs;
   long CrsCod;
   unsigned Ind;
   struct Ind_IndicatorsCrs Indicators;

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
     }
  }

/*****************************************************************************/
/** Show table with numbers of courses with 0, 1, 2... indicators set to yes */
/*****************************************************************************/

static void Ind_ShowNumCoursesWithIndicators (unsigned NumCrssWithIndicatorYes[1+Ind_NUM_INDICATORS],
                                              unsigned NumCrss)
  {
   extern const char *Txt_No_of_indicators;
   extern const char *Txt_Courses;
   extern const char *Txt_Total;
   unsigned Ind;
   const char *Class;
   const char *ClassNormal = "DAT RIGHT_MIDDLE";
   const char *ClassHighlight = "DAT_N RIGHT_MIDDLE LIGHT_BLUE";
   const char *ClassTotalNormal = "DAT_N_LINE_TOP RIGHT_MIDDLE";
   const char *ClassTotalHighlight = "DAT_N_LINE_TOP RIGHT_MIDDLE LIGHT_BLUE";

   /***** Write number of courses with each number of indicators valid *****/
   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\""
	              " style=\"padding:10px; margin:0 auto;\">"
                      "<tr>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th colspan=\"2\" class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_No_of_indicators,
            Txt_Courses);
   for (Ind = 0;
	Ind <= Ind_NUM_INDICATORS;
	Ind++)
     {
      Class = (Ind == Gbl.Stat.NumIndicators) ? ClassHighlight :
                                                ClassNormal;
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"%s\">"
                         "%u"
                         "</td>"
                         "<td class=\"%s\">"
                         "%u"
                         "</td>"
                         "<td class=\"%s\">"
                         "(%.1f%%)"
                         "</td>"
                         "</tr>",
               Class,
               Ind,
               Class,
               NumCrssWithIndicatorYes[Ind],
               Class,
               NumCrss ? (float) NumCrssWithIndicatorYes[Ind] * 100.0 / (float) NumCrss :
        	         0.0);
     }

   /***** Write total of courses *****/
   Class = (Gbl.Stat.NumIndicators < 0) ? ClassTotalHighlight :
					  ClassTotalNormal;
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s\">"
                      "%s"
                      "</td>"
                      "<td class=\"%s\">"
                      "%u"
                      "</td>"
                      "<td class=\"%s\">"
                      "(%.1f%%)"
                      "</td>"
                      "</tr>"
                      "</table>",
            Class,
            Txt_Total,
            Class,
            NumCrss,
            Class,
            100.0);
  }

/*****************************************************************************/
/****************** Get and show total number of courses *********************/
/*****************************************************************************/

static void Ind_ShowTableOfCoursesWithIndicators (Ind_IndicatorsLayout_t IndicatorsLayout,
                                                  unsigned NumCrss,MYSQL_RES *mysql_res)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
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
   unsigned NumTchs;
   unsigned NumStds;
   struct Ind_IndicatorsCrs Indicators;

   /***** Table start *****/
   fprintf (Gbl.F.Out,"<table class=\"INDICATORS\">");

   /***** Write table heading *****/
   switch (IndicatorsLayout)
     {
      case Ind_INDICATORS_BRIEF:
         fprintf (Gbl.F.Out,"<tr>"
                            "<th rowspan=\"3\" class=\"LEFT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th rowspan=\"3\" class=\"LEFT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th rowspan=\"3\" class=\"LEFT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th rowspan=\"3\" class=\"LEFT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th colspan=\"11\" class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "</tr>"
                            "<tr>"
                            "<th rowspan=\"2\" class=\"CENTER_TOP COLOR0\">"
                            "%s"
                            "</th>"
                            "<th colspan=\"2\" class=\"CENTER_TOP COLOR0\">"
                            "(A) %s"
                            "</th>"
                            "<th colspan=\"2\" class=\"CENTER_TOP COLOR0\">"
                            "(B) %s"
                            "</th>"
                            "<th colspan=\"2\" class=\"CENTER_TOP COLOR0\">"
                            "(C) %s"
                            "</th>"
                            "<th colspan=\"2\" class=\"CENTER_TOP COLOR0\">"
                            "(D) %s"
                            "</th>"
                            "<th colspan=\"2\" class=\"CENTER_TOP COLOR0\">"
                            "(E) %s"
                            "</th>"
                            "</tr>"
                            "<tr>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "</tr>",
                  Txt_Degree,
                  Txt_Course,
                  Txt_Institutional_BR_code,
                  Txt_Web_page_of_the_course,
                  Txt_Indicators,

                  Txt_No_INDEX,
                  Txt_Syllabus_of_the_course,
                  Txt_Guided_academic_assignments,
                  Txt_Online_tutoring,
                  Txt_Materials,
                  Txt_Assessment_criteria,

                  Txt_YES,
                  Txt_NO,

                  Txt_YES,
                  Txt_NO,

                  Txt_YES,
                  Txt_NO,

                  Txt_YES,
                  Txt_NO,

                  Txt_YES,
                  Txt_NO);
         break;
      case Ind_INDICATORS_FULL:
         fprintf (Gbl.F.Out,"<tr>"
                            "<th rowspan=\"3\" class=\"LEFT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th rowspan=\"3\" class=\"LEFT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th rowspan=\"3\" class=\"LEFT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th rowspan=\"3\" class=\"LEFT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th rowspan=\"3\" class=\"LEFT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th rowspan=\"3\" class=\"LEFT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th colspan=\"24\" class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "</tr>"
                            "<tr>"
                            "<th rowspan=\"2\" class=\"CENTER_TOP COLOR0\">"
                            "%s"
                            "</th>"
                            "<th colspan=\"5\" class=\"CENTER_TOP COLOR0\">"
                            "(A) %s"
                            "</th>"
                            "<th colspan=\"5\" class=\"CENTER_TOP COLOR0\">"
                            "(B) %s"
                            "</th>"
                            "<th colspan=\"5\" class=\"CENTER_TOP COLOR0\">"
                            "(C) %s"
                            "</th>"
                            "<th colspan=\"4\" class=\"CENTER_TOP COLOR0\">"
                            "(D) %s"
                            "</th>"
                            "<th colspan=\"4\" class=\"CENTER_TOP COLOR0\">"
                            "(E) %s"
                            "</th>"
                            "</tr>"
                            "<tr>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"LEFT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"LEFT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"LEFT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"RIGHT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"RIGHT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"RIGHT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"RIGHT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"RIGHT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"RIGHT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"RIGHT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"RIGHT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"CENTER_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"LEFT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "<th class=\"LEFT_MIDDLE COLOR0\">"
                            "%s"
                            "</th>"
                            "</tr>",
                  Txt_Degree,
                  Txt_Course,
                  Txt_Institutional_BR_code,
                  Txt_Web_page_of_the_course,
                  Txt_Teachers_ABBREVIATION,
                  Txt_Students_ABBREVIATION,
                  Txt_Indicators,

                  Txt_No_INDEX,
                  Txt_Syllabus_of_the_course,
                  Txt_Guided_academic_assignments,
                  Txt_Online_tutoring,
                  Txt_Materials,
                  Txt_Assessment_criteria,

                  Txt_YES,
                  Txt_NO,
                  Txt_INFO_TITLE[Inf_LECTURES],
                  Txt_INFO_TITLE[Inf_PRACTICALS],
                  Txt_INFO_TITLE[Inf_TEACHING_GUIDE],

                  Txt_YES,
                  Txt_NO,
                  Txt_Assignments,
                  Txt_Files_assignments,
                  Txt_Files_works,

                  Txt_YES,
                  Txt_NO,
                  Txt_Forum_threads,
                  Txt_Forum_posts,
                  Txt_Messages_sent_by_teachers,

                  Txt_YES,
                  Txt_NO,
                  Txt_No_of_files_in_DOCUM_zones,
                  Txt_No_of_files_in_SHARE_zones,

                  Txt_YES,
                  Txt_NO,
                  Txt_INFO_TITLE[Inf_ASSESSMENT],
                  Txt_INFO_TITLE[Inf_TEACHING_GUIDE]);
      break;
     }

   /***** List courses *****/
   mysql_data_seek (mysql_res, 0);
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

      if (Gbl.Stat.NumIndicators < 0 ||		// -1 means any number of indicators
          Gbl.Stat.NumIndicators == (long) Indicators.CountIndicators)
        {
         /* Write a row for this course */
         switch (IndicatorsLayout)
           {
            case Ind_INDICATORS_BRIEF:
               fprintf (Gbl.F.Out,"<tr>"
                                  "<td class=\"%s LEFT_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s LEFT_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s LEFT_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"DAT_SMALL LEFT_MIDDLE COLOR%u\">"
                                  "<a href=\"%s/?crs=%ld&amp;act=%ld\" target=\"_blank\">"
                                  "%s/?crs=%ld&amp;act=%ld"
                                  "</a>"
                                  "</td>"

                                  "<td class=\"%s RIGHT_MIDDLE COLOR%u\">"
                                  "%u"
                                  "</td>"

                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"

                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"

                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"

                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"

                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "</tr>",
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.RowEvenOdd,
                        row[0],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.RowEvenOdd,
                        row[1],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.RowEvenOdd,
                        row[3],
                        Gbl.RowEvenOdd,Cfg_HTTPS_URL_SWAD_CGI,CrsCod,Act_Actions[ActReqStaCrs].ActCod,
                                       Cfg_HTTPS_URL_SWAD_CGI,CrsCod,Act_Actions[ActReqStaCrs].ActCod,

                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.RowEvenOdd,
                        Indicators.CountIndicators,

                        "DAT_SMALL_GREEN",Gbl.RowEvenOdd,
                        Indicators.ThereIsSyllabus ? Txt_YES :
                                                     "",
                        "DAT_SMALL_RED",Gbl.RowEvenOdd,
                        Indicators.ThereIsSyllabus ? "" :
                                                     Txt_NO,

                        "DAT_SMALL_GREEN",Gbl.RowEvenOdd,
                        Indicators.ThereAreAssignments ? Txt_YES :
                                                         "",
                        "DAT_SMALL_RED",Gbl.RowEvenOdd,
                        Indicators.ThereAreAssignments ? "" :
                                                         Txt_NO,

                        "DAT_SMALL_GREEN",Gbl.RowEvenOdd,
                        Indicators.ThereIsOnlineTutoring ? Txt_YES :
                                                           "",
                        "DAT_SMALL_RED",Gbl.RowEvenOdd,
                        Indicators.ThereIsOnlineTutoring ? "" :
                                                           Txt_NO,

                        "DAT_SMALL_GREEN",Gbl.RowEvenOdd,
                        Indicators.ThereAreMaterials ? Txt_YES :
                                                       "",
                        "DAT_SMALL_RED",Gbl.RowEvenOdd,
                        Indicators.ThereAreMaterials ? "" :
                                                       Txt_NO,

                        "DAT_SMALL_GREEN",Gbl.RowEvenOdd,
                        Indicators.ThereIsAssessment ? Txt_YES :
                                                       "",
                        "DAT_SMALL_RED",Gbl.RowEvenOdd,
                        Indicators.ThereIsAssessment ? "" :
                                                       Txt_NO);
               break;
            case Ind_INDICATORS_FULL:
               /* Get number of users */
               NumStds = Usr_GetNumUsrsInCrs (Rol_STUDENT,CrsCod);
               NumTchs = Usr_GetNumUsrsInCrs (Rol_TEACHER,CrsCod);

               fprintf (Gbl.F.Out,"<tr>"
                                  "<td class=\"%s LEFT_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s LEFT_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s LEFT_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"DAT_SMALL LEFT_MIDDLE COLOR%u\">"
                                  "<a href=\"%s/?crs=%ld&amp;act=%ld\" target=\"_blank\">"
                                  "%s/?crs=%ld&amp;act=%ld"
                                  "</a>"
                                  "</td>"

                                  "<td class=\"%s RIGHT_MIDDLE COLOR%u\">"
                                  "%u"
                                  "</td>"
                                  "<td class=\"%s RIGHT_MIDDLE COLOR%u\">"
                                  "%u"
                                  "</td>"

                                  "<td class=\"%s RIGHT_MIDDLE COLOR%u\">"
                                  "%u"
                                  "</td>"

                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s LEFT_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s LEFT_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s LEFT_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"

                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s RIGHT_MIDDLE COLOR%u\">"
                                  "%u"
                                  "</td>"
                                  "<td class=\"%s RIGHT_MIDDLE COLOR%u\">"
                                  "%lu"
                                  "</td>"
                                  "<td class=\"%s RIGHT_MIDDLE COLOR%u\">"
                                  "%lu"
                                  "</td>"

                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s RIGHT_MIDDLE COLOR%u\">"
                                  "%u"
                                  "</td>"
                                  "<td class=\"%s RIGHT_MIDDLE COLOR%u\">"
                                  "%u"
                                  "</td>"
                                  "<td class=\"%s RIGHT_MIDDLE COLOR%u\">"
                                  "%u"
                                  "</td>"

                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s RIGHT_MIDDLE COLOR%u\">"
                                  "%lu"
                                  "</td>"
                                  "<td class=\"%s RIGHT_MIDDLE COLOR%u\">"
                                  "%lu"
                                  "</td>"

                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s CENTER_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s LEFT_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "<td class=\"%s LEFT_MIDDLE COLOR%u\">"
                                  "%s"
                                  "</td>"
                                  "</tr>",
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.RowEvenOdd,
                        row[0],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.RowEvenOdd,
                        row[1],
                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.RowEvenOdd,
                        row[3],
                        Gbl.RowEvenOdd,Cfg_HTTPS_URL_SWAD_CGI,CrsCod,Act_Actions[ActReqStaCrs].ActCod,
                                       Cfg_HTTPS_URL_SWAD_CGI,CrsCod,Act_Actions[ActReqStaCrs].ActCod,

                        NumTchs != 0 ? "DAT_SMALL_GREEN" :
                                       "DAT_SMALL_RED",
                        Gbl.RowEvenOdd,
                        NumTchs,
                        NumStds != 0 ? "DAT_SMALL_GREEN" :
                                       "DAT_SMALL_RED",
                        Gbl.RowEvenOdd,
                        NumStds,

                        Indicators.CourseAllOK ? "DAT_SMALL_GREEN" :
                        (Indicators.CoursePartiallyOK ? "DAT_SMALL" :
                                                        "DAT_SMALL_RED"),
                        Gbl.RowEvenOdd,
                        Indicators.CountIndicators,

                        "DAT_SMALL_GREEN",Gbl.RowEvenOdd,
                        Indicators.ThereIsSyllabus ? Txt_YES :
                                                     "",
                        "DAT_SMALL_RED",Gbl.RowEvenOdd,
                        Indicators.ThereIsSyllabus ? "" :
                                                     Txt_NO,
                        (Indicators.SyllabusLecSrc != Inf_INFO_SRC_NONE) ? "DAT_SMALL_GREEN" :
                                                                           "DAT_SMALL_RED",
                        Gbl.RowEvenOdd,
                        Txt_INFO_SRC_SHORT_TEXT[Indicators.SyllabusLecSrc],
                        (Indicators.SyllabusPraSrc != Inf_INFO_SRC_NONE) ? "DAT_SMALL_GREEN" :
                                                                           "DAT_SMALL_RED",
                        Gbl.RowEvenOdd,
                        Txt_INFO_SRC_SHORT_TEXT[Indicators.SyllabusPraSrc],
                        (Indicators.TeachingGuideSrc != Inf_INFO_SRC_NONE) ? "DAT_SMALL_GREEN" :
                                                                             "DAT_SMALL_RED",
                        Gbl.RowEvenOdd,
                        Txt_INFO_SRC_SHORT_TEXT[Indicators.TeachingGuideSrc],

                        "DAT_SMALL_GREEN",Gbl.RowEvenOdd,
                        Indicators.ThereAreAssignments ? Txt_YES :
                                                         "",
                        "DAT_SMALL_RED",Gbl.RowEvenOdd,
                        Indicators.ThereAreAssignments ? "" :
                                                         Txt_NO,
                        (Indicators.NumAssignments != 0) ? "DAT_SMALL_GREEN" :
                                                           "DAT_SMALL_RED",
                        Gbl.RowEvenOdd,
                        Indicators.NumAssignments,
                        (Indicators.NumFilesAssignments != 0) ? "DAT_SMALL_GREEN" :
                                                                "DAT_SMALL_RED",
                        Gbl.RowEvenOdd,
                        Indicators.NumFilesAssignments,
                        (Indicators.NumFilesWorks != 0) ? "DAT_SMALL_GREEN" :
                                                          "DAT_SMALL_RED",
                        Gbl.RowEvenOdd,
                        Indicators.NumFilesWorks,

                        "DAT_SMALL_GREEN",Gbl.RowEvenOdd,
                        Indicators.ThereIsOnlineTutoring ? Txt_YES :
                                                           "",
                        "DAT_SMALL_RED",Gbl.RowEvenOdd,
                        Indicators.ThereIsOnlineTutoring ? "" :
                                                           Txt_NO,
                        (Indicators.NumThreads != 0) ? "DAT_SMALL_GREEN" :
                                                       "DAT_SMALL_RED",
                        Gbl.RowEvenOdd,
                        Indicators.NumThreads,
                        (Indicators.NumPosts != 0) ? "DAT_SMALL_GREEN" :
                                                     "DAT_SMALL_RED",
                        Gbl.RowEvenOdd,
                        Indicators.NumPosts,
                        (Indicators.NumMsgsSentByTchs != 0) ? "DAT_SMALL_GREEN" :
                                                              "DAT_SMALL_RED",
                        Gbl.RowEvenOdd,
                        Indicators.NumMsgsSentByTchs,

                        "DAT_SMALL_GREEN",Gbl.RowEvenOdd,
                        Indicators.ThereAreMaterials ? Txt_YES :
                                                       "",
                        "DAT_SMALL_RED",Gbl.RowEvenOdd,
                        Indicators.ThereAreMaterials ? "" :
                                                       Txt_NO,
                        (Indicators.NumFilesInDocumentZones != 0) ? "DAT_SMALL_GREEN" :
                                                                    "DAT_SMALL_RED",
                        Gbl.RowEvenOdd,
                        Indicators.NumFilesInDocumentZones,
                        (Indicators.NumFilesInSharedZones != 0) ? "DAT_SMALL_GREEN" :
                                                                  "DAT_SMALL_RED",
                        Gbl.RowEvenOdd,
                        Indicators.NumFilesInSharedZones,

                        "DAT_SMALL_GREEN",Gbl.RowEvenOdd,
                        Indicators.ThereIsAssessment ? Txt_YES :
                                                       "",
                        "DAT_SMALL_RED",Gbl.RowEvenOdd,
                        Indicators.ThereIsAssessment ? "" :
                                                       Txt_NO,
                        (Indicators.AssessmentSrc != Inf_INFO_SRC_NONE) ? "DAT_SMALL_GREEN" :
                                                                          "DAT_SMALL_RED",
                        Gbl.RowEvenOdd,
                        Txt_INFO_SRC_SHORT_TEXT[Indicators.AssessmentSrc],
                        (Indicators.TeachingGuideSrc != Inf_INFO_SRC_NONE) ? "DAT_SMALL_GREEN" :
                                                                             "DAT_SMALL_RED",
                        Gbl.RowEvenOdd,
                        Txt_INFO_SRC_SHORT_TEXT[Indicators.TeachingGuideSrc]);
               break;
              }
        }
     }

   /***** End table *****/
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
   Indicators->NumFilesInDocumentZonesCrs = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_ADMI_DOCUM_CRS,CrsCod);
   Indicators->NumFilesInDocumentZonesGrp = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_ADMI_DOCUM_GRP,CrsCod);
   Indicators->NumFilesInDocumentZones    = Indicators->NumFilesInDocumentZonesCrs +
	                                    Indicators->NumFilesInDocumentZonesGrp;
   Indicators->NumFilesInSharedZonesCrs   = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_ADMI_SHARE_CRS,CrsCod);
   Indicators->NumFilesInSharedZonesGrp   = Ind_GetNumFilesOfCrsFileZoneFromDB (Brw_ADMI_SHARE_GRP,CrsCod);
   Indicators->NumFilesInSharedZones      = Indicators->NumFilesInSharedZonesCrs +
	                                    Indicators->NumFilesInSharedZonesGrp;

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
   Indicators->ThereAreMaterials = (Indicators->NumFilesInDocumentZones != 0) ||
                                   (Indicators->NumFilesInSharedZones   != 0);
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
