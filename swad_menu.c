// swad_menu.c: menu (horizontal or vertical) selection

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

/*****************************************************************************/
/************* Write the menu of current tab (desktop layout) ****************/
/*****************************************************************************/

void Mnu_WriteVerticalMenuThisTabDesktop (void)
  {
   extern const char *The_ClassMenuOn[The_NUM_THEMES];
   extern const char *The_ClassMenuOff[The_NUM_THEMES];
   extern const char *The_ClassSeparator[The_NUM_THEMES];
   extern const struct Act_Menu Act_Menu[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];
   extern const char *Txt_MENU_TITLE[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];
   unsigned NumOptInMenu;
   Act_Action_t NumAct;
   const char *Title;
   bool IsTheSelectedAction;
   bool SeparationBetweenPreviousAndCurrentOption = false;
   bool PreviousVisibleOptions = false;

   /***** List start *****/
   fprintf (Gbl.F.Out,"<ul id=\"vertical_menu_container\">");

   /***** Loop to write all options in menu. Each row holds an option *****/
   for (NumOptInMenu = 0;
        NumOptInMenu < Act_MAX_OPTIONS_IN_MENU_PER_TAB;
        NumOptInMenu++)
     {
      NumAct = Act_Menu[Gbl.CurrentTab][NumOptInMenu].Action;
      if (NumAct == 0)  // At the end of each tab, actions are initialized to 0, so 0 marks the end of the menu
         break;

      if (Act_CheckIfIHavePermissionToExecuteAction (NumAct))
        {
         IsTheSelectedAction = (NumAct == Act_Actions[Gbl.CurrentAct].SuperAction);

         Title = Act_GetSubtitleAction (NumAct);

         if (SeparationBetweenPreviousAndCurrentOption)
           {
            if (PreviousVisibleOptions)
               fprintf (Gbl.F.Out,"<li>"
        	                  "<hr class=\"%s\" />"
        	                  "</li>",
                        The_ClassSeparator[Gbl.Prefs.Theme]);
            SeparationBetweenPreviousAndCurrentOption = false;
           }

         /***** Start of element *****/
         fprintf (Gbl.F.Out,"<li>");

         /***** Start of container used to highlight this option *****/
         if (!IsTheSelectedAction)
            fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\">");

         /***** Start of form and link *****/
         Act_FormStart (NumAct);
         Act_LinkFormSubmit (Title,IsTheSelectedAction ? The_ClassMenuOn[Gbl.Prefs.Theme] :
                                                         The_ClassMenuOff[Gbl.Prefs.Theme]);

         /***** Icon *****/
	 fprintf (Gbl.F.Out,"<div class=\"MENU_OPTION\""
			    " style=\"background-image:url('%s/%s/%s64x64.gif');"
			    " background-size:48px 48px;\"\">",
	          Gbl.Prefs.PathIconSet,Cfg_ICON_ACTION,
                  Act_Actions[NumAct].Icon);

         /***** Text *****/
	 fprintf (Gbl.F.Out,"<div class=\"MENU_TEXT\">"
	                    "<span class=\"%s\">%s</span>",
		  IsTheSelectedAction ? The_ClassMenuOn[Gbl.Prefs.Theme] :
                                        The_ClassMenuOff[Gbl.Prefs.Theme],
		  Txt_MENU_TITLE[Gbl.CurrentTab][NumOptInMenu]);

         /***** End of link and form *****/
         fprintf (Gbl.F.Out,"</div>"
	                    "</div>"
	                    "</a>");
         Act_FormEnd ();

         /***** End of container used to highlight this option *****/
         if (!IsTheSelectedAction)
	    fprintf (Gbl.F.Out,"</div>");

         /***** End of element *****/
         fprintf (Gbl.F.Out,"</li>");

         PreviousVisibleOptions = true;
        }

      if (!SeparationBetweenPreviousAndCurrentOption)
         SeparationBetweenPreviousAndCurrentOption = Act_Menu[Gbl.CurrentTab][NumOptInMenu].SubsequentSeparation;
     }

   /***** List end *****/
   fprintf (Gbl.F.Out,"</ul>");
  }

/*****************************************************************************/
/********** Write horizontal menu of current tab (desktop layout) ************/
/*****************************************************************************/

