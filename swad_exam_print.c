// swad_exam_print.c: exam prints (each copy of an exam in an event for a student)

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

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_database.h"
#include "swad_exam.h"
#include "swad_exam_event.h"
#include "swad_exam_result.h"
#include "swad_exam_set.h"
#include "swad_exam_type.h"
#include "swad_form.h"
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define ExaPrn_MAX_QUESTIONS_PER_EXAM_PRINT	100	// Absolute maximum number of questions in an exam print

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct ExaPrn_Print
  {
   long PrnCod;			// Exam print code
   long EvtCod;			// Event code associated to this print
   long UsrCod;			// User who answered the exam print
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   unsigned NumQsts;		// Number of questions
   unsigned NumQstsNotBlank;	// Number of questions not blank
   bool Sent;			// This exam print has been sent or not?
				// "Sent" means that user has clicked "Send" button after finishing
   double Score;		// Total score of the exam print
   struct TstPrn_PrintedQuestion PrintedQuestions[ExaPrn_MAX_QUESTIONS_PER_EXAM_PRINT];
  };

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaPrn_ResetPrint (struct ExaPrn_Print *Print);
static void ExaPrn_ResetPrintExceptEvtCodAndUsrCod (struct ExaPrn_Print *Print);

static void ExaPrn_GetPrintDataByEvtCodAndUsrCod (struct ExaPrn_Print *Print);
static void ExaPrn_GetQuestionsForNewPrintFromDB (struct Exa_Exam *Exam,
	                                          struct ExaPrn_Print *Print);
static unsigned ExaPrn_GetSomeQstsFromSetToPrint (struct ExaPrn_Print *Print,
                                                  struct ExaSet_Set *Set,
                                                  unsigned *NumQstInPrint);
static void ExaPrn_CreatePrintInDB (const struct ExaEvt_Event *Event,
				    struct ExaPrn_Print *Print);
static void ExaPrn_ComputeScoresAndStoreQuestionsOfPrint (struct ExaPrn_Print *Print,
                                                          bool UpdateQstScore);
static void ExaPrn_StoreOneQstOfPrintInDB (const struct ExaPrn_Print *Print,
                                           unsigned NumQst);

static void ExaPrn_GetPrintQuestionsFromDB (struct ExaPrn_Print *Print);

static void ExaPrn_ShowExamPrintToFillIt (struct Exa_Exam *Exam,
                                          struct ExaPrn_Print *Print);
static void ExaPrn_WriteQstAndAnsToFill (const struct Exa_Exam *Exam,
                                         struct TstPrn_PrintedQuestion *PrintedQuestion,
                                         unsigned NumQst,
                                         const struct Tst_Question *Question);
static void ExaPrn_WriteAnswersToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                       unsigned NumQst,
                                       const struct Tst_Question *Question);
static void ExaPrn_WriteIntAnsSeeing (const struct TstPrn_PrintedQuestion *PrintedQuestion,
				      unsigned NumQst);
static void ExaPrn_WriteFloatAnsSeeing (const struct TstPrn_PrintedQuestion *PrintedQuestion,
				        unsigned NumQst);
static void ExaPrn_WriteTFAnsSeeing (const struct TstPrn_PrintedQuestion *PrintedQuestion,
	                             unsigned NumQst);
static void ExaPrn_WriteChoiceAnsSeeing (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                         unsigned NumQst,
                                         const struct Tst_Question *Question);
static void ExaPrn_WriteTextAnsSeeing (const struct TstPrn_PrintedQuestion *PrintedQuestion,
	                               unsigned NumQst);

static void ExaPrn_PutParamPrnCod (long ExaCod);
// static long ExaPrn_GetParamPrnCod (void);

/*****************************************************************************/
/**************************** Reset exam print *******************************/
/*****************************************************************************/

static void ExaPrn_ResetPrint (struct ExaPrn_Print *Print)
  {
   Print->EvtCod = -1L;
   Print->UsrCod = -1L;
   ExaPrn_ResetPrintExceptEvtCodAndUsrCod (Print);
  }

static void ExaPrn_ResetPrintExceptEvtCodAndUsrCod (struct ExaPrn_Print *Print)
  {
   Print->PrnCod                  = -1L;
   Print->TimeUTC[Dat_START_TIME] =
   Print->TimeUTC[Dat_END_TIME  ] = (time_t) 0;
   Print->NumQsts                 =
   Print->NumQstsNotBlank         = 0;
   Print->Sent                    = false;	// After creating an exam print, it's not sent
   Print->Score                   = 0.0;
  }

