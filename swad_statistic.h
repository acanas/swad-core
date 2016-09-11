// swad_statistic.h: statistics

#ifndef _SWAD_STA
#define _SWAD_STA
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#include "swad_user.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Sta_NUM_ROLES_STAT	11
typedef enum
  {
   Sta_IDENTIFIED_USRS =  0,
   Sta_ALL_USRS        =  1,
   Sta_INS_ADMINS      =  2,
   Sta_CTR_ADMINS      =  3,
   Sta_DEG_ADMINS      =  4,
   Sta_TEACHERS        =  5,
   Sta_STUDENTS        =  6,
   Sta_VISITORS        =  7,
   Sta_GUESTS         =  8,
   Sta_UNKNOWN_USRS    =  9,
   Sta_ME              = 10,
  } Sta_Role_t;

#define Sta_NUM_COUNT_TYPES	5
typedef enum
  {
   Sta_TOTAL_CLICKS     = 0,
   Sta_DISTINCT_USRS    = 1,
   Sta_CLICKS_PER_USR   = 2,
   Sta_GENERATION_TIME  = 3,
   Sta_SEND_TIME	= 4,
  } Sta_CountType_t;

#define Sta_NUM_CLICKS_DETAILED_OR_GROUPED	2
typedef enum
  {
   Sta_CLICKS_DETAILED	= 0,
   Sta_CLICKS_GROUPED	= 1,
  } Sta_ClicksDetailedOrGrouped_t;

#define Sta_NUM_CLICKS_GROUPED_BY	24
typedef enum
  {
   Sta_CLICKS_CRS_DETAILED_LIST			=  0,

   Sta_CLICKS_CRS_PER_USR			=  1,
   Sta_CLICKS_CRS_PER_DAYS			=  2,
   Sta_CLICKS_CRS_PER_DAYS_AND_HOUR		=  3,
   Sta_CLICKS_CRS_PER_WEEKS			=  4,
   Sta_CLICKS_CRS_PER_MONTHS			=  5,
   Sta_CLICKS_CRS_PER_HOUR			=  6,
   Sta_CLICKS_CRS_PER_MINUTE			=  7,
   Sta_CLICKS_CRS_PER_ACTION			=  8,

   Sta_CLICKS_GBL_PER_DAYS			=  9,
   Sta_CLICKS_GBL_PER_DAYS_AND_HOUR		= 10,
   Sta_CLICKS_GBL_PER_WEEKS			= 11,
   Sta_CLICKS_GBL_PER_MONTHS			= 12,
   Sta_CLICKS_GBL_PER_HOUR			= 13,
   Sta_CLICKS_GBL_PER_MINUTE			= 14,
   Sta_CLICKS_GBL_PER_ACTION			= 15,
   Sta_CLICKS_GBL_PER_PLUGIN			= 16,
   Sta_CLICKS_GBL_PER_WEB_SERVICE_FUNCTION	= 17,
   Sta_CLICKS_GBL_PER_BANNER			= 18,
   Sta_CLICKS_GBL_PER_COUNTRY			= 19,
   Sta_CLICKS_GBL_PER_INSTITUTION		= 20,
   Sta_CLICKS_GBL_PER_CENTRE			= 21,
   Sta_CLICKS_GBL_PER_DEGREE			= 22,
   Sta_CLICKS_GBL_PER_COURSE			= 23,
  } Sta_ClicksGroupedBy_t;

#define Sta_NUM_FIGURES 23
typedef enum
  {
   Sta_USERS,			// Number of users
   Sta_USERS_RANKING,		// Users' ranking
   Sta_HIERARCHY,		// Number of countries, institutions, centres, degrees and courses
   Sta_INSTITUTIONS,		// Number of users in each institution
   Sta_SOCIAL_NETWORKS,		// Number of users in social networks
   Sta_FOLDERS_AND_FILES,	// Number of folders and files
   Sta_OER,			// Number of OERs (Open Educational Resources)
   Sta_ASSIGNMENTS,		// Number of assignments
   Sta_TESTS,			// Number of test questions
   Sta_SOCIAL_ACTIVITY,		// Number of social notes
   Sta_FOLLOW,			// Number of following and followers
   Sta_FORUMS,			// Number of forums, threads and posts
   Sta_NOTIFY_EVENTS,		// Number of users per notify event
   Sta_NOTICES,			// Number of notices
   Sta_MESSAGES,		// Number of users' (sent and received) messages
   Sta_SURVEYS,			// Number of surveys
   Sta_PRIVACY,			// Number of users per privacity
   Sta_LANGUAGES,		// Number of users per language
   Sta_FIRST_DAY_OF_WEEK,	// Number of users per first day of week
   Sta_THEMES,			// Number of users per theme
   Sta_ICON_SETS,		// Number of users per icon set
   Sta_MENUS,			// Number of users per menu
   Sta_SIDE_COLUMNS,		// Number of users per layout of columns
  } Sta_FigureType_t;

#define Sta_NUM_COLOR_TYPES 3
typedef enum
  {
   Sta_COLOR,
   Sta_BLACK_TO_WHITE,
   Sta_WHITE_TO_BLACK,
  } Sta_ColorType_t;

#define Sta_NUM_STAT_CRS_FILE_ZONES 11

struct Sta_Hits
  {
   float Num;
   float Max;
   float Total;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Sta_GetRemoteAddr (void);
void Sta_LogAccess (const char *Comments);
void Sta_RemoveOldEntriesRecentLog (void);
void Sta_AskShowCrsHits (void);
void Sta_AskShowGblHits (void);
void Sta_SetIniEndDates (void);
void Sta_SeeGblAccesses (void);
void Sta_SeeCrsAccesses (void);

void Sta_ComputeMaxAndTotalHits (struct Sta_Hits *Hits,
                                 unsigned long NumRows,
                                 MYSQL_RES *mysql_res,unsigned Field,
                                 unsigned Divisor);

void Sta_ReqUseOfPlatform (void);
void Pho_PutHiddenParamFigureType (void);
void Sta_ShowUseOfPlatform (void);
unsigned Sta_GetTotalNumberOfUsersInCourses (Sco_Scope_t Scope,Rol_Role_t Role);

void Sta_DrawBarNumHits (char Color,float HitsNum,float HitsMax,float HitsTotal,unsigned MaxBarWidth);

void Sta_WriteParamsDatesSeeAccesses (void);

void Sta_ComputeTimeToGeneratePage (void);
void Sta_ComputeTimeToSendPage (void);
void Sta_WriteTimeToGenerateAndSendPage (void);
void Sta_WriteTime (char *Str,long TimeInMicroseconds);

#endif
