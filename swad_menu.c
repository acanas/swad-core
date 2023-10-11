// swad_menu.c: menu (horizontal or vertical) selection

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
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_layout.h"
#include "swad_menu.h"
#include "swad_parameter.h"
#include "swad_setting.h"
#include "swad_setting_database.h"
#include "swad_tab.h"
#include "swad_user_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Public constants ****************************/
/*****************************************************************************/

const char *Mnu_MenuIcons[Mnu_NUM_MENUS] =
  {
   [Mnu_MENU_HORIZONTAL] = "ellipsis-h.svg",
   [Mnu_MENU_VERTICAL  ] = "ellipsis-v.svg",
  };

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

#define MAX_MENU_ID 16

// Actions not initialized are 0 by default
static const Act_Action_t Mnu_MenuActions[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB] =
  {
   [TabUnk] =  {
	       },
   [TabStr] =  {
		[ 0] = ActFrmLogIn,
		[ 1] = ActReqSch,
		[ 2] = ActSeeGblTL,
		[ 3] = ActSeeSocPrf,
		[ 4] = ActSeeCal,
		[ 5] = ActSeeNtf,
	       },
   [TabSys] =  {
		[ 0] = ActSeeSysInf,
		[ 1] = ActSeeCty,
		[ 2] = ActSeePen,
		[ 3] = ActSeeLnk,
		[ 4] = ActSeePlg,
		[ 5] = ActMtn,
	       },
   [TabCty] =  {
		[ 0] = ActSeeCtyInf,
		[ 1] = ActSeeIns,
	       },
   [TabIns] =  {
		[ 0] = ActSeeInsInf,
		[ 1] = ActSeeCtr,
		[ 2] = ActSeePlc,
		[ 3] = ActSeeDpt,
		[ 4] = ActSeeHld,
	       },
   [TabCtr] =  {
		[ 0] = ActSeeCtrInf,
		[ 1] = ActSeeDeg,
		[ 2] = ActSeeBld,
		[ 3] = ActSeeRoo,
	       },
   [TabDeg] =  {
		[ 0] = ActSeeDegInf,
		[ 1] = ActSeeCrs,
	       },
   [TabCrs] =  {
		[ 0] = ActSeeCrsInf,
		[ 1] = ActSeePrg,
		[ 2] = ActSeeTchGui,
		[ 3] = ActSeeSyl,
		[ 4] = ActSeeBib,
		[ 5] = ActSeeFAQ,
		[ 6] = ActSeeCrsLnk,
		[ 7] = ActSeeAss,
		[ 8] = ActSeeCrsTT,
	       },
   [TabAss] =  {
		[ 0] = ActSeeAllAsg,
		[ 1] = ActSeeAllPrj,
		[ 2] = ActSeeAllCfe,
		[ 3] = ActEdiTstQst,
		[ 4] = ActReqTst,
		[ 5] = ActSeeAllExa,
		[ 6] = ActSeeAllGam,
		[ 7] = ActSeeAllRub,
	       },
   [TabFil] =  {
		[ 0] = ActSeeAdmDocIns,
		[ 1] = ActAdmShaIns,
		[ 2] = ActSeeAdmDocCtr,
		[ 3] = ActAdmShaCtr,
		[ 4] = ActSeeAdmDocDeg,
		[ 5] = ActAdmShaDeg,
		[ 6] = ActSeeAdmDocCrsGrp,
		[ 7] = ActAdmTchCrsGrp,
		[ 8] = ActAdmShaCrsGrp,
		[ 9] = ActAdmAsgWrkUsr,
		[10] = ActReqAsgWrkCrs,
		[11] = ActSeeAdmMrk,
		[12] = ActAdmBrf,
	       },
   [TabUsr] =  {
		[ 0] = ActReqSelGrp,
		[ 1] = ActLstStd,
		[ 2] = ActLstTch,
		[ 3] = ActLstOth,
		[ 4] = ActSeeAllAtt,
		[ 5] = ActReqSignUp,
		[ 6] = ActSeeSignUpReq,
		[ 7] = ActLstCon,
		},
   [TabMsg] =  {
		[ 0] = ActSeeAnn,
		[ 1] = ActSeeAllNot,
		[ 2] = ActSeeFor,
		[ 3] = ActSeeChtRms,
		[ 4] = ActSeeRcvMsg,
		[ 5] = ActReqMaiUsr,
	       },
   [TabAna] =  {
		[ 0] = ActReqUseGbl,
		[ 1] = ActSeePhoDeg,
		[ 2] = ActReqStaCrs,
		[ 3] = ActSeeAllSvy,
		[ 4] = ActReqAccGbl,
		[ 5] = ActReqMyUsgRep,
		[ 6] = ActMFUAct,
	       },
   [TabPrf] =  {
		[ 0] = ActFrmRolSes,
		[ 1] = ActMyCrs,
		[ 2] = ActSeeMyTT,
		[ 3] = ActSeeMyAgd,
		[ 4] = ActFrmMyAcc,
		[ 5] = ActReqEdiRecSha,
		[ 6] = ActReqEdiSet,
	       },
  };

