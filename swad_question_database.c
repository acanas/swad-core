// swad_question_database.c: test/exam/game questions, operations with database

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <string.h>		// For string functions

#include "swad_alert.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_question.h"
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
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*********** Insert or update question in the table of questions *************/
/*****************************************************************************/

long Qst_DB_CreateQst (const struct Qst_Question *Question)
  {
   extern const char Qst_Shuffle_YN[Qst_NUM_SHUFFLE];

   return
   DB_QueryINSERTandReturnCode ("can not create question",
				"INSERT INTO tst_questions"
					   " (CrsCod,"
					     "EditTime,"
					     "AnsType,"
					     "Shuffle,"
					     "Stem,"
					     "Feedback,"
					     "MedCod,"
					     "NumHits,"
					     "Score)"
			            " VALUES"
					  " (%ld,"	// CrsCod
					     "NOW(),"	// EditTime
					     "'%s',"	// AnsType
					     "'%c',"	// Shuffle
					     "'%s',"	// Stem
					     "'%s',"	// Feedback
					     "%ld,"	// MedCod
					     "0,"	// NumHits
					     "0)",	// Score
				Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				Qst_DB_StrAnswerTypes[Question->Answer.Type],
				Qst_Shuffle_YN[Question->Answer.Shuffle],
				Question->Stem,
				Question->Feedback ? Question->Feedback :
						     "",
				Question->Media.MedCod);
  }

/*****************************************************************************/
/************ Update existing question in the table of questions *************/
/*****************************************************************************/

