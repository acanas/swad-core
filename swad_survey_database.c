// swad_survey_database.c: surveys, operations with database

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

// #define _GNU_SOURCE 		// For asprintf
// #include <linux/limits.h>	// For PATH_MAX
// #include <stddef.h>		// For NULL
// #include <stdio.h>		// For asprintf
// #include <stdlib.h>		// For calloc
// #include <string.h>		// For string functions

// #include "swad_attendance.h"
// #include "swad_box.h"
#include "swad_database.h"
// #include "swad_error.h"
// #include "swad_figure.h"
// #include "swad_form.h"
#include "swad_global.h"
// #include "swad_group.h"
// #include "swad_group_database.h"
// #include "swad_HTML.h"
// #include "swad_notification.h"
// #include "swad_notification_database.h"
// #include "swad_pagination.h"
// #include "swad_parameter.h"
// #include "swad_role.h"
// #include "swad_setting.h"
// #include "swad_survey.h"
#include "swad_survey_database.h"

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

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/************ Get all user codes belonging to a survey, except me ************/
/*****************************************************************************/

unsigned Svy_DB_GetUsrsFromSurveyExceptMe (MYSQL_RES **mysql_res,long SvyCod)
  {
   // 1. If the survey is available for the whole course ==> get users enroled in the course whose role is available in survey, except me
   // 2. If the survey is available only for some groups ==> get users who belong to any of the groups and whose role is available in survey, except me
   // Cases 1 and 2 are mutually exclusive, so the union returns the case 1 or 2
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users to be notified",
		   "(SELECT crs_users.UsrCod"
		     " FROM svy_surveys,"
			   "crs_users"
		    " WHERE svy_surveys.SvyCod=%ld"
		      " AND svy_surveys.SvyCod NOT IN"
			  " (SELECT SvyCod"
			     " FROM svy_groups"
			    " WHERE SvyCod=%ld)"
		      " AND svy_surveys.Scope='%s'"
		      " AND svy_surveys.Cod=crs_users.CrsCod"
		      " AND crs_users.UsrCod<>%ld"
		      " AND (svy_surveys.Roles&(1<<crs_users.Role))<>0)"
		   " UNION "
		   "(SELECT DISTINCT grp_users.UsrCod"
		     " FROM svy_groups,"
			   "grp_users,"
			   "svy_surveys,"
			   "crs_users"
		    " WHERE svy_groups.SvyCod=%ld"
		      " AND svy_groups.GrpCod=grp_users.GrpCod"
		      " AND grp_users.UsrCod=crs_users.UsrCod"
		      " AND grp_users.UsrCod<>%ld"
		      " AND svy_groups.SvyCod=svy_surveys.SvyCod"
		      " AND svy_surveys.Scope='%s'"
		      " AND svy_surveys.Cod=crs_users.CrsCod"
		      " AND (svy_surveys.Roles&(1<<crs_users.Role))<>0)",
		   SvyCod,
		   SvyCod,
		   Sco_GetDBStrFromScope (HieLvl_CRS),
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   SvyCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Sco_GetDBStrFromScope (HieLvl_CRS));
  }
