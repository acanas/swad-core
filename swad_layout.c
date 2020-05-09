// swad_layout.c: page layout

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <stddef.h>		// For NULL
#include <stdlib.h>		// For exit
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_API.h"
#include "swad_banner.h"
#include "swad_box.h"
#include "swad_calendar.h"
#include "swad_changelog.h"
#include "swad_config.h"
#include "swad_connected.h"
#include "swad_database.h"
#include "swad_exam_announcement.h"
#include "swad_exam_event.h"
#include "swad_firewall.h"
#include "swad_follow.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy.h"
#include "swad_holiday.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_link.h"
#include "swad_log.h"
#include "swad_logo.h"
#include "swad_match.h"
#include "swad_MFU.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_setting.h"
#include "swad_tab.h"
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
   static const char *LayoutMainZone[Mnu_NUM_MENUS] =
     {
      [Mnu_MENU_HORIZONTAL] = "main_horizontal",
      [Mnu_MENU_VERTICAL  ] = "main_vertical",
     };
   Act_BrowserTab_t BrowserTab;

   /***** If, when this function is called, the head is being written
          or the head is already written ==> don't do anything *****/
   if (Gbl.Layout.WritingHTMLStart ||
       Gbl.Layout.HTMLStartWritten)
      return;

   /***** Compute connected users to be displayed in right column *****/
   Con_ComputeConnectedUsrsBelongingToCurrentCrs ();

   /***** Send head width the file type for the HTTP protocol *****/
   if (Gbl.Action.UsesAJAX)
     // Don't generate a full HTML page, only the content of a DIV or similar
     {
      HTM_Txt ("Content-Type: text/html; charset=windows-1252\r\n\r\n");
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
   HTM_TxtF ("<html lang=\"%s\">\n",Lan_STR_LANG_ID[Gbl.Prefs.Language]);
   HTM_Txt ("<head>\n"
            "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=windows-1252\" />\n"
            "<meta name=\"description\" content=\"A free-software, educational, online tool for managing courses and students.\" />\n"
            "<meta name=\"keywords\" content=\"");
   HTM_Txt (Cfg_PLATFORM_SHORT_NAME);
   HTM_Txt (","
            "SWAD,"
            "shared workspace at a distance,"
            "educational platform,"
            "sistema web de apoyo a la docencia,"
            "plataforma educativa,"
            "campus virtual,"
            "SWADroid,"
            "LMS,"
            "Learning Management System\" />\n");

   /* Viewport (used for responsive design) */
   HTM_Txt ("<meta name=\"viewport\""
	    " content=\"width=device-width, initial-scale=1.0\">\n");

   /* Title */
   Lay_WritePageTitle ();

   /* Canonical URL */
   HTM_TxtF ("<link rel=\"canonical\" href=\"%s\" />\n",Cfg_URL_SWAD_CGI);

   /* Favicon */
   HTM_TxtF ("<link type=\"image/x-icon\" href=\"%s/favicon.ico\" rel=\"icon\" />\n",
	     Cfg_URL_ICON_PUBLIC);
   HTM_TxtF ("<link type=\"image/x-icon\" href=\"%s/favicon.ico\" rel=\"shortcut icon\" />\n",
	     Cfg_URL_ICON_PUBLIC);

   /* Style sheet for SWAD */
   HTM_TxtF ("<link rel=\"stylesheet\" href=\"%s/%s\" type=\"text/css\" />\n",
             Cfg_URL_SWAD_PUBLIC,CSS_FILE);

   /* Style sheets for Font Awesome */
   HTM_TxtF ("<link rel=\"stylesheet\""
	     " href=\"%s/fontawesome/css/fontawesome.css\""
	     " type=\"text/css\" />\n",
             Cfg_URL_SWAD_PUBLIC);
   HTM_TxtF ("<link rel=\"stylesheet\""
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
	 HTM_TxtF ("<link rel=\"stylesheet\""
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

   HTM_Txt ("</head>\n");

   /***** HTML body *****/
   BrowserTab = Act_GetBrowserTab (Gbl.Action.Act);
   switch (BrowserTab)
     {
      case Act_BRW_1ST_TAB:
	 HTM_Txt ("<body onload=\"init();\">\n");
	 HTM_DIV_Begin ("id=\"zoomLyr\" class=\"ZOOM\"");
	 HTM_IMG (Cfg_URL_ICON_PUBLIC,"usr_bl.jpg",NULL,
	          "class=\"IMG_USR\" id=\"zoomImg\"");
	 HTM_DIV_Begin ("id=\"zoomTxt\" class=\"CM\"");
	 HTM_DIV_End ();
	 HTM_DIV_End ();
	 break;
      case Act_BRW_NEW_TAB:
      case Act_BRW_2ND_TAB:
	 HTM_Txt ("<body onload=\"init();\"");
	 switch (Gbl.Action.Act)
	   {
	    case ActNewExaEvt:
	    case ActResExaEvt:
	    case ActBckExaEvt:
	    case ActPlyPauExaEvt:
	    case ActFwdExaEvt:
	    case ActChgNumColExaEvt:
	    case ActChgVisResExaEvtQst:
	    case ActExaEvtCntDwn:
	       HTM_Txt (" class=\"EXA_BG\"");
	       break;
	    case ActNewMch:
	    case ActResMch:
	    case ActBckMch:
	    case ActPlyPauMch:
	    case ActFwdMch:
	    case ActChgNumColMch:
	    case ActChgVisResMchQst:
	    case ActMchCntDwn:
	       HTM_Txt (" class=\"MCH_BG\"");
	       break;
	    default:
	       break;
           }
	 HTM_Txt (">\n");
         Gbl.Layout.WritingHTMLStart = false;
	 Gbl.Layout.HTMLStartWritten =
	 Gbl.Layout.DivsEndWritten   = true;
	 return;
      default:
	 HTM_Txt ("<body>\n");
	 Gbl.Layout.WritingHTMLStart = false;
	 Gbl.Layout.HTMLStartWritten =
	 Gbl.Layout.DivsEndWritten   = true;
	 return;
     }

   /***** Begin box that contains the whole page except the foot *****/
   HTM_DIV_Begin ("id=\"whole_page\"");

   /***** Header of layout *****/
   Lay_WritePageTopHeading ();

   /***** 3rd. row (tabs) *****/
   Tab_DrawTabs ();

   /***** 4th row: main zone *****/
   HTM_DIV_Begin ("id=\"main_zone\"");

   /* Left column */
   if (Gbl.Prefs.SideCols & Lay_SHOW_LEFT_COLUMN)		// Left column visible
     {
      HTM_Txt ("<aside id=\"left_col\">");
      Lay_ShowLeftColumn ();
      HTM_Txt ("</aside>");
     }

   /* Right column */
   // Right column is written before central column
   // but it must be drawn at right using "position:absolute; right:0".
   // The reason to write right column before central column
   // is that central column may hold a lot of content drawn slowly.
   if (Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN)	// Right column visible
     {
      HTM_Txt ("<aside id=\"right_col\">");
      Lay_ShowRightColumn ();
      HTM_Txt ("</aside>");
     }

   /* Central (main) column */
   switch (Gbl.Prefs.SideCols)
     {
      case 0:
         HTM_DIV_Begin ("id=\"main_zone_central_none\"");
	 break;
      case Lay_SHOW_LEFT_COLUMN:
         HTM_DIV_Begin ("id=\"main_zone_central_left\"");
	 break;
      case Lay_SHOW_RIGHT_COLUMN:
         HTM_DIV_Begin ("id=\"main_zone_central_right\"");
	 break;
      case (Lay_SHOW_LEFT_COLUMN | Lay_SHOW_RIGHT_COLUMN):
         HTM_DIV_Begin ("id=\"main_zone_central_both\"");
         break;
     }
   HTM_DIV_Begin ("id=\"main_zone_central_container\" class=\"%s\"",
	          The_TabOnBgColors[Gbl.Prefs.Theme]);

   /* Layout with horizontal or vertical menu */
   HTM_DIV_Begin ("id=\"%s\"",LayoutMainZone[Gbl.Prefs.Menu]);

   /* Menu */
   Mnu_WriteMenuThisTab ();

   /* Start canvas: main zone for actions output */
   HTM_MAIN_Begin ("MAIN_ZONE_CANVAS");

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
      HTM_MAIN_End ();	// Canvas (main zone to output content of the current action)
      HTM_DIV_End ();	// Layout with horizontal or vertical menu
      HTM_DIV_End ();	// main_zone_central_container

      /***** Write page footer *****/
      if (Act_GetBrowserTab (Gbl.Action.Act) == Act_BRW_1ST_TAB)
         Lay_WriteFootFromHTMLFile ();

      /***** End of main zone and page *****/
      HTM_DIV_End ();	// main_zone_central
      HTM_DIV_End ();	// main_zone
      HTM_DIV_End ();	// whole_page_* (box that contains the whole page except the foot)

      Gbl.Layout.DivsEndWritten = true;
     }
  }

/*****************************************************************************/
/************************* Write the title of the page ***********************/
/*****************************************************************************/

static void Lay_WritePageTitle (void)
  {
   extern const char *Txt_TAGLINE;

   HTM_Txt ("<title>");

   if (Gbl.Params.GetMethod && Gbl.Hierarchy.Deg.DegCod > 0)
     {
      HTM_TxtF ("%s &gt; %s",Cfg_PLATFORM_SHORT_NAME,Gbl.Hierarchy.Deg.ShrtName);
      if (Gbl.Hierarchy.Level == Hie_CRS)
         HTM_TxtF (" &gt; %s",Gbl.Hierarchy.Crs.ShrtName);
     }
   else
      HTM_TxtF ("%s:&nbsp;%s",Cfg_PLATFORM_SHORT_NAME,Txt_TAGLINE);

   HTM_Txt ("</title>\n");
  }

/*****************************************************************************/
/************* Write script and meta to redirect to my language **************/
/*****************************************************************************/

static void Lay_WriteRedirToMyLangOnLogIn (void)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   HTM_TxtF ("<meta http-equiv=\"refresh\""
	     " content=\"0; url='%s/%s?act=%ld&amp;ses=%s'\">",
	     Cfg_URL_SWAD_CGI,
	     Lan_STR_LANG_ID[Gbl.Usrs.Me.UsrDat.Prefs.Language],
	     Act_GetActCod (ActLogInLan),
	     Gbl.Session.Id);
  }

