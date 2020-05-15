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
#include "swad_exam_print.h"
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
   // long ExaCod;			// Exam code
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
static void ExaPrn_GetQuestionsForNewPrintFromDB (struct ExaPrn_Print *Print,long ExaCod);
static unsigned ExaPrn_GetSomeQstsFromSetToPrint (struct ExaPrn_Print *Print,
                                                  struct ExaSet_Set *Set,
                                                  unsigned *NumQstInPrint);
static void ExaPrn_GenerateChoiceIndexes (struct TstPrn_PrintedQuestion *PrintedQuestion,
					  bool Shuffle);
static void ExaPrn_CreatePrintInDB (struct ExaPrn_Print *Print);

static void ExaPrn_GetPrintQuestionsFromDB (struct ExaPrn_Print *Print);

static void ExaPrn_ShowExamPrintToFillIt (const char *Title,
                                          struct ExaPrn_Print *Print);
static void ExaPrn_ShowTableWithQstsToFill (struct ExaPrn_Print *Print);
static void ExaPrn_WriteQstAndAnsToFill (struct ExaPrn_Print *Print,
                                         unsigned NumQst,
                                         const struct Tst_Question *Question);
static void ExaPrn_WriteAnswersToFill (struct ExaPrn_Print *Print,
                                       unsigned NumQst,
                                       const struct Tst_Question *Question);
static void ExaPrn_WriteIntAnsToFill (const struct ExaPrn_Print *Print,
				      unsigned NumQst);
static void ExaPrn_WriteFloatAnsToFill (const struct ExaPrn_Print *Print,
				        unsigned NumQst);
static void ExaPrn_WriteTFAnsToFill (const struct ExaPrn_Print *Print,
	                             unsigned NumQst);
static void ExaPrn_WriteChoiceAnsToFill (struct ExaPrn_Print *Print,
                                         unsigned NumQst,
                                         const struct Tst_Question *Question);
static void ExaPrn_WriteTextAnsToFill (const struct ExaPrn_Print *Print,
	                               unsigned NumQst);

static unsigned ExaPrn_GetAnswerFromForm (struct ExaPrn_Print *Print);

// static void ExaPrn_PutParamNumQst (unsigned NumQst);
static unsigned ExaPrn_GetParamQstInd (void);

static void ExaPrn_ComputeScoresAndStoreQuestionsOfPrint (struct ExaPrn_Print *Print);
static void ExaPrn_ComputeScoreAndStoreQuestionOfPrint (struct ExaPrn_Print *Print,
                                                        unsigned NumQst);

//-----------------------------------------------------------------------------

static void ExaPrn_GetCorrectAndComputeIntAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question);
static void ExaPrn_GetCorrectAndComputeFltAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question);
static void ExaPrn_GetCorrectAndComputeTF_AnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question);
static void ExaPrn_GetCorrectAndComputeChoAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question);
static void ExaPrn_GetCorrectAndComputeTxtAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question);

static void ExaPrn_GetCorrectIntAnswerFromDB (struct Tst_Question *Question);
static void ExaPrn_GetCorrectFltAnswerFromDB (struct Tst_Question *Question);
static void ExaPrn_GetCorrectTF_AnswerFromDB (struct Tst_Question *Question);
static void ExaPrn_GetCorrectChoAnswerFromDB (struct Tst_Question *Question);
static void ExaPrn_GetCorrectTxtAnswerFromDB (struct Tst_Question *Question);

//-----------------------------------------------------------------------------

static void ExaPrn_GetAnswerFromDB (struct ExaPrn_Print *Print,long QstCod,
                                    char StrAnswers[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1]);
static void ExaPrn_StoreOneQstOfPrintInDB (const struct ExaPrn_Print *Print,
                                           unsigned NumQst);

