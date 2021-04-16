// swad_course.c: edition of courses

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include "swad_attendance.h"
#include "swad_call_for_exam.h"
#include "swad_course.h"
#include "swad_course_config.h"
#include "swad_database.h"
#include "swad_figure.h"
#include "swad_figure_cache.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_game.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy.h"
#include "swad_HTML.h"
#include "swad_info.h"
#include "swad_logo.h"
#include "swad_message.h"
#include "swad_project.h"
#include "swad_search.h"
#include "swad_survey.h"
#include "swad_test.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*************************** Public constants ********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private types *********************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private variables ******************************/
/*****************************************************************************/

static struct Crs_Course *Crs_EditingCrs = NULL;	// Static variable to keep the course being edited

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Crs_WriteListMyCoursesToSelectOne (void);

static void Crs_GetListCrssInCurrentDeg (Crs_WhatCourses_t WhatCourses);
static void Crs_ListCourses (void);
static bool Crs_CheckIfICanCreateCourses (void);
static void Crs_PutIconsListCourses (__attribute__((unused)) void *Args);
static void Crs_PutIconToEditCourses (void);
static bool Crs_ListCoursesOfAYearForSeeing (unsigned Year);

static void Crs_EditCoursesInternal (void);
static void Crs_PutIconsEditingCourses (__attribute__((unused)) void *Args);
static void Crs_PutIconToViewCourses (void);
static void Crs_ListCoursesForEdition (void);
static void Crs_ListCoursesOfAYearForEdition (unsigned Year);
static bool Crs_CheckIfICanEdit (struct Crs_Course *Crs);
static Crs_StatusTxt_t Crs_GetStatusTxtFromStatusBits (Crs_Status_t Status);
static Crs_Status_t Crs_GetStatusBitsFromStatusTxt (Crs_StatusTxt_t StatusTxt);
static void Crs_PutFormToCreateCourse (void);
static void Crs_PutHeadCoursesForSeeing (void);
static void Crs_PutHeadCoursesForEdition (void);
static void Crs_ReceiveFormRequestOrCreateCrs (unsigned Status);
static void Crs_GetParamsNewCourse (struct Crs_Course *Crs);

static void Crs_CreateCourse (unsigned Status);
static void Crs_GetDataOfCourseFromRow (struct Crs_Course *Crs,MYSQL_ROW row);

static void Crs_GetShortNamesByCod (long CrsCod,
                                    char CrsShortName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1],
                                    char DegShortName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1]);

static void Crs_EmptyCourseCompletely (long CrsCod);

static void Crs_UpdateCrsNameDB (long CrsCod,const char *FieldName,const char *NewCrsName);

static void Crs_PutButtonToGoToCrs (void);
static void Crs_PutButtonToRegisterInCrs (void);

static void Crs_PutIconToSearchCourses (__attribute__((unused)) void *Args);

static void Crs_PutParamOtherCrsCod (void *CrsCod);
static long Crs_GetAndCheckParamOtherCrsCod (long MinCodAllowed);

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
   CrsCfg_Configuration (false);
   HTM_DIV_End ();

   /***** Course introduction *****/
   Inf_ShowInfo ();

   /***** Show help to enrol me *****/
   Hlp_ShowHelpWhatWouldYouLikeToDo ();
  }

/*****************************************************************************/
/************************ Write menu with my courses *************************/
/*****************************************************************************/

#define Crs_MAX_BYTES_TXT_LINK 40

static void Crs_WriteListMyCoursesToSelectOne (void)
  {
   extern const char *Hlp_PROFILE_Courses;
   extern const char *The_ClassFormLinkInBox[The_NUM_THEMES];
   extern const char *The_ClassFormLinkInBoxBold[The_NUM_THEMES];
   extern const char *Txt_My_courses;
   extern const char *Txt_System;
   struct Hie_Hierarchy Hie;
   bool IsLastItemInLevel[1 + 5];
   bool Highlight;	// Highlight because degree, course, etc. is selected
   MYSQL_RES *mysql_resCty;
   MYSQL_RES *mysql_resIns;
   MYSQL_RES *mysql_resCtr;
   MYSQL_RES *mysql_resDeg;
   MYSQL_RES *mysql_resCrs;
   MYSQL_ROW row;
   unsigned NumCty;
   unsigned NumCtys;
   unsigned NumIns;
   unsigned NumInss;
   unsigned NumCtr;
   unsigned NumCtrs;
   unsigned NumDeg;
   unsigned NumDegs;
   unsigned NumCrs;
   unsigned NumCrss;
   const char *ClassNormal;
   char ClassHighlight[64];

   ClassNormal = The_ClassFormLinkInBox[Gbl.Prefs.Theme];
   snprintf (ClassHighlight,sizeof (ClassHighlight),"%s LIGHT_BLUE",
	     The_ClassFormLinkInBoxBold[Gbl.Prefs.Theme]);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_My_courses,
                 Crs_PutIconToSearchCourses,NULL,
                 Hlp_PROFILE_Courses,Box_NOT_CLOSABLE);
   HTM_UL_Begin ("class=\"LIST_TREE\"");

   /***** Write link to platform *****/
   Highlight = (Gbl.Hierarchy.Cty.CtyCod <= 0);
   HTM_LI_Begin ("class=\"%s\"",Highlight ? ClassHighlight :
			                    ClassNormal);
   Frm_BeginForm (ActMyCrs);
   Cty_PutParamCtyCod (-1L);
   HTM_BUTTON_SUBMIT_Begin (Txt_System,
			    Highlight ? ClassHighlight :
				        ClassNormal,
			    NULL);
   Ico_PutIcon ("sitemap.svg",Txt_System,"ICO16x16");
   HTM_TxtF ("&nbsp;%s",Txt_System);
   HTM_BUTTON_End ();
   Frm_EndForm ();
   HTM_LI_End ();

   /***** Get my countries *****/
   NumCtys = Usr_GetCtysFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,&mysql_resCty);
   for (NumCty = 0;
	NumCty < NumCtys;
	NumCty++)
     {
      /***** Get next institution *****/
      row = mysql_fetch_row (mysql_resCty);

      /***** Get data of this institution *****/
      Hie.Cty.CtyCod = Str_ConvertStrCodToLongCod (row[0]);
      if (!Cty_GetDataOfCountryByCod (&Hie.Cty))
	 Lay_ShowErrorAndExit ("Country not found.");

      /***** Write link to country *****/
      Highlight = (Gbl.Hierarchy.Ins.InsCod <= 0 &&
	           Gbl.Hierarchy.Cty.CtyCod == Hie.Cty.CtyCod);
      HTM_LI_Begin ("class=\"%s\"",Highlight ? ClassHighlight :
			                       ClassNormal);
      IsLastItemInLevel[1] = (NumCty == NumCtys - 1);
      Lay_IndentDependingOnLevel (1,IsLastItemInLevel);
      Frm_BeginForm (ActMyCrs);
      Cty_PutParamCtyCod (Hie.Cty.CtyCod);
      HTM_BUTTON_SUBMIT_Begin (Act_GetActionText (ActSeeCtyInf),
			       Highlight ? ClassHighlight :
					   ClassNormal,
			       NULL);
      Cty_DrawCountryMap (&Hie.Cty,"ICO16x16");
      HTM_TxtF ("&nbsp;%s",Hie.Cty.Name[Gbl.Prefs.Language]);
      HTM_BUTTON_End ();
      Frm_EndForm ();
      HTM_LI_End ();

      /***** Get my institutions in this country *****/
      NumInss = Usr_GetInssFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,
                                    Hie.Cty.CtyCod,&mysql_resIns);
      for (NumIns = 0;
	   NumIns < NumInss;
	   NumIns++)
	{
	 /***** Get next institution *****/
	 row = mysql_fetch_row (mysql_resIns);

	 /***** Get data of this institution *****/
	 Hie.Ins.InsCod = Str_ConvertStrCodToLongCod (row[0]);
	 if (!Ins_GetDataOfInstitutionByCod (&Hie.Ins))
	    Lay_ShowErrorAndExit ("Institution not found.");

	 /***** Write link to institution *****/
	 Highlight = (Gbl.Hierarchy.Ctr.CtrCod <= 0 &&
	              Gbl.Hierarchy.Ins.InsCod == Hie.Ins.InsCod);
	 HTM_LI_Begin ("class=\"%s\"",Highlight ? ClassHighlight :
			                          ClassNormal);
	 IsLastItemInLevel[2] = (NumIns == NumInss - 1);
	 Lay_IndentDependingOnLevel (2,IsLastItemInLevel);
         Frm_BeginForm (ActMyCrs);
	 Ins_PutParamInsCod (Hie.Ins.InsCod);
	 HTM_BUTTON_SUBMIT_Begin (Act_GetActionText (ActSeeInsInf),
				  Highlight ? ClassHighlight :
					      ClassNormal,
				  NULL);
	 Lgo_DrawLogo (Hie_Lvl_INS,Hie.Ins.InsCod,Hie.Ins.ShrtName,16,NULL,true);
	 HTM_TxtF ("&nbsp;%s",Hie.Ins.ShrtName);
	 HTM_BUTTON_End ();
	 Frm_EndForm ();
	 HTM_LI_End ();

	 /***** Get my centers in this institution *****/
	 NumCtrs = Usr_GetCtrsFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,
	                               Hie.Ins.InsCod,&mysql_resCtr);
	 for (NumCtr = 0;
	      NumCtr < NumCtrs;
	      NumCtr++)
	   {
	    /***** Get next center *****/
	    row = mysql_fetch_row (mysql_resCtr);

	    /***** Get data of this center *****/
	    Hie.Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[0]);
	    if (!Ctr_GetDataOfCenterByCod (&Hie.Ctr))
	       Lay_ShowErrorAndExit ("Center not found.");

	    /***** Write link to center *****/
	    Highlight = (Gbl.Hierarchy.Level == Hie_Lvl_CTR &&
			 Gbl.Hierarchy.Ctr.CtrCod == Hie.Ctr.CtrCod);
	    HTM_LI_Begin ("class=\"%s\"",Highlight ? ClassHighlight :
			                             ClassNormal);
	    IsLastItemInLevel[3] = (NumCtr == NumCtrs - 1);
	    Lay_IndentDependingOnLevel (3,IsLastItemInLevel);
            Frm_BeginForm (ActMyCrs);
	    Ctr_PutParamCtrCod (Hie.Ctr.CtrCod);
	    HTM_BUTTON_SUBMIT_Begin (Act_GetActionText (ActSeeCtrInf),
				     Highlight ? ClassHighlight :
						 ClassNormal,
				     NULL);
	    Lgo_DrawLogo (Hie_Lvl_CTR,Hie.Ctr.CtrCod,Hie.Ctr.ShrtName,16,NULL,true);
	    HTM_TxtF ("&nbsp;%s",Hie.Ctr.ShrtName);
	    HTM_BUTTON_End ();
	    Frm_EndForm ();
	    HTM_LI_End ();

	    /***** Get my degrees in this center *****/
	    NumDegs = Usr_GetDegsFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,
	                                  Hie.Ctr.CtrCod,&mysql_resDeg);
	    for (NumDeg = 0;
		 NumDeg < NumDegs;
		 NumDeg++)
	      {
	       /***** Get next degree *****/
	       row = mysql_fetch_row (mysql_resDeg);

	       /***** Get data of this degree *****/
	       Hie.Deg.DegCod = Str_ConvertStrCodToLongCod (row[0]);
	       if (!Deg_GetDataOfDegreeByCod (&Hie.Deg))
		  Lay_ShowErrorAndExit ("Degree not found.");

	       /***** Write link to degree *****/
	       Highlight = (Gbl.Hierarchy.Level == Hie_Lvl_DEG &&
			    Gbl.Hierarchy.Deg.DegCod == Hie.Deg.DegCod);
	       HTM_LI_Begin ("class=\"%s\"",Highlight ? ClassHighlight :
			                                ClassNormal);
	       IsLastItemInLevel[4] = (NumDeg == NumDegs - 1);
	       Lay_IndentDependingOnLevel (4,IsLastItemInLevel);
               Frm_BeginForm (ActMyCrs);
	       Deg_PutParamDegCod (Hie.Deg.DegCod);
	       HTM_BUTTON_SUBMIT_Begin (Act_GetActionText (ActSeeDegInf),
					Highlight ? ClassHighlight :
						    ClassNormal,
					NULL);
	       Lgo_DrawLogo (Hie_Lvl_DEG,Hie.Deg.DegCod,Hie.Deg.ShrtName,16,NULL,true);
	       HTM_TxtF ("&nbsp;%s",Hie.Deg.ShrtName);
	       HTM_BUTTON_End ();
	       Frm_EndForm ();
	       HTM_LI_End ();

	       /***** Get my courses in this degree *****/
	       NumCrss = Usr_GetCrssFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,
	                                     Hie.Deg.DegCod,&mysql_resCrs);
	       for (NumCrs = 0;
		    NumCrs < NumCrss;
		    NumCrs++)
		 {
		  /***** Get next course *****/
		  row = mysql_fetch_row (mysql_resCrs);

		  /***** Get data of this course *****/
		  Hie.Crs.CrsCod = Str_ConvertStrCodToLongCod (row[0]);
		  if (!Crs_GetDataOfCourseByCod (&Hie.Crs))
		     Lay_ShowErrorAndExit ("Course not found.");

		  /***** Write link to course *****/
		  Highlight = (Gbl.Hierarchy.Level == Hie_Lvl_CRS &&
			       Gbl.Hierarchy.Crs.CrsCod == Hie.Crs.CrsCod);
		  HTM_LI_Begin ("class=\"%s\"",Highlight ? ClassHighlight :
			                                   ClassNormal);
		  IsLastItemInLevel[5] = (NumCrs == NumCrss - 1);
		  Lay_IndentDependingOnLevel (5,IsLastItemInLevel);
                  Frm_BeginForm (ActMyCrs);
		  Crs_PutParamCrsCod (Hie.Crs.CrsCod);
		  HTM_BUTTON_SUBMIT_Begin (Hie_BuildGoToMsg (Hie.Crs.ShrtName),
					   Highlight ? ClassHighlight :
						       ClassNormal,
					   NULL);
		  Hie_FreeGoToMsg ();
		  Ico_PutIcon ("chalkboard-teacher.svg",Hie.Crs.FullName,"ICO16x16");
		  HTM_TxtF ("&nbsp;%s",Hie.Crs.ShrtName);
		  HTM_BUTTON_End ();
		  Frm_EndForm ();

		  /***** Put link to register students *****/
		  Enr_PutButtonInlineToRegisterStds (Hie.Crs.CrsCod);

		  HTM_LI_End ();
		 }

	       /* Free structure that stores the query result */
	       DB_FreeMySQLResult (&mysql_resCrs);
	      }

	    /* Free structure that stores the query result */
	    DB_FreeMySQLResult (&mysql_resDeg);
	   }

	 /* Free structure that stores the query result */
	 DB_FreeMySQLResult (&mysql_resCtr);
	}

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_resIns);
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_resCty);

   /***** End box *****/
   HTM_UL_End ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*********************** Get total number of courses *************************/
