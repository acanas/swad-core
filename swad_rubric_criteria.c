// swad_rubric_criteria.c: criteria in assessment rubrics
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <float.h>		// For DBL_MAX
#include <mysql/mysql.h>	// To access MySQL databases
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_assignment_resource.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_exam_resource.h"
#include "swad_form.h"
#include "swad_game_resource.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_project_database.h"
#include "swad_resource_database.h"
#include "swad_rubric.h"
#include "swad_rubric_criteria.h"
#include "swad_rubric_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

// Form parameters for minimum/maximum criterion values
static const char *RubCri_ParValues[RubCri_NUM_VALUES] =
  {
   [RubCri_MIN] = "MinVal",
   [RubCri_MAX] = "MaxVal",
  };
#define RubCri_SCORE_STEP	0.1

#define RubCri_WEIGHT_MIN	0.0
#define RubCri_WEIGHT_MAX	1.0
#define RubCri_WEIGHT_STEP	0.000001

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void RubCri_PutParsOneCriterion (void *Rubrics);

static void RubCri_PutFormNewCriterion (struct Rub_Rubrics *Rubrics,
				        unsigned MaxCriInd);
static void RubCri_ReceiveCriterionFieldsFromForm (struct RubCri_Criterion *Criterion);
static bool RubCri_CheckCriterionTitleReceivedFromForm (const struct RubCri_Criterion *Criterion,
                                                        const char NewTitle[RubCri_MAX_BYTES_TITLE + 1]);

static void RubCri_ChangeValueCriterion (RubCri_ValueRange_t ValueRange);

static void RubCri_CreateCriterion (struct RubCri_Criterion *Criterion);

static void RubCri_ListOneOrMoreCriteriaForSeeing (unsigned NumCriteria,
                                                   MYSQL_RES *mysql_res);
static void RubCri_ListOneOrMoreCriteriaForEdition (struct Rub_Rubrics *Rubrics,
					            unsigned MaxCriInd,
					            unsigned NumCriteria,
                                                    MYSQL_RES *mysql_res);
static void RubCri_ListOneOrMoreCriteriaInProject (struct Prj_Projects *Projects,
                                                   struct Rub_Node **TOS,
                                                   bool ICanFill,
					           unsigned NumCriteria,
                                                   MYSQL_RES *mysql_res);
static void RubCri_WriteIndex (const struct RubCri_Criterion *Criterion,
                               const char *Anchor);
static void RubCri_WriteTitle (const struct RubCri_Criterion *Criterion);
static void RubCri_WriteLink (const struct RubCri_Criterion *Criterion);
static void RubCri_WriteMinimumMaximum (const struct RubCri_Criterion *Criterion);
static void RubCri_WriteTxt (const char *Txt);
static void RubCri_WriteWeight (const struct RubCri_Criterion *Criterion);
static void RubCri_WriteTotalLabel (unsigned ColSpan);
static void RubCri_WriteTotalEmpty (void);
static void RubCri_WriteTotalValue (double Total);

static bool RubCri_ComputeRubricScore (Rsc_Type_t Type,long Cod,long UsrCod,
                                       struct Rub_Node **TOS,long RubCod,
                                       double *RubricScore);

static void RubCri_PutTableHeadingForSeeing (void);
static void RubCri_PutTableHeadingForEdition (void);
static void RubCri_PutTableHeadingForScoring (void);

static void RubCri_GetAndCheckRubricAndCriterion (struct Rub_Rubrics *Rubrics);

static void RubCri_ExchangeCriteria (long RubCod,
                                     unsigned CriIndTop,unsigned CriIndBottom);

/*****************************************************************************/
/*************** Put parameter to edit one rubric criterion ******************/
/*****************************************************************************/

static void RubCri_PutParsOneCriterion (void *Rubrics)
  {
   if (Rubrics)
     {
      Rub_PutPars (Rubrics);
      ParCod_PutPar (ParCod_Cri,((struct Rub_Rubrics *) Rubrics)->Criterion.CriCod);
     }
  }

/*****************************************************************************/
/******************** Get criterion data using its code **********************/
/*****************************************************************************/

void RubCri_GetCriterionDataByCod (struct RubCri_Criterion *Criterion)
  {
   MYSQL_RES *mysql_res;

   /***** Trivial check *****/
   if (Criterion->CriCod <= 0)
     {
      /* Initialize to empty criterion */
      RubCri_ResetCriterion (Criterion);
      return;
     }

   /***** Get data of rubric criterion from database *****/
   if (Rub_DB_GetCriterionDataByCod (&mysql_res,Criterion->CriCod)) // Criterion found...
      RubCri_GetCriterionDataFromRow (mysql_res,Criterion);
   else
      /* Initialize to empty criterion */
      RubCri_ResetCriterion (Criterion);

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************** Put a form to create/edit a rubric criterion ****************/
/*****************************************************************************/

static void RubCri_PutFormNewCriterion (struct Rub_Rubrics *Rubrics,
				        unsigned MaxCriInd)
  {
   RubCri_ValueRange_t ValueRange;

   /***** Begin form to create *****/
   Frm_BeginFormTable (ActNewRubCri,NULL,Rub_PutPars,Rubrics);

      /***** Table heading *****/
      RubCri_PutTableHeadingForEdition ();

      /***** Begin row *****/
      HTM_TR_Begin (NULL);

	 /***** Icons *****/
	 HTM_TD_Begin ("class=\"BT\"");
	 HTM_TD_End ();

	 /***** Index *****/
	 HTM_TD_Begin ("class=\"RT\"");
	    Lay_WriteIndex (MaxCriInd + 1,"BIG_INDEX");
	 HTM_TD_End ();

	 /***** Title *****/
	 HTM_TD_Begin ("class=\"LT\"");
	    HTM_INPUT_TEXT ("Title",RubCri_MAX_CHARS_TITLE,Rubrics->Criterion.Title,
			    HTM_DONT_SUBMIT_ON_CHANGE,
			    "id=\"Title\""
			    " class=\"TITLE_DESCRIPTION_WIDTH INPUT_%s\""
			    " required=\"required\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Link *****/
	 HTM_TD_Begin ("class=\"LT\"");
	    Rsc_ShowClipboardToChangeLink (NULL);
	 HTM_TD_End ();

	 /***** Minimum and maximum values of the criterion *****/
	 for (ValueRange  = (RubCri_ValueRange_t) 0;
	      ValueRange <= (RubCri_ValueRange_t) (RubCri_NUM_VALUES - 1);
	      ValueRange++)
	   {
	    HTM_TD_Begin ("class=\"RT\"");
	       HTM_INPUT_FLOAT (RubCri_ParValues[ValueRange],
				0.0,DBL_MAX,RubCri_SCORE_STEP,
				Rubrics->Criterion.Values[ValueRange],
				HTM_DONT_SUBMIT_ON_CHANGE,false,
				" class=\"INPUT_FLOAT INPUT_%s\" required=\"required\"",
				The_GetSuffix ());
	    HTM_TD_End ();
	   }

	 /***** Weight *****/
	 HTM_TD_Begin ("class=\"RT\"");
	    HTM_INPUT_FLOAT ("Weight",
			     RubCri_WEIGHT_MIN,
			     RubCri_WEIGHT_MAX,
			     RubCri_WEIGHT_STEP,
			     Rubrics->Criterion.Weight,
			     HTM_DONT_SUBMIT_ON_CHANGE,false,
			     " class=\"INPUT_FLOAT INPUT_%s\" required=\"required\"",
			     The_GetSuffix ());
	 HTM_TD_End ();

      /***** End row *****/
      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE_BUTTON);
  }

/*****************************************************************************/
/**************** Receive form to create a new rubric criterion **************/
/*****************************************************************************/

void RubCri_ReceiveFormCriterion (void)
  {
   struct Rub_Rubrics Rubrics;

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);
   Rub_RubricConstructor (&Rubrics.Rubric);
   RubCri_ResetCriterion (&Rubrics.Criterion);

   /***** Get parameters *****/
   Rub_GetPars (&Rubrics,true);
   Rubrics.Criterion.RubCod = Rubrics.Rubric.RubCod;

   /***** Get rubric data from database *****/
   Rub_GetRubricDataByCod (&Rubrics.Rubric);

   /***** Check if rubric is editable *****/
   if (!Rub_CheckIfEditable ())
      Err_NoPermissionExit ();

   /***** If I can edit rubrics ==> receive criterion from form *****/
   RubCri_ReceiveCriterionFieldsFromForm (&Rubrics.Criterion);

   if (RubCri_CheckCriterionTitleReceivedFromForm (&Rubrics.Criterion,Rubrics.Criterion.Title))
      RubCri_CreateCriterion (&Rubrics.Criterion);	// Add new criterion to database

   /***** Show current rubric and its criteria *****/
   Rub_PutFormsOneRubric (&Rubrics,Rub_EXISTING_RUBRIC);

   /***** Free memory used for rubric *****/
   Rub_RubricDestructor (&Rubrics.Rubric);
  }

