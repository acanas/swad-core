// swad_course.c: edition of courses

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_attendance.h"
#include "swad_box.h"
#include "swad_browser_database.h"
#include "swad_call_for_exam_database.h"
#include "swad_center_database.h"
#include "swad_country_database.h"
#include "swad_course.h"
#include "swad_course_config.h"
#include "swad_course_database.h"
#include "swad_database.h"
#include "swad_degree_database.h"
#include "swad_enrolment_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_figure_cache.h"
#include "swad_form.h"
#include "swad_forum_database.h"
#include "swad_game.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_info.h"
#include "swad_institution_database.h"
#include "swad_logo.h"
#include "swad_message.h"
#include "swad_notice.h"
#include "swad_notice_database.h"
#include "swad_notification_database.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_project.h"
#include "swad_record_database.h"
#include "swad_rubric.h"
#include "swad_search.h"
#include "swad_setting.h"
#include "swad_setting_database.h"
#include "swad_survey.h"
#include "swad_test.h"
#include "swad_test_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Private variables ******************************/
/*****************************************************************************/

static struct Hie_Node *Crs_EditingCrs = NULL;	// Static variable to keep the course being edited

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Crs_GetListCrssInCurrentDeg (void);
static void Crs_ListCourses (void);
static void Crs_PutIconsListCourses (__attribute__((unused)) void *Args);
static void Crs_PutIconToEditCourses (void);
static bool Crs_ListCoursesOfAYearForSeeing (unsigned Year);

static void Crs_EditCoursesInternal (void);
static void Crs_PutIconsEditingCourses (__attribute__((unused)) void *Args);
static void Crs_ListCoursesForEdition (void);
static void Crs_ListCoursesOfAYearForEdition (unsigned Year);
static Usr_Can_t Crs_CheckIfICanEdit (struct Hie_Node *Crs);
static void Crs_PutFormToCreateCourse (void);
static void Crs_PutHeadCoursesForSeeing (void);
static void Crs_PutHeadCoursesForEdition (void);
static void Crs_ReceiveRequestOrCreateCrs (Hie_Status_t Status);
static void Crs_GetParsNewCourse (struct Hie_Node *Crs);

static void Crs_GetCourseDataFromRow (MYSQL_RES *mysql_res,
				      struct Hie_Node *Crs);

static void Crs_EmptyCourseCompletely (long HieCod);

static void Crs_PutButtonToGoToCrs (void);
static void Crs_PutButtonToRegisterInCrs (void);

static void Crs_WriteRowCrsData (unsigned NumCrs,MYSQL_ROW row,bool WriteColumnAccepted);

static void Crs_EditingCourseConstructor (void);
static void Crs_EditingCourseDestructor (void);

/*****************************************************************************/
/***************** Show introduction to the current course *******************/
/*****************************************************************************/

void Crs_ShowIntroduction (void)
  {
   /***** Course configuration *****/
   HTM_DIV_Begin ("class=\"CM\"");
      CrsCfg_Configuration (Vie_VIEW);
   HTM_DIV_End ();

   /***** Course introduction *****/
   Inf_ShowInfo ();

   /***** Show help to enrol me *****/
   Hlp_ShowHelpWhatWouldYouLikeToDo ();
  }

/*****************************************************************************/
/********************* Get number of courses with users **********************/
/*****************************************************************************/

unsigned Crs_GetCachedNumCrssWithUsrs (Rol_Role_t Role)
  {
   static const FigCch_FigureCached_t FigureCrss[Rol_NUM_ROLES] =
     {
      [Rol_STD] = FigCch_NUM_CRSS_WITH_STDS,	// Students
      [Rol_NET] = FigCch_NUM_CRSS_WITH_NETS,	// Non-editing teachers
      [Rol_TCH] = FigCch_NUM_CRSS_WITH_TCHS,	// Teachers
     };
   unsigned NumNodesWithUsrs;
   long HieCod = Hie_GetCurrentCod ();

   /***** Get number of courses with users from cache *****/
   if (!FigCch_GetFigureFromCache (FigureCrss[Role],Gbl.Scope.Current,HieCod,
				   FigCch_UNSIGNED,&NumNodesWithUsrs))
     {
      /***** Get current number of courses with users from database and update cache *****/
      NumNodesWithUsrs = Crs_DB_GetNumCrssWithUsrs (Role,Gbl.Scope.Current,HieCod);
      FigCch_UpdateFigureIntoCache (FigureCrss[Role],Gbl.Scope.Current,HieCod,
				    FigCch_UNSIGNED,&NumNodesWithUsrs);
     }

   return NumNodesWithUsrs;
  }

/*****************************************************************************/
/*************************** Write selector of course ************************/
/*****************************************************************************/

