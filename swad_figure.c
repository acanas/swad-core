// swad_figure.c: figures (global stats)

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For system, getenv, etc.
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_figure.h"
#include "swad_file_browser.h"
#include "swad_follow.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_institution.h"
#include "swad_logo.h"
#include "swad_network.h"
#include "swad_notice.h"
#include "swad_profile.h"
#include "swad_role.h"
#include "swad_table.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct Fig_SizeOfFileZones
  {
   int NumCrss;	// -1 stands for not aplicable
   int NumGrps;	// -1 stands for not aplicable
   int NumUsrs;	// -1 stands for not aplicable
   unsigned MaxLevels;
   unsigned long NumFolders;
   unsigned long NumFiles;
   unsigned long long int Size;	// Total size in bytes
  };

struct Fig_FiguresForum
  {
   unsigned NumForums;
   unsigned NumThreads;
   unsigned NumPosts;
   unsigned NumUsrsToBeNotifiedByEMail;
  };

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

static void Fig_PutParamsToShowFigure (void);
static void Fig_PutHiddenParamFigureType (void);
static void Fig_PutHiddenParamScopeFig (void);

static void Fig_GetAndShowHierarchyStats (void);
static void Fig_WriteHeadHierarchy (void);
static void Fig_GetAndShowHierarchyWithInss (void);
static void Fig_GetAndShowHierarchyWithCtrs (void);
static void Fig_GetAndShowHierarchyWithDegs (void);
static void Fig_GetAndShowHierarchyWithCrss (void);
static void Fig_GetAndShowHierarchyWithUsrs (Rol_Role_t Role);
static void Fig_GetAndShowHierarchyTotal (void);
static void Fig_ShowHierarchyRow (const char *Text1,const char *Text2,
				  const char *ClassTxt,
				  int NumCtys,	// < 0 ==> do not show number
				  int NumInss,	// < 0 ==> do not show number
				  int NumCtrs,	// < 0 ==> do not show number
				  int NumDegs,	// < 0 ==> do not show number
				  int NumCrss);	// < 0 ==> do not show number
static void Fig_ShowHierarchyCell (const char *ClassTxt,int Num);

static void Fig_GetAndShowInstitutionsStats (void);
static void Fig_GetAndShowInssOrderedByNumCtrs (void);
static void Fig_GetAndShowInssOrderedByNumDegs (void);
static void Fig_GetAndShowInssOrderedByNumCrss (void);
static void Fig_GetAndShowInssOrderedByNumUsrsInCrss (void);
static void Fig_GetAndShowInssOrderedByNumUsrsWhoClaimToBelongToThem (void);
static void Fig_ShowInss (MYSQL_RES **mysql_res,unsigned NumInss,
		          const char *TxtFigure);
static unsigned Fig_GetInsAndStat (struct Instit *Ins,MYSQL_RES *mysql_res);

static void Fig_GetAndShowDegreeTypesStats (void);

static void Fig_GetAndShowUsersStats (void);
static void Fig_GetAndShowNumUsrsInCrss (Rol_Role_t Role);
static void Fig_GetAndShowNumUsrsNotBelongingToAnyCrs (void);

static void Fig_GetAndShowUsersRanking (void);

static void Fig_GetAndShowFileBrowsersStats (void);
static void Fig_GetSizeOfFileZoneFromDB (Sco_Scope_t Scope,
                                         Brw_FileBrowser_t FileBrowser,
                                         struct Fig_SizeOfFileZones *SizeOfFileZones);
static void Fig_WriteStatsExpTreesTableHead1 (void);
static void Fig_WriteStatsExpTreesTableHead2 (void);
static void Fig_WriteStatsExpTreesTableHead3 (void);
static void Fig_WriteRowStatsFileBrowsers1 (const char *NameOfFileZones,
					    Brw_FileBrowser_t FileZone,
                                            struct Fig_SizeOfFileZones *SizeOfFileZones);
static void Fig_WriteRowStatsFileBrowsers2 (const char *NameOfFileZones,
					    Brw_FileBrowser_t FileZone,
                                            struct Fig_SizeOfFileZones *SizeOfFileZones);
static void Fig_WriteRowStatsFileBrowsers3 (const char *NameOfFileZones,
					    Brw_FileBrowser_t FileZone,
                                            struct Fig_SizeOfFileZones *SizeOfFileZones);

static void Fig_GetAndShowOERsStats (void);
static void Fig_GetNumberOfOERsFromDB (Sco_Scope_t Scope,Brw_License_t License,unsigned long NumFiles[2]);

static void Fig_GetAndShowAssignmentsStats (void);
static void Fig_GetAndShowProjectsStats (void);
static void Fig_GetAndShowTestsStats (void);
static void Fig_GetAndShowGamesStats (void);

static void Fig_GetAndShowSocialActivityStats (void);
static void Fig_GetAndShowFollowStats (void);

static void Fig_GetAndShowForumStats (void);
static void Fig_ShowStatOfAForumType (For_ForumType_t ForumType,
                                      long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod,
                                      struct Fig_FiguresForum *FiguresForum);
static void Fig_WriteForumTitleAndStats (For_ForumType_t ForumType,
                                         long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod,
                                         const char *Icon,struct Fig_FiguresForum *FiguresForum,
                                         const char *ForumName1,const char *ForumName2);
static void Fig_WriteForumTotalStats (struct Fig_FiguresForum *FiguresForum);

static void Fig_GetAndShowNumUsrsPerNotifyEvent (void);
static void Fig_GetAndShowNoticesStats (void);
static void Fig_GetAndShowMsgsStats (void);

static void Fig_GetAndShowSurveysStats (void);
static void Fig_GetAndShowNumUsrsPerPrivacy (void);
static void Fig_GetAndShowNumUsrsPerPrivacyForAnObject (const char *TxtObject,const char *FieldName);
static void Fig_GetAndShowNumUsrsPerCookies (void);
static void Fig_GetAndShowNumUsrsPerLanguage (void);
static void Fig_GetAndShowNumUsrsPerFirstDayOfWeek (void);
static void Fig_GetAndShowNumUsrsPerDateFormat (void);
static void Fig_GetAndShowNumUsrsPerIconSet (void);
static void Fig_GetAndShowNumUsrsPerMenu (void);
static void Fig_GetAndShowNumUsrsPerTheme (void);
static void Fig_GetAndShowNumUsrsPerSideColumns (void);
unsigned Fig_GetNumUsrsWhoChoseAnOption (const char *SubQuery);


/*****************************************************************************/
/************************** Show use of the platform *************************/
/*****************************************************************************/

void Fig_ReqShowFigures (void)
  {
   extern const char *Hlp_ANALYTICS_Figures;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Figures;
   extern const char *Txt_Scope;
   extern const char *Txt_Statistic;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Show_statistic;
   Fig_FigureType_t FigureType;

   /***** Form to show statistic *****/
   Frm_StartForm (ActSeeUseGbl);

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Figures,NULL,
                 Hlp_ANALYTICS_Figures,Box_NOT_CLOSABLE);

   /***** Compute stats for anywhere, degree or course? *****/
   fprintf (Gbl.F.Out,"<label class=\"%s\">%s:&nbsp;",
            The_ClassFormInBox[Gbl.Prefs.Theme],Txt_Scope);
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	               1 << Sco_SCOPE_CTY |
	               1 << Sco_SCOPE_INS |
		       1 << Sco_SCOPE_CTR |
		       1 << Sco_SCOPE_DEG |
		       1 << Sco_SCOPE_CRS;
   Gbl.Scope.Default = Sco_SCOPE_SYS;
   Sco_GetScope ("ScopeFig");
   Sco_PutSelectorScope ("ScopeFig",false);
   fprintf (Gbl.F.Out,"</label><br />");

   /***** Type of statistic *****/
   fprintf (Gbl.F.Out,"<label class=\"%s\">%s:&nbsp;"
	              "<select name=\"FigureType\">",
	    The_ClassFormInBox[Gbl.Prefs.Theme],Txt_Statistic);
   for (FigureType = (Fig_FigureType_t) 0;
	FigureType < Fig_NUM_FIGURES;
	FigureType++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",
               (unsigned) FigureType);
      if (FigureType == Gbl.Figures.FigureType)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out," />"
	                 "%s"
	                 "</option>",
               Txt_FIGURE_TYPES[FigureType]);
     }
   fprintf (Gbl.F.Out,"</select>"
	              "</label>");

   /***** Send button and end box *****/
   Box_EndBoxWithButton (Btn_CONFIRM_BUTTON,Txt_Show_statistic);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************* Put icon to show a figure *************************/
/*****************************************************************************/
// Gbl.Figures.FigureType must be set to the desired figure before calling this function

void Fig_PutIconToShowFigure (void)
  {
   extern const char *Txt_Show_statistic;

   Lay_PutContextualLinkOnlyIcon (ActSeeUseGbl,NULL,Fig_PutParamsToShowFigure,
				  "chart-pie.svg",
				  Txt_Show_statistic);
  }

/*****************************************************************************/
/************* Put hidden parameters for figures (statistics) ****************/
/*****************************************************************************/
// Gbl.Figures.FigureType must be set to the desired figure before calling this function

static void Fig_PutParamsToShowFigure (void)
  {
   /***** Set default scope (used only if Gbl.Scope.Current is unknown) *****/
   Gbl.Scope.Default = Sco_SCOPE_CRS;
   Sco_AdjustScope ();

   Fig_PutHiddenParamFigures ();
  }

/*****************************************************************************/
/************* Put hidden parameters for figures (statistics) ****************/
/*****************************************************************************/

void Fig_PutHiddenParamFigures (void)
  {
   Fig_PutHiddenParamScopeFig ();
   Fig_PutHiddenParamFigureType ();
  }

/*****************************************************************************/
/********* Put hidden parameter for the type of figure (statistic) ***********/
/*****************************************************************************/

static void Fig_PutHiddenParamFigureType (void)
  {
   Par_PutHiddenParamUnsigned ("FigureType",(unsigned) Gbl.Figures.FigureType);
  }

/*****************************************************************************/
/********* Put hidden parameter for the type of figure (statistic) ***********/
/*****************************************************************************/

static void Fig_PutHiddenParamScopeFig (void)
  {
   Sco_PutParamScope ("ScopeFig",Gbl.Scope.Current);
  }

/*****************************************************************************/
/************************** Show use of the platform *************************/
/*****************************************************************************/

void Fig_ShowFigures (void)
  {
   static void (*Fig_Function[Fig_NUM_FIGURES])(void) =	// Array of pointers to functions
     {
      Fig_GetAndShowUsersStats,			// Fig_USERS
      Fig_GetAndShowUsersRanking,		// Fig_USERS_RANKING
      Fig_GetAndShowHierarchyStats,		// Fig_HIERARCHY
      Fig_GetAndShowInstitutionsStats,		// Fig_INSTITS
      Fig_GetAndShowDegreeTypesStats,		// Fig_DEGREE_TYPES
      Fig_GetAndShowFileBrowsersStats,		// Fig_FOLDERS_AND_FILES
      Fig_GetAndShowOERsStats,			// Fig_OER
      Fig_GetAndShowAssignmentsStats,		// Fig_ASSIGNMENTS
      Fig_GetAndShowProjectsStats,		// Fig_PROJECTS
      Fig_GetAndShowTestsStats,			// Fig_TESTS
      Fig_GetAndShowGamesStats,			// Fig_GAMES
      Fig_GetAndShowSurveysStats,		// Fig_SURVEYS
      Fig_GetAndShowSocialActivityStats,	// Fig_SOCIAL_ACTIVITY
      Fig_GetAndShowFollowStats,		// Fig_FOLLOW
      Fig_GetAndShowForumStats,			// Fig_FORUMS
      Fig_GetAndShowNumUsrsPerNotifyEvent,	// Fig_NOTIFY_EVENTS
      Fig_GetAndShowNoticesStats,		// Fig_NOTICES
      Fig_GetAndShowMsgsStats,			// Fig_MESSAGES
      Net_ShowWebAndSocialNetworksStats,	// Fig_SOCIAL_NETWORKS
      Fig_GetAndShowNumUsrsPerLanguage,		// Fig_LANGUAGES
      Fig_GetAndShowNumUsrsPerFirstDayOfWeek,	// Fig_FIRST_DAY_OF_WEEK
      Fig_GetAndShowNumUsrsPerDateFormat,	// Fig_DATE_FORMAT
      Fig_GetAndShowNumUsrsPerIconSet,		// Fig_ICON_SETS
      Fig_GetAndShowNumUsrsPerMenu,		// Fig_MENUS
      Fig_GetAndShowNumUsrsPerTheme,		// Fig_THEMES
      Fig_GetAndShowNumUsrsPerSideColumns,	// Fig_SIDE_COLUMNS
      Fig_GetAndShowNumUsrsPerPrivacy,		// Fig_PRIVACY
      Fig_GetAndShowNumUsrsPerCookies,		// Fig_COOKIES
     };

   /***** Get the type of figure ******/
   Gbl.Figures.FigureType = (Fig_FigureType_t)
	                 Par_GetParToUnsignedLong ("FigureType",
	                                           0,
	                                           Fig_NUM_FIGURES - 1,
	                                           (unsigned long) Fig_FIGURE_TYPE_DEF);

   /***** Show again the form to see use of the platform *****/
   Fig_ReqShowFigures ();

   /***** Show the stat of use selected by user *****/
   Fig_Function[Gbl.Figures.FigureType] ();
  }

/*****************************************************************************/
/********************** Show stats about number of users *********************/
/*****************************************************************************/

static void Fig_GetAndShowUsersStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_users;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Users;
   extern const char *Txt_No_of_users;
   extern const char *Txt_Average_number_of_courses_to_which_a_user_belongs;
   extern const char *Txt_Average_number_of_users_belonging_to_a_course;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_USERS],NULL,
                      Hlp_ANALYTICS_Figures_users,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
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

   Fig_GetAndShowNumUsrsInCrss (Rol_STD);		// Students
   Fig_GetAndShowNumUsrsInCrss (Rol_NET);		// Non-editing teachers
   Fig_GetAndShowNumUsrsInCrss (Rol_TCH);		// Teachers
   Fig_GetAndShowNumUsrsInCrss (Rol_UNK);		// Any user in courses
   fprintf (Gbl.F.Out,"<tr>"
                      "<th colspan=\"4\" style=\"height:10px;\">"
                      "</tr>");
   Fig_GetAndShowNumUsrsNotBelongingToAnyCrs ();	// Users not beloging to any course

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/**************** Get and show number of users in courses ********************/
/*****************************************************************************/
// Rol_UNK means any role in courses

