// swad_report.c: report on my use of the platform

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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

#include <sys/stat.h>		// For mkdir
#include <sys/types.h>		// For mkdir

#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_profile.h"
#include "swad_tab.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

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
/****************************** Internal types *******************************/
/*****************************************************************************/

struct Rep_CurrentTimeUTC
  {
   char StrDate[10 + 1];	// Example: 2016-10-02
				//          1234567890
   char StrTime[8 + 1];		// Example: 19:03:49
				//          12345678
   unsigned Date;		// Example: 20161002
   unsigned Time;		// Example: 190349
  };

struct Rep_Hits
  {
   unsigned long Num;
   unsigned long Max;
  };

struct Rep_Report
  {
   struct UsrFigures UsrFigures;
   struct tm tm_FirstClickTime;
   struct tm tm_CurrentTime;
   struct Rep_CurrentTimeUTC CurrentTimeUTC;
   struct Rep_Hits Hits;
   unsigned long MaxHitsPerYear;
   char FilenameReport[NAME_MAX + 1];
   char Permalink[Cns_MAX_BYTES_WWW + 1];
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Rep_CreateMyUsageReport (struct Rep_Report *Report);
static void Rep_PutLinkToMyUsageReport (struct Rep_Report *Report);
static void Rep_TitleReport (struct Rep_CurrentTimeUTC *CurrentTimeUTC);

static void Rep_GetCurrentDateTimeUTC (struct Rep_Report *Report);

static void Rep_CreateNewReportFile (struct Rep_Report *Report);
static void Rep_CreateNewReportEntryIntoDB (const struct Rep_Report *Report);
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
                                        unsigned long NumRows,
                                        MYSQL_RES *mysql_res,unsigned Field);
static void Rep_DrawBarNumHits (unsigned long HitsNum,unsigned long HitsMax,
                                unsigned MaxBarWidth);

static void Rep_RemoveUsrReportsFiles (long UsrCod);
static void Rep_RemoveUsrReportsFromDB (long UsrCod);

/*****************************************************************************/
/******* Request my usage report (report on my use of the platform) **********/
/*****************************************************************************/

void Rep_ReqMyUsageReport (void)
  {
   extern const char *Hlp_ANALYTICS_Report;
   extern const char *Txt_Report_of_use_of_PLATFORM;
   extern const char *Txt_Generate_report;

   /***** Form to show my usage report *****/
   Frm_StartForm (ActSeeMyUsgRep);

   /***** Begin box *****/
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Report_of_use_of_PLATFORM,
	     Cfg_PLATFORM_SHORT_NAME);
   Box_BoxBegin (NULL,Gbl.Title,NULL,
                 Hlp_ANALYTICS_Report,Box_NOT_CLOSABLE);

   /***** Header *****/
   Rep_TitleReport (NULL);	// NULL means do not write date

   /***** Send button and end box *****/
   Box_EndBoxWithButton (Btn_CONFIRM_BUTTON,Txt_Generate_report);

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
   Rep_CreateNewReportEntryIntoDB (Report);

   /***** Start file *****/
   Lay_StartHTMLFile (Gbl.F.Rep,Report->FilenameReport);
   fprintf (Gbl.F.Rep,"<body style=\"margin:1em;"
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
   fprintf (Gbl.F.Rep,"</body>\n"
	              "</html>\n");

   /***** Close report file *****/
   Fil_CloseReportFile ();
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

   /***** Begin box *****/
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Report_of_use_of_PLATFORM,
	     Cfg_PLATFORM_SHORT_NAME);
   Box_BoxBegin (NULL,Gbl.Title,NULL,
                 Hlp_ANALYTICS_Report,Box_NOT_CLOSABLE);

   /***** Header *****/
   Rep_TitleReport (&Report->CurrentTimeUTC);

   /***** Put anchor and report filename *****/
   HTM_DIV_Begin ("class=\"FILENAME_TXT CM\"");
   HTM_A_Begin ("href=\"%s\" class=\"FILENAME_TXT\" title=\"%s\" target=\"_blank\"",
	        Report->Permalink,
	        Txt_Report);
   Ico_PutIcon ("file-alt.svg",Txt_Report,"ICO64x64");
   HTM_BR ();
   HTM_Txt (Report->FilenameReport);
   HTM_A_End ();
   HTM_DIV_End ();

   HTM_DIV_Begin ("class=\"DAT_LIGHT\"");
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

   HTM_DIV_Begin ("class=\"TITLE_REPORT DAT\"");

   /***** User *****/
   HTM_TxtF ("%s:&nbsp;",Txt_User[Gbl.Usrs.Me.UsrDat.Sex]);
   HTM_SPAN_Begin ("class=\"DAT_N_BOLD\"");
   HTM_Txt (Gbl.Usrs.Me.UsrDat.FullName);
   HTM_SPAN_End ();

   /***** Report date *****/
   if (CurrentTimeUTC)
     {
      HTM_BR ();
      HTM_TxtF ("%s:&nbsp;",Txt_Date);
      HTM_SPAN_Begin ("class=\"DAT_N\"");
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
	        sizeof (Report->CurrentTimeUTC.StrDate),
	        "%04d-%02d-%02d",
	        1900 + Report->tm_CurrentTime.tm_year,	// year
	        1 + Report->tm_CurrentTime.tm_mon,	// month
	        Report->tm_CurrentTime.tm_mday);		// day of the month
      snprintf (Report->CurrentTimeUTC.StrTime,
	        sizeof (Report->CurrentTimeUTC.StrTime),
	        "%02d:%02d:%02d",
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
   char PathUniqueDirR[PATH_MAX + 1];
   char PathFileReport[PATH_MAX + 1];
   char Permalink[128 +
		  Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 +
		  NAME_MAX];

   /***** Path for reports *****/
   Fil_CreateDirIfNotExists (Cfg_PATH_REP_PUBLIC);

   /***** Unique directory for the file with the report *****/
   /* 1. Create a directory using the leftmost 2 chars of a unique name */
   snprintf (PathUniqueDirL,sizeof (PathUniqueDirL),
	     "%s/%c%c",
             Cfg_PATH_REP_PUBLIC,
             Gbl.UniqueNameEncrypted[0],
             Gbl.UniqueNameEncrypted[1]);
   Fil_CreateDirIfNotExists (PathUniqueDirL);

   /* 2. Create a directory using the rightmost 41 chars of a unique name */
   snprintf (PathUniqueDirR,sizeof (PathUniqueDirR),
	     "%s/%s",
             PathUniqueDirL,
             &Gbl.UniqueNameEncrypted[2]);
   if (mkdir (PathUniqueDirR,(mode_t) 0xFFF))
      Lay_ShowErrorAndExit ("Can not create directory for report.");

   /***** Path of the public file with the report */
   snprintf (Report->FilenameReport,sizeof (Report->FilenameReport),
	     "%s_%06u_%06u.html",
             Rep_FILENAME_ROOT,Report->CurrentTimeUTC.Date,Report->CurrentTimeUTC.Time);
   snprintf (PathFileReport,sizeof (PathFileReport),
	     "%s/%s",
             PathUniqueDirR,Report->FilenameReport);
   if ((Gbl.F.Rep = fopen (PathFileReport,"wb")) == NULL)
      Lay_ShowErrorAndExit ("Can not create report file.");

   /***** Permalink *****/
   snprintf (Permalink,sizeof (Permalink),
	     "%s/%c%c/%s/%s",
             Cfg_URL_REP_PUBLIC,
             Gbl.UniqueNameEncrypted[0],
             Gbl.UniqueNameEncrypted[1],
             &Gbl.UniqueNameEncrypted[2],
             Report->FilenameReport);
   Str_Copy (Report->Permalink,Permalink,
	     Cns_MAX_BYTES_WWW);
  }

