// swad_language.c: user's settings on language

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

#include "swad_box.h"
#include "swad_database.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_setting.h"

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
  };

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Private prototypes ***************************/
/*****************************************************************************/

static void Lan_PutIconsLanguage (__attribute__((unused)) void *Args);

static void Lan_PutParamLanguage (void *Language);

/*****************************************************************************/
/*************** Put link to change language (edit settings) *****************/
/*****************************************************************************/

void Lan_PutLinkToChangeLanguage (void)
  {
   Lay_PutContextualLinkIconText (ActReqEdiSet,NULL,
                                  NULL,NULL,
			          "globe.svg",
				  "Change language");
  }

/*****************************************************************************/
/************** Put a selector to select language in settings ****************/
/*****************************************************************************/

void Lan_PutBoxToSelectLanguage (void)
  {
   extern const char *Hlp_PROFILE_Settings_language;
   extern const char *Txt_Language;

   Box_BoxBegin (NULL,Txt_Language,
                 Lan_PutIconsLanguage,NULL,
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

   Frm_StartForm (ActReqChgLan);
   HTM_SELECT_Begin (true,
		     "name=\"Lan\" style=\"width:112px; margin:0;\"");
   for (Lan  = (Lan_Language_t) 1;
	Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	Lan++)
     {
      LanUnsigned = (unsigned) Lan;
      HTM_OPTION (HTM_Type_UNSIGNED,&LanUnsigned,
		  Lan == Gbl.Prefs.Language,false,
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
   extern const char *Txt_Switch_to_LANGUAGE[1 + Lan_NUM_LANGUAGES];
   Lan_Language_t CurrentLanguage = Gbl.Prefs.Language;

   /***** Get param language *****/
   Gbl.Prefs.Language = Lan_GetParamLanguage ();	// Change temporarily language to set form action

   /***** Request confirmation *****/
   Ale_ShowAlertAndButton (ActChgLan,NULL,NULL,
                           Lan_PutParamLanguage,&Gbl.Prefs.Language,
                           Btn_CONFIRM_BUTTON,
                           Txt_Switch_to_LANGUAGE[Gbl.Prefs.Language],
                           Ale_QUESTION,Gbl.Usrs.Me.Logged ? Txt_Do_you_want_to_change_your_language_to_LANGUAGE[Gbl.Prefs.Language] :
	                                                     Txt_Do_you_want_to_change_the_language_to_LANGUAGE[Gbl.Prefs.Language]);

   Gbl.Prefs.Language = CurrentLanguage;		// Restore current language

   /***** Display settings *****/
   Set_EditSettings ();
  }

/*****************************************************************************/
/******************************* Change language *****************************/
/*****************************************************************************/

static void Lan_PutParamLanguage (void *Language)
  {
   if (Language)
      Par_PutHiddenParamUnsigned (NULL,"Lan",(unsigned) *((Lan_Language_t *) Language));
  }

/*****************************************************************************/
/******************************* Change language *****************************/
/*****************************************************************************/

void Lan_ChangeLanguage (void)
  {
   /***** Get param language *****/
   Gbl.Prefs.Language = Lan_GetParamLanguage ();

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
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   /***** Set my language to the current language *****/
   Gbl.Usrs.Me.UsrDat.Prefs.Language = Gbl.Prefs.Language;

   /***** Update my language in database *****/
   DB_QueryUPDATE ("can not update your language",
		   "UPDATE usr_data SET Language='%s' WHERE UsrCod=%ld",
	           Lan_STR_LANG_ID[Gbl.Prefs.Language],
	           Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*************************** Get parameter language **************************/
/*****************************************************************************/

Lan_Language_t Lan_GetParamLanguage (void)
  {
   extern const unsigned Txt_Current_CGI_SWAD_Language;

   return (Lan_Language_t)
	  Par_GetParToUnsignedLong ("Lan",
                                    1,
                                    Lan_NUM_LANGUAGES,
                                    (unsigned long) Txt_Current_CGI_SWAD_Language);
  }
