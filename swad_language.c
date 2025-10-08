// swad_language.c: user's settings on language

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
#include <string.h>		// For strcasecmp

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_parameter.h"
#include "swad_setting.h"
#include "swad_setting_database.h"
#include "swad_user_database.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES] = // ISO 639-1 language codes
  {
   [Lan_LANGUAGE_UNKNOWN] = "",
   [Lan_LANGUAGE_CA     ] = "ca",
   [Lan_LANGUAGE_DE     ] = "de",
   [Lan_LANGUAGE_EN     ] = "en",
   [Lan_LANGUAGE_ES     ] = "es",
   [Lan_LANGUAGE_FR     ] = "fr",
   [Lan_LANGUAGE_GN     ] = "gn",
   [Lan_LANGUAGE_IT     ] = "it",
   [Lan_LANGUAGE_PL     ] = "pl",
   [Lan_LANGUAGE_PT     ] = "pt",
   [Lan_LANGUAGE_TR     ] = "tr",
  };

/*****************************************************************************/
/****************************** Private prototypes ***************************/
/*****************************************************************************/

static void Lan_PutIconsLanguage (__attribute__((unused)) void *Args);

static void Lan_PutParLanguage (void *Language);

/*****************************************************************************/
/*************** Put link to change language (edit settings) *****************/
/*****************************************************************************/

void Lan_PutLinkToChangeLanguage (void)
  {
   Lay_PutContextualLinkIconText (ActReqEdiSet,NULL,
                                  NULL,NULL,
			          "globe-americas.svg",Ico_BLACK,
				  "Change language",NULL);
  }

/*****************************************************************************/
/************** Put a selector to select language in settings ****************/
/*****************************************************************************/

void Lan_PutBoxToSelectLanguage (void)
  {
   extern const char *Hlp_PROFILE_Settings_language;
   extern const char *Txt_Language;

   Box_BoxBegin (Txt_Language,Lan_PutIconsLanguage,NULL,
                 Hlp_PROFILE_Settings_language,Box_NOT_CLOSABLE);
      Lan_PutSelectorToSelectLanguage ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/**************** Put contextual icons in language setting *******************/
/*****************************************************************************/

static void Lan_PutIconsLanguage (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_LANGUAGES);
  }

/*****************************************************************************/
/********************* Put a selector to select language *********************/
/*****************************************************************************/

void Lan_PutSelectorToSelectLanguage (void)
  {
   extern const char *Txt_STR_LANG_NAME[1 + Lan_NUM_LANGUAGES];
   Lan_Language_t Lan;
   unsigned LanUnsigned;

   Frm_BeginForm (ActReqChgLan);
      HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
			"name=\"Lan\" class=\"INPUT_%s\""
			" style=\"width:112px; margin:0;\"",The_GetSuffix ());
	 for (Lan  = (Lan_Language_t) 1;
	      Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	      Lan++)
	   {
	    LanUnsigned = (unsigned) Lan;
	    HTM_OPTION (HTM_Type_UNSIGNED,&LanUnsigned,
			Lan == Gbl.Prefs.Language ? HTM_SELECTED :
						    HTM_NO_ATTR,
			"%s",Txt_STR_LANG_NAME[Lan]);
	   }
      HTM_SELECT_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/********* Ask user if he/she really wants to change the language ************/
/*****************************************************************************/

