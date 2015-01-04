// swad_layout.c: page layout

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For exit, system, malloc, calloc, free, etc
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_calendar.h"
#include "swad_changelog.h"
#include "swad_config.h"
#include "swad_connected.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_preference.h"
#include "swad_theme.h"
#include "swad_web_service.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;
extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

const char *Lay_LayoutIcons[Lay_NUM_LAYOUTS] =
  {
   "desktop",
   "mobile",
  };

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

const char *Lay_TabIcons[Act_NUM_TABS] =
  {
   /* TabUnk */	NULL,
   /* TabSys */	"sys",
   /* TabCty */	"cty",
   /* TabIns */	"ins",
   /* TabCtr */	"ctr",
   /* TabDeg */	"deg",
   /* TabCrs */	"crs",
   /* TabAss */	"ass",
   /* TabUsr */	"usr",
   /* TabMsg */	"msg",
   /* TabSta */	"sta",
   /* TabPrf */	"prf",
  };

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Lay_WriteEndOfPage (void);

static void Lay_WritePageTitle (void);

static void Lay_WriteRedirectionToMyLanguage (void);
static void Lay_WriteScripts (void);
static void Lay_WriteScriptInit (void);
static void Lay_WriteScriptConnectedUsrs (void);
static void Lay_WriteScriptCustomDropzone (void);

static void Lay_WritePageTopHeading (void);
static void Lay_DrawTabs (void);
static void Lay_DrawTabsDeskTop (void);
static void Lay_DrawTabsMobile (void);
static bool Lay_CheckIfICanViewTab (Act_Tab_t Tab);
static void Lay_DrawBreadcrumb (void);

static void Lay_WriteBreadcrumbHome (void);
static void Lay_WriteBreadcrumbTab (void);
static void Lay_WriteBreadcrumbAction (void);
static void Lay_WriteTitleAction (void);

static void Lay_ShowLeftColumn (void);
static void Lay_ShowRightColumn (void);

static void Lay_WriteFootFromHTMLFile (void);

/*****************************************************************************/
/*********************** Write the start of the page *************************/
/*****************************************************************************/

void Lay_WriteStartOfPage (void)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const unsigned Txt_Current_CGI_SWAD_Language;
   extern const char *The_TabOnBgColors[The_NUM_THEMES];
   extern const char *Txt_NEW_YEAR_GREETING;
   unsigned ColspanCentralPart = 3;	// Initialized to avoid warnning
//   char QueryDebug[1024];

   /***** If, when this function is called, the head is being written, or the head is already written ==>
           ==> don't do anything *****/
   if (Gbl.Layout.WritingHTMLStart ||
       Gbl.Layout.HTMLStartWritten)
      return;

//   if (Gbl.Usrs.Me.UsrDat.UsrCod == 1346)
//     {
//      sprintf (QueryDebug,"INSERT INTO debug (DebugTime,Txt) VALUES (NOW(),'Act_Actions[ActRefLstClk].ActCod = %u --- Act_Actions[Gbl.CurrentAct].ActCod = %u')",
//	       (unsigned) Act_Actions[ActRefLstClk].ActCod,(unsigned) Act_Actions[Gbl.CurrentAct].ActCod);
//      DB_QueryINSERT (QueryDebug,"Error inserting in debug table");
//     }

   /***** Compute connected users *****/
   if (Gbl.CurrentAct == ActLstCon ||
       (Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP &&
        (Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN) &&
        Gbl.CurrentCrs.Crs.CrsCod >= 0))	// Right column visible && There is a course selected
      Con_ComputeConnectedUsrsBelongingToCurrentCrs ();

   /***** Send head width the file type for the HTTP protocol *****/
   if (Gbl.CurrentAct == ActRefCon ||
       Gbl.CurrentAct == ActRefLstClk)	// Don't generate a full HTML page, only refresh connected users
     {
      fprintf (Gbl.F.Out,"Content-Type: text/html; charset=windows-1252\r\n");
      fprintf (Gbl.F.Out,"Cache-Control: max-age=0, no-cache, must-revalidate\r\n\r\n");

      Gbl.Layout.WritingHTMLStart = false;
      Gbl.Layout.HTMLStartWritten = Gbl.Layout.TablEndWritten = true;
      return;
     }

   /***** If serving a web service ==> don't do anything *****/
   if (Gbl.WebService.IsWebService)
     {
      Gbl.Layout.WritingHTMLStart = false;
      Gbl.Layout.HTMLStartWritten = Gbl.Layout.TablEndWritten = true;
      return;
     }

   Gbl.Layout.WritingHTMLStart = true;

   /***** Write header to standard output to avoid timeout *****/
   // Two \r\n are necessary
   fprintf (stdout,"Content-type: text/html; charset=windows-1252\r\n\r\n"
                   "<!DOCTYPE html>\n");

   /***** Write start of HTML code *****/
   // WARNING: It is necessary to comment the line AddDefaultCharset UTF8 in httpd.conf to enable meta tag
   fprintf (Gbl.F.Out,"<html lang=\"%s\">\n"
                      "<head>\n"
                      "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=windows-1252\" />\n",
            Txt_STR_LANG_ID[Gbl.Prefs.Language]);

   /* Title */
   Lay_WritePageTitle ();

   /* Favicon */
   fprintf (Gbl.F.Out,"<link type=\"image/x-icon\" href=\"%s/favicon.ico\" rel=\"icon\" />\n"
                      "<link type=\"image/x-icon\" href=\"%s/favicon.ico\" rel=\"shortcut icon\" />\n",
	    Gbl.Prefs.IconsURL,
	    Gbl.Prefs.IconsURL);

   /* Style sheet for SWAD */
   fprintf (Gbl.F.Out,"<link rel=\"StyleSheet\" href=\"%s/%s\" type=\"text/css\" />\n",
            Cfg_HTTPS_URL_SWAD_PUBLIC,
            (Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP) ? "swad_desktop.css" :
        	                                       "swad_mobile.css");

   /* Style sheet for Dropzone.js (http://www.dropzonejs.com/) */
   // The public directory dropzone must hold:
   // dropzone.js
   // css/dropzone.css
   // images/spritemap@2x.png
   // images/spritemap.png
   if (Gbl.CurrentAct == ActFrmCreDocCrs ||	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
       Gbl.CurrentAct == ActFrmCreDocGrp ||	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
       Gbl.CurrentAct == ActFrmCreComCrs ||	// Brw_FILE_BRW_COMMON_CRS
       Gbl.CurrentAct == ActFrmCreComGrp ||	// Brw_FILE_BRW_COMMON_GRP
       Gbl.CurrentAct == ActFrmCreAsgUsr ||	// Brw_FILE_BRW_ASSIGNMENTS_USR
       Gbl.CurrentAct == ActFrmCreAsgCrs ||	// Brw_FILE_BRW_ASSIGNMENTS_CRS
       Gbl.CurrentAct == ActFrmCreWrkUsr ||	// Brw_FILE_BRW_WORKS_USR
       Gbl.CurrentAct == ActFrmCreWrkCrs ||	// Brw_FILE_BRW_WORKS_CRS
       Gbl.CurrentAct == ActFrmCreMrkCrs ||	// Brw_FILE_BRW_ADMIN_MARKS_CRS
       Gbl.CurrentAct == ActFrmCreMrkGrp ||	// Brw_FILE_BRW_ADMIN_MARKS_GRP
       Gbl.CurrentAct == ActFrmCreBrf)		// Brw_FILE_BRW_BRIEFCASE_USR
      fprintf (Gbl.F.Out,"<link rel=\"StyleSheet\" href=\"%s/dropzone/css/dropzone.css\" type=\"text/css\" />\n",
               Cfg_HTTPS_URL_SWAD_PUBLIC);

   /* Redirect to correct language */
   if ((Gbl.CurrentAct == ActAutUsrInt ||
        Gbl.CurrentAct == ActAutUsrExt) &&					// Action is log in
       Gbl.Usrs.Me.Logged &&							// I am just logged
       Gbl.Usrs.Me.UsrDat.Prefs.Language != Txt_Current_CGI_SWAD_Language)	// My language != current language
      Lay_WriteRedirectionToMyLanguage ();

   /* Write initial scripts depending on the action */
   Lay_WriteScripts ();

   fprintf (Gbl.F.Out,"</head>\n");

   /***** HTML body *****/
   fprintf (Gbl.F.Out,"<body");
   if (Act_Actions[Gbl.CurrentAct].BrowserWindow != Act_MAIN_WINDOW)
     {
      fprintf (Gbl.F.Out,">\n");
      Gbl.Layout.WritingHTMLStart = false;
      Gbl.Layout.HTMLStartWritten =
      Gbl.Layout.TablEndWritten   = true;
      return;
     }
   fprintf (Gbl.F.Out," onload=\"init()\">\n");

   if (Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW)
      fprintf (Gbl.F.Out,"<div id=\"zoomLyr\" class=\"ZOOM\">"
                         "<img id=\"zoomImg\" src=\"%s/_.gif\" alt=\"\" class=\"IMG_USR\" />"
                         "<div id=\"zoomTxt\" style=\"text-align:center;\">"
                         "</div>"
                         "</div>",
	       Gbl.Prefs.IconsURL);

   /***** Header of layout *****/
   fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">");

   Lay_WritePageTopHeading ();

   switch (Gbl.Prefs.Layout)
     {
      case Lay_LAYOUT_DESKTOP:
	 if (Gbl.Prefs.SideCols == Lay_SHOW_BOTH_COLUMNS)
	    ColspanCentralPart = 1;	// 11: both side columns visible, left and right
	 else if (Gbl.Prefs.SideCols == Lay_HIDE_BOTH_COLUMNS)
	    ColspanCentralPart = 3;	// 00: both side columns hidden
	 else
	    ColspanCentralPart = 2;	// 10 or 01: only one side column visible, left or right
	 break;
      case Lay_LAYOUT_MOBILE:
         ColspanCentralPart = 3;
         break;
      default:
      	 break;
     }

   fprintf (Gbl.F.Out,"<td colspan=\"%u\" style=\"text-align:center;"
	              " vertical-align:top;\">"
		      "<div id=\"CENTRAL_ZONE\""
		      " style=\"background-color:%s;\">"
		      "<table style=\"width:100%%; vertical-align:top;\">"
		      "<tr>",
	    ColspanCentralPart,
	    The_TabOnBgColors[Gbl.Prefs.Theme]);

   /***** Central (main) part *****/
   switch (Gbl.Prefs.Layout)
     {
      case Lay_LAYOUT_DESKTOP:
         /* Left bar used to expand-contract central zone */
         fprintf (Gbl.F.Out,"<td style=\"width:10px; text-align:right;"
                            " vertical-align:top;\">");
         Prf_PutLeftIconToHideShowCols ();
         fprintf (Gbl.F.Out,"</td>");

         if (Gbl.Prefs.Menu == Mnu_MENU_VERTICAL)
           {
	    /* Tab content, including vertical menu (left) and main zone (right) */
	    fprintf (Gbl.F.Out,"<td style=\"width:140px; text-align:left;"
			       " vertical-align:top;\">");
	    Mnu_WriteVerticalMenuThisTabDesktop ();
	    fprintf (Gbl.F.Out,"</td>");
           }
         break;
      case Lay_LAYOUT_MOBILE:
         /* Tab content */
         fprintf (Gbl.F.Out,"<td style=\"text-align:left;"
                            " vertical-align:top;\">");
         Usr_WarningWhenDegreeTypeDoesntAllowDirectLogin ();
         if (Act_Actions[Act_Actions[Gbl.CurrentAct].SuperAction].IndexInMenu < 0)	// Write vertical menu
           {
            if (Gbl.CurrentAct == ActMnu)
               Mnu_WriteMenuThisTabMobile ();
            else
               Lay_DrawTabsMobile ();
           }
         break;
      default:
      	 break;
     }

   /***** Main zone *****/
   /* Start of main zone for actions output */
   fprintf (Gbl.F.Out,"<td style=\"padding:0 10px 10px 10px;"
		      " text-align:left; vertical-align:top;\">");

   if (Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP &&
       Gbl.Prefs.Menu == Mnu_MENU_HORIZONTAL)
      Mnu_WriteHorizontalMenuThisTabDesktop ();

   Usr_WarningWhenDegreeTypeDoesntAllowDirectLogin ();

   /* If it is mandatory to read any information about course */
   if (Gbl.CurrentCrs.Info.ShowMsgMustBeRead)
      Inf_WriteMsgYouMustReadInfo ();

   /* Write title of the current action */
   if (Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP &&
       Gbl.Prefs.Menu == Mnu_MENU_VERTICAL &&
      Act_Actions[Act_Actions[Gbl.CurrentAct].SuperAction].IndexInMenu >= 0)
      Lay_WriteTitleAction ();

   Gbl.Layout.WritingHTMLStart = false;
   Gbl.Layout.HTMLStartWritten = true;

   /* Write new year greeting */
   if (Gbl.CurrentAct == ActAutUsrInt ||
       Gbl.CurrentAct == ActAutUsrExt ||
       Gbl.CurrentAct == ActAutUsrChgLan)
      if (Gbl.Now.Date.Month == 1 &&
	  Gbl.Now.Date.Day == 1)
        {
         fprintf (Gbl.F.Out,"<div class=\"ASG_TITLE\""
                            " style=\"text-align:center; margin:50px;\">");
         fprintf (Gbl.F.Out,Txt_NEW_YEAR_GREETING,Gbl.Now.Date.Year);
         fprintf (Gbl.F.Out,"</div>");
        }

   /* Write message indicating number of clicks allowed before sending my photo */
   Usr_InformAboutNumClicksBeforePhoto ();
  }