static void ExaPrn_GetNumQstsNotBlank (struct ExaPrn_Print *Print);
static void ExaPrn_ComputeTotalScoreOfPrint (struct ExaPrn_Print *Print);
static void ExaPrn_UpdatePrintInDB (const struct ExaPrn_Print *Print);

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
   // Print->ExaCod		  = -1L;
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

   /***** Check if I can access to this event *****/
   if (ExaEvt_CheckIfICanAnswerThisEvent (&Event))
     {
      /***** Get print data from database *****/
      Print.EvtCod = Event.EvtCod;
      Print.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
      ExaPrn_GetPrintDataByEvtCodAndUsrCod (&Print);

      if (Print.PrnCod > 0)	// Print exists and I can access to it
	 /***** Get questions and answers from database *****/
	 ExaPrn_GetPrintQuestionsFromDB (&Print);
      else
	{
	 /***** Get questions from database *****/
	 ExaPrn_GetQuestionsForNewPrintFromDB (&Print,Exam.ExaCod);

	 if (Print.NumQsts)
	   {
	    /***** Create/update new exam print in database *****/
	    ExaPrn_CreatePrintInDB (&Print);
	    ExaPrn_ComputeScoresAndStoreQuestionsOfPrint (&Print);
	   }
	 }

      /***** Show test exam to be answered *****/
      ExaPrn_ShowExamPrintToFillIt (Exam.Title,&Print);
     }
   else
      Ale_ShowAlert (Ale_INFO,"Usted no tiene acceso al examen.");	// TODO: Need translation!!!!
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
	               " WHERE EvtCod=%ld"
                       " AND UsrCod=%ld",
		       Print->EvtCod,
		       Print->UsrCod) == 1)
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

static void ExaPrn_GetQuestionsForNewPrintFromDB (struct ExaPrn_Print *Print,long ExaCod)
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
			      ExaCod);

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
				  "SELECT QstCod,"	// row[0]
					 "AnsType,"	// row[1]
					 "Shuffle"	// row[2]
				  " FROM exa_set_questions"
				  " WHERE SetCod=%ld"
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
	    ExaPrn_GenerateChoiceIndexes (&Print->PrintedQuestions[*NumQstInPrint],Shuffle);
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
/*************** Generate choice indexes depending on shuffle ****************/
/*****************************************************************************/

static void ExaPrn_GenerateChoiceIndexes (struct TstPrn_PrintedQuestion *PrintedQuestion,
					  bool Shuffle)
  {
   struct Tst_Question Question;
   unsigned NumOpt;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned Index;
   bool ErrorInIndex;
   char StrInd[1 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Create test question *****/
   Tst_QstConstructor (&Question);
   Question.QstCod = PrintedQuestion->QstCod;

   /***** Get answers of question from database *****/
   ExaSet_GetAnswersQst (&Question,&mysql_res,Shuffle);
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */

   for (NumOpt = 0;
	NumOpt < Question.Answer.NumOptions;
	NumOpt++)
     {
      /***** Get next answer *****/
      row = mysql_fetch_row (mysql_res);

      /***** Assign index (row[0]).
             Index is 0,1,2,3... if no shuffle
             or 1,3,0,2... (example) if shuffle *****/
      ErrorInIndex = false;
      if (sscanf (row[0],"%u",&Index) == 1)
        {
         if (Index >= Tst_MAX_OPTIONS_PER_QUESTION)
            ErrorInIndex = true;
        }
      else
         ErrorInIndex = true;
      if (ErrorInIndex)
         Lay_ShowErrorAndExit ("Wrong index of answer.");

      if (NumOpt == 0)
	 snprintf (StrInd,sizeof (StrInd),"%u",Index);
      else
	 snprintf (StrInd,sizeof (StrInd),",%u",Index);
      Str_Concat (PrintedQuestion->StrIndexes,StrInd,
                  Tst_MAX_BYTES_INDEXES_ONE_QST);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Destroy test question *****/
   Tst_QstDestructor (&Question);
  }

/*****************************************************************************/
/***************** Create new blank exam print in database *******************/
/*****************************************************************************/

static void ExaPrn_CreatePrintInDB (struct ExaPrn_Print *Print)
  {
   /***** Insert new exam print into table *****/
   Print->PrnCod =
   DB_QueryINSERTandReturnCode ("can not create new exam print",
				"INSERT INTO exa_prints"
				" (EvtCod,UsrCod,StartTime,EndTime,NumQsts,NumQstsNotBlank,Sent,Score)"
				" VALUES"
				" (%ld,%ld,NOW(),NOW(),%u,0,'N',0)",
				Print->EvtCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Print->NumQsts);
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

   /***** Get questions of an exam print from database *****/
   NumQsts =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get questions"
					 " of an exam print",
			      "SELECT QstCod,"	// row[0]
			             "SetCod,"	// row[1]
			             "Score,"	// row[2]
			             "Indexes,"	// row[3]
			             "Answers"	// row[4]
			      " FROM exa_print_questions"
			      " WHERE PrnCod=%ld"
			      " ORDER BY QstInd",
			      Print->PrnCod);

   /***** Get questions *****/
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

         /* Get score (row[2]) */
	 Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
         if (sscanf (row[2],"%lf",&Print->PrintedQuestions[NumQst].Score) != 1)
            Lay_ShowErrorAndExit ("Wrong question score.");
         Str_SetDecimalPointToLocal ();	// Return to local system

	 /* Get indexes for this question (row[3]) */
	 Str_Copy (Print->PrintedQuestions[NumQst].StrIndexes,row[3],
		   Tst_MAX_BYTES_INDEXES_ONE_QST);

	 /* Get answers selected by user for this question (row[4]) */
	 Str_Copy (Print->PrintedQuestions[NumQst].StrAnswers,row[4],
		   Tst_MAX_BYTES_ANSWERS_ONE_QST);
	}

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (NumQsts != Print->NumQsts)
      Lay_WrongExamExit ();
  }

