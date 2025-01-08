// swad_hierarchy_config.c: hierarchy (country, institution, center, degree, course) configuration

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_action_list.h"
#include "swad_error.h"
#include "swad_figure_cache.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy_config.h"
#include "swad_HTML.h"
#include "swad_logo.h"
#include "swad_QR.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************ Show title in configuration ************************/
/*****************************************************************************/

void HieCfg_Title (Hie_PutLink_t PutLink,Hie_Level_t Level)
  {
   Hie_Level_t LevelLogo = (Level == Hie_CRS) ? Hie_DEG :
						Level;

   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"FRAME_TITLE FRAME_TITLE_BIG FRAME_TITLE_%s\"",
                  The_GetSuffix ());

      /* Begin link */
      if (PutLink == Hie_PUT_LINK)
	 HTM_A_Begin ("href=\"%s\" target=\"_blank\" title=\"%s\""
		      " class=\"FRAME_TITLE_BIG FRAME_TITLE_%s\"",
		      Gbl.Hierarchy.Node[LevelLogo].WWW,
		      Gbl.Hierarchy.Node[LevelLogo].FullName,
		      The_GetSuffix ());

      /* Logo and name */
      Lgo_DrawLogo (LevelLogo,&Gbl.Hierarchy.Node[LevelLogo],"ICO64x64");
      HTM_BR ();
      HTM_Txt (Gbl.Hierarchy.Node[Level].FullName);

      /* End link */
      if (PutLink == Hie_PUT_LINK)
	 HTM_A_End ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************* Show short/full name in configuration *******************/
/*****************************************************************************/

void HieCfg_Name (Frm_PutForm_t PutForm,Hie_Level_t Level,
		  Nam_ShrtOrFullName_t ShrtOrFull)
  {
   extern const char *Nam_Params[Nam_NUM_SHRT_FULL_NAMES];
   extern unsigned Nam_MaxChars[Nam_NUM_SHRT_FULL_NAMES];
   extern const char *Txt_Short_name;
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   static Act_Action_t Action[Hie_NUM_LEVELS][Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Hie_INS][Nam_SHRT_NAME] = ActRenInsShoCfg,
      [Hie_INS][Nam_FULL_NAME] = ActRenInsFulCfg,
      [Hie_CTR][Nam_SHRT_NAME] = ActRenCtrShoCfg,
      [Hie_CTR][Nam_FULL_NAME] = ActRenCtrFulCfg,
      [Hie_DEG][Nam_SHRT_NAME] = ActRenDegShoCfg,
      [Hie_DEG][Nam_FULL_NAME] = ActRenDegFulCfg,
      [Hie_CRS][Nam_SHRT_NAME] = ActRenCrsShoCfg,
      [Hie_CRS][Nam_FULL_NAME] = ActRenCrsFulCfg,
     };
   const char *Id[Frm_NUM_PUT_FORM] =
     {
      [Frm_DONT_PUT_FORM] = NULL,
      [Frm_PUT_FORM     ] = Nam_Params[ShrtOrFull],
     };
   const char *Name[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = Gbl.Hierarchy.Node[Level].ShrtName,
      [Nam_FULL_NAME] = Gbl.Hierarchy.Node[Level].FullName,
     };

   /***** Full name *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",Id[PutForm],
		       ShrtOrFull == Nam_SHRT_NAME ? Txt_Short_name :
						     Txt_HIERARCHY_SINGUL_Abc[Level]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
         switch (PutForm)
           {
            case Frm_DONT_PUT_FORM:	// I can not edit full name
               HTM_Txt (Name[ShrtOrFull]);
               break;
            case Frm_PUT_FORM:
	       /* Form to change full name */
	       Frm_BeginForm (Action[Level][ShrtOrFull]);
		  HTM_INPUT_TEXT (Nam_Params[ShrtOrFull],
				  Nam_MaxChars[ShrtOrFull],
				  Name[ShrtOrFull],
				  HTM_REQUIRED | HTM_SUBMIT_ON_CHANGE,
				  "id=\"%s\" class=\"Frm_C2_INPUT INPUT_%s\"",
				  Nam_Params[ShrtOrFull],
				  The_GetSuffix ());
	       Frm_EndForm ();
               break;
           }
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************************* Show web in configuration *************************/
/*****************************************************************************/

