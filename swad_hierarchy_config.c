// swad_hierarchy_config.c: hierarchy (country, institution, centre, degree, course) configuration

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_logo.h"
#include "swad_QR.h"

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

void HieCfg_Title (bool PutLink,
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

void HieCfg_FullName (bool PutForm,const char *Label,Act_Action_t NextAction,
		      const char FullName[Hie_MAX_BYTES_FULL_NAME + 1])
  {
   /***** Full name *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",PutForm ? "FullName" :
	                           NULL,
		    Label);

   /* Data */
   HTM_TD_Begin ("class=\"DAT_N LB\"");
   if (PutForm)
     {
      /* Form to change full name */
      Frm_StartForm (NextAction);
      HTM_INPUT_TEXT ("FullName",Hie_MAX_CHARS_FULL_NAME,FullName,true,
		      "id=\"FullName\" class=\"INPUT_FULL_NAME\""
		      " required=\"required\"");
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

void HieCfg_ShrtName (bool PutForm,Act_Action_t NextAction,
		      const char ShrtName[Hie_MAX_BYTES_SHRT_NAME + 1])
  {
   extern const char *Txt_Short_name;

   /***** Short name *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",PutForm ? "ShortName" :
	                           NULL,
		    Txt_Short_name);

   /* Data */
   HTM_TD_Begin ("class=\"DAT_N LB\"");
   if (PutForm)
     {
      /* Form to change short name */
      Frm_StartForm (NextAction);
      HTM_INPUT_TEXT ("ShortName",Hie_MAX_CHARS_SHRT_NAME,ShrtName,true,
		      "id=\"ShortName\" class=\"INPUT_SHORT_NAME\""
		      " required=\"required\"");
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

void HieCfg_WWW (bool PrintView,bool PutForm,Act_Action_t NextAction,
		 const char WWW[Cns_MAX_BYTES_WWW + 1])
  {
   extern const char *Txt_Web;

   /***** Web *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",PutForm ? "WWW" :
	                           NULL,
		    Txt_Web);

   /* Data */
   HTM_TD_Begin ("class=\"DAT LB\"");
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
      if (!PrintView)
         HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"DAT\"",WWW);
      HTM_Txt (WWW);
      if (!PrintView)
         HTM_A_End ();
      HTM_DIV_End ();
     }
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/********************** Show shortcut in configuration ***********************/
/*****************************************************************************/

void HieCfg_Shortcut (bool PrintView,const char *ParamName,long HieCod)
  {
   extern const char *Txt_Shortcut;

   /***** Short cut *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_Shortcut);

   /* Data */
   HTM_TD_Begin ("class=\"DAT LB\"");
   if (!PrintView)
     {
      if (ParamName)
	 HTM_A_Begin ("href=\"%s/?%s=%ld\" class=\"DAT\" target=\"_blank\"",
		      Cfg_URL_SWAD_CGI,
		      ParamName,HieCod);
      else
	 HTM_A_Begin ("href=\"%s/\" class=\"DAT\" target=\"_blank\"",
		      Cfg_URL_SWAD_CGI);
     }
   if (ParamName)
      HTM_TxtF ("%s/?%s=%ld",
		Cfg_URL_SWAD_CGI,
		ParamName,HieCod);
   else
      HTM_TxtF ("%s/",
		Cfg_URL_SWAD_CGI);
   if (!PrintView)
      HTM_A_End ();
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************************** Show number of centres ***************************/
/*****************************************************************************/

void HieCfg_NumCtrs (unsigned NumCtrs,bool PutForm)
  {
   extern const char *Txt_Centres;
   extern const char *Txt_Centres_of_INSTITUTION_X;

   /***** Number of centres *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_Centres);

   /* Data */
   HTM_TD_Begin ("class=\"LB\"");
   if (PutForm)
     {
      Frm_StartFormGoTo (ActSeeCtr);
      Ins_PutParamInsCod (Gbl.Hierarchy.Ins.InsCod);
      HTM_BUTTON_SUBMIT_Begin (Str_BuildStringStr (Txt_Centres_of_INSTITUTION_X,
						   Gbl.Hierarchy.Ins.ShrtName),
			       "BT_LINK DAT",NULL);
      Str_FreeString ();
     }
   HTM_Unsigned (NumCtrs);
   if (PutForm)
     {
      HTM_BUTTON_End ();
      Frm_EndForm ();
     }
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/********************* Show number of centres with map ***********************/
/*****************************************************************************/

void HieCfg_NumCtrsWithMap (unsigned NumCtrs,unsigned NumCtrsWithMap)
  {
   extern const char *Txt_Centres_with_map;

   /***** Number of centres with map *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_Centres_with_map);

   /* Data */
   HTM_TD_Begin ("class=\"DAT LB\"");
   HTM_TxtF ("%u (%.1lf%%)",
	     NumCtrsWithMap,
	     NumCtrs ? (double) NumCtrsWithMap * 100.0 /
		       (double) NumCtrs :
		       0.0);
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************************* Show QR in configuration **************************/
/*****************************************************************************/

void HieCfg_QR (const char *ParamName,long HieCod)
  {
   extern const char *Txt_QR_code;

   /***** QR *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_QR_code);

   /* Data */
   HTM_TD_Begin ("class=\"DAT LB\"");
   QR_LinkTo (250,ParamName,HieCod);
   HTM_TD_End ();

   HTM_TR_End ();
  }
