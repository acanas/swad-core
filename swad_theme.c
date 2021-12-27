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
   [The_THEME_DARK  ] = "dark",
  };

const char *The_ThemeNames[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "White",
   [The_THEME_GREY  ] = "Grey",
   [The_THEME_PURPLE] = "Purple",
   [The_THEME_BLUE  ] = "Blue",
   [The_THEME_YELLOW] = "Yellow",
   [The_THEME_PINK  ] = "Pink",
   [The_THEME_DARK  ] = "Dark",
  };

const char *The_ClassFormLinkInBox[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "BT_LINK FORM_IN_WHITE",
   [The_THEME_GREY  ] = "BT_LINK FORM_IN_GREY",
   [The_THEME_PURPLE] = "BT_LINK FORM_IN_PURPLE",
   [The_THEME_BLUE  ] = "BT_LINK FORM_IN_BLUE",
   [The_THEME_YELLOW] = "BT_LINK FORM_IN_YELLOW",
   [The_THEME_PINK  ] = "BT_LINK FORM_IN_PINK",
   [The_THEME_DARK  ] = "BT_LINK FORM_IN_DARK",
  };

const char *The_ClassFormInBox[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "FORM_IN_WHITE",
   [The_THEME_GREY  ] = "FORM_IN_GREY",
   [The_THEME_PURPLE] = "FORM_IN_PURPLE",
   [The_THEME_BLUE  ] = "FORM_IN_BLUE",
   [The_THEME_YELLOW] = "FORM_IN_YELLOW",
   [The_THEME_PINK  ] = "FORM_IN_PINK",
   [The_THEME_DARK  ] = "FORM_IN_DARK",
  };

const char *The_ClassFormLinkInBoxBold[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "BT_LINK FORM_IN_WHITE BOLD",
   [The_THEME_GREY  ] = "BT_LINK FORM_IN_GREY BOLD",
   [The_THEME_PURPLE] = "BT_LINK FORM_IN_PURPLE BOLD",
   [The_THEME_BLUE  ] = "BT_LINK FORM_IN_BLUE BOLD",
   [The_THEME_YELLOW] = "BT_LINK FORM_IN_YELLOW BOLD",
   [The_THEME_PINK  ] = "BT_LINK FORM_IN_PINK BOLD",
   [The_THEME_DARK  ] = "BT_LINK FORM_IN_DARK BOLD",
  };

const char *The_ClassFormInBoxBold[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "FORM_IN_WHITE BOLD",
   [The_THEME_GREY  ] = "FORM_IN_GREY BOLD",
   [The_THEME_PURPLE] = "FORM_IN_PURPLE BOLD",
   [The_THEME_BLUE  ] = "FORM_IN_BLUE BOLD",
   [The_THEME_YELLOW] = "FORM_IN_YELLOW BOLD",
   [The_THEME_PINK  ] = "FORM_IN_PINK BOLD",
   [The_THEME_DARK  ] = "FORM_IN_DARK BOLD",
  };

const char *The_ClassFormLinkInBoxNoWrap[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "BT_LINK FORM_IN_WHITE NOWRAP",
   [The_THEME_GREY  ] = "BT_LINK FORM_IN_GREY NOWRAP",
   [The_THEME_PURPLE] = "BT_LINK FORM_IN_PURPLE NOWRAP",
   [The_THEME_BLUE  ] = "BT_LINK FORM_IN_BLUE NOWRAP",
   [The_THEME_YELLOW] = "BT_LINK FORM_IN_YELLOW NOWRAP",
   [The_THEME_PINK  ] = "BT_LINK FORM_IN_PINK NOWRAP",
   [The_THEME_DARK  ] = "BT_LINK FORM_IN_DARK NOWRAP",
  };

const char *The_ClassFormInBoxNoWrap[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "FORM_IN_WHITE NOWRAP",
   [The_THEME_GREY  ] = "FORM_IN_GREY NOWRAP",
   [The_THEME_PURPLE] = "FORM_IN_PURPLE NOWRAP",
   [The_THEME_BLUE  ] = "FORM_IN_BLUE NOWRAP",
   [The_THEME_YELLOW] = "FORM_IN_YELLOW NOWRAP",
   [The_THEME_PINK  ] = "FORM_IN_PINK NOWRAP",
   [The_THEME_DARK  ] = "FORM_IN_DARK NOWRAP",
  };

