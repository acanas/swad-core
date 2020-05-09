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
static void ExaPrn_ResetPrintExceptPrnCod (struct ExaPrn_Print *Print);

static bool ExaPrn_CheckIfMyPrintExists (const struct ExaEvt_Event *Event);
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
static void ExaPrn_ShowExamPrintToFillIt (struct Exa_Exam *Exam,
                                          struct ExaPrn_Print *Print);

static void ExaPrn_PutParamPrnCod (long ExaCod);
static long ExaPrn_GetParamPrnCod (void);

/*****************************************************************************/
/**************************** Reset exam print *******************************/
/*****************************************************************************/

static void ExaPrn_ResetPrint (struct ExaPrn_Print *Print)
  {
   Print->PrnCod = -1L;
   ExaPrn_ResetPrintExceptPrnCod (Print);
  }

static void ExaPrn_ResetPrintExceptPrnCod (struct ExaPrn_Print *Print)
  {
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
   extern const char *Hlp_ASSESSMENT_Exams;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;
   struct ExaPrn_Print Print;
   bool PrintExists;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaEvt_ResetEvent (&Event);
   ExaPrn_ResetPrint (&Print);

   /***** Get and check parameters *****/
   ExaEvt_GetAndCheckParameters (&Exams,&Exam,&Event);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Exam.Title,
		 NULL,NULL,
		 Hlp_ASSESSMENT_Exams,Box_NOT_CLOSABLE);
   Lay_WriteHeaderClassPhoto (false,false,
			      Gbl.Hierarchy.Ins.InsCod,
			      Gbl.Hierarchy.Deg.DegCod,
			      Gbl.Hierarchy.Crs.CrsCod);

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (10);

   /***** Check if already exists exam in database *****/
   PrintExists = ExaPrn_CheckIfMyPrintExists (&Event);

   if (PrintExists)
     {
      Ale_ShowAlert (Ale_INFO,"El examen ya existe.");
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

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********* Check if my exam print associated to a given event exists *********/
/*****************************************************************************/
// Return true if print exists

static bool ExaPrn_CheckIfMyPrintExists (const struct ExaEvt_Event *Event)
  {
   return (DB_QueryCOUNT ("can not check if exam print exists",
			  "SELECT COUNT(*) FROM exa_prints"
			  " WHERE EvtCod=%ld AND UsrCod=%ld",
			  Event->EvtCod,Gbl.Usrs.Me.UsrDat.UsrCod) != 0);
  }

/*****************************************************************************/
/*********** Get questions for a new exam print from the database ************/
/*****************************************************************************/

static void ExaPrn_GetQuestionsForNewPrintFromDB (struct Exa_Exam *Exam,
	                                          struct ExaPrn_Print *Print)
  {
   extern const char *Txt_question;
   extern const char *Txt_questions;
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

	 /***** Title for this set *****/
	 /* Begin title for this set */
	 HTM_TR_Begin (NULL);
	 HTM_TD_Begin ("colspan=\"2\"");
         HTM_TABLE_BeginWide ();

	 /* Title */
	 HTM_TD_Begin ("class=\"EXA_SET_TITLE\"");
	 HTM_Txt (Set.Title);
	 HTM_TD_End ();

	 /* Number of questions to appear in exam print */
	 HTM_TD_Begin ("class=\"EXA_SET_NUM_QSTS\"");
	 HTM_Unsigned (Set.NumQstsToPrint);
	 HTM_NBSP ();
	 HTM_Txt (Set.NumQstsToPrint == 1 ? Txt_question :
		                            Txt_questions);
	 HTM_TD_End ();

	 /* End title for this set */
	 HTM_TABLE_End ();
	 HTM_TD_End ();
	 HTM_TR_End ();

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

      Ale_ShowAlert (Ale_INFO,"DEBUG: ExaPrn_GetSomeQstsFromSetToPrint Print->PrintedQuestions[*NumQstInPrint].SetCod = %ld",
                     Print->PrintedQuestions[*NumQstInPrint].SetCod);

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

      /* Begin row for this question */
      HTM_TR_Begin (NULL);

      /* Title */
      HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TxtF ("Pregunta %ld",Print->PrintedQuestions[*NumQstInPrint].QstCod);
      HTM_TD_End ();

      /* Number of questions to appear in exam print */
      HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_Txt ("Enunciado y respuestas");
      HTM_TD_End ();

      /* End title for this question */
      HTM_TR_End ();
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

      Ale_ShowAlert (Ale_INFO,"DEBUG: ExaPrn_StoreOneQstOfPrintInDB Print->PrintedQuestions[NumQst].SetCod = %ld",
                     Print->PrintedQuestions[NumQst].SetCod);

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
	 Tst_WriteQstAndAnsSeeing (&Print->PrintedQuestions[NumQst],NumQst,&Question);

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

static long ExaPrn_GetParamPrnCod (void)
  {
   /***** Get code of exam print *****/
   return Par_GetParToLong ("PrnCod");
  }