/*****************************************************************************/
/************** Insert a new user's usage report into database ***************/
/*****************************************************************************/

static void Rep_CreateNewReportEntryIntoDB (const struct Rep_Report *Report)
  {
   /***** Insert a new user's usage report into database *****/
   DB_QueryINSERT ("can not create new user's usage report",
		   "INSERT INTO usr_report"
	           " (UsrCod,ReportTimeUTC,"
	           "UniqueDirL,UniqueDirR,Filename,Permalink)"
                   " VALUES"
                   " (%ld,'%04d-%02d-%02d %02d:%02d:%02d',"
                   "'%c%c','%s','%s','%s')",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   1900 + Report->tm_CurrentTime.tm_year,	// year
		   1 +  Report->tm_CurrentTime.tm_mon,		// month
		   Report->tm_CurrentTime.tm_mday,		// day of the month
		   Report->tm_CurrentTime.tm_hour,		// hours
		   Report->tm_CurrentTime.tm_min,		// minutes
		   Report->tm_CurrentTime.tm_sec,		// seconds
		   Gbl.UniqueNameEncrypted[0],		//  2  leftmost chars from a unique 43 chars base64url codified from a unique SHA-256 string
		   Gbl.UniqueNameEncrypted[1],
		   &Gbl.UniqueNameEncrypted[2],		// 41 rightmost chars from a unique 43 chars base64url codified from a unique SHA-256 string
		   Report->FilenameReport,Report->Permalink);
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

   /***** Start header *****/
   fprintf (Gbl.F.Rep,"<header>");

   /***** Main title *****/
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Report_of_use_of_PLATFORM,
	     Cfg_PLATFORM_SHORT_NAME);
   fprintf (Gbl.F.Rep,"<h1>%s</h1>",Gbl.Title);
   HTM_UL_Begin (NULL);

   /***** User *****/
   fprintf (Gbl.F.Rep,"<li>%s: <strong>%s</strong></li>",
	    Txt_User[Gbl.Usrs.Me.UsrDat.Sex],
	    Gbl.Usrs.Me.UsrDat.FullName);

   /***** Date-time *****/
   fprintf (Gbl.F.Rep,"<li>%s: %s %s UTC</li>",
            Txt_Date,
	    Report->CurrentTimeUTC.StrDate,
	    Report->CurrentTimeUTC.StrTime);

   /***** Permalink *****/
   fprintf (Gbl.F.Rep,"<li>%s: "
	              "<a href=\"%s\" target=\"_blank\""
		      " style=\"text-decoration:none;\">"
	              "%s"
	              "</a>"
	              "</li>",
            Txt_Permalink,
            Report->Permalink,Report->Permalink);

   /***** End header *****/
   fprintf (Gbl.F.Rep,"</ul>"
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

   /***** Start section *****/
   fprintf (Gbl.F.Rep,"<section>"
	              "<h3>%s</h3>",
	    Txt_Teaching_platform);
   HTM_UL_Begin (NULL);

   /***** Platform name *****/
   fprintf (Gbl.F.Rep,"<li>%s: %s, %s</li>",
            Txt_Name,
            Cfg_PLATFORM_FULL_NAME,Txt_TAGLINE);

   /***** Server URL *****/
   fprintf (Gbl.F.Rep,"<li>%s: "
	              "<a href=\"%s\" target=\"_blank\""
		      " style=\"text-decoration:none;\">"
	              "%s"
	              "</a>"
	              "</li>",
            Txt_URL,Cfg_URL_SWAD_SERVER,Cfg_URL_SWAD_SERVER);

   /***** End section *****/
   fprintf (Gbl.F.Rep,"</ul>"
	              "</section>\n");
  }

