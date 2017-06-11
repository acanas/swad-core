// swad_theme.c: themes (colour layouts)

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <stdio.h>	// For fprintf, etc.
#include <string.h>

#include "swad_box.h"
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

/*****************************************************************************/
/******************************** Private constants **************************/
/*****************************************************************************/

#define The_MAX_BYTES_THEME_ID 16

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
   "TAB_ON_WHITE",
   "TAB_ON_GREY",
   "TAB_ON_BLUE",
   "TAB_ON_YELLOW",
  };
const char *The_TabOffBgColors[The_NUM_THEMES] =
  {
   "TAB_OFF_WHITE",
   "TAB_OFF_GREY",
   "TAB_OFF_BLUE",
   "TAB_OFF_YELLOW",
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

const char *The_ClassTagline[The_NUM_THEMES] =
  {
   "WHITE_TAGLINE",
   "GREY_TAGLINE",
   "BLUE_TAGLINE",
   "YELLOW_TAGLINE",
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

const char *The_ClassBreadcrumb[The_NUM_THEMES] =
  {
   "BC_WHITE",
   "BC_GREY",
   "BC_BLUE",
   "BC_YELLOW",
  };

const char *The_ClassCourse[The_NUM_THEMES] =
  {
   "WHITE_COURSE",
   "GREY_COURSE",
   "BLUE_COURSE",
   "YELLOW_COURSE",
  };

const char *The_ClassTxtMenuOff[The_NUM_THEMES] =
  {
   "WHITE_MENU_OFF",
   "GREY_MENU_OFF",
   "BLUE_MENU_OFF",
   "YELLOW_MENU_OFF",
  };

const char *The_ClassTxtMenuOn[The_NUM_THEMES] =
  {
   "WHITE_MENU_ON",
   "GREY_MENU_ON",
   "BLUE_MENU_ON",
   "YELLOW_MENU_ON",
  };

const char *The_ClassTxtTabOff[The_NUM_THEMES] =
  {
   "WHITE_TAB_OFF",
   "GREY_TAB_OFF",
   "BLUE_TAB_OFF",
   "YELLOW_TAB_OFF",
  };

const char *The_ClassTxtTabOn[The_NUM_THEMES] =
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

const char *The_ClassFormDark[The_NUM_THEMES] =
  {
   "WHITE_FORM_DARK",
   "GREY_FORM_DARK",
   "BLUE_FORM_DARK",
   "YELLOW_FORM_DARK",
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
/****************************** Private prototypes ***************************/
/*****************************************************************************/

static void The_PutIconsTheme (void);

/*****************************************************************************/
/************************ Put icons to select a theme ***********************/
/*****************************************************************************/

void The_PutIconsToSelectTheme (void)
  {
   extern const char *Hlp_PROFILE_Preferences_theme;
   extern const char *Txt_Theme_SKIN;
   The_Theme_t Theme;

   Box_StartBox (NULL,Txt_Theme_SKIN,The_PutIconsTheme,
                 Hlp_PROFILE_Preferences_theme,
                 false);	// Not closable
   fprintf (Gbl.F.Out,"<div class=\"PREF_CONTAINER\">");
   for (Theme = (The_Theme_t) 0;
	Theme < The_NUM_THEMES;
	Theme++)
     {
      fprintf (Gbl.F.Out,"<div class=\"%s\">",
               Theme == Gbl.Prefs.Theme ? "PREF_ON" :
        	                          "PREF_OFF");
      Act_FormStart (ActChgThe);
      Par_PutHiddenParamString ("Theme",The_ThemeId[Theme]);
      fprintf (Gbl.F.Out,"<input type=\"image\""
	                 " src=\"%s/%s/%s/theme_32x20.gif\" alt=\"%s\""
	                 " title=\"%s\" class=\"ICO40x25B\" />",
               Gbl.Prefs.IconsURL,
               Cfg_ICON_FOLDER_THEMES,
               The_ThemeId[Theme],
               The_ThemeNames[Theme],
               The_ThemeNames[Theme]);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");
     }
   fprintf (Gbl.F.Out,"</div>");
   Box_EndBox ();
  }

/*****************************************************************************/
/***************** Put contextual icons in theme preference ******************/
/*****************************************************************************/

static void The_PutIconsTheme (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_THEMES;
   Sta_PutIconToShowFigure ();
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
      sprintf (Query,"UPDATE usr_data SET Theme='%s'"
	             " WHERE UsrCod=%ld",
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
   char ThemeId[The_MAX_BYTES_THEME_ID + 1];
   The_Theme_t Theme;

   Par_GetParToText ("Theme",ThemeId,The_MAX_BYTES_THEME_ID);
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
