// swad_indicators.c: indicators of courses

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Ca�as Vargas

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
#include "swad_hierarchy_type.h"
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

#define Ind_NUM_LAYOUTS 2
typedef enum
  {
   Ind_INDICATORS_BRIEF,
   Ind_INDICATORS_FULL,
  } Ind_IndicatorsLayout_t;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ind_GetParsIndicators (struct Ind_Indicators *Indicators,
				   unsigned AllowedLvls);
static void Ind_GetParNumIndicators (struct Ind_Indicators *Indicators);
static Lay_Show_t Ind_GetIfShowBigList (struct Ind_Indicators *Indicators,
					unsigned NumCrss);
static void Ind_PutButtonToConfirmIWantToSeeBigList (struct Ind_Indicators *Indicators,
                                                     unsigned NumCrss);
static void Ind_PutParsConfirmIWantToSeeBigList (void *Indicators);

static void Ind_GetNumCoursesWithIndicators (unsigned NumCrssWithIndicatorYes[1 + Ind_NUM_INDICATORS],
                                             unsigned NumCrss,MYSQL_RES *mysql_res);
static void Ind_ShowNumCoursesWithIndicators (const struct Ind_Indicators *Indicators,
                                              unsigned NumCrssWithIndicatorYes[1 + Ind_NUM_INDICATORS],
                                              unsigned NumCrss,
                                              Frm_PutForm_t PutForm);
static void Ind_ShowTableOfCoursesWithIndicators (const struct Ind_Indicators *Indicators,
	                                          Ind_IndicatorsLayout_t IndicatorsLayout,
                                                  unsigned NumCrss,MYSQL_RES *mysql_res);
static unsigned Ind_GetAndUpdateNumIndicatorsCrs (long HieCod);

/*****************************************************************************/
/******************* Request showing statistics of courses *******************/
/*****************************************************************************/

void Ind_ReqIndicatorsCourses (void)
  {
   extern const char *Hlp_ANALYTICS_Indicators;
   extern const char *Par_CodeStr[Par_NUM_PAR_COD];
   extern const char *Txt_Scope;
   extern const char *Txt_Types_of_degree;
   extern const char *Txt_only_if_the_scope_is_X;
   extern const char *Txt_Department;
   extern const char *Txt_Any_department;
   extern const char *Txt_Number_of_indicators;
   extern const char *Txt_Indicators_of_courses;
   struct Ind_Indicators Indicators;
   unsigned AllowedLvls;
   char *SelectClass;
   MYSQL_RES *mysql_res;
   unsigned NumCrss;
   unsigned NumCrssWithIndicatorYes[1 + Ind_NUM_INDICATORS];
   unsigned NumCrssToList;
   unsigned Ind;

   /***** Get parameters *****/
   AllowedLvls = 1 << Hie_SYS |
		 1 << Hie_CTY |
		 1 << Hie_INS |
		 1 << Hie_CTR |
		 1 << Hie_DEG |
		 1 << Hie_CRS;
   Ind_GetParsIndicators (&Indicators,AllowedLvls);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Indicators_of_courses,NULL,NULL,
                 Hlp_ANALYTICS_Indicators,Box_NOT_CLOSABLE);

      /***** Form to update indicators *****/
      /* Begin form and table */
      Frm_BeginForm (ActReqStaCrs);
	 HTM_TABLE_BeginWidePadding (2);

	    /* Scope */
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("Frm_C1 RT","ScopeInd",Txt_Scope);

	       /* Data */
	       HTM_TD_Begin ("class=\"Frm_C2 LT\"");
		  Sco_PutSelectorScope ("ScopeInd",HTM_SUBMIT_ON_CHANGE,
					Indicators.HieLvl,AllowedLvls);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /* Compute stats for a type of degree */
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("Frm_C1 RT",Par_CodeStr[ParCod_OthDegTyp],Txt_Types_of_degree);

	       /* Data */
	       HTM_TD_Begin ("class=\"Frm_C2 LT DAT_%s\"",The_GetSuffix ());
		  DegTyp_WriteSelectorDegTypes (Indicators.DegTypCod);
		  HTM_SP ();
		  HTM_OpenParenthesis ();
		     HTM_TxtF (Txt_only_if_the_scope_is_X,Cfg_PLATFORM_SHORT_NAME);
		  HTM_CloseParenthesis ();
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /* Compute stats for courses with teachers belonging to any department or to a particular departament? */
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("Frm_C1 RT",Par_CodeStr[ParCod_Dpt],Txt_Department);

	       /* Data */
	       HTM_TD_Begin ("class=\"Frm_C2 LT\"");
		  if (asprintf (&SelectClass,"Frm_C2_INPUT INPUT_%s",
		                The_GetSuffix ()) < 0)
		     Err_NotEnoughMemoryExit ();
		  Dpt_WriteSelectorDepartment (Gbl.Hierarchy.Node[Hie_INS].HieCod,	// Departments in current insitution
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

	       Frm_LabelColumn ("Frm_C1 RT","",Txt_Number_of_indicators);

	       HTM_TD_Begin ("class=\"Frm_C2 LT\"");
		  Ind_ShowNumCoursesWithIndicators (&Indicators,
						    NumCrssWithIndicatorYes,NumCrss,
						    Frm_PUT_FORM);
	       HTM_TD_End ();

	    HTM_TR_End ();

	 /* End table and form */
	 HTM_TABLE_End ();
      Frm_EndForm ();

      /***** Show the stats of courses *****/
      for (Ind  = 0, NumCrssToList = 0;
	   Ind <= Ind_NUM_INDICATORS;
	   Ind++)
	 if (Indicators.Checked[Ind] == HTM_CHECKED)
	    NumCrssToList += NumCrssWithIndicatorYes[Ind];
      if (Ind_GetIfShowBigList (&Indicators,NumCrssToList) == Lay_SHOW)
	{
	 /* Show table */
	 Ind_ShowTableOfCoursesWithIndicators (&Indicators,Ind_INDICATORS_BRIEF,
					       NumCrss,mysql_res);

	 /* Button to show more details */
	 Frm_BeginForm (ActSeeAllStaCrs);
	    Sco_PutParScope ("ScopeInd",Indicators.HieLvl);
	    ParCod_PutPar (ParCod_OthDegTyp,Indicators.DegTypCod);
	    ParCod_PutPar (ParCod_Dpt      ,Indicators.DptCod   );
	    if (Indicators.StrChecked[0])
	       Par_PutParString (NULL,"Indicators",Indicators.StrChecked);
	    Btn_PutButton (Btn_SHOW_MORE_DETAILS,NULL);
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

static void Ind_GetParsIndicators (struct Ind_Indicators *Indicators,
				   unsigned AllowedLvls)
  {
   /***** Get scope *****/
   Indicators->HieLvl = Sco_GetScope ("ScopeInd",Hie_CRS,AllowedLvls);

   /***** Get degree type code *****/
   Indicators->DegTypCod = Indicators->HieLvl == Hie_SYS ?
	                      ParCod_GetPar (ParCod_OthDegTyp) :	// -1L (any degree type) is allowed here
                              -1L;

   /***** Get department code *****/
   Indicators->DptCod = ParCod_GetPar (ParCod_Dpt);			// -1L (any department) is allowed here

   /***** Get number of indicators *****/
   Ind_GetParNumIndicators (Indicators);
  }

/*****************************************************************************/
/*********************** Show statistics of courses **************************/
/*****************************************************************************/

void Ind_ShowIndicatorsCourses (void)
  {
   struct Ind_Indicators Indicators;
   unsigned AllowedLvls;
   MYSQL_RES *mysql_res;
   unsigned NumCrss;
   unsigned NumCrssWithIndicatorYes[1 + Ind_NUM_INDICATORS];

   /***** Get parameters *****/
   AllowedLvls = 1 << Hie_SYS |
		 1 << Hie_CTY |
		 1 << Hie_INS |
		 1 << Hie_CTR |
		 1 << Hie_DEG |
		 1 << Hie_CRS;
   Ind_GetParsIndicators (&Indicators,AllowedLvls);

   /***** Get courses from database *****/
   NumCrss = Ind_DB_GetTableOfCourses (&mysql_res,&Indicators);

   /***** Get vector with numbers of courses with 0, 1, 2... indicators set to yes *****/
   Ind_GetNumCoursesWithIndicators (NumCrssWithIndicatorYes,NumCrss,mysql_res);

   /***** Show table with numbers of courses with 0, 1, 2... indicators set to yes *****/
   Ind_ShowNumCoursesWithIndicators (&Indicators,
				     NumCrssWithIndicatorYes,NumCrss,
				     Frm_DONT_PUT_FORM);

   /***** Show the stats of courses *****/
   Ind_ShowTableOfCoursesWithIndicators (&Indicators,Ind_INDICATORS_FULL,
				         NumCrss,mysql_res);

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
   char LongStr[Cns_MAX_DIGITS_LONG + 1];
   long Indicator;

   /***** Get parameter multiple with list of indicators selected *****/
   Par_GetParMultiToText ("Indicators",Indicators->StrChecked,Ind_MAX_SIZE_INDICATORS_CHECKED);

   /***** Set which indicators have been selected (checkboxes on) *****/
   if (Indicators->StrChecked[0])
     {
      /* Reset all indicators */
      for (Ind = 0;
	   Ind <= Ind_NUM_INDICATORS;
	   Ind++)
	 Indicators->Checked[Ind] = HTM_NO_ATTR;

      /* Set indicators selected */
      for (Ptr = Indicators->StrChecked;
	   *Ptr;
	  )
	{
	 /* Get next indicator selected */
	 Par_GetNextStrUntilSeparParMult (&Ptr,LongStr,Cns_MAX_DIGITS_LONG);
	 Indicator = Str_ConvertStrCodToLongCod (LongStr);

	 /* Set each indicator in list StrIndicatorsSelected as selected */
	 for (Ind = 0;
	      Ind <= Ind_NUM_INDICATORS;
	      Ind++)
	    if ((long) Ind == Indicator)
	       Indicators->Checked[Ind] = HTM_CHECKED;
	}
     }
   else
      /* Set all indicators */
      for (Ind = 0;
	   Ind <= Ind_NUM_INDICATORS;
	   Ind++)
	 Indicators->Checked[Ind] = HTM_CHECKED;
  }

/*****************************************************************************/
/******* Show form to confirm that I want to see a big list of courses *******/
/*****************************************************************************/

static Lay_Show_t Ind_GetIfShowBigList (struct Ind_Indicators *Indicators,
					unsigned NumCrss)
  {
   Lay_Show_t ShowBigList;

   /***** If list of courses is too big... *****/
   if (NumCrss <= Cfg_MIN_NUM_COURSES_TO_CONFIRM_SHOW_BIG_LIST)
      return Lay_SHOW;	// List is not too big ==> show it

   /***** Get parameter with user's confirmation to see a big list of courses *****/
   if ((ShowBigList = Lay_GetParShow ("ShowBigList")) == Lay_DONT_SHOW)
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

   /***** Show alert and button to confirm that I want to see the big list *****/
   Ale_ShowAlertAndButton (Gbl.Action.Act,NULL,NULL,
                           Ind_PutParsConfirmIWantToSeeBigList,Indicators,
                           Btn_SHOW,
			   Ale_WARNING,Txt_The_list_of_X_courses_is_too_large_to_be_displayed,
                           NumCrss);
  }

static void Ind_PutParsConfirmIWantToSeeBigList (void *Indicators)
  {
   if (Indicators)
     {
      Sco_PutParScope ("ScopeInd",((struct Ind_Indicators *) Indicators)->HieLvl);
      ParCod_PutPar (ParCod_OthDegTyp,((struct Ind_Indicators *) Indicators)->DegTypCod);
      ParCod_PutPar (ParCod_Dpt      ,((struct Ind_Indicators *) Indicators)->DptCod   );
      if (((struct Ind_Indicators *) Indicators)->StrChecked[0])
	 Par_PutParString (NULL,"Indicators",((struct Ind_Indicators *) Indicators)->StrChecked);
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
   long HieCod;
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
      if ((HieCod = Str_ConvertStrCodToLongCod (row[2])) <= 0)
         Err_WrongCourseExit ();

      /* Get stored number of indicators of this course */
      NumIndicators = Ind_GetAndUpdateNumIndicatorsCrs (HieCod);
      NumCrssWithIndicatorYes[NumIndicators]++;
     }
  }

/*****************************************************************************/
/** Show table with numbers of courses with 0, 1, 2... indicators set to yes */
/*****************************************************************************/

static void Ind_ShowNumCoursesWithIndicators (const struct Ind_Indicators *Indicators,
                                              unsigned NumCrssWithIndicatorYes[1 + Ind_NUM_INDICATORS],
                                              unsigned NumCrss,
                                              Frm_PutForm_t PutForm)
  {
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];
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
	 if (PutForm == Frm_PUT_FORM)
	    HTM_TH_Empty (1);
	 HTM_TH_Empty (1);
	 HTM_TH_Span (Txt_HIERARCHY_PLURAL_Abc[Hie_CRS],HTM_HEAD_RIGHT,1,2,NULL);
      HTM_TR_End ();

      for (Ind  = 0;
	   Ind <= Ind_NUM_INDICATORS;
	   Ind++)
	{
	 Class = Indicators->Checked[Ind] == HTM_CHECKED ? ClassHighlight :
							   ClassNormal;
	 HTM_TR_Begin (NULL);

	    if (PutForm == Frm_PUT_FORM)
	      {
	       HTM_TD_Begin ("class=\"%s\"",Class);
		  HTM_INPUT_CHECKBOX ("Indicators",
				      Indicators->Checked[Ind] | HTM_SUBMIT_ON_CHANGE,
				      "id=\"Indicators%u\" value=\"%u\"",
				      Ind,Ind);
	       HTM_TD_End ();
	      }

	    HTM_TD_Begin ("class=\"%s\"",Class);
	       HTM_LABEL_Begin ("for=\"Indicators%u\"",Ind);
		  HTM_Unsigned (Ind); HTM_Colon ();
	       HTM_LABEL_End ();
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"%s\"",Class);
	       HTM_Unsigned (NumCrssWithIndicatorYes[Ind]);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"%s\"",Class);
	       HTM_OpenParenthesis ();
		  HTM_Double1Decimal (NumCrss ? (double) NumCrssWithIndicatorYes[Ind] * 100.0 /
						(double) NumCrss :
						0.0);
		  HTM_Percent ();
	       HTM_CloseParenthesis ();
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

      /***** Write total of courses *****/
      HTM_TR_Begin (NULL);

	 if (PutForm == Frm_PUT_FORM)
	    HTM_TD_Empty (1);

	 HTM_TD_LINE_TOP_Txt (Txt_Total);
	 HTM_TD_LINE_TOP_Unsigned (NumCrss);

	 HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP\"",The_GetSuffix ());
	    HTM_OpenParenthesis ();
	       HTM_Double1Decimal (100.0); HTM_Percent ();
	    HTM_CloseParenthesis ();
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
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
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
   MYSQL_ROW row;
   unsigned NumCrs;
   long HieCod;	// Course code
   unsigned NumTchs;
   unsigned NumStds;
   unsigned NumIndicators;
   struct Ind_IndicatorsCrs IndicatorsCrs;
   long ActCod;
   static const char *TableClass[Ind_NUM_LAYOUTS] =
     {
      [Ind_INDICATORS_BRIEF] = "Ind_TBL TBL_SCROLL",
      [Ind_INDICATORS_FULL ] = "Ind_TBL",
    };

   /***** Begin table *****/
   HTM_TABLE_Begin (TableClass[IndicatorsLayout]);

      /***** Write table heading *****/
      switch (IndicatorsLayout)
	{
	 case Ind_INDICATORS_BRIEF:
	    HTM_TR_Begin (NULL);
	       HTM_TH_Span (Txt_HIERARCHY_SINGUL_Abc[Hie_DEG],HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_HIERARCHY_SINGUL_Abc[Hie_CRS],HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_Institutional_BR_code 	     ,HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_Web_page_of_the_course	     ,HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_Indicators		     ,HTM_HEAD_CENTER,1,11,NULL);
	    HTM_TR_End ();

	    HTM_TR_Begin (NULL);
	       HTM_TH_Span (Txt_No_INDEX,HTM_HEAD_CENTER,2, 1,NULL);
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,2,NULL);
		  HTM_Txt ("(A) "); HTM_Txt (Txt_Syllabus_of_the_course);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,2,NULL);
		  HTM_Txt ("(B) "); HTM_Txt (Txt_Guided_academic_assignments);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,2,NULL);
		  HTM_Txt ("(C) "); HTM_Txt (Txt_Online_tutoring);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,2,NULL);
		  HTM_Txt ("(D) "); HTM_Txt (Txt_Materials);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,2,NULL);
		  HTM_Txt ("(E) "); HTM_Txt (Txt_Assessment_criteria);
	       HTM_TH_End ();
	    HTM_TR_End ();

	    HTM_TR_Begin (NULL);
	       HTM_TH (Txt_YES	,HTM_HEAD_CENTER);
	       HTM_TH (Txt_NO	,HTM_HEAD_CENTER);
	       HTM_TH (Txt_YES	,HTM_HEAD_CENTER);
	       HTM_TH (Txt_NO 	,HTM_HEAD_CENTER);
	       HTM_TH (Txt_YES	,HTM_HEAD_CENTER);
	       HTM_TH (Txt_NO	,HTM_HEAD_CENTER);
	       HTM_TH (Txt_YES	,HTM_HEAD_CENTER);
	       HTM_TH (Txt_NO	,HTM_HEAD_CENTER);
	       HTM_TH (Txt_YES	,HTM_HEAD_CENTER);
	       HTM_TH (Txt_NO	,HTM_HEAD_CENTER);
	    HTM_TR_End ();
	    break;
	 case Ind_INDICATORS_FULL:
	    HTM_TR_Begin (NULL);
	       HTM_TH_Span (Txt_HIERARCHY_SINGUL_Abc[Hie_DEG]  ,HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_HIERARCHY_SINGUL_Abc[Hie_CRS]  ,HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_Institutional_BR_code          ,HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_Web_page_of_the_course         ,HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH],HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD],HTM_HEAD_LEFT  ,3, 1,NULL);
	       HTM_TH_Span (Txt_Indicators                     ,HTM_HEAD_CENTER,1,24,NULL);
	    HTM_TR_End ();

	    HTM_TR_Begin (NULL);
	       HTM_TH_Span (Txt_No_INDEX,HTM_HEAD_CENTER,2,1,NULL);
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,5,NULL);
		  HTM_Txt ("(A) "); HTM_Txt (Txt_Syllabus_of_the_course);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,5,NULL);
		  HTM_Txt ("(B) "); HTM_Txt (Txt_Guided_academic_assignments);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,5,NULL);
		  HTM_Txt ("(C) "); HTM_Txt (Txt_Online_tutoring);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,4,NULL);
		  HTM_Txt ("(D) "); HTM_Txt (Txt_Materials);
	       HTM_TH_End ();
	       HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,4,NULL);
		  HTM_Txt ("(E) "); HTM_Txt (Txt_Assessment_criteria);
	       HTM_TH_End ();
	    HTM_TR_End ();

	    HTM_TR_Begin (NULL);
	       HTM_TH (Txt_YES					,HTM_HEAD_CENTER);
	       HTM_TH (Txt_NO					,HTM_HEAD_CENTER);
	       HTM_TH (Txt_INFO_TITLE[Inf_SYLLABUS_LEC]		,HTM_HEAD_LEFT  );
	       HTM_TH (Txt_INFO_TITLE[Inf_SYLLABUS_PRA]		,HTM_HEAD_LEFT  );
	       HTM_TH (Txt_INFO_TITLE[Inf_TEACH_GUIDE]		,HTM_HEAD_LEFT  );
	       HTM_TH (Txt_YES					,HTM_HEAD_CENTER);
	       HTM_TH (Txt_NO					,HTM_HEAD_CENTER);
	       HTM_TH (Txt_Assignments				,HTM_HEAD_RIGHT );
	       HTM_TH (Txt_Files_assignments			,HTM_HEAD_RIGHT );
	       HTM_TH (Txt_Files_works				,HTM_HEAD_RIGHT );
	       HTM_TH (Txt_YES					,HTM_HEAD_CENTER);
	       HTM_TH (Txt_NO					,HTM_HEAD_CENTER);
	       HTM_TH (Txt_Forum_threads			,HTM_HEAD_RIGHT );
	       HTM_TH (Txt_Forum_posts				,HTM_HEAD_RIGHT );
	       HTM_TH (Txt_Messages_sent_by_teachers		,HTM_HEAD_RIGHT );
	       HTM_TH (Txt_YES					,HTM_HEAD_CENTER);
	       HTM_TH (Txt_NO					,HTM_HEAD_CENTER);
	       HTM_TH (Txt_Number_of_files_in_DOCUM_zones	,HTM_HEAD_RIGHT );
	       HTM_TH (Txt_Number_of_files_in_SHARE_zones	,HTM_HEAD_RIGHT );
	       HTM_TH (Txt_YES					,HTM_HEAD_CENTER);
	       HTM_TH (Txt_NO					,HTM_HEAD_CENTER);
	       HTM_TH (Txt_INFO_TITLE[Inf_ASSESSMENT]		,HTM_HEAD_LEFT  );
	       HTM_TH (Txt_INFO_TITLE[Inf_TEACH_GUIDE]		,HTM_HEAD_LEFT  );
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
	 if ((HieCod = Str_ConvertStrCodToLongCod (row[2])) <= 0)
	    Err_WrongCourseExit ();

	 /* Get stored number of indicators of this course */
	 NumIndicators = Ind_GetAndUpdateNumIndicatorsCrs (HieCod);
	 if (Indicators->Checked[NumIndicators] == HTM_CHECKED)
	   {
	    /* Compute and store indicators */
	    Ind_ComputeAndStoreIndicatorsCrs (HieCod,(int) NumIndicators,&IndicatorsCrs);

	    /* The number of indicators may have changed */
	    if (Indicators->Checked[IndicatorsCrs.NumIndicators] == HTM_CHECKED)
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
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Txt (row[0]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.CourseAllOK ? "DAT_SMALL_GREEN" :
				      (IndicatorsCrs.CoursePartiallyOK ? "DAT_SMALL" :
									 "DAT_SMALL_RED"),
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Txt (row[1]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.CourseAllOK ? "DAT_SMALL_GREEN" :
				      (IndicatorsCrs.CoursePartiallyOK ? "DAT_SMALL" :
									 "DAT_SMALL_RED"),
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Txt (row[3]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM DAT_SMALL_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_A_Begin ("href=\"%s/?crs=%ld&amp;act=%ld\" target=\"_blank\"",
					Cfg_URL_SWAD_CGI,HieCod,ActCod);
			      HTM_TxtF ("%s/?crs=%ld&amp;act=%ld",
					Cfg_URL_SWAD_CGI,HieCod,ActCod);
			   HTM_A_End ();
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      IndicatorsCrs.CourseAllOK ? "DAT_SMALL_GREEN" :
				      (IndicatorsCrs.CoursePartiallyOK ? "DAT_SMALL" :
									 "DAT_SMALL_RED"),
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumIndicators);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
			              The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Syllabus == Exi_EXISTS)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Syllabus == Exi_DOES_NOT_EXIST)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Assignment == Exi_EXISTS)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Assignment == Exi_DOES_NOT_EXIST)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.OnlineTutoring == Exi_EXISTS)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.OnlineTutoring == Exi_DOES_NOT_EXIST)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Material == Exi_EXISTS)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Material == Exi_DOES_NOT_EXIST)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Assessment == Exi_EXISTS)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Assessment == Exi_DOES_NOT_EXIST)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

		     HTM_TR_End ();
		     break;
		  case Ind_INDICATORS_FULL:
		     /* Get number of users */
		     NumTchs = Enr_GetNumUsrsInCrss (Hie_CRS,HieCod,
						     1 << Rol_NET |	// Non-editing teachers
						     1 << Rol_TCH);	// Teachers
		     NumStds = Enr_GetNumUsrsInCrss (Hie_CRS,HieCod,
						     1 << Rol_STD);	// Students

		     HTM_TR_Begin (NULL);

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.CourseAllOK ? "DAT_SMALL_GREEN" :
				      (IndicatorsCrs.CoursePartiallyOK ? "DAT_SMALL" :
									 "DAT_SMALL_RED"),
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Txt (row[0]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.CourseAllOK ? "DAT_SMALL_GREEN" :
				      (IndicatorsCrs.CoursePartiallyOK ? "DAT_SMALL" :
									 "DAT_SMALL_RED"),
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Txt (row[1]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.CourseAllOK ? "DAT_SMALL_GREEN" :
				      (IndicatorsCrs.CoursePartiallyOK ? "DAT_SMALL" :
									 "DAT_SMALL_RED"),
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Txt (row[3]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM DAT_SMALL_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_A_Begin ("href=\"%s/?crs=%ld&amp;act=%ld\" target=\"_blank\"",
					Cfg_URL_SWAD_CGI,HieCod,ActCod);
			      HTM_TxtF ("%s/?crs=%ld&amp;act=%ld",
					Cfg_URL_SWAD_CGI,HieCod,ActCod);
			   HTM_A_End ();
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      NumTchs ? "DAT_SMALL_GREEN" :
						"DAT_SMALL_RED",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Unsigned (NumTchs);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      NumStds ? "DAT_SMALL_GREEN" :
						"DAT_SMALL_RED",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Unsigned (NumStds);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      IndicatorsCrs.CourseAllOK ? "DAT_SMALL_GREEN" :
				      (IndicatorsCrs.CoursePartiallyOK ? "DAT_SMALL" :
									 "DAT_SMALL_RED"),
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumIndicators);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Syllabus == Exi_EXISTS)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Syllabus == Exi_DOES_NOT_EXIST)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.SyllabusLecSrc != Inf_SRC_NONE ? "DAT_SMALL_GREEN" :
										     "DAT_SMALL_RED",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Txt (Txt_INFO_SRC_SHORT_TEXT[IndicatorsCrs.SyllabusLecSrc]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.SyllabusPraSrc != Inf_SRC_NONE ? "DAT_SMALL_GREEN" :
										     "DAT_SMALL_RED",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Txt (Txt_INFO_SRC_SHORT_TEXT[IndicatorsCrs.SyllabusPraSrc]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.TeachingGuideSrc != Inf_SRC_NONE ? "DAT_SMALL_GREEN" :
										       "DAT_SMALL_RED",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Txt (Txt_INFO_SRC_SHORT_TEXT[IndicatorsCrs.TeachingGuideSrc]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Assignment == Exi_EXISTS)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Assignment == Exi_DOES_NOT_EXIST)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      IndicatorsCrs.NumAssignments ? "DAT_SMALL_GREEN" :
								     "DAT_SMALL_RED",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumAssignments);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      IndicatorsCrs.NumFilesAssignments ? "DAT_SMALL_GREEN" :
									  "DAT_SMALL_RED",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumFilesAssignments);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      IndicatorsCrs.NumFilesWorks ? "DAT_SMALL_GREEN" :
								    "DAT_SMALL_RED",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumFilesWorks);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.OnlineTutoring == Exi_EXISTS)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.OnlineTutoring == Exi_DOES_NOT_EXIST)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s RM\"",
				      IndicatorsCrs.NumThreads ? "DAT_SMALL_GREEN" :
								 "DAT_SMALL_RED",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumThreads);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      IndicatorsCrs.NumPosts ? "DAT_SMALL_GREEN" :
							       "DAT_SMALL_RED",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumPosts);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      IndicatorsCrs.NumMsgsSentByTchs ? "DAT_SMALL_GREEN" :
									"DAT_SMALL_RED",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumMsgsSentByTchs);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Material == Exi_EXISTS)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Material == Exi_DOES_NOT_EXIST)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      IndicatorsCrs.NumFilesInDocumentZones ? "DAT_SMALL_GREEN" :
									      "DAT_SMALL_RED",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumFilesInDocumentZones);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"RM %s_%s %s\"",
				      IndicatorsCrs.NumFilesInSharedZones ? "DAT_SMALL_GREEN" :
									    "DAT_SMALL_RED",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Unsigned (IndicatorsCrs.NumFilesInSharedZones);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_GREEN_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Assessment == Exi_EXISTS)
			      HTM_Txt (Txt_YES);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"CM DAT_SMALL_RED_%s %s\"",
				      The_GetSuffix (),The_GetColorRows ());
			   if (IndicatorsCrs.Exist.Assessment == Exi_DOES_NOT_EXIST)
			      HTM_Txt (Txt_NO);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.AssessmentSrc != Inf_SRC_NONE ? "DAT_SMALL_GREEN" :
										    "DAT_SMALL_RED",
				      The_GetSuffix (),The_GetColorRows ());
			   HTM_Txt (Txt_INFO_SRC_SHORT_TEXT[IndicatorsCrs.AssessmentSrc]);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM %s_%s %s\"",
				      IndicatorsCrs.TeachingGuideSrc != Inf_SRC_NONE ? "DAT_SMALL_GREEN" :
										       "DAT_SMALL_RED",
				      The_GetSuffix (),The_GetColorRows ());
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

