// swad_statistic.c: statistics

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <math.h>		// For log10, floor, ceil, modf, sqrt...
#include <stdlib.h>		// For system, getenv, etc.
#include <string.h>		// For string functions
#include <sys/wait.h>		// For the macro WEXITSTATUS
#include <unistd.h>		// For unlink

#include "swad_action.h"
#include "swad_config.h"
#include "swad_course.h"
#include "swad_database.h"
#include "swad_file_browser.h"
#include "swad_follow.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_logo.h"
#include "swad_network.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_profile.h"
#include "swad_social.h"
#include "swad_statistic.h"
#include "swad_tab.h"
#include "swad_web_service.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Sta_SECONDS_IN_RECENT_LOG ((time_t) (Cfg_DAYS_IN_RECENT_LOG * 24UL * 60UL * 60UL))	// Remove entries in recent log oldest than this time

const unsigned Sta_CellPadding[Sta_NUM_CLICKS_GROUPED_BY] =
  {
   1,	// Sta_CLICKS_CRS_DETAILED_LIST

   1,	// Sta_CLICKS_CRS_PER_USR
   1,	// Sta_CLICKS_CRS_PER_DAYS
   0,	// Sta_CLICKS_CRS_PER_DAYS_AND_HOUR
   1,	// Sta_CLICKS_CRS_PER_WEEKS
   1,	// Sta_CLICKS_CRS_PER_MONTHS
   1,	// Sta_CLICKS_CRS_PER_HOUR
   0,	// Sta_CLICKS_CRS_PER_MINUTE
   1,	// Sta_CLICKS_CRS_PER_ACTION

   1,	// Sta_CLICKS_GBL_PER_DAYS
   0,	// Sta_CLICKS_GBL_PER_DAYS_AND_HOUR
   1,	// Sta_CLICKS_GBL_PER_WEEKS
   1,	// Sta_CLICKS_GBL_PER_MONTHS
   1,	// Sta_CLICKS_GBL_PER_HOUR
   0,	// Sta_CLICKS_GBL_PER_MINUTE
   1,	// Sta_CLICKS_GBL_PER_ACTION
   1,	// Sta_CLICKS_GBL_PER_PLUGIN
   1,	// Sta_CLICKS_GBL_PER_WEB_SERVICE_FUNCTION
   1,	// Sta_CLICKS_GBL_PER_BANNER
   1,	// Sta_CLICKS_GBL_PER_COUNTRY
   1,	// Sta_CLICKS_GBL_PER_INSTITUTION
   1,	// Sta_CLICKS_GBL_PER_CENTRE
   1,	// Sta_CLICKS_GBL_PER_DEGREE
   1,	// Sta_CLICKS_GBL_PER_COURSE
  };

#define Sta_STAT_RESULTS_SECTION_ID	"stat_results"

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct Sta_SizeOfFileZones
  {
   int NumCrss;	// -1 stands for not aplicable
   int NumGrps;	// -1 stands for not aplicable
   int NumUsrs;	// -1 stands for not aplicable
   unsigned MaxLevels;
   unsigned long NumFolders;
   unsigned long NumFiles;
   unsigned long long int Size;	// Total size in bytes
  };

struct Sta_StatsForum
  {
   unsigned NumForums;
   unsigned NumThreads;
   unsigned NumPosts;
   unsigned NumUsrsToBeNotifiedByEMail;
  };

typedef enum
  {
   Sta_SHOW_GLOBAL_ACCESSES,
   Sta_SHOW_COURSE_ACCESSES,
  } Sta_GlobalOrCourseAccesses_t;

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

static void Sta_WriteSelectorCountType (void);
static void Sta_WriteSelectorAction (void);
static void Sta_ShowHits (Sta_GlobalOrCourseAccesses_t GlobalOrCourse);
static void Sta_ShowDetailedAccessesList (unsigned long NumRows,MYSQL_RES *mysql_res);
static void Sta_WriteLogComments (long LogCod);
static void Sta_ShowNumHitsPerUsr (unsigned long NumRows,MYSQL_RES *mysql_res);
static void Sta_ShowNumHitsPerDays (unsigned long NumRows,MYSQL_RES *mysql_res);
static void Sta_ShowDistrAccessesPerDaysAndHour (unsigned long NumRows,MYSQL_RES *mysql_res);
static Sta_ColorType_t Sta_GetStatColorType (void);
static void Sta_DrawBarColors (Sta_ColorType_t ColorType,float HitsMax);
static void Sta_DrawAccessesPerHourForADay (Sta_ColorType_t ColorType,float HitsNum[24],float HitsMax);
static void Sta_SetColor (Sta_ColorType_t ColorType,float HitsNum,float HitsMax,
                          unsigned *R,unsigned *G,unsigned *B);
static void Sta_ShowNumHitsPerWeeks (unsigned long NumRows,
                                     MYSQL_RES *mysql_res);
static void Sta_ShowNumHitsPerMonths (unsigned long NumRows,
                                      MYSQL_RES *mysql_res);
static void Sta_ShowNumHitsPerHour (unsigned long NumRows,
                                    MYSQL_RES *mysql_res);
static void Sta_WriteAccessHour (unsigned Hour,struct Sta_Hits *Hits,unsigned ColumnWidth);
static void Sta_ShowAverageAccessesPerMinute (unsigned long NumRows,MYSQL_RES *mysql_res);
static void Sta_WriteLabelsXAxisAccMin (float IncX,const char *Format);
static void Sta_WriteAccessMinute (unsigned Minute,float HitsNum,float MaxX);
static void Sta_ShowNumHitsPerAction (unsigned long NumRows,
                                      MYSQL_RES *mysql_res);
static void Sta_ShowNumHitsPerPlugin (unsigned long NumRows,
                                      MYSQL_RES *mysql_res);
static void Sta_ShowNumHitsPerWSFunction (unsigned long NumRows,
                                          MYSQL_RES *mysql_res);
static void Sta_ShowNumHitsPerBanner (unsigned long NumRows,
                                      MYSQL_RES *mysql_res);
static void Sta_ShowNumHitsPerCountry (unsigned long NumRows,
                                       MYSQL_RES *mysql_res);
static void Sta_WriteCountry (long CtyCod);
static void Sta_ShowNumHitsPerInstitution (unsigned long NumRows,
                                           MYSQL_RES *mysql_res);
static void Sta_WriteInstitution (long InsCod);
static void Sta_ShowNumHitsPerCentre (unsigned long NumRows,
                                      MYSQL_RES *mysql_res);
static void Sta_WriteCentre (long CtrCod);
static void Sta_ShowNumHitsPerDegree (unsigned long NumRows,
                                      MYSQL_RES *mysql_res);
static void Sta_WriteDegree (long DegCod);
static void Sta_ShowNumHitsPerCourse (unsigned long NumRows,
                                      MYSQL_RES *mysql_res);

static void Sta_DrawBarNumHits (char Color,float HitsNum,float HitsMax,float HitsTotal,unsigned MaxBarWidth);

static void Sta_PutParamsToShowFigure (void);
static void Sta_PutHiddenParamFigureType (void);
static void Sta_PutHiddenParamScopeSta (void);

static void Sta_GetAndShowHierarchyStats (void);
static void Sta_WriteHeadDegsCrssInSWAD (void);
static void Sta_GetAndShowNumCtysInSWAD (void);
static void Sta_GetAndShowNumInssInSWAD (void);
static void Sta_GetAndShowNumCtrsInSWAD (void);
static void Sta_GetAndShowNumDegsInSWAD (void);
static void Sta_GetAndShowNumCrssInSWAD (void);

static void Sta_GetAndShowInstitutionsStats (void);
static void Sta_GetAndShowInssOrderedByNumCtrs (void);
static void Sta_GetAndShowInssOrderedByNumDegs (void);
static void Sta_GetAndShowInssOrderedByNumCrss (void);
static void Sta_GetAndShowInssOrderedByNumUsrsInCrss (void);
static void Sta_GetAndShowInssOrderedByNumUsrsWhoClaimToBelongToThem (void);
static void Sta_GetAndShowInss (const char *Query,const char *TxtFigure);
static unsigned Sta_GetInsAndStat (struct Instit *Ins,MYSQL_RES *mysql_res);

static void Sta_GetAndShowDegreeTypesStats (void);

static void Sta_GetAndShowUsersStats (void);
static void Sta_GetAndShowNumUsrsInCrss (Rol_Role_t Role);
static void Sta_GetAndShowNumUsrsNotBelongingToAnyCrs (void);

static void Sta_GetAndShowUsersRanking (void);

static void Sta_GetAndShowFileBrowsersStats (void);
static void Sta_WriteStatsExpTreesTableHead (void);
static void Sta_WriteRowStatsFileBrowsers (Brw_FileBrowser_t FileZone,const char *NameOfFileZones);
static void Sta_GetSizeOfFileZoneFromDB (Sco_Scope_t Scope,
                                         Brw_FileBrowser_t FileBrowser,
                                         struct Sta_SizeOfFileZones *SizeOfFileZones);

static void Sta_GetAndShowOERsStats (void);
static void Sta_GetNumberOfOERsFromDB (Sco_Scope_t Scope,Brw_License_t License,unsigned long NumFiles[2]);

static void Sta_GetAndShowAssignmentsStats (void);
static void Sta_GetAndShowTestsStats (void);

static void Sta_GetAndShowSocialActivityStats (void);
static void Sta_GetAndShowFollowStats (void);

static void Sta_GetAndShowForumStats (void);
static void Sta_ShowStatOfAForumType (For_ForumType_t ForumType,
                                      long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod,
                                      struct Sta_StatsForum *StatsForum);
static void Sta_WriteForumTitleAndStats (For_ForumType_t ForumType,
                                         long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod,
                                         const char *Icon,struct Sta_StatsForum *StatsForum,
                                         const char *ForumName1,const char *ForumName2);
static void Sta_WriteForumTotalStats (struct Sta_StatsForum *StatsForum);

static void Sta_GetAndShowNumUsrsPerNotifyEvent (void);
static void Sta_GetAndShowNoticesStats (void);
static void Sta_GetAndShowMsgsStats (void);

static void Sta_GetAndShowSurveysStats (void);
static void Sta_GetAndShowNumUsrsPerPrivacy (void);
static void Sta_GetAndShowNumUsrsPerPrivacyForAnObject (const char *TxtObject,const char *FieldName);
static void Sta_GetAndShowNumUsrsPerLanguage (void);
static void Sta_GetAndShowNumUsrsPerFirstDayOfWeek (void);
static void Sta_GetAndShowNumUsrsPerDateFormat (void);
static void Sta_GetAndShowNumUsrsPerIconSet (void);
static void Sta_GetAndShowNumUsrsPerMenu (void);
static void Sta_GetAndShowNumUsrsPerTheme (void);
static void Sta_GetAndShowNumUsrsPerSideColumns (void);

/*****************************************************************************/
/*************** Read CGI environment variable REMOTE_ADDR *******************/
/*****************************************************************************/
/*
CGI Environment Variables:
REMOTE_ADDR
The IP address of the remote host making the request.
*/
void Sta_GetRemoteAddr (void)
  {
   if (getenv ("REMOTE_ADDR"))
      Str_Copy (Gbl.IP,getenv ("REMOTE_ADDR"),
                Cns_MAX_BYTES_IP);
   else
      Gbl.IP[0] = '\0';
  }

/*****************************************************************************/
/**************************** Log access in database *************************/
/*****************************************************************************/

#define Sta_MAX_BYTES_QUERY_LOG (2048 - 1)

void Sta_LogAccess (const char *Comments)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   char Query[Sta_MAX_BYTES_QUERY_LOG +
              Sch_MAX_BYTES_STRING_TO_FIND + 1];
   long LogCod;
   Rol_Role_t RoleToStore = (Gbl.Action.Act == ActLogOut) ? Gbl.Usrs.Me.LoggedRoleBeforeCloseSession :
                                                            Gbl.Usrs.Me.LoggedRole;

   /***** Insert access into database *****/
   /* Log access in historical log (log_full) */
   sprintf (Query,"INSERT INTO log_full "
	          "(ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,"
	          "Role,ClickTime,TimeToGenerate,TimeToSend,IP)"
                  " VALUES "
                  "(%ld,%ld,%ld,%ld,%ld,%ld,%ld,"
                  "%u,NOW(),%ld,%ld,'%s')",
            Act_Actions[Gbl.Action.Act].ActCod,
            Gbl.CurrentCty.Cty.CtyCod,
            Gbl.CurrentIns.Ins.InsCod,
            Gbl.CurrentCtr.Ctr.CtrCod,
            Gbl.CurrentDeg.Deg.DegCod,
            Gbl.CurrentCrs.Crs.CrsCod,
            Gbl.Usrs.Me.UsrDat.UsrCod,
            (unsigned) RoleToStore,
            Gbl.TimeGenerationInMicroseconds,
            Gbl.TimeSendInMicroseconds,
            Gbl.IP);
   if (Gbl.WebService.IsWebService)
     {
      if (mysql_query (&Gbl.mysql,Query))
         Svc_Exit ("can not log access (full)");
      LogCod = (long) mysql_insert_id (&Gbl.mysql);
     }
   else
      LogCod = DB_QueryINSERTandReturnCode (Query,"can not log access (full)");

   /* Log access in recent log (log_recent) */
   sprintf (Query,"INSERT INTO log_recent "
	          "(LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,"
	          "Role,ClickTime,TimeToGenerate,TimeToSend,IP)"
                  " VALUES "
                  "(%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,"
                  "%u,NOW(),%ld,%ld,'%s')",
            LogCod,Act_Actions[Gbl.Action.Act].ActCod,
            Gbl.CurrentCty.Cty.CtyCod,
            Gbl.CurrentIns.Ins.InsCod,
            Gbl.CurrentCtr.Ctr.CtrCod,
            Gbl.CurrentDeg.Deg.DegCod,
            Gbl.CurrentCrs.Crs.CrsCod,
            Gbl.Usrs.Me.UsrDat.UsrCod,
            (unsigned) RoleToStore,
            Gbl.TimeGenerationInMicroseconds,
            Gbl.TimeSendInMicroseconds,
            Gbl.IP);
   if (Gbl.WebService.IsWebService)
     {
      if (mysql_query (&Gbl.mysql,Query))
         Svc_Exit ("can not log access (recent)");
     }
   else
      DB_QueryINSERT (Query,"can not log access (recent)");

   if (Comments)
     {
      /* Log comments */
      sprintf (Query,"INSERT INTO log_comments"
	             " (LogCod,Comments)"
                     " VALUES"
                     " (%ld,'",
	       LogCod);
      Str_AddStrToQuery (Query,Comments,sizeof (Query));
      Str_Concat (Query,"')",
                  Sta_MAX_BYTES_QUERY_LOG);

      if (Gbl.WebService.IsWebService)
        {
         if (mysql_query (&Gbl.mysql,Query))
            Svc_Exit ("can not log access (comments)");
        }
      else
         DB_QueryINSERT (Query,"can not log access (comments)");
     }

   if (Gbl.Search.LogSearch && Gbl.Search.Str[0])
     {
      /* Log search string */
      sprintf (Query,"INSERT INTO log_search"
	             " (LogCod,SearchStr)"
                     " VALUES"
                     " (%ld,'",
	       LogCod);
      Str_AddStrToQuery (Query,Gbl.Search.Str,sizeof (Query));
      Str_Concat (Query,"')",
                  Sta_MAX_BYTES_QUERY_LOG);

      if (Gbl.WebService.IsWebService)
        {
         if (mysql_query (&Gbl.mysql,Query))
            Svc_Exit ("can not log access (search)");
        }
      else
         DB_QueryINSERT (Query,"can not log access (search)");
     }

   if (Gbl.WebService.IsWebService)
     {
      /* Log web service plugin and function */
      sprintf (Query,"INSERT INTO log_ws"
	             " (LogCod,PlgCod,FunCod)"
                     " VALUES"
                     " (%ld,%ld,%u)",
	       LogCod,Gbl.WebService.PlgCod,(unsigned) Gbl.WebService.Function);

      if (mysql_query (&Gbl.mysql,Query))
         Svc_Exit ("can not log access (comments)");
     }
   else if (Gbl.Banners.BanCodClicked > 0)
     {
      /* Log banner clicked */
      sprintf (Query,"INSERT INTO log_banners"
	             " (LogCod,BanCod)"
                     " VALUES"
                     " (%ld,%ld)",
	       LogCod,Gbl.Banners.BanCodClicked);
      DB_QueryINSERT (Query,"can not log banner clicked");
     }

   /***** Increment my number of clicks *****/
   if (Gbl.Usrs.Me.UsrDat.UsrCod > 0)
      Prf_IncrementNumClicksUsr (Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************ Sometimes, we delete old entries in recent log table ***********/
/*****************************************************************************/

void Sta_RemoveOldEntriesRecentLog (void)
  {
   char Query[512];

   /***** Remove all expired clipboards *****/
   sprintf (Query,"DELETE LOW_PRIORITY FROM log_recent"
                  " WHERE ClickTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-'%lu')",
            Sta_SECONDS_IN_RECENT_LOG);
   DB_QueryDELETE (Query,"can not remove old entries from recent log");
  }

/*****************************************************************************/
/******************** Show a form to make a query of clicks ******************/
/*****************************************************************************/

void Sta_AskShowCrsHits (void)
  {
   extern const char *Hlp_STATS_Visits_visits_to_course;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Statistics_of_visits_to_the_course_X;
   extern const char *Txt_Users;
   extern const char *Txt_Show;
   extern const char *Txt_distributed_by;
   extern const char *Txt_STAT_CLICKS_GROUPED_BY[Sta_NUM_CLICKS_GROUPED_BY];
   extern const char *Txt_results_per_page;
   extern const char *Txt_Show_hits;
   extern const char *Txt_No_teachers_or_students_found;
   static unsigned long RowsPerPage[] =
     {
      Sta_MIN_ROWS_PER_PAGE * 1,
      Sta_MIN_ROWS_PER_PAGE * 2,
      Sta_MIN_ROWS_PER_PAGE * 3,
      Sta_MIN_ROWS_PER_PAGE * 4,
      Sta_MIN_ROWS_PER_PAGE * 5,
      Sta_MIN_ROWS_PER_PAGE * 10,
      Sta_MIN_ROWS_PER_PAGE * 50,
      Sta_MIN_ROWS_PER_PAGE * 100,
      Sta_MIN_ROWS_PER_PAGE * 500,
      Sta_MIN_ROWS_PER_PAGE * 1000,
      Sta_MIN_ROWS_PER_PAGE * 5000,
      Sta_MAX_ROWS_PER_PAGE,
     };
#define NUM_OPTIONS_ROWS_PER_PAGE (sizeof (RowsPerPage) / sizeof (RowsPerPage[0]))
   unsigned NumTotalUsrs;
   Sta_ClicksGroupedBy_t ClicksGroupedBy;
   unsigned long i;

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Get groups to show ******/
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Get and order the lists of users of this course *****/
   Usr_GetListUsrs (Sco_SCOPE_CRS,Rol_STD);
   Usr_GetListUsrs (Sco_SCOPE_CRS,Rol_NET);
   Usr_GetListUsrs (Sco_SCOPE_CRS,Rol_TCH);
   NumTotalUsrs = Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs +
	          Gbl.Usrs.LstUsrs[Rol_NET].NumUsrs +
	          Gbl.Usrs.LstUsrs[Rol_TCH].NumUsrs;

   /***** Start frame *****/
   sprintf (Gbl.Title,Txt_Statistics_of_visits_to_the_course_X,
	    Gbl.CurrentCrs.Crs.ShrtName);
   Lay_StartRoundFrame (NULL,Gbl.Title,NULL,Hlp_STATS_Visits_visits_to_course);

   /***** Show form to select the groups *****/
   Grp_ShowFormToSelectSeveralGroups (ActReqAccCrs);

   /***** Start section with user list *****/
   Lay_StartSection (Usr_USER_LIST_SECTION_ID);

   if (NumTotalUsrs)
     {
      if (Usr_GetIfShowBigList (NumTotalUsrs,NULL))
        {
	 /***** Form to select type of list used for select several users *****/
	 Usr_ShowFormsToSelectUsrListType (ActReqAccCrs);

	 /***** Start form *****/
         Act_FormStartAnchor (ActSeeAccCrs,Sta_STAT_RESULTS_SECTION_ID);

         Grp_PutParamsCodGrps ();
         Par_PutHiddenParamLong ("FirstRow",0);
         Par_PutHiddenParamLong ("LastRow",0);

         /***** Put list of users to select some of them *****/
         Lay_StartTableCenter (2);
         fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"RIGHT_TOP %s\">%s:"
			    "</td>"
			    "<td colspan=\"2\" class=\"%s LEFT_TOP\">"
                            "<table>",
                  The_ClassForm[Gbl.Prefs.Theme],Txt_Users,
                  The_ClassForm[Gbl.Prefs.Theme]);
         Usr_ListUsersToSelect (Rol_TCH);
         Usr_ListUsersToSelect (Rol_NET);
         Usr_ListUsersToSelect (Rol_STD);
         fprintf (Gbl.F.Out,"</table>"
                            "</td>"
                            "</tr>");

         /***** Initial and final dates of the search *****/
         Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (Gbl.Action.Act == ActReqAccCrs);

         /***** Selection of action *****/
         Sta_WriteSelectorAction ();

         /***** Option a) Listing of clicks distributed by some metric *****/
         fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"RIGHT_TOP %s\">%s:</td>"
			    "<td colspan=\"2\" class=\"LEFT_TOP\">",
                  The_ClassForm[Gbl.Prefs.Theme],Txt_Show);

         if ((Gbl.Stat.ClicksGroupedBy < Sta_CLICKS_CRS_PER_USR ||
              Gbl.Stat.ClicksGroupedBy > Sta_CLICKS_CRS_PER_ACTION) &&
              Gbl.Stat.ClicksGroupedBy != Sta_CLICKS_CRS_DETAILED_LIST)
            Gbl.Stat.ClicksGroupedBy = Sta_CLICKS_GROUPED_BY_DEFAULT;

         fprintf (Gbl.F.Out,"<input type=\"radio\""
                            " name=\"GroupedOrDetailed\" value=\"%u\"",
                  (unsigned) Sta_CLICKS_GROUPED);
         if (Gbl.Stat.ClicksGroupedBy != Sta_CLICKS_CRS_DETAILED_LIST)
            fprintf (Gbl.F.Out," checked=\"checked\"");
         fprintf (Gbl.F.Out," onclick=\"disableDetailedClicks()\" />");

         /* Selection of count type (number of pages generated, accesses per user, etc.) */
         Sta_WriteSelectorCountType ();

         fprintf (Gbl.F.Out,"<label class=\"%s\">&nbsp;%s&nbsp;"
                            "<select id=\"GroupedBy\" name=\"GroupedBy\">",
                  The_ClassForm[Gbl.Prefs.Theme],Txt_distributed_by);
         for (ClicksGroupedBy = Sta_CLICKS_CRS_PER_USR;
              ClicksGroupedBy <= Sta_CLICKS_CRS_PER_ACTION;
              ClicksGroupedBy++)
           {
            fprintf (Gbl.F.Out,"<option value=\"%u\"",
                     (unsigned) ClicksGroupedBy);
            if (ClicksGroupedBy == Gbl.Stat.ClicksGroupedBy)
	       fprintf (Gbl.F.Out," selected=\"selected\"");
            fprintf (Gbl.F.Out,">%s",Txt_STAT_CLICKS_GROUPED_BY[ClicksGroupedBy]);
           }
         fprintf (Gbl.F.Out,"</select>"
                            "</label><br />");

         /***** Option b) Listing of detailed clicks to this course *****/
         fprintf (Gbl.F.Out,"<label>"
                            "<input type=\"radio\""
                            " name=\"GroupedOrDetailed\" value=\"%u\"",
                  (unsigned) Sta_CLICKS_DETAILED);
         if (Gbl.Stat.ClicksGroupedBy == Sta_CLICKS_CRS_DETAILED_LIST)
            fprintf (Gbl.F.Out," checked=\"checked\"");
         fprintf (Gbl.F.Out," onclick=\"enableDetailedClicks()\" />"
                            "%s"
                            "</label>",
                  Txt_STAT_CLICKS_GROUPED_BY[Sta_CLICKS_CRS_DETAILED_LIST]);

         /* Number of rows per page */
         // To use getElementById in Firefox, it's necessary to have the id attribute
         fprintf (Gbl.F.Out," "
                            "<label>"
                            "(%s: <select id=\"RowsPage\" name=\"RowsPage\"",
                  Txt_results_per_page);
         if (Gbl.Stat.ClicksGroupedBy != Sta_CLICKS_CRS_DETAILED_LIST)
            fprintf (Gbl.F.Out," disabled=\"disabled\"");
         fprintf (Gbl.F.Out,">");
         for (i = 0;
              i < NUM_OPTIONS_ROWS_PER_PAGE;
              i++)
           {
            fprintf (Gbl.F.Out,"<option");
            if (RowsPerPage[i] == Gbl.Stat.RowsPerPage)
	       fprintf (Gbl.F.Out," selected=\"selected\"");
            fprintf (Gbl.F.Out,">%lu",RowsPerPage[i]);
           }
         fprintf (Gbl.F.Out,"</select>)"
                            "</label>"
                            "</td>"
                            "</tr>");
         Lay_EndTable ();

	 /***** Hidden param used to get client time zone *****/
	 Dat_PutHiddenParBrowserTZDiff ();

         /***** Send button *****/
	 Lay_PutConfirmButton (Txt_Show_hits);

         /***** End form *****/
         Act_FormEnd ();
        }
     }
   else	// No teachers nor students found
      Ale_ShowAlert (Ale_WARNING,Txt_No_teachers_or_students_found);

   /***** End section with user list *****/
   Lay_EndSection ();

   /***** End frame *****/
   Lay_EndRoundFrame ();

   /***** Free memory used by the lists *****/
   Usr_FreeUsrsList (Rol_TCH);
   Usr_FreeUsrsList (Rol_NET);
   Usr_FreeUsrsList (Rol_STD);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/********** Show a form to select the type of global stat of clics ***********/
/*****************************************************************************/

void Sta_AskShowGblHits (void)
  {
   extern const char *Hlp_STATS_Visits_global_visits;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Visits_to_course;
   extern const char *Txt_Statistics_of_all_visits;
   extern const char *Txt_Users;
   extern const char *Txt_ROLE_STATS[Sta_NUM_ROLES_STAT];
   extern const char *Txt_Scope;
   extern const char *Txt_Show;
   extern const char *Txt_distributed_by;
   extern const char *Txt_STAT_CLICKS_GROUPED_BY[Sta_NUM_CLICKS_GROUPED_BY];
   extern const char *Txt_Show_hits;
   Sta_Role_t RoleStat;
   Sta_ClicksGroupedBy_t ClicksGroupedBy;

   /***** Contextual links *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

   /* Put form to go to test edition and configuration */
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)		// Course selected
      switch (Gbl.Usrs.Me.LoggedRole)
        {
	 case Rol_NET:
	 case Rol_TCH:
	 case Rol_SYS_ADM:
	    Lay_PutContextualLink (ActReqAccCrs,NULL,NULL,
				   "stats64x64.gif",
				   Txt_Visits_to_course,Txt_Visits_to_course,
				   NULL);
	    break;
	 default:
	    break;
        }

   /* Link to show last clicks in real time */
   Con_PutLinkToLastClicks ();

   fprintf (Gbl.F.Out,"</div>");

   /***** Start form *****/
   Act_FormStartAnchor (ActSeeAccGbl,Sta_STAT_RESULTS_SECTION_ID);

   /***** Start frame *****/
   Lay_StartRoundFrameTable (NULL,Txt_Statistics_of_all_visits,NULL,
                             Hlp_STATS_Visits_global_visits,2);

   /***** Start and end dates for the search *****/
   Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (Gbl.Action.Act == ActReqAccGbl);

   /***** Users' roles whose accesses we want to see *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_MIDDLE\">"
                      "<label for=\"Role\" class=\"%s\">%s:</label>"
                      "</td>"
                      "<td colspan=\"2\" class=\"LEFT_MIDDLE\">"
                      "<select id=\"Role\" name=\"Role\">",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Users);
   for (RoleStat = (Sta_Role_t) 0;
	RoleStat < Sta_NUM_ROLES_STAT;
	RoleStat++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) RoleStat);
      if (RoleStat == Gbl.Stat.Role)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s",Txt_ROLE_STATS[RoleStat]);
     }
   fprintf (Gbl.F.Out,"</select>"
	              "</td>"
	              "</tr>");

   /***** Selection of action *****/
   Sta_WriteSelectorAction ();

   /***** Clicks made from anywhere, current centre, current degree or current course *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_MIDDLE\">"
                      "<label for=\"ScopeSta\" class=\"%s\">%s:</label>"
                      "</td>"
                      "<td colspan=\"2\" class=\"LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Scope);
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	               1 << Sco_SCOPE_CTY |
		       1 << Sco_SCOPE_INS |
		       1 << Sco_SCOPE_CTR |
		       1 << Sco_SCOPE_DEG |
		       1 << Sco_SCOPE_CRS;
   Gbl.Scope.Default = Sco_SCOPE_SYS;
   Sco_GetScope ("ScopeSta");
   Sco_PutSelectorScope ("ScopeSta",false);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Count type for the statistic *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_TOP\">"
                      "<label for=\"CountType\" class=\"%s\">%s:</label>"
                      "</td>"
                      "<td colspan=\"2\" class=\"LEFT_TOP\">",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Show);
   Sta_WriteSelectorCountType ();

   /***** Type of statistic *****/
   fprintf (Gbl.F.Out,"<label class=\"%s\">&nbsp;%s&nbsp;",
            The_ClassForm[Gbl.Prefs.Theme],Txt_distributed_by);

   if (Gbl.Stat.ClicksGroupedBy < Sta_CLICKS_GBL_PER_DAYS ||
       Gbl.Stat.ClicksGroupedBy > Sta_CLICKS_GBL_PER_COURSE)
      Gbl.Stat.ClicksGroupedBy = Sta_CLICKS_GBL_PER_DAYS;

   fprintf (Gbl.F.Out,"<select name=\"GroupedBy\">");
   for (ClicksGroupedBy = Sta_CLICKS_GBL_PER_DAYS;
	ClicksGroupedBy <= Sta_CLICKS_GBL_PER_COURSE;
	ClicksGroupedBy++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",
	       (unsigned) ClicksGroupedBy);
      if (ClicksGroupedBy == Gbl.Stat.ClicksGroupedBy)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s",Txt_STAT_CLICKS_GROUPED_BY[ClicksGroupedBy]);
     }
   fprintf (Gbl.F.Out,"</select>"
	              "</label>"
		      "</td>"
		      "</tr>"
		      "</table>");

   /***** Hidden param used to get client time zone *****/
   Dat_PutHiddenParBrowserTZDiff ();

   /***** End frame with button *****/
   Lay_EndRoundFrameWithButton (Lay_CONFIRM_BUTTON,Txt_Show_hits);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/****** Put selectors for type of access count and for degree or course ******/
/*****************************************************************************/

static void Sta_WriteSelectorCountType (void)
  {
   extern const char *Txt_STAT_TYPE_COUNT_SMALL[Sta_NUM_COUNT_TYPES];
   Sta_CountType_t StatCountType;

   /**** Count type *****/
   fprintf (Gbl.F.Out,"<select id=\"CountType\" name=\"CountType\">");
   for (StatCountType = (Sta_CountType_t) 0;
	StatCountType < Sta_NUM_COUNT_TYPES;
	StatCountType++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) StatCountType);
      if (StatCountType == Gbl.Stat.CountType)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s",Txt_STAT_TYPE_COUNT_SMALL[StatCountType]);
     }
   fprintf (Gbl.F.Out,"</select>");
  }

/*****************************************************************************/
/****** Put selectors for type of access count and for degree or course ******/
/*****************************************************************************/

static void Sta_WriteSelectorAction (void)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Action;
   extern const char *Txt_TABS_TXT[Tab_NUM_TABS];
   Act_Action_t Action;
   Act_Action_t SuperAction;
   Tab_Tab_t Tab;
   char ActTxt[Act_MAX_BYTES_ACTION_TXT + 1];

   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_TOP\">"
                      "<label for=\"StatAct\" class=\"%s\">%s:</label>"
                      "</td>"
                      "<td colspan=\"2\" class=\"LEFT_TOP\">"
                      "<select id=\"StatAct\" name=\"StatAct\""
                      " style=\"width:375px;\">",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Action);
   for (Action = (Act_Action_t) 0;
	Action < Act_NUM_ACTIONS;
	Action++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) Action);
      if (Action == Gbl.Stat.NumAction)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">");
      if (Action)
         fprintf (Gbl.F.Out,"%u: ",(unsigned) Action);
      SuperAction = Act_Actions[Action].SuperAction;
      Tab = Act_Actions[SuperAction].Tab;
      if (Txt_TABS_TXT[Tab])
         fprintf (Gbl.F.Out,"%s &gt; ",Txt_TABS_TXT[Tab]);
      fprintf (Gbl.F.Out,"%s",
               Act_GetActionTextFromDB (Act_Actions[Action].ActCod,ActTxt));
     }

   fprintf (Gbl.F.Out,"</select>"
	              "</td>"
	              "</tr>");
  }

/*****************************************************************************/
/************ Set end date to current date                        ************/
/************ and set initial date to end date minus several days ************/
/*****************************************************************************/

void Sta_SetIniEndDates (void)
  {
   Gbl.DateRange.TimeUTC[0] = Gbl.StartExecutionTimeUTC - ((Cfg_DAYS_IN_RECENT_LOG - 1) * 24 * 60 * 60);
   Gbl.DateRange.TimeUTC[1] = Gbl.StartExecutionTimeUTC;
  }

/*****************************************************************************/
/******************** Compute and show access statistics *********************/
/*****************************************************************************/

void Sta_SeeGblAccesses (void)
  {
   Sta_ShowHits (Sta_SHOW_GLOBAL_ACCESSES);
  }

void Sta_SeeCrsAccesses (void)
  {
   Sta_ShowHits (Sta_SHOW_COURSE_ACCESSES);
  }

/*****************************************************************************/
/******************** Compute and show access statistics ********************/
/*****************************************************************************/

#define Sta_MAX_BYTES_QUERY_ACCESS (1024 + (10 + ID_MAX_BYTES_USR_ID) * 5000 - 1)

#define Sta_MAX_BYTES_COUNT_TYPE (256 - 1)

static void Sta_ShowHits (Sta_GlobalOrCourseAccesses_t GlobalOrCourse)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   extern const char *Txt_You_must_select_one_ore_more_users;
   extern const char *Txt_There_is_no_knowing_how_many_users_not_logged_have_accessed;
   extern const char *Txt_The_date_range_must_be_less_than_or_equal_to_X_days;
   extern const char *Txt_There_are_no_accesses_with_the_selected_search_criteria;
   extern const char *Txt_List_of_detailed_clicks;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   extern const char *Txt_Time_zone_used_in_the_calculation_of_these_statistics;
   char Query[Sta_MAX_BYTES_QUERY_ACCESS + 1];
   char QueryAux[512];
   long LengthQuery;
   MYSQL_RES *mysql_res;
   unsigned long NumRows;
   const char *LogTable;
   Sta_ClicksDetailedOrGrouped_t DetailedOrGrouped = Sta_CLICKS_GROUPED;
   struct UsrData UsrDat;
   char BrowserTimeZone[Dat_MAX_BYTES_TIME_ZONE + 1];
   unsigned NumUsr = 0;
   const char *Ptr;
   char StrRole[256];
   char StrQueryCountType[Sta_MAX_BYTES_COUNT_TYPE + 1];
   unsigned NumDays;
   bool ICanQueryWholeRange;

   /***** Get initial and ending dates *****/
   Dat_GetIniEndDatesFromForm ();

   /***** Get client time zone *****/
   Dat_GetBrowserTimeZone (BrowserTimeZone);

   /***** Set table where to find depending on initial date *****/
   /* If initial day is older than current day minus Cfg_DAYS_IN_RECENT_LOG,
      then use recent log table, else use historic log table */
   LogTable = (Dat_GetNumDaysBetweenDates (&Gbl.DateRange.DateIni.Date,&Gbl.Now.Date)
	       <= Cfg_DAYS_IN_RECENT_LOG) ? "log_recent" :
	                                    "log_full";

   /***** Get the type of stat of clicks ******/
   DetailedOrGrouped = (Sta_ClicksDetailedOrGrouped_t)
	               Par_GetParToUnsignedLong ("GroupedOrDetailed",
	                                         0,
	                                         Sta_NUM_CLICKS_DETAILED_OR_GROUPED - 1,
	                                         (unsigned long) Sta_CLICKS_DETAILED_OR_GROUPED_DEFAULT);

   if (DetailedOrGrouped == Sta_CLICKS_DETAILED)
      Gbl.Stat.ClicksGroupedBy = Sta_CLICKS_CRS_DETAILED_LIST;
   else	// DetailedOrGrouped == Sta_CLICKS_GROUPED
      Gbl.Stat.ClicksGroupedBy = (Sta_ClicksGroupedBy_t)
			         Par_GetParToUnsignedLong ("GroupedBy",
						           0,
						           Sta_NUM_CLICKS_GROUPED_BY - 1,
						           (unsigned long) Sta_CLICKS_GROUPED_BY_DEFAULT);

   /***** Get the type of count of clicks *****/
   if (Gbl.Stat.ClicksGroupedBy != Sta_CLICKS_CRS_DETAILED_LIST)
      Gbl.Stat.CountType = (Sta_CountType_t)
	                   Par_GetParToUnsignedLong ("CountType",
	                                             0,
	                                             Sta_NUM_COUNT_TYPES - 1,
	                                             (unsigned long) Sta_COUNT_TYPE_DEFAULT);

   /***** Get action *****/
   Gbl.Stat.NumAction = (Act_Action_t)
			Par_GetParToUnsignedLong ("StatAct",
					          0,
					          Act_NUM_ACTIONS - 1,
					          (unsigned long) Sta_NUM_ACTION_DEFAULT);

   switch (GlobalOrCourse)
     {
      case Sta_SHOW_GLOBAL_ACCESSES:
	 /***** Get the type of user of clicks *****/
	 Gbl.Stat.Role = (Sta_Role_t)
			 Par_GetParToUnsignedLong ("Role",
				                   0,
					           Sta_NUM_ROLES_STAT - 1,
				                   (unsigned long) Sta_ROLE_DEFAULT);

	 /***** Get users range for access statistics *****/
	 Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
			     1 << Sco_SCOPE_CTY |
			     1 << Sco_SCOPE_INS |
			     1 << Sco_SCOPE_CTR |
			     1 << Sco_SCOPE_DEG |
			     1 << Sco_SCOPE_CRS;
	 Gbl.Scope.Default = Sco_SCOPE_SYS;
	 Sco_GetScope ("ScopeSta");

	 /***** Show form again *****/
	 Sta_AskShowGblHits ();

	 /***** Start results section *****/
	 Lay_StartSection (Sta_STAT_RESULTS_SECTION_ID);

	 /***** Check selection *****/
	 if ((Gbl.Stat.Role == Sta_ROLE_ALL_USRS ||
	      Gbl.Stat.Role == Sta_ROLE_UNKNOWN_USRS) &&
	     (Gbl.Stat.CountType == Sta_DISTINCT_USRS ||
	      Gbl.Stat.CountType == Sta_CLICKS_PER_USR))	// These types of query will never give a valid result
	   {
	    /* Write warning message and abort */
	    Ale_ShowAlert (Ale_WARNING,Txt_There_is_no_knowing_how_many_users_not_logged_have_accessed);
	    return;
	   }
	 break;
      case Sta_SHOW_COURSE_ACCESSES:
	 if (Gbl.Stat.ClicksGroupedBy == Sta_CLICKS_CRS_DETAILED_LIST)
	   {
	    /****** Get the number of the first row to show ******/
	    Gbl.Stat.FirstRow = Par_GetParToUnsignedLong ("FirstRow",
	                                                  1,
	                                                  ULONG_MAX,
	                                                  0);

	    /****** Get the number of the last row to show ******/
	    Gbl.Stat.LastRow = Par_GetParToUnsignedLong ("LastRow",
	                                                 1,
	                                                 ULONG_MAX,
	                                                 0);

	    /****** Get the number of rows per page ******/
	    Gbl.Stat.RowsPerPage = Par_GetParToUnsignedLong ("RowsPage",
	                                                     Sta_MIN_ROWS_PER_PAGE,
	                                                     Sta_MAX_ROWS_PER_PAGE,
	                                                     Sta_DEF_ROWS_PER_PAGE);
	   }

	 /****** Get lists of selected users ******/
	 Usr_GetListsSelectedUsrsCods ();

	 /***** Show the form again *****/
	 Sta_AskShowCrsHits ();

	 /***** Start results section *****/
	 Lay_StartSection (Sta_STAT_RESULTS_SECTION_ID);

	 /***** Check selection *****/
	 if (!Usr_CountNumUsrsInListOfSelectedUsrs ())	// Error: there are no users selected
	   {
	    /* Write warning message, clean and abort */
	    Ale_ShowAlert (Ale_WARNING,Txt_You_must_select_one_ore_more_users);
            Usr_FreeListsSelectedUsrsCods ();
	    return;
	   }
	 break;
     }

   /***** Check if range of dates is forbidden for me *****/
   NumDays = Dat_GetNumDaysBetweenDates (&Gbl.DateRange.DateIni.Date,&Gbl.DateRange.DateEnd.Date);
   ICanQueryWholeRange = (Gbl.Usrs.Me.LoggedRole >= Rol_TCH && GlobalOrCourse == Sta_SHOW_COURSE_ACCESSES) ||
			 (Gbl.Usrs.Me.LoggedRole == Rol_TCH &&  Gbl.Scope.Current == Sco_SCOPE_CRS)  ||
			 (Gbl.Usrs.Me.LoggedRole == Rol_DEG_ADM && (Gbl.Scope.Current == Sco_SCOPE_DEG   ||
			                                            Gbl.Scope.Current == Sco_SCOPE_CRS)) ||
			 (Gbl.Usrs.Me.LoggedRole == Rol_CTR_ADM && (Gbl.Scope.Current == Sco_SCOPE_CTR   ||
			                                            Gbl.Scope.Current == Sco_SCOPE_DEG   ||
			                                            Gbl.Scope.Current == Sco_SCOPE_CRS)) ||
			 (Gbl.Usrs.Me.LoggedRole == Rol_INS_ADM && (Gbl.Scope.Current == Sco_SCOPE_INS   ||
			                                            Gbl.Scope.Current == Sco_SCOPE_CTR   ||
			                                            Gbl.Scope.Current == Sco_SCOPE_DEG   ||
			                                            Gbl.Scope.Current == Sco_SCOPE_CRS)) ||
			  Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM;
   if (!ICanQueryWholeRange && NumDays > Cfg_DAYS_IN_RECENT_LOG)
     {
      sprintf (Gbl.Alert.Txt,Txt_The_date_range_must_be_less_than_or_equal_to_X_days,
	       Cfg_DAYS_IN_RECENT_LOG);
      Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);	// ...write warning message and show the form again
      return;
     }

   /***** Query depending on the type of count *****/
   switch (Gbl.Stat.CountType)
     {
      case Sta_TOTAL_CLICKS:
         Str_Copy (StrQueryCountType,"COUNT(*)",
                   Sta_MAX_BYTES_COUNT_TYPE);
	 break;
      case Sta_DISTINCT_USRS:
         sprintf (StrQueryCountType,"COUNT(DISTINCT(%s.UsrCod))",LogTable);
	 break;
      case Sta_CLICKS_PER_USR:
         sprintf (StrQueryCountType,"COUNT(*)/GREATEST(COUNT(DISTINCT(%s.UsrCod)),1)+0.000000",LogTable);
	 break;
      case Sta_GENERATION_TIME:
         sprintf (StrQueryCountType,"(AVG(%s.TimeToGenerate)/1E6)+0.000000",LogTable);
	 break;
      case Sta_SEND_TIME:
         sprintf (StrQueryCountType,"(AVG(%s.TimeToSend)/1E6)+0.000000",LogTable);
	 break;
     }

   /***** Select clicks from the table of log *****/
   /* Start the query */
   switch (Gbl.Stat.ClicksGroupedBy)
     {
      case Sta_CLICKS_CRS_DETAILED_LIST:
   	 sprintf (Query,"SELECT SQL_NO_CACHE LogCod,UsrCod,Role,"
   	                "UNIX_TIMESTAMP(ClickTime) AS F,ActCod FROM %s",
                  LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_USR:
	 sprintf (Query,"SELECT SQL_NO_CACHE UsrCod,%s AS Num FROM %s",
                  StrQueryCountType,LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_DAYS:
      case Sta_CLICKS_GBL_PER_DAYS:
         sprintf (Query,"SELECT SQL_NO_CACHE "
                        "DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%Y%%m%%d') AS Day,"
                        "%s FROM %s",
                  BrowserTimeZone,
                  StrQueryCountType,LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_DAYS_AND_HOUR:
      case Sta_CLICKS_GBL_PER_DAYS_AND_HOUR:
         sprintf (Query,"SELECT SQL_NO_CACHE "
                        "DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%Y%%m%%d') AS Day,"
                        "DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%H') AS Hour,"
                        "%s FROM %s",
                  BrowserTimeZone,
                  BrowserTimeZone,
                  StrQueryCountType,LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_WEEKS:
      case Sta_CLICKS_GBL_PER_WEEKS:
	 /* With %x%v the weeks are counted from monday to sunday.
	    With %X%V the weeks are counted from sunday to saturday. */
	 sprintf (Query,(Gbl.Prefs.FirstDayOfWeek == 0) ?
			"SELECT SQL_NO_CACHE "	// Weeks start on monday
			"DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%x%%v') AS Week,"
			"%s FROM %s" :
			"SELECT SQL_NO_CACHE "	// Weeks start on sunday
			"DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%X%%V') AS Week,"
			"%s FROM %s",
		  BrowserTimeZone,
		  StrQueryCountType,LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_MONTHS:
      case Sta_CLICKS_GBL_PER_MONTHS:
         sprintf (Query,"SELECT SQL_NO_CACHE "
                        "DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%Y%%m') AS Month,"
                        "%s FROM %s",
                  BrowserTimeZone,
                  StrQueryCountType,LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_HOUR:
      case Sta_CLICKS_GBL_PER_HOUR:
         sprintf (Query,"SELECT SQL_NO_CACHE "
                        "DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%H') AS Hour,"
                        "%s FROM %s",
                  BrowserTimeZone,
                  StrQueryCountType,LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_MINUTE:
      case Sta_CLICKS_GBL_PER_MINUTE:
         sprintf (Query,"SELECT SQL_NO_CACHE "
                        "DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%H%%i') AS Minute,"
                        "%s FROM %s",
                  BrowserTimeZone,
                  StrQueryCountType,LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_ACTION:
      case Sta_CLICKS_GBL_PER_ACTION:
         sprintf (Query,"SELECT SQL_NO_CACHE ActCod,%s AS Num FROM %s",
                  StrQueryCountType,LogTable);
	 break;
      case Sta_CLICKS_GBL_PER_PLUGIN:
         sprintf (Query,"SELECT SQL_NO_CACHE log_ws.PlgCod,%s AS Num FROM %s,log_ws",
                  StrQueryCountType,LogTable);
         break;
      case Sta_CLICKS_GBL_PER_WEB_SERVICE_FUNCTION:
         sprintf (Query,"SELECT SQL_NO_CACHE log_ws.FunCod,%s AS Num FROM %s,log_ws",
                  StrQueryCountType,LogTable);
         break;
      case Sta_CLICKS_GBL_PER_BANNER:
         sprintf (Query,"SELECT SQL_NO_CACHE log_banners.BanCod,%s AS Num FROM %s,log_banners",
                  StrQueryCountType,LogTable);
         break;
      case Sta_CLICKS_GBL_PER_COUNTRY:
         sprintf (Query,"SELECT SQL_NO_CACHE CtyCod,%s AS Num FROM %s",
                  StrQueryCountType,LogTable);
	 break;
      case Sta_CLICKS_GBL_PER_INSTITUTION:
         sprintf (Query,"SELECT SQL_NO_CACHE InsCod,%s AS Num FROM %s",
                  StrQueryCountType,LogTable);
	 break;
      case Sta_CLICKS_GBL_PER_CENTRE:
         sprintf (Query,"SELECT SQL_NO_CACHE CtrCod,%s AS Num FROM %s",
                  StrQueryCountType,LogTable);
	 break;
      case Sta_CLICKS_GBL_PER_DEGREE:
         sprintf (Query,"SELECT SQL_NO_CACHE DegCod,%s AS Num FROM %s",
                  StrQueryCountType,LogTable);
	 break;
      case Sta_CLICKS_GBL_PER_COURSE:
	 sprintf (Query,"SELECT SQL_NO_CACHE CrsCod,%s AS Num FROM %s",
                  StrQueryCountType,LogTable);
	 break;
     }
   sprintf (QueryAux," WHERE %s.ClickTime"
	             " BETWEEN FROM_UNIXTIME(%ld) AND FROM_UNIXTIME(%ld)",
            LogTable,
            (long) Gbl.DateRange.TimeUTC[0],
            (long) Gbl.DateRange.TimeUTC[1]);
   Str_Concat (Query,QueryAux,
               Sta_MAX_BYTES_QUERY_ACCESS);

   switch (GlobalOrCourse)
     {
      case Sta_SHOW_GLOBAL_ACCESSES:
	 /* Scope */
	 switch (Gbl.Scope.Current)
	   {
	    case Sco_SCOPE_UNK:
	    case Sco_SCOPE_SYS:
               break;
	    case Sco_SCOPE_CTY:
               if (Gbl.CurrentCty.Cty.CtyCod > 0)
		 {
		  sprintf (QueryAux," AND %s.CtyCod=%ld",
			   LogTable,Gbl.CurrentCty.Cty.CtyCod);
		  Str_Concat (Query,QueryAux,
		              Sta_MAX_BYTES_QUERY_ACCESS);
		 }
               break;
	    case Sco_SCOPE_INS:
	       if (Gbl.CurrentIns.Ins.InsCod > 0)
		 {
		  sprintf (QueryAux," AND %s.InsCod=%ld",
			   LogTable,Gbl.CurrentIns.Ins.InsCod);
		  Str_Concat (Query,QueryAux,
		              Sta_MAX_BYTES_QUERY_ACCESS);
		 }
	       break;
	    case Sco_SCOPE_CTR:
               if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
		 {
		  sprintf (QueryAux," AND %s.CtrCod=%ld",
			   LogTable,Gbl.CurrentCtr.Ctr.CtrCod);
		  Str_Concat (Query,QueryAux,
		              Sta_MAX_BYTES_QUERY_ACCESS);
		 }
               break;
	    case Sco_SCOPE_DEG:
	       if (Gbl.CurrentDeg.Deg.DegCod > 0)
		 {
		  sprintf (QueryAux," AND %s.DegCod=%ld",
			   LogTable,Gbl.CurrentDeg.Deg.DegCod);
		  Str_Concat (Query,QueryAux,
		              Sta_MAX_BYTES_QUERY_ACCESS);
		 }
	       break;
	    case Sco_SCOPE_CRS:
	       if (Gbl.CurrentCrs.Crs.CrsCod > 0)
		 {
		  sprintf (QueryAux," AND %s.CrsCod=%ld",
			   LogTable,Gbl.CurrentCrs.Crs.CrsCod);
		  Str_Concat (Query,QueryAux,
		              Sta_MAX_BYTES_QUERY_ACCESS);
		 }
	       break;
	   }

         /* Type of users */
	 switch (Gbl.Stat.Role)
	   {
	    case Sta_ROLE_IDENTIFIED_USRS:
               sprintf (StrRole," AND %s.Role<>%u",
                        LogTable,(unsigned) Rol_UNK);
	       break;
	    case Sta_ROLE_ALL_USRS:
               switch (Gbl.Stat.CountType)
                 {
                  case Sta_TOTAL_CLICKS:
                  case Sta_GENERATION_TIME:
                  case Sta_SEND_TIME:
                     StrRole[0] = '\0';
	             break;
                  case Sta_DISTINCT_USRS:
                  case Sta_CLICKS_PER_USR:
                     sprintf (StrRole," AND %s.Role<>%u",
                              LogTable,(unsigned) Rol_UNK);
                     break;
                    }
	       break;
	    case Sta_ROLE_INS_ADMINS:
               sprintf (StrRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_INS_ADM);
	       break;
	    case Sta_ROLE_CTR_ADMINS:
               sprintf (StrRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_CTR_ADM);
	       break;
	    case Sta_ROLE_DEG_ADMINS:
               sprintf (StrRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_DEG_ADM);
	       break;
	    case Sta_ROLE_TEACHERS:
               sprintf (StrRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_TCH);
	       break;
	    case Sta_ROLE_NON_EDITING_TEACHERS:
               sprintf (StrRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_NET);
	       break;
	    case Sta_ROLE_STUDENTS:
               sprintf (StrRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_STD);
	       break;
	    case Sta_ROLE_USERS:
               sprintf (StrRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_USR);
               break;
	    case Sta_ROLE_GUESTS:
               sprintf (StrRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_GST);
               break;
	    case Sta_ROLE_UNKNOWN_USRS:
               sprintf (StrRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_UNK);
               break;
	    case Sta_ROLE_ME:
               sprintf (StrRole," AND %s.UsrCod=%ld",
                        LogTable,Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	   }
         Str_Concat (Query,StrRole,
                     Sta_MAX_BYTES_QUERY_ACCESS);

         switch (Gbl.Stat.ClicksGroupedBy)
           {
            case Sta_CLICKS_GBL_PER_PLUGIN:
            case Sta_CLICKS_GBL_PER_WEB_SERVICE_FUNCTION:
               sprintf (QueryAux," AND %s.LogCod=log_ws.LogCod",
                        LogTable);
               Str_Concat (Query,QueryAux,
                           Sta_MAX_BYTES_QUERY_ACCESS);
               break;
            case Sta_CLICKS_GBL_PER_BANNER:
               sprintf (QueryAux," AND %s.LogCod=log_banners.LogCod",
                        LogTable);
               Str_Concat (Query,QueryAux,
                           Sta_MAX_BYTES_QUERY_ACCESS);
               break;
            default:
               break;
           }
	 break;
      case Sta_SHOW_COURSE_ACCESSES:
         sprintf (QueryAux," AND %s.CrsCod=%ld",
                  LogTable,Gbl.CurrentCrs.Crs.CrsCod);
	 Str_Concat (Query,QueryAux,
	             Sta_MAX_BYTES_QUERY_ACCESS);

	 /***** Initialize data structure of the user *****/
         Usr_UsrDataConstructor (&UsrDat);

	 LengthQuery = strlen (Query);
	 NumUsr = 0;
	 Ptr = Gbl.Usrs.Select[Rol_UNK];
	 while (*Ptr)
	   {
	    Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDat.EncryptedUsrCod,
	                                       Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
            Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
	    if (UsrDat.UsrCod > 0)
	      {
	       LengthQuery = LengthQuery + 25 + 10 + 1;
	       if (LengthQuery > Sta_MAX_BYTES_QUERY_ACCESS - 128)
                  Lay_ShowErrorAndExit ("Query is too large.");
               sprintf (QueryAux,
                        NumUsr ? " OR %s.UsrCod=%ld" :
                                 " AND (%s.UsrCod=%ld",
                        LogTable,UsrDat.UsrCod);
	       Str_Concat (Query,QueryAux,
	                   Sta_MAX_BYTES_QUERY_ACCESS);
	       NumUsr++;
	      }
	   }
	 Str_Concat (Query,")",
	             Sta_MAX_BYTES_QUERY_ACCESS);

	 /***** Free memory used by the data of the user *****/
         Usr_UsrDataDestructor (&UsrDat);
	 break;
     }

   /* Select action */
   if (Gbl.Stat.NumAction != ActAll)
     {
      sprintf (QueryAux," AND %s.ActCod=%ld",
               LogTable,Act_Actions[Gbl.Stat.NumAction].ActCod);
      Str_Concat (Query,QueryAux,
                  Sta_MAX_BYTES_QUERY_ACCESS);
     }

   /* End the query */
   switch (Gbl.Stat.ClicksGroupedBy)
     {
      case Sta_CLICKS_CRS_DETAILED_LIST:
	 Str_Concat (Query," ORDER BY F",
	             Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_USR:
	 sprintf (QueryAux," GROUP BY %s.UsrCod ORDER BY Num DESC",LogTable);
         Str_Concat (Query,QueryAux,
                     Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_DAYS:
      case Sta_CLICKS_GBL_PER_DAYS:
	 Str_Concat (Query," GROUP BY Day DESC",
	             Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_DAYS_AND_HOUR:
      case Sta_CLICKS_GBL_PER_DAYS_AND_HOUR:
	 Str_Concat (Query," GROUP BY Day DESC,Hour",
	             Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_WEEKS:
      case Sta_CLICKS_GBL_PER_WEEKS:
	 Str_Concat (Query," GROUP BY Week DESC",
	             Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_MONTHS:
      case Sta_CLICKS_GBL_PER_MONTHS:
	 Str_Concat (Query," GROUP BY Month DESC",
	             Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_HOUR:
      case Sta_CLICKS_GBL_PER_HOUR:
	 Str_Concat (Query," GROUP BY Hour",
	             Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_MINUTE:
      case Sta_CLICKS_GBL_PER_MINUTE:
	 Str_Concat (Query," GROUP BY Minute",
	             Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_ACTION:
      case Sta_CLICKS_GBL_PER_ACTION:
	 sprintf (QueryAux," GROUP BY %s.ActCod ORDER BY Num DESC",LogTable);
         Str_Concat (Query,QueryAux,
                     Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_GBL_PER_PLUGIN:
         Str_Concat (Query," GROUP BY log_ws.PlgCod ORDER BY Num DESC",
                     Sta_MAX_BYTES_QUERY_ACCESS);
         break;
      case Sta_CLICKS_GBL_PER_WEB_SERVICE_FUNCTION:
         Str_Concat (Query," GROUP BY log_ws.FunCod ORDER BY Num DESC",
                     Sta_MAX_BYTES_QUERY_ACCESS);
         break;
      case Sta_CLICKS_GBL_PER_BANNER:
         Str_Concat (Query," GROUP BY log_banners.BanCod ORDER BY Num DESC",
                     Sta_MAX_BYTES_QUERY_ACCESS);
         break;
      case Sta_CLICKS_GBL_PER_COUNTRY:
	 sprintf (QueryAux," GROUP BY %s.CtyCod ORDER BY Num DESC",LogTable);
         Str_Concat (Query,QueryAux,
                     Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_GBL_PER_INSTITUTION:
	 sprintf (QueryAux," GROUP BY %s.InsCod ORDER BY Num DESC",LogTable);
         Str_Concat (Query,QueryAux,
                     Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_GBL_PER_CENTRE:
	 sprintf (QueryAux," GROUP BY %s.CtrCod ORDER BY Num DESC",LogTable);
         Str_Concat (Query,QueryAux,
                     Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_GBL_PER_DEGREE:
	 sprintf (QueryAux," GROUP BY %s.DegCod ORDER BY Num DESC",LogTable);
         Str_Concat (Query,QueryAux,
                     Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_GBL_PER_COURSE:
	 sprintf (QueryAux," GROUP BY %s.CrsCod ORDER BY Num DESC",LogTable);
         Str_Concat (Query,QueryAux,
                     Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
     }
   /***** Write query for debug *****/
   /*
   if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
      Lay_ShowAlert (Lay_INFO,Query);
   */
   /***** Make the query *****/
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get clicks");

   /***** Count the number of rows in result *****/
   if (NumRows == 0)
      Ale_ShowAlert (Ale_INFO,Txt_There_are_no_accesses_with_the_selected_search_criteria);
   else
     {
      /***** Put the table with the clicks *****/
      if (Gbl.Stat.ClicksGroupedBy == Sta_CLICKS_CRS_DETAILED_LIST)
	 Lay_StartRoundFrame ("95%",Txt_List_of_detailed_clicks,
	                      NULL,NULL);
      else
	 Lay_StartRoundFrame (NULL,Txt_STAT_TYPE_COUNT_CAPS[Gbl.Stat.CountType],
	                      NULL,NULL);

      fprintf (Gbl.F.Out,"<table");
      if (Sta_CellPadding[Gbl.Stat.ClicksGroupedBy])
         fprintf (Gbl.F.Out," class=\"CELLS_PAD_%u\"",
                  Sta_CellPadding[Gbl.Stat.ClicksGroupedBy]);
      fprintf (Gbl.F.Out,">");

      switch (Gbl.Stat.ClicksGroupedBy)
	{
	 case Sta_CLICKS_CRS_DETAILED_LIST:
	    Sta_ShowDetailedAccessesList (NumRows,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_USR:
	    Sta_ShowNumHitsPerUsr (NumRows,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_DAYS:
	 case Sta_CLICKS_GBL_PER_DAYS:
	    Sta_ShowNumHitsPerDays (NumRows,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_DAYS_AND_HOUR:
	 case Sta_CLICKS_GBL_PER_DAYS_AND_HOUR:
	    Sta_ShowDistrAccessesPerDaysAndHour (NumRows,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_WEEKS:
	 case Sta_CLICKS_GBL_PER_WEEKS:
	    Sta_ShowNumHitsPerWeeks (NumRows,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_MONTHS:
	 case Sta_CLICKS_GBL_PER_MONTHS:
	    Sta_ShowNumHitsPerMonths (NumRows,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_HOUR:
	 case Sta_CLICKS_GBL_PER_HOUR:
	    Sta_ShowNumHitsPerHour (NumRows,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_MINUTE:
	 case Sta_CLICKS_GBL_PER_MINUTE:
	    Sta_ShowAverageAccessesPerMinute (NumRows,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_ACTION:
	 case Sta_CLICKS_GBL_PER_ACTION:
	    Sta_ShowNumHitsPerAction (NumRows,mysql_res);
	    break;
         case Sta_CLICKS_GBL_PER_PLUGIN:
            Sta_ShowNumHitsPerPlugin (NumRows,mysql_res);
            break;
         case Sta_CLICKS_GBL_PER_WEB_SERVICE_FUNCTION:
            Sta_ShowNumHitsPerWSFunction (NumRows,mysql_res);
            break;
         case Sta_CLICKS_GBL_PER_BANNER:
            Sta_ShowNumHitsPerBanner (NumRows,mysql_res);
            break;
         case Sta_CLICKS_GBL_PER_COUNTRY:
	    Sta_ShowNumHitsPerCountry (NumRows,mysql_res);
	    break;
         case Sta_CLICKS_GBL_PER_INSTITUTION:
	    Sta_ShowNumHitsPerInstitution (NumRows,mysql_res);
	    break;
         case Sta_CLICKS_GBL_PER_CENTRE:
	    Sta_ShowNumHitsPerCentre (NumRows,mysql_res);
	    break;
	 case Sta_CLICKS_GBL_PER_DEGREE:
	    Sta_ShowNumHitsPerDegree (NumRows,mysql_res);
	    break;
	 case Sta_CLICKS_GBL_PER_COURSE:
	    Sta_ShowNumHitsPerCourse (NumRows,mysql_res);
	    break;
	}
      fprintf (Gbl.F.Out,"</table>");

      /* End of frame */
      Lay_EndRoundFrame ();
      Lay_EndSection ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Free memory used by list of selected users' codes *****/
   if (Gbl.Action.Act == ActSeeAccCrs)
      Usr_FreeListsSelectedUsrsCods ();

   /***** Write time zone used in the calculation of these statistics *****/
   switch (Gbl.Stat.ClicksGroupedBy)
     {
      case Sta_CLICKS_CRS_PER_DAYS:
      case Sta_CLICKS_GBL_PER_DAYS:
      case Sta_CLICKS_CRS_PER_DAYS_AND_HOUR:
      case Sta_CLICKS_GBL_PER_DAYS_AND_HOUR:
      case Sta_CLICKS_CRS_PER_WEEKS:
      case Sta_CLICKS_GBL_PER_WEEKS:
      case Sta_CLICKS_CRS_PER_MONTHS:
      case Sta_CLICKS_GBL_PER_MONTHS:
      case Sta_CLICKS_CRS_PER_HOUR:
      case Sta_CLICKS_GBL_PER_HOUR:
      case Sta_CLICKS_CRS_PER_MINUTE:
      case Sta_CLICKS_GBL_PER_MINUTE:
	 fprintf (Gbl.F.Out,"<p class=\"DAT_SMALL CENTER_MIDDLE\">%s: %s</p>",
		  Txt_Time_zone_used_in_the_calculation_of_these_statistics,
		  BrowserTimeZone);
	 break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/******************* Show a listing of detailed clicks ***********************/
/*****************************************************************************/

static void Sta_ShowDetailedAccessesList (unsigned long NumRows,MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Show_previous_X_clicks;
   extern const char *Txt_PAGES_Previous;
   extern const char *Txt_Clicks;
   extern const char *Txt_of_PART_OF_A_TOTAL;
   extern const char *Txt_page;
   extern const char *Txt_Show_next_X_clicks;
   extern const char *Txt_PAGES_Next;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_User_ID;
   extern const char *Txt_Name;
   extern const char *Txt_Type;
   extern const char *Txt_Date;
   extern const char *Txt_Action;
   extern const char *Txt_LOG_More_info;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Today;
   unsigned long NumRow;
   unsigned long FirstRow;	// First row to show
   unsigned long LastRow;	// Last rows to show
   unsigned long NumPagesBefore;
   unsigned long NumPagesAfter;
   unsigned long NumPagsTotal;
   struct UsrData UsrDat;
   MYSQL_ROW row;
   long LogCod;
   Rol_Role_t RoleFromLog;
   unsigned UniqueId;
   long ActCod;
   char ActTxt[Act_MAX_BYTES_ACTION_TXT + 1];

   /***** Initialize estructura of data of the user *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Compute the first and the last row to show *****/
   FirstRow = Gbl.Stat.FirstRow;
   LastRow  = Gbl.Stat.LastRow;
   if (FirstRow == 0 && LastRow == 0) // Call from main form
     {
      // Show last clicks
      FirstRow = (NumRows / Gbl.Stat.RowsPerPage - 1) * Gbl.Stat.RowsPerPage + 1;
      if ((FirstRow + Gbl.Stat.RowsPerPage - 1) < NumRows)
	 FirstRow += Gbl.Stat.RowsPerPage;
      LastRow = NumRows;
     }
   if (FirstRow < 1) // For security reasons; really it should never be less than 1
      FirstRow = 1;
   if (LastRow > NumRows)
      LastRow = NumRows;
   if ((LastRow - FirstRow) >= Gbl.Stat.RowsPerPage) // For if there have been clicks that have increased the number of rows
      LastRow = FirstRow + Gbl.Stat.RowsPerPage - 1;

   /***** Compute the number total of pages *****/
   /* Number of pages before the current one */
   NumPagesBefore = (FirstRow-1) / Gbl.Stat.RowsPerPage;
   if (NumPagesBefore * Gbl.Stat.RowsPerPage < (FirstRow-1))
      NumPagesBefore++;
   /* Number of pages after the current one */
   NumPagesAfter = (NumRows - LastRow) / Gbl.Stat.RowsPerPage;
   if (NumPagesAfter * Gbl.Stat.RowsPerPage < (NumRows - LastRow))
      NumPagesAfter++;
   /* Count the total number of pages */
   NumPagsTotal = NumPagesBefore + 1 + NumPagesAfter;

   /***** Put heading with backward and forward buttons *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"7\" class=\"LEFT_MIDDLE\">");
   Lay_StartTableWide (2);
   fprintf (Gbl.F.Out,"<tr>");

   /* Put link to jump to previous page (older clicks) */
   if (FirstRow > 1)
     {
      Act_FormStartAnchor (ActSeeAccCrs,Sta_STAT_RESULTS_SECTION_ID);
      Sta_WriteParamsDatesSeeAccesses ();
      Par_PutHiddenParamUnsigned ("GroupedBy",(unsigned) Sta_CLICKS_CRS_DETAILED_LIST);
      Par_PutHiddenParamUnsigned ("StatAct"  ,(unsigned) Gbl.Stat.NumAction);
      Par_PutHiddenParamLong ("FirstRow",FirstRow - Gbl.Stat.RowsPerPage);
      Par_PutHiddenParamLong ("LastRow" ,FirstRow - 1);
      Par_PutHiddenParamLong ("RowsPage",Gbl.Stat.RowsPerPage);
      Usr_PutHiddenParUsrCodAll (ActSeeAccCrs,Gbl.Usrs.Select[Rol_UNK]);
     }
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\" style=\"width:20%%;\">");
   if (FirstRow > 1)
     {
      sprintf (Gbl.Title,Txt_Show_previous_X_clicks,
               Gbl.Stat.RowsPerPage);
      Act_LinkFormSubmit (Gbl.Title,"TIT_TBL",NULL);
      fprintf (Gbl.F.Out,"<strong>&lt;%s</strong></a>",
               Txt_PAGES_Previous);
     }
   fprintf (Gbl.F.Out,"</td>");
   if (FirstRow > 1)
      Act_FormEnd ();

   /* Write number of current page */
   fprintf (Gbl.F.Out,"<td class=\"DAT_N CENTER_MIDDLE\" style=\"width:60%%;\">"
                      "<strong>"
                      "%s %lu-%lu %s %lu (%s %ld %s %lu)"
                      "</strong>"
                      "</td>",
            Txt_Clicks,
            FirstRow,LastRow,Txt_of_PART_OF_A_TOTAL,NumRows,
            Txt_page,NumPagesBefore + 1,Txt_of_PART_OF_A_TOTAL,NumPagsTotal);

   /* Put link to jump to next page (more recent clicks) */
   if (LastRow < NumRows)
     {
      Act_FormStartAnchor (ActSeeAccCrs,Sta_STAT_RESULTS_SECTION_ID);
      Sta_WriteParamsDatesSeeAccesses ();
      Par_PutHiddenParamUnsigned ("GroupedBy",(unsigned) Sta_CLICKS_CRS_DETAILED_LIST);
      Par_PutHiddenParamUnsigned ("StatAct"  ,(unsigned) Gbl.Stat.NumAction);
      Par_PutHiddenParamUnsigned ("FirstRow" ,(unsigned) (LastRow + 1));
      Par_PutHiddenParamUnsigned ("LastRow"  ,(unsigned) (LastRow + Gbl.Stat.RowsPerPage));
      Par_PutHiddenParamUnsigned ("RowsPage" ,(unsigned) Gbl.Stat.RowsPerPage);
      Usr_PutHiddenParUsrCodAll (ActSeeAccCrs,Gbl.Usrs.Select[Rol_UNK]);
     }
   fprintf (Gbl.F.Out,"<td class=\"RIGHT_MIDDLE\" style=\"width:20%%;\">");
   if (LastRow < NumRows)
     {
      sprintf (Gbl.Title,Txt_Show_next_X_clicks,
               Gbl.Stat.RowsPerPage);
      Act_LinkFormSubmit (Gbl.Title,"TIT_TBL",NULL);
      fprintf (Gbl.F.Out,"<strong>%s&gt;</strong>"
	                 "</a>",
               Txt_PAGES_Next);
     }
   fprintf (Gbl.F.Out,"</td>");
   if (LastRow < NumRows)
      Act_FormEnd ();

   fprintf (Gbl.F.Out,"</tr>");
   Lay_EndTable ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"RIGHT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\" style=\"width:10%%;\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_No_INDEX,
            Txt_User_ID,
            Txt_Name,
            Txt_Type,
            Txt_Date,
            Txt_Action,
            Txt_LOG_More_info);

   /***** Write rows back *****/
   for (NumRow = LastRow, UniqueId = 1, Gbl.RowEvenOdd = 0;
	NumRow >= FirstRow;
	NumRow--, UniqueId++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
     {
      mysql_data_seek (mysql_res,(my_ulonglong) (NumRow - 1));
      row = mysql_fetch_row (mysql_res);

      /* Get log code */
      LogCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get user's data of the database */
      UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[1]);
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);

      /* Get logged role */
      if (sscanf (row[2],"%u",&RoleFromLog) != 1)
	 Lay_ShowErrorAndExit ("Wrong user's role.");

      /* Write the number of row */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LOG RIGHT_TOP COLOR%u\">"
	                 "%ld&nbsp;"
	                 "</td>",
	       Gbl.RowEvenOdd,NumRow);

      /* Write the user's ID if user is a student */
      fprintf (Gbl.F.Out,"<td class=\"LOG CENTER_TOP COLOR%u\">",
	       Gbl.RowEvenOdd);
      ID_WriteUsrIDs (&UsrDat,NULL);
      fprintf (Gbl.F.Out,"&nbsp;</td>");

      /* Write the first name and the surnames */
      fprintf (Gbl.F.Out,"<td class=\"LOG LEFT_TOP COLOR%u\">"
	                 "%s&nbsp;"
	                 "</td>",
	       Gbl.RowEvenOdd,UsrDat.FullName);

      /* Write the user's role */
      fprintf (Gbl.F.Out,"<td class=\"LOG CENTER_TOP COLOR%u\">"
	                 "%s&nbsp;"
	                 "</td>",
	       Gbl.RowEvenOdd,
	       RoleFromLog < Rol_NUM_ROLES ? Txt_ROLES_SINGUL_Abc[RoleFromLog][UsrDat.Sex] :
		                             "?");

      /* Write the date-time (row[3]) */
      fprintf (Gbl.F.Out,"<td id=\"log_date_%u\" class=\"LOG CENTER_TOP COLOR%u\">"
			 "<script type=\"text/javascript\">"
			 "writeLocalDateHMSFromUTC('log_date_%u',%ld,"
			 "%u,',&nbsp;','%s',true,false,0x7);"
			 "</script>"
			 "</td>",
               UniqueId,Gbl.RowEvenOdd,
               UniqueId,(long) Dat_GetUNIXTimeFromStr (row[3]),
               (unsigned) Gbl.Prefs.DateFormat,Txt_Today);

      /* Write the action */
      if (sscanf (row[4],"%ld",&ActCod) != 1)
	 Lay_ShowErrorAndExit ("Wrong action code.");
      if (ActCod >= 0)
         fprintf (Gbl.F.Out,"<td class=\"LOG LEFT_TOP COLOR%u\">"
                            "%s&nbsp;"
                            "</td>",
	          Gbl.RowEvenOdd,
	          Act_GetActionTextFromDB (ActCod,ActTxt));
      else
         fprintf (Gbl.F.Out,"<td class=\"LOG LEFT_TOP COLOR%u\">"
                            "?&nbsp;"
                            "</td>",
	          Gbl.RowEvenOdd);
      /* Write the comments of the access */
      fprintf (Gbl.F.Out,"<td class=\"LOG LEFT_TOP COLOR%u\">",
               Gbl.RowEvenOdd);
      Sta_WriteLogComments (LogCod);
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");
     }

   /***** Free memory used by the data of the user *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/******** Show a listing of with the number of clicks of each user ***********/
/*****************************************************************************/

static void Sta_WriteLogComments (long LogCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get log comments from database *****/
   sprintf (Query,"SELECT Comments FROM log_comments WHERE LogCod=%ld",
            LogCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get log comments"))
     {
      /***** Get and write comments *****/
      row = mysql_fetch_row (mysql_res);
      fprintf (Gbl.F.Out,"%s",row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********* Show a listing of with the number of clicks of each user **********/
/*****************************************************************************/

static void Sta_ShowNumHitsPerUsr (unsigned long NumRows,MYSQL_RES *mysql_res)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Photo;
   extern const char *Txt_ID;
   extern const char *Txt_Name;
   extern const char *Txt_Type;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   MYSQL_ROW row;
   unsigned long NumRow;
   struct Sta_Hits Hits;
   unsigned BarWidth;
   struct UsrData UsrDat;
   char PhotoURL[PATH_MAX + 1];
   bool ShowPhoto;

   /***** Initialize user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"RIGHT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th colspan=\"2\" class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_No_INDEX,
            Txt_Photo,
            Txt_ID,
            Txt_Name,
            Txt_Type,
            Txt_STAT_TYPE_COUNT_CAPS[Gbl.Stat.CountType]);

   /***** Write rows *****/
   for (NumRow = 1, Hits.Max = 0.0, Gbl.RowEvenOdd = 0;
	NumRow <= NumRows;
	NumRow++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get user's data from the database */
      UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);	// Get the data of the user from the database

      /* Write the number of row */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LOG RIGHT_TOP COLOR%u\">"
	                 "%ld&nbsp;"
	                 "</td>",
	       Gbl.RowEvenOdd,NumRow);

      /* Show the photo */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_TOP COLOR%u\">",
	       Gbl.RowEvenOdd);
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                                            NULL,
                        "PHOTO15x20",Pho_ZOOM,false);
      fprintf (Gbl.F.Out,"</td>");

      /* Write the user's ID if user is a student in current course */
      fprintf (Gbl.F.Out,"<td class=\"LOG LEFT_TOP COLOR%u\">",
	       Gbl.RowEvenOdd);
      ID_WriteUsrIDs (&UsrDat,NULL);
      fprintf (Gbl.F.Out,"&nbsp;</td>");

      /* Write the name and the surnames */
      fprintf (Gbl.F.Out,"<td class=\"LOG LEFT_TOP COLOR%u\">"
	                 "%s&nbsp;"
	                 "</td>",
	       Gbl.RowEvenOdd,UsrDat.FullName);

      /* Write user's role */
      fprintf (Gbl.F.Out,"<td class=\"LOG CENTER_TOP COLOR%u\">"
	                 "%s&nbsp;"
	                 "</td>",
	       Gbl.RowEvenOdd,
	       Txt_ROLES_SINGUL_Abc[UsrDat.RoleInCurrentCrsDB][UsrDat.Sex]);

      /* Write the number of clicks */
      Hits.Num = Str_GetFloatNumFromStr (row[1]);
      if (NumRow == 1)
	 Hits.Max = Hits.Num;
      if (Hits.Max > 0.0)
        {
         BarWidth = (unsigned) (((Hits.Num * 375.0) / Hits.Max) + 0.5);
         if (BarWidth == 0)
            BarWidth = 1;
        }
      else
         BarWidth = 0;
      fprintf (Gbl.F.Out,"<td class=\"LOG LEFT_TOP COLOR%u\">",
	       Gbl.RowEvenOdd);
      if (BarWidth)
	 fprintf (Gbl.F.Out,"<img src=\"%s/%c1x14.gif\""
	                    " alt=\"\" title=\"\""
                            " class=\"LEFT_TOP\""
	                    " style=\"width:%upx; height:18px;\" />"
	                    "&nbsp;",
		  Gbl.Prefs.IconsURL,
		  UsrDat.RoleInCurrentCrsDB == Rol_STD ? 'c' :	// Student
			                                 'v',	// Non-editing teacher or teacher
		  BarWidth);
      Str_WriteFloatNum (Gbl.F.Out,Hits.Num);
      fprintf (Gbl.F.Out,"&nbsp;</td>"
	                 "</tr>");
     }

   /***** Free memory used by the data of the user *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/********** Show a listing of with the number of clicks in each date *********/
/*****************************************************************************/

static void Sta_ShowNumHitsPerDays (unsigned long NumRows,MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Date;
   extern const char *Txt_Day;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   extern const char *Txt_DAYS_SMALL[7];
   unsigned long NumRow;
   struct Date ReadDate;
   struct Date LastDate;
   struct Date Date;
   unsigned D;
   unsigned NumDaysFromLastDateToCurrDate;
   int NumDayWeek;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   char StrDate[Cns_MAX_BYTES_DATE + 1];

   /***** Initialize LastDate *****/
   Dat_AssignDate (&LastDate,&Gbl.DateRange.DateEnd.Date);

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Date,
            Txt_Day,
            Txt_STAT_TYPE_COUNT_CAPS[Gbl.Stat.CountType]);

   /***** Compute maximum number of pages generated per day *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,1,1);

   /***** Write rows beginning by the most recent day and ending by the oldest *****/
   mysql_data_seek (mysql_res,0);
   for (NumRow = 1;
	NumRow <= NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get year, month and day (row[0] holds the date in YYYYMMDD format) */
      if (!(Dat_GetDateFromYYYYMMDD (&ReadDate,row[0])))
	 Lay_ShowErrorAndExit ("Wrong date.");

      /* Get number of pages generated (in row[1]) */
      Hits.Num = Str_GetFloatNumFromStr (row[1]);

      Dat_AssignDate (&Date,&LastDate);
      NumDaysFromLastDateToCurrDate = Dat_GetNumDaysBetweenDates (&ReadDate,&LastDate);
      /* In the next loop (NumDaysFromLastDateToCurrDate-1) días (the more recent) with 0 clicks are shown
         and a last day (the oldest) with Hits.Num */
      for (D = 1;
	   D <= NumDaysFromLastDateToCurrDate;
	   D++)
        {
         NumDayWeek = Dat_GetDayOfWeek (Date.Year,Date.Month,Date.Day);

         /* Write the date */
	 Dat_ConvDateToDateStr (&Date,StrDate);
         fprintf (Gbl.F.Out,"<tr>"
                            "<td class=\"%s LEFT_TOP\">"
                            "%s&nbsp;"
                            "</td>",
	          NumDayWeek == 6 ? "LOG_R" :
	        	            "LOG",
	          StrDate);

         /* Write the day of the week */
         fprintf (Gbl.F.Out,"<td class=\"%s LEFT_TOP\">"
                            "%s&nbsp;"
                            "</td>",
                  NumDayWeek == 6 ? "LOG_R" :
                	            "LOG",
                  Txt_DAYS_SMALL[NumDayWeek]);

         /* Draw bar proportional to number of pages generated */
         Sta_DrawBarNumHits (NumDayWeek == 6 ? 'r' :
                                               'c',
                             D == NumDaysFromLastDateToCurrDate ? Hits.Num :
                        	                                  0.0,
                             Hits.Max,Hits.Total,500);

         /* Decrease day */
         Dat_GetDateBefore (&Date,&Date);
        }
      Dat_AssignDate (&LastDate,&Date);
     }
   NumDaysFromLastDateToCurrDate = Dat_GetNumDaysBetweenDates (&Gbl.DateRange.DateIni.Date,&LastDate);

   /***** Finally NumDaysFromLastDateToCurrDate days are shown with 0 clicks
          (the oldest days from the requested initial day until the first with clicks) *****/
   for (D = 1;
	D <= NumDaysFromLastDateToCurrDate;
	D++)
     {
      NumDayWeek = Dat_GetDayOfWeek (Date.Year,Date.Month,Date.Day);

      /* Write the date */
      Dat_ConvDateToDateStr (&Date,StrDate);
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"%s RIGHT_TOP\">"
	                 "%s&nbsp;"
	                 "</td>",
               NumDayWeek == 6 ? "LOG_R" :
        	                 "LOG",
               StrDate);

      /* Write the day of the week */
      fprintf (Gbl.F.Out,"<td class=\"%s LEFT_TOP\">"
	                 "%s&nbsp;"
	                 "</td>",
               NumDayWeek == 6 ? "LOG_R" :
        	                 "LOG",
               Txt_DAYS_SMALL[NumDayWeek]);

      /* Draw bar proportional to number of pages generated */
      Sta_DrawBarNumHits (NumDayWeek == 6 ? 'r' :
	                                    'c',
	                  0.0,Hits.Max,Hits.Total,500);

      /* Decrease day */
      Dat_GetDateBefore (&Date,&Date);
     }
  }

/*****************************************************************************/
/************ Show graphic of number of pages generated per hour *************/
/*****************************************************************************/

#define GRAPH_DISTRIBUTION_PER_HOUR_HOUR_WIDTH 25
#define GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH (GRAPH_DISTRIBUTION_PER_HOUR_HOUR_WIDTH * 24)

static void Sta_ShowDistrAccessesPerDaysAndHour (unsigned long NumRows,MYSQL_RES *mysql_res)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Color_of_the_graphic;
   extern const char *Txt_STAT_COLOR_TYPES[Sta_NUM_COLOR_TYPES];
   extern const char *Txt_Date;
   extern const char *Txt_Day;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   extern const char *Txt_DAYS_SMALL[7];
   Sta_ColorType_t ColorType;
   Sta_ColorType_t SelectedColorType;
   unsigned long NumRow;
   struct Date PreviousReadDate;
   struct Date CurrentReadDate;
   struct Date LastDate;
   struct Date Date;
   unsigned D;
   unsigned NumDaysFromLastDateToCurrDate = 1;
   unsigned NumDayWeek;
   unsigned Hour;
   unsigned ReadHour = 0;
   struct Sta_Hits Hits;
   float NumAccPerHour[24];
   float NumAccPerHourZero[24];
   MYSQL_ROW row;
   char StrDate[Cns_MAX_BYTES_DATE + 1];

   /***** Get selected color type *****/
   SelectedColorType = Sta_GetStatColorType ();

   /***** Put a selector for the type of color *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"26\" class=\"CENTER_MIDDLE\">");

   Act_FormStartAnchor (Gbl.Action.Act,Sta_STAT_RESULTS_SECTION_ID);
   Sta_WriteParamsDatesSeeAccesses ();
   Par_PutHiddenParamUnsigned ("GroupedBy",(unsigned) Gbl.Stat.ClicksGroupedBy);
   Par_PutHiddenParamUnsigned ("CountType",(unsigned) Gbl.Stat.CountType);
   Par_PutHiddenParamUnsigned ("StatAct"  ,(unsigned) Gbl.Stat.NumAction);
   if (Gbl.Action.Act == ActSeeAccCrs)
      Usr_PutHiddenParUsrCodAll (ActSeeAccCrs,Gbl.Usrs.Select[Rol_UNK]);
   else // Gbl.Action.Act == ActSeeAccGbl
     {
      Par_PutHiddenParamUnsigned ("Role",(unsigned) Gbl.Stat.Role);
      Sta_PutHiddenParamScopeSta ();
     }

   fprintf (Gbl.F.Out,"<label class=\"%s\">%s:&nbsp;"
                      "<select name=\"ColorType\""
                      " onchange=\"document.getElementById('%s').submit();\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Color_of_the_graphic,
            Gbl.Form.Id);
   for (ColorType = (Sta_ColorType_t) 0;
	ColorType < Sta_NUM_COLOR_TYPES;
	ColorType++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) ColorType);
      if (ColorType == SelectedColorType)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s",Txt_STAT_COLOR_TYPES[ColorType]);
     }
   fprintf (Gbl.F.Out,"</select>"
	              "</label>");
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Compute maximum number of pages generated per day-hour *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,2,1);

   /***** Initialize LastDate *****/
   Dat_AssignDate (&LastDate,&Gbl.DateRange.DateEnd.Date);

   /***** Reset number of pages generated per hour *****/
   for (Hour = 0;
	Hour < 24;
	Hour++)
      NumAccPerHour[Hour] = NumAccPerHourZero[Hour] = 0.0;

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th rowspan=\"3\" class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "<th rowspan=\"3\" class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "<th colspan=\"24\" class=\"LEFT_TOP\""
                      " style=\"width:%upx;\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Date,
            Txt_Day,
            GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH,
            Txt_STAT_TYPE_COUNT_CAPS[Gbl.Stat.CountType]);
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"24\" class=\"LEFT_TOP\""
	              " style=\"width:%upx;\">",
	    GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH);
   Sta_DrawBarColors (SelectedColorType,Hits.Max);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>"
	              "<tr>");
   for (Hour = 0;
	Hour < 24;
	Hour++)
      fprintf (Gbl.F.Out,"<td class=\"LOG CENTER_TOP\" style=\"width:%upx;\">"
	                 "%02uh"
	                 "</td>",
               GRAPH_DISTRIBUTION_PER_HOUR_HOUR_WIDTH,Hour);
   fprintf (Gbl.F.Out,"</tr>");

   /***** Write rows beginning by the most recent day and ending by the oldest one *****/
   mysql_data_seek (mysql_res,0);

   for (NumRow = 1;
	NumRow <= NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get year, month and day (row[0] holds the date in YYYYMMDD format) */
      if (!(Dat_GetDateFromYYYYMMDD (&CurrentReadDate,row[0])))
	 Lay_ShowErrorAndExit ("Wrong date.");

      /* Get the hour (in row[1] is the hour in formato HH) */
      if (sscanf (row[1],"%02u",&ReadHour) != 1)
	 Lay_ShowErrorAndExit ("Wrong hour.");

      /* Get number of pages generated (in row[2]) */
      Hits.Num = Str_GetFloatNumFromStr (row[2]);

      /* If this is the first read date, initialize PreviousReadDate */
      if (NumRow == 1)
         Dat_AssignDate (&PreviousReadDate,&CurrentReadDate);

      /* Update number of hits per hour */
      if (PreviousReadDate.Year  != CurrentReadDate.Year  ||
          PreviousReadDate.Month != CurrentReadDate.Month ||
          PreviousReadDate.Day   != CurrentReadDate.Day)	// Current read date (CurrentReadDate) is older than previous read date (PreviousReadDate) */
        {
         /* In the next loop we show (NumDaysFromLastDateToCurrDate-1) days with 0 clicks
            and a last day (older) with Hits.Num */
         Dat_AssignDate (&Date,&LastDate);
         NumDaysFromLastDateToCurrDate = Dat_GetNumDaysBetweenDates (&PreviousReadDate,&LastDate);
         for (D = 1;
              D <= NumDaysFromLastDateToCurrDate;
              D++)
           {
            NumDayWeek = Dat_GetDayOfWeek (Date.Year,Date.Month,Date.Day);

            /* Write the date */
            Dat_ConvDateToDateStr (&Date,StrDate);
            fprintf (Gbl.F.Out,"<tr>"
        	               "<td class=\"%s LEFT_TOP\">"
        	               "%s&nbsp;"
        	               "</td>",
	             NumDayWeek == 6 ? "LOG_R" :
	        	               "LOG",
	             StrDate);

            /* Write the day of the week */
            fprintf (Gbl.F.Out,"<td class=\"%s LEFT_TOP\">"
        	               "%s&nbsp;"
        	               "</td>",
                     NumDayWeek == 6 ? "LOG_R" :
                	               "LOG",
                     Txt_DAYS_SMALL[NumDayWeek]);

            /* Draw a cell with the color proportional to the number of clicks */
            if (D == NumDaysFromLastDateToCurrDate)
               Sta_DrawAccessesPerHourForADay (SelectedColorType,NumAccPerHour,Hits.Max);
            else	// D < NumDaysFromLastDateToCurrDate
               Sta_DrawAccessesPerHourForADay (SelectedColorType,NumAccPerHourZero,Hits.Max);
            fprintf (Gbl.F.Out,"</tr>");

            /* Decrease day */
            Dat_GetDateBefore (&Date,&Date);
           }
         Dat_AssignDate (&LastDate,&Date);
         Dat_AssignDate (&PreviousReadDate,&CurrentReadDate);

         /* Reset number of pages generated per hour */
         for (Hour = 0;
              Hour < 24;
              Hour++)
            NumAccPerHour[Hour] = 0.0;
        }
      NumAccPerHour[ReadHour] = Hits.Num;
     }

   /***** Show the clicks of the oldest day with clicks *****/
   /* In the next loop we show (NumDaysFromLastDateToCurrDate-1) days (more recent) with 0 clicks
      and a last day (older) with Hits.Num clicks */
   Dat_AssignDate (&Date,&LastDate);
   NumDaysFromLastDateToCurrDate = Dat_GetNumDaysBetweenDates (&PreviousReadDate,&LastDate);
   for (D = 1;
	D <= NumDaysFromLastDateToCurrDate;
	D++)
     {
      NumDayWeek = Dat_GetDayOfWeek (Date.Year,Date.Month,Date.Day);

      /* Write the date */
      Dat_ConvDateToDateStr (&Date,StrDate);
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"%s RIGHT_TOP\">"
	                 "%s&nbsp;"
	                 "</td>",
               NumDayWeek == 6 ? "LOG_R" :
        	                 "LOG",
               StrDate);

      /* Write the day of the week */
      fprintf (Gbl.F.Out,"<td class=\"%s LEFT_TOP\">"
	                 "%s&nbsp;"
	                 "</td>",
               NumDayWeek == 6 ? "LOG_R" :
        	                 "LOG",
               Txt_DAYS_SMALL[NumDayWeek]);

      /* Draw the color proporcional al number of clicks */
      if (D == NumDaysFromLastDateToCurrDate)
         Sta_DrawAccessesPerHourForADay (SelectedColorType,NumAccPerHour,Hits.Max);
      else	// D < NumDaysFromLastDateToCurrDate
         Sta_DrawAccessesPerHourForADay (SelectedColorType,NumAccPerHourZero,Hits.Max);
      fprintf (Gbl.F.Out,"</tr>");

      /* Decrease day */
      Dat_GetDateBefore (&Date,&Date);
     }

   /***** Finally NumDaysFromLastDateToCurrDate days are shown with 0 clicks
          (the oldest days since the initial day requested by the user until the first with clicks) *****/
   Dat_AssignDate (&LastDate,&Date);
   NumDaysFromLastDateToCurrDate = Dat_GetNumDaysBetweenDates (&Gbl.DateRange.DateIni.Date,&LastDate);
   for (D = 1;
	D <= NumDaysFromLastDateToCurrDate;
	D++)
     {
      NumDayWeek = Dat_GetDayOfWeek (Date.Year,Date.Month,Date.Day);

      /* Write the date */
      Dat_ConvDateToDateStr (&Date,StrDate);
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"%s RIGHT_TOP\">"
	                 "%s&nbsp;"
	                 "</td>",
               NumDayWeek == 6 ? "LOG_R" :
        	                 "LOG",
               StrDate);

      /* Write the day of the week */
      fprintf (Gbl.F.Out,"<td class=\"%s LEFT_TOP\">"
	                 "%s&nbsp;"
	                 "</td>",
               NumDayWeek == 6 ? "LOG_R" :
        	                 "LOG",
               Txt_DAYS_SMALL[NumDayWeek]);

      /* Draw the color proportional to number of clicks */
      Sta_DrawAccessesPerHourForADay (SelectedColorType,NumAccPerHourZero,Hits.Max);
      fprintf (Gbl.F.Out,"</tr>");

      /* Decrease day */
      Dat_GetDateBefore (&Date,&Date);
     }
  }

/*****************************************************************************/
/********************** Get type of color for statistics *********************/
/*****************************************************************************/

static Sta_ColorType_t Sta_GetStatColorType (void)
  {
   return (Sta_ColorType_t)
	  Par_GetParToUnsignedLong ("ColorType",
	                            0,
	                            Sta_NUM_COLOR_TYPES - 1,
	                            (unsigned long) Sta_COLOR_TYPE_DEF);
  }

/*****************************************************************************/
/************************* Draw a bar with colors ****************************/
/*****************************************************************************/

static void Sta_DrawBarColors (Sta_ColorType_t ColorType,float HitsMax)
  {
   unsigned Interval;
   unsigned NumColor;
   unsigned R;
   unsigned G;
   unsigned B;

   /***** Write numbers from 0 to Hits.Max *****/
   Lay_StartTableWide (0);
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"%u\" class=\"LOG LEFT_BOTTOM\""
	              " style=\"width:%upx;\">"
	              "0"
	              "</td>",
            (GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH/5)/2,
            (GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH/5)/2);
   for (Interval = 1;
	Interval <= 4;
	Interval++)
     {
      fprintf (Gbl.F.Out,"<td colspan=\"%u\" class=\"LOG CENTER_BOTTOM\""
	                 " style=\"width:%upx;\">",
               GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH/5,
               GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH/5);
      Str_WriteFloatNum (Gbl.F.Out,(float) Interval * HitsMax / 5.0);
      fprintf (Gbl.F.Out,"</td>");
     }
   fprintf (Gbl.F.Out,"<td colspan=\"%u\" class=\"LOG RIGHT_BOTTOM\""
	              " style=\"width:%upx;\">",
            (GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH/5)/2,
            (GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH/5)/2);
   Str_WriteFloatNum (Gbl.F.Out,HitsMax);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>"
	              "<tr>");

   /***** Draw colors *****/
   for (NumColor = 0;
	NumColor < GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH;
	NumColor++)
     {
      Sta_SetColor (ColorType,(float) NumColor,(float) GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH,&R,&G,&B);
      fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\" style=\"width:1px;"
	                 " background-color:#%02X%02X%02X;\">"
	                 "<img src=\"%s/tr1x14.gif\""
	                 " alt=\"\" title=\"\" />"
	                 "</td>",
               R,G,B,Gbl.Prefs.IconsURL);
     }
   fprintf (Gbl.F.Out,"</tr>");
   Lay_EndTable ();
  }

/*****************************************************************************/
/********************* Draw accesses per hour for a day **********************/
/*****************************************************************************/

static void Sta_DrawAccessesPerHourForADay (Sta_ColorType_t ColorType,float HitsNum[24],float HitsMax)
  {
   unsigned Hour;
   unsigned R;
   unsigned G;
   unsigned B;

   for (Hour = 0;
	Hour < 24;
	Hour++)
     {
      Sta_SetColor (ColorType,HitsNum[Hour],HitsMax,&R,&G,&B);
      fprintf (Gbl.F.Out,"<td class=\"LOG LEFT_MIDDLE\" title=\"");
      Str_WriteFloatNum (Gbl.F.Out,HitsNum[Hour]);
      fprintf (Gbl.F.Out,"\" style=\"width:%upx;"
	                 " background-color:#%02X%02X%02X;\">"
                         "</td>",
               GRAPH_DISTRIBUTION_PER_HOUR_HOUR_WIDTH,R,G,B);
     }
  }

/*****************************************************************************/
/************************* Set color depending on ratio **********************/
/*****************************************************************************/
// Hits.Max must be > 0
/*
Black         Blue         Cyan        Green        Yellow        Red
  +------------+------------+------------+------------+------------+
  |     0.2    |     0.2    |     0.2    |     0.2    |     0.2    |
  +------------+------------+------------+------------+------------+
 0.0          0.2          0.4          0.6          0.8          1.0
*/

static void Sta_SetColor (Sta_ColorType_t ColorType,float HitsNum,float HitsMax,
                          unsigned *R,unsigned *G,unsigned *B)
  {
   float Result = (HitsNum / HitsMax);

   switch (ColorType)
     {
      case Sta_COLOR:
         if (Result < 0.2)		// Black -> Blue
           {
            *R = 0;
            *G = 0;
            *B = (unsigned) (Result * 256.0 / 0.2 + 0.5);
            if (*B == 256)
               *B = 255;
           }
         else if (Result < 0.4)	// Blue -> Cyan
           {
            *R = 0;
            *G = (unsigned) ((Result-0.2) * 256.0 / 0.2 + 0.5);
            if (*G == 256)
               *G = 255;
            *B = 255;
           }
         else if (Result < 0.6)	// Cyan -> Green
           {
            *R = 0;
            *G = 255;
            *B = 256 - (unsigned) ((Result-0.4) * 256.0 / 0.2 + 0.5);
            if (*B == 256)
               *B = 255;
           }
         else if (Result < 0.8)	// Green -> Yellow
           {
            *R = (unsigned) ((Result-0.6) * 256.0 / 0.2 + 0.5);
            if (*R == 256)
               *R = 255;
            *G = 255;
            *B = 0;
           }
         else			// Yellow -> Red
           {
            *R = 255;
            *G = 256 - (unsigned) ((Result-0.8) * 256.0 / 0.2 + 0.5);
            if (*G == 256)
               *G = 255;
            *B = 0;
           }
         break;
      case Sta_BLACK_TO_WHITE:
         *B = (unsigned) (Result * 256.0 + 0.5);
         if (*B == 256)
            *B = 255;
         *R = *G = *B;
         break;
      case Sta_WHITE_TO_BLACK:
         *B = 256 - (unsigned) (Result * 256.0 + 0.5);
         if (*B == 256)
            *B = 255;
         *R = *G = *B;
         break;
     }
  }

/*****************************************************************************/
/********** Show listing with number of pages generated per week *************/
/*****************************************************************************/

static void Sta_ShowNumHitsPerWeeks (unsigned long NumRows,
                                     MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Week;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned long NumRow;
   struct Date ReadDate;
   struct Date LastDate;
   struct Date Date;
   unsigned W;
   unsigned NumWeeksBetweenLastDateAndCurrentDate;
   struct Sta_Hits Hits;
   MYSQL_ROW row;

   /***** Initialize LastDate to avoid warning *****/
   Dat_CalculateWeekOfYear (&Gbl.DateRange.DateEnd.Date);	// Changes Week and Year
   Dat_AssignDate (&LastDate,&Gbl.DateRange.DateEnd.Date);

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Week,
            Txt_STAT_TYPE_COUNT_CAPS[Gbl.Stat.CountType]);

   /***** Compute maximum number of pages generated per week *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumRow = 1;
	NumRow <= NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get year and week (row[0] holds date in YYYYWW format) */
      if (sscanf (row[0],"%04u%02u",&ReadDate.Year,&ReadDate.Week) != 2)
	 Lay_ShowErrorAndExit ("Wrong date.");

      /* Get number of pages generated (in row[1]) */
      Hits.Num = Str_GetFloatNumFromStr (row[1]);

      Dat_AssignDate (&Date,&LastDate);
      NumWeeksBetweenLastDateAndCurrentDate = Dat_GetNumWeeksBetweenDates (&ReadDate,&LastDate);
      for (W = 1;
	   W <= NumWeeksBetweenLastDateAndCurrentDate;
	   W++)
        {
         /* Write week */
         fprintf (Gbl.F.Out,"<tr>"
                            "<td class=\"LOG LEFT_TOP\">"
                            "%04u-%02u&nbsp;"
                            "</td>",
	          Date.Year,Date.Week);

         /* Draw bar proportional to number of pages generated */
         Sta_DrawBarNumHits ('c',
                             W == NumWeeksBetweenLastDateAndCurrentDate ? Hits.Num :
                        	                                          0.0,
                             Hits.Max,Hits.Total,500);

         /* Decrement week */
         Dat_GetWeekBefore (&Date,&Date);
        }
      Dat_AssignDate (&LastDate,&Date);
     }

  /***** Finally, show the old weeks without pages generated *****/
  Dat_CalculateWeekOfYear (&Gbl.DateRange.DateIni.Date);	// Changes Week and Year
  NumWeeksBetweenLastDateAndCurrentDate = Dat_GetNumWeeksBetweenDates (&Gbl.DateRange.DateIni.Date,&LastDate);
  for (W = 1;
       W <= NumWeeksBetweenLastDateAndCurrentDate;
       W++)
    {
     /* Write week */
     fprintf (Gbl.F.Out,"<tr>"
	                "<td class=\"LOG LEFT_TOP\">"
	                "%04u-%02u&nbsp;"
	                "</td>",
              Date.Year,Date.Week);

     /* Draw bar proportional to number of pages generated */
     Sta_DrawBarNumHits ('c',0.0,Hits.Max,Hits.Total,500);

     /* Decrement week */
     Dat_GetWeekBefore (&Date,&Date);
    }
  }

/*****************************************************************************/
/********** Show a graph with the number of clicks in each month *************/
/*****************************************************************************/

static void Sta_ShowNumHitsPerMonths (unsigned long NumRows,
                                      MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Month;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned long NumRow;
   struct Date ReadDate;
   struct Date LastDate;
   struct Date Date;
   unsigned M;
   unsigned NumMonthsBetweenLastDateAndCurrentDate;
   struct Sta_Hits Hits;
   MYSQL_ROW row;

   /***** Initialize LastDate *****/
   Dat_AssignDate (&LastDate,&Gbl.DateRange.DateEnd.Date);

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Month,
            Txt_STAT_TYPE_COUNT_CAPS[Gbl.Stat.CountType]);

   /***** Compute maximum number of pages generated per month *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumRow = 1;
	NumRow <= NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get the year and the month (in row[0] is the date in YYYYMM format) */
      if (sscanf (row[0],"%04u%02u",&ReadDate.Year,&ReadDate.Month) != 2)
	 Lay_ShowErrorAndExit ("Wrong date.");

      /* Get number of pages generated (in row[1]) */
      Hits.Num = Str_GetFloatNumFromStr (row[1]);

      Dat_AssignDate (&Date,&LastDate);
      NumMonthsBetweenLastDateAndCurrentDate = Dat_GetNumMonthsBetweenDates (&ReadDate,&LastDate);
      for (M = 1;
	   M <= NumMonthsBetweenLastDateAndCurrentDate;
	   M++)
        {
         /* Write the month */
         fprintf (Gbl.F.Out,"<tr>"
                            "<td class=\"LOG LEFT_TOP\">"
                            "%04u-%02u&nbsp;"
                            "</td>",
	          Date.Year,Date.Month);

         /* Draw bar proportional to number of pages generated */
         Sta_DrawBarNumHits ('c',
                             M == NumMonthsBetweenLastDateAndCurrentDate ? Hits.Num :
                        	                                           0.0,
                             Hits.Max,Hits.Total,500);

         /* Decrease month */
         Dat_GetMonthBefore (&Date,&Date);
        }
      Dat_AssignDate (&LastDate,&Date);
     }

  /***** Finally, show the oldest months without clicks *****/
  NumMonthsBetweenLastDateAndCurrentDate = Dat_GetNumMonthsBetweenDates (&Gbl.DateRange.DateIni.Date,&LastDate);
  for (M = 1;
       M <= NumMonthsBetweenLastDateAndCurrentDate;
       M++)
    {
     /* Write the month */
     fprintf (Gbl.F.Out,"<tr>"
	                "<td class=\"LOG LEFT_TOP\">"
	                "%04u-%02u&nbsp;"
	                "</td>",
              Date.Year,Date.Month);

     /* Draw bar proportional to number of pages generated */
     Sta_DrawBarNumHits ('c',0.0,Hits.Max,Hits.Total,500);

     /* Decrease month */
     Dat_GetMonthBefore (&Date,&Date);
    }
  }

/*****************************************************************************/
/**************** Show graphic of number of pages generated per hour ***************/
/*****************************************************************************/

#define DIGIT_WIDTH 6

static void Sta_ShowNumHitsPerHour (unsigned long NumRows,
                                    MYSQL_RES *mysql_res)
  {
   unsigned long NumRow;
   struct Sta_Hits Hits;
   unsigned NumDays;
   unsigned Hour = 0;
   unsigned ReadHour = 0;
   unsigned H;
   unsigned NumDigits;
   unsigned ColumnWidth;
   MYSQL_ROW row;

   if ((NumDays = Dat_GetNumDaysBetweenDates (&Gbl.DateRange.DateIni.Date,&Gbl.DateRange.DateEnd.Date)))
     {
      /***** Compute maximum number of pages generated per hour *****/
      Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,1,NumDays);

      /***** Compute width of columns (one for each hour) *****/
      /* Maximum number of dígits. If less than 4, set it to 4 to ensure a minimum width */
      NumDigits = (Hits.Max >= 1000) ? (unsigned) floor (log10 ((double) Hits.Max)) + 1 :
	                               4;
      ColumnWidth = NumDigits * DIGIT_WIDTH + 2;

      /***** Draw the graphic *****/
      mysql_data_seek (mysql_res,0);
      NumRow = 1;
      fprintf (Gbl.F.Out,"<tr>");
      while (Hour < 24)
	{
	 Hits.Num = 0.0;
	 if (NumRow <= NumRows)	// If not read yet all the results of the query
	   {
	    row = mysql_fetch_row (mysql_res); // Get next result
	    NumRow++;
	    if (sscanf (row[0],"%02u",&ReadHour) != 1)   // In row[0] is the date in HH format
	       Lay_ShowErrorAndExit ("Wrong hour.");

	    for (H = Hour;
		 H < ReadHour;
		 H++, Hour++)
	       Sta_WriteAccessHour (H,&Hits,ColumnWidth);

	    Hits.Num = Str_GetFloatNumFromStr (row[1]) / (float) NumDays;
	    Sta_WriteAccessHour (ReadHour,&Hits,ColumnWidth);

	    Hour++;
	   }
	 else
	    for (H = ReadHour + 1;
		 H < 24;
		 H++, Hour++)
	       Sta_WriteAccessHour (H,&Hits,ColumnWidth);
	}
      fprintf (Gbl.F.Out,"</tr>");
     }
  }

/*****************************************************************************/
/**** Write a column of the graphic of the number of clicks in each hour *****/
/*****************************************************************************/

static void Sta_WriteAccessHour (unsigned Hour,struct Sta_Hits *Hits,unsigned ColumnWidth)
  {
   unsigned BarHeight;

   fprintf (Gbl.F.Out,"<td class=\"DAT_SMALL CENTER_BOTTOM\""
	              " style=\"width:%upx;\">",
	    ColumnWidth);

   /* Draw bar with a height porportional to the number of clicks */
   if (Hits->Num > 0.0)
     {
      fprintf (Gbl.F.Out,"%u%%<br />",
	       (unsigned) (((Hits->Num * 100.0) /
		            Hits->Total) + 0.5));
      Str_WriteFloatNum (Gbl.F.Out,Hits->Num);
      fprintf (Gbl.F.Out,"<br />");
      BarHeight = (unsigned) (((Hits->Num * 500.0) / Hits->Max) + 0.5);
      if (BarHeight == 0)
         BarHeight = 1;
      fprintf (Gbl.F.Out,"<img src=\"%s/c8x1.gif\""
	                 " alt=\"\" title=\"\""
	                 " style=\"width:10px; height:%upx;\" />",
	       Gbl.Prefs.IconsURL,BarHeight);
     }
   else
      fprintf (Gbl.F.Out,"0%%<br />0");

   /* Write the hour */
   fprintf (Gbl.F.Out,"<br />%uh</td>",Hour);
  }

/*****************************************************************************/
/**** Show a listing with the number of clicks in every minute of the day ***/
/*****************************************************************************/

#define Sta_NUM_MINUTES_PER_DAY		(60 * 24)	// 1440 minutes in a day
#define Sta_WIDTH_SEMIDIVISION_GRAPHIC	30
#define Sta_NUM_DIVISIONS_X		10

static void Sta_ShowAverageAccessesPerMinute (unsigned long NumRows,MYSQL_RES *mysql_res)
  {
   unsigned long NumRow = 1;
   MYSQL_ROW row;
   unsigned NumDays;
   unsigned MinuteDay = 0;
   unsigned ReadHour;
   unsigned MinuteRead;
   unsigned MinuteDayRead = 0;
   unsigned i;
   struct Sta_Hits Hits;
   float NumClicksPerMin[Sta_NUM_MINUTES_PER_DAY];
   float Power10LeastOrEqual;
   float MaxX;
   float IncX;
   char *Format;

   if ((NumDays = Dat_GetNumDaysBetweenDates (&Gbl.DateRange.DateIni.Date,&Gbl.DateRange.DateEnd.Date)))
     {
      /***** Compute number of clicks (and máximo) in every minute *****/
      Hits.Max = 0.0;
      while (MinuteDay < Sta_NUM_MINUTES_PER_DAY)
	{
	 if (NumRow <= NumRows)	// If not all the result of the query are yet read
	   {
	    row = mysql_fetch_row (mysql_res); // Get next result
	    NumRow++;
	    if (sscanf (row[0],"%02u%02u",&ReadHour,&MinuteRead) != 2)   // In row[0] is the date in formato HHMM
	       Lay_ShowErrorAndExit ("Wrong hour-minute.");
	    /* Get number of pages generated */
	    Hits.Num = Str_GetFloatNumFromStr (row[1]);
	    MinuteDayRead = ReadHour * 60 + MinuteRead;
	    for (i = MinuteDay;
		 i < MinuteDayRead;
		 i++, MinuteDay++)
	       NumClicksPerMin[i] = 0.0;
	    NumClicksPerMin[MinuteDayRead] = Hits.Num / (float) NumDays;
	    if (NumClicksPerMin[MinuteDayRead] > Hits.Max)
	       Hits.Max = NumClicksPerMin[MinuteDayRead];
	    MinuteDay++;
	   }
	 else
	    for (i = MinuteDayRead + 1;
		 i < Sta_NUM_MINUTES_PER_DAY;
		 i++, MinuteDay++)
	       NumClicksPerMin[i] = 0.0;
	}

      /***** Compute the maximum value of X and the increment of the X axis *****/
      if (Hits.Max <= 0.000001)
	 MaxX = 0.000001;
      else
	{
	 Power10LeastOrEqual = (float) pow (10.0,floor (log10 ((double) Hits.Max)));
	 MaxX = ceil (Hits.Max / Power10LeastOrEqual) * Power10LeastOrEqual;
	}
      IncX = MaxX / (float) Sta_NUM_DIVISIONS_X;
      if (IncX >= 1.0)
	 Format = "%.0f";
      else if (IncX >= 0.1)
	 Format = "%.1f";
      else if (IncX >= 0.01)
	 Format = "%.2f";
      else if (IncX >= 0.001)
	 Format = "%.3f";
      else
	 Format = "%f";

      /***** X axis tags *****/
      Sta_WriteLabelsXAxisAccMin (IncX,Format);

      /***** Y axis and graphic *****/
      for (i = 0;
	   i < Sta_NUM_MINUTES_PER_DAY;
	   i++)
	 Sta_WriteAccessMinute (i,NumClicksPerMin[i],MaxX);

      /***** X axis *****/
      /* First division (left) */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LEFT_MIDDLE\" style=\"width:%upx;\">"
	                 "<img src=\"%s/ejexizq24x1.gif\""
	                 " alt=\"\" title=\"\""
	                 " style=\"display:block; width:%upx; height:1px;\" />"
	                 "</td>",
	       Sta_WIDTH_SEMIDIVISION_GRAPHIC,Gbl.Prefs.IconsURL,
	       Sta_WIDTH_SEMIDIVISION_GRAPHIC);
      /* All the intermediate divisions */
      for (i = 0;
	   i < Sta_NUM_DIVISIONS_X * 2;
	   i++)
	 fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\" style=\"width:%upx;\">"
	                    "<img src=\"%s/ejex24x1.gif\""
	                    " alt=\"\" title=\"\""
	                    " style=\"display:block;"
	                    " width:%upx; height:1px;\" />"
	                    "</td>",
		  Sta_WIDTH_SEMIDIVISION_GRAPHIC,Gbl.Prefs.IconsURL,
		  Sta_WIDTH_SEMIDIVISION_GRAPHIC);
      /* Last division (right) */
      fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\" style=\"width:%upx;\">"
	                 "<img src=\"%s/tr24x1.gif\""
	                 " alt=\"\" title=\"\""
	                 " style=\"display:block; width:%upx; height:1px;\" />"
	                 "</td>"
	                 "</tr>",
	       Sta_WIDTH_SEMIDIVISION_GRAPHIC,Gbl.Prefs.IconsURL,
	       Sta_WIDTH_SEMIDIVISION_GRAPHIC);

      /***** Write again the labels of the X axis *****/
      Sta_WriteLabelsXAxisAccMin (IncX,Format);
     }
  }

/*****************************************************************************/
/****** Write labels of the X axis in the graphic of clicks per minute *******/
/*****************************************************************************/

#define Sta_WIDTH_DIVISION_GRAPHIC	(Sta_WIDTH_SEMIDIVISION_GRAPHIC * 2)	// 60

static void Sta_WriteLabelsXAxisAccMin (float IncX,const char *Format)
  {
   unsigned i;
   float NumX;

   fprintf (Gbl.F.Out,"<tr>");
   for (i = 0, NumX = 0;
	i <= Sta_NUM_DIVISIONS_X;
	i++, NumX += IncX)
     {
      fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"LOG CENTER_BOTTOM\""
	                 " style=\"width:%upx;\">",
               Sta_WIDTH_DIVISION_GRAPHIC);
      fprintf (Gbl.F.Out,Format,NumX);
      fprintf (Gbl.F.Out,"</td>");
     }
   fprintf (Gbl.F.Out,"</tr>");
  }

/*****************************************************************************/
/***** Write a row of the graphic with number of clicks in every minute ******/
/*****************************************************************************/

#define Sta_WIDTH_GRAPHIC	(Sta_WIDTH_DIVISION_GRAPHIC * Sta_NUM_DIVISIONS_X)	// 60 * 10 = 600

static void Sta_WriteAccessMinute (unsigned Minute,float HitsNum,float MaxX)
  {
   unsigned BarWidth;

   /***** Start row *****/
   fprintf (Gbl.F.Out,"<tr>");

   /***** Labels of the Y axis, and Y axis *****/
   if (!Minute)
      // If minute 0
      fprintf (Gbl.F.Out,"<td rowspan=\"30\" class=\"LOG LEFT_TOP\""
	                 " style=\"width:%upx;"
	                 " background-image:url('%s/ejey24x30.gif');"
	                 " background-size:30px 30px;"
	                 " background-repeat:repeat;\">"
	                 "00h"
	                 "</td>",
               Sta_WIDTH_SEMIDIVISION_GRAPHIC,Gbl.Prefs.IconsURL);
   else if (Minute == (Sta_NUM_MINUTES_PER_DAY - 30))
      // If 23:30
      fprintf (Gbl.F.Out,"<td rowspan=\"30\" class=\"LOG LEFT_BOTTOM\""
	                 " style=\"width:%upx;"
	                 " background-image:url('%s/ejey24x30.gif');"
	                 " background-size:30px 30px;"
	                 " background-repeat:repeat;\">"
	                 "24h"
	                 "</td>",
               Sta_WIDTH_SEMIDIVISION_GRAPHIC,Gbl.Prefs.IconsURL);
   else if (!(Minute % 30) && (Minute % 60))
      // If minute is multiple of 30 but not of 60 (i.e.: 30, 90, 150...)
      fprintf (Gbl.F.Out,"<td rowspan=\"60\" class=\"LOG LEFT_MIDDLE\""
	                 " style=\"width:%upx;"
	                 " background-image:url('%s/ejey24x60.gif');"
	                 " background-size:30px 60px;"
	                 " background-repeat:repeat;\">"
	                 "%02uh"
	                 "</td>",
               Sta_WIDTH_SEMIDIVISION_GRAPHIC,Gbl.Prefs.IconsURL,(Minute + 30) / 60);

   /***** Start of cell for the graphic *****/
   fprintf (Gbl.F.Out,"<td colspan=\"%u\" class=\"LEFT_BOTTOM\""
	              " style=\"width:%upx; height:1px;"
	              " background-image:url('%s/malla%c48x1.gif');"
	              " background-size:60px 1px;"
	              " background-repeat:repeat;\">",
	    Sta_NUM_DIVISIONS_X * 2,Sta_WIDTH_GRAPHIC,Gbl.Prefs.IconsURL,
	    (Minute % 60) == 0 ? 'v' :
		                 'h');

   /***** Draw bar with anchura proporcional al number of clicks *****/
   if (HitsNum != 0.0)
      if ((BarWidth = (unsigned) (((HitsNum * (float) Sta_WIDTH_GRAPHIC / MaxX)) + 0.5)) != 0)
	 fprintf (Gbl.F.Out,"<img src=\"%s/b%c1x1.gif\""
	                    " alt=\"\" title=\"\""
	                    " style=\"display:block;"
	                    " width:%upx; height:1px;\" />",
                  Gbl.Prefs.IconsURL,
                  (Minute % 60) == 0 ? 'g' :
                	               'b',
                  BarWidth);

   /***** End of cell of graphic and end of row *****/
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
  }

/*****************************************************************************/
/**** Show a listing of accesses with the number of clicks a each action *****/
/*****************************************************************************/

static void Sta_ShowNumHitsPerAction (unsigned long NumRows,
                                      MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Action;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned long NumRow;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   long ActCod;
   char ActTxt[Act_MAX_BYTES_ACTION_TXT + 1];

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"RIGHT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Action,
            Txt_STAT_TYPE_COUNT_CAPS[Gbl.Stat.CountType]);

   /***** Compute maximum number of pages generated per day *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumRow = 1;
	NumRow <= NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Write the action */
      ActCod = Str_ConvertStrCodToLongCod (row[0]);

      if (ActCod >= 0)
         fprintf (Gbl.F.Out,"<tr>"
                            "<td class=\"LOG RIGHT_TOP\">"
                            "%s&nbsp;"
                            "</td>",
                  Act_GetActionTextFromDB (ActCod,ActTxt));
      else
         fprintf (Gbl.F.Out,"<tr>"
                            "<td class=\"LOG RIGHT_TOP\">"
                            "?&nbsp;"
                            "</td>");

      /* Draw bar proportional to number of pages generated */
      Hits.Num = Str_GetFloatNumFromStr (row[1]);
      Sta_DrawBarNumHits ('c',Hits.Num,Hits.Max,Hits.Total,500);
     }
  }

/*****************************************************************************/
/*************** Show number of clicks distributed by plugin *****************/
/*****************************************************************************/

static void Sta_ShowNumHitsPerPlugin (unsigned long NumRows,
                                      MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Plugin;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned long NumRow;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   struct Plugin Plg;

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"RIGHT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Plugin,
            Txt_STAT_TYPE_COUNT_CAPS[Gbl.Stat.CountType]);

   /***** Compute maximum number of pages generated per plugin *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumRow = 1;
	NumRow <= NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Write the plugin */
      if (sscanf (row[0],"%ld",&Plg.PlgCod) != 1)
	 Lay_ShowErrorAndExit ("Wrong plugin code.");
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LOG RIGHT_TOP\">");
      if (Plg_GetDataOfPluginByCod (&Plg))
         fprintf (Gbl.F.Out,"%s",Plg.Name);
      else
         fprintf (Gbl.F.Out,"?");
      fprintf (Gbl.F.Out,"&nbsp;</td>");

      /* Draw bar proportional to number of pages generated */
      Hits.Num = Str_GetFloatNumFromStr (row[1]);
      Sta_DrawBarNumHits ('c',Hits.Num,Hits.Max,Hits.Total,500);
     }
  }

/*****************************************************************************/
/******** Show number of clicks distributed by web service function **********/
/*****************************************************************************/

static void Sta_ShowNumHitsPerWSFunction (unsigned long NumRows,
                                          MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Function;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned long NumRow;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   long FunCod;

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Function,
            Txt_STAT_TYPE_COUNT_CAPS[Gbl.Stat.CountType]);

   /***** Compute maximum number of pages generated per function *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumRow = 1;
	NumRow <= NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Write the plugin */
      if (sscanf (row[0],"%ld",&FunCod) != 1)
	 Lay_ShowErrorAndExit ("Wrong function code.");
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LOG LEFT_TOP\">"
	                 "%s&nbsp;"
	                 "</td>",
               Svc_GetFunctionNameFromFunCod (FunCod));

      /* Draw bar proportional to number of pages generated */
      Hits.Num = Str_GetFloatNumFromStr (row[1]);
      Sta_DrawBarNumHits ('c',Hits.Num,Hits.Max,Hits.Total,500);
     }
  }

/*****************************************************************************/
/******** Show number of clicks distributed by web service function **********/
/*****************************************************************************/

static void Sta_ShowNumHitsPerBanner (unsigned long NumRows,
                                      MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Banner;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned long NumRow;
   float NumClicks;
   float MaxClicks = 0.0;
   float TotalClicks = 0.0;
   MYSQL_ROW row;
   struct Banner Ban;

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Banner,
            Txt_STAT_TYPE_COUNT_CAPS[Gbl.Stat.CountType]);

   /***** Compute maximum number of clicks per banner *****/
   for (NumRow = 1;
	NumRow <= NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get number of pages generated */
      NumClicks = Str_GetFloatNumFromStr (row[1]);
      if (NumRow == 1)
	 MaxClicks = NumClicks;
      TotalClicks += NumClicks;
     }

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumRow = 1;
	NumRow <= NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Write the banner */
      if (sscanf (row[0],"%ld",&(Ban.BanCod)) != 1)
	 Lay_ShowErrorAndExit ("Wrong banner code.");
      Ban_GetDataOfBannerByCod (&Ban);
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"LOG LEFT_TOP\">"
                         "<a href=\"%s\" title=\"%s\" class=\"DAT\" target=\"_blank\">"
                         "<img src=\"%s/%s/%s\""
                         " alt=\"%s\" title=\"%s\""
                         " class=\"BANNER_SMALL\""
                         " style=\"margin:0 10px 5px 0;\" />"
                         "</a>",
               Ban.WWW,
               Ban.FullName,
               Cfg_URL_SWAD_PUBLIC,Cfg_FOLDER_BANNER,
               Ban.Img,
               Ban.ShrtName,
               Ban.FullName);

      /* Draw bar proportional to number of clicks */
      NumClicks = Str_GetFloatNumFromStr (row[1]);
      Sta_DrawBarNumHits ('c',NumClicks,MaxClicks,TotalClicks,500);
     }
  }

/*****************************************************************************/
/******* Show a listing with the number of hits distributed by country *******/
/*****************************************************************************/

static void Sta_ShowNumHitsPerCountry (unsigned long NumRows,
                                       MYSQL_RES *mysql_res)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Country;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned long NumRow;
   unsigned long Ranking;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   long CtyCod;

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_No_INDEX,
            Txt_Country,
            Txt_STAT_TYPE_COUNT_CAPS[Gbl.Stat.CountType]);

   /***** Compute maximum number of hits per country *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumRow = 1, Ranking = 0;
	NumRow <= NumRows;
	NumRow++)
     {
      /* Get country code */
      row = mysql_fetch_row (mysql_res);
      CtyCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Write ranking of this country */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LOG RIGHT_MIDDLE\">");
      if (CtyCod > 0)
         fprintf (Gbl.F.Out,"%lu",++Ranking);
      fprintf (Gbl.F.Out,"&nbsp;"
	                 "</td>");

      /* Write country */
      Sta_WriteCountry (CtyCod);

      /* Draw bar proportional to number of hits */
      Hits.Num = Str_GetFloatNumFromStr (row[1]);
      Sta_DrawBarNumHits ('c',Hits.Num,Hits.Max,Hits.Total,375);
     }
  }

/*****************************************************************************/
/************************ Write country with an icon *************************/
/*****************************************************************************/

static void Sta_WriteCountry (long CtyCod)
  {
   struct Country Cty;

   /***** Start cell *****/
   fprintf (Gbl.F.Out,"<td class=\"LOG LEFT_MIDDLE\">");

   if (CtyCod > 0)	// Hit with a country selected
     {
      /***** Get data of country *****/
      Cty.CtyCod = CtyCod;
      Cty_GetDataOfCountryByCod (&Cty,Cty_GET_BASIC_DATA);

      /***** Form to go to country *****/
      Cty_DrawCountryMapAndNameWithLink (&Cty,ActSeeCtyInf,
                                         "COUNTRY_TINY",
                                         "COUNTRY_MAP_TINY",
                                         "LOG");
     }
   else			// Hit with no country selected
      /***** No country selected *****/
      fprintf (Gbl.F.Out,"&nbsp;-&nbsp;");

   /***** End cell *****/
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/***** Show a listing with the number of hits distributed by institution *****/
/*****************************************************************************/

static void Sta_ShowNumHitsPerInstitution (unsigned long NumRows,
                                           MYSQL_RES *mysql_res)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Institution;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned long NumRow;
   unsigned long Ranking;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   long InsCod;

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_No_INDEX,
            Txt_Institution,
            Txt_STAT_TYPE_COUNT_CAPS[Gbl.Stat.CountType]);

   /***** Compute maximum number of hits per institution *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumRow = 1, Ranking = 0;
	NumRow <= NumRows;
	NumRow++)
     {
      /* Get institution code */
      row = mysql_fetch_row (mysql_res);
      InsCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Write ranking of this institution */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LOG RIGHT_TOP\">");
      if (InsCod > 0)
         fprintf (Gbl.F.Out,"%lu",++Ranking);
      fprintf (Gbl.F.Out,"&nbsp;"
	                 "</td>");

      /* Write institution */
      Sta_WriteInstitution (InsCod);

      /* Draw bar proportional to number of hits */
      Hits.Num = Str_GetFloatNumFromStr (row[1]);
      Sta_DrawBarNumHits ('c',Hits.Num,Hits.Max,Hits.Total,375);
     }
  }

/*****************************************************************************/
/********************** Write institution with an icon ***********************/
/*****************************************************************************/

static void Sta_WriteInstitution (long InsCod)
  {
   struct Instit Ins;

   /***** Start cell *****/
   fprintf (Gbl.F.Out,"<td class=\"LOG LEFT_MIDDLE\"");

   if (InsCod > 0)	// Hit with an institution selected
     {
      /***** Get data of institution *****/
      Ins.InsCod = InsCod;
      Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);

      /***** Title in cell *****/
      fprintf (Gbl.F.Out,"title=\"%s\">",
               Ins.FullName);

      /***** Form to go to institution *****/
      Ins_DrawInstitutionLogoAndNameWithLink (&Ins,ActSeeInsInf,
                                              "LOG","CENTER_TOP");
     }
   else			// Hit with no institution selected
      /***** No institution selected *****/
      fprintf (Gbl.F.Out,">&nbsp;-&nbsp;");

   /***** End cell *****/
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/******* Show a listing with the number of hits distributed by centre ********/
/*****************************************************************************/

static void Sta_ShowNumHitsPerCentre (unsigned long NumRows,
                                      MYSQL_RES *mysql_res)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Centre;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned long NumRow;
   unsigned long Ranking;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   long CtrCod;

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_No_INDEX,
            Txt_Centre,
            Txt_STAT_TYPE_COUNT_CAPS[Gbl.Stat.CountType]);

   /***** Compute maximum number of hits per centre *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumRow = 1, Ranking = 0;
	NumRow <= NumRows;
	NumRow++)
     {
      /* Get centre code */
      row = mysql_fetch_row (mysql_res);
      CtrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Write ranking of this centre */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LOG RIGHT_TOP\">");
      if (CtrCod > 0)
         fprintf (Gbl.F.Out,"%lu",++Ranking);
      fprintf (Gbl.F.Out,"&nbsp;"
	                 "</td>");

      /* Write centre */
      Sta_WriteCentre (CtrCod);

      /* Draw bar proportional to number of hits */
      Hits.Num = Str_GetFloatNumFromStr (row[1]);
      Sta_DrawBarNumHits ('c',Hits.Num,Hits.Max,Hits.Total,375);
     }
  }

/*****************************************************************************/
/************************* Write centre with an icon *************************/
/*****************************************************************************/

static void Sta_WriteCentre (long CtrCod)
  {
   struct Centre Ctr;

   /***** Start cell *****/
   fprintf (Gbl.F.Out,"<td class=\"LOG LEFT_MIDDLE\"");

   if (CtrCod > 0)	// Hit with a centre selected
     {
      /***** Get data of centre *****/
      Ctr.CtrCod = CtrCod;
      Ctr_GetDataOfCentreByCod (&Ctr);

      /***** Title in cell *****/
      fprintf (Gbl.F.Out,"title=\"%s\">",
               Ctr.FullName);

      /***** Form to go to centre *****/
      Ctr_DrawCentreLogoAndNameWithLink (&Ctr,ActSeeCtrInf,
                                         "LOG","CENTER_TOP");
     }
   else			// Hit with no centre selected
      /***** No centre selected *****/
      fprintf (Gbl.F.Out,">&nbsp;-&nbsp;");

   /***** End cell *****/
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/******* Show a listing with the number of hits distributed by degree ********/
/*****************************************************************************/

static void Sta_ShowNumHitsPerDegree (unsigned long NumRows,
                                      MYSQL_RES *mysql_res)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Degree;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned long NumRow;
   unsigned long Ranking;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   long DegCod;

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_No_INDEX,
            Txt_Degree,
            Txt_STAT_TYPE_COUNT_CAPS[Gbl.Stat.CountType]);

   /***** Compute maximum number of hits per degree *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumRow = 1, Ranking = 0;
	NumRow <= NumRows;
	NumRow++)
     {
      /* Get degree code */
      row = mysql_fetch_row (mysql_res);
      DegCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Write ranking of this degree */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LOG RIGHT_TOP\">");
      if (DegCod > 0)
         fprintf (Gbl.F.Out,"%lu",++Ranking);
      fprintf (Gbl.F.Out,"&nbsp;"
	                 "</td>");

      /* Write degree */
      Sta_WriteDegree (DegCod);

      /* Draw bar proportional to number of hits */
      Hits.Num = Str_GetFloatNumFromStr (row[1]);
      Sta_DrawBarNumHits ('c',Hits.Num,Hits.Max,Hits.Total,375);
     }
  }

/*****************************************************************************/
/************************* Write degree with an icon *************************/
/*****************************************************************************/

static void Sta_WriteDegree (long DegCod)
  {
   struct Degree Deg;

   /***** Start cell *****/
   fprintf (Gbl.F.Out,"<td class=\"LOG LEFT_MIDDLE\"");

   if (DegCod > 0)	// Hit with a degree selected
     {
      /***** Get data of degree *****/
      Deg.DegCod = DegCod;
      Deg_GetDataOfDegreeByCod (&Deg);

      /***** Title in cell *****/
      fprintf (Gbl.F.Out,"title=\"%s\">",
               Deg.FullName);

      /***** Form to go to degree *****/
      Deg_DrawDegreeLogoAndNameWithLink (&Deg,ActSeeDegInf,
                                         "LOG","CENTER_TOP");
     }
   else			// Hit with no degree selected
      /***** No degree selected *****/
      fprintf (Gbl.F.Out,">&nbsp;-&nbsp;");

   /***** End cell *****/
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/********* Show a listing with the number of clicks to each course ***********/
/*****************************************************************************/

static void Sta_ShowNumHitsPerCourse (unsigned long NumRows,
                                      MYSQL_RES *mysql_res)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Degree;
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_Course;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   extern const char *Txt_Go_to_X;
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];	// Declaration in swad_degree.c
   unsigned long NumRow;
   unsigned long Ranking;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   bool CrsOK;
   struct Course Crs;

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_No_INDEX,
            Txt_Degree,
            Txt_Year_OF_A_DEGREE,
            Txt_Course,
            Txt_STAT_TYPE_COUNT_CAPS[Gbl.Stat.CountType]);

   /***** Compute maximum number of pages generated per course *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumRows,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumRow = 1, Ranking = 0;
	NumRow <= NumRows;
	NumRow++)
     {
      /* Get degree, the year and the course */
      row = mysql_fetch_row (mysql_res);

      /* Get course code */
      Crs.CrsCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get data of current degree */
      CrsOK = Crs_GetDataOfCourseByCod (&Crs);

      /* Write ranking of this course */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LOG RIGHT_TOP\">");
      if (CrsOK)
         fprintf (Gbl.F.Out,"%lu",++Ranking);
      fprintf (Gbl.F.Out,"&nbsp;</td>");

      /* Write degree */
      Sta_WriteDegree (Crs.DegCod);

      /* Write degree year */
      fprintf (Gbl.F.Out,"<td class=\"LOG CENTER_TOP\">"
	                 "%s&nbsp;"
	                 "</td>",
               CrsOK ? Txt_YEAR_OF_DEGREE[Crs.Year] :
        	       "-");

      /* Write course, including link */
      fprintf (Gbl.F.Out,"<td class=\"LOG LEFT_TOP\">");
      if (CrsOK)
        {
         Act_FormGoToStart (ActSeeCrsInf);
         Crs_PutParamCrsCod (Crs.CrsCod);
         sprintf (Gbl.Title,Txt_Go_to_X,Crs.FullName);
         Act_LinkFormSubmit (Gbl.Title,"LOG",NULL);
         fprintf (Gbl.F.Out,"%s"
                            "</a>",
                  Crs.ShrtName);
        }
      else
         fprintf (Gbl.F.Out,"-");
      fprintf (Gbl.F.Out,"&nbsp;");
      if (CrsOK)
         Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Draw bar proportional to number of pages generated */
      Hits.Num = Str_GetFloatNumFromStr (row[1]);
      Sta_DrawBarNumHits ('c',Hits.Num,Hits.Max,Hits.Total,375);
     }
  }

/*****************************************************************************/
/*************** Compute maximum and total number of hits ********************/
/*****************************************************************************/

void Sta_ComputeMaxAndTotalHits (struct Sta_Hits *Hits,
                                 unsigned long NumRows,
                                 MYSQL_RES *mysql_res,unsigned Field,
                                 unsigned Divisor)
  {
   unsigned long NumRow;
   MYSQL_ROW row;

   /***** For each row... *****/
   for (NumRow = 1, Hits->Max = Hits->Total = 0.0;
	NumRow <= NumRows;
	NumRow++)
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get number of hits */
      Hits->Num = Str_GetFloatNumFromStr (row[Field]);
      if (Divisor > 1)
         Hits->Num /= (float) Divisor;

      /* Update total hits */
      Hits->Total += Hits->Num;

      /* Update maximum hits */
      if (Hits->Num > Hits->Max)
	 Hits->Max = Hits->Num;
     }
  }

/*****************************************************************************/
/********************* Draw a bar with the number of hits ********************/
/*****************************************************************************/

static void Sta_DrawBarNumHits (char Color,float HitsNum,float HitsMax,float HitsTotal,unsigned MaxBarWidth)
  {
   unsigned BarWidth;

   fprintf (Gbl.F.Out,"<td class=\"LOG LEFT_MIDDLE\">");

   if (HitsNum != 0.0)
     {
      /***** Draw bar with a with proportional to the number of hits *****/
      BarWidth = (unsigned) (((HitsNum * (float) MaxBarWidth) / HitsMax) + 0.5);
      if (BarWidth == 0)
         BarWidth = 1;
      fprintf (Gbl.F.Out,"<img src=\"%s/%c1x14.gif\""
	                 " alt=\"\" title=\"\""
                         " class=\"LEFT_TOP\""
	                 " style=\"width:%upx; height:18px;\" />"
                         "&nbsp;",
	       Gbl.Prefs.IconsURL,Color,BarWidth);

      /***** Write the number of hits *****/
      Str_WriteFloatNum (Gbl.F.Out,HitsNum);
      fprintf (Gbl.F.Out,"&nbsp;(%u",
               (unsigned) (((HitsNum * 100.0) /
        	            HitsTotal) + 0.5));
     }
   else
      /***** Write the number of clicks *****/
      fprintf (Gbl.F.Out,"0&nbsp;(0");

   fprintf (Gbl.F.Out,"%%)&nbsp;"
	              "</td>"
	              "</tr>");
  }

/*****************************************************************************/
/**** Write parameters of initial and final dates in the query of clicks *****/
/*****************************************************************************/

void Sta_WriteParamsDatesSeeAccesses (void)
  {
   Par_PutHiddenParamUnsigned ("StartTimeUTC",Gbl.DateRange.TimeUTC[0]);
   Par_PutHiddenParamUnsigned ("EndTimeUTC"  ,Gbl.DateRange.TimeUTC[1]);
  }

/*****************************************************************************/
/************************** Show use of the platform *************************/
/*****************************************************************************/

void Sta_ReqShowFigures (void)
  {
   extern const char *Hlp_STATS_Figures;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Figures;
   extern const char *Txt_Scope;
   extern const char *Txt_Statistic;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Show_statistic;
   Sta_FigureType_t FigureType;

   /***** Form to show statistic *****/
   Act_FormStart (ActSeeUseGbl);

   /***** Start frame *****/
   Lay_StartRoundFrame (NULL,Txt_Figures,NULL,Hlp_STATS_Figures);

   /***** Compute stats for anywhere, degree or course? *****/
   fprintf (Gbl.F.Out,"<label class=\"%s\">%s:&nbsp;",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Scope);
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	               1 << Sco_SCOPE_CTY |
	               1 << Sco_SCOPE_INS |
		       1 << Sco_SCOPE_CTR |
		       1 << Sco_SCOPE_DEG |
		       1 << Sco_SCOPE_CRS;
   Gbl.Scope.Default = Sco_SCOPE_SYS;
   Sco_GetScope ("ScopeSta");
   Sco_PutSelectorScope ("ScopeSta",false);
   fprintf (Gbl.F.Out,"</label><br />");

   /***** Type of statistic *****/
   fprintf (Gbl.F.Out,"<label class=\"%s\">%s:&nbsp;"
	              "<select name=\"FigureType\">",
	    The_ClassForm[Gbl.Prefs.Theme],Txt_Statistic);
   for (FigureType = (Sta_FigureType_t) 0;
	FigureType < Sta_NUM_FIGURES;
	FigureType++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",
               (unsigned) FigureType);
      if (FigureType == Gbl.Stat.FigureType)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out," />"
	                 "%s"
	                 "</option>",
               Txt_STAT_USE_STAT_TYPES[FigureType]);
     }
   fprintf (Gbl.F.Out,"</select>"
	              "</label>");

   /***** Send button and end frame *****/
   Lay_EndRoundFrameWithButton (Lay_CONFIRM_BUTTON,Txt_Show_statistic);

   /***** End of form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/************************* Put icon to show a figure *************************/
/*****************************************************************************/
// Gbl.Stat.FigureType must be set to the desired figure before calling this function

void Sta_PutIconToShowFigure (void)
  {
   extern const char *Txt_Show_statistic;

   Lay_PutContextualLink (ActSeeUseGbl,NULL,Sta_PutParamsToShowFigure,
                          "pie64x64.gif",
                          Txt_Show_statistic,NULL,
                          NULL);
  }

/*****************************************************************************/
/************* Put hidden parameters for figures (statistics) ****************/
/*****************************************************************************/
// Gbl.Stat.FigureType must be set to the desired figure before calling this function

static void Sta_PutParamsToShowFigure (void)
  {
   /***** Set default scope (used only if Gbl.Scope.Current is unknown) *****/
   Gbl.Scope.Default = Sco_SCOPE_CRS;
   Sco_AdjustScope ();

   Sta_PutHiddenParamFigures ();
  }

/*****************************************************************************/
/************* Put hidden parameters for figures (statistics) ****************/
/*****************************************************************************/

void Sta_PutHiddenParamFigures (void)
  {
   Sta_PutHiddenParamScopeSta ();
   Sta_PutHiddenParamFigureType ();
  }

/*****************************************************************************/
/********* Put hidden parameter for the type of figure (statistic) ***********/
/*****************************************************************************/

static void Sta_PutHiddenParamFigureType (void)
  {
   Par_PutHiddenParamUnsigned ("FigureType",(unsigned) Gbl.Stat.FigureType);
  }

/*****************************************************************************/
/********* Put hidden parameter for the type of figure (statistic) ***********/
/*****************************************************************************/

static void Sta_PutHiddenParamScopeSta (void)
  {
   Sco_PutParamScope ("ScopeSta",Gbl.Scope.Current);
  }

/*****************************************************************************/
/************************** Show use of the platform *************************/
/*****************************************************************************/

void Sta_ShowFigures (void)
  {
   static void (*Sta_Function[Sta_NUM_FIGURES])(void) =	// Array of pointers to functions
     {
      Sta_GetAndShowUsersStats,			// Sta_USERS
      Sta_GetAndShowUsersRanking,		// Sta_USERS_RANKING
      Sta_GetAndShowHierarchyStats,		// Sta_HIERARCHY
      Sta_GetAndShowInstitutionsStats,		// Sta_INSTITS
      Sta_GetAndShowDegreeTypesStats,		// Sta_DEGREE_TYPES
      Sta_GetAndShowFileBrowsersStats,		// Sta_FOLDERS_AND_FILES
      Sta_GetAndShowOERsStats,			// Sta_OER
      Sta_GetAndShowAssignmentsStats,		// Sta_ASSIGNMENTS
      Sta_GetAndShowTestsStats,			// Sta_TESTS
      Sta_GetAndShowSocialActivityStats,	// Sta_SOCIAL_ACTIVITY
      Sta_GetAndShowFollowStats,		// Sta_FOLLOW
      Sta_GetAndShowForumStats,			// Sta_FORUMS
      Sta_GetAndShowNumUsrsPerNotifyEvent,	// Sta_NOTIFY_EVENTS
      Sta_GetAndShowNoticesStats,		// Sta_NOTICES
      Sta_GetAndShowMsgsStats,			// Sta_MESSAGES
      Sta_GetAndShowSurveysStats,		// Sta_SURVEYS
      Net_ShowWebAndSocialNetworksStats,	// Sta_SOCIAL_NETWORKS
      Sta_GetAndShowNumUsrsPerLanguage,		// Sta_LANGUAGES
      Sta_GetAndShowNumUsrsPerFirstDayOfWeek,	// Sta_FIRST_DAY_OF_WEEK
      Sta_GetAndShowNumUsrsPerDateFormat,	// Sta_DATE_FORMAT
      Sta_GetAndShowNumUsrsPerIconSet,		// Sta_ICON_SETS
      Sta_GetAndShowNumUsrsPerMenu,		// Sta_MENUS
      Sta_GetAndShowNumUsrsPerTheme,		// Sta_THEMES
      Sta_GetAndShowNumUsrsPerSideColumns,	// Sta_SIDE_COLUMNS
      Sta_GetAndShowNumUsrsPerPrivacy,		// Sta_PRIVACY
     };

   /***** Get the type of figure ******/
   Gbl.Stat.FigureType = (Sta_FigureType_t)
	                 Par_GetParToUnsignedLong ("FigureType",
	                                           0,
	                                           Sta_NUM_FIGURES - 1,
	                                           (unsigned long) Sta_FIGURE_TYPE_DEF);

   /***** Show again the form to see use of the platform *****/
   Sta_ReqShowFigures ();

   /***** Show the stat of use selected by user *****/
   Sta_Function[Gbl.Stat.FigureType] ();
  }

/*****************************************************************************/
/********************** Show stats about number of users *********************/
/*****************************************************************************/

static void Sta_GetAndShowUsersStats (void)
  {
   extern const char *Hlp_STATS_Figures_users;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Users;
   extern const char *Txt_No_of_users;
   extern const char *Txt_Average_number_of_courses_to_which_a_user_belongs;
   extern const char *Txt_Average_number_of_users_belonging_to_a_course;

   /***** Number of users *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_USERS],
                             NULL,Hlp_STATS_Figures_users,2);

   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Users,
            Txt_No_of_users,
            Txt_Average_number_of_courses_to_which_a_user_belongs,
            Txt_Average_number_of_users_belonging_to_a_course);
   Sta_GetAndShowNumUsrsInCrss (Rol_STD);		// Students
   Sta_GetAndShowNumUsrsInCrss (Rol_NET);		// Non-editing teachers
   Sta_GetAndShowNumUsrsInCrss (Rol_TCH);		// Teachers
   Sta_GetAndShowNumUsrsInCrss (Rol_UNK);		// Any user in courses
   fprintf (Gbl.F.Out,"<tr>"
                      "<th colspan=\"4\" style=\"height:10px;\">"
                      "</tr>");
   Sta_GetAndShowNumUsrsNotBelongingToAnyCrs ();	// Users not beloging to any course

   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/**************** Get and show number of users in courses ********************/
/*****************************************************************************/
// Rol_UNK means any role in courses

static void Sta_GetAndShowNumUsrsInCrss (Rol_Role_t Role)
  {
   extern const char *Txt_Total;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   unsigned NumUsrs;
   float NumCrssPerUsr;
   float NumUsrsPerCrs;
   char *Class = (Role == Rol_UNK) ? "DAT_N_LINE_TOP RIGHT_BOTTOM" :
	                             "DAT RIGHT_BOTTOM";
   unsigned Roles = (Role == Rol_UNK) ? ((1 << Rol_STD) |
	                                 (1 << Rol_NET) |
	                                 (1 << Rol_TCH)) :
	                                (1 << Role);

   /***** Get the number of users belonging to any course *****/
   NumUsrs = Usr_GetTotalNumberOfUsersInCourses (Gbl.Scope.Current,
						 Roles);

   /***** Get average number of courses per user *****/
   NumCrssPerUsr = Usr_GetNumCrssPerUsr (Role);

   /***** Query the number of users per course *****/
   NumUsrsPerCrs = Usr_GetNumUsrsPerCrs (Role);

   /***** Write the total number of users *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s\">"
                      "%s"
                      "</td>"
                      "<td class=\"%s\">"
                      "%u"
                      "</td>"
                      "<td class=\"%s\">"
                      "%.2f"
                      "</td>"
                      "<td class=\"%s\">"
                      "%.2f"
                      "</td>"
                      "</tr>",
            Class,(Role == Rol_UNK) ? Txt_Total :
        	                      Txt_ROLES_PLURAL_Abc[Role][Usr_SEX_UNKNOWN],
            Class,NumUsrs,
            Class,NumCrssPerUsr,
            Class,NumUsrsPerCrs);
  }

/*****************************************************************************/
/**************** Get and show number of users in courses ********************/
/*****************************************************************************/

static void Sta_GetAndShowNumUsrsNotBelongingToAnyCrs (void)
  {
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char *Class = "DAT RIGHT_BOTTOM";

   /***** Write the total number of users not belonging to any course *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s\">"
                      "%s"
                      "</td>"
                      "<td class=\"%s\">"
                      "%u"
                      "</td>"
                      "<td class=\"%s\">"
                      "%.2f"
                      "</td>"
                      "<td class=\"%s\">"
                      "%.2f"
                      "</td>"
                      "</tr>",
            Class,Txt_ROLES_PLURAL_Abc[Rol_GST][Usr_SEX_UNKNOWN],
            Class,Usr_GetNumUsrsNotBelongingToAnyCrs (),
            Class,0.0,
            Class,0.0);
  }

/*****************************************************************************/
/****************************** Show users' ranking **************************/
/*****************************************************************************/

static void Sta_GetAndShowUsersRanking (void)
  {
   extern const char *Hlp_STATS_Figures_ranking;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Clicks;
   extern const char *Txt_Clicks_per_day;
   extern const char *Txt_Downloads;
   extern const char *Txt_Forums;
   extern const char *Txt_Messages;
   extern const char *Txt_Followers;

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_USERS_RANKING],
                             NULL,Hlp_STATS_Figures_ranking,2);

   /***** Header *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Clicks,
            Txt_Clicks_per_day,
            Txt_Downloads,
            Txt_Forums,
            Txt_Messages,
            Txt_Followers);

   /***** Rankings *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"DAT LEFT_TOP\">");
   Prf_GetAndShowRankingClicks ();
   fprintf (Gbl.F.Out,"</td>"
                      "<td class=\"DAT LEFT_TOP\">");
   Prf_GetAndShowRankingClicksPerDay ();
   fprintf (Gbl.F.Out,"</td>"
                      "<td class=\"DAT LEFT_TOP\">");
   Prf_GetAndShowRankingFileViews ();
   fprintf (Gbl.F.Out,"</td>"
                      "<td class=\"DAT LEFT_TOP\">");
   Prf_GetAndShowRankingForPst ();
   fprintf (Gbl.F.Out,"</td>"
                      "<td class=\"DAT LEFT_TOP\">");
   Prf_GetAndShowRankingMsgSnt ();
   fprintf (Gbl.F.Out,"</td>"
                      "<td class=\"DAT LEFT_TOP\">");
   Fol_GetAndShowRankingFollowers ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/*********            Get and show stats about hierarchy           ***********/
/********* (countries, institutions, centres, degrees and courses) ***********/
/*****************************************************************************/

static void Sta_GetAndShowHierarchyStats (void)
  {
   extern const char *Hlp_STATS_Figures_hierarchy;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];

   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_HIERARCHY],
                             NULL,Hlp_STATS_Figures_hierarchy,2);
   Sta_WriteHeadDegsCrssInSWAD ();
   Sta_GetAndShowNumCtysInSWAD ();
   Sta_GetAndShowNumInssInSWAD ();
   Sta_GetAndShowNumCtrsInSWAD ();
   Sta_GetAndShowNumDegsInSWAD ();
   Sta_GetAndShowNumCrssInSWAD ();
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/******************* Write head table degrees and courses ********************/
/*****************************************************************************/

static void Sta_WriteHeadDegsCrssInSWAD (void)
  {
   extern const char *Txt_Total;
   extern const char *Txt_With_institutions;
   extern const char *Txt_With_centres;
   extern const char *Txt_With_degrees;
   extern const char *Txt_With_courses;
   extern const char *Txt_With_teachers;
   extern const char *Txt_With_non_editing_teachers;
   extern const char *Txt_With_students;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th></th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Total,
            Txt_With_institutions,
            Txt_With_centres,
            Txt_With_degrees,
            Txt_With_courses,
            Txt_With_teachers,
            Txt_With_non_editing_teachers,
            Txt_With_students);
  }

/*****************************************************************************/
/******************* Get and show total number of countries ******************/
/*****************************************************************************/

static void Sta_GetAndShowNumCtysInSWAD (void)
  {
   extern const char *Txt_Countries;
   char SubQuery[128];
   unsigned NumCtysTotal = 0;
   unsigned NumCtysWithInss = 0;
   unsigned NumCtysWithCtrs = 0;
   unsigned NumCtysWithDegs = 0;
   unsigned NumCtysWithCrss = 0;
   unsigned NumCtysWithTchs = 0;
   unsigned NumCtysWithNETs = 0;
   unsigned NumCtysWithStds = 0;

   /***** Get number of countries *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 NumCtysTotal = Cty_GetNumCtysTotal ();
         NumCtysWithInss = Cty_GetNumCtysWithInss ("");
	 NumCtysWithCtrs = Cty_GetNumCtysWithCtrs ("");
	 NumCtysWithDegs = Cty_GetNumCtysWithDegs ("");
	 NumCtysWithCrss = Cty_GetNumCtysWithCrss ("");
         NumCtysWithTchs = Cty_GetNumCtysWithUsrs (Rol_TCH,"");
         NumCtysWithNETs = Cty_GetNumCtysWithUsrs (Rol_NET,"");
	 NumCtysWithStds = Cty_GetNumCtysWithUsrs (Rol_STD,"");
         SubQuery[0] = '\0';
         break;
      case Sco_SCOPE_CTY:
	 NumCtysTotal = 1;
	 NumCtysWithInss = 1;
         sprintf (SubQuery,"institutions.CtyCod=%ld AND ",
                  Gbl.CurrentCty.Cty.CtyCod);
	 NumCtysWithCtrs = Cty_GetNumCtysWithCtrs (SubQuery);
	 NumCtysWithDegs = Cty_GetNumCtysWithDegs (SubQuery);
	 NumCtysWithCrss = Cty_GetNumCtysWithCrss (SubQuery);
         NumCtysWithTchs = Cty_GetNumCtysWithUsrs (Rol_TCH,SubQuery);
         NumCtysWithNETs = Cty_GetNumCtysWithUsrs (Rol_NET,SubQuery);
	 NumCtysWithStds = Cty_GetNumCtysWithUsrs (Rol_STD,SubQuery);
         break;
      case Sco_SCOPE_INS:
	 NumCtysTotal = 1;
	 NumCtysWithInss = 1;
         sprintf (SubQuery,"centres.InsCod=%ld AND ",
                  Gbl.CurrentIns.Ins.InsCod);
	 NumCtysWithCtrs = Cty_GetNumCtysWithCtrs (SubQuery);
	 NumCtysWithDegs = Cty_GetNumCtysWithDegs (SubQuery);
	 NumCtysWithCrss = Cty_GetNumCtysWithCrss (SubQuery);
         NumCtysWithTchs = Cty_GetNumCtysWithUsrs (Rol_TCH,SubQuery);
         NumCtysWithNETs = Cty_GetNumCtysWithUsrs (Rol_NET,SubQuery);
	 NumCtysWithStds = Cty_GetNumCtysWithUsrs (Rol_STD,SubQuery);
         break;
      case Sco_SCOPE_CTR:
	 NumCtysTotal = 1;
	 NumCtysWithInss = 1;
	 NumCtysWithCtrs = 1;
         sprintf (SubQuery,"degrees.CtrCod=%ld AND ",
                  Gbl.CurrentCtr.Ctr.CtrCod);
	 NumCtysWithDegs = Cty_GetNumCtysWithDegs (SubQuery);
	 NumCtysWithCrss = Cty_GetNumCtysWithCrss (SubQuery);
         NumCtysWithTchs = Cty_GetNumCtysWithUsrs (Rol_TCH,SubQuery);
         NumCtysWithNETs = Cty_GetNumCtysWithUsrs (Rol_NET,SubQuery);
	 NumCtysWithStds = Cty_GetNumCtysWithUsrs (Rol_STD,SubQuery);
	 break;
      case Sco_SCOPE_DEG:
	 NumCtysTotal = 1;
	 NumCtysWithInss = 1;
	 NumCtysWithCtrs = 1;
	 NumCtysWithDegs = 1;
         sprintf (SubQuery,"courses.DegCod=%ld AND ",
                  Gbl.CurrentDeg.Deg.DegCod);
	 NumCtysWithCrss = Cty_GetNumCtysWithCrss (SubQuery);
         NumCtysWithTchs = Cty_GetNumCtysWithUsrs (Rol_TCH,SubQuery);
         NumCtysWithNETs = Cty_GetNumCtysWithUsrs (Rol_NET,SubQuery);
	 NumCtysWithStds = Cty_GetNumCtysWithUsrs (Rol_STD,SubQuery);
	 break;
     case Sco_SCOPE_CRS:
	 NumCtysTotal = 1;
	 NumCtysWithInss = 1;
	 NumCtysWithCtrs = 1;
	 NumCtysWithDegs = 1;
	 NumCtysWithCrss = 1;
         sprintf (SubQuery,"crs_usr.CrsCod=%ld AND ",
                  Gbl.CurrentCrs.Crs.CrsCod);
         NumCtysWithTchs = Cty_GetNumCtysWithUsrs (Rol_TCH,SubQuery);
         NumCtysWithNETs = Cty_GetNumCtysWithUsrs (Rol_NET,SubQuery);
	 NumCtysWithStds = Cty_GetNumCtysWithUsrs (Rol_STD,SubQuery);
	 break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }

   /***** Write number of countries *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"TIT_TBL LEFT_MIDDLE\">"
                      "<img src=\"%s/cty64x64.gif\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO20x20\" />"
                      "&nbsp;%s:"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "</tr>",
            Gbl.Prefs.IconsURL,
            Txt_Countries,
            Txt_Countries,
            Txt_Countries,
            NumCtysTotal,
            NumCtysWithInss,
            NumCtysWithCtrs,
            NumCtysWithDegs,
            NumCtysWithCrss,
            NumCtysWithTchs,
            NumCtysWithNETs,
            NumCtysWithStds);
  }

/*****************************************************************************/
/***************** Get and show total number of institutions *****************/
/*****************************************************************************/

static void Sta_GetAndShowNumInssInSWAD (void)
  {
   extern const char *Txt_Institutions;
   char SubQuery[128];
   unsigned NumInssTotal = 0;
   unsigned NumInssWithCtrs = 0;
   unsigned NumInssWithDegs = 0;
   unsigned NumInssWithCrss = 0;
   unsigned NumInssWithTchs = 0;
   unsigned NumInssWithNETs = 0;
   unsigned NumInssWithStds = 0;

   /***** Get number of institutions *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 NumInssTotal = Ins_GetNumInssTotal ();
	 NumInssWithCtrs = Ins_GetNumInssWithCtrs ("");
	 NumInssWithDegs = Ins_GetNumInssWithDegs ("");
	 NumInssWithCrss = Ins_GetNumInssWithCrss ("");
         NumInssWithTchs = Ins_GetNumInssWithUsrs (Rol_TCH,"");
         NumInssWithNETs = Ins_GetNumInssWithUsrs (Rol_NET,"");
	 NumInssWithStds = Ins_GetNumInssWithUsrs (Rol_STD,"");
         SubQuery[0] = '\0';
         break;
      case Sco_SCOPE_CTY:
	 NumInssTotal = Ins_GetNumInssInCty (Gbl.CurrentCty.Cty.CtyCod);
         sprintf (SubQuery,"institutions.CtyCod=%ld AND ",
                  Gbl.CurrentCty.Cty.CtyCod);
	 NumInssWithCtrs = Ins_GetNumInssWithCtrs (SubQuery);
	 NumInssWithDegs = Ins_GetNumInssWithDegs (SubQuery);
	 NumInssWithCrss = Ins_GetNumInssWithCrss (SubQuery);
         NumInssWithTchs = Ins_GetNumInssWithUsrs (Rol_TCH,SubQuery);
         NumInssWithNETs = Ins_GetNumInssWithUsrs (Rol_NET,SubQuery);
	 NumInssWithStds = Ins_GetNumInssWithUsrs (Rol_STD,SubQuery);
         break;
      case Sco_SCOPE_INS:
	 NumInssTotal = 1;
         sprintf (SubQuery,"centres.InsCod=%ld AND ",
                  Gbl.CurrentIns.Ins.InsCod);
	 NumInssWithCtrs = Ins_GetNumInssWithCtrs (SubQuery);
	 NumInssWithDegs = Ins_GetNumInssWithDegs (SubQuery);
	 NumInssWithCrss = Ins_GetNumInssWithCrss (SubQuery);
         NumInssWithTchs = Ins_GetNumInssWithUsrs (Rol_TCH,SubQuery);
         NumInssWithNETs = Ins_GetNumInssWithUsrs (Rol_NET,SubQuery);
	 NumInssWithStds = Ins_GetNumInssWithUsrs (Rol_STD,SubQuery);
         break;
      case Sco_SCOPE_CTR:
	 NumInssTotal = 1;
	 NumInssWithCtrs = 1;
         sprintf (SubQuery,"degrees.CtrCod=%ld AND ",
                  Gbl.CurrentCtr.Ctr.CtrCod);
	 NumInssWithDegs = Ins_GetNumInssWithDegs (SubQuery);
	 NumInssWithCrss = Ins_GetNumInssWithCrss (SubQuery);
         NumInssWithTchs = Ins_GetNumInssWithUsrs (Rol_TCH,SubQuery);
         NumInssWithNETs = Ins_GetNumInssWithUsrs (Rol_NET,SubQuery);
	 NumInssWithStds = Ins_GetNumInssWithUsrs (Rol_STD,SubQuery);
	 break;
      case Sco_SCOPE_DEG:
	 NumInssTotal = 1;
	 NumInssWithCtrs = 1;
	 NumInssWithDegs = 1;
         sprintf (SubQuery,"courses.DegCod=%ld AND ",
                  Gbl.CurrentDeg.Deg.DegCod);
	 NumInssWithCrss = Ins_GetNumInssWithCrss (SubQuery);
         NumInssWithTchs = Ins_GetNumInssWithUsrs (Rol_TCH,SubQuery);
         NumInssWithNETs = Ins_GetNumInssWithUsrs (Rol_NET,SubQuery);
	 NumInssWithStds = Ins_GetNumInssWithUsrs (Rol_STD,SubQuery);
	 break;
     case Sco_SCOPE_CRS:
	 NumInssTotal = 1;
	 NumInssWithCtrs = 1;
	 NumInssWithDegs = 1;
	 NumInssWithCrss = 1;
         sprintf (SubQuery,"crs_usr.CrsCod=%ld AND ",
                  Gbl.CurrentCrs.Crs.CrsCod);
         NumInssWithTchs = Ins_GetNumInssWithUsrs (Rol_TCH,SubQuery);
         NumInssWithNETs = Ins_GetNumInssWithUsrs (Rol_NET,SubQuery);
	 NumInssWithStds = Ins_GetNumInssWithUsrs (Rol_STD,SubQuery);
	 break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }

   /***** Write number of institutions *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"TIT_TBL LEFT_MIDDLE\">"
                      "<img src=\"%s/ins64x64.gif\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO20x20\" />"
                      "&nbsp;%s:"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td></td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "</tr>",
            Gbl.Prefs.IconsURL,
            Txt_Institutions,
            Txt_Institutions,
            Txt_Institutions,
            NumInssTotal,
            NumInssWithCtrs,
            NumInssWithDegs,
            NumInssWithCrss,
            NumInssWithTchs,
            NumInssWithNETs,
            NumInssWithStds);
  }

/*****************************************************************************/
/********************* Get and show total number of centres ******************/
/*****************************************************************************/

static void Sta_GetAndShowNumCtrsInSWAD (void)
  {
   extern const char *Txt_Centres;
   char SubQuery[128];
   unsigned NumCtrsTotal = 0;
   unsigned NumCtrsWithDegs = 0;
   unsigned NumCtrsWithCrss = 0;
   unsigned NumCtrsWithTchs = 0;
   unsigned NumCtrsWithNETs = 0;
   unsigned NumCtrsWithStds = 0;

   /***** Get number of centres *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 NumCtrsTotal = Ctr_GetNumCtrsTotal ();
	 NumCtrsWithDegs = Ctr_GetNumCtrsWithDegs ("");
	 NumCtrsWithCrss = Ctr_GetNumCtrsWithCrss ("");
         NumCtrsWithTchs = Ctr_GetNumCtrsWithUsrs (Rol_TCH,"");
         NumCtrsWithNETs = Ctr_GetNumCtrsWithUsrs (Rol_NET,"");
	 NumCtrsWithStds = Ctr_GetNumCtrsWithUsrs (Rol_STD,"");
         SubQuery[0] = '\0';
         break;
      case Sco_SCOPE_CTY:
	 NumCtrsTotal = Ctr_GetNumCtrsInCty (Gbl.CurrentCty.Cty.CtyCod);
         sprintf (SubQuery,"institutions.CtyCod=%ld AND ",
                  Gbl.CurrentCty.Cty.CtyCod);
	 NumCtrsWithDegs = Ctr_GetNumCtrsWithDegs (SubQuery);
	 NumCtrsWithCrss = Ctr_GetNumCtrsWithCrss (SubQuery);
         NumCtrsWithTchs = Ctr_GetNumCtrsWithUsrs (Rol_TCH,SubQuery);
         NumCtrsWithNETs = Ctr_GetNumCtrsWithUsrs (Rol_NET,SubQuery);
	 NumCtrsWithStds = Ctr_GetNumCtrsWithUsrs (Rol_STD,SubQuery);
         break;
      case Sco_SCOPE_INS:
	 NumCtrsTotal = Ctr_GetNumCtrsInIns (Gbl.CurrentIns.Ins.InsCod);
         sprintf (SubQuery,"centres.InsCod=%ld AND ",
                  Gbl.CurrentIns.Ins.InsCod);
	 NumCtrsWithDegs = Ctr_GetNumCtrsWithDegs (SubQuery);
	 NumCtrsWithCrss = Ctr_GetNumCtrsWithCrss (SubQuery);
         NumCtrsWithTchs = Ctr_GetNumCtrsWithUsrs (Rol_TCH,SubQuery);
         NumCtrsWithNETs = Ctr_GetNumCtrsWithUsrs (Rol_NET,SubQuery);
	 NumCtrsWithStds = Ctr_GetNumCtrsWithUsrs (Rol_STD,SubQuery);
         break;
      case Sco_SCOPE_CTR:
	 NumCtrsTotal = 1;
         sprintf (SubQuery,"degrees.CtrCod=%ld AND ",
                  Gbl.CurrentCtr.Ctr.CtrCod);
	 NumCtrsWithDegs = Ctr_GetNumCtrsWithDegs (SubQuery);
	 NumCtrsWithCrss = Ctr_GetNumCtrsWithCrss (SubQuery);
         NumCtrsWithTchs = Ctr_GetNumCtrsWithUsrs (Rol_TCH,SubQuery);
         NumCtrsWithNETs = Ctr_GetNumCtrsWithUsrs (Rol_NET,SubQuery);
	 NumCtrsWithStds = Ctr_GetNumCtrsWithUsrs (Rol_STD,SubQuery);
	 break;
      case Sco_SCOPE_DEG:
	 NumCtrsTotal = 1;
	 NumCtrsWithDegs = 1;
         sprintf (SubQuery,"courses.DegCod=%ld AND ",
                  Gbl.CurrentDeg.Deg.DegCod);
	 NumCtrsWithCrss = Ctr_GetNumCtrsWithCrss (SubQuery);
         NumCtrsWithTchs = Ctr_GetNumCtrsWithUsrs (Rol_TCH,SubQuery);
         NumCtrsWithNETs = Ctr_GetNumCtrsWithUsrs (Rol_NET,SubQuery);
	 NumCtrsWithStds = Ctr_GetNumCtrsWithUsrs (Rol_STD,SubQuery);
	 break;
     case Sco_SCOPE_CRS:
	 NumCtrsTotal = 1;
	 NumCtrsWithDegs = 1;
	 NumCtrsWithCrss = 1;
         sprintf (SubQuery,"crs_usr.CrsCod=%ld AND ",
                  Gbl.CurrentCrs.Crs.CrsCod);
         NumCtrsWithTchs = Ctr_GetNumCtrsWithUsrs (Rol_TCH,SubQuery);
         NumCtrsWithNETs = Ctr_GetNumCtrsWithUsrs (Rol_NET,SubQuery);
	 NumCtrsWithStds = Ctr_GetNumCtrsWithUsrs (Rol_STD,SubQuery);
	 break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }

   /***** Write number of centres *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"TIT_TBL LEFT_MIDDLE\">"
                      "<img src=\"%s/ctr64x64.gif\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO20x20\" />"
                      "&nbsp;%s:"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td></td>"
                      "<td></td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "</tr>",
            Gbl.Prefs.IconsURL,
            Txt_Centres,
            Txt_Centres,
            Txt_Centres,
            NumCtrsTotal,
            NumCtrsWithDegs,
            NumCtrsWithCrss,
            NumCtrsWithTchs,
            NumCtrsWithNETs,
            NumCtrsWithStds);
  }

/*****************************************************************************/
/********************* Get and show total number of degrees ******************/
/*****************************************************************************/

static void Sta_GetAndShowNumDegsInSWAD (void)
  {
   extern const char *Txt_Degrees;
   char SubQuery[128];
   unsigned NumDegsTotal = 0;
   unsigned NumDegsWithCrss = 0;
   unsigned NumDegsWithTchs = 0;
   unsigned NumDegsWithNETs = 0;
   unsigned NumDegsWithStds = 0;

   /***** Get number of degrees *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 NumDegsTotal = Deg_GetNumDegsTotal ();
	 NumDegsWithCrss = Deg_GetNumDegsWithCrss ("");
         NumDegsWithTchs = Deg_GetNumDegsWithUsrs (Rol_TCH,"");
         NumDegsWithNETs = Deg_GetNumDegsWithUsrs (Rol_NET,"");
	 NumDegsWithStds = Deg_GetNumDegsWithUsrs (Rol_STD,"");
         SubQuery[0] = '\0';
         break;
      case Sco_SCOPE_CTY:
	 NumDegsTotal = Deg_GetNumDegsInCty (Gbl.CurrentCty.Cty.CtyCod);
         sprintf (SubQuery,"institutions.CtyCod=%ld AND ",
                  Gbl.CurrentCty.Cty.CtyCod);
	 NumDegsWithCrss = Deg_GetNumDegsWithCrss (SubQuery);
         NumDegsWithTchs = Deg_GetNumDegsWithUsrs (Rol_TCH,SubQuery);
         NumDegsWithNETs = Deg_GetNumDegsWithUsrs (Rol_NET,SubQuery);
	 NumDegsWithStds = Deg_GetNumDegsWithUsrs (Rol_STD,SubQuery);
         break;
      case Sco_SCOPE_INS:
	 NumDegsTotal = Deg_GetNumDegsInIns (Gbl.CurrentIns.Ins.InsCod);
         sprintf (SubQuery,"centres.InsCod=%ld AND ",
                  Gbl.CurrentIns.Ins.InsCod);
	 NumDegsWithCrss = Deg_GetNumDegsWithCrss (SubQuery);
         NumDegsWithTchs = Deg_GetNumDegsWithUsrs (Rol_TCH,SubQuery);
         NumDegsWithNETs = Deg_GetNumDegsWithUsrs (Rol_NET,SubQuery);
	 NumDegsWithStds = Deg_GetNumDegsWithUsrs (Rol_STD,SubQuery);
         break;
      case Sco_SCOPE_CTR:
	 NumDegsTotal = Deg_GetNumDegsInCtr (Gbl.CurrentCtr.Ctr.CtrCod);
         sprintf (SubQuery,"degrees.CtrCod=%ld AND ",
                  Gbl.CurrentCtr.Ctr.CtrCod);
	 NumDegsWithCrss = Deg_GetNumDegsWithCrss (SubQuery);
         NumDegsWithTchs = Deg_GetNumDegsWithUsrs (Rol_TCH,SubQuery);
         NumDegsWithNETs = Deg_GetNumDegsWithUsrs (Rol_NET,SubQuery);
	 NumDegsWithStds = Deg_GetNumDegsWithUsrs (Rol_STD,SubQuery);
	 break;
      case Sco_SCOPE_DEG:
	 NumDegsTotal = 1;
         sprintf (SubQuery,"courses.DegCod=%ld AND ",
                  Gbl.CurrentDeg.Deg.DegCod);
	 NumDegsWithCrss = Deg_GetNumDegsWithCrss (SubQuery);
         NumDegsWithTchs = Deg_GetNumDegsWithUsrs (Rol_TCH,SubQuery);
         NumDegsWithNETs = Deg_GetNumDegsWithUsrs (Rol_NET,SubQuery);
	 NumDegsWithStds = Deg_GetNumDegsWithUsrs (Rol_STD,SubQuery);
	 break;
     case Sco_SCOPE_CRS:
	 NumDegsTotal = 1;
	 NumDegsWithCrss = 1;
         sprintf (SubQuery,"crs_usr.CrsCod=%ld AND ",
                  Gbl.CurrentCrs.Crs.CrsCod);
         NumDegsWithTchs = Deg_GetNumDegsWithUsrs (Rol_TCH,SubQuery);
         NumDegsWithNETs = Deg_GetNumDegsWithUsrs (Rol_NET,SubQuery);
	 NumDegsWithStds = Deg_GetNumDegsWithUsrs (Rol_STD,SubQuery);
	 break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }

   /***** Write number of degrees *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"TIT_TBL LEFT_MIDDLE\">"
                      "<img src=\"%s/deg64x64.gif\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO20x20\" />"
                      "&nbsp;%s:"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td></td>"
                      "<td></td>"
                      "<td></td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "</tr>",
            Gbl.Prefs.IconsURL,
            Txt_Degrees,
            Txt_Degrees,
            Txt_Degrees,
            NumDegsTotal,
            NumDegsWithCrss,
            NumDegsWithTchs,
            NumDegsWithNETs,
            NumDegsWithStds);
  }

/*****************************************************************************/
/****************** Get and show total number of courses *********************/
/*****************************************************************************/

static void Sta_GetAndShowNumCrssInSWAD (void)
  {
   extern const char *Txt_Courses;
   char SubQuery[128];
   unsigned NumCrssTotal = 0;
   unsigned NumCrssWithTchs = 0;
   unsigned NumCrssWithNETs = 0;
   unsigned NumCrssWithStds = 0;

   /***** Get number of courses *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 NumCrssTotal = Crs_GetNumCrssTotal ();
         NumCrssWithTchs = Crs_GetNumCrssWithUsrs (Rol_TCH,"");
         NumCrssWithNETs = Crs_GetNumCrssWithUsrs (Rol_NET,"");
	 NumCrssWithStds = Crs_GetNumCrssWithUsrs (Rol_STD,"");
         SubQuery[0] = '\0';
         break;
      case Sco_SCOPE_CTY:
	 NumCrssTotal = Crs_GetNumCrssInCty (Gbl.CurrentCty.Cty.CtyCod);
         sprintf (SubQuery,"institutions.CtyCod=%ld AND ",
                  Gbl.CurrentCty.Cty.CtyCod);
         NumCrssWithTchs = Crs_GetNumCrssWithUsrs (Rol_TCH,SubQuery);
         NumCrssWithNETs = Crs_GetNumCrssWithUsrs (Rol_NET,SubQuery);
	 NumCrssWithStds = Crs_GetNumCrssWithUsrs (Rol_STD,SubQuery);
         break;
      case Sco_SCOPE_INS:
	 NumCrssTotal = Crs_GetNumCrssInIns (Gbl.CurrentIns.Ins.InsCod);
         sprintf (SubQuery,"centres.InsCod=%ld AND ",
                  Gbl.CurrentIns.Ins.InsCod);
         NumCrssWithTchs = Crs_GetNumCrssWithUsrs (Rol_TCH,SubQuery);
         NumCrssWithNETs = Crs_GetNumCrssWithUsrs (Rol_NET,SubQuery);
	 NumCrssWithStds = Crs_GetNumCrssWithUsrs (Rol_STD,SubQuery);
         break;
      case Sco_SCOPE_CTR:
	 NumCrssTotal = Crs_GetNumCrssInCtr (Gbl.CurrentCtr.Ctr.CtrCod);
         sprintf (SubQuery,"degrees.CtrCod=%ld AND ",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         NumCrssWithTchs = Crs_GetNumCrssWithUsrs (Rol_TCH,SubQuery);
         NumCrssWithNETs = Crs_GetNumCrssWithUsrs (Rol_NET,SubQuery);
	 NumCrssWithStds = Crs_GetNumCrssWithUsrs (Rol_STD,SubQuery);
	 break;
      case Sco_SCOPE_DEG:
	 NumCrssTotal = Crs_GetNumCrssInDeg (Gbl.CurrentDeg.Deg.DegCod);
         sprintf (SubQuery,"courses.DegCod=%ld AND ",
                  Gbl.CurrentDeg.Deg.DegCod);
         NumCrssWithTchs = Crs_GetNumCrssWithUsrs (Rol_TCH,SubQuery);
         NumCrssWithNETs = Crs_GetNumCrssWithUsrs (Rol_NET,SubQuery);
	 NumCrssWithStds = Crs_GetNumCrssWithUsrs (Rol_STD,SubQuery);
	 break;
     case Sco_SCOPE_CRS:
	 NumCrssTotal = 1;
         sprintf (SubQuery,"crs_usr.CrsCod=%ld AND ",
                  Gbl.CurrentCrs.Crs.CrsCod);
         NumCrssWithTchs = Crs_GetNumCrssWithUsrs (Rol_TCH,SubQuery);
         NumCrssWithNETs = Crs_GetNumCrssWithUsrs (Rol_NET,SubQuery);
	 NumCrssWithStds = Crs_GetNumCrssWithUsrs (Rol_STD,SubQuery);
	 break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }

   /***** Write number of courses *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"TIT_TBL LEFT_MIDDLE\">"
                      "<img src=\"%s/crs64x64.gif\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO20x20\" />"
                      "&nbsp;%s:"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td></td>"
                      "<td></td>"
                      "<td></td>"
                      "<td></td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "</tr>",
            Gbl.Prefs.IconsURL,
            Txt_Courses,
            Txt_Courses,
            Txt_Courses,
            NumCrssTotal,
            NumCrssWithTchs,
            NumCrssWithNETs,
            NumCrssWithStds);
  }

/*****************************************************************************/
/****************** Get and show stats about institutions ********************/
/*****************************************************************************/

static void Sta_GetAndShowInstitutionsStats (void)
  {
   extern const char *Hlp_STATS_Figures_institutions;
   extern const char *Txt_Institutions;

   /***** Get and update type of list, number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Form to select type of list used to display degree photos *****/
   Lay_StartRoundFrame (NULL,Txt_Institutions,
                        NULL,Hlp_STATS_Figures_institutions);
   Usr_ShowFormsToSelectUsrListType (ActSeeUseGbl);
   Lay_EndRoundFrame ();

   /****** Institutions ordered by number of centres ******/
   Sta_GetAndShowInssOrderedByNumCtrs ();

   /****** Institutions ordered by number of degrees ******/
   Sta_GetAndShowInssOrderedByNumDegs ();

   /****** Institutions ordered by number of courses ******/
   Sta_GetAndShowInssOrderedByNumCrss ();

   /****** Institutions ordered by number of users in courses ******/
   Sta_GetAndShowInssOrderedByNumUsrsInCrss ();

   /****** Institutions ordered by number of users who claim to belong to them ******/
   Sta_GetAndShowInssOrderedByNumUsrsWhoClaimToBelongToThem ();
  }

/*****************************************************************************/
/**** Get and show stats about institutions ordered by number of centres *****/
/*****************************************************************************/

static void Sta_GetAndShowInssOrderedByNumCtrs (void)
  {
   extern const char *Txt_Institutions_by_number_of_centres;
   extern const char *Txt_Centres;
   char Query[1024];

   /****** Institutions ordered by number of centres ******/
   Lay_StartRoundFrameTable ("95%",Txt_Institutions_by_number_of_centres,
                             NULL,NULL,2);

   /***** Get institutions ordered by number of centres *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 sprintf (Query,"SELECT InsCod,COUNT(*) AS N"
			" FROM centres"
			" GROUP BY InsCod"
			" ORDER BY N DESC");
         break;
      case Sco_SCOPE_CTY:
            sprintf (Query,"SELECT centres.InsCod,COUNT(*) AS N"
        	           " FROM institutions,centres"
                           " WHERE institutions.CtyCod=%ld"
                           " AND institutions.InsCod=centres.InsCod"
			   " GROUP BY centres.InsCod"
			   " ORDER BY N DESC",
	             Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
      case Sco_SCOPE_CTR:
      case Sco_SCOPE_DEG:
      case Sco_SCOPE_CRS:
            sprintf (Query,"SELECT InsCod,COUNT(*) AS N"
			   " FROM centres"
                           " WHERE InsCod=%ld"
			   " GROUP BY InsCod"
			   " ORDER BY N DESC",
                     Gbl.CurrentIns.Ins.InsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   Sta_GetAndShowInss (Query,Txt_Centres);

   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/**** Get and show stats about institutions ordered by number of degrees *****/
/*****************************************************************************/

static void Sta_GetAndShowInssOrderedByNumDegs (void)
  {
   extern const char *Txt_Institutions_by_number_of_degrees;
   extern const char *Txt_Degrees;
   char Query[1024];

   /****** Institutions ordered by number of centres ******/
   Lay_StartRoundFrameTable ("95%",Txt_Institutions_by_number_of_degrees,
                             NULL,NULL,2);

   /***** Get institutions ordered by number of degrees *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 sprintf (Query,"SELECT centres.InsCod,COUNT(*) AS N"
			" FROM centres,degrees"
	                " WHERE centres.CtrCod=degrees.CtrCod"
			" GROUP BY InsCod"
			" ORDER BY N DESC");
         break;
      case Sco_SCOPE_CTY:
            sprintf (Query,"SELECT centres.InsCod,COUNT(*) AS N"
        	           " FROM institutions,centres,degrees"
                           " WHERE institutions.CtyCod=%ld"
                           " AND institutions.InsCod=centres.InsCod"
	                   " AND centres.CtrCod=degrees.CtrCod"
			   " GROUP BY centres.InsCod"
			   " ORDER BY N DESC",
	             Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
      case Sco_SCOPE_CTR:
      case Sco_SCOPE_DEG:
      case Sco_SCOPE_CRS:
            sprintf (Query,"SELECT centres.InsCod,COUNT(*) AS N"
			   " FROM centres,degrees"
                           " WHERE centres.InsCod=%ld"
	                   " AND centres.CtrCod=degrees.CtrCod"
			   " GROUP BY centres.InsCod"
			   " ORDER BY N DESC",
                     Gbl.CurrentIns.Ins.InsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   Sta_GetAndShowInss (Query,Txt_Degrees);

   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/**** Get and show stats about institutions ordered by number of courses *****/
/*****************************************************************************/

static void Sta_GetAndShowInssOrderedByNumCrss (void)
  {
   extern const char *Txt_Institutions_by_number_of_courses;
   extern const char *Txt_Courses;
   char Query[1024];

   /****** Institutions ordered by number of centres ******/
   Lay_StartRoundFrameTable ("95%",Txt_Institutions_by_number_of_courses,
                             NULL,NULL,2);

   /***** Get institutions ordered by number of courses *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 sprintf (Query,"SELECT centres.InsCod,COUNT(*) AS N"
			" FROM centres,degrees,courses"
	                " WHERE centres.CtrCod=degrees.CtrCod"
	                " AND degrees.DegCod=courses.DegCod"
			" GROUP BY InsCod"
			" ORDER BY N DESC");
         break;
      case Sco_SCOPE_CTY:
            sprintf (Query,"SELECT centres.InsCod,COUNT(*) AS N"
        	           " FROM institutions,centres,degrees,courses"
                           " WHERE institutions.CtyCod=%ld"
                           " AND institutions.InsCod=centres.InsCod"
	                   " AND centres.CtrCod=degrees.CtrCod"
	                   " AND degrees.DegCod=courses.DegCod"
			   " GROUP BY centres.InsCod"
			   " ORDER BY N DESC",
	             Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
      case Sco_SCOPE_CTR:
      case Sco_SCOPE_DEG:
      case Sco_SCOPE_CRS:
            sprintf (Query,"SELECT centres.InsCod,COUNT(*) AS N"
			   " FROM centres,degrees,courses"
                           " WHERE centres.InsCod=%ld"
	                   " AND centres.CtrCod=degrees.CtrCod"
	                   " AND degrees.DegCod=courses.DegCod"
			   " GROUP BY centres.InsCod"
			   " ORDER BY N DESC",
                     Gbl.CurrentIns.Ins.InsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   Sta_GetAndShowInss (Query,Txt_Courses);

   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/***** Get and show stats about institutions ordered by users in courses *****/
/*****************************************************************************/

static void Sta_GetAndShowInssOrderedByNumUsrsInCrss (void)
  {
   extern const char *Txt_Institutions_by_number_of_users_in_courses;
   extern const char *Txt_Users;
   char Query[1024];

   /****** Institutions ordered by number of centres ******/
   Lay_StartRoundFrameTable ("95%",Txt_Institutions_by_number_of_users_in_courses,
                             NULL,NULL,2);

   /***** Get institutions ordered by number of users in courses *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 sprintf (Query,"SELECT centres.InsCod,COUNT(DISTINCT crs_usr.UsrCod) AS N"
			" FROM centres,degrees,courses,crs_usr"
	                " WHERE centres.CtrCod=degrees.CtrCod"
	                " AND degrees.DegCod=courses.DegCod"
	                " AND courses.CrsCod=crs_usr.CrsCod"
			" GROUP BY InsCod"
			" ORDER BY N DESC");
         break;
      case Sco_SCOPE_CTY:
            sprintf (Query,"SELECT centres.InsCod,COUNT(DISTINCT crs_usr.UsrCod) AS N"
        	           " FROM institutions,centres,degrees,courses,crs_usr"
                           " WHERE institutions.CtyCod=%ld"
                           " AND institutions.InsCod=centres.InsCod"
	                   " AND centres.CtrCod=degrees.CtrCod"
	                   " AND degrees.DegCod=courses.DegCod"
	                   " AND courses.CrsCod=crs_usr.CrsCod"
			   " GROUP BY centres.InsCod"
			   " ORDER BY N DESC",
	             Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
      case Sco_SCOPE_CTR:
      case Sco_SCOPE_DEG:
      case Sco_SCOPE_CRS:
            sprintf (Query,"SELECT centres.InsCod,COUNT(DISTINCT crs_usr.UsrCod) AS N"
			   " FROM centres,degrees,courses,crs_usr"
                           " WHERE centres.InsCod=%ld"
	                   " AND centres.CtrCod=degrees.CtrCod"
	                   " AND degrees.DegCod=courses.DegCod"
	                   " AND courses.CrsCod=crs_usr.CrsCod"
			   " GROUP BY centres.InsCod"
			   " ORDER BY N DESC",
                     Gbl.CurrentIns.Ins.InsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   Sta_GetAndShowInss (Query,Txt_Users);

   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/************* Get and show stats about institutions ordered by **************/
/************* number of users who claim to belong to them      **************/
/*****************************************************************************/

static void Sta_GetAndShowInssOrderedByNumUsrsWhoClaimToBelongToThem (void)
  {
   extern const char *Txt_Institutions_by_number_of_users_who_claim_to_belong_to_them;
   extern const char *Txt_Users;
   char Query[1024];

   /****** Institutions ordered by number of centres ******/
   Lay_StartRoundFrameTable ("95%",Txt_Institutions_by_number_of_users_who_claim_to_belong_to_them,
                             NULL,NULL,2);

   /***** Get institutions ordered by number of users who claim to belong to them *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 sprintf (Query,"SELECT InsCod,COUNT(*) AS N"
			" FROM usr_data"
                        " WHERE InsCod>0"
			" GROUP BY InsCod"
			" ORDER BY N DESC");
         break;
      case Sco_SCOPE_CTY:
            sprintf (Query,"SELECT usr_data.InsCod,COUNT(*) AS N"
        	           " FROM institutions,usr_data"
                           " WHERE institutions.CtyCod=%ld"
                           " AND institutions.InsCod=usr_data.InsCod"
			   " GROUP BY usr_data.InsCod"
			   " ORDER BY N DESC",
	             Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
      case Sco_SCOPE_CTR:
      case Sco_SCOPE_DEG:
      case Sco_SCOPE_CRS:
            sprintf (Query,"SELECT InsCod,COUNT(*) AS N"
			   " FROM usr_data"
                           " WHERE InsCod=%ld"
			   " GROUP BY InsCod"
			   " ORDER BY N DESC",
                     Gbl.CurrentIns.Ins.InsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   Sta_GetAndShowInss (Query,Txt_Users);

   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/****************** Get and show stats about institutions ********************/
/*****************************************************************************/

static void Sta_GetAndShowInss (const char *Query,const char *TxtFigure)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Institution;
   MYSQL_RES *mysql_res;
   unsigned NumInss;
   unsigned NumIns;
   unsigned NumOrder;
   unsigned NumberLastRow;
   unsigned NumberThisRow;
   struct Instit Ins;
   bool TRIsOpen = false;

   /***** Query database *****/
   if ((NumInss = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get institutions")))
     {
      /* Draw the classphoto/list */
      switch (Gbl.Usrs.Me.ListType)
	{
	 case Usr_LIST_AS_CLASS_PHOTO:
	    /***** Draw institutions as a class photo *****/
	    for (NumIns = 0;
		 NumIns < NumInss;)
	      {
	       if ((NumIns % Gbl.Usrs.ClassPhoto.Cols) == 0)
		 {
		  fprintf (Gbl.F.Out,"<tr>");
		  TRIsOpen = true;
		 }

	       /***** Get institution data and statistic *****/
	       NumberThisRow = Sta_GetInsAndStat (&Ins,mysql_res);

	       /***** Write link to institution *****/
	       fprintf (Gbl.F.Out,"<td class=\"%s CENTER_MIDDLE\">",
			The_ClassForm[Gbl.Prefs.Theme]);
	       Ins_DrawInstitutionLogoWithLink (&Ins,40);
               fprintf (Gbl.F.Out,"<br />%u</td>",
	                NumberThisRow);

	       /***** End of user's cell *****/
	       fprintf (Gbl.F.Out,"</td>");

	       if ((++NumIns % Gbl.Usrs.ClassPhoto.Cols) == 0)
		 {
		  fprintf (Gbl.F.Out,"</tr>");
		  TRIsOpen = false;
		 }
	      }
	    if (TRIsOpen)
	       fprintf (Gbl.F.Out,"</tr>");

	    break;
	 case Usr_LIST_AS_LISTING:
	    /***** Draw institutions as a list *****/
	    fprintf (Gbl.F.Out,"<tr>"
			       "<th></th>"
			       "<th class=\"LEFT_MIDDLE\">"
			       "%s"
			       "</th>"
			       "<th class=\"RIGHT_MIDDLE\">"
			       "%s"
			       "</th>"
			       "</tr>",
		     Txt_Institution,
		     TxtFigure);

	    for (NumIns = 1, NumOrder = 1, NumberLastRow = 0;
		 NumIns <= NumInss;
		 NumIns++)
	      {
	       /***** Get institution data and statistic *****/
	       NumberThisRow = Sta_GetInsAndStat (&Ins,mysql_res);

	       /***** Number of order *****/
	       if (NumberThisRow != NumberLastRow)
		  NumOrder = NumIns;
	       fprintf (Gbl.F.Out,"<tr>"
				  "<td class=\"DAT RIGHT_MIDDLE\">"
				  "%u"
				  "</td>",
			NumOrder);

	       /***** Write link to institution *****/
	       fprintf (Gbl.F.Out,"<td class=\"%s LEFT_MIDDLE\">",
			The_ClassForm[Gbl.Prefs.Theme]);

	       /* Icon and name of this institution */
	       Act_FormStart (ActSeeInsInf);
	       Ins_PutParamInsCod (Ins.InsCod);
	       Act_LinkFormSubmit (Ins.ShrtName,The_ClassForm[Gbl.Prefs.Theme],NULL);
	       if (Gbl.Usrs.Listing.WithPhotos)
		 {
		  Log_DrawLogo (Sco_SCOPE_INS,Ins.InsCod,Ins.ShrtName,
				40,NULL,true);
	          fprintf (Gbl.F.Out,"&nbsp;");
		 }
	       fprintf (Gbl.F.Out,"%s</a>",Ins.FullName);
	       Act_FormEnd ();

	       fprintf (Gbl.F.Out,"</td>");

	       /***** Write statistic *****/
	       fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
				  "%u"
				  "</td>"
				  "</tr>",
			NumberThisRow);

	       NumberLastRow = NumberThisRow;
	      }
	    break;
	 default:
	    break;
	}
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Get institution data and statistic *********************/
/*****************************************************************************/

static unsigned Sta_GetInsAndStat (struct Instit *Ins,MYSQL_RES *mysql_res)
  {
   MYSQL_ROW row;
   unsigned NumberThisRow;

   /***** Get next institution *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get data of this institution (row[0]) *****/
   Ins->InsCod = Str_ConvertStrCodToLongCod (row[0]);
   if (!Ins_GetDataOfInstitutionByCod (Ins,Ins_GET_BASIC_DATA))
      Lay_ShowErrorAndExit ("Institution not found.");

   /***** Get statistic (row[1]) *****/
   if (sscanf (row[1],"%u",&NumberThisRow) != 1)
      Lay_ShowErrorAndExit ("Error in statistic");

   return NumberThisRow;
  }

/*****************************************************************************/
/****************** Get and show stats about institutions ********************/
/*****************************************************************************/

static void Sta_GetAndShowDegreeTypesStats (void)
  {
   /***** Show statistic about number of degrees in each type of degree *****/
   DT_SeeDegreeTypesInStaTab ();
  }

/*****************************************************************************/
/********************* Show stats about exploration trees ********************/
/*****************************************************************************/
// TODO: add links to statistic

static void Sta_GetAndShowFileBrowsersStats (void)
  {
   extern const char *Hlp_STATS_Figures_folders_and_files;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_STAT_COURSE_FILE_ZONES[];
   extern const char *Txt_Briefcases;
   static const Brw_FileBrowser_t StatCrsFileZones[Sta_NUM_STAT_CRS_FILE_ZONES] =
     {
      Brw_ADMI_DOCUM_CRS,
      Brw_ADMI_DOCUM_GRP,
      Brw_ADMI_TEACH_CRS,
      Brw_ADMI_TEACH_GRP,
      Brw_ADMI_SHARE_CRS,
      Brw_ADMI_SHARE_GRP,
      Brw_ADMI_MARKS_CRS,
      Brw_ADMI_MARKS_GRP,
      Brw_ADMI_ASSIG_USR,
      Brw_ADMI_WORKS_USR,
      Brw_UNKNOWN,
     };
   unsigned NumStat;

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_FOLDERS_AND_FILES],
                             NULL,Hlp_STATS_Figures_folders_and_files,2);

   /***** Write table heading *****/
   Sta_WriteStatsExpTreesTableHead ();

   /***** Write sizes of course file zones *****/
   for (NumStat = 0;
	NumStat < Sta_NUM_STAT_CRS_FILE_ZONES;
	NumStat++)
      Sta_WriteRowStatsFileBrowsers (StatCrsFileZones[NumStat],Txt_STAT_COURSE_FILE_ZONES[NumStat]);

   /***** Write table heading *****/
   Sta_WriteStatsExpTreesTableHead ();

   /***** Write size of briefcases *****/
   Sta_WriteRowStatsFileBrowsers (Brw_ADMI_BRIEF_USR,Txt_Briefcases);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/*********** Write table heading for stats of exploration trees **************/
/*****************************************************************************/

static void Sta_WriteStatsExpTreesTableHead (void)
  {
   extern const char *Txt_File_zones;
   extern const char *Txt_Courses;
   extern const char *Txt_Groups;
   extern const char *Txt_Users;
   extern const char *Txt_Max_levels;
   extern const char *Txt_Folders;
   extern const char *Txt_Files;
   extern const char *Txt_Size;
   extern const char *Txt_STAT_COURSE_FILE_ZONES[];
   extern const char *Txt_crs;
   extern const char *Txt_usr;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s/<br />%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s/<br />%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s/<br />%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s/<br />%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s/<br />%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s/<br />%s"
                      "</th>"
                      "</tr>",
            Txt_File_zones,
            Txt_Courses,
            Txt_Groups,
            Txt_Users,
            Txt_Max_levels,
            Txt_Folders,
            Txt_Files,
            Txt_Size,
            Txt_Folders,Txt_crs,
            Txt_Files,Txt_crs,
            Txt_Size,Txt_crs,
            Txt_Folders,Txt_usr,
            Txt_Files,Txt_usr,
            Txt_Size,Txt_usr);
  }

/*****************************************************************************/
/*************** Write a row of stats of exploration trees *******************/
/*****************************************************************************/

static void Sta_WriteRowStatsFileBrowsers (Brw_FileBrowser_t FileZone,const char *NameOfFileZones)
  {
   char StrNumCrss[10 + 1];
   char StrNumGrps[10 + 1];
   char StrNumUsrs[10 + 1];
   char StrNumFoldersPerCrs[10 + 1];
   char StrNumFoldersPerUsr[10 + 1];
   char StrNumFilesPerCrs[10 + 1];
   char StrNumFilesPerUsr[10 + 1];
   struct Sta_SizeOfFileZones SizeOfFileZones;
   char FileSizeStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];
   char FileSizePerCrsStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];
   char FileSizePerUsrStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];
   char *Class = (FileZone == Brw_UNKNOWN) ? "DAT_N_LINE_TOP" :
	                                     "DAT";

   Sta_GetSizeOfFileZoneFromDB (Gbl.Scope.Current,FileZone,&SizeOfFileZones);

   Fil_WriteFileSizeFull ((double) SizeOfFileZones.Size,FileSizeStr);

   if (SizeOfFileZones.NumCrss == -1)
     {
      Str_Copy (StrNumCrss,"-",
                10);
      Str_Copy (StrNumFoldersPerCrs,"-",
                10);
      Str_Copy (StrNumFilesPerCrs,"-",
                10);
      Str_Copy (FileSizePerCrsStr,"-",
                Fil_MAX_BYTES_FILE_SIZE_STRING);
     }
   else
     {
      sprintf (StrNumCrss,"%d",SizeOfFileZones.NumCrss);
      sprintf (StrNumFoldersPerCrs,"%.1f",
               SizeOfFileZones.NumCrss ? (double) SizeOfFileZones.NumFolders /
        	                         (double) SizeOfFileZones.NumCrss :
        	                         0.0);
      sprintf (StrNumFilesPerCrs,"%.1f",
               SizeOfFileZones.NumCrss ? (double) SizeOfFileZones.NumFiles /
        	                         (double) SizeOfFileZones.NumCrss :
        	                         0.0);
      Fil_WriteFileSizeFull (SizeOfFileZones.NumCrss ? (double) SizeOfFileZones.Size /
	                                               (double) SizeOfFileZones.NumCrss :
	                                               0.0,
	                     FileSizePerCrsStr);
     }

   if (SizeOfFileZones.NumGrps == -1)
      Str_Copy (StrNumGrps,"-",
                10);
   else
      sprintf (StrNumGrps,"%d",SizeOfFileZones.NumGrps);

   if (SizeOfFileZones.NumUsrs == -1)
     {
      Str_Copy (StrNumUsrs,"-",
                10);
      Str_Copy (StrNumFoldersPerUsr,"-",
                10);
      Str_Copy (StrNumFilesPerUsr,"-",
                10);
      Str_Copy (FileSizePerUsrStr,"-",
                Fil_MAX_BYTES_FILE_SIZE_STRING);
     }
   else
     {
      sprintf (StrNumUsrs,"%d",SizeOfFileZones.NumUsrs);
      sprintf (StrNumFoldersPerUsr,"%.1f",
               SizeOfFileZones.NumUsrs ? (double) SizeOfFileZones.NumFolders /
        	                         (double) SizeOfFileZones.NumUsrs :
        	                         0.0);
      sprintf (StrNumFilesPerUsr,"%.1f",
               SizeOfFileZones.NumUsrs ? (double) SizeOfFileZones.NumFiles /
        	                         (double) SizeOfFileZones.NumUsrs :
        	                         0.0);
      Fil_WriteFileSizeFull (SizeOfFileZones.NumUsrs ? (double) SizeOfFileZones.Size /
	                                               (double) SizeOfFileZones.NumUsrs :
	                                               0.0,
	                     FileSizePerUsrStr);
     }

   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s LEFT_MIDDLE\">"
                      "%s"
                      "</td>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%s"
                      "</td>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%s"
                      "</td>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%s"
                      "</td>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%lu"
                      "</td>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%lu"
                      "</td>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%s"
                      "</td>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%s"
                      "</td>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%s"
                      "</td>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%s"
                      "</td>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%s"
                      "</td>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%s"
                      "</td>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%s"
                      "</td>"
	              "</tr>",
            Class,NameOfFileZones,
            Class,StrNumCrss,
            Class,StrNumGrps,
            Class,StrNumUsrs,
            Class,SizeOfFileZones.MaxLevels,
            Class,SizeOfFileZones.NumFolders,
            Class,SizeOfFileZones.NumFiles,
            Class,FileSizeStr,
            Class,StrNumFoldersPerCrs,
            Class,StrNumFilesPerCrs,
            Class,FileSizePerCrsStr,
            Class,StrNumFoldersPerUsr,
            Class,StrNumFilesPerUsr,
            Class,FileSizePerUsrStr);
  }

/*****************************************************************************/
/**************** Get the size of a file zone from database ******************/
/*****************************************************************************/

static void Sta_GetSizeOfFileZoneFromDB (Sco_Scope_t Scope,
                                         Brw_FileBrowser_t FileBrowser,
                                         struct Sta_SizeOfFileZones *SizeOfFileZones)
  {
   char Query[2048];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get the size of a file browser *****/
   switch (Scope)
     {
      /* Scope = the whole platform */
      case Sco_SCOPE_SYS:
	 switch (FileBrowser)
	   {
	    case Brw_UNKNOWN:
	       sprintf (Query,"SELECT COUNT(DISTINCT CrsCod),"
		              "COUNT(DISTINCT GrpCod)-1,"
		              "-1,"
			      "MAX(NumLevels),"
			      "SUM(NumFolders),"
			      "SUM(NumFiles),"
			      "SUM(TotalSize)"
			      " FROM "
	                      "("
	                      "SELECT Cod AS CrsCod,"
	                      "-1 AS GrpCod,"
			      "NumLevels,"
			      "NumFolders,"
			      "NumFiles,"
			      "TotalSize"
			      " FROM file_browser_size"
			      " WHERE FileBrowser IN (%u,%u,%u,%u,%u,%u)"
	                      " UNION "
	                      "SELECT crs_grp_types.CrsCod,"
	                      "file_browser_size.Cod AS GrpCod,"
			      "file_browser_size.NumLevels,"
			      "file_browser_size.NumFolders,"
			      "file_browser_size.NumFiles,"
			      "file_browser_size.TotalSize"
			      " FROM crs_grp_types,crs_grp,file_browser_size"
			      " WHERE crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
			      " AND crs_grp.GrpCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser IN (%u,%u,%u,%u)"
			      ") AS sizes",
			(unsigned) Brw_ADMI_DOCUM_CRS,
			(unsigned) Brw_ADMI_TEACH_CRS,
			(unsigned) Brw_ADMI_SHARE_CRS,
			(unsigned) Brw_ADMI_ASSIG_USR,
			(unsigned) Brw_ADMI_WORKS_USR,
			(unsigned) Brw_ADMI_MARKS_CRS,
			(unsigned) Brw_ADMI_DOCUM_GRP,
			(unsigned) Brw_ADMI_TEACH_GRP,
			(unsigned) Brw_ADMI_SHARE_GRP,
			(unsigned) Brw_ADMI_MARKS_GRP);
	       break;
	    case Brw_ADMI_DOCUM_CRS:
	    case Brw_ADMI_TEACH_CRS:
	    case Brw_ADMI_SHARE_CRS:
	    case Brw_ADMI_MARKS_CRS:
	       sprintf (Query,"SELECT COUNT(DISTINCT Cod),"
		              "-1,"
		              "-1,"
			      "MAX(NumLevels),"
			      "SUM(NumFolders),"
			      "SUM(NumFiles),"
			      "SUM(TotalSize)"
			      " FROM file_browser_size"
			      " WHERE FileBrowser=%u",
			(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_DOCUM_GRP:
	    case Brw_ADMI_TEACH_GRP:
	    case Brw_ADMI_SHARE_GRP:
	    case Brw_ADMI_MARKS_GRP:
	       sprintf (Query,"SELECT COUNT(DISTINCT crs_grp_types.CrsCod),"
		              "COUNT(DISTINCT file_browser_size.Cod),"
		              "-1,"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM crs_grp_types,crs_grp,file_browser_size"
			      " WHERE crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
			      " AND crs_grp.GrpCod=file_browser_size.Cod"
	                      " AND file_browser_size.FileBrowser=%u",
			(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_ASSIG_USR:
	    case Brw_ADMI_WORKS_USR:
	       sprintf (Query,"SELECT COUNT(DISTINCT Cod),"
		              "-1,"
		              "COUNT(DISTINCT ZoneUsrCod),"
			      "MAX(NumLevels),"
			      "SUM(NumFolders),"
			      "SUM(NumFiles),"
			      "SUM(TotalSize)"
			      " FROM file_browser_size"
			      " WHERE FileBrowser=%u",
			(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_BRIEF_USR:
	       sprintf (Query,"SELECT -1,"
		              "-1,"
		              "COUNT(DISTINCT ZoneUsrCod),"
			      "MAX(NumLevels),"
			      "SUM(NumFolders),"
			      "SUM(NumFiles),"
			      "SUM(TotalSize)"
			      " FROM file_browser_size"
			      " WHERE FileBrowser=%u",
			(unsigned) FileBrowser);
	       break;
	    default:
	       Lay_ShowErrorAndExit ("Wrong file browser.");
	       break;
	   }
         break;
      /* Scope = the current country */
      case Sco_SCOPE_CTY:
	 switch (FileBrowser)
	   {
	    case Brw_UNKNOWN:
	       sprintf (Query,"SELECT COUNT(DISTINCT CrsCod),"
		              "COUNT(DISTINCT GrpCod)-1,"
		              "-1,"
			      "MAX(NumLevels),"
			      "SUM(NumFolders),"
			      "SUM(NumFiles),"
			      "SUM(TotalSize)"
			      " FROM "
	                      "("
	                      "SELECT file_browser_size.Cod AS CrsCod,"
	                      "-1 AS GrpCod,"				// Course zones
			      "file_browser_size.NumLevels,"
			      "file_browser_size.NumFolders,"
			      "file_browser_size.NumFiles,"
			      "file_browser_size.TotalSize"
			      " FROM institutions,centres,degrees,courses,file_browser_size"
			      " WHERE institutions.CtyCod=%ld"
			      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser IN (%u,%u,%u,%u,%u,%u)"
	                      " UNION "
	                      "SELECT crs_grp_types.CrsCod,"
	                      "file_browser_size.Cod AS GrpCod,"	// Group zones
			      "file_browser_size.NumLevels,"
			      "file_browser_size.NumFolders,"
			      "file_browser_size.NumFiles,"
			      "file_browser_size.TotalSize"
			      " FROM institutions,centres,degrees,courses,crs_grp_types,crs_grp,file_browser_size"
			      " WHERE institutions.CtyCod=%ld"
	                      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_grp_types.CrsCod"
			      " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
			      " AND crs_grp.GrpCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser IN (%u,%u,%u,%u)"
			      ") AS sizes",
			Gbl.CurrentCty.Cty.CtyCod,
			(unsigned) Brw_ADMI_DOCUM_CRS,
			(unsigned) Brw_ADMI_TEACH_CRS,
			(unsigned) Brw_ADMI_SHARE_CRS,
			(unsigned) Brw_ADMI_ASSIG_USR,
			(unsigned) Brw_ADMI_WORKS_USR,
			(unsigned) Brw_ADMI_MARKS_CRS,
			Gbl.CurrentCty.Cty.CtyCod,
			(unsigned) Brw_ADMI_DOCUM_GRP,
			(unsigned) Brw_ADMI_TEACH_GRP,
			(unsigned) Brw_ADMI_SHARE_GRP,
			(unsigned) Brw_ADMI_MARKS_GRP);
	       break;
	    case Brw_ADMI_DOCUM_CRS:
	    case Brw_ADMI_TEACH_CRS:
	    case Brw_ADMI_SHARE_CRS:
	    case Brw_ADMI_MARKS_CRS:
	       sprintf (Query,"SELECT COUNT(DISTINCT file_browser_size.Cod),"
		              "-1,"
		              "-1,"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM institutions,centres,degrees,courses,file_browser_size"
			      " WHERE institutions.CtyCod=%ld"
	                      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=file_browser_size.Cod"
			      " and file_browser_size.FileBrowser=%u",
			Gbl.CurrentCty.Cty.CtyCod,(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_DOCUM_GRP:
	    case Brw_ADMI_TEACH_GRP:
	    case Brw_ADMI_SHARE_GRP:
	    case Brw_ADMI_MARKS_GRP:
	       sprintf (Query,"SELECT COUNT(DISTINCT crs_grp_types.CrsCod),"
		              "COUNT(DISTINCT file_browser_size.Cod),"
		              "-1,"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM institutions,centres,degrees,courses,crs_grp_types,crs_grp,file_browser_size"
			      " WHERE institutions.CtyCod=%ld"
	                      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_grp_types.CrsCod"
			      " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
			      " AND crs_grp.GrpCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentCty.Cty.CtyCod,(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_ASSIG_USR:
	    case Brw_ADMI_WORKS_USR:
	       sprintf (Query,"SELECT COUNT(DISTINCT file_browser_size.Cod),"
		              "-1,"
		              "COUNT(DISTINCT file_browser_size.ZoneUsrCod),"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM institutions,centres,degrees,courses,file_browser_size"
			      " WHERE institutions.CtyCod=%ld"
	                      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=file_browser_size.Cod"
	                      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentCty.Cty.CtyCod,(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_BRIEF_USR:
	       sprintf (Query,"SELECT -1,"
		              "-1,"
		              "COUNT(DISTINCT file_browser_size.ZoneUsrCod),"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM institutions,centres,degrees,courses,crs_usr,file_browser_size"
			      " WHERE institutions.CtyCod=%ld"
	                      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=file_browser_size.ZoneUsrCod"
			      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentCty.Cty.CtyCod,(unsigned) FileBrowser);
	       break;
	    default:
	       Lay_ShowErrorAndExit ("Wrong file browser.");
	       break;
	   }
         break;
      /* Scope = the current institution */
      case Sco_SCOPE_INS:
	 switch (FileBrowser)
	   {
	    case Brw_UNKNOWN:
	       sprintf (Query,"SELECT COUNT(DISTINCT CrsCod),"
		              "COUNT(DISTINCT GrpCod)-1,"
		              "-1,"
			      "MAX(NumLevels),"
			      "SUM(NumFolders),"
			      "SUM(NumFiles),"
			      "SUM(TotalSize)"
			      " FROM "
	                      "("
	                      "SELECT file_browser_size.Cod AS CrsCod,"
	                      "-1 AS GrpCod,"				// Course zones
			      "file_browser_size.NumLevels,"
			      "file_browser_size.NumFolders,"
			      "file_browser_size.NumFiles,"
			      "file_browser_size.TotalSize"
			      " FROM centres,degrees,courses,file_browser_size"
			      " WHERE centres.InsCod=%ld"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser IN (%u,%u,%u,%u,%u,%u)"
	                      " UNION "
	                      "SELECT crs_grp_types.CrsCod,"
	                      "file_browser_size.Cod AS GrpCod,"	// Group zones
			      "file_browser_size.NumLevels,"
			      "file_browser_size.NumFolders,"
			      "file_browser_size.NumFiles,"
			      "file_browser_size.TotalSize"
			      " FROM centres,degrees,courses,crs_grp_types,crs_grp,file_browser_size"
			      " WHERE centres.InsCod=%ld"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_grp_types.CrsCod"
			      " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
			      " AND crs_grp.GrpCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser IN (%u,%u,%u,%u)"
			      ") AS sizes",
			Gbl.CurrentIns.Ins.InsCod,
			(unsigned) Brw_ADMI_DOCUM_CRS,
			(unsigned) Brw_ADMI_TEACH_CRS,
			(unsigned) Brw_ADMI_SHARE_CRS,
			(unsigned) Brw_ADMI_ASSIG_USR,
			(unsigned) Brw_ADMI_WORKS_USR,
			(unsigned) Brw_ADMI_MARKS_CRS,
			Gbl.CurrentIns.Ins.InsCod,
			(unsigned) Brw_ADMI_DOCUM_GRP,
			(unsigned) Brw_ADMI_TEACH_GRP,
			(unsigned) Brw_ADMI_SHARE_GRP,
			(unsigned) Brw_ADMI_MARKS_GRP);
	       break;
	    case Brw_ADMI_DOCUM_CRS:
	    case Brw_ADMI_TEACH_CRS:
	    case Brw_ADMI_SHARE_CRS:
	    case Brw_ADMI_MARKS_CRS:
	       sprintf (Query,"SELECT COUNT(DISTINCT file_browser_size.Cod),"
		              "-1,"
		              "-1,"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM centres,degrees,courses,file_browser_size"
			      " WHERE centres.InsCod=%ld"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=file_browser_size.Cod"
			      " and file_browser_size.FileBrowser=%u",
			Gbl.CurrentIns.Ins.InsCod,(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_DOCUM_GRP:
	    case Brw_ADMI_TEACH_GRP:
	    case Brw_ADMI_SHARE_GRP:
	    case Brw_ADMI_MARKS_GRP:
	       sprintf (Query,"SELECT COUNT(DISTINCT crs_grp_types.CrsCod),"
		              "COUNT(DISTINCT file_browser_size.Cod),"
		              "-1,"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM centres,degrees,courses,crs_grp_types,crs_grp,file_browser_size"
			      " WHERE centres.InsCod=%ld"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_grp_types.CrsCod"
			      " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
			      " AND crs_grp.GrpCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentIns.Ins.InsCod,(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_ASSIG_USR:
	    case Brw_ADMI_WORKS_USR:
	       sprintf (Query,"SELECT COUNT(DISTINCT file_browser_size.Cod),"
		              "-1,"
		              "COUNT(DISTINCT file_browser_size.ZoneUsrCod),"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM centres,degrees,courses,file_browser_size"
			      " WHERE centres.InsCod=%ld"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=file_browser_size.Cod"
	                      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentIns.Ins.InsCod,(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_BRIEF_USR:
	       sprintf (Query,"SELECT -1,"
		              "-1,"
		              "COUNT(DISTINCT file_browser_size.ZoneUsrCod),"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM centres,degrees,courses,crs_usr,file_browser_size"
			      " WHERE centres.InsCod=%ld"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=file_browser_size.ZoneUsrCod"
			      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentIns.Ins.InsCod,(unsigned) FileBrowser);
	       break;
	    default:
	       Lay_ShowErrorAndExit ("Wrong file browser.");
	       break;
	   }
         break;
      /* Scope = the current centre */
      case Sco_SCOPE_CTR:
	 switch (FileBrowser)
	   {
	    case Brw_UNKNOWN:
	       sprintf (Query,"SELECT COUNT(DISTINCT CrsCod),"
		              "COUNT(DISTINCT GrpCod)-1,"
		              "-1,"
			      "MAX(NumLevels),"
			      "SUM(NumFolders),"
			      "SUM(NumFiles),"
			      "SUM(TotalSize)"
			      " FROM "
	                      "("
	                      "SELECT file_browser_size.Cod AS CrsCod,"
	                      "-1 AS GrpCod,"				// Course zones
			      "file_browser_size.NumLevels,"
			      "file_browser_size.NumFolders,"
			      "file_browser_size.NumFiles,"
			      "file_browser_size.TotalSize"
			      " FROM degrees,courses,file_browser_size"
			      " WHERE degrees.CtrCod=%ld"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser IN (%u,%u,%u,%u,%u,%u)"
	                      " UNION "
	                      "SELECT crs_grp_types.CrsCod,"
	                      "file_browser_size.Cod AS GrpCod,"	// Group zones
			      "file_browser_size.NumLevels,"
			      "file_browser_size.NumFolders,"
			      "file_browser_size.NumFiles,"
			      "file_browser_size.TotalSize"
			      " FROM degrees,courses,crs_grp_types,crs_grp,file_browser_size"
			      " WHERE degrees.CtrCod=%ld"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_grp_types.CrsCod"
			      " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
			      " AND crs_grp.GrpCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser IN (%u,%u,%u,%u)"
			      ") AS sizes",
			Gbl.CurrentCtr.Ctr.CtrCod,
			(unsigned) Brw_ADMI_DOCUM_CRS,
			(unsigned) Brw_ADMI_TEACH_CRS,
			(unsigned) Brw_ADMI_SHARE_CRS,
			(unsigned) Brw_ADMI_ASSIG_USR,
			(unsigned) Brw_ADMI_WORKS_USR,
			(unsigned) Brw_ADMI_MARKS_CRS,
			Gbl.CurrentCtr.Ctr.CtrCod,
			(unsigned) Brw_ADMI_DOCUM_GRP,
			(unsigned) Brw_ADMI_TEACH_GRP,
			(unsigned) Brw_ADMI_SHARE_GRP,
			(unsigned) Brw_ADMI_MARKS_GRP);
	       break;
	    case Brw_ADMI_DOCUM_CRS:
	    case Brw_ADMI_TEACH_CRS:
	    case Brw_ADMI_SHARE_CRS:
	    case Brw_ADMI_MARKS_CRS:
	       sprintf (Query,"SELECT COUNT(DISTINCT file_browser_size.Cod),"
		              "-1,"
		              "-1,"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM degrees,courses,file_browser_size"
			      " WHERE degrees.CtrCod=%ld"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentCtr.Ctr.CtrCod,(unsigned) FileBrowser);
               break;
	    case Brw_ADMI_DOCUM_GRP:
	    case Brw_ADMI_TEACH_GRP:
	    case Brw_ADMI_SHARE_GRP:
	    case Brw_ADMI_MARKS_GRP:
	       sprintf (Query,"SELECT COUNT(DISTINCT crs_grp_types.CrsCod),"
		              "COUNT(DISTINCT file_browser_size.Cod),"
		              "-1,"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM degrees,courses,crs_grp_types,crs_grp,file_browser_size"
			      " WHERE degrees.CtrCod=%ld"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_grp_types.CrsCod"
			      " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
			      " AND crs_grp.GrpCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentCtr.Ctr.CtrCod,(unsigned) FileBrowser);
               break;
	    case Brw_ADMI_ASSIG_USR:
	    case Brw_ADMI_WORKS_USR:
	       sprintf (Query,"SELECT COUNT(DISTINCT file_browser_size.Cod),"
		              "-1,"
		              "COUNT(DISTINCT file_browser_size.ZoneUsrCod),"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM degrees,courses,file_browser_size"
			      " WHERE degrees.CtrCod=%ld"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentCtr.Ctr.CtrCod,(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_BRIEF_USR:
	       sprintf (Query,"SELECT -1,"
		              "-1,"
		              "COUNT(DISTINCT file_browser_size.ZoneUsrCod),"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM degrees,courses,crs_usr,file_browser_size"
			      " WHERE degrees.CtrCod=%ld"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=file_browser_size.ZoneUsrCod"
			      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentCtr.Ctr.CtrCod,(unsigned) FileBrowser);
	       break;
	    default:
	       Lay_ShowErrorAndExit ("Wrong file browser.");
	       break;
	   }
         break;
      /* Scope = the current degree */
      case Sco_SCOPE_DEG:
	 switch (FileBrowser)
	   {
	    case Brw_UNKNOWN:
	       sprintf (Query,"SELECT COUNT(DISTINCT CrsCod),"
		              "COUNT(DISTINCT GrpCod)-1,"
		              "-1,"
			      "MAX(NumLevels),"
			      "SUM(NumFolders),"
			      "SUM(NumFiles),"
			      "SUM(TotalSize)"
			      " FROM "
	                      "("
	                      "SELECT file_browser_size.Cod AS CrsCod,"
	                      "-1 AS GrpCod,"				// Course zones
			      "file_browser_size.NumLevels,"
			      "file_browser_size.NumFolders,"
			      "file_browser_size.NumFiles,"
			      "file_browser_size.TotalSize"
			      " FROM courses,file_browser_size"
			      " WHERE courses.DegCod=%ld"
			      " AND courses.CrsCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser IN (%u,%u,%u,%u,%u,%u)"
	                      " UNION "
	                      "SELECT crs_grp_types.CrsCod,"
	                      "file_browser_size.Cod AS GrpCod,"	// Group zones
			      "file_browser_size.NumLevels,"
			      "file_browser_size.NumFolders,"
			      "file_browser_size.NumFiles,"
			      "file_browser_size.TotalSize"
			      " FROM courses,crs_grp_types,crs_grp,file_browser_size"
			      " WHERE courses.DegCod=%ld"
			      " AND courses.CrsCod=crs_grp_types.CrsCod"
			      " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
			      " AND crs_grp.GrpCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser IN (%u,%u,%u,%u)"
			      ") AS sizes",
			Gbl.CurrentDeg.Deg.DegCod,
			(unsigned) Brw_ADMI_DOCUM_CRS,
			(unsigned) Brw_ADMI_TEACH_CRS,
			(unsigned) Brw_ADMI_SHARE_CRS,
			(unsigned) Brw_ADMI_ASSIG_USR,
			(unsigned) Brw_ADMI_WORKS_USR,
			(unsigned) Brw_ADMI_MARKS_CRS,
			Gbl.CurrentDeg.Deg.DegCod,
			(unsigned) Brw_ADMI_DOCUM_GRP,
			(unsigned) Brw_ADMI_TEACH_GRP,
			(unsigned) Brw_ADMI_SHARE_GRP,
			(unsigned) Brw_ADMI_MARKS_GRP);
	       break;
	    case Brw_ADMI_DOCUM_CRS:
	    case Brw_ADMI_TEACH_CRS:
	    case Brw_ADMI_SHARE_CRS:
	    case Brw_ADMI_MARKS_CRS:
	       sprintf (Query,"SELECT COUNT(DISTINCT file_browser_size.Cod),"
		              "-1,"
		              "-1,"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM courses,file_browser_size"
			      " WHERE courses.DegCod=%ld"
			      " AND courses.CrsCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentDeg.Deg.DegCod,(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_DOCUM_GRP:
	    case Brw_ADMI_TEACH_GRP:
	    case Brw_ADMI_SHARE_GRP:
	    case Brw_ADMI_MARKS_GRP:
	       sprintf (Query,"SELECT COUNT(DISTINCT crs_grp_types.CrsCod),"
		              "COUNT(DISTINCT file_browser_size.Cod),"
		              "-1,"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM courses,crs_grp_types,crs_grp,file_browser_size"
			      " WHERE courses.DegCod=%ld"
			      " AND courses.CrsCod=crs_grp_types.CrsCod"
			      " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
			      " AND crs_grp.GrpCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentDeg.Deg.DegCod,(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_ASSIG_USR:
	    case Brw_ADMI_WORKS_USR:
	       sprintf (Query,"SELECT COUNT(DISTINCT file_browser_size.Cod),"
		              "-1,"
		              "COUNT(DISTINCT file_browser_size.ZoneUsrCod),"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM courses,file_browser_size"
			      " WHERE courses.DegCod=%ld"
			      " AND courses.CrsCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentDeg.Deg.DegCod,(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_BRIEF_USR:
	       sprintf (Query,"SELECT -1,"
		              "-1,"
		              "COUNT(DISTINCT file_browser_size.ZoneUsrCod),"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM courses,crs_usr,file_browser_size"
			      " WHERE courses.DegCod=%ld"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=file_browser_size.ZoneUsrCod"
			      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentDeg.Deg.DegCod,(unsigned) FileBrowser);
	       break;
	    default:
	       Lay_ShowErrorAndExit ("Wrong file browser.");
	       break;
	   }
         break;
      /* Scope = the current course */
      case Sco_SCOPE_CRS:
	 switch (FileBrowser)
	   {
	    case Brw_UNKNOWN:
	       sprintf (Query,"SELECT COUNT(DISTINCT CrsCod),"
		              "COUNT(DISTINCT GrpCod)-1,"
		              "-1,"
			      "MAX(NumLevels),"
			      "SUM(NumFolders),"
			      "SUM(NumFiles),"
			      "SUM(TotalSize)"
			      " FROM "
	                      "("
	                      "SELECT Cod AS CrsCod,"
	                      "-1 AS GrpCod,"				// Course zones
			      "NumLevels,"
			      "NumFolders,"
			      "NumFiles,"
			      "TotalSize"
			      " FROM file_browser_size"
			      " WHERE Cod=%ld"
			      " AND FileBrowser IN (%u,%u,%u,%u,%u,%u)"
	                      " UNION "
	                      "SELECT crs_grp_types.CrsCod,"
	                      "file_browser_size.Cod AS GrpCod,"	// Group zones
			      "file_browser_size.NumLevels,"
			      "file_browser_size.NumFolders,"
			      "file_browser_size.NumFiles,"
			      "file_browser_size.TotalSize"
			      " FROM crs_grp_types,crs_grp,file_browser_size"
			      " WHERE crs_grp_types.CrsCod=%ld"
			      " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
			      " AND crs_grp.GrpCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser IN (%u,%u,%u,%u)"
			      ") AS sizes",
			Gbl.CurrentCrs.Crs.CrsCod,
			(unsigned) Brw_ADMI_DOCUM_CRS,
			(unsigned) Brw_ADMI_TEACH_CRS,
			(unsigned) Brw_ADMI_SHARE_CRS,
			(unsigned) Brw_ADMI_ASSIG_USR,
			(unsigned) Brw_ADMI_WORKS_USR,
			(unsigned) Brw_ADMI_MARKS_CRS,
			Gbl.CurrentCrs.Crs.CrsCod,
			(unsigned) Brw_ADMI_DOCUM_GRP,
			(unsigned) Brw_ADMI_TEACH_GRP,
			(unsigned) Brw_ADMI_SHARE_GRP,
			(unsigned) Brw_ADMI_MARKS_GRP);
	       break;
	    case Brw_ADMI_DOCUM_CRS:
	    case Brw_ADMI_TEACH_CRS:
	    case Brw_ADMI_SHARE_CRS:
	    case Brw_ADMI_MARKS_CRS:
	       sprintf (Query,"SELECT 1,"
		              "-1,"
		              "-1,"
			      "MAX(NumLevels),"
			      "SUM(NumFolders),"
			      "SUM(NumFiles),"
			      "SUM(TotalSize)"
			      " FROM file_browser_size"
			      " WHERE Cod=%ld AND FileBrowser=%u",
			Gbl.CurrentCrs.Crs.CrsCod,(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_DOCUM_GRP:
	    case Brw_ADMI_TEACH_GRP:
	    case Brw_ADMI_SHARE_GRP:
	    case Brw_ADMI_MARKS_GRP:
	       sprintf (Query,"SELECT COUNT(DISTINCT crs_grp_types.CrsCod),"
		              "COUNT(DISTINCT file_browser_size.Cod),"
		              "-1,"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM crs_grp_types,crs_grp,file_browser_size"
			      " WHERE crs_grp_types.CrsCod=%ld"
			      " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
			      " AND crs_grp.GrpCod=file_browser_size.Cod"
			      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentCrs.Crs.CrsCod,(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_ASSIG_USR:
	    case Brw_ADMI_WORKS_USR:
	       sprintf (Query,"SELECT 1,"
		              "-1,"
		              "COUNT(DISTINCT ZoneUsrCod),"
			      "MAX(NumLevels),"
			      "SUM(NumFolders),"
			      "SUM(NumFiles),"
			      "SUM(TotalSize)"
			      " FROM file_browser_size"
			      " WHERE Cod=%ld AND FileBrowser=%u",
			Gbl.CurrentCrs.Crs.CrsCod,(unsigned) FileBrowser);
	       break;
	    case Brw_ADMI_BRIEF_USR:
	       sprintf (Query,"SELECT -1,"
		              "-1,"
		              "COUNT(DISTINCT file_browser_size.ZoneUsrCod),"
			      "MAX(file_browser_size.NumLevels),"
			      "SUM(file_browser_size.NumFolders),"
			      "SUM(file_browser_size.NumFiles),"
			      "SUM(file_browser_size.TotalSize)"
			      " FROM crs_usr,file_browser_size"
			      " WHERE crs_usr.CrsCod=%ld"
			      " AND crs_usr.UsrCod=file_browser_size.ZoneUsrCod"
			      " AND file_browser_size.FileBrowser=%u",
			Gbl.CurrentCrs.Crs.CrsCod,(unsigned) FileBrowser);
	       break;
	    default:
	       Lay_ShowErrorAndExit ("Wrong file browser.");
	       break;
	   }
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get size of a file browser");

   /* Get row */
   row = mysql_fetch_row (mysql_res);

   /* Reset default values to zero */
   SizeOfFileZones->NumCrss = SizeOfFileZones->NumUsrs = 0;
   SizeOfFileZones->MaxLevels = 0;
   SizeOfFileZones->NumFolders = SizeOfFileZones->NumFiles = 0;
   SizeOfFileZones->Size = 0;

   /* Get number of courses (row[0]) */
   if (row[0])
      if (sscanf (row[0],"%d",&(SizeOfFileZones->NumCrss)) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of courses.");

   /* Get number of groups (row[1]) */
   if (row[1])
      if (sscanf (row[1],"%d",&(SizeOfFileZones->NumGrps)) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of groups.");

   /* Get number of users (row[2]) */
   if (row[2])
      if (sscanf (row[2],"%d",&(SizeOfFileZones->NumUsrs)) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of users.");

   /* Get maximum number of levels (row[3]) */
   if (row[3])
      if (sscanf (row[3],"%u",&(SizeOfFileZones->MaxLevels)) != 1)
         Lay_ShowErrorAndExit ("Error when getting maximum number of levels.");

   /* Get number of folders (row[4]) */
   if (row[4])
      if (sscanf (row[4],"%lu",&(SizeOfFileZones->NumFolders)) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of folders.");

   /* Get number of files (row[5]) */
   if (row[5])
      if (sscanf (row[5],"%lu",&(SizeOfFileZones->NumFiles)) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of files.");

   /* Get total size (row[6]) */
   if (row[6])
      if (sscanf (row[6],"%llu",&(SizeOfFileZones->Size)) != 1)
         Lay_ShowErrorAndExit ("Error when getting toal size.");

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Show stats about Open Educational Resources (OERs) *************/
/*****************************************************************************/

static void Sta_GetAndShowOERsStats (void)
  {
   extern const char *Hlp_STATS_Figures_open_educational_resources_oer;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_License;
   extern const char *Txt_No_of_private_files;
   extern const char *Txt_No_of_public_files;
   extern const char *Txt_LICENSES[Brw_NUM_LICENSES];
   Brw_License_t License;
   unsigned long NumFiles[2];

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_OER],
                             NULL,Hlp_STATS_Figures_open_educational_resources_oer,2);

   /***** Write table heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_License,
            Txt_No_of_private_files,
            Txt_No_of_public_files);

   for (License = 0;
	License < Brw_NUM_LICENSES;
	License++)
     {
      Sta_GetNumberOfOERsFromDB (Gbl.Scope.Current,License,NumFiles);

      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"DAT LEFT_MIDDLE\">"
                         "%s"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%lu"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%lu"
                         "</td>"
                         "</tr>",
               Txt_LICENSES[License],
               NumFiles[0],
               NumFiles[1]);
     }

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/**************** Get the size of a file zone from database ******************/
/*****************************************************************************/

static void Sta_GetNumberOfOERsFromDB (Sco_Scope_t Scope,Brw_License_t License,unsigned long NumFiles[2])
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows,NumRow;
   unsigned Public;

   /***** Get the size of a file browser *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT Public,COUNT(*)"
                        " FROM files"
                        " WHERE License=%u"
                        " GROUP BY Public",
                  (unsigned) License);
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT files.Public,COUNT(*)"
                        " FROM institutions,centres,degrees,courses,files"
                        " WHERE institutions.CtyCod=%ld"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=files.Cod"
	                " AND files.FileBrowser IN (%u,%u)"
                        " AND files.License=%u"
                        " GROUP BY files.Public",
                  Gbl.CurrentCty.Cty.CtyCod,
                  (unsigned) Brw_ADMI_DOCUM_CRS,
		  (unsigned) Brw_ADMI_SHARE_CRS,
                  (unsigned) License);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT files.Public,COUNT(*)"
                        " FROM centres,degrees,courses,files"
                        " WHERE centres.InsCod=%ld"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=files.Cod"
	                " AND files.FileBrowser IN (%u,%u)"
                        " AND files.License=%u"
                        " GROUP BY files.Public",
                  Gbl.CurrentIns.Ins.InsCod,
                  (unsigned) Brw_ADMI_DOCUM_CRS,
		  (unsigned) Brw_ADMI_SHARE_CRS,
                  (unsigned) License);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT files.Public,COUNT(*)"
                        " FROM degrees,courses,files"
                        " WHERE degrees.CtrCod=%ld"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=files.Cod"
	                " AND files.FileBrowser IN (%u,%u)"
                        " AND files.License=%u"
                        " GROUP BY files.Public",
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  (unsigned) Brw_ADMI_DOCUM_CRS,
		  (unsigned) Brw_ADMI_SHARE_CRS,
                  (unsigned) License);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT files.Public,COUNT(*)"
                        " FROM courses,files"
                        " WHERE courses.DegCod=%ld"
                        " AND courses.CrsCod=files.Cod"
	                " AND files.FileBrowser IN (%u,%u)"
                        " AND files.License=%u"
                        " GROUP BY files.Public",
                  Gbl.CurrentDeg.Deg.DegCod,
                  (unsigned) Brw_ADMI_DOCUM_CRS,
		  (unsigned) Brw_ADMI_SHARE_CRS,
                  (unsigned) License);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT Public,COUNT(*)"
                        " FROM files"
                        " WHERE Cod=%ld"
	                " AND FileBrowser IN (%u,%u)"
                        " AND License=%u"
                        " GROUP BY Public",
                  Gbl.CurrentCrs.Crs.CrsCod,
                  (unsigned) Brw_ADMI_DOCUM_CRS,
		  (unsigned) Brw_ADMI_SHARE_CRS,
                  (unsigned) License);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   NumRows = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get number of OERs");

   /* Reset values to zero */
   NumFiles[0] = NumFiles[1] = 0L;

   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get if public (row[0]) */
      Public = (row[0][0] == 'Y') ? 1 :
	                            0;

      /* Get number of files (row[1]) */
      if (sscanf (row[1],"%lu",&NumFiles[Public]) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of files.");
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Show stats about assignments ***********************/
/*****************************************************************************/

static void Sta_GetAndShowAssignmentsStats (void)
  {
   extern const char *Hlp_STATS_Figures_assignments;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Number_of_BR_assignments;
   extern const char *Txt_Number_of_BR_courses_with_BR_assignments;
   extern const char *Txt_Average_number_BR_of_ASSIG_BR_per_course;
   extern const char *Txt_Number_of_BR_notifications;
   unsigned NumAssignments;
   unsigned NumNotif;
   unsigned NumCoursesWithAssignments = 0;
   float NumAssignmentsPerCourse = 0.0;

   /***** Get the number of assignments from this location
          (all the platform, current degree or current course) *****/
   if ((NumAssignments = Asg_GetNumAssignments (Gbl.Scope.Current,&NumNotif)))
      if ((NumCoursesWithAssignments = Asg_GetNumCoursesWithAssignments (Gbl.Scope.Current)) != 0)
         NumAssignmentsPerCourse = (float) NumAssignments / (float) NumCoursesWithAssignments;

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_ASSIGNMENTS],
                             NULL,Hlp_STATS_Figures_assignments,2);

   /***** Write table heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Number_of_BR_assignments,
            Txt_Number_of_BR_courses_with_BR_assignments,
            Txt_Average_number_BR_of_ASSIG_BR_per_course,
            Txt_Number_of_BR_notifications);

   /***** Write number of assignments *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%.2f"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "</tr>",
            NumAssignments,
            NumCoursesWithAssignments,
            NumAssignmentsPerCourse,
            NumNotif);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/********************** Show stats about test questions **********************/
/*****************************************************************************/

static void Sta_GetAndShowTestsStats (void)
  {
   extern const char *Hlp_STATS_Figures_tests;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Type_of_BR_answers;
   extern const char *Txt_Number_of_BR_courses_BR_with_test_BR_questions;
   extern const char *Txt_Number_of_BR_courses_with_BR_exportable_BR_test_BR_questions;
   extern const char *Txt_Number_BR_of_test_BR_questions;
   extern const char *Txt_Average_BR_number_BR_of_test_BR_questions_BR_per_course;
   extern const char *Txt_Number_of_BR_times_that_BR_questions_BR_have_been_BR_responded;
   extern const char *Txt_Average_BR_number_of_BR_times_that_BR_questions_BR_have_been_BR_responded_BR_per_course;
   extern const char *Txt_Average_BR_number_of_BR_times_that_BR_a_question_BR_has_been_BR_responded;
   extern const char *Txt_Average_BR_score_BR_per_question_BR_from_0_to_1;
   extern const char *Txt_TST_STR_ANSWER_TYPES[Tst_NUM_ANS_TYPES];
   extern const char *Txt_Total;
   Tst_AnswerType_t AnsType;
   struct Tst_Stats Stats;

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_TESTS],
                             NULL,Hlp_STATS_Figures_tests,2);

   /***** Write table heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Type_of_BR_answers,
            Txt_Number_of_BR_courses_BR_with_test_BR_questions,
            Txt_Number_of_BR_courses_with_BR_exportable_BR_test_BR_questions,
            Txt_Number_BR_of_test_BR_questions,
            Txt_Average_BR_number_BR_of_test_BR_questions_BR_per_course,
            Txt_Number_of_BR_times_that_BR_questions_BR_have_been_BR_responded,
            Txt_Average_BR_number_of_BR_times_that_BR_questions_BR_have_been_BR_responded_BR_per_course,
            Txt_Average_BR_number_of_BR_times_that_BR_a_question_BR_has_been_BR_responded,
            Txt_Average_BR_score_BR_per_question_BR_from_0_to_1);

   for (AnsType = (Tst_AnswerType_t) 0;
	AnsType < Tst_NUM_ANS_TYPES;
	AnsType++)
     {
      /***** Get the stats about test questions from this location
             (all the platform, current degree or current course) *****/
      Tst_GetTestStats (AnsType,&Stats);

      /***** Write number of assignments *****/
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"DAT LEFT_MIDDLE\">"
                         "%s"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%u (%.1f%%)"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%.2f"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%lu"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%.2f"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%.2f"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%.2f"
                         "</td>"
                         "</tr>",
               Txt_TST_STR_ANSWER_TYPES[AnsType],
               Stats.NumCoursesWithQuestions,
               Stats.NumCoursesWithPluggableQuestions,
               Stats.NumCoursesWithQuestions ? (float) Stats.NumCoursesWithPluggableQuestions * 100.0 /
        	                               (float) Stats.NumCoursesWithQuestions :
        	                               0.0,
               Stats.NumQsts,
               Stats.AvgQstsPerCourse,
               Stats.NumHits,
               Stats.AvgHitsPerCourse,
               Stats.AvgHitsPerQuestion,
               Stats.AvgScorePerQuestion);
     }

   /***** Get the stats about test questions from this location
          (all the platform, current degree or current course) *****/
   Tst_GetTestStats (Tst_ANS_ALL,&Stats);

   /***** Write number of assignments *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"DAT_N_LINE_TOP LEFT_MIDDLE\">"
                      "%s"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%u (%.1f%%)"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%.2f"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%lu"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%.2f"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%.2f"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%.2f"
                      "</td>"
                      "</tr>",
            Txt_Total,
            Stats.NumCoursesWithQuestions,
            Stats.NumCoursesWithPluggableQuestions,
            Stats.NumCoursesWithQuestions ? (float) Stats.NumCoursesWithPluggableQuestions * 100.0 /
        	                            (float) Stats.NumCoursesWithQuestions :
        	                            0.0,
            Stats.NumQsts,
            Stats.AvgQstsPerCourse,
            Stats.NumHits,
            Stats.AvgHitsPerCourse,
            Stats.AvgHitsPerQuestion,
            Stats.AvgScorePerQuestion);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/******************** Get and show number of social notes ********************/
/*****************************************************************************/

static void Sta_GetAndShowSocialActivityStats (void)
  {
   extern const char *Hlp_STATS_Figures_timeline;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Type;
   extern const char *Txt_No_of_social_posts;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Txt_No_of_posts_BR_per_user;
   extern const char *Txt_SOCIAL_NOTE[Soc_NUM_NOTE_TYPES];
   extern const char *Txt_Total;
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Soc_NoteType_t NoteType;
   unsigned long NumSocialNotes;
   unsigned NumUsrs;
   unsigned NumUsrsTotal;

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_SOCIAL_ACTIVITY],
                             NULL,Hlp_STATS_Figures_timeline,2);

   /***** Heading row *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Type,
            Txt_No_of_social_posts,
            Txt_No_of_users,
            Txt_PERCENT_of_users,
            Txt_No_of_posts_BR_per_user);

   /***** Get total number of users *****/
   NumUsrsTotal = (Gbl.Scope.Current == Sco_SCOPE_SYS) ? Usr_GetTotalNumberOfUsersInPlatform () :
                                                         Usr_GetTotalNumberOfUsersInCourses (Gbl.Scope.Current,
                                                                                             1 << Rol_STD |
                                                                                             1 << Rol_NET |
                                                                                             1 << Rol_TCH);

   /***** Get total number of following/followers from database *****/
   for (NoteType = (Soc_NoteType_t) 0;
	NoteType < Soc_NUM_NOTE_TYPES;
	NoteType++)
     {
      switch (Gbl.Scope.Current)
	{
	 case Sco_SCOPE_SYS:
	    sprintf (Query,"SELECT COUNT(*),COUNT(DISTINCT UsrCod)"
		           " FROM social_notes WHERE NoteType=%u",
	             NoteType);
	    break;
	 case Sco_SCOPE_CTY:
	    sprintf (Query,"SELECT COUNT(DISTINCT social_notes.NotCod),COUNT(DISTINCT social_notes.UsrCod)"
			   " FROM institutions,centres,degrees,courses,crs_usr,social_notes"
			   " WHERE institutions.CtyCod=%ld"
			   " AND institutions.InsCod=centres.InsCod"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=social_notes.UsrCod"
			   " AND social_notes.NoteType=%u",
		     Gbl.CurrentCty.Cty.CtyCod,
		     (unsigned) NoteType);
	    break;
	 case Sco_SCOPE_INS:
	    sprintf (Query,"SELECT COUNT(DISTINCT social_notes.NotCod),COUNT(DISTINCT social_notes.UsrCod)"
			   " FROM centres,degrees,courses,crs_usr,social_notes"
			   " WHERE centres.InsCod=%ld"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=social_notes.UsrCod"
			   " AND social_notes.NoteType=%u",
		     Gbl.CurrentIns.Ins.InsCod,
		     (unsigned) NoteType);
	    break;
	 case Sco_SCOPE_CTR:
	    sprintf (Query,"SELECT COUNT(DISTINCT social_notes.NotCod),COUNT(DISTINCT social_notes.UsrCod)"
			   " FROM degrees,courses,crs_usr,social_notes"
			   " WHERE degrees.CtrCod=%ld"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=social_notes.UsrCod"
			   " AND social_notes.NoteType=%u",
		     Gbl.CurrentCtr.Ctr.CtrCod,
		     (unsigned) NoteType);
	    break;
	 case Sco_SCOPE_DEG:
	    sprintf (Query,"SELECT COUNT(DISTINCT social_notes.NotCod),COUNT(DISTINCT social_notes.UsrCod)"
			   " FROM courses,crs_usr,social_notes"
			   " WHERE courses.DegCod=%ld"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=social_notes.UsrCod"
			   " AND social_notes.NoteType=%u",
		     Gbl.CurrentDeg.Deg.DegCod,
		     (unsigned) NoteType);
	    break;
	 case Sco_SCOPE_CRS:
	    sprintf (Query,"SELECT COUNT(DISTINCT social_notes.NotCod),COUNT(DISTINCT social_notes.UsrCod)"
			   " FROM crs_usr,social_notes"
			   " WHERE crs_usr.CrsCod=%ld"
			   " AND crs_usr.UsrCod=social_notes.UsrCod"
			   " AND social_notes.NoteType=%u",
		     Gbl.CurrentCrs.Crs.CrsCod,
		     (unsigned) NoteType);
	    break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
	}
      NumSocialNotes = 0;
      NumUsrs = 0;
      if (DB_QuerySELECT (Query,&mysql_res,"can not get number of social notes"))
	{
	 /***** Get number of social notes and number of users *****/
	 row = mysql_fetch_row (mysql_res);

	 /* Get number of social notes */
	 if (row[0])
	    if (sscanf (row[0],"%lu",&NumSocialNotes) != 1)
	       NumSocialNotes = 0;

	 /* Get number of users */
	 if (row[1])
	    if (sscanf (row[1],"%u",&NumUsrs) != 1)
	       NumUsrs = 0;
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Write number of social notes and number of users *****/
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"DAT LEFT_MIDDLE\">"
                         "%s"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%lu"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%5.2f%%"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%.2f"
                         "</td>"
                         "</tr>",
               Txt_SOCIAL_NOTE[NoteType],
               NumSocialNotes,
               NumUsrs,
               NumUsrsTotal ? (float) NumUsrs * 100.0 / (float) NumUsrsTotal :
        	              0.0,
               NumUsrs ? (float) NumSocialNotes / (float) NumUsrs :
        	         0.0);
     }

   /***** Get and write totals *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 sprintf (Query,"SELECT COUNT(*),COUNT(DISTINCT UsrCod)"
			" FROM social_notes");
	 break;
      case Sco_SCOPE_CTY:
	 sprintf (Query,"SELECT COUNT(DISTINCT social_notes.NotCod),COUNT(DISTINCT social_notes.UsrCod)"
			" FROM institutions,centres,degrees,courses,crs_usr,social_notes"
			" WHERE institutions.CtyCod=%ld"
			" AND institutions.InsCod=centres.InsCod"
			" AND centres.CtrCod=degrees.CtrCod"
			" AND degrees.DegCod=courses.DegCod"
			" AND courses.CrsCod=crs_usr.CrsCod"
			" AND crs_usr.UsrCod=social_notes.UsrCod",
		  Gbl.CurrentCty.Cty.CtyCod);
	 break;
      case Sco_SCOPE_INS:
	 sprintf (Query,"SELECT COUNT(DISTINCT social_notes.NotCod),COUNT(DISTINCT social_notes.UsrCod)"
			" FROM centres,degrees,courses,crs_usr,social_notes"
			" WHERE centres.InsCod=%ld"
			" AND centres.CtrCod=degrees.CtrCod"
			" AND degrees.DegCod=courses.DegCod"
			" AND courses.CrsCod=crs_usr.CrsCod"
			" AND crs_usr.UsrCod=social_notes.UsrCod",
		  Gbl.CurrentIns.Ins.InsCod);
	 break;
      case Sco_SCOPE_CTR:
	 sprintf (Query,"SELECT COUNT(DISTINCT social_notes.NotCod),COUNT(DISTINCT social_notes.UsrCod)"
			" FROM degrees,courses,crs_usr,social_notes"
			" WHERE degrees.CtrCod=%ld"
			" AND degrees.DegCod=courses.DegCod"
			" AND courses.CrsCod=crs_usr.CrsCod"
			" AND crs_usr.UsrCod=social_notes.UsrCod",
		  Gbl.CurrentCtr.Ctr.CtrCod);
	 break;
      case Sco_SCOPE_DEG:
	 sprintf (Query,"SELECT COUNT(DISTINCT social_notes.NotCod),COUNT(DISTINCT social_notes.UsrCod)"
			" FROM courses,crs_usr,social_notes"
			" WHERE courses.DegCod=%ld"
			" AND courses.CrsCod=crs_usr.CrsCod"
			" AND crs_usr.UsrCod=social_notes.UsrCod",
		  Gbl.CurrentDeg.Deg.DegCod);
	 break;
      case Sco_SCOPE_CRS:
	 sprintf (Query,"SELECT COUNT(DISTINCT social_notes.NotCod),COUNT(DISTINCT social_notes.UsrCod)"
			" FROM crs_usr,social_notes"
			" WHERE crs_usr.CrsCod=%ld"
			" AND crs_usr.UsrCod=social_notes.UsrCod",
		  Gbl.CurrentCrs.Crs.CrsCod);
	 break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   NumSocialNotes = 0;
   NumUsrs = 0;
   if (DB_QuerySELECT (Query,&mysql_res,"can not get number of social notes"))
     {
      /* Get number of social notes and number of users */
      row = mysql_fetch_row (mysql_res);

      /* Get number of social notes */
      if (row[0])
	 if (sscanf (row[0],"%lu",&NumSocialNotes) != 1)
	    NumSocialNotes = 0;

      /* Get number of users */
      if (row[1])
	 if (sscanf (row[1],"%u",&NumUsrs) != 1)
	    NumUsrs = 0;
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /* Write totals */
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"DAT_N_LINE_TOP LEFT_MIDDLE\">"
		      "%s"
		      "</td>"
		      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
		      "%lu"
		      "</td>"
		      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
		      "%u"
		      "</td>"
		      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
		      "%5.2f%%"
		      "</td>"
		      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
		      "%.2f"
		      "</td>"
		      "</tr>",
	    Txt_Total,
	    NumSocialNotes,
	    NumUsrs,
	    NumUsrsTotal ? (float) NumUsrs * 100.0 / (float) NumUsrsTotal :
			   0.0,
	    NumUsrs ? (float) NumSocialNotes / (float) NumUsrs :
		      0.0);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/************** Get and show number of following and followers ***************/
/*****************************************************************************/

static void Sta_GetAndShowFollowStats (void)
  {
   extern const char *Hlp_STATS_Figures_followed_followers;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Users;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Txt_Followed;
   extern const char *Txt_Followers;
   extern const char *Txt_FollowPerFollow[2];
   const char *FieldDB[2] =
     {
      "FollowedCod",
      "FollowerCod"
     };
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned Fol;
   unsigned NumUsrsTotal;
   unsigned NumUsrs;
   float Average;

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_FOLLOW],
                             NULL,Hlp_STATS_Figures_followed_followers,2);

   /***** Heading row *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Users,
            Txt_No_of_users,
            Txt_PERCENT_of_users);

   /***** Get total number of users *****/
   NumUsrsTotal = (Gbl.Scope.Current == Sco_SCOPE_SYS) ? Usr_GetTotalNumberOfUsersInPlatform () :
                                                         Usr_GetTotalNumberOfUsersInCourses (Gbl.Scope.Current,
                                                                                             1 << Rol_STD |
                                                                                             1 << Rol_NET |
                                                                                             1 << Rol_TCH);

   /***** Get total number of following/followers from database *****/
   for (Fol = 0;
	Fol < 2;
	Fol++)
     {
      switch (Gbl.Scope.Current)
	{
	 case Sco_SCOPE_SYS:
	    sprintf (Query,"SELECT COUNT(DISTINCT %s) FROM usr_follow",
	             FieldDB[Fol]);
	    break;
	 case Sco_SCOPE_CTY:
	    sprintf (Query,"SELECT COUNT(DISTINCT usr_follow.%s)"
			   " FROM institutions,centres,degrees,courses,crs_usr,usr_follow"
			   " WHERE institutions.CtyCod=%ld"
			   " AND institutions.InsCod=centres.InsCod"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=usr_follow.%s",
	             FieldDB[Fol],
		     Gbl.CurrentCty.Cty.CtyCod,
		     FieldDB[Fol]);
	    break;
	 case Sco_SCOPE_INS:
	    sprintf (Query,"SELECT COUNT(DISTINCT usr_follow.%s)"
			   " FROM centres,degrees,courses,crs_usr,usr_follow"
			   " WHERE centres.InsCod=%ld"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=usr_follow.%s",
	             FieldDB[Fol],
		     Gbl.CurrentIns.Ins.InsCod,
		     FieldDB[Fol]);
	    break;
	 case Sco_SCOPE_CTR:
	    sprintf (Query,"SELECT COUNT(DISTINCT usr_follow.%s)"
			   " FROM degrees,courses,crs_usr,usr_follow"
			   " WHERE degrees.CtrCod=%ld"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=usr_follow.%s",
	             FieldDB[Fol],
		     Gbl.CurrentCtr.Ctr.CtrCod,
		     FieldDB[Fol]);
	    break;
	 case Sco_SCOPE_DEG:
	    sprintf (Query,"SELECT COUNT(DISTINCT usr_follow.%s)"
			   " FROM courses,crs_usr,usr_follow"
			   " WHERE courses.DegCod=%ld"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=usr_follow.%s",
	             FieldDB[Fol],
		     Gbl.CurrentDeg.Deg.DegCod,
		     FieldDB[Fol]);
	    break;
	 case Sco_SCOPE_CRS:
	    sprintf (Query,"SELECT COUNT(DISTINCT usr_follow.%s)"
			   " FROM crs_usr,usr_follow"
			   " WHERE crs_usr.CrsCod=%ld"
			   " AND crs_usr.UsrCod=usr_follow.%s",
	             FieldDB[Fol],
		     Gbl.CurrentCrs.Crs.CrsCod,
		     FieldDB[Fol]);
	    break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
	}
      NumUsrs = (unsigned) DB_QueryCOUNT (Query,"can not get the total number of following/followers");

      /***** Write number of followed / followers *****/
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"DAT LEFT_MIDDLE\">"
                         "%s"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%5.2f%%"
                         "</td>"
                         "</tr>",
               Fol == 0 ? Txt_Followed :
        	          Txt_Followers,
               NumUsrs,
               NumUsrsTotal ? (float) NumUsrs * 100.0 /
        	              (float) NumUsrsTotal :
        	              0.0);
     }

   /***** Write number of followed/followers per follower/followed *****/
   for (Fol = 0;
	Fol < 2;
	Fol++)
     {
      switch (Gbl.Scope.Current)
	{
	 case Sco_SCOPE_SYS:
	    sprintf (Query,"SELECT AVG(N) FROM "
			   "(SELECT COUNT(%s) AS N"
			   " FROM usr_follow"
			   " GROUP BY %s) AS F",
		     FieldDB[Fol],
		     FieldDB[1 - Fol]);
	    break;
	 case Sco_SCOPE_CTY:
	    sprintf (Query,"SELECT AVG(N) FROM "
			   "(SELECT COUNT(DISTINCT usr_follow.%s) AS N"
			   " FROM institutions,centres,degrees,courses,crs_usr,usr_follow"
			   " WHERE institutions.CtyCod=%ld"
			   " AND institutions.InsCod=centres.InsCod"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=usr_follow.%s"
			   " GROUP BY %s) AS F",
		     FieldDB[Fol],
		     Gbl.CurrentCty.Cty.CtyCod,
		     FieldDB[Fol],
		     FieldDB[1 - Fol]);
	    break;
	 case Sco_SCOPE_INS:
	    sprintf (Query,"SELECT AVG(N) FROM "
			   "(SELECT COUNT(DISTINCT usr_follow.%s) AS N"
			   " FROM centres,degrees,courses,crs_usr,usr_follow"
			   " WHERE centres.InsCod=%ld"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=usr_follow.%s"
			   " GROUP BY %s) AS F",
		     FieldDB[Fol],
		     Gbl.CurrentIns.Ins.InsCod,
		     FieldDB[Fol],
		     FieldDB[1 - Fol]);
	    break;
	 case Sco_SCOPE_CTR:
	    sprintf (Query,"SELECT AVG(N) FROM "
			   "(SELECT COUNT(DISTINCT usr_follow.%s) AS N"
			   " FROM degrees,courses,crs_usr,usr_follow"
			   " WHERE degrees.CtrCod=%ld"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=usr_follow.%s"
			   " GROUP BY %s) AS F",
		     FieldDB[Fol],
		     Gbl.CurrentCtr.Ctr.CtrCod,
		     FieldDB[Fol],
		     FieldDB[1 - Fol]);
	    break;
	 case Sco_SCOPE_DEG:
	    sprintf (Query,"SELECT AVG(N) FROM "
			   "(SELECT COUNT(DISTINCT usr_follow.%s) AS N"
			   " FROM courses,crs_usr,usr_follow"
			   " WHERE courses.DegCod=%ld"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=usr_follow.%s"
			   " GROUP BY %s) AS F",
		     FieldDB[Fol],
		     Gbl.CurrentDeg.Deg.DegCod,
		     FieldDB[Fol],
		     FieldDB[1 - Fol]);
	    break;
	 case Sco_SCOPE_CRS:
	    sprintf (Query,"SELECT AVG(N) FROM "
			   "(SELECT COUNT(DISTINCT usr_follow.%s) AS N"
			   " FROM crs_usr,usr_follow"
			   " WHERE crs_usr.CrsCod=%ld"
			   " AND crs_usr.UsrCod=usr_follow.%s"
			   " GROUP BY %s) AS F",
		     FieldDB[Fol],
		     Gbl.CurrentCrs.Crs.CrsCod,
		     FieldDB[Fol],
		     FieldDB[1 - Fol]);
	    break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
	}
      DB_QuerySELECT (Query,&mysql_res,"can not get number of questions per survey");

      /***** Get average *****/
      row = mysql_fetch_row (mysql_res);
      Average = Str_GetFloatNumFromStr (row[0]);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Write number of followed per follower *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"DAT LEFT_MIDDLE\">"
			 "%s"
			 "</td>"
			 "<td class=\"DAT RIGHT_MIDDLE\">"
			 "%5.2f"
			 "</td>"
			 "<td>"
			 "</td>"
			 "</tr>",
	       Txt_FollowPerFollow[Fol],
	       Average);
     }

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/***************************** Show stats of forums **************************/
/*****************************************************************************/

static void Sta_GetAndShowForumStats (void)
  {
   extern const char *Hlp_STATS_Figures_forums;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Scope;
   extern const char *Txt_Forums;
   extern const char *Txt_No_of_forums;
   extern const char *Txt_No_of_threads;
   extern const char *Txt_No_of_posts;
   extern const char *Txt_Number_of_BR_notifications;
   extern const char *Txt_No_of_threads_BR_per_forum;
   extern const char *Txt_No_of_posts_BR_per_thread;
   extern const char *Txt_No_of_posts_BR_per_forum;
   struct Sta_StatsForum StatsForum;

   /***** Reset total stats *****/
   StatsForum.NumForums           = 0;
   StatsForum.NumThreads          = 0;
   StatsForum.NumPosts            = 0;
   StatsForum.NumUsrsToBeNotifiedByEMail = 0;

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_FORUMS],
                             NULL,Hlp_STATS_Figures_forums,2);

   /***** Write table heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_TOP\" style=\"width:20px;\">"
                      "<img src=\"%s/forum64x64.gif\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO20x20\" />"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Gbl.Prefs.IconsURL,
            Txt_Scope,
            Txt_Scope,
            Txt_Forums,
            Txt_No_of_forums,
            Txt_No_of_threads,
            Txt_No_of_posts,
            Txt_Number_of_BR_notifications,
            Txt_No_of_threads_BR_per_forum,
            Txt_No_of_posts_BR_per_thread,
            Txt_No_of_posts_BR_per_forum);

   /***** Write a row for each type of forum *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
         Sta_ShowStatOfAForumType (For_FORUM_GLOBAL_USRS     ,-1L,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_GLOBAL_TCHS     ,-1L,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM__SWAD__USRS       ,-1L,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM__SWAD__TCHS       ,-1L,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_INSTIT_USRS,-1L,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_INSTIT_TCHS,-1L,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_CENTRE_USRS     ,-1L,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_CENTRE_TCHS     ,-1L,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_DEGREE_USRS     ,-1L,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_DEGREE_TCHS     ,-1L,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_COURSE_USRS     ,-1L,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_COURSE_TCHS     ,-1L,-1L,-1L,-1L,-1L,&StatsForum);
         break;
      case Sco_SCOPE_CTY:
         Sta_ShowStatOfAForumType (For_FORUM_INSTIT_USRS,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_INSTIT_TCHS,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_CENTRE_USRS     ,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_CENTRE_TCHS     ,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_DEGREE_USRS     ,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_DEGREE_TCHS     ,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_COURSE_USRS     ,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_COURSE_TCHS     ,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&StatsForum);
         break;
      case Sco_SCOPE_INS:
         Sta_ShowStatOfAForumType (For_FORUM_INSTIT_USRS,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_INSTIT_TCHS,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_CENTRE_USRS     ,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_CENTRE_TCHS     ,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_DEGREE_USRS     ,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_DEGREE_TCHS     ,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_COURSE_USRS     ,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_COURSE_TCHS     ,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&StatsForum);
         break;
      case Sco_SCOPE_CTR:
         Sta_ShowStatOfAForumType (For_FORUM_CENTRE_USRS,-1L,-1L,Gbl.CurrentCtr.Ctr.CtrCod,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_CENTRE_TCHS,-1L,-1L,Gbl.CurrentCtr.Ctr.CtrCod,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_DEGREE_USRS,-1L,-1L,Gbl.CurrentCtr.Ctr.CtrCod,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_DEGREE_TCHS,-1L,-1L,Gbl.CurrentCtr.Ctr.CtrCod,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_COURSE_USRS,-1L,-1L,Gbl.CurrentCtr.Ctr.CtrCod,-1L,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_COURSE_TCHS,-1L,-1L,Gbl.CurrentCtr.Ctr.CtrCod,-1L,-1L,&StatsForum);
         break;
      case Sco_SCOPE_DEG:
         Sta_ShowStatOfAForumType (For_FORUM_DEGREE_USRS,-1L,-1L,-1L,Gbl.CurrentDeg.Deg.DegCod,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_DEGREE_TCHS,-1L,-1L,-1L,Gbl.CurrentDeg.Deg.DegCod,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_COURSE_USRS,-1L,-1L,-1L,Gbl.CurrentDeg.Deg.DegCod,-1L,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_COURSE_TCHS,-1L,-1L,-1L,Gbl.CurrentDeg.Deg.DegCod,-1L,&StatsForum);
         break;
      case Sco_SCOPE_CRS:
         Sta_ShowStatOfAForumType (For_FORUM_COURSE_USRS,-1L,-1L,-1L,-1L,Gbl.CurrentCrs.Crs.CrsCod,&StatsForum);
         Sta_ShowStatOfAForumType (For_FORUM_COURSE_TCHS,-1L,-1L,-1L,-1L,Gbl.CurrentCrs.Crs.CrsCod,&StatsForum);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }

   Sta_WriteForumTotalStats (&StatsForum);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/************************* Show stats of a forum type ************************/
/*****************************************************************************/

static void Sta_ShowStatOfAForumType (For_ForumType_t ForumType,
                                      long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod,
                                      struct Sta_StatsForum *StatsForum)
  {
   extern const char *Txt_Courses;
   extern const char *Txt_Degrees;
   extern const char *Txt_Centres;
   extern const char *Txt_Institutions;
   extern const char *Txt_General;
   extern const char *Txt_only_teachers;

   switch (ForumType)
     {
      case For_FORUM_GLOBAL_USRS:
         Sta_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "forum64x64.gif",StatsForum,
                                      Txt_General,"");
         break;
      case For_FORUM_GLOBAL_TCHS:
         Sta_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "forum64x64.gif",StatsForum,
                                      Txt_General,Txt_only_teachers);
         break;
      case For_FORUM__SWAD__USRS:
         Sta_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "swad64x64.gif",StatsForum,
                                      Cfg_PLATFORM_SHORT_NAME,"");
         break;
      case For_FORUM__SWAD__TCHS:
         Sta_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "swad64x64.gif",StatsForum,
                                      Cfg_PLATFORM_SHORT_NAME,Txt_only_teachers);
         break;
      case For_FORUM_INSTIT_USRS:
         Sta_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "ins64x64.gif",StatsForum,
                                      Txt_Institutions,"");
         break;
      case For_FORUM_INSTIT_TCHS:
         Sta_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "ins64x64.gif",StatsForum,
                                      Txt_Institutions,Txt_only_teachers);
         break;
      case For_FORUM_CENTRE_USRS:
         Sta_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "ctr64x64.gif",StatsForum,
                                      Txt_Centres,"");
         break;
      case For_FORUM_CENTRE_TCHS:
         Sta_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "ctr64x64.gif",StatsForum,
                                      Txt_Centres,Txt_only_teachers);
         break;
      case For_FORUM_DEGREE_USRS:
         Sta_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "deg64x64.gif",StatsForum,
                                      Txt_Degrees,"");
         break;
      case For_FORUM_DEGREE_TCHS:
         Sta_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "deg64x64.gif",StatsForum,
                                      Txt_Degrees,Txt_only_teachers);
         break;
      case For_FORUM_COURSE_USRS:
         Sta_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "crs64x64.gif",StatsForum,
                                      Txt_Courses,"");
         break;
      case For_FORUM_COURSE_TCHS:
         Sta_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "crs64x64.gif",StatsForum,
                                      Txt_Courses,Txt_only_teachers);
         break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/******************* Write title and stats of a forum type *******************/
/*****************************************************************************/

static void Sta_WriteForumTitleAndStats (For_ForumType_t ForumType,
                                         long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod,
                                         const char *Icon,struct Sta_StatsForum *StatsForum,
                                         const char *ForumName1,const char *ForumName2)
  {
   unsigned NumForums;
   unsigned NumThreads;
   unsigned NumPosts;
   unsigned NumUsrsToBeNotifiedByEMail;
   float NumThrsPerForum;
   float NumPostsPerThread;
   float NumPostsPerForum;

   /***** Compute number of forums, number of threads and number of posts *****/
   NumForums  = For_GetNumTotalForumsOfType       (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod);
   NumThreads = For_GetNumTotalThrsInForumsOfType (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod);
   NumPosts   = For_GetNumTotalPstsInForumsOfType (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,&NumUsrsToBeNotifiedByEMail);

   /***** Compute number of threads per forum, number of posts per forum and number of posts per thread *****/
   NumThrsPerForum = (NumForums ? (float) NumThreads / (float) NumForums :
	                          0.0);
   NumPostsPerThread = (NumThreads ? (float) NumPosts / (float) NumThreads :
	                             0.0);
   NumPostsPerForum = (NumForums ? (float) NumPosts / (float) NumForums :
	                           0.0);

   /***** Update total stats *****/
   StatsForum->NumForums                  += NumForums;
   StatsForum->NumThreads                 += NumThreads;
   StatsForum->NumPosts                   += NumPosts;
   StatsForum->NumUsrsToBeNotifiedByEMail += NumUsrsToBeNotifiedByEMail;

   /***** Write forum name and stats *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"LEFT_TOP\" style=\"width:20px;\">"
                      "<img src=\"%s/%s\""
                      " alt=\"%s%s\" title=\"%s%s\""
                      " class=\"ICO20x20\" />"
                      "</td>"
                      "<td class=\"DAT LEFT_TOP\">"
                      "%s%s"
                      "</td>"
                      "<td class=\"DAT RIGHT_TOP\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_TOP\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_TOP\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_TOP\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_TOP\">"
                      "%.2f"
                      "</td>"
                      "<td class=\"DAT RIGHT_TOP\">"
                      "%.2f"
                      "</td>"
                      "<td class=\"DAT RIGHT_TOP\">"
                      "%.2f"
                      "</td>"
                      "</tr>",
            Gbl.Prefs.IconsURL,Icon,
            ForumName1,ForumName2,
            ForumName1,ForumName2,
            ForumName1,ForumName2,
            NumForums,NumThreads,NumPosts,NumUsrsToBeNotifiedByEMail,
            NumThrsPerForum,NumPostsPerThread,NumPostsPerForum);
  }

/*****************************************************************************/
/******************* Write title and stats of a forum type *******************/
/*****************************************************************************/

static void Sta_WriteForumTotalStats (struct Sta_StatsForum *StatsForum)
  {
   extern const char *Txt_Total;
   float NumThrsPerForum;
   float NumPostsPerThread;
   float NumPostsPerForum;

   /***** Compute number of threads per forum, number of posts per forum and number of posts per thread *****/
   NumThrsPerForum  = (StatsForum->NumForums ? (float) StatsForum->NumThreads / (float) StatsForum->NumForums :
	                                       0.0);
   NumPostsPerThread = (StatsForum->NumThreads ? (float) StatsForum->NumPosts / (float) StatsForum->NumThreads :
	                                         0.0);
   NumPostsPerForum = (StatsForum->NumForums ? (float) StatsForum->NumPosts / (float) StatsForum->NumForums :
	                                       0.0);

   /***** Write forum name and stats *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"DAT_N_LINE_TOP\" style=\"width:20px;\">"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP LEFT_MIDDLE\">"
                      "%s"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%.2f"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%.2f"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%.2f"
                      "</td>"
                      "</tr>",
            Txt_Total,
            StatsForum->NumForums,
            StatsForum->NumThreads,
            StatsForum->NumPosts,
            StatsForum->NumUsrsToBeNotifiedByEMail,
            NumThrsPerForum,
            NumPostsPerThread,
            NumPostsPerForum);
  }

/*****************************************************************************/
/****** Get and show number of users who want to be notified by email ********/
/*****************************************************************************/

static void Sta_GetAndShowNumUsrsPerNotifyEvent (void)
  {
   extern const char *Hlp_STATS_Figures_notifications;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Event;
   extern const char *Txt_NOTIFY_EVENTS_PLURAL[Ntf_NUM_NOTIFY_EVENTS];
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Txt_Number_of_BR_events;
   extern const char *Txt_Number_of_BR_emails;
   extern const char *Txt_Total;
   Ntf_NotifyEvent_t NotifyEvent;
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrsTotal;
   unsigned NumUsrsTotalWhoWantToBeNotifiedByEMailAboutSomeEvent;
   unsigned NumUsrs[Ntf_NUM_NOTIFY_EVENTS];
   unsigned NumEventsTotal = 0;
   unsigned NumEvents[Ntf_NUM_NOTIFY_EVENTS];
   unsigned NumMailsTotal = 0;
   unsigned NumMails[Ntf_NUM_NOTIFY_EVENTS];

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_NOTIFY_EVENTS],
                             NULL,Hlp_STATS_Figures_notifications,2);

   /***** Heading row *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Event,
            Txt_No_of_users,
            Txt_PERCENT_of_users,
            Txt_Number_of_BR_events,
            Txt_Number_of_BR_emails);

   /***** Get total number of users *****/
   NumUsrsTotal = (Gbl.Scope.Current == Sco_SCOPE_SYS) ? Usr_GetTotalNumberOfUsersInPlatform () :
                                                         Usr_GetTotalNumberOfUsersInCourses (Gbl.Scope.Current,
                                                                                             1 << Rol_STD |
                                                                                             1 << Rol_NET |
                                                                                             1 << Rol_TCH);

   /***** Get total number of users who want to be
          notified by email on some event, from database *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(*) FROM usr_data"
                        " WHERE EmailNtfEvents<>0");
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
                        " FROM institutions,centres,degrees,courses,crs_usr,usr_data"
                        " WHERE institutions.CtyCod=%ld"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_data.UsrCod"
                        " AND usr_data.EmailNtfEvents<>0",
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
                        " FROM centres,degrees,courses,crs_usr,usr_data"
                        " WHERE centres.InsCod=%ld"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_data.UsrCod"
                        " AND usr_data.EmailNtfEvents<>0",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
                        " FROM degrees,courses,crs_usr,usr_data"
                        " WHERE degrees.CtrCod=%ld"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_data.UsrCod"
                        " AND usr_data.EmailNtfEvents<>0",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
                        " FROM courses,crs_usr,usr_data"
                        " WHERE courses.DegCod=%ld"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_data.UsrCod"
                        " AND usr_data.EmailNtfEvents<>0",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
                        " FROM crs_usr,usr_data"
                        " WHERE crs_usr.CrsCod=%ld"
                        " AND crs_usr.UsrCod=usr_data.UsrCod"
                        " AND usr_data.EmailNtfEvents<>0",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   NumUsrsTotalWhoWantToBeNotifiedByEMailAboutSomeEvent = (unsigned) DB_QueryCOUNT (Query,"can not get the total number of users who want to be notified by email on some event");

   /***** For each notify event... *****/
   for (NotifyEvent = (Ntf_NotifyEvent_t) 1;
	NotifyEvent < Ntf_NUM_NOTIFY_EVENTS;
	NotifyEvent++) // 0 is reserved for Ntf_EVENT_UNKNOWN
     {
      /***** Get the number of users who want to be notified by email on this event, from database *****/
      switch (Gbl.Scope.Current)
        {
         case Sco_SCOPE_SYS:
            sprintf (Query,"SELECT COUNT(*) FROM usr_data"
        	           " WHERE ((EmailNtfEvents & %u)<>0)",
                     (1 << NotifyEvent));
            break;
	 case Sco_SCOPE_CTY:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM institutions,centres,degrees,courses,crs_usr,usr_data"
                           " WHERE institutions.CtyCod=%ld"
                           " AND institutions.InsCod=centres.InsCod"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND ((usr_data.EmailNtfEvents & %u)<>0)",
                     Gbl.CurrentCty.Cty.CtyCod,(1 << NotifyEvent));
            break;
	 case Sco_SCOPE_INS:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM centres,degrees,courses,crs_usr,usr_data"
                           " WHERE centres.InsCod=%ld"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND ((usr_data.EmailNtfEvents & %u)<>0)",
                     Gbl.CurrentIns.Ins.InsCod,(1 << NotifyEvent));
            break;
         case Sco_SCOPE_CTR:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM degrees,courses,crs_usr,usr_data"
                           " WHERE degrees.CtrCod=%ld"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND ((usr_data.EmailNtfEvents & %u)<>0)",
                     Gbl.CurrentCtr.Ctr.CtrCod,(1 << NotifyEvent));
            break;
         case Sco_SCOPE_DEG:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM courses,crs_usr,usr_data"
                           " WHERE courses.DegCod=%ld"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND ((usr_data.EmailNtfEvents & %u)<>0)",
                     Gbl.CurrentDeg.Deg.DegCod,(1 << NotifyEvent));
            break;
         case Sco_SCOPE_CRS:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM crs_usr,usr_data"
                           " WHERE crs_usr.CrsCod=%ld"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND ((usr_data.EmailNtfEvents & %u)<>0)",
                     Gbl.CurrentCrs.Crs.CrsCod,(1 << NotifyEvent));
            break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
        }
      NumUsrs[NotifyEvent] = (unsigned) DB_QueryCOUNT (Query,"can not get the number of users who want to be notified by email on an event");

      /***** Get number of notifications by email from database *****/
      switch (Gbl.Scope.Current)
        {
         case Sco_SCOPE_SYS:
            sprintf (Query,"SELECT SUM(NumEvents),SUM(NumMails)"
                           " FROM sta_notif"
                           " WHERE NotifyEvent=%u",
                     (unsigned) NotifyEvent);
            break;
	 case Sco_SCOPE_CTY:
            sprintf (Query,"SELECT SUM(sta_notif.NumEvents),SUM(sta_notif.NumMails)"
                           " FROM institutions,centres,degrees,sta_notif"
                           " WHERE institutions.CtyCod=%ld"
                           " AND institutions.InsCod=centres.InsCod"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=sta_notif.DegCod"
                           " AND sta_notif.NotifyEvent=%u",
                     Gbl.CurrentCty.Cty.CtyCod,(unsigned) NotifyEvent);
            break;
	 case Sco_SCOPE_INS:
            sprintf (Query,"SELECT SUM(sta_notif.NumEvents),SUM(sta_notif.NumMails)"
                           " FROM centres,degrees,sta_notif"
                           " WHERE centres.InsCod=%ld"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=sta_notif.DegCod"
                           " AND sta_notif.NotifyEvent=%u",
                     Gbl.CurrentIns.Ins.InsCod,(unsigned) NotifyEvent);
            break;
         case Sco_SCOPE_CTR:
            sprintf (Query,"SELECT SUM(sta_notif.NumEvents),SUM(sta_notif.NumMails)"
                           " FROM degrees,sta_notif"
                           " WHERE degrees.CtrCod=%ld"
                           " AND degrees.DegCod=sta_notif.DegCod"
                           " AND sta_notif.NotifyEvent=%u",
                     Gbl.CurrentCtr.Ctr.CtrCod,(unsigned) NotifyEvent);
            break;
         case Sco_SCOPE_DEG:
            sprintf (Query,"SELECT SUM(NumEvents),SUM(NumMails)"
                           " FROM sta_notif"
                           " WHERE DegCod=%ld"
                           " AND NotifyEvent=%u",
                     Gbl.CurrentDeg.Deg.DegCod,(unsigned) NotifyEvent);
            break;
         case Sco_SCOPE_CRS:
            sprintf (Query,"SELECT SUM(NumEvents),SUM(NumMails)"
                           " FROM sta_notif"
                           " WHERE CrsCod=%ld"
                           " AND NotifyEvent=%u",
                     Gbl.CurrentCrs.Crs.CrsCod,(unsigned) NotifyEvent);
            break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
        }
      DB_QuerySELECT (Query,&mysql_res,"can not get the number of notifications by email");

      row = mysql_fetch_row (mysql_res);

      /* Get number of events notified */
      if (row[0])
        {
         if (sscanf (row[0],"%u",&NumEvents[NotifyEvent]) != 1)
            Lay_ShowErrorAndExit ("Error when getting the number of notifications by email.");
        }
      else
         NumEvents[NotifyEvent] = 0;

      /* Get number of mails sent */
      if (row[1])
        {
         if (sscanf (row[1],"%u",&NumMails[NotifyEvent]) != 1)
            Lay_ShowErrorAndExit ("Error when getting the number of emails to notify events3.");
        }
      else
         NumMails[NotifyEvent] = 0;

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);

      /* Update total number of events and mails */
      NumEventsTotal += NumEvents[NotifyEvent];
      NumMailsTotal += NumMails[NotifyEvent];
     }

   /***** Write number of users who want to be notified by email on each event *****/
   for (NotifyEvent = (Ntf_NotifyEvent_t) 1;
	NotifyEvent < Ntf_NUM_NOTIFY_EVENTS;
	NotifyEvent++) // 0 is reserved for Ntf_EVENT_UNKNOWN
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"DAT LEFT_MIDDLE\">"
                         "%s"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%5.2f%%"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%u"
                         "</td>"
                         "</tr>",
               Txt_NOTIFY_EVENTS_PLURAL[NotifyEvent],
               NumUsrs[NotifyEvent],
               NumUsrsTotal ? (float) NumUsrs[NotifyEvent] * 100.0 /
        	              (float) NumUsrsTotal :
        	              0.0,
               NumEvents[NotifyEvent],
               NumMails[NotifyEvent]);

   /***** Write total number of users who want to be notified by email on some event *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"DAT_N_LINE_TOP LEFT_MIDDLE\">"
                      "%s"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%5.2f%%"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "</tr>",
            Txt_Total,
            NumUsrsTotalWhoWantToBeNotifiedByEMailAboutSomeEvent,
            NumUsrsTotal ? (float) NumUsrsTotalWhoWantToBeNotifiedByEMailAboutSomeEvent * 100.0 /
        	           (float) NumUsrsTotal :
        	           0.0,
            NumEventsTotal,
            NumMailsTotal);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/***************************** Show stats of notices *************************/
/*****************************************************************************/

static void Sta_GetAndShowNoticesStats (void)
  {
   extern const char *Hlp_STATS_Figures_notices;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_NOTICE_Active_BR_notices;
   extern const char *Txt_NOTICE_Obsolete_BR_notices;
   extern const char *Txt_NOTICE_Deleted_BR_notices;
   extern const char *Txt_Total;
   extern const char *Txt_Number_of_BR_notifications;
   Not_Status_t NoticeStatus;
   unsigned NumNotices[Not_NUM_STATUS];
   unsigned NumNoticesDeleted;
   unsigned NumTotalNotices = 0;
   unsigned NumNotif;
   unsigned NumTotalNotifications = 0;

   /***** Get the number of notices active and obsolete
          from this location (all the platform, current degree or current course) *****/
   for (NoticeStatus = (Not_Status_t) 0;
	NoticeStatus < Not_NUM_STATUS;
	NoticeStatus++)
     {
      NumNotices[NoticeStatus] = Not_GetNumNotices (Gbl.Scope.Current,NoticeStatus,&NumNotif);
      NumTotalNotices += NumNotices[NoticeStatus];
      NumTotalNotifications += NumNotif;
     }
   NumNoticesDeleted = Not_GetNumNoticesDeleted (Gbl.Scope.Current,&NumNotif);
   NumTotalNotices += NumNoticesDeleted;
   NumTotalNotifications += NumNotif;

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_NOTICES],
                             NULL,Hlp_STATS_Figures_notices,2);

   /***** Write table heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_NOTICE_Active_BR_notices,
            Txt_NOTICE_Obsolete_BR_notices,
            Txt_NOTICE_Deleted_BR_notices,
            Txt_Total,
            Txt_Number_of_BR_notifications);

   /***** Write number of notices *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT_N RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "</tr>",
            NumNotices[Not_ACTIVE_NOTICE],
            NumNotices[Not_OBSOLETE_NOTICE],
            NumNoticesDeleted,
            NumTotalNotices,
            NumTotalNotifications);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/*************************** Show stats of messages **************************/
/*****************************************************************************/

static void Sta_GetAndShowMsgsStats (void)
  {
   extern const char *Hlp_STATS_Figures_messages;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Messages;
   extern const char *Txt_MSGS_Not_deleted;
   extern const char *Txt_MSGS_Deleted;
   extern const char *Txt_Total;
   extern const char *Txt_Number_of_BR_notifications;
   extern const char *Txt_MSGS_Sent;
   extern const char *Txt_MSGS_Received;
   unsigned NumMsgsSentNotDeleted,NumMsgsSentDeleted;
   unsigned NumMsgsReceivedNotDeleted,NumMsgsReceivedAndDeleted;
   unsigned NumMsgsReceivedAndNotified;

   /***** Get the number of unique messages sent from this location (all the platform, current degree or current course) *****/
   NumMsgsSentNotDeleted      = Msg_GetNumMsgsSent     (Gbl.Scope.Current,Msg_STATUS_ALL     );
   NumMsgsSentDeleted         = Msg_GetNumMsgsSent     (Gbl.Scope.Current,Msg_STATUS_DELETED );

   NumMsgsReceivedNotDeleted  = Msg_GetNumMsgsReceived (Gbl.Scope.Current,Msg_STATUS_ALL     );
   NumMsgsReceivedAndDeleted  = Msg_GetNumMsgsReceived (Gbl.Scope.Current,Msg_STATUS_DELETED );
   NumMsgsReceivedAndNotified = Msg_GetNumMsgsReceived (Gbl.Scope.Current,Msg_STATUS_NOTIFIED);

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_MESSAGES],
                             NULL,Hlp_STATS_Figures_messages,2);

   /***** Write table heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Messages,
            Txt_MSGS_Not_deleted,
            Txt_MSGS_Deleted,
            Txt_Total,
            Txt_Number_of_BR_notifications);

   /***** Write number of messages *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"DAT LEFT_MIDDLE\">"
                      "%s"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT_N RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "-"
                      "</td>"
                      "</tr>"
                      "<tr>"
                      "<td class=\"DAT LEFT_MIDDLE\">"
                      "%s"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT_N RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "</tr>",
            Txt_MSGS_Sent,
            NumMsgsSentNotDeleted,
            NumMsgsSentDeleted,
            NumMsgsSentNotDeleted + NumMsgsSentDeleted,
            Txt_MSGS_Received,
            NumMsgsReceivedNotDeleted,
            NumMsgsReceivedAndDeleted,
            NumMsgsReceivedNotDeleted + NumMsgsReceivedAndDeleted,
            NumMsgsReceivedAndNotified);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/***************************** Show stats of surveys *************************/
/*****************************************************************************/

static void Sta_GetAndShowSurveysStats (void)
  {
   extern const char *Hlp_STATS_Figures_surveys;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Number_of_BR_surveys;
   extern const char *Txt_Number_of_BR_courses_with_BR_surveys;
   extern const char *Txt_Average_number_BR_of_surveys_BR_per_course;
   extern const char *Txt_Average_number_BR_of_questions_BR_per_survey;
   extern const char *Txt_Number_of_BR_notifications;
   unsigned NumSurveys;
   unsigned NumNotif;
   unsigned NumCoursesWithSurveys = 0;
   float NumSurveysPerCourse = 0.0;
   float NumQstsPerSurvey = 0.0;

   /***** Get the number of surveys and the average number of questions per survey from this location
          (all the platform, current degree or current course) *****/
   if ((NumSurveys = Svy_GetNumCrsSurveys (Gbl.Scope.Current,&NumNotif)))
     {
      if ((NumCoursesWithSurveys = Svy_GetNumCoursesWithCrsSurveys (Gbl.Scope.Current)) != 0)
         NumSurveysPerCourse = (float) NumSurveys / (float) NumCoursesWithSurveys;
      NumQstsPerSurvey = Svy_GetNumQstsPerCrsSurvey (Gbl.Scope.Current);
     }

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_SURVEYS],
                             NULL,Hlp_STATS_Figures_surveys,2);

   /***** Write table heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Number_of_BR_surveys,
            Txt_Number_of_BR_courses_with_BR_surveys,
            Txt_Average_number_BR_of_surveys_BR_per_course,
            Txt_Average_number_BR_of_questions_BR_per_survey,
            Txt_Number_of_BR_notifications);

   /***** Write number of surveys *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%.2f"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%.2f"
                      "</td>"
                      "<td class=\"DAT RIGHT_MIDDLE\">"
                      "%u"
                      "</td>"
                      "</tr>",
            NumSurveys,
            NumCoursesWithSurveys,
            NumSurveysPerCourse,
            NumQstsPerSurvey,
            NumNotif);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/********** Get and show number of users who have chosen a privacy ***********/
/*****************************************************************************/

static void Sta_GetAndShowNumUsrsPerPrivacy (void)
  {
   extern const char *Hlp_STATS_Figures_privacy;
   extern const char *Txt_Photo;
   extern const char *Txt_Public_profile;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_PRIVACY],
                             NULL,Hlp_STATS_Figures_privacy,2);

   /***** Privacy for photo *****/
   Sta_GetAndShowNumUsrsPerPrivacyForAnObject (Txt_Photo,"PhotoVisibility");

   /***** Privacy for public profile *****/
   Sta_GetAndShowNumUsrsPerPrivacyForAnObject (Txt_Public_profile,"ProfileVisibility");

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/********** Get and show number of users who have chosen a privacy ***********/
/*****************************************************************************/

static void Sta_GetAndShowNumUsrsPerPrivacyForAnObject (const char *TxtObject,const char *FieldName)
  {
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];
   extern const char *Txt_PRIVACY_OPTIONS[Pri_NUM_OPTIONS_PRIVACY];
   Pri_Visibility_t Visibility;
   char Query[1024];
   unsigned NumUsrs[Pri_NUM_OPTIONS_PRIVACY];
   unsigned NumUsrsTotal = 0;

   /***** Heading row *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            TxtObject,
            Txt_No_of_users,
            Txt_PERCENT_of_users);

   /***** For each privacy option... *****/
   for (Visibility = (Pri_Visibility_t) 0;
	Visibility < Pri_NUM_OPTIONS_PRIVACY;
	Visibility++)
     {
      /***** Get the number of users who have chosen this privacy option from database *****/
      switch (Gbl.Scope.Current)
        {
         case Sco_SCOPE_SYS:
            sprintf (Query,"SELECT COUNT(*)"
        	           " FROM usr_data WHERE %s='%s'",
                     FieldName,
        	     Pri_VisibilityDB[Visibility]);
            break;
	 case Sco_SCOPE_CTY:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM institutions,centres,degrees,courses,crs_usr,usr_data"
                           " WHERE institutions.CtyCod=%ld"
                           " AND institutions.InsCod=centres.InsCod"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.%s='%s'",
                     Gbl.CurrentCty.Cty.CtyCod,
                     FieldName,
                     Pri_VisibilityDB[Visibility]);
            break;
         case Sco_SCOPE_INS:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM centres,degrees,courses,crs_usr,usr_data"
                           " WHERE centres.InsCod=%ld"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.%s='%s'",
                     Gbl.CurrentIns.Ins.InsCod,
                     FieldName,
                     Pri_VisibilityDB[Visibility]);
            break;
         case Sco_SCOPE_CTR:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM degrees,courses,crs_usr,usr_data"
                           " WHERE degrees.CtrCod=%ld"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.%s='%s'",
                     Gbl.CurrentCtr.Ctr.CtrCod,
                     FieldName,
                     Pri_VisibilityDB[Visibility]);
            break;
         case Sco_SCOPE_DEG:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM courses,crs_usr,usr_data"
                           " WHERE courses.DegCod=%ld"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.%s='%s'",
                     Gbl.CurrentDeg.Deg.DegCod,
                     FieldName,
                     Pri_VisibilityDB[Visibility]);
            break;
         case Sco_SCOPE_CRS:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM crs_usr,usr_data"
                           " WHERE crs_usr.CrsCod=%ld"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.%s='%s'",
                     Gbl.CurrentCrs.Crs.CrsCod,
                     FieldName,
                     Pri_VisibilityDB[Visibility]);
            break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
        }
      NumUsrs[Visibility] = (unsigned) DB_QueryCOUNT (Query,"can not get the number of users who have chosen a privacy");

      /* Update total number of users */
      NumUsrsTotal += NumUsrs[Visibility];
     }

   /***** Write number of users who have chosen each privacy option *****/
   for (Visibility = (Pri_Visibility_t) 0;
	Visibility < Pri_NUM_OPTIONS_PRIVACY;
	Visibility++)
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"DAT LEFT_MIDDLE\">"
                         "%s"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%5.2f%%"
                         "</td>"
                         "</tr>",
               Txt_PRIVACY_OPTIONS[Visibility],NumUsrs[Visibility],
               NumUsrsTotal ? (float) NumUsrs[Visibility] * 100.0 /
        	              (float) NumUsrsTotal :
        	              0);
   }

/*****************************************************************************/
/********* Get and show number of users who have chosen a language ***********/
/*****************************************************************************/

static void Sta_GetAndShowNumUsrsPerLanguage (void)
  {
   extern const char *Hlp_STATS_Figures_language;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Language;
   extern const char *Txt_STR_LANG_ID[1 + Txt_NUM_LANGUAGES];
   extern const char *Txt_STR_LANG_NAME[1 + Txt_NUM_LANGUAGES];
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   Txt_Language_t Lan;
   char Query[1024];
   unsigned NumUsrs[1 + Txt_NUM_LANGUAGES];
   unsigned NumUsrsTotal = 0;

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_LANGUAGES],
                             NULL,Hlp_STATS_Figures_language,2);

   /***** Heading row *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Language,
            Txt_No_of_users,
            Txt_PERCENT_of_users);

   /***** For each language... *****/
   for (Lan = (Txt_Language_t) 1;
	Lan <= Txt_NUM_LANGUAGES;
	Lan++)
     {
      /***** Get the number of users who have chosen this language from database *****/
      switch (Gbl.Scope.Current)
        {
         case Sco_SCOPE_SYS:
            sprintf (Query,"SELECT COUNT(*)"
        	           " FROM usr_data WHERE Language='%s'",
        	     Txt_STR_LANG_ID[Lan]);
            break;
	 case Sco_SCOPE_CTY:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM institutions,centres,degrees,courses,crs_usr,usr_data"
                           " WHERE institutions.CtyCod=%ld"
                           " AND institutions.InsCod=centres.InsCod"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.Language='%s'",
                     Gbl.CurrentCty.Cty.CtyCod,
                     Txt_STR_LANG_ID[Lan]);
            break;
	 case Sco_SCOPE_INS:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM centres,degrees,courses,crs_usr,usr_data"
                           " WHERE centres.InsCod=%ld"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.Language='%s'",
                     Gbl.CurrentIns.Ins.InsCod,
                     Txt_STR_LANG_ID[Lan]);
            break;
         case Sco_SCOPE_CTR:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM degrees,courses,crs_usr,usr_data"
                           " WHERE degrees.CtrCod=%ld"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.Language='%s'",
                     Gbl.CurrentCtr.Ctr.CtrCod,
                     Txt_STR_LANG_ID[Lan]);
            break;
         case Sco_SCOPE_DEG:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM courses,crs_usr,usr_data"
                           " WHERE courses.DegCod=%ld"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.Language='%s'",
                     Gbl.CurrentDeg.Deg.DegCod,
                     Txt_STR_LANG_ID[Lan]);
            break;
         case Sco_SCOPE_CRS:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM crs_usr,usr_data"
                           " WHERE crs_usr.CrsCod=%ld"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.Language='%s'",
                     Gbl.CurrentCrs.Crs.CrsCod,
                     Txt_STR_LANG_ID[Lan]);
            break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
        }
      NumUsrs[Lan] = (unsigned) DB_QueryCOUNT (Query,"can not get the number of users who have chosen a language");

      /* Update total number of users */
      NumUsrsTotal += NumUsrs[Lan];
     }

   /***** Write number of users who have chosen each language *****/
   for (Lan = (Txt_Language_t) 1;
	Lan <= Txt_NUM_LANGUAGES;
	Lan++)
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"DAT LEFT_MIDDLE\">"
                         "%s"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%5.2f%%"
                         "</td>"
                         "</tr>",
               Txt_STR_LANG_NAME[Lan],NumUsrs[Lan],
               NumUsrsTotal ? (float) NumUsrs[Lan] * 100.0 /
        	              (float) NumUsrsTotal :
        	              0);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/***** Get and show number of users who have chosen a first day of week ******/
/*****************************************************************************/

static void Sta_GetAndShowNumUsrsPerFirstDayOfWeek (void)
  {
   extern const bool Cal_DayIsValidAsFirstDayOfWeek[7];
   extern const char *Hlp_STATS_Figures_calendar;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Calendar;
   extern const char *Txt_First_day_of_the_week;
   extern const char *Txt_DAYS_SMALL[7];
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   unsigned FirstDayOfWeek;
   char Query[1024];
   unsigned NumUsrs[7];	// 7: seven days in a week
   unsigned NumUsrsTotal = 0;

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_FIRST_DAY_OF_WEEK],
                             NULL,Hlp_STATS_Figures_calendar,2);

   /***** Heading row *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Calendar,
            Txt_No_of_users,
            Txt_PERCENT_of_users);

   /***** For each day... *****/
   for (FirstDayOfWeek = 0;	// Monday
	FirstDayOfWeek <= 6;	// Sunday
	FirstDayOfWeek++)
      if (Cal_DayIsValidAsFirstDayOfWeek[FirstDayOfWeek])
	{
	 /***** Get number of users who have chosen this first day of week from database *****/
	 switch (Gbl.Scope.Current)
	   {
	    case Sco_SCOPE_SYS:
	       sprintf (Query,"SELECT COUNT(*) FROM usr_data"
			      " WHERE FirstDayOfWeek=%u",
			(unsigned) FirstDayOfWeek);
	       break;
	    case Sco_SCOPE_CTY:
	       sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
			      " FROM institutions,centres,degrees,courses,crs_usr,usr_data"
			      " WHERE institutions.CtyCod=%ld"
			      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=usr_data.UsrCod"
			      " AND usr_data.FirstDayOfWeek=%u",
			Gbl.CurrentCty.Cty.CtyCod,(unsigned) FirstDayOfWeek);
	       break;
	    case Sco_SCOPE_INS:
	       sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
			      " FROM centres,degrees,courses,crs_usr,usr_data"
			      " WHERE centres.InsCod=%ld"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=usr_data.UsrCod"
			      " AND usr_data.FirstDayOfWeek=%u",
			Gbl.CurrentIns.Ins.InsCod,(unsigned) FirstDayOfWeek);
	       break;
	    case Sco_SCOPE_CTR:
	       sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
			      " FROM degrees,courses,crs_usr,usr_data"
			      " WHERE degrees.CtrCod=%ld"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=usr_data.UsrCod"
			      " AND usr_data.FirstDayOfWeek=%u",
			Gbl.CurrentCtr.Ctr.CtrCod,(unsigned) FirstDayOfWeek);
	       break;
	    case Sco_SCOPE_DEG:
	       sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
			      " FROM courses,crs_usr,usr_data"
			      " WHERE courses.DegCod=%ld"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=usr_data.UsrCod"
			      " AND usr_data.FirstDayOfWeek=%u",
			Gbl.CurrentDeg.Deg.DegCod,(unsigned) FirstDayOfWeek);
	       break;
	    case Sco_SCOPE_CRS:
	       sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
			      " FROM crs_usr,usr_data"
			      " WHERE crs_usr.CrsCod=%ld"
			      " AND crs_usr.UsrCod=usr_data.UsrCod"
			      " AND usr_data.FirstDayOfWeek=%u",
			Gbl.CurrentCrs.Crs.CrsCod,(unsigned) FirstDayOfWeek);
	       break;
	    default:
	       Lay_ShowErrorAndExit ("Wrong scope.");
	       break;
	   }
	 NumUsrs[FirstDayOfWeek] = (unsigned) DB_QueryCOUNT (Query,"can not get the number of users who have chosen a first day of week");

	 /* Update total number of users */
	 NumUsrsTotal += NumUsrs[FirstDayOfWeek];
        }

   /***** Write number of users who have chosen each first day of week *****/
   for (FirstDayOfWeek = 0;	// Monday
	FirstDayOfWeek <= 6;	// Sunday
	FirstDayOfWeek++)
      if (Cal_DayIsValidAsFirstDayOfWeek[FirstDayOfWeek])
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"CENTER_MIDDLE\">"
			    "<img src=\"%s/first-day-of-week-%u-64x64.png\""
			    " alt=\"%s\" title=\"%s: %s\""
			    " class=\"ICO40x40\" />"
			    "</td>"
			    "<td class=\"DAT RIGHT_MIDDLE\">"
			    "%u"
			    "</td>"
			    "<td class=\"DAT RIGHT_MIDDLE\">"
			    "%5.2f%%"
			    "</td>"
			    "</tr>",
		  Gbl.Prefs.IconsURL,FirstDayOfWeek,
		  Txt_DAYS_SMALL[FirstDayOfWeek],
		  Txt_First_day_of_the_week,Txt_DAYS_SMALL[FirstDayOfWeek],
		  NumUsrs[FirstDayOfWeek],
		  NumUsrsTotal ? (float) NumUsrs[FirstDayOfWeek] * 100.0 /
				 (float) NumUsrsTotal :
				 0);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/******** Get and show number of users who have chosen a date format *********/
/*****************************************************************************/

static void Sta_GetAndShowNumUsrsPerDateFormat (void)
  {
   extern const char *Hlp_STATS_Figures_dates;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Format;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   unsigned Format;
   char Query[1024];
   unsigned NumUsrs[Dat_NUM_OPTIONS_FORMAT];
   unsigned NumUsrsTotal = 0;

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_DATE_FORMAT],
                             NULL,Hlp_STATS_Figures_dates,2);

   /***** Heading row *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Format,
            Txt_No_of_users,
            Txt_PERCENT_of_users);

   /***** For each format... *****/
   for (Format = (Dat_Format_t) 0;
	Format <= (Dat_Format_t) (Dat_NUM_OPTIONS_FORMAT - 1);
	Format++)
     {
      /***** Get number of users who have chosen this date format from database *****/
      switch (Gbl.Scope.Current)
	{
	 case Sco_SCOPE_SYS:
	    sprintf (Query,"SELECT COUNT(*) FROM usr_data"
			   " WHERE DateFormat=%u",
		     (unsigned) Format);
	    break;
	 case Sco_SCOPE_CTY:
	    sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
			   " FROM institutions,centres,degrees,courses,crs_usr,usr_data"
			   " WHERE institutions.CtyCod=%ld"
			   " AND institutions.InsCod=centres.InsCod"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=usr_data.UsrCod"
			   " AND usr_data.DateFormat=%u",
		     Gbl.CurrentCty.Cty.CtyCod,(unsigned) Format);
	    break;
	 case Sco_SCOPE_INS:
	    sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
			   " FROM centres,degrees,courses,crs_usr,usr_data"
			   " WHERE centres.InsCod=%ld"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=usr_data.UsrCod"
			   " AND usr_data.DateFormat=%u",
		     Gbl.CurrentIns.Ins.InsCod,(unsigned) Format);
	    break;
	 case Sco_SCOPE_CTR:
	    sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
			   " FROM degrees,courses,crs_usr,usr_data"
			   " WHERE degrees.CtrCod=%ld"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=usr_data.UsrCod"
			   " AND usr_data.DateFormat=%u",
		     Gbl.CurrentCtr.Ctr.CtrCod,(unsigned) Format);
	    break;
	 case Sco_SCOPE_DEG:
	    sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
			   " FROM courses,crs_usr,usr_data"
			   " WHERE courses.DegCod=%ld"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.UsrCod=usr_data.UsrCod"
			   " AND usr_data.DateFormat=%u",
		     Gbl.CurrentDeg.Deg.DegCod,(unsigned) Format);
	    break;
	 case Sco_SCOPE_CRS:
	    sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
			   " FROM crs_usr,usr_data"
			   " WHERE crs_usr.CrsCod=%ld"
			   " AND crs_usr.UsrCod=usr_data.UsrCod"
			   " AND usr_data.DateFormat=%u",
		     Gbl.CurrentCrs.Crs.CrsCod,(unsigned) Format);
	    break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
	}
      NumUsrs[Format] = (unsigned) DB_QueryCOUNT (Query,"can not get the number of users who have chosen a date format");

      /* Update total number of users */
      NumUsrsTotal += NumUsrs[Format];
     }

   /***** Write number of users who have chosen each date format *****/
   for (Format = (Dat_Format_t) 0;
	Format <= (Dat_Format_t) (Dat_NUM_OPTIONS_FORMAT - 1);
	Format++)
     {
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"DAT_N LEFT_MIDDLE\">");
      Dat_PutSpanDateFormat (Format);
      Dat_PutScriptDateFormat (Format);
      fprintf (Gbl.F.Out,"</td>"
			 "<td class=\"DAT RIGHT_MIDDLE\">"
			 "%u"
			 "</td>"
			 "<td class=\"DAT RIGHT_MIDDLE\">"
			 "%5.2f%%"
			 "</td>"
			 "</tr>",
	       NumUsrs[Format],
	       NumUsrsTotal ? (float) NumUsrs[Format] * 100.0 /
			      (float) NumUsrsTotal :
			      0);
     }

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/********* Get and show number of users who have chosen an icon set **********/
/*****************************************************************************/

static void Sta_GetAndShowNumUsrsPerIconSet (void)
  {
   extern const char *Hlp_STATS_Figures_icons;
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   extern const char *Ico_IconSetNames[Ico_NUM_ICON_SETS];
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Icons;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   Ico_IconSet_t IconSet;
   char Query[1024];
   unsigned NumUsrs[Ico_NUM_ICON_SETS];
   unsigned NumUsrsTotal = 0;

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_ICON_SETS],
                             NULL,Hlp_STATS_Figures_icons,2);

   /***** Heading row *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Icons,
            Txt_No_of_users,
            Txt_PERCENT_of_users);

   /***** For each icon set... *****/
   for (IconSet = (Ico_IconSet_t) 0;
	IconSet < Ico_NUM_ICON_SETS;
	IconSet++)
     {
      /***** Get the number of users who have chosen this icon set from database *****/
      switch (Gbl.Scope.Current)
        {
         case Sco_SCOPE_SYS:
            sprintf (Query,"SELECT COUNT(*) FROM usr_data"
        	           " WHERE IconSet='%s'",
        	     Ico_IconSetId[IconSet]);
            break;
	 case Sco_SCOPE_CTY:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM institutions,centres,degrees,courses,crs_usr,usr_data"
                           " WHERE institutions.CtyCod=%ld"
                           " AND institutions.InsCod=centres.InsCod"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.IconSet='%s'",
                     Gbl.CurrentCty.Cty.CtyCod,Ico_IconSetId[IconSet]);
            break;
	 case Sco_SCOPE_INS:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM centres,degrees,courses,crs_usr,usr_data"
                           " WHERE centres.InsCod=%ld"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.IconSet='%s'",
                     Gbl.CurrentIns.Ins.InsCod,Ico_IconSetId[IconSet]);
            break;
         case Sco_SCOPE_CTR:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM degrees,courses,crs_usr,usr_data"
                           " WHERE degrees.CtrCod=%ld"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.IconSet='%s'",
                     Gbl.CurrentCtr.Ctr.CtrCod,Ico_IconSetId[IconSet]);
            break;
         case Sco_SCOPE_DEG:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM courses,crs_usr,usr_data"
                           " WHERE courses.DegCod=%ld"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.IconSet='%s'",
                     Gbl.CurrentDeg.Deg.DegCod,Ico_IconSetId[IconSet]);
            break;
         case Sco_SCOPE_CRS:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM crs_usr,usr_data"
                           " WHERE crs_usr.CrsCod=%ld"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.IconSet='%s'",
                     Gbl.CurrentCrs.Crs.CrsCod,Ico_IconSetId[IconSet]);
            break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
        }
      NumUsrs[IconSet] = (unsigned) DB_QueryCOUNT (Query,"can not get the number of users who have chosen an icon set");

      /* Update total number of users */
      NumUsrsTotal += NumUsrs[IconSet];
     }

   /***** Write number of users who have chosen each icon set *****/
   for (IconSet = (Ico_IconSet_t) 0;
	IconSet < Ico_NUM_ICON_SETS;
	IconSet++)
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"LEFT_MIDDLE\">"
                         "<img src=\"%s/%s/%s/%s/heart64x64.gif\""
                         " alt=\"%s\" title=\"%s\""
                         " class=\"ICO40x40\" />"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%5.2f%%"
                         "</td>"
                         "</tr>",
               Gbl.Prefs.IconsURL,
               Cfg_ICON_FOLDER_ICON_SETS,
               Ico_IconSetId[IconSet],
               Cfg_ICON_ACTION,
               Ico_IconSetNames[IconSet],
               Ico_IconSetNames[IconSet],
               NumUsrs[IconSet],
               NumUsrsTotal ? (float) NumUsrs[IconSet] * 100.0 /
        	              (float) NumUsrsTotal :
        	              0);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/*********** Get and show number of users who have chosen a menu *************/
/*****************************************************************************/

static void Sta_GetAndShowNumUsrsPerMenu (void)
  {
   extern const char *Hlp_STATS_Figures_menu;
   extern const char *Mnu_MenuIcons[Mnu_NUM_MENUS];
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Menu;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Txt_MENU_NAMES[Mnu_NUM_MENUS];
   Mnu_Menu_t Menu;
   char Query[1024];
   unsigned NumUsrs[Mnu_NUM_MENUS];
   unsigned NumUsrsTotal = 0;

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_MENUS],
                             NULL,Hlp_STATS_Figures_menu,2);

   /***** Heading row *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Menu,
            Txt_No_of_users,
            Txt_PERCENT_of_users);

   /***** For each menu... *****/
   for (Menu = (Mnu_Menu_t) 0;
	Menu < Mnu_NUM_MENUS;
	Menu++)
     {
      /***** Get number of users who have chosen this menu from database *****/
      switch (Gbl.Scope.Current)
        {
         case Sco_SCOPE_SYS:
            sprintf (Query,"SELECT COUNT(*) FROM usr_data"
        	           " WHERE Menu=%u",
                     (unsigned) Menu);
            break;
	 case Sco_SCOPE_CTY:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM institutions,centres,degrees,courses,crs_usr,usr_data"
                           " WHERE institutions.CtyCod=%ld"
                           " AND institutions.InsCod=centres.InsCod"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.Menu=%u",
                     Gbl.CurrentCty.Cty.CtyCod,(unsigned) Menu);
            break;
	 case Sco_SCOPE_INS:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
                           " FROM centres,degrees,courses,crs_usr,usr_data"
                           " WHERE centres.InsCod=%ld"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.Menu=%u",
                     Gbl.CurrentIns.Ins.InsCod,(unsigned) Menu);
            break;
         case Sco_SCOPE_CTR:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
                           " FROM degrees,courses,crs_usr,usr_data"
                           " WHERE degrees.CtrCod=%ld"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.Menu=%u",
                     Gbl.CurrentCtr.Ctr.CtrCod,(unsigned) Menu);
            break;
         case Sco_SCOPE_DEG:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
                           " FROM courses,crs_usr,usr_data"
                           " WHERE courses.DegCod=%ld"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.Menu=%u",
                     Gbl.CurrentDeg.Deg.DegCod,(unsigned) Menu);
            break;
         case Sco_SCOPE_CRS:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
                           " FROM crs_usr,usr_data"
                           " WHERE crs_usr.CrsCod=%ld"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.Menu=%u",
                     Gbl.CurrentCrs.Crs.CrsCod,(unsigned) Menu);
            break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
        }
      NumUsrs[Menu] = (unsigned) DB_QueryCOUNT (Query,"can not get the number of users who have chosen a menu");

      /* Update total number of users */
      NumUsrsTotal += NumUsrs[Menu];
     }

   /***** Write number of users who have chosen each menu *****/
   for (Menu = (Mnu_Menu_t) 0;
	Menu < Mnu_NUM_MENUS;
	Menu++)
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"CENTER_MIDDLE\">"
                         "<img src=\"%s/%s32x32.gif\""
                         " alt=\"%s\" title=\"%s\""
                         " class=\"ICO40x40\" />"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%5.2f%%"
                         "</td>"
                         "</tr>",
               Gbl.Prefs.IconsURL,Mnu_MenuIcons[Menu],
               Txt_MENU_NAMES[Menu],
               Txt_MENU_NAMES[Menu],
               NumUsrs[Menu],
               NumUsrsTotal ? (float) NumUsrs[Menu] * 100.0 /
        	              (float) NumUsrsTotal :
        	              0);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/********** Get and show number of users who have chosen a theme *************/
/*****************************************************************************/

static void Sta_GetAndShowNumUsrsPerTheme (void)
  {
   extern const char *Hlp_STATS_Figures_theme;
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *The_ThemeNames[The_NUM_THEMES];
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Theme_SKIN;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   The_Theme_t Theme;
   char Query[1024];
   unsigned NumUsrs[The_NUM_THEMES];
   unsigned NumUsrsTotal = 0;

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_THEMES],
                             NULL,Hlp_STATS_Figures_theme,2);

   /***** Heading row *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Theme_SKIN,
            Txt_No_of_users,
            Txt_PERCENT_of_users);

   /***** For each theme... *****/
   for (Theme = (The_Theme_t) 0;
	Theme < The_NUM_THEMES;
	Theme++)
     {
      /***** Get number of users who have chosen this theme from database *****/
      switch (Gbl.Scope.Current)
        {
         case Sco_SCOPE_SYS:
            sprintf (Query,"SELECT COUNT(*) FROM usr_data"
        	           " WHERE Theme='%s'",
                     The_ThemeId[Theme]);
            break;
	 case Sco_SCOPE_CTY:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM institutions,centres,degrees,courses,crs_usr,usr_data"
                           " WHERE institutions.CtyCod=%ld"
                           " AND institutions.InsCod=centres.InsCod"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.Theme='%s'",
                     Gbl.CurrentCty.Cty.CtyCod,The_ThemeId[Theme]);
            break;
	 case Sco_SCOPE_INS:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
                           " FROM centres,degrees,courses,crs_usr,usr_data"
                           " WHERE centres.InsCod=%ld"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.Theme='%s'",
                     Gbl.CurrentIns.Ins.InsCod,The_ThemeId[Theme]);
            break;
         case Sco_SCOPE_CTR:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
                           " FROM degrees,courses,crs_usr,usr_data"
                           " WHERE degrees.CtrCod=%ld"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.Theme='%s'",
                     Gbl.CurrentCtr.Ctr.CtrCod,The_ThemeId[Theme]);
            break;
         case Sco_SCOPE_DEG:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
                           " FROM courses,crs_usr,usr_data"
                           " WHERE courses.DegCod=%ld"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.Theme='%s'",
                     Gbl.CurrentDeg.Deg.DegCod,The_ThemeId[Theme]);
            break;
         case Sco_SCOPE_CRS:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
                           " FROM crs_usr,usr_data"
                           " WHERE crs_usr.CrsCod=%ld"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.Theme='%s'",
                     Gbl.CurrentCrs.Crs.CrsCod,The_ThemeId[Theme]);
            break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
        }
      NumUsrs[Theme] = (unsigned) DB_QueryCOUNT (Query,"can not get the number of users who have chosen a theme");

      /* Update total number of users */
      NumUsrsTotal += NumUsrs[Theme];
     }

   /***** Write number of users who have chosen each theme *****/
   for (Theme = (The_Theme_t) 0;
	Theme < The_NUM_THEMES;
	Theme++)
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"CENTER_MIDDLE\">"
                         "<img src=\"%s/%s/%s/theme_32x20.gif\""
                         " alt=\"%s\" title=\"%s\""
                         " style=\"width:40px; height:25px;\" />"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%5.2f%%"
                         "</td>"
                         "</tr>",
               Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_THEMES,The_ThemeId[Theme],
               The_ThemeNames[Theme],
               The_ThemeNames[Theme],
               NumUsrs[Theme],
               NumUsrsTotal ? (float) NumUsrs[Theme] * 100.0 /
        	              (float) NumUsrsTotal :
        	              0);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/***** Get and show number of users who have chosen a layout of columns ******/
/*****************************************************************************/

static void Sta_GetAndShowNumUsrsPerSideColumns (void)
  {
   extern const char *Hlp_STATS_Figures_columns;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Columns;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   unsigned SideCols;
   char Query[1024];
   unsigned NumUsrs[4];
   unsigned NumUsrsTotal = 0;
   extern const char *Txt_LAYOUT_SIDE_COLUMNS[4];

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_SIDE_COLUMNS],
                             NULL,Hlp_STATS_Figures_columns,2);

   /***** Heading row *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Columns,
            Txt_No_of_users,
            Txt_PERCENT_of_users);

   /***** For each language... *****/
   for (SideCols = 0;
	SideCols <= Lay_SHOW_BOTH_COLUMNS;
	SideCols++)
     {
      /***** Get the number of users who have chosen this layout of columns from database *****/
      switch (Gbl.Scope.Current)
        {
         case Sco_SCOPE_SYS:
            sprintf (Query,"SELECT COUNT(*) FROM usr_data"
        	           " WHERE SideCols=%u",
                     SideCols);
            break;
	 case Sco_SCOPE_CTY:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM institutions,centres,degrees,courses,crs_usr,usr_data"
                           " WHERE institutions.CtyCod=%ld"
                           " AND institutions.InsCod=centres.InsCod"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.SideCols=%u",
                     Gbl.CurrentCty.Cty.CtyCod,SideCols);
            break;
	 case Sco_SCOPE_INS:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM centres,degrees,courses,crs_usr,usr_data"
                           " WHERE centres.InsCod=%ld"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.SideCols=%u",
                     Gbl.CurrentIns.Ins.InsCod,SideCols);
            break;
         case Sco_SCOPE_CTR:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM degrees,courses,crs_usr,usr_data"
                           " WHERE degrees.CtrCod=%ld"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.SideCols=%u",
                     Gbl.CurrentCtr.Ctr.CtrCod,SideCols);
            break;
         case Sco_SCOPE_DEG:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM courses,crs_usr,usr_data"
                           " WHERE courses.DegCod=%ld"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.SideCols=%u",
                     Gbl.CurrentDeg.Deg.DegCod,SideCols);
            break;
         case Sco_SCOPE_CRS:
            sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
        	           " FROM crs_usr,usr_data"
                           " WHERE crs_usr.CrsCod=%ld"
                           " AND crs_usr.UsrCod=usr_data.UsrCod"
                           " AND usr_data.SideCols=%u",
                     Gbl.CurrentCrs.Crs.CrsCod,SideCols);
            break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
        }
      NumUsrs[SideCols] = (unsigned) DB_QueryCOUNT (Query,"can not get the number of users who have chosen a layout of columns");

      /* Update total number of users */
      NumUsrsTotal += NumUsrs[SideCols];
     }

   /***** Write number of users who have chosen this layout of columns *****/
   for (SideCols = 0;
	SideCols <= Lay_SHOW_BOTH_COLUMNS;
	SideCols++)
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"CENTER_MIDDLE\">"
                         "<img src=\"%s/layout%u%u_32x20.gif\""
                         " alt=\"%s\" title=\"%s\""
                         " style=\"width:40px; height:25px;\" />"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%5.2f%%"
                         "</td>"
                         "</tr>",
               Gbl.Prefs.IconsURL,SideCols >> 1,SideCols & 1,
               Txt_LAYOUT_SIDE_COLUMNS[SideCols],
               Txt_LAYOUT_SIDE_COLUMNS[SideCols],
               NumUsrs[SideCols],
               NumUsrsTotal ? (float) NumUsrs[SideCols] * 100.0 /
        	              (float) NumUsrsTotal :
        	              0);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/**************** Compute the time used to generate the page *****************/
/*****************************************************************************/

void Sta_ComputeTimeToGeneratePage (void)
  {
   if (gettimeofday (&Gbl.tvPageCreated, &Gbl.tz))
      // Error in gettimeofday
      Gbl.TimeGenerationInMicroseconds = 0;
   else
     {
      if (Gbl.tvPageCreated.tv_usec < Gbl.tvStart.tv_usec)
	{
	 Gbl.tvPageCreated.tv_sec--;
	 Gbl.tvPageCreated.tv_usec += 1000000;
	}
      Gbl.TimeGenerationInMicroseconds = (Gbl.tvPageCreated.tv_sec  - Gbl.tvStart.tv_sec) * 1000000L +
                                          Gbl.tvPageCreated.tv_usec - Gbl.tvStart.tv_usec;
     }
  }

/*****************************************************************************/
/****************** Compute the time used to send the page *******************/
/*****************************************************************************/

void Sta_ComputeTimeToSendPage (void)
  {
   if (gettimeofday (&Gbl.tvPageSent, &Gbl.tz))
      // Error in gettimeofday
      Gbl.TimeSendInMicroseconds = 0;
   else
     {
      if (Gbl.tvPageSent.tv_usec < Gbl.tvPageCreated.tv_usec)
	{
	 Gbl.tvPageSent.tv_sec--;
	 Gbl.tvPageSent.tv_usec += 1000000;
	}
      Gbl.TimeSendInMicroseconds = (Gbl.tvPageSent.tv_sec  - Gbl.tvPageCreated.tv_sec) * 1000000L +
                                    Gbl.tvPageSent.tv_usec - Gbl.tvPageCreated.tv_usec;
     }
  }

/*****************************************************************************/
/************** Write the time to generate and send the page *****************/
/*****************************************************************************/

void Sta_WriteTimeToGenerateAndSendPage (void)
  {
   extern const char *Txt_PAGE1_Page_generated_in;
   extern const char *Txt_PAGE2_and_sent_in;
   char StrTimeGenerationInMicroseconds[64];
   char StrTimeSendInMicroseconds[64];

   Sta_WriteTime (StrTimeGenerationInMicroseconds,Gbl.TimeGenerationInMicroseconds);
   Sta_WriteTime (StrTimeSendInMicroseconds,Gbl.TimeSendInMicroseconds);
   fprintf (Gbl.F.Out,"%s %s %s %s",
            Txt_PAGE1_Page_generated_in,StrTimeGenerationInMicroseconds,
            Txt_PAGE2_and_sent_in,StrTimeSendInMicroseconds);
  }

/*****************************************************************************/
/********* Write time (given in microseconds) depending on amount ************/
/*****************************************************************************/

void Sta_WriteTime (char *Str,long TimeInMicroseconds)
  {
   if (TimeInMicroseconds < 1000L)
      sprintf (Str,"%ld &micro;s",TimeInMicroseconds);
   else if (TimeInMicroseconds < 1000000L)
      sprintf (Str,"%ld ms",TimeInMicroseconds / 1000);
   else if (TimeInMicroseconds < (60 * 1000000L))
      sprintf (Str,"%.1f s",(float) TimeInMicroseconds / 1E6);
   else
      sprintf (Str,"%ld min, %ld s",
               TimeInMicroseconds / (60 * 1000000L),
               (TimeInMicroseconds / 1000000L) % 60);
  }
