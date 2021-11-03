// swad_user_database.c: users, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

// #define _GNU_SOURCE 		// For asprintf
// #include <ctype.h>		// For isalnum, isdigit, etc.
// #include <limits.h>		// For maximum values
// #include <linux/limits.h>	// For PATH_MAX
// #include <stddef.h>		// For NULL
// #include <stdio.h>		// For asprintf
// #include <stdlib.h>		// For exit, system, malloc, free, rand, etc.
// #include <string.h>		// For string functions
// #include <sys/wait.h>		// For the macro WEXITSTATUS
// #include <unistd.h>		// For access, lstat, getpid, chdir, symlink, unlink

// #include "swad_account.h"
// #include "swad_agenda.h"
// #include "swad_announcement.h"
// #include "swad_box.h"
// #include "swad_calendar.h"
// #include "swad_config.h"
// #include "swad_connected_database.h"
// #include "swad_course.h"
#include "swad_database.h"
// #include "swad_department.h"
// #include "swad_duplicate.h"
// #include "swad_enrolment.h"
#include "swad_error.h"
// #include "swad_figure.h"
// #include "swad_figure_cache.h"
// #include "swad_follow.h"
// #include "swad_form.h"
#include "swad_global.h"
// #include "swad_group.h"
// #include "swad_help.h"
// #include "swad_hierarchy.h"
// #include "swad_hierarchy_level.h"
// #include "swad_HTML.h"
// #include "swad_ID.h"
// #include "swad_language.h"
// #include "swad_mail_database.h"
// #include "swad_message.h"
// #include "swad_MFU.h"
// #include "swad_nickname.h"
// #include "swad_nickname_database.h"
// #include "swad_notification.h"
// #include "swad_parameter.h"
// #include "swad_password.h"
// #include "swad_photo.h"
// #include "swad_privacy.h"
// #include "swad_QR.h"
// #include "swad_record.h"
// #include "swad_record_database.h"
// #include "swad_role.h"
// #include "swad_session_database.h"
// #include "swad_setting.h"
// #include "swad_tab.h"
// #include "swad_user.h"
#include "swad_user_database.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Private types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Private variables ****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Update my office ****************************/
/*****************************************************************************/

