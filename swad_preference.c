// swad_preference.c: user's preferences

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL
#include <stdbool.h>		// For boolean type
#include <stdio.h>		// For fprintf, etc.
#include <string.h>

#include "swad_calendar.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_layout.h"
#include "swad_notification.h"
#include "swad_menu.h"
#include "swad_parameter.h"
#include "swad_preference.h"
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

static void Pre_PutIconsLanguage (void);

static void Pre_PutIconsToSelectSideCols (void);
static void Pre_PutIconsSideColumns (void);
static void Pre_UpdateSideColsOnUsrDataTable (void);

/*****************************************************************************/
/***************************** Edit preferences ******************************/
/*****************************************************************************/

void Pre_EditPrefs (void)
  {
   extern const char *Hlp_PROFILE_Preferences_language;
   extern const char *Txt_Language;

   /***** Language, first day of week *****/
   fprintf (Gbl.F.Out,"<table style=\"margin:0 auto; border-spacing:16px 0;\">"
                      "<tr>"
                      "<td>");
   Lay_StartRoundFrame (NULL,Txt_Language,
                        Pre_PutIconsLanguage,Hlp_PROFILE_Preferences_language);
   fprintf (Gbl.F.Out,"<div style=\"height:42px;\">");
   Pre_PutSelectorToSelectLanguage ();		// 1. Language
   fprintf (Gbl.F.Out,"</div>");
   Lay_EndRoundFrame ();
   fprintf (Gbl.F.Out,"</td>"
                      "<td>");
   Cal_PutIconsToSelectFirstDayOfWeek ();	// 2. First day of week
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>"
	              "</table>");

   /***** Icon set, theme *****/
   fprintf (Gbl.F.Out,"<table style=\"margin:0 auto; border-spacing:16px 0;\">"
                      "<tr>"
                      "<td>");
   Ico_PutIconsToSelectIconSet ();		// 3. Icon set
   fprintf (Gbl.F.Out,"</td>"
		      "<td>");
   The_PutIconsToSelectTheme ();		// 4. Theme
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>"
	              "</table>");

   /***** Menu, side columns *****/
   fprintf (Gbl.F.Out,"<table style=\"margin:0 auto; border-spacing:16px 0;\">"
                      "<tr>"
                      "<td>");
   Mnu_PutIconsToSelectMenu ();			// 5. Menu
   fprintf (Gbl.F.Out,"</td>"
		      "<td>");
   Pre_PutIconsToSelectSideCols ();		// 6. Side columns
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>"
	              "</table>");

   if (Gbl.Usrs.Me.Logged)
     {
      /***** Form to set my preferences on privacy *****/
      Pri_EditMyPrivacy ();

      /***** Automatic e-mail to notify of new events *****/
      Ntf_PutFormChangeNotifSentByEMail ();
     }
  }

/*****************************************************************************/
/*************** Put contextual icons in language preference *****************/
/*****************************************************************************/