void Mnu_WriteHorizontalMenuThisTabDesktop (void)
  {
   extern const char *The_ClassMenuOn[The_NUM_THEMES];
   extern const char *The_ClassMenuOff[The_NUM_THEMES];
   extern const struct Act_Menu Act_Menu[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];
   extern const char *Txt_MENU_TITLE[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];
   unsigned NumOptInMenu;
   Act_Action_t NumAct;
   const char *Title;
   bool IsTheSelectedAction;

   /***** List start *****/
   fprintf (Gbl.F.Out,"<div id=\"horizontal_menu_container\">"
                      "<ul>");

   /***** Loop to write all options in menu. Each row holds an option *****/
   for (NumOptInMenu = 0;
        NumOptInMenu < Act_MAX_OPTIONS_IN_MENU_PER_TAB;
        NumOptInMenu++)
     {
      NumAct = Act_Menu[Gbl.CurrentTab][NumOptInMenu].Action;
      if (NumAct == 0)  // At the end of each tab, actions are initialized to 0, so 0 marks the end of the menu
         break;
      if (Act_CheckIfIHavePermissionToExecuteAction (NumAct))
        {
         IsTheSelectedAction = (NumAct == Act_Actions[Gbl.CurrentAct].SuperAction);

         Title = Act_GetSubtitleAction (NumAct);

         /***** Start of element *****/
	 fprintf (Gbl.F.Out,"<li class=\"%s\">",
		  IsTheSelectedAction ? "MENU_ON" :
					"MENU_OFF");

         /***** Start of container used to highlight this option *****/
         if (IsTheSelectedAction)
            fprintf (Gbl.F.Out,"<div class=\"ICON_SCALED\">");
         else
            fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT ICON_SCALING\">");

         /***** Start of form and link *****/
         Act_FormStart (NumAct);
         Act_LinkFormSubmit (Title,IsTheSelectedAction ? The_ClassMenuOn[Gbl.Prefs.Theme] :
                                                         The_ClassMenuOff[Gbl.Prefs.Theme]);
	 fprintf (Gbl.F.Out,"<img src=\"%s/%s/%s64x64.gif\""
	                    " alt=\"%s\" title=\"%s\""
	                    " class=\"ICON28x28\" />"
			    "<div>%s</div>"
                            "</a>",
	          Gbl.Prefs.PathIconSet,Cfg_ICON_ACTION,
	          Act_Actions[NumAct].Icon,
	          Txt_MENU_TITLE[Gbl.CurrentTab][NumOptInMenu],
	          Txt_MENU_TITLE[Gbl.CurrentTab][NumOptInMenu],
                  Txt_MENU_TITLE[Gbl.CurrentTab][NumOptInMenu]);
	 Act_FormEnd ();

         /***** End of container used to highlight this option *****/
         fprintf (Gbl.F.Out,"</div>");

         /***** End of element *****/
         fprintf (Gbl.F.Out,"</li>");
        }
     }

   /***** List end *****/
   fprintf (Gbl.F.Out,"</ul>"
	              "</div>");
  }

/*****************************************************************************/
/************* Write the menu of current tab (mobile layout) *****************/
/*****************************************************************************/

void Mnu_WriteMenuThisTabMobile (void)
  {
   extern const char *The_ClassMenuOn[The_NUM_THEMES];
   extern const char *The_ClassMenuOff[The_NUM_THEMES];
   extern const struct Act_Menu Act_Menu[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];
   extern const char *Txt_MENU_TITLE[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];
   unsigned NumOptInMenu;
   unsigned NumOptVisible;	// Only options I can see
   Act_Action_t NumAct;
   const char *Title;

   /***** Table start *****/
   fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">");

   /***** Loop to write all options in menu. Each row holds an option *****/
   for (NumOptInMenu = NumOptVisible = 0;
        NumOptInMenu < Act_MAX_OPTIONS_IN_MENU_PER_TAB;
        NumOptInMenu++)
     {
      if ((NumAct = Act_Menu[Gbl.CurrentTab][NumOptInMenu].Action) == 0)	// At the end of each tab, actions are initialized to 0, so 0 marks the end of the menu
         break;
      if (Act_CheckIfIHavePermissionToExecuteAction (NumAct))
        {
         Title = Act_GetSubtitleAction (NumAct);

         if (NumOptVisible % Cfg_LAYOUT_MOBILE_NUM_COLUMNS == 0)
            fprintf (Gbl.F.Out,"<tr>");

         /* Icon at top and text at bottom */
	 fprintf (Gbl.F.Out,"<td class=\"CENTER_TOP\" style=\"width:25%%;\">"
	                    "<div class=\"ICON_HIGHLIGHT\">");

         Act_FormStart (NumAct);
         Act_LinkFormSubmit (Title,
                             (NumAct == Act_Actions[Gbl.CurrentAct].SuperAction) ? The_ClassMenuOn[Gbl.Prefs.Theme] :
                                                                                   The_ClassMenuOff[Gbl.Prefs.Theme]);
	 fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s/%s64x64.gif\""
	                    " alt=\"%s\" class=\"ICON64x64\""
	                    " style=\"margin:6px;\" />"
			    "<div>%s</div>"
                            "</a>",
	          Gbl.Prefs.PathIconSet,Cfg_ICON_ACTION,
	          Act_Actions[NumAct].Icon,
	          Txt_MENU_TITLE[Gbl.CurrentTab][NumOptInMenu],
                  Txt_MENU_TITLE[Gbl.CurrentTab][NumOptInMenu]);
	 Act_FormEnd ();
	 fprintf (Gbl.F.Out,"</div>"
                            "</td>");

         if ((NumOptVisible % Cfg_LAYOUT_MOBILE_NUM_COLUMNS) ==
             (Cfg_LAYOUT_MOBILE_NUM_COLUMNS - 1))
            fprintf (Gbl.F.Out,"</tr>");

         NumOptVisible++;
        }
     }

   /***** Table end *****/
   fprintf (Gbl.F.Out,"</table>");
  }

/*****************************************************************************/
/************* Put icons to select menu (horizontal or vertical) *************/
/*****************************************************************************/

void Mnu_PutIconsToSelectMenu (void)
  {
   extern const char *Txt_Menu;
   extern const char *Txt_MENU_NAMES[Mnu_NUM_MENUS];
   Mnu_Menu_t Menu;

   Lay_StartRoundFrameTable (NULL,2,Txt_Menu);
   fprintf (Gbl.F.Out,"<tr>");
   for (Menu = (Mnu_Menu_t) 0;
	Menu < Mnu_NUM_MENUS;
	Menu++)
     {
      fprintf (Gbl.F.Out,"<td class=\"%s\">",
               Menu == Gbl.Prefs.Menu ? "LAYOUT_ON" :
        	                        "LAYOUT_OFF");
      Act_FormStart (ActChgMnu);
      Par_PutHiddenParamUnsigned ("Menu",(unsigned) Menu);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s32x32.gif\""
	                 " alt=\"%s\" title=\"%s\" class=\"ICON32x32B\""
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

   return Mnu_MENU_UNKNOWN;
  }