/*****************************************************************************/
/*********** Write section for user's info in user's usage report ************/
/*****************************************************************************/

static void Rep_WriteSectionUsrInfo (void)
  {
   extern const char *Txt_Personal_information;
   extern const char *Txt_Name;
   extern const char *Txt_Email;
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   char CtyName[Hie_MAX_BYTES_FULL_NAME + 1];
   struct Instit Ins;

   /***** Start section *****/
   fprintf (Gbl.F.Rep,"<section>"
	              "<h3>%s</h3>",
	    Txt_Personal_information);
   HTM_UL_Begin (NULL);

   /***** User's name *****/
   fprintf (Gbl.F.Rep,"<li>%s: %s</li>",
            Txt_Name,
            Gbl.Usrs.Me.UsrDat.FullName);

   /***** User's email *****/
   fprintf (Gbl.F.Rep,"<li>%s: %s</li>",
            Txt_Email,
            Gbl.Usrs.Me.UsrDat.Email);

   /***** User's country *****/
   Cty_GetCountryName (Gbl.Usrs.Me.UsrDat.CtyCod,CtyName);
   fprintf (Gbl.F.Rep,"<li>%s: %s</li>",
            Txt_Country,
            CtyName);

   /***** User's institution *****/
   Ins.InsCod = Gbl.Usrs.Me.UsrDat.InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);
   fprintf (Gbl.F.Rep,"<li>%s: %s</li>",
            Txt_Institution,
            Ins.FullName);

   /***** End section *****/
   fprintf (Gbl.F.Rep,"</ul>"
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

   /***** Start section *****/
   fprintf (Gbl.F.Rep,"<section>"
                      "<h3>%s</h3>",
	    Txt_Figures);
   HTM_UL_Begin (NULL);

   /***** Time since first click until now *****/
   fprintf (Gbl.F.Rep,"<li>%s ",Txt_TIME_Since);
   if (Report->UsrFigures.FirstClickTimeUTC)
     {
      fprintf (Gbl.F.Rep,"%04d-%02d-%02d %02d:%02d:%02d UTC",
	       1900 + Report->tm_FirstClickTime.tm_year,	// year
	       1 + Report->tm_FirstClickTime.tm_mon,		// month
	       Report->tm_FirstClickTime.tm_mday,		// day of the month
	       Report->tm_FirstClickTime.tm_hour,		// hours
	       Report->tm_FirstClickTime.tm_min,		// minutes
	       Report->tm_FirstClickTime.tm_sec);		// seconds
      if (Report->CurrentTimeUTC.StrDate[0])
	 fprintf (Gbl.F.Rep," %s %s %s UTC",
		  Txt_TIME_until,
		  Report->CurrentTimeUTC.StrDate,
		  Report->CurrentTimeUTC.StrTime);
      if (Report->UsrFigures.NumDays > 0)
	 fprintf (Gbl.F.Rep," (%d %s)",
		  Report->UsrFigures.NumDays,
		  (Report->UsrFigures.NumDays == 1) ? Txt_day :
						      Txt_days);
     }
   else	// Time of first click is unknown
     {
      fprintf (Gbl.F.Rep,"?");
      if (Report->CurrentTimeUTC.StrDate[0])
         fprintf (Gbl.F.Rep," - %s %s UTC",
                  Report->CurrentTimeUTC.StrDate,
                  Report->CurrentTimeUTC.StrTime);
     }
   fprintf (Gbl.F.Rep,"</li>");

   /***** Number of clicks *****/
   fprintf (Gbl.F.Rep,"<li>%s: ",Txt_Clicks);
   if (Report->UsrFigures.NumClicks >= 0)
     {
      fprintf (Gbl.F.Rep,"%ld",Report->UsrFigures.NumClicks);
      if (Report->UsrFigures.NumDays > 0)
	{
	 fprintf (Gbl.F.Rep," (");
	 Str_WriteDoubleNumToFile (Gbl.F.Rep,
	                           (double) Report->UsrFigures.NumClicks /
			           (double) Report->UsrFigures.NumDays);
	 fprintf (Gbl.F.Rep," / %s)",Txt_day);
	}
     }
   else	// Number of clicks is unknown
      fprintf (Gbl.F.Rep,"?");
   fprintf (Gbl.F.Rep,"</li>");

   /***** Number of files currently published *****/
   if ((NumFiles = Brw_GetNumFilesUsr (Gbl.Usrs.Me.UsrDat.UsrCod)))
      NumPublicFiles = Brw_GetNumPublicFilesUsr (Gbl.Usrs.Me.UsrDat.UsrCod);
   else
      NumPublicFiles = 0;
   fprintf (Gbl.F.Rep,"<li>"
		      "%s: %u %s (%u %s)"
		      "</li>",
	    Txt_Files_uploaded,
	    NumFiles,
	    (NumFiles == 1) ? Txt_file :
		              Txt_files,
	    NumPublicFiles,Txt_public_FILES);

   /***** Number of file views *****/
   fprintf (Gbl.F.Rep,"<li>%s: ",Txt_Downloads);
   if (Report->UsrFigures.NumFileViews >= 0)
     {
      fprintf (Gbl.F.Rep,"%ld %s",
               Report->UsrFigures.NumFileViews,
	       (Report->UsrFigures.NumFileViews == 1) ? Txt_download :
						        Txt_downloads);
      if (Report->UsrFigures.NumDays > 0)
	{
	 fprintf (Gbl.F.Rep," (");
	 Str_WriteDoubleNumToFile (Gbl.F.Rep,
	                           (double) Report->UsrFigures.NumFileViews /
			           (double) Report->UsrFigures.NumDays);
	 fprintf (Gbl.F.Rep," / %s)",Txt_day);
	}
     }
   else	// Number of file views is unknown
      fprintf (Gbl.F.Rep,"?");
   fprintf (Gbl.F.Rep,"</li>");

   /***** Number of posts in forums *****/
   fprintf (Gbl.F.Rep,"<li>%s: ",Txt_Forum_posts);
   if (Report->UsrFigures.NumForPst >= 0)
     {
      fprintf (Gbl.F.Rep,"%ld %s",
	       Report->UsrFigures.NumForPst,
	       (Report->UsrFigures.NumForPst == 1) ? Txt_FORUM_post :
					             Txt_FORUM_posts);
      if (Report->UsrFigures.NumDays > 0)
	{
	 fprintf (Gbl.F.Rep," (");
	 Str_WriteDoubleNumToFile (Gbl.F.Rep,
	                           (double) Report->UsrFigures.NumForPst /
			           (double) Report->UsrFigures.NumDays);
	 fprintf (Gbl.F.Rep," / %s)",Txt_day);
	}
     }
   else	// Number of forum posts is unknown
      fprintf (Gbl.F.Rep,"?");
   fprintf (Gbl.F.Rep,"</li>");

   /***** Number of messages sent *****/
   fprintf (Gbl.F.Rep,"<li>%s: ",Txt_Messages_sent);
   if (Report->UsrFigures.NumMsgSnt >= 0)
     {
      fprintf (Gbl.F.Rep,"%ld %s",
	       Report->UsrFigures.NumMsgSnt,
	       (Report->UsrFigures.NumMsgSnt == 1) ? Txt_message :
					             Txt_messages);
      if (Report->UsrFigures.NumDays > 0)
	{
	 fprintf (Gbl.F.Rep," (");
	 Str_WriteDoubleNumToFile (Gbl.F.Rep,
	                           (double) Report->UsrFigures.NumMsgSnt /
			           (double) Report->UsrFigures.NumDays);
	 fprintf (Gbl.F.Rep," / %s)",Txt_day);
	}
     }
   else	// Number of messages sent is unknown
      fprintf (Gbl.F.Rep,"?");
   fprintf (Gbl.F.Rep,"</li>");

   /***** End section *****/
   fprintf (Gbl.F.Rep,"</ul>"
	              "</section>\n");
  }