static void RubCri_ReceiveCriterionFieldsFromForm (struct RubCri_Criterion *Criterion)
  {
   RubCri_ValueRange_t ValueRange;
   char ValueStr[64];
   char WeightStr[64];

   /***** Get criterion title *****/
   Par_GetParText ("Title",Criterion->Title,RubCri_MAX_BYTES_TITLE);

   /***** Get criterion link to resource *****/
   if (Rsc_GetParLink (&Criterion->Link))
      /***** Remove link from clipboard *****/
      Rsc_DB_RemoveLinkFromClipboard (&Criterion->Link);

   /***** Get minimum and maximum values of criterion *****/
   for (ValueRange  = (RubCri_ValueRange_t) 0;
	ValueRange <= (RubCri_ValueRange_t) (RubCri_NUM_VALUES - 1);
	ValueRange++)
     {
      Par_GetParText (RubCri_ParValues[ValueRange],ValueStr,sizeof (ValueStr) - 1);
      Criterion->Values[ValueRange] = Str_GetDoubleFromStr (ValueStr);
     }

   /***** Get criterion weight *****/
   Par_GetParText ("Weight",WeightStr,sizeof (WeightStr) - 1);
   Criterion->Weight = Str_GetDoubleFromStr (WeightStr);
  }

static bool RubCri_CheckCriterionTitleReceivedFromForm (const struct RubCri_Criterion *Criterion,
                                                        const char NewTitle[RubCri_MAX_BYTES_TITLE + 1])
  {
   extern const char *Txt_Already_existed_a_criterion_in_this_rubric_with_the_title_X;
   bool NewTitleIsCorrect;

   /***** Check if title is correct *****/
   NewTitleIsCorrect = true;
   if (NewTitle[0])	// If there's an criterion title
     {
      /***** Check if old and new titles are the same
	     (this happens when return is pressed without changes) *****/
      if (strcmp (Criterion->Title,NewTitle))	// Different titles
	{
	 /* If title of criterion was in database... */
	 if (Rub_DB_CheckIfSimilarCriterionExists (Criterion,NewTitle))
	   {
	    NewTitleIsCorrect = false;
	    Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_a_criterion_in_this_rubric_with_the_title_X,
			   Criterion->Title);
	   }
	}
     }
   else	// If there is not a criterion title
     {
      NewTitleIsCorrect = false;
      Ale_CreateAlertYouMustSpecifyTheTitle ();
     }

   return NewTitleIsCorrect;
  }

/*****************************************************************************/
/************* Receive form to change title of rubric criterion **************/
/*****************************************************************************/

void RubCri_ChangeTitle (void)
  {
   struct Rub_Rubrics Rubrics;
   char NewTitle[RubCri_MAX_BYTES_TITLE + 1];

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);
   Rub_RubricConstructor (&Rubrics.Rubric);
   RubCri_ResetCriterion (&Rubrics.Criterion);

   /***** Get and check parameters *****/
   RubCri_GetAndCheckRubricAndCriterion (&Rubrics);

   /***** Check if rubric is editable *****/
   if (!Rub_CheckIfEditable ())
     Err_NoPermissionExit ();

   /***** Receive new title from form *****/
   Par_GetParText ("Title",NewTitle,RubCri_MAX_BYTES_TITLE);

   /***** Check if title should be changed *****/
   if (RubCri_CheckCriterionTitleReceivedFromForm (&Rubrics.Criterion,NewTitle))
     {
      /* Update title and database table */
      Str_Copy (Rubrics.Criterion.Title,NewTitle,sizeof (Rubrics.Criterion.Title) - 1);
      Rub_DB_UpdateCriterionTitle (&Rubrics.Criterion);
     }

   /***** Show current rubric and its criteria *****/
   Rub_PutFormsOneRubric (&Rubrics,Rub_EXISTING_RUBRIC);

   /***** Free memory used for rubric *****/
   Rub_RubricDestructor (&Rubrics.Rubric);
  }

/*****************************************************************************/
/********* Receive form to change minimum/maximum value of criterion *********/
/*****************************************************************************/

void RubCri_ChangeMinValue (void)
  {
   RubCri_ChangeValueCriterion (RubCri_MIN);
  }

void RubCri_ChangeMaxValue (void)
  {
   RubCri_ChangeValueCriterion (RubCri_MAX);
  }

