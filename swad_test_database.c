// swad_test_database.c: self-assessment tests, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Ca�as Vargas

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

#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_question.h"
#include "swad_test_config.h"
#include "swad_test_print.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

const char *Tst_DB_Pluggable[TstCfg_NUM_OPTIONS_PLUGGABLE] =
  {
   [TstCfg_PLUGGABLE_UNKNOWN] = "unknown",
   [TstCfg_PLUGGABLE_NO     ] = "N",
   [TstCfg_PLUGGABLE_YES    ] = "Y",
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************** Update my number of test prints in this course ***************/
/*****************************************************************************/

void Tst_DB_IncreaseNumMyPrints (void)
  {
   /***** Trivial check: do I belong to current course? *****/
   if (Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS] == Usr_DONT_BELONG)
      return;

   /***** Update my number of accesses to test in this course *****/
   DB_QueryUPDATE ("can not update the number of accesses to test",
		   "UPDATE crs_user_settings"
		     " SET NumAccTst=NumAccTst+1"
                   " WHERE UsrCod=%ld"
                     " AND CrsCod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
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
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
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
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
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
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/************** Save configuration of test for current course ****************/
/*****************************************************************************/

void Tst_DB_SaveConfig (void)
  {
   DB_QueryREPLACE ("can not save configuration of tests",
		    "REPLACE INTO tst_config"
	            " (CrsCod,Pluggable,Min,Def,Max,"
	              "MinTimeNxtTstPerQst,Visibility)"
                    " VALUES"
                    " (%ld,'%s',%u,%u,%u,"
                      "'%lu',%u)",
		    Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		    Tst_DB_Pluggable[TstCfg_GetConfigPluggable ()],
		    TstCfg_GetConfigMin (),
		    TstCfg_GetConfigDef (),
		    TstCfg_GetConfigMax (),
		    TstCfg_GetConfigMinTimeNxtTstPerQst (),
		    TstCfg_GetConfigVisibility ());
  }

/*****************************************************************************/
/***************** Get configuration of test in a course *********************/
/*****************************************************************************/

unsigned Tst_DB_GetConfig (MYSQL_RES **mysql_res,long CrsCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get configuration of test",
		   "SELECT Pluggable,"			// row[0]
			  "Min,"			// row[1]
			  "Def,"			// row[2]
			  "Max,"			// row[3]
			  "MinTimeNxtTstPerQst,"	// row[4]
			  "Visibility"			// row[5]
		    " FROM tst_config"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/********* Get pluggability of tests for current course from database ********/
/*****************************************************************************/

unsigned Tst_DB_GetPluggableFromConfig (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get configuration of test",
		   "SELECT Pluggable"		// row[0]
		    " FROM tst_config"
		   " WHERE CrsCod=%ld",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/****************** Remove test configuration in a course ********************/
/*****************************************************************************/

void Tst_DB_RemoveTstConfig (long CrsCod)
  {
   DB_QueryDELETE ("can not remove configuration of tests of a course",
		   "DELETE FROM tst_config"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/**************** Create new blank test print in database ********************/
/*****************************************************************************/

long Tst_DB_CreatePrint (unsigned NumQsts)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new test print",
				"INSERT INTO tst_exams"
				" (CrsCod,UsrCod,StartTime,EndTime,"
				  "NumQsts,NumQstsNotBlank,"
				  "Sent,AllowTeachers,Score)"
				" VALUES"
				" (%ld,%ld,NOW(),NOW(),"
				  "%u,0,"
				  "'N','N',0)",
				Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				NumQsts);
  }

/*****************************************************************************/
/*********************** Update test print in database ***********************/
/*****************************************************************************/

void Tst_DB_UpdatePrint (const struct TstPrn_Print *Print)
  {
   extern const char HidVis_Visible_YN[HidVis_NUM_HIDDEN_VISIBLE];

   Str_SetDecimalPointToUS ();		// To print the floating point as a dot
   DB_QueryUPDATE ("can not update test",
		   "UPDATE tst_exams"
	             " SET EndTime=NOW(),"
	                  "NumQstsNotBlank=%u,"
		          "Sent='%c',"
		          "AllowTeachers='%c',"
	                  "Score='%.15lg'"
	           " WHERE ExaCod=%ld"
	             " AND CrsCod=%ld"	// Extra check
	             " AND UsrCod=%ld",	// Extra check
		   Print->NumQsts.NotBlank,
		   Print->Sent ? 'Y' :
			         'N',
		   HidVis_Visible_YN[Print->VisibleByTchs],
		   Print->Score,
		   Print->PrnCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/************************ Get test prints between dates **********************/
/*****************************************************************************/

unsigned Tst_DB_GetPrintsBetweenDates (MYSQL_RES **mysql_res,
                                       const char *From,
                                       const char *To)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get prints",
		   "SELECT ExaCod,"			// row[0]
                          "UsrCod,"			// row[1]
			  "UNIX_TIMESTAMP(StartTime),"	// row[2]
			  "UNIX_TIMESTAMP(EndTime),"	// row[3]
                   	  "NumQsts,"  			// row[4]
                   	  "NumQstsNotBlank,"  		// row[5]
                   	  "Score"  			// row[6]
		    " FROM tst_exams"
		   " WHERE StartTime>='%s'"
                     " AND StartTime<='%s'"
		" ORDER BY ExaCod",
		   From,
		   To);
  }

/*****************************************************************************/
/************ Get the test prints of a user in the current course ************/
/*****************************************************************************/

unsigned Tst_DB_GetUsrPrintsInCurrentCrs (MYSQL_RES **mysql_res,long UsrCod)
  {
   /***** Make database query *****/
   /*           From here...                 ...to here
         ___________|_____                   _____|___________
   -----|______Exam_|_____|-----------------|_____|_Exam______|-----> time
      Start         |    End              Start   |          End
   */
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get tests of a user",
		   "SELECT ExaCod"			// row[0]
		    " FROM tst_exams"
		   " WHERE CrsCod=%ld"
		     " AND UsrCod=%ld"
		     " AND EndTime>=FROM_UNIXTIME(%ld)"
		     " AND StartTime<=FROM_UNIXTIME(%ld)"
		" ORDER BY ExaCod",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   UsrCod,
		   (long) Dat_GetRangeTimeUTC (Dat_STR_TIME),
		   (long) Dat_GetRangeTimeUTC (Dat_END_TIME));
  }

/*****************************************************************************/
/**************** Get data of a test using its test code *********************/
/*****************************************************************************/

unsigned Tst_DB_GetPrintDataByPrnCod (MYSQL_RES **mysql_res,long PrnCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of a test print",
		   "SELECT UsrCod,"			// row[0]
			  "UNIX_TIMESTAMP(StartTime),"	// row[1]
			  "UNIX_TIMESTAMP(EndTime),"	// row[2]
			  "NumQsts,"			// row[3]
			  "NumQstsNotBlank,"		// row[4]
			  "Sent,"			// row[5]
			  "AllowTeachers,"		// row[6]
			  "Score"			// row[7]
		    " FROM tst_exams"
		   " WHERE ExaCod=%ld"
		     " AND CrsCod=%ld",		// Extra check
		   PrnCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/****************** Remove test prints made by a given user ******************/
/*****************************************************************************/

void Tst_DB_RemovePrintsMadeByUsrInAllCrss (long UsrCod)
  {
   DB_QueryDELETE ("can not remove tests made by a user",
		   "DELETE FROM tst_exams"
	           " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/************** Remove test prints made by a user in a course ****************/
/*****************************************************************************/

void Tst_DB_RemovePrintsMadeByUsrInCrs (long UsrCod,long CrsCod)
  {
   DB_QueryDELETE ("can not remove tests made by a user in a course",
		   "DELETE FROM tst_exams"
	           " WHERE CrsCod=%ld"
	             " AND UsrCod=%ld",
		   CrsCod,
		   UsrCod);
  }

/*****************************************************************************/
/****************** Remove all test prints made in a course ******************/
/*****************************************************************************/

void Tst_DB_RemovePrintsMadeByInCrs (long CrsCod)
  {
   DB_QueryDELETE ("can not remove tests made in a course",
		   "DELETE FROM tst_exams"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/************ Store user's answers of an test print into database ************/
/*****************************************************************************/

void Tst_DB_StoreOneQstOfPrint (const struct TstPrn_Print *Print,unsigned QstInd)
  {
   /***** Insert question and user's answers into database *****/
   Str_SetDecimalPointToUS ();	// To print the floating point as a dot
   DB_QueryREPLACE ("can not update a question of a test",
		    "REPLACE INTO tst_exam_questions"
		    " (ExaCod,QstCod,QstInd,Score,Indexes,Answers)"
		    " VALUES"
		    " (%ld,%ld,%u,'%.15lg','%s','%s')",
		    Print->PrnCod,
		    Print->PrintedQuestions[QstInd].QstCod,
		    QstInd,	// 0, 1, 2, 3...
		    Print->PrintedQuestions[QstInd].Answer.Score,
		    Print->PrintedQuestions[QstInd].StrIndexes,
		    Print->PrintedQuestions[QstInd].Answer.Str);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/**************** Get all tags of questions in a test print ******************/
/*****************************************************************************/

unsigned Tst_DB_GetTagsPresentInAPrint (MYSQL_RES **mysql_res,long PrnCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get tags present in a test print",
		   "SELECT tst_tags.TagTxt"	// row[0]
		    " FROM (SELECT DISTINCT "
		                  "tst_question_tags.TagCod"
			    " FROM tst_question_tags,"
				  "tst_exam_questions"
			   " WHERE tst_exam_questions.ExaCod=%ld"
			     " AND tst_exam_questions.QstCod=tst_question_tags.QstCod) AS TagsCods,"
			  "tst_tags"
		   " WHERE TagsCods.TagCod=tst_tags.TagCod"
		" ORDER BY tst_tags.TagTxt",
		   PrnCod);
  }

/*****************************************************************************/
/************* Get the questions of a test print from database ***************/
/*****************************************************************************/

unsigned Tst_DB_GetPrintQuestions (MYSQL_RES **mysql_res,long PrnCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get questions of a test print",
		   "SELECT QstCod,"	// row[0]
			  "Score,"	// row[1]
			  "Indexes,"	// row[2]
			  "Answers"	// row[3]
		    " FROM tst_exam_questions"
		   " WHERE ExaCod=%ld"
		" ORDER BY QstInd",
		   PrnCod);
  }

/*****************************************************************************/
/*************** Remove test prints questions for a given user ***************/
/*****************************************************************************/

void Tst_DB_RemovePrintQuestionsMadeByUsrInAllCrss (long UsrCod)
  {
   DB_QueryDELETE ("can not remove tests made by a user",
		   "DELETE FROM tst_exam_questions"
	           " USING tst_exams,"
	                  "tst_exam_questions"
                   " WHERE tst_exams.UsrCod=%ld"
                     " AND tst_exams.ExaCod=tst_exam_questions.ExaCod",
		   UsrCod);
  }

/*****************************************************************************/
/********* Remove test prints questions made by a user in a course ***********/
/*****************************************************************************/

void Tst_DB_RemovePrintQuestionsMadeByUsrInCrs (long UsrCod,long CrsCod)
  {
   DB_QueryDELETE ("can not remove tests made by a user in a course",
		   "DELETE FROM tst_exam_questions"
	           " USING tst_exams,"
	                  "tst_exam_questions"
                   " WHERE tst_exams.CrsCod=%ld"
                     " AND tst_exams.UsrCod=%ld"
                     " AND tst_exams.ExaCod=tst_exam_questions.ExaCod",
		   CrsCod,
		   UsrCod);
  }

/*****************************************************************************/
/******* Remove all test prints questions made by any user in a course *******/
/*****************************************************************************/

void Tst_DB_RemovePrintQuestionsMadeInCrs (long CrsCod)
  {
   DB_QueryDELETE ("can not remove tests made in a course",
		   "DELETE FROM tst_exam_questions"
	           " USING tst_exams,"
	                  "tst_exam_questions"
                   " WHERE tst_exams.CrsCod=%ld"
                     " AND tst_exams.ExaCod=tst_exam_questions.ExaCod",
		   CrsCod);
  }
