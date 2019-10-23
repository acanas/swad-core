// swad_menu.c: menu (horizontal or vertical) selection

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

#include <stdio.h>		// For fprintf, etc.

#include "swad_box.h"
#include "swad_database.h"
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
/****************************** Private constants ****************************/
/*****************************************************************************/

#define MAX_MENU_ID 16

const char *Mnu_MenuId[Mnu_NUM_MENUS] =
  {
   "horizontal",
   "vertical",
  };

const char *Mnu_MenuNames[Mnu_NUM_MENUS] =
  {
   "Horizontal",
   "Vertical",
  };

const char *Mnu_MenuIcons[Mnu_NUM_MENUS] =
  {
   "ellipsis-h.svg",
   "ellipsis-v.svg",
  };

// Actions not initialized are 0 by default
const Act_Action_t Mnu_MenuActions[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB] =
	{
		// TabUnk *******************
		{
		0,			//  0
		0,			//  1
		0,			//  2
		0,			//  3
		0,			//  4
		0,			//  5
		0,			//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		0,			// 12
		},
		// TabStr *******************
		{
		ActFrmLogIn,		//  0
		ActReqSch,		//  1
		ActSeeSocTmlGbl,	//  2
		ActSeeSocPrf,		//  3
		ActSeeCal,		//  4
		ActSeeNtf,		//  5
		0,			//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		0,			// 12
		},
		// TabSys *******************
		{
		ActSeeCty,		//  0
		ActSeePen,		//  1
		ActSeeLnk,		//  2
		ActLstPlg,		//  3
		ActSetUp,		//  4
		0,			//  5
		0,			//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		0,			// 12
		},
		// TabCty *******************
		{
		ActSeeCtyInf,		//  0
		ActSeeIns,		//  1
		0,			//  2
		0,			//  3
		0,			//  4
		0,			//  5
		0,			//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		0,			// 12
		},
		// TabIns *******************
		{
		ActSeeInsInf,		//  0
		ActSeeCtr,		//  1
		ActSeePlc,		//  2
		ActSeeDpt,		//  3
		ActSeeHld,		//  4
		0,			//  5
		0,			//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		0,			// 12
		},
		// TabCtr *******************
		{
		ActSeeCtrInf,		//  0
		ActSeeDeg,		//  1
		ActSeeCla,		//  2
		0,			//  3
		0,			//  4
		0,			//  5
		0,			//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		0,			// 12
		},
		// TabDeg *******************
		{
		ActSeeDegInf,		//  0
		ActSeeCrs,		//  1
		0,			//  2
		0,			//  3
		0,			//  4
		0,			//  5
		0,			//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		0,			// 12
		},
		// TabCrs *******************
		{
		ActSeeCrsInf,		//  0
		ActSeeTchGui,		//  1
		ActSeeSyl,		//  2
		ActSeeBib,		//  3
		ActSeeFAQ,		//  4
		ActSeeCrsLnk,		//  5
		ActSeeCrsTT,		//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		0,			// 12
		},
		// TabAss *******************
		{
		ActSeeAss,		//  0
		ActSeeAsg,		//  1
		ActSeePrj,		//  2
		ActReqTst,		//  3
		ActSeeAllGam,		//  4
		ActSeeAllSvy,		//  5
		ActSeeAllExaAnn,	//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		0,			// 12
		},
		// TabFil *******************
		{
		ActSeeAdmDocIns,	//  0
		ActAdmShaIns,		//  1
		ActSeeAdmDocCtr,	//  2
		ActAdmShaCtr,		//  3
		ActSeeAdmDocDeg,	//  4
		ActAdmShaDeg,		//  5
		ActSeeAdmDocCrsGrp,	//  6
		ActAdmTchCrsGrp,	//  7
		ActAdmShaCrsGrp,	//  8
		ActAdmAsgWrkUsr,	//  9
		ActReqAsgWrkCrs,	// 10
		ActSeeAdmMrk,		// 11
		ActAdmBrf,		// 12
		},
		// TabUsr *******************
		{
		ActReqSelGrp,		//  0
		ActLstStd,		//  1
		ActLstTch,		//  2
		ActLstOth,		//  3
		ActSeeAtt,		//  4
		ActReqSignUp,		//  5
		ActSeeSignUpReq,	//  6
		ActLstCon,		//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		0,			// 12
		},
		// TabMsg *******************
		{
		ActSeeAnn,		//  0
		ActSeeAllNot,		//  1
		ActSeeFor,		//  2
		ActSeeChtRms,		//  3
		ActReqMsgUsr,		//  4
		ActSeeRcvMsg,		//  5
		ActSeeSntMsg,		//  6
		ActReqMaiUsr,		//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		0,			// 12
		},
		// TabAna *******************
		{
		ActReqUseGbl,		//  0
		ActSeePhoDeg,		//  1
		ActReqStaCrs,		//  2
		ActReqAccGbl,		//  3
		ActReqMyUsgRep,		//  4
		ActMFUAct,		//  5
		0,			//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		0,			// 12
		},
		// TabPrf *******************
		{
		ActFrmRolSes,		//  0
		ActMyCrs,		//  1
		ActSeeMyTT,		//  2
		ActSeeMyAgd,		//  3
		ActFrmMyAcc,		//  4
		ActReqEdiRecSha,	//  5
		ActReqEdiSet,		//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		0,			// 12
		},
	};

