// swad_hierarchy.c: hierarchy (system, institution, center, degree, course)

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
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_center_database.h"
#include "swad_country_database.h"
#include "swad_course_database.h"
#include "swad_database.h"
#include "swad_degree_database.h"
#include "swad_enrolment_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_group_database.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_database.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_institution_database.h"
#include "swad_logo.h"
#include "swad_parameter_code.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

static FigCch_FigureCached_t Hie_FiguresCached[Hie_NUM_LEVELS] =
  {
   [Hie_CTY] = FigCch_NUM_CTYS,
   [Hie_INS] = FigCch_NUM_INSS,
   [Hie_CTR] = FigCch_NUM_CTRS,
   [Hie_DEG] = FigCch_NUM_DEGS,
   [Hie_CRS] = FigCch_NUM_CRSS,
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Hie_DrawLogo (void);

static Hie_StatusTxt_t Hie_GetStatusTxtFromStatusBits (Hie_Status_t Status);
static Hie_Status_t Hie_GetStatusBitsFromStatusTxt (Hie_StatusTxt_t StatusTxt);

static void Hie_WriteHeadHierarchy (void);
static void Hie_GetAndShowHierarchyWithNodes (Hie_Level_t HavingNodesOfLevel);
static void Hie_GetAndShowHierarchyWithUsrs (Rol_Role_t Role);
static void Hie_GetAndShowHierarchyTotal (void);
static void Hie_ShowHierarchyRow (const char *Text1,const char *Text2,
				  const char *ClassTxt,
				  int NumNodes[Hie_NUM_LEVELS]);
static void Hie_ShowHierarchyCell (const char *ClassTxt,int Num);

/*****************************************************************************/
/********** List pending institutions, centers, degrees and courses **********/
/*****************************************************************************/

void Hie_SeePending (void)
  {
   /***** List countries with pending institutions *****/
   Cty_SeeCtyWithPendingInss ();

   /***** List institutions with pending centers *****/
   Ins_SeeInsWithPendingCtrs ();

   /***** List centers with pending degrees *****/
   Ctr_SeeCtrWithPendingDegs ();

   /***** List degrees with pending courses *****/
   Deg_SeeDegWithPendingCrss ();
  }

/*****************************************************************************/
/*** Write menu to select country, institution, center, degree and course ****/
/*****************************************************************************/

void Hie_WriteMenuHierarchy (void)
  {
   extern const char *Par_CodeStr[];
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Center;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;

   /***** Begin table *****/
   HTM_TABLE_BeginCenterPadding (2);

      /***** Write a 1st selector with all countries *****/
      HTM_TR_Begin (NULL);

	 /* Label */
	 Frm_LabelColumn ("RT",Par_CodeStr[ParCod_Cty],Txt_Country);

	 /* Data */
	 HTM_TD_Begin ("class=\"LT\"");
	    Cty_WriteSelectorOfCountry ();
	 HTM_TD_End ();

      HTM_TR_End ();

      if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)
	{
	 /***** Write a 2nd selector
		with the institutions of selected country *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT",Par_CodeStr[ParCod_Ins],Txt_Institution);

	    /* Data */
	    HTM_TD_Begin ("class=\"LT\"");
	       Ins_WriteSelectorOfInstitution ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)
	   {
	    /***** Write a 3rd selector
		   with all the centers of selected institution *****/
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("RT",Par_CodeStr[ParCod_Ctr],Txt_Center);

	       /* Data */
	       HTM_TD_Begin ("class=\"LT\"");
		  Ctr_WriteSelectorOfCenter ();
	       HTM_TD_End ();

	    HTM_TR_End ();

	    if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)
	      {
	       /***** Write a 4th selector
		      with all degrees of selected center *****/
	       HTM_TR_Begin (NULL);

		  /* Label */
		  Frm_LabelColumn ("RT",Par_CodeStr[ParCod_Deg],Txt_Degree);

		  /* Data */
		  HTM_TD_Begin ("class=\"LT\"");
		     Deg_WriteSelectorOfDegree ();
		  HTM_TD_End ();

	       HTM_TR_End ();

	       if (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0)
		 {
		  /***** Write a 5th selector
			 with all courses of selected degree *****/
		  HTM_TR_Begin (NULL);

		     /* Label */
		     Frm_LabelColumn ("RT",Par_CodeStr[ParCod_Crs],Txt_Course);

		     /* Data */
		     HTM_TD_Begin ("class=\"LT\"");
			Crs_WriteSelectorOfCourse ();
		     HTM_TD_End ();

		  HTM_TR_End ();
		 }
	      }
	   }
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************* Write hierarchy breadcrumb in the top of the page *************/
/*****************************************************************************/

