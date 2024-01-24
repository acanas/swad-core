// swad_statistic.h: statistics

#ifndef _SWAD_STA
#define _SWAD_STA
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#include "swad_indicator.h"
#include "swad_photo.h"
#include "swad_user.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Sta_NUM_ROLES_STAT	12
typedef enum
  {
   Sta_ROLE_IDENTIFIED_USRS      =  0,
   Sta_ROLE_ALL_USRS             =  1,
   Sta_ROLE_INS_ADMINS           =  2,
   Sta_ROLE_CTR_ADMINS           =  3,
   Sta_ROLE_DEG_ADMINS           =  4,
   Sta_ROLE_TEACHERS             =  5,
   Sta_ROLE_NON_EDITING_TEACHERS =  6,
   Sta_ROLE_STUDENTS             =  7,
   Sta_ROLE_USERS                =  8,
   Sta_ROLE_GUESTS               =  9,
   Sta_ROLE_UNKNOWN_USRS         = 10,
   Sta_ROLE_ME                   = 11,
  } Sta_Role_t;
#define Sta_ROLE_DEFAULT Sta_ROLE_IDENTIFIED_USRS

#define Sta_NUM_ACTION_DEFAULT ActAll

#define Sta_NUM_COUNT_TYPES	5
typedef enum
  {
   Sta_TOTAL_CLICKS     = 0,
   Sta_DISTINCT_USRS    = 1,
   Sta_CLICKS_PER_USR   = 2,
   Sta_GENERATION_TIME  = 3,
   Sta_SEND_TIME	= 4,
  } Sta_CountType_t;
#define Sta_COUNT_TYPE_DEFAULT Sta_TOTAL_CLICKS

#define Sta_NUM_CLICKS_DETAILED_OR_GROUPED	2
typedef enum
  {
   Sta_CLICKS_DETAILED	= 0,
   Sta_CLICKS_GROUPED	= 1,
  } Sta_ClicksDetailedOrGrouped_t;
#define Sta_CLICKS_DETAILED_OR_GROUPED_DEFAULT Sta_CLICKS_GROUPED

#define Sta_NUM_CLICKS_GROUPED_BY	26
typedef enum
  {
   Sta_CLICKS_CRS_DETAILED_LIST			=  0,

   Sta_CLICKS_CRS_PER_USR			=  1,
   Sta_CLICKS_CRS_PER_DAY			=  2,
   Sta_CLICKS_CRS_PER_DAY_AND_HOUR		=  3,
   Sta_CLICKS_CRS_PER_WEEK			=  4,
   Sta_CLICKS_CRS_PER_MONTH			=  5,
   Sta_CLICKS_CRS_PER_YEAR			=  6,
   Sta_CLICKS_CRS_PER_HOUR			=  7,
   Sta_CLICKS_CRS_PER_MINUTE			=  8,
   Sta_CLICKS_CRS_PER_ACTION			=  9,

   Sta_CLICKS_GBL_PER_DAY			= 10,
   Sta_CLICKS_GBL_PER_DAY_AND_HOUR		= 11,
   Sta_CLICKS_GBL_PER_WEEK			= 12,
   Sta_CLICKS_GBL_PER_MONTH			= 13,
   Sta_CLICKS_GBL_PER_YEAR			= 14,
   Sta_CLICKS_GBL_PER_HOUR			= 15,
   Sta_CLICKS_GBL_PER_MINUTE			= 16,
   Sta_CLICKS_GBL_PER_ACTION			= 17,
   Sta_CLICKS_GBL_PER_PLUGIN			= 18,
   Sta_CLICKS_GBL_PER_API_FUNCTION		= 19,
   Sta_CLICKS_GBL_PER_BANNER			= 20,
   Sta_CLICKS_GBL_PER_COUNTRY			= 21,
   Sta_CLICKS_GBL_PER_INSTITUTION		= 22,
   Sta_CLICKS_GBL_PER_CENTER			= 23,
   Sta_CLICKS_GBL_PER_DEGREE			= 24,
   Sta_CLICKS_GBL_PER_COURSE			= 25,
  } Sta_ClicksGroupedBy_t;
#define Sta_CLICKS_GROUPED_BY_DEFAULT Sta_CLICKS_CRS_PER_USR

#define Sta_MIN_ROWS_PER_PAGE 10
#define Sta_MAX_ROWS_PER_PAGE (Sta_MIN_ROWS_PER_PAGE * 10000)
#define Sta_DEF_ROWS_PER_PAGE (Sta_MIN_ROWS_PER_PAGE * 5)

#define Sta_NUM_COLOR_TYPES 3
typedef enum
  {
   Sta_COLOR,
   Sta_BLACK_TO_WHITE,
   Sta_WHITE_TO_BLACK,
  } Sta_ColorType_t;
#define Sta_COLOR_TYPE_DEF Sta_COLOR

struct Sta_Hits
  {
   double Num;
   double Max;
   double Total;
  };

typedef enum
  {
   Sta_SHOW_GLOBAL_ACCESSES,
   Sta_SHOW_COURSE_ACCESSES,
  } Sta_GlobalOrCourseAccesses_t;

struct Sta_Stats
  {
   Sta_GlobalOrCourseAccesses_t GlobalOrCourse;
   Sta_ClicksGroupedBy_t ClicksGroupedBy;
   Sta_Role_t Role;
   Sta_CountType_t CountType;
   Act_Action_t NumAction;
   unsigned long FirstRow;
   unsigned long LastRow;
   unsigned RowsPerPage;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Sta_ResetStats (struct Sta_Stats *Stats);

void Sta_ReqCrsHits (void);
void Sta_AskShowGblHits (void);
void Sta_PutLinkToCourseHits (void);
void Sta_PutLinkToGlobalHits (void);

void Sta_SeeGblAccesses (void);
void Sta_SeeCrsAccesses (void);

void Sta_ComputeMaxAndTotalHits (struct Sta_Hits *Hits,
                                 unsigned NumHits,
                                 MYSQL_RES *mysql_res,unsigned Field,
                                 unsigned Divisor);

#endif
