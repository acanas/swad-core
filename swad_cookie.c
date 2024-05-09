// swad_cookies.c: user's preferences about cookies

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

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_cookie.h"
#include "swad_cookie_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_layout.h"
#include "swad_parameter.h"
#include "swad_setting.h"
#include "swad_user_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Coo_PutIconsCookies (__attribute__((unused)) void *Args);

/*****************************************************************************/
/********************* Edit my preferences on cookies ************************/
/*****************************************************************************/

void Coo_EditMyPrefsOnCookies (void)
  {
   extern const char *Hlp_PROFILE_Settings_cookies;
   extern const char *Txt_Cookies;
   extern const char *Txt_Accept_third_party_cookies_to_view_multimedia_content_from_other_websites;
   Cns_Checked_t Checked;

   /***** Begin section with preferences about cookies *****/
   HTM_SECTION_Begin (Coo_COOKIES_ID);

      /***** Begin box and table *****/
      Box_BoxTableBegin (Txt_Cookies,Coo_PutIconsCookies,NULL,
			 Hlp_PROFILE_Settings_cookies,Box_NOT_CLOSABLE,2);

	 /***** Edit my preference about cookies *****/
	 /* Begin form */
	 Frm_BeginFormAnchor (ActChgCooPrf,Coo_COOKIES_ID);

	    /* Begin container */
	    switch (Gbl.Usrs.Me.UsrDat.Prefs.RefuseAcceptCookies)
	      {
	       case Coo_REFUSE:
		  HTM_DIV_Begin ("class=\"DAT_%s\"",
				 The_GetSuffix ());
		  break;
	       case Coo_ACCEPT:
		  HTM_DIV_Begin ("class=\"DAT_STRONG_%s BG_HIGHLIGHT\"",
				 The_GetSuffix ());
		  break;
	      }

	    /* Check box */
	    HTM_LABEL_Begin (NULL);
	       Checked = (Gbl.Usrs.Me.UsrDat.Prefs.RefuseAcceptCookies == Coo_ACCEPT) ? Cns_CHECKED :
												   Cns_UNCHECKED;
	       HTM_INPUT_CHECKBOX ("cookies",
				   Checked,HTM_ENABLED,HTM_SUBMIT_ON_CHANGE,
				   "value=\"Y\"");
	       HTM_Txt (Txt_Accept_third_party_cookies_to_view_multimedia_content_from_other_websites);
	    HTM_LABEL_End ();

	    /* End container */
	    HTM_DIV_End ();

	 /* End form */
         Frm_EndForm ();

      /***** End table and box *****/
      Box_BoxTableEnd ();

   /***** End section with preferences about cookies *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/***************** Put contextual icons in cookies preference ****************/
/*****************************************************************************/

static void Coo_PutIconsCookies (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_COOKIES);
  }

/*****************************************************************************/
/************** Change my preference about third party cookies ***************/
/*****************************************************************************/

void Coo_ChangeMyPrefsCookies (void)
  {
   /***** Get param with preference about third party cookies *****/
   Gbl.Usrs.Me.UsrDat.Prefs.RefuseAcceptCookies = Par_GetParBool ("cookies") ? Coo_ACCEPT :
									       Coo_REFUSE;

   /***** Store preference in database *****/
   if (Gbl.Usrs.Me.Logged)
      Coo_DB_UpdateMyPrefsCookies ();

   /***** Show forms again *****/
   Set_EditSettings ();
  }

/*****************************************************************************/
/** Get and show number of users who have chosen a preference about cookies **/
/*****************************************************************************/

void Coo_GetAndShowNumUsrsPerCookies (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_cookies;
   extern const char *Txt_Do_not_accept_third_party_cookies;
   extern const char *Txt_Accept_third_party_cookies;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Cookies;
   extern const char *Txt_Number_of_users;
   extern const char *Txt_PERCENT_of_users;
   static struct
     {
      const char InDB;
      const char *Icon;
      Ico_Color_t Color;
      const char **Title;
     } Accepted[Coo_NUM_REFUSE_ACCEPT] =
     {
      [Coo_REFUSE] =
        {
         .InDB  = 'N',
         .Icon  = "times.svg",
         .Color = Ico_RED,
         .Title = &Txt_Do_not_accept_third_party_cookies
        },
      [Coo_ACCEPT] =
	{
	 .InDB  = 'Y',
	 .Icon  = "check.svg",
	 .Color = Ico_GREEN,
	 .Title = &Txt_Accept_third_party_cookies
	}
     };
   Coo_RefuseAccept_t RefAcc;
   char *SubQuery;
   unsigned NumUsrs[Mnu_NUM_MENUS];
   unsigned NumUsrsTotal = 0;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_COOKIES],NULL,NULL,
                      Hlp_ANALYTICS_Figures_cookies,Box_NOT_CLOSABLE,2);

      /***** Heading row *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Cookies         ,HTM_HEAD_LEFT);
	 HTM_TH (Txt_Number_of_users ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_PERCENT_of_users,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** For each option... *****/
      for (RefAcc = (Coo_RefuseAccept_t) 0;
	   RefAcc < (Coo_RefuseAccept_t) (Coo_NUM_REFUSE_ACCEPT - 1);
	   RefAcc++)
	{
	 /* Get number of users who have chosen this menu from database */
	 if (asprintf (&SubQuery,"usr_data.ThirdPartyCookies='%c'",
		       Accepted[RefAcc].InDB) < 0)
	    Err_NotEnoughMemoryExit ();
	 NumUsrs[RefAcc] = Usr_DB_GetNumUsrsWhoChoseAnOption (SubQuery);
	 free (SubQuery);

	 /* Update total number of users */
	 NumUsrsTotal += NumUsrs[RefAcc];
	}

      /***** Write number of users who have chosen each option *****/
      for (RefAcc = (Coo_RefuseAccept_t) 0;
	   RefAcc < (Coo_RefuseAccept_t) (Coo_NUM_REFUSE_ACCEPT - 1);
	   RefAcc++)
	{
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"CM\"");
               Ico_PutIcon (Accepted[RefAcc].Icon,Accepted[RefAcc].Color,
                            *Accepted[RefAcc].Title,"ICOx16");
	    HTM_TD_End ();

	    HTM_TD_Unsigned (NumUsrs[RefAcc]);
	    HTM_TD_Percentage (NumUsrs[RefAcc],NumUsrsTotal);

	 HTM_TR_End ();
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