const char *The_ClassFormLinkOutBox[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "BT_LINK FORM_OUT_WHITE",
   [The_THEME_GREY  ] = "BT_LINK FORM_OUT_GREY",
   [The_THEME_PURPLE] = "BT_LINK FORM_OUT_PURPLE",
   [The_THEME_BLUE  ] = "BT_LINK FORM_OUT_BLUE",
   [The_THEME_YELLOW] = "BT_LINK FORM_OUT_YELLOW",
   [The_THEME_PINK  ] = "BT_LINK FORM_OUT_PINK",
   [The_THEME_DARK  ] = "BT_LINK FORM_OUT_DARK",
  };

const char *The_ClassFormOutBox[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "FORM_OUT_WHITE",
   [The_THEME_GREY  ] = "FORM_OUT_GREY",
   [The_THEME_PURPLE] = "FORM_OUT_PURPLE",
   [The_THEME_BLUE  ] = "FORM_OUT_BLUE",
   [The_THEME_YELLOW] = "FORM_OUT_YELLOW",
   [The_THEME_PINK  ] = "FORM_OUT_PINK",
   [The_THEME_DARK  ] = "FORM_OUT_DARK",
  };

const char *The_ClassFormLinkOutBoxBold[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "BT_LINK FORM_OUT_WHITE BOLD",
   [The_THEME_GREY  ] = "BT_LINK FORM_OUT_GREY BOLD",
   [The_THEME_PURPLE] = "BT_LINK FORM_OUT_PURPLE BOLD",
   [The_THEME_BLUE  ] = "BT_LINK FORM_OUT_BLUE BOLD",
   [The_THEME_YELLOW] = "BT_LINK FORM_OUT_YELLOW BOLD",
   [The_THEME_PINK  ] = "BT_LINK FORM_OUT_PINK BOLD",
   [The_THEME_DARK  ] = "BT_LINK FORM_OUT_DARK BOLD",
  };

const char *The_ClassFormOutBoxBold[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "FORM_OUT_WHITE BOLD",
   [The_THEME_GREY  ] = "FORM_OUT_GREY BOLD",
   [The_THEME_PURPLE] = "FORM_OUT_PURPLE BOLD",
   [The_THEME_BLUE  ] = "FORM_OUT_BLUE BOLD",
   [The_THEME_YELLOW] = "FORM_OUT_YELLOW BOLD",
   [The_THEME_PINK  ] = "FORM_OUT_PINK BOLD",
   [The_THEME_DARK  ] = "FORM_OUT_DARK BOLD",
  };

const char *The_ClassDat[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "DAT_WHITE",
   [The_THEME_GREY  ] = "DAT_GREY",
   [The_THEME_PURPLE] = "DAT_PURPLE",
   [The_THEME_BLUE  ] = "DAT_BLUE",
   [The_THEME_YELLOW] = "DAT_YELLOW",
   [The_THEME_PINK  ] = "DAT_PINK",
   [The_THEME_DARK  ] = "DAT_DARK",
  };

const char *The_ClassDatStrong[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "DAT_STRONG_WHITE",
   [The_THEME_GREY  ] = "DAT_STRONG_GREY",
   [The_THEME_PURPLE] = "DAT_STRONG_PURPLE",
   [The_THEME_BLUE  ] = "DAT_STRONG_BLUE",
   [The_THEME_YELLOW] = "DAT_STRONG_YELLOW",
   [The_THEME_PINK  ] = "DAT_STRONG_PINK",
   [The_THEME_DARK  ] = "DAT_STRONG_DARK",
  };

const char *The_ClassDatLight[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "DAT_LIGHT_WHITE",
   [The_THEME_GREY  ] = "DAT_LIGHT_GREY",
   [The_THEME_PURPLE] = "DAT_LIGHT_PURPLE",
   [The_THEME_BLUE  ] = "DAT_LIGHT_BLUE",
   [The_THEME_YELLOW] = "DAT_LIGHT_YELLOW",
   [The_THEME_PINK  ] = "DAT_LIGHT_PINK",
   [The_THEME_DARK  ] = "DAT_LIGHT_DARK",
  };

