// swad_question.c: test/exam/game questions

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
#include "swad_question_import.h"
// #include "swad_tag_database.h"
#include "swad_test.h"
// #include "swad_test_config.h"
// #include "swad_test_print.h"
// #include "swad_test_visibility.h"
// #include "swad_theme.h"
// #include "swad_user.h"
// #include "swad_xml.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

// strings are limited to Qst_MAX_BYTES_ANSWER_TYPE characters
const char *Qst_StrAnswerTypesXML[Qst_NUM_ANS_TYPES] =
  {
   [Qst_ANS_INT            ] = "int",
   [Qst_ANS_FLOAT          ] = "float",
   [Qst_ANS_TRUE_FALSE     ] = "TF",
   [Qst_ANS_UNIQUE_CHOICE  ] = "uniqueChoice",
   [Qst_ANS_MULTIPLE_CHOICE] = "multipleChoice",
   [Qst_ANS_TEXT           ] = "text",
  };

const char *Qst_DB_StrAnswerTypes[Qst_NUM_ANS_TYPES] =
  {
   [Qst_ANS_INT            ] = "int",
   [Qst_ANS_FLOAT          ] = "float",
   [Qst_ANS_TRUE_FALSE     ] = "true_false",
   [Qst_ANS_UNIQUE_CHOICE  ] = "unique_choice",
   [Qst_ANS_MULTIPLE_CHOICE] = "multiple_choice",
   [Qst_ANS_TEXT           ] = "text",
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
/***************** List several test questions for edition *******************/
/*****************************************************************************/

void Qst_ListQuestionsToEdit (void)
  {
   struct Qst_Questions Questions;
   MYSQL_RES *mysql_res;

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Get parameters, query the database and list the questions *****/
   if (Tst_GetParamsTst (&Questions,Tst_EDIT_QUESTIONS))	// Get parameters from the form
     {
      /***** Get question codes from database *****/
      Qst_GetQuestions (&Questions,&mysql_res);	// Query database
      if (Questions.NumQsts)
        {
	 /* Contextual menu */
	 if (QstImp_GetCreateXMLParamFromForm ())
	   {
            Mnu_ContextMenuBegin ();
            QstImp_CreateXML (Questions.NumQsts,mysql_res);	// Create XML file with exported questions...
							// ...and put a link to download it
            Mnu_ContextMenuEnd ();
	   }

	 /* Show the table with the questions */
         Qst_ListOneOrMoreQstsForEdition (&Questions,mysql_res);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /* Show the form again */
      Qst_ShowFormRequestEditQsts (&Questions);

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/************ List several test questions for selection for exam *************/
/*****************************************************************************/

void Qst_ListQuestionsToSelectForExamSet (struct Exa_Exams *Exams)
  {
   struct Qst_Questions Questions;
   MYSQL_RES *mysql_res;

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Get parameters, query the database and list the questions *****/
   if (Tst_GetParamsTst (&Questions,Tst_SELECT_QUESTIONS_FOR_EXAM))	// Get parameters from the form
     {
      Qst_GetQuestions (&Questions,&mysql_res);	// Query database
      if (Questions.NumQsts)
	 /* Show the table with the questions */
         Qst_ListOneOrMoreQstsForSelectionForExamSet (Exams,Questions.NumQsts,mysql_res);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /* Show the form again */
      Qst_ShowFormRequestSelectQstsForExamSet (Exams,&Questions);

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/************ List several test questions for selection for game *************/
/*****************************************************************************/

void Qst_ListQuestionsToSelectForGame (struct Gam_Games *Games)
  {
   struct Qst_Questions Questions;
   MYSQL_RES *mysql_res;

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Get parameters, query the database and list the questions *****/
   if (Tst_GetParamsTst (&Questions,Tst_SELECT_QUESTIONS_FOR_GAME))	// Get parameters from the form
     {
      Qst_GetQuestions (&Questions,&mysql_res);	// Query database
      if (Questions.NumQsts)
	 /* Show the table with the questions */
         Qst_ListOneOrMoreQstsForSelectionForGame (Games,Questions.NumQsts,mysql_res);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /* Show the form again */
      Qst_ShowFormRequestSelectQstsForGame (Games,&Questions);

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/********** Get from the database several test questions for listing *********/
/*****************************************************************************/

#define Qst_MAX_BYTES_QUERY_QUESTIONS (16 * 1024 - 1)

void Qst_GetQuestions (struct Qst_Questions *Questions,MYSQL_RES **mysql_res)
  {
   extern const char *Txt_No_questions_found_matching_your_search_criteria;
   char *Query = NULL;
   long LengthQuery;
   unsigned NumItemInList;
   const char *Ptr;
   char TagText[Tag_MAX_BYTES_TAG + 1];
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   char UnsignedStr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   Qst_AnswerType_t AnsType;
   char CrsCodStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Allocate space for query *****/
   if ((Query = malloc (Qst_MAX_BYTES_QUERY_QUESTIONS + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Select questions *****/
   /* Begin query */
   Str_Copy (Query,"SELECT tst_questions.QstCod"	// row[0]
		    " FROM tst_questions",Qst_MAX_BYTES_QUERY_QUESTIONS);
   if (!Questions->Tags.All)
      Str_Concat (Query,",tst_question_tags,tst_tags",Qst_MAX_BYTES_QUERY_QUESTIONS);

   Str_Concat (Query," WHERE tst_questions.CrsCod='",Qst_MAX_BYTES_QUERY_QUESTIONS);
   snprintf (CrsCodStr,sizeof (CrsCodStr),"%ld",Gbl.Hierarchy.Crs.CrsCod);
   Str_Concat (Query,CrsCodStr,Qst_MAX_BYTES_QUERY_QUESTIONS);
   Str_Concat (Query,"' AND tst_questions.EditTime>=FROM_UNIXTIME('",
               Qst_MAX_BYTES_QUERY_QUESTIONS);
   snprintf (LongStr,sizeof (LongStr),"%ld",
             (long) Gbl.DateRange.TimeUTC[Dat_STR_TIME]);
   Str_Concat (Query,LongStr,Qst_MAX_BYTES_QUERY_QUESTIONS);
   Str_Concat (Query,"') AND tst_questions.EditTime<=FROM_UNIXTIME('",
               Qst_MAX_BYTES_QUERY_QUESTIONS);
   snprintf (LongStr,sizeof (LongStr),"%ld",
	     (long) Gbl.DateRange.TimeUTC[Dat_END_TIME]);
   Str_Concat (Query,LongStr,Qst_MAX_BYTES_QUERY_QUESTIONS);
   Str_Concat (Query,"')",Qst_MAX_BYTES_QUERY_QUESTIONS);

   /* Add the tags selected */
   if (!Questions->Tags.All)
     {
      Str_Concat (Query," AND tst_questions.QstCod=tst_question_tags.QstCod"
	                " AND tst_question_tags.TagCod=tst_tags.TagCod"
                        " AND tst_tags.CrsCod='",
                  Qst_MAX_BYTES_QUERY_QUESTIONS);
      Str_Concat (Query,CrsCodStr,Qst_MAX_BYTES_QUERY_QUESTIONS);
      Str_Concat (Query,"'",Qst_MAX_BYTES_QUERY_QUESTIONS);
      LengthQuery = strlen (Query);
      NumItemInList = 0;
      Ptr = Questions->Tags.List;
      while (*Ptr)
        {
         Par_GetNextStrUntilSeparParamMult (&Ptr,TagText,Tag_MAX_BYTES_TAG);
         LengthQuery = LengthQuery + 35 + strlen (TagText) + 1;
         if (LengthQuery > Qst_MAX_BYTES_QUERY_QUESTIONS - 256)
            Err_ShowErrorAndExit ("Query size exceed.");
         Str_Concat (Query,
                     NumItemInList ? " OR tst_tags.TagTxt='" :
                                     " AND (tst_tags.TagTxt='",
                     Qst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,TagText,Qst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,"'",Qst_MAX_BYTES_QUERY_QUESTIONS);
         NumItemInList++;
        }
      Str_Concat (Query,")",Qst_MAX_BYTES_QUERY_QUESTIONS);
     }

   /* Add the types of answer selected */
   if (!Questions->AnswerTypes.All)
     {
      LengthQuery = strlen (Query);
      NumItemInList = 0;
      Ptr = Questions->AnswerTypes.List;
      while (*Ptr)
        {
         Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,Tag_MAX_BYTES_TAG);
	 AnsType = Qst_ConvertFromUnsignedStrToAnsTyp (UnsignedStr);
         LengthQuery = LengthQuery + 35 + strlen (Qst_DB_StrAnswerTypes[AnsType]) + 1;
         if (LengthQuery > Qst_MAX_BYTES_QUERY_QUESTIONS - 256)
            Err_ShowErrorAndExit ("Query size exceed.");
         Str_Concat (Query,
                     NumItemInList ? " OR tst_questions.AnsType='" :
                                     " AND (tst_questions.AnsType='",
                     Qst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,Qst_DB_StrAnswerTypes[AnsType],Qst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,"'",Qst_MAX_BYTES_QUERY_QUESTIONS);
         NumItemInList++;
        }
      Str_Concat (Query,")",Qst_MAX_BYTES_QUERY_QUESTIONS);
     }

   /* End the query */
   Str_Concat (Query," GROUP BY tst_questions.QstCod",Qst_MAX_BYTES_QUERY_QUESTIONS);

   switch (Questions->SelectedOrder)
     {
      case Qst_ORDER_STEM:
         Str_Concat (Query," ORDER BY tst_questions.Stem",
                     Qst_MAX_BYTES_QUERY_QUESTIONS);
         break;
      case Qst_ORDER_NUM_HITS:
         Str_Concat (Query," ORDER BY tst_questions.NumHits DESC,"
				     "tst_questions.Stem",
                     Qst_MAX_BYTES_QUERY_QUESTIONS);
         break;
      case Qst_ORDER_AVERAGE_SCORE:
         Str_Concat (Query," ORDER BY tst_questions.Score/tst_questions.NumHits DESC,"
				     "tst_questions.NumHits DESC,"
				     "tst_questions.Stem",
                     Qst_MAX_BYTES_QUERY_QUESTIONS);
         break;
      case Qst_ORDER_NUM_HITS_NOT_BLANK:
         Str_Concat (Query," ORDER BY tst_questions.NumHitsNotBlank DESC,"
				     "tst_questions.Stem",
                     Qst_MAX_BYTES_QUERY_QUESTIONS);
         break;
      case Qst_ORDER_AVERAGE_SCORE_NOT_BLANK:
         Str_Concat (Query," ORDER BY tst_questions.Score/tst_questions.NumHitsNotBlank DESC,"
				     "tst_questions.NumHitsNotBlank DESC,"
				     "tst_questions.Stem",
                     Qst_MAX_BYTES_QUERY_QUESTIONS);
         break;
     }

   /* Make the query */
   Questions->NumQsts = (unsigned) DB_QuerySELECT (mysql_res,"can not get questions",
					           "%s",
					           Query);
   if (Questions->NumQsts == 0)
      Ale_ShowAlert (Ale_INFO,Txt_No_questions_found_matching_your_search_criteria);
  }

/*****************************************************************************/
/***************** Change format of answers text / feedback ******************/
/*****************************************************************************/

void Qst_ChangeFormatAnswersText (struct Qst_Question *Question)
  {
   unsigned NumOpt;

   /***** Change format of answers text *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
      /* Convert answer text, that is in HTML, to rigorous HTML */
      if (Question->Answer.Options[NumOpt].Text[0])
	 Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			   Question->Answer.Options[NumOpt].Text,
			   Qst_MAX_BYTES_ANSWER_OR_FEEDBACK,false);
  }

void Qst_ChangeFormatAnswersFeedback (struct Qst_Question *Question)
  {
   unsigned NumOpt;

   /***** Change format of answers text and feedback *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
      /* Convert answer feedback, that is in HTML, to rigorous HTML */
      if (Question->Answer.Options[NumOpt].Feedback)
	 if (Question->Answer.Options[NumOpt].Feedback[0])
	    Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			      Question->Answer.Options[NumOpt].Feedback,
			      Qst_MAX_BYTES_ANSWER_OR_FEEDBACK,false);
  }

/*****************************************************************************/
/** Convert a string with the type of answer in database to type of answer ***/
/*****************************************************************************/

Qst_AnswerType_t Qst_ConvertFromStrAnsTypDBToAnsTyp (const char *StrAnsTypeDB)
  {
   Qst_AnswerType_t AnsType;

   if (StrAnsTypeDB != NULL)
      if (StrAnsTypeDB[0])
	 for (AnsType  = (Qst_AnswerType_t) 0;
	      AnsType <= (Qst_AnswerType_t) (Qst_NUM_ANS_TYPES - 1);
	      AnsType++)
	    if (!strcmp (StrAnsTypeDB,Qst_DB_StrAnswerTypes[AnsType]))
	       return AnsType;

   return Qst_ANS_UNKNOWN;
  }

/*****************************************************************************/
/************ Convert a string with an unsigned to answer type ***************/
/*****************************************************************************/

Qst_AnswerType_t Qst_ConvertFromUnsignedStrToAnsTyp (const char *UnsignedStr)
  {
   unsigned AnsType;

   if (sscanf (UnsignedStr,"%u",&AnsType) != 1)
      Err_WrongAnswerExit ();
   if (AnsType >= Qst_NUM_ANS_TYPES)
      Err_WrongAnswerExit ();
   return (Qst_AnswerType_t) AnsType;
  }