/*****************************************************************************/
/********************** Show print of an exam in an event ********************/
/*****************************************************************************/

void ExaPrn_ShowExamPrint (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;
   struct ExaPrn_Print Print;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaEvt_ResetEvent (&Event);
   ExaPrn_ResetPrint (&Print);

   /***** Get and check parameters *****/
   ExaEvt_GetAndCheckParameters (&Exams,&Exam,&Event);

   /***** Get print data from database *****/
   Print.EvtCod = Event.EvtCod;
   Print.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   ExaPrn_GetPrintDataByEvtCodAndUsrCod (&Print);

   if (Print.PrnCod > 0)	// Print exists
     {
      Ale_ShowAlert (Ale_INFO,"El examen ya existe.");

      /***** Get questions and answers from database *****/
      ExaPrn_GetPrintQuestionsFromDB (&Print);
     }
   else
     {
      /***** Get questions from database *****/
      ExaPrn_GetQuestionsForNewPrintFromDB (&Exam,&Print);

      if (Print.NumQsts)
	{
	 /***** Create/update new exam print in database *****/
	 ExaPrn_CreatePrintInDB (&Event,&Print);
	 ExaPrn_ComputeScoresAndStoreQuestionsOfPrint (&Print,
						       false);	// Don't update question score
	}
      }

   /***** Show test exam to be answered *****/
   ExaPrn_ShowExamPrintToFillIt (&Exam,&Print);
  }

/*****************************************************************************/
/********* Get data of an exam print using event code and user code **********/
/*****************************************************************************/