static void RubCri_ChangeValueCriterion (RubCri_ValueRange_t ValueRange)
  {
   struct Rub_Rubrics Rubrics;
   char ValueStr[64];

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);
   Rub_RubricConstructor (&Rubrics.Rubric);
   RubCri_ResetCriterion (&Rubrics.Criterion);

   /***** Get parameters *****/
   Rub_GetPars (&Rubrics,true);
   Rubrics.Criterion.RubCod = Rubrics.Rubric.RubCod;
   Rubrics.Criterion.CriCod = ParCod_GetAndCheckPar (ParCod_Cri);

   /***** Get and check parameters *****/
   RubCri_GetAndCheckRubricAndCriterion (&Rubrics);

   /***** Check if rubric is editable *****/
   if (!Rub_CheckIfEditable ())
      Err_NoPermissionExit ();

   /***** Receive new value from form *****/
   Par_GetParText (RubCri_ParValues[ValueRange],ValueStr,sizeof (ValueStr) - 1);
   Rubrics.Criterion.Values[ValueRange] = Str_GetDoubleFromStr (ValueStr);

   /***** Change value *****/
   /* Update the table changing old value by new value */
   Rub_DB_UpdateCriterionValue (&Rubrics.Criterion,ValueRange);

   /***** Show current rubric and its criteria *****/
   Rub_PutFormsOneRubric (&Rubrics,Rub_EXISTING_RUBRIC);

   /***** Free memory used for rubric *****/
   Rub_RubricDestructor (&Rubrics.Rubric);
  }

/*****************************************************************************/
/****************** Receive form to change criterion weight ******************/
/*****************************************************************************/

void RubCri_ChangeWeight (void)
  {
   struct Rub_Rubrics Rubrics;
   char WeightStr[64];

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);
   Rub_RubricConstructor (&Rubrics.Rubric);
   RubCri_ResetCriterion (&Rubrics.Criterion);

   /***** Get parameters *****/
   Rub_GetPars (&Rubrics,true);
   Rubrics.Criterion.RubCod = Rubrics.Rubric.RubCod;
   Rubrics.Criterion.CriCod = ParCod_GetAndCheckPar (ParCod_Cri);

   /***** Get and check parameters *****/
   RubCri_GetAndCheckRubricAndCriterion (&Rubrics);

   /***** Check if rubric is editable *****/
   if (!Rub_CheckIfEditable ())
      Err_NoPermissionExit ();

   /***** Receive new weight from form *****/
   Par_GetParText ("Weight",WeightStr,sizeof (WeightStr) - 1);
   Rubrics.Criterion.Weight = Str_GetDoubleFromStr (WeightStr);

   /***** Change value *****/
   /* Update the table changing old weight by new weight */
   Rub_DB_UpdateCriterionWeight (&Rubrics.Criterion);

   /***** Show current rubric and its criteria *****/
   Rub_PutFormsOneRubric (&Rubrics,Rub_EXISTING_RUBRIC);

   /***** Free memory used for rubric *****/
   Rub_RubricDestructor (&Rubrics.Rubric);
  }

/*****************************************************************************/
/************************ Create a new rubric criterion **********************/
/*****************************************************************************/

static void RubCri_CreateCriterion (struct RubCri_Criterion *Criterion)
  {
   extern const char *Txt_Created_new_criterion_X;
   unsigned MaxCriInd;

   /***** Get maximum criterion index *****/
   MaxCriInd = Rub_DB_GetMaxCriterionIndexInRubric (Criterion->RubCod);

   /***** Create a new criterion *****/
   Criterion->CriInd = MaxCriInd + 1;
   Criterion->CriCod = Rub_DB_CreateCriterion (Criterion);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_criterion_X,
                  Criterion->Title);
  }

/*****************************************************************************/
/***************** List the criteria of a rubric for seeing ******************/
/*****************************************************************************/

void RubCri_ListCriteriaForSeeing (const struct Rub_Rubrics *Rubrics)
  {
   extern const char *Hlp_ASSESSMENT_Rubrics_criteria;
   extern const char *Txt_Criteria;
   MYSQL_RES *mysql_res;
   unsigned NumCriteria;

   /***** Get data of rubric criteria from database *****/
   NumCriteria = Rub_DB_GetCriteria (&mysql_res,Rubrics->Rubric.RubCod);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Criteria,
		 NULL,NULL,
		 Hlp_ASSESSMENT_Rubrics_criteria,Box_NOT_CLOSABLE);

      /***** Show table with rubric criteria *****/
      if (NumCriteria)
         RubCri_ListOneOrMoreCriteriaForSeeing (NumCriteria,mysql_res);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************** List the criteria of a rubric for edition ******************/
/*****************************************************************************/

void RubCri_ListCriteriaForEdition (struct Rub_Rubrics *Rubrics)
  {
   extern const char *Hlp_ASSESSMENT_Rubrics_criteria;
   extern const char *Txt_Criteria;
   MYSQL_RES *mysql_res;
   unsigned MaxCriInd;
   unsigned NumCriteria;
   bool ICanEditCriteria = Rub_CheckIfEditable ();

   /***** Get maximum criterion index *****/
   MaxCriInd = Rub_DB_GetMaxCriterionIndexInRubric (Rubrics->Rubric.RubCod);

   /***** Get data of rubric criteria from database *****/
   NumCriteria = Rub_DB_GetCriteria (&mysql_res,Rubrics->Rubric.RubCod);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Criteria,
		 NULL,NULL,
		 Hlp_ASSESSMENT_Rubrics_criteria,Box_NOT_CLOSABLE);

      /***** Show table with rubric criteria *****/
      if (NumCriteria)
	 RubCri_ListOneOrMoreCriteriaForEdition (Rubrics,
						 MaxCriInd,
						 NumCriteria,mysql_res);

      /***** Put forms to create/edit a criterion *****/
      if (ICanEditCriteria)
	 RubCri_PutFormNewCriterion (Rubrics,MaxCriInd);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************** List the criteria of a rubric in a project *****************/
/*****************************************************************************/

