// swad_theme.c: themes (colour layouts)

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <stdio.h>	// For fprintf, etc.
#include <string.h>

#include "swad_config.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_layout.h"
#include "swad_parameter.h"
#include "swad_preference.h"
#include "swad_theme.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;
const char *Lay_LayoutIcons[Lay_NUM_LAYOUTS];

/*****************************************************************************/
/******************************** Private constants **************************/
/*****************************************************************************/

#define MAX_THEME_ID 16

const char *The_ThemeId[The_NUM_THEMES] =
  {
   "white",
   "grey",
   "blue",
   "yellow",
  };

const char *The_ThemeNames[The_NUM_THEMES] =
  {
   "White",
   "Grey",
   "Blue",
   "Yellow",
  };

const char *The_TabOnBgColors[The_NUM_THEMES] =
  {
   "#F7F6F5",
   "#F7F6F5",
   "#E8F3F6",
   "#FFF2BD",
  };
const char *The_TabOffBgColors[The_NUM_THEMES] =
  {
   "#D4D4D4",
   "#D4D4D4",
   "#CAE1E8",
   "#FADE94",
  };

const char *The_ClassSeparator[The_NUM_THEMES] =
  {
   "WHITE_SEPARA",
   "GREY_SEPARA",
   "BLUE_SEPARA",
   "YELLOW_SEPARA",
  };

const char *The_ClassHead[The_NUM_THEMES] =
  {
   "WHITE_HEAD",
   "GREY_HEAD",
   "BLUE_HEAD",
   "YELLOW_HEAD",
  };

const char *The_ClassCurrentTime[The_NUM_THEMES] =
  {
   "WHITE_CUR_TIME",
   "GREY_CUR_TIME",
   "BLUE_CUR_TIME",
   "YELLOW_CUR_TIME",
  };

const char *The_ClassNotif[The_NUM_THEMES] =
  {
   "WHITE_NOTIF",
   "GREY_NOTIF",
   "BLUE_NOTIF",
   "YELLOW_NOTIF",
  };

const char *The_ClassUsr[The_NUM_THEMES] =
  {
   "WHITE_USR",
   "GREY_USR",
   "BLUE_USR",
   "YELLOW_USR",
  };

const char *The_ClassDegree[The_NUM_THEMES] =
  {
   "WHITE_DEGREE",
   "GREY_DEGREE",
   "BLUE_DEGREE",
   "YELLOW_DEGREE",
  };

const char *The_ClassCourse[The_NUM_THEMES] =
  {
   "WHITE_COURSE",
   "GREY_COURSE",
   "BLUE_COURSE",
   "YELLOW_COURSE",
  };

const char *The_ClassConnected[The_NUM_THEMES] =
  {
   "WHITE_CONNECTED",
   "GREY_CONNECTED",
   "BLUE_CONNECTED",
   "YELLOW_CONNECTED",
  };

const char *The_ClassMenuOff[The_NUM_THEMES] =
  {
   "WHITE_MENU_OFF",
   "GREY_MENU_OFF",
   "BLUE_MENU_OFF",
   "YELLOW_MENU_OFF",
  };

const char *The_ClassMenuOn[The_NUM_THEMES] =
  {
   "WHITE_MENU_ON",
   "GREY_MENU_ON",
   "BLUE_MENU_ON",
   "YELLOW_MENU_ON",
  };

const char *The_ClassTabOff[The_NUM_THEMES] =
  {
   "WHITE_TAB_OFF",
   "GREY_TAB_OFF",
   "BLUE_TAB_OFF",
   "YELLOW_TAB_OFF",
  };

const char *The_ClassTabOn[The_NUM_THEMES] =
  {
   "WHITE_TAB_ON",
   "GREY_TAB_ON",
   "BLUE_TAB_ON",
   "YELLOW_TAB_ON",
  };

const char *The_ClassTitleAction[The_NUM_THEMES] =
  {
   "WHITE_TITLE_ACTION",
   "GREY_TITLE_ACTION",
   "BLUE_TITLE_ACTION",
   "YELLOW_TITLE_ACTION",
  };

const char *The_ClassSubtitleAction[The_NUM_THEMES] =
  {
   "WHITE_SUBTITLE_ACTION",
   "GREY_SUBTITLE_ACTION",
   "BLUE_SUBTITLE_ACTION",
   "YELLOW_SUBTITLE_ACTION",
  };

const char *The_ClassTitle[The_NUM_THEMES] =
  {
   "WHITE_TITLE",
   "GREY_TITLE",
   "BLUE_TITLE",
   "YELLOW_TITLE",
  };