static void Fig_GetAndShowNumUsrsInCrss (Rol_Role_t Role)
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

static void Fig_GetAndShowNumUsrsNotBelongingToAnyCrs (void)
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

static void Fig_GetAndShowUsersRanking (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_ranking;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Clicks;
   extern const char *Txt_Clicks_per_day;
   extern const char *Txt_Timeline;
   extern const char *Txt_Downloads;
   extern const char *Txt_Forums;
   extern const char *Txt_Messages;
   extern const char *Txt_Followers;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_USERS_RANKING],NULL,
                      Hlp_ANALYTICS_Figures_ranking,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
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
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Clicks,
            Txt_Clicks_per_day,
            Txt_Timeline,
            Txt_Followers,
            Txt_Downloads,
            Txt_Forums,
            Txt_Messages);

   /***** Rankings *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"DAT LEFT_TOP\">");
   Prf_GetAndShowRankingClicks ();
   fprintf (Gbl.F.Out,"</td>"
                      "<td class=\"DAT LEFT_TOP\">");
   Prf_GetAndShowRankingClicksPerDay ();
   fprintf (Gbl.F.Out,"</td>"
                      "<td class=\"DAT LEFT_TOP\">");
   Prf_GetAndShowRankingSocPub ();
   fprintf (Gbl.F.Out,"</td>"
                      "<td class=\"DAT LEFT_TOP\">");
   Fol_GetAndShowRankingFollowers ();
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
	              "</tr>");

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/*********            Get and show stats about hierarchy           ***********/
/********* (countries, institutions, centres, degrees and courses) ***********/
/*****************************************************************************/

static void Fig_GetAndShowHierarchyStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_hierarchy;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   Rol_Role_t Role;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_HIERARCHY],NULL,
                      Hlp_ANALYTICS_Figures_hierarchy,Box_NOT_CLOSABLE,2);

   Fig_WriteHeadHierarchy ();
   Fig_GetAndShowHierarchyWithInss ();
   Fig_GetAndShowHierarchyWithCtrs ();
   Fig_GetAndShowHierarchyWithDegs ();
   Fig_GetAndShowHierarchyWithCrss ();
   for (Role =  Rol_TCH;
	Role >= Rol_STD;
	Role--)
      Fig_GetAndShowHierarchyWithUsrs (Role);
   Fig_GetAndShowHierarchyTotal ();

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/************************ Write head of hierarchy table **********************/
/*****************************************************************************/

static void Fig_WriteHeadHierarchy (void)
  {
   extern const char *Txt_Countries;
   extern const char *Txt_Institutions;
   extern const char *Txt_Centres;
   extern const char *Txt_Degrees;
   extern const char *Txt_Courses;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th></th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "<img src=\"%s/globe.svg\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"CONTEXT_ICO_x16\" />"
                      "<br />"
                      "%s"
		      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "<img src=\"%s/university.svg\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"CONTEXT_ICO_x16\" />"
                      "<br />"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "<img src=\"%s/building.svg\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"CONTEXT_ICO_x16\" />"
                      "<br />"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "<img src=\"%s/graduation-cap.svg\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"CONTEXT_ICO_x16\" />"
                      "<br />"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "<img src=\"%s/list-ol.svg\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"CONTEXT_ICO_x16\" />"
                      "<br />"
                      "%s"
                      "</th>"
                      "</tr>",
            Cfg_URL_ICON_PUBLIC,Txt_Countries   ,Txt_Countries   ,Txt_Countries,
            Cfg_URL_ICON_PUBLIC,Txt_Institutions,Txt_Institutions,Txt_Institutions,
            Cfg_URL_ICON_PUBLIC,Txt_Centres     ,Txt_Centres     ,Txt_Centres,
            Cfg_URL_ICON_PUBLIC,Txt_Degrees     ,Txt_Degrees     ,Txt_Degrees,
            Cfg_URL_ICON_PUBLIC,Txt_Courses     ,Txt_Courses     ,Txt_Courses);
  }

/*****************************************************************************/
/****** Get and show number of elements in hierarchy with institutions *******/
/*****************************************************************************/

static void Fig_GetAndShowHierarchyWithInss (void)
  {
   extern const char *Txt_With_;
   extern const char *Txt_institutions;
   unsigned NumCtysWithInss = 1;

   /***** Get number of elements with institutions *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
         NumCtysWithInss = Cty_GetNumCtysWithInss ("");
         break;
      case Sco_SCOPE_CTY:
      case Sco_SCOPE_INS:
      case Sco_SCOPE_CTR:
      case Sco_SCOPE_DEG:
      case Sco_SCOPE_CRS:
	 break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Write number of elements with institutions *****/
   Fig_ShowHierarchyRow (Txt_With_,Txt_institutions,
			 "DAT",
                         (int) NumCtysWithInss,
                         -1,		// < 0 ==> do not show number
                         -1,		// < 0 ==> do not show number
                         -1,		// < 0 ==> do not show number
			 -1);		// < 0 ==> do not show number
  }

/*****************************************************************************/
/******** Get and show number of elements in hierarchy with centres **********/
/*****************************************************************************/

static void Fig_GetAndShowHierarchyWithCtrs (void)
  {
   extern const char *Txt_With_;
   extern const char *Txt_centres;
   char SubQuery[128];
   unsigned NumCtysWithCtrs = 1;
   unsigned NumInssWithCtrs = 1;

   /***** Get number of elements with centres *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
         NumCtysWithCtrs = Cty_GetNumCtysWithCtrs ("");
         NumInssWithCtrs = Ins_GetNumInssWithCtrs ("");
         break;
      case Sco_SCOPE_CTY:
         sprintf (SubQuery,"institutions.CtyCod=%ld AND ",
                  Gbl.CurrentCty.Cty.CtyCod);
         NumInssWithCtrs = Ins_GetNumInssWithCtrs (SubQuery);
         break;
      case Sco_SCOPE_INS:
      case Sco_SCOPE_CTR:
      case Sco_SCOPE_DEG:
      case Sco_SCOPE_CRS:
	 break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Write number of elements with centres *****/
   Fig_ShowHierarchyRow (Txt_With_,Txt_centres,
			 "DAT",
                         (int) NumCtysWithCtrs,
                         (int) NumInssWithCtrs,
                         -1,		// < 0 ==> do not show number
                         -1,		// < 0 ==> do not show number
			 -1);		// < 0 ==> do not show number
  }

/*****************************************************************************/
/******** Get and show number of elements in hierarchy with degrees **********/
/*****************************************************************************/

static void Fig_GetAndShowHierarchyWithDegs (void)
  {
   extern const char *Txt_With_;
   extern const char *Txt_degrees;
   char SubQuery[128];
   unsigned NumCtysWithDegs = 1;
   unsigned NumInssWithDegs = 1;
   unsigned NumCtrsWithDegs = 1;

   /***** Get number of elements with degrees *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
         NumCtysWithDegs = Cty_GetNumCtysWithDegs ("");
         NumInssWithDegs = Ins_GetNumInssWithDegs ("");
	 NumCtrsWithDegs = Ctr_GetNumCtrsWithDegs ("");
         break;
      case Sco_SCOPE_CTY:
         sprintf (SubQuery,"institutions.CtyCod=%ld AND ",
                  Gbl.CurrentCty.Cty.CtyCod);
         NumInssWithDegs = Ins_GetNumInssWithDegs (SubQuery);
	 NumCtrsWithDegs = Ctr_GetNumCtrsWithDegs (SubQuery);
         break;
      case Sco_SCOPE_INS:
         sprintf (SubQuery,"centres.InsCod=%ld AND ",
                  Gbl.CurrentIns.Ins.InsCod);
	 NumCtrsWithDegs = Ctr_GetNumCtrsWithDegs (SubQuery);
         break;
      case Sco_SCOPE_CTR:
      case Sco_SCOPE_DEG:
      case Sco_SCOPE_CRS:
	 break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Write number of elements with degrees *****/
   Fig_ShowHierarchyRow (Txt_With_,Txt_degrees,
			 "DAT",
                         (int) NumCtysWithDegs,
                         (int) NumInssWithDegs,
                         (int) NumCtrsWithDegs,
                         -1,		// < 0 ==> do not show number
			 -1);		// < 0 ==> do not show number
  }

/*****************************************************************************/
/******** Get and show number of elements in hierarchy with courses **********/
/*****************************************************************************/

static void Fig_GetAndShowHierarchyWithCrss (void)
  {
   extern const char *Txt_With_;
   extern const char *Txt_courses;
   char SubQuery[128];
   unsigned NumCtysWithCrss = 1;
   unsigned NumInssWithCrss = 1;
   unsigned NumCtrsWithCrss = 1;
   unsigned NumDegsWithCrss = 1;

   /***** Get number of elements with courses *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
         NumCtysWithCrss = Cty_GetNumCtysWithCrss ("");
         NumInssWithCrss = Ins_GetNumInssWithCrss ("");
	 NumCtrsWithCrss = Ctr_GetNumCtrsWithCrss ("");
	 NumDegsWithCrss = Deg_GetNumDegsWithCrss ("");
         break;
      case Sco_SCOPE_CTY:
         sprintf (SubQuery,"institutions.CtyCod=%ld AND ",
                  Gbl.CurrentCty.Cty.CtyCod);
         NumInssWithCrss = Ins_GetNumInssWithCrss (SubQuery);
	 NumCtrsWithCrss = Ctr_GetNumCtrsWithCrss (SubQuery);
	 NumDegsWithCrss = Deg_GetNumDegsWithCrss (SubQuery);
         break;
      case Sco_SCOPE_INS:
         sprintf (SubQuery,"centres.InsCod=%ld AND ",
                  Gbl.CurrentIns.Ins.InsCod);
	 NumCtrsWithCrss = Ctr_GetNumCtrsWithCrss (SubQuery);
	 NumDegsWithCrss = Deg_GetNumDegsWithCrss (SubQuery);
         break;
      case Sco_SCOPE_CTR:
         sprintf (SubQuery,"degrees.CtrCod=%ld AND ",
                  Gbl.CurrentCtr.Ctr.CtrCod);
	 NumDegsWithCrss = Deg_GetNumDegsWithCrss (SubQuery);
	 break;
      case Sco_SCOPE_DEG:
      case Sco_SCOPE_CRS:
	 break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Write number of elements with courses *****/
   Fig_ShowHierarchyRow (Txt_With_,Txt_courses,
			 "DAT",
                         (int) NumCtysWithCrss,
                         (int) NumInssWithCrss,
                         (int) NumCtrsWithCrss,
                         (int) NumDegsWithCrss,
			 -1);		// < 0 ==> do not show number
  }

/*****************************************************************************/
/********** Get and show number of elements in hierarchy with users **********/
/*****************************************************************************/

static void Fig_GetAndShowHierarchyWithUsrs (Rol_Role_t Role)
  {
   extern const char *Txt_With_;
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char SubQuery[128];
   unsigned NumCtysWithUsrs = 0;
   unsigned NumInssWithUsrs = 0;
   unsigned NumCtrsWithUsrs = 0;
   unsigned NumDegsWithUsrs = 0;
   unsigned NumCrssWithUsrs = 0;

   /***** Get number of elements with students *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
         NumCtysWithUsrs = Cty_GetNumCtysWithUsrs (Role,"");
         NumInssWithUsrs = Ins_GetNumInssWithUsrs (Role,"");
	 NumCtrsWithUsrs = Ctr_GetNumCtrsWithUsrs (Role,"");
	 NumDegsWithUsrs = Deg_GetNumDegsWithUsrs (Role,"");
	 NumCrssWithUsrs = Crs_GetNumCrssWithUsrs (Role,"");
         break;
      case Sco_SCOPE_CTY:
         sprintf (SubQuery,"institutions.CtyCod=%ld AND ",
                  Gbl.CurrentCty.Cty.CtyCod);
         NumCtysWithUsrs = Cty_GetNumCtysWithUsrs (Role,SubQuery);
         NumInssWithUsrs = Ins_GetNumInssWithUsrs (Role,SubQuery);
	 NumCtrsWithUsrs = Ctr_GetNumCtrsWithUsrs (Role,SubQuery);
	 NumDegsWithUsrs = Deg_GetNumDegsWithUsrs (Role,SubQuery);
	 NumCrssWithUsrs = Crs_GetNumCrssWithUsrs (Role,SubQuery);
         break;
      case Sco_SCOPE_INS:
         sprintf (SubQuery,"centres.InsCod=%ld AND ",
                  Gbl.CurrentIns.Ins.InsCod);
         NumCtysWithUsrs = Cty_GetNumCtysWithUsrs (Role,SubQuery);
         NumInssWithUsrs = Ins_GetNumInssWithUsrs (Role,SubQuery);
	 NumCtrsWithUsrs = Ctr_GetNumCtrsWithUsrs (Role,SubQuery);
	 NumDegsWithUsrs = Deg_GetNumDegsWithUsrs (Role,SubQuery);
	 NumCrssWithUsrs = Crs_GetNumCrssWithUsrs (Role,SubQuery);
         break;
      case Sco_SCOPE_CTR:
         sprintf (SubQuery,"degrees.CtrCod=%ld AND ",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         NumCtysWithUsrs = Cty_GetNumCtysWithUsrs (Role,SubQuery);
         NumInssWithUsrs = Ins_GetNumInssWithUsrs (Role,SubQuery);
	 NumCtrsWithUsrs = Ctr_GetNumCtrsWithUsrs (Role,SubQuery);
	 NumDegsWithUsrs = Deg_GetNumDegsWithUsrs (Role,SubQuery);
	 NumCrssWithUsrs = Crs_GetNumCrssWithUsrs (Role,SubQuery);
	 break;
      case Sco_SCOPE_DEG:
         sprintf (SubQuery,"courses.DegCod=%ld AND ",
                  Gbl.CurrentDeg.Deg.DegCod);
         NumCtysWithUsrs = Cty_GetNumCtysWithUsrs (Role,SubQuery);
         NumInssWithUsrs = Ins_GetNumInssWithUsrs (Role,SubQuery);
	 NumCtrsWithUsrs = Ctr_GetNumCtrsWithUsrs (Role,SubQuery);
	 NumDegsWithUsrs = Deg_GetNumDegsWithUsrs (Role,SubQuery);
	 NumCrssWithUsrs = Crs_GetNumCrssWithUsrs (Role,SubQuery);
	 break;
     case Sco_SCOPE_CRS:
         sprintf (SubQuery,"crs_usr.CrsCod=%ld AND ",
                  Gbl.CurrentCrs.Crs.CrsCod);
         NumCtysWithUsrs = Cty_GetNumCtysWithUsrs (Role,SubQuery);
         NumInssWithUsrs = Ins_GetNumInssWithUsrs (Role,SubQuery);
	 NumCtrsWithUsrs = Ctr_GetNumCtrsWithUsrs (Role,SubQuery);
	 NumDegsWithUsrs = Deg_GetNumDegsWithUsrs (Role,SubQuery);
	 NumCrssWithUsrs = Crs_GetNumCrssWithUsrs (Role,SubQuery);
	 break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Write number of elements with students *****/
   Fig_ShowHierarchyRow (Txt_With_,Txt_ROLES_PLURAL_abc[Role][Usr_SEX_UNKNOWN],
			 "DAT",
                         (int) NumCtysWithUsrs,
                         (int) NumInssWithUsrs,
                         (int) NumCtrsWithUsrs,
                         (int) NumDegsWithUsrs,
			 (int) NumCrssWithUsrs);
  }

