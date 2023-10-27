// swad_survey.c: surveys

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
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_attendance.h"
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_group_database.h"
#include "swad_hidden_visible.h"
#include "swad_HTML.h"
#include "swad_notification.h"
#include "swad_notification_database.h"
#include "swad_pagination.h"
#include "swad_parameter_code.h"
#include "swad_program.h"
#include "swad_program_database.h"
#include "swad_role.h"
#include "swad_setting.h"
#include "swad_survey.h"
#include "swad_survey_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Svy_MAX_CHARS_ANSWER	(1024 - 1)	// 1023
#define Svy_MAX_BYTES_ANSWER	((Svy_MAX_CHARS_ANSWER + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 16383

#define Svy_MAX_BYTES_LIST_ANSWER_TYPES	(Svy_NUM_ANS_TYPES * (Cns_MAX_DECIMAL_DIGITS_UINT + 1))

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static bool Svy_CheckIfICanCreateSvy (void);
static void Svy_PutIconsListSurveys (void *Surveys);
static void Svy_PutIconToCreateNewSvy (struct Svy_Surveys *Surveys);
static void Svy_PutParsToCreateNewSvy (void *Surveys);
static void Svy_ParsWhichGroupsToShow (void *Surveys);
static void Svy_ShowOneSurvey (struct Svy_Surveys *Surveys,
                               bool ShowOnlyThisSvyComplete);
static void Svy_PutIconsOneSvy (void *Surveys);
static void Svy_WriteAuthor (struct Svy_Survey *Svy);
static void Svy_WriteStatus (struct Svy_Survey *Svy);
static Dat_StartEndTime_t Svy_GetParSvyOrder (void);

static void Svy_PutFormsToRemEditOneSvy (struct Svy_Surveys *Surveys,
                                         const char *Anchor);
static void Svy_PutPars (void *Surveys);

static void Svy_GetListSurveys (struct Svy_Surveys *Surveys);

static void Svy_SetAllowedAndHiddenScopes (unsigned *ScopesAllowed,
                                           unsigned *HiddenAllowed);

static void Svy_HideUnhideSurvey (HidVis_HiddenOrVisible_t HiddenOrVisible);

static void Svy_SetAllowedScopes (struct Svy_Survey *Svy);
static void Svy_ShowLstGrpsToEditSurvey (long SvyCod);
static void Svy_CreateSurvey (struct Svy_Survey *Svy,const char *Txt);
static void Svy_UpdateSurvey (struct Svy_Survey *Svy,const char *Txt);
static void Svy_CreateGrps (long SvyCod);
static void Svy_GetAndWriteNamesOfGrpsAssociatedToSvy (struct Svy_Survey *Svy);

static void Svy_ShowFormEditOneQst (struct Svy_Surveys *Surveys,
                                    struct Svy_Question *SvyQst,
                                    char Stem[Cns_MAX_BYTES_TEXT + 1]);
static void Svy_InitQst (struct Svy_Question *SvyQst);
static bool Svy_AllocateTextChoiceAnswer (struct Svy_Question *SvyQst,unsigned NumAns);
static void Svy_FreeTextChoiceAnswers (struct Svy_Question *SvyQst,unsigned NumAnswers);
static void Svy_FreeTextChoiceAnswer (struct Svy_Question *SvyQst,unsigned NumAns);

static unsigned Svy_GetNextQuestionIndexInSvy (long SvyCod);
static void Svy_ListSvyQuestions (struct Svy_Surveys *Surveys);
static void Svy_GetQstDataFromRow (MYSQL_RES *mysql_res,
                                   struct Svy_Question *SvyQst,
                                   char Stem[Cns_MAX_BYTES_TEXT + 1]);
static void Svy_PutParsToEditQuestion (void *Surveys);
static void Svy_PutIconToAddNewQuestion (void *Surveys);
static void Svy_WriteQstStem (const char *Stem);
static void Svy_WriteAnswersOfAQst (struct Svy_Survey *Svy,
                                    struct Svy_Question *SvyQst,
                                    bool PutFormAnswerSurvey);
static void Svy_WriteCommentsOfAQst (struct Svy_Survey *Svy,
                                     struct Svy_Question *SvyQst,
                                     bool PutFormAnswerSurvey);
static void Svy_DrawBarNumUsrs (unsigned NumUsrs,unsigned MaxUsrs);

static void Svy_PutIconToRemoveOneQst (void *Surveys);
static void Svy_PutParsRemoveOneQst (void *Surveys);

static void Svy_ReceiveAndStoreUserAnswersToASurvey (long SvyCod);

/*****************************************************************************/
/*************************** Reset surveys context ***************************/
/*****************************************************************************/

void Svy_ResetSurveys (struct Svy_Surveys *Surveys)
  {
   Surveys->LstIsRead     = false;	// Is the list already read from database, or it needs to be read?
   Surveys->Num           = 0;		// Number of surveys
   Surveys->LstSvyCods    = NULL;	// List of survey codes
   Surveys->SelectedOrder = Svy_ORDER_DEFAULT;
   Surveys->CurrentPage   = 0;
   Surveys->Svy.SvyCod    = -1L;
   Surveys->QstCod        = -1L;
  }

/*****************************************************************************/
/***************************** List all surveys ******************************/
/*****************************************************************************/

void Svy_SeeAllSurveys (void)
  {
   struct Svy_Surveys Surveys;

   /***** Reset surveys *****/
   Svy_ResetSurveys (&Surveys);

   /***** Get parameters *****/
   Surveys.SelectedOrder = Svy_GetParSvyOrder ();
   Grp_GetParWhichGroups ();
   Surveys.CurrentPage = Pag_GetParPagNum (Pag_SURVEYS);

   /***** Show all surveys *****/
   Svy_ListAllSurveys (&Surveys);
  }

/*****************************************************************************/
/***************************** Show all surveys ******************************/
/*****************************************************************************/

void Svy_ListAllSurveys (struct Svy_Surveys *Surveys)
  {
   extern const char *Hlp_ANALYTICS_Surveys;
   extern const char *Txt_Surveys;
   extern const char *Txt_START_END_TIME_HELP[Dat_NUM_START_END_TIME];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Survey;
   extern const char *Txt_Status;
   extern const char *Txt_No_surveys;
   Dat_StartEndTime_t Order;
   Grp_WhichGroups_t WhichGroups;
   struct Pag_Pagination Pagination;
   unsigned NumSvy;

   /***** Get number of groups in current course *****/
   if (!Gbl.Crs.Grps.NumGrps)
      Gbl.Crs.Grps.WhichGrps = Grp_ALL_GROUPS;

   /***** Get list of surveys *****/
   Svy_GetListSurveys (Surveys);

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Surveys->Num;
   Pagination.CurrentPage = (int) Surveys->CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Surveys->CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Begin box *****/
   Box_BoxBegin ("100%",Txt_Surveys,
                 Svy_PutIconsListSurveys,Surveys,
                 Hlp_ANALYTICS_Surveys,Box_NOT_CLOSABLE);

      /***** Select whether show only my groups or all groups *****/
      if (Gbl.Crs.Grps.NumGrps)
	{
	 Set_BeginSettingsHead ();
	    Grp_ShowFormToSelWhichGrps (ActSeeAllSvy,
					Svy_ParsWhichGroupsToShow,Surveys);
	 Set_EndSettingsHead ();
	}

      /***** Write links to pages *****/
      Pag_WriteLinksToPagesCentered (Pag_SURVEYS,&Pagination,
				     Surveys,-1L);

      if (Surveys->Num)
	{
	 /***** Table head *****/
	 HTM_TABLE_BeginWideMarginPadding (5);
	    HTM_TR_Begin (NULL);

	       if (Surveys->Svy.Status.ICanEdit ||
		   Rsc_CheckIfICanGetLink ())
                  HTM_TH_Span (NULL,HTM_HEAD_CENTER,1,1,"CONTEXT_COL");	// Column for contextual icons

	       for (Order  = (Dat_StartEndTime_t) 0;
		    Order <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
		    Order++)
		 {
                  HTM_TH_Begin (HTM_HEAD_LEFT);

		     /* Form to change order */
		     Frm_BeginForm (ActSeeAllSvy);
			WhichGroups = Grp_GetParWhichGroups ();
			Grp_PutParWhichGroups (&WhichGroups);
			Pag_PutParPagNum (Pag_SURVEYS,Surveys->CurrentPage);
			Par_PutParOrder ((unsigned) Order);
			HTM_BUTTON_Submit_Begin (Txt_START_END_TIME_HELP[Order],
			                         "class=\"BT_LINK\"");
			   if (Order == Surveys->SelectedOrder)
			      HTM_U_Begin ();
			   HTM_Txt (Txt_START_END_TIME[Order]);
			   if (Order == Surveys->SelectedOrder)
			      HTM_U_End ();
			HTM_BUTTON_End ();
		     Frm_EndForm ();

		  HTM_TH_End ();
		 }

	       HTM_TH (Txt_Survey,HTM_HEAD_LEFT  );
	       HTM_TH (Txt_Status,HTM_HEAD_CENTER);

	    HTM_TR_End ();

	    /***** Write all surveys *****/
	    for (NumSvy  = Pagination.FirstItemVisible;
		 NumSvy <= Pagination.LastItemVisible;
		 NumSvy++)
	      {
	       Surveys->Svy.SvyCod = Surveys->LstSvyCods[NumSvy - 1];
	       Svy_ShowOneSurvey (Surveys,false);
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();
	}
      else	// No surveys created
	 Ale_ShowAlert (Ale_INFO,Txt_No_surveys);

      /***** Write again links to pages *****/
      Pag_WriteLinksToPagesCentered (Pag_SURVEYS,&Pagination,
				     Surveys,-1L);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of surveys *****/
   Svy_FreeListSurveys (Surveys);
  }

/*****************************************************************************/
/******************* Check if I can create a new survey **********************/
/*****************************************************************************/

static bool Svy_CheckIfICanCreateSvy (void)
  {
   static const bool ICanCreateSvy[Rol_NUM_ROLES] =
     {
      [Rol_TCH	  ] = true,
      [Rol_DEG_ADM] = true,
      [Rol_CTR_ADM] = true,
      [Rol_INS_ADM] = true,
      [Rol_SYS_ADM] = true,
     };

   return ICanCreateSvy[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/***************** Put contextual icons in list of surveys *******************/
/*****************************************************************************/

static void Svy_PutIconsListSurveys (void *Surveys)
  {
   /***** Put icon to create a new survey *****/
   if (Svy_CheckIfICanCreateSvy ())
      Svy_PutIconToCreateNewSvy ((struct Svy_Surveys *) Surveys);

   /***** Put icon to get resource link *****/
   if (Rsc_CheckIfICanGetLink ())
      Ico_PutContextualIconToGetLink (ActReqLnkSvy,NULL,
				      Svy_PutPars,Surveys);

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_SURVEYS);
  }

/*****************************************************************************/
/********************** Put icon to create a new survey **********************/
/*****************************************************************************/

static void Svy_PutIconToCreateNewSvy (struct Svy_Surveys *Surveys)
  {
   Ico_PutContextualIconToAdd (ActFrmNewSvy,NULL,
                               Svy_PutParsToCreateNewSvy,Surveys);
  }

/*****************************************************************************/
/******************* Put parameters to create a new survey *******************/
/*****************************************************************************/

static void Svy_PutParsToCreateNewSvy (void *Surveys)
  {
   Grp_WhichGroups_t WhichGroups;

   if (Surveys)
     {
      Svy_PutParSvyOrder (((struct Svy_Surveys *) Surveys)->SelectedOrder);
      WhichGroups = Grp_GetParWhichGroups ();
      Grp_PutParWhichGroups (&WhichGroups);
      Pag_PutParPagNum (Pag_SURVEYS,((struct Svy_Surveys *) Surveys)->CurrentPage);
     }
  }

/*****************************************************************************/
/**************** Put params to select which groups to show ******************/
/*****************************************************************************/

static void Svy_ParsWhichGroupsToShow (void *Surveys)
  {
   if (Surveys)
     {
      Svy_PutParSvyOrder (((struct Svy_Surveys *) Surveys)->SelectedOrder);
      Pag_PutParPagNum (Pag_SURVEYS,((struct Svy_Surveys *) Surveys)->CurrentPage);
     }
  }

/*****************************************************************************/
/****************************** Show one survey ******************************/
/*****************************************************************************/

void Svy_SeeOneSurvey (void)
  {
   struct Svy_Surveys Surveys;

   /***** Reset surveys *****/
   Svy_ResetSurveys (&Surveys);

   /***** Get parameters *****/
   Surveys.SelectedOrder = Svy_GetParSvyOrder ();
   Grp_GetParWhichGroups ();
   Surveys.CurrentPage = Pag_GetParPagNum (Pag_SURVEYS);

   /***** Get survey code *****/
   Surveys.Svy.SvyCod = ParCod_GetAndCheckPar (ParCod_Svy);

   /***** Show survey *****/
   Svy_ShowOneSurvey (&Surveys,true);
  }

/*****************************************************************************/
/****************************** Show one survey ******************************/
/*****************************************************************************/

static void Svy_ShowOneSurvey (struct Svy_Surveys *Surveys,
                               bool ShowOnlyThisSvyComplete)
  {
   extern const char **Hie_TxtLevel[Hie_NUM_LEVELS];
   extern const char *Hlp_ANALYTICS_Surveys;
   extern const char *Txt_Survey;
   extern const char *Txt_View_survey;
   extern const char *Txt_Number_of_questions;
   extern const char *Txt_Number_of_users;
   extern const char *Txt_Scope;
   extern const char *Txt_Users;
   extern const char *Txt_Answer_survey;
   extern const char *Txt_View_results;
   extern const char *HidVis_DateGreenClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_DateRedClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_TitleClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_GroupClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE];
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   char *Id;
   Grp_WhichGroups_t WhichGroups;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Get data of this survey *****/
   Svy_GetSurveyDataByCod (&Surveys->Svy);
   if (Surveys->Svy.Level == Hie_UNK)
      Err_WrongHierarchyLevelExit ();

   /***** Begin box *****/
   if (ShowOnlyThisSvyComplete)
      Box_BoxBegin (NULL,Surveys->Svy.Title[0] ? Surveys->Svy.Title :
					         Txt_Survey,
                    Svy_PutIconsOneSvy,Surveys,
                    Hlp_ANALYTICS_Surveys,Box_NOT_CLOSABLE);

   /***** Set anchor string *****/
   Frm_SetAnchorStr (Surveys->Svy.SvyCod,&Anchor);

   /***** Begin table *****/
   if (ShowOnlyThisSvyComplete)
      HTM_TABLE_BeginWidePadding (2);

   /***** Write first row of data of this assignment *****/
   HTM_TR_Begin (NULL);

      /* Forms to remove/edit this assignment */
      if (!ShowOnlyThisSvyComplete)
	{
	 HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL %s\"",
	               The_GetColorRows ());
	    if (Surveys->Svy.Status.ICanEdit ||
		Rsc_CheckIfICanGetLink ())
	       Svy_PutFormsToRemEditOneSvy (Surveys,Anchor);
	 HTM_TD_End ();
	}

      /* Start date/time */
      UniqueId++;
      if (asprintf (&Id,"svy_date_start_%u",UniqueId) < 0)
	 Err_NotEnoughMemoryExit ();
      if (ShowOnlyThisSvyComplete)
	 HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s\"",
		       Id,
		       Surveys->Svy.Status.Open ? HidVis_DateGreenClass[Surveys->Svy.Status.HiddenOrVisible] :
						  HidVis_DateRedClass[Surveys->Svy.Status.HiddenOrVisible],
		       The_GetSuffix ());
      else
	 HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s %s\"",
		       Id,
		       Surveys->Svy.Status.Open ? HidVis_DateGreenClass[Surveys->Svy.Status.HiddenOrVisible] :
						  HidVis_DateRedClass[Surveys->Svy.Status.HiddenOrVisible],
		       The_GetSuffix (),
		       The_GetColorRows ());
      Dat_WriteLocalDateHMSFromUTC (Id,Surveys->Svy.TimeUTC[Dat_STR_TIME],
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				    true,true,true,0x7);
      HTM_TD_End ();
      free (Id);

      /* End date/time */
      if (asprintf (&Id,"svy_date_end_%u",UniqueId) < 0)
	 Err_NotEnoughMemoryExit ();
      if (ShowOnlyThisSvyComplete)
	 HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s\"",
		       Id,
		       Surveys->Svy.Status.Open ? HidVis_DateGreenClass[Surveys->Svy.Status.HiddenOrVisible] :
						  HidVis_DateRedClass[Surveys->Svy.Status.HiddenOrVisible],
		       The_GetSuffix ());
      else
	 HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s %s\"",
		       Id,
		       Surveys->Svy.Status.Open ? HidVis_DateGreenClass[Surveys->Svy.Status.HiddenOrVisible] :
						  HidVis_DateRedClass[Surveys->Svy.Status.HiddenOrVisible],
		       The_GetSuffix (),
		       The_GetColorRows ());
      Dat_WriteLocalDateHMSFromUTC (Id,Surveys->Svy.TimeUTC[Dat_END_TIME],
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				    true,false,true,0x7);
      HTM_TD_End ();
      free (Id);

      /* Survey title */
      if (ShowOnlyThisSvyComplete)
	 HTM_TD_Begin ("class=\"LT\"");
      else
	 HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());

      HTM_ARTICLE_Begin (Anchor);
	 Frm_BeginForm (ActSeeOneSvy);
	    ParCod_PutPar (ParCod_Svy,Surveys->Svy.SvyCod);
	    Svy_PutParSvyOrder (Surveys->SelectedOrder);
	    WhichGroups = Grp_GetParWhichGroups ();
	    Grp_PutParWhichGroups (&WhichGroups);
	    Pag_PutParPagNum (Pag_SURVEYS,Surveys->CurrentPage);
	    HTM_BUTTON_Submit_Begin (Txt_View_survey,"class=\"LT BT_LINK %s_%s\"",
				     HidVis_TitleClass[Surveys->Svy.Status.HiddenOrVisible],
				     The_GetSuffix ());
	       HTM_Txt (Surveys->Svy.Title);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
      HTM_ARTICLE_End ();

      /* Number of questions and number of distinct users who have already answered this survey */
      HTM_DIV_Begin ("class=\"%s_%s\"",
		     HidVis_GroupClass[Surveys->Svy.Status.HiddenOrVisible],
		     The_GetSuffix ());
	 HTM_TxtColonNBSP (Txt_Number_of_questions);
	 HTM_Unsigned (Surveys->Svy.NumQsts);
	 HTM_Txt ("; ");
	 HTM_TxtColonNBSP (Txt_Number_of_users);
	 HTM_Unsigned (Surveys->Svy.NumUsrs);
      HTM_DIV_End ();

      HTM_TD_End ();

      /* Status of the survey */
      if (ShowOnlyThisSvyComplete)
	 HTM_TD_Begin ("rowspan=\"2\" class=\"LT\"");
      else
	 HTM_TD_Begin ("rowspan=\"2\" class=\"LT %s\"",
	               The_GetColorRows ());
      Svy_WriteStatus (&Surveys->Svy);

      if (!ShowOnlyThisSvyComplete)
	{
	 /* Possible button to answer this survey */
	 if (Surveys->Svy.Status.ICanAnswer)
	   {
	    HTM_DIV_Begin ("class=\"BUTTONS_AFTER_ALERT\"");

	       Frm_BeginForm (ActSeeOneSvy);
		  ParCod_PutPar (ParCod_Svy,Surveys->Svy.SvyCod);
		  Svy_PutParSvyOrder (Surveys->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
		  Pag_PutParPagNum (Pag_SURVEYS,Surveys->CurrentPage);
		     Btn_PutCreateButtonInline (Txt_Answer_survey);
	       Frm_EndForm ();

	    HTM_DIV_End ();
	   }
	 /* Possible button to see the result of the survey */
	 else if (Surveys->Svy.Status.ICanViewResults)
	   {
	    HTM_DIV_Begin ("class=\"BUTTONS_AFTER_ALERT\"");

	       Frm_BeginForm (ActSeeOneSvy);
		  ParCod_PutPar (ParCod_Svy,Surveys->Svy.SvyCod);
		  Svy_PutParSvyOrder (Surveys->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
		  Pag_PutParPagNum (Pag_SURVEYS,Surveys->CurrentPage);
		  Btn_PutConfirmButtonInline (Txt_View_results);
	       Frm_EndForm ();

	    HTM_DIV_End ();
	   }
	}

      HTM_TD_End ();
   HTM_TR_End ();

   /***** Write second row of data of this survey *****/
   HTM_TR_Begin (NULL);

      /* 1st column: Author of the survey */
      if (ShowOnlyThisSvyComplete)
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
      else
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",
	               The_GetColorRows ());
      Svy_WriteAuthor (&Surveys->Svy);
      HTM_TD_End ();

      /* 2nd column: Scope, Users, Groups and Text */
      if (ShowOnlyThisSvyComplete)
	 HTM_TD_Begin ("class=\"LT\"");
      else
	 HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());

      /* Scope of the survey */
      HTM_DIV_Begin ("class=\"%s_%s\"",
		     HidVis_GroupClass[Surveys->Svy.Status.HiddenOrVisible],
		     The_GetSuffix ());
	 HTM_TxtColonNBSP (Txt_Scope);
	 HTM_TxtF ("%s&nbsp;%s",*Hie_TxtLevel[Surveys->Svy.Level],
		   Gbl.Hierarchy.Node[Surveys->Svy.Level].ShrtName);
      HTM_DIV_End ();

      /* Users' roles who can answer the survey */
      HTM_DIV_Begin ("class=\"%s_%s\"",
		     HidVis_GroupClass[Surveys->Svy.Status.HiddenOrVisible],
		     The_GetSuffix ());
	 HTM_TxtColon (Txt_Users);
	 HTM_BR ();
	 Rol_WriteSelectorRoles (1 << Rol_STD |
				 1 << Rol_NET |
				 1 << Rol_TCH,
				 Surveys->Svy.Roles,
				 true,false);
      HTM_DIV_End ();

      /* Groups whose users can answer this survey */
      if (Surveys->Svy.Level == Hie_CRS)
	 if (Gbl.Crs.Grps.NumGrps)
	    Svy_GetAndWriteNamesOfGrpsAssociatedToSvy (&Surveys->Svy);

      /* Text of the survey */
      Svy_DB_GetSurveyTxt (Surveys->Svy.SvyCod,Txt);
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			Txt,Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
      ALn_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
      HTM_DIV_Begin ("class=\"PAR %s_%s\"",
                     HidVis_DataClass[Surveys->Svy.Status.HiddenOrVisible],
		     The_GetSuffix ());
	 HTM_Txt (Txt);
      HTM_DIV_End ();
      HTM_TD_End ();

   HTM_TR_End ();

   /***** Write questions of this survey *****/
   if (ShowOnlyThisSvyComplete)
     {
      HTM_TR_Begin (NULL);
	 HTM_TD_Begin ("colspan=\"5\"");
	    Svy_ListSvyQuestions (Surveys);
	 HTM_TD_End ();
      HTM_TR_End ();
     }

   The_ChangeRowColor ();

   /***** Mark possible notification as seen *****/
   if (Surveys->Svy.Level == Hie_CRS)	// Only course surveys are notified
      Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_SURVEY,Surveys->Svy.HieCod);

   if (ShowOnlyThisSvyComplete)
     {
	 /***** End table *****/
	 HTM_TABLE_End ();

      /***** End box *****/
      Box_BoxEnd ();
     }

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (&Anchor);
  }

