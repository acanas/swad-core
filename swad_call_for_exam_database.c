// swad_exam_announcement_database.c: calls for exams operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_call_for_exam.h"
#include "swad_call_for_exam_database.h"
#include "swad_database.h"
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

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
				Gbl.Hierarchy.Node[Hie_CRS].HieCod,
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
/**************** Get all calls for exams in current course ******************/
/*****************************************************************************/

unsigned Cfe_DB_GetCallsForExamsInCurrentCrs (MYSQL_RES **mysql_res)
  {
   char SubQueryStatus[64];

   /***** Build subquery about status depending on my role *****/
   if (Cfe_CheckIfICanEditCallsForExams () == Usr_I_CAN)
      sprintf (SubQueryStatus,"Status<>%u",
	       (unsigned) Cfe_DELETED_CALL_FOR_EXAM);
   else
      sprintf (SubQueryStatus,"Status=%u",
	       (unsigned) Cfe_VISIBLE_CALL_FOR_EXAM);

   /***** Get calls for exams (the most recent first)
          in current course from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get calls for exams",
		   "SELECT ExaCod"	// row[0]
		    " FROM cfe_exams"
		   " WHERE CrsCod=%ld"
		     " AND %s"
		   " ORDER BY ExamDate DESC",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   SubQueryStatus);
  }

/*****************************************************************************/
/**************** Get all calls for exams in current course ******************/
/*****************************************************************************/

unsigned Cfe_DB_GetVisibleCallsForExamsInCurrentCrs (MYSQL_RES **mysql_res)
  {
   /***** Get exam dates (ordered from more recent to older)
	  of visible calls for exams
	  in current course from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get calls for exams",
		   "SELECT ExaCod,"		// row[0]
			  "DATE(ExamDate)"	// row[1]
		    " FROM cfe_exams"
		   " WHERE CrsCod=%ld"
		     " AND Status=%u"
		   " ORDER BY ExamDate DESC",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   (unsigned) Cfe_VISIBLE_CALL_FOR_EXAM);
  }

/*****************************************************************************/
/******* Get exam announcements (only future exams) in current course ********/
/*****************************************************************************/

unsigned Cfe_DB_GetFutureCallsForExamsInCurrentCrs (MYSQL_RES **mysql_res)
  {
   /***** Get exam dates (ordered from older to more recent)
	  of future visible calls for exams
	  in current course from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get calls for exams",
		   "SELECT ExaCod,"					// row[0]
			  "UNIX_TIMESTAMP(CallDate),"			// row[1]
			  "DATE_FORMAT(ExamDate,'%%d/%%m/%%Y %%H:%%i')"	// row[2]
		    " FROM cfe_exams"
		   " WHERE CrsCod=%ld"
		     " AND Status=%u"
		     " AND ExamDate>=NOW()"
		   " ORDER BY ExamDate",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   (unsigned) Cfe_VISIBLE_CALL_FOR_EXAM);
  }

/*****************************************************************************/
/***************** Get data of a call for exam from database *****************/
/*****************************************************************************/

unsigned Cfe_DB_GetCallForExamDataByCod (MYSQL_RES **mysql_res,long ExaCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of a call for exam",
		   "SELECT CrsCod,"		// row[ 0]
			  "Status,"		// row[ 1]
			  "CrsFullName,"	// row[ 2]
			  "Year,"		// row[ 3]
			  "ExamSession,"	// row[ 4]
			  "CallDate,"		// row[ 5]
			  "ExamDate,"		// row[ 6]
			  "Duration,"		// row[ 7]
			  "Place,"		// row[ 8]
			  "ExamMode,"		// row[ 9]
			  "Structure,"		// row[10]
			  "DocRequired,"	// row[11]
			  "MatRequired,"	// row[12]
			  "MatAllowed,"		// row[13]
			  "OtherInfo"		// row[14]
		    " FROM cfe_exams"
		   " WHERE ExaCod=%ld",
		   ExaCod);
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
                   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
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
                   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
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
                   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
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