/*****************************************************************************/

unsigned Crs_GetCachedNumCrssInSys (void)
  {
   unsigned NumCrss;

   /***** Get number of courses from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CRSS,Hie_Lvl_SYS,-1L,
                                   FigCch_UNSIGNED,&NumCrss))
     {
      /***** Get current number of courses from database and update cache *****/
      NumCrss = (unsigned) DB_GetNumRowsTable ("crs_courses");
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CRSS,Hie_Lvl_SYS,-1L,
                                    FigCch_UNSIGNED,&NumCrss);
     }

   return NumCrss;
  }

/*****************************************************************************/
/****************** Get number of courses in a country ***********************/
/*****************************************************************************/

void Crs_FlushCacheNumCrssInCty (void)
  {
   Gbl.Cache.NumCrssInCty.CtyCod  = -1L;
   Gbl.Cache.NumCrssInCty.NumCrss = 0;
  }

unsigned Crs_GetNumCrssInCty (long CtyCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (CtyCod <= 0)
      return 0;

   /***** 2. Fast check: If cached... *****/
   if (CtyCod == Gbl.Cache.NumCrssInCty.CtyCod)
      return Gbl.Cache.NumCrssInCty.NumCrss;

   /***** 3. Slow: number of courses in a country from database *****/
   Gbl.Cache.NumCrssInCty.CtyCod  = CtyCod;
   Gbl.Cache.NumCrssInCty.NumCrss = (unsigned)
   DB_QueryCOUNT ("can not get the number of courses in a country",
		  "SELECT COUNT(*)"
		   " FROM ins_instits,"
		         "ctr_centers,"
		         "deg_degrees,"
		         "crs_courses"
		  " WHERE ins_instits.CtyCod=%ld"
		    " AND ins_instits.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		    " AND deg_degrees.DegCod=crs_courses.DegCod",
		  CtyCod);
   FigCch_UpdateFigureIntoCache (FigCch_NUM_CRSS,Hie_Lvl_CTY,Gbl.Cache.NumCrssInCty.CtyCod,
				 FigCch_UNSIGNED,&Gbl.Cache.NumCrssInCty.NumCrss);
   return Gbl.Cache.NumCrssInCty.NumCrss;
  }

unsigned Crs_GetCachedNumCrssInCty (long CtyCod)
  {
   unsigned NumCrss;

   /***** Get number of courses from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CRSS,Hie_Lvl_CTY,CtyCod,
				   FigCch_UNSIGNED,&NumCrss))
      /***** Get current number of courses from database and update cache *****/
      NumCrss = Crs_GetNumCrssInCty (CtyCod);

   return NumCrss;
  }

/*****************************************************************************/
/**************** Get number of courses in an institution ********************/
/*****************************************************************************/

void Crs_FlushCacheNumCrssInIns (void)
  {
   Gbl.Cache.NumCrssInIns.InsCod  = -1L;
   Gbl.Cache.NumCrssInIns.NumCrss = 0;
  }

unsigned Crs_GetNumCrssInIns (long InsCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (InsCod <= 0)
      return 0;

   /***** 2. Fast check: If cached... *****/
   if (InsCod == Gbl.Cache.NumCrssInIns.InsCod)
      return Gbl.Cache.NumCrssInIns.NumCrss;

   /***** 3. Slow: number of courses in an institution from database *****/
   Gbl.Cache.NumCrssInIns.InsCod  = InsCod;
   Gbl.Cache.NumCrssInIns.NumCrss = (unsigned)
   DB_QueryCOUNT ("can not get the number of courses in an institution",
		  "SELECT COUNT(*)"
		   " FROM ctr_centers,"
		         "deg_degrees,"
		         "crs_courses"
		  " WHERE ctr_centers.InsCod=%ld"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		    " AND deg_degrees.DegCod=crs_courses.DegCod",
		  InsCod);
   FigCch_UpdateFigureIntoCache (FigCch_NUM_CRSS,Hie_Lvl_INS,Gbl.Cache.NumCrssInIns.InsCod,
				 FigCch_UNSIGNED,&Gbl.Cache.NumCrssInIns.NumCrss);
   return Gbl.Cache.NumCrssInIns.NumCrss;
  }

unsigned Crs_GetCachedNumCrssInIns (long InsCod)
  {
   unsigned NumCrss;

   /***** Get number of courses from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CRSS,Hie_Lvl_INS,InsCod,
				   FigCch_UNSIGNED,&NumCrss))
      /***** Get current number of courses from database and update cache *****/
      NumCrss = Crs_GetNumCrssInIns (InsCod);

   return NumCrss;
  }

/*****************************************************************************/
/******************** Get number of courses in a center **********************/
/*****************************************************************************/

void Crs_FlushCacheNumCrssInCtr (void)
  {
   Gbl.Cache.NumCrssInCtr.CtrCod  = -1L;
   Gbl.Cache.NumCrssInCtr.NumCrss = 0;
  }

unsigned Crs_GetNumCrssInCtr (long CtrCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (CtrCod <= 0)
      return 0;

   /***** 2. Fast check: If cached... *****/
   if (CtrCod == Gbl.Cache.NumCrssInCtr.CtrCod)
      return Gbl.Cache.NumCrssInCtr.NumCrss;

   /***** 3. Slow: number of courses in a center from database *****/
   Gbl.Cache.NumCrssInCtr.CtrCod  = CtrCod;
   Gbl.Cache.NumCrssInCtr.NumCrss = (unsigned)
   DB_QueryCOUNT ("can not get the number of courses in a center",
		  "SELECT COUNT(*)"
		   " FROM deg_degrees,"
		         "crs_courses"
		  " WHERE deg_degrees.CtrCod=%ld"
		    " AND deg_degrees.DegCod=crs_courses.DegCod",
		  CtrCod);
   return Gbl.Cache.NumCrssInCtr.NumCrss;
  }

unsigned Crs_GetCachedNumCrssInCtr (long CtrCod)
  {
   unsigned NumCrss;

   /***** Get number of courses from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CRSS,Hie_Lvl_CTR,CtrCod,
				   FigCch_UNSIGNED,&NumCrss))
     {
      /***** Get current number of courses from database and update cache *****/
      NumCrss = Crs_GetNumCrssInCtr (CtrCod);
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CRSS,Hie_Lvl_CTR,CtrCod,
				    FigCch_UNSIGNED,&NumCrss);
     }

   return NumCrss;
  }

/*****************************************************************************/
/******************** Get number of courses in a degree **********************/
/*****************************************************************************/

void Crs_FlushCacheNumCrssInDeg (void)
  {
   Gbl.Cache.NumCrssInDeg.DegCod  = -1L;
   Gbl.Cache.NumCrssInDeg.NumCrss = 0;
  }

unsigned Crs_GetNumCrssInDeg (long DegCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (DegCod <= 0)
      return 0;

   /***** 2. Fast check: If cached... *****/
   if (DegCod == Gbl.Cache.NumCrssInDeg.DegCod)
      return Gbl.Cache.NumCrssInDeg.NumCrss;

   /***** 3. Slow: number of courses in a degree from database *****/
   Gbl.Cache.NumCrssInDeg.DegCod  = DegCod;
   Gbl.Cache.NumCrssInDeg.NumCrss = (unsigned)
   DB_QueryCOUNT ("can not get the number of courses in a degree",
		  "SELECT COUNT(*)"
		   " FROM crs_courses"
		  " WHERE DegCod=%ld",
		  DegCod);
   FigCch_UpdateFigureIntoCache (FigCch_NUM_CRSS,Hie_Lvl_DEG,Gbl.Cache.NumCrssInDeg.DegCod,
				 FigCch_UNSIGNED,&Gbl.Cache.NumCrssInDeg.NumCrss);
   return Gbl.Cache.NumCrssInDeg.NumCrss;
  }

unsigned Crs_GetCachedNumCrssInDeg (long DegCod)
  {
   unsigned NumCrss;

   /***** Get number of courses from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CRSS,Hie_Lvl_DEG,DegCod,
				   FigCch_UNSIGNED,&NumCrss))
      /***** Get current number of courses from database and update cache *****/
      NumCrss = Crs_GetNumCrssInDeg (DegCod);

   return NumCrss;
  }

/*****************************************************************************/
/********************* Get number of courses with users **********************/
/*****************************************************************************/

unsigned Crs_GetCachedNumCrssWithUsrs (Rol_Role_t Role,const char *SubQuery,
                                       Hie_Lvl_Level_t Scope,long Cod)
  {
   static const FigCch_FigureCached_t FigureCrss[Rol_NUM_ROLES] =
     {
      [Rol_STD] = FigCch_NUM_CRSS_WITH_STDS,	// Students
      [Rol_NET] = FigCch_NUM_CRSS_WITH_NETS,	// Non-editing teachers
      [Rol_TCH] = FigCch_NUM_CRSS_WITH_TCHS,	// Teachers
     };
   unsigned NumCrssWithUsrs;

   /***** Get number of courses with users from cache *****/
   if (!FigCch_GetFigureFromCache (FigureCrss[Role],Scope,Cod,
				   FigCch_UNSIGNED,&NumCrssWithUsrs))
     {
      /***** Get current number of courses with users from database and update cache *****/
      NumCrssWithUsrs = (unsigned)
      DB_QueryCOUNT ("can not get number of courses with users",
		     "SELECT COUNT(DISTINCT crs_courses.CrsCod)"
		      " FROM ins_instits,"
		            "ctr_centers,"
		            "deg_degrees,"
		            "crs_courses,"
		            "crs_users"
		     " WHERE %s"
		            "institutions.InsCod=ctr_centers.InsCod"
		       " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		       " AND deg_degrees.DegCod=crs_courses.DegCod"
		       " AND crs_courses.CrsCod=crs_users.CrsCod"
		       " AND crs_users.Role=%u",
		     SubQuery,
		     (unsigned) Role);
      FigCch_UpdateFigureIntoCache (FigureCrss[Role],Scope,Cod,
				    FigCch_UNSIGNED,&NumCrssWithUsrs);
     }

   return NumCrssWithUsrs;
  }

