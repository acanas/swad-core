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

static void ExaPrn_GetQuestionsForNewPrintFromDB (struct Exa_Exam *Exam,
	                                          struct ExaPrn_Print *Print);
static void ExaPrn_ShowQuestionsFromSet (struct ExaPrn_Print *Print,
                                         struct ExaSet_Set *Set);

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
/******************* Generate print of an exam in an event *******************/
/*****************************************************************************/

void ExaPrn_ShowNewExamPrint (void)
  {
   extern const char *Hlp_ASSESSMENT_Exams;
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

   /***** Get questions from database *****/
   ExaPrn_GetQuestionsForNewPrintFromDB (&Exam,&Print);

   /***** End table *****/
   HTM_TABLE_End ();
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

   /***** Show table with sets *****/
   if (NumSets)
      /***** Write rows *****/
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
	 ExaPrn_ShowQuestionsFromSet (Print,&Set);
	}

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Show questions from a set **************************/
/*****************************************************************************/

static void ExaPrn_ShowQuestionsFromSet (struct ExaPrn_Print *Print,
                                         struct ExaSet_Set *Set)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQsts;
   unsigned NumQst;
   long QstCod;
   Tst_AnswerType_t AnswerType;
   bool Shuffle;

   /***** Get questions from database *****/
   NumQsts = (unsigned)
	     DB_QuerySELECT (&mysql_res,"can not get questions from set",
			     "SELECT tst_questions.QstCod,"	// row[0]
			            "tst_questions.AnsType,"	// row[1]
			            "tst_questions.Shuffle"	// row[2]
	                     " FROM exa_questions,tst_questions"
			     " WHERE exa_questions.setCod=%ld"
	                     " AND exa_questions.QstCod=tst_questions.QstCod"
			     " ORDER BY RAND(NOW())"
			     " LIMIT %u",
			     Set->SetCod,
			     Set->NumQstsToPrint);

   /***** Questions in this set *****/
   for (NumQst = 0;
	NumQst < NumQsts;
	NumQst++)
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
      QstCod = Str_ConvertStrCodToLongCod (row[0]);

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
	    Print->PrintedQuestions[NumQst].StrIndexes[0] = '\0';
	    break;
	 case Tst_ANS_UNIQUE_CHOICE:
	 case Tst_ANS_MULTIPLE_CHOICE:
            /* If answer type is unique or multiple option,
               generate indexes of answers depending on shuffle */
	    Tst_GenerateChoiceIndexesDependingOnShuffle (&Print->PrintedQuestions[NumQst],Shuffle);
	    break;
	 default:
	    break;
	}

      /* Reset user's answers.
         Initially user has not answered the question ==> initially all the answers will be blank.
         If the user does not confirm the submission of their exam ==>
         ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
      Print->PrintedQuestions[NumQst].StrAnswers[0] = '\0';

      /* Begin row for this question */
      HTM_TR_Begin (NULL);

      /* Title */
      HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TxtF ("Pregunta %ld",QstCod);
      HTM_TD_End ();

      /* Number of questions to appear in exam print */
      HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_Txt ("Enunciado y respuestas");
      HTM_TD_End ();

      /* End title for this question */
      HTM_TR_End ();
     }
  }