/*****************************************************************************/
/************ Get and show total number of elements in hierarchy *************/
/*****************************************************************************/

static void Fig_GetAndShowHierarchyTotal (void)
  {
   extern const char *Txt_Total;
   unsigned NumCtysTotal = 1;
   unsigned NumInssTotal = 1;
   unsigned NumCtrsTotal = 1;
   unsigned NumDegsTotal = 1;
   unsigned NumCrssTotal = 1;

   /***** Get total number of elements *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 NumCtysTotal = Cty_GetNumCtysTotal ();
	 NumInssTotal = Ins_GetNumInssTotal ();
	 NumCtrsTotal = Ctr_GetNumCtrsTotal ();
	 NumDegsTotal = Deg_GetNumDegsTotal ();
	 NumCrssTotal = Crs_GetNumCrssTotal ();
         break;
      case Sco_SCOPE_CTY:
	 NumInssTotal = Ins_GetNumInssInCty (Gbl.CurrentCty.Cty.CtyCod);
	 NumCtrsTotal = Ctr_GetNumCtrsInCty (Gbl.CurrentCty.Cty.CtyCod);
	 NumDegsTotal = Deg_GetNumDegsInCty (Gbl.CurrentCty.Cty.CtyCod);
	 NumCrssTotal = Crs_GetNumCrssInCty (Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
	 NumCtrsTotal = Ctr_GetNumCtrsInIns (Gbl.CurrentIns.Ins.InsCod);
	 NumDegsTotal = Deg_GetNumDegsInIns (Gbl.CurrentIns.Ins.InsCod);
	 NumCrssTotal = Crs_GetNumCrssInIns (Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
	 NumDegsTotal = Deg_GetNumDegsInCtr (Gbl.CurrentCtr.Ctr.CtrCod);
	 NumCrssTotal = Crs_GetNumCrssInCtr (Gbl.CurrentCtr.Ctr.CtrCod);
	 break;
      case Sco_SCOPE_DEG:
	 NumCrssTotal = Crs_GetNumCrssInDeg (Gbl.CurrentDeg.Deg.DegCod);
	 break;
     case Sco_SCOPE_CRS:
	 break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Write total number of elements *****/
   Fig_ShowHierarchyRow ("",Txt_Total,
			 "DAT_N_LINE_TOP",
                         (int) NumCtysTotal,
                         (int) NumInssTotal,
                         (int) NumCtrsTotal,
                         (int) NumDegsTotal,
			 (int) NumCrssTotal);
  }

/*****************************************************************************/
/************** Show row with number of elements in hierarchy ****************/
/*****************************************************************************/

static void Fig_ShowHierarchyRow (const char *Text1,const char *Text2,
				  const char *ClassTxt,
				  int NumCtys,	// < 0 ==> do not show number
				  int NumInss,	// < 0 ==> do not show number
				  int NumCtrs,	// < 0 ==> do not show number
				  int NumDegs,	// < 0 ==> do not show number
				  int NumCrss)	// < 0 ==> do not show number
  {
   /***** Start row and write text *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_MIDDLE\">"
		      "%s%s"
		      "</td>",
	    ClassTxt,Text1,Text2);

   /***** Write number of countries *****/
   Fig_ShowHierarchyCell (ClassTxt,NumCtys);
   Fig_ShowHierarchyCell (ClassTxt,NumInss);
   Fig_ShowHierarchyCell (ClassTxt,NumCtrs);
   Fig_ShowHierarchyCell (ClassTxt,NumDegs);
   Fig_ShowHierarchyCell (ClassTxt,NumCrss);

   /***** End row *****/
   fprintf (Gbl.F.Out,"</tr>");
  }

static void Fig_ShowHierarchyCell (const char *ClassTxt,int Num)
  {
   /***** Write number *****/
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE\">",ClassTxt);
   if (Num >= 0)
      fprintf (Gbl.F.Out,"%d",Num);
   else		// < 0 ==> do not show number
      fprintf (Gbl.F.Out,"-");
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/****************** Get and show stats about institutions ********************/
/*****************************************************************************/

static void Fig_GetAndShowInstitutionsStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_institutions;
   extern const char *Txt_Institutions;

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Institutions,NULL,
                 Hlp_ANALYTICS_Figures_institutions,Box_NOT_CLOSABLE);

   /***** Form to select type of list used to display degree photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();
   Usr_ShowFormsToSelectUsrListType (ActSeeUseGbl);

   /***** Institutions ordered by number of centres *****/
   Fig_GetAndShowInssOrderedByNumCtrs ();

   /***** Institutions ordered by number of degrees *****/
   Fig_GetAndShowInssOrderedByNumDegs ();

   /***** Institutions ordered by number of courses *****/
   Fig_GetAndShowInssOrderedByNumCrss ();

   /***** Institutions ordered by number of users in courses *****/
   Fig_GetAndShowInssOrderedByNumUsrsInCrss ();

   /***** Institutions ordered by number of users who claim to belong to them *****/
   Fig_GetAndShowInssOrderedByNumUsrsWhoClaimToBelongToThem ();

   /***** End box *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/**** Get and show stats about institutions ordered by number of centres *****/
/*****************************************************************************/

