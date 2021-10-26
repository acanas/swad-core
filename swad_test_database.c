// swad_test_database.c: self-assessment tests, operations with database

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
/*********************************** Headers *********************************/
/*****************************************************************************/

// #define _GNU_SOURCE 		// For asprintf
// #include <limits.h>		// For UINT_MAX
// #include <linux/limits.h>	// For PATH_MAX
// #include <mysql/mysql.h>	// To access MySQL databases
// #include <stdbool.h>		// For boolean type
// #include <stddef.h>		// For NULL
// #include <stdio.h>		// For asprintf
// #include <stdlib.h>		// For exit, system, malloc, free, etc
// #include <string.h>		// For string functions
// #include <sys/stat.h>		// For mkdir
// #include <sys/types.h>		// For mkdir

// #include "swad_action.h"
// #include "swad_box.h"
#include "swad_database.h"
// #include "swad_error.h"
// #include "swad_exam_set.h"
// #include "swad_figure.h"
// #include "swad_form.h"
#include "swad_global.h"
// #include "swad_hierarchy_level.h"
// #include "swad_HTML.h"
// #include "swad_ID.h"
// #include "swad_language.h"
// #include "swad_match.h"
// #include "swad_media.h"
// #include "swad_parameter.h"
// #include "swad_question.h"
// #include "swad_question_import.h"
// #include "swad_tag_database.h"
// #include "swad_test.h"
#include "swad_test_config.h"
// #include "swad_test_print.h"
// #include "swad_test_visibility.h"
// #include "swad_theme.h"
// #include "swad_user.h"
// #include "swad_xml.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/************** Update my number of test prints in this course ***************/
/*****************************************************************************/

void Tst_DB_IncreaseNumMyPrints (void)
  {
   /***** Trivial check *****/
   if (!Gbl.Usrs.Me.IBelongToCurrentCrs)
      return;

   /***** Update my number of accesses to test in this course *****/
   DB_QueryUPDATE ("can not update the number of accesses to test",
		   "UPDATE crs_user_settings"
		     " SET NumAccTst=NumAccTst+1"
                   " WHERE UsrCod=%ld"
                     " AND CrsCod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/******** Update date-time and number of questions of this test print ********/
/*****************************************************************************/

void Tst_DB_UpdateLastAccTst (unsigned NumQsts)
  {
   DB_QueryUPDATE ("can not update time and number of questions of this test",
		   "UPDATE crs_user_settings"
		     " SET LastAccTst=NOW(),"
		          "NumQstsLastTst=%u"
                   " WHERE UsrCod=%ld"
                     " AND CrsCod=%ld",
		   NumQsts,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/********** Get date of next allowed access to test from database ************/
/*****************************************************************************/

unsigned Tst_DB_GetDateNextTstAllowed (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get date of last test print",
		   "SELECT UNIX_TIMESTAMP(LastAccTst+INTERVAL (NumQstsLastTst*%lu) SECOND)-"
			  "UNIX_TIMESTAMP(),"							// row[0]
			  "UNIX_TIMESTAMP(LastAccTst+INTERVAL (NumQstsLastTst*%lu) SECOND)"	// row[1]
		    " FROM crs_user_settings"
		   " WHERE UsrCod=%ld"
		     " AND CrsCod=%ld",
		   TstCfg_GetConfigMinTimeNxtTstPerQst (),
		   TstCfg_GetConfigMinTimeNxtTstPerQst (),
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/**************** Get number of test prints generated by me ******************/
/*****************************************************************************/

unsigned Tst_DB_GetNumPrintsGeneratedByMe (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get number of test prints generated",
		   "SELECT NumAccTst"	// row[0]
		    " FROM crs_user_settings"
		   " WHERE UsrCod=%ld"
		     " AND CrsCod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }
