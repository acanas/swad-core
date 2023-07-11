// swad_figure.h: figures (global stats)

#ifndef _SWAD_FIG
#define _SWAD_FIG
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

#include "swad_hierarchy_level.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Fig_NUM_FIGURES 33
typedef enum
  {
   Fig_USERS,			// Number of users
   Fig_USERS_RANKING,		// Users' ranking
   Fig_HIERARCHY,		// Number of countries, institutions, centers, degrees and courses
   Fig_INSTITS,			// Number of users in each institution
   Fig_DEGREE_TYPES,		// Number of degrees in each type of degree
   Fig_FOLDERS_AND_FILES,	// Number of folders and files
   Fig_OER,			// Number of OERs (Open Educational Resources)
   Fig_COURSE_PROGRAMS,		// Number of program items
   Fig_ASSIGNMENTS,		// Number of assignments
   Fig_PROJECTS,		// Number of projects
   Fig_TESTS,			// Number of test questions
   Fig_EXAMS,			// Number of exams
   Fig_GAMES,			// Number of games
   Fig_RUBRICS,			// Number of rubrics
   Fig_TIMELINE,		// Number of timeline notes
   Fig_FOLLOW,			// Number of following and followers
   Fig_FORUMS,			// Number of forums, threads and posts
   Fig_NOTIFY_EVENTS,		// Number of users per notify event
   Fig_NOTICES,			// Number of notices
   Fig_MESSAGES,		// Number of users' (sent and received) messages
   Fig_SURVEYS,			// Number of surveys
   Fig_AGENDAS,			// Number of agendas
   Fig_SOCIAL_NETWORKS,		// Number of users in social networks
   Fig_LANGUAGES,		// Number of users per language
   Fig_FIRST_DAY_OF_WEEK,	// Number of users per first day of week
   Fig_DATE_FORMAT,		// Number of users per date format
   Fig_ICON_SETS,		// Number of users per icon set
   Fig_MENUS,			// Number of users per menu
   Fig_THEMES,			// Number of users per theme
   Fig_SIDE_COLUMNS,		// Number of users per layout of columns
   Fig_PHOTO_SHAPES,		// Number of users per user photo shape
   Fig_PRIVACY,			// Number of users per privacity
   Fig_COOKIES,			// Number of users per acceptation of cookies
  } Fig_FigureType_t;
#define Fig_FIGURE_TYPE_DEF Fig_USERS

#define Fig_NUM_STAT_CRS_FILE_ZONES 12

struct Fig_Figures
  {
   HieLvl_Level_t Level;
   Fig_FigureType_t FigureType;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Fig_ReqShowFigures (void);
void Fig_PutIconToShowFigure (Fig_FigureType_t FigureType);
void Fig_PutParsFigures (void *Figures);
void Fig_ShowFigures (void);

#endif