static unsigned Ind_GetAndUpdateNumIndicatorsCrs (long HieCod)
  {
   unsigned NumIndicators;
   struct Ind_IndicatorsCrs IndicatorsCrs;
   int NumIndicatorsFromDB = Ind_GetNumIndicatorsCrsFromDB (HieCod);

   /***** If number of indicators is not already computed ==> compute it! *****/
   if (NumIndicatorsFromDB >= 0)
      NumIndicators = (unsigned) NumIndicatorsFromDB;
   else	// Number of indicators is not already computed
     {
      /***** Compute and store number of indicators *****/
      Ind_ComputeAndStoreIndicatorsCrs (HieCod,NumIndicatorsFromDB,&IndicatorsCrs);
      NumIndicators = IndicatorsCrs.NumIndicators;
     }
   return NumIndicators;
  }

/*****************************************************************************/
/************ Get number of indicators of a course from database *************/
/*****************************************************************************/
// This function returns -1 if number of indicators is not yet calculated

int Ind_GetNumIndicatorsCrsFromDB (long HieCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   int NumIndicatorsFromDB = -1;	// -1 means not yet calculated

   /***** Get number of indicators of a course from database *****/
   if (Ind_DB_GetNumIndicatorsCrs (&mysql_res,HieCod) == Exi_EXISTS)
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

void Ind_ComputeAndStoreIndicatorsCrs (long HieCod,int NumIndicatorsFromDB,
                                       struct Ind_IndicatorsCrs *IndicatorsCrs)
  {
   long HieCods[Hie_NUM_LEVELS] =
     {
      [Hie_UNK] = -1L,
      [Hie_SYS] = -1L,
      [Hie_CTY] = -1L,
      [Hie_INS] = -1L,
      [Hie_CTR] = -1L,
      [Hie_DEG] = -1L,
      [Hie_CRS] = HieCod,
     };

   /***** Initialize number of indicators *****/
   IndicatorsCrs->NumIndicators = 0;

   /***** Get whether download zones are empty or not *****/
   IndicatorsCrs->NumFilesInDocumentZones = Brw_DB_GetNumFilesInDocumZonesOfCrs (HieCod);
   IndicatorsCrs->NumFilesInSharedZones   = Brw_DB_GetNumFilesInShareZonesOfCrs (HieCod);

   /***** Indicator #1: information about syllabus *****/
   IndicatorsCrs->SyllabusLecSrc   = Inf_GetInfoSrcFromDB (HieCod,Inf_SYLLABUS_LEC);
   IndicatorsCrs->SyllabusPraSrc   = Inf_GetInfoSrcFromDB (HieCod,Inf_SYLLABUS_PRA);
   IndicatorsCrs->TeachingGuideSrc = Inf_GetInfoSrcFromDB (HieCod,Inf_TEACH_GUIDE);
   IndicatorsCrs->Exist.Syllabus = IndicatorsCrs->SyllabusLecSrc   != Inf_SRC_NONE ||
                                   IndicatorsCrs->SyllabusPraSrc   != Inf_SRC_NONE ||
                                   IndicatorsCrs->TeachingGuideSrc != Inf_SRC_NONE ? Exi_EXISTS :
                                						     Exi_DOES_NOT_EXIST;
   if (IndicatorsCrs->Exist.Syllabus == Exi_EXISTS)
      IndicatorsCrs->NumIndicators++;

   /***** Indicator #2: information about assignments *****/
   IndicatorsCrs->NumAssignments      = Asg_DB_GetNumAssignmentsInCrs (HieCod);
   IndicatorsCrs->NumFilesAssignments = Brw_DB_GetNumFilesInAssigZonesOfCrs (HieCod);
   IndicatorsCrs->NumFilesWorks       = Brw_DB_GetNumFilesInWorksZonesOfCrs (HieCod);
   IndicatorsCrs->Exist.Assignment = IndicatorsCrs->NumAssignments ||
                                     IndicatorsCrs->NumFilesAssignments ||
                                     IndicatorsCrs->NumFilesWorks ? Exi_EXISTS :
                                				    Exi_DOES_NOT_EXIST;
   if (IndicatorsCrs->Exist.Assignment == Exi_EXISTS)
      IndicatorsCrs->NumIndicators++;

   /***** Indicator #3: information about online tutoring *****/
   IndicatorsCrs->NumThreads = For_DB_GetNumTotalThrsInForumsOfType (For_FORUM_COURSE_USRS,HieCods);
   IndicatorsCrs->NumPosts   = For_DB_GetNumTotalPstsInForumsOfType (For_FORUM_COURSE_USRS,HieCods,&(IndicatorsCrs->NumUsrsToBeNotifiedByEMail));
   IndicatorsCrs->NumMsgsSentByTchs = Msg_DB_GetNumMsgsSentByTchsCrs (HieCod);
   IndicatorsCrs->Exist.OnlineTutoring = IndicatorsCrs->NumThreads ||
	                                 IndicatorsCrs->NumPosts ||
	                                 IndicatorsCrs->NumMsgsSentByTchs ? Exi_EXISTS :
									    Exi_DOES_NOT_EXIST;
   if (IndicatorsCrs->Exist.OnlineTutoring == Exi_EXISTS)
      IndicatorsCrs->NumIndicators++;

   /***** Indicator #4: information about materials *****/
   IndicatorsCrs->Exist.Material = IndicatorsCrs->NumFilesInDocumentZones ||
                                   IndicatorsCrs->NumFilesInSharedZones ? Exi_EXISTS :
                                					  Exi_DOES_NOT_EXIST;
   if (IndicatorsCrs->Exist.Material == Exi_EXISTS)
      IndicatorsCrs->NumIndicators++;

   /***** Indicator #5: information about assessment *****/
   IndicatorsCrs->AssessmentSrc = Inf_GetInfoSrcFromDB (HieCod,Inf_ASSESSMENT);
   IndicatorsCrs->Exist.Assessment = IndicatorsCrs->AssessmentSrc    != Inf_SRC_NONE ||
                                     IndicatorsCrs->TeachingGuideSrc != Inf_SRC_NONE ? Exi_EXISTS :
                                						       Exi_DOES_NOT_EXIST;
   if (IndicatorsCrs->Exist.Assessment == Exi_EXISTS)
      IndicatorsCrs->NumIndicators++;

   /***** All the indicators are OK? *****/
   IndicatorsCrs->CoursePartiallyOK = IndicatorsCrs->NumIndicators >= 1 &&
	                              IndicatorsCrs->NumIndicators  < Ind_NUM_INDICATORS;
   IndicatorsCrs->CourseAllOK       = IndicatorsCrs->NumIndicators == Ind_NUM_INDICATORS;

   /***** Update number of indicators into database
          if different to the stored one *****/
   if (NumIndicatorsFromDB != (int) IndicatorsCrs->NumIndicators)
      Ind_DB_StoreIndicatorsCrs (HieCod,IndicatorsCrs->NumIndicators);
  }