/*****************************************************************************/
/******************** Put contextual icons in a survey ***********************/
/*****************************************************************************/

static void Svy_PutIconsOneSvy (void *Surveys)
  {
   char *Anchor = NULL;

   if (Surveys)
     {
      /***** Set anchor string *****/
      Frm_SetAnchorStr (((struct Svy_Surveys *) Surveys)->Svy.SvyCod,&Anchor);

      /***** Icons to remove/edit this survey *****/
      Svy_PutFormsToRemEditOneSvy ((struct Svy_Surveys *) Surveys,Anchor);

      /***** Free anchor string *****/
      Frm_FreeAnchorStr (&Anchor);
     }
  }

/*****************************************************************************/
/*********************** Write the author of a survey ************************/
/*****************************************************************************/

static void Svy_WriteAuthor (struct Svy_Survey *Svy)
  {
   Usr_WriteAuthor1Line (Svy->UsrCod,Svy->Status.HiddenOrVisible);
  }

/*****************************************************************************/
/************************ Write status of a survey ***************************/
/*****************************************************************************/

static void Svy_WriteStatus (struct Svy_Survey *Svy)
  {
   extern const char *Txt_Hidden_survey;
   extern const char *Txt_Visible_survey;
   extern const char *Txt_Closed_survey;
   extern const char *Txt_Open_survey;
   extern const char *Txt_SURVEY_Type_of_user_not_allowed;
   extern const char *Txt_SURVEY_Type_of_user_allowed;
   extern const char *Txt_SURVEY_You_belong_to_the_scope_of_the_survey;
   extern const char *Txt_SURVEY_You_dont_belong_to_the_scope_of_the_survey;
   extern const char *Txt_SURVEY_You_have_already_answered;
   extern const char *Txt_SURVEY_You_have_not_answered;
   extern const char *HidVis_DateGreenClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_DateRedClass[HidVis_NUM_HIDDEN_VISIBLE];
   static struct
     {
      const char **Class;
      const char **Txt;
     } HiddenOrVisible[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] =
        {
         .Class = &HidVis_DateRedClass  [HidVis_HIDDEN ],
         .Txt   = &Txt_Hidden_survey
        },
      [HidVis_VISIBLE] =
	{
	 .Class = &HidVis_DateGreenClass[HidVis_VISIBLE],
	 .Txt   = &Txt_Visible_survey
	}
     };

   /***** Begin list with items of status *****/
   HTM_UL_Begin (NULL);

      /* Write whether survey is visible or hidden */
      HTM_LI_Begin ("class=\"%s_%s\"",
		    *HiddenOrVisible[Svy->Status.HiddenOrVisible].Class,
		    The_GetSuffix ());
         HTM_Txt (*HiddenOrVisible[Svy->Status.HiddenOrVisible].Txt);
      HTM_LI_End ();

      /* Write whether survey is open or closed */
      if (Svy->Status.Open)
	{
	 HTM_LI_Begin ("class=\"%s_%s\"",
		       HidVis_DateGreenClass[Svy->Status.HiddenOrVisible],
		       The_GetSuffix ());
	    HTM_Txt (Txt_Open_survey);
	}
      else
	{
	 HTM_LI_Begin ("class=\"%s_%s\"",
		       HidVis_DateRedClass[Svy->Status.HiddenOrVisible],
		       The_GetSuffix ());
	    HTM_Txt (Txt_Closed_survey);
	}
      HTM_LI_End ();

      /* Write whether survey can be answered by me or not depending on user type */
      if (Svy->Status.IAmLoggedWithAValidRoleToAnswer)
	{
	 HTM_LI_Begin ("class=\"%s_%s\"",
		       HidVis_DateGreenClass[Svy->Status.HiddenOrVisible],
		       The_GetSuffix ());
	    HTM_Txt (Txt_SURVEY_Type_of_user_allowed);
	}
      else
	{
	 HTM_LI_Begin ("class=\"%s_%s\"",
		       HidVis_DateRedClass[Svy->Status.HiddenOrVisible],
		       The_GetSuffix ());
	    HTM_Txt (Txt_SURVEY_Type_of_user_not_allowed);
	}
      HTM_LI_End ();

      /* Write whether survey can be answered by me or not depending on groups */
      if (Svy->Status.IBelongToScope)
	{
	 HTM_LI_Begin ("class=\"%s_%s\"",
		       HidVis_DateGreenClass[Svy->Status.HiddenOrVisible],
		       The_GetSuffix ());
	    HTM_Txt (Txt_SURVEY_You_belong_to_the_scope_of_the_survey);
	}
      else
	{
	 HTM_LI_Begin ("class=\"%s_%s\"",
		       HidVis_DateRedClass[Svy->Status.HiddenOrVisible],
		       The_GetSuffix ());
	    HTM_Txt (Txt_SURVEY_You_dont_belong_to_the_scope_of_the_survey);
	}
      HTM_LI_End ();

      /* Write whether survey has been already answered by me or not */
      if (Svy->Status.IHaveAnswered)
	{
	 HTM_LI_Begin ("class=\"%s_%s\"",
		       HidVis_DateGreenClass[Svy->Status.HiddenOrVisible],
		       The_GetSuffix ());
	    HTM_Txt (Txt_SURVEY_You_have_already_answered);
	}
      else
	{
	 HTM_LI_Begin ("class=\"%s_%s\"",
		       HidVis_DateRedClass[Svy->Status.HiddenOrVisible],
		       The_GetSuffix ());
	    HTM_Txt (Txt_SURVEY_You_have_not_answered);
	}
      HTM_LI_End ();

   /***** End list with items of status *****/
   HTM_UL_End ();
  }

/*****************************************************************************/
/********* Get parameter with the type or order in list of surveys ***********/
/*****************************************************************************/

static Dat_StartEndTime_t Svy_GetParSvyOrder (void)
  {
   return (Dat_StartEndTime_t)
	  Par_GetParUnsignedLong ("Order",
				  0,
				  Dat_NUM_START_END_TIME - 1,
				  (unsigned long) Svy_ORDER_DEFAULT);
  }

/*****************************************************************************/
/***** Put a hidden parameter with the type of order in list of surveys ******/
/*****************************************************************************/

