// swad_theme.c: themes (colour layouts)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include <string.h>

#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_layout.h"
#include "swad_parameter.h"
#include "swad_setting.h"
#include "swad_setting_database.h"
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
   [The_THEME_WHITE ] = "white",
   [The_THEME_GREY  ] = "grey",
   [The_THEME_PURPLE] = "purple",
   [The_THEME_BLUE  ] = "blue",
   [The_THEME_YELLOW] = "yellow",
   [The_THEME_PINK  ] = "pink",
  };

const char *The_ThemeNames[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "White",
   [The_THEME_GREY  ] = "Grey",
   [The_THEME_PURPLE] = "Purple",
   [The_THEME_BLUE  ] = "Blue",
   [The_THEME_YELLOW] = "Yellow",
   [The_THEME_PINK  ] = "Pink",
  };

const char *The_TabOnBgColors[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "TAB_ON_WHITE",
   [The_THEME_GREY  ] = "TAB_ON_GREY",
   [The_THEME_PURPLE] = "TAB_ON_PURPLE",
   [The_THEME_BLUE  ] = "TAB_ON_BLUE",
   [The_THEME_YELLOW] = "TAB_ON_YELLOW",
   [The_THEME_PINK  ] = "TAB_ON_PINK",
  };

const char *The_TabOffBgColors[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "TAB_OFF_WHITE",
   [The_THEME_GREY  ] = "TAB_OFF_GREY",
   [The_THEME_PURPLE] = "TAB_OFF_PURPLE",
   [The_THEME_BLUE  ] = "TAB_OFF_BLUE",
   [The_THEME_YELLOW] = "TAB_OFF_YELLOW",
   [The_THEME_PINK  ] = "TAB_OFF_PINK",
  };

const char *The_ClassTagline[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "BT_LINK TAGLINE_WHITE",
   [The_THEME_GREY  ] = "BT_LINK TAGLINE_GREY",
   [The_THEME_PURPLE] = "BT_LINK TAGLINE_PURPLE",
   [The_THEME_BLUE  ] = "BT_LINK TAGLINE_BLUE",
   [The_THEME_YELLOW] = "BT_LINK TAGLINE_YELLOW",
   [The_THEME_PINK  ] = "BT_LINK TAGLINE_PINK",
  };

const char *The_ClassNotif[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "BT_LINK NOTIF_WHITE",
   [The_THEME_GREY  ] = "BT_LINK NOTIF_GREY",
   [The_THEME_PURPLE] = "BT_LINK NOTIF_PURPLE",
   [The_THEME_BLUE  ] = "BT_LINK NOTIF_BLUE",
   [The_THEME_YELLOW] = "BT_LINK NOTIF_YELLOW",
   [The_THEME_PINK  ] = "BT_LINK NOTIF_PINK",
  };

const char *The_ClassUsr[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "USR_WHITE",
   [The_THEME_GREY  ] = "USR_GREY",
   [The_THEME_PURPLE] = "USR_PURPLE",
   [The_THEME_BLUE  ] = "USR_BLUE",
   [The_THEME_YELLOW] = "USR_YELLOW",
   [The_THEME_PINK  ] = "USR_PINK",
  };

const char *The_ClassBreadcrumb[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "BC_WHITE",
   [The_THEME_GREY  ] = "BC_GREY",
   [The_THEME_PURPLE] = "BC_PURPLE",
   [The_THEME_BLUE  ] = "BC_BLUE",
   [The_THEME_YELLOW] = "BC_YELLOW",
   [The_THEME_PINK  ] = "BC_PINK",
  };

const char *The_ClassCourse[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "MAIN_TITLE_WHITE",
   [The_THEME_GREY  ] = "MAIN_TITLE_GREY",
   [The_THEME_PURPLE] = "MAIN_TITLE_PURPLE",
   [The_THEME_BLUE  ] = "MAIN_TITLE_BLUE",
   [The_THEME_YELLOW] = "MAIN_TITLE_YELLOW",
   [The_THEME_PINK  ] = "MAIN_TITLE_PINK",
  };

