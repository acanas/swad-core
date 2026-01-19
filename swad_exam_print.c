// swad_exam_print.c: exam prints (each copy of an exam in a session for a student)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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
#include <stdlib.h>		// For free
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
#include "swad_question.h"
#include "swad_question_choice.h"
#include "swad_question_database.h"
#include "swad_question_float.h"
#include "swad_question_int.h"
#include "swad_question_text.h"
#include "swad_question_tf.h"
#include "swad_view.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

//-----------------------------------------------------------------------------

static void ExaPrn_GetPrintDataFromRow (MYSQL_RES **mysql_res,
                                        struct ExaPrn_Print *Print,
                                        Exi_Exist_t PrintExists);

static void ExaPrn_GetQuestionsForNewPrintFromDB (struct ExaPrn_Print *Print,long ExaCod);
static unsigned ExaPrn_GetSomeQstsFromSetToPrint (struct ExaPrn_Print *Print,
                                                  struct ExaSet_Set *Set,
                                                  unsigned *NumQstsInPrint);
static void ExaPrn_GenerateChoiceIndexes (struct Qst_PrintedQuestion *PrintedQst,
					  Qst_Shuffle_t Shuffle);
static void ExaPrn_CreatePrint (struct ExaPrn_Print *Print,
				ExaPrn_UpdateDates_t UpdateDates);

static void ExaPrn_ShowPrintToFill (struct Exa_Exams *Exams,
      				    const struct ExaSes_Session *Session,
                                    const struct ExaPrn_Print *Print);

static void ExaPrn_ShowQstsAndAnssToFill (struct Exa_Exams *Exams,
					  const struct ExaSes_Session *Session,
				          const struct ExaPrn_Print *Print);

static void ExaPrn_WriteQstAndAnsToFill (const struct ExaPrn_Print *Print,
                                         unsigned QstInd,
                                         struct Qst_Question *Qst);

static void ExaPrn_WriteAnswersToFill (const struct ExaPrn_Print *Print,
                                       unsigned QstInd,
                                       struct Qst_Question *Qst);

//-----------------------------------------------------------------------------

static void ExaPrn_GetAnswerFromForm (struct ExaPrn_Print *Print,unsigned QstInd);
static void ExaPrn_ComputeScoreAndStoreQuestionOfPrint (struct ExaPrn_Print *Print,
							unsigned QstInd);

/*****************************************************************************/
/**************************** Reset exam print *******************************/
/*****************************************************************************/

void ExaPrn_ResetPrint (struct ExaPrn_Print *Print)
  {
   Print->PrnCod = -1L;
   Print->SesCod = -1L;
   Print->UsrCod = -1L;
   Print->EnUsrCod[0] = '\0';

   Print->TimeUTC[Dat_STR_TIME] =
   Print->TimeUTC[Dat_END_TIME] = (time_t) 0;
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
/******** Show a printout of the exam to a student to be filled online *******/
/*****************************************************************************/

void ExaPrn_ShowPrintToStdToFill (void)
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
	 /***** Create print or get existing print *****/
	 ExaPrn_GetQstsPrint (&Exams,&Session,&Gbl.Usrs.Me.UsrDat,&Print,
			      ExaPrn_UPDATE_DATES);

	 /***** Show exam print to be answered *****/
	 ExaPrn_ShowPrintToFill (&Exams,&Session,&Print);
	 break;
      case Usr_CAN_NOT:	// Session not open or accessible
      default:
	 /***** Show warning *****/
	 Ale_ShowAlert (Ale_INFO,Txt_You_dont_have_access_to_the_exam);
	 break;
     }
  }

/*****************************************************************************/
/*** Get questions of a printout. If printout does not exists, create it. ****/
/*****************************************************************************/

