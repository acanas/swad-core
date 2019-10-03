// swad_layout.c: page layout

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For exit, system, malloc, calloc, free, etc
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_API.h"
#include "swad_box.h"
#include "swad_calendar.h"
#include "swad_changelog.h"
#include "swad_config.h"
#include "swad_connected.h"
#include "swad_database.h"
#include "swad_exam.h"
#include "swad_firewall.h"
#include "swad_follow.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy.h"
#include "swad_language.h"
#include "swad_logo.h"
#include "swad_MFU.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_setting.h"
#include "swad_tab.h"
#include "swad_table.h"
#include "swad_theme.h"
#include "swad_timeline.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Lay_WriteEndOfPage (void);

static void Lay_WritePageTitle (void);

static void Lay_WriteRedirToMyLangOnLogIn (void);
static void Lay_WriteRedirToMyLangOnViewUsrAgd (void);

static void Lay_WriteScripts (void);
static void Lay_WriteScriptMathJax (void);
static void Lay_WriteScriptInit (void);
static void Lay_WriteScriptParamsAJAX (void);
static void Lay_WriteScriptCustomDropzone (void);

static void Lay_WritePageTopHeading (void);
static void Lay_WriteBreadcrumb (void);

static void Lay_WriteTitleAction (void);

static void Lay_ShowLeftColumn (void);
static void Lay_ShowRightColumn (void);

static void Lay_WriteAboutZone (void);
static void Lay_WriteFootFromHTMLFile (void);

static void Lay_HelpTextEditor (const char *Text,const char *InlineMath,const char *Equation);

/*****************************************************************************/
/*********************** Write the start of the page *************************/
/*****************************************************************************/

void Lay_WriteStartOfPage (void)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const unsigned Txt_Current_CGI_SWAD_Language;
   extern const char *The_TabOnBgColors[The_NUM_THEMES];
   const char *LayoutMainZone[Mnu_NUM_MENUS] =
     {
      "main_horizontal",	// Mnu_MENU_HORIZONTAL
      "main_vertical",		// Mnu_MENU_VERTICAL
     };
   Act_BrowserTab_t BrowserTab;

   /***** If, when this function is called, the head is being written
          or the head is already written ==> don't do anything *****/
   if (Gbl.Layout.WritingHTMLStart ||
       Gbl.Layout.HTMLStartWritten)
      return;

   /***** Compute connected users to be displayed in right columns *****/
   Con_ComputeConnectedUsrsBelongingToCurrentCrs ();

   /***** Send head width the file type for the HTTP protocol *****/
   if (Gbl.Action.UsesAJAX)
     // Don't generate a full HTML page, only the content of a DIV or similar
     {
      fprintf (Gbl.F.Out,"Content-Type: text/html; charset=windows-1252\r\n\r\n");
      Gbl.Layout.WritingHTMLStart = false;
      Gbl.Layout.HTMLStartWritten = Gbl.Layout.DivsEndWritten = true;
      return;
     }

   /***** If serving a web service ==> don't do anything *****/
   if (Gbl.WebService.IsWebService)
     {
      Gbl.Layout.WritingHTMLStart = false;
      Gbl.Layout.HTMLStartWritten = Gbl.Layout.DivsEndWritten = true;
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
// "<meta http-equiv=\"Page-Enter\" content=\"blendTrans(Duration=0)\">\n"
// "<meta http-equiv=\"Page-Exit\" content=\"blendTrans(Duration=0)\">\n"
                      "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=windows-1252\" />\n"
                      "<meta name=\"description\" content=\"A free-software, educational, online tool for managing courses and students.\" />\n"
                      "<meta name=\"keywords\" content=\""
                      "%s,"
                      "SWAD,"
                      "shared workspace at a distance,"
                      "educational platform,"
                      "sistema web de apoyo a la docencia,"
                      "plataforma educativa,"
                      "campus virtual,"
                      "SWADroid,"
                      "LMS,"
                      "Learning Management System\" />\n",
            Lan_STR_LANG_ID[Gbl.Prefs.Language],
            Cfg_PLATFORM_SHORT_NAME);

   /* Viewport (used for responsive design) */
   fprintf (Gbl.F.Out,"<meta name=\"viewport\""
	              " content=\"width=device-width, initial-scale=1.0\">\n");

   /* Title */
   Lay_WritePageTitle ();

   /* Canonical URL */
   fprintf (Gbl.F.Out,"<link rel=\"canonical\""
	              " href=\"%s\" />\n",
	    Cfg_URL_SWAD_CGI);

   /* Favicon */
   fprintf (Gbl.F.Out,"<link type=\"image/x-icon\" href=\"%s/favicon.ico\" rel=\"icon\" />\n"
                      "<link type=\"image/x-icon\" href=\"%s/favicon.ico\" rel=\"shortcut icon\" />\n",
	    Cfg_URL_ICON_PUBLIC,
	    Cfg_URL_ICON_PUBLIC);

   /* Style sheet for SWAD */
   fprintf (Gbl.F.Out,"<link rel=\"stylesheet\""
	              " href=\"%s/%s\""
	              " type=\"text/css\" />\n",
            Cfg_URL_SWAD_PUBLIC,CSS_FILE);

   /* Style sheets for Font Awesome */
   fprintf (Gbl.F.Out,"<link rel=\"stylesheet\""
	              " href=\"%s/fontawesome/css/fontawesome.css\""
	              " type=\"text/css\" />\n",
            Cfg_URL_SWAD_PUBLIC);
   fprintf (Gbl.F.Out,"<link rel=\"stylesheet\""
	              " href=\"%s/fontawesome/css/solid.css\""
	              " type=\"text/css\" />\n",
            Cfg_URL_SWAD_PUBLIC);

   /* Style sheet for Dropzone.js (http://www.dropzonejs.com/) */
   // The public directory dropzone must hold:
   // dropzone.js
   // css/dropzone.css
   // images/spritemap@2x.png
   // images/spritemap.png
   switch (Gbl.Action.Act)
     {
      case ActFrmCreDocIns:	// Brw_ADMI_DOC_INS
      case ActFrmCreShaIns:	// Brw_ADMI_SHR_INS
      case ActFrmCreDocCtr:	// Brw_ADMI_DOC_CTR
      case ActFrmCreShaCtr:	// Brw_ADMI_SHR_CTR
      case ActFrmCreDocDeg:	// Brw_ADMI_DOC_DEG
      case ActFrmCreShaDeg:	// Brw_ADMI_SHR_DEG
      case ActFrmCreDocCrs:	// Brw_ADMI_DOC_CRS
      case ActFrmCreDocGrp:	// Brw_ADMI_DOC_GRP
      case ActFrmCreTchCrs:	// Brw_ADMI_TCH_CRS
      case ActFrmCreTchGrp:	// Brw_ADMI_TCH_GRP
      case ActFrmCreShaCrs:	// Brw_ADMI_SHR_CRS
      case ActFrmCreShaGrp:	// Brw_ADMI_SHR_GRP
      case ActFrmCreAsgUsr:	// Brw_ADMI_ASG_USR
      case ActFrmCreAsgCrs:	// Brw_ADMI_ASG_CRS
      case ActFrmCreWrkUsr:	// Brw_ADMI_WRK_USR
      case ActFrmCreWrkCrs:	// Brw_ADMI_WRK_CRS
      case ActFrmCreDocPrj:	// Brw_ADMI_DOC_PRJ
      case ActFrmCreAssPrj:	// Brw_ADMI_ASS_PRJ
      case ActFrmCreMrkCrs:	// Brw_ADMI_MRK_CRS
      case ActFrmCreMrkGrp:	// Brw_ADMI_MRK_GRP
      case ActFrmCreBrf:	// Brw_ADMI_BRF_USR
	 fprintf (Gbl.F.Out,"<link rel=\"stylesheet\""
			    " href=\"%s/dropzone/css/dropzone.css\""
			    " type=\"text/css\" />\n",
		  Cfg_URL_SWAD_PUBLIC);
	 break;
      default:
	 break;
     }

   /* Redirect to correct language */
   if (Gbl.Usrs.Me.Logged &&							// I am logged
       Gbl.Usrs.Me.UsrDat.Prefs.Language != Txt_Current_CGI_SWAD_Language)	// My language != current language
     {
      if (Gbl.Action.Original == ActLogIn ||	// Regular log in
	  Gbl.Action.Original == ActLogInNew)	// Log in when checking account
         Lay_WriteRedirToMyLangOnLogIn ();
      else if (Gbl.Action.Original == ActLogInUsrAgd)	// Log in to view another user's public agenda
         Lay_WriteRedirToMyLangOnViewUsrAgd ();
     }

   /* Write initial scripts depending on the action */
   Lay_WriteScripts ();

   fprintf (Gbl.F.Out,"</head>\n");

   /***** HTML body *****/
   BrowserTab = Act_GetBrowserTab (Gbl.Action.Act);
   switch (BrowserTab)
     {
      case Act_BRW_1ST_TAB:
	 fprintf (Gbl.F.Out,"<body onload=\"init();\">\n"
			    "<div id=\"zoomLyr\" class=\"ZOOM\">"
			    "<img id=\"zoomImg\" src=\"%s/usr_bl.jpg\""
			    " alt=\"\" title=\"\""
			    " class=\"IMG_USR\" />"
			    "<div id=\"zoomTxt\" class=\"CENTER_MIDDLE\">"
			    "</div>"
			    "</div>",
		  Cfg_URL_ICON_PUBLIC);
	 break;
      case Act_BRW_NEW_TAB:
      case Act_BRW_2ND_TAB:
	 fprintf (Gbl.F.Out,"<body onload=\"init();\">\n");
	 Gbl.Layout.WritingHTMLStart = false;
	 Gbl.Layout.HTMLStartWritten =
	 Gbl.Layout.DivsEndWritten   = true;
	 return;
      default:
	 fprintf (Gbl.F.Out,"<body>\n");
	 Gbl.Layout.WritingHTMLStart = false;
	 Gbl.Layout.HTMLStartWritten =
	 Gbl.Layout.DivsEndWritten   = true;
	 return;
     }

   /***** Start box that contains the whole page except the foot *****/
   fprintf (Gbl.F.Out,"<div id=\"whole_page\">");

   /***** Header of layout *****/
   Lay_WritePageTopHeading ();

   /***** 3rd. row (tabs) *****/
   Tab_DrawTabs ();

   /***** 4th row: main zone *****/
   fprintf (Gbl.F.Out,"<div id=\"main_zone\">");

   /* Left column */
   if (Gbl.Prefs.SideCols & Lay_SHOW_LEFT_COLUMN)		// Left column visible
     {
      fprintf (Gbl.F.Out,"<aside id=\"left_col\">");
      Lay_ShowLeftColumn ();
      fprintf (Gbl.F.Out,"</aside>");
     }

   /* Right column */
   // Right column is written before central column
   // but it must be drawn at right using "position:absolute; right:0".
   // The reason to write right column before central column
   // is that central column may hold a lot of content drawn slowly.
   if (Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN)	// Right column visible
     {
      fprintf (Gbl.F.Out,"<aside id=\"right_col\">");
      Lay_ShowRightColumn ();
      fprintf (Gbl.F.Out,"</aside>");
     }

   /* Central (main) column */
   switch (Gbl.Prefs.SideCols)
     {
      case 0:
         fprintf (Gbl.F.Out,"<div id=\"main_zone_central_none\">");
	 break;
      case Lay_SHOW_LEFT_COLUMN:
         fprintf (Gbl.F.Out,"<div id=\"main_zone_central_left\">");
	 break;
      case Lay_SHOW_RIGHT_COLUMN:
         fprintf (Gbl.F.Out,"<div id=\"main_zone_central_right\">");
	 break;
      case (Lay_SHOW_LEFT_COLUMN | Lay_SHOW_RIGHT_COLUMN):
         fprintf (Gbl.F.Out,"<div id=\"main_zone_central_both\">");
         break;
     }
   fprintf (Gbl.F.Out,"<div id=\"main_zone_central_container\" class=\"%s\">",
	    The_TabOnBgColors[Gbl.Prefs.Theme]);

   /* Layout with horizontal or vertical menu */
   fprintf (Gbl.F.Out,"<div id=\"%s\">",LayoutMainZone[Gbl.Prefs.Menu]);

   /* Menu */
   Mnu_WriteMenuThisTab ();

   /* Start canvas: main zone for actions output */
   fprintf (Gbl.F.Out,"<div class=\"MAIN_ZONE_CANVAS\">");

   /* If it is mandatory to read any information about course */
   if (Gbl.Crs.Info.ShowMsgMustBeRead)
      Inf_WriteMsgYouMustReadInfo ();

   /* Write title of the current action */
   if (Gbl.Prefs.Menu == Mnu_MENU_VERTICAL &&
      Act_GetIndexInMenu (Gbl.Action.Act) >= 0)
      Lay_WriteTitleAction ();

   Gbl.Layout.WritingHTMLStart = false;
   Gbl.Layout.HTMLStartWritten = true;

   /* Write message indicating number of clicks allowed before sending my photo */
   Usr_InformAboutNumClicksBeforePhoto ();
  }

