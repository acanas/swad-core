// swad_preference.c: user's preferences

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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
#include <stdio.h>		// For fprintf, etc.
#include <string.h>

#include "swad_bool.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_layout.h"
#include "swad_notification.h"
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

static void Prf_UpdateSideColsOnUsrDataTable (void);
static void Prf_PutFormPublicPhoto (void);

/*****************************************************************************/
/***************************** Edit preferences ******************************/
/*****************************************************************************/

void Prf_EditPrefs (void)
  {
   extern const char *Txt_Language;
   extern const char *Txt_Layout;
   extern const char *Txt_Theme_SKIN;
   extern const char *Txt_Columns;
   extern const char *Txt_Icons;
   extern const char *Txt_You_can_only_receive_email_notifications_if_;
   char MailDomain[Cns_MAX_BYTES_STRING+1];

   /***** Language selection *****/
   Lay_StartRoundFrameTable10 (NULL,0,NULL);
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"TIT_TBL\" style=\"text-align:center;\">"
	              "%s"
	              "</td>"
	              "</tr>"
                      "<tr>"
                      "<td style=\"text-align:center;\">",
            Txt_Language);
   Prf_PutSelectorToSelectLanguage ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
   Lay_EndRoundFrameTable10 ();

   /***** Layout selection *****/
   Lay_StartRoundFrameTable10 (NULL,0,NULL);
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"TIT_TBL\" style=\"text-align:center;\">"
	              "%s"
	              "</td>"
	              "</tr>"
                      "<tr>"
                      "<td style=\"text-align:center;\">",
            Txt_Layout);
   Lay_PutIconsToSelectLayout ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
   Lay_EndRoundFrameTable10 ();

   /***** Theme selection *****/
   Lay_StartRoundFrameTable10 (NULL,0,NULL);
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"TIT_TBL\" style=\"text-align:center;\">"
	              "%s"
	              "</td>"
	              "</tr>"
                      "<tr>"
                      "<td style=\"text-align:center;\">",
            Txt_Theme_SKIN);
   The_PutIconsToSelectTheme ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
   Lay_EndRoundFrameTable10 ();

   /***** Side columns selection *****/
   Lay_StartRoundFrameTable10 (NULL,0,NULL);
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"TIT_TBL\" style=\"text-align:center;\">"
	              "%s"
	              "</td>"
	              "</tr>"
                      "<tr>"
                      "<td style=\"text-align:center;\">",
            Txt_Columns);
   Prf_PutIconsToSelectSideCols ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
   Lay_EndRoundFrameTable10 ();

   /***** Icons selection *****/
   Lay_StartRoundFrameTable10 (NULL,0,NULL);
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"TIT_TBL\" style=\"text-align:center;\">"
	              "%s"
	              "</td>"
	              "</tr>"
                      "<tr>"
                      "<td style=\"text-align:center;\">",
            Txt_Icons);
   Ico_PutIconsToSelectIconSet ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
   Lay_EndRoundFrameTable10 ();

   if (Gbl.Usrs.Me.Logged)
     {
      /***** Public / private photo *****/
      Lay_StartRoundFrameTable10 (NULL,0,NULL);
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td style=\"text-align:center;\">");
      Prf_PutFormPublicPhoto ();
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");
      Lay_EndRoundFrameTable10 ();

      /***** Automatic e-mail to notify of new events *****/
      Ntf_PutFormChangeNotifSentByEMail ();

      Str_GetMailBox (Gbl.Usrs.Me.UsrDat.Email,MailDomain,Cns_MAX_BYTES_STRING);
      if (!Mai_CheckIfMailDomainIsAllowedForNotifications (MailDomain))
         Lay_ShowAlert (Lay_WARNING,Txt_You_can_only_receive_email_notifications_if_);
     }
  }

/*****************************************************************************/
/******************* Get preferences changed from current IP *****************/
/*****************************************************************************/

