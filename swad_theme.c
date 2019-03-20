// swad_theme.c: themes (colour layouts)

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <stdio.h>		// For fprintf, etc.
#include <string.h>

#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_form.h"
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
   "purple",
   "blue",
   "yellow",
   "pink",
  };

const char *The_ThemeNames[The_NUM_THEMES] =
  {
   "White",
   "Grey",
   "Purple",
   "Blue",
   "Yellow",
   "Pink",
  };

const char *The_TabOnBgColors[The_NUM_THEMES] =
  {
   "TAB_ON_WHITE",
   "TAB_ON_GREY",
   "TAB_ON_PURPLE",
   "TAB_ON_BLUE",
   "TAB_ON_YELLOW",
   "TAB_ON_PINK",
  };
const char *The_TabOffBgColors[The_NUM_THEMES] =
  {
   "TAB_OFF_WHITE",
   "TAB_OFF_GREY",
   "TAB_OFF_PURPLE",
   "TAB_OFF_BLUE",
   "TAB_OFF_YELLOW",
   "TAB_OFF_PINK",
  };

const char *The_ClassTagline[The_NUM_THEMES] =
  {
   "TAGLINE_WHITE",
   "TAGLINE_GREY",
   "TAGLINE_PURPLE",
   "TAGLINE_BLUE",
   "TAGLINE_YELLOW",
   "TAGLINE_PINK",
  };

const char *The_ClassNotif[The_NUM_THEMES] =
  {
   "NOTIF_WHITE",
   "NOTIF_GREY",
   "NOTIF_PURPLE",
   "NOTIF_BLUE",
   "NOTIF_YELLOW",
   "NOTIF_PINK",
  };

const char *The_ClassUsr[The_NUM_THEMES] =
  {
   "USR_WHITE",
   "USR_GREY",
   "USR_PURPLE",
   "USR_BLUE",
   "USR_YELLOW",
   "USR_PINK",
  };

const char *The_ClassBreadcrumb[The_NUM_THEMES] =
  {
   "BC_WHITE",
   "BC_GREY",
   "BC_PURPLE",
   "BC_BLUE",
   "BC_YELLOW",
   "BC_PINK",
  };

const char *The_ClassCourse[The_NUM_THEMES] =
  {
   "MAIN_TITLE_WHITE",
   "MAIN_TITLE_GREY",
   "MAIN_TITLE_PURPLE",
   "MAIN_TITLE_BLUE",
   "MAIN_TITLE_YELLOW",
   "MAIN_TITLE_PINK",
  };

const char *The_ClassTxtMenu[The_NUM_THEMES] =
  {
   "MENU_TXT_WHITE",
   "MENU_TXT_GREY",
   "MENU_TXT_PURPLE",
   "MENU_TXT_BLUE",
   "MENU_TXT_YELLOW",
   "MENU_TXT_PINK",
  };

const char *The_ClassTxtTabOff[The_NUM_THEMES] =
  {
   "TAB_OFF_TXT_WHITE",
   "TAB_OFF_TXT_GREY",
   "TAB_OFF_TXT_PURPLE",
   "TAB_OFF_TXT_BLUE",
   "TAB_OFF_TXT_YELLOW",
   "TAB_OFF_TXT_PINK",
  };

const char *The_ClassTxtTabOn[The_NUM_THEMES] =
  {
   "TAB_ON_TXT_WHITE",
   "TAB_ON_TXT_GREY",
   "TAB_ON_TXT_PURPLE",
   "TAB_ON_TXT_BLUE",
   "TAB_ON_TXT_YELLOW",
   "TAB_ON_TXT_PINK",
  };

const char *The_ClassTitleAction[The_NUM_THEMES] =
  {
   "TITLE_ACTION_WHITE",
   "TITLE_ACTION_GREY",
   "TITLE_ACTION_PURPLE",
   "TITLE_ACTION_BLUE",
   "TITLE_ACTION_YELLOW",
   "TITLE_ACTION_PINK",
  };

const char *The_ClassSubtitleAction[The_NUM_THEMES] =
  {
   "SUBTITLE_ACTION_WHITE",
   "SUBTITLE_ACTION_GREY",
   "SUBTITLE_ACTION_PURPLE",
   "SUBTITLE_ACTION_BLUE",
   "SUBTITLE_ACTION_YELLOW",
   "SUBTITLE_ACTION_PINK",
  };

