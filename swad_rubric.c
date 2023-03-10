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
static void Rub_PutButtonToCreateNewRubric (struct Rub_Rubrics *Rubrics);
static void Rub_PutParsToCreateNewRubric (void *Rubrics);

static void Rub_ShowRubricMainData (struct Rub_Rubrics *Rubrics,
                                    bool ShowOnlyThisRubric);

static void Rub_PutIconsOneRubric (void *Rubrics);
static void Rub_WriteAuthor (struct Rub_Rubric *Rubric);

static void Rub_PutIconsToRemEditOneRubric (struct Rub_Rubrics *Rubrics);

static void Rub_RemoveRubricFromAllTables (long RubCod);

static void Rub_PutFormsEditionRubric (struct Rub_Rubrics *Rubrics,
				       char Txt[Cns_MAX_BYTES_TEXT + 1],
				       bool ItsANewRubric);
static void Rub_ReceiveRubricFieldsFromForm (struct Rub_Rubric *Rubric,
				             char Txt[Cns_MAX_BYTES_TEXT + 1]);
static bool Rub_CheckRubricFieldsReceivedFromForm (const struct Rub_Rubric *Rubric);

static void Rub_CreateRubric (struct Rub_Rubric *Rubric,const char *Txt);
static void Rub_UpdateRubric (struct Rub_Rubric *Rubric,const char *Txt);

/*****************************************************************************/
/*************************** Reset rubrics context ***************************/
/*****************************************************************************/

void Rub_ResetRubrics (struct Rub_Rubrics *Rubrics)
  {
   Rubrics->LstIsRead     = false;	// List not read from database...
   Rubrics->Num           = 0;		// Total number of rubrics
   Rubrics->Lst           = NULL;	// List of rubrics
   Rubrics->CurrentPage   = 0;
   Rubrics->Rubric.RubCod = -1L;	// Current/selected rubric code
  }

/*****************************************************************************/
/*************************** Initialize rubric to empty **********************/
/*****************************************************************************/

