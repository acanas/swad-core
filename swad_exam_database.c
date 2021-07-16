// swad_exam_database.c: exams operations with database

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
//#include <stdlib.h>		// For system, getenv, etc.
//#include <string.h>		// For string functions

//#include "swad_action.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_exam_database.h"
#include "swad_exam_log.h"
#include "swad_exam_print.h"
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
/***************** Get sets of questions in a given exam *********************/
/*****************************************************************************/

unsigned Exa_DB_GetExamSets (MYSQL_RES **mysql_res,long ExaCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get sets of questions",
		   "SELECT SetCod,"		// row[0]
			  "NumQstsToPrint,"	// row[1]
			  "Title"		// row[2]
		    " FROM exa_sets"
		   " WHERE ExaCod=%ld"
		   " ORDER BY SetInd",
		   ExaCod);
  }

/*****************************************************************************/
/******************* Get some random questions from a set ********************/
/*****************************************************************************/

unsigned Exa_DB_GetSomeQstsFromSetToPrint (MYSQL_RES **mysql_res,
                                           long SetCod,unsigned NumQstsToPrint)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get questions from set",
		   "SELECT QstCod,"	// row[0]
			  "AnsType,"	// row[1]
			  "Shuffle"	// row[2]
		    " FROM exa_set_questions"
		   " WHERE SetCod=%ld"
		   " ORDER BY RAND()"	// Don't use RAND(NOW()) because the same ordering will be repeated across sets
		   " LIMIT %u",
		   SetCod,
		   NumQstsToPrint);
  }

/*****************************************************************************/
/********** Get validity and answer type from a question in a set ************/
/*****************************************************************************/

unsigned Exa_DB_GetValidityAndTypeOfQuestion (MYSQL_RES **mysql_res,long QstCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get a question",
		   "SELECT Invalid,"	// row[0]
			  "AnsType"	// row[1]
		    " FROM exa_set_questions"
		   " WHERE QstCod=%ld",
		   QstCod);
  }

/*****************************************************************************/
/************** Get answers text for a question in an exam set ***************/
/*****************************************************************************/

unsigned Exa_DB_GetQstAnswersTextFromSet (MYSQL_RES **mysql_res,long QstCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get text of answers of a question",
		   "SELECT Answer"		// row[0]
		    " FROM exa_set_answers"
		   " WHERE QstCod=%ld",
		   QstCod);
  }

/*****************************************************************************/
/********** Get answers correctness for a question in an exam set ************/
/*****************************************************************************/

unsigned Exa_DB_GetQstAnswersCorrFromSet (MYSQL_RES **mysql_res,long QstCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get correctness of answers of a question",
		   "SELECT Correct"		// row[0]
		    " FROM exa_set_answers"
		   " WHERE QstCod=%ld"
		   " ORDER BY AnsInd",
		   QstCod);
  }

/*****************************************************************************/
/***************** Create new blank exam print in database *******************/
/*****************************************************************************/

long Exa_DB_CreatePrint (const struct ExaPrn_Print *Print)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new exam print",
				"INSERT INTO exa_prints"
				" (SesCod,UsrCod,StartTime,EndTime,"
				  "NumQsts,NumQstsNotBlank,Sent,Score)"
				" VALUES"
				" (%ld,%ld,NOW(),NOW(),"
				  "%u,0,'N',0)",
				Print->SesCod,
				Print->UsrCod,
				Print->NumQsts.All);
  }

/*****************************************************************************/
/********************** Update exam print in database ************************/
/*****************************************************************************/