const char *The_ClassTxtMenu[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "MENU_TXT_WHITE",
   [The_THEME_GREY  ] = "MENU_TXT_GREY",
   [The_THEME_PURPLE] = "MENU_TXT_PURPLE",
   [The_THEME_BLUE  ] = "MENU_TXT_BLUE",
   [The_THEME_YELLOW] = "MENU_TXT_YELLOW",
   [The_THEME_PINK  ] = "MENU_TXT_PINK",
  };

const char *The_ClassTxtTabOff[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "TAB_OFF_TXT_WHITE",
   [The_THEME_GREY  ] = "TAB_OFF_TXT_GREY",
   [The_THEME_PURPLE] = "TAB_OFF_TXT_PURPLE",
   [The_THEME_BLUE  ] = "TAB_OFF_TXT_BLUE",
   [The_THEME_YELLOW] = "TAB_OFF_TXT_YELLOW",
   [The_THEME_PINK  ] = "TAB_OFF_TXT_PINK",
  };

const char *The_ClassTxtTabOn[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "TAB_ON_TXT_WHITE",
   [The_THEME_GREY  ] = "TAB_ON_TXT_GREY",
   [The_THEME_PURPLE] = "TAB_ON_TXT_PURPLE",
   [The_THEME_BLUE  ] = "TAB_ON_TXT_BLUE",
   [The_THEME_YELLOW] = "TAB_ON_TXT_YELLOW",
   [The_THEME_PINK  ] = "TAB_ON_TXT_PINK",
  };

const char *The_ClassTitleAction[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "TITLE_ACTION_WHITE",
   [The_THEME_GREY  ] = "TITLE_ACTION_GREY",
   [The_THEME_PURPLE] = "TITLE_ACTION_PURPLE",
   [The_THEME_BLUE  ] = "TITLE_ACTION_BLUE",
   [The_THEME_YELLOW] = "TITLE_ACTION_YELLOW",
   [The_THEME_PINK  ] = "TITLE_ACTION_PINK",
  };

const char *The_ClassSubtitleAction[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "SUBTITLE_ACTION_WHITE",
   [The_THEME_GREY  ] = "SUBTITLE_ACTION_GREY",
   [The_THEME_PURPLE] = "SUBTITLE_ACTION_PURPLE",
   [The_THEME_BLUE  ] = "SUBTITLE_ACTION_BLUE",
   [The_THEME_YELLOW] = "SUBTITLE_ACTION_YELLOW",
   [The_THEME_PINK  ] = "SUBTITLE_ACTION_PINK",
  };

const char *The_ClassTitle[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "TITLE_WHITE",
   [The_THEME_GREY  ] = "TITLE_GREY",
   [The_THEME_PURPLE] = "TITLE_PURPLE",
   [The_THEME_BLUE  ] = "TITLE_BLUE",
   [The_THEME_YELLOW] = "TITLE_YELLOW",
   [The_THEME_PINK  ] = "TITLE_PINK",
  };

const char *The_ClassFormLinkInBox[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "BT_LINK FORM_IN_WHITE",
   [The_THEME_GREY  ] = "BT_LINK FORM_IN_GREY",
   [The_THEME_PURPLE] = "BT_LINK FORM_IN_PURPLE",
   [The_THEME_BLUE  ] = "BT_LINK FORM_IN_BLUE",
   [The_THEME_YELLOW] = "BT_LINK FORM_IN_YELLOW",
   [The_THEME_PINK  ] = "BT_LINK FORM_IN_PINK",
  };

const char *The_ClassFormInBox[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "FORM_IN_WHITE",
   [The_THEME_GREY  ] = "FORM_IN_GREY",
   [The_THEME_PURPLE] = "FORM_IN_PURPLE",
   [The_THEME_BLUE  ] = "FORM_IN_BLUE",
   [The_THEME_YELLOW] = "FORM_IN_YELLOW",
   [The_THEME_PINK  ] = "FORM_IN_PINK",
  };