void Qst_DB_UpdateQst (const struct Qst_Question *Question)
  {
   extern const char Qst_Shuffle_YN[Qst_NUM_SHUFFLE];

   DB_QueryUPDATE ("can not update question",
		   "UPDATE tst_questions"
		     " SET EditTime=NOW(),"
			  "AnsType='%s',"
			  "Shuffle='%c',"
			  "Stem='%s',"
			  "Feedback='%s',"
			  "MedCod=%ld"
		   " WHERE QstCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
		   Qst_DB_StrAnswerTypes[Question->Answer.Type],
		   Qst_Shuffle_YN[Question->Answer.Shuffle],
		   Question->Stem,
		   Question->Feedback ? Question->Feedback :
					"",
		   Question->Media.MedCod,
		   Question->QstCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/*********************** Update the score of a question **********************/
/*****************************************************************************/

void Qst_DB_UpdateQstScore (const struct TstPrn_Print *Print,unsigned QstInd)
  {
   Str_SetDecimalPointToUS ();		// To print the floating point as a dot
      if (Print->PrintedQuestions[QstInd].Answer.Str[0] != '\0')		// User's answer is not blank
	 DB_QueryUPDATE ("can not update the score of a question",
			 "UPDATE tst_questions"
			   " SET NumHits=NumHits+1,"
				"NumHitsNotBlank=NumHitsNotBlank+1,"
				"Score=Score+(%.15lg)"
			 " WHERE QstCod=%ld",
			 Print->PrintedQuestions[QstInd].Answer.Score,
			 Print->PrintedQuestions[QstInd].QstCod);
      else					// User's answer is blank
	 DB_QueryUPDATE ("can not update the score of a question",
			 "UPDATE tst_questions"
			   " SET NumHits=NumHits+1"
			 " WHERE QstCod=%ld",
			 Print->PrintedQuestions[QstInd].QstCod);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/*********************** Change the shuffle of a question ********************/
/*****************************************************************************/

void Qst_DB_UpdateQstShuffle (long QstCod,Qst_Shuffle_t Shuffle)
  {
   extern const char Qst_Shuffle_YN[Qst_NUM_SHUFFLE];

   DB_QueryUPDATE ("can not update the shuffle type of a question",
		   "UPDATE tst_questions"
		     " SET Shuffle='%c'"
                   " WHERE QstCod=%ld"
                     " AND CrsCod=%ld",	// Extra check
		   Qst_Shuffle_YN[Shuffle],
		   QstCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/*************************** Create integer answer ***************************/
/*****************************************************************************/

void Qst_DB_CreateIntAnswer (const struct Qst_Question *Question)
  {
   DB_QueryINSERT ("can not create answer",
		   "INSERT INTO tst_answers"
		   " (QstCod,AnsInd,Answer,Feedback,MedCod,Correct)"
		   " VALUES"
		   " (%ld,0,%ld,'',-1,'Y')",
		   Question->QstCod,
		   Question->Answer.Integer);
  }

/*****************************************************************************/
/**************************** Create float answer ****************************/
/*****************************************************************************/

void Qst_DB_CreateFltAnswer (const struct Qst_Question *Question)
  {
   unsigned i;

   Str_SetDecimalPointToUS ();		// To print the floating point as a dot
   for (i = 0;
	i < 2;
	i++)
      DB_QueryINSERT ("can not create answer",
		      "INSERT INTO tst_answers"
		      " (QstCod,AnsInd,Answer,Feedback,MedCod,Correct)"
		      " VALUES"
		      " (%ld,%u,'%.15lg','',-1,'Y')",
		      Question->QstCod,
		      i,
		      Question->Answer.FloatingPoint[i]);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/***************************** Create T/F answer *****************************/
/*****************************************************************************/

void Qst_DB_CreateTF_Answer (const struct Qst_Question *Question)
  {
   DB_QueryINSERT ("can not create answer",
		   "INSERT INTO tst_answers"
		   " (QstCod,AnsInd,Answer,Feedback,MedCod,Correct)"
		   " VALUES"
		   " (%ld,0,'%c','',-1,'Y')",
		   Question->QstCod,
		   Question->Answer.TF);
  }

/*****************************************************************************/
/***************************** Create T/F answer *****************************/
/*****************************************************************************/

void Qst_DB_CreateChoAnswer (struct Qst_Question *Question)
  {
   extern const char Qst_Correct_YN[Qst_NUM_WRONG_CORRECT];
   unsigned NumOpt;

   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
      if (Question->Answer.Options[NumOpt].Text[0] ||			// Text
	  Question->Answer.Options[NumOpt].Media.Type != Med_TYPE_NONE)	// or media
	{
	 DB_QueryINSERT ("can not create answer",
			 "INSERT INTO tst_answers"
			 " (QstCod,AnsInd,Answer,Feedback,MedCod,Correct)"
			 " VALUES"
			 " (%ld,%u,'%s','%s',%ld,'%c')",
			 Question->QstCod,NumOpt,
			 Question->Answer.Options[NumOpt].Text,
			 Question->Answer.Options[NumOpt].Feedback ? Question->Answer.Options[NumOpt].Feedback :
								     "",
			 Question->Answer.Options[NumOpt].Media.MedCod,
			 Qst_Correct_YN[Question->Answer.Options[NumOpt].Correct]);

	 /* Update image status */
	 if (Question->Answer.Options[NumOpt].Media.Type != Med_TYPE_NONE)
	    Question->Answer.Options[NumOpt].Media.Status = Med_STORED_IN_DB;
	}
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
   unsigned NumSelTag;
   const char *Ptr;
   char LongStr[Cns_MAX_DIGITS_LONG + 1];
   char UnsignedStr[Cns_MAX_DIGITS_UINT + 1];
   Qst_AnswerType_t AnsType;
   char CrsCodStr[Cns_MAX_DIGITS_LONG + 1];
   unsigned NumQsts;

   /***** Allocate space for query *****/
   if ((Query = malloc (Qst_MAX_BYTES_QUERY_QUESTIONS + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Select questions *****/
   /* Begin query */
   Str_Copy (Query,"SELECT tst_questions.QstCod"	// row[0]
		    " FROM tst_questions",Qst_MAX_BYTES_QUERY_QUESTIONS);
   if (!Questions->Tags.All)
      Str_Concat (Query,",tst_question_tags",Qst_MAX_BYTES_QUERY_QUESTIONS);

   Str_Concat (Query," WHERE tst_questions.CrsCod=",
               Qst_MAX_BYTES_QUERY_QUESTIONS);
   snprintf (CrsCodStr,sizeof (CrsCodStr),"%ld",
	     Gbl.Hierarchy.Node[Hie_CRS].HieCod);
   Str_Concat (Query,CrsCodStr,Qst_MAX_BYTES_QUERY_QUESTIONS);
   Str_Concat (Query," AND tst_questions.EditTime>=FROM_UNIXTIME('",
               Qst_MAX_BYTES_QUERY_QUESTIONS);
   snprintf (LongStr,sizeof (LongStr),"%ld",
             (long) Dat_GetRangeTimeUTC (Dat_STR_TIME));
   Str_Concat (Query,LongStr,Qst_MAX_BYTES_QUERY_QUESTIONS);
   Str_Concat (Query,"') AND tst_questions.EditTime<=FROM_UNIXTIME('",
               Qst_MAX_BYTES_QUERY_QUESTIONS);
   snprintf (LongStr,sizeof (LongStr),"%ld",
	     (long) Dat_GetRangeTimeUTC (Dat_END_TIME));
   Str_Concat (Query,LongStr,Qst_MAX_BYTES_QUERY_QUESTIONS);
   Str_Concat (Query,"')",Qst_MAX_BYTES_QUERY_QUESTIONS);

   /* Add the tags selected */
   if (!Questions->Tags.All)
     {
      Str_Concat (Query," AND tst_questions.QstCod=tst_question_tags.QstCod",
                  Qst_MAX_BYTES_QUERY_QUESTIONS);
      for (NumSelTag = 0, LengthQuery = strlen (Query);
	   NumSelTag < Questions->Tags.NumSelected;
	   NumSelTag++)
        {
	 snprintf (LongStr,sizeof (LongStr),"%ld",
	           Questions->Tags.ListSelectedTagCods[NumSelTag]);
         LengthQuery += 35 + strlen (LongStr) + 1;
         if (LengthQuery > Qst_MAX_BYTES_QUERY_QUESTIONS - 256)
            Err_QuerySizeExceededExit ();
         Str_Concat (Query,NumSelTag ? " OR tst_question_tags.TagCod=" :
				       " AND (tst_question_tags.TagCod=",
                     Qst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,LongStr,Qst_MAX_BYTES_QUERY_QUESTIONS);
        }
      Str_Concat (Query,")",Qst_MAX_BYTES_QUERY_QUESTIONS);
     }

   /* Add the types of answer selected */
   if (!Questions->AnswerTypes.All)
     {
      for (Ptr = Questions->AnswerTypes.List, NumItemInList = 0, LengthQuery = strlen (Query);
           *Ptr;
           NumItemInList++)
        {
         Par_GetNextStrUntilSeparParMult (&Ptr,UnsignedStr,Tag_MAX_BYTES_TAG);
	 AnsType = Qst_ConvertFromUnsignedStrToAnsTyp (UnsignedStr);
         LengthQuery += 35 + strlen (Qst_DB_StrAnswerTypes[AnsType]) + 1;
         if (LengthQuery > Qst_MAX_BYTES_QUERY_QUESTIONS - 256)
            Err_QuerySizeExceededExit ();
         Str_Concat (Query,NumItemInList ? " OR tst_questions.AnsType='" :
					   " AND (tst_questions.AnsType='",
                     Qst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,Qst_DB_StrAnswerTypes[AnsType],
                     Qst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,"'",Qst_MAX_BYTES_QUERY_QUESTIONS);
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
   unsigned NumSelTag;
   unsigned NumItemInList;
   const char *Ptr;
   char LongStr[Cns_MAX_DIGITS_LONG + 1];
   char UnsignedStr[Cns_MAX_DIGITS_UINT + 1];
   Qst_AnswerType_t AnswerType;
   char StrNumQsts[Cns_MAX_DIGITS_UINT + 1];

   /***** Allocate space for query *****/
   if ((Query = malloc (Qst_MAX_BYTES_QUERY_QUESTIONS + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Select questions without hidden tags *****/
   /* Begin query */
   // Reject questions with any tag hidden
   // Select only questions with tags
   // DISTINCT is necessary to not repeat questions
   snprintf (Query,Qst_MAX_BYTES_QUERY_QUESTIONS + 1,
	     "SELECT DISTINCT "
	            "tst_questions.QstCod,"	// row[0]
                    "tst_questions.AnsType,"	// row[1]
                    "tst_questions.Shuffle"	// row[2]
	      " FROM tst_questions,tst_question_tags"
	     " WHERE tst_questions.CrsCod=%ld"
	       " AND tst_questions.QstCod NOT IN"
		   " (SELECT tst_question_tags.QstCod"
		      " FROM tst_tags,tst_question_tags"
		     " WHERE tst_tags.CrsCod=%ld"
		       " AND tst_tags.TagHidden='Y'"
		       " AND tst_tags.TagCod=tst_question_tags.TagCod)"
	       " AND tst_questions.QstCod=tst_question_tags.QstCod",
	     Gbl.Hierarchy.Node[Hie_CRS].HieCod,
	     Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   /* Add selected tags */
   if (Questions->Tags.PreselectedTagCod > 0)	// Only one preselected tag
     {
      Str_Concat (Query," AND tst_question_tags.TagCod=",
	          Qst_MAX_BYTES_QUERY_QUESTIONS);
      snprintf (LongStr,sizeof (LongStr),"%ld",
	        Questions->Tags.PreselectedTagCod);
      Str_Concat (Query,LongStr,Qst_MAX_BYTES_QUERY_QUESTIONS);
     }
   else if (!Questions->Tags.All)	// User has selected some tags, but not all
     {
      LengthQuery = strlen (Query);
      for (NumSelTag = 0;
	   NumSelTag < Questions->Tags.NumSelected;
	   NumSelTag++)
        {
	 snprintf (LongStr,sizeof (LongStr),"%ld",
	           Questions->Tags.ListSelectedTagCods[NumSelTag]);
         LengthQuery += 35 + strlen (LongStr) + 1;
         if (LengthQuery > Qst_MAX_BYTES_QUERY_QUESTIONS - 128)
            Err_QuerySizeExceededExit ();
         Str_Concat (Query,NumSelTag ? " OR tst_question_tags.TagCod=" :
				       " AND (tst_question_tags.TagCod=",
                     Qst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,LongStr,Qst_MAX_BYTES_QUERY_QUESTIONS);
        }
      Str_Concat (Query,")",Qst_MAX_BYTES_QUERY_QUESTIONS);
     }

   /* Add answer types selected */
   if (!Questions->AnswerTypes.All)
     {
      for (Ptr = Questions->AnswerTypes.List, NumItemInList = 0, LengthQuery = strlen (Query);
           *Ptr;
           NumItemInList++)
        {
         Par_GetNextStrUntilSeparParMult (&Ptr,UnsignedStr,Tag_MAX_BYTES_TAG);
	 AnswerType = Qst_ConvertFromUnsignedStrToAnsTyp (UnsignedStr);
         LengthQuery += 35 + strlen (Qst_DB_StrAnswerTypes[AnswerType]) + 1;
         if (LengthQuery > Qst_MAX_BYTES_QUERY_QUESTIONS - 128)
            Err_QuerySizeExceededExit ();
         Str_Concat (Query,NumItemInList ? " OR tst_questions.AnsType='" :
					   " AND (tst_questions.AnsType='",
                     Qst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,Qst_DB_StrAnswerTypes[AnswerType],Qst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,"'",Qst_MAX_BYTES_QUERY_QUESTIONS);
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
/******************* Get one question for trivial game **********************/
/*****************************************************************************/

unsigned Qst_DB_GetTrivialQst (MYSQL_RES **mysql_res,
                               char DegreesStr[API_MAX_BYTES_DEGREES_STR + 1],
                               float lowerScore,float upperScore)
  {
   unsigned NumQsts;

   Str_SetDecimalPointToUS ();	// To print the floating point as a dot

   NumQsts = (unsigned)
   DB_QuerySELECT (mysql_res,"can not get test questions",
		   "SELECT DISTINCT "
			  "tst_questions.QstCod,"				// row[0]
			  "tst_questions.AnsType,"				// row[1]
			  "tst_questions.Shuffle,"				// row[2]
			  "tst_questions.Stem,"					// row[3]
			  "tst_questions.Feedback,"				// row[4]
			  "tst_questions.Score/tst_questions.NumHits AS S"	// row[5]
		    " FROM crs_courses,"
			  "tst_questions"
		   " WHERE crs_courses.DegCod IN (%s)"
		     " AND crs_courses.CrsCod=tst_questions.CrsCod"
		     " AND tst_questions.AnsType='unique_choice'"
		     " AND tst_questions.NumHits>0"
		     " AND tst_questions.QstCod NOT IN"
			 " (SELECT tst_question_tags.QstCod"
			    " FROM crs_courses,"
				  "tst_tags,"
				  "tst_question_tags"
			   " WHERE crs_courses.DegCod IN (%s)"
			     " AND crs_courses.CrsCod=tst_tags.CrsCod"
			     " AND tst_tags.TagHidden='Y'"
			     " AND tst_tags.TagCod=tst_question_tags.TagCod)"
		   " HAVING S>='%f'"
		      " AND S<='%f'"
		 " ORDER BY RAND()"
		   " LIMIT 1",
		   DegreesStr,
		   DegreesStr,
		   lowerScore,
		   upperScore);

   Str_SetDecimalPointToLocal ();	// Return to local system

   return NumQsts;
  }

/*****************************************************************************/
/** Get number of visible test questions from database giving a course code **/
/*****************************************************************************/

unsigned Qst_DB_GetNumQstsInCrs (long CrsCod)
  {
   /***** Get number of questions *****/
   // Reject questions with any tag hidden
   // Select only questions with tags
   return (unsigned)
   DB_QueryCOUNT ("can not get number of test questions",
		  "SELECT COUNT(*)"
		   " FROM tst_questions,"
			 "tst_question_tags,"
			 "tst_tags"
		  " WHERE tst_questions.CrsCod=%ld"
		    " AND tst_questions.QstCod NOT IN"
			" (SELECT tst_question_tags.QstCod"
			   " FROM tst_tags,"
				 "tst_question_tags"
			  " WHERE tst_tags.CrsCod=%ld"
			    " AND tst_tags.TagHidden='Y'"
			    " AND tst_tags.TagCod=tst_question_tags.TagCod)"
		    " AND tst_questions.QstCod=tst_question_tags.QstCod"
		    " AND tst_question_tags.TagCod=tst_tags.TagCod"
		    " AND tst_tags.CrsCod=%ld",
		  CrsCod,
		  CrsCod,
		  CrsCod);
  }

/*****************************************************************************/
/*********************** Get number of test questions ************************/
/*****************************************************************************/
// Returns the number of test questions
// in this location (all the platform, current degree or current course)

unsigned Qst_DB_GetNumQsts (MYSQL_RES **mysql_res,
                            Hie_Level_t HieLvl,Qst_AnswerType_t AnsType)
  {
   switch (HieLvl)
     {
      case Hie_SYS:
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
      case Hie_CTY:
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
			    Gbl.Hierarchy.Node[Hie_CTY].HieCod);

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
			 Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case Hie_INS:
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
			    Gbl.Hierarchy.Node[Hie_INS].HieCod);

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
			 Gbl.Hierarchy.Node[Hie_INS].HieCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case Hie_CTR:
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
			    Gbl.Hierarchy.Node[Hie_CTR].HieCod);

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
			 Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case Hie_DEG:
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
			    Gbl.Hierarchy.Node[Hie_DEG].HieCod);

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
			 Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case Hie_CRS:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),"		// row[0]
        		           "SUM(NumHits),"	// row[1]
        		           "SUM(Score)"		// row[2]
        	             " FROM tst_questions"
                            " WHERE CrsCod=%ld",
			    Gbl.Hierarchy.Node[Hie_CRS].HieCod);

         return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of test questions",
			 "SELECT COUNT(*),"		// row[0]
				"SUM(NumHits),"		// row[1]
				"SUM(Score)"		// row[2]
			  " FROM tst_questions"
			 " WHERE CrsCod=%ld"
			   " AND AnsType='%s'",
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/**************** Get number of courses with test questions ******************/
/*****************************************************************************/
// Returns the number of courses with test questions
// in this location (all the platform, current degree or current course)

unsigned Qst_DB_GetNumCrssWithQsts (Hie_Level_t HieLvl,Qst_AnswerType_t AnsType)
  {
   extern const char *Qst_DB_StrAnswerTypes[Qst_NUM_ANS_TYPES];

   /***** Get number of courses with test questions from database *****/
   switch (HieLvl)
     {
      case Hie_SYS:
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
      case Hie_CTY:
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
			    Gbl.Hierarchy.Node[Hie_CTY].HieCod);

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
			 Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case Hie_INS:
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
			    Gbl.Hierarchy.Node[Hie_INS].HieCod);

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
			 Gbl.Hierarchy.Node[Hie_INS].HieCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case Hie_CTR:
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
			    Gbl.Hierarchy.Node[Hie_CTR].HieCod);

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
			 Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case Hie_DEG:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
            DB_QueryCOUNT ("can not get number of courses with test questions",
        		    "SELECT COUNTDISTINCT (tst_questions.CrsCod)"
        	             " FROM crs_courses,"
        	                   "tst_questions"
                            " WHERE crs_courses.DegCod=%ld"
                              " AND crs_courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Node[Hie_DEG].HieCod);

         return (unsigned)
	 DB_QueryCOUNT ("can not get number of courses with test questions",
			 "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
			  " FROM crs_courses,"
			        "tst_questions"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=tst_questions.CrsCod"
			   " AND tst_questions.AnsType='%s'",
			 Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      case Hie_CRS:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
            DB_QueryCOUNT ("can not get number of courses with test questions",
        		    "SELECT COUNT(DISTINCT CrsCod)"
        	             " FROM tst_questions"
                            " WHERE CrsCod=%ld",
			    Gbl.Hierarchy.Node[Hie_CRS].HieCod);

         return (unsigned)
	 DB_QueryCOUNT ("can not get number of courses with test questions",
			 "SELECT COUNT(DISTINCT CrsCod)"
			  " FROM tst_questions"
			 " WHERE CrsCod=%ld"
			   " AND AnsType='%s'",
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			 Qst_DB_StrAnswerTypes[AnsType]);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/*********** Get number of courses with pluggable test questions *************/
/*****************************************************************************/
// Returns the number of courses with pluggable test questions
// in this location (all the platform, current degree or current course)

unsigned Qst_DB_GetNumCrssWithPluggableQsts (Hie_Level_t HieLvl,
                                             Qst_AnswerType_t AnsType)
  {
   extern const char *Qst_DB_StrAnswerTypes[Qst_NUM_ANS_TYPES];
   extern const char *Tst_DB_Pluggable[TstCfg_NUM_OPTIONS_PLUGGABLE];

   /***** Get number of courses with test questions from database *****/
   switch (HieLvl)
     {
      case Hie_SYS:
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
      case Hie_CTY:
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
			    Gbl.Hierarchy.Node[Hie_CTY].HieCod,
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
			    Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			    Qst_DB_StrAnswerTypes[AnsType],
			    Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);
      case Hie_INS:
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
			    Gbl.Hierarchy.Node[Hie_INS].HieCod,
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
			    Gbl.Hierarchy.Node[Hie_INS].HieCod,
			    Qst_DB_StrAnswerTypes[AnsType],
			    Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);
      case Hie_CTR:
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
			    Gbl.Hierarchy.Node[Hie_CTR].HieCod,
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
			    Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			    Qst_DB_StrAnswerTypes[AnsType],
			    Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);
      case Hie_DEG:
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
			    Gbl.Hierarchy.Node[Hie_DEG].HieCod,
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
			    Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			    Qst_DB_StrAnswerTypes[AnsType],
			    Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);
      case Hie_CRS:
         if (AnsType == Qst_ANS_UNKNOWN)	// Any type
            return (unsigned)
	    DB_QueryCOUNT ("can not get number of courses with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	             " FROM tst_questions,"
        	                   "tst_config"
                            " WHERE tst_questions.CrsCod=%ld"
                              " AND tst_questions.CrsCod=tst_config.CrsCod"
                              " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Node[Hie_CRS].HieCod,
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
			    Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			    Qst_DB_StrAnswerTypes[AnsType],
			    Tst_DB_Pluggable[TstCfg_PLUGGABLE_YES]);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/******* Get recent test questions from database giving a course code ********/
/*****************************************************************************/

unsigned Qst_DB_GetRecentQuestions (MYSQL_RES **mysql_res,
                                    long CrsCod,time_t BeginTime)
  {
   // DISTINCT is necessary to not repeat questions
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get test questions",
		   "SELECT DISTINCT "
		          "tst_questions.QstCod,"	// row[0]
			  "tst_questions.AnsType,"	// row[1]
			  "tst_questions.Shuffle,"	// row[2]
			  "tst_questions.Stem,"		// row[3]
			  "tst_questions.Feedback"	// row[4]
		    " FROM tst_questions,"
			  "tst_question_tags,"
			  "tst_tags"
		   " WHERE tst_questions.CrsCod=%ld"
		     " AND tst_questions.QstCod NOT IN"
			 " (SELECT tst_question_tags.QstCod"
			    " FROM tst_tags,"
				  "tst_question_tags"
			   " WHERE tst_tags.CrsCod=%ld"
			     " AND tst_tags.TagHidden='Y'"
			     " AND tst_tags.TagCod=tst_question_tags.TagCod)"
		     " AND tst_questions.QstCod=tst_question_tags.QstCod"
		     " AND tst_question_tags.TagCod=tst_tags.TagCod"
		     " AND tst_tags.CrsCod=%ld"
		     " AND (tst_questions.EditTime>=FROM_UNIXTIME(%ld)"
			   " OR "
			   "tst_tags.ChangeTime>=FROM_UNIXTIME(%ld))"
		" ORDER BY QstCod",
		   CrsCod,
		   CrsCod,
		   CrsCod,
		   (long) BeginTime,
		   (long) BeginTime);
  }

/*****************************************************************************/
/** Get answers of recent test questions from database giving a course code **/
/*****************************************************************************/

unsigned Qst_DB_GetRecentAnswers (MYSQL_RES **mysql_res,
                                  long CrsCod,time_t BeginTime)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get test answers",
		   "SELECT QstCod,"	// row[0]
			  "AnsInd,"	// row[1]
			  "Correct,"	// row[2]
			  "Answer,"	// row[3]
			  "Feedback"	// row[4]
		    " FROM tst_answers"
		   " WHERE QstCod IN "
			  "(SELECT tst_questions.QstCod"
			    " FROM tst_questions,"
				  "tst_question_tags,"
				  "tst_tags"
			   " WHERE tst_questions.CrsCod=%ld"
			     " AND tst_questions.QstCod NOT IN"
				 " (SELECT tst_question_tags.QstCod"
				    " FROM tst_tags,"
					  "tst_question_tags"
				   " WHERE tst_tags.CrsCod=%ld"
				     " AND tst_tags.TagHidden='Y'"
				     " AND tst_tags.TagCod=tst_question_tags.TagCod)"
			    " AND tst_questions.QstCod=tst_question_tags.QstCod"
			    " AND tst_question_tags.TagCod=tst_tags.TagCod"
			    " AND tst_tags.CrsCod=%ld"
			    " AND (tst_questions.EditTime>=FROM_UNIXTIME(%ld)"
				 " OR "
				  "tst_tags.ChangeTime>=FROM_UNIXTIME(%ld)))"
		" ORDER BY QstCod,"
			  "AnsInd",
		   CrsCod,
		   CrsCod,
		   CrsCod,
		   (long) BeginTime,
		   (long) BeginTime);
  }

/*****************************************************************************/
/****************** Get data of a question from database *********************/
/*****************************************************************************/

Exi_Exist_t Qst_DB_GetQstDataByCod (MYSQL_RES **mysql_res,long QstCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get a question",
			 "SELECT UNIX_TIMESTAMP(EditTime),"	// row[0]
				"AnsType,"			// row[1]
				"Shuffle,"			// row[2]
				"Stem,"				// row[3]
				"Feedback,"			// row[4]
				"MedCod,"			// row[5]
				"NumHits,"			// row[6]
				"NumHitsNotBlank,"		// row[7]
				"Score"				// row[8]
			  " FROM tst_questions"
			 " WHERE QstCod=%ld"
			   " AND CrsCod=%ld",	// Extra check
			 QstCod,
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod);
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
/****************** Get question codes from type and stem ********************/
/*****************************************************************************/

unsigned Qst_DB_GetQstCodsFromTypeAnsStem (MYSQL_RES **mysql_res,
                                           const struct Qst_Question *Question)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not check if a question exists",
		   "SELECT QstCod"
		    " FROM tst_questions"
		   " WHERE CrsCod=%ld"
		     " AND AnsType='%s'"
		     " AND Stem='%s'",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Qst_DB_StrAnswerTypes[Question->Answer.Type],
		   Question->Stem);
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

unsigned Qst_DB_GetAnswersData (MYSQL_RES **mysql_res,long QstCod,
			        Qst_Shuffle_t Shuffle)
  {
   extern const char *Qst_OrderByShuffle[Qst_NUM_SHUFFLE];
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
			 Qst_OrderByShuffle[Shuffle])))
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
/*************** Get answers correctness for a question **********************/
/*****************************************************************************/

unsigned Qst_DB_GetQstAnswersCorr (MYSQL_RES **mysql_res,long QstCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get correctness of answers of a question",
		   "SELECT Correct"		// row[0]
		    " FROM tst_answers"
		   " WHERE QstCod=%ld"
		" ORDER BY AnsInd",
		   QstCod);
  }

/*****************************************************************************/
/*********** Get suffled/not-shuffled answers indexes of question ************/
/*****************************************************************************/

unsigned Qst_DB_GetShuffledAnswersIndexes (MYSQL_RES **mysql_res,
                                           const struct Qst_Question *Question)
  {
   extern const char *Qst_OrderByShuffle[Qst_NUM_SHUFFLE];

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get questions of a game",
		   "SELECT AnsInd"	// row[0]
		    " FROM tst_answers"
		   " WHERE QstCod=%ld"
		" ORDER BY %s",
		   Question->QstCod,
		   Qst_OrderByShuffle[Question->Answer.Shuffle]);
  }

/*****************************************************************************/
/****** Get media codes associated to stems of test questions in course ******/
/*****************************************************************************/

unsigned Qst_DB_GetMedCodsFromStemsOfQstsInCrs (MYSQL_RES **mysql_res,long CrsCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get media",
		   "SELECT MedCod"	// row[0]
		    " FROM tst_questions"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/***** Get media codes associated to answers of test questions in course *****/
/*****************************************************************************/

unsigned Qst_DB_GetMedCodsFromAnssOfQstsInCrs (MYSQL_RES **mysql_res,long CrsCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get media",
		   "SELECT tst_answers.MedCod"
		    " FROM tst_questions,"
			  "tst_answers"
		   " WHERE tst_questions.CrsCod=%ld"
		     " AND tst_questions.QstCod=tst_answers.QstCod",
		   CrsCod);
  }

/*****************************************************************************/
/************** Get media code associated to stem of a question **************/
/*****************************************************************************/

unsigned Qst_DB_GetMedCodFromStemOfQst (MYSQL_RES **mysql_res,long CrsCod,long QstCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get media",
		   "SELECT MedCod"
		    " FROM tst_questions"
		   " WHERE QstCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
		   QstCod,
		   CrsCod);
  }

/*****************************************************************************/
/************** Get media code associated to stem of a question **************/
/*****************************************************************************/

unsigned Qst_DB_GetMedCodsFromAnssOfQst (MYSQL_RES **mysql_res,long CrsCod,long QstCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get media",
		   "SELECT tst_answers.MedCod"
		    " FROM tst_answers,"
			  "tst_questions"
		   " WHERE tst_answers.QstCod=%ld"
		     " AND tst_answers.QstCod=tst_questions.QstCod"
		     " AND tst_questions.CrsCod=%ld"	// Extra check
		     " AND tst_questions.QstCod=%ld",	// Extra check
		   QstCod,
		   CrsCod,
		   QstCod);
  }

/*****************************************************************************/
/********* Get media code associated to an answer of a test question *********/
/*****************************************************************************/

long Qst_DB_GetMedCodFromAnsOfQst (long QstCod,unsigned AnsInd)
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
/********************* Remove all questions in a course **********************/
/*****************************************************************************/

void Qst_DB_RemoveQstsInCrs (long CrsCod)
  {
   DB_QueryDELETE ("can not remove questions in a course",
		   "DELETE FROM tst_questions"
		   " WHERE CrsCod=%ld",
		   CrsCod);
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
/*************** Remove answers from all questions in a course ****************/
/*****************************************************************************/

void Qst_DB_RemAnssFromQstsInCrs (long CrsCod)
  {
   DB_QueryDELETE ("can not remove answers of tests of a course",
		   "DELETE FROM tst_answers"
		   " USING tst_questions,"
		          "tst_answers"
                   " WHERE tst_questions.CrsCod=%ld"
                     " AND tst_questions.QstCod=tst_answers.QstCod",
		   CrsCod);
  }

/*****************************************************************************/
/******************** Remove answers from a test question ********************/
/*****************************************************************************/

void Qst_DB_RemAnsFromQst (long QstCod)
  {
   DB_QueryDELETE ("can not remove the answers of a question",
		   "DELETE FROM tst_answers"
		   " WHERE QstCod=%ld",
		   QstCod);
  }
