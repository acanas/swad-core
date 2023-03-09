// swad_rubric_criteria.c: criteria in assessment rubrics
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2022 Antonio Cañas Vargas

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

#include <stdbool.h>		// For boolean type
#include <mysql/mysql.h>	// To access MySQL databases
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_button.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_parameter.h"
#include "swad_rubric.h"
#include "swad_rubric_criteria.h"
#include "swad_rubric_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void RubCri_ListOneOrMoreCriteriaForEdition (struct Rub_Rubrics *Rubrics,
						    unsigned NumCriteria,
                                                    MYSQL_RES *mysql_res,
						    bool ICanEditCriteria);
static void RubCri_PutParsOneCriterion (void *Rubrics);

/*****************************************************************************/
/********************** Initialize a new citerion to zero ********************/
/*****************************************************************************/

void RubCri_CriterionConstructor (struct Rub_Criterion *Criterion)
  {
   Criterion->CriCod = -1L;
  }

/*****************************************************************************/
/*************** Free memory allocated for rubric criterion ******************/
/*****************************************************************************/

void RubCri_CriterionDestructor (struct Rub_Criterion *Criterion)
  {
   Criterion->CriCod = -1L;
  }

/*****************************************************************************/
/*********** Put parameter with criterion code to edit, remove... ************/
/*****************************************************************************/

void RubCri_PutParCriCod (void *CriCod)	// Should be a pointer to long
  {
   if (CriCod)
      if (*((long *) CriCod) > 0)	// If criterion exists
	 Par_PutParLong (NULL,"CriCod",*((long *) CriCod));
  }

/*****************************************************************************/
/*************** Get data of a rubric criterion from database ****************/
/*****************************************************************************/

bool RubCri_GetCriterionDataFromDB (struct Rub_Criterion *Criterion)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool CriterionExists;

   /***** Get question data from database *****/
   if ((CriterionExists = (Rub_DB_GetCriterionData (&mysql_res,Criterion->CriCod) != 0)))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get the title (row[0]) */
      Criterion->Title[0] = '\0';
      if (row[0])
	 if (row[0][0])
	    Str_Copy (Criterion->Title,row[0],Cns_MAX_BYTES_TEXT);
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   return CriterionExists;
  }

/*****************************************************************************/
/******************* List a rubric criterion for edition *********************/
/*****************************************************************************/

void RubCri_ListCriterionForEdition (struct Rub_Criterion *Criterion,
                                     unsigned CriInd,bool CriterionExists,
                                     const char *Anchor)
  {
   extern const char *Txt_Criterion_removed;

   /***** Number of criterion *****/
   HTM_TD_Begin ("class=\"RT %s\"",The_GetColorRows ());
      RubCri_WriteNumCriterion (CriInd,"BIG_INDEX");
   HTM_TD_End ();

   /***** Write criterion code *****/
   HTM_TD_Begin ("class=\"CT DAT_SMALL_%s %s CT\"",
                 The_GetSuffix (),The_GetColorRows ());
      HTM_TxtF ("%ld&nbsp;",Criterion->CriCod);
   HTM_TD_End ();

   /***** Write title *****/
   HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
      HTM_ARTICLE_Begin (Anchor);
	 if (CriterionExists)
	    /* Write title */
	    RubCri_WriteCriterionTitle (Criterion->Title,"Qst_TXT",
			       true);	// Visible
	 else
	   {
	    HTM_SPAN_Begin ("class=\"DAT_LIGHT_%s\"",
	                    The_GetSuffix ());
	       HTM_Txt (Txt_Criterion_removed);
	    HTM_SPAN_End ();
	   }
      HTM_ARTICLE_End ();
   HTM_TD_End ();
  }

/*****************************************************************************/
/******************* Write the number of a rubric criterion ******************/
/*****************************************************************************/
// Number of criterion should be 1, 2, 3...

void RubCri_WriteNumCriterion (unsigned NumCri,const char *Class)
  {
   HTM_DIV_Begin ("class=\"%s_%s\"",Class,The_GetSuffix ());
      HTM_Unsigned (NumCri);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************* Write the title of a rubric criterion *******************/
/*****************************************************************************/

void RubCri_WriteCriterionTitle (const char *Title,const char *ClassTitle,bool Visible)
  {
   unsigned long TitleLength;
   char *TitleRigorousHTML;

   /***** DIV begin *****/
   HTM_DIV_Begin ("class=\"%s_%s\"",ClassTitle,The_GetSuffix ());

      /***** Write title *****/
      if (Title && Visible)
	{
	 if (Title[0])
	   {
	    /* Convert the title, that is in HTML, to rigorous HTML */
	    TitleLength = strlen (Title) * Str_MAX_BYTES_PER_CHAR;
	    if ((TitleRigorousHTML = malloc (TitleLength + 1)) == NULL)
	       Err_NotEnoughMemoryExit ();
	    Str_Copy (TitleRigorousHTML,Title,TitleLength);

	    Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			      TitleRigorousHTML,TitleLength,false);

	    /* Write title text */
	    HTM_Txt (TitleRigorousHTML);

	    /* Free memory allocated for the title */
	    free (TitleRigorousHTML);
	   }
	}
      else
	 Ico_PutIconNotVisible ();

   /***** DIV end *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/***************** Put icon to add a new criterion to rubric *****************/
/*****************************************************************************/

void RubCri_PutIconToAddNewCriterion (void *Rubrics)
  {
   Ico_PutContextualIconToAdd (ActFrmNewRubCri,NULL,Rub_PutPars,Rubrics);
  }

/*****************************************************************************/
/***************** Put button to add new criteria to rubric ******************/
/*****************************************************************************/

void RubCri_PutButtonToAddNewCriterion (struct Rub_Rubrics *Rubrics)
  {
   extern const char *Txt_New_criterion;

   Frm_BeginForm (ActFrmNewRubCri);
      Rub_PutPars (Rubrics);

      Btn_PutConfirmButton (Txt_New_criterion);

   Frm_EndForm ();
  }

/*****************************************************************************/
/****************** Write parameter with index of criterion ******************/
/*****************************************************************************/

void RubCri_PutParCriInd (unsigned CriInd)
  {
   Par_PutParUnsigned (NULL,"CriInd",CriInd);
  }

/*****************************************************************************/
/******************* Get parameter with index of criterion *******************/
/*****************************************************************************/

unsigned RubCri_GetParCriInd (void)
  {
   long CriInd;

   if ((CriInd = Par_GetParLong ("CriInd")) <= 0)	// In rubrics, criterion index should be 1, 2, 3...
      Err_WrongCriterionIndexExit ();

   return (unsigned) CriInd;
  }

/*****************************************************************************/
/*********************** List the criteria of a rubric ***********************/
/*****************************************************************************/

void RubCri_ListRubricCriteria (struct Rub_Rubrics *Rubrics)
  {
   extern const char *Hlp_ASSESSMENT_Rubrics_criteria;
   extern const char *Txt_Criteria;
   MYSQL_RES *mysql_res;
   unsigned NumCriteria;
   bool ICanEditCriteria = Rub_CheckIfICanEditRubrics ();

   /***** Get data of criteria from database *****/
   NumCriteria = Rub_DB_GetRubricCriteriaBasic (&mysql_res,Rubrics->Rubric.RubCod);

   /***** Begin box *****/
   if (ICanEditCriteria)
      Box_BoxBegin (NULL,Txt_Criteria,
		    RubCri_PutIconToAddNewCriterion,Rubrics,
		    Hlp_ASSESSMENT_Rubrics_criteria,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,Txt_Criteria,
		    NULL,NULL,
		    Hlp_ASSESSMENT_Rubrics_criteria,Box_NOT_CLOSABLE);

   /***** Show table with criteria *****/
   if (NumCriteria)
      RubCri_ListOneOrMoreCriteriaForEdition (Rubrics,NumCriteria,mysql_res,
					     ICanEditCriteria);

   /***** Put button to add a new criterion in this rubric *****/
   if (ICanEditCriteria)		// I can edit criteria
      RubCri_PutButtonToAddNewCriterion (Rubrics);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* List rubric criteria for edition **********************/
/*****************************************************************************/

static void RubCri_ListOneOrMoreCriteriaForEdition (struct Rub_Rubrics *Rubrics,
						    unsigned NumCriteria,
                                                    MYSQL_RES *mysql_res,
						    bool ICanEditCriteria)
  {
   extern const char *Txt_Criteria;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Code;
   extern const char *Txt_Tags;
   extern const char *Txt_Criterion;
   extern const char *Txt_Movement_not_allowed;
   unsigned NumCri;
   unsigned CriInd;
   unsigned MaxCriInd;
   MYSQL_ROW row;
   struct Rub_Criterion Criterion;
   char StrCriInd[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   bool CriterionExists;
   char *Anchor = NULL;

   /***** Trivial check *****/
   if (!NumCriteria)
      return;

   /***** Get maximum criterion index *****/
   MaxCriInd = Rub_DB_GetMaxCriterionIndexInRubric (Rubrics->Rubric.RubCod);	// 0 is no criteria in rubric

   /***** Write the heading *****/
   HTM_TABLE_BeginWideMarginPadding (5);

      HTM_TR_Begin (NULL);
	 HTM_TH_Empty (1);
         HTM_TH (Txt_No_INDEX ,HTM_HEAD_CENTER);
         HTM_TH (Txt_Code     ,HTM_HEAD_CENTER);
         HTM_TH (Txt_Tags     ,HTM_HEAD_CENTER);
         HTM_TH (Txt_Criterion,HTM_HEAD_CENTER);
      HTM_TR_End ();

      /***** Write rows *****/
      for (NumCri = 0, The_ResetRowColor ();
	   NumCri < NumCriteria;
	   NumCri++, The_ChangeRowColor ())
	{
	 /***** Create rubric criterion *****/
	 RubCri_CriterionConstructor (&Criterion);

	 /***** Get criterion data *****/
	 row = mysql_fetch_row (mysql_res);
	 /*
	 row[0] QstCod
	 row[1] QstInd
	 */
	 /* Get criterion code (row[0]) */
	 Criterion.CriCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get criterion index (row[1]) */
	 CriInd = Str_ConvertStrToUnsigned (row[1]);
	 snprintf (StrCriInd,sizeof (StrCriInd),"%u",CriInd);

	 /***** Build anchor string *****/
	 Frm_SetAnchorStr (Criterion.CriCod,&Anchor);

	 /***** Begin row *****/
	 HTM_TR_Begin (NULL);

	    /***** Icons *****/
	    HTM_TD_Begin ("class=\"BT %s\"",The_GetColorRows ());

	       /* Put icon to remove the criterion */
	       if (ICanEditCriteria)
		  Ico_PutContextualIconToRemove (ActReqRemGamQst,NULL,
						 RubCri_PutParsOneCriterion,Rubrics);
	       else
		  Ico_PutIconRemovalNotAllowed ();

	       /* Put icon to move up the criterion */
	       if (ICanEditCriteria && CriInd > 1)
		  Lay_PutContextualLinkOnlyIcon (ActUp_GamQst,Anchor,
						 RubCri_PutParsOneCriterion,Rubrics,
						 "arrow-up.svg",Ico_BLACK);
	       else
		  Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,
		                  Txt_Movement_not_allowed);

	       /* Put icon to move down the criterion */
	       if (ICanEditCriteria && CriInd < MaxCriInd)
		  Lay_PutContextualLinkOnlyIcon (ActDwnGamQst,Anchor,
						 RubCri_PutParsOneCriterion,Rubrics,
						 "arrow-down.svg",Ico_BLACK);
	       else
		  Ico_PutIconOff ("arrow-down.svg",Ico_BLACK,
		                  Txt_Movement_not_allowed);

	       /* Put icon to edit the criterion */
	       if (ICanEditCriteria)
		  Ico_PutContextualIconToEdit (ActEdiOneTstQst,NULL,
					       RubCri_PutParCriCod,&Criterion.CriCod);

	    HTM_TD_End ();

	    /***** Criterion *****/
	    CriterionExists = RubCri_GetCriterionDataFromDB (&Criterion);
	    RubCri_ListCriterionForEdition (&Criterion,CriInd,CriterionExists,Anchor);

	 /***** End row *****/
	 HTM_TR_End ();

	 /***** Free anchor string *****/
	 Frm_FreeAnchorStr (Anchor);

	 /***** Destroy rubric criterion *****/
	 RubCri_CriterionDestructor (&Criterion);
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/**************** Put parameter to move/remove one criterion *****************/
/*****************************************************************************/

static void RubCri_PutParsOneCriterion (void *Rubrics)
  {
   if (Rubrics)
      Rub_PutPars (Rubrics);
  }

/*****************************************************************************/
/************ Request the creation or edition of a rubric criterion **********/
/*****************************************************************************/

void RubCri_RequestCreatOrEditCri (void)
  {
   struct Rub_Rubrics Rubrics;
   bool ItsANewCriterion;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   Err_ShowErrorAndExit ("Not implemented.");

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);

   /***** Reset rubric *****/
   Rub_ResetRubric (&Rubrics.Rubric);

   /***** Check if I can edit rubrics *****/
   if (!Rub_CheckIfICanEditRubrics ())
      Err_NoPermissionExit ();

   /***** Get parameters *****/
   ItsANewCriterion = ((Rubrics.Rubric.RubCod = Rub_GetPars (&Rubrics)) <= 0);

   /***** Get rubric data *****/
   if (ItsANewCriterion)
     {
      /* Initialize to empty rubric */
      Rub_ResetRubric (&Rubrics.Rubric);
      Txt[0] = '\0';
     }
   else
     {
      /* Get rubric data from database */
      Rub_GetDataOfRubricByCod (&Rubrics.Rubric);
      Rub_DB_GetRubricTxt (Rubrics.Rubric.RubCod,Txt);
     }

   /***** Put forms to create/edit a rubric *****/
   // Rub_PutFormsEditionRubric (&Rubrics,Txt,ItsANewCriterion);

   /***** Show rubrics or criteria *****/
   if (ItsANewCriterion)
      /* Show rubrics again */
      Rub_ListAllRubrics (&Rubrics);
   else
      /* Show criteria of the rubric ready to be edited */
      RubCri_ListRubricCriteria (&Rubrics);
  }