static void Fig_GetAndShowInssOrderedByNumCtrs (void)
  {
   extern const char *Txt_Institutions_by_number_of_centres;
   extern const char *Txt_Centres;
   MYSQL_RES *mysql_res;
   unsigned NumInss = 0;

   /***** Start box and table *****/
   Box_StartBoxTable ("100%",Txt_Institutions_by_number_of_centres,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Get institutions ordered by number of centres *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 NumInss =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				    "SELECT InsCod,COUNT(*) AS N"
				    " FROM centres"
				    " GROUP BY InsCod"
				    " ORDER BY N DESC");
         break;
      case Sco_SCOPE_CTY:
	 NumInss =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				    "SELECT centres.InsCod,COUNT(*) AS N"
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
	 NumInss =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				    "SELECT InsCod,COUNT(*) AS N"
				    " FROM centres"
				    " WHERE InsCod=%ld"
				    " GROUP BY InsCod"
				    " ORDER BY N DESC",
				    Gbl.CurrentIns.Ins.InsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Show institutions *****/
   Fig_ShowInss (&mysql_res,NumInss,Txt_Centres);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/**** Get and show stats about institutions ordered by number of degrees *****/
/*****************************************************************************/

static void Fig_GetAndShowInssOrderedByNumDegs (void)
  {
   extern const char *Txt_Institutions_by_number_of_degrees;
   extern const char *Txt_Degrees;
   MYSQL_RES *mysql_res;
   unsigned NumInss = 0;

   /***** Start box and table *****/
   Box_StartBoxTable ("100%",Txt_Institutions_by_number_of_degrees,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Get institutions ordered by number of degrees *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 NumInss =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				    "SELECT centres.InsCod,COUNT(*) AS N"
				    " FROM centres,degrees"
				    " WHERE centres.CtrCod=degrees.CtrCod"
				    " GROUP BY InsCod"
				    " ORDER BY N DESC");
         break;
      case Sco_SCOPE_CTY:
	 NumInss =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				    "SELECT centres.InsCod,COUNT(*) AS N"
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
	 NumInss =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				    "SELECT centres.InsCod,COUNT(*) AS N"
				    " FROM centres,degrees"
				    " WHERE centres.InsCod=%ld"
				    " AND centres.CtrCod=degrees.CtrCod"
				    " GROUP BY centres.InsCod"
				    " ORDER BY N DESC",
				    Gbl.CurrentIns.Ins.InsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Show institutions *****/
   Fig_ShowInss (&mysql_res,NumInss,Txt_Degrees);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/**** Get and show stats about institutions ordered by number of courses *****/
/*****************************************************************************/

static void Fig_GetAndShowInssOrderedByNumCrss (void)
  {
   extern const char *Txt_Institutions_by_number_of_courses;
   extern const char *Txt_Courses;
   MYSQL_RES *mysql_res;
   unsigned NumInss = 0;

   /***** Start box and table *****/
   Box_StartBoxTable ("100%",Txt_Institutions_by_number_of_courses,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Get institutions ordered by number of courses *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 NumInss =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				    "SELECT centres.InsCod,COUNT(*) AS N"
				    " FROM centres,degrees,courses"
				    " WHERE centres.CtrCod=degrees.CtrCod"
				    " AND degrees.DegCod=courses.DegCod"
				    " GROUP BY InsCod"
				    " ORDER BY N DESC");
         break;
      case Sco_SCOPE_CTY:
	 NumInss =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				    "SELECT centres.InsCod,COUNT(*) AS N"
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
	 NumInss =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				    "SELECT centres.InsCod,COUNT(*) AS N"
				    " FROM centres,degrees,courses"
				    " WHERE centres.InsCod=%ld"
				    " AND centres.CtrCod=degrees.CtrCod"
				    " AND degrees.DegCod=courses.DegCod"
				    " GROUP BY centres.InsCod"
				    " ORDER BY N DESC",
				    Gbl.CurrentIns.Ins.InsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Show institutions *****/
   Fig_ShowInss (&mysql_res,NumInss,Txt_Courses);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/***** Get and show stats about institutions ordered by users in courses *****/
/*****************************************************************************/

static void Fig_GetAndShowInssOrderedByNumUsrsInCrss (void)
  {
   extern const char *Txt_Institutions_by_number_of_users_in_courses;
   extern const char *Txt_Users;
   MYSQL_RES *mysql_res;
   unsigned NumInss = 0;

   /***** Start box and table *****/
   Box_StartBoxTable ("100%",Txt_Institutions_by_number_of_users_in_courses,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Get institutions ordered by number of users in courses *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 NumInss =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				    "SELECT centres.InsCod,COUNT(DISTINCT crs_usr.UsrCod) AS N"
				    " FROM centres,degrees,courses,crs_usr"
				    " WHERE centres.CtrCod=degrees.CtrCod"
				    " AND degrees.DegCod=courses.DegCod"
				    " AND courses.CrsCod=crs_usr.CrsCod"
				    " GROUP BY InsCod"
				    " ORDER BY N DESC");
         break;
      case Sco_SCOPE_CTY:
	 NumInss =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				    "SELECT centres.InsCod,COUNT(DISTINCT crs_usr.UsrCod) AS N"
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
	 NumInss =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				    "SELECT centres.InsCod,COUNT(DISTINCT crs_usr.UsrCod) AS N"
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
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Show institutions *****/
   Fig_ShowInss (&mysql_res,NumInss,Txt_Users);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/************* Get and show stats about institutions ordered by **************/
/************* number of users who claim to belong to them      **************/
/*****************************************************************************/

static void Fig_GetAndShowInssOrderedByNumUsrsWhoClaimToBelongToThem (void)
  {
   extern const char *Txt_Institutions_by_number_of_users_who_claim_to_belong_to_them;
   extern const char *Txt_Users;
   MYSQL_RES *mysql_res;
   unsigned NumInss;

   /***** Start box and table *****/
   Box_StartBoxTable ("100%",Txt_Institutions_by_number_of_users_who_claim_to_belong_to_them,
                      NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Get institutions ordered by number of users who claim to belong to them *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 NumInss =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				    "SELECT InsCod,COUNT(*) AS N"
				    " FROM usr_data"
				    " WHERE InsCod>0"
				    " GROUP BY InsCod"
				    " ORDER BY N DESC");
         break;
      case Sco_SCOPE_CTY:
	 NumInss =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				    "SELECT usr_data.InsCod,COUNT(*) AS N"
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
	 NumInss =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				    "SELECT InsCod,COUNT(*) AS N"
				    " FROM usr_data"
				    " WHERE InsCod=%ld"
				    " GROUP BY InsCod"
				    " ORDER BY N DESC",
				    Gbl.CurrentIns.Ins.InsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 NumInss = 0;	// Not reached. Initialized to avoid warning.
	 break;
     }

   /***** Show institutions *****/
   Fig_ShowInss (&mysql_res,NumInss,Txt_Users);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/****************** Get and show stats about institutions ********************/
/*****************************************************************************/

static void Fig_ShowInss (MYSQL_RES **mysql_res,unsigned NumInss,
		          const char *TxtFigure)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Institution;
   unsigned NumIns;
   unsigned NumOrder;
   unsigned NumberLastRow;
   unsigned NumberThisRow;
   struct Instit Ins;
   bool TRIsOpen = false;

   /***** Query database *****/
   if (NumInss)
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
	       NumberThisRow = Fig_GetInsAndStat (&Ins,*mysql_res);

	       /***** Write link to institution *****/
	       fprintf (Gbl.F.Out,"<td class=\"%s CENTER_MIDDLE\">",
			The_ClassFormInBox[Gbl.Prefs.Theme]);
	       Ins_DrawInstitutionLogoWithLink (&Ins,40);
               fprintf (Gbl.F.Out,"<br />%u</td>",
	                NumberThisRow);

	       /***** End user's cell *****/
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
	       NumberThisRow = Fig_GetInsAndStat (&Ins,*mysql_res);

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
			The_ClassFormInBox[Gbl.Prefs.Theme]);

	       /* Icon and name of this institution */
	       Frm_StartForm (ActSeeInsInf);
	       Ins_PutParamInsCod (Ins.InsCod);
	       Frm_LinkFormSubmit (Ins.ShrtName,The_ClassFormInBox[Gbl.Prefs.Theme],NULL);
	       if (Gbl.Usrs.Listing.WithPhotos)
		 {
		  Log_DrawLogo (Sco_SCOPE_INS,Ins.InsCod,Ins.ShrtName,
				40,NULL,true);
	          fprintf (Gbl.F.Out,"&nbsp;");
		 }
	       fprintf (Gbl.F.Out,"%s</a>",Ins.FullName);
	       Frm_EndForm ();

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
  }

/*****************************************************************************/
/******************** Get institution data and statistic *********************/
/*****************************************************************************/

static unsigned Fig_GetInsAndStat (struct Instit *Ins,MYSQL_RES *mysql_res)
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

static void Fig_GetAndShowDegreeTypesStats (void)
  {
   /***** Show statistic about number of degrees in each type of degree *****/
   DT_SeeDegreeTypesInStaTab ();
  }

/*****************************************************************************/
/********************* Show stats about exploration trees ********************/
/*****************************************************************************/
// TODO: add links to statistic

static void Fig_GetAndShowFileBrowsersStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_folders_and_files;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_STAT_COURSE_FILE_ZONES[Fig_NUM_STAT_CRS_FILE_ZONES];
   static const Brw_FileBrowser_t StatCrsFileZones[Fig_NUM_STAT_CRS_FILE_ZONES] =
     {
      Brw_ADMI_DOC_CRS,
      Brw_ADMI_DOC_GRP,
      Brw_ADMI_TCH_CRS,
      Brw_ADMI_TCH_GRP,
      Brw_ADMI_SHR_CRS,
      Brw_ADMI_SHR_GRP,
      Brw_ADMI_MRK_CRS,
      Brw_ADMI_MRK_GRP,
      Brw_ADMI_ASG_USR,
      Brw_ADMI_WRK_USR,
      Brw_UNKNOWN,
      Brw_ADMI_BRF_USR,
     };
   struct Fig_SizeOfFileZones SizeOfFileZones[Fig_NUM_STAT_CRS_FILE_ZONES];
   unsigned NumStat;

   /***** Get sizes of all file zones *****/
   for (NumStat = 0;
	NumStat < Fig_NUM_STAT_CRS_FILE_ZONES;
	NumStat++)
      Fig_GetSizeOfFileZoneFromDB (Gbl.Scope.Current,
	                           StatCrsFileZones[NumStat],
				   &SizeOfFileZones[NumStat]);

   /***** Start box *****/
   Box_StartBox (NULL,Txt_FIGURE_TYPES[Fig_FOLDERS_AND_FILES],NULL,
                 Hlp_ANALYTICS_Figures_folders_and_files,Box_NOT_CLOSABLE);

   /***** Write sizes of all file zones *****/
   Tbl_StartTableCenter (2);
   Fig_WriteStatsExpTreesTableHead1 ();
   for (NumStat = 0;
	NumStat < Fig_NUM_STAT_CRS_FILE_ZONES;
	NumStat++)
      Fig_WriteRowStatsFileBrowsers1 (Txt_STAT_COURSE_FILE_ZONES[NumStat],
	                              StatCrsFileZones[NumStat],
				      &SizeOfFileZones[NumStat]);
   Tbl_EndTable ();

   /***** Write sizes of all file zones per course *****/
   Tbl_StartTableCenter (2);
   Fig_WriteStatsExpTreesTableHead2 ();
   for (NumStat = 0;
	NumStat < Fig_NUM_STAT_CRS_FILE_ZONES;
	NumStat++)
      Fig_WriteRowStatsFileBrowsers2 (Txt_STAT_COURSE_FILE_ZONES[NumStat],
	                              StatCrsFileZones[NumStat],
				      &SizeOfFileZones[NumStat]);
   Tbl_EndTable ();

   /***** Write sizes of all file zones per user *****/
   Tbl_StartTableCenter (2);
   Fig_WriteStatsExpTreesTableHead3 ();
   for (NumStat = 0;
	NumStat < Fig_NUM_STAT_CRS_FILE_ZONES;
	NumStat++)
      Fig_WriteRowStatsFileBrowsers3 (Txt_STAT_COURSE_FILE_ZONES[NumStat],
	                              StatCrsFileZones[NumStat],
				      &SizeOfFileZones[NumStat]);
   Tbl_EndTable ();

   /***** End box *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/**************** Get the size of a file zone from database ******************/
/*****************************************************************************/

static void Fig_GetSizeOfFileZoneFromDB (Sco_Scope_t Scope,
                                         Brw_FileBrowser_t FileBrowser,
                                         struct Fig_SizeOfFileZones *SizeOfFileZones)
  {
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
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT CrsCod),"
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
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       (unsigned) Brw_ADMI_DOC_GRP,
			       (unsigned) Brw_ADMI_TCH_GRP,
			       (unsigned) Brw_ADMI_SHR_GRP,
			       (unsigned) Brw_ADMI_MRK_GRP);
	       break;
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_MRK_CRS:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT Cod),"
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
	    case Brw_ADMI_DOC_GRP:
	    case Brw_ADMI_TCH_GRP:
	    case Brw_ADMI_SHR_GRP:
	    case Brw_ADMI_MRK_GRP:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT crs_grp_types.CrsCod),"
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
	    case Brw_ADMI_ASG_USR:
	    case Brw_ADMI_WRK_USR:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT Cod),"
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
	    case Brw_ADMI_BRF_USR:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT -1,"
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
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT CrsCod),"
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
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       Gbl.CurrentCty.Cty.CtyCod,
			       (unsigned) Brw_ADMI_DOC_GRP,
			       (unsigned) Brw_ADMI_TCH_GRP,
			       (unsigned) Brw_ADMI_SHR_GRP,
			       (unsigned) Brw_ADMI_MRK_GRP);
	       break;
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_MRK_CRS:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT file_browser_size.Cod),"
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
	    case Brw_ADMI_DOC_GRP:
	    case Brw_ADMI_TCH_GRP:
	    case Brw_ADMI_SHR_GRP:
	    case Brw_ADMI_MRK_GRP:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT crs_grp_types.CrsCod),"
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
	    case Brw_ADMI_ASG_USR:
	    case Brw_ADMI_WRK_USR:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT file_browser_size.Cod),"
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
	    case Brw_ADMI_BRF_USR:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT -1,"
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
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT CrsCod),"
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
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       Gbl.CurrentIns.Ins.InsCod,
			       (unsigned) Brw_ADMI_DOC_GRP,
			       (unsigned) Brw_ADMI_TCH_GRP,
			       (unsigned) Brw_ADMI_SHR_GRP,
			       (unsigned) Brw_ADMI_MRK_GRP);
	       break;
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_MRK_CRS:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT file_browser_size.Cod),"
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
	    case Brw_ADMI_DOC_GRP:
	    case Brw_ADMI_TCH_GRP:
	    case Brw_ADMI_SHR_GRP:
	    case Brw_ADMI_MRK_GRP:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT crs_grp_types.CrsCod),"
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
	    case Brw_ADMI_ASG_USR:
	    case Brw_ADMI_WRK_USR:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT file_browser_size.Cod),"
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
	    case Brw_ADMI_BRF_USR:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT -1,"
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
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT CrsCod),"
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
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       Gbl.CurrentCtr.Ctr.CtrCod,
			       (unsigned) Brw_ADMI_DOC_GRP,
			       (unsigned) Brw_ADMI_TCH_GRP,
			       (unsigned) Brw_ADMI_SHR_GRP,
			       (unsigned) Brw_ADMI_MRK_GRP);
	       break;
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_MRK_CRS:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT file_browser_size.Cod),"
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
	    case Brw_ADMI_DOC_GRP:
	    case Brw_ADMI_TCH_GRP:
	    case Brw_ADMI_SHR_GRP:
	    case Brw_ADMI_MRK_GRP:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT crs_grp_types.CrsCod),"
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
	    case Brw_ADMI_ASG_USR:
	    case Brw_ADMI_WRK_USR:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT file_browser_size.Cod),"
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
	    case Brw_ADMI_BRF_USR:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT -1,"
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
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT CrsCod),"
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
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       Gbl.CurrentDeg.Deg.DegCod,
			       (unsigned) Brw_ADMI_DOC_GRP,
			       (unsigned) Brw_ADMI_TCH_GRP,
			       (unsigned) Brw_ADMI_SHR_GRP,
			       (unsigned) Brw_ADMI_MRK_GRP);
	       break;
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_MRK_CRS:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT file_browser_size.Cod),"
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
	    case Brw_ADMI_DOC_GRP:
	    case Brw_ADMI_TCH_GRP:
	    case Brw_ADMI_SHR_GRP:
	    case Brw_ADMI_MRK_GRP:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT crs_grp_types.CrsCod),"
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
	    case Brw_ADMI_ASG_USR:
	    case Brw_ADMI_WRK_USR:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT file_browser_size.Cod),"
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
	    case Brw_ADMI_BRF_USR:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT -1,"
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
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT CrsCod),"
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
			       (unsigned) Brw_ADMI_DOC_CRS,
			       (unsigned) Brw_ADMI_TCH_CRS,
			       (unsigned) Brw_ADMI_SHR_CRS,
			       (unsigned) Brw_ADMI_ASG_USR,
			       (unsigned) Brw_ADMI_WRK_USR,
			       (unsigned) Brw_ADMI_MRK_CRS,
			       Gbl.CurrentCrs.Crs.CrsCod,
			       (unsigned) Brw_ADMI_DOC_GRP,
			       (unsigned) Brw_ADMI_TCH_GRP,
			       (unsigned) Brw_ADMI_SHR_GRP,
			       (unsigned) Brw_ADMI_MRK_GRP);
	       break;
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_MRK_CRS:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT 1,"
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
	    case Brw_ADMI_DOC_GRP:
	    case Brw_ADMI_TCH_GRP:
	    case Brw_ADMI_SHR_GRP:
	    case Brw_ADMI_MRK_GRP:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT COUNT(DISTINCT crs_grp_types.CrsCod),"
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
	    case Brw_ADMI_ASG_USR:
	    case Brw_ADMI_WRK_USR:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT 1,"
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
	    case Brw_ADMI_BRF_USR:
	       DB_QuerySELECT (&mysql_res,"can not get size of a file browser",
			       "SELECT -1,"
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
	 Lay_WrongScopeExit ();
	 break;
     }

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
/*********** Write table heading for stats of exploration trees **************/
/*****************************************************************************/

static void Fig_WriteStatsExpTreesTableHead1 (void)
  {
   extern const char *Txt_File_zones;
   extern const char *Txt_Courses;
   extern const char *Txt_Groups;
   extern const char *Txt_Users;
   extern const char *Txt_Max_levels;
   extern const char *Txt_Folders;
   extern const char *Txt_Files;
   extern const char *Txt_Size;

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
                      "</tr>",
            Txt_File_zones,
            Txt_Courses,
            Txt_Groups,
            Txt_Users,
            Txt_Max_levels,
            Txt_Folders,
            Txt_Files,
            Txt_Size);
  }

static void Fig_WriteStatsExpTreesTableHead2 (void)
  {
   extern const char *Txt_File_zones;
   extern const char *Txt_Folders;
   extern const char *Txt_Files;
   extern const char *Txt_Size;
   extern const char *Txt_course;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
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
                      "</tr>",
            Txt_File_zones,
            Txt_Folders,Txt_course,
            Txt_Files,Txt_course,
            Txt_Size,Txt_course);
  }

static void Fig_WriteStatsExpTreesTableHead3 (void)
  {
   extern const char *Txt_File_zones;
   extern const char *Txt_Folders;
   extern const char *Txt_Files;
   extern const char *Txt_Size;
   extern const char *Txt_user[Usr_NUM_SEXS];

   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
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
                      "</tr>",
            Txt_File_zones,
            Txt_Folders,Txt_user[Usr_SEX_UNKNOWN],
            Txt_Files,Txt_user[Usr_SEX_UNKNOWN],
            Txt_Size,Txt_user[Usr_SEX_UNKNOWN]);
  }

/*****************************************************************************/
/*************** Write a row of stats of exploration trees *******************/
/*****************************************************************************/

static void Fig_WriteRowStatsFileBrowsers1 (const char *NameOfFileZones,
					    Brw_FileBrowser_t FileZone,
                                            struct Fig_SizeOfFileZones *SizeOfFileZones)
  {
   char StrNumCrss[10 + 1];
   char StrNumGrps[10 + 1];
   char StrNumUsrs[10 + 1];
   char FileSizeStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];
   char *Class = (FileZone == Brw_UNKNOWN) ? "DAT_N_LINE_TOP" :
	                                     "DAT";

   Fil_WriteFileSizeFull ((double) SizeOfFileZones->Size,FileSizeStr);

   if (SizeOfFileZones->NumCrss == -1)	// Not applicable
      Str_Copy (StrNumCrss,"-",
                10);
   else
      snprintf (StrNumCrss,sizeof (StrNumCrss),
	        "%d",
		SizeOfFileZones->NumCrss);

   if (SizeOfFileZones->NumGrps == -1)	// Not applicable
      Str_Copy (StrNumGrps,"-",
                10);
   else
      snprintf (StrNumGrps,sizeof (StrNumGrps),
	        "%d",
		SizeOfFileZones->NumGrps);

   if (SizeOfFileZones->NumUsrs == -1)	// Not applicable
      Str_Copy (StrNumUsrs,"-",
                10);
   else
      snprintf (StrNumUsrs,sizeof (StrNumUsrs),
	        "%d",
		SizeOfFileZones->NumUsrs);

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
	              "</tr>",
            Class,NameOfFileZones,
            Class,StrNumCrss,
            Class,StrNumGrps,
            Class,StrNumUsrs,
            Class,SizeOfFileZones->MaxLevels,
            Class,SizeOfFileZones->NumFolders,
            Class,SizeOfFileZones->NumFiles,
            Class,FileSizeStr);
  }