void Svy_PutParSvyOrder (Dat_StartEndTime_t SelectedOrder)
  {
   Par_PutParOrder ((unsigned) SelectedOrder);
  }

/*****************************************************************************/
/*********************** Put icons to edit one survey ************************/
/*****************************************************************************/

static void Svy_PutFormsToRemEditOneSvy (struct Svy_Surveys *Surveys,
                                         const char *Anchor)
  {
   static const Act_Action_t ActionHideUnhide[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] = ActUnhSvy,	// Hidden ==> action to unhide
      [HidVis_VISIBLE] = ActHidSvy,	// Visible ==> action to hide
     };

   if (Surveys->Svy.Status.ICanEdit)
     {
      /***** Icon to remove survey *****/
      Ico_PutContextualIconToRemove (ActReqRemSvy,NULL,
				     Svy_PutPars,Surveys);

      /***** Icon to reset survey *****/
      Ico_PutContextualIconToReset (ActReqRstSvy,NULL,
				    Svy_PutPars,Surveys);

      /***** Icon to hide/unhide survey *****/
      Ico_PutContextualIconToHideUnhide (ActionHideUnhide,Anchor,
					 Svy_PutPars,Surveys,
					 Surveys->Svy.Status.HiddenOrVisible);

      /***** Icon to edit survey *****/
      Ico_PutContextualIconToEdit (ActEdiOneSvy,NULL,
				   Svy_PutPars,Surveys);
     }

   /***** Icon to get resource link *****/
   if (Rsc_CheckIfICanGetLink ())
      Ico_PutContextualIconToGetLink (ActReqLnkSvy,NULL,
				      Svy_PutPars,Surveys);
  }

/*****************************************************************************/
/********************** Params used to edit a survey *************************/
/*****************************************************************************/

static void Svy_PutPars (void *Surveys)
  {
   Grp_WhichGroups_t WhichGroups;

   if (Surveys)
     {
      ParCod_PutPar (ParCod_Svy,((struct Svy_Surveys *) Surveys)->Svy.SvyCod);
      Par_PutParOrder ((unsigned) ((struct Svy_Surveys *) Surveys)->SelectedOrder);
      WhichGroups = Grp_GetParWhichGroups ();
      Grp_PutParWhichGroups (&WhichGroups);
      Pag_PutParPagNum (Pag_SURVEYS,((struct Svy_Surveys *) Surveys)->CurrentPage);
     }
  }

/*****************************************************************************/
/************************** Get list of all surveys **************************/
/*****************************************************************************/

static void Svy_GetListSurveys (struct Svy_Surveys *Surveys)
  {
   MYSQL_RES *mysql_res;
   unsigned NumSvy;
   unsigned ScopesAllowed = 0;
   unsigned HiddenAllowed = 0;

   /***** Free list of surveys *****/
   if (Surveys->LstIsRead)
      Svy_FreeListSurveys (Surveys);

   /***** Set allowed and hidden scopes to get list depending on my user's role *****/
   Svy_SetAllowedAndHiddenScopes (&ScopesAllowed,&HiddenAllowed);

   /***** Get list of surveys from database *****/
   if ((Surveys->Num = Svy_DB_GetListSurveys (&mysql_res,
                                              ScopesAllowed,
                                              HiddenAllowed,
                                              Surveys->SelectedOrder))) // Surveys found...
     {
      /***** Create list of surveys *****/
      if ((Surveys->LstSvyCods = calloc ((size_t) Surveys->Num,
                                         sizeof (*Surveys->LstSvyCods))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the surveys codes *****/
      for (NumSvy = 0;
	   NumSvy < Surveys->Num;
	   NumSvy++)
         /* Get next survey code */
         if ((Surveys->LstSvyCods[NumSvy] = DB_GetNextCode (mysql_res)) < 0)
            Err_WrongSurveyExit ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Surveys->LstIsRead = true;
  }

/*****************************************************************************/
/*** Set allowed and hidden scopes to get list depending on my user's role ***/
/*****************************************************************************/

static void Svy_SetAllowedAndHiddenScopes (unsigned *ScopesAllowed,
                                           unsigned *HiddenAllowed)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_UNK:	// User not logged in *********************************
	 *ScopesAllowed = 0;
	 *HiddenAllowed = 0;
         break;
      case Rol_GST:	// User not belonging to any course *******************
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 0;
	 break;
      case Rol_USR:	// Student or teacher in other courses...
   	   	   	// ...but not belonging to the current course *********
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 0;
	 if (Hie_CheckIfIBelongTo (Hie_CTY,Gbl.Hierarchy.Node[Hie_CTY].HieCod))
	   {
	    *ScopesAllowed |= 1 << Hie_CTY;
	    if (Hie_CheckIfIBelongTo (Hie_INS,Gbl.Hierarchy.Node[Hie_INS].HieCod))
	      {
	       *ScopesAllowed |= 1 << Hie_INS;
	       if (Hie_CheckIfIBelongTo (Hie_CTR,Gbl.Hierarchy.Node[Hie_CTR].HieCod))
		 {
		  *ScopesAllowed |= 1 << Hie_CTR;
		  if (Hie_CheckIfIBelongTo (Hie_DEG,Gbl.Hierarchy.Node[Hie_DEG].HieCod))
		     *ScopesAllowed |= 1 << Hie_DEG;
		 }
	      }
	   }
         break;
      case Rol_STD:	// Student in current course **************************
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 0;
	 if (Hie_CheckIfIBelongTo (Hie_CTY,Gbl.Hierarchy.Node[Hie_CTY].HieCod))
	   {
	    *ScopesAllowed |= 1 << Hie_CTY;
	    if (Hie_CheckIfIBelongTo (Hie_INS,Gbl.Hierarchy.Node[Hie_INS].HieCod))
	      {
	       *ScopesAllowed |= 1 << Hie_INS;
	       if (Hie_CheckIfIBelongTo (Hie_CTR,Gbl.Hierarchy.Node[Hie_CTR].HieCod))
		 {
		  *ScopesAllowed |= 1 << Hie_CTR;
		  if (Hie_CheckIfIBelongTo (Hie_DEG,Gbl.Hierarchy.Node[Hie_DEG].HieCod))
		    {
		     *ScopesAllowed |= 1 << Hie_DEG;
		     if (Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS])
			*ScopesAllowed |= 1 << Hie_CRS;
		    }
		 }
	      }
	   }
         break;
      case Rol_NET:	// Non-editing teacher in current course **************
      case Rol_TCH:	// Teacher in current course **************************
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 0;
	 if (Hie_CheckIfIBelongTo (Hie_CTY,Gbl.Hierarchy.Node[Hie_CTY].HieCod))
	   {
	    *ScopesAllowed |= 1 << Hie_CTY;
	    if (Hie_CheckIfIBelongTo (Hie_INS,Gbl.Hierarchy.Node[Hie_INS].HieCod))
	      {
	       *ScopesAllowed |= 1 << Hie_INS;
	       if (Hie_CheckIfIBelongTo (Hie_CTR,Gbl.Hierarchy.Node[Hie_CTR].HieCod))
		 {
		  *ScopesAllowed |= 1 << Hie_CTR;
		  if (Hie_CheckIfIBelongTo (Hie_DEG,Gbl.Hierarchy.Node[Hie_DEG].HieCod))
		    {
		     *ScopesAllowed |= 1 << Hie_DEG;
		     if (Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS])
		       {
			*ScopesAllowed |= 1 << Hie_CRS;
			*HiddenAllowed |= 1 << Hie_CRS;	// A non-editing teacher or teacher can view hidden course surveys
		       }
		    }
		 }
	      }
	   }
         break;
      case Rol_DEG_ADM:	// Degree administrator *******************************
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 0;
	 if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)		// Country selected
	   {
	    *ScopesAllowed |= 1 << Hie_CTY;
	    if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)		// Institution selected
	      {
	       *ScopesAllowed |= 1 << Hie_INS;
	       if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)	// Center selected
		 {
		  *ScopesAllowed |= 1 << Hie_CTR;
		  if (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0)	// Degree selected
		    {
		     *ScopesAllowed |= 1 << Hie_DEG;
		     *HiddenAllowed |= 1 << Hie_DEG;	// A degree admin can view hidden degree surveys
		    }
	         }
	      }
	   }
	 break;
      case Rol_CTR_ADM:	// Center administrator *******************************
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 0;
	 if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)		// Country selected
	   {
	    *ScopesAllowed |= 1 << Hie_CTY;
	    if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)		// Institution selected
	      {
	       *ScopesAllowed |= 1 << Hie_INS;
	       if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)	// Center selected
		 {
		  *ScopesAllowed |= 1 << Hie_CTR;
		  *HiddenAllowed |= 1 << Hie_CTR;	// A center admin can view hidden center surveys
		 }
	      }
	   }
	 break;
      case Rol_INS_ADM:	// Institution administrator **************************
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 0;
	 if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)		// Country selected
	   {
	    *ScopesAllowed |= 1 << Hie_CTY;
	    if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)		// Institution selected
	      {
	       *ScopesAllowed |= 1 << Hie_INS;
	       *HiddenAllowed |= 1 << Hie_INS;		// An institution admin can view hidden institution surveys
	      }
	   }
	 break;
      case Rol_SYS_ADM:	// System administrator (superuser) *******************
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 1 << Hie_SYS;			// A system admin can view hidden system surveys
	 if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)		// Country selected
	   {
	    *ScopesAllowed |= 1 << Hie_CTY;
	    *HiddenAllowed |= 1 << Hie_CTY;		// A system admin can view hidden country surveys
	    if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)		// Institution selected
	      {
	       *ScopesAllowed |= 1 << Hie_INS;
	       *HiddenAllowed |= 1 << Hie_INS;		// A system admin can view hidden institution surveys
	       if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)	// Center selected
		 {
		  *ScopesAllowed |= 1 << Hie_CTR;
	          *HiddenAllowed |= 1 << Hie_CTR;	// A system admin can view hidden center surveys
		  if (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0)	// Degree selected
		    {
		     *ScopesAllowed |= 1 << Hie_DEG;
	             *HiddenAllowed |= 1 << Hie_DEG;	// A system admin can view hidden degree surveys
		     if (Gbl.Hierarchy.Level == Hie_CRS)	// Course selected
		       {
			*ScopesAllowed |= 1 << Hie_CRS;
	                *HiddenAllowed |= 1 << Hie_CRS;	// A system admin can view hidden course surveys
		       }
		    }
		 }
	      }
	   }
	 break;
     }
  }

/*****************************************************************************/
/********************* Get survey data using its code ************************/
/*****************************************************************************/

