// swad_indicators.c: indicators of courses

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <mysql/mysql.h>	// To access MySQL databases
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_assignment_database.h"
#include "swad_box.h"
#include "swad_browser_database.h"
#include "swad_database.h"
#include "swad_degree_type.h"
#include "swad_department.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_forum_database.h"
#include "swad_global.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_indicator.h"
#include "swad_indicator_database.h"
#include "swad_message.h"
#include "swad_message_database.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_theme.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

typedef enum
  {
   Ind_INDICATORS_BRIEF,
   Ind_INDICATORS_FULL,
  } Ind_IndicatorsLayout_t;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ind_GetParsIndicators (struct Ind_Indicators *Indicators);
static void Ind_GetParNumIndicators (struct Ind_Indicators *Indicators);
static bool Ind_GetIfShowBigList (struct Ind_Indicators *Indicators,
                                  unsigned NumCrss);
static void Ind_PutButtonToConfirmIWantToSeeBigList (struct Ind_Indicators *Indicators,
                                                     unsigned NumCrss);
static void Ind_PutParsConfirmIWantToSeeBigList (void *Indicators);

static void Ind_GetNumCoursesWithIndicators (unsigned NumCrssWithIndicatorYes[1 + Ind_NUM_INDICATORS],
                                             unsigned NumCrss,MYSQL_RES *mysql_res);
static void Ind_ShowNumCoursesWithIndicators (const struct Ind_Indicators *Indicators,
                                              unsigned NumCrssWithIndicatorYes[1 + Ind_NUM_INDICATORS],
                                              unsigned NumCrss,bool PutForm);
static void Ind_ShowTableOfCoursesWithIndicators (const struct Ind_Indicators *Indicators,
	                                          Ind_IndicatorsLayout_t IndicatorsLayout,
                                                  unsigned NumCrss,MYSQL_RES *mysql_res);
static unsigned Ind_GetAndUpdateNumIndicatorsCrs (long CrsCod);

/*****************************************************************************/
/******************* Request showing statistics of courses *******************/
/*****************************************************************************/

void Ind_ReqIndicatorsCourses (void)
  {
   extern const char *Hlp_ANALYTICS_Indicators;
   extern const char *Par_CodeStr[];
   extern const char *Txt_Scope;
   extern const char *Txt_Types_of_degree;
   extern const char *Txt_only_if_the_scope_is_X;
   extern const char *Txt_Department;
   extern const char *Txt_Any_department;
   extern const char *Txt_Number_of_indicators;
   extern const char *Txt_Indicators_of_courses;
   extern const char *Txt_Show_more_details;
   struct Ind_Indicators Indicators;
   char *SelectClass;
   MYSQL_RES *mysql_res;
   unsigned NumCrss;
   unsigned NumCrssWithIndicatorYes[1 + Ind_NUM_INDICATORS];
   unsigned NumCrssToList;
   unsigned Ind;

   /***** Get parameters *****/
   Ind_GetParsIndicators (&Indicators);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Indicators_of_courses,
                 NULL,NULL,
                 Hlp_ANALYTICS_Indicators,Box_NOT_CLOSABLE);

      /***** Form to update indicators *****/
      /* Begin form and table */
      Frm_BeginForm (ActReqStaCrs);
	 HTM_TABLE_BeginWidePadding (2);

	    /* Scope */
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("RT","ScopeInd",Txt_Scope);

	       /* Data */
	       HTM_TD_Begin ("class=\"LT\"");
		  Sco_PutSelectorScope ("ScopeInd",HTM_SUBMIT_ON_CHANGE);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /* Compute stats for a type of degree */
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("RT",Par_CodeStr[ParCod_OthDegTyp],Txt_Types_of_degree);

	       /* Data */
	       HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
		  DegTyp_WriteSelectorDegreeTypes (Indicators.DegTypCod);
		  HTM_Txt (" (");
		  HTM_TxtF (Txt_only_if_the_scope_is_X,Cfg_PLATFORM_SHORT_NAME);
		  HTM_Txt (")");
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /* Compute stats for courses with teachers belonging to any department or to a particular departament? */
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("RT",Par_CodeStr[ParCod_Dpt],Txt_Department);

	       /* Data */
	       HTM_TD_Begin ("class=\"LT\"");
		  if (asprintf (&SelectClass,"INDICATORS_INPUT INPUT_%s",
		                The_GetSuffix ()) < 0)
		     Err_NotEnoughMemoryExit ();
		  Dpt_WriteSelectorDepartment (Gbl.Hierarchy.Node[HieLvl_INS].Cod,	// Departments in current insitution
					       Indicators.DptCod,			// Selected department
					       Par_CodeStr[ParCod_Dpt],			// Parameter name
					       SelectClass,				// Selector class
					       -1L,					// First option
					       Txt_Any_department,			// Text when no department selected
					       HTM_SUBMIT_ON_CHANGE);
		  free (SelectClass);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** Get courses from database *****/
	    /* The result will contain courses with any number of indicators
	       If Indicators.NumIndicators <  0 ==> all courses in result will be listed
	       If Indicators.NumIndicators >= 0 ==> only those courses in result
						  with Indicators.NumIndicators set to yes
						  will be listed */
	    NumCrss = Ind_DB_GetTableOfCourses (&mysql_res,&Indicators);

	    /***** Get vector with numbers of courses with 0, 1, 2... indicators set to yes *****/
	    Ind_GetNumCoursesWithIndicators (NumCrssWithIndicatorYes,NumCrss,mysql_res);

	    /* Selection of the number of indicators */
	    HTM_TR_Begin (NULL);

	       Frm_LabelColumn ("RT","",Txt_Number_of_indicators);

	       HTM_TD_Begin ("class=\"LT\"");
		  Ind_ShowNumCoursesWithIndicators (&Indicators,NumCrssWithIndicatorYes,NumCrss,true);
	       HTM_TD_End ();

	    HTM_TR_End ();

	 /* End table and form */
	 HTM_TABLE_End ();
      Frm_EndForm ();

      /***** Show the stats of courses *****/
      for (Ind  = 0, NumCrssToList = 0;
	   Ind <= Ind_NUM_INDICATORS;
	   Ind++)
	 if (Indicators.IndicatorsSelected[Ind])
	    NumCrssToList += NumCrssWithIndicatorYes[Ind];
      if (Ind_GetIfShowBigList (&Indicators,NumCrssToList))
	{
	 /* Show table */
	 Ind_ShowTableOfCoursesWithIndicators (&Indicators,Ind_INDICATORS_BRIEF,NumCrss,mysql_res);

	 /* Button to show more details */
	 Frm_BeginForm (ActSeeAllStaCrs);
	    Sco_PutParScope ("ScopeInd",Gbl.Scope.Current);
	    ParCod_PutPar (ParCod_OthDegTyp,Indicators.DegTypCod);
	    ParCod_PutPar (ParCod_Dpt      ,Indicators.DptCod   );
	    if (Indicators.StrIndicatorsSelected[0])
	       Par_PutParString (NULL,"Indicators",Indicators.StrIndicatorsSelected);
	    Btn_PutConfirmButton (Txt_Show_more_details);
	 Frm_EndForm ();
	}

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Get parameters related to indicators of courses ***************/
/*****************************************************************************/

