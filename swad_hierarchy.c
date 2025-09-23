// swad_hierarchy.c: hierarchy (system, institution, center, degree, course)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include <stdlib.h>		// For free
#include <string.h>		// For string functions

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
#include "swad_help.h"
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
/****************************** Public constants *****************************/
/*****************************************************************************/

ParCod_Param_t Hie_ParCod[Hie_NUM_LEVELS] =
  {
   [Hie_CTY] = ParCod_Cty,
   [Hie_INS] = ParCod_Ins,
   [Hie_CTR] = ParCod_Ctr,
   [Hie_DEG] = ParCod_Deg,
   [Hie_CRS] = ParCod_Crs,
  };

const char *Hie_Icons[Hie_NUM_LEVELS] =
  {
   [Hie_SYS] = "sitemap.svg",
   [Hie_CTY] = "globe-americas.svg",
   [Hie_INS] = "university.svg",
   [Hie_CTR] = "building.svg",
   [Hie_DEG] = "graduation-cap.svg",
   [Hie_CRS] = "chalkboard-user.svg",
  };

unsigned (*Hie_GetMyNodesFromDB[Hie_NUM_LEVELS]) (MYSQL_RES **mysql_res,
						  long PrtCod) =
  {
   [Hie_CTY] = Cty_DB_GetMyCtys,
   [Hie_INS] = Ins_DB_GetMyInss,
   [Hie_CTR] = Ctr_DB_GetMyCtrs,
   [Hie_DEG] = Deg_DB_GetMyDegs,
   [Hie_CRS] = Enr_DB_GetMyCrss,
  };

Err_SuccessOrError_t (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node) =
  {
   [Hie_CTY] = Cty_GetCountrDataByCod,
   [Hie_INS] = Ins_GetInstitDataByCod,
   [Hie_CTR] = Ctr_GetCenterDataByCod,
   [Hie_DEG] = Deg_GetDegreeDataByCod,
   [Hie_CRS] = Crs_GetCourseDataByCod,
  };

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
static void Hie_GetAndShowHierarchyWithNodes (Hie_Level_t HieLvl,
					      Hie_Level_t HavingNodesOfLevel);
static void Hie_GetAndShowHierarchyWithUsrs (Hie_Level_t HieLvl,Rol_Role_t Role);
static void Hie_GetAndShowHierarchyTotal (Hie_Level_t HieLvl);
static void Hie_ShowHierarchyRow (const char *Text1,const char *Text2,
				  const char *ClassTxt,
				  int NumNodes[Hie_NUM_LEVELS]);
static void Hie_ShowHierarchyCell (const char *ClassTxt,int Num);

//--------------------------- My hierarchy ------------------------------------
static void Hie_WriteListMyHierarchyToSelectNode (void);
static void Hie_PutIconToSearchCourses (__attribute__((unused)) void *Args);
static void Hie_WriteMyHierarchyNodes (struct Hie_Node Hie[Hie_NUM_LEVELS],
			               Lay_Last_t IsLastItemInLevel[1 + 6],
			               Hie_Level_t HieLvl);
static void Hie_WriteRowMyHierarchy (Hie_Level_t HieLvl,
				     const struct Hie_Node Hie[Hie_NUM_LEVELS],
				     Lay_Highlight_t Highlight,
				     Lay_Last_t IsLastItemInLevel[1 + 6]);

/*****************************************************************************/
/***************** Get hierarchy level from database string ******************/
/*****************************************************************************/

Hie_Level_t Hie_GetLevelFromDBStr (const char *HieLvlDBStr)
  {
   Hie_Level_t HieLvl;

   for (HieLvl  = (Hie_Level_t) 0;
	HieLvl <= (Hie_Level_t) (Hie_NUM_LEVELS - 1);
	HieLvl++)
      if (!strcmp (Hie_GetDBStrFromLevel (HieLvl),HieLvlDBStr))
	 return HieLvl;

   return Hie_UNK;
  }

/*****************************************************************************/
/****************** Get database string from hierarchy level *****************/
/*****************************************************************************/

const char *Hie_GetDBStrFromLevel (Hie_Level_t HieLvl)
  {
   static const char *Sco_ScopeDB[Hie_NUM_LEVELS] =
     {
      [Hie_UNK] = "Unk",
      [Hie_SYS] = "Sys",
      [Hie_CTY] = "Cty",
      [Hie_INS] = "Ins",
      [Hie_CTR] = "Ctr",
      [Hie_DEG] = "Deg",
      [Hie_CRS] = "Crs",
     };

   if (HieLvl >= Hie_NUM_LEVELS)
      HieLvl = Hie_UNK;

   return Sco_ScopeDB[HieLvl];
  }

/*****************************************************************************/
/************************** Get current hierarchy code ***********************/
/*****************************************************************************/

long Hie_GetHieCod (Hie_Level_t HieLvl)
  {
   switch (HieLvl)
     {
      case Hie_SYS:
      case Hie_CTY:
      case Hie_INS:
      case Hie_CTR:
      case Hie_DEG:
      case Hie_CRS:
	 return Gbl.Hierarchy.Node[HieLvl].HieCod;
      default:
	 Err_WrongHierarchyLevelExit ();
	 return -1L;	// Not reached
     }
  }

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
   extern const char *Par_CodeStr[Par_NUM_PAR_COD];
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   static void (*FunctionWriteSelector[Hie_NUM_LEVELS]) (void) =
     {
      [Hie_CTY] = Cty_WriteSelectorOfCountry,
      [Hie_INS] = Ins_WriteSelectorOfInstitution,
      [Hie_CTR] = Ctr_WriteSelectorOfCenter,
      [Hie_DEG] = Deg_WriteSelectorOfDegree,
      [Hie_CRS] = Crs_WriteSelectorOfCourse,
     };
   Hie_Level_t HieLvl;
   Hie_Level_t LastHieLvl = Gbl.Hierarchy.HieLvl + 1;

   /***** Begin table *****/
   HTM_TABLE_BeginCenterPadding (2);

      for (HieLvl  = Hie_CTY;
	   HieLvl <= LastHieLvl;
	   HieLvl++)
        {
	 /***** Write selector of nodes *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT",Par_CodeStr[Hie_ParCod[HieLvl]],
			     Txt_HIERARCHY_SINGUL_Abc[HieLvl]);

	    /* Data */
	    HTM_TD_Begin ("class=\"LT\"");
	       FunctionWriteSelector[HieLvl] ();
	    HTM_TD_End ();

	 HTM_TR_End ();
        }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************* Write hierarchy breadcrumb in the top of the page *************/
/*****************************************************************************/