/*****************************************************************************/
/*************************** Write selector of course ************************/
/*****************************************************************************/

void Crs_WriteSelectorOfCourse (void)
  {
   extern const char *Txt_Course;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   long CrsCod;

   /***** Begin form *****/
   Frm_BeginFormGoTo (ActSeeCrsInf);
   if (Gbl.Hierarchy.Deg.DegCod > 0)
      HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			"id=\"crs\" name=\"crs\" class=\"HIE_SEL\"");
   else
      HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
			"id=\"crs\" name=\"crs\" class=\"HIE_SEL\""
			" disabled=\"disabled\"");
   HTM_OPTION (HTM_Type_STRING,"",Gbl.Hierarchy.Crs.CrsCod < 0,true,
	       "[%s]",Txt_Course);

   if (Gbl.Hierarchy.Deg.DegCod > 0)
     {
      /***** Get courses belonging to the current degree from database *****/
      NumCrss = (unsigned)
      DB_QuerySELECT (&mysql_res,"can not get courses of a degree",
		      "SELECT CrsCod,"		// row[0]
		             "ShortName"	// row[1]
		       " FROM crs_courses"
		      " WHERE DegCod=%ld"
		      " ORDER BY ShortName",
		      Gbl.Hierarchy.Deg.DegCod);

      /***** Get courses of this degree *****/
      for (NumCrs = 0;
	   NumCrs < NumCrss;
	   NumCrs++)
        {
         /* Get next course */
         row = mysql_fetch_row (mysql_res);

         /* Get course code (row[0]) */
         if ((CrsCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong course.");

         /* Write option */
	 HTM_OPTION (HTM_Type_LONG,&CrsCod,
		     Gbl.Hierarchy.Level == Hie_Lvl_CRS &&	// Course selected
                     CrsCod == Gbl.Hierarchy.Crs.CrsCod,false,
		     "%s",row[1]);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** End form *****/
   HTM_SELECT_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************** Show courses of a degree *************************/
/*****************************************************************************/

void Crs_ShowCrssOfCurrentDeg (void)
  {
   /***** Trivial check *****/
   if (Gbl.Hierarchy.Deg.DegCod <= 0)	// No degree selected
      return;

   /***** Get list of courses in this degree *****/
   Crs_GetListCrssInCurrentDeg (Crs_ALL_COURSES_EXCEPT_REMOVED);

   /***** Write menu to select country, institution, center and degree *****/
   Hie_WriteMenuHierarchy ();

   /***** Show list of courses *****/
   Crs_ListCourses ();

   /***** Free list of courses in this degree *****/
   Crs_FreeListCoursesInCurrentDegree ();
  }

/*****************************************************************************/
/*************** Create a list with courses in current degree ****************/
/*****************************************************************************/

static void Crs_GetListCrssInCurrentDeg (Crs_WhatCourses_t WhatCourses)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   struct Crs_Course *Crs;

   /***** Get courses of a degree from database *****/
   switch (WhatCourses)
     {
      case Crs_ACTIVE_COURSES:
         NumCrss = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get courses of a degree",
			 "SELECT CrsCod,"		// row[0]
			        "DegCod,"		// row[1]
			        "Year,"			// row[2]
			        "InsCrsCod,"		// row[3]
			        "Status,"		// row[4]
			        "RequesterUsrCod,"	// row[5]
			        "ShortName,"		// row[6]
			        "FullName"		// row[7]
			  " FROM crs_courses"
			 " WHERE DegCod=%ld"
			   " AND Status=0"
			 " ORDER BY Year,"
			           "ShortName",
			 Gbl.Hierarchy.Deg.DegCod);
         break;
      case Crs_ALL_COURSES_EXCEPT_REMOVED:
         NumCrss = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get courses of a degree",
			 "SELECT CrsCod,"		// row[0]
			        "DegCod,"		// row[1]
			        "Year,"			// row[2]
			        "InsCrsCod,"		// row[3]
			        "Status,"		// row[4]
			        "RequesterUsrCod,"	// row[5]
			        "ShortName,"		// row[6]
			        "FullName"		// row[7]
			  " FROM crs_courses"
			 " WHERE DegCod=%ld"
			   " AND (Status & %u)=0"
			 " ORDER BY Year,"
			           "ShortName",
			 Gbl.Hierarchy.Deg.DegCod,
			 (unsigned) Crs_STATUS_BIT_REMOVED);
         break;
      default:
	 break;
     }
   if (NumCrss) // Courses found...
     {
      /***** Create list with courses in degree *****/
      if ((Gbl.Hierarchy.Crss.Lst = calloc (NumCrss,
	                                    sizeof (*Gbl.Hierarchy.Ctys.Lst))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the courses in degree *****/
      for (NumCrs = 0;
	   NumCrs < NumCrss;
	   NumCrs++)
        {
         Crs = &Gbl.Hierarchy.Crss.Lst[NumCrs];

         /* Get next course */
         row = mysql_fetch_row (mysql_res);
         Crs_GetDataOfCourseFromRow (Crs,row);
        }
     }

   Gbl.Hierarchy.Crss.Num = NumCrss;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Free list of courses in this degree *******************/
/*****************************************************************************/

void Crs_FreeListCoursesInCurrentDegree (void)
  {
   if (Gbl.Hierarchy.Crss.Lst)
     {
      /***** Free memory used by the list of courses in degree *****/
      free (Gbl.Hierarchy.Crss.Lst);
      Gbl.Hierarchy.Crss.Lst = NULL;
     }
  }

/*****************************************************************************/
/********************** Write selector of my coursess ************************/
/*****************************************************************************/

void Crs_WriteSelectorMyCoursesInBreadcrumb (void)
  {
   extern const char *Txt_Course;
   unsigned NumMyCrs;
   long CrsCod;
   long DegCod;
   long LastDegCod;
   char CrsShortName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1];
   char DegShortName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1];

   /***** Fill the list with the courses I belong to, if not filled *****/
   if (Gbl.Usrs.Me.Logged)
      Usr_GetMyCourses ();

   /***** Begin form *****/
   Frm_BeginFormGoTo (Gbl.Usrs.Me.MyCrss.Num ? ActSeeCrsInf :
                                               ActReqSch);

   /***** Start selector of courses *****/
   HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
		     "id=\"my_courses\" name=\"crs\"");

   /***** Write an option when no course selected *****/
   if (Gbl.Hierarchy.Crs.CrsCod <= 0)	// No course selected
      HTM_OPTION (HTM_Type_STRING,"-1",true,true,
		  "%s",Txt_Course);

   if (Gbl.Usrs.Me.MyCrss.Num)
     {
      /***** Write an option for each of my courses *****/
      for (NumMyCrs = 0, LastDegCod = -1L;
           NumMyCrs < Gbl.Usrs.Me.MyCrss.Num;
           NumMyCrs++)
        {
	 CrsCod = Gbl.Usrs.Me.MyCrss.Crss[NumMyCrs].CrsCod;
	 DegCod = Gbl.Usrs.Me.MyCrss.Crss[NumMyCrs].DegCod;

         Crs_GetShortNamesByCod (CrsCod,CrsShortName,DegShortName);

	 if (DegCod != LastDegCod)
	   {
	    if (LastDegCod > 0)
	       HTM_OPTGROUP_End ();
	    HTM_OPTGROUP_Begin (DegShortName);
	    LastDegCod = DegCod;
	   }

	 HTM_OPTION (HTM_Type_LONG,&Gbl.Usrs.Me.MyCrss.Crss[NumMyCrs].CrsCod,
		     CrsCod == Gbl.Hierarchy.Crs.CrsCod,false,	// Course selected
		     "%s",CrsShortName);
        }

      if (LastDegCod > 0)
	 HTM_OPTGROUP_End ();
     }

   /***** Write an option with the current course
          when I don't belong to it *****/
   if (Gbl.Hierarchy.Level == Hie_Lvl_CRS &&	// Course selected
       !Gbl.Usrs.Me.IBelongToCurrentCrs)	// I do not belong to it
      HTM_OPTION (HTM_Type_LONG,&Gbl.Hierarchy.Crs.CrsCod,true,true,
		  "%s",Gbl.Hierarchy.Crs.ShrtName);

   /***** End form *****/
   HTM_SELECT_End ();
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
   extern const char *Txt_Create_another_course;
   extern const char *Txt_Create_course;
   unsigned Year;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Str_BuildStringStr (Txt_Courses_of_DEGREE_X,
				          Gbl.Hierarchy.Deg.ShrtName),
		 Crs_PutIconsListCourses,NULL,
                 Hlp_DEGREE_Courses,Box_NOT_CLOSABLE);
   Str_FreeString ();

   if (Gbl.Hierarchy.Crss.Num)	// There are courses in the current degree
     {
      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (2);
      Crs_PutHeadCoursesForSeeing ();

      /***** List the courses *****/
      for (Year = 1;
	   Year <= Deg_MAX_YEARS_PER_DEGREE;
	   Year++)
	 if (Crs_ListCoursesOfAYearForSeeing (Year))	// If this year has courses ==>
	    Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;	// ==> change color for the next year
      Crs_ListCoursesOfAYearForSeeing (0);		// Courses without a year selected

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else	// No courses created in the current degree
      Ale_ShowAlert (Ale_INFO,Txt_No_courses);

   /***** Button to create course *****/
   if (Crs_CheckIfICanCreateCourses ())
     {
      Frm_BeginForm (ActEdiCrs);
      Btn_PutConfirmButton (Gbl.Hierarchy.Crss.Num ? Txt_Create_another_course :
	                                                 Txt_Create_course);
      Frm_EndForm ();
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************** Check if I can create courses ************************/
/*****************************************************************************/

static bool Crs_CheckIfICanCreateCourses (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_GST);
  }

/*****************************************************************************/
/***************** Put contextual icons in list of courses *******************/
/*****************************************************************************/

static void Crs_PutIconsListCourses (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit courses *****/
   if (Crs_CheckIfICanCreateCourses ())
      Crs_PutIconToEditCourses ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
  }

/*****************************************************************************/
/************************* Put icon to edit courses **************************/
/*****************************************************************************/

static void Crs_PutIconToEditCourses (void)
  {
   Ico_PutContextualIconToEdit (ActEdiCrs,NULL,
                                NULL,NULL);
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
   extern const char *Txt_COURSE_STATUS[Crs_NUM_STATUS_TXT];
   unsigned NumCrs;
   struct Crs_Course *Crs;
   const char *TxtClassNormal;
   const char *TxtClassStrong;
   const char *BgColor;
   Crs_StatusTxt_t StatusTxt;
   bool ThisYearHasCourses = false;
   unsigned NumUsrs[Rol_NUM_ROLES];

   /***** Write all the courses of this year *****/
   for (NumCrs = 0;
	NumCrs < Gbl.Hierarchy.Crss.Num;
	NumCrs++)
     {
      Crs = &(Gbl.Hierarchy.Crss.Lst[NumCrs]);
      if (Crs->Year == Year)	// The year of the course is this?
	{
	 ThisYearHasCourses = true;
	 if (Crs->Status & Crs_STATUS_BIT_PENDING)
	   {
	    TxtClassNormal = "DAT_LIGHT";
	    TxtClassStrong = "BT_LINK LT DAT_LIGHT";
	   }
	 else
	   {
	    TxtClassNormal = "DAT";
	    TxtClassStrong = "BT_LINK LT DAT_N";
	   }

	 /* Check if this course is one of my courses */
	 BgColor = (Usr_CheckIfIBelongToCrs (Crs->CrsCod)) ? "LIGHT_BLUE" :
				                             Gbl.ColorRows[Gbl.RowEvenOdd];

	 HTM_TR_Begin (NULL);

	 /* Get number of users */
	 NumUsrs[Rol_STD] = Usr_GetCachedNumUsrsInCrss (Hie_Lvl_CRS,Crs->CrsCod,1 << Rol_STD);
	 NumUsrs[Rol_NET] = Usr_GetCachedNumUsrsInCrss (Hie_Lvl_CRS,Crs->CrsCod,1 << Rol_NET);
	 NumUsrs[Rol_TCH] = Usr_GetCachedNumUsrsInCrss (Hie_Lvl_CRS,Crs->CrsCod,1 << Rol_TCH);
	 NumUsrs[Rol_UNK] = NumUsrs[Rol_STD] +
	                    NumUsrs[Rol_NET] +
			    NumUsrs[Rol_TCH];

	 /* Put green tip if course has users */
	 HTM_TD_Begin ("class=\"%s CM %s\" title=\"%s\"",
		       TxtClassNormal,BgColor,
		       NumUsrs[Rol_UNK] ? Txt_COURSE_With_users :
				          Txt_COURSE_Without_users);
	 HTM_Txt (NumUsrs[Rol_UNK] ? "&check;" :
			             "&nbsp;");
	 HTM_TD_End ();

	 /* Institutional code of the course */
	 HTM_TD_Begin ("class=\"%s CM %s\"",TxtClassNormal,BgColor);
	 HTM_Txt (Crs->InstitutionalCrsCod);
	 HTM_TD_End ();

	 /* Course year */
	 HTM_TD_Begin ("class=\"%s CM %s\"",TxtClassNormal,BgColor);
	 HTM_Txt (Txt_YEAR_OF_DEGREE[Crs->Year]);
	 HTM_TD_End ();

	 /* Course full name */
	 HTM_TD_Begin ("class=\"%s LM %s\"",TxtClassStrong,BgColor);
	 Frm_BeginFormGoTo (ActSeeCrsInf);
	 Crs_PutParamCrsCod (Crs->CrsCod);
	 HTM_BUTTON_SUBMIT_Begin (Hie_BuildGoToMsg (Crs->FullName),
				  TxtClassStrong,NULL);
         Hie_FreeGoToMsg ();
	 HTM_Txt (Crs->FullName);
	 HTM_BUTTON_End ();
	 Frm_EndForm ();
	 HTM_TD_End ();

	 /* Number of teachers in this course */
	 HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
	 HTM_Unsigned (NumUsrs[Rol_TCH] +
		       NumUsrs[Rol_NET]);
	 HTM_TD_End ();

	 /* Number of students in this course */
	 HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
	 HTM_Unsigned (NumUsrs[Rol_STD]);
	 HTM_TD_End ();

	 /* Course status */
	 StatusTxt = Crs_GetStatusTxtFromStatusBits (Crs->Status);
	 HTM_TD_Begin ("class=\"%s LM %s\"",TxtClassNormal,BgColor);
	 if (StatusTxt != Crs_STATUS_ACTIVE) // If active ==> do not show anything
	    HTM_Txt (Txt_COURSE_STATUS[StatusTxt]);
	 HTM_TD_End ();

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

   /***** Get list of degrees in this center *****/
   Deg_GetListDegsInCurrentCtr ();

   /***** Get list of courses in this degree *****/
   Crs_GetListCrssInCurrentDeg (Crs_ALL_COURSES_EXCEPT_REMOVED);

   /***** Write menu to select country, institution, center and degree *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Str_BuildStringStr (Txt_Courses_of_DEGREE_X,
				          Gbl.Hierarchy.Deg.ShrtName),
		 Crs_PutIconsEditingCourses,NULL,
                 Hlp_DEGREE_Courses,Box_NOT_CLOSABLE);
   Str_FreeString ();

   /***** Put a form to create or request a new course *****/
   Crs_PutFormToCreateCourse ();

   /***** Forms to edit current courses *****/
   if (Gbl.Hierarchy.Crss.Num)
      Crs_ListCoursesForEdition ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of courses in this degree *****/
   Crs_FreeListCoursesInCurrentDegree ();

   /***** Free list of degrees in this center *****/
   Deg_FreeListDegs (&Gbl.Hierarchy.Degs);
  }

/*****************************************************************************/
/**************** Put contextual icons in edition of courses *****************/
/*****************************************************************************/

static void Crs_PutIconsEditingCourses (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view degrees *****/
   Crs_PutIconToViewCourses ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
  }

/*****************************************************************************/
/************************* Put icon to view courses **************************/
/*****************************************************************************/

static void Crs_PutIconToViewCourses (void)
  {
   extern const char *Txt_Courses;

   Lay_PutContextualLinkOnlyIcon (ActSeeCrs,NULL,
                                  NULL,NULL,
                                  "chalkboard-teacher.svg",
                                  Txt_Courses);
  }

/*****************************************************************************/
/********************* List current courses for edition **********************/
/*****************************************************************************/

static void Crs_ListCoursesForEdition (void)
  {
   unsigned Year;

   /***** Write heading *****/
   HTM_TABLE_BeginWidePadding (2);
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
   extern const char *Txt_COURSE_STATUS[Crs_NUM_STATUS_TXT];
   struct Crs_Course *Crs;
   unsigned YearAux;
   unsigned NumCrs;
   struct UsrData UsrDat;
   bool ICanEdit;
   unsigned NumUsrs[Rol_NUM_ROLES];
   Crs_StatusTxt_t StatusTxt;
   unsigned StatusUnsigned;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** List courses of a given year *****/
   for (NumCrs = 0;
	NumCrs < Gbl.Hierarchy.Crss.Num;
	NumCrs++)
     {
      Crs = &(Gbl.Hierarchy.Crss.Lst[NumCrs]);
      if (Crs->Year == Year)
	{
	 ICanEdit = Crs_CheckIfICanEdit (Crs);

	 /* Get number of users */
	 NumUsrs[Rol_STD] = Usr_GetNumUsrsInCrss (Hie_Lvl_CRS,Crs->CrsCod,1 << Rol_STD);
	 NumUsrs[Rol_NET] = Usr_GetNumUsrsInCrss (Hie_Lvl_CRS,Crs->CrsCod,1 << Rol_NET);
	 NumUsrs[Rol_TCH] = Usr_GetNumUsrsInCrss (Hie_Lvl_CRS,Crs->CrsCod,1 << Rol_TCH);
	 NumUsrs[Rol_UNK] = NumUsrs[Rol_STD] +
	                    NumUsrs[Rol_NET] +
			    NumUsrs[Rol_TCH];

	 HTM_TR_Begin (NULL);

	 /* Put icon to remove course */
	 HTM_TD_Begin ("class=\"BM\"");
	 if (NumUsrs[Rol_UNK] ||	// Course has users ==> deletion forbidden
	     !ICanEdit)
	    Ico_PutIconRemovalNotAllowed ();
	 else	// Crs->NumUsrs == 0 && ICanEdit
	    Ico_PutContextualIconToRemove (ActRemCrs,NULL,
					   Crs_PutParamOtherCrsCod,&Crs->CrsCod);
	 HTM_TD_End ();

	 /* Course code */
	 HTM_TD_Begin ("class=\"DAT CODE\"");
	 HTM_Long (Crs->CrsCod);
	 HTM_TD_End ();

	 /* Institutional code of the course */
	 HTM_TD_Begin ("class=\"DAT CM\"");
	 if (ICanEdit)
	   {
	    Frm_BeginForm (ActChgInsCrsCod);
	    Crs_PutParamOtherCrsCod (&Crs->CrsCod);
	    HTM_INPUT_TEXT ("InsCrsCod",Crs_MAX_CHARS_INSTITUTIONAL_CRS_COD,
			    Crs->InstitutionalCrsCod,HTM_SUBMIT_ON_CHANGE,
			    "class=\"INPUT_INS_CODE\"");
	    Frm_EndForm ();
	   }
	 else
	    HTM_Txt (Crs->InstitutionalCrsCod);
	 HTM_TD_End ();

	 /* Course year */
	 HTM_TD_Begin ("class=\"DAT CM\"");
	 if (ICanEdit)
	   {
	    Frm_BeginForm (ActChgCrsYea);
	    Crs_PutParamOtherCrsCod (&Crs->CrsCod);
	    HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			      "name=\"OthCrsYear\" class=\"HIE_SEL_NARROW\"");
	    for (YearAux = 0;
		 YearAux <= Deg_MAX_YEARS_PER_DEGREE;
		 YearAux++)	// All the years are permitted because it's possible to move this course to another degree (with other active years)
	       HTM_OPTION (HTM_Type_UNSIGNED,&YearAux,
			   YearAux == Crs->Year,false,
		           "%s",Txt_YEAR_OF_DEGREE[YearAux]);
	    HTM_SELECT_End ();
	    Frm_EndForm ();
	   }
	 else
	    HTM_Txt (Txt_YEAR_OF_DEGREE[Crs->Year]);
	 HTM_TD_End ();

	 /* Course short name */
	 HTM_TD_Begin ("class=\"DAT LM\"");
	 if (ICanEdit)
	   {
	    Frm_BeginForm (ActRenCrsSho);
	    Crs_PutParamOtherCrsCod (&Crs->CrsCod);
	    HTM_INPUT_TEXT ("ShortName",Cns_HIERARCHY_MAX_CHARS_SHRT_NAME,Crs->ShrtName,
	                    HTM_SUBMIT_ON_CHANGE,
			    "class=\"INPUT_SHORT_NAME\"");
	    Frm_EndForm ();
	   }
	 else
	    HTM_Txt (Crs->ShrtName);
	 HTM_TD_End ();

	 /* Course full name */
	 HTM_TD_Begin ("class=\"DAT LM\"");
	 if (ICanEdit)
	   {
	    Frm_BeginForm (ActRenCrsFul);
	    Crs_PutParamOtherCrsCod (&Crs->CrsCod);
	    HTM_INPUT_TEXT ("FullName",Cns_HIERARCHY_MAX_CHARS_FULL_NAME,Crs->FullName,
	                    HTM_SUBMIT_ON_CHANGE,
			    "class=\"INPUT_FULL_NAME\"");
	    Frm_EndForm ();
	   }
	 else
	    HTM_Txt (Crs->FullName);
	 HTM_TD_End ();

	 /* Current number of teachers in this course */
	 HTM_TD_Begin ("class=\"DAT RM\"");
	 HTM_Unsigned (NumUsrs[Rol_TCH] +
		       NumUsrs[Rol_NET]);
	 HTM_TD_End ();

	 /* Current number of students in this course */
	 HTM_TD_Begin ("class=\"DAT RM\"");
	 HTM_Unsigned (NumUsrs[Rol_STD]);
	 HTM_TD_End ();

	 /* Course requester */
	 UsrDat.UsrCod = Crs->RequesterUsrCod;
	 Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);
	 HTM_TD_Begin ("class=\"DAT INPUT_REQUESTER LT\"");
	 Msg_WriteMsgAuthor (&UsrDat,true,NULL);
	 HTM_TD_End ();

	 /* Course status */
	 StatusTxt = Crs_GetStatusTxtFromStatusBits (Crs->Status);
	 HTM_TD_Begin ("class=\"DAT LM\"");
	 if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM &&
	     StatusTxt == Crs_STATUS_PENDING)
	   {
	    Frm_BeginForm (ActChgCrsSta);
	    Crs_PutParamOtherCrsCod (&Crs->CrsCod);
	    HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			      "name=\"Status\" class=\"INPUT_STATUS\"");

	    StatusUnsigned = (unsigned) Crs_GetStatusBitsFromStatusTxt (Crs_STATUS_PENDING);
	    HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,true,false,
			"%s",Txt_COURSE_STATUS[Crs_STATUS_PENDING]);

	    StatusUnsigned = (unsigned) Crs_GetStatusBitsFromStatusTxt (Crs_STATUS_ACTIVE);
	    HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,false,false,
			"%s",Txt_COURSE_STATUS[Crs_STATUS_ACTIVE]);

	    HTM_SELECT_End ();
	    Frm_EndForm ();
	   }
         else if (StatusTxt != Crs_STATUS_ACTIVE)	// If active ==> do not show anything
	    HTM_Txt (Txt_COURSE_STATUS[StatusTxt]);
	 HTM_TD_End ();

	 HTM_TR_End ();
	}
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************** Check if I can edit, remove, etc. a course *******************/
/*****************************************************************************/

