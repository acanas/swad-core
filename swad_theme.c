// swad_theme.c: themes (colour layouts)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_layout.h"
#include "swad_parameter.h"
#include "swad_setting.h"
#include "swad_setting_database.h"
#include "swad_theme.h"
#include "swad_user_database.h"

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
	       Set_BeginPref (Theme == Gbl.Prefs.Theme);
		  Frm_BeginForm (ActChgThe);
		     Par_PutParString (NULL,"Theme",The_ThemeId[Theme]);
		     snprintf (Icon,sizeof (Icon),"%s/%s/theme_32x20.gif",
			       Cfg_ICON_FOLDER_THEMES,The_ThemeId[Theme]);
		     Ico_PutSettingIconLink (Icon,Ico_UNCHANGED,The_ThemeNames[Theme]);
		  Frm_EndForm ();
	       Set_EndPref ();
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
   /***** Get param theme *****/
   Gbl.Prefs.Theme = The_GetParTheme ();

   /***** Store theme in database *****/
   if (Gbl.Usrs.Me.Logged)
      Set_DB_UpdateMySettingsAboutTheme (The_ThemeId[Gbl.Prefs.Theme]);

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/***************************** Get parameter theme ***************************/
/*****************************************************************************/

The_Theme_t The_GetParTheme (void)
  {
   char ThemeId[The_MAX_BYTES_THEME_ID + 1];
   The_Theme_t Theme;

   Par_GetParText ("Theme",ThemeId,The_MAX_BYTES_THEME_ID);
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
/******************* Get theme name for CSS class suffix *********************/
/*****************************************************************************/

static const char *The_CSS_Suffix[The_NUM_THEMES] =
  {
   [The_THEME_WHITE ] = "WHITE",
   [The_THEME_GREY  ] = "GREY",
   [The_THEME_PURPLE] = "PURPLE",
   [The_THEME_BLUE  ] = "BLUE",
   [The_THEME_YELLOW] = "YELLOW",
   [The_THEME_PINK  ] = "PINK",
   [The_THEME_DARK  ] = "DARK",
  };

const char *The_GetSuffix (void)
  {
   return The_CSS_Suffix[Gbl.Prefs.Theme];
  }

/*****************************************************************************/
/******** Get background colors for rows depending on selected theme *********/
/*****************************************************************************/

static unsigned The_RowEvenOdd[3] =	// To alternate row colors in listings
  {
   0,
   0,
   0,
  };

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

const char *The_GetColorRows (void)
  {
   return The_GetColorRows1 (0);
  }

const char *The_GetColorRows1 (unsigned Level)
  {
   return The_ClassColorRows[The_RowEvenOdd[Level]][Gbl.Prefs.Theme];
  }

void The_ResetRowColor (void)
  {
   The_ResetRowColor1 (0);
  }

void The_ResetRowColor1 (unsigned Level)
  {
   The_RowEvenOdd[Level] = 0;
  }

void The_ChangeRowColor (void)
  {
   The_ChangeRowColor1 (0);
  }

void The_ChangeRowColor1 (unsigned Level)
  {
   The_RowEvenOdd[Level] = 1 - The_RowEvenOdd[Level];
  }

/*****************************************************************************/
/********** Get and show number of users who have chosen a theme *************/
/*****************************************************************************/

void The_GetAndShowNumUsrsPerTheme (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_theme;
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *The_ThemeNames[The_NUM_THEMES];
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Theme_SKIN;
   extern const char *Txt_Number_of_users;
   extern const char *Txt_PERCENT_of_users;
   The_Theme_t Theme;
   char *SubQuery;
   char *URL;
   unsigned NumUsrs[The_NUM_THEMES];
   unsigned NumUsrsTotal = 0;

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_FIGURE_TYPES[Fig_THEMES],
                      NULL,NULL,
                      Hlp_ANALYTICS_Figures_theme,Box_NOT_CLOSABLE,2);

      /***** Heading row *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Theme_SKIN      ,HTM_HEAD_LEFT);
	 HTM_TH (Txt_Number_of_users ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_PERCENT_of_users,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** For each theme... *****/
      for (Theme  = (The_Theme_t) 0;
	   Theme <= (The_Theme_t) (The_NUM_THEMES - 1);
	   Theme++)
	{
	 /* Get number of users who have chosen this theme from database */
	 if (asprintf (&SubQuery,"usr_data.Theme='%s'",
		       The_ThemeId[Theme]) < 0)
	    Err_NotEnoughMemoryExit ();
	 NumUsrs[Theme] = Usr_DB_GetNumUsrsWhoChoseAnOption (SubQuery);
	 free (SubQuery);

	 /* Update total number of users */
	 NumUsrsTotal += NumUsrs[Theme];
	}

      /***** Write number of users who have chosen each theme *****/
      for (Theme  = (The_Theme_t) 0;
	   Theme <= (The_Theme_t) (The_NUM_THEMES - 1);
	   Theme++)
	{
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"CM\"");
	       if (asprintf (&URL,"%s/%s",
			     Cfg_URL_ICON_THEMES_PUBLIC,The_ThemeId[Theme]) < 0)
		  Err_NotEnoughMemoryExit ();
	       HTM_IMG (URL,"theme_32x20.gif",The_ThemeNames[Theme],
			"style=\"width:40px;height:25px;\"");
	       free (URL);
	    HTM_TD_End ();

	    HTM_TD_Unsigned (NumUsrs[Theme]);
	    HTM_TD_Percentage (NumUsrs[Theme],NumUsrsTotal);

	 HTM_TR_End ();
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
