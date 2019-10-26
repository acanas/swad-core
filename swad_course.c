// swad_course.c: edition of courses

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <limits.h>		// For maximum values
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For getenv, etc.
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_course.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_degree.h"
#include "swad_enrolment.h"
#include "swad_exam.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy.h"
#include "swad_HTML.h"
#include "swad_indicator.h"
#include "swad_logo.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_role.h"
#include "swad_RSS.h"
#include "swad_tab.h"
#include "swad_theme.h"

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

static struct Course *Crs_EditingCrs = NULL;	// Static variable to keep the course being edited

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Crs_Configuration (bool PrintView);
static void Crs_PutIconToPrint (void);
static void Crs_ShowNumUsrsInCrs (Rol_Role_t Role);

static void Crs_WriteListMyCoursesToSelectOne (void);

static void Crs_GetListCoursesInCurrentDegree (Crs_WhatCourses_t WhatCourses);
static void Crs_ListCourses (void);
static bool Crs_CheckIfICanCreateCourses (void);
static void Crs_PutIconsListCourses (void);
static void Crs_PutIconToEditCourses (void);
static bool Crs_ListCoursesOfAYearForSeeing (unsigned Year);

static void Crs_EditCoursesInternal (void);
static void Crs_PutIconsEditingCourses (void);
static void Crs_PutIconToViewCourses (void);
static void Crs_ListCoursesForEdition (void);
static void Crs_ListCoursesOfAYearForEdition (unsigned Year);
static bool Crs_CheckIfICanEdit (struct Course *Crs);
static Crs_StatusTxt_t Crs_GetStatusTxtFromStatusBits (Crs_Status_t Status);
static Crs_Status_t Crs_GetStatusBitsFromStatusTxt (Crs_StatusTxt_t StatusTxt);
static void Crs_PutFormToCreateCourse (void);
static void Crs_PutHeadCoursesForSeeing (void);
static void Crs_PutHeadCoursesForEdition (void);
static void Crs_RecFormRequestOrCreateCrs (unsigned Status);
static void Crs_GetParamsNewCourse (struct Course *Crs);

static void Crs_CreateCourse (unsigned Status);
static void Crs_GetDataOfCourseFromRow (struct Course *Crs,MYSQL_ROW row);

static void Crs_UpdateCrsDegDB (long CrsCod,long DegCod);

static void Crs_UpdateCrsYear (struct Course *Crs,unsigned NewYear);

static void Crs_GetShortNamesByCod (long CrsCod,
                                    char CrsShortName[Hie_MAX_BYTES_SHRT_NAME + 1],
                                    char DegShortName[Hie_MAX_BYTES_SHRT_NAME + 1]);

static void Crs_EmptyCourseCompletely (long CrsCod);

static void Crs_RenameCourse (struct Course *Crs,Cns_ShrtOrFullName_t ShrtOrFullName);
static bool Crs_CheckIfCrsNameExistsInYearOfDeg (const char *FieldName,const char *Name,long CrsCod,
                                                 long DegCod,unsigned Year);
static void Crs_UpdateCrsNameDB (long CrsCod,const char *FieldName,const char *NewCrsName);

static void Crs_PutButtonToGoToCrs (void);
static void Crs_PutButtonToRegisterInCrs (void);

static void Crs_PutIconToSearchCourses (void);
static void Sch_PutLinkToSearchCoursesParams (void);

static void Crs_PutParamOtherCrsCod (long CrsCod);
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
   Crs_Configuration (false);
   HTM_DIV_End ();

   /***** Course introduction *****/
   Inf_ShowInfo ();

   /***** Show help to enrol me *****/
   Hlp_ShowHelpWhatWouldYouLikeToDo ();
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
   extern const char *Hlp_COURSE_Information;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Degree;
   extern const char *Txt_Course;
   extern const char *Txt_Short_name;
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_Not_applicable;
   extern const char *Txt_Institutional_code;
   extern const char *Txt_Internal_code;
   extern const char *Txt_Shortcut;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_QR_code;
   extern const char *Txt_Indicators;
   extern const char *Txt_of_PART_OF_A_TOTAL;
   unsigned NumDeg;
   unsigned Year;
   int NumIndicatorsFromDB;
   struct Ind_IndicatorsCrs Indicators;
   bool IsForm;
   bool PutLink;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Crs.CrsCod <= 0)	// No course selected
      return;

   /***** Contextual menu *****/
   if (!PrintView)
      if (Gbl.Usrs.Me.Role.Logged == Rol_GST ||
	  Gbl.Usrs.Me.Role.Logged == Rol_USR)
	{
         Mnu_ContextMenuBegin ();
         Enr_PutLinkToRequestSignUp ();	// Request enrolment in the current course
         Mnu_ContextMenuEnd ();
	}

   /***** Begin box *****/
   if (PrintView)
      Box_BoxBegin (NULL,NULL,NULL,
		    NULL,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,NULL,Crs_PutIconToPrint,
		    Hlp_COURSE_Information,Box_NOT_CLOSABLE);

   /***** Title *****/
   PutLink = !PrintView && Gbl.Hierarchy.Deg.WWW[0];
   HTM_DIV_Begin ("class=\"FRAME_TITLE FRAME_TITLE_BIG\"");
   if (PutLink)
      fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\""
	                 " class=\"FRAME_TITLE_BIG\" title=\"%s\">",
	       Gbl.Hierarchy.Deg.WWW,
	       Gbl.Hierarchy.Deg.FullName);
   Log_DrawLogo (Hie_DEG,Gbl.Hierarchy.Deg.DegCod,
                 Gbl.Hierarchy.Deg.ShrtName,64,NULL,true);
   if (PutLink)
      fprintf (Gbl.F.Out,"</a>");
   fprintf (Gbl.F.Out,"<br />%s",
            Gbl.Hierarchy.Crs.FullName);
   HTM_DIV_End ();

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

   /***** Degree *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   fprintf (Gbl.F.Out,"<label for=\"OthDegCod\" class=\"%s\">%s:</label>",
            The_ClassFormInBox[Gbl.Prefs.Theme],
            Txt_Degree);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LM\"");

   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM)
      // Only centre admins, institution admins and system admin can move a course to another degree
     {
      /* Get list of degrees of the current centre */
      Deg_GetListDegsOfCurrentCtr ();

      /* Put form to select degree */
      Frm_StartForm (ActChgCrsDegCfg);
      fprintf (Gbl.F.Out,"<select id=\"OthDegCod\" name=\"OthDegCod\""
			 " class=\"INPUT_SHORT_NAME\""
			 " onchange=\"document.getElementById('%s').submit();\">",
	       Gbl.Form.Id);
      for (NumDeg = 0;
	   NumDeg < Gbl.Hierarchy.Ctr.Degs.Num;
	   NumDeg++)
	 fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s</option>",
		  Gbl.Hierarchy.Ctr.Degs.Lst[NumDeg].DegCod,
		  Gbl.Hierarchy.Ctr.Degs.Lst[NumDeg].DegCod == Gbl.Hierarchy.Deg.DegCod ? " selected=\"selected\"" :
										          "",
		  Gbl.Hierarchy.Ctr.Degs.Lst[NumDeg].ShrtName);
      fprintf (Gbl.F.Out,"</select>");
      Frm_EndForm ();

      /* Free list of degrees of the current centre */
      Deg_FreeListDegs (&Gbl.Hierarchy.Ctr.Degs);
     }
   else	// I can not move course to another degree
      fprintf (Gbl.F.Out,"%s",Gbl.Hierarchy.Deg.FullName);

   HTM_TD_End ();

   HTM_TR_End ();

   /***** Course full name *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   fprintf (Gbl.F.Out,"<label for=\"FullName\" class=\"%s\">%s:</label>",
	    The_ClassFormInBox[Gbl.Prefs.Theme],
	    Txt_Course);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT_N LM\"");
   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
      // Only degree admins, centre admins, institution admins and system admins can edit course full name
     {
      /* Form to change course full name */
      Frm_StartForm (ActRenCrsFulCfg);
      fprintf (Gbl.F.Out,"<input type=\"text\""
	                 " id=\"FullName\" name=\"FullName\""
			 " maxlength=\"%u\" value=\"%s\""
			 " class=\"INPUT_FULL_NAME\""
			 " onchange=\"document.getElementById('%s').submit();\" />",
	       Hie_MAX_CHARS_FULL_NAME,
	       Gbl.Hierarchy.Crs.FullName,
	       Gbl.Form.Id);
      Frm_EndForm ();
     }
   else	// I can not edit course full name
      fprintf (Gbl.F.Out,"%s",Gbl.Hierarchy.Crs.FullName);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Course short name *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   fprintf (Gbl.F.Out,"<label for=\"ShortName\" class=\"%s\">%s:</label>",
	    The_ClassFormInBox[Gbl.Prefs.Theme],
	    Txt_Short_name);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT_N LM\"");
   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
      // Only degree admins, centre admins, institution admins and system admins can edit course short name
     {
      /* Form to change course short name */
      Frm_StartForm (ActRenCrsShoCfg);
      fprintf (Gbl.F.Out,"<input type=\"text\""
	                 " id=\"ShortName\" name=\"ShortName\""
			 " maxlength=\"%u\" value=\"%s\""
			 " class=\"INPUT_SHORT_NAME\""
			 " onchange=\"document.getElementById('%s').submit();\" />",
	       Hie_MAX_CHARS_SHRT_NAME,
	       Gbl.Hierarchy.Crs.ShrtName,
	       Gbl.Form.Id);
      Frm_EndForm ();
     }
   else	// I can not edit course short name
      fprintf (Gbl.F.Out,"%s",Gbl.Hierarchy.Crs.ShrtName);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Course year *****/
   IsForm = (!PrintView && Gbl.Usrs.Me.Role.Logged >= Rol_TCH);
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   fprintf (Gbl.F.Out,"<label for=\"OthCrsYear\" class=\"%s\">%s:</label>",
            The_ClassFormInBox[Gbl.Prefs.Theme],
            Txt_Year_OF_A_DEGREE);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LM\"");
   if (IsForm)
     {
      Frm_StartForm (ActChgCrsYeaCfg);
      fprintf (Gbl.F.Out,"<select id=\"OthCrsYear\" name=\"OthCrsYear\""
	                 " onchange=\"document.getElementById('%s').submit();\">",
               Gbl.Form.Id);
      for (Year = 0;
	   Year <= Deg_MAX_YEARS_PER_DEGREE;
           Year++)
	 fprintf (Gbl.F.Out,"<option value=\"%u\"%s>%s</option>",
		  Year,
		  Year == Gbl.Hierarchy.Crs.Year ? " selected=\"selected\"" :
						    "",
		  Txt_YEAR_OF_DEGREE[Year]);
      fprintf (Gbl.F.Out,"</select>");
      Frm_EndForm ();
     }
   else
      fprintf (Gbl.F.Out,"%s",
               Gbl.Hierarchy.Crs.Year ? Txt_YEAR_OF_DEGREE[Gbl.Hierarchy.Crs.Year] :
	                                 Txt_Not_applicable);
   HTM_TD_End ();

   HTM_TR_End ();

   if (!PrintView)
     {
      /***** Institutional code of the course *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"RM\"");
      fprintf (Gbl.F.Out,"<label for=\"InsCrsCod\" class=\"%s\">%s:</label>",
              The_ClassFormInBox[Gbl.Prefs.Theme],
              Txt_Institutional_code);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LM\"");
      if (IsForm)
	{
         Frm_StartForm (ActChgInsCrsCodCfg);
         fprintf (Gbl.F.Out,"<input type=\"text\""
                            " id=\"InsCrsCod\" name=\"InsCrsCod\""
                            " size=\"%u\" maxlength=\"%u\" value=\"%s\""
	                    " onchange=\"document.getElementById('%s').submit();\" />",
                  Crs_MAX_CHARS_INSTITUTIONAL_CRS_COD,
                  Crs_MAX_CHARS_INSTITUTIONAL_CRS_COD,
                  Gbl.Hierarchy.Crs.InstitutionalCrsCod,
                  Gbl.Form.Id);
         Frm_EndForm ();
	}
      else
         fprintf (Gbl.F.Out,"%s",Gbl.Hierarchy.Crs.InstitutionalCrsCod);
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Internal code of the course *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"%s:",Txt_Internal_code);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LM\"");
      fprintf (Gbl.F.Out,"%ld",Gbl.Hierarchy.Crs.CrsCod);
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** Link to the course *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s:",Txt_Shortcut);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LM\"");
   fprintf (Gbl.F.Out,"<a href=\"%s/%s?crs=%ld\" class=\"DAT\" target=\"_blank\">"
                      "%s/%s?crs=%ld</a>",
            Cfg_URL_SWAD_CGI,
            Lan_STR_LANG_ID[Gbl.Prefs.Language],
            Gbl.Hierarchy.Crs.CrsCod,
            Cfg_URL_SWAD_CGI,
            Lan_STR_LANG_ID[Gbl.Prefs.Language],
            Gbl.Hierarchy.Crs.CrsCod);
   HTM_TD_End ();

   HTM_TR_End ();

   if (PrintView)
     {
      /***** QR code with link to the course *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"%s:",Txt_QR_code);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LM\"");
      QR_LinkTo (250,"crs",Gbl.Hierarchy.Crs.CrsCod);
      HTM_TD_End ();

      HTM_TR_End ();
     }
   else
     {
      /***** Number of users *****/
      Crs_ShowNumUsrsInCrs (Rol_TCH);
      Crs_ShowNumUsrsInCrs (Rol_NET);
      Crs_ShowNumUsrsInCrs (Rol_STD);

      /***** Indicators *****/
      NumIndicatorsFromDB = Ind_GetNumIndicatorsCrsFromDB (Gbl.Hierarchy.Crs.CrsCod);
      Ind_ComputeAndStoreIndicatorsCrs (Gbl.Hierarchy.Crs.CrsCod,
                                        NumIndicatorsFromDB,&Indicators);
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"%s:",Txt_Indicators);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartForm (ActReqStaCrs);
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        "%u %s %u",
                Indicators.NumIndicators,
		Txt_of_PART_OF_A_TOTAL,Ind_NUM_INDICATORS);
      Frm_LinkFormSubmit (Gbl.Title,"DAT",NULL);
      fprintf (Gbl.F.Out,"%s "
                         "<img src=\"%s/%s\" alt=\"%s\" class=\"ICO16x16\" />",
               Gbl.Title,
               Cfg_URL_ICON_PUBLIC,
               (Indicators.NumIndicators == Ind_NUM_INDICATORS) ? "check-circle.svg" :
        	                                                  "exclamation-triangle.svg",
               Gbl.Title);
      Frm_EndForm ();
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************* Put icon to print the configuration of a course ***************/
/*****************************************************************************/

