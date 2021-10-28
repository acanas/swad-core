// swad_question_database.c: test/exam/game questions, operations with database

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
// #include <stdio.h>		// For asprintf
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_error.h"
// #include "swad_exam_set.h"
// #include "swad_figure.h"
// #include "swad_form.h"
#include "swad_global.h"
#include "swad_question.h"
// #include "swad_question_import.h"
// #include "swad_tag_database.h"
// #include "swad_test.h"
#include "swad_test_print.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

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

#define Qst_MAX_BYTES_QUERY_QUESTIONS (16 * 1024 - 1)

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
/*********************** Update the score of a question **********************/
/*****************************************************************************/

void Qst_DB_UpdateQstScore (long QstCod,bool AnswerIsNotBlank,double Score)
  {
   /***** Update number of clicks and score of the question *****/
   Str_SetDecimalPointToUS ();		// To print the floating point as a dot
   if (AnswerIsNotBlank)	// User's answer is not blank
      DB_QueryUPDATE ("can not update the score of a question",
		      "UPDATE tst_questions"
	                " SET NumHits=NumHits+1,"
	                     "NumHitsNotBlank=NumHitsNotBlank+1,"
	                     "Score=Score+(%.15lg)"
                      " WHERE QstCod=%ld",
		      Score,
		      QstCod);
   else					// User's answer is blank
      DB_QueryUPDATE ("can not update the score of a question",
		      "UPDATE tst_questions"
	                " SET NumHits=NumHits+1"
                      " WHERE QstCod=%ld",
		      QstCod);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/***************** Get several test questions from database ******************/
/*****************************************************************************/

unsigned Qst_DB_GetQsts (MYSQL_RES **mysql_res,
                         const struct Qst_Questions *Questions)
  {
   extern const char *Qst_DB_StrAnswerTypes[Qst_NUM_ANS_TYPES];
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
   unsigned NumQsts;

   /***** Allocate space for query *****/
   if ((Query = malloc (Qst_MAX_BYTES_QUERY_QUESTIONS + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Select questions *****/
   /* Begin query */
   Str_Copy (Query,"SELECT tst_questions.QstCod"	// row[0]
		    " FROM tst_questions",Qst_MAX_BYTES_QUERY_QUESTIONS);
   if (!Questions->Tags.All)
      Str_Concat (Query,","
	                "tst_question_tags,"
	                "tst_tags",
	          Qst_MAX_BYTES_QUERY_QUESTIONS);

   Str_Concat (Query," WHERE tst_questions.CrsCod='",
               Qst_MAX_BYTES_QUERY_QUESTIONS);
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
            Err_QuerySizeExceededExit ();
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
            Err_QuerySizeExceededExit ();
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
   if ((NumQsts = (unsigned)
        DB_QuerySELECT (mysql_res,"can not get questions",
		        "%s",
		        Query)) == 0)
      Ale_ShowAlert (Ale_INFO,Txt_No_questions_found_matching_your_search_criteria);

   return NumQsts;
  }

/*****************************************************************************/
/******************* Get questions for a new test print **********************/
/*****************************************************************************/

unsigned Qst_DB_GetQstsForNewTestPrint (MYSQL_RES **mysql_res,
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
   if ((Query = malloc (Qst_MAX_BYTES_QUERY_QUESTIONS + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Select questions without hidden tags *****/
   /* Begin query */
   // Reject questions with any tag hidden
   // Select only questions with tags
   // DISTINCTROW is necessary to not repeat questions
   snprintf (Query,Qst_MAX_BYTES_QUERY_QUESTIONS + 1,
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
         if (LengthQuery > Qst_MAX_BYTES_QUERY_QUESTIONS - 128)
            Err_QuerySizeExceededExit ();
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
         if (LengthQuery > Qst_MAX_BYTES_QUERY_QUESTIONS - 128)
            Err_QuerySizeExceededExit ();
         Str_Concat (Query,
                     NumItemInList ? " OR tst_questions.AnsType='" :
                                     " AND (tst_questions.AnsType='",
                     Qst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,Qst_DB_StrAnswerTypes[AnswerType],Qst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,"'",Qst_MAX_BYTES_QUERY_QUESTIONS);
         NumItemInList++;
        }
      Str_Concat (Query,")",Qst_MAX_BYTES_QUERY_QUESTIONS);
     }

   /* End query */
   Str_Concat (Query," ORDER BY RAND() LIMIT ",Qst_MAX_BYTES_QUERY_QUESTIONS);
   snprintf (StrNumQsts,sizeof (StrNumQsts),"%u",Questions->NumQsts);
   Str_Concat (Query,StrNumQsts,Qst_MAX_BYTES_QUERY_QUESTIONS);
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
/*********************** Get number of test questions ************************/
/*****************************************************************************/
// Returns the number of test questions
// in this location (all the platform, current degree or current course)

unsigned Qst_DB_GetNumQsts (MYSQL_RES **mysql_res,
                            HieLvl_Level_t Scope,Qst_AnswerType_t AnsType)
  {
   switch (Scope)
     {
      case HieLvl_SYS:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),"		// row[0]
        		           "SUM(NumHits),"	// row[1]
        		           "SUM(Score)"		// row[2]
        	             " FROM tst_questions");

         return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of test questions",
			 "SELECT COUNT(*),"		// row[0]
				"SUM(NumHits),"		// row[1]
				"SUM(Score)"		// row[2]
			  " FROM tst_questions"
			 " WHERE AnsType='%s'",
			 Qst_DB_StrAnswerTypes[AnsType]);
      case HieLvl_CTY:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),"		// row[0]
        		           "SUM(NumHits),"	// row[1]
        		           "SUM(Score)"		// row[2]
        	             " FROM ins_instits,"
        	                   "ctr_centers,"
        	                   "deg_degrees,"
        	                   "crs_courses,"
        	                   "tst_questions"
                            " WHERE ins_instits.CtyCod=%ld"
                              " AND ins_instits.InsCod=ctr_centers.InsCod"
                              " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
                              " AND deg_degrees.DegCod=crs_courses.DegCod"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Cty.CtyCod);

         return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of test questions",
			 "SELECT COUNT(*),"		// row[0]
				"SUM(NumHits),"		// row[1]
				"SUM(Score)"		// row[2]
			  " FROM ins_instits,"
				"ctr_centers,"
				"deg_degrees,"
				"crs_courses,"
				"tst_questions"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=tst_questions.CrsCod"
			   " AND tst_questions.AnsType='%s'",
			 Gbl.Hierarchy.Cty.CtyCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case HieLvl_INS:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),"		// row[0]
        		           "SUM(NumHits),"	// row[1]
        		           "SUM(Score)"		// row[2]
        	             " FROM ctr_centers,"
        	                   "deg_degrees,"
        	                   "crs_courses,"
        	                   "tst_questions"
                            " WHERE ctr_centers.InsCod=%ld"
                              " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
                              " AND deg_degrees.DegCod=crs_courses.DegCod"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Ins.InsCod);

         return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of test questions",
			 "SELECT COUNT(*),"		// row[0]
				"SUM(NumHits),"		// row[1]
				"SUM(Score)"		// row[2]
			  " FROM ctr_centers,"
				"deg_degrees,"
				"crs_courses,"
				"tst_questions"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=tst_questions.CrsCod"
			   " AND tst_questions.AnsType='%s'",
			 Gbl.Hierarchy.Ins.InsCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case HieLvl_CTR:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),"		// row[0]
        		           "SUM(NumHits),"	// row[1]
        		           "SUM(Score)"		// row[2]
        	             " FROM deg_degrees,"
        	                   "crs_courses,"
        	                   "tst_questions"
                            " WHERE deg_degrees.CtrCod=%ld"
                              " AND deg_degrees.DegCod=crs_courses.DegCod"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Ctr.CtrCod);

         return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of test questions",
			 "SELECT COUNT(*),"		// row[0]
				"SUM(NumHits),"		// row[1]
				"SUM(Score)"		// row[2]
			  " FROM deg_degrees,"
				"crs_courses,"
				"tst_questions"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=tst_questions.CrsCod"
			   " AND tst_questions.AnsType='%s'",
			 Gbl.Hierarchy.Ctr.CtrCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case HieLvl_DEG:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),"		// row[0]
        		           "SUM(NumHits),"	// row[1]
        		           "SUM(Score)"		// row[2]
        	             " FROM crs_courses,"
        	                   "tst_questions"
                            " WHERE crs_courses.DegCod=%ld"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Deg.DegCod);

         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of test questions",
			 "SELECT COUNT(*),"		// row[0]
				"SUM(NumHits),"		// row[1]
				"SUM(Score)"		// row[2]
			  " FROM crs_courses,"
				"tst_questions"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=tst_questions.CrsCod"
			   " AND tst_questions.AnsType='%s'",
			 Gbl.Hierarchy.Deg.DegCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case HieLvl_CRS:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),"		// row[0]
        		           "SUM(NumHits),"	// row[1]
        		           "SUM(Score)"		// row[2]
        	             " FROM tst_questions"
                            " WHERE CrsCod=%ld",
			    Gbl.Hierarchy.Crs.CrsCod);

         return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of test questions",
			 "SELECT COUNT(*),"		// row[0]
				"SUM(NumHits),"		// row[1]
				"SUM(Score)"		// row[2]
			  " FROM tst_questions"
			 " WHERE CrsCod=%ld"
			   " AND AnsType='%s'",
			 Gbl.Hierarchy.Crs.CrsCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      default:
	 Err_WrongScopeExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/**************** Get number of courses with test questions ******************/
/*****************************************************************************/
// Returns the number of courses with test questions
// in this location (all the platform, current degree or current course)

unsigned Qst_DB_GetNumCrssWithQsts (HieLvl_Level_t Scope,
                                    Qst_AnswerType_t AnsType)
  {
   extern const char *Qst_DB_StrAnswerTypes[Qst_NUM_ANS_TYPES];

   /***** Get number of courses with test questions from database *****/
   switch (Scope)
     {
      case HieLvl_SYS:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
            DB_QueryCOUNT ("can not get number of courses with test questions",
        		    "SELECT COUNT(DISTINCT CrsCod)"
        	             " FROM tst_questions");

         return (unsigned)
	 DB_QueryCOUNT ("can not get number of courses with test questions",
			 "SELECT COUNT(DISTINCT CrsCod)"
			  " FROM tst_questions"
			 " WHERE AnsType='%s'",
			 Qst_DB_StrAnswerTypes[AnsType]);
      case HieLvl_CTY:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
            DB_QueryCOUNT ("can not get number of courses with test questions",
        		    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	             " FROM ins_instits,"
        	                   "ctr_centers,"
        	                   "deg_degrees,"
        	                   "crs_courses,"
        	                   "tst_questions"
                            " WHERE ins_instits.CtyCod=%ld"
                              " AND ins_instits.InsCod=ctr_centers.InsCod"
                              " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
                              " AND deg_degrees.DegCod=crs_courses.DegCod"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Cty.CtyCod);

	 return (unsigned)
	 DB_QueryCOUNT ("can not get number of courses with test questions",
			 "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
			  " FROM ins_instits,"
			         "ctr_centers,"
			         "deg_degrees,"
			         "crs_courses,"
			         "tst_questions"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=tst_questions.CrsCod"
			   " AND tst_questions.AnsType='%s'",
			 Gbl.Hierarchy.Cty.CtyCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case HieLvl_INS:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
            DB_QueryCOUNT ("can not get number of courses with test questions",
        		    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	             " FROM ctr_centers,"
        	                   "deg_degrees,"
        	                   "crs_courses,"
        	                   "tst_questions"
                            " WHERE ctr_centers.InsCod=%ld"
                              " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
                              " AND deg_degrees.DegCod=crs_courses.DegCod"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Ins.InsCod);

         return (unsigned)
	 DB_QueryCOUNT ("can not get number of courses with test questions",
			 "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "tst_questions"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=tst_questions.CrsCod"
			   " AND tst_questions.AnsType='%s'",
			 Gbl.Hierarchy.Ins.InsCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case HieLvl_CTR:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
            DB_QueryCOUNT ("can not get number of courses with test questions",
        		    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	             " FROM deg_degrees,"
        	                   "crs_courses,"
        	                   "tst_questions"
                            " WHERE deg_degrees.CtrCod=%ld"
                              " AND deg_degrees.DegCod=crs_courses.DegCod"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Ctr.CtrCod);

         return (unsigned)
	 DB_QueryCOUNT ("can not get number of courses with test questions",
			 "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "tst_questions"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=tst_questions.CrsCod"
			   " AND tst_questions.AnsType='%s'",
			 Gbl.Hierarchy.Ctr.CtrCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case HieLvl_DEG:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
            DB_QueryCOUNT ("can not get number of courses with test questions",
        		    "SELECT COUNTDISTINCT (tst_questions.CrsCod)"
        	             " FROM crs_courses,"
        	                   "tst_questions"
                            " WHERE crs_courses.DegCod=%ld"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Deg.DegCod);

         return (unsigned)
	 DB_QueryCOUNT ("can not get number of courses with test questions",
			 "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
			  " FROM crs_courses,"
			        "tst_questions"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=tst_questions.CrsCod"
			   " AND tst_questions.AnsType='%s'",
			 Gbl.Hierarchy.Deg.DegCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case HieLvl_CRS:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
            DB_QueryCOUNT ("can not get number of courses with test questions",
        		    "SELECT COUNT(DISTINCT CrsCod)"
        	             " FROM tst_questions"
                            " WHERE CrsCod=%ld",
			    Gbl.Hierarchy.Crs.CrsCod);

         return (unsigned)
	 DB_QueryCOUNT ("can not get number of courses with test questions",
			 "SELECT COUNT(DISTINCT CrsCod)"
			  " FROM tst_questions"
			 " WHERE CrsCod=%ld"
			   " AND AnsType='%s'",
			 Gbl.Hierarchy.Crs.CrsCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      default:
	 Err_WrongScopeExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/*********** Get number of courses with pluggable test questions *************/
/*****************************************************************************/
// Returns the number of courses with pluggable test questions
// in this location (all the platform, current degree or current course)

unsigned Qst_DB_GetNumCrssWithPluggableQsts (HieLvl_Level_t Scope,
                                             Qst_AnswerType_t AnsType)
  {
   extern const char *Qst_DB_StrAnswerTypes[Qst_NUM_ANS_TYPES];
   extern const char *Tst_DB_Pluggable[TstCfg_NUM_OPTIONS_PLUGGABLE];

   /***** Get number of courses with test questions from database *****/
   switch (Scope)
     {
      case HieLvl_SYS:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
	    DB_QueryCOUNT ("can not get number of courses with pluggable test questions",
			   "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM tst_questions,"
        	                  "tst_config"
                           " WHERE tst_questions.CrsCod=tst_config.CrsCod"
                             " AND tst_config.pluggable='%s'",
			   Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);

	 return (unsigned)
	 DB_QueryCOUNT ("can not get number of courses with pluggable test questions",
			"SELECT COUNT(DISTINCT tst_questions.CrsCod)"
			 " FROM tst_questions,"
			       "tst_config"
			" WHERE tst_questions.AnsType='%s'"
			  " AND tst_questions.CrsCod=tst_config.CrsCod"
			  " AND tst_config.pluggable='%s'",
			Qst_DB_StrAnswerTypes[AnsType],
			Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);
      case HieLvl_CTY:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
	    DB_QueryCOUNT ("can not get number of courses with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	             " FROM ins_instits,"
        	                   "ctr_centers,"
        	                   "deg_degrees,"
        	                   "crs_courses,"
        	                   "tst_questions,"
        	                   "tst_config"
                            " WHERE ins_instits.CtyCod=%ld"
                              " AND ins_instits.InsCod=ctr_centers.InsCod"
                              " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
                              " AND deg_degrees.DegCod=crs_courses.DegCod"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod"
                              " AND tst_questions.CrsCod=tst_config.CrsCod"
                              " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Cty.CtyCod,
			    Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);

	 return (unsigned)
	 DB_QueryCOUNT ("can not get number of courses with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	             " FROM ins_instits,"
        	                   "ctr_centers,"
        	                   "deg_degrees,"
        	                   "crs_courses,"
        	                   "tst_questions,"
        	                   "tst_config"
                            " WHERE ins_instits.CtyCod=%ld"
                              " AND ins_instits.InsCod=ctr_centers.InsCod"
                              " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
                              " AND deg_degrees.DegCod=crs_courses.DegCod"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod"
                              " AND tst_questions.AnsType='%s'"
                              " AND tst_questions.CrsCod=tst_config.CrsCod"
                              " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Cty.CtyCod,
			    Qst_DB_StrAnswerTypes[AnsType],
			    Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);
      case HieLvl_INS:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
	    DB_QueryCOUNT ("can not get number of courses with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	             " FROM ctr_centers,"
        	                   "deg_degrees,"
        	                   "crs_courses,"
        	                   "tst_questions,"
        	                   "tst_config"
                            " WHERE ctr_centers.InsCod=%ld"
                              " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
                              " AND deg_degrees.DegCod=crs_courses.DegCod"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod"
                              " AND tst_questions.CrsCod=tst_config.CrsCod"
                              " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Ins.InsCod,
			    Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);

	 return (unsigned)
	 DB_QueryCOUNT ("can not get number of courses with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	             " FROM ctr_centers,"
        	                   "deg_degrees,"
        	                   "crs_courses,"
        	                   "tst_questions,"
        	                   "tst_config"
                            " WHERE ctr_centers.InsCod=%ld"
                              " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
                              " AND deg_degrees.DegCod=crs_courses.DegCod"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod"
                              " AND tst_questions.AnsType='%s'"
                              " AND tst_questions.CrsCod=tst_config.CrsCod"
                              " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Ins.InsCod,
			    Qst_DB_StrAnswerTypes[AnsType],
			    Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);
      case HieLvl_CTR:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
	    DB_QueryCOUNT ("can not get number of courses with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	             " FROM deg_degrees,"
        	                   "crs_courses,"
        	                   "tst_questions,"
        	                   "tst_config"
                            " WHERE deg_degrees.CtrCod=%ld"
                              " AND deg_degrees.DegCod=crs_courses.DegCod"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod"
                              " AND tst_questions.CrsCod=tst_config.CrsCod"
                              " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Ctr.CtrCod,
			    Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);

	 return (unsigned)
	 DB_QueryCOUNT ("can not get number of courses with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	             " FROM deg_degrees,"
        	                   "crs_courses,"
        	                   "tst_questions,"
        	                   "tst_config"
                            " WHERE deg_degrees.CtrCod=%ld"
                              " AND deg_degrees.DegCod=crs_courses.DegCod"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod"
                              " AND tst_questions.AnsType='%s'"
                              " AND tst_questions.CrsCod=tst_config.CrsCod"
                              " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Ctr.CtrCod,
			    Qst_DB_StrAnswerTypes[AnsType],
			    Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);
      case HieLvl_DEG:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
	    DB_QueryCOUNT ("can not get number of courses with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	             " FROM crs_courses,"
        	                   "tst_questions,"
        	                   "tst_config"
                            " WHERE crs_courses.DegCod=%ld"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod"
                              " AND tst_questions.CrsCod=tst_config.CrsCod"
                              " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Deg.DegCod,
			    Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);

	 return (unsigned)
	 DB_QueryCOUNT ("can not get number of courses with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	             " FROM crs_courses,"
        	                   "tst_questions,"
        	                   "tst_config"
                            " WHERE crs_courses.DegCod=%ld"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod"
                              " AND tst_questions.AnsType='%s'"
                              " AND tst_questions.CrsCod=tst_config.CrsCod"
                              " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Deg.DegCod,
			    Qst_DB_StrAnswerTypes[AnsType],
			    Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);
      case HieLvl_CRS:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
	    DB_QueryCOUNT ("can not get number of courses with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	             " FROM tst_questions,"
        	                   "tst_config"
                            " WHERE tst_questions.CrsCod=%ld"
                              " AND tst_questions.CrsCod=tst_config.CrsCod"
                              " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Crs.CrsCod,
			    Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);

	 return (unsigned)
	 DB_QueryCOUNT ("can not get number of courses with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	             " FROM tst_questions,"
        	                   "tst_config"
                            " WHERE tst_questions.CrsCod=%ld"
                              " AND tst_questions.AnsType='%s'"
                              " AND tst_questions.CrsCod=tst_config.CrsCod"
                              " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Crs.CrsCod,
			    Qst_DB_StrAnswerTypes[AnsType],
			    Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);
      default:
	 Err_WrongScopeExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/****************** Get data of a question from database *********************/
/*****************************************************************************/

unsigned Qst_DB_GetQstData (MYSQL_RES **mysql_res,long QstCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get a question",
		   "SELECT UNIX_TIMESTAMP(EditTime),"	// row[0]
			  "AnsType,"			// row[1]
			  "Shuffle,"			// row[2]
			  "Stem,"			// row[3]
			  "Feedback,"			// row[4]
			  "MedCod,"			// row[5]
			  "NumHits,"			// row[6]
			  "NumHitsNotBlank,"		// row[7]
			  "Score"			// row[8]
		    " FROM tst_questions"
		   " WHERE QstCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
		   QstCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/*************** Get answer type of a question from database *****************/
/*****************************************************************************/

Qst_AnswerType_t Qst_DB_GetQstAnswerType (long QstCod)
  {
   char StrAnsTypeDB[256];

   /***** Get type of answer from database *****/
   DB_QuerySELECTString (StrAnsTypeDB,sizeof (StrAnsTypeDB) - 1,
                         "can not get the type of a question",
		         "SELECT AnsType"
		          " FROM tst_questions"
		         " WHERE QstCod=%ld",
		         QstCod);
   return Qst_ConvertFromStrAnsTypDBToAnsTyp (StrAnsTypeDB);
  }

/*****************************************************************************/
/******** Get media code associated with the stem of a test question *********/
/*****************************************************************************/

long Qst_DB_GetQstMedCod (long CrsCod,long QstCod)
  {
   return DB_QuerySELECTCode ("can not get media",
			      "SELECT MedCod"
			       " FROM tst_questions"
			      " WHERE QstCod=%ld"
				" AND CrsCod=%ld",	// Extra check
			      QstCod,
			      CrsCod);
  }

/*****************************************************************************/
/************ Get number of answers of a question from database **************/
/*****************************************************************************/

unsigned Qst_DB_GetNumAnswersQst (long QstCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of answers of a question",
		  "SELECT COUNT(*)"
		   " FROM tst_answers"
		  " WHERE QstCod=%ld",
		  QstCod);
  }

/*****************************************************************************/
/***************** Get answers of a question from database *******************/
/*****************************************************************************/

unsigned Qst_DB_GetDataOfAnswers (MYSQL_RES **mysql_res,long QstCod,bool Shuffle)
  {
   unsigned NumOptions;

   if (!(NumOptions = (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get answers of a question",
			 "SELECT AnsInd,"	// row[0]
				"Answer,"	// row[1]
				"Feedback,"	// row[2]
				"MedCod,"	// row[3]
				"Correct"	// row[4]
			  " FROM tst_answers"
			 " WHERE QstCod=%ld"
			 " ORDER BY %s",
			 QstCod,
			 Shuffle ? "RAND()" :
				   "AnsInd")))
      Err_WrongAnswerExit ();

   return NumOptions;
  }

/*****************************************************************************/
/***************** Get answers of a question from database *******************/
/*****************************************************************************/

unsigned Qst_DB_GetTextOfAnswers (MYSQL_RES **mysql_res,long QstCod)
  {
   unsigned NumOptions;

   if (!(NumOptions = (unsigned)
         DB_QuerySELECT (mysql_res,"can not get answers of a question",
			 "SELECT Answer"		// row[0]
			  " FROM tst_answers"
			 " WHERE QstCod=%ld"
		         " ORDER BY AnsInd",
			 QstCod)))
      Err_WrongAnswerExit ();

   return NumOptions;
  }

/*****************************************************************************/
/*********** Get suffled/not-shuffled answers indexes of question ************/
/*****************************************************************************/

unsigned Qst_DB_GetShuffledAnswersIndexes (MYSQL_RES **mysql_res,
                                           const struct Qst_Question *Question)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get questions of a game",
		   "SELECT AnsInd"	// row[0]
		    " FROM tst_answers"
		   " WHERE QstCod=%ld"
		   " ORDER BY %s",
		   Question->QstCod,
		   Question->Answer.Shuffle ? "RAND()" :	// Use RAND() because is really random; RAND(NOW()) repeats order
					      "AnsInd");
  }

/*****************************************************************************/
/********* Get media code associated to an answer of a test question *********/
/*****************************************************************************/

long Qst_DB_GetAnswerMedCod (long QstCod,unsigned AnsInd)
  {
   return DB_QuerySELECTCode ("can not get media",
			      "SELECT MedCod"
			       " FROM tst_answers"
			      " WHERE QstCod=%ld"
				" AND AnsInd=%u",
			      QstCod,
			      AnsInd);
  }

/*****************************************************************************/
/********************** Remove a question from database **********************/
/*****************************************************************************/

void Qst_DB_RemoveQst (long CrsCod,long QstCod)
  {
   DB_QueryDELETE ("can not remove a question",
		   "DELETE FROM tst_questions"
		   " WHERE QstCod=%ld"
		     " AND CrsCod=%ld",
		   QstCod,
		   CrsCod);
  }

/*****************************************************************************/
/******************** Remove answers from a test question ********************/
/*****************************************************************************/

void Qst_DB_RemAnsFromQst (long QstCod)
  {
   /***** Remove answers *****/
   DB_QueryDELETE ("can not remove the answers of a question",
		   "DELETE FROM tst_answers"
		   " WHERE QstCod=%ld",
		   QstCod);
  }
