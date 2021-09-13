// swad_statistic.c: statistics

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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
#include <math.h>		// For log10, floor, ceil, modf, sqrt...
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For getenv, malloc
#include <string.h>		// For string functions

#include "swad_banner.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_log.h"
#include "swad_profile.h"
#include "swad_role.h"
#include "swad_statistic.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const unsigned Sta_CellPadding[Sta_NUM_CLICKS_GROUPED_BY] =
  {
   [Sta_CLICKS_CRS_DETAILED_LIST   ] = 2,

   [Sta_CLICKS_CRS_PER_USR         ] = 1,
   [Sta_CLICKS_CRS_PER_DAY         ] = 1,
   [Sta_CLICKS_CRS_PER_DAY_AND_HOUR] = 0,
   [Sta_CLICKS_CRS_PER_WEEK        ] = 1,
   [Sta_CLICKS_CRS_PER_MONTH       ] = 1,
   [Sta_CLICKS_CRS_PER_YEAR        ] = 1,
   [Sta_CLICKS_CRS_PER_HOUR        ] = 1,
   [Sta_CLICKS_CRS_PER_MINUTE      ] = 0,
   [Sta_CLICKS_CRS_PER_ACTION      ] = 1,

   [Sta_CLICKS_GBL_PER_DAY         ] = 1,
   [Sta_CLICKS_GBL_PER_DAY_AND_HOUR] = 0,
   [Sta_CLICKS_GBL_PER_WEEK        ] = 1,
   [Sta_CLICKS_GBL_PER_MONTH       ] = 1,
   [Sta_CLICKS_GBL_PER_YEAR        ] = 1,
   [Sta_CLICKS_GBL_PER_HOUR        ] = 1,
   [Sta_CLICKS_GBL_PER_MINUTE      ] = 0,
   [Sta_CLICKS_GBL_PER_ACTION      ] = 1,
   [Sta_CLICKS_GBL_PER_PLUGIN      ] = 1,
   [Sta_CLICKS_GBL_PER_API_FUNCTION] = 1,
   [Sta_CLICKS_GBL_PER_BANNER      ] = 1,
   [Sta_CLICKS_GBL_PER_COUNTRY     ] = 1,
   [Sta_CLICKS_GBL_PER_INSTITUTION ] = 1,
   [Sta_CLICKS_GBL_PER_CENTER      ] = 1,
   [Sta_CLICKS_GBL_PER_DEGREE      ] = 1,
   [Sta_CLICKS_GBL_PER_COURSE      ] = 1,
  };

#define Sta_STAT_RESULTS_SECTION_ID	"stat_results"

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

typedef enum
  {
   Sta_SHOW_GLOBAL_ACCESSES,
   Sta_SHOW_COURSE_ACCESSES,
  } Sta_GlobalOrCourseAccesses_t;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Sta_PutFormCrsHits (struct Sta_Stats *Stats);
static void Sta_PutFormGblHits (struct Sta_Stats *Stats);

static void Sta_WriteSelectorCountType (const struct Sta_Stats *Stats);
static void Sta_WriteSelectorAction (const struct Sta_Stats *Stats);
static void Sta_ShowHits (Sta_GlobalOrCourseAccesses_t GlobalOrCourse);
static void Sta_ShowDetailedAccessesList (const struct Sta_Stats *Stats,
                                          unsigned NumHits,
                                          MYSQL_RES *mysql_res);
static void Sta_WriteLogComments (long LogCod);
static void Sta_ShowNumHitsPerUsr (Sta_CountType_t CountType,
                                   unsigned NumHits,
                                   MYSQL_RES *mysql_res);
static void Sta_ShowNumHitsPerDay (Sta_CountType_t CountType,
                                   unsigned NumHits,
                                   MYSQL_RES *mysql_res);
static void Sta_ShowDistrAccessesPerDayAndHour (const struct Sta_Stats *Stats,
                                                unsigned NumHits,
                                                MYSQL_RES *mysql_res);
static void Sta_PutHiddenParamScopeSta (void);
static Sta_ColorType_t Sta_GetStatColorType (void);
static void Sta_DrawBarColors (Sta_ColorType_t ColorType,double HitsMax);
static void Sta_DrawAccessesPerHourForADay (Sta_ColorType_t ColorType,double HitsNum[24],double HitsMax);
static void Sta_SetColor (Sta_ColorType_t ColorType,double HitsNum,double HitsMax,
                          unsigned *R,unsigned *G,unsigned *B);
static void Sta_ShowNumHitsPerWeek (Sta_CountType_t CountType,
                                    unsigned NumHits,
                                    MYSQL_RES *mysql_res);
static void Sta_ShowNumHitsPerMonth (Sta_CountType_t CountType,
                                     unsigned NumHits,
                                     MYSQL_RES *mysql_res);
static void Sta_ShowNumHitsPerYear (Sta_CountType_t CountType,
                                    unsigned NumHits,
                                    MYSQL_RES *mysql_res);
static void Sta_ShowNumHitsPerHour (unsigned NumHits,
                                    MYSQL_RES *mysql_res);
static void Sta_WriteAccessHour (unsigned Hour,struct Sta_Hits *Hits,unsigned ColumnWidth);
static void Sta_ShowAverageAccessesPerMinute (unsigned NumHits,MYSQL_RES *mysql_res);
static void Sta_WriteLabelsXAxisAccMin (double IncX,const char *Format);
static void Sta_WriteAccessMinute (unsigned Minute,double HitsNum,double MaxX);
static void Sta_ShowNumHitsPerAction (Sta_CountType_t CountTypes,
                                      unsigned NumHits,
                                      MYSQL_RES *mysql_res);
static void Sta_ShowNumHitsPerPlugin (Sta_CountType_t CountType,
                                      unsigned NumHits,
                                      MYSQL_RES *mysql_res);
static void Sta_ShowNumHitsPerWSFunction (Sta_CountType_t CountType,
                                          unsigned NumHits,
                                          MYSQL_RES *mysql_res);
static void Sta_ShowNumHitsPerBanner (Sta_CountType_t CountType,
                                      unsigned NumHits,
                                      MYSQL_RES *mysql_res);
static void Sta_ShowNumHitsPerCountry (Sta_CountType_t CountType,
                                       unsigned NumHits,
                                       MYSQL_RES *mysql_res);
static void Sta_WriteCountry (long CtyCod);
static void Sta_ShowNumHitsPerInstitution (Sta_CountType_t CountType,
                                           unsigned NumHits,
                                           MYSQL_RES *mysql_res);
static void Sta_WriteInstit (long InsCod);
static void Sta_ShowNumHitsPerCenter (Sta_CountType_t CountType,
                                      unsigned NumHits,
                                      MYSQL_RES *mysql_res);
static void Sta_WriteCenter (long CtrCod);
static void Sta_ShowNumHitsPerDegree (Sta_CountType_t CountType,
                                      unsigned NumHits,
                                      MYSQL_RES *mysql_res);
static void Sta_WriteDegree (long DegCod);
static void Sta_ShowNumHitsPerCourse (Sta_CountType_t CountType,
                                      unsigned NumHits,
                                      MYSQL_RES *mysql_res);

static void Sta_DrawBarNumHits (char Color,
				double HitsNum,double HitsMax,double HitsTotal,
				unsigned MaxBarWidth);

/*****************************************************************************/
/**************************** Reset stats context ****************************/
/*****************************************************************************/

void Sta_ResetStats (struct Sta_Stats *Stats)
  {
   Stats->ClicksGroupedBy = Sta_CLICKS_GROUPED_BY_DEFAULT;
   Stats->Role            = Sta_ROLE_DEFAULT;
   Stats->CountType       = Sta_COUNT_TYPE_DEFAULT;
   Stats->NumAction       = Sta_NUM_ACTION_DEFAULT;
   Stats->FirstRow        = 0;
   Stats->LastRow         = 0;
   Stats->RowsPerPage     = Sta_DEF_ROWS_PER_PAGE;
  }

/*****************************************************************************/
/**************** Read CGI environment variable REMOTE_ADDR ******************/
/*****************************************************************************/
/*
CGI Environment Variables:
REMOTE_ADDR
The IP address of the remote host making the request.
*/
void Sta_GetRemoteAddr (void)
  {
   if (getenv ("REMOTE_ADDR"))
      Str_Copy (Gbl.IP,getenv ("REMOTE_ADDR"),sizeof (Gbl.IP) - 1);
   else
      Gbl.IP[0] = '\0';
  }

/*****************************************************************************/
/******************** Show a form to make a query of clicks ******************/
/*****************************************************************************/

void Sta_AskShowCrsHits (void)
  {
   struct Sta_Stats Stats;

   /***** Reset stats context *****/
   Sta_ResetStats (&Stats);

   /***** Show form to select global hits *****/
   Sta_PutFormCrsHits (&Stats);
  }

static void Sta_PutFormCrsHits (struct Sta_Stats *Stats)
  {
   extern const char *Hlp_ANALYTICS_Visits_visits_to_course;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Statistics_of_visits_to_the_course_X;
   extern const char *Txt_Users;
   extern const char *Txt_Show;
   extern const char *Txt_distributed_by;
   extern const char *Txt_STAT_CLICKS_GROUPED_BY[Sta_NUM_CLICKS_GROUPED_BY];
   extern const char *Txt_results_per_page;
   extern const char *Txt_Show_hits;
   extern const char *Txt_No_teachers_or_students_found;
   static unsigned RowsPerPage[] =
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
   Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME];
   unsigned NumTotalUsrs;
   Sta_ClicksGroupedBy_t ClicksGroupedBy;
   unsigned ClicksGroupedByUnsigned;
   size_t i;

   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
   Sta_PutLinkToGlobalHits ();	// Global hits
   Log_PutLinkToLastClicks ();	// Last clicks in real time
   Mnu_ContextMenuEnd ();

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Get groups to show ******/
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Get and order the lists of users of this course *****/
   Usr_GetListUsrs (HieLvl_CRS,Rol_STD);
   Usr_GetListUsrs (HieLvl_CRS,Rol_NET);
   Usr_GetListUsrs (HieLvl_CRS,Rol_TCH);
   NumTotalUsrs = Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs +
	          Gbl.Usrs.LstUsrs[Rol_NET].NumUsrs +
	          Gbl.Usrs.LstUsrs[Rol_TCH].NumUsrs;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Str_BuildStringStr (Txt_Statistics_of_visits_to_the_course_X,
				          Gbl.Hierarchy.Crs.ShrtName),
                 NULL,NULL,
                 Hlp_ANALYTICS_Visits_visits_to_course,Box_NOT_CLOSABLE);
   Str_FreeString ();

      /***** Show form to select the groups *****/
      Grp_ShowFormToSelectSeveralGroups (NULL,NULL,
					 Grp_MY_GROUPS);

      /***** Begin section with user list *****/
      HTM_SECTION_Begin (Usr_USER_LIST_SECTION_ID);

	 if (NumTotalUsrs)
	   {
	    if (Usr_GetIfShowBigList (NumTotalUsrs,
				      NULL,NULL,
				      NULL))
	      {
	       /***** Form to select type of list used for select several users *****/
	       Usr_ShowFormsToSelectUsrListType (NULL,NULL);

	       /***** Put link to register students *****/
	       Enr_CheckStdsAndPutButtonToRegisterStdsInCurrentCrs ();

	       /***** Begin form *****/
	       Frm_BeginFormAnchor (ActSeeAccCrs,Sta_STAT_RESULTS_SECTION_ID);

	       Grp_PutParamsCodGrps ();
	       Par_PutHiddenParamLong (NULL,"FirstRow",0);
	       Par_PutHiddenParamLong (NULL,"LastRow",0);

		  /***** Put list of users to select some of them *****/
		  HTM_TABLE_BeginCenterPadding (2);

		     HTM_TR_Begin (NULL);

			HTM_TD_Begin ("class=\"RT %s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
			   HTM_TxtColon (Txt_Users);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"%s LT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
			   HTM_TABLE_Begin (NULL);
			      Usr_ListUsersToSelect (Rol_TCH,&Gbl.Usrs.Selected);
			      Usr_ListUsersToSelect (Rol_NET,&Gbl.Usrs.Selected);
			      Usr_ListUsersToSelect (Rol_STD,&Gbl.Usrs.Selected);
			   HTM_TABLE_End ();
			HTM_TD_End ();

		     HTM_TR_End ();

		     /***** Initial and final dates of the search *****/
		     if (Gbl.Action.Act == ActReqAccCrs)
		       {
			SetHMS[Dat_START_TIME] = Dat_HMS_TO_000000;
			SetHMS[Dat_END_TIME  ] = Dat_HMS_TO_235959;
		       }
		     else
		       {
			SetHMS[Dat_START_TIME] = Dat_HMS_DO_NOT_SET;
			SetHMS[Dat_END_TIME  ] = Dat_HMS_DO_NOT_SET;
		       }
		     Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (SetHMS);

		     /***** Selection of action *****/
		     Sta_WriteSelectorAction (Stats);

		     /***** Option a) Listing of clicks distributed by some metric *****/
		     HTM_TR_Begin (NULL);

			HTM_TD_Begin ("class=\"RM %s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
			   HTM_TxtColon (Txt_Show);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM\"");

			   if ((Stats->ClicksGroupedBy < Sta_CLICKS_CRS_PER_USR ||
				Stats->ClicksGroupedBy > Sta_CLICKS_CRS_PER_ACTION) &&
				Stats->ClicksGroupedBy != Sta_CLICKS_CRS_DETAILED_LIST)
			      Stats->ClicksGroupedBy = Sta_CLICKS_GROUPED_BY_DEFAULT;

			   HTM_INPUT_RADIO ("GroupedOrDetailed",false,
					    "value=\"%u\"%s onclick=\"disableDetailedClicks();\"",
					    (unsigned) Sta_CLICKS_GROUPED,
					    Stats->ClicksGroupedBy == Sta_CLICKS_CRS_DETAILED_LIST ? "" :
												     " checked=\"checked\"");

			   /* Selection of count type (number of pages generated, accesses per user, etc.) */
			   Sta_WriteSelectorCountType (Stats);

			   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
			      HTM_TxtF ("&nbsp;%s&nbsp;",Txt_distributed_by);
			      HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
						"id=\"GroupedBy\" name=\"GroupedBy\"");
				 for (ClicksGroupedBy = Sta_CLICKS_CRS_PER_USR;
				      ClicksGroupedBy <= Sta_CLICKS_CRS_PER_ACTION;
				      ClicksGroupedBy++)
				   {
				    ClicksGroupedByUnsigned = (unsigned) ClicksGroupedBy;
				    HTM_OPTION (HTM_Type_UNSIGNED,&ClicksGroupedByUnsigned,
						ClicksGroupedBy == Stats->ClicksGroupedBy,false,
						"%s",Txt_STAT_CLICKS_GROUPED_BY[ClicksGroupedBy]);
				   }
			      HTM_SELECT_End ();
			   HTM_LABEL_End ();

			   /***** Separator *****/
			   HTM_BR ();

			   /***** Option b) Listing of detailed clicks to this course *****/
			   HTM_LABEL_Begin (NULL);
			      HTM_INPUT_RADIO ("GroupedOrDetailed",false,
					       "value=\"%u\"%s onclick=\"enableDetailedClicks();\"",
					       (unsigned) Sta_CLICKS_DETAILED,
					       Stats->ClicksGroupedBy == Sta_CLICKS_CRS_DETAILED_LIST ? " checked=\"checked\"" :
													"");
			      HTM_Txt (Txt_STAT_CLICKS_GROUPED_BY[Sta_CLICKS_CRS_DETAILED_LIST]);
			   HTM_LABEL_End ();

			   /* Separator */
			   HTM_Txt (" ");

			   /* Number of rows per page */
			   // To use getElementById in Firefox, it's necessary to have the id attribute
			   HTM_LABEL_Begin (NULL);
			      HTM_TxtF ("(%s: ",Txt_results_per_page);
			      HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
						"id=\"RowsPage\" name=\"RowsPage\"%s",
						Stats->ClicksGroupedBy == Sta_CLICKS_CRS_DETAILED_LIST ? "" :
													 " disabled=\"disabled\"");
				 for (i = 0;
				      i < NUM_OPTIONS_ROWS_PER_PAGE;
				      i++)
				    HTM_OPTION (HTM_Type_UNSIGNED,&RowsPerPage[i],
						RowsPerPage[i] == Stats->RowsPerPage,false,
						"%u",RowsPerPage[i]);
			      HTM_SELECT_End ();
			      HTM_Txt (")");
			   HTM_LABEL_End ();

			HTM_TD_End ();

		     HTM_TR_End ();

		  HTM_TABLE_End ();

		  /***** Hidden param used to get client time zone *****/
		  Dat_PutHiddenParBrowserTZDiff ();

		  /***** Send button *****/
		  Btn_PutConfirmButton (Txt_Show_hits);

	       /***** End form *****/
	       Frm_EndForm ();
	      }
	   }
	 else	// No teachers nor students found
	    Ale_ShowAlert (Ale_WARNING,Txt_No_teachers_or_students_found);

      /***** End section with user list *****/
      HTM_SECTION_End ();

   /***** End box *****/
   Box_BoxEnd ();

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
   struct Sta_Stats Stats;

   /***** Reset stats context *****/
   Sta_ResetStats (&Stats);

   /***** Show form to select global hits *****/
   Sta_PutFormGblHits (&Stats);
  }