void Prf_GetPrefsFromIP (void)
  {
   char Query[1024];
   unsigned long NumRows;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;

   if (Gbl.IP[0])
     {
      /***** Get preferences from database *****/
      sprintf (Query,"SELECT Layout,Theme,IconSet,SideCols"
		     " FROM IP_prefs WHERE IP='%s'",
	       Gbl.IP);
      if ((NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get preferences")))
	{
	 if (NumRows != 1)
	    Lay_ShowErrorAndExit ("Internal error while getting preferences.");

	 /***** Get preferences from database *****/
	 row = mysql_fetch_row (mysql_res);

	 /* Get layout (row[0]) */
	 Gbl.Prefs.Layout = Lay_LAYOUT_DEFAULT;
	 if (sscanf (row[0],"%u",&UnsignedNum) == 1)
	    if (UnsignedNum < Lay_NUM_LAYOUTS)
	       Gbl.Prefs.Layout = (Lay_Layout_t) UnsignedNum;

	 /* Get theme (row[1]) */
	 Gbl.Prefs.Theme = The_GetThemeFromStr (row[1]);

	 /* Get icon set (row[2]) */
	 Gbl.Prefs.IconSet = Ico_GetIconSetFromStr (row[2]);

	 /* Get if user wants to show side columns (row[3]) */
	 if (sscanf (row[3],"%u",&Gbl.Prefs.SideCols) == 1)
	   {
	    if (Gbl.Prefs.SideCols > Lay_SHOW_BOTH_COLUMNS)
	       Gbl.Prefs.SideCols = Lay_SHOW_BOTH_COLUMNS;	// Show both side columns
	   }
	 else
	    Gbl.Prefs.SideCols = Cfg_DEFAULT_COLUMNS;
	}
     }
  }

/*****************************************************************************/
/************************ Set preferences from current IP ********************/
/*****************************************************************************/

void Prf_SetPrefsFromIP (void)
  {
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   char Query[512];

   /***** Update preferences from current IP in database *****/
   sprintf (Query,"REPLACE INTO IP_prefs (IP,UsrCod,LastChange,Layout,Theme,IconSet,SideCols)"
                  " VALUES ('%s','%ld',NOW(),'%u','%s','%s','%u')",
            Gbl.IP,Gbl.Usrs.Me.UsrDat.UsrCod,
            (unsigned) Gbl.Prefs.Layout,
            The_ThemeId[Gbl.Prefs.Theme],
            Ico_IconSetId[Gbl.Prefs.IconSet],
            Gbl.Prefs.SideCols);
   DB_QueryREPLACE (Query,"can not store preferences from current IP address");

   /***** If a user is logged, update its preferences in database for all its IP's *****/
   if (Gbl.Usrs.Me.Logged)
     {
      sprintf (Query,"UPDATE IP_prefs SET Layout='%u',Theme='%s',IconSet='%s',SideCols='%u'"
                     " WHERE UsrCod='%ld'",
               (unsigned) Gbl.Prefs.Layout,
               The_ThemeId[Gbl.Prefs.Theme],
               Ico_IconSetId[Gbl.Prefs.IconSet],
               Gbl.Prefs.SideCols,
               Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryUPDATE (Query,"can not update your preferences");
     }
  }

/*****************************************************************************/
/*********************** Remove old preferences from IP **********************/
/*****************************************************************************/

void Prf_RemoveOldPrefsFromIP (void)
  {
   char Query[256];

   /***** Remove old preferences *****/
   sprintf (Query,"DELETE LOW_PRIORITY FROM IP_prefs"
                  " WHERE UNIX_TIMESTAMP() > UNIX_TIMESTAMP(LastChange)+%ld",
            Cfg_TIME_TO_DELETE_IP_PREFS);
   DB_QueryDELETE (Query,"can not remove old preferences");
  }

/*****************************************************************************/
/********************* Put a selector to select language *********************/
/*****************************************************************************/
// Width == 0 means don't force width of selector

void Prf_PutSelectorToSelectLanguage (void)
  {
   extern const char *Txt_STR_LANG_NAME[Txt_NUM_LANGUAGES];
   Txt_Language_t Lan;
   static const unsigned SelectorWidth[Lay_NUM_LAYOUTS] =
     {
      90,	// Lay_LAYOUT_DESKTOP
      120,	// Lay_LAYOUT_MOBILE
     };

   Act_FormStart (ActReqChgLan);
   fprintf (Gbl.F.Out,"<select name=\"Lan\" style=\"width:%upx;margin:0;\""
	              " onchange=\"javascript:document.getElementById('%s').submit();\">",
            SelectorWidth[Gbl.Prefs.Layout],Gbl.FormId);
   for (Lan = (Txt_Language_t) 0;
	Lan < Txt_NUM_LANGUAGES;
	Lan++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) Lan);
      if (Lan == Gbl.Prefs.Language)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",Txt_STR_LANG_NAME[Lan]);
     }
   fprintf (Gbl.F.Out,"</select>"
	              "</form>");
  }

/*****************************************************************************/
/********* Ask user if he/she really wants to change the language ************/
/*****************************************************************************/

void Prf_AskChangeLanguage (void)
  {
   extern const char *Txt_Do_you_want_to_change_your_language_to_LANGUAGE[Txt_NUM_LANGUAGES];
   extern const char *Txt_Do_you_want_to_change_the_language_to_LANGUAGE[Txt_NUM_LANGUAGES];
   extern const char *Txt_Switch_to_LANGUAGE[Txt_NUM_LANGUAGES];
   Txt_Language_t CurrentLanguage = Gbl.Prefs.Language;

   /***** Get param language *****/
   Gbl.Prefs.Language = Prf_GetParamLanguage ();	// Change temporarily language to set form action

   /***** Request confirmation *****/
   if (Gbl.Usrs.Me.Logged)
   Lay_ShowAlert (Lay_INFO,
                  Gbl.Usrs.Me.Logged ? Txt_Do_you_want_to_change_your_language_to_LANGUAGE[Gbl.Prefs.Language] :
	                               Txt_Do_you_want_to_change_the_language_to_LANGUAGE[Gbl.Prefs.Language]);

   /***** Send button *****/
   Act_FormStart (ActChgLan);
   Par_PutHiddenParamUnsigned ("Lan",(unsigned) Gbl.Prefs.Language);
   Lay_PutSendButton (Txt_Switch_to_LANGUAGE[Gbl.Prefs.Language]);
   fprintf (Gbl.F.Out,"</form>");

   Gbl.Prefs.Language = CurrentLanguage;		// Restore current language
  }

/*****************************************************************************/
/******************************* Change language *****************************/
/*****************************************************************************/

void Prf_ChangeLanguage (void)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *Txt_Your_language_has_changed_to_LANGUAGE;
   extern const char *Txt_The_language_has_changed_to_LANGUAGE;
   extern const char *Txt_STR_LANG_NAME[Txt_NUM_LANGUAGES];
   char Query[512];
   bool MyLanguageHasChanged = false;

   /***** Get param language *****/
   Gbl.Prefs.Language = Prf_GetParamLanguage ();

   /***** Store language in database *****/
   if (Gbl.Usrs.Me.Logged && Gbl.Prefs.Language != Gbl.Usrs.Me.UsrDat.Prefs.Language)
     {
      sprintf (Query,"UPDATE usr_data SET Language='%s' WHERE UsrCod='%ld'",
               Txt_STR_LANG_ID[Gbl.Prefs.Language],Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryUPDATE (Query,"can not update your language");
      MyLanguageHasChanged = true;
     }

   /***** Set preferences from current IP *****/
   Prf_SetPrefsFromIP ();

   /***** Confirmation *****/
   if (MyLanguageHasChanged)
   Lay_ShowAlert (Lay_SUCCESS,
                  MyLanguageHasChanged ? Txt_Your_language_has_changed_to_LANGUAGE :
                                         Txt_The_language_has_changed_to_LANGUAGE);
  }

/*****************************************************************************/
/*************************** Get parameter language **************************/
/*****************************************************************************/

Txt_Language_t Prf_GetParamLanguage (void)
  {
   extern const unsigned Txt_Current_CGI_SWAD_Language;
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("Lan",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Txt_NUM_LANGUAGES)
         return (Txt_Language_t) UnsignedNum;

   return Txt_Current_CGI_SWAD_Language;
  }

/*****************************************************************************/
/************ Put icons to select the layout of the side columns *************/
/*****************************************************************************/

void Prf_PutIconsToSelectSideCols (void)
  {
   unsigned SideCols;
   extern const char *Txt_LAYOUT_SIDE_COLUMNS[4];

   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_1\">"
                      "<tr>");
   for (SideCols = 0;
	SideCols <= Lay_SHOW_BOTH_COLUMNS;
	SideCols++)
     {
      fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:center;\">",
               SideCols == Gbl.Prefs.SideCols ? "LAYOUT_ON" :
        	                                "LAYOUT_OFF");
      Act_FormStart (ActChgCol);
      Par_PutHiddenParamUnsigned ("SideCols",SideCols);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/layout%u%u_32x20.gif\""
	                 " alt=\"%s\" title=\"%s\" style=\"display:block; width:32px; height:20px;\" />"
	                 "</form>"
	                 "</td>",
               Gbl.Prefs.IconsURL,
               SideCols >> 1,SideCols & 1,
               Txt_LAYOUT_SIDE_COLUMNS[SideCols],
               Txt_LAYOUT_SIDE_COLUMNS[SideCols]);
     }
   fprintf (Gbl.F.Out,"</tr>"
                      "</table>");
  }

