// swad_test.c: self-assessment tests

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <limits.h>		// For UINT_MAX
#include <linux/limits.h>	// For PATH_MAX
#include <mysql/mysql.h>	// To access MySQL databases
#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For exit, system, malloc, free, etc
#include <string.h>		// For string functions
#include <sys/stat.h>		// For mkdir
#include <sys/types.h>		// For mkdir

#include "swad_action.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_language.h"
#include "swad_match.h"
#include "swad_media.h"
#include "swad_parameter.h"
#include "swad_theme.h"
#include "swad_test.h"
#include "swad_test_config.h"
#include "swad_test_import.h"
#include "swad_test_visibility.h"
#include "swad_user.h"
#include "swad_xml.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

// strings are limited to Tst_MAX_BYTES_ANSWER_TYPE characters
const char *Tst_StrAnswerTypesXML[Tst_NUM_ANS_TYPES] =
  {
   [Tst_ANS_INT            ] = "int",
   [Tst_ANS_FLOAT          ] = "float",
   [Tst_ANS_TRUE_FALSE     ] = "TF",
   [Tst_ANS_UNIQUE_CHOICE  ] = "uniqueChoice",
   [Tst_ANS_MULTIPLE_CHOICE] = "multipleChoice",
   [Tst_ANS_TEXT           ] = "text",
  };

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

#define Tst_MAX_BYTES_TAGS_LIST		(16 * 1024)
#define Tst_MAX_BYTES_FLOAT_ANSWER	30	// Maximum length of the strings that store an floating point answer

static const char *Tst_StrAnswerTypesDB[Tst_NUM_ANS_TYPES] =
  {
   [Tst_ANS_INT            ] = "int",
   [Tst_ANS_FLOAT          ] = "float",
   [Tst_ANS_TRUE_FALSE     ] = "true_false",
   [Tst_ANS_UNIQUE_CHOICE  ] = "unique_choice",
   [Tst_ANS_MULTIPLE_CHOICE] = "multiple_choice",
   [Tst_ANS_TEXT           ] = "text",
  };

// Test images will be saved with:
// - maximum width of Tst_IMAGE_SAVED_MAX_HEIGHT
// - maximum height of Tst_IMAGE_SAVED_MAX_HEIGHT
// - maintaining the original aspect ratio (aspect ratio recommended: 3:2)
#define Tst_IMAGE_SAVED_MAX_WIDTH	768
#define Tst_IMAGE_SAVED_MAX_HEIGHT	768
#define Tst_IMAGE_SAVED_QUALITY		 90	// 1 to 100

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

#define Tst_NUM_STATUS 2
typedef enum
  {
   Tst_STATUS_SHOWN_BUT_NOT_ASSESSED	= 0,
   Tst_STATUS_ASSESSED			= 1,
   Tst_STATUS_ERROR			= 2,
  } Tst_Status_t;

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

static void Tst_TstConstructor (struct Tst_Test *Test);
static void Tst_TstDestructor (struct Tst_Test *Test);
static void Tst_ResetTags (struct Tst_Tags *Tags);
static void Tst_FreeTagsList (struct Tst_Tags *Tags);

static void Tst_ShowFormRequestTest (struct Tst_Test *Test);

static void Tst_PutCheckBoxAllowTeachers (bool AllowTeachers);

static void Tst_GetQuestionsAndAnswersFromForm (struct TsR_Result *Result);
static bool Tst_CheckIfNextTstAllowed (void);
static void Tst_SetTstStatus (unsigned NumTst,Tst_Status_t TstStatus);
static Tst_Status_t Tst_GetTstStatus (unsigned NumTst);
static unsigned Tst_GetNumAccessesTst (void);
static void Tst_ShowTestQuestionsWhenSeeing (unsigned NumQsts,
                                             MYSQL_RES *mysql_res);
static void Tst_ShowOneTestQuestionWhenSeeing (unsigned NumQst,
                                               struct Tst_Question *Question);
static void Tst_ShowTestResultAfterAssess (long TstCod,
                                           struct TsR_Result *Result);
static void Tst_WriteQstAndAnsTestToAnswer (unsigned NumQst,
                                            struct Tst_Question *Question,
                                            MYSQL_ROW row);

static void Tst_PutFormToEditQstMedia (const struct Media *Media,int NumMediaInForm,
                                       bool OptionsDisabled);
static void Tst_UpdateScoreQst (long QstCod,double ScoreThisQst,bool AnswerIsNotBlank);
static void Tst_UpdateMyNumAccessTst (unsigned NumAccessesTst);
static void Tst_UpdateLastAccTst (unsigned NumQsts);

static void Tst_ShowFormRequestEditTests (struct Tst_Test *Test);
static void Tst_ShowFormRequestSelectTestsForGame (struct Tst_Test *Test);
static bool Tst_CheckIfICanEditTests (void);
static void Tst_PutIconsTests (void *TestPtr);
static void Tst_PutButtonToAddQuestion (void);

static long Tst_GetParamTagCode (void);
static bool Tst_CheckIfCurrentCrsHasTestTags (void);
static unsigned Tst_GetAllTagsFromCurrentCrs (MYSQL_RES **mysql_res);
static unsigned Tst_GetEnabledTagsFromThisCrs (MYSQL_RES **mysql_res);
static void Tst_ShowFormSelTags (const struct Tst_Tags *Tags,
                                 MYSQL_RES *mysql_res,
                                 bool ShowOnlyEnabledTags);
static void Tst_ShowFormEditTags (void);
static void Tst_PutIconEnable (long TagCod,const char *TagTxt);
static void Tst_PutIconDisable (long TagCod,const char *TagTxt);
static void Tst_ShowFormConfigTst (void);

static void Tst_PutInputFieldNumQst (const char *Field,const char *Label,
                                     unsigned Value);

static void Tst_ShowFormAnswerTypes (const struct Tst_AnswerTypes *AnswerTypes);
static void Tst_GetQuestions (struct Tst_Test *Test,MYSQL_RES **mysql_res);
static void Tst_GetQuestionsForTest (struct Tst_Test *Test,MYSQL_RES **mysql_res);
static void Tst_ListOneQstToEdit (struct Tst_Test *Test);
static void Tst_ListOneOrMoreQuestionsForEdition (struct Tst_Test *Test,
                                                  MYSQL_RES *mysql_res);
static void Tst_WriteHeadingRowQuestionsForEdition (const struct Tst_Test *Test);
static void Tst_WriteQuestionListing (struct Tst_Test *Test,unsigned NumQst);
static void Tst_ListOneOrMoreQuestionsForSelection (unsigned NumQsts,
                                                    MYSQL_RES *mysql_res);
static void Tst_WriteQuestionRowForSelection (unsigned NumQst,
                                              struct Tst_Question *Question);

static void Tst_WriteAnswersSeeing (unsigned NumQst,
                                    struct Tst_Question *Question,
                                    bool Shuffle);
static void Tst_WriteAnswersResult (struct UsrData *UsrDat,
                                    const struct TsR_Result *Result,
                                    unsigned NumQst,
				    struct Tst_Question *Question,
				    unsigned Visibility,
                                    double *ScoreThisQst,bool *AnswerIsNotBlank);

static void Tst_WriteTFAnsListing (const struct Tst_Question *Question,
                                   MYSQL_RES *mysql_res);
static void Tst_WriteTFAnsSeeing (unsigned NumQst);
static void Tst_WriteTFAnsResult (struct UsrData *UsrDat,
                                  const struct TsR_Result *Result,
				  unsigned NumQst,
				  const struct Tst_Question *Question,
				  MYSQL_RES *mysql_res,
				  unsigned Visibility,
                                  double *ScoreThisQst,
                                  bool *AnswerIsNotBlank);

static void Tst_WriteChoiceAnsListing (struct Tst_Question *Question,
                                       MYSQL_RES *mysql_res);
static void Tst_WriteChoiceAnsSeeing (unsigned NumQst,
                                      struct Tst_Question *Question,
                                      bool Shuffle);
static void Tst_WriteChoiceAnsResult (struct UsrData *UsrDat,
                                      const struct TsR_Result *Result,
				      unsigned NumQst,
				      struct Tst_Question *Question,
				      MYSQL_RES *mysql_res,
				      unsigned Visibility,
                                      double *ScoreThisQst,
                                      bool *AnswerIsNotBlank);
static void Tst_GetChoiceAns (struct Tst_Question *Question,MYSQL_RES *mysql_res);

static void Tst_WriteTextAnsSeeing (unsigned NumQst);
static void Tst_WriteTextAnsResult (struct UsrData *UsrDat,
                                    const struct TsR_Result *Result,
				    unsigned NumQst,
				    struct Tst_Question *Question,
				    MYSQL_RES *mysql_res,
				    unsigned Visibility,
                                    double *ScoreThisQst,
                                    bool *AnswerIsNotBlank);

static void Tst_WriteIntAnsListing (const struct Tst_Question *Question,
                                 MYSQL_RES *mysql_res);
static void Tst_WriteIntAnsSeeing (unsigned NumQst);
static void Tst_WriteIntAnsResult (struct UsrData *UsrDat,
                                   const struct TsR_Result *Result,
				   unsigned NumQst,
				   const struct Tst_Question *Question,
				   MYSQL_RES *mysql_res,
				   unsigned Visibility,
                                   double *ScoreThisQst,
                                   bool *AnswerIsNotBlank);

static void Tst_WriteFloatAnsEdit (const struct Tst_Question *Question,
                                   MYSQL_RES *mysql_res);
static void Tst_WriteFloatAnsSeeing (unsigned NumQst);
static void Tst_WriteFloatAnsResult (struct UsrData *UsrDat,
                                     const struct TsR_Result *Result,
				     unsigned NumQst,
				     const struct Tst_Question *Question,
				     MYSQL_RES *mysql_res,
				     unsigned Visibility,
                                     double *ScoreThisQst,
                                     bool *AnswerIsNotBlank);

static void Tst_WriteHeadUserCorrect (struct UsrData *UsrDat);
static void Tst_WriteScoreStart (unsigned ColSpan);
static void Tst_WriteScoreEnd (void);
static void Tst_WriteParamQstCod (unsigned NumQst,long QstCod);
static bool Tst_GetParamsTst (struct Tst_Test *Test,
                              Tst_ActionToDoWithQuestions_t ActionToDoWithQuestions);
static unsigned Tst_GetAndCheckParamNumTst (void);
static unsigned Tst_GetParamNumQsts (void);
static unsigned Tst_CountNumTagsInList (const struct Tst_Tags *Tags);
static int Tst_CountNumAnswerTypesInList (const struct Tst_AnswerTypes *AnswerTypes);

static void Tst_PutFormEditOneQst (struct Tst_Question *Question,
	                           char Stem[Cns_MAX_BYTES_TEXT + 1],
                                   char Feedback[Cns_MAX_BYTES_TEXT + 1]);
static void Tst_PutFloatInputField (const char *Label,const char *Field,
                                    const struct Tst_Question *Question,
                                    unsigned Index);
static void Tst_PutTFInputField (const struct Tst_Question *Question,
                                 const char *Label,char Value);

static void Tst_FreeTextChoiceAnswers (struct Tst_Question *Question);
static void Tst_FreeTextChoiceAnswer (struct Tst_Question *Question,unsigned NumOpt);

static void Tst_ResetMediaOfQuestion (struct Tst_Question *Question);
static void Tst_FreeMediaOfQuestion (struct Tst_Question *Question);

static void Tst_GetQstDataFromDB (struct Tst_Question *Question,
                                  char Stem[Cns_MAX_BYTES_TEXT + 1],
                                  char Feedback[Cns_MAX_BYTES_TEXT + 1]);
static long Tst_GetMedCodFromDB (long CrsCod,long QstCod,int NumOpt);
static void Tst_GetMediaFromDB (long CrsCod,long QstCod,int NumOpt,
                                struct Media *Media);

static Tst_AnswerType_t Tst_ConvertFromUnsignedStrToAnsTyp (const char *UnsignedStr);
static void Tst_GetQstFromForm (struct Tst_Question *Question,
                                char *Stem,char *Feedback);
static void Tst_MoveMediaToDefinitiveDirectories (struct Tst_Question *Question);

static long Tst_GetTagCodFromTagTxt (const char *TagTxt);
static long Tst_CreateNewTag (long CrsCod,const char *TagTxt);
static void Tst_EnableOrDisableTag (long TagCod,bool TagHidden);

static void Tst_PutParamsRemoveSelectedQsts (void *TestPtr);
static void Tst_PutIconToRemoveOneQst (void *QstCodPtr);
static void Tst_PutParamsRemoveOnlyThisQst (void *QstCodPtr);
static void Tst_PutParamsRemoveOneQstWhileEditing (void *TestPtr);
static void Tst_RemoveOneQstFromDB (long CrsCod,long QstCod);

static long Tst_GetQstCod (void);

static void Tst_InsertOrUpdateQstIntoDB (struct Tst_Question *Question);
static void Tst_InsertTagsIntoDB (const struct Tst_Question *Question);
static void Tst_InsertAnswersIntoDB (struct Tst_Question *Question);

static void Tst_RemAnsFromQst (long QstCod);
static void Tst_RemTagsFromQst (long QstCod);
static void Tst_RemoveUnusedTagsFromCrs (long CrsCod);

static void Tst_RemoveAllMedFilesFromStemOfAllQstsInCrs (long CrsCod);
static void Tst_RemoveMediaFromAllAnsOfQst (long CrsCod,long QstCod);
static void Tst_RemoveAllMedFilesFromAnsOfAllQstsInCrs (long CrsCod);

static unsigned Tst_GetNumTstQuestions (Hie_Level_t Scope,Tst_AnswerType_t AnsType,struct Tst_Stats *Stats);
static unsigned Tst_GetNumCoursesWithTstQuestions (Hie_Level_t Scope,Tst_AnswerType_t AnsType);
static unsigned Tst_GetNumCoursesWithPluggableTstQuestions (Hie_Level_t Scope,Tst_AnswerType_t AnsType);

/*****************************************************************************/
/********************* Request a self-assessment test ************************/
/*****************************************************************************/

void Tst_RequestTest (void)
  {
   struct Tst_Test Test;

   /***** Create test *****/
   Tst_TstConstructor (&Test);

   /***** Show form to generate a self-assessment test *****/
   Tst_ShowFormRequestTest (&Test);

   /***** Destroy test *****/
   Tst_TstDestructor (&Test);
  }

/*****************************************************************************/
/***************************** Test constructor ******************************/
/*****************************************************************************/

static void Tst_TstConstructor (struct Tst_Test *Test)
  {
   /***** Reset tags *****/
   Tst_ResetTags (&Test->Tags);

   /***** Reset answer types *****/
   Test->AnswerTypes.All = false;
   Test->AnswerTypes.List[0] = '\0';

   /***** Reset selected order *****/
   Test->SelectedOrder = Tst_DEFAULT_ORDER;

   /***** Question constructor *****/
   Tst_QstConstructor (&Test->Question);
  }

/*****************************************************************************/
/****************************** Test destructor ******************************/
/*****************************************************************************/

static void Tst_TstDestructor (struct Tst_Test *Test)
  {
   /***** Question destructor *****/
   Tst_QstDestructor (&Test->Question);

   /***** Free tag list *****/
   Tst_FreeTagsList (&Test->Tags);
  }

/*****************************************************************************/
/********************************* Reset tags ********************************/
/*****************************************************************************/

static void Tst_ResetTags (struct Tst_Tags *Tags)
  {
   Tags->Num  = 0;
   Tags->All  = false;
   Tags->List = NULL;
  }

/*****************************************************************************/
/**************** Free memory allocated for the list of tags *****************/
/*****************************************************************************/

static void Tst_FreeTagsList (struct Tst_Tags *Tags)
  {
   if (Tags->List)
     {
      free (Tags->List);
      Tst_ResetTags (Tags);
     }
  }

/*****************************************************************************/
/*************** Show form to generate a self-assessment test ****************/
/*****************************************************************************/

static void Tst_ShowFormRequestTest (struct Tst_Test *Test)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *Txt_Take_a_test;
   extern const char *Txt_No_of_questions;
   extern const char *Txt_Generate_test;
   extern const char *Txt_No_test_questions;
   MYSQL_RES *mysql_res;

   /***** Read test configuration from database *****/
   TstCfg_GetConfigFromDB ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Take_a_test,
                 Tst_PutIconsTests,Test,
                 Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);

   /***** Get tags *****/
   if ((Test->Tags.Num = Tst_GetEnabledTagsFromThisCrs (&mysql_res)) != 0)
     {
      /***** Check if minimum date-time of next access to test is older than now *****/
      if (Tst_CheckIfNextTstAllowed ())
        {
         Frm_StartForm (ActSeeTst);

         HTM_TABLE_BeginPadding (2);

         /***** Selection of tags *****/
         Tst_ShowFormSelTags (&Test->Tags,mysql_res,true);

         /***** Selection of types of answers *****/
         Tst_ShowFormAnswerTypes (&Test->AnswerTypes);

         /***** Number of questions to generate ****/
         HTM_TR_Begin (NULL);

         /* Label */
         Frm_LabelColumn ("RT","NumQst",Txt_No_of_questions);

         /* Data */
         HTM_TD_Begin ("class=\"LT\"");
         HTM_INPUT_LONG ("NumQst",
			 (long) TstCfg_GetConfigMin (),
			 (long) TstCfg_GetConfigMax (),
			 (long) TstCfg_GetConfigDef (),
			 TstCfg_GetConfigMin () == TstCfg_GetConfigMax (),
			 "id=\"NumQst\"");
         HTM_TD_End ();

         HTM_TR_End ();

         HTM_TABLE_End ();

         /***** Send button *****/
         Btn_PutConfirmButton (Txt_Generate_test);
         Frm_EndForm ();
        }
     }
   else
     {
      /***** Warning message *****/
      Ale_ShowAlert (Ale_INFO,Txt_No_test_questions);

      /***** Button to create a new question *****/
      if (Tst_CheckIfICanEditTests ())
         Tst_PutButtonToAddQuestion ();
     }

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Generate self-assessment test ************************/
/*****************************************************************************/

void Tst_ShowNewTest (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *Txt_No_questions_found_matching_your_search_criteria;
   extern const char *Txt_Test;
   extern const char *Txt_Done_assess_test;
   struct Tst_Test Test;
   MYSQL_RES *mysql_res;
   unsigned NumAccessesTst;

   /***** Create test *****/
   Tst_TstConstructor (&Test);

   /***** Read test configuration from database *****/
   TstCfg_GetConfigFromDB ();

   if (Tst_CheckIfNextTstAllowed ())
     {
      /***** Check that all parameters used to generate a test are valid *****/
      if (Tst_GetParamsTst (&Test,Tst_SHOW_TEST_TO_ANSWER))	// Get parameters from form
        {
         /***** Get questions *****/
	 Tst_GetQuestionsForTest (&Test,&mysql_res);
         if (Test.NumQsts)
           {
            /***** Get and update number of hits *****/
            NumAccessesTst = Tst_GetNumAccessesTst () + 1;
            if (Gbl.Usrs.Me.IBelongToCurrentCrs)
	       Tst_UpdateMyNumAccessTst (NumAccessesTst);

	    /***** Begin box *****/
	    Box_BoxBegin (NULL,Txt_Test,
	                  NULL,NULL,
	                  Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);
	    Lay_WriteHeaderClassPhoto (false,false,
				       Gbl.Hierarchy.Ins.InsCod,
				       Gbl.Hierarchy.Deg.DegCod,
				       Gbl.Hierarchy.Crs.CrsCod);

            /***** Begin form *****/
            Frm_StartForm (ActAssTst);
            Par_PutHiddenParamUnsigned (NULL,"NumTst",NumAccessesTst);
            Par_PutHiddenParamUnsigned (NULL,"NumQst",Test.NumQsts);

            /***** List the questions *****/
            Tst_ShowTestQuestionsWhenSeeing (Test.NumQsts,mysql_res);

	    /***** Test result will be saved? *****/
	    Tst_PutCheckBoxAllowTeachers (true);

            /***** End form *****/
            Btn_PutConfirmButton (Txt_Done_assess_test);
            Frm_EndForm ();

            /***** End box *****/
	    Box_BoxEnd ();

            /***** Set test status *****/
            Tst_SetTstStatus (NumAccessesTst,Tst_STATUS_SHOWN_BUT_NOT_ASSESSED);

            /***** Update date-time of my next allowed access to test *****/
            if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
               Tst_UpdateLastAccTst (Test.NumQsts);
           }
         else	// No questions found
           {
            Ale_ShowAlert (Ale_INFO,Txt_No_questions_found_matching_your_search_criteria);
            Tst_ShowFormRequestTest (&Test);	// Show the form again
           }

         /***** Free structure that stores the query result *****/
         DB_FreeMySQLResult (&mysql_res);
        }
      else
         Tst_ShowFormRequestTest (&Test);	// Show the form again
     }

   /***** Destroy test *****/
   Tst_TstDestructor (&Test);
  }

/*****************************************************************************/
/************ Put checkbox to allow teachers to see test result **************/
/*****************************************************************************/

static void Tst_PutCheckBoxAllowTeachers (bool AllowTeachers)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Allow_teachers_to_consult_this_test;

   /***** Test result will be available for teachers? *****/
   HTM_DIV_Begin ("class=\"CM\"");
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_INPUT_CHECKBOX ("AllowTchs",HTM_DONT_SUBMIT_ON_CHANGE,
		       "value=\"Y\"%s",
                       AllowTeachers ? " checked=\"checked\"" :	// Teachers can see test result
                		       "");
   HTM_TxtF ("&nbsp;%s",Txt_Allow_teachers_to_consult_this_test);
   HTM_LABEL_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Request the assessment of a test **********************/
/*****************************************************************************/

void Tst_RequestAssessTest (void)
  {
   extern const char *Txt_The_test_X_has_already_been_assessed_previously;
   extern const char *Txt_There_was_an_error_in_assessing_the_test_X;
   unsigned NumTst;
   // long TstCod = -1L;	// Initialized to avoid warning
   struct TsR_Result Result;

   /***** Read test configuration from database *****/
   TstCfg_GetConfigFromDB ();

   /***** Get number of this test from form *****/
   NumTst = Tst_GetAndCheckParamNumTst ();

   /****** Get test status in database for this session-course-num.test *****/
   switch (Tst_GetTstStatus (NumTst))
     {
      case Tst_STATUS_SHOWN_BUT_NOT_ASSESSED:
         /***** Get parameters from the form *****/
	 /* Get questions and answers from form to assess a test */
	 Tst_GetQuestionsAndAnswersFromForm (&Result);

	 /***** Show question and button to send the test *****/
	 /* Start alert */
	 Ale_ShowAlertAndButton1 (Ale_INFO,"Por favor, revise el test antes de enviarlo");

	 /* Show test again */


	 /* End alert */
         Ale_ShowAlertAndButton2 (ActAssTst,NULL,NULL,
                                  NULL,NULL,
                                  Btn_CONFIRM_BUTTON,"Enviar test");
         break;
      case Tst_STATUS_ASSESSED:
         Ale_ShowAlert (Ale_WARNING,Txt_The_test_X_has_already_been_assessed_previously,
                        NumTst);
         break;
      case Tst_STATUS_ERROR:
         Ale_ShowAlert (Ale_WARNING,Txt_There_was_an_error_in_assessing_the_test_X,
                        NumTst);
         break;
     }
  }

/*****************************************************************************/
/******************************** Assess a test ******************************/
/*****************************************************************************/

void Tst_AssessTest (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *Txt_Test_result;
   extern const char *Txt_Test_No_X_that_you_make_in_this_course;
   extern const char *Txt_Score;
   extern const char *Txt_Grade;
   extern const char *Txt_The_test_X_has_already_been_assessed_previously;
   extern const char *Txt_There_was_an_error_in_assessing_the_test_X;
   unsigned NumTst;
   long TstCod = -1L;	// Initialized to avoid warning
   struct TsR_Result Result;

   /***** Read test configuration from database *****/
   TstCfg_GetConfigFromDB ();

   /***** Get number of this test from form *****/
   NumTst = Tst_GetAndCheckParamNumTst ();

   /****** Get test status in database for this session-course-num.test *****/
   switch (Tst_GetTstStatus (NumTst))
     {
      case Tst_STATUS_SHOWN_BUT_NOT_ASSESSED:
         /***** Get parameters from the form *****/
	 /* Get questions and answers from form to assess a test */
	 Tst_GetQuestionsAndAnswersFromForm (&Result);

	 /***** Create new test in database to store the result *****/
	 TstCod = TsR_CreateTestResultInDB (&Result);

	 /***** Begin box *****/
	 Box_BoxBegin (NULL,Txt_Test_result,
	               NULL,NULL,
	               Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);
	 Lay_WriteHeaderClassPhoto (false,false,
				    Gbl.Hierarchy.Ins.InsCod,
				    Gbl.Hierarchy.Deg.DegCod,
				    Gbl.Hierarchy.Crs.CrsCod);

	 /***** Header *****/
	 if (Gbl.Usrs.Me.IBelongToCurrentCrs)
	   {
	    HTM_DIV_Begin ("class=\"TEST_SUBTITLE\"");
	    HTM_TxtF (Txt_Test_No_X_that_you_make_in_this_course,NumTst);
	    HTM_DIV_End ();
	   }

	 /***** Write answers and solutions *****/
         HTM_TABLE_BeginWideMarginPadding (10);
	 Tst_ShowTestResultAfterAssess (TstCod,&Result);
	 HTM_TABLE_End ();

	 /***** Write total score and grade *****/
	 if (TsV_IsVisibleTotalScore (TstCfg_GetConfigVisibility ()))
	   {
	    HTM_DIV_Begin ("class=\"DAT_N_BOLD CM\"");
	    HTM_TxtColonNBSP (Txt_Score);
	    HTM_Double2Decimals (Result.Score);
	    HTM_BR ();
	    HTM_TxtColonNBSP (Txt_Grade);
	    Tst_ComputeAndShowGrade (Result.NumQsts,
	                             Result.Score,
	                             TsR_SCORE_MAX);
	    HTM_DIV_End ();
	   }

	 /***** End box *****/
	 Box_BoxEnd ();

	 /***** Store test result in database *****/
	 TsR_StoreScoreOfTestResultInDB (TstCod,&Result);

         /***** Set test status *****/
         Tst_SetTstStatus (NumTst,Tst_STATUS_ASSESSED);
         break;
      case Tst_STATUS_ASSESSED:
         Ale_ShowAlert (Ale_WARNING,Txt_The_test_X_has_already_been_assessed_previously,
                        NumTst);
         break;
      case Tst_STATUS_ERROR:
         Ale_ShowAlert (Ale_WARNING,Txt_There_was_an_error_in_assessing_the_test_X,
                        NumTst);
         break;
     }
  }

/*****************************************************************************/
/*********** Get questions and answers from form to assess a test ************/
/*****************************************************************************/

static void Tst_GetQuestionsAndAnswersFromForm (struct TsR_Result *Result)
  {
   unsigned NumQst;
   char StrQstIndOrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Qstxx...x", "Indxx...x" or "Ansxx...x"

   /***** Get number of questions *****/
   Result->NumQsts = Tst_GetParamNumQsts ();

   /***** Get questions and answers *****/
   for (NumQst = 0;
	NumQst < Result->NumQsts;
	NumQst++)
     {
      /* Get question code */
      snprintf (StrQstIndOrAns,sizeof (StrQstIndOrAns),
	        "Qst%010u",
		NumQst);
      if ((Result->QstCodes[NumQst] = Par_GetParToLong (StrQstIndOrAns)) <= 0)
	 Lay_ShowErrorAndExit ("Code of question is missing.");

      /* Get indexes for this question */
      snprintf (StrQstIndOrAns,sizeof (StrQstIndOrAns),
	        "Ind%010u",
		NumQst);
      Par_GetParMultiToText (StrQstIndOrAns,Result->StrIndexes[NumQst],
                             Tst_MAX_BYTES_INDEXES_ONE_QST);  /* If choice ==> "0", "1", "2",... */

      /* Get answers selected by user for this question */
      snprintf (StrQstIndOrAns,sizeof (StrQstIndOrAns),
	        "Ans%010u",
		NumQst);
      Par_GetParMultiToText (StrQstIndOrAns,Result->StrAnswers[NumQst],
                             Tst_MAX_BYTES_ANSWERS_ONE_QST);  /* If answer type == T/F ==> " ", "T", "F"; if choice ==> "0", "2",... */
     }

   /***** Get if test result will be visible by teachers *****/
   Result->AllowTeachers = Par_GetParToBool ("AllowTchs");
  }

/*****************************************************************************/
/************ Compute and show total grade out of maximum grade **************/
/*****************************************************************************/

void Tst_ComputeAndShowGrade (unsigned NumQsts,double Score,double MaxGrade)
  {
   Tst_ShowGrade (Tst_ComputeGrade (NumQsts,Score,MaxGrade),MaxGrade);
  }

/*****************************************************************************/
/**************** Compute total grade out of maximum grade *******************/
/*****************************************************************************/

double Tst_ComputeGrade (unsigned NumQsts,double Score,double MaxGrade)
  {
   double MaxScore;
   double Grade;

   /***** Compute grade *****/
   if (NumQsts)
     {
      MaxScore = (double) NumQsts;
      Grade = Score * MaxGrade / MaxScore;
     }
   else
      Grade = 0.0;

   return Grade;
  }

/*****************************************************************************/
/****************** Show total grade out of maximum grade ********************/
/*****************************************************************************/

void Tst_ShowGrade (double Grade,double MaxGrade)
  {
   /***** Write grade over maximum grade *****/
   HTM_Double2Decimals (Grade);
   HTM_Txt ("/");
   HTM_Double2Decimals (MaxGrade);
  }

/*****************************************************************************/
/************** Check minimum date-time of next access to test ***************/
/*****************************************************************************/
// Return true if allowed date-time of next access to test is older than now

static bool Tst_CheckIfNextTstAllowed (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *Txt_Test;
   extern const char *Txt_You_can_not_take_a_new_test_until;
   extern const char *Txt_Today;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long NumSecondsFromNowToNextAccTst = -1L;	// Access allowed when this number <= 0
   time_t TimeNextTestUTC = (time_t) 0;

   /***** Teachers and superusers are allowed to do all tests they want *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
       Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      return true;

   /***** Get date of next allowed access to test from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get last access to test",
		       "SELECT UNIX_TIMESTAMP(LastAccTst+INTERVAL (NumQstsLastTst*%lu) SECOND)-"
			      "UNIX_TIMESTAMP(),"						// row[0]
			      "UNIX_TIMESTAMP(LastAccTst+INTERVAL (NumQstsLastTst*%lu) SECOND)"	// row[1]
		       " FROM crs_usr"
		       " WHERE CrsCod=%ld AND UsrCod=%ld",
		       TstCfg_GetConfigMinTimeNxtTstPerQst (),
		       TstCfg_GetConfigMinTimeNxtTstPerQst (),
		       Gbl.Hierarchy.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod) == 1)
     {
      /* Get seconds from now to next access to test */
      row = mysql_fetch_row (mysql_res);
      if (row[0])
         if (sscanf (row[0],"%ld",&NumSecondsFromNowToNextAccTst) == 1)
            /* Time UTC of next access allowed (row[1]) */
            TimeNextTestUTC = Dat_GetUNIXTimeFromStr (row[1]);
     }
   else
      Lay_ShowErrorAndExit ("Error when reading date of next allowed access to test.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Check if access is allowed *****/
   if (NumSecondsFromNowToNextAccTst > 0)
     {
      /***** Write warning *****/
      Ale_ShowAlert (Ale_WARNING,"%s:<br /><span id=\"date_next_test\"></span>."
		     "<script type=\"text/javascript\">"
		     "writeLocalDateHMSFromUTC('date_next_test',%ld,"
		     "%u,',&nbsp;','%s',true,true,0x7);"
		     "</script>",
		     Txt_You_can_not_take_a_new_test_until,
		     (long) TimeNextTestUTC,
		     (unsigned) Gbl.Prefs.DateFormat,Txt_Today);

      return false;
     }
   return true;
  }

