// swad_exam_print.c: exam prints (each copy of an exam in a session for a student)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_exam.h"
#include "swad_exam_database.h"
#include "swad_exam_log.h"
#include "swad_exam_print.h"
#include "swad_exam_result.h"
#include "swad_exam_session.h"
#include "swad_exam_set.h"
#include "swad_exam_type.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaPrn_GetPrintDataFromRow (MYSQL_RES **mysql_res,
                                        struct ExaPrn_Print *Print,
                                        unsigned NumPrints);

static void ExaPrn_GetQuestionsForNewPrintFromDB (struct ExaPrn_Print *Print,long ExaCod);
static unsigned ExaPrn_GetSomeQstsFromSetToPrint (struct ExaPrn_Print *Print,
                                                  struct ExaSet_Set *Set,
                                                  unsigned *NumQstsInPrint);
static void ExaPrn_GenerateChoiceIndexes (struct TstPrn_PrintedQuestion *PrintedQuestion,
					  bool Shuffle);
static void ExaPrn_CreatePrint (struct ExaPrn_Print *Print);

static void ExaPrn_ShowExamPrintToFillIt (struct Exa_Exams *Exams,
                                          struct ExaPrn_Print *Print);
static void ExaPrn_GetAndWriteDescription (long ExaCod);
static void ExaPrn_ShowTableWithQstsToFill (struct Exa_Exams *Exams,
					    const struct ExaPrn_Print *Print);
static void ExaPrn_WriteQstAndAnsToFill (const struct ExaPrn_Print *Print,
                                         unsigned QstInd,
                                         struct Qst_Question *Question);
static void ExaPrn_WriteAnswersToFill (const struct ExaPrn_Print *Print,
                                       unsigned QstInd,
                                       struct Qst_Question *Question);

//-----------------------------------------------------------------------------
static void ExaPrn_WriteIntAnsToFill (const struct ExaPrn_Print *Print,
				      unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question);
static void ExaPrn_WriteFltAnsToFill (const struct ExaPrn_Print *Print,
				      unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question);
static void ExaPrn_WriteTF_AnsToFill (const struct ExaPrn_Print *Print,
	                              unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question);
static void ExaPrn_WriteChoAnsToFill (const struct ExaPrn_Print *Print,
                                      unsigned QstInd,
                                      struct Qst_Question *Question);
static void ExaPrn_WriteTxtAnsToFill (const struct ExaPrn_Print *Print,
	                              unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question);
//-----------------------------------------------------------------------------

static void ExaPrn_WriteJSToUpdateExamPrint (const struct ExaPrn_Print *Print,
	                                     unsigned QstInd,
	                                     const char *Id,int NumOpt);

static void ExaPrn_GetAnswerFromForm (struct ExaPrn_Print *Print,unsigned QstInd);

static unsigned ExaPrn_GetParQstInd (void);

static void ExaPrn_ComputeScoreAndStoreQuestionOfPrint (struct ExaPrn_Print *Print,
                                                        unsigned QstInd);

//-----------------------------------------------------------------------------
static void ExaPrn_GetCorrectAndComputeIntAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question);
static void ExaPrn_GetCorrectAndComputeFltAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question);
static void ExaPrn_GetCorrectAndComputeTF_AnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question);
static void ExaPrn_GetCorrectAndComputeChoAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question);
static void ExaPrn_GetCorrectAndComputeTxtAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question);
//-----------------------------------------------------------------------------
static void ExaPrn_GetCorrectIntAnswerFromDB (struct Qst_Question *Question);
static void ExaPrn_GetCorrectFltAnswerFromDB (struct Qst_Question *Question);
static void ExaPrn_GetCorrectTF_AnswerFromDB (struct Qst_Question *Question);
static void ExaPrn_GetCorrectChoAnswerFromDB (struct Qst_Question *Question);
static void ExaPrn_GetCorrectTxtAnswerFromDB (struct Qst_Question *Question);
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**************************** Reset exam print *******************************/
/*****************************************************************************/

void ExaPrn_ResetPrint (struct ExaPrn_Print *Print)
  {
   Print->PrnCod = -1L;
   Print->SesCod = -1L;
   Print->UsrCod = -1L;
   Print->TimeUTC[Dat_STR_TIME] =
   Print->TimeUTC[Dat_END_TIME] = (time_t) 0;
   Print->Sent                    = false;	// After creating an exam print, it's not sent
   Print->NumQsts.All                  =
   Print->NumQsts.NotBlank             =
   Print->NumQsts.Valid.Correct        =
   Print->NumQsts.Valid.Wrong.Negative =
   Print->NumQsts.Valid.Wrong.Zero     =
   Print->NumQsts.Valid.Wrong.Positive =
   Print->NumQsts.Valid.Blank          =
   Print->NumQsts.Valid.Total          = 0;
   Print->Score.All   =
   Print->Score.Valid = 0.0;
  }

/*****************************************************************************/
/********************** Show print of an exam in a session *******************/
/*****************************************************************************/