/*****************************************************************************/
/************************ Write the end of the page **************************/
/*****************************************************************************/

static void Lay_WriteEndOfPage (void)
  {
   if (!Gbl.Layout.TablEndWritten)
     {
      Gbl.Layout.TablEndWritten = true;
      fprintf (Gbl.F.Out,"</td>");

      if (Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP)
	{
	 /* Right bar used to expand-contract central zone */
	 fprintf (Gbl.F.Out,"<td style=\"width:10px; text-align:left;"
	                    " vertical-align:top;\">");
	 Prf_PutRigthIconToHideShowCols ();
	 fprintf (Gbl.F.Out,"</td>");
	}

      fprintf (Gbl.F.Out,"</tr>"
			 "</table>"
			 "</div>"
			 "</td>"
                         "</tr>"
	                 "</table>\n");
     }
  }

/*****************************************************************************/
/************************* Write the title of the page ***********************/
/*****************************************************************************/

static void Lay_WritePageTitle (void)
  {
   fprintf (Gbl.F.Out,"<title>");

   if (Gbl.GetMethod && Gbl.CurrentDeg.Deg.DegCod > 0)
     {
      fprintf (Gbl.F.Out,"%s &gt; %s",
	       Cfg_PLATFORM_PAGE_TITLE,
	       Gbl.CurrentDeg.Deg.ShortName);
      if (Gbl.CurrentCrs.Crs.CrsCod > 0)
         fprintf (Gbl.F.Out," &gt; %s",
                  Gbl.CurrentCrs.Crs.ShortName);
     }
   else
      fprintf (Gbl.F.Out,"%s",
	       Cfg_PLATFORM_PAGE_TITLE);

   fprintf (Gbl.F.Out,"</title>");
  }

/*****************************************************************************/
/************* Write script and meta to redirect to my language **************/
/*****************************************************************************/

static void Lay_WriteRedirectionToMyLanguage (void)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];

   fprintf (Gbl.F.Out,"<meta http-equiv=\"refresh\" content=\"0; url='%s/%s?ActCod=%ld&IdSes=%s'\">",
	    Cfg_HTTPS_URL_SWAD_CGI,
	    Txt_STR_LANG_ID[Gbl.Usrs.Me.UsrDat.Prefs.Language],
	    Act_Actions[ActAutUsrChgLan].ActCod,
	    Gbl.Session.Id);
  }

/*****************************************************************************/
/************ Write some scripts depending on the current action *************/
/*****************************************************************************/

static void Lay_WriteScripts (void)
  {
   /***** General scripts for swad *****/
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\" src=\"%s/swad.js\">"
                      "</script>\n",
	    Cfg_HTTPS_URL_SWAD_PUBLIC);
   if (Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW)
     {
      Lay_WriteScriptInit ();
      Lay_WriteScriptConnectedUsrs ();
     }

   /***** Script for uploading files using Dropzone.js (http://www.dropzonejs.com/) *****/
   // The public directory dropzone must hold:
   // dropzone.js
   // css/dropzone.css
   // images/spritemap@2x.png
   // images/spritemap.png
   if (Gbl.CurrentAct == ActFrmCreDocCrs ||	// Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS
       Gbl.CurrentAct == ActFrmCreDocGrp ||	// Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP
       Gbl.CurrentAct == ActFrmCreComCrs ||	// Brw_FILE_BRW_COMMON_CRS
       Gbl.CurrentAct == ActFrmCreComGrp ||	// Brw_FILE_BRW_COMMON_GRP
       Gbl.CurrentAct == ActFrmCreAsgUsr ||	// Brw_FILE_BRW_ASSIGNMENTS_USR
       Gbl.CurrentAct == ActFrmCreAsgCrs ||	// Brw_FILE_BRW_ASSIGNMENTS_CRS
       Gbl.CurrentAct == ActFrmCreWrkUsr ||	// Brw_FILE_BRW_WORKS_USR
       Gbl.CurrentAct == ActFrmCreWrkCrs ||	// Brw_FILE_BRW_WORKS_CRS
       Gbl.CurrentAct == ActFrmCreMrkCrs ||	// Brw_FILE_BRW_ADMIN_MARKS_CRS
       Gbl.CurrentAct == ActFrmCreMrkGrp ||	// Brw_FILE_BRW_ADMIN_MARKS_GRP
       Gbl.CurrentAct == ActFrmCreBrf)		// Brw_FILE_BRW_BRIEFCASE_USR
     {
      // Use charset="windows-1252" to force error messages in windows-1252 (default is UTF-8)
      fprintf (Gbl.F.Out,"<script type=\"text/javascript\" src=\"%s/dropzone/dropzone.js\""
	                 " charset=\"windows-1252\">"
			 "</script>\n",
	       Cfg_HTTPS_URL_SWAD_PUBLIC);
      Lay_WriteScriptCustomDropzone ();
     }

   /***** Script to print world map *****/
   if (Gbl.CurrentAct == ActSeeCty)
      Cty_WriteScriptGoogleGeochart ();

   /***** Script for Google Analytics *****/
   fprintf (Gbl.F.Out,"%s",Cfg_GOOGLE_ANALYTICS);
  }

// Change page title
//function changeTitle(title) {
//	document.title = title;
//}

/*****************************************************************************/
/******* Write script with init function executed after loading page *********/
/*****************************************************************************/

static void Lay_WriteScriptInit (void)
  {
   extern const char *Txt_MONTHS_SMALL_SHORT[12];
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];

   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">\n");

   fprintf (Gbl.F.Out,"function init(){\n");

   if (Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP)	// Clock visible
    {
      fprintf (Gbl.F.Out,"	IsToday = true;\n");
      fprintf (Gbl.F.Out,"	StrToday = \"%u %s\";\n",
               Gbl.Now.Date.Day,
               Txt_MONTHS_SMALL_SHORT[Gbl.Now.Date.Month-1]);
      fprintf (Gbl.F.Out,"	Hour = %u;\n",Gbl.Now.Time.Hour);
      fprintf (Gbl.F.Out,"	Minute = %u;\n",Gbl.Now.Time.Minute);
      fprintf (Gbl.F.Out,"	writeClock();\n");
     }

   if (Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP &&
       (Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN))	// Right column visible
      Con_WriteScriptClockConnected ();

   // Put the focus on login form
   fprintf (Gbl.F.Out,"	LoginForm = document.getElementById('UsrId');\n"
                      "	if (LoginForm)\n"
                      "		LoginForm.focus();\n");

   fprintf (Gbl.F.Out,"	ActionAJAX = \"%s\";\n",Txt_STR_LANG_ID[Gbl.Prefs.Language]);
   fprintf (Gbl.F.Out,"	setTimeout(\"refreshConnected()\",%lu);\n",
            Gbl.Usrs.Connected.TimeToRefreshInMs);
   if (Gbl.CurrentAct == ActLstClk)
      fprintf (Gbl.F.Out,"	setTimeout(\"refreshLastClicks()\",%lu);\n",
               Cfg_TIME_TO_REFRESH_LAST_CLICKS);
   fprintf (Gbl.F.Out,"}\n");

   fprintf (Gbl.F.Out,"</script>\n");
  }

/*****************************************************************************/
/************** Write script to show connected users using AJAX **************/
/*****************************************************************************/

static void Lay_WriteScriptConnectedUsrs (void)
  {
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">\n");

   fprintf (Gbl.F.Out,"var RefreshParamNxtActCon = \"ActCod=%ld\";\n",
            Act_Actions[ActRefCon].ActCod);
   fprintf (Gbl.F.Out,"var RefreshParamNxtActLog = \"ActCod=%ld\";\n",
            Act_Actions[ActRefLstClk].ActCod);
   fprintf (Gbl.F.Out,"var RefreshParamIdSes = \"IdSes=%s\";\n",
            Gbl.Session.Id);
   fprintf (Gbl.F.Out,"var RefreshParamCrsCod = \"CrsCod=%ld\";\n",
            Gbl.CurrentCrs.Crs.CrsCod);

   fprintf (Gbl.F.Out,"</script>\n");
  }

/*****************************************************************************/
/******* Write script to customize upload of files using Dropzone.js *********/
/*****************************************************************************/
// More info: http://www.dropzonejs.com/

static void Lay_WriteScriptCustomDropzone (void)
  {
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">\n");

   // "myAwesomeDropzone" is the camelized version of the HTML element's ID
   // Add a line "forceFallback: true,\n" to test classic upload
   fprintf (Gbl.F.Out,"Dropzone.options.myAwesomeDropzone = {\n"
	              "maxFiles: 100,\n"
		      "parallelUploads: 100,\n"
		      "maxFilesize: %lu,\n"
		      "fallback: function() {\n"
		      "document.getElementById('dropzone-upload').style.display='none';\n"
		      "document.getElementById('classic-upload').style.display='block';\n"
		      "}\n"
		      "};\n",
            (unsigned long) (Fil_MAX_FILE_SIZE / (1024ULL * 1024ULL) - 1));

   fprintf (Gbl.F.Out,"</script>\n");
  }

/*****************************************************************************/
/************************ Write top heading of the page **********************/
/*****************************************************************************/

static void Lay_WritePageTopHeading (void)
  {
   extern const char *The_ClassHead[The_NUM_THEMES];
   const struct
     {
      const char *Icon;
      const unsigned Width;
      const unsigned Height;
     } LogoLayout[Lay_NUM_LAYOUTS] =
     {
        {	// Lay_LAYOUT_DESKTOP
         "swad112x32.gif",112,32,
        },
        {	// Lay_LAYOUT_MOBILE
         "swad168x48.gif",168,48,
        },
     };

   fprintf (Gbl.F.Out,"<tr style=\"background-image:url('%s/head_row1_1x48.gif');"
	              " background-repeat:repeat-x;\">",
            Gbl.Prefs.PathTheme);

   /***** 1st. row, 1st. column: logo *****/
   fprintf (Gbl.F.Out,"<td style=\"width:%upx; text-align:center;\">",
            LogoLayout[Gbl.Prefs.Layout].Width + 16);

   /* Left logo */
   fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\">"
	              "<img src=\"%s/%s\" alt=\"%s\""
	              " style=\"width:%upx; height:%upx;"
	              " margin:0 auto; vertical-align:middle;\" />"
                      "</a>",
            Cfg_HTTP_SwAD_HOME_PAGE,Gbl.Prefs.PathTheme,
            LogoLayout[Gbl.Prefs.Layout].Icon,
            Cfg_PLATFORM_FULL_NAME,
            LogoLayout[Gbl.Prefs.Layout].Width,
            LogoLayout[Gbl.Prefs.Layout].Height);
   fprintf (Gbl.F.Out,"</td>");
   fprintf (Gbl.F.Out,"<td style=\"text-align:center; vertical-align:middle;\">"
                      "<table style=\"width:100%%;\">"
                      "<tr>");
   /***** 1st. row, 2nd. column:
          search, and logged user / language selection *****/
   if (Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP)
     {
      /* Search courses / teachers */
      fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
	                 " vertical-align:middle;\">",
               The_ClassHead[Gbl.Prefs.Theme]);
      Act_FormStart ( Gbl.CurrentCrs.Crs.CrsCod > 0 ? ActCrsSch :
	             (Gbl.CurrentDeg.Deg.DegCod > 0 ? ActDegSch :
	             (Gbl.CurrentCtr.Ctr.CtrCod > 0 ? ActCtrSch :
	             (Gbl.CurrentIns.Ins.InsCod > 0 ? ActInsSch :
	             (Gbl.CurrentCty.Cty.CtyCod > 0 ? ActCtySch :
	                                              ActSysSch)))));
      Sco_PutParamScope (Sco_SCOPE_PLATFORM);
      Sch_PutFormToSearch (Gbl.Prefs.PathTheme);
      fprintf (Gbl.F.Out,"</form>"
	                 "</td>");
     }

   /* Logged user or language selection */
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:right;"
	              " vertical-align:middle;\">",
            The_ClassHead[Gbl.Prefs.Theme]);
   if (Gbl.Usrs.Me.Logged)
      Usr_WriteLoggedUsrHead ();
   else
      Prf_PutSelectorToSelectLanguage ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>"
	              "</table>"
	              "</td>");

   /***** 1st. row, 3rd. column: link to open/close session *****/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"width:128px;"
	              " text-align:center; vertical-align:middle;\">",
            The_ClassHead[Gbl.Prefs.Theme]);
   if (Gbl.Usrs.Me.Logged)
      Usr_PutFormLogOut ();
   else
      Usr_PutFormLogIn ();
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");

   /***** 2nd. row *****/
   fprintf (Gbl.F.Out,"<tr style=\"height:64px;"
	              " background-image:url('%s/head_row2_1x64.gif');"
	              " background-repeat:repeat-x;\">",
            Gbl.Prefs.PathTheme);

   switch (Gbl.Prefs.Layout)
     {
      case Lay_LAYOUT_DESKTOP:
         /***** 2nd. row, 1st. column *****/
         /* Clock with hour:minute (server hour is shown) */
         fprintf (Gbl.F.Out,"<td style=\"width:128px; height:64px;"
                            " text-align:center; vertical-align:top;\">");
         Dat_ShowCurrentDateTime ();
         fprintf (Gbl.F.Out,"</td>");	// End of first column

         /***** 2nd. row, 2nd. column: degree and course *****/
         fprintf (Gbl.F.Out,"<td style=\"height:64px;"
                            " text-align:center; vertical-align:top;\">"
                            "<div style=\"text-align:center; padding-top:4px;\">");
         Deg_WriteCtyInsCtrDeg ();
         Crs_WriteSelectorMyCourses ();
         Deg_WriteBigNameCtyInsCtrDegCrs ();
         fprintf (Gbl.F.Out,"</div>"
                            "</td>");

         /***** 2nd. row, 3rd. column *****/
         fprintf (Gbl.F.Out,"<td style=\"width:128px; height:64px;"
                            " text-align:center; vertical-align:top;\">");
         if (Gbl.Usrs.Me.Logged)
           {
            /* Number of new messages (not seen) */
            fprintf (Gbl.F.Out,"<div id=\"msg\" style=\"padding-top:8px;\">");		// Used for AJAX based refresh
            Ntf_WriteNumberOfNewNtfs ();
            fprintf (Gbl.F.Out,"</div>");			// Used for AJAX based refresh
           }
         break;
      case Lay_LAYOUT_MOBILE:
         fprintf (Gbl.F.Out,"<td colspan=\"3\" style=\"height:32px;"
                            " text-align:center; vertical-align:middle;"
                            " background-image:url('%s/head_row2_1x64.gif');"
                            " background-repeat:repeat-x;\">",
                  Gbl.Prefs.PathTheme);
         Deg_WriteCtyInsCtrDeg ();
         Crs_WriteSelectorMyCourses ();
         break;
      default:
      	 break;
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** 3rd. row *****/
   fprintf (Gbl.F.Out,"</tr>"
                      "<tr>");

   switch (Gbl.Prefs.Layout)
     {
      case Lay_LAYOUT_DESKTOP:
         /***** 3rd. row, 1st. column *****/
         if (Gbl.Prefs.SideCols & Lay_SHOW_LEFT_COLUMN)	// Left column visible
            fprintf (Gbl.F.Out,"<td style=\"width:128px;"
        	               " text-align:center; vertical-align:top;"
                               " background-image:url('%s/head_base_background_1x56.gif');"
                               " background-repeat:repeat-x;\">"
                               "</td>",
            Gbl.Prefs.PathTheme);

         /***** 3rd. row, 2nd. column *****/
         Lay_DrawTabs ();

         /***** 3rd. row, 3rd. column *****/
         if (Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN)	// Right column visible
           {
            fprintf (Gbl.F.Out,"<td rowspan=\"2\" style=\"width:128px;"
        	               " text-align:center; vertical-align:top;\">");
            Lay_ShowRightColumn ();
            fprintf (Gbl.F.Out,"</td>");
           }

         fprintf (Gbl.F.Out,"</tr>"
                            "<tr>");

         /***** 4th. row, 1st. column *****/
         if (Gbl.Prefs.SideCols & Lay_SHOW_LEFT_COLUMN)	// Left column visible
           {
            fprintf (Gbl.F.Out,"<td style=\"width:128px; text-align:center;"
        	               " vertical-align:top;\">");
            Lay_ShowLeftColumn ();
            fprintf (Gbl.F.Out,"</td>");	// End of first column
           }
         break;
      case Lay_LAYOUT_MOBILE:
         Lay_DrawTabs ();
         fprintf (Gbl.F.Out,"</tr>"
                            "<tr>");
         break;
      default:
      	 break;
     }
  }