/*****************************************************************************/
/****************************** Update test status ***************************/
/*****************************************************************************/

static void Tst_SetTstStatus (unsigned NumTst,Tst_Status_t TstStatus)
  {
   /***** Delete old status from expired sessions *****/
   DB_QueryDELETE ("can not remove old status of tests",
		   "DELETE FROM tst_status"
                   " WHERE SessionId NOT IN"
                   " (SELECT SessionId FROM sessions)");

   /***** Update database *****/
   DB_QueryREPLACE ("can not update status of test",
		    "REPLACE INTO tst_status"
	            " (SessionId,CrsCod,NumTst,Status)"
	            " VALUES"
	            " ('%s',%ld,%u,%u)",
		    Gbl.Session.Id,Gbl.Hierarchy.Crs.CrsCod,
		    NumTst,(unsigned) TstStatus);
  }

/*****************************************************************************/
/****************************** Update test status ***************************/
/*****************************************************************************/

static Tst_Status_t Tst_GetTstStatus (unsigned NumTst)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;
   Tst_Status_t TstStatus = Tst_STATUS_ERROR;

   /***** Get status of test from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get status of test",
		       "SELECT Status"		// row[0]
		       " FROM tst_status"
		       " WHERE SessionId='%s'"
		       " AND CrsCod=%ld"
		       " AND NumTst=%u",
		       Gbl.Session.Id,Gbl.Hierarchy.Crs.CrsCod,NumTst) == 1)
     {
      /* Get number of hits */
      row = mysql_fetch_row (mysql_res);
      if (row[0])
         if (sscanf (row[0],"%u",&UnsignedNum) == 1)
            if (UnsignedNum < Tst_NUM_STATUS)
               TstStatus = (Tst_Status_t) UnsignedNum;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return TstStatus;
  }

/*****************************************************************************/
/************************* Get number of hits to test ************************/
/*****************************************************************************/

static unsigned Tst_GetNumAccessesTst (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumAccessesTst = 0;

   if (Gbl.Usrs.Me.IBelongToCurrentCrs)
     {
      /***** Get number of hits to test from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get number of hits to test",
				"SELECT NumAccTst"	// row[0]
				" FROM crs_usr"
				" WHERE CrsCod=%ld AND UsrCod=%ld",
				Gbl.Hierarchy.Crs.CrsCod,
				Gbl.Usrs.Me.UsrDat.UsrCod);

      if (NumRows == 0)
         NumAccessesTst = 0;
      else if (NumRows == 1)
        {
         /* Get number of hits */
         row = mysql_fetch_row (mysql_res);
         if (row[0] == NULL)
            NumAccessesTst = 0;
         else if (sscanf (row[0],"%u",&NumAccessesTst) != 1)
            NumAccessesTst = 0;
        }
      else
         Lay_ShowErrorAndExit ("Error when getting number of hits to test.");

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return NumAccessesTst;
  }

/*****************************************************************************/
/*************************** Write the test questions ************************/
/*****************************************************************************/
// NumQsts must hold the number of rows of a MySQL query
// In each row mysql_res holds: in the column 0 the code of a question, in the column 1 the type of answer, and in the column 2 the stem