static void Lay_WriteRedirToMyLangOnViewUsrAgd (void)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   HTM_TxtF ("<meta http-equiv=\"refresh\""
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
   struct Hld_Holidays Holidays;
   struct ExaAnn_ExamAnnouncements ExamAnns;
   unsigned DayOfWeek; /* 0, 1, 2, 3, 4, 5, 6 */
   unsigned NumHld;
   unsigned NumExamAnnouncement;	// Number of exam announcement

   /***** General scripts for swad *****/
   HTM_SCRIPT_Begin (Cfg_URL_SWAD_PUBLIC "/" JS_FILE,NULL);
   HTM_SCRIPT_End ();

   /***** Script for MathJax *****/
   Lay_WriteScriptMathJax ();

   /***** Write script with init function executed after loading page *****/
   Lay_WriteScriptInit ();

   /***** Write script to set parameters needed by AJAX *****/
   Lay_WriteScriptParamsAJAX ();

   /***** Write script to initialize variables used to draw dates *****/
   HTM_SCRIPT_Begin (NULL,NULL);
   HTM_Txt ("\tvar DAYS = [");
   for (DayOfWeek = 0;
	DayOfWeek < 7;
	DayOfWeek++)
     {
      if (DayOfWeek)
	 HTM_Comma ();
      HTM_TxtF ("'%s'",Txt_DAYS_SMALL[DayOfWeek]);
     }
   HTM_Txt ("];\n");
   HTM_SCRIPT_End ();

   /***** Prepare script to draw months *****/
   if ((Gbl.Prefs.SideCols & Lay_SHOW_LEFT_COLUMN) ||	// Left column visible
       (Gbl.Hierarchy.Ins.InsCod > 0 &&			// Institution selected
        (Gbl.Action.Act == ActSeeCal ||			// Viewing calendar
         Gbl.Action.Act == ActPrnCal ||			// Printing calendar
         Gbl.Action.Act == ActChgCal1stDay)))		// Changing first day
     {
      /***** Reset places context *****/
      Hld_ResetHolidays (&Holidays);

      /***** Get list of holidays *****/
      Holidays.SelectedOrder = Hld_ORDER_BY_START_DATE;
      Hld_GetListHolidays (&Holidays);

      /***** Reset exam announcements context *****/
      ExaAnn_ResetExamAnns (&ExamAnns);

      /***** Create list of exam announcements *****/
      ExaAnn_CreateListExamAnns (&ExamAnns);

      /***** Write script to initialize variables used to draw months *****/
      HTM_SCRIPT_Begin (NULL,NULL);

      HTM_Txt ("\tvar DAYS_CAPS = [");
      for (DayOfWeek = 0;
	   DayOfWeek < 7;
	   DayOfWeek++)
	{
	 if (DayOfWeek)
	    HTM_Comma ();
	 HTM_TxtF ("'%c'",Txt_DAYS_CAPS[DayOfWeek][0]);
	}
      HTM_Txt ("];\n");

      HTM_Txt ("\tvar STR_EXAM = '");
      HTM_TxtF (Txt_Exam_of_X,Gbl.Hierarchy.Crs.FullName);
      HTM_Txt ("';\n");

      HTM_Txt ("\tvar Hlds = [];\n");

      for (NumHld = 0;
	   NumHld < Holidays.Num;
	   NumHld++)
	 HTM_TxtF ("\tHlds.push({ PlcCod: %ld, HldTyp: %u, StartDate: %s, EndDate: %s, Name: '%s' });\n",
		   Holidays.Lst[NumHld].PlcCod,
		   (unsigned) Holidays.Lst[NumHld].HldTyp,
		   Holidays.Lst[NumHld].StartDate.YYYYMMDD,
		   Holidays.Lst[NumHld].EndDate.YYYYMMDD,
		   Holidays.Lst[NumHld].Name);

      HTM_TxtF ("\tvar LstExamAnnouncements = [];\n");
      for (NumExamAnnouncement = 0;
	   NumExamAnnouncement < ExamAnns.NumExaAnns;
	   NumExamAnnouncement++)
	 HTM_TxtF ("LstExamAnnouncements.push({ ExaCod: %ld, Year: %u, Month: %u, Day: %u });\n",
		   ExamAnns.Lst[NumExamAnnouncement].ExaCod,
		   ExamAnns.Lst[NumExamAnnouncement].ExamDate.Year,
		   ExamAnns.Lst[NumExamAnnouncement].ExamDate.Month,
		   ExamAnns.Lst[NumExamAnnouncement].ExamDate.Day);

      HTM_SCRIPT_End ();

      /***** Free list of exam announcements *****/
      ExaAnn_FreeListExamAnns (&ExamAnns);

      /***** Free list of holidays *****/
      Hld_FreeListHolidays (&Holidays);
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
	 HTM_SCRIPT_Begin (Cfg_URL_SWAD_PUBLIC "/dropzone/dropzone.js","windows-1252");
	 HTM_SCRIPT_End ();
	 Lay_WriteScriptCustomDropzone ();
         break;
      case ActReqAccGbl:
      case ActSeeAccGbl:
      case ActReqAccCrs:
      case ActSeeAccCrs:
      case ActSeeAllStaCrs:
	 HTM_SCRIPT_Begin (Cfg_URL_SWAD_PUBLIC "/jstz/jstz.js",NULL);
	 HTM_SCRIPT_End ();
	 break;
      default:
	 break;
     }

   /***** Script for Google Analytics *****/
   HTM_Txt (Cfg_GOOGLE_ANALYTICS);
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
   /* MathJax 2.5.1 (obsolete) */
   /*
#ifdef Cfg_MATHJAX_LOCAL
   // Use the local copy of MathJax
   HTM_SCRIPT_Begin (Cfg_URL_SWAD_PUBLIC "/MathJax/MathJax.js?config=TeX-AMS-MML_HTMLorMML",NULL);
#else
   // Use the MathJax Content Delivery Network (CDN)
   HTM_SCRIPT_Begin ("//cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML",NULL);
#endif
   HTM_SCRIPT_End ();
   */

   /* MathJax 3.0.1 (march 2020)
   Source:
   http://docs.mathjax.org/en/latest/web/configuration.html
   */

   /* Configuration Using an In-Line Script */
   /*
   HTM_Txt ("<script type=\"text/x-mathjax-config\">\n"
	    "MathJax = {\n"
	    "  tex: {\n"
	    "   inlineMath: [['$','$'], ['\\\\(','\\\\)']]\n"
	    "  }\n"
	    "};\n"
	    "</script>");
   */

   /* Using a Local File for Configuration

   Using a Local File for Configuration

   If you are using the same MathJax configuration over multiple pages,
   you may find it convenient to store your configuration
   in a separate JavaScript file that you load into the page.
   For example, you could create a file called mathjax-config.js that contains

   window.MathJax = {
     tex: {
       inlineMath: [['$', '$'], ['\\(', '\\)']],
     },
     svg: {
       fontCache: 'global'
     }
   };

   and then use

   <script src="mathjax-config.js" defer></script>
   <script type="text/javascript" id="MathJax-script" defer
     src="https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-svg.js">
   </script>

   to first load your configuration file,
   and then load the tex-svg component from the jsdelivr CDN.

   Note that here we use the defer attribute on both scripts
   so that they will execute in order,
   but still not block the rest of the page
   while the files are being downloaded to the browser.
   If the async attribute were used,
   there is no guarantee that the configuration would run first,
   and so you could get instances
   where MathJax doesn't get properly configured,
   and they would seem to occur randomly.
   */
   HTM_TxtF ("<script src=\"%s/mathjax-config.js\" defer>\n"
	     "</script>\n",
	     Cfg_URL_SWAD_PUBLIC);

#ifdef Cfg_MATHJAX_LOCAL
   // Use the local copy of MathJax
   HTM_TxtF ("<script type=\"text/javascript\" id=\"MathJax-script\" defer"
	     " src=\"%s/mathjax/tex-chtml.js\">\n"
	     "</script>\n",
	     Cfg_URL_SWAD_PUBLIC);
#else
   // Use the MathJax Content Delivery Network (CDN)
   HTM_TxtF ("<script type=\"text/javascript\" id=\"MathJax-script\" defer"
             " src=\"https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-svg.js\">\n"
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
   bool RefreshLastClicks   = false;
   bool RefreshNewTimeline  = false;
   bool RefreshExamEventStd = false;
   bool RefreshExamEventTch = false;
   bool RefreshMatchStd     = false;
   bool RefreshMatchTch     = false;

   RefreshConnected = Act_GetBrowserTab (Gbl.Action.Act) == Act_BRW_1ST_TAB &&
	              (Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN);	// Right column visible

   switch (Gbl.Action.Act)
     {
      /* Last clicks */
      case ActLstClk:
	 RefreshLastClicks = true;
	 break;

      /* Timeline */
      case ActSeeSocTmlGbl:
      case ActRcvSocPstGbl:
      case ActRcvSocComGbl:
      case ActReqRemSocPubGbl:
      case ActRemSocPubGbl:
      case ActReqRemSocComGbl:
      case ActRemSocComGbl:
	 RefreshNewTimeline = true;
	 break;

      /* Exam event */
      case ActJoiExaEvt:
      case ActSeeExaEvtAnsQstStd:
      case ActRemExaEvtAnsQstStd:
      case ActAnsExaEvtQstStd:
	 RefreshExamEventStd = true;
	 break;
      case ActNewExaEvt:
      case ActResExaEvt:
      case ActBckExaEvt:
      case ActPlyPauExaEvt:
      case ActFwdExaEvt:
      case ActChgNumColExaEvt:
      case ActChgVisResExaEvtQst:
      case ActExaEvtCntDwn:
	 RefreshExamEventTch = true;
	 break;

      /* Match */
      case ActJoiMch:
      case ActSeeMchAnsQstStd:
      case ActRemMchAnsQstStd:
      case ActAnsMchQstStd:
	 RefreshMatchStd = true;
	 break;
      case ActNewMch:
      case ActResMch:
      case ActBckMch:
      case ActPlyPauMch:
      case ActFwdMch:
      case ActChgNumColMch:
      case ActChgVisResMchQst:
      case ActMchCntDwn:
	 RefreshMatchTch = true;
	 break;

      default:
	 break;
     }

   HTM_SCRIPT_Begin (NULL,NULL);

   Dat_WriteScriptMonths ();

   if (RefreshNewTimeline)		// Refresh new timeline via AJAX
      HTM_TxtF ("\tvar delayNewTL = %lu;\n",Cfg_TIME_TO_REFRESH_TIMELINE);
   else if (RefreshExamEventStd)	// Refresh exam event via AJAX
      HTM_TxtF ("\tvar delayExamEvent = %lu;\n",Cfg_TIME_TO_REFRESH_MATCH_STD);
   else if (RefreshExamEventTch)	// Refresh exam event via AJAX
      HTM_TxtF ("\tvar delayExamEvent = %lu;\n",Cfg_TIME_TO_REFRESH_MATCH_TCH);
   else if (RefreshMatchStd)		// Refresh match via AJAX
      HTM_TxtF ("\tvar delayMatch = %lu;\n",Cfg_TIME_TO_REFRESH_MATCH_STD);
   else if (RefreshMatchTch)		// Refresh match via AJAX
      HTM_TxtF ("\tvar delayMatch = %lu;\n",Cfg_TIME_TO_REFRESH_MATCH_TCH);

   /***** Function init () ******/
   HTM_Txt ("function init(){\n");

   HTM_TxtF ("\tActionAJAX = \"%s\";\n",Lan_STR_LANG_ID[Gbl.Prefs.Language]);

   if (RefreshConnected)	// Refresh connected users via AJAX
     {
      Con_WriteScriptClockConnected ();
      HTM_TxtF ("\tsetTimeout(\"refreshConnected()\",%lu);\n",
	        Gbl.Usrs.Connected.TimeToRefreshInMs);
     }

   if (RefreshLastClicks)		// Refresh last clicks via AJAX
      HTM_TxtF ("\tsetTimeout(\"refreshLastClicks()\",%lu);\n",
                Cfg_TIME_TO_REFRESH_LAST_CLICKS);
   else if (RefreshNewTimeline)		// Refresh timeline via AJAX
      HTM_Txt ("\tsetTimeout(\"refreshNewTL()\",delayNewTL);\n");
   else if (RefreshExamEventStd)	// Refresh exam event for a student via AJAX
      HTM_Txt ("\tsetTimeout(\"refreshExamEventStd()\",delayExamEvent);\n");
   else if (RefreshExamEventTch)	// Refresh exam event for a teacher via AJAX
      HTM_Txt ("\tsetTimeout(\"refreshExamEventTch()\",delayExamEvent);\n");
   else if (RefreshMatchStd)		// Refresh match for a student via AJAX
      HTM_Txt ("\tsetTimeout(\"refreshMatchStd()\",delayMatch);\n");
   else if (RefreshMatchTch)		// Refresh match for a teacher via AJAX
      HTM_Txt ("\tsetTimeout(\"refreshMatchTch()\",delayMatch);\n");

   HTM_Txt ("}\n");

   HTM_SCRIPT_End ();
  }

/*****************************************************************************/
/************** Write script to set parameters needed by AJAX ****************/
/*****************************************************************************/

static void Lay_WriteScriptParamsAJAX (void)
  {
   /***** Start script *****/
   HTM_SCRIPT_Begin (NULL,NULL);

   /***** Parameters with code of session and current course code *****/
   // Refresh parameters
   HTM_TxtF ("var RefreshParamIdSes = \"ses=%s\";\n"
             "var RefreshParamCrsCod = \"crs=%ld\";\n",
	     Gbl.Session.Id,
	     Gbl.Hierarchy.Crs.CrsCod);

   /***** Parameter to refresh connected users *****/
   if (Act_GetBrowserTab (Gbl.Action.Act) == Act_BRW_1ST_TAB)
      // Refresh parameter
      HTM_TxtF ("var RefreshParamNxtActCon = \"act=%ld\";\n",
	        Act_GetActCod (ActRefCon));

   /***** Parameters related with expanding/contracting folders in file browsers *****/
   if (Gbl.FileBrowser.Type != Brw_UNKNOWN)
      /* In all the actions related to file browsers ==>
	 put parameters used by AJAX */
      // Refresh parameters
      HTM_TxtF ("var RefreshParamExpand = \"act=%ld\";\n"
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
	 // Refresh parameters
	 HTM_TxtF ("var RefreshParamNxtActNewPub = \"act=%ld\";\n"
		   "var RefreshParamNxtActOldPub = \"act=%ld\";\n"
	           "var RefreshParamWho = \"Who=%u\";\n",
		   Act_GetActCod (ActRefNewSocPubGbl),
		   Act_GetActCod (ActRefOldSocPubGbl),
		   (unsigned) TL_GetGlobalWho ());	// Global variable got in a priori function
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
	    Usr_GetParamOtherUsrCodEncrypted (&Gbl.Usrs.Other.UsrDat);
	 // Refresh parameters
	 HTM_TxtF ("var RefreshParamNxtActOldPub = \"act=%ld\";\n"
		   "var RefreshParamUsr = \"OtherUsrCod=%s\";\n",
		   Act_GetActCod (ActRefOldSocPubUsr),
		   Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
	 break;
      /* Parameters related with exam event refreshing (for students) */
      case ActJoiExaEvt:
      case ActSeeExaEvtAnsQstStd:
      case ActRemExaEvtAnsQstStd:
      case ActAnsExaEvtQstStd:
	 // Refresh parameters
	 HTM_TxtF ("var RefreshParamNxtActExaEvt = \"act=%ld\";\n"
	           "var RefreshParamExaEvtCod = \"EvtCod=%ld\";\n",
		   Act_GetActCod (ActRefExaEvtStd),
		   ExaEvt_GetEvtCodBeingPlayed ());
	 break;
      /* Parameters related with exam event refreshing (for teachers) */
      case ActNewExaEvt:
      case ActResExaEvt:
      case ActBckExaEvt:
      case ActPlyPauExaEvt:
      case ActFwdExaEvt:
      case ActChgNumColExaEvt:
      case ActChgVisResExaEvtQst:
      case ActExaEvtCntDwn:
	 // Handle keys in keyboard/presenter
	 HTM_Txt ("document.addEventListener(\"keydown\",handleMatchKeys);\n");
	 // Refresh parameters
	 HTM_TxtF ("var RefreshParamNxtActExaEvt = \"act=%ld\";\n"
		   "var RefreshParamExaEvtCod = \"EvtCod=%ld\";\n",
		   Act_GetActCod (ActRefExaEvtTch),
		   ExaEvt_GetEvtCodBeingPlayed ());
	 break;
      /* Parameters related with match refreshing (for students) */
      case ActJoiMch:
      case ActSeeMchAnsQstStd:
      case ActRemMchAnsQstStd:
      case ActAnsMchQstStd:
	 // Refresh parameters
	 HTM_TxtF ("var RefreshParamNxtActMch = \"act=%ld\";\n"
	           "var RefreshParamMchCod = \"MchCod=%ld\";\n",
		   Act_GetActCod (ActRefMchStd),
		   Mch_GetMchCodBeingPlayed ());
	 break;
      /* Parameters related with match refreshing (for teachers) */
      case ActNewMch:
      case ActResMch:
      case ActBckMch:
      case ActPlyPauMch:
      case ActFwdMch:
      case ActChgNumColMch:
      case ActChgVisResMchQst:
      case ActMchCntDwn:
	 // Handle keys in keyboard/presenter
	 HTM_Txt ("document.addEventListener(\"keydown\",handleMatchKeys);\n");
	 // Refresh parameters
	 HTM_TxtF ("var RefreshParamNxtActMch = \"act=%ld\";\n"
		   "var RefreshParamMchCod = \"MchCod=%ld\";\n",
		   Act_GetActCod (ActRefMchTch),
		   Mch_GetMchCodBeingPlayed ());
	 break;
      /* Parameter related with clicks refreshing */
      case ActLstClk:
	 // Refresh parameter
	 HTM_TxtF ("var RefreshParamNxtActLstClk = \"act=%ld\";\n",
		   Act_GetActCod (ActRefLstClk));
	 break;
      default:
	 break;
     }

   /***** End script *****/
   HTM_SCRIPT_End ();
  }

/*****************************************************************************/
/******* Write script to customize upload of files using Dropzone.js *********/
/*****************************************************************************/
// More info: http://www.dropzonejs.com/

static void Lay_WriteScriptCustomDropzone (void)
  {
   // "myAwesomeDropzone" is the camelized version of the HTML element's ID
   // Add a line "forceFallback: true,\n" to test classic upload
   HTM_SCRIPT_Begin (NULL,NULL);
   HTM_TxtF ("Dropzone.options.myAwesomeDropzone = {\n"
	     "maxFiles: 100,\n"
	     "parallelUploads: 100,\n"
	     "maxFilesize: %lu,\n"
	     "fallback: function() {\n"
	     "document.getElementById('dropzone-upload').style.display='none';\n"
	     "document.getElementById('classic-upload').style.display='block';\n"
	     "}\n"
	     "};\n",
             (unsigned long) (Fil_MAX_FILE_SIZE / (1024ULL * 1024ULL) - 1));
   HTM_SCRIPT_End ();
  }

/*****************************************************************************/
/************************ Write top heading of the page **********************/
/*****************************************************************************/

static void Lay_WritePageTopHeading (void)
  {
   extern const char *The_ClassTagline[The_NUM_THEMES];
   extern const char *Txt_TAGLINE;
   extern const char *Txt_TAGLINE_BR;
   static const char *ClassHeadRow1[The_NUM_THEMES] =
     {
      [The_THEME_WHITE ] = "HEAD_ROW_1_WHITE",
      [The_THEME_GREY  ] = "HEAD_ROW_1_GREY",
      [The_THEME_PURPLE] = "HEAD_ROW_1_PURPLE",
      [The_THEME_BLUE  ] = "HEAD_ROW_1_BLUE",
      [The_THEME_YELLOW] = "HEAD_ROW_1_YELLOW",
      [The_THEME_PINK  ] = "HEAD_ROW_1_PINK",
     };
   static const char *ClassHeadRow2[The_NUM_THEMES] =
     {
      [The_THEME_WHITE ] = "HEAD_ROW_2_WHITE",
      [The_THEME_GREY  ] = "HEAD_ROW_2_GREY",
      [The_THEME_PURPLE] = "HEAD_ROW_2_PURPLE",
      [The_THEME_BLUE  ] = "HEAD_ROW_2_BLUE",
      [The_THEME_YELLOW] = "HEAD_ROW_2_YELLOW",
      [The_THEME_PINK  ] = "HEAD_ROW_2_PINK",
      };

   /***** Start header *****/
   HTM_Txt ("<header>");

   /***** 1st. row *****/
   /* Start 1st. row */
   HTM_DIV_Begin ("id=\"head_row_1\" class=\"%s\"",ClassHeadRow1[Gbl.Prefs.Theme]);

   /* 1st. row, 1st. column: logo, tagline and search */
   HTM_DIV_Begin ("id=\"head_row_1_left\"");

   /* Begin form to go to home page */
   Frm_StartFormGoTo (ActMnu);
   Par_PutHiddenParamUnsigned (NULL,"NxtTab",(unsigned) TabSys);

   HTM_DIV_Begin ("id=\"head_row_1_logo_small\"");
   HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,Cfg_PLATFORM_LOGO_SMALL_FILENAME,
		    Cfg_PLATFORM_SHORT_NAME,Cfg_PLATFORM_LOGO_SMALL_CLASS);
   HTM_DIV_End ();	// head_row_1_logo_small

   HTM_DIV_Begin ("id=\"head_row_1_logo_big\"");
   HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,Cfg_PLATFORM_LOGO_BIG_FILENAME,
		    Cfg_PLATFORM_SHORT_NAME,Cfg_PLATFORM_LOGO_BIG_CLASS);
   HTM_DIV_End ();	// head_row_1_logo_big

   HTM_DIV_Begin ("id=\"head_row_1_tagline\"");
   HTM_BUTTON_SUBMIT_Begin (Txt_TAGLINE,The_ClassTagline[Gbl.Prefs.Theme],NULL);
   HTM_Txt (Txt_TAGLINE_BR);
   HTM_BUTTON_End ();
   HTM_DIV_End ();	// head_row_1_tagline

   /* End form to go to home page */
   Frm_EndForm ();

   Sch_PutFormToSearchInPageTopHeading ();

   HTM_DIV_End ();	// head_row_1_left

   /* 1st. row, 3rd. column: logged user or language selection,
      and link to open/close session */
   HTM_DIV_Begin ("id=\"head_row_1_right\"");
   if (Gbl.Usrs.Me.Logged)
      Usr_WriteLoggedUsrHead ();
   else
      Lan_PutSelectorToSelectLanguage ();

   HTM_DIV_Begin ("id=\"login_box\"");
   if (Gbl.Usrs.Me.Logged)
      Usr_PutFormLogOut ();
   else
      Usr_PutFormLogIn ();
   HTM_DIV_End ();	// login_box

   HTM_DIV_End ();	// head_row_1_right

   /* End 1st. row */
   HTM_DIV_End ();	// head_row_1

   /***** 2nd. row *****/
   /* Start second row */
   HTM_DIV_Begin ("id=\"head_row_2\" class=\"%s\"",ClassHeadRow2[Gbl.Prefs.Theme]);

   /* 2nd. row, 1st. column
      Clock with hour:minute (server hour is shown) */
   HTM_DIV_Begin ("id=\"head_row_2_time\"");
   Dat_ShowClientLocalTime ();
   HTM_DIV_End ();	// End first column

   /* 2nd. row, 2nd. column: degree and course */
   HTM_DIV_Begin ("id=\"head_row_2_hierarchy\"");
   Lay_WriteBreadcrumb ();
   Hie_WriteBigNameCtyInsCtrDegCrs ();
   HTM_DIV_End ();

   /* 2nd. row, 3rd. column */
   HTM_DIV_Begin ("id=\"msg\"");	// Used for AJAX based refresh
   if (Gbl.Usrs.Me.Logged)
      Ntf_WriteNumberOfNewNtfs ();
   HTM_DIV_End ();		// Used for AJAX based refresh

   /* End 2nd. row */
   HTM_DIV_End ();

   /***** End header *****/
   HTM_Txt ("</header>");
  }