void Svy_GetSurveyDataByCod (struct Svy_Survey *Svy)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get data of survey from database *****/
   if (Svy_DB_GetSurveyDataByCod (&mysql_res,Svy->SvyCod)) // Survey found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get code of the survey (row[0]) */
      Svy->SvyCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get survey scope (row[1]) */
      if ((Svy->Level = Hie_GetLevelFromDBStr (row[1])) == Hie_UNK)
         Err_WrongHierarchyLevelExit ();

      /* Get code of the country, institution, center, degree or course (row[2]) */
      Svy->HieCod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get whether the survey is hidden (row[3]) */
      Svy->Status.HiddenOrVisible = HidVid_GetHiddenOrVisible (row[3][0]);

      /* Get roles (row[4]) */
      if (sscanf (row[4],"%u",&Svy->Roles) != 1)
      	 Err_ShowErrorAndExit ("Error when reading roles of survey.");

      /* Get author of the survey (row[5]) */
      Svy->UsrCod = Str_ConvertStrCodToLongCod (row[5]);

      /* Get start date (row[6] holds the start UTC time)
         and   end date (row[7] holds the end   UTC time) */
      Svy->TimeUTC[Dat_STR_TIME] = Dat_GetUNIXTimeFromStr (row[6]);
      Svy->TimeUTC[Dat_END_TIME] = Dat_GetUNIXTimeFromStr (row[7]);

      /* Get whether the survey is open or closed (row(8)) */
      Svy->Status.Open = (row[8][0] == '1');

      /* Get the title of the survey (row[9]) */
      Str_Copy (Svy->Title,row[9],sizeof (Svy->Title) - 1);

      /* Get number of questions and number of users who have already answer this survey */
      Svy->NumQsts = Svy_DB_GetNumQstsSvy (Svy->SvyCod);
      Svy->NumUsrs = Svy_DB_GetNumUsrsWhoHaveAnsweredSvy (Svy->SvyCod);

      /* Am I logged with a valid role to answer this survey? */
      Svy->Status.IAmLoggedWithAValidRoleToAnswer = (Svy->Roles & (1 << Gbl.Usrs.Me.Role.Logged));

      /* Do I belong to valid groups to answer this survey? */
      switch (Svy->Level)
        {
	 case Hie_SYS:	// System
            Svy->Status.IBelongToScope = Gbl.Usrs.Me.Logged;
	    break;
	 case Hie_CTY:	// Country
	 case Hie_INS:	// Institution
	 case Hie_CTR:	// Center
	 case Hie_DEG:	// Degree
            Svy->Status.IBelongToScope = Hie_CheckIfIBelongTo (Svy->Level,Svy->HieCod);
	    break;
	 case Hie_CRS:	// Course
	    Svy->Status.IBelongToScope = Hie_CheckIfIBelongTo (Svy->Level,Svy->HieCod) &&
					 Svy_DB_CheckIfICanDoThisSurveyBasedOnGrps (Svy->SvyCod);
	    break;
	 default:	// Unknown
            Err_WrongHierarchyLevelExit ();
	    break;
        }

      /* Have I answered this survey? */
      Svy->Status.IHaveAnswered = Svy_DB_CheckIfIHaveAnsweredSvy (Svy->SvyCod);

      /* Can I answer survey? */
      Svy->Status.ICanAnswer = ( Svy->NumQsts != 0) &&
                                 Svy->Status.HiddenOrVisible == HidVis_VISIBLE &&
                                 Svy->Status.Open &&
                                 Svy->Status.IAmLoggedWithAValidRoleToAnswer &&
                                 Svy->Status.IBelongToScope &&
                                !Svy->Status.IHaveAnswered;

      /* Can I view results of the survey?
         Can I edit survey? */
      switch (Gbl.Usrs.Me.Role.Logged)
        {
         case Rol_STD:
            Svy->Status.ICanViewResults  = (Svy->Level == Hie_CRS ||
        	                            Svy->Level == Hie_DEG ||
        	                            Svy->Level == Hie_CTR ||
        	                            Svy->Level == Hie_INS ||
        	                            Svy->Level == Hie_CTY ||
        	                            Svy->Level == Hie_SYS) &&
        	                           (Svy->NumQsts != 0) &&
                                            Svy->Status.HiddenOrVisible == HidVis_VISIBLE &&
                                            Svy->Status.Open &&
                                            Svy->Status.IAmLoggedWithAValidRoleToAnswer &&
                                            Svy->Status.IBelongToScope &&
                                            Svy->Status.IHaveAnswered;
            Svy->Status.ICanViewComments = false;
            Svy->Status.ICanEdit         = false;
            break;
         case Rol_NET:
            Svy->Status.ICanViewResults  =
            Svy->Status.ICanViewComments = (Svy->Level == Hie_CRS ||
        	                            Svy->Level == Hie_DEG ||
        	                            Svy->Level == Hie_CTR ||
        	                            Svy->Level == Hie_INS ||
        	                            Svy->Level == Hie_CTY ||
        	                            Svy->Level == Hie_SYS) &&
        	                            Svy->NumQsts != 0 &&
                                           !Svy->Status.ICanAnswer;
            Svy->Status.ICanEdit        = false;
            break;
         case Rol_TCH:
            Svy->Status.ICanViewResults  =
            Svy->Status.ICanViewComments = (Svy->Level == Hie_CRS ||
        	                           Svy->Level == Hie_DEG ||
        	                           Svy->Level == Hie_CTR ||
        	                           Svy->Level == Hie_INS ||
        	                           Svy->Level == Hie_CTY ||
        	                           Svy->Level == Hie_SYS) &&
        	                           Svy->NumQsts != 0 &&
                                          !Svy->Status.ICanAnswer;
            Svy->Status.ICanEdit        =  Svy->Level == Hie_CRS; // && Svy->Status.IBelongToScope
            break;
         case Rol_DEG_ADM:
            Svy->Status.ICanViewResults  =
            Svy->Status.ICanViewComments = (Svy->Level == Hie_DEG ||
        	                            Svy->Level == Hie_CTR ||
        	                            Svy->Level == Hie_INS ||
        	                            Svy->Level == Hie_CTY ||
        	                            Svy->Level == Hie_SYS) &&
        	                           (Svy->NumQsts != 0) &&
                                           !Svy->Status.ICanAnswer;
            Svy->Status.ICanEdit         =  Svy->Level == Hie_DEG &&
                                            Svy->Status.IBelongToScope;
            break;
         case Rol_CTR_ADM:
            Svy->Status.ICanViewResults  =
            Svy->Status.ICanViewComments = (Svy->Level == Hie_CTR ||
        	                            Svy->Level == Hie_INS ||
        	                            Svy->Level == Hie_CTY ||
        	                            Svy->Level == Hie_SYS) &&
        	                           (Svy->NumQsts != 0) &&
                                           !Svy->Status.ICanAnswer;
            Svy->Status.ICanEdit         =  Svy->Level == Hie_CTR &&
                                            Svy->Status.IBelongToScope;
            break;
         case Rol_INS_ADM:
            Svy->Status.ICanViewResults  =
            Svy->Status.ICanViewComments = (Svy->Level == Hie_INS ||
        	                            Svy->Level == Hie_CTY ||
        	                            Svy->Level == Hie_SYS) &&
        	                           (Svy->NumQsts != 0) &&
                                           !Svy->Status.ICanAnswer;
            Svy->Status.ICanEdit         =  Svy->Level == Hie_INS &&
                                            Svy->Status.IBelongToScope;
            break;
         case Rol_SYS_ADM:
            Svy->Status.ICanViewResults  =
            Svy->Status.ICanViewComments = (Svy->NumQsts != 0);
            Svy->Status.ICanEdit         = true;
            break;
         default:
            Svy->Status.ICanViewResults  = false;
            Svy->Status.ICanViewComments = false;
            Svy->Status.ICanEdit         = false;
            break;
        }
     }
   else
     {
      /* Initialize to empty survey */
      Svy->SvyCod                = -1L;
      Svy->Level                 = Hie_UNK;
      Svy->Roles                 = 0;
      Svy->UsrCod                = -1L;
      Svy->TimeUTC[Dat_STR_TIME] =
      Svy->TimeUTC[Dat_END_TIME] = (time_t) 0;
      Svy->Title[0]              = '\0';
      Svy->NumQsts               = 0;
      Svy->NumUsrs               = 0;
      Svy->Status.HiddenOrVisible                 = HidVis_VISIBLE;
      Svy->Status.Open                            = false;
      Svy->Status.IAmLoggedWithAValidRoleToAnswer = false;
      Svy->Status.IBelongToScope                  = false;
      Svy->Status.IHaveAnswered                   = false;
      Svy->Status.ICanAnswer                      = false;
      Svy->Status.ICanViewResults                 = false;
      Svy->Status.ICanViewComments                = false;
      Svy->Status.ICanEdit                        = false;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************************** Free list of surveys ***************************/
/*****************************************************************************/

void Svy_FreeListSurveys (struct Svy_Surveys *Surveys)
  {
   if (Surveys->LstIsRead && Surveys->LstSvyCods)
     {
      /***** Free memory used by the list of surveys *****/
      free (Surveys->LstSvyCods);
      Surveys->LstSvyCods = NULL;
      Surveys->Num = 0;
      Surveys->LstIsRead = false;
     }
  }

/*****************************************************************************/
/******************** Get summary and content of a survey  *******************/
/*****************************************************************************/

void Svy_GetNotifSurvey (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                         char **ContentStr,
                         long SvyCod,bool GetContent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   size_t Length;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Get title and text *****/
   if (Svy_DB_GetSurveyTitleAndText (&mysql_res,SvyCod))
     {
      /***** Get row *****/
      row = mysql_fetch_row (mysql_res);

      /***** Get summary *****/
      Str_Copy (SummaryStr,row[0],Ntf_MAX_BYTES_SUMMARY);

      /***** Get content *****/
      if (GetContent)
	{
	 Length = strlen (row[1]);
	 if ((*ContentStr = malloc (Length + 1)) == NULL)
            Err_NotEnoughMemoryExit ();
	 Str_Copy (*ContentStr,row[1],Length);
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************** Ask for confirmation of removing of a survey ****************/
/*****************************************************************************/

void Svy_AskRemSurvey (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_survey_X;
   struct Svy_Surveys Surveys;

   /***** Reset surveys *****/
   Svy_ResetSurveys (&Surveys);

   /***** Get parameters *****/
   Surveys.SelectedOrder = Svy_GetParSvyOrder ();
   Grp_GetParWhichGroups ();
   Surveys.CurrentPage = Pag_GetParPagNum (Pag_SURVEYS);

   /***** Get survey code *****/
   Surveys.Svy.SvyCod = ParCod_GetAndCheckPar (ParCod_Svy);

   /***** Get data of the survey from database *****/
   Svy_GetSurveyDataByCod (&Surveys.Svy);
   if (!Surveys.Svy.Status.ICanEdit)
      Err_NoPermissionExit ();

   /***** Show question and button to remove survey *****/
   Ale_ShowAlertRemove (ActRemSvy,NULL,
                        Svy_PutPars,&Surveys,
			Txt_Do_you_really_want_to_remove_the_survey_X,
			Surveys.Svy.Title);

   /***** Show surveys again *****/
   Svy_ListAllSurveys (&Surveys);
  }

/*****************************************************************************/
/****************************** Remove a survey ******************************/
/*****************************************************************************/

void Svy_RemoveSurvey (void)
  {
   extern const char *Txt_Survey_X_removed;
   struct Svy_Surveys Surveys;

   /***** Reset surveys *****/
   Svy_ResetSurveys (&Surveys);

   /***** Get parameters *****/
   Surveys.SelectedOrder = Svy_GetParSvyOrder ();
   Grp_GetParWhichGroups ();
   Surveys.CurrentPage = Pag_GetParPagNum (Pag_SURVEYS);

   /***** Get survey code *****/
   Surveys.Svy.SvyCod = ParCod_GetAndCheckPar (ParCod_Svy);

   /***** Get data of the survey from database *****/
   Svy_GetSurveyDataByCod (&Surveys.Svy);
   if (!Surveys.Svy.Status.ICanEdit)
      Err_NoPermissionExit ();

   /***** Remove all users in this survey *****/
   Svy_DB_RemoveUsrsWhoHaveAnsweredSvy (Surveys.Svy.SvyCod);

   /***** Remove all answers and comments in this survey *****/
   Svy_DB_RemoveAnswersSvy (Surveys.Svy.SvyCod);
   Svy_DB_RemoveCommentsSvy (Surveys.Svy.SvyCod);

   /***** Remove all questions in this survey *****/
   Svy_DB_RemoveQstsSvy (Surveys.Svy.SvyCod);

   /***** Remove all groups of this survey *****/
   Svy_DB_RemoveGrpsAssociatedToSurvey (Surveys.Svy.SvyCod);

   /***** Remove survey *****/
   Svy_DB_RemoveSvy (Surveys.Svy.SvyCod);

   /***** Mark possible notifications as removed *****/
   Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_SURVEY,Surveys.Svy.SvyCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Survey_X_removed,
                  Surveys.Svy.Title);

   /***** Show surveys again *****/
   Svy_ListAllSurveys (&Surveys);
  }

/*****************************************************************************/
/***************** Ask for confirmation of reset of a survey *****************/
/*****************************************************************************/

void Svy_AskResetSurvey (void)
  {
   extern const char *Txt_Do_you_really_want_to_reset_the_survey_X;
   extern const char *Txt_Reset_survey;
   struct Svy_Surveys Surveys;

   /***** Reset surveys *****/
   Svy_ResetSurveys (&Surveys);

   /***** Get parameters *****/
   Surveys.SelectedOrder = Svy_GetParSvyOrder ();
   Grp_GetParWhichGroups ();
   Surveys.CurrentPage = Pag_GetParPagNum (Pag_SURVEYS);

   /***** Get survey code *****/
   Surveys.Svy.SvyCod = ParCod_GetAndCheckPar (ParCod_Svy);

   /***** Get data of the survey from database *****/
   Svy_GetSurveyDataByCod (&Surveys.Svy);
   if (!Surveys.Svy.Status.ICanEdit)
      Err_NoPermissionExit ();

   /***** Show question and button to reset survey *****/
   Ale_ShowAlertAndButton (ActRstSvy,NULL,NULL,
                           Svy_PutPars,&Surveys,
			   Btn_REMOVE_BUTTON,Txt_Reset_survey,
			   Ale_QUESTION,Txt_Do_you_really_want_to_reset_the_survey_X,
			   Surveys.Svy.Title);

   /***** Show surveys again *****/
   Svy_ListAllSurveys (&Surveys);
  }

/*****************************************************************************/
/******************************* Reset a survey ******************************/
/*****************************************************************************/

void Svy_ResetSurvey (void)
  {
   extern const char *Txt_Survey_X_reset;
   struct Svy_Surveys Surveys;

   /***** Reset surveys *****/
   Svy_ResetSurveys (&Surveys);

   /***** Get parameters *****/
   Surveys.SelectedOrder = Svy_GetParSvyOrder ();
   Grp_GetParWhichGroups ();
   Surveys.CurrentPage = Pag_GetParPagNum (Pag_SURVEYS);

   /***** Get survey code *****/
   Surveys.Svy.SvyCod = ParCod_GetAndCheckPar (ParCod_Svy);

   /***** Get data of the survey from database *****/
   Svy_GetSurveyDataByCod (&Surveys.Svy);
   if (!Surveys.Svy.Status.ICanEdit)
      Err_NoPermissionExit ();

   /***** Remove all users in this survey *****/
   Svy_DB_RemoveUsrsWhoHaveAnsweredSvy (Surveys.Svy.SvyCod);

   /***** Reset all answers in this survey *****/
   Svy_DB_ResetAnswersSvy (Surveys.Svy.SvyCod);

   /***** Remove all comments in this survey *****/
   Svy_DB_RemoveCommentsSvy (Surveys.Svy.SvyCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Survey_X_reset,Surveys.Svy.Title);

   /***** Show surveys again *****/
   Svy_ListAllSurveys (&Surveys);
  }

/*****************************************************************************/
/*************************** Hide/unhide a survey ****************************/
/*****************************************************************************/

void Svy_HideSurvey (void)
  {
   Svy_HideUnhideSurvey (HidVis_HIDDEN);
  }

void Svy_UnhideSurvey (void)
  {
   Svy_HideUnhideSurvey (HidVis_VISIBLE);
  }

static void Svy_HideUnhideSurvey (HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   struct Svy_Surveys Surveys;

   /***** Reset surveys *****/
   Svy_ResetSurveys (&Surveys);

   /***** Get parameters *****/
   Surveys.SelectedOrder = Svy_GetParSvyOrder ();
   Grp_GetParWhichGroups ();
   Surveys.CurrentPage = Pag_GetParPagNum (Pag_SURVEYS);

   /***** Get survey code *****/
   Surveys.Svy.SvyCod = ParCod_GetAndCheckPar (ParCod_Svy);

   /***** Get data of the survey from database *****/
   Svy_GetSurveyDataByCod (&Surveys.Svy);
   if (!Surveys.Svy.Status.ICanEdit)
      Err_NoPermissionExit ();

   /***** Show survey *****/
   Svy_DB_HideOrUnhideSurvey (Surveys.Svy.SvyCod,HiddenOrVisible);

   /***** Show surveys again *****/
   Svy_ListAllSurveys (&Surveys);
  }

/*****************************************************************************/
/********************* Put a form to create a new survey *********************/
/*****************************************************************************/

void Svy_ReqCreatOrEditSvy (void)
  {
   extern const char *Hlp_ANALYTICS_Surveys_edit_survey;
   extern const char *Txt_Scope;
   extern const char *Txt_Survey;
   extern const char *Txt_Title;
   extern const char *Txt_Description;
   extern const char *Txt_Users;
   extern const char *Txt_Create;
   extern const char *Txt_Save_changes;
   struct Svy_Surveys Surveys;
   bool ItsANewSurvey;
   char Txt[Cns_MAX_BYTES_TEXT + 1];
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_TO_000000,
      [Dat_END_TIME] = Dat_HMS_TO_235959
     };

   /***** Reset surveys *****/
   Svy_ResetSurveys (&Surveys);

   /***** Get parameters *****/
   Surveys.SelectedOrder = Svy_GetParSvyOrder ();
   Grp_GetParWhichGroups ();
   Surveys.CurrentPage = Pag_GetParPagNum (Pag_SURVEYS);

   /***** Get the code of the survey *****/
   ItsANewSurvey = ((Surveys.Svy.SvyCod = ParCod_GetPar (ParCod_Svy)) <= 0);

   /***** Get from the database the data of the survey *****/
   if (ItsANewSurvey)
     {
      /***** Put link (form) to create new survey *****/
      if (!Svy_CheckIfICanCreateSvy ())
         Err_NoPermissionExit ();

      /* Initialize to empty survey */
      Surveys.Svy.SvyCod = -1L;
      Surveys.Svy.Level  = Hie_UNK;
      Surveys.Svy.Roles  = (1 << Rol_STD);
      Surveys.Svy.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
      Surveys.Svy.TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();
      Surveys.Svy.TimeUTC[Dat_END_TIME] = Surveys.Svy.TimeUTC[Dat_STR_TIME] + (24 * 60 * 60);	// +24 hours
      Surveys.Svy.Title[0] = '\0';
      Surveys.Svy.NumQsts = 0;
      Surveys.Svy.NumUsrs = 0;
      Surveys.Svy.Status.HiddenOrVisible  = HidVis_VISIBLE;
      Surveys.Svy.Status.Open		  = true;
      Surveys.Svy.Status.IAmLoggedWithAValidRoleToAnswer = false;
      Surveys.Svy.Status.IBelongToScope   = false;
      Surveys.Svy.Status.IHaveAnswered    = false;
      Surveys.Svy.Status.ICanAnswer	  = false;
      Surveys.Svy.Status.ICanViewResults  = false;
      Surveys.Svy.Status.ICanViewComments = false;
     }
   else
     {
      /* Get data of the survey from database */
      Svy_GetSurveyDataByCod (&Surveys.Svy);
      if (!Surveys.Svy.Status.ICanEdit)
         Err_NoPermissionExit ();

      /* Get text of the survey from database */
      Svy_DB_GetSurveyTxt (Surveys.Svy.SvyCod,Txt);
     }

   /***** Begin form *****/
   Frm_BeginForm (ItsANewSurvey ? ActNewSvy :
	                          ActChgSvy);
      Svy_PutPars (&Surveys);

      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,
			 Surveys.Svy.Title[0] ? Surveys.Svy.Title :
						Txt_Survey,
			 NULL,NULL,
			 Hlp_ANALYTICS_Surveys_edit_survey,Box_NOT_CLOSABLE,2);

	 /***** Scope of the survey *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT","ScopeSvy",Txt_Scope);

	    /* Data */
	    HTM_TD_Begin ("class=\"LT\"");
	       Svy_SetAllowedScopes (&Surveys.Svy);
	       Sco_GetScope ("ScopeSvy",Surveys.Svy.Level);
	       Sco_PutSelectorScope ("ScopeSvy",HTM_DONT_SUBMIT_ON_CHANGE);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Survey title *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT","Title",Txt_Title);

	    /* Data */
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_INPUT_TEXT ("Title",Svy_MAX_CHARS_SURVEY_TITLE,Surveys.Svy.Title,
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "id=\"Title\""
			       " class=\"TITLE_DESCRIPTION_WIDTH INPUT_%s\""
			       " required=\"required\"",
			       The_GetSuffix ());
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Survey start and end dates *****/
	 Dat_PutFormStartEndClientLocalDateTimes (Surveys.Svy.TimeUTC,
						  Dat_FORM_SECONDS_ON,
						  SetHMS);

	 /***** Survey text *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT","Txt",Txt_Description);

	    /* Data */
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_TEXTAREA_Begin ("id=\"Txt\" name=\"Txt\" rows=\"5\""
				   " class=\"TITLE_DESCRIPTION_WIDTH INPUT_%s\"",
				   The_GetSuffix ());
		  if (!ItsANewSurvey)
		     HTM_Txt (Txt);
	       HTM_TEXTAREA_End ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Users' roles who can answer the survey *****/
	 HTM_TR_Begin (NULL);
	    HTM_TD_TxtColon (Txt_Users);
	    HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	       Rol_WriteSelectorRoles (1 << Rol_STD |
				       1 << Rol_NET |
				       1 << Rol_TCH,
				       Surveys.Svy.Roles,
				       false,false);
	    HTM_TD_End ();
	 HTM_TR_End ();

	 /***** Groups *****/
	 Svy_ShowLstGrpsToEditSurvey (Surveys.Svy.SvyCod);

      /***** End table, send button and end box *****/
      if (ItsANewSurvey)
	 Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create);
      else
	 Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();

   /***** Show questions of the survey ready to be edited *****/
   if (!ItsANewSurvey)
      Svy_ListSvyQuestions (&Surveys);

   /***** Show all surveys *****/
   Svy_ListAllSurveys (&Surveys);
  }