/*****************************************************************************/
/*********************** Write status 204 No Content *************************/
/*****************************************************************************/

void Lay_WriteHTTPStatus204NoContent (void)
  {
   /***** The HTTP response is a code status *****/
   /* Don't write HTML at all */
   Gbl.Layout.HTMLStartWritten =
   Gbl.Layout.DivsEndWritten   =
   Gbl.Layout.HTMLEndWritten   = true;

   /* Start HTTP response */
   fprintf (stdout,"Content-type: text/plain; charset=windows-1252\n");

   /* Return HTTP status code 204 No Content:
      The server has successfully fulfilled the request
      and there is no additional content to send
      in the response payload body. */
   fprintf (stdout,"Status: 204\r\n\r\n");
  }

/*****************************************************************************/
/************************ Write the end of the page **************************/
/*****************************************************************************/

static void Lay_WriteEndOfPage (void)
  {
   if (!Gbl.Layout.DivsEndWritten)
     {
      /***** End of central part of main zone *****/
      fprintf (Gbl.F.Out,"</div>"	// Canvas (main zone to output content of the current action)
                         "</div>"	// Layout with horizontal or vertical menu
			 "</div>");	// main_zone_central_container

      /***** Write page footer *****/
      if (Act_GetBrowserTab (Gbl.Action.Act) == Act_BRW_1ST_TAB)
         Lay_WriteFootFromHTMLFile ();

      /***** End of main zone and page *****/
      fprintf (Gbl.F.Out,"</div>"	// main_zone_central
			 "</div>"	// main_zone
                         "</div>\n");	// whole_page_* (box that contains the whole page except the foot)

      Gbl.Layout.DivsEndWritten = true;
     }
  }

/*****************************************************************************/
/************************* Write the title of the page ***********************/
/*****************************************************************************/

static void Lay_WritePageTitle (void)
  {
   extern const char *Txt_TAGLINE;

   fprintf (Gbl.F.Out,"<title>");

   if (Gbl.Params.GetMethod && Gbl.Hierarchy.Deg.DegCod > 0)
     {
      fprintf (Gbl.F.Out,"%s &gt; %s",
	       Cfg_PLATFORM_SHORT_NAME,
	       Gbl.Hierarchy.Deg.ShrtName);
      if (Gbl.Hierarchy.Level == Hie_CRS)
         fprintf (Gbl.F.Out," &gt; %s",
                  Gbl.Hierarchy.Crs.ShrtName);
     }
   else
      fprintf (Gbl.F.Out,"%s: %s",
	       Cfg_PLATFORM_SHORT_NAME,Txt_TAGLINE);

   fprintf (Gbl.F.Out,"</title>\n");
  }

/*****************************************************************************/
/************* Write script and meta to redirect to my language **************/
/*****************************************************************************/

static void Lay_WriteRedirToMyLangOnLogIn (void)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   fprintf (Gbl.F.Out,"<meta http-equiv=\"refresh\""
	              " content=\"0; url='%s/%s?act=%ld&amp;ses=%s'\">",
	    Cfg_URL_SWAD_CGI,
	    Lan_STR_LANG_ID[Gbl.Usrs.Me.UsrDat.Prefs.Language],
	    Act_GetActCod (ActLogInLan),
	    Gbl.Session.Id);
  }

static void Lay_WriteRedirToMyLangOnViewUsrAgd (void)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   fprintf (Gbl.F.Out,"<meta http-equiv=\"refresh\""
	              " content=\"0; url='%s/%s?act=%ld&amp;ses=%s&amp;agd=@%s'\">",
	    Cfg_URL_SWAD_CGI,
	    Lan_STR_LANG_ID[Gbl.Usrs.Me.UsrDat.Prefs.Language],
	    Act_GetActCod (ActLogInUsrAgdLan),
	    Gbl.Session.Id,
	    Gbl.Usrs.Other.UsrDat.Nickname);
  }

