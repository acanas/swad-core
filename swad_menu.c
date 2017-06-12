// swad_menu.c: menu (horizontal or vertical) selection

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#include <stdio.h>	// For fprintf, etc.

#include "swad_box.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_layout.h"
#include "swad_menu.h"
#include "swad_parameter.h"
#include "swad_preference.h"
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
   "horizontal",
   "vertical",
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
		},
		// TabSys *******************
		{
		ActSysReqSch,		//  0
		ActSeeCty,		//  1
		ActSeePen,		//  2
		ActSeeDegTyp,		//  3
		ActSeeLnk,		//  4
		ActLstPlg,		//  5
		ActSetUp,		//  6
		ActSeeCalSys,		//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		},
		// TabCty *******************
		{
		ActCtyReqSch,		//  0
		ActSeeCtyInf,		//  1
		ActSeeIns,		//  2
		ActSeeCalCty,		//  3
		0,			//  4
		0,			//  5
		0,			//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		},
		// TabIns *******************
		{
		ActInsReqSch,		//  0
		ActSeeInsInf,		//  1
		ActSeeCtr,		//  2
		ActSeeDpt,		//  3
		ActSeeCalIns,		//  4
		0,			//  5
		0,			//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		},
		// TabCtr *******************
		{
		ActCtrReqSch,		//  0
		ActSeeCtrInf,		//  1
		ActSeeDeg,		//  2
		ActSeeCalCtr,		//  3
		0,			//  4
		0,			//  5
		0,			//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		},
		// TabDeg *******************
		{
		ActDegReqSch,		//  0
		ActSeeDegInf,		//  1
		ActSeeCrs,		//  2
		ActSeeCalDeg,		//  3
		0,			//  4
		0,			//  5
		0,			//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		},
		// TabCrs *******************
		{
		ActCrsReqSch,		//  0
		ActSeeCrsInf,		//  1
		ActSeeTchGui,		//  2
		ActSeeSyl,		//  3
		ActSeeBib,		//  4
		ActSeeFAQ,		//  5
		ActSeeCrsLnk,		//  6
		ActSeeCrsTT,		//  7
		ActSeeCalCrs,		//  8
		0,			//  9
		0,			// 10
		0,			// 11
		},
		// TabAss *******************
		{
		ActSeeAss,		//  0
		ActSeeAsg,		//  1
		ActReqTst,		//  2
		ActSeeAllSvy,		//  3
		ActSeeAllExaAnn,	//  4
		0,			//  5
		0,			//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
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
		},
		// TabSoc *******************
		{
		ActSeeSocTmlGbl,	//  0
		ActSeeSocPrf,		//  1
		ActSeeFor,		//  2
		ActSeeChtRms,		//  3
		0,			//  4
		0,			//  5
		0,			//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		},
		// TabMsg *******************
		{
		ActSeeNtf,		//  0
		ActSeeAnn,		//  1
		ActSeeAllNot,		//  2
		ActReqMsgUsr,		//  3
		ActSeeRcvMsg,		//  4
		ActSeeSntMsg,		//  5
		ActMaiStd,		//  6
		0,			//  7
		0,			//  8
		0,			//  9
		0,			// 10
		0,			// 11
		},
		// TabSta *******************
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
		},
		// TabPrf *******************
		{
		ActFrmLogIn,		//  0
		ActFrmRolSes,		//  1
		ActMyCrs,		//  2
		ActSeeMyTT,		//  3
		ActSeeMyAgd,		//  4
		ActFrmMyAcc,		//  5
		ActReqEdiRecCom,	//  6
		ActEdiPrf,		//  7
		ActAdmBrf,		//  8
		0,			//  9
		0,			// 10
		0,			// 11
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
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   extern const char *The_ClassTxtMenuOn[The_NUM_THEMES];
   extern const char *The_ClassTxtMenuOff[The_NUM_THEMES];
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
         IsTheSelectedAction = (NumAct == Act_Actions[Gbl.Action.Act].SuperAction);

         Title = Act_GetSubtitleAction (NumAct);

         /***** Start element *****/
	 fprintf (Gbl.F.Out,"<li class=\"MENU_LIST_ITEM\">");

         /***** Start container used to highlight this option *****/
         fprintf (Gbl.F.Out,"<div class=\"%s\">",
                  IsTheSelectedAction ? "MENU_OPT_ON" :
                	                "MENU_OPT_OFF");

         /***** Start form and link *****/
         Act_FormStart (NumAct);
         Act_LinkFormSubmit (Title,
                             IsTheSelectedAction ? The_ClassTxtMenuOn[Gbl.Prefs.Theme] :
                                                   The_ClassTxtMenuOff[Gbl.Prefs.Theme],NULL);

         /***** Icon *****/
	 fprintf (Gbl.F.Out,"<div class=\"MENU_ICO\""
			    " style=\"background-image:url('%s/%s/%s');\">",
	          Gbl.Prefs.PathIconSet,Cfg_ICON_ACTION,
                  Act_Actions[NumAct].Icon);

         /***** Text *****/
	 fprintf (Gbl.F.Out,"<div class=\"MENU_TEXT %s\">"
	                    "%s"
	                    "</div>",
		  IsTheSelectedAction ? The_ClassTxtMenuOn[Gbl.Prefs.Theme] :
                                        The_ClassTxtMenuOff[Gbl.Prefs.Theme],
		  Txt_MENU_TITLE[Gbl.Action.Tab][NumOptInMenu]);

         /***** End link and form *****/
         fprintf (Gbl.F.Out,"</div>"
	                    "</a>");
	 Act_FormEnd ();

         /***** End container used to highlight this option *****/
         fprintf (Gbl.F.Out,"</div>");

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
   extern const char *Hlp_PROFILE_Preferences_menu;
   extern const char *Txt_Menu;
   extern const char *Txt_MENU_NAMES[Mnu_NUM_MENUS];
   Mnu_Menu_t Menu;

   Box_StartBox (NULL,Txt_Menu,Mnu_PutIconsMenu,
                 Hlp_PROFILE_Preferences_menu,
                 false);	// Not closable
   fprintf (Gbl.F.Out,"<div class=\"PREF_CONTAINER\">");
   for (Menu = (Mnu_Menu_t) 0;
	Menu < Mnu_NUM_MENUS;
	Menu++)
     {
      fprintf (Gbl.F.Out,"<div class=\"%s\">",
               Menu == Gbl.Prefs.Menu ? "PREF_ON" :
        	                        "PREF_OFF");
      Act_FormStart (ActChgMnu);
      Par_PutHiddenParamUnsigned ("Menu",(unsigned) Menu);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s32x32.gif\""
	                 " alt=\"%s\" title=\"%s\" class=\"ICO25x25B\" />",
               Gbl.Prefs.IconsURL,
               Mnu_MenuIcons[Menu],
               Txt_MENU_NAMES[Menu],
               Txt_MENU_NAMES[Menu]);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");
     }
   fprintf (Gbl.F.Out,"</div>");
   Box_EndBox ();
  }

/*****************************************************************************/
/****************** Put contextual icons in menu preference ******************/
/*****************************************************************************/

static void Mnu_PutIconsMenu (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_MENUS;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/******************************** Change menu ********************************/
/*****************************************************************************/

void Mnu_ChangeMenu (void)
  {
   char Query[512];

   /***** Get param with menu *****/
   Gbl.Prefs.Menu = Mnu_GetParamMenu ();

   /***** Store menu in database *****/
   if (Gbl.Usrs.Me.Logged)
     {
      sprintf (Query,"UPDATE usr_data SET Menu=%u WHERE UsrCod=%ld",
               (unsigned) Gbl.Prefs.Menu,Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryUPDATE (Query,"can not update your preference about menu");
     }

   /***** Set preferences from current IP *****/
   Pre_SetPrefsFromIP ();
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
