// swad_report.c: report on my use of the platform

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <sys/stat.h>		// For mkdir
#include <sys/types.h>		// For mkdir

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_browser_database.h"
#include "swad_database.h"
#include "swad_enrolment_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_log_database.h"
#include "swad_profile.h"
#include "swad_report_database.h"
#include "swad_tab.h"

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Rep_FILENAME_ROOT "report"

#define Rep_MIN_CLICKS_CRS 100	// Minimum number of clicks to show a course in historic log
#define Rep_MAX_ACTIONS	    50  // Maximum number of actions in list of frequent actions
#define Rep_MAX_BAR_WIDTH 50	// Maximum width of graphic bar

// #define Rep_BLOCK "&boxH;"	// HTML code for a block in graphic bar
// #define Rep_BLOCK "&blk12;"	// HTML code for a block in graphic bar
// #define Rep_BLOCK "&block;"	// HTML code for a block in graphic bar
// #define Rep_BLOCK "&equiv;"	// HTML code for a block in graphic bar
// #define Rep_BLOCK "&bull;"	// HTML code for a block in graphic bar
// #define Rep_BLOCK "&squf;"	// HTML code for a block in graphic bar
#define Rep_BLOCK "-"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

static FILE *Rep_File;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Rep_CreateMyUsageReport (struct Rep_Report *Report);
static void Rep_PutLinkToMyUsageReport (struct Rep_Report *Report);
static void Rep_TitleReport (struct Rep_CurrentTimeUTC *CurrentTimeUTC);

static void Rep_GetCurrentDateTimeUTC (struct Rep_Report *Report);

static void Rep_CreateNewReportFile (struct Rep_Report *Report);
static void Rep_WriteHeader (const struct Rep_Report *Report);
static void Rep_WriteSectionPlatform (void);
static void Rep_WriteSectionUsrInfo (void);
static void Rep_WriteSectionUsrFigures (const struct Rep_Report *Report);
static void Rep_WriteSectionHitsPerAction (struct Rep_Report *Report);
static void Rep_WriteSectionGlobalHits (struct Rep_Report *Report);
static void Rep_WriteSectionCurrentCourses (struct Rep_Report *Report);
static void Rep_WriteSectionHistoricCourses (struct Rep_Report *Report);

static void Rep_GetMaxHitsPerYear (struct Rep_Report *Report);
static void Rep_GetAndWriteMyCurrentCrss (Rol_Role_t Role,
                                          struct Rep_Report *Report);
static void Rep_GetAndWriteMyHistoricClicsWithoutCrs (struct Rep_Report *Report);
static void Rep_GetAndWriteMyHistoricCrss (Rol_Role_t Role,
                                           struct Rep_Report *Report);
static void Rep_WriteRowCrsData (long CrsCod,Rol_Role_t Role,
                                 struct Rep_Report *Report,
                                 bool WriteNumUsrs);

static void Rep_ShowMyHitsPerYear (bool AnyCourse,long CrsCod,Rol_Role_t Role,
                                   struct Rep_Report *Report);
static void Rep_ComputeMaxAndTotalHits (struct Rep_Hits *Hits,
                                        unsigned NumHits,
                                        MYSQL_RES *mysql_res,unsigned Field);
static void Rep_DrawBarNumHits (unsigned long HitsNum,unsigned long HitsMax,
                                unsigned MaxBarWidth);

static void Rep_WriteDouble (double Num);

static void Rep_RemoveUsrReportsFiles (long UsrCod);

/*****************************************************************************/
/******* Request my usage report (report on my use of the platform) **********/
/*****************************************************************************/

void Rep_ReqMyUsageReport (void)
  {
   extern const char *Hlp_ANALYTICS_Report;
   extern const char *Txt_Report_of_use_of_PLATFORM;
   extern const char *Txt_Generate_report;
   char *Title;

   /***** Form to show my usage report *****/
   Frm_BeginForm (ActSeeMyUsgRep);

      /***** Begin box *****/
      if (asprintf (&Title,Txt_Report_of_use_of_PLATFORM,Cfg_PLATFORM_SHORT_NAME) < 0)
	 Err_NotEnoughMemoryExit ();
      Box_BoxBegin (Title,NULL,NULL,Hlp_ANALYTICS_Report,Box_NOT_CLOSABLE);
      free (Title);

	 /***** Header *****/
	 Rep_TitleReport (NULL);	// NULL means do not write date

      /***** Send button and end box *****/
      Box_BoxWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Generate_report);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/********* Show my usage report (report on my use of the platform) ***********/
/*****************************************************************************/

void Rep_ShowMyUsageReport (void)
  {
   struct Rep_Report Report;

   /***** Create my usage report *****/
   Rep_CreateMyUsageReport (&Report);

   /***** Put link to my usage report *****/
   Rep_PutLinkToMyUsageReport (&Report);
  }

/*****************************************************************************/
/******** Create my usage report (report on my use of the platform) **********/
/*****************************************************************************/