/*****************************************************************************/
/*************** Set current tab depending on current action *****************/
/*****************************************************************************/

void Lay_SetCurrentTab (void)
  {
   Gbl.CurrentTab = Act_Actions[Gbl.CurrentAct].Tab;

   /***** Change action and tab if country, institution, centre or degree
          are incompatible with the current tab *****/
   switch (Gbl.CurrentTab)
     {
      case TabCty:
	 if (Gbl.CurrentCty.Cty.CtyCod <= 0)		// No country selected
	    Gbl.CurrentAct = ActSeeCty;
	 break;
      case TabIns:
	 if (Gbl.CurrentIns.Ins.InsCod <= 0)		// No institution selected
	   {
	    if (Gbl.CurrentCty.Cty.CtyCod > 0)		// Country selected, but no institution selected
	       Gbl.CurrentAct = ActSeeIns;
	    else					// No country selected
	       Gbl.CurrentAct = ActSeeCty;
	  }
	break;
      case TabCtr:
	 if (Gbl.CurrentCtr.Ctr.CtrCod <= 0)		// No centre selected
	   {
	    if (Gbl.CurrentIns.Ins.InsCod > 0)		// Institution selected, but no centre selected
	       Gbl.CurrentAct = ActSeeCtr;
	    else if (Gbl.CurrentCty.Cty.CtyCod > 0)	// Country selected, but no institution selected
	       Gbl.CurrentAct = ActSeeIns;
	    else					// No country selected
	       Gbl.CurrentAct = ActSeeCty;
	   }
         break;
      case TabDeg:
         if (Gbl.CurrentDeg.Deg.DegCod <= 0)		// No degree selected
	   {
	    if (Gbl.CurrentCtr.Ctr.CtrCod > 0)		// Centre selected, but no degree selected
	       Gbl.CurrentAct = ActSeeDeg;
	    else if (Gbl.CurrentIns.Ins.InsCod > 0)	// Institution selected, but no centre selected
	       Gbl.CurrentAct = ActSeeCtr;
	    else if (Gbl.CurrentCty.Cty.CtyCod > 0)	// Country selected, but no institution selected
	       Gbl.CurrentAct = ActSeeIns;
	    else					// No country selected
	       Gbl.CurrentAct = ActSeeCty;
	   }
         break;
      default:
         break;
     }
   Gbl.CurrentTab = Act_Actions[Gbl.CurrentAct].Tab;

   Lay_DisableIncompatibleTabs ();
  }

/*****************************************************************************/
/************************** Disable incompatible tabs ************************/
/*****************************************************************************/

void Lay_DisableIncompatibleTabs (void)
  {
   /***** Set country, institution, centre, degree and course depending on the current tab.
          This will disable tabs incompatible with the current one. *****/
   switch (Gbl.CurrentTab)
     {
      case TabSys:
	 Gbl.CurrentCty.Cty.CtyCod = -1L;
	 // no break
      case TabCty:
	 Gbl.CurrentIns.Ins.InsCod = -1L;
	 // no break
      case TabIns:
	 Gbl.CurrentCtr.Ctr.CtrCod = -1L;
	 // no break
      case TabCtr:
	 Gbl.CurrentDeg.Deg.DegCod = -1L;
	 // no break
      case TabDeg:
	 Gbl.CurrentCrs.Crs.CrsCod = -1L;
	 break;
      default:
         break;
     }
  }

/*****************************************************************************/
/**************** Draw tabs with the current tab highlighted *****************/
/*****************************************************************************/

static void Lay_DrawTabs (void)
  {
   unsigned ColspanCentralPart = 3;

   if (Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP)
     {
      if (Gbl.Prefs.SideCols == Lay_SHOW_BOTH_COLUMNS)	// 11: both side columns visible, left and right
	 ColspanCentralPart = 1;
      else if (Gbl.Prefs.SideCols != Lay_HIDE_BOTH_COLUMNS)	// 10 or 01: only one side column visible, left or right
	 ColspanCentralPart = 2;
     }
   fprintf (Gbl.F.Out,"<td colspan=\"%u\" style=\"height:56px;"
	              " text-align:center; vertical-align:top;"
		      " background-image: url('%s/head_base_background_1x56.gif');"
		      " background-repeat:repeat-x;\">"
		      "<div id=\"tabs_container\">",
            ColspanCentralPart,Gbl.Prefs.PathTheme);
   switch (Gbl.Prefs.Layout)
     {
      case Lay_LAYOUT_DESKTOP:
         Lay_DrawTabsDeskTop ();
         break;
      case Lay_LAYOUT_MOBILE:
         Lay_DrawBreadcrumb ();
         break;
      default:
      	 break;
     }
   fprintf (Gbl.F.Out,"</div>"
	              "</td>");
  }

/*****************************************************************************/
/**************** Draw tabs with the current tab highlighted *****************/
/*****************************************************************************/

static void Lay_DrawTabsDeskTop (void)
  {
   extern const char *The_ClassTabOn[The_NUM_THEMES];
   extern const char *The_ClassTabOff[The_NUM_THEMES];
   extern const char *The_TabOnBgColors[The_NUM_THEMES];
   extern const char *The_TabOffBgColors[The_NUM_THEMES];
   extern const char *Txt_TABS_FULL_TXT[Act_NUM_TABS];
   extern const char *Txt_TABS_SHORT_TXT[Act_NUM_TABS];
   Act_Tab_t NumTab;
   bool ICanViewTab;

   /***** Table start *****/
   fprintf (Gbl.F.Out,"<ul style=\"list-style-type:none; padding:0; margin:0;\">");

   /***** Draw the tabs *****/
   for (NumTab = (Act_Tab_t) 1;
        NumTab <= (Act_Tab_t) Act_NUM_TABS - 1;
        NumTab++)
     {
      ICanViewTab = Lay_CheckIfICanViewTab (NumTab);

      /* If current tab is unknown, then activate the first one with access allowed */
      if (Gbl.CurrentTab == TabUnk)
	{
	 Gbl.CurrentTab = NumTab;
	 Lay_DisableIncompatibleTabs ();
	}

      if (ICanViewTab || NumTab > TabCrs)	// Don't show the first hidden tabs
	{
	 /* Form, icon (at top) and text (at bottom) of the tab */
	 fprintf (Gbl.F.Out,"<li class=\"%s\" style=\"background-color:%s;\">",
		  NumTab == Gbl.CurrentTab ? "TAB_ON" :
					     "TAB_OFF",
		  NumTab == Gbl.CurrentTab ? The_TabOnBgColors[Gbl.Prefs.Theme] :
					     The_TabOffBgColors[Gbl.Prefs.Theme]);
	 if (ICanViewTab)
	   {
	    fprintf (Gbl.F.Out,"<div");	// This div must be present even in current tab in order to render properly the tab
	    if (NumTab != Gbl.CurrentTab)
	       fprintf (Gbl.F.Out," class=\"ICON_HIGHLIGHT\"");
	    fprintf (Gbl.F.Out,">");
	    Act_FormStart (ActMnu);
	    Par_PutHiddenParamUnsigned ("NxtTab",(unsigned) NumTab);
	    Act_LinkFormSubmit (Txt_TABS_FULL_TXT[NumTab],
				NumTab == Gbl.CurrentTab ? The_ClassTabOn[Gbl.Prefs.Theme] :
							   The_ClassTabOff[Gbl.Prefs.Theme]);
	    fprintf (Gbl.F.Out,"<img src=\"%s/%s/%s32x32.gif\""
			       " alt=\"%s\" title=\"%s\""
			       " class=\"ICON32x32\" style=\"margin:4px;\" />"
			       "<div>%s</div>"
			       "</a>"
			       "</form>",
		     Gbl.Prefs.PathIconSet,Cfg_ICON_ACTION_32x32,
		     Lay_TabIcons[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     Txt_TABS_SHORT_TXT[NumTab]);
	   }
	 else
	    fprintf (Gbl.F.Out,"<div class=\"ICON_HIDDEN\">"
			       "<img src=\"%s/%s/%s32x32.gif\""
			       " alt=\"%s\" title=\"%s\""
			       " class=\"ICON32x32\" style=\"margin:4px;\" />"
			       "<div class=\"%s\">%s</div>",
		     Gbl.Prefs.PathIconSet,Cfg_ICON_ACTION_32x32,
		     Lay_TabIcons[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     The_ClassTabOff[Gbl.Prefs.Theme],
		     Txt_TABS_SHORT_TXT[NumTab]);

	 fprintf (Gbl.F.Out,"</div>"
			    "</li>");
	}
     }

   /***** End of the table *****/
   fprintf (Gbl.F.Out,"</ul>");
  }

/*****************************************************************************/
/************************ Draw vertical menu with tabs ***********************/
/*****************************************************************************/

static void Lay_DrawTabsMobile (void)
  {
   extern const char *The_ClassMenuOff[The_NUM_THEMES];
   extern const char *Txt_TABS_FULL_TXT[Act_NUM_TABS];
   unsigned NumTabVisible;
   Act_Tab_t NumTab;
   bool ICanViewTab;

   /***** Table start *****/
   fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">");

   /***** Loop to write all tabs. Each row holds a tab *****/
   for (NumTabVisible = 0, NumTab = (Act_Tab_t) 1;
        NumTab <= (Act_Tab_t) Act_NUM_TABS - 1;
        NumTab++)
     {
      ICanViewTab = Lay_CheckIfICanViewTab (NumTab);

      if (ICanViewTab || NumTab > TabCrs)	// Don't show the first hidden tabs
	{
	 if (NumTabVisible % Cfg_LAYOUT_MOBILE_NUM_COLUMNS == 0)
	    fprintf (Gbl.F.Out,"<tr>");

	 /* Icon at top and text at bottom */
	 fprintf (Gbl.F.Out,"<td style=\"width:25%%; text-align:center;"
	                    " vertical-align:top;\">");
	 if (ICanViewTab)
	   {
	    fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\">");
	    Act_FormStart (ActMnu);
	    Par_PutHiddenParamUnsigned ("NxtTab",(unsigned) NumTab);
	    Act_LinkFormSubmit (Txt_TABS_FULL_TXT[NumTab],The_ClassMenuOff[Gbl.Prefs.Theme]);
	    fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s/%s64x64.gif\""
			       " alt=\"%s\" title=\"%s\" class=\"ICON64x64\""
			       " style=\"margin:4px;\" />"
			       "<div>%s</div>"
			       "</a>"
			       "</form>"
			       "</div>",
		     Gbl.Prefs.PathIconSet,Cfg_ICON_ACTION_64x64,
		     Lay_TabIcons[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     Txt_TABS_FULL_TXT[NumTab]);
	   }
	 else
	    fprintf (Gbl.F.Out,"<div class=\"ICON_HIDDEN\">"
			       "<img src=\"%s/%s/%s64x64.gif\""
			       " alt=\"%s\" title=\"%s\""
			       " class=\"ICON64x64\" style=\"margin:4px;\" />"
			       "<div class=\"%s\">%s</div>"
			       "</div>",
		     Gbl.Prefs.PathIconSet,
		     Cfg_ICON_ACTION_64x64,
		     Lay_TabIcons[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     The_ClassMenuOff[Gbl.Prefs.Theme],
		     Txt_TABS_FULL_TXT[NumTab]);
	 fprintf (Gbl.F.Out,"</td>");
	 if ((NumTabVisible % Cfg_LAYOUT_MOBILE_NUM_COLUMNS) == (Cfg_LAYOUT_MOBILE_NUM_COLUMNS-1))
	    fprintf (Gbl.F.Out,"</tr>");

	 NumTabVisible++;
	}
     }

   /***** End of the table *****/
   fprintf (Gbl.F.Out,"</table>");
  }

/*****************************************************************************/
/************************* Check if I can view a tab *************************/
/*****************************************************************************/

static bool Lay_CheckIfICanViewTab (Act_Tab_t Tab)
  {
   switch (Tab)
     {
      case TabUnk:
	 return false;
      case TabSys:
	 return (Gbl.CurrentCty.Cty.CtyCod <= 0);
      case TabCty:
	 return (Gbl.CurrentCty.Cty.CtyCod > 0 &&
	         Gbl.CurrentIns.Ins.InsCod <= 0);
      case TabIns:
	 return (Gbl.CurrentIns.Ins.InsCod > 0 &&
	         Gbl.CurrentCtr.Ctr.CtrCod <= 0);
      case TabCtr:
	 return (Gbl.CurrentCtr.Ctr.CtrCod > 0 &&
	         Gbl.CurrentDeg.Deg.DegCod <= 0);
      case TabDeg:
	 return (Gbl.CurrentDeg.Deg.DegCod > 0 &&
	         Gbl.CurrentCrs.Crs.CrsCod <= 0);
      case TabCrs:
      case TabAss:
	 return (Gbl.CurrentCrs.Crs.CrsCod > 0);
      case TabMsg:
	 return (Gbl.Usrs.Me.Logged ||
	         Gbl.CurrentCrs.Crs.CrsCod > 0);
      default:
	 return true;
     }
  }

/*****************************************************************************/
/********************* Draw breadcrumb with tab and action *******************/
/*****************************************************************************/

static void Lay_DrawBreadcrumb (void)
  {
   extern const char *The_TabOnBgColors[The_NUM_THEMES];
   extern const char *The_ClassTabOn[The_NUM_THEMES];

   fprintf (Gbl.F.Out,"<div class=\"TAB_ON\" style=\"background-color:%s;\">",
	    The_TabOnBgColors[Gbl.Prefs.Theme]);

   /***** Home *****/
   Lay_WriteBreadcrumbHome ();

   if (Gbl.CurrentAct == ActMnu ||
       Act_Actions[Act_Actions[Gbl.CurrentAct].SuperAction].IndexInMenu >= 0)
     {
      /***** Tab *****/
      fprintf (Gbl.F.Out,"<span class=\"%s\"> &gt; </span>",
               The_ClassTabOn[Gbl.Prefs.Theme]);
      Lay_WriteBreadcrumbTab ();

      if (Act_Actions[Act_Actions[Gbl.CurrentAct].SuperAction].IndexInMenu >= 0)
        {
         /***** Menu *****/
         fprintf (Gbl.F.Out,"<span class=\"%s\"> &gt; </span>",
                  The_ClassTabOn[Gbl.Prefs.Theme]);
         Lay_WriteBreadcrumbAction ();
        }
     }

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************************ Write home in breadcrumb ***************************/
/*****************************************************************************/

static void Lay_WriteBreadcrumbHome (void)
  {
   extern const char *The_ClassTabOn[The_NUM_THEMES];
   extern const char *Txt_Home_PAGE;

   Act_FormStart (ActHom);
   Act_LinkFormSubmit (Txt_Home_PAGE,The_ClassTabOn[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s</a>"
	              "</form>",
	    Txt_Home_PAGE);
  }

/*****************************************************************************/
/************ Write icon and title associated to the current tab *************/
/*****************************************************************************/

static void Lay_WriteBreadcrumbTab (void)
  {
   extern const char *The_ClassTabOn[The_NUM_THEMES];
   extern const char *Txt_TABS_FULL_TXT[Act_NUM_TABS];

   /***** Start form *****/
   Act_FormStart (ActMnu);
   Par_PutHiddenParamUnsigned ("NxtTab",(unsigned) Gbl.CurrentTab);
   Act_LinkFormSubmit (Txt_TABS_FULL_TXT[Gbl.CurrentTab],The_ClassTabOn[Gbl.Prefs.Theme]);

   /***** Title and end of form *****/
   fprintf (Gbl.F.Out,"%s</a>"
	              "</form>",
	    Txt_TABS_FULL_TXT[Gbl.CurrentTab]);
  }

/*****************************************************************************/
/***************** Write title associated to the current action **************/
/*****************************************************************************/

static void Lay_WriteBreadcrumbAction (void)
  {
   extern const char *The_ClassTabOn[The_NUM_THEMES];
   const char *Title = Act_GetTitleAction (Gbl.CurrentAct);

   /***** Start form *****/
   Act_FormStart (Act_Actions[Gbl.CurrentAct].SuperAction);
   Act_LinkFormSubmit (Title,The_ClassTabOn[Gbl.Prefs.Theme]);

   /***** Title and end of form *****/
   fprintf (Gbl.F.Out,"%s</a>"
	              "</form>",
	    Title);
  }

/*****************************************************************************/
/*********** Write icon and title associated to the current action ***********/
/*****************************************************************************/

static void Lay_WriteTitleAction (void)
  {
   extern const char *The_ClassTitleAction[The_NUM_THEMES];
   extern const char *The_ClassSubtitleAction[The_NUM_THEMES];
   extern const char *Txt_TABS_FULL_TXT[Act_NUM_TABS];

   /***** Container start *****/
   fprintf (Gbl.F.Out,"<div id=\"action_title\""
	              " style=\"background-image: url('%s/%s/%s64x64.gif');\">",
	    Gbl.Prefs.PathIconSet,Cfg_ICON_ACTION_64x64,
	    Act_Actions[Act_Actions[Gbl.CurrentAct].SuperAction].Icon);

   /***** Title *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s &gt; %s</div>",
	    The_ClassTitleAction[Gbl.Prefs.Theme],
	    Txt_TABS_FULL_TXT[Act_Actions[Gbl.CurrentAct].Tab],
	    Act_GetTitleAction (Gbl.CurrentAct));

   /***** Subtitle *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s</div>",
	    The_ClassSubtitleAction[Gbl.Prefs.Theme],
	    Act_GetSubtitleAction (Gbl.CurrentAct));

   /***** Container end *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/***************************** Show left column ******************************/
/*****************************************************************************/

static void Lay_ShowLeftColumn (void)
  {
   struct Act_ListMFUActions ListMFUActions;

   fprintf (Gbl.F.Out,"<table style=\"width:128px;\">"
                      "<tr>"
                      "<td style=\"text-align:left; vertical-align:top;\">"
                      "<table style=\"width:128px; border-spacing:4px;\">");

   /***** Most frequently used actions *****/
   if (Gbl.Usrs.Me.Logged)
     {
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"MFU_ACT\" style=\"text-align:center;"
	                 " vertical-align:top;\">");
      Act_AllocateMFUActions (&ListMFUActions,6);
      Act_GetMFUActions (&ListMFUActions,6);
      Act_WriteSmallMFUActions (&ListMFUActions);
      Act_FreeMFUActions (&ListMFUActions);
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");
     }

   /***** Month *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td style=\"text-align:center; vertical-align:top;\">");
   Cal_DrawCurrentMonth ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Notices (yellow notes) *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
     {
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td style=\"text-align:center;"
	                 " vertical-align:top;\">");
      Not_ShowNotices (Not_LIST_BRIEF_NOTICES);
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");
     }

   fprintf (Gbl.F.Out,"</table>"
	              "</td>"
	              "</tr>"
	              "</table>");
  }

/*****************************************************************************/
/**************************** Show right column ******************************/
/*****************************************************************************/

static void Lay_ShowRightColumn (void)
  {
   extern const char *The_ClassConnected[The_NUM_THEMES];
   extern const char *Txt_Connected_users;
   extern const char *Txt_Connected_PLURAL;
   extern const char *Txt_If_you_have_an_Android_device_try_SWADroid;

   Gbl.Usrs.Connected.WhereToShow = Con_SHOW_ON_RIGHT_COLUMN;

   /***** Connected users *****/
   fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">"
                      "<tr>"
                      "<td style=\"text-align:right; vertical-align:top;"
                      " background-image:url('%s/head_base_background_1x56.gif');"
                      " background-repeat:repeat-x;\">"
                      "<table style=\"width:100%%; padding-top:56px;"
                      " border-spacing:4px;\">",
            Gbl.Prefs.PathTheme);

   /***** Banners *****/
   Ban_WriteMenuWithBanners ();

   fprintf (Gbl.F.Out,"<tr>"
	              "<td style=\"text-align:center;\">");
   Act_FormStart (ActLstCon);
   Act_LinkFormSubmit (Txt_Connected_users,The_ClassConnected[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s</a>"
		      "</form>",
            Txt_Connected_PLURAL);

   /***** Number of connected users in the whole platform *****/
   fprintf (Gbl.F.Out,"<div id=\"globalconnected\">");	// Used for AJAX based refresh
   Con_ShowGlobalConnectedUsrs ();
   fprintf (Gbl.F.Out,"</div>");			// Used for AJAX based refresh

   /***** Number of connected users in the current course *****/
   fprintf (Gbl.F.Out,"<div id=\"courseconnected\">");	// Used for AJAX based refresh
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// There is a course selected
     {
      Gbl.Scope.Current = Sco_SCOPE_COURSE;
      Con_ShowConnectedUsrsBelongingToScope ();
     }
   fprintf (Gbl.F.Out,"</div>");			// Used for AJAX based refresh

   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** SWADroid advertisement *****/
   if (!Gbl.Usrs.Me.Logged ||
       Gbl.CurrentAct == ActAutUsrInt ||
       Gbl.CurrentAct == ActAutUsrExt)
      fprintf (Gbl.F.Out,"<tr>"
			 "<td style=\"text-align:center;\">"
			 "<a href=\"https://play.google.com/store/apps/details?id=es.ugr.swad.swadroid\""
			 " target=\"_blank\" title=\"%s\">"
			 "<img src=\"%s/SWADroid120x200.png\" alt=\"SWADroid\""
			 " style=\"width:120px; height:200px;\" />"
			 "</a>"
			 "</td>"
			 "</tr>",
	       Txt_If_you_have_an_Android_device_try_SWADroid,
	       Gbl.Prefs.IconsURL);

   /***** Institutional links *****/
   Lnk_WriteMenuWithInstitutionalLinks ();

   fprintf (Gbl.F.Out,"</table>"
	              "</td>"
	              "</tr>"
	              "</table>");
  }

/*****************************************************************************/
/************************* Put a link (form) to edit *************************/
/*****************************************************************************/

void Lay_PutFormToEdit (Act_Action_t Action)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Edit;

   fprintf (Gbl.F.Out,"<div style=\"text-align:center; margin-bottom:10px;\">");
   Act_FormStart (Action);
   Act_LinkFormSubmit (Txt_Edit,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("edit",Txt_Edit,Txt_Edit);
   fprintf (Gbl.F.Out,"</form>"
	              "</div>");
  }

/*****************************************************************************/
/**************** Put a icon with a text to submit a form ********************/
/*****************************************************************************/

void Lay_PutSendIcon (const char *Icon,const char *Alt,const char *Text)
  {
   // margin-left is used because this form link may be placed after another one
   fprintf (Gbl.F.Out,"<img src=\"%s/%s16x16.gif\" alt=\"%s\""
	              " class=\"ICON16x16\" style=\"margin-left:10px;\" />"
                      "&nbsp;%s</a>",
            Gbl.Prefs.IconsURL,Icon,Alt,Text);
  }

/*****************************************************************************/
/********************** Put a button to submit a form ************************/
/*****************************************************************************/

void Lay_PutSendButton (const char *TextSendButton)
  {
   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">"
                      "<input type=\"submit\" value=\"%s\" />"
                      "</div>",
            TextSendButton);
  }

/*****************************************************************************/
/************ Write a centered message with the title of a table *************/
/*****************************************************************************/

void Lay_WriteTitle (const char *Title)
  {
   fprintf (Gbl.F.Out,"<div class=\"TIT\">"
	              "%s"
	              "</div>",
	    Title);
  }

/*****************************************************************************/
/****************** Start and end a table with rounded frame *****************/
/*****************************************************************************/
// CellPadding must be 0, 1, 2, 4 or 8

void Lay_StartRoundFrameTable10 (const char *Width,unsigned CellPadding,const char *Title)
  {
   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">"
                      "<div class=\"FRAME10\"");
   if (Width)
      fprintf (Gbl.F.Out," style=\"width:%s;\"",Width);
   fprintf (Gbl.F.Out,">");

   if (Title)
      fprintf (Gbl.F.Out,"<div class=\"TIT_TBL_10\""
	                 " style=\"text-align:center;\">"
	                 "%s"
	                 "</div>",
	       Title);

   fprintf (Gbl.F.Out,"<table class=\"TABLE10");
   if (CellPadding)
      fprintf (Gbl.F.Out," CELLS_PAD_%u",CellPadding);	// CellPadding must be 0, 1, 2, 4 or 8
   fprintf (Gbl.F.Out,"\">");
  }

// CellPadding must be 0, 1, 2, 4 or 8

void Lay_StartRoundFrameTable10Shadow (const char *Width,unsigned CellPadding)
  {
   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">"
                      "<div class=\"FRAME10_SHADOW\"");
   if (Width)
      fprintf (Gbl.F.Out," style=\"width:%s\"",
               Width);
   fprintf (Gbl.F.Out,">"
                      "<table class=\"TABLE10");
   if (CellPadding)
      fprintf (Gbl.F.Out," CELLS_PAD_%u",CellPadding);	// CellPadding must be 0, 1, 2, 4 or 8
   fprintf (Gbl.F.Out,"\">");
  }

void Lay_EndRoundFrameTable10 (void)
  {
   fprintf (Gbl.F.Out,"</table>"
		      "</div>"
		      "</div>");
  }

/*****************************************************************************/
/******* Write error message, close files, remove lock file, and exit ********/
/*****************************************************************************/

void Lay_ShowErrorAndExit (const char *Message)
  {
   /***** Unlock tables if locked *****/
   if (Gbl.DB.LockedTables)
     {
      Gbl.DB.LockedTables = false;
      mysql_query (&Gbl.mysql,"UNLOCK TABLES");
     }

   if (!Gbl.WebService.IsWebService)
     {
      /***** Write possible error message *****/
      Lay_ShowAlert (Lay_ERROR,Message);

      /***** Finish the page, except </body> and </html> *****/
      Lay_WriteEndOfPage ();
     }

   /***** Free memory and close all the open files *****/
   Gbl_Cleanup ();

   /***** Page is generated (except </body> and </html>). Compute time to generate page *****/
   if (Gbl.CurrentAct != ActRefCon &&	// Refreshing connected users
       Gbl.CurrentAct != ActRefLstClk)	// Refreshing last clics
      Sta_ComputeTimeToGeneratePage ();

   /***** Send page. The HTML output is now in Gbl.F.Out file ==> copy it to standard output *****/
   if (!Gbl.WebService.IsWebService)
     {
      rewind (Gbl.F.Out);
      Fil_FastCopyOfOpenFiles (Gbl.F.Out,stdout);
      Fil_CloseAndRemoveFileForHTMLOutput ();
     }

   if (Gbl.WebService.IsWebService)		// Serving a plugin request
     {
      /***** Log access *****/
      Gbl.TimeSendInMicroseconds = 0L;
      Sta_LogAccess (Message);
     }
   else if (Gbl.CurrentAct != ActRefCon &&	// Refreshing connected users
            Gbl.CurrentAct != ActRefLstClk)	// Refreshing last clicks
     {
      /***** Compute time to send page *****/
      Sta_ComputeTimeToSendPage ();

      /***** Write page footer *****/
      if (Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW)
         Lay_WritePageFooter ();

      /***** Log access *****/
      Sta_LogAccess (Message);

      /***** End the output *****/
      if (!Gbl.Layout.HTMLEndWritten)
        {
         fprintf (Gbl.F.Out,"</body>\n</html>\n");
         Gbl.Layout.HTMLEndWritten = true;
        }
     }

   /***** Close database connection *****/
   DB_CloseDBConnection ();

   /***** Exit *****/
   if (Gbl.WebService.IsWebService)
      Svc_Exit (Message);
   exit (0);
  }

/*****************************************************************************/
/******************** Show an alert message to the user **********************/
/*****************************************************************************/

void Lay_ShowAlert (Lay_AlertType_t MsgType,const char *Message)
  {
   static const char *MsgIcons[Lay_NUM_ALERT_TYPES] =
     {
      "info",
      "success",
      "warning",
      "error",
     };

   /****** If start of page is not written yet, do it now ******/
   if (!Gbl.Layout.HTMLStartWritten)
      Lay_WriteStartOfPage ();

   if (Message)
      fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">"
	                 "<div class=\"ALERT\""
	                 " style=\"background-image:url('%s/%s16x16.gif');\">"
                         "%s"
			 "</div>"
			 "</div>",
	       Gbl.Prefs.IconsURL,MsgIcons[MsgType],
	       Message);
  }