void ExaPrn_ShowExamPrint (void)
  {
   extern const char *Txt_You_dont_have_access_to_the_exam;
   struct Exa_Exams Exams;
   struct ExaSes_Session Session;
   struct ExaPrn_Print Print;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exams.Exam);
   ExaSes_ResetSession (&Session);

   /***** Get and check parameters *****/
   ExaSes_GetAndCheckPars (&Exams,&Session);

   /***** Check if I can access to this session *****/
   switch (ExaSes_CheckIfICanAnswerThisSession (&Exams.Exam,&Session))
     {
      case Usr_CAN:
	 /***** Set basic data of exam print *****/
	 Print.SesCod = Session.SesCod;
	 Print.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;

	 /***** Get exam print data from database *****/
	 ExaPrn_GetPrintDataBySesCodAndUsrCod (&Print);

	 if (Print.PrnCod <= 0)	// Exam print does not exists ==> create it
	   {
	    /***** Set again basic data of exam print *****/
	    Print.SesCod = Session.SesCod;
	    Print.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;

	    /***** Get questions from database *****/
	    ExaPrn_GetQuestionsForNewPrintFromDB (&Print,Exams.Exam.ExaCod);

	    if (Print.NumQsts.All)
	      {
	       /***** Create new exam print in database *****/
	       ExaPrn_CreatePrint (&Print);

	       /***** Set log print code and action *****/
	       ExaLog_SetPrnCod (Print.PrnCod);
	       ExaLog_SetAction (ExaLog_START_EXAM);
	       ExaLog_SetIfCanAnswer (true);
	      }
	   }
	 else			// Exam print exists
	   {
	    /***** Get exam print data from database *****/
	    ExaPrn_GetPrintDataBySesCodAndUsrCod (&Print);

	    /***** Get questions and current user's answers from database *****/
	    ExaPrn_GetPrintQuestionsFromDB (&Print);

	    /***** Set log print code and action *****/
	    ExaLog_SetPrnCod (Print.PrnCod);
	    ExaLog_SetAction (ExaLog_RESUME_EXAM);
	    ExaLog_SetIfCanAnswer (true);
	   }

	 /***** Show test to be answered *****/
	 ExaPrn_ShowExamPrintToFillIt (&Exams,&Print);
	 break;
      case Usr_CAN_NOT:	// Session not open or accessible
      default:
	 /***** Show warning *****/
	 Ale_ShowAlert (Ale_INFO,Txt_You_dont_have_access_to_the_exam);
	 break;
     }
  }

/*****************************************************************************/
/**************** Get data of an exam print using print code *****************/
/*****************************************************************************/

void ExaPrn_GetPrintDataByPrnCod (struct ExaPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   unsigned NumPrints;

   /***** Make database query *****/
   NumPrints = Exa_DB_GetPrintDataByPrnCod (&mysql_res,Print->PrnCod);

   /***** Get data of print *****/
   ExaPrn_GetPrintDataFromRow (&mysql_res,Print,NumPrints);
  }

/*****************************************************************************/
/******** Get data of an exam print using session code and user code *********/
/*****************************************************************************/

void ExaPrn_GetPrintDataBySesCodAndUsrCod (struct ExaPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   unsigned NumPrints;

   /***** Make database query *****/
   NumPrints = Exa_DB_GetPrintDataBySesCodAndUsrCod (&mysql_res,
                                                     Print->SesCod,
                                                     Print->UsrCod);

   /***** Get data of print *****/
   ExaPrn_GetPrintDataFromRow (&mysql_res,Print,NumPrints);
  }

/*****************************************************************************/
/************************* Get assignment data *******************************/
/*****************************************************************************/