static void Tst_ShowTestQuestionsWhenSeeing (unsigned NumQsts,
                                             MYSQL_RES *mysql_res)
  {
   unsigned NumQst;
   struct Tst_Question Question;
   MYSQL_ROW row;

   if (NumQsts)
     {
      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (10);

      /***** Write one row for each question *****/
      for (NumQst = 0;
	   NumQst < NumQsts;
	   NumQst++)
	{
	 Gbl.RowEvenOdd = NumQst % 2;

	 /* Create test question */
	 Tst_QstConstructor (&Question);

	 /* Get question code (row[0]) */
	 row = mysql_fetch_row (mysql_res);
	 if ((Question.QstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of question.");

	 /* Show question */
	 Tst_ShowOneTestQuestionWhenSeeing (NumQst,&Question);

	 /* Destroy test question */
	 Tst_QstDestructor (&Question);
	}

      /***** End table *****/
      HTM_TABLE_End ();
     }
  }

/*****************************************************************************/
/*************************** Write one test question *************************/
/*****************************************************************************/

static void Tst_ShowOneTestQuestionWhenSeeing (unsigned NumQst,
                                               struct Tst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (Tst_GetOneQuestionByCod (Question->QstCod,&mysql_res))	// Question exists
     {
      /***** Get row of the result of the query *****/
      row = mysql_fetch_row (mysql_res);
      Tst_WriteQstAndAnsTestToAnswer (NumQst,Question,row);
     }
   else
      Lay_ShowErrorAndExit ("Wrong question.");
  }

/*****************************************************************************/
/************************** Show list of test tags ***************************/
/*****************************************************************************/

void Tst_ShowTagList (unsigned NumTags,MYSQL_RES *mysql_res)
  {
   extern const char *Txt_no_tags;
   MYSQL_ROW row;
   unsigned NumTag;

   if (NumTags)
     {
      /***** Write the tags *****/
      HTM_UL_Begin (NULL);
      for (NumTag = 0;
	   NumTag < NumTags;
	   NumTag++)
        {
         row = mysql_fetch_row (mysql_res);
         HTM_LI_Begin (NULL);
         HTM_Txt (row[0]);
         HTM_LI_End ();
        }
      HTM_UL_End ();
     }
   else
      HTM_Txt (Txt_no_tags);
  }

/*****************************************************************************/
/******************* Show the result of assessing a test *********************/
/*****************************************************************************/

static void Tst_ShowTestResultAfterAssess (long TstCod,
                                           struct TsR_Result *Result)
  {
   extern const char *Txt_Question_removed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQst;
   double ScoreThisQst;
   bool AnswerIsNotBlank;

   /***** Initialize score and number of questions not blank *****/
   Result->NumQstsNotBlank = 0;
   Result->Score = 0.0;

   for (NumQst = 0;
	NumQst < Result->NumQsts;
	NumQst++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      /***** Query database *****/
      if (Tst_GetOneQuestionByCod (Result->QstCodes[NumQst],&mysql_res))	// Question exists
	{
	 /***** Write question and answers *****/
	 row = mysql_fetch_row (mysql_res);
	 Tst_WriteQstAndAnsTestResult (&Gbl.Usrs.Me.UsrDat,
				       Result,
				       NumQst,
				       row,
				       TstCfg_GetConfigVisibility (),
				       &ScoreThisQst,&AnswerIsNotBlank);

	 /***** Store test result question in database *****/
	 TsR_StoreOneTestResultQstInDB (TstCod,Result,
				        NumQst,	// 0, 1, 2, 3...
				        ScoreThisQst);

	 /***** Compute total score *****/
	 Result->Score += ScoreThisQst;
	 if (AnswerIsNotBlank)
	    Result->NumQstsNotBlank++;

	 /***** Update the number of accesses and the score of this question *****/
	 if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
	    Tst_UpdateScoreQst (Result->QstCodes[NumQst],ScoreThisQst,AnswerIsNotBlank);
	}
      else
	{
	 /***** Question does not exists *****/
         HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
         Tst_WriteNumQst (NumQst + 1);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"DAT_LIGHT LT COLOR%u\"",Gbl.RowEvenOdd);
	 HTM_Txt (Txt_Question_removed);
	 HTM_TD_End ();

	 HTM_TR_End ();
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/********** Write a row of a test, with one question and its answer **********/
/*****************************************************************************/

static void Tst_WriteQstAndAnsTestToAnswer (unsigned NumQst,
                                            struct Tst_Question *Question,
                                            MYSQL_ROW row)
  {
   /*
   row[0] UNIX_TIMESTAMP(EditTime)
   row[1] AnsType
   row[2] Shuffle
   row[3] Stem
   row[4] Feedback
   row[5] MedCod
   row[6] NumHits
   row[7] NumHitsNotBlank
   row[8] Score
   */

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

   /***** Number of question and answer type (row[1]) *****/
   HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
   Tst_WriteNumQst (NumQst + 1);
   Question->Answer.Type = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);
   Tst_WriteAnswerType (Question->Answer.Type);
   HTM_TD_End ();

   /***** Stem, media and answers *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);

   /* Stem (row[3]) */
   Tst_WriteQstStem (row[3],"TEST_EXA",true);

   /* Media (row[5]) */
   Question->Media.MedCod = Str_ConvertStrCodToLongCod (row[5]);
   Med_GetMediaDataByCod (&Question->Media);
   Med_ShowMedia (&Question->Media,
		  "TEST_MED_SHOW_CONT",
		  "TEST_MED_SHOW");

   /* Answers depending on shuffle (row[2]) */
   Tst_WriteAnswersSeeing (NumQst,Question,(row[2][0] == 'Y'));

   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/********** Write a row of a test, with one question and its answer **********/
/*****************************************************************************/

void Tst_WriteQstAndAnsTestResult (struct UsrData *UsrDat,
				   const struct TsR_Result *Result,
				   unsigned NumQst,
				   MYSQL_ROW row,
				   unsigned Visibility,
				   double *ScoreThisQst,bool *AnswerIsNotBlank)
  {
   struct Tst_Question Question;
   bool IsVisibleQstAndAnsTxt = TsV_IsVisibleQstAndAnsTxt (Visibility);
   /*
   row[0] UNIX_TIMESTAMP(EditTime)
   row[1] AnsType
   row[2] Shuffle
   row[3] Stem
   row[4] Feedback
   row[5] MedCod
   row[6] NumHits
   row[7] NumHitsNotBlank
   row[8] Score
   */

   /***** Create test question *****/
   Tst_QstConstructor (&Question);
   Question.QstCod = Result->QstCodes[NumQst];

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

   /***** Number of question and answer type (row[1]) *****/
   HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
   Tst_WriteNumQst (NumQst + 1);
   Question.Answer.Type = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);
   Tst_WriteAnswerType (Question.Answer.Type);
   HTM_TD_End ();

   /***** Stem, media and answers *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);

   /* Stem (row[3]) */
   Tst_WriteQstStem (row[3],"TEST_EXA",IsVisibleQstAndAnsTxt);

   /* Media (row[5]) */
   if (IsVisibleQstAndAnsTxt)
     {
      Question.Media.MedCod = Str_ConvertStrCodToLongCod (row[5]);
      Med_GetMediaDataByCod (&Question.Media);
      Med_ShowMedia (&Question.Media,
		     "TEST_MED_SHOW_CONT",
		     "TEST_MED_SHOW");
     }

   /* Answers */
   Tst_WriteAnswersResult (UsrDat,Result,
			       NumQst,&Question,
			       Visibility,
			       ScoreThisQst,AnswerIsNotBlank);

   /* Question feedback (row[4]) */
   if (TsV_IsVisibleFeedbackTxt (Visibility))
      Tst_WriteQstFeedback (row[4],"TEST_EXA_LIGHT");

   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();

   /***** Destroy test question *****/
   Tst_QstDestructor (&Question);
  }

/*****************************************************************************/
/********************* Write the number of a test question *******************/
/*****************************************************************************/
// Number of question should be 1, 2, 3...

void Tst_WriteNumQst (unsigned NumQst)
  {
   HTM_DIV_Begin ("class=\"BIG_INDEX\"");
   HTM_Unsigned (NumQst);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************************** Write the type of answer *************************/
/*****************************************************************************/

void Tst_WriteAnswerType (Tst_AnswerType_t AnswerType)
  {
   extern const char *Txt_TST_STR_ANSWER_TYPES[Tst_NUM_ANS_TYPES];

   HTM_DIV_Begin ("class=\"DAT_SMALL\"");
   HTM_Txt (Txt_TST_STR_ANSWER_TYPES[AnswerType]);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Write the stem of a test question *********************/
/*****************************************************************************/

void Tst_WriteQstStem (const char *Stem,const char *ClassStem,bool Visible)
  {
   unsigned long StemLength;
   char *StemRigorousHTML;

   /***** DIV begin *****/
   HTM_DIV_Begin ("class=\"%s\"",ClassStem);

   /***** Write stem *****/
   if (Visible)
     {
      /* Convert the stem, that is in HTML, to rigorous HTML */
      StemLength = strlen (Stem) * Str_MAX_BYTES_PER_CHAR;
      if ((StemRigorousHTML = (char *) malloc (StemLength + 1)) == NULL)
	 Lay_NotEnoughMemoryExit ();
      Str_Copy (StemRigorousHTML,Stem,
		StemLength);

      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			StemRigorousHTML,StemLength,false);

      /* Write stem text */
      HTM_Txt (StemRigorousHTML);

      /* Free memory allocated for the stem */
      free (StemRigorousHTML);
     }
   else
      Ico_PutIconNotVisible ();

   /***** DIV end *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************* Put form to upload a new image for a test question ************/
/*****************************************************************************/

static void Tst_PutFormToEditQstMedia (const struct Media *Media,int NumMediaInForm,
                                       bool OptionsDisabled)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_No_image_video;
   extern const char *Txt_Current_image_video;
   extern const char *Txt_Change_image_video;
   static unsigned UniqueId = 0;
   struct ParamUploadMedia ParamUploadMedia;

   if (Media->Name[0])
     {
      /***** Set names of parameters depending on number of image in form *****/
      Med_SetParamNames (&ParamUploadMedia,NumMediaInForm);

      /***** Start container *****/
      HTM_DIV_Begin ("class=\"TEST_MED_EDIT_FORM\"");

      /***** Choice 1: No media *****/
      HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_INPUT_RADIO (ParamUploadMedia.Action,false,
		       "value=\"%u\"%s",
		       (unsigned) Med_ACTION_NO_MEDIA,
		       OptionsDisabled ? " disabled=\"disabled\"" : "");
      HTM_Txt (Txt_No_image_video);
      HTM_LABEL_End ();
      HTM_BR ();

      /***** Choice 2: Current media *****/
      HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_INPUT_RADIO (ParamUploadMedia.Action,false,
		       "value=\"%u\"%s checked=\"checked\"",
		       (unsigned) Med_ACTION_KEEP_MEDIA,
		       OptionsDisabled ? " disabled=\"disabled\"" : "");
      HTM_Txt (Txt_Current_image_video);
      HTM_LABEL_End ();
      Med_ShowMedia (Media,
	             "TEST_MED_EDIT_ONE_CONT",
		     "TEST_MED_EDIT_ONE");

      /***** Choice 3: Change media *****/
      UniqueId++;
      HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_INPUT_RADIO (ParamUploadMedia.Action,false,
		       "id=\"chg_img_%u\" value=\"%u\"%s",
		       UniqueId,
		       (unsigned) Med_ACTION_NEW_MEDIA,
		       OptionsDisabled ? " disabled=\"disabled\"" : "");
      HTM_TxtColonNBSP (Txt_Change_image_video);
      HTM_LABEL_End ();
      Med_PutMediaUploader (NumMediaInForm,"TEST_MED_INPUT");

      /***** End container *****/
      HTM_DIV_End ();
     }
   else	// No current image
      /***** Attached media *****/
      Med_PutMediaUploader (NumMediaInForm,"TEST_MED_INPUT");
  }

/*****************************************************************************/
/******************* Write the feedback of a test question *******************/
/*****************************************************************************/

void Tst_WriteQstFeedback (const char *Feedback,const char *ClassFeedback)
  {
   unsigned long FeedbackLength;
   char *FeedbackRigorousHTML;

   if (Feedback)
      if (Feedback[0])
	{
	 /***** Convert the feedback, that is in HTML, to rigorous HTML *****/
	 FeedbackLength = strlen (Feedback) * Str_MAX_BYTES_PER_CHAR;
	 if ((FeedbackRigorousHTML = (char *) malloc (FeedbackLength + 1)) == NULL)
	    Lay_NotEnoughMemoryExit ();
	 Str_Copy (FeedbackRigorousHTML,Feedback,
	           FeedbackLength);
	 Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			   FeedbackRigorousHTML,FeedbackLength,false);

	 /***** Write the feedback *****/
	 HTM_DIV_Begin ("class=\"%s\"",ClassFeedback);
	 HTM_Txt (FeedbackRigorousHTML);
	 HTM_DIV_End ();

	 /***** Free memory allocated for the feedback *****/
	 free (FeedbackRigorousHTML);
	}
  }

/*****************************************************************************/
/*********************** Update the score of a question **********************/
/*****************************************************************************/

static void Tst_UpdateScoreQst (long QstCod,double ScoreThisQst,bool AnswerIsNotBlank)
  {
   /***** Update number of clicks and score of the question *****/
   Str_SetDecimalPointToUS ();	// To print the floating point as a dot
   if (AnswerIsNotBlank)
      DB_QueryUPDATE ("can not update the score of a question",
		      "UPDATE tst_questions"
	              " SET NumHits=NumHits+1,NumHitsNotBlank=NumHitsNotBlank+1,"
	              "Score=Score+(%.15lg)"
                      " WHERE QstCod=%ld",
		      ScoreThisQst,QstCod);
   else	// The answer is blank
      DB_QueryUPDATE ("can not update the score of a question",
		      "UPDATE tst_questions"
	              " SET NumHits=NumHits+1"
                      " WHERE QstCod=%ld",
		      QstCod);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/*********** Update my number of accesses to test in this course *************/
/*****************************************************************************/

static void Tst_UpdateMyNumAccessTst (unsigned NumAccessesTst)
  {
   /***** Update my number of accesses to test in this course *****/
   DB_QueryUPDATE ("can not update the number of accesses to test",
		   "UPDATE crs_usr SET NumAccTst=%u"
                   " WHERE CrsCod=%ld AND UsrCod=%ld",
		   NumAccessesTst,
		   Gbl.Hierarchy.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************ Update date-time of my next allowed access to test *************/
/*****************************************************************************/

static void Tst_UpdateLastAccTst (unsigned NumQsts)
  {
   /***** Update date-time and number of questions of this test *****/
   DB_QueryUPDATE ("can not update time and number of questions of this test",
		   "UPDATE crs_usr SET LastAccTst=NOW(),NumQstsLastTst=%u"
                   " WHERE CrsCod=%ld AND UsrCod=%ld",
		   NumQsts,
		   Gbl.Hierarchy.Crs.CrsCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*********************** Request the edition of tests ************************/
/*****************************************************************************/

void Tst_RequestEditTests (void)
  {
   struct Tst_Test Test;

   /***** Create test *****/
   Tst_TstConstructor (&Test);

   /***** Show form to generate a self-assessment test *****/
   Tst_ShowFormRequestEditTests (&Test);

   /***** Destroy test *****/
   Tst_TstDestructor (&Test);
  }

/*****************************************************************************/
/******* Select tags and dates for edition of the self-assessment test *******/
/*****************************************************************************/

static void Tst_ShowFormRequestEditTests (struct Tst_Test *Test)
  {
   extern const char *Hlp_ASSESSMENT_Tests_editing_questions;
   extern const char *Txt_No_test_questions;
   extern const char *Txt_List_edit_questions;
   extern const char *Txt_Show_questions;
   MYSQL_RES *mysql_res;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      Dat_HMS_DO_NOT_SET,
      Dat_HMS_DO_NOT_SET
     };

   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
   TsI_PutFormToImportQuestions ();	// Import questions from XML file
   Mnu_ContextMenuEnd ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_List_edit_questions,
                 Tst_PutIconsTests,Test,
                 Hlp_ASSESSMENT_Tests_editing_questions,Box_NOT_CLOSABLE);

   /***** Get tags already present in the table of questions *****/
   if ((Test->Tags.Num = Tst_GetAllTagsFromCurrentCrs (&mysql_res)))
     {
      Frm_StartForm (ActLstTstQst);
      Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Tst_DEFAULT_ORDER);

      HTM_TABLE_BeginPadding (2);

      /***** Selection of tags *****/
      Tst_ShowFormSelTags (&Test->Tags,mysql_res,false);

      /***** Selection of types of answers *****/
      Tst_ShowFormAnswerTypes (&Test->AnswerTypes);

      /***** Starting and ending dates in the search *****/
      Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (SetHMS);

      HTM_TABLE_End ();

      /***** Send button *****/
      Btn_PutConfirmButton (Txt_Show_questions);
      Frm_EndForm ();
     }
   else	// No test questions
     {
      /***** Warning message *****/
      Ale_ShowAlert (Ale_INFO,Txt_No_test_questions);

      /***** Button to create a new question *****/
      Tst_PutButtonToAddQuestion ();
     }

   /***** End box *****/
   Box_BoxEnd ();

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Select test questions for a game ************************/
/*****************************************************************************/

void Tst_RequestSelectTestsForGame (void)
  {
   struct Tst_Test Test;

   /***** Create test *****/
   Tst_TstConstructor (&Test);

   /***** Show form to select test for game *****/
   Tst_ShowFormRequestSelectTestsForGame (&Test);	// No tags selected

   /***** Destroy test *****/
   Tst_TstDestructor (&Test);
  }

/*****************************************************************************/
/************** Show form to select test questions for a game ****************/
/*****************************************************************************/

static void Tst_ShowFormRequestSelectTestsForGame (struct Tst_Test *Test)
  {
   extern const char *Hlp_ASSESSMENT_Games_questions;
   extern const char *Txt_No_test_questions;
   extern const char *Txt_Select_questions;
   extern const char *Txt_Show_questions;
   MYSQL_RES *mysql_res;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      Dat_HMS_DO_NOT_SET,
      Dat_HMS_DO_NOT_SET
     };

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Select_questions,
                 NULL,NULL,
                 Hlp_ASSESSMENT_Games_questions,Box_NOT_CLOSABLE);

   /***** Get tags already present in the table of questions *****/
   if ((Test->Tags.Num = Tst_GetAllTagsFromCurrentCrs (&mysql_res)))
     {
      Frm_StartForm (ActGamLstTstQst);
      Gam_PutParams (&Gbl);

      HTM_TABLE_BeginPadding (2);

      /***** Selection of tags *****/
      Tst_ShowFormSelTags (&Test->Tags,mysql_res,false);

      /***** Starting and ending dates in the search *****/
      Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (SetHMS);

      HTM_TABLE_End ();

      /***** Send button *****/
      Btn_PutConfirmButton (Txt_Show_questions);
      Frm_EndForm ();
     }
   else	// No test questions
     {
      /***** Warning message *****/
      Ale_ShowAlert (Ale_INFO,Txt_No_test_questions);

      /***** Button to create a new question *****/
      Tst_PutButtonToAddQuestion ();
     }

   /***** End box *****/
   Box_BoxEnd ();

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************* Check if I can edit tests *************************/
/*****************************************************************************/

static bool Tst_CheckIfICanEditTests (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
                  Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);
  }

/*****************************************************************************/
/********************* Put contextual icons in tests *************************/
/*****************************************************************************/

static void Tst_PutIconsTests (void *TestPtr)
  {
   extern const char *Txt_New_question;

   if (TestPtr)
     {
      if (Tst_CheckIfICanEditTests ())
	{
	 switch (Gbl.Action.Act)
	   {
	    case ActLstTstQst:		// List selected test questions for edition
	    case ActReqRemSevTstQst:	// Request removal of selected questions
	    case ActReqRemOneTstQst:	// Request removal of a question
	    case ActRemOneTstQst:		// Remove a question
	    case ActChgShfTstQst:		// Change shuffle of a question
	       /***** Put form to remove selected test questions *****/
	       Ico_PutContextualIconToRemove (ActReqRemSevTstQst,
					      Tst_PutParamsRemoveSelectedQsts,TestPtr);
	       break;
	    default:
	       break;
	   }

	 if (Gbl.Action.Act != ActEdiTstQst)
	    /***** Put form to edit existing test questions *****/
	    Ico_PutContextualIconToEdit (ActEdiTstQst,NULL,
					 NULL,NULL);

	 if (Gbl.Action.Act != ActEdiOneTstQst)
	    /***** Put form to create a new test question *****/
	    Ico_PutContextualIconToAdd (ActEdiOneTstQst,NULL,
					NULL,NULL,
					Txt_New_question);

	 /***** Put form to go to test configuration *****/
	 Ico_PutContextualIconToConfigure (ActCfgTst,
					   NULL,NULL);
	}

      /***** Put icon to view tests results *****/
      switch (Gbl.Usrs.Me.Role.Logged)
	{
	 case Rol_STD:
	    Ico_PutContextualIconToShowResults (ActReqSeeMyTstRes,NULL,
						NULL,NULL);
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	 case Rol_SYS_ADM:
	    Ico_PutContextualIconToShowResults (ActReqSeeUsrTstRes,NULL,
						NULL,NULL);
	    break;
	 default:
	    break;
	}

      /***** Put icon to show a figure *****/
      Gbl.Figures.FigureType = Fig_TESTS;
      Fig_PutIconToShowFigure ();
     }
  }

/*****************************************************************************/
/**************** Put button to create a new test question *******************/
/*****************************************************************************/

static void Tst_PutButtonToAddQuestion (void)
  {
   extern const char *Txt_New_question;

   Frm_StartForm (ActEdiOneTstQst);
   Btn_PutConfirmButton (Txt_New_question);
   Frm_EndForm ();
  }

/*****************************************************************************/
/***************************** Form to rename tags ***************************/
/*****************************************************************************/

void Tst_ShowFormConfig (void)
  {
   extern const char *Txt_Please_specify_if_you_allow_access_to_test_questions_from_mobile_applications;

   /***** If current course has tests and pluggable is unknown... *****/
   if (Tst_CheckIfCourseHaveTestsAndPluggableIsUnknown ())
      Ale_ShowAlert (Ale_WARNING,Txt_Please_specify_if_you_allow_access_to_test_questions_from_mobile_applications);

   /***** Form to configure test *****/
   Tst_ShowFormConfigTst ();

   /***** Form to edit tags *****/
   Tst_ShowFormEditTags ();
  }

/*****************************************************************************/
/******************************* Enable a test tag ***************************/
/*****************************************************************************/

void Tst_EnableTag (void)
  {
   long TagCod = Tst_GetParamTagCode ();

   /***** Change tag status to enabled *****/
   Tst_EnableOrDisableTag (TagCod,false);

   /***** Show again the form to configure test *****/
   Tst_ShowFormConfig ();
  }

/*****************************************************************************/
/****************************** Disable a test tag ***************************/
/*****************************************************************************/

void Tst_DisableTag (void)
  {
   long TagCod = Tst_GetParamTagCode ();

   /***** Change tag status to disabled *****/
   Tst_EnableOrDisableTag (TagCod,true);

   /***** Show again the form to configure test *****/
   Tst_ShowFormConfig ();
  }

/*****************************************************************************/
/************************* Get parameter with tag code ***********************/
/*****************************************************************************/

static long Tst_GetParamTagCode (void)
  {
   long TagCod;

   /***** Get tag code *****/
   if ((TagCod = Par_GetParToLong ("TagCod")) <= 0)
      Lay_ShowErrorAndExit ("Code of tag is missing.");

   return TagCod;
  }

/*****************************************************************************/
/************************ Rename a tag of test questions *********************/
/*****************************************************************************/

void Tst_RenameTag (void)
  {
   extern const char *Txt_The_tag_X_has_been_renamed_as_Y;
   extern const char *Txt_The_tag_X_has_not_changed;
   char OldTagTxt[Tst_MAX_BYTES_TAG + 1];
   char NewTagTxt[Tst_MAX_BYTES_TAG + 1];
   long ExistingTagCod;
   long OldTagCod;
   bool ComplexRenaming;

   /***** Get old and new tags from the form *****/
   Par_GetParToText ("OldTagTxt",OldTagTxt,Tst_MAX_BYTES_TAG);
   Par_GetParToText ("NewTagTxt",NewTagTxt,Tst_MAX_BYTES_TAG);

   /***** Check that the new tag is not empty *****/
   if (NewTagTxt[0])	// New tag not empty
     {
      /***** Check if the old tag is equal to the new one *****/
      if (!strcmp (OldTagTxt,NewTagTxt))	// The old and the new tag
						// are exactly the same (case sensitively).
						// This happens when user press INTRO
						// without changing anything in the form.
         Ale_ShowAlert (Ale_INFO,Txt_The_tag_X_has_not_changed,
                        NewTagTxt);
      else					// The old and the new tag
						// are not exactly the same (case sensitively).
	{
	 /***** Check if renaming is complex or easy *****/
	 ComplexRenaming = false;
	 if (strcasecmp (OldTagTxt,NewTagTxt))	// The old and the new tag
						// are not the same (case insensitively)
	    /* Check if the new tag text is equal to any of the tags
	       already present in the database */
	    if ((ExistingTagCod = Tst_GetTagCodFromTagTxt (NewTagTxt)) > 0)
	       // The new tag was already in database
	       ComplexRenaming = true;

	 if (ComplexRenaming)	// Renaming is not easy
	   {
	    /***** Complex update made to not repeat tags:
		   - If the new tag existed for a question ==>
		     delete old tag from tst_question_tags;
		     the new tag will remain
		   - If the new tag did not exist for a question ==>
		     change old tag to new tag in tst_question_tags *****/
	    /* Get tag code of the old tag */
	    if ((OldTagCod =  Tst_GetTagCodFromTagTxt (OldTagTxt)) < 0)
	       Lay_ShowErrorAndExit ("Tag does not exists.");

	    /* Create a temporary table with all the question codes
	       that had the new tag as one of their tags */
	    DB_Query ("can not remove temporary table",
		      "DROP TEMPORARY TABLE IF EXISTS tst_question_tags_tmp");

	    DB_Query ("can not create temporary table",
		      "CREATE TEMPORARY TABLE tst_question_tags_tmp"
		      " ENGINE=MEMORY"
		      " SELECT QstCod FROM tst_question_tags"
	   	      " WHERE TagCod=%ld",
		      ExistingTagCod);

	    /* Remove old tag in questions where it would be repeated */
	    // New tag existed for a question ==> delete old tag
	    DB_QueryDELETE ("can not remove a tag from some questions",
			    "DELETE FROM tst_question_tags"
			    " WHERE TagCod=%ld"
			    " AND QstCod IN"
			    " (SELECT QstCod FROM tst_question_tags_tmp)",
			    OldTagCod);

	    /* Change old tag to new tag in questions where it would not be repeated */
	    // New tag did not exist for a question ==> change old tag to new tag
	    DB_QueryUPDATE ("can not update a tag in some questions",
			    "UPDATE tst_question_tags"
			    " SET TagCod=%ld"
			    " WHERE TagCod=%ld"
			    " AND QstCod NOT IN"
			    " (SELECT QstCod FROM tst_question_tags_tmp)",
			    ExistingTagCod,
			    OldTagCod);

	    /* Drop temporary table, no longer necessary */
	    DB_Query ("can not remove temporary table",
		      "DROP TEMPORARY TABLE IF EXISTS tst_question_tags_tmp");

	    /***** Delete old tag from tst_tags
		   because it is not longer used *****/
	    DB_QueryDELETE ("can not remove old tag",
			    "DELETE FROM tst_tags WHERE TagCod=%ld",
			    OldTagCod);
	   }
	 else			// Renaming is easy
	   {
	    /***** Simple update replacing each instance of the old tag by the new tag *****/
	    DB_QueryUPDATE ("can not update tag",
			    "UPDATE tst_tags SET TagTxt='%s',ChangeTime=NOW()"
			    " WHERE tst_tags.CrsCod=%ld"
			    " AND tst_tags.TagTxt='%s'",
			    NewTagTxt,Gbl.Hierarchy.Crs.CrsCod,OldTagTxt);
	   }

	 /***** Write message to show the change made *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_The_tag_X_has_been_renamed_as_Y,
		        OldTagTxt,NewTagTxt);
	}
     }
   else			// New tag empty
      Ale_ShowAlertYouCanNotLeaveFieldEmpty ();

   /***** Show again the form to configure test *****/
   Tst_ShowFormConfig ();
  }

/*****************************************************************************/
/*************** Get configuration of test for current course ****************/
/*****************************************************************************/
// Returns true if course has test tags and pluggable is unknown
// Return false if course has no test tags or pluggable is known

bool Tst_CheckIfCourseHaveTestsAndPluggableIsUnknown (void)
  {
   extern const char *TstCfg_PluggableDB[TstCfg_NUM_OPTIONS_PLUGGABLE];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   TstCfg_Pluggable_t Pluggable;

   /***** Get pluggability of tests for current course from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get configuration of test",
			     "SELECT Pluggable"		// row[0]
			     " FROM tst_config"
			     " WHERE CrsCod=%ld",
			     Gbl.Hierarchy.Crs.CrsCod);

   if (NumRows == 0)
      TstCfg_SetConfigPluggable (TstCfg_PLUGGABLE_UNKNOWN);
   else // NumRows == 1
     {
      /***** Get whether test are visible via plugins or not *****/
      row = mysql_fetch_row (mysql_res);

      TstCfg_SetConfigPluggable (TstCfg_PLUGGABLE_UNKNOWN);
      for (Pluggable = TstCfg_PLUGGABLE_NO;
	   Pluggable <= TstCfg_PLUGGABLE_YES;
	   Pluggable++)
         if (!strcmp (row[0],TstCfg_PluggableDB[Pluggable]))
           {
            TstCfg_SetConfigPluggable (Pluggable);
            break;
           }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Get if current course has tests from database *****/
   if (TstCfg_GetConfigPluggable () == TstCfg_PLUGGABLE_UNKNOWN)
      return Tst_CheckIfCurrentCrsHasTestTags ();	// Return true if course has tests

   return false;	// Pluggable is not unknown
  }

/*****************************************************************************/
/******************* Check if current course has test tags *******************/
/*****************************************************************************/
// Return the number of rows of the result

static bool Tst_CheckIfCurrentCrsHasTestTags (void)
  {
   /***** Get available tags from database *****/
   return (DB_QueryCOUNT ("can not check if course has tags",
			  "SELECT COUNT(*) FROM tst_tags"
			  " WHERE CrsCod=%ld",
			  Gbl.Hierarchy.Crs.CrsCod) != 0);
  }

/*****************************************************************************/
/********* Get all (enabled or disabled) test tags for this course ***********/
/*****************************************************************************/
// Return the number of rows of the result

static unsigned Tst_GetAllTagsFromCurrentCrs (MYSQL_RES **mysql_res)
  {
   /***** Get available tags from database *****/
   return (unsigned) DB_QuerySELECT (mysql_res,"can not get available tags",
				     "SELECT TagCod,"	// row[0]
					    "TagTxt,"	// row[1]
					    "TagHidden"	// row[2]
				     " FROM tst_tags"
				     " WHERE CrsCod=%ld"
				     " ORDER BY TagTxt",
				     Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/********************** Get enabled test tags for this course ****************/
/*****************************************************************************/
// Return the number of rows of the result

static unsigned Tst_GetEnabledTagsFromThisCrs (MYSQL_RES **mysql_res)
  {
   /***** Get available not hidden tags from database *****/
   return (unsigned) DB_QuerySELECT (mysql_res,"can not get available enabled tags",
				     "SELECT TagCod,"	// row[0]
					    "TagTxt"	// row[1]
				     " FROM tst_tags"
				     " WHERE CrsCod=%ld AND TagHidden='N'"
				     " ORDER BY TagTxt",
				     Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/********************* Show a form to select test tags ***********************/
/*****************************************************************************/

static void Tst_ShowFormSelTags (const struct Tst_Tags *Tags,
                                 MYSQL_RES *mysql_res,
                                 bool ShowOnlyEnabledTags)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Tags;
   extern const char *Txt_All_tags;
   extern const char *Txt_Tag_not_allowed;
   extern const char *Txt_Tag_allowed;
   unsigned NumTag;
   MYSQL_ROW row;
   bool TagHidden = false;
   bool Checked;
   const char *Ptr;
   char TagText[Tst_MAX_BYTES_TAG + 1];
   /*
   row[0] TagCod
   row[1] TagTxt
   row[2] TagHidden
   */
   HTM_TR_Begin (NULL);

   /***** Label *****/
   HTM_TD_Begin ("class=\"RT %s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Tags);
   HTM_TD_End ();

   /***** Select all tags *****/
   HTM_TD_Begin ("class=\"LT\"");

   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   if (!ShowOnlyEnabledTags)
      HTM_TD_Empty (1);

   HTM_TD_Begin ("class=\"LM\"");
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_INPUT_CHECKBOX ("AllTags",HTM_DONT_SUBMIT_ON_CHANGE,
		       "value=\"Y\"%s onclick=\"togglecheckChildren(this,'ChkTag');\"",
		       Tags->All ? " checked=\"checked\"" :
			           "");
   HTM_TxtF ("&nbsp;%s",Txt_All_tags);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Select tags one by one *****/
   for (NumTag = 1;
	NumTag <= Tags->Num;
	NumTag++)
     {
      row = mysql_fetch_row (mysql_res);
      HTM_TR_Begin (NULL);

      if (!ShowOnlyEnabledTags)
        {
         TagHidden = (row[2][0] == 'Y');
         HTM_TD_Begin ("class=\"LM\"");
         Ico_PutIconOff (TagHidden ? "eye-slash-red.svg" :
                                     "eye-green.svg",
			 TagHidden ? Txt_Tag_not_allowed :
			             Txt_Tag_allowed);
         HTM_TD_End ();
        }

      Checked = false;
      if (Tags->List)
        {
         Ptr = Tags->List;
         while (*Ptr)
           {
            Par_GetNextStrUntilSeparParamMult (&Ptr,TagText,Tst_MAX_BYTES_TAG);
            if (!strcmp (row[1],TagText))
              {
               Checked = true;
               break;
              }
           }
        }

      HTM_TD_Begin ("class=\"LM\"");
      HTM_LABEL_Begin ("class=\"DAT\"");
      HTM_INPUT_CHECKBOX ("ChkTag",HTM_DONT_SUBMIT_ON_CHANGE,
			  "value=\"%s\"%s onclick=\"checkParent(this,'AllTags');\"",
			  row[1],
			  Checked ? " checked=\"checked\"" :
				    "");
      HTM_TxtF ("&nbsp;%s",row[1]);
      HTM_LABEL_End ();
      HTM_TD_End ();

      HTM_TR_End ();
     }

   HTM_TABLE_End ();
   HTM_TD_End ();
   HTM_TR_End ();
  }

/*****************************************************************************/
/************* Show a form to enable/disable and rename test tags ************/
/*****************************************************************************/

static void Tst_ShowFormEditTags (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_writing_a_question;
   extern const char *Txt_No_test_questions;
   extern const char *Txt_Tags;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumTags;
   unsigned NumTag;
   long TagCod;

   /***** Get current tags in current course *****/
   if ((NumTags = Tst_GetAllTagsFromCurrentCrs (&mysql_res)))
     {
      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_Tags,
                         NULL,NULL,
                         Hlp_ASSESSMENT_Tests_writing_a_question,Box_NOT_CLOSABLE,2);

      /***** Show tags *****/
      for (NumTag = 0;
	   NumTag < NumTags;
	   NumTag++)
        {
         row = mysql_fetch_row (mysql_res);
	 /*
	 row[0] TagCod
	 row[1] TagTxt
	 row[2] TagHidden
	 */
         if ((TagCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of tag.");

         HTM_TR_Begin (NULL);

         /* Form to enable / disable this tag */
         if (row[2][0] == 'Y')	// Tag disabled
            Tst_PutIconEnable (TagCod,row[1]);
         else
            Tst_PutIconDisable (TagCod,row[1]);

         /* Form to rename this tag */
         HTM_TD_Begin ("class=\"LM\"");
         Frm_StartForm (ActRenTag);
         Par_PutHiddenParamString (NULL,"OldTagTxt",row[1]);
	 HTM_INPUT_TEXT ("NewTagTxt",Tst_MAX_CHARS_TAG,row[1],true,
			 "size=\"36\" required=\"required\"");
         Frm_EndForm ();
         HTM_TD_End ();

         HTM_TR_End ();
        }

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }
   else
      Ale_ShowAlert (Ale_INFO,Txt_No_test_questions);

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Put a link and an icon to enable a tag ******************/
/*****************************************************************************/

static void Tst_PutIconEnable (long TagCod,const char *TagTxt)
  {
   extern const char *Txt_Tag_X_not_allowed_Click_to_allow_it;

   HTM_TD_Begin ("class=\"BM\"");
   Frm_StartForm (ActEnableTag);
   Par_PutHiddenParamLong (NULL,"TagCod",TagCod);
   Ico_PutIconLink ("eye-slash-red.svg",
		    Str_BuildStringStr (Txt_Tag_X_not_allowed_Click_to_allow_it,
				        TagTxt));
   Str_FreeString ();
   Frm_EndForm ();
   HTM_TD_End ();
  }

/*****************************************************************************/
/****************** Put a link and an icon to disable a tag ******************/
/*****************************************************************************/

static void Tst_PutIconDisable (long TagCod,const char *TagTxt)
  {
   extern const char *Txt_Tag_X_allowed_Click_to_disable_it;

   HTM_TD_Begin ("class=\"BM\"");
   Frm_StartForm (ActDisableTag);
   Par_PutHiddenParamLong (NULL,"TagCod",TagCod);
   Ico_PutIconLink ("eye-green.svg",
		    Str_BuildStringStr (Txt_Tag_X_allowed_Click_to_disable_it,
				        TagTxt));
   Str_FreeString ();
   Frm_EndForm ();
   HTM_TD_End ();
  }

/*****************************************************************************/
/********************* Show a form to to configure test **********************/
/*****************************************************************************/

static void Tst_ShowFormConfigTst (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Configure_tests;
   extern const char *Txt_Plugins;
   extern const char *Txt_TST_PLUGGABLE[TstCfg_NUM_OPTIONS_PLUGGABLE];
   extern const char *Txt_No_of_questions;
   extern const char *Txt_minimum;
   extern const char *Txt_default;
   extern const char *Txt_maximum;
   extern const char *Txt_Minimum_time_seconds_per_question_between_two_tests;
   extern const char *Txt_Result_visibility;
   extern const char *Txt_Save_changes;
   struct Tst_Test Test;
   TstCfg_Pluggable_t Pluggable;
   char StrMinTimeNxtTstPerQst[Cns_MAX_DECIMAL_DIGITS_ULONG + 1];

   /***** Create test *****/
   Tst_TstConstructor (&Test);

   /***** Read test configuration from database *****/
   TstCfg_GetConfigFromDB ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Configure_tests,
                 Tst_PutIconsTests,&Test,
                 Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);

   /***** Begin form *****/
   Frm_StartForm (ActRcvCfgTst);

   /***** Tests are visible from plugins? *****/
   HTM_TABLE_BeginCenterPadding (2);
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Plugins);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LB\"");
   for (Pluggable = TstCfg_PLUGGABLE_NO;
	Pluggable <= TstCfg_PLUGGABLE_YES;
	Pluggable++)
     {
      HTM_LABEL_Begin ("class=\"DAT\"");
      HTM_INPUT_RADIO ("Pluggable",false,
		       "value=\"%u\"%s",
		       (unsigned) Pluggable,
		       Pluggable == TstCfg_GetConfigPluggable () ? " checked=\"checked\"" :
				                                "");
      HTM_Txt (Txt_TST_PLUGGABLE[Pluggable]);
      HTM_LABEL_End ();
      HTM_BR ();
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Number of questions *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_No_of_questions);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LB\"");
   HTM_TABLE_BeginPadding (2);
   Tst_PutInputFieldNumQst ("NumQstMin",Txt_minimum,
                            TstCfg_GetConfigMin ());	// Minimum number of questions
   Tst_PutInputFieldNumQst ("NumQstDef",Txt_default,
                            TstCfg_GetConfigDef ());	// Default number of questions
   Tst_PutInputFieldNumQst ("NumQstMax",Txt_maximum,
                            TstCfg_GetConfigMax ());	// Maximum number of questions
   HTM_TABLE_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Minimum time between consecutive tests, per question *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","MinTimeNxtTstPerQst",
		    Txt_Minimum_time_seconds_per_question_between_two_tests);

   /* Data */
   HTM_TD_Begin ("class=\"LB\"");
   snprintf (StrMinTimeNxtTstPerQst,sizeof (StrMinTimeNxtTstPerQst),
             "%lu",
	     TstCfg_GetConfigMinTimeNxtTstPerQst ());
   HTM_INPUT_TEXT ("MinTimeNxtTstPerQst",Cns_MAX_DECIMAL_DIGITS_ULONG,StrMinTimeNxtTstPerQst,false,
		   "id=\"MinTimeNxtTstPerQst\" size=\"7\" required=\"required\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Visibility of results *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Result_visibility);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LB\"");
   TsV_PutVisibilityCheckboxes (TstCfg_GetConfigVisibility ());
   HTM_TD_End ();

   HTM_TR_End ();

   HTM_TABLE_End ();

   /***** Send button *****/
   Btn_PutConfirmButton (Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Destroy test *****/
   Tst_TstDestructor (&Test);
  }

/*****************************************************************************/
/*************** Get configuration of test for current course ****************/
/*****************************************************************************/

static void Tst_PutInputFieldNumQst (const char *Field,const char *Label,
                                     unsigned Value)
  {
   char StrValue[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   HTM_LABEL_Begin ("for=\"%s\" class=\"DAT\"",Field);
   HTM_Txt (Label);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LM\"");
   snprintf (StrValue,sizeof (StrValue),
	     "%u",
	     Value);
   HTM_INPUT_TEXT (Field,Cns_MAX_DECIMAL_DIGITS_UINT,StrValue,false,
		   "id=\"%s\" size=\"3\" required=\"required\"",Field);
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Show form for select the types of answers *****************/
/*****************************************************************************/

static void Tst_ShowFormAnswerTypes (const struct Tst_AnswerTypes *AnswerTypes)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Types_of_answers;
   extern const char *Txt_All_types_of_answers;
   extern const char *Txt_TST_STR_ANSWER_TYPES[Tst_NUM_ANS_TYPES];
   Tst_AnswerType_t AnsType;
   bool Checked;
   char UnsignedStr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   const char *Ptr;

   HTM_TR_Begin (NULL);

   /***** Label *****/
   HTM_TD_Begin ("class=\"RT %s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Types_of_answers);
   HTM_TD_End ();

   /***** Select all types of answers *****/
   HTM_TD_Begin ("class=\"LT\"");
   HTM_TABLE_BeginPadding (2);

   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"LM\"");
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_INPUT_CHECKBOX ("AllAnsTypes",HTM_DONT_SUBMIT_ON_CHANGE,
		       "value=\"Y\"%s onclick=\"togglecheckChildren(this,'AnswerType');\"",
		       AnswerTypes->All ? " checked=\"checked\"" :
			                  "");
   HTM_TxtF ("&nbsp;%s",Txt_All_types_of_answers);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Type of answer *****/
   for (AnsType  = (Tst_AnswerType_t) 0;
	AnsType <= (Tst_AnswerType_t) (Tst_NUM_ANS_TYPES - 1);
	AnsType++)
     {
      HTM_TR_Begin (NULL);

      Checked = false;
      Ptr = AnswerTypes->List;
      while (*Ptr)
        {
         Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,Cns_MAX_DECIMAL_DIGITS_UINT);
         if (Tst_ConvertFromUnsignedStrToAnsTyp (UnsignedStr) == AnsType)
           {
            Checked = true;
            break;
           }
        }
      HTM_TD_Begin ("class=\"LM\"");
      HTM_LABEL_Begin ("class=\"DAT\"");
      HTM_INPUT_CHECKBOX ("AnswerType",HTM_DONT_SUBMIT_ON_CHANGE,
			  "value=\"%u\"%s onclick=\"checkParent(this,'AllAnsTypes');\"",
			  (unsigned) AnsType,
			  Checked ? " checked=\"checked\"" :
				    "");
      HTM_TxtF ("&nbsp;%s",Txt_TST_STR_ANSWER_TYPES[AnsType]);
      HTM_LABEL_End ();
      HTM_TD_End ();

      HTM_TR_End ();
     }

   HTM_TABLE_End ();
   HTM_TD_End ();
   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** List several test questions for edition *******************/
/*****************************************************************************/

void Tst_ListQuestionsToEdit (void)
  {
   struct Tst_Test Test;
   MYSQL_RES *mysql_res;

   /***** Create test *****/
   Tst_TstConstructor (&Test);

   /***** Get parameters, query the database and list the questions *****/
   if (Tst_GetParamsTst (&Test,Tst_EDIT_TEST))	// Get parameters from the form
     {
      /***** Get question codes from database *****/
      Tst_GetQuestions (&Test,&mysql_res);	// Query database
      if (Test.NumQsts)
        {
	 /* Contextual menu */
         Mnu_ContextMenuBegin ();

	 if (TsI_GetCreateXMLParamFromForm ())
            TsI_CreateXML (Test.NumQsts,mysql_res);	// Create XML file with exported questions...
							// ...and put a link to download it
         else
            TsI_PutFormToExportQuestions (&Test);	// Export questions
	 Mnu_ContextMenuEnd ();

	 /* Show the table with the questions */
         Tst_ListOneOrMoreQuestionsForEdition (&Test,mysql_res);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /* Show the form again */
      Tst_ShowFormRequestEditTests (&Test);

   /***** Destroy test *****/
   Tst_TstDestructor (&Test);
  }

/*****************************************************************************/
/**************** List several test questions for selection ******************/
/*****************************************************************************/

void Tst_ListQuestionsToSelect (void)
  {
   struct Tst_Test Test;
   MYSQL_RES *mysql_res;

   /***** Create test *****/
   Tst_TstConstructor (&Test);

   /***** Get parameters, query the database and list the questions *****/
   if (Tst_GetParamsTst (&Test,Tst_SELECT_QUESTIONS_FOR_GAME))	// Get parameters from the form
     {
      Tst_GetQuestions (&Test,&mysql_res);	// Query database
      if (Test.NumQsts)
	 /* Show the table with the questions */
         Tst_ListOneOrMoreQuestionsForSelection (Test.NumQsts,mysql_res);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /* Show the form again */
      Tst_ShowFormRequestSelectTestsForGame (&Test);

   /***** Destroy test *****/
   Tst_TstDestructor (&Test);
  }

/*****************************************************************************/
/********** Get from the database several test questions for listing *********/
/*****************************************************************************/

#define Tst_MAX_BYTES_QUERY_TEST (16 * 1024 - 1)

static void Tst_GetQuestions (struct Tst_Test *Test,MYSQL_RES **mysql_res)
  {
   extern const char *Txt_No_questions_found_matching_your_search_criteria;
   char *Query = NULL;
   long LengthQuery;
   unsigned NumItemInList;
   const char *Ptr;
   char TagText[Tst_MAX_BYTES_TAG + 1];
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   char UnsignedStr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   Tst_AnswerType_t AnsType;
   char CrsCodStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Allocate space for query *****/
   if ((Query = (char *) malloc (Tst_MAX_BYTES_QUERY_TEST + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   /***** Select questions *****/
   /* Start query */
   snprintf (Query,Tst_MAX_BYTES_QUERY_TEST + 1,
	     "SELECT tst_questions.QstCod"	// row[0]
	     " FROM tst_questions");
   if (!Test->Tags.All)
      Str_Concat (Query,",tst_question_tags,tst_tags",
                  Tst_MAX_BYTES_QUERY_TEST);

   Str_Concat (Query," WHERE tst_questions.CrsCod='",
               Tst_MAX_BYTES_QUERY_TEST);
   snprintf (CrsCodStr,sizeof (CrsCodStr),
	     "%ld",
	     Gbl.Hierarchy.Crs.CrsCod);
   Str_Concat (Query,CrsCodStr,
               Tst_MAX_BYTES_QUERY_TEST);
   Str_Concat (Query,"' AND tst_questions.EditTime>=FROM_UNIXTIME('",
               Tst_MAX_BYTES_QUERY_TEST);
   snprintf (LongStr,sizeof (LongStr),
	     "%ld",
	     (long) Gbl.DateRange.TimeUTC[Dat_START_TIME]);
   Str_Concat (Query,LongStr,
               Tst_MAX_BYTES_QUERY_TEST);
   Str_Concat (Query,"') AND tst_questions.EditTime<=FROM_UNIXTIME('",
               Tst_MAX_BYTES_QUERY_TEST);
   snprintf (LongStr,sizeof (LongStr),
	     "%ld",
	     (long) Gbl.DateRange.TimeUTC[Dat_END_TIME]);
   Str_Concat (Query,LongStr,
               Tst_MAX_BYTES_QUERY_TEST);
   Str_Concat (Query,"')",
               Tst_MAX_BYTES_QUERY_TEST);

   /* Add the tags selected */
   if (!Test->Tags.All)
     {
      Str_Concat (Query," AND tst_questions.QstCod=tst_question_tags.QstCod"
	                " AND tst_question_tags.TagCod=tst_tags.TagCod"
                        " AND tst_tags.CrsCod='",
                  Tst_MAX_BYTES_QUERY_TEST);
      Str_Concat (Query,CrsCodStr,
                  Tst_MAX_BYTES_QUERY_TEST);
      Str_Concat (Query,"'",
                  Tst_MAX_BYTES_QUERY_TEST);
      LengthQuery = strlen (Query);
      NumItemInList = 0;
      Ptr = Test->Tags.List;
      while (*Ptr)
        {
         Par_GetNextStrUntilSeparParamMult (&Ptr,TagText,Tst_MAX_BYTES_TAG);
         LengthQuery = LengthQuery + 35 + strlen (TagText) + 1;
         if (LengthQuery > Tst_MAX_BYTES_QUERY_TEST - 256)
            Lay_ShowErrorAndExit ("Query size exceed.");
         Str_Concat (Query,
                     NumItemInList ? " OR tst_tags.TagTxt='" :
                                     " AND (tst_tags.TagTxt='",
                     Tst_MAX_BYTES_QUERY_TEST);
         Str_Concat (Query,TagText,
                     Tst_MAX_BYTES_QUERY_TEST);
         Str_Concat (Query,"'",
                     Tst_MAX_BYTES_QUERY_TEST);
         NumItemInList++;
        }
      Str_Concat (Query,")",
                  Tst_MAX_BYTES_QUERY_TEST);
     }

   /* Add the types of answer selected */
   if (!Test->AnswerTypes.All)
     {
      LengthQuery = strlen (Query);
      NumItemInList = 0;
      Ptr = Test->AnswerTypes.List;
      while (*Ptr)
        {
         Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,Tst_MAX_BYTES_TAG);
	 AnsType = Tst_ConvertFromUnsignedStrToAnsTyp (UnsignedStr);
         LengthQuery = LengthQuery + 35 + strlen (Tst_StrAnswerTypesDB[AnsType]) + 1;
         if (LengthQuery > Tst_MAX_BYTES_QUERY_TEST - 256)
            Lay_ShowErrorAndExit ("Query size exceed.");
         Str_Concat (Query,
                     NumItemInList ? " OR tst_questions.AnsType='" :
                                     " AND (tst_questions.AnsType='",
                     Tst_MAX_BYTES_QUERY_TEST);
         Str_Concat (Query,Tst_StrAnswerTypesDB[AnsType],
                     Tst_MAX_BYTES_QUERY_TEST);
         Str_Concat (Query,"'",
                     Tst_MAX_BYTES_QUERY_TEST);
         NumItemInList++;
        }
      Str_Concat (Query,")",
                  Tst_MAX_BYTES_QUERY_TEST);
     }

   /* End the query */
   Str_Concat (Query," GROUP BY tst_questions.QstCod",
               Tst_MAX_BYTES_QUERY_TEST);

   switch (Test->SelectedOrder)
     {
      case Tst_ORDER_STEM:
         Str_Concat (Query," ORDER BY tst_questions.Stem",
                     Tst_MAX_BYTES_QUERY_TEST);
         break;
      case Tst_ORDER_NUM_HITS:
         Str_Concat (Query," ORDER BY tst_questions.NumHits DESC,"
				     "tst_questions.Stem",
                     Tst_MAX_BYTES_QUERY_TEST);
         break;
      case Tst_ORDER_AVERAGE_SCORE:
         Str_Concat (Query," ORDER BY tst_questions.Score/tst_questions.NumHits DESC,"
				     "tst_questions.NumHits DESC,"
				     "tst_questions.Stem",
                     Tst_MAX_BYTES_QUERY_TEST);
         break;
      case Tst_ORDER_NUM_HITS_NOT_BLANK:
         Str_Concat (Query," ORDER BY tst_questions.NumHitsNotBlank DESC,"
				     "tst_questions.Stem",
                     Tst_MAX_BYTES_QUERY_TEST);
         break;
      case Tst_ORDER_AVERAGE_SCORE_NOT_BLANK:
         Str_Concat (Query," ORDER BY tst_questions.Score/tst_questions.NumHitsNotBlank DESC,"
				     "tst_questions.NumHitsNotBlank DESC,"
				     "tst_questions.Stem",
                     Tst_MAX_BYTES_QUERY_TEST);
         break;
     }

   /* Make the query */
   Test->NumQsts = (unsigned) DB_QuerySELECT (mysql_res,"can not get questions",
					      "%s",
					      Query);
   if (Test->NumQsts == 0)
      Ale_ShowAlert (Ale_INFO,Txt_No_questions_found_matching_your_search_criteria);
  }

/*****************************************************************************/
/********* Get from the database several test questions to list them *********/
/*****************************************************************************/

static void Tst_GetQuestionsForTest (struct Tst_Test *Test,MYSQL_RES **mysql_res)
  {
   char *Query = NULL;
   long LengthQuery;
   unsigned NumItemInList;
   const char *Ptr;
   char TagText[Tst_MAX_BYTES_TAG + 1];
   char UnsignedStr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   Tst_AnswerType_t AnsType;
   char StrNumQsts[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Allocate space for query *****/
   if ((Query = (char *) malloc (Tst_MAX_BYTES_QUERY_TEST + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   /***** Select questions without hidden tags *****/
   /* Start query */
   // Reject questions with any tag hidden
   // Select only questions with tags
   // DISTINCTROW is necessary to not repeat questions
   snprintf (Query,Tst_MAX_BYTES_QUERY_TEST + 1,
	     "SELECT DISTINCT tst_questions.QstCod"		// row[0]
	     " FROM tst_questions,tst_question_tags,tst_tags"
	     " WHERE tst_questions.CrsCod=%ld"
	     " AND tst_questions.QstCod NOT IN"
	     " (SELECT tst_question_tags.QstCod"
	     " FROM tst_tags,tst_question_tags"
	     " WHERE tst_tags.CrsCod=%ld AND tst_tags.TagHidden='Y'"
	     " AND tst_tags.TagCod=tst_question_tags.TagCod)"
	     " AND tst_questions.QstCod=tst_question_tags.QstCod"
	     " AND tst_question_tags.TagCod=tst_tags.TagCod"
	     " AND tst_tags.CrsCod=%ld",
	     Gbl.Hierarchy.Crs.CrsCod,
	     Gbl.Hierarchy.Crs.CrsCod,
	     Gbl.Hierarchy.Crs.CrsCod);

   if (!Test->Tags.All) // User has not selected all the tags
     {
      /* Add selected tags */
      LengthQuery = strlen (Query);
      NumItemInList = 0;
      Ptr = Test->Tags.List;
      while (*Ptr)
        {
         Par_GetNextStrUntilSeparParamMult (&Ptr,TagText,Tst_MAX_BYTES_TAG);
         LengthQuery = LengthQuery + 35 + strlen (TagText) + 1;
         if (LengthQuery > Tst_MAX_BYTES_QUERY_TEST - 128)
            Lay_ShowErrorAndExit ("Query size exceed.");
         Str_Concat (Query,
                     NumItemInList ? " OR tst_tags.TagTxt='" :
                                     " AND (tst_tags.TagTxt='",
                     Tst_MAX_BYTES_QUERY_TEST);
         Str_Concat (Query,TagText,
                     Tst_MAX_BYTES_QUERY_TEST);
         Str_Concat (Query,"'",
                     Tst_MAX_BYTES_QUERY_TEST);
         NumItemInList++;
        }
      Str_Concat (Query,")",
                  Tst_MAX_BYTES_QUERY_TEST);
     }

   /* Add answer types selected */
   if (!Test->AnswerTypes.All)
     {
      LengthQuery = strlen (Query);
      NumItemInList = 0;
      Ptr = Test->AnswerTypes.List;
      while (*Ptr)
        {
         Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,Tst_MAX_BYTES_TAG);
	 AnsType = Tst_ConvertFromUnsignedStrToAnsTyp (UnsignedStr);
         LengthQuery = LengthQuery + 35 + strlen (Tst_StrAnswerTypesDB[AnsType]) + 1;
         if (LengthQuery > Tst_MAX_BYTES_QUERY_TEST - 128)
            Lay_ShowErrorAndExit ("Query size exceed.");
         Str_Concat (Query,
                     NumItemInList ? " OR tst_questions.AnsType='" :
                                     " AND (tst_questions.AnsType='",
                     Tst_MAX_BYTES_QUERY_TEST);
         Str_Concat (Query,Tst_StrAnswerTypesDB[AnsType],
                     Tst_MAX_BYTES_QUERY_TEST);
         Str_Concat (Query,"'",
                     Tst_MAX_BYTES_QUERY_TEST);
         NumItemInList++;
        }
      Str_Concat (Query,")",
                  Tst_MAX_BYTES_QUERY_TEST);
     }

   /* End query */
   Str_Concat (Query," ORDER BY RAND(NOW()) LIMIT ",
               Tst_MAX_BYTES_QUERY_TEST);
   snprintf (StrNumQsts,sizeof (StrNumQsts),
	     "%u",
	     Test->NumQsts);
   Str_Concat (Query,StrNumQsts,
               Tst_MAX_BYTES_QUERY_TEST);
/*
   if (Gbl.Usrs.Me.Roles.LoggedRole == Rol_SYS_ADM)
      Lay_ShowAlert (Lay_INFO,Query);
*/
   /* Make the query */
   Test->NumQsts = (unsigned) DB_QuerySELECT (mysql_res,"can not get questions",
			                      "%s",
			                      Query);
  }

/*****************************************************************************/
/*********************** List a test question for edition ********************/
/*****************************************************************************/

static void Tst_ListOneQstToEdit (struct Tst_Test *Test)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *Txt_Questions;

   /***** List only one question *****/
   Test->NumQsts = 1;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Questions,
                 Tst_PutIconsTests,Test,
		 Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);

   /***** Write the heading *****/
   HTM_TABLE_BeginWideMarginPadding (2);
   Tst_WriteHeadingRowQuestionsForEdition (Test);

   /***** Write question row *****/
   Tst_WriteQuestionListing (Test,0);

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Button to add a new question *****/
   Tst_PutButtonToAddQuestion ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*********************** Get data of one test question ***********************/
/*****************************************************************************/
// Return true on success, false on error

bool Tst_GetOneQuestionByCod (long QstCod,MYSQL_RES **mysql_res)
  {
   /***** Get data of a question from database *****/
   return (DB_QuerySELECT (mysql_res,"can not get data of a question",
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
			   " WHERE QstCod=%ld",
			   QstCod) == 1);
  }

/*****************************************************************************/
/****************** List for edition one or more test questions **************/
/*****************************************************************************/

static void Tst_ListOneOrMoreQuestionsForEdition (struct Tst_Test *Test,
                                                  MYSQL_RES *mysql_res)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *Txt_Questions;
   unsigned NumQst;
   MYSQL_ROW row;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Questions,
                 Tst_PutIconsTests,Test,
		 Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);

   /***** Write the heading *****/
   HTM_TABLE_BeginWideMarginPadding (2);
   Tst_WriteHeadingRowQuestionsForEdition (Test);

   /***** Write rows *****/
   for (NumQst = 0;
	NumQst < Test->NumQsts;
	NumQst++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      /***** Create test question *****/
      Tst_QstConstructor (&Test->Question);

      /***** Get question code (row[0]) *****/
      row = mysql_fetch_row (mysql_res);
      if ((Test->Question.QstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
         Lay_ShowErrorAndExit ("Wrong code of question.");

      /***** Write question row *****/
      Tst_WriteQuestionListing (Test,NumQst);

      /***** Destroy test question *****/
      Tst_QstDestructor (&Test->Question);
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Button to add a new question *****/
   Tst_PutButtonToAddQuestion ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*********** Write heading row in listing of questions for edition ***********/
/*****************************************************************************/

static void Tst_WriteHeadingRowQuestionsForEdition (const struct Tst_Test *Test)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Code;
   extern const char *Txt_Date;
   extern const char *Txt_Tags;
   extern const char *Txt_Shuffle;
   extern const char *Txt_TST_STR_ORDER_FULL[Tst_NUM_TYPES_ORDER_QST];
   extern const char *Txt_TST_STR_ORDER_SHORT[Tst_NUM_TYPES_ORDER_QST];
   Tst_QuestionsOrder_t Order;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

   /***** First columns *****/
   HTM_TH_Empty (1);
   HTM_TH (1,1,"CT",Txt_No_INDEX);
   HTM_TH (1,1,"CT",Txt_Code);
   HTM_TH (1,1,"CT",Txt_Date);
   HTM_TH (1,1,"CT",Txt_Tags);
   HTM_TH (1,1,"CT",Txt_Shuffle);

   /***** Columns which data can be ordered *****/
   /* Stem and answers of question */
   /* Number of times that the question has been answered */
   /* Average score */
   for (Order  = (Tst_QuestionsOrder_t) 0;
	Order <= (Tst_QuestionsOrder_t) (Tst_NUM_TYPES_ORDER_QST - 1);
	Order++)
     {
      HTM_TH_Begin (1,1,"LT");

      if (Test->NumQsts > 1)
        {
         Frm_StartForm (ActLstTstQst);
         Dat_WriteParamsIniEndDates ();
         Tst_WriteParamEditQst (Test);
         Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
         HTM_BUTTON_SUBMIT_Begin (Txt_TST_STR_ORDER_FULL[Order],"BT_LINK TIT_TBL",NULL);
         if (Order == Test->SelectedOrder)
            HTM_U_Begin ();
        }
      HTM_Txt (Txt_TST_STR_ORDER_SHORT[Order]);
      if (Test->NumQsts > 1)
        {
         if (Order == Test->SelectedOrder)
            HTM_U_End ();
         HTM_BUTTON_End ();
         Frm_EndForm ();
        }

      HTM_TH_End ();
     }

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/********** Write question row in listing of questions for edition ***********/
/*****************************************************************************/

static void Tst_WriteQuestionListing (struct Tst_Test *Test,unsigned NumQst)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   static unsigned UniqueId = 0;
   char *Id;
   time_t TimeUTC;
   unsigned long NumHitsThisQst;
   unsigned long NumHitsNotBlankThisQst;
   double TotalScoreThisQst;

   /***** Get and show question data *****/
   if (Tst_GetOneQuestionByCod (Test->Question.QstCod,&mysql_res))
     {
      /***** Get row from database *****/
      row = mysql_fetch_row (mysql_res);
      /*
      row[0] UNIX_TIMESTAMP(EditTime)
      row[1] AnsType
      row[2] Shuffle
      row[3] Stem
      row[4] Feedback
      row[5] MedCod
      row[6] NumHits
      row[7] NumHitsNotBlank
      row[8] Score
      */

      /***** Begin table row *****/
      HTM_TR_Begin (NULL);

      /***** Icons *****/
      HTM_TD_Begin ("class=\"BT%u\"",Gbl.RowEvenOdd);

      /* Write icon to remove the question */
      Frm_StartForm (ActReqRemOneTstQst);
      Tst_PutParamQstCod (&Test->Question.QstCod);
      if (Test->NumQsts == 1)
	 Par_PutHiddenParamChar ("OnlyThisQst",'Y'); // If there are only one row, don't list again after removing
      Dat_WriteParamsIniEndDates ();
      Tst_WriteParamEditQst (Test);
      Ico_PutIconRemove ();
      Frm_EndForm ();

      /* Write icon to edit the question */
      Ico_PutContextualIconToEdit (ActEdiOneTstQst,NULL,
                                   Tst_PutParamQstCod,&Test->Question.QstCod);

      HTM_TD_End ();

      /* Number of question and answer type (row[1]) */
      HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
      Tst_WriteNumQst (NumQst + 1);
      Test->Question.Answer.Type = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);
      Tst_WriteAnswerType (Test->Question.Answer.Type);
      HTM_TD_End ();

      /* Question code */
      HTM_TD_Begin ("class=\"DAT_SMALL CT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TxtF ("%ld&nbsp;",Test->Question.QstCod);
      HTM_TD_End ();

      /* Date (row[0] has the UTC date-time) */
      TimeUTC = Dat_GetUNIXTimeFromStr (row[0]);
      if (asprintf (&Id,"tst_date_%u",++UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      HTM_TD_Begin ("id=\"%s\" class=\"DAT_SMALL CT COLOR%u\"",
		    Id,Gbl.RowEvenOdd);
      Dat_WriteLocalDateHMSFromUTC (Id,TimeUTC,
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				    true,true,false,0x7);
      HTM_TD_End ();
      free (Id);

      /* Question tags */
      HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
      Tst_GetAndWriteTagsQst (Test->Question.QstCod);
      HTM_TD_End ();

      /* Shuffle (row[2]) */
      HTM_TD_Begin ("class=\"DAT_SMALL CT COLOR%u\"",Gbl.RowEvenOdd);
      if (Test->Question.Answer.Type == Tst_ANS_UNIQUE_CHOICE ||
	  Test->Question.Answer.Type == Tst_ANS_MULTIPLE_CHOICE)
	{
	 Frm_StartForm (ActChgShfTstQst);
	 Tst_PutParamQstCod (&Test->Question.QstCod);
	 Dat_WriteParamsIniEndDates ();
	 Tst_WriteParamEditQst (Test);
	 if (Test->NumQsts == 1)
	    Par_PutHiddenParamChar ("OnlyThisQst",'Y'); // If editing only one question, don't edit others
	 Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Test->SelectedOrder);
	 HTM_INPUT_CHECKBOX ("Shuffle",HTM_SUBMIT_ON_CHANGE,
			     "value=\"Y\"%s",
			     row[2][0] == 'Y' ? " checked=\"checked\"" :
						"");
	 Frm_EndForm ();
	}
      HTM_TD_End ();

      /* Stem (row[3]) */
      HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
      Tst_WriteQstStem (row[3],"TEST_EDI",
			true);	// Visible

      /***** Get and show media (row[5]) *****/
      Test->Question.Media.MedCod = Str_ConvertStrCodToLongCod (row[5]);
      Med_GetMediaDataByCod (&Test->Question.Media);
      Med_ShowMedia (&Test->Question.Media,
		     "TEST_MED_EDIT_LIST_CONT",
		     "TEST_MED_EDIT_LIST");

      /* Feedback (row[4]) and answers */
      Tst_WriteQstFeedback (row[4],"TEST_EDI_LIGHT");
      Tst_WriteAnswersListing (&Test->Question);
      HTM_TD_End ();

      /* Get number of hits
	 (number of times that the question has been answered,
	 including blank answers) (row[6]) */
      if (sscanf (row[6],"%lu",&NumHitsThisQst) != 1)
	 Lay_ShowErrorAndExit ("Wrong number of hits to a question.");

      /* Get number of hits not blank
	 (number of times that the question has been answered
	 with a not blank answer) (row[7]) */
      if (sscanf (row[7],"%lu",&NumHitsNotBlankThisQst) != 1)
	 Lay_ShowErrorAndExit ("Wrong number of hits not blank to a question.");

      /* Get the acumulated score of the question (row[8]) */
      Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
      if (sscanf (row[8],"%lf",&TotalScoreThisQst) != 1)
	 Lay_ShowErrorAndExit ("Wrong score of a question.");
      Str_SetDecimalPointToLocal ();	// Return to local system

      /* Number of times this question has been answered */
      HTM_TD_Begin ("class=\"DAT_SMALL CT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_UnsignedLong (NumHitsThisQst);
      HTM_TD_End ();

      /* Average score */
      HTM_TD_Begin ("class=\"DAT_SMALL CT COLOR%u\"",Gbl.RowEvenOdd);
      if (NumHitsThisQst)
	 HTM_Double2Decimals (TotalScoreThisQst /
	                      (double) NumHitsThisQst);
      else
	 HTM_Txt ("N.A.");
      HTM_TD_End ();

      /* Number of times this question has been answered (not blank) */
      HTM_TD_Begin ("class=\"DAT_SMALL CT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_UnsignedLong (NumHitsNotBlankThisQst);
      HTM_TD_End ();

      /* Average score (not blank) */
      HTM_TD_Begin ("class=\"DAT_SMALL CT COLOR%u\"",Gbl.RowEvenOdd);
      if (NumHitsNotBlankThisQst)
	 HTM_Double2Decimals (TotalScoreThisQst /
	                      (double) NumHitsNotBlankThisQst);
      else
	 HTM_Txt ("N.A.");
      HTM_TD_End ();

      /***** End table row *****/
      HTM_TR_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************** List for selection one or more test questions ***************/
/*****************************************************************************/

static void Tst_ListOneOrMoreQuestionsForSelection (unsigned NumQsts,
                                                    MYSQL_RES *mysql_res)
  {
   extern const char *Hlp_ASSESSMENT_Games_questions;
   extern const char *Txt_Questions;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Code;
   extern const char *Txt_Date;
   extern const char *Txt_Tags;
   extern const char *Txt_Type;
   extern const char *Txt_Shuffle;
   extern const char *Txt_Question;
   extern const char *Txt_Add_questions;
   unsigned NumQst;
   struct Tst_Question Question;
   MYSQL_ROW row;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Questions,
                 NULL,NULL,
		 Hlp_ASSESSMENT_Games_questions,Box_NOT_CLOSABLE);

   /***** Begin form *****/
   Frm_StartForm (ActAddTstQstToGam);
   Gam_PutParams (&Gbl);

   /***** Write the heading *****/
   HTM_TABLE_BeginWideMarginPadding (2);
   HTM_TR_Begin (NULL);

   HTM_TH_Empty (1);

   HTM_TH (1,1,"CT",Txt_No_INDEX);
   HTM_TH (1,1,"CT",Txt_Code);
   HTM_TH (1,1,"CT",Txt_Date);
   HTM_TH (1,1,"LT",Txt_Tags);
   HTM_TH (1,1,"CT",Txt_Type);
   HTM_TH (1,1,"CT",Txt_Shuffle);
   HTM_TH (1,1,"CT",Txt_Question);

   HTM_TR_End ();

   /***** Write rows *****/
   for (NumQst = 0;
	NumQst < NumQsts;
	NumQst++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      /* Create test question */
      Tst_QstConstructor (&Question);

      /* Get question code (row[0]) */
      row = mysql_fetch_row (mysql_res);
      if ((Question.QstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
         Lay_ShowErrorAndExit ("Wrong code of question.");

      /* Write question row */
      Tst_WriteQuestionRowForSelection (NumQst,&Question);

      /* Destroy test question */
      Tst_QstDestructor (&Question);
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Button to add questions *****/
   Btn_PutConfirmButton (Txt_Add_questions);

   /***** End form *****/
   Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************** Write question row for selection *********************/
/*****************************************************************************/

static void Tst_WriteQuestionRowForSelection (unsigned NumQst,
                                              struct Tst_Question *Question)
  {
   extern const char *Txt_TST_STR_ANSWER_TYPES[Tst_NUM_ANS_TYPES];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   static unsigned UniqueId = 0;
   char *Id;
   time_t TimeUTC;

   /***** Get and show questvoidion data *****/
   if (Tst_GetOneQuestionByCod (Question->QstCod,&mysql_res))
     {
      /***** Get row of the result of the query *****/
      row = mysql_fetch_row (mysql_res);
      /*
      row[0] UNIX_TIMESTAMP(EditTime)
      row[1] AnsType
      row[2] Shuffle
      row[3] Stem
      row[4] Feedback
      row[5] MedCod
      row[6] NumHits
      row[7] NumHitsNotBlank
      row[8] Score
      */

      /***** Begin table row *****/
      HTM_TR_Begin (NULL);

      /***** Icons *****/
      HTM_TD_Begin ("class=\"BT%u\"",Gbl.RowEvenOdd);

      /* Write checkbox to select the question */
      HTM_INPUT_CHECKBOX ("QstCods",HTM_DONT_SUBMIT_ON_CHANGE,
			  "value=\"%ld\"",
			  Question->QstCod);

      /* Write number of question */
      HTM_TD_Begin ("class=\"DAT_SMALL CT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TxtF ("%u&nbsp;",NumQst + 1);
      HTM_TD_End ();

      /* Write question code */
      HTM_TD_Begin ("class=\"DAT_SMALL CT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TxtF ("%ld&nbsp;",Question->QstCod);
      HTM_TD_End ();

      /* Write the date (row[0] has the UTC date-time) */
      TimeUTC = Dat_GetUNIXTimeFromStr (row[0]);
      if (asprintf (&Id,"tst_date_%u",++UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      HTM_TD_Begin ("id=\"%s\" class=\"DAT_SMALL CT COLOR%u\">",
		    Id,Gbl.RowEvenOdd);
      Dat_WriteLocalDateHMSFromUTC (Id,TimeUTC,
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				    true,true,false,0x7);
      HTM_TD_End ();
      free (Id);

      /* Write the question tags */
      HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
      Tst_GetAndWriteTagsQst (Question->QstCod);
      HTM_TD_End ();

      /* Write the question type (row[1]) */
      Question->Answer.Type = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);
      HTM_TD_Begin ("class=\"DAT_SMALL CT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TxtF ("%s&nbsp;",Txt_TST_STR_ANSWER_TYPES[Question->Answer.Type]);
      HTM_TD_End ();

      /* Write if shuffle is enabled (row[2]) */
      HTM_TD_Begin ("class=\"DAT_SMALL CT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_INPUT_CHECKBOX ("Shuffle",HTM_DONT_SUBMIT_ON_CHANGE,
			  "value=\"Y\"%s  disabled=\"disabled\"",
			  row[2][0] == 'Y' ? " checked=\"checked\"" :
					     "");
      HTM_TD_End ();

      /* Write stem (row[3]) */
      HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
      Tst_WriteQstStem (row[3],"TEST_EDI",
			true);	// Visible

      /***** Get and show media (row[5]) *****/
      Question->Media.MedCod = Str_ConvertStrCodToLongCod (row[5]);
      Med_GetMediaDataByCod (&Question->Media);
      Med_ShowMedia (&Question->Media,
		     "TEST_MED_EDIT_LIST_CONT",
		     "TEST_MED_EDIT_LIST");

      /* Write feedback (row[4]) */
      Tst_WriteQstFeedback (row[4],"TEST_EDI_LIGHT");

      /* Write answers */
      Tst_WriteAnswersListing (Question);
      HTM_TD_End ();

      /***** End table row *****/
      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/*********** Write hidden parameters for edition of test questions ***********/
/*****************************************************************************/

void Tst_WriteParamEditQst (const struct Tst_Test *Test)
  {
   Par_PutHiddenParamChar   ("AllTags",Test->Tags.All ? 'Y' :
                        	                        'N');
   Par_PutHiddenParamString (NULL,"ChkTag",Test->Tags.List ? Test->Tags.List :
                        	                             "");
   Par_PutHiddenParamChar   ("AllAnsTypes",Test->AnswerTypes.All ? 'Y' :
                        	                                   'N');
   Par_PutHiddenParamString (NULL,"AnswerType",Test->AnswerTypes.List);
  }

/*****************************************************************************/
/*************** Get answers of a test question from database ****************/
/*****************************************************************************/

unsigned Tst_GetNumAnswersQst (long QstCod)
  {
   return (unsigned) DB_QueryCOUNT ("can not get number of answers of a question",
			            "SELECT COUNT(*)"
			            " FROM tst_answers"
			            " WHERE QstCod=%ld",
			            QstCod);
  }

void Tst_GetAnswersQst (struct Tst_Question *Question,MYSQL_RES **mysql_res,
                        bool Shuffle)
  {
   /***** Get answers of a question from database *****/
   Question->Answer.NumOptions = (unsigned)
   DB_QuerySELECT (mysql_res,"can not get answers of a question",
		   "SELECT AnsInd,"		// row[0]
			  "Answer,"		// row[1]
			  "Feedback,"		// row[2]
			  "MedCod,"		// row[3]
			  "Correct"		// row[4]
		   " FROM tst_answers"
		   " WHERE QstCod=%ld"
		   " ORDER BY %s",
		   Question->QstCod,
		   Shuffle ? "RAND(NOW())" :
		             "AnsInd");
   if (!Question->Answer.NumOptions)
      Ale_ShowAlert (Ale_ERROR,"Error when getting answers of a question.");
  }

void Tst_GetCorrectAnswersFromDB (struct Tst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumOpt;

   /***** Query database *****/
   Question->Answer.NumOptions =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get answers of a question",
			      "SELECT Correct"		// row[0]
			      " FROM tst_answers"
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

/*****************************************************************************/
/**************** Get and write the answers of a test question ***************/
/*****************************************************************************/

void Tst_WriteAnswersListing (struct Tst_Question *Question)
  {
   MYSQL_RES *mysql_res;

   /***** Get answers *****/
   Tst_GetAnswersQst (Question,&mysql_res,
                      false);	// Don't shuffle
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */

   /***** Write answers *****/
   switch (Question->Answer.Type)
     {
      case Tst_ANS_TRUE_FALSE:
         Tst_WriteTFAnsListing (Question,mysql_res);
         break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
      case Tst_ANS_TEXT:
         Tst_WriteChoiceAnsListing (Question,mysql_res);
	 break;
      case Tst_ANS_INT:
         Tst_WriteIntAnsListing (Question,mysql_res);
         break;
      case Tst_ANS_FLOAT:
         Tst_WriteFloatAnsEdit (Question,mysql_res);
         break;
      default:
         break;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Write answers of a question when seeing a test ***************/
/*****************************************************************************/

static void Tst_WriteAnswersSeeing (unsigned NumQst,
                                    struct Tst_Question *Question,
                                    bool Shuffle)
  {
   /***** Write parameter with question code *****/
   Tst_WriteParamQstCod (NumQst,Question->QstCod);

   /***** Write answer depending on type *****/
   switch (Question->Answer.Type)
     {
      case Tst_ANS_INT:
         Tst_WriteIntAnsSeeing (NumQst);
         break;
      case Tst_ANS_FLOAT:
         Tst_WriteFloatAnsSeeing (NumQst);
         break;
      case Tst_ANS_TRUE_FALSE:
         Tst_WriteTFAnsSeeing (NumQst);
         break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
         Tst_WriteChoiceAnsSeeing (NumQst,Question,Shuffle);
         break;
      case Tst_ANS_TEXT:
         Tst_WriteTextAnsSeeing (NumQst);
         break;
      default:
         break;
     }
  }

/*****************************************************************************/
/************* Write answers of a question when assessing a test *************/
/*****************************************************************************/

static void Tst_WriteAnswersResult (struct UsrData *UsrDat,
                                    const struct TsR_Result *Result,
                                    unsigned NumQst,
				    struct Tst_Question *Question,
				    unsigned Visibility,
                                    double *ScoreThisQst,bool *AnswerIsNotBlank)
  {
   MYSQL_RES *mysql_res;

   /***** Get answer of a question from database *****/
   Tst_GetAnswersQst (Question,&mysql_res,
                      false);	// Don't shuffle
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */

   /***** Write answer depending on type *****/
   switch (Question->Answer.Type)
     {
      case Tst_ANS_INT:
         Tst_WriteIntAnsResult    (UsrDat,Result,
                                   NumQst,Question,mysql_res,
				   Visibility,
				   ScoreThisQst,AnswerIsNotBlank);
         break;
      case Tst_ANS_FLOAT:
	 Tst_WriteFloatAnsResult  (UsrDat,Result,
	                           NumQst,Question,mysql_res,
				   Visibility,
				   ScoreThisQst,AnswerIsNotBlank);
         break;
      case Tst_ANS_TRUE_FALSE:
         Tst_WriteTFAnsResult     (UsrDat,Result,
                                   NumQst,Question,mysql_res,
				   Visibility,
				   ScoreThisQst,AnswerIsNotBlank);
         break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
         Tst_WriteChoiceAnsResult (UsrDat,Result,
                                   NumQst,Question,mysql_res,
				   Visibility,
				   ScoreThisQst,AnswerIsNotBlank);
         break;
      case Tst_ANS_TEXT:
         Tst_WriteTextAnsResult   (UsrDat,Result,
                                   NumQst,Question,mysql_res,
				   Visibility,
				   ScoreThisQst,AnswerIsNotBlank);
         break;
      default:
         break;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Check if a question is valid for a game *******************/
/*****************************************************************************/

bool Tst_CheckIfQuestionIsValidForGame (long QstCod)
  {
   /***** Check if a question is valid for a game from database *****/
   return DB_QueryCOUNT ("can not check type of a question",
			 "SELECT COUNT(*)"
			 " FROM tst_questions"
			 " WHERE QstCod=%ld AND AnsType='%s'",
			 QstCod,Tst_StrAnswerTypesDB[Tst_ANS_UNIQUE_CHOICE]) != 0;
  }

/*****************************************************************************/
/*********** Write false / true answer when listing test questions ***********/
/*****************************************************************************/

static void Tst_WriteTFAnsListing (const struct Tst_Question *Question,
                                   MYSQL_RES *mysql_res)
  {
   MYSQL_ROW row;
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */

   Tst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get answer true or false *****/
   row = mysql_fetch_row (mysql_res);

   /***** Write answer *****/
   HTM_SPAN_Begin ("class=\"TEST_EDI\"");
   HTM_Txt ("(");
   Tst_WriteAnsTF (row[1][0]);
   HTM_Txt (")");
   HTM_SPAN_End ();
  }

/*****************************************************************************/
/************** Write false / true answer when seeing a test ****************/
/*****************************************************************************/

static void Tst_WriteTFAnsSeeing (unsigned NumQst)
  {
   extern const char *Txt_TF_QST[2];

   /***** Write selector for the answer *****/
   HTM_SELECT_Begin (false,
		     "name=\"Ans%010u\"",NumQst);
   HTM_OPTION (HTM_Type_STRING,"" ,true ,false,"&nbsp;");
   HTM_OPTION (HTM_Type_STRING,"T",false,false,"%s",Txt_TF_QST[0]);
   HTM_OPTION (HTM_Type_STRING,"F",false,false,"%s",Txt_TF_QST[1]);
   HTM_SELECT_End ();
  }

/*****************************************************************************/
/************** Write false / true answer when seeing a test *****************/
/*****************************************************************************/

void Tst_WriteAnsTF (char AnsTF)
  {
   extern const char *Txt_TF_QST[2];

   switch (AnsTF)
     {
      case 'T':		// true
         HTM_Txt (Txt_TF_QST[0]);
         break;
      case 'F':		// false
         HTM_Txt (Txt_TF_QST[1]);
         break;
      default:		// no answer
         HTM_NBSP ();
         break;
     }
  }

/*****************************************************************************/
/************** Write false / true answer when assessing a test **************/
/*****************************************************************************/

static void Tst_WriteTFAnsResult (struct UsrData *UsrDat,
                                  const struct TsR_Result *Result,
				  unsigned NumQst,
				  const struct Tst_Question *Question,
				  MYSQL_RES *mysql_res,
				  unsigned Visibility,
                                  double *ScoreThisQst,
                                  bool *AnswerIsNotBlank)
  {
   MYSQL_ROW row;
   char AnsTF;
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */
   /***** Check if number of rows is correct *****/
   Tst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get answer true or false *****/
   row = mysql_fetch_row (mysql_res);

   /***** Compute the mark for this question *****/
   AnsTF = Result->StrAnswers[NumQst][0];
   if (AnsTF == '\0')			// User has omitted the answer (the answer is blank)
     {
      *AnswerIsNotBlank = false;
      *ScoreThisQst = 0.0;
     }
   else
     {
      *AnswerIsNotBlank = true;
      if (AnsTF == row[1][0])		// Correct
         *ScoreThisQst = 1.0;
      else				// Wrong
         *ScoreThisQst = -1.0;
     }

   /***** Header with the title of each column *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   Tst_WriteHeadUserCorrect (UsrDat);
   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Write the user answer *****/
   HTM_TD_Begin ("class=\"%s CM\"",
		 TsV_IsVisibleCorrectAns (Visibility) ?
		    (AnsTF == row[1][0] ? "ANS_OK" :
					  "ANS_BAD") :
		    "ANS_0");
   Tst_WriteAnsTF (AnsTF);
   HTM_TD_End ();

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"ANS_0 CM\"");
   if (TsV_IsVisibleQstAndAnsTxt (Visibility) &&
       TsV_IsVisibleCorrectAns   (Visibility))
      Tst_WriteAnsTF (row[1][0]);
   else
      Ico_PutIconNotVisible ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Write the score of this question *****/
   if (TsV_IsVisibleEachQstScore (Visibility))
     {
      Tst_WriteScoreStart (2);
      if (AnsTF == '\0')		// If user has omitted the answer
	{
         HTM_SPAN_Begin ("class=\"ANS_0\"");
         HTM_Double2Decimals (0.0);
	}
      else if (AnsTF == row[1][0])	// If correct
	{
         HTM_SPAN_Begin ("class=\"ANS_OK\"");
         HTM_Double2Decimals (1.0);
	}
      else				// If wrong
	{
         HTM_SPAN_Begin ("class=\"ANS_BAD\"");
         HTM_Double2Decimals (-1.0);
	}
      HTM_SPAN_End ();
      Tst_WriteScoreEnd ();
     }

   HTM_TABLE_End ();
  }


/*****************************************************************************/
/**** Write single or multiple choice answer when listing test questions *****/
/*****************************************************************************/

static void Tst_WriteChoiceAnsListing (struct Tst_Question *Question,
                                       MYSQL_RES *mysql_res)
  {
   extern const char *Txt_TST_Answer_given_by_the_teachers;
   MYSQL_ROW row;
   unsigned NumOpt;
   char *AnswerTxt;
   char *Feedback;
   size_t LengthAnswerTxt;
   size_t LengthFeedback;
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */

   HTM_TABLE_BeginPadding (2);
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Convert the answer (row[1]), that is in HTML, to rigorous HTML */
      LengthAnswerTxt = strlen (row[1]) * Str_MAX_BYTES_PER_CHAR;
      if ((AnswerTxt = (char *) malloc (LengthAnswerTxt + 1)) == NULL)
	 Lay_NotEnoughMemoryExit ();
      Str_Copy (AnswerTxt,row[1],
		LengthAnswerTxt);
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			AnswerTxt,LengthAnswerTxt,false);

      /* Convert the feedback (row[2]), that is in HTML, to rigorous HTML */
      LengthFeedback = 0;
      Feedback = NULL;
      if (row[2])
	 if (row[2][0])
	   {
	    LengthFeedback = strlen (row[2]) * Str_MAX_BYTES_PER_CHAR;
	    if ((Feedback = (char *) malloc (LengthFeedback + 1)) == NULL)
	       Lay_NotEnoughMemoryExit ();
	    Str_Copy (Feedback,row[2],
		      LengthFeedback);
	    Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			      Feedback,LengthFeedback,false);
	   }

      /* Get media (row[3]) */
      Question->Answer.Options[NumOpt].Media.MedCod = Str_ConvertStrCodToLongCod (row[3]);
      Med_GetMediaDataByCod (&Question->Answer.Options[NumOpt].Media);

      HTM_TR_Begin (NULL);

      /* Put an icon that indicates whether the answer
	 is correct or wrong (row[4]) */
      HTM_TD_Begin ("class=\"BT%u\"",Gbl.RowEvenOdd);
      if (row[4][0] == 'Y')
	 Ico_PutIcon ("check.svg",Txt_TST_Answer_given_by_the_teachers,"CONTEXT_ICO_16x16");
      HTM_TD_End ();

      /* Write the number of option */
      HTM_TD_Begin ("class=\"DAT_SMALL LT\"");
      HTM_TxtF ("%c)&nbsp;",'a' + (char) NumOpt);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LT\"");

      /* Write the text of the answer and the media */
      HTM_DIV_Begin ("class=\"TEST_EDI\"");
      HTM_Txt (AnswerTxt);
      Med_ShowMedia (&Question->Answer.Options[NumOpt].Media,
		     "TEST_MED_EDIT_LIST_CONT",
		     "TEST_MED_EDIT_LIST");
      HTM_DIV_End ();

      /* Write the text of the feedback */
      HTM_DIV_Begin ("class=\"TEST_EDI_LIGHT\"");
      if (LengthFeedback)
	 HTM_Txt (Feedback);
      HTM_DIV_End ();

      HTM_TD_End ();

      HTM_TR_End ();

      /* Free memory allocated for the answer and the feedback */
      free (AnswerTxt);
      if (LengthFeedback)
	 free (Feedback);
     }
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******** Write single or multiple choice answer when seeing a test **********/
/*****************************************************************************/

static void Tst_WriteChoiceAnsSeeing (unsigned NumQst,
                                      struct Tst_Question *Question,
                                      bool Shuffle)
  {
   unsigned NumOpt;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned Index;
   bool ErrorInIndex = false;
   char StrQstIndOrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Qstxx...x", "Indxx...x" or "Ansxx...x"

   /***** Get answers of a question from database *****/
   Tst_GetAnswersQst (Question,&mysql_res,Shuffle);
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

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

      /***** Assign index (row[0]).
             Index is 0,1,2,3... if no shuffle
             or 1,3,0,2... (example) if shuffle *****/
      if (sscanf (row[0],"%u",&Index) == 1)
        {
         if (Index >= Tst_MAX_OPTIONS_PER_QUESTION)
            ErrorInIndex = true;
        }
      else
         ErrorInIndex = true;
      if (ErrorInIndex)
         Lay_ShowErrorAndExit ("Wrong index of answer when showing a test.");

      /***** Copy text (row[1]) and convert it, that is in HTML, to rigorous HTML ******/
      Str_Copy (Question->Answer.Options[NumOpt].Text,row[1],
                Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Question->Answer.Options[NumOpt].Text,
                        Tst_MAX_BYTES_ANSWER_OR_FEEDBACK,false);

      /***** Get media (row[3]) *****/
      Question->Answer.Options[NumOpt].Media.MedCod = Str_ConvertStrCodToLongCod (row[3]);
      Med_GetMediaDataByCod (&Question->Answer.Options[NumOpt].Media);

      HTM_TR_Begin (NULL);

      /***** Write selectors and letter of this option *****/
      HTM_TD_Begin ("class=\"LT\"");
      snprintf (StrQstIndOrAns,sizeof (StrQstIndOrAns),
	        "Ind%010u",
		NumQst);
      Par_PutHiddenParamUnsigned (NULL,StrQstIndOrAns,Index);
      snprintf (StrQstIndOrAns,sizeof (StrQstIndOrAns),
		"Ans%010u",
		NumQst);
      if (Question->Answer.Type == Tst_ANS_UNIQUE_CHOICE)
	 HTM_INPUT_RADIO (StrQstIndOrAns,false,
			  "id=\"Ans%010u_%u\" value=\"%u\""
			  " onclick=\"selectUnselectRadio(this,this.form.Ans%010u,%u);\"",
			  NumQst,NumOpt,
			  Index,
                          NumQst,Question->Answer.NumOptions);
      else // Answer.Type == Tst_ANS_MULTIPLE_CHOICE
	 HTM_INPUT_CHECKBOX (StrQstIndOrAns,HTM_DONT_SUBMIT_ON_CHANGE,
			     "id=\"Ans%010u_%u\" value=\"%u\"",
			     NumQst,NumOpt,
			     Index);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LT\"");
      HTM_LABEL_Begin ("for=\"Ans%010u_%u\" class=\"ANS_TXT\"",NumQst,NumOpt);
      HTM_TxtF ("%c)&nbsp;",'a' + (char) NumOpt);
      HTM_LABEL_End ();
      HTM_TD_End ();

      /***** Write the option text *****/
      HTM_TD_Begin ("class=\"LT\"");
      HTM_LABEL_Begin ("for=\"Ans%010u_%u\" class=\"ANS_TXT\"",NumQst,NumOpt);
      HTM_Txt (Question->Answer.Options[NumOpt].Text);
      HTM_LABEL_End ();
      Med_ShowMedia (&Question->Answer.Options[NumOpt].Media,
                     "TEST_MED_SHOW_CONT",
                     "TEST_MED_SHOW");
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******* Write single or multiple choice answer when assessing a test ********/
/*****************************************************************************/

static void Tst_WriteChoiceAnsResult (struct UsrData *UsrDat,
                                      const struct TsR_Result *Result,
				      unsigned NumQst,
				      struct Tst_Question *Question,
				      MYSQL_RES *mysql_res,
				      unsigned Visibility,
                                      double *ScoreThisQst,
                                      bool *AnswerIsNotBlank)
  {
   extern const char *Txt_TST_Answer_given_by_the_user;
   extern const char *Txt_TST_Answer_given_by_the_teachers;
   unsigned NumOpt;
   unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   bool AnswersUsr[Tst_MAX_OPTIONS_PER_QUESTION];
   struct
     {
      char *Class;
      char *Str;
     } Ans;

   /***** Get text and correctness of answers for this question
          from database (one row per answer) *****/
   Tst_GetChoiceAns (Question,mysql_res);

   /***** Get indexes for this question from string *****/
   Tst_GetIndexesFromStr (Result->StrIndexes[NumQst],Indexes);

   /***** Get the user's answers for this question from string *****/
   Tst_GetAnswersFromStr (Result->StrAnswers[NumQst],AnswersUsr);

   /***** Compute the total score of this question *****/
   Tst_ComputeScoreQst (Question,Indexes,AnswersUsr,ScoreThisQst,AnswerIsNotBlank);

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   Tst_WriteHeadUserCorrect (UsrDat);
   HTM_TD_Empty (2);
   HTM_TR_End ();

   /***** Write answers (one row per answer) *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      HTM_TR_Begin (NULL);

      /* Draw icon depending on user's answer */
      if (AnswersUsr[Indexes[NumOpt]] == true)	// This answer has been selected by the user
        {
         if (TsV_IsVisibleCorrectAns (Visibility))
           {
            if (Question->Answer.Options[Indexes[NumOpt]].Correct)
              {
               Ans.Class = "ANS_OK";
               Ans.Str   = "&check;";
              }
            else
              {
               Ans.Class = "ANS_BAD";
               Ans.Str   = "&cross;";
              }
           }
         else
	   {
	    Ans.Class = "ANS_0";
	    Ans.Str   = "&bull;";
	   }

	 HTM_TD_Begin ("class=\"%s CT\" title=\"%s\"",
		       Ans.Class,Txt_TST_Answer_given_by_the_user);
	 HTM_Txt (Ans.Str);
	 HTM_TD_End ();
        }
      else	// This answer has NOT been selected by the user
         HTM_TD_Empty (1);

      /* Draw icon that indicates whether the answer is correct */
      if (TsV_IsVisibleCorrectAns (Visibility))
        {
         if (Question->Answer.Options[Indexes[NumOpt]].Correct)
           {
	    HTM_TD_Begin ("class=\"ANS_0 CT\" title=\"%s\"",
		          Txt_TST_Answer_given_by_the_teachers);
	    HTM_Txt ("&bull;");
	    HTM_TD_End ();
           }
         else
            HTM_TD_Empty (1);
        }
      else
	{
	 HTM_TD_Begin ("class=\"ANS_0 CT\"");
         Ico_PutIconNotVisible ();
         HTM_TD_End ();
	}

      /* Answer letter (a, b, c,...) */
      HTM_TD_Begin ("class=\"ANS_TXT LT\"");
      HTM_TxtF ("%c)&nbsp;",'a' + (char) NumOpt);
      HTM_TD_End ();

      /* Answer text and feedback */
      HTM_TD_Begin ("class=\"LT\"");

      HTM_DIV_Begin ("class=\"ANS_TXT\"");
      if (TsV_IsVisibleQstAndAnsTxt (Visibility))
	{
	 HTM_Txt (Question->Answer.Options[Indexes[NumOpt]].Text);
	 Med_ShowMedia (&Question->Answer.Options[Indexes[NumOpt]].Media,
			"TEST_MED_SHOW_CONT",
			"TEST_MED_SHOW");
	}
      else
         Ico_PutIconNotVisible ();
      HTM_DIV_End ();

      if (TsV_IsVisibleCorrectAns (Visibility))
	 if (Question->Answer.Options[Indexes[NumOpt]].Feedback)
	    if (Question->Answer.Options[Indexes[NumOpt]].Feedback[0])
	      {
	       HTM_DIV_Begin ("class=\"TEST_EXA_LIGHT\"");
	       HTM_Txt (Question->Answer.Options[Indexes[NumOpt]].Feedback);
	       HTM_DIV_End ();
	      }

      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** Write the score of this question *****/
   if (TsV_IsVisibleEachQstScore (Visibility))
     {
      Tst_WriteScoreStart (4);
      if (*ScoreThisQst == 0.0)
         HTM_SPAN_Begin ("class=\"ANS_0\"");
      else if (*ScoreThisQst > 0.0)
         HTM_SPAN_Begin ("class=\"ANS_OK\"");
      else
         HTM_SPAN_Begin ("class=\"ANS_BAD\"");
      HTM_Double2Decimals (*ScoreThisQst);
      HTM_SPAN_End ();
      Tst_WriteScoreEnd ();
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************************ Get choice answer from row *************************/
/*****************************************************************************/

static void Tst_GetChoiceAns (struct Tst_Question *Question,MYSQL_RES *mysql_res)
  {
   unsigned NumOpt;
   MYSQL_ROW row;

   /***** Get text and correctness of answers for this question
          from database (one row per answer) *****/
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      /***** Get next answer *****/
      row = mysql_fetch_row (mysql_res);

      /***** Allocate memory for text in this choice option *****/
      if (!Tst_AllocateTextChoiceAnswer (Question,NumOpt))
	 /* Abort on error */
	 Ale_ShowAlertsAndExit ();

      /***** Copy answer text (row[1]) and convert it,
             that is in HTML, to rigorous HTML ******/
      Str_Copy (Question->Answer.Options[NumOpt].Text,row[1],
                Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Question->Answer.Options[NumOpt].Text,
                        Tst_MAX_BYTES_ANSWER_OR_FEEDBACK,false);

      /***** Copy answer feedback (row[2]) and convert it,
             that is in HTML, to rigorous HTML ******/
      if (TsV_IsVisibleFeedbackTxt (TstCfg_GetConfigVisibility ()))
	 if (row[2])
	    if (row[2][0])
	      {
	       Str_Copy (Question->Answer.Options[NumOpt].Feedback,row[2],
	                 Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
	       Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
	                         Question->Answer.Options[NumOpt].Feedback,
	                         Tst_MAX_BYTES_ANSWER_OR_FEEDBACK,false);
	      }

      /***** Get media (row[3]) *****/
      Question->Answer.Options[NumOpt].Media.MedCod = Str_ConvertStrCodToLongCod (row[3]);
      Med_GetMediaDataByCod (&Question->Answer.Options[NumOpt].Media);

      /***** Assign correctness (row[4]) of this answer (this option) *****/
      Question->Answer.Options[NumOpt].Correct = (row[4][0] == 'Y');
     }
  }

/*****************************************************************************/
/********************* Get vector of indexes from string *********************/
/*****************************************************************************/

void Tst_GetIndexesFromStr (const char StrIndexesOneQst[Tst_MAX_BYTES_INDEXES_ONE_QST + 1],	// 0 1 2 3, 3 0 2 1, etc.
			    unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION])
  {
   unsigned NumOpt;
   const char *Ptr;
   char StrOneIndex[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Get indexes from string *****/
   for (NumOpt = 0, Ptr = StrIndexesOneQst;
	NumOpt < Tst_MAX_OPTIONS_PER_QUESTION && *Ptr;
	NumOpt++)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,StrOneIndex,Cns_MAX_DECIMAL_DIGITS_UINT);

      if (sscanf (StrOneIndex,"%u",&(Indexes[NumOpt])) != 1)
	 Lay_ShowErrorAndExit ("Wrong index of answer.");

      if (Indexes[NumOpt] >= Tst_MAX_OPTIONS_PER_QUESTION)
	 Lay_ShowErrorAndExit ("Wrong index of answer.");
     }

   /***** Initialize remaining to 0 *****/
   for (;
	NumOpt < Tst_MAX_OPTIONS_PER_QUESTION;
	NumOpt++)
      Indexes[NumOpt] = 0;
  }

/*****************************************************************************/
/****************** Get vector of user's answers from string *****************/
/*****************************************************************************/

void Tst_GetAnswersFromStr (const char StrAnswersOneQst[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1],
			    bool AnswersUsr[Tst_MAX_OPTIONS_PER_QUESTION])
  {
   unsigned NumOpt;
   const char *Ptr;
   char StrOneAnswer[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   unsigned AnsUsr;

   /***** Initialize all answers to false *****/
   for (NumOpt = 0;
	NumOpt < Tst_MAX_OPTIONS_PER_QUESTION;
	NumOpt++)
      AnswersUsr[NumOpt] = false;

   /***** Set selected answers to true *****/
   for (NumOpt = 0, Ptr = StrAnswersOneQst;
	NumOpt < Tst_MAX_OPTIONS_PER_QUESTION && *Ptr;
	NumOpt++)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,StrOneAnswer,Cns_MAX_DECIMAL_DIGITS_UINT);

      if (sscanf (StrOneAnswer,"%u",&AnsUsr) != 1)
	 Lay_ShowErrorAndExit ("Bad user's answer.");

      if (AnsUsr >= Tst_MAX_OPTIONS_PER_QUESTION)
	 Lay_ShowErrorAndExit ("Bad user's answer.");

      AnswersUsr[AnsUsr] = true;
     }
  }

/*****************************************************************************/
/********************* Compute the score of this question ********************/
/*****************************************************************************/

void Tst_ComputeScoreQst (const struct Tst_Question *Question,
                          unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION],	// Indexes of all answers of this question
                          bool AnswersUsr[Tst_MAX_OPTIONS_PER_QUESTION],
			  double *ScoreThisQst,bool *AnswerIsNotBlank)
  {
   unsigned NumOpt;
   unsigned NumOptTotInQst = 0;
   unsigned NumOptCorrInQst = 0;
   unsigned NumAnsGood = 0;
   unsigned NumAnsBad = 0;

   /***** Compute the total score of this question *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      NumOptTotInQst++;
      if (Question->Answer.Options[Indexes[NumOpt]].Correct)
         NumOptCorrInQst++;

      if (AnswersUsr[Indexes[NumOpt]] == true)	// This answer has been selected by the user
        {
         if (Question->Answer.Options[Indexes[NumOpt]].Correct)
            NumAnsGood++;
         else
            NumAnsBad++;
        }
     }

   /* The answer is blank? */
   *AnswerIsNotBlank = NumAnsGood != 0 || NumAnsBad != 0;
   if (*AnswerIsNotBlank)
     {
      /* Compute the score */
      if (Question->Answer.Type == Tst_ANS_UNIQUE_CHOICE)
        {
         if (NumOptTotInQst >= 2)	// It should be 2 options at least
            *ScoreThisQst = (double) NumAnsGood -
                            (double) NumAnsBad / (double) (NumOptTotInQst - 1);
         else			// 0 or 1 options (impossible)
            *ScoreThisQst = (double) NumAnsGood;
        }
      else	// AnswerType == Tst_ANS_MULTIPLE_CHOICE
        {
         if (NumOptCorrInQst)	// There are correct options in the question
           {
            if (NumOptCorrInQst < NumOptTotInQst)	// If there are correct options and wrong options (typical case)
               *ScoreThisQst = (double) NumAnsGood / (double) NumOptCorrInQst -
                               (double) NumAnsBad / (double) (NumOptTotInQst - NumOptCorrInQst);
            else					// Si todas the opciones son correctas (caso raro)
               *ScoreThisQst = (double) NumAnsGood / (double) NumOptCorrInQst;
           }
         else
           {
            if (NumOptTotInQst)	// There are options but none is correct (extrange case)
               *ScoreThisQst = - (double) NumAnsBad / (double) NumOptTotInQst;
            else			// There are no options (impossible!)
               *ScoreThisQst = 0.0;
           }
        }
     }
   else	// Answer is blank
      *ScoreThisQst = 0.0;
  }

/*****************************************************************************/
/******************** Write text answer when seeing a test *******************/
/*****************************************************************************/

static void Tst_WriteTextAnsSeeing (unsigned NumQst)
  {
   char StrQstIndOrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Qstxx...x", "Indxx...x" or "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (StrQstIndOrAns,sizeof (StrQstIndOrAns),
	     "Ans%010u",
	     NumQst);
   HTM_INPUT_TEXT (StrQstIndOrAns,Tst_MAX_BYTES_ANSWERS_ONE_QST,"",false,
		   "size=\"40\"");
  }

/*****************************************************************************/
/***************** Write text answer when assessing a test *******************/
/*****************************************************************************/

static void Tst_WriteTextAnsResult (struct UsrData *UsrDat,
                                    const struct TsR_Result *Result,
				    unsigned NumQst,
				    struct Tst_Question *Question,
				    MYSQL_RES *mysql_res,
				    unsigned Visibility,
                                    double *ScoreThisQst,
                                    bool *AnswerIsNotBlank)
  {
   unsigned NumOpt;
   MYSQL_ROW row;
   char TextAnsUsr[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   bool Correct = false;
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */
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

      /***** Copy answer text (row[1]) and convert it, that is in HTML, to rigorous HTML ******/
      Str_Copy (Question->Answer.Options[NumOpt].Text,row[1],
                Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Question->Answer.Options[NumOpt].Text,
                        Tst_MAX_BYTES_ANSWER_OR_FEEDBACK,false);

      /***** Copy answer feedback (row[2]) and convert it, that is in HTML, to rigorous HTML ******/
      if (TsV_IsVisibleFeedbackTxt (Visibility))
	 if (row[2])
	    if (row[2][0])
	      {
	       Str_Copy (Question->Answer.Options[NumOpt].Feedback,row[2],
	                 Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
	       Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
	                         Question->Answer.Options[NumOpt].Feedback,
	                         Tst_MAX_BYTES_ANSWER_OR_FEEDBACK,false);
	      }

      /***** Assign correctness (row[4]) of this answer (this option) *****/
      Question->Answer.Options[NumOpt].Correct = (row[4][0] == 'Y');
     }

   /***** Header with the title of each column *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   Tst_WriteHeadUserCorrect (UsrDat);
   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Write the user answer *****/
   if (Result->StrAnswers[NumQst][0])	// If user has answered the question
     {
      /* Filter the user answer */
      Str_Copy (TextAnsUsr,Result->StrAnswers[NumQst],
                Tst_MAX_BYTES_ANSWERS_ONE_QST);

      /* In order to compare student answer to stored answer,
	 the text answers are stored avoiding two or more consecurive spaces */
      Str_ReplaceSeveralSpacesForOne (TextAnsUsr);

      Str_ConvertToComparable (TextAnsUsr);

      for (NumOpt = 0;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
        {
         /* Filter this correct answer */
         Str_Copy (TextAnsOK,Question->Answer.Options[NumOpt].Text,
                   Tst_MAX_BYTES_ANSWERS_ONE_QST);
         Str_ConvertToComparable (TextAnsOK);

         /* Check is user answer is correct */
         if (!strcoll (TextAnsUsr,TextAnsOK))
           {
            Correct = true;
            break;
           }
        }
      HTM_TD_Begin ("class=\"%s CT\"",
		    TsV_IsVisibleCorrectAns (Visibility) ?
		       (Correct ? "ANS_OK" :
				  "ANS_BAD") :
		       "ANS_0");
      HTM_Txt (Result->StrAnswers[NumQst]);
     }
   else						// If user has omitted the answer
      HTM_TD_Begin (NULL);
   HTM_TD_End ();

   /***** Write the correct answers *****/
   if (TsV_IsVisibleQstAndAnsTxt (Visibility) &&
       TsV_IsVisibleCorrectAns   (Visibility))
     {
      HTM_TD_Begin ("class=\"CT\"");
      HTM_TABLE_BeginPadding (2);

      for (NumOpt = 0;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
        {
	 HTM_TR_Begin (NULL);

         /* Answer letter (a, b, c,...) */
         HTM_TD_Begin ("class=\"ANS_0 LT\"");
         HTM_TxtF ("%c)&nbsp;",'a' + (char) NumOpt);
         HTM_TD_End ();

         /* Answer text and feedback */
         HTM_TD_Begin ("class=\"LT\"");

         HTM_DIV_Begin ("class=\"ANS_0\"");
         HTM_Txt (Question->Answer.Options[NumOpt].Text);
         HTM_DIV_End ();

         if (TsV_IsVisibleFeedbackTxt (Visibility))
	    if (Question->Answer.Options[NumOpt].Feedback)
	       if (Question->Answer.Options[NumOpt].Feedback[0])
		 {
		  HTM_DIV_Begin ("class=\"TEST_EXA_LIGHT\"");
		  HTM_Txt (Question->Answer.Options[NumOpt].Feedback);
		  HTM_DIV_End ();
		 }

	 HTM_TD_End ();

	 HTM_TR_End ();
        }

      HTM_TABLE_End ();
      HTM_TD_End ();
     }
   else
     {
      HTM_TD_Begin ("class=\"ANS_0 CT\"");
      Ico_PutIconNotVisible ();
      HTM_TD_End ();
     }
   HTM_TR_End ();

   /***** Compute the mark *****/
   if (!Result->StrAnswers[NumQst][0])	// If user has omitted the answer
     {
      *AnswerIsNotBlank = false;
      *ScoreThisQst = 0.0;
     }
   else
     {
      *AnswerIsNotBlank = true;
      if (Correct)				// If correct
         *ScoreThisQst = 1.0;
      else					// If wrong
         *ScoreThisQst = 0.0;
     }

   /***** Write the score of this question *****/
   if (TsV_IsVisibleEachQstScore (Visibility))
     {
      Tst_WriteScoreStart (4);
      if (!Result->StrAnswers[NumQst][0])	// If user has omitted the answer
	{
         HTM_SPAN_Begin ("class=\"ANS_0\"");
         HTM_Double2Decimals (0.0);
	}
      else if (Correct)					// If correct
	{
         HTM_SPAN_Begin ("class=\"ANS_OK\"");
         HTM_Double2Decimals (1.0);
	}
      else						// If wrong
	{
         HTM_SPAN_Begin ("class=\"ANS_BAD\"");
         HTM_Double2Decimals (0.0);
	}
      HTM_SPAN_End ();
      Tst_WriteScoreEnd ();
     }

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/****************** Write integer answer when editing a test *****************/
/*****************************************************************************/

static void Tst_WriteIntAnsListing (const struct Tst_Question *Question,
                                    MYSQL_RES *mysql_res)
  {
   MYSQL_ROW row;
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */

   Tst_CheckIfNumberOfAnswersIsOne (Question);
   row = mysql_fetch_row (mysql_res);
   HTM_SPAN_Begin ("class=\"TEST_EDI\"");
   HTM_TxtF ("(%ld)",Tst_GetIntAnsFromStr (row[1]));
   HTM_SPAN_End ();
  }

/*****************************************************************************/
/****************** Write integer answer when seeing a test ******************/
/*****************************************************************************/

static void Tst_WriteIntAnsSeeing (unsigned NumQst)
  {
   char StrQstIndOrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Qstxx...x", "Indxx...x" or "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (StrQstIndOrAns,sizeof (StrQstIndOrAns),
	     "Ans%010u",
	     NumQst);
   HTM_INPUT_TEXT (StrQstIndOrAns,11,"",false,
		   "size=\"11\"");
  }

/*****************************************************************************/
/**************** Write integer answer when assessing a test *****************/
/*****************************************************************************/

static void Tst_WriteIntAnsResult (struct UsrData *UsrDat,
                                   const struct TsR_Result *Result,
				   unsigned NumQst,
				   const struct Tst_Question *Question,
				   MYSQL_RES *mysql_res,
				   unsigned Visibility,
                                   double *ScoreThisQst,
                                   bool *AnswerIsNotBlank)
  {
   MYSQL_ROW row;
   long IntAnswerUsr;
   long IntAnswerCorr;
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */
   /***** Check if number of rows is correct *****/
   Tst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get the numerical value of the correct answer *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[1],"%ld",&IntAnswerCorr) != 1)
      Lay_ShowErrorAndExit ("Wrong integer answer.");

   /***** Header with the title of each column *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   Tst_WriteHeadUserCorrect (UsrDat);
   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Write the user answer *****/
   if (Result->StrAnswers[NumQst][0])		// If user has answered the question
     {
      if (sscanf (Result->StrAnswers[NumQst],"%ld",&IntAnswerUsr) == 1)
	{
         HTM_TD_Begin ("class=\"%s CM\"",
		       TsV_IsVisibleCorrectAns (Visibility) ?
			  (IntAnswerUsr == IntAnswerCorr ? "ANS_OK" :
							   "ANS_BAD") :
			  "ANS_0");
         HTM_Long (IntAnswerUsr);
         HTM_TD_End ();
	}
      else
        {
         HTM_TD_Begin ("class=\"ANS_0 CM\"");
         HTM_Txt ("?");
         HTM_TD_End ();
        }
     }
   else							// If user has omitted the answer
      HTM_TD_Empty (1);

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"ANS_0 CM\"");
   if (TsV_IsVisibleQstAndAnsTxt (Visibility) &&
       TsV_IsVisibleCorrectAns   (Visibility))
      HTM_Long (IntAnswerCorr);
   else
      Ico_PutIconNotVisible ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Compute the score *****/
   if (!Result->StrAnswers[NumQst][0])	// If user has omitted the answer
     {
      *AnswerIsNotBlank = false;
      *ScoreThisQst = 0.0;
     }
   else
     {
      *AnswerIsNotBlank = true;
      if (IntAnswerUsr == IntAnswerCorr)	// If correct
         *ScoreThisQst = 1.0;
      else					// If wrong
         *ScoreThisQst = 0.0;
     }

   /***** Write the score of this question *****/
   if (TsV_IsVisibleEachQstScore (Visibility))
     {
      Tst_WriteScoreStart (2);
      if (!Result->StrAnswers[NumQst][0])	// If user has omitted the answer
	{
         HTM_SPAN_Begin ("class=\"ANS_0\"");
         HTM_Double2Decimals (0.0);
	}
      else if (IntAnswerUsr == IntAnswerCorr)		// If correct
	{
         HTM_SPAN_Begin ("class=\"ANS_OK\"");
         HTM_Double2Decimals (1.0);
	}
      else						// If wrong
	{
         HTM_SPAN_Begin ("class=\"ANS_BAD\"");
         HTM_Double2Decimals (0.0);
	}
      HTM_SPAN_End ();
      Tst_WriteScoreEnd ();
     }

   HTM_TABLE_End ();
  }


/*****************************************************************************/
/****************** Write float answer when editing a test *******************/
/*****************************************************************************/

static void Tst_WriteFloatAnsEdit (const struct Tst_Question *Question,
                                   MYSQL_RES *mysql_res)
  {
   MYSQL_ROW row;
   unsigned i;
   double FloatNum[2];
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */

   if (Question->Answer.NumOptions != 2)
      Lay_ShowErrorAndExit ("Wrong float range.");

   for (i = 0;
	i < 2;
	i++)
     {
      row = mysql_fetch_row (mysql_res);
      FloatNum[i] = Str_GetDoubleFromStr (row[1]);
     }
   HTM_SPAN_Begin ("class=\"TEST_EDI\"");
   HTM_Txt ("([");
   HTM_Double (FloatNum[0]);
   HTM_Txt ("; ");
   HTM_Double (FloatNum[1]);
   HTM_Txt ("])");
   HTM_SPAN_End ();
  }

/*****************************************************************************/
/****************** Write float answer when seeing a test ********************/
/*****************************************************************************/

static void Tst_WriteFloatAnsSeeing (unsigned NumQst)
  {
   char StrQstIndOrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Qstxx...x", "Indxx...x" or "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (StrQstIndOrAns,sizeof (StrQstIndOrAns),
	     "Ans%010u",
	     NumQst);
   HTM_INPUT_TEXT (StrQstIndOrAns,Tst_MAX_BYTES_FLOAT_ANSWER,"",false,
		   "size=\"11\"");
  }

/*****************************************************************************/
/***************** Write float answer when assessing a test ******************/
/*****************************************************************************/

static void Tst_WriteFloatAnsResult (struct UsrData *UsrDat,
                                     const struct TsR_Result *Result,
				     unsigned NumQst,
				     const struct Tst_Question *Question,
				     MYSQL_RES *mysql_res,
				     unsigned Visibility,
                                     double *ScoreThisQst,
                                     bool *AnswerIsNotBlank)
  {
   MYSQL_ROW row;
   unsigned i;
   double FloatAnsUsr = 0.0,Tmp;
   double FloatAnsCorr[2];
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */
   /***** Check if number of rows is correct *****/
   if (Question->Answer.NumOptions != 2)
      Lay_ShowErrorAndExit ("Wrong float range.");

   /***** Get the numerical value of the minimum and maximum correct answers *****/
   for (i = 0;
	i < 2;
	i++)
     {
      row = mysql_fetch_row (mysql_res);
      FloatAnsCorr[i] = Str_GetDoubleFromStr (row[1]);
     }
   if (FloatAnsCorr[0] > FloatAnsCorr[1]) 	// The maximum and the minimum are swapped
    {
      /* Swap maximum and minimum */
      Tmp = FloatAnsCorr[0];
      FloatAnsCorr[0] = FloatAnsCorr[1];
      FloatAnsCorr[1] = Tmp;
     }

   /***** Header with the title of each column *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   Tst_WriteHeadUserCorrect (UsrDat);
   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Write the user answer *****/
   if (Result->StrAnswers[NumQst][0])	// If user has answered the question
     {
      FloatAnsUsr = Str_GetDoubleFromStr (Result->StrAnswers[NumQst]);
      if (Result->StrAnswers[NumQst][0])	// It's a correct floating point number
        {
         HTM_TD_Begin ("class=\"%s CM\"",
		       TsV_IsVisibleCorrectAns (Visibility) ?
			  ((FloatAnsUsr >= FloatAnsCorr[0] &&
			    FloatAnsUsr <= FloatAnsCorr[1]) ? "ANS_OK" :
							      "ANS_BAD") :
			  "ANS_0");
         HTM_Double (FloatAnsUsr);
        }
      else				// Not a floating point number
	{
         HTM_TD_Begin ("class=\"ANS_0 CM\"");
         HTM_Txt ("?");
	}
     }
   else					// If user has omitted the answer
      HTM_TD_Begin (NULL);
   HTM_TD_End ();

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"ANS_0 CM\"");
   if (TsV_IsVisibleQstAndAnsTxt (Visibility) &&
       TsV_IsVisibleCorrectAns   (Visibility))
     {
      HTM_Txt ("[");
      HTM_Double (FloatAnsCorr[0]);
      HTM_Txt ("; ");
      HTM_Double (FloatAnsCorr[1]);
      HTM_Txt ("]");
     }
   else
      Ico_PutIconNotVisible ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Compute mark *****/
   if (!Result->StrAnswers[NumQst][0])	// If user has omitted the answer
     {
      *AnswerIsNotBlank = false;
      *ScoreThisQst = 0.0;
     }
   else
     {
      *AnswerIsNotBlank = true;
      if (FloatAnsUsr >= FloatAnsCorr[0] &&
          FloatAnsUsr <= FloatAnsCorr[1])	// If correct (inside the interval)
         *ScoreThisQst = 1.0;
      else						// If wrong (outside the interval)
         *ScoreThisQst = 0.0;
     }

   /***** Write the score of this question *****/
   if (TsV_IsVisibleEachQstScore (Visibility))
     {
      Tst_WriteScoreStart (2);
      if (!Result->StrAnswers[NumQst][0])	// If user has omitted the answer
	{
         HTM_SPAN_Begin ("class=\"ANS_0\"");
         HTM_Double2Decimals (0.0);
	}
      else if (FloatAnsUsr >= FloatAnsCorr[0] &&
               FloatAnsUsr <= FloatAnsCorr[1])		// If correct (inside the interval)
	{
         HTM_SPAN_Begin ("class=\"ANS_OK\"");
         HTM_Double2Decimals (1.0);
	}
      else						// If wrong (outside the interval)
	{
         HTM_SPAN_Begin ("class=\"ANS_BAD\"");
         HTM_Double2Decimals (0.0);
	}
      HTM_SPAN_End ();
      Tst_WriteScoreEnd ();
     }

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********* Write head with two columns:                               ********/
/********* one for the user's answer and other for the correct answer ********/
/*****************************************************************************/

static void Tst_WriteHeadUserCorrect (struct UsrData *UsrDat)
  {
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   HTM_TD_Begin ("class=\"DAT_SMALL CM\"");
   HTM_Txt (Txt_User[UsrDat->Sex]);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT_SMALL CM\"");
   HTM_Txt (Txt_ROLES_PLURAL_Abc[Rol_TCH][Usr_SEX_UNKNOWN]);
   HTM_TD_End ();
  }

/*****************************************************************************/
/*********** Write the start ans the end of the score of an answer ***********/
/*****************************************************************************/

static void Tst_WriteScoreStart (unsigned ColSpan)
  {
   extern const char *Txt_Score;

   HTM_TR_Begin (NULL);
   HTM_TD_Begin ("colspan=\"%u\" class=\"DAT_SMALL LM\"",ColSpan);
   HTM_TxtColonNBSP (Txt_Score);
  }

static void Tst_WriteScoreEnd (void)
  {
   HTM_TD_End ();
   HTM_TR_End ();
  }

/*****************************************************************************/
/*************** Write parameter with the code of a question *****************/
/*****************************************************************************/

static void Tst_WriteParamQstCod (unsigned NumQst,long QstCod)
  {
   char StrQstIndOrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Qstxx...x", "Indxx...x" or "Ansxx...x"

   snprintf (StrQstIndOrAns,sizeof (StrQstIndOrAns),
	     "Qst%010u",
	     NumQst);
   Par_PutHiddenParamLong (NULL,StrQstIndOrAns,QstCod);
  }

/*****************************************************************************/
/********************* Check if number of answers is one *********************/
/*****************************************************************************/

void Tst_CheckIfNumberOfAnswersIsOne (const struct Tst_Question *Question)
  {
   if (Question->Answer.NumOptions != 1)
      Lay_ShowErrorAndExit ("Wrong answer.");
  }

/*****************************************************************************/
/************************* Get tags of a test question ***********************/
/*****************************************************************************/

unsigned long Tst_GetTagsQst (long QstCod,MYSQL_RES **mysql_res)
  {
   /***** Get the tags of a question from database *****/
   return DB_QuerySELECT (mysql_res,"can not get the tags of a question",
			  "SELECT tst_tags.TagTxt FROM tst_question_tags,tst_tags"
			  " WHERE tst_question_tags.QstCod=%ld"
			  " AND tst_question_tags.TagCod=tst_tags.TagCod"
			  " AND tst_tags.CrsCod=%ld"
			  " ORDER BY tst_question_tags.TagInd",
			  QstCod,Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/******************** Get and write tags of a test question ******************/
/*****************************************************************************/

void Tst_GetAndWriteTagsQst (long QstCod)
  {
   extern const char *Txt_no_tags;
   unsigned long NumRow;
   unsigned long NumRows;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if ((NumRows = Tst_GetTagsQst (QstCod,&mysql_res)))	// Result: TagTxt
     {
      /***** Write the tags *****/
      HTM_UL_Begin ("class=\"TEST_TAG_LIST DAT_SMALL\"");
      for (NumRow = 0;
	   NumRow < NumRows;
	   NumRow++)
        {
         row = mysql_fetch_row (mysql_res);
         HTM_LI_Begin (NULL);
         HTM_Txt (row[0]);
         HTM_LI_End ();
        }
      HTM_UL_End ();
     }
   else
     {
      HTM_SPAN_Begin ("class=\"DAT_SMALL\"");
      HTM_TxtF ("(%s)",Txt_no_tags);
      HTM_SPAN_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Get parameters for the selection of test questions *************/
/*****************************************************************************/
// Return true (OK) if all parameters are found, or false (error) if any necessary parameter is not found

static bool Tst_GetParamsTst (struct Tst_Test *Test,
                              Tst_ActionToDoWithQuestions_t ActionToDoWithQuestions)
  {
   extern const char *Txt_You_must_select_one_ore_more_tags;
   extern const char *Txt_You_must_select_one_ore_more_types_of_answer;
   extern const char *Txt_The_number_of_questions_must_be_in_the_interval_X;
   bool Error = false;
   char UnsignedStr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   unsigned UnsignedNum;

   /***** Tags *****/
   /* Get parameter that indicates whether all tags are selected */
   Test->Tags.All = Par_GetParToBool ("AllTags");

   /* Get the tags */
   if ((Test->Tags.List = (char *) malloc (Tst_MAX_BYTES_TAGS_LIST + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();
   Par_GetParMultiToText ("ChkTag",Test->Tags.List,Tst_MAX_BYTES_TAGS_LIST);

   /* Check number of tags selected */
   if (Tst_CountNumTagsInList (&Test->Tags) == 0)	// If no tags selected...
     {						// ...write alert
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_select_one_ore_more_tags);
      Error = true;
     }

   /***** Types of answer *****/
   switch (ActionToDoWithQuestions)
     {
      case Tst_SHOW_TEST_TO_ANSWER:
      case Tst_EDIT_TEST:
	 /* Get parameter that indicates if all types of answer are selected */
	 Test->AnswerTypes.All = Par_GetParToBool ("AllAnsTypes");

	 /* Get types of answer */
	 Par_GetParMultiToText ("AnswerType",Test->AnswerTypes.List,Tst_MAX_BYTES_LIST_ANSWER_TYPES);

	 /* Check number of types of answer */
	 if (Tst_CountNumAnswerTypesInList (&Test->AnswerTypes) == 0)	// If no types of answer selected...
	   {							// ...write warning alert
	    Ale_ShowAlert (Ale_WARNING,Txt_You_must_select_one_ore_more_types_of_answer);
	    Error = true;
	   }
	 break;
      case Tst_SELECT_QUESTIONS_FOR_GAME:
	 /* The unique allowed type of answer in a game is unique choice */
	 Test->AnswerTypes.All = false;
	 snprintf (Test->AnswerTypes.List,sizeof (Test->AnswerTypes.List),
	           "%u",
		   (unsigned) Tst_ANS_UNIQUE_CHOICE);
	 break;
      default:
	 break;
     }

   /***** Get other parameters, depending on action *****/
   switch (ActionToDoWithQuestions)
     {
      case Tst_SHOW_TEST_TO_ANSWER:
	 Test->NumQsts = Tst_GetParamNumQsts ();
	 if (Test->NumQsts < TstCfg_GetConfigMin () ||
	     Test->NumQsts > TstCfg_GetConfigMax ())
	   {
	    Ale_ShowAlert (Ale_WARNING,Txt_The_number_of_questions_must_be_in_the_interval_X,
		           TstCfg_GetConfigMin (),TstCfg_GetConfigMax ());
	    Error = true;
	   }
	 break;
      case Tst_EDIT_TEST:
	 /* Get starting and ending dates */
	 Dat_GetIniEndDatesFromForm ();

	 /* Get ordering criteria */
	 Par_GetParMultiToText ("Order",UnsignedStr,Cns_MAX_DECIMAL_DIGITS_UINT);
	 if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
	    Test->SelectedOrder = (Tst_QuestionsOrder_t)
	                          ((UnsignedNum < Tst_NUM_TYPES_ORDER_QST) ? UnsignedNum :
									     0);
	 else
	    Test->SelectedOrder = (Tst_QuestionsOrder_t) 0;
	 break;
      case Tst_SELECT_QUESTIONS_FOR_GAME:
	 /* Get starting and ending dates */
	 Dat_GetIniEndDatesFromForm ();

	 /* Order question by stem */
	 Test->SelectedOrder = Tst_ORDER_STEM;
	 break;
      default:
	 break;
     }

   return !Error;
  }

/*****************************************************************************/
/******************** Get parameter with the number of test ******************/
/*****************************************************************************/

static unsigned Tst_GetAndCheckParamNumTst (void)
  {
   return (unsigned) Par_GetParToUnsignedLong ("NumTst",
                                               1,
                                               UINT_MAX,
                                               1);
  }

/*****************************************************************************/
/***** Get parameter with the number of questions to generate in an test *****/
/*****************************************************************************/

static unsigned Tst_GetParamNumQsts (void)
  {
   return (unsigned) Par_GetParToUnsignedLong ("NumQst",
	                                       (unsigned long) TstCfg_GetConfigMin (),
	                                       (unsigned long) TstCfg_GetConfigMax (),
	                                       (unsigned long) TstCfg_GetConfigDef ());
  }

/*****************************************************************************/
/***************** Count number of tags in the list of tags ******************/
/*****************************************************************************/

static unsigned Tst_CountNumTagsInList (const struct Tst_Tags *Tags)
  {
   const char *Ptr;
   unsigned NumTags = 0;
   char TagText[Tst_MAX_BYTES_TAG + 1];

   /***** Go over the list of tags counting the number of tags *****/
   Ptr = Tags->List;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,TagText,Tst_MAX_BYTES_TAG);
      NumTags++;
     }

   return NumTags;
  }

/*****************************************************************************/
/**** Count the number of types of answers in the list of types of answers ***/
/*****************************************************************************/

static int Tst_CountNumAnswerTypesInList (const struct Tst_AnswerTypes *AnswerTypes)
  {
   const char *Ptr;
   int NumAnsTypes = 0;
   char UnsignedStr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Go over the list of answer types counting the number of types of answer *****/
   Ptr = AnswerTypes->List;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,Cns_MAX_DECIMAL_DIGITS_UINT);
      Tst_ConvertFromUnsignedStrToAnsTyp (UnsignedStr);
      NumAnsTypes++;
     }
   return NumAnsTypes;
  }

/*****************************************************************************/
/******************** Show form to edit one test question ********************/
/*****************************************************************************/

void Tst_ShowFormEditOneQst (void)
  {
   struct Tst_Question Question;
   char Stem[Cns_MAX_BYTES_TEXT + 1];
   char Feedback[Cns_MAX_BYTES_TEXT + 1];

   /***** Create test question *****/
   Tst_QstConstructor (&Question);

   /***** Get question data *****/
   Question.QstCod = Tst_GetQstCod ();
   Stem[0] = Feedback[0] = '\0';
   if (Question.QstCod > 0)	// If question already exists in the database
      Tst_GetQstDataFromDB (&Question,Stem,Feedback);

   /***** Put form to edit question *****/
   Tst_PutFormEditOneQst (&Question,Stem,Feedback);

   /***** Destroy test question *****/
   Tst_QstDestructor (&Question);
  }

/*****************************************************************************/
/******************** Show form to edit one test question ********************/
/*****************************************************************************/

// This function may be called from three places:
// 1. By clicking "New question" icon
// 2. By clicking "Edit" icon in a listing of existing questions
// 3. From the action associated to reception of a question, on error in the parameters received from the form

static void Tst_PutFormEditOneQst (struct Tst_Question *Question,
	                           char Stem[Cns_MAX_BYTES_TEXT + 1],
                                   char Feedback[Cns_MAX_BYTES_TEXT + 1])
  {
   extern const char *Hlp_ASSESSMENT_Tests_writing_a_question;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Question_code_X;
   extern const char *Txt_New_question;
   extern const char *Txt_Tags;
   extern const char *Txt_new_tag;
   extern const char *Txt_Wording;
   extern const char *Txt_Feedback;
   extern const char *Txt_optional;
   extern const char *Txt_Type;
   extern const char *Txt_TST_STR_ANSWER_TYPES[Tst_NUM_ANS_TYPES];
   extern const char *Txt_Answers;
   extern const char *Txt_Integer_number;
   extern const char *Txt_Real_number_between_A_and_B_1;
   extern const char *Txt_Real_number_between_A_and_B_2;
   extern const char *Txt_TF_QST[2];
   extern const char *Txt_Shuffle;
   extern const char *Txt_Expand;
   extern const char *Txt_Contract;
   extern const char *Txt_Save_changes;
   extern const char *Txt_Create_question;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumTags;
   unsigned IndTag;
   unsigned NumTag;
   unsigned NumOpt;
   Tst_AnswerType_t AnsType;
   bool IsThisTag;
   bool TagFound;
   bool OptionsDisabled;
   bool AnswerHasContent;
   bool DisplayRightColumn;
   char StrTagTxt[6 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   char StrInteger[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   char *Title;

   /***** Begin box *****/
   if (Question->QstCod > 0)	// The question already has assigned a code
     {
      Box_BoxBegin (NULL,Str_BuildStringLong (Txt_Question_code_X,Question->QstCod),
		    Tst_PutIconToRemoveOneQst,&Question->QstCod,
                    Hlp_ASSESSMENT_Tests_writing_a_question,Box_NOT_CLOSABLE);
      Str_FreeString ();
     }
   else
      Box_BoxBegin (NULL,Txt_New_question,
                    NULL,NULL,
                    Hlp_ASSESSMENT_Tests_writing_a_question,Box_NOT_CLOSABLE);

   /***** Begin form *****/
   Frm_StartForm (ActRcvTstQst);
   Tst_PutParamQstCod (&Question->QstCod);

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);	// Table for this question

   /***** Help for text editor *****/
   HTM_TR_Begin (NULL);
   HTM_TD_Begin ("colspan=\"2\"");
   Lay_HelpPlainEditor ();
   HTM_TD_End ();
   HTM_TR_End ();

   /***** Get tags already existing for questions in current course *****/
   NumTags = Tst_GetAllTagsFromCurrentCrs (&mysql_res);

   /***** Write the tags *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RT %s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Tags);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LT\"");
   HTM_TABLE_BeginPadding (2);	// Table for tags

   for (IndTag = 0;
	IndTag < Tst_MAX_TAGS_PER_QUESTION;
	IndTag++)
     {
      HTM_TR_Begin (NULL);

      /***** Write the tags already existing in a selector *****/
      HTM_TD_Begin ("class=\"LM\"");
      HTM_SELECT_Begin (false,
			"id=\"SelTag%u\" name=\"SelTag%u\""
	                " class=\"TAG_SEL\" onchange=\"changeTxtTag('%u')\"",
                        IndTag,IndTag,IndTag);
      HTM_OPTION (HTM_Type_STRING,"",false,false,"&nbsp;");
      mysql_data_seek (mysql_res,0);
      TagFound = false;
      for (NumTag = 1;
	   NumTag <= NumTags;
	   NumTag++)
        {
         row = mysql_fetch_row (mysql_res);
	 /*
	 row[0] TagCod
	 row[1] TagTxt
	 row[2] TagHidden
	 */
         IsThisTag = false;
         if (!strcasecmp (Question->Tags.Txt[IndTag],row[1]))
           {
  	    HTM_Txt (" selected=\"selected\"");
  	    IsThisTag = true;
            TagFound = true;
           }
         HTM_OPTION (HTM_Type_STRING,row[1],
		     IsThisTag,false,
		     "%s",row[1]);
        }
      /* If it's a new tag received from the form */
      if (!TagFound && Question->Tags.Txt[IndTag][0])
         HTM_OPTION (HTM_Type_STRING,Question->Tags.Txt[IndTag],
		     true,false,
		     "%s",Question->Tags.Txt[IndTag]);
      HTM_OPTION (HTM_Type_STRING,"",
		  false,false,
		  "[%s]",Txt_new_tag);
      HTM_SELECT_End ();
      HTM_TD_End ();

      /***** Input of a new tag *****/
      HTM_TD_Begin ("class=\"RM\"");
      snprintf (StrTagTxt,sizeof (StrTagTxt),
		"TagTxt%u",
		IndTag);
      HTM_INPUT_TEXT (StrTagTxt,Tst_MAX_CHARS_TAG,Question->Tags.Txt[IndTag],false,
		      "id=\"%s\" class=\"TAG_TXT\" onchange=\"changeSelTag('%u')\"",
	              StrTagTxt,IndTag);
      HTM_TD_End ();

      HTM_TR_End ();
     }

   HTM_TABLE_End ();	// Table for tags
   HTM_TD_End ();

   HTM_TR_End ();

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Stem and image *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","Stem",Txt_Wording);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   HTM_TEXTAREA_Begin ("id=\"Stem\" name=\"Stem\" class=\"STEM_TEXTAREA\""
	               " rows=\"5\" required=\"required\"");
   HTM_Txt (Stem);
   HTM_TEXTAREA_End ();
   HTM_BR ();
   Tst_PutFormToEditQstMedia (&Question->Media,-1,
                              false);

   /***** Feedback *****/
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s&nbsp;(%s):",Txt_Feedback,Txt_optional);
   HTM_BR ();
   HTM_TEXTAREA_Begin ("name=\"Feedback\" class=\"STEM_TEXTAREA\" rows=\"2\"");
   if (Feedback)
      if (Feedback[0])
	 HTM_Txt (Feedback);
   HTM_TEXTAREA_End ();
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Type of answer *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RT %s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Type);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"%s LT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   for (AnsType  = (Tst_AnswerType_t) 0;
	AnsType <= (Tst_AnswerType_t) (Tst_NUM_ANS_TYPES - 1);
	AnsType++)
     {
      HTM_LABEL_Begin (NULL);
      HTM_INPUT_RADIO ("AnswerType",false,
		       "value=\"%u\"%s onclick=\"enableDisableAns(this.form);\"",
		       (unsigned) AnsType,
		       AnsType == Question->Answer.Type ? " checked=\"checked\"" :
				                          "");
      HTM_TxtF ("%s&nbsp;",Txt_TST_STR_ANSWER_TYPES[AnsType]);
      HTM_LABEL_End ();
      HTM_BR ();
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Answers *****/
   /* Integer answer */
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RT %s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Answers);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LT\"");
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtColonNBSP (Txt_Integer_number);
   snprintf (StrInteger,sizeof (StrInteger),
	     "%ld",
	     Question->Answer.Integer);
   HTM_INPUT_TEXT ("AnsInt",Cns_MAX_DECIMAL_DIGITS_LONG,StrInteger,false,
		   "size=\"11\" required=\"required\"%s",
                   Question->Answer.Type == Tst_ANS_INT ? "" :
                                                          " disabled=\"disabled\"");
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /* Floating point answer */
   HTM_TR_Begin (NULL);
   HTM_TD_Empty (1);
   HTM_TD_Begin ("class=\"LT\"");
   Tst_PutFloatInputField (Txt_Real_number_between_A_and_B_1,"AnsFloatMin",
                           Question,0);
   Tst_PutFloatInputField (Txt_Real_number_between_A_and_B_2,"AnsFloatMax",
                           Question,1);
   HTM_TD_End ();
   HTM_TR_End ();

   /* T/F answer */
   HTM_TR_Begin (NULL);
   HTM_TD_Empty (1);
   HTM_TD_Begin ("class=\"LT\"");
   Tst_PutTFInputField (Question,Txt_TF_QST[0],'T');
   Tst_PutTFInputField (Question,Txt_TF_QST[1],'F');
   HTM_TD_End ();
   HTM_TR_End ();

   /* Questions can be shuffled? */
   HTM_TR_Begin (NULL);

   HTM_TD_Empty (1);

   HTM_TD_Begin ("class=\"LT\"");
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_INPUT_CHECKBOX ("Shuffle",HTM_DONT_SUBMIT_ON_CHANGE,
		       "value=\"Y\"%s%s",
		       Question->Shuffle ? " checked=\"checked\"" :
				           "",
   		       Question->Answer.Type != Tst_ANS_UNIQUE_CHOICE &&
                       Question->Answer.Type != Tst_ANS_MULTIPLE_CHOICE ? " disabled=\"disabled\"" :
                	                                                  "");
   HTM_Txt (Txt_Shuffle);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /* Simple or multiple choice answers */
   HTM_TR_Begin (NULL);
   HTM_TD_Empty (1);
   HTM_TD_Begin ("class=\"LT\"");
   HTM_TABLE_BeginPadding (2);	// Table with choice answers

   OptionsDisabled = Question->Answer.Type != Tst_ANS_UNIQUE_CHOICE &&
                     Question->Answer.Type != Tst_ANS_MULTIPLE_CHOICE &&
	             Question->Answer.Type != Tst_ANS_TEXT;
   for (NumOpt = 0;
	NumOpt < Tst_MAX_OPTIONS_PER_QUESTION;
	NumOpt++)
     {
      Gbl.RowEvenOdd = NumOpt % 2;

      AnswerHasContent = false;
      if (Question->Answer.Options[NumOpt].Text)
	 if (Question->Answer.Options[NumOpt].Text[0] ||			// Text
	     Question->Answer.Options[NumOpt].Media.Type != Med_TYPE_NONE)	// or media
	    AnswerHasContent = true;
      DisplayRightColumn = NumOpt < 2 ||	// Display at least the two first options
	                   AnswerHasContent;

      HTM_TR_Begin (NULL);

      /***** Left column: selectors *****/
      HTM_TD_Begin ("class=\"TEST_EDI_ANS_LEFT_COL COLOR%u\"",Gbl.RowEvenOdd);

      /* Radio selector for unique choice answers */
      HTM_INPUT_RADIO ("AnsUni",false,
		       "value=\"%u\"%s%s%s onclick=\"enableDisableAns(this.form);\"",
		       NumOpt,
		       Question->Answer.Options[NumOpt].Correct ? " checked=\"checked\"" :
			                                          "",
		       NumOpt < 2 ? " required=\"required\"" :	// First or second options required
				    "",
		       Question->Answer.Type == Tst_ANS_UNIQUE_CHOICE ? "" :
				                                        " disabled=\"disabled\"");

      /* Checkbox for multiple choice answers */
      HTM_INPUT_CHECKBOX ("AnsMulti",HTM_DONT_SUBMIT_ON_CHANGE,
			  "value=\"%u\"%s%s",
			  NumOpt,
			  Question->Answer.Options[NumOpt].Correct ? " checked=\"checked\"" :
				                                     "",
			  Question->Answer.Type == Tst_ANS_MULTIPLE_CHOICE ? "" :
				                                             " disabled=\"disabled\"");

      HTM_TD_End ();

      /***** Center column: letter of the answer and expand / contract icon *****/
      HTM_TD_Begin ("class=\"%s TEST_EDI_ANS_CENTER_COL COLOR%u\"",
	            The_ClassFormInBox[Gbl.Prefs.Theme],Gbl.RowEvenOdd);
      HTM_TxtF ("%c)",'a' + (char) NumOpt);

      /* Icon to expand (show the answer) */
      HTM_A_Begin ("href=\"\" id=\"exp_%u\"%s"
	           " onclick=\"toggleAnswer('%u');return false;\"",
                   NumOpt,
		   DisplayRightColumn ?	" style=\"display:none;\"" :	// Answer does have content ==> Hide icon
	                                "",
	           NumOpt);
      if (asprintf (&Title,"%s %c)",Txt_Expand,'a' + (char) NumOpt) < 0)
	 Lay_NotEnoughMemoryExit ();
      Ico_PutIcon ("caret-right.svg",Title,"ICO16x16");
      free (Title);
      HTM_A_End ();

      /* Icon to contract (hide the answer) */
      HTM_A_Begin ("href=\"\" id=\"con_%u\"%s"
	           " onclick=\"toggleAnswer(%u);return false;\"",
		   NumOpt,
                   DisplayRightColumn ? "" :
	                                " style=\"display:none;\"",	// Answer does not have content ==> Hide icon
                   NumOpt);
      if (asprintf (&Title,"%s %c)",Txt_Contract,'a' + (char) NumOpt) < 0)
	 Lay_NotEnoughMemoryExit ();
      Ico_PutIcon ("caret-down.svg",Title,"ICO16x16");
      free (Title);
      HTM_A_End ();

      HTM_TD_End ();

      /***** Right column: content of the answer *****/
      HTM_TD_Begin ("class=\"TEST_EDI_ANS_RIGHT_COL COLOR%u\"",Gbl.RowEvenOdd);
      HTM_DIV_Begin ("id=\"ans_%u\"%s",
		     NumOpt,
		     DisplayRightColumn ? "" :
	                                  " style=\"display:none;\"");	// Answer does not have content ==> Hide column

      /* Answer text */
      HTM_TEXTAREA_Begin ("name=\"AnsStr%u\" class=\"ANSWER_TEXTAREA\" rows=\"5\"%s",
	                  NumOpt,OptionsDisabled ? " disabled=\"disabled\"" :
	                	                   "");
      if (AnswerHasContent)
         HTM_Txt (Question->Answer.Options[NumOpt].Text);
      HTM_TEXTAREA_End ();

      /* Media */
      Tst_PutFormToEditQstMedia (&Question->Answer.Options[NumOpt].Media,
                                 (int) NumOpt,
                                 OptionsDisabled);

      /* Feedback */
      HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_TxtF ("%s&nbsp;(%s):",Txt_Feedback,Txt_optional);
      HTM_BR ();
      HTM_TEXTAREA_Begin ("name=\"FbStr%u\" class=\"ANSWER_TEXTAREA\" rows=\"2\"%s",
			  NumOpt,OptionsDisabled ? " disabled=\"disabled\"" :
				                   "");
      if (Question->Answer.Options[NumOpt].Feedback)
         if (Question->Answer.Options[NumOpt].Feedback[0])
            HTM_Txt (Question->Answer.Options[NumOpt].Feedback);
      HTM_TEXTAREA_End ();
      HTM_LABEL_End ();

      /* End of right column */
      HTM_DIV_End ();
      HTM_TD_End ();

      HTM_TR_End ();
     }
   HTM_TABLE_End ();	// Table with choice answers
   HTM_TD_End ();
   HTM_TR_End ();

   /***** End table *****/
   HTM_TABLE_End ();	// Table for this question

   /***** Send button *****/
   if (Question->QstCod > 0)	// The question already has assigned a code
      Btn_PutConfirmButton (Txt_Save_changes);
   else
      Btn_PutCreateButton (Txt_Create_question);

   /***** End form *****/
   Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* Put input field for floating answer *******************/
/*****************************************************************************/

static void Tst_PutFloatInputField (const char *Label,const char *Field,
                                    const struct Tst_Question *Question,
                                    unsigned Index)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   char StrDouble[32];

   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s&nbsp;",Label);
   snprintf (StrDouble,sizeof (StrDouble),
	     "%.15lg",
	     Question->Answer.FloatingPoint[Index]);
   HTM_INPUT_TEXT (Field,Tst_MAX_BYTES_FLOAT_ANSWER,StrDouble,false,
		   "size=\"11\" required=\"required\"%s",
                   Question->Answer.Type == Tst_ANS_FLOAT ? "" :
                                                            " disabled=\"disabled\"");
   HTM_LABEL_End ();
  }

/*****************************************************************************/
/*********************** Put input field for T/F answer **********************/
/*****************************************************************************/

static void Tst_PutTFInputField (const struct Tst_Question *Question,
                                 const char *Label,char Value)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];

   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_INPUT_RADIO ("AnsTF",false,
		    "value=\"%c\"%s%s required=\"required\"",
		    Value,
		    Question->Answer.TF == Value ? " checked=\"checked\"" :
			                           "",
		    Question->Answer.Type == Tst_ANS_TRUE_FALSE ? "" :
			                                          " disabled=\"disabled\"");
   HTM_Txt (Label);
   HTM_LABEL_End ();
  }

/*****************************************************************************/
/********************* Initialize a new question to zero *********************/
/*****************************************************************************/

void Tst_QstConstructor (struct Tst_Question *Question)
  {
   unsigned NumOpt;

   Tst_ResetTags (&Question->Tags);

   Question->Stem.Text = NULL;
   Question->Stem.Length = 0;
   Question->Feedback.Text = NULL;
   Question->Feedback.Length = 0;
   Question->Shuffle = false;

   Question->Answer.Type = Tst_ANS_UNIQUE_CHOICE;
   Question->Answer.NumOptions = 0;
   Question->Answer.TF = ' ';

   /***** Initialize image attached to stem *****/
   Med_MediaConstructor (&Question->Media);

   for (NumOpt = 0;
	NumOpt < Tst_MAX_OPTIONS_PER_QUESTION;
	NumOpt++)
     {
      Question->Answer.Options[NumOpt].Correct  = false;
      Question->Answer.Options[NumOpt].Text     = NULL;
      Question->Answer.Options[NumOpt].Feedback = NULL;

      /***** Initialize image attached to option *****/
      Med_MediaConstructor (&Question->Answer.Options[NumOpt].Media);
     }
   Question->Answer.Integer = 0;
   Question->Answer.FloatingPoint[0] =
   Question->Answer.FloatingPoint[1] = 0.0;
  }

/*****************************************************************************/
/***************** Free memory allocated for test question *******************/
/*****************************************************************************/

void Tst_QstDestructor (struct Tst_Question *Question)
  {
   Tst_FreeTextChoiceAnswers (Question);
   Tst_FreeMediaOfQuestion (Question);
  }

/*****************************************************************************/
/******************* Allocate memory for a choice answer *********************/
/*****************************************************************************/
// Return false on error

bool Tst_AllocateTextChoiceAnswer (struct Tst_Question *Question,unsigned NumOpt)
  {
   // Tst_FreeTagsList (&Question->Tags);	// TODO: Necessary?

   Tst_FreeTextChoiceAnswer (Question,NumOpt);

   if ((Question->Answer.Options[NumOpt].Text =
	(char *) malloc (Tst_MAX_BYTES_ANSWER_OR_FEEDBACK + 1)) == NULL)
     {
      Ale_CreateAlert (Ale_ERROR,NULL,
		       "Not enough memory to store answer.");
      return false;
     }
   if ((Question->Answer.Options[NumOpt].Feedback =
	(char *) malloc (Tst_MAX_BYTES_ANSWER_OR_FEEDBACK + 1)) == NULL)
     {
      Ale_CreateAlert (Ale_ERROR,NULL,
		       "Not enough memory to store feedback.");
      return false;
     }

   Question->Answer.Options[NumOpt].Text[0] =
   Question->Answer.Options[NumOpt].Feedback[0] = '\0';
   return true;
  }

/*****************************************************************************/
/******************** Free memory of all choice answers **********************/
/*****************************************************************************/

static void Tst_FreeTextChoiceAnswers (struct Tst_Question *Question)
  {
   unsigned NumOpt;

   for (NumOpt = 0;
	NumOpt < Tst_MAX_OPTIONS_PER_QUESTION;
	NumOpt++)
      Tst_FreeTextChoiceAnswer (Question,NumOpt);
  }

/*****************************************************************************/
/********************** Free memory of a choice answer ***********************/
/*****************************************************************************/

static void Tst_FreeTextChoiceAnswer (struct Tst_Question *Question,unsigned NumOpt)
  {
   if (Question->Answer.Options[NumOpt].Text)
     {
      free (Question->Answer.Options[NumOpt].Text);
      Question->Answer.Options[NumOpt].Text = NULL;
     }
   if (Question->Answer.Options[NumOpt].Feedback)
     {
      free (Question->Answer.Options[NumOpt].Feedback);
      Question->Answer.Options[NumOpt].Feedback = NULL;
     }
  }

/*****************************************************************************/
/***************** Initialize images of a question to zero *******************/
/*****************************************************************************/

static void Tst_ResetMediaOfQuestion (struct Tst_Question *Question)
  {
   unsigned NumOpt;

   /***** Reset media for stem *****/
   Med_ResetMedia (&Question->Media);

   /***** Reset media for every answer option *****/
   for (NumOpt = 0;
	NumOpt < Tst_MAX_OPTIONS_PER_QUESTION;
	NumOpt++)
      Med_ResetMedia (&Question->Answer.Options[NumOpt].Media);
  }

/*****************************************************************************/
/*********************** Free images of a question ***************************/
/*****************************************************************************/

static void Tst_FreeMediaOfQuestion (struct Tst_Question *Question)
  {
   unsigned NumOpt;

   Med_MediaDestructor (&Question->Media);
   for (NumOpt = 0;
	NumOpt < Tst_MAX_OPTIONS_PER_QUESTION;
	NumOpt++)
      Med_MediaDestructor (&Question->Answer.Options[NumOpt].Media);
  }

/*****************************************************************************/
/****************** Get data of a question from database *********************/
/*****************************************************************************/

static void Tst_GetQstDataFromDB (struct Tst_Question *Question,
                                  char Stem[Cns_MAX_BYTES_TEXT + 1],
                                  char Feedback[Cns_MAX_BYTES_TEXT + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned long NumRow;
   unsigned NumOpt;

   /***** Get the type of answer and the stem from the database *****/
   /* Get the question from database */
   DB_QuerySELECT (&mysql_res,"can not get a question",
		   "SELECT AnsType,"		// row[0]
		          "Shuffle,"		// row[1]
		          "Stem,"		// row[2]
		          "Feedback,"		// row[3]
	                  "MedCod"		// row[4]
		   " FROM tst_questions"
		   " WHERE QstCod=%ld AND CrsCod=%ld",
		   Question->QstCod,Gbl.Hierarchy.Crs.CrsCod);
   row = mysql_fetch_row (mysql_res);

   /* Get the type of answer */
   Question->Answer.Type = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[0]);

   /* Get shuffle (row[1]) */
   Question->Shuffle = (row[1][0] == 'Y');

   /* Get the stem of the question from the database (row[2]) */
   Str_Copy (Stem,row[2],
             Cns_MAX_BYTES_TEXT);

   /* Get the feedback of the question from the database (row[3]) */
   Feedback[0] = '\0';
   if (row[3])
      if (row[3][0])
	 Str_Copy (Feedback,row[3],
	           Cns_MAX_BYTES_TEXT);

   /* Get media (row[4]) */
   Question->Media.MedCod = Str_ConvertStrCodToLongCod (row[4]);
   Med_GetMediaDataByCod (&Question->Media);

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Get the tags from the database *****/
   NumRows = Tst_GetTagsQst (Question->QstCod,&mysql_res);
   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);
      Str_Copy (Question->Tags.Txt[NumRow],row[0],
                Tst_MAX_BYTES_TAG);
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Get the answers from the database *****/
   Tst_GetAnswersQst (Question,&mysql_res,
                      false);	// Don't shuffle
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      row = mysql_fetch_row (mysql_res);
      switch (Question->Answer.Type)
	{
	 case Tst_ANS_INT:
	    if (Question->Answer.NumOptions != 1)
	       Lay_ShowErrorAndExit ("Wrong answer.");
	    Question->Answer.Integer = Tst_GetIntAnsFromStr (row[1]);
	    break;
	 case Tst_ANS_FLOAT:
	    if (Question->Answer.NumOptions != 2)
	       Lay_ShowErrorAndExit ("Wrong answer.");
	    Question->Answer.FloatingPoint[NumOpt] = Str_GetDoubleFromStr (row[1]);
	    break;
	 case Tst_ANS_TRUE_FALSE:
	    if (Question->Answer.NumOptions != 1)
	       Lay_ShowErrorAndExit ("Wrong answer.");
	    Question->Answer.TF = row[1][0];
	    break;
	 case Tst_ANS_UNIQUE_CHOICE:
	 case Tst_ANS_MULTIPLE_CHOICE:
	 case Tst_ANS_TEXT:
	    if (Question->Answer.NumOptions > Tst_MAX_OPTIONS_PER_QUESTION)
	       Lay_ShowErrorAndExit ("Wrong answer.");
	    if (!Tst_AllocateTextChoiceAnswer (Question,NumOpt))
	       /* Abort on error */
	       Ale_ShowAlertsAndExit ();

	    Str_Copy (Question->Answer.Options[NumOpt].Text,row[1],
	              Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);

	    // Feedback (row[2]) is initialized to empty string
	    if (row[2])
	       if (row[2][0])
		  Str_Copy (Question->Answer.Options[NumOpt].Feedback,row[2],
		            Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);

            /* Get media (row[3]) */
	    Question->Answer.Options[NumOpt].Media.MedCod = Str_ConvertStrCodToLongCod (row[3]);
	    Med_GetMediaDataByCod (&Question->Answer.Options[NumOpt].Media);

            /* Get if this option is correct (row[4]) */
	    Question->Answer.Options[NumOpt].Correct = (row[4][0] == 'Y');
	    break;
	 default:
	    break;
	}
     }
   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******* Get media code associated with a test question from database ********/
/*****************************************************************************/
// NumOpt <  0 ==> media associated to stem
// NumOpt >= 0 ==> media associated to answer

static long Tst_GetMedCodFromDB (long CrsCod,long QstCod,int NumOpt)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   long MedCod = -1L;

   if (QstCod > 0)	// Existing question
     {
      /***** Query depending on NumOpt *****/
      if (NumOpt < 0)
	 // Get media associated to stem
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get media",
				   "SELECT MedCod"		// row[0]
				   " FROM tst_questions"
				   " WHERE QstCod=%ld AND CrsCod=%ld",
				   QstCod,CrsCod);
      else
	 // Get media associated to answer
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get media",
				   "SELECT MedCod"		// row[0]
				   " FROM tst_answers"
				   " WHERE QstCod=%ld AND AnsInd=%u",
				   QstCod,(unsigned) NumOpt);

      if (NumRows)
	{
	 if (NumRows == 1)
	   {
	    /***** Get media code (row[0]) *****/
	    row = mysql_fetch_row (mysql_res);
	    MedCod = Str_ConvertStrCodToLongCod (row[0]);
	   }
	 else	// NumRows > 1
	    Lay_ShowErrorAndExit ("Duplicated media in database.");
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return MedCod;
  }

/*****************************************************************************/
/***** Get possible media associated with a test question from database ******/
/*****************************************************************************/
// NumOpt <  0 ==> media associated to stem
// NumOpt >= 0 ==> media associated to an answer option

static void Tst_GetMediaFromDB (long CrsCod,long QstCod,int NumOpt,
                                struct Media *Media)
  {
   /***** Get media *****/
   Media->MedCod = Tst_GetMedCodFromDB (CrsCod,QstCod,NumOpt);
   Med_GetMediaDataByCod (Media);
  }

/*****************************************************************************/
/** Convert a string with the type of answer in database to type of answer ***/
/*****************************************************************************/

Tst_AnswerType_t Tst_ConvertFromStrAnsTypDBToAnsTyp (const char *StrAnsTypeBD)
  {
   Tst_AnswerType_t AnsType;

   if (StrAnsTypeBD != NULL)
      for (AnsType  = (Tst_AnswerType_t) 0;
	   AnsType <= (Tst_AnswerType_t) (Tst_NUM_ANS_TYPES - 1);
	   AnsType++)
         if (!strcmp (StrAnsTypeBD,Tst_StrAnswerTypesDB[AnsType]))
            return AnsType;

   Lay_ShowErrorAndExit ("Wrong type of answer. 1");
   return (Tst_AnswerType_t) 0;	// Not reached
  }

/*****************************************************************************/
/************ Convert a string with an unsigned to answer type ***************/
/*****************************************************************************/

static Tst_AnswerType_t Tst_ConvertFromUnsignedStrToAnsTyp (const char *UnsignedStr)
  {
   unsigned AnsType;

   if (sscanf (UnsignedStr,"%u",&AnsType) != 1)
      Lay_ShowErrorAndExit ("Wrong type of answer. 2");
   if (AnsType >= Tst_NUM_ANS_TYPES)
      Lay_ShowErrorAndExit ("Wrong type of answer. 3");
   return (Tst_AnswerType_t) AnsType;
  }

/*****************************************************************************/
/*************** Receive a question of the self-assessment test **************/
/*****************************************************************************/

void Tst_ReceiveQst (void)
  {
   struct Tst_Test Test;
   char Stem[Cns_MAX_BYTES_TEXT + 1];
   char Feedback[Cns_MAX_BYTES_TEXT + 1];

   /***** Create test *****/
   Tst_TstConstructor (&Test);

   /***** Get parameters of the question from form *****/
   Stem[0] = Feedback[0] = '\0';
   Tst_GetQstFromForm (&Test.Question,Stem,Feedback);

   /***** Make sure that tags, text and answer are not empty *****/
   if (Tst_CheckIfQstFormatIsCorrectAndCountNumOptions (&Test.Question))
     {
      /***** Move images to definitive directories *****/
      Tst_MoveMediaToDefinitiveDirectories (&Test.Question);

      /***** Insert or update question, tags and answer in the database *****/
      Tst_InsertOrUpdateQstTagsAnsIntoDB (&Test.Question);

      /***** Show the question just inserted in the database *****/
      snprintf (Test.AnswerTypes.List,sizeof (Test.AnswerTypes.List),
		"%u",
		(unsigned) Test.Question.Answer.Type);
      Tst_ListOneQstToEdit (&Test);
     }
   else	// Question is wrong
     {
      /***** Whether images has been received or not, reset images *****/
      Tst_ResetMediaOfQuestion (&Test.Question);

      /***** Put form to edit question again *****/
      Tst_PutFormEditOneQst (&Test.Question,Stem,Feedback);
     }

   /***** Destroy test *****/
   Tst_TstDestructor (&Test);
  }

/*****************************************************************************/
/**************** Get parameters of a test question from form ****************/
/*****************************************************************************/

static void Tst_GetQstFromForm (struct Tst_Question *Question,
                                char *Stem,char *Feedback)
  {
   unsigned NumTag;
   unsigned NumTagRead;
   unsigned NumOpt;
   char UnsignedStr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   char TagStr[6 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   char AnsStr[6 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   char FbStr[5 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   char StrMultiAns[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TF[1 + 1];	// (T)rue or (F)alse
   const char *Ptr;
   unsigned NumCorrectAns;

   /***** Get question code *****/
   Question->QstCod = Tst_GetQstCod ();

   /***** Get answer type *****/
   Question->Answer.Type = (Tst_AnswerType_t)
			   Par_GetParToUnsignedLong ("AnswerType",
						     0,
						     Tst_NUM_ANS_TYPES - 1,
						     (unsigned long) Tst_ANS_ALL);
   if (Question->Answer.Type == Tst_ANS_ALL)
      Lay_ShowErrorAndExit ("Wrong type of answer. 4");

   /***** Get question tags *****/
   for (NumTag = 0;
	NumTag < Tst_MAX_TAGS_PER_QUESTION;
	NumTag++)
     {
      snprintf (TagStr,sizeof (TagStr),
	        "TagTxt%u",
		NumTag);
      Par_GetParToText (TagStr,Question->Tags.Txt[NumTag],Tst_MAX_BYTES_TAG);

      if (Question->Tags.Txt[NumTag][0])
        {
         Str_ChangeFormat (Str_FROM_FORM,Str_TO_TEXT,
                           Question->Tags.Txt[NumTag],Tst_MAX_BYTES_TAG,true);
         /* Check if not repeated */
         for (NumTagRead = 0;
              NumTagRead < NumTag;
              NumTagRead++)
            if (!strcmp (Question->Tags.Txt[NumTagRead],Question->Tags.Txt[NumTag]))
              {
               Question->Tags.Txt[NumTag][0] = '\0';
               break;
              }
        }
     }

   /***** Get question stem *****/
   Par_GetParToHTML ("Stem",Stem,Cns_MAX_BYTES_TEXT);

   /***** Get question feedback *****/
   Par_GetParToHTML ("Feedback",Feedback,Cns_MAX_BYTES_TEXT);

   /***** Get media associated to the stem (action, file and title) *****/
   Question->Media.Width   = Tst_IMAGE_SAVED_MAX_WIDTH;
   Question->Media.Height  = Tst_IMAGE_SAVED_MAX_HEIGHT;
   Question->Media.Quality = Tst_IMAGE_SAVED_QUALITY;
   Med_GetMediaFromForm (Gbl.Hierarchy.Crs.CrsCod,Question->QstCod,
                         -1,	// < 0 ==> the image associated to the stem
                         &Question->Media,
                         Tst_GetMediaFromDB,
			 NULL);
   Ale_ShowAlerts (NULL);

   /***** Get answers *****/
   Question->Shuffle = false;
   switch (Question->Answer.Type)
     {
      case Tst_ANS_INT:
         if (!Tst_AllocateTextChoiceAnswer (Question,0))
	    /* Abort on error */
	    Ale_ShowAlertsAndExit ();

	 Par_GetParToText ("AnsInt",Question->Answer.Options[0].Text,
			   Cns_MAX_DECIMAL_DIGITS_LONG);
	 break;
      case Tst_ANS_FLOAT:
         if (!Tst_AllocateTextChoiceAnswer (Question,0))
	    /* Abort on error */
	    Ale_ShowAlertsAndExit ();

	 Par_GetParToText ("AnsFloatMin",Question->Answer.Options[0].Text,
	                   Tst_MAX_BYTES_FLOAT_ANSWER);

         if (!Tst_AllocateTextChoiceAnswer (Question,1))
	    /* Abort on error */
	    Ale_ShowAlertsAndExit ();

	 Par_GetParToText ("AnsFloatMax",Question->Answer.Options[1].Text,
	                   Tst_MAX_BYTES_FLOAT_ANSWER);
	 break;
      case Tst_ANS_TRUE_FALSE:
	 Par_GetParToText ("AnsTF",TF,1);
	 Question->Answer.TF = TF[0];
	 break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
         /* Get shuffle */
         Question->Shuffle = Par_GetParToBool ("Shuffle");
	 /* falls through */
	 /* no break */
      case Tst_ANS_TEXT:
         /* Get the texts of the answers */
         for (NumOpt = 0;
              NumOpt < Tst_MAX_OPTIONS_PER_QUESTION;
              NumOpt++)
           {
            if (!Tst_AllocateTextChoiceAnswer (Question,NumOpt))
	       /* Abort on error */
	       Ale_ShowAlertsAndExit ();

            /* Get answer */
            snprintf (AnsStr,sizeof (AnsStr),
        	      "AnsStr%u",
		      NumOpt);
	    Par_GetParToHTML (AnsStr,Question->Answer.Options[NumOpt].Text,
	                      Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
	    if (Question->Answer.Type == Tst_ANS_TEXT)
	       /* In order to compare student answer to stored answer,
	          the text answers are stored avoiding two or more consecurive spaces */
               Str_ReplaceSeveralSpacesForOne (Question->Answer.Options[NumOpt].Text);

            /* Get feedback */
            snprintf (FbStr,sizeof (FbStr),
        	      "FbStr%u",
		      NumOpt);
	    Par_GetParToHTML (FbStr,Question->Answer.Options[NumOpt].Feedback,
	                      Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);

	    /* Get media associated to the answer (action, file and title) */
	    if (Question->Answer.Type == Tst_ANS_UNIQUE_CHOICE ||
		Question->Answer.Type == Tst_ANS_MULTIPLE_CHOICE)
	      {
	       Question->Answer.Options[NumOpt].Media.Width   = Tst_IMAGE_SAVED_MAX_WIDTH;
	       Question->Answer.Options[NumOpt].Media.Height  = Tst_IMAGE_SAVED_MAX_HEIGHT;
	       Question->Answer.Options[NumOpt].Media.Quality = Tst_IMAGE_SAVED_QUALITY;
	       Med_GetMediaFromForm (Gbl.Hierarchy.Crs.CrsCod,Question->QstCod,
	                             (int) NumOpt,	// >= 0 ==> the image associated to an answer
	                             &Question->Answer.Options[NumOpt].Media,
				     Tst_GetMediaFromDB,
				     NULL);
	       Ale_ShowAlerts (NULL);
	      }
           }

         /* Get the numbers of correct answers */
         if (Question->Answer.Type == Tst_ANS_UNIQUE_CHOICE)
           {
	    NumCorrectAns = (unsigned) Par_GetParToUnsignedLong ("AnsUni",
	                                                         0,
	                                                         Tst_MAX_OPTIONS_PER_QUESTION - 1,
	                                                         0);
            Question->Answer.Options[NumCorrectAns].Correct = true;
           }
      	 else if (Question->Answer.Type == Tst_ANS_MULTIPLE_CHOICE)
           {
	    Par_GetParMultiToText ("AnsMulti",StrMultiAns,Tst_MAX_BYTES_ANSWERS_ONE_QST);
 	    Ptr = StrMultiAns;
            while (*Ptr)
              {
  	       Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,Cns_MAX_DECIMAL_DIGITS_UINT);
	       if (sscanf (UnsignedStr,"%u",&NumCorrectAns) != 1)
	          Lay_ShowErrorAndExit ("Wrong selected answer.");
               if (NumCorrectAns >= Tst_MAX_OPTIONS_PER_QUESTION)
	          Lay_ShowErrorAndExit ("Wrong selected answer.");
               Question->Answer.Options[NumCorrectAns].Correct = true;
              }
           }
         else // Tst_ANS_TEXT
            for (NumOpt = 0;
        	 NumOpt < Tst_MAX_OPTIONS_PER_QUESTION;
        	 NumOpt++)
               if (Question->Answer.Options[NumOpt].Text[0])
                  Question->Answer.Options[NumOpt].Correct = true;	// All the answers are correct
	 break;
      default:
         break;
     }

   /***** Adjust variables related to this test question *****/
   for (NumTag = 0, Question->Tags.Num = 0;
        NumTag < Tst_MAX_TAGS_PER_QUESTION;
        NumTag++)
      if (Question->Tags.Txt[NumTag][0])
         Question->Tags.Num++;
   Question->Stem.Text = Stem;
   Question->Stem.Length = strlen (Question->Stem.Text);
   Question->Feedback.Text = Feedback;
   Question->Feedback.Length = strlen (Question->Feedback.Text);
  }

/*****************************************************************************/
/*********************** Check if a question is correct **********************/
/*****************************************************************************/
// Returns false if question format is wrong
// Counts Question->Answer.NumOptions
// Computes Question->Answer.Integer and Question->Answer.FloatingPoint[0..1]

bool Tst_CheckIfQstFormatIsCorrectAndCountNumOptions (struct Tst_Question *Question)
  {
   extern const char *Txt_You_must_type_at_least_one_tag_for_the_question;
   extern const char *Txt_You_must_type_the_stem_of_the_question;
   extern const char *Txt_You_must_select_a_T_F_answer;
   extern const char *Txt_You_can_not_leave_empty_intermediate_answers;
   extern const char *Txt_You_must_type_at_least_the_first_two_answers;
   extern const char *Txt_You_must_mark_an_answer_as_correct;
   extern const char *Txt_You_must_type_at_least_the_first_answer;
   extern const char *Txt_You_must_enter_an_integer_value_as_the_correct_answer;
   extern const char *Txt_You_must_enter_the_range_of_floating_point_values_allowed_as_answer;
   extern const char *Txt_The_lower_limit_of_correct_answers_must_be_less_than_or_equal_to_the_upper_limit;
   unsigned NumOpt;
   unsigned NumLastOpt;
   bool ThereIsEndOfAnswers;
   unsigned i;

   /***** This function also counts the number of options. Initialize this number to 0. *****/
   Question->Answer.NumOptions = 0;

   /***** A question must have at least one tag *****/
   if (!Question->Tags.Num) // There are no tags with text
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_at_least_one_tag_for_the_question);
      return false;
     }

   /***** A question must have a stem*****/
   if (!Question->Stem.Length)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_the_stem_of_the_question);
      return false;
     }

   /***** Check answer *****/
   switch (Question->Answer.Type)
     {
      case Tst_ANS_INT:
	 /* First option should be filled */
         if (!Question->Answer.Options[0].Text)
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_enter_an_integer_value_as_the_correct_answer);
            return false;
           }
         if (!Question->Answer.Options[0].Text[0])
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_enter_an_integer_value_as_the_correct_answer);
            return false;
           }

         Question->Answer.Integer = Tst_GetIntAnsFromStr (Question->Answer.Options[0].Text);
         Question->Answer.NumOptions = 1;
         break;
      case Tst_ANS_FLOAT:
	 /* First two options should be filled */
         if (!Question->Answer.Options[0].Text ||
             !Question->Answer.Options[1].Text)
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_enter_the_range_of_floating_point_values_allowed_as_answer);
            return false;
           }
         if (!Question->Answer.Options[0].Text[0] ||
             !Question->Answer.Options[1].Text[0])
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_enter_the_range_of_floating_point_values_allowed_as_answer);
            return false;
           }

         /* Lower limit should be <= upper limit */
         for (i = 0;
              i < 2;
              i++)
            Question->Answer.FloatingPoint[i] = Str_GetDoubleFromStr (Question->Answer.Options[i].Text);
         if (Question->Answer.FloatingPoint[0] >
             Question->Answer.FloatingPoint[1])
           {
            Ale_ShowAlert (Ale_WARNING,Txt_The_lower_limit_of_correct_answers_must_be_less_than_or_equal_to_the_upper_limit);
            return false;
           }

         Question->Answer.NumOptions = 2;
         break;
      case Tst_ANS_TRUE_FALSE:
	 /* Answer should be 'T' or 'F' */
         if (Question->Answer.TF != 'T' &&
             Question->Answer.TF != 'F')
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_select_a_T_F_answer);
            return false;
           }

         Question->Answer.NumOptions = 1;
         break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
	 /* No option should be empty before a non-empty option */
         for (NumOpt = 0, NumLastOpt = 0, ThereIsEndOfAnswers = false;
              NumOpt < Tst_MAX_OPTIONS_PER_QUESTION;
              NumOpt++)
            if (Question->Answer.Options[NumOpt].Text)
              {
               if (Question->Answer.Options[NumOpt].Text[0] ||				// Text
        	   Question->Answer.Options[NumOpt].Media.Type != Med_TYPE_NONE)	// or media
                 {
                  if (ThereIsEndOfAnswers)
                    {
                     Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_empty_intermediate_answers);
                     return false;
                    }
                  NumLastOpt = NumOpt;
                  Question->Answer.NumOptions++;
                 }
               else
                  ThereIsEndOfAnswers = true;
              }
            else
               ThereIsEndOfAnswers = true;

         /* The two first options must be filled */
         if (NumLastOpt < 1)
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_at_least_the_first_two_answers);
            return false;
           }

         /* Its mandatory to mark at least one option as correct */
         for (NumOpt = 0;
              NumOpt <= NumLastOpt;
              NumOpt++)
            if (Question->Answer.Options[NumOpt].Correct)
               break;
         if (NumOpt > NumLastOpt)
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_mark_an_answer_as_correct);
            return false;
           }
         break;
      case Tst_ANS_TEXT:
	 /* First option should be filled */
         if (!Question->Answer.Options[0].Text)		// If the first answer is empty
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_at_least_the_first_answer);
            return false;
           }
         if (!Question->Answer.Options[0].Text[0])	// If the first answer is empty
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_at_least_the_first_answer);
            return false;
           }

	 /* No option should be empty before a non-empty option */
         for (NumOpt=0, ThereIsEndOfAnswers=false;
              NumOpt<Tst_MAX_OPTIONS_PER_QUESTION;
              NumOpt++)
            if (Question->Answer.Options[NumOpt].Text)
              {
               if (Question->Answer.Options[NumOpt].Text[0])
                 {
                  if (ThereIsEndOfAnswers)
                    {
                     Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_empty_intermediate_answers);
                     return false;
                    }
                  Question->Answer.NumOptions++;
                 }
               else
                  ThereIsEndOfAnswers = true;
              }
            else
               ThereIsEndOfAnswers = true;
         break;
      default:
         break;
     }

    return true;	// Question format without errors
   }

/*****************************************************************************/
/*********** Check if a test question already exists in database *************/
/*****************************************************************************/

bool Tst_CheckIfQuestionExistsInDB (struct Tst_Question *Question)
  {
   extern const char *Tst_StrAnswerTypesDB[Tst_NUM_ANS_TYPES];
   MYSQL_RES *mysql_res_qst;
   MYSQL_RES *mysql_res_ans;
   MYSQL_ROW row;
   bool IdenticalQuestionFound = false;
   bool IdenticalAnswers;
   unsigned NumQst;
   unsigned NumQstsWithThisStem;
   unsigned NumOpt;
   unsigned NumOptsExistingQstInDB;
   unsigned i;

   /***** Check if stem exists *****/
   NumQstsWithThisStem =
   (unsigned) DB_QuerySELECT (&mysql_res_qst,"can not check"
					     " if a question exists",
			      "SELECT QstCod FROM tst_questions"
			      " WHERE CrsCod=%ld AND AnsType='%s' AND Stem='%s'",
			      Gbl.Hierarchy.Crs.CrsCod,
			      Tst_StrAnswerTypesDB[Question->Answer.Type],
			      Question->Stem.Text);

   if (NumQstsWithThisStem)	// There are questions in database with the same stem that the one of this question
     {
      /***** Check if the answer exists in any of the questions with the same stem *****/
      /* For each question with the same stem */
      for (NumQst = 0;
           !IdenticalQuestionFound && NumQst < NumQstsWithThisStem;
           NumQst++)
        {
	 /* Get question code */
         row = mysql_fetch_row (mysql_res_qst);
         if ((Question->QstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of question.");

         /* Get answers from this question */
         NumOptsExistingQstInDB =
         (unsigned) DB_QuerySELECT (&mysql_res_ans,"can not get the answer"
						   " of a question",
				    "SELECT Answer FROM tst_answers"
				    " WHERE QstCod=%ld ORDER BY AnsInd",
				    Question->QstCod);

         switch (Question->Answer.Type)
           {
            case Tst_ANS_INT:
               row = mysql_fetch_row (mysql_res_ans);
               IdenticalQuestionFound = (Tst_GetIntAnsFromStr (row[0]) == Question->Answer.Integer);
               break;
            case Tst_ANS_FLOAT:
               for (IdenticalAnswers = true, i = 0;
                    IdenticalAnswers && i < 2;
                    i++)
                 {
                  row = mysql_fetch_row (mysql_res_ans);
                  IdenticalAnswers = (Str_GetDoubleFromStr (row[0]) == Question->Answer.FloatingPoint[i]);
                 }
               IdenticalQuestionFound = IdenticalAnswers;
               break;
            case Tst_ANS_TRUE_FALSE:
               row = mysql_fetch_row (mysql_res_ans);
               IdenticalQuestionFound = (Str_ConvertToUpperLetter (row[0][0]) == Question->Answer.TF);
               break;
            case Tst_ANS_UNIQUE_CHOICE:
            case Tst_ANS_MULTIPLE_CHOICE:
            case Tst_ANS_TEXT:
               if (NumOptsExistingQstInDB == Question->Answer.NumOptions)
                 {
                  for (IdenticalAnswers = true, NumOpt = 0;
                       IdenticalAnswers && NumOpt < NumOptsExistingQstInDB;
                       NumOpt++)
                    {
                     row = mysql_fetch_row (mysql_res_ans);

                     if (strcasecmp (row[0],Question->Answer.Options[NumOpt].Text))
                        IdenticalAnswers = false;
                    }
                 }
               else	// Different number of answers (options)
                  IdenticalAnswers = false;
               IdenticalQuestionFound = IdenticalAnswers;
               break;
            default:
               break;
           }

         /* Free structure that stores the query result for answers */
         DB_FreeMySQLResult (&mysql_res_ans);
        }
     }
   else	// Stem does not exist
      IdenticalQuestionFound = false;

   /* Free structure that stores the query result for questions */
   DB_FreeMySQLResult (&mysql_res_qst);

   return IdenticalQuestionFound;
  }

/*****************************************************************************/
/* Move images associates to a test question to their definitive directories */
/*****************************************************************************/

static void Tst_MoveMediaToDefinitiveDirectories (struct Tst_Question *Question)
  {
   unsigned NumOpt;
   long CurrentMedCodInDB;

   /***** Media associated to question stem *****/
   CurrentMedCodInDB = Tst_GetMedCodFromDB (Gbl.Hierarchy.Crs.CrsCod,Question->QstCod,
                                            -1L);	// Get current media code associated to stem
   Med_RemoveKeepOrStoreMedia (CurrentMedCodInDB,&Question->Media);

   /****** Move media associated to answers *****/
   switch (Question->Answer.Type)
     {
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
	 for (NumOpt = 0;
	      NumOpt < Question->Answer.NumOptions;
	      NumOpt++)
	   {
	    CurrentMedCodInDB = Tst_GetMedCodFromDB (Gbl.Hierarchy.Crs.CrsCod,Question->QstCod,
						     NumOpt);	// Get current media code associated to this option
	    Med_RemoveKeepOrStoreMedia (CurrentMedCodInDB,&Question->Answer.Options[NumOpt].Media);
	   }
	 break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/******************** Get a integer number from a string *********************/
/*****************************************************************************/

long Tst_GetIntAnsFromStr (char *Str)
  {
   long LongNum;

   if (Str == NULL)
      return 0.0;

   /***** The string is "scanned" as long *****/
   if (sscanf (Str,"%ld",&LongNum) != 1)	// If the string does not hold a valid integer number...
     {
      LongNum = 0L;	// ...the number is reset to 0
      Str[0] = '\0';	// ...and the string is reset to ""
     }

   return LongNum;
  }

/*****************************************************************************/
/***************** Check if this tag exists for current course ***************/
/*****************************************************************************/

static long Tst_GetTagCodFromTagTxt (const char *TagTxt)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   long TagCod = -1L;	// -1 means that the tag does not exist in database

   /***** Get tag code from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get tag",
			     "SELECT TagCod FROM tst_tags"
			     " WHERE CrsCod=%ld AND TagTxt='%s'",
			     Gbl.Hierarchy.Crs.CrsCod,TagTxt);
   if (NumRows == 1)
     {
      /***** Get tag code *****/
      row = mysql_fetch_row (mysql_res);
      if ((TagCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
         Ale_CreateAlert (Ale_ERROR,NULL,
                          "Wrong code of tag.");
     }
   else if (NumRows > 1)
      Ale_CreateAlert (Ale_ERROR,NULL,
	               "Duplicated tag.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Abort on error *****/
   if (Ale_GetTypeOfLastAlert () == Ale_ERROR)
      Ale_ShowAlertsAndExit ();

   return TagCod;
  }

/*****************************************************************************/
/********************* Insert new tag into tst_tags table ********************/
/*****************************************************************************/

static long Tst_CreateNewTag (long CrsCod,const char *TagTxt)
  {
   /***** Insert new tag into tst_tags table *****/
   return
   DB_QueryINSERTandReturnCode ("can not create new tag",
				"INSERT INTO tst_tags"
				" (CrsCod,ChangeTime,TagTxt,TagHidden)"
				" VALUES"
				" (%ld,NOW(),'%s','N')",
				CrsCod,TagTxt);
  }

/*****************************************************************************/
/********** Change visibility of an existing tag into tst_tags table *********/
/*****************************************************************************/

static void Tst_EnableOrDisableTag (long TagCod,bool TagHidden)
  {
   /***** Insert new tag into tst_tags table *****/
   DB_QueryUPDATE ("can not update the visibility of a tag",
		   "UPDATE tst_tags SET TagHidden='%c',ChangeTime=NOW()"
                   " WHERE TagCod=%ld AND CrsCod=%ld",
		   TagHidden ? 'Y' :
			       'N',
		   TagCod,Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/***************** Request the removal of selected questions *****************/
/*****************************************************************************/

void Tst_RequestRemoveSelectedQsts (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_selected_questions;
   extern const char *Txt_Remove_questions;
   struct Tst_Test Test;

   /***** Create test *****/
   Tst_TstConstructor (&Test);

   /***** Get parameters *****/
   if (Tst_GetParamsTst (&Test,Tst_EDIT_TEST))	// Get parameters from the form
     {
      /***** Show question and button to remove question *****/
      Ale_ShowAlertAndButton (ActRemSevTstQst,NULL,NULL,
			     Tst_PutParamsRemoveSelectedQsts,&Test,
			     Btn_REMOVE_BUTTON,Txt_Remove_questions,
			     Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_selected_questions);
     }
   else
      Ale_ShowAlert (Ale_ERROR,"Wrong parameters.");

   /***** Continue editing questions *****/
   Tst_ListQuestionsToEdit ();

   /***** Destroy test *****/
   Tst_TstDestructor (&Test);
  }

/*****************************************************************************/
/**************** Put parameters to remove selected questions ****************/
/*****************************************************************************/

static void Tst_PutParamsRemoveSelectedQsts (void *TestPtr)
  {
   struct Tst_Test *Test;

   if (TestPtr)
     {
      Test = (struct Tst_Test *) TestPtr;

      Dat_WriteParamsIniEndDates ();
      Tst_WriteParamEditQst (Test);
     }
  }

/*****************************************************************************/
/************************** Remove several questions *************************/
/*****************************************************************************/

void Tst_RemoveSelectedQsts (void)
  {
   extern const char *Txt_Questions_removed_X;
   struct Tst_Test Test;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQst;
   long QstCod;

   /***** Create test *****/
   Tst_TstConstructor (&Test);

   /***** Get parameters *****/
   if (Tst_GetParamsTst (&Test,Tst_EDIT_TEST))	// Get parameters
     {
      /***** Get question codes *****/
      Tst_GetQuestions (&Test,&mysql_res);	// Query database

      /***** Remove questions one by one *****/
      for (NumQst = 0;
	   NumQst < Test.NumQsts;
	   NumQst++)
	{
	 /* Get question code (row[0]) */
	 row = mysql_fetch_row (mysql_res);
	 if ((QstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of question.");

	 /* Remove test question from database */
	 Tst_RemoveOneQstFromDB (Gbl.Hierarchy.Crs.CrsCod,QstCod);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Write message *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_Questions_removed_X,Test.NumQsts);
     }

   /***** Destroy test *****/
   Tst_TstDestructor (&Test);
  }

/*****************************************************************************/
/********************* Put icon to remove one question ***********************/
/*****************************************************************************/

static void Tst_PutIconToRemoveOneQst (void *QstCodPtr)
  {
   Ico_PutContextualIconToRemove (ActReqRemOneTstQst,
                                  Tst_PutParamsRemoveOnlyThisQst,QstCodPtr);
  }

/*****************************************************************************/
/******************** Request the removal of a question **********************/
/*****************************************************************************/

void Tst_RequestRemoveOneQst (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_question_X;
   extern const char *Txt_Remove_question;
   bool EditingOnlyThisQst;
   struct Tst_Test Test;

   /***** Create test *****/
   Tst_TstConstructor (&Test);

   /***** Get main parameters from form *****/
   /* Get the question code */
   Test.Question.QstCod = Tst_GetQstCod ();
   if (Test.Question.QstCod <= 0)
      Lay_ShowErrorAndExit ("Wrong code of question.");

   /* Get a parameter that indicates whether it's necessary
      to continue listing the rest of questions */
   EditingOnlyThisQst = Par_GetParToBool ("OnlyThisQst");

   /* Get other parameters */
   if (!EditingOnlyThisQst)
      if (!Tst_GetParamsTst (&Test,Tst_EDIT_TEST))
	 Lay_ShowErrorAndExit ("Wrong test parameters.");

   /***** Show question and button to remove question *****/
   if (EditingOnlyThisQst)
      Ale_ShowAlertAndButton (ActRemOneTstQst,NULL,NULL,
			      Tst_PutParamsRemoveOnlyThisQst,&Test.Question.QstCod,
			      Btn_REMOVE_BUTTON,Txt_Remove_question,
			      Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_question_X,
			      Test.Question.QstCod);
   else
      Ale_ShowAlertAndButton (ActRemOneTstQst,NULL,NULL,
			      Tst_PutParamsRemoveOneQstWhileEditing,&Test,
			      Btn_REMOVE_BUTTON,Txt_Remove_question,
			      Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_question_X,
			      Test.Question.QstCod);

   /***** Continue editing questions *****/
   if (EditingOnlyThisQst)
      Tst_ListOneQstToEdit (&Test);
   else
      Tst_ListQuestionsToEdit ();

   /***** Destroy test *****/
   Tst_TstDestructor (&Test);
  }

/*****************************************************************************/
/***** Put parameters to remove question when editing only one question ******/
/*****************************************************************************/

static void Tst_PutParamsRemoveOnlyThisQst (void *QstCodPtr)
  {
   if (QstCodPtr)
     {
      Tst_PutParamQstCod (QstCodPtr);
      Par_PutHiddenParamChar ("OnlyThisQst",'Y');
     }
  }

/*****************************************************************************/
/***** Put parameters to remove question when editing several questions ******/
/*****************************************************************************/

static void Tst_PutParamsRemoveOneQstWhileEditing (void *TestPtr)
  {
   struct Tst_Test *Test;

   if (TestPtr)
     {
      Test = (struct Tst_Test *) TestPtr;

      Tst_PutParamQstCod (&Test->Question.QstCod);
      Dat_WriteParamsIniEndDates ();
      Tst_WriteParamEditQst (Test);
     }
  }

/*****************************************************************************/
/***************************** Remove a question *****************************/
/*****************************************************************************/

void Tst_RemoveOneQst (void)
  {
   extern const char *Txt_Question_removed;
   long QstCod;
   bool EditingOnlyThisQst;

   /***** Get the question code *****/
   QstCod = Tst_GetQstCod ();
   if (QstCod <= 0)
      Lay_ShowErrorAndExit ("Wrong code of question.");

   /***** Get a parameter that indicates whether it's necessary
          to continue listing the rest of questions ******/
   EditingOnlyThisQst = Par_GetParToBool ("OnlyThisQst");

   /***** Remove test question from database *****/
   Tst_RemoveOneQstFromDB (Gbl.Hierarchy.Crs.CrsCod,QstCod);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Question_removed);

   /***** Continue editing questions *****/
   if (!EditingOnlyThisQst)
      Tst_ListQuestionsToEdit ();
  }

/*****************************************************************************/
/********************** Remove a question from database **********************/
/*****************************************************************************/

static void Tst_RemoveOneQstFromDB (long CrsCod,long QstCod)
  {
   long MedCod;

   /***** Remove media associated to question *****/
   /* Remove media associated to answers */
   Tst_RemoveMediaFromAllAnsOfQst (CrsCod,QstCod);

   /* Remove media associated to stem */
   MedCod = Tst_GetMedCodFromDB (CrsCod,QstCod,
                                 -1L);	// Get current media code associated to stem
   Med_RemoveMedia (MedCod);

   /***** Remove the question from all the tables *****/
   /* Remove answers and tags from this test question */
   Tst_RemAnsFromQst (QstCod);
   Tst_RemTagsFromQst (QstCod);
   Tst_RemoveUnusedTagsFromCrs (CrsCod);

   /* Remove the question itself */
   DB_QueryDELETE ("can not remove a question",
		   "DELETE FROM tst_questions"
		   " WHERE QstCod=%ld AND CrsCod=%ld",
		   QstCod,CrsCod);

   if (!mysql_affected_rows (&Gbl.mysql))
      Lay_ShowErrorAndExit ("Wrong question.");
  }

/*****************************************************************************/
/*********************** Change the shuffle of a question ********************/
/*****************************************************************************/

void Tst_ChangeShuffleQst (void)
  {
   extern const char *Txt_The_answers_of_the_question_with_code_X_will_appear_shuffled;
   extern const char *Txt_The_answers_of_the_question_with_code_X_will_appear_without_shuffling;
   struct Tst_Test Test;
   bool EditingOnlyThisQst;
   bool Shuffle;

   /***** Create test *****/
   Tst_TstConstructor (&Test);

   /***** Get the question code *****/
   Test.Question.QstCod = Tst_GetQstCod ();
   if (Test.Question.QstCod <= 0)
      Lay_ShowErrorAndExit ("Wrong code of question.");

   /***** Get a parameter that indicates whether it's necessary to continue listing the rest of questions ******/
   EditingOnlyThisQst = Par_GetParToBool ("OnlyThisQst");

   /***** Get a parameter that indicates whether it's possible to shuffle the answers of this question ******/
   Shuffle = Par_GetParToBool ("Shuffle");

   /***** Remove the question from all the tables *****/
   /* Update the question changing the current shuffle */
   DB_QueryUPDATE ("can not update the shuffle type of a question",
		   "UPDATE tst_questions SET Shuffle='%c'"
                   " WHERE QstCod=%ld AND CrsCod=%ld",
		   Shuffle ? 'Y' :
			     'N',
		   Test.Question.QstCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Shuffle ? Txt_The_answers_of_the_question_with_code_X_will_appear_shuffled :
                                        Txt_The_answers_of_the_question_with_code_X_will_appear_without_shuffling,
                  Test.Question.QstCod);

   /***** Continue editing questions *****/
   if (EditingOnlyThisQst)
      Tst_ListOneQstToEdit (&Test);
   else
      Tst_ListQuestionsToEdit ();

   /***** Destroy test *****/
   Tst_TstDestructor (&Test);
  }

/*****************************************************************************/
/************ Get the parameter with the code of a test question *************/
/*****************************************************************************/

static long Tst_GetQstCod (void)
  {
   /***** Get code of test question *****/
   return Par_GetParToLong ("QstCod");
  }

/*****************************************************************************/
/************ Put parameter with question code to edit, remove... ************/
/*****************************************************************************/

void Tst_PutParamQstCod (void *QstCodPtr)	// Should be a pointer to long
  {
   long QstCod;

   if (QstCodPtr)
     {
      QstCod = *((long *) QstCodPtr);
      if (QstCod > 0)	// If question exists
	 Par_PutHiddenParamLong (NULL,"QstCod",QstCod);
     }
  }

/*****************************************************************************/
/******** Insert or update question, tags and anser in the database **********/
/*****************************************************************************/

void Tst_InsertOrUpdateQstTagsAnsIntoDB (struct Tst_Question *Question)
  {
   /***** Insert or update question in the table of questions *****/
   Tst_InsertOrUpdateQstIntoDB (Question);
   if (Question->QstCod > 0)
     {
      /***** Insert tags in the tags table *****/
      Tst_InsertTagsIntoDB (Question);

      /***** Remove unused tags in current course *****/
      Tst_RemoveUnusedTagsFromCrs (Gbl.Hierarchy.Crs.CrsCod);

      /***** Insert answers in the answers table *****/
      Tst_InsertAnswersIntoDB (Question);
     }
  }

/*****************************************************************************/
/*********** Insert or update question in the table of questions *************/
/*****************************************************************************/

static void Tst_InsertOrUpdateQstIntoDB (struct Tst_Question *Question)
  {
   if (Question->QstCod < 0)	// It's a new question
     {
      /***** Insert question in the table of questions *****/
      Question->QstCod =
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
				   Gbl.Hierarchy.Crs.CrsCod,
				   Tst_StrAnswerTypesDB[Question->Answer.Type],
				   Question->Shuffle ? 'Y' :
						       'N',
				   Question->Stem.Text,
				   Question->Feedback.Text ? Question->Feedback.Text :
					                     "",
				   Question->Media.MedCod);
     }
   else			// It's an existing question
     {
      /***** Update existing question *****/
      /* Update question in database */
      DB_QueryUPDATE ("can not update question",
		      "UPDATE tst_questions"
		      " SET EditTime=NOW(),"
		           "AnsType='%s',"
		           "Shuffle='%c',"
		           "Stem='%s',"
		           "Feedback='%s',"
		           "MedCod=%ld"
		      " WHERE QstCod=%ld AND CrsCod=%ld",
		      Tst_StrAnswerTypesDB[Question->Answer.Type],
		      Question->Shuffle ? 'Y' :
					  'N',
		      Question->Stem.Text,
		      Question->Feedback.Text ? Question->Feedback.Text :
			                        "",
		      Question->Media.MedCod,
		      Question->QstCod,Gbl.Hierarchy.Crs.CrsCod);

      /* Remove answers and tags from this test question */
      Tst_RemAnsFromQst (Question->QstCod);
      Tst_RemTagsFromQst (Question->QstCod);
     }
  }

/*****************************************************************************/
/*********************** Insert tags in the tags table ***********************/
/*****************************************************************************/

static void Tst_InsertTagsIntoDB (const struct Tst_Question *Question)
  {
   unsigned NumTag;
   unsigned TagIdx;
   long TagCod;

   /***** For each tag... *****/
   for (NumTag = 0, TagIdx = 0;
        TagIdx < Question->Tags.Num;
        NumTag++)
      if (Question->Tags.Txt[NumTag][0])
        {
         /***** Check if this tag exists for current course *****/
         if ((TagCod = Tst_GetTagCodFromTagTxt (Question->Tags.Txt[NumTag])) < 0)
            /* This tag is new for current course. Add it to tags table */
            TagCod = Tst_CreateNewTag (Gbl.Hierarchy.Crs.CrsCod,Question->Tags.Txt[NumTag]);

         /***** Insert tag in tst_question_tags *****/
         DB_QueryINSERT ("can not create tag",
			 "INSERT INTO tst_question_tags"
                         " (QstCod,TagCod,TagInd)"
                         " VALUES"
                         " (%ld,%ld,%u)",
			 Question->QstCod,TagCod,TagIdx);

         TagIdx++;
        }
  }

/*****************************************************************************/
/******************* Insert answers in the answers table *********************/
/*****************************************************************************/

static void Tst_InsertAnswersIntoDB (struct Tst_Question *Question)
  {
   unsigned NumOpt;
   unsigned i;

   /***** Insert answers in the answers table *****/
   switch (Question->Answer.Type)
     {
      case Tst_ANS_INT:
         DB_QueryINSERT ("can not create answer",
			 "INSERT INTO tst_answers"
                         " (QstCod,AnsInd,Answer,Feedback,MedCod,Correct)"
                         " VALUES"
                         " (%ld,0,%ld,'',-1,'Y')",
			 Question->QstCod,
			 Question->Answer.Integer);
         break;
      case Tst_ANS_FLOAT:
	 Str_SetDecimalPointToUS ();	// To print the floating point as a dot
   	 for (i = 0;
   	      i < 2;
   	      i++)
            DB_QueryINSERT ("can not create answer",
        		    "INSERT INTO tst_answers"
                            " (QstCod,AnsInd,Answer,Feedback,MedCod,Correct)"
                            " VALUES"
                            " (%ld,%u,'%.15lg','',-1,'Y')",
			    Question->QstCod,i,
			    Question->Answer.FloatingPoint[i]);
         Str_SetDecimalPointToLocal ();	// Return to local system
         break;
      case Tst_ANS_TRUE_FALSE:
         DB_QueryINSERT ("can not create answer",
			 "INSERT INTO tst_answers"
                         " (QstCod,AnsInd,Answer,Feedback,MedCod,Correct)"
                         " VALUES"
                         " (%ld,0,'%c','',-1,'Y')",
			 Question->QstCod,
			 Question->Answer.TF);
         break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
      case Tst_ANS_TEXT:
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
			       Question->Answer.Options[NumOpt].Correct ? 'Y' :
								          'N');

               /* Update image status */
	       if (Question->Answer.Options[NumOpt].Media.Type != Med_TYPE_NONE)
		  Question->Answer.Options[NumOpt].Media.Status = Med_STORED_IN_DB;
              }
	 break;
      default:
         break;
     }
  }

/*****************************************************************************/
/******************* Remove all test exams made in a course ******************/
/*****************************************************************************/

void Tst_RemoveCrsTests (long CrsCod)
  {
   /***** Remove tests status in the course *****/
   DB_QueryDELETE ("can not remove status of tests of a course",
		   "DELETE FROM tst_status WHERE CrsCod=%ld",
		   CrsCod);

   /***** Remove test configuration of the course *****/
   DB_QueryDELETE ("can not remove configuration of tests of a course",
		   "DELETE FROM tst_config WHERE CrsCod=%ld",
		   CrsCod);

   /***** Remove associations between test questions
          and test tags in the course *****/
   DB_QueryDELETE ("can not remove tags associated"
		   " to questions of tests of a course",
		   "DELETE FROM tst_question_tags"
	           " USING tst_questions,tst_question_tags"
                   " WHERE tst_questions.CrsCod=%ld"
                   " AND tst_questions.QstCod=tst_question_tags.QstCod",
		   CrsCod);

   /***** Remove test tags in the course *****/
   DB_QueryDELETE ("can not remove tags of test of a course",
		   "DELETE FROM tst_tags WHERE CrsCod=%ld",
		   CrsCod);

   /***** Remove test answers in the course *****/
   DB_QueryDELETE ("can not remove answers of tests of a course",
		   "DELETE FROM tst_answers USING tst_questions,tst_answers"
                   " WHERE tst_questions.CrsCod=%ld"
                   " AND tst_questions.QstCod=tst_answers.QstCod",
		   CrsCod);

   /***** Remove media associated to test questions in the course *****/
   Tst_RemoveAllMedFilesFromAnsOfAllQstsInCrs (CrsCod);
   Tst_RemoveAllMedFilesFromStemOfAllQstsInCrs (CrsCod);

   /***** Remove test questions in the course *****/
   DB_QueryDELETE ("can not remove test questions of a course",
		   "DELETE FROM tst_questions WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/******************** Remove answers from a test question ********************/
/*****************************************************************************/

static void Tst_RemAnsFromQst (long QstCod)
  {
   /***** Remove answers *****/
   DB_QueryDELETE ("can not remove the answers of a question",
		   "DELETE FROM tst_answers WHERE QstCod=%ld",
		   QstCod);
  }

/*****************************************************************************/
/************************** Remove tags from a test question *****************/
/*****************************************************************************/

static void Tst_RemTagsFromQst (long QstCod)
  {
   /***** Remove tags *****/
   DB_QueryDELETE ("can not remove the tags of a question",
		   "DELETE FROM tst_question_tags WHERE QstCod=%ld",
		   QstCod);
  }

/*****************************************************************************/
/********************** Remove unused tags in a course ***********************/
/*****************************************************************************/

static void Tst_RemoveUnusedTagsFromCrs (long CrsCod)
  {
   /***** Remove unused tags from tst_tags *****/
   DB_QueryDELETE ("can not remove unused tags",
		   "DELETE FROM tst_tags"
	           " WHERE CrsCod=%ld AND TagCod NOT IN"
                   " (SELECT DISTINCT tst_question_tags.TagCod"
                   " FROM tst_questions,tst_question_tags"
                   " WHERE tst_questions.CrsCod=%ld"
                   " AND tst_questions.QstCod=tst_question_tags.QstCod)",
		   CrsCod,
		   CrsCod);
  }

/*****************************************************************************/
/** Remove all media associated to stems of all test questions in a course ***/
/*****************************************************************************/

static void Tst_RemoveAllMedFilesFromStemOfAllQstsInCrs (long CrsCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumMedia;

   /***** Get media codes associated to stems of test questions from database *****/
   NumMedia =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get media",
			      "SELECT MedCod"	// row[0]
			      " FROM tst_questions"
			      " WHERE CrsCod=%ld",
			      CrsCod);

   /***** Go over result removing media files *****/
   Med_RemoveMediaFromAllRows (NumMedia,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******* Remove all media associated to all answers of a test question *******/
/*****************************************************************************/

static void Tst_RemoveMediaFromAllAnsOfQst (long CrsCod,long QstCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumMedia;

   /***** Get media codes associated to answers of test questions from database *****/
   NumMedia =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get media",
			      "SELECT tst_answers.MedCod"	// row[0]
			      " FROM tst_questions,tst_answers"
			      " WHERE tst_questions.CrsCod=%ld"	// Extra check
			      " AND tst_questions.QstCod=%ld"	// Extra check
			      " AND tst_questions.QstCod=tst_answers.QstCod"
			      " AND tst_answers.QstCod=%ld",
			      CrsCod,QstCod,QstCod);

   /***** Go over result removing media *****/
   Med_RemoveMediaFromAllRows (NumMedia,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/* Remove media associated to all answers of all test questions in a course **/
/*****************************************************************************/

static void Tst_RemoveAllMedFilesFromAnsOfAllQstsInCrs (long CrsCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumMedia;

   /***** Get names of media files associated to answers of test questions from database *****/
   NumMedia =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get media",
			      "SELECT tst_answers.MedCod"	// row[0]
			      " FROM tst_questions,tst_answers"
			      " WHERE tst_questions.CrsCod=%ld"
			      " AND tst_questions.QstCod=tst_answers.QstCod",
			      CrsCod);

   /***** Go over result removing media files *****/
   Med_RemoveMediaFromAllRows (NumMedia,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*********************** Get stats about test questions **********************/
/*****************************************************************************/

void Tst_GetTestStats (Tst_AnswerType_t AnsType,struct Tst_Stats *Stats)
  {
   Stats->NumQsts = 0;
   Stats->NumCoursesWithQuestions = Stats->NumCoursesWithPluggableQuestions = 0;
   Stats->AvgQstsPerCourse = 0.0;
   Stats->NumHits = 0L;
   Stats->AvgHitsPerCourse = 0.0;
   Stats->AvgHitsPerQuestion = 0.0;
   Stats->TotalScore = 0.0;
   Stats->AvgScorePerQuestion = 0.0;

   if (Tst_GetNumTstQuestions (Gbl.Scope.Current,AnsType,Stats))
     {
      if ((Stats->NumCoursesWithQuestions = Tst_GetNumCoursesWithTstQuestions (Gbl.Scope.Current,AnsType)) != 0)
        {
         Stats->NumCoursesWithPluggableQuestions = Tst_GetNumCoursesWithPluggableTstQuestions (Gbl.Scope.Current,AnsType);
         Stats->AvgQstsPerCourse = (double) Stats->NumQsts / (double) Stats->NumCoursesWithQuestions;
         Stats->AvgHitsPerCourse = (double) Stats->NumHits / (double) Stats->NumCoursesWithQuestions;
        }
      Stats->AvgHitsPerQuestion = (double) Stats->NumHits / (double) Stats->NumQsts;
      if (Stats->NumHits)
         Stats->AvgScorePerQuestion = Stats->TotalScore / (double) Stats->NumHits;
     }
  }

/*****************************************************************************/
/*********************** Get number of test questions ************************/
/*****************************************************************************/
// Returns the number of test questions
// in this location (all the platform, current degree or current course)

static unsigned Tst_GetNumTstQuestions (Hie_Level_t Scope,Tst_AnswerType_t AnsType,struct Tst_Stats *Stats)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get number of test questions from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),SUM(NumHits),SUM(Score)"
        	            " FROM tst_questions");
         else
            DB_QuerySELECT (&mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),SUM(NumHits),SUM(Score)"
        	            " FROM tst_questions"
                            " WHERE AnsType='%s'",
			    Tst_StrAnswerTypesDB[AnsType]);
         break;
      case Hie_CTY:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),SUM(NumHits),SUM(Score)"
        	            " FROM institutions,centres,degrees,courses,tst_questions"
                            " WHERE institutions.CtyCod=%ld"
                            " AND institutions.InsCod=centres.InsCod"
                            " AND centres.CtrCod=degrees.CtrCod"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Cty.CtyCod);
         else
            DB_QuerySELECT (&mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),SUM(NumHits),SUM(Score)"
        	            " FROM institutions,centres,degrees,courses,tst_questions"
                            " WHERE institutions.CtyCod=%ld"
                            " AND institutions.InsCod=centres.InsCod"
                            " AND centres.CtrCod=degrees.CtrCod"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.AnsType='%s'",
			    Gbl.Hierarchy.Cty.CtyCod,
			    Tst_StrAnswerTypesDB[AnsType]);
         break;
      case Hie_INS:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),SUM(NumHits),SUM(Score)"
        	            " FROM centres,degrees,courses,tst_questions"
                            " WHERE centres.InsCod=%ld"
                            " AND centres.CtrCod=degrees.CtrCod"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Ins.InsCod);
         else
            DB_QuerySELECT (&mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),SUM(NumHits),SUM(Score)"
        	            " FROM centres,degrees,courses,tst_questions"
                            " WHERE centres.InsCod=%ld"
                            " AND centres.CtrCod=degrees.CtrCod"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.AnsType='%s'",
			    Gbl.Hierarchy.Ins.InsCod,
			    Tst_StrAnswerTypesDB[AnsType]);
         break;
      case Hie_CTR:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),SUM(NumHits),SUM(Score)"
        	            " FROM degrees,courses,tst_questions"
                            " WHERE degrees.CtrCod=%ld"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Ctr.CtrCod);
         else
            DB_QuerySELECT (&mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),SUM(NumHits),SUM(Score)"
        	            " FROM degrees,courses,tst_questions"
                            " WHERE degrees.CtrCod=%ld"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.AnsType='%s'",
			    Gbl.Hierarchy.Ctr.CtrCod,
			    Tst_StrAnswerTypesDB[AnsType]);
         break;
      case Hie_DEG:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),SUM(NumHits),SUM(Score)"
        	            " FROM courses,tst_questions"
                            " WHERE courses.DegCod=%ld"
                            " AND courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Deg.DegCod);
         else
            DB_QuerySELECT (&mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),SUM(NumHits),SUM(Score)"
        	            " FROM courses,tst_questions"
                            " WHERE courses.DegCod=%ld"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.AnsType='%s'",
			    Gbl.Hierarchy.Deg.DegCod,
			    Tst_StrAnswerTypesDB[AnsType]);
         break;
      case Hie_CRS:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),SUM(NumHits),SUM(Score)"
        	            " FROM tst_questions"
                            " WHERE CrsCod=%ld",
			    Gbl.Hierarchy.Crs.CrsCod);
         else
            DB_QuerySELECT (&mysql_res,"can not get number of test questions",
        		    "SELECT COUNT(*),SUM(NumHits),SUM(Score)"
        	            " FROM tst_questions"
                            " WHERE CrsCod=%ld AND AnsType='%s'",
			    Gbl.Hierarchy.Crs.CrsCod,
			    Tst_StrAnswerTypesDB[AnsType]);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of questions *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&(Stats->NumQsts)) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of test questions.");

   if (Stats->NumQsts)
     {
      if (sscanf (row[1],"%lu",&(Stats->NumHits)) != 1)
         Lay_ShowErrorAndExit ("Error when getting total number of hits in test questions.");

      Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
      if (sscanf (row[2],"%lf",&(Stats->TotalScore)) != 1)
         Lay_ShowErrorAndExit ("Error when getting total score in test questions.");
      Str_SetDecimalPointToLocal ();	// Return to local system
     }
   else
     {
      Stats->NumHits = 0L;
      Stats->TotalScore = 0.0;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Stats->NumQsts;
  }

