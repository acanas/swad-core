// swad_exam_log.c: exam access log

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

#include "swad_action.h"
// #include "swad_box.h"
#include "swad_database.h"
// #include "swad_exam.h"
#include "swad_exam_print.h"
// #include "swad_exam_result.h"
// #include "swad_exam_session.h"
// #include "swad_exam_set.h"
// #include "swad_exam_type.h"
// #include "swad_form.h"
#include "swad_global.h"

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
/**************************** Log access in database *************************/
/*****************************************************************************/

void ExaLog_LogAccess (long LogCod)
  {
   long PrnCod;

   if (Gbl.Action.Act == ActAnsExaPrn ||	// Answer question
       Gbl.Action.Act == ActSeeExaPrn ||	// Create/resume print exam
       Gbl.Action.Act == ActEndExaPrn)		// End print exam
     {
      PrnCod = ExaPrn_GetCurrentPrnCod ();

      if (PrnCod > 0)	// Only if exam print is accesible (visible, open...)
	 /***** Insert access into database *****/
	 /* Log access in exam log.
	    Redundant data (also present in log table) are stored for speed */
	 DB_QueryINSERT ("can not log exam access",
			 "INSERT INTO exa_log "
			 "(LogCod,PrnCod,ActCod,ClickTime,IP,SessionId)"
			 " VALUES "
			 "(%ld,%ld,%ld,NOW(),'%s','%s')",
			 LogCod,
			 PrnCod,
			 Act_GetActCod (Gbl.Action.Act),	// Redundant, for speed
			 // NOW()			  	   Redundant, for speed
			 Gbl.IP,				// Redundant, for speed
			 Gbl.Session.Id);
     }
  }