/*****************************************************************************/
/*********** Write breadcrumb with the path to the current location **********/
/*****************************************************************************/

static void Lay_WriteBreadcrumb (void)
  {
   HTM_Txt ("<nav id=\"breadcrumb\">");
   Crs_PutIconToSelectMyCoursesInBreadcrumb ();
   Hie_WriteHierarchyInBreadcrumb ();
   Crs_WriteSelectorMyCoursesInBreadcrumb ();
   HTM_Txt ("</nav>");
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
   HTM_DIV_Begin ("id=\"action_title\" style=\"background-image:url('%s/%s');\"",
	          Gbl.Prefs.URLIconSet,
	          Act_GetIcon (Act_GetSuperAction (Gbl.Action.Act)));

   /***** Title *****/
   HTM_DIV_Begin ("class=\"%s\"",The_ClassTitleAction[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s &gt; %s",Txt_TABS_TXT[Act_GetTab (Gbl.Action.Act)],
	                  Act_GetTitleAction (Gbl.Action.Act));
   HTM_DIV_End ();

   /***** Subtitle *****/
   HTM_DIV_Begin ("class=\"%s\"",The_ClassSubtitleAction[Gbl.Prefs.Theme]);
   HTM_Txt (Act_GetSubtitleAction (Gbl.Action.Act));
   HTM_DIV_End ();

   /***** Container end *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/***************************** Show left column ******************************/
/*****************************************************************************/

static void Lay_ShowLeftColumn (void)
  {
   struct MFU_ListMFUActions ListMFUActions;

   HTM_DIV_Begin ("style=\"width:160px;\"");

   HTM_DIV_Begin ("class=\"LEFT_RIGHT_CELL\"");
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
   HTM_DIV_End ();

   /***** Month *****/
   HTM_DIV_Begin ("class=\"LEFT_RIGHT_CELL\"");
   Cal_DrawCurrentMonth ();
   HTM_DIV_End ();

   /***** Notices (yellow notes) *****/
   if (Gbl.Hierarchy.Level == Hie_CRS)
     {
      HTM_DIV_Begin ("class=\"LEFT_RIGHT_CELL\"");
      Not_ShowNotices (Not_LIST_BRIEF_NOTICES,
	               -1L);	// No notice highlighted
      HTM_DIV_End ();
     }

   HTM_DIV_End ();
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
      HTM_DIV_Begin ("id=\"globalconnected\" class=\"LEFT_RIGHT_CELL\"");	// Used for AJAX based refresh
      Con_ShowGlobalConnectedUsrs ();
      HTM_DIV_End ();								// Used for AJAX based refresh
     }

   /***** Number of connected users in the current course *****/
   if (Gbl.Hierarchy.Level == Hie_CRS)	// There is a course selected
     {
      HTM_DIV_Begin ("id=\"courseconnected\" class=\"LEFT_RIGHT_CELL\"");	// Used for AJAX based refresh
      Gbl.Scope.Current = Hie_CRS;
      Con_ShowConnectedUsrsBelongingToCurrentCrs ();
      HTM_DIV_End ();								// Used for AJAX based refresh
     }
   else if (Gbl.Usrs.Me.Logged)		// I am logged
     {
      /*
      gettimeofday (&tv1,NULL);
      */
      /***** Suggest one user to follow *****/
      HTM_DIV_Begin ("class=\"LEFT_RIGHT_CELL\"");
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
      HTM_DIV_End ();
     }

   if (!Gbl.Usrs.Me.Logged)
     {
      /***** SWADroid advertisement *****/
      HTM_DIV_Begin ("class=\"LEFT_RIGHT_CELL\"");
      HTM_A_Begin ("href=\"https://play.google.com/store/apps/details?id=es.ugr.swad.swadroid\""
		   " target=\"_blank\" title=\"%s\"",
	           Txt_If_you_have_an_Android_device_try_SWADroid);
      HTM_IMG (Cfg_URL_ICON_PUBLIC,"SWADroid120x200.png","SWADroid",
	       "style=\"width:150px; height:250px;\"");
      HTM_A_End ();
      HTM_DIV_End ();
     }
  }

/*****************************************************************************/
/**************** Show an icon with a link in contextual menu ****************/
/*****************************************************************************/

void Lay_PutContextualLinkOnlyIcon (Act_Action_t NextAction,const char *Anchor,
				    void (*FuncParams) (void *Args),void *Args,
				    const char *Icon,
				    const char *Title)
  {
   /***** Begin form *****/
   Frm_StartFormAnchor (NextAction,Anchor);
   if (FuncParams)
      FuncParams (Args);

   /***** Put icon with link *****/
   Ico_PutIconLink (Icon,Title);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/**************** Show an icon with a link in contextual menu ****************/
/*****************************************************************************/

void Lay_PutContextualLinkIconText (Act_Action_t NextAction,const char *Anchor,
				    void (*FuncParams) (void *Args),void *Args,
				    const char *Icon,
				    const char *Text)
  {
   extern const char *The_ClassFormLinkOutBoxBold[The_NUM_THEMES];

   /***** Separator *****/
   HTM_Txt (" ");	// This space is necessary to enable
			// jumping to the next line on narrow screens

   /***** Begin form *****/
   Frm_StartFormAnchor (NextAction,Anchor);
   if (FuncParams)
      FuncParams (Args);

   /***** Put icon and text with link *****/
   HTM_BUTTON_SUBMIT_Begin (Text,The_ClassFormLinkOutBoxBold[Gbl.Prefs.Theme],NULL);
   Ico_PutIconTextLink (Icon,Text);
   HTM_BUTTON_End ();

   /***** End form *****/
   Frm_EndForm ();

   /***** Separator *****/
   HTM_Txt (" ");	// This space is necessary to enable
			// jumping to the next line on narrow screens
  }

/*****************************************************************************/
/**************** Show an icon with a link in contextual menu ****************/
/*****************************************************************************/

void Lay_PutContextualLinkIconTextOnSubmit (Act_Action_t NextAction,const char *Anchor,
					    void (*FuncParams) (const void *Args),const void *Args,
					    const char *Icon,
					    const char *Text,
					    const char *OnSubmit)
  {
   extern const char *The_ClassFormLinkOutBoxBold[The_NUM_THEMES];

   /***** Separator *****/
   HTM_Txt (" ");	// This space is necessary to enable
			// jumping to the next line on narrow screens

   /***** Begin form *****/
   Frm_StartFormAnchor (NextAction,Anchor);
   if (FuncParams)
      FuncParams (Args);

   /***** Put icon with link *****/
   HTM_BUTTON_SUBMIT_Begin (Text,The_ClassFormLinkOutBoxBold[Gbl.Prefs.Theme],OnSubmit);
   Ico_PutIconTextLink (Icon,Text);
   HTM_BUTTON_End ();

   /***** End form *****/
   Frm_EndForm ();

   /***** Separator *****/
   HTM_Txt (" ");	// This space is necessary to enable
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
      HTM_Txt (" ");	// This space is necessary to enable
			// jumping to the next line on narrow screens

   /***** Begin form *****/
   Frm_StartForm (NextAction);
   if (FuncParams)
      FuncParams ();

   /***** Start container and label *****/
   HTM_DIV_Begin ("class=\"CONTEXT_OPT %s %s\" title=\"%s\"",
		  Checked ? "CHECKBOX_CHECKED" :
			    "CHECKBOX_UNCHECKED",
		  The_ClassFormOutBoxBold[Gbl.Prefs.Theme],
		  Title);
   HTM_LABEL_Begin (NULL);

   /****** Checkbox *****/
   HTM_INPUT_CHECKBOX (CheckboxName,HTM_SUBMIT_ON_CHANGE,
		       "value=\"Y\"%s%s",
		       Checked ? " checked=\"checked\"" :
			         "",
		       Disabled ? " disabled=\"disabled\"" :
			          "");

   /***** Text *****/
   if (Text)
      if (Text[0])
	 HTM_TxtF ("&nbsp;%s",Text);

   /***** End label and container *****/
   HTM_LABEL_End ();
   HTM_DIV_End ();

   /***** End form *****/
   Frm_EndForm ();

   /***** Separator *****/
   if (Text)
      HTM_Txt (" ");	// This space is necessary to enable
			// jumping to the next line on narrow screens
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
/************* Write error message and exit when wrong order *****************/
/*****************************************************************************/

void Lay_WrongOrderExit (void)
  {
   Lay_ShowErrorAndExit ("Wrong order.");
  }

/*****************************************************************************/
/************* Write error message and exit when wrong order *****************/
/*****************************************************************************/

void Lay_WrongTypeOfViewExit (void)
  {
   Lay_ShowErrorAndExit ("Wrong type of view.");
  }

/*****************************************************************************/
/************* Write error message and exit when wrong project ***************/
/*****************************************************************************/

void Lay_WrongProjectExit (void)
  {
   Lay_ShowErrorAndExit ("Wrong project.");
  }

/*****************************************************************************/
/************** Write error message and exit when wrong exam *****************/
/*****************************************************************************/

void Lay_WrongExamExit (void)
  {
   Lay_ShowErrorAndExit ("Wrong exam.");
  }

/*****************************************************************************/
/********* Write error message and exit when wrong set of questions **********/
/*****************************************************************************/

void Lay_WrongSetExit (void)
  {
   Lay_ShowErrorAndExit ("Wrong set of questions.");
  }

/*****************************************************************************/
/************** Write error message and exit when wrong event ****************/
/*****************************************************************************/

void Lay_WrongEventExit (void)
  {
   Lay_ShowErrorAndExit ("Wrong event.");
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
      Gbl.TimeGenerationInMicroseconds = (long) Sta_ComputeTimeToGeneratePage ();

   if (Gbl.WebService.IsWebService)		// Serving a plugin request
     {
      /***** Log access *****/
      Gbl.TimeSendInMicroseconds = 0L;
      Log_LogAccess (Txt);
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
	 Log_LogAccess (Txt);

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

	    HTM_Txt ("</body>\n"
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
   unsigned i;

   /***** Start about zone *****/
   HTM_Txt ("<address id=\"about_zone\" class=\"ABOUT\">");

   /***** Institution and centre hosting the platform *****/
   HTM_A_Begin ("href=\"%s\" class=\"ABOUT\" target=\"_blank\"",Cfg_ABOUT_URL);
   HTM_IMG (Cfg_URL_ICON_PUBLIC,Cfg_ABOUT_LOGO,Cfg_ABOUT_NAME,
	    "style=\"width:%upx; height:%upx;\"",
	    Cfg_ABOUT_LOGO_WIDTH,Cfg_ABOUT_LOGO_HEIGHT);
   HTM_DIV_Begin (NULL);
   HTM_Txt (Cfg_ABOUT_NAME);
   HTM_DIV_End ();
   HTM_A_End ();

   /***** Questions and problems *****/
   HTM_DIV_Begin (NULL);
   HTM_TxtColonNBSP (Txt_Questions_and_problems);
   HTM_A_Begin ("href=\"mailto:%s\" class=\"ABOUT\" target=\"_blank\"",
	        Cfg_PLATFORM_RESPONSIBLE_EMAIL);
   HTM_Txt (Cfg_PLATFORM_RESPONSIBLE_EMAIL);
   HTM_A_End ();
   HTM_DIV_End ();

   /***** About and time to generate and send page *****/
   HTM_DIV_Begin (NULL);

   /* About */
   HTM_A_Begin ("href=\"%s\" class=\"ABOUT\" target=\"_blank\"",Cfg_ABOUT_SWAD_URL);
   HTM_TxtF (Txt_About_X,Log_PLATFORM_VERSION);
   HTM_A_End ();
   for (i = 0;
	i < 5;
	i++)
      HTM_NBSP ();

   /* Time to generate and send page */
   Sta_WriteTimeToGenerateAndSendPage ();

   HTM_DIV_End ();

   /***** End about zone *****/
   HTM_Txt ("</address>");	// about_zone
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
      Log_RemoveOldEntriesRecentLog ();		// Remove old entries in recent log table, it's a slow query
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
   HTM_TxtF ("%lu|",Gbl.Usrs.Connected.TimeToRefreshInMs);
   if (Gbl.Usrs.Me.Logged)
      Ntf_WriteNumberOfNewNtfs ();
   HTM_Txt ("|");
   Con_ShowGlobalConnectedUsrs ();
   HTM_Txt ("|");
   if (ShowConnected)
     {
      Gbl.Scope.Current = Hie_CRS;
      Con_ShowConnectedUsrsBelongingToCurrentCrs ();
     }
   HTM_Txt ("|");
   if (ShowConnected)
      HTM_Unsigned (Gbl.Usrs.Connected.NumUsrsToList);
   HTM_Txt ("|");
   if (ShowConnected)
      for (NumUsr = 0;
	   NumUsr < Gbl.Usrs.Connected.NumUsrsToList;
	   NumUsr++)
         HTM_TxtF ("%ld|",Gbl.Usrs.Connected.Lst[NumUsr].TimeDiff);
  }

/*****************************************************************************/
/**************** Refresh last clicks in realtime via AJAX *******************/
/*****************************************************************************/

void Lay_RefreshLastClicks (void)
  {
   if (Gbl.Session.IsOpen)	// If session has been closed, do not write anything
     {
      /***** Send, before the HTML, the refresh time *****/
      HTM_TxtF ("%lu|",Cfg_TIME_TO_REFRESH_LAST_CLICKS);

      /***** Get and show last clicks *****/
      Log_GetAndShowLastClicks ();
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
      HTM_Txt ("<footer id=\"foot_zone\">");

      /***** Copy HTML to output file *****/
      Fil_FastCopyOfOpenFiles (FileHTML,Gbl.F.Out);
      fclose (FileHTML);

      HTM_Txt ("</footer>");
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
   Ins_GetDataOfInstitutionByCod (&Ins);

   /***** Get data of degree *****/
   Deg.DegCod = DegCod;
   Deg_GetDataOfDegreeByCod (&Deg);

   /***** Get data of course *****/
   Crs.CrsCod = CrsCod;
   Crs_GetDataOfCourseByCod (&Crs);

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (10);
   HTM_TR_Begin (NULL);

   /***** First column: institution logo *****/
   HTM_TD_Begin ("class=\"LT\" style=\"width:60px;\"");
   if (InsCod > 0)
     {
      if (!PrintView)
         HTM_A_Begin ("href=\"%s\" target=\"_blank\"",Ins.WWW);
      Lgo_DrawLogo (Hie_INS,Ins.InsCod,Ins.ShrtName,40,NULL,true);
      if (!PrintView)
         HTM_A_End ();
     }
   HTM_TD_End ();

   /***** Second column: class photo title *****/
   HTM_TD_Begin ("class=\"CLASSPHOTO_TITLE CM\"");
   if (InsCod > 0)
     {
      if (!PrintView)
         HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"CLASSPHOTO_TITLE\"",
                      Ins.WWW);
      HTM_Txt (Ins.FullName);
      if (!PrintView)
         HTM_A_End ();
     }
   if (DegCod > 0)
     {
      if (Ins.InsCod > 0)
         HTM_Txt (" - ");
      if (!PrintView)
         HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"CLASSPHOTO_TITLE\"",
                      Deg.WWW);
      HTM_Txt (Deg.FullName);
      if (!PrintView)
         HTM_A_End ();
     }
   HTM_BR ();
   if (CrsCod > 0)
     {
      HTM_Txt (Crs.FullName);
      if (DrawingClassPhoto && !Gbl.Usrs.ClassPhoto.AllGroups)
        {
         HTM_BR ();
         Grp_WriteNamesOfSelectedGrps ();
        }
     }
   HTM_TD_End ();

   /***** Third column: degree logo *****/
   HTM_TD_Begin ("class=\"RT\" style=\"width:60px;\"");
   if (DegCod > 0)
     {
      if (!PrintView)
         HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"CLASSPHOTO_TITLE\"",
                      Deg.WWW);
      Lgo_DrawLogo (Hie_DEG,Deg.DegCod,Deg.ShrtName,40,NULL,true);
      if (!PrintView)
         HTM_A_End ();
     }
   HTM_TD_End ();

   /***** End table *****/
   HTM_TR_End ();
   HTM_TABLE_End ();
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
      HTM_DIV_Begin ("style=\"margin-top:25px;\"");

      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,NULL,
                         NULL,NULL,
                         NULL,Box_NOT_CLOSABLE,8);

      /***** Show advertisement *****/
      HTM_TR_Begin (NULL);
      HTM_TD_Begin ("class=\"DAT CM\"");
      HTM_A_Begin ("href=\"https://play.google.com/store/apps/details?id=es.ugr.swad.swadroid\""
	           " class=\"DAT\"");
      HTM_Txt (Txt_Stay_connected_with_SWADroid);
      HTM_BR ();
      HTM_BR ();
      HTM_IMG (Cfg_URL_ICON_PUBLIC,"SWADroid200x300.png",Txt_Stay_connected_with_SWADroid,
	       "style=\"width:250px; height:375px;\"");
      HTM_A_End ();
      HTM_TD_End ();
      HTM_TR_End ();

      /***** End table and box *****/
      Box_BoxTableEnd ();

      HTM_DIV_End ();
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
      Ico_PutIcon (IsLastItemInLevel[i] ? "tr20x20.gif" :
		                          "subleft20x20.gif",
		   "","ICO25x25");

   /***** Level *****/
   if (Level)
      Ico_PutIcon (IsLastItemInLevel[Level] ? "subend20x20.gif" :
					      "submid20x20.gif",
		   "","ICO25x25");
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

   HTM_DIV_Begin ("class=\"HELP_EDIT\"");
   HTM_TxtF ("%s: %s"
             " "
             "%s: <code>%s</code>"
             " "
             "%s: <code>%s</code>",
             Txt_Text,Text,
             Txt_Inline_math,InlineMath,
             Txt_Equation_centered,Equation);
   HTM_DIV_End ();
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