void Lan_AskChangeLanguage (void)
  {
   extern const char *Txt_Do_you_want_to_change_your_language_to_LANGUAGE[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_Do_you_want_to_change_the_language_to_LANGUAGE[1 + Lan_NUM_LANGUAGES];
   Lan_Language_t CurrentLanguage = Gbl.Prefs.Language;

   /***** Get param language *****/
   Gbl.Prefs.Language = Lan_GetParLanguage ();	// Change temporarily language to set form action

   /***** Request confirmation *****/
   Ale_ShowAlertAndButton (ActChgLan,NULL,NULL,
                           Lan_PutParLanguage,&Gbl.Prefs.Language,
                           Btn_CHANGE,
                           Ale_QUESTION,Gbl.Usrs.Me.Logged ? Txt_Do_you_want_to_change_your_language_to_LANGUAGE[Gbl.Prefs.Language] :
	                                                     Txt_Do_you_want_to_change_the_language_to_LANGUAGE[Gbl.Prefs.Language]);

   Gbl.Prefs.Language = CurrentLanguage;		// Restore current language

   /***** Display settings *****/
   Set_EditSettings ();
  }

/*****************************************************************************/
/******************************* Change language *****************************/
/*****************************************************************************/

static void Lan_PutParLanguage (void *Language)
  {
   if (Language)
      Par_PutParUnsigned (NULL,"Lan",(unsigned) *((Lan_Language_t *) Language));
  }

/*****************************************************************************/
/******************************* Change language *****************************/
/*****************************************************************************/

void Lan_ChangeLanguage (void)
  {
   /***** Get param language *****/
   Gbl.Prefs.Language = Lan_GetParLanguage ();

   /***** Store language in database *****/
   if (Gbl.Usrs.Me.Logged &&
       Gbl.Prefs.Language != Gbl.Usrs.Me.UsrDat.Prefs.Language)
     Lan_UpdateMyLanguageToCurrentLanguage ();

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/**************** Update my language to the current language *****************/
/*****************************************************************************/

void Lan_UpdateMyLanguageToCurrentLanguage (void)
  {
   /***** Set my language to the current language *****/
   Gbl.Usrs.Me.UsrDat.Prefs.Language = Gbl.Prefs.Language;

   /***** Update my language in database *****/
   Set_DB_UpdateMySettingsAboutLanguage ();
  }

/*****************************************************************************/
/*************************** Get parameter language **************************/
/*****************************************************************************/

Lan_Language_t Lan_GetParLanguage (void)
  {
   extern unsigned Txt_Current_CGI_SWAD_Language;

   return (Lan_Language_t)
	  Par_GetParUnsignedLong ("Lan",
                                  1,
                                  Lan_NUM_LANGUAGES,
                                  (unsigned long) Txt_Current_CGI_SWAD_Language);
  }

/*****************************************************************************/
/************************** Get language from string *************************/
/*****************************************************************************/

Lan_Language_t Lan_GetLanguageFromStr (const char *Str)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   Lan_Language_t Lan;

   for (Lan  = (Lan_Language_t) 1;
	Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	Lan++)
      if (!strcasecmp (Str,Lan_STR_LANG_ID[Lan]))
	 return Lan;

   return Lan_LANGUAGE_UNKNOWN;
  }

/*****************************************************************************/
/********* Get and show number of users who have chosen a language ***********/
/*****************************************************************************/

void Lan_GetAndShowNumUsrsPerLanguage (Hie_Level_t HieLvl)
  {
   extern const char *Hlp_ANALYTICS_Figures_language;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Language;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_STR_LANG_NAME[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_Number_of_users;
   extern const char *Txt_PERCENT_of_users;
   Lan_Language_t Lan;
   char *SubQuery;
   unsigned NumUsrs[1 + Lan_NUM_LANGUAGES];
   unsigned NumUsrsTotal = 0;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_LANGUAGES],NULL,NULL,
                      Hlp_ANALYTICS_Figures_language,Box_NOT_CLOSABLE,2);

      /***** Heading row *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Language        ,HTM_HEAD_LEFT);
	 HTM_TH (Txt_Number_of_users ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_PERCENT_of_users,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** For each language... *****/
      for (Lan  = (Lan_Language_t) 1;
	   Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	   Lan++)
	{
	 /* Get the number of users who have chosen this language from database */
	 if (asprintf (&SubQuery,"usr_data.Language='%s'",
		       Lan_STR_LANG_ID[Lan]) < 0)
	    Err_NotEnoughMemoryExit ();
	 NumUsrs[Lan] = Usr_DB_GetNumUsrsWhoChoseAnOption (HieLvl,SubQuery);
	 free (SubQuery);

	 /* Update total number of users */
	 NumUsrsTotal += NumUsrs[Lan];
	}

      /***** Write number of users who have chosen each language *****/
      for (Lan  = (Lan_Language_t) 1;
	   Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	   Lan++)
	{
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	       HTM_Txt (Txt_STR_LANG_NAME[Lan]);
	    HTM_TD_End ();

	    HTM_TD_Unsigned (NumUsrs[Lan]);
	    HTM_TD_Percentage (NumUsrs[Lan],NumUsrsTotal);

	 HTM_TR_End ();
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