void Rub_ResetRubric (struct Rub_Rubric *Rubric)
  {
   /***** Initialize to empty rubric *****/
   Rubric->RubCod      = -1L;
   Rubric->CrsCod      = -1L;
   Rubric->UsrCod      = -1L;
   Rubric->Title[0]    = '\0';
   Rubric->NumCriteria = 0;
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
   Rub_GetPars (&Rubrics);	// Return value ignored

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

   /***** Reset rubric *****/
   Rub_ResetRubric (&Rubrics->Rubric);

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
               /*
	       HTM_TH_Begin (HTM_HEAD_LEFT);
	          HTM_Txt (Txt_Rubric);
	       HTM_TH_End ();
	       */
	       HTM_TH_Span (Txt_Rubric,HTM_HEAD_LEFT,1,2,NULL);

	       // HTM_TH (Txt_Criteria,HTM_HEAD_RIGHT);

	    HTM_TR_End ();

	    /***** Write all rubrics *****/
	    for (NumRubric  = Pagination.FirstItemVisible;
		 NumRubric <= Pagination.LastItemVisible;
		 NumRubric++)
	      {
	       /* Get data of this rubric */
	       Rubrics->Rubric.RubCod = Rubrics->Lst[NumRubric - 1];
	       Rub_GetDataOfRubricByCod (&Rubrics->Rubric);

	       /* Show a pair of rows with the main data of this rubric */
	       Rub_ShowRubricMainData (Rubrics,
				       false);	// Do not show only this rubric
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();
	}
      else	// No rubrics created
	 Ale_ShowAlert (Ale_INFO,Txt_No_rubrics);

      /***** Write again links to pages *****/
      Pag_WriteLinksToPagesCentered (Pag_RUBRICS,&Pagination,
				     Rubrics,-1L);

      /***** Button to create a new rubric *****/
      if (Rub_CheckIfICanEditRubrics ())
	 Rub_PutButtonToCreateNewRubric (Rubrics);

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
/****************** Put contextual icons in list of rubrics ******************/
/*****************************************************************************/

static void Rub_PutIconsListRubrics (void *Rubrics)
  {
   if (Rubrics)
     {
      /***** Put icon to create a new rubric *****/
      if (Rub_CheckIfICanEditRubrics ())
	 Rub_PutIconToCreateNewRubric ((struct Rub_Rubrics *) Rubrics);

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
/******************** Put button to create a new rubric **********************/
/*****************************************************************************/

static void Rub_PutButtonToCreateNewRubric (struct Rub_Rubrics *Rubrics)
  {
   extern const char *Txt_New_rubric;

   Frm_BeginForm (ActFrmNewRub);
      Rub_PutParsToCreateNewRubric (Rubrics);

      Btn_PutConfirmButton (Txt_New_rubric);

   Frm_EndForm ();
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

   /***** Reset rubric *****/
   Rub_ResetRubric (&Rubrics.Rubric);

   /***** Get parameters *****/
   if ((Rubrics.Rubric.RubCod = Rub_GetPars (&Rubrics)) <= 0)
      Err_WrongRubricExit ();
   Rub_GetDataOfRubricByCod (&Rubrics.Rubric);

   /***** Show rubric *****/
   Rub_ShowOnlyOneRubric (&Rubrics,
                          false);	// Do not list rubric criteria
  }

/*****************************************************************************/
/******************************* Show one rubric *****************************/
/*****************************************************************************/

void Rub_ShowOnlyOneRubric (struct Rub_Rubrics *Rubrics,
			    bool ListRubricCriteria)
  {
   Rub_ShowOnlyOneRubricBegin (Rubrics,ListRubricCriteria);
   Rub_ShowOnlyOneRubricEnd ();
  }

void Rub_ShowOnlyOneRubricBegin (struct Rub_Rubrics *Rubrics,
			         bool ListRubricCriteria)
  {
   extern const char *Hlp_ASSESSMENT_Rubrics;
   extern const char *Txt_Rubric;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Rubric,
                 Rub_PutIconsOneRubric,Rubrics,
		 Hlp_ASSESSMENT_Rubrics,Box_NOT_CLOSABLE);

      /***** Show main data of this rubric *****/
      Rub_ShowRubricMainData (Rubrics,
		              true);	// Show only this rubric

      /***** Write criteria of this rubric *****/
      if (ListRubricCriteria)
	 Ale_ShowAlert (Ale_INFO,"The criteria of the rubric should be listed here.");
	 // Rub_ListRubricCriteria (Rubrics);
  }

void Rub_ShowOnlyOneRubricEnd (void)
  {
   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********* Show a pair of rows with the main data of a given rubric **********/
/*****************************************************************************/

static void Rub_ShowRubricMainData (struct Rub_Rubrics *Rubrics,
                                    bool ShowOnlyThisRubric)
  {
   extern const char *Txt_View_rubric;
   extern const char *Txt_Number_of_criteria;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

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
      Frm_BeginForm (ActSeeRub);
	 Rub_PutPars (Rubrics);
	 HTM_BUTTON_Submit_Begin (Txt_View_rubric,"class=\"LT BT_LINK ASG_TITLE_%s\"",
				  The_GetSuffix ());
	    HTM_Txt (Rubrics->Rubric.Title);
	 HTM_BUTTON_End ();
      Frm_EndForm ();

      /* Number of criteria */
      HTM_DIV_Begin ("class=\"ASG_GRP_%s\"",The_GetSuffix ());
	 HTM_TxtColonNBSP (Txt_Number_of_criteria);
	 HTM_Unsigned (Rubrics->Rubric.NumCriteria);
      HTM_DIV_End ();

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
      Rub_DB_GetRubricTxt (Rubrics->Rubric.RubCod,Txt);
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      ALn_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
      HTM_DIV_Begin ("class=\"PAR DAT_%s\"",The_GetSuffix ());
	 HTM_Txt (Txt);
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
/******************* Put icons to remove/edit one rubric *********************/
/*****************************************************************************/

static void Rub_PutIconsOneRubric (void *Rubrics)
  {
   if (Rubrics)
      Rub_PutIconsToRemEditOneRubric (Rubrics);
  }

/*****************************************************************************/
/*********************** Write the author of a rubric ************************/
/*****************************************************************************/

static void Rub_WriteAuthor (struct Rub_Rubric *Rubric)
  {
   Usr_WriteAuthor1Line (Rubric->UsrCod,
                         false);	// Not hidden
  }

/*****************************************************************************/
/******************* Put icons to remove/edit one rubric *********************/
/*****************************************************************************/

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
     }
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

long Rub_GetPars (struct Rub_Rubrics *Rubrics)
  {
   /***** Get other parameters *****/
   Rubrics->CurrentPage = Pag_GetParPagNum (Pag_RUBRICS);

   /***** Get rubric code *****/
   return ParCod_GetPar (ParCod_Rub);
  }

/*****************************************************************************/
/************************* Get list of all rubrics ***************************/
/*****************************************************************************/

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

      /***** Get the rubrics codes *****/
      for (NumRubric = 0;
	   NumRubric < Rubrics->Num;
	   NumRubric++)
        {
         /* Get next rubric code (row[0]) */
         row = mysql_fetch_row (mysql_res);
         if ((Rubrics->Lst[NumRubric] = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Err_WrongRubricExit ();
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Rubrics->LstIsRead = true;
  }

/*****************************************************************************/
/********************* Get rubric data using its code ************************/
/*****************************************************************************/

void Rub_GetDataOfRubricByCod (struct Rub_Rubric *Rubric)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get data of rubric from database *****/
   if (Rub_DB_GetDataOfRubricByCod (&mysql_res,Rubric->RubCod)) // Rubric found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get code of the rubric (row[0]) */
      Rubric->RubCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get code of the course (row[1]) */
      Rubric->CrsCod = Str_ConvertStrCodToLongCod (row[1]);

      /* Get author of the rubric (row[2]) */
      Rubric->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get the title of the rubric (row[3]) */
      Str_Copy (Rubric->Title,row[3],sizeof (Rubric->Title) - 1);

      /* Get number of criteria */
      Rubric->NumCriteria = Rub_DB_GetNumCriteriaInRubric (Rubric->RubCod);
     }
   else
      /* Initialize to empty rubric */
      Rub_ResetRubric (Rubric);

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

   /***** Reset rubric *****/
   Rub_ResetRubric (&Rubrics.Rubric);

   /***** Get parameters *****/
   if ((Rubrics.Rubric.RubCod = Rub_GetPars (&Rubrics)) <= 0)
      Err_WrongRubricExit ();

   /***** Get data of the rubric from database *****/
   Rub_GetDataOfRubricByCod (&Rubrics.Rubric);
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

   /***** Reset rubric *****/
   Rub_ResetRubric (&Rubrics.Rubric);

   /***** Get rubric code *****/
   Rubrics.Rubric.RubCod = ParCod_GetAndCheckPar (ParCod_Rub);

   /***** Get data of the rubric from database *****/
   Rub_GetDataOfRubricByCod (&Rubrics.Rubric);
   if (!Rub_CheckIfICanEditRubrics ())
      Err_NoPermissionExit ();

   /***** Remove rubric from all tables *****/
   Rub_RemoveRubricFromAllTables (Rubrics.Rubric.RubCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Rubric_X_removed,
                  Rubrics.Rubric.Title);

   /***** Show rubrics again *****/
   Rub_ListAllRubrics (&Rubrics);
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
/************************ List the criteria in a rubric **********************/
/*****************************************************************************/

void Rub_ListRubric (void)
  {
   struct Rub_Rubrics Rubrics;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);

   /***** Reset rubric *****/
   Rub_ResetRubric (&Rubrics.Rubric);

   /***** Get parameters *****/
   if ((Rubrics.Rubric.RubCod = Rub_GetPars (&Rubrics)) <= 0)
      Err_WrongRubricExit ();

   /***** Get rubric data *****/
   Rub_GetDataOfRubricByCod (&Rubrics.Rubric);
   Rub_DB_GetRubricTxt (Rubrics.Rubric.RubCod,Txt);

   /***** Show rubric *****/
   Rub_ShowOnlyOneRubric (&Rubrics,
                          true);		// List rubric criteria
  }

/*****************************************************************************/
/*************** Request the creation or edition of a rubric *****************/
/*****************************************************************************/

void Rub_RequestCreatOrEditRubric (void)
  {
   struct Rub_Rubrics Rubrics;
   bool ItsANewRubric;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);

   /***** Reset rubric *****/
   Rub_ResetRubric (&Rubrics.Rubric);

   /***** Check if I can edit rubrics *****/
   if (!Rub_CheckIfICanEditRubrics ())
      Err_NoPermissionExit ();

   /***** Get parameters *****/
   ItsANewRubric = ((Rubrics.Rubric.RubCod = Rub_GetPars (&Rubrics)) <= 0);

   /***** Get rubric data *****/
   if (ItsANewRubric)
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
   Rub_PutFormsEditionRubric (&Rubrics,Txt,ItsANewRubric);

   /***** Show rubrics or criteria *****/
   if (ItsANewRubric)
      /* Show rubrics again */
      Rub_ListAllRubrics (&Rubrics);
   else
      /* Show criteria of the rubric ready to be edited */
      RubCri_ListRubricCriteria (&Rubrics);
  }

/*****************************************************************************/
/******************** Put a form to create/edit a rubric *********************/
/*****************************************************************************/

static void Rub_PutFormsEditionRubric (struct Rub_Rubrics *Rubrics,
				       char Txt[Cns_MAX_BYTES_TEXT + 1],
				       bool ItsANewRubric)
  {
   extern const char *Hlp_ASSESSMENT_Rubrics_new_rubric;
   extern const char *Hlp_ASSESSMENT_Rubrics_edit_rubric;
   extern const char *Txt_New_rubric;
   extern const char *Txt_Edit_rubric;
   extern const char *Txt_Title;
   extern const char *Txt_Description;
   extern const char *Txt_Create_rubric;
   extern const char *Txt_Save_changes;

   /***** Begin form *****/
   Frm_BeginForm (ItsANewRubric ? ActNewRub :
				  ActChgRub);
      Rub_PutPars (Rubrics);

      /***** Begin box and table *****/
      if (ItsANewRubric)
	 Box_BoxTableBegin (NULL,Txt_New_rubric,
			    NULL,NULL,
			    Hlp_ASSESSMENT_Rubrics_new_rubric,Box_NOT_CLOSABLE,2);
      else
	 Box_BoxTableBegin (NULL,
			    Rubrics->Rubric.Title[0] ? Rubrics->Rubric.Title :
					               Txt_Edit_rubric,
			    NULL,NULL,
			    Hlp_ASSESSMENT_Rubrics_edit_rubric,Box_NOT_CLOSABLE,2);

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
	       HTM_Txt (Txt);
	    HTM_TEXTAREA_End ();
	 HTM_TD_End ();

      HTM_TR_End ();

      /***** End table, send button and end box *****/
      if (ItsANewRubric)
	 Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_rubric);
      else
	 Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/********************* Receive form to create a new rubric *******************/
/*****************************************************************************/

void Rub_ReceiveFormRubric (void)
  {
   struct Rub_Rubrics Rubrics;
   bool ItsANewRubric;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);

   /***** Reset rubric *****/
   Rub_ResetRubric (&Rubrics.Rubric);

   /***** Check if I can edit rubrics *****/
   if (!Rub_CheckIfICanEditRubrics ())
      Err_NoPermissionExit ();

   /***** Get parameters *****/
   ItsANewRubric = ((Rubrics.Rubric.RubCod = Rub_GetPars (&Rubrics)) <= 0);

   /***** If I can edit rubrics ==> receive rubric from form *****/
   if (Rub_CheckIfICanEditRubrics ())
     {
      Rub_ReceiveRubricFieldsFromForm (&Rubrics.Rubric,Txt);
      if (Rub_CheckRubricFieldsReceivedFromForm (&Rubrics.Rubric))
	{
         /***** Create a new rubric or update an existing one *****/
	 if (ItsANewRubric)
	    Rub_CreateRubric (&Rubrics.Rubric,Txt);	// Add new rubric to database
	 else
	    Rub_UpdateRubric (&Rubrics.Rubric,Txt);	// Update rubric data in database

         /***** Put forms to edit the rubric created or updated *****/
         Rub_PutFormsEditionRubric (&Rubrics,Txt,
                                    false);	// No new rubric

         /***** Show criteria of the rubric ready to be edited ******/
         RubCri_ListRubricCriteria (&Rubrics);
	}
      else
	{
         /***** Put forms to create/edit the rubric *****/
         Rub_PutFormsEditionRubric (&Rubrics,Txt,ItsANewRubric);

         /***** Show rubrics or criteria *****/
         if (ItsANewRubric)
            /* Show rubrics again */
            Rub_ListAllRubrics (&Rubrics);
         else
            /* Show criteria of the rubric ready to be edited */
            RubCri_ListRubricCriteria (&Rubrics);
	}
     }
   else
      Err_NoPermissionExit ();
  }

static void Rub_ReceiveRubricFieldsFromForm (struct Rub_Rubric *Rubric,
				             char Txt[Cns_MAX_BYTES_TEXT + 1])
  {
   /***** Get rubric title *****/
   Par_GetParText ("Title",Rubric->Title,Rub_MAX_BYTES_TITLE);

   /***** Get rubric text *****/
   Par_GetParHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)
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
      Ale_ShowAlertYouMustSpecifyTheTitle ();
     }

   return NewRubricIsCorrect;
  }

/*****************************************************************************/
/**************************** Create a new rubric ****************************/
/*****************************************************************************/

static void Rub_CreateRubric (struct Rub_Rubric *Rubric,const char *Txt)
  {
   extern const char *Txt_Created_new_rubric_X;

   /***** Create a new rubric *****/
   Rubric->RubCod = Rub_DB_CreateRubric (Rubric,Txt);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_rubric_X,Rubric->Title);
  }

/*****************************************************************************/
/************************* Update an existing rubric *************************/
/*****************************************************************************/

static void Rub_UpdateRubric (struct Rub_Rubric *Rubric,const char *Txt)
  {
   extern const char *Txt_The_rubric_has_been_modified;

   /***** Update the data of the rubric *****/
   Rub_DB_UpdateRubric (Rubric,Txt);

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