void ExaPrn_GetQstsPrint (struct Exa_Exams *Exams,
			  const struct ExaSes_Session *Session,
			  struct Usr_Data *UsrDat,
			  struct ExaPrn_Print *Print,
			  ExaPrn_UpdateDates_t UpdateDates)
  {
   /***** Set basic data of exam print *****/
   Print->SesCod = Session->SesCod;
   Print->UsrCod = UsrDat->UsrCod;

   /***** Get exam print data from database *****/
   ExaPrn_GetPrintDataBySesCodAndUsrCod (Print);

   if (Print->PrnCod <= 0)	// Exam print does not exists ==> create it
     {
      /***** When creating a new exam printout, its data has been reset,
             so some of the basic data must be copied again *****/
      Print->SesCod = Session->SesCod;
      Print->UsrCod = UsrDat->UsrCod;

      /***** Get questions from database *****/
      ExaPrn_GetQuestionsForNewPrintFromDB (Print,Exams->Exam.ExaCod);

      if (Print->NumQsts.All)
	{
	 /***** Create new exam print in database *****/
	 ExaPrn_CreatePrint (Print,UpdateDates);	// Create and start exam print?

	 /***** Set log print code and action *****/
	 if (UpdateDates == ExaPrn_UPDATE_DATES)
	   {
	    ExaLog_SetPrnCod (Print->PrnCod);
	    ExaLog_SetAction (ExaLog_START_EXAM);
	    ExaLog_SetIfCanAnswer (Usr_CAN);
	   }
	}
     }
   else				// Exam print exists
     {
      /***** Get questions and current user's answers from database *****/
      ExaPrn_GetPrintQuestionsFromDB (Print);

      /***** Set log print code and action *****/
      if (UpdateDates == ExaPrn_UPDATE_DATES)
	{
	 ExaLog_SetPrnCod (Print->PrnCod);
	 ExaLog_SetAction (ExaLog_RESUME_EXAM);
	 ExaLog_SetIfCanAnswer (Usr_CAN);
	}
     }

   /***** Copy encrypted user code *****/
   Str_Copy (Print->EnUsrCod,UsrDat->EnUsrCod,sizeof (Print->EnUsrCod) - 1);
  }

/*****************************************************************************/
/**************** Get data of an exam print using print code *****************/
/*****************************************************************************/

void ExaPrn_GetPrintDataByPrnCod (struct ExaPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   Exi_Exist_t PrintExists;

   /***** Make database query *****/
   PrintExists = Exa_DB_GetPrintDataByPrnCod (&mysql_res,Print->PrnCod);

   /***** Get data of print *****/
   ExaPrn_GetPrintDataFromRow (&mysql_res,Print,PrintExists);
  }

/*****************************************************************************/
/******** Get data of an exam print using session code and user code *********/
/*****************************************************************************/

void ExaPrn_GetPrintDataBySesCodAndUsrCod (struct ExaPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   Exi_Exist_t PrintExists;

   /***** Make database query *****/
   PrintExists = Exa_DB_GetPrintDataBySesCodAndUsrCod (&mysql_res,
                                                       Print->SesCod,
                                                       Print->UsrCod);

   /***** Get data of print *****/
   ExaPrn_GetPrintDataFromRow (&mysql_res,Print,PrintExists);
  }

/*****************************************************************************/
/************************* Get assignment data *******************************/
/*****************************************************************************/