void Hie_WriteHierarchyInBreadcrumb (void)
  {
   extern const char *Txt_System;
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Center;
   extern const char *Txt_Degree;

   /***** Form to go to the system *****/
   HTM_DIV_Begin ("class=\"BC BC_%s\"",The_GetSuffix ());

      HTM_NBSP ();

      Frm_BeginFormGoTo (ActMnu);
	 Par_PutParUnsigned (NULL,"NxtTab",(unsigned) TabSys);
	 HTM_BUTTON_Submit_Begin (Txt_System,"class=\"BT_LINK\"");
	    HTM_Txt (Txt_System);
	 HTM_BUTTON_End ();
      Frm_EndForm ();

   HTM_DIV_End ();

   if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)		// Country selected...
     {
      HTM_DIV_Begin ("class=\"BC BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_Txt ("&nbsp;&gt;&nbsp;");

	 /***** Form to go to see institutions of this country *****/
	 Frm_BeginFormGoTo (ActSeeIns);
	    ParCod_PutPar (ParCod_Cty,Gbl.Hierarchy.Node[Hie_CTY].HieCod);
	    HTM_BUTTON_Submit_Begin (Gbl.Hierarchy.Node[Hie_CTY].FullName,
	                             "class=\"BT_LINK\"");
	       HTM_Txt (Gbl.Hierarchy.Node[Hie_CTY].FullName);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();

      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"BC BC_SEMIOFF BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_Txt ("&nbsp;&gt;&nbsp;");

	 /***** Form to go to select countries *****/
	 Frm_BeginFormGoTo (ActSeeCty);
	    HTM_BUTTON_Submit_Begin (Txt_Country,"class=\"BT_LINK\"");
	       HTM_Txt (Txt_Country);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();

      HTM_DIV_End ();
     }

   if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)		// Institution selected...
     {
      HTM_DIV_Begin ("class=\"BC BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_Txt ("&nbsp;&gt;&nbsp;");

	 /***** Form to see centers of this institution *****/
	 Frm_BeginFormGoTo (ActSeeCtr);
	    ParCod_PutPar (ParCod_Ins,Gbl.Hierarchy.Node[Hie_INS].HieCod);
	    HTM_BUTTON_Submit_Begin (Gbl.Hierarchy.Node[Hie_INS].FullName,
	                             "class=\"BT_LINK\"");
	       HTM_Txt (Gbl.Hierarchy.Node[Hie_INS].ShrtName);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();

      HTM_DIV_End ();
     }
   else if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)
     {
      HTM_DIV_Begin ("class=\"BC BC_SEMIOFF BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_Txt ("&nbsp;&gt;&nbsp;");

	 /***** Form to go to select institutions *****/
	 Frm_BeginFormGoTo (ActSeeIns);
	    HTM_BUTTON_Submit_Begin (Txt_Institution,"class=\"BT_LINK\"");
	       HTM_Txt (Txt_Institution);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();

      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"BC BC_OFF BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_Txt ("&nbsp;&gt;&nbsp;");

	 /***** Hidden institution *****/
	 HTM_Txt (Txt_Institution);

      HTM_DIV_End ();
     }

   if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)	// Center selected...
     {
      HTM_DIV_Begin ("class=\"BC BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_Txt ("&nbsp;&gt;&nbsp;");

	 /***** Form to see degrees of this center *****/
	 Frm_BeginFormGoTo (ActSeeDeg);
	    ParCod_PutPar (ParCod_Ctr,Gbl.Hierarchy.Node[Hie_CTR].HieCod);
	    HTM_BUTTON_Submit_Begin (Gbl.Hierarchy.Node[Hie_CTR].FullName,
	                             "class=\"BT_LINK\"");
	       HTM_Txt (Gbl.Hierarchy.Node[Hie_CTR].ShrtName);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();

      HTM_DIV_End ();
     }
   else if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)
     {
      HTM_DIV_Begin ("class=\"BC BC_SEMIOFF BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_Txt ("&nbsp;&gt;&nbsp;");

	 /***** Form to go to select centers *****/
	 Frm_BeginFormGoTo (ActSeeCtr);
	    HTM_BUTTON_Submit_Begin (Txt_Center,"class=\"BT_LINK\"");
	       HTM_Txt (Txt_Center);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();

      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"BC BC_OFF BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_Txt ("&nbsp;&gt;&nbsp;");

	 /***** Hidden center *****/
	 HTM_Txt (Txt_Center);

      HTM_DIV_End ();
     }

   if (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0)	// Degree selected...
     {
      HTM_DIV_Begin ("class=\"BC BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_Txt ("&nbsp;&gt;&nbsp;");

	 /***** Form to go to see courses of this degree *****/
	 Frm_BeginFormGoTo (ActSeeCrs);
	    ParCod_PutPar (ParCod_Deg,Gbl.Hierarchy.Node[Hie_DEG].HieCod);
	    HTM_BUTTON_Submit_Begin (Gbl.Hierarchy.Node[Hie_DEG].FullName,
	                             "class=\"BT_LINK\"");
	       HTM_Txt (Gbl.Hierarchy.Node[Hie_DEG].ShrtName);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();

      HTM_DIV_End ();
     }
   else if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)
     {
      HTM_DIV_Begin ("class=\"BC BC_SEMIOFF BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_Txt ("&nbsp;&gt;&nbsp;");

	 /***** Form to go to select degrees *****/
	 Frm_BeginFormGoTo (ActSeeDeg);
	    HTM_BUTTON_Submit_Begin (Txt_Degree,"class=\"BT_LINK\"");
	       HTM_Txt (Txt_Degree);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();

      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"BC BC_OFF BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_Txt ("&nbsp;&gt;&nbsp;");

	 /***** Hidden degree *****/
	 HTM_Txt (Txt_Degree);

      HTM_DIV_End ();
     }

   HTM_DIV_Begin ("class=\"BC%s BC_%s\"",
		   (Gbl.Hierarchy.Level == Hie_CRS) ? "" :
		  ((Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0) ? " BC_SEMIOFF" :
							      " BC_OFF"),
		  The_GetSuffix ());

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

   HTM_DIV_End ();
  }

/*****************************************************************************/
/*************** Write course full name in the top of the page ***************/
/*****************************************************************************/

void Hie_WriteBigNameCtyInsCtrDegCrs (void)
  {
   static Tab_Tab_t NextTab[Hie_NUM_LEVELS] =
     {
      [Hie_UNK] = TabUnk,
      [Hie_SYS] = TabSys,
      [Hie_CTY] = TabCty,
      [Hie_INS] = TabIns,
      [Hie_CTR] = TabCtr,
      [Hie_DEG] = TabDeg,
      [Hie_CRS] = TabCrs,
     };

   HTM_TxtF ("<h1 id=\"main_title\" class=\"MAIN_TITLE_%s\">",
	     The_GetSuffix ());

      /***** Logo and text *****/
      HTM_DIV_Begin ("id=\"big_name_container\"");

	 Frm_BeginForm (ActMnu);
	    Par_PutParUnsigned (NULL,"NxtTab",(unsigned) NextTab[Gbl.Hierarchy.Level]);
	    HTM_BUTTON_Submit_Begin (Gbl.Hierarchy.Node[Gbl.Hierarchy.Level].ShrtName,
				     "class=\"BT_LINK ICO_HIGHLIGHT\"");

	       HTM_DIV_Begin ("id=\"big_full_name\"");	// Full name
		     Hie_DrawLogo ();
		     HTM_Txt (Gbl.Hierarchy.Node[Gbl.Hierarchy.Level].FullName);
	       HTM_DIV_End ();

	       HTM_DIV_Begin ("id=\"big_short_name\"");	// Short name
		     Hie_DrawLogo ();
		     HTM_Txt (Gbl.Hierarchy.Node[Gbl.Hierarchy.Level].ShrtName);
	       HTM_DIV_End ();

	    HTM_BUTTON_End ();
	 Frm_EndForm ();

      HTM_DIV_End ();

   HTM_TxtF ("</h1>");
  }

/*****************************************************************************/
/********************** Draw logo in the top of the page *********************/
/*****************************************************************************/

static void Hie_DrawLogo (void)
  {
   static Hie_Level_t LogoScope[Hie_NUM_LEVELS] =
     {
      [Hie_INS] = Hie_INS,
      [Hie_CTR] = Hie_CTR,
      [Hie_DEG] = Hie_DEG,
      [Hie_CRS] = Hie_DEG,	// Draw logo of degree
     };
   static const long *LogoCode[Hie_NUM_LEVELS] =
     {
      [Hie_INS] = &Gbl.Hierarchy.Node[Hie_INS].HieCod,
      [Hie_CTR] = &Gbl.Hierarchy.Node[Hie_CTR].HieCod,
      [Hie_DEG] = &Gbl.Hierarchy.Node[Hie_DEG].HieCod,
      [Hie_CRS] = &Gbl.Hierarchy.Node[Hie_DEG].HieCod,	// Degree code
     };

   /***** Logo *****/
   switch (Gbl.Hierarchy.Level)
     {
      case Hie_SYS:	// System
	 Ico_PutIcon ("swad64x64.png",Ico_UNCHANGED,
		      Gbl.Hierarchy.Node[Gbl.Hierarchy.Level].ShrtName,"TOP_LOGO");
	 break;
      case Hie_CTY:	// Country
	 Cty_DrawCountryMap (&Gbl.Hierarchy.Node[Hie_CTY],"TOP_LOGO");
	 break;
      default:
	 Lgo_DrawLogo (LogoScope[Gbl.Hierarchy.Level],
		       *LogoCode[Gbl.Hierarchy.Level],
		       Gbl.Hierarchy.Node[Gbl.Hierarchy.Level].ShrtName,
		       40,"TOP_LOGO");
	 break;
     }
  }

/*****************************************************************************/
/**************** Copy last hierarchy to current hierarchy *******************/
/*****************************************************************************/

void Hie_SetHierarchyFromUsrLastHierarchy (void)
  {
   /***** Initialize all codes to -1 *****/
   Hie_ResetHierarchy ();

   /***** Copy last hierarchy code to current hierarchy *****/
   Gbl.Hierarchy.Node[Gbl.Usrs.Me.UsrLast.LastHie.Level].HieCod = Gbl.Usrs.Me.UsrLast.LastHie.HieCod;

   /****** Initialize again current course, degree, center... ******/
   Hie_InitHierarchy ();
  }

/*****************************************************************************/
/**** Initialize current country, institution, center, degree and course *****/
/*****************************************************************************/

void Hie_InitHierarchy (void)
  {
   /***** If course code is available, get course data *****/
   if (Gbl.Hierarchy.Node[Hie_CRS].HieCod > 0)
     {
      if (Crs_GetCourseDataByCod (&Gbl.Hierarchy.Node[Hie_CRS]))	// Course found
         Gbl.Hierarchy.Node[Hie_DEG].HieCod = Gbl.Hierarchy.Node[Hie_CRS].PrtCod;
      else
         Hie_ResetHierarchy ();
     }

   /***** If degree code is available, get degree data *****/
   if (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0)
     {
      if (Deg_GetDegreeDataByCod (&Gbl.Hierarchy.Node[Hie_DEG]))	// Degree found
	{
	 Gbl.Hierarchy.Node[Hie_CTR].HieCod = Gbl.Hierarchy.Node[Hie_DEG].PrtCod;
         Gbl.Hierarchy.Node[Hie_INS].HieCod = Deg_DB_GetInsCodOfDegreeByCod (Gbl.Hierarchy.Node[Hie_DEG].HieCod);
	}
      else
         Hie_ResetHierarchy ();
     }

   /***** If center code is available, get center data *****/
   if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)
     {
      if (Ctr_GetCenterDataByCod (&Gbl.Hierarchy.Node[Hie_CTR]))	// Center found
         Gbl.Hierarchy.Node[Hie_INS].HieCod = Gbl.Hierarchy.Node[Hie_CTR].PrtCod;
      else
         Hie_ResetHierarchy ();
     }

   /***** If institution code is available, get institution data *****/
   if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)
     {
      if (Ins_GetInstitDataByCod (&Gbl.Hierarchy.Node[Hie_INS]))	// Institution found
	 Gbl.Hierarchy.Node[Hie_CTY].HieCod = Gbl.Hierarchy.Node[Hie_INS].PrtCod;
      else
         Hie_ResetHierarchy ();
     }

   /***** If country code is available, get country data *****/
   if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)
      if (!Cty_GetBasicCountryDataByCod (&Gbl.Hierarchy.Node[Hie_CTY]))		// Country not found
         Hie_ResetHierarchy ();

   /***** Set system data *****/
   Str_Copy (Gbl.Hierarchy.Node[Hie_SYS].ShrtName,Cfg_PLATFORM_SHORT_NAME,
	     sizeof (Gbl.Hierarchy.Node[Hie_SYS].ShrtName) - 1);
   Str_Copy (Gbl.Hierarchy.Node[Hie_SYS].FullName,Cfg_PLATFORM_FULL_NAME ,
	     sizeof (Gbl.Hierarchy.Node[Hie_SYS].FullName) - 1);
   Str_Copy (Gbl.Hierarchy.Node[Hie_SYS].WWW     ,Cfg_URL_SWAD_PUBLIC    ,
	     sizeof (Gbl.Hierarchy.Node[Hie_SYS].WWW     ) - 1);

   /***** Set current hierarchy level *****/
   Gbl.Hierarchy.Level = (Gbl.Hierarchy.Node[Hie_CRS].HieCod > 0) ? Hie_CRS :	// Course selected
			 (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0) ? Hie_DEG :	// Degree selected
			 (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0) ? Hie_CTR :	// Center selected
			 (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0) ? Hie_INS :	// Institution selected
			 (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0) ? Hie_CTY :	// Country selected
								    Hie_SYS;	// System/nothing selected

   /***** Initialize paths *****/
   if (Gbl.Hierarchy.Level == Hie_CRS)	// Course selected
     {
      /***** Paths of course directories *****/
      snprintf (Gbl.Crs.PathPriv   ,sizeof (Gbl.Crs.PathPriv   ),"%s/%ld",
	        Cfg_PATH_CRS_PRIVATE,Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      snprintf (Gbl.Crs.PathRelPubl,sizeof (Gbl.Crs.PathRelPubl),"%s/%ld",
	        Cfg_PATH_CRS_PUBLIC ,Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      snprintf (Gbl.Crs.PathURLPubl,sizeof (Gbl.Crs.PathURLPubl),"%s/%ld",
	        Cfg_URL_CRS_PUBLIC  ,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

      /***** If any of the course directories does not exist, create it *****/
      if (!Fil_CheckIfPathExists (Gbl.Crs.PathPriv))
	 Fil_CreateDirIfNotExists (Gbl.Crs.PathPriv);
      if (!Fil_CheckIfPathExists (Gbl.Crs.PathRelPubl))
	 Fil_CreateDirIfNotExists (Gbl.Crs.PathRelPubl);

      /***** Count number of groups in current course
             (used in some actions) *****/
      Gbl.Crs.Grps.NumGrps = Grp_DB_CountNumGrpsInCurrentCrs ();
     }
  }

/*****************************************************************************/
/******* Reset current country, institution, center, degree and course *******/
/*****************************************************************************/

void Hie_ResetHierarchy (void)
  {
   Hie_Level_t Level;

   Gbl.Hierarchy.Level = Hie_UNK;

   for (Level  = (Hie_Level_t) 0;
	Level <= (Hie_Level_t) Hie_NUM_LEVELS - 1;
	Level++)
     {
      Gbl.Hierarchy.List[Level].Num = 0;
      Gbl.Hierarchy.List[Level].Lst = NULL;
      Gbl.Hierarchy.List[Level].SelectedOrder = Hie_ORDER_DEFAULT;
      Gbl.Hierarchy.Node[Level].HieCod = -1L;
      Gbl.Hierarchy.Node[Level].PrtCod = -1L;
      Gbl.Hierarchy.Node[Level].ShrtName[0] =
      Gbl.Hierarchy.Node[Level].FullName[0] = '\0';
      Gbl.Hierarchy.Node[Level].WWW[0]      = '\0';
     }
   Gbl.Hierarchy.Node[Hie_CTR].Specific.PlcCod = -1L;
   Gbl.Hierarchy.Node[Hie_DEG].Specific.TypCod = -1L;
   Gbl.Hierarchy.Node[Hie_CRS].Specific.Year   = 0;
  }

/*****************************************************************************/
/***** Write institutions, centers and degrees administrated by an admin *****/
/*****************************************************************************/

void Hie_GetAndWriteInsCtrDegAdminBy (long UsrCod,unsigned ColSpan)
  {
   extern const char *Txt_all_degrees;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRow;
   unsigned NumRows;
   struct Hie_Node Hie[Hie_NUM_LEVELS];

   /***** Get institutions, centers, degrees admin by user from database *****/
   NumRows = Hie_DB_GetInsCtrDegAdminBy (&mysql_res,UsrCod);

   /***** Get the list of degrees *****/
   for (NumRow  = 1;
	NumRow <= NumRows;
	NumRow++)
     {
      HTM_TR_Begin (NULL);

	 /***** Indent *****/
	 HTM_TD_Begin ("class=\"RT %s\"",The_GetColorRows ());
	    Ico_PutIcon (NumRow == NumRows ? "subend20x20.gif" :
					     "submid20x20.gif",Ico_BLACK,
			 "","ICO25x25");
	 HTM_TD_End ();

	 /***** Write institution, center, degree *****/
	 HTM_TD_Begin ("colspan=\"%u\" class=\"LT DAT_SMALL_NOBR_%s %s\"",
		       ColSpan - 1,
		       The_GetSuffix (),
		       The_GetColorRows ());

	    /* Get next institution, center, degree */
	    row = mysql_fetch_row (mysql_res);

	    /* Get scope */
	    switch (Sco_GetScopeFromUnsignedStr (row[0]))
	      {
	       case Hie_SYS:	// System
		  Ico_PutIcon ("swad64x64.png",Ico_UNCHANGED,
		               Txt_all_degrees,"ICO16x16");
		  HTM_TxtF ("&nbsp;%s",Txt_all_degrees);
		  break;
	       case Hie_INS:	// Institution
		  if ((Hie[Hie_INS].HieCod = Str_ConvertStrCodToLongCod (row[1])) > 0)
		    {
		     /* Get data of institution */
		     Ins_GetInstitDataByCod (&Hie[Hie_INS]);

		     /* Write institution logo and name */
		     Ins_DrawInstitLogoAndNameWithLink (&Hie[Hie_INS],ActSeeInsInf,"LT");
		    }
		  break;
	       case Hie_CTR:	// Center
		  if ((Hie[Hie_CTR].HieCod = Str_ConvertStrCodToLongCod (row[1])) > 0)
		    {
		     /* Get data of center */
		     Ctr_GetCenterDataByCod (&Hie[Hie_CTR]);

		     /* Write center logo and name */
		     Ctr_DrawCenterLogoAndNameWithLink (&Hie[Hie_CTR],ActSeeCtrInf,"LT");
		    }
		  break;
	       case Hie_DEG:	// Degree
		  if ((Hie[Hie_DEG].HieCod = Str_ConvertStrCodToLongCod (row[1])) > 0)
		    {
		     /* Get data of degree */
		     Deg_GetDegreeDataByCod (&Hie[Hie_DEG]);

		     /* Write degree logo and name */
		     Deg_DrawDegreeLogoAndNameWithLink (&Hie[Hie_DEG],ActSeeDegInf,"LT");
		    }
		  break;
	       default:	// There are no administrators in other scopes
		  Err_WrongHierarchyLevelExit ();
		  break;
	      }
	 HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Check if I can edit hierarchy elements *******************/
/*****************************************************************************/

bool Hie_CheckIfICanEdit (void)
  {
   // Some admins can edit all hierarchy elements.
   // Any user can edit the elements he/she has created...
   // ...as long as they are in pending status.
   static const bool ICanEdit[Rol_NUM_ROLES] =
     {
      /* Users who can edit */
      [Rol_GST    ] = true,
      [Rol_USR    ] = true,
      [Rol_STD    ] = true,
      [Rol_NET    ] = true,
      [Rol_TCH    ] = true,
      [Rol_DEG_ADM] = true,
      [Rol_CTR_ADM] = true,
      [Rol_INS_ADM] = true,
      [Rol_SYS_ADM] = true,
     };

   return ICanEdit[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/*********************** Write status cell in table **************************/
/*****************************************************************************/

void Hie_WriteStatusCell (Hie_Status_t Status,
			  const char *Class,const char *BgColor,
			  const char *Txt[Hie_NUM_STATUS_TXT])
  {
   Hie_StatusTxt_t StatusTxt = Hie_GetStatusTxtFromStatusBits (Status);

   HTM_TD_Begin ("class=\"LM %s_%s %s\"",
                 Class,The_GetSuffix (),BgColor);
      if (StatusTxt != Hie_STATUS_ACTIVE) // If active ==> do not show anything
	 HTM_Txt (Txt[StatusTxt]);
   HTM_TD_End ();
  }

void Hie_WriteStatusCellEditable (bool ICanEdit,Hie_Status_t Status,
                                  Act_Action_t NextAction,long HieCod,
                                  const char *Txt[Hie_NUM_STATUS_TXT])
  {
   Hie_StatusTxt_t StatusTxt = Hie_GetStatusTxtFromStatusBits (Status);
   unsigned StatusUnsigned;

   /***** Begin cell *****/
   HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
      if (ICanEdit && StatusTxt == Hie_STATUS_PENDING)
	{
	 /* Begin form */
	 Frm_BeginForm (NextAction);
	    ParCod_PutPar (ParCod_OthHie,HieCod);

	    /* Selector */
	    HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
			      "name=\"Status\" class=\"INPUT_STATUS\"");

	       StatusUnsigned = (unsigned) Hie_GetStatusBitsFromStatusTxt (Hie_STATUS_PENDING);
	       HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,
	                   HTM_OPTION_SELECTED,
	                   HTM_OPTION_ENABLED,
			   "%s",Txt[Hie_STATUS_PENDING]);

	       StatusUnsigned = (unsigned) Hie_GetStatusBitsFromStatusTxt (Hie_STATUS_ACTIVE);
	       HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,
	                   HTM_OPTION_UNSELECTED,
	                   HTM_OPTION_ENABLED,
			   "%s",Txt[Hie_STATUS_ACTIVE]);

	    HTM_SELECT_End ();

	 /* End form */
	 Frm_EndForm ();
	}
      else if (StatusTxt != Hie_STATUS_ACTIVE)	// If active ==> do not show anything
	 HTM_Txt (Txt[StatusTxt]);

   /***** End cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/**************************** Get parameter status ***************************/
/*****************************************************************************/

Hie_Status_t Hie_GetParStatus (void)
  {
   Hie_Status_t Status;
   Hie_StatusTxt_t StatusTxt;

   /***** Get parameter with status *****/
   Status = (Hie_Status_t)
	    Par_GetParUnsignedLong ("Status",
	                            0,
	                            (unsigned long) Hie_MAX_STATUS,
                                    (unsigned long) Hie_WRONG_STATUS);

   if (Status == Hie_WRONG_STATUS)
      Err_WrongStatusExit ();

   StatusTxt = Hie_GetStatusTxtFromStatusBits (Status);
   Status = Hie_GetStatusBitsFromStatusTxt (StatusTxt);	// New status

   return Status;
  }

/*****************************************************************************/
/******************* Set StatusTxt depending on status bits ******************/
/*****************************************************************************/
// Hie_STATUS_UNKNOWN = 0	// Other
// Hie_STATUS_ACTIVE  = 1	// 00 (Status == 0)
// Hie_STATUS_PENDING = 2	// 01 (Status == Hie_STATUS_BIT_PENDING)
// Hie_STATUS_REMOVED = 3	// 1- (Status & Hie_STATUS_BIT_REMOVED)

static Hie_StatusTxt_t Hie_GetStatusTxtFromStatusBits (Hie_Status_t Status)
  {
   if (Status == 0)
      return Hie_STATUS_ACTIVE;
   if (Status == Hie_STATUS_BIT_PENDING)
      return Hie_STATUS_PENDING;
   if (Status & Hie_STATUS_BIT_REMOVED)
      return Hie_STATUS_REMOVED;
   return Hie_STATUS_UNKNOWN;
  }

/*****************************************************************************/
/******************* Set status bits depending on StatusTxt ******************/
/*****************************************************************************/
// Hie_STATUS_UNKNOWN = 0	// Other
// Hie_STATUS_ACTIVE  = 1	// 00 (Status == 0)
// Hie_STATUS_PENDING = 2	// 01 (Status == Hie_STATUS_BIT_PENDING)
// Hie_STATUS_REMOVED = 3	// 1- (Status & Hie_STATUS_BIT_REMOVED)

static Hie_Status_t Hie_GetStatusBitsFromStatusTxt (Hie_StatusTxt_t StatusTxt)
  {
   static const Hie_Status_t StatusBits[Hie_NUM_STATUS_TXT] =
     {
      [Hie_STATUS_UNKNOWN] = (Hie_Status_t) 0,
      [Hie_STATUS_ACTIVE ] = (Hie_Status_t) 0,
      [Hie_STATUS_PENDING] = Hie_STATUS_BIT_PENDING,
      [Hie_STATUS_REMOVED] = Hie_STATUS_BIT_REMOVED,
     };

   return StatusBits[StatusTxt];
  }

/*****************************************************************************/
/**** Write parameter with code of other institution/center/degree/course ****/
/*****************************************************************************/

void Hie_PutParOtherHieCod (void *HieCod)
  {
   if (HieCod)
      ParCod_PutPar (ParCod_OthHie,*((long *) HieCod));
  }

/*****************************************************************************/
/****** Get parameter with the type or order in list of hierarchy nodes ******/
/*****************************************************************************/

Hie_Order_t Hie_GetParHieOrder (void)
  {
   return (Hie_Order_t) Par_GetParUnsignedLong ("Order",
						0,
						Hie_NUM_ORDERS - 1,
						(unsigned long) Hie_ORDER_DEFAULT);
  }

/*****************************************************************************/
/**************************** Free list of centers ***************************/
/*****************************************************************************/

void Hie_FreeList (Hie_Level_t Level)
  {
   if (Gbl.Hierarchy.List[Level].Lst)
     {
      /***** Free memory used by the list of child nodes *****/
      free (Gbl.Hierarchy.List[Level].Lst);
      Gbl.Hierarchy.List[Level].Lst = NULL;
      Gbl.Hierarchy.List[Level].Num = 0;
     }
  }

/*****************************************************************************/
/*** Flush cache that stores if a user belongs to a node of the hierarchy ****/
/*****************************************************************************/

void Hie_FlushCacheUsrBelongsTo (Hie_Level_t Level)
  {
   Gbl.Cache.UsrBelongsTo[Level].Valid = false;
  }

/*****************************************************************************/
/******************** Check if a user belongs to a course ********************/
/*****************************************************************************/

bool Hie_CheckIfUsrBelongsTo (Hie_Level_t Level,long UsrCod,long HieCod,
                              bool CountOnlyAcceptedCourses)
  {
   static bool (*FunctionToGetIfUsrBelongsToFromDB[Hie_NUM_LEVELS]) (long UsrCod,long HieCod,
								        bool CountOnlyAcceptedCourses) =
     {
      [Hie_INS] = Ins_DB_CheckIfUsrBelongsToIns,
      [Hie_CTR] = Ctr_DB_CheckIfUsrBelongsToCtr,
      [Hie_DEG] = Deg_DB_CheckIfUsrBelongsToDeg,
      [Hie_CRS] = Enr_DB_CheckIfUsrBelongsToCrs,
     };

   /***** Check if level is correct *****/
   if (!FunctionToGetIfUsrBelongsToFromDB[Level])
      Err_WrongHierarchyLevelExit ();

   /***** 1. Fast check: Trivial cases *****/
   if (UsrCod <= 0 ||
       HieCod <= 0)
      return false;

   /***** 2. Fast check: If cached... *****/
   if (Gbl.Cache.UsrBelongsTo[Level].Valid &&
       UsrCod == Gbl.Cache.UsrBelongsTo[Level].UsrCod &&
       HieCod == Gbl.Cache.UsrBelongsTo[Level].HieCod &&
       CountOnlyAcceptedCourses == Gbl.Cache.UsrBelongsTo[Level].CountOnlyAcceptedCourses)
      return Gbl.Cache.UsrBelongsTo[Level].Belongs;

   /***** 3. Slow check: Get if user belongs to hierarchy node from database *****/
   Gbl.Cache.UsrBelongsTo[Level].UsrCod = UsrCod;
   Gbl.Cache.UsrBelongsTo[Level].HieCod = HieCod;
   Gbl.Cache.UsrBelongsTo[Level].CountOnlyAcceptedCourses = CountOnlyAcceptedCourses;
   Gbl.Cache.UsrBelongsTo[Level].Belongs = FunctionToGetIfUsrBelongsToFromDB[Level] (UsrCod,HieCod,
										     CountOnlyAcceptedCourses);
   Gbl.Cache.UsrBelongsTo[Level].Valid = true;
   return Gbl.Cache.UsrBelongsTo[Level].Belongs;
  }

/*****************************************************************************/
/*********            Get and show stats about hierarchy           ***********/
/********* (countries, institutions, centers, degrees and courses) ***********/
/*****************************************************************************/

void Hie_GetAndShowHierarchyStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_hierarchy;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   Hie_Level_t HavingNodesOfLevel;
   Rol_Role_t Role;

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_FIGURE_TYPES[Fig_HIERARCHY],
                      NULL,NULL,
                      Hlp_ANALYTICS_Figures_hierarchy,Box_NOT_CLOSABLE,2);

      /* Head row */
      Hie_WriteHeadHierarchy ();

      /* Rows with number of nodes having nodes of each level */
      for (HavingNodesOfLevel  = Hie_INS;
	   HavingNodesOfLevel <= Hie_CRS;
	   HavingNodesOfLevel++)
         Hie_GetAndShowHierarchyWithNodes (HavingNodesOfLevel);

      /* Rows with number of nodes having users of each role */
      for (Role  = Rol_TCH;
	   Role >= Rol_STD;
	   Role--)
	 Hie_GetAndShowHierarchyWithUsrs (Role);

      /* Row with total nodes */
      Hie_GetAndShowHierarchyTotal ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/************************ Write head of hierarchy table **********************/
/*****************************************************************************/

static void Hie_WriteHeadHierarchy (void)
  {
   extern const char *Txt_Countries;
   extern const char *Txt_Institutions;
   extern const char *Txt_Centers;
   extern const char *Txt_Degrees;
   extern const char *Txt_Courses;

   HTM_TR_Begin (NULL);

      HTM_TH_Empty (1);

      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 Ico_PutIcon ("globe-americas.svg",Ico_BLACK,
	              Txt_Countries,"ICOx16");
	 HTM_BR ();
	 HTM_Txt (Txt_Countries);
      HTM_TH_End ();

      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 Ico_PutIcon ("university.svg",Ico_BLACK,
	              Txt_Institutions,"ICOx16");
	 HTM_BR ();
	 HTM_Txt (Txt_Institutions);
      HTM_TH_End ();

      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 Ico_PutIcon ("building.svg",Ico_BLACK,
	              Txt_Centers,"ICOx16");
	 HTM_BR ();
	 HTM_Txt (Txt_Centers);
      HTM_TH_End ();

      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 Ico_PutIcon ("graduation-cap.svg",Ico_BLACK,
	              Txt_Degrees,"ICOx16");
	 HTM_BR ();
	 HTM_Txt (Txt_Degrees);
      HTM_TH_End ();

      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 Ico_PutIcon ("chalkboard-teacher.svg",Ico_BLACK,
	              Txt_Courses,"ICOx16");
	 HTM_BR ();
	 HTM_Txt (Txt_Courses);
      HTM_TH_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/********* Get and show number of elements in hierarchy with nodes ***********/
/*****************************************************************************/

static void Hie_GetAndShowHierarchyWithNodes (Hie_Level_t HavingNodesOfLevel)
  {
   extern const char *Txt_With_;
   extern const char *Txt_institutions;
   extern const char *Txt_centers;
   extern const char *Txt_degrees;
   extern const char *Txt_courses;
   static const char **Txt[Hie_NUM_LEVELS] =
     {
      [Hie_INS] = &Txt_institutions,	// Number of ... with institutions
      [Hie_CTR] = &Txt_centers,		// Number of ... with centers
      [Hie_DEG] = &Txt_degrees,		// Number of ... with degrees
      [Hie_CRS] = &Txt_courses,		// Number of ... with courses
     };
   int NumNodes[Hie_NUM_LEVELS];
   Hie_Level_t LevelChildren;

   /***** Get number of elements with courses *****/
   // For each level country, institution, center, degree and course...
   for (LevelChildren  = Hie_CTY;
        LevelChildren <= Hie_CRS;
        LevelChildren++)
      if (LevelChildren >= HavingNodesOfLevel)			// Example: don't show number of centers with institutions
	 NumNodes[LevelChildren] = -1;
      else if (LevelChildren      <  Gbl.Scope.Current &&	// Example: if scope is center (4)
	       HavingNodesOfLevel <= Gbl.Scope.Current)		//          number of nodes with instit./countries/centers
	 NumNodes[LevelChildren] = 1;				//          in current center is 1
      else
         NumNodes[LevelChildren] = (int) Hie_GetCachedNumNodesInHieLvlWith (LevelChildren,	// Child
        							            Gbl.Scope.Current,	// Parent
        							            HavingNodesOfLevel);// Grand child

   /***** Write number of elements with courses *****/
   Hie_ShowHierarchyRow (Txt_With_,*Txt[HavingNodesOfLevel],"DAT",NumNodes);
  }

/*****************************************************************************/
/********** Get and show number of elements in hierarchy with users **********/
/*****************************************************************************/

static void Hie_GetAndShowHierarchyWithUsrs (Rol_Role_t Role)
  {
   extern const char *Txt_With_;
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   int NumNodes[Hie_NUM_LEVELS];

   /***** Get number of elements with students *****/
   NumNodes[Hie_CTY] = Cty_GetCachedNumCtysWithUsrs (Role);
   NumNodes[Hie_INS] = Ins_GetCachedNumInssWithUsrs (Role);
   NumNodes[Hie_CTR] = Ctr_GetCachedNumCtrsWithUsrs (Role);
   NumNodes[Hie_DEG] = Deg_GetCachedNumDegsWithUsrs (Role);
   NumNodes[Hie_CRS] = Crs_GetCachedNumCrssWithUsrs (Role);

   /***** Write number of elements with users *****/
   Hie_ShowHierarchyRow (Txt_With_,Txt_ROLES_PLURAL_abc[Role][Usr_SEX_UNKNOWN],
			 "DAT",NumNodes);
  }

/*****************************************************************************/
/************ Get and show total number of elements in hierarchy *************/
/*****************************************************************************/

static void Hie_GetAndShowHierarchyTotal (void)
  {
   extern const char *Txt_Total;
   Hie_Level_t Level;
   int NumNodes[Hie_NUM_LEVELS];

   /***** Get total number of nodes of each level in current scope *****/
   for (Level  = Hie_CTY;
	Level <= Hie_CRS;
	Level++)
      if (Level > Gbl.Scope.Current)
	 NumNodes[Level] = (int) Hie_GetCachedNumNodesInHieLvl (Level,Gbl.Scope.Current,
							        Gbl.Hierarchy.Node[Gbl.Scope.Current].HieCod);
      else
	 NumNodes[Level] = 1;

   /***** Write total number of elements *****/
   Hie_ShowHierarchyRow ("",Txt_Total,"LINE_TOP DAT_STRONG",NumNodes);
  }

/*****************************************************************************/
/**** Get total number of courses/degrees/centers/institutions in country ****/
/*****************************************************************************/

void Hie_FlushCachedNumNodesInHieLvl (Hie_Level_t LevelChildren,
		      		      Hie_Level_t LevelParent)
  {
   Gbl.Cache.NumNodesInHieLvl[LevelChildren][LevelParent].Valid = false;
  }

unsigned Hie_GetCachedNumNodesInHieLvl (Hie_Level_t LevelChildren,
		      		        Hie_Level_t LevelParent,long HieCod)
  {
   unsigned NumNodes;

   /***** Get number of nodes from cache *****/
   if (!FigCch_GetFigureFromCache (Hie_FiguresCached[LevelChildren],LevelParent,HieCod,
				   FigCch_UNSIGNED,&NumNodes))
      /***** Get current number of nodes from database and update cache *****/
      NumNodes = Hie_GetNumNodesInHieLvl (LevelChildren,LevelParent,HieCod);

   return NumNodes;
  }

unsigned Hie_GetNumNodesInHieLvl (Hie_Level_t LevelChildren,
		      	          Hie_Level_t LevelParent,long HieCod)
  {
   static unsigned (*FunctionGetFigure[Hie_NUM_LEVELS][Hie_NUM_LEVELS]) (long HieCod) =
     {
      /* Number of nodes in system */
      [Hie_CTY][Hie_SYS] = Cty_DB_GetNumCtysInSys,
      [Hie_INS][Hie_SYS] = Ins_DB_GetNumInssInSys,
      [Hie_CTR][Hie_SYS] = Ctr_DB_GetNumCtrsInSys,
      [Hie_DEG][Hie_SYS] = Deg_DB_GetNumDegsInSys,
      [Hie_CRS][Hie_SYS] = Crs_DB_GetNumCrssInSys,
      /* Number of nodes in country */
      [Hie_INS][Hie_CTY] = Ins_DB_GetNumInssInCty,
      [Hie_CTR][Hie_CTY] = Ctr_DB_GetNumCtrsInCty,
      [Hie_DEG][Hie_CTY] = Deg_DB_GetNumDegsInCty,
      [Hie_CRS][Hie_CTY] = Crs_DB_GetNumCrssInCty,
      /* Number of nodes in institution */
      [Hie_CTR][Hie_INS] = Ctr_DB_GetNumCtrsInIns,
      [Hie_DEG][Hie_INS] = Deg_DB_GetNumDegsInIns,
      [Hie_CRS][Hie_INS] = Crs_DB_GetNumCrssInIns,
      /* Number of nodes in center */
      [Hie_DEG][Hie_CTR] = Deg_DB_GetNumDegsInCtr,
      [Hie_CRS][Hie_CTR] = Crs_DB_GetNumCrssInCtr,
      /* Number of nodes in degree */
      [Hie_CRS][Hie_DEG] = Crs_DB_GetNumCrssInDeg,
     };

   /***** 1. Fast check: If cached... *****/
   if (Gbl.Cache.NumNodesInHieLvl[LevelChildren][LevelParent].Valid &&
       HieCod == Gbl.Cache.NumNodesInHieLvl[LevelChildren][LevelParent].HieCod)
      return Gbl.Cache.NumNodesInHieLvl[LevelChildren][LevelParent].Num;

   /***** 2. Slow: number of institutions in a country from database *****/
   Gbl.Cache.NumNodesInHieLvl[LevelChildren][LevelParent].HieCod = HieCod;
   Gbl.Cache.NumNodesInHieLvl[LevelChildren][LevelParent].Num    = FunctionGetFigure[LevelChildren][LevelParent] (HieCod);
   Gbl.Cache.NumNodesInHieLvl[LevelChildren][LevelParent].Valid  = true;
   FigCch_UpdateFigureIntoCache (Hie_FiguresCached[LevelChildren],LevelParent,
				 Gbl.Cache.NumNodesInHieLvl[LevelChildren][LevelParent].HieCod,
				 FigCch_UNSIGNED,&Gbl.Cache.NumNodesInHieLvl[LevelChildren][LevelParent].Num);
   return Gbl.Cache.NumNodesInHieLvl[LevelChildren][LevelParent].Num;
  }

/*****************************************************************************/
/**** Get number of children nodes in parent node having nodes of a level ****/
/*****************************************************************************/

unsigned Hie_GetCachedNumNodesInHieLvlWith (Hie_Level_t LevelChildren,
					    Hie_Level_t LevelParent,
					    Hie_Level_t HavingNodesOfLevel)
  {
   // Example: number of centers with courses in current institution
   //          LevelChildren      = center
   //          LevelParent        = institution
   //	       HavingNodesOfLevel = course
   static FigCch_FigureCached_t Figure[Hie_NUM_LEVELS][Hie_NUM_LEVELS] =
     {
     // Child / Grandchild
      [Hie_CTY][Hie_INS] = FigCch_NUM_CTYS_WITH_INSS,
      [Hie_CTY][Hie_CTR] = FigCch_NUM_CTYS_WITH_CTRS,
      [Hie_CTY][Hie_DEG] = FigCch_NUM_CTYS_WITH_DEGS,
      [Hie_CTY][Hie_CRS] = FigCch_NUM_CTYS_WITH_CRSS,

      [Hie_INS][Hie_CTR] = FigCch_NUM_INSS_WITH_CTRS,
      [Hie_INS][Hie_DEG] = FigCch_NUM_INSS_WITH_DEGS,
      [Hie_INS][Hie_CRS] = FigCch_NUM_INSS_WITH_CRSS,

      [Hie_CTR][Hie_DEG] = FigCch_NUM_CTRS_WITH_DEGS,
      [Hie_CTR][Hie_CRS] = FigCch_NUM_CTRS_WITH_CRSS,

      [Hie_DEG][Hie_CRS] = FigCch_NUM_DEGS_WITH_CRSS,
     };
   static unsigned (*FunctionGetFigure[Hie_NUM_LEVELS][Hie_NUM_LEVELS]) (Hie_Level_t Level,long HieCod) =
     {
     // Child / Grandchild
      [Hie_CTY][Hie_INS] = Cty_DB_GetNumCtysWithInss,
      [Hie_CTY][Hie_CTR] = Cty_DB_GetNumCtysWithCtrs,
      [Hie_CTY][Hie_DEG] = Cty_DB_GetNumCtysWithDegs,
      [Hie_CTY][Hie_CRS] = Cty_DB_GetNumCtysWithCrss,

      [Hie_INS][Hie_CTR] = Ins_DB_GetNumInssWithCtrs,
      [Hie_INS][Hie_DEG] = Ins_DB_GetNumInssWithDegs,
      [Hie_INS][Hie_CRS] = Ins_DB_GetNumInssWithCrss,

      [Hie_CTR][Hie_DEG] = Ctr_DB_GetNumCtrsWithDegs,
      [Hie_CTR][Hie_CRS] = Ctr_DB_GetNumCtrsWithCrss,

      [Hie_DEG][Hie_CRS] = Deg_DB_GetNumDegsWithCrss,
     };
   unsigned NumNodes;

   /***** Get number of centers with degrees from cache *****/
   if (!FigCch_GetFigureFromCache (Figure[LevelChildren][HavingNodesOfLevel],
				   LevelParent,Gbl.Hierarchy.Node[LevelParent].HieCod,
				   FigCch_UNSIGNED,&NumNodes))
     {
      /***** Get current number of nodes with degrees from database and update cache *****/
      NumNodes = FunctionGetFigure[LevelChildren][HavingNodesOfLevel] (LevelParent,Gbl.Hierarchy.Node[LevelParent].HieCod);
      FigCch_UpdateFigureIntoCache (Figure[LevelChildren][HavingNodesOfLevel],
				    LevelParent,Gbl.Hierarchy.Node[LevelParent].HieCod,
				    FigCch_UNSIGNED,&NumNodes);
     }

   return NumNodes;
  }

/*****************************************************************************/
/******** Get number of users who claim to belong to a hierarchy node ********/
/*****************************************************************************/

void Hie_FlushCacheNumUsrsWhoClaimToBelongTo (Hie_Level_t Level)
  {
   Gbl.Cache.NumUsrsWhoClaimToBelongTo[Level].Valid = false;
  }

unsigned Hie_GetCachedNumUsrsWhoClaimToBelongTo (Hie_Level_t Level,
						 struct Hie_Node *Node)
  {
   static FigCch_FigureCached_t Figure[Hie_NUM_LEVELS] =
     {
      [Hie_CTY] = FigCch_NUM_USRS_BELONG_CTY,
      [Hie_INS] = FigCch_NUM_USRS_BELONG_INS,
      [Hie_CTR] = FigCch_NUM_USRS_BELONG_CTR,
     };
   unsigned NumUsrs;

   /***** Get number of users who claim to belong to hierarchy node from cache *****/
   if (!FigCch_GetFigureFromCache (Figure[Level],Level,Node->HieCod,
                                   FigCch_UNSIGNED,&NumUsrs))
      /***** Get current number of users who claim to belong to hierarchy node
             from database and update cache *****/
      NumUsrs = Hie_GetNumUsrsWhoClaimToBelongTo (Level,Node);

   return NumUsrs;
  }

unsigned Hie_GetNumUsrsWhoClaimToBelongTo (Hie_Level_t Level,
					   struct Hie_Node *Node)
  {
   static FigCch_FigureCached_t Figure[Hie_NUM_LEVELS] =
     {
      [Hie_CTY] = FigCch_NUM_USRS_BELONG_CTY,
      [Hie_INS] = FigCch_NUM_USRS_BELONG_INS,
      [Hie_CTR] = FigCch_NUM_USRS_BELONG_CTR,
     };
   static unsigned (*FunctionToGetNumUsrsWhoClaimToBelongToFromDB[Hie_NUM_LEVELS]) (long HieCod) =
     {
      [Hie_CTY] = Cty_DB_GetNumUsrsWhoClaimToBelongToCty,
      [Hie_INS] = Ins_DB_GetNumUsrsWhoClaimToBelongToIns,
      [Hie_CTR] = Ctr_DB_GetNumUsrsWhoClaimToBelongToCtr,
     };

   /***** 1. Fast check: Trivial case *****/
   if (Node->HieCod <= 0)
      return 0;

   /***** 2. Fast check: If already got... *****/
   if (Node->NumUsrsWhoClaimToBelong.Valid)
      return Node->NumUsrsWhoClaimToBelong.NumUsrs;

   /***** 3. Fast check: If cached... *****/
   if (Gbl.Cache.NumUsrsWhoClaimToBelongTo[Level].Valid &&
       Node->HieCod == Gbl.Cache.NumUsrsWhoClaimToBelongTo[Level].HieCod)
     {
      Node->NumUsrsWhoClaimToBelong.NumUsrs = Gbl.Cache.NumUsrsWhoClaimToBelongTo[Level].NumUsrs;
      Node->NumUsrsWhoClaimToBelong.Valid = true;
      return Node->NumUsrsWhoClaimToBelong.NumUsrs;
     }

   /***** 4. Slow: number of users who claim to belong to a hierarchy node
                   from database *****/
   Gbl.Cache.NumUsrsWhoClaimToBelongTo[Level].HieCod  = Node->HieCod;
   Gbl.Cache.NumUsrsWhoClaimToBelongTo[Level].NumUsrs =
   Node->NumUsrsWhoClaimToBelong.NumUsrs = FunctionToGetNumUsrsWhoClaimToBelongToFromDB[Level] (Node->HieCod);
   Gbl.Cache.NumUsrsWhoClaimToBelongTo[Level].Valid =
   Node->NumUsrsWhoClaimToBelong.Valid = true;
   FigCch_UpdateFigureIntoCache (Figure[Level],Level,Gbl.Cache.NumUsrsWhoClaimToBelongTo[Level].HieCod,
				 FigCch_UNSIGNED,&Gbl.Cache.NumUsrsWhoClaimToBelongTo[Level].NumUsrs);
   return Node->NumUsrsWhoClaimToBelong.NumUsrs;
  }

/*****************************************************************************/
/************** Show row with number of elements in hierarchy ****************/
/*****************************************************************************/

static void Hie_ShowHierarchyRow (const char *Text1,const char *Text2,
				  const char *ClassTxt,
				  int NumNodes[Hie_NUM_LEVELS])
  {
   Hie_Level_t Level;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Write text *****/
      HTM_TD_Begin ("class=\"RM %s_%s\"",ClassTxt,The_GetSuffix ());
	 HTM_Txt (Text1);
	 HTM_Txt (Text2);
      HTM_TD_End ();

      /***** Write number of countries *****/
      for (Level = Hie_CTY;
	   Level <= Hie_CRS;
	   Level++)
         Hie_ShowHierarchyCell (ClassTxt,NumNodes[Level]);

   /***** End row *****/
   HTM_TR_End ();
  }

static void Hie_ShowHierarchyCell (const char *ClassTxt,int Num)
  {
   /***** Write number *****/
   HTM_TD_Begin ("class=\"RM %s_%s\"",ClassTxt,The_GetSuffix ());
      if (Num >= 0)
	 HTM_Unsigned ((unsigned) Num);
      else		// < 0 ==> do not show number
	 HTM_Hyphen ();
   HTM_TD_End ();
  }