const char *The_ClassBgHighlight[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "BG_HIGHLIGHT_WHITE",
   [The_THEME_GREY  ] = "BG_HIGHLIGHT_GREY",
   [The_THEME_PURPLE] = "BG_HIGHLIGHT_PURPLE",
   [The_THEME_BLUE  ] = "BG_HIGHLIGHT_BLUE",
   [The_THEME_YELLOW] = "BG_HIGHLIGHT_YELLOW",
   [The_THEME_PINK  ] = "BG_HIGHLIGHT_PINK",
   [The_THEME_DARK  ] = "BG_HIGHLIGHT_DARK",
  };

const char *The_ClassInput[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "INPUT_WHITE",
   [The_THEME_GREY  ] = "INPUT_GREY",
   [The_THEME_PURPLE] = "INPUT_PURPLE",
   [The_THEME_BLUE  ] = "INPUT_BLUE",
   [The_THEME_YELLOW] = "INPUT_YELLOW",
   [The_THEME_PINK  ] = "INPUT_PINK",
   [The_THEME_DARK  ] = "INPUT_DARK",
  };

const char *The_Colors[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "WHITE",
   [The_THEME_GREY  ] = "GREY",
   [The_THEME_PURPLE] = "PURPLE",
   [The_THEME_BLUE  ] = "BLUE",
   [The_THEME_YELLOW] = "YELLOW",
   [The_THEME_PINK  ] = "PINK",
   [The_THEME_DARK  ] = "DARK",
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
	       if (Theme == Gbl.Prefs.Theme)
		  HTM_DIV_Begin ("class=\"PREF_ON PREF_ON_%s\"",
		                 The_Colors[Gbl.Prefs.Theme]);
	       else
		  HTM_DIV_Begin ("class=\"PREF_OFF\"");
	       Frm_BeginForm (ActChgThe);
		  Par_PutHiddenParamString (NULL,"Theme",The_ThemeId[Theme]);
		  snprintf (Icon,sizeof (Icon),"%s/%s/theme_32x20.gif",
			    Cfg_ICON_FOLDER_THEMES,The_ThemeId[Theme]);
		  Ico_PutSettingIconLink (Icon,Ico_UNCHANGED,The_ThemeNames[Theme]);
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
   The_SetColorRows ();
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

   return The_THEME_DEFAULT;
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

   return The_THEME_DEFAULT;
  }

/*****************************************************************************/
/******** Set background colors for rows depending on selected theme *********/
/*****************************************************************************/

void The_SetColorRows (void)
  {
   static const char *The_ClassColorRows[2][The_NUM_THEMES] =
     {
      [0][The_THEME_WHITE ] = "COLOR0_WHITE",
      [0][The_THEME_GREY  ] = "COLOR0_GREY",
      [0][The_THEME_PURPLE] = "COLOR0_PURPLE",
      [0][The_THEME_BLUE  ] = "COLOR0_BLUE",
      [0][The_THEME_YELLOW] = "COLOR0_YELLOW",
      [0][The_THEME_PINK  ] = "COLOR0_PINK",
      [0][The_THEME_DARK  ] = "COLOR0_DARK",

      [1][The_THEME_WHITE ] = "COLOR1_WHITE",
      [1][The_THEME_GREY  ] = "COLOR1_GREY",
      [1][The_THEME_PURPLE] = "COLOR1_PURPLE",
      [1][The_THEME_BLUE  ] = "COLOR1_BLUE",
      [1][The_THEME_YELLOW] = "COLOR1_YELLOW",
      [1][The_THEME_PINK  ] = "COLOR1_PINK",
      [1][The_THEME_DARK  ] = "COLOR1_DARK",
     };

   Gbl.ColorRows[0] = The_ClassColorRows[0][Gbl.Prefs.Theme];	// Darker
   Gbl.ColorRows[1] = The_ClassColorRows[1][Gbl.Prefs.Theme];	// Lighter
  }
