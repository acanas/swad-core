// swad_hierarchy.c: hierarchy (system, institution, centre, degree, course)

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
#include <stdio.h>		// For fprintf, asprintf, etc.
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_config.h"
#include "swad_degree.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_logo.h"
#include "swad_QR.h"
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
/**************************** Private constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************ Show title in configuration ************************/
/*****************************************************************************/

void Hie_ConfigTitle (bool PutLink,
		      Hie_Level_t LogoScope,
		      long LogoCod,
                      char LogoShrtName[Hie_MAX_BYTES_SHRT_NAME + 1],
		      char LogoFullName[Hie_MAX_BYTES_FULL_NAME + 1],
		      char LogoWWW[Cns_MAX_BYTES_WWW + 1],
		      char TextFullName[Hie_MAX_BYTES_FULL_NAME + 1])
  {
   HTM_DIV_Begin ("class=\"FRAME_TITLE FRAME_TITLE_BIG\"");
   if (PutLink)
      HTM_A_Begin ("href=\"%s\" target=\"_blank\""
	           " class=\"FRAME_TITLE_BIG\" title=\"%s\"",
	           LogoWWW,LogoFullName);
   Lgo_DrawLogo (LogoScope,LogoCod,LogoShrtName,64,NULL,true);
   HTM_BR ();
   HTM_Txt (TextFullName);
   if (PutLink)
      HTM_A_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************** Show full name in configuration **********************/
/*****************************************************************************/

void Hie_ConfigFullName (bool PutForm,const char *Label,Act_Action_t NextAction,
			 const char FullName[Hie_MAX_BYTES_FULL_NAME + 1])
  {
   HTM_TR_Begin (NULL);

   Frm_LabelColumn ("RM",PutForm ? "FullName" :
	                           NULL,
		    Label);

   HTM_TD_Begin ("class=\"DAT_N LM\"");
   if (PutForm)
     {
      /* Form to change full name */
      Frm_StartForm (NextAction);
      HTM_INPUT_TEXT ("FullName",Hie_MAX_CHARS_FULL_NAME,FullName,true,
		      "id=\"FullName\" class=\"INPUT_FULL_NAME\"");
      Frm_EndForm ();
     }
   else	// I can not edit full name
      HTM_Txt (FullName);
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/********* Show institution short name in institution configuration **********/
/*****************************************************************************/

void Hie_ConfigShrtName (bool PutForm,Act_Action_t NextAction,
			 const char ShrtName[Hie_MAX_BYTES_SHRT_NAME + 1])
  {
   extern const char *Txt_Short_name;

   HTM_TR_Begin (NULL);

   Frm_LabelColumn ("RM",PutForm ? "ShortName" :
	                           NULL,
		    Txt_Short_name);

   HTM_TD_Begin ("class=\"DAT_N LM\"");
   if (PutForm)
     {
      /* Form to change short name */
      Frm_StartForm (NextAction);
      HTM_INPUT_TEXT ("ShortName",Hie_MAX_CHARS_SHRT_NAME,ShrtName,true,
		      "id=\"ShortName\" class=\"INPUT_SHORT_NAME\"");
      Frm_EndForm ();
     }
   else	// I can not edit short name
      HTM_Txt (ShrtName);
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************************* Show web in configuration *************************/
/*****************************************************************************/

void Hie_ConfigWWW (bool PutForm,Act_Action_t NextAction,
		    const char WWW[Cns_MAX_BYTES_WWW + 1])
  {
   extern const char *Txt_Web;

   HTM_TR_Begin (NULL);

   Frm_LabelColumn ("RM",PutForm ? "WWW" :
	                           NULL,
		    Txt_Web);

   HTM_TD_Begin ("class=\"DAT LM\"");
   if (PutForm)
     {
      /* Form to change web */
      Frm_StartForm (NextAction);
      HTM_INPUT_URL ("WWW",WWW,true,
		     "id=\"WWW\" class=\"INPUT_WWW_WIDE\" required=\"required\"");
      Frm_EndForm ();
     }
   else	// I can not change web
     {
      HTM_DIV_Begin ("class=\"EXTERNAL_WWW_LONG\"");
      HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"DAT\"",WWW);
      HTM_Txt (WWW);
      HTM_A_End ();
      HTM_DIV_End ();
     }
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/********************** Show shortcut in configuration ***********************/
/*****************************************************************************/

void Hie_ConfigShortcut (const char *ParamName,long HieCod)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_Shortcut;

   HTM_TR_Begin (NULL);

   Frm_LabelColumn ("RM",NULL,Txt_Shortcut);

   HTM_TD_Begin ("class=\"DAT LM\"");
   HTM_A_Begin ("href=\"%s/%s?%s=%ld\" class=\"DAT\" target=\"_blank\"",
                Cfg_URL_SWAD_CGI,
                Lan_STR_LANG_ID[Gbl.Prefs.Language],
                ParamName,HieCod);
   HTM_TxtF ("%s/%s?%s=%ld",
             Cfg_URL_SWAD_CGI,
             Lan_STR_LANG_ID[Gbl.Prefs.Language],
             ParamName,HieCod);
   HTM_A_End ();
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************************* Show QR in configuration **************************/
/*****************************************************************************/

void Hie_ConfigQR (const char *ParamName,long HieCod)
  {
   extern const char *Txt_QR_code;

   HTM_TR_Begin (NULL);

   Frm_LabelColumn ("RT",NULL,Txt_QR_code);

   HTM_TD_Begin ("class=\"DAT LT\"");
   QR_LinkTo (250,ParamName,HieCod);
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/********** List pending institutions, centres, degrees and courses **********/
/*****************************************************************************/

void Hie_SeePending (void)
  {
   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
   Crs_PutLinkToRemoveOldCrss ();	// Remove old courses
   Mnu_ContextMenuEnd ();

   /***** List countries with pending institutions *****/
   Cty_SeeCtyWithPendingInss ();

   /***** List institutions with pending centres *****/
   Ins_SeeInsWithPendingCtrs ();

   /***** List centres with pending degrees *****/
   Ctr_SeeCtrWithPendingDegs ();

   /***** List degrees with pending courses *****/
   Deg_SeeDegWithPendingCrss ();
  }

/*****************************************************************************/
/*** Write menu to select country, institution, centre, degree and course ****/
/*****************************************************************************/

void Hie_WriteMenuHierarchy (void)
  {
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;

   /***** Begin table *****/
   HTM_TABLE_BeginCenterPadding (2);

   /***** Write a 1st selector
          with all the countries *****/
   HTM_TR_Begin (NULL);

   Frm_LabelColumn ("RT","cty",Txt_Country);

   HTM_TD_Begin ("class=\"LT\"");
   Cty_WriteSelectorOfCountry ();
   HTM_TD_End ();

   HTM_TR_End ();

   if (Gbl.Hierarchy.Cty.CtyCod > 0)
     {
      /***** Write a 2nd selector
             with the institutions of selected country *****/
      HTM_TR_Begin (NULL);

      Frm_LabelColumn ("RT","ins",Txt_Institution);

      HTM_TD_Begin ("class=\"LT\"");
      Ins_WriteSelectorOfInstitution ();
      HTM_TD_End ();

      HTM_TR_End ();

      if (Gbl.Hierarchy.Ins.InsCod > 0)
        {
         /***** Write a 3rd selector
                with all the centres of selected institution *****/
         HTM_TR_Begin (NULL);

         Frm_LabelColumn ("RT","ctr",Txt_Centre);

         HTM_TD_Begin ("class=\"LT\"");
         Ctr_WriteSelectorOfCentre ();
         HTM_TD_End ();

         HTM_TR_End ();

         if (Gbl.Hierarchy.Ctr.CtrCod > 0)
           {
            /***** Write a 4th selector
                   with all the degrees of selected centre *****/
            HTM_TR_Begin (NULL);

            Frm_LabelColumn ("RT","deg",Txt_Degree);

            HTM_TD_Begin ("class=\"LT\"");
            Deg_WriteSelectorOfDegree ();
            HTM_TD_End ();

            HTM_TR_End ();

	    if (Gbl.Hierarchy.Deg.DegCod > 0)
	      {
	       /***** Write a 5th selector
		      with all the courses of selected degree *****/
	       HTM_TR_Begin (NULL);

               Frm_LabelColumn ("RT","crs",Txt_Course);

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
   extern const char *The_ClassBreadcrumb[The_NUM_THEMES];
   extern const char *Txt_System;
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Degree;
   const char *ClassTxt = The_ClassBreadcrumb[Gbl.Prefs.Theme];
   char *ClassLink;

   /***** Create CSS class of links *****/
   if (asprintf (&ClassLink,"BT_LINK %s",ClassTxt) < 0)
      Lay_NotEnoughMemoryExit ();

   /***** Form to go to the system *****/
   HTM_DIV_Begin ("class=\"BC %s\"",ClassTxt);
   HTM_NBSP ();

   Frm_StartFormGoTo (ActMnu);
   Par_PutHiddenParamUnsigned (NULL,"NxtTab",(unsigned) TabSys);
   HTM_BUTTON_SUBMIT_Begin (Txt_System,ClassLink,NULL);
   HTM_Txt (Txt_System);
   HTM_BUTTON_End ();
   Frm_EndForm ();

   HTM_DIV_End ();

   if (Gbl.Hierarchy.Cty.CtyCod > 0)		// Country selected...
     {
      HTM_DIV_Begin ("class=\"BC %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to go to see institutions of this country *****/
      Frm_StartFormGoTo (ActSeeIns);
      Cty_PutParamCtyCod (Gbl.Hierarchy.Cty.CtyCod);
      HTM_BUTTON_SUBMIT_Begin (Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language],ClassLink,NULL);
      HTM_Txt (Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"BC BC_SEMIOFF %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to go to select countries *****/
      Frm_StartFormGoTo (ActSeeCty);
      HTM_BUTTON_SUBMIT_Begin (Txt_Country,ClassLink,NULL);
      HTM_Txt (Txt_Country);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }

   if (Gbl.Hierarchy.Ins.InsCod > 0)		// Institution selected...
     {
      HTM_DIV_Begin ("class=\"BC %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to see centres of this institution *****/
      Frm_StartFormGoTo (ActSeeCtr);
      Ins_PutParamInsCod (Gbl.Hierarchy.Ins.InsCod);
      HTM_BUTTON_SUBMIT_Begin (Gbl.Hierarchy.Ins.FullName,ClassLink,NULL);
      HTM_Txt (Gbl.Hierarchy.Ins.ShrtName);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }
   else if (Gbl.Hierarchy.Cty.CtyCod > 0)
     {
      HTM_DIV_Begin ("class=\"BC BC_SEMIOFF %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to go to select institutions *****/
      Frm_StartFormGoTo (ActSeeIns);
      HTM_BUTTON_SUBMIT_Begin (Txt_Institution,ClassLink,NULL);
      HTM_Txt (Txt_Institution);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"BC BC_OFF %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Hidden institution *****/
      HTM_Txt (Txt_Institution);

      HTM_DIV_End ();
     }

   if (Gbl.Hierarchy.Ctr.CtrCod > 0)	// Centre selected...
     {
      HTM_DIV_Begin ("class=\"BC %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to see degrees of this centre *****/
      Frm_StartFormGoTo (ActSeeDeg);
      Ctr_PutParamCtrCod (Gbl.Hierarchy.Ctr.CtrCod);
      HTM_BUTTON_SUBMIT_Begin (Gbl.Hierarchy.Ctr.FullName,ClassLink,NULL);
      HTM_Txt (Gbl.Hierarchy.Ctr.ShrtName);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }
   else if (Gbl.Hierarchy.Ins.InsCod > 0)
     {
      HTM_DIV_Begin ("class=\"BC BC_SEMIOFF %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to go to select centres *****/
      Frm_StartFormGoTo (ActSeeCtr);
      HTM_BUTTON_SUBMIT_Begin (Txt_Centre,ClassLink,NULL);
      HTM_Txt (Txt_Centre);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"BC BC_OFF %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Hidden centre *****/
      HTM_Txt (Txt_Centre);

      HTM_DIV_End ();
     }

   if (Gbl.Hierarchy.Deg.DegCod > 0)	// Degree selected...
     {
      HTM_DIV_Begin ("class=\"BC %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to go to see courses of this degree *****/
      Frm_StartFormGoTo (ActSeeCrs);
      Deg_PutParamDegCod (Gbl.Hierarchy.Deg.DegCod);
      HTM_BUTTON_SUBMIT_Begin (Gbl.Hierarchy.Deg.FullName,ClassLink,NULL);
      HTM_Txt (Gbl.Hierarchy.Deg.ShrtName);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }
   else if (Gbl.Hierarchy.Ctr.CtrCod > 0)
     {
      HTM_DIV_Begin ("class=\"BC BC_SEMIOFF %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to go to select degrees *****/
      Frm_StartFormGoTo (ActSeeDeg);
      HTM_BUTTON_SUBMIT_Begin (Txt_Degree,ClassLink,NULL);
      HTM_Txt (Txt_Degree);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"BC BC_OFF %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Hidden degree *****/
      HTM_Txt (Txt_Degree);

      HTM_DIV_End ();
     }

   HTM_DIV_Begin ("class=\"BC%s %s\"",
		   (Gbl.Hierarchy.Level == Hie_CRS) ? "" :
		  ((Gbl.Hierarchy.Deg.DegCod > 0) ? " BC_SEMIOFF" :
						    " BC_OFF"),
		  ClassTxt);

   /***** Separator *****/
   HTM_Txt ("&nbsp;&gt;&nbsp;");

   HTM_DIV_End ();

   /***** Free memory used for CSS class of links *****/
   free (ClassLink);
  }

/*****************************************************************************/
/*************** Write course full name in the top of the page ***************/
/*****************************************************************************/

void Hie_WriteBigNameCtyInsCtrDegCrs (void)
  {
   extern const char *The_ClassCourse[The_NUM_THEMES];
   extern const char *Txt_TAGLINE;

   HTM_TxtF ("<h1 id=\"main_title\" class=\"%s\">",
	     The_ClassCourse[Gbl.Prefs.Theme]);

   /***** Logo *****/
   switch (Gbl.Hierarchy.Level)
     {
      case Hie_SYS:	// System
	 Ico_PutIcon ("swad64x64.png",Cfg_PLATFORM_FULL_NAME,"ICO40x40 TOP_LOGO");
         break;
      case Hie_CTY:	// Country
         Cty_DrawCountryMap (&Gbl.Hierarchy.Cty,"COUNTRY_MAP_TITLE");
         break;
      case Hie_INS:	// Institution
	 Lgo_DrawLogo (Hie_INS,Gbl.Hierarchy.Ins.InsCod,
		       Gbl.Hierarchy.Ins.ShrtName,40,"TOP_LOGO",false);
         break;
      case Hie_CTR:	// Centre
	 Lgo_DrawLogo (Hie_CTR,Gbl.Hierarchy.Ctr.CtrCod,
		       Gbl.Hierarchy.Ctr.ShrtName,40,"TOP_LOGO",false);
         break;
      case Hie_DEG:	// Degree
      case Hie_CRS:	// Course
	 Lgo_DrawLogo (Hie_DEG,Gbl.Hierarchy.Deg.DegCod,
		       Gbl.Hierarchy.Deg.ShrtName,40,"TOP_LOGO",false);
         break;
      default:
	 break;
     }

   /***** Text *****/
   HTM_DIV_Begin ("id=\"big_name_container\"");
   if (Gbl.Hierarchy.Cty.CtyCod > 0)
     {
      HTM_DIV_Begin ("id=\"big_full_name\"");
      HTM_Txt (	(Gbl.Hierarchy.Level == Hie_CRS) ? Gbl.Hierarchy.Crs.FullName :// Full name
	       ((Gbl.Hierarchy.Level == Hie_DEG) ? Gbl.Hierarchy.Deg.FullName :
	       ((Gbl.Hierarchy.Level == Hie_CTR) ? Gbl.Hierarchy.Ctr.FullName :
	       ((Gbl.Hierarchy.Level == Hie_INS) ? Gbl.Hierarchy.Ins.FullName :
	                                           Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]))));
      HTM_DIV_End ();

      HTM_DIV_Begin ("class=\"NOT_SHOWN\"");
      HTM_Txt (" / ");	// To separate
      HTM_DIV_End ();

      HTM_DIV_Begin ("id=\"big_short_name\"");
      HTM_Txt (	(Gbl.Hierarchy.Level == Hie_CRS) ? Gbl.Hierarchy.Crs.ShrtName :// Short name
	       ((Gbl.Hierarchy.Level == Hie_DEG) ? Gbl.Hierarchy.Deg.ShrtName :
	       ((Gbl.Hierarchy.Level == Hie_CTR) ? Gbl.Hierarchy.Ctr.ShrtName :
	       ((Gbl.Hierarchy.Level == Hie_INS) ? Gbl.Hierarchy.Ins.ShrtName :
	                                           Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]))));
      HTM_DIV_End ();
     }
   else	// No country specified ==> home page
     {
      HTM_DIV_Begin ("id=\"big_full_name\"");	// Full name
      HTM_TxtF ("%s:&nbsp;%s",Cfg_PLATFORM_SHORT_NAME,Txt_TAGLINE);
      HTM_DIV_End ();

      HTM_DIV_Begin ("class=\"NOT_SHOWN\"");
      HTM_Txt (" / ");	// To separate
      HTM_DIV_End ();

      HTM_DIV_Begin ("id=\"big_short_name\"");	// Short name
      HTM_Txt (Cfg_PLATFORM_SHORT_NAME);
      HTM_DIV_End ();
     }
   HTM_DIV_End ();
   HTM_TxtF ("</h1>");
  }

/*****************************************************************************/
/**************** Copy last hierarchy to current hierarchy *******************/
/*****************************************************************************/

void Hie_SetHierarchyFromUsrLastHierarchy (void)
  {
   /***** Initialize all codes to -1 *****/
   Hie_ResetHierarchy ();

   /***** Copy last hierarchy scope and code to current hierarchy *****/
   switch (Gbl.Usrs.Me.UsrLast.LastHie.Scope)
     {
      case Hie_CTY:	// Country
         Gbl.Hierarchy.Cty.CtyCod = Gbl.Usrs.Me.UsrLast.LastHie.Cod;
	 break;
      case Hie_INS:	// Institution
         Gbl.Hierarchy.Ins.InsCod = Gbl.Usrs.Me.UsrLast.LastHie.Cod;
	 break;
      case Hie_CTR:	// Centre
         Gbl.Hierarchy.Ctr.CtrCod = Gbl.Usrs.Me.UsrLast.LastHie.Cod;
	 break;
      case Hie_DEG:	// Degree
         Gbl.Hierarchy.Deg.DegCod = Gbl.Usrs.Me.UsrLast.LastHie.Cod;
	 break;
      case Hie_CRS:	// Course
         Gbl.Hierarchy.Crs.CrsCod = Gbl.Usrs.Me.UsrLast.LastHie.Cod;
	 break;
      default:
	 break;
     }

   /****** Initialize again current course, degree, centre... ******/
   Hie_InitHierarchy ();
  }

/*****************************************************************************/
/**** Initialize current country, institution, centre, degree and course *****/
/*****************************************************************************/

void Hie_InitHierarchy (void)
  {
   /***** If course code is available, get course data *****/
   if (Gbl.Hierarchy.Crs.CrsCod > 0)
     {
      if (Crs_GetDataOfCourseByCod (&Gbl.Hierarchy.Crs,Crs_GET_BASIC_DATA))		// Course found
         Gbl.Hierarchy.Deg.DegCod = Gbl.Hierarchy.Crs.DegCod;
      else
         Hie_ResetHierarchy ();
     }

   /***** If degree code is available, get degree data *****/
   if (Gbl.Hierarchy.Deg.DegCod > 0)
     {
      if (Deg_GetDataOfDegreeByCod (&Gbl.Hierarchy.Deg,Deg_GET_BASIC_DATA))		// Degree found
	{
	 Gbl.Hierarchy.Ctr.CtrCod = Gbl.Hierarchy.Deg.CtrCod;
         Gbl.Hierarchy.Ins.InsCod = Deg_GetInsCodOfDegreeByCod (Gbl.Hierarchy.Deg.DegCod);
	}
      else
         Hie_ResetHierarchy ();
     }

   /***** If centre code is available, get centre data *****/
   if (Gbl.Hierarchy.Ctr.CtrCod > 0)
     {
      if (Ctr_GetDataOfCentreByCod (&Gbl.Hierarchy.Ctr,Ctr_GET_BASIC_DATA))		// Centre found
         Gbl.Hierarchy.Ins.InsCod = Gbl.Hierarchy.Ctr.InsCod;
      else
         Hie_ResetHierarchy ();
     }

   /***** If institution code is available, get institution data *****/
   if (Gbl.Hierarchy.Ins.InsCod > 0)
     {
      if (Ins_GetDataOfInstitutionByCod (&Gbl.Hierarchy.Ins,Ins_GET_BASIC_DATA))	// Institution found
	 Gbl.Hierarchy.Cty.CtyCod = Gbl.Hierarchy.Ins.CtyCod;
      else
         Hie_ResetHierarchy ();
     }

   /***** If country code is available, get country data *****/
   if (Gbl.Hierarchy.Cty.CtyCod > 0)
     {
      if (!Cty_GetDataOfCountryByCod (&Gbl.Hierarchy.Cty,Cty_GET_BASIC_DATA))		// Country not found
         Hie_ResetHierarchy ();
     }

   /***** Set current hierarchy level and code
          depending on course code, degree code, etc. *****/
   if      (Gbl.Hierarchy.Crs.CrsCod > 0)	// Course selected
     {
      Gbl.Hierarchy.Level = Hie_CRS;
      Gbl.Hierarchy.Cod = Gbl.Hierarchy.Crs.CrsCod;
     }
   else if (Gbl.Hierarchy.Deg.DegCod > 0)	// Degree selected
     {
      Gbl.Hierarchy.Level = Hie_DEG;
      Gbl.Hierarchy.Cod = Gbl.Hierarchy.Deg.DegCod;
     }
   else if (Gbl.Hierarchy.Ctr.CtrCod > 0)	// Centre selected
     {
      Gbl.Hierarchy.Level = Hie_CTR;
      Gbl.Hierarchy.Cod = Gbl.Hierarchy.Ctr.CtrCod;
     }
   else if (Gbl.Hierarchy.Ins.InsCod > 0)	// Institution selected
     {
      Gbl.Hierarchy.Level = Hie_INS;
      Gbl.Hierarchy.Cod = Gbl.Hierarchy.Ins.InsCod;
     }
   else if (Gbl.Hierarchy.Cty.CtyCod > 0)	// Country selected
     {
      Gbl.Hierarchy.Level = Hie_CTY;
      Gbl.Hierarchy.Cod = Gbl.Hierarchy.Cty.CtyCod;
     }
   else
     {
      Gbl.Hierarchy.Level = Hie_SYS;
      Gbl.Hierarchy.Cod = -1L;
     }

   /***** Initialize paths *****/
   if (Gbl.Hierarchy.Level == Hie_CRS)	// Course selected
     {
      /***** Paths of course directories *****/
      snprintf (Gbl.Crs.PathPriv,sizeof (Gbl.Crs.PathPriv),
	        "%s/%ld",
	        Cfg_PATH_CRS_PRIVATE,Gbl.Hierarchy.Crs.CrsCod);
      snprintf (Gbl.Crs.PathRelPubl,sizeof (Gbl.Crs.PathRelPubl),
	        "%s/%ld",
	        Cfg_PATH_CRS_PUBLIC,Gbl.Hierarchy.Crs.CrsCod);
      snprintf (Gbl.Crs.PathURLPubl,sizeof (Gbl.Crs.PathURLPubl),
	        "%s/%ld",
	        Cfg_URL_CRS_PUBLIC,Gbl.Hierarchy.Crs.CrsCod);

      /***** If any of the course directories does not exist, create it *****/
      if (!Fil_CheckIfPathExists (Gbl.Crs.PathPriv))
	 Fil_CreateDirIfNotExists (Gbl.Crs.PathPriv);
      if (!Fil_CheckIfPathExists (Gbl.Crs.PathRelPubl))
	 Fil_CreateDirIfNotExists (Gbl.Crs.PathRelPubl);

      /***** Count number of groups in current course
             (used in some actions) *****/
      Gbl.Crs.Grps.NumGrps = Grp_CountNumGrpsInCurrentCrs ();
     }
  }

/*****************************************************************************/
/******* Reset current country, institution, centre, degree and course *******/
/*****************************************************************************/

void Hie_ResetHierarchy (void)
  {
   /***** Country *****/
   Gbl.Hierarchy.Cty.CtyCod = -1L;

   /***** Institution *****/
   Gbl.Hierarchy.Ins.InsCod = -1L;

   /***** Centre *****/
   Gbl.Hierarchy.Ctr.CtrCod = -1L;
   Gbl.Hierarchy.Ctr.InsCod = -1L;
   Gbl.Hierarchy.Ctr.PlcCod = -1L;

   /***** Degree *****/
   Gbl.Hierarchy.Deg.DegCod = -1L;

   /***** Course *****/
   Gbl.Hierarchy.Crs.CrsCod = -1L;

   /***** Hierarchy level and code *****/
   Gbl.Hierarchy.Level = Hie_UNK;
   Gbl.Hierarchy.Cod   = -1L;
  }