/*****************************************************************************/
/************ Write some scripts depending on the current action *************/
/*****************************************************************************/

static void Lay_WriteScripts (void)
  {
   extern const char *Txt_DAYS_CAPS[7];
   extern const char *Txt_DAYS_SMALL[7];
   extern const char *Txt_Exam_of_X;
   unsigned DayOfWeek; /* 0, 1, 2, 3, 4, 5, 6 */
   unsigned NumHld;
   unsigned NumExamAnnouncement;	// Number of exam announcement

   /***** General scripts for swad *****/
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\" src=\"%s/%s\">"
                      "</script>\n",
	    Cfg_URL_SWAD_PUBLIC,JS_FILE);

   /***** Script for MathJax *****/
   Lay_WriteScriptMathJax ();

   /***** Write script with init function executed after loading page *****/
   Lay_WriteScriptInit ();

   /***** Write script to set parameters needed by AJAX *****/
   Lay_WriteScriptParamsAJAX ();

   /***** Prepare script to draw months *****/
   if ((Gbl.Prefs.SideCols & Lay_SHOW_LEFT_COLUMN) ||	// Left column visible
       (Gbl.Hierarchy.Ins.InsCod > 0 &&		// Institution selected
        (Gbl.Action.Act == ActSeeCal ||			// Viewing calendar
         Gbl.Action.Act == ActPrnCal ||			// Printing calendar
         Gbl.Action.Act == ActChgCal1stDay)))		// Changing first day
     {
      /***** Get list of holidays *****/
      if (!Gbl.Hlds.LstIsRead)
	{
	 Gbl.Hlds.SelectedOrder = Hld_ORDER_BY_START_DATE;
	 Hld_GetListHolidays ();
	}

      /***** Create list of calls for examination *****/
      Exa_CreateListDatesOfExamAnnouncements ();

      /***** Write script to initialize variables used to draw months *****/
      fprintf (Gbl.F.Out,"<script type=\"text/javascript\">\n"
			 "	var DAYS_CAPS = [");
      for (DayOfWeek = 0;
	   DayOfWeek < 7;
	   DayOfWeek++)
	{
	 if (DayOfWeek)
	    fprintf (Gbl.F.Out,",");
	 fprintf (Gbl.F.Out,"'%c'",Txt_DAYS_CAPS[DayOfWeek][0]);
	}
      fprintf (Gbl.F.Out,"];\n");

      fprintf (Gbl.F.Out,"	var DAYS = [");
      for (DayOfWeek = 0;
	   DayOfWeek < 7;
	   DayOfWeek++)
	{
	 if (DayOfWeek)
	    fprintf (Gbl.F.Out,",");
	 fprintf (Gbl.F.Out,"'%s'",Txt_DAYS_SMALL[DayOfWeek]);
	}
      fprintf (Gbl.F.Out,"];\n");

      fprintf (Gbl.F.Out,"	var STR_EXAM = '");
      fprintf (Gbl.F.Out,Txt_Exam_of_X,Gbl.Hierarchy.Crs.FullName);
      fprintf (Gbl.F.Out,"';\n");

      fprintf (Gbl.F.Out,"	var Hlds = [];\n");
      for (NumHld = 0;
	   NumHld < Gbl.Hlds.Num;
	   NumHld++)
	 fprintf (Gbl.F.Out,"	Hlds.push({ PlcCod: %ld, HldTyp: %u, StartDate: %s, EndDate: %s, Name: '%s' });\n",
		  Gbl.Hlds.Lst[NumHld].PlcCod,
		  (unsigned) Gbl.Hlds.Lst[NumHld].HldTyp,
		  Gbl.Hlds.Lst[NumHld].StartDate.YYYYMMDD,
		  Gbl.Hlds.Lst[NumHld].EndDate.YYYYMMDD,
		  Gbl.Hlds.Lst[NumHld].Name);

      fprintf (Gbl.F.Out,"	var LstExamAnnouncements = [];\n");
      for (NumExamAnnouncement = 0;
	   NumExamAnnouncement < Gbl.ExamAnns.NumExaAnns;
	   NumExamAnnouncement++)
	 fprintf (Gbl.F.Out,"	LstExamAnnouncements.push({ ExaCod: %ld, Year: %u, Month: %u, Day: %u });\n",
		  Gbl.ExamAnns.Lst[NumExamAnnouncement].ExaCod,
		  Gbl.ExamAnns.Lst[NumExamAnnouncement].ExamDate.Year,
		  Gbl.ExamAnns.Lst[NumExamAnnouncement].ExamDate.Month,
		  Gbl.ExamAnns.Lst[NumExamAnnouncement].ExamDate.Day);

      fprintf (Gbl.F.Out,"</script>\n");

      /***** Free list of dates of exam announcements *****/
      Exa_FreeListExamAnnouncements ();
     }

   /***** Scripts depending on action *****/
   switch (Gbl.Action.Act)
     {
      /***** Script to print world map *****/
      case ActSeeCty:
         Cty_WriteScriptGoogleGeochart ();
         break;
      /***** Script for uploading files using Dropzone.js (http://www.dropzonejs.com/) *****/
      // The public directory dropzone must hold:
      // dropzone.js
      // css/dropzone.css
      // images/spritemap@2x.png
      // images/spritemap.png
      case ActFrmCreDocIns:	// Brw_ADMI_DOC_INS
      case ActFrmCreShaIns:	// Brw_ADMI_SHR_INS
      case ActFrmCreDocCtr:	// Brw_ADMI_DOC_CTR
      case ActFrmCreShaCtr:	// Brw_ADMI_SHR_CTR
      case ActFrmCreDocDeg:	// Brw_ADMI_DOC_DEG
      case ActFrmCreShaDeg:	// Brw_ADMI_SHR_DEG
      case ActFrmCreDocCrs:	// Brw_ADMI_DOC_CRS
      case ActFrmCreDocGrp:	// Brw_ADMI_DOC_GRP
      case ActFrmCreTchCrs:	// Brw_ADMI_TCH_CRS
      case ActFrmCreTchGrp:	// Brw_ADMI_TCH_GRP
      case ActFrmCreShaCrs:	// Brw_ADMI_SHR_CRS
      case ActFrmCreShaGrp:	// Brw_ADMI_SHR_GRP
      case ActFrmCreAsgUsr:	// Brw_ADMI_ASG_USR
      case ActFrmCreAsgCrs:	// Brw_ADMI_ASG_CRS
      case ActFrmCreWrkUsr:	// Brw_ADMI_WRK_USR
      case ActFrmCreWrkCrs:	// Brw_ADMI_WRK_CRS
      case ActFrmCreDocPrj:	// Brw_ADMI_DOC_PRJ
      case ActFrmCreAssPrj:	// Brw_ADMI_ASS_PRJ
      case ActFrmCreMrkCrs:	// Brw_ADMI_MRK_CRS
      case ActFrmCreMrkGrp:	// Brw_ADMI_MRK_GRP
      case ActFrmCreBrf:	// Brw_ADMI_BRF_USR
	 // Use charset="windows-1252" to force error messages in windows-1252 (default is UTF-8)
	 fprintf (Gbl.F.Out,"<script type=\"text/javascript\""
			    " src=\"%s/dropzone/dropzone.js\""
			    " charset=\"windows-1252\">"
			    "</script>\n",
		  Cfg_URL_SWAD_PUBLIC);
	 Lay_WriteScriptCustomDropzone ();
         break;
      case ActReqAccGbl:
      case ActSeeAccGbl:
      case ActReqAccCrs:
      case ActSeeAccCrs:
      case ActSeeAllStaCrs:
	 fprintf (Gbl.F.Out,"<script type=\"text/javascript\""
			    " src=\"%s/jstz/jstz.js\">"
			    "</script>\n",
		  Cfg_URL_SWAD_PUBLIC);
	 break;
      default:
	 break;
     }

   /***** Script for Google Analytics *****/
   fprintf (Gbl.F.Out,"%s",Cfg_GOOGLE_ANALYTICS);
  }

// Change page title
//function changeTitle(title) {
//	document.title = title;
//}