void Exa_DB_UpdatePrint (const struct ExaPrn_Print *Print)
  {
   /***** Update exam print in database *****/
   Str_SetDecimalPointToUS ();		// To print the floating point as a dot
   DB_QueryUPDATE ("can not update exam print",
		   "UPDATE exa_prints"
	           " SET EndTime=NOW(),"
	                "NumQstsNotBlank=%u,"
		        "Sent='%c',"
	                "Score='%.15lg'"
	           " WHERE PrnCod=%ld"
	             " AND SesCod=%ld"
	             " AND UsrCod=%ld",	// Extra checks
		   Print->NumQsts.NotBlank,
		   Print->Sent ? 'Y' :
			         'N',
		   Print->Score,
		   Print->PrnCod,
		   Print->SesCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/**************** Get data of an exam print using print code *****************/
/*****************************************************************************/

unsigned Exa_DB_GetDataOfPrintByPrnCod (MYSQL_RES **mysql_res,long PrnCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of an exam print",
		   "SELECT PrnCod,"			// row[0]
			  "SesCod,"			// row[1]
			  "UsrCod,"			// row[2]
			  "UNIX_TIMESTAMP(StartTime),"	// row[3]
			  "UNIX_TIMESTAMP(EndTime),"	// row[4]
			  "NumQsts,"			// row[5]
			  "NumQstsNotBlank,"		// row[6]
			  "Sent,"			// row[7]
			  "Score"			// row[8]
		    " FROM exa_prints"
		   " WHERE PrnCod=%ld",
		   PrnCod);
  }

/*****************************************************************************/
/******** Get data of an exam print using session code and user code *********/
/*****************************************************************************/

unsigned Exa_DB_GetDataOfPrintBySesCodAndUsrCod (MYSQL_RES **mysql_res,
                                                 long SesCod,long UsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of an exam print",
		   "SELECT PrnCod,"			// row[0]
			  "SesCod,"			// row[1]
			  "UsrCod,"			// row[2]
			  "UNIX_TIMESTAMP(StartTime),"	// row[3]
			  "UNIX_TIMESTAMP(EndTime),"	// row[4]
			  "NumQsts,"			// row[5]
			  "NumQstsNotBlank,"		// row[6]
			  "Sent,"			// row[7]
			  "Score"			// row[8]
		    " FROM exa_prints"
		   " WHERE SesCod=%ld"
		     " AND UsrCod=%ld",
		   SesCod,
		   UsrCod);
  }

/*****************************************************************************/
/******************* Remove exam prints for a given user *********************/
/*****************************************************************************/

