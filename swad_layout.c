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
#include "swad_logo.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_preference.h"
#include "swad_tab.h"
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
/***************************** Private constants *****************************/
/*****************************************************************************/

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

static void Lay_WriteTitleAction (void);

static void Lay_ShowLeftColumn (void);
static void Lay_ShowRightColumn (void);

static void Lay_WriteFootFromHTMLFile (void);

static void Lay_HelpTextEditor (const char *Text,const char *InlineMath,const char *Equation);

/*****************************************************************************/
/*********************** Write the start of the page *************************/
/*****************************************************************************/

void Lay_WriteStartOfPage (void)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const unsigned Txt_Current_CGI_SWAD_Language;
   extern const char *The_TabOnBgColors[The_NUM_THEMES];
   extern const char *Txt_NEW_YEAR_GREETING;
   const char *ClassBackground[The_NUM_THEMES] =
     {
      "WHITE_BACKGROUND",	// The_THEME_WHITE
      "GREY_BACKGROUND",	// The_THEME_GREY
      "BLUE_BACKGROUND",	// The_THEME_BLUE
      "YELLOW_BACKGROUND",	// The_THEME_YELLOW
      };
   unsigned ColspanCentralPart = 3;	// Initialized to avoid warnning

   /***** If, when this function is called, the head is being written
          or the head is already written ==> don't do anything *****/
   if (Gbl.Layout.WritingHTMLStart ||
       Gbl.Layout.HTMLStartWritten)
      return;

   /***** Compute connected users *****/
   if (Gbl.CurrentAct == ActLstCon ||
       (Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP &&
        (Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN) &&
        Gbl.CurrentCrs.Crs.CrsCod > 0))
      // Right column visible && There is a course selected
      Con_ComputeConnectedUsrsBelongingToCurrentCrs ();

   /***** Send head width the file type for the HTTP protocol *****/
   if (Gbl.CurrentAct == ActRefCon ||
       Gbl.CurrentAct == ActRefLstClk)
     // Don't generate a full HTML page, only refresh connected users
     {
      fprintf (Gbl.F.Out,"Content-Type: text/html; charset=windows-1252\r\n\r\n");
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
   // WARNING: It is necessary to comment the line 'AddDefaultCharset UTF8'
   // in httpd.conf to enable meta tag
   fprintf (Gbl.F.Out,"<html lang=\"%s\">\n"
                      "<head>\n"
                      "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=windows-1252\" />\n"
                      "<meta name=\"description\" content=\"A free-software, educational, online tool for managing courses and students.\" />\n"
                      "<meta name=\"keywords\" content=\""
                      "%s,"
                      "SWAD,"
                      "shared workspace at a distance,"
                      "sistema web de apoyo a la docencia,"
                      "SWADroid,"
                      "LMS,"
                      "Learning Management System\" />\n",
            Txt_STR_LANG_ID[Gbl.Prefs.Language],
            Cfg_PLATFORM_SHORT_NAME);

   /* Title */
   Lay_WritePageTitle ();

   /* Canonical URL */
   fprintf (Gbl.F.Out,"<link rel=\"canonical\""
	              " href=\"%s\" />\n",
	    Cfg_HTTPS_URL_SWAD_CGI);

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
   if (Gbl.CurrentAct == ActFrmCreDocIns ||	// Brw_ADMI_DOCUM_INS
       Gbl.CurrentAct == ActFrmCreComIns ||	// Brw_ADMI_SHARE_INS
       Gbl.CurrentAct == ActFrmCreDocCtr ||	// Brw_ADMI_DOCUM_CTR
       Gbl.CurrentAct == ActFrmCreComCtr ||	// Brw_ADMI_SHARE_CTR
       Gbl.CurrentAct == ActFrmCreDocDeg ||	// Brw_ADMI_DOCUM_DEG
       Gbl.CurrentAct == ActFrmCreComDeg ||	// Brw_ADMI_SHARE_DEG
       Gbl.CurrentAct == ActFrmCreDocCrs ||	// Brw_ADMI_DOCUM_CRS
       Gbl.CurrentAct == ActFrmCreDocGrp ||	// Brw_ADMI_DOCUM_GRP
       Gbl.CurrentAct == ActFrmCreComCrs ||	// Brw_ADMI_SHARE_CRS
       Gbl.CurrentAct == ActFrmCreComGrp ||	// Brw_ADMI_SHARE_GRP
       Gbl.CurrentAct == ActFrmCreAsgUsr ||	// Brw_ADMI_ASSIG_USR
       Gbl.CurrentAct == ActFrmCreAsgCrs ||	// Brw_ADMI_ASSIG_CRS
       Gbl.CurrentAct == ActFrmCreWrkUsr ||	// Brw_ADMI_WORKS_USR
       Gbl.CurrentAct == ActFrmCreWrkCrs ||	// Brw_ADMI_WORKS_CRS
       Gbl.CurrentAct == ActFrmCreMrkCrs ||	// Brw_ADMI_MARKS_CRS
       Gbl.CurrentAct == ActFrmCreMrkGrp ||	// Brw_ADMI_MARKS_GRP
       Gbl.CurrentAct == ActFrmCreBrf)		// Brw_ADMI_BRIEF_USR
      fprintf (Gbl.F.Out,"<link rel=\"StyleSheet\""
	                 " href=\"%s/dropzone/css/dropzone.css\""
	                 " type=\"text/css\" />\n",
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
   if (Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW)
      fprintf (Gbl.F.Out,"<body class=\"%s\" onload=\"init()\">\n"
                         "<div id=\"zoomLyr\" class=\"ZOOM\">"
                         "<img id=\"zoomImg\" src=\"%s/_.gif\""
                         " alt=\"\" title=\"\""
                         " class=\"IMG_USR\" />"
                         "<div id=\"zoomTxt\" class=\"CENTER_MIDDLE\">"
                         "</div>"
                         "</div>",
               ClassBackground[Gbl.Prefs.Theme],
	       Gbl.Prefs.IconsURL);
   else
     {
      fprintf (Gbl.F.Out,"<body>\n");
      Gbl.Layout.WritingHTMLStart = false;
      Gbl.Layout.HTMLStartWritten =
      Gbl.Layout.TablEndWritten   = true;
      return;
     }

   if (Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW)
      fprintf (Gbl.F.Out,"<div id=\"zoomLyr\" class=\"ZOOM\">"
                         "<img id=\"zoomImg\" src=\"%s/_.gif\""
                         " alt=\"\" title=\"\""
                         " class=\"IMG_USR\" />"
                         "<div id=\"zoomTxt\" class=\"CENTER_MIDDLE\">"
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

   fprintf (Gbl.F.Out,"<td colspan=\"%u\" class=\"CENTER_TOP\">"
		      "<div id=\"CENTRAL_ZONE\" class=\"%s\">"
		      "<table class=\"CENTER_TOP\" style=\"width:100%%;\">"
		      "<tr>",
	    ColspanCentralPart,
	    The_TabOnBgColors[Gbl.Prefs.Theme]);

   /***** Central (main) part *****/
   switch (Gbl.Prefs.Layout)
     {
      case Lay_LAYOUT_DESKTOP:
         /* Left bar used to expand-contract central zone */
         fprintf (Gbl.F.Out,"<td class=\"RIGHT_TOP\" style=\"width:12px;\">");
         Pre_PutLeftIconToHideShowCols ();
         fprintf (Gbl.F.Out,"</td>");

         if (Gbl.Prefs.Menu == Mnu_MENU_VERTICAL)
           {
	    /* Tab content, including vertical menu (left) and main zone (right) */
	    fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\" style=\"width:175px;\">");
	    Mnu_WriteVerticalMenuThisTabDesktop ();
	    fprintf (Gbl.F.Out,"</td>");
           }
         break;
      case Lay_LAYOUT_MOBILE:
         /* Tab content */
         fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\">");
         Usr_WarningWhenDegreeTypeDoesntAllowDirectLogin ();
         if (Act_Actions[Act_Actions[Gbl.CurrentAct].SuperAction].IndexInMenu < 0)	// Write vertical menu
           {
            if (Gbl.CurrentAct == ActMnu)
               Mnu_WriteMenuThisTabMobile ();
            else
               Tab_DrawTabsMobile ();
           }
         break;
      default:
      	 break;
     }

   /***** Main zone *****/
   /* Start of main zone for actions output */
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\""
	              " style=\"padding:0 12px 12px 12px;\">");

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
         fprintf (Gbl.F.Out,"<div class=\"ASG_TITLE CENTER_MIDDLE\""
                            " style=\"margin:62px;\">");
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
	 fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\" style=\"width:12px;\">");
	 Pre_PutRigthIconToHideShowCols ();
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

   fprintf (Gbl.F.Out,"</title>\n");
  }