/*****************************************************************************/
/************ Write some scripts depending on the current action *************/
/*****************************************************************************/

static void Lay_WriteScriptMathJax (void)
  {
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
	    Cfg_URL_SWAD_PUBLIC);
#else
   // Use the MathJax Content Delivery Network (CDN)
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\""
	              " src=\"//cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML\">"
	              "</script>\n");
#endif
  }

/*****************************************************************************/
/******* Write script with init function executed after loading page *********/
/*****************************************************************************/

static void Lay_WriteScriptInit (void)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   bool RefreshConnected;
   bool RefreshNewTimeline = false;
   bool RefreshMatchStd       = false;
   bool RefreshMatchTch   = false;
   bool RefreshLastClicks  = false;

   RefreshConnected = Act_GetBrowserTab (Gbl.Action.Act) == Act_BRW_1ST_TAB &&
	              (Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN);	// Right column visible

   switch (Gbl.Action.Act)
     {
      case ActSeeSocTmlGbl:
      case ActRcvSocPstGbl:
      case ActRcvSocComGbl:
      case ActReqRemSocPubGbl:
      case ActRemSocPubGbl:
      case ActReqRemSocComGbl:
      case ActRemSocComGbl:
	 RefreshNewTimeline = true;
	 break;
      case ActJoiMch:
      case ActAnsMchQstStd:
	 RefreshMatchStd = true;
	 break;
      case ActNewMch:
      case ActResMch:
      case ActBckMch:
      case ActPlyMch:
      case ActPauMch:
      case ActFwdMch:
      case ActChgVisResMchQst:
	 RefreshMatchTch = true;
	 break;
      case ActLstClk:
	 RefreshLastClicks = true;
	 break;
      default:
	 break;
     }

   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">\n");

   Dat_WriteScriptMonths ();

   if (RefreshNewTimeline)	// Refresh new timeline via AJAX
      fprintf (Gbl.F.Out,"\tvar delayNewTL = %lu;\n",Cfg_TIME_TO_REFRESH_TIMELINE);
   else if (RefreshMatchStd)	// Refresh match via AJAX
      fprintf (Gbl.F.Out,"\tvar delayMatch = %lu;\n",Cfg_TIME_TO_REFRESH_MATCH_STD);
   else if (RefreshMatchTch)	// Refresh match via AJAX
      fprintf (Gbl.F.Out,"\tvar delayMatch = %lu;\n",Cfg_TIME_TO_REFRESH_MATCH_TCH);

   fprintf (Gbl.F.Out,"function init(){\n");

   fprintf (Gbl.F.Out,"\tActionAJAX = \"%s\";\n",
            Lan_STR_LANG_ID[Gbl.Prefs.Language]);

   if (RefreshConnected)	// Refresh connected users via AJAX
     {
      Con_WriteScriptClockConnected ();
      fprintf (Gbl.F.Out,"\tsetTimeout(\"refreshConnected()\",%lu);\n",
	       Gbl.Usrs.Connected.TimeToRefreshInMs);
     }

   if (RefreshLastClicks)	// Refresh last clicks via AJAX
      fprintf (Gbl.F.Out,"\tsetTimeout(\"refreshLastClicks()\",%lu);\n",
               Cfg_TIME_TO_REFRESH_LAST_CLICKS);
   else if (RefreshMatchStd)	// Refresh match for a student via AJAX
      fprintf (Gbl.F.Out,"\tsetTimeout(\"refreshMatchStd()\",delayMatch);\n");
   else if (RefreshMatchTch)	// Refresh match for a teacher via AJAX
      fprintf (Gbl.F.Out,"\tsetTimeout(\"refreshMatchTch()\",delayMatch);\n");
   else if (RefreshNewTimeline)	// Refresh timeline via AJAX
      fprintf (Gbl.F.Out,"\tsetTimeout(\"refreshNewTL()\",delayNewTL);\n");

   fprintf (Gbl.F.Out,"}\n"
                      "</script>\n");
  }

/*****************************************************************************/
/************** Write script to set parameters needed by AJAX ****************/
/*****************************************************************************/

static void Lay_WriteScriptParamsAJAX (void)
  {
   /***** Start script *****/
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">\n");

   /***** Parameters with code of session and current course code *****/
   fprintf (Gbl.F.Out,"var RefreshParamIdSes = \"ses=%s\";\n"
                      "var RefreshParamCrsCod = \"crs=%ld\";\n",
	    Gbl.Session.Id,
	    Gbl.Hierarchy.Crs.CrsCod);

   /***** Parameter to refresh connected users *****/
   if (Act_GetBrowserTab (Gbl.Action.Act) == Act_BRW_1ST_TAB)
      fprintf (Gbl.F.Out,"var RefreshParamNxtActCon = \"act=%ld\";\n",
	       Act_GetActCod (ActRefCon));

   /***** Parameters related with expanding/contracting folders in file browsers *****/
   if (Gbl.FileBrowser.Type != Brw_UNKNOWN)
      /* In all the actions related to file browsers ==>
	 put parameters used by AJAX */
      fprintf (Gbl.F.Out,"var RefreshParamExpand = \"act=%ld\";\n"
			 "var RefreshParamContract = \"act=%ld\";\n",
	       Act_GetActCod (Brw_GetActionExpand   ()),
	       Act_GetActCod (Brw_GetActionContract ()));

   /***** Parameters related with other actions *****/
   switch (Gbl.Action.Act)
     {
      /* Parameters related with global timeline refreshing */
      case ActSeeSocTmlGbl:
      case ActRcvSocPstGbl:
      case ActRcvSocComGbl:
      case ActReqRemSocPubGbl:
      case ActRemSocPubGbl:
      case ActReqRemSocComGbl:
      case ActRemSocComGbl:
	 /* In all the actions related to view or editing global timeline ==>
	    put parameters used by AJAX */
	 fprintf (Gbl.F.Out,"var RefreshParamNxtActNewPub = \"act=%ld\";\n"
			    "var RefreshParamNxtActOldPub = \"act=%ld\";\n"
			    "var RefreshParamWhichUsrs = \"WhichUsrs=%u\";\n",
		  Act_GetActCod (ActRefNewSocPubGbl),
		  Act_GetActCod (ActRefOldSocPubGbl),
		  (unsigned) Gbl.Timeline.WhichUsrs);
	 break;
      /* Parameters related with user timeline refreshing */
      case ActSeeOthPubPrf:
      case ActRcvSocPstUsr:
      case ActRcvSocComUsr:
      case ActReqRemSocPubUsr:
      case ActRemSocPubUsr:
      case ActReqRemSocComUsr:
      case ActRemSocComUsr:
	 /* In all the actions related to view or editing user's timeline ==>
	    put parameters used by AJAX */
	 if (Gbl.Usrs.Other.UsrDat.UsrCod <= 0)
	    Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();
	 if (!Gbl.Usrs.Other.UsrDat.Nickname[0])
	    Nck_GetNicknameFromUsrCod (Gbl.Usrs.Other.UsrDat.UsrCod,
				       Gbl.Usrs.Other.UsrDat.Nickname);
	 fprintf (Gbl.F.Out,"var RefreshParamNxtActOldPub = \"act=%ld\";\n"
			    "var RefreshParamUsr = \"OtherUsrCod=%s\";\n",
		  Act_GetActCod (ActRefOldSocPubUsr),
		  Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
	 break;
      /* Parameters related with match refreshing (for students) */
      case ActJoiMch:
      case ActAnsMchQstStd:
	 fprintf (Gbl.F.Out,"var RefreshParamNxtActMch = \"act=%ld\";\n"
			    "var RefreshParamMchCod = \"MchCod=%ld\";\n",
		  Act_GetActCod (ActRefMchStd),
		  Gbl.Games.MchCodBeingPlayed);
	 break;
      /* Parameters related with match refreshing (for teachers) */
      case ActNewMch:
      case ActResMch:
      case ActBckMch:
      case ActPlyMch:
      case ActPauMch:
      case ActFwdMch:
      case ActChgVisResMchQst:
	 fprintf (Gbl.F.Out,"var RefreshParamNxtActMch = \"act=%ld\";\n"
			    "var RefreshParamMchCod = \"MchCod=%ld\";\n",
		  Act_GetActCod (ActRefMchTch),
		  Gbl.Games.MchCodBeingPlayed);
	 break;
      /* Parameter related with clicks refreshing */
      case ActLstClk:
	 fprintf (Gbl.F.Out,"var RefreshParamNxtActLstClk = \"act=%ld\";\n",
		  Act_GetActCod (ActRefLstClk));
	 break;
      default:
	 break;
     }

   /***** End script *****/
   fprintf (Gbl.F.Out,"</script>\n");
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
   extern const char *The_ClassTagline[The_NUM_THEMES];
   extern const char *Txt_System;
   extern const char *Txt_TAGLINE;
   extern const char *Txt_TAGLINE_BR;
   const char *ClassHeadRow1[The_NUM_THEMES] =
     {
      "HEAD_ROW_1_WHITE",	// The_THEME_WHITE
      "HEAD_ROW_1_GREY",	// The_THEME_GREY
      "HEAD_ROW_1_PURPLE",	// The_THEME_PURPLE
      "HEAD_ROW_1_BLUE",	// The_THEME_BLUE
      "HEAD_ROW_1_YELLOW",	// The_THEME_YELLOW
      "HEAD_ROW_1_PINK",	// The_THEME_PINK
      };
   const char *ClassHeadRow2[The_NUM_THEMES] =
     {
      "HEAD_ROW_2_WHITE",	// The_THEME_WHITE
      "HEAD_ROW_2_GREY",	// The_THEME_GREY
      "HEAD_ROW_2_PURPLE",	// The_THEME_PURPLE
      "HEAD_ROW_2_BLUE",	// The_THEME_BLUE
      "HEAD_ROW_2_YELLOW",	// The_THEME_YELLOW
      "HEAD_ROW_2_PINK",	// The_THEME_PINK
      };

   /***** Start header *****/
   fprintf (Gbl.F.Out,"<header>");

   /***** 1st. row *****/
   /* Start 1st. row */
   fprintf (Gbl.F.Out,"<div id=\"head_row_1\" class=\"%s\">",
            ClassHeadRow1[Gbl.Prefs.Theme]);

   /* 1st. row, 1st. column: logo, tagline and search */
   fprintf (Gbl.F.Out,"<div id=\"head_row_1_left\">");

   /* Start form to go to home page */
   Frm_StartFormGoTo (ActMnu);
   Par_PutHiddenParamUnsigned ("NxtTab",(unsigned) TabSys);

   fprintf (Gbl.F.Out,"<div id=\"head_row_1_logo_small\">");
   Frm_LinkFormSubmit (Txt_System,NULL,NULL);
   fprintf (Gbl.F.Out,"<img src=\"%s/%s\" alt=\"%s\" title=\"%s\""
                      " class=\"CENTER_MIDDLE\""
	              " style=\"width:%upx; height:%upx;\" />"
                      "</a>",	// head_row_1_logo_small
            Cfg_URL_ICON_PUBLIC,Cfg_PLATFORM_LOGO_SMALL_FILENAME,
            Cfg_PLATFORM_SHORT_NAME,Cfg_PLATFORM_FULL_NAME,
            Cfg_PLATFORM_LOGO_SMALL_WIDTH,Cfg_PLATFORM_LOGO_SMALL_HEIGHT);
   fprintf (Gbl.F.Out,"</div>"
                      "<div id=\"head_row_1_logo_big\">");
   Frm_LinkFormSubmit (Txt_System,NULL,NULL);
   fprintf (Gbl.F.Out,"<img src=\"%s/%s\" alt=\"%s\" title=\"%s\""
                      " class=\"CENTER_MIDDLE\""
	              " style=\"width:%upx; height:%upx;\" />"
                      "</a>",	// head_row_1_logo_big
            Cfg_URL_ICON_PUBLIC,Cfg_PLATFORM_LOGO_BIG_FILENAME,
            Cfg_PLATFORM_SHORT_NAME,Cfg_PLATFORM_FULL_NAME,
            Cfg_PLATFORM_LOGO_BIG_WIDTH,Cfg_PLATFORM_LOGO_BIG_HEIGHT);
   fprintf (Gbl.F.Out,"</div>"
                      "<div id=\"head_row_1_tagline\">");
   Frm_LinkFormSubmit (Txt_TAGLINE,The_ClassTagline[Gbl.Prefs.Theme],NULL);
   fprintf (Gbl.F.Out,"%s"
	              "</a>"
                      "</div>",	// head_row_1_tagline
            Txt_TAGLINE_BR);

   /* End form to go to home page */
   Frm_EndForm ();

   Sch_PutFormToSearchInPageTopHeading ();

   fprintf (Gbl.F.Out,"</div>");	// head_row_1_left

   /* 1st. row, 3rd. column: logged user or language selection,
      and link to open/close session */
   fprintf (Gbl.F.Out,"<div id=\"head_row_1_right\">");
   if (Gbl.Usrs.Me.Logged)
      Usr_WriteLoggedUsrHead ();
   else
      Lan_PutSelectorToSelectLanguage ();

   fprintf (Gbl.F.Out,"<div id=\"login_box\">");
   if (Gbl.Usrs.Me.Logged)
      Usr_PutFormLogOut ();
   else
      Usr_PutFormLogIn ();
   fprintf (Gbl.F.Out,"</div>"		// login_box
	              "</div>");	// head_row_1_right

   /* End 1st. row */
   fprintf (Gbl.F.Out,"</div>");	// head_row_1

   /***** 2nd. row *****/
   /* Start second row */
   fprintf (Gbl.F.Out,"<div id=\"head_row_2\" class=\"%s\">",
            ClassHeadRow2[Gbl.Prefs.Theme]);

   /* 2nd. row, 1st. column
      Clock with hour:minute (server hour is shown) */
   fprintf (Gbl.F.Out,"<div id=\"head_row_2_time\">");
   Dat_ShowClientLocalTime ();
   fprintf (Gbl.F.Out,"</div>");	// End first column

   /* 2nd. row, 2nd. column: degree and course */
   fprintf (Gbl.F.Out,"<div id=\"head_row_2_hierarchy\">");
   Lay_WriteBreadcrumb ();
   Hie_WriteBigNameCtyInsCtrDegCrs ();
   fprintf (Gbl.F.Out,"</div>");

   /* 2nd. row, 3rd. column */
   fprintf (Gbl.F.Out,"<div id=\"msg\">");	// Used for AJAX based refresh
   if (Gbl.Usrs.Me.Logged)
      Ntf_WriteNumberOfNewNtfs ();
   fprintf (Gbl.F.Out,"</div>");		// Used for AJAX based refresh

   /* End 2nd. row */
   fprintf (Gbl.F.Out,"</div>");

   /***** End header *****/
   fprintf (Gbl.F.Out,"</header>");
  }

/*****************************************************************************/
/*********** Write breadcrumb with the path to the current location **********/
/*****************************************************************************/

static void Lay_WriteBreadcrumb (void)
  {
   fprintf (Gbl.F.Out,"<nav id=\"breadcrumb\">");
   Crs_PutIconToSelectMyCoursesInBreadcrumb ();
   Hie_WriteHierarchyInBreadcrumb ();
   Crs_WriteSelectorMyCoursesInBreadcrumb ();
   fprintf (Gbl.F.Out,"</nav>");
  }

/*****************************************************************************/
/*********** Write icon and title associated to the current action ***********/
/*****************************************************************************/

static void Lay_WriteTitleAction (void)
  {
   extern const char *The_ClassTitleAction[The_NUM_THEMES];
   extern const char *The_ClassSubtitleAction[The_NUM_THEMES];
   extern const char *Txt_TABS_TXT[Tab_NUM_TABS];

   /***** Container start *****/
   fprintf (Gbl.F.Out,"<div id=\"action_title\""
	              " style=\"background-image:url('%s/%s');\">",
	    Gbl.Prefs.URLIconSet,
	    Act_GetIcon (Act_GetSuperAction (Gbl.Action.Act)));

   /***** Title *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s &gt; %s</div>",
	    The_ClassTitleAction[Gbl.Prefs.Theme],
	    Txt_TABS_TXT[Act_GetTab (Gbl.Action.Act)],
	    Act_GetTitleAction (Gbl.Action.Act));

   /***** Subtitle *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s</div>",
	    The_ClassSubtitleAction[Gbl.Prefs.Theme],
	    Act_GetSubtitleAction (Gbl.Action.Act));

   /***** Container end *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/***************************** Show left column ******************************/
/*****************************************************************************/

static void Lay_ShowLeftColumn (void)
  {
   struct MFU_ListMFUActions ListMFUActions;

   fprintf (Gbl.F.Out,"<div style=\"width:160px;\">");

   fprintf (Gbl.F.Out,"<div class=\"LEFT_RIGHT_CELL\">");
   if (Gbl.Usrs.Me.Logged)
     {
      /***** Most frequently used actions *****/
      MFU_AllocateMFUActions (&ListMFUActions,6);
      MFU_GetMFUActions (&ListMFUActions,6);
      MFU_WriteSmallMFUActions (&ListMFUActions);
      MFU_FreeMFUActions (&ListMFUActions);
     }
   else
      /***** Institutional links *****/
      Lnk_WriteMenuWithInstitutionalLinks ();
   fprintf (Gbl.F.Out,"</div>");

   /***** Month *****/
   fprintf (Gbl.F.Out,"<div class=\"LEFT_RIGHT_CELL\">");
   Cal_DrawCurrentMonth ();
   fprintf (Gbl.F.Out,"</div>");

   /***** Notices (yellow notes) *****/
   if (Gbl.Hierarchy.Level == Hie_CRS)
     {
      fprintf (Gbl.F.Out,"<div class=\"LEFT_RIGHT_CELL\">");
      Not_ShowNotices (Not_LIST_BRIEF_NOTICES,
	               -1L);	// No notice highlighted
      fprintf (Gbl.F.Out,"</div>");
     }

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/**************************** Show right column ******************************/
/*****************************************************************************/

static void Lay_ShowRightColumn (void)
  {
   extern const char *Txt_If_you_have_an_Android_device_try_SWADroid;
   // struct timeval tv1,tv2;
   // long tv_usecs;

   /***** Banners *****/
   Ban_WriteMenuWithBanners ();

   /***** Number of connected users in the whole platform *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
     {
      fprintf (Gbl.F.Out,"<div id=\"globalconnected\""	// Used for AJAX based refresh
			 " class=\"LEFT_RIGHT_CELL\">");
      Con_ShowGlobalConnectedUsrs ();
      fprintf (Gbl.F.Out,"</div>");			// Used for AJAX based refresh
     }

   /***** Number of connected users in the current course *****/
   if (Gbl.Hierarchy.Level == Hie_CRS)	// There is a course selected
     {
      fprintf (Gbl.F.Out,"<div id=\"courseconnected\""	// Used for AJAX based refresh
	                 " class=\"LEFT_RIGHT_CELL\">");
      Gbl.Scope.Current = Hie_CRS;
      Con_ShowConnectedUsrsBelongingToCurrentCrs ();
      fprintf (Gbl.F.Out,"</div>");			// Used for AJAX based refresh
     }
   else if (Gbl.Usrs.Me.Logged)		// I am logged
     {
      /*
      gettimeofday (&tv1,NULL);
      */
      /***** Suggest one user to follow *****/
      fprintf (Gbl.F.Out,"<div class=\"LEFT_RIGHT_CELL\">");
      Fol_SuggestUsrsToFollowMainZoneOnRightColumn ();
      /*
      gettimeofday (&tv2,NULL);
      tv_usecs = (tv2.tv_sec  - tv1.tv_sec ) * 1E6 +
	         (tv2.tv_usec - tv1.tv_usec);

      DB_QueryINSERT ("can not debug",
		      "INSERT INTO debug"
		      " (DebugTime,Txt)"
		      " VALUES"
		      " (NOW(),'Fol_SuggestUsrsToFollowMainZoneOnRightColumn: %ld us')",
		      tv_usecs);
      */
      fprintf (Gbl.F.Out,"</div>");
     }

   if (!Gbl.Usrs.Me.Logged)
      /***** SWADroid advertisement *****/
      fprintf (Gbl.F.Out,"<div class=\"LEFT_RIGHT_CELL\">"
			 "<a href=\"https://play.google.com/store/apps/details?id=es.ugr.swad.swadroid\""
			 " target=\"_blank\" title=\"%s\">"
			 "<img src=\"%s/SWADroid120x200.png\""
			 " alt=\"SWADroid\" title=\"SWADroid\""
			 " style=\"width:150px; height:250px;\" />"
			 "</a>"
			 "</div>",
	       Txt_If_you_have_an_Android_device_try_SWADroid,
	       Cfg_URL_ICON_PUBLIC);
  }

/*****************************************************************************/
/**************** Show an icon with a link in contextual menu ****************/
/*****************************************************************************/

void Lay_PutContextualLinkOnlyIcon (Act_Action_t NextAction,const char *Anchor,
				    void (*FuncParams) (void),
				    const char *Icon,
				    const char *Title)
  {
   /***** Start form *****/
   Frm_StartFormAnchor (NextAction,Anchor);
   if (FuncParams)
      FuncParams ();

   /***** Put icon with link *****/
   Ico_PutIconLink (Icon,Title);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/**************** Show an icon with a link in contextual menu ****************/
/*****************************************************************************/

void Lay_PutContextualLinkIconText (Act_Action_t NextAction,const char *Anchor,
				    void (*FuncParams) (void),
				    const char *Icon,
				    const char *Text)
  {
   extern const char *The_ClassFormOutBoxBold[The_NUM_THEMES];

   /***** Separator *****/
   fprintf (Gbl.F.Out," ");	// This space is necessary to enable
				// jumping to the next line on narrow screens

   /***** Start form *****/
   Frm_StartFormAnchor (NextAction,Anchor);
   if (FuncParams)
      FuncParams ();

   /***** Put icon and text with link *****/
   Frm_LinkFormSubmit (Text,The_ClassFormOutBoxBold[Gbl.Prefs.Theme],NULL);
   Ico_PutIconTextLink (Icon,Text);
   fprintf (Gbl.F.Out,"</a>");

   /***** End form *****/
   Frm_EndForm ();

   /***** Separator *****/
   fprintf (Gbl.F.Out," ");	// This space is necessary to enable
				// jumping to the next line on narrow screens
  }

/*****************************************************************************/
/**************** Show an icon with a link in contextual menu ****************/
/*****************************************************************************/

void Lay_PutContextualLinkIconTextOnSubmit (Act_Action_t NextAction,const char *Anchor,
					    void (*FuncParams) (void),
					    const char *Icon,
					    const char *Text,
					    const char *OnSubmit)
  {
   extern const char *The_ClassFormOutBoxBold[The_NUM_THEMES];

   /***** Separator *****/
   fprintf (Gbl.F.Out," ");	// This space is necessary to enable
				// jumping to the next line on narrow screens

   /***** Start form *****/
   Frm_StartFormAnchor (NextAction,Anchor);
   if (FuncParams)
      FuncParams ();

   /***** Put icon with link *****/
   Frm_LinkFormSubmit (Text,The_ClassFormOutBoxBold[Gbl.Prefs.Theme],OnSubmit);
   Ico_PutIconTextLink (Icon,Text);
   fprintf (Gbl.F.Out,"</a>");

   /***** End form *****/
   Frm_EndForm ();

   /***** Separator *****/
   fprintf (Gbl.F.Out," ");	// This space is necessary to enable
				// jumping to the next line on narrow screens
  }

/*****************************************************************************/
/******************** Show a checkbox in contextual menu *********************/
/*****************************************************************************/

void Lay_PutContextualCheckbox (Act_Action_t NextAction,
                                void (*FuncParams) (void),
                                const char *CheckboxName,
                                bool Checked,bool Disabled,
                                const char *Title,const char *Text)
  {
   extern const char *The_ClassFormOutBoxBold[The_NUM_THEMES];

   /***** Separator *****/
   if (Text)
      fprintf (Gbl.F.Out," ");	// This space is necessary to enable
				// jumping to the next line on narrow screens

   /***** Start form *****/
   Frm_StartForm (NextAction);
   if (FuncParams)
      FuncParams ();

   /***** Start container and label *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_OPT %s %s\" title=\"%s\">"
	              "<label>",
            Checked ? "CHECKBOX_CHECKED" :
        	      "CHECKBOX_UNCHECKED",
            The_ClassFormOutBoxBold[Gbl.Prefs.Theme],
            Title);

   /****** Checkbox and text *****/
   fprintf (Gbl.F.Out,"<input type=\"checkbox\" name=\"%s\" value=\"Y\"",
            CheckboxName);
   if (Checked)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   if (Disabled)
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   else
      fprintf (Gbl.F.Out," onclick=\"document.getElementById('%s').submit();\"",
	       Gbl.Form.Id);
   fprintf (Gbl.F.Out," />");
   if (Text)
      if (Text[0])
	 fprintf (Gbl.F.Out,"&nbsp;%s",
		  Text);

   /***** End label and container *****/
   fprintf (Gbl.F.Out,"</label>"
	              "</div>");

   /***** End form *****/
   Frm_EndForm ();

   /***** Separator *****/
   if (Text)
      fprintf (Gbl.F.Out," ");	// This space is necessary to enable
				// jumping to the next line on narrow screens
  }

/*****************************************************************************/
/****************************** Start/end section ****************************/
/*****************************************************************************/

void Lay_StartSection (const char *SectionId)
  {
   fprintf (Gbl.F.Out,"<section id=\"%s\">",SectionId);
  }

void Lay_EndSection (void)
  {
   fprintf (Gbl.F.Out,"</section>");
  }

/*****************************************************************************/
/****************************** Start/end article ****************************/
/*****************************************************************************/

void Lay_StartArticle (const char *ArticleId)
  {
   fprintf (Gbl.F.Out,"<article id=\"%s\">",ArticleId);
  }

void Lay_EndArticle (void)
  {
   fprintf (Gbl.F.Out,"</article>");
  }

/*****************************************************************************/
/********** Write error message and exit when not enough memory **************/
/*****************************************************************************/

void Lay_NotEnoughMemoryExit (void)
  {
   Lay_ShowErrorAndExit ("Not enough memory.");
  }

/*****************************************************************************/
/************* Write error message and exit when wrong scope *****************/
/*****************************************************************************/

void Lay_WrongScopeExit (void)
  {
   Lay_ShowErrorAndExit ("Wrong scope.");
  }

/*****************************************************************************/
/******* Write error message, close files, remove lock file, and exit ********/
/*****************************************************************************/

void Lay_ShowErrorAndExit (const char *Txt)
  {
   /***** Unlock tables if locked *****/
   if (Gbl.DB.LockedTables)
     {
      Gbl.DB.LockedTables = false;
      mysql_query (&Gbl.mysql,"UNLOCK TABLES");
     }

   if (!Gbl.WebService.IsWebService)
     {
      /****** If start of page is not written yet, do it now ******/
      if (!Gbl.Layout.HTMLStartWritten)
	 Lay_WriteStartOfPage ();

      /***** Write possible error message *****/
      if (Txt)
         Ale_ShowAlert (Ale_ERROR,Txt);

      /***** Finish the page, except </body> and </html> *****/
      Lay_WriteEndOfPage ();
     }

   /***** Free memory and close all the open files *****/
   Gbl_Cleanup ();

   /***** Page is generated (except </body> and </html>).
          Compute time to generate page *****/
   if (!Gbl.Action.IsAJAXAutoRefresh)
      Sta_ComputeTimeToGeneratePage ();

   if (Gbl.WebService.IsWebService)		// Serving a plugin request
     {
      /***** Log access *****/
      Gbl.TimeSendInMicroseconds = 0L;
      Sta_LogAccess (Txt);
     }
   else
     {
      /***** Send page.
             The HTML output is now in Gbl.F.Out file ==>
             ==> copy it to standard output *****/
      rewind (Gbl.F.Out);
      Fil_FastCopyOfOpenFiles (Gbl.F.Out,stdout);
      Fil_CloseAndRemoveFileForHTMLOutput ();

      if (!Gbl.Action.IsAJAXAutoRefresh)
	{
	 /***** Compute time to send page *****/
	 Sta_ComputeTimeToSendPage ();

	 /***** Log access *****/
	 Sta_LogAccess (Txt);

	 /***** Update last data for next time *****/
	 if (Gbl.Usrs.Me.Logged)
	   {
	    Usr_UpdateMyLastData ();
	    Crs_UpdateCrsLast ();
	   }

	 /***** End the output *****/
	 if (!Gbl.Layout.HTMLEndWritten)
	   {
	    // Here Gbl.F.Out is stdout
	    if (Act_GetBrowserTab (Gbl.Action.Act) == Act_BRW_1ST_TAB)
	       Lay_WriteAboutZone ();

	    fprintf (Gbl.F.Out,"</body>\n"
			       "</html>\n");
	    Gbl.Layout.HTMLEndWritten = true;
	   }
	}
     }

   /***** Close database connection *****/
   DB_CloseDBConnection ();

   /***** Exit *****/
   if (Gbl.WebService.IsWebService)
      API_Exit (Txt);
   exit (0);
  }

/*****************************************************************************/
/***************** Write about zone at the end of the page *******************/
/*****************************************************************************/

static void Lay_WriteAboutZone (void)
  {
   extern const char *Txt_About_X;
   extern const char *Txt_Questions_and_problems;

   /***** Start about zone *****/
   fprintf (Gbl.F.Out,"<address id=\"about_zone\" class=\"ABOUT\">");

   /***** Institution and centre hosting the platform *****/
   fprintf (Gbl.F.Out,"<a href=\"%s\" class=\"ABOUT\" target=\"_blank\">"
		      "<img src=\"%s/%s\""
		      " alt=\"%s\" title=\"%s\""
		      " style=\"width:%upx; height:%upx;\" />"
		      "<div>%s</div>"
		      "</a>",
	    Cfg_ABOUT_URL,
	    Cfg_URL_ICON_PUBLIC,Cfg_ABOUT_LOGO,
	    Cfg_ABOUT_NAME,Cfg_ABOUT_NAME,
	    Cfg_ABOUT_LOGO_WIDTH,Cfg_ABOUT_LOGO_HEIGHT,
	    Cfg_ABOUT_NAME);

   /***** Questions and problems *****/
   fprintf (Gbl.F.Out,"<div>"
		      "%s: "
		      "<a href=\"mailto:%s\" class=\"ABOUT\" target=\"_blank\">%s</a>"
		      "</div>",
	    Txt_Questions_and_problems,
	    Cfg_PLATFORM_RESPONSIBLE_EMAIL,Cfg_PLATFORM_RESPONSIBLE_EMAIL);

   /***** About and time to generate and send page *****/
   fprintf (Gbl.F.Out,"<div>");

   /* About */
   fprintf (Gbl.F.Out,"<a href=\"%s\" class=\"ABOUT\" target=\"_blank\">",
	    Cfg_ABOUT_SWAD_URL);
   fprintf (Gbl.F.Out,Txt_About_X,Log_PLATFORM_VERSION);
   fprintf (Gbl.F.Out,"</a>"
	              "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");

   /* Time to generate and send page */
   Sta_WriteTimeToGenerateAndSendPage ();

   fprintf (Gbl.F.Out,"</div>");

   /***** End about zone *****/
   fprintf (Gbl.F.Out,"</address>");	// about_zone
  }

/*****************************************************************************/
/*********** Refresh notifications and connected users via AJAX **************/
/*****************************************************************************/

void Lay_RefreshNotifsAndConnected (void)
  {
   unsigned NumUsr;
   bool ShowConnected = (Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN) &&
                        Gbl.Hierarchy.Level == Hie_CRS;	// Right column visible && There is a course selected

   /***** Sometimes, someone must do this work,
          so who best than processes that refresh via AJAX? *****/
   // We use (PID % prime-number) to do only one action as much
   if      (!(Gbl.PID %  11))
      Ntf_SendPendingNotifByEMailToAllUsrs ();	// Send pending notifications by email
   else if (!(Gbl.PID %  19))
      FW_PurgeFirewall ();			// Remove old clicks from firewall
   else if (!(Gbl.PID %  23))
      Fil_RemoveOldTmpFiles (Cfg_PATH_FILE_BROWSER_TMP_PUBLIC	,Cfg_TIME_TO_DELETE_BROWSER_TMP_FILES	,false);	// Remove the oldest temporary public directories used for downloading
   else if (!(Gbl.PID % 101))
      Brw_RemoveExpiredExpandedFolders ();	// Remove old expanded folders (from all users)
   else if (!(Gbl.PID % 103))
      Set_RemoveOldSettingsFromIP ();		// Remove old settings from IP
   else if (!(Gbl.PID % 107))
      Sta_RemoveOldEntriesRecentLog ();		// Remove old entries in recent log table, it's a slow query
   else if (!(Gbl.PID % 109))
      Fil_RemoveOldTmpFiles (Cfg_PATH_OUT_PRIVATE		,Cfg_TIME_TO_DELETE_HTML_OUTPUT		,false);
   else if (!(Gbl.PID % 113))
      Fil_RemoveOldTmpFiles (Cfg_PATH_PHOTO_TMP_PUBLIC		,Cfg_TIME_TO_DELETE_PHOTOS_TMP_FILES	,false);
   else if (!(Gbl.PID % 127))
      Fil_RemoveOldTmpFiles (Cfg_PATH_PHOTO_TMP_PRIVATE		,Cfg_TIME_TO_DELETE_PHOTOS_TMP_FILES	,false);
   else if (!(Gbl.PID % 131))
      Fil_RemoveOldTmpFiles (Cfg_PATH_MEDIA_TMP_PRIVATE		,Cfg_TIME_TO_DELETE_MEDIA_TMP_FILES	,false);
   else if (!(Gbl.PID % 137))
      Fil_RemoveOldTmpFiles (Cfg_PATH_ZIP_PRIVATE		,Cfg_TIME_TO_DELETE_BROWSER_ZIP_FILES	,false);
   else if (!(Gbl.PID % 139))
      Fil_RemoveOldTmpFiles (Cfg_PATH_MARK_PRIVATE		,Cfg_TIME_TO_DELETE_MARKS_TMP_FILES	,false);
   else if (!(Gbl.PID % 149))
      Fil_RemoveOldTmpFiles (Cfg_PATH_TEST_PRIVATE		,Cfg_TIME_TO_DELETE_TEST_TMP_FILES	,false);

   /***** Send, before the HTML, the refresh time *****/
   fprintf (Gbl.F.Out,"%lu|",Gbl.Usrs.Connected.TimeToRefreshInMs);
   if (Gbl.Usrs.Me.Logged)
      Ntf_WriteNumberOfNewNtfs ();
   fprintf (Gbl.F.Out,"|");
   Con_ShowGlobalConnectedUsrs ();
   fprintf (Gbl.F.Out,"|");
   if (ShowConnected)
     {
      Gbl.Scope.Current = Hie_CRS;
      Con_ShowConnectedUsrsBelongingToCurrentCrs ();
     }
   fprintf (Gbl.F.Out,"|");
   if (ShowConnected)
      fprintf (Gbl.F.Out,"%u",Gbl.Usrs.Connected.NumUsrsToList);
   fprintf (Gbl.F.Out,"|");
   if (ShowConnected)
      for (NumUsr = 0;
	   NumUsr < Gbl.Usrs.Connected.NumUsrsToList;
	   NumUsr++)
         fprintf (Gbl.F.Out,"%ld|",Gbl.Usrs.Connected.Lst[NumUsr].TimeDiff);
  }

/*****************************************************************************/
/**************** Refresh last clicks in realtime via AJAX *******************/
/*****************************************************************************/

void Lay_RefreshLastClicks (void)
  {
   if (Gbl.Session.IsOpen)	// If session has been closed, do not write anything
     {
      /***** Send, before the HTML, the refresh time *****/
      fprintf (Gbl.F.Out,"%lu|",Cfg_TIME_TO_REFRESH_LAST_CLICKS);

      /***** Get and show last clicks *****/
      Sta_GetAndShowLastClicks ();
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
      fprintf (Gbl.F.Out,"<footer id=\"foot_zone\">");

      /***** Copy HTML to output file *****/
      Fil_FastCopyOfOpenFiles (FileHTML,Gbl.F.Out);
      fclose (FileHTML);

      fprintf (Gbl.F.Out,"</footer>");
     }
  }

/*****************************************************************************/
/****** Write header and footer of the class photo or academic calendar ******/
/*****************************************************************************/

void Lay_WriteHeaderClassPhoto (bool PrintView,bool DrawingClassPhoto,
                                long InsCod,long DegCod,long CrsCod)
  {
   struct Instit Ins;
   struct Degree Deg;
   struct Course Crs;

   /***** Get data of institution *****/
   Ins.InsCod = InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);

   /***** Get data of degree *****/
   Deg.DegCod = DegCod;
   Deg_GetDataOfDegreeByCod (&Deg);

   /***** Get data of course *****/
   Crs.CrsCod = CrsCod;
   Crs_GetDataOfCourseByCod (&Crs);

   /***** Start table *****/
   fprintf (Gbl.F.Out,"<table style=\"width:100%%; padding:12px;\">"
                      "<tr>");

   /***** First column: institution logo *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\" style=\"width:80px;\">");
   if (InsCod > 0)
     {
      if (!PrintView)
         fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\">",Ins.WWW);
      Log_DrawLogo (Hie_INS,Ins.InsCod,Ins.ShrtName,40,NULL,true);
      if (!PrintView)
        fprintf (Gbl.F.Out,"</a>");
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** Second column: class photo title *****/
   fprintf (Gbl.F.Out,"<td class=\"CLASSPHOTO_TITLE CENTER_MIDDLE\">");
   if (InsCod > 0)
     {
      if (!PrintView)
         fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\""
                            " class=\"CLASSPHOTO_TITLE\">",
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
         fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\""
                            " class=\"CLASSPHOTO_TITLE\">",
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
                            " class=\"CLASSPHOTO_TITLE\">",
                  Deg.WWW);
      Log_DrawLogo (Hie_DEG,Deg.DegCod,Deg.ShrtName,40,NULL,true);
      if (!PrintView)
         fprintf (Gbl.F.Out,"</a>");
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** End table *****/
   fprintf (Gbl.F.Out,"</tr>");
   Tbl_EndTable ();
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

      /***** Start box and table *****/
      Box_StartBoxTable (NULL,NULL,NULL,
                         NULL,Box_NOT_CLOSABLE,8);

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
               Cfg_URL_ICON_PUBLIC,
               Txt_Stay_connected_with_SWADroid);

      /***** End table and box *****/
      Box_EndBoxTable ();

      fprintf (Gbl.F.Out,"</div>");
     }
  }

