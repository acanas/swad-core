// swad_report.c: report on my use of the platform

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#include "swad_database.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_profile.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Rep_FILENAME_ROOT "report"

#define Rep_MIN_CLICKS_CRS 100	// Minimum number of clicks to show a course in historic log
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

struct CurrentTimeUTC
  {
   char StrDate[10+1];	// Example: 2016-10-02
			//          1234567890
   char StrTime[8+1];	// Example: 19:03:49
			//          12345678
   unsigned Date;	// Example: 20161002
   unsigned Time;	// Example: 190349
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

static void Rep_CreateMyUsageReport (struct CurrentTimeUTC *CurrentTimeUTC,
                                     char *FilenameReport,
                                     char *Permalink);
static void Rep_PutLinkToMyUsageReport (struct CurrentTimeUTC *CurrentTimeUTC,
                                        const char *FilenameReport,
                                        const char *Permalink);
static void Req_TitleReport (struct CurrentTimeUTC *CurrentTimeUTC);

static void Rep_GetCurrentDateTimeUTC (struct tm *tm_CurrentTime,
                                       struct CurrentTimeUTC *CurrentTimeUTC);

static void Rep_CreateNewReportFile (const struct CurrentTimeUTC *CurrentTimeUTC,
                                     char *FilenameReport,char *Permalink);
static void Rep_CreateNewReportEntryIntoDB (const struct tm *tm_CurrentTime,
                                            const char *FilenameReport,
                                            const char *Permalink);
static void Rep_WriteHeader (const struct CurrentTimeUTC *CurrentTimeUTC,
                             const char *Permalink);
static void Rep_WriteSectionPlatform (void);
static void Rep_WriteSectionUsrInfo (void);
static void Rep_WriteSectionUsrFigures (const struct UsrFigures *UsrFigures,
                                        const struct tm *tm_FirstClickTime,
                                        const struct CurrentTimeUTC *CurrentTimeUTC);
static void Rep_WriteSectionGlobalHits (const struct UsrFigures *UsrFigures,
                                        const struct tm *tm_FirstClickTime);
static void Rep_WriteSectionCurrentCourses (const struct UsrFigures *UsrFigures,
                                            const struct tm *tm_FirstClickTime,
                                            const struct CurrentTimeUTC *CurrentTimeUTC,
                                            unsigned long MaxHitsPerYear);
static void Rep_WriteSectionHistoricCourses (const struct UsrFigures *UsrFigures,
                                             const struct tm *tm_FirstClickTime,
                                             unsigned long MaxHitsPerYear);

static unsigned long Rep_GetMaxHitsPerYear (time_t FirstClickTimeUTC);
static void Rep_GetAndWriteCurrentCrssOfAUsr (const struct UsrData *UsrDat,Rol_Role_t Role,
                                              time_t FirstClickTimeUTC,
                                              const struct tm *tm_FirstClickTime,
                                              unsigned long MaxHitsPerYear);
static void Rep_GetAndWriteHistoricCrssOfAUsr (const struct UsrData *UsrDat,Rol_Role_t Role,
                                               time_t FirstClickTimeUTC,
                                               const struct tm *tm_FirstClickTime,
                                               unsigned long MaxHitsPerYear);
static void Rep_WriteRowCrsData (long CrsCod,Rol_Role_t Role,
                                 time_t FirstClickTimeUTC,
                                 const struct tm *tm_FirstClickTime,
                                 unsigned long MaxHitsPerYear);

static void Rep_ShowMyHitsPerYear (bool AnyCourse,long CrsCod,Rol_Role_t Role,
                                   time_t FirstClickTimeUTC,
                                   const struct tm *tm_FirstClickTime,
                                   unsigned long MaxHitsPerYear);
static void Rep_DrawBarNumHits (float HitsNum,float HitsMax,
                                unsigned MaxBarWidth);

static void Rep_RemoveUsrReportsFiles (long UsrCod);
static void Rep_RemoveUsrReportsFromDB (long UsrCod);

/*****************************************************************************/
/******* Request my usage report (report on my use of the platform) **********/
/*****************************************************************************/

void Rep_ReqMyUsageReport (void)
  {
   extern const char *Txt_Report_of_use_of_PLATFORM;
   extern const char *Txt_Generate_report;

   /***** Form to show my usage report *****/
   Act_FormStart (ActSeeMyUsgRep);

   /***** Start frame *****/
   sprintf (Gbl.Title,Txt_Report_of_use_of_PLATFORM,Cfg_PLATFORM_SHORT_NAME);
   Lay_StartRoundFrame (NULL,Gbl.Title,NULL);

   /***** Header *****/
   Req_TitleReport (NULL);	// NULL means do not write date

   /***** Send button and end frame *****/
   Lay_EndRoundFrameWithButton (Lay_CONFIRM_BUTTON,Txt_Generate_report);

   /***** End of form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/********* Show my usage report (report on my use of the platform) ***********/
/*****************************************************************************/

void Rep_ShowMyUsageReport (void)
  {
   struct CurrentTimeUTC CurrentTimeUTC;
   char FilenameReport[NAME_MAX+1];
   char Permalink[PATH_MAX+1];

   /***** Create my usage report *****/
   Rep_CreateMyUsageReport (&CurrentTimeUTC,FilenameReport,Permalink);

   /***** Put link to my usage report *****/
   Rep_PutLinkToMyUsageReport (&CurrentTimeUTC,FilenameReport,Permalink);
  }

/*****************************************************************************/
/******** Create my usage report (report on my use of the platform) **********/
/*****************************************************************************/

static void Rep_CreateMyUsageReport (struct CurrentTimeUTC *CurrentTimeUTC,
                                     char *FilenameReport,
                                     char *Permalink)
  {
   extern const char *Txt_Report_of_use_of_PLATFORM;
   struct tm tm_CurrentTime;
   struct UsrFigures UsrFigures;
   struct tm tm_FirstClickTime;
   bool GetUsrFiguresAgain;
   unsigned long MaxHitsPerYear;

   /***** Get current date-time *****/
   Rep_GetCurrentDateTimeUTC (&tm_CurrentTime,CurrentTimeUTC);

   /***** Create a new report file *****/
   Rep_CreateNewReportFile (CurrentTimeUTC,FilenameReport,Permalink);

   /***** Store report entry into database *****/
   Rep_CreateNewReportEntryIntoDB (&tm_CurrentTime,FilenameReport,Permalink);

   /***** Start file *****/
   Lay_StartHTMLFile (Gbl.F.Rep,FilenameReport);
   fprintf (Gbl.F.Rep,"<body style=\"margin:1em;"
	              " text-align:left;"
	              " font-family:Helvetica,Arial,sans-serif;\">\n");

   /***** Header *****/
   Rep_WriteHeader (CurrentTimeUTC,Permalink);

   /***** Platform *****/
   Rep_WriteSectionPlatform ();

   /***** Personal information *****/
   Rep_WriteSectionUsrInfo ();

   /***** Figures *****/
   Prf_GetUsrFigures (Gbl.Usrs.Me.UsrDat.UsrCod,&UsrFigures);
   GetUsrFiguresAgain = Prf_GetAndStoreAllUsrFigures (Gbl.Usrs.Me.UsrDat.UsrCod,&UsrFigures);
   if (GetUsrFiguresAgain)
      Prf_GetUsrFigures (Gbl.Usrs.Me.UsrDat.UsrCod,&UsrFigures);
   if (UsrFigures.FirstClickTimeUTC)
      gmtime_r (&UsrFigures.FirstClickTimeUTC,&tm_FirstClickTime);
   Rep_WriteSectionUsrFigures (&UsrFigures,&tm_FirstClickTime,CurrentTimeUTC);

   /***** Global hits *****/
   Rep_WriteSectionGlobalHits (&UsrFigures,&tm_FirstClickTime);

   /***** Current courses *****/
   MaxHitsPerYear = Rep_GetMaxHitsPerYear (UsrFigures.FirstClickTimeUTC);
   Rep_WriteSectionCurrentCourses (&UsrFigures,&tm_FirstClickTime,
                                   CurrentTimeUTC,MaxHitsPerYear);

   /***** Historic courses *****/
   Rep_WriteSectionHistoricCourses (&UsrFigures,&tm_FirstClickTime,
                                    MaxHitsPerYear);

   /***** End file *****/
   fprintf (Gbl.F.Rep,"</body>\n"
	              "</html>\n");

   /***** Close report file *****/
   Fil_CloseReportFile ();
  }

/*****************************************************************************/
/******* Put link to my usage report (report on my use of the platform) ******/
/*****************************************************************************/

static void Rep_PutLinkToMyUsageReport (struct CurrentTimeUTC *CurrentTimeUTC,
                                        const char *FilenameReport,
                                        const char *Permalink)
  {
   extern const char *Txt_Report_of_use_of_PLATFORM;
   extern const char *Txt_Report;

   /***** Start frame *****/
   sprintf (Gbl.Title,Txt_Report_of_use_of_PLATFORM,Cfg_PLATFORM_SHORT_NAME);
   Lay_StartRoundFrame (NULL,Gbl.Title,NULL);

   /***** Header *****/
   Req_TitleReport (CurrentTimeUTC);

   /***** Put anchor and report filename *****/
   fprintf (Gbl.F.Out,"<div class=\"FILENAME CENTER_MIDDLE\">"
                      "<a href=\"%s\" class=\"FILENAME\""
		      " title=\"%s\" target=\"_blank\">"
                      "<img src=\"%s/report64x64.png\" alt=\"%s\""
	              " class=\"ICON64x64\" /><br />"
                      "%s"
		      "</a>"
		      "</div>",
	    Permalink,
	    Txt_Report,
            Gbl.Prefs.IconsURL,
            Txt_Report,
	    FilenameReport);

   /***** End frame *****/
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/********************* Write title of user's usage report ********************/
/*****************************************************************************/
// CurrentTimeUTC == NULL ==> do not write date

static void Req_TitleReport (struct CurrentTimeUTC *CurrentTimeUTC)
  {
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_Date;

   fprintf (Gbl.F.Out,"<div class=\"DAT\" style=\"margin-bottom:10px;\">");

   /***** User *****/
   fprintf (Gbl.F.Out,"%s: <span class=\"DAT_N_BOLD\">%s</span>",
	    Txt_User[Gbl.Usrs.Me.UsrDat.Sex],
	    Gbl.Usrs.Me.UsrDat.FullName);

   /***** Report date *****/
   if (CurrentTimeUTC)
      fprintf (Gbl.F.Out,"<br />"
			 "%s: <span class=\"DAT_N\">%s %s UTC</span>",
	       Txt_Date,
	       CurrentTimeUTC->StrDate,
	       CurrentTimeUTC->StrTime);

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********************* Get current date and time in UTC **********************/
/*****************************************************************************/

static void Rep_GetCurrentDateTimeUTC (struct tm *tm_CurrentTime,
                                       struct CurrentTimeUTC *CurrentTimeUTC)
  {
   time_t CurrentTime;

   /***** Initialize to empty strings *****/
   CurrentTimeUTC->StrDate[0] = '\0';
   CurrentTimeUTC->StrTime[0] = '\0';

   /***** Get current time UTC *****/
   time (&CurrentTime);
   if ((gmtime_r (&CurrentTime,tm_CurrentTime)) != NULL)
     {
      /* Date and time as strings */
      sprintf (CurrentTimeUTC->StrDate,"%04d-%02d-%02d",
	       1900 + tm_CurrentTime->tm_year,	// year
	       1 + tm_CurrentTime->tm_mon,	// month
	       tm_CurrentTime->tm_mday);	// day of the month
      sprintf (CurrentTimeUTC->StrTime,"%02d:%02d:%02d",
	       tm_CurrentTime->tm_hour,		// hours
	       tm_CurrentTime->tm_min,		// minutes
	       tm_CurrentTime->tm_sec);		// seconds

      /* Date and time as unsigned */
      CurrentTimeUTC->Date = (1900 + tm_CurrentTime->tm_year) * 10000 +
	                     (1 + tm_CurrentTime->tm_mon)     * 100   +
	                     tm_CurrentTime->tm_mday;
      CurrentTimeUTC->Time = tm_CurrentTime->tm_hour          * 10000 +
	                     tm_CurrentTime->tm_min           * 100   +
	                     tm_CurrentTime->tm_sec;
     }
  }

/*****************************************************************************/
/*************** Create a new file for user's usage report *******************/
/*****************************************************************************/

static void Rep_CreateNewReportFile (const struct CurrentTimeUTC *CurrentTimeUTC,
                                     char *FilenameReport,char *Permalink)
  {
   char PathReports[PATH_MAX+1];
   char PathUniqueDirL[PATH_MAX+1];
   char PathUniqueDirR[PATH_MAX+1];
   char PathFileReport[PATH_MAX+1];

   /***** Path for reports *****/
   sprintf (PathReports,"%s/%s",
            Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_REP);
   Fil_CreateDirIfNotExists (PathReports);

   /***** Unique directory for the file with the report *****/
   /* 1. Create a directory using the leftmost 2 chars of a unique name */
   sprintf (PathUniqueDirL,"%s/%s/%c%c",
            Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_REP,
            Gbl.UniqueNameEncrypted[0],
            Gbl.UniqueNameEncrypted[1]);
   Fil_CreateDirIfNotExists (PathUniqueDirL);

   /* 2. Create a directory using the rightmost 41 chars of a unique name */
   sprintf (PathUniqueDirR,"%s/%s",
            PathUniqueDirL,
            &Gbl.UniqueNameEncrypted[2]);
   if (mkdir (PathUniqueDirR,(mode_t) 0xFFF))
      Lay_ShowErrorAndExit ("Can not create directory for report.");

   /***** Path of the public file with the report */
   sprintf (FilenameReport,"%s_%06u_%06u.html",
            Rep_FILENAME_ROOT,CurrentTimeUTC->Date,CurrentTimeUTC->Time);
   sprintf (PathFileReport,"%s/%s",
            PathUniqueDirR,FilenameReport);
   if ((Gbl.F.Rep = fopen (PathFileReport,"wb")) == NULL)
      Lay_ShowErrorAndExit ("Can not create report file.");

   /***** Permalink *****/
   sprintf (Permalink,"%s/%s/%c%c/%s/%s",
            Cfg_URL_SWAD_PUBLIC,
            Cfg_FOLDER_REP,
            Gbl.UniqueNameEncrypted[0],
            Gbl.UniqueNameEncrypted[1],
            &Gbl.UniqueNameEncrypted[2],
            FilenameReport);
  }

/*****************************************************************************/
/************** Insert a new user's usage report into database ***************/
/*****************************************************************************/

static void Rep_CreateNewReportEntryIntoDB (const struct tm *tm_CurrentTime,
                                            const char *FilenameReport,
                                            const char *Permalink)
  {
   char Query[1024+PATH_MAX*2];

   /***** Insert a new user's usage report into database *****/
   sprintf (Query,"INSERT INTO usr_report"
	          " (UsrCod,ReportTimeUTC,"
	          "UniqueDirL,UniqueDirR,Filename,Permalink)"
                  " VALUES"
                  " ('%ld','%04d-%02d-%02d %02d:%02d:%02d',"
                  "'%c%c','%s','%s','%s')",
            Gbl.Usrs.Me.UsrDat.UsrCod,
	    1900 + tm_CurrentTime->tm_year,	// year
	    1 + tm_CurrentTime->tm_mon,		// month
	    tm_CurrentTime->tm_mday,		// day of the month
	    tm_CurrentTime->tm_hour,		// hours
	    tm_CurrentTime->tm_min,		// minutes
	    tm_CurrentTime->tm_sec,		// seconds
            Gbl.UniqueNameEncrypted[0],		//  2  leftmost chars from a unique 43 chars base64url codified from a unique SHA-256 string
            Gbl.UniqueNameEncrypted[1],
            &Gbl.UniqueNameEncrypted[2],	// 41 rightmost chars from a unique 43 chars base64url codified from a unique SHA-256 string
            FilenameReport,Permalink);
   DB_QueryINSERT (Query,"can not create new user's usage report");
  }

/*****************************************************************************/
/******************** Write header of user's usage report ********************/
/*****************************************************************************/

static void Rep_WriteHeader (const struct CurrentTimeUTC *CurrentTimeUTC,
                             const char *Permalink)
  {
   extern const char *Txt_Report_of_use_of_PLATFORM;
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_Date;
   extern const char *Txt_Permalink;

   /***** Start of header *****/
   fprintf (Gbl.F.Rep,"<header>");

   /***** Main title *****/
   sprintf (Gbl.Title,Txt_Report_of_use_of_PLATFORM,Cfg_PLATFORM_SHORT_NAME);
   fprintf (Gbl.F.Rep,"<h1>%s</h1>"
	              "<ul>",
	    Gbl.Title);

   /***** User *****/
   fprintf (Gbl.F.Rep,"<li>%s: <strong>%s</strong></li>",
	    Txt_User[Gbl.Usrs.Me.UsrDat.Sex],
	    Gbl.Usrs.Me.UsrDat.FullName);

   /***** Date-time *****/
   fprintf (Gbl.F.Rep,"<li>%s: %s %s UTC</li>",
            Txt_Date,
	    CurrentTimeUTC->StrDate,
	    CurrentTimeUTC->StrTime);

   /***** Permalink *****/
   fprintf (Gbl.F.Rep,"<li>%s: "
	              "<a href=\"%s\" target=\"_blank\""
		      " style=\"text-decoration:none;\">"
	              "%s"
	              "</a>"
	              "</li>",
            Txt_Permalink,
            Permalink,Permalink);

   /***** End of header *****/
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

   /***** Start of section *****/
   fprintf (Gbl.F.Rep,"<section>"
	              "<h3>%s</h3>"
	              "<ul>",
	    Txt_Teaching_platform);

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

   /***** End of section *****/
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
   char CtyName[Cty_MAX_BYTES_COUNTRY_NAME+1];
   struct Institution Ins;

   /***** Start of section *****/
   fprintf (Gbl.F.Rep,"<section>"
	              "<h3>%s</h3>"
	              "<ul>",
	    Txt_Personal_information);

   /***** User's name *****/
   fprintf (Gbl.F.Rep,"<li>%s: %s</li>",
            Txt_Name,
            Gbl.Usrs.Me.UsrDat.FullName);

   /***** User's e-mail *****/
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

   /***** End of section *****/
   fprintf (Gbl.F.Rep,"</ul>"
	              "</section>\n");
  }

/*****************************************************************************/
/********* Write section for user's figures in user's usage report ***********/
/*****************************************************************************/

static void Rep_WriteSectionUsrFigures (const struct UsrFigures *UsrFigures,
                                        const struct tm *tm_FirstClickTime,
                                        const struct CurrentTimeUTC *CurrentTimeUTC)
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
   extern const char *Txt_post;
   extern const char *Txt_posts;
   extern const char *Txt_Messages_sent;
   extern const char *Txt_message;
   extern const char *Txt_messages;
   unsigned NumFiles;
   unsigned NumPublicFiles;

   /***** Start of section *****/
   fprintf (Gbl.F.Rep,"<section>"
                      "<h3>%s</h3>"
	              "<ul>",
	    Txt_Figures);

   /***** Time since first click until now *****/
   fprintf (Gbl.F.Rep,"<li>%s ",Txt_TIME_Since);
   if (UsrFigures->FirstClickTimeUTC)
     {
      if (tm_FirstClickTime != NULL)
	{
	 fprintf (Gbl.F.Rep,"%04d-%02d-%02d %02d:%02d:%02d UTC",
                  1900 + tm_FirstClickTime->tm_year,	// year
                  1 + tm_FirstClickTime->tm_mon,	// month
                  tm_FirstClickTime->tm_mday,		// day of the month
                  tm_FirstClickTime->tm_hour,		// hours
                  tm_FirstClickTime->tm_min,		// minutes
		  tm_FirstClickTime->tm_sec);		// seconds
	 if (CurrentTimeUTC->StrDate[0])
	    fprintf (Gbl.F.Rep," %s %s %s UTC",
	             Txt_TIME_until,
	             CurrentTimeUTC->StrDate,
	             CurrentTimeUTC->StrTime);
	 if (UsrFigures->NumDays > 0)
	    fprintf (Gbl.F.Rep," (%d %s)",
		     UsrFigures->NumDays,
		     (UsrFigures->NumDays == 1) ? Txt_day :
						  Txt_days);
	}
     }
   else	// Time of first click is unknown
     {
      fprintf (Gbl.F.Rep,"?");
      if (CurrentTimeUTC->StrDate[0])
         fprintf (Gbl.F.Rep," - %s %s UTC",
                  CurrentTimeUTC->StrDate,
                  CurrentTimeUTC->StrTime);
     }
   fprintf (Gbl.F.Rep,"</li>");

   /***** Number of clicks *****/
   fprintf (Gbl.F.Rep,"<li>%s: ",Txt_Clicks);
   if (UsrFigures->NumClicks >= 0)
     {
      fprintf (Gbl.F.Rep,"%ld",UsrFigures->NumClicks);
      if (UsrFigures->NumDays > 0)
	{
	 fprintf (Gbl.F.Rep," (");
	 Str_WriteFloatNum (Gbl.F.Rep,
	                    (float) UsrFigures->NumClicks /
			    (float) UsrFigures->NumDays);
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
   if (UsrFigures->NumFileViews >= 0)
     {
      fprintf (Gbl.F.Rep,"%ld %s",
               UsrFigures->NumFileViews,
	       (UsrFigures->NumFileViews == 1) ? Txt_download :
						 Txt_downloads);
      if (UsrFigures->NumDays > 0)
	{
	 fprintf (Gbl.F.Rep," (");
	 Str_WriteFloatNum (Gbl.F.Rep,
	                    (float) UsrFigures->NumFileViews /
			    (float) UsrFigures->NumDays);
	 fprintf (Gbl.F.Rep," / %s)",Txt_day);
	}
     }
   else	// Number of file views is unknown
      fprintf (Gbl.F.Rep,"?");
   fprintf (Gbl.F.Rep,"</li>");

   /***** Number of posts in forums *****/
   fprintf (Gbl.F.Rep,"<li>%s: ",Txt_Forum_posts);
   if (UsrFigures->NumForPst >= 0)
     {
      fprintf (Gbl.F.Rep,"%ld %s",
	       UsrFigures->NumForPst,
	       (UsrFigures->NumForPst == 1) ? Txt_post :
					      Txt_posts);
      if (UsrFigures->NumDays > 0)
	{
	 fprintf (Gbl.F.Rep," (");
	 Str_WriteFloatNum (Gbl.F.Rep,
	                    (float) UsrFigures->NumForPst /
			    (float) UsrFigures->NumDays);
	 fprintf (Gbl.F.Rep," / %s)",Txt_day);
	}
     }
   else	// Number of forum posts is unknown
      fprintf (Gbl.F.Rep,"?");
   fprintf (Gbl.F.Rep,"</li>");

   /***** Number of messages sent *****/
   fprintf (Gbl.F.Rep,"<li>%s: ",Txt_Messages_sent);
   if (UsrFigures->NumMsgSnt >= 0)
     {
      fprintf (Gbl.F.Rep,"%ld %s",
	       UsrFigures->NumMsgSnt,
	       (UsrFigures->NumMsgSnt == 1) ? Txt_message :
					      Txt_messages);
      if (UsrFigures->NumDays > 0)
	{
	 fprintf (Gbl.F.Rep," (");
	 Str_WriteFloatNum (Gbl.F.Rep,
	                    (float) UsrFigures->NumMsgSnt /
			    (float) UsrFigures->NumDays);
	 fprintf (Gbl.F.Rep," / %s)",Txt_day);
	}
     }
   else	// Number of messages sent is unknown
      fprintf (Gbl.F.Rep,"?");
   fprintf (Gbl.F.Rep,"</li>");

   /***** End of section *****/
   fprintf (Gbl.F.Rep,"</ul>"
	              "</section>\n");
  }

/*****************************************************************************/
/******** Write section for user's global hits in user's usage report ********/
/*****************************************************************************/

static void Rep_WriteSectionGlobalHits (const struct UsrFigures *UsrFigures,
                                        const struct tm *tm_FirstClickTime)
  {
   extern const char *Txt_Hits;

   /***** Start of section *****/
   fprintf (Gbl.F.Rep,"<section>"
                      "<h3>%s</h3>",
	    Txt_Hits);

   /***** Global (in any course) hits per year *****/
   Rep_ShowMyHitsPerYear (true,-1L,	// Any course
                          Rol_UNKNOWN,	// Any role
                          UsrFigures->FirstClickTimeUTC,
                          tm_FirstClickTime,
                          0);	// MaxHitsPerYear not passed as an argument but computed inside the function

   /***** End of section *****/
   fprintf (Gbl.F.Rep,"</section>\n");
  }

/*****************************************************************************/
/****** Write section for user's current courses in user's usage report ******/
/*****************************************************************************/

static void Rep_WriteSectionCurrentCourses (const struct UsrFigures *UsrFigures,
                                            const struct tm *tm_FirstClickTime,
                                            const struct CurrentTimeUTC *CurrentTimeUTC,
                                            unsigned long MaxHitsPerYear)
  {
   extern const char *Txt_Courses;
   Rol_Role_t Role;

   /***** Start of section *****/
   fprintf (Gbl.F.Rep,"<section>"
                      "<h3>%s",
            Txt_Courses);
   if (CurrentTimeUTC->StrDate[0])
      fprintf (Gbl.F.Rep," (%s)",CurrentTimeUTC->StrDate);
   fprintf (Gbl.F.Rep,"</h3>"
	              "<ul>");

   /***** Number of courses in which the user is student/teacher *****/
   for (Role  = Rol_STUDENT;
	Role <= Rol_TEACHER;
	Role++)
      /* List my courses with this role */
      Rep_GetAndWriteCurrentCrssOfAUsr (&Gbl.Usrs.Me.UsrDat,Role,
					UsrFigures->FirstClickTimeUTC,tm_FirstClickTime,
					MaxHitsPerYear);

   /***** End of section *****/
   fprintf (Gbl.F.Rep,"</ul>"
	              "</section>\n");
  }

/*****************************************************************************/
/***** Write section for user's historic courses in user's usage report ******/
/*****************************************************************************/

static void Rep_WriteSectionHistoricCourses (const struct UsrFigures *UsrFigures,
                                             const struct tm *tm_FirstClickTime,
                                             unsigned long MaxHitsPerYear)
  {
   extern const char *Txt_Courses;
   extern const char *Txt_historical_log;
   extern const char *Txt_Only_courses_with_more_than_X_clicks_are_shown;
   Rol_Role_t Role;

   /***** Start of section *****/
   fprintf (Gbl.F.Rep,"<section>"
                      "<h3>%s (%s)</h3>",
	    Txt_Courses,Txt_historical_log);
   fprintf (Gbl.F.Rep,Txt_Only_courses_with_more_than_X_clicks_are_shown,
            Rep_MIN_CLICKS_CRS);
   fprintf (Gbl.F.Rep,"<ul>");

   /***** Number of courses in which the user clicked as student/teacher *****/
   for (Role  = Rol_STUDENT;
	Role <= Rol_TEACHER;
	Role++)
      /* List my courses with this role */
      Rep_GetAndWriteHistoricCrssOfAUsr (&Gbl.Usrs.Me.UsrDat,Role,
					 UsrFigures->FirstClickTimeUTC,tm_FirstClickTime,
					 MaxHitsPerYear);

   /***** End of section *****/
   fprintf (Gbl.F.Rep,"</ul>"
	              "</section>\n");
  }

/*****************************************************************************/
/************ Get the maximum number of hits per course-year-role ************/
/*****************************************************************************/
// Return the maximum number of hits per year

static unsigned long Rep_GetMaxHitsPerYear (time_t FirstClickTimeUTC)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long MaxHitsPerYear = 0;

   sprintf (Query,"SELECT MAX(N) FROM (SELECT "
	          "CrsCod,"
	          "YEAR(CONVERT_TZ(ClickTime,@@session.time_zone,'UTC')) AS Year,"
	          "Role,"
	          "COUNT(*) AS N"
	          " FROM log_full"
	          " WHERE ClickTime>=FROM_UNIXTIME('%ld')"
	          " AND UsrCod='%ld' AND Role>='%u' AND Role<='%u'"
	          " GROUP BY CrsCod,Year,Role)"
	          " AS hits_per_crs_year",
            (long) FirstClickTimeUTC,
	    Gbl.Usrs.Me.UsrDat.UsrCod,
	    (unsigned) Rol_STUDENT,
	    (unsigned) Rol_TEACHER);
   DB_QuerySELECT (Query,&mysql_res,"can not get last question index");

   /***** Get number of users *****/
   row = mysql_fetch_row (mysql_res);
   if (row[0])	// There are questions
      if (sscanf (row[0],"%lu",&MaxHitsPerYear) != 1)
         Lay_ShowErrorAndExit ("Error when getting maximum hits.");

   return MaxHitsPerYear;
  }

/*****************************************************************************/
/************************** Write courses of a user **************************/
/*****************************************************************************/

static void Rep_GetAndWriteCurrentCrssOfAUsr (const struct UsrData *UsrDat,Rol_Role_t Role,
                                              time_t FirstClickTimeUTC,
                                              const struct tm *tm_FirstClickTime,
                                              unsigned long MaxHitsPerYear)
  {
   extern const char *Txt_USER_in_COURSE;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_course;
   extern const char *Txt_courses;
   extern const char *Txt_teachers_ABBREVIATION;
   extern const char *Txt_students_ABBREVIATION;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   long CrsCod;

   NumCrss = Usr_GetNumCrssOfUsrWithARole (UsrDat->UsrCod,Role);
   sprintf (Gbl.Title,Txt_USER_in_COURSE,Txt_ROLES_SINGUL_Abc[Role][Gbl.Usrs.Me.UsrDat.Sex]);
   fprintf (Gbl.F.Rep,"<li>%s %u %s",
	    Gbl.Title,
	    NumCrss,
	    NumCrss == 1 ? Txt_course :
			   Txt_courses);
   if (NumCrss)
     {
      fprintf (Gbl.F.Rep," (%u %s / %u %s):",
	       Usr_GetNumUsrsInCrssOfAUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Role,Rol_TEACHER),
	       Txt_teachers_ABBREVIATION,
	       Usr_GetNumUsrsInCrssOfAUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Role,Rol_STUDENT),
	       Txt_students_ABBREVIATION);

      /***** Get courses of a user from database *****/
      sprintf (Query,"SELECT courses.CrsCod,degrees.FullName,courses.Year,courses.FullName"
		     " FROM crs_usr,courses,degrees"
		     " WHERE crs_usr.UsrCod='%ld'"
		     " AND crs_usr.Role='%u'"
		     " AND crs_usr.CrsCod=courses.CrsCod"
		     " AND courses.DegCod=degrees.DegCod"
		     " ORDER BY degrees.FullName,courses.Year,courses.FullName",
	       UsrDat->UsrCod,(unsigned) Role);

      /***** List the courses (one row per course) *****/
      if ((NumCrss = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get courses of a user")))
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
	    Rep_WriteRowCrsData (CrsCod,Role,
				 FirstClickTimeUTC,tm_FirstClickTime,
				 MaxHitsPerYear);
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
/************************** Write courses of a user **************************/
/*****************************************************************************/

static void Rep_GetAndWriteHistoricCrssOfAUsr (const struct UsrData *UsrDat,Rol_Role_t Role,
                                               time_t FirstClickTimeUTC,
                                               const struct tm *tm_FirstClickTime,
                                               unsigned long MaxHitsPerYear)
  {
   extern const char *Txt_Hits_as_a_USER;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   long CrsCod;

   /***** Get courses of a user from database *****/
   sprintf (Query,"SELECT CrsCod,COUNT(*) AS N"
	          " FROM log_full"
	          " WHERE UsrCod='%ld' AND Role='%u'"
                  " GROUP BY CrsCod"
	          " HAVING N>'%u'"
                  " ORDER BY N DESC",
            UsrDat->UsrCod,(unsigned) Role,
            Rep_MIN_CLICKS_CRS);

   /***** List the courses (one row per course) *****/
   if ((NumCrss = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get courses of a user")))
     {
      /* Heading row */
      sprintf (Gbl.Title,Txt_Hits_as_a_USER,
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
         Rep_WriteRowCrsData (CrsCod,Role,
                              FirstClickTimeUTC,tm_FirstClickTime,
                              MaxHitsPerYear);
        }

      /* End of list */
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
                                 time_t FirstClickTimeUTC,
                                 const struct tm *tm_FirstClickTime,
                                 unsigned long MaxHitsPerYear)
  {
   extern const char *Txt_YEAR_OF_DEGREE[1+Deg_MAX_YEARS_PER_DEGREE];
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
	 fprintf (Gbl.F.Rep," (%u %s / %u %s)",
		  Usr_GetNumUsrsInCrs (Rol_TEACHER,Crs.CrsCod),Txt_teachers_ABBREVIATION,
		  Usr_GetNumUsrsInCrs (Rol_STUDENT,Crs.CrsCod),Txt_students_ABBREVIATION);
	}
      else
         fprintf (Gbl.F.Rep,"(%s)",Txt_unknown_removed_course);
     }
   else	// CrsCod <= 0 in log ==> no course selected
      fprintf (Gbl.F.Rep,"(%s)",Txt_no_course_selected);

   /***** Write hits per year for this course *****/
   fprintf (Gbl.F.Rep,"<br />");
   Rep_ShowMyHitsPerYear (false,CrsCod,Role,
                          FirstClickTimeUTC,tm_FirstClickTime,
                          MaxHitsPerYear);

   fprintf (Gbl.F.Rep,"</li>");
  }

/*****************************************************************************/
/********************** Write my hits grouped by years ***********************/
/*****************************************************************************/

static void Rep_ShowMyHitsPerYear (bool AnyCourse,long CrsCod,Rol_Role_t Role,
                                   time_t FirstClickTimeUTC,
                                   const struct tm *tm_FirstClickTime,
                                   unsigned long MaxHitsPerYear)
  {
   char Query[512];
   char SubQueryCrs[128];
   char SubQueryRol[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned long NumRow;
   unsigned ReadYear;
   unsigned LastYear;
   unsigned Year;
   struct Sta_Hits Hits;

   /***** Make the query *****/
   if (AnyCourse)
      SubQueryCrs[0] = '\0';
   else
      sprintf (SubQueryCrs," AND CrsCod='%ld'",CrsCod);

   if (Role == Rol_UNKNOWN)	// Here Rol_UNKNOWN means any role
      SubQueryRol[0] = '\0';
   else
      sprintf (SubQueryRol," AND Role='%u'",(unsigned) Role);

   sprintf (Query,"SELECT SQL_NO_CACHE "
		  "YEAR(CONVERT_TZ(ClickTime,@@session.time_zone,'UTC')) AS Year,"
		  "COUNT(*) FROM log_full"
                  " WHERE ClickTime>=FROM_UNIXTIME('%ld')"
		  " AND UsrCod='%ld'%s%s"
		  " GROUP BY Year DESC",
            (long) FirstClickTimeUTC,
	    Gbl.Usrs.Me.UsrDat.UsrCod,
	    SubQueryCrs,
	    SubQueryRol);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get clicks");

   /***** Initialize first year *****/
   Gbl.DateRange.DateIni.Date.Year = 1900 + tm_FirstClickTime->tm_year;

   /***** Initialize LastYear *****/
   LastYear = Gbl.Now.Date.Year;

   /***** Set maximum number of hits per year *****/
   if (MaxHitsPerYear)
      /* Set maximum number of hits per year from parameter */
      Hits.Max = (float) MaxHitsPerYear;
   else
     {
      /* Compute maximum number of hits per year */
      Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,1,1);
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
      Hits.Num = Str_GetFloatNumFromStr (row[1]);

      for (Year = LastYear;
	   Year >= ReadYear;
	   Year--)
        {
         /* Write the year */
         fprintf (Gbl.F.Rep,"%04u&nbsp;",Year);

         /* Draw bar proportional to number of hits */
         Rep_DrawBarNumHits (Year == LastYear ? Hits.Num :
                        	                0.0,
                             Hits.Max,Rep_MAX_BAR_WIDTH);
        }
      LastYear = Year;
     }

   /***** Finally, show the oldest years without clicks *****/
   for (Year = LastYear;
        Year >= Gbl.DateRange.DateIni.Date.Year;
        Year--)
     {
      /* Write the year */
      fprintf (Gbl.F.Rep,"%04u&nbsp;",Year);

      /* Draw bar proportional to number of hits */
      Rep_DrawBarNumHits (0.0,Hits.Max,Rep_MAX_BAR_WIDTH);
     }
  }

/*****************************************************************************/
/********************* Draw a bar with the number of hits ********************/
/*****************************************************************************/

static void Rep_DrawBarNumHits (float HitsNum,float HitsMax,
                                unsigned MaxBarWidth)
  {
   unsigned BarWidth;
   unsigned i;

   if (HitsNum != 0.0)
     {
      /***** Draw bar with a with proportional to the number of hits *****/
      BarWidth = (unsigned) (((HitsNum * (float) MaxBarWidth) / HitsMax) + 0.5);
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
      Str_WriteFloatNum (Gbl.F.Rep,HitsNum);
     }
   fprintf (Gbl.F.Rep,"<br />");
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
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumReports;
   unsigned NumReport;
   char PathUniqueDirReport[PATH_MAX+1];

   /***** Get directories for the reports *****/
   sprintf (Query,"SELECT UniqueDirL,UniqueDirR FROM usr_report"
	          " WHERE UsrCod='%ld'",
	    UsrCod);
   NumReports = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get user's usage reports");

   /***** Remove the reports *****/
   for (NumReport = 0;
	NumReport < NumReports;
	NumReport++)
     {
      /* Get next report */
      row = mysql_fetch_row (mysql_res);

      /* Remove report directory and file */
      sprintf (PathUniqueDirReport,"%s/%s/%s/%s",
	       Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_REP,row[0],row[1]);
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
   char Query[128];

   /***** Insert a new user's usage report into database *****/
   sprintf (Query,"DELETE FROM usr_report WHERE UsrCod='%ld'",UsrCod);
   DB_QueryDELETE (Query,"can not remove user's usage reports");
  }