/*****************************************************************************/
/******** Write section for user's global hits in user's usage report ********/
/*****************************************************************************/

static void Rep_WriteSectionGlobalHits (struct Rep_Report *Report)
  {
   extern const char *Txt_Hits_per_year;

   /***** Start section *****/
   fprintf (Gbl.F.Rep,"<section>"
                      "<h3>%s</h3>",
	    Txt_Hits_per_year);

   /***** Global (in any course) hits per year *****/
   Report->MaxHitsPerYear = 0;	// MaxHitsPerYear not passed as an argument but computed inside the function
   Rep_ShowMyHitsPerYear (true,-1L,	// Any course
                          Rol_UNK,	// Any role
                          Report);

   /***** End section *****/
   fprintf (Gbl.F.Rep,"</section>\n");
  }

/*****************************************************************************/
/******** Write section for user's global hits in user's usage report ********/
/*****************************************************************************/

static void Rep_WriteSectionHitsPerAction (struct Rep_Report *Report)
  {
   extern Act_Action_t Act_FromActCodToAction[1 + Act_MAX_ACTION_COD];
   extern const char *Txt_Hits_per_action;
   extern const char *Txt_TABS_TXT[Tab_NUM_TABS];
   extern const char *Txt_Other_actions;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned long NumRow;
   long ActCod;
   Act_Action_t Action;
   Tab_Tab_t Tab;
   char ActTxt[Act_MAX_BYTES_ACTION_TXT + 1];
   unsigned long NumClicks;

   /***** Start section *****/
   fprintf (Gbl.F.Rep,"<section>"
                      "<h3>%s</h3>",
	    Txt_Hits_per_action);

   /***** Make the query *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get clicks",
			     "SELECT SQL_NO_CACHE ActCod,COUNT(*) AS N"
			     " FROM log_full"
			     " WHERE ClickTime>=FROM_UNIXTIME(%ld)"
			     " AND UsrCod=%ld"
			     " GROUP BY ActCod ORDER BY N DESC LIMIT %u",
			     (long) Report->UsrFigures.FirstClickTimeUTC,
			     Gbl.Usrs.Me.UsrDat.UsrCod,
			     Rep_MAX_ACTIONS);

   /***** Compute maximum number of hits per action *****/
   Rep_ComputeMaxAndTotalHits (&Report->Hits,NumRows,mysql_res,1);
   mysql_data_seek (mysql_res,0);

   /***** Write rows *****/
   for (NumRow = 1, NumClicks = 0;
	NumRow <= NumRows;
	NumRow++)
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
      fprintf (Gbl.F.Rep,"&nbsp;");
      if (ActCod >= 0)
	{
	 Action = Act_FromActCodToAction[ActCod];
	 if (Action >= 0)
	   {
	    Tab = Act_GetTab (Act_GetSuperAction (Action));
	    if (Txt_TABS_TXT[Tab])
	       fprintf (Gbl.F.Rep,"%s &gt; ",Txt_TABS_TXT[Tab]);
	   }
	 fprintf (Gbl.F.Rep,"%s",Act_GetActionTextFromDB (ActCod,ActTxt));
	}
      else
	 fprintf (Gbl.F.Rep,"?");
      fprintf (Gbl.F.Rep,"<br />");
     }

   /***** Draw bar for the rest of the clicks *****/
   if ((unsigned long) Report->UsrFigures.NumClicks > NumClicks)
     {
      fprintf (Gbl.F.Rep,"%ld&nbsp;%s",
               Report->UsrFigures.NumClicks - NumClicks,
               Txt_Other_actions);
      HTM_BR ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End section *****/
   fprintf (Gbl.F.Rep,"</section>\n");
  }