/*****************************************************************************/
/*********** Refresh notifications and connected users via AJAX **************/
/*****************************************************************************/

void Lay_RefreshNotifsAndConnected (void)
  {
   unsigned NumUsr;
   bool ShowConnected = (Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP &&
                         (Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN) &&
                         Gbl.CurrentCrs.Crs.CrsCod > 0);	// Right column visible && There is a course selected

   // Sometimes, someone must do this work, so who best than processes that refresh via AJAX?
   if (!(Gbl.PID % 11))		// Do this only one of   11 times (  11 is prime)
      Ntf_SendPendingNotifByEMailToAllUsrs ();	// Send pending notifications by e-mail
   else if (!(Gbl.PID % 1013))	// Do this only one of 1013 times (1013 is prime)
      Brw_RemoveExpiredExpandedFolders ();	// Remove old expanded folders (from all users)
   else if (!(Gbl.PID % 1019))	// Do this only one of 1019 times (1019 is prime)
      Prf_RemoveOldPrefsFromIP ();		// Remove old preferences from IP
   else if (!(Gbl.PID % 1021))	// Do this only one of 1021 times (1021 is prime)
      Sta_RemoveOldEntriesRecentLog ();		// Remove old entries in recent log table, it's a slow query

   Gbl.Usrs.Connected.WhereToShow = Con_SHOW_ON_RIGHT_COLUMN;

   // Send, before the HTML, the refresh time
   fprintf (Gbl.F.Out,"%lu|",Gbl.Usrs.Connected.TimeToRefreshInMs);
   if (Gbl.Usrs.Me.Logged)
      Ntf_WriteNumberOfNewNtfs ();
   fprintf (Gbl.F.Out,"|");
   Con_ShowGlobalConnectedUsrs ();
   fprintf (Gbl.F.Out,"|");
   if (ShowConnected)
     {
      Gbl.Scope.Current = Sco_SCOPE_COURSE;
      Con_ShowConnectedUsrsBelongingToScope ();
     }
   fprintf (Gbl.F.Out,"|");
   if (ShowConnected)
      fprintf (Gbl.F.Out,"%u",Gbl.Usrs.Connected.NumUsrsToList);
   fprintf (Gbl.F.Out,"|");
   if (ShowConnected)
      for (NumUsr = 0;
	   NumUsr < Gbl.Usrs.Connected.NumUsrsToList;
	   NumUsr++)
         fprintf (Gbl.F.Out,"%u|",Gbl.Usrs.Connected.Lst[NumUsr].Seconds);

   /***** All the output is made, so don't write anymore *****/
   Gbl.Layout.TablEndWritten = Gbl.Layout.HTMLEndWritten = true;
  }

