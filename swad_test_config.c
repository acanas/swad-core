// swad_test_config.c: self-assessment tests configuration

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_question.h"
#include "swad_tag_database.h"
#include "swad_test.h"
#include "swad_test_config.h"
#include "swad_test_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

struct TstCfg_Config
  {
   TstCfg_Pluggable_t Pluggable;
   unsigned Min;	// Minimum number of questions
   unsigned Def;	// Default number of questions
   unsigned Max;	// Maximum number of questions
   unsigned long MinTimeNxtTstPerQst;
   unsigned Visibility;	// One bit for each visibility item
  };

struct TstCfg_Config TstCfg_Config;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TstCfg_ShowFormConfig (void);
static void TstCfg_PutInputFieldNumQsts (const char *Field,const char *Label,
                                         unsigned Value);

static void TstCfg_GetConfigDataFromRow (MYSQL_RES *mysql_res);

static TstCfg_Pluggable_t TstCfg_GetPluggableFromForm (void);
static void TstCfg_CheckAndCorrectMinDefMax (void);

/*****************************************************************************/
/***************************** Form to rename tags ***************************/
/*****************************************************************************/

void TstCfg_CheckAndShowFormConfig (void)
  {
   extern const char *Txt_Please_specify_if_you_allow_downloading_the_question_bank_from_other_applications;

   /***** If current course has tests and pluggable is unknown... *****/
   if (TstCfg_CheckIfPluggableIsUnknownAndCrsHasTests ())
      Ale_ShowAlert (Ale_WARNING,Txt_Please_specify_if_you_allow_downloading_the_question_bank_from_other_applications);

   /***** Form to configure test *****/
   TstCfg_ShowFormConfig ();
  }

/*****************************************************************************/
/*************** Get configuration of test for current course ****************/
/*****************************************************************************/
// Returns true if course has test tags and pluggable is unknown
// Return false if course has no test tags or pluggable is known