static bool Crs_CheckIfICanEdit (struct Crs_Course *Crs)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM ||		// I am a degree administrator or higher
                  ((Crs->Status & Crs_STATUS_BIT_PENDING) != 0 &&	// Course is not yet activated
                   Gbl.Usrs.Me.UsrDat.UsrCod == Crs->RequesterUsrCod));	// I am the requester
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
   extern const char *Txt_New_course;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_Create_course;
   unsigned Year;

   /***** Begin form *****/
   if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
      Frm_BeginForm (ActNewCrs);
   else if (Gbl.Usrs.Me.Role.Max >= Rol_GST)
      Frm_BeginForm (ActReqCrs);
   else
      Lay_NoPermissionExit ();

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_New_course,
                      NULL,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Crs_PutHeadCoursesForEdition ();

   HTM_TR_Begin (NULL);

   /***** Column to remove course, disabled here *****/
   HTM_TD_Begin ("class=\"BM\"");
   HTM_TD_End ();

   /***** Course code *****/
   HTM_TD_Begin ("class=\"CODE\"");
   HTM_TD_End ();

   /***** Institutional code of the course *****/
   HTM_TD_Begin ("class=\"CM\"");
   HTM_INPUT_TEXT ("InsCrsCod",Crs_MAX_CHARS_INSTITUTIONAL_CRS_COD,
		   Crs_EditingCrs->InstitutionalCrsCod,
		   HTM_DONT_SUBMIT_ON_CHANGE,
		   "class=\"INPUT_INS_CODE\"");
   HTM_TD_End ();

   /***** Year *****/
   HTM_TD_Begin ("class=\"CM\"");
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "name=\"OthCrsYear\" class=\"HIE_SEL_NARROW\"");
   for (Year = 0;
	Year <= Deg_MAX_YEARS_PER_DEGREE;
        Year++)
      HTM_OPTION (HTM_Type_UNSIGNED,&Year,
		  Year == Crs_EditingCrs->Year,false,
		  "%s",Txt_YEAR_OF_DEGREE[Year]);
   HTM_SELECT_End ();
   HTM_TD_End ();

   /***** Course short name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("ShortName",Cns_HIERARCHY_MAX_CHARS_SHRT_NAME,Crs_EditingCrs->ShrtName,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "class=\"INPUT_SHORT_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Course full name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("FullName",Cns_HIERARCHY_MAX_CHARS_FULL_NAME,Crs_EditingCrs->FullName,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "class=\"INPUT_FULL_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Current number of teachers in this course *****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (0);
   HTM_TD_End ();

   /***** Current number of students in this course *****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (0);
   HTM_TD_End ();

   /***** Course requester *****/
   HTM_TD_Begin ("class=\"DAT INPUT_REQUESTER LT\"");
   Msg_WriteMsgAuthor (&Gbl.Usrs.Me.UsrDat,true,NULL);
   HTM_TD_End ();

   /***** Course status *****/
   HTM_TD_Begin ("class=\"DAT LM\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_course);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Write header with fields of a course *******************/
/*****************************************************************************/

static void Crs_PutHeadCoursesForSeeing (void)
  {
   extern const char *Txt_Institutional_BR_code;
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_Course;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];

   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"BM",NULL);
   HTM_TH (1,1,"CM",Txt_Institutional_BR_code);
   HTM_TH (1,1,"CM",Txt_Year_OF_A_DEGREE);
   HTM_TH (1,1,"LM",Txt_Course);
   HTM_TH (1,1,"RM",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
   HTM_TH (1,1,"RM",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
   HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Write header with fields of a course *******************/
/*****************************************************************************/

static void Crs_PutHeadCoursesForEdition (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Institutional_code;
   extern const char *Txt_optional;
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_Short_name_of_the_course;
   extern const char *Txt_Full_name_of_the_course;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_Requester;

   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"BM",NULL);
   HTM_TH (1,1,"RM",Txt_Code);
   HTM_TH_Begin (1,1,"CM");
   HTM_TxtF ("%s&nbsp;(%s)",Txt_Institutional_code,Txt_optional);
   HTM_TH_End ();
   HTM_TH (1,1,"CM",Txt_Year_OF_A_DEGREE);
   HTM_TH (1,1,"LM",Txt_Short_name_of_the_course);
   HTM_TH (1,1,"LM",Txt_Full_name_of_the_course);
   HTM_TH (1,1,"RM",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
   HTM_TH (1,1,"RM",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
   HTM_TH (1,1,"LM",Txt_Requester);
   HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/****************** Receive form to request a new course *********************/
/*****************************************************************************/

void Crs_ReceiveFormReqCrs (void)
  {
   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Receive form to request a new course *****/
   Crs_ReceiveFormRequestOrCreateCrs ((unsigned) Crs_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/******************* Receive form to create a new course *********************/
/*****************************************************************************/

void Crs_ReceiveFormNewCrs (void)
  {
   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Receive form to create a new course *****/
   Crs_ReceiveFormRequestOrCreateCrs (0);
  }

/*****************************************************************************/
/************* Receive form to request or create a new course ****************/
/*****************************************************************************/

static void Crs_ReceiveFormRequestOrCreateCrs (unsigned Status)
  {
   extern const char *Txt_The_course_X_already_exists;
   extern const char *Txt_Created_new_course_X;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_course;
   extern const char *Txt_The_year_X_is_not_allowed;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];

   /***** Get parameters from form *****/
   /* Set course degree */
   // Deg.DegCod =
   Crs_EditingCrs->DegCod = Gbl.Hierarchy.Deg.DegCod;

   /* Get parameters of the new course */
   Crs_GetParamsNewCourse (Crs_EditingCrs);

   /***** Check if year is correct *****/
   if (Crs_EditingCrs->Year <= Deg_MAX_YEARS_PER_DEGREE)	// If year is valid
     {
      if (Crs_EditingCrs->ShrtName[0] &&
	  Crs_EditingCrs->FullName[0])	// If there's a course name
	{
	 /***** If name of course was in database... *****/
	 if (Crs_CheckIfCrsNameExistsInYearOfDeg ("ShortName",Crs_EditingCrs->ShrtName,
						  -1L,Crs_EditingCrs->DegCod,Crs_EditingCrs->Year))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_course_X_already_exists,
	                     Crs_EditingCrs->ShrtName);
	 else if (Crs_CheckIfCrsNameExistsInYearOfDeg ("FullName",Crs_EditingCrs->FullName,
	                                               -1L,Crs_EditingCrs->DegCod,Crs_EditingCrs->Year))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_course_X_already_exists,
		             Crs_EditingCrs->FullName);
	 else	// Add new requested course to database
	   {
	    Crs_CreateCourse (Status);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
			     Txt_Created_new_course_X,
			     Crs_EditingCrs->FullName);
	   }
	}
      else	// If there is not a course name
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_course);
     }
   else	// Year not valid
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_The_year_X_is_not_allowed,
                       Crs_EditingCrs->Year);
  }