/*****************************************************************************/
/******************** Refresh connected users via AJAX ***********************/
/*****************************************************************************/

void Lay_RefreshLastClicks (void)
  {
   // Send, before the HTML, the refresh time
   fprintf (Gbl.F.Out,"%lu|",Cfg_TIME_TO_REFRESH_LAST_CLICKS);
   Con_GetAndShowLastClicks ();

   /***** All the output is made, so don't write anymore *****/
   Gbl.Layout.TablEndWritten = Gbl.Layout.HTMLEndWritten = true;
  }

/*****************************************************************************/
/*************************** Write footer of page ****************************/
/*****************************************************************************/

void Lay_WritePageFooter (void)
  {
   extern const char *Txt_About_X;
   extern const char *Txt_Questions_and_problems;

   switch (Gbl.Prefs.Layout)
     {
      case Lay_LAYOUT_DESKTOP:
	 Lay_WriteFootFromHTMLFile ();

         fprintf (Gbl.F.Out,"<div class=\"FOOT\" style=\"text-align:center;"
                            " padding-bottom:10px;\">");

         /***** Institution and centre hosting the platform *****/
         fprintf (Gbl.F.Out,"<a href=\"%s\" class=\"FOOT\" target=\"_blank\">"
                            "<img src=\"%s/%s\" alt=\"%s\""
                            " style=\"width:%upx; height:%upx;\" />"
                            "<div>%s</div>"
                            "</a>",
                  Cfg_ABOUT_URL,
                  Gbl.Prefs.IconsURL,Cfg_ABOUT_LOGO,
                  Cfg_ABOUT_NAME,
                  Cfg_ABOUT_LOGO_WIDTH,Cfg_ABOUT_LOGO_HEIGHT,
                  Cfg_ABOUT_NAME);

         fprintf (Gbl.F.Out,"<div>"
                            "<a href=\"%s\" class=\"FOOT\" target=\"_blank\">%s:</a> "
                            "<a href=\"mailto:%s\" class=\"FOOT\" target=\"_blank\">%s</a>"
                            "</div>",
                  Cfg_HELP_WEB,Txt_Questions_and_problems,
                  Cfg_PLATFORM_RESPONSIBLE_E_MAIL,Cfg_PLATFORM_RESPONSIBLE_E_MAIL);

         /***** About *****/
         fprintf (Gbl.F.Out,"<div>"
                            "<a href=\"%s\" class=\"FOOT\" target=\"_blank\">",
                  Cfg_ABOUT_SWAD_URL);
         fprintf (Gbl.F.Out,Txt_About_X,Log_PLATFORM_VERSION);
         fprintf (Gbl.F.Out,"</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");

         /***** Write time to generate and send page *****/
         Sta_WriteTimeToGenerateAndSendPage ();

         fprintf (Gbl.F.Out,"</div>"
                            "</div>");
         break;
      case Lay_LAYOUT_MOBILE:
      default:
      	 break;
     }
  }

