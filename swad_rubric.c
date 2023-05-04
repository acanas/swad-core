// swad_rubric.c: assessment rubrics

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

#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL

#include "swad_action_list.h"
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
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

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Rub_PutIconsListRubrics (void *Rubrics);
static void Rub_PutIconToCreateNewRubric (struct Rub_Rubrics *Rubrics);
static void Prg_PutIconToViewResourceClipboard (void);
static void Rub_PutParsToCreateNewRubric (void *Rubrics);

static void Rub_PutIconsViewingOneRubric (void *Rubrics);
static void Rub_PutIconsEditingOneRubric (void *Rubrics);
static void Rub_PutIconsToRemEditOneRubric (struct Rub_Rubrics *Rubrics);
static void Rub_WriteAuthor (const struct Rub_Rubric *Rubric);

static void Rub_RemoveRubricFromAllTables (long RubCod);

static void Rub_ReceiveRubricFieldsFromForm (struct Rub_Rubric *Rubric);
static bool Rub_CheckRubricFieldsReceivedFromForm (const struct Rub_Rubric *Rubric);

static void Rub_CreateRubric (struct Rub_Rubric *Rubric);
static void Rub_UpdateRubric (struct Rub_Rubric *Rubric);

/*****************************************************************************/
/*************************** Reset rubrics context ***************************/
/*****************************************************************************/

void Rub_ResetRubrics (struct Rub_Rubrics *Rubrics)
  {
   Rubrics->LstIsRead     = false;	// List not read from database...
   Rubrics->Num           = 0;		// Total number of rubrics
   Rubrics->Lst           = NULL;	// List of rubrics
   Rubrics->CurrentPage   = 0;
   // Rubrics->Rubric.RubCod = -1L;	// Current/selected rubric code
  }

/*****************************************************************************/
/************************ Rubric constructor/destructor **********************/
/*****************************************************************************/