static void Ind_GetParsIndicators (struct Ind_Indicators *Indicators)
  {
   /***** Get scope *****/
   Gbl.Scope.Allowed = 1 << HieLvl_SYS |
	               1 << HieLvl_CTY |
		       1 << HieLvl_INS |
		       1 << HieLvl_CTR |
		       1 << HieLvl_DEG |
		       1 << HieLvl_CRS;
   Gbl.Scope.Default = HieLvl_CRS;
   Sco_GetScope ("ScopeInd");

   /***** Get degree type code *****/
   Indicators->DegTypCod = (Gbl.Scope.Current == HieLvl_SYS) ?
	                   ParCod_GetPar (ParCod_OthDegTyp) :	// -1L (any degree type) is allowed here
                           -1L;

   /***** Get department code *****/
   Indicators->DptCod = ParCod_GetPar (ParCod_Dpt);		// -1L (any department) is allowed here

   /***** Get number of indicators *****/
   Ind_GetParNumIndicators (Indicators);
  }

/*****************************************************************************/
/*********************** Show statistics of courses **************************/
/*****************************************************************************/

void Ind_ShowIndicatorsCourses (void)
  {
   struct Ind_Indicators Indicators;
   MYSQL_RES *mysql_res;
   unsigned NumCrss;
   unsigned NumCrssWithIndicatorYes[1 + Ind_NUM_INDICATORS];

   /***** Get parameters *****/
   Ind_GetParsIndicators (&Indicators);

   /***** Get courses from database *****/
   NumCrss = Ind_DB_GetTableOfCourses (&mysql_res,&Indicators);

   /***** Get vector with numbers of courses with 0, 1, 2... indicators set to yes *****/
   Ind_GetNumCoursesWithIndicators (NumCrssWithIndicatorYes,NumCrss,mysql_res);

   /***** Show table with numbers of courses with 0, 1, 2... indicators set to yes *****/
   Ind_ShowNumCoursesWithIndicators (&Indicators,NumCrssWithIndicatorYes,NumCrss,false);

   /***** Show the stats of courses *****/
   Ind_ShowTableOfCoursesWithIndicators (&Indicators,Ind_INDICATORS_FULL,NumCrss,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************** Get parameter with the number of indicators *****************/
/*****************************************************************************/

static void Ind_GetParNumIndicators (struct Ind_Indicators *Indicators)
  {
   unsigned Ind;
   const char *Ptr;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   long Indicator;

   /***** Get parameter multiple with list of indicators selected *****/
   Par_GetParMultiToText ("Indicators",Indicators->StrIndicatorsSelected,Ind_MAX_SIZE_INDICATORS_SELECTED);

   /***** Set which indicators have been selected (checkboxes on) *****/
   if (Indicators->StrIndicatorsSelected[0])
     {
      /* Reset all indicators */
      for (Ind = 0;
	   Ind <= Ind_NUM_INDICATORS;
	   Ind++)
	 Indicators->IndicatorsSelected[Ind] = false;

      /* Set indicators selected */
      for (Ptr = Indicators->StrIndicatorsSelected;
	   *Ptr;
	   )
	{
	 /* Get next indicator selected */
	 Par_GetNextStrUntilSeparParMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
	 Indicator = Str_ConvertStrCodToLongCod (LongStr);

	 /* Set each indicator in list StrIndicatorsSelected as selected */
	 for (Ind = 0;
	      Ind <= Ind_NUM_INDICATORS;
	      Ind++)
	    if ((long) Ind == Indicator)
	       Indicators->IndicatorsSelected[Ind] = true;
	}
     }
   else
      /* Set all indicators */
      for (Ind = 0;
	   Ind <= Ind_NUM_INDICATORS;
	   Ind++)
	 Indicators->IndicatorsSelected[Ind] = true;
  }

/*****************************************************************************/
/******* Show form to confirm that I want to see a big list of courses *******/
/*****************************************************************************/

static bool Ind_GetIfShowBigList (struct Ind_Indicators *Indicators,
                                  unsigned NumCrss)
  {
   bool ShowBigList;

   /***** If list of courses is too big... *****/
   if (NumCrss <= Cfg_MIN_NUM_COURSES_TO_CONFIRM_SHOW_BIG_LIST)
      return true;	// List is not too big ==> show it

   /***** Get parameter with user's confirmation to see a big list of courses *****/
   if (!(ShowBigList = Par_GetParBool ("ShowBigList")))
      Ind_PutButtonToConfirmIWantToSeeBigList (Indicators,NumCrss);

   return ShowBigList;
  }

/*****************************************************************************/
/****** Show form to confirm that I want to see a big list of courses ********/
/*****************************************************************************/

static void Ind_PutButtonToConfirmIWantToSeeBigList (struct Ind_Indicators *Indicators,
                                                     unsigned NumCrss)
  {
   extern const char *Txt_The_list_of_X_courses_is_too_large_to_be_displayed;
   extern const char *Txt_Show_anyway;

   /***** Show alert and button to confirm that I want to see the big list *****/
   Ale_ShowAlertAndButton (Gbl.Action.Act,NULL,NULL,
                           Ind_PutParsConfirmIWantToSeeBigList,Indicators,
                           Btn_CONFIRM_BUTTON,Txt_Show_anyway,
			   Ale_WARNING,Txt_The_list_of_X_courses_is_too_large_to_be_displayed,
                           NumCrss);
  }

static void Ind_PutParsConfirmIWantToSeeBigList (void *Indicators)
  {
   if (Indicators)
     {
      Sco_PutParScope ("ScopeInd",Gbl.Scope.Current);
      ParCod_PutPar (ParCod_OthDegTyp,((struct Ind_Indicators *) Indicators)->DegTypCod);
      ParCod_PutPar (ParCod_Dpt      ,((struct Ind_Indicators *) Indicators)->DptCod   );
      if (((struct Ind_Indicators *) Indicators)->StrIndicatorsSelected[0])
	 Par_PutParString (NULL,"Indicators",((struct Ind_Indicators *) Indicators)->StrIndicatorsSelected);
      Par_PutParChar ("ShowBigList",'Y');
     }
  }

/*****************************************************************************/
/** Get vector with numbers of courses with 0, 1, 2... indicators set to yes */
/*****************************************************************************/

static void Ind_GetNumCoursesWithIndicators (unsigned NumCrssWithIndicatorYes[1 + Ind_NUM_INDICATORS],
                                             unsigned NumCrss,MYSQL_RES *mysql_res)
  {
   MYSQL_ROW row;
   unsigned NumCrs;
   long CrsCod;
   unsigned Ind;
   unsigned NumIndicators;

   /***** Reset counters of courses with each number of indicators *****/
   for (Ind = 0;
	Ind <= Ind_NUM_INDICATORS;
	Ind++)
      NumCrssWithIndicatorYes[Ind] = 0;

   /***** List courses *****/
   for (NumCrs = 0, The_ResetRowColor ();
	NumCrs < NumCrss;
	NumCrs++, The_ChangeRowColor ())
     {
      /* Get next course */
      row = mysql_fetch_row (mysql_res);

      /* Get course code (row[2]) */
      if ((CrsCod = Str_ConvertStrCodToLongCod (row[2])) <= 0)
         Err_WrongCourseExit ();

      /* Get stored number of indicators of this course */
      NumIndicators = Ind_GetAndUpdateNumIndicatorsCrs (CrsCod);
      NumCrssWithIndicatorYes[NumIndicators]++;
     }
  }

/*****************************************************************************/
/** Show table with numbers of courses with 0, 1, 2... indicators set to yes */
/*****************************************************************************/

static void Ind_ShowNumCoursesWithIndicators (const struct Ind_Indicators *Indicators,
                                              unsigned NumCrssWithIndicatorYes[1 + Ind_NUM_INDICATORS],
                                              unsigned NumCrss,bool PutForm)
  {
   extern const char *Txt_Indicators;
   extern const char *Txt_Courses;
   extern const char *Txt_Total;
   char *ClassNormal;
   char *ClassHighlight;
   const char *Class;
   unsigned Ind;

   /***** Initialize classes *****/
   if (asprintf (&ClassNormal   ,"RM DAT_LIGHT_%s"       ,The_GetSuffix ()) < 0)
      Err_NotEnoughMemoryExit ();
   if (asprintf (&ClassHighlight,"RM DAT_%s BG_HIGHLIGHT",The_GetSuffix ()) < 0)
      Err_NotEnoughMemoryExit ();

   /***** Write number of courses with each number of indicators valid *****/
   HTM_TABLE_BeginPadding (2);

      /* Header */
      HTM_TR_Begin (NULL);
	 if (PutForm)
	    HTM_TH_Empty (1);
	 HTM_TH      (Txt_Indicators,HTM_HEAD_RIGHT);
	 HTM_TH_Span (Txt_Courses   ,HTM_HEAD_RIGHT,1,2,NULL);
      HTM_TR_End ();

      for (Ind  = 0;
	   Ind <= Ind_NUM_INDICATORS;
	   Ind++)
	{
	 Class = Indicators->IndicatorsSelected[Ind] ? ClassHighlight :
						       ClassNormal;
	 HTM_TR_Begin (NULL);

	    if (PutForm)
	      {
	       HTM_TD_Begin ("class=\"%s\"",Class);
		  HTM_INPUT_CHECKBOX ("Indicators",HTM_SUBMIT_ON_CHANGE,
				      "id=\"Indicators%u\" value=\"%u\"%s",
				      Ind,Ind,
				      Indicators->IndicatorsSelected[Ind] ? " checked=\"checked\"" :
									    "");
	       HTM_TD_End ();
	      }

	    HTM_TD_Begin ("class=\"%s\"",Class);
	       HTM_LABEL_Begin ("for=\"Indicators%u\"",Ind);
		  HTM_Unsigned (Ind);
	       HTM_LABEL_End ();
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"%s\"",Class);
	       HTM_Unsigned (NumCrssWithIndicatorYes[Ind]);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"%s\"",Class);
	       HTM_TxtF ("(%.1f%%)",
			 NumCrss ? (double) NumCrssWithIndicatorYes[Ind] * 100.0 /
				   (double) NumCrss :
				   0.0);
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

      /***** Write total of courses *****/
      HTM_TR_Begin (NULL);

	 if (PutForm)
	    HTM_TD_Empty (1);

	 HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP\"",
	               The_GetSuffix ());
	    HTM_Txt (Txt_Total);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP\"",
	               The_GetSuffix ());
	    HTM_Unsigned (NumCrss);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP\"",
	               The_GetSuffix ());
	    HTM_TxtF ("(%.1f%%)",100.0);
	 HTM_TD_End ();

      HTM_TR_End ();

   HTM_TABLE_End ();

   free (ClassHighlight);
   free (ClassNormal);
  }

