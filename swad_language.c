// swad_language.c: user's preferences on language

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf

#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_language.h"
#include "swad_preference.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES] = // ISO 639-1 language codes
	{
	"",	// Lan_LANGUAGE_UNKNOWN
	"ca",	// Lan_LANGUAGE_CA
	"de",	// Lan_LANGUAGE_DE
	"en",	// Lan_LANGUAGE_EN
	"es",	// Lan_LANGUAGE_ES
	"fr",	// Lan_LANGUAGE_FR
	"gn",	// Lan_LANGUAGE_GN
	"it",	// Lan_LANGUAGE_IT
	"pl",	// Lan_LANGUAGE_PL
	"pt",	// Lan_LANGUAGE_PT
	};

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Private prototypes ***************************/
/*****************************************************************************/

static void Lan_PutIconsLanguage (void);

static void Lan_PutParamLanguage (void);

/*****************************************************************************/
/************** Put link to change language (edit preferences) ***************/
/*****************************************************************************/

void Lan_PutLinkToChangeLanguage (void)
  {
   Lay_PutContextualLinkIconText (ActEdiPrf,NULL,NULL,
			          "globe.svg",
				  "Change language");
  }

/*****************************************************************************/
/************* Put a selector to select language in preferences **************/
/*****************************************************************************/

void Lan_PutBoxToSelectLanguage (void)
  {
   extern const char *Hlp_PROFILE_Preferences_language;
   extern const char *Txt_Language;

   Box_StartBox (NULL,Txt_Language,Lan_PutIconsLanguage,
                 Hlp_PROFILE_Preferences_language,Box_NOT_CLOSABLE);
   Lan_PutSelectorToSelectLanguage ();
   Box_EndBox ();
  }

/*****************************************************************************/
/*************** Put contextual icons in language preference *****************/
/*****************************************************************************/

static void Lan_PutIconsLanguage (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_LANGUAGES;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/********************* Put a selector to select language *********************/
/*****************************************************************************/

void Lan_PutSelectorToSelectLanguage (void)
  {
   extern const char *Txt_STR_LANG_NAME[1 + Lan_NUM_LANGUAGES];
   Lan_Language_t Lan;

   Frm_StartForm (ActReqChgLan);
   fprintf (Gbl.F.Out,"<select name=\"Lan\""
	              " style=\"width:112px; margin:0;\""
	              " onchange=\"document.getElementById('%s').submit();\">",
            Gbl.Form.Id);
   for (Lan = (Lan_Language_t) 1;
	Lan <= Lan_NUM_LANGUAGES;
	Lan++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) Lan);
      if (Lan == Gbl.Prefs.Language)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",Txt_STR_LANG_NAME[Lan]);
     }
   fprintf (Gbl.F.Out,"</select>");
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
   Ale_ShowAlertAndButton (Ale_QUESTION,
                           Gbl.Usrs.Me.Logged ? Txt_Do_you_want_to_change_your_language_to_LANGUAGE[Gbl.Prefs.Language] :
	                                        Txt_Do_you_want_to_change_the_language_to_LANGUAGE[Gbl.Prefs.Language],
                           ActChgLan,NULL,NULL,Lan_PutParamLanguage,
                           Btn_CONFIRM_BUTTON,
                           Txt_Switch_to_LANGUAGE[Gbl.Prefs.Language]);

   Gbl.Prefs.Language = CurrentLanguage;		// Restore current language

   /***** Display preferences *****/
   Pre_EditPrefs ();
  }

/*****************************************************************************/
/******************************* Change language *****************************/
/*****************************************************************************/

static void Lan_PutParamLanguage (void)
  {
   Par_PutHiddenParamUnsigned ("Lan",(unsigned) Gbl.Prefs.Language);
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

   /***** Set preferences from current IP *****/
   Pre_SetPrefsFromIP ();
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