void Rub_RubricConstructor (struct Rub_Rubric *Rubric)
  {
   /***** Initialize to empty rubric *****/
   Rubric->RubCod   = -1L;
   Rubric->CrsCod   = -1L;
   Rubric->UsrCod   = -1L;
   Rubric->Title[0] = '\0';

   /***** Allocate memory for rubric text *****/
   if ((Rubric->Txt = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();
   Rubric->Txt[0] = '\0';
  }

void Rub_RubricDestructor (struct Rub_Rubric *Rubric)
  {
   /***** Free memory used for rubric text *****/
   if (Rubric->Txt != NULL)
     {
      free (Rubric->Txt);
      Rubric->Txt = NULL;
     }
  }

/*****************************************************************************/
/***************************** Show all rubrics ******************************/
/*****************************************************************************/

void Rub_SeeAllRubrics (void)
  {
   struct Rub_Rubrics Rubrics;

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);

   /***** Get parameters *****/
   Rub_GetPars (&Rubrics,false);

   /***** Show all rubrics *****/
   Rub_ListAllRubrics (&Rubrics);
  }

/*****************************************************************************/
/******************************* List all rubrics ****************************/
/*****************************************************************************/

void Rub_ListAllRubrics (struct Rub_Rubrics *Rubrics)
  {
   extern const char *Hlp_ASSESSMENT_Rubrics;
   extern const char *Txt_Rubrics;
   extern const char *Txt_Rubric;
   extern const char *Txt_No_rubrics;
   struct Pag_Pagination Pagination;
   unsigned NumRubric;

   /***** Get number of groups in current course *****/
   if (!Gbl.Crs.Grps.NumGrps)
      Gbl.Crs.Grps.WhichGrps = Grp_ALL_GROUPS;

   /***** Get list of rubrics *****/
   Rub_GetListRubrics (Rubrics);

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Rubrics->Num;
   Pagination.CurrentPage = (int) Rubrics->CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Rubrics->CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Begin box *****/
   Box_BoxBegin ("100%",Txt_Rubrics,
                 Rub_PutIconsListRubrics,Rubrics,
                 Hlp_ASSESSMENT_Rubrics,Box_NOT_CLOSABLE);

      /***** Write links to pages *****/
      Pag_WriteLinksToPagesCentered (Pag_RUBRICS,&Pagination,
				     Rubrics,-1L);

      if (Rubrics->Num)
	{
	 /***** Begin table *****/
	 HTM_TABLE_BeginWideMarginPadding (5);

	    /***** Table head *****/
	    HTM_TR_Begin (NULL);

               HTM_TH_Span (NULL,HTM_HEAD_CENTER,1,1,"CONTEXT_COL");	// Column for contextual icons
	       HTM_TH_Span (Txt_Rubric,HTM_HEAD_LEFT,1,2,NULL);

	       // HTM_TH (Txt_Criteria,HTM_HEAD_RIGHT);

	    HTM_TR_End ();

	    /***** Write all rubrics *****/
	    for (NumRubric  = Pagination.FirstItemVisible;
		 NumRubric <= Pagination.LastItemVisible;
		 NumRubric++)
	      {
	       /***** Get rubric data *****/
	       Rub_RubricConstructor (&Rubrics->Rubric);
	       Rubrics->Rubric.RubCod = Rubrics->Lst[NumRubric - 1];
	       Rub_GetRubricDataByCod (&Rubrics->Rubric);

	       /***** Show main data of this rubric *****/
	       Rub_ShowRubricMainData (Rubrics,
	                               false);	// Do not show only this rubric

	       /***** Free memory used for rubric *****/
	       Rub_RubricDestructor (&Rubrics->Rubric);
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();
	}
      else	// No rubrics created
	 Ale_ShowAlert (Ale_INFO,Txt_No_rubrics);

      /***** Write again links to pages *****/
      Pag_WriteLinksToPagesCentered (Pag_RUBRICS,&Pagination,
				     Rubrics,-1L);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of rubrics *****/
   Rub_FreeListRubrics (Rubrics);
  }

/*****************************************************************************/
/************************ Check if I can edit rubrics ************************/
/*****************************************************************************/

bool Rub_CheckIfICanEditRubrics (void)
  {
   static const bool ICanEditRubrics[Rol_NUM_ROLES] =
     {
      [Rol_TCH    ] = true,
      [Rol_SYS_ADM] = true,
     };

   return ICanEditRubrics[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/*********************** Check if edition is possible ************************/
/*****************************************************************************/

bool Rub_CheckIfEditable (void)
  {
   if (Rub_CheckIfICanEditRubrics ())
      /***** Rubric is editable only if ... *****/
      // TODO: Change to control that a rubric is not edited in some circunstances?
      /*
      return Rubric->NumCriteria == 0 ||
	     Rubric->NumCriteria != 0;
      */
      return true;
   else
      return false;	// Questions are not editable
  }

/*****************************************************************************/
/****************** Put contextual icons in list of rubrics ******************/
/*****************************************************************************/

static void Rub_PutIconsListRubrics (void *Rubrics)
  {
   if (Rubrics)
     {
      if (Rub_CheckIfICanEditRubrics ())
	{
         /***** Put icon to create a new rubric *****/
	 Rub_PutIconToCreateNewRubric ((struct Rub_Rubrics *) Rubrics);

	 /***** Put icon to view resource clipboard *****/
	 Prg_PutIconToViewResourceClipboard ();
	}

      /***** Link to get resource link *****/
      if (Rsc_CheckIfICanGetLink ())
	 Ico_PutContextualIconToGetLink (ActReqLnkRub,NULL,
					 Rub_PutPars,Rubrics);

      /***** Put icon to show a figure *****/
      Fig_PutIconToShowFigure (Fig_RUBRICS);
     }
  }

/*****************************************************************************/
/********************** Put icon to create a new rubric **********************/
/*****************************************************************************/

static void Rub_PutIconToCreateNewRubric (struct Rub_Rubrics *Rubrics)
  {
   Ico_PutContextualIconToAdd (ActFrmNewRub,NULL,
                               Rub_PutParsToCreateNewRubric,Rubrics);
  }

/*****************************************************************************/
/******************* Put icon to view resource clipboard *********************/
/*****************************************************************************/

static void Prg_PutIconToViewResourceClipboard (void)
  {
   Ico_PutContextualIconToViewClipboard (ActSeeRscCli_InRub,NULL,
                                         NULL,NULL);
  }

/*****************************************************************************/
/******************* Put parameters to create a new rubric *******************/
/*****************************************************************************/

static void Rub_PutParsToCreateNewRubric (void *Rubrics)
  {
   if (Rubrics)
      Pag_PutParPagNum (Pag_RUBRICS,((struct Rub_Rubrics *) Rubrics)->CurrentPage);
  }

/*****************************************************************************/
/****************************** Show one rubric ******************************/
/*****************************************************************************/

void Rub_SeeOneRubric (void)
  {
   struct Rub_Rubrics Rubrics;

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);
   Rub_RubricConstructor (&Rubrics.Rubric);

   /***** Get parameters *****/
   Rub_GetPars (&Rubrics,true);

   /***** Get rubric data *****/
   Rub_GetRubricDataByCod (&Rubrics.Rubric);

   /***** Show rubric *****/
   Rub_ShowOnlyOneRubric (&Rubrics);

   /***** Free memory used for rubric *****/
   Rub_RubricDestructor (&Rubrics.Rubric);
  }

/*****************************************************************************/
/******************************* Show one rubric *****************************/
/*****************************************************************************/

void Rub_ShowOnlyOneRubric (struct Rub_Rubrics *Rubrics)
  {
   extern const char *Hlp_ASSESSMENT_Rubrics;
   extern const char *Txt_Rubric;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Rubrics->Rubric.Title[0] ? Rubrics->Rubric.Title :
						 Txt_Rubric,
                 Rub_PutIconsViewingOneRubric,Rubrics,
		 Hlp_ASSESSMENT_Rubrics,Box_NOT_CLOSABLE);

      /***** Show main data of this rubric *****/
      Rub_ShowRubricMainData (Rubrics,
		              true);	// Show only this rubric

      /***** Write criteria of this rubric *****/
      RubCri_ListCriteriaForSeeing (Rubrics);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********* Show a pair of rows with the main data of a given rubric **********/
/*****************************************************************************/

void Rub_ShowRubricMainData (struct Rub_Rubrics *Rubrics,
                             bool ShowOnlyThisRubric)
  {
   extern const char *Txt_View_rubric;
   extern const char *Txt_Number_of_criteria;

   /***** Begin box and table *****/
   if (ShowOnlyThisRubric)
      HTM_TABLE_BeginWidePadding (2);

   /***** Begin first row of this rubric *****/
   HTM_TR_Begin (NULL);

      /***** Icons related to this rubric *****/
      if (!ShowOnlyThisRubric)
	{
	 HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL %s\"",
		       The_GetColorRows ());
	    Rub_PutIconsToRemEditOneRubric (Rubrics);
	 HTM_TD_End ();
	}

      /***** Rubric title and main data *****/
      if (ShowOnlyThisRubric)
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
      else
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",The_GetColorRows ());

      /* Rubric title */
      Frm_BeginForm (ActSeeOneRub);
	 Rub_PutPars (Rubrics);
	 HTM_BUTTON_Submit_Begin (Txt_View_rubric,"class=\"LT BT_LINK ASG_TITLE_%s\"",
				  The_GetSuffix ());
	    HTM_Txt (Rubrics->Rubric.Title);
	 HTM_BUTTON_End ();
      Frm_EndForm ();

      /* Number of criteria */
      HTM_DIV_Begin ("class=\"ASG_GRP_%s\"",The_GetSuffix ());
	 HTM_TxtColonNBSP (Txt_Number_of_criteria);
	 // HTM_Unsigned (Rubrics->Rubric.NumCriteria);
	 HTM_Unsigned (Rub_DB_GetNumCriteriaInRubric (Rubrics->Rubric.RubCod));
      HTM_DIV_End ();

      HTM_TD_End ();

   /***** End 1st row of this rubric *****/
   HTM_TR_End ();

   /***** Begin 2nd row of this rubric *****/
   HTM_TR_Begin (NULL);

      /***** Author of the rubric *****/
      if (ShowOnlyThisRubric)
	 HTM_TD_Begin ("class=\"LT\"");
      else
	 HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
      Rub_WriteAuthor (&Rubrics->Rubric);
      HTM_TD_End ();

      /***** Text of the rubric *****/
      if (ShowOnlyThisRubric)
	 HTM_TD_Begin ("class=\"LT\"");
      else
	 HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			Rubrics->Rubric.Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      ALn_InsertLinks (Rubrics->Rubric.Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
      HTM_DIV_Begin ("class=\"PAR DAT_%s\"",The_GetSuffix ());
	 HTM_Txt (Rubrics->Rubric.Txt);
      HTM_DIV_End ();
      HTM_TD_End ();

   /***** End 2nd row of this rubric *****/
   HTM_TR_End ();

   /***** End table *****/
   if (ShowOnlyThisRubric)
      HTM_TABLE_End ();
   else
      The_ChangeRowColor ();
  }

/*****************************************************************************/
/************************ Show one rubric in a project ***********************/
/*****************************************************************************/

void Rub_ShowRubricInProject (struct Prj_Projects *Projects,
			      struct Rub_Rubric *Rubric,
			      const char *WhichRubricTxt,
			      bool ICanFill)
  {
   extern const char *Txt_Rubric;

   /***** Begin first row of this rubric *****/
   HTM_TR_Begin (NULL);

      /***** Rubric title *****/
      HTM_TD_Begin ("colspan=\"8\" class=\"LT ASG_TITLE_%s %s\"",
                    The_GetSuffix (),The_GetColorRows ());
	 HTM_TxtColonNBSP (WhichRubricTxt);
	 HTM_Txt (Rubric->Title);
      HTM_TD_End ();

   /***** End 1st row of this rubric *****/
   HTM_TR_End ();

   /***** Begin 2nd row of this rubric *****/
   HTM_TR_Begin (NULL);

      /***** Text of the rubric *****/
      HTM_TD_Begin ("colspan=\"8\" class=\"LT PAR DAT_%s %s\"",
                    The_GetSuffix (),The_GetColorRows ());
	 Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			   Rubric->Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
	 ALn_InsertLinks (Rubric->Txt,Cns_MAX_BYTES_TEXT,60);		// Insert links
	 HTM_Txt (Rubric->Txt);
      HTM_TD_End ();

   /***** End 2nd row of this rubric *****/
   HTM_TR_End ();

   /***** Write criteria of this rubric *****/
   RubCri_ListCriteriaInProject (Projects,Rubric->RubCod,ICanFill);

   /***** Change color for next rubric *****/
   The_ChangeRowColor ();
  }

/*****************************************************************************/
/*************** Put icons when viewing or editing one rubric ****************/
/*****************************************************************************/

static void Rub_PutIconsViewingOneRubric (void *Rubrics)
  {
   if (Rubrics)
      Rub_PutIconsToRemEditOneRubric (Rubrics);
  }

static void Rub_PutIconsEditingOneRubric (void *Rubrics)
  {
   if (Rubrics)
      /***** Icon to view rubric *****/
      Ico_PutContextualIconToView (ActSeeOneRub,NULL,
				   Rub_PutPars,Rubrics);
  }

static void Rub_PutIconsToRemEditOneRubric (struct Rub_Rubrics *Rubrics)
  {
   if (Rub_CheckIfICanEditRubrics ())
     {
      /***** Icon to remove rubric *****/
      Ico_PutContextualIconToRemove (ActReqRemRub,NULL,
				     Rub_PutPars,Rubrics);

      /***** Icon to edit rubric *****/
      Ico_PutContextualIconToEdit (ActEdiOneRub,NULL,
				   Rub_PutPars,Rubrics);

      /***** Link to get resource link *****/
      if (Rsc_CheckIfICanGetLink ())
	 Ico_PutContextualIconToGetLink (ActReqLnkRub,NULL,
					 Rub_PutPars,Rubrics);
     }
  }

/*****************************************************************************/
/*********************** Write the author of a rubric ************************/
/*****************************************************************************/

static void Rub_WriteAuthor (const struct Rub_Rubric *Rubric)
  {
   Usr_WriteAuthor1Line (Rubric->UsrCod,
                         false);	// Not hidden
  }

/*****************************************************************************/
/*********************** Params used to edit a rubric ************************/
/*****************************************************************************/

void Rub_PutPars (void *Rubrics)
  {
   if (Rubrics)
     {
      ParCod_PutPar (ParCod_Rub,((struct Rub_Rubrics *) Rubrics)->Rubric.RubCod);
      Pag_PutParPagNum (Pag_RUBRICS,((struct Rub_Rubrics *) Rubrics)->CurrentPage);
     }
  }

/*****************************************************************************/
/****************** Get parameters used to edit a rubric *********************/
/*****************************************************************************/

void Rub_GetPars (struct Rub_Rubrics *Rubrics,bool CheckRubCod)
  {
   long (*GetExaCo[2]) (ParCod_Param_t ParCode) =
     {
      [false] = ParCod_GetPar,
      [true ] = ParCod_GetAndCheckPar,
     };

   /***** Get other parameters *****/
   Rubrics->CurrentPage = Pag_GetParPagNum (Pag_RUBRICS);

   /***** Get rubric code *****/
   Rubrics->Rubric.RubCod = GetExaCo[CheckRubCod] (ParCod_Rub);
  }

/*****************************************************************************/
/************************* Get list of all rubrics ***************************/
/*****************************************************************************/
// Txt is not retrieved from database

void Rub_GetListRubrics (struct Rub_Rubrics *Rubrics)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRubric;

   /***** Free list of rubrics *****/
   if (Rubrics->LstIsRead)
      Rub_FreeListRubrics (Rubrics);

   /***** Get list of rubrics from database *****/
   if ((Rubrics->Num = Rub_DB_GetListRubrics (&mysql_res))) // Rubrics found...
     {
      /***** Create list of rubrics *****/
      if ((Rubrics->Lst = malloc ((size_t) Rubrics->Num *
                                  sizeof (*Rubrics->Lst))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the rubrics *****/
      for (NumRubric = 0;
	   NumRubric < Rubrics->Num;
	   NumRubric++)
        {
	 /* Get row */
	 row = mysql_fetch_row (mysql_res);

	 /* Get code of the rubric (row[0]) */
	 Rubrics->Lst[NumRubric] = Str_ConvertStrCodToLongCod (row[0]);
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Rubrics->LstIsRead = true;
  }

/*****************************************************************************/
/*********************** Get rubric data using its code **********************/
/*****************************************************************************/

void Rub_GetRubricDataByCod (struct Rub_Rubric *Rubric)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get data of rubric from database *****/
   if (Rub_DB_GetRubricDataByCod (&mysql_res,Rubric->RubCod)) // Rubric found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get code of the rubric (row[0]), course (row[1] and author (row[2]) */
      Rubric->RubCod = Str_ConvertStrCodToLongCod (row[0]);
      Rubric->CrsCod = Str_ConvertStrCodToLongCod (row[1]);
      Rubric->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get the title of the rubric (row[3]) */
      Str_Copy (Rubric->Title,row[3],sizeof (Rubric->Title) - 1);

      /* Get rubric text */
      Rub_DB_GetRubricTxt (Rubric);
     }
   else
     {
      /***** Initialize to empty rubric *****/
      Rubric->RubCod   = -1L;
      Rubric->CrsCod   = -1L;
      Rubric->UsrCod   = -1L;
      Rubric->Title[0] = '\0';
      Rubric->Txt[0]   = '\0';
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************************** Free list of rubrics ****************************/
/*****************************************************************************/

void Rub_FreeListRubrics (struct Rub_Rubrics *Rubrics)
  {
   if (Rubrics->LstIsRead && Rubrics->Lst)
     {
      /***** Free memory used by the list of rubrics *****/
      free (Rubrics->Lst);
      Rubrics->Lst       = NULL;
      Rubrics->Num       = 0;
      Rubrics->LstIsRead = false;
     }
  }

/*****************************************************************************/
/************** Ask for confirmation of removing of a rubric *****************/
/*****************************************************************************/

void Rub_AskRemRubric (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_rubric_X;
   extern const char *Txt_Remove_rubric;
   struct Rub_Rubrics Rubrics;

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);
   Rub_RubricConstructor (&Rubrics.Rubric);

   /***** Get parameters *****/
   Rub_GetPars (&Rubrics,true);

   /***** Get data of the rubric from database *****/
   Rub_GetRubricDataByCod (&Rubrics.Rubric);
   if (!Rub_CheckIfICanEditRubrics ())
      Err_NoPermissionExit ();

   /***** Show criterion and button to remove rubric *****/
   Ale_ShowAlertAndButton (ActRemRub,NULL,NULL,
                           Rub_PutPars,&Rubrics,
			   Btn_REMOVE_BUTTON,Txt_Remove_rubric,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_rubric_X,
                           Rubrics.Rubric.Title);

   /***** Show rubrics again *****/
   Rub_ListAllRubrics (&Rubrics);

   /***** Free memory used for rubric *****/
   Rub_RubricDestructor (&Rubrics.Rubric);
  }

/*****************************************************************************/
/****************************** Remove a rubric ******************************/
/*****************************************************************************/

void Rub_RemoveRubric (void)
  {
   extern const char *Txt_Rubric_X_removed;
   struct Rub_Rubrics Rubrics;

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);
   Rub_RubricConstructor (&Rubrics.Rubric);

   /***** Get parameters *****/
   Rub_GetPars (&Rubrics,true);

   /***** Get data of the rubric from database *****/
   Rub_GetRubricDataByCod (&Rubrics.Rubric);
   if (!Rub_CheckIfICanEditRubrics ())
      Err_NoPermissionExit ();

   /***** Remove rubric from all tables *****/
   Rub_RemoveRubricFromAllTables (Rubrics.Rubric.RubCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Rubric_X_removed,Rubrics.Rubric.Title);

   /***** Show rubrics again *****/
   Rub_ListAllRubrics (&Rubrics);

   /***** Free memory used for rubric *****/
   Rub_RubricDestructor (&Rubrics.Rubric);
  }

/*****************************************************************************/
/********************** Remove rubric from all tables ************************/
/*****************************************************************************/

static void Rub_RemoveRubricFromAllTables (long RubCod)
  {
   /***** Remove rubric criteria *****/
   Rub_DB_RemoveRubricCriteria (RubCod);

   /***** Remove rubric *****/
   Rub_DB_RemoveRubric (RubCod);
  }

/*****************************************************************************/
/********************** Remove all rubrics of a course ***********************/
/*****************************************************************************/

void Rub_RemoveCrsRubrics (long CrsCod)
  {
   /***** Remove the criteria in rubrics *****/
   Rub_DB_RemoveCrsRubricCriteria (CrsCod);

   /***** Remove the rubrics *****/
   Rub_DB_RemoveCrsRubrics (CrsCod);
  }

/*****************************************************************************/
/*************** Request the creation or edition of a rubric *****************/
/*****************************************************************************/

void Rub_ReqCreatOrEditRubric (void)
  {
   struct Rub_Rubrics Rubrics;
   Rub_ExistingNewRubric_t ExistingNewRubric;

   /***** Check if I can edit rubrics *****/
   if (!Rub_CheckIfICanEditRubrics ())
      Err_NoPermissionExit ();

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);
   Rub_RubricConstructor (&Rubrics.Rubric);
   RubCri_ResetCriterion (&Rubrics.Criterion);

   /***** Get parameters *****/
   Rub_GetPars (&Rubrics,false);	// Don't check rubric code
   Rubrics.Criterion.RubCod = Rubrics.Rubric.RubCod;
   ExistingNewRubric = (Rubrics.Rubric.RubCod > 0) ? Rub_EXISTING_RUBRIC :
						     Rub_NEW_RUBRIC;

   /***** Get rubric data *****/
   switch (ExistingNewRubric)
     {
      case Rub_EXISTING_RUBRIC:
         /* Get rubric data from database */
         Rub_GetRubricDataByCod (&Rubrics.Rubric);
         break;
      case Rub_NEW_RUBRIC:
         break;
     }

   /***** Put form to create/edit a rubric and show criteria *****/
   Rub_PutFormsOneRubric (&Rubrics,ExistingNewRubric);

   /***** Free memory used for rubric *****/
   Rub_RubricDestructor (&Rubrics.Rubric);
  }

/*****************************************************************************/
/******************** Put forms to create/edit a rubric **********************/
/*****************************************************************************/

void Rub_PutFormsOneRubric (struct Rub_Rubrics *Rubrics,
			    Rub_ExistingNewRubric_t ExistingNewRubric)
  {
   /***** Put form to create/edit a rubric *****/
   Rub_PutFormEditionRubric (Rubrics,ExistingNewRubric);

   /***** Show other lists *****/
   switch (ExistingNewRubric)
     {
      case Rub_EXISTING_RUBRIC:
	 /* Show list of criteria */
	 RubCri_ListCriteriaForEdition (Rubrics);
         break;
      case Rub_NEW_RUBRIC:
	 /* Show rubrics again */
	 Rub_ListAllRubrics (Rubrics);
         break;
     }
  }

/*****************************************************************************/
/******************** Put a form to create/edit a rubric *********************/
/*****************************************************************************/

void Rub_PutFormEditionRubric (struct Rub_Rubrics *Rubrics,
			       Rub_ExistingNewRubric_t ExistingNewRubric)
  {
   extern const char *Hlp_ASSESSMENT_Rubrics_new_rubric;
   extern const char *Hlp_ASSESSMENT_Rubrics_edit_rubric;
   extern const char *Txt_Rubric;
   extern const char *Txt_Title;
   extern const char *Txt_Description;
   extern const char *Txt_Create_rubric;
   extern const char *Txt_Save_changes;
   static void (*FunctionToDrawContextualIcons[]) (void *Args) =
     {
      [Rub_EXISTING_RUBRIC] = Rub_PutIconsEditingOneRubric,
      [Rub_NEW_RUBRIC     ] = NULL,
     };
   static Act_Action_t NextAction[] =
     {
      [Rub_EXISTING_RUBRIC] = ActChgRub,
      [Rub_NEW_RUBRIC     ] = ActNewRub,
     };
   static Btn_Button_t Button[] =
     {
      [Rub_EXISTING_RUBRIC] = Btn_CONFIRM_BUTTON,
      [Rub_NEW_RUBRIC     ] = Btn_CREATE_BUTTON,
     };
   const char *HelpLink[] =
     {
      [Rub_EXISTING_RUBRIC] = Hlp_ASSESSMENT_Rubrics_edit_rubric,
      [Rub_NEW_RUBRIC     ] = Hlp_ASSESSMENT_Rubrics_new_rubric,
     };
   const char *TxtButton[] =
     {
      [Rub_EXISTING_RUBRIC] = Txt_Save_changes,
      [Rub_NEW_RUBRIC     ] = Txt_Create_rubric,
     };

   /***** Begin box *****/
   Box_BoxBegin (NULL,
		 Rubrics->Rubric.Title[0] ? Rubrics->Rubric.Title :
					    Txt_Rubric,
		 FunctionToDrawContextualIcons[ExistingNewRubric],Rubrics,
		 HelpLink[ExistingNewRubric],Box_NOT_CLOSABLE);

      /***** Begin form *****/
      Frm_BeginForm (NextAction[ExistingNewRubric]);
	 Rub_PutPars (Rubrics);

         /***** Begin table *****/
         HTM_TABLE_BeginWidePadding (2);

	    /***** Rubric title *****/
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("RT","Title",Txt_Title);

	       /* Data */
	       HTM_TD_Begin ("class=\"LT\"");
		  HTM_INPUT_TEXT ("Title",Rub_MAX_CHARS_TITLE,Rubrics->Rubric.Title,
				  HTM_DONT_SUBMIT_ON_CHANGE,
				  "id=\"Title\""
				  " class=\"TITLE_DESCRIPTION_WIDTH INPUT_%s\""
				  " required=\"required\"",
				  The_GetSuffix ());
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** Rubric text *****/
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("RT","Txt",Txt_Description);

	       /* Data */
	       HTM_TD_Begin ("class=\"LT\"");
		  HTM_TEXTAREA_Begin ("id=\"Txt\" name=\"Txt\" rows=\"5\""
				      " class=\"TITLE_DESCRIPTION_WIDTH INPUT_%s\"",
				      The_GetSuffix ());
		     HTM_Txt (Rubrics->Rubric.Txt);
		  HTM_TEXTAREA_End ();
	       HTM_TD_End ();

	    HTM_TR_End ();

	 /***** End table ****/
	 HTM_TABLE_End ();

	 /***** Send button *****/
	 Btn_PutButton (Button[ExistingNewRubric],
			TxtButton[ExistingNewRubric]);

      /***** End form *****/
      Frm_EndForm ();

   /***** End box ****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* Receive form to create a new rubric *******************/
/*****************************************************************************/

void Rub_ReceiveFormRubric (void)
  {
   struct Rub_Rubrics Rubrics;
   Rub_ExistingNewRubric_t ExistingNewRubric;

   /***** Check if I can edit rubrics *****/
   if (!Rub_CheckIfICanEditRubrics ())
      Err_NoPermissionExit ();

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);
   Rub_RubricConstructor (&Rubrics.Rubric);
   RubCri_ResetCriterion (&Rubrics.Criterion);

   /***** Get parameters *****/
   Rub_GetPars (&Rubrics,false);
   Rubrics.Criterion.RubCod = Rubrics.Rubric.RubCod;
   ExistingNewRubric = (Rubrics.Rubric.RubCod > 0) ? Rub_EXISTING_RUBRIC :
						     Rub_NEW_RUBRIC;

   /***** Get all current rubric data from database *****/
   // Some data, not received from form,
   // are necessary to show rubric and criteria again
   switch (ExistingNewRubric)
     {
      case Rub_EXISTING_RUBRIC:
         Rub_GetRubricDataByCod (&Rubrics.Rubric);
         break;
      case Rub_NEW_RUBRIC:
         break;
     }

   /***** Overwrite some rubric data with the data received from form *****/
   Rub_ReceiveRubricFieldsFromForm (&Rubrics.Rubric);
   if (Rub_CheckRubricFieldsReceivedFromForm (&Rubrics.Rubric))
     {
      /***** Create a new rubric or update an existing one *****/
      switch (ExistingNewRubric)
	{
	 case Rub_EXISTING_RUBRIC:
	    Rub_UpdateRubric (&Rubrics.Rubric);	// Update rubric data in database
	    break;
	 case Rub_NEW_RUBRIC:
	    Rub_CreateRubric (&Rubrics.Rubric);	// Add new rubric to database
	    ExistingNewRubric = Rub_EXISTING_RUBRIC;
	    break;
	}
     }

   /***** Show pending alerts */
   Ale_ShowAlerts (NULL);

   /***** Show current rubric and its criteria *****/
   Rub_PutFormsOneRubric (&Rubrics,ExistingNewRubric);

   /***** Free memory used for rubric *****/
   Rub_RubricDestructor (&Rubrics.Rubric);
  }

static void Rub_ReceiveRubricFieldsFromForm (struct Rub_Rubric *Rubric)
  {
   /***** Get rubric title *****/
   Par_GetParText ("Title",Rubric->Title,Rub_MAX_BYTES_TITLE);

   /***** Get rubric text *****/
   Par_GetParHTML ("Txt",Rubric->Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)
  }

static bool Rub_CheckRubricFieldsReceivedFromForm (const struct Rub_Rubric *Rubric)
  {
   extern const char *Txt_Already_existed_a_game_with_the_title_X;
   bool NewRubricIsCorrect;

   /***** Check if title is correct *****/
   NewRubricIsCorrect = true;
   if (Rubric->Title[0])	// If there's a rubric title
     {
      /* If title of rubric was in database... */
      if (Rub_DB_CheckIfSimilarRubricExists (Rubric))
	{
	 NewRubricIsCorrect = false;
	 Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_a_game_with_the_title_X,
			Rubric->Title);
	}
     }
   else	// If there is not a rubric title
     {
      NewRubricIsCorrect = false;
      Ale_CreateAlertYouMustSpecifyTheTitle ();
     }

   return NewRubricIsCorrect;
  }

/*****************************************************************************/
/**************************** Create a new rubric ****************************/
/*****************************************************************************/

static void Rub_CreateRubric (struct Rub_Rubric *Rubric)
  {
   extern const char *Txt_Created_new_rubric_X;

   /***** Create a new rubric *****/
   Rubric->RubCod = Rub_DB_CreateRubric (Rubric);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_rubric_X,Rubric->Title);
  }

/*****************************************************************************/
/************************* Update an existing rubric *************************/
/*****************************************************************************/

static void Rub_UpdateRubric (struct Rub_Rubric *Rubric)
  {
   extern const char *Txt_The_rubric_has_been_modified;

   /***** Update the data of the rubric *****/
   Rub_DB_UpdateRubric (Rubric);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_rubric_has_been_modified);
  }