/*****************************************************************************/
/************** Get the parameters of a new course from form *****************/
/*****************************************************************************/

static void Crs_GetParamsNewCourse (struct Crs_Course *Crs)
  {
   char YearStr[2 + 1];

   /***** Get parameters of the course from form *****/
   /* Get institutional code */
   Par_GetParToText ("InsCrsCod",Crs->InstitutionalCrsCod,Crs_MAX_BYTES_INSTITUTIONAL_CRS_COD);

   /* Get year */
   Par_GetParToText ("OthCrsYear",YearStr,2);
   Crs->Year = Deg_ConvStrToYear (YearStr);

   /* Get course short name */
   Par_GetParToText ("ShortName",Crs->ShrtName,Cns_HIERARCHY_MAX_BYTES_SHRT_NAME);

   /* Get course full name */
   Par_GetParToText ("FullName",Crs->FullName,Cns_HIERARCHY_MAX_BYTES_FULL_NAME);
  }

/*****************************************************************************/
/************* Add a new requested course to pending requests ****************/
/*****************************************************************************/

static void Crs_CreateCourse (unsigned Status)
  {
   /***** Insert new course into pending requests *****/
   Crs_EditingCrs->CrsCod =
   DB_QueryINSERTandReturnCode ("can not create a new course",
				"INSERT INTO crs_courses"
				" (DegCod,Year,InsCrsCod,Status,RequesterUsrCod,"
				"ShortName,FullName)"
				" VALUES"
				" (%ld,%u,'%s',%u,%ld,"
				"'%s','%s')",
				Crs_EditingCrs->DegCod,Crs_EditingCrs->Year,
				Crs_EditingCrs->InstitutionalCrsCod,
				Status,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Crs_EditingCrs->ShrtName,
				Crs_EditingCrs->FullName);
  }

/*****************************************************************************/
/****************************** Remove a course ******************************/
/*****************************************************************************/

void Crs_RemoveCourse (void)
  {
   extern const char *Txt_To_remove_a_course_you_must_first_remove_all_users_in_the_course;
   extern const char *Txt_Course_X_removed;

   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Get course code *****/
   Crs_EditingCrs->CrsCod = Crs_GetAndCheckParamOtherCrsCod (1);

   /***** Get data of the course from database *****/
   Crs_GetDataOfCourseByCod (Crs_EditingCrs);

   if (Crs_CheckIfICanEdit (Crs_EditingCrs))
     {
      /***** Check if this course has users *****/
      if (Usr_GetNumUsrsInCrss (Hie_Lvl_CRS,Crs_EditingCrs->CrsCod,
				1 << Rol_STD |
				1 << Rol_NET |
				1 << Rol_TCH))	// Course has users ==> don't remove
         Ale_ShowAlert (Ale_WARNING,
			Txt_To_remove_a_course_you_must_first_remove_all_users_in_the_course);
      else					// Course has no users ==> remove it
        {
         /***** Remove course *****/
         Crs_RemoveCourseCompletely (Crs_EditingCrs->CrsCod);

         /***** Write message to show the change made *****/
         Ale_ShowAlert (Ale_SUCCESS,Txt_Course_X_removed,
                        Crs_EditingCrs->FullName);

         Crs_EditingCrs->CrsCod = -1L;	// To not showing button to go to course
        }
     }
   else
      Lay_NoPermissionExit ();
  }

/*****************************************************************************/
/********************* Get data of a course from its code ********************/
/*****************************************************************************/

bool Crs_GetDataOfCourseByCod (struct Crs_Course *Crs)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool CrsFound = false;

   /***** Clear data *****/
   Crs->DegCod = -1L;
   Crs->Year = 0;
   Crs->Status = (Crs_Status_t) 0;
   Crs->RequesterUsrCod = -1L;
   Crs->ShrtName[0] = '\0';
   Crs->FullName[0] = '\0';

   /***** Check if course code is correct *****/
   if (Crs->CrsCod > 0)
     {
      /***** Get data of a course from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get data of a course",
			  "SELECT CrsCod,"		// row[0]
			         "DegCod,"		// row[1]
			         "Year,"		// row[2]
			         "InsCrsCod,"		// row[3]
			         "Status,"		// row[4]
			         "RequesterUsrCod,"	// row[5]
			         "ShortName,"		// row[6]
			         "FullName"		// row[7]
			   " FROM crs_courses"
			  " WHERE CrsCod=%ld",
			  Crs->CrsCod)) // Course found...
	{
	 /***** Get data of the course *****/
	 row = mysql_fetch_row (mysql_res);
	 Crs_GetDataOfCourseFromRow (Crs,row);

         /* Set return value */
	 CrsFound = true;
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return CrsFound;
  }

/*****************************************************************************/
/********** Get data of a course from a row resulting of a query *************/
/*****************************************************************************/

