// swad_setting.c: user's settings / preferences

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

#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <string.h>

#include "swad_box.h"
#include "swad_calendar.h"
#include "swad_config.h"
#include "swad_cookie.h"
#include "swad_database.h"
#include "swad_date.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_layout.h"
#include "swad_notification.h"
#include "swad_menu.h"
#include "swad_parameter.h"
#include "swad_privacy.h"
#include "swad_setting.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Set_PutIconsToSelectSideCols (void);
static void Set_PutIconsSideColumns (__attribute__((unused)) void *Args);
static void Set_UpdateSideColsOnUsrDataTable (void);

/*****************************************************************************/
/***************************** Edit settings ******************************/
/*****************************************************************************/

void Set_EditSettings (void)
  {
   extern const char *Hlp_PROFILE_Settings_internationalization;
   extern const char *Hlp_PROFILE_Settings_design;
   extern const char *Txt_Internationalization;
   extern const char *Txt_Design;

   /***** Internationalization: language, first day of week, date format *****/
   Box_BoxBegin (NULL,Txt_Internationalization,
                 NULL,NULL,
                 Hlp_PROFILE_Settings_internationalization,Box_NOT_CLOSABLE);

   HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
   Lan_PutBoxToSelectLanguage ();		// 1. Language
   HTM_DIV_End ();

   HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
   Cal_PutIconsToSelectFirstDayOfWeek ();	// 2. First day of week
   HTM_DIV_End ();

   HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
   Dat_PutBoxToSelectDateFormat ();		// 3. Date format
   HTM_DIV_End ();

   Box_BoxEnd ();

   /***** Design: icon set, menu, theme, side columns *****/
   Box_BoxBegin (NULL,Txt_Design,
                 NULL,NULL,
                 Hlp_PROFILE_Settings_design,Box_NOT_CLOSABLE);

   HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
   Ico_PutIconsToSelectIconSet ();		// 4. Icon set
   HTM_DIV_End ();

   HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
   Mnu_PutIconsToSelectMenu ();			// 5. Menu
   HTM_DIV_End ();

   HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
   The_PutIconsToSelectTheme ();		// 6. Theme
   HTM_DIV_End ();

   HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
   Set_PutIconsToSelectSideCols ();		// 7. Side columns
   HTM_DIV_End ();

   Box_BoxEnd ();

   if (Gbl.Usrs.Me.Logged)
     {
      /***** Form to set my settings on privacy *****/
      Pri_EditMyPrivacy ();

      /***** Form to set my settings on cookies *****/
      Coo_EditMyPrefsOnCookies ();

      /***** Automatic email to notify of new events *****/
      Ntf_PutFormChangeNotifSentByEMail ();
     }
  }

/*****************************************************************************/
/******************** Get settings changed from current IP *******************/
/*****************************************************************************/

void Set_GetSettingsFromIP (void)
  {
   unsigned NumRows;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (Gbl.IP[0])
     {
      /***** Get settings from database *****/
      NumRows = (unsigned)
      DB_QuerySELECT (&mysql_res,"can not get settings",
		      "SELECT FirstDayOfWeek,"	// row[0]
			     "DateFormat,"	// row[1]
			     "Theme,"		// row[2]
			     "IconSet,"		// row[3]
			     "Menu,"		// row[4]
			     "SideCols"		// row[5]
		       " FROM set_ip_settings"
		      " WHERE IP='%s'",
		      Gbl.IP);
      if (NumRows)
	{
	 if (NumRows != 1)
	    Lay_ShowErrorAndExit ("Internal error while getting settings.");

	 /***** Get settings from database *****/
	 row = mysql_fetch_row (mysql_res);

	 /* Get first day of week (row[0]) */
	 Gbl.Prefs.FirstDayOfWeek = Cal_GetFirstDayOfWeekFromStr (row[0]);

	 /* Get date format (row[1]) */
	 Gbl.Prefs.DateFormat = Dat_GetDateFormatFromStr (row[1]);

	 /* Get theme (row[2]) */
	 Gbl.Prefs.Theme = The_GetThemeFromStr (row[2]);

	 /* Get icon set (row[3]) */
	 Gbl.Prefs.IconSet = Ico_GetIconSetFromStr (row[3]);

	 /* Get menu (row[4]) */
	 Gbl.Prefs.Menu = Mnu_GetMenuFromStr (row[4]);

	 /* Get if user wants to show side columns (row[5]) */
	 if (sscanf (row[5],"%u",&Gbl.Prefs.SideCols) == 1)
	   {
	    if (Gbl.Prefs.SideCols > Lay_SHOW_BOTH_COLUMNS)
	       Gbl.Prefs.SideCols = Cfg_DEFAULT_COLUMNS;
	   }
	 else
	    Gbl.Prefs.SideCols = Cfg_DEFAULT_COLUMNS;
	}
     }
  }

/*****************************************************************************/
/************************ Set settings from current IP ***********************/
/*****************************************************************************/

