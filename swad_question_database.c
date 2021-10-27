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
/******************* Get questions for a new test print **********************/
/*****************************************************************************/

#define Tst_MAX_BYTES_QUERY_QUESTIONS (16 * 1024 - 1)

unsigned Qst_DB_GetQuestionsForNewTestPrint (MYSQL_RES **mysql_res,
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
