// swad_preference.c: user's preferences

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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
#include <linux/stddef.h>	// For NULL
#include <stdbool.h>		// For boolean type
#include <stdio.h>		// For asprintf, fprintf, etc.
#include <string.h>

#include "swad_box.h"
#include "swad_calendar.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_date.h"
#include "swad_global.h"
#include "swad_language.h"
#include "swad_layout.h"
#include "swad_notification.h"
#include "swad_menu.h"
#include "swad_parameter.h"
#include "swad_preference.h"
#include "swad_privacy.h"
#include "swad_text.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Private prototypes ***************************/
/*****************************************************************************/

static void Pre_PutIconsToSelectSideCols (void);
static void Pre_PutIconsSideColumns (void);
static void Pre_UpdateSideColsOnUsrDataTable (void);

/*****************************************************************************/
/***************************** Edit preferences ******************************/
/*****************************************************************************/

void Pre_EditPrefs (void)
  {
   extern const char *Hlp_PROFILE_Preferences_internationalization;
   extern const char *Hlp_PROFILE_Preferences_design;
   extern const char *Txt_Internationalization;
   extern const char *Txt_Design;

   /***** Internationalization: language, first day of week, date format *****/
   Box_StartBox (NULL,Txt_Internationalization,NULL,
                 Hlp_PROFILE_Preferences_internationalization,Box_NOT_CLOSABLE);
   fprintf (Gbl.F.Out,"<div class=\"FRAME_INLINE\">");
   Lan_PutBoxToSelectLanguage ();		// 1. Language
   fprintf (Gbl.F.Out,"</div>"
                      "<div class=\"FRAME_INLINE\">");
   Cal_PutIconsToSelectFirstDayOfWeek ();	// 2. First day of week
   fprintf (Gbl.F.Out,"</div>"
                      "<div class=\"FRAME_INLINE\">");
   Dat_PutBoxToSelectDateFormat ();		// 3. Date format
   fprintf (Gbl.F.Out,"</div>");
   Box_EndBox ();

   /***** Design: icon set, menu, theme, side columns *****/
   Box_StartBox (NULL,Txt_Design,NULL,
                 Hlp_PROFILE_Preferences_design,Box_NOT_CLOSABLE);
   fprintf (Gbl.F.Out,"<div class=\"FRAME_INLINE\">");
   Ico_PutIconsToSelectIconSet ();		// 4. Icon set
   fprintf (Gbl.F.Out,"</div>"
                      "<div class=\"FRAME_INLINE\">");
   Mnu_PutIconsToSelectMenu ();			// 5. Menu
   fprintf (Gbl.F.Out,"</div>"
	              "<div class=\"FRAME_INLINE\">");
   The_PutIconsToSelectTheme ();		// 6. Theme
   fprintf (Gbl.F.Out,"</div>"
                      "<div class=\"FRAME_INLINE\">");
   Pre_PutIconsToSelectSideCols ();		// 7. Side columns
   fprintf (Gbl.F.Out,"</div>");
   Box_EndBox ();

   if (Gbl.Usrs.Me.Logged)
     {
      /***** Form to set my preferences on privacy *****/
      Pri_EditMyPrivacy ();

      /***** Automatic email to notify of new events *****/
      Ntf_PutFormChangeNotifSentByEMail ();
     }
  }

/*****************************************************************************/
/******************* Get preferences changed from current IP *****************/
/*****************************************************************************/