/*****************************************************************************/
/****** Write section for user's current courses in user's usage report ******/
/*****************************************************************************/

static void Rep_WriteSectionCurrentCourses (struct Rep_Report *Report)
  {
   extern const char *Txt_Courses;
   Rol_Role_t Role;

   /***** Start section *****/
   fprintf (Gbl.F.Rep,"<section>"
                      "<h3>%s",
            Txt_Courses);
   if (Report->CurrentTimeUTC.StrDate[0])
      fprintf (Gbl.F.Rep," (%s)",Report->CurrentTimeUTC.StrDate);
   fprintf (Gbl.F.Rep,"</h3>");
   HTM_UL_Begin (NULL);

   /***** Number of courses in which the user is student/teacher *****/
   for (Role  = Rol_STD;
	Role <= Rol_TCH;
	Role++)
      /* List my courses with this role */
      Rep_GetAndWriteMyCurrentCrss (Role,Report);

   /***** End section *****/
   fprintf (Gbl.F.Rep,"</ul>"
	              "</section>\n");
  }

/*****************************************************************************/
/***** Write section for user's historic courses in user's usage report ******/
/*****************************************************************************/

static void Rep_WriteSectionHistoricCourses (struct Rep_Report *Report)
  {
   extern const char *Txt_Courses;
   extern const char *Txt_historical_log;
   extern const char *Txt_Only_courses_with_more_than_X_clicks_are_shown;
   Rol_Role_t Role;

   /***** Start section *****/
   fprintf (Gbl.F.Rep,"<section>"
                      "<h3>%s (%s)</h3>",
	    Txt_Courses,Txt_historical_log);
   fprintf (Gbl.F.Rep,Txt_Only_courses_with_more_than_X_clicks_are_shown,
            Rep_MIN_CLICKS_CRS);
   HTM_UL_Begin (NULL);

   /********* Historic clicks of a user without course selected ***********/
   Rep_GetAndWriteMyHistoricClicsWithoutCrs (Report);

   /***** Historic courses in which the user clicked as student/teacher *****/
   for (Role  = Rol_STD;
	Role <= Rol_TCH;
	Role++)
      /* List my courses with this role */
      Rep_GetAndWriteMyHistoricCrss (Role,Report);

   /***** End section *****/
   fprintf (Gbl.F.Rep,"</ul>"
	              "</section>\n");
  }