/*****************************************************************************/
/****************************** Private prototypes ***************************/
/*****************************************************************************/

static void Mnu_PutIconsMenu (void);

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
   fprintf (Gbl.F.Out,"<nav class=\"MENU_LIST_CONTAINER\">"
	              "<ul class=\"MENU_LIST\">");

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
	 fprintf (Gbl.F.Out,"<li class=\"MENU_LIST_ITEM\">");

         /***** Start container used to highlight this option *****/
         HTM_DIV_Begin ("class=\"%s\"",
			IsTheSelectedAction ? "MENU_OPT_ON" :
					      "MENU_OPT_OFF");

         /***** Begin form and link *****/
         Frm_StartForm (NumAct);
         Frm_LinkFormSubmit (Title,The_ClassTxtMenu[Gbl.Prefs.Theme],NULL);

         /***** Icon *****/
	 HTM_DIV_Begin ("class=\"MENU_ICO\" style=\"background-image:url('%s/%s');\"",
			Gbl.Prefs.URLIconSet,
			Act_GetIcon (NumAct));

         /***** Text *****/
	 HTM_DIV_Begin ("class=\"MENU_TEXT %s\"",The_ClassTxtMenu[Gbl.Prefs.Theme]);
	 fprintf (Gbl.F.Out,"%s",Txt_MENU_TITLE[Gbl.Action.Tab][NumOptInMenu]);
	 HTM_DIV_End ();

         /***** End link and form *****/
	 HTM_DIV_End ();
         fprintf (Gbl.F.Out,"</a>");
	 Frm_EndForm ();

         /***** End container used to highlight this option *****/
         HTM_DIV_End ();

         /***** End element *****/
         fprintf (Gbl.F.Out,"</li>");
        }
     }

   /***** Menu end *****/
   fprintf (Gbl.F.Out,"</ul>"
	              "</nav>");
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

   Box_StartBox (NULL,Txt_Menu,Mnu_PutIconsMenu,
                 Hlp_PROFILE_Settings_menu,Box_NOT_CLOSABLE);
   Set_StartSettingsHead ();
   Set_StartOneSettingSelector ();
   for (Menu = (Mnu_Menu_t) 0;
	Menu < Mnu_NUM_MENUS;
	Menu++)
     {
      HTM_DIV_Begin ("class=\"%s\"",Menu == Gbl.Prefs.Menu ? "PREF_ON" :
        	                                             "PREF_OFF");
      Frm_StartForm (ActChgMnu);
      Par_PutHiddenParamUnsigned ("Menu",(unsigned) Menu);
      Ico_PutSettingIconLink (Mnu_MenuIcons[Menu],Txt_MENU_NAMES[Menu]);
      Frm_EndForm ();
      HTM_DIV_End ();
     }
   Set_EndOneSettingSelector ();
   Set_EndSettingsHead ();
   Box_EndBox ();
  }

/*****************************************************************************/
/******************* Put contextual icons in menu setting ********************/
/*****************************************************************************/

static void Mnu_PutIconsMenu (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_MENUS;
   Fig_PutIconToShowFigure ();
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