void Hie_WriteHierarchyInBreadcrumb (void)
  {
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];

   /***** Form to go to the system *****/
   HTM_DIV_Begin ("class=\"BC BC_%s\"",The_GetSuffix ());

      HTM_NBSP ();

      Frm_BeginFormGoTo (ActMnu);
	 Par_PutParUnsigned (NULL,"NxtTab",(unsigned) TabSys);
	 HTM_BUTTON_Submit_Begin (Txt_HIERARCHY_SINGUL_Abc[Hie_SYS],NULL,
				  "class=\"BT_LINK\"");
	    HTM_Txt (Txt_HIERARCHY_SINGUL_Abc[Hie_SYS]);
	 HTM_BUTTON_End ();
      Frm_EndForm ();

   HTM_DIV_End ();

   if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)		// Country selected...
     {
      HTM_DIV_Begin ("class=\"BC BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_NBSP ();
	 HTM_GT ();
	 HTM_NBSP ();

	 /***** Form to go to see institutions of this country *****/
	 Frm_BeginFormGoTo (ActSeeIns);
	    ParCod_PutPar (ParCod_Cty,Gbl.Hierarchy.Node[Hie_CTY].HieCod);
	    HTM_BUTTON_Submit_Begin (Gbl.Hierarchy.Node[Hie_CTY].FullName,NULL,
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
	 HTM_NBSP ();
	 HTM_GT ();
	 HTM_NBSP ();

	 /***** Form to go to select countries *****/
	 Frm_BeginFormGoTo (ActSeeCty);
	    HTM_BUTTON_Submit_Begin (Txt_HIERARCHY_SINGUL_Abc[Hie_CTY],NULL,
				     "class=\"BT_LINK\"");
	       HTM_Txt (Txt_HIERARCHY_SINGUL_Abc[Hie_CTY]);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();

      HTM_DIV_End ();
     }

   if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)		// Institution selected...
     {
      HTM_DIV_Begin ("class=\"BC BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_NBSP ();
	 HTM_GT ();
	 HTM_NBSP ();

	 /***** Form to see centers of this institution *****/
	 Frm_BeginFormGoTo (ActSeeCtr);
	    ParCod_PutPar (ParCod_Ins,Gbl.Hierarchy.Node[Hie_INS].HieCod);
	    HTM_BUTTON_Submit_Begin (Gbl.Hierarchy.Node[Hie_INS].FullName,NULL,
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
	 HTM_NBSP ();
	 HTM_GT ();
	 HTM_NBSP ();

	 /***** Form to go to select institutions *****/
	 Frm_BeginFormGoTo (ActSeeIns);
	    HTM_BUTTON_Submit_Begin (Txt_HIERARCHY_SINGUL_Abc[Hie_INS],NULL,
				     "class=\"BT_LINK\"");
	       HTM_Txt (Txt_HIERARCHY_SINGUL_Abc[Hie_INS]);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();

      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"BC BC_OFF BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_NBSP ();
	 HTM_GT ();
	 HTM_NBSP ();

	 /***** Hidden institution *****/
	 HTM_Txt (Txt_HIERARCHY_SINGUL_Abc[Hie_INS]);

      HTM_DIV_End ();
     }

   if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)	// Center selected...
     {
      HTM_DIV_Begin ("class=\"BC BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_NBSP ();
	 HTM_GT ();
	 HTM_NBSP ();

	 /***** Form to see degrees of this center *****/
	 Frm_BeginFormGoTo (ActSeeDeg);
	    ParCod_PutPar (ParCod_Ctr,Gbl.Hierarchy.Node[Hie_CTR].HieCod);
	    HTM_BUTTON_Submit_Begin (Gbl.Hierarchy.Node[Hie_CTR].FullName,NULL,
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
	 HTM_NBSP ();
	 HTM_GT ();
	 HTM_NBSP ();

	 /***** Form to go to select centers *****/
	 Frm_BeginFormGoTo (ActSeeCtr);
	    HTM_BUTTON_Submit_Begin (Txt_HIERARCHY_SINGUL_Abc[Hie_CTR],NULL,
				     "class=\"BT_LINK\"");
	       HTM_Txt (Txt_HIERARCHY_SINGUL_Abc[Hie_CTR]);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();

      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"BC BC_OFF BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_NBSP ();
	 HTM_GT ();
	 HTM_NBSP ();

	 /***** Hidden center *****/
	 HTM_Txt (Txt_HIERARCHY_SINGUL_Abc[Hie_CTR]);

      HTM_DIV_End ();
     }

   if (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0)	// Degree selected...
     {
      HTM_DIV_Begin ("class=\"BC BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_NBSP ();
	 HTM_GT ();
	 HTM_NBSP ();

	 /***** Form to go to see courses of this degree *****/
	 Frm_BeginFormGoTo (ActSeeCrs);
	    ParCod_PutPar (ParCod_Deg,Gbl.Hierarchy.Node[Hie_DEG].HieCod);
	    HTM_BUTTON_Submit_Begin (Gbl.Hierarchy.Node[Hie_DEG].FullName,NULL,
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
	 HTM_NBSP ();
	 HTM_GT ();
	 HTM_NBSP ();

	 /***** Form to go to select degrees *****/
	 Frm_BeginFormGoTo (ActSeeDeg);
	    HTM_BUTTON_Submit_Begin (Txt_HIERARCHY_SINGUL_Abc[Hie_DEG],NULL,
				     "class=\"BT_LINK\"");
	       HTM_Txt (Txt_HIERARCHY_SINGUL_Abc[Hie_DEG]);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();

      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"BC BC_OFF BC_%s\"",The_GetSuffix ());

	 /***** Separator *****/
	 HTM_NBSP ();
	 HTM_GT ();
	 HTM_NBSP ();

	 /***** Hidden degree *****/
	 HTM_Txt (Txt_HIERARCHY_SINGUL_Abc[Hie_DEG]);

      HTM_DIV_End ();
     }

   HTM_DIV_Begin ("class=\"BC%s BC_%s\"",
		   (Gbl.Hierarchy.HieLvl == Hie_CRS) ? "" :
		  ((Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0) ? " BC_SEMIOFF" :
							      " BC_OFF"),
		  The_GetSuffix ());

      /***** Separator *****/
      HTM_NBSP ();
      HTM_GT ();
      HTM_NBSP ();

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

   HTM_TxtF ("<h1 id=\"main_title\" class=\"MAIN_TITLE_%s\">",The_GetSuffix ());

      /***** Logo and text *****/
      HTM_DIV_Begin ("id=\"big_name_container\"");

	 Frm_BeginForm (ActMnu);
	    Par_PutParUnsigned (NULL,"NxtTab",(unsigned) NextTab[Gbl.Hierarchy.HieLvl]);
	    HTM_BUTTON_Submit_Begin (Gbl.Hierarchy.Node[Gbl.Hierarchy.HieLvl].ShrtName,NULL,
				     "class=\"BT_LINK ICO_HIGHLIGHT\"");

	       HTM_DIV_Begin ("id=\"big_full_name\"");	// Full name
		     Hie_DrawLogo ();
		     HTM_Txt (Gbl.Hierarchy.Node[Gbl.Hierarchy.HieLvl].FullName);
	       HTM_DIV_End ();

	       HTM_DIV_Begin ("id=\"big_short_name\"");	// Short name
		     Hie_DrawLogo ();
		     HTM_Txt (Gbl.Hierarchy.Node[Gbl.Hierarchy.HieLvl].ShrtName);
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
   /***** Logo *****/
   Lgo_DrawLogo (Gbl.Hierarchy.HieLvl,&Gbl.Hierarchy.Node[Gbl.Hierarchy.HieLvl],
		 "TOP_LOGO ICO40x40");
  }

/*****************************************************************************/
/**************** Copy last hierarchy to current hierarchy *******************/
/*****************************************************************************/

void Hie_SetHierarchyFromUsrLastHierarchy (void)
  {
   /***** Initialize all codes to -1 *****/
   Hie_ResetHierarchy ();

   /***** Copy last hierarchy code to current hierarchy *****/
   Gbl.Hierarchy.Node[Gbl.Usrs.Me.UsrLast.LastHie.HieLvl].HieCod = Gbl.Usrs.Me.UsrLast.LastHie.HieCod;

   /****** Initialize again current course, degree, center... ******/
   Hie_InitHierarchy ();
  }

/*****************************************************************************/
/**** Initialize current country, institution, center, degree and course *****/
/*****************************************************************************/

void Hie_InitHierarchy (void)
  {
   Hie_Level_t HieLvl;

   /***** If course code is available, get course data *****/
   if (Gbl.Hierarchy.Node[Hie_CRS].HieCod > 0)
      switch (Hie_GetDataByCod[Hie_CRS] (&Gbl.Hierarchy.Node[Hie_CRS]))	// Course found
	{
	 case Err_SUCCESS:
	    Gbl.Hierarchy.Node[Hie_DEG].HieCod = Gbl.Hierarchy.Node[Hie_CRS].PrtCod;
	    break;
	 case Err_ERROR:
	 default:
	    Hie_ResetHierarchy ();
	    break;
	}

   /***** If degree code is available, get degree data *****/
   if (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0)
      switch (Hie_GetDataByCod[Hie_DEG] (&Gbl.Hierarchy.Node[Hie_DEG]))	// Degree found
	{
	 case Err_SUCCESS:
	    Gbl.Hierarchy.Node[Hie_CTR].HieCod = Gbl.Hierarchy.Node[Hie_DEG].PrtCod;
	    Gbl.Hierarchy.Node[Hie_INS].HieCod = Deg_DB_GetInsCodOfDegreeByCod (Gbl.Hierarchy.Node[Hie_DEG].HieCod);
	    break;
	 case Err_ERROR:
	 default:
	    Hie_ResetHierarchy ();
	    break;
	}

   /***** If center code is available, get center data *****/
   if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)
      switch (Hie_GetDataByCod[Hie_CTR] (&Gbl.Hierarchy.Node[Hie_CTR]))	// Center found
	{
	 case Err_SUCCESS:
	    Gbl.Hierarchy.Node[Hie_INS].HieCod = Gbl.Hierarchy.Node[Hie_CTR].PrtCod;
	    break;
	 case Err_ERROR:
	 default:
	    Hie_ResetHierarchy ();
	    break;
	}

   /***** If institution code is available, get institution data *****/
   if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)
      switch (Hie_GetDataByCod[Hie_INS] (&Gbl.Hierarchy.Node[Hie_INS]))	// Institution found
	{
	 case Err_SUCCESS:
	    Gbl.Hierarchy.Node[Hie_CTY].HieCod = Gbl.Hierarchy.Node[Hie_INS].PrtCod;
	    break;
	 case Err_ERROR:
	 default:
	    Hie_ResetHierarchy ();
	    break;
	}

   /***** If country code is available, get country data *****/
   if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)
      if (Hie_GetDataByCod[Hie_CTY] (&Gbl.Hierarchy.Node[Hie_CTY]) == Err_ERROR)	// Country not found
         Hie_ResetHierarchy ();

   /***** Set system data *****/
   Str_Copy (Gbl.Hierarchy.Node[Hie_SYS].ShrtName,Cfg_PLATFORM_SHORT_NAME,
	     sizeof (Gbl.Hierarchy.Node[Hie_SYS].ShrtName) - 1);
   Str_Copy (Gbl.Hierarchy.Node[Hie_SYS].FullName,Cfg_PLATFORM_FULL_NAME ,
	     sizeof (Gbl.Hierarchy.Node[Hie_SYS].FullName) - 1);
   Str_Copy (Gbl.Hierarchy.Node[Hie_SYS].WWW     ,Cfg_URL_SWAD_PUBLIC    ,
	     sizeof (Gbl.Hierarchy.Node[Hie_SYS].WWW     ) - 1);

   /***** Set current hierarchy level *****/
   Gbl.Hierarchy.HieLvl = Hie_SYS;
   for (HieLvl  = Hie_CRS;
	HieLvl >= Hie_CTY;
	HieLvl--)
      if (Gbl.Hierarchy.Node[HieLvl].HieCod > 0)
        {
	 Gbl.Hierarchy.HieLvl = HieLvl;
         break;
        }

   /***** Initialize paths *****/
   if (Gbl.Hierarchy.HieLvl == Hie_CRS)	// Course selected
     {
      /***** Paths of course directories *****/
      snprintf (Gbl.Crs.Path.AbsPriv,sizeof (Gbl.Crs.Path.AbsPriv),"%s/%ld",
	        Cfg_PATH_CRS_PRIVATE,Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      snprintf (Gbl.Crs.Path.RelPubl,sizeof (Gbl.Crs.Path.RelPubl),"%s/%ld",
	        Cfg_PATH_CRS_PUBLIC ,Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      snprintf (Gbl.Crs.Path.URLPubl,sizeof (Gbl.Crs.Path.URLPubl),"%s/%ld",
	        Cfg_URL_CRS_PUBLIC  ,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

      /***** If any of the course directories does not exist, create it *****/
      if (Fil_CheckIfPathExists (Gbl.Crs.Path.AbsPriv) == Exi_DOES_NOT_EXIST)
	 Fil_CreateDirIfNotExists (Gbl.Crs.Path.AbsPriv);
      if (Fil_CheckIfPathExists (Gbl.Crs.Path.RelPubl) == Exi_DOES_NOT_EXIST)
	 Fil_CreateDirIfNotExists (Gbl.Crs.Path.RelPubl);

      /***** Count number of groups in current course
             (used in some actions) *****/
      Gbl.Crs.Grps.NumGrps = Grp_DB_CountNumGrpsInCurrentCrs ();
      if (Gbl.Crs.Grps.NumGrps == 0)
	 Gbl.Crs.Grps.MyAllGrps = Grp_ALL_GROUPS;
     }
  }

/*****************************************************************************/
/******* Reset current country, institution, center, degree and course *******/
/*****************************************************************************/

void Hie_ResetHierarchy (void)
  {
   Hie_Level_t HieLvl;

   Gbl.Hierarchy.HieLvl = Hie_UNK;

   for (HieLvl  = (Hie_Level_t) 0;
	HieLvl <= (Hie_Level_t) Hie_NUM_LEVELS - 1;
	HieLvl++)
     {
      Gbl.Hierarchy.List[HieLvl].Num = 0;
      Gbl.Hierarchy.List[HieLvl].Lst = NULL;
      Gbl.Hierarchy.List[HieLvl].SelectedOrder = Hie_ORDER_DEFAULT;
      Gbl.Hierarchy.Node[HieLvl].HieCod = -1L;
      Gbl.Hierarchy.Node[HieLvl].PrtCod = -1L;
      Gbl.Hierarchy.Node[HieLvl].ShrtName[0] =
      Gbl.Hierarchy.Node[HieLvl].FullName[0] = '\0';
      Gbl.Hierarchy.Node[HieLvl].WWW[0]      = '\0';
      Gbl.Usrs.Me.IBelongToCurrent[HieLvl] = Usr_DONT_BELONG;
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
   __attribute__((unused)) Err_SuccessOrError_t SuccessOrError;

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
		       ColSpan - 1,The_GetSuffix (),The_GetColorRows ());

	    /* Get next institution, center, degree */
	    row = mysql_fetch_row (mysql_res);

	    /* Get scope */
	    switch (Sco_GetScopeFromUnsignedStr (row[0]))
	      {
	       case Hie_SYS:	// System
		  Ico_PutIcon ("swad64x64.png",Ico_UNCHANGED,
		               Txt_all_degrees,"ICO16x16");
		  HTM_NBSP ();
		  HTM_Txt (Txt_all_degrees);
		  break;
	       case Hie_INS:	// Institution
		  if ((Hie[Hie_INS].HieCod = Str_ConvertStrCodToLongCod (row[1])) > 0)
		    {
		     /* Get data of institution */
		     SuccessOrError = Hie_GetDataByCod[Hie_INS] (&Hie[Hie_INS]);

		     /* Write institution logo and name */
		     Ins_DrawInstitLogoAndNameWithLink (&Hie[Hie_INS],ActSeeInsInf,"LT ICO16x16");
		    }
		  break;
	       case Hie_CTR:	// Center
		  if ((Hie[Hie_CTR].HieCod = Str_ConvertStrCodToLongCod (row[1])) > 0)
		    {
		     /* Get data of center */
		     SuccessOrError = Hie_GetDataByCod[Hie_CTR] (&Hie[Hie_CTR]);

		     /* Write center logo and name */
		     Ctr_DrawCenterLogoAndNameWithLink (&Hie[Hie_CTR],ActSeeCtrInf,"LT ICO16x16");
		    }
		  break;
	       case Hie_DEG:	// Degree
		  if ((Hie[Hie_DEG].HieCod = Str_ConvertStrCodToLongCod (row[1])) > 0)
		    {
		     /* Get data of degree */
		     SuccessOrError = Hie_GetDataByCod[Hie_DEG] (&Hie[Hie_DEG]);

		     /* Write degree logo and name */
		     Deg_DrawDegreeLogoAndNameWithLink (&Hie[Hie_DEG],ActSeeDegInf,"LT ICO20x20");
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

Usr_Can_t Hie_CheckIfICanEdit (void)
  {
   // Some admins can edit all hierarchy elements.
   // Any user can edit the elements he/she has created...
   // ...as long as they are in pending status.
   static Usr_Can_t ICanEdit[Rol_NUM_ROLES] =
     {
      /* Users who can edit */
      [Rol_GST    ] = Usr_CAN,
      [Rol_USR    ] = Usr_CAN,
      [Rol_STD    ] = Usr_CAN,
      [Rol_NET    ] = Usr_CAN,
      [Rol_TCH    ] = Usr_CAN,
      [Rol_DEG_ADM] = Usr_CAN,
      [Rol_CTR_ADM] = Usr_CAN,
      [Rol_INS_ADM] = Usr_CAN,
      [Rol_SYS_ADM] = Usr_CAN,
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

   HTM_TD_Begin ("class=\"LT %s_%s %s\"",Class,The_GetSuffix (),BgColor);
      if (StatusTxt != Hie_STATUS_ACTIVE) // If active ==> do not show anything
	 HTM_Txt (Txt[StatusTxt]);
   HTM_TD_End ();
  }

void Hie_WriteStatusCellEditable (Usr_Can_t ICanEdit,Hie_Status_t Status,
                                  Act_Action_t NextAction,long HieCod,
                                  const char *Txt[Hie_NUM_STATUS_TXT])
  {
   Hie_StatusTxt_t StatusTxt = Hie_GetStatusTxtFromStatusBits (Status);
   unsigned StatusUnsigned;

   /***** Begin cell *****/
   HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
      if (ICanEdit == Usr_CAN && StatusTxt == Hie_STATUS_PENDING)
	{
	 /* Begin form */
	 Frm_BeginForm (NextAction);
	    ParCod_PutPar (ParCod_OthHie,HieCod);

	    /* Selector */
	    HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
			      "name=\"Status\" class=\"INPUT_STATUS\"");

	       StatusUnsigned = (unsigned) Hie_GetStatusBitsFromStatusTxt (Hie_STATUS_PENDING);
	       HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,
	                   HTM_SELECTED,
			   "%s",Txt[Hie_STATUS_PENDING]);

	       StatusUnsigned = (unsigned) Hie_GetStatusBitsFromStatusTxt (Hie_STATUS_ACTIVE);
	       HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,
	                   HTM_NO_ATTR,
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
   static Hie_Status_t StatusBits[Hie_NUM_STATUS_TXT] =
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
/***************** Free list of courses/degrees/centers... *******************/
/*****************************************************************************/

void Hie_FreeList (Hie_Level_t HieLvl)
  {
   if (Gbl.Hierarchy.List[HieLvl].Lst)
     {
      /***** Free memory used by the list of child nodes *****/
      free (Gbl.Hierarchy.List[HieLvl].Lst);
      Gbl.Hierarchy.List[HieLvl].Lst = NULL;
      Gbl.Hierarchy.List[HieLvl].Num = 0;
     }
  }

/*****************************************************************************/
/***** Reset lists of my courses/degrees/centers/institutions/countries ******/
/*****************************************************************************/

void Hie_ResetMyHierarchy (void)
  {
   Hie_Level_t HieLvl;

   for (HieLvl  = Hie_CTY;
	HieLvl <= Hie_CRS;
	HieLvl++)
     {
      Gbl.Usrs.Me.Hierarchy[HieLvl].Nodes = NULL;
      Gbl.Usrs.Me.Hierarchy[HieLvl].Num = 0;
      Gbl.Usrs.Me.Hierarchy[HieLvl].Filled = false;
      Gbl.Usrs.Me.IBelongToCurrent[HieLvl] = Usr_DONT_BELONG;
     }
  }

/*****************************************************************************/
/****** Free lists of my courses/degrees/centers/institutions/countries ******/
/*****************************************************************************/

void Hie_FreeMyHierarchy (void)
  {
   Hie_Level_t HieLvl;

   /***** Remove temporary table with my courses *****/
   if (Gbl.Usrs.Me.Hierarchy[Hie_CRS].Filled)
      Enr_DB_DropTmpTableMyCourses ();

   /***** Free allocated memory
          for my courses/degrees/centers/institutions/countries *****/
   for (HieLvl  = Hie_CTY;
	HieLvl <= Hie_CRS;
	HieLvl++)
      if (Gbl.Usrs.Me.Hierarchy[HieLvl].Filled &&
	  Gbl.Usrs.Me.Hierarchy[HieLvl].Num &&
	  Gbl.Usrs.Me.Hierarchy[HieLvl].Nodes)
         free (Gbl.Usrs.Me.Hierarchy[HieLvl].Nodes);

   Hie_ResetMyHierarchy ();
  }

/*****************************************************************************/
/********* Get all my courses/degrees/centers/institutions/countries *********/
/********* and store them in a list                                  *********/
/*****************************************************************************/

void Hie_GetMyHierarchy (Hie_Level_t HieLvl)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumNode;
   unsigned NumNodes;
   long HieCod;

   /***** Trivial check 1: if list of nodes is already filled, there's nothing to do *****/
   if (Gbl.Usrs.Me.Hierarchy[HieLvl].Filled)
      return;

   /***** Trivial check 2: if user's code is not set, don't query database *****/
   if (Gbl.Usrs.Me.UsrDat.UsrCod <= 0)
      return;

   /***** If retrieving my list of courses, create a temporary table with them *****/
   if (HieLvl == Hie_CRS)
     {
      Enr_DB_DropTmpTableMyCourses ();
      Enr_DB_CreateTmpTableMyCourses ();
     }

   /***** Set default values *****/
   Gbl.Usrs.Me.Hierarchy[HieLvl].Num   = 0;
   Gbl.Usrs.Me.Hierarchy[HieLvl].Nodes = NULL;

   /***** Get my courses/degrees/centers/institutions/countries from database *****/
   if ((NumNodes = Hie_GetMyNodesFromDB[HieLvl] (&mysql_res,-1L)))
     {
      if ((Gbl.Usrs.Me.Hierarchy[HieLvl].Nodes = malloc (NumNodes *
							sizeof (*Gbl.Usrs.Me.Hierarchy[HieLvl].Nodes))) == NULL)
	 Err_NotEnoughMemoryExit ();
      for (NumNode = 0;
	   NumNode < NumNodes;
	   NumNode++)
	{
	 /* Get next course/degree/center/institution/country */
	 row = mysql_fetch_row (mysql_res);

	 /* Get hierarchy code (row[0]) */
	 if ((HieCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
	   {
	    Gbl.Usrs.Me.Hierarchy[HieLvl].Nodes[Gbl.Usrs.Me.Hierarchy[HieLvl].Num].HieCod  = HieCod;

	    /* Get role or maximum role (row[1]) in this node */
	    Gbl.Usrs.Me.Hierarchy[HieLvl].Nodes[Gbl.Usrs.Me.Hierarchy[HieLvl].Num].MaxRole = Rol_ConvertUnsignedStrToRole (row[1]);

	    /* Get parent hierarchy code */
	    if (HieLvl == Hie_CRS)
	       Gbl.Usrs.Me.Hierarchy[HieLvl].Nodes[Gbl.Usrs.Me.Hierarchy[HieLvl].Num].PrtCod = Str_ConvertStrCodToLongCod (row[2]);

	    Gbl.Usrs.Me.Hierarchy[HieLvl].Num++;
	   }
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Set boolean that indicates that my institutions are yet filled *****/
   Gbl.Usrs.Me.Hierarchy[HieLvl].Filled = true;
  }

/*****************************************************************************/
/****** Check if I belong to a course/degree/center/institution/country ******/
/*****************************************************************************/

Usr_Belong_t Hie_CheckIfIBelongTo (Hie_Level_t HieLvl,long HieCod)
  {
   unsigned NumMyNode;

   /***** Fill the list with the nodes I belong to *****/
   Hie_GetMyHierarchy (HieLvl);

   /***** Check if the node passed as parameter is any of my nodes *****/
   for (NumMyNode = 0;
        NumMyNode < Gbl.Usrs.Me.Hierarchy[HieLvl].Num;
        NumMyNode++)
      if (Gbl.Usrs.Me.Hierarchy[HieLvl].Nodes[NumMyNode].HieCod == HieCod)
         return Usr_BELONG;
   return Usr_DONT_BELONG;
  }

/*****************************************************************************/
/*** Flush cache that stores if a user belongs to a node of the hierarchy ****/
/*****************************************************************************/

void Hie_FlushCacheUsrBelongsTo (Hie_Level_t HieLvl)
  {
   Gbl.Cache.UsrBelongsTo[HieLvl].Valid = false;
  }

/*****************************************************************************/
/******************** Check if a user belongs to a course ********************/
/*****************************************************************************/

Usr_Belong_t Hie_CheckIfUsrBelongsTo (Hie_Level_t HieLvl,long UsrCod,long HieCod,
				      bool CountOnlyAcceptedCourses)
  {
   static Usr_Belong_t (*FunctionToGetIfUsrBelongsToFromDB[Hie_NUM_LEVELS]) (long UsrCod,long HieCod,
									     bool CountOnlyAcceptedCourses) =
     {
      [Hie_INS] = Ins_DB_CheckIfUsrBelongsToIns,
      [Hie_CTR] = Ctr_DB_CheckIfUsrBelongsToCtr,
      [Hie_DEG] = Deg_DB_CheckIfUsrBelongsToDeg,
      [Hie_CRS] = Enr_DB_CheckIfUsrBelongsToCrs,
     };

   /***** Check if level is correct *****/
   if (!FunctionToGetIfUsrBelongsToFromDB[HieLvl])
      Err_WrongHierarchyLevelExit ();

   /***** 1. Fast check: Trivial cases *****/
   if (UsrCod <= 0 ||
       HieCod <= 0)
      return Usr_DONT_BELONG;

   /***** 2. Fast check: If cached... *****/
   if (Gbl.Cache.UsrBelongsTo[HieLvl].Valid &&
       UsrCod == Gbl.Cache.UsrBelongsTo[HieLvl].UsrCod &&
       HieCod == Gbl.Cache.UsrBelongsTo[HieLvl].HieCod &&
       CountOnlyAcceptedCourses == Gbl.Cache.UsrBelongsTo[HieLvl].CountOnlyAcceptedCourses)
      return Gbl.Cache.UsrBelongsTo[HieLvl].Belongs;

   /***** 3. Slow check: Get if user belongs to hierarchy node from database *****/
   Gbl.Cache.UsrBelongsTo[HieLvl].UsrCod = UsrCod;
   Gbl.Cache.UsrBelongsTo[HieLvl].HieCod = HieCod;
   Gbl.Cache.UsrBelongsTo[HieLvl].CountOnlyAcceptedCourses = CountOnlyAcceptedCourses;
   Gbl.Cache.UsrBelongsTo[HieLvl].Belongs = FunctionToGetIfUsrBelongsToFromDB[HieLvl] (UsrCod,HieCod,
										     CountOnlyAcceptedCourses);
   Gbl.Cache.UsrBelongsTo[HieLvl].Valid = true;
   return Gbl.Cache.UsrBelongsTo[HieLvl].Belongs;
  }

/*****************************************************************************/
/*********            Get and show stats about hierarchy           ***********/
/********* (countries, institutions, centers, degrees and courses) ***********/
/*****************************************************************************/

void Hie_GetAndShowHierarchyStats (Hie_Level_t HieLvl)
  {
   extern const char *Hlp_ANALYTICS_Figures_hierarchy;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   Hie_Level_t HavingNodesOfLevel;
   Rol_Role_t Role;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_HIERARCHY],NULL,NULL,
                      Hlp_ANALYTICS_Figures_hierarchy,Box_NOT_CLOSABLE,2);

      /* Head row */
      Hie_WriteHeadHierarchy ();

      /* Rows with number of nodes having nodes of each level */
      for (HavingNodesOfLevel  = Hie_INS;
	   HavingNodesOfLevel <= Hie_CRS;
	   HavingNodesOfLevel++)
         Hie_GetAndShowHierarchyWithNodes (HieLvl,HavingNodesOfLevel);

      /* Rows with number of nodes having users of each role */
      for (Role  = Rol_TCH;
	   Role >= Rol_STD;
	   Role--)
	 Hie_GetAndShowHierarchyWithUsrs (HieLvl,Role);

      /* Row with total nodes */
      Hie_GetAndShowHierarchyTotal (HieLvl);

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/************************ Write head of hierarchy table **********************/
/*****************************************************************************/

static void Hie_WriteHeadHierarchy (void)
  {
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];
   Hie_Level_t HieLvl;

   HTM_TR_Begin (NULL);

      HTM_TH_Empty (1);

      for (HieLvl  = Hie_CTY;
	   HieLvl <= Hie_CRS;
	   HieLvl++)
	{
	 HTM_TH_Begin (HTM_HEAD_RIGHT);
	    Ico_PutIcon (Hie_Icons[HieLvl],Ico_BLACK,
			 Txt_HIERARCHY_PLURAL_Abc[HieLvl],"ICOx16");
	    HTM_BR ();
	    HTM_Txt (Txt_HIERARCHY_PLURAL_Abc[HieLvl]);
	 HTM_TH_End ();
	}

   HTM_TR_End ();
  }

/*****************************************************************************/
/********* Get and show number of elements in hierarchy with nodes ***********/
/*****************************************************************************/

static void Hie_GetAndShowHierarchyWithNodes (Hie_Level_t HieLvl,
					      Hie_Level_t HavingNodesOfLevel)
  {
   extern const char *Txt_With_;
   extern const char *Txt_HIERARCHY_PLURAL_abc[Hie_NUM_LEVELS];
   int NumNodes[Hie_NUM_LEVELS];
   Hie_Level_t HieLvlChild;

   /***** Get number of elements with courses *****/
   // For each level country, institution, center, degree and course...
   for (HieLvlChild  = Hie_CTY;
        HieLvlChild <= Hie_CRS;
        HieLvlChild++)
      if (HieLvlChild >= HavingNodesOfLevel)	// Example: don't show number of centers with institutions
	 NumNodes[HieLvlChild] = -1;
      else if (HavingNodesOfLevel <= HieLvl)	// Example: if scope is center (4)
	                                     	//          number of nodes with instit./countries/centers
	 NumNodes[HieLvlChild] = 1;		//          in current center is 1
      else
         NumNodes[HieLvlChild] = (int) Hie_GetCachedNumNodesInHieLvlWith (HieLvlChild,		// Child
        							          HieLvl,		// Parent
        							          HavingNodesOfLevel);	// Grand child

   /***** Write number of elements with courses *****/
   Hie_ShowHierarchyRow (Txt_With_,Txt_HIERARCHY_PLURAL_abc[HavingNodesOfLevel],
			 "DAT",NumNodes);
  }

/*****************************************************************************/
/********** Get and show number of elements in hierarchy with users **********/
/*****************************************************************************/

static void Hie_GetAndShowHierarchyWithUsrs (Hie_Level_t HieLvl,Rol_Role_t Role)
  {
   extern const char *Txt_With_;
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   static unsigned (*FunctionGetCachedNumNodesWithusrs[Hie_NUM_LEVELS]) (Hie_Level_t HieLvl,Rol_Role_t Role) =
     {
      [Hie_CTY] = Cty_GetCachedNumCtysWithUsrs,
      [Hie_INS] = Ins_GetCachedNumInssWithUsrs,
      [Hie_CTR] = Ctr_GetCachedNumCtrsWithUsrs,
      [Hie_DEG] = Deg_GetCachedNumDegsWithUsrs,
      [Hie_CRS] = Crs_GetCachedNumCrssWithUsrs,
     };
   int NumNodes[Hie_NUM_LEVELS];
   Hie_Level_t L;

   /***** Get number of nodes with users *****/
   for (L  = Hie_CTY;
        L <= Hie_CRS;
        L++)
      NumNodes[L] = FunctionGetCachedNumNodesWithusrs[L] (HieLvl,Role);

   /***** Write number of elements with users *****/
   Hie_ShowHierarchyRow (Txt_With_,Txt_ROLES_PLURAL_abc[Role][Usr_SEX_UNKNOWN],
			 "DAT",NumNodes);
  }

/*****************************************************************************/
/************ Get and show total number of elements in hierarchy *************/
/*****************************************************************************/

static void Hie_GetAndShowHierarchyTotal (Hie_Level_t HieLvl)
  {
   extern const char *Txt_Total;
   Hie_Level_t L;
   int NumNodes[Hie_NUM_LEVELS];

   /***** Get total number of nodes of each level in current scope *****/
   for (L  = Hie_CTY;
	L <= Hie_CRS;
	L++)
      if (L > HieLvl)
	 NumNodes[L] = (int) Hie_GetCachedNumNodesInHieLvl (L,HieLvl,
							    Gbl.Hierarchy.Node[HieLvl].HieCod);
      else
	 NumNodes[L] = 1;

   /***** Write total number of elements *****/
   Hie_ShowHierarchyRow ("",Txt_Total,"LINE_TOP DAT_STRONG",NumNodes);
  }

/*****************************************************************************/
/**** Get total number of courses/degrees/centers/institutions in country ****/
/*****************************************************************************/

void Hie_FlushCachedNumNodesInHieLvl (Hie_Level_t HieLvlChild,
		      		      Hie_Level_t HieLvlParent)
  {
   Gbl.Cache.NumNodesInHieLvl[HieLvlChild][HieLvlParent].Valid = false;
  }

unsigned Hie_GetCachedNumNodesInHieLvl (Hie_Level_t HieLvlChild,
		      		        Hie_Level_t HieLvlParent,long HieCod)
  {
   unsigned NumNodes;

   /***** Get number of nodes from cache *****/
   if (FigCch_GetFigureFromCache (Hie_FiguresCached[HieLvlChild],HieLvlParent,HieCod,
				  FigCch_UNSIGNED,&NumNodes) == Exi_DOES_NOT_EXIST)
      /***** Get current number of nodes from database and update cache *****/
      NumNodes = Hie_GetNumNodesInHieLvl (HieLvlChild,HieLvlParent,HieCod);

   return NumNodes;
  }

unsigned Hie_GetNumNodesInHieLvl (Hie_Level_t HieLvlChild,
		      	          Hie_Level_t HieLvlParent,long HieCod)
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
   if (Gbl.Cache.NumNodesInHieLvl[HieLvlChild][HieLvlParent].Valid &&
       HieCod == Gbl.Cache.NumNodesInHieLvl[HieLvlChild][HieLvlParent].HieCod)
      return Gbl.Cache.NumNodesInHieLvl[HieLvlChild][HieLvlParent].Num;

   /***** 2. Slow: number of institutions in a country from database *****/
   Gbl.Cache.NumNodesInHieLvl[HieLvlChild][HieLvlParent].HieCod = HieCod;
   Gbl.Cache.NumNodesInHieLvl[HieLvlChild][HieLvlParent].Num    = FunctionGetFigure[HieLvlChild][HieLvlParent] (HieCod);
   Gbl.Cache.NumNodesInHieLvl[HieLvlChild][HieLvlParent].Valid  = true;
   FigCch_UpdateFigureIntoCache (Hie_FiguresCached[HieLvlChild],HieLvlParent,
				 Gbl.Cache.NumNodesInHieLvl[HieLvlChild][HieLvlParent].HieCod,
				 FigCch_UNSIGNED,&Gbl.Cache.NumNodesInHieLvl[HieLvlChild][HieLvlParent].Num);
   return Gbl.Cache.NumNodesInHieLvl[HieLvlChild][HieLvlParent].Num;
  }

/*****************************************************************************/
/**** Get number of children nodes in parent node having nodes of a level ****/
/*****************************************************************************/

unsigned Hie_GetCachedNumNodesInHieLvlWith (Hie_Level_t HieLvlChild,
					    Hie_Level_t HieLvlParent,
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
   static unsigned (*FunctionGetFigure[Hie_NUM_LEVELS][Hie_NUM_LEVELS]) (Hie_Level_t HieLvl,
									 long HieCod) =
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
   if (FigCch_GetFigureFromCache (Figure[HieLvlChild][HavingNodesOfLevel],
				  HieLvlParent,Gbl.Hierarchy.Node[HieLvlParent].HieCod,
				  FigCch_UNSIGNED,&NumNodes) == Exi_DOES_NOT_EXIST)
     {
      /***** Get current number of nodes with degrees from database and update cache *****/
      NumNodes = FunctionGetFigure[HieLvlChild][HavingNodesOfLevel] (HieLvlParent,
								       Gbl.Hierarchy.Node[HieLvlParent].HieCod);
      FigCch_UpdateFigureIntoCache (Figure[HieLvlChild][HavingNodesOfLevel],
				    HieLvlParent,Gbl.Hierarchy.Node[HieLvlParent].HieCod,
				    FigCch_UNSIGNED,&NumNodes);
     }

   return NumNodes;
  }

/*****************************************************************************/
/******** Get number of users who claim to belong to a hierarchy node ********/
/*****************************************************************************/

void Hie_FlushCacheNumUsrsWhoClaimToBelongTo (Hie_Level_t HieLvl)
  {
   Gbl.Cache.NumUsrsWhoClaimToBelongTo[HieLvl].Valid = false;
  }

unsigned Hie_GetCachedNumUsrsWhoClaimToBelongTo (Hie_Level_t HieLvl,
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
   if (FigCch_GetFigureFromCache (Figure[HieLvl],HieLvl,Node->HieCod,
                                  FigCch_UNSIGNED,&NumUsrs) == Exi_DOES_NOT_EXIST)
      /***** Get current number of users who claim to belong to hierarchy node
             from database and update cache *****/
      NumUsrs = Hie_GetNumUsrsWhoClaimToBelongTo (HieLvl,Node);

   return NumUsrs;
  }

unsigned Hie_GetNumUsrsWhoClaimToBelongTo (Hie_Level_t HieLvl,
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
   if (Gbl.Cache.NumUsrsWhoClaimToBelongTo[HieLvl].Valid &&
       Node->HieCod == Gbl.Cache.NumUsrsWhoClaimToBelongTo[HieLvl].HieCod)
     {
      Node->NumUsrsWhoClaimToBelong.NumUsrs = Gbl.Cache.NumUsrsWhoClaimToBelongTo[HieLvl].NumUsrs;
      Node->NumUsrsWhoClaimToBelong.Valid = true;
      return Node->NumUsrsWhoClaimToBelong.NumUsrs;
     }

   /***** 4. Slow: number of users who claim to belong to a hierarchy node
                   from database *****/
   Gbl.Cache.NumUsrsWhoClaimToBelongTo[HieLvl].HieCod  = Node->HieCod;
   Gbl.Cache.NumUsrsWhoClaimToBelongTo[HieLvl].NumUsrs =
   Node->NumUsrsWhoClaimToBelong.NumUsrs = FunctionToGetNumUsrsWhoClaimToBelongToFromDB[HieLvl] (Node->HieCod);
   Gbl.Cache.NumUsrsWhoClaimToBelongTo[HieLvl].Valid =
   Node->NumUsrsWhoClaimToBelong.Valid = true;
   FigCch_UpdateFigureIntoCache (Figure[HieLvl],HieLvl,Gbl.Cache.NumUsrsWhoClaimToBelongTo[HieLvl].HieCod,
				 FigCch_UNSIGNED,&Gbl.Cache.NumUsrsWhoClaimToBelongTo[HieLvl].NumUsrs);
   return Node->NumUsrsWhoClaimToBelong.NumUsrs;
  }

/*****************************************************************************/
/************** Show row with number of elements in hierarchy ****************/
/*****************************************************************************/

static void Hie_ShowHierarchyRow (const char *Text1,const char *Text2,
				  const char *ClassTxt,
				  int NumNodes[Hie_NUM_LEVELS])
  {
   Hie_Level_t HieLvl;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Write text *****/
      HTM_TD_Begin ("class=\"RM %s_%s\"",ClassTxt,The_GetSuffix ());
	 HTM_Txt (Text1);
	 HTM_Txt (Text2);
      HTM_TD_End ();

      /***** Write number of countries *****/
      for (HieLvl  = Hie_CTY;
	   HieLvl <= Hie_CRS;
	   HieLvl++)
         Hie_ShowHierarchyCell (ClassTxt,NumNodes[HieLvl]);

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

/*****************************************************************************/
/********** Put an icon (form) to select my courses in breadcrumb ************/
/*****************************************************************************/

void Hie_PutIconToSelectMyHierarchyInBreadcrumb (void)
  {
   extern const char *Txt_My_courses;

   if (Gbl.Usrs.Me.Logged)		// I am logged
     {
      /***** Begin form *****/
      Frm_BeginForm (ActMyCrs);

	 /***** Put icon with link *****/
	 HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,Hie_Icons[Hie_SYS],Txt_My_courses,
			  "class=\"BC_ICO BC_ICO_%s ICO_HIGHLIGHT\"",
			  The_GetSuffix ());

      /***** End form *****/
      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/***************** Put an icon (form) to select my hierarchy *****************/
/*****************************************************************************/

void Hie_PutIconToSelectMyHierarchy (__attribute__((unused)) void *Args)
  {
   if (Gbl.Usrs.Me.Logged)		// I am logged
      Lay_PutContextualLinkOnlyIcon (ActMyCrs,NULL,
				     NULL,NULL,
				     "sitemap.svg",Ico_BLACK);
  }

/*****************************************************************************/
/****************** Select one node from my hierarchy ************************/
/*****************************************************************************/

void Hie_SelOneNodeFromMyHierarchy (void)
  {
   /***** Fill the list with the courses I belong to, if not filled *****/
   Hie_GetMyHierarchy (Hie_CRS);

   /***** Select one of my courses *****/
   if (Gbl.Usrs.Me.Hierarchy[Hie_CRS].Num)
      /* Show my courses */
      Hie_WriteListMyHierarchyToSelectNode ();
   else	// I am not enroled in any course
      /* Show help to enrol me */
      Hlp_ShowHelpWhatWouldYouLikeToDo ();
  }

/*****************************************************************************/
/************************ Write menu with my courses *************************/
/*****************************************************************************/

static void Hie_WriteListMyHierarchyToSelectNode (void)
  {
   extern const char *Hlp_PROFILE_Courses;
   extern const char *Txt_My_courses;
   struct Hie_Node Hie[Hie_NUM_LEVELS];
   Lay_Last_t IsLastItemInLevel[1 + 6];

   /***** Begin box *****/
   Box_BoxBegin (Txt_My_courses,Hie_PutIconToSearchCourses,NULL,
                 Hlp_PROFILE_Courses,Box_NOT_CLOSABLE);

      /***** Begin list *****/
      HTM_UL_Begin ("class=\"LIST_TREE\"");

	 /***** Write nodes recursively *****/
	 Hie_WriteMyHierarchyNodes (Hie,IsLastItemInLevel,Hie_SYS);

      /***** End list *****/
      HTM_UL_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************* Put an icon (form) to search courses ********************/
/*****************************************************************************/

static void Hie_PutIconToSearchCourses (__attribute__((unused)) void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (ActReqSch,NULL,
				  Sch_PutLinkToSearchCoursesPars,NULL,
				  "search.svg",Ico_BLACK);
  }

/*****************************************************************************/
/****************** Write my hierarchy nodes recursively *********************/
/*****************************************************************************/

static void Hie_WriteMyHierarchyNodes (struct Hie_Node Hie[Hie_NUM_LEVELS],
			               Lay_Last_t IsLastItemInLevel[1 + 6],
			               Hie_Level_t HieLvl)
  {
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   Lay_Highlight_t Highlight;	// Highlight because degree, course, etc. is selected
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumNode;
   unsigned NumNodes;

   if (HieLvl > Hie_SYS)
      NumNodes = Hie_GetMyNodesFromDB[HieLvl] (&mysql_res,Hie[HieLvl - 1].HieCod);
   else
      NumNodes = 1;

   for (NumNode = 0;
	NumNode < NumNodes;
	NumNode++)
     {
      if (HieLvl > Hie_SYS)
        {
         /***** Get next node *****/
         row = mysql_fetch_row (mysql_res);
         Hie[HieLvl].HieCod = Str_ConvertStrCodToLongCod (row[0]);

	 /***** Get data of this node *****/
	 if (Hie_GetDataByCod[HieLvl] (&Hie[HieLvl]) == Err_ERROR)
	    Err_WrongCountrExit ();
        }
      else
        {
	 Hie[HieLvl].HieCod = -1L;
	 Str_Copy (Hie[HieLvl].ShrtName,Txt_HIERARCHY_SINGUL_Abc[HieLvl],
		   sizeof (Hie[HieLvl].ShrtName) - 1);
        }

      /***** Write link to node *****/
      if (HieLvl < Hie_CRS)
	 Highlight = (Gbl.Hierarchy.Node[HieLvl + 1].HieCod <= 0 &&
		      Gbl.Hierarchy.Node[HieLvl].HieCod == Hie[HieLvl].HieCod) ? Lay_HIGHLIGHT :
									         Lay_NO_HIGHLIGHT;
      else
	 Highlight = (Gbl.Hierarchy.Node[HieLvl].HieCod == Hie[HieLvl].HieCod) ? Lay_HIGHLIGHT :
									         Lay_NO_HIGHLIGHT;

      IsLastItemInLevel[HieLvl] = (NumNode == NumNodes - 1) ? Lay_LAST :
							      Lay_NO_LAST;
      Hie_WriteRowMyHierarchy (HieLvl,Hie,Highlight,IsLastItemInLevel);

      /***** Write subnodes recursively ******/
      if (HieLvl < Hie_CRS)
	 Hie_WriteMyHierarchyNodes (Hie,IsLastItemInLevel,HieLvl + 1);
     }

   if (HieLvl > Hie_SYS)
      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Write a row in my hierarchy ************************/
/*****************************************************************************/

static void Hie_WriteRowMyHierarchy (Hie_Level_t HieLvl,
				     const struct Hie_Node Hie[Hie_NUM_LEVELS],
				     Lay_Highlight_t Highlight,
				     Lay_Last_t IsLastItemInLevel[1 + 6])
  {
   extern const char *Lay_HighlightClass[Lay_NUM_HIGHLIGHT];
   static Act_Action_t Actions[Hie_NUM_LEVELS] =
     {
      [Hie_SYS] = ActSeeSysInf,
      [Hie_CTY] = ActSeeCtyInf,
      [Hie_INS] = ActSeeInsInf,
      [Hie_CTR] = ActSeeCtrInf,
      [Hie_DEG] = ActSeeDegInf,
      [Hie_CRS] = ActSeeCrsInf,
     };

   HTM_LI_Begin (Lay_HighlightClass[Highlight]);
      Lay_IndentDependingOnLevel (HieLvl,IsLastItemInLevel,
				  Lay_HORIZONTAL_LINE_AT_RIGHT);
      Frm_BeginForm (ActMyCrs);
	 ParCod_PutPar (Hie_ParCod[HieLvl],Hie[HieLvl].HieCod);
	 HTM_BUTTON_Submit_Begin (Act_GetActionText (Actions[HieLvl]),NULL,
				  "class=\"BT_LINK FORM_IN_%s\"",
				  The_GetSuffix ());

	    /* Node logo */
	    Lgo_DrawLogo (HieLvl,&Hie[HieLvl],"ICO16x16");

	    /* Node name */
	    HTM_DIV_Begin ("class=\"MY_CRS_TXT\"");
	       HTM_NBSP ();
	       HTM_Txt (Hie[HieLvl].ShrtName);
	    HTM_DIV_End ();

	 HTM_BUTTON_End ();
      Frm_EndForm ();
   HTM_LI_End ();
  }