/*****************************************************************************/
/**************** Get number of courses with test questions ******************/
/*****************************************************************************/
// Returns the number of courses with test questions
// in this location (all the platform, current degree or current course)

static unsigned Tst_GetNumCoursesWithTstQuestions (Hie_Level_t Scope,Tst_AnswerType_t AnsType)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with test questions from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with test questions",
        		    "SELECT COUNT(DISTINCT CrsCod)"
        	            " FROM tst_questions");
         else
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with test questions",
        		    "SELECT COUNT(DISTINCT CrsCod)"
        	            " FROM tst_questions"
                            " WHERE AnsType='%s'",
			    Tst_StrAnswerTypesDB[AnsType]);
         break;
      case Hie_CTY:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with test questions",
        		    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM institutions,centres,degrees,courses,tst_questions"
                            " WHERE institutions.CtyCod=%ld"
                            " AND institutions.InsCod=centres.InsCod"
                            " AND centres.CtrCod=degrees.CtrCod"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Cty.CtyCod);
         else
           DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with test questions",
        		    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM institutions,centres,degrees,courses,tst_questions"
                            " WHERE institutions.CtyCod=%ld"
                            " AND institutions.InsCod=centres.InsCod"
                            " AND centres.CtrCod=degrees.CtrCod"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.AnsType='%s'",
			    Gbl.Hierarchy.Cty.CtyCod,
			    Tst_StrAnswerTypesDB[AnsType]);
         break;
      case Hie_INS:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with test questions",
        		    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM centres,degrees,courses,tst_questions"
                            " WHERE centres.InsCod=%ld"
                            " AND centres.CtrCod=degrees.CtrCod"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Ins.InsCod);
         else
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with test questions",
        		    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM centres,degrees,courses,tst_questions"
                            " WHERE centres.InsCod=%ld"
                            " AND centres.CtrCod=degrees.CtrCod"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.AnsType='%s'",
			    Gbl.Hierarchy.Ins.InsCod,
			    Tst_StrAnswerTypesDB[AnsType]);
         break;
      case Hie_CTR:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with test questions",
        		    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM degrees,courses,tst_questions"
                            " WHERE degrees.CtrCod=%ld"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Ctr.CtrCod);
         else
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with test questions",
        		    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM degrees,courses,tst_questions"
                            " WHERE degrees.CtrCod=%ld"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.AnsType='%s'",
			    Gbl.Hierarchy.Ctr.CtrCod,
			    Tst_StrAnswerTypesDB[AnsType]);
         break;
      case Hie_DEG:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with test questions",
        		    "SELECT COUNTDISTINCT (tst_questions.CrsCod)"
        	            " FROM courses,tst_questions"
                            " WHERE courses.DegCod=%ld"
                            " AND courses.CrsCod=tst_questions.CrsCod",
			    Gbl.Hierarchy.Deg.DegCod);
         else
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with test questions",
        		    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM courses,tst_questions"
                            " WHERE courses.DegCod=%ld"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.AnsType='%s'",
			    Gbl.Hierarchy.Deg.DegCod,
			    Tst_StrAnswerTypesDB[AnsType]);
         break;
      case Hie_CRS:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with test questions",
        		    "SELECT COUNT(DISTINCT CrsCod)"
        	            " FROM tst_questions"
                            " WHERE CrsCod=%ld",
			    Gbl.Hierarchy.Crs.CrsCod);
         else
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with test questions",
        		    "SELECT COUNT(DISTINCT CrsCod)"
			    " FROM tst_questions"
			    " WHERE CrsCod=%ld"
			    " AND AnsType='%s'",
			    Gbl.Hierarchy.Crs.CrsCod,
			    Tst_StrAnswerTypesDB[AnsType]);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of courses *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumCourses) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of courses with test questions.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCourses;
  }