void RubCri_ListCriteriaInProject (struct Prj_Projects *Projects,long RubCod,
				   bool ICanFill)
  {
   MYSQL_RES *mysql_res;
   unsigned NumCriteria;
   struct Rub_Node *TOS;

   /***** Get data of rubric criteria from database *****/
   NumCriteria = Rub_DB_GetCriteria (&mysql_res,RubCod);

   /***** Show table with rubric criteria *****/
   if (NumCriteria)
     {
      /* Push rubric code in stack */
      Rub_PushRubCod (&TOS,RubCod);

      /* List rubric criteria */
      RubCri_ListOneOrMoreCriteriaInProject (Projects,&TOS,ICanFill,
                                             NumCriteria,mysql_res);

      /* Pop rubric code from stack */
      Rub_PopRubCod (&TOS);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** List rubric criteria for edition *********************/
/*****************************************************************************/

static void RubCri_ListOneOrMoreCriteriaForSeeing (unsigned NumCriteria,
                                                   MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Criteria;
   unsigned NumCriterion;
   struct RubCri_Criterion Criterion;
   double SumOfWeights = 0.0;

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (5);

      /***** Write the heading *****/
      RubCri_PutTableHeadingForSeeing ();

      /***** Write rows *****/
      for (NumCriterion = 0, The_ResetRowColor ();
	   NumCriterion < NumCriteria;
	   NumCriterion++, The_ChangeRowColor ())
	{
	 /***** Reset criterion *****/
	 RubCri_ResetCriterion (&Criterion);

	 /***** Get criterion data *****/
	 RubCri_GetCriterionDataFromRow (mysql_res,&Criterion);
	 SumOfWeights += Criterion.Weight;

	 /***** Begin row *****/
	 HTM_TR_Begin (NULL);

	    /***** Index, title, link, minimum/maximum values
	           and weight of criterion *****/
	    RubCri_WriteIndex (&Criterion,NULL);
	    RubCri_WriteTitle (&Criterion);
	    RubCri_WriteLink (&Criterion);
	    RubCri_WriteMinimumMaximum (&Criterion);
	    RubCri_WriteWeight (&Criterion);

	 /***** End row *****/
	 HTM_TR_End ();
	}

      /***** Write total row *****/
      HTM_TR_Begin (NULL);
	 RubCri_WriteTotalLabel (5);
	 RubCri_WriteTotalValue (SumOfWeights);
      HTM_TR_End ();

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********************** List rubric criteria for edition *********************/
/*****************************************************************************/

static void RubCri_ListOneOrMoreCriteriaForEdition (struct Rub_Rubrics *Rubrics,
					            unsigned MaxCriInd,
					            unsigned NumCriteria,
                                                    MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Criteria;
   extern const char *Txt_Movement_not_allowed;
   // Actions to change minimum/maximum criterion values
   static Act_Action_t RubCri_ActionsValues[RubCri_NUM_VALUES] =
     {
      [RubCri_MIN] = ActChgMinRubCri,
      [RubCri_MAX] = ActChgMaxRubCri,
     };
   unsigned NumCriterion;
   char *Anchor;
   RubCri_ValueRange_t ValueRange;
   double SumOfWeights = 0.0;

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (5);

      /***** Write the heading *****/
      RubCri_PutTableHeadingForEdition ();

      /***** Write rows *****/
      for (NumCriterion = 0, The_ResetRowColor ();
	   NumCriterion < NumCriteria;
	   NumCriterion++, The_ChangeRowColor ())
	{
	 /***** Get criterion data *****/
	 RubCri_GetCriterionDataFromRow (mysql_res,&Rubrics->Criterion);
	 SumOfWeights += Rubrics->Criterion.Weight;

	 /***** Build anchor string *****/
	 Frm_SetAnchorStr (Rubrics->Criterion.CriCod,&Anchor);

	 /***** Begin row *****/
	 HTM_TR_Begin (NULL);

	    /***** Icons *****/
	    HTM_TD_Begin ("class=\"BT %s\"",The_GetColorRows ());

	       /* Put icon to remove the criterion */
	       Ico_PutContextualIconToRemove (ActReqRemRubCri,NULL,
					      RubCri_PutParsOneCriterion,Rubrics);

	       /* Put icon to move up the question */
	       if (Rubrics->Criterion.CriInd > 1)
		  Lay_PutContextualLinkOnlyIcon (ActUp_RubCri,Anchor,
						 RubCri_PutParsOneCriterion,Rubrics,
						 "arrow-up.svg",Ico_BLACK);
	       else
		  Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,
		                  Txt_Movement_not_allowed);

	       /* Put icon to move down the criterion */
	       if (Rubrics->Criterion.CriInd < MaxCriInd)
		  Lay_PutContextualLinkOnlyIcon (ActDwnRubCri,Anchor,
						 RubCri_PutParsOneCriterion,Rubrics,
						 "arrow-down.svg",Ico_BLACK);
	       else
		  Ico_PutIconOff ("arrow-down.svg",Ico_BLACK,
		                  Txt_Movement_not_allowed);

	    HTM_TD_End ();

	    /***** Index *****/
	    RubCri_WriteIndex (&Rubrics->Criterion,Anchor);

	    /***** Title *****/
	    HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
		  Frm_BeginFormAnchor (ActChgTitRubCri,Anchor);
		     RubCri_PutParsOneCriterion (Rubrics);
		     HTM_INPUT_TEXT ("Title",RubCri_MAX_CHARS_TITLE,Rubrics->Criterion.Title,
				     HTM_SUBMIT_ON_CHANGE,
				     "id=\"Title\""
				     " class=\"TITLE_DESCRIPTION_WIDTH INPUT_%s\""
				     " required=\"required\"",
				     The_GetSuffix ());
		  Frm_EndForm ();
	    HTM_TD_End ();

	    /***** Link to resource *****/
	    HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
	       Frm_BeginFormAnchor (ActChgLnkRubCri,Anchor);
		  RubCri_PutParsOneCriterion (Rubrics);
		  Rsc_ShowClipboardToChangeLink (&Rubrics->Criterion.Link);
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /***** Minimum and maximum values of criterion *****/
	    for (ValueRange  = (RubCri_ValueRange_t) 0;
		 ValueRange <= (RubCri_ValueRange_t) (RubCri_NUM_VALUES - 1);
		 ValueRange++)
	      {
	       HTM_TD_Begin ("class=\"RT %s\"",The_GetColorRows ());
		  Frm_BeginFormAnchor (RubCri_ActionsValues[ValueRange],Anchor);
		     RubCri_PutParsOneCriterion (Rubrics);
		     HTM_INPUT_FLOAT (RubCri_ParValues[ValueRange],
		                      0.0,DBL_MAX,RubCri_SCORE_STEP,
				      Rubrics->Criterion.Values[ValueRange],
				      HTM_SUBMIT_ON_CHANGE,false,
				      " class=\"INPUT_FLOAT INPUT_%s\""
				      " required=\"required\"",
				      The_GetSuffix ());
		  Frm_EndForm ();
	       HTM_TD_End ();
	      }

	    /***** Criterion weight *****/
	    HTM_TD_Begin ("class=\"RT %s\"",The_GetColorRows ());
	       Frm_BeginFormAnchor (ActChgWeiRubCri,Anchor);
		  RubCri_PutParsOneCriterion (Rubrics);
		  HTM_INPUT_FLOAT ("Weight",
				   RubCri_WEIGHT_MIN,
				   RubCri_WEIGHT_MAX,
				   RubCri_WEIGHT_STEP,
				   Rubrics->Criterion.Weight,
				   HTM_SUBMIT_ON_CHANGE,false,
				   " class=\"INPUT_FLOAT INPUT_%s\""
				   " required=\"required\"",
				   The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	 /***** End row *****/
	 HTM_TR_End ();

	 /***** Free anchor string *****/
	 Frm_FreeAnchorStr (&Anchor);
	}

      /***** Write total row *****/
      HTM_TR_Begin (NULL);
	 RubCri_WriteTotalLabel (6);
         RubCri_WriteTotalValue (SumOfWeights);
      HTM_TR_End ();

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********************* List rubric criteria in a project *********************/
/*****************************************************************************/

static void RubCri_ListOneOrMoreCriteriaInProject (struct Prj_Projects *Projects,
                                                   struct Rub_Node **TOS,
                                                   bool ICanFill,
					           unsigned NumCriteria,
                                                   MYSQL_RES *mysql_res)
  {
   extern const char *Txt_Criteria;
   struct RubCri_Criterion Criterion;
   unsigned NumCriterion;
   char *Anchor;
   double CriterionScore;
   double WeightedScore;
   double SumOfWeights = 0.0;
   double SumOfScores  = 0.0;
   double WeightedSum  = 0.0;

   /***** Write the heading *****/
   RubCri_PutTableHeadingForScoring ();

   /***** Write rows *****/
   for (NumCriterion = 0, The_ResetRowColor ();
	NumCriterion < NumCriteria;
	NumCriterion++, The_ChangeRowColor ())
     {
      /***** Get criterion data *****/
      RubCri_GetCriterionDataFromRow (mysql_res,&Criterion);

      /***** Build anchor string *****/
      Frm_SetAnchorStr (Criterion.CriCod,&Anchor);

      /***** Begin row *****/
      HTM_TR_Begin (NULL);

	 /***** Index, title, link *****/
	 RubCri_WriteIndex (&Criterion,Anchor);
	 RubCri_WriteTitle (&Criterion);
         RubCri_WriteLink (&Criterion);

	 /***** Criterion score *****/
	 HTM_TD_Begin ("class=\"RT NO_BR %s\"",The_GetColorRows ());

	    /* Minimum value */
	    HTM_SPAN_Begin ("class=\"DAT_LIGHT_%s\"",The_GetSuffix ());
	       HTM_Double (Criterion.Values[RubCri_MIN]);
	       HTM_Txt ("&le;");
	       HTM_NBSP ();
	    HTM_SPAN_End ();

	    switch (Criterion.Link.Type)
	      {
	       case Rsc_NONE:
		  CriterionScore = Rub_DB_GetScore (Rsc_PROJECT,Projects->Prj.PrjCod,-1L,Criterion.CriCod);
		  if (ICanFill)
		    {
		     Frm_BeginFormAnchor (ActChgPrjSco,Anchor);
			Prj_PutCurrentPars (Projects);
			ParCod_PutPar (ParCod_Cri,Criterion.CriCod);
			HTM_INPUT_FLOAT ("Score",
					 Criterion.Values[RubCri_MIN],
					 Criterion.Values[RubCri_MAX],
					 RubCri_SCORE_STEP,
					 CriterionScore,
					 HTM_SUBMIT_ON_CHANGE,false,
					 " class=\"INPUT_FLOAT INPUT_%s\""
					 " required=\"required\"",
					 The_GetSuffix ());
		     Frm_EndForm ();
		    }
		  else
		    {
		     HTM_SPAN_Begin ("class=\"DAT_%s\"",The_GetSuffix ());
			HTM_Double2Decimals (CriterionScore);
		     HTM_SPAN_End ();
		    }
		  break;
	       case Rsc_RUBRIC:
		  if (RubCri_ComputeRubricScore (Rsc_PROJECT,Projects->Prj.PrjCod,-1L,
						 TOS,Criterion.Link.Cod,
						 &CriterionScore))
		     Err_RecursiveRubric ();
		  else
		    {
		     HTM_SPAN_Begin ("class=\"DAT_%s\"",The_GetSuffix ());
			HTM_Double2Decimals (CriterionScore);
		     HTM_SPAN_End ();
		    }
		  break;
	       default:
		  Err_NoPermission ();
		  break;
	      }

	    /* Maximum value */
	    HTM_SPAN_Begin ("class=\"DAT_LIGHT_%s\"",The_GetSuffix ());
	       HTM_NBSP ();
	       HTM_Txt ("&le;");
	       HTM_Double (Criterion.Values[RubCri_MAX]);
	    HTM_SPAN_End ();

	 HTM_TD_End ();

	 /***** Weight *****/
         RubCri_WriteTxt ("&times;");
	 RubCri_WriteWeight (&Criterion);

	 /***** Weighted score *****/
         RubCri_WriteTxt ("=");
	 HTM_TD_Begin ("class=\"RT DAT_%s %s\"",
		       The_GetSuffix (),
		       The_GetColorRows ());
	    WeightedScore = CriterionScore * Criterion.Weight;
	    HTM_Double2Decimals (WeightedScore);
	 HTM_TD_End ();

      /***** End row *****/
      HTM_TR_End ();

      /***** Update totals *****/
      SumOfWeights += Criterion.Weight;
      SumOfScores += CriterionScore;
      WeightedSum += WeightedScore;
     }

   /***** Write total row *****/
   HTM_TR_Begin (NULL);
      RubCri_WriteTotalLabel (3);
      RubCri_WriteTotalValue (SumOfScores);
      RubCri_WriteTotalEmpty ();
      RubCri_WriteTotalValue (SumOfWeights);
      RubCri_WriteTotalEmpty ();
      RubCri_WriteTotalValue (WeightedSum);
   HTM_TR_End ();
  }

/*****************************************************************************/
/********************* Write table cell with criterion title *****************/
/*****************************************************************************/

static void RubCri_WriteIndex (const struct RubCri_Criterion *Criterion,
                               const char *Anchor)
  {
   HTM_TD_Begin ("class=\"RT %s\"",The_GetColorRows ());
      if (Anchor)
         HTM_ARTICLE_Begin (Anchor);
      Lay_WriteIndex (Criterion->CriInd,"BIG_INDEX");
      if (Anchor)
         HTM_ARTICLE_End ();
   HTM_TD_End ();
  }

/*****************************************************************************/
/********************* Write table cell with criterion title *****************/
/*****************************************************************************/

static void RubCri_WriteTitle (const struct RubCri_Criterion *Criterion)
  {
   HTM_TD_Begin ("class=\"LT DAT_%s %s\"",The_GetSuffix (),The_GetColorRows ());
      HTM_Txt (Criterion->Title);
   HTM_TD_End ();
  }

/*****************************************************************************/
/********************* Write table cell with criterion link ******************/
/*****************************************************************************/

static void RubCri_WriteLink (const struct RubCri_Criterion *Criterion)
  {
   HTM_TD_Begin ("class=\"LT DAT_%s %s\"",The_GetSuffix (),The_GetColorRows ());
      Rsc_WriteLinkName (&Criterion->Link,Frm_DONT_PUT_FORM_TO_GO);
   HTM_TD_End ();
  }

/*****************************************************************************/
/****** Write table cells with minimum and maximum values of criterion *******/
/*****************************************************************************/

static void RubCri_WriteMinimumMaximum (const struct RubCri_Criterion *Criterion)
  {
   RubCri_ValueRange_t ValueRange;

   for (ValueRange  = (RubCri_ValueRange_t) 0;
	ValueRange <= (RubCri_ValueRange_t) (RubCri_NUM_VALUES - 1);
	ValueRange++)
     {
      HTM_TD_Begin ("class=\"RT DAT_%s %s\"",The_GetSuffix (),The_GetColorRows ());
	 HTM_Double (Criterion->Values[ValueRange]);
      HTM_TD_End ();
     }
  }

/*****************************************************************************/
/********************** Write table cell with a text *************************/
/*****************************************************************************/

static void RubCri_WriteTxt (const char *Txt)
  {
   HTM_TD_Begin ("class=\"CT DAT_%s %s\"",The_GetSuffix (),The_GetColorRows ());
      HTM_Txt (Txt);
   HTM_TD_End ();
  }

/*****************************************************************************/
/************** Write table cell with weight value of criterion **************/
/*****************************************************************************/

static void RubCri_WriteWeight (const struct RubCri_Criterion *Criterion)
  {
   HTM_TD_Begin ("class=\"RT DAT_%s %s\"",The_GetSuffix (),The_GetColorRows ());
      HTM_Double (Criterion->Weight);
   HTM_TD_End ();
  }

/*****************************************************************************/
/******************** Write table cells with total value *********************/
/*****************************************************************************/

static void RubCri_WriteTotalLabel (unsigned ColSpan)
  {
   extern const char *Txt_Total;

   HTM_TD_Begin ("colspan=\"%u\" class=\"RB LINE_TOP DAT_STRONG_%s\"",
		 ColSpan,The_GetSuffix ());
      HTM_Txt (Txt_Total);
   HTM_TD_End ();
  }

static void RubCri_WriteTotalEmpty (void)
  {
   HTM_TD_Begin ("class=\"RB LINE_TOP\"");
   HTM_TD_End ();
  }

static void RubCri_WriteTotalValue (double Total)
  {
   HTM_TD_Begin ("class=\"RB LINE_TOP DAT_STRONG_%s\"",The_GetSuffix ());
      HTM_Double2Decimals (Total);
   HTM_TD_End ();
  }

/*****************************************************************************/
/********** Recursive function to compute the score of a criterion ***********/
/*****************************************************************************/
// Return true if rubric tree is recursive

static bool RubCri_ComputeRubricScore (Rsc_Type_t Type,long Cod,long UsrCod,
                                       struct Rub_Node **TOS,long RubCod,
                                       double *RubricScore)
  {
   bool RecursiveTree;
   MYSQL_RES *mysql_res;
   unsigned NumCriteria;
   unsigned NumCriterion;
   struct RubCri_Criterion Criterion;
   double CriterionScore;

   /***** Initialize rubric score *****/
   *RubricScore = 0.0;

   /***** Check that rubric is not yet in the stack *****/
   RecursiveTree = Rub_FindRubCodInStack (*TOS,RubCod);

   if (!RecursiveTree)
     {
      /***** Push rubric code in stack *****/
      Rub_PushRubCod (TOS,RubCod);

      /***** Get data of rubric criteria from database *****/
      NumCriteria = Rub_DB_GetCriteria (&mysql_res,RubCod);
      for (NumCriterion = 0;
	   NumCriterion < NumCriteria;
	   NumCriterion++)
	{
	 /***** Get criterion data *****/
	 RubCri_GetCriterionDataFromRow (mysql_res,&Criterion);

	 /***** Get/compute criterion score *****/
	 CriterionScore = 0.0;
	 switch (Criterion.Link.Type)
	   {
	    case Rsc_NONE:
	       CriterionScore = Rub_DB_GetScore (Rsc_PROJECT,Cod,UsrCod,Criterion.CriCod);
	       break;
	    case Rsc_RUBRIC:
	       if (RubCri_ComputeRubricScore (Type,Cod,UsrCod,
					      TOS,Criterion.Link.Cod,
					      &CriterionScore))
		  RecursiveTree = true;
	       break;
	    default:
	       break;
	   }

	 /***** Compute score of this criterion in the child rubric *****/
	 *RubricScore += Criterion.Weight * CriterionScore;	// Update weighted sum
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Pop rubric code from stack *****/
      Rub_PopRubCod (TOS);
     }

   return RecursiveTree;
  }

/*****************************************************************************/
/********************** Push/pop rubric code in stack ************************/
/*****************************************************************************/

void Rub_PushRubCod (struct Rub_Node **TOS,long RubCod)
  {
   struct Rub_Node *Node;

   /***** Save current top of stack *****/
   Node = *TOS;

   /***** Create top of stack node *****/
   if ((*TOS = malloc (sizeof (struct Rub_Node))) == NULL)
      Err_NotEnoughMemoryExit ();
   (*TOS)->RubCod = RubCod;
   (*TOS)->Prev = Node;		// Link to previous top of stack
  }

void Rub_PopRubCod (struct Rub_Node **TOS)
  {
   struct Rub_Node *Node;

   if (*TOS)
     {
      /***** Save current top of stack *****/
      Node = (*TOS)->Prev;

      /***** Free current top of stack node *****/
      free (*TOS);

      /***** Assign new top of stack *****/
      *TOS = Node;
     }
  }

/*****************************************************************************/
/************************ Find rubric code in stack **************************/
/*****************************************************************************/
// Return true if found

bool Rub_FindRubCodInStack (const struct Rub_Node *TOS,long RubCod)
  {
   while (TOS)
     {
      if (TOS->RubCod == RubCod)
	 return true;

      TOS = TOS->Prev;
     }

   return false;
  }

/*****************************************************************************/
/***************** Get parameter with score of a criterion *******************/
/*****************************************************************************/

double RubCri_GetParScore (void)
  {
   char ScoreStr[64];

   /***** Get criterion weight *****/
   Par_GetParText ("Score",ScoreStr,sizeof (ScoreStr) - 1);
   return Str_GetDoubleFromStr (ScoreStr);
  }

/*****************************************************************************/
/************************** Get rubric criteria data *************************/
/*****************************************************************************/

void RubCri_GetCriterionDataFromRow (MYSQL_RES *mysql_res,
                                     struct RubCri_Criterion *Criterion)
  {
   MYSQL_ROW row;
   RubCri_ValueRange_t ValueRange;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);
   /*
   row[0] CriCod
   row[1] RubCod
   row[2] CriInd
   row[3] Source
   row[4] Cod
   row[5] MinVal
   row[6] MaxVal
   row[7] Weight
   row[8] Title
   */
   /***** Get criterion code (row[0]) and rubric code (row[1]) *****/
   if ((Criterion->CriCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongCriterionExit ();
   if ((Criterion->RubCod = Str_ConvertStrCodToLongCod (row[1])) <= 0)
      Err_WrongRubricExit ();

   /***** Get criterion index (row[2]) *****/
   Criterion->CriInd = Str_ConvertStrToUnsigned (row[2]);

   /***** Get type (row[3]) and code (row[4]) *****/
   Criterion->Link.Type = Rsc_GetTypeFromString (row[3]);
   Criterion->Link.Cod  = Str_ConvertStrCodToLongCod (row[4]);

   /***** Get criterion minimum and maximum values (row[5], row[6]) *****/
   for (ValueRange  = (RubCri_ValueRange_t) 0;
	ValueRange <= (RubCri_ValueRange_t) (RubCri_NUM_VALUES - 1);
	ValueRange++)
      Criterion->Values[ValueRange] = Str_GetDoubleFromStr (row[5 + ValueRange]);

   /***** Get criterion weight (row[7]) *****/
   Criterion->Weight = Str_GetDoubleFromStr (row[5 + RubCri_NUM_VALUES]);

   /***** Get the title of the criterion (row[8]) *****/
   Str_Copy (Criterion->Title,row[5 + RubCri_NUM_VALUES + 1],
             sizeof (Criterion->Title) - 1);
  }

/*****************************************************************************/
/****************** Put table heading for rubric criteria ********************/
/*****************************************************************************/

static void RubCri_PutTableHeadingForSeeing (void)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Criterion;
   extern const char *Txt_Source;
   extern const char *Txt_Minimum;
   extern const char *Txt_Maximum;
   extern const char *Txt_Weight;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Header cells *****/
      HTM_TH (Txt_No_INDEX ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Criterion,HTM_HEAD_LEFT );
      HTM_TH (Txt_Source   ,HTM_HEAD_LEFT );
      HTM_TH (Txt_Minimum  ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Maximum  ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Weight   ,HTM_HEAD_RIGHT);

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/****************** Put table heading for rubric criteria ********************/
/*****************************************************************************/

static void RubCri_PutTableHeadingForEdition (void)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Criterion;
   extern const char *Txt_Source;
   extern const char *Txt_Minimum;
   extern const char *Txt_Maximum;
   extern const char *Txt_Weight;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Header cells *****/
      HTM_TH_Empty (1);
      HTM_TH (Txt_No_INDEX ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Criterion,HTM_HEAD_LEFT );
      HTM_TH (Txt_Source   ,HTM_HEAD_LEFT );
      HTM_TH (Txt_Minimum  ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Maximum  ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Weight   ,HTM_HEAD_RIGHT);

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/****************** Put table heading for rubric criteria ********************/
/*****************************************************************************/

static void RubCri_PutTableHeadingForScoring (void)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Criterion;
   extern const char *Txt_Source;
   extern const char *Txt_Weight;
   extern const char *Txt_Score;
   extern const char *Txt_Grade;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Header cells *****/
      HTM_TH (Txt_No_INDEX ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Criterion,HTM_HEAD_LEFT );
      HTM_TH (Txt_Source   ,HTM_HEAD_LEFT );
      HTM_TH (Txt_Score    ,HTM_HEAD_RIGHT);
      HTM_TH_Empty (1);
      HTM_TH (Txt_Weight   ,HTM_HEAD_RIGHT);
      HTM_TH_Empty (1);
      HTM_TH (Txt_Grade    ,HTM_HEAD_RIGHT);

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*************************** Reset rubric criterion **************************/
/*****************************************************************************/

void RubCri_ResetCriterion (struct RubCri_Criterion *Criterion)
  {
   // Default values minimum/maximum criterion values
   static double RubCri_DefaultValues[RubCri_NUM_VALUES] =
     {
      [RubCri_MIN] =  0.0,
      [RubCri_MAX] = 10.0,
     };
   RubCri_ValueRange_t ValueRange;

   Criterion->RubCod = -1L;
   Criterion->CriCod = -1L;
   Criterion->CriInd = 0;
   Criterion->Link.Type = Rsc_NONE;
   Criterion->Link.Cod  = -1L;
   for (ValueRange  = (RubCri_ValueRange_t) 0;
	ValueRange <= (RubCri_ValueRange_t) (RubCri_NUM_VALUES - 1);
	ValueRange++)
      Criterion->Values[ValueRange] = RubCri_DefaultValues[ValueRange];
   Criterion->Weight = RubCri_WEIGHT_MAX;;
   Criterion->Title[0] = '\0';
  }

/*****************************************************************************/
/***************** Request the removal of a rubric criterion *****************/
/*****************************************************************************/

void RubCri_ReqRemCriterion (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_criterion_X;
   struct Rub_Rubrics Rubrics;

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);
   Rub_RubricConstructor (&Rubrics.Rubric);
   RubCri_ResetCriterion (&Rubrics.Criterion);

   /***** Get and check parameters *****/
   RubCri_GetAndCheckRubricAndCriterion (&Rubrics);

   /***** Check if rubric is editable *****/
   if (!Rub_CheckIfEditable ())
      Err_NoPermissionExit ();

   /***** Show question and button to remove question *****/
   Ale_ShowAlertRemove (ActRemRubCri,NULL,
			RubCri_PutParsOneCriterion,&Rubrics,
			Txt_Do_you_really_want_to_remove_the_criterion_X,
			Rubrics.Criterion.Title);

   /***** Show current rubric and its criteria *****/
   Rub_PutFormsOneRubric (&Rubrics,Rub_EXISTING_RUBRIC);

   /***** Free memory used for rubric *****/
   Rub_RubricDestructor (&Rubrics.Rubric);
  }

/*****************************************************************************/
/************************* Remove a rubric criterion *************************/
/*****************************************************************************/

void RubCri_RemoveCriterion (void)
  {
   extern const char *Txt_Criterion_removed;
   struct Rub_Rubrics Rubrics;

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);
   Rub_RubricConstructor (&Rubrics.Rubric);
   RubCri_ResetCriterion (&Rubrics.Criterion);

   /***** Get and check parameters *****/
   RubCri_GetAndCheckRubricAndCriterion (&Rubrics);

   /***** Check if rubric is editable *****/
   if (!Rub_CheckIfEditable ())
      Err_NoPermissionExit ();

   /***** Remove the criterion from all tables *****/
   /* Remove questions associated to criterion */
   // Exa_DB_RemoveAllSetQuestionsFromSet (Criterion.CriCod,Criterion.RubCod);

   /* Remove the criterion itself */
   Rub_DB_RemoveCriterionFromRubric (&Rubrics.Criterion);

   /* Change indexes of criteria greater than this */
   Rub_DB_UpdateCriteriaIndexesInRubricGreaterThan (&Rubrics.Criterion);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Criterion_removed);

   /***** Show current rubric and its criteria *****/
   Rub_PutFormsOneRubric (&Rubrics,Rub_EXISTING_RUBRIC);

   /***** Free memory used for rubric *****/
   Rub_RubricDestructor (&Rubrics.Rubric);
  }

/*****************************************************************************/
/*************** Move up position of a criterion in a rubric *****************/
/*****************************************************************************/

void RubCri_MoveUpCriterion (void)
  {
   extern const char *Txt_Movement_not_allowed;
   struct Rub_Rubrics Rubrics;
   unsigned CriIndTop;
   unsigned CriIndBottom;

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);
   Rub_RubricConstructor (&Rubrics.Rubric);
   RubCri_ResetCriterion (&Rubrics.Criterion);

   /***** Get and check parameters *****/
   RubCri_GetAndCheckRubricAndCriterion (&Rubrics);

   /***** Check if rubric is editable *****/
   if (!Rub_CheckIfEditable ())
      Err_NoPermissionExit ();

   /***** Get criterion index *****/
   CriIndBottom = Rub_DB_GetCriIndFromCriCod (Rubrics.Rubric.RubCod,Rubrics.Criterion.CriCod);

   /***** Move up criterion *****/
   if (CriIndBottom > 1)
     {
      /* Indexes of criteria to be exchanged */
      CriIndTop = Rub_DB_GetPrevCriterionIndexInRubric (Rubrics.Rubric.RubCod,CriIndBottom);
      if (CriIndTop == 0)
	 Err_ShowErrorAndExit ("Wrong criterion index.");

      /* Exchange criteria */
      RubCri_ExchangeCriteria (Rubrics.Rubric.RubCod,CriIndTop,CriIndBottom);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

   /***** Show current rubric and its criteria *****/
   Rub_PutFormsOneRubric (&Rubrics,Rub_EXISTING_RUBRIC);

   /***** Free memory used for rubric *****/
   Rub_RubricDestructor (&Rubrics.Rubric);
  }

/*****************************************************************************/
/************** Move down position of a criterion in a rubric ****************/
/*****************************************************************************/

void RubCri_MoveDownCriterion (void)
  {
   extern const char *Txt_Movement_not_allowed;
   struct Rub_Rubrics Rubrics;
   unsigned CriIndTop;
   unsigned CriIndBottom;
   unsigned MaxCriInd;	// 0 if no criteria

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);
   Rub_RubricConstructor (&Rubrics.Rubric);
   RubCri_ResetCriterion (&Rubrics.Criterion);

   /***** Get and check parameters *****/
   RubCri_GetAndCheckRubricAndCriterion (&Rubrics);

   /***** Check if rubric is editable *****/
   if (!Rub_CheckIfEditable ())
      Err_NoPermissionExit ();

   /***** Get criterion index *****/
   CriIndTop = Rub_DB_GetCriIndFromCriCod (Rubrics.Rubric.RubCod,Rubrics.Criterion.CriCod);

   /***** Get maximum criterion index *****/
   MaxCriInd = Rub_DB_GetMaxCriterionIndexInRubric (Rubrics.Rubric.RubCod);

   /***** Move down criterion *****/
   if (CriIndTop < MaxCriInd)
     {
      /* Indexes of criteria to be exchanged */
      CriIndBottom = Rub_DB_GetNextCriterionIndexInRubric (Rubrics.Rubric.RubCod,CriIndTop);
      if (CriIndBottom == 0)	// 0 means error reading from database
	 Err_ShowErrorAndExit ("Wrong criterion index.");

      /* Exchange criteria */
      RubCri_ExchangeCriteria (Rubrics.Rubric.RubCod,CriIndTop,CriIndBottom);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

   /***** Show current rubric and its criteria *****/
   Rub_PutFormsOneRubric (&Rubrics,Rub_EXISTING_RUBRIC);

   /***** Free memory used for rubric *****/
   Rub_RubricDestructor (&Rubrics.Rubric);
  }

/*****************************************************************************/
/********* Get and check parameters, rubric data and criterion data **********/
/*****************************************************************************/

static void RubCri_GetAndCheckRubricAndCriterion (struct Rub_Rubrics *Rubrics)
  {
   /***** Get parameters *****/
   Rub_GetPars (Rubrics,true);
   Rubrics->Criterion.CriCod = ParCod_GetAndCheckPar (ParCod_Cri);

   /***** Get rubric data from database *****/
   Rub_GetRubricDataByCod (&Rubrics->Rubric);
   if (Rubrics->Rubric.CrsCod != Gbl.Hierarchy.Crs.CrsCod)
      Err_WrongRubricExit ();

   /***** Get criterion data from database *****/
   RubCri_GetCriterionDataByCod (&Rubrics->Criterion);
   if (Rubrics->Criterion.RubCod != Rubrics->Rubric.RubCod)
      Err_WrongCriterionExit ();
  }

/*****************************************************************************/
/******** Exchange the order of two consecutive criteria in a rubric *********/
/*****************************************************************************/

static void RubCri_ExchangeCriteria (long RubCod,
                                     unsigned CriIndTop,unsigned CriIndBottom)
  {
   long CriCodTop;
   long CriCodBottom;

   /***** Lock table to make the move atomic *****/
   Rub_DB_LockTable ();

   /***** Get criterion codes of the sets to be moved *****/
   CriCodTop    = Rub_DB_GetCriCodFromCriInd (RubCod,CriIndTop   );
   CriCodBottom = Rub_DB_GetCriCodFromCriInd (RubCod,CriIndBottom);

   /***** Exchange indexes of sets *****/
   /*
   Example:
   CriIndTop    = 1; CriCodTop    = 218
   CriIndBottom = 2; CriCodBottom = 220
                     Step 1              Step 2              Step 3
+--------+--------+ +--------+--------+ +--------+--------+ +--------+--------+
| CriInd | CriCod | | CriInd | CriCod | | CriInd | CriCod | | CriInd | CriCod |
+--------+--------+ +--------+--------+ +--------+--------+ +--------+--------+
|      1 |    218 |>|     -2 |    218 |>|     -2 |    218 |>|      2 |    218 |
|      2 |    220 | |      2 |    220 | |      1 |    220 | |      1 |    220 |
|      3 |    232 | |      3 |    232 | |      3 |    232 | |      3 |    232 |
+--------+--------+ +--------+--------+ +--------+--------+ +--------+--------+
   */
   /* Step 1: change temporarily top index to minus bottom index
              in order to not repeat unique index (RubCod,CriInd) */
   Rub_DB_UpdateCriterionIndex (-((long) CriIndBottom),CriCodTop   ,RubCod);

   /* Step 2: change bottom index to old top index  */
   Rub_DB_UpdateCriterionIndex (  (long) CriIndTop    ,CriCodBottom,RubCod);

   /* Step 3: change top index to old bottom index */
   Rub_DB_UpdateCriterionIndex (  (long) CriIndBottom ,CriCodTop   ,RubCod);

   /***** Unlock table *****/
   DB_UnlockTables ();
  }

/*****************************************************************************/
/***************** Show clipboard to change resource link ********************/
/*****************************************************************************/

void RubCri_ChangeLink (void)
  {
   struct Rub_Rubrics Rubrics;

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);
   Rub_RubricConstructor (&Rubrics.Rubric);
   RubCri_ResetCriterion (&Rubrics.Criterion);

   /***** Get and check parameters *****/
   RubCri_GetAndCheckRubricAndCriterion (&Rubrics);

   /***** Check if rubric is editable *****/
   if (!Rub_CheckIfEditable ())
      Err_NoPermissionExit ();

   /***** Get link type and code *****/
   if (Rsc_GetParLink (&Rubrics.Criterion.Link))
     {
      /***** Update link to resource in criterion *****/
      Rub_DB_UpdateCriterionLink (&Rubrics.Criterion);

      /***** Remove link from clipboard *****/
      Rsc_DB_RemoveLinkFromClipboard (&Rubrics.Criterion.Link);
     }

   /***** Show current rubric and its criteria *****/
   Rub_PutFormsOneRubric (&Rubrics,Rub_EXISTING_RUBRIC);

   /***** Free memory used for rubric *****/
   Rub_RubricDestructor (&Rubrics.Rubric);
  }