void Set_SetSettingsFromIP (void)
  {
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];

   /***** Update settings from current IP in database *****/
   DB_QueryREPLACE ("can not store settings from current IP address",
		    "REPLACE INTO set_ip_settings"
		    " (IP,UsrCod,LastChange,"
		    "FirstDayOfWeek,DateFormat,Theme,IconSet,Menu,SideCols)"
		    " VALUES"
		    " ('%s',%ld,NOW(),"
		    "%u,%u,'%s','%s',%u,%u)",
	            Gbl.IP,Gbl.Usrs.Me.UsrDat.UsrCod,
	            Gbl.Prefs.FirstDayOfWeek,
	            (unsigned) Gbl.Prefs.DateFormat,
	            The_ThemeId[Gbl.Prefs.Theme],
	            Ico_IconSetId[Gbl.Prefs.IconSet],
	            (unsigned) Gbl.Prefs.Menu,
	            Gbl.Prefs.SideCols);

   /***** If a user is logged, update its settings in database for all its IP's *****/
   if (Gbl.Usrs.Me.Logged)
      DB_QueryUPDATE ("can not update your settings",
		      "UPDATE set_ip_settings"
		        " SET FirstDayOfWeek=%u,"
		             "DateFormat=%u,"
		             "Theme='%s',"
		             "IconSet='%s',"
		             "Menu=%u,"
		             "SideCols=%u"
		      " WHERE UsrCod=%ld",
		      Gbl.Prefs.FirstDayOfWeek,
		      (unsigned) Gbl.Prefs.DateFormat,
		      The_ThemeId[Gbl.Prefs.Theme],
		      Ico_IconSetId[Gbl.Prefs.IconSet],
		      (unsigned) Gbl.Prefs.Menu,
		      Gbl.Prefs.SideCols,
		      Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*********************** Remove old settings from IP *************************/
/*****************************************************************************/

void Set_RemoveOldSettingsFromIP (void)
  {
   /***** Remove old settings *****/
   DB_QueryDELETE ("can not remove old settings",
		   "DELETE LOW_PRIORITY FROM set_ip_settings"
		   " WHERE LastChange<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Cfg_TIME_TO_DELETE_IP_PREFS);
  }

/*****************************************************************************/
/************ Put icons to select the layout of the side columns *************/
/*****************************************************************************/

static void Set_PutIconsToSelectSideCols (void)
  {
   extern const char *Hlp_PROFILE_Settings_columns;
   extern const char *Txt_Columns;
   extern const char *Txt_LAYOUT_SIDE_COLUMNS[4];
   unsigned SideCols;
   char Icon[32 + 1];

   Box_BoxBegin (NULL,Txt_Columns,
                 Set_PutIconsSideColumns,NULL,
                 Hlp_PROFILE_Settings_columns,Box_NOT_CLOSABLE);
   Set_BeginSettingsHead ();
   Set_BeginOneSettingSelector ();
   for (SideCols = 0;
	SideCols <= Lay_SHOW_BOTH_COLUMNS;
	SideCols++)
     {
      HTM_DIV_Begin ("class=\"%s\"",SideCols == Gbl.Prefs.SideCols ? "PREF_ON" :
        	                                                     "PREF_OFF");
      Frm_BeginForm (ActChgCol);
      Par_PutHiddenParamUnsigned (NULL,"SideCols",SideCols);
      snprintf (Icon,sizeof (Icon),"layout%u%u_32x20.gif",
                SideCols >> 1,SideCols & 1);
      Ico_PutSettingIconLink (Icon,Txt_LAYOUT_SIDE_COLUMNS[SideCols]);
      Frm_EndForm ();
      HTM_DIV_End ();
     }
   Set_EndOneSettingSelector ();
   Set_EndSettingsHead ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************** Put contextual icons in side-columns setting *****************/
/*****************************************************************************/

static void Set_PutIconsSideColumns (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_SIDE_COLUMNS);
  }

/*****************************************************************************/
/*********************** Change layout of side columns ***********************/
/*****************************************************************************/

void Set_ChangeSideCols (void)
  {
   /***** Get param side-columns *****/
   Gbl.Prefs.SideCols = Set_GetParamSideCols ();

   /***** Store side colums in database *****/
   if (Gbl.Usrs.Me.Logged)
      Set_UpdateSideColsOnUsrDataTable ();

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/************** Update layout of side colums on user data table **************/
/*****************************************************************************/

static void Set_UpdateSideColsOnUsrDataTable (void)
  {
   DB_QueryUPDATE ("can not update your setting about side columns",
		   "UPDATE usr_data SET SideCols=%u"
		   " WHERE UsrCod=%ld",
		   Gbl.Prefs.SideCols,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************** Get parameter used to show/hide side columns *****************/
/*****************************************************************************/

unsigned Set_GetParamSideCols (void)
  {
   return (unsigned) Par_GetParToUnsignedLong ("SideCols",
                                               0,
                                               Lay_SHOW_BOTH_COLUMNS,
                                               Cfg_DEFAULT_COLUMNS);
  }

/*****************************************************************************/
/*********** Head to select one or several settings using icons **************/
/*****************************************************************************/
/*
                +-- Container for several pref. selectors ---+
                |  +One pref.selector+  +One pref.selector+  |
                |  | +-----+ +-----+ |  | +-----+ +-----+ |  |
                |  | |Icon | |Icon | |  | |Icon | |Icon | |  |
                |  | |opt.A| |opt.B| |  | |opt.A| |opt.B| |  |
                |  | +-----+ +-----+ |  | +-----+ +-----+ |  |
                |  +-----------------+  +-----------------+  |
                +--------------------------------------------+
*/
void Set_BeginSettingsHead (void)
  {
   HTM_DIV_Begin ("class=\"PREF_CONTS\"");
  }

void Set_EndSettingsHead (void)
  {
   HTM_DIV_End ();
  }

void Set_BeginOneSettingSelector (void)
  {
   HTM_DIV_Begin ("class=\"PREF_CONT\"");
  }

void Set_EndOneSettingSelector (void)
  {
   HTM_DIV_End ();
  }