/*****************************************************************************/
/************* Write script and meta to redirect to my language **************/
/*****************************************************************************/

static void Lay_WriteRedirectionToMyLanguage (void)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];

   fprintf (Gbl.F.Out,"<meta http-equiv=\"refresh\""
	              " content=\"0; url='%s/%s?act=%ld&amp;ses=%s'\">",
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

   /***** Script for MathJax *****/
   // MathJax configuration
   /*
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
	              " window.MathJax = {"
	              "  tex2jax: {"
	              "   inlineMath: [ ['$','$'], [\"\\\\(\",\"\\\\)\"] ],"
	              "   processEscapes: true"
	              "  }"
	              " };"
	              "</script>");
   */
#ifdef Cfg_MATHJAX_LOCAL
   // Use the local copy of MathJax
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\""
	              " src=\"%s/MathJax/MathJax.js?config=TeX-AMS-MML_HTMLorMML\">"
	              "</script>\n",
	    Cfg_HTTPS_URL_SWAD_PUBLIC);
#else
   // Use the MathJax Content Delivery Network (CDN)
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\""
	              " src=\"//cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML\">"
	              "</script>\n");
#endif

   /***** Scripts used only in main window *****/
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
   if (Gbl.CurrentAct == ActFrmCreDocIns ||	// Brw_ADMI_DOCUM_INS
       Gbl.CurrentAct == ActFrmCreComIns ||	// Brw_ADMI_SHARE_INS
       Gbl.CurrentAct == ActFrmCreDocCtr ||	// Brw_ADMI_DOCUM_CTR
       Gbl.CurrentAct == ActFrmCreComCtr ||	// Brw_ADMI_SHARE_CTR
       Gbl.CurrentAct == ActFrmCreDocDeg ||	// Brw_ADMI_DOCUM_DEG
       Gbl.CurrentAct == ActFrmCreComDeg ||	// Brw_ADMI_SHARE_DEG
       Gbl.CurrentAct == ActFrmCreDocCrs ||	// Brw_ADMI_DOCUM_CRS
       Gbl.CurrentAct == ActFrmCreDocGrp ||	// Brw_ADMI_DOCUM_GRP
       Gbl.CurrentAct == ActFrmCreComCrs ||	// Brw_ADMI_SHARE_CRS
       Gbl.CurrentAct == ActFrmCreComGrp ||	// Brw_ADMI_SHARE_GRP
       Gbl.CurrentAct == ActFrmCreAsgUsr ||	// Brw_ADMI_ASSIG_USR
       Gbl.CurrentAct == ActFrmCreAsgCrs ||	// Brw_ADMI_ASSIG_CRS
       Gbl.CurrentAct == ActFrmCreWrkUsr ||	// Brw_ADMI_WORKS_USR
       Gbl.CurrentAct == ActFrmCreWrkCrs ||	// Brw_ADMI_WORKS_CRS
       Gbl.CurrentAct == ActFrmCreMrkCrs ||	// Brw_ADMI_MARKS_CRS
       Gbl.CurrentAct == ActFrmCreMrkGrp ||	// Brw_ADMI_MARKS_GRP
       Gbl.CurrentAct == ActFrmCreBrf)		// Brw_ADMI_BRIEF_USR
     {
      // Use charset="windows-1252" to force error messages in windows-1252 (default is UTF-8)
      fprintf (Gbl.F.Out,"<script type=\"text/javascript\""
	                 " src=\"%s/dropzone/dropzone.js\""
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
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];

   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">\n"
                      "function init(){\n");

   if (Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP)	// Clock visible
      fprintf (Gbl.F.Out,"	secondsSince1970UTC = %ld;\n"
                         "	writeLocalClock();",
               (long) Gbl.TimeStartExecution);

   if (Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP &&
       (Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN))	// Right column visible
      Con_WriteScriptClockConnected ();

   // Put the focus on login form
   fprintf (Gbl.F.Out,"	LoginForm = document.getElementById('UsrId');\n"
                      "	if (LoginForm)\n"
                      "		LoginForm.focus();\n"
                      "	ActionAJAX = \"%s\";\n"
                      "	setTimeout(\"refreshConnected()\",%lu);\n",
            Txt_STR_LANG_ID[Gbl.Prefs.Language],
            Gbl.Usrs.Connected.TimeToRefreshInMs);
   if (Gbl.CurrentAct == ActLstClk)
      fprintf (Gbl.F.Out,"	setTimeout(\"refreshLastClicks()\",%lu);\n",
               Cfg_TIME_TO_REFRESH_LAST_CLICKS);
   fprintf (Gbl.F.Out,"}\n"
                      "</script>\n");
  }