/*****************************************************************************/
/****************** Get and show total number of courses *********************/
/*****************************************************************************/

static void Ind_ShowTableOfCoursesWithIndicators (const struct Ind_Indicators *Indicators,
	                                          Ind_IndicatorsLayout_t IndicatorsLayout,
                                                  unsigned NumCrss,MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Degree;
   extern const char *Txt_Course;
   extern const char *Txt_Institutional_BR_code;
   extern const char *Txt_Web_page_of_the_course;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_Indicators;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Syllabus_of_the_course;
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   extern const char *Txt_Number_of_files_in_SHARE_zones;
   extern const char *Txt_Number_of_files_in_DOCUM_zones;
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
   extern const char *Txt_INFO_SRC_SHORT_TEXT[Inf_NUM_SOURCES];
   extern const char *Txt_Courses;
   MYSQL_ROW row;
   unsigned NumCrs;
   long CrsCod;
   unsigned NumTchs;
   unsigned NumStds;
   unsigned NumIndicators;
   struct Ind_IndicatorsCrs IndicatorsCrs;
   long ActCod;

   /***** Begin table *****/
   HTM_TABLE_Begin ("INDICATORS");

      /***** Write table heading *****/
      switch (IndicatorsLayout)
	{
	 case Ind_INDICATORS_BRIEF:
	    HTM_TR_Begin (NULL);
	       HTM_TH_Span (Txt_Degree                ,HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_Course                ,HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_Institutional_BR_code ,HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_Web_page_of_the_course,HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_Indicators            ,HTM_HEAD_CENTER,1,11,NULL);
	    HTM_TR_End ();

	    HTM_TR_Begin (NULL);
	       HTM_TH_Span (Txt_No_INDEX              ,HTM_HEAD_CENTER,2, 1,NULL);
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,2,NULL);
		  HTM_TxtF ("(A) %s",Txt_Syllabus_of_the_course);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,2,NULL);
		  HTM_TxtF ("(B) %s",Txt_Guided_academic_assignments);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,2,NULL);
		  HTM_TxtF ("(C) %s",Txt_Online_tutoring);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,2,NULL);
		  HTM_TxtF ("(D) %s",Txt_Materials);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,2,NULL);
		  HTM_TxtF ("(E) %s",Txt_Assessment_criteria);
	       HTM_TH_End ();
	    HTM_TR_End ();

	    HTM_TR_Begin (NULL);
	       HTM_TH      (Txt_YES                            ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_NO                             ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_YES                            ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_NO                             ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_YES                            ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_NO                             ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_YES                            ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_NO                             ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_YES                            ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_NO                             ,HTM_HEAD_CENTER);
	    HTM_TR_End ();
	    break;
	 case Ind_INDICATORS_FULL:
	    HTM_TR_Begin (NULL);
	       HTM_TH_Span (Txt_Degree                         ,HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_Course                         ,HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_Institutional_BR_code          ,HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_Web_page_of_the_course         ,HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH],HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD],HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_Indicators                     ,HTM_HEAD_CENTER,1,24,NULL);
	    HTM_TR_End ();

	    HTM_TR_Begin (NULL);
	       HTM_TH_Span (Txt_No_INDEX                       ,HTM_HEAD_CENTER,2,1,NULL);
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,5,NULL);
		  HTM_TxtF ("(A) %s",Txt_Syllabus_of_the_course);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,5,NULL);
		  HTM_TxtF ("(B) %s",Txt_Guided_academic_assignments);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,5,NULL);
		  HTM_TxtF ("(C) %s",Txt_Online_tutoring);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,4,NULL);
		  HTM_TxtF ("(D) %s",Txt_Materials);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,4,NULL);
		  HTM_TxtF ("(E) %s",Txt_Assessment_criteria);
	       HTM_TH_End ();
	    HTM_TR_End ();

	    HTM_TR_Begin (NULL);
	       HTM_TH      (Txt_YES                            ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_NO                             ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_INFO_TITLE[Inf_LECTURES]       ,HTM_HEAD_LEFT  );
	       HTM_TH      (Txt_INFO_TITLE[Inf_PRACTICALS]     ,HTM_HEAD_LEFT  );
	       HTM_TH      (Txt_INFO_TITLE[Inf_TEACHING_GUIDE] ,HTM_HEAD_LEFT  );
	       HTM_TH      (Txt_YES                            ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_NO                             ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_Assignments                    ,HTM_HEAD_RIGHT );
	       HTM_TH      (Txt_Files_assignments              ,HTM_HEAD_RIGHT );
	       HTM_TH      (Txt_Files_works                    ,HTM_HEAD_RIGHT );
	       HTM_TH      (Txt_YES                            ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_NO                             ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_Forum_threads                  ,HTM_HEAD_RIGHT );
	       HTM_TH      (Txt_Forum_posts                    ,HTM_HEAD_RIGHT );
	       HTM_TH      (Txt_Messages_sent_by_teachers      ,HTM_HEAD_RIGHT );
	       HTM_TH      (Txt_YES                            ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_NO                             ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_Number_of_files_in_DOCUM_zones ,HTM_HEAD_RIGHT );
	       HTM_TH      (Txt_Number_of_files_in_SHARE_zones ,HTM_HEAD_RIGHT );
	       HTM_TH      (Txt_YES                            ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_NO                             ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_INFO_TITLE[Inf_ASSESSMENT]     ,HTM_HEAD_LEFT  );
	       HTM_TH      (Txt_INFO_TITLE[Inf_TEACHING_GUIDE] ,HTM_HEAD_LEFT  );
	    HTM_TR_End ();
	 break;
	}

      /***** List courses *****/
      mysql_data_seek (mysql_res,0);
      for (NumCrs = 0, The_ResetRowColor ();
	   NumCrs < NumCrss;
	   NumCrs++, The_ChangeRowColor ())
	{
	 /* Get next course */
	 row = mysql_fetch_row (mysql_res);

	 /* Get course code (row[2]) */
	 if ((CrsCod = Str_ConvertStrCodToLongCod (row[2])) <= 0)
	    Err_WrongCourseExit ();

	 /* Get stored number of indicators of this course */
	 NumIndicators = Ind_GetAndUpdateNumIndicatorsCrs (CrsCod);
	 if (Indicators->IndicatorsSelected[NumIndicators])
	   {
	    /* Compute and store indicators */
	    Ind_ComputeAndStoreIndicatorsCrs (CrsCod,(int) NumIndicators,&IndicatorsCrs);

	    /* The number of indicators may have changed */
	    if (Indicators->IndicatorsSelected[IndicatorsCrs.NumIndicators])
	      {
	       ActCod = Act_GetActCod (ActReqStaCrs);

	       /* Write a row for this course */
	       switch (IndicatorsLayout)
		 {
		  case Ind_INDICATORS_BRIEF:
		     HTM_TR_Begin (NULL);

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.CourseAllOK ? "DAT_SMALL_GREEN" :
				      (IndicatorsCrs.CoursePartiallyOK ? "DAT_SMALL" :
									 "DAT_SMALL_RED"),
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Txt (row[0]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.CourseAllOK ? "DAT_SMALL_GREEN" :
				      (IndicatorsCrs.CoursePartiallyOK ? "DAT_SMALL" :
									 "DAT_SMALL_RED"),
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Txt (row[1]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.CourseAllOK ? "DAT_SMALL_GREEN" :
				      (IndicatorsCrs.CoursePartiallyOK ? "DAT_SMALL" :
									 "DAT_SMALL_RED"),
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Txt (row[3]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM DAT_SMALL_%s %s\"",
				      The_GetSuffix (),
			              The_GetColorRows ());
			   HTM_A_Begin ("href=\"%s/?crs=%ld&amp;act=%ld\" target=\"_blank\"",
					Cfg_URL_SWAD_CGI,CrsCod,ActCod);
			      HTM_TxtF ("%s/?crs=%ld&amp;act=%ld",
					Cfg_URL_SWAD_CGI,CrsCod,ActCod);
			   HTM_A_End ();
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      IndicatorsCrs.CourseAllOK ? "DAT_SMALL_GREEN" :
				      (IndicatorsCrs.CoursePartiallyOK ? "DAT_SMALL" :
									 "DAT_SMALL_RED"),
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumIndicators);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
			              The_GetSuffix (),
				      The_GetColorRows ());
			   if (IndicatorsCrs.ThereIsSyllabus)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (!IndicatorsCrs.ThereIsSyllabus)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (IndicatorsCrs.ThereAreAssignments)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (!IndicatorsCrs.ThereAreAssignments)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (IndicatorsCrs.ThereIsOnlineTutoring)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (!IndicatorsCrs.ThereIsOnlineTutoring)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (IndicatorsCrs.ThereAreMaterials)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (!IndicatorsCrs.ThereAreMaterials)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (IndicatorsCrs.ThereIsAssessment)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (!IndicatorsCrs.ThereIsAssessment)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

		     HTM_TR_End ();
		     break;
		  case Ind_INDICATORS_FULL:
		     /* Get number of users */
		     NumTchs = Enr_GetNumUsrsInCrss (HieLvl_CRS,CrsCod,
						     1 << Rol_NET |	// Non-editing teachers
						     1 << Rol_TCH);	// Teachers
		     NumStds = Enr_GetNumUsrsInCrss (HieLvl_CRS,CrsCod,
						     1 << Rol_STD);	// Students

		     HTM_TR_Begin (NULL);

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.CourseAllOK ? "DAT_SMALL_GREEN" :
				      (IndicatorsCrs.CoursePartiallyOK ? "DAT_SMALL" :
									 "DAT_SMALL_RED"),
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Txt (row[0]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.CourseAllOK ? "DAT_SMALL_GREEN" :
				      (IndicatorsCrs.CoursePartiallyOK ? "DAT_SMALL" :
									 "DAT_SMALL_RED"),
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Txt (row[1]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.CourseAllOK ? "DAT_SMALL_GREEN" :
				      (IndicatorsCrs.CoursePartiallyOK ? "DAT_SMALL" :
									 "DAT_SMALL_RED"),
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Txt (row[3]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM DAT_SMALL_%s %s\"",
				      The_GetSuffix (),
			              The_GetColorRows ());
			   HTM_A_Begin ("href=\"%s/?crs=%ld&amp;act=%ld\" target=\"_blank\"",
					Cfg_URL_SWAD_CGI,CrsCod,ActCod);
			      HTM_TxtF ("%s/?crs=%ld&amp;act=%ld",
					Cfg_URL_SWAD_CGI,CrsCod,ActCod);
			   HTM_A_End ();
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      NumTchs != 0 ? "DAT_SMALL_GREEN" :
						     "DAT_SMALL_RED",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Unsigned (NumTchs);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      NumStds != 0 ? "DAT_SMALL_GREEN" :
						     "DAT_SMALL_RED",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Unsigned (NumStds);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      IndicatorsCrs.CourseAllOK ? "DAT_SMALL_GREEN" :
				      (IndicatorsCrs.CoursePartiallyOK ? "DAT_SMALL" :
									 "DAT_SMALL_RED"),
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumIndicators);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (IndicatorsCrs.ThereIsSyllabus)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (!IndicatorsCrs.ThereIsSyllabus)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      (IndicatorsCrs.SyllabusLecSrc != Inf_NONE) ? "DAT_SMALL_GREEN" :
										   "DAT_SMALL_RED",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Txt (Txt_INFO_SRC_SHORT_TEXT[IndicatorsCrs.SyllabusLecSrc]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      (IndicatorsCrs.SyllabusPraSrc != Inf_NONE) ? "DAT_SMALL_GREEN" :
										   "DAT_SMALL_RED",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Txt (Txt_INFO_SRC_SHORT_TEXT[IndicatorsCrs.SyllabusPraSrc]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      (IndicatorsCrs.TeachingGuideSrc != Inf_NONE) ? "DAT_SMALL_GREEN" :
										     "DAT_SMALL_RED",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Txt (Txt_INFO_SRC_SHORT_TEXT[IndicatorsCrs.TeachingGuideSrc]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (IndicatorsCrs.ThereAreAssignments)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (!IndicatorsCrs.ThereAreAssignments)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      (IndicatorsCrs.NumAssignments != 0) ? "DAT_SMALL_GREEN" :
									    "DAT_SMALL_RED",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumAssignments);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      (IndicatorsCrs.NumFilesAssignments != 0) ? "DAT_SMALL_GREEN" :
										 "DAT_SMALL_RED",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumFilesAssignments);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      (IndicatorsCrs.NumFilesWorks != 0) ? "DAT_SMALL_GREEN" :
									   "DAT_SMALL_RED",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumFilesWorks);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (IndicatorsCrs.ThereIsOnlineTutoring)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (!IndicatorsCrs.ThereIsOnlineTutoring)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s RM\"",
				      (IndicatorsCrs.NumThreads != 0) ? "DAT_SMALL_GREEN" :
									"DAT_SMALL_RED",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumThreads);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      (IndicatorsCrs.NumPosts != 0) ? "DAT_SMALL_GREEN" :
								      "DAT_SMALL_RED",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumPosts);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      (IndicatorsCrs.NumMsgsSentByTchs != 0) ? "DAT_SMALL_GREEN" :
									       "DAT_SMALL_RED",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumMsgsSentByTchs);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (IndicatorsCrs.ThereAreMaterials)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (!IndicatorsCrs.ThereAreMaterials)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      (IndicatorsCrs.NumFilesInDocumentZones != 0) ? "DAT_SMALL_GREEN" :
										     "DAT_SMALL_RED",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumFilesInDocumentZones);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      (IndicatorsCrs.NumFilesInSharedZones != 0) ? "DAT_SMALL_GREEN" :
										   "DAT_SMALL_RED",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumFilesInSharedZones);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (IndicatorsCrs.ThereIsAssessment)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   if (!IndicatorsCrs.ThereIsAssessment)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      (IndicatorsCrs.AssessmentSrc != Inf_NONE) ? "DAT_SMALL_GREEN" :
										  "DAT_SMALL_RED",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Txt (Txt_INFO_SRC_SHORT_TEXT[IndicatorsCrs.AssessmentSrc]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      (IndicatorsCrs.TeachingGuideSrc != Inf_NONE) ? "DAT_SMALL_GREEN" :
										     "DAT_SMALL_RED",
				      The_GetSuffix (),
				      The_GetColorRows ());
			   HTM_Txt (Txt_INFO_SRC_SHORT_TEXT[IndicatorsCrs.TeachingGuideSrc]);
			HTM_TD_End ();

		     HTM_TR_End ();
		     break;
		    }
	      }
	   }
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************ Get number of indicators of a course from database *************/
/************ If not stored ==> compute and store it             *************/
/*****************************************************************************/