static void Pre_PutIconsLanguage (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_LANGUAGES;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/******************* Get preferences changed from current IP *****************/
/*****************************************************************************/

void Pre_GetPrefsFromIP (void)
  {
   extern const bool Cal_DayIsValidAsFirstDayOfWeek[7];
   char Query[1024];
   unsigned long NumRows;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;

   if (Gbl.IP[0])
     {
      /***** Get preferences from database *****/
      sprintf (Query,"SELECT FirstDayOfWeek,Theme,IconSet,Menu,SideCols"
		     " FROM IP_prefs WHERE IP='%s'",
	       Gbl.IP);
      if ((NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get preferences")))
	{
	 if (NumRows != 1)
	    Lay_ShowErrorAndExit ("Internal error while getting preferences.");

	 /***** Get preferences from database *****/
	 row = mysql_fetch_row (mysql_res);

	 /* Get first day of week (row[0]) */
	 Gbl.Prefs.FirstDayOfWeek = Cal_FIRST_DAY_OF_WEEK_DEFAULT;
	 if (sscanf (row[0],"%u",&UnsignedNum) == 1)
	    if (Cal_DayIsValidAsFirstDayOfWeek[UnsignedNum])
	       Gbl.Prefs.FirstDayOfWeek = UnsignedNum;

	 /* Get theme (row[1]) */
	 Gbl.Prefs.Theme = The_GetThemeFromStr (row[1]);

	 /* Get icon set (row[2]) */
	 Gbl.Prefs.IconSet = Ico_GetIconSetFromStr (row[2]);

	 /* Get menu (row[3]) */
	 Gbl.Prefs.Menu = Mnu_MENU_DEFAULT;
	 if (sscanf (row[3],"%u",&UnsignedNum) == 1)
	    if (UnsignedNum < Mnu_NUM_MENUS)
	       Gbl.Prefs.Menu = (Mnu_Menu_t) UnsignedNum;

	 /* Get if user wants to show side columns (row[4]) */
	 if (sscanf (row[4],"%u",&Gbl.Prefs.SideCols) == 1)
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
   char Query[512];

   /***** Update preferences from current IP in database *****/
   sprintf (Query,"REPLACE INTO IP_prefs (IP,UsrCod,LastChange,FirstDayOfWeek,Theme,IconSet,Menu,SideCols)"
                  " VALUES ('%s','%ld',NOW(),'%u','%s','%s','%u','%u')",
            Gbl.IP,Gbl.Usrs.Me.UsrDat.UsrCod,
            Gbl.Prefs.FirstDayOfWeek,
            The_ThemeId[Gbl.Prefs.Theme],
            Ico_IconSetId[Gbl.Prefs.IconSet],
            (unsigned) Gbl.Prefs.Menu,
            Gbl.Prefs.SideCols);
   DB_QueryREPLACE (Query,"can not store preferences from current IP address");

   /***** If a user is logged, update its preferences in database for all its IP's *****/
   if (Gbl.Usrs.Me.Logged)
     {
      sprintf (Query,"UPDATE IP_prefs SET FirstDayOfWeek='%u',Theme='%s',IconSet='%s',Menu='%u',SideCols='%u'"
                     " WHERE UsrCod='%ld'",
               Gbl.Prefs.FirstDayOfWeek,
               The_ThemeId[Gbl.Prefs.Theme],
               Ico_IconSetId[Gbl.Prefs.IconSet],
               (unsigned) Gbl.Prefs.Menu,
               Gbl.Prefs.SideCols,
               Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryUPDATE (Query,"can not update your preferences");
     }
  }

/*****************************************************************************/
/*********************** Remove old preferences from IP **********************/
/*****************************************************************************/

void Pre_RemoveOldPrefsFromIP (void)
  {
   char Query[256];

   /***** Remove old preferences *****/
   sprintf (Query,"DELETE LOW_PRIORITY FROM IP_prefs"
                  " WHERE LastChange<FROM_UNIXTIME(UNIX_TIMESTAMP()-'%lu')",
            Cfg_TIME_TO_DELETE_IP_PREFS);
   DB_QueryDELETE (Query,"can not remove old preferences");
  }

/*****************************************************************************/
/************** Put link to change language (edit preferences) ***************/
/*****************************************************************************/

void Pre_PutLinkToChangeLanguage (void)
  {
   Lay_PutContextualLink (ActEdiPrf,NULL,
                          "cty64x64.gif",
                          "Change language","Change language",
		          NULL);
  }

/*****************************************************************************/
/********************* Put a selector to select language *********************/
/*****************************************************************************/
// Width == 0 means don't force width of selector

void Pre_PutSelectorToSelectLanguage (void)
  {
   extern const char *Txt_STR_LANG_NAME[1+Txt_NUM_LANGUAGES];
   Txt_Language_t Lan;

   Act_FormStart (ActReqChgLan);
   fprintf (Gbl.F.Out,"<select name=\"Lan\""
	              " style=\"width:112px; margin:0;\""
	              " onchange=\"document.getElementById('%s').submit();\">",
            Gbl.Form.Id);
   for (Lan = (Txt_Language_t) 1;
	Lan <= Txt_NUM_LANGUAGES;
	Lan++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) Lan);
      if (Lan == Gbl.Prefs.Language)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",Txt_STR_LANG_NAME[Lan]);
     }
   fprintf (Gbl.F.Out,"</select>");
   Act_FormEnd ();
  }

/*****************************************************************************/
/********* Ask user if he/she really wants to change the language ************/
/*****************************************************************************/

void Pre_AskChangeLanguage (void)
  {
   extern const char *Txt_Do_you_want_to_change_your_language_to_LANGUAGE[1+Txt_NUM_LANGUAGES];
   extern const char *Txt_Do_you_want_to_change_the_language_to_LANGUAGE[1+Txt_NUM_LANGUAGES];
   extern const char *Txt_Switch_to_LANGUAGE[1+Txt_NUM_LANGUAGES];
   Txt_Language_t CurrentLanguage = Gbl.Prefs.Language;

   /***** Get param language *****/
   Gbl.Prefs.Language = Pre_GetParamLanguage ();	// Change temporarily language to set form action

   /***** Request confirmation *****/
   if (Gbl.Usrs.Me.Logged)
   Lay_ShowAlert (Lay_INFO,
                  Gbl.Usrs.Me.Logged ? Txt_Do_you_want_to_change_your_language_to_LANGUAGE[Gbl.Prefs.Language] :
	                               Txt_Do_you_want_to_change_the_language_to_LANGUAGE[Gbl.Prefs.Language]);

   /***** Send button *****/
   Act_FormStart (ActChgLan);
   Par_PutHiddenParamUnsigned ("Lan",(unsigned) Gbl.Prefs.Language);
   Lay_PutConfirmButton (Txt_Switch_to_LANGUAGE[Gbl.Prefs.Language]);
   Act_FormEnd ();

   Gbl.Prefs.Language = CurrentLanguage;		// Restore current language
  }

/*****************************************************************************/
/******************************* Change language *****************************/
/*****************************************************************************/

void Pre_ChangeLanguage (void)
  {
   /***** Get param language *****/
   Gbl.Prefs.Language = Pre_GetParamLanguage ();

   /***** Store language in database *****/
   /*
   sprintf (Gbl.Message,"Txt_STR_LANG_ID[Gbl.Prefs.Language] = %s",Txt_STR_LANG_ID[Gbl.Prefs.Language]);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);
   sprintf (Gbl.Message,"Txt_STR_LANG_ID[Gbl.Usrs.Me.UsrDat.Prefs.Language] = %s",Txt_STR_LANG_ID[Gbl.Usrs.Me.UsrDat.Prefs.Language]);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);
   */

   if (Gbl.Usrs.Me.Logged &&
       Gbl.Prefs.Language != Gbl.Usrs.Me.UsrDat.Prefs.Language)
     Pre_UpdateMyLanguageToCurrentLanguage ();

   /***** Set preferences from current IP *****/
   Pre_SetPrefsFromIP ();
  }

/*****************************************************************************/
/**************** Update my language to the current language *****************/
/*****************************************************************************/

void Pre_UpdateMyLanguageToCurrentLanguage (void)
  {
   extern const char *Txt_STR_LANG_ID[1+Txt_NUM_LANGUAGES];
   char Query[128];

   /***** Set my language to the current language *****/
   Gbl.Usrs.Me.UsrDat.Prefs.Language = Gbl.Prefs.Language;

   /***** Update my language in database *****/
   sprintf (Query,"UPDATE usr_data SET Language='%s' WHERE UsrCod='%ld'",
	    Txt_STR_LANG_ID[Gbl.Prefs.Language],
	    Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update your language");
  }

/*****************************************************************************/
/*************************** Get parameter language **************************/
/*****************************************************************************/

Txt_Language_t Pre_GetParamLanguage (void)
  {
   extern const unsigned Txt_Current_CGI_SWAD_Language;
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("Lan",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      if (UnsignedNum >= 1 &&
	  UnsignedNum <= Txt_NUM_LANGUAGES)
         return (Txt_Language_t) UnsignedNum;

   return Txt_Current_CGI_SWAD_Language;
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

   Lay_StartRoundFrame (NULL,Txt_Columns,
                        Pre_PutIconsSideColumns,
                        Hlp_PROFILE_Preferences_columns);
   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\" style=\"margin:0 auto;\">"
                      "<tr>");
   for (SideCols = 0;
	SideCols <= Lay_SHOW_BOTH_COLUMNS;
	SideCols++)
     {
      fprintf (Gbl.F.Out,"<td class=\"%s CENTER_MIDDLE\">",
               SideCols == Gbl.Prefs.SideCols ? "PREF_ON" :
        	                                "PREF_OFF");
      Act_FormStart (ActChgCol);
      Par_PutHiddenParamUnsigned ("SideCols",SideCols);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/layout%u%u_32x20.gif\""
	                 " alt=\"%s\" title=\"%s\""
	                 " style=\"display:block;"
	                 " width:40px; height:25px;\" />",
               Gbl.Prefs.IconsURL,
               SideCols >> 1,SideCols & 1,
               Txt_LAYOUT_SIDE_COLUMNS[SideCols],
               Txt_LAYOUT_SIDE_COLUMNS[SideCols]);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");
     }
   fprintf (Gbl.F.Out,"</tr>"
	              "</table>");
   Lay_EndRoundFrame ();
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
   char Query[512];

   sprintf (Query,"UPDATE usr_data SET SideCols='%u'"
	          " WHERE UsrCod='%ld'",
            Gbl.Prefs.SideCols,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update your preference about side columns");
  }

/*****************************************************************************/
/************** Get parameter used to show/hide side columns *****************/
/*****************************************************************************/

unsigned Pre_GetParamSideCols (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;	// 11 ==> by default, show both side columns

   Par_GetParToText ("SideCols",UnsignedStr,10);

   if (!UnsignedStr[0])
      return Cfg_DEFAULT_COLUMNS;

   if (sscanf (UnsignedStr,"%u",&UnsignedNum) != 1)
      return Cfg_DEFAULT_COLUMNS;

   if (UnsignedNum > 3)
      return Cfg_DEFAULT_COLUMNS;

   return UnsignedNum;
  }