static void Crs_GetDataOfCourseFromRow (struct Crs_Course *Crs,MYSQL_ROW row)
  {
   /***** Get course code (row[0]) *****/
   if ((Crs->CrsCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
      Lay_ShowErrorAndExit ("Wrong code of course.");

   /***** Get code of degree (row[1]) *****/
   Crs->DegCod = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get year (row[2]) *****/
   Crs->Year = Deg_ConvStrToYear (row[2]);

   /***** Get institutional course code (row[3]) *****/
   Str_Copy (Crs->InstitutionalCrsCod,row[3],sizeof (Crs->InstitutionalCrsCod) - 1);

   /***** Get course status (row[4]) *****/
   if (sscanf (row[4],"%u",&(Crs->Status)) != 1)
      Lay_ShowErrorAndExit ("Wrong course status.");

   /***** Get requester user'code (row[5]) *****/
   Crs->RequesterUsrCod = Str_ConvertStrCodToLongCod (row[5]);

   /***** Get short name (row[6]) and full name (row[7]) of the course *****/
   Str_Copy (Crs->ShrtName,row[6],sizeof (Crs->ShrtName) - 1);
   Str_Copy (Crs->FullName,row[7],sizeof (Crs->FullName) - 1);
  }

/*****************************************************************************/
/******* Get the short names of degree and course from a course code *********/
/*****************************************************************************/

static void Crs_GetShortNamesByCod (long CrsCod,
                                    char CrsShortName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1],
                                    char DegShortName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   DegShortName[0] = CrsShortName[0] = '\0';

   if (CrsCod > 0)
     {
      /***** Get the short name of a degree from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get the short name of a course",
			  "SELECT crs_courses.ShortName,"	// row[0]
			         "deg_degrees.ShortName"	// row[1]
			   " FROM crs_courses,"
			         "deg_degrees"
			  " WHERE crs_courses.CrsCod=%ld"
			    " AND crs_courses.DegCod=deg_degrees.DegCod",
			  CrsCod) == 1)
	{
	 /***** Get the course short name and degree short name *****/
	 row = mysql_fetch_row (mysql_res);
	 Str_Copy (CrsShortName,row[0],Cns_HIERARCHY_MAX_BYTES_SHRT_NAME);
	 Str_Copy (DegShortName,row[1],Cns_HIERARCHY_MAX_BYTES_SHRT_NAME);
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
   if (CrsCod > 0)
     {
      /***** Empty course *****/
      Crs_EmptyCourseCompletely (CrsCod);

      /***** Remove course from table of last accesses to courses in database *****/
      DB_QueryDELETE ("can not remove a course",
		      "DELETE FROM crs_last WHERE CrsCod=%ld",
		      CrsCod);

      /***** Remove course from table of courses in database *****/
      DB_QueryDELETE ("can not remove a course",
		      "DELETE FROM crs_courses WHERE CrsCod=%ld",
		      CrsCod);
     }
  }

/*****************************************************************************/
/********** Empty a course (remove all its information and users) ************/
/*****************************************************************************/
// Start removing less important things to more important things;
// so, in case of failure, important things can been removed in the future

static void Crs_EmptyCourseCompletely (long CrsCod)
  {
   struct Crs_Course Crs;
   char PathRelCrs[PATH_MAX + 1];

   if (CrsCod > 0)
     {
      /***** Get course data *****/
      Crs.CrsCod = CrsCod;
      Crs_GetDataOfCourseByCod (&Crs);

      /***** Remove all the students in the course *****/
      Enr_RemAllStdsInCrs (&Crs);

      /***** Set all the notifications from the course as removed,
	     except notifications about new messages *****/
      Ntf_MarkNotifInCrsAsRemoved (-1L,CrsCod);

      /***** Remove information of the course ****/
      /* Remove timetable of the course */
      DB_QueryDELETE ("can not remove the timetable of a course",
		      "DELETE FROM tmt_courses"
		      " WHERE CrsCod=%ld",
		      CrsCod);

      /* Remove other information of the course */
      DB_QueryDELETE ("can not remove info sources of a course",
		      "DELETE FROM crs_info_src"
		      " WHERE CrsCod=%ld",
		      CrsCod);

      DB_QueryDELETE ("can not remove info of a course",
		      "DELETE FROM crs_info_txt"
		      " WHERE CrsCod=%ld",
		      CrsCod);

      /***** Remove exam announcements in the course *****/
      /* Mark all exam announcements in the course as deleted */
      DB_QueryUPDATE ("can not remove exam announcements of a course",
		      "UPDATE cfe_exams"
		        " SET Status=%u"
		      " WHERE CrsCod=%ld",
	              (unsigned) Cfe_DELETED_CALL_FOR_EXAM,CrsCod);

      /***** Remove course cards of the course *****/
      /* Remove content of course cards */
      DB_QueryDELETE ("can not remove content of cards in a course",
		      "DELETE FROM crs_records"
		      " USING crs_record_fields,"
		             "crs_records"
		      " WHERE crs_record_fields.CrsCod=%ld"
		        " AND crs_record_fields.FieldCod=crs_records.FieldCod",
	              CrsCod);

      /* Remove definition of fields in course cards */
      DB_QueryDELETE ("can not remove fields of cards in a course",
		      "DELETE FROM crs_record_fields"
		      " WHERE CrsCod=%ld",
		      CrsCod);

      /***** Remove information related to files in course,
             including groups and projects,
             so this function must be called
             before removing groups and projects *****/
      Brw_RemoveCrsFilesFromDB (CrsCod);

      /***** Remove assignments of the course *****/
      Asg_RemoveCrsAssignments (CrsCod);

      /***** Remove projects of the course *****/
      Prj_RemoveCrsProjects (CrsCod);

      /***** Remove attendance events of the course *****/
      Att_RemoveCrsAttEvents (CrsCod);

      /***** Remove notices in the course *****/
      /* Copy all notices from the course to table of deleted notices */
      DB_QueryINSERT ("can not remove notices in a course",
		      "INSERT INTO not_deleted"
		      " (NotCod,CrsCod,UsrCod,CreatTime,Content,NumNotif)"
		      " SELECT NotCod,"
		              "CrsCod,"
		              "UsrCod,"
		              "CreatTime,"
		              "Content,"
		              "NumNotif"
		       " FROM not_notices"
		      " WHERE CrsCod=%ld",
	              CrsCod);

      /* Remove all notices from the course */
      DB_QueryDELETE ("can not remove notices in a course",
		      "DELETE FROM not_notices"
		      " WHERE CrsCod=%ld",
		      CrsCod);

      /***** Remove all the threads and posts in forums of the course *****/
      For_RemoveForums (Hie_Lvl_CRS,CrsCod);

      /***** Remove all surveys in the course *****/
      Svy_RemoveSurveys (Hie_Lvl_CRS,CrsCod);

      /***** Remove all games in the course *****/
      Gam_RemoveCrsGames (CrsCod);

      /***** Remove all exams in the course *****/
      Exa_RemoveCrsExams (CrsCod);

      /***** Remove all tests in the course *****/
      Tst_RemoveCrsTests (CrsCod);

      /***** Remove groups in the course *****/
      /* Remove all the users in groups in the course */
      DB_QueryDELETE ("can not remove users from groups of a course",
		      "DELETE FROM grp_users"
		      " USING grp_types,"
		             "grp_groups,"
		             "grp_users"
		      " WHERE grp_types.CrsCod=%ld"
		        " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
		        " AND grp_groups.GrpCod=grp_users.GrpCod",
	              CrsCod);

      /* Remove all the groups in the course */
      DB_QueryDELETE ("can not remove groups of a course",
		      "DELETE FROM grp_groups"
		      " USING grp_types,"
		             "grp_groups"
		      " WHERE grp_types.CrsCod=%ld"
		        " AND grp_types.GrpTypCod=grp_groups.GrpTypCod",
	              CrsCod);

      /* Remove all the group types in the course */
      DB_QueryDELETE ("can not remove types of group of a course",
		      "DELETE FROM grp_types"
		      " WHERE CrsCod=%ld",
		      CrsCod);

      /***** Remove users' requests for inscription in the course *****/
      DB_QueryDELETE ("can not remove requests for inscription to a course",
		      "DELETE FROM crs_requests"
		      " WHERE CrsCod=%ld",
		      CrsCod);

      /***** Remove possible users remaining in the course (teachers) *****/
      DB_QueryDELETE ("can not remove users from a course",
		      "DELETE FROM crs_user_settings"
		      " WHERE CrsCod=%ld",
		      CrsCod);
      DB_QueryDELETE ("can not remove users from a course",
		      "DELETE FROM crs_users"
		      " WHERE CrsCod=%ld",
		      CrsCod);

      /***** Remove directories of the course *****/
      snprintf (PathRelCrs,sizeof (PathRelCrs),"%s/%ld",
	        Cfg_PATH_CRS_PRIVATE,CrsCod);
      Fil_RemoveTree (PathRelCrs);
      snprintf (PathRelCrs,sizeof (PathRelCrs),"%s/%ld",
	        Cfg_PATH_CRS_PUBLIC,CrsCod);
      Fil_RemoveTree (PathRelCrs);
     }
  }

/*****************************************************************************/
/************** Change the institutional code of a course ********************/
/*****************************************************************************/

void Crs_ChangeInsCrsCod (void)
  {
   extern const char *Txt_The_institutional_code_of_the_course_X_has_changed_to_Y;
   extern const char *Txt_The_institutional_code_of_the_course_X_has_not_changed;
   char NewInstitutionalCrsCod[Crs_MAX_BYTES_INSTITUTIONAL_CRS_COD + 1];

   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Get parameters from form *****/
   /* Get course code */
   Crs_EditingCrs->CrsCod = Crs_GetAndCheckParamOtherCrsCod (1);

   /* Get institutional code */
   Par_GetParToText ("InsCrsCod",NewInstitutionalCrsCod,Crs_MAX_BYTES_INSTITUTIONAL_CRS_COD);

   /* Get data of the course */
   Crs_GetDataOfCourseByCod (Crs_EditingCrs);

   if (Crs_CheckIfICanEdit (Crs_EditingCrs))
     {
      /***** Change the institutional course code *****/
      if (strcmp (NewInstitutionalCrsCod,Crs_EditingCrs->InstitutionalCrsCod))
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
   else
      Lay_NoPermissionExit ();
  }

/*****************************************************************************/
/************************ Change the year of a course ************************/
/*****************************************************************************/

void Crs_ChangeCrsYear (void)
  {
   extern const char *Txt_The_course_X_already_exists_in_year_Y;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_The_year_of_the_course_X_has_changed;
   extern const char *Txt_The_year_X_is_not_allowed;
   char YearStr[2 + 1];
   unsigned NewYear;

   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Get parameters from form *****/
   /* Get course code */
   Crs_EditingCrs->CrsCod = Crs_GetAndCheckParamOtherCrsCod (1);

   /* Get parameter with year */
   Par_GetParToText ("OthCrsYear",YearStr,2);
   NewYear = Deg_ConvStrToYear (YearStr);

   Crs_GetDataOfCourseByCod (Crs_EditingCrs);

   if (Crs_CheckIfICanEdit (Crs_EditingCrs))
     {
      if (NewYear <= Deg_MAX_YEARS_PER_DEGREE)	// If year is valid
        {
         /***** If name of course was in database in the new year... *****/
         if (Crs_CheckIfCrsNameExistsInYearOfDeg ("ShortName",Crs_EditingCrs->ShrtName,
                                                  -1L,Crs_EditingCrs->DegCod,NewYear))
	    Ale_CreateAlert (Ale_WARNING,NULL,
		             Txt_The_course_X_already_exists_in_year_Y,
                             Crs_EditingCrs->ShrtName,
			     Txt_YEAR_OF_DEGREE[NewYear]);
         else if (Crs_CheckIfCrsNameExistsInYearOfDeg ("FullName",Crs_EditingCrs->FullName,
                                                       -1L,Crs_EditingCrs->DegCod,NewYear))
	    Ale_CreateAlert (Ale_WARNING,NULL,
		             Txt_The_course_X_already_exists_in_year_Y,
                             Crs_EditingCrs->FullName,
			     Txt_YEAR_OF_DEGREE[NewYear]);
         else	// Update year in database
           {
            /***** Update year in table of courses *****/
            Crs_UpdateCrsYear (Crs_EditingCrs,NewYear);

            /***** Create message to show the change made *****/
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
		             Txt_The_year_of_the_course_X_has_changed,
			     Crs_EditingCrs->ShrtName);
           }
        }
      else	// Year not valid
	 Ale_CreateAlert (Ale_WARNING,NULL,
		          Txt_The_year_X_is_not_allowed,
			  NewYear);
     }
   else
      Lay_NoPermissionExit ();
  }

/*****************************************************************************/
/****************** Change the year/semester of a course *********************/
/*****************************************************************************/

void Crs_UpdateCrsYear (struct Crs_Course *Crs,unsigned NewYear)
  {
   /***** Update year/semester in table of courses *****/
   DB_QueryUPDATE ("can not update the year of a course",
		   "UPDATE crs_courses SET Year=%u WHERE CrsCod=%ld",
	           NewYear,Crs->CrsCod);

   /***** Copy course year/semester *****/
   Crs->Year = NewYear;
  }

/*****************************************************************************/
/************* Change the institutional course code of a course **************/
/*****************************************************************************/

void Crs_UpdateInstitutionalCrsCod (struct Crs_Course *Crs,const char *NewInstitutionalCrsCod)
  {
   /***** Update institutional course code in table of courses *****/
   DB_QueryUPDATE ("can not update the institutional code"
	           " of the current course",
		   "UPDATE crs_courses SET InsCrsCod='%s' WHERE CrsCod=%ld",
                   NewInstitutionalCrsCod,Crs->CrsCod);

   /***** Copy institutional course code *****/
   Str_Copy (Crs->InstitutionalCrsCod,NewInstitutionalCrsCod,
             sizeof (Crs->InstitutionalCrsCod) - 1);
  }

/*****************************************************************************/
/************************ Change the name of a course ************************/
/*****************************************************************************/

void Crs_RenameCourseShort (void)
  {
   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Rename course *****/
   Crs_EditingCrs->CrsCod = Crs_GetAndCheckParamOtherCrsCod (1);
   Crs_RenameCourse (Crs_EditingCrs,Cns_SHRT_NAME);
  }

void Crs_RenameCourseFull (void)
  {
   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Rename course *****/
   Crs_EditingCrs->CrsCod = Crs_GetAndCheckParamOtherCrsCod (1);
   Crs_RenameCourse (Crs_EditingCrs,Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a course ************************/
/*****************************************************************************/

void Crs_RenameCourse (struct Crs_Course *Crs,Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_The_course_X_already_exists;
   extern const char *Txt_The_name_of_the_course_X_has_changed_to_Y;
   extern const char *Txt_The_name_of_the_course_X_has_not_changed;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentCrsName = NULL;		// Initialized to avoid warning
   char NewCrsName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Cns_HIERARCHY_MAX_BYTES_SHRT_NAME;
         CurrentCrsName = Crs->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Cns_HIERARCHY_MAX_BYTES_FULL_NAME;
         CurrentCrsName = Crs->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the new name for the course */
   Par_GetParToText (ParamName,NewCrsName,MaxBytes);

   /***** Get from the database the data of the degree *****/
   Crs_GetDataOfCourseByCod (Crs);

   if (Crs_CheckIfICanEdit (Crs))
     {
      /***** Check if new name is empty *****/
      if (NewCrsName[0])
        {
         /***** Check if old and new names are the same
                (this happens when return is pressed without changes) *****/
         if (strcmp (CurrentCrsName,NewCrsName))	// Different names
           {
            /***** If course was in database... *****/
            if (Crs_CheckIfCrsNameExistsInYearOfDeg (ParamName,NewCrsName,Crs->CrsCod,
                                                     Crs->DegCod,Crs->Year))
	       Ale_CreateAlert (Ale_WARNING,NULL,
		                Txt_The_course_X_already_exists,
                                NewCrsName);
            else
              {
               /* Update the table changing old name by new name */
               Crs_UpdateCrsNameDB (Crs->CrsCod,FieldName,NewCrsName);

               /* Create alert to show the change made */
	       Ale_CreateAlert (Ale_SUCCESS,NULL,
		                Txt_The_name_of_the_course_X_has_changed_to_Y,
				CurrentCrsName,NewCrsName);

               /* Change current course name in order to display it properly */
               Str_Copy (CurrentCrsName,NewCrsName,MaxBytes);
              }
           }
         else	// The same name
	    Ale_CreateAlert (Ale_INFO,NULL,
		             Txt_The_name_of_the_course_X_has_not_changed,
                             CurrentCrsName);
        }
      else
         Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
     }
   else
      Lay_NoPermissionExit ();
  }

/*****************************************************************************/
/********** Check if the name of course exists in existing courses ***********/
/*****************************************************************************/

bool Crs_CheckIfCrsNameExistsInYearOfDeg (const char *FieldName,const char *Name,long CrsCod,
                                          long DegCod,unsigned Year)
  {
   /***** Get number of courses in a year of a degree and with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name"
	                  " of a course already existed",
			  "SELECT COUNT(*)"
			   " FROM crs_courses"
			  " WHERE DegCod=%ld"
			   " AND Year=%u"
			   " AND %s='%s'"
			   " AND CrsCod<>%ld",
			  DegCod,
			  Year,
			  FieldName,
			  Name,
			  CrsCod) != 0);
  }

/*****************************************************************************/
/***************** Update course name in table of courses ********************/
/*****************************************************************************/

static void Crs_UpdateCrsNameDB (long CrsCod,const char *FieldName,const char *NewCrsName)
  {
   /***** Update course changing old name by new name *****/
   DB_QueryUPDATE ("can not update the name of a course",
		   "UPDATE crs_courses SET %s='%s' WHERE CrsCod=%ld",
	           FieldName,NewCrsName,CrsCod);
  }

/*****************************************************************************/
/*********************** Change the status of a course ***********************/
/*****************************************************************************/

void Crs_ChangeCrsStatus (void)
  {
   extern const char *Txt_The_status_of_the_course_X_has_changed;
   Crs_Status_t Status;
   Crs_StatusTxt_t StatusTxt;

   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Get parameters from form *****/
   /* Get course code */
   Crs_EditingCrs->CrsCod = Crs_GetAndCheckParamOtherCrsCod (1);

   /* Get parameter with status */
   Status = (Crs_Status_t)
	    Par_GetParToUnsignedLong ("Status",
	                              0,
	                              (unsigned long) Crs_MAX_STATUS,
                                      (unsigned long) Crs_WRONG_STATUS);
   if (Status == Crs_WRONG_STATUS)
      Lay_ShowErrorAndExit ("Wrong status.");
   StatusTxt = Crs_GetStatusTxtFromStatusBits (Status);
   Status = Crs_GetStatusBitsFromStatusTxt (StatusTxt);	// New status

   /***** Get data of course *****/
   Crs_GetDataOfCourseByCod (Crs_EditingCrs);

   /***** Update status in table of courses *****/
   DB_QueryUPDATE ("can not update the status of a course",
		   "UPDATE crs_courses SET Status=%u WHERE CrsCod=%ld",
                   (unsigned) Status,Crs_EditingCrs->CrsCod);
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
      Ale_ShowLastAlertAndButton1 ();

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
	    PutButtonToRequestRegistration = !Usr_CheckIfUsrBelongsToCrs (Gbl.Usrs.Me.UsrDat.UsrCod,
					                                  Crs_EditingCrs->CrsCod,
					                                  false);
            break;
	 case Rol_STD:
	 case Rol_NET:
	 case Rol_TCH:
	    if (Crs_EditingCrs->CrsCod != Gbl.Hierarchy.Crs.CrsCod)
	       PutButtonToRequestRegistration = !Usr_CheckIfUsrBelongsToCrs (Gbl.Usrs.Me.UsrDat.UsrCod,
									     Crs_EditingCrs->CrsCod,
									     false);
	    break;
	 default:
	    break;

        }
      if (PutButtonToRequestRegistration)
	 Crs_PutButtonToRegisterInCrs ();

      /***** End alert *****/
      Ale_ShowAlertAndButton2 (ActUnk,NULL,NULL,
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
   if (Crs_EditingCrs->CrsCod != Gbl.Hierarchy.Crs.CrsCod)
     {
      Frm_BeginForm (ActSeeCrsInf);
      Crs_PutParamCrsCod (Crs_EditingCrs->CrsCod);
      Btn_PutConfirmButton (Hie_BuildGoToMsg (Crs_EditingCrs->ShrtName));
      Hie_FreeGoToMsg ();
      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/************************ Put button to go to course *************************/
/*****************************************************************************/

static void Crs_PutButtonToRegisterInCrs (void)
  {
   extern const char *Txt_Register_me_in_X;

   Frm_BeginForm (ActReqSignUp);
   // If the course being edited is different to the current one...
   if (Crs_EditingCrs->CrsCod != Gbl.Hierarchy.Crs.CrsCod)
      Crs_PutParamCrsCod (Crs_EditingCrs->CrsCod);
   Btn_PutCreateButton (Str_BuildStringStr (Txt_Register_me_in_X,
					    Crs_EditingCrs->ShrtName));
   Str_FreeString ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************* Select one of my courses **************************/
/*****************************************************************************/

void Crs_ReqSelectOneOfMyCourses (void)
  {
   /***** Fill the list with the courses I belong to, if not filled *****/
   Usr_GetMyCourses ();

   /***** Select one of my courses *****/
   if (Gbl.Usrs.Me.MyCrss.Num)
      /* Show my courses */
      Crs_WriteListMyCoursesToSelectOne ();
   else	// I am not enroled in any course
      /* Show help to enrol me */
      Hlp_ShowHelpWhatWouldYouLikeToDo ();
  }

/*****************************************************************************/
/******************* Put an icon (form) to search courses ********************/
/*****************************************************************************/

static void Crs_PutIconToSearchCourses (__attribute__((unused)) void *Args)
  {
   extern const char *Txt_Search_courses;

   /***** Put form to search / select courses *****/
   Lay_PutContextualLinkOnlyIcon (ActReqSch,NULL,
				  Sch_PutLinkToSearchCoursesParams,NULL,
				  "search.svg",
				  Txt_Search_courses);
  }

/*****************************************************************************/
/********** Put an icon (form) to select my courses in breadcrumb ************/
/*****************************************************************************/

void Crs_PutIconToSelectMyCoursesInBreadcrumb (void)
  {
   extern const char *Txt_My_courses;

   if (Gbl.Usrs.Me.Logged)		// I am logged
     {
      /***** Begin form *****/
      Frm_BeginForm (ActMyCrs);

      /***** Put icon with link *****/
      HTM_INPUT_IMAGE (Gbl.Prefs.URLTheme,"sitemap.svg",Txt_My_courses,
	               "BC_ICON ICO_HIGHLIGHT");
      /*
      HTM_BUTTON_Begin (Txt_My_courses,NULL,NULL);
      HTM_IMG (Gbl.Prefs.URLTheme,"sitemap.svg",Txt_My_courses,
	       "class=\"BC_ICON ICO_HIGHLIGHT\"");
      HTM_BUTTON_End ();
      */

      /***** End form *****/
      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/****************** Put an icon (form) to select my courses ******************/
/*****************************************************************************/

void Crs_PutIconToSelectMyCourses (__attribute__((unused)) void *Args)
  {
   extern const char *Txt_My_courses;

   if (Gbl.Usrs.Me.Logged)		// I am logged
      /***** Put icon with link *****/
      Lay_PutContextualLinkOnlyIcon (ActMyCrs,NULL,
				     NULL,NULL,
				     "sitemap.svg",
				     Txt_My_courses);
  }

/*****************************************************************************/
/******************** Write parameter with code of course ********************/
/*****************************************************************************/

void Crs_PutParamCrsCod (long CrsCod)
  {
   Par_PutHiddenParamLong (NULL,"crs",CrsCod);
  }

/*****************************************************************************/
/******************** Write parameter with code of course ********************/
/*****************************************************************************/

static void Crs_PutParamOtherCrsCod (void *CrsCod)
  {
   if (CrsCod)
      Par_PutHiddenParamLong (NULL,"OthCrsCod",*((long *) CrsCod));
  }

/*****************************************************************************/
/********************* Get parameter with code of course *********************/
/*****************************************************************************/

static long Crs_GetAndCheckParamOtherCrsCod (long MinCodAllowed)
  {
   long CrsCod;

   /***** Get and check parameter with code of course *****/
   if ((CrsCod = Par_GetParToLong ("OthCrsCod")) < MinCodAllowed)
      Lay_ShowErrorAndExit ("Code of course is missing or invalid.");

   return CrsCod;
  }

/*****************************************************************************/
/************************** Write courses of a user **************************/
/*****************************************************************************/

void Crs_GetAndWriteCrssOfAUsr (const struct UsrData *UsrDat,Rol_Role_t Role)
  {
   extern const char *Txt_USER_in_COURSE;
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Degree;
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_Course;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   char *SubQuery;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;

   /***** Get courses of a user from database *****/
   if (Role == Rol_UNK)	// Role == Rol_UNK ==> any role
     {
      if (asprintf (&SubQuery,"%s","") < 0)
	 Lay_NotEnoughMemoryExit ();
     }
   else
     {
      if (asprintf (&SubQuery," AND crs_users.Role=%u",(unsigned) Role) < 0)
	 Lay_NotEnoughMemoryExit ();
     }
   NumCrss = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get courses of a user",
		   "SELECT deg_degrees.DegCod,"		// row[0]
			  "crs_courses.CrsCod,"		// row[1]
			  "deg_degrees.ShortName,"	// row[2]
			  "deg_degrees.FullName,"	// row[3]
			  "crs_courses.Year,"		// row[4]
			  "crs_courses.FullName,"	// row[5]
			  "ctr_centers.ShortName,"	// row[6]
			  "crs_users.Accepted"		// row[7]
		    " FROM crs_users,"
		          "crs_courses,"
		          "deg_degrees,"
		          "ctr_centers"
		   " WHERE crs_users.UsrCod=%ld%s"
		     " AND crs_users.CrsCod=crs_courses.CrsCod"
		     " AND crs_courses.DegCod=deg_degrees.DegCod"
		     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
		   " ORDER BY deg_degrees.FullName,"
		             "crs_courses.Year,"
		             "crs_courses.FullName",
		   UsrDat->UsrCod,SubQuery);

   /***** Free allocated memory for subquery *****/
   free (SubQuery);

   /***** List the courses (one row per course) *****/
   if (NumCrss)
     {
      /* Begin box and table */
      Box_BoxTableBegin ("100%",NULL,
                         NULL,NULL,
                         NULL,Box_NOT_CLOSABLE,2);

      /* Heading row */
      HTM_TR_Begin (NULL);

      HTM_TH_Begin (1,7,"LM");
      HTM_TxtColon (Str_BuildStringStr (Txt_USER_in_COURSE,
				        Role == Rol_UNK ? Txt_User[Usr_SEX_UNKNOWN] : // Role == Rol_UNK ==> any role
							  Txt_ROLES_SINGUL_Abc[Role][UsrDat->Sex]));
      Str_FreeString ();
      HTM_TH_End ();

      HTM_TR_End ();

      HTM_TR_Begin (NULL);

      HTM_TH (1,1,"BM",NULL);
      HTM_TH (1,1,"BM",NULL);
      HTM_TH (1,1,"LM",Txt_Degree);
      HTM_TH (1,1,"CM",Txt_Year_OF_A_DEGREE);
      HTM_TH (1,1,"LM",Txt_Course);
      HTM_TH (1,1,"RM",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
      HTM_TH (1,1,"RM",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NET]);
      HTM_TH (1,1,"RM",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);

      HTM_TR_End ();

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
   extern const char *Txt_course;
   extern const char *Txt_courses;
   extern const char *Txt_Degree;
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_Course;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   MYSQL_ROW row;
   unsigned NumCrs;

   /***** List the courses (one row per course) *****/
   if (NumCrss)
     {
      /***** Begin box and table *****/
      /* Number of courses found */
      Box_BoxTableBegin (NULL,Str_BuildStringLongStr ((long) NumCrss,
						      (NumCrss == 1) ? Txt_course :
								       Txt_courses),
			 NULL,NULL,
			 NULL,Box_NOT_CLOSABLE,2);
      Str_FreeString ();

      /***** Heading row *****/
      HTM_TR_Begin (NULL);

      HTM_TH (1,1,"BM",NULL);
      HTM_TH (1,1,"LM",Txt_Degree);
      HTM_TH (1,1,"CM",Txt_Year_OF_A_DEGREE);
      HTM_TH (1,1,"LM",Txt_Course);
      HTM_TH (1,1,"RM",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
      HTM_TH (1,1,"RM",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NET]);
      HTM_TH (1,1,"RM",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);

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
   extern const char *Txt_Enrolment_confirmed;
   extern const char *Txt_Enrolment_not_confirmed;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   struct Deg_Degree Deg;
   long CrsCod;
   unsigned NumStds;
   unsigned NumNETs;
   unsigned NumTchs;
   unsigned NumUsrs;
   const char *ClassTxt;
   const char *ClassLink;
   const char *BgColor;
   bool Accepted;
   static unsigned RowEvenOdd = 1;
   /*
   SELECT deg_degrees.DegCod		row[0]
	  crs_courses.CrsCod		row[1]
	  deg_degrees.ShortName		row[2]
	  deg_degrees.FullName		row[3]
	  crs_courses.Year		row[4]
	  crs_courses.FullName		row[5]
	  ctr_centers.ShortName		row[6]
	  crs_users.Accepted		row[7]	(only if WriteColumnAccepted == true)
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
   NumStds = Usr_GetNumUsrsInCrss (Hie_Lvl_CRS,CrsCod,1 << Rol_STD);
   NumNETs = Usr_GetNumUsrsInCrss (Hie_Lvl_CRS,CrsCod,1 << Rol_NET);
   NumTchs = Usr_GetNumUsrsInCrss (Hie_Lvl_CRS,CrsCod,1 << Rol_TCH);
   NumUsrs = NumStds + NumNETs + NumTchs;
   if (NumUsrs)
     {
      ClassTxt  = "DAT_N";
      ClassLink = "BT_LINK LT DAT_N";
     }
   else
     {
      ClassTxt  = "DAT";
      ClassLink = "BT_LINK LT DAT";
     }
   BgColor = (CrsCod == Gbl.Hierarchy.Crs.CrsCod) ? "LIGHT_BLUE" :
                                                     Gbl.ColorRows[RowEvenOdd];

   /***** Start row *****/
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
   HTM_TD_Begin ("class=\"%s RT %s\"",ClassTxt,BgColor);
   HTM_Unsigned (NumCrs);
   HTM_TD_End ();

   /***** Write degree logo, degree short name (row[2])
          and center short name (row[6]) *****/
   HTM_TD_Begin ("class=\"LT %s\"",BgColor);
   Frm_BeginFormGoTo (ActSeeDegInf);
   Deg_PutParamDegCod (Deg.DegCod);
   HTM_BUTTON_SUBMIT_Begin (Hie_BuildGoToMsg (row[2]),ClassLink,NULL);
   Hie_FreeGoToMsg ();
   Lgo_DrawLogo (Hie_Lvl_DEG,Deg.DegCod,Deg.ShrtName,20,"CT",true);
   HTM_TxtF ("&nbsp;%s&nbsp;(%s)",row[2],row[6]);
   HTM_BUTTON_End ();
   Frm_EndForm ();
   HTM_TD_End ();

   /***** Write year (row[4]) *****/
   HTM_TD_Begin ("class=\"%s CT %s\"",ClassTxt,BgColor);
   HTM_Txt (Txt_YEAR_OF_DEGREE[Deg_ConvStrToYear (row[4])]);
   HTM_TD_End ();

   /***** Write course full name (row[5]) *****/
   HTM_TD_Begin ("class=\"LT %s\"",BgColor);
   Frm_BeginFormGoTo (ActSeeCrsInf);
   Crs_PutParamCrsCod (CrsCod);
   HTM_BUTTON_SUBMIT_Begin (Hie_BuildGoToMsg (row[5]),ClassLink,NULL);
   Hie_FreeGoToMsg ();
   HTM_Txt (row[5]);
   HTM_BUTTON_End ();
   Frm_EndForm ();
   HTM_TD_End ();

   /***** Write number of teachers in course *****/
   HTM_TD_Begin ("class=\"%s RT %s\"",ClassTxt,BgColor);
   HTM_Unsigned (NumTchs);
   HTM_TD_End ();

   /***** Write number of non-editing teachers in course *****/
   HTM_TD_Begin ("class=\"%s RT %s\"",ClassTxt,BgColor);
   HTM_Unsigned (NumNETs);
   HTM_TD_End ();

   /***** Write number of students in course *****/
   HTM_TD_Begin ("class=\"%s RT %s\"",ClassTxt,BgColor);
   HTM_Unsigned (NumStds);
   HTM_TD_End ();

   HTM_TR_End ();

   RowEvenOdd = 1 - RowEvenOdd;
  }

/*****************************************************************************/
/***************** Update my last click in current course ********************/
/*****************************************************************************/

void Crs_UpdateCrsLast (void)
  {
   if (Gbl.Hierarchy.Level == Hie_Lvl_CRS &&	// Course selected
       Gbl.Usrs.Me.Role.Logged >= Rol_STD)
      /***** Update my last access to current course *****/
      DB_QueryUPDATE ("can not update last access to current course",
		      "REPLACE INTO crs_last (CrsCod,LastTime)"
		      " VALUES (%ld,NOW())",
	              Gbl.Hierarchy.Crs.CrsCod);
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
				  "trash.svg",
				  Txt_Eliminate_old_courses);
  }

/*****************************************************************************/
/********************** Write form to remove old courses *********************/
/*****************************************************************************/

void Crs_AskRemoveOldCrss (void)
  {
   extern const char *Hlp_SYSTEM_Maintenance_eliminate_old_courses;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Eliminate_old_courses;
   extern const char *Txt_Eliminate_all_courses_whithout_users_PART_1_OF_2;
   extern const char *Txt_Eliminate_all_courses_whithout_users_PART_2_OF_2;
   extern const char *Txt_Eliminate;
   unsigned MonthsWithoutAccess = Crs_DEF_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS;
   unsigned i;

   /***** Begin form *****/
   Frm_BeginForm (ActRemOldCrs);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Eliminate_old_courses,
                 NULL,NULL,
                 Hlp_SYSTEM_Maintenance_eliminate_old_courses,Box_NOT_CLOSABLE);

   /***** Form to request number of months without clicks *****/
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s&nbsp;",Txt_Eliminate_all_courses_whithout_users_PART_1_OF_2);
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "name=\"Months\"");
   for (i  = Crs_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS;
        i <= Crs_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS;
        i++)
      HTM_OPTION (HTM_Type_UNSIGNED,&i,
		  i == MonthsWithoutAccess,false,
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
	                 Par_GetParToUnsignedLong ("Months",
                                                   Crs_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS,
                                                   Crs_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS,
                                                   UINT_MAX);
   if (MonthsWithoutAccess == UINT_MAX)
      Lay_ShowErrorAndExit ("Wrong number of months without clicks.");
   SecondsWithoutAccess = (unsigned long) MonthsWithoutAccess * Dat_SECONDS_IN_ONE_MONTH;

   /***** Get old courses from database *****/
   NumCrss = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get old courses",
		   "SELECT CrsCod"
		    " FROM crs_last"
		   " WHERE LastTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)"
		     " AND CrsCod NOT IN"
		         " (SELECT DISTINCT CrsCod"
			    " FROM crs_users)",
		   SecondsWithoutAccess);
   if (NumCrss)
     {
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

   /***** Write end message *****/
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
      Lay_ShowErrorAndExit ("Error initializing course.");

   /***** Allocate memory for course *****/
   if ((Crs_EditingCrs = malloc (sizeof (*Crs_EditingCrs))) == NULL)
      Lay_NotEnoughMemoryExit ();

   /***** Reset course *****/
   Crs_EditingCrs->CrsCod      = -1L;
   Crs_EditingCrs->InstitutionalCrsCod[0] = '\0';
   Crs_EditingCrs->DegCod      = -1L;
   Crs_EditingCrs->Year        = 0;
   Crs_EditingCrs->Status      = 0;
   Crs_EditingCrs->ShrtName[0] = '\0';
   Crs_EditingCrs->FullName[0] = '\0';
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