const char *The_ClassTitle[The_NUM_THEMES] =
  {
   "TITLE_WHITE",
   "TITLE_GREY",
   "TITLE_PURPLE",
   "TITLE_BLUE",
   "TITLE_YELLOW",
   "TITLE_PINK",
  };

const char *The_ClassFormInBox[The_NUM_THEMES] =
  {
   "FORM_IN_WHITE",
   "FORM_IN_GREY",
   "FORM_IN_PURPLE",
   "FORM_IN_BLUE",
   "FORM_IN_YELLOW",
   "FORM_IN_PINK",
  };

const char *The_ClassFormInBoxBold[The_NUM_THEMES] =
  {
   "FORM_IN_WHITE BOLD",
   "FORM_IN_GREY BOLD",
   "FORM_IN_PURPLE BOLD",
   "FORM_IN_BLUE BOLD",
   "FORM_IN_YELLOW BOLD",
   "FORM_IN_PINK BOLD",
  };

const char *The_ClassFormInBoxNoWrap[The_NUM_THEMES] =
  {
   "FORM_IN_WHITE NOWRAP",
   "FORM_IN_GREY NOWRAP",
   "FORM_IN_PURPLE NOWRAP",
   "FORM_IN_BLUE NOWRAP",
   "FORM_IN_YELLOW NOWRAP",
   "FORM_IN_PINK NOWRAP",
  };

const char *The_ClassFormOutBox[The_NUM_THEMES] =
  {
   "FORM_OUT_WHITE",
   "FORM_OUT_GREY",
   "FORM_OUT_PURPLE",
   "FORM_OUT_BLUE",
   "FORM_OUT_YELLOW",
   "FORM_OUT_PINK",
  };

const char *The_ClassFormOutBoxBold[The_NUM_THEMES] =
  {
   "FORM_OUT_WHITE BOLD",
   "FORM_OUT_GREY BOLD",
   "FORM_OUT_PURPLE BOLD",
   "FORM_OUT_BLUE BOLD",
   "FORM_OUT_YELLOW BOLD",
   "FORM_OUT_PINK BOLD",
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
   char Icon[PATH_MAX + 1];

   Box_StartBox (NULL,Txt_Theme_SKIN,The_PutIconsTheme,
                 Hlp_PROFILE_Preferences_theme,Box_NOT_CLOSABLE);
   Pre_StartPrefsHead ();
   Pre_StartOnePrefSelector ();
   for (Theme = (The_Theme_t) 0;
	Theme < The_NUM_THEMES;
	Theme++)
     {
      fprintf (Gbl.F.Out,"<div class=\"%s\">",
               Theme == Gbl.Prefs.Theme ? "PREF_ON" :
        	                          "PREF_OFF");
      Frm_StartForm (ActChgThe);
      Par_PutHiddenParamString ("Theme",The_ThemeId[Theme]);
      snprintf (Icon,sizeof (Icon),
		"%s/%s/theme_32x20.gif",
		Cfg_ICON_FOLDER_THEMES,
                The_ThemeId[Theme]);
      Ico_PutPrefIconLink (Icon,The_ThemeNames[Theme]);
      Frm_EndForm ();
      fprintf (Gbl.F.Out,"</div>");
     }
   Pre_EndOnePrefSelector ();
   Pre_EndPrefsHead ();
   Box_EndBox ();
  }

/*****************************************************************************/
/***************** Put contextual icons in theme preference ******************/
/*****************************************************************************/

static void The_PutIconsTheme (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_THEMES;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/********************************* Change theme ******************************/
/*****************************************************************************/

void The_ChangeTheme (void)
  {
   char Path[PATH_MAX + 1 +
             NAME_MAX + 1 +
             NAME_MAX + 1];

   /***** Get param theme *****/
   Gbl.Prefs.Theme = The_GetParamTheme ();
   snprintf (Path,sizeof (Path),
	     "%s/%s",
             Cfg_URL_ICON_THEMES_PUBLIC,
             The_ThemeId[Gbl.Prefs.Theme]);
   Str_Copy (Gbl.Prefs.URLTheme,Path,
             PATH_MAX);

   /***** Store theme in database *****/
   if (Gbl.Usrs.Me.Logged)
      DB_QueryUPDATE ("can not update your preference about theme",
		      "UPDATE usr_data SET Theme='%s'"
		      " WHERE UsrCod=%ld",
                      The_ThemeId[Gbl.Prefs.Theme],
		      Gbl.Usrs.Me.UsrDat.UsrCod);

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