void Usr_DB_UpdateMyOffice (void)
  {
   DB_QueryUPDATE ("can not update office",
		   "UPDATE usr_data"
		     " SET Office='%s'"
		   " WHERE UsrCod=%ld",
		   Gbl.Usrs.Me.UsrDat.Tch.Office,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/***************************** Update my office phone ************************/
/*****************************************************************************/

void Usr_DB_UpdateMyOfficePhone (void)
  {
   DB_QueryUPDATE ("can not update office phone",
		   "UPDATE usr_data"
		     " SET OfficePhone='%s'"
		   " WHERE UsrCod=%ld",
	           Gbl.Usrs.Me.UsrDat.Tch.OfficePhone,
	           Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************** Update my last type of search ************************/
/*****************************************************************************/

void Usr_DB_UpdateMyLastWhatToSearch (void)
  {
   // WhatToSearch is stored in usr_last for next time I log in
   // In other existing sessions distinct to this, WhatToSearch will remain unchanged
   DB_QueryUPDATE ("can not update type of search in user's last data",
		   "UPDATE usr_last"
		     " SET WhatToSearch=%u"
		   " WHERE UsrCod=%ld",
		   (unsigned) Gbl.Search.WhatToSearch,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************** Check if a user exists with a given user's code **************/
/*****************************************************************************/

bool Usr_DB_ChkIfUsrCodExists (long UsrCod)
  {
   /***** Trivial check: user's code should be > 0 *****/
   if (UsrCod <= 0)	// Wrong user's code
      return false;

   /***** Get if a user exists in database *****/
   return (DB_QueryCOUNT ("can not check if a user exists",
			  "SELECT COUNT(*)"
			   " FROM usr_data"
			  " WHERE UsrCod=%ld",
			  UsrCod) != 0);
  }

/*****************************************************************************/
/******** Check if a user exists with a given encrypted user's code **********/
/*****************************************************************************/

bool Usr_DB_ChkIfEncryptedUsrCodExists (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64])
  {
   /***** Get if an encrypted user's code already existed in database *****/
   return (DB_QueryCOUNT ("can not check if an encrypted user's code"
			  " already existed",
			  "SELECT COUNT(*)"
			   " FROM usr_data"
			  " WHERE EncryptedUsrCod='%s'",
			  EncryptedUsrCod) != 0);
  }

/*****************************************************************************/
/****** Check if a string is found in first name or surnames of anybody ******/
/*****************************************************************************/

bool Usr_DB_FindStrInUsrsNames (const char *Str)
  {
   return (DB_QueryCOUNT ("can not check if a string matches"
			  " a first name or a surname",
			  "SELECT COUNT(*)"
			   " FROM usr_data"
			  " WHERE FirstName='%s'"
			     " OR Surname1='%s'"
			     " OR Surname2='%s'",
			  Str,
			  Str,
			  Str) != 0);
  }

/*****************************************************************************/
/************** Get number of users who have chosen an option ****************/
/*****************************************************************************/

unsigned Usr_DB_GetNumUsrsWhoChoseAnOption (const char *SubQuery)
  {
   switch (Gbl.Scope.Current)
     {
      case HieLvl_SYS:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the number of users who have chosen an option",
		        "SELECT COUNT(*)"
		         " FROM usr_data"
		        " WHERE %s",
		        SubQuery);
      case HieLvl_CTY:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the number of users who have chosen an option",
		        "SELECT COUNT(DISTINCT usr_data.UsrCod)"
		         " FROM ins_instits,"
			       "ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "crs_users,"
			       "usr_data"
		        " WHERE ins_instits.CtyCod=%ld"
		          " AND ins_instits.InsCod=ctr_centers.InsCod"
		          " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		          " AND deg_degrees.DegCod=crs_courses.DegCod"
		          " AND crs_courses.CrsCod=crs_users.CrsCod"
		          " AND crs_users.UsrCod=usr_data.UsrCod"
		          " AND %s",
		        Gbl.Hierarchy.Cty.CtyCod,SubQuery);
      case HieLvl_INS:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the number of users who have chosen an option",
		        "SELECT COUNT(DISTINCT usr_data.UsrCod)"
		         " FROM ctr_centers,"
		               "deg_degrees,"
		               "crs_courses,"
		               "crs_users,"
		               "usr_data"
		        " WHERE ctr_centers.InsCod=%ld"
		          " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		          " AND deg_degrees.DegCod=crs_courses.DegCod"
		          " AND crs_courses.CrsCod=crs_users.CrsCod"
		          " AND crs_users.UsrCod=usr_data.UsrCod"
		          " AND %s",
		        Gbl.Hierarchy.Ins.InsCod,SubQuery);
      case HieLvl_CTR:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the number of users who have chosen an option",
		        "SELECT COUNT(DISTINCT usr_data.UsrCod)"
		         " FROM deg_degrees,"
		               "crs_courses,"
		               "crs_users,"
		               "usr_data"
		        " WHERE deg_degrees.CtrCod=%ld"
		          " AND deg_degrees.DegCod=crs_courses.DegCod"
		          " AND crs_courses.CrsCod=crs_users.CrsCod"
		          " AND crs_users.UsrCod=usr_data.UsrCod"
		          " AND %s",
		        Gbl.Hierarchy.Ctr.CtrCod,SubQuery);
      case HieLvl_DEG:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the number of users who have chosen an option",
		        "SELECT COUNT(DISTINCT usr_data.UsrCod)"
		         " FROM crs_courses,"
		               "crs_users,"
		               "usr_data"
		        " WHERE crs_courses.DegCod=%ld"
		          " AND crs_courses.CrsCod=crs_users.CrsCod"
		          " AND crs_users.UsrCod=usr_data.UsrCod"
		          " AND %s",
		        Gbl.Hierarchy.Deg.DegCod,SubQuery);
      case HieLvl_CRS:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the number of users who have chosen an option",
		        "SELECT COUNT(DISTINCT usr_data.UsrCod)"
		         " FROM crs_users,"
		               "usr_data"
		        " WHERE crs_users.CrsCod=%ld"
		          " AND crs_users.UsrCod=usr_data.UsrCod"
		          " AND %s",
		        Gbl.Hierarchy.Crs.CrsCod,SubQuery);
      default:
	 Err_WrongScopeExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/************************* Get old users from database ***********************/
/*****************************************************************************/

unsigned Usr_DB_GetOldUsrs (MYSQL_RES **mysql_res,time_t SecondsWithoutAccess)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get old users",
		   "SELECT UsrCod"
		    " FROM (SELECT UsrCod"
			    " FROM usr_last"
			   " WHERE LastTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%llu)"
			   " UNION "
			   "SELECT UsrCod"
			    " FROM usr_data"
			   " WHERE UsrCod NOT IN"
			         " (SELECT UsrCod"
				    " FROM usr_last)"
			  ") AS candidate_usrs"
		   " WHERE UsrCod NOT IN"
		         " (SELECT DISTINCT UsrCod"
			    " FROM crs_users)",
		   (unsigned long long) SecondsWithoutAccess);
  }

/*****************************************************************************/
/*************************** Remove user's last data *************************/
/*****************************************************************************/

void Usr_DB_RemoveUsrLastData (long UsrCod)
  {
   DB_QueryDELETE ("can not remove user's last data",
		   "DELETE FROM usr_last"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/****************************** Remove user's data ***************************/
/*****************************************************************************/

void Usr_DB_RemoveUsrData (long UsrCod)
  {
   /***** Remove user's data *****/
   DB_QueryDELETE ("can not remove user's data",
		   "DELETE FROM usr_data"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/*** Insert my user's code in the table of birthdays already congratulated ***/
/*****************************************************************************/

void Usr_DB_MarkMyBirthdayAsCongratulated (void)
  {
   DB_QueryINSERT ("can not insert birthday",
		   "INSERT INTO usr_birthdays_today"
	           " (UsrCod,Today)"
	           " VALUES"
	           " (%ld,CURDATE())",
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*************** Check if my birthday is already congratulated ***************/
/*****************************************************************************/

bool Usr_DB_CheckIfMyBirthdayHasNotBeenCongratulated (void)
  {
   return (DB_QueryCOUNT ("can not check if my birthday has been congratulated",
			  "SELECT COUNT(*)"
			   " FROM usr_birthdays_today"
			  " WHERE UsrCod=%ld",
			  Gbl.Usrs.Me.UsrDat.UsrCod) == 0);
  }

/*****************************************************************************/
/****************************** Delete old birthdays *************************/
/*****************************************************************************/

void Usr_DB_DeleteOldBirthdays (void)
  {
   DB_QueryDELETE ("can not delete old birthdays",
		   "DELETE FROM usr_birthdays_today"
		   " WHERE Today<>CURDATE()");
  }
