// swad_menu.c: menu (horizontal or vertical) selection

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

#include "swad_box.h"
#include "swad_database.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_layout.h"
#include "swad_menu.h"
#include "swad_parameter.h"
#include "swad_setting.h"
#include "swad_tab.h"

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
		[ 2] = ActSeeSocTmlGbl,
		[ 3] = ActSeeSocPrf,
		[ 4] = ActSeeCal,
		[ 5] = ActSeeNtf,
	       },
   [TabSys] =  {
		[ 0] = ActSeeSysInf,
		[ 1] = ActSeeCty,
		[ 2] = ActSeePen,
		[ 3] = ActSeeLnk,
		[ 4] = ActLstPlg,
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
		[ 7] = ActSeeCrsTT,
	       },
   [TabAss] =  {
		[ 0] = ActSeeAss,
		[ 1] = ActSeeAsg,
		[ 2] = ActSeePrj,
		[ 3] = ActSeeAllExaAnn,
		[ 4] = ActEdiTstQst,
		[ 5] = ActReqTst,
		[ 6] = ActSeeAllExa,
		[ 7] = ActSeeAllGam,
		[ 8] = ActSeeAllSvy,
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
		[ 4] = ActSeeAtt,
		[ 5] = ActReqSignUp,
		[ 6] = ActSeeSignUpReq,
		[ 7] = ActLstCon,
		},
   [TabMsg] =  {
		[ 0] = ActSeeAnn,
		[ 1] = ActSeeAllNot,
		[ 2] = ActSeeFor,
		[ 3] = ActSeeChtRms,
		[ 4] = ActSeeMsg,
		[ 5] = ActReqMsgUsr,
		[ 6] = ActSeeRcvMsg,
		[ 7] = ActSeeSntMsg,
		[ 8] = ActReqMaiUsr,
	       },
   [TabAna] =  {
		[ 0] = ActReqUseGbl,
		[ 1] = ActSeePhoDeg,
		[ 2] = ActReqStaCrs,
		[ 3] = ActReqAccGbl,
		[ 4] = ActReqMyUsgRep,
		[ 5] = ActMFUAct,
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
   extern const char *The_ClassTxtMenu[The_NUM_THEMES];
   extern const char *Txt_MENU_TITLE[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];
   unsigned NumOptInMenu;
   Act_Action_t NumAct;
   const char *Title;
   bool IsTheSelectedAction;

   /***** Menu start *****/
   HTM_Txt ("<nav class=\"MENU_LIST_CONT\">");
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

         Title = Act_GetSubtitleAction (NumAct);

         /***** Start element *****/
	 HTM_LI_Begin ("class=\"MENU_LIST_ITEM\"");

         /***** Begin form and link *****/
         Frm_StartForm (NumAct);
         HTM_BUTTON_SUBMIT_Begin (Title,
			          IsTheSelectedAction ? "BT_LINK MENU_OPT_ON" :
					                "BT_LINK MENU_OPT_OFF",
				  NULL);

         /***** Icon and text *****/
	 HTM_DIV_Begin ("class=\"MENU_ICO\" style=\"background-image:url('%s/%s');\"",
			Gbl.Prefs.URLIconSet,
			Act_GetIcon (NumAct));
	 HTM_DIV_Begin ("class=\"MENU_TEXT %s\"",The_ClassTxtMenu[Gbl.Prefs.Theme]);
	 HTM_Txt (Txt_MENU_TITLE[Gbl.Action.Tab][NumOptInMenu]);
	 HTM_DIV_End ();
	 HTM_DIV_End ();

         /***** End link and form *****/
         HTM_BUTTON_End ();
	 Frm_EndForm ();

         /***** End element *****/
         HTM_LI_End ();
        }
     }

   /***** Menu end *****/
   HTM_UL_End ();
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
   Set_StartSettingsHead ();
   Set_StartOneSettingSelector ();
   for (Menu  = (Mnu_Menu_t) 0;
	Menu <= (Mnu_Menu_t) (Mnu_NUM_MENUS - 1);
	Menu++)
     {
      HTM_DIV_Begin ("class=\"%s\"",Menu == Gbl.Prefs.Menu ? "PREF_ON" :
        	                                             "PREF_OFF");
      Frm_StartForm (ActChgMnu);
      Par_PutHiddenParamUnsigned (NULL,"Menu",(unsigned) Menu);
      Ico_PutSettingIconLink (Mnu_MenuIcons[Menu],Txt_MENU_NAMES[Menu]);
      Frm_EndForm ();
      HTM_DIV_End ();
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
   Gbl.Prefs.Menu = Mnu_GetParamMenu ();

   /***** Store menu in database *****/
   if (Gbl.Usrs.Me.Logged)
      DB_QueryUPDATE ("can not update your setting about menu",
		      "UPDATE usr_data SET Menu=%u WHERE UsrCod=%ld",
                      (unsigned) Gbl.Prefs.Menu,Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/************************* Get parameter with menu ***************************/
/*****************************************************************************/

Mnu_Menu_t Mnu_GetParamMenu (void)
  {
   return (Mnu_Menu_t)
	  Par_GetParToUnsignedLong ("Menu",
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