static void Crs_PutIconToPrint (void)
  {
   Ico_PutContextualIconToPrint (ActPrnCrsInf,NULL);
  }

/*****************************************************************************/
/**************** Number of users in courses of this country *****************/
/*****************************************************************************/

static void Crs_ShowNumUsrsInCrs (Rol_Role_t Role)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s:",Txt_ROLES_PLURAL_Abc[Role][Usr_SEX_UNKNOWN]);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LM\"");
   fprintf (Gbl.F.Out,"%u",Gbl.Hierarchy.Crs.NumUsrs[Role]);
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************************ Write menu with my courses *************************/
/*****************************************************************************/

#define Crs_MAX_BYTES_TXT_LINK 40

static void Crs_WriteListMyCoursesToSelectOne (void)
  {
   extern const char *Hlp_PROFILE_Courses;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *The_ClassFormInBoxBold[The_NUM_THEMES];
   extern const char *Txt_My_courses;
   extern const char *Txt_System;
   extern const char *Txt_Go_to_X;
   struct Country Cty;
   struct Instit Ins;
   struct Centre Ctr;
   struct Degree Deg;
   struct Course Crs;
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
   char ActTxt[Act_MAX_BYTES_ACTION_TXT + 1];
   const char *ClassNormal;
   char ClassHighlight[64];

   ClassNormal = The_ClassFormInBox[Gbl.Prefs.Theme];
   snprintf (ClassHighlight,sizeof (ClassHighlight),
	     "%s LIGHT_BLUE",
	     The_ClassFormInBoxBold[Gbl.Prefs.Theme]);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_My_courses,Crs_PutIconToSearchCourses,
                 Hlp_PROFILE_Courses,Box_NOT_CLOSABLE);
   HTM_UL_Begin ("class=\"LIST_TREE\"");

   /***** Write link to platform *****/
   Highlight = (Gbl.Hierarchy.Cty.CtyCod <= 0);
   HTM_LI_Begin ("class=\"%s\"",Highlight ? ClassHighlight :
			                    ClassNormal);
   Frm_StartForm (ActMyCrs);
   Cty_PutParamCtyCod (-1L);
   Frm_LinkFormSubmit (Txt_System,
                       Highlight ? ClassHighlight :
        	                   ClassNormal,
		       NULL);
   fprintf (Gbl.F.Out,"<img src=\"%s/sitemap.svg\""
	              " alt=\"%s\" title=\"%s\""
                      " class=\"ICO16x16\" />&nbsp;%s</a>",
	    Cfg_URL_ICON_PUBLIC,
	    Txt_System,
	    Txt_System,
	    Txt_System);
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
      Cty.CtyCod = Str_ConvertStrCodToLongCod (row[0]);
      if (!Cty_GetDataOfCountryByCod (&Cty,Cty_GET_BASIC_DATA))
	 Lay_ShowErrorAndExit ("Country not found.");

      /***** Write link to country *****/
      Highlight = (Gbl.Hierarchy.Ins.InsCod <= 0 &&
	           Gbl.Hierarchy.Cty.CtyCod == Cty.CtyCod);
      HTM_LI_Begin ("class=\"%s\"",Highlight ? ClassHighlight :
			                       ClassNormal);
      IsLastItemInLevel[1] = (NumCty == NumCtys - 1);
      Lay_IndentDependingOnLevel (1,IsLastItemInLevel);
      Frm_StartForm (ActMyCrs);
      Cty_PutParamCtyCod (Cty.CtyCod);
      Frm_LinkFormSubmit (Act_GetActionTextFromDB (Act_GetActCod (ActSeeCtyInf),ActTxt),
			  Highlight ? ClassHighlight :
        	                      ClassNormal,NULL);
      /* Country map */
      fprintf (Gbl.F.Out,"<img src=\"%s/%s/%s.png\""
	                 " alt=\"%s\" title=\"%s\""
                         " class=\"ICO16x16\" />&nbsp;%s</a>",
	       Cfg_URL_ICON_COUNTRIES_PUBLIC,
	       Cty.Alpha2,
	       Cty.Alpha2,
	       Cty.Alpha2,
	       Cty.Name[Gbl.Prefs.Language],
               Cty.Name[Gbl.Prefs.Language]);
      Frm_EndForm ();
      HTM_LI_End ();

      /***** Get my institutions in this country *****/
      NumInss = (unsigned) Usr_GetInssFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,
                                               Cty.CtyCod,&mysql_resIns);
      for (NumIns = 0;
	   NumIns < NumInss;
	   NumIns++)
	{
	 /***** Get next institution *****/
	 row = mysql_fetch_row (mysql_resIns);

	 /***** Get data of this institution *****/
	 Ins.InsCod = Str_ConvertStrCodToLongCod (row[0]);
	 if (!Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA))
	    Lay_ShowErrorAndExit ("Institution not found.");

	 /***** Write link to institution *****/
	 Highlight = (Gbl.Hierarchy.Ctr.CtrCod <= 0 &&
	              Gbl.Hierarchy.Ins.InsCod == Ins.InsCod);
	 HTM_LI_Begin ("class=\"%s\"",Highlight ? ClassHighlight :
			                          ClassNormal);
	 IsLastItemInLevel[2] = (NumIns == NumInss - 1);
	 Lay_IndentDependingOnLevel (2,IsLastItemInLevel);
         Frm_StartForm (ActMyCrs);
	 Ins_PutParamInsCod (Ins.InsCod);
	 Frm_LinkFormSubmit (Act_GetActionTextFromDB (Act_GetActCod (ActSeeInsInf),ActTxt),
	                     Highlight ? ClassHighlight :
        	                         ClassNormal,NULL);
	 Log_DrawLogo (Hie_INS,Ins.InsCod,Ins.ShrtName,16,NULL,true);
	 fprintf (Gbl.F.Out,"&nbsp;%s</a>",Ins.FullName);
	 Frm_EndForm ();
	 HTM_LI_End ();

	 /***** Get my centres in this institution *****/
	 NumCtrs = (unsigned) Usr_GetCtrsFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,
	                                          Ins.InsCod,&mysql_resCtr);
	 for (NumCtr = 0;
	      NumCtr < NumCtrs;
	      NumCtr++)
	   {
	    /***** Get next centre *****/
	    row = mysql_fetch_row (mysql_resCtr);

	    /***** Get data of this centre *****/
	    Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[0]);
	    if (!Ctr_GetDataOfCentreByCod (&Ctr))
	       Lay_ShowErrorAndExit ("Centre not found.");

	    /***** Write link to centre *****/
	    Highlight = (Gbl.Hierarchy.Level == Hie_CTR &&
			 Gbl.Hierarchy.Ctr.CtrCod == Ctr.CtrCod);
	    HTM_LI_Begin ("class=\"%s\"",Highlight ? ClassHighlight :
			                             ClassNormal);
	    IsLastItemInLevel[3] = (NumCtr == NumCtrs - 1);
	    Lay_IndentDependingOnLevel (3,IsLastItemInLevel);
            Frm_StartForm (ActMyCrs);
	    Ctr_PutParamCtrCod (Ctr.CtrCod);
	    Frm_LinkFormSubmit (Act_GetActionTextFromDB (Act_GetActCod (ActSeeCtrInf),ActTxt),
	                        Highlight ? ClassHighlight :
        	                            ClassNormal,NULL);
	    Log_DrawLogo (Hie_CTR,Ctr.CtrCod,Ctr.ShrtName,16,NULL,true);
	    fprintf (Gbl.F.Out,"&nbsp;%s</a>",Ctr.FullName);
	    Frm_EndForm ();
	    HTM_LI_End ();

	    /***** Get my degrees in this centre *****/
	    NumDegs = (unsigned) Usr_GetDegsFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,
	                                             Ctr.CtrCod,&mysql_resDeg);
	    for (NumDeg = 0;
		 NumDeg < NumDegs;
		 NumDeg++)
	      {
	       /***** Get next degree *****/
	       row = mysql_fetch_row (mysql_resDeg);

	       /***** Get data of this degree *****/
	       Deg.DegCod = Str_ConvertStrCodToLongCod (row[0]);
	       if (!Deg_GetDataOfDegreeByCod (&Deg))
		  Lay_ShowErrorAndExit ("Degree not found.");

	       /***** Write link to degree *****/
	       Highlight = (Gbl.Hierarchy.Level == Hie_DEG &&
			    Gbl.Hierarchy.Deg.DegCod == Deg.DegCod);
	       HTM_LI_Begin ("class=\"%s\"",Highlight ? ClassHighlight :
			                                ClassNormal);
	       IsLastItemInLevel[4] = (NumDeg == NumDegs - 1);
	       Lay_IndentDependingOnLevel (4,IsLastItemInLevel);
               Frm_StartForm (ActMyCrs);
	       Deg_PutParamDegCod (Deg.DegCod);
	       Frm_LinkFormSubmit (Act_GetActionTextFromDB (Act_GetActCod (ActSeeDegInf),ActTxt),
	                           Highlight ? ClassHighlight :
        	                               ClassNormal,NULL);
	       Log_DrawLogo (Hie_DEG,Deg.DegCod,Deg.ShrtName,16,NULL,true);
	       fprintf (Gbl.F.Out,"&nbsp;%s</a>",Deg.FullName);
	       Frm_EndForm ();
	       HTM_LI_End ();

	       /***** Get my courses in this degree *****/
	       NumCrss = (unsigned) Usr_GetCrssFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,
	                                                Deg.DegCod,&mysql_resCrs);
	       for (NumCrs = 0;
		    NumCrs < NumCrss;
		    NumCrs++)
		 {
		  /***** Get next course *****/
		  row = mysql_fetch_row (mysql_resCrs);

		  /***** Get data of this course *****/
		  Crs.CrsCod = Str_ConvertStrCodToLongCod (row[0]);
		  if (!Crs_GetDataOfCourseByCod (&Crs))
		     Lay_ShowErrorAndExit ("Course not found.");

		  /***** Write link to course *****/
		  Highlight = (Gbl.Hierarchy.Level == Hie_CRS &&
			       Gbl.Hierarchy.Crs.CrsCod == Crs.CrsCod);
		  HTM_LI_Begin ("class=\"%s\"",Highlight ? ClassHighlight :
			                                   ClassNormal);
		  IsLastItemInLevel[5] = (NumCrs == NumCrss - 1);
		  Lay_IndentDependingOnLevel (5,IsLastItemInLevel);
                  Frm_StartForm (ActMyCrs);
		  Crs_PutParamCrsCod (Crs.CrsCod);
		  snprintf (Gbl.Title,sizeof (Gbl.Title),
			    Txt_Go_to_X,
			    Crs.ShrtName);
		  Frm_LinkFormSubmit (Gbl.Title,
		                      Highlight ? ClassHighlight :
        	                                  ClassNormal,NULL);
		  fprintf (Gbl.F.Out,"<img src=\"%s/list-ol.svg\""
			             " alt=\"%s\" title=\"%s\""
			             " class=\"ICO16x16\" />"
			             "&nbsp;%s"
			             "</a>",
		           Cfg_URL_ICON_PUBLIC,
		           Crs.ShrtName,
		           Crs.FullName,
		           Crs.FullName);
		  Frm_EndForm ();

		  /***** Put link to register students *****/
		  Enr_PutButtonInlineToRegisterStds (Crs.CrsCod);

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

unsigned Crs_GetNumCrssTotal (void)
  {
   /***** Get total number of courses from database *****/
   return (unsigned) DB_GetNumRowsTable ("courses");
  }

/*****************************************************************************/
/****************** Get number of courses in a country ***********************/
/*****************************************************************************/

unsigned Crs_GetNumCrssInCty (long CtyCod)
  {
   /***** Get number of courses in a country from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get the number of courses in a country",
			     "SELECT COUNT(*)"
			     " FROM institutions,centres,degrees,courses"
			     " WHERE institutions.CtyCod=%ld"
			     " AND institutions.InsCod=centres.InsCod"
			     " AND centres.CtrCod=degrees.CtrCod"
			     " AND degrees.DegCod=courses.DegCod",
			     CtyCod);
  }

/*****************************************************************************/
/**************** Get number of courses in an institution ********************/
/*****************************************************************************/

unsigned Crs_GetNumCrssInIns (long InsCod)
  {
   /***** Get number of courses in a degree from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get the number of courses"
			     " in an institution",
			     "SELECT COUNT(*) FROM centres,degrees,courses"
			     " WHERE centres.InsCod=%ld"
			     " AND centres.CtrCod=degrees.CtrCod"
			     " AND degrees.DegCod=courses.DegCod",
			     InsCod);
  }

/*****************************************************************************/
/******************** Get number of courses in a centre **********************/
/*****************************************************************************/

unsigned Crs_GetNumCrssInCtr (long CtrCod)
  {
   /***** Get number of courses in a degree from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get the number of courses in a centre",
			     "SELECT COUNT(*) FROM degrees,courses"
			     " WHERE degrees.CtrCod=%ld"
			     " AND degrees.DegCod=courses.DegCod",
			     CtrCod);
  }

/*****************************************************************************/
/******************** Get number of courses in a degree **********************/
/*****************************************************************************/

unsigned Crs_GetNumCrssInDeg (long DegCod)
  {
   /***** Get number of courses in a degree from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get the number of courses in a degree",
			     "SELECT COUNT(*) FROM courses"
			     " WHERE DegCod=%ld",
			     DegCod);
  }

/*****************************************************************************/
/********************* Get number of courses with users **********************/
/*****************************************************************************/

unsigned Crs_GetNumCrssWithUsrs (Rol_Role_t Role,const char *SubQuery)
  {
   /***** Get number of degrees with users from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of courses with users",
			     "SELECT COUNT(DISTINCT courses.CrsCod)"
			     " FROM institutions,centres,degrees,courses,crs_usr"
			     " WHERE %sinstitutions.InsCod=centres.InsCod"
			     " AND centres.CtrCod=degrees.CtrCod"
			     " AND degrees.DegCod=courses.DegCod"
			     " AND courses.CrsCod=crs_usr.CrsCod"
			     " AND crs_usr.Role=%u",
			     SubQuery,(unsigned) Role);
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
   Frm_StartFormGoTo (ActSeeCrsInf);
   fprintf (Gbl.F.Out,"<select id=\"crs\" name=\"crs\" class=\"HIE_SEL\"");
   if (Gbl.Hierarchy.Deg.DegCod > 0)
      fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
               Gbl.Form.Id);
   else
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,">"
	              "<option value=\"\"");
   if (Gbl.Hierarchy.Crs.CrsCod < 0)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out," disabled=\"disabled\">[%s]</option>",
            Txt_Course);

   if (Gbl.Hierarchy.Deg.DegCod > 0)
     {
      /***** Get courses belonging to the current degree from database *****/
      NumCrss = (unsigned) DB_QuerySELECT (&mysql_res,"can not get courses"
						      " of a degree",
					   "SELECT CrsCod,ShortName FROM courses"
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
         fprintf (Gbl.F.Out,"<option value=\"%ld\"",CrsCod);
         if (Gbl.Hierarchy.Level == Hie_CRS &&	// Course selected
             (CrsCod == Gbl.Hierarchy.Crs.CrsCod))
	    fprintf (Gbl.F.Out," selected=\"selected\"");
         fprintf (Gbl.F.Out,">%s</option>",row[1]);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** End form *****/
   fprintf (Gbl.F.Out,"</select>");
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
   Crs_GetListCoursesInCurrentDegree (Crs_ALL_COURSES_EXCEPT_REMOVED);

   /***** Write menu to select country, institution, centre and degree *****/
   Hie_WriteMenuHierarchy ();

   /***** Show list of courses *****/
   Crs_ListCourses ();

   /***** Free list of courses in this degree *****/
   Crs_FreeListCoursesInCurrentDegree ();
  }

/*****************************************************************************/
/*************** Create a list with courses in current degree ****************/
/*****************************************************************************/

static void Crs_GetListCoursesInCurrentDegree (Crs_WhatCourses_t WhatCourses)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   struct Course *Crs;

   /***** Get courses of a degree from database *****/
   switch (WhatCourses)
     {
      case Crs_ACTIVE_COURSES:
         NumCrss = (unsigned) DB_QuerySELECT (&mysql_res,"can not get courses"
							 " of a degree",
					      "SELECT CrsCod,DegCod,Year,InsCrsCod,Status,RequesterUsrCod,ShortName,FullName"
					      " FROM courses WHERE DegCod=%ld AND Status=0"
					      " ORDER BY Year,ShortName",
					      Gbl.Hierarchy.Deg.DegCod);
         break;
      case Crs_ALL_COURSES_EXCEPT_REMOVED:
         NumCrss = (unsigned) DB_QuerySELECT (&mysql_res,"can not get courses"
							 " of a degree",
					      "SELECT CrsCod,DegCod,Year,InsCrsCod,Status,RequesterUsrCod,ShortName,FullName"
					      " FROM courses WHERE DegCod=%ld AND (Status & %u)=0"
					      " ORDER BY Year,ShortName",
					      Gbl.Hierarchy.Deg.DegCod,
					      (unsigned) Crs_STATUS_BIT_REMOVED);
         break;
      default:
	 break;
     }
   if (NumCrss) // Courses found...
     {
      /***** Create list with courses in degree *****/
      if ((Gbl.Hierarchy.Deg.Crss.Lst = (struct Course *) calloc ((size_t) NumCrss,
	                                                         sizeof (struct Course))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the courses in degree *****/
      for (NumCrs = 0;
	   NumCrs < NumCrss;
	   NumCrs++)
        {
         Crs = &(Gbl.Hierarchy.Deg.Crss.Lst[NumCrs]);

         /* Get next course */
         row = mysql_fetch_row (mysql_res);
         Crs_GetDataOfCourseFromRow (Crs,row);
        }
     }

   Gbl.Hierarchy.Deg.Crss.Num = NumCrss;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Free list of courses in this degree *******************/
/*****************************************************************************/

void Crs_FreeListCoursesInCurrentDegree (void)
  {
   if (Gbl.Hierarchy.Deg.Crss.Lst)
     {
      /***** Free memory used by the list of courses in degree *****/
      free ((void *) Gbl.Hierarchy.Deg.Crss.Lst);
      Gbl.Hierarchy.Deg.Crss.Lst = NULL;
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
   char CrsShortName[Hie_MAX_BYTES_SHRT_NAME + 1];
   char DegShortName[Hie_MAX_BYTES_SHRT_NAME + 1];

   /***** Fill the list with the courses I belong to, if not filled *****/
   if (Gbl.Usrs.Me.Logged)
      Usr_GetMyCourses ();

   /***** Begin form *****/
   Frm_StartFormGoTo (Gbl.Usrs.Me.MyCrss.Num ? ActSeeCrsInf :
                                               ActReqSch);

   /***** Start selector of courses *****/
   fprintf (Gbl.F.Out,"<select id=\"my_courses\" name=\"crs\""
                      " onchange=\"document.getElementById('%s').submit();\">",
            Gbl.Form.Id);

   /***** Write an option when no course selected *****/
   if (Gbl.Hierarchy.Crs.CrsCod <= 0)	// No course selected
      fprintf (Gbl.F.Out,"<option value=\"-1\""
	                 " disabled=\"disabled\" selected=\"selected\">"
			 "%s"
			 "</option>",
	       Txt_Course);

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
	       fprintf (Gbl.F.Out,"</optgroup>");
	    fprintf (Gbl.F.Out,"<optgroup label=\"%s\">",DegShortName);
	    LastDegCod = DegCod;
	   }

         fprintf (Gbl.F.Out,"<option value=\"%ld\"",
                  Gbl.Usrs.Me.MyCrss.Crss[NumMyCrs].CrsCod);
         if (CrsCod == Gbl.Hierarchy.Crs.CrsCod)	// Course selected
            fprintf (Gbl.F.Out," selected=\"selected\"");
         fprintf (Gbl.F.Out,">%s</option>",CrsShortName);
        }

      if (LastDegCod > 0)
	 fprintf (Gbl.F.Out,"</optgroup>");
     }

   /***** Write an option with the current course
          when I don't belong to it *****/
   if (Gbl.Hierarchy.Level == Hie_CRS &&	// Course selected
       !Gbl.Usrs.Me.IBelongToCurrentCrs)	// I do not belong to it
      fprintf (Gbl.F.Out,"<option value=\"%ld\""
	                 " disabled=\"disabled\" selected=\"selected\">"
			 "%s"
			 "</option>",
	       Gbl.Hierarchy.Crs.CrsCod,
	       Gbl.Hierarchy.Crs.ShrtName);

   /***** End form *****/
   fprintf (Gbl.F.Out,"</select>");
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
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Courses_of_DEGREE_X,
	     Gbl.Hierarchy.Deg.ShrtName);
   Box_BoxBegin (NULL,Gbl.Title,Crs_PutIconsListCourses,
                 Hlp_DEGREE_Courses,Box_NOT_CLOSABLE);

   if (Gbl.Hierarchy.Deg.Crss.Num)	// There are courses in the current degree
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
      Frm_StartForm (ActEdiCrs);
      Btn_PutConfirmButton (Gbl.Hierarchy.Deg.Crss.Num ? Txt_Create_another_course :
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

static void Crs_PutIconsListCourses (void)
  {
   /***** Put icon to edit courses *****/
   if (Crs_CheckIfICanCreateCourses ())
      Crs_PutIconToEditCourses ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_HIERARCHY;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/************************* Put icon to edit courses **************************/
/*****************************************************************************/

static void Crs_PutIconToEditCourses (void)
  {
   Ico_PutContextualIconToEdit (ActEdiCrs,NULL);
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
   extern const char *Txt_Go_to_X;
   extern const char *Txt_COURSE_STATUS[Crs_NUM_STATUS_TXT];
   unsigned NumCrs;
   struct Course *Crs;
   const char *TxtClassNormal;
   const char *TxtClassStrong;
   const char *BgColor;
   Crs_StatusTxt_t StatusTxt;
   bool ThisYearHasCourses = false;

   /***** Write all the courses of this year *****/
   for (NumCrs = 0;
	NumCrs < Gbl.Hierarchy.Deg.Crss.Num;
	NumCrs++)
     {
      Crs = &(Gbl.Hierarchy.Deg.Crss.Lst[NumCrs]);
      if (Crs->Year == Year)	// The year of the course is this?
	{
	 ThisYearHasCourses = true;
	 if (Crs->Status & Crs_STATUS_BIT_PENDING)
	   {
	    TxtClassNormal = "DAT_LIGHT";
	    TxtClassStrong = "DAT_LIGHT";
	   }
	 else
	   {
	    TxtClassNormal = "DAT";
	    TxtClassStrong = "DAT_N";
	   }

	 /* Check if this course is one of my courses */
	 BgColor = (Usr_CheckIfIBelongToCrs (Crs->CrsCod)) ? "LIGHT_BLUE" :
				                             Gbl.ColorRows[Gbl.RowEvenOdd];

	 HTM_TR_Begin (NULL);

	 /* Put green tip if course has users */
	 HTM_TD_Begin ("class=\"%s CM %s\" title=\"%s\"",
		       TxtClassNormal,BgColor,
		       Crs->NumUsrs[Rol_UNK] ? Txt_COURSE_With_users :
					       Txt_COURSE_Without_users);
	 fprintf (Gbl.F.Out,"%s",Crs->NumUsrs[Rol_UNK] ? "&check;" :
				                         "&nbsp;");
	 HTM_TD_End ();

	 /* Institutional code of the course */
	 HTM_TD_Begin ("class=\"%s CM %s\"",
		            TxtClassNormal,BgColor);
	 fprintf (Gbl.F.Out,"%s",Crs->InstitutionalCrsCod);
	 HTM_TD_End ();

	 /* Course year */
	 HTM_TD_Begin ("class=\"%s CM %s\"",
		            TxtClassNormal,BgColor);
	 fprintf (Gbl.F.Out,"%s",Txt_YEAR_OF_DEGREE[Crs->Year]);
	 HTM_TD_End ();

	 /* Course full name */
	 HTM_TD_Begin ("class=\"%s LM %s\"",TxtClassStrong,BgColor);
	 Frm_StartFormGoTo (ActSeeCrsInf);
	 Crs_PutParamCrsCod (Crs->CrsCod);
	 snprintf (Gbl.Title,sizeof (Gbl.Title),
	           Txt_Go_to_X,
		   Crs->FullName);
	 Frm_LinkFormSubmit (Gbl.Title,TxtClassStrong,NULL);
	 fprintf (Gbl.F.Out,"%s</a>",
		  Crs->FullName);
	 Frm_EndForm ();
	 HTM_TD_End ();

	 /* Current number of teachers in this course */
	 HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
	 fprintf (Gbl.F.Out,"%u",Crs->NumUsrs[Rol_TCH] +
		                 Crs->NumUsrs[Rol_NET]);
	 HTM_TD_End ();

	 /* Current number of students in this course */
	 HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
	 fprintf (Gbl.F.Out,"%u",Crs->NumUsrs[Rol_STD]);
	 HTM_TD_End ();

	 /* Course status */
	 StatusTxt = Crs_GetStatusTxtFromStatusBits (Crs->Status);
	 HTM_TD_Begin ("class=\"%s LM %s\"",TxtClassNormal,BgColor);
	 if (StatusTxt != Crs_STATUS_ACTIVE) // If active ==> do not show anything
	    fprintf (Gbl.F.Out,"%s",Txt_COURSE_STATUS[StatusTxt]);
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

   /***** Get list of degrees in this centre *****/
   Deg_GetListDegsOfCurrentCtr ();

   /***** Get list of courses in this degree *****/
   Crs_GetListCoursesInCurrentDegree (Crs_ALL_COURSES_EXCEPT_REMOVED);

   /***** Write menu to select country, institution, centre and degree *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Courses_of_DEGREE_X,
	     Gbl.Hierarchy.Deg.ShrtName);
   Box_BoxBegin (NULL,Gbl.Title,Crs_PutIconsEditingCourses,
                 Hlp_DEGREE_Courses,Box_NOT_CLOSABLE);

   /***** Put a form to create or request a new course *****/
   Crs_PutFormToCreateCourse ();

   /***** Forms to edit current courses *****/
   if (Gbl.Hierarchy.Deg.Crss.Num)
      Crs_ListCoursesForEdition ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of courses in this degree *****/
   Crs_FreeListCoursesInCurrentDegree ();

   /***** Free list of degrees in this centre *****/
   Deg_FreeListDegs (&Gbl.Hierarchy.Ctr.Degs);
  }

/*****************************************************************************/
/**************** Put contextual icons in edition of courses *****************/
/*****************************************************************************/

static void Crs_PutIconsEditingCourses (void)
  {
   /***** Put icon to view degrees *****/
   Crs_PutIconToViewCourses ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_HIERARCHY;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/************************* Put icon to view courses **************************/
/*****************************************************************************/

static void Crs_PutIconToViewCourses (void)
  {
   extern const char *Txt_Courses;

   Lay_PutContextualLinkOnlyIcon (ActSeeCrs,NULL,NULL,
                                  "list-ol.svg",
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
   struct Course *Crs;
   unsigned YearAux;
   unsigned NumCrs;
   struct UsrData UsrDat;
   bool ICanEdit;
   Crs_StatusTxt_t StatusTxt;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** List courses of a given year *****/
   for (NumCrs = 0;
	NumCrs < Gbl.Hierarchy.Deg.Crss.Num;
	NumCrs++)
     {
      Crs = &(Gbl.Hierarchy.Deg.Crss.Lst[NumCrs]);
      if (Crs->Year == Year)
	{
	 ICanEdit = Crs_CheckIfICanEdit (Crs);

	 HTM_TR_Begin (NULL);

	 /* Put icon to remove course */
	 HTM_TD_Begin ("class=\"BM\"");
	 if (Crs->NumUsrs[Rol_UNK] ||	// Course has users ==> deletion forbidden
	     !ICanEdit)
	    Ico_PutIconRemovalNotAllowed ();
	 else	// Crs->NumUsrs == 0 && ICanEdit
	   {
	    Frm_StartForm (ActRemCrs);
	    Crs_PutParamOtherCrsCod (Crs->CrsCod);
	    Ico_PutIconRemove ();
	    Frm_EndForm ();
	   }
	 HTM_TD_End ();

	 /* Course code */
	 HTM_TD_Begin ("class=\"DAT CODE\"");
	 fprintf (Gbl.F.Out,"%ld",Crs->CrsCod);
	 HTM_TD_End ();

	 /* Institutional code of the course */
	 HTM_TD_Begin ("class=\"DAT CM\"");
	 if (ICanEdit)
	   {
	    Frm_StartForm (ActChgInsCrsCod);
	    Crs_PutParamOtherCrsCod (Crs->CrsCod);
	    fprintf (Gbl.F.Out,"<input type=\"text\" name=\"InsCrsCod\""
			       " maxlength=\"%u\" value=\"%s\""
			       " class=\"INPUT_INS_CODE\""
			       " onchange=\"document.getElementById('%s').submit();\" />",
		     Crs_MAX_CHARS_INSTITUTIONAL_CRS_COD,
		     Crs->InstitutionalCrsCod,
		     Gbl.Form.Id);
	    Frm_EndForm ();
	   }
	 else
	    fprintf (Gbl.F.Out,"%s",Crs->InstitutionalCrsCod);
	 HTM_TD_End ();

	 /* Course year */
	 HTM_TD_Begin ("class=\"DAT CM\"");
	 if (ICanEdit)
	   {
	    Frm_StartForm (ActChgCrsYea);
	    Crs_PutParamOtherCrsCod (Crs->CrsCod);
	    fprintf (Gbl.F.Out,"<select name=\"OthCrsYear\" class=\"HIE_SEL_NARROW\""
			       " onchange=\"document.getElementById('%s').submit();\">",
		     Gbl.Form.Id);
	    for (YearAux = 0;
		 YearAux <= Deg_MAX_YEARS_PER_DEGREE;
		 YearAux++)	// All the years are permitted because it's possible to move this course to another degree (with other active years)
	       fprintf (Gbl.F.Out,"<option value=\"%u\"%s>%s</option>",
			YearAux,
			YearAux == Crs->Year ? " selected=\"selected\"" :
					       "",
			Txt_YEAR_OF_DEGREE[YearAux]);
	    fprintf (Gbl.F.Out,"</select>");
	    Frm_EndForm ();
	   }
	 else
	    fprintf (Gbl.F.Out,"%s",Txt_YEAR_OF_DEGREE[Crs->Year]);
	 HTM_TD_End ();

	 /* Course short name */
	 HTM_TD_Begin ("class=\"DAT LM\"");
	 if (ICanEdit)
	   {
	    Frm_StartForm (ActRenCrsSho);
	    Crs_PutParamOtherCrsCod (Crs->CrsCod);
	    fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\""
			       " maxlength=\"%u\" value=\"%s\""
			       " class=\"INPUT_SHORT_NAME\""
			       " onchange=\"document.getElementById('%s').submit();\" />",
		     Hie_MAX_CHARS_SHRT_NAME,Crs->ShrtName,
		     Gbl.Form.Id);
	    Frm_EndForm ();
	   }
	 else
	    fprintf (Gbl.F.Out,"%s",Crs->ShrtName);
	 HTM_TD_End ();

	 /* Course full name */
	 HTM_TD_Begin ("class=\"DAT LM\"");
	 if (ICanEdit)
	   {
	    Frm_StartForm (ActRenCrsFul);
	    Crs_PutParamOtherCrsCod (Crs->CrsCod);
	    fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\""
			       " maxlength=\"%u\" value=\"%s\""
			       " class=\"INPUT_FULL_NAME\""
			       " onchange=\"document.getElementById('%s').submit();\" />",
		     Hie_MAX_CHARS_FULL_NAME,Crs->FullName,
		     Gbl.Form.Id);
	    Frm_EndForm ();
	   }
	 else
	    fprintf (Gbl.F.Out,"%s",Crs->FullName);
	 HTM_TD_End ();

	 /* Current number of teachers in this course */
	 HTM_TD_Begin ("class=\"DAT RM\"");
	 fprintf (Gbl.F.Out,"%u",Crs->NumUsrs[Rol_TCH] +
				 Crs->NumUsrs[Rol_NET]);
	 HTM_TD_End ();

	 /* Current number of students in this course */
	 HTM_TD_Begin ("class=\"DAT RM\"");
	 fprintf (Gbl.F.Out,"%u",Crs->NumUsrs[Rol_STD]);
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
	    Frm_StartForm (ActChgCrsSta);
	    Crs_PutParamOtherCrsCod (Crs->CrsCod);
	    fprintf (Gbl.F.Out,"<select name=\"Status\" class=\"INPUT_STATUS\""
			       " onchange=\"document.getElementById('%s').submit();\">"
			       "<option value=\"%u\" selected=\"selected\">%s</option>"
			       "<option value=\"%u\">%s</option>"
			       "</select>",
		     Gbl.Form.Id,
		     (unsigned) Crs_GetStatusBitsFromStatusTxt (Crs_STATUS_PENDING),
		     Txt_COURSE_STATUS[Crs_STATUS_PENDING],
		     (unsigned) Crs_GetStatusBitsFromStatusTxt (Crs_STATUS_ACTIVE),
		     Txt_COURSE_STATUS[Crs_STATUS_ACTIVE]);
	    Frm_EndForm ();
	   }
         else if (StatusTxt != Crs_STATUS_ACTIVE)	// If active ==> do not show anything
	    fprintf (Gbl.F.Out,"%s",Txt_COURSE_STATUS[StatusTxt]);
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

static bool Crs_CheckIfICanEdit (struct Course *Crs)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM ||		// I am a degree administrator or higher
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
   extern const char *Txt_New_course;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_Create_course;
   unsigned Year;

   /***** Begin form *****/
   if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
      Frm_StartForm (ActNewCrs);
   else if (Gbl.Usrs.Me.Role.Max >= Rol_GST)
      Frm_StartForm (ActReqCrs);
   else
      Lay_NoPermissionExit ();

   /***** Begin box and table *****/
   Box_StartBoxTable (NULL,Txt_New_course,NULL,
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
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"InsCrsCod\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_INS_CODE\" />",
            Crs_MAX_CHARS_INSTITUTIONAL_CRS_COD,
            Crs_EditingCrs->InstitutionalCrsCod);
   HTM_TD_End ();

   /***** Year *****/
   HTM_TD_Begin ("class=\"CM\"");
   fprintf (Gbl.F.Out,"<select name=\"OthCrsYear\" class=\"HIE_SEL_NARROW\">");
   for (Year = 0;
	Year <= Deg_MAX_YEARS_PER_DEGREE;
        Year++)
      fprintf (Gbl.F.Out,"<option value=\"%u\"%s>%s</option>",
	       Year,
	       Year == Crs_EditingCrs->Year ? " selected=\"selected\"" :
				              "",
	       Txt_YEAR_OF_DEGREE[Year]);
   fprintf (Gbl.F.Out,"</select>");
   HTM_TD_End ();

   /***** Course short name *****/
   HTM_TD_Begin ("class=\"LM\"");
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_SHORT_NAME\""
                      " required=\"required\" />",
            Hie_MAX_CHARS_SHRT_NAME,Crs_EditingCrs->ShrtName);
   HTM_TD_End ();

   /***** Course full name *****/
   HTM_TD_Begin ("class=\"LM\"");
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_FULL_NAME\""
                      " required=\"required\" />",
            Hie_MAX_CHARS_FULL_NAME,Crs_EditingCrs->FullName);
   HTM_TD_End ();

   /***** Current number of teachers in this course *****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   fprintf (Gbl.F.Out,"0");
   HTM_TD_End ();

   /***** Current number of students in this course *****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   fprintf (Gbl.F.Out,"0");
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
   Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_course);

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
   fprintf (Gbl.F.Out,"%s (%s)",Txt_Institutional_code,Txt_optional);
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

void Crs_RecFormReqCrs (void)
  {
   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Receive form to request a new course *****/
   Crs_RecFormRequestOrCreateCrs ((unsigned) Crs_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/******************* Receive form to create a new course *********************/
/*****************************************************************************/

void Crs_RecFormNewCrs (void)
  {
   /***** Course constructor *****/
   Crs_EditingCourseConstructor ();

   /***** Receive form to create a new course *****/
   Crs_RecFormRequestOrCreateCrs (0);
  }

/*****************************************************************************/
/************* Receive form to request or create a new course ****************/
/*****************************************************************************/

static void Crs_RecFormRequestOrCreateCrs (unsigned Status)
  {
   extern const char *Txt_The_course_X_already_exists;
   extern const char *Txt_Created_new_course_X;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_course;
   extern const char *Txt_The_year_X_is_not_allowed;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   struct Degree Deg;

   /***** Get parameters from form *****/
   /* Set course degree */
   Deg.DegCod = Crs_EditingCrs->DegCod = Gbl.Hierarchy.Deg.DegCod;

   /* Get parameters of the new course */
   Crs_GetParamsNewCourse (Crs_EditingCrs);

   /***** Check if year is correct *****/
   Deg_GetDataOfDegreeByCod (&Deg);
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

static void Crs_GetParamsNewCourse (struct Course *Crs)
  {
   char YearStr[2 + 1];

   /***** Get parameters of the course from form *****/
   /* Get institutional code */
   Par_GetParToText ("InsCrsCod",Crs->InstitutionalCrsCod,Crs_MAX_BYTES_INSTITUTIONAL_CRS_COD);

   /* Get year */
   Par_GetParToText ("OthCrsYear",YearStr,2);
   Crs->Year = Deg_ConvStrToYear (YearStr);

   /* Get course short name */
   Par_GetParToText ("ShortName",Crs->ShrtName,Hie_MAX_BYTES_SHRT_NAME);

   /* Get course full name */
   Par_GetParToText ("FullName",Crs->FullName,Hie_MAX_BYTES_FULL_NAME);
  }

/*****************************************************************************/
/************* Add a new requested course to pending requests ****************/
/*****************************************************************************/

static void Crs_CreateCourse (unsigned Status)
  {
   /***** Insert new course into pending requests *****/
   Crs_EditingCrs->CrsCod =
   DB_QueryINSERTandReturnCode ("can not create a new course",
				"INSERT INTO courses"
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
      if (Crs_EditingCrs->NumUsrs[Rol_UNK])	// Course has users ==> don't remove
         Ale_ShowAlert (Ale_WARNING,Txt_To_remove_a_course_you_must_first_remove_all_users_in_the_course);
      else			// Course has no users ==> remove it
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

bool Crs_GetDataOfCourseByCod (struct Course *Crs)
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
   Crs->NumUsrs[Rol_UNK] =
   Crs->NumUsrs[Rol_STD] =
   Crs->NumUsrs[Rol_NET] =
   Crs->NumUsrs[Rol_TCH] = 0;

   /***** Check if course code is correct *****/
   if (Crs->CrsCod > 0)
     {
      /***** Get data of a course from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get data of a course",
			  "SELECT CrsCod,DegCod,Year,InsCrsCod,Status,RequesterUsrCod,ShortName,FullName"
			  " FROM courses WHERE CrsCod=%ld",
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

static void Crs_GetDataOfCourseFromRow (struct Course *Crs,MYSQL_ROW row)
  {
   /***** Get course code (row[0]) *****/
   if ((Crs->CrsCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
      Lay_ShowErrorAndExit ("Wrong code of course.");

   /***** Get code of degree (row[1]) *****/
   Crs->DegCod = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get year (row[2]) *****/
   Crs->Year = Deg_ConvStrToYear (row[2]);

   /***** Get institutional course code (row[3]) *****/
   Str_Copy (Crs->InstitutionalCrsCod,row[3],
             Crs_MAX_BYTES_INSTITUTIONAL_CRS_COD);

   /***** Get course status (row[4]) *****/
   if (sscanf (row[4],"%u",&(Crs->Status)) != 1)
      Lay_ShowErrorAndExit ("Wrong course status.");

   /***** Get requester user'code (row[5]) *****/
   Crs->RequesterUsrCod = Str_ConvertStrCodToLongCod (row[5]);

   /***** Get the short name of the course (row[6]) *****/
   Str_Copy (Crs->ShrtName,row[6],
             Hie_MAX_BYTES_SHRT_NAME);

   /***** Get the full name of the course (row[7]) *****/
   Str_Copy (Crs->FullName,row[7],
             Hie_MAX_BYTES_FULL_NAME);

   /***** Get number of users *****/
   Crs->NumUsrs[Rol_STD] = Usr_GetNumUsrsInCrs (Rol_STD,Crs->CrsCod);
   Crs->NumUsrs[Rol_NET] = Usr_GetNumUsrsInCrs (Rol_NET,Crs->CrsCod);
   Crs->NumUsrs[Rol_TCH] = Usr_GetNumUsrsInCrs (Rol_TCH,Crs->CrsCod);
   Crs->NumUsrs[Rol_UNK] = Crs->NumUsrs[Rol_STD] +
	                   Crs->NumUsrs[Rol_NET] +
	                   Crs->NumUsrs[Rol_TCH];
  }

/*****************************************************************************/
/******* Get the short names of degree and course from a course code *********/
/*****************************************************************************/

static void Crs_GetShortNamesByCod (long CrsCod,
                                    char CrsShortName[Hie_MAX_BYTES_SHRT_NAME + 1],
                                    char DegShortName[Hie_MAX_BYTES_SHRT_NAME + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   DegShortName[0] = CrsShortName[0] = '\0';

   if (CrsCod > 0)
     {
      /***** Get the short name of a degree from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get the short name of a course",
			  "SELECT courses.ShortName,degrees.ShortName"
			  " FROM courses,degrees"
			  " WHERE courses.CrsCod=%ld"
			  " AND courses.DegCod=degrees.DegCod",
			  CrsCod) == 1)
	{
	 /***** Get the short name of this course *****/
	 row = mysql_fetch_row (mysql_res);

	 Str_Copy (CrsShortName,row[0],
	           Hie_MAX_BYTES_SHRT_NAME);
	 Str_Copy (DegShortName,row[1],
	           Hie_MAX_BYTES_SHRT_NAME);
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
		      "DELETE FROM courses WHERE CrsCod=%ld",
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
   struct Course Crs;
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
		      "DELETE FROM timetable_crs WHERE CrsCod=%ld",
		      CrsCod);

      /* Remove other information of the course */
      DB_QueryDELETE ("can not remove info sources of a course",
		      "DELETE FROM crs_info_src WHERE CrsCod=%ld",
		      CrsCod);

      DB_QueryDELETE ("can not remove info of a course",
		      "DELETE FROM crs_info_txt WHERE CrsCod=%ld",
		      CrsCod);

      /***** Remove exam announcements in the course *****/
      /* Mark all exam announcements in the course as deleted */
      DB_QueryUPDATE ("can not remove exam announcements of a course",
		      "UPDATE exam_announcements SET Status=%u"
		      " WHERE CrsCod=%ld",
	              (unsigned) Exa_DELETED_EXAM_ANNOUNCEMENT,CrsCod);

      /***** Remove course cards of the course *****/
      /* Remove content of course cards */
      DB_QueryDELETE ("can not remove content of cards in a course",
		      "DELETE FROM crs_records"
		      " USING crs_record_fields,crs_records"
		      " WHERE crs_record_fields.CrsCod=%ld"
		      " AND crs_record_fields.FieldCod=crs_records.FieldCod",
	              CrsCod);

      /* Remove definition of fields in course cards */
      DB_QueryDELETE ("can not remove fields of cards in a course",
		      "DELETE FROM crs_record_fields WHERE CrsCod=%ld",
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
		      "INSERT INTO notices_deleted"
		      " (NotCod,CrsCod,UsrCod,CreatTime,Content,NumNotif)"
		      " SELECT NotCod,CrsCod,UsrCod,CreatTime,Content,NumNotif"
		      " FROM notices"
		      " WHERE CrsCod=%ld",
	              CrsCod);

      /* Remove all notices from the course */
      DB_QueryDELETE ("can not remove notices in a course",
		      "DELETE FROM notices WHERE CrsCod=%ld",
		      CrsCod);

      /***** Remove all the threads and posts in forums of the course *****/
      For_RemoveForums (Hie_CRS,CrsCod);

      /***** Remove games of the course *****/
      Gam_RemoveGamesCrs (CrsCod);

      /***** Remove surveys of the course *****/
      Svy_RemoveSurveys (Hie_CRS,CrsCod);

      /***** Remove all test exams made in the course *****/
      Tst_RemoveCrsTestResults (CrsCod);

      /***** Remove all tests questions in the course *****/
      Tst_RemoveCrsTests (CrsCod);

      /***** Remove groups in the course *****/
      /* Remove all the users in groups in the course */
      DB_QueryDELETE ("can not remove users from groups of a course",
		      "DELETE FROM crs_grp_usr"
		      " USING crs_grp_types,crs_grp,crs_grp_usr"
		      " WHERE crs_grp_types.CrsCod=%ld"
		      " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
		      " AND crs_grp.GrpCod=crs_grp_usr.GrpCod",
	              CrsCod);

      /* Remove all the groups in the course */
      DB_QueryDELETE ("can not remove groups of a course",
		      "DELETE FROM crs_grp"
		      " USING crs_grp_types,crs_grp"
		      " WHERE crs_grp_types.CrsCod=%ld"
		      " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod",
	              CrsCod);

      /* Remove all the group types in the course */
      DB_QueryDELETE ("can not remove types of group of a course",
		      "DELETE FROM crs_grp_types WHERE CrsCod=%ld",
		      CrsCod);

      /***** Remove users' requests for inscription in the course *****/
      DB_QueryDELETE ("can not remove requests for inscription to a course",
		      "DELETE FROM crs_usr_requests WHERE CrsCod=%ld",
		      CrsCod);

      /***** Remove possible users remaining in the course (teachers) *****/
      DB_QueryDELETE ("can not remove users from a course",
		      "DELETE FROM crs_usr WHERE CrsCod=%ld",
		      CrsCod);

      /***** Remove directories of the course *****/
      snprintf (PathRelCrs,sizeof (PathRelCrs),
	        "%s/%ld",
	        Cfg_PATH_CRS_PRIVATE,CrsCod);
      Fil_RemoveTree (PathRelCrs);
      snprintf (PathRelCrs,sizeof (PathRelCrs),
	        "%s/%ld",
	        Cfg_PATH_CRS_PUBLIC,CrsCod);
      Fil_RemoveTree (PathRelCrs);
     }
  }

/*****************************************************************************/
/***** Change the institutional code of a course in course configuration *****/
/*****************************************************************************/

void Crs_ChangeInsCrsCodInConfig (void)
  {
   extern const char *Txt_The_institutional_code_of_the_course_X_has_changed_to_Y;
   extern const char *Txt_The_institutional_code_of_the_course_X_has_not_changed;
   char NewInstitutionalCrsCod[Crs_MAX_BYTES_INSTITUTIONAL_CRS_COD + 1];

   /***** Get institutional code from form *****/
   Par_GetParToText ("InsCrsCod",NewInstitutionalCrsCod,Crs_MAX_BYTES_INSTITUTIONAL_CRS_COD);

   /***** Change the institutional course code *****/
   if (strcmp (NewInstitutionalCrsCod,Gbl.Hierarchy.Crs.InstitutionalCrsCod))
     {
      Crs_UpdateInstitutionalCrsCod (&Gbl.Hierarchy.Crs,NewInstitutionalCrsCod);

      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_institutional_code_of_the_course_X_has_changed_to_Y,
	               Gbl.Hierarchy.Crs.ShrtName,
		       NewInstitutionalCrsCod);
     }
   else	// The same institutional code
      Ale_CreateAlert (Ale_INFO,NULL,
	               Txt_The_institutional_code_of_the_course_X_has_not_changed,
	               Gbl.Hierarchy.Crs.ShrtName);
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
/***************** Change the degree of the current course *******************/
/*****************************************************************************/

void Crs_ChangeCrsDegInConfig (void)
  {
   extern const char *Txt_In_the_year_X_of_the_degree_Y_already_existed_a_course_with_the_name_Z;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_The_course_X_has_been_moved_to_the_degree_Y;
   struct Degree NewDeg;

   /***** Get parameter with degree code *****/
   NewDeg.DegCod = Deg_GetAndCheckParamOtherDegCod (1);

   /***** Check if degree has changed *****/
   if (NewDeg.DegCod != Gbl.Hierarchy.Crs.DegCod)
     {
      /***** Get data of new degree *****/
      Deg_GetDataOfDegreeByCod (&NewDeg);

      /***** If name of course was in database in the new degree... *****/
      if (Crs_CheckIfCrsNameExistsInYearOfDeg ("ShortName",Gbl.Hierarchy.Crs.ShrtName,-1L,
                                               NewDeg.DegCod,Gbl.Hierarchy.Crs.Year))
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_In_the_year_X_of_the_degree_Y_already_existed_a_course_with_the_name_Z,
		          Txt_YEAR_OF_DEGREE[Gbl.Hierarchy.Crs.Year],
			  NewDeg.FullName,
		          Gbl.Hierarchy.Crs.ShrtName);
      else if (Crs_CheckIfCrsNameExistsInYearOfDeg ("FullName",Gbl.Hierarchy.Crs.FullName,-1L,
                                                    NewDeg.DegCod,Gbl.Hierarchy.Crs.Year))
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_In_the_year_X_of_the_degree_Y_already_existed_a_course_with_the_name_Z,
		          Txt_YEAR_OF_DEGREE[Gbl.Hierarchy.Crs.Year],
			  NewDeg.FullName,
		          Gbl.Hierarchy.Crs.FullName);
      else	// Update degree in database
	{
	 /***** Update degree in table of courses *****/
	 Crs_UpdateCrsDegDB (Gbl.Hierarchy.Crs.CrsCod,NewDeg.DegCod);
	 Gbl.Hierarchy.Crs.DegCod =
	 Gbl.Hierarchy.Deg.DegCod = NewDeg.DegCod;

	 /***** Initialize again current course, degree, centre... *****/
      	 Hie_InitHierarchy ();

	 /***** Create alert to show the change made *****/
	 Ale_CreateAlert (Ale_SUCCESS,NULL,
	                  Txt_The_course_X_has_been_moved_to_the_degree_Y,
		          Gbl.Hierarchy.Crs.FullName,
		          Gbl.Hierarchy.Deg.FullName);
	}
     }
  }

/*****************************************************************************/
/** Show message of success after changing a course in course configuration **/
/*****************************************************************************/

void Crs_ContEditAfterChgCrsInConfig (void)
  {
   /***** Write error/success message *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Crs_ShowIntroduction ();
  }

/*****************************************************************************/
/********************** Update degree in table of courses ********************/
/*****************************************************************************/

static void Crs_UpdateCrsDegDB (long CrsCod,long DegCod)
  {
   /***** Update degree in table of courses *****/
   DB_QueryUPDATE ("can not move course to another degree",
		   "UPDATE courses SET DegCod=%ld WHERE CrsCod=%ld",
	           DegCod,CrsCod);
  }

/*****************************************************************************/
/*********** Change the year of a course in course configuration *************/
/*****************************************************************************/

void Crs_ChangeCrsYearInConfig (void)
  {
   extern const char *Txt_The_course_X_already_exists_in_year_Y;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_The_year_of_the_course_X_has_changed;
   extern const char *Txt_The_year_X_is_not_allowed;
   char YearStr[2 + 1];
   unsigned NewYear;

   /***** Get parameter with year/semester *****/
   Par_GetParToText ("OthCrsYear",YearStr,2);
   NewYear = Deg_ConvStrToYear (YearStr);

   if (NewYear <= Deg_MAX_YEARS_PER_DEGREE)	// If year is valid
     {
      /***** If name of course was in database in the new year... *****/
      if (Crs_CheckIfCrsNameExistsInYearOfDeg ("ShortName",Gbl.Hierarchy.Crs.ShrtName,-1L,
                                               Gbl.Hierarchy.Crs.DegCod,NewYear))
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_The_course_X_already_exists_in_year_Y,
		          Gbl.Hierarchy.Crs.ShrtName,
			  Txt_YEAR_OF_DEGREE[NewYear]);
      else if (Crs_CheckIfCrsNameExistsInYearOfDeg ("FullName",Gbl.Hierarchy.Crs.FullName,-1L,
                                                    Gbl.Hierarchy.Crs.DegCod,NewYear))
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_The_course_X_already_exists_in_year_Y,
		          Gbl.Hierarchy.Crs.FullName,
			  Txt_YEAR_OF_DEGREE[NewYear]);
      else	// Update year in database
	{
	 /***** Update year in table of courses *****/
         Crs_UpdateCrsYear (&Gbl.Hierarchy.Crs,NewYear);

	 /***** Create alert to show the change made *****/
	 Ale_CreateAlert (Ale_SUCCESS,NULL,
	                  Txt_The_year_of_the_course_X_has_changed,
		          Gbl.Hierarchy.Crs.ShrtName);
	}
     }
   else	// Year not valid
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_The_year_X_is_not_allowed,
		       NewYear);
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
   struct Degree Deg;
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
      Deg.DegCod = Crs_EditingCrs->DegCod;
      Deg_GetDataOfDegreeByCod (&Deg);

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

static void Crs_UpdateCrsYear (struct Course *Crs,unsigned NewYear)
  {
   /***** Update year/semester in table of courses *****/
   DB_QueryUPDATE ("can not update the year of a course",
		   "UPDATE courses SET Year=%u WHERE CrsCod=%ld",
	           NewYear,Crs->CrsCod);

   /***** Copy course year/semester *****/
   Crs->Year = NewYear;
  }

/*****************************************************************************/
/************* Change the institutional course code of a course **************/
/*****************************************************************************/

void Crs_UpdateInstitutionalCrsCod (struct Course *Crs,const char *NewInstitutionalCrsCod)
  {
   /***** Update institutional course code in table of courses *****/
   DB_QueryUPDATE ("can not update the institutional code"
	           " of the current course",
		   "UPDATE courses SET InsCrsCod='%s' WHERE CrsCod=%ld",
                   NewInstitutionalCrsCod,Crs->CrsCod);

   /***** Copy institutional course code *****/
   Str_Copy (Crs->InstitutionalCrsCod,NewInstitutionalCrsCod,
             Crs_MAX_BYTES_INSTITUTIONAL_CRS_COD);
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
/*************** Change the name of a course in configuration ****************/
/*****************************************************************************/

void Crs_RenameCourseShortInConfig (void)
  {
   Crs_RenameCourse (&Gbl.Hierarchy.Crs,Cns_SHRT_NAME);
  }

void Crs_RenameCourseFullInConfig (void)
  {
   Crs_RenameCourse (&Gbl.Hierarchy.Crs,Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a course ************************/
/*****************************************************************************/

static void Crs_RenameCourse (struct Course *Crs,Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_course_X_empty;
   extern const char *Txt_The_course_X_already_exists;
   extern const char *Txt_The_name_of_the_course_X_has_changed_to_Y;
   extern const char *Txt_The_name_of_the_course_X_has_not_changed;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentCrsName = NULL;		// Initialized to avoid warning
   char NewCrsName[Hie_MAX_BYTES_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Hie_MAX_BYTES_SHRT_NAME;
         CurrentCrsName = Crs->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Hie_MAX_BYTES_FULL_NAME;
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
      if (!NewCrsName[0])
	 Ale_CreateAlert (Ale_WARNING,NULL,
		          Txt_You_can_not_leave_the_name_of_the_course_X_empty,
                          CurrentCrsName);
      else
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
               Str_Copy (CurrentCrsName,NewCrsName,
                         MaxBytes);
              }
           }
         else	// The same name
	    Ale_CreateAlert (Ale_INFO,NULL,
		             Txt_The_name_of_the_course_X_has_not_changed,
                             CurrentCrsName);
        }
     }
   else
      Lay_NoPermissionExit ();
  }

/*****************************************************************************/
/********** Check if the name of course exists in existing courses ***********/
/*****************************************************************************/

static bool Crs_CheckIfCrsNameExistsInYearOfDeg (const char *FieldName,const char *Name,long CrsCod,
                                                 long DegCod,unsigned Year)
  {
   /***** Get number of courses in a year of a degree and with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name"
	                  " of a course already existed",
			  "SELECT COUNT(*) FROM courses"
			  " WHERE DegCod=%ld AND Year=%u"
			  " AND %s='%s' AND CrsCod<>%ld",
			  DegCod,Year,FieldName,Name,CrsCod) != 0);
  }

/*****************************************************************************/
/***************** Update course name in table of courses ********************/
/*****************************************************************************/

static void Crs_UpdateCrsNameDB (long CrsCod,const char *FieldName,const char *NewCrsName)
  {
   /***** Update course changing old name by new name *****/
   DB_QueryUPDATE ("can not update the name of a course",
		   "UPDATE courses SET %s='%s' WHERE CrsCod=%ld",
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
		   "UPDATE courses SET Status=%u WHERE CrsCod=%ld",
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
      /***** Start alert *****/
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
      Ale_ShowAlertAndButton2 (ActUnk,NULL,NULL,NULL,Btn_NO_BUTTON,NULL);
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
   extern const char *Txt_Go_to_X;

   // If the course being edited is different to the current one...
   if (Crs_EditingCrs->CrsCod != Gbl.Hierarchy.Crs.CrsCod)
     {
      Frm_StartForm (ActSeeCrsInf);
      Crs_PutParamCrsCod (Crs_EditingCrs->CrsCod);
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        Txt_Go_to_X,
		Crs_EditingCrs->ShrtName);
      Btn_PutConfirmButton (Gbl.Title);
      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/************************ Put button to go to course *************************/
/*****************************************************************************/

static void Crs_PutButtonToRegisterInCrs (void)
  {
   extern const char *Txt_Register_me_in_X;

   Frm_StartForm (ActReqSignUp);
   // If the course being edited is different to the current one...
   if (Crs_EditingCrs->CrsCod != Gbl.Hierarchy.Crs.CrsCod)
      Crs_PutParamCrsCod (Crs_EditingCrs->CrsCod);
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Register_me_in_X,
	     Crs_EditingCrs->ShrtName);
   Btn_PutCreateButton (Gbl.Title);
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

static void Crs_PutIconToSearchCourses (void)
  {
   extern const char *Txt_Search_courses;

   /***** Put form to search / select courses *****/
   Lay_PutContextualLinkOnlyIcon (ActReqSch,
				  NULL,Sch_PutLinkToSearchCoursesParams,
			          "search.svg",
			          Txt_Search_courses);
  }

static void Sch_PutLinkToSearchCoursesParams (void)	// TODO: Move to search module
  {
   Sco_PutParamScope ("ScopeSch",Hie_SYS);
   Par_PutHiddenParamUnsigned ("WhatToSearch",(unsigned) Sch_SEARCH_COURSES);
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
      Frm_StartForm (ActMyCrs);

      /***** Put icon with link *****/
      Frm_LinkFormSubmit (Txt_My_courses,NULL,NULL);
      fprintf (Gbl.F.Out,"<img src=\"%s/sitemap.svg\""
			 " alt=\"%s\" title=\"%s\""
			 " class=\"BC_ICON ICO_HIGHLIGHT\" />"
			 "</a>",
	       Gbl.Prefs.URLTheme,
	       Txt_My_courses,Txt_My_courses);

      /***** End form *****/
      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/****************** Put an icon (form) to select my courses ******************/
/*****************************************************************************/

void Crs_PutIconToSelectMyCourses (void)
  {
   extern const char *Txt_My_courses;

   if (Gbl.Usrs.Me.Logged)		// I am logged
      /***** Put icon with link *****/
      Lay_PutContextualLinkOnlyIcon (ActMyCrs,NULL,NULL,
				     "sitemap.svg",
				     Txt_My_courses);
  }

/*****************************************************************************/
/******************** Write parameter with code of course ********************/
/*****************************************************************************/

void Crs_PutParamCrsCod (long CrsCod)
  {
   Par_PutHiddenParamLong ("crs",CrsCod);
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
      if (asprintf (&SubQuery," AND crs_usr.Role=%u",(unsigned) Role) < 0)
	 Lay_NotEnoughMemoryExit ();
     }
   NumCrss = (unsigned) DB_QuerySELECT (&mysql_res,"can not get courses of a user",
				        "SELECT degrees.DegCod,courses.CrsCod,degrees.ShortName,degrees.FullName,"
				        "courses.Year,courses.FullName,centres.ShortName,crs_usr.Accepted"
				        " FROM crs_usr,courses,degrees,centres"
				        " WHERE crs_usr.UsrCod=%ld%s"
				        " AND crs_usr.CrsCod=courses.CrsCod"
				        " AND courses.DegCod=degrees.DegCod"
				        " AND degrees.CtrCod=centres.CtrCod"
				        " ORDER BY degrees.FullName,courses.Year,courses.FullName",
				        UsrDat->UsrCod,SubQuery);

   /***** Free allocated memory for subquery *****/
   free ((void *) SubQuery);

   /***** List the courses (one row per course) *****/
   if (NumCrss)
     {
      /* Begin box and table */
      Box_StartBoxTable ("100%",NULL,NULL,
                         NULL,Box_NOT_CLOSABLE,2);

      /* Heading row */
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        Txt_USER_in_COURSE,
                Role == Rol_UNK ? Txt_User[Usr_SEX_UNKNOWN] : // Role == Rol_UNK ==> any role
        	                  Txt_ROLES_SINGUL_Abc[Role][UsrDat->Sex]);

      HTM_TR_Begin (NULL);

      HTM_TH_Begin (1,7,"LM");
      fprintf (Gbl.F.Out,"%s:",Gbl.Title);
      HTM_TH_End ();

      HTM_TR_End ();

      HTM_TR_Begin (NULL);

      HTM_TH (1,1,"BM",NULL);
      HTM_TH (1,1,"BM",NULL);
      HTM_TH (1,1,"LM",Txt_Degree);
      HTM_TH (1,1,"CM",Txt_Year_OF_A_DEGREE);
      HTM_TH (1,1,"LM",Txt_Course);
      HTM_TH (1,1,"RM",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
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
      Box_EndBoxTable ();
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
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        "%u %s",
                NumCrss,(NumCrss == 1) ? Txt_course :
	                                 Txt_courses);
      Box_StartBoxTable (NULL,Gbl.Title,NULL,
                         NULL,Box_NOT_CLOSABLE,2);

      /***** Heading row *****/
      HTM_TR_Begin (NULL);

      HTM_TH (1,1,"BM",NULL);
      HTM_TH (1,1,"LM",Txt_Degree);
      HTM_TH (1,1,"CM",Txt_Year_OF_A_DEGREE);
      HTM_TH (1,1,"LM",Txt_Course);
      HTM_TH (1,1,"RM",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
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
      Box_EndBoxTable ();
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
   extern const char *Txt_Go_to_X;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   struct Degree Deg;
   long CrsCod;
   unsigned NumTchs;
   unsigned NumStds;
   const char *Style;
   const char *StyleNoBR;
   const char *BgColor;
   bool Accepted;
   static unsigned RowEvenOdd = 1;

   /*
   SELECT degrees.DegCod	0
	  courses.CrsCod	1
	  degrees.ShortName	2
	  degrees.FullName	3
	  courses.Year		4
	  courses.FullName	5
	  centres.ShortName	6
	  crs_usr.Accepted	7	(only if WriteColumnAccepted == true)
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
   NumTchs = Usr_GetNumUsrsInCrs (Rol_TCH,CrsCod) +
	     Usr_GetNumUsrsInCrs (Rol_NET,CrsCod);
   NumStds = Usr_GetNumUsrsInCrs (Rol_STD,CrsCod);
   if (NumTchs + NumStds)
     {
      Style = "DAT_N";
      StyleNoBR = "DAT_NOBR_N";
     }
   else
     {
      Style = "DAT";
      StyleNoBR = "DAT_NOBR";
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
      fprintf (Gbl.F.Out,"%s",
               Accepted ? "&check;" :
        	          "&cross;");
      HTM_TD_End ();
     }

   /***** Write number of course in this search *****/
   HTM_TD_Begin ("class=\"%s RT %s\"",StyleNoBR,BgColor);
   fprintf (Gbl.F.Out,"%u",NumCrs);
   HTM_TD_End ();

   /***** Write degree logo, degree short name (row[2])
          and centre short name (row[6]) *****/
   HTM_TD_Begin ("class=\"%s LT %s\"",StyleNoBR,BgColor);
   Frm_StartFormGoTo (ActSeeDegInf);
   Deg_PutParamDegCod (Deg.DegCod);
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Go_to_X,
	     row[2]);
   Frm_LinkFormSubmit (Gbl.Title,StyleNoBR,NULL);
   Log_DrawLogo (Hie_DEG,Deg.DegCod,Deg.ShrtName,20,"CT",true);
   fprintf (Gbl.F.Out," %s (%s)"
                      "</a>",
            row[2],row[6]);
   Frm_EndForm ();
   HTM_TD_End ();

   /***** Write year (row[4]) *****/
   HTM_TD_Begin ("class=\"%s CT %s\"",Style,BgColor);
   fprintf (Gbl.F.Out,"%s",Txt_YEAR_OF_DEGREE[Deg_ConvStrToYear (row[4])]);
   HTM_TD_End ();

   /***** Write course full name (row[5]) *****/
   HTM_TD_Begin ("class=\"%s LT %s\"",Style,BgColor);
   Frm_StartFormGoTo (ActSeeCrsInf);
   Crs_PutParamCrsCod (CrsCod);
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Go_to_X,
	     row[6]);
   Frm_LinkFormSubmit (Gbl.Title,Style,NULL);
   fprintf (Gbl.F.Out,"%s</a>",row[5]);
   Frm_EndForm ();
   HTM_TD_End ();

   /***** Write number of teachers in course *****/
   HTM_TD_Begin ("class=\"%s RT %s\"",Style,BgColor);
   fprintf (Gbl.F.Out,"%u",NumTchs);
   HTM_TD_End ();

   /***** Write number of students in course *****/
   HTM_TD_Begin ("class=\"%s RT %s\"",Style,BgColor);
   fprintf (Gbl.F.Out,"%u",NumStds);
   HTM_TD_End ();

   HTM_TR_End ();

   RowEvenOdd = 1 - RowEvenOdd;
  }

/*****************************************************************************/
/***************** Update my last click in current course ********************/
/*****************************************************************************/

void Crs_UpdateCrsLast (void)
  {
   if (Gbl.Hierarchy.Level == Hie_CRS &&	// Course selected
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
   Lay_PutContextualLinkIconText (ActReqRemOldCrs,NULL,NULL,
				  "trash.svg",
				  Txt_Eliminate_old_courses);
  }

/*****************************************************************************/
/********************** Write form to remove old courses *********************/
/*****************************************************************************/

void Crs_AskRemoveOldCrss (void)
  {
   extern const char *Hlp_SYSTEM_Hierarchy_eliminate_old_courses;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Eliminate_old_courses;
   extern const char *Txt_Eliminate_all_courses_whithout_users_PART_1_OF_2;
   extern const char *Txt_Eliminate_all_courses_whithout_users_PART_2_OF_2;
   extern const char *Txt_Eliminate;
   unsigned MonthsWithoutAccess = Crs_DEF_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS;
   unsigned i;

   /***** Begin form *****/
   Frm_StartForm (ActRemOldCrs);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Eliminate_old_courses,NULL,
                 Hlp_SYSTEM_Hierarchy_eliminate_old_courses,Box_NOT_CLOSABLE);

   /***** Form to request number of months without clicks *****/
   fprintf (Gbl.F.Out,"<label class=\"%s\">%s&nbsp;"
	              "<select name=\"Months\">",
            The_ClassFormInBox[Gbl.Prefs.Theme],
            Txt_Eliminate_all_courses_whithout_users_PART_1_OF_2);
   for (i  = Crs_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS;
        i <= Crs_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS;
        i++)
     {
      fprintf (Gbl.F.Out,"<option");
      if (i == MonthsWithoutAccess)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%u</option>",i);
     }
   fprintf (Gbl.F.Out,"</select>&nbsp;");
   fprintf (Gbl.F.Out,Txt_Eliminate_all_courses_whithout_users_PART_2_OF_2,
            Cfg_PLATFORM_SHORT_NAME);
   fprintf (Gbl.F.Out,"</label>");

   /***** Send button and end box *****/
   Box_EndBoxWithButton (Btn_REMOVE_BUTTON,Txt_Eliminate);

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
   MYSQL_ROW row;
   unsigned long NumCrs;
   unsigned long NumCrss;
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
   NumCrss = DB_QuerySELECT (&mysql_res,"can not get old courses",
			     "SELECT CrsCod FROM crs_last WHERE"
			     " LastTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)"
			     " AND CrsCod NOT IN (SELECT DISTINCT CrsCod FROM crs_usr)",
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
         row = mysql_fetch_row (mysql_res);
         CrsCod = Str_ConvertStrCodToLongCod (row[0]);
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
   Rol_Role_t Role;

   /***** Pointer must be NULL *****/
   if (Crs_EditingCrs != NULL)
      Lay_ShowErrorAndExit ("Error initializing course.");

   /***** Allocate memory for course *****/
   if ((Crs_EditingCrs = (struct Course *) malloc (sizeof (struct Course))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for course.");

   /***** Reset course *****/
   Crs_EditingCrs->CrsCod      = -1L;
   Crs_EditingCrs->InstitutionalCrsCod[0] = '\0';
   Crs_EditingCrs->DegCod      = -1L;
   Crs_EditingCrs->Year        = 0;
   Crs_EditingCrs->Status      = 0;
   Crs_EditingCrs->ShrtName[0] = '\0';
   Crs_EditingCrs->FullName[0] = '\0';
   for (Role = (Rol_Role_t) 0;
	Role < Rol_NUM_ROLES;
	Role++)
      Crs_EditingCrs->NumUsrs[Role] = 0;
  }

static void Crs_EditingCourseDestructor (void)
  {
   /***** Free memory used for course *****/
   if (Crs_EditingCrs != NULL)
     {
      free ((void *) Crs_EditingCrs);
      Crs_EditingCrs = NULL;
     }
  }