/*****************************************************************************/
/****************************** Private prototypes ***************************/
/*****************************************************************************/

static void Mnu_PutIconsMenu (__attribute__((unused)) void *Args);

/*****************************************************************************/
/******* When I change to another tab, go to the first option allowed ********/
/*****************************************************************************/

Act_Action_t Mnu_GetFirstActionAvailableInCurrentTab (void)
  {
   unsigned NumOptInMenu;
   Act_Action_t Action;

   /* Change current action to the first allowed action in current tab */
   for (NumOptInMenu = 0;
        NumOptInMenu < Act_MAX_OPTIONS_IN_MENU_PER_TAB;
        NumOptInMenu++)
     {
      if ((Action = Mnu_MenuActions[Gbl.Action.Tab][NumOptInMenu]) == 0)
         return ActUnk;
      if (Act_CheckIfIHavePermissionToExecuteAction (Action))
         return Action;
     }
   return ActUnk;
  }

/*****************************************************************************/
/******************* Write horizontal menu of current tab ********************/
/*****************************************************************************/

void Mnu_WriteMenuThisTab (void)
  {
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   extern const char *Txt_MENU_TITLE[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];
   static const char *ClassIcoMenu[Ico_NUM_ICON_SETS] =
     {
      [Ico_ICON_SET_AWESOME] = "MENU_ICO",
      [Ico_ICON_SET_NUVOLA ] = NULL,
     };
   unsigned NumOptInMenu;
   Act_Action_t NumAct;
   const char *Title;
   bool IsTheSelectedAction;

   /***** Begin container *****/
   HTM_Txt ("<nav class=\"MENU_LIST_CONT\">");

      /***** Begin option list *****/
      HTM_UL_Begin ("class=\"MENU_LIST\"");

	 /***** Loop to write all options in menu. Each row holds an option *****/
	 for (NumOptInMenu = 0;
	      NumOptInMenu < Act_MAX_OPTIONS_IN_MENU_PER_TAB;
	      NumOptInMenu++)
	   {
	    NumAct = Mnu_MenuActions[Gbl.Action.Tab][NumOptInMenu];
	    if (NumAct == 0)  // At the end of each tab, actions are initialized to 0, so 0 marks the end of the menu
	       break;
	    if (Act_CheckIfIHavePermissionToExecuteAction (NumAct))
	      {
	       IsTheSelectedAction = (NumAct == Act_GetSuperAction (Gbl.Action.Act));

	       Title = Act_GetActionText (NumAct);

	       /***** Begin option *****/
	       HTM_LI_Begin ("class=\"MENU_LIST_ITEM %s\"",
	                     IsTheSelectedAction ? "MENU_OPT_ON" :
						   "MENU_OPT_OFF");

		  /***** Begin form *****/
		  Frm_BeginForm (NumAct);

		     /***** Begin link *****/
		     HTM_BUTTON_Submit_Begin (Title,"class=\"BT_LINK\"");

			/***** Icon and text *****/
			HTM_DIV_Begin ("class=\"MENU_ICO_TXT\"");
			   if (ClassIcoMenu[Gbl.Prefs.IconSet])
			      HTM_DIV_Begin ("class=\"MENU_ICO %s_%s\""
					     " style=\"background-image:url('%s/%s/%s');\"",
					     ClassIcoMenu[Gbl.Prefs.IconSet],The_GetSuffix (),
					     Cfg_URL_ICON_SETS_PUBLIC,Ico_IconSetId[Gbl.Prefs.IconSet],
					     Act_GetIcon (NumAct));
			   else
			      HTM_DIV_Begin ("class=\"MENU_ICO\""
					     " style=\"background-image:url('%s/%s/%s');\"",
					     Cfg_URL_ICON_SETS_PUBLIC,Ico_IconSetId[Gbl.Prefs.IconSet],
					     Act_GetIcon (NumAct));
			   HTM_DIV_End ();
			   HTM_DIV_Begin ("class=\"MENU_TXT MENU_TXT_%s\"",
			                  The_GetSuffix ());
			      HTM_Txt (Txt_MENU_TITLE[Gbl.Action.Tab][NumOptInMenu]);
			   HTM_DIV_End ();
			HTM_DIV_End ();

		     /***** End link *****/
		     HTM_BUTTON_End ();

		  /***** End form *****/
		  Frm_EndForm ();

	       /***** End option *****/
	       HTM_LI_End ();
	      }
	   }

      /***** End option list *****/
      HTM_UL_End ();

   /***** End container *****/
   HTM_Txt ("</nav>");
  }

/*****************************************************************************/
/************* Put icons to select menu (horizontal or vertical) *************/
/*****************************************************************************/