static void ExaPrn_GetPrintDataByEvtCodAndUsrCod (struct ExaPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Make database query *****/
   if (DB_QuerySELECT (&mysql_res,"can not get data of an exam print",
		       "SELECT PrnCod,"				// row[0]
			      "UNIX_TIMESTAMP(StartTime),"	// row[1]
			      "UNIX_TIMESTAMP(EndTime),"	// row[2]
		              "NumQsts,"			// row[3]
		              "NumQstsNotBlank,"		// row[4]
			      "Sent,"				// row[5]
		              "Score"				// row[6]
		       " FROM exa_prints"
	               " WHERE EvtCod=%ld AND UsrCod=%ld",
		       Print->EvtCod,Print->UsrCod) == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get print code (row[0]) */
      Print->PrnCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get date-time (row[1] and row[2] hold UTC date-time) */
      Print->TimeUTC[Dat_START_TIME] = Dat_GetUNIXTimeFromStr (row[1]);
      Print->TimeUTC[Dat_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[2]);

      /* Get number of questions (row[3]) */
      if (sscanf (row[3],"%u",&Print->NumQsts) != 1)
	 Print->NumQsts = 0;

      /* Get number of questions not blank (row[4]) */
      if (sscanf (row[4],"%u",&Print->NumQstsNotBlank) != 1)
	 Print->NumQstsNotBlank = 0;

      /* Get if exam has been sent (row[5]) */
      Print->Sent = (row[5][0] == 'Y');

      /* Get score (row[6]) */
      Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
      if (sscanf (row[6],"%lf",&Print->Score) != 1)
	 Print->Score = 0.0;
      Str_SetDecimalPointToLocal ();	// Return to local system
     }
   else
      ExaPrn_ResetPrintExceptEvtCodAndUsrCod (Print);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*********** Get questions for a new exam print from the database ************/
/*****************************************************************************/

static void ExaPrn_GetQuestionsForNewPrintFromDB (struct Exa_Exam *Exam,
	                                          struct ExaPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumSets;
   unsigned NumSet;
   struct ExaSet_Set Set;
   unsigned NumQstsFromSet;
   unsigned NumQstInPrint = 0;

   /***** Get data of set of questions from database *****/
   NumSets = (unsigned)
	     DB_QuerySELECT (&mysql_res,"can not get sets of questions",
			      "SELECT SetCod,"		// row[0]
				     "NumQstsToPrint,"	// row[1]
				     "Title"		// row[2]
			      " FROM exa_sets"
			      " WHERE ExaCod=%ld"
			      " ORDER BY SetInd",
			      Exam->ExaCod);

   /***** Get questions from all sets *****/
   Print->NumQsts = 0;
   if (NumSets)
      /***** For each set in exam... *****/
      for (NumSet = 0;
	   NumSet < NumSets;
	   NumSet++)
	{
	 /***** Create set of questions *****/
	 ExaSet_ResetSet (&Set);

	 /***** Get set data *****/
	 row = mysql_fetch_row (mysql_res);
	 /*
	 row[0] SetCod
	 row[1] NumQstsToPrint
	 row[2] Title
	 */
	 /* Get set code (row[0]) */
	 Set.SetCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get set index (row[1]) */
	 Set.NumQstsToPrint = Str_ConvertStrToUnsigned (row[1]);

	 /* Get the title of the set (row[2]) */
	 Str_Copy (Set.Title,row[2],
		   ExaSet_MAX_BYTES_TITLE);

	 /***** Questions in this set *****/
	 NumQstsFromSet = ExaPrn_GetSomeQstsFromSetToPrint (Print,&Set,&NumQstInPrint);
	 Print->NumQsts += NumQstsFromSet;
	}

   /***** Check *****/
   if (Print->NumQsts != NumQstInPrint)
      Lay_ShowErrorAndExit ("Wrong number of questions.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Show questions from a set **************************/
/*****************************************************************************/

static unsigned ExaPrn_GetSomeQstsFromSetToPrint (struct ExaPrn_Print *Print,
                                                  struct ExaSet_Set *Set,
                                                  unsigned *NumQstInPrint)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQstsInSet;
   unsigned NumQstInSet;
   Tst_AnswerType_t AnswerType;
   bool Shuffle;

   /***** Get questions from database *****/
   NumQstsInSet = (unsigned)
		  DB_QuerySELECT (&mysql_res,"can not get questions from set",
				  "SELECT tst_questions.QstCod,"	// row[0]
					 "tst_questions.AnsType,"	// row[1]
					 "tst_questions.Shuffle"	// row[2]
				  " FROM exa_questions,tst_questions"
				  " WHERE exa_questions.setCod=%ld"
				  " AND exa_questions.QstCod=tst_questions.QstCod"
				  " ORDER BY RAND()"	// Don't use RAND(NOW()) because the same ordering will be repeated across sets
				  " LIMIT %u",
				  Set->SetCod,
				  Set->NumQstsToPrint);

   /***** Questions in this set *****/
   for (NumQstInSet = 0;
	NumQstInSet < NumQstsInSet;
	NumQstInSet++, (*NumQstInPrint)++)
     {
      Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;

      /***** Get question data *****/
      row = mysql_fetch_row (mysql_res);
      /*
      row[0] QstCod
      row[1] AnsType
      row[2] Shuffle
      */

      /* Get question code (row[0]) */
      Print->PrintedQuestions[*NumQstInPrint].QstCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Set set of questions */
      Print->PrintedQuestions[*NumQstInPrint].SetCod = Set->SetCod;

      /* Get answer type (row[1]) */
      AnswerType = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);

      /* Get shuffle (row[2]) */
      Shuffle = (row[2][0] == 'Y');

      /* Set indexes of answers */
      switch (AnswerType)
	{
	 case Tst_ANS_INT:
	 case Tst_ANS_FLOAT:
	 case Tst_ANS_TRUE_FALSE:
	 case Tst_ANS_TEXT:
	    Print->PrintedQuestions[*NumQstInPrint].StrIndexes[0] = '\0';
	    break;
	 case Tst_ANS_UNIQUE_CHOICE:
	 case Tst_ANS_MULTIPLE_CHOICE:
            /* If answer type is unique or multiple option,
               generate indexes of answers depending on shuffle */
	    Tst_GenerateChoiceIndexesDependingOnShuffle (&Print->PrintedQuestions[*NumQstInPrint],Shuffle);
	    break;
	 default:
	    break;
	}

      /* Reset user's answers.
         Initially user has not answered the question ==> initially all the answers will be blank.
         If the user does not confirm the submission of their exam ==>
         ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
      Print->PrintedQuestions[*NumQstInPrint].StrAnswers[0] = '\0';
     }

   return NumQstsInSet;
  }

/*****************************************************************************/
/***************** Create new blank exam print in database *******************/
/*****************************************************************************/

static void ExaPrn_CreatePrintInDB (const struct ExaEvt_Event *Event,
				    struct ExaPrn_Print *Print)
  {
   /***** Insert new exam print into table *****/
   Print->PrnCod =
   DB_QueryINSERTandReturnCode ("can not create new exam print",
				"INSERT INTO exa_prints"
				" (EvtCod,UsrCod,StartTime,EndTime,NumQsts,NumQstsNotBlank,Sent,Score)"
				" VALUES"
				" (%ld,%ld,NOW(),NOW(),%u,0,'N',0)",
				Event->EvtCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Print->NumQsts);
  }

/*****************************************************************************/
/*********** Compute score of each question and store in database ************/
/*****************************************************************************/

static void ExaPrn_ComputeScoresAndStoreQuestionsOfPrint (struct ExaPrn_Print *Print,
                                                          bool UpdateQstScore)
  {
   unsigned NumQst;
   struct Tst_Question Question;

   /***** Initialize total score *****/
   Print->Score = 0.0;
   Print->NumQstsNotBlank = 0;

   /***** Compute and store scores of all questions *****/
   for (NumQst = 0;
	NumQst < Print->NumQsts;
	NumQst++)
     {
      /* Compute question score */
      Tst_QstConstructor (&Question);
      Question.QstCod = Print->PrintedQuestions[NumQst].QstCod;
      Question.Answer.Type = Tst_GetQstAnswerType (Question.QstCod);
      TstPrn_ComputeAnswerScore (&Print->PrintedQuestions[NumQst],&Question);
      Tst_QstDestructor (&Question);

      /* Store test exam question in database */
      ExaPrn_StoreOneQstOfPrintInDB (Print,
				     NumQst);	// 0, 1, 2, 3...

      /* Accumulate total score */
      Print->Score += Print->PrintedQuestions[NumQst].Score;
      if (Print->PrintedQuestions[NumQst].AnswerIsNotBlank)
	 Print->NumQstsNotBlank++;

      /* Update the number of hits and the score of this question in tests database */
      if (UpdateQstScore)
	 Tst_UpdateQstScoreInDB (&Print->PrintedQuestions[NumQst]);
     }
  }


/*****************************************************************************/
/************* Store user's answers of an test exam into database ************/
/*****************************************************************************/

static void ExaPrn_StoreOneQstOfPrintInDB (const struct ExaPrn_Print *Print,
                                           unsigned NumQst)
  {
   char StrIndexes[Tst_MAX_BYTES_INDEXES_ONE_QST + 1];
   char StrAnswers[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];

   /***** Replace each separator of multiple parameters by a comma *****/
   /* In database commas are used as separators instead of special chars */
   Par_ReplaceSeparatorMultipleByComma (Print->PrintedQuestions[NumQst].StrIndexes,StrIndexes);
   Par_ReplaceSeparatorMultipleByComma (Print->PrintedQuestions[NumQst].StrAnswers,StrAnswers);

   /***** Insert question and user's answers into database *****/
   Str_SetDecimalPointToUS ();	// To print the floating point as a dot
   DB_QueryREPLACE ("can not update a question in an exam print",
		    "REPLACE INTO exa_print_questions"
		    " (PrnCod,QstCod,QstInd,SetCod,Score,Indexes,Answers)"
		    " VALUES"
		    " (%ld,%ld,%u,%ld,'%.15lg','%s','%s')",
		    Print->PrnCod,Print->PrintedQuestions[NumQst].QstCod,
		    NumQst,	// 0, 1, 2, 3...
		    Print->PrintedQuestions[NumQst].SetCod,
		    Print->PrintedQuestions[NumQst].Score,
		    StrIndexes,
		    StrAnswers);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/************* Get the questions of an exam print from database **************/
/*****************************************************************************/

static void ExaPrn_GetPrintQuestionsFromDB (struct ExaPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQsts;
   unsigned NumQst;
   Tst_AnswerType_t AnswerType;

   /***** Get questions of an exam print from database *****/
   NumQsts =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get questions"
					 " of an exam print",
			      "SELECT exa_print_questions.QstCod,"	// row[0]
			             "exa_print_questions.SetCod,"	// row[1]
				     "tst_questions.AnsType,"		// row[2]
			             "exa_print_questions.Indexes,"	// row[3]
			             "exa_print_questions.Answers"	// row[4]
			      " FROM exa_print_questions,tst_questions"
			      " WHERE exa_print_questions.PrnCod=%ld"
			      " AND exa_print_questions.QstCod=tst_questions.QstCod"
			      " ORDER BY exa_print_questions.QstInd",
			      Print->PrnCod);

   /***** Get questions *****/
   // The number of questions in table of print questions
   // should match the number of questions got from print
   if (NumQsts == Print->NumQsts)
      for (NumQst = 0;
	   NumQst < NumQsts;
	   NumQst++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get question code (row[0]) */
	 if ((Print->PrintedQuestions[NumQst].QstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of question.");

	 /* Get set code (row[1]) */
	 if ((Print->PrintedQuestions[NumQst].SetCod = Str_ConvertStrCodToLongCod (row[1])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of set.");

	 /* Get answer type (row[2]) */
         AnswerType = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[2]);

	 /* Get indexes for this question (row[3]) */
	 Str_Copy (Print->PrintedQuestions[NumQst].StrIndexes,row[3],
		   Tst_MAX_BYTES_INDEXES_ONE_QST);

	 /* Get answers selected by user for this question (row[4]) */
	 Str_Copy (Print->PrintedQuestions[NumQst].StrAnswers,row[4],
		   Tst_MAX_BYTES_ANSWERS_ONE_QST);

	 /* Replace each comma by a separator of multiple parameters */
	 /* In database commas are used as separators instead of special chars */
	 Par_ReplaceCommaBySeparatorMultiple (Print->PrintedQuestions[NumQst].StrIndexes);
	 if (AnswerType == Tst_ANS_MULTIPLE_CHOICE)
	    // Only multiple choice questions have multiple answers separated by commas
	    // Other types of questions have a unique answer, and comma may be part of that answer
	    Par_ReplaceCommaBySeparatorMultiple (Print->PrintedQuestions[NumQst].StrAnswers);
	}

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (NumQsts != Print->NumQsts)
      Lay_WrongExamExit ();
  }

/*****************************************************************************/
/****************** Show a test exam print to be answered ********************/
/*****************************************************************************/

static void ExaPrn_ShowExamPrintToFillIt (struct Exa_Exam *Exam,
                                          struct ExaPrn_Print *Print)
  {
   extern const char *Hlp_ASSESSMENT_Exams;
   extern const char *Txt_Send;
   unsigned NumQst;
   struct Tst_Question Question;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Exam->Title,
		 NULL,NULL,
		 Hlp_ASSESSMENT_Exams,Box_NOT_CLOSABLE);
   Lay_WriteHeaderClassPhoto (false,false,
			      Gbl.Hierarchy.Ins.InsCod,
			      Gbl.Hierarchy.Deg.DegCod,
			      Gbl.Hierarchy.Crs.CrsCod);

   if (Print->NumQsts)
     {
      /***** Begin form *****/
      Frm_StartForm (ActReqAssExaPrn);
      ExaPrn_PutParamPrnCod (Print->PrnCod);

      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (10);

      /***** Write one row for each question *****/
      for (NumQst = 0;
	   NumQst < Print->NumQsts;
	   NumQst++)
	{
	 Gbl.RowEvenOdd = NumQst % 2;

	 /* Create test question */
	 Tst_QstConstructor (&Question);
	 Question.QstCod = Print->PrintedQuestions[NumQst].QstCod;

	 /* Show question */
	 if (!Tst_GetQstDataFromDB (&Question))	// Question exists
	    Lay_ShowErrorAndExit ("Wrong question.");

	 /* Write question and answers */
	 ExaPrn_WriteQstAndAnsToFill (Exam,&Print->PrintedQuestions[NumQst],NumQst,&Question);

	 /* Destroy test question */
	 Tst_QstDestructor (&Question);
	}

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Send buttona and end form *****/
      Btn_PutCreateButton (Txt_Send);
      Frm_EndForm ();
     }

   /***** End box *****/
   Box_BoxEnd ();
  }


/*****************************************************************************/
/********** Write a row of a test, with one question and its answer **********/
/*****************************************************************************/

static void ExaPrn_WriteQstAndAnsToFill (const struct Exa_Exam *Exam,
                                         struct TstPrn_PrintedQuestion *PrintedQuestion,
                                         unsigned NumQst,
                                         const struct Tst_Question *Question)
  {
   static struct ExaSet_Set CurrentSet =
     {
      .ExaCod = -1L,
      .SetCod = -1L,
      .SetInd = 0,
      .NumQstsToPrint = 0,
      .Title[0] = '\0'
     };

   if (PrintedQuestion->SetCod != CurrentSet.SetCod)
     {
      /***** Get data of this set *****/
      CurrentSet.ExaCod = Exam->ExaCod;
      CurrentSet.SetCod = PrintedQuestion->SetCod;
      ExaSet_GetDataOfSetByCod (&CurrentSet);

      /***** Title for this set *****/
      HTM_TR_Begin (NULL);
      HTM_TD_Begin ("colspan=\"2\"");
      ExaSet_WriteSetTitle (&CurrentSet);
      HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

   /***** Number of question and answer type *****/
   HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
   Tst_WriteNumQst (NumQst + 1);
   Tst_WriteAnswerType (Question->Answer.Type);
   HTM_TD_End ();

   /***** Stem, media and answers *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);

   /* Write parameter with question code */
   Tst_WriteParamQstCod (NumQst,Question->QstCod);

   /* Stem */
   Tst_WriteQstStem (Question->Stem,"TEST_EXA",true);

   /* Media */
   Med_ShowMedia (&Question->Media,
		  "TEST_MED_SHOW_CONT",
		  "TEST_MED_SHOW");

   /* Answers */
   ExaPrn_WriteAnswersToFill (PrintedQuestion,NumQst,Question);

   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Write answers of a question to fill them ******************/
/*****************************************************************************/

static void ExaPrn_WriteAnswersToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                       unsigned NumQst,
                                       const struct Tst_Question *Question)
  {
   /***** Write answer depending on type *****/
   switch (Question->Answer.Type)
     {
      case Tst_ANS_INT:
         ExaPrn_WriteIntAnsSeeing (PrintedQuestion,NumQst);
         break;
      case Tst_ANS_FLOAT:
         ExaPrn_WriteFloatAnsSeeing (PrintedQuestion,NumQst);
         break;
      case Tst_ANS_TRUE_FALSE:
         ExaPrn_WriteTFAnsSeeing (PrintedQuestion,NumQst);
         break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
         ExaPrn_WriteChoiceAnsSeeing (PrintedQuestion,NumQst,Question);
         break;
      case Tst_ANS_TEXT:
         ExaPrn_WriteTextAnsSeeing (PrintedQuestion,NumQst);
         break;
      default:
         break;
     }
  }

/*****************************************************************************/
/****************** Write integer answer when seeing a test ******************/
/*****************************************************************************/

static void ExaPrn_WriteIntAnsSeeing (const struct TstPrn_PrintedQuestion *PrintedQuestion,
				      unsigned NumQst)
  {
   char StrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (StrAns,sizeof (StrAns),
	     "Ans%010u",
	     NumQst);
   HTM_INPUT_TEXT (StrAns,11,PrintedQuestion->StrAnswers,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"11\"");
  }

/*****************************************************************************/
/****************** Write float answer when seeing a test ********************/
/*****************************************************************************/

static void ExaPrn_WriteFloatAnsSeeing (const struct TstPrn_PrintedQuestion *PrintedQuestion,
				        unsigned NumQst)
  {
   char StrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (StrAns,sizeof (StrAns),
	     "Ans%010u",
	     NumQst);
   HTM_INPUT_TEXT (StrAns,Tst_MAX_BYTES_FLOAT_ANSWER,PrintedQuestion->StrAnswers,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"11\"");
  }

/*****************************************************************************/
/************** Write false / true answer when seeing a test ****************/
/*****************************************************************************/

static void ExaPrn_WriteTFAnsSeeing (const struct TstPrn_PrintedQuestion *PrintedQuestion,
	                             unsigned NumQst)
  {
   extern const char *Txt_TF_QST[2];

   /***** Write selector for the answer *****/
   /* Initially user has not answered the question ==> initially all the answers will be blank.
      If the user does not confirm the submission of their exam ==>
      ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "name=\"Ans%010u\"",NumQst);
   HTM_OPTION (HTM_Type_STRING,"" ,PrintedQuestion->StrAnswers[0] == '\0',false,"&nbsp;");
   HTM_OPTION (HTM_Type_STRING,"T",PrintedQuestion->StrAnswers[0] == 'T' ,false,"%s",Txt_TF_QST[0]);
   HTM_OPTION (HTM_Type_STRING,"F",PrintedQuestion->StrAnswers[0] == 'F' ,false,"%s",Txt_TF_QST[1]);
   HTM_SELECT_End ();
  }

/*****************************************************************************/
/******** Write single or multiple choice answer when seeing a test **********/
/*****************************************************************************/

static void ExaPrn_WriteChoiceAnsSeeing (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                         unsigned NumQst,
                                         const struct Tst_Question *Question)
  {
   unsigned NumOpt;
   unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   bool UsrAnswers[Tst_MAX_OPTIONS_PER_QUESTION];
   char StrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Get indexes for this question from string *****/
   TstPrn_GetIndexesFromStr (PrintedQuestion->StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   TstPrn_GetAnswersFromStr (PrintedQuestion->StrAnswers,UsrAnswers);

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      /***** Indexes are 0 1 2 3... if no shuffle
             or 3 1 0 2... (example) if shuffle *****/
      HTM_TR_Begin (NULL);

      /***** Write selectors and letter of this option *****/
      /* Initially user has not answered the question ==> initially all the answers will be blank.
	 If the user does not confirm the submission of their exam ==>
	 ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
      HTM_TD_Begin ("class=\"LT\"");

      snprintf (StrAns,sizeof (StrAns),
		"Ans%010u",
		NumQst);
      if (Question->Answer.Type == Tst_ANS_UNIQUE_CHOICE)
	 HTM_INPUT_RADIO (StrAns,false,
			  "id=\"Ans%010u_%u\" value=\"%u\"%s"
			  " onclick=\"selectUnselectRadio(this,this.form.Ans%010u,%u);\"",
			  NumQst,NumOpt,
			  Indexes[NumOpt],
			  UsrAnswers[Indexes[NumOpt]] ? " checked=\"checked\"" :
				                           "",
                          NumQst,Question->Answer.NumOptions);
      else // Answer.Type == Tst_ANS_MULTIPLE_CHOICE
	 HTM_INPUT_CHECKBOX (StrAns,HTM_DONT_SUBMIT_ON_CHANGE,
			     "id=\"Ans%010u_%u\" value=\"%u\"%s",
			     NumQst,NumOpt,
			     Indexes[NumOpt],
			     UsrAnswers[Indexes[NumOpt]] ? " checked=\"checked\"" :
				                           "");

      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LT\"");
      HTM_LABEL_Begin ("for=\"Ans%010u_%u\" class=\"ANS_TXT\"",NumQst,NumOpt);
      HTM_TxtF ("%c)&nbsp;",'a' + (char) NumOpt);
      HTM_LABEL_End ();
      HTM_TD_End ();

      /***** Write the option text *****/
      HTM_TD_Begin ("class=\"LT\"");
      HTM_LABEL_Begin ("for=\"Ans%010u_%u\" class=\"ANS_TXT\"",NumQst,NumOpt);
      HTM_Txt (Question->Answer.Options[Indexes[NumOpt]].Text);
      HTM_LABEL_End ();
      Med_ShowMedia (&Question->Answer.Options[Indexes[NumOpt]].Media,
                     "TEST_MED_SHOW_CONT",
                     "TEST_MED_SHOW");
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************** Write text answer when seeing a test *******************/
/*****************************************************************************/

static void ExaPrn_WriteTextAnsSeeing (const struct TstPrn_PrintedQuestion *PrintedQuestion,
	                               unsigned NumQst)
  {
   char StrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (StrAns,sizeof (StrAns),
	     "Ans%010u",
	     NumQst);
   HTM_INPUT_TEXT (StrAns,Tst_MAX_CHARS_ANSWERS_ONE_QST,PrintedQuestion->StrAnswers,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"40\"");
  }

/*****************************************************************************/
/********************** Receive answer to an exam print **********************/
/*****************************************************************************/

void ExaPrn_ReceivePrintAnswer (void)
  {
   Ale_ShowAlert (Ale_INFO,"Recepci&oacute;n del examen contestado.");
  }

/*****************************************************************************/
/***************** Write parameter with code of exam print *******************/
/*****************************************************************************/

static void ExaPrn_PutParamPrnCod (long ExaCod)
  {
   Par_PutHiddenParamLong (NULL,"PrnCod",ExaCod);
  }

/*****************************************************************************/
/***************** Get parameter with code of exam print *********************/
/*****************************************************************************/

// static long ExaPrn_GetParamPrnCod (void)
//   {
//    /***** Get code of exam print *****/
//    return Par_GetParToLong ("PrnCod");
//   }