static void Rep_CreateMyUsageReport (struct Rep_Report *Report)
  {
   bool GetUsrFiguresAgain;

   /***** Get current date-time *****/
   Rep_GetCurrentDateTimeUTC (Report);

   /***** Create a new report file *****/
   Rep_CreateNewReportFile (Report);

   /***** Store report entry into database *****/
   Rep_DB_CreateNewReport (Gbl.Usrs.Me.UsrDat.UsrCod,Report,
                           Cry_GetUniqueNameEncrypted ());

   /***** Begin file *****/
   Lay_BeginHTMLFile (Rep_File,Report->FilenameReport);
   fprintf (Rep_File,"<body style=\"margin:1em;"
	             " text-align:left;"
	             " font-family:Helvetica,Arial,sans-serif;\">\n");

   /***** Header *****/
   Rep_WriteHeader (Report);

   /***** Platform *****/
   Rep_WriteSectionPlatform ();

   /***** Personal information *****/
   Rep_WriteSectionUsrInfo ();

   /***** Figures *****/
   Prf_GetUsrFigures (Gbl.Usrs.Me.UsrDat.UsrCod,&Report->UsrFigures);
   GetUsrFiguresAgain = Prf_GetAndStoreAllUsrFigures (Gbl.Usrs.Me.UsrDat.UsrCod,
                                                      &Report->UsrFigures);
   if (GetUsrFiguresAgain)
      Prf_GetUsrFigures (Gbl.Usrs.Me.UsrDat.UsrCod,&Report->UsrFigures);
   if (Report->UsrFigures.FirstClickTimeUTC)
      gmtime_r (&Report->UsrFigures.FirstClickTimeUTC,
                &Report->tm_FirstClickTime);
   Rep_WriteSectionUsrFigures (Report);

   /***** Global count of hits *****/
   Rep_WriteSectionGlobalHits (Report);

   /***** Global hits distributed by action *****/
   Rep_WriteSectionHitsPerAction (Report);

   /***** Current courses *****/
   Rep_GetMaxHitsPerYear (Report);
   Rep_WriteSectionCurrentCourses (Report);

   /***** Historic courses *****/
   Rep_WriteSectionHistoricCourses (Report);

   /***** End file *****/
   fprintf (Rep_File,"</body>\n"
	             "</html>\n");

   /***** Close report file *****/
   fclose (Rep_File);
  }

/*****************************************************************************/
/******* Put link to my usage report (report on my use of the platform) ******/
/*****************************************************************************/