/*****************************************************************************/
/************ Set allowed scopes depending on logged user's role *************/
/*****************************************************************************/

static void Svy_SetAllowedScopes (struct Svy_Survey *Svy)
  {
   bool ICanEdit = false;

   /***** Set allowed scopes *****/
   Gbl.Scope.Allowed = 0;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:	// Teachers only can edit course surveys
	 if (Gbl.Hierarchy.Level == Hie_CRS)	// Course selected
	   {
	    if (Svy->Level == Hie_UNK)		// Scope not defined
	       Svy->Level = Hie_CRS;
	    if (Svy->Level == Hie_CRS)
	      {
	       Gbl.Scope.Allowed = 1 << Hie_CRS;
	       ICanEdit = true;
	      }
	   }
         break;
      case Rol_DEG_ADM:	// Degree admins only can edit degree surveys
	 if (Svy->Level == Hie_UNK)		// Scope not defined
	    Svy->Level = Hie_DEG;
	 if (Svy->Level == Hie_DEG)
	   {
	    Gbl.Scope.Allowed = 1 << Hie_DEG;
	    ICanEdit = true;
	   }
         break;
      case Rol_CTR_ADM:	// Center admins only can edit center surveys
	 if (Svy->Level == Hie_UNK)		// Scope not defined
	    Svy->Level = Hie_CTR;
	 if (Svy->Level == Hie_CTR)
	   {
	    Gbl.Scope.Allowed = 1 << Hie_CTR;
	    ICanEdit = true;
	   }
         break;
      case Rol_INS_ADM:	// Institution admins only can edit institution surveys
	 if (Svy->Level == Hie_UNK)		// Scope not defined
	    Svy->Level = Hie_INS;
	 if (Svy->Level == Hie_INS)
	   {
	    Gbl.Scope.Allowed = 1 << Hie_INS;
	    ICanEdit = true;
	   }
         break;
      case Rol_SYS_ADM:// System admins can edit any survey
	 if (Svy->Level == Hie_UNK)	// Scope not defined
	    Svy->Level = (Gbl.Hierarchy.Level < Hie_NUM_LEVELS &&
		          Gbl.Hierarchy.Level != Hie_UNK) ? Gbl.Hierarchy.Level :
		        	                            Hie_SYS;
         Gbl.Scope.Allowed = 1 << Hie_SYS |
	                     1 << Hie_CTY |
	                     1 << Hie_INS |
	                     1 << Hie_CTR |
                             1 << Hie_DEG |
                             1 << Hie_CRS;
	 ICanEdit = true;
	 break;
      default:
	 break;
     }

   if (!ICanEdit)
      Err_NoPermissionExit ();
  }

/*****************************************************************************/
/******************** Show list of groups to edit a survey *******************/
/*****************************************************************************/

static void Svy_ShowLstGrpsToEditSurvey (long SvyCod)
  {
   extern const char *Txt_Groups;
   extern const char *Txt_The_whole_course;
   unsigned NumGrpTyp;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   if (Gbl.Crs.Grps.GrpTypes.NumGrpTypes)
     {
      /***** Begin box and table *****/
      HTM_TR_Begin (NULL);
	 HTM_TD_TxtColon (Txt_Groups);
	 HTM_TD_Begin ("class=\"LT\"");
	    Box_BoxTableBegin ("95%",NULL,
			       NULL,NULL,
			       NULL,Box_NOT_CLOSABLE,0);

	    /***** First row: checkbox to select the whole course *****/
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("colspan=\"7\" class=\"LM DAT_%s\"",
	                     The_GetSuffix ());
		  HTM_LABEL_Begin (NULL);
		     HTM_INPUT_CHECKBOX ("WholeCrs",HTM_DONT_SUBMIT_ON_CHANGE,
					 "id=\"WholeCrs\" value=\"Y\"%s onclick=\"uncheckChildren(this,'GrpCods')\"",
					 Grp_DB_CheckIfAssociatedToGrps ("svy_groups",
					                                 "SvyCod",
					                                 SvyCod) ? "" :
									           " checked=\"checked\"");
		     HTM_TxtF ("%s&nbsp;%s",Txt_The_whole_course,Gbl.Hierarchy.Node[Hie_CRS].ShrtName);
		  HTM_LABEL_End ();
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** List the groups for each group type *****/
	    for (NumGrpTyp = 0;
		 NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
		 NumGrpTyp++)
	       if (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
		  Grp_ListGrpsToEditAsgAttSvyEvtMch (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],
						     Grp_SURVEY,SvyCod);

	    /***** End table and box *****/
	    Box_BoxTableEnd ();
	 HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/********************* Receive form to create a new survey *******************/
/*****************************************************************************/

void Svy_ReceiveFormSurvey (void)
  {
   extern const char *Txt_Already_existed_a_survey_with_the_title_X;
   struct Svy_Surveys Surveys;
   struct Svy_Survey OldSvy;
   struct Svy_Survey NewSvy;
   bool ItsANewSurvey;
   bool NewSurveyIsCorrect = true;
   unsigned NumUsrsToBeNotifiedByEMail;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset surveys *****/
   Svy_ResetSurveys (&Surveys);

   /***** Get parameters *****/
   Surveys.SelectedOrder = Svy_GetParSvyOrder ();
   Grp_GetParWhichGroups ();
   Surveys.CurrentPage = Pag_GetParPagNum (Pag_SURVEYS);

   /***** Get the code of the survey *****/
   ItsANewSurvey = ((NewSvy.SvyCod = ParCod_GetPar (ParCod_Svy)) <= 0);

   if (ItsANewSurvey)
      NewSvy.Level = Hie_UNK;
   else
     {
      /* Get data of the old (current) survey from database */
      OldSvy.SvyCod = NewSvy.SvyCod;
      Svy_GetSurveyDataByCod (&OldSvy);
      if (!OldSvy.Status.ICanEdit)
         Err_NoPermissionExit ();
      NewSvy.Level = OldSvy.Level;
     }

   /***** Get scope *****/
   Svy_SetAllowedScopes (&NewSvy);
   Sco_GetScope ("ScopeSvy",NewSvy.Level);
   if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM)
      switch (Gbl.Scope.Current)
	{
	 case Hie_SYS:
	 case Hie_CTY:
	 default:
	    Err_WrongHierarchyLevelExit ();
	    break;
	 case Hie_INS:
	    if (Gbl.Usrs.Me.Role.Logged != Rol_INS_ADM)
	       Err_WrongHierarchyLevelExit ();
	    break;
	 case Hie_CTR:
	    if (Gbl.Usrs.Me.Role.Logged != Rol_CTR_ADM)
	       Err_WrongHierarchyLevelExit ();
	    break;
	 case Hie_DEG:
	    if (Gbl.Usrs.Me.Role.Logged != Rol_DEG_ADM)
	       Err_WrongHierarchyLevelExit ();
	    break;
	 case Hie_CRS:
	    if (Gbl.Usrs.Me.Role.Logged != Rol_TCH)
	       Err_WrongHierarchyLevelExit ();
	    break;
	}
   NewSvy.Level = Gbl.Scope.Current;
   NewSvy.HieCod = Gbl.Hierarchy.Node[Gbl.Scope.Current].HieCod;

   /***** Get start/end date-times *****/
   NewSvy.TimeUTC[Dat_STR_TIME] = Dat_GetTimeUTCFromForm (Dat_STR_TIME);
   NewSvy.TimeUTC[Dat_END_TIME] = Dat_GetTimeUTCFromForm (Dat_END_TIME);

   /***** Get survey title *****/
   Par_GetParText ("Title",NewSvy.Title,Svy_MAX_BYTES_SURVEY_TITLE);

   /***** Get survey text and insert links *****/
   Par_GetParHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (NewSvy.TimeUTC[Dat_STR_TIME] == 0)
      NewSvy.TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();
   if (NewSvy.TimeUTC[Dat_END_TIME] == 0)
      NewSvy.TimeUTC[Dat_END_TIME] = NewSvy.TimeUTC[Dat_STR_TIME] + 24 * 60 * 60;	// +24 hours

   /***** Get users who can answer this survey *****/
   NewSvy.Roles = Rol_GetSelectedRoles ();

   /***** Check if title is correct *****/
   if (NewSvy.Title[0])	// If there's a survey title
     {
      /* If title of survey was in database... */
      if (Svy_DB_CheckIfSimilarSurveyExists (&NewSvy))
        {
         NewSurveyIsCorrect = false;
         Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_a_survey_with_the_title_X,
                        NewSvy.Title);
        }
     }
   else	// If there is not a survey title
     {
      NewSurveyIsCorrect = false;
      Ale_CreateAlertYouMustSpecifyTheTitle ();
     }

   /***** Create a new survey or update an existing one *****/
   if (NewSurveyIsCorrect)
     {
      /* Get groups for this surveys */
      Grp_GetParCodsSeveralGrps ();

      if (ItsANewSurvey)
         Svy_CreateSurvey (&NewSvy,Txt);	// Add new survey to database
      else
         Svy_UpdateSurvey (&NewSvy,Txt);

      /* Free memory for list of selected groups */
      Grp_FreeListCodSelectedGrps ();
     }
   else
      Svy_ReqCreatOrEditSvy ();

   /***** Notify by email about the new survey *****/
   if (NewSvy.Level == Hie_CRS)	// Notify only the surveys for a course, not for a degree or global
      if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_SURVEY,NewSvy.SvyCod)))
         Svy_DB_UpdateNumUsrsNotifiedByEMailAboutSurvey (NewSvy.SvyCod,NumUsrsToBeNotifiedByEMail);

   /***** Show surveys again *****/
   Svy_ListAllSurveys (&Surveys);
  }

/*****************************************************************************/
/*************************** Create a new survey *****************************/
/*****************************************************************************/

static void Svy_CreateSurvey (struct Svy_Survey *Svy,const char *Txt)
  {
   extern const char *Txt_Created_new_survey_X;

   /***** Create a new survey *****/
   Svy->SvyCod = Svy_DB_CreateSurvey (Svy,Txt);

   /***** Create groups *****/
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Svy_CreateGrps (Svy->SvyCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_survey_X,
                  Svy->Title);
  }

/*****************************************************************************/
/************************* Update an existing survey *************************/
/*****************************************************************************/

static void Svy_UpdateSurvey (struct Svy_Survey *Svy,const char *Txt)
  {
   extern const char *Txt_The_survey_has_been_modified;

   /***** Update the data of the survey *****/
   Svy_DB_UpdateSurvey (Svy,Txt);

   /***** Update groups *****/
   /* Remove old groups */
   Svy_DB_RemoveGrpsAssociatedToSurvey (Svy->SvyCod);

   /* Create new groups */
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Svy_CreateGrps (Svy->SvyCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_survey_has_been_modified);
  }

/*****************************************************************************/
/************************ Create groups of a survey **************************/
/*****************************************************************************/

static void Svy_CreateGrps (long SvyCod)
  {
   unsigned NumGrpSel;

   /***** Create groups of the survey *****/
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
      Svy_DB_CreateGrp (SvyCod,Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]);
  }

/*****************************************************************************/
/************ Get and write the names of the groups of a survey **************/
/*****************************************************************************/