/*****************************************************************************/
/************** Write script to show connected users using AJAX **************/
/*****************************************************************************/

static void Lay_WriteScriptConnectedUsrs (void)
  {
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">\n"
                      "var RefreshParamNxtActCon = \"act=%ld\";\n"
                      "var RefreshParamNxtActLog = \"act=%ld\";\n"
                      "var RefreshParamIdSes = \"ses=%s\";\n"
                      "var RefreshParamCrsCod = \"crs=%ld\";\n"
                      "</script>\n",
            Act_Actions[ActRefCon].ActCod,
            Act_Actions[ActRefLstClk].ActCod,
            Gbl.Session.Id,
            Gbl.CurrentCrs.Crs.CrsCod);
  }

/*****************************************************************************/
/******* Write script to customize upload of files using Dropzone.js *********/
/*****************************************************************************/
// More info: http://www.dropzonejs.com/

static void Lay_WriteScriptCustomDropzone (void)
  {
   // "myAwesomeDropzone" is the camelized version of the HTML element's ID
   // Add a line "forceFallback: true,\n" to test classic upload
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">\n"
                      "Dropzone.options.myAwesomeDropzone = {\n"
	              "maxFiles: 100,\n"
		      "parallelUploads: 100,\n"
		      "maxFilesize: %lu,\n"
		      "fallback: function() {\n"
		      "document.getElementById('dropzone-upload').style.display='none';\n"
		      "document.getElementById('classic-upload').style.display='block';\n"
		      "}\n"
		      "};\n"
                      "</script>\n",
            (unsigned long) (Fil_MAX_FILE_SIZE / (1024ULL * 1024ULL) - 1));
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
         "swad112x32.gif",140,40,
        },
        {	// Lay_LAYOUT_MOBILE
         "swad168x48.gif",210,60,
        },
     };
   const char *ClassHeadRow1[The_NUM_THEMES] =
     {
      "WHITE_HEAD_ROW_1",	// The_THEME_WHITE
      "GREY_HEAD_ROW_1",	// The_THEME_GREY
      "BLUE_HEAD_ROW_1",	// The_THEME_BLUE
      "YELLOW_HEAD_ROW_1",	// The_THEME_YELLOW
      };

   fprintf (Gbl.F.Out,"<tr class=\"%s\">",ClassHeadRow1[Gbl.Prefs.Theme]);

   /***** 1st. row, 1st. column: logo *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\" style=\"width:%upx;\">",
            LogoLayout[Gbl.Prefs.Layout].Width + 20);

   /* Left logo */
   fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\">"
	              "<img src=\"%s/%s\""
	              " alt=\"%s\" title=\"%s\""
                      " class=\"CENTER_MIDDLE\""
	              " style=\"width:%upx; height:%upx;\" />"
                      "</a>",
            Cfg_HTTPS_URL_SWAD_CGI,Gbl.Prefs.PathTheme,
            LogoLayout[Gbl.Prefs.Layout].Icon,
            Cfg_PLATFORM_SHORT_NAME,Cfg_PLATFORM_FULL_NAME,
            LogoLayout[Gbl.Prefs.Layout].Width,
            LogoLayout[Gbl.Prefs.Layout].Height);
   fprintf (Gbl.F.Out,"</td>");
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<table style=\"width:100%%;\">"
                      "<tr>");
   /***** 1st. row, 2nd. column:
          search, and logged user / language selection *****/
   if (Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP)
     {
      /* Search courses / teachers */
      fprintf (Gbl.F.Out,"<td class=\"%s LEFT_MIDDLE\">",
               The_ClassHead[Gbl.Prefs.Theme]);
      Act_FormStart ( Gbl.CurrentCrs.Crs.CrsCod > 0 ? ActCrsSch :
	             (Gbl.CurrentDeg.Deg.DegCod > 0 ? ActDegSch :
	             (Gbl.CurrentCtr.Ctr.CtrCod > 0 ? ActCtrSch :
	             (Gbl.CurrentIns.Ins.InsCod > 0 ? ActInsSch :
	             (Gbl.CurrentCty.Cty.CtyCod > 0 ? ActCtySch :
	                                              ActSysSch)))));
      Sco_PutParamScope (Sco_SCOPE_SYS);
      Sch_PutFormToSearch (Gbl.Prefs.PathTheme);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");
     }

   /* Logged user or language selection */
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE\">",
            The_ClassHead[Gbl.Prefs.Theme]);
   if (Gbl.Usrs.Me.Logged)
      Usr_WriteLoggedUsrHead ();
   else
      Pre_PutSelectorToSelectLanguage ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>"
	              "</table>"
	              "</td>");

   /***** 1st. row, 3rd. column: link to open/close session *****/
   fprintf (Gbl.F.Out,"<td class=\"%s CENTER_MIDDLE\" style=\"width:160px;\">",
            The_ClassHead[Gbl.Prefs.Theme]);
   if (Gbl.Usrs.Me.Logged)
      Usr_PutFormLogOut ();
   else
      Usr_PutFormLogIn ();
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");

   /***** 2nd. row *****/
   fprintf (Gbl.F.Out,"<tr>");

   switch (Gbl.Prefs.Layout)
     {
      case Lay_LAYOUT_DESKTOP:
         /***** 2nd. row, 1st. column *****/
         /* Clock with hour:minute (server hour is shown) */
         fprintf (Gbl.F.Out,"<td class=\"CENTER_TOP\""
                            " style=\"width:160px; height:80px;\">");
         Dat_ShowClientLocalTime ();
         fprintf (Gbl.F.Out,"</td>");	// End of first column

         /***** 2nd. row, 2nd. column: degree and course *****/
         fprintf (Gbl.F.Out,"<td class=\"CENTER_TOP\" style=\"height:80px;\">"
                            "<div class=\"CENTER_TOP\""
                            " style=\"padding-top:5px;\">");
         Deg_WriteCtyInsCtrDeg ();
         Crs_WriteSelectorMyCourses ();
         Deg_WriteBigNameCtyInsCtrDegCrs ();
         fprintf (Gbl.F.Out,"</div>"
                            "</td>");

         /***** 2nd. row, 3rd. column *****/
         fprintf (Gbl.F.Out,"<td class=\"CENTER_TOP\""
                            " style=\"width:160px; height:80px;\">");
         if (Gbl.Usrs.Me.Logged)
           {
            /* Number of new messages (not seen) */
            fprintf (Gbl.F.Out,"<div id=\"msg\""	// Used for AJAX based refresh
        	               " style=\"padding-top:10px;\">");
            Ntf_WriteNumberOfNewNtfs ();
            fprintf (Gbl.F.Out,"</div>");		// Used for AJAX based refresh
           }
         break;
      case Lay_LAYOUT_MOBILE:
         fprintf (Gbl.F.Out,"<td colspan=\"3\" class=\"CENTER_MIDDLE\""
                            " style=\"height:40px;\">");
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
         if (Gbl.Prefs.SideCols & Lay_SHOW_LEFT_COLUMN)		// Left column visible
            fprintf (Gbl.F.Out,"<td class=\"CENTER_TOP\">"
        	               "</td>");

         /***** 3rd. row, 2nd. column *****/
         Tab_DrawTabs ();

         /***** 3rd. row, 3rd. column *****/
         if (Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN)	// Right column visible
           {
            fprintf (Gbl.F.Out,"<td rowspan=\"2\" class=\"CENTER_TOP\""
        	               " style=\"width:160px;\">");
            Lay_ShowRightColumn ();
            fprintf (Gbl.F.Out,"</td>");
           }

         fprintf (Gbl.F.Out,"</tr>"
                            "<tr>");

         /***** 4th. row, 1st. column *****/
         if (Gbl.Prefs.SideCols & Lay_SHOW_LEFT_COLUMN)		// Left column visible
           {
            fprintf (Gbl.F.Out,"<td class=\"CENTER_TOP\""
        	               " style=\"width:160px;\">");
            Lay_ShowLeftColumn ();
            fprintf (Gbl.F.Out,"</td>");
           }
         break;
      case Lay_LAYOUT_MOBILE:
         Tab_DrawTabs ();
         fprintf (Gbl.F.Out,"</tr>"
                            "<tr>");
         break;
      default:
      	 break;
     }
  }