/*****************************************************************************/
/************ Get the maximum number of hits per course-year-role ************/
/*****************************************************************************/
// Return the maximum number of hits per year

static void Rep_GetMaxHitsPerYear (struct Rep_Report *Report)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   DB_QuerySELECT (&mysql_res,"can not get last question index",
		   "SELECT MAX(N) FROM ("
		   // Clicks without course selected --------------------------
	           "SELECT "
	           "-1 AS CrsCod,"
	           "YEAR(CONVERT_TZ(ClickTime,@@session.time_zone,'UTC')) AS Year,"
	           "%u AS Role,"
	           "COUNT(*) AS N"
	           " FROM log_full"
	           " WHERE ClickTime>=FROM_UNIXTIME(%ld)"
	           " AND UsrCod=%ld"
	           " AND CrsCod<=0"
	           " GROUP BY Year"
		   // ---------------------------------------------------------
	           " UNION "
		   // Clicks as student, non-editing teacher or teacher in courses
	           "SELECT "
	           "CrsCod,"
	           "YEAR(CONVERT_TZ(ClickTime,@@session.time_zone,'UTC')) AS Year,"
	           "Role,"
	           "COUNT(*) AS N"
	           " FROM log_full"
	           " WHERE ClickTime>=FROM_UNIXTIME(%ld)"
	           " AND UsrCod=%ld"
	           " AND Role>=%u"	// Student
	           " AND Role<=%u"	// Teacher
	           " AND CrsCod>0"
	           " GROUP BY CrsCod,Year,Role"
		   // ---------------------------------------------------------
	           ") AS hits_per_crs_year",
		   (unsigned) Rol_UNK,
		   (long) Report->UsrFigures.FirstClickTimeUTC,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (long) Report->UsrFigures.FirstClickTimeUTC,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) Rol_STD,
		   (unsigned) Rol_TCH);

   /***** Get number of users *****/
   Report->MaxHitsPerYear = 0;
   row = mysql_fetch_row (mysql_res);
   if (row[0])	// There are questions
      if (sscanf (row[0],"%lu",&Report->MaxHitsPerYear) != 1)
         Lay_ShowErrorAndExit ("Error when getting maximum hits.");

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
   extern const char *Txt_course;
   extern const char *Txt_courses;
   extern const char *Txt_teachers_ABBREVIATION;
   extern const char *Txt_students_ABBREVIATION;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   long CrsCod;

   NumCrss = Usr_GetNumCrssOfUsrWithARole (Gbl.Usrs.Me.UsrDat.UsrCod,Role);
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_USER_in_COURSE,
	     Txt_ROLES_SINGUL_Abc[Role][Gbl.Usrs.Me.UsrDat.Sex]);
   fprintf (Gbl.F.Rep,"<li>%s %u %s",
	    Gbl.Title,
	    NumCrss,
	    NumCrss == 1 ? Txt_course :
			   Txt_courses);
   if (NumCrss)
     {
      fprintf (Gbl.F.Rep," (%u %s / %u %s):",
	       Usr_GetNumUsrsInCrssOfAUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Role,
		                           (1 << Rol_NET) |
		                           (1 << Rol_TCH)),
	       Txt_teachers_ABBREVIATION,
	       Usr_GetNumUsrsInCrssOfAUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Role,
					   (1 << Rol_STD)),
	       Txt_students_ABBREVIATION);

      /***** Get courses of a user from database *****/
      NumCrss =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get courses of a user",
				 "SELECT crs_usr.CrsCod,log_full.CrsCod,COUNT(*) AS N"
				 " FROM crs_usr LEFT JOIN log_full ON"
				 " (crs_usr.CrsCod=log_full.CrsCod"
				 " AND crs_usr.UsrCod=log_full.UsrCod"
				 " AND crs_usr.Role=log_full.Role)"
				 " WHERE crs_usr.UsrCod=%ld"
				 " AND crs_usr.Role=%u"
				 " GROUP BY crs_usr.CrsCod"
				 " ORDER BY N DESC,log_full.CrsCod DESC",
				 Gbl.Usrs.Me.UsrDat.UsrCod,(unsigned) Role);

      /***** List the courses (one row per course) *****/
      if (NumCrss)
	{
	 /* Heading row */
	 fprintf (Gbl.F.Rep,"<ol>");

	 /* Write courses */
	 for (NumCrs = 1;
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
	 fprintf (Gbl.F.Rep,"</ol>");
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   fprintf (Gbl.F.Rep,"</li>");
  }

/*****************************************************************************/
/************* Write my historic clicks without course selected **************/
/*****************************************************************************/

static void Rep_GetAndWriteMyHistoricClicsWithoutCrs (struct Rep_Report *Report)
  {
   extern const char *Txt_Hits_without_course_selected;

   /***** Heading row *****/
   fprintf (Gbl.F.Rep,"<li>%s:"
		      "<ol>",
	    Txt_Hits_without_course_selected);

   /***** Historic clicks *****/
   Rep_WriteRowCrsData (-1L,
                        Rol_UNK,	// Role does not matter
			Report,
			false);	// Do not write number of users in course

   /***** End list *****/
   fprintf (Gbl.F.Rep,"</ol>"
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

   /***** Get historic courses of a user from log *****/
   NumCrss =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get courses of a user",
			      "SELECT CrsCod,COUNT(*) AS N"
			      " FROM log_full"
			      " WHERE UsrCod=%ld AND Role=%u AND CrsCod>0"
			      " GROUP BY CrsCod"
			      " HAVING N>%u"
			      " ORDER BY N DESC",
			      Gbl.Usrs.Me.UsrDat.UsrCod,(unsigned) Role,
			      Rep_MIN_CLICKS_CRS);

   /***** List the courses (one row per course) *****/
   if (NumCrss)
     {
      /* Heading row */
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        Txt_Hits_as_a_USER,
                Txt_ROLES_SINGUL_abc[Role][Gbl.Usrs.Me.UsrDat.Sex]);
      fprintf (Gbl.F.Rep,"<li>%s:"
	                 "<ol>",
	       Gbl.Title);

      /* Write courses */
      for (NumCrs = 1;
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
      fprintf (Gbl.F.Rep,"</ol>"
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
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];
   extern const char *Txt_teachers_ABBREVIATION;
   extern const char *Txt_students_ABBREVIATION;
   extern const char *Txt_unknown_removed_course;
   extern const char *Txt_no_course_selected;
   struct Course Crs;
   struct Degree Deg;

   /***** Get course data *****/
   Crs.CrsCod = CrsCod;
   Crs_GetDataOfCourseByCod (&Crs);

   /***** Get degree data *****/
   Deg.DegCod = Crs.DegCod;
   Deg_GetDataOfDegreeByCod (&Deg);

   /***** Start row *****/
   fprintf (Gbl.F.Rep,"<li>");

   if (CrsCod > 0)	// CrsCod > 0 in log ==> course selected
     {
      if (Crs.CrsCod > 0)	// Course exists
	{
	 /***** Write course full name *****/
	 fprintf (Gbl.F.Rep,"<strong>%s</strong> -",Crs.FullName);

	 /***** Write year *****/
	 if (Crs.Year)
	    fprintf (Gbl.F.Rep," %s",Txt_YEAR_OF_DEGREE[Crs.Year]);

	 /***** Write degree full name *****/
	 fprintf (Gbl.F.Rep," %s",Deg.FullName);

	 /***** Write number of teachers / students in course *****/
	 if (WriteNumUsrs)
	    fprintf (Gbl.F.Rep," (%u %s / %u %s)",
		     Usr_GetNumUsrsInCrs (Rol_NET,Crs.CrsCod) +
		     Usr_GetNumUsrsInCrs (Rol_TCH,Crs.CrsCod),
		     Txt_teachers_ABBREVIATION,
		     Usr_GetNumUsrsInCrs (Rol_STD,Crs.CrsCod),
		     Txt_students_ABBREVIATION);
	}
      else
         fprintf (Gbl.F.Rep,"(%s)",Txt_unknown_removed_course);
     }
   else	// CrsCod <= 0 in log ==> no course selected
      fprintf (Gbl.F.Rep,"(%s)",Txt_no_course_selected);

   /***** Write hits per year for this course *****/
   fprintf (Gbl.F.Rep,"<br />");
   Rep_ShowMyHitsPerYear (false,CrsCod,Role,Report);

   fprintf (Gbl.F.Rep,"</li>");
  }

/*****************************************************************************/
/********************** Write my hits grouped by years ***********************/
/*****************************************************************************/

static void Rep_ShowMyHitsPerYear (bool AnyCourse,long CrsCod,Rol_Role_t Role,
                                   struct Rep_Report *Report)
  {
   char SubQueryCrs[128];
   char SubQueryRol[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned long NumRow;
   unsigned ReadYear;
   unsigned LastYear;
   unsigned Year;

   /***** Make the query *****/
   if (AnyCourse)
      SubQueryCrs[0] = '\0';
   else
      sprintf (SubQueryCrs," AND CrsCod=%ld",CrsCod);

   if (Role == Rol_UNK)	// Here Rol_UNK means any role
      SubQueryRol[0] = '\0';
   else
      sprintf (SubQueryRol," AND Role=%u",(unsigned) Role);

   NumRows = DB_QuerySELECT (&mysql_res,"can not get clicks",
			     "SELECT SQL_NO_CACHE "
			     "YEAR(CONVERT_TZ(ClickTime,@@session.time_zone,'UTC')) AS Year,"
			     "COUNT(*) FROM log_full"
			     " WHERE ClickTime>=FROM_UNIXTIME(%ld)"
			     " AND UsrCod=%ld%s%s"
			     " GROUP BY Year DESC",
			     (long) Report->UsrFigures.FirstClickTimeUTC,
			     Gbl.Usrs.Me.UsrDat.UsrCod,
			     SubQueryCrs,
			     SubQueryRol);

   /***** Initialize first year *****/
   Gbl.DateRange.DateIni.Date.Year = 1900 + Report->tm_FirstClickTime.tm_year;

   /***** Initialize LastYear *****/
   LastYear = Gbl.Now.Date.Year;

   /***** Set maximum number of hits per year *****/
   if (Report->MaxHitsPerYear)
      /* Set maximum number of hits per year from parameter */
      Report->Hits.Max = Report->MaxHitsPerYear;
   else
     {
      /* Compute maximum number of hits per year */
      Rep_ComputeMaxAndTotalHits (&Report->Hits,NumRows,mysql_res,1);
      mysql_data_seek (mysql_res,0);
     }

   /***** Write rows *****/
   for (NumRow = 1;
	NumRow <= NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get the year (in row[0] is the date in YYYY format) */
      if (sscanf (row[0],"%04u",&ReadYear) != 1)
	 Lay_ShowErrorAndExit ("Wrong date.");

      /* Get number hits (in row[1]) */
      if (sscanf (row[1],"%lu",&Report->Hits.Num) != 1)
	 Report->Hits.Num = 0;

      for (Year = LastYear;
	   Year >= ReadYear;
	   Year--)
        {
         /* Write the year */
         fprintf (Gbl.F.Rep,"%04u&nbsp;",Year);

         /* Draw bar proportional to number of hits */
         Rep_DrawBarNumHits (Year == ReadYear ? Report->Hits.Num :
                        	                0,
                             Report->Hits.Max,Rep_MAX_BAR_WIDTH);
         fprintf (Gbl.F.Rep,"<br />");
        }
      LastYear = Year;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Finally, show the oldest years without clicks *****/
   for (Year = LastYear;
        Year >= Gbl.DateRange.DateIni.Date.Year;
        Year--)
     {
      /* Write the year */
      fprintf (Gbl.F.Rep,"%04u&nbsp;",Year);

      /* Draw bar proportional to number of hits */
      Rep_DrawBarNumHits (0,Report->Hits.Max,Rep_MAX_BAR_WIDTH);
      fprintf (Gbl.F.Rep,"<br />");
     }
  }

/*****************************************************************************/
/*************** Compute maximum and total number of hits ********************/
/*****************************************************************************/

static void Rep_ComputeMaxAndTotalHits (struct Rep_Hits *Hits,
                                        unsigned long NumRows,
                                        MYSQL_RES *mysql_res,unsigned Field)
  {
   unsigned long NumRow;
   MYSQL_ROW row;

   /***** For each row... *****/
   for (NumRow = 1, Hits->Max = 0;
	NumRow <= NumRows;
	NumRow++)
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
         fprintf (Gbl.F.Rep,"<strong>");
	 for (i = 0;
	      i < BarWidth;
	      i++)
	    fprintf (Gbl.F.Rep,Rep_BLOCK);
         fprintf (Gbl.F.Rep,"</strong>");
	}

      /***** Write the number of hits *****/
      fprintf (Gbl.F.Rep,"&nbsp;");
      Str_WriteDoubleNumToFile (Gbl.F.Rep,HitsNum);
     }
  }

/*****************************************************************************/
/********** Remove all user's usage report of a user from database ***********/
/*****************************************************************************/

void Rep_RemoveUsrUsageReports (long UsrCod)
  {
   /***** Remove all user's usage report files of a user *****/
   Rep_RemoveUsrReportsFiles (UsrCod);

   /***** Remove all user's usage reports of a user from database *****/
   Rep_RemoveUsrReportsFromDB (UsrCod);
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
   NumReports =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get user's usage reports",
			      "SELECT UniqueDirL,UniqueDirR"
			      " FROM usr_report"
			      " WHERE UsrCod=%ld",
			      UsrCod);

   /***** Remove the reports *****/
   for (NumReport = 0;
	NumReport < NumReports;
	NumReport++)
     {
      /* Get next report */
      row = mysql_fetch_row (mysql_res);

      /* Remove report directory and file */
      snprintf (PathUniqueDirReport,sizeof (PathUniqueDirReport),
	        "%s/%s/%s",
	        Cfg_PATH_REP_PUBLIC,row[0],row[1]);
      Fil_RemoveTree (PathUniqueDirReport);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********** Remove all user's usage reports of a user from database **********/
/*****************************************************************************/

static void Rep_RemoveUsrReportsFromDB (long UsrCod)
  {
   /***** Insert a new user's usage report into database *****/
   DB_QueryDELETE ("can not remove user's usage reports",
		   "DELETE FROM usr_report WHERE UsrCod=%ld",
		   UsrCod);
  }