/*****************************************************************************/
/************************** Show stats about rubrics *************************/
/*****************************************************************************/

void Rub_GetAndShowRubricsStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_rubrics;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Number_of_BR_rubrics;
   extern const char *Txt_Number_of_BR_courses_with_BR_rubrics;
   extern const char *Txt_Average_number_BR_of_rubrics_BR_per_course;
   extern const char *Txt_Average_number_BR_of_criteria_BR_per_rubric;
   unsigned NumRubrics;
   unsigned NumCoursesWithRubrics = 0;
   double NumRubricsPerCourse = 0.0;
   double NumCriteriaPerRubric = 0.0;

   /***** Get the number of rubrics from this location *****/
   if ((NumRubrics = Rub_DB_GetNumRubrics (Gbl.Scope.Current)))
     {
      if ((NumCoursesWithRubrics = Rub_DB_GetNumCoursesWithRubrics (Gbl.Scope.Current)) != 0)
         NumRubricsPerCourse = (double) NumRubrics /
			       (double) NumCoursesWithRubrics;
      NumCriteriaPerRubric = Rub_DB_GetNumCriteriaPerRubric (Gbl.Scope.Current);
     }

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_FIGURE_TYPES[Fig_RUBRICS],
                      NULL,NULL,
                      Hlp_ANALYTICS_Figures_rubrics,Box_NOT_CLOSABLE,2);

      /***** Write table heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Number_of_BR_rubrics                       ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_BR_courses_with_BR_rubrics       ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Average_number_BR_of_rubrics_BR_per_course ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Average_number_BR_of_criteria_BR_per_rubric,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Write number of rubrics *****/
      HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (NumRubrics);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (NumCoursesWithRubrics);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Double2Decimals (NumRubricsPerCourse);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Double2Decimals (NumCriteriaPerRubric);
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