static void ExaPrn_GetPrintDataFromRow (MYSQL_RES **mysql_res,
                                        struct ExaPrn_Print *Print,
                                        Exi_Exist_t PrintExists)
  {
   MYSQL_ROW row;

   switch (PrintExists)
     {
      case Exi_EXISTS:
	 /* Get next row from result */
	 row = mysql_fetch_row (*mysql_res);
	 /*
	 row[0]: PrnCod
	 row[1]: SesCod
	 row[2]: UsrCod
	 row[3]: UNIX_TIMESTAMP(StartTime)
	 row[4]: UNIX_TIMESTAMP(EndTime)
	 row[5]: NumQsts
	 row[6]: NumQstsNotBlank
	 row[7]: Score
	 */
	 /* Get print code (row[0]), session code (row[1]) and user code (row[2]) */
	 Print->PrnCod = Str_ConvertStrCodToLongCod (row[0]);
	 Print->SesCod = Str_ConvertStrCodToLongCod (row[1]);
	 Print->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

	 /* Get date-time (row[3] and row[4] hold UTC date-time) */
	 Print->TimeUTC[Dat_STR_TIME] = Dat_GetUNIXTimeFromStr (row[3]);
	 Print->TimeUTC[Dat_END_TIME] = Dat_GetUNIXTimeFromStr (row[4]);

	 /* Get number of questions (row[5]) and number of questions not blank (row[6]) */
	 if (sscanf (row[5],"%u",&Print->NumQsts.All) != 1)
	    Print->NumQsts.All = 0;
	 if (sscanf (row[6],"%u",&Print->NumQsts.NotBlank) != 1)
	    Print->NumQsts.NotBlank = 0;

	 /* Get score (row[7]) */
	 Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
	 if (sscanf (row[7],"%lf",&Print->Score.All) != 1)
	    Print->Score.All = 0.0;
	 Str_SetDecimalPointToLocal ();	// Return to local system
	 break;
      case Exi_DOES_NOT_EXIST:
      default:
	 ExaPrn_ResetPrint (Print);
	 break;
     }

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
   Qst_Shuffle_t Shuffle;

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
      Print->PrintedQsts[*NumQstsInPrint].QstCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Set set of questions */
      Print->PrintedQsts[*NumQstsInPrint].SetCod = Set->SetCod;

      /* Get answer type (row[1]) */
      AnswerType = Qst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);

      /* Get shuffle (row[2]) */
      Shuffle = Qst_GetShuffleFromYN (row[2][0]);

      /* Set indexes of answers */
      switch (AnswerType)
	{
	 case Qst_ANS_INT:
	 case Qst_ANS_FLOAT:
	 case Qst_ANS_TRUE_FALSE:
	 case Qst_ANS_TEXT:
	    Print->PrintedQsts[*NumQstsInPrint].StrIndexes[0] = '\0';
	    break;
	 case Qst_ANS_UNIQUE_CHOICE:
	 case Qst_ANS_MULTIPLE_CHOICE:
            /* If answer type is unique or multiple option,
               generate indexes of answers depending on shuffle */
	    ExaPrn_GenerateChoiceIndexes (&Print->PrintedQsts[*NumQstsInPrint],
					  Shuffle);
	    break;
	 default:
	    break;
	}

      /* Reset user's answers.
         Initially user has not answered the question ==> initially all answers will be blank.
         If the user does not confirm the submission of their exam ==>
         ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
      Print->PrintedQsts[*NumQstsInPrint].Answer.Str[0] = '\0';

      /* Reset score of this question in print */
      Print->PrintedQsts[*NumQstsInPrint].Answer.Score = 0.0;
     }

   return NumQstsInSet;
  }

/*****************************************************************************/
/*************** Generate choice indexes depending on shuffle ****************/
/*****************************************************************************/

