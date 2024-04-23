// swad_calendar.c: Draw month and calendar

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_calendar.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy_type.h"
#include "swad_holiday.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_setting.h"
#include "swad_setting_database.h"
#include "swad_user_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

bool Cal_DayIsValidAsFirstDayOfWeek[7] =
  {
   [0] = true,	// monday
   [1] = false,	// tuesday
   [2] = false,	// wednesday
   [3] = false,	// thursday
   [4] = false,	// friday
   [5] = false,	// saturday
   [6] = true,	// sunday
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Cal_PutIconsFirstDayOfWeek (__attribute__((unused)) void *Args);

static unsigned Cal_GetParFirstDayOfWeek (void);

static void Cal_DrawCalendar (Act_Action_t ActionSeeCalendar,
                              Act_Action_t ActionChangeCalendar1stDay,
                              void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
                              Vie_ViewType_t ViewType);
static void Cal_PutIconsCalendar (__attribute__((unused)) void *Args);

/*****************************************************************************/
/************** Put icons to select the first day of the week ****************/
/*****************************************************************************/

void Cal_PutIconsToSelectFirstDayOfWeek (void)
  {
   extern const char *Hlp_PROFILE_Settings_calendar;
   extern const char *Txt_Calendar;

   Box_BoxBegin (Txt_Calendar,Cal_PutIconsFirstDayOfWeek,NULL,
                 Hlp_PROFILE_Settings_calendar,Box_NOT_CLOSABLE);
      Set_BeginSettingsHead ();
	 Cal_ShowFormToSelFirstDayOfWeek (ActChg1stDay,
					  NULL,NULL);
      Set_EndSettingsHead ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************ Put contextual icons in first-day-of-week setting **************/
/*****************************************************************************/

static void Cal_PutIconsFirstDayOfWeek (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_FIRST_DAY_OF_WEEK);
  }

/*****************************************************************************/
/************** Show form to select the first day of the week ****************/
/*****************************************************************************/

void Cal_ShowFormToSelFirstDayOfWeek (Act_Action_t Action,
                                      void (*FuncPars) (void *Args),void *Args)
  {
   extern const char *Txt_First_day_of_the_week_X;
   extern const char *Txt_DAYS[7];
   unsigned FirstDayOfWeek;
   char *Title;
   char Icon[32 + 1];

   Set_BeginOneSettingSelector ();
      for (FirstDayOfWeek  = 0;	// Monday
	   FirstDayOfWeek <= 6;	// Sunday
	   FirstDayOfWeek++)
	 if (Cal_DayIsValidAsFirstDayOfWeek[FirstDayOfWeek])
	   {
	    Set_BeginPref (FirstDayOfWeek == Gbl.Prefs.FirstDayOfWeek);
	       Frm_BeginForm (Action);
		  Par_PutParUnsigned (NULL,"FirstDayOfWeek",FirstDayOfWeek);
		  if (FuncPars)	// Extra parameters depending on the action
		     FuncPars (Args);
		  snprintf (Icon,sizeof (Icon),"first-day-of-week-%u.png",FirstDayOfWeek);
		  if (asprintf (&Title,Txt_First_day_of_the_week_X,
				Txt_DAYS[FirstDayOfWeek]) < 0)
		     Err_NotEnoughMemoryExit ();
		  Ico_PutSettingIconLink (Icon,Ico_BLACK,Title);
		  free (Title);
	       Frm_EndForm ();
	    Set_EndPref ();
	   }
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/************************* Change first day of week **************************/
/*****************************************************************************/

void Cal_Change1stDayOfWeek (void)
  {
   /***** Get param with icon set *****/
   Gbl.Prefs.FirstDayOfWeek = Cal_GetParFirstDayOfWeek ();

   /***** Store icon first day of week database *****/
   if (Gbl.Usrs.Me.Logged)
      Set_DB_UpdateMySettingsAboutFirstDayOfWeek (Gbl.Prefs.FirstDayOfWeek);

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/*********************** Get parameter with icon set *************************/
/*****************************************************************************/

static unsigned Cal_GetParFirstDayOfWeek (void)
  {
   unsigned FirstDayOfWeek;

   FirstDayOfWeek = (unsigned)
	            Par_GetParUnsignedLong ("FirstDayOfWeek",
                                            0,
                                            6,
                                            Cal_FIRST_DAY_OF_WEEK_DEFAULT);
   if (!Cal_DayIsValidAsFirstDayOfWeek[FirstDayOfWeek])
      FirstDayOfWeek = Cal_FIRST_DAY_OF_WEEK_DEFAULT;

   return FirstDayOfWeek;
  }

/*****************************************************************************/
/******************** Get first day of week from string **********************/
/*****************************************************************************/

unsigned Cal_GetFirstDayOfWeekFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (Cal_DayIsValidAsFirstDayOfWeek[UnsignedNum])
         return (Dat_Format_t) UnsignedNum;

   return Cal_FIRST_DAY_OF_WEEK_DEFAULT;
  }

/*****************************************************************************/
/***************************** Draw current month ****************************/
/*****************************************************************************/

void Cal_DrawCurrentMonth (void)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   char ParsStr[Frm_MAX_BYTES_PARAMS_STR + 1];

   /***** Draw the month in JavaScript *****/
   /* JavaScript will write HTML here */
   HTM_DIV_Begin ("id=\"CurrentMonth\"");
   HTM_DIV_End ();

   /* Write script to draw the month */
   HTM_SCRIPT_Begin (NULL,NULL);
      HTM_Txt ("\tGbl_HTMLContent = '';");
      HTM_TxtF ("\tDrawCurrentMonth ('CurrentMonth',%u,%ld,%ld,'%s','%s/%s',",
		Gbl.Prefs.FirstDayOfWeek,
		(long) Dat_GetStartExecutionTimeUTC (),
		Gbl.Hierarchy.Node[Hie_CTR].Specific.PlcCod,
		The_GetSuffix (),
		Cfg_URL_SWAD_CGI,Lan_STR_LANG_ID[Gbl.Prefs.Language]);
      Frm_SetParsForm (ParsStr,ActSeeCal,true);
      HTM_TxtF ("'%s',",ParsStr);
      Frm_SetParsForm (ParsStr,ActSeeDatCfe,true);
      HTM_TxtF ("'%s');",ParsStr);
   HTM_SCRIPT_End ();
  }

/*****************************************************************************/
/************************ Draw an academic calendar **************************/
/*****************************************************************************/

void Cal_ShowCalendar (void)
  {
   Cal_DrawCalendar (ActSeeCal,ActChgCal1stDay,
                     Cal_PutIconsCalendar,NULL,
                     Vie_VIEW);
  }

void Cal_PrintCalendar (void)
  {
   Cal_DrawCalendar (ActUnk,ActUnk,
                     NULL,NULL,
                     Vie_PRINT);
  }

static void Cal_DrawCalendar (Act_Action_t ActionSeeCalendar,
                              Act_Action_t ActionChangeCalendar1stDay,
                              void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
                              Vie_ViewType_t ViewType)
  {
   extern const char *Hlp_START_Calendar;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   char ParsStr[Frm_MAX_BYTES_PARAMS_STR + 1];
   static const char *Print[Vie_NUM_VIEW_TYPES] =
     {
      [Vie_VIEW ] = "false",
      [Vie_PRINT] = "true",
     };

   /***** Begin box *****/
   Box_BoxBegin (NULL,FunctionToDrawContextualIcons,Args,
	         ViewType == Vie_VIEW ? Hlp_START_Calendar :
					NULL,
	         Box_NOT_CLOSABLE);

      /***** Write header *****/
      Lay_WriteHeaderClassPhoto (ViewType);

      /***** Preference selector to change first day of week *****/
      if (ViewType == Vie_VIEW)
	{
	 Set_BeginSettingsHead ();
	    Cal_ShowFormToSelFirstDayOfWeek (ActionChangeCalendar1stDay,
					     NULL,NULL);
	 Set_EndSettingsHead ();
	}

      /***** Draw several months *****/
      /* JavaScript will write HTML here */
      HTM_DIV_Begin ("id=\"calendar\"");
      HTM_DIV_End ();

      /* Write script to draw the month */
      HTM_SCRIPT_Begin (NULL,NULL);
	 HTM_Txt ("\tGbl_HTMLContent = '';");
	 HTM_TxtF ("\tCal_DrawCalendar('calendar',%u,%ld,%ld,%s,'%s','%s/%s',",
		   Gbl.Prefs.FirstDayOfWeek,
		   (long) Dat_GetStartExecutionTimeUTC (),
		   Gbl.Hierarchy.Node[Hie_CTR].Specific.PlcCod,
		   Print[ViewType],
		   The_GetSuffix (),
		   Cfg_URL_SWAD_CGI,Lan_STR_LANG_ID[Gbl.Prefs.Language]);
	 Frm_SetParsForm (ParsStr,ActionSeeCalendar,true);
	 HTM_TxtF ("'%s',",ParsStr);
	 Frm_SetParsForm (ParsStr,ActSeeDatCfe,true);
	 HTM_TxtF ("'%s');",ParsStr);
      HTM_SCRIPT_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************** Put contextual icons in calendar ***********************/
/*****************************************************************************/

static void Cal_PutIconsCalendar (__attribute__((unused)) void *Args)
  {
   /***** Print calendar *****/
   Ico_PutContextualIconToPrint (ActPrnCal,NULL,NULL);

   /***** View holidays *****/
   if (Gbl.Hierarchy.Level == Hie_INS)		// Institution selected
      Hld_PutIconToSeeHlds ();
  }

/*****************************************************************************/
/************************** Put icon to see calendar *************************/
/*****************************************************************************/

void Cal_PutIconToSeeCalendar (void)
  {
   Lay_PutContextualLinkOnlyIcon (ActSeeCal,NULL,
				  NULL,NULL,
				  "calendar.svg",Ico_BLACK);
  }

/*****************************************************************************/
/***** Get and show number of users who have chosen a first day of week ******/
/*****************************************************************************/

void Cal_GetAndShowNumUsrsPerFirstDayOfWeek (void)
  {
   extern bool Cal_DayIsValidAsFirstDayOfWeek[7];
   extern const char *Hlp_ANALYTICS_Figures_calendar;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Calendar;
   extern const char *Txt_First_day_of_the_week_X;
   extern const char *Txt_DAYS[7];
   extern const char *Txt_Number_of_users;
   extern const char *Txt_PERCENT_of_users;
   unsigned FirstDayOfWeek;
   char *SubQuery;
   char *Icon;
   char *Title;
   unsigned NumUsrs[7];	// 7: seven days in a week
   unsigned NumUsrsTotal = 0;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_FIRST_DAY_OF_WEEK],NULL,NULL,
                      Hlp_ANALYTICS_Figures_calendar,Box_NOT_CLOSABLE,2);

      /***** Heading row *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Calendar        ,HTM_HEAD_LEFT);
	 HTM_TH (Txt_Number_of_users ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_PERCENT_of_users,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** For each day... *****/
      for (FirstDayOfWeek = 0;	// Monday
	   FirstDayOfWeek <= 6;	// Sunday
	   FirstDayOfWeek++)
	 if (Cal_DayIsValidAsFirstDayOfWeek[FirstDayOfWeek])
	   {
	    /* Get number of users who have chosen this first day of week from database */
	    if (asprintf (&SubQuery,"usr_data.FirstDayOfWeek=%u",
			  (unsigned) FirstDayOfWeek) < 0)
	       Err_NotEnoughMemoryExit ();
	    NumUsrs[FirstDayOfWeek] = Usr_DB_GetNumUsrsWhoChoseAnOption (SubQuery);
	    free (SubQuery);

	    /* Update total number of users */
	    NumUsrsTotal += NumUsrs[FirstDayOfWeek];
	   }

      /***** Write number of users who have chosen each first day of week *****/
      for (FirstDayOfWeek = 0;	// Monday
	   FirstDayOfWeek <= 6;	// Sunday
	   FirstDayOfWeek++)
	 if (Cal_DayIsValidAsFirstDayOfWeek[FirstDayOfWeek])
	   {
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"CM\"");
		  if (asprintf (&Icon,"first-day-of-week-%u.png",
				FirstDayOfWeek) < 0)
		     Err_NotEnoughMemoryExit ();
		  if (asprintf (&Title,Txt_First_day_of_the_week_X,
		                Txt_DAYS[FirstDayOfWeek]) < 0)
		     Err_NotEnoughMemoryExit ();
		  Ico_PutIcon (Icon,Ico_BLACK,Title,"ICOx20");
		  free (Title);
		  free (Icon);
	       HTM_TD_End ();

	       HTM_TD_Unsigned (NumUsrs[FirstDayOfWeek]);
	       HTM_TD_Percentage (NumUsrs[FirstDayOfWeek],NumUsrsTotal);

	    HTM_TR_End ();
	   }

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
