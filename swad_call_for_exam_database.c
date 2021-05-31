// swad_exam_announcement_database.c: calls for exams operations with database

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
/********************************* Headers ***********************************/
/*****************************************************************************/

// #define _GNU_SOURCE 		// For asprintf
// #include <stddef.h>		// For NULL
// #include <stdio.h>		// For sscanf, asprintf, etc.
// #include <stdlib.h>		// For exit, system, malloc, calloc, free, etc.
// #include <string.h>		// For string functions

// #include "swad_box.h"
#include "swad_call_for_exam.h"
#include "swad_call_for_exam_database.h"
// #include "swad_config.h"
#include "swad_database.h"
// #include "swad_degree_database.h"
// #include "swad_error.h"
// #include "swad_form.h"
#include "swad_global.h"
// #include "swad_hierarchy.h"
// #include "swad_hierarchy_level.h"
// #include "swad_HTML.h"
// #include "swad_logo.h"
// #include "swad_notification.h"
// #include "swad_parameter.h"
// #include "swad_QR.h"
// #include "swad_RSS.h"
// #include "swad_string.h"
// #include "swad_timeline.h"
// #include "swad_timeline_database.h"

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
/******************** Add a call for exam to the database ********************/
/*****************************************************************************/
// Return the code of the call for exam just added

long Cfe_DB_CreateCallForExam (const struct Cfe_CallForExam *CallForExam)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create a new call for exam",
				"INSERT INTO cfe_exams "
				"(CrsCod,Status,NumNotif,CrsFullName,Year,ExamSession,"
				  "CallDate,ExamDate,Duration,"
				  "Place,ExamMode,Structure,"
				  "DocRequired,MatRequired,MatAllowed,OtherInfo)"
				" VALUES "
				"(%ld,%u,0,'%s',%u,'%s',"
				  "NOW(),'%04u-%02u-%02u %02u:%02u:00','%02u:%02u:00',"
				  "'%s','%s','%s',"
				  "'%s','%s','%s','%s')",
				Gbl.Hierarchy.Crs.CrsCod,
				(unsigned) Cfe_VISIBLE_CALL_FOR_EXAM,
				CallForExam->CrsFullName,
				CallForExam->Year,
				CallForExam->Session,
				CallForExam->ExamDate.Year,
				CallForExam->ExamDate.Month,
				CallForExam->ExamDate.Day,
				CallForExam->StartTime.Hour,
				CallForExam->StartTime.Minute,
				CallForExam->Duration.Hour,
				CallForExam->Duration.Minute,
				CallForExam->Place,
				CallForExam->Mode,
				CallForExam->Structure,
				CallForExam->DocRequired,
				CallForExam->MatRequired,
				CallForExam->MatAllowed,
				CallForExam->OtherInfo);
  }

/*****************************************************************************/
/***************** Modify a call for exam in the database ********************/
/*****************************************************************************/

void Cfe_DB_ModifyCallForExam (const struct Cfe_CallForExam *CallForExam,
                               long ExaCod)
  {
   /***** Modify call for exam *****/
   DB_QueryUPDATE ("can not update a call for exam",
		   "UPDATE cfe_exams"
		     " SET CrsFullName='%s',"
		          "Year=%u,"
		          "ExamSession='%s',"
		          "ExamDate='%04u-%02u-%02u %02u:%02u:00',"
		          "Duration='%02u:%02u:00',"
		          "Place='%s',"
		          "ExamMode='%s',"
		          "Structure='%s',"
		          "DocRequired='%s',"
		          "MatRequired='%s',"
		          "MatAllowed='%s',"
		          "OtherInfo='%s'"
		   " WHERE ExaCod=%ld",
	           CallForExam->CrsFullName,
	           CallForExam->Year,
	           CallForExam->Session,
	           CallForExam->ExamDate.Year,
	           CallForExam->ExamDate.Month,
	           CallForExam->ExamDate.Day,
	           CallForExam->StartTime.Hour,
	           CallForExam->StartTime.Minute,
	           CallForExam->Duration.Hour,
	           CallForExam->Duration.Minute,
	           CallForExam->Place,
	           CallForExam->Mode,
	           CallForExam->Structure,
	           CallForExam->DocRequired,
	           CallForExam->MatRequired,
	           CallForExam->MatAllowed,
	           CallForExam->OtherInfo,
	           ExaCod);
  }

/*****************************************************************************/
/******* Update number of users notified in table of calls for exams *********/
/*****************************************************************************/

void Cfe_DB_UpdateNumUsrsNotifiedByEMailAboutCallForExam (long ExaCod,
                                                          unsigned NumUsrsToBeNotifiedByEMail)
  {
   DB_QueryUPDATE ("can not update the number of notifications"
		   " of a call for exam",
		   "UPDATE cfe_exams"
		     " SET NumNotif=NumNotif+%u"
		   " WHERE ExaCod=%ld",
                   NumUsrsToBeNotifiedByEMail,
                   ExaCod);
  }

/*****************************************************************************/
/************* Mark the call for exam as hidden in the database **************/
/*****************************************************************************/

void Cfe_DB_HideCallForExam (long ExaCod)
  {
   DB_QueryUPDATE ("can not hide call for exam",
		   "UPDATE cfe_exams"
		     " SET Status=%u"
		   " WHERE ExaCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
                   (unsigned) Cfe_HIDDEN_CALL_FOR_EXAM,
                   ExaCod,
                   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/************* Mark the call for exam as visible in the database *************/
/*****************************************************************************/

void Cfe_DB_UnhideCallForExam (long ExaCod)
  {
   DB_QueryUPDATE ("can not unhide call for exam",
		   "UPDATE cfe_exams"
		     " SET Status=%u"
		   " WHERE ExaCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
                   (unsigned) Cfe_VISIBLE_CALL_FOR_EXAM,
                   ExaCod,
                   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
   /***** Mark a call for exam as deleted in the database *****/
/*****************************************************************************/

void Cfe_DB_MarkACallForExamAsDeleted (long ExaCod)
  {
   DB_QueryUPDATE ("can not remove call for exam",
		   "UPDATE cfe_exams"
		     " SET Status=%u"
		   " WHERE ExaCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
                   (unsigned) Cfe_DELETED_CALL_FOR_EXAM,
                   ExaCod,
                   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/*********** Mark all exam announcements in the course as deleted ************/
/*****************************************************************************/

void Cfe_DB_MarkCallForExamsInCrsAsDeleted (long CrsCod)
  {
   DB_QueryUPDATE ("can not remove calls for exams of a course",
		   "UPDATE cfe_exams"
		     " SET Status=%u"
		   " WHERE CrsCod=%ld",
		   (unsigned) Cfe_DELETED_CALL_FOR_EXAM,
		   CrsCod);
  }