const char *The_ClassFormLinkInBoxBold[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "BT_LINK FORM_IN_WHITE BOLD",
   [The_THEME_GREY  ] = "BT_LINK FORM_IN_GREY BOLD",
   [The_THEME_PURPLE] = "BT_LINK FORM_IN_PURPLE BOLD",
   [The_THEME_BLUE  ] = "BT_LINK FORM_IN_BLUE BOLD",
   [The_THEME_YELLOW] = "BT_LINK FORM_IN_YELLOW BOLD",
   [The_THEME_PINK  ] = "BT_LINK FORM_IN_PINK BOLD",
  };

const char *The_ClassFormInBoxBold[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "FORM_IN_WHITE BOLD",
   [The_THEME_GREY  ] = "FORM_IN_GREY BOLD",
   [The_THEME_PURPLE] = "FORM_IN_PURPLE BOLD",
   [The_THEME_BLUE  ] = "FORM_IN_BLUE BOLD",
   [The_THEME_YELLOW] = "FORM_IN_YELLOW BOLD",
   [The_THEME_PINK  ] = "FORM_IN_PINK BOLD",
  };

const char *The_ClassFormLinkInBoxNoWrap[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "BT_LINK FORM_IN_WHITE NOWRAP",
   [The_THEME_GREY  ] = "BT_LINK FORM_IN_GREY NOWRAP",
   [The_THEME_PURPLE] = "BT_LINK FORM_IN_PURPLE NOWRAP",
   [The_THEME_BLUE  ] = "BT_LINK FORM_IN_BLUE NOWRAP",
   [The_THEME_YELLOW] = "BT_LINK FORM_IN_YELLOW NOWRAP",
   [The_THEME_PINK  ] = "BT_LINK FORM_IN_PINK NOWRAP",
  };

const char *The_ClassFormInBoxNoWrap[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "FORM_IN_WHITE NOWRAP",
   [The_THEME_GREY  ] = "FORM_IN_GREY NOWRAP",
   [The_THEME_PURPLE] = "FORM_IN_PURPLE NOWRAP",
   [The_THEME_BLUE  ] = "FORM_IN_BLUE NOWRAP",
   [The_THEME_YELLOW] = "FORM_IN_YELLOW NOWRAP",
   [The_THEME_PINK  ] = "FORM_IN_PINK NOWRAP",
  };

const char *The_ClassFormLinkOutBox[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "BT_LINK FORM_OUT_WHITE",
   [The_THEME_GREY  ] = "BT_LINK FORM_OUT_GREY",
   [The_THEME_PURPLE] = "BT_LINK FORM_OUT_PURPLE",
   [The_THEME_BLUE  ] = "BT_LINK FORM_OUT_BLUE",
   [The_THEME_YELLOW] = "BT_LINK FORM_OUT_YELLOW",
   [The_THEME_PINK  ] = "BT_LINK FORM_OUT_PINK",
  };

const char *The_ClassFormOutBox[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "FORM_OUT_WHITE",
   [The_THEME_GREY  ] = "FORM_OUT_GREY",
   [The_THEME_PURPLE] = "FORM_OUT_PURPLE",
   [The_THEME_BLUE  ] = "FORM_OUT_BLUE",
   [The_THEME_YELLOW] = "FORM_OUT_YELLOW",
   [The_THEME_PINK  ] = "FORM_OUT_PINK",
  };

const char *The_ClassFormLinkOutBoxBold[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "BT_LINK FORM_OUT_WHITE BOLD",
   [The_THEME_GREY  ] = "BT_LINK FORM_OUT_GREY BOLD",
   [The_THEME_PURPLE] = "BT_LINK FORM_OUT_PURPLE BOLD",
   [The_THEME_BLUE  ] = "BT_LINK FORM_OUT_BLUE BOLD",
   [The_THEME_YELLOW] = "BT_LINK FORM_OUT_YELLOW BOLD",
   [The_THEME_PINK  ] = "BT_LINK FORM_OUT_PINK BOLD",
  };

const char *The_ClassFormOutBoxBold[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "FORM_OUT_WHITE BOLD",
   [The_THEME_GREY  ] = "FORM_OUT_GREY BOLD",
   [The_THEME_PURPLE] = "FORM_OUT_PURPLE BOLD",
   [The_THEME_BLUE  ] = "FORM_OUT_BLUE BOLD",
   [The_THEME_YELLOW] = "FORM_OUT_YELLOW BOLD",
   [The_THEME_PINK  ] = "FORM_OUT_PINK BOLD",
  };

/*****************************************************************************/
/****************************** Private prototypes ***************************/
/*****************************************************************************/

static void The_PutIconsTheme (__attribute__((unused)) void *Args);

/*****************************************************************************/
/************************ Put icons to select a theme ***********************/
/*****************************************************************************/

void The_PutIconsToSelectTheme (void)
  {
   extern const char *Hlp_PROFILE_Settings_theme;
   extern const char *Txt_Theme_SKIN;
   The_Theme_t Theme;
   char Icon[PATH_MAX + 1];

   Box_BoxBegin (NULL,Txt_Theme_SKIN,
                 The_PutIconsTheme,NULL,
                 Hlp_PROFILE_Settings_theme,Box_NOT_CLOSABLE);
      Set_BeginSettingsHead ();
	 Set_BeginOneSettingSelector ();
	    for (Theme  = (The_Theme_t) 0;
		 Theme <= (The_Theme_t) (The_NUM_THEMES - 1);
		 Theme++)
	      {
	       HTM_DIV_Begin ("class=\"%s\"",Theme == Gbl.Prefs.Theme ? "PREF_ON" :
									"PREF_OFF");
		  Frm_BeginForm (ActChgThe);
		  Par_PutHiddenParamString (NULL,"Theme",The_ThemeId[Theme]);
		     snprintf (Icon,sizeof (Icon),"%s/%s/theme_32x20.gif",
			       Cfg_ICON_FOLDER_THEMES,The_ThemeId[Theme]);
		     Ico_PutSettingIconLink (Icon,The_ThemeNames[Theme]);
		  Frm_EndForm ();
	       HTM_DIV_End ();
	      }
	 Set_EndOneSettingSelector ();
      Set_EndSettingsHead ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/****************** Put contextual icons in theme setting ********************/
/*****************************************************************************/

static void The_PutIconsTheme (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_THEMES);
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
   snprintf (Path,sizeof (Path),"%s/%s",
             Cfg_URL_ICON_THEMES_PUBLIC,The_ThemeId[Gbl.Prefs.Theme]);
   Str_Copy (Gbl.Prefs.URLTheme,Path,sizeof (Gbl.Prefs.URLTheme) - 1);

   /***** Store theme in database *****/
   if (Gbl.Usrs.Me.Logged)
      Set_DB_UpdateMySettingsAboutTheme (The_ThemeId[Gbl.Prefs.Theme]);

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/***************************** Get parameter theme ***************************/
/*****************************************************************************/

The_Theme_t The_GetParamTheme (void)
  {
   char ThemeId[The_MAX_BYTES_THEME_ID + 1];
   The_Theme_t Theme;

   Par_GetParToText ("Theme",ThemeId,The_MAX_BYTES_THEME_ID);
   for (Theme  = (The_Theme_t) 0;
	Theme <= (The_Theme_t) (The_NUM_THEMES - 1);
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

   for (Theme  = (The_Theme_t) 0;
	Theme <= (The_Theme_t) (The_NUM_THEMES - 1);
	Theme++)
      if (!strcasecmp (Str,The_ThemeId[Theme]))
	 return Theme;

   return The_THEME_UNKNOWN;
  }