static void ExaPrn_GenerateChoiceIndexes (struct Qst_PrintedQuestion *PrintedQst,
					  Qst_Shuffle_t Shuffle)
  {
   struct Qst_Question Qst;
   unsigned NumOpt;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned Index;
   Err_SuccessOrError_t ErrorInIndex;
   char StrInd[1 + Cns_MAX_DIGITS_UINT + 1];

   /***** Create question *****/
   Qst_QstConstructor (&Qst);
   Qst.QstCod = PrintedQst->QstCod;

      /***** Get answers of question from database *****/
      Qst.Answer.NumOpts = Qst_DB_GetQstIndexesFromQst (&mysql_res,"exa_set_answers",
							Qst.QstCod,Shuffle);

      /***** Reset string of indexes *****/
      PrintedQst->StrIndexes[0] = '\0';

      for (NumOpt = 0;
	   NumOpt < Qst.Answer.NumOpts;
	   NumOpt++)
	{
	 /***** Get next answer *****/
	 row = mysql_fetch_row (mysql_res);
	 /*
	 row[0] AnsInd
	 */
	 /***** Assign index (row[0]).
		Index is 0,1,2,3... if no shuffle
		or 1,3,0,2... (example) if shuffle *****/
	 ErrorInIndex = Err_SUCCESS;
	 if (sscanf (row[0],"%u",&Index) == 1)
	   {
	    if (Index >= Qst_MAX_OPTS_PER_QST)
	       ErrorInIndex = Err_ERROR;
	   }
	 else
	    ErrorInIndex = Err_ERROR;
	 if (ErrorInIndex == Err_ERROR)
	    Err_WrongAnswerIndexExit ();

	 /***** Add index to string *****/
	 if (NumOpt == 0)
	    snprintf (StrInd,sizeof (StrInd),"%u",Index);
	 else
	    snprintf (StrInd,sizeof (StrInd),",%u",Index);
	 Str_Concat (PrintedQst->StrIndexes,StrInd,
		     sizeof (PrintedQst->StrIndexes) - 1);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

   /***** Destroy question *****/
   Qst_QstDestructor (&Qst);
  }

/*****************************************************************************/
/***************** Create new blank exam print in database *******************/
/*****************************************************************************/

static void ExaPrn_CreatePrint (struct ExaPrn_Print *Print,
				ExaPrn_UpdateDates_t UpdateDates)
  {
   unsigned QstInd;

   /***** Insert new exam print into database *****/
   Print->PrnCod = Exa_DB_CreatePrint (Print,UpdateDates);

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
	 if ((Print->PrintedQsts[QstInd].QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
	    Err_WrongQuestionExit ();
	 if ((Print->PrintedQsts[QstInd].SetCod = Str_ConvertStrCodToLongCod (row[1])) <= 0)
	    Err_WrongSetExit ();

         /* Get score (row[2]) */
	 Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
         if (sscanf (row[2],"%lf",&Print->PrintedQsts[QstInd].Answer.Score) != 1)
            Err_ShowErrorAndExit ("Wrong question score.");
         Str_SetDecimalPointToLocal ();	// Return to local system

	 /* Get indexes for this question (row[3])
	    and answers selected by user for this question (row[4]) */
	 Str_Copy (Print->PrintedQsts[QstInd].StrIndexes,row[3],
		   sizeof (Print->PrintedQsts[QstInd].StrIndexes) - 1);
	 Str_Copy (Print->PrintedQsts[QstInd].Answer.Str,row[4],
		   sizeof (Print->PrintedQsts[QstInd].Answer.Str) - 1);
	}

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (Print->NumQsts.All > ExaPrn_MAX_QUESTIONS_PER_EXAM_PRINT)
      Err_ShowErrorAndExit ("Too many questions.");
  }

/*****************************************************************************/
/******************** Show an exam print to be answered **********************/
/*****************************************************************************/

static void ExaPrn_ShowPrintToFill (struct Exa_Exams *Exams,
      				    const struct ExaSes_Session *Session,
                                    const struct ExaPrn_Print *Print)
  {
   extern const char *Hlp_ASSESSMENT_Exams_answer_exam;

   /***** Begin box *****/
   Box_BoxBegin (Exams->Exam.Title,NULL,NULL,
		 Hlp_ASSESSMENT_Exams_answer_exam,Box_NOT_CLOSABLE);

      /***** Heading *****/
      Exa_Header (Exams->Exam.ExaCod,&Gbl.Usrs.Me.UsrDat,Session->ShowPhotos);

      /***** Show table with questions to answer *****/
      if (Print->NumQsts.All)
	{
	 HTM_DIV_Begin ("id=\"examprint\" class=\"Exa_QSTS\"");	// Used for AJAX based refresh
	    ExaPrn_ShowQstsAndAnssToFill (Exams,Session,Print);
	 HTM_DIV_End ();					// Used for AJAX based refresh
	}

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********* Show the main part (table) of an exam print to be answered ********/
/*****************************************************************************/

static void ExaPrn_ShowQstsAndAnssToFill (struct Exa_Exams *Exams,
					  const struct ExaSes_Session *Session,
				          const struct ExaPrn_Print *Print)
  {
   unsigned QstInd;
   struct Qst_Question Qst;

   /***** Write questions in columns *****/
   HTM_DIV_Begin ("class=\"Exa_COLS_%u\"",Session->NumCols);

      /***** Write one row for each question *****/
      for (QstInd = 0;
	   QstInd < Print->NumQsts.All;
	   QstInd++)
	{
	 /* Create question */
	 Qst_QstConstructor (&Qst);
	 Qst.QstCod = Print->PrintedQsts[QstInd].QstCod;

	    /* Get question from database */
	    ExaSet_GetQstDataFromDB (&Qst);

	    /* Write question and answers */
	    ExaPrn_WriteQstAndAnsToFill (Print,QstInd,&Qst);

	 /* Destroy question */
	 Qst_QstDestructor (&Qst);
	}

   /***** End list of questions *****/
   HTM_DIV_End ();

   /***** Form to end/close this exam print *****/
   Frm_BeginForm (ActEndExaPrn);
      Exa_PutPars (Exams);
      Btn_PutButton (Btn_DONE,NULL);
   Frm_EndForm ();
  }

/*****************************************************************************/
/****** Write a row of an exam print, with one question and its answer *******/
/*****************************************************************************/

static void ExaPrn_WriteQstAndAnsToFill (const struct ExaPrn_Print *Print,
                                         unsigned QstInd,
                                         struct Qst_Question *Qst)
  {
   static struct ExaSet_Set CurrentSet =
     {
      .ExaCod = -1L,
      .SetCod = -1L,
      .SetInd = 0,
      .NumQstsToPrint = 0,
      .Title[0] = '\0'
     };

   /***** If this is the first question *****/
   if (QstInd == 0)
      CurrentSet.SetCod = -1L;	// Reset current set

   if (Print->PrintedQsts[QstInd].SetCod != CurrentSet.SetCod)
     {
      /***** Get data of this set *****/
      CurrentSet.SetCod = Print->PrintedQsts[QstInd].SetCod;
      ExaSet_GetSetDataByCod (&CurrentSet);

      /***** Title for this set *****/
      HTM_DIV_Begin ("class=\"Exa_COL_SPAN %s\"",The_GetColorRows ());
	 ExaSet_WriteSetTitle (&CurrentSet);
      HTM_DIV_End ();
     }

   /***** Begin row *****/
   HTM_DIV_Begin ("class=\"Exa_CONTAINER\"");

      /***** Number of question and answer type *****/
      HTM_DIV_Begin ("class=\"Exa_LEFT\"");
	 Lay_WriteIndex (QstInd + 1,"BIG_INDEX");
	 Qst_WriteAnswerType (Qst->Answer.Type,Qst->Validity);
      HTM_DIV_End ();

      /***** Stem, media and answers *****/
      HTM_DIV_Begin ("class=\"Exa_RIGHT\"");

	 /* Stem */
	 Qst_WriteQstStem (Qst->Stem,"Qst_TXT",HidVis_VISIBLE);

	 /* Media */
	 Med_ShowMedia (&Qst->Media,"Tst_MED_SHOW_CONT","Tst_MED_SHOW");

	 /* Answers */
	 Frm_BeginFormNoAction ();	// Form that can not be submitted, to avoid enter key to send it
	    ExaPrn_WriteAnswersToFill (Print,QstInd,Qst);
	 Frm_EndForm ();

      HTM_DIV_End ();

   /***** End row *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/***************** Write answers of a question to fill them ******************/
/*****************************************************************************/

static void ExaPrn_WriteAnswersToFill (const struct ExaPrn_Print *Print,
                                       unsigned QstInd,
                                       struct Qst_Question *Qst)
  {
   void (*ExaPrn_WriteAnsToFill[Qst_NUM_ANS_TYPES]) (const struct ExaPrn_Print *Print,
                                                     unsigned QstInd,
                                                     struct Qst_Question *Qst) =
    {
     [Qst_ANS_INT            ] = QstInt_WriteExaFillAns,
     [Qst_ANS_FLOAT          ] = QstFlt_WriteExaFillAns,
     [Qst_ANS_TRUE_FALSE     ] = QstTF__WriteExaFillAns,
     [Qst_ANS_UNIQUE_CHOICE  ] = QstCho_WriteExaFillAns,
     [Qst_ANS_MULTIPLE_CHOICE] = QstCho_WriteExaFillAns,
     [Qst_ANS_TEXT           ] = QstTxt_WriteExaFillAns,
    };

   /***** Write answers *****/
   ExaPrn_WriteAnsToFill[Qst->Answer.Type] (Print,QstInd,Qst);
  }

/*****************************************************************************/
/********************** Receive answer to an exam print **********************/
/*****************************************************************************/

void ExaPrn_WriteJSToUpdateExamPrint (const struct ExaPrn_Print *Print,
				      unsigned QstInd,
				      const char *Id,int NumOpt)
  {
   char *Pars;

   if (asprintf (&Pars,"act=%ld&ses=%s&SesCod=%ld&QstInd=%u",
		 Act_GetActCod (ActAnsExaPrn),Gbl.Session.Id,
		 Print->SesCod,QstInd) < 0)
      Err_NotEnoughMemoryExit ();

   if (NumOpt < 0)
      HTM_TxtF (" onchange=\"updateExamPrint('examprint','%s','Ans','%s',%u);",
		Id,Pars,(unsigned) Gbl.Prefs.Language);
   else	// NumOpt >= 0
      HTM_TxtF (" onclick=\"updateExamPrint('examprint','%s_%d','Ans','%s',%u);",
		Id,NumOpt,Pars,
	        (unsigned) Gbl.Prefs.Language);
   HTM_Txt (" return false;\"");	// return false is necessary to not submit form

   free (Pars);
  }

/*****************************************************************************/
/********************** Receive answer to an exam print **********************/
/*****************************************************************************/

void ExaPrn_ReceiveAnswer (void)
  {
   extern const char *Txt_You_dont_have_access_to_the_exam;
   struct Exa_Exams Exams;
   struct ExaSes_Session Session;
   struct ExaPrn_Print Print;
   unsigned QstInd;
   Usr_Can_t ICanAnswerThisSession;

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
   Str_Copy (Print.EnUsrCod,Gbl.Usrs.Me.UsrDat.EnUsrCod,
	     sizeof (Print.EnUsrCod) - 1);

   /***** Get session data *****/
   Session.SesCod = Print.SesCod;
   ExaSes_GetSessionDataByCod (&Session);
   if (Session.SesCod <= 0)
      Err_WrongExamExit ();
   Exams.SesCod.Sel =
   Exams.SesCod.Par = Session.SesCod;

   /***** Get exam data *****/
   Exams.Exam.ExaCod = Session.ExaCod;
   Exa_GetExamDataByCod (&Exams.Exam);
   if (Exams.Exam.ExaCod <= 0)
      Err_WrongExamExit ();
   if (Exams.Exam.HieCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)
      Err_WrongExamExit ();

   /***** Get question index from form *****/
   QstInd = ExaPrn_GetParQstInd ();

   /***** Set log print code, action and question index *****/
   ExaLog_SetPrnCod (Print.PrnCod);
   ExaLog_SetAction (ExaLog_ANSWER_QUESTION);
   ExaLog_SetQstInd (QstInd);

   /***** Check if session if visible and open *****/
   ICanAnswerThisSession = ExaSes_CheckIfICanAnswerThisSession (&Exams.Exam,&Session);
   ExaLog_SetIfCanAnswer (ICanAnswerThisSession);

   switch (ICanAnswerThisSession)
     {
      case Usr_CAN:
	 /***** Update answer and print in database *****/
	 ExaPrn_UpdateAnswerAndPrint (&Print,QstInd);

	 /***** Show table with questions to answer *****/
	 ExaPrn_ShowQstsAndAnssToFill (&Exams,&Session,&Print);
	 break;
      case Usr_CAN_NOT:	// Not accessible to answer
      default:
	 /***** Show warning *****/
	 Ale_ShowAlert (Ale_INFO,Txt_You_dont_have_access_to_the_exam);

	 /***** Form to end/close this exam print *****/
	 Frm_BeginForm (ActEndExaPrn);
	    Exa_PutPars (&Exams);
	    Btn_PutButton (Btn_CONTINUE,NULL);
	 Frm_EndForm ();
	 break;
     }
  }

/*****************************************************************************/
/********************* Get parameter with question index *********************/
/*****************************************************************************/

unsigned ExaPrn_GetParQstInd (void)
  {
   long QstInd;

   if ((QstInd = Par_GetParLong ("QstInd")) < 0)	// In exams, question index should be 0, 1, 2, 3...
      Err_WrongQuestionIndexExit ();

   return (unsigned) QstInd;
  }

/*****************************************************************************/
/************************* Update answer and exam print **********************/
/*****************************************************************************/

void ExaPrn_UpdateAnswerAndPrint (struct ExaPrn_Print *Print,unsigned QstInd)
  {
   /***** Get questions and current user's answers of exam print from database *****/
   ExaPrn_GetPrintQuestionsFromDB (Print);

   /***** Get answer to the specified question from form *****/
   ExaPrn_GetAnswerFromForm (Print,QstInd);

   /***** Update answer in database *****/
   /* Compute question score and store in database */
   ExaPrn_ComputeScoreAndStoreQuestionOfPrint (Print,QstInd);

   /* Update exam print in database */
   Print->NumQsts.NotBlank = Exa_DB_GetNumQstsNotBlankInPrint (Print->PrnCod);
   Print->Score.All = Exa_DB_ComputeTotalScoreOfPrint (Print->PrnCod);
   Exa_DB_UpdatePrint (Print);
  }

/*****************************************************************************/
/* Get answer given by user for a given question from form in an exam print **/
/*****************************************************************************/

static void ExaPrn_GetAnswerFromForm (struct ExaPrn_Print *Print,unsigned QstInd)
  {
   Par_GetParText ("Ans",Print->PrintedQsts[QstInd].Answer.Str,
		   Qst_MAX_BYTES_ANSWERS_ONE_QST);  /* If answer type == T/F ==> "", "T", "F"; if choice ==> "0", "2",... */
  }

/*****************************************************************************/
/*********** Compute score of one question and store in database *************/
/*****************************************************************************/

static void ExaPrn_ComputeScoreAndStoreQuestionOfPrint (struct ExaPrn_Print *Print,
							unsigned QstInd)
  {
   struct Qst_Question Qst;
   char CurrentStrAnswersInDB[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];	// Answers selected by user

   /***** Create question *****/
   Qst_QstConstructor (&Qst);
   Qst.QstCod = Print->PrintedQsts[QstInd].QstCod;
   Qst.Answer.Type = ExaSet_GetAnswerType (Qst.QstCod);

      /***** Compute question score *****/
      Qst_ComputeAnswerScore ("exa_set_answers",&Print->PrintedQsts[QstInd],&Qst);

      /***** If type is unique choice and the option (radio button) is checked
	     ==> uncheck it by deleting answer *****/
      if (Qst.Answer.Type == Qst_ANS_UNIQUE_CHOICE)
	{
	 Exa_DB_GetAnswersFromQstInPrint (Print->PrnCod,
					  Print->PrintedQsts[QstInd].QstCod,
					  CurrentStrAnswersInDB);
	 if (!strcmp (Print->PrintedQsts[QstInd].Answer.Str,CurrentStrAnswersInDB))
	   {
	    /* The answer just clicked by user
	       is the same as the last one checked and stored in database */
	    Print->PrintedQsts[QstInd].Answer.Str[0] = '\0';	// Uncheck option
	    Print->PrintedQsts[QstInd].Answer.Score  = 0;		// Clear question score
	   }
	}

   /***** Destroy question *****/
   Qst_QstDestructor (&Qst);

   /***** Store question in database *****/
   Exa_DB_StoreOneQstOfPrint (Print,
			      QstInd);	// 0, 1, 2, 3...

  }