static void Rep_PutLinkToMyUsageReport (struct Rep_Report *Report)
  {
   extern const char *Hlp_ANALYTICS_Report;
   extern const char *Txt_Report_of_use_of_PLATFORM;
   extern const char *Txt_Report;
   extern const char *Txt_This_link_will_remain_active_as_long_as_your_user_s_account_exists;
   char *Title;

   /***** Begin box *****/
   if (asprintf (&Title,Txt_Report_of_use_of_PLATFORM,Cfg_PLATFORM_SHORT_NAME) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin (Title,NULL,NULL,Hlp_ANALYTICS_Report,Box_NOT_CLOSABLE);
   free (Title);

      /***** Header *****/
      Rep_TitleReport (&Report->CurrentTimeUTC);

      /***** Put anchor and report filename *****/
      HTM_DIV_Begin ("class=\"FILENAME_BIG CM\"");
	 HTM_A_Begin ("href=\"%s\" class=\"FILENAME_BIG\" title=\"%s\" target=\"_blank\"",
		      Report->Permalink,
		      Txt_Report);
	    Ico_PutIcon ("file-alt.svg",Ico_BLACK,Txt_Report,"ICO64x64");
	    HTM_BR ();
	    HTM_Txt (Report->FilenameReport);
	 HTM_A_End ();
      HTM_DIV_End ();

      HTM_DIV_Begin ("class=\"DAT_LIGHT_%s\"",The_GetSuffix ());
	 HTM_Txt (Txt_This_link_will_remain_active_as_long_as_your_user_s_account_exists);
      HTM_DIV_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* Write title of user's usage report ********************/
/*****************************************************************************/
// CurrentTimeUTC == NULL ==> do not write date

static void Rep_TitleReport (struct Rep_CurrentTimeUTC *CurrentTimeUTC)
  {
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_Date;

   HTM_DIV_Begin ("class=\"TITLE_REPORT DAT_%s\"",
                  The_GetSuffix ());

      /***** User *****/
      HTM_TxtColonNBSP (Txt_User[Gbl.Usrs.Me.UsrDat.Sex]);
      HTM_SPAN_Begin ("class=\"DAT_STRONG_%s BOLD\"",
                      The_GetSuffix ());
	 HTM_Txt (Gbl.Usrs.Me.UsrDat.FullName);
      HTM_SPAN_End ();

      /***** Report date *****/
      if (CurrentTimeUTC)
	{
	 HTM_BR ();
	 HTM_TxtColonNBSP (Txt_Date);
	 HTM_SPAN_Begin ("class=\"DAT_STRONG_%s\"",
	                 The_GetSuffix ());
	    HTM_TxtF ("%s %s UTC",CurrentTimeUTC->StrDate,
				  CurrentTimeUTC->StrTime);
	 HTM_SPAN_End ();
	}

   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Get current date and time in UTC **********************/
/*****************************************************************************/

static void Rep_GetCurrentDateTimeUTC (struct Rep_Report *Report)
  {
   time_t CurrentTime;

   /***** Initialize to empty strings *****/
   Report->CurrentTimeUTC.StrDate[0] = '\0';
   Report->CurrentTimeUTC.StrTime[0] = '\0';

   /***** Get current time UTC *****/
   time (&CurrentTime);
   if ((gmtime_r (&CurrentTime,&Report->tm_CurrentTime)) != NULL)
     {
      /* Date and time as strings */
      snprintf (Report->CurrentTimeUTC.StrDate,
	        sizeof (Report->CurrentTimeUTC.StrDate),"%04d-%02d-%02d",
	        1900 + Report->tm_CurrentTime.tm_year,	// year
	        1 + Report->tm_CurrentTime.tm_mon,	// month
	        Report->tm_CurrentTime.tm_mday);		// day of the month
      snprintf (Report->CurrentTimeUTC.StrTime,
	        sizeof (Report->CurrentTimeUTC.StrTime),"%02d:%02d:%02d",
	        Report->tm_CurrentTime.tm_hour,		// hours
	        Report->tm_CurrentTime.tm_min,		// minutes
	        Report->tm_CurrentTime.tm_sec);		// seconds

      /* Date and time as unsigned */
      Report->CurrentTimeUTC.Date = (1900 + Report->tm_CurrentTime.tm_year) * 10000 +
	                               (1 + Report->tm_CurrentTime.tm_mon)  * 100   +
	                                    Report->tm_CurrentTime.tm_mday;
      Report->CurrentTimeUTC.Time = Report->tm_CurrentTime.tm_hour          * 10000 +
	                            Report->tm_CurrentTime.tm_min           * 100   +
	                            Report->tm_CurrentTime.tm_sec;
     }
  }

/*****************************************************************************/
/*************** Create a new file for user's usage report *******************/
/*****************************************************************************/

static void Rep_CreateNewReportFile (struct Rep_Report *Report)
  {
   char PathUniqueDirL[PATH_MAX + 1];
   char PathUniqueDirR[PATH_MAX + 1 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1];
   char PathFileReport[PATH_MAX + 1 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1 + NAME_MAX + 1];
   const char *UniqueNameEncrypted = Cry_GetUniqueNameEncrypted ();
   char Permalink[128 +
		  Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 +
		  NAME_MAX];

   /***** Path for reports *****/
   Fil_CreateDirIfNotExists (Cfg_PATH_REP_PUBLIC);

   /***** Unique directory for the file with the report *****/
   /* 1. Create a directory using the leftmost 2 chars of a unique name */
   snprintf (PathUniqueDirL,sizeof (PathUniqueDirL),"%s/%c%c",
             Cfg_PATH_REP_PUBLIC,
             UniqueNameEncrypted[0],
             UniqueNameEncrypted[1]);
   Fil_CreateDirIfNotExists (PathUniqueDirL);

   /* 2. Create a directory using the rightmost 41 chars of a unique name */
   snprintf (PathUniqueDirR,sizeof (PathUniqueDirR),"%s/%s",
             PathUniqueDirL,
             &UniqueNameEncrypted[2]);
   if (mkdir (PathUniqueDirR,(mode_t) 0xFFF))
      Err_ShowErrorAndExit ("Can not create directory for report.");

   /***** Path of the public file with the report */
   snprintf (Report->FilenameReport,sizeof (Report->FilenameReport),
	     "%s_%06u_%06u.html",
             Rep_FILENAME_ROOT,Report->CurrentTimeUTC.Date,Report->CurrentTimeUTC.Time);
   snprintf (PathFileReport,sizeof (PathFileReport),"%s/%s",
             PathUniqueDirR,Report->FilenameReport);
   if ((Rep_File = fopen (PathFileReport,"wb")) == NULL)
      Err_ShowErrorAndExit ("Can not create report file.");

   /***** Permalink *****/
   snprintf (Permalink,sizeof (Permalink),"%s/%c%c/%s/%s",
             Cfg_URL_REP_PUBLIC,
             UniqueNameEncrypted[0],
             UniqueNameEncrypted[1],
             &UniqueNameEncrypted[2],
             Report->FilenameReport);
   Str_Copy (Report->Permalink,Permalink,sizeof (Report->Permalink) - 1);
  }

/*****************************************************************************/
/******************** Write header of user's usage report ********************/
/*****************************************************************************/

static void Rep_WriteHeader (const struct Rep_Report *Report)
  {
   extern const char *Txt_Report_of_use_of_PLATFORM;
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_Date;
   extern const char *Txt_Permalink;

   /***** Begin header *****/
   fprintf (Rep_File,"<header>"
                     "<h1>");
      fprintf (Rep_File,Txt_Report_of_use_of_PLATFORM,Cfg_PLATFORM_SHORT_NAME);
   fprintf (Rep_File,"</h1>"
                     "<ul>");

   /***** User *****/
   fprintf (Rep_File,"<li>%s: <strong>%s</strong></li>",
	    Txt_User[Gbl.Usrs.Me.UsrDat.Sex],
	    Gbl.Usrs.Me.UsrDat.FullName);

   /***** Date-time *****/
   fprintf (Rep_File,"<li>%s: %s %s UTC</li>",
            Txt_Date,
	    Report->CurrentTimeUTC.StrDate,
	    Report->CurrentTimeUTC.StrTime);

   /***** Permalink *****/
   fprintf (Rep_File,"<li>%s: "
	             "<a href=\"%s\" target=\"_blank\""
		     " style=\"text-decoration:none;\">"
	             "%s"
	             "</a>"
	             "</li>",
            Txt_Permalink,
            Report->Permalink,Report->Permalink);

   /***** End header *****/
   fprintf (Rep_File,"</ul>"
	             "</header>\n");
  }

/*****************************************************************************/
/************* Write section for platform in user's usage report *************/
/*****************************************************************************/

static void Rep_WriteSectionPlatform (void)
  {
   extern const char *Txt_Teaching_platform;
   extern const char *Txt_Name;
   extern const char *Txt_TAGLINE;
   extern const char *Txt_URL;

   /***** Begin section *****/
   fprintf (Rep_File,"<section>"
	             "<h3>%s</h3>",
	    Txt_Teaching_platform);
   fprintf (Rep_File,"<ul>");

   /***** Platform name *****/
   fprintf (Rep_File,"<li>%s: %s, %s</li>",
            Txt_Name,
            Cfg_PLATFORM_FULL_NAME,Txt_TAGLINE);

   /***** Server URL *****/
   fprintf (Rep_File,"<li>%s: "
	             "<a href=\"%s\" target=\"_blank\""
		     " style=\"text-decoration:none;\">"
	             "%s"
	             "</a>"
	             "</li>",
            Txt_URL,Cfg_URL_SWAD_SERVER,Cfg_URL_SWAD_SERVER);

   /***** End section *****/
   fprintf (Rep_File,"</ul>"
	             "</section>\n");
  }

/*****************************************************************************/
/*********** Write section for user's info in user's usage report ************/
/*****************************************************************************/

static void Rep_WriteSectionUsrInfo (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_Personal_information;
   extern const char *Txt_Name;
   extern const char *Txt_Email;
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   char CtyName[Nam_MAX_BYTES_FULL_NAME + 1];
   struct Hie_Node Ins;

   /***** Begin section *****/
   fprintf (Rep_File,"<section>"
	             "<h3>%s</h3>",
	    Txt_Personal_information);
   fprintf (Rep_File,"<ul>");

   /***** User's name *****/
   fprintf (Rep_File,"<li>%s: %s</li>",
            Txt_Name,
            Gbl.Usrs.Me.UsrDat.FullName);

   /***** User's email *****/
   fprintf (Rep_File,"<li>%s: %s</li>",
            Txt_Email,
            Gbl.Usrs.Me.UsrDat.Email);

   /***** User's country *****/
   Cty_GetCountryNameInLanguage (Gbl.Usrs.Me.UsrDat.CtyCod,Gbl.Prefs.Language,CtyName);
   fprintf (Rep_File,"<li>%s: %s</li>",
            Txt_HIERARCHY_SINGUL_Abc[Hie_CTY],CtyName);

   /***** User's institution *****/
   Ins.HieCod = Gbl.Usrs.Me.UsrDat.InsCod;
   Hie_GetDataByCod[Hie_INS] (&Ins);
   fprintf (Rep_File,"<li>%s: %s</li>",
            Txt_HIERARCHY_SINGUL_Abc[Hie_INS],Ins.FullName);

   /***** End section *****/
   fprintf (Rep_File,"</ul>"
	             "</section>\n");
  }

/*****************************************************************************/
/********* Write section for user's figures in user's usage report ***********/
/*****************************************************************************/

static void Rep_WriteSectionUsrFigures (const struct Rep_Report *Report)
  {
   extern const char *Txt_Figures;
   extern const char *Txt_TIME_Since;
   extern const char *Txt_TIME_until;
   extern const char *Txt_day;
   extern const char *Txt_days;
   extern const char *Txt_Clicks;
   extern const char *Txt_Files_uploaded;
   extern const char *Txt_file;
   extern const char *Txt_files;
   extern const char *Txt_public_FILES;
   extern const char *Txt_Downloads;
   extern const char *Txt_download;
   extern const char *Txt_downloads;
   extern const char *Txt_Forum_posts;
   extern const char *Txt_FORUM_post;
   extern const char *Txt_FORUM_posts;
   extern const char *Txt_Messages_sent;
   extern const char *Txt_message;
   extern const char *Txt_messages;
   unsigned NumFiles;
   unsigned NumPublicFiles;

   /***** Begin section *****/
   fprintf (Rep_File,"<section>"
                     "<h3>%s</h3>",
	    Txt_Figures);
   fprintf (Rep_File,"<ul>");

   /***** Time since first click until now *****/
   fprintf (Rep_File,"<li>%s ",Txt_TIME_Since);
   if (Report->UsrFigures.FirstClickTimeUTC)
     {
      fprintf (Rep_File,"%04d-%02d-%02d %02d:%02d:%02d UTC",
	       1900 + Report->tm_FirstClickTime.tm_year,	// year
	       1 + Report->tm_FirstClickTime.tm_mon,		// month
	       Report->tm_FirstClickTime.tm_mday,		// day of the month
	       Report->tm_FirstClickTime.tm_hour,		// hours
	       Report->tm_FirstClickTime.tm_min,		// minutes
	       Report->tm_FirstClickTime.tm_sec);		// seconds
      if (Report->CurrentTimeUTC.StrDate[0])
	 fprintf (Rep_File," %s %s %s UTC",
		  Txt_TIME_until,
		  Report->CurrentTimeUTC.StrDate,
		  Report->CurrentTimeUTC.StrTime);
      if (Report->UsrFigures.NumDays > 0)
	 fprintf (Rep_File," (%d %s)",
		  Report->UsrFigures.NumDays,
		  (Report->UsrFigures.NumDays == 1) ? Txt_day :
						      Txt_days);
     }
   else	// Time of first click is unknown
     {
      fprintf (Rep_File,"?");
      if (Report->CurrentTimeUTC.StrDate[0])
         fprintf (Rep_File," - %s %s UTC",
                  Report->CurrentTimeUTC.StrDate,
                  Report->CurrentTimeUTC.StrTime);
     }
   fprintf (Rep_File,"</li>");

   /***** Number of clicks *****/
   fprintf (Rep_File,"<li>%s: ",Txt_Clicks);
   if (Report->UsrFigures.NumClicks >= 0)
     {
      fprintf (Rep_File,"%d",Report->UsrFigures.NumClicks);
      if (Report->UsrFigures.NumDays > 0)
	{
	 fprintf (Rep_File," (");
	 Rep_WriteDouble ((double) Report->UsrFigures.NumClicks /
			  (double) Report->UsrFigures.NumDays);
	 fprintf (Rep_File," / %s)",Txt_day);
	}
     }
   else	// Number of clicks is unknown
      fprintf (Rep_File,"?");
   fprintf (Rep_File,"</li>");

   /***** Number of files currently published *****/
   if ((NumFiles = Brw_DB_GetNumFilesUsr (Gbl.Usrs.Me.UsrDat.UsrCod)))
      NumPublicFiles = Brw_DB_GetNumPublicFilesUsr (Gbl.Usrs.Me.UsrDat.UsrCod);
   else
      NumPublicFiles = 0;
   fprintf (Rep_File,"<li>"
		     "%s: %u %s (%u %s)"
		     "</li>",
	    Txt_Files_uploaded,
	    NumFiles,
	    (NumFiles == 1) ? Txt_file :
		              Txt_files,
	    NumPublicFiles,Txt_public_FILES);

   /***** Number of file views *****/
   fprintf (Rep_File,"<li>%s: ",Txt_Downloads);
   if (Report->UsrFigures.NumFileViews >= 0)
     {
      fprintf (Rep_File,"%d %s",
               Report->UsrFigures.NumFileViews,
	       (Report->UsrFigures.NumFileViews == 1) ? Txt_download :
						        Txt_downloads);
      if (Report->UsrFigures.NumDays > 0)
	{
	 fprintf (Rep_File," (");
	 Rep_WriteDouble ((double) Report->UsrFigures.NumFileViews /
			  (double) Report->UsrFigures.NumDays);
	 fprintf (Rep_File," / %s)",Txt_day);
	}
     }
   else	// Number of file views is unknown
      fprintf (Rep_File,"?");
   fprintf (Rep_File,"</li>");

   /***** Number of posts in forums *****/
   fprintf (Rep_File,"<li>%s: ",Txt_Forum_posts);
   if (Report->UsrFigures.NumForumPosts >= 0)
     {
      fprintf (Rep_File,"%d %s",
	       Report->UsrFigures.NumForumPosts,
	       (Report->UsrFigures.NumForumPosts == 1) ? Txt_FORUM_post :
					             Txt_FORUM_posts);
      if (Report->UsrFigures.NumDays > 0)
	{
	 fprintf (Rep_File," (");
	 Rep_WriteDouble ((double) Report->UsrFigures.NumForumPosts /
			  (double) Report->UsrFigures.NumDays);
	 fprintf (Rep_File," / %s)",Txt_day);
	}
     }
   else	// Number of forum posts is unknown
      fprintf (Rep_File,"?");
   fprintf (Rep_File,"</li>");

   /***** Number of messages sent *****/
   fprintf (Rep_File,"<li>%s: ",Txt_Messages_sent);
   if (Report->UsrFigures.NumMessagesSent >= 0)
     {
      fprintf (Rep_File,"%d %s",
	       Report->UsrFigures.NumMessagesSent,
	       (Report->UsrFigures.NumMessagesSent == 1) ? Txt_message :
					             Txt_messages);
      if (Report->UsrFigures.NumDays > 0)
	{
	 fprintf (Rep_File," (");
	 Rep_WriteDouble ((double) Report->UsrFigures.NumMessagesSent /
			  (double) Report->UsrFigures.NumDays);
	 fprintf (Rep_File," / %s)",Txt_day);
	}
     }
   else	// Number of messages sent is unknown
      fprintf (Rep_File,"?");
   fprintf (Rep_File,"</li>");

   /***** End section *****/
   fprintf (Rep_File,"</ul>"
	             "</section>\n");
  }

/*****************************************************************************/
/******** Write section for user's global hits in user's usage report ********/
/*****************************************************************************/

static void Rep_WriteSectionGlobalHits (struct Rep_Report *Report)
  {
   extern const char *Txt_Hits_per_year;

   /***** Begin section *****/
   fprintf (Rep_File,"<section>"
                     "<h3>%s</h3>",
	    Txt_Hits_per_year);

   /***** Global (in any course) hits per year *****/
   Report->MaxHitsPerYear = 0;	// MaxHitsPerYear not passed as an argument but computed inside the function
   Rep_ShowMyHitsPerYear (true,-1L,	// Any course
                          Rol_UNK,	// Any role
                          Report);

   /***** End section *****/
   fprintf (Rep_File,"</section>\n");
  }

/*****************************************************************************/
/******** Write section for user's global hits in user's usage report ********/
/*****************************************************************************/

static void Rep_WriteSectionHitsPerAction (struct Rep_Report *Report)
  {
   extern const char *Txt_Hits_per_action;
   extern const char *Txt_TABS_TXT[Tab_NUM_TABS];
   extern const char *Txt_Other_actions;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumHits;
   unsigned NumHit;
   long ActCod;
   Act_Action_t Action;
   Tab_Tab_t Tab;
   unsigned long NumClicks;

   /***** Begin section *****/
   fprintf (Rep_File,"<section>"
                     "<h3>%s</h3>",
	    Txt_Hits_per_action);

   /***** Make the query *****/
   NumHits = Log_DB_GetMyClicksGroupedByAction (&mysql_res,
					        Report->UsrFigures.FirstClickTimeUTC,
					        Rep_MAX_ACTIONS);

   /***** Compute maximum number of hits per action *****/
   Rep_ComputeMaxAndTotalHits (&Report->Hits,NumHits,mysql_res,1);
   mysql_data_seek (mysql_res,0);

   /***** Write rows *****/
   for (NumHit  = 1, NumClicks = 0;
	NumHit <= NumHits;
	NumHit++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get the action (row[0]) */
      ActCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get number of hits (row[1]) */
      if (sscanf (row[1],"%lu",&Report->Hits.Num) != 1)
	 Report->Hits.Num = 0;
      NumClicks += Report->Hits.Num;

      /* Draw bar proportional to number of hits */
      Rep_DrawBarNumHits (Report->Hits.Num,Report->Hits.Max,Rep_MAX_BAR_WIDTH);

      /* Write action text */
      fprintf (Rep_File,"&nbsp;");
      if (ActCod >= 0)
	{
	 if ((Action = Act_GetActionFromActCod (ActCod)) >= 0)
	   {
	    Tab = Act_GetTab (Act_GetSuperAction (Action));
	    if (Txt_TABS_TXT[Tab])
	       fprintf (Rep_File,"%s &gt; ",Txt_TABS_TXT[Tab]);
	   }

	 fprintf (Rep_File,"%s",Act_GetActionText (Action));
	}
      else
	 fprintf (Rep_File,"?");
      fprintf (Rep_File,"<br />");
     }

   /***** Draw bar for the rest of the clicks *****/
   if ((unsigned long) Report->UsrFigures.NumClicks > NumClicks)
     {
      fprintf (Rep_File,"%ld&nbsp;%s",
               Report->UsrFigures.NumClicks - NumClicks,
               Txt_Other_actions);
      fprintf (Rep_File,"<br />");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End section *****/
   fprintf (Rep_File,"</section>\n");
  }

/*****************************************************************************/
/****** Write section for user's current courses in user's usage report ******/
/*****************************************************************************/

static void Rep_WriteSectionCurrentCourses (struct Rep_Report *Report)
  {
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];
   Rol_Role_t Role;

   /***** Begin section *****/
   fprintf (Rep_File,"<section>"
                     "<h3>%s",
            Txt_HIERARCHY_PLURAL_Abc[Hie_CRS]);
   if (Report->CurrentTimeUTC.StrDate[0])
      fprintf (Rep_File," (%s)",Report->CurrentTimeUTC.StrDate);
   fprintf (Rep_File,"</h3>");
   fprintf (Rep_File,"<ul>");

   /***** Number of courses in which the user is student/teacher *****/
   for (Role  = Rol_STD;
	Role <= Rol_TCH;
	Role++)
      /* List my courses with this role */
      Rep_GetAndWriteMyCurrentCrss (Role,Report);

   /***** End section *****/
   fprintf (Rep_File,"</ul>"
	             "</section>\n");
  }

/*****************************************************************************/
/***** Write section for user's historic courses in user's usage report ******/
/*****************************************************************************/

static void Rep_WriteSectionHistoricCourses (struct Rep_Report *Report)
  {
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_historical_log;
   extern const char *Txt_Only_courses_with_more_than_X_clicks_are_shown;
   Rol_Role_t Role;

   /***** Begin section *****/
   fprintf (Rep_File,"<section>"
                     "<h3>%s (%s)</h3>",
	    Txt_HIERARCHY_PLURAL_Abc[Hie_CRS],Txt_historical_log);
   fprintf (Rep_File,Txt_Only_courses_with_more_than_X_clicks_are_shown,
            Rep_MIN_CLICKS_CRS);
   fprintf (Rep_File,"<ul>");

   /********* Historic clicks of a user without course selected ***********/
   Rep_GetAndWriteMyHistoricClicsWithoutCrs (Report);

   /***** Historic courses in which the user clicked as student/teacher *****/
   for (Role  = Rol_STD;
	Role <= Rol_TCH;
	Role++)
      /* List my courses with this role */
      Rep_GetAndWriteMyHistoricCrss (Role,Report);

   /***** End section *****/
   fprintf (Rep_File,"</ul>"
	             "</section>\n");
  }

/*****************************************************************************/
/************ Get the maximum number of hits per course-year-role ************/
/*****************************************************************************/

static void Rep_GetMaxHitsPerYear (struct Rep_Report *Report)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   Report->MaxHitsPerYear = 0;

   /***** Get the maximum number of hits per year *****/
   if (Log_DB_GetMyMaxHitsPerYear (&mysql_res,Report->UsrFigures.FirstClickTimeUTC))
     {
      row = mysql_fetch_row (mysql_res);
      if (row[0])	// There are questions
	 if (sscanf (row[0],"%lu",&Report->MaxHitsPerYear) != 1)
	    Err_ShowErrorAndExit ("Error when getting maximum hits.");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************* Write my current courses **************************/
/*****************************************************************************/

static void Rep_GetAndWriteMyCurrentCrss (Rol_Role_t Role,
                                          struct Rep_Report *Report)
  {
   extern const char *Txt_USER_in_COURSE;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_HIERARCHY_SINGUL_abc[Hie_NUM_LEVELS];
   extern const char *Txt_HIERARCHY_PLURAL_abc[Hie_NUM_LEVELS];
   extern const char *Txt_teachers_ABBREVIATION;
   extern const char *Txt_students_ABBREVIATION;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   long CrsCod;

   NumCrss = Enr_DB_GetNumCrssOfUsrWithARole (Gbl.Usrs.Me.UsrDat.UsrCod,Role);
   fprintf (Rep_File,"<li>");
   fprintf (Rep_File,Txt_USER_in_COURSE,
	    Txt_ROLES_SINGUL_Abc[Role][Gbl.Usrs.Me.UsrDat.Sex]);
   fprintf (Rep_File," %u %s",
	    NumCrss,
	    NumCrss == 1 ? Txt_HIERARCHY_SINGUL_abc[Hie_CRS] :
			   Txt_HIERARCHY_PLURAL_abc[Hie_CRS]);

   if (NumCrss)
     {
      fprintf (Rep_File," (%u %s / %u %s):",
	       Enr_DB_GetNumUsrsInCrssOfAUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Role,
		                           (1 << Rol_NET) |
		                           (1 << Rol_TCH)),
	       Txt_teachers_ABBREVIATION,
	       Enr_DB_GetNumUsrsInCrssOfAUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Role,
					   (1 << Rol_STD)),
	       Txt_students_ABBREVIATION);

      /***** Get and list my courses (one row per course) *****/
      if ((NumCrss = Log_DB_GetMyCrssAndHitsPerCrs (&mysql_res,Role)))
	{
	 /* Heading row */
	 fprintf (Rep_File,"<ol>");

	 /* Write courses */
	 for (NumCrs  = 1;
	      NumCrs <= NumCrss;
	      NumCrs++)
	   {
	    /* Get next course */
	    row = mysql_fetch_row (mysql_res);

	    /* Get course code (row[0]) */
	    CrsCod = Str_ConvertStrCodToLongCod (row[0]);

	    /* Write data of this course */
	    Rep_WriteRowCrsData (CrsCod,Role,Report,
				 true);	// Write number of users in course
	   }

	 /* End table */
	 fprintf (Rep_File,"</ol>");
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   fprintf (Rep_File,"</li>");
  }

/*****************************************************************************/
/************* Write my historic clicks without course selected **************/
/*****************************************************************************/

static void Rep_GetAndWriteMyHistoricClicsWithoutCrs (struct Rep_Report *Report)
  {
   extern const char *Txt_Hits_without_course_selected;

   /***** Heading row *****/
   fprintf (Rep_File,"<li>%s:"
		     "<ol>",
	    Txt_Hits_without_course_selected);

   /***** Historic clicks *****/
   Rep_WriteRowCrsData (-1L,
                        Rol_UNK,	// Role does not matter
			Report,
			false);	// Do not write number of users in course

   /***** End list *****/
   fprintf (Rep_File,"</ol>"
		     "</li>");
  }

/*****************************************************************************/
/********************** Write historic courses of a user *********************/
/*****************************************************************************/

static void Rep_GetAndWriteMyHistoricCrss (Rol_Role_t Role,
                                           struct Rep_Report *Report)
  {
   extern const char *Txt_Hits_as_a_USER;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   long CrsCod;

   /***** Get and list historic courses of a user from log (one row per course) *****/
   if ((NumCrss = Log_DB_GetMyHistoricCrss (&mysql_res,Role,Rep_MIN_CLICKS_CRS)))
     {
      /* Heading row */
      fprintf (Rep_File,"<li>");
      fprintf (Rep_File,Txt_Hits_as_a_USER,
	       Txt_ROLES_SINGUL_abc[Role][Gbl.Usrs.Me.UsrDat.Sex]);
      fprintf (Rep_File,":<ol>");

      /* Write courses */
      for (NumCrs  = 1;
	   NumCrs <= NumCrss;
	   NumCrs++)
        {
         /* Get next course */
         row = mysql_fetch_row (mysql_res);

	 /* Get course code (row[0]) */
	 CrsCod = Str_ConvertStrCodToLongCod (row[0]);

         /* Write data of this course */
         Rep_WriteRowCrsData (CrsCod,Role,Report,
			      false);	// Do not write number of users in course
	}

      /* End list */
      fprintf (Rep_File,"</ol>"
	                "</li>");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Write the data of a course (result of a query) ***************/
/*****************************************************************************/

static void Rep_WriteRowCrsData (long CrsCod,Rol_Role_t Role,
                                 struct Rep_Report *Report,
                                 bool WriteNumUsrs)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_teachers_ABBREVIATION;
   extern const char *Txt_students_ABBREVIATION;
   extern const char *Txt_unknown_removed_course;
   extern const char *Txt_no_course_selected;
   struct Hie_Node Crs;
   struct Hie_Node Deg;

   /***** Get course data *****/
   Crs.HieCod = CrsCod;
   Hie_GetDataByCod[Hie_CRS] (&Crs);

   /***** Get degree data *****/
   Deg.HieCod = Crs.PrtCod;
   Hie_GetDataByCod[Hie_DEG] (&Deg);

   /***** Begin row *****/
   fprintf (Rep_File,"<li>");

   if (CrsCod > 0)	// CrsCod > 0 in log ==> course selected
     {
      if (Crs.HieCod > 0)	// Course exists
	{
	 /***** Write course full name *****/
	 fprintf (Rep_File,"<strong>%s</strong> -",Crs.FullName);

	 /***** Write year *****/
	 if (Crs.Specific.Year)
	    fprintf (Rep_File," %s",Txt_YEAR_OF_DEGREE[Crs.Specific.Year]);

	 /***** Write degree full name *****/
	 fprintf (Rep_File," %s",Deg.FullName);

	 /***** Write number of teachers / students in course *****/
	 if (WriteNumUsrs)
	    fprintf (Rep_File," (%u %s / %u %s)",
		     Enr_GetCachedNumUsrsInCrss (Hie_CRS,Crs.HieCod,
				                 1 << Rol_NET |
					         1 << Rol_TCH),
		     Txt_teachers_ABBREVIATION,
		     Enr_GetCachedNumUsrsInCrss (Hie_CRS,Crs.HieCod,
				                 1 << Rol_STD),
		     Txt_students_ABBREVIATION);
	}
      else
         fprintf (Rep_File,"(%s)",Txt_unknown_removed_course);
     }
   else	// CrsCod <= 0 in log ==> no course selected
      fprintf (Rep_File,"(%s)",Txt_no_course_selected);

   /***** Write hits per year for this course *****/
   fprintf (Rep_File,"<br />");
   Rep_ShowMyHitsPerYear (false,CrsCod,Role,Report);

   fprintf (Rep_File,"</li>");
  }

/*****************************************************************************/
/********************** Write my hits grouped by years ***********************/
/*****************************************************************************/

static void Rep_ShowMyHitsPerYear (bool AnyCourse,long CrsCod,Rol_Role_t Role,
                                   struct Rep_Report *Report)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumHits;
   unsigned NumHit;
   unsigned ReadYear;
   unsigned FirstYear;
   unsigned LastYear;
   unsigned Year;

   /***** Make the query *****/
   NumHits = Log_DB_GetMyHitsPerYear (&mysql_res,AnyCourse,CrsCod,Role,
                                      Report->UsrFigures.FirstClickTimeUTC);

   /***** Initialize first year *****/
   FirstYear = 1900 + Report->tm_FirstClickTime.tm_year;

   /***** Initialize LastYear *****/
   LastYear = Dat_GetCurrentYear ();

   /***** Set maximum number of hits per year *****/
   if (Report->MaxHitsPerYear)
      /* Set maximum number of hits per year from parameter */
      Report->Hits.Max = Report->MaxHitsPerYear;
   else
     {
      /* Compute maximum number of hits per year */
      Rep_ComputeMaxAndTotalHits (&Report->Hits,NumHits,mysql_res,1);
      mysql_data_seek (mysql_res,0);
     }

   /***** Write rows *****/
   for (NumHit  = 1;
	NumHit <= NumHits;
	NumHit++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get the year (in row[0] is the date in YYYY format) */
      if (sscanf (row[0],"%04u",&ReadYear) != 1)
	 Err_WrongDateExit ();

      /* Get number hits (in row[1]) */
      if (sscanf (row[1],"%lu",&Report->Hits.Num) != 1)
	 Report->Hits.Num = 0;

      for (Year = LastYear;
	   Year >= ReadYear;
	   Year--)
        {
         /* Write the year */
         fprintf (Rep_File,"%04u&nbsp;",Year);

         /* Draw bar proportional to number of hits */
         Rep_DrawBarNumHits (Year == ReadYear ? Report->Hits.Num :
                        	                0,
                             Report->Hits.Max,Rep_MAX_BAR_WIDTH);
         fprintf (Rep_File,"<br />");
        }
      LastYear = Year;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Finally, show the oldest years without clicks *****/
   for (Year  = LastYear;
        Year >= FirstYear;
        Year--)
     {
      /* Write the year */
      fprintf (Rep_File,"%04u&nbsp;",Year);

      /* Draw bar proportional to number of hits */
      Rep_DrawBarNumHits (0,Report->Hits.Max,Rep_MAX_BAR_WIDTH);
      fprintf (Rep_File,"<br />");
     }
  }

/*****************************************************************************/
/*************** Compute maximum and total number of hits ********************/
/*****************************************************************************/

static void Rep_ComputeMaxAndTotalHits (struct Rep_Hits *Hits,
                                        unsigned NumHits,
                                        MYSQL_RES *mysql_res,unsigned Field)
  {
   unsigned NumHit;
   MYSQL_ROW row;

   /***** For each row... *****/
   for (NumHit  = 1, Hits->Max = 0;
	NumHit <= NumHits;
	NumHit++)
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get number of hits */
      if (sscanf (row[Field],"%lu",&Hits->Num) != 1)
	 Hits->Num = 0;

      /* Update maximum hits */
      if (Hits->Num > Hits->Max)
	 Hits->Max = Hits->Num;
     }
  }

/*****************************************************************************/
/********************* Draw a bar with the number of hits ********************/
/*****************************************************************************/

static void Rep_DrawBarNumHits (unsigned long HitsNum,unsigned long HitsMax,
                                unsigned MaxBarWidth)
  {
   unsigned BarWidth;
   unsigned i;

   if (HitsNum)
     {
      /***** Draw bar with a with proportional to the number of hits *****/
      BarWidth = (unsigned) ((((double) HitsNum * (double) MaxBarWidth) /
	                       (double) HitsMax) + 0.5);
      if (BarWidth)
	{
         fprintf (Rep_File,"<strong>");
	 for (i = 0;
	      i < BarWidth;
	      i++)
	    fprintf (Rep_File,Rep_BLOCK);
         fprintf (Rep_File,"</strong>");
	}

      /***** Write the number of hits *****/
      fprintf (Rep_File,"&nbsp;");
      Rep_WriteDouble (HitsNum);
     }
  }

/*****************************************************************************/
/********** Remove all user's usage report of a user from database ***********/
/*****************************************************************************/

static void Rep_WriteDouble (double Num)
  {
   char *Str;

   /***** Write from floating point number to string
          with the correct accuracy *****/
   Str_DoubleNumToStrFewDigits (&Str,Num);

   /***** Write number from string to file *****/
   fputs (Str,Rep_File);

   /***** Free memory allocated for string *****/
   free (Str);
  }

/*****************************************************************************/
/********** Remove all user's usage report of a user from database ***********/
/*****************************************************************************/

void Rep_RemoveUsrUsageReports (long UsrCod)
  {
   /***** Remove all user's usage report files of a user *****/
   Rep_RemoveUsrReportsFiles (UsrCod);

   /***** Remove all user's usage reports of a user from database *****/
   Rep_DB_RemoveUsrReports (UsrCod);
  }

/*****************************************************************************/
/********** Remove all user's usage reports of a user from database **********/
/*****************************************************************************/

static void Rep_RemoveUsrReportsFiles (long UsrCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumReports;
   unsigned NumReport;
   char PathUniqueDirReport[PATH_MAX + 1];

   /***** Get directories for the reports *****/
   NumReports = Rep_DB_GetUsrReportsFiles (&mysql_res,UsrCod);

   /***** Remove the reports *****/
   for (NumReport = 0;
	NumReport < NumReports;
	NumReport++)
     {
      /* Get next report */
      row = mysql_fetch_row (mysql_res);

      /* Remove report directory and file */
      snprintf (PathUniqueDirReport,sizeof (PathUniqueDirReport),"%s/%s/%s",
	        Cfg_PATH_REP_PUBLIC,row[0],row[1]);
      Fil_RemoveTree (PathUniqueDirReport);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