/*****************************************************************************/
/*********** Write icon and title associated to the current action ***********/
/*****************************************************************************/

static void Lay_WriteTitleAction (void)
  {
   extern const char *The_ClassTitleAction[The_NUM_THEMES];
   extern const char *The_ClassSubtitleAction[The_NUM_THEMES];
   extern const char *Txt_TABS_FULL_TXT[Tab_NUM_TABS];

   /***** Container start *****/
   fprintf (Gbl.F.Out,"<div id=\"action_title\""
	              " style=\"background-image:url('%s/%s/%s64x64.gif');"
	              " background-size:80px 80px;\">",
	    Gbl.Prefs.PathIconSet,Cfg_ICON_ACTION,
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

   fprintf (Gbl.F.Out,"<table style=\"width:160px;\">"
                      "<tr>"
                      "<td class=\"LEFT_TOP\">"
                      "<table style=\"width:160px; border-spacing:5px;\">");

   /***** Most frequently used actions *****/
   if (Gbl.Usrs.Me.Logged)
     {
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"MFU_ACT CENTER_TOP\">");
      Act_AllocateMFUActions (&ListMFUActions,6);
      Act_GetMFUActions (&ListMFUActions,6);
      Act_WriteSmallMFUActions (&ListMFUActions);
      Act_FreeMFUActions (&ListMFUActions);
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");
     }

   /***** Month *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"CENTER_TOP\">");
   Cal_DrawCurrentMonth ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Notices (yellow notes) *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
     {
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"CENTER_TOP\">");
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
                      "<td class=\"RIGHT_TOP\">"
                      "<table style=\"width:100%%; padding-top:70px;"
                      " border-spacing:5px;\">");

   /***** Banners *****/
   Ban_WriteMenuWithBanners ();

   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"CENTER_MIDDLE\">");
   Act_FormStart (ActLstCon);
   Act_LinkFormSubmit (Txt_Connected_users,The_ClassConnected[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s</a>",Txt_Connected_PLURAL);
   Act_FormEnd ();

   /***** Number of connected users in the whole platform *****/
   fprintf (Gbl.F.Out,"<div id=\"globalconnected\">");	// Used for AJAX based refresh
   Con_ShowGlobalConnectedUsrs ();
   fprintf (Gbl.F.Out,"</div>");			// Used for AJAX based refresh

   /***** Number of connected users in the current course *****/
   fprintf (Gbl.F.Out,"<div id=\"courseconnected\">");	// Used for AJAX based refresh
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// There is a course selected
     {
      Gbl.Scope.Current = Sco_SCOPE_CRS;
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
			 "<td class=\"CENTER_MIDDLE\">"
			 "<a href=\"https://play.google.com/store/apps/details?id=es.ugr.swad.swadroid\""
			 " target=\"_blank\" title=\"%s\">"
			 "<img src=\"%s/SWADroid120x200.png\""
			 " alt=\"SWADroid\" title=\"SWADroid\""
			 " style=\"width:150px; height:250px;\" />"
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
/************************* Put a link (form) to view *************************/
/*****************************************************************************/

void Lay_PutFormToView (Act_Action_t Action)
  {
   extern const char *Txt_View;

   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Act_PutContextualLink (Action,NULL,"visible_on",Txt_View);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************************* Put a link (form) to edit *************************/
/*****************************************************************************/

void Lay_PutFormToEdit (Act_Action_t Action)
  {
   extern const char *Txt_Edit;

   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Act_PutContextualLink (Action,NULL,"edit",Txt_Edit);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/**************** Put a icon with a text to submit a form ********************/
/*****************************************************************************/

void Lay_PutIconWithText (const char *Icon,const char *Alt,const char *Text)
  {
   // margin is used because this form link may be placed after another one
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_OPT ICON_HIGHLIGHT\">"
	              "<img src=\"%s/%s16x16.gif\""
	              " alt=\"%s\" title=\"%s\""
	              " class=\"ICON16x16\" />",
            Gbl.Prefs.IconsURL,Icon,
            Alt,Text ? Text : Alt);
   if (Text)
      if (Text[0])
	 fprintf (Gbl.F.Out," %s",
		  Text);
   fprintf (Gbl.F.Out,"</div>"
	              "</a>");
  }

/*****************************************************************************/
/********** Put a icon with a text to submit a form.                **********/
/********** When clicked, the icon will be replaced by an animation **********/
/*****************************************************************************/

void Lay_PutCalculateIconWithText (const char *Alt,const char *Text)
  {
   fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\""
	              " style=\"margin:0 6px; display:inline;\">"
	              "<img id=\"update_%d\" src=\"%s/recycle16x16.gif\""
	              " alt=\"%s\" title=\"%s\""
		      " class=\"ICON16x16\" />"
		      "<img id=\"updating_%d\" src=\"%s/working16x16.gif\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON16x16\" style=\"display:none;\" />"	// Animated icon hidden
		      "&nbsp;%s"
		      "</div>"
		      "</a>",
	    Gbl.NumForm,Gbl.Prefs.IconsURL,Alt,Text,
	    Gbl.NumForm,Gbl.Prefs.IconsURL,Alt,Text,
	    Text);
  }

/*****************************************************************************/
/******** Put a disabled icon indicating that removal is not allowed *********/
/*****************************************************************************/

void Lay_PutIconRemovalNotAllowed (void)
  {
   extern const char *Txt_Removal_not_allowed;

   fprintf (Gbl.F.Out,"<img src=\"%s/deloff16x16.gif\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON16x16\" />",
	    Gbl.Prefs.IconsURL,
	    Txt_Removal_not_allowed,
	    Txt_Removal_not_allowed);
  }

void Lay_PutIconBRemovalNotAllowed (void)
  {
   extern const char *Txt_Removal_not_allowed;

   fprintf (Gbl.F.Out,"<img src=\"%s/deloff16x16.gif\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON16x16B\" />",
	    Gbl.Prefs.IconsURL,
	    Txt_Removal_not_allowed,
	    Txt_Removal_not_allowed);
  }

/*****************************************************************************/
/******** Put a disabled icon indicating that removal is not allowed *********/
/*****************************************************************************/

void Lay_PutIconRemove (void)
  {
   extern const char *Txt_Remove;

   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/delon16x16.gif\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON16x16\" />",
	    Gbl.Prefs.IconsURL,
	    Txt_Remove,
	    Txt_Remove);
  }

void Lay_PutIconBRemove (void)
  {
   extern const char *Txt_Remove;

   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/delon16x16.gif\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON16x16B\" />",
	    Gbl.Prefs.IconsURL,
	    Txt_Remove,
	    Txt_Remove);
  }

/*****************************************************************************/
/********************** Put a button to submit a form ************************/
/*****************************************************************************/

void Lay_PutCreateButton (const char *Text)
  {
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">"
                      "<button type=\"submit\" class=\"BT_SUBMIT BT_CREATE\">"
                      "%s"
                      "</button>"
                      "</div>",
            Text);
  }