void Mnu_PutIconsToSelectMenu (void)
  {
   extern const char *Hlp_PROFILE_Settings_menu;
   extern const char *Txt_Menu;
   extern const char *Txt_MENU_NAMES[Mnu_NUM_MENUS];
   Mnu_Menu_t Menu;

   Box_BoxBegin (NULL,Txt_Menu,
                 Mnu_PutIconsMenu,NULL,
                 Hlp_PROFILE_Settings_menu,Box_NOT_CLOSABLE);
      Set_BeginSettingsHead ();
	 Set_BeginOneSettingSelector ();
	    for (Menu  = (Mnu_Menu_t) 0;
		 Menu <= (Mnu_Menu_t) (Mnu_NUM_MENUS - 1);
		 Menu++)
	      {
	       Set_BeginPref (Menu == Gbl.Prefs.Menu);
		  Frm_BeginForm (ActChgMnu);
		     Par_PutParUnsigned (NULL,"Menu",(unsigned) Menu);
		     Ico_PutSettingIconLink (Mnu_MenuIcons[Menu],Ico_BLACK,
					     Txt_MENU_NAMES[Menu]);
		  Frm_EndForm ();
	       Set_EndPref ();
	      }
	 Set_EndOneSettingSelector ();
      Set_EndSettingsHead ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************* Put contextual icons in menu setting ********************/
/*****************************************************************************/

static void Mnu_PutIconsMenu (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_MENUS);
  }

/*****************************************************************************/
/******************************** Change menu ********************************/
/*****************************************************************************/

void Mnu_ChangeMenu (void)
  {
   /***** Get param with menu *****/
   Gbl.Prefs.Menu = Mnu_GetParMenu ();

   /***** Store menu in database *****/
   if (Gbl.Usrs.Me.Logged)
      Set_DB_UpdateMySettingsAboutMenu (Gbl.Prefs.Menu);

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/************************* Get parameter with menu ***************************/
/*****************************************************************************/

Mnu_Menu_t Mnu_GetParMenu (void)
  {
   return (Mnu_Menu_t)
	  Par_GetParUnsignedLong ("Menu",
	                          0,
	                          Mnu_NUM_MENUS - 1,
	                          (unsigned long) Mnu_MENU_DEFAULT);
  }

/*****************************************************************************/
/*************************** Get menu from string ****************************/
/*****************************************************************************/

Mnu_Menu_t Mnu_GetMenuFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Mnu_NUM_MENUS)
         return (Mnu_Menu_t) UnsignedNum;

   return Mnu_MENU_DEFAULT;
  }

/*****************************************************************************/
/******************************** Context menu *******************************/
/*****************************************************************************/

void Mnu_ContextMenuBegin (void)
  {
   HTM_DIV_Begin ("class=\"CONTEXT_MENU\"");
  }

void Mnu_ContextMenuEnd (void)
  {
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*********** Get and show number of users who have chosen a menu *************/
/*****************************************************************************/

void Mnu_GetAndShowNumUsrsPerMenu (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_menu;
   extern const char *Mnu_MenuIcons[Mnu_NUM_MENUS];
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Menu;
   extern const char *Txt_Number_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Txt_MENU_NAMES[Mnu_NUM_MENUS];
   Mnu_Menu_t Menu;
   char *SubQuery;
   unsigned NumUsrs[Mnu_NUM_MENUS];
   unsigned NumUsrsTotal = 0;

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_FIGURE_TYPES[Fig_MENUS],
                      NULL,NULL,
                      Hlp_ANALYTICS_Figures_menu,Box_NOT_CLOSABLE,2);

      /***** Heading row *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Menu            ,HTM_HEAD_LEFT);
	 HTM_TH (Txt_Number_of_users ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_PERCENT_of_users,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** For each menu... *****/
      for (Menu  = (Mnu_Menu_t) 0;
	   Menu <= (Mnu_Menu_t) (Mnu_NUM_MENUS - 1);
	   Menu++)
	{
	 /* Get number of users who have chosen this menu from database */
	 if (asprintf (&SubQuery,"usr_data.Menu=%u",
		       (unsigned) Menu) < 0)
	    Err_NotEnoughMemoryExit ();
	 NumUsrs[Menu] = Usr_DB_GetNumUsrsWhoChoseAnOption (SubQuery);
	 free (SubQuery);

	 /* Update total number of users */
	 NumUsrsTotal += NumUsrs[Menu];
	}

      /***** Write number of users who have chosen each menu *****/
      for (Menu  = (Mnu_Menu_t) 0;
	   Menu <= (Mnu_Menu_t) (Mnu_NUM_MENUS - 1);
	   Menu++)
	{
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"CM\"");
	       Ico_PutIcon (Mnu_MenuIcons[Menu],Ico_BLACK,
	                    Txt_MENU_NAMES[Menu],"ICOx20");
	    HTM_TD_End ();

	    HTM_TD_Unsigned (NumUsrs[Menu]);
	    HTM_TD_Percentage (NumUsrs[Menu],NumUsrsTotal);

	 HTM_TR_End ();
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