/*****************************************************************************/
/**************** Put left icon to hide/show side columns ********************/
/*****************************************************************************/

void Prf_PutLeftIconToHideShowCols (void)
  {
   extern const char *Txt_Hide_left_column;
   extern const char *Txt_Show_left_column;

   if (Gbl.Prefs.SideCols & Lay_SHOW_LEFT_COLUMN)
     {
      Act_FormStart (ActHidLftCol);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/central_left_8x800.gif\""
	                 " alt=\"%s\" title=\"%s\" style=\"width:8px;height:800px;\" />",
               Gbl.Prefs.IconsURL,
               Txt_Hide_left_column,
               Txt_Hide_left_column);
     }
   else
     {
      Act_FormStart (ActShoLftCol);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/central_right_8x800.gif\""
	                 " alt=\"%s\" title=\"%s\" style=\"width:8px;height:800px;\" />",
               Gbl.Prefs.IconsURL,
               Txt_Show_left_column,
               Txt_Show_left_column);
     }
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/**************** Put right icon to hide/show side columns *******************/
/*****************************************************************************/

void Prf_PutRigthIconToHideShowCols (void)
  {
   extern const char *Txt_Hide_right_column;
   extern const char *Txt_Show_right_column;

   if (Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN)
     {
      Act_FormStart (ActHidRgtCol);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/central_right_8x800.gif\""
	                 " alt=\"%s\" title=\"%s\" style=\"width:8px;height:800px;\" />",
               Gbl.Prefs.IconsURL,
               Txt_Hide_right_column,
               Txt_Hide_right_column);
     }
   else
     {
      Act_FormStart (ActShoRgtCol);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/central_left_8x800.gif\""
	                 " alt=\"%s\" title=\"%s\" style=\"width:8px;height:800px;\" />",
               Gbl.Prefs.IconsURL,
               Txt_Show_right_column,
               Txt_Show_right_column);
     }
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/*********************** Change layout of side columns ***********************/
/*****************************************************************************/