static void Fig_WriteRowStatsFileBrowsers2 (const char *NameOfFileZones,
					    Brw_FileBrowser_t FileZone,
                                            struct Fig_SizeOfFileZones *SizeOfFileZones)
  {
   char StrNumFoldersPerCrs[10 + 1];
   char StrNumFilesPerCrs[10 + 1];
   char FileSizePerCrsStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];
   char *Class = (FileZone == Brw_UNKNOWN) ? "DAT_N_LINE_TOP" :
	                                     "DAT";

   if (SizeOfFileZones->NumCrss == -1)	// Not applicable
     {
      Str_Copy (StrNumFoldersPerCrs,"-",
                10);
      Str_Copy (StrNumFilesPerCrs,"-",
                10);
      Str_Copy (FileSizePerCrsStr,"-",
                Fil_MAX_BYTES_FILE_SIZE_STRING);
     }
   else
     {
      snprintf (StrNumFoldersPerCrs,sizeof (StrNumFoldersPerCrs),
	        "%.1f",
                SizeOfFileZones->NumCrss ? (double) SizeOfFileZones->NumFolders /
        	                           (double) SizeOfFileZones->NumCrss :
        	                           0.0);
      snprintf (StrNumFilesPerCrs,sizeof (StrNumFilesPerCrs),
	        "%.1f",
                SizeOfFileZones->NumCrss ? (double) SizeOfFileZones->NumFiles /
        	                           (double) SizeOfFileZones->NumCrss :
        	                           0.0);
      Fil_WriteFileSizeFull (SizeOfFileZones->NumCrss ? (double) SizeOfFileZones->Size /
	                                                (double) SizeOfFileZones->NumCrss :
	                                                0.0,
	                     FileSizePerCrsStr);
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
	              "</tr>",
            Class,NameOfFileZones,
            Class,StrNumFoldersPerCrs,
            Class,StrNumFilesPerCrs,
            Class,FileSizePerCrsStr);
  }


static void Fig_WriteRowStatsFileBrowsers3 (const char *NameOfFileZones,
					    Brw_FileBrowser_t FileZone,
                                            struct Fig_SizeOfFileZones *SizeOfFileZones)
  {
   char StrNumFoldersPerUsr[10 + 1];
   char StrNumFilesPerUsr[10 + 1];
   char FileSizePerUsrStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];
   char *Class = (FileZone == Brw_UNKNOWN) ? "DAT_N_LINE_TOP" :
	                                     "DAT";

   if (SizeOfFileZones->NumUsrs == -1)	// Not applicable
     {
      Str_Copy (StrNumFoldersPerUsr,"-",
                10);
      Str_Copy (StrNumFilesPerUsr,"-",
                10);
      Str_Copy (FileSizePerUsrStr,"-",
                Fil_MAX_BYTES_FILE_SIZE_STRING);
     }
   else
     {
      snprintf (StrNumFoldersPerUsr,sizeof (StrNumFoldersPerUsr),
	        "%.1f",
                SizeOfFileZones->NumUsrs ? (double) SizeOfFileZones->NumFolders /
        	                           (double) SizeOfFileZones->NumUsrs :
        	                           0.0);
      snprintf (StrNumFilesPerUsr,sizeof (StrNumFilesPerUsr),
	        "%.1f",
                SizeOfFileZones->NumUsrs ? (double) SizeOfFileZones->NumFiles /
        	                           (double) SizeOfFileZones->NumUsrs :
        	                           0.0);
      Fil_WriteFileSizeFull (SizeOfFileZones->NumUsrs ? (double) SizeOfFileZones->Size /
	                                                (double) SizeOfFileZones->NumUsrs :
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
	              "</tr>",
            Class,NameOfFileZones,
            Class,StrNumFoldersPerUsr,
            Class,StrNumFilesPerUsr,
            Class,FileSizePerUsrStr);
  }

/*****************************************************************************/
/************ Show stats about Open Educational Resources (OERs) *************/
/*****************************************************************************/

static void Fig_GetAndShowOERsStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_open_educational_resources_oer;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_License;
   extern const char *Txt_No_of_private_files;
   extern const char *Txt_No_of_public_files;
   extern const char *Txt_LICENSES[Brw_NUM_LICENSES];
   Brw_License_t License;
   unsigned long NumFiles[2];

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_OER],NULL,
                      Hlp_ANALYTICS_Figures_open_educational_resources_oer,Box_NOT_CLOSABLE,2);

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
      Fig_GetNumberOfOERsFromDB (Gbl.Scope.Current,License,NumFiles);

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

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/**************** Get the size of a file zone from database ******************/
/*****************************************************************************/

static void Fig_GetNumberOfOERsFromDB (Sco_Scope_t Scope,Brw_License_t License,unsigned long NumFiles[2])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows = 0;	// Initialized to avoid warning
   unsigned NumRow;
   unsigned Public;

   /***** Get the size of a file browser *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         NumRows =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get number of OERs",
				    "SELECT Public,COUNT(*)"
				    " FROM files"
				    " WHERE License=%u"
				    " GROUP BY Public",
				    (unsigned) License);
         break;
      case Sco_SCOPE_CTY:
         NumRows =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get number of OERs",
				    "SELECT files.Public,COUNT(*)"
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
				    (unsigned) Brw_ADMI_DOC_CRS,
				    (unsigned) Brw_ADMI_SHR_CRS,
				    (unsigned) License);
         break;
      case Sco_SCOPE_INS:
         NumRows =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get number of OERs",
				    "SELECT files.Public,COUNT(*)"
				    " FROM centres,degrees,courses,files"
				    " WHERE centres.InsCod=%ld"
				    " AND centres.CtrCod=degrees.CtrCod"
				    " AND degrees.DegCod=courses.DegCod"
				    " AND courses.CrsCod=files.Cod"
				    " AND files.FileBrowser IN (%u,%u)"
				    " AND files.License=%u"
				    " GROUP BY files.Public",
				    Gbl.CurrentIns.Ins.InsCod,
				    (unsigned) Brw_ADMI_DOC_CRS,
				    (unsigned) Brw_ADMI_SHR_CRS,
				    (unsigned) License);
         break;
      case Sco_SCOPE_CTR:
         NumRows =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get number of OERs",
				    "SELECT files.Public,COUNT(*)"
				    " FROM degrees,courses,files"
				    " WHERE degrees.CtrCod=%ld"
				    " AND degrees.DegCod=courses.DegCod"
				    " AND courses.CrsCod=files.Cod"
				    " AND files.FileBrowser IN (%u,%u)"
				    " AND files.License=%u"
				    " GROUP BY files.Public",
				    Gbl.CurrentCtr.Ctr.CtrCod,
				    (unsigned) Brw_ADMI_DOC_CRS,
				    (unsigned) Brw_ADMI_SHR_CRS,
				    (unsigned) License);
         break;
      case Sco_SCOPE_DEG:
         NumRows =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get number of OERs",
				    "SELECT files.Public,COUNT(*)"
				    " FROM courses,files"
				    " WHERE courses.DegCod=%ld"
				    " AND courses.CrsCod=files.Cod"
				    " AND files.FileBrowser IN (%u,%u)"
				    " AND files.License=%u"
				    " GROUP BY files.Public",
				    Gbl.CurrentDeg.Deg.DegCod,
				    (unsigned) Brw_ADMI_DOC_CRS,
				    (unsigned) Brw_ADMI_SHR_CRS,
				    (unsigned) License);
         break;
      case Sco_SCOPE_CRS:
         NumRows =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get number of OERs",
				    "SELECT Public,COUNT(*)"
				    " FROM files"
				    " WHERE Cod=%ld"
				    " AND FileBrowser IN (%u,%u)"
				    " AND License=%u"
				    " GROUP BY Public",
				    Gbl.CurrentCrs.Crs.CrsCod,
				    (unsigned) Brw_ADMI_DOC_CRS,
				    (unsigned) Brw_ADMI_SHR_CRS,
				    (unsigned) License);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

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

static void Fig_GetAndShowAssignmentsStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_assignments;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Number_of_BR_assignments;
   extern const char *Txt_Number_of_BR_courses_with_BR_assignments;
   extern const char *Txt_Average_number_BR_of_ASSIG_BR_per_course;
   extern const char *Txt_Number_of_BR_notifications;
   unsigned NumAssignments;
   unsigned NumNotif;
   unsigned NumCoursesWithAssignments = 0;
   float NumAssignmentsPerCourse = 0.0;

   /***** Get the number of assignments from this location *****/
   if ((NumAssignments = Asg_GetNumAssignments (Gbl.Scope.Current,&NumNotif)))
      if ((NumCoursesWithAssignments = Asg_GetNumCoursesWithAssignments (Gbl.Scope.Current)) != 0)
         NumAssignmentsPerCourse = (float) NumAssignments / (float) NumCoursesWithAssignments;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_ASSIGNMENTS],NULL,
                      Hlp_ANALYTICS_Figures_assignments,Box_NOT_CLOSABLE,2);

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

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/************************ Show figures about projects ************************/
/*****************************************************************************/

static void Fig_GetAndShowProjectsStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_projects;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Number_of_BR_projects;
   extern const char *Txt_Number_of_BR_courses_with_BR_projects;
   extern const char *Txt_Average_number_BR_of_projects_BR_per_course;
   unsigned NumProjects;
   unsigned NumCoursesWithProjects = 0;
   float NumProjectsPerCourse = 0.0;

   /***** Get the number of projects from this location *****/
   if ((NumProjects = Prj_GetNumProjects (Gbl.Scope.Current)))
      if ((NumCoursesWithProjects = Prj_GetNumCoursesWithProjects (Gbl.Scope.Current)) != 0)
         NumProjectsPerCourse = (float) NumProjects / (float) NumCoursesWithProjects;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_PROJECTS],NULL,
                      Hlp_ANALYTICS_Figures_projects,Box_NOT_CLOSABLE,2);

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
                      "</tr>",
            Txt_Number_of_BR_projects,
            Txt_Number_of_BR_courses_with_BR_projects,
            Txt_Average_number_BR_of_projects_BR_per_course);

   /***** Write number of projects *****/
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
                      "</tr>",
            NumProjects,
            NumCoursesWithProjects,
            NumProjectsPerCourse);

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/********************** Show figures about test questions ********************/
/*****************************************************************************/

static void Fig_GetAndShowTestsStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_tests;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
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

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_TESTS],NULL,
                      Hlp_ANALYTICS_Figures_tests,Box_NOT_CLOSABLE,2);

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
      /***** Get the stats about test questions from this location *****/
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

   /***** Get the stats about test questions from this location *****/
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

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/*************************** Show stats about games **************************/
/*****************************************************************************/

static void Fig_GetAndShowGamesStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_games;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Number_of_BR_games;
   extern const char *Txt_Number_of_BR_courses_with_BR_games;
   extern const char *Txt_Average_number_BR_of_games_BR_per_course;
   unsigned NumGames;
   unsigned NumCoursesWithGames = 0;
   float NumGamesPerCourse = 0.0;

   /***** Get the number of games from this location *****/
   if ((NumGames = Gam_GetNumGames (Gbl.Scope.Current)))
      if ((NumCoursesWithGames = Gam_GetNumCoursesWithGames (Gbl.Scope.Current)) != 0)
         NumGamesPerCourse = (float) NumGames / (float) NumCoursesWithGames;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_GAMES],NULL,
                      Hlp_ANALYTICS_Figures_games,Box_NOT_CLOSABLE,2);

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
                      "</tr>",
            Txt_Number_of_BR_games,
            Txt_Number_of_BR_courses_with_BR_games,
            Txt_Average_number_BR_of_games_BR_per_course);

   /***** Write number of games *****/
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
                      "</tr>",
            NumGames,
            NumCoursesWithGames,
            NumGamesPerCourse);

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/******************** Get and show number of social notes ********************/
/*****************************************************************************/

static void Fig_GetAndShowSocialActivityStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_timeline;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Type;
   extern const char *Txt_No_of_social_posts;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Txt_No_of_posts_BR_per_user;
   extern const char *Txt_TIMELINE_NOTE[TL_NUM_NOTE_TYPES];
   extern const char *Txt_Total;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   TL_NoteType_t NoteType;
   unsigned long NumSocialNotes;
   unsigned long NumRows;
   unsigned NumUsrs;
   unsigned NumUsrsTotal;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_TIMELINE],NULL,
                      Hlp_ANALYTICS_Figures_timeline,Box_NOT_CLOSABLE,2);

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
   for (NoteType = (TL_NoteType_t) 0;
	NoteType < TL_NUM_NOTE_TYPES;
	NoteType++)
     {
      switch (Gbl.Scope.Current)
	{
	 case Sco_SCOPE_SYS:
	    NumRows = DB_QuerySELECT (&mysql_res,"can not get number of social notes",
				      "SELECT COUNT(*),"
					     "COUNT(DISTINCT UsrCod)"
				      " FROM social_notes WHERE NoteType=%u",
				      NoteType);
	    break;
	 case Sco_SCOPE_CTY:
	    NumRows = DB_QuerySELECT (&mysql_res,"can not get number of social notes",
				      "SELECT COUNT(DISTINCT social_notes.NotCod),"
					     "COUNT(DISTINCT social_notes.UsrCod)"
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
	    NumRows = DB_QuerySELECT (&mysql_res,"can not get number of social notes",
				      "SELECT COUNT(DISTINCT social_notes.NotCod),"
					     "COUNT(DISTINCT social_notes.UsrCod)"
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
	    NumRows = DB_QuerySELECT (&mysql_res,"can not get number of social notes",
				      "SELECT COUNT(DISTINCT social_notes.NotCod),"
					     "COUNT(DISTINCT social_notes.UsrCod)"
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
	    NumRows = DB_QuerySELECT (&mysql_res,"can not get number of social notes",
				      "SELECT COUNT(DISTINCT social_notes.NotCod),"
					     "COUNT(DISTINCT social_notes.UsrCod)"
				      " FROM courses,crs_usr,social_notes"
				      " WHERE courses.DegCod=%ld"
				      " AND courses.CrsCod=crs_usr.CrsCod"
				      " AND crs_usr.UsrCod=social_notes.UsrCod"
				      " AND social_notes.NoteType=%u",
				      Gbl.CurrentDeg.Deg.DegCod,
				      (unsigned) NoteType);
	    break;
	 case Sco_SCOPE_CRS:
	    NumRows = DB_QuerySELECT (&mysql_res,"can not get number of social notes",
				      "SELECT COUNT(DISTINCT social_notes.NotCod),"
					     "COUNT(DISTINCT social_notes.UsrCod)"
				      " FROM crs_usr,social_notes"
				      " WHERE crs_usr.CrsCod=%ld"
				      " AND crs_usr.UsrCod=social_notes.UsrCod"
				      " AND social_notes.NoteType=%u",
				      Gbl.CurrentCrs.Crs.CrsCod,
				      (unsigned) NoteType);
	    break;
	 default:
	    Lay_WrongScopeExit ();
	    NumRows = 0;	// Initialized to avoid warning
	    break;
	}
      NumSocialNotes = 0;
      NumUsrs = 0;

      if (NumRows)
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
               Txt_TIMELINE_NOTE[NoteType],
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
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get number of social notes",
				   "SELECT COUNT(*),"
					  "COUNT(DISTINCT UsrCod)"
				   " FROM social_notes");
	 break;
      case Sco_SCOPE_CTY:
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get number of social notes",
				   "SELECT COUNT(DISTINCT social_notes.NotCod),"
					  "COUNT(DISTINCT social_notes.UsrCod)"
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
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get number of social notes",
				   "SELECT COUNT(DISTINCT social_notes.NotCod),"
					  "COUNT(DISTINCT social_notes.UsrCod)"
				   " FROM centres,degrees,courses,crs_usr,social_notes"
				   " WHERE centres.InsCod=%ld"
				   " AND centres.CtrCod=degrees.CtrCod"
				   " AND degrees.DegCod=courses.DegCod"
				   " AND courses.CrsCod=crs_usr.CrsCod"
				   " AND crs_usr.UsrCod=social_notes.UsrCod",
				   Gbl.CurrentIns.Ins.InsCod);
	 break;
      case Sco_SCOPE_CTR:
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get number of social notes",
				   "SELECT COUNT(DISTINCT social_notes.NotCod),"
					  "COUNT(DISTINCT social_notes.UsrCod)"
				   " FROM degrees,courses,crs_usr,social_notes"
				   " WHERE degrees.CtrCod=%ld"
				   " AND degrees.DegCod=courses.DegCod"
				   " AND courses.CrsCod=crs_usr.CrsCod"
				   " AND crs_usr.UsrCod=social_notes.UsrCod",
				   Gbl.CurrentCtr.Ctr.CtrCod);
	 break;
      case Sco_SCOPE_DEG:
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get number of social notes",
				   "SELECT COUNT(DISTINCT social_notes.NotCod),"
					  "COUNT(DISTINCT social_notes.UsrCod)"
				   " FROM courses,crs_usr,social_notes"
				   " WHERE courses.DegCod=%ld"
				   " AND courses.CrsCod=crs_usr.CrsCod"
				   " AND crs_usr.UsrCod=social_notes.UsrCod",
				   Gbl.CurrentDeg.Deg.DegCod);
	 break;
      case Sco_SCOPE_CRS:
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get number of social notes",
				   "SELECT COUNT(DISTINCT social_notes.NotCod),"
					  "COUNT(DISTINCT social_notes.UsrCod)"
				   " FROM crs_usr,social_notes"
				   " WHERE crs_usr.CrsCod=%ld"
				   " AND crs_usr.UsrCod=social_notes.UsrCod",
				   Gbl.CurrentCrs.Crs.CrsCod);
	 break;
      default:
	 Lay_WrongScopeExit ();
	 NumRows = 0;	// Initialized to avoid warning
	 break;
     }
   NumSocialNotes = 0;
   NumUsrs = 0;

   if (NumRows)
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

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/************** Get and show number of following and followers ***************/
/*****************************************************************************/

static void Fig_GetAndShowFollowStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_followed_followers;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned Fol;
   unsigned NumUsrsTotal;
   unsigned NumUsrs;
   float Average;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_FOLLOW],NULL,
                      Hlp_ANALYTICS_Figures_followed_followers,Box_NOT_CLOSABLE,2);

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
	    NumUsrs =
	    (unsigned) DB_QueryCOUNT ("can not get the total number"
				      " of following/followers",
				      "SELECT COUNT(DISTINCT %s) FROM usr_follow",
				      FieldDB[Fol]);
	    break;
	 case Sco_SCOPE_CTY:
	    NumUsrs =
	    (unsigned) DB_QueryCOUNT ("can not get the total number"
				      " of following/followers",
				      "SELECT COUNT(DISTINCT usr_follow.%s)"
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
	    NumUsrs =
	    (unsigned) DB_QueryCOUNT ("can not get the total number"
				      " of following/followers",
				      "SELECT COUNT(DISTINCT usr_follow.%s)"
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
	    NumUsrs =
	    (unsigned) DB_QueryCOUNT ("can not get the total number"
				      " of following/followers",
				      "SELECT COUNT(DISTINCT usr_follow.%s)"
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
	    NumUsrs =
	    (unsigned) DB_QueryCOUNT ("can not get the total number"
				      " of following/followers",
				      "SELECT COUNT(DISTINCT usr_follow.%s)"
				      " FROM courses,crs_usr,usr_follow"
				      " WHERE courses.DegCod=%ld"
				      " AND courses.CrsCod=crs_usr.CrsCod"
				      " AND crs_usr.UsrCod=usr_follow.%s",
				      FieldDB[Fol],
				      Gbl.CurrentDeg.Deg.DegCod,
				      FieldDB[Fol]);
	    break;
	 case Sco_SCOPE_CRS:
	    NumUsrs =
	    (unsigned) DB_QueryCOUNT ("can not get the total number"
				      " of following/followers",
				      "SELECT COUNT(DISTINCT usr_follow.%s)"
				      " FROM crs_usr,usr_follow"
				      " WHERE crs_usr.CrsCod=%ld"
				      " AND crs_usr.UsrCod=usr_follow.%s",
				      FieldDB[Fol],
				      Gbl.CurrentCrs.Crs.CrsCod,
				      FieldDB[Fol]);
	    break;
	 default:
	    Lay_WrongScopeExit ();
	    NumUsrs = 0;	// Not reached. Initialized to av oid warning
	    break;
	}

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
	    DB_QuerySELECT (&mysql_res,"can not get number of questions"
				       " per survey",
			    "SELECT AVG(N) FROM "
			    "(SELECT COUNT(%s) AS N"
			    " FROM usr_follow"
			    " GROUP BY %s) AS F",
			    FieldDB[Fol],
			    FieldDB[1 - Fol]);
	    break;
	 case Sco_SCOPE_CTY:
	    DB_QuerySELECT (&mysql_res,"can not get number of questions"
				       " per survey",
			    "SELECT AVG(N) FROM "
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
	    DB_QuerySELECT (&mysql_res,"can not get number of questions"
				       " per survey",
			    "SELECT AVG(N) FROM "
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
	    DB_QuerySELECT (&mysql_res,"can not get number of questions"
				       " per survey",
			    "SELECT AVG(N) FROM "
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
	    DB_QuerySELECT (&mysql_res,"can not get number of questions"
				       " per survey",
			    "SELECT AVG(N) FROM "
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
	    DB_QuerySELECT (&mysql_res,"can not get number of questions"
				       " per survey",
			    "SELECT AVG(N) FROM "
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
	    Lay_WrongScopeExit ();
	    break;
	}

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

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/************************** Show figures about forums ************************/
/*****************************************************************************/

static void Fig_GetAndShowForumStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_forums;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Scope;
   extern const char *Txt_Forums;
   extern const char *Txt_No_of_forums;
   extern const char *Txt_No_of_threads;
   extern const char *Txt_No_of_posts;
   extern const char *Txt_Number_of_BR_notifications;
   extern const char *Txt_No_of_threads_BR_per_forum;
   extern const char *Txt_No_of_posts_BR_per_thread;
   extern const char *Txt_No_of_posts_BR_per_forum;
   struct Fig_FiguresForum FiguresForum;

   /***** Reset total stats *****/
   FiguresForum.NumForums           = 0;
   FiguresForum.NumThreads          = 0;
   FiguresForum.NumPosts            = 0;
   FiguresForum.NumUsrsToBeNotifiedByEMail = 0;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_FORUMS],NULL,
                      Hlp_ANALYTICS_Figures_forums,Box_NOT_CLOSABLE,2);

   /***** Write table heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_TOP\" style=\"width:20px;\">"
                      "<img src=\"%s/comments.svg\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO16x16\" />"
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
            Cfg_URL_ICON_PUBLIC,
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
         Fig_ShowStatOfAForumType (For_FORUM_GLOBAL_USRS     ,-1L,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_GLOBAL_TCHS     ,-1L,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM__SWAD__USRS       ,-1L,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM__SWAD__TCHS       ,-1L,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_INSTIT_USRS,-1L,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_INSTIT_TCHS,-1L,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_CENTRE_USRS     ,-1L,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_CENTRE_TCHS     ,-1L,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_DEGREE_USRS     ,-1L,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_DEGREE_TCHS     ,-1L,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_COURSE_USRS     ,-1L,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_COURSE_TCHS     ,-1L,-1L,-1L,-1L,-1L,&FiguresForum);
         break;
      case Sco_SCOPE_CTY:
         Fig_ShowStatOfAForumType (For_FORUM_INSTIT_USRS,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_INSTIT_TCHS,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_CENTRE_USRS     ,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_CENTRE_TCHS     ,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_DEGREE_USRS     ,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_DEGREE_TCHS     ,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_COURSE_USRS     ,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_COURSE_TCHS     ,Gbl.CurrentCty.Cty.CtyCod,-1L,-1L,-1L,-1L,&FiguresForum);
         break;
      case Sco_SCOPE_INS:
         Fig_ShowStatOfAForumType (For_FORUM_INSTIT_USRS,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_INSTIT_TCHS,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_CENTRE_USRS     ,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_CENTRE_TCHS     ,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_DEGREE_USRS     ,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_DEGREE_TCHS     ,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_COURSE_USRS     ,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_COURSE_TCHS     ,-1L,Gbl.CurrentIns.Ins.InsCod,-1L,-1L,-1L,&FiguresForum);
         break;
      case Sco_SCOPE_CTR:
         Fig_ShowStatOfAForumType (For_FORUM_CENTRE_USRS,-1L,-1L,Gbl.CurrentCtr.Ctr.CtrCod,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_CENTRE_TCHS,-1L,-1L,Gbl.CurrentCtr.Ctr.CtrCod,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_DEGREE_USRS,-1L,-1L,Gbl.CurrentCtr.Ctr.CtrCod,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_DEGREE_TCHS,-1L,-1L,Gbl.CurrentCtr.Ctr.CtrCod,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_COURSE_USRS,-1L,-1L,Gbl.CurrentCtr.Ctr.CtrCod,-1L,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_COURSE_TCHS,-1L,-1L,Gbl.CurrentCtr.Ctr.CtrCod,-1L,-1L,&FiguresForum);
         break;
      case Sco_SCOPE_DEG:
         Fig_ShowStatOfAForumType (For_FORUM_DEGREE_USRS,-1L,-1L,-1L,Gbl.CurrentDeg.Deg.DegCod,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_DEGREE_TCHS,-1L,-1L,-1L,Gbl.CurrentDeg.Deg.DegCod,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_COURSE_USRS,-1L,-1L,-1L,Gbl.CurrentDeg.Deg.DegCod,-1L,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_COURSE_TCHS,-1L,-1L,-1L,Gbl.CurrentDeg.Deg.DegCod,-1L,&FiguresForum);
         break;
      case Sco_SCOPE_CRS:
         Fig_ShowStatOfAForumType (For_FORUM_COURSE_USRS,-1L,-1L,-1L,-1L,Gbl.CurrentCrs.Crs.CrsCod,&FiguresForum);
         Fig_ShowStatOfAForumType (For_FORUM_COURSE_TCHS,-1L,-1L,-1L,-1L,Gbl.CurrentCrs.Crs.CrsCod,&FiguresForum);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   Fig_WriteForumTotalStats (&FiguresForum);

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/************************* Show stats of a forum type ************************/
/*****************************************************************************/

static void Fig_ShowStatOfAForumType (For_ForumType_t ForumType,
                                      long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod,
                                      struct Fig_FiguresForum *FiguresForum)
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
         Fig_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "comments.svg",FiguresForum,
                                      Txt_General,"");
         break;
      case For_FORUM_GLOBAL_TCHS:
         Fig_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "comments.svg",FiguresForum,
                                      Txt_General,Txt_only_teachers);
         break;
      case For_FORUM__SWAD__USRS:
         Fig_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "swad64x64.png",FiguresForum,
                                      Cfg_PLATFORM_SHORT_NAME,"");
         break;
      case For_FORUM__SWAD__TCHS:
         Fig_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "swad64x64.png",FiguresForum,
                                      Cfg_PLATFORM_SHORT_NAME,Txt_only_teachers);
         break;
      case For_FORUM_INSTIT_USRS:
         Fig_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "university.svg",FiguresForum,
                                      Txt_Institutions,"");
         break;
      case For_FORUM_INSTIT_TCHS:
         Fig_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "university.svg",FiguresForum,
                                      Txt_Institutions,Txt_only_teachers);
         break;
      case For_FORUM_CENTRE_USRS:
         Fig_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "building.svg",FiguresForum,
                                      Txt_Centres,"");
         break;
      case For_FORUM_CENTRE_TCHS:
         Fig_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "building.svg",FiguresForum,
                                      Txt_Centres,Txt_only_teachers);
         break;
      case For_FORUM_DEGREE_USRS:
         Fig_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "graduation-cap.svg",FiguresForum,
                                      Txt_Degrees,"");
         break;
      case For_FORUM_DEGREE_TCHS:
         Fig_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "graduation-cap.svg",FiguresForum,
                                      Txt_Degrees,Txt_only_teachers);
         break;
      case For_FORUM_COURSE_USRS:
         Fig_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "list-ol.svg",FiguresForum,
                                      Txt_Courses,"");
         break;
      case For_FORUM_COURSE_TCHS:
         Fig_WriteForumTitleAndStats (ForumType,CtyCod,InsCod,CtrCod,DegCod,CrsCod,
                                      "list-ol.svg",FiguresForum,
                                      Txt_Courses,Txt_only_teachers);
         break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/******************* Write title and stats of a forum type *******************/