/*****************************************************************************/
/******************** Show an exam print to be answered **********************/
/*****************************************************************************/

static void ExaPrn_ShowExamPrintToFillIt (const char *Title,
                                          struct ExaPrn_Print *Print)
  {
   extern const char *Hlp_ASSESSMENT_Exams;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Title,
		 NULL,NULL,
		 Hlp_ASSESSMENT_Exams,Box_NOT_CLOSABLE);
   Lay_WriteHeaderClassPhoto (false,false,
			      Gbl.Hierarchy.Ins.InsCod,
			      Gbl.Hierarchy.Deg.DegCod,
			      Gbl.Hierarchy.Crs.CrsCod);

   if (Print->NumQsts)
     {
      /***** Show table with questions to answer *****/
      HTM_DIV_Begin ("id=\"examprint\"");	// Used for AJAX based refresh
      ExaPrn_ShowTableWithQstsToFill (Print);
      HTM_DIV_End ();				// Used for AJAX based refresh

      /***** Form to end/close this exam print *****/
      Frm_StartForm (ActEndExaPrn);
      // ExaEvt_PutParamEvtCod (Print->EvtCod);
      Btn_PutCreateButton ("He terminado");	// TODO: Translate!!!
      Frm_EndForm ();
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********* Show the main part (table) of an exam print to be answered ********/
/*****************************************************************************/

static void ExaPrn_ShowTableWithQstsToFill (struct ExaPrn_Print *Print)
  {
   unsigned NumQst;
   struct Tst_Question Question;

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (10);

   /***** Write one row for each question *****/
   for (NumQst = 0;
	NumQst < Print->NumQsts;
	NumQst++)
     {
      /* Create test question */
      Tst_QstConstructor (&Question);
      Question.QstCod = Print->PrintedQuestions[NumQst].QstCod;

      /* Get question from database */
      ExaSet_GetQstDataFromDB (&Question);

      /* Write question and answers */
      ExaPrn_WriteQstAndAnsToFill (Print,NumQst,&Question);

      /* Destroy test question */
      Tst_QstDestructor (&Question);
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********** Write a row of a test, with one question and its answer **********/
/*****************************************************************************/

static void ExaPrn_WriteQstAndAnsToFill (struct ExaPrn_Print *Print,
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

   if (Print->PrintedQuestions[NumQst].SetCod != CurrentSet.SetCod)
     {
      /***** Get data of this set *****/
      CurrentSet.SetCod = Print->PrintedQuestions[NumQst].SetCod;
      ExaSet_GetDataOfSetByCod (&CurrentSet);

      /***** Title for this set *****/
      HTM_TR_Begin (NULL);
      HTM_TD_Begin ("colspan=\"2\" class=\"COLOR0\"");
      ExaSet_WriteSetTitle (&CurrentSet);
      HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

   /***** Number of question and answer type *****/
   HTM_TD_Begin ("class=\"RT\"");
   Tst_WriteNumQst (NumQst + 1);
   Tst_WriteAnswerType (Question->Answer.Type);
   HTM_TD_End ();

   /***** Stem, media and answers *****/
   HTM_TD_Begin ("class=\"LT\"");

   /* Stem */
   Tst_WriteQstStem (Question->Stem,"TEST_EXA",true);

   /* Media */
   Med_ShowMedia (&Question->Media,
		  "TEST_MED_SHOW_CONT",
		  "TEST_MED_SHOW");

   /* Answers */
   Frm_StartFormNoAction ();	// Form that can not be submitted, to avoid enter key to send it
   ExaPrn_WriteAnswersToFill (Print,NumQst,Question);
   Frm_EndForm ();

   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Write answers of a question to fill them ******************/
/*****************************************************************************/

static void ExaPrn_WriteAnswersToFill (struct ExaPrn_Print *Print,
                                       unsigned NumQst,
                                       const struct Tst_Question *Question)
  {
   /***** Write answer depending on type *****/
   switch (Question->Answer.Type)
     {
      case Tst_ANS_INT:
         ExaPrn_WriteIntAnsToFill (Print,NumQst);
         break;
      case Tst_ANS_FLOAT:
         ExaPrn_WriteFloatAnsToFill (Print,NumQst);
         break;
      case Tst_ANS_TRUE_FALSE:
         ExaPrn_WriteTFAnsToFill (Print,NumQst);
         break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
         ExaPrn_WriteChoiceAnsToFill (Print,NumQst,Question);
         break;
      case Tst_ANS_TEXT:
         ExaPrn_WriteTextAnsToFill (Print,NumQst);
         break;
      default:
         break;
     }
  }

/*****************************************************************************/
/****************** Write integer answer when seeing a test ******************/
/*****************************************************************************/

static void ExaPrn_WriteIntAnsToFill (const struct ExaPrn_Print *Print,
				      unsigned NumQst)
  {
   char Id[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (Id,sizeof (Id),
	     "Ans%010u",
	     NumQst);
   HTM_TxtF ("<input type=\"text\" id=\"%s\" name=\"Ans\""
	     " size=\"11\" maxlength=\"11\" value=\"%s\"",
	     Id,
	     Print->PrintedQuestions[NumQst].StrAnswers);
   HTM_TxtF (" onchange=\"updateExamPrint('examprint','%s','Ans',"
			 "'act=%ld&ses=%s&EvtCod=%ld&NumQst=%u');"
		         " return false;\" />",	// return false is necessary to not submit form
	     Id,
	     Act_GetActCod (ActAnsExaPrn),Gbl.Session.Id,Print->EvtCod,NumQst);
  }

/*****************************************************************************/
/****************** Write float answer when seeing a test ********************/
/*****************************************************************************/

static void ExaPrn_WriteFloatAnsToFill (const struct ExaPrn_Print *Print,
				        unsigned NumQst)
  {
   char Id[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (Id,sizeof (Id),
	     "Ans%010u",
	     NumQst);
   HTM_TxtF ("<input type=\"text\" id=\"%s\" name=\"Ans\""
	     " size=\"11\" maxlength=\"%u\" value=\"%s\"",
	     Id,Tst_MAX_BYTES_FLOAT_ANSWER,
	     Print->PrintedQuestions[NumQst].StrAnswers);
   HTM_TxtF (" onchange=\"updateExamPrint('examprint','%s','Ans',"
			 "'act=%ld&ses=%s&EvtCod=%ld&NumQst=%u');"
		         " return false;\" />",	// return false is necessary to not submit form
	     Id,
	     Act_GetActCod (ActAnsExaPrn),Gbl.Session.Id,Print->EvtCod,NumQst);
  }

/*****************************************************************************/
/************** Write false / true answer when seeing a test ****************/
/*****************************************************************************/

static void ExaPrn_WriteTFAnsToFill (const struct ExaPrn_Print *Print,
	                             unsigned NumQst)
  {
   extern const char *Txt_TF_QST[2];
   char Id[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write selector for the answer *****/
   /* Initially user has not answered the question ==> initially all the answers will be blank.
      If the user does not confirm the submission of their exam ==>
      ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
   snprintf (Id,sizeof (Id),
	     "Ans%010u",
	     NumQst);
   HTM_TxtF ("<select id=\"%s\" name=\"Ans\"",Id);
   HTM_TxtF (" onchange=\"updateExamPrint('examprint','%s','Ans',"
			 "'act=%ld&ses=%s&EvtCod=%ld&NumQst=%u');"
		         " return false;\" />",	// return false is necessary to not submit form
	     Id,
	     Act_GetActCod (ActAnsExaPrn),
	     Gbl.Session.Id,Print->EvtCod,NumQst);
   HTM_OPTION (HTM_Type_STRING,"" ,Print->PrintedQuestions[NumQst].StrAnswers[0] == '\0',false,"&nbsp;");
   HTM_OPTION (HTM_Type_STRING,"T",Print->PrintedQuestions[NumQst].StrAnswers[0] == 'T' ,false,"%s",Txt_TF_QST[0]);
   HTM_OPTION (HTM_Type_STRING,"F",Print->PrintedQuestions[NumQst].StrAnswers[0] == 'F' ,false,"%s",Txt_TF_QST[1]);
   HTM_Txt ("</select>");
  }

/*****************************************************************************/
/******** Write single or multiple choice answer when seeing a test **********/
/*****************************************************************************/

static void ExaPrn_WriteChoiceAnsToFill (struct ExaPrn_Print *Print,
                                         unsigned NumQst,
                                         const struct Tst_Question *Question)
  {
   unsigned NumOpt;
   unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   bool UsrAnswers[Tst_MAX_OPTIONS_PER_QUESTION];
   char Id[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Get indexes for this question from string *****/
   TstPrn_GetIndexesFromStr (Print->PrintedQuestions[NumQst].StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   TstPrn_GetAnswersFromStr (Print->PrintedQuestions[NumQst].StrAnswers,UsrAnswers);

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

      snprintf (Id,sizeof (Id),
		"Ans%010u",
		NumQst);
      HTM_TxtF ("<input type=\"%s\" id=\"%s_%u\" name=\"Ans\" value=\"%u\"%s",
		Question->Answer.Type == Tst_ANS_UNIQUE_CHOICE ? "radio" :
								 "checkbox",
		Id,NumOpt,Indexes[NumOpt],
		UsrAnswers[Indexes[NumOpt]] ? " checked=\"checked\"" :
					      "");
      HTM_TxtF (" onclick=\"updateExamPrint('examprint','%s_%u','Ans',"
		"'act=%ld&ses=%s&EvtCod=%ld&NumQst=%u');"
		" return false;\" />",	// return false is necessary to not submit form
		Id,NumOpt,
		Act_GetActCod (ActAnsExaPrn),Gbl.Session.Id,Print->EvtCod,NumQst);
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

static void ExaPrn_WriteTextAnsToFill (const struct ExaPrn_Print *Print,
	                               unsigned NumQst)
  {
   char Id[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (Id,sizeof (Id),
	     "Ans%010u",
	     NumQst);
   HTM_TxtF ("<input type=\"text\" id=\"%s\" name=\"Ans\""
	     " size=\"40\" maxlength=\"%u\" value=\"%s\"",
	     Id,Tst_MAX_CHARS_ANSWERS_ONE_QST,
	     Print->PrintedQuestions[NumQst].StrAnswers);
   HTM_TxtF (" onchange=\"updateExamPrint('examprint','%s','Ans',"
			 "'act=%ld&ses=%s&EvtCod=%ld&NumQst=%u');"
		         " return false;\" />",	// return false is necessary to not submit form
	     Id,
	     Act_GetActCod (ActAnsExaPrn),Gbl.Session.Id,Print->EvtCod,NumQst);
  }

/*****************************************************************************/
/********************** Receive answer to an exam print **********************/
/*****************************************************************************/

void ExaPrn_ReceivePrintAnswer (void)
  {
   struct ExaPrn_Print Print;
   unsigned NumQst;

   /***** Reset print *****/
   ExaPrn_ResetPrint (&Print);

   /***** Get event code *****/
   Print.EvtCod = ExaEvt_GetParamEvtCod ();

   /***** Check if event if visible and open *****/
   if (ExaEvt_CheckIfEventIsVisibleAndOpen (Print.EvtCod))
     {
      /***** Get print data *****/
      Print.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
      ExaPrn_GetPrintDataByEvtCodAndUsrCod (&Print);
      if (Print.PrnCod <= 0)
	 Lay_WrongExamExit ();

      /***** Get questions and answers from database *****/
      ExaPrn_GetPrintQuestionsFromDB (&Print);

      /***** Get answers from form to assess a test *****/
      NumQst = ExaPrn_GetAnswerFromForm (&Print);

      /***** Update answer in database *****/
      /* Compute question score and store in database */
      ExaPrn_ComputeScoreAndStoreQuestionOfPrint (&Print,NumQst);

      /* Update exam print in database */
      ExaPrn_GetNumQstsNotBlank (&Print);
      ExaPrn_ComputeTotalScoreOfPrint (&Print);
      ExaPrn_UpdatePrintInDB (&Print);

      /***** Show table with questions to answer *****/
      ExaPrn_ShowTableWithQstsToFill (&Print);
     }
   else
      Ale_ShowAlert (Ale_INFO,"Usted no tiene acceso al examen.");	// TODO: Need translation!!!!
  }

/*****************************************************************************/
/******** Get questions and answers from form to assess an exam print ********/
/*****************************************************************************/

static unsigned ExaPrn_GetAnswerFromForm (struct ExaPrn_Print *Print)
  {
   unsigned NumQst;

   /***** Get question index from form *****/
   NumQst = ExaPrn_GetParamQstInd ();

   /***** Get answers selected by user for this question *****/
   Par_GetParToText ("Ans",Print->PrintedQuestions[NumQst].StrAnswers,
		     Tst_MAX_BYTES_ANSWERS_ONE_QST);  /* If answer type == T/F ==> " ", "T", "F"; if choice ==> "0", "2",... */

   return NumQst;
  }

/*****************************************************************************/
/******************* Get parameter with index of question ********************/
/*****************************************************************************/

static unsigned ExaPrn_GetParamQstInd (void)
  {
   long NumQst;

   NumQst = Par_GetParToLong ("NumQst");
   if (NumQst < 0)
      Lay_ShowErrorAndExit ("Wrong number of question.");

   return (unsigned) NumQst;
  }

/*****************************************************************************/
/*********** Compute score of each question and store in database ************/
/*****************************************************************************/

static void ExaPrn_ComputeScoresAndStoreQuestionsOfPrint (struct ExaPrn_Print *Print)
  {
   unsigned NumQst;

   /***** Initialize total score *****/
   Print->Score = 0.0;
   Print->NumQstsNotBlank = 0;

   /***** Compute and store scores of all questions *****/
   for (NumQst = 0;
	NumQst < Print->NumQsts;
	NumQst++)
     {
      /* Compute question score and store in database */
      ExaPrn_ComputeScoreAndStoreQuestionOfPrint (Print,NumQst);

      /* Accumulate total score */
      Print->Score += Print->PrintedQuestions[NumQst].Score;
      if (Print->PrintedQuestions[NumQst].AnswerIsNotBlank)
	 Print->NumQstsNotBlank++;
     }
  }

/*****************************************************************************/
/*********** Compute score of one question and store in database *************/
/*****************************************************************************/

static void ExaPrn_ComputeScoreAndStoreQuestionOfPrint (struct ExaPrn_Print *Print,
                                                        unsigned NumQst)
  {
   struct Tst_Question Question;
   char CurrentStrAnswersInDB[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];	// Answers selected by user

   /***** Compute question score *****/
   Tst_QstConstructor (&Question);
   Question.QstCod = Print->PrintedQuestions[NumQst].QstCod;
   Question.Answer.Type = ExaSet_GetQstAnswerTypeFromDB (Question.QstCod);
   ExaPrn_ComputeAnswerScore (&Print->PrintedQuestions[NumQst],&Question);
   Tst_QstDestructor (&Question);

   /***** If type is unique choice and the option (radio button) is checked
          ==> uncheck it by deleting answer *****/
   if (Question.Answer.Type == Tst_ANS_UNIQUE_CHOICE)
     {
      ExaPrn_GetAnswerFromDB (Print,Print->PrintedQuestions[NumQst].QstCod,
                              CurrentStrAnswersInDB);
      if (!strcmp (Print->PrintedQuestions[NumQst].StrAnswers,CurrentStrAnswersInDB))
	 Print->PrintedQuestions[NumQst].StrAnswers[0] = '\0';
     }

   /***** Store test exam question in database *****/
   ExaPrn_StoreOneQstOfPrintInDB (Print,
				  NumQst);	// 0, 1, 2, 3...
  }

/*****************************************************************************/
/************* Write answers of a question when assessing a test *************/
/*****************************************************************************/

void ExaPrn_ComputeAnswerScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				struct Tst_Question *Question)
  {
   void (*ExaPrn_GetCorrectAndComputeAnsScore[Tst_NUM_ANS_TYPES]) (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                                   struct Tst_Question *Question) =
    {
     [Tst_ANS_INT            ] = ExaPrn_GetCorrectAndComputeIntAnsScore,
     [Tst_ANS_FLOAT          ] = ExaPrn_GetCorrectAndComputeFltAnsScore,
     [Tst_ANS_TRUE_FALSE     ] = ExaPrn_GetCorrectAndComputeTF_AnsScore,
     [Tst_ANS_UNIQUE_CHOICE  ] = ExaPrn_GetCorrectAndComputeChoAnsScore,
     [Tst_ANS_MULTIPLE_CHOICE] = ExaPrn_GetCorrectAndComputeChoAnsScore,
     [Tst_ANS_TEXT           ] = ExaPrn_GetCorrectAndComputeTxtAnsScore,
    };

   /***** Get correct answer and compute answer score depending on type *****/
   ExaPrn_GetCorrectAndComputeAnsScore[Question->Answer.Type] (PrintedQuestion,Question);
  }

/*****************************************************************************/
/******* Get correct answer and compute score for each type of answer ********/
/*****************************************************************************/

static void ExaPrn_GetCorrectAndComputeIntAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question)
  {
   /***** Get the numerical value of the correct answer,
          and compute score *****/
   ExaPrn_GetCorrectIntAnswerFromDB (Question);
   TstPrn_ComputeIntAnsScore (PrintedQuestion,Question);
  }

static void ExaPrn_GetCorrectAndComputeFltAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question)
  {
   /***** Get the numerical value of the minimum and maximum correct answers,
          and compute score *****/
   ExaPrn_GetCorrectFltAnswerFromDB (Question);
   TstPrn_ComputeFltAnsScore (PrintedQuestion,Question);
  }

static void ExaPrn_GetCorrectAndComputeTF_AnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question)
  {
   /***** Get answer true or false,
          and compute score *****/
   ExaPrn_GetCorrectTF_AnswerFromDB (Question);
   TstPrn_ComputeTF_AnsScore (PrintedQuestion,Question);
  }

static void ExaPrn_GetCorrectAndComputeChoAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question)
  {
   /***** Get correct options of test question from database,
          and compute score *****/
   ExaPrn_GetCorrectChoAnswerFromDB (Question);
   TstPrn_ComputeChoAnsScore (PrintedQuestion,Question);
  }

static void ExaPrn_GetCorrectAndComputeTxtAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question)
  {
   /***** Get correct text answers for this question from database,
          and compute score *****/
   ExaPrn_GetCorrectTxtAnswerFromDB (Question);
   TstPrn_ComputeTxtAnsScore (PrintedQuestion,Question);
  }

/*****************************************************************************/
/***************** Get correct answer for each type of answer ****************/
/*****************************************************************************/

static void ExaPrn_GetCorrectIntAnswerFromDB (struct Tst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Query database *****/
   Question->Answer.NumOptions =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get answers of a question",
			      "SELECT Answer"		// row[0]
			      " FROM exa_set_answers"
			      " WHERE QstCod=%ld",
			      Question->QstCod);

   /***** Check if number of rows is correct *****/
   Tst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get correct answer *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%ld",&Question->Answer.Integer) != 1)
      Lay_ShowErrorAndExit ("Wrong integer answer.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

static void ExaPrn_GetCorrectFltAnswerFromDB (struct Tst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumOpt;
   double Tmp;

   /***** Query database *****/
   Question->Answer.NumOptions =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get answers of a question",
			      "SELECT Answer"		// row[0]
			      " FROM exa_set_answers"
			      " WHERE QstCod=%ld",
			      Question->QstCod);

   /***** Check if number of rows is correct *****/
   if (Question->Answer.NumOptions != 2)
      Lay_ShowErrorAndExit ("Wrong float range.");

   /***** Get float range *****/
   for (NumOpt = 0;
	NumOpt < 2;
	NumOpt++)
     {
      row = mysql_fetch_row (mysql_res);
      Question->Answer.FloatingPoint[NumOpt] = Str_GetDoubleFromStr (row[0]);
     }
   if (Question->Answer.FloatingPoint[0] >
       Question->Answer.FloatingPoint[1]) 	// The maximum and the minimum are swapped
    {
      /* Swap maximum and minimum */
      Tmp = Question->Answer.FloatingPoint[0];
      Question->Answer.FloatingPoint[0] = Question->Answer.FloatingPoint[1];
      Question->Answer.FloatingPoint[1] = Tmp;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

static void ExaPrn_GetCorrectTF_AnswerFromDB (struct Tst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Query database *****/
   Question->Answer.NumOptions =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get answers of a question",
			      "SELECT Answer"		// row[0]
			      " FROM exa_set_answers"
			      " WHERE QstCod=%ld",
			      Question->QstCod);

   /***** Check if number of rows is correct *****/
   Tst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get answer *****/
   row = mysql_fetch_row (mysql_res);
   Question->Answer.TF = row[0][0];

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

static void ExaPrn_GetCorrectChoAnswerFromDB (struct Tst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumOpt;

   /***** Query database *****/
   Question->Answer.NumOptions =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get answers of a question",
			      "SELECT Correct"		// row[0]
			      " FROM exa_set_answers"
			      " WHERE QstCod=%ld"
			      " ORDER BY AnsInd",
			      Question->QstCod);
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      /* Get next answer */
      row = mysql_fetch_row (mysql_res);

      /* Assign correctness (row[0]) of this answer (this option) */
      Question->Answer.Options[NumOpt].Correct = (row[0][0] == 'Y');
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

static void ExaPrn_GetCorrectTxtAnswerFromDB (struct Tst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumOpt;

   /***** Query database *****/
   Question->Answer.NumOptions =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get answers of a question",
			      "SELECT Answer"		// row[0]
			      " FROM exa_set_answers"
			      " WHERE QstCod=%ld",
			      Question->QstCod);

   /***** Get text and correctness of answers for this question from database (one row per answer) *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      /***** Get next answer *****/
      row = mysql_fetch_row (mysql_res);

      /***** Allocate memory for text in this choice answer *****/
      if (!Tst_AllocateTextChoiceAnswer (Question,NumOpt))
	 /* Abort on error */
	 Ale_ShowAlertsAndExit ();

      /***** Copy answer text (row[0]) and convert it, that is in HTML, to rigorous HTML ******/
      Str_Copy (Question->Answer.Options[NumOpt].Text,row[0],
                Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Question->Answer.Options[NumOpt].Text,
                        Tst_MAX_BYTES_ANSWER_OR_FEEDBACK,false);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Get the questions of an exam print from database **************/
/*****************************************************************************/

static void ExaPrn_GetAnswerFromDB (struct ExaPrn_Print *Print,long QstCod,
                                    char StrAnswers[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get questions of an exam print from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get answer in an exam print",
		       "SELECT Answers"
		       " FROM exa_print_questions"
		       " WHERE PrnCod=%ld AND QstCod=%ld",
		       Print->PrnCod,QstCod))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get answers selected by user for this question (row[0]) */
      Str_Copy (StrAnswers,row[0],
		Tst_MAX_BYTES_ANSWERS_ONE_QST);
     }
   else
      StrAnswers[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Store user's answers of an test exam into database ************/
/*****************************************************************************/

static void ExaPrn_StoreOneQstOfPrintInDB (const struct ExaPrn_Print *Print,
                                           unsigned NumQst)
  {
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
		    Print->PrintedQuestions[NumQst].StrIndexes,
		    Print->PrintedQuestions[NumQst].StrAnswers);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/************ Get number of questions not blank in an exam print *************/
/*****************************************************************************/

static void ExaPrn_GetNumQstsNotBlank (struct ExaPrn_Print *Print)
  {
   /***** Count number of questions not blank in exam print in database *****/
   Print->NumQstsNotBlank = (unsigned)
	                    DB_QueryCOUNT ("can not get number of questions not blank",
				           "SELECT COUNT(*) FROM exa_print_questions"
				           " WHERE PrnCod=%ld AND Answers<>''",
				           Print->PrnCod);
  }

/*****************************************************************************/
/***************** Compute score of questions of an exam print ***************/
/*****************************************************************************/

static void ExaPrn_ComputeTotalScoreOfPrint (struct ExaPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Default score *****/
   Print->Score = 0.0;

   /***** Compute total score of exam print *****/
   if (DB_QuerySELECT (&mysql_res,"can not get score of exam print",
		       "SELECT SUM(Score) FROM exa_print_questions WHERE PrnCod=%ld",
		       Print->PrnCod))
     {
      /***** Get sum of individual scores (row[0]) *****/
      row = mysql_fetch_row (mysql_res);
      if (row[0])
	{
	 /* Get score (row[0]) */
	 Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
	 if (sscanf (row[0],"%lf",&Print->Score) != 1)
	    Print->Score = 0.0;
	 Str_SetDecimalPointToLocal ();	// Return to local system
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Update exam print in database ************************/
/*****************************************************************************/

static void ExaPrn_UpdatePrintInDB (const struct ExaPrn_Print *Print)
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
	           " AND EvtCod=%ld AND UsrCod=%ld",	// Extra checks
		   Print->NumQstsNotBlank,
		   Print->Sent ? 'Y' :
			         'N',
		   Print->Score,
		   Print->PrnCod,
		   Print->EvtCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/********************** Receive answer to an exam print **********************/
/*****************************************************************************/

void ExaPrn_EndPrintAnswer (void)
  {
   Ale_ShowAlert (Ale_INFO,"Terminar de contestar el examen.");
  }

/*****************************************************************************/
/***************** Write parameter with code of exam print *******************/
/*****************************************************************************/

// static void ExaPrn_PutParamPrnCod (long ExaCod)
//   {
//    Par_PutHiddenParamLong (NULL,"PrnCod",ExaCod);
//   }

/*****************************************************************************/
/***************** Get parameter with code of exam print *********************/
/*****************************************************************************/

// static long ExaPrn_GetParamPrnCod (void)
//   {
//    /***** Get code of exam print *****/
//    return Par_GetParToLong ("PrnCod");
//   }