void Exa_DB_RemovePrintsMadeByUsrInAllCrss (long UsrCod)
  {
   DB_QueryDELETE ("can not remove exam prints made by a user",
		   "DELETE FROM exa_prints"
	           " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/*************** Remove exam prints made by a user in a course ***************/
/*****************************************************************************/

void Exa_DB_RemovePrintsMadeByUsrInCrs (long UsrCod,long CrsCod)
  {
   DB_QueryDELETE ("can not remove exams prints made by a user in a course",
		   "DELETE FROM exa_prints"
	           " USING exa_exams,"
	                  "exa_sessions,"
	                  "exa_prints"
                   " WHERE exa_exams.CrsCod=%ld"
                     " AND exa_exams.ExaCod=exa_sessions.ExaCod"
                     " AND exa_sessions.SesCod=exa_prints.SesCod"
                     " AND exa_prints.UsrCod=%ld",
		   CrsCod,
		   UsrCod);
  }

/*****************************************************************************/
/******* Remove exams prints made by the given user in the given course ******/
/*****************************************************************************/

void Exa_DB_RemovePrintsInCrs (long CrsCod)
  {
   DB_QueryDELETE ("can not remove exams prints in a course",
		   "DELETE FROM exa_prints"
	           " USING exa_exams,"
	                  "exa_sessions,"
	                  "exa_prints"
                   " WHERE exa_exams.CrsCod=%ld"
                     " AND exa_exams.ExaCod=exa_sessions.ExaCod"
                     " AND exa_sessions.SesCod=exa_prints.SesCod",
		   CrsCod);
  }

/*****************************************************************************/
/************* Store user's answers of an test exam into database ************/
/*****************************************************************************/

void Exa_DB_StoreOneQstOfPrint (const struct ExaPrn_Print *Print,
                                unsigned QstInd)
  {
   Str_SetDecimalPointToUS ();	// To print the floating point as a dot
   DB_QueryREPLACE ("can not update a question in an exam print",
		    "REPLACE INTO exa_print_questions"
		    " (PrnCod,QstCod,QstInd,SetCod,Score,Indexes,Answers)"
		    " VALUES"
		    " (%ld,%ld,%u,%ld,'%.15lg','%s','%s')",
		    Print->PrnCod,
		    Print->PrintedQuestions[QstInd].QstCod,
		    QstInd,	// 0, 1, 2, 3...
		    Print->PrintedQuestions[QstInd].SetCod,
		    Print->PrintedQuestions[QstInd].Score,
		    Print->PrintedQuestions[QstInd].StrIndexes,
		    Print->PrintedQuestions[QstInd].StrAnswers);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/************* Get the questions of an exam print from database **************/
/*****************************************************************************/

unsigned Exa_DB_GetPrintQuestions (MYSQL_RES **mysql_res,long PrnCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get questions of an exam print",
		   "SELECT QstCod,"	// row[0]
			  "SetCod,"	// row[1]
			  "Score,"	// row[2]
			  "Indexes,"	// row[3]
			  "Answers"	// row[4]
		    " FROM exa_print_questions"
		   " WHERE PrnCod=%ld"
		   " ORDER BY QstInd",
		   PrnCod);
  }

/*****************************************************************************/
/************** Get the answers of an exam print from database ***************/
/*****************************************************************************/

void Exa_DB_GetAnswersFromQstInPrint (long PrnCod,long QstCod,
                                      char StrAnswers[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1])
  {
   DB_QuerySELECTString (StrAnswers,Tst_MAX_BYTES_ANSWERS_ONE_QST,
                         "can not get answer in an exam print",
			 "SELECT Answers"
			  " FROM exa_print_questions"
			 " WHERE PrnCod=%ld"
			   " AND QstCod=%ld",
			 PrnCod,QstCod);
  }

/*****************************************************************************/
/************ Get number of questions not blank in an exam print *************/
/*****************************************************************************/

unsigned Exa_DB_GetNumQstsNotBlankInPrint (long PrnCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of questions not blank",
		  "SELECT COUNT(*)"
		   " FROM exa_print_questions"
		  " WHERE PrnCod=%ld"
		    " AND Answers<>''",
		  PrnCod);
  }

/*****************************************************************************/
/************* Compute total score of questions of an exam print *************/
/*****************************************************************************/

double Exa_DB_ComputeTotalScoreOfPrint (long PrnCod)
  {
   return DB_QuerySELECTDouble ("can not get score of exam print",
				"SELECT SUM(Score)"
				 " FROM exa_print_questions"
				" WHERE PrnCod=%ld",
				PrnCod);
  }

/*****************************************************************************/
/*************** Remove exam prints questions for a given user ***************/
/*****************************************************************************/

void Exa_DB_RemovePrintQuestionsMadeByUsrInAllCrss (long UsrCod)
  {
   DB_QueryDELETE ("can not remove exam prints made by a user",
		   "DELETE FROM exa_print_questions"
	           " USING exa_prints,"
	                  "exa_print_questions"
                   " WHERE exa_prints.UsrCod=%ld"
                     " AND exa_prints.PrnCod=exa_print_questions.PrnCod",
		   UsrCod);
  }

/*****************************************************************************/
/* Remove questions of exams prints made by the given user in a given course */
/*****************************************************************************/

void Exa_DB_RemovePrintsQuestionsMadeByUsrInCrs (long UsrCod,long CrsCod)
  {
   DB_QueryDELETE ("can not remove exams prints made by a user in a course",
		   "DELETE FROM exa_print_questions"
	           " USING exa_exams,"
	                  "exa_sessions,"
	                  "exa_prints,"
	                  "exa_print_questions"
                   " WHERE exa_exams.CrsCod=%ld"
                     " AND exa_exams.ExaCod=exa_sessions.ExaCod"
                     " AND exa_sessions.SesCod=exa_prints.SesCod"
                     " AND exa_prints.UsrCod=%ld"
                     " AND exa_prints.PrnCod=exa_print_questions.PrnCod",
		   CrsCod,
		   UsrCod);
  }

/*****************************************************************************/
/* Remove questions of exams prints made by the given user in a given course */
/*****************************************************************************/

void Exa_DB_RemovePrintQuestionsInCrs (long CrsCod)
  {
   DB_QueryDELETE ("can not remove exams prints in a course",
		   "DELETE FROM exa_print_questions"
	           " USING exa_exams,"
	                  "exa_sessions,"
	                  "exa_prints,"
	                  "exa_print_questions"
                   " WHERE exa_exams.CrsCod=%ld"
                     " AND exa_exams.ExaCod=exa_sessions.ExaCod"
                     " AND exa_sessions.SesCod=exa_prints.SesCod"
                     " AND exa_prints.PrnCod=exa_print_questions.PrnCod",
		   CrsCod);
  }

/*****************************************************************************/
/******** Check if the current session id is the same as the last one ********/
/*****************************************************************************/

bool Exa_DB_CheckIfSessionIsTheSameAsTheLast (long PrnCod)
  {
   /***** Check if the current session id
          is the same as the last one stored in database *****/
   return (DB_QueryCOUNT ("can not check session",
			  "SELECT COUNT(*)"
			   " FROM exa_log_sessions"
			  " WHERE LogCod="
				 "(SELECT MAX(LogCod)"
				   " FROM exa_log_sessions"
				  " WHERE PrnCod=%ld)"
			    " AND SessionId='%s'",
			  PrnCod,
			  Gbl.Session.Id) != 0);
  }

/*****************************************************************************/
/******** Check if the current user agent is the same as the last one ********/
/*****************************************************************************/

bool Exa_DB_CheckIfUserAgentIsTheSameAsTheLast (long PrnCod,const char *UserAgentDB)
  {
   /***** Get if the current user agent
          is the same as the last stored in database *****/
   return (DB_QueryCOUNT ("can not check user agent",
			  "SELECT COUNT(*)"
			   " FROM exa_log_user_agents"
			  " WHERE LogCod="
				 "(SELECT MAX(LogCod)"
				   " FROM exa_log_user_agents"
				  " WHERE PrnCod=%ld)"
			    " AND UserAgent='%s'",
			  PrnCod,
			  UserAgentDB) != 0);
  }

/*****************************************************************************/
/******************************** Log access *********************************/
/*****************************************************************************/

void Exa_DB_LogAccess (long LogCod,long PrnCod,ExaLog_Action_t Action)
  {
   /* Log access in exam log.
      Redundant data (also present in log table) are stored for speed */
   DB_QueryINSERT ("can not log exam access",
		   "INSERT INTO exa_log "
		   "(LogCod,PrnCod,ActCod,QstInd,CanAnswer,ClickTime,IP)"
		   " VALUES "
		   "(%ld,%ld,%ld,%d,'%c',NOW(),'%s')",
		   LogCod,
		   PrnCod,
		   (unsigned) Action,
		   ExaLog_GetQstInd (),
		   ExaLog_GetIfCanAnswer () ? 'Y' :
					      'N',
		   // NOW()   	   Redundant, for speed
		   Gbl.IP);	// Redundant, for speed
  }

/*****************************************************************************/
/*************************** Log session in database *************************/
/*****************************************************************************/

void Exa_DB_LogSession (long LogCod,long PrnCod)
  {
   DB_QueryINSERT ("can not log session",
		   "INSERT INTO exa_log_sessions "
		   "(LogCod,PrnCod,SessionId)"
		   " VALUES "
		   "(%ld,%ld,'%s')",
		   LogCod,
		   PrnCod,
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/************************* Log user agent in database ************************/
/*****************************************************************************/

void Exa_DB_LogUserAgent (long LogCod,long PrnCod,const char *UserAgentDB)
  {
   DB_QueryINSERT ("can not log user agent",
		   "INSERT INTO exa_log_user_agents "
		   "(LogCod,PrnCod,UserAgent)"
		   " VALUES "
		   "(%ld,%ld,'%s')",
		   LogCod,
		   PrnCod,
		   UserAgentDB);
  }

/*****************************************************************************/
/********************* Get exam print log from database **********************/
/*****************************************************************************/

unsigned Exa_DB_GetExamLog (MYSQL_RES **mysql_res,long PrnCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get exam print log",
		   "SELECT exa_log.ActCod,"			// row[0]
			  "exa_log.QstInd,"			// row[1]
			  "exa_log.CanAnswer,"			// row[2]
			  "UNIX_TIMESTAMP(exa_log.ClickTime),"	// row[3]
			  "exa_log.IP,"				// row[4]
			  "exa_log_sessions.SessionId,"		// row[5]
			  "exa_log_user_agents.UserAgent"	// row[6]
		    " FROM exa_log"
		    " LEFT JOIN exa_log_sessions"
		      " ON exa_log.LogCod=exa_log_sessions.LogCod"
		    " LEFT JOIN exa_log_user_agents"
		      " ON exa_log.LogCod=exa_log_user_agents.LogCod"
		   " WHERE exa_log.PrnCod=%ld"
		   " ORDER BY exa_log.LogCod",
		   PrnCod);
  }

/*****************************************************************************/
/*** Get all users who have answered any session question in a given exam ****/
/*****************************************************************************/

unsigned Exa_DB_GetAllUsrsWhoHaveMadeExam (MYSQL_RES **mysql_res,long ExaCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users in exam",
		   "SELECT users.UsrCod"	// row[0]
		    " FROM (SELECT DISTINCT exa_prints.UsrCod AS UsrCod"
			    " FROM exa_prints,"
			          "exa_sessions,"
			          "exa_exams"
			   " WHERE exa_sessions.ExaCod=%ld"
			     " AND exa_sessions.SesCod=exa_prints.SesCod"
			     " AND exa_sessions.ExaCod=exa_exams.ExaCod"
			     " AND exa_exams.CrsCod=%ld) AS users,"		// Extra check
			  "usr_data"
		   " WHERE users.UsrCod=usr_data.UsrCod"
		   " ORDER BY usr_data.Surname1,"
			     "usr_data.Surname2,"
			     "usr_data.FirstName",
		   ExaCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/*** Get all users who have answered any question in a given exam session ****/
/*****************************************************************************/

unsigned Exa_DB_GetAllUsrsWhoHaveMadeSession (MYSQL_RES **mysql_res,long SesCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users in session",
		   "SELECT users.UsrCod"	// row[0]
		    " FROM (SELECT exa_prints.UsrCod AS UsrCod"
			    " FROM exa_prints,exa_sessions,exa_exams"
			   " WHERE exa_prints.SesCod=%ld"
			     " AND exa_prints.SesCod=exa_sessions.SesCod"
			     " AND exa_sessions.ExaCod=exa_exams.ExaCod"
			     " AND exa_exams.CrsCod=%ld) AS users,"	// Extra check
			  "usr_data"
		   " WHERE users.UsrCod=usr_data.UsrCod"
		   " ORDER BY usr_data.Surname1,"
			     "usr_data.Surname2,"
			     "usr_data.FirstName",
		   SesCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/********* Show the sessions results of a user in the current course *********/
/*****************************************************************************/

unsigned Exa_DB_GetResults (MYSQL_RES **mysql_res,
			    Usr_MeOrOther_t MeOrOther,
			    long SesCod,	// <= 0 ==> any
			    long ExaCod,	// <= 0 ==> any
			    const char *ExamsSelectedCommas)
  {
   char *SesSubQuery;
   char *HidSesSubQuery;
   char *HidExaSubQuery;
   char *ExaSubQuery;
   long UsrCod;
   unsigned NumResults;

   /***** Set user *****/
   UsrCod = (MeOrOther == Usr_ME) ? Gbl.Usrs.Me.UsrDat.UsrCod :
				    Gbl.Usrs.Other.UsrDat.UsrCod;

   /***** Build sessions subquery *****/
   if (SesCod > 0)	// One unique session
     {
      if (asprintf (&SesSubQuery," AND exa_prints.SesCod=%ld",SesCod) < 0)
	 Err_NotEnoughMemoryExit ();
     }
   else			// All sessions of selected exams
     {
      if (asprintf (&SesSubQuery,"%s","") < 0)
	 Err_NotEnoughMemoryExit ();
     }

   /***** Subquery: get hidden sessions?
	  · A student will not be able to see their results in hidden sessions
	  · A teacher will be able to see results from other users even in hidden sessions
   *****/
   switch (MeOrOther)
     {
      case Usr_ME:	// A student watching her/his results
         if (asprintf (&HidSesSubQuery," AND exa_sessions.Hidden='N'") < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
      case Usr_OTHER:	// A teacher/admin watching the results of other users
      default:
	 if (asprintf (&HidSesSubQuery,"%s","") < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
     }

   /***** Build exams subquery *****/
   if (ExaCod > 0)			// One unique exams
     {
      if (asprintf (&ExaSubQuery," AND exa_sessions.ExaCod=%ld",ExaCod) < 0)
	 Err_NotEnoughMemoryExit ();
     }
   else if (ExamsSelectedCommas)
     {
      if (ExamsSelectedCommas[0])	// Selected exams
	{
	 if (asprintf (&ExaSubQuery," AND exa_sessions.ExaCod IN (%s)",
		       ExamsSelectedCommas) < 0)
	    Err_NotEnoughMemoryExit ();
	}
      else
	{
	 if (asprintf (&ExaSubQuery,"%s","") < 0)
	    Err_NotEnoughMemoryExit ();
	}
     }
   else					// All exams
     {
      if (asprintf (&ExaSubQuery,"%s","") < 0)
	 Err_NotEnoughMemoryExit ();
     }

   /***** Subquery: get hidden exams?
	  · A student will not be able to see their results in hidden exams
	  · A teacher will be able to see results from other users even in hidden exams
   *****/
   switch (MeOrOther)
     {
      case Usr_ME:	// A student watching her/his results
         if (asprintf (&HidExaSubQuery," AND exa_exams.Hidden='N'") < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
      case Usr_OTHER:	// A teacher/admin watching the results of other users
      default:
	 if (asprintf (&HidExaSubQuery,"%s","") < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
     }

   /***** Make database query *****/
   // Do not filter by groups, because a student who has changed groups
   // must be able to access exams taken in other groups
   NumResults = (unsigned)
   DB_QuerySELECT (mysql_res,"can not get sessions results",
		   "SELECT exa_prints.PrnCod"			// row[0]
		    " FROM exa_prints,exa_sessions,exa_exams"
		   " WHERE exa_prints.UsrCod=%ld"
		      "%s"	// Session subquery
		     " AND exa_prints.SesCod=exa_sessions.SesCod"
		      "%s"	// Hidden sessions subquery
		      "%s"	// Exams subquery
		     " AND exa_sessions.ExaCod=exa_exams.ExaCod"
		      "%s"	// Hidden exams subquery
		     " AND exa_exams.CrsCod=%ld"		// Extra check
		   " ORDER BY exa_sessions.Title",
		   UsrCod,
		   SesSubQuery,
		   HidSesSubQuery,
		   ExaSubQuery,
		   HidExaSubQuery,
		   Gbl.Hierarchy.Crs.CrsCod);
   free (HidExaSubQuery);
   free (ExaSubQuery);
   free (HidSesSubQuery);
   free (SesSubQuery);

   return NumResults;
  }