/*****************************************************************************/

static void Fig_WriteForumTitleAndStats (For_ForumType_t ForumType,
                                         long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod,
                                         const char *Icon,struct Fig_FiguresForum *FiguresForum,
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
   FiguresForum->NumForums                  += NumForums;
   FiguresForum->NumThreads                 += NumThreads;
   FiguresForum->NumPosts                   += NumPosts;
   FiguresForum->NumUsrsToBeNotifiedByEMail += NumUsrsToBeNotifiedByEMail;

   /***** Write forum name and stats *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"LEFT_TOP\" style=\"width:20px;\">"
                      "<img src=\"%s/%s\""
                      " alt=\"%s%s\" title=\"%s%s\""
                      " class=\"ICO16x16\" />"
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
            Cfg_URL_ICON_PUBLIC,Icon,
            ForumName1,ForumName2,
            ForumName1,ForumName2,
            ForumName1,ForumName2,
            NumForums,NumThreads,NumPosts,NumUsrsToBeNotifiedByEMail,
            NumThrsPerForum,NumPostsPerThread,NumPostsPerForum);
  }

/*****************************************************************************/
/******************* Write title and stats of a forum type *******************/
/*****************************************************************************/

static void Fig_WriteForumTotalStats (struct Fig_FiguresForum *FiguresForum)
  {
   extern const char *Txt_Total;
   float NumThrsPerForum;
   float NumPostsPerThread;
   float NumPostsPerForum;

   /***** Compute number of threads per forum, number of posts per forum and number of posts per thread *****/
   NumThrsPerForum  = (FiguresForum->NumForums ? (float) FiguresForum->NumThreads / (float) FiguresForum->NumForums :
	                                       0.0);
   NumPostsPerThread = (FiguresForum->NumThreads ? (float) FiguresForum->NumPosts / (float) FiguresForum->NumThreads :
	                                         0.0);
   NumPostsPerForum = (FiguresForum->NumForums ? (float) FiguresForum->NumPosts / (float) FiguresForum->NumForums :
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
            FiguresForum->NumForums,
            FiguresForum->NumThreads,
            FiguresForum->NumPosts,
            FiguresForum->NumUsrsToBeNotifiedByEMail,
            NumThrsPerForum,
            NumPostsPerThread,
            NumPostsPerForum);
  }

/*****************************************************************************/
/****** Get and show number of users who want to be notified by email ********/
/*****************************************************************************/

static void Fig_GetAndShowNumUsrsPerNotifyEvent (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_notifications;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Event;
   extern const char *Txt_NOTIFY_EVENTS_PLURAL[Ntf_NUM_NOTIFY_EVENTS];
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Txt_Number_of_BR_events;
   extern const char *Txt_Number_of_BR_emails;
   extern const char *Txt_Total;
   Ntf_NotifyEvent_t NotifyEvent;
   char *SubQuery;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrsTotal;
   unsigned NumUsrsTotalWhoWantToBeNotifiedByEMailAboutSomeEvent;
   unsigned NumUsrs[Ntf_NUM_NOTIFY_EVENTS];
   unsigned NumEventsTotal = 0;
   unsigned NumEvents[Ntf_NUM_NOTIFY_EVENTS];
   unsigned NumMailsTotal = 0;
   unsigned NumMails[Ntf_NUM_NOTIFY_EVENTS];

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_NOTIFY_EVENTS],NULL,
                      Hlp_ANALYTICS_Figures_notifications,Box_NOT_CLOSABLE,2);

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
   NumUsrsTotalWhoWantToBeNotifiedByEMailAboutSomeEvent =
   Fig_GetNumUsrsWhoChoseAnOption ("usr_data.EmailNtfEvents<>0");

   /***** For each notify event... *****/
   for (NotifyEvent = (Ntf_NotifyEvent_t) 1;
	NotifyEvent < Ntf_NUM_NOTIFY_EVENTS;
	NotifyEvent++) // 0 is reserved for Ntf_EVENT_UNKNOWN
     {
      /* Get the number of users who want to be notified by email on this event, from database */
      if (asprintf (&SubQuery,"((usr_data.EmailNtfEvents & %u)<>0)",
	            (1 << NotifyEvent)) < 0)
	 Lay_NotEnoughMemoryExit ();
      NumUsrs[NotifyEvent] = Fig_GetNumUsrsWhoChoseAnOption (SubQuery);
      free ((void *) SubQuery);

      /* Get number of notifications by email from database */
      switch (Gbl.Scope.Current)
        {
         case Sco_SCOPE_SYS:
            DB_QuerySELECT (&mysql_res,"can not get the number"
        			       " of notifications by email",
        		    "SELECT SUM(NumEvents),SUM(NumMails)"
                            " FROM sta_notif"
                            " WHERE NotifyEvent=%u",
			    (unsigned) NotifyEvent);
            break;
	 case Sco_SCOPE_CTY:
            DB_QuerySELECT (&mysql_res,"can not get the number"
        			       " of notifications by email",
        		    "SELECT SUM(sta_notif.NumEvents),SUM(sta_notif.NumMails)"
                            " FROM institutions,centres,degrees,sta_notif"
                            " WHERE institutions.CtyCod=%ld"
                            " AND institutions.InsCod=centres.InsCod"
                            " AND centres.CtrCod=degrees.CtrCod"
                            " AND degrees.DegCod=sta_notif.DegCod"
                            " AND sta_notif.NotifyEvent=%u",
			    Gbl.CurrentCty.Cty.CtyCod,(unsigned) NotifyEvent);
            break;
	 case Sco_SCOPE_INS:
            DB_QuerySELECT (&mysql_res,"can not get the number"
        			       " of notifications by email",
        		    "SELECT SUM(sta_notif.NumEvents),SUM(sta_notif.NumMails)"
                            " FROM centres,degrees,sta_notif"
                            " WHERE centres.InsCod=%ld"
                            " AND centres.CtrCod=degrees.CtrCod"
                            " AND degrees.DegCod=sta_notif.DegCod"
                            " AND sta_notif.NotifyEvent=%u",
			    Gbl.CurrentIns.Ins.InsCod,(unsigned) NotifyEvent);
            break;
         case Sco_SCOPE_CTR:
            DB_QuerySELECT (&mysql_res,"can not get the number"
        			       " of notifications by email",
        		    "SELECT SUM(sta_notif.NumEvents),SUM(sta_notif.NumMails)"
                            " FROM degrees,sta_notif"
                            " WHERE degrees.CtrCod=%ld"
                            " AND degrees.DegCod=sta_notif.DegCod"
                            " AND sta_notif.NotifyEvent=%u",
			    Gbl.CurrentCtr.Ctr.CtrCod,(unsigned) NotifyEvent);
            break;
         case Sco_SCOPE_DEG:
            DB_QuerySELECT (&mysql_res,"can not get the number"
        			       " of notifications by email",
        		    "SELECT SUM(NumEvents),SUM(NumMails)"
                            " FROM sta_notif"
                            " WHERE DegCod=%ld"
                            " AND NotifyEvent=%u",
			    Gbl.CurrentDeg.Deg.DegCod,(unsigned) NotifyEvent);
            break;
         case Sco_SCOPE_CRS:
            DB_QuerySELECT (&mysql_res,"can not get the number"
        			       " of notifications by email",
        		    "SELECT SUM(NumEvents),SUM(NumMails)"
                            " FROM sta_notif"
                            " WHERE CrsCod=%ld"
                            " AND NotifyEvent=%u",
			    Gbl.CurrentCrs.Crs.CrsCod,(unsigned) NotifyEvent);
            break;
	 default:
	    Lay_WrongScopeExit ();
	    break;
        }

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
      NumMailsTotal  += NumMails [NotifyEvent];
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

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/************************** Show figures about notices ***********************/
/*****************************************************************************/

static void Fig_GetAndShowNoticesStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_notices;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
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

   /***** Get the number of notices active and obsolete *****/
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

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_NOTICES],NULL,
                      Hlp_ANALYTICS_Figures_notices,Box_NOT_CLOSABLE,2);

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

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/************************ Show figures about messages ************************/
/*****************************************************************************/

static void Fig_GetAndShowMsgsStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_messages;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
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

   /***** Get the number of unique messages sent from this location *****/
   NumMsgsSentNotDeleted      = Msg_GetNumMsgsSent     (Gbl.Scope.Current,Msg_STATUS_ALL     );
   NumMsgsSentDeleted         = Msg_GetNumMsgsSent     (Gbl.Scope.Current,Msg_STATUS_DELETED );

   NumMsgsReceivedNotDeleted  = Msg_GetNumMsgsReceived (Gbl.Scope.Current,Msg_STATUS_ALL     );
   NumMsgsReceivedAndDeleted  = Msg_GetNumMsgsReceived (Gbl.Scope.Current,Msg_STATUS_DELETED );
   NumMsgsReceivedAndNotified = Msg_GetNumMsgsReceived (Gbl.Scope.Current,Msg_STATUS_NOTIFIED);

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_MESSAGES],NULL,
                      Hlp_ANALYTICS_Figures_messages,Box_NOT_CLOSABLE,2);

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

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/***************************** Show stats of surveys *************************/
/*****************************************************************************/

static void Fig_GetAndShowSurveysStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_surveys;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
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

   /***** Get the number of surveys and the average number of questions per survey from this location *****/
   if ((NumSurveys = Svy_GetNumCrsSurveys (Gbl.Scope.Current,&NumNotif)))
     {
      if ((NumCoursesWithSurveys = Svy_GetNumCoursesWithCrsSurveys (Gbl.Scope.Current)) != 0)
         NumSurveysPerCourse = (float) NumSurveys / (float) NumCoursesWithSurveys;
      NumQstsPerSurvey = Svy_GetNumQstsPerCrsSurvey (Gbl.Scope.Current);
     }

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_SURVEYS],NULL,
                      Hlp_ANALYTICS_Figures_surveys,Box_NOT_CLOSABLE,2);

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

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/********** Get and show number of users who have chosen a privacy ***********/
/*****************************************************************************/

static void Fig_GetAndShowNumUsrsPerPrivacy (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_privacy;
   extern const char *Txt_Photo;
   extern const char *Txt_Public_profile;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_PRIVACY],NULL,
                      Hlp_ANALYTICS_Figures_privacy,Box_NOT_CLOSABLE,2);

   /***** Privacy for photo *****/
   Fig_GetAndShowNumUsrsPerPrivacyForAnObject (Txt_Photo,"PhotoVisibility");

   /***** Privacy for public profile *****/
   Fig_GetAndShowNumUsrsPerPrivacyForAnObject (Txt_Public_profile,"ProfileVisibility");

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/********** Get and show number of users who have chosen a privacy ***********/
/*****************************************************************************/

static void Fig_GetAndShowNumUsrsPerPrivacyForAnObject (const char *TxtObject,const char *FieldName)
  {
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];
   extern const char *Txt_PRIVACY_OPTIONS[Pri_NUM_OPTIONS_PRIVACY];
   Pri_Visibility_t Visibility;
   char *SubQuery;
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
      /* Get the number of users who have chosen this privacy option from database */
      if (asprintf (&SubQuery,"usr_data.%s='%s'",
	            FieldName,Pri_VisibilityDB[Visibility]) < 0)
	 Lay_NotEnoughMemoryExit ();
      NumUsrs[Visibility] = Fig_GetNumUsrsWhoChoseAnOption (SubQuery);
      free ((void *) SubQuery);

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
/** Get and show number of users who have chosen a preference about cookies **/
/*****************************************************************************/

static void Fig_GetAndShowNumUsrsPerCookies (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_cookies;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Cookies;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   unsigned i;
   char AcceptedInDB[2] =
     {
      'N',		// false
      'Y'		// true
     };
   char *AcceptedClass[2] =
     {
      "DAT_RED",	// false
      "DAT_GREEN"	// true
     };
   char *AcceptedSymbol[2] =
     {
      "&cross;",	// false
      "&check;"		// true
     };

   char *SubQuery;
   unsigned NumUsrs[Mnu_NUM_MENUS];
   unsigned NumUsrsTotal = 0;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_COOKIES],NULL,
                      Hlp_ANALYTICS_Figures_cookies,Box_NOT_CLOSABLE,2);

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
            Txt_Cookies,
            Txt_No_of_users,
            Txt_PERCENT_of_users);

   /***** For each option... *****/
   for (i = 0;
	i < 2;
	i++)
     {
      /* Get number of users who have chosen this menu from database */
      if (asprintf (&SubQuery,"usr_data.ThirdPartyCookies='%c'",
	            AcceptedInDB[i]) < 0)
	 Lay_NotEnoughMemoryExit ();
      NumUsrs[i] = Fig_GetNumUsrsWhoChoseAnOption (SubQuery);
      free ((void *) SubQuery);

      /* Update total number of users */
      NumUsrsTotal += NumUsrs[i];
     }

   /***** Write number of users who have chosen each option *****/
   for (i = 0;
	i < 2;
	i++)
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"%s CENTER_MIDDLE\">"
                         "%s"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT RIGHT_MIDDLE\">"
                         "%5.2f%%"
                         "</td>"
                         "</tr>",
	       AcceptedClass[i],
               AcceptedSymbol[i],
               NumUsrs[i],
               NumUsrsTotal ? (float) NumUsrs[i] * 100.0 /
        	              (float) NumUsrsTotal :
        	              0);

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/********* Get and show number of users who have chosen a language ***********/
/*****************************************************************************/