/*****************************************************************************/
/*********************** Indent forum or chat title **************************/
/*****************************************************************************/
// IsLastItemInLevel[] is a vector with at least 1 + Level booleans

void Lay_IndentDependingOnLevel (unsigned Level,bool IsLastItemInLevel[])
  {
   unsigned i;

   /***** Indent (from 1 to Level-1) *****/
   for (i = 1;
	i < Level;
	i++)
      fprintf (Gbl.F.Out,"<img src=\"%s/%s20x20.gif\""
	                 " alt=\"\" title=\"\""
                         " class=\"ICO25x25\" />",
		  Cfg_URL_ICON_PUBLIC,
		  IsLastItemInLevel[i] ? "tr" :
		                         "subleft");

   /***** Level *****/
   if (Level)
      fprintf (Gbl.F.Out,"<img src=\"%s/%s20x20.gif\""
			 " alt=\"\" title=\"\""
			 " class=\"ICO25x25\" />",
	       Cfg_URL_ICON_PUBLIC,
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
                       "$$LaTeX$$&nbsp;\\[LaTeX\\]");
  }

void Lay_HelpRichEditor (void)
  {
   Lay_HelpTextEditor ("<a href=\"http://johnmacfarlane.net/pandoc/README.html#pandocs-markdown\""
	               " target=\"_blank\">Markdown+Pandoc</a>",
                       "$LaTeX$",
                       "$$LaTeX$$");
  }

static void Lay_HelpTextEditor (const char *Text,const char *InlineMath,const char *Equation)
  {
   extern const char *Txt_Text;
   extern const char *Txt_Inline_math;
   extern const char *Txt_Equation_centered;

   fprintf (Gbl.F.Out,"<div class=\"HELP_EDIT\">"
	              "%s: %s"
                      " "
                      "%s: <code>%s</code>"
                      " "
                      "%s: <code>%s</code>"
                      "</div>",
            Txt_Text,Text,
            Txt_Inline_math,InlineMath,
            Txt_Equation_centered,Equation);
  }

/*****************************************************************************/
/************************** Help for the text editor *************************/
/*****************************************************************************/

void Lay_StartHTMLFile (FILE *File,const char *Title)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   fprintf (File,"<!DOCTYPE html>\n"
		 "<html lang=\"%s\">\n"
		 "<head>\n"
		 "<meta http-equiv=\"Content-Type\""
		 " content=\"text/html;charset=windows-1252\" />\n"
		 "<title>%s</title>\n"
		 "</head>\n",
	    Lan_STR_LANG_ID[Gbl.Prefs.Language],	// Language
	    Title);					// Page title
  }
