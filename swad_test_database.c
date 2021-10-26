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
#include <string.h>		// For string functions
// #include <sys/stat.h>		// For mkdir
// #include <sys/types.h>		// For mkdir

// #include "swad_action.h"
// #include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
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
#include "swad_question.h"
// #include "swad_question_import.h"
// #include "swad_tag_database.h"
// #include "swad_test.h"
#include "swad_test_config.h"
#include "swad_test_print.h"
// #include "swad_test_visibility.h"
// #include "swad_theme.h"
// #include "swad_user.h"
// #include "swad_xml.h"

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

/*****************************************************************************/
/*********************** Update the score of a question **********************/
/*****************************************************************************/

void Tst_DB_UpdateQstScore (const struct TstPrn_PrintedQuestion *PrintedQuestion)
  {
   /***** Update number of clicks and score of the question *****/
   Str_SetDecimalPointToUS ();		// To print the floating point as a dot
   if (PrintedQuestion->StrAnswers[0])	// User's answer is not blank
      DB_QueryUPDATE ("can not update the score of a question",
		      "UPDATE tst_questions"
	                " SET NumHits=NumHits+1,"
	                     "NumHitsNotBlank=NumHitsNotBlank+1,"
	                     "Score=Score+(%.15lg)"
                      " WHERE QstCod=%ld",
		      PrintedQuestion->Score,
		      PrintedQuestion->QstCod);
   else					// User's answer is blank
      DB_QueryUPDATE ("can not update the score of a question",
		      "UPDATE tst_questions"
	                " SET NumHits=NumHits+1"
                      " WHERE QstCod=%ld",
		      PrintedQuestion->QstCod);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/************** Get questions for a new test from the database ***************/
/*****************************************************************************/

#define Tst_MAX_BYTES_QUERY_QUESTIONS (16 * 1024 - 1)

unsigned Tst_DB_GetQuestionsForNewTest (MYSQL_RES **mysql_res,
                                        const struct Qst_Questions *Questions)
  {
   extern const char *Qst_DB_StrAnswerTypes[Qst_NUM_ANS_TYPES];
   char *Query = NULL;
   long LengthQuery;
   unsigned NumItemInList;
   const char *Ptr;
   char TagText[Tag_MAX_BYTES_TAG + 1];
   char UnsignedStr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   Qst_AnswerType_t AnswerType;
   char StrNumQsts[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Allocate space for query *****/
   if ((Query = malloc (Tst_MAX_BYTES_QUERY_QUESTIONS + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Select questions without hidden tags *****/
   /* Begin query */
   // Reject questions with any tag hidden
   // Select only questions with tags
   // DISTINCTROW is necessary to not repeat questions
   snprintf (Query,Tst_MAX_BYTES_QUERY_QUESTIONS + 1,
	     "SELECT DISTINCTROW tst_questions.QstCod,"		// row[0]
                                "tst_questions.AnsType,"	// row[1]
                                "tst_questions.Shuffle"		// row[2]
	      " FROM tst_questions,tst_question_tags,tst_tags"
	     " WHERE tst_questions.CrsCod=%ld"
	       " AND tst_questions.QstCod NOT IN"
		   " (SELECT tst_question_tags.QstCod"
		      " FROM tst_tags,tst_question_tags"
		     " WHERE tst_tags.CrsCod=%ld"
		       " AND tst_tags.TagHidden='Y'"
		       " AND tst_tags.TagCod=tst_question_tags.TagCod)"
	       " AND tst_questions.QstCod=tst_question_tags.QstCod"
	       " AND tst_question_tags.TagCod=tst_tags.TagCod"
	       " AND tst_tags.CrsCod=%ld",
	     Gbl.Hierarchy.Crs.CrsCod,
	     Gbl.Hierarchy.Crs.CrsCod,
	     Gbl.Hierarchy.Crs.CrsCod);

   if (!Questions->Tags.All) // User has not selected all the tags
     {
      /* Add selected tags */
      LengthQuery = strlen (Query);
      NumItemInList = 0;
      Ptr = Questions->Tags.List;
      while (*Ptr)
        {
         Par_GetNextStrUntilSeparParamMult (&Ptr,TagText,Tag_MAX_BYTES_TAG);
         LengthQuery = LengthQuery + 35 + strlen (TagText) + 1;
         if (LengthQuery > Tst_MAX_BYTES_QUERY_QUESTIONS - 128)
            Err_QuerySizeExceededExit ();
         Str_Concat (Query,
                     NumItemInList ? " OR tst_tags.TagTxt='" :
                                     " AND (tst_tags.TagTxt='",
                     Tst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,TagText,Tst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,"'",Tst_MAX_BYTES_QUERY_QUESTIONS);
         NumItemInList++;
        }
      Str_Concat (Query,")",Tst_MAX_BYTES_QUERY_QUESTIONS);
     }

   /* Add answer types selected */
   if (!Questions->AnswerTypes.All)
     {
      LengthQuery = strlen (Query);
      NumItemInList = 0;
      Ptr = Questions->AnswerTypes.List;
      while (*Ptr)
        {
         Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,Tag_MAX_BYTES_TAG);
	 AnswerType = Qst_ConvertFromUnsignedStrToAnsTyp (UnsignedStr);
         LengthQuery = LengthQuery + 35 + strlen (Qst_DB_StrAnswerTypes[AnswerType]) + 1;
         if (LengthQuery > Tst_MAX_BYTES_QUERY_QUESTIONS - 128)
            Err_QuerySizeExceededExit ();
         Str_Concat (Query,
                     NumItemInList ? " OR tst_questions.AnsType='" :
                                     " AND (tst_questions.AnsType='",
                     Tst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,Qst_DB_StrAnswerTypes[AnswerType],Tst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,"'",Tst_MAX_BYTES_QUERY_QUESTIONS);
         NumItemInList++;
        }
      Str_Concat (Query,")",Tst_MAX_BYTES_QUERY_QUESTIONS);
     }

   /* End query */
   Str_Concat (Query," ORDER BY RAND() LIMIT ",Tst_MAX_BYTES_QUERY_QUESTIONS);
   snprintf (StrNumQsts,sizeof (StrNumQsts),"%u",Questions->NumQsts);
   Str_Concat (Query,StrNumQsts,Tst_MAX_BYTES_QUERY_QUESTIONS);
/*
   if (Gbl.Usrs.Me.Roles.LoggedRole == Rol_SYS_ADM)
      Lay_ShowAlert (Lay_INFO,Query);
*/
   /* Make the query */
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get questions",
		   "%s",
		   Query);
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
		    Gbl.Hierarchy.Crs.CrsCod,
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
		   Gbl.Hierarchy.Crs.CrsCod);
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
				Gbl.Hierarchy.Crs.CrsCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				NumQsts);
  }

/*****************************************************************************/
/*********************** Update test print in database ***********************/
/*****************************************************************************/

void Tst_DB_UpdatePrint (const struct TstPrn_Print *Print)
  {
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
		   Print->AllowTeachers ? 'Y' :
			                  'N',
		   Print->Score,
		   Print->PrnCod,
		   Gbl.Hierarchy.Crs.CrsCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }
