// swad_menu.c: menu (horizontal or vertical) selection

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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
extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];

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
		// TabUnk **********
		{
		},
		// TabSys **********
		{
		ActSysReqSch,
		ActSeeCty,
		ActSeePen,
		ActReqRemOldCrs,
		ActSeeDegTyp,
		ActSeeMai,
		ActSeeBan,
		ActSeeLnk,
		ActLstPlg,
		ActSetUp,
		},
		// TabCty **********
		{
		ActCtyReqSch,
		ActSeeCtyInf,
		ActSeeIns,
		},
		// TabIns **********
		{
		ActInsReqSch,
		ActSeeInsInf,
		ActSeeCtr,
		ActSeeDpt,
		ActSeePlc,
		ActSeeHld,
		ActSeeAdmDocIns,
		ActAdmShaIns,
		},
		// TabCtr **********
		{
		ActCtrReqSch,
		ActSeeCtrInf,
		ActSeeDeg,
		ActSeeAdmDocCtr,
		ActAdmShaCtr,
		},
		// TabDeg **********
		{
		ActDegReqSch,
		ActSeeDegInf,
		ActSeeCrs,
		ActSeeAdmDocDeg,
		ActAdmShaDeg,
		},
		// TabCrs **********
		{
		ActCrsReqSch,
		ActSeeCrsInf,
		ActSeeTchGui,
		ActSeeSyl,
		ActSeeAdmDocCrsGrp,
		ActAdmShaCrsGrp,
		ActSeeCrsTT,
		ActSeeBib,
		ActSeeFAQ,
		ActSeeCrsLnk,
		},
		// TabAss **********
		{
		ActSeeAss,
		ActSeeAsg,
		ActAdmAsgWrkUsr,
		ActReqAsgWrkCrs,
		ActReqTst,
		ActSeeCal,
		ActSeeExaAnn,
		ActSeeAdmMrk,
		},
		// TabUsr **********
		{
		ActReqSelGrp,
		ActLstStd,
		ActLstTch,
		ActLstOth,
		ActSeeAtt,
		ActReqSignUp,
		ActSeeSignUpReq,
		ActLstCon,
		},
		// TabSoc **********
		{
		ActSeeSocTmlGbl,
		ActSeeSocPrf,
		ActSeeFor,
		ActSeeChtRms,
		},
		// TabMsg **********
		{
		ActSeeNtf,
		ActSeeAnn,
		ActSeeNot,
		ActReqMsgUsr,
		ActSeeRcvMsg,
		ActSeeSntMsg,
		ActMaiStd,
		},
		// TabSta **********
		{
		ActSeeAllSvy,
		ActReqUseGbl,
		ActSeePhoDeg,
		ActReqStaCrs,
		ActReqAccGbl,
		},
		// TabPrf **********
		{
		ActFrmLogIn,
		ActMyCrs,
		ActSeeMyTT,
		ActFrmUsrAcc,
		ActReqEdiRecCom,
		ActEdiPrf,
		ActAdmBrf,
		ActMFUAct,
		},
	};

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
   extern const char *The_ClassTxtMenuOn[The_NUM_THEMES];
   extern const char *The_ClassTxtMenuOff[The_NUM_THEMES];
   extern const char *Txt_MENU_TITLE[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];
   unsigned NumOptInMenu;
   Act_Action_t NumAct;
   const char *Title;
   bool IsTheSelectedAction;

   /***** Menu start *****/
   fprintf (Gbl.F.Out,"<div class=\"MENU_LIST_CONTAINER\">"
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

         /***** Start of element *****/
	 fprintf (Gbl.F.Out,"<li class=\"MENU_LIST_ITEM\">");

         /***** Start of container used to highlight this option *****/
         fprintf (Gbl.F.Out,"<div class=\"%s\">",
                  IsTheSelectedAction ? "MENU_OPT_ON" :
                	                "MENU_OPT_OFF");

         /***** Start of form and link *****/
         Act_FormStart (NumAct);
         Act_LinkFormSubmit (Title,
                             IsTheSelectedAction ? The_ClassTxtMenuOn[Gbl.Prefs.Theme] :
                                                   The_ClassTxtMenuOff[Gbl.Prefs.Theme]);

         /***** Icon *****/
	 fprintf (Gbl.F.Out,"<div class=\"MENU_ICON\""
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

         /***** End of link and form *****/
         fprintf (Gbl.F.Out,"</div>"
	                    "</a>");
	 Act_FormEnd ();

         /***** End of container used to highlight this option *****/
         fprintf (Gbl.F.Out,"</div>");

         /***** End of element *****/
         fprintf (Gbl.F.Out,"</li>");
        }
     }

   /***** Menu end *****/
   fprintf (Gbl.F.Out,"</ul>"
	              "</div>");
  }

/*****************************************************************************/
/************* Put icons to select menu (horizontal or vertical) *************/
/*****************************************************************************/

void Mnu_PutIconsToSelectMenu (void)
  {
   extern const char *Txt_Menu;
   extern const char *Txt_MENU_NAMES[Mnu_NUM_MENUS];
   Mnu_Menu_t Menu;

   Lay_StartRoundFrameTable (NULL,0,Txt_Menu);
   fprintf (Gbl.F.Out,"<tr>");
   for (Menu = (Mnu_Menu_t) 0;
	Menu < Mnu_NUM_MENUS;
	Menu++)
     {
      fprintf (Gbl.F.Out,"<td class=\"%s\">",
               Menu == Gbl.Prefs.Menu ? "PREF_ON" :
        	                        "PREF_OFF");
      Act_FormStart (ActChgMnu);
      Par_PutHiddenParamUnsigned ("Menu",(unsigned) Menu);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s32x32.gif\""
	                 " alt=\"%s\" title=\"%s\" class=\"ICON40x40B\""
	                 " style=\"margin:0 auto;\" />",
               Gbl.Prefs.IconsURL,
               Mnu_MenuIcons[Menu],
               Txt_MENU_NAMES[Menu],
               Txt_MENU_NAMES[Menu]);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");
     }
   fprintf (Gbl.F.Out,"</tr>");
   Lay_EndRoundFrameTable ();
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
      sprintf (Query,"UPDATE usr_data SET Menu='%u' WHERE UsrCod='%ld'",
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
   char UnsignedStr[1+10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("Menu",UnsignedStr,1+10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Mnu_NUM_MENUS)
         return (Mnu_Menu_t) UnsignedNum;

   return Mnu_MENU_DEFAULT;
  }