static void ExaPrn_GetPrintDataFromRow (MYSQL_RES **mysql_res,
                                        struct ExaPrn_Print *Print,
                                        unsigned NumPrints)
  {
   MYSQL_ROW row;

   if (NumPrints)
     {
      /* Get next row from result */
      row = mysql_fetch_row (*mysql_res);

      /* Get print code (row[0]) */
      Print->PrnCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get session code (row[1]) */
      Print->SesCod = Str_ConvertStrCodToLongCod (row[1]);

      /* Get user code (row[2]) */
      Print->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get date-time (row[3] and row[4] hold UTC date-time) */
      Print->TimeUTC[Dat_STR_TIME] = Dat_GetUNIXTimeFromStr (row[3]);
      Print->TimeUTC[Dat_END_TIME] = Dat_GetUNIXTimeFromStr (row[4]);

      /* Get number of questions (row[5]) */
      if (sscanf (row[5],"%u",&Print->NumQsts.All) != 1)
	 Print->NumQsts.All = 0;

      /* Get number of questions not blank (row[6]) */
      if (sscanf (row[6],"%u",&Print->NumQsts.NotBlank) != 1)
	 Print->NumQsts.NotBlank = 0;

      /* Get if exam has been sent (row[7]) */
      Print->Sent = (row[7][0] == 'Y');

      /* Get score (row[8]) */
      Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
      if (sscanf (row[8],"%lf",&Print->Score.All) != 1)
	 Print->Score.All = 0.0;
      Str_SetDecimalPointToLocal ();	// Return to local system
     }
   else
      ExaPrn_ResetPrint (Print);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/*********** Get questions for a new exam print from the database ************/
/*****************************************************************************/

static void ExaPrn_GetQuestionsForNewPrintFromDB (struct ExaPrn_Print *Print,long ExaCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumSets;
   unsigned NumSet;
   struct ExaSet_Set Set;
   unsigned NumQstsFromSet;
   unsigned NumQstsInPrint = 0;

   /***** Get questions from all sets *****/
   Print->NumQsts.All = 0;
   NumSets = Exa_DB_GetExamSets (&mysql_res,ExaCod);

   /***** For each set in exam... *****/
   for (NumSet = 0;
	NumSet < NumSets;
	NumSet++)
     {
      /***** Create set of questions *****/
      ExaSet_ResetSet (&Set);

      /***** Get set data *****/
      ExaSet_GetSetDataFromRow (mysql_res,&Set);

      /***** Questions in this set *****/
      NumQstsFromSet = ExaPrn_GetSomeQstsFromSetToPrint (Print,&Set,&NumQstsInPrint);
      Print->NumQsts.All += NumQstsFromSet;
     }

   /***** Check *****/
   if (Print->NumQsts.All != NumQstsInPrint)
      Err_ShowErrorAndExit ("Wrong number of questions.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Get some questions from a set ************************/
/*****************************************************************************/

static unsigned ExaPrn_GetSomeQstsFromSetToPrint (struct ExaPrn_Print *Print,
                                                  struct ExaSet_Set *Set,
                                                  unsigned *NumQstsInPrint)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQstsInSet;
   unsigned NumQstInSet;
   Qst_AnswerType_t AnswerType;
   bool Shuffle;

   /***** Get questions from database *****/
   NumQstsInSet = Exa_DB_GetSomeQstsFromSetToPrint (&mysql_res,
                                                    Set->SetCod,
                                                    Set->NumQstsToPrint);

   /***** Questions in this set *****/
   for (NumQstInSet = 0, The_ResetRowColor ();
	NumQstInSet < NumQstsInSet;
	NumQstInSet++, (*NumQstsInPrint)++, The_ChangeRowColor ())
     {
      /***** Get question data *****/
      row = mysql_fetch_row (mysql_res);
      /*
      row[0] QstCod
      row[1] AnsType
      row[2] Shuffle
      */

      /* Get question code (row[0]) */
      Print->PrintedQuestions[*NumQstsInPrint].QstCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Set set of questions */
      Print->PrintedQuestions[*NumQstsInPrint].SetCod = Set->SetCod;

      /* Get answer type (row[1]) */
      AnswerType = Qst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);

      /* Get shuffle (row[2]) */
      Shuffle = (row[2][0] == 'Y');

      /* Set indexes of answers */
      switch (AnswerType)
	{
	 case Qst_ANS_INT:
	 case Qst_ANS_FLOAT:
	 case Qst_ANS_TRUE_FALSE:
	 case Qst_ANS_TEXT:
	    Print->PrintedQuestions[*NumQstsInPrint].StrIndexes[0] = '\0';
	    break;
	 case Qst_ANS_UNIQUE_CHOICE:
	 case Qst_ANS_MULTIPLE_CHOICE:
            /* If answer type is unique or multiple option,
               generate indexes of answers depending on shuffle */
	    ExaPrn_GenerateChoiceIndexes (&Print->PrintedQuestions[*NumQstsInPrint],Shuffle);
	    break;
	 default:
	    break;
	}

      /* Reset user's answers.
         Initially user has not answered the question ==> initially all answers will be blank.
         If the user does not confirm the submission of their exam ==>
         ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
      Print->PrintedQuestions[*NumQstsInPrint].StrAnswers[0] = '\0';

      /* Reset score of this question in print */
      Print->PrintedQuestions[*NumQstsInPrint].Score = 0.0;
     }

   return NumQstsInSet;
  }

/*****************************************************************************/
/*************** Generate choice indexes depending on shuffle ****************/
/*****************************************************************************/

static void ExaPrn_GenerateChoiceIndexes (struct TstPrn_PrintedQuestion *PrintedQuestion,
					  bool Shuffle)
  {
   struct Qst_Question Question;
   unsigned NumOpt;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned Index;
   bool ErrorInIndex;
   char StrInd[1 + Cns_MAX_DIGITS_UINT + 1];

   /***** Create test question *****/
   Qst_QstConstructor (&Question);
   Question.QstCod = PrintedQuestion->QstCod;

   /***** Get answers of question from database *****/
   Question.Answer.NumOptions = Exa_DB_GetQstAnswersFromSet (&mysql_res,
                                                             Question.QstCod,
                                                             Shuffle);
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
         if (Index >= Qst_MAX_OPTIONS_PER_QUESTION)
            ErrorInIndex = true;
        }
      else
         ErrorInIndex = true;
      if (ErrorInIndex)
         Err_WrongAnswerIndexExit ();

      if (NumOpt == 0)
	 snprintf (StrInd,sizeof (StrInd),"%u",Index);
      else
	 snprintf (StrInd,sizeof (StrInd),",%u",Index);
      Str_Concat (PrintedQuestion->StrIndexes,StrInd,
                  sizeof (PrintedQuestion->StrIndexes) - 1);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Destroy test question *****/
   Qst_QstDestructor (&Question);
  }

/*****************************************************************************/
/***************** Create new blank exam print in database *******************/
/*****************************************************************************/

static void ExaPrn_CreatePrint (struct ExaPrn_Print *Print)
  {
   unsigned QstInd;

   /***** Insert new exam print into database *****/
   Print->PrnCod = Exa_DB_CreatePrint (Print);

   /***** Store all questions (with blank answers)
          of this exam print just generated in database *****/
   for (QstInd = 0;
	QstInd < Print->NumQsts.All;
	QstInd++)
      Exa_DB_StoreOneQstOfPrint (Print,QstInd);
  }

/*****************************************************************************/
/************* Get the questions of an exam print from database **************/
/*****************************************************************************/