void Crs_WriteSelectorOfCourse (void)
  {
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   long CrsCod;
   HTM_Disabled_t Disabled = (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0) ? HTM_ENABLED :
									HTM_DISABLED;
   HTM_SubmitOnChange_t SubmitOnChange = (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0) ? HTM_SUBMIT_ON_CHANGE :
										    HTM_DONT_SUBMIT_ON_CHANGE;

   /***** Begin form *****/
   Frm_BeginFormGoTo (ActSeeCrsInf);

      /***** Begin selector of course *****/
      HTM_SELECT_Begin (Disabled,HTM_NOT_REQUIRED,SubmitOnChange,NULL,
			"id=\"crs\" name=\"crs\" class=\"HIE_SEL INPUT_%s\"",
			The_GetSuffix ());

	 /***** Initial disabled option *****/
	 HTM_OPTION (HTM_Type_STRING,"",
		     Gbl.Hierarchy.Node[Hie_CRS].HieCod < 0 ? HTM_OPTION_SELECTED :
							      HTM_OPTION_UNSELECTED,
		     HTM_DISABLED,
		     "[%s]",Txt_HIERARCHY_SINGUL_Abc[Hie_CRS]);

	 if (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0)
	   {
	    /***** Get courses belonging to the current degree from database *****/
	    NumCrss = Crs_DB_GetCrssInCurrentDegBasic (&mysql_res);
	    for (NumCrs = 0;
		 NumCrs < NumCrss;
		 NumCrs++)
	      {
	       /* Get next course */
	       row = mysql_fetch_row (mysql_res);

	       /* Get course code (row[0]) */
	       if ((CrsCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
		  Err_WrongCourseExit ();

	       /* Write option */
	       HTM_OPTION (HTM_Type_LONG,&CrsCod,
			   Gbl.Hierarchy.Level == Hie_CRS &&	// Course selected
			   CrsCod == Gbl.Hierarchy.Node[Hie_CRS].HieCod ? HTM_OPTION_SELECTED :
									  HTM_OPTION_UNSELECTED,
			   HTM_ENABLED,
			   "%s",row[1]);	// Short name (row[1])
	      }

	    /***** Free structure that stores the query result *****/
	    DB_FreeMySQLResult (&mysql_res);
	   }

      /***** End selector of course *****/
      HTM_SELECT_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************** Show courses of a degree *************************/
/*****************************************************************************/

void Crs_ShowCrssOfCurrentDeg (void)
  {
   /***** Trivial check *****/
   if (Gbl.Hierarchy.Node[Hie_DEG].HieCod <= 0)	// No degree selected
      return;

   /***** Get list of courses in this degree *****/
   Crs_GetListCrssInCurrentDeg ();

   /***** Write menu to select country, institution, center and degree *****/
   Hie_WriteMenuHierarchy ();

   /***** Show list of courses *****/
   Crs_ListCourses ();

   /***** Free list of courses in this degree *****/
   Hie_FreeList (Hie_DEG);
  }

/*****************************************************************************/
/*************** Create a list with courses in current degree ****************/
/*****************************************************************************/

static void Crs_GetListCrssInCurrentDeg (void)
  {
   MYSQL_RES *mysql_res;
   unsigned NumCrss;
   unsigned NumCrs;

   /***** Get courses of a degree from database *****/
   if ((NumCrss = Crs_DB_GetCrssInCurrentDegFull (&mysql_res))) // Courses found...
     {
      /***** Create list with courses in degree *****/
      if ((Gbl.Hierarchy.List[Hie_DEG].Lst = calloc (NumCrss,
	                                                sizeof (*Gbl.Hierarchy.List[Hie_DEG].Lst))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the courses in degree *****/
      for (NumCrs = 0;
	   NumCrs < NumCrss;
	   NumCrs++)
         /* Get data of next course */
         Crs_GetCourseDataFromRow (mysql_res,&Gbl.Hierarchy.List[Hie_DEG].Lst[NumCrs]);
     }

   Gbl.Hierarchy.List[Hie_DEG].Num = NumCrss;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Write selector of my coursess ************************/
/*****************************************************************************/

void Crs_WriteSelectorMyCoursesInBreadcrumb (void)
  {
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   unsigned NumMyCrs;
   long CrsCod;
   long DegCod;
   long LastDegCod;
   char CrsShortName[Nam_MAX_BYTES_SHRT_NAME + 1];
   char DegShortName[Nam_MAX_BYTES_SHRT_NAME + 1];

   /***** Fill the list with the courses I belong to, if not filled *****/
   if (Gbl.Usrs.Me.Logged)
      Hie_GetMyHierarchy (Hie_CRS);

   /***** Begin form *****/
   Frm_BeginFormGoTo (Gbl.Usrs.Me.Hierarchy[Hie_CRS].Num ? ActSeePrg :
							   ActReqSch);

      /***** Begin selector of courses *****/
      HTM_SELECT_Begin (HTM_ENABLED,HTM_NOT_REQUIRED,HTM_SUBMIT_ON_CHANGE,NULL,
			"id=\"my_courses\" name=\"crs\" class=\"INPUT_%s\"",
			The_GetSuffix ());

	 /***** Write an option when no course selected *****/
	 if (Gbl.Hierarchy.Node[Hie_CRS].HieCod <= 0)	// No course selected
	    HTM_OPTION (HTM_Type_STRING,"-1",
	                HTM_OPTION_SELECTED,
	                HTM_DISABLED,
			"%s",Txt_HIERARCHY_SINGUL_Abc[Hie_CRS]);

	 if (Gbl.Usrs.Me.Hierarchy[Hie_CRS].Num)
	   {
	    /***** Write an option for each of my courses *****/
	    for (NumMyCrs = 0, LastDegCod = -1L;
		 NumMyCrs < Gbl.Usrs.Me.Hierarchy[Hie_CRS].Num;
		 NumMyCrs++)
	      {
	       CrsCod = Gbl.Usrs.Me.Hierarchy[Hie_CRS].Nodes[NumMyCrs].HieCod;
	       DegCod = Gbl.Usrs.Me.Hierarchy[Hie_CRS].Nodes[NumMyCrs].PrtCod;

	       Crs_DB_GetShortNamesByCod (CrsCod,CrsShortName,DegShortName);

	       if (DegCod != LastDegCod)
		 {
		  if (LastDegCod > 0)
		     HTM_OPTGROUP_End ();
		  HTM_OPTGROUP_Begin (DegShortName);
		  LastDegCod = DegCod;
		 }

	       HTM_OPTION (HTM_Type_LONG,&CrsCod,
			   CrsCod == Gbl.Hierarchy.Node[Hie_CRS].HieCod ? HTM_OPTION_SELECTED :
									  HTM_OPTION_UNSELECTED,
			   HTM_ENABLED,
			   "%s",CrsShortName);
	      }

	    if (LastDegCod > 0)
	       HTM_OPTGROUP_End ();
	   }

	 /***** Write an option with the current course
		when I don't belong to it *****/
	 if (Gbl.Hierarchy.Level == Hie_CRS &&					// Course selected
	     Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS] == Usr_DONT_BELONG)	// I do not belong to it
	    HTM_OPTION (HTM_Type_LONG,&Gbl.Hierarchy.Node[Hie_CRS].HieCod,
	                HTM_OPTION_SELECTED,
	                HTM_DISABLED,
			"%s",Gbl.Hierarchy.Node[Hie_CRS].ShrtName);

      /***** End selector of courses *****/
      HTM_SELECT_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************* List courses in this degree ***********************/
/*****************************************************************************/

static void Crs_ListCourses (void)
  {
   extern const char *Hlp_DEGREE_Courses;
   extern const char *Txt_Courses_of_DEGREE_X;
   extern const char *Txt_No_courses;
   char *Title;
   unsigned Year;

   /***** Begin box *****/
   if (asprintf (&Title,Txt_Courses_of_DEGREE_X,Gbl.Hierarchy.Node[Hie_DEG].ShrtName) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin (Title,Crs_PutIconsListCourses,NULL,
                 Hlp_DEGREE_Courses,Box_NOT_CLOSABLE);
   free (Title);

      if (Gbl.Hierarchy.List[Hie_DEG].Num)	// There are courses in the current degree
	{
	 /***** Begin table *****/
	 HTM_TABLE_Begin ("TBL_SCROLL");

	    /***** Heading *****/
	    Crs_PutHeadCoursesForSeeing ();

	    /***** List the courses *****/
	    for (Year  = 1;
		 Year <= Deg_MAX_YEARS_PER_DEGREE;
		 Year++)
	       if (Crs_ListCoursesOfAYearForSeeing (Year))	// If this year has courses ==>
		  The_ChangeRowColor ();	// ==> change color for the next year
	    Crs_ListCoursesOfAYearForSeeing (0);		// Courses without a year selected

	 /***** End table *****/
	 HTM_TABLE_End ();
	}
      else	// No courses created in the current degree
	 Ale_ShowAlert (Ale_INFO,Txt_No_courses);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/***************** Put contextual icons in list of courses *******************/
/*****************************************************************************/

static void Crs_PutIconsListCourses (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit courses *****/
   if (Hie_CheckIfICanEdit () == Usr_CAN)
      Crs_PutIconToEditCourses ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
  }

/*****************************************************************************/
/************************* Put icon to edit courses **************************/
/*****************************************************************************/

static void Crs_PutIconToEditCourses (void)
  {
   Ico_PutContextualIconToEdit (ActEdiCrs,NULL,NULL,NULL);
  }

/*****************************************************************************/
/********************* List courses of a year for seeing *********************/
/*****************************************************************************/
// Return true if this year has courses

static bool Crs_ListCoursesOfAYearForSeeing (unsigned Year)
  {
   extern const char *Txt_COURSE_With_users;
   extern const char *Txt_COURSE_Without_users;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_COURSE_STATUS[Hie_NUM_STATUS_TXT];
   unsigned NumCrs;
   struct Hie_Node *Crs;
   const char *TxtClassNormal;
   const char *TxtClassStrong;
   const char *BgColor;
   bool ThisYearHasCourses = false;
   unsigned NumUsrs[Rol_NUM_ROLES];

   /***** Write all courses of this year *****/
   for (NumCrs = 0;
	NumCrs < Gbl.Hierarchy.List[Hie_DEG].Num;
	NumCrs++)
     {
      Crs = &(Gbl.Hierarchy.List[Hie_DEG].Lst[NumCrs]);
      if (Crs->Specific.Year == Year)	// The year of the course is this?
	{
	 ThisYearHasCourses = true;
	 if (Crs->Status & Hie_STATUS_BIT_PENDING)
	   {
	    TxtClassNormal =
	    TxtClassStrong = "DAT_LIGHT";
	   }
	 else
	   {
	    TxtClassNormal = "DAT";
	    TxtClassStrong = "DAT_STRONG";
	   }

	 /* Check if this course is one of my courses */
	 BgColor = (Hie_CheckIfIBelongTo (Hie_CRS,Crs->HieCod) == Usr_BELONG) ? "BG_HIGHLIGHT" :
										  The_GetColorRows ();

	 HTM_TR_Begin (NULL);

	    /* Get number of users */
	    NumUsrs[Rol_STD] = Enr_GetCachedNumUsrsInCrss (Hie_CRS,Crs->HieCod,1 << Rol_STD);
	    NumUsrs[Rol_NET] = Enr_GetCachedNumUsrsInCrss (Hie_CRS,Crs->HieCod,1 << Rol_NET);
	    NumUsrs[Rol_TCH] = Enr_GetCachedNumUsrsInCrss (Hie_CRS,Crs->HieCod,1 << Rol_TCH);
	    NumUsrs[Rol_UNK] = NumUsrs[Rol_STD] +
			       NumUsrs[Rol_NET] +
			       NumUsrs[Rol_TCH];

	    /* Put green tip if course has users */
	    HTM_TD_Begin ("class=\"CT %s_%s %s\" title=\"%s\"",
			  TxtClassNormal,The_GetSuffix (),BgColor,
			  NumUsrs[Rol_UNK] ? Txt_COURSE_With_users :
					     Txt_COURSE_Without_users);
	       HTM_Txt (NumUsrs[Rol_UNK] ? "&check;" :
					   "&nbsp;");
	    HTM_TD_End ();

	    /* Course year */
	    HTM_TD_Begin ("class=\"CT %s_%s %s\"",
	                  TxtClassNormal,The_GetSuffix (),BgColor);
	       HTM_Txt (Txt_YEAR_OF_DEGREE[Crs->Specific.Year]);
	    HTM_TD_End ();

	    /* Institutional code of the course */
	    HTM_TD_Begin ("class=\"CT %s_%s %s\"",
	                  TxtClassNormal,The_GetSuffix (),BgColor);
	       HTM_Txt (Crs->InstitutionalCod);
	    HTM_TD_End ();

	    /* Course full name */
	    HTM_TD_Begin ("class=\"LT %s_%s %s\"",
	                  TxtClassStrong,The_GetSuffix (),BgColor);
	       Frm_BeginFormGoTo (ActSeeCrsInf);
		  ParCod_PutPar (ParCod_Crs,Crs->HieCod);
		  HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Crs->FullName),
					   "class=\"BT_LINK LT\"");
		  Str_FreeGoToTitle ();
		     HTM_Txt (Crs->FullName);
		  HTM_BUTTON_End ();
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Number of teachers in this course */
	    HTM_TD_Begin ("class=\"RT %s_%s %s\"",
	                  TxtClassNormal,The_GetSuffix (),BgColor);
	       HTM_Unsigned (NumUsrs[Rol_TCH] +
			     NumUsrs[Rol_NET]);
	    HTM_TD_End ();

	    /* Number of students in this course */
	    HTM_TD_Begin ("class=\"RT %s_%s %s\"",
	                  TxtClassNormal,The_GetSuffix (),BgColor);
	       HTM_Unsigned (NumUsrs[Rol_STD]);
	    HTM_TD_End ();

	    /* Course status */
            Hie_WriteStatusCell (Crs->Status,TxtClassNormal,BgColor,Txt_COURSE_STATUS);

	 HTM_TR_End ();
	}
     }

   return ThisYearHasCourses;
  }

/*****************************************************************************/
/****************** Put forms to edit courses in this degree *****************/
/*****************************************************************************/

void Crs_EditCourses (void)
  {
   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Edit courses *****/
   Crs_EditCoursesInternal ();

   /***** Course destructor *****/
   Crs_EditingCourseDestructor ();
  }

static void Crs_EditCoursesInternal (void)
  {
   extern const char *Hlp_DEGREE_Courses;
   extern const char *Txt_Courses_of_DEGREE_X;
   char *Title;

   /***** Get list of degrees in this center *****/
   Deg_GetListDegsInCurrentCtr ();

   /***** Get list of courses in this degree *****/
   Crs_GetListCrssInCurrentDeg ();

   /***** Write menu to select country, institution, center and degree *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   if (asprintf (&Title,Txt_Courses_of_DEGREE_X,
		 Gbl.Hierarchy.Node[Hie_DEG].ShrtName) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin (Title,Crs_PutIconsEditingCourses,NULL,
                 Hlp_DEGREE_Courses,Box_NOT_CLOSABLE);
   free (Title);

      /***** Put a form to create or request a new course *****/
      Crs_PutFormToCreateCourse ();

      /***** Forms to edit current courses *****/
      if (Gbl.Hierarchy.List[Hie_DEG].Num)
	 Crs_ListCoursesForEdition ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of courses in this degree *****/
   Hie_FreeList (Hie_DEG);

   /***** Free list of degrees in this center *****/
   Hie_FreeList (Hie_CTR);
  }

/*****************************************************************************/
/**************** Put contextual icons in edition of courses *****************/
/*****************************************************************************/

static void Crs_PutIconsEditingCourses (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view courses *****/
   Ico_PutContextualIconToView (ActSeeCrs,NULL,NULL,NULL);

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
  }

/*****************************************************************************/
/********************* List current courses for edition **********************/
/*****************************************************************************/

static void Crs_ListCoursesForEdition (void)
  {
   unsigned Year;

   /***** Begin table *****/
   HTM_TABLE_Begin ("TBL_SCROLL");

      /***** Write heading *****/
      Crs_PutHeadCoursesForEdition ();

      /***** List the courses *****/
      for (Year = 1;
	   Year <= Deg_MAX_YEARS_PER_DEGREE;
	   Year++)
	 Crs_ListCoursesOfAYearForEdition (Year);
      Crs_ListCoursesOfAYearForEdition (0);

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************** List courses of a year for edition *********************/
/*****************************************************************************/

static void Crs_ListCoursesOfAYearForEdition (unsigned Year)
  {
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_COURSE_STATUS[Hie_NUM_STATUS_TXT];
   static Act_Action_t ActionRename[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = ActRenCrsSho,
      [Nam_FULL_NAME] = ActRenCrsFul,
     };
   struct Hie_Node *Crs;
   unsigned YearAux;
   unsigned NumCrs;
   struct Usr_Data UsrDat;
   Usr_Can_t ICanEdit;
   unsigned NumUsrs[Rol_NUM_ROLES];
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** List courses of a given year *****/
   for (NumCrs = 0;
	NumCrs < Gbl.Hierarchy.List[Hie_DEG].Num;
	NumCrs++)
     {
      Crs = &(Gbl.Hierarchy.List[Hie_DEG].Lst[NumCrs]);

      if (Crs->Specific.Year == Year)
	{
	 ICanEdit = Crs_CheckIfICanEdit (Crs);

	 /* Get number of users */
	 NumUsrs[Rol_STD] = Enr_GetNumUsrsInCrss (Hie_CRS,Crs->HieCod,1 << Rol_STD);
	 NumUsrs[Rol_NET] = Enr_GetNumUsrsInCrss (Hie_CRS,Crs->HieCod,1 << Rol_NET);
	 NumUsrs[Rol_TCH] = Enr_GetNumUsrsInCrss (Hie_CRS,Crs->HieCod,1 << Rol_TCH);
	 NumUsrs[Rol_UNK] = NumUsrs[Rol_STD] +
	                    NumUsrs[Rol_NET] +
			    NumUsrs[Rol_TCH];

	 HTM_TR_Begin (NULL);

	    /* Put icon to remove course */
	    HTM_TD_Begin ("class=\"BT\"");
	       if (NumUsrs[Rol_UNK] ||	// Course has users ==> deletion forbidden
		   ICanEdit == Usr_CAN_NOT)
		  Ico_PutIconRemovalNotAllowed ();
	       else	// Crs->NumUsrs == 0 && ICanEdit == Usr_CAN
		  Ico_PutContextualIconToRemove (ActRemCrs,NULL,
						 Hie_PutParOtherHieCod,&Crs->HieCod);
	    HTM_TD_End ();

	    /* Course code */
	    HTM_TD_Begin ("class=\"CODE DAT_%s\"",The_GetSuffix ());
	       HTM_Long (Crs->HieCod);
	    HTM_TD_End ();

	    /* Course year */
	    HTM_TD_Begin ("class=\"CT DAT_%s\"",The_GetSuffix ());
	       switch (ICanEdit)
		 {
		  case Usr_CAN:
		     Frm_BeginForm (ActChgCrsYea);
			ParCod_PutPar (ParCod_OthHie,Crs->HieCod);
			HTM_SELECT_Begin (HTM_ENABLED,HTM_NOT_REQUIRED,
					  HTM_SUBMIT_ON_CHANGE,NULL,
					  "name=\"OthCrsYear\""
					  " class=\"HIE_SEL_NARROW INPUT_%s\"",
					  The_GetSuffix ());
			   for (YearAux  = 0;
				YearAux <= Deg_MAX_YEARS_PER_DEGREE;
				YearAux++)	// All the years are permitted
					      // because it's possible to move this course
					      // to another degree (with other active years)
			      HTM_OPTION (HTM_Type_UNSIGNED,&YearAux,
					  YearAux == Crs->Specific.Year ? HTM_OPTION_SELECTED :
									  HTM_OPTION_UNSELECTED,
					  HTM_ENABLED,
					  "%s",Txt_YEAR_OF_DEGREE[YearAux]);
			HTM_SELECT_End ();
		     Frm_EndForm ();
		     break;
		  case Usr_CAN_NOT:
		  default:
		     HTM_Txt (Txt_YEAR_OF_DEGREE[Crs->Specific.Year]);
		     break;
		 }
	    HTM_TD_End ();

	    /* Institutional code of the course */
	    HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
	       switch (ICanEdit)
		 {
		  case Usr_CAN:
		     Frm_BeginForm (ActChgInsCrsCod);
			ParCod_PutPar (ParCod_OthHie,Crs->HieCod);
			HTM_INPUT_TEXT ("InsCrsCod",Hie_MAX_CHARS_INSTITUTIONAL_COD,
					Crs->InstitutionalCod,
					HTM_ENABLED,HTM_NOT_REQUIRED,HTM_SUBMIT_ON_CHANGE,
					"class=\"INPUT_INS_CODE INPUT_%s\"",
					The_GetSuffix ());
		     Frm_EndForm ();
		     break;
		  case Usr_CAN_NOT:
		  default:
		     HTM_Txt (Crs->InstitutionalCod);
		     break;
		 }
	    HTM_TD_End ();

	    /* Course short name and full name */
	    Names[Nam_SHRT_NAME] = Crs->ShrtName;
	    Names[Nam_FULL_NAME] = Crs->FullName;
	    Nam_ExistingShortAndFullNames (ActionRename,
				           ParCod_OthHie,Crs->HieCod,
				           Names,
				           ICanEdit == Usr_CAN ? Frm_PUT_FORM :
				        			   Frm_DONT_PUT_FORM);

	    /* Current number of teachers in this course */
	    HTM_TD_Unsigned (NumUsrs[Rol_TCH] +
			     NumUsrs[Rol_NET]);

	    /* Current number of students in this course */
	    HTM_TD_Unsigned (NumUsrs[Rol_STD]);

	    /* Course requester */
	    UsrDat.UsrCod = Crs->RequesterUsrCod;
	    Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
						     Usr_DONT_GET_PREFS,
						     Usr_DONT_GET_ROLE_IN_CRS);
	    HTM_TD_Begin ("class=\"LT DAT_%s INPUT_REQUESTER\"",
	                  The_GetSuffix ());
	       Usr_WriteAuthor (&UsrDat,For_ENABLED);
	    HTM_TD_End ();

	    /* Course status */
	    Hie_WriteStatusCellEditable (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM ? Usr_CAN :
										  Usr_CAN_NOT,
	                                 Crs->Status,ActChgCrsSta,Crs->HieCod,
	                                 Txt_COURSE_STATUS);

	 HTM_TR_End ();
	}
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************** Check if I can edit, remove, etc. a course *******************/
/*****************************************************************************/

static Usr_Can_t Crs_CheckIfICanEdit (struct Hie_Node *Crs)
  {
   return (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM ||		// I am a degree administrator or higher
           ((Crs->Status & Hie_STATUS_BIT_PENDING) != 0 &&	// Course is not yet activated
           Gbl.Usrs.Me.UsrDat.UsrCod == Crs->RequesterUsrCod)) ? Usr_CAN :	// I am the requester
        						         Usr_CAN_NOT;
  }

/*****************************************************************************/
/*********************** Put a form to create a new course *******************/
/*****************************************************************************/

static void Crs_PutFormToCreateCourse (void)
  {
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   Act_Action_t NextAction = ActUnk;
   unsigned Year;
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Begin form *****/
   if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
      NextAction = ActNewCrs;
   else if (Gbl.Usrs.Me.Role.Max >= Rol_GST)
      NextAction = ActReqCrs;
   else
      Err_NoPermissionExit ();

   /***** Begin form to create *****/
   Frm_BeginFormTable (NextAction,NULL,NULL,NULL,"TBL_SCROLL");

      /***** Write heading *****/
      Crs_PutHeadCoursesForEdition ();

      HTM_TR_Begin (NULL);

	 /***** Column to remove course, disabled here *****/
	 HTM_TD_Begin ("class=\"BM\"");
	 HTM_TD_End ();

	 /***** Course code *****/
	 HTM_TD_Begin ("class=\"CODE\"");
	 HTM_TD_End ();

	 /***** Year *****/
	 HTM_TD_Begin ("class=\"CM\"");
	    HTM_SELECT_Begin (HTM_ENABLED,HTM_NOT_REQUIRED,
			      HTM_DONT_SUBMIT_ON_CHANGE,NULL,
			      "name=\"OthCrsYear\""
			      " class=\"HIE_SEL_NARROW INPUT_%s\"",
			      The_GetSuffix ());
	       for (Year = 0;
		    Year <= Deg_MAX_YEARS_PER_DEGREE;
		    Year++)
		  HTM_OPTION (HTM_Type_UNSIGNED,&Year,
			      Year == Crs_EditingCrs->Specific.Year ? HTM_OPTION_SELECTED :
								      HTM_OPTION_UNSELECTED,
			      HTM_ENABLED,
			      "%s",Txt_YEAR_OF_DEGREE[Year]);
	    HTM_SELECT_End ();
	 HTM_TD_End ();

	 /***** Institutional code of the course *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("InsCrsCod",Hie_MAX_CHARS_INSTITUTIONAL_COD,
			    Crs_EditingCrs->InstitutionalCod,
			    HTM_ENABLED,HTM_NOT_REQUIRED,HTM_DONT_SUBMIT_ON_CHANGE,
			    "class=\"INPUT_INS_CODE INPUT_%s\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Course short name and full name *****/
	 Names[Nam_SHRT_NAME] = Crs_EditingCrs->ShrtName;
	 Names[Nam_FULL_NAME] = Crs_EditingCrs->FullName;
	 Nam_NewShortAndFullNames (Names);

	 /***** Current number of teachers and students in this course *****/
	 HTM_TD_Unsigned (0);
	 HTM_TD_Unsigned (0);

	 /***** Course requester *****/
	 HTM_TD_Begin ("class=\"LT DAT_%s INPUT_REQUESTER\"",The_GetSuffix ());
	    Usr_WriteAuthor (&Gbl.Usrs.Me.UsrDat,For_ENABLED);
	 HTM_TD_End ();

	 /***** Course status *****/
	 HTM_TD_Empty (1);

      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE_BUTTON);
  }

/*****************************************************************************/
/******************** Write header with fields of a course *******************/
/*****************************************************************************/

static void Crs_PutHeadCoursesForSeeing (void)
  {
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_Institutional_BR_code;
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];

   HTM_TR_Begin (NULL);

      HTM_TH_Span (NULL                               ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH      (Txt_Year_OF_A_DEGREE               ,HTM_HEAD_CENTER);
      HTM_TH      (Txt_Institutional_BR_code          ,HTM_HEAD_CENTER);
      HTM_TH      (Txt_HIERARCHY_SINGUL_Abc[Hie_CRS]         ,HTM_HEAD_LEFT  );
      HTM_TH      (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH],HTM_HEAD_RIGHT );
      HTM_TH      (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD],HTM_HEAD_RIGHT );
      HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Write header with fields of a course *******************/
/*****************************************************************************/

static void Crs_PutHeadCoursesForEdition (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_optional;
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_Institutional_code;
   extern const char *Txt_Short_name_of_the_course;
   extern const char *Txt_Full_name_of_the_course;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_Requester;

   HTM_TR_Begin (NULL);

      HTM_TH_Span (NULL                               ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH      (Txt_Code                           ,HTM_HEAD_RIGHT );
      HTM_TH      (Txt_Year_OF_A_DEGREE               ,HTM_HEAD_CENTER);
      HTM_TH_Begin (HTM_HEAD_LEFT);
	 HTM_TxtF ("%s&nbsp;(%s)",Txt_Institutional_code,Txt_optional);
      HTM_TH_End ();
      HTM_TH      (Txt_Short_name_of_the_course       ,HTM_HEAD_LEFT  );
      HTM_TH      (Txt_Full_name_of_the_course        ,HTM_HEAD_LEFT  );
      HTM_TH      (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH],HTM_HEAD_RIGHT );
      HTM_TH      (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD],HTM_HEAD_RIGHT );
      HTM_TH      (Txt_Requester                      ,HTM_HEAD_LEFT  );
      HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/****************** Receive form to request a new course *********************/
/*****************************************************************************/

void Crs_ReceiveReqCrs (void)
  {
   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Receive form to request a new course *****/
   Crs_ReceiveRequestOrCreateCrs ((Hie_Status_t) Hie_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/******************* Receive form to create a new course *********************/
/*****************************************************************************/

void Crs_ReceiveNewCrs (void)
  {
   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Receive form to create a new course *****/
   Crs_ReceiveRequestOrCreateCrs ((Hie_Status_t) 0);
  }

/*****************************************************************************/
/************* Receive form to request or create a new course ****************/
/*****************************************************************************/

static void Crs_ReceiveRequestOrCreateCrs (Hie_Status_t Status)
  {
   extern const char *Txt_Created_new_course_X;
   extern const char *Txt_The_year_X_is_not_allowed;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Get parameters from form *****/
   /* Set course degree */
   Crs_EditingCrs->PrtCod = Gbl.Hierarchy.Node[Hie_DEG].HieCod;

   /* Get parameters of the new course */
   Crs_GetParsNewCourse (Crs_EditingCrs);

   /***** Check if year is correct *****/
   if (Crs_EditingCrs->Specific.Year <= Deg_MAX_YEARS_PER_DEGREE)	// If year is valid
     {
      if (Crs_EditingCrs->ShrtName[0] &&
	  Crs_EditingCrs->FullName[0])	// If there's a course name
	{
	 /***** If name of course was not in database... *****/
	 Names[Nam_SHRT_NAME] = Crs_EditingCrs->ShrtName;
	 Names[Nam_FULL_NAME] = Crs_EditingCrs->FullName;
	 if (!Nam_CheckIfNameExists (Crs_DB_CheckIfCrsNameExistsInYearOfDeg,
				     Names,
				     -1L,
				     Crs_EditingCrs->PrtCod,
				     Crs_EditingCrs->Specific.Year))
	   {
	    Crs_DB_CreateCourse (Crs_EditingCrs,Status);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,Txt_Created_new_course_X,
			     Names[Nam_FULL_NAME]);
	   }
	}
      else	// If there is not a course name
         Ale_CreateAlertYouMustSpecifyShrtNameAndFullName ();
     }
   else	// Year not valid
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_The_year_X_is_not_allowed,
                       Crs_EditingCrs->Specific.Year);
  }

/*****************************************************************************/
/************** Get the parameters of a new course from form *****************/
/*****************************************************************************/

static void Crs_GetParsNewCourse (struct Hie_Node *Crs)
  {
   char YearStr[2 + 1];
   char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Get parameters of the course from form *****/
   /* Get year */
   Par_GetParText ("OthCrsYear",YearStr,2);
   Crs->Specific.Year = Deg_ConvStrToYear (YearStr);

   /* Get institutional code */
   Par_GetParText ("InsCrsCod",Crs->InstitutionalCod,Hie_MAX_BYTES_INSTITUTIONAL_COD);

   /* Get course short name and full name */
   Names[Nam_SHRT_NAME] = Crs->ShrtName;
   Names[Nam_FULL_NAME] = Crs->FullName;
   Nam_GetParsShrtAndFullName (Names);
  }

/*****************************************************************************/
/****************************** Remove a course ******************************/
/*****************************************************************************/

void Crs_RemoveCourse (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_To_remove_a_course_you_must_first_remove_all_users_in_the_course;
   extern const char *Txt_Course_X_removed;

   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Get course code *****/
   Crs_EditingCrs->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /***** Get data of the course from database *****/
   Hie_GetDataByCod[Hie_CRS] (Crs_EditingCrs);
   if (Crs_CheckIfICanEdit (Crs_EditingCrs) == Usr_CAN_NOT)
      Err_NoPermissionExit ();

   /***** Check if this course has users *****/
   if (Enr_GetNumUsrsInCrss (Hie_CRS,Crs_EditingCrs->HieCod,
			     1 << Rol_STD |
			     1 << Rol_NET |
			     1 << Rol_TCH))	// Course has users ==> don't remove
      Ale_ShowAlert (Ale_WARNING,
		     Txt_To_remove_a_course_you_must_first_remove_all_users_in_the_course);
   else					// Course has no users ==> remove it
     {
      /***** Remove course *****/
      Crs_RemoveCourseCompletely (Crs_EditingCrs->HieCod);

      /***** Write message to show the change made *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_Course_X_removed,
		     Crs_EditingCrs->FullName);

      Crs_EditingCrs->HieCod = -1L;	// To not showing button to go to course
     }
  }

/*****************************************************************************/
/********************* Get data of a course from its code ********************/
/*****************************************************************************/

bool Crs_GetCourseDataByCod (struct Hie_Node *Node)
  {
   MYSQL_RES *mysql_res;
   bool Found = false;

   /***** Clear data *****/
   Node->PrtCod 	 = -1L;
   Node->Specific.Year   = 0;
   Node->Status          = (Hie_Status_t) 0;
   Node->RequesterUsrCod = -1L;
   Node->ShrtName[0]     = '\0';
   Node->FullName[0]     = '\0';

   /***** Check if course code is correct *****/
   if (Node->HieCod > 0)
     {
      /***** Get data of a course from database *****/
      if (Crs_DB_GetCourseDataByCod (&mysql_res,Node->HieCod)) // Course found...
	{
	 /***** Get data of the course *****/
	 Crs_GetCourseDataFromRow (mysql_res,Node);

         /* Set return value */
	 Found = true;
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return Found;
  }

/*****************************************************************************/
/********** Get data of a course from a row resulting of a query *************/
/*****************************************************************************/

static void Crs_GetCourseDataFromRow (MYSQL_RES *mysql_res,
				      struct Hie_Node *Crs)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get course code (row[0]) *****/
   if ((Crs->HieCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongCourseExit ();

   /***** Get code of degree (row[1]) *****/
   Crs->PrtCod = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get year (row[2]) *****/
   Crs->Specific.Year = Deg_ConvStrToYear (row[2]);

   /***** Get course status (row[4]) *****/
   if (sscanf (row[4],"%u",&(Crs->Status)) != 1)
      Err_WrongStatusExit ();

   /***** Get requester user'code (row[5]) *****/
   Crs->RequesterUsrCod = Str_ConvertStrCodToLongCod (row[5]);

   /***** Get institutional course code (row[3]) *****/
   Str_Copy (Crs->InstitutionalCod,row[3],sizeof (Crs->InstitutionalCod) - 1);

   /***** Get short name (row[6]) and full name (row[7]) of the course *****/
   Str_Copy (Crs->ShrtName,row[6],sizeof (Crs->ShrtName) - 1);
   Str_Copy (Crs->FullName,row[7],sizeof (Crs->FullName) - 1);
  }

/*****************************************************************************/
/****************************** Remove a course ******************************/
/*****************************************************************************/

void Crs_RemoveCourseCompletely (long HieCod)
  {
   if (HieCod > 0)
     {
      /***** Empty course *****/
      Crs_EmptyCourseCompletely (HieCod);

      /***** Remove course from table of last accesses to courses in database *****/
      Crs_DB_RemoveCrsLast (HieCod);

      /***** Remove course from table of courses in database *****/
      Crs_DB_RemoveCrs (HieCod);
     }
  }

/*****************************************************************************/
/********** Empty a course (remove all its information and users) ************/
/*****************************************************************************/
// Start removing less important things to more important things;
// so, in case of failure, important things can been removed in the future

static void Crs_EmptyCourseCompletely (long HieCod)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   struct Hie_Node Crs;
   char PathRelCrs[PATH_MAX + 1];

   if (HieCod > 0)
     {
      /***** Get course data *****/
      Crs.HieCod = HieCod;
      Hie_GetDataByCod[Hie_CRS] (&Crs);

      /***** Remove all students in the course *****/
      Enr_RemAllStdsInCrs (&Crs);

      /***** Set all notifications from the course as removed,
	     except notifications about new messages *****/
      Ntf_DB_MarkNotifInCrsAsRemoved (-1L,HieCod);

      /***** Remove information of the course ****/
      /* Remove information of the course */
      Crs_DB_RemoveCrsInfo (HieCod);

      /* Remove timetable of the course */
      Crs_DB_RemoveCrsTimetable (HieCod);

      /***** Remove exam announcements in the course *****/
      /* Mark all exam announcements in the course as deleted */
      Cfe_DB_MarkCallForExamsInCrsAsDeleted (HieCod);

      /***** Remove course cards of the course *****/
      /* Remove content of course cards */
      Rec_DB_RemoveAllFieldContentsInCrs (HieCod);

      /* Remove definition of fields in course cards */
      Rec_DB_RemoveAllFieldsInCrs (HieCod);

      /***** Remove information related to files in course,
             including groups and projects,
             so this function must be called
             before removing groups and projects *****/
      Brw_DB_RemoveCrsFiles (HieCod);

      /***** Assessment tab *****/
      /* Remove assignments of the course */
      Asg_RemoveCrsAssignments (HieCod);

      /* Remove projects of the course */
      Prj_RemoveCrsProjects (HieCod);

      /* Remove tests of the course */
      TstPrn_RemoveCrsPrints (HieCod);
      Tst_DB_RemoveTstConfig (HieCod);

      /* Remove all exams in the course */
      Exa_RemoveCrsExams (HieCod);

      /* Remove all games in the course */
      Gam_RemoveCrsGames (HieCod);

      /* Remove all questions in the course */
      Qst_RemoveCrsQsts (HieCod);

      /* Remove all rubrics in the course */
      Rub_RemoveCrsRubrics (HieCod);

      /***** Remove attendance events of the course *****/
      Att_RemoveCrsEvents (HieCod);

      /***** Remove notices in the course *****/
      Not_DB_RemoveCrsNotices (HieCod);

      /***** Remove all threads and posts in forums of the course *****/
      For_DB_RemoveForums (Hie_CRS,HieCod);

      /***** Remove all surveys in the course *****/
      Svy_RemoveSurveys (Hie_CRS,HieCod);

      /***** Remove groups in the course *****/
      Grp_DB_RemoveCrsGrps (HieCod);

      /***** Remove users' requests for inscription in the course *****/
      Enr_DB_RemCrsRequests (HieCod);

      /***** Remove possible users remaining in the course (teachers) *****/
      Set_DB_RemAllUsrsFromCrsSettings (HieCod);
      Enr_DB_RemAllUsrsFromCrs (HieCod);

      /***** Remove directories of the course *****/
      snprintf (PathRelCrs,sizeof (PathRelCrs),"%s/%ld",
	        Cfg_PATH_CRS_PRIVATE,HieCod);
      Fil_RemoveTree (PathRelCrs);
      snprintf (PathRelCrs,sizeof (PathRelCrs),"%s/%ld",
	        Cfg_PATH_CRS_PUBLIC,HieCod);
      Fil_RemoveTree (PathRelCrs);
     }
  }

/*****************************************************************************/
/************** Change the institutional code of a course ********************/
/*****************************************************************************/

void Crs_ChangeInsCrsCod (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_The_institutional_code_of_the_course_X_has_changed_to_Y;
   extern const char *Txt_The_institutional_code_of_the_course_X_has_not_changed;
   char NewInstitutionalCrsCod[Hie_MAX_BYTES_INSTITUTIONAL_COD + 1];

   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Get parameters from form *****/
   /* Get course code */
   Crs_EditingCrs->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /* Get institutional code */
   Par_GetParText ("InsCrsCod",NewInstitutionalCrsCod,Hie_MAX_BYTES_INSTITUTIONAL_COD);

   /* Get data of the course */
   Hie_GetDataByCod[Hie_CRS] (Crs_EditingCrs);
   if (Crs_CheckIfICanEdit (Crs_EditingCrs) == Usr_CAN_NOT)
      Err_NoPermissionExit ();

   /***** Change the institutional course code *****/
   if (strcmp (NewInstitutionalCrsCod,Crs_EditingCrs->InstitutionalCod))
     {
      Crs_UpdateInstitutionalCrsCod (Crs_EditingCrs,NewInstitutionalCrsCod);
      Ale_CreateAlert (Ale_SUCCESS,NULL,
		       Txt_The_institutional_code_of_the_course_X_has_changed_to_Y,
		       Crs_EditingCrs->ShrtName,
		       NewInstitutionalCrsCod);
     }
   else	// The same institutional code
      Ale_CreateAlert (Ale_INFO,NULL,
		       Txt_The_institutional_code_of_the_course_X_has_not_changed,
		       Crs_EditingCrs->ShrtName);
  }

/*****************************************************************************/
/************************ Change the year of a course ************************/
/*****************************************************************************/

void Crs_ChangeCrsYear (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_The_year_of_the_course_X_has_changed;
   extern const char *Txt_The_year_X_is_not_allowed;
   char YearStr[2 + 1];
   unsigned NewYear;
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Get parameters from form *****/
   /* Get course code */
   Crs_EditingCrs->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /* Get parameter with year */
   Par_GetParText ("OthCrsYear",YearStr,2);
   NewYear = Deg_ConvStrToYear (YearStr);

   /* Get data of the course */
   Hie_GetDataByCod[Hie_CRS] (Crs_EditingCrs);
   if (Crs_CheckIfICanEdit (Crs_EditingCrs) == Usr_CAN_NOT)
      Err_NoPermissionExit ();

   if (NewYear <= Deg_MAX_YEARS_PER_DEGREE)	// If year is valid
     {
      /***** If name of course was not in database in the new year... *****/
      Names[Nam_SHRT_NAME] = Crs_EditingCrs->ShrtName;
      Names[Nam_FULL_NAME] = Crs_EditingCrs->FullName;
      if (!Nam_CheckIfNameExists (Crs_DB_CheckIfCrsNameExistsInYearOfDeg,
				  Names,
				  -1L,
				  Crs_EditingCrs->PrtCod,
				  NewYear))
	{
	 /***** Update year in table of courses *****/
	 Crs_UpdateCrsYear (Crs_EditingCrs,NewYear);

	 /***** Create message to show the change made *****/
	 Ale_CreateAlert (Ale_SUCCESS,NULL,Txt_The_year_of_the_course_X_has_changed,
			  Names[Nam_FULL_NAME]);
	}
     }
   else	// Year not valid
      Ale_CreateAlert (Ale_WARNING,NULL,
		       Txt_The_year_X_is_not_allowed,
		       NewYear);
  }

/*****************************************************************************/
/************* Change the institutional course code of a course **************/
/*****************************************************************************/

void Crs_UpdateInstitutionalCrsCod (struct Hie_Node *Crs,
                                    const char *NewInstitutionalCrsCod)
  {
   /***** Update institutional course code in table of courses *****/
   Crs_DB_UpdateInstitutionalCrsCod (Crs->HieCod,NewInstitutionalCrsCod);

   /***** Copy institutional course code *****/
   Str_Copy (Crs->InstitutionalCod,NewInstitutionalCrsCod,
             sizeof (Crs->InstitutionalCod) - 1);
  }

/*****************************************************************************/
/****************** Change the year/semester of a course *********************/
/*****************************************************************************/

void Crs_UpdateCrsYear (struct Hie_Node *Crs,unsigned NewYear)
  {
   /***** Update year/semester in table of courses *****/
   Crs_DB_UpdateCrsYear (Crs->HieCod,NewYear);

   /***** Copy course year/semester *****/
   Crs->Specific.Year = NewYear;
  }

/*****************************************************************************/
/************************ Change the name of a course ************************/
/*****************************************************************************/

void Crs_RenameCourseShrt (void)
  {
   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Rename course *****/
   Crs_EditingCrs->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);
   Crs_RenameCourse (Crs_EditingCrs,Nam_SHRT_NAME);
  }

void Crs_RenameCourseFull (void)
  {
   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Rename course *****/
   Crs_EditingCrs->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);
   Crs_RenameCourse (Crs_EditingCrs,Nam_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a course ************************/
/*****************************************************************************/

void Crs_RenameCourse (struct Hie_Node *Crs,Nam_ShrtOrFullName_t ShrtOrFull)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Nam_Fields[Nam_NUM_SHRT_FULL_NAMES];
   extern unsigned Nam_MaxBytes[Nam_NUM_SHRT_FULL_NAMES];
   extern const char *Txt_X_already_exists;
   extern const char *Txt_The_course_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   char *CurrentName[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = Crs->ShrtName,
      [Nam_FULL_NAME] = Crs->FullName,
     };
   char NewName[Nam_MAX_BYTES_FULL_NAME + 1];

   /***** Get parameters from form *****/
   /* Get the new name for the course */
   Nam_GetParShrtOrFullName (ShrtOrFull,NewName);

   /***** Get from the database the data of the degree *****/
   Hie_GetDataByCod[Hie_CRS] (Crs);
   if (Crs_CheckIfICanEdit (Crs) == Usr_CAN_NOT)
      Err_NoPermissionExit ();

   /***** Check if new name is empty *****/
   if (NewName[0])
     {
      /***** Check if old and new names are the same
	     (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentName[ShrtOrFull],NewName))	// Different names
	{
	 /***** If course was in database... *****/
	 if (Crs_DB_CheckIfCrsNameExistsInYearOfDeg (Nam_Fields[ShrtOrFull],
						     NewName,Crs->HieCod,
						     Crs->PrtCod,Crs->Specific.Year))
	    Ale_CreateAlert (Ale_WARNING,NULL,Txt_X_already_exists,NewName);
	 else
	   {
	    /* Update the table changing old name by new name */
	    Crs_DB_UpdateCrsName (Crs->HieCod,
				  Nam_Fields[ShrtOrFull],NewName);

	    /* Create alert to show the change made */
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
			     Txt_The_course_X_has_been_renamed_as_Y,
			     CurrentName[ShrtOrFull],NewName);

	    /* Change current course name in order to display it properly */
	    Str_Copy (CurrentName[ShrtOrFull],NewName,
		      Nam_MaxBytes[ShrtOrFull]);
	   }
	}
      else	// The same name
	 Ale_CreateAlert (Ale_INFO,NULL,
			  Txt_The_name_X_has_not_changed,CurrentName[ShrtOrFull]);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
  }

/*****************************************************************************/
/*********************** Change the status of a course ***********************/
/*****************************************************************************/

void Crs_ChangeCrsStatus (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_The_status_of_the_course_X_has_changed;
   Hie_Status_t Status;

   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Get parameters from form *****/
   /* Get course code */
   Crs_EditingCrs->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /* Get parameter with status */
   Status = Hie_GetParStatus ();	// New status

   /***** Get data of course *****/
   Hie_GetDataByCod[Hie_CRS] (Crs_EditingCrs);

   /***** Update status *****/
   Crs_DB_UpdateCrsStatus (Crs_EditingCrs->HieCod,Status);
   Crs_EditingCrs->Status = Status;

   /***** Create alert to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_status_of_the_course_X_has_changed,
                    Crs_EditingCrs->ShrtName);
  }

/*****************************************************************************/
/********* Show alerts after changing a course and continue editing **********/
/*****************************************************************************/

void Crs_ContEditAfterChgCrs (void)
  {
   bool PutButtonToRequestRegistration;

   if (Ale_GetTypeOfLastAlert () == Ale_SUCCESS)
     {
      /***** Begin alert *****/
      Ale_ShowLastAlertAndButtonBegin ();

      /***** Put button to go to course changed *****/
      Crs_PutButtonToGoToCrs ();

      /***** Put button to request my registration in course *****/
      PutButtonToRequestRegistration = false;
      switch (Gbl.Usrs.Me.Role.Logged)
        {
	 case Rol_GST:	// I do not belong to any course
	    PutButtonToRequestRegistration = true;
	    break;
	 case Rol_USR:
	    PutButtonToRequestRegistration = (Hie_CheckIfUsrBelongsTo (Hie_CRS,
								       Gbl.Usrs.Me.UsrDat.UsrCod,
					                               Crs_EditingCrs->HieCod,
					                               false) == Usr_DONT_BELONG);
            break;
	 case Rol_STD:
	 case Rol_NET:
	 case Rol_TCH:
	    if (Crs_EditingCrs->HieCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)
	       PutButtonToRequestRegistration = (Hie_CheckIfUsrBelongsTo (Hie_CRS,
									  Gbl.Usrs.Me.UsrDat.UsrCod,
									  Crs_EditingCrs->HieCod,
									  false) == Usr_DONT_BELONG);
	    break;
	 default:
	    break;

        }
      if (PutButtonToRequestRegistration)
	 Crs_PutButtonToRegisterInCrs ();

      /***** End alert *****/
      Ale_ShowAlertAndButtonEnd (ActUnk,NULL,NULL,
                               NULL,NULL,
                               Btn_NO_BUTTON,NULL);
     }

   /***** Show possible delayed alerts *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Crs_EditCoursesInternal ();

   /***** Course destructor *****/
   Crs_EditingCourseDestructor ();
  }

/*****************************************************************************/
/************************ Put button to go to course *************************/
/*****************************************************************************/

static void Crs_PutButtonToGoToCrs (void)
  {
   // If the course being edited is different to the current one...
   if (Crs_EditingCrs->HieCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)
     {
      Frm_BeginForm (ActSeeCrsInf);
	 ParCod_PutPar (ParCod_Crs,Crs_EditingCrs->HieCod);
	 Btn_PutConfirmButton (Str_BuildGoToTitle (Crs_EditingCrs->ShrtName));
	 Str_FreeGoToTitle ();
      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/************************ Put button to go to course *************************/
/*****************************************************************************/

static void Crs_PutButtonToRegisterInCrs (void)
  {
   extern const char *Txt_Register_me_in_X;
   char *TxtButton;

   Frm_BeginForm (ActReqSignUp);
      // If the course being edited is different to the current one...
      if (Crs_EditingCrs->HieCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)
	 ParCod_PutPar (ParCod_Crs,Crs_EditingCrs->HieCod);

      if (asprintf (&TxtButton,Txt_Register_me_in_X,Crs_EditingCrs->ShrtName) < 0)
	 Err_NotEnoughMemoryExit ();
      Btn_PutCreateButton (TxtButton);
      free (TxtButton);

   Frm_EndForm ();
  }

/*****************************************************************************/
/************************** Write courses of a user **************************/
/*****************************************************************************/

void Crs_GetAndWriteCrssOfAUsr (const struct Usr_Data *UsrDat,Rol_Role_t Role)
  {
   extern const char *Txt_USER_in_COURSE;
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   char *Txt;

   /***** Get courses of a user from database *****/
   NumCrss = Crs_DB_GetCrssOfAUsr (&mysql_res,UsrDat->UsrCod,Role);

   /***** List the courses (one row per course) *****/
   if (NumCrss)
     {
      /* Begin box and table */
      Box_BoxTableBegin (NULL,NULL,NULL,NULL,Box_NOT_CLOSABLE,2);

	 /* Heading row */
	 HTM_TR_Begin (NULL);

	    HTM_TH_Span_Begin (HTM_HEAD_LEFT,1,7,NULL);
	       if (asprintf (&Txt,Txt_USER_in_COURSE,
				  Role == Rol_UNK ? Txt_User[Usr_SEX_UNKNOWN] : // Role == Rol_UNK ==> any role
						    Txt_ROLES_SINGUL_Abc[Role][UsrDat->Sex]) < 0)
		  Err_NotEnoughMemoryExit ();
	       HTM_TxtColon (Txt);
	       free (Txt);
	    HTM_TH_End ();

	 HTM_TR_End ();

	 HTM_TR_Begin (NULL);

            HTM_TH_Span (NULL                               ,HTM_HEAD_CENTER,1,1,"BT");
            HTM_TH_Span (NULL                               ,HTM_HEAD_CENTER,1,1,"BT");
	    HTM_TH      (Txt_HIERARCHY_SINGUL_Abc[Hie_DEG]         ,HTM_HEAD_LEFT  );
	    HTM_TH      (Txt_Year_OF_A_DEGREE               ,HTM_HEAD_CENTER);
	    HTM_TH      (Txt_HIERARCHY_SINGUL_Abc[Hie_CRS]         ,HTM_HEAD_LEFT  );
	    HTM_TH      (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH],HTM_HEAD_RIGHT );
	    HTM_TH      (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NET],HTM_HEAD_RIGHT );
	    HTM_TH      (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD],HTM_HEAD_RIGHT );

	 HTM_TR_End ();

	 /* Write courses */
	 for (NumCrs  = 1;
	      NumCrs <= NumCrss;
	      NumCrs++)
	   {
	    /* Get next course */
	    row = mysql_fetch_row (mysql_res);

	    /* Write data of this course */
	    Crs_WriteRowCrsData (NumCrs,row,true);
	   }

      /* End table and box */
      Box_BoxTableEnd ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************************** List courses found *****************************/
/*****************************************************************************/

void Crs_ListCrssFound (MYSQL_RES **mysql_res,unsigned NumCrss)
  {
   extern const char *Txt_HIERARCHY_SINGUL_abc[Hie_NUM_LEVELS];
   extern const char *Txt_HIERARCHY_PLURAL_abc[Hie_NUM_LEVELS];
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   char *Title;
   MYSQL_ROW row;
   unsigned NumCrs;

   /***** List the courses (one row per course) *****/
   if (NumCrss)
     {
      /***** Begin box and table *****/
      /* Number of courses found */
      if (asprintf (&Title,"%u %s",NumCrss,
				   NumCrss == 1 ? Txt_HIERARCHY_SINGUL_abc[Hie_CRS] :
						  Txt_HIERARCHY_PLURAL_abc[Hie_CRS]) < 0)
	 Err_NotEnoughMemoryExit ();
      Box_BoxTableBegin (Title,NULL,NULL,NULL,Box_NOT_CLOSABLE,2);
      free (Title);

	 /***** Heading row *****/
	 HTM_TR_Begin (NULL);

            HTM_TH_Span (NULL                          ,HTM_HEAD_CENTER,1,1,"BT");
	    HTM_TH (Txt_HIERARCHY_SINGUL_Abc[Hie_DEG]         ,HTM_HEAD_LEFT  );
	    HTM_TH (Txt_Year_OF_A_DEGREE               ,HTM_HEAD_CENTER);
	    HTM_TH (Txt_HIERARCHY_SINGUL_Abc[Hie_CRS]         ,HTM_HEAD_LEFT  );
	    HTM_TH (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH],HTM_HEAD_RIGHT );
	    HTM_TH (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NET],HTM_HEAD_RIGHT );
	    HTM_TH (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD],HTM_HEAD_RIGHT );

	 HTM_TR_End ();

	 /***** Write courses *****/
	 for (NumCrs = 1;
	      NumCrs <= NumCrss;
	      NumCrs++)
	   {
	    /* Get next course */
	    row = mysql_fetch_row (*mysql_res);

	    /* Write data of this course */
	    Crs_WriteRowCrsData (NumCrs,row,false);
	   }

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/************** Write the data of a course (result of a query) ***************/
/*****************************************************************************/

static void Crs_WriteRowCrsData (unsigned NumCrs,MYSQL_ROW row,bool WriteColumnAccepted)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_Enrolment_confirmed;
   extern const char *Txt_Enrolment_not_confirmed;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   struct Hie_Node Deg;
   long CrsCod;
   unsigned NumStds;
   unsigned NumNETs;
   unsigned NumTchs;
   unsigned NumUsrs;
   const char *ClassTxt;
   const char *BgColor;
   bool Accepted;
   /*
   row[0]: deg_degrees.DegCod
   row[1]: crs_courses.CrsCod
   row[2]: deg_degrees.ShortName
   row[3]: deg_degrees.FullName
   row[4]: crs_courses.Year
   row[5]: crs_courses.FullName
   row[6]: ctr_centers.ShortName
   row[7]: crs_users.Accepted (only if WriteColumnAccepted == true)
   */

   /***** Get degree code (row[0]) *****/
   if ((Deg.HieCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongDegreeExit ();
   if (!Hie_GetDataByCod[Hie_DEG] (&Deg))
      Err_WrongDegreeExit ();

   /***** Get course code (row[1]) *****/
   if ((CrsCod = Str_ConvertStrCodToLongCod (row[1])) <= 0)
      Err_WrongCourseExit ();

   /***** Get number of teachers and students in this course *****/
   NumStds = Enr_GetNumUsrsInCrss (Hie_CRS,CrsCod,1 << Rol_STD);
   NumNETs = Enr_GetNumUsrsInCrss (Hie_CRS,CrsCod,1 << Rol_NET);
   NumTchs = Enr_GetNumUsrsInCrss (Hie_CRS,CrsCod,1 << Rol_TCH);
   NumUsrs = NumStds + NumNETs + NumTchs;
   ClassTxt = NumUsrs ? "DAT_STRONG" :
	                "DAT";
   BgColor = (CrsCod == Gbl.Hierarchy.Node[Hie_CRS].HieCod) ? "BG_HIGHLIGHT" :
							      The_GetColorRows ();

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** User has accepted joining to this course/to any course in degree/to any course? *****/
      if (WriteColumnAccepted)
	{
	 Accepted = (row[7][0] == 'Y');
	 HTM_TD_Begin ("class=\"BT %s\" title=\"%s\"",
		       BgColor,
		       Accepted ? Txt_Enrolment_confirmed :
				  Txt_Enrolment_not_confirmed);
	    HTM_Txt (Accepted ? "&check;" :
				"&cross;");
	 HTM_TD_End ();
	}

      /***** Write number of course in this search *****/
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",
                    ClassTxt,The_GetSuffix (),BgColor);
	 HTM_Unsigned (NumCrs);
      HTM_TD_End ();

      /***** Write degree logo, degree short name (row[2])
	     and center short name (row[6]) *****/
      HTM_TD_Begin ("class=\"LT %s_%s %s\"",
                    ClassTxt,The_GetSuffix (),BgColor);
	 Frm_BeginFormGoTo (ActSeeDegInf);
	    ParCod_PutPar (ParCod_Deg,Deg.HieCod);
	    HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (row[2]),
	                             "class=\"LT BT_LINK\"");
            Str_FreeGoToTitle ();
	       Lgo_DrawLogo (Hie_DEG,&Deg,"CT ICO20x20");
	       HTM_TxtF ("&nbsp;%s&nbsp;(%s)",row[2],row[6]);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
      HTM_TD_End ();

      /***** Write year (row[4]) *****/
      HTM_TD_Begin ("class=\"CT %s_%s %s\"",
                    ClassTxt,The_GetSuffix (),BgColor);
	 HTM_Txt (Txt_YEAR_OF_DEGREE[Deg_ConvStrToYear (row[4])]);
      HTM_TD_End ();

      /***** Write course full name (row[5]) *****/
      HTM_TD_Begin ("class=\"LT %s_%s %s\"",
                    ClassTxt,The_GetSuffix (),BgColor);
	 Frm_BeginFormGoTo (ActSeeCrsInf);
	    ParCod_PutPar (ParCod_Crs,CrsCod);
	    HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (row[5]),
	                             "class=\"LT BT_LINK\"");
            Str_FreeGoToTitle ();
	       HTM_Txt (row[5]);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
      HTM_TD_End ();

      /***** Write number of teachers in course *****/
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",
                    ClassTxt,The_GetSuffix (),BgColor);
	 HTM_Unsigned (NumTchs);
      HTM_TD_End ();

      /***** Write number of non-editing teachers in course *****/
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",
                    ClassTxt,The_GetSuffix (),BgColor);
	 HTM_Unsigned (NumNETs);
      HTM_TD_End ();

      /***** Write number of students in course *****/
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",
                    ClassTxt,The_GetSuffix (),BgColor);
	 HTM_Unsigned (NumStds);
      HTM_TD_End ();

   HTM_TR_End ();

   The_ChangeRowColor ();
  }

/*****************************************************************************/
/******************* Update last click in current course *********************/
/*****************************************************************************/

void Crs_UpdateCrsLast (void)
  {
   if (Gbl.Hierarchy.Level == Hie_CRS &&	// Course selected
       Gbl.Usrs.Me.Role.Logged >= Rol_STD)
      /***** Update last access to current course *****/
      Crs_DB_UpdateCrsLastClick ();
  }

/*****************************************************************************/
/********************** Put link to remove old courses ***********************/
/*****************************************************************************/

void Crs_PutLinkToRemoveOldCrss (void)
  {
   extern const char *Txt_Eliminate_old_courses;

   /***** Put form to remove old courses *****/
   Lay_PutContextualLinkIconText (ActReqRemOldCrs,NULL,
                                  NULL,NULL,
				  "trash.svg",Ico_RED,
				  Txt_Eliminate_old_courses,NULL);
  }

/*****************************************************************************/
/********************** Write form to remove old courses *********************/
/*****************************************************************************/

void Crs_AskRemoveOldCrss (void)
  {
   extern const char *Hlp_SYSTEM_Maintenance_eliminate_old_courses;
   extern const char *Txt_Eliminate_old_courses;
   extern const char *Txt_Eliminate_all_courses_whithout_users_PART_1_OF_2;
   extern const char *Txt_Eliminate_all_courses_whithout_users_PART_2_OF_2;
   extern const char *Txt_Eliminate;
   unsigned MonthsWithoutAccess = Crs_DEF_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS;
   unsigned i;

   /***** Begin form *****/
   Frm_BeginForm (ActRemOldCrs);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Eliminate_old_courses,NULL,NULL,
		    Hlp_SYSTEM_Maintenance_eliminate_old_courses,Box_NOT_CLOSABLE);

      /***** Form to request number of months without clicks *****/
      HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	 HTM_TxtF ("%s&nbsp;",Txt_Eliminate_all_courses_whithout_users_PART_1_OF_2);
	 HTM_SELECT_Begin (HTM_ENABLED,HTM_NOT_REQUIRED,
			   HTM_DONT_SUBMIT_ON_CHANGE,NULL,
			   "name=\"Months\" class=\"INPUT_%s\"",
			   The_GetSuffix ());
	    for (i  = Crs_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS;
		 i <= Crs_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS;
		 i++)
	       HTM_OPTION (HTM_Type_UNSIGNED,&i,
			   i == MonthsWithoutAccess ? HTM_OPTION_SELECTED :
						      HTM_OPTION_UNSELECTED,
			   HTM_ENABLED,
			   "%u",i);
	 HTM_SELECT_End ();
	 HTM_NBSP ();
	 HTM_TxtF (Txt_Eliminate_all_courses_whithout_users_PART_2_OF_2,
		   Cfg_PLATFORM_SHORT_NAME);
      HTM_LABEL_End ();

      /***** Send button and end box *****/
      Box_BoxWithButtonEnd (Btn_REMOVE_BUTTON,Txt_Eliminate);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/**************************** Remove old courses *****************************/
/*****************************************************************************/

void Crs_RemoveOldCrss (void)
  {
   extern const char *Txt_Eliminating_X_courses_whithout_users_and_with_more_than_Y_months_without_access;
   extern const char *Txt_X_courses_have_been_eliminated;
   unsigned MonthsWithoutAccess;
   unsigned long SecondsWithoutAccess;
   MYSQL_RES *mysql_res;
   unsigned NumCrss;
   unsigned NumCrs;
   unsigned NumCrssRemoved = 0;
   long CrsCod;

   /***** Get parameter with number of months without access *****/
   MonthsWithoutAccess = (unsigned)
	                 Par_GetParUnsignedLong ("Months",
                                                 Crs_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS,
                                                 Crs_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS,
                                                 UINT_MAX);
   if (MonthsWithoutAccess == UINT_MAX)
      Err_ShowErrorAndExit ("Wrong number of months without clicks.");
   SecondsWithoutAccess = (unsigned long) MonthsWithoutAccess * Dat_SECONDS_IN_ONE_MONTH;

   /***** Get old courses from database *****/
   if ((NumCrss = Crs_DB_GetOldCrss (&mysql_res,SecondsWithoutAccess)))
     {
      /***** Initial warning alert *****/
      Ale_ShowAlert (Ale_INFO,Txt_Eliminating_X_courses_whithout_users_and_with_more_than_Y_months_without_access,
		     NumCrss,
		     MonthsWithoutAccess,
		     Cfg_PLATFORM_SHORT_NAME);

      /***** Remove courses *****/
      for (NumCrs = 0;
           NumCrs < NumCrss;
           NumCrs++)
        {
         CrsCod = DB_GetNextCode (mysql_res);
         Crs_RemoveCourseCompletely (CrsCod);
         NumCrssRemoved++;
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** Write final success alert *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_X_courses_have_been_eliminated,
                  NumCrssRemoved);
  }

/*****************************************************************************/
/************************ Course constructor/destructor **********************/
/*****************************************************************************/

static void Crs_EditingCourseConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Crs_EditingCrs != NULL)
      Err_WrongCourseExit ();

   /***** Allocate memory for course *****/
   if ((Crs_EditingCrs = malloc (sizeof (*Crs_EditingCrs))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Reset course *****/
   Crs_EditingCrs->HieCod        = -1L;
   Crs_EditingCrs->PrtCod        = -1L;
   Crs_EditingCrs->Specific.Year = 0;
   Crs_EditingCrs->Status        = 0;
   Crs_EditingCrs->InstitutionalCod[0] = '\0';
   Crs_EditingCrs->ShrtName[0]   = '\0';
   Crs_EditingCrs->FullName[0]   = '\0';
  }

static void Crs_EditingCourseDestructor (void)
  {
   /***** Free memory used for course *****/
   if (Crs_EditingCrs != NULL)
     {
      free (Crs_EditingCrs);
      Crs_EditingCrs = NULL;
     }
  }
