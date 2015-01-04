// swad_menu.h: menu (horizontal or vertical) selection

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

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************** Private constants **************************/
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
/************* Put icons to select menu (horizontal or vertical) *************/
/*****************************************************************************/

void Mnu_PutIconsToSelectMenu (void)
  {
   extern const char *Txt_Menu;
   extern const char *Txt_MENU_NAMES[Mnu_NUM_MENUS];
   Mnu_Menu_t Menu;

   Lay_StartRoundFrameTable10 (NULL,2,Txt_Menu);
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
	                 " style=\"margin:0 auto;\" />"
                         "</form>"
                         "</td>",
               Gbl.Prefs.IconsURL,
               Mnu_MenuIcons[Menu],
               Txt_MENU_NAMES[Menu],
               Txt_MENU_NAMES[Menu]);
     }
   fprintf (Gbl.F.Out,"</tr>");
   Lay_EndRoundFrameTable10 ();
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
   Prf_SetPrefsFromIP ();
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