static void Sta_PutFormGblHits (struct Sta_Stats *Stats)
  {
   extern const char *Hlp_ANALYTICS_Visits_global_visits;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Statistics_of_all_visits;
   extern const char *Txt_Users;
   extern const char *Txt_ROLE_STATS[Sta_NUM_ROLES_STAT];
   extern const char *Txt_Scope;
   extern const char *Txt_Show;
   extern const char *Txt_distributed_by;
   extern const char *Txt_STAT_CLICKS_GROUPED_BY[Sta_NUM_CLICKS_GROUPED_BY];
   extern const char *Txt_Show_hits;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_START_TIME] = Dat_HMS_TO_000000,
      [Dat_END_TIME  ] = Dat_HMS_TO_235959
     };
   Sta_Role_t RoleStat;
   unsigned RoleStatUnsigned;
   Sta_ClicksGroupedBy_t ClicksGroupedBy;
   unsigned ClicksGroupedByUnsigned;

   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
   Sta_PutLinkToCourseHits ();	// Course hits
   Log_PutLinkToLastClicks ();	// Last clicks in real time
   Mnu_ContextMenuEnd ();

   /***** Begin form *****/
   Frm_BeginFormAnchor (ActSeeAccGbl,Sta_STAT_RESULTS_SECTION_ID);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_Statistics_of_all_visits,
                      NULL,NULL,
                      Hlp_ANALYTICS_Visits_global_visits,Box_NOT_CLOSABLE,2);

   /***** Start and end dates for the search *****/
   Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (SetHMS);

   /***** Users' roles whose accesses we want to see *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","Role",Txt_Users);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "id=\"Role\" name=\"Role\" class=\"STAT_SEL\"");
   for (RoleStat  = (Sta_Role_t) 0;
	RoleStat <= (Sta_Role_t) (Sta_NUM_ROLES_STAT - 1);
	RoleStat++)
     {
      RoleStatUnsigned = (unsigned) RoleStat;
      HTM_OPTION (HTM_Type_UNSIGNED,&RoleStatUnsigned,
		  RoleStat == Stats->Role,false,
		  "%s",Txt_ROLE_STATS[RoleStat]);
     }
   HTM_SELECT_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Selection of action *****/
   Sta_WriteSelectorAction (Stats);

   /***** Clicks made from anywhere, current center, current degree or current course *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","ScopeSta",Txt_Scope);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   Gbl.Scope.Allowed = 1 << HieLvl_SYS |
	               1 << HieLvl_CTY |
		       1 << HieLvl_INS |
		       1 << HieLvl_CTR |
		       1 << HieLvl_DEG |
		       1 << HieLvl_CRS;
   Gbl.Scope.Default = HieLvl_SYS;
   Sco_GetScope ("ScopeSta");
   Sco_PutSelectorScope ("ScopeSta",HTM_DONT_SUBMIT_ON_CHANGE);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Count type for the statistic *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","CountType",Txt_Show);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   Sta_WriteSelectorCountType (Stats);

   /***** Type of statistic *****/
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("&nbsp;%s&nbsp;",Txt_distributed_by);

   if (Stats->ClicksGroupedBy < Sta_CLICKS_GBL_PER_DAY ||
       Stats->ClicksGroupedBy > Sta_CLICKS_GBL_PER_COURSE)
      Stats->ClicksGroupedBy = Sta_CLICKS_GBL_PER_DAY;

   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "name=\"GroupedBy\"");
   for (ClicksGroupedBy = Sta_CLICKS_GBL_PER_DAY;
	ClicksGroupedBy <= Sta_CLICKS_GBL_PER_COURSE;
	ClicksGroupedBy++)
     {
      ClicksGroupedByUnsigned = (unsigned) ClicksGroupedBy;
      HTM_OPTION (HTM_Type_UNSIGNED,&ClicksGroupedByUnsigned,
		  ClicksGroupedBy == Stats->ClicksGroupedBy,false,
		  "%s",Txt_STAT_CLICKS_GROUPED_BY[ClicksGroupedBy]);
     }
   HTM_SELECT_End ();
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Hidden param used to get client time zone *****/
   Dat_PutHiddenParBrowserTZDiff ();

   /***** Send button and end box *****/
   Box_BoxWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Show_hits);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************** Put a link to show visits to current course *****************/
/*****************************************************************************/

void Sta_PutLinkToCourseHits (void)
  {
   extern const char *Txt_Visits_to_course;

   if (Gbl.Hierarchy.Level == HieLvl_CRS)		// Course selected
      switch (Gbl.Usrs.Me.Role.Logged)
        {
	 case Rol_NET:
	 case Rol_TCH:
	 case Rol_SYS_ADM:
	    Lay_PutContextualLinkIconText (ActReqAccCrs,NULL,
	                                   NULL,NULL,
					   "chart-line.svg",
					   Txt_Visits_to_course);
	    break;
	 default:
	    break;
        }
  }

/*****************************************************************************/
/********************* Put a link to show global visits **********************/
/*****************************************************************************/

void Sta_PutLinkToGlobalHits (void)
  {
   extern const char *Txt_Global_visits;

   Lay_PutContextualLinkIconText (ActReqAccGbl,NULL,
                                  NULL,NULL,
				  "chart-line.svg",
				  Txt_Global_visits);
  }

/*****************************************************************************/
/****** Put selectors for type of access count and for degree or course ******/
/*****************************************************************************/

static void Sta_WriteSelectorCountType (const struct Sta_Stats *Stats)
  {
   extern const char *Txt_STAT_TYPE_COUNT_SMALL[Sta_NUM_COUNT_TYPES];
   Sta_CountType_t StatCountType;
   unsigned StatCountTypeUnsigned;

   /**** Count type *****/
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "id=\"CountType\" name=\"CountType\"");
   for (StatCountType  = (Sta_CountType_t) 0;
	StatCountType <= (Sta_CountType_t) (Sta_NUM_COUNT_TYPES - 1);
	StatCountType++)
     {
      StatCountTypeUnsigned = (unsigned) StatCountType;
      HTM_OPTION (HTM_Type_UNSIGNED,&StatCountTypeUnsigned,
		  StatCountType == Stats->CountType,false,
		  "%s",Txt_STAT_TYPE_COUNT_SMALL[StatCountType]);
     }
   HTM_SELECT_End ();
  }

/*****************************************************************************/
/******************** Put selector for type of action ************************/
/*****************************************************************************/

static void Sta_WriteSelectorAction (const struct Sta_Stats *Stats)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Action;
   extern const char *Txt_Any_action;
   extern const char *Txt_TABS_TXT[Tab_NUM_TABS];
   Act_Action_t Action;
   unsigned ActionUnsigned;
   Tab_Tab_t Tab;

   /***** Action *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","StatAct",Txt_Action);

   HTM_TD_Begin ("class=\"LT\"");
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "id=\"StatAct\" name=\"StatAct\" class=\"STAT_SEL\"");
   HTM_OPTION (HTM_Type_STRING,"0",Stats->NumAction == 0,false,
	       "%s",Txt_Any_action);
   for (Action  = (Act_Action_t) 1;
	Action <= (Act_Action_t) (Act_NUM_ACTIONS - 1);
	Action++)
     {
      Tab = Act_GetTab (Act_GetSuperAction (Action));
      ActionUnsigned = (unsigned) Action;
      HTM_OPTION (HTM_Type_UNSIGNED,&ActionUnsigned,
		  Action == Stats->NumAction,false,
		  "%u: %s &gt; %s",
		  (unsigned) Action,Txt_TABS_TXT[Tab],Act_GetActionText (Action));
     }
   HTM_SELECT_End ();
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************ Set end date to current date                        ************/
/************ and set initial date to end date minus several days ************/
/*****************************************************************************/

void Sta_SetIniEndDates (void)
  {
   Gbl.DateRange.TimeUTC[Dat_START_TIME] = Gbl.StartExecutionTimeUTC - ((Cfg_DAYS_IN_RECENT_LOG - 1) * 24 * 60 * 60);
   Gbl.DateRange.TimeUTC[Dat_END_TIME  ] = Gbl.StartExecutionTimeUTC;
  }

/*****************************************************************************/
/******************** Compute and show access statistics *********************/
/*****************************************************************************/

void Sta_SeeGblAccesses (void)
  {
   /***** Show hits *****/
   Sta_ShowHits (Sta_SHOW_GLOBAL_ACCESSES);
  }

void Sta_SeeCrsAccesses (void)
  {
   /***** Show hits *****/
   Sta_ShowHits (Sta_SHOW_COURSE_ACCESSES);
  }

/*****************************************************************************/
/******************** Compute and show access statistics ********************/
/*****************************************************************************/

#define Sta_MAX_BYTES_QUERY_ACCESS (1024 + (10 + ID_MAX_BYTES_USR_ID) * 5000 - 1)

#define Sta_MAX_BYTES_COUNT_TYPE (256 - 1)

