// swad_course_config.c: configuration of current course

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_course_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_config.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_indicator.h"
#include "swad_logo.h"
#include "swad_parameter.h"
#include "swad_role.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void CrsCfg_PutIconToPrint (__attribute__((unused)) void *Args);
static void CrsCfg_Degree (Vie_ViewType_t ViewType,Frm_PutForm_t PutForm);
static void CrsCfg_Year (Frm_PutForm_t PutForm);
static void CrsCfg_InstitutionalCode (Frm_PutForm_t PutForm);
static void CrsCfg_InternalCode (void);
static void CrsCfg_Indicators (void);

/*****************************************************************************/
/***************** Configuration of the current course ***********************/
/*****************************************************************************/

void CrsCfg_Configuration (Vie_ViewType_t ViewType)
  {
   extern const char *Hlp_COURSE_Information;
   extern const char *Txt_NULL;
   static struct
     {
      void (*FunctionToDrawContextualIcons) (void *Args);
      const char **HelpLink;
     } BoxArgs[Vie_NUM_VIEW_TYPES] =
     {
      [Vie_VIEW  ] = {CrsCfg_PutIconToPrint	,&Hlp_COURSE_Information	},
      [Vie_EDIT  ] = {NULL			,&Txt_NULL			},
      [Vie_CONFIG] = {NULL			,&Txt_NULL			},
      [Vie_PRINT ] = {NULL			,&Txt_NULL			},
     };
   Hie_PutLink_t PutLink;
   Frm_PutForm_t PutFormDeg;
   Frm_PutForm_t PutFormName;
   Frm_PutForm_t PutFormYear;
   Frm_PutForm_t PutFormInsCod;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Node[Hie_CRS].HieCod <= 0)	// No course selected
      return;

   /***** Initializations *****/
   PutLink       = ViewType == Vie_VIEW &&
		   Gbl.Hierarchy.Node[Hie_DEG].WWW[0] ? Hie_PUT_LINK :
							Hie_DONT_PUT_LINK;
   PutFormDeg    = ViewType == Vie_VIEW &&
	           Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM ? Frm_PUT_FORM :
							    Frm_DONT_PUT_FORM;
   PutFormName   = ViewType == Vie_VIEW &&
		   Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM ? Frm_PUT_FORM :
							    Frm_DONT_PUT_FORM;
   PutFormYear   =
   PutFormInsCod = ViewType == Vie_VIEW &&
		   Gbl.Usrs.Me.Role.Logged >= Rol_TCH ? Frm_PUT_FORM :
							Frm_DONT_PUT_FORM;

   /***** Contextual menu *****/
   if (ViewType == Vie_VIEW &&
       (Gbl.Usrs.Me.Role.Logged == Rol_GST ||
	Gbl.Usrs.Me.Role.Logged == Rol_USR))
     {
      Mnu_ContextMenuBegin ();
	 Enr_PutLinkToRequestSignUp ();	// Request enrolment in the current course
      Mnu_ContextMenuEnd ();
     }

   /***** Begin box *****/
   Box_BoxBegin (NULL,BoxArgs[ViewType].FunctionToDrawContextualIcons,NULL,
		 *BoxArgs[ViewType].HelpLink,Box_NOT_CLOSABLE);

      /***** Title *****/
      HieCfg_Title (PutLink,Hie_CRS);

      /**************************** Left part ***********************************/
      HTM_DIV_Begin ("class=\"HIE_CFG_LEFT\"");

	 /***** Begin table *****/
	 HTM_TABLE_BeginCenterPadding (2);

	    /***** Degree *****/
	    CrsCfg_Degree (ViewType,PutFormDeg);

	    /***** Course name *****/
	    HieCfg_Name (PutFormName,Hie_CRS,Nam_FULL_NAME);
	    HieCfg_Name (PutFormName,Hie_CRS,Nam_SHRT_NAME);

	    /***** Course year *****/
	    CrsCfg_Year (PutFormYear);

	    if (ViewType == Vie_VIEW)
	      {
	       /***** Institutional code of the course *****/
	       CrsCfg_InstitutionalCode (PutFormInsCod);

	       /***** Internal code of the course *****/
	       CrsCfg_InternalCode ();
	      }

	    /***** Shortcut to the couse *****/
	    HieCfg_Shortcut (ViewType,ParCod_Crs,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

	    switch (ViewType)
	      {
	       case Vie_VIEW:
		  /***** Number of users *****/
		  HieCfg_NumUsrsInCrss (Hie_CRS,Gbl.Hierarchy.Node[Hie_CRS].HieCod,Rol_TCH);
		  HieCfg_NumUsrsInCrss (Hie_CRS,Gbl.Hierarchy.Node[Hie_CRS].HieCod,Rol_NET);
		  HieCfg_NumUsrsInCrss (Hie_CRS,Gbl.Hierarchy.Node[Hie_CRS].HieCod,Rol_STD);
		  HieCfg_NumUsrsInCrss (Hie_CRS,Gbl.Hierarchy.Node[Hie_CRS].HieCod,Rol_UNK);

		  /***** Indicators *****/
		  CrsCfg_Indicators ();
		  break;
	       case Vie_PRINT:
		  /***** QR code with link to the course *****/
		  HieCfg_QR (ParCod_Crs,Gbl.Hierarchy.Node[Hie_CRS].HieCod);
		  break;
	       default:
		  Err_WrongTypeExit ();
		  break;
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();

      /***** End of left part *****/
      HTM_DIV_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************* Put icon to print the configuration of a course ***************/
/*****************************************************************************/

static void CrsCfg_PutIconToPrint (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToPrint (ActPrnCrsInf,NULL,NULL);
  }

/*****************************************************************************/
/***************** Print configuration of the current course *****************/
/*****************************************************************************/

void CrsCfg_PrintConfiguration (void)
  {
   CrsCfg_Configuration (Vie_PRINT);
  }

/*****************************************************************************/
/******************** Show degree in course configuration ********************/
/*****************************************************************************/

static void CrsCfg_Degree (Vie_ViewType_t ViewType,Frm_PutForm_t PutForm)
  {
   extern const char *Par_CodeStr[Par_NUM_PAR_COD];
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   unsigned NumDeg;
   const struct Hie_Node *Deg;
   const char *Id[Frm_NUM_PUT_FORM] =
     {
      [Frm_DONT_PUT_FORM] = NULL,
      [Frm_PUT_FORM     ] = Par_CodeStr[ParCod_OthDeg],
     };

   /***** Degree *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",Id[PutForm],Txt_HIERARCHY_SINGUL_Abc[Hie_DEG]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LT DAT_%s\"",The_GetSuffix ());
         switch (PutForm)
           {
            case Frm_DONT_PUT_FORM:	// I can not move course to another degree
	       if (ViewType == Vie_VIEW)
		 {
		  Frm_BeginFormGoTo (ActSeeDegInf);
		     ParCod_PutPar (ParCod_Deg,Gbl.Hierarchy.Node[Hie_DEG].HieCod);
		     HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Gbl.Hierarchy.Node[Hie_DEG].ShrtName),NULL,
					      "class=\"LT BT_LINK\"");
		     Str_FreeGoToTitle ();
		 }
	       Lgo_DrawLogo (Hie_DEG,&Gbl.Hierarchy.Node[Hie_DEG],"LM ICO20x20");
	       HTM_NBSP ();
	       HTM_Txt (Gbl.Hierarchy.Node[Hie_DEG].FullName);
	       if (ViewType == Vie_VIEW)
		 {
		     HTM_BUTTON_End ();
		  Frm_EndForm ();
		 }
               break;
            case Frm_PUT_FORM:
	       /* Get list of degrees of the current center */
	       Deg_GetListDegsInCurrentCtr ();

	       /* Put form to select degree */
	       Frm_BeginForm (ActChgCrsDegCfg);
		  HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
				    "id=\"OthDegCod\" name=\"OthDegCod\""
				    " class=\"Frm_C2_INPUT INPUT_%s\"",
				    The_GetSuffix ());
		     for (NumDeg = 0;
			  NumDeg < Gbl.Hierarchy.List[Hie_CTR].Num;
			  NumDeg++)
		       {
			Deg = &Gbl.Hierarchy.List[Hie_CTR].Lst[NumDeg];
			HTM_OPTION (HTM_Type_LONG,&Deg->HieCod,
				    Deg->HieCod == Gbl.Hierarchy.Node[Hie_DEG].HieCod ? HTM_SELECTED :
											HTM_NO_ATTR,
				    "%s",Deg->ShrtName);
		       }
		  HTM_SELECT_End ();
	       Frm_EndForm ();

	       /* Free list of degrees of the current center */
	       Hie_FreeList (Hie_CTR);
               break;
           }
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Show course year in course configuration ******************/
/*****************************************************************************/

static void CrsCfg_Year (Frm_PutForm_t PutForm)
  {
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_Not_applicable;
   unsigned Year;
   static const char *Id[Frm_NUM_PUT_FORM] =
     {
      [Frm_DONT_PUT_FORM] = NULL,
      [Frm_PUT_FORM     ] = "OthCrsYear",
     };

   /***** Academic year *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",Id[PutForm],Txt_Year_OF_A_DEGREE);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
         switch (PutForm)
           {
            case Frm_DONT_PUT_FORM:
	       HTM_Txt (Gbl.Hierarchy.Node[Hie_CRS].Specific.Year ? Txt_YEAR_OF_DEGREE[Gbl.Hierarchy.Node[Hie_CRS].Specific.Year] :
								    Txt_Not_applicable);
               break;
            case Frm_PUT_FORM:
	       Frm_BeginForm (ActChgCrsYeaCfg);
		  HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
				    "id=\"OthCrsYear\" name=\"OthCrsYear\""
				    " class=\"Frm_C2_INPUT INPUT_%s\"",
				    The_GetSuffix ());
		     for (Year  = 0;
			  Year <= Deg_MAX_YEARS_PER_DEGREE;
			  Year++)
			HTM_OPTION (HTM_Type_UNSIGNED,&Year,
				    Year == Gbl.Hierarchy.Node[Hie_CRS].Specific.Year ? HTM_SELECTED :
											HTM_NO_ATTR,
				    "%s",Txt_YEAR_OF_DEGREE[Year]);
		  HTM_SELECT_End ();
	       Frm_EndForm ();
               break;
           }
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************* Show institutional code in course configuration ***************/
/*****************************************************************************/

static void CrsCfg_InstitutionalCode (Frm_PutForm_t PutForm)
  {
   extern const char *Txt_Institutional_code;
   static const char *Id[Frm_NUM_PUT_FORM] =
     {
      [Frm_DONT_PUT_FORM] = NULL,
      [Frm_PUT_FORM     ] = "InsCrsCod",
     };

   /***** Institutional course code *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",Id[PutForm],Txt_Institutional_code);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
         switch (PutForm)
           {
            case Frm_DONT_PUT_FORM:
               HTM_Txt (Gbl.Hierarchy.Node[Hie_CRS].InstitutionalCod);
               break;
            case Frm_PUT_FORM:
	       Frm_BeginForm (ActChgInsCrsCodCfg);
		  HTM_INPUT_TEXT ("InsCrsCod",Hie_MAX_CHARS_INSTITUTIONAL_COD,
				  Gbl.Hierarchy.Node[Hie_CRS].InstitutionalCod,
				  HTM_SUBMIT_ON_CHANGE,
				  "id=\"InsCrsCod\" maxlength=\"%u\""
				  " class=\"Frm_C2_INPUT INPUT_%s\"",
				  Hie_MAX_CHARS_INSTITUTIONAL_COD,
				  The_GetSuffix ());
	       Frm_EndForm ();
               break;
           }
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/**************** Show internal code in course configuration *****************/
/*****************************************************************************/

static void CrsCfg_InternalCode (void)
  {
   extern const char *Txt_Internal_code;

   /***** Internal course code *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_Internal_code);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 HTM_Long (Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/****************** Show indicators in course configuration ******************/
/*****************************************************************************/

static void CrsCfg_Indicators (void)
  {
   extern const char *Txt_Indicators;
   extern const char *Txt_of_PART_OF_A_TOTAL;
   struct Ind_IndicatorsCrs IndicatorsCrs;
   int NumIndicatorsFromDB = Ind_GetNumIndicatorsCrsFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod);
   char *Title;

   /***** Compute indicators ******/
   Ind_ComputeAndStoreIndicatorsCrs (Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				     NumIndicatorsFromDB,&IndicatorsCrs);

   /***** Number of indicators *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_Indicators);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 Frm_BeginForm (ActReqStaCrs);
	    if (asprintf (&Title,"%u %s %u",
			  IndicatorsCrs.NumIndicators,
			  Txt_of_PART_OF_A_TOTAL,Ind_NUM_INDICATORS) < 0)
	       Err_NotEnoughMemoryExit ();
	    HTM_BUTTON_Submit_Begin (Title,NULL,"class=\"LB BT_LINK\"");
	       HTM_NBSP ();
	       HTM_Txt (Title);
	       if (IndicatorsCrs.NumIndicators == Ind_NUM_INDICATORS)
		  Ico_PutIcon ("check-circle.svg",Ico_GREEN,
			       Title,"ICO16x16");
	       else
		  Ico_PutIcon ("exclamation-triangle.svg",Ico_YELLOW,
			       Title,"ICO16x16");
	    HTM_BUTTON_End ();
	    free (Title);
	 Frm_EndForm ();
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Change the degree of the current course *******************/
/*****************************************************************************/

void CrsCfg_ChangeCrsDeg (void)
  {
   extern Err_SuccessOrError_t (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_The_course_X_has_been_moved_to_the_degree_Y;
   struct Hie_Node NewDeg;
   __attribute__((unused)) Err_SuccessOrError_t SuccessOrError;
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Get parameter with degree code *****/
   NewDeg.HieCod = ParCod_GetAndCheckPar (ParCod_OthDeg);

   /***** Check if degree has changed *****/
   if (NewDeg.HieCod != Gbl.Hierarchy.Node[Hie_CRS].PrtCod)
     {
      /***** Get data of new degree *****/
      SuccessOrError = Hie_GetDataByCod[Hie_DEG] (&NewDeg);

      /***** If name of course was not in database in the new degree... *****/
      Names[Nam_SHRT_NAME] = Gbl.Hierarchy.Node[Hie_CRS].ShrtName;
      Names[Nam_FULL_NAME] = Gbl.Hierarchy.Node[Hie_CRS].FullName;
      if (Nam_CheckIfNameExists (Crs_DB_CheckIfCrsNameExistsInYearOfDeg,
				 Names,
				 -1L,
				 NewDeg.HieCod,
				 Gbl.Hierarchy.Node[Hie_CRS].Specific.Year) == Exi_DOES_NOT_EXIST)
	{
	 /***** Update degree in table of courses *****/
	 Crs_DB_UpdateDegOfCurrentCrs (NewDeg.HieCod);
	 Gbl.Hierarchy.Node[Hie_CRS].PrtCod =
	 Gbl.Hierarchy.Node[Hie_DEG].HieCod = NewDeg.HieCod;

	 /***** Initialize again current course, degree, center... *****/
      	 Hie_InitHierarchy ();

	 /***** Create alert to show the change made *****/
	 Ale_CreateAlert (Ale_SUCCESS,NULL,
	                  Txt_The_course_X_has_been_moved_to_the_degree_Y,
		          Names[Nam_FULL_NAME],
		          Gbl.Hierarchy.Node[Hie_DEG].FullName);
	}
     }
  }

/*****************************************************************************/
/*************** Change the name of a course in configuration ****************/
/*****************************************************************************/

void CrsCfg_RenameCourseShrt (void)
  {
   Crs_RenameCourse (&Gbl.Hierarchy.Node[Hie_CRS],Nam_SHRT_NAME);
  }

void CrsCfg_RenameCourseFull (void)
  {
   Crs_RenameCourse (&Gbl.Hierarchy.Node[Hie_CRS],Nam_FULL_NAME);
  }

/*****************************************************************************/
/*********** Change the year of a course in course configuration *************/
/*****************************************************************************/

void CrsCfg_ChangeCrsYear (void)
  {
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_The_year_of_the_course_X_has_changed;
   extern const char *Txt_The_year_X_is_not_allowed;
   char YearStr[2 + 1];
   unsigned NewYear;
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Get parameter with year/semester *****/
   Par_GetParText ("OthCrsYear",YearStr,2);
   NewYear = Deg_ConvStrToYear (YearStr);

   if (NewYear <= Deg_MAX_YEARS_PER_DEGREE)	// If year is valid
     {
      /***** If name of course was not in database in the new year... *****/
      Names[Nam_SHRT_NAME] = Gbl.Hierarchy.Node[Hie_CRS].ShrtName;
      Names[Nam_FULL_NAME] = Gbl.Hierarchy.Node[Hie_CRS].FullName;
      if (Nam_CheckIfNameExists (Crs_DB_CheckIfCrsNameExistsInYearOfDeg,
				 Names,
				 -1L,
				 Gbl.Hierarchy.Node[Hie_CRS].PrtCod,
				 NewYear) == Exi_DOES_NOT_EXIST)
	{
	 /***** Update year in table of courses *****/
         Crs_UpdateCrsYear (&Gbl.Hierarchy.Node[Hie_CRS],NewYear);

	 /***** Create alert to show the change made *****/
	 Ale_CreateAlert (Ale_SUCCESS,NULL,
			  Txt_The_year_of_the_course_X_has_changed,
		          Names[Nam_FULL_NAME]);
	}
     }
   else	// Year not valid
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_The_year_X_is_not_allowed,
		       NewYear);
  }

/*****************************************************************************/
/***** Change the institutional code of a course in course configuration *****/
/*****************************************************************************/

void CrsCfg_ChangeInsCrsCod (void)
  {
   extern const char *Txt_The_institutional_code_of_the_course_X_has_changed_to_Y;
   extern const char *Txt_The_institutional_code_of_the_course_X_has_not_changed;
   char NewInstitutionalCrsCod[Hie_MAX_BYTES_INSTITUTIONAL_COD + 1];

   /***** Get institutional code from form *****/
   Par_GetParText ("InsCrsCod",NewInstitutionalCrsCod,Hie_MAX_BYTES_INSTITUTIONAL_COD);

   /***** Change the institutional course code *****/
   if (strcmp (NewInstitutionalCrsCod,Gbl.Hierarchy.Node[Hie_CRS].InstitutionalCod))
     {
      Crs_UpdateInstitutionalCrsCod (&Gbl.Hierarchy.Node[Hie_CRS],NewInstitutionalCrsCod);

      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_institutional_code_of_the_course_X_has_changed_to_Y,
	               Gbl.Hierarchy.Node[Hie_CRS].ShrtName,
		       NewInstitutionalCrsCod);
     }
   else	// The same institutional code
      Ale_CreateAlert (Ale_INFO,NULL,
	               Txt_The_institutional_code_of_the_course_X_has_not_changed,
	               Gbl.Hierarchy.Node[Hie_CRS].ShrtName);
  }

/*****************************************************************************/
/** Show message of success after changing a course in course configuration **/
/*****************************************************************************/

void CrsCfg_ContEditAfterChgCrs (void)
  {
   /***** Write error/success message *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Crs_ShowIntroduction ();
  }