bool TstCfg_CheckIfPluggableIsUnknownAndCrsHasTests (void)
  {
   extern const char *Tst_DB_Pluggable[TstCfg_NUM_OPTIONS_PLUGGABLE];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;
   TstCfg_Pluggable_t Pluggable;

   /***** Get pluggability of tests for current course from database *****/
   NumRows = Tst_DB_GetPluggableFromConfig (&mysql_res);

   if (NumRows == 0)
      TstCfg_SetConfigPluggable (TstCfg_PLUGGABLE_UNKNOWN);
   else // NumRows == 1
     {
      /***** Get whether test are visible via plugins or not *****/
      row = mysql_fetch_row (mysql_res);

      TstCfg_SetConfigPluggable (TstCfg_PLUGGABLE_UNKNOWN);
      for (Pluggable  = TstCfg_PLUGGABLE_NO;
	   Pluggable <= TstCfg_PLUGGABLE_YES;
	   Pluggable++)
         if (!strcmp (row[0],Tst_DB_Pluggable[Pluggable]))
           {
            TstCfg_SetConfigPluggable (Pluggable);
            break;
           }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Get if current course has tests from database *****/
   if (TstCfg_GetConfigPluggable () == TstCfg_PLUGGABLE_UNKNOWN)
      return Tag_DB_CheckIfCurrentCrsHasTestTags ();	// Return true if course has test tags

   return false;	// Pluggable is not unknown
  }

/*****************************************************************************/
/********************* Show a form to to configure test **********************/
/*****************************************************************************/

static void TstCfg_ShowFormConfig (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_configuring_tests;
   extern const char *Txt_Configure_tests;
   extern const char *Txt_Plugins;
   extern const char *Txt_TST_PLUGGABLE[TstCfg_NUM_OPTIONS_PLUGGABLE];
   extern const char *Txt_Number_of_questions;
   extern const char *Txt_minimum;
   extern const char *Txt_default;
   extern const char *Txt_maximum;
   extern const char *Txt_Minimum_time_seconds_per_question_between_two_tests;
   extern const char *Txt_Result_visibility;
   extern const char *Txt_Save_changes;
   struct Qst_Questions Questions;
   TstCfg_Pluggable_t Pluggable;
   char StrMinTimeNxtTstPerQst[Cns_MAX_DECIMAL_DIGITS_ULONG + 1];

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Read test configuration from database *****/
   TstCfg_GetConfig ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Configure_tests,
                 Tst_PutIconsTests,NULL,
                 Hlp_ASSESSMENT_Tests_configuring_tests,Box_NOT_CLOSABLE);

      /***** Begin form *****/
      Frm_BeginForm (ActRcvCfgTst);

	 /***** Tests are visible from plugins? *****/
	 HTM_TABLE_BeginCenterPadding (2);
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"RT FORM_IN_%s\"",
	                     The_GetSuffix ());
		  HTM_TxtColon (Txt_Plugins);
	       HTM_TD_End ();

	       HTM_TD_Begin ("class=\"LB\"");
		  for (Pluggable  = TstCfg_PLUGGABLE_NO;
		       Pluggable <= TstCfg_PLUGGABLE_YES;
		       Pluggable++)
		    {
		     HTM_LABEL_Begin ("class=\"DAT_%s\"",
		                      The_GetSuffix ());
			HTM_INPUT_RADIO ("Pluggable",HTM_DONT_SUBMIT_ON_CLICK,
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

	       HTM_TD_Begin ("class=\"RT FORM_IN_%s\"",
	                     The_GetSuffix ());
		  HTM_TxtColon (Txt_Number_of_questions);
	       HTM_TD_End ();

	       HTM_TD_Begin ("class=\"LB\"");
		  HTM_TABLE_BeginPadding (2);
		     TstCfg_PutInputFieldNumQsts ("NumQstMin",Txt_minimum,
					          TstCfg_GetConfigMin ());	// Minimum number of questions
		     TstCfg_PutInputFieldNumQsts ("NumQstDef",Txt_default,
					          TstCfg_GetConfigDef ());	// Default number of questions
		     TstCfg_PutInputFieldNumQsts ("NumQstMax",Txt_maximum,
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
		  snprintf (StrMinTimeNxtTstPerQst,sizeof (StrMinTimeNxtTstPerQst),"%lu",
			    TstCfg_GetConfigMinTimeNxtTstPerQst ());
		  HTM_INPUT_TEXT ("MinTimeNxtTstPerQst",Cns_MAX_DECIMAL_DIGITS_ULONG,StrMinTimeNxtTstPerQst,
				  HTM_DONT_SUBMIT_ON_CHANGE,
				  "id=\"MinTimeNxtTstPerQst\" size=\"7\""
				  " class=\"INPUT_%s\""
				  " required=\"required\"",
				  The_GetSuffix ());
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** Visibility of test prints *****/
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"RT FORM_IN_%s\"",
	                     The_GetSuffix ());
		  HTM_TxtColon (Txt_Result_visibility);
	       HTM_TD_End ();

	       HTM_TD_Begin ("class=\"LB\"");
		  TstVis_PutVisibilityCheckboxes (TstCfg_GetConfigVisibility ());
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
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/*************** Get configuration of test for current course ****************/
/*****************************************************************************/

static void TstCfg_PutInputFieldNumQsts (const char *Field,const char *Label,
                                         unsigned Value)
  {
   char StrValue[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"RM\"");
	 HTM_LABEL_Begin ("for=\"%s\" class=\"DAT_%s\"",
	                  Field,The_GetSuffix ());
	    HTM_Txt (Label);
	 HTM_LABEL_End ();
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LM\"");
	 snprintf (StrValue,sizeof (StrValue),"%u",Value);
	 HTM_INPUT_TEXT (Field,Cns_MAX_DECIMAL_DIGITS_UINT,StrValue,
			 HTM_DONT_SUBMIT_ON_CHANGE,
			 "id=\"%s\" size=\"3\" class=\"INPUT_%s\""
			 " required=\"required\"",
			 Field,The_GetSuffix ());
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/*************** Get configuration of test for current course ****************/
/*****************************************************************************/

void TstCfg_GetConfig (void)
  {
   MYSQL_RES *mysql_res;

   /***** Get configuration of test for current course from database *****/
   if (Tst_DB_GetConfig (&mysql_res,Gbl.Hierarchy.Node[HieLvl_CRS].Cod))
      TstCfg_GetConfigDataFromRow (mysql_res);
   else
     {
      TstCfg_SetConfigPluggable (TstCfg_PLUGGABLE_UNKNOWN);
      TstCfg_SetConfigMin (TstCfg_DEFAULT_MIN_QUESTIONS);
      TstCfg_SetConfigDef (TstCfg_DEFAULT_DEF_QUESTIONS);
      TstCfg_SetConfigMax (TstCfg_DEFAULT_MAX_QUESTIONS);
      TstCfg_SetConfigMinTimeNxtTstPerQst (0UL);
      TstCfg_SetConfigVisibility (TstVis_VISIBILITY_DEFAULT);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Get configuration values from a database table row *************/
/*****************************************************************************/

static void TstCfg_GetConfigDataFromRow (MYSQL_RES *mysql_res)
  {
   extern const char *Tst_DB_Pluggable[TstCfg_NUM_OPTIONS_PLUGGABLE];
   MYSQL_ROW row;
   int IntNum;
   long LongNum;
   TstCfg_Pluggable_t Pluggable;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get whether test are visible via plugins or not *****/
   TstCfg_SetConfigPluggable (TstCfg_PLUGGABLE_UNKNOWN);
   for (Pluggable  = TstCfg_PLUGGABLE_NO;
	Pluggable <= TstCfg_PLUGGABLE_YES;
	Pluggable++)
      if (!strcmp (row[0],Tst_DB_Pluggable[Pluggable]))
        {
         TstCfg_SetConfigPluggable (Pluggable);
         break;
        }

   /***** Get number of questions *****/
   if (sscanf (row[1],"%d",&IntNum) == 1)
      TstCfg_SetConfigMin ((IntNum < 1) ? 1 :
	                                  (unsigned) IntNum);
   else
      TstCfg_SetConfigMin (TstCfg_DEFAULT_MIN_QUESTIONS);

   if (sscanf (row[2],"%d",&IntNum) == 1)
      TstCfg_SetConfigDef ((IntNum < 1) ? 1 :
	                                  (unsigned) IntNum);
   else
      TstCfg_SetConfigDef (TstCfg_DEFAULT_DEF_QUESTIONS);

   if (sscanf (row[3],"%d",&IntNum) == 1)
      TstCfg_SetConfigMax ((IntNum < 1) ? 1 :
	                                  (unsigned) IntNum);
   else
      TstCfg_SetConfigMax (TstCfg_DEFAULT_MAX_QUESTIONS);

   /***** Check and correct numbers *****/
   TstCfg_CheckAndCorrectMinDefMax ();

   /***** Get minimum time between consecutive tests, per question (row[4]) *****/
   if (sscanf (row[4],"%ld",&LongNum) == 1)
      TstCfg_SetConfigMinTimeNxtTstPerQst ((LongNum < 1L) ? 0UL :
	                                                    (unsigned long) LongNum);
   else
      TstCfg_SetConfigMinTimeNxtTstPerQst (0UL);

   /***** Get visibility (row[5]) *****/
   TstCfg_SetConfigVisibility (TstVis_GetVisibilityFromStr (row[5]));
  }

/*****************************************************************************/
/************* Receive configuration of test for current course **************/
/*****************************************************************************/

void TstCfg_ReceiveConfigTst (void)
  {
   extern const char *Txt_The_test_configuration_has_been_updated;

   /***** Get whether test are visible via plugins or not *****/
   TstCfg_SetConfigPluggable (TstCfg_GetPluggableFromForm ());

   /***** Get number of questions *****/
   /* Get minimum number of questions */
   TstCfg_SetConfigMin ((unsigned)
	                Par_GetParUnsignedLong ("NumQstMin",
	                                        1,
	                                        UINT_MAX,
	                                        1));

   /* Get default number of questions */
   TstCfg_SetConfigDef ((unsigned)
	                Par_GetParUnsignedLong ("NumQstDef",
	                                        1,
	                                        UINT_MAX,
	                                        1));

   /* Get maximum number of questions */
   TstCfg_SetConfigMax ((unsigned)
	                Par_GetParUnsignedLong ("NumQstMax",
	                                        1,
	                                        UINT_MAX,
	                                        1));

   /* Check and correct numbers */
   TstCfg_CheckAndCorrectMinDefMax ();

   /***** Get minimum time between consecutive tests, per question *****/
   TstCfg_SetConfigMinTimeNxtTstPerQst (Par_GetParUnsignedLong ("MinTimeNxtTstPerQst",
                                                                0,
                                                                ULONG_MAX,
                                                                0));

   /***** Get visibility from form *****/
   TstCfg_SetConfigVisibility (TstVis_GetVisibilityFromForm ());

   /***** Update database *****/
   Tst_DB_SaveConfig ();

   /***** Show confirmation message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_test_configuration_has_been_updated);

   /***** Show again the form to configure test *****/
   TstCfg_CheckAndShowFormConfig ();
  }

/*****************************************************************************/
/******************* Get if tests are pluggable from form ********************/
/*****************************************************************************/

static TstCfg_Pluggable_t TstCfg_GetPluggableFromForm (void)
  {
   return (TstCfg_Pluggable_t)
	  Par_GetParUnsignedLong ("Pluggable",
	                          0,
                                  TstCfg_NUM_OPTIONS_PLUGGABLE - 1,
                                  (unsigned long) TstCfg_PLUGGABLE_UNKNOWN);
  }

/*****************************************************************************/
/**** Check and correct minimum, default and maximum numbers of questions ****/
/*****************************************************************************/

static void TstCfg_CheckAndCorrectMinDefMax (void)
  {
   /***** Check if minimum is correct *****/
   if (TstCfg_GetConfigMin () < 1)
      TstCfg_SetConfigMin (1);
   else if (TstCfg_GetConfigMin () > TstCfg_MAX_QUESTIONS_PER_TEST)
      TstCfg_SetConfigMin (TstCfg_MAX_QUESTIONS_PER_TEST);

   /***** Check if maximum is correct *****/
   if (TstCfg_GetConfigMax () < 1)
      TstCfg_SetConfigMax (1);
   else if (TstCfg_GetConfigMax () > TstCfg_MAX_QUESTIONS_PER_TEST)
      TstCfg_SetConfigMax (TstCfg_MAX_QUESTIONS_PER_TEST);

   /***** Check if minimum is lower than maximum *****/
   if (TstCfg_GetConfigMin () > TstCfg_GetConfigMax ())
      TstCfg_SetConfigMin (TstCfg_GetConfigMax ());

   /***** Check if default is correct *****/
   if (TstCfg_GetConfigDef () < TstCfg_GetConfigMin ())
      TstCfg_SetConfigDef (TstCfg_GetConfigMin ());
   else if (TstCfg_GetConfigDef () > TstCfg_GetConfigMax ())
      TstCfg_SetConfigDef (TstCfg_GetConfigMax ());
  }

/*****************************************************************************/
/********* Get fields of current test configuration for this course **********/
/*****************************************************************************/

void TstCfg_SetConfigPluggable (TstCfg_Pluggable_t Pluggable)
  {
   TstCfg_Config.Pluggable = Pluggable;
  }

TstCfg_Pluggable_t TstCfg_GetConfigPluggable (void)
  {
   return TstCfg_Config.Pluggable;
  }

void TstCfg_SetConfigMin (unsigned Min)
  {
   TstCfg_Config.Min = Min;
  }

unsigned TstCfg_GetConfigMin (void)
  {
   return TstCfg_Config.Min;
  }

void TstCfg_SetConfigDef (unsigned Def)
  {
   TstCfg_Config.Def = Def;
  }

unsigned TstCfg_GetConfigDef (void)
  {
   return TstCfg_Config.Def;
  }

void TstCfg_SetConfigMax (unsigned Max)
  {
   TstCfg_Config.Max = Max;
  }

unsigned TstCfg_GetConfigMax (void)
  {
   return TstCfg_Config.Max;
  }

void TstCfg_SetConfigMinTimeNxtTstPerQst (unsigned long MinTimeNxtTstPerQst)
  {
   TstCfg_Config.MinTimeNxtTstPerQst = MinTimeNxtTstPerQst;
  }

unsigned long TstCfg_GetConfigMinTimeNxtTstPerQst (void)
  {
   return TstCfg_Config.MinTimeNxtTstPerQst;
  }

void TstCfg_SetConfigVisibility (unsigned Visibility)
  {
   TstCfg_Config.Visibility = Visibility;
  }

unsigned TstCfg_GetConfigVisibility (void)
  {
   return TstCfg_Config.Visibility;
  }