/*****************************************************************************/
/*********** Get number of courses with pluggable test questions *************/
/*****************************************************************************/
// Returns the number of courses with pluggable test questions
// in this location (all the platform, current degree or current course)

static unsigned Tst_GetNumCoursesWithPluggableTstQuestions (Hie_Level_t Scope,Tst_AnswerType_t AnsType)
  {
   extern const char *TstCfg_PluggableDB[TstCfg_NUM_OPTIONS_PLUGGABLE];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with test questions from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM tst_questions,tst_config"
                            " WHERE tst_questions.CrsCod=tst_config.CrsCod"
                            " AND tst_config.pluggable='%s'",
			    TstCfg_PluggableDB[TstCfg_PLUGGABLE_YES]);
         else
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM tst_questions,tst_config"
                            " WHERE tst_questions.AnsType='%s'"
                            " AND tst_questions.CrsCod=tst_config.CrsCod"
                            " AND tst_config.pluggable='%s'",
			    Tst_StrAnswerTypesDB[AnsType],
			    TstCfg_PluggableDB[TstCfg_PLUGGABLE_YES]);
         break;
      case Hie_CTY:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM institutions,centres,degrees,courses,tst_questions,tst_config"
                            " WHERE institutions.CtyCod=%ld"
                            " AND institutions.InsCod=centres.InsCod"
                            " AND centres.CtrCod=degrees.CtrCod"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.CrsCod=tst_config.CrsCod"
                            " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Cty.CtyCod,
			    TstCfg_PluggableDB[TstCfg_PLUGGABLE_YES]);
         else
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM institutions,centres,degrees,courses,tst_questions,tst_config"
                            " WHERE institutions.CtyCod=%ld"
                            " AND institutions.InsCod=centres.InsCod"
                            " AND centres.CtrCod=degrees.CtrCod"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.AnsType='%s'"
                            " AND tst_questions.CrsCod=tst_config.CrsCod"
                            " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Cty.CtyCod,
			    Tst_StrAnswerTypesDB[AnsType],
			    TstCfg_PluggableDB[TstCfg_PLUGGABLE_YES]);
         break;
      case Hie_INS:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM centres,degrees,courses,tst_questions,tst_config"
                            " WHERE centres.InsCod=%ld"
                            " AND centres.CtrCod=degrees.CtrCod"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.CrsCod=tst_config.CrsCod"
                            " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Ins.InsCod,
			    TstCfg_PluggableDB[TstCfg_PLUGGABLE_YES]);
         else
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM centres,degrees,courses,tst_questions,tst_config"
                            " WHERE centres.InsCod=%ld"
                            " AND centres.CtrCod=degrees.CtrCod"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.AnsType='%s'"
                            " AND tst_questions.CrsCod=tst_config.CrsCod"
                            " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Ins.InsCod,
			    Tst_StrAnswerTypesDB[AnsType],
			    TstCfg_PluggableDB[TstCfg_PLUGGABLE_YES]);
         break;
      case Hie_CTR:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM degrees,courses,tst_questions,tst_config"
                            " WHERE degrees.CtrCod=%ld"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.CrsCod=tst_config.CrsCod"
                            " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Ctr.CtrCod,
			    TstCfg_PluggableDB[TstCfg_PLUGGABLE_YES]);
         else
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM degrees,courses,tst_questions,tst_config"
                            " WHERE degrees.CtrCod=%ld"
                            " AND degrees.DegCod=courses.DegCod"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.AnsType='%s'"
                            " AND tst_questions.CrsCod=tst_config.CrsCod"
                            " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Ctr.CtrCod,
			    Tst_StrAnswerTypesDB[AnsType],
			    TstCfg_PluggableDB[TstCfg_PLUGGABLE_YES]);
         break;
      case Hie_DEG:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM courses,tst_questions,tst_config"
                            " WHERE courses.DegCod=%ld"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.CrsCod=tst_config.CrsCod"
                            " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Deg.DegCod,
			    TstCfg_PluggableDB[TstCfg_PLUGGABLE_YES]);
         else
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM courses,tst_questions,tst_config"
                            " WHERE courses.DegCod=%ld"
                            " AND courses.CrsCod=tst_questions.CrsCod"
                            " AND tst_questions.AnsType='%s'"
                            " AND tst_questions.CrsCod=tst_config.CrsCod"
                            " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Deg.DegCod,
			    Tst_StrAnswerTypesDB[AnsType],
			    TstCfg_PluggableDB[TstCfg_PLUGGABLE_YES]);
         break;
      case Hie_CRS:
         if (AnsType == Tst_ANS_ALL)
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM tst_questions,tst_config"
                            " WHERE tst_questions.CrsCod=%ld"
                            " AND tst_questions.CrsCod=tst_config.CrsCod"
                            " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Crs.CrsCod,
			    TstCfg_PluggableDB[TstCfg_PLUGGABLE_YES]);
         else
            DB_QuerySELECT (&mysql_res,"can not get number of courses"
        			       " with pluggable test questions",
			    "SELECT COUNT(DISTINCT tst_questions.CrsCod)"
        	            " FROM tst_questions,tst_config"
                            " WHERE tst_questions.CrsCod=%ld"
                            " AND tst_questions.AnsType='%s'"
                            " AND tst_questions.CrsCod=tst_config.CrsCod"
                            " AND tst_config.pluggable='%s'",
			    Gbl.Hierarchy.Crs.CrsCod,
			    Tst_StrAnswerTypesDB[AnsType],
			    TstCfg_PluggableDB[TstCfg_PLUGGABLE_YES]);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of courses *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumCourses) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of courses with pluggable test questions.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCourses;
  }