static void Svy_GetAndWriteNamesOfGrpsAssociatedToSvy (struct Svy_Survey *Svy)
  {
   extern const char *HidVis_GroupClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *Txt_and;
   extern const char *Txt_The_whole_course;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrps;
   unsigned NumGrp;

   /***** Get groups associated to a survey from database *****/
   NumGrps = Svy_DB_GetGrpNamesAssociatedToSvy (&mysql_res,Svy->SvyCod);

   /***** Write heading *****/
   HTM_DIV_Begin ("class=\"%s_%s\"",
		  HidVis_GroupClass[Svy->Status.HiddenOrVisible],
		  The_GetSuffix ());
      HTM_TxtColonNBSP (NumGrps == 1 ? Txt_Group  :
				       Txt_Groups);

      /***** Write groups *****/
      if (NumGrps) // Groups found...
	{
	 /* Get and write the group types and names */
	 for (NumGrp = 0;
	      NumGrp < NumGrps;
	      NumGrp++)
	   {
	    /* Get next group */
	    row = mysql_fetch_row (mysql_res);

	    /* Write group type name and group name */
	    HTM_TxtF ("%s&nbsp;%s",row[0],row[1]);

	    if (NumGrps >= 2)
	      {
	       if (NumGrp == NumGrps - 2)
		  HTM_TxtF (" %s ",Txt_and);
	       if (NumGrps >= 3)
		 if (NumGrp < NumGrps - 2)
		     HTM_Txt (", ");
	      }
	   }
	}
      else
	 HTM_TxtF ("%s&nbsp;%s",Txt_The_whole_course,
		   Gbl.Hierarchy.Node[Hie_CRS].ShrtName);

   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Remove all surveys in a place on the hierarchy   **************/
/************* (country, institution, center, degree or course) **************/
/*****************************************************************************/

void Svy_RemoveSurveys (Hie_Level_t Level,long HieCod)
  {
   /***** Remove all users in surveys *****/
   Svy_DB_RemoveUsrsWhoHaveAnsweredSvysIn (Level,HieCod);

   /***** Remove all answers and comments in surveys *****/
   Svy_DB_RemoveAnswersSvysIn (Level,HieCod);
   Svy_DB_RemoveCommentsSvysIn (Level,HieCod);

   /***** Remove all questions in surveys *****/
   Svy_DB_RemoveQstsSvysIn (Level,HieCod);

   /***** Remove all groups *****/
   Svy_DB_RemoveGrpsSvysIn (Level,HieCod);

   /***** Remove all surveys *****/
   Svy_DB_RemoveSvysIn (Level,HieCod);
  }

/*****************************************************************************/
/*********** Put a form to edit/create a question in survey  *****************/
/*****************************************************************************/

void Svy_ReqEditQuestion (void)
  {
   struct Svy_Surveys Surveys;
   struct Svy_Question SvyQst;
   char Stem[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset surveys *****/
   Svy_ResetSurveys (&Surveys);

   /***** Initialize question to zero *****/
   Svy_InitQst (&SvyQst);

   /***** Initialize text to empty string *****/
   Stem[0] = '\0';

   /***** Get survey code *****/
   Surveys.Svy.SvyCod = ParCod_GetAndCheckPar (ParCod_Svy);

   /* Get the question code */
   SvyQst.QstCod = ParCod_GetPar (ParCod_Qst);

   /***** Get other parameters *****/
   Surveys.SelectedOrder = Svy_GetParSvyOrder ();
   Grp_GetParWhichGroups ();
   Surveys.CurrentPage = Pag_GetParPagNum (Pag_SURVEYS);

   /***** Show form to create a new question in this survey *****/
   Svy_ShowFormEditOneQst (&Surveys,&SvyQst,Stem);

   /***** Show current survey *****/
   Svy_ShowOneSurvey (&Surveys,true);
  }

/*****************************************************************************/
/******************* Show form to edit one survey question *******************/
/*****************************************************************************/

static void Svy_ShowFormEditOneQst (struct Svy_Surveys *Surveys,
                                    struct Svy_Question *SvyQst,
                                    char Stem[Cns_MAX_BYTES_TEXT + 1])
  {
   extern const char *Hlp_ANALYTICS_Surveys_questions;
   extern const char *Txt_Question;
   extern const char *Txt_Wording;
   extern const char *Txt_Type;
   extern const char *Txt_SURVEY_STR_ANSWER_TYPES[Svy_NUM_ANS_TYPES];
   extern const char *Txt_Options;
   extern const char *Txt_Comments;
   extern const char *Txt_Comments_allowed;
   extern const char *Txt_Save_changes;
   extern const char *Txt_Create;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAns;
   unsigned NumAnswers = 0;
   char *Title;
   Svy_AnswerType_t AnsType;
   bool NewQuestion = (SvyQst->QstCod <= 0);

   if (Gbl.Action.Act == ActEdiOneSvyQst) // If no receiving the question, but editing a new or existing question
      if (!NewQuestion)
        {
         /***** Get question data from database *****/
         if (Svy_DB_GetQstDataByCod (&mysql_res,SvyQst->QstCod,Surveys->Svy.SvyCod))
            Svy_GetQstDataFromRow (mysql_res,SvyQst,Stem);
         else
            Err_WrongQuestionExit ();

         /* Free structure that stores the query result */
	 DB_FreeMySQLResult (&mysql_res);

         /***** Get the answers from the database *****/
         NumAnswers = Svy_DB_GetAnswersQst (&mysql_res,SvyQst->QstCod);	// Result: AnsInd,NumUsrs,Answer
         for (NumAns = 0;
              NumAns < NumAnswers;
              NumAns++)
           {
            row = mysql_fetch_row (mysql_res);

            if (NumAnswers > Svy_MAX_ANSWERS_PER_QUESTION)
               Err_WrongAnswerExit ();
            if (!Svy_AllocateTextChoiceAnswer (SvyQst,NumAns))
	       /* Abort on error */
	       Ale_ShowAlertsAndExit ();

            Str_Copy (SvyQst->AnsChoice[NumAns].Text,row[2],Svy_MAX_BYTES_ANSWER);
           }
         /* Free structure that stores the query result */
	 DB_FreeMySQLResult (&mysql_res);
        }

   /***** Begin box *****/
   if (NewQuestion)
      Box_BoxBegin (NULL,Txt_Question,
                    NULL,NULL,
                    Hlp_ANALYTICS_Surveys_questions,Box_NOT_CLOSABLE);
   else
     {
      /* Parameters for contextual icon */
      Surveys->QstCod = SvyQst->QstCod;

      if (asprintf (&Title,"%s %u",Txt_Question,SvyQst->QstInd + 1) < 0)	// Question index may be 0, 1, 2, 3,...
	 Err_NotEnoughMemoryExit ();
      Box_BoxBegin (NULL,Title,
                    Svy_PutIconToRemoveOneQst,Surveys,
                    NULL,Box_NOT_CLOSABLE);
      free (Title);
     }

   /***** Begin form *****/
   Frm_BeginForm (NewQuestion ? ActNewSvyQst :
				ActChgSvyQst);
      ParCod_PutPar (ParCod_Svy,Surveys->Svy.SvyCod);
      ParCod_PutPar (ParCod_Qst,SvyQst->QstCod);

      /***** Begin table *****/
      HTM_TABLE_BeginWidePadding (2);

	 /***** Stem *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT","Txt",Txt_Wording);

	    /* Data */
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_TEXTAREA_Begin ("id=\"Txt\" name=\"Txt\""
			           " cols=\"60\" rows=\"4\""
			           " class=\"INPUT_%s\"",
			           The_GetSuffix ());
		  HTM_Txt (Stem);
	       HTM_TEXTAREA_End ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Type of answer *****/
	 HTM_TR_Begin (NULL);
	    HTM_TD_TxtColon (Txt_Type);
	    HTM_TD_Begin ("class=\"LT FORM_IN_%s\"",The_GetSuffix ());
	       for (AnsType  = (Svy_AnswerType_t) 0;
		    AnsType <= (Svy_AnswerType_t) (Svy_NUM_ANS_TYPES - 1);
		    AnsType++)
		 {
		  HTM_LABEL_Begin (NULL);
		     HTM_INPUT_RADIO ("AnswerType",HTM_DONT_SUBMIT_ON_CLICK,
				      "value=\"%u\"%s",
				      (unsigned) AnsType,
				      AnsType == SvyQst->AnswerType ? " checked=\"checked\"" : "");
		     HTM_Txt (Txt_SURVEY_STR_ANSWER_TYPES[AnsType]);
		  HTM_LABEL_End ();
		  HTM_BR ();
		 }
	    HTM_TD_End ();
	 HTM_TR_End ();

	 /***** Answers *****/
	 HTM_TR_Begin (NULL);
	    HTM_TD_TxtColon (Txt_Options);
	    HTM_TD_Begin ("class=\"LT\"");

	       /* Unique or multiple choice answers */
	       HTM_TABLE_BeginPadding (2);
		  for (NumAns = 0;
		       NumAns < Svy_MAX_ANSWERS_PER_QUESTION;
		       NumAns++)
		    {
		     HTM_TR_Begin (NULL);

			/* Label with the number of the answer */
			HTM_TD_Begin ("class=\"RT\"");
			   HTM_LABEL_Begin ("for=\"AnsStr%u\" class=\"FORM_IN_%s\"",
					    NumAns,The_GetSuffix ());
			      HTM_TxtF ("%u)",NumAns + 1);
			   HTM_LABEL_End ();
			HTM_TD_End ();

			/* Answer text */
			HTM_TD_Begin ("class=\"RT\"");
			   HTM_TEXTAREA_Begin ("id=\"AnsStr%u\" name=\"AnsStr%u\" cols=\"50\" rows=\"1\"",
					       NumAns,NumAns);
			      if (SvyQst->AnsChoice[NumAns].Text)
				 HTM_Txt (SvyQst->AnsChoice[NumAns].Text);
			   HTM_TEXTAREA_End ();
			HTM_TD_End ();

		     HTM_TR_End ();
		    }
	       HTM_TABLE_End ();

	    HTM_TD_End ();
	 HTM_TR_End ();

	 /***** Comments allowed? *****/
	 HTM_TR_Begin (NULL);
	    HTM_TD_TxtColon (Txt_Comments);
	    HTM_TD_Begin ("class=\"LT FORM_IN_%s\"",The_GetSuffix ());
	       HTM_LABEL_Begin (NULL);
		  HTM_INPUT_CHECKBOX ("Comment",HTM_DONT_SUBMIT_ON_CHANGE,
				      "value=\"Y\"%s",
				      SvyQst->CommentsAllowed ? " checked=\"checked\"" :
							        "");
		  HTM_Txt (Txt_Comments_allowed);
	       HTM_LABEL_End ();
	    HTM_TD_End ();
	 HTM_TR_End ();

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Send button *****/
      if (NewQuestion)	// If the question already has assigned a code
	 Btn_PutCreateButton (Txt_Create);
      else
	 Btn_PutConfirmButton (Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free memory for answers *****/
   Svy_FreeTextChoiceAnswers (SvyQst,NumAnswers);
  }

/*****************************************************************************/
/********************* Initialize a new question to zero *********************/
/*****************************************************************************/

static void Svy_InitQst (struct Svy_Question *SvyQst)
  {
   unsigned NumAns;

   SvyQst->QstCod = -1L;
   SvyQst->QstInd = 0;
   SvyQst->AnswerType = Svy_ANS_UNIQUE_CHOICE;
   for (NumAns = 0;
	NumAns < Svy_MAX_ANSWERS_PER_QUESTION;
	NumAns++)
      SvyQst->AnsChoice[NumAns].Text = NULL;
   SvyQst->CommentsAllowed = false;
  }

/*****************************************************************************/
/******************* Allocate memory for a choice answer *********************/
/*****************************************************************************/

static bool Svy_AllocateTextChoiceAnswer (struct Svy_Question *SvyQst,
                                          unsigned NumAns)
  {
   Svy_FreeTextChoiceAnswer (SvyQst,NumAns);
   if ((SvyQst->AnsChoice[NumAns].Text = malloc (Svy_MAX_BYTES_ANSWER + 1)) == NULL)
     {
      Ale_CreateAlert (Ale_ERROR,NULL,
	               "Not enough memory to store answer.");
      return false;
     }
   SvyQst->AnsChoice[NumAns].Text[0] = '\0';
   return true;
  }

/*****************************************************************************/
/******************** Free memory of all choice answers **********************/
/*****************************************************************************/

static void Svy_FreeTextChoiceAnswers (struct Svy_Question *SvyQst,unsigned NumAnswers)
  {
   unsigned NumAns;

   for (NumAns = 0;
	NumAns < NumAnswers;
	NumAns++)
      Svy_FreeTextChoiceAnswer (SvyQst,NumAns);
  }

/*****************************************************************************/
/********************** Free memory of a choice answer ***********************/
/*****************************************************************************/

static void Svy_FreeTextChoiceAnswer (struct Svy_Question *SvyQst,unsigned NumAns)
  {
   if (SvyQst->AnsChoice[NumAns].Text)
     {
      free (SvyQst->AnsChoice[NumAns].Text);
      SvyQst->AnsChoice[NumAns].Text = NULL;
     }
  }

/*****************************************************************************/
/*********************** Receive a question of a survey **********************/
/*****************************************************************************/

void Svy_ReceiveQst (void)
  {
   extern const char *Txt_You_must_type_the_question_stem;
   extern const char *Txt_You_can_not_leave_empty_intermediate_answers;
   extern const char *Txt_The_survey_has_been_modified;
   struct Svy_Surveys Surveys;
   struct Svy_Question SvyQst;
   char Stem[Cns_MAX_BYTES_TEXT + 1];
   unsigned NumAns;
   char AnsStr[8 + 10 + 1];
   bool ThereIsEndOfAnswers;
   bool Error = false;

   /***** Reset surveys *****/
   Svy_ResetSurveys (&Surveys);

   /***** Initialize question to zero *****/
   Svy_InitQst (&SvyQst);

   /***** Get parameters from form *****/
   /* Get survey code */
   Surveys.Svy.SvyCod = ParCod_GetAndCheckPar (ParCod_Svy);

   /* Get question code */
   SvyQst.QstCod = ParCod_GetPar (ParCod_Qst);

   /* Get answer type */
   SvyQst.AnswerType = (Svy_AnswerType_t)
	               Par_GetParUnsignedLong ("AnswerType",
	                                       0,
	                                       Svy_NUM_ANS_TYPES - 1,
                                               (unsigned long) Svy_ANSWER_TYPE_DEFAULT);

   /* Get question text */
   Par_GetParHTML ("Txt",Stem,Cns_MAX_BYTES_TEXT);

   /* Get the texts of the answers */
   for (NumAns = 0;
	NumAns < Svy_MAX_ANSWERS_PER_QUESTION;
	NumAns++)
     {
      if (!Svy_AllocateTextChoiceAnswer (&SvyQst,NumAns))
	 /* Abort on error */
	 Ale_ShowAlertsAndExit ();
      snprintf (AnsStr,sizeof (AnsStr),"AnsStr%u",NumAns);
      Par_GetParHTML (AnsStr,SvyQst.AnsChoice[NumAns].Text,Svy_MAX_BYTES_ANSWER);
     }

   /* Get if comments are allowed */
   SvyQst.CommentsAllowed = Par_GetParBool ("Comment");

   /***** Make sure that stem is not empty *****/
   if (Stem[0])
     {
      for (NumAns = 0, ThereIsEndOfAnswers = false;
	   !Error && NumAns < Svy_MAX_ANSWERS_PER_QUESTION;
	   NumAns++)
	 if (SvyQst.AnsChoice[NumAns].Text[0])
	   {
	    if (ThereIsEndOfAnswers)
	      {
	       Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_empty_intermediate_answers);
	       Error = true;
	      }
	   }
	 else
	    ThereIsEndOfAnswers = true;
     }
   else
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_the_question_stem);
      Error = true;
     }

   if (Error)
      Svy_ShowFormEditOneQst (&Surveys,&SvyQst,Stem);
   else
     {
      /***** Form is received OK ==> insert question and answer in the database *****/
      if (SvyQst.QstCod < 0)	// It's a new question
        {
         SvyQst.QstInd = Svy_GetNextQuestionIndexInSvy (Surveys.Svy.SvyCod);
         SvyQst.QstCod = Svy_DB_CreateQuestion (Surveys.Svy.SvyCod,&SvyQst,Stem);
        }
      else			// It's an existing question
         /* Update question */
         Svy_DB_UpdateQuestion (Surveys.Svy.SvyCod,&SvyQst,Stem);

      /* Insert, update or delete answers in the answers table */
      for (NumAns = 0;
	   NumAns < Svy_MAX_ANSWERS_PER_QUESTION;
	   NumAns++)
         if (Svy_DB_CheckIfAnswerExists (SvyQst.QstCod,NumAns))	// If this answer exists...
           {
            if (SvyQst.AnsChoice[NumAns].Text[0])	// Answer is not empty
               /* Update answer text */
               Svy_DB_UpdateAnswerText (SvyQst.QstCod,NumAns,
                                        SvyQst.AnsChoice[NumAns].Text);
            else					// Answer is empty
               /* Delete answer from database */
               Svy_DB_RemoveAnswerQst (SvyQst.QstCod,NumAns);
           }
         else	// If this answer does not exist...
           {
            if (SvyQst.AnsChoice[NumAns].Text[0])	// Answer is not empty
               /* Create answer into database */
               Svy_DB_CreateAnswer (SvyQst.QstCod,NumAns,
                                    SvyQst.AnsChoice[NumAns].Text);
           }

      /***** List the questions of this survey, including the new one just inserted into the database *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_The_survey_has_been_modified);
     }

   /***** Free answers *****/
   Svy_FreeTextChoiceAnswers (&SvyQst,Svy_MAX_ANSWERS_PER_QUESTION);

   /***** Show current survey *****/
   Svy_ShowOneSurvey (&Surveys,true);
  }

/*****************************************************************************/
/******************* Get next question index in a survey *********************/
/*****************************************************************************/

static unsigned Svy_GetNextQuestionIndexInSvy (long SvyCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned QstInd = 0;

   /***** Get last question index *****/
   if (Svy_DB_GetLastQstInd (&mysql_res,SvyCod))
     {
      row = mysql_fetch_row (mysql_res);
      if (row[0])	// There are questions
	{
	 if (sscanf (row[0],"%u",&QstInd) != 1)
	    Err_WrongQuestionIndexExit ();
	 QstInd++;
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return QstInd;
  }

/*****************************************************************************/
/************************ List the questions of a survey *********************/
/*****************************************************************************/

static void Svy_ListSvyQuestions (struct Svy_Surveys *Surveys)
  {
   extern const char *Hlp_ANALYTICS_Surveys_questions;
   extern const char *Txt_Questions;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Type;
   extern const char *Txt_Question;
   extern const char *Txt_SURVEY_STR_ANSWER_TYPES[Svy_NUM_ANS_TYPES];
   extern const char *Txt_This_survey_has_no_questions;
   extern const char *Txt_Done;
   MYSQL_RES *mysql_res;
   unsigned NumQsts;
   unsigned NumQst;
   struct Svy_Question SvyQst;
   char Stem[Cns_MAX_BYTES_TEXT + 1];
   bool Editing = (Gbl.Action.Act == ActEdiOneSvy    ||
	           Gbl.Action.Act == ActEdiOneSvyQst ||
	           Gbl.Action.Act == ActNewSvyQst    ||
	           Gbl.Action.Act == ActChgSvyQst);
   bool PutFormAnswerSurvey = Surveys->Svy.Status.ICanAnswer && !Editing;

   /***** Begin box *****/
   if (Surveys->Svy.Status.ICanEdit)
      Box_BoxBegin (NULL,Txt_Questions,
		    Svy_PutIconToAddNewQuestion,Surveys,
		    Hlp_ANALYTICS_Surveys_questions,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,Txt_Questions,
		    NULL,NULL,
		    Hlp_ANALYTICS_Surveys_questions,Box_NOT_CLOSABLE);

   /***** Get data of questions from database *****/
   if ((NumQsts = Svy_DB_GetSurveyQsts (&mysql_res,Surveys->Svy.SvyCod)))
     {
      if (PutFormAnswerSurvey)
	{
	 /***** Begin form to send answers to survey *****/
	 Frm_BeginForm (ActAnsSvy);
	    ParCod_PutPar (ParCod_Svy,Surveys->Svy.SvyCod);
	}

      /***** Write the heading *****/
      HTM_TABLE_BeginWideMarginPadding (5);

	 HTM_TR_Begin (NULL);
	    if (Surveys->Svy.Status.ICanEdit)
	       HTM_TH_Empty (1);
	    HTM_TH (Txt_No_INDEX,HTM_HEAD_CENTER);
	    HTM_TH (Txt_Type    ,HTM_HEAD_CENTER);
	    HTM_TH (Txt_Question,HTM_HEAD_LEFT  );
	 HTM_TR_End ();

	 /***** Write questions one by one *****/
	 for (NumQst = 0, The_ResetRowColor ();
	      NumQst < NumQsts;
	      NumQst++, The_ChangeRowColor ())
	   {
	    /* Initialize question to zero */
	    Svy_InitQst (&SvyQst);

	    /* Get question data from row */
	    Svy_GetQstDataFromRow (mysql_res,&SvyQst,Stem);

	    HTM_TR_Begin (NULL);

	       if (Surveys->Svy.Status.ICanEdit)
		 {
		  HTM_TD_Begin ("class=\"BT %s\"",The_GetColorRows ());

		     /* Initialize context */
		     Surveys->QstCod = SvyQst.QstCod;

		     /* Write icon to remove the question */
		     Ico_PutContextualIconToRemove (ActReqRemSvyQst,NULL,
						    Svy_PutParsToEditQuestion,Surveys);

		     /* Write icon to edit the question */
		     Ico_PutContextualIconToEdit (ActEdiOneSvyQst,NULL,
						  Svy_PutParsToEditQuestion,Surveys);

		  HTM_TD_End ();
		 }

	       /* Write index of question inside survey */
	       HTM_TD_Begin ("class=\"CT DAT_SMALL_%s %s\"",
	                     The_GetSuffix (),
	                     The_GetColorRows ());
		  HTM_Unsigned (SvyQst.QstInd + 1);
	       HTM_TD_End ();

	       /* Write the question type (row[2]) */
	       HTM_TD_Begin ("class=\"CT DAT_SMALL_%s %s\"",
	                     The_GetSuffix (),
	                     The_GetColorRows ());
		  HTM_Txt (Txt_SURVEY_STR_ANSWER_TYPES[SvyQst.AnswerType]);
	       HTM_TD_End ();

	       /* Write the stem and the answers of this question */
	       HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
	                     The_GetSuffix (),
	                     The_GetColorRows ());
		  Svy_WriteQstStem (Stem);
		  Svy_WriteAnswersOfAQst (&Surveys->Svy,&SvyQst,PutFormAnswerSurvey);
		  if (SvyQst.CommentsAllowed)
		     Svy_WriteCommentsOfAQst (&Surveys->Svy,&SvyQst,PutFormAnswerSurvey);

	       HTM_TD_End ();

	    HTM_TR_End ();
	   }

      HTM_TABLE_End ();

      if (PutFormAnswerSurvey)
	{
	 /***** Button to create/modify survey *****/
	 Btn_PutConfirmButton (Txt_Done);

	 /***** End form *****/
	 Frm_EndForm ();
	}
     }
   else	// This survey has no questions
      Ale_ShowAlert (Ale_INFO,Txt_This_survey_has_no_questions);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************************* Get question data from row ************************/
/*****************************************************************************/

static void Svy_GetQstDataFromRow (MYSQL_RES *mysql_res,
                                   struct Svy_Question *SvyQst,
                                   char Stem[Cns_MAX_BYTES_TEXT + 1])
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get the code of the question (row[0]) *****/
   if (sscanf (row[0],"%ld",&(SvyQst->QstCod)) != 1)
      Err_WrongQuestionExit ();

   /***** Get the index of the question inside the survey (row[1]) *****/
   SvyQst->QstInd = Str_ConvertStrToUnsigned (row[1]);

   /***** Get the answer type (row[2]) *****/
   SvyQst->AnswerType = Svy_DB_ConvertFromStrAnsTypDBToAnsTyp (row[2]);

   /***** Get whether the comments are allowed (row[3]) *****/
   SvyQst->CommentsAllowed = (row[3][0] == 'Y');

   /***** Get the stem of the question from the database (row[4]) *****/
   Str_Copy (Stem,row[4],Cns_MAX_BYTES_TEXT);
  }

/*****************************************************************************/
/******************** Put parameters to edit a question **********************/
/*****************************************************************************/

static void Svy_PutParsToEditQuestion (void *Surveys)
  {
   if (Surveys)
     {
      ParCod_PutPar (ParCod_Svy,((struct Svy_Surveys *) Surveys)->Svy.SvyCod);
      ParCod_PutPar (ParCod_Qst,((struct Svy_Surveys *) Surveys)->QstCod);
     }
  }

/*****************************************************************************/
/***************** Put icon to add a new question to survey ******************/
/*****************************************************************************/

static void Svy_PutIconToAddNewQuestion (void *Surveys)
  {
   Ico_PutContextualIconToAdd (ActEdiOneSvyQst,NULL,Svy_PutPars,Surveys);
  }

/*****************************************************************************/
/****************** Write the heading of a survey question *******************/
/*****************************************************************************/

static void Svy_WriteQstStem (const char *Stem)
  {
   char *HeadingRigorousHTML;
   size_t Length;

   /* Convert the stem, that is in HTML, to rigorous HTML */
   Length = strlen (Stem) * Str_MAX_BYTES_PER_CHAR;
   if ((HeadingRigorousHTML = malloc (Length + 1)) == NULL)
      Err_NotEnoughMemoryExit ();
   Str_Copy (HeadingRigorousHTML,Stem,Length);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     HeadingRigorousHTML,Length,Str_DONT_REMOVE_SPACES);

   /* Write the stem */
   HTM_Txt (HeadingRigorousHTML);

   /* Free memory allocated for the stem */
   free (HeadingRigorousHTML);
  }

/*****************************************************************************/
/************** Get and write the answers of a survey question ***************/
/*****************************************************************************/

static void Svy_WriteAnswersOfAQst (struct Svy_Survey *Svy,
                                    struct Svy_Question *SvyQst,
                                    bool PutFormAnswerSurvey)
  {
   unsigned NumAnswers;
   unsigned NumAns;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrsThisAnswer;
   char StrAns[32];

   /***** Get answers of this question *****/
   NumAnswers = Svy_DB_GetAnswersQst (&mysql_res,SvyQst->QstCod);	// Result: AnsInd,NumUsrs,Answer

   /***** Write the answers *****/
   if (NumAnswers)
     {
      /* Check number of answers */
      if (NumAnswers > Svy_MAX_ANSWERS_PER_QUESTION)
         Err_WrongAnswerExit ();

      /* Write one row for each answer */
      HTM_TABLE_BeginPadding (5);

	 for (NumAns = 0;
	      NumAns < NumAnswers;
	      NumAns++)
	   {
	    row = mysql_fetch_row (mysql_res);

	    /* Get number of users who have marked this answer (row[1]) */
	    if (sscanf (row[1],"%u",&NumUsrsThisAnswer) != 1)
	       Err_ShowErrorAndExit ("Error when getting number of users who have marked an answer.");

	    /* Convert the answer (row[2]), that is in HTML, to rigorous HTML */
	    if (!Svy_AllocateTextChoiceAnswer (SvyQst,NumAns))
	       /* Abort on error */
	       Ale_ShowAlertsAndExit ();

	    Str_Copy (SvyQst->AnsChoice[NumAns].Text,row[2],Svy_MAX_BYTES_ANSWER);
	    Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			      SvyQst->AnsChoice[NumAns].Text,Svy_MAX_BYTES_ANSWER,
			      Str_DONT_REMOVE_SPACES);

	    /* Selectors and label with the letter of the answer */
	    HTM_TR_Begin (NULL);

	       if (PutFormAnswerSurvey)
		 {
		  /* Write selector to choice this answer */
		  HTM_TD_Begin ("class=\"LT\"");
		     snprintf (StrAns,sizeof (StrAns),"Ans%010u",
			       (unsigned) SvyQst->QstCod);
		     switch (SvyQst->AnswerType)
		       {
			case Svy_ANS_UNIQUE_CHOICE:
			   HTM_INPUT_RADIO (StrAns,HTM_DONT_SUBMIT_ON_CLICK,
					    "id=\"Ans%010u_%u\" value=\"%u\""
					    " onclick=\"selectUnselectRadio(this,this.form.Ans%010u,%u)\"",
					    (unsigned) SvyQst->QstCod,NumAns,NumAns,
					    (unsigned) SvyQst->QstCod,NumAnswers);
			   break;
			case Svy_ANS_MULTIPLE_CHOICE:
			   HTM_INPUT_CHECKBOX (StrAns,HTM_DONT_SUBMIT_ON_CHANGE,
					       "id=\"Ans%010u_%u\" value=\"%u\"",
					       (unsigned) SvyQst->QstCod,NumAns,NumAns);
			   break;
			default:
			   break;
		       }
		  HTM_TD_End ();
		 }

	       /* Write the number of option */
	       HTM_TD_Begin ("class=\"SVY_OPT LT\"");
		  HTM_LABEL_Begin ("for=\"Ans%010u_%u\" class=\"DAT_%s\"",
				   (unsigned) SvyQst->QstCod,NumAns,
				   The_GetSuffix ());
		     HTM_TxtF ("%u)",NumAns + 1);
		  HTM_LABEL_End ();
	       HTM_TD_End ();

	       /* Write the text of the answer */
	       HTM_TD_Begin ("class=\"LT\"");
		  HTM_LABEL_Begin ("for=\"Ans%010u_%u\" class=\"DAT_%s\"",
				   (unsigned) SvyQst->QstCod,NumAns,
				   The_GetSuffix ());
		     HTM_Txt (SvyQst->AnsChoice[NumAns].Text);
		  HTM_LABEL_End ();
	       HTM_TD_End ();

	       /* Show stats of this answer */
	       if (Svy->Status.ICanViewResults)
		  Svy_DrawBarNumUsrs (NumUsrsThisAnswer,Svy->NumUsrs);

	    HTM_TR_End ();

	    /* Free memory allocated for the answer */
	    Svy_FreeTextChoiceAnswer (SvyQst,NumAns);
	   }

      HTM_TABLE_End ();
     }
   else
      HTM_BR ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Get and write the answers of a survey question ***************/
/*****************************************************************************/

static void Svy_WriteCommentsOfAQst (struct Svy_Survey *Svy,
                                     struct Svy_Question *SvyQst,
                                     bool PutFormAnswerSurvey)
  {
   extern const char *Txt_Comments;
   unsigned NumComments;
   unsigned NumCom;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (PutFormAnswerSurvey)
     {
      HTM_TEXTAREA_Begin ("name=\"Com%010u\""
			  " cols=\"60\" rows=\"4\""
			  " class=\"INPUT_%s\""
			  " placeholder=\"%s&hellip;\"",
			  (unsigned) SvyQst->QstCod,
			  The_GetSuffix (),
			  Txt_Comments);
      HTM_TEXTAREA_End ();
     }
   else if (Svy->Status.ICanViewComments)
     {
      HTM_DL_Begin ();

	 HTM_DT_Begin ();
	    HTM_TxtColon (Txt_Comments);
	 HTM_DT_End ();

	 HTM_DD_Begin ();

	    /***** Get comments of this question *****/
	    NumComments = Svy_DB_GetCommentsQst (&mysql_res,SvyQst->QstCod);

	    /***** Write the answers *****/
	    if (NumComments)
	      {
	       HTM_OL_Begin ();

		  /* Write one row for each user who has commented */
		  for (NumCom = 0;
		       NumCom < NumComments;
		       NumCom++)
		    {
		     row = mysql_fetch_row (mysql_res);

		     HTM_LI_Begin (NULL);
			HTM_Txt (row[0]);
		     HTM_LI_End ();
		    }

	       HTM_OL_End ();
	      }

	    /***** Free structure that stores the query result *****/
	    DB_FreeMySQLResult (&mysql_res);

	 HTM_DD_End ();

      HTM_DL_End ();
     }
  }

/*****************************************************************************/
/***************** Draw a bar with the percentage of answers *****************/
/*****************************************************************************/

#define Svy_MAX_BAR_WIDTH 125

static void Svy_DrawBarNumUsrs (unsigned NumUsrs,unsigned MaxUsrs)
  {
   extern const char *Txt_of_PART_OF_A_TOTAL;
   unsigned BarWidth = 0;
   char *Title;

   /***** Build string with the number of users *****/
   if (MaxUsrs)
     {
      if (asprintf (&Title,"%u&nbsp;(%u%%&nbsp;%s&nbsp;%u)",
                    NumUsrs,
                    (unsigned) ((((double) NumUsrs * 100.0) / (double) MaxUsrs) + 0.5),
                    Txt_of_PART_OF_A_TOTAL,MaxUsrs) < 0)
         Err_NotEnoughMemoryExit ();
     }
   else
     {
      if (asprintf (&Title,"0&nbsp;(0%%&nbsp;%s&nbsp;%u)",
                    Txt_of_PART_OF_A_TOTAL,MaxUsrs) < 0)
         Err_NotEnoughMemoryExit ();
     }

   HTM_TD_Begin ("class=\"LT DAT_%s\" style=\"width:%upx;\"",
                 The_GetSuffix (),Svy_MAX_BAR_WIDTH + 125);

      /***** Draw bar with a with proportional to the number of clicks *****/
      if (NumUsrs && MaxUsrs)
	 BarWidth = (unsigned) ((((double) NumUsrs * (double) Svy_MAX_BAR_WIDTH) /
				  (double) MaxUsrs) + 0.5);
      if (BarWidth < 2)
	 BarWidth = 2;
      HTM_IMG (Cfg_URL_ICON_PUBLIC,"o1x1.png",Title,
	       "class=\"LT\" style=\"width:%upx; height:20px;\"",BarWidth);

      /***** Write the number of users *****/
      HTM_TxtF ("&nbsp;%s",Title);

      /***** Free string with the number of users *****/
      free (Title);

   HTM_TD_End ();
  }

/*****************************************************************************/
/********************* Put icon to remove one question ***********************/
/*****************************************************************************/

static void Svy_PutIconToRemoveOneQst (void *Surveys)
  {
   Ico_PutContextualIconToRemove (ActReqRemSvyQst,NULL,
				  Svy_PutParsRemoveOneQst,Surveys);
  }

/*****************************************************************************/
/****************** Put parameter to remove one question *********************/
/*****************************************************************************/

static void Svy_PutParsRemoveOneQst (void *Surveys)
  {
   if (Surveys)
     {
      ParCod_PutPar (ParCod_Svy,((struct Svy_Surveys *) Surveys)->Svy.SvyCod);
      ParCod_PutPar (ParCod_Qst,((struct Svy_Surveys *) Surveys)->QstCod);
     }
  }

/*****************************************************************************/
/********************** Request the removal of a question ********************/
/*****************************************************************************/

void Svy_ReqRemQst (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_question_X;
   struct Svy_Surveys Surveys;
   struct Svy_Question SvyQst;
   char StrQstInd[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Reset surveys *****/
   Svy_ResetSurveys (&Surveys);

   /***** Initialize question to zero *****/
   Svy_InitQst (&SvyQst);

   /***** Get parameters from form *****/
   /* Get survey code */
   Surveys.Svy.SvyCod = ParCod_GetAndCheckPar (ParCod_Svy);

   /* Get question code */
   SvyQst.QstCod = ParCod_GetAndCheckPar (ParCod_Qst);

   /* Get question index */
   SvyQst.QstInd = Svy_DB_GetQstIndFromQstCod (SvyQst.QstCod);

   /***** Show question and button to remove question *****/
   Surveys.QstCod = SvyQst.QstCod;
   sprintf (StrQstInd,"%u",SvyQst.QstInd + 1);
   Ale_ShowAlertRemove (ActRemSvyQst,NULL,
                        Svy_PutParsRemoveOneQst,&Surveys,
			Txt_Do_you_really_want_to_remove_the_question_X,
	                StrQstInd);

   /***** Show current survey *****/
   Svy_ShowOneSurvey (&Surveys,true);
  }

/*****************************************************************************/
/****************************** Remove a question ****************************/
/*****************************************************************************/

void Svy_RemoveQst (void)
  {
   extern const char *Txt_Question_removed;
   struct Svy_Surveys Surveys;
   struct Svy_Question SvyQst;

   /***** Reset surveys *****/
   Svy_ResetSurveys (&Surveys);

   /***** Initialize question to zero *****/
   Svy_InitQst (&SvyQst);

   /***** Get parameters from form *****/
   /* Get survey code */
   Surveys.Svy.SvyCod = ParCod_GetAndCheckPar (ParCod_Svy);

   /* Get question code */
   SvyQst.QstCod = ParCod_GetAndCheckPar (ParCod_Qst);

   /* Get question index */
   SvyQst.QstInd = Svy_DB_GetQstIndFromQstCod (SvyQst.QstCod);

   /***** Remove the question from all tables *****/
   /* Remove answers and comments from this survey question */
   Svy_DB_RemoveAnswersQst (SvyQst.QstCod);
   Svy_DB_RemoveCommentsQst (SvyQst.QstCod);

   /* Remove the question itself */
   Svy_DB_RemoveQst (SvyQst.QstCod);

   /* Change index of questions greater than this */
   Svy_DB_ChangeIndexesQsts (Surveys.Svy.SvyCod,SvyQst.QstInd);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Question_removed);

   /***** Show current survey *****/
   Svy_ShowOneSurvey (&Surveys,true);
  }

/*****************************************************************************/
/************************ Receive answers of a survey ************************/
/*****************************************************************************/

void Svy_ReceiveSurveyAnswers (void)
  {
   extern const char *Txt_You_already_answered_this_survey_before;
   extern const char *Txt_Thanks_for_answering_the_survey;
   struct Svy_Surveys Surveys;

   /***** Reset surveys *****/
   Svy_ResetSurveys (&Surveys);

   /***** Get survey code *****/
   Surveys.Svy.SvyCod = ParCod_GetAndCheckPar (ParCod_Svy);

   /***** Get data of the survey from database *****/
   Svy_GetSurveyDataByCod (&Surveys.Svy);

   /***** Check if I have no answered this survey formerly *****/
   if (Surveys.Svy.Status.IHaveAnswered)
      Ale_ShowAlert (Ale_WARNING,Txt_You_already_answered_this_survey_before);
   else
     {
      /***** Receive and store user's answers *****/
      Svy_ReceiveAndStoreUserAnswersToASurvey (Surveys.Svy.SvyCod);
      Ale_ShowAlert (Ale_SUCCESS,Txt_Thanks_for_answering_the_survey);
     }

   /***** Show current survey *****/
   Svy_ShowOneSurvey (&Surveys,true);
  }

/*****************************************************************************/
/**************** Get and store user's answers to a survey *******************/
/*****************************************************************************/

static void Svy_ReceiveAndStoreUserAnswersToASurvey (long SvyCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumQst;
   unsigned NumQsts;
   long QstCod;
   char ParName[3 + 10 + 6 + 1];
   char StrAnswersIndexes[Svy_MAX_ANSWERS_PER_QUESTION * (Cns_MAX_DECIMAL_DIGITS_UINT + 1)];
   const char *Ptr;
   char UnsignedStr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   unsigned AnsInd;
   char Comments[Cns_MAX_BYTES_TEXT + 1];

   /***** Get questions of this survey from database *****/
   if ((NumQsts = Svy_DB_GetSurveyQstsCodes (&mysql_res,SvyCod)))
     {
      // This survey has questions
      /***** Get questions *****/
      for (NumQst = 0;
	   NumQst < NumQsts;
	   NumQst++)
        {
         /* Get next question */
         if ((QstCod = DB_GetNextCode (mysql_res)) <= 0)
            Err_WrongQuestionExit ();

         /* Get possible parameter with the user's answer */
         snprintf (ParName,sizeof (ParName),"Ans%010u",(unsigned) QstCod);
         Par_GetParMultiToText (ParName,StrAnswersIndexes,
                                Svy_MAX_ANSWERS_PER_QUESTION * (Cns_MAX_DECIMAL_DIGITS_UINT + 1));
         Ptr = StrAnswersIndexes;
         while (*Ptr)
           {
            Par_GetNextStrUntilSeparParMult (&Ptr,UnsignedStr,Cns_MAX_DECIMAL_DIGITS_UINT);
            if (sscanf (UnsignedStr,"%u",&AnsInd) == 1)
               // Parameter exists ==> user has checked this answer
               // 		   ==> store it in database
               Svy_DB_IncreaseAnswer (QstCod,AnsInd);
           }

         /* Get possible parameter with the user's comment */
         snprintf (ParName,sizeof (ParName),"Com%010u",(unsigned) QstCod);
         Par_GetParAndChangeFormat (ParName,Comments,Cns_MAX_BYTES_TEXT,
                                    Str_TO_RIGOROUS_HTML,Str_REMOVE_SPACES);
         if (Comments[0])
            Svy_DB_CreateComments (QstCod,Comments);
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Register that you have answered this survey *****/
   Svy_DB_RegisterIHaveAnsweredSvy (SvyCod);
  }

/*****************************************************************************/
/******************** Get number of surveys for courses **********************/
/*****************************************************************************/
// Returns the number of surveys for courses
// in this location (all the platform, current degree or current course)

unsigned Svy_GetNumCrsSurveys (Hie_Level_t Level,unsigned *NumNotif)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumSurveys;

   /***** Get number of surveys from database *****/
   if (Svy_DB_GetNumCrsSurveys (&mysql_res,Level))
     {
      /***** Get number of surveys *****/
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%u",&NumSurveys) != 1)
	 Err_ShowErrorAndExit ("Error when getting number of surveys.");

      /***** Get number of notifications by email *****/
      if (row[1])
	{
	 if (sscanf (row[1],"%u",NumNotif) != 1)
	    Err_ShowErrorAndExit ("Error when getting number of notifications of surveys.");
	}
      else
	 *NumNotif = 0;
     }
   else
      NumSurveys = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumSurveys;
  }

/*****************************************************************************/
/***************************** Show stats of surveys *************************/
/*****************************************************************************/

void Svy_GetAndShowSurveysStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_surveys;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Number_of_BR_surveys;
   extern const char *Txt_Number_of_BR_courses_with_BR_surveys;
   extern const char *Txt_Average_number_BR_of_surveys_BR_per_course;
   extern const char *Txt_Average_number_BR_of_questions_BR_per_survey;
   extern const char *Txt_Number_of_BR_notifications;
   unsigned NumSurveys;
   unsigned NumNotif;
   unsigned NumCoursesWithSurveys;
   double NumQstsPerSurvey;

   /***** Get the number of surveys and the average number of questions per survey from this location *****/
   if ((NumSurveys = Svy_GetNumCrsSurveys (Gbl.Scope.Current,&NumNotif)))
     {
      NumCoursesWithSurveys = Svy_DB_GetNumCrssWithCrsSurveys (Gbl.Scope.Current);
      NumQstsPerSurvey = Svy_DB_GetNumQstsPerCrsSurvey (Gbl.Scope.Current);
     }
   else
     {
      NumCoursesWithSurveys = 0;
      NumQstsPerSurvey = 0.0;
     }

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_FIGURE_TYPES[Fig_SURVEYS],
                      NULL,NULL,
                      Hlp_ANALYTICS_Figures_surveys,Box_NOT_CLOSABLE,2);

      /***** Write table heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Number_of_BR_surveys                        ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_BR_courses_with_BR_surveys        ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Average_number_BR_of_surveys_BR_per_course  ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Average_number_BR_of_questions_BR_per_survey,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_BR_notifications                  ,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Write number of surveys *****/
      HTM_TR_Begin (NULL);
	 HTM_TD_Unsigned (NumSurveys);
	 HTM_TD_Unsigned (NumCoursesWithSurveys);
	 HTM_TD_Ratio (NumSurveys,NumCoursesWithSurveys);
	 HTM_TD_Double2Decimals (NumQstsPerSurvey);
	 HTM_TD_Unsigned (NumNotif);
      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