/*****************************************************************************/
/************************ Write the end of the page **************************/
/*****************************************************************************/

static void Lay_WriteFootFromHTMLFile (void)
  {
   FILE *FileHTML;

   /***** Open file with the HTML page for the chat *****/
   if ((FileHTML = fopen (Cfg_PATH_AND_FILE_REL_HTML_PRIVATE,"rb")))
     {
      /***** Copy HTML to output file *****/
      Fil_FastCopyOfOpenFiles (FileHTML,Gbl.F.Out);
      fclose (FileHTML);
     }
  }

/*****************************************************************************/
/****** Write header and footer of the class photo or academic calendar ******/
/*****************************************************************************/

void Lay_WriteHeaderClassPhoto (unsigned NumColumns,bool PrintView,bool DrawingClassPhoto,
                                long InsCod,long DegCod,long CrsCod)
  {
   struct Institution Ins;
   struct Degree Deg;
   struct Course Crs;
   unsigned Size = (PrintView ? 64 :
	                        32);

   /***** Get data of institution *****/
   Ins.InsCod = InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);

   /***** Get data of degree *****/
   Deg.DegCod = DegCod;
   Deg_GetDataOfDegreeByCod (&Deg);

   /***** Get data of course *****/
   Crs.CrsCod = CrsCod;
   Crs_GetDataOfCourseByCod (&Crs);

   /***** Table start *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td colspan=\"%u\" style=\"text-align:center;\">"
                      "<table style=\"width:100%%; padding:10px;\">"
                      "<tr>",
            NumColumns);

   /***** First column: institution logo *****/
   fprintf (Gbl.F.Out,"<td style=\"width:64px; text-align:left;"
	              " vertical-align:top;\">");
   if (InsCod > 0)
     {
      if (!PrintView)
         fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\">",Ins.WWW);
      Ins_DrawInstitutionLogo (Ins.Logo,Ins.ShortName,Size,NULL);
      if (!PrintView)
        fprintf (Gbl.F.Out,"</a>");
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** Second column: class photo title *****/
   fprintf (Gbl.F.Out,"<td class=\"TIT_CLASSPHOTO\" style=\"text-align:center;"
	              " vertical-align:middle;\">");
   if (InsCod > 0)
     {
      if (!PrintView)
         fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"TIT_CLASSPHOTO\">",
                  Ins.WWW);
      fprintf (Gbl.F.Out,"%s",Ins.FullName);
      if (!PrintView)
         fprintf (Gbl.F.Out,"</a>");
     }
   if (DegCod > 0)
     {
      if (Ins.InsCod > 0)
         fprintf (Gbl.F.Out," - ");
      if (!PrintView)
         fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"TIT_CLASSPHOTO\">",
                  Deg.WWW);
      fprintf (Gbl.F.Out,"%s",Deg.FullName);
      if (!PrintView)
         fprintf (Gbl.F.Out,"</a>");
     }
   fprintf (Gbl.F.Out,"<br />");
   if (CrsCod > 0)
     {
      fprintf (Gbl.F.Out,"%s",Crs.FullName);
      if (DrawingClassPhoto && !Gbl.Usrs.ClassPhoto.AllGroups)
        {
         fprintf (Gbl.F.Out,"<br />");
         Grp_WriteNamesOfSelectedGrps ();
        }
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** Third column: degree logo *****/
   fprintf (Gbl.F.Out,"<td style=\"width:64px; text-align:right;"
	              " vertical-align:top;\">");
   if (DegCod > 0)
     {
      if (!PrintView)
         fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"TIT_CLASSPHOTO\">",
                  Deg.WWW);
      Deg_DrawDegreeLogo (Deg.Logo,Deg.ShortName,Size,NULL);
      if (!PrintView)
         fprintf (Gbl.F.Out,"</a>");
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** Table end *****/
   fprintf (Gbl.F.Out,"</tr>"
	              "</table>"
	              "</td>"
	              "</tr>");
  }

/*****************************************************************************/
/************************ Put icons to select a layout ***********************/
/*****************************************************************************/

void Lay_PutIconsToSelectLayout (void)
  {
   extern const char *Txt_Layout;
   extern const char *Txt_LAYOUT_NAMES[Lay_NUM_LAYOUTS];
   Lay_Layout_t Layout;

   Lay_StartRoundFrameTable10 (NULL,2,Txt_Layout);
   fprintf (Gbl.F.Out,"<tr>");
   for (Layout = (Lay_Layout_t) 0;
	Layout < Lay_NUM_LAYOUTS;
	Layout++)
     {
      fprintf (Gbl.F.Out,"<td class=\"%s\">",
               Layout == Gbl.Prefs.Layout ? "LAYOUT_ON" :
        	                            "LAYOUT_OFF");
      Act_FormStart (ActChgLay);
      Par_PutHiddenParamUnsigned ("Layout",(unsigned) Layout);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s32x32.gif\""
	                 " alt=\"%s\" title=\"%s\" class=\"ICON32x32B\""
	                 " style=\"margin:0 auto;\" />"
                         "</form>"
                         "</td>",
               Gbl.Prefs.IconsURL,
               Lay_LayoutIcons[Layout],
               Txt_LAYOUT_NAMES[Layout],
               Txt_LAYOUT_NAMES[Layout]);
     }
   fprintf (Gbl.F.Out,"</tr>");
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/******************************** Change layout ******************************/
/*****************************************************************************/

void Lay_ChangeLayout (void)
  {
   char Query[512];

   /***** Get param layout *****/
   Gbl.Prefs.Layout = Lay_GetParamLayout ();

   /***** Store layout in database *****/
   if (Gbl.Usrs.Me.Logged)
     {
      sprintf (Query,"UPDATE usr_data SET Layout='%u' WHERE UsrCod='%ld'",
               (unsigned) Gbl.Prefs.Layout,Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryUPDATE (Query,"can not update your preference about layout");
     }

   /***** Set preferences from current IP *****/
   Prf_SetPrefsFromIP ();
  }

/*****************************************************************************/
/**************************** Get parameter layout ***************************/
/*****************************************************************************/

Lay_Layout_t Lay_GetParamLayout (void)
  {
   char UnsignedStr[1+10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("Layout",UnsignedStr,1+10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Lay_NUM_LAYOUTS)
         return (Lay_Layout_t) UnsignedNum;

   return Lay_LAYOUT_UNKNOWN;
  }

/*****************************************************************************/
/************************* Put link to print view ****************************/
/*****************************************************************************/

void Lay_PutLinkToPrintView1 (Act_Action_t NextAction)
  {
   Act_FormStart (NextAction);
  }

void Lay_PutLinkToPrintView2 (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Print_view;

   Act_LinkFormSubmit (Txt_Print_view,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("print",Txt_Print_view,Txt_Print_view);
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/**************** Show advertisement about mobile version ********************/
/*****************************************************************************/

#define Lay_SHOW_ADVERTISEMENT_ONE_TIME_OF_X	1	// n ==> show advertisement 1/n of the times

void Lay_AdvertisementMobile (void)
  {
   extern const char *Txt_Stay_connected_with_SWADroid;

   if (!(rand () % Lay_SHOW_ADVERTISEMENT_ONE_TIME_OF_X))
     {
      /***** Table start *****/
      Lay_StartRoundFrameTable10 (NULL,8,NULL);

      /***** Show advertisement *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"DAT\" style=\"text-align:center;\">"
	                 "<a href=\"https://play.google.com/store/apps/details?id=es.ugr.swad.swadroid\""
	                 " class=\"DAT\">"
                         "%s<br /><br />"
                         "<img src=\"%s/SWADroid200x300.png\" alt=\"SWADroid\""
                         " style=\"width:200px; height:300px;\" />"
                         "</a>"
	                 "</td>"
	                 "</tr>",
               Txt_Stay_connected_with_SWADroid,
               Gbl.Prefs.IconsURL);

      /***** End table *****/
      Lay_EndRoundFrameTable10 ();
     }
  }

/*****************************************************************************/
/*********************** Indent forum or chat title **************************/
/*****************************************************************************/
// IsLastItemInLevel[] is a vector with at least 1+Level booleans

void Lay_IndentDependingOnLevel (unsigned Level,bool IsLastItemInLevel[])
  {
   unsigned i;

   /***** Indent (from 1 to Level-1) *****/
   for (i = 1;
	i < Level;
	i++)
      fprintf (Gbl.F.Out,"<img src=\"%s/%s20x20.gif\" alt=\"\""
			 " style=\"width:20px; height:20px;"
			 " vertical-align:middle;\" />",
		  Gbl.Prefs.IconsURL,
		  IsLastItemInLevel[i] ? "tr" :
		                         "subleft");

   /***** Level *****/
   fprintf (Gbl.F.Out,"<img src=\"%s/%s20x20.gif\" alt=\"\""
		      " style=\"width:20px; height:20px;"
		      " vertical-align:middle;\" />",
	    Gbl.Prefs.IconsURL,
	    IsLastItemInLevel[Level] ? "subend" :
				       "submid");
  }