void ExaPrn_GetPrintQuestionsFromDB (struct ExaPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned QstInd;

   /***** Get questions of an exam print from database *****/
   if ((Print->NumQsts.All = Exa_DB_GetPrintQuestions (&mysql_res,Print->PrnCod))
       <= ExaPrn_MAX_QUESTIONS_PER_EXAM_PRINT)
      for (QstInd = 0;
	   QstInd < Print->NumQsts.All;
	   QstInd++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get question code (row[0])
	    and set code (row[1]) */
	 if ((Print->PrintedQuestions[QstInd].QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
	    Err_WrongQuestionExit ();
	 if ((Print->PrintedQuestions[QstInd].SetCod = Str_ConvertStrCodToLongCod (row[1])) <= 0)
	    Err_WrongSetExit ();

         /* Get score (row[2]) */
	 Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
         if (sscanf (row[2],"%lf",&Print->PrintedQuestions[QstInd].Score) != 1)
            Err_ShowErrorAndExit ("Wrong question score.");
         Str_SetDecimalPointToLocal ();	// Return to local system

	 /* Get indexes for this question (row[3])
	    and answers selected by user for this question (row[4]) */
	 Str_Copy (Print->PrintedQuestions[QstInd].StrIndexes,row[3],
		   sizeof (Print->PrintedQuestions[QstInd].StrIndexes) - 1);
	 Str_Copy (Print->PrintedQuestions[QstInd].StrAnswers,row[4],
		   sizeof (Print->PrintedQuestions[QstInd].StrAnswers) - 1);
	}

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (Print->NumQsts.All > ExaPrn_MAX_QUESTIONS_PER_EXAM_PRINT)
      Err_ShowErrorAndExit ("Too many questions.");
  }

/*****************************************************************************/
/******************** Show an exam print to be answered **********************/
/*****************************************************************************/

static void ExaPrn_ShowExamPrintToFillIt (struct Exa_Exams *Exams,
                                          struct ExaPrn_Print *Print)
  {
   extern const char *Hlp_ASSESSMENT_Exams_answer_exam;

   /***** Begin box *****/
   Box_BoxBegin (Exams->Exam.Title,NULL,NULL,
		 Hlp_ASSESSMENT_Exams_answer_exam,Box_NOT_CLOSABLE);

      /***** Heading *****/
      /* Institution, degree and course */
      Lay_WriteHeaderClassPhoto (Vie_VIEW);

      /***** Show user and time *****/
      HTM_TABLE_BeginWideMarginPadding (10);
   	 ExaRes_ShowExamResultUser (&Gbl.Usrs.Me.UsrDat);
      HTM_TABLE_End ();

      /***** Exam description *****/
      ExaPrn_GetAndWriteDescription (Exams->Exam.ExaCod);

      if (Print->NumQsts.All)
	{
	 /***** Show table with questions to answer *****/
	 HTM_DIV_Begin ("id=\"examprint\"");	// Used for AJAX based refresh
	    ExaPrn_ShowTableWithQstsToFill (Exams,Print);
	 HTM_DIV_End ();			// Used for AJAX based refresh
	}

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* Write description in an exam print ********************/
/*****************************************************************************/

static void ExaPrn_GetAndWriteDescription (long ExaCod)
  {
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Get description from database *****/
   Exa_DB_GetExamTxt (ExaCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
   ALn_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);			// Insert links

   /***** Write description *****/
   HTM_DIV_Begin ("class=\"EXA_PRN_DESC DAT_SMALL_%s\"",The_GetSuffix ());
      HTM_Txt (Txt);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********* Show the main part (table) of an exam print to be answered ********/
/*****************************************************************************/

static void ExaPrn_ShowTableWithQstsToFill (struct Exa_Exams *Exams,
					    const struct ExaPrn_Print *Print)
  {
   extern const char *Txt_I_have_finished;
   unsigned QstInd;
   struct Qst_Question Question;

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (10);

      /***** Write one row for each question *****/
      for (QstInd = 0;
	   QstInd < Print->NumQsts.All;
	   QstInd++)
	{
	 /* Create test question */
	 Qst_QstConstructor (&Question);
	 Question.QstCod = Print->PrintedQuestions[QstInd].QstCod;

	 /* Get question from database */
	 ExaSet_GetQstDataFromDB (&Question);

	 /* Write question and answers */
	 ExaPrn_WriteQstAndAnsToFill (Print,QstInd,&Question);

	 /* Destroy test question */
	 Qst_QstDestructor (&Question);
	}

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Form to end/close this exam print *****/
   Frm_BeginForm (ActEndExaPrn);
      ExaSes_PutParsEdit (Exams);
      Btn_PutCreateButton (Txt_I_have_finished);
   Frm_EndForm ();
  }

/*****************************************************************************/
/********** Write a row of a test, with one question and its answer **********/
/*****************************************************************************/

static void ExaPrn_WriteQstAndAnsToFill (const struct ExaPrn_Print *Print,
                                         unsigned QstInd,
                                         struct Qst_Question *Question)
  {
   static struct ExaSet_Set CurrentSet =
     {
      .ExaCod = -1L,
      .SetCod = -1L,
      .SetInd = 0,
      .NumQstsToPrint = 0,
      .Title[0] = '\0'
     };

   if (Print->PrintedQuestions[QstInd].SetCod != CurrentSet.SetCod)
     {
      /***** Get data of this set *****/
      CurrentSet.SetCod = Print->PrintedQuestions[QstInd].SetCod;
      ExaSet_GetSetDataByCod (&CurrentSet);

      /***** Title for this set *****/
      HTM_TR_Begin (NULL);
	 HTM_TD_Begin ("colspan=\"2\" class=\"%s\"",The_GetColorRows ());
	    ExaSet_WriteSetTitle (&CurrentSet);
	 HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Number of question and answer type *****/
      HTM_TD_Begin ("class=\"RT\"");
	 Lay_WriteIndex (QstInd + 1,"BIG_INDEX");
	 Qst_WriteAnswerType (Question->Answer.Type,"DAT_SMALL");
      HTM_TD_End ();

      /***** Stem, media and answers *****/
      HTM_TD_Begin ("class=\"LT\"");

	 /* Stem */
	 Qst_WriteQstStem (Question->Stem,"Qst_TXT",HidVis_VISIBLE);

	 /* Media */
	 Med_ShowMedia (&Question->Media,
			"Tst_MED_SHOW_CONT",
			"Tst_MED_SHOW");

	 /* Answers */
	 Frm_BeginFormNoAction ();	// Form that can not be submitted, to avoid enter key to send it
	    ExaPrn_WriteAnswersToFill (Print,QstInd,Question);
	 Frm_EndForm ();

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Write answers of a question to fill them ******************/
/*****************************************************************************/

static void ExaPrn_WriteAnswersToFill (const struct ExaPrn_Print *Print,
                                       unsigned QstInd,
                                       struct Qst_Question *Question)
  {
   void (*ExaPrn_WriteAnsToFill[Qst_NUM_ANS_TYPES]) (const struct ExaPrn_Print *Print,
                                                     unsigned QstInd,
                                                     struct Qst_Question *Question) =
    {
     [Qst_ANS_INT            ] = ExaPrn_WriteIntAnsToFill,
     [Qst_ANS_FLOAT          ] = ExaPrn_WriteFltAnsToFill,
     [Qst_ANS_TRUE_FALSE     ] = ExaPrn_WriteTF_AnsToFill,
     [Qst_ANS_UNIQUE_CHOICE  ] = ExaPrn_WriteChoAnsToFill,
     [Qst_ANS_MULTIPLE_CHOICE] = ExaPrn_WriteChoAnsToFill,
     [Qst_ANS_TEXT           ] = ExaPrn_WriteTxtAnsToFill,
    };

   /***** Write answers *****/
   ExaPrn_WriteAnsToFill[Question->Answer.Type] (Print,QstInd,Question);
  }

/*****************************************************************************/
/****************** Write integer answer when seeing a test ******************/
/*****************************************************************************/

static void ExaPrn_WriteIntAnsToFill (const struct ExaPrn_Print *Print,
				      unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question)
  {
   char Id[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (Id,sizeof (Id),"Ans%010u",QstInd);
   HTM_TxtF ("<input type=\"text\" id=\"%s\" name=\"Ans\""
	     " size=\"11\" maxlength=\"11\" value=\"%s\"",
	     Id,Print->PrintedQuestions[QstInd].StrAnswers);
   ExaPrn_WriteJSToUpdateExamPrint (Print,QstInd,Id,-1);
   HTM_Txt (" />");
  }

/*****************************************************************************/
/****************** Write float answer when seeing a test ********************/
/*****************************************************************************/

static void ExaPrn_WriteFltAnsToFill (const struct ExaPrn_Print *Print,
				      unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question)
  {
   char Id[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (Id,sizeof (Id),"Ans%010u",QstInd);
   HTM_TxtF ("<input type=\"text\" id=\"%s\" name=\"Ans\""
	     " size=\"11\" maxlength=\"%u\" value=\"%s\"",
	     Id,Qst_MAX_BYTES_FLOAT_ANSWER,
	     Print->PrintedQuestions[QstInd].StrAnswers);
   ExaPrn_WriteJSToUpdateExamPrint (Print,QstInd,Id,-1);
   HTM_Txt (" />");
  }

/*****************************************************************************/
/************** Write false / true answer when seeing a test ****************/
/*****************************************************************************/

static void ExaPrn_WriteTF_AnsToFill (const struct ExaPrn_Print *Print,
	                              unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question)
  {
   extern const char *Txt_TF_QST[2];
   char Id[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write selector for the answer *****/
   /* Initially user has not answered the question ==> initially all answers will be blank.
      If the user does not confirm the submission of their exam ==>
      ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
   snprintf (Id,sizeof (Id),"Ans%010u",QstInd);
   HTM_TxtF ("<select id=\"%s\" name=\"Ans\"",Id);
   ExaPrn_WriteJSToUpdateExamPrint (Print,QstInd,Id,-1);
   HTM_Txt (" />");
      HTM_OPTION (HTM_Type_STRING,"" ,
                  (Print->PrintedQuestions[QstInd].StrAnswers[0] == '\0') ? HTM_SELECTED :
                							    HTM_NO_ATTR,
                  "&nbsp;");
      HTM_OPTION (HTM_Type_STRING,"T",
                  (Print->PrintedQuestions[QstInd].StrAnswers[0] == 'T') ? HTM_SELECTED :
                							   HTM_NO_ATTR,
                  "%s",Txt_TF_QST[0]);
      HTM_OPTION (HTM_Type_STRING,"F",
                  (Print->PrintedQuestions[QstInd].StrAnswers[0] == 'F') ? HTM_SELECTED :
                							   HTM_NO_ATTR,
                  "%s",Txt_TF_QST[1]);
   HTM_Txt ("</select>");
  }

/*****************************************************************************/
/***** Write single or multiple choice answer when seeing an exam print ******/
/*****************************************************************************/

static void ExaPrn_WriteChoAnsToFill (const struct ExaPrn_Print *Print,
                                      unsigned QstInd,
                                      struct Qst_Question *Question)
  {
   unsigned NumOpt;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   HTM_Attributes_t UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION];
   char Id[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Change format of answers text *****/
   Qst_ChangeFormatAnswersText (Question);

   /***** Get indexes for this question from string *****/
   TstPrn_GetIndexesFromStr (Print->PrintedQuestions[QstInd].StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   TstPrn_GetAnswersFromStr (Print->PrintedQuestions[QstInd].StrAnswers,UsrAnswers);

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
	    /* Initially user has not answered the question ==> initially all answers will be blank.
	       If the user does not confirm the submission of their exam ==>
	       ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
	    HTM_TD_Begin ("class=\"LT\"");
	       snprintf (Id,sizeof (Id),"Ans%010u",QstInd);
	       HTM_TxtF ("<input type=\"%s\" id=\"%s_%u\" name=\"Ans\" value=\"%u\"",
			 Question->Answer.Type == Qst_ANS_UNIQUE_CHOICE ? "radio" :
									  "checkbox",
			 Id,NumOpt,Indexes[NumOpt]);
	       if ((UsrAnswers[Indexes[NumOpt]] & HTM_CHECKED))
		  HTM_Txt (" checked");
	       ExaPrn_WriteJSToUpdateExamPrint (Print,QstInd,Id,(int) NumOpt);
	       HTM_Txt (" />");
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_LABEL_Begin ("for=\"Ans%010u_%u\" class=\"Qst_TXT_%s\"",
	                        QstInd,NumOpt,The_GetSuffix ());
		  HTM_TxtF ("%c)&nbsp;",'a' + (char) NumOpt);
	       HTM_LABEL_End ();
	    HTM_TD_End ();

	    /***** Write the option text *****/
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_LABEL_Begin ("for=\"Ans%010u_%u\" class=\"Qst_TXT_%s\"",
	                        QstInd,NumOpt,The_GetSuffix ());
		  HTM_Txt (Question->Answer.Options[Indexes[NumOpt]].Text);
	       HTM_LABEL_End ();
	       Med_ShowMedia (&Question->Answer.Options[Indexes[NumOpt]].Media,
			      "Tst_MED_SHOW_CONT",
			      "Tst_MED_SHOW");
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************** Write text answer when seeing a test *******************/
/*****************************************************************************/

static void ExaPrn_WriteTxtAnsToFill (const struct ExaPrn_Print *Print,
	                              unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question)
  {
   char Id[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (Id,sizeof (Id),"Ans%010u",QstInd);
   HTM_TxtF ("<input type=\"text\" id=\"%s\" name=\"Ans\""
	     " size=\"40\" maxlength=\"%u\" value=\"%s\"",
	     Id,Qst_MAX_CHARS_ANSWERS_ONE_QST,
	     Print->PrintedQuestions[QstInd].StrAnswers);
   ExaPrn_WriteJSToUpdateExamPrint (Print,QstInd,Id,-1);
   HTM_Txt (" />");
  }

/*****************************************************************************/
/********************** Receive answer to an exam print **********************/
/*****************************************************************************/

static void ExaPrn_WriteJSToUpdateExamPrint (const struct ExaPrn_Print *Print,
	                                     unsigned QstInd,
	                                     const char *Id,int NumOpt)
  {
   if (NumOpt < 0)
      HTM_TxtF (" onchange=\"updateExamPrint('examprint','%s','Ans',"
			    "'act=%ld&ses=%s&SesCod=%ld&QstInd=%u',%u);",
		Id,
		Act_GetActCod (ActAnsExaPrn),Gbl.Session.Id,Print->SesCod,QstInd,
		(unsigned) Gbl.Prefs.Language);
   else	// NumOpt >= 0
      HTM_TxtF (" onclick=\"updateExamPrint('examprint','%s_%d','Ans',"
		           "'act=%ld&ses=%s&SesCod=%ld&QstInd=%u',%u);",
		Id,NumOpt,
		Act_GetActCod (ActAnsExaPrn),Gbl.Session.Id,Print->SesCod,QstInd,
	        (unsigned) Gbl.Prefs.Language);
   HTM_Txt (" return false;\"");	// return false is necessary to not submit form
  }

/*****************************************************************************/
/********************** Receive answer to an exam print **********************/
/*****************************************************************************/

void ExaPrn_ReceivePrintAnswer (void)
  {
   extern const char *Txt_You_dont_have_access_to_the_exam;
   extern const char *Txt_Continue;
   struct Exa_Exams Exams;
   struct ExaSes_Session Session;
   struct ExaPrn_Print Print;
   unsigned QstInd;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exams.Exam);
   ExaSes_ResetSession (&Session);

   /***** Get session code *****/
   Print.SesCod = ParCod_GetAndCheckPar (ParCod_Ses);

   /***** Get print data *****/
   Print.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   ExaPrn_GetPrintDataBySesCodAndUsrCod (&Print);
   if (Print.PrnCod <= 0)
      Err_WrongExamExit ();

   /***** Get session data *****/
   Session.SesCod = Print.SesCod;
   ExaSes_GetSessionDataByCod (&Session);
   if (Session.SesCod <= 0)
      Err_WrongExamExit ();
   Exams.SesCod = Session.SesCod;

   /***** Get exam data *****/
   Exams.Exam.ExaCod = Session.ExaCod;
   Exa_GetExamDataByCod (&Exams.Exam);
   if (Exams.Exam.ExaCod <= 0)
      Err_WrongExamExit ();
   if (Exams.Exam.CrsCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)
      Err_WrongExamExit ();

   /***** Get question index from form *****/
   QstInd = ExaPrn_GetParQstInd ();

   /***** Set log print code, action and question index *****/
   ExaLog_SetPrnCod (Print.PrnCod);
   ExaLog_SetAction (ExaLog_ANSWER_QUESTION);
   ExaLog_SetQstInd (QstInd);

   /***** Check if session if visible and open *****/
   switch (ExaSes_CheckIfICanAnswerThisSession (&Exams.Exam,&Session))
     {
      case Usr_CAN:
	 /***** Set log open to true ****/
	 ExaLog_SetIfCanAnswer (true);

	 /***** Get questions and current user's answers of exam print from database *****/
	 ExaPrn_GetPrintQuestionsFromDB (&Print);

	 /***** Get answers from form to assess a test *****/
	 ExaPrn_GetAnswerFromForm (&Print,QstInd);

	 /***** Update answer in database *****/
	 /* Compute question score and store in database */
	 ExaPrn_ComputeScoreAndStoreQuestionOfPrint (&Print,QstInd);

	 /* Update exam print in database */
	 Print.NumQsts.NotBlank = Exa_DB_GetNumQstsNotBlankInPrint (Print.PrnCod);
	 Print.Score.All = Exa_DB_ComputeTotalScoreOfPrint (Print.PrnCod);
	 Exa_DB_UpdatePrint (&Print);

	 /***** Show table with questions to answer *****/
	 ExaPrn_ShowTableWithQstsToFill (&Exams,&Print);
	 break;
      case Usr_CAN_NOT:	// Not accessible to answer
      default:
	 /***** Set log open to false ****/
	 ExaLog_SetIfCanAnswer (false);

	 /***** Show warning *****/
	 Ale_ShowAlert (Ale_INFO,Txt_You_dont_have_access_to_the_exam);

	 /***** Form to end/close this exam print *****/
	 Frm_BeginForm (ActEndExaPrn);
	    ExaSes_PutParsEdit (&Exams);
	    Btn_PutCreateButton (Txt_Continue);
	 Frm_EndForm ();
	 break;
     }
  }

/*****************************************************************************/
/******** Get questions and answers from form to assess an exam print ********/
/*****************************************************************************/

static void ExaPrn_GetAnswerFromForm (struct ExaPrn_Print *Print,unsigned QstInd)
  {
   /***** Get answers selected by user for this question *****/
   Par_GetParText ("Ans",Print->PrintedQuestions[QstInd].StrAnswers,
		     Qst_MAX_BYTES_ANSWERS_ONE_QST);  /* If answer type == T/F ==> " ", "T", "F"; if choice ==> "0", "2",... */
  }

/*****************************************************************************/
/********************* Get parameter with question index *********************/
/*****************************************************************************/

static unsigned ExaPrn_GetParQstInd (void)
  {
   long QstInd;

   if ((QstInd = Par_GetParLong ("QstInd")) < 0)	// In exams, question index should be 0, 1, 2, 3...
      Err_WrongQuestionIndexExit ();

   return (unsigned) QstInd;
  }

/*****************************************************************************/
/*********** Compute score of one question and store in database *************/
/*****************************************************************************/

static void ExaPrn_ComputeScoreAndStoreQuestionOfPrint (struct ExaPrn_Print *Print,
                                                        unsigned QstInd)
  {
   struct Qst_Question Question;
   char CurrentStrAnswersInDB[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];	// Answers selected by user

   /***** Compute question score *****/
   Qst_QstConstructor (&Question);
   Question.QstCod = Print->PrintedQuestions[QstInd].QstCod;
   Question.Answer.Type = ExaSet_GetAnswerType (Question.QstCod);
   ExaPrn_ComputeAnswerScore (&Print->PrintedQuestions[QstInd],&Question);
   Qst_QstDestructor (&Question);

   /***** If type is unique choice and the option (radio button) is checked
          ==> uncheck it by deleting answer *****/
   if (Question.Answer.Type == Qst_ANS_UNIQUE_CHOICE)
     {
      Exa_DB_GetAnswersFromQstInPrint (Print->PrnCod,Print->PrintedQuestions[QstInd].QstCod,
                                       CurrentStrAnswersInDB);
      if (!strcmp (Print->PrintedQuestions[QstInd].StrAnswers,CurrentStrAnswersInDB))
	{
	 /* The answer just clicked by user
	    is the same as the last one checked and stored in database */
	 Print->PrintedQuestions[QstInd].StrAnswers[0]    = '\0';	// Uncheck option
	 Print->PrintedQuestions[QstInd].Score            = 0;		// Clear question score
	}
     }

   /***** Store test question in database *****/
   Exa_DB_StoreOneQstOfPrint (Print,
			      QstInd);	// 0, 1, 2, 3...
  }

/*****************************************************************************/
/************* Write answers of a question when assessing a test *************/
/*****************************************************************************/

void ExaPrn_ComputeAnswerScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				struct Qst_Question *Question)
  {
   void (*ExaPrn_GetCorrectAndComputeAnsScore[Qst_NUM_ANS_TYPES]) (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                                   struct Qst_Question *Question) =
    {
     [Qst_ANS_INT            ] = ExaPrn_GetCorrectAndComputeIntAnsScore,
     [Qst_ANS_FLOAT          ] = ExaPrn_GetCorrectAndComputeFltAnsScore,
     [Qst_ANS_TRUE_FALSE     ] = ExaPrn_GetCorrectAndComputeTF_AnsScore,
     [Qst_ANS_UNIQUE_CHOICE  ] = ExaPrn_GetCorrectAndComputeChoAnsScore,
     [Qst_ANS_MULTIPLE_CHOICE] = ExaPrn_GetCorrectAndComputeChoAnsScore,
     [Qst_ANS_TEXT           ] = ExaPrn_GetCorrectAndComputeTxtAnsScore,
    };

   /***** Get correct answer and compute answer score depending on type *****/
   ExaPrn_GetCorrectAndComputeAnsScore[Question->Answer.Type] (PrintedQuestion,Question);
  }

/*****************************************************************************/
/******* Get correct answer and compute score for each type of answer ********/
/*****************************************************************************/

static void ExaPrn_GetCorrectAndComputeIntAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question)
  {
   /***** Get the numerical value of the correct answer,
          and compute score *****/
   ExaPrn_GetCorrectIntAnswerFromDB (Question);
   TstPrn_ComputeIntAnsScore (PrintedQuestion,Question);
  }

static void ExaPrn_GetCorrectAndComputeFltAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question)
  {
   /***** Get the numerical value of the minimum and maximum correct answers,
          and compute score *****/
   ExaPrn_GetCorrectFltAnswerFromDB (Question);
   TstPrn_ComputeFltAnsScore (PrintedQuestion,Question);
  }

static void ExaPrn_GetCorrectAndComputeTF_AnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question)
  {
   /***** Get answer true or false,
          and compute score *****/
   ExaPrn_GetCorrectTF_AnswerFromDB (Question);
   TstPrn_ComputeTF_AnsScore (PrintedQuestion,Question);
  }

static void ExaPrn_GetCorrectAndComputeChoAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question)
  {
   /***** Get correct options of test question from database,
          and compute score *****/
   ExaPrn_GetCorrectChoAnswerFromDB (Question);
   TstPrn_ComputeChoAnsScore (PrintedQuestion,Question);
  }

static void ExaPrn_GetCorrectAndComputeTxtAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question)
  {
   /***** Get correct text answers for this question from database,
          and compute score *****/
   ExaPrn_GetCorrectTxtAnswerFromDB (Question);
   TstPrn_ComputeTxtAnsScore (PrintedQuestion,Question);
  }

/*****************************************************************************/
/***************** Get correct answer for each type of answer ****************/
/*****************************************************************************/

static void ExaPrn_GetCorrectIntAnswerFromDB (struct Qst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Query database *****/
   Question->Answer.NumOptions = Exa_DB_GetQstAnswersTextFromSet (&mysql_res,Question->QstCod);

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get correct answer *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%ld",&Question->Answer.Integer) != 1)
      Err_WrongAnswerExit ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

static void ExaPrn_GetCorrectFltAnswerFromDB (struct Qst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumOpt;
   double Tmp;

   /***** Query database *****/
   Question->Answer.NumOptions = Exa_DB_GetQstAnswersTextFromSet (&mysql_res,Question->QstCod);

   /***** Check if number of rows is correct *****/
   if (Question->Answer.NumOptions != 2)
      Err_WrongAnswerExit ();

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

static void ExaPrn_GetCorrectTF_AnswerFromDB (struct Qst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Query database *****/
   Question->Answer.NumOptions = Exa_DB_GetQstAnswersTextFromSet (&mysql_res,Question->QstCod);

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get answer *****/
   row = mysql_fetch_row (mysql_res);
   Question->Answer.TF = row[0][0];

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

static void ExaPrn_GetCorrectChoAnswerFromDB (struct Qst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumOpt;

   /***** Query database *****/
   Question->Answer.NumOptions = Exa_DB_GetQstAnswersCorrFromSet (&mysql_res,Question->QstCod);
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

static void ExaPrn_GetCorrectTxtAnswerFromDB (struct Qst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumOpt;

   /***** Query database *****/
   Question->Answer.NumOptions = Exa_DB_GetQstAnswersTextFromSet (&mysql_res,Question->QstCod);

   /***** Get text and correctness of answers for this question from database (one row per answer) *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      /***** Get next answer *****/
      row = mysql_fetch_row (mysql_res);

      /***** Allocate memory for text in this choice answer *****/
      if (!Qst_AllocateTextChoiceAnswer (Question,NumOpt))
	 /* Abort on error */
	 Ale_ShowAlertsAndExit ();

      /***** Copy answer text (row[0]) ******/
      Str_Copy (Question->Answer.Options[NumOpt].Text,row[0],
                Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);
     }

   /***** Change format of answers text *****/
   Qst_ChangeFormatAnswersText (Question);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