void Pre_GetPrefsFromIP (void)
  {
   unsigned long NumRows;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (Gbl.IP[0])
     {
      /***** Get preferences from database *****/
      DB_BuildQuery ("SELECT FirstDayOfWeek,DateFormat,Theme,IconSet,Menu,SideCols"
		     " FROM IP_prefs WHERE IP='%s'",
	             Gbl.IP);
      if ((NumRows = DB_QuerySELECT_new (&mysql_res,"can not get preferences")))
	{
	 if (NumRows != 1)
	    Lay_ShowErrorAndExit ("Internal error while getting preferences.");

	 /***** Get preferences from database *****/
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
/************************ Set preferences from current IP ********************/
/*****************************************************************************/

void Pre_SetPrefsFromIP (void)
  {
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];

   /***** Update preferences from current IP in database *****/
   DB_BuildQuery ("REPLACE INTO IP_prefs"
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
   DB_QueryREPLACE_new ("can not store preferences from current IP address");

   /***** If a user is logged, update its preferences in database for all its IP's *****/
   if (Gbl.Usrs.Me.Logged)
     {
      DB_BuildQuery ("UPDATE IP_prefs"
		    " SET FirstDayOfWeek=%u,DateFormat=%u,"
		    "Theme='%s',IconSet='%s',Menu=%u,SideCols=%u"
		     " WHERE UsrCod=%ld",
		     Gbl.Prefs.FirstDayOfWeek,
		     (unsigned) Gbl.Prefs.DateFormat,
		     The_ThemeId[Gbl.Prefs.Theme],
		     Ico_IconSetId[Gbl.Prefs.IconSet],
		     (unsigned) Gbl.Prefs.Menu,
		     Gbl.Prefs.SideCols,
		     Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryUPDATE_new ("can not update your preferences");
     }
  }

/*****************************************************************************/
/*********************** Remove old preferences from IP **********************/
/*****************************************************************************/

void Pre_RemoveOldPrefsFromIP (void)
  {
   char *Query;

   /***** Remove old preferences *****/
   if (asprintf (&Query,"DELETE LOW_PRIORITY FROM IP_prefs"
			" WHERE LastChange<FROM_UNIXTIME(UNIX_TIMESTAMP()-'%lu')",
                 Cfg_TIME_TO_DELETE_IP_PREFS) < 0)
      Lay_NotEnoughMemoryExit ();
   DB_QueryDELETE_free (Query,"can not remove old preferences");
  }

/*****************************************************************************/
/************ Put icons to select the layout of the side columns *************/
/*****************************************************************************/

static void Pre_PutIconsToSelectSideCols (void)
  {
   extern const char *Hlp_PROFILE_Preferences_columns;
   extern const char *Txt_Columns;
   extern const char *Txt_LAYOUT_SIDE_COLUMNS[4];
   unsigned SideCols;

   Box_StartBox (NULL,Txt_Columns,Pre_PutIconsSideColumns,
                 Hlp_PROFILE_Preferences_columns,Box_NOT_CLOSABLE);
   fprintf (Gbl.F.Out,"<div class=\"PREF_CONTAINER\">");
   for (SideCols = 0;
	SideCols <= Lay_SHOW_BOTH_COLUMNS;
	SideCols++)
     {
      fprintf (Gbl.F.Out,"<div class=\"%s\">",
               SideCols == Gbl.Prefs.SideCols ? "PREF_ON" :
        	                                "PREF_OFF");
      Act_StartForm (ActChgCol);
      Par_PutHiddenParamUnsigned ("SideCols",SideCols);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/layout%u%u_32x20.gif\""
	                 " alt=\"%s\" title=\"%s\" class=\"ICO40x25\" />",
               Gbl.Prefs.IconsURL,
               SideCols >> 1,SideCols & 1,
               Txt_LAYOUT_SIDE_COLUMNS[SideCols],
               Txt_LAYOUT_SIDE_COLUMNS[SideCols]);
      Act_EndForm ();
      fprintf (Gbl.F.Out,"</div>");
     }
   fprintf (Gbl.F.Out,"</div>");
   Box_EndBox ();
  }

/*****************************************************************************/
/************* Put contextual icons in side-columns preference ***************/
/*****************************************************************************/

static void Pre_PutIconsSideColumns (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_SIDE_COLUMNS;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/*********************** Change layout of side columns ***********************/
/*****************************************************************************/

void Pre_ChangeSideCols (void)
  {
   /***** Get param side-columns *****/
   Gbl.Prefs.SideCols = Pre_GetParamSideCols ();

   /***** Store side colums in database *****/
   if (Gbl.Usrs.Me.Logged)
      Pre_UpdateSideColsOnUsrDataTable ();

   /***** Set preferences from current IP *****/
   Pre_SetPrefsFromIP ();
  }

/*****************************************************************************/
/*************************** Hide left side column ***************************/
/*****************************************************************************/

void Pre_HideLeftCol (void)
  {
   Gbl.Prefs.SideCols &= ~Lay_SHOW_LEFT_COLUMN;	//  And with 1...101 to hide left column
   if (Gbl.Usrs.Me.Logged)
      Pre_UpdateSideColsOnUsrDataTable ();

   /***** Set preferences from current IP *****/
   Pre_SetPrefsFromIP ();
  }

/*****************************************************************************/
/*************************** Hide right side column **************************/
/*****************************************************************************/

void Pre_HideRightCol (void)
  {
   Gbl.Prefs.SideCols &= ~Lay_SHOW_RIGHT_COLUMN;	//  And with 1...110 to hide right column
   if (Gbl.Usrs.Me.Logged)
      Pre_UpdateSideColsOnUsrDataTable ();

   /***** Set preferences from current IP *****/
   Pre_SetPrefsFromIP ();
  }

/*****************************************************************************/
/**************************** Show left side column **************************/
/*****************************************************************************/

void Pre_ShowLeftCol (void)
  {
   Gbl.Prefs.SideCols |= Lay_SHOW_LEFT_COLUMN;	// Or with 10 to show left column
   if (Gbl.Usrs.Me.Logged)
      Pre_UpdateSideColsOnUsrDataTable ();

   /***** Set preferences from current IP *****/
   Pre_SetPrefsFromIP ();
  }

/*****************************************************************************/
/**************************** Show right side column *************************/
/*****************************************************************************/

void Pre_ShowRightCol (void)
  {
   Gbl.Prefs.SideCols |= Lay_SHOW_RIGHT_COLUMN;	// Or with 01 to show right column
   if (Gbl.Usrs.Me.Logged)
      Pre_UpdateSideColsOnUsrDataTable ();

   /***** Set preferences from current IP *****/
   Pre_SetPrefsFromIP ();
  }

/*****************************************************************************/
/************** Update layout of side colums on user data table **************/
/*****************************************************************************/

static void Pre_UpdateSideColsOnUsrDataTable (void)
  {
   DB_BuildQuery ("UPDATE usr_data SET SideCols=%u"
		  " WHERE UsrCod=%ld",
		  Gbl.Prefs.SideCols,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE_new ("can not update your preference about side columns");
  }

/*****************************************************************************/
/************** Get parameter used to show/hide side columns *****************/
/*****************************************************************************/

unsigned Pre_GetParamSideCols (void)
  {
   return (unsigned) Par_GetParToUnsignedLong ("SideCols",
                                               0,
                                               Lay_SHOW_BOTH_COLUMNS,
                                               Cfg_DEFAULT_COLUMNS);
  }