void Prf_ChangeSideCols (void)
  {
   /***** Get param side-columns *****/
   Gbl.Prefs.SideCols = Prf_GetParamSideCols ();

   /***** Store side colums in database *****/
   if (Gbl.Usrs.Me.Logged)
      Prf_UpdateSideColsOnUsrDataTable ();

   /***** Set preferences from current IP *****/
   Prf_SetPrefsFromIP ();
  }

/*****************************************************************************/
/*************************** Hide left side column ***************************/
/*****************************************************************************/

void Prf_HideLeftCol (void)
  {
   Gbl.Prefs.SideCols &= ~Lay_SHOW_LEFT_COLUMN;	//  And with 1...101 to hide left column
   if (Gbl.Usrs.Me.Logged)
      Prf_UpdateSideColsOnUsrDataTable ();

   /***** Set preferences from current IP *****/
   Prf_SetPrefsFromIP ();
  }

/*****************************************************************************/
/*************************** Hide right side column **************************/
/*****************************************************************************/

void Prf_HideRightCol (void)
  {
   Gbl.Prefs.SideCols &= ~Lay_SHOW_RIGHT_COLUMN;	//  And with 1...110 to hide right column
   if (Gbl.Usrs.Me.Logged)
      Prf_UpdateSideColsOnUsrDataTable ();

   /***** Set preferences from current IP *****/
   Prf_SetPrefsFromIP ();
  }