void Lay_PutCreateButtonInline (const char *Text)
  {
   fprintf (Gbl.F.Out,"<button type=\"submit\" class=\"BT_SUBMIT_INLINE BT_CREATE\">"
                      "%s"
                      "</button>",
            Text);
  }

void Lay_PutConfirmButton (const char *Text)
  {
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">"
                      "<button type=\"submit\" class=\"BT_SUBMIT BT_CONFIRM\">"
                      "%s"
                      "</button>"
                      "</div>",
            Text);
  }

void Lay_PutConfirmButtonInline (const char *Text)
  {
   fprintf (Gbl.F.Out,"<button type=\"submit\" class=\"BT_SUBMIT_INLINE BT_CONFIRM\">"
                      "%s"
                      "</button>",
            Text);
  }

void Lay_PutRemoveButton (const char *Text)
  {
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">"
                      "<button type=\"submit\" class=\"BT_SUBMIT BT_REMOVE\">"
                      "%s"
                      "</button>"
                      "</div>",
            Text);
  }

void Lay_PutRemoveButtonInline (const char *Text)
  {
   fprintf (Gbl.F.Out,"<button type=\"submit\" class=\"BT_SUBMIT_INLINE BT_REMOVE\">"
                      "%s"
                      "</button>",
            Text);
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

void Lay_StartRoundFrameTable (const char *Width,unsigned CellPadding,const char *Title)
  {
   Lay_StartRoundFrame (Width,Title);

   fprintf (Gbl.F.Out,"<table class=\"TABLE10");
   if (CellPadding)
      fprintf (Gbl.F.Out," CELLS_PAD_%u",CellPadding);	// CellPadding must be 0, 1, 2, 4 or 8
   fprintf (Gbl.F.Out,"\">");
  }

void Lay_StartRoundFrame (const char *Width,const char *Title)
  {
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\""
	              " style=\"width:100%%;\">"
	              "<div class=\"FRAME10\"");
   if (Width)
       fprintf (Gbl.F.Out," style=\"width:%s;\"",Width);
   fprintf (Gbl.F.Out,">");

   if (Title)
      fprintf (Gbl.F.Out,"<div class=\"TIT_TBL_10 CENTER_MIDDLE\">"
	                 "%s"
	                 "</div>",
	       Title);
  }

// CellPadding must be 0, 1, 2, 4 or 8

void Lay_StartRoundFrameTableShadow (const char *Width,unsigned CellPadding)
  {
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\""
	              " style=\"width:100%%;\">"
	              "<div class=\"FRAME10_SHADOW\"");
   if (Width)
       fprintf (Gbl.F.Out," style=\"width:%s;\"",Width);
   fprintf (Gbl.F.Out,">"
                      "<table class=\"TABLE10");
   if (CellPadding)
      fprintf (Gbl.F.Out," CELLS_PAD_%u",CellPadding);	// CellPadding must be 0, 1, 2, 4 or 8
   fprintf (Gbl.F.Out,"\">");
  }

void Lay_EndRoundFrameTable (void)
  {
   fprintf (Gbl.F.Out,"</table>");

   Lay_EndRoundFrame ();
  }

void Lay_EndRoundFrame (void)
  {
   fprintf (Gbl.F.Out,"</div>"
		      "</div>");
  }

void Lay_EndRoundFrameTableWithButton (Lay_Button_t Button,const char *TxtButton)
  {
   fprintf (Gbl.F.Out,"</table>");

   Lay_EndRoundFrameWithButton (Button,TxtButton);
  }

void Lay_EndRoundFrameWithButton (Lay_Button_t Button,const char *TxtButton)
  {
   /***** Button *****/
   if (TxtButton)
      if (TxtButton[0])
	 switch (Button)
           {
	    case Lay_NO_BUTTON:
	       break;
	    case Lay_CREATE_BUTTON:
	       Lay_PutCreateButton (TxtButton);
	       break;
	    case Lay_CONFIRM_BUTTON:
	       Lay_PutConfirmButton (TxtButton);
	       break;
	    case Lay_REMOVE_BUTTON:
 	       Lay_PutRemoveButton (TxtButton);
	       break;
          }

   fprintf (Gbl.F.Out,"</div>"
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
      fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">"
	                 "<div class=\"ALERT\""
	                 " style=\"background-image:url('%s/%s16x16.gif');"
	                 " background-size:20px 20px;\">"
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
      Pre_RemoveOldPrefsFromIP ();		// Remove old preferences from IP
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
      Gbl.Scope.Current = Sco_SCOPE_CRS;
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

         fprintf (Gbl.F.Out,"<div class=\"FOOT CENTER_MIDDLE\""
                            " style=\"padding-bottom:12px;\">");

         /***** Institution and centre hosting the platform *****/
         fprintf (Gbl.F.Out,"<a href=\"%s\" class=\"FOOT\" target=\"_blank\">"
                            "<img src=\"%s/%s\""
                            " alt=\"%s\" title=\"%s\""
                            " style=\"width:%upx; height:%upx;\" />"
                            "<div>%s</div>"
                            "</a>",
                  Cfg_ABOUT_URL,
                  Gbl.Prefs.IconsURL,Cfg_ABOUT_LOGO,
                  Cfg_ABOUT_NAME,Cfg_ABOUT_NAME,
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
                      "<td colspan=\"%u\" class=\"CENTER_MIDDLE\">"
                      "<table style=\"width:100%%; padding:12px;\">"
                      "<tr>",
            NumColumns);

   /***** First column: institution logo *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\" style=\"width:80px;\">");
   if (InsCod > 0)
     {
      if (!PrintView)
         fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\">",Ins.WWW);
      Log_DrawLogo (Sco_SCOPE_INS,Ins.InsCod,Ins.ShortName,
                    Size,NULL,true);
      if (!PrintView)
        fprintf (Gbl.F.Out,"</a>");
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** Second column: class photo title *****/
   fprintf (Gbl.F.Out,"<td class=\"TIT_CLASSPHOTO CENTER_MIDDLE\">");
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
   fprintf (Gbl.F.Out,"<td class=\"RIGHT_TOP\" style=\"width:80px;\">");
   if (DegCod > 0)
     {
      if (!PrintView)
         fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\""
                            " class=\"TIT_CLASSPHOTO\">",
                  Deg.WWW);
      Log_DrawLogo (Sco_SCOPE_DEG,Deg.DegCod,Deg.ShortName,Size,NULL,true);
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

   Lay_StartRoundFrameTable (NULL,2,Txt_Layout);
   fprintf (Gbl.F.Out,"<tr>");
   for (Layout = (Lay_Layout_t) 0;
	Layout < Lay_NUM_LAYOUTS;
	Layout++)
     {
      fprintf (Gbl.F.Out,"<td class=\"%s CENTER_MIDDLE\">",
               Layout == Gbl.Prefs.Layout ? "LAYOUT_ON" :
        	                            "LAYOUT_OFF");
      Act_FormStart (ActChgLay);
      Par_PutHiddenParamUnsigned ("Layout",(unsigned) Layout);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s32x32.gif\""
	                 " alt=\"%s\" title=\"%s\" class=\"ICON32x32B\""
	                 " style=\"margin:0 auto;\" />",
               Gbl.Prefs.IconsURL,
               Lay_LayoutIcons[Layout],
               Txt_LAYOUT_NAMES[Layout],
               Txt_LAYOUT_NAMES[Layout]);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");
     }
   fprintf (Gbl.F.Out,"</tr>");
   Lay_EndRoundFrameTableWithButton (Lay_NO_BUTTON,NULL);
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
   Pre_SetPrefsFromIP ();
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
/**************** Show advertisement about mobile version ********************/
/*****************************************************************************/

#define Lay_SHOW_ADVERTISEMENT_ONE_TIME_OF_X	5	// n ==> show advertisement 1/n of the times

void Lay_AdvertisementMobile (void)
  {
   extern const char *Txt_Stay_connected_with_SWADroid;

   if (!(rand () % Lay_SHOW_ADVERTISEMENT_ONE_TIME_OF_X))
     {
      fprintf (Gbl.F.Out,"<div style=\"margin-top:25px;\">");

      /***** Table start *****/
      Lay_StartRoundFrameTable (NULL,8,NULL);

      /***** Show advertisement *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"DAT CENTER_MIDDLE\">"
	                 "<a href=\"https://play.google.com/store/apps/details?id=es.ugr.swad.swadroid\""
	                 " class=\"DAT\">"
                         "%s<br /><br />"
                         "<img src=\"%s/SWADroid200x300.png\""
                         " alt=\"SWADroid\" title=\"%s\""
                         " style=\"width:250px; height:375px;\" />"
                         "</a>"
	                 "</td>"
	                 "</tr>",
               Txt_Stay_connected_with_SWADroid,
               Gbl.Prefs.IconsURL,
               Txt_Stay_connected_with_SWADroid);

      /***** End table *****/
      Lay_EndRoundFrameTable ();

      fprintf (Gbl.F.Out,"</div>");
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
      fprintf (Gbl.F.Out,"<img src=\"%s/%s20x20.gif\""
	                 " alt=\"\" title=\"\""
                         " class=\"ICON20x20\" />",
		  Gbl.Prefs.IconsURL,
		  IsLastItemInLevel[i] ? "tr" :
		                         "subleft");

   /***** Level *****/
   fprintf (Gbl.F.Out,"<img src=\"%s/%s20x20.gif\""
	              " alt=\"\" title=\"\""
                      " class=\"ICON20x20\" />",
	    Gbl.Prefs.IconsURL,
	    IsLastItemInLevel[Level] ? "subend" :
				       "submid");
  }
/*****************************************************************************/
/************************** Help for the text editor *************************/
/*****************************************************************************/

void Lay_HelpPlainEditor (void)
  {
   extern const char *Txt_TEXT_plain;

   Lay_HelpTextEditor (Txt_TEXT_plain,
                       "\\(LaTeX\\)",
                       "$$LaTeX$$, \\[LaTeX\\]");
  }

void Lay_HelpRichEditor (void)
  {
   Lay_HelpTextEditor ("<a href=\"http://johnmacfarlane.net/pandoc/README.html#pandocs-markdown\""
	               " target=\"_blank\">Markdown + Pandoc</a>",
                       "$LaTeX$",
                       "$$LaTeX$$");
  }

static void Lay_HelpTextEditor (const char *Text,const char *InlineMath,const char *Equation)
  {
   extern const char *Txt_Text;
   extern const char *Txt_Inline_math;
   extern const char *Txt_Equation_centered;

   fprintf (Gbl.F.Out,"<div class=\"HELP_EDIT\">"
	              "<code>"
	              "%s: %s"
                      "&nbsp;&nbsp;&nbsp;"
                      "%s: %s"
                      "&nbsp;&nbsp;&nbsp;"
                      "%s: %s"
                      "</code>"
                      "</div>",
            Txt_Text,Text,
            Txt_Inline_math,InlineMath,
            Txt_Equation_centered,Equation);
  }