const char *The_ClassForm[The_NUM_THEMES] =
  {
   "WHITE_FORM",
   "GREY_FORM",
   "BLUE_FORM",
   "YELLOW_FORM",
  };
const char *The_ClassFormRightMiddle[The_NUM_THEMES] =
  {
   "WHITE_FORM RIGHT_MIDDLE",
   "GREY_FORM RIGHT_MIDDLE",
   "BLUE_FORM RIGHT_MIDDLE",
   "YELLOW_FORM RIGHT_MIDDLE",
  };
const char *The_ClassFormRightTop[The_NUM_THEMES] =
  {
   "WHITE_FORM RIGHT_TOP",
   "GREY_FORM RIGHT_TOP",
   "BLUE_FORM RIGHT_TOP",
   "YELLOW_FORM RIGHT_TOP",
  };
const char *The_ClassFormNoWrap[The_NUM_THEMES] =
  {
   "WHITE_FORM_NOWRAP",
   "GREY_FORM_NOWRAP",
   "BLUE_FORM_NOWRAP",
   "YELLOW_FORM_NOWRAP",
  };
const char *The_ClassFormBold[The_NUM_THEMES] =
  {
   "WHITE_FORM_BOLD",
   "GREY_FORM_BOLD",
   "BLUE_FORM_BOLD",
   "YELLOW_FORM_BOLD",
  };

/*****************************************************************************/
/************************ Put icons to select a theme ***********************/
/*****************************************************************************/

void The_PutIconsToSelectTheme (void)
  {
   extern const char *Txt_Theme_SKIN;
   The_Theme_t Theme;

   Lay_StartRoundFrameTable (NULL,2,Txt_Theme_SKIN);
   fprintf (Gbl.F.Out,"<tr>");
   for (Theme = (The_Theme_t) 0;
	Theme < The_NUM_THEMES;
	Theme++)
     {
      fprintf (Gbl.F.Out,"<td class=\"%s\">",
               Theme == Gbl.Prefs.Theme ? "LAYOUT_ON" :
        	                          "LAYOUT_OFF");
      Act_FormStart (ActChgThe);
      Par_PutHiddenParamString ("Theme",The_ThemeId[Theme]);
      fprintf (Gbl.F.Out,"<input type=\"image\""
	                 " src=\"%s/%s/%s/theme_32x20.gif\" alt=\"%s\""
	                 " title=\"%s\" style=\"display:block;"
	                 " width:32px; height:20px; margin:0 auto;\" />",
               Gbl.Prefs.IconsURL,
               Cfg_ICON_FOLDER_THEMES,
               The_ThemeId[Theme],
               The_ThemeNames[Theme],
               The_ThemeNames[Theme]);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");
     }
   fprintf (Gbl.F.Out,"</tr>");
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/********************************* Change theme ******************************/
/*****************************************************************************/

void The_ChangeTheme (void)
  {
   char Query[512];

   /***** Get param theme *****/
   Gbl.Prefs.Theme = The_GetParamTheme ();
   sprintf (Gbl.Prefs.PathTheme,"%s/%s/%s",
            Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_THEMES,The_ThemeId[Gbl.Prefs.Theme]);

   /***** Store theme in database *****/
   if (Gbl.Usrs.Me.Logged)
     {
      sprintf (Query,"UPDATE usr_data SET Theme='%s' WHERE UsrCod='%ld'",
               The_ThemeId[Gbl.Prefs.Theme],Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryUPDATE (Query,"can not update your preference about theme");
     }

   /***** Set preferences from current IP *****/
   Pre_SetPrefsFromIP ();
  }

/*****************************************************************************/
/***************************** Get parameter theme ***************************/
/*****************************************************************************/

The_Theme_t The_GetParamTheme (void)
  {
   char ThemeId[MAX_THEME_ID+1];
   The_Theme_t Theme;

   Par_GetParToText ("Theme",ThemeId,MAX_THEME_ID);
   for (Theme = (The_Theme_t) 0;
	Theme < The_NUM_THEMES;
	Theme++)
      if (!strcmp (ThemeId,The_ThemeId[Theme]))
         return Theme;

   return The_THEME_UNKNOWN;
  }

/*****************************************************************************/
/**************************** Get theme from string **************************/
/*****************************************************************************/

The_Theme_t The_GetThemeFromStr (const char *Str)
  {
   The_Theme_t Theme;

   for (Theme = (The_Theme_t) 0;
	Theme < The_NUM_THEMES;
	Theme++)
      if (!strcasecmp (Str,The_ThemeId[Theme]))
	 return Theme;

   return The_THEME_UNKNOWN;
  }