static unsigned Ind_GetAndUpdateNumIndicatorsCrs (long CrsCod)
  {
   unsigned NumIndicators;
   struct Ind_IndicatorsCrs IndicatorsCrs;
   int NumIndicatorsFromDB = Ind_GetNumIndicatorsCrsFromDB (CrsCod);

   /***** If number of indicators is not already computed ==> compute it! *****/
   if (NumIndicatorsFromDB >= 0)
      NumIndicators = (unsigned) NumIndicatorsFromDB;
   else	// Number of indicators is not already computed
     {
      /***** Compute and store number of indicators *****/
      Ind_ComputeAndStoreIndicatorsCrs (CrsCod,NumIndicatorsFromDB,&IndicatorsCrs);
      NumIndicators = IndicatorsCrs.NumIndicators;
     }
   return NumIndicators;
  }

/*****************************************************************************/
/************ Get number of indicators of a course from database *************/
/*****************************************************************************/
// This function returns -1 if number of indicators is not yet calculated

int Ind_GetNumIndicatorsCrsFromDB (long CrsCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   int NumIndicatorsFromDB = -1;	// -1 means not yet calculated

   /***** Get number of indicators of a course from database *****/
   if (Ind_DB_GetNumIndicatorsCrs (&mysql_res,CrsCod))
     {
      /***** Get row *****/
      row = mysql_fetch_row (mysql_res);

      /***** Get number of indicators (row[0]) *****/
      if (sscanf (row[0],"%d",&NumIndicatorsFromDB) != 1)
	 Err_ShowErrorAndExit ("Error when getting number of indicators.");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumIndicatorsFromDB;
  }

/*****************************************************************************/
/********************* Compute indicators of a course ************************/
/*****************************************************************************/
/* NumIndicatorsFromDB (number of indicators stored in database)
   must be retrieved before calling this function.
   If NumIndicatorsFromDB is different from number of indicators just computed
   ==> update it into database */

void Ind_ComputeAndStoreIndicatorsCrs (long CrsCod,int NumIndicatorsFromDB,
                                       struct Ind_IndicatorsCrs *IndicatorsCrs)
  {
   /***** Initialize number of indicators *****/
   IndicatorsCrs->NumIndicators = 0;

   /***** Get whether download zones are empty or not *****/
   IndicatorsCrs->NumFilesInDocumentZones = Brw_DB_GetNumFilesInDocumZonesOfCrs (CrsCod);
   IndicatorsCrs->NumFilesInSharedZones   = Brw_DB_GetNumFilesInShareZonesOfCrs (CrsCod);

   /***** Indicator #1: information about syllabus *****/
   IndicatorsCrs->SyllabusLecSrc   = Inf_GetInfoSrcFromDB (CrsCod,Inf_LECTURES);
   IndicatorsCrs->SyllabusPraSrc   = Inf_GetInfoSrcFromDB (CrsCod,Inf_PRACTICALS);
   IndicatorsCrs->TeachingGuideSrc = Inf_GetInfoSrcFromDB (CrsCod,Inf_TEACHING_GUIDE);
   IndicatorsCrs->ThereIsSyllabus = (IndicatorsCrs->SyllabusLecSrc   != Inf_NONE) ||
                                    (IndicatorsCrs->SyllabusPraSrc   != Inf_NONE) ||
                                    (IndicatorsCrs->TeachingGuideSrc != Inf_NONE);
   if (IndicatorsCrs->ThereIsSyllabus)
      IndicatorsCrs->NumIndicators++;

   /***** Indicator #2: information about assignments *****/
   IndicatorsCrs->NumAssignments      = Asg_DB_GetNumAssignmentsInCrs (CrsCod);
   IndicatorsCrs->NumFilesAssignments = Brw_DB_GetNumFilesInAssigZonesOfCrs (CrsCod);
   IndicatorsCrs->NumFilesWorks       = Brw_DB_GetNumFilesInWorksZonesOfCrs (CrsCod);
   IndicatorsCrs->ThereAreAssignments = (IndicatorsCrs->NumAssignments      != 0) ||
                                        (IndicatorsCrs->NumFilesAssignments != 0) ||
                                        (IndicatorsCrs->NumFilesWorks       != 0);
   if (IndicatorsCrs->ThereAreAssignments)
      IndicatorsCrs->NumIndicators++;

   /***** Indicator #3: information about online tutoring *****/
   IndicatorsCrs->NumThreads = For_DB_GetNumTotalThrsInForumsOfType (For_FORUM_COURSE_USRS,-1L,-1L,-1L,-1L,CrsCod);
   IndicatorsCrs->NumPosts   = For_DB_GetNumTotalPstsInForumsOfType (For_FORUM_COURSE_USRS,-1L,-1L,-1L,-1L,CrsCod,&(IndicatorsCrs->NumUsrsToBeNotifiedByEMail));
   IndicatorsCrs->NumMsgsSentByTchs = Msg_DB_GetNumMsgsSentByTchsCrs (CrsCod);
   IndicatorsCrs->ThereIsOnlineTutoring = (IndicatorsCrs->NumThreads        != 0) ||
	                                  (IndicatorsCrs->NumPosts          != 0) ||
	                                  (IndicatorsCrs->NumMsgsSentByTchs != 0);
   if (IndicatorsCrs->ThereIsOnlineTutoring)
      IndicatorsCrs->NumIndicators++;

   /***** Indicator #4: information about materials *****/
   IndicatorsCrs->ThereAreMaterials = (IndicatorsCrs->NumFilesInDocumentZones != 0) ||
                                      (IndicatorsCrs->NumFilesInSharedZones   != 0);
   if (IndicatorsCrs->ThereAreMaterials)
      IndicatorsCrs->NumIndicators++;

   /***** Indicator #5: information about assessment *****/
   IndicatorsCrs->AssessmentSrc = Inf_GetInfoSrcFromDB (CrsCod,Inf_ASSESSMENT);
   IndicatorsCrs->ThereIsAssessment = (IndicatorsCrs->AssessmentSrc    != Inf_NONE) ||
                                      (IndicatorsCrs->TeachingGuideSrc != Inf_NONE);
   if (IndicatorsCrs->ThereIsAssessment)
      IndicatorsCrs->NumIndicators++;

   /***** All the indicators are OK? *****/
   IndicatorsCrs->CoursePartiallyOK = IndicatorsCrs->NumIndicators >= 1 &&
	                              IndicatorsCrs->NumIndicators < Ind_NUM_INDICATORS;
   IndicatorsCrs->CourseAllOK       = IndicatorsCrs->NumIndicators == Ind_NUM_INDICATORS;

   /***** Update number of indicators into database
          if different to the stored one *****/
   if (NumIndicatorsFromDB != (int) IndicatorsCrs->NumIndicators)
      Ind_DB_StoreIndicatorsCrs (CrsCod,IndicatorsCrs->NumIndicators);
  }