void HieCfg_WWW (Vie_ViewType_t ViewType,Frm_PutForm_t PutForm,
		 Act_Action_t NextAction,const char WWW[WWW_MAX_BYTES_WWW + 1])
  {
   extern const char *Txt_Web;
   static const char *Id[Frm_NUM_PUT_FORM] =
     {
      [Frm_DONT_PUT_FORM] = NULL,
      [Frm_PUT_FORM     ] = "WWW",
     };

   /***** Web *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",Id[PutForm],Txt_Web);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
         switch (PutForm)
           {
            case Frm_DONT_PUT_FORM:	// I can not change web
	       HTM_DIV_Begin ("class=\"EXTERNAL_WWW_FULL\"");
		  if (ViewType == Vie_VIEW)
		     HTM_A_Begin ("href=\"%s\" target=\"_blank\""
				  " class=\"DAT_%s\"",WWW,The_GetSuffix ());
		  HTM_Txt (WWW);
		  if (ViewType == Vie_VIEW)
		     HTM_A_End ();
	       HTM_DIV_End ();
               break;
            case Frm_PUT_FORM:
	       /* Form to change web */
	       Frm_BeginForm (NextAction);
		  HTM_INPUT_URL ("WWW",WWW,
				 HTM_REQUIRED | HTM_SUBMIT_ON_CHANGE,
				 "id=\"WWW\" class=\"Frm_C2_INPUT INPUT_%s\"",
				 The_GetSuffix ());
	       Frm_EndForm ();
               break;
           }
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/********************** Show shortcut in configuration ***********************/
/*****************************************************************************/

void HieCfg_Shortcut (Vie_ViewType_t ViewType,ParCod_Param_t ParCode,long HieCod)
  {
   extern const char *Par_CodeStr[Par_NUM_PAR_COD];
   extern const char *Txt_Shortcut;

   /***** Short cut *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_Shortcut);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 if (ViewType == Vie_VIEW)
	   {
	    if (ParCode == ParCod_None)
	       HTM_A_Begin ("href=\"%s/\" target=\"_blank\" class=\"DAT_%s\"",
			    Cfg_URL_SWAD_CGI,The_GetSuffix ());
	    else
	       HTM_A_Begin ("href=\"%s/?%s=%ld\" target=\"_blank\""
			    " class=\"DAT_%s\"",
			    Cfg_URL_SWAD_CGI,
			    Par_CodeStr[ParCode],HieCod,The_GetSuffix ());
	   }
	 if (ParCode == ParCod_None)
	    HTM_TxtF ("%s/",Cfg_URL_SWAD_CGI);
	 else
	    HTM_TxtF ("%s/?%s=%ld",Cfg_URL_SWAD_CGI,Par_CodeStr[ParCode],HieCod);
	 if (ViewType == Vie_VIEW)
	    HTM_A_End ();
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************************** Show number of centers ***************************/
/*****************************************************************************/

void HieCfg_NumCtrs (unsigned NumCtrs,Frm_PutForm_t PutForm)
  {
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Centers_of_INSTITUTION_X;
   char *Title;

   /***** Number of centers *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_HIERARCHY_PLURAL_Abc[Hie_CTR]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 if (PutForm == Frm_PUT_FORM)
	   {
	    Frm_BeginFormGoTo (ActSeeCtr);
	       ParCod_PutPar (ParCod_Ins,Gbl.Hierarchy.Node[Hie_INS].HieCod);
	       if (asprintf (&Title,Txt_Centers_of_INSTITUTION_X,
	                     Gbl.Hierarchy.Node[Hie_INS].ShrtName) < 0)
		  Err_NotEnoughMemoryExit ();
	       HTM_BUTTON_Submit_Begin (Title,"class=\"LB BT_LINK\"");
	       free (Title);
	   }
	 HTM_Unsigned (NumCtrs);
	 if (PutForm == Frm_PUT_FORM)
	   {
	       HTM_BUTTON_End ();
	    Frm_EndForm ();
	   }
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/********************* Show number of centers with map ***********************/
/*****************************************************************************/

void HieCfg_NumCtrsWithMap (unsigned NumCtrs,unsigned NumCtrsWithMap)
  {
   extern const char *Txt_Centers_with_map;

   /***** Number of centers with map *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_Centers_with_map);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
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

void HieCfg_QR (ParCod_Param_t ParCode,long HieCod)
  {
   extern const char *Txt_QR_code;

   /***** QR *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",NULL,Txt_QR_code);

      /* Data */
      HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	 QR_LinkTo (250,ParCode,HieCod);
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************************ Number of users in courses *************************/
/*****************************************************************************/

void HieCfg_NumUsrsInCrss (Hie_Level_t Level,long Cod,Rol_Role_t Role)
  {
   extern const char *Txt_Users_in_courses;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   /***** Number of users in courses *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,
		       Role == Rol_UNK ? Txt_Users_in_courses :
					 Txt_ROLES_PLURAL_Abc[Role][Usr_SEX_UNKNOWN]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Enr_GetCachedNumUsrsInCrss (Level,Cod,
						   Role == Rol_UNK ? (1 << Rol_STD) |
								     (1 << Rol_NET) |
								     (1 << Rol_TCH) :	// Any user
								     (1 << Role)));
      HTM_TD_End ();

   HTM_TR_End ();
  }