/*****************************************************************************/
/**************************** Show left side column **************************/
/*****************************************************************************/

void Prf_ShowLeftCol (void)
  {
   Gbl.Prefs.SideCols |= Lay_SHOW_LEFT_COLUMN;	// Or with 10 to show left column
   if (Gbl.Usrs.Me.Logged)
      Prf_UpdateSideColsOnUsrDataTable ();

   /***** Set preferences from current IP *****/
   Prf_SetPrefsFromIP ();
  }

/*****************************************************************************/
/**************************** Show right side column *************************/
/*****************************************************************************/

void Prf_ShowRightCol (void)
  {
   Gbl.Prefs.SideCols |= Lay_SHOW_RIGHT_COLUMN;	// Or with 01 to show right column
   if (Gbl.Usrs.Me.Logged)
      Prf_UpdateSideColsOnUsrDataTable ();

   /***** Set preferences from current IP *****/
   Prf_SetPrefsFromIP ();
  }

/*****************************************************************************/
/************** Update layout of side colums on user data table **************/
/*****************************************************************************/

static void Prf_UpdateSideColsOnUsrDataTable (void)
  {
   char Query[512];

   sprintf (Query,"UPDATE usr_data SET SideCols='%u' WHERE UsrCod='%ld'",
            Gbl.Prefs.SideCols,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update your preference about side columns");
  }

/*****************************************************************************/
/************** Get parameter used to show/hide side columns *****************/
/*****************************************************************************/

unsigned Prf_GetParamSideCols (void)
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

/*****************************************************************************/
/*********************** Select public / private photo ***********************/
/*****************************************************************************/

static void Prf_PutFormPublicPhoto (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Public_photo;

   /***** Start form *****/
   Act_FormStart (ActChgPubPho);
   fprintf (Gbl.F.Out,"<table style=\"border-spacing:2px;\">"
                      "<tr>");

   /***** Checkbox to select between public or private photo *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:right;\">"
                      "<input type=\"checkbox\" name=\"PublicPhoto\" value=\"Y\"");
   if (Gbl.Usrs.Me.UsrDat.PublicPhoto)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   fprintf (Gbl.F.Out," onchange=\"javascript:document.getElementById('%s').submit();\" />"
                      "</td>",
            Gbl.FormId);
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;\">"
	              "%s"
	              "</td>",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Public_photo);

   /***** End form *****/
   fprintf (Gbl.F.Out,"</tr>"
                      "</table>"
                      "</form>");
  }

/*****************************************************************************/
/********** Get parameter with public / private photo from form **************/
/*****************************************************************************/

bool Prf_GetParamPublicPhoto (void)
  {
   char YN[1+1];

   Par_GetParToText ("PublicPhoto",YN,1);
   return (Str_ConvertToUpperLetter (YN[0]) == 'Y');
  }

/*****************************************************************************/
/*********************** Change public / private photo ***********************/
/*****************************************************************************/

void Prf_ChangePublicPhoto (void)
  {
   char Query[512];

   /***** Get param with public/private photo *****/
   Gbl.Usrs.Me.UsrDat.PublicPhoto = Prf_GetParamPublicPhoto ();

   /***** Store public/private photo in database *****/
   sprintf (Query,"UPDATE usr_data SET PublicPhoto='%c' WHERE UsrCod='%ld'",
            Gbl.Usrs.Me.UsrDat.PublicPhoto ? 'Y' :
        	                             'N',
            Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update your preference about public photo");
  }