static void Sta_ShowHits (Sta_GlobalOrCourseAccesses_t GlobalOrCourse)
  {
   extern const char *Txt_You_must_select_one_ore_more_users;
   extern const char *Txt_There_is_no_knowing_how_many_users_not_logged_have_accessed;
   extern const char *Txt_The_date_range_must_be_less_than_or_equal_to_X_days;
   extern const char *Txt_There_are_no_accesses_with_the_selected_search_criteria;
   extern const char *Txt_List_of_detailed_clicks;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   extern const char *Txt_Time_zone_used_in_the_calculation_of_these_statistics;
   struct Sta_Stats Stats;
   char *Query = NULL;
   char QueryAux[512];
   long LengthQuery;
   MYSQL_RES *mysql_res;
   unsigned NumHits;
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

   /***** Reset stats context *****/
   Sta_ResetStats (&Stats);

   /***** Get initial and ending dates *****/
   Dat_GetIniEndDatesFromForm ();

   /***** Get client time zone *****/
   Dat_GetBrowserTimeZone (BrowserTimeZone);

   /***** Set table where to find depending on initial date *****/
   /* If initial day is older than current day minus Cfg_DAYS_IN_RECENT_LOG,
      then use recent log table, else use historic log table */
   LogTable = (Dat_GetNumDaysBetweenDates (&Gbl.DateRange.DateIni.Date,&Gbl.Now.Date)
	       <= Cfg_DAYS_IN_RECENT_LOG) ? "log_recent" :
	                                    "log";

   /***** Get the type of stat of clicks ******/
   DetailedOrGrouped = (Sta_ClicksDetailedOrGrouped_t)
	               Par_GetParToUnsignedLong ("GroupedOrDetailed",
	                                         0,
	                                         Sta_NUM_CLICKS_DETAILED_OR_GROUPED - 1,
	                                         (unsigned long) Sta_CLICKS_DETAILED_OR_GROUPED_DEFAULT);

   if (DetailedOrGrouped == Sta_CLICKS_DETAILED)
      Stats.ClicksGroupedBy = Sta_CLICKS_CRS_DETAILED_LIST;
   else	// DetailedOrGrouped == Sta_CLICKS_GROUPED
      Stats.ClicksGroupedBy = (Sta_ClicksGroupedBy_t)
			      Par_GetParToUnsignedLong ("GroupedBy",
							0,
							Sta_NUM_CLICKS_GROUPED_BY - 1,
							(unsigned long) Sta_CLICKS_GROUPED_BY_DEFAULT);

   /***** Get the type of count of clicks *****/
   if (Stats.ClicksGroupedBy != Sta_CLICKS_CRS_DETAILED_LIST)
      Stats.CountType = (Sta_CountType_t)
			Par_GetParToUnsignedLong ("CountType",
						  0,
						  Sta_NUM_COUNT_TYPES - 1,
						  (unsigned long) Sta_COUNT_TYPE_DEFAULT);

   /***** Get action *****/
   Stats.NumAction = (Act_Action_t)
		     Par_GetParToUnsignedLong ("StatAct",
					       0,
					       Act_NUM_ACTIONS - 1,
					       (unsigned long) Sta_NUM_ACTION_DEFAULT);

   switch (GlobalOrCourse)
     {
      case Sta_SHOW_GLOBAL_ACCESSES:
	 /***** Get the type of user of clicks *****/
	 Stats.Role = (Sta_Role_t)
		      Par_GetParToUnsignedLong ("Role",
						0,
						Sta_NUM_ROLES_STAT - 1,
						(unsigned long) Sta_ROLE_DEFAULT);

	 /***** Get users range for access statistics *****/
	 Gbl.Scope.Allowed = 1 << HieLvl_SYS |
			     1 << HieLvl_CTY |
			     1 << HieLvl_INS |
			     1 << HieLvl_CTR |
			     1 << HieLvl_DEG |
			     1 << HieLvl_CRS;
	 Gbl.Scope.Default = HieLvl_SYS;
	 Sco_GetScope ("ScopeSta");

	 /***** Show form again *****/
	 Sta_PutFormGblHits (&Stats);

	 /***** Begin results section *****/
	 HTM_SECTION_Begin (Sta_STAT_RESULTS_SECTION_ID);

	 /***** Check selection *****/
	 if ((Stats.Role == Sta_ROLE_ALL_USRS ||
	      Stats.Role == Sta_ROLE_UNKNOWN_USRS) &&
	     (Stats.CountType == Sta_DISTINCT_USRS ||
	      Stats.CountType == Sta_CLICKS_PER_USR))	// These types of query will never give a valid result
	   {
	    /* Write warning message and abort */
	    Ale_ShowAlert (Ale_WARNING,Txt_There_is_no_knowing_how_many_users_not_logged_have_accessed);
	    return;
	   }
	 break;
      case Sta_SHOW_COURSE_ACCESSES:
	 if (Stats.ClicksGroupedBy == Sta_CLICKS_CRS_DETAILED_LIST)
	   {
	    /****** Get the number of the first row to show ******/
	    Stats.FirstRow = Par_GetParToUnsignedLong ("FirstRow",
						       1,
						       ULONG_MAX,
						       0);

	    /****** Get the number of the last row to show ******/
	    Stats.LastRow = Par_GetParToUnsignedLong ("LastRow",
						      1,
						      ULONG_MAX,
						      0);

	    /****** Get the number of rows per page ******/
	    Stats.RowsPerPage =
	    (unsigned) Par_GetParToUnsignedLong ("RowsPage",
	                                         Sta_MIN_ROWS_PER_PAGE,
	                                         Sta_MAX_ROWS_PER_PAGE,
	                                         Sta_DEF_ROWS_PER_PAGE);
	   }

	 /****** Get lists of selected users ******/
	 Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

	 /***** Show the form again *****/
	 Sta_PutFormCrsHits (&Stats);

	 /***** Begin results section *****/
	 HTM_SECTION_Begin (Sta_STAT_RESULTS_SECTION_ID);

	 /***** Check selection *****/
	 if (!Usr_CheckIfThereAreUsrsInListOfSelectedEncryptedUsrCods (&Gbl.Usrs.Selected))	// Error: there are no users selected
	   {
	    /* Write warning message, clean and abort */
	    Ale_ShowAlert (Ale_WARNING,Txt_You_must_select_one_ore_more_users);
            Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
	    return;
	   }
	 break;
     }

   /***** Check if range of dates is forbidden for me *****/
   NumDays = Dat_GetNumDaysBetweenDates (&Gbl.DateRange.DateIni.Date,&Gbl.DateRange.DateEnd.Date);
   ICanQueryWholeRange = (Gbl.Usrs.Me.Role.Logged >= Rol_TCH && GlobalOrCourse == Sta_SHOW_COURSE_ACCESSES) ||
			 (Gbl.Usrs.Me.Role.Logged == Rol_TCH     &&  Gbl.Scope.Current == HieLvl_CRS)  ||
			 (Gbl.Usrs.Me.Role.Logged == Rol_DEG_ADM && (Gbl.Scope.Current == HieLvl_DEG   ||
			                                             Gbl.Scope.Current == HieLvl_CRS)) ||
			 (Gbl.Usrs.Me.Role.Logged == Rol_CTR_ADM && (Gbl.Scope.Current == HieLvl_CTR   ||
			                                             Gbl.Scope.Current == HieLvl_DEG   ||
			                                             Gbl.Scope.Current == HieLvl_CRS)) ||
			 (Gbl.Usrs.Me.Role.Logged == Rol_INS_ADM && (Gbl.Scope.Current == HieLvl_INS   ||
			                                             Gbl.Scope.Current == HieLvl_CTR   ||
			                                             Gbl.Scope.Current == HieLvl_DEG   ||
			                                             Gbl.Scope.Current == HieLvl_CRS)) ||
			  Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM;
   if (!ICanQueryWholeRange && NumDays > Cfg_DAYS_IN_RECENT_LOG)
     {
      /* ...write warning message and show the form again */
      Ale_ShowAlert (Ale_WARNING,Txt_The_date_range_must_be_less_than_or_equal_to_X_days,
	             Cfg_DAYS_IN_RECENT_LOG);
      return;
     }

   /***** Query depending on the type of count *****/
   switch (Stats.CountType)
     {
      case Sta_TOTAL_CLICKS:
         Str_Copy (StrQueryCountType,"COUNT(*)",sizeof (StrQueryCountType) - 1);
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
   /* Allocate memory for the query */
   if ((Query = malloc (Sta_MAX_BYTES_QUERY_ACCESS + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /* Start the query */
   switch (Stats.ClicksGroupedBy)
     {
      case Sta_CLICKS_CRS_DETAILED_LIST:
   	 snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE LogCod,"
   	                               "UsrCod,"
   	                               "Role,"
   		                       "UNIX_TIMESTAMP(ClickTime) AS F,"
   		                       "ActCod"
   		    " FROM %s",
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_USR:
	 snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE UsrCod,"
   	                               "%s AS Num"
   	            " FROM %s",
                   StrQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_DAY:
      case Sta_CLICKS_GBL_PER_DAY:
         snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%Y%%m%%d') AS Day,"
                                       "%s"
                    " FROM %s",
                   BrowserTimeZone,
                   StrQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_DAY_AND_HOUR:
      case Sta_CLICKS_GBL_PER_DAY_AND_HOUR:
         snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%Y%%m%%d') AS Day,"
                                       "DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%H') AS Hour,"
                                       "%s"
                    " FROM %s",
                   BrowserTimeZone,
                   BrowserTimeZone,
                   StrQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_WEEK:
      case Sta_CLICKS_GBL_PER_WEEK:
	 /* With %x%v the weeks are counted from monday to sunday.
	    With %X%V the weeks are counted from sunday to saturday. */
	 snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           (Gbl.Prefs.FirstDayOfWeek == 0) ?
	           "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%x%%v') AS Week,"// Weeks start on monday
		                       "%s"
		    " FROM %s" :
		   "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%X%%V') AS Week,"// Weeks start on sunday
		                       "%s"
		    " FROM %s",
		   BrowserTimeZone,
		   StrQueryCountType,
		   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_MONTH:
      case Sta_CLICKS_GBL_PER_MONTH:
         snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%Y%%m') AS Month,"
                                       "%s"
                    " FROM %s",
                   BrowserTimeZone,
                   StrQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_YEAR:
      case Sta_CLICKS_GBL_PER_YEAR:
         snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%Y') AS Year,"
                                       "%s"
                    " FROM %s",
                   BrowserTimeZone,
                   StrQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_HOUR:
      case Sta_CLICKS_GBL_PER_HOUR:
         snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%H') AS Hour,"
                                       "%s"
                   " FROM %s",
                   BrowserTimeZone,
                   StrQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_MINUTE:
      case Sta_CLICKS_GBL_PER_MINUTE:
         snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%H%%i') AS Minute,"
                                       "%s"
                    " FROM %s",
                   BrowserTimeZone,
                   StrQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_ACTION:
      case Sta_CLICKS_GBL_PER_ACTION:
         snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE ActCod,"
   	                               "%s AS Num"
   	            " FROM %s",
                   StrQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_GBL_PER_PLUGIN:
         snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE log_api.PlgCod,"
   	                                "%s AS Num"
   	            " FROM %s,"
   	                  "log_api",
                   StrQueryCountType,
                   LogTable);
         break;
      case Sta_CLICKS_GBL_PER_API_FUNCTION:
         snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE log_api.FunCod,"
   	                               "%s AS Num"
   	            " FROM %s,"
   	                  "log_api",
                   StrQueryCountType,
                   LogTable);
         break;
      case Sta_CLICKS_GBL_PER_BANNER:
         snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE log_banners.BanCod,"
   	                               "%s AS Num"
   	            " FROM %s,"
   	                  "log_banners",
                   StrQueryCountType,
                   LogTable);
         break;
      case Sta_CLICKS_GBL_PER_COUNTRY:
         snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE CtyCod,"
   	                               "%s AS Num"
   	            " FROM %s",
                   StrQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_GBL_PER_INSTITUTION:
         snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE InsCod,"
   	                               "%s AS Num"
   	            " FROM %s",
                   StrQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_GBL_PER_CENTER:
         snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE CtrCod,"
   	                               "%s AS Num"
   	            " FROM %s",
                   StrQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_GBL_PER_DEGREE:
         snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE DegCod,"
   	                               "%s AS Num"
   	            " FROM %s",
                   StrQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_GBL_PER_COURSE:
	 snprintf (Query,Sta_MAX_BYTES_QUERY_ACCESS + 1,
   	           "SELECT SQL_NO_CACHE CrsCod,"
   	                               "%s AS Num"
   	            " FROM %s",
                   StrQueryCountType,
                   LogTable);
	 break;
     }
   sprintf (QueryAux," WHERE %s.ClickTime"
	             " BETWEEN FROM_UNIXTIME(%ld)"
	                 " AND FROM_UNIXTIME(%ld)",
            LogTable,
            (long) Gbl.DateRange.TimeUTC[Dat_START_TIME],
            (long) Gbl.DateRange.TimeUTC[Dat_END_TIME  ]);
   Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);

   switch (GlobalOrCourse)
     {
      case Sta_SHOW_GLOBAL_ACCESSES:
	 /* Scope */
	 switch (Gbl.Scope.Current)
	   {
	    case HieLvl_UNK:
	    case HieLvl_SYS:
               break;
	    case HieLvl_CTY:
               if (Gbl.Hierarchy.Cty.CtyCod > 0)
		 {
		  sprintf (QueryAux," AND %s.CtyCod=%ld",
			   LogTable,Gbl.Hierarchy.Cty.CtyCod);
		  Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
		 }
               break;
	    case HieLvl_INS:
	       if (Gbl.Hierarchy.Ins.InsCod > 0)
		 {
		  sprintf (QueryAux," AND %s.InsCod=%ld",
			   LogTable,Gbl.Hierarchy.Ins.InsCod);
		  Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
		 }
	       break;
	    case HieLvl_CTR:
               if (Gbl.Hierarchy.Ctr.CtrCod > 0)
		 {
		  sprintf (QueryAux," AND %s.CtrCod=%ld",
			   LogTable,Gbl.Hierarchy.Ctr.CtrCod);
		  Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
		 }
               break;
	    case HieLvl_DEG:
	       if (Gbl.Hierarchy.Deg.DegCod > 0)
		 {
		  sprintf (QueryAux," AND %s.DegCod=%ld",
			   LogTable,Gbl.Hierarchy.Deg.DegCod);
		  Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
		 }
	       break;
	    case HieLvl_CRS:
	       if (Gbl.Hierarchy.Level == HieLvl_CRS)
		 {
		  sprintf (QueryAux," AND %s.CrsCod=%ld",
			   LogTable,Gbl.Hierarchy.Crs.CrsCod);
		  Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
		 }
	       break;
	   }

         /* Type of users */
	 switch (Stats.Role)
	   {
	    case Sta_ROLE_IDENTIFIED_USRS:
               sprintf (StrRole," AND %s.Role<>%u",
                        LogTable,(unsigned) Rol_UNK);
	       break;
	    case Sta_ROLE_ALL_USRS:
               switch (Stats.CountType)
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
         Str_Concat (Query,StrRole,Sta_MAX_BYTES_QUERY_ACCESS);

         switch (Stats.ClicksGroupedBy)
           {
            case Sta_CLICKS_GBL_PER_PLUGIN:
            case Sta_CLICKS_GBL_PER_API_FUNCTION:
               sprintf (QueryAux," AND %s.LogCod=log_api.LogCod",
                        LogTable);
               Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
               break;
            case Sta_CLICKS_GBL_PER_BANNER:
               sprintf (QueryAux," AND %s.LogCod=log_banners.LogCod",
                        LogTable);
               Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
               break;
            default:
               break;
           }
	 break;
      case Sta_SHOW_COURSE_ACCESSES:
         sprintf (QueryAux," AND %s.CrsCod=%ld",
                  LogTable,Gbl.Hierarchy.Crs.CrsCod);
	 Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);

	 /***** Initialize data structure of the user *****/
         Usr_UsrDataConstructor (&UsrDat);

	 LengthQuery = strlen (Query);
	 NumUsr = 0;
	 Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
	 while (*Ptr)
	   {
	    Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDat.EnUsrCod,
	                                       Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
            Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
	    if (UsrDat.UsrCod > 0)
	      {
	       LengthQuery = LengthQuery + 25 + 10 + 1;
	       if (LengthQuery > Sta_MAX_BYTES_QUERY_ACCESS - 128)
                  Err_ShowErrorAndExit ("Query is too large.");
               sprintf (QueryAux,
                        NumUsr ? " OR %s.UsrCod=%ld" :
                                 " AND (%s.UsrCod=%ld",
                        LogTable,UsrDat.UsrCod);
	       Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
	       NumUsr++;
	      }
	   }
	 Str_Concat (Query,")",Sta_MAX_BYTES_QUERY_ACCESS);

	 /***** Free memory used by the data of the user *****/
         Usr_UsrDataDestructor (&UsrDat);
	 break;
     }

   /* Select action */
   if (Stats.NumAction != ActAll)
     {
      sprintf (QueryAux," AND %s.ActCod=%ld",
               LogTable,Act_GetActCod (Stats.NumAction));
      Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
     }

   /* End the query */
   switch (Stats.ClicksGroupedBy)
     {
      case Sta_CLICKS_CRS_DETAILED_LIST:
	 Str_Concat (Query," ORDER BY F",
	             Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_USR:
	 sprintf (QueryAux," GROUP BY %s.UsrCod"
		           " ORDER BY Num DESC",
		  LogTable);
         Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_DAY:
      case Sta_CLICKS_GBL_PER_DAY:
	 Str_Concat (Query," GROUP BY Day"
		           " ORDER BY Day DESC",
		     Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_DAY_AND_HOUR:
      case Sta_CLICKS_GBL_PER_DAY_AND_HOUR:
	 Str_Concat (Query," GROUP BY Day,Hour"
		           " ORDER BY Day DESC,Hour",
		     Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_WEEK:
      case Sta_CLICKS_GBL_PER_WEEK:
	 Str_Concat (Query," GROUP BY Week"
		           " ORDER BY Week DESC",
		     Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_MONTH:
      case Sta_CLICKS_GBL_PER_MONTH:
	 Str_Concat (Query," GROUP BY Month"
		           " ORDER BY Month DESC",
		     Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_YEAR:
      case Sta_CLICKS_GBL_PER_YEAR:
	 Str_Concat (Query," GROUP BY Year"
		           " ORDER BY Year DESC",
		     Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_HOUR:
      case Sta_CLICKS_GBL_PER_HOUR:
	 Str_Concat (Query," GROUP BY Hour"
		           " ORDER BY Hour",
		     Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_MINUTE:
      case Sta_CLICKS_GBL_PER_MINUTE:
	 Str_Concat (Query," GROUP BY Minute"
		           " ORDER BY Minute",
		     Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_CRS_PER_ACTION:
      case Sta_CLICKS_GBL_PER_ACTION:
	 sprintf (QueryAux," GROUP BY %s.ActCod"
		           " ORDER BY Num DESC",
		  LogTable);
         Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_GBL_PER_PLUGIN:
         Str_Concat (Query," GROUP BY log_api.PlgCod"
        	           " ORDER BY Num DESC",
                     Sta_MAX_BYTES_QUERY_ACCESS);
         break;
      case Sta_CLICKS_GBL_PER_API_FUNCTION:
         Str_Concat (Query," GROUP BY log_api.FunCod"
        	           " ORDER BY Num DESC",
                     Sta_MAX_BYTES_QUERY_ACCESS);
         break;
      case Sta_CLICKS_GBL_PER_BANNER:
         Str_Concat (Query," GROUP BY log_banners.BanCod"
        	           " ORDER BY Num DESC",
                     Sta_MAX_BYTES_QUERY_ACCESS);
         break;
      case Sta_CLICKS_GBL_PER_COUNTRY:
	 sprintf (QueryAux," GROUP BY %s.CtyCod"
		           " ORDER BY Num DESC",
		  LogTable);
         Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_GBL_PER_INSTITUTION:
	 sprintf (QueryAux," GROUP BY %s.InsCod"
		           " ORDER BY Num DESC",
		  LogTable);
         Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_GBL_PER_CENTER:
	 sprintf (QueryAux," GROUP BY %s.CtrCod"
		           " ORDER BY Num DESC",
		  LogTable);
         Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_GBL_PER_DEGREE:
	 sprintf (QueryAux," GROUP BY %s.DegCod"
		           " ORDER BY Num DESC",
		  LogTable);
         Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
      case Sta_CLICKS_GBL_PER_COURSE:
	 sprintf (QueryAux," GROUP BY %s.CrsCod"
		           " ORDER BY Num DESC",
		  LogTable);
         Str_Concat (Query,QueryAux,Sta_MAX_BYTES_QUERY_ACCESS);
	 break;
     }
   /***** Write query for debug *****/
   /*
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      Ale_ShowAlert (Ale_INFO,Query);
   */

   /***** Make the query *****/
   NumHits = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get clicks",
		   "%s",
		   Query);

   /***** Count the number of rows in result *****/
   if (NumHits == 0)
      Ale_ShowAlert (Ale_INFO,Txt_There_are_no_accesses_with_the_selected_search_criteria);
   else
     {
      /***** Put the table with the clicks *****/
      if (Stats.ClicksGroupedBy == Sta_CLICKS_CRS_DETAILED_LIST)
	 Box_BoxBegin ("100%",Txt_List_of_detailed_clicks,
	               NULL,NULL,
	               NULL,Box_NOT_CLOSABLE);
      else
	 Box_BoxBegin (NULL,Txt_STAT_TYPE_COUNT_CAPS[Stats.CountType],
	               NULL,NULL,
	               NULL,Box_NOT_CLOSABLE);

      HTM_TABLE_BeginPadding (Sta_CellPadding[Stats.ClicksGroupedBy]);
      switch (Stats.ClicksGroupedBy)
	{
	 case Sta_CLICKS_CRS_DETAILED_LIST:
	    Sta_ShowDetailedAccessesList (&Stats,NumHits,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_USR:
	    Sta_ShowNumHitsPerUsr (Stats.CountType,NumHits,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_DAY:
	 case Sta_CLICKS_GBL_PER_DAY:
	    Sta_ShowNumHitsPerDay (Stats.CountType,NumHits,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_DAY_AND_HOUR:
	 case Sta_CLICKS_GBL_PER_DAY_AND_HOUR:
	    Sta_ShowDistrAccessesPerDayAndHour (&Stats,NumHits,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_WEEK:
	 case Sta_CLICKS_GBL_PER_WEEK:
	    Sta_ShowNumHitsPerWeek (Stats.CountType,NumHits,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_MONTH:
	 case Sta_CLICKS_GBL_PER_MONTH:
	    Sta_ShowNumHitsPerMonth (Stats.CountType,NumHits,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_YEAR:
	 case Sta_CLICKS_GBL_PER_YEAR:
	    Sta_ShowNumHitsPerYear (Stats.CountType,NumHits,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_HOUR:
	 case Sta_CLICKS_GBL_PER_HOUR:
	    Sta_ShowNumHitsPerHour (NumHits,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_MINUTE:
	 case Sta_CLICKS_GBL_PER_MINUTE:
	    Sta_ShowAverageAccessesPerMinute (NumHits,mysql_res);
	    break;
	 case Sta_CLICKS_CRS_PER_ACTION:
	 case Sta_CLICKS_GBL_PER_ACTION:
	    Sta_ShowNumHitsPerAction (Stats.CountType,NumHits,mysql_res);
	    break;
         case Sta_CLICKS_GBL_PER_PLUGIN:
            Sta_ShowNumHitsPerPlugin (Stats.CountType,NumHits,mysql_res);
            break;
         case Sta_CLICKS_GBL_PER_API_FUNCTION:
            Sta_ShowNumHitsPerWSFunction (Stats.CountType,NumHits,mysql_res);
            break;
         case Sta_CLICKS_GBL_PER_BANNER:
            Sta_ShowNumHitsPerBanner (Stats.CountType,NumHits,mysql_res);
            break;
         case Sta_CLICKS_GBL_PER_COUNTRY:
	    Sta_ShowNumHitsPerCountry (Stats.CountType,NumHits,mysql_res);
	    break;
         case Sta_CLICKS_GBL_PER_INSTITUTION:
	    Sta_ShowNumHitsPerInstitution (Stats.CountType,NumHits,mysql_res);
	    break;
         case Sta_CLICKS_GBL_PER_CENTER:
	    Sta_ShowNumHitsPerCenter (Stats.CountType,NumHits,mysql_res);
	    break;
	 case Sta_CLICKS_GBL_PER_DEGREE:
	    Sta_ShowNumHitsPerDegree (Stats.CountType,NumHits,mysql_res);
	    break;
	 case Sta_CLICKS_GBL_PER_COURSE:
	    Sta_ShowNumHitsPerCourse (Stats.CountType,NumHits,mysql_res);
	    break;
	}
      HTM_TABLE_End ();

      /* End box and section */
      Box_BoxEnd ();
      HTM_SECTION_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Free memory used by list of selected users' codes *****/
   if (Gbl.Action.Act == ActSeeAccCrs)
      Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

   /***** Write time zone used in the calculation of these statistics *****/
   switch (Stats.ClicksGroupedBy)
     {
      case Sta_CLICKS_CRS_PER_DAY:
      case Sta_CLICKS_GBL_PER_DAY:
      case Sta_CLICKS_CRS_PER_DAY_AND_HOUR:
      case Sta_CLICKS_GBL_PER_DAY_AND_HOUR:
      case Sta_CLICKS_CRS_PER_WEEK:
      case Sta_CLICKS_GBL_PER_WEEK:
      case Sta_CLICKS_CRS_PER_MONTH:
      case Sta_CLICKS_GBL_PER_MONTH:
      case Sta_CLICKS_CRS_PER_YEAR:
      case Sta_CLICKS_GBL_PER_YEAR:
      case Sta_CLICKS_CRS_PER_HOUR:
      case Sta_CLICKS_GBL_PER_HOUR:
      case Sta_CLICKS_CRS_PER_MINUTE:
      case Sta_CLICKS_GBL_PER_MINUTE:
	 HTM_TxtF ("<p class=\"DAT_SMALL CM\">%s: %s</p>",
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

static void Sta_ShowDetailedAccessesList (const struct Sta_Stats *Stats,
                                          unsigned NumHits,
                                          MYSQL_RES *mysql_res)
  {
   extern Act_Action_t Act_FromActCodToAction[1 + Act_MAX_ACTION_COD];
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
   extern const char *Txt_Role;
   extern const char *Txt_Date;
   extern const char *Txt_Action;
   extern const char *Txt_LOG_More_info;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   unsigned NumRow;
   unsigned FirstRow;	// First row to show
   unsigned LastRow;	// Last rows to show
   unsigned NumPagesBefore;
   unsigned NumPagesAfter;
   unsigned NumPagsTotal;
   struct UsrData UsrDat;
   MYSQL_ROW row;
   long LogCod;
   Rol_Role_t RoleFromLog;
   unsigned UniqueId;
   char *Id;
   long ActCod;

   /***** Initialize estructura of data of the user *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Compute the first and the last row to show *****/
   FirstRow = Stats->FirstRow;
   LastRow  = Stats->LastRow;
   if (FirstRow == 0 && LastRow == 0) // Call from main form
     {
      // Show last clicks
      FirstRow = (NumHits / Stats->RowsPerPage - 1) * Stats->RowsPerPage + 1;
      if ((FirstRow + Stats->RowsPerPage - 1) < NumHits)
	 FirstRow += Stats->RowsPerPage;
      LastRow = NumHits;
     }
   if (FirstRow < 1) // For security reasons; really it should never be less than 1
      FirstRow = 1;
   if (LastRow > NumHits)
      LastRow = NumHits;
   if ((LastRow - FirstRow) >= Stats->RowsPerPage) // For if there have been clicks that have increased the number of rows
      LastRow = FirstRow + Stats->RowsPerPage - 1;

   /***** Compute the number total of pages *****/
   /* Number of pages before the current one */
   NumPagesBefore = (FirstRow-1) / Stats->RowsPerPage;
   if (NumPagesBefore * Stats->RowsPerPage < (FirstRow-1))
      NumPagesBefore++;
   /* Number of pages after the current one */
   NumPagesAfter = (NumHits - LastRow) / Stats->RowsPerPage;
   if (NumPagesAfter * Stats->RowsPerPage < (NumHits - LastRow))
      NumPagesAfter++;
   /* Count the total number of pages */
   NumPagsTotal = NumPagesBefore + 1 + NumPagesAfter;

   /***** Put heading with backward and forward buttons *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("colspan=\"7\" class=\"LM\"");
   HTM_TABLE_BeginWidePadding (2);
   HTM_TR_Begin (NULL);

   /* Put link to jump to previous page (older clicks) */
   if (FirstRow > 1)
     {
      Frm_BeginFormAnchor (ActSeeAccCrs,Sta_STAT_RESULTS_SECTION_ID);
      Dat_WriteParamsIniEndDates ();
      Par_PutHiddenParamUnsigned (NULL,"GroupedBy",(unsigned) Sta_CLICKS_CRS_DETAILED_LIST);
      Par_PutHiddenParamUnsigned (NULL,"StatAct"  ,(unsigned) Stats->NumAction);
      Par_PutHiddenParamLong (NULL,"FirstRow",FirstRow - Stats->RowsPerPage);
      Par_PutHiddenParamLong (NULL,"LastRow" ,FirstRow - 1);
      Par_PutHiddenParamUnsigned (NULL,"RowsPage",Stats->RowsPerPage);
      Usr_PutHiddenParSelectedUsrsCods (&Gbl.Usrs.Selected);
     }
   HTM_TD_Begin ("class=\"LM\"");
   if (FirstRow > 1)
     {
      HTM_BUTTON_SUBMIT_Begin (Str_BuildStringLong (Txt_Show_previous_X_clicks,
						    (long) Stats->RowsPerPage),
			       "BT_LINK TIT_TBL",NULL);
      Str_FreeString ();
      HTM_STRONG_Begin ();
      HTM_TxtF ("&lt;%s",Txt_PAGES_Previous);
      HTM_STRONG_End ();
      HTM_BUTTON_End ();
     }
   HTM_TD_End ();
   if (FirstRow > 1)
      Frm_EndForm ();

   /* Write number of current page */
   HTM_TD_Begin ("class=\"DAT_N CM\"");
   HTM_STRONG_Begin ();
   HTM_TxtF ("%s %u-%u %s %u (%s %u %s %u)",
             Txt_Clicks,
             FirstRow,LastRow,Txt_of_PART_OF_A_TOTAL,NumHits,
             Txt_page,NumPagesBefore + 1,Txt_of_PART_OF_A_TOTAL,NumPagsTotal);
   HTM_STRONG_End ();
   HTM_TD_End ();

   /* Put link to jump to next page (more recent clicks) */
   if (LastRow < NumHits)
     {
      Frm_BeginFormAnchor (ActSeeAccCrs,Sta_STAT_RESULTS_SECTION_ID);
      Dat_WriteParamsIniEndDates ();
      Par_PutHiddenParamUnsigned (NULL,"GroupedBy",(unsigned) Sta_CLICKS_CRS_DETAILED_LIST);
      Par_PutHiddenParamUnsigned (NULL,"StatAct"  ,(unsigned) Stats->NumAction);
      Par_PutHiddenParamUnsigned (NULL,"FirstRow" ,(unsigned) (LastRow + 1));
      Par_PutHiddenParamUnsigned (NULL,"LastRow"  ,(unsigned) (LastRow + Stats->RowsPerPage));
      Par_PutHiddenParamUnsigned (NULL,"RowsPage" ,(unsigned) Stats->RowsPerPage);
      Usr_PutHiddenParSelectedUsrsCods (&Gbl.Usrs.Selected);
     }
   HTM_TD_Begin ("class=\"RM\"");
   if (LastRow < NumHits)
     {
      HTM_BUTTON_SUBMIT_Begin (Str_BuildStringLong (Txt_Show_next_X_clicks,
						    (long) Stats->RowsPerPage),
			       "BT_LINK TIT_TBL",NULL);
      Str_FreeString ();
      HTM_STRONG_Begin ();
      HTM_TxtF ("%s&gt;",Txt_PAGES_Next);
      HTM_STRONG_End ();
      HTM_BUTTON_End ();
     }
   HTM_TD_End ();
   if (LastRow < NumHits)
      Frm_EndForm ();

   HTM_TR_End ();
   HTM_TABLE_End ();
   HTM_TD_End ();
   HTM_TR_End ();

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"RT",Txt_No_INDEX);
   HTM_TH (1,1,"CT",Txt_User_ID);
   HTM_TH (1,1,"LT",Txt_Name);
   HTM_TH (1,1,"CT",Txt_Role);
   HTM_TH (1,1,"CT",Txt_Date);
   HTM_TH (1,1,"LT",Txt_Action);
   HTM_TH (1,1,"LT",Txt_LOG_More_info);

   HTM_TR_End ();

   /***** Write rows back *****/
   for (NumRow  = LastRow, UniqueId = 1, Gbl.RowEvenOdd = 0;
	NumRow >= FirstRow;
	NumRow--, UniqueId++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
     {
      mysql_data_seek (mysql_res,(my_ulonglong) (NumRow - 1));
      row = mysql_fetch_row (mysql_res);

      /* Get log code */
      LogCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get user's data of the database */
      UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[1]);
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
                                               Usr_DONT_GET_PREFS,
                                               Usr_DONT_GET_ROLE_IN_CURRENT_CRS);

      /* Get logged role */
      if (sscanf (row[2],"%u",&RoleFromLog) != 1)
	 Err_WrongRoleExit ();

      HTM_TR_Begin (NULL);

      /* Write the number of row */
      HTM_TD_Begin ("class=\"LOG RT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TxtF ("%u&nbsp;",NumRow);
      HTM_TD_End ();

      /* Write the user's ID if user is a student */
      HTM_TD_Begin ("class=\"LOG CT COLOR%u\"",Gbl.RowEvenOdd);
      ID_WriteUsrIDs (&UsrDat,NULL);
      HTM_NBSP ();
      HTM_TD_End ();

      /* Write the first name and the surnames */
      HTM_TD_Begin ("class=\"LOG LT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TxtF ("%s&nbsp;",UsrDat.FullName);
      HTM_TD_End ();

      /* Write the user's role */
      HTM_TD_Begin ("class=\"LOG CT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TxtF ("%s&nbsp;",RoleFromLog < Rol_NUM_ROLES ? Txt_ROLES_SINGUL_Abc[RoleFromLog][UsrDat.Sex] :
		                                         "?");
      HTM_TD_End ();

      /* Write the date-time (row[3]) */
      if (asprintf (&Id,"log_date_%u",UniqueId) < 0)
	 Err_NotEnoughMemoryExit ();
      HTM_TD_Begin ("id=\"%s\" class=\"LOG RT COLOR%u\"",Id,Gbl.RowEvenOdd);
      Dat_WriteLocalDateHMSFromUTC (Id,Dat_GetUNIXTimeFromStr (row[3]),
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				    true,true,false,0x7);
      HTM_TD_End ();
      free (Id);

      /* Write the action */
      if (sscanf (row[4],"%ld",&ActCod) != 1)
	 Err_WrongActionExit ();
      HTM_TD_Begin ("class=\"LOG LT COLOR%u\"",Gbl.RowEvenOdd);
      if (ActCod >= 0)
         HTM_TxtF ("%s&nbsp;",Act_GetActionText (Act_FromActCodToAction[ActCod]));
      else
         HTM_TxtF ("?&nbsp;");
      HTM_TD_End ();

      /* Write the comments of the access */
      HTM_TD_Begin ("class=\"LOG LT COLOR%u\"",Gbl.RowEvenOdd);
      Sta_WriteLogComments (LogCod);
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** Free memory used by the data of the user *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/******** Show a listing of with the number of clicks of each user ***********/
/*****************************************************************************/

static void Sta_WriteLogComments (long LogCod)
  {
   char Comments[Cns_MAX_BYTES_TEXT + 1];

   /***** Get log comments from database *****/
   DB_QuerySELECTString (Comments,sizeof (Comments) - 1,
                         "can not get log comments",
			 "SELECT Comments"
			  " FROM log_comments"
			 " WHERE LogCod=%ld",
			 LogCod);

   /***** Write comments *****/
   if (Comments[0])
      HTM_Txt (Comments);
  }

/*****************************************************************************/
/********* Show a listing of with the number of clicks of each user **********/
/*****************************************************************************/

static void Sta_ShowNumHitsPerUsr (Sta_CountType_t CountType,
                                   unsigned NumHits,
                                   MYSQL_RES *mysql_res)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Photo;
   extern const char *Txt_ID;
   extern const char *Txt_Name;
   extern const char *Txt_Role;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   MYSQL_ROW row;
   unsigned NumHit;
   struct Sta_Hits Hits;
   unsigned BarWidth;
   struct UsrData UsrDat;

   /***** Initialize user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"RT",Txt_No_INDEX);
   HTM_TH (1,1,"CT",Txt_Photo);
   HTM_TH (1,1,"LT",Txt_ID);
   HTM_TH (1,1,"LT",Txt_Name);
   HTM_TH (1,1,"CT",Txt_Role);
   HTM_TH (1,2,"LT",Txt_STAT_TYPE_COUNT_CAPS[CountType]);

   HTM_TR_End ();

   /***** Write rows *****/
   for (NumHit = 1, Hits.Max = 0.0, Gbl.RowEvenOdd = 0;
	NumHit <= NumHits;
	NumHit++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get user's data from the database */
      UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,	// Get user's data from database
                                               Usr_DONT_GET_PREFS,
                                               Usr_DONT_GET_ROLE_IN_CURRENT_CRS);

      HTM_TR_Begin (NULL);

      /* Write the number of row */
      HTM_TD_Begin ("class=\"LOG RT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TxtF ("%u&nbsp;",NumHit);
      HTM_TD_End ();

      /* Show the photo */
      HTM_TD_Begin ("class=\"CT COLOR%u\"",Gbl.RowEvenOdd);
      Pho_ShowUsrPhotoIfAllowed (&UsrDat,"PHOTO15x20",Pho_ZOOM,false);
      HTM_TD_End ();

      /* Write the user's ID if user is a student in current course */
      HTM_TD_Begin ("class=\"LOG LT COLOR%u\"",Gbl.RowEvenOdd);
      ID_WriteUsrIDs (&UsrDat,NULL);
      HTM_NBSP ();
      HTM_TD_End ();

      /* Write the name and the surnames */
      HTM_TD_Begin ("class=\"LOG LT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TxtF ("%s&nbsp;",UsrDat.FullName);
      HTM_TD_End ();

      /* Write user's role */
      HTM_TD_Begin ("class=\"LOG CT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TxtF ("%s&nbsp;",Txt_ROLES_SINGUL_Abc[UsrDat.Roles.InCurrentCrs][UsrDat.Sex]);
      HTM_TD_End ();

      /* Write the number of clicks */
      Hits.Num = Str_GetDoubleFromStr (row[1]);
      if (NumHits == 1)
	 Hits.Max = Hits.Num;
      if (Hits.Max > 0.0)
        {
         BarWidth = (unsigned) (((Hits.Num * 375.0) / Hits.Max) + 0.5);
         if (BarWidth == 0)
            BarWidth = 1;
        }
      else
         BarWidth = 0;

      HTM_TD_Begin ("class=\"LOG LT COLOR%u\"",Gbl.RowEvenOdd);
      if (BarWidth)
	{
	 HTM_IMG (Cfg_URL_ICON_PUBLIC,
		  UsrDat.Roles.InCurrentCrs == Rol_STD ? "o1x1.png" :	// Student
			                                 "r1x1.png",	// Non-editing teacher or teacher
		  NULL,
	          "class=\"LT\" style=\"width:%upx; height:10px; padding-top:4px;\"",
		  BarWidth);
	 HTM_NBSP ();
	}
      HTM_DoubleFewDigits (Hits.Num);
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** Free memory used by the data of the user *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/********** Show a listing of with the number of clicks in each date *********/
/*****************************************************************************/

static void Sta_ShowNumHitsPerDay (Sta_CountType_t CountType,
                                   unsigned NumHits,
                                   MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Date;
   extern const char *Txt_Day;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   extern const char *Txt_DAYS_SMALL[7];
   unsigned NumHit;
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
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"CT",Txt_Date);
   HTM_TH (1,1,"LT",Txt_Day);
   HTM_TH (1,1,"LT",Txt_STAT_TYPE_COUNT_CAPS[CountType]);

   HTM_TR_End ();

   /***** Compute maximum number of pages generated per day *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumHits,mysql_res,1,1);

   /***** Write rows beginning by the most recent day and ending by the oldest *****/
   mysql_data_seek (mysql_res,0);
   for (NumHit = 1;
	NumHit <= NumHits;
	NumHit++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get year, month and day (row[0] holds the date in YYYYMMDD format) */
      if (!(Dat_GetDateFromYYYYMMDD (&ReadDate,row[0])))
	 Err_WrongDateExit ();

      /* Get number of pages generated (in row[1]) */
      Hits.Num = Str_GetDoubleFromStr (row[1]);

      Dat_AssignDate (&Date,&LastDate);
      NumDaysFromLastDateToCurrDate = Dat_GetNumDaysBetweenDates (&ReadDate,&LastDate);
      /* In the next loop (NumDaysFromLastDateToCurrDate-1) días (the more recent) with 0 clicks are shown
         and a last day (the oldest) with Hits.Num */
      for (D = 1;
	   D <= NumDaysFromLastDateToCurrDate;
	   D++)
        {
         NumDayWeek = Dat_GetDayOfWeek (Date.Year,Date.Month,Date.Day);

         HTM_TR_Begin (NULL);

         /* Write the date */
	 Dat_ConvDateToDateStr (&Date,StrDate);
         HTM_TD_Begin ("class=\"%s RT\"",NumDayWeek == 6 ? "LOG_R" :
					                   "LOG");
         HTM_TxtF ("%s&nbsp;",StrDate);
         HTM_TD_End ();

         /* Write the day of the week */
         HTM_TD_Begin ("class=\"%s LT\"",NumDayWeek == 6 ? "LOG_R" :
					                   "LOG");
         HTM_TxtF ("%s&nbsp;",Txt_DAYS_SMALL[NumDayWeek]);
         HTM_TD_End ();

         /* Draw bar proportional to number of hits */
         Sta_DrawBarNumHits (NumDayWeek == 6 ? 'r' :	// red background
                                               'o',	// orange background
                             D == NumDaysFromLastDateToCurrDate ? Hits.Num :
                        	                                  0.0,
                             Hits.Max,Hits.Total,500);

         HTM_TR_End ();

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

      HTM_TR_Begin (NULL);

      /* Write the date */
      Dat_ConvDateToDateStr (&Date,StrDate);
      HTM_TD_Begin ("class=\"%s RT\"",NumDayWeek == 6 ? "LOG_R" :
					                "LOG");
      HTM_TxtF ("%s&nbsp;",StrDate);
      HTM_TD_End ();

      /* Write the day of the week */
      HTM_TD_Begin ("class=\"%s LT\"",NumDayWeek == 6 ? "LOG_R" :
					                "LOG");
      HTM_TxtF ("%s&nbsp;",Txt_DAYS_SMALL[NumDayWeek]);
      HTM_TD_End ();

      /* Draw bar proportional to number of hits */
      Sta_DrawBarNumHits (NumDayWeek == 6 ? 'r' :	// red background
	                                    'o',	// orange background
	                  0.0,Hits.Max,Hits.Total,500);

      HTM_TR_End ();

      /* Decrease day */
      Dat_GetDateBefore (&Date,&Date);
     }
  }

/*****************************************************************************/
/************ Show graphic of number of pages generated per hour *************/
/*****************************************************************************/

#define GRAPH_DISTRIBUTION_PER_HOUR_HOUR_WIDTH 25
#define GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH (GRAPH_DISTRIBUTION_PER_HOUR_HOUR_WIDTH * 24)

static void Sta_ShowDistrAccessesPerDayAndHour (const struct Sta_Stats *Stats,
                                                unsigned NumHits,
                                                MYSQL_RES *mysql_res)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Color_of_the_graphic;
   extern const char *Txt_STAT_COLOR_TYPES[Sta_NUM_COLOR_TYPES];
   extern const char *Txt_Date;
   extern const char *Txt_Day;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   extern const char *Txt_DAYS_SMALL[7];
   Sta_ColorType_t ColorType;
   unsigned ColorTypeUnsigned;
   Sta_ColorType_t SelectedColorType;
   unsigned NumHit;
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
   double NumAccPerHour[24];
   double NumAccPerHourZero[24];
   MYSQL_ROW row;
   char StrDate[Cns_MAX_BYTES_DATE + 1];

   /***** Get selected color type *****/
   SelectedColorType = Sta_GetStatColorType ();

   /***** Put a selector for the type of color *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("colspan=\"26\" class=\"CM\"");

   Frm_BeginFormAnchor (Gbl.Action.Act,Sta_STAT_RESULTS_SECTION_ID);
   Dat_WriteParamsIniEndDates ();
   Par_PutHiddenParamUnsigned (NULL,"GroupedBy",(unsigned) Stats->ClicksGroupedBy);
   Par_PutHiddenParamUnsigned (NULL,"CountType",(unsigned) Stats->CountType);
   Par_PutHiddenParamUnsigned (NULL,"StatAct"  ,(unsigned) Stats->NumAction);
   if (Gbl.Action.Act == ActSeeAccCrs)
      Usr_PutHiddenParSelectedUsrsCods (&Gbl.Usrs.Selected);
   else // Gbl.Action.Act == ActSeeAccGbl
     {
      Par_PutHiddenParamUnsigned (NULL,"Role",(unsigned) Stats->Role);
      Sta_PutHiddenParamScopeSta ();
     }

   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtColonNBSP (Txt_Color_of_the_graphic);
   HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
		     "name=\"ColorType\"");
   for (ColorType  = (Sta_ColorType_t) 0;
	ColorType <= (Sta_ColorType_t) (Sta_NUM_COLOR_TYPES - 1);
	ColorType++)
     {
      ColorTypeUnsigned = (unsigned) ColorType;
      HTM_OPTION (HTM_Type_UNSIGNED,&ColorTypeUnsigned,
		  ColorType == SelectedColorType,false,
		  "%s",Txt_STAT_COLOR_TYPES[ColorType]);
     }
   HTM_SELECT_End ();
   HTM_LABEL_End ();
   Frm_EndForm ();
   HTM_TD_End ();
   HTM_TR_End ();

   /***** Compute maximum number of pages generated per day-hour *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumHits,mysql_res,2,1);

   /***** Initialize LastDate *****/
   Dat_AssignDate (&LastDate,&Gbl.DateRange.DateEnd.Date);

   /***** Reset number of pages generated per hour *****/
   for (Hour = 0;
	Hour < 24;
	Hour++)
      NumAccPerHour[Hour] = NumAccPerHourZero[Hour] = 0.0;

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (3,1,"CT",Txt_Date);
   HTM_TH (3,1,"LT",Txt_Day);
   HTM_TH (1,24,"LT",Txt_STAT_TYPE_COUNT_CAPS[Stats->CountType]);

   HTM_TR_End ();

   HTM_TR_Begin (NULL);
   HTM_TD_Begin ("colspan=\"24\" class=\"LT\"");
   Sta_DrawBarColors (SelectedColorType,Hits.Max);
   HTM_TD_End ();
   HTM_TR_End ();

   HTM_TR_Begin (NULL);
   for (Hour = 0;
	Hour < 24;
	Hour++)
     {
      HTM_TD_Begin ("class=\"LOG CT\" style=\"width:%upx;\"",
	            GRAPH_DISTRIBUTION_PER_HOUR_HOUR_WIDTH);
      HTM_TxtF ("%02uh",Hour);
      HTM_TD_End ();
     }
   HTM_TR_End ();

   /***** Write rows beginning by the most recent day and ending by the oldest one *****/
   mysql_data_seek (mysql_res,0);

   for (NumHit = 1;
	NumHit <= NumHits;
	NumHit++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get year, month and day (row[0] holds the date in YYYYMMDD format) */
      if (!(Dat_GetDateFromYYYYMMDD (&CurrentReadDate,row[0])))
	 Err_WrongDateExit ();

      /* Get the hour (in row[1] is the hour in formato HH) */
      if (sscanf (row[1],"%02u",&ReadHour) != 1)
	 Err_WrongDateExit ();

      /* Get number of pages generated (in row[2]) */
      Hits.Num = Str_GetDoubleFromStr (row[2]);

      /* If this is the first read date, initialize PreviousReadDate */
      if (NumHit == 1)
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

            HTM_TR_Begin (NULL);

            /* Write the date */
            Dat_ConvDateToDateStr (&Date,StrDate);
            HTM_TD_Begin ("class=\"%s RT\"",NumDayWeek == 6 ? "LOG_R" :
						              "LOG");
            HTM_TxtF ("%s&nbsp;",StrDate);
            HTM_TD_End ();

            /* Write the day of the week */
            HTM_TD_Begin ("class=\"%s LT\"",NumDayWeek == 6 ? "LOG_R" :
						              "LOG");
            HTM_TxtF ("%s&nbsp;",Txt_DAYS_SMALL[NumDayWeek]);
            HTM_TD_End ();

            /* Draw a cell with the color proportional to the number of clicks */
            if (D == NumDaysFromLastDateToCurrDate)
               Sta_DrawAccessesPerHourForADay (SelectedColorType,NumAccPerHour,Hits.Max);
            else	// D < NumDaysFromLastDateToCurrDate
               Sta_DrawAccessesPerHourForADay (SelectedColorType,NumAccPerHourZero,Hits.Max);
            HTM_TR_End ();

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

      HTM_TR_Begin (NULL);

      /* Write the date */
      Dat_ConvDateToDateStr (&Date,StrDate);
      HTM_TD_Begin ("class=\"%s RT\"",NumDayWeek == 6 ? "LOG_R" :
					                "LOG");
      HTM_TxtF ("%s&nbsp;",StrDate);
      HTM_TD_End ();

      /* Write the day of the week */
      HTM_TD_Begin ("class=\"%s LT\"",NumDayWeek == 6 ? "LOG_R" :
					                "LOG");
      HTM_TxtF ("%s&nbsp;",Txt_DAYS_SMALL[NumDayWeek]);
      HTM_TD_End ();

      /* Draw the color proporcional al number of clicks */
      if (D == NumDaysFromLastDateToCurrDate)
         Sta_DrawAccessesPerHourForADay (SelectedColorType,NumAccPerHour,Hits.Max);
      else	// D < NumDaysFromLastDateToCurrDate
         Sta_DrawAccessesPerHourForADay (SelectedColorType,NumAccPerHourZero,Hits.Max);
      HTM_TR_End ();

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

      HTM_TR_Begin (NULL);

      /* Write the date */
      Dat_ConvDateToDateStr (&Date,StrDate);
      HTM_TD_Begin ("class=\"%s RT\"",NumDayWeek == 6 ? "LOG_R" :
					                "LOG");
      HTM_TxtF ("%s&nbsp;",StrDate);
      HTM_TD_End ();

      /* Write the day of the week */
      HTM_TD_Begin ("class=\"%s LT\"",NumDayWeek == 6 ? "LOG_R" :
					                "LOG");
      HTM_TxtF ("%s&nbsp;",Txt_DAYS_SMALL[NumDayWeek]);
      HTM_TD_End ();

      /* Draw the color proportional to number of clicks */
      Sta_DrawAccessesPerHourForADay (SelectedColorType,NumAccPerHourZero,Hits.Max);

      HTM_TR_End ();

      /* Decrease day */
      Dat_GetDateBefore (&Date,&Date);
     }
  }

/*****************************************************************************/
/********* Put hidden parameter for the type of figure (statistic) ***********/
/*****************************************************************************/

static void Sta_PutHiddenParamScopeSta (void)
  {
   Sco_PutParamScope ("ScopeSta",Gbl.Scope.Current);
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

static void Sta_DrawBarColors (Sta_ColorType_t ColorType,double HitsMax)
  {
   unsigned Interval;
   unsigned NumColor;
   unsigned R = 0;	// Initialized to avoid warning
   unsigned G = 0;	// Initialized to avoid warning
   unsigned B = 0;	// Initialized to avoid warning

   /***** Write numbers from 0 to Hits.Max *****/
   HTM_TABLE_BeginWide ();
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("colspan=\"%u\" class=\"LOG LB\" style=\"width:%upx;\"",
		 (GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH/5)/2,
		 (GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH/5)/2);
   HTM_Unsigned (0);
   HTM_TD_End ();

   for (Interval = 1;
	Interval <= 4;
	Interval++)
     {
      HTM_TD_Begin ("colspan=\"%u\" class=\"LOG CB\" style=\"width:%upx;\"",
		    GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH/5,
		    GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH/5);
      HTM_DoubleFewDigits ((double) Interval * HitsMax / 5.0);
      HTM_TD_End ();
     }

   HTM_TD_Begin ("colspan=\"%u\" class=\"LOG RB\" style=\"width:%upx;\"",
		 (GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH/5)/2,
		 (GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH/5)/2);
   HTM_DoubleFewDigits (HitsMax);
   HTM_TD_End ();

   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Draw colors *****/
   for (NumColor = 0;
	NumColor < GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH;
	NumColor++)
     {
      Sta_SetColor (ColorType,(double) NumColor,(double) GRAPH_DISTRIBUTION_PER_HOUR_TOTAL_WIDTH,&R,&G,&B);
      HTM_TD_Begin ("class=\"LM\" style=\"width:1px; background-color:#%02X%02X%02X;\"",
	            R,G,B);
      Ico_PutIcon ("tr1x14.gif","","");
      HTM_TD_End ();
     }
   HTM_TR_End ();
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********************* Draw accesses per hour for a day **********************/
/*****************************************************************************/

static void Sta_DrawAccessesPerHourForADay (Sta_ColorType_t ColorType,double HitsNum[24],double HitsMax)
  {
   unsigned Hour;
   unsigned R = 0;	// Initialized to avoid warning
   unsigned G = 0;	// Initialized to avoid warning
   unsigned B = 0;	// Initialized to avoid warning
   char *Str;

   for (Hour = 0;
	Hour < 24;
	Hour++)
     {
      /***** Set color depending on hits *****/
      Sta_SetColor (ColorType,HitsNum[Hour],HitsMax,&R,&G,&B);

      /***** Write from floating point number to string *****/
      Str_DoubleNumToStrFewDigits (&Str,HitsNum[Hour]);

      /***** Write cell *****/
      HTM_TD_Begin ("class=\"LOG LM\" title=\"%s\""
	            " style=\"width:%upx; background-color:#%02X%02X%02X;\"",
	            Str,GRAPH_DISTRIBUTION_PER_HOUR_HOUR_WIDTH,R,G,B);
      HTM_TD_End ();

      /***** Free memory allocated for string *****/
      free (Str);
     }
  }

/*****************************************************************************/
/************************* Set color depending on hits ***********************/
/*****************************************************************************/
// Hits.Max must be > 0
/*
Black         Blue         Cyan        Green        Yellow        Red
  +------------+------------+------------+------------+------------+
  |     0.2    |     0.2    |     0.2    |     0.2    |     0.2    |
  +------------+------------+------------+------------+------------+
 0.0          0.2          0.4          0.6          0.8          1.0
*/

static void Sta_SetColor (Sta_ColorType_t ColorType,double HitsNum,double HitsMax,
                          unsigned *R,unsigned *G,unsigned *B)
  {
   double Result = (HitsNum / HitsMax);

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

static void Sta_ShowNumHitsPerWeek (Sta_CountType_t CountType,
                                    unsigned NumHits,
                                    MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Week;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned NumHit;
   struct Date ReadDate;
   struct Date LastDate;
   struct Date Date;
   unsigned W;
   unsigned NumWeeksBetweenLastDateAndCurDate;
   struct Sta_Hits Hits;
   MYSQL_ROW row;

   /***** Initialize LastDate to avoid warning *****/
   Dat_CalculateWeekOfYear (&Gbl.DateRange.DateEnd.Date);	// Changes Week and Year
   Dat_AssignDate (&LastDate,&Gbl.DateRange.DateEnd.Date);

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"LT",Txt_Week);
   HTM_TH (1,1,"LT",Txt_STAT_TYPE_COUNT_CAPS[CountType]);

   HTM_TR_End ();

   /***** Compute maximum number of pages generated per week *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumHits,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumHit = 1;
	NumHit <= NumHits;
	NumHit++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get year and week (row[0] holds date in YYYYWW format) */
      if (sscanf (row[0],"%04u%02u",&ReadDate.Year,&ReadDate.Week) != 2)
	 Err_WrongDateExit ();

      /* Get number of pages generated (in row[1]) */
      Hits.Num = Str_GetDoubleFromStr (row[1]);

      Dat_AssignDate (&Date,&LastDate);
      NumWeeksBetweenLastDateAndCurDate = Dat_GetNumWeeksBetweenDates (&ReadDate,&LastDate);
      for (W = 1;
	   W <= NumWeeksBetweenLastDateAndCurDate;
	   W++)
        {
         HTM_TR_Begin (NULL);

         /* Write week */
         HTM_TD_Begin ("class=\"LOG LT\"");
         HTM_TxtF ("%04u-%02u&nbsp;",Date.Year,Date.Week);
         HTM_TD_End ();

         /* Draw bar proportional to number of hits */
         Sta_DrawBarNumHits ('o',	// orange background
                             W == NumWeeksBetweenLastDateAndCurDate ? Hits.Num :
                        	                                      0.0,
                             Hits.Max,Hits.Total,500);

         HTM_TR_End ();

         /* Decrement week */
         Dat_GetWeekBefore (&Date,&Date);
        }
      Dat_AssignDate (&LastDate,&Date);
     }

  /***** Finally, show the old weeks without pages generated *****/
  Dat_CalculateWeekOfYear (&Gbl.DateRange.DateIni.Date);	// Changes Week and Year
  NumWeeksBetweenLastDateAndCurDate = Dat_GetNumWeeksBetweenDates (&Gbl.DateRange.DateIni.Date,
                                                                   &LastDate);
  for (W = 1;
       W <= NumWeeksBetweenLastDateAndCurDate;
       W++)
    {
     HTM_TR_Begin (NULL);

     /* Write week */
     HTM_TD_Begin ("class=\"LOG LT\"");
     HTM_TxtF ("%04u-%02u&nbsp;",Date.Year,Date.Week);
     HTM_TD_End ();

     /* Draw bar proportional to number of hits */
     Sta_DrawBarNumHits ('o',	// orange background
                         0.0,Hits.Max,Hits.Total,500);

     HTM_TR_End ();

     /* Decrement week */
     Dat_GetWeekBefore (&Date,&Date);
    }
  }

/*****************************************************************************/
/********** Show a graph with the number of clicks in each month *************/
/*****************************************************************************/

static void Sta_ShowNumHitsPerMonth (Sta_CountType_t CountType,
                                     unsigned NumHits,
                                     MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Month;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned NumHit;
   struct Date ReadDate;
   struct Date LastDate;
   struct Date Date;
   unsigned M;
   unsigned NumMonthsBetweenLastDateAndCurDate;
   struct Sta_Hits Hits;
   MYSQL_ROW row;

   /***** Initialize LastDate *****/
   Dat_AssignDate (&LastDate,&Gbl.DateRange.DateEnd.Date);

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"LT",Txt_Month);
   HTM_TH (1,1,"LT",Txt_STAT_TYPE_COUNT_CAPS[CountType]);

   HTM_TR_End ();

   /***** Compute maximum number of pages generated per month *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumHits,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumHit = 1;
	NumHit <= NumHits;
	NumHit++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get the year and the month (in row[0] is the date in YYYYMM format) */
      if (sscanf (row[0],"%04u%02u",&ReadDate.Year,&ReadDate.Month) != 2)
	 Err_WrongDateExit ();

      /* Get number of pages generated (in row[1]) */
      Hits.Num = Str_GetDoubleFromStr (row[1]);

      Dat_AssignDate (&Date,&LastDate);
      NumMonthsBetweenLastDateAndCurDate = Dat_GetNumMonthsBetweenDates (&ReadDate,
                                                                         &LastDate);
      for (M = 1;
	   M <= NumMonthsBetweenLastDateAndCurDate;
	   M++)
        {
         HTM_TR_Begin (NULL);

         /* Write the month */
         HTM_TD_Begin ("class=\"LOG LT\"");
         HTM_TxtF ("%04u-%02u&nbsp;",Date.Year,Date.Month);
         HTM_TD_End ();

         /* Draw bar proportional to number of hits */
         Sta_DrawBarNumHits ('o',	// orange background
                             M == NumMonthsBetweenLastDateAndCurDate ? Hits.Num :
                        	                                       0.0,
                             Hits.Max,Hits.Total,500);

         HTM_TR_End ();

         /* Decrease month */
         Dat_GetMonthBefore (&Date,&Date);
        }
      Dat_AssignDate (&LastDate,&Date);
     }

  /***** Finally, show the oldest months without clicks *****/
  NumMonthsBetweenLastDateAndCurDate = Dat_GetNumMonthsBetweenDates (&Gbl.DateRange.DateIni.Date,
                                                                     &LastDate);
  for (M = 1;
       M <= NumMonthsBetweenLastDateAndCurDate;
       M++)
    {
     HTM_TR_Begin (NULL);

     /* Write the month */
     HTM_TD_Begin ("class=\"LOG LT\"");
     HTM_TxtF ("%04u-%02u&nbsp;",Date.Year,Date.Month);
     HTM_TD_End ();

     /* Draw bar proportional to number of hits */
     Sta_DrawBarNumHits ('o',	// orange background
                         0.0,Hits.Max,Hits.Total,500);

     HTM_TR_End ();

     /* Decrease month */
     Dat_GetMonthBefore (&Date,&Date);
    }
  }

/*****************************************************************************/
/*********** Show a graph with the number of clicks in each year *************/
/*****************************************************************************/

static void Sta_ShowNumHitsPerYear (Sta_CountType_t CountType,
                                    unsigned NumHits,
                                    MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Year;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned NumHit;
   struct Date ReadDate;
   struct Date LastDate;
   struct Date Date;
   unsigned Y;
   unsigned NumYearsBetweenLastDateAndCurDate;
   struct Sta_Hits Hits;
   MYSQL_ROW row;

   /***** Initialize LastDate *****/
   Dat_AssignDate (&LastDate,&Gbl.DateRange.DateEnd.Date);

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"LT",Txt_Year);
   HTM_TH (1,1,"LT",Txt_STAT_TYPE_COUNT_CAPS[CountType]);

   HTM_TR_End ();

   /***** Compute maximum number of pages generated per year *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumHits,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumHit = 1;
	NumHit <= NumHits;
	NumHit++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get the year (in row[0] is the date in YYYY format) */
      if (sscanf (row[0],"%04u",&ReadDate.Year) != 1)
	 Err_WrongDateExit ();

      /* Get number of pages generated (in row[1]) */
      Hits.Num = Str_GetDoubleFromStr (row[1]);

      Dat_AssignDate (&Date,&LastDate);
      NumYearsBetweenLastDateAndCurDate = Dat_GetNumYearsBetweenDates (&ReadDate,
                                                                       &LastDate);
      for (Y = 1;
	   Y <= NumYearsBetweenLastDateAndCurDate;
	   Y++)
        {
         HTM_TR_Begin (NULL);

         /* Write the year */
         HTM_TD_Begin ("class=\"LOG LT\"");
         HTM_TxtF ("%04u&nbsp;",Date.Year);
         HTM_TD_End ();

         /* Draw bar proportional to number of hits */
         Sta_DrawBarNumHits ('o',	// orange background
                             Y == NumYearsBetweenLastDateAndCurDate ? Hits.Num :
                        	                                      0.0,
                             Hits.Max,Hits.Total,500);

         HTM_TR_End ();

         /* Decrease year */
         Dat_GetYearBefore (&Date,&Date);
        }
      Dat_AssignDate (&LastDate,&Date);
     }

  /***** Finally, show the oldest years without clicks *****/
  NumYearsBetweenLastDateAndCurDate = Dat_GetNumYearsBetweenDates (&Gbl.DateRange.DateIni.Date,
                                                                   &LastDate);
  for (Y = 1;
       Y <= NumYearsBetweenLastDateAndCurDate;
       Y++)
    {
     HTM_TR_Begin (NULL);

     /* Write the year */
     HTM_TD_Begin ("class=\"LOG LT\"");
     HTM_TxtF ("%04u&nbsp;",Date.Year);
     HTM_TD_End ();

     /* Draw bar proportional to number of hits */
     Sta_DrawBarNumHits ('o',	// orange background
                         0.0,Hits.Max,Hits.Total,500);

     HTM_TR_End ();

     /* Decrease year */
     Dat_GetYearBefore (&Date,&Date);
    }
  }

/*****************************************************************************/
/**************** Show graphic of number of pages generated per hour ***************/
/*****************************************************************************/

#define DIGIT_WIDTH 6

static void Sta_ShowNumHitsPerHour (unsigned NumHits,
                                    MYSQL_RES *mysql_res)
  {
   unsigned NumHit;
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
      Sta_ComputeMaxAndTotalHits (&Hits,NumHits,mysql_res,1,NumDays);

      /***** Compute width of columns (one for each hour) *****/
      /* Maximum number of dígits. If less than 4, set it to 4 to ensure a minimum width */
      NumDigits = (Hits.Max >= 1000) ? (unsigned) floor (log10 ((double) Hits.Max)) + 1 :
	                               4;
      ColumnWidth = NumDigits * DIGIT_WIDTH + 2;

      /***** Draw the graphic *****/
      mysql_data_seek (mysql_res,0);
      NumHit = 1;
      HTM_TR_Begin (NULL);
      while (Hour < 24)
	{
	 Hits.Num = 0.0;
	 if (NumHit <= NumHits)	// If not read yet all the results of the query
	   {
	    row = mysql_fetch_row (mysql_res); // Get next result
	    NumHit++;
	    if (sscanf (row[0],"%02u",&ReadHour) != 1)   // In row[0] is the date in HH format
	       Err_WrongDateExit ();

	    for (H = Hour;
		 H < ReadHour;
		 H++, Hour++)
	       Sta_WriteAccessHour (H,&Hits,ColumnWidth);

	    Hits.Num = Str_GetDoubleFromStr (row[1]) / (float) NumDays;
	    Sta_WriteAccessHour (ReadHour,&Hits,ColumnWidth);

	    Hour++;
	   }
	 else
	    for (H = ReadHour + 1;
		 H < 24;
		 H++, Hour++)
	       Sta_WriteAccessHour (H,&Hits,ColumnWidth);
	}
      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/**** Write a column of the graphic of the number of clicks in each hour *****/
/*****************************************************************************/

static void Sta_WriteAccessHour (unsigned Hour,struct Sta_Hits *Hits,unsigned ColumnWidth)
  {
   unsigned BarHeight;

   HTM_TD_Begin ("class=\"DAT_SMALL CB\" style=\"width:%upx;\"",ColumnWidth);

   /* Draw bar with a height porportional to the number of clicks */
   if (Hits->Num > 0.0)
     {
      HTM_TxtF ("%u%%",(unsigned) (((Hits->Num * 100.0) /
		                     Hits->Total) + 0.5));
      HTM_BR ();
      HTM_DoubleFewDigits (Hits->Num);
      HTM_BR ();
      BarHeight = (unsigned) (((Hits->Num * 500.0) / Hits->Max) + 0.5);
      if (BarHeight == 0)
         BarHeight = 1;
      HTM_IMG (Cfg_URL_ICON_PUBLIC,"o1x1.png",NULL,	// Orange background
	       "style=\"width:10px;height:%upx;\"",BarHeight);
     }
   else
     {
      HTM_Txt ("0%");
      HTM_BR ();
      HTM_Unsigned (0);
     }

   /* Write the hour */
   HTM_BR ();
   HTM_TxtF ("%uh",Hour);
   HTM_TD_End ();
  }

/*****************************************************************************/
/**** Show a listing with the number of clicks in every minute of the day ***/
/*****************************************************************************/

#define Sta_NUM_MINUTES_PER_DAY		(60 * 24)	// 1440 minutes in a day
#define Sta_WIDTH_SEMIDIVISION_GRAPHIC	30
#define Sta_NUM_DIVISIONS_X		10

static void Sta_ShowAverageAccessesPerMinute (unsigned NumHits,MYSQL_RES *mysql_res)
  {
   unsigned NumHit = 1;
   MYSQL_ROW row;
   unsigned NumDays;
   unsigned MinuteDay = 0;
   unsigned ReadHour;
   unsigned MinuteRead;
   unsigned MinuteDayRead = 0;
   unsigned i;
   struct Sta_Hits Hits;
   double NumClicksPerMin[Sta_NUM_MINUTES_PER_DAY];
   double Power10LeastOrEqual;
   double MaxX;
   double IncX;
   char *Format;

   if ((NumDays = Dat_GetNumDaysBetweenDates (&Gbl.DateRange.DateIni.Date,&Gbl.DateRange.DateEnd.Date)))
     {
      /***** Compute number of clicks (and máximo) in every minute *****/
      Hits.Max = 0.0;
      while (MinuteDay < Sta_NUM_MINUTES_PER_DAY)
	{
	 if (NumHit <= NumHits)	// If not all the result of the query are yet read
	   {
	    row = mysql_fetch_row (mysql_res); // Get next result
	    NumHit++;
	    if (sscanf (row[0],"%02u%02u",&ReadHour,&MinuteRead) != 2)   // In row[0] is the date in formato HHMM
	       Err_WrongDateExit ();
	    /* Get number of pages generated */
	    Hits.Num = Str_GetDoubleFromStr (row[1]);
	    MinuteDayRead = ReadHour * 60 + MinuteRead;
	    for (i = MinuteDay;
		 i < MinuteDayRead;
		 i++, MinuteDay++)
	       NumClicksPerMin[i] = 0.0;
	    NumClicksPerMin[MinuteDayRead] = Hits.Num / (double) NumDays;
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
	 Power10LeastOrEqual = pow (10.0,floor (log10 (Hits.Max)));
	 MaxX = ceil (Hits.Max / Power10LeastOrEqual) * Power10LeastOrEqual;
	}
      IncX = MaxX / Sta_NUM_DIVISIONS_X;
      if (IncX >= 1.0)
	 Format = "%.0lf";
      else if (IncX >= 0.1)
	 Format = "%.1lf";
      else if (IncX >= 0.01)
	 Format = "%.2lf";
      else if (IncX >= 0.001)
	 Format = "%.3lf";
      else
	 Format = "%lf";

      /***** X axis tags *****/
      Sta_WriteLabelsXAxisAccMin (IncX,Format);

      /***** Y axis and graphic *****/
      for (i = 0;
	   i < Sta_NUM_MINUTES_PER_DAY;
	   i++)
	 Sta_WriteAccessMinute (i,NumClicksPerMin[i],MaxX);

      /***** X axis *****/
      HTM_TR_Begin (NULL);

      /* First division (left) */
      HTM_TD_Begin ("class=\"LM\" style=\"width:%upx;\"",
	            Sta_WIDTH_SEMIDIVISION_GRAPHIC);
      HTM_IMG (Cfg_URL_ICON_PUBLIC,"ejexizq24x1.gif",NULL,
	       "style=\"display:block;width:%upx;height:1px;\"",
	       Sta_WIDTH_SEMIDIVISION_GRAPHIC);
      HTM_TD_End ();

      /* All the intermediate divisions */
      for (i = 0;
	   i < Sta_NUM_DIVISIONS_X * 2;
	   i++)
	{
	 HTM_TD_Begin ("class=\"LM\" style=\"width:%upx;\"",
		       Sta_WIDTH_SEMIDIVISION_GRAPHIC);
	 HTM_IMG (Cfg_URL_ICON_PUBLIC,"ejex24x1.gif",NULL,
		  "style=\"display:block;width:%upx;height:1px;\"",
		  Sta_WIDTH_SEMIDIVISION_GRAPHIC);
	 HTM_TD_End ();
	}

      /* Last division (right) */
      HTM_TD_Begin ("class=\"LM\" style=\"width:%upx;\"",
	            Sta_WIDTH_SEMIDIVISION_GRAPHIC);
      HTM_IMG (Cfg_URL_ICON_PUBLIC,"tr24x1.gif",NULL,
	       "style=\"display:block;width:%upx;height:1px;\"",
	       Sta_WIDTH_SEMIDIVISION_GRAPHIC);
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Write again the labels of the X axis *****/
      Sta_WriteLabelsXAxisAccMin (IncX,Format);
     }
  }

/*****************************************************************************/
/****** Write labels of the X axis in the graphic of clicks per minute *******/
/*****************************************************************************/

#define Sta_WIDTH_DIVISION_GRAPHIC	(Sta_WIDTH_SEMIDIVISION_GRAPHIC * 2)	// 60

static void Sta_WriteLabelsXAxisAccMin (double IncX,const char *Format)
  {
   unsigned i;
   double NumX;

   HTM_TR_Begin (NULL);
   for (i = 0, NumX = 0;
	i <= Sta_NUM_DIVISIONS_X;
	i++, NumX += IncX)
     {
      HTM_TD_Begin ("colspan=\"2\" class=\"LOG CB\" style=\"width:%upx;\"",
                    Sta_WIDTH_DIVISION_GRAPHIC);
      HTM_TxtF (Format,NumX);
      HTM_TD_End ();
     }
   HTM_TR_End ();
  }

/*****************************************************************************/
/***** Write a row of the graphic with number of clicks in every minute ******/
/*****************************************************************************/

#define Sta_WIDTH_GRAPHIC	(Sta_WIDTH_DIVISION_GRAPHIC * Sta_NUM_DIVISIONS_X)	// 60 * 10 = 600

static void Sta_WriteAccessMinute (unsigned Minute,double HitsNum,double MaxX)
  {
   unsigned BarWidth;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Labels of the Y axis, and Y axis *****/
      if (!Minute)
	{
	 // If minute 0
	 HTM_TD_Begin ("rowspan=\"30\" class=\"LOG LT\""
		       " style=\"width:%upx;"
		       " background-image:url('%s/ejey24x30.gif');"
		       " background-size:30px 30px;"
		       " background-repeat:repeat;\"",
		       Sta_WIDTH_SEMIDIVISION_GRAPHIC,Cfg_URL_ICON_PUBLIC);
	    HTM_Txt ("00h");
	 HTM_TD_End ();
	}
      else if (Minute == (Sta_NUM_MINUTES_PER_DAY - 30))
	{
	 // If 23:30
	 HTM_TD_Begin ("rowspan=\"30\" class=\"LOG LB\""
		       " style=\"width:%upx;"
		       " background-image:url('%s/ejey24x30.gif');"
		       " background-size:30px 30px;"
		       " background-repeat:repeat;\"",
		       Sta_WIDTH_SEMIDIVISION_GRAPHIC,Cfg_URL_ICON_PUBLIC);
	    HTM_Txt ("24h");
	 HTM_TD_End ();
	}
      else if (!(Minute % 30) && (Minute % 60))
	{
	 // If minute is multiple of 30 but not of 60 (i.e.: 30, 90, 150...)
	 HTM_TD_Begin ("rowspan=\"60\" class=\"LOG LM\""
		       " style=\"width:%upx;"
		       " background-image:url('%s/ejey24x60.gif');"
		       " background-size:30px 60px;"
		       " background-repeat:repeat;\"",
		       Sta_WIDTH_SEMIDIVISION_GRAPHIC,Cfg_URL_ICON_PUBLIC);
	    HTM_TxtF ("%02uh",(Minute + 30) / 60);
	 HTM_TD_End ();
	}

      /***** Begin cell for the graphic *****/
      HTM_TD_Begin ("colspan=\"%u\" class=\"LB\""
		    " style=\"width:%upx; height:1px;"
		    " background-image:url('%s/malla%c48x1.gif');"
		    " background-size:60px 1px;"
		    " background-repeat:repeat;\"",
		    Sta_NUM_DIVISIONS_X * 2,Sta_WIDTH_GRAPHIC,Cfg_URL_ICON_PUBLIC,
		    (Minute % 60) == 0 ? 'v' :
					 'h');

	 /***** Draw bar with a width proportional to the number of hits *****/
	 if (HitsNum != 0.0)
	    if ((BarWidth = (unsigned) (((HitsNum * (double) Sta_WIDTH_GRAPHIC / MaxX)) + 0.5)) != 0)
	       HTM_IMG (Cfg_URL_ICON_PUBLIC,
			(Minute % 60) == 0 ? "r1x1.png" :	// red background
					     "o1x1.png",	// orange background
			NULL,
			"style=\"display:block;width:%upx;height:1px;\"",
			BarWidth);

      /***** End cell of graphic and end row *****/
      HTM_TD_End ();
   HTM_TR_End ();
  }

/*****************************************************************************/
/**** Show a listing of accesses with the number of clicks a each action *****/
/*****************************************************************************/

static void Sta_ShowNumHitsPerAction (Sta_CountType_t CountType,
                                      unsigned NumHits,
                                      MYSQL_RES *mysql_res)
  {
   extern Act_Action_t Act_FromActCodToAction[1 + Act_MAX_ACTION_COD];
   extern const char *Txt_Action;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned NumHit;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   long ActCod;

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"RT",Txt_Action);
   HTM_TH (1,1,"LT",Txt_STAT_TYPE_COUNT_CAPS[CountType]);

   HTM_TR_End ();

   /***** Compute maximum number of pages generated per day *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumHits,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumHit = 1;
	NumHit <= NumHits;
	NumHit++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Write the action */
      ActCod = Str_ConvertStrCodToLongCod (row[0]);

      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"LOG RT\"");
      if (ActCod >= 0)
	 HTM_TxtF ("%s&nbsp;",Act_GetActionText (Act_FromActCodToAction[ActCod]));
      else
         HTM_Txt ("?&nbsp;");
      HTM_TD_End ();

      /* Draw bar proportional to number of hits */
      Hits.Num = Str_GetDoubleFromStr (row[1]);
      Sta_DrawBarNumHits ('o',	// orange background
                          Hits.Num,Hits.Max,Hits.Total,500);

      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/*************** Show number of clicks distributed by plugin *****************/
/*****************************************************************************/

static void Sta_ShowNumHitsPerPlugin (Sta_CountType_t CountType,
                                      unsigned NumHits,
                                      MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Plugin;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned NumHit;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   struct Plugin Plg;

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"RT",Txt_Plugin);
   HTM_TH (1,1,"LT",Txt_STAT_TYPE_COUNT_CAPS[CountType]);

   HTM_TR_End ();

   /***** Compute maximum number of pages generated per plugin *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumHits,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumHit = 1;
	NumHit <= NumHits;
	NumHit++)
     {
      row = mysql_fetch_row (mysql_res);

      HTM_TR_Begin (NULL);

      /* Write the plugin */
      if (sscanf (row[0],"%ld",&Plg.PlgCod) != 1)
	 Err_WrongPluginExit ();
      HTM_TD_Begin ("class=\"LOG RT\"");
      if (Plg_GetDataOfPluginByCod (&Plg))
         HTM_Txt (Plg.Name);
      else
         HTM_Txt ("?");
      HTM_NBSP ();
      HTM_TD_End ();

      /* Draw bar proportional to number of hits */
      Hits.Num = Str_GetDoubleFromStr (row[1]);
      Sta_DrawBarNumHits ('o',	// orange background
                          Hits.Num,Hits.Max,Hits.Total,500);

      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/******** Show number of clicks distributed by web service function **********/
/*****************************************************************************/

static void Sta_ShowNumHitsPerWSFunction (Sta_CountType_t CountType,
                                          unsigned NumHits,
                                          MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Function;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned NumHit;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   long FunCod;

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"LT",Txt_Function);
   HTM_TH (1,1,"LT",Txt_STAT_TYPE_COUNT_CAPS[CountType]);

   HTM_TR_End ();

   /***** Compute maximum number of pages generated per function *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumHits,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumHit = 1;
	NumHit <= NumHits;
	NumHit++)
     {
      row = mysql_fetch_row (mysql_res);

      HTM_TR_Begin (NULL);

      /* Write the plugin */
      if (sscanf (row[0],"%ld",&FunCod) != 1)
	 Err_ShowErrorAndExit ("Wrong function code.");

      HTM_TD_Begin ("class=\"LOG LT\"");
      HTM_TxtF ("%s&nbsp;",API_GetFunctionNameFromFunCod (FunCod));
      HTM_TD_End ();

      /* Draw bar proportional to number of hits */
      Hits.Num = Str_GetDoubleFromStr (row[1]);
      Sta_DrawBarNumHits ('o',	// orange background
                          Hits.Num,Hits.Max,Hits.Total,500);

      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/******** Show number of clicks distributed by web service function **********/
/*****************************************************************************/

static void Sta_ShowNumHitsPerBanner (Sta_CountType_t CountType,
                                      unsigned NumHits,
                                      MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Banner;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned NumHit;
   double NumClicks;
   double MaxClicks = 0.0;
   double TotalClicks = 0.0;
   MYSQL_ROW row;
   struct Ban_Banner Ban;

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"CT",Txt_Banner);
   HTM_TH (1,1,"LT",Txt_STAT_TYPE_COUNT_CAPS[CountType]);

   HTM_TR_End ();

   /***** Compute maximum number of clicks per banner *****/
   for (NumHit = 1;
	NumHit <= NumHits;
	NumHit++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get number of pages generated */
      NumClicks = Str_GetDoubleFromStr (row[1]);
      if (NumHit == 1)
	 MaxClicks = NumClicks;
      TotalClicks += NumClicks;
     }

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumHit = 1;
	NumHit <= NumHits;
	NumHit++)
     {
      row = mysql_fetch_row (mysql_res);

      HTM_TR_Begin (NULL);

      /* Write the banner */
      if (sscanf (row[0],"%ld",&(Ban.BanCod)) != 1)
	 Err_WrongBannerExit ();
      Ban_GetDataOfBannerByCod (&Ban);
      HTM_TD_Begin ("class=\"LOG LT\"");
      HTM_A_Begin ("href=\"%s\" title=\"%s\" class=\"DAT\" target=\"_blank\"",
                   Ban.WWW,
                   Ban.FullName);
      HTM_IMG (Cfg_URL_BANNER_PUBLIC,Ban.Img,Ban.FullName,
	       "style=\"margin:0 10px 5px 0;\"");
      HTM_A_End ();

      /* Draw bar proportional to number of clicks */
      NumClicks = Str_GetDoubleFromStr (row[1]);
      Sta_DrawBarNumHits ('o',	// orange background
		      	  NumClicks,MaxClicks,TotalClicks,500);

      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/******* Show a listing with the number of hits distributed by country *******/
/*****************************************************************************/

static void Sta_ShowNumHitsPerCountry (Sta_CountType_t CountType,
                                       unsigned NumHits,
                                       MYSQL_RES *mysql_res)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Country;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned NumHit;
   unsigned Ranking;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   long CtyCod;

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"CT",Txt_No_INDEX);
   HTM_TH (1,1,"CT",Txt_Country);
   HTM_TH (1,1,"LT",Txt_STAT_TYPE_COUNT_CAPS[CountType]);

   HTM_TR_End ();

   /***** Compute maximum number of hits per country *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumHits,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumHit = 1, Ranking = 0;
	NumHit <= NumHits;
	NumHit++)
     {
      /* Get country code */
      row = mysql_fetch_row (mysql_res);
      CtyCod = Str_ConvertStrCodToLongCod (row[0]);

      HTM_TR_Begin (NULL);

      /* Write ranking of this country */
      HTM_TD_Begin ("class=\"LOG RM\"");
      if (CtyCod > 0)
         HTM_Unsigned (++Ranking);
      HTM_NBSP ();
      HTM_TD_End ();

      /* Write country */
      Sta_WriteCountry (CtyCod);

      /* Draw bar proportional to number of hits */
      Hits.Num = Str_GetDoubleFromStr (row[1]);
      Sta_DrawBarNumHits ('o',	// orange background
                          Hits.Num,Hits.Max,Hits.Total,375);

      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/************************ Write country with an icon *************************/
/*****************************************************************************/

static void Sta_WriteCountry (long CtyCod)
  {
   struct Cty_Countr Cty;

   /***** Begin cell *****/
   HTM_TD_Begin ("class=\"LOG LM\"");

      if (CtyCod > 0)	// Hit with a country selected
	{
	 /***** Get data of country *****/
	 Cty.CtyCod = CtyCod;
	 Cty_GetDataOfCountryByCod (&Cty);

	 /***** Form to go to country *****/
	 Cty_DrawCountryMapAndNameWithLink (&Cty,ActSeeCtyInf,
					    "COUNTRY_TINY",
					    "COUNTRY_MAP_TINY",
					    "BT_LINK LT LOG");
	}
      else			// Hit with no country selected
	 /***** No country selected *****/
	 HTM_Txt ("&nbsp;-&nbsp;");

   /***** End cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/***** Show a listing with the number of hits distributed by institution *****/
/*****************************************************************************/

static void Sta_ShowNumHitsPerInstitution (Sta_CountType_t CountType,
                                           unsigned NumHits,
                                           MYSQL_RES *mysql_res)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Institution;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned NumHit;
   unsigned Ranking;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   long InsCod;

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"CT",Txt_No_INDEX);
   HTM_TH (1,1,"CT",Txt_Institution);
   HTM_TH (1,1,"LT",Txt_STAT_TYPE_COUNT_CAPS[CountType]);

   HTM_TR_End ();

   /***** Compute maximum number of hits per institution *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumHits,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumHit = 1, Ranking = 0;
	NumHit <= NumHits;
	NumHit++)
     {
      /* Get institution code */
      row = mysql_fetch_row (mysql_res);
      InsCod = Str_ConvertStrCodToLongCod (row[0]);

      HTM_TR_Begin (NULL);

      /* Write ranking of this institution */
      HTM_TD_Begin ("class=\"LOG RT\"");
      if (InsCod > 0)
         HTM_Unsigned (++Ranking);
      HTM_NBSP ();
      HTM_TD_End ();

      /* Write institution */
      Sta_WriteInstit (InsCod);

      /* Draw bar proportional to number of hits */
      Hits.Num = Str_GetDoubleFromStr (row[1]);
      Sta_DrawBarNumHits ('o',	// orange background
		      	  Hits.Num,Hits.Max,Hits.Total,375);

      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/********************** Write institution with an icon ***********************/
/*****************************************************************************/

static void Sta_WriteInstit (long InsCod)
  {
   struct Ins_Instit Ins;

   /***** Begin cell *****/
   if (InsCod > 0)	// Hit with an institution selected
     {
      /***** Get data of institution *****/
      Ins.InsCod = InsCod;
      Ins_GetDataOfInstitutionByCod (&Ins);

      /***** Title in cell *****/
      HTM_TD_Begin ("class=\"LOG LM\" title=\"%s\"",Ins.FullName);

	 /***** Form to go to institution *****/
	 Ins_DrawInstitutionLogoAndNameWithLink (&Ins,ActSeeInsInf,
						 "BT_LINK LT LOG","CT");
     }
   else			// Hit with no institution selected
     {
      /***** No institution selected *****/
      HTM_TD_Begin ("class=\"LOG LM\"");
	 HTM_Txt ("&nbsp;-&nbsp;");
     }

   /***** End cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/******* Show a listing with the number of hits distributed by center ********/
/*****************************************************************************/

static void Sta_ShowNumHitsPerCenter (Sta_CountType_t CountType,
                                      unsigned NumHits,
                                      MYSQL_RES *mysql_res)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Center;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned NumHit;
   unsigned Ranking;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   long CtrCod;

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"CT",Txt_No_INDEX);
   HTM_TH (1,1,"CT",Txt_Center);
   HTM_TH (1,1,"LT",Txt_STAT_TYPE_COUNT_CAPS[CountType]);

   HTM_TR_End ();

   /***** Compute maximum number of hits per center *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumHits,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumHit = 1, Ranking = 0;
	NumHit <= NumHits;
	NumHit++)
     {
      /* Get center code */
      row = mysql_fetch_row (mysql_res);
      CtrCod = Str_ConvertStrCodToLongCod (row[0]);

      HTM_TR_Begin (NULL);

      /* Write ranking of this center */
      HTM_TD_Begin ("class=\"LOG RT\"");
      if (CtrCod > 0)
         HTM_Unsigned (++Ranking);
      HTM_NBSP ();
      HTM_TD_End ();

      /* Write center */
      Sta_WriteCenter (CtrCod);

      /* Draw bar proportional to number of hits */
      Hits.Num = Str_GetDoubleFromStr (row[1]);
      Sta_DrawBarNumHits ('o',	// orange background
		      	  Hits.Num,Hits.Max,Hits.Total,375);

      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/************************* Write center with an icon *************************/
/*****************************************************************************/

static void Sta_WriteCenter (long CtrCod)
  {
   struct Ctr_Center Ctr;

   /***** Begin cell *****/
   if (CtrCod > 0)	// Hit with a center selected
     {
      /***** Get data of center *****/
      Ctr.CtrCod = CtrCod;
      Ctr_GetDataOfCenterByCod (&Ctr);

      /***** Title in cell *****/
      HTM_TD_Begin ("class=\"LOG LM\" title=\"%s\"",Ctr.FullName);

	 /***** Form to go to center *****/
	 Ctr_DrawCenterLogoAndNameWithLink (&Ctr,ActSeeCtrInf,
					    "BT_LINK LT LOG","CT");
     }
   else			// Hit with no center selected
     {
      /***** No center selected *****/
      HTM_TD_Begin ("class=\"LOG LM\"");
	 HTM_Txt ("&nbsp;-&nbsp;");
     }

   /***** End cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/******* Show a listing with the number of hits distributed by degree ********/
/*****************************************************************************/

static void Sta_ShowNumHitsPerDegree (Sta_CountType_t CountType,
                                      unsigned NumHits,
                                      MYSQL_RES *mysql_res)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Degree;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   unsigned NumHit;
   unsigned Ranking;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   long DegCod;

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"CT",Txt_No_INDEX);
   HTM_TH (1,1,"CT",Txt_Degree);
   HTM_TH (1,1,"LT",Txt_STAT_TYPE_COUNT_CAPS[CountType]);

   HTM_TR_End ();

   /***** Compute maximum number of hits per degree *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumHits,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumHit = 1, Ranking = 0;
	NumHit <= NumHits;
	NumHit++)
     {
      /* Get degree code */
      row = mysql_fetch_row (mysql_res);
      DegCod = Str_ConvertStrCodToLongCod (row[0]);

      HTM_TR_Begin (NULL);

      /* Write ranking of this degree */
      HTM_TD_Begin ("class=\"LOG RT\"");
      if (DegCod > 0)
         HTM_Unsigned (++Ranking);
      HTM_NBSP ();
      HTM_TD_End ();

      /* Write degree */
      Sta_WriteDegree (DegCod);

      /* Draw bar proportional to number of hits */
      Hits.Num = Str_GetDoubleFromStr (row[1]);
      Sta_DrawBarNumHits ('o',	// orange background
		      	  Hits.Num,Hits.Max,Hits.Total,375);

      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/************************* Write degree with an icon *************************/
/*****************************************************************************/

static void Sta_WriteDegree (long DegCod)
  {
   struct Deg_Degree Deg;

   /***** Begin cell *****/
   if (DegCod > 0)	// Hit with a degree selected
     {
      /***** Get data of degree *****/
      Deg.DegCod = DegCod;
      Deg_GetDataOfDegreeByCod (&Deg);

      /***** Title in cell *****/
      HTM_TD_Begin ("class=\"LOG LM\" title=\"%s\"",Deg.FullName);

	 /***** Form to go to degree *****/
	 Deg_DrawDegreeLogoAndNameWithLink (&Deg,ActSeeDegInf,
					    "BT_LINK LT LOG","CT");
     }
   else			// Hit with no degree selected
     {
      /***** No degree selected *****/
      HTM_TD_Begin ("class=\"LOG LM\"");
	 HTM_Txt ("&nbsp;-&nbsp;");
     }

   /***** End cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/********* Show a listing with the number of clicks to each course ***********/
/*****************************************************************************/

static void Sta_ShowNumHitsPerCourse (Sta_CountType_t CountType,
                                      unsigned NumHits,
                                      MYSQL_RES *mysql_res)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Degree;
   extern const char *Txt_Year_OF_A_DEGREE;
   extern const char *Txt_Course;
   extern const char *Txt_STAT_TYPE_COUNT_CAPS[Sta_NUM_COUNT_TYPES];
   extern const char *Txt_YEAR_OF_DEGREE[1 + Deg_MAX_YEARS_PER_DEGREE];	// Declaration in swad_degree.c
   unsigned NumHit;
   unsigned Ranking;
   struct Sta_Hits Hits;
   MYSQL_ROW row;
   bool CrsOK;
   struct Crs_Course Crs;

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"CT",Txt_No_INDEX);
   HTM_TH (1,1,"CT",Txt_Degree);
   HTM_TH (1,1,"CT",Txt_Year_OF_A_DEGREE);
   HTM_TH (1,1,"CT",Txt_Course);
   HTM_TH (1,1,"LT",Txt_STAT_TYPE_COUNT_CAPS[CountType]);

   HTM_TR_End ();

   /***** Compute maximum number of pages generated per course *****/
   Sta_ComputeMaxAndTotalHits (&Hits,NumHits,mysql_res,1,1);

   /***** Write rows *****/
   mysql_data_seek (mysql_res,0);
   for (NumHit = 1, Ranking = 0;
	NumHit <= NumHits;
	NumHit++)
     {
      /* Get degree, the year and the course */
      row = mysql_fetch_row (mysql_res);

      /* Get course code */
      Crs.CrsCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get data of current degree */
      CrsOK = Crs_GetDataOfCourseByCod (&Crs);

      HTM_TR_Begin (NULL);

      /* Write ranking of this course */
      HTM_TD_Begin ("class=\"LOG RT\"");
      if (CrsOK)
         HTM_Unsigned (++Ranking);
      HTM_NBSP ();
      HTM_TD_End ();

      /* Write degree */
      Sta_WriteDegree (Crs.DegCod);

      /* Write degree year */
      HTM_TD_Begin ("class=\"LOG CT\"");
      if (CrsOK)
         HTM_Txt (Txt_YEAR_OF_DEGREE[Crs.Year]);
      else
	 HTM_Hyphen ();
      HTM_NBSP ();
      HTM_TD_End ();

      /* Write course, including link */
      HTM_TD_Begin ("class=\"LOG LT\"");
      if (CrsOK)
        {
         Frm_BeginFormGoTo (ActSeeCrsInf);
         Crs_PutParamCrsCod (Crs.CrsCod);
         HTM_BUTTON_SUBMIT_Begin (Hie_BuildGoToMsg (Crs.FullName),"BT_LINK LT LOG",NULL);
         Hie_FreeGoToMsg ();
         HTM_Txt (Crs.ShrtName);
         HTM_BUTTON_End ();
        }
      else
         HTM_Hyphen ();
      HTM_NBSP ();
      if (CrsOK)
         Frm_EndForm ();
      HTM_TD_End ();

      /* Draw bar proportional to number of hits */
      Hits.Num = Str_GetDoubleFromStr (row[1]);
      Sta_DrawBarNumHits ('o',	// orange background
		      	  Hits.Num,Hits.Max,Hits.Total,375);

      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/*************** Compute maximum and total number of hits ********************/
/*****************************************************************************/

void Sta_ComputeMaxAndTotalHits (struct Sta_Hits *Hits,
                                 unsigned NumHits,
                                 MYSQL_RES *mysql_res,unsigned Field,
                                 unsigned Divisor)
  {
   unsigned NumHit;
   MYSQL_ROW row;

   /***** For each row... *****/
   for (NumHit = 1, Hits->Max = Hits->Total = 0.0;
	NumHit <= NumHits;
	NumHit++)
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get number of hits */
      Hits->Num = Str_GetDoubleFromStr (row[Field]);
      if (Divisor > 1)
         Hits->Num /= (double) Divisor;

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

static void Sta_DrawBarNumHits (char Color,
				double HitsNum,double HitsMax,double HitsTotal,
				unsigned MaxBarWidth)
  {
   unsigned BarWidth;
   char *Icon;

   HTM_TD_Begin ("class=\"LOG LM\"");

   if (HitsNum != 0.0)
     {
      /***** Draw bar with a with proportional to the number of hits *****/
      BarWidth = (unsigned) (((HitsNum * (double) MaxBarWidth) / HitsMax) + 0.5);
      if (BarWidth == 0)
         BarWidth = 1;
      if (asprintf (&Icon,"%c1x1.png",Color) < 0)	// Background
	 Err_NotEnoughMemoryExit ();
      HTM_IMG (Cfg_URL_ICON_PUBLIC,Icon,NULL,
	       "style=\"width:%upx;height:10px;\"",BarWidth);
      free (Icon);

      /***** Write the number of hits *****/
      HTM_NBSP ();
      HTM_DoubleFewDigits (HitsNum);
      HTM_TxtF ("&nbsp;(%u",(unsigned) (((HitsNum * 100.0) /
        	                          HitsTotal) + 0.5));
     }
   else
      /***** Write the number of clicks *****/
      HTM_Txt ("0&nbsp;(0");

   HTM_Txt ("%)&nbsp;");

   HTM_TD_End ();
  }

/*****************************************************************************/
/**************** Compute the time used to generate the page *****************/
/*****************************************************************************/

time_t Sta_ComputeTimeToGeneratePage (void)
  {
   if (gettimeofday (&Gbl.tvPageCreated, &Gbl.tz))
      // Error in gettimeofday
      return (time_t) 0;

   return (time_t) ((Gbl.tvPageCreated.tv_sec  - Gbl.tvStart.tv_sec) * 1000000L +
                     Gbl.tvPageCreated.tv_usec - Gbl.tvStart.tv_usec);
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
   char StrTimeGenerationInMicroseconds[Dat_MAX_BYTES_TIME + 1];
   char StrTimeSendInMicroseconds[Dat_MAX_BYTES_TIME + 1];

   Sta_WriteTime (StrTimeGenerationInMicroseconds,Gbl.TimeGenerationInMicroseconds);
   Sta_WriteTime (StrTimeSendInMicroseconds,Gbl.TimeSendInMicroseconds);
   HTM_TxtF ("%s %s %s %s",
             Txt_PAGE1_Page_generated_in,StrTimeGenerationInMicroseconds,
             Txt_PAGE2_and_sent_in,StrTimeSendInMicroseconds);
  }

/*****************************************************************************/
/********* Write time (given in microseconds) depending on amount ************/
/*****************************************************************************/

void Sta_WriteTime (char Str[Dat_MAX_BYTES_TIME],long TimeInMicroseconds)
  {
   if (TimeInMicroseconds < 1000L)
      snprintf (Str,Dat_MAX_BYTES_TIME + 1,"%ld &micro;s",TimeInMicroseconds);
   else if (TimeInMicroseconds < 1000000L)
      snprintf (Str,Dat_MAX_BYTES_TIME + 1,"%ld ms",TimeInMicroseconds / 1000);
   else if (TimeInMicroseconds < (60 * 1000000L))
      snprintf (Str,Dat_MAX_BYTES_TIME + 1,"%.1f s",
                (double) TimeInMicroseconds / 1E6);
   else
      snprintf (Str,Dat_MAX_BYTES_TIME + 1,"%ld min, %ld s",
                TimeInMicroseconds / (60 * 1000000L),
                (TimeInMicroseconds / 1000000L) % 60);
  }