static void Fig_GetAndShowNumUsrsPerLanguage (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_language;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Language;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_STR_LANG_NAME[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   Lan_Language_t Lan;
   char *SubQuery;
   unsigned NumUsrs[1 + Lan_NUM_LANGUAGES];
   unsigned NumUsrsTotal = 0;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_LANGUAGES],NULL,
                      Hlp_ANALYTICS_Figures_language,Box_NOT_CLOSABLE,2);

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
   for (Lan = (Lan_Language_t) 1;
	Lan <= Lan_NUM_LANGUAGES;
	Lan++)
     {
      /* Get the number of users who have chosen this language from database */
      if (asprintf (&SubQuery,"usr_data.Language='%s'",
		    Lan_STR_LANG_ID[Lan]) < 0)
	 Lay_NotEnoughMemoryExit ();
      NumUsrs[Lan] = Fig_GetNumUsrsWhoChoseAnOption (SubQuery);
      free ((void *) SubQuery);

      /* Update total number of users */
      NumUsrsTotal += NumUsrs[Lan];
     }

   /***** Write number of users who have chosen each language *****/
   for (Lan = (Lan_Language_t) 1;
	Lan <= Lan_NUM_LANGUAGES;
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

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/***** Get and show number of users who have chosen a first day of week ******/
/*****************************************************************************/

static void Fig_GetAndShowNumUsrsPerFirstDayOfWeek (void)
  {
   extern const bool Cal_DayIsValidAsFirstDayOfWeek[7];
   extern const char *Hlp_ANALYTICS_Figures_calendar;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Calendar;
   extern const char *Txt_First_day_of_the_week;
   extern const char *Txt_DAYS_SMALL[7];
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   unsigned FirstDayOfWeek;
   char *SubQuery;
   unsigned NumUsrs[7];	// 7: seven days in a week
   unsigned NumUsrsTotal = 0;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_FIRST_DAY_OF_WEEK],NULL,
                      Hlp_ANALYTICS_Figures_calendar,Box_NOT_CLOSABLE,2);

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
	 /* Get number of users who have chosen this first day of week from database */
	 if (asprintf (&SubQuery,"usr_data.FirstDayOfWeek=%u",
		       (unsigned) FirstDayOfWeek) < 0)
	    Lay_NotEnoughMemoryExit ();
	 NumUsrs[FirstDayOfWeek] = Fig_GetNumUsrsWhoChoseAnOption (SubQuery);
	 free ((void *) SubQuery);

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
			    "<img src=\"%s/first-day-of-week-%u.png\""
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
		  Cfg_URL_ICON_PUBLIC,FirstDayOfWeek,
		  Txt_DAYS_SMALL[FirstDayOfWeek],
		  Txt_First_day_of_the_week,Txt_DAYS_SMALL[FirstDayOfWeek],
		  NumUsrs[FirstDayOfWeek],
		  NumUsrsTotal ? (float) NumUsrs[FirstDayOfWeek] * 100.0 /
				 (float) NumUsrsTotal :
				 0);

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/******** Get and show number of users who have chosen a date format *********/
/*****************************************************************************/

static void Fig_GetAndShowNumUsrsPerDateFormat (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_dates;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Format;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   unsigned Format;
   char *SubQuery;
   unsigned NumUsrs[Dat_NUM_OPTIONS_FORMAT];
   unsigned NumUsrsTotal = 0;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_DATE_FORMAT],NULL,
                      Hlp_ANALYTICS_Figures_dates,Box_NOT_CLOSABLE,2);

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
      /* Get number of users who have chosen this date format from database */
      if (asprintf (&SubQuery,"usr_data.DateFormat=%u",
	            (unsigned) Format) < 0)
	 Lay_NotEnoughMemoryExit ();
      NumUsrs[Format] = Fig_GetNumUsrsWhoChoseAnOption (SubQuery);
      free ((void *) SubQuery);

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

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/********* Get and show number of users who have chosen an icon set **********/
/*****************************************************************************/

static void Fig_GetAndShowNumUsrsPerIconSet (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_icons;
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   extern const char *Ico_IconSetNames[Ico_NUM_ICON_SETS];
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Icons;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   Ico_IconSet_t IconSet;
   char *SubQuery;
   unsigned NumUsrs[Ico_NUM_ICON_SETS];
   unsigned NumUsrsTotal = 0;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_ICON_SETS],NULL,
                      Hlp_ANALYTICS_Figures_icons,Box_NOT_CLOSABLE,2);

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
      /* Get the number of users who have chosen this icon set from database */
      if (asprintf (&SubQuery,"usr_data.IconSet='%s'",
	            Ico_IconSetId[IconSet]) < 0)
	 Lay_NotEnoughMemoryExit ();
      NumUsrs[IconSet] = Fig_GetNumUsrsWhoChoseAnOption (SubQuery);
      free ((void *) SubQuery);

      /* Update total number of users */
      NumUsrsTotal += NumUsrs[IconSet];
     }

   /***** Write number of users who have chosen each icon set *****/
   for (IconSet = (Ico_IconSet_t) 0;
	IconSet < Ico_NUM_ICON_SETS;
	IconSet++)
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"LEFT_MIDDLE\">"
                         "<img src=\"%s/%s/cog.svg\""
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
               Cfg_URL_ICON_SETS_PUBLIC,
               Ico_IconSetId[IconSet],
               Ico_IconSetNames[IconSet],
               Ico_IconSetNames[IconSet],
               NumUsrs[IconSet],
               NumUsrsTotal ? (float) NumUsrs[IconSet] * 100.0 /
        	              (float) NumUsrsTotal :
        	              0);

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/*********** Get and show number of users who have chosen a menu *************/
/*****************************************************************************/

static void Fig_GetAndShowNumUsrsPerMenu (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_menu;
   extern const char *Mnu_MenuIcons[Mnu_NUM_MENUS];
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Menu;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Txt_MENU_NAMES[Mnu_NUM_MENUS];
   Mnu_Menu_t Menu;
   char *SubQuery;
   unsigned NumUsrs[Mnu_NUM_MENUS];
   unsigned NumUsrsTotal = 0;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_MENUS],NULL,
                      Hlp_ANALYTICS_Figures_menu,Box_NOT_CLOSABLE,2);

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
      /* Get number of users who have chosen this menu from database */
      if (asprintf (&SubQuery,"usr_data.Menu=%u",
	            (unsigned) Menu) < 0)
	 Lay_NotEnoughMemoryExit ();
      NumUsrs[Menu] = Fig_GetNumUsrsWhoChoseAnOption (SubQuery);
      free ((void *) SubQuery);

      /* Update total number of users */
      NumUsrsTotal += NumUsrs[Menu];
     }

   /***** Write number of users who have chosen each menu *****/
   for (Menu = (Mnu_Menu_t) 0;
	Menu < Mnu_NUM_MENUS;
	Menu++)
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"CENTER_MIDDLE\">"
                         "<img src=\"%s/%s\""
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
               Cfg_URL_ICON_PUBLIC,Mnu_MenuIcons[Menu],
               Txt_MENU_NAMES[Menu],
               Txt_MENU_NAMES[Menu],
               NumUsrs[Menu],
               NumUsrsTotal ? (float) NumUsrs[Menu] * 100.0 /
        	              (float) NumUsrsTotal :
        	              0);

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/********** Get and show number of users who have chosen a theme *************/
/*****************************************************************************/

static void Fig_GetAndShowNumUsrsPerTheme (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_theme;
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *The_ThemeNames[The_NUM_THEMES];
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Theme_SKIN;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   The_Theme_t Theme;
   char *SubQuery;
   unsigned NumUsrs[The_NUM_THEMES];
   unsigned NumUsrsTotal = 0;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_THEMES],NULL,
                      Hlp_ANALYTICS_Figures_theme,Box_NOT_CLOSABLE,2);

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
      /* Get number of users who have chosen this theme from database */
      if (asprintf (&SubQuery,"usr_data.Theme='%s'",
		    The_ThemeId[Theme]) < 0)
	 Lay_NotEnoughMemoryExit ();
      NumUsrs[Theme] = Fig_GetNumUsrsWhoChoseAnOption (SubQuery);
      free ((void *) SubQuery);

      /* Update total number of users */
      NumUsrsTotal += NumUsrs[Theme];
     }

   /***** Write number of users who have chosen each theme *****/
   for (Theme = (The_Theme_t) 0;
	Theme < The_NUM_THEMES;
	Theme++)
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"CENTER_MIDDLE\">"
                         "<img src=\"%s/%s/theme_32x20.gif\""
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
               Cfg_URL_ICON_THEMES_PUBLIC,The_ThemeId[Theme],
               The_ThemeNames[Theme],
               The_ThemeNames[Theme],
               NumUsrs[Theme],
               NumUsrsTotal ? (float) NumUsrs[Theme] * 100.0 /
        	              (float) NumUsrsTotal :
        	              0);

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/***** Get and show number of users who have chosen a layout of columns ******/
/*****************************************************************************/

static void Fig_GetAndShowNumUsrsPerSideColumns (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_columns;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Columns;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   unsigned SideCols;
   char *SubQuery;
   unsigned NumUsrs[4];
   unsigned NumUsrsTotal = 0;
   extern const char *Txt_LAYOUT_SIDE_COLUMNS[4];

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_FIGURE_TYPES[Fig_SIDE_COLUMNS],NULL,
                      Hlp_ANALYTICS_Figures_columns,Box_NOT_CLOSABLE,2);

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
      /* Get the number of users who have chosen this layout of columns from database */
      if (asprintf (&SubQuery,"usr_data.SideCols=%u",
	            SideCols) < 0)
	 Lay_NotEnoughMemoryExit ();
      NumUsrs[SideCols] = Fig_GetNumUsrsWhoChoseAnOption (SubQuery);
      free ((void *) SubQuery);

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
               Cfg_URL_ICON_PUBLIC,SideCols >> 1,SideCols & 1,
               Txt_LAYOUT_SIDE_COLUMNS[SideCols],
               Txt_LAYOUT_SIDE_COLUMNS[SideCols],
               NumUsrs[SideCols],
               NumUsrsTotal ? (float) NumUsrs[SideCols] * 100.0 /
        	              (float) NumUsrsTotal :
        	              0);

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/************** Get number of users who have chosen an option ****************/
/*****************************************************************************/

unsigned Fig_GetNumUsrsWhoChoseAnOption (const char *SubQuery)
  {
   unsigned NumUsrs;

   /***** Get the number of users who have chosen this privacy option from database *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 NumUsrs =
	 (unsigned) DB_QueryCOUNT ("can not get the number of users"
				   " who have chosen an option",
				   "SELECT COUNT(*)"
				   " FROM usr_data WHERE %s",
				   SubQuery);
	 break;
      case Sco_SCOPE_CTY:
	 NumUsrs =
	 (unsigned) DB_QueryCOUNT ("can not get the number of users"
				   " who have chosen an option",
				   "SELECT COUNT(DISTINCT usr_data.UsrCod)"
				   " FROM institutions,centres,degrees,courses,crs_usr,usr_data"
				   " WHERE institutions.CtyCod=%ld"
				   " AND institutions.InsCod=centres.InsCod"
				   " AND centres.CtrCod=degrees.CtrCod"
				   " AND degrees.DegCod=courses.DegCod"
				   " AND courses.CrsCod=crs_usr.CrsCod"
				   " AND crs_usr.UsrCod=usr_data.UsrCod"
				   " AND %s",
				   Gbl.CurrentCty.Cty.CtyCod,SubQuery);
	 break;
      case Sco_SCOPE_INS:
	 NumUsrs =
	 (unsigned) DB_QueryCOUNT ("can not get the number of users"
				   " who have chosen an option",
				   "SELECT COUNT(DISTINCT usr_data.UsrCod)"
				   " FROM centres,degrees,courses,crs_usr,usr_data"
				   " WHERE centres.InsCod=%ld"
				   " AND centres.CtrCod=degrees.CtrCod"
				   " AND degrees.DegCod=courses.DegCod"
				   " AND courses.CrsCod=crs_usr.CrsCod"
				   " AND crs_usr.UsrCod=usr_data.UsrCod"
				   " AND %s",
				   Gbl.CurrentIns.Ins.InsCod,SubQuery);
	 break;
      case Sco_SCOPE_CTR:
	 NumUsrs =
	 (unsigned) DB_QueryCOUNT ("can not get the number of users"
				   " who have chosen an option",
				   "SELECT COUNT(DISTINCT usr_data.UsrCod)"
				   " FROM degrees,courses,crs_usr,usr_data"
				   " WHERE degrees.CtrCod=%ld"
				   " AND degrees.DegCod=courses.DegCod"
				   " AND courses.CrsCod=crs_usr.CrsCod"
				   " AND crs_usr.UsrCod=usr_data.UsrCod"
				   " AND %s",
				   Gbl.CurrentCtr.Ctr.CtrCod,SubQuery);
	 break;
      case Sco_SCOPE_DEG:
	 NumUsrs =
	 (unsigned) DB_QueryCOUNT ("can not get the number of users"
				   " who have chosen an option",
				   "SELECT COUNT(DISTINCT usr_data.UsrCod)"
				   " FROM courses,crs_usr,usr_data"
				   " WHERE courses.DegCod=%ld"
				   " AND courses.CrsCod=crs_usr.CrsCod"
				   " AND crs_usr.UsrCod=usr_data.UsrCod"
				   " AND %s",
				   Gbl.CurrentDeg.Deg.DegCod,SubQuery);
	 break;
      case Sco_SCOPE_CRS:
	 NumUsrs =
	 (unsigned) DB_QueryCOUNT ("can not get the number of users"
				   " who have chosen an option",
				   "SELECT COUNT(DISTINCT usr_data.UsrCod)"
				   " FROM crs_usr,usr_data"
				   " WHERE crs_usr.CrsCod=%ld"
				   " AND crs_usr.UsrCod=usr_data.UsrCod"
				   " AND %s",
				   Gbl.CurrentCrs.Crs.CrsCod,SubQuery);
	 break;
      default:
	 Lay_WrongScopeExit ();
	 NumUsrs = 0;	// Not reached. Initialized to avoid warning.
	 break;
     }

   return NumUsrs;
  }
