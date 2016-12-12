// swad_survey.c: surveys

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_notification.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_survey.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Svy_MAX_LENGTH_ANSWER		1000
#define Svy_MAX_BYTES_ANSWER		(Svy_MAX_LENGTH_ANSWER*Str_MAX_CHARACTER)
#define Svy_MAX_BYTES_LIST_ANSWER_TYPES		(10+(Svy_NUM_ANS_TYPES-1)*(1+10))

typedef enum
  {
   Svy_ANS_UNIQUE_CHOICE   = 0,
   Svy_ANS_MULTIPLE_CHOICE = 1,
  } Svy_AnswerType_t;
const char *Svy_StrAnswerTypesDB[Svy_NUM_ANS_TYPES] =
  {
   "unique_choice",
   "multiple_choice",
  };

#define Svy_MAX_ANSWERS_PER_QUESTION	10

struct SurveyQuestion
  {
   long QstCod;
   unsigned QstInd;
   Svy_AnswerType_t AnswerType;
   struct
     {
      char *Text;
     } AnsChoice[Svy_MAX_ANSWERS_PER_QUESTION];
   bool AllAnsTypes;
   char ListAnsTypes[Svy_MAX_BYTES_LIST_ANSWER_TYPES+1];
  };

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Svy_ListAllSurveys (struct SurveyQuestion *SvyQst);
static bool Svy_CheckIfICanCreateSvy (void);
static void Svy_PutIconsListSurveys (void);
static void Svy_PutIconToCreateNewSvy (void);
static void Svy_PutButtonToCreateNewSvy (void);
static void Svy_PutParamsToCreateNewSvy (void);
static void Svy_PutFormToSelectWhichGroupsToShow (void);
static void Svy_ParamsWhichGroupsToShow (void);
static void Svy_ShowOneSurvey (long SvyCod,struct SurveyQuestion *SvyQst,
                               bool ShowOnlyThisSvyComplete);
static void Svy_WriteAuthor (struct Survey *Svy);
static void Svy_WriteStatus (struct Survey *Svy);
static void Svy_GetParamSvyOrderType (void);

static void Svy_PutFormsToRemEditOneSvy (long SvyCod,bool Visible);
static void Svy_PutParams (void);

static void Svy_SetAllowedAndHiddenScopes (unsigned *ScopesAllowed,
                                           unsigned *HiddenAllowed);

static void Svy_GetSurveyTxtFromDB (long SvyCod,char *Txt);
static void Svy_PutParamSvyCod (long SvyCod);
static long Svy_GetParamSvyCod (void);

static void Svy_PutButtonToResetSurvey (void);

static bool Svy_CheckIfSimilarSurveyExists (struct Survey *Svy);
static void Svy_SetDefaultAndAllowedScope (struct Survey *Svy);
static void Svy_ShowLstGrpsToEditSurvey (long SvyCod);
static void Svy_UpdateNumUsrsNotifiedByEMailAboutSurvey (long SvyCod,
                                                         unsigned NumUsrsToBeNotifiedByEMail);
static void Svy_CreateSurvey (struct Survey *Svy,const char *Txt);
static void Svy_UpdateSurvey (struct Survey *Svy,const char *Txt);
static bool Svy_CheckIfSvyIsAssociatedToGrps (long SvyCod);
static void Svy_RemoveAllTheGrpsAssociatedToAndSurvey (long SvyCod);
static void Svy_CreateGrps (long SvyCod);
static void Svy_GetAndWriteNamesOfGrpsAssociatedToSvy (struct Survey *Svy);
static bool Svy_CheckIfICanDoThisSurveyBasedOnGrps (long SvyCod);

static unsigned Svy_GetNumQstsSvy (long SvyCod);
static void Svy_ShowFormEditOneQst (long SvyCod,struct SurveyQuestion *SvyQst,char *Txt);
static void Svy_InitQst (struct SurveyQuestion *SvyQst);
static void Svy_PutParamQstCod (long QstCod);
static long Svy_GetParamQstCod (void);
static void Svy_RemAnswersOfAQuestion (long QstCod);
static Svy_AnswerType_t Svy_ConvertFromStrAnsTypDBToAnsTyp (const char *StrAnsTypeBD);
static Svy_AnswerType_t Svy_ConvertFromUnsignedStrToAnsTyp (const char *UnsignedStr);
static bool Svy_CheckIfAnswerExists (long QstCod,unsigned AnsInd);
static unsigned Svy_GetAnswersQst (long QstCod,MYSQL_RES **mysql_res);
static int Svy_AllocateTextChoiceAnswer (struct SurveyQuestion *SvyQst,unsigned NumAns);
static void Svy_FreeTextChoiceAnswers (struct SurveyQuestion *SvyQst,unsigned NumAnswers);
static void Svy_FreeTextChoiceAnswer (struct SurveyQuestion *SvyQst,unsigned NumAns);

static unsigned Svy_GetQstIndFromQstCod (long QstCod);
static unsigned Svy_GetNextQuestionIndexInSvy (long SvyCod);
static void Svy_ListSvyQuestions (struct Survey *Svy,struct SurveyQuestion *SvyQst);
static void Svy_PutIconToAddNewQuestion (void);
static void Svy_PutButtonToCreateNewQuestion (void);
static void Svy_WriteQstStem (const char *Stem);
static void Svy_WriteAnswersOfAQst (struct Survey *Svy,struct SurveyQuestion *SvyQst,bool PutFormAnswerSurvey);
static void Svy_DrawBarNumUsrs (unsigned NumUsrs,unsigned MaxUsrs);

static void Svy_PutIconToRemoveOneQst (void);
static void Svy_PutParamsRemoveOneQst (void);

static void Svy_ReceiveAndStoreUserAnswersToASurvey (long SvyCod);
static void Svy_IncreaseAnswerInDB (long QstCod,unsigned AnsInd);
static void Svy_RegisterIHaveAnsweredSvy (long SvyCod);
static bool Svy_CheckIfIHaveAnsweredSvy (long SvyCod);
static unsigned Svy_GetNumUsrsWhoHaveAnsweredSvy (long SvyCod);

/*****************************************************************************/
/************************** List all the surveys *****************************/
/*****************************************************************************/

void Svy_SeeAllSurveys (void)
  {
   struct SurveyQuestion SvyQst;

   /***** Get parameters *****/
   Svy_GetParamSvyOrderType ();
   Grp_GetParamWhichGrps ();
   Pag_GetParamPagNum (Pag_SURVEYS);

   /***** Show all the surveys *****/
   Svy_ListAllSurveys (&SvyQst);
  }

/*****************************************************************************/
/*************************** Show all the surveys ****************************/
/*****************************************************************************/

static void Svy_ListAllSurveys (struct SurveyQuestion *SvyQst)
  {
   extern const char *Hlp_STATS_Surveys;
   extern const char *Txt_Surveys;
   extern const char *Txt_ASG_ATT_SVY_OR_AGD_HELP_ORDER[2];
   extern const char *Txt_ASG_ATT_SVY_OR_AGD_ORDER[2];
   extern const char *Txt_Survey;
   extern const char *Txt_Status;
   extern const char *Txt_No_surveys;
   tSvysOrderType Order;
   struct Pagination Pagination;
   unsigned NumSvy;

   /***** Get number of groups in current course *****/
   if (!Gbl.CurrentCrs.Grps.NumGrps)
      Gbl.CurrentCrs.Grps.WhichGrps = Grp_ALL_GROUPS;

   /***** Get list of surveys *****/
   Svy_GetListSurveys ();

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Gbl.Svys.Num;
   Pagination.CurrentPage = (int) Gbl.Pag.CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Gbl.Pag.CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Write links to pages *****/
   if (Pagination.MoreThanOnePage)
      Pag_WriteLinksToPagesCentered (Pag_SURVEYS,0,&Pagination);

   /***** Start frame *****/
   Lay_StartRoundFrame ("100%",Txt_Surveys,
                        Svy_PutIconsListSurveys,Hlp_STATS_Surveys);

   /***** Select whether show only my groups or all groups *****/
   if (Gbl.CurrentCrs.Grps.NumGrps)
      Svy_PutFormToSelectWhichGroupsToShow ();

   if (Gbl.Svys.Num)
     {
      /***** Table head *****/
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_MARGIN CELLS_PAD_2\">"
			 "<tr>");
      for (Order = Svy_ORDER_BY_START_DATE;
	   Order <= Svy_ORDER_BY_END_DATE;
	   Order++)
	{
	 fprintf (Gbl.F.Out,"<th class=\"CENTER_MIDDLE\">");

	 /* Form to change order */
	 Act_FormStart (ActSeeAllSvy);
	 Grp_PutParamWhichGrps ();
	 Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Act_LinkFormSubmit (Txt_ASG_ATT_SVY_OR_AGD_HELP_ORDER[Order],"TIT_TBL",NULL);
	 if (Order == Gbl.Svys.SelectedOrderType)
	    fprintf (Gbl.F.Out,"<u>");
	 fprintf (Gbl.F.Out,"%s",Txt_ASG_ATT_SVY_OR_AGD_ORDER[Order]);
	 if (Order == Gbl.Svys.SelectedOrderType)
	    fprintf (Gbl.F.Out,"</u>");
	 fprintf (Gbl.F.Out,"</a>");
	 Act_FormEnd ();

	 fprintf (Gbl.F.Out,"</th>");
	}
      fprintf (Gbl.F.Out,"<th class=\"CENTER_MIDDLE\">"
			 "%s"
			 "</th>"
			 "<th class=\"CENTER_MIDDLE\">"
			 "%s"
			 "</th>"
			 "</tr>",
	       Txt_Survey,
	       Txt_Status);

      /***** Write all the surveys *****/
      for (NumSvy = Pagination.FirstItemVisible;
	   NumSvy <= Pagination.LastItemVisible;
	   NumSvy++)
	 Svy_ShowOneSurvey (Gbl.Svys.LstSvyCods[NumSvy - 1],SvyQst,false);

      /***** End table *****/
      fprintf (Gbl.F.Out,"</table>");
     }
   else	// No surveys created
      Lay_ShowAlert (Lay_INFO,Txt_No_surveys);

   /***** Button to create a new survey *****/
   if (Svy_CheckIfICanCreateSvy ())
      Svy_PutButtonToCreateNewSvy ();

   /***** End frame *****/
   Lay_EndRoundFrame ();

   /***** Write again links to pages *****/
   if (Pagination.MoreThanOnePage)
      Pag_WriteLinksToPagesCentered (Pag_SURVEYS,0,&Pagination);

   /***** Free list of surveys *****/
   Svy_FreeListSurveys ();
  }

/*****************************************************************************/
/******************* Check if I can create a new survey **********************/
/*****************************************************************************/

static bool Svy_CheckIfICanCreateSvy (void)
  {
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_TEACHER:
         return (Gbl.CurrentCrs.Crs.CrsCod > 0);
      case Rol_DEG_ADM:
         return (Gbl.CurrentDeg.Deg.DegCod > 0);	// Always true
      case Rol_CTR_ADM:
         return (Gbl.CurrentCtr.Ctr.CtrCod > 0);	// Always true
      case Rol_INS_ADM:
         return (Gbl.CurrentIns.Ins.InsCod > 0);	// Always true
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
   return false;
  }

/*****************************************************************************/
/***************** Put contextual icons in list of surveys *******************/
/*****************************************************************************/

static void Svy_PutIconsListSurveys (void)
  {
   /***** Put icon to create a new survey *****/
   if (Svy_CheckIfICanCreateSvy ())
      Svy_PutIconToCreateNewSvy ();

   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_SURVEYS;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/********************** Put icon to create a new survey **********************/
/*****************************************************************************/

static void Svy_PutIconToCreateNewSvy (void)
  {
   extern const char *Txt_New_survey;

   Lay_PutContextualLink (ActFrmNewSvy,Svy_PutParamsToCreateNewSvy,
                          "plus64x64.png",
                          Txt_New_survey,NULL,
		          NULL);
  }

/*****************************************************************************/
/********************* Put button to create a new survey *********************/
/*****************************************************************************/

static void Svy_PutButtonToCreateNewSvy (void)
  {
   extern const char *Txt_New_survey;

   Act_FormStart (ActFrmNewSvy);
   Svy_PutParamsToCreateNewSvy ();
   Lay_PutConfirmButton (Txt_New_survey);
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************* Put parameters to create a new survey *******************/
/*****************************************************************************/

static void Svy_PutParamsToCreateNewSvy (void)
  {
   Svy_PutHiddenParamSvyOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
  }

/*****************************************************************************/
/***************** Put form to select which groups to show *******************/
/*****************************************************************************/

static void Svy_PutFormToSelectWhichGroupsToShow (void)
  {
   fprintf (Gbl.F.Out,"<div style=\"display:table; margin:0 auto;\">");
   Grp_ShowFormToSelWhichGrps (ActSeeAllSvy,Svy_ParamsWhichGroupsToShow);
   fprintf (Gbl.F.Out,"</div>");
  }

static void Svy_ParamsWhichGroupsToShow (void)
  {
   Svy_PutHiddenParamSvyOrderType ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
  }

/*****************************************************************************/
/****************************** Show one survey ******************************/
/*****************************************************************************/

void Svy_SeeOneSurvey (void)
  {
   struct Survey Svy;
   struct SurveyQuestion SvyQst;

   /***** Get parameters *****/
   Svy_GetParamSvyOrderType ();
   Grp_GetParamWhichGrps ();
   Pag_GetParamPagNum (Pag_SURVEYS);

   /***** Get survey code *****/
   if ((Svy.SvyCod = Svy_GetParamSvyCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of survey is missing.");

   /***** Show survey *****/
   Svy_ShowOneSurvey (Svy.SvyCod,&SvyQst,true);
  }

/*****************************************************************************/
/****************************** Show one survey ******************************/
/*****************************************************************************/

static void Svy_ShowOneSurvey (long SvyCod,struct SurveyQuestion *SvyQst,
                               bool ShowOnlyThisSvyComplete)
  {
   extern const char *Hlp_STATS_Surveys;
   extern const char *Txt_Survey;
   extern const char *Txt_Today;
   extern const char *Txt_View_survey;
   extern const char *Txt_No_of_questions;
   extern const char *Txt_No_of_users;
   extern const char *Txt_Scope;
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;
   extern const char *Txt_Users;
   extern const char *Txt_Answer_survey;
   extern const char *Txt_View_survey_results;
   static unsigned UniqueId = 0;
   struct Survey Svy;
   char Txt[Cns_MAX_BYTES_TEXT+1];

   /***** Start frame *****/
   if (ShowOnlyThisSvyComplete)
      Lay_StartRoundFrame (NULL,Txt_Survey,NULL,Hlp_STATS_Surveys);

   /***** Get data of this survey *****/
   Svy.SvyCod = SvyCod;
   Svy_GetDataOfSurveyByCod (&Svy);

   /***** Start table *****/
   if (ShowOnlyThisSvyComplete)
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL CELLS_PAD_2\">");

   /***** Start date/time *****/
   UniqueId++;
   fprintf (Gbl.F.Out,"<tr>"
                      "<td id=\"svy_date_start_%u\" class=\"%s LEFT_BOTTOM",
	    UniqueId,
            Svy.Status.Visible ? (Svy.Status.Open ? "DATE_GREEN" :
        	                                    "DATE_RED") :
                                 (Svy.Status.Open ? "DATE_GREEN_LIGHT" :
                                                    "DATE_RED_LIGHT"));
   if (!ShowOnlyThisSvyComplete)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">"
                      "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('svy_date_start_%u',"
                      "%ld,'<br />','%s',true,true,true);"
                      "</script>"
	              "</td>",
            UniqueId,Svy.TimeUTC[Svy_START_TIME],Txt_Today);

   /***** End date/time *****/
   fprintf (Gbl.F.Out,"<td id=\"svy_date_end_%u\" class=\"%s LEFT_BOTTOM",
            UniqueId,
            Svy.Status.Visible ? (Svy.Status.Open ? "DATE_GREEN" :
        	                                    "DATE_RED") :
                                 (Svy.Status.Open ? "DATE_GREEN_LIGHT" :
                                                    "DATE_RED_LIGHT"));
   if (!ShowOnlyThisSvyComplete)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">"
                      "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('svy_date_end_%u',"
                      "%ld,'<br />','%s',false,true,true);"
                      "</script>"
	              "</td>",
            UniqueId,Svy.TimeUTC[Svy_END_TIME],Txt_Today);

   /***** Survey title *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP");
   if (!ShowOnlyThisSvyComplete)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");

   /* Put form to view survey */
   Act_FormStart (ActSeeOneSvy);
   Svy_PutParamSvyCod (SvyCod);
   Svy_PutHiddenParamSvyOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   Act_LinkFormSubmit (Txt_View_survey,
                       Svy.Status.Visible ? "ASG_TITLE" :
	                                    "ASG_TITLE_LIGHT",NULL);
   fprintf (Gbl.F.Out,"%s</a>",
            Svy.Title);
   Act_FormEnd ();

   /* Number of questions and number of distinct users who have already answered this survey */
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s: %u; %s: %u</div>"
	              "</td>",
            Svy.Status.Visible ? "ASG_GRP" :
        	                 "ASG_GRP_LIGHT",
            Txt_No_of_questions,
            Svy.NumQsts,
            Txt_No_of_users,
            Svy.NumUsrs);

   /***** Status of the survey *****/
   fprintf (Gbl.F.Out,"<td rowspan=\"2\" class=\"LEFT_TOP");
   if (!ShowOnlyThisSvyComplete)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");
   Svy_WriteStatus (&Svy);

   if (!ShowOnlyThisSvyComplete)
     {
      /* Possible button to answer this survey */
      if (Svy.Status.ICanAnswer)
	{
	 fprintf (Gbl.F.Out,"<div class=\"BUTTONS_AFTER_ALERT\">");

	 Act_FormStart (ActSeeOneSvy);
	 Svy_PutParamSvyCod (Svy.SvyCod);
	 Svy_PutHiddenParamSvyOrderType ();
	 Grp_PutParamWhichGrps ();
	 Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
	 Lay_PutCreateButtonInline (Txt_Answer_survey);
	 Act_FormEnd ();

	 fprintf (Gbl.F.Out,"</div>");
	}
      /* Possible button to see the result of the survey */
      else if (Svy.Status.ICanViewResults)
	{
	 fprintf (Gbl.F.Out,"<div class=\"BUTTONS_AFTER_ALERT\">");

	 Act_FormStart (ActSeeOneSvy);
	 Svy_PutParamSvyCod (Svy.SvyCod);
	 Svy_PutHiddenParamSvyOrderType ();
	 Grp_PutParamWhichGrps ();
	 Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
	 Lay_PutConfirmButtonInline (Txt_View_survey_results);
	 Act_FormEnd ();

	 fprintf (Gbl.F.Out,"</div>");
	}
     }

   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Write second row of data of this survey *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" class=\"LEFT_TOP");
   if (!ShowOnlyThisSvyComplete)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");

   /* Author of the survey */
   Svy_WriteAuthor (&Svy);

   /* Forms to remove/edit this survey */
   if (Svy.Status.ICanEdit)
      Svy_PutFormsToRemEditOneSvy (Svy.SvyCod,Svy.Status.Visible);

   fprintf (Gbl.F.Out,"</td>"
                      "<td class=\"LEFT_TOP");
   if (!ShowOnlyThisSvyComplete)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");

   /* Scope of the survey */
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s: ",
            Svy.Status.Visible ? "ASG_GRP" :
        	                 "ASG_GRP_LIGHT",
            Txt_Scope);
   switch (Svy.Scope)
     {
      case Sco_SCOPE_UNK:	// Unknown
         Lay_ShowErrorAndExit ("Wrong survey scope.");
         break;
      case Sco_SCOPE_SYS:	// System
         fprintf (Gbl.F.Out,"%s",
                  Cfg_PLATFORM_SHORT_NAME);
	 break;
      case Sco_SCOPE_CTY:	// Country
         fprintf (Gbl.F.Out,"%s %s",
                  Txt_Country,Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);
	 break;
      case Sco_SCOPE_INS:	// Institution
         fprintf (Gbl.F.Out,"%s %s",
                  Txt_Institution,Gbl.CurrentIns.Ins.ShrtName);
	 break;
      case Sco_SCOPE_CTR:	// Centre
         fprintf (Gbl.F.Out,"%s %s",
                  Txt_Centre,Gbl.CurrentCtr.Ctr.ShrtName);
	 break;
      case Sco_SCOPE_DEG:	// Degree
         fprintf (Gbl.F.Out,"%s %s",
                  Txt_Degree,Gbl.CurrentDeg.Deg.ShrtName);
 	 break;
      case Sco_SCOPE_CRS:	// Course
	 fprintf (Gbl.F.Out,"%s %s",
	          Txt_Course,Gbl.CurrentCrs.Crs.ShrtName);
	 break;
     }
   fprintf (Gbl.F.Out,"</div>");

   /* Users' roles who can answer the survey */
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s:<br />",
            Svy.Status.Visible ? "ASG_GRP" :
        	                 "ASG_GRP_LIGHT",
            Txt_Users);
   Rol_WriteSelectorRoles (1 << Rol_STUDENT |
			   1 << Rol_TEACHER,
			   Svy.Roles,
			   true,false);
   fprintf (Gbl.F.Out,"</div>");

   /* Groups whose users can answer this survey */
   if (Svy.Scope == Sco_SCOPE_CRS)
      if (Gbl.CurrentCrs.Grps.NumGrps)
         Svy_GetAndWriteNamesOfGrpsAssociatedToSvy (&Svy);

   /* Text of the survey */
   Svy_GetSurveyTxtFromDB (Svy.SvyCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
   Str_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   fprintf (Gbl.F.Out,"<p class=\"%s\">"
                      "%s"
                      "</p>"
                      "</td>"
                      "</tr>",
            Svy.Status.Visible ? "DAT" :
        	                 "DAT_LIGHT",
            Txt);

   /***** Write questions of this survey *****/
   if (ShowOnlyThisSvyComplete)
     {
      fprintf (Gbl.F.Out,"<tr>"
			 "<td colspan=\"4\">");
      Svy_ListSvyQuestions (&Svy,SvyQst);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;

   /***** Mark possible notification as seen *****/
   if (Svy.Scope == Sco_SCOPE_CRS)	// Only course surveys are notified
      Ntf_MarkNotifAsSeen (Ntf_EVENT_SURVEY,
	                   SvyCod,Svy.Cod,
	                   Gbl.Usrs.Me.UsrDat.UsrCod);

   if (ShowOnlyThisSvyComplete)
     {
      /***** End table *****/
      fprintf (Gbl.F.Out,"</table>");

      /***** End frame *****/
      Lay_EndRoundFrame ();
     }
  }

/*****************************************************************************/
/*********************** Write the author of a survey ************************/
/*****************************************************************************/

static void Svy_WriteAuthor (struct Survey *Svy)
  {
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX+1];
   char FirstName[Usr_MAX_BYTES_NAME+1];
   char Surnames[2*(Usr_MAX_BYTES_NAME+1)];
   struct UsrData UsrDat;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Get data of author *****/
   UsrDat.UsrCod = Svy->UsrCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))	// Get of the database the data of the author
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);

   /***** Show photo *****/
   Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                	                 NULL,
                     "PHOTO15x20",Pho_ZOOM,false);

   /***** Write name *****/
   strcpy (FirstName,UsrDat.FirstName);
   strcpy (Surnames,UsrDat.Surname1);
   if (UsrDat.Surname2[0])
     {
      strcat (Surnames," ");
      strcat (Surnames,UsrDat.Surname2);
     }
   Str_LimitLengthHTMLStr (FirstName,8);
   Str_LimitLengthHTMLStr (Surnames,8);
   fprintf (Gbl.F.Out,"<span class=\"%s\">%s %s</span>",
            Svy->Status.Visible ? "MSG_AUT" :
        	                  "MSG_AUT_LIGHT",
            FirstName,Surnames);

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************************ Write status of a survey ***************************/
/*****************************************************************************/

static void Svy_WriteStatus (struct Survey *Svy)
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

   /***** Start list with items of status *****/
   fprintf (Gbl.F.Out,"<ul>");

   /* Write whether survey is visible or hidden */
   if (Svy->Status.Visible)
      fprintf (Gbl.F.Out,"<li class=\"STATUS_GREEN\">%s</li>",
               Txt_Visible_survey);
   else
      fprintf (Gbl.F.Out,"<li class=\"STATUS_RED_LIGHT\">%s</li>",
               Txt_Hidden_survey);

   /* Write whether survey is open or closed */
   if (Svy->Status.Open)
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Svy->Status.Visible ? "STATUS_GREEN" :
        	                     "STATUS_GREEN_LIGHT",
               Txt_Open_survey);
   else
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Svy->Status.Visible ? "STATUS_RED" :
        	                     "STATUS_RED_LIGHT",
               Txt_Closed_survey);

   /* Write whether survey can be answered by me or not depending on user type */
   if (Svy->Status.IAmLoggedWithAValidRoleToAnswer)
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Svy->Status.Visible ? "STATUS_GREEN" :
        	                     "STATUS_GREEN_LIGHT",
               Txt_SURVEY_Type_of_user_allowed);
   else
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Svy->Status.Visible ? "STATUS_RED" :
        	                     "STATUS_RED_LIGHT",
               Txt_SURVEY_Type_of_user_not_allowed);

   /* Write whether survey can be answered by me or not depending on groups */
   if (Svy->Status.IBelongToScope)
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Svy->Status.Visible ? "STATUS_GREEN" :
        	                     "STATUS_GREEN_LIGHT",
               Txt_SURVEY_You_belong_to_the_scope_of_the_survey);
   else
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Svy->Status.Visible ? "STATUS_RED" :
        	                     "STATUS_RED_LIGHT",
               Txt_SURVEY_You_dont_belong_to_the_scope_of_the_survey);

   /* Write whether survey has been already answered by me or not */
   if (Svy->Status.IHaveAnswered)
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Svy->Status.Visible ? "STATUS_GREEN" :
        	                     "STATUS_GREEN_LIGHT",
               Txt_SURVEY_You_have_already_answered);
   else
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Svy->Status.Visible ? "STATUS_RED" :
        	                     "STATUS_RED_LIGHT",
               Txt_SURVEY_You_have_not_answered);

   /***** End list with items of status *****/
   fprintf (Gbl.F.Out,"</ul>");
  }

/*****************************************************************************/
/********* Get parameter with the type or order in list of surveys ***********/
/*****************************************************************************/

static void Svy_GetParamSvyOrderType (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   /***** Set default order type *****/
   Gbl.Svys.SelectedOrderType = Svy_ORDER_BY_START_DATE;

   /***** Get parameter from form with the order type *****/
   Par_GetParToText ("Order",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Svy_NUM_ORDERS)
         Gbl.Svys.SelectedOrderType = (tSvysOrderType) UnsignedNum;
  }

/*****************************************************************************/
/***** Put a hidden parameter with the type of order in list of surveys ******/
/*****************************************************************************/

void Svy_PutHiddenParamSvyOrderType (void)
  {
   Par_PutHiddenParamUnsigned ("Order",(unsigned) Gbl.Svys.SelectedOrderType);
  }

/*****************************************************************************/
/******************* Put a link (form) to edit one survey ********************/
/*****************************************************************************/

static void Svy_PutFormsToRemEditOneSvy (long SvyCod,bool Visible)
  {
   extern const char *Txt_Remove;
   extern const char *Txt_Reset;
   extern const char *Txt_Show;
   extern const char *Txt_Hide;
   extern const char *Txt_Edit;

   fprintf (Gbl.F.Out,"<div>");

   Gbl.Svys.SvyCodToEdit = SvyCod;	// Used as parameters in contextual links

   /***** Put form to remove survey *****/
   Lay_PutContextualLink (ActReqRemSvy,Svy_PutParams,
                          "remove-on64x64.png",
                          Txt_Remove,NULL,
		          NULL);

   /***** Put form to reset survey *****/
   Lay_PutContextualLink (ActReqRstSvy,Svy_PutParams,
                          "recycle64x64.png",
                          Txt_Reset,NULL,
		          NULL);

   /***** Put form to hide/show survey *****/
   if (Visible)
      Lay_PutContextualLink (ActHidSvy,Svy_PutParams,
                             "eye-on64x64.png",
			     Txt_Hide,NULL,
		             NULL);
   else
      Lay_PutContextualLink (ActShoSvy,Svy_PutParams,
                             "eye-slash-on64x64.png",
			     Txt_Show,NULL,
		             NULL);

   /***** Put form to edit survey *****/
   Lay_PutContextualLink (ActEdiOneSvy,Svy_PutParams,
                          "edit64x64.png",
                          Txt_Edit,NULL,
		          NULL);

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********************** Params used to edit a survey *************************/
/*****************************************************************************/

static void Svy_PutParams (void)
  {
   if (Gbl.Svys.SvyCodToEdit > 0)
      Svy_PutParamSvyCod (Gbl.Svys.SvyCodToEdit);
   Att_PutHiddenParamAttOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
  }

/*****************************************************************************/
/*********************** Get list of all the surveys *************************/
/*****************************************************************************/

void Svy_GetListSurveys (void)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   char SubQuery[Sco_NUM_SCOPES][256];
   char OrderBySubQuery[256];
   char Query[2048];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumSvy;
   unsigned ScopesAllowed = 0;
   unsigned HiddenAllowed = 0;
   long Cods[Sco_NUM_SCOPES];
   Sco_Scope_t Scope;
   bool SubQueryFilled;

   /***** Free list of surveys *****/
   if (Gbl.Svys.LstIsRead)
      Svy_FreeListSurveys ();

   /***** Set allowed and hidden scopes to get list depending on my user's role *****/
   Svy_SetAllowedAndHiddenScopes (&ScopesAllowed,&HiddenAllowed);

   /***** Get list of surveys from database *****/
   Cods[Sco_SCOPE_SYS] = -1L;				// System
   Cods[Sco_SCOPE_CTY] = Gbl.CurrentCty.Cty.CtyCod;	// Country
   Cods[Sco_SCOPE_INS] = Gbl.CurrentIns.Ins.InsCod;	// Institution
   Cods[Sco_SCOPE_CTR] = Gbl.CurrentCtr.Ctr.CtrCod;	// Centre
   Cods[Sco_SCOPE_DEG] = Gbl.CurrentDeg.Deg.DegCod;	// Degree
   Cods[Sco_SCOPE_CRS] = Gbl.CurrentCrs.Crs.CrsCod;	// Course

   /* Fill subqueries for system, country, institution, centre and degree */
   for (Scope = Sco_SCOPE_SYS, SubQueryFilled = false;
	Scope <= Sco_SCOPE_DEG;
	Scope++)
      if (ScopesAllowed & 1 << Scope)
	{
	 sprintf (SubQuery[Scope],"%s(Scope='%s' AND Cod='%ld'%s)",
	          SubQueryFilled ? " OR " :
	        	           "",
		  Sco_ScopeDB[Scope],Cods[Scope],
		  (HiddenAllowed & 1 << Scope) ? "" :
						 " AND Hidden='N'");
	 SubQueryFilled = true;
	}
      else
	 SubQuery[Scope][0] = '\0';

   /* Fill subquery for course */
   if (ScopesAllowed & 1 << Sco_SCOPE_CRS)
     {
      if (Gbl.CurrentCrs.Grps.WhichGrps == Grp_ONLY_MY_GROUPS)
	 sprintf (SubQuery[Sco_SCOPE_CRS],"%s("
	                                  "Scope='%s' AND Cod='%ld'%s"
	                                  " AND "
	                                  "(SvyCod NOT IN"
	                                  " (SELECT SvyCod FROM svy_grp)"
	                                  " OR"
                                          " SvyCod IN"
                                          " (SELECT svy_grp.SvyCod"
                                          " FROM svy_grp,crs_grp_usr"
                                          " WHERE crs_grp_usr.UsrCod='%ld'"
                                          " AND svy_grp.GrpCod=crs_grp_usr.GrpCod))"
	                                  ")",
	          SubQueryFilled ? " OR " :
	        	           "",
		  Sco_ScopeDB[Sco_SCOPE_CRS],Cods[Sco_SCOPE_CRS],
		  (HiddenAllowed & 1 << Sco_SCOPE_CRS) ? "" :
						         " AND Hidden='N'",
                  Gbl.Usrs.Me.UsrDat.UsrCod);
      else	// Gbl.CurrentCrs.Grps.WhichGrps == Grp_ALL_GROUPS
	 sprintf (SubQuery[Sco_SCOPE_CRS],"%s(Scope='%s' AND Cod='%ld'%s)",
	          SubQueryFilled ? " OR " :
	        	           "",
		  Sco_ScopeDB[Sco_SCOPE_CRS],Cods[Sco_SCOPE_CRS],
		  (HiddenAllowed & 1 << Sco_SCOPE_CRS) ? "" :
						         " AND Hidden='N'");
      SubQueryFilled = true;
     }
   else
      SubQuery[Sco_SCOPE_CRS][0] = '\0';

   /* Build query */
   if (SubQueryFilled)
     {
      switch (Gbl.Svys.SelectedOrderType)
	{
	 case Svy_ORDER_BY_START_DATE:
	    sprintf (OrderBySubQuery,"StartTime DESC,EndTime DESC,Title DESC");
	    break;
	 case Svy_ORDER_BY_END_DATE:
	    sprintf (OrderBySubQuery,"EndTime DESC,StartTime DESC,Title DESC");
	    break;
	}

      sprintf (Query,"SELECT SvyCod FROM surveys"
		     " WHERE %s%s%s%s%s%s"
		     " ORDER BY %s",
		  SubQuery[Sco_SCOPE_SYS],
		  SubQuery[Sco_SCOPE_CTY],
		  SubQuery[Sco_SCOPE_INS],
		  SubQuery[Sco_SCOPE_CTR],
		  SubQuery[Sco_SCOPE_DEG],
		  SubQuery[Sco_SCOPE_CRS],
		  OrderBySubQuery);
     }
   else
      Lay_ShowErrorAndExit ("Can not get list of surveys.");

   /* Make query */
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get surveys");

   if (NumRows) // Surveys found...
     {
      Gbl.Svys.Num = (unsigned) NumRows;

      /***** Create list of surveys *****/
      if ((Gbl.Svys.LstSvyCods = (long *) calloc (NumRows,sizeof (long))) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store list of surveys.");

      /***** Get the surveys codes *****/
      for (NumSvy = 0;
	   NumSvy < Gbl.Svys.Num;
	   NumSvy++)
        {
         /* Get next survey code */
         row = mysql_fetch_row (mysql_res);
         if ((Gbl.Svys.LstSvyCods[NumSvy] = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Error: wrong survey code.");
        }
     }
   else
      Gbl.Svys.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Gbl.Svys.LstIsRead = true;
  }

/*****************************************************************************/
/*** Set allowed and hidden scopes to get list depending on my user's role ***/
/*****************************************************************************/

static void Svy_SetAllowedAndHiddenScopes (unsigned *ScopesAllowed,
                                           unsigned *HiddenAllowed)
  {
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_UNKNOWN:	// User not logged in *********************************
	 *ScopesAllowed = 0;
	 *HiddenAllowed = 0;
         break;
      case Rol__GUEST_:	// User not belonging to any course *******************
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 0;
	 break;
      case Rol_VISITOR:	// Student or teacher in other courses...
   	   	   	// ...but not belonging to the current course *********
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 0;
	 if (Usr_CheckIfIBelongToCty (Gbl.CurrentCty.Cty.CtyCod))
	   {
	    *ScopesAllowed |= 1 << Sco_SCOPE_CTY;
	    if (Usr_CheckIfIBelongToIns (Gbl.CurrentIns.Ins.InsCod))
	      {
	       *ScopesAllowed |= 1 << Sco_SCOPE_INS;
	       if (Usr_CheckIfIBelongToCtr (Gbl.CurrentCtr.Ctr.CtrCod))
		 {
		  *ScopesAllowed |= 1 << Sco_SCOPE_CTR;
		  if (Usr_CheckIfIBelongToDeg (Gbl.CurrentDeg.Deg.DegCod))
		     *ScopesAllowed |= 1 << Sco_SCOPE_DEG;
		 }
	      }
	   }
         break;
      case Rol_STUDENT:	// Student in current course **************************
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 0;
	 if (Usr_CheckIfIBelongToCty (Gbl.CurrentCty.Cty.CtyCod))
	   {
	    *ScopesAllowed |= 1 << Sco_SCOPE_CTY;
	    if (Usr_CheckIfIBelongToIns (Gbl.CurrentIns.Ins.InsCod))
	      {
	       *ScopesAllowed |= 1 << Sco_SCOPE_INS;
	       if (Usr_CheckIfIBelongToCtr (Gbl.CurrentCtr.Ctr.CtrCod))
		 {
		  *ScopesAllowed |= 1 << Sco_SCOPE_CTR;
		  if (Usr_CheckIfIBelongToDeg (Gbl.CurrentDeg.Deg.DegCod))
		    {
		     *ScopesAllowed |= 1 << Sco_SCOPE_DEG;
		     if (Usr_CheckIfIBelongToCrs (Gbl.CurrentCrs.Crs.CrsCod))
			*ScopesAllowed |= 1 << Sco_SCOPE_CRS;
		    }
		 }
	      }
	   }
         break;
      case Rol_TEACHER:	// Teacher in current course **************************
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 0;
	 if (Usr_CheckIfIBelongToCty (Gbl.CurrentCty.Cty.CtyCod))
	   {
	    *ScopesAllowed |= 1 << Sco_SCOPE_CTY;
	    if (Usr_CheckIfIBelongToIns (Gbl.CurrentIns.Ins.InsCod))
	      {
	       *ScopesAllowed |= 1 << Sco_SCOPE_INS;
	       if (Usr_CheckIfIBelongToCtr (Gbl.CurrentCtr.Ctr.CtrCod))
		 {
		  *ScopesAllowed |= 1 << Sco_SCOPE_CTR;
		  if (Usr_CheckIfIBelongToDeg (Gbl.CurrentDeg.Deg.DegCod))
		    {
		     *ScopesAllowed |= 1 << Sco_SCOPE_DEG;
		     if (Usr_CheckIfIBelongToCrs (Gbl.CurrentCrs.Crs.CrsCod))
		       {
			*ScopesAllowed |= 1 << Sco_SCOPE_CRS;
			*HiddenAllowed |= 1 << Sco_SCOPE_CRS;	// A teacher can view hidden course surveys
		       }
		    }
		 }
	      }
	   }
         break;
      case Rol_DEG_ADM:	// Degree administrator *******************************
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 0;
	 if (Gbl.CurrentCty.Cty.CtyCod > 0)			// Country selected
	   {
	    *ScopesAllowed |= 1 << Sco_SCOPE_CTY;
	    if (Gbl.CurrentIns.Ins.InsCod > 0)			// Institution selected
	      {
	       *ScopesAllowed |= 1 << Sco_SCOPE_INS;
	       if (Gbl.CurrentCtr.Ctr.CtrCod > 0)		// Centre selected
		 {
		  *ScopesAllowed |= 1 << Sco_SCOPE_CTR;
		  if (Gbl.CurrentDeg.Deg.DegCod > 0)		// Degree selected
		    {
		     *ScopesAllowed |= 1 << Sco_SCOPE_DEG;
		     *HiddenAllowed |= 1 << Sco_SCOPE_DEG;	// A degree admin can view hidden degree surveys
		    }
	         }
	      }
	   }
	 break;
      case Rol_CTR_ADM:	// Centre administrator *******************************
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 0;
	 if (Gbl.CurrentCty.Cty.CtyCod > 0)			// Country selected
	   {
	    *ScopesAllowed |= 1 << Sco_SCOPE_CTY;
	    if (Gbl.CurrentIns.Ins.InsCod > 0)			// Institution selected
	      {
	       *ScopesAllowed |= 1 << Sco_SCOPE_INS;
	       if (Gbl.CurrentCtr.Ctr.CtrCod > 0)		// Centre selected
		 {
		  *ScopesAllowed |= 1 << Sco_SCOPE_CTR;
		  *HiddenAllowed |= 1 << Sco_SCOPE_CTR;		// A centre admin can view hidden centre surveys
		 }
	      }
	   }
	 break;
      case Rol_INS_ADM:	// Institution administrator **************************
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 0;
	 if (Gbl.CurrentCty.Cty.CtyCod > 0)			// Country selected
	   {
	    *ScopesAllowed |= 1 << Sco_SCOPE_CTY;
	    if (Gbl.CurrentIns.Ins.InsCod > 0)			// Institution selected
	      {
	       *ScopesAllowed |= 1 << Sco_SCOPE_INS;
	       *HiddenAllowed |= 1 << Sco_SCOPE_INS;		// An institution admin can view hidden institution surveys
	      }
	   }
	 break;
      case Rol_SYS_ADM:	// System administrator (superuser) *******************
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 1 << Sco_SCOPE_SYS;			// A system admin can view hidden system surveys
	 if (Gbl.CurrentCty.Cty.CtyCod > 0)			// Country selected
	   {
	    *ScopesAllowed |= 1 << Sco_SCOPE_CTY;
	    *HiddenAllowed |= 1 << Sco_SCOPE_CTY;		// A system admin can view hidden country surveys
	    if (Gbl.CurrentIns.Ins.InsCod > 0)			// Institution selected
	      {
	       *ScopesAllowed |= 1 << Sco_SCOPE_INS;
	       *HiddenAllowed |= 1 << Sco_SCOPE_INS;		// A system admin can view hidden institution surveys
	       if (Gbl.CurrentCtr.Ctr.CtrCod > 0)		// Centre selected
		 {
		  *ScopesAllowed |= 1 << Sco_SCOPE_CTR;
	          *HiddenAllowed |= 1 << Sco_SCOPE_CTR;		// A system admin can view hidden centre surveys
		  if (Gbl.CurrentDeg.Deg.DegCod > 0)		// Degree selected
		    {
		     *ScopesAllowed |= 1 << Sco_SCOPE_DEG;
	             *HiddenAllowed |= 1 << Sco_SCOPE_DEG;	// A system admin can view hidden degree surveys
		     if (Gbl.CurrentCrs.Crs.CrsCod > 0)		// Course selected
		       {
			*ScopesAllowed |= 1 << Sco_SCOPE_CRS;
	                *HiddenAllowed |= 1 << Sco_SCOPE_CRS;	// A system admin can view hidden course surveys
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

void Svy_GetDataOfSurveyByCod (struct Survey *Svy)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Build query *****/
   sprintf (Query,"SELECT SvyCod,Scope,Cod,Hidden,Roles,UsrCod,"
                  "UNIX_TIMESTAMP(StartTime),"
                  "UNIX_TIMESTAMP(EndTime),"
                  "NOW() BETWEEN StartTime AND EndTime,"
                  "Title"
                  " FROM surveys"
                  " WHERE SvyCod='%ld'",
            Svy->SvyCod);

   /***** Get data of survey from database *****/
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get survey data");

   if (NumRows) // Survey found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get code of the survey (row[0]) */
      Svy->SvyCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get survey scope (row[1]) */
      if ((Svy->Scope = Sco_GetScopeFromDBStr (row[1])) == Sco_SCOPE_UNK)
         Lay_ShowErrorAndExit ("Wrong survey scope.");

      /* Get code of the country, institution, centre, degree or course (row[2]) */
      Svy->Cod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get whether the survey is hidden (row[3]) */
      Svy->Status.Visible = (row[3][0] == 'N');

      /* Get roles (row[4]) */
      if (sscanf (row[4],"%u",&Svy->Roles) != 1)
      	 Lay_ShowErrorAndExit ("Error when reading roles of survey.");

      /* Get author of the survey (row[5]) */
      Svy->UsrCod = Str_ConvertStrCodToLongCod (row[5]);

      /* Get start date (row[6] holds the start UTC time) */
      Svy->TimeUTC[Att_START_TIME] = Dat_GetUNIXTimeFromStr (row[6]);

      /* Get end   date (row[7] holds the end   UTC time) */
      Svy->TimeUTC[Att_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[7]);

      /* Get whether the survey is open or closed (row(8)) */
      Svy->Status.Open = (row[8][0] == '1');

      /* Get the title of the survey (row[9]) */
      strcpy (Svy->Title,row[9]);

      /* Get number of questions and number of users who have already answer this survey */
      Svy->NumQsts = Svy_GetNumQstsSvy (Svy->SvyCod);
      Svy->NumUsrs = Svy_GetNumUsrsWhoHaveAnsweredSvy (Svy->SvyCod);

      /* Am I logged with a valid role to answer this survey? */
      Svy->Status.IAmLoggedWithAValidRoleToAnswer = (Svy->Roles & (1 << Gbl.Usrs.Me.LoggedRole));

      /* Do I belong to valid groups to answer this survey? */
      switch (Svy->Scope)
        {
	 case Sco_SCOPE_UNK:	// Unknown
            Lay_ShowErrorAndExit ("Wrong survey scope.");
	    break;
	 case Sco_SCOPE_SYS:	// System
            Svy->Status.IBelongToScope = Gbl.Usrs.Me.Logged;
	    break;
	 case Sco_SCOPE_CTY:	// Country
            Svy->Status.IBelongToScope = Usr_CheckIfIBelongToCty (Svy->Cod);
	    break;
	 case Sco_SCOPE_INS:	// Institution
            Svy->Status.IBelongToScope = Usr_CheckIfIBelongToIns (Svy->Cod);
	    break;
	 case Sco_SCOPE_CTR:	// Centre
            Svy->Status.IBelongToScope = Usr_CheckIfIBelongToCtr (Svy->Cod);
	    break;
	 case Sco_SCOPE_DEG:	// Degree
            Svy->Status.IBelongToScope = Usr_CheckIfIBelongToDeg (Svy->Cod);
	    break;
	 case Sco_SCOPE_CRS:	// Course
	    Svy->Status.IBelongToScope = Usr_CheckIfIBelongToCrs (Svy->Cod) &&
					 Svy_CheckIfICanDoThisSurveyBasedOnGrps (Svy->SvyCod);
	    break;
        }

      /* Have I answered this survey? */
      Svy->Status.IHaveAnswered = Svy_CheckIfIHaveAnsweredSvy (Svy->SvyCod);

      /* Can I answer survey? */
      Svy->Status.ICanAnswer = (Svy->NumQsts != 0) &&
                                Svy->Status.Visible &&
                                Svy->Status.Open &&
                                Svy->Status.IAmLoggedWithAValidRoleToAnswer &&
                                Svy->Status.IBelongToScope &&
                               !Svy->Status.IHaveAnswered;

      /* Can I view results of the survey?
         Can I edit survey? */
      switch (Gbl.Usrs.Me.LoggedRole)
        {
         case Rol_STUDENT:
            Svy->Status.ICanViewResults = (Svy->Scope == Sco_SCOPE_CRS ||
        	                           Svy->Scope == Sco_SCOPE_DEG ||
        	                           Svy->Scope == Sco_SCOPE_CTR ||
        	                           Svy->Scope == Sco_SCOPE_INS ||
        	                           Svy->Scope == Sco_SCOPE_CTY ||
        	                           Svy->Scope == Sco_SCOPE_SYS) &&
        	                          (Svy->NumQsts != 0) &&
                                           Svy->Status.Visible &&
                                           Svy->Status.Open &&
                                           Svy->Status.IAmLoggedWithAValidRoleToAnswer &&
                                           Svy->Status.IBelongToScope &&
                                           Svy->Status.IHaveAnswered;
            Svy->Status.ICanEdit         = false;
            break;
         case Rol_TEACHER:
            Svy->Status.ICanViewResults = (Svy->Scope == Sco_SCOPE_CRS ||
        	                           Svy->Scope == Sco_SCOPE_DEG ||
        	                           Svy->Scope == Sco_SCOPE_CTR ||
        	                           Svy->Scope == Sco_SCOPE_INS ||
        	                           Svy->Scope == Sco_SCOPE_CTY ||
        	                           Svy->Scope == Sco_SCOPE_SYS) &&
        	                           Svy->NumQsts != 0 &&
                                          !Svy->Status.ICanAnswer;
            Svy->Status.ICanEdit        =  Svy->Scope == Sco_SCOPE_CRS &&
                                           Svy->Status.IBelongToScope;
            break;
         case Rol_DEG_ADM:
            Svy->Status.ICanViewResults = (Svy->Scope == Sco_SCOPE_DEG ||
        	                           Svy->Scope == Sco_SCOPE_CTR ||
        	                           Svy->Scope == Sco_SCOPE_INS ||
        	                           Svy->Scope == Sco_SCOPE_CTY ||
        	                           Svy->Scope == Sco_SCOPE_SYS) &&
        	                          (Svy->NumQsts != 0) &&
                                          !Svy->Status.ICanAnswer;
            Svy->Status.ICanEdit        =  Svy->Scope == Sco_SCOPE_DEG &&
                                           Svy->Status.IBelongToScope;
            break;
         case Rol_CTR_ADM:
            Svy->Status.ICanViewResults = (Svy->Scope == Sco_SCOPE_CTR ||
        	                           Svy->Scope == Sco_SCOPE_INS ||
        	                           Svy->Scope == Sco_SCOPE_CTY ||
        	                           Svy->Scope == Sco_SCOPE_SYS) &&
        	                          (Svy->NumQsts != 0) &&
                                          !Svy->Status.ICanAnswer;
            Svy->Status.ICanEdit        =  Svy->Scope == Sco_SCOPE_CTR &&
                                           Svy->Status.IBelongToScope;
            break;
         case Rol_INS_ADM:
            Svy->Status.ICanViewResults = (Svy->Scope == Sco_SCOPE_INS ||
        	                           Svy->Scope == Sco_SCOPE_CTY ||
        	                           Svy->Scope == Sco_SCOPE_SYS) &&
        	                          (Svy->NumQsts != 0) &&
                                          !Svy->Status.ICanAnswer;
            Svy->Status.ICanEdit        =  Svy->Scope == Sco_SCOPE_INS &&
                                           Svy->Status.IBelongToScope;
            break;
         case Rol_SYS_ADM:
            Svy->Status.ICanViewResults = (Svy->NumQsts != 0);
            Svy->Status.ICanEdit        = true;
            break;
         default:
            Svy->Status.ICanViewResults = false;
            Svy->Status.ICanEdit        = false;
            break;
        }
     }
   else
     {
      /* Initialize to empty survey */
      Svy->SvyCod                  = -1L;
      Svy->Scope                   = Sco_SCOPE_UNK;
      Svy->Roles                   = 0;
      Svy->UsrCod                  = -1L;
      Svy->TimeUTC[Svy_START_TIME] =
      Svy->TimeUTC[Svy_END_TIME  ] = (time_t) 0;
      Svy->Title[0]                = '\0';
      Svy->NumQsts                 = 0;
      Svy->NumUsrs                 = 0;
      Svy->Status.Visible                         = true;
      Svy->Status.Open                            = false;
      Svy->Status.IAmLoggedWithAValidRoleToAnswer = false;
      Svy->Status.IBelongToScope                  = false;
      Svy->Status.IHaveAnswered                   = false;
      Svy->Status.ICanAnswer                      = false;
      Svy->Status.ICanViewResults                 = false;
      Svy->Status.ICanEdit                        = false;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************************** Free list of surveys ***************************/
/*****************************************************************************/

void Svy_FreeListSurveys (void)
  {
   if (Gbl.Svys.LstIsRead && Gbl.Svys.LstSvyCods)
     {
      /***** Free memory used by the list of surveys *****/
      free ((void *) Gbl.Svys.LstSvyCods);
      Gbl.Svys.LstSvyCods = NULL;
      Gbl.Svys.Num = 0;
      Gbl.Svys.LstIsRead = false;
     }
  }

/*****************************************************************************/
/********************** Get survey text from database ************************/
/*****************************************************************************/

static void Svy_GetSurveyTxtFromDB (long SvyCod,char *Txt)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get text of survey from database *****/
   sprintf (Query,"SELECT Txt FROM surveys WHERE SvyCod='%ld'",SvyCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get survey text");

   /***** The result of the query must have one row or none *****/
   if (NumRows == 1)
     {
      /* Get info text */
      row = mysql_fetch_row (mysql_res);
      strcpy (Txt,row[0]);
     }
   else
      Txt[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (NumRows > 1)
      Lay_ShowErrorAndExit ("Error when getting survey text.");
  }

/*****************************************************************************/
/******************** Get summary and content of a survey  *******************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error

void Svy_GetNotifSurvey (char *SummaryStr,char **ContentStr,long SvyCod,unsigned MaxChars,bool GetContent)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Build query *****/
   sprintf (Query,"SELECT Title,Txt FROM surveys WHERE SvyCod='%ld'",
            SvyCod);
   if (!mysql_query (&Gbl.mysql,Query))
      if ((mysql_res = mysql_store_result (&Gbl.mysql)) != NULL)
        {
         /***** Result should have a unique row *****/
         if (mysql_num_rows (mysql_res) == 1)
           {
            /***** Get row *****/
            row = mysql_fetch_row (mysql_res);

            /***** Get summary *****/
            strcpy (SummaryStr,row[0]);
            if (MaxChars)
               Str_LimitLengthHTMLStr (SummaryStr,MaxChars);

            /***** Get content *****/
            if (GetContent)
              {
               if ((*ContentStr = (char *) malloc (512+Cns_MAX_BYTES_TEXT)) == NULL)
                  Lay_ShowErrorAndExit ("Error allocating memory for notification content.");
               strcpy (*ContentStr,row[1]);
              }
           }
         mysql_free_result (mysql_res);
        }
  }

/*****************************************************************************/
/******************* Write parameter with code of survey *********************/
/*****************************************************************************/

static void Svy_PutParamSvyCod (long SvyCod)
  {
   Par_PutHiddenParamLong ("SvyCod",SvyCod);
  }

/*****************************************************************************/
/******************** Get parameter with code of survey **********************/
/*****************************************************************************/

static long Svy_GetParamSvyCod (void)
  {
   char LongStr[1+10+1];

   /***** Get parameter with code of survey *****/
   Par_GetParToText ("SvyCod",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/*************** Ask for confirmation of removing of a survey ****************/
/*****************************************************************************/

void Svy_AskRemSurvey (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_survey_X;
   extern const char *Txt_Remove_survey;
   struct Survey Svy;
   struct SurveyQuestion SvyQst;

   /***** Get parameters *****/
   Svy_GetParamSvyOrderType ();
   Grp_GetParamWhichGrps ();
   Pag_GetParamPagNum (Pag_SURVEYS);

   /***** Get survey code *****/
   if ((Svy.SvyCod = Svy_GetParamSvyCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of survey is missing.");

   /***** Get data of the survey from database *****/
   Svy_GetDataOfSurveyByCod (&Svy);
   if (!Svy.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not remove this survey.");

   /***** Ask for confirmation of removing *****/
   sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_survey_X,
            Svy.Title);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);

   /***** Button of confirmation of removing *****/
   Gbl.Svys.SvyCodToEdit = Svy.SvyCod;
   Act_FormStart (ActRemSvy);
   Svy_PutParams ();
   Lay_PutRemoveButton (Txt_Remove_survey);
   Act_FormEnd ();

   /***** Show surveys again *****/
   Svy_ListAllSurveys (&SvyQst);
  }

/*****************************************************************************/
/****************************** Remove a survey ******************************/
/*****************************************************************************/

void Svy_RemoveSurvey (void)
  {
   extern const char *Txt_Survey_X_removed;
   char Query[512];
   struct Survey Svy;
   struct SurveyQuestion SvyQst;

   /***** Get survey code *****/
   if ((Svy.SvyCod = Svy_GetParamSvyCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of survey is missing.");

   /***** Get data of the survey from database *****/
   Svy_GetDataOfSurveyByCod (&Svy);
   if (!Svy.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not remove this survey.");

   /***** Remove all the users in this survey *****/
   sprintf (Query,"DELETE FROM svy_users WHERE SvyCod='%ld'",
            Svy.SvyCod);
   DB_QueryDELETE (Query,"can not remove users who are answered a survey");

   /***** Remove all the answers in this survey *****/
   sprintf (Query,"DELETE FROM svy_answers USING svy_questions,svy_answers"
                  " WHERE svy_questions.SvyCod='%ld'"
                  " AND svy_questions.QstCod=svy_answers.QstCod",
            Svy.SvyCod);
   DB_QueryDELETE (Query,"can not remove answers of a survey");

   /***** Remove all the questions in this survey *****/
   sprintf (Query,"DELETE FROM svy_questions"
                  " WHERE SvyCod='%ld'",
            Svy.SvyCod);
   DB_QueryDELETE (Query,"can not remove questions of a survey");

   /***** Remove all the groups of this survey *****/
   Svy_RemoveAllTheGrpsAssociatedToAndSurvey (Svy.SvyCod);

   /***** Remove survey *****/
   sprintf (Query,"DELETE FROM surveys WHERE SvyCod='%ld'",Svy.SvyCod);
   DB_QueryDELETE (Query,"can not remove survey");

   /***** Mark possible notifications as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_SURVEY,Svy.SvyCod);

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Survey_X_removed,
            Svy.Title);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show surveys again *****/
   Svy_ListAllSurveys (&SvyQst);
  }

/*****************************************************************************/
/***************** Ask for confirmation of reset of a survey *****************/
/*****************************************************************************/

void Svy_AskResetSurvey (void)
  {
   extern const char *Txt_Do_you_really_want_to_reset_the_survey_X;
   struct Survey Svy;
   struct SurveyQuestion SvyQst;

   /***** Get parameters *****/
   Svy_GetParamSvyOrderType ();
   Grp_GetParamWhichGrps ();
   Pag_GetParamPagNum (Pag_SURVEYS);

   /***** Get survey code *****/
   if ((Svy.SvyCod = Svy_GetParamSvyCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of survey is missing.");

   /***** Get data of the survey from database *****/
   Svy_GetDataOfSurveyByCod (&Svy);
   if (!Svy.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not reset this survey.");

   /***** Ask for confirmation of reset *****/
   sprintf (Gbl.Message,Txt_Do_you_really_want_to_reset_the_survey_X,
            Svy.Title);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);

   /***** Button of confirmation of reset *****/
   Gbl.Svys.SvyCodToEdit = Svy.SvyCod;
   Svy_PutButtonToResetSurvey ();

   /***** Show surveys again *****/
   Svy_ListAllSurveys (&SvyQst);
  }

/*****************************************************************************/
/************************* Put button to reset survey ************************/
/*****************************************************************************/

static void Svy_PutButtonToResetSurvey (void)
  {
   extern const char *Txt_Reset_survey;

   Act_FormStart (ActRstSvy);
   Svy_PutParams ();
   Lay_PutRemoveButton (Txt_Reset_survey);
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************************* Reset a survey ******************************/
/*****************************************************************************/

void Svy_ResetSurvey (void)
  {
   extern const char *Txt_Survey_X_reset;
   char Query[512];
   struct Survey Svy;
   struct SurveyQuestion SvyQst;

   /***** Get survey code *****/
   if ((Svy.SvyCod = Svy_GetParamSvyCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of survey is missing.");

   /***** Get data of the survey from database *****/
   Svy_GetDataOfSurveyByCod (&Svy);
   if (!Svy.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not reset this survey.");

   /***** Remove all the users in this survey *****/
   sprintf (Query,"DELETE FROM svy_users WHERE SvyCod='%ld'",
            Svy.SvyCod);
   DB_QueryDELETE (Query,"can not remove users who are answered a survey");

   /***** Reset all the answers in this survey *****/
   sprintf (Query,"UPDATE svy_answers,svy_questions SET svy_answers.NumUsrs='0'"
                  " WHERE svy_questions.SvyCod='%ld'"
                  " AND svy_questions.QstCod=svy_answers.QstCod",
            Svy.SvyCod);
   DB_QueryUPDATE (Query,"can not reset answers of a survey");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Survey_X_reset,
            Svy.Title);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show surveys again *****/
   Svy_ListAllSurveys (&SvyQst);
  }

/*****************************************************************************/
/******************************** Hide a survey ******************************/
/*****************************************************************************/

void Svy_HideSurvey (void)
  {
   extern const char *Txt_Survey_X_is_now_hidden;
   char Query[512];
   struct Survey Svy;
   struct SurveyQuestion SvyQst;

   /***** Get survey code *****/
   if ((Svy.SvyCod = Svy_GetParamSvyCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of survey is missing.");

   /***** Get data of the survey from database *****/
   Svy_GetDataOfSurveyByCod (&Svy);
   if (!Svy.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not hide this survey.");

   /***** Hide survey *****/
   sprintf (Query,"UPDATE surveys SET Hidden='Y' WHERE SvyCod='%ld'",
            Svy.SvyCod);
   DB_QueryUPDATE (Query,"can not hide survey");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Survey_X_is_now_hidden,
            Svy.Title);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show surveys again *****/
   Svy_ListAllSurveys (&SvyQst);
  }

/*****************************************************************************/
/******************************** Show a survey ******************************/
/*****************************************************************************/

void Svy_UnhideSurvey (void)
  {
   extern const char *Txt_Survey_X_is_now_visible;
   char Query[512];
   struct Survey Svy;
   struct SurveyQuestion SvyQst;

   /***** Get survey code *****/
   if ((Svy.SvyCod = Svy_GetParamSvyCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of survey is missing.");

   /***** Get data of the survey from database *****/
   Svy_GetDataOfSurveyByCod (&Svy);
   if (!Svy.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not unhide this survey.");

   /***** Show survey *****/
   sprintf (Query,"UPDATE surveys SET Hidden='N' WHERE SvyCod='%ld'",
            Svy.SvyCod);
   DB_QueryUPDATE (Query,"can not show survey");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Survey_X_is_now_visible,
            Svy.Title);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show surveys again *****/
   Svy_ListAllSurveys (&SvyQst);
  }

/*****************************************************************************/
/******************* Check if the title of a survey exists *******************/
/*****************************************************************************/

static bool Svy_CheckIfSimilarSurveyExists (struct Survey *Svy)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   char Query[512];

   /***** Get number of surveys with a field value from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM surveys"
                  " WHERE Scope='%s' AND Cod='%ld'"
                  " AND Title='%s' AND SvyCod<>'%ld'",
            Sco_ScopeDB[Svy->Scope],Svy->Cod,
            Svy->Title,Svy->SvyCod);
   return (DB_QueryCOUNT (Query,"can not get similar surveys") != 0);
  }

/*****************************************************************************/
/********************* Put a form to create a new survey *********************/
/*****************************************************************************/

void Svy_RequestCreatOrEditSvy (void)
  {
   extern const char *Hlp_STATS_Surveys_new_survey;
   extern const char *Hlp_STATS_Surveys_edit_survey;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_New_survey;
   extern const char *Txt_Scope;
   extern const char *Txt_Edit_survey;
   extern const char *Txt_Title;
   extern const char *Txt_Description;
   extern const char *Txt_Users;
   extern const char *Txt_Create_survey;
   extern const char *Txt_Save;
   struct Survey Svy;
   struct SurveyQuestion SvyQst;
   bool ItsANewSurvey;
   char Txt[Cns_MAX_BYTES_TEXT+1];

   /***** Get parameters *****/
   Svy_GetParamSvyOrderType ();
   Grp_GetParamWhichGrps ();
   Pag_GetParamPagNum (Pag_SURVEYS);

   /***** Get the code of the survey *****/
   ItsANewSurvey = ((Svy.SvyCod = Svy_GetParamSvyCod ()) == -1L);

   /***** Get from the database the data of the survey *****/
   if (ItsANewSurvey)
     {
      /***** Put link (form) to create new survey *****/
      if (!Svy_CheckIfICanCreateSvy ())
         Lay_ShowErrorAndExit ("You can not create a new survey here.");

      /* Initialize to empty survey */
      Svy.SvyCod = -1L;
      Svy.Scope  = Sco_SCOPE_UNK;
      Svy.Roles  = (1 << Rol_STUDENT);
      Svy.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
      Svy.TimeUTC[Svy_START_TIME] = Gbl.StartExecutionTimeUTC;
      Svy.TimeUTC[Svy_END_TIME  ] = Gbl.StartExecutionTimeUTC + (24 * 60 * 60);	// +24 hours
      Svy.Title[0] = '\0';
      Svy.NumQsts = 0;
      Svy.NumUsrs = 0;
      Svy.Status.Visible = true;
      Svy.Status.Open = true;
      Svy.Status.IAmLoggedWithAValidRoleToAnswer = false;
      Svy.Status.IBelongToScope = false;
      Svy.Status.IHaveAnswered = false;
      Svy.Status.ICanAnswer = false;
      Svy.Status.ICanViewResults = false;
     }
   else
     {
      /* Get data of the survey from database */
      Svy_GetDataOfSurveyByCod (&Svy);
      if (!Svy.Status.ICanEdit)
         Lay_ShowErrorAndExit ("You can not update this survey.");

      /* Get text of the survey from database */
      Svy_GetSurveyTxtFromDB (Svy.SvyCod,Txt);
     }

   /***** Start form *****/
   Gbl.Svys.SvyCodToEdit = Svy.SvyCod;
   Act_FormStart (ItsANewSurvey ? ActNewSvy :
	                          ActChgSvy);
   Svy_PutParams ();

   /***** Start frame *****/
   Lay_StartRoundFrameTable (NULL,ItsANewSurvey ? Txt_New_survey :
	                                          Txt_Edit_survey,
	                     NULL,ItsANewSurvey ? Hlp_STATS_Surveys_new_survey :
	                                          Hlp_STATS_Surveys_edit_survey,
	                     2);

   /***** Scope of the survey *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_MIDDLE\">"
                      "<label class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Scope);
   Svy_SetDefaultAndAllowedScope (&Svy);
   Sco_GetScope ("ScopeSvy");
   Sco_PutSelectorScope ("ScopeSvy",false);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Survey title *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_MIDDLE\">"
	              "<label class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" name=\"Title\""
                      " size=\"45\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Title,
            Svy_MAX_LENGTH_SURVEY_TITLE,Svy.Title);

   /***** Survey start and end dates *****/
   Dat_PutFormStartEndClientLocalDateTimes (Svy.TimeUTC,Dat_FORM_SECONDS_ON);

   /***** Survey text *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_TOP\">"
	              "<label class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"LEFT_TOP\">"
                      "<textarea name=\"Txt\" cols=\"60\" rows=\"10\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Description);
   if (!ItsANewSurvey)
      fprintf (Gbl.F.Out,"%s",Txt);
   fprintf (Gbl.F.Out,"</textarea>"
                      "</td>"
                      "</tr>");

   /***** Users' roles who can answer the survey *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_TOP\">"
	              "<label class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"DAT LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Users);
   Rol_WriteSelectorRoles (1 << Rol_STUDENT |
                           1 << Rol_TEACHER,
                           Svy.Roles,
                           false,false);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Groups *****/
   Svy_ShowLstGrpsToEditSurvey (Svy.SvyCod);

   /***** Button to create/modify survey and end frame *****/
   if (ItsANewSurvey)
      Lay_EndRoundFrameTableWithButton (Lay_CREATE_BUTTON,Txt_Create_survey);
   else
      Lay_EndRoundFrameTableWithButton (Lay_CONFIRM_BUTTON,Txt_Save);

   /***** End form *****/
   Act_FormEnd ();

   /***** Show questions of the survey ready to be edited *****/
   if (!ItsANewSurvey)
      Svy_ListSvyQuestions (&Svy,&SvyQst);
  }

/*****************************************************************************/
/****** Set default and allowed scopes depending on logged user's role *******/
/*****************************************************************************/

static void Svy_SetDefaultAndAllowedScope (struct Survey *Svy)
  {
   bool ICanEdit = false;

   /***** Set default scope *****/
   Gbl.Scope.Default = Sco_SCOPE_UNK;
   Gbl.Scope.Allowed = 0;

   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_TEACHER:	// Teachers only can edit course surveys
	 if (Gbl.CurrentCrs.Crs.CrsCod > 0)
	   {
	    if (Svy->Scope == Sco_SCOPE_UNK)	// Scope not defined
	       Svy->Scope = Sco_SCOPE_CRS;
	    if (Svy->Scope == Sco_SCOPE_CRS)
	      {
               Gbl.Scope.Default = Svy->Scope;
	       Gbl.Scope.Allowed = 1 << Sco_SCOPE_CRS;
	       ICanEdit = true;
	      }
	   }
         break;
      case Rol_DEG_ADM:	// Degree admins only can edit degree surveys
	 if (Svy->Scope == Sco_SCOPE_UNK)	// Scope not defined
	    Svy->Scope = Sco_SCOPE_DEG;
	 if (Svy->Scope == Sco_SCOPE_DEG)
	   {
	    Gbl.Scope.Default = Svy->Scope;
	    Gbl.Scope.Allowed = 1 << Sco_SCOPE_DEG;
	    ICanEdit = true;
	   }
         break;
      case Rol_CTR_ADM:	// Centre admins only can edit centre surveys
	 if (Svy->Scope == Sco_SCOPE_UNK)	// Scope not defined
	    Svy->Scope = Sco_SCOPE_CTR;
	 if (Svy->Scope == Sco_SCOPE_CTR)
	   {
	    Gbl.Scope.Default = Svy->Scope;
	    Gbl.Scope.Allowed = 1 << Sco_SCOPE_CTR;
	    ICanEdit = true;
	   }
         break;
      case Rol_INS_ADM:	// Institution admins only can edit institution surveys
	 if (Svy->Scope == Sco_SCOPE_UNK)	// Scope not defined
	    Svy->Scope = Sco_SCOPE_INS;
	 if (Svy->Scope == Sco_SCOPE_INS)
	   {
	    Gbl.Scope.Default = Svy->Scope;
	    Gbl.Scope.Allowed = 1 << Sco_SCOPE_INS;
	    ICanEdit = true;
	   }
         break;
      case Rol_SYS_ADM:// System admins can edit any survey
	 if (Svy->Scope == Sco_SCOPE_UNK)	// Scope not defined
	   {
	    if      (Gbl.CurrentCrs.Crs.CrsCod > 0)
	       Svy->Scope = Sco_SCOPE_CRS;
	    else if (Gbl.CurrentDeg.Deg.DegCod > 0)
	       Svy->Scope = Sco_SCOPE_DEG;
	    else if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
	       Svy->Scope = Sco_SCOPE_CTR;
	    else if (Gbl.CurrentIns.Ins.InsCod > 0)
	       Svy->Scope = Sco_SCOPE_INS;
	    else if (Gbl.CurrentCty.Cty.CtyCod > 0)
	       Svy->Scope = Sco_SCOPE_CTY;
	    else
	       Svy->Scope = Sco_SCOPE_SYS;
	   }
         Gbl.Scope.Default = Svy->Scope;
         Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	                     1 << Sco_SCOPE_CTY |
	                     1 << Sco_SCOPE_INS |
	                     1 << Sco_SCOPE_CTR |
                             1 << Sco_SCOPE_DEG |
                             1 << Sco_SCOPE_CRS;
	 ICanEdit = true;
	 break;
      default:
	 break;
     }

   if (!ICanEdit)
      Lay_ShowErrorAndExit ("You don't have permission to edit surveys here.");
  }

/*****************************************************************************/
/******************** Show list of groups to edit a survey *******************/
/*****************************************************************************/

static void Svy_ShowLstGrpsToEditSurvey (long SvyCod)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Groups;
   extern const char *Txt_The_whole_course;
   unsigned NumGrpTyp;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   if (Gbl.CurrentCrs.Grps.GrpTypes.Num)
     {
      /***** Start table *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"%s RIGHT_TOP\">"
	                 "%s:"
	                 "</td>"
                         "<td class=\"LEFT_TOP\">",
               The_ClassForm[Gbl.Prefs.Theme],
               Txt_Groups);
      Lay_StartRoundFrameTable ("95%",NULL,NULL,NULL,0);

      /***** First row: checkbox to select the whole course *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td colspan=\"7\" class=\"DAT LEFT_MIDDLE\">"
                         "<input type=\"checkbox\""
                         " id=\"WholeCrs\" name=\"WholeCrs\" value=\"Y\"");
      if (!Svy_CheckIfSvyIsAssociatedToGrps (SvyCod))
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," onclick=\"uncheckChildren(this,'GrpCods')\" />%s %s</td>"
	                 "</tr>",
               Txt_The_whole_course,Gbl.CurrentCrs.Crs.ShrtName);

      /***** List the groups for each group type *****/
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
         if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
            Grp_ListGrpsToEditAsgAttOrSvy (&Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],SvyCod,Grp_SURVEY);

      /***** End table *****/
      Lay_EndRoundFrameTable ();
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");
     }

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/********************* Receive form to create a new survey *******************/
/*****************************************************************************/

void Svy_RecFormSurvey (void)
  {
   extern const char *Txt_Already_existed_a_survey_with_the_title_X;
   extern const char *Txt_You_must_specify_the_title_of_the_survey;
   struct Survey OldSvy;
   struct Survey NewSvy;
   struct SurveyQuestion SvyQst;
   bool ItsANewSurvey;
   bool NewSurveyIsCorrect = true;
   unsigned NumUsrsToBeNotifiedByEMail;
   char Txt[Cns_MAX_BYTES_TEXT+1];

   /***** Get the code of the survey *****/
   ItsANewSurvey = ((NewSvy.SvyCod = Svy_GetParamSvyCod ()) == -1L);

   if (ItsANewSurvey)
      NewSvy.Scope = Sco_SCOPE_UNK;
   else
     {
      /* Get data of the old (current) survey from database */
      OldSvy.SvyCod = NewSvy.SvyCod;
      Svy_GetDataOfSurveyByCod (&OldSvy);
      if (!OldSvy.Status.ICanEdit)
         Lay_ShowErrorAndExit ("You can not update this survey.");
      NewSvy.Scope = OldSvy.Scope;
     }

   /***** Get scope *****/
   Svy_SetDefaultAndAllowedScope (&NewSvy);
   Sco_GetScope ("ScopeSvy");
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 if (Gbl.Usrs.Me.LoggedRole != Rol_SYS_ADM)
	    Lay_ShowErrorAndExit ("Wrong survey scope.");
         NewSvy.Scope = Sco_SCOPE_SYS;
         NewSvy.Cod = -1L;
         break;
      case Sco_SCOPE_CTY:
	 if (Gbl.Usrs.Me.LoggedRole != Rol_SYS_ADM)
	    Lay_ShowErrorAndExit ("Wrong survey scope.");
	 NewSvy.Scope = Sco_SCOPE_CTY;
	 NewSvy.Cod = Gbl.CurrentCty.Cty.CtyCod;
         break;
      case Sco_SCOPE_INS:
	 if (Gbl.Usrs.Me.LoggedRole != Rol_SYS_ADM &&
	     Gbl.Usrs.Me.LoggedRole != Rol_INS_ADM)
	    Lay_ShowErrorAndExit ("Wrong survey scope.");
	 NewSvy.Scope = Sco_SCOPE_INS;
	 NewSvy.Cod = Gbl.CurrentIns.Ins.InsCod;
         break;
      case Sco_SCOPE_CTR:
	 if (Gbl.Usrs.Me.LoggedRole != Rol_SYS_ADM &&
	     Gbl.Usrs.Me.LoggedRole != Rol_CTR_ADM)
	    Lay_ShowErrorAndExit ("Wrong survey scope.");
	 NewSvy.Scope = Sco_SCOPE_CTR;
	 NewSvy.Cod = Gbl.CurrentCtr.Ctr.CtrCod;
         break;
      case Sco_SCOPE_DEG:
	 if (Gbl.Usrs.Me.LoggedRole != Rol_SYS_ADM &&
	     Gbl.Usrs.Me.LoggedRole != Rol_DEG_ADM)
	    Lay_ShowErrorAndExit ("Wrong survey scope.");
	 NewSvy.Scope = Sco_SCOPE_DEG;
	 NewSvy.Cod = Gbl.CurrentDeg.Deg.DegCod;
         break;
      case Sco_SCOPE_CRS:
	 if (Gbl.Usrs.Me.LoggedRole != Rol_SYS_ADM &&
	     Gbl.Usrs.Me.LoggedRole != Rol_TEACHER)
	    Lay_ShowErrorAndExit ("Wrong survey scope.");
	 NewSvy.Scope = Sco_SCOPE_CRS;
	 NewSvy.Cod = Gbl.CurrentCrs.Crs.CrsCod;
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }

   /***** Get start/end date-times *****/
   NewSvy.TimeUTC[Asg_START_TIME] = Dat_GetTimeUTCFromForm ("StartTimeUTC");
   NewSvy.TimeUTC[Asg_END_TIME  ] = Dat_GetTimeUTCFromForm ("EndTimeUTC"  );

   /***** Get survey title *****/
   Par_GetParToText ("Title",NewSvy.Title,Svy_MAX_LENGTH_SURVEY_TITLE);

   /***** Get survey text and insert links *****/
   Par_GetParToHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (NewSvy.TimeUTC[Svy_START_TIME] == 0)
      NewSvy.TimeUTC[Svy_START_TIME] = Gbl.StartExecutionTimeUTC;
   if (NewSvy.TimeUTC[Svy_END_TIME] == 0)
      NewSvy.TimeUTC[Svy_END_TIME] = NewSvy.TimeUTC[Svy_START_TIME] + 24*60*60;	// +24 hours

   /***** Get users who can answer this survey *****/
   NewSvy.Roles = Rol_GetSelectedRoles ();

   /***** Check if title is correct *****/
   if (NewSvy.Title[0])	// If there's a survey title
     {
      /* If title of survey was in database... */
      if (Svy_CheckIfSimilarSurveyExists (&NewSvy))
        {
         NewSurveyIsCorrect = false;
         sprintf (Gbl.Message,Txt_Already_existed_a_survey_with_the_title_X,
                  NewSvy.Title);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }
     }
   else	// If there is not a survey title
     {
      NewSurveyIsCorrect = false;
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_the_title_of_the_survey);
     }

   /***** Create a new survey or update an existing one *****/
   if (NewSurveyIsCorrect)
     {
      /* Get groups for this surveys */
      Grp_GetParCodsSeveralGrpsToEditAsgAttOrSvy ();

      if (ItsANewSurvey)
         Svy_CreateSurvey (&NewSvy,Txt);	// Add new survey to database
      else
         Svy_UpdateSurvey (&NewSvy,Txt);

      /* Free memory for list of selected groups */
      Grp_FreeListCodSelectedGrps ();
     }
   else
      Svy_RequestCreatOrEditSvy ();

   /***** Notify by email about the new survey *****/
   if (NewSvy.Scope == Sco_SCOPE_CRS)	// Notify only the surveys for a course, not for a degree or global
     {
      if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_SURVEY,NewSvy.SvyCod)))
         Svy_UpdateNumUsrsNotifiedByEMailAboutSurvey (NewSvy.SvyCod,NumUsrsToBeNotifiedByEMail);
      Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);
     }

   /***** Show surveys again *****/
   Svy_ListAllSurveys (&SvyQst);
  }

/*****************************************************************************/
/*********** Update number of users notified in table of surveys *************/
/*****************************************************************************/

static void Svy_UpdateNumUsrsNotifiedByEMailAboutSurvey (long SvyCod,
                                                         unsigned NumUsrsToBeNotifiedByEMail)
  {
   char Query[512];

   /***** Update number of users notified *****/
   sprintf (Query,"UPDATE surveys SET NumNotif=NumNotif+'%u'"
                  " WHERE SvyCod='%ld'",
            NumUsrsToBeNotifiedByEMail,SvyCod);
   DB_QueryUPDATE (Query,"can not update the number of notifications of a survey");
  }

/*****************************************************************************/
/*************************** Create a new survey *****************************/
/*****************************************************************************/

static void Svy_CreateSurvey (struct Survey *Svy,const char *Txt)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   extern const char *Txt_Created_new_survey_X;
   char Query[1024+Cns_MAX_BYTES_TEXT];

   /***** Create a new survey *****/
   sprintf (Query,"INSERT INTO surveys"
	          " (Scope,Cod,Hidden,Roles,UsrCod,StartTime,EndTime,Title,Txt)"
                  " VALUES ('%s','%ld','N','%u','%ld',"
                  "FROM_UNIXTIME('%ld'),FROM_UNIXTIME('%ld'),"
                  "'%s','%s')",
            Sco_ScopeDB[Svy->Scope],Svy->Cod,
            Svy->Roles,
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Svy->TimeUTC[Svy_START_TIME],
            Svy->TimeUTC[Svy_END_TIME  ],
            Svy->Title,
            Txt);
   Svy->SvyCod = DB_QueryINSERTandReturnCode (Query,"can not create new survey");

   /***** Create groups *****/
   if (Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps)
      Svy_CreateGrps (Svy->SvyCod);

   /***** Write success message *****/
   sprintf (Gbl.Message,Txt_Created_new_survey_X,
            Svy->Title);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/************************* Update an existing survey *************************/
/*****************************************************************************/

static void Svy_UpdateSurvey (struct Survey *Svy,const char *Txt)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   extern const char *Txt_The_survey_has_been_modified;
   char Query[1024+Cns_MAX_BYTES_TEXT];

   /***** Update the data of the survey *****/
   sprintf (Query,"UPDATE surveys"
	          " SET Scope='%s',Cod='%ld',Roles='%u',"
	          "StartTime=FROM_UNIXTIME('%ld'),"
	          "EndTime=FROM_UNIXTIME('%ld'),"
	          "Title='%s',Txt='%s'"
                  " WHERE SvyCod='%ld'",
            Sco_ScopeDB[Svy->Scope],Svy->Cod,
            Svy->Roles,
            Svy->TimeUTC[Svy_START_TIME],
            Svy->TimeUTC[Svy_END_TIME  ],
            Svy->Title,
            Txt,
            Svy->SvyCod);
   DB_QueryUPDATE (Query,"can not update survey");

   /***** Update groups *****/
   /* Remove old groups */
   Svy_RemoveAllTheGrpsAssociatedToAndSurvey (Svy->SvyCod);

   /* Create new groups */
   if (Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps)
      Svy_CreateGrps (Svy->SvyCod);

   /***** Write success message *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_The_survey_has_been_modified);
  }

/*****************************************************************************/
/*************** Check if a survey is associated to any group ****************/
/*****************************************************************************/

static bool Svy_CheckIfSvyIsAssociatedToGrps (long SvyCod)
  {
   char Query[256];

   /***** Get if a survey is associated to a group from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM svy_grp WHERE SvyCod='%ld'",
            SvyCod);
   return (DB_QueryCOUNT (Query,"can not check if a survey is associated to groups") != 0);
  }

/*****************************************************************************/
/**************** Check if a survey is associated to a group *****************/
/*****************************************************************************/

bool Svy_CheckIfSvyIsAssociatedToGrp (long SvyCod,long GrpCod)
  {
   char Query[512];

   /***** Get if a survey is associated to a group from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM svy_grp WHERE SvyCod='%ld' AND GrpCod='%ld'",
            SvyCod,GrpCod);
   return (DB_QueryCOUNT (Query,"can not check if a survey is associated to a group") != 0);
  }

/*****************************************************************************/
/************************* Remove groups of a survey *************************/
/*****************************************************************************/

static void Svy_RemoveAllTheGrpsAssociatedToAndSurvey (long SvyCod)
  {
   char Query[256];

   /***** Remove groups of the survey *****/
   sprintf (Query,"DELETE FROM svy_grp WHERE SvyCod='%ld'",SvyCod);
   DB_QueryDELETE (Query,"can not remove the groups associated to a survey");
  }

/*****************************************************************************/
/******************* Remove one group from all the surveys *******************/
/*****************************************************************************/

void Svy_RemoveGroup (long GrpCod)
  {
   char Query[256];

   /***** Remove group from all the surveys *****/
   sprintf (Query,"DELETE FROM svy_grp"
	          " WHERE GrpCod='%ld'",
	    GrpCod);
   DB_QueryDELETE (Query,"can not remove group"
	                 " from the associations between surveys and groups");
  }

/*****************************************************************************/
/*************** Remove groups of one type from all the surveys **************/
/*****************************************************************************/

void Svy_RemoveGroupsOfType (long GrpTypCod)
  {
   char Query[256];

   /***** Remove group from all the surveys *****/
   sprintf (Query,"DELETE FROM svy_grp USING crs_grp,svy_grp"
                  " WHERE crs_grp.GrpTypCod='%ld'"
                  " AND crs_grp.GrpCod=svy_grp.GrpCod",
            GrpTypCod);
   DB_QueryDELETE (Query,"can not remove groups of a type"
	                 " from the associations between surveys and groups");
  }

/*****************************************************************************/
/************************ Create groups of a survey **************************/
/*****************************************************************************/

static void Svy_CreateGrps (long SvyCod)
  {
   unsigned NumGrpSel;
   char Query[256];

   /***** Create groups of the survey *****/
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
     {
      /* Create group */
      sprintf (Query,"INSERT INTO svy_grp (SvyCod,GrpCod) VALUES ('%ld','%ld')",
               SvyCod,Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCod[NumGrpSel]);
      DB_QueryINSERT (Query,"can not associate a group to a survey");
     }
  }

/*****************************************************************************/
/************ Get and write the names of the groups of a survey **************/
/*****************************************************************************/

static void Svy_GetAndWriteNamesOfGrpsAssociatedToSvy (struct Survey *Svy)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *Txt_and;
   extern const char *Txt_The_whole_course;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;

   /***** Get groups associated to a survey from database *****/
   sprintf (Query,"SELECT crs_grp_types.GrpTypName,crs_grp.GrpName"
                  " FROM svy_grp,crs_grp,crs_grp_types"
                  " WHERE svy_grp.SvyCod='%ld'"
                  " AND svy_grp.GrpCod=crs_grp.GrpCod"
                  " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
                  " ORDER BY crs_grp_types.GrpTypName,crs_grp.GrpName",
            Svy->SvyCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get groups of a survey");

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s: ",
            Svy->Status.Visible ? "ASG_GRP" :
        	                  "ASG_GRP_LIGHT",
            NumRows == 1 ? Txt_Group  :
                           Txt_Groups);

   /***** Write groups *****/
   if (NumRows) // Groups found...
     {
      /* Get and write the group types and names */
      for (NumRow = 0;
	   NumRow < NumRows;
	   NumRow++)
        {
         /* Get next group */
         row = mysql_fetch_row (mysql_res);

         /* Write group type name and group name */
         fprintf (Gbl.F.Out,"%s %s",row[0],row[1]);

         if (NumRows >= 2)
           {
            if (NumRow == NumRows-2)
               fprintf (Gbl.F.Out," %s ",Txt_and);
            if (NumRows >= 3)
              if (NumRow < NumRows-2)
                  fprintf (Gbl.F.Out,", ");
           }
        }
     }
   else
      fprintf (Gbl.F.Out,"%s %s",
               Txt_The_whole_course,Gbl.CurrentCrs.Crs.ShrtName);

   fprintf (Gbl.F.Out,"</div>");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Remove all the surveys of a place on the hierarchy *************/
/************ (country, institution, centre, degree or course)   *************/
/*****************************************************************************/

void Svy_RemoveSurveys (Sco_Scope_t Scope,long Cod)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   char Query[512];

   /***** Remove all the users in course surveys *****/
   sprintf (Query,"DELETE FROM svy_users"
	          " USING surveys,svy_users"
                  " WHERE surveys.Scope='%s' AND surveys.Cod='%ld'"
                  " AND surveys.SvyCod=svy_users.SvyCod",
            Sco_ScopeDB[Scope],Cod);
   DB_QueryDELETE (Query,"can not remove users"
	                 " who had answered surveys in a place on the hierarchy");

   /***** Remove all the answers in course surveys *****/
   sprintf (Query,"DELETE FROM svy_answers"
	          " USING surveys,svy_questions,svy_answers"
                  " WHERE surveys.Scope='%s' AND surveys.Cod='%ld'"
                  " AND surveys.SvyCod=svy_questions.SvyCod"
                  " AND svy_questions.QstCod=svy_answers.QstCod",
            Sco_ScopeDB[Scope],Cod);
   DB_QueryDELETE (Query,"can not remove answers of surveys in a place on the hierarchy");

   /***** Remove all the questions in course surveys *****/
   sprintf (Query,"DELETE FROM svy_questions"
	          " USING surveys,svy_questions"
                  " WHERE surveys.Scope='%s' AND surveys.Cod='%ld'"
                  " AND surveys.SvyCod=svy_questions.SvyCod",
            Sco_ScopeDB[Scope],Cod);
   DB_QueryDELETE (Query,"can not remove questions of surveys in a place on the hierarchy");

   /***** Remove groups *****/
   sprintf (Query,"DELETE FROM svy_grp"
	          " USING surveys,svy_grp"
                  " WHERE surveys.Scope='%s' AND surveys.Cod='%ld'"
                  " AND surveys.SvyCod=svy_grp.SvyCod",
            Sco_ScopeDB[Scope],Cod);
   DB_QueryDELETE (Query,"can not remove all the groups"
	                 " associated to surveys of a course");

   /***** Remove course surveys *****/
   sprintf (Query,"DELETE FROM surveys"
	          " WHERE Scope='%s' AND Cod='%ld'",
            Sco_ScopeDB[Scope],Cod);
   DB_QueryDELETE (Query,"can not remove all the surveys in a place on the hierarchy");
  }

/*****************************************************************************/
/************ Check if I belong to any of the groups of a survey *************/
/*****************************************************************************/

static bool Svy_CheckIfICanDoThisSurveyBasedOnGrps (long SvyCod)
  {
   char Query[512];

   /***** Get if I can do a survey from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM surveys"
                  " WHERE SvyCod='%ld'"
                  " AND (SvyCod NOT IN (SELECT SvyCod FROM svy_grp) OR"
                  " SvyCod IN (SELECT svy_grp.SvyCod FROM svy_grp,crs_grp_usr"
                  " WHERE crs_grp_usr.UsrCod='%ld'"
                  " AND svy_grp.GrpCod=crs_grp_usr.GrpCod))",
            SvyCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if I can do a survey") != 0);
  }

/*****************************************************************************/
/******************* Get number of questions of a survey *********************/
/*****************************************************************************/

static unsigned Svy_GetNumQstsSvy (long SvyCod)
  {
   char Query[512];

   /***** Get data of questions from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM svy_questions WHERE SvyCod='%ld'",
            SvyCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of questions of a survey");
  }

/*****************************************************************************/
/*********** Put a form to edit/create a question in survey  *****************/
/*****************************************************************************/

void Svy_RequestEditQuestion (void)
  {
   long SvyCod;
   struct SurveyQuestion SvyQst;
   char Txt[Cns_MAX_BYTES_TEXT+1];

   /***** Initialize question to zero *****/
   Svy_InitQst (&SvyQst);
   Txt[0] = '\0';

   /***** Get survey code *****/
   if ((SvyCod = Svy_GetParamSvyCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of survey is missing.");

   /* Get the question code */
   SvyQst.QstCod = Svy_GetParamQstCod ();

   /***** Get other parameters *****/
   Svy_GetParamSvyOrderType ();
   Grp_GetParamWhichGrps ();
   Pag_GetParamPagNum (Pag_SURVEYS);

   /***** Show form to create a new question in this survey *****/
   Svy_ShowFormEditOneQst (SvyCod,&SvyQst,Txt);

   /***** Show current survey *****/
   Svy_ShowOneSurvey (SvyCod,&SvyQst,true);
  }

/*****************************************************************************/
/******************* Show form to edit one survey question *******************/
/*****************************************************************************/

static void Svy_ShowFormEditOneQst (long SvyCod,struct SurveyQuestion *SvyQst,char *Txt)
  {
   extern const char *Hlp_STATS_Surveys_questions;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Question;
   extern const char *Txt_New_question;
   extern const char *Txt_Stem;
   extern const char *Txt_Type;
   extern const char *Txt_SURVEY_STR_ANSWER_TYPES[Svy_NUM_ANS_TYPES];
   extern const char *Txt_Save;
   extern const char *Txt_Create_question;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAns,NumAnswers = 0;
   Svy_AnswerType_t AnsType;

   if (Gbl.Action.Act == ActEdiOneSvyQst) // If no receiving the question, but editing a new or existing question
     {
      if ((SvyQst->QstCod > 0))	// If parameter QstCod received ==> question already exists in the database
        {
         /***** Get the type of answer and the stem from the database *****/
         /* Get the question from database */
         sprintf (Query,"SELECT QstInd,AnsType,Stem FROM svy_questions"
                        " WHERE QstCod='%ld' AND SvyCod='%ld'",
                  SvyQst->QstCod,SvyCod);
         DB_QuerySELECT (Query,&mysql_res,"can not get a question");

         row = mysql_fetch_row (mysql_res);

         /* Get question index inside survey (row[0]) */
         if (sscanf (row[0],"%u",&(SvyQst->QstInd)) != 1)
            Lay_ShowErrorAndExit ("Error: wrong question index.");

         /* Get the type of answer (row[1]) */
         SvyQst->AnswerType = Svy_ConvertFromStrAnsTypDBToAnsTyp (row[1]);

         /* Get the stem of the question from the database (row[2]) */
         strncpy (Txt,row[2],Cns_MAX_BYTES_TEXT);
         Txt[Cns_MAX_BYTES_TEXT] = '\0';

         /* Free structure that stores the query result */
	 DB_FreeMySQLResult (&mysql_res);

         /***** Get the answers from the database *****/
         NumAnswers = Svy_GetAnswersQst (SvyQst->QstCod,&mysql_res);	// Result: AnsInd,NumUsrs,Answer
         for (NumAns = 0;
              NumAns < NumAnswers;
              NumAns++)
           {
            row = mysql_fetch_row (mysql_res);

            if (NumAnswers > Svy_MAX_ANSWERS_PER_QUESTION)
               Lay_ShowErrorAndExit ("Wrong answer.");
            if (!Svy_AllocateTextChoiceAnswer (SvyQst,NumAns))
               Lay_ShowErrorAndExit (Gbl.Message);

            strncpy (SvyQst->AnsChoice[NumAns].Text,row[2],Svy_MAX_BYTES_ANSWER);
            SvyQst->AnsChoice[NumAns].Text[Svy_MAX_BYTES_ANSWER] = '\0';
           }
         /* Free structure that stores the query result */
	 DB_FreeMySQLResult (&mysql_res);
        }
     }

   /***** Start frame *****/
   if (SvyQst->QstCod > 0)	// If the question already has assigned a code
     {
      /* Parameters for contextual icon */
      Gbl.Svys.SvyCodToEdit = SvyCod;
      Gbl.Svys.SvyQstCodToEdit = SvyQst->QstCod;

      sprintf (Gbl.Title,"%s %u",
               Txt_Question,SvyQst->QstInd + 1);	// Question index may be 0, 1, 2, 3,...
      Lay_StartRoundFrame (NULL,Gbl.Title,Svy_PutIconToRemoveOneQst,NULL);
     }
   else
      Lay_StartRoundFrame (NULL,Txt_New_question,
                           NULL,Hlp_STATS_Surveys_questions);

   /***** Start form *****/
   Act_FormStart (ActRcvSvyQst);
   Svy_PutParamSvyCod (SvyCod);
   if (SvyQst->QstCod > 0)	// If the question already has assigned a code
      Svy_PutParamQstCod (SvyQst->QstCod);

   /***** Start table *****/
   fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL CELLS_PAD_2\">");

   /***** Stem *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>"
                      "<td class=\"LEFT_TOP\">"
                      "<textarea name=\"Txt\" cols=\"60\" rows=\"4\">"
	              "%s"
                      "</textarea>"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Stem,
	    Txt);

   /***** Type of answer *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>"
	              "<td class=\"%s LEFT_TOP\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Type,
            The_ClassForm[Gbl.Prefs.Theme]);
   for (AnsType = (Svy_AnswerType_t) 0;
	AnsType < Svy_NUM_ANS_TYPES;
	AnsType++)
     {
      fprintf (Gbl.F.Out,"<input type=\"radio\" name=\"AnswerType\" value=\"%u\"",
               (unsigned) AnsType);
      if (AnsType == SvyQst->AnswerType)
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," />%s<br />",
               Txt_SURVEY_STR_ANSWER_TYPES[AnsType]);
     }
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Answers *****/
   /* Unique or multiple choice answers */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td></td>"
                      "<td class=\"LEFT_TOP\">"
                      "<table class=\"CELLS_PAD_2\">");

   for (NumAns = 0;
	NumAns < Svy_MAX_ANSWERS_PER_QUESTION;
	NumAns++)
     {
      /* Label with the number of the answer */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"%s RIGHT_TOP\">"
	                 "%u)"
	                 "</td>",
               The_ClassForm[Gbl.Prefs.Theme],NumAns+1);

      /* Answer text */
      fprintf (Gbl.F.Out,"<td class=\"RIGHT_TOP\">"
                         "<textarea name=\"AnsStr%u\" cols=\"50\" rows=\"1\">",
               NumAns);
      if (SvyQst->AnsChoice[NumAns].Text)
         fprintf (Gbl.F.Out,"%s",SvyQst->AnsChoice[NumAns].Text);
      fprintf (Gbl.F.Out,"</textarea>"
	                 "</td>"
	                 "</tr>");
     }
   fprintf (Gbl.F.Out,"</table>"
	              "</td>"
	              "</tr>");

   /***** End table *****/
   fprintf (Gbl.F.Out,"</table>");

   /***** Send button *****/
   if (SvyQst->QstCod > 0)	// If the question already has assigned a code
      Lay_PutConfirmButton (Txt_Save);
   else
      Lay_PutCreateButton (Txt_Create_question);

   /***** End form *****/
   Act_FormEnd ();

   /***** End frame *****/
   Lay_EndRoundFrame ();

   /***** Free memory for answers *****/
   Svy_FreeTextChoiceAnswers (SvyQst,NumAnswers);
  }

/*****************************************************************************/
/********************* Initialize a new question to zero *********************/
/*****************************************************************************/

static void Svy_InitQst (struct SurveyQuestion *SvyQst)
  {
   unsigned NumAns;

   SvyQst->QstCod = -1L;
   SvyQst->QstInd = 0;
   SvyQst->AnswerType = Svy_ANS_UNIQUE_CHOICE;
   for (NumAns = 0;
	NumAns < Svy_MAX_ANSWERS_PER_QUESTION;
	NumAns++)
      SvyQst->AnsChoice[NumAns].Text = NULL;
  }

/*****************************************************************************/
/****************** Write parameter with code of question ********************/
/*****************************************************************************/

static void Svy_PutParamQstCod (long QstCod)
  {
   Par_PutHiddenParamLong ("QstCod",QstCod);
  }

/*****************************************************************************/
/******************* Get parameter with code of question *********************/
/*****************************************************************************/

static long Svy_GetParamQstCod (void)
  {
   char LongStr[1+10+1];

   /***** Get parameter with code of survey *****/
   Par_GetParToText ("QstCod",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/********************* Remove answers of a survey question *******************/
/*****************************************************************************/

static void Svy_RemAnswersOfAQuestion (long QstCod)
  {
   char Query[512];

   /***** Remove answers *****/
   sprintf (Query,"DELETE FROM svy_answers WHERE QstCod='%ld'",QstCod);
   DB_QueryDELETE (Query,"can not remove the answers of a question");
  }

/*****************************************************************************/
/*********** Convert a string with the answer type to answer type ************/
/*****************************************************************************/

static Svy_AnswerType_t Svy_ConvertFromStrAnsTypDBToAnsTyp (const char *StrAnsTypeBD)
  {
   Svy_AnswerType_t AnsType;

   for (AnsType = (Svy_AnswerType_t) 0;
	AnsType < Svy_NUM_ANS_TYPES;
	AnsType++)
      if (!strcmp (StrAnsTypeBD,Svy_StrAnswerTypesDB[AnsType]))
         return AnsType;

   return (Svy_AnswerType_t) 0;
  }

/*****************************************************************************/
/************ Convert a string with an unsigned to answer type ***************/
/*****************************************************************************/

static Svy_AnswerType_t Svy_ConvertFromUnsignedStrToAnsTyp (const char *UnsignedStr)
  {
   unsigned AnsType;

   if (sscanf (UnsignedStr,"%u",&AnsType) != 1)
      Lay_ShowErrorAndExit ("Wrong type of answer.");
   if (AnsType >= Svy_NUM_ANS_TYPES)
      Lay_ShowErrorAndExit ("Wrong type of answer.");
   return (Svy_AnswerType_t) AnsType;
  }

/*****************************************************************************/
/*********** Check if an answer of a question exists in database *************/
/*****************************************************************************/

static bool Svy_CheckIfAnswerExists (long QstCod,unsigned AnsInd)
  {
   char Query[512];

   /***** Get answers of a question from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM svy_answers"
                  " WHERE QstCod='%ld' AND AnsInd='%u'",
            QstCod,AnsInd);
   return (DB_QueryCOUNT (Query,"can not check if an answer exists") != 0);
  }

/*****************************************************************************/
/************** Get answers of a survey question from database ***************/
/*****************************************************************************/

static unsigned Svy_GetAnswersQst (long QstCod,MYSQL_RES **mysql_res)
  {
   char Query[512];
   unsigned long NumRows;

   /***** Get answers of a question from database *****/
   sprintf (Query,"SELECT AnsInd,NumUsrs,Answer FROM svy_answers"
                  " WHERE QstCod='%ld' ORDER BY AnsInd",
            QstCod);
   NumRows = DB_QuerySELECT (Query,mysql_res,"can not get answers of a question");

   /***** Count number of rows of result *****/
   if (NumRows == 0)
      Lay_ShowAlert (Lay_ERROR,"Error when getting answers of a question.");

   return (unsigned) NumRows;
  }

/*****************************************************************************/
/******************* Allocate memory for a choice answer *********************/
/*****************************************************************************/

static int Svy_AllocateTextChoiceAnswer (struct SurveyQuestion *SvyQst,unsigned NumAns)
  {
   Svy_FreeTextChoiceAnswer (SvyQst,NumAns);
   if ((SvyQst->AnsChoice[NumAns].Text = malloc (Svy_MAX_BYTES_ANSWER+1)) == NULL)
     {
      sprintf (Gbl.Message,"Not enough memory to store answer.");
      return 0;
     }
   SvyQst->AnsChoice[NumAns].Text[0] = '\0';
   return 1;
  }

/*****************************************************************************/
/******************** Free memory of all choice answers **********************/
/*****************************************************************************/

static void Svy_FreeTextChoiceAnswers (struct SurveyQuestion *SvyQst,unsigned NumAnswers)
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

static void Svy_FreeTextChoiceAnswer (struct SurveyQuestion *SvyQst,unsigned NumAns)
  {
   if (SvyQst->AnsChoice[NumAns].Text)
     {
      free ((void *) SvyQst->AnsChoice[NumAns].Text);
      SvyQst->AnsChoice[NumAns].Text = NULL;
     }
  }

/*****************************************************************************/
/*********************** Receive a question of a survey **********************/
/*****************************************************************************/

void Svy_ReceiveQst (void)
  {
   extern const char *Txt_You_must_type_the_stem_of_the_question;
   extern const char *Txt_You_can_not_leave_empty_intermediate_answers;
   extern const char *Txt_You_must_type_at_least_the_first_two_answers;
   extern const char *Txt_The_survey_has_been_modified;
   char Txt[Cns_MAX_BYTES_TEXT+1];
   char Query[512+Cns_MAX_BYTES_TEXT+1];
   long SvyCod;
   struct SurveyQuestion SvyQst;
   char UnsignedStr[10+1];
   unsigned NumAns;
   char AnsStr[8+10+1];
   unsigned NumLastAns;
   bool ThereIsEndOfAnswers;
   bool Error = false;

   /***** Initialize new question to zero *****/
   Svy_InitQst (&SvyQst);

   /***** Get parameters from form *****/
   /* Get survey code */
   if ((SvyCod = Svy_GetParamSvyCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of survey is missing.");

   /* Get question code */
   SvyQst.QstCod = Svy_GetParamQstCod ();

   /* Get answer type */
   Par_GetParToText ("AnswerType",UnsignedStr,10);
   SvyQst.AnswerType = Svy_ConvertFromUnsignedStrToAnsTyp (UnsignedStr);

   /* Get question text */
   Par_GetParToHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);

   /* Get the texts of the answers */
   for (NumAns = 0;
	NumAns < Svy_MAX_ANSWERS_PER_QUESTION;
	NumAns++)
     {
      Svy_AllocateTextChoiceAnswer (&SvyQst,NumAns);
      sprintf (AnsStr,"AnsStr%u",NumAns);
      Par_GetParToHTML (AnsStr,SvyQst.AnsChoice[NumAns].Text,Svy_MAX_BYTES_ANSWER);
     }

   /***** Make sure that stem and answer are not empty *****/
   if (Txt[0])
     {
      if (SvyQst.AnsChoice[0].Text[0])	// If the first answer has been filled
        {
         for (NumAns = 0, NumLastAns = 0, ThereIsEndOfAnswers = false;
              !Error && NumAns < Svy_MAX_ANSWERS_PER_QUESTION;
              NumAns++)
            if (SvyQst.AnsChoice[NumAns].Text[0])
              {
               if (ThereIsEndOfAnswers)
                 {
                  Lay_ShowAlert (Lay_WARNING,Txt_You_can_not_leave_empty_intermediate_answers);
                  Error = true;
                 }
               else
                  NumLastAns = NumAns;
              }
            else
               ThereIsEndOfAnswers = true;
         if (!Error)
           {
            if (NumLastAns < 1)
              {
               Lay_ShowAlert (Lay_WARNING,Txt_You_must_type_at_least_the_first_two_answers);
               Error = true;
              }
           }
        }
      else	// If first answer is empty
        {
         Lay_ShowAlert (Lay_WARNING,Txt_You_must_type_at_least_the_first_two_answers);
         Error = true;
        }
     }
   else
     {
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_type_the_stem_of_the_question);
      Error = true;
     }

   if (Error)
      Svy_ShowFormEditOneQst (SvyCod,&SvyQst,Txt);
   else
     {
      /***** Form is received OK ==> insert question and answer in the database *****/
      if (SvyQst.QstCod < 0)	// It's a new question
        {
         SvyQst.QstInd = Svy_GetNextQuestionIndexInSvy (SvyCod);

         /* Insert question in the table of questions */
         sprintf (Query,"INSERT INTO svy_questions (SvyCod,QstInd,AnsType,Stem)"
                        " VALUES ('%ld','%u','%s','%s')",
	          SvyCod,SvyQst.QstInd,Svy_StrAnswerTypesDB[SvyQst.AnswerType],Txt);
         SvyQst.QstCod = DB_QueryINSERTandReturnCode (Query,"can not create question");
        }
      else			// It's an existing question
        {
         /* Update question */
         sprintf (Query,"UPDATE svy_questions SET Stem='%s',AnsType='%s'"
                        " WHERE QstCod='%ld' AND SvyCod='%ld'",
                  Txt,Svy_StrAnswerTypesDB[SvyQst.AnswerType],SvyQst.QstCod,SvyCod);
         DB_QueryUPDATE (Query,"can not update question");
        }

      /* Insert, update or delete answers in the answers table */
      for (NumAns = 0;
	   NumAns < Svy_MAX_ANSWERS_PER_QUESTION;
	   NumAns++)
         if (Svy_CheckIfAnswerExists (SvyQst.QstCod,NumAns))	// If this answer exists...
           {
            if (SvyQst.AnsChoice[NumAns].Text[0])	// Answer is not empty
              {
               /* Update answer text */
               sprintf (Query,"UPDATE svy_answers SET Answer='%s'"
                              " WHERE QstCod='%ld' AND AnsInd='%u'",
                        SvyQst.AnsChoice[NumAns].Text,SvyQst.QstCod,NumAns);
               DB_QueryUPDATE (Query,"can not update answer");
              }
            else	// Answer is empty
              {
               /* Delete answer from database */
               sprintf (Query,"DELETE FROM svy_answers"
                              " WHERE QstCod='%ld' AND AnsInd='%u'",
                        SvyQst.QstCod,NumAns);
               DB_QueryDELETE (Query,"can not delete answer");
              }
           }
         else	// If this answer does not exist...
           {
            if (SvyQst.AnsChoice[NumAns].Text[0])	// Answer is not empty
              {
               /* Create answer into database */
               sprintf (Query,"INSERT INTO svy_answers (QstCod,AnsInd,NumUsrs,Answer)"
                              " VALUES ('%ld','%u','0','%s')",
                        SvyQst.QstCod,NumAns,SvyQst.AnsChoice[NumAns].Text);
               DB_QueryINSERT (Query,"can not create answer");
              }
           }

      /***** List the questions of this survey, including the new one just inserted into the database *****/
      Lay_ShowAlert (Lay_SUCCESS,Txt_The_survey_has_been_modified);
     }

   /***** Free answers *****/
   Svy_FreeTextChoiceAnswers (&SvyQst,Svy_MAX_ANSWERS_PER_QUESTION);

   /***** Show current survey *****/
   Svy_ShowOneSurvey (SvyCod,&SvyQst,true);
  }

/*****************************************************************************/
/******************* Get next question index in a survey *********************/
/*****************************************************************************/

static unsigned Svy_GetQstIndFromQstCod (long QstCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned QstInd = 0;

   /***** Get number of surveys with a field value from database *****/
   sprintf (Query,"SELECT QstInd FROM svy_questions"
                  " WHERE QstCod='%ld'",
            QstCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get question index");

   /***** Get number of users *****/
   if (NumRows)
     {
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%u",&QstInd) != 1)
         Lay_ShowErrorAndExit ("Error when getting question index.");
     }
   else
      Lay_ShowErrorAndExit ("Error when getting question index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return QstInd;
  }

/*****************************************************************************/
/******************* Get next question index in a survey *********************/
/*****************************************************************************/

static unsigned Svy_GetNextQuestionIndexInSvy (long SvyCod)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned QstInd = 0;

   /***** Get number of surveys with a field value from database *****/
   sprintf (Query,"SELECT MAX(QstInd) FROM svy_questions"
                  " WHERE SvyCod='%ld'",
            SvyCod);
   DB_QuerySELECT (Query,&mysql_res,"can not get last question index");

   /***** Get number of users *****/
   row = mysql_fetch_row (mysql_res);
   if (row[0])	// There are questions
     {
      if (sscanf (row[0],"%u",&QstInd) != 1)
         Lay_ShowErrorAndExit ("Error when getting last question index.");
      QstInd++;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return QstInd;
  }

/*****************************************************************************/
/************************ List the questions of a survey *********************/
/*****************************************************************************/

static void Svy_ListSvyQuestions (struct Survey *Svy,struct SurveyQuestion *SvyQst)
  {
   extern const char *Hlp_STATS_Surveys_questions;
   extern const char *Txt_Questions;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Type;
   extern const char *Txt_Question;
   extern const char *Txt_SURVEY_STR_ANSWER_TYPES[Svy_NUM_ANS_TYPES];
   extern const char *Txt_This_survey_has_no_questions;
   extern const char *Txt_Done;
   extern const char *Txt_Edit_question;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQsts;
   unsigned NumQst;
   bool Editing = (Gbl.Action.Act == ActEdiOneSvy    ||
	           Gbl.Action.Act == ActEdiOneSvyQst ||
	           Gbl.Action.Act == ActRcvSvyQst);
   bool PutFormAnswerSurvey = Svy->Status.ICanAnswer && !Editing;

   /***** Get data of questions from database *****/
   sprintf (Query,"SELECT QstCod,QstInd,AnsType,Stem"
                  " FROM svy_questions WHERE SvyCod='%ld' ORDER BY QstInd",
            Svy->SvyCod);
   NumQsts = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get data of a question");

   /***** Start frame *****/
   Gbl.Svys.SvyCodToEdit = Svy->SvyCod;
   Lay_StartRoundFrame (NULL,Txt_Questions,
                        Svy->Status.ICanEdit ? Svy_PutIconToAddNewQuestion :
                                               NULL,
                        Hlp_STATS_Surveys_questions);

   if (NumQsts)
     {
      if (PutFormAnswerSurvey)
	{
	 /***** Start form to send answers to survey *****/
	 Act_FormStart (ActAnsSvy);
	 Svy_PutParamSvyCod (Svy->SvyCod);
	}

      /***** Write the heading *****/
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL CELLS_PAD_2\">"
                         "<tr>");
      if (Svy->Status.ICanEdit)
         fprintf (Gbl.F.Out,"<th colspan=\"2\"></th>");
      fprintf (Gbl.F.Out,"<th class=\"CENTER_TOP\">"
	                 "%s"
	                 "</th>"
                         "<th class=\"CENTER_TOP\">"
                         "%s"
                         "</th>"
                         "<th class=\"LEFT_TOP\">"
                         "%s"
                         "</th>"
                         "</tr>",
               Txt_No_INDEX,
               Txt_Type,
               Txt_Question);

      /***** Write questions one by one *****/
      for (NumQst = 0;
	   NumQst < NumQsts;
	   NumQst++)
        {
         Gbl.RowEvenOdd = (int) (NumQst % 2);

         row = mysql_fetch_row (mysql_res);

         /* row[0] holds the code of the question */
         if (sscanf (row[0],"%ld",&(SvyQst->QstCod)) != 1)
            Lay_ShowErrorAndExit ("Wrong code of question.");

         fprintf (Gbl.F.Out,"<tr>");

         if (Svy->Status.ICanEdit)
           {
            /* Write icon to remove the question */
            fprintf (Gbl.F.Out,"<td class=\"BT%u\">",Gbl.RowEvenOdd);
            Act_FormStart (ActReqRemSvyQst);
            Svy_PutParamSvyCod (Svy->SvyCod);
            Svy_PutParamQstCod (SvyQst->QstCod);
            Lay_PutIconRemove ();
            Act_FormEnd ();
            fprintf (Gbl.F.Out,"</td>");

            /* Write icon to edit the question */
            fprintf (Gbl.F.Out,"<td class=\"BT%u\">",Gbl.RowEvenOdd);
            Act_FormStart (ActEdiOneSvyQst);
            Svy_PutParamSvyCod (Svy->SvyCod);
            Svy_PutParamQstCod (SvyQst->QstCod);
            fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/edit64x64.png\""
        	               " alt=\"%s\" title=\"%s\""
        	               " class=\"ICO20x20\" />",
                     Gbl.Prefs.IconsURL,
                     Txt_Edit_question,
                     Txt_Edit_question);
            Act_FormEnd ();
            fprintf (Gbl.F.Out,"</td>");
           }

         /* Write index of question inside survey (row[1]) */
         if (sscanf (row[1],"%u",&(SvyQst->QstInd)) != 1)
            Lay_ShowErrorAndExit ("Error: wrong question index.");
         fprintf (Gbl.F.Out,"<td class=\"DAT_SMALL CENTER_TOP COLOR%u\">"
                            "%u"
                            "</td>",
                  Gbl.RowEvenOdd,SvyQst->QstInd+1);

         /* Write the question type (row[2]) */
         SvyQst->AnswerType = Svy_ConvertFromStrAnsTypDBToAnsTyp (row[2]);
         fprintf (Gbl.F.Out,"<td class=\"DAT_SMALL CENTER_TOP COLOR%u\">"
                            "%s"
                            "</td>",
	          Gbl.RowEvenOdd,
                  Txt_SURVEY_STR_ANSWER_TYPES[SvyQst->AnswerType]);

         /* Write the stem (row[3]) and the answers of this question */
         fprintf (Gbl.F.Out,"<td class=\"TEST_EDI LEFT_TOP COLOR%u\">",
	          Gbl.RowEvenOdd);
         Svy_WriteQstStem (row[3]);
         Svy_WriteAnswersOfAQst (Svy,SvyQst,PutFormAnswerSurvey);
         fprintf (Gbl.F.Out,"</td>"
                            "</tr>");
        }

      fprintf (Gbl.F.Out,"</table>");

      if (PutFormAnswerSurvey)
	{
	 /***** Button to create/modify survey *****/
	 Lay_PutConfirmButton (Txt_Done);

	 /***** End form *****/
	 Act_FormEnd ();
	}
     }
   else	// This survey has no questions
      Lay_ShowAlert (Lay_INFO,Txt_This_survey_has_no_questions);

   if (Svy->Status.ICanEdit &&	// I can edit
       (!NumQsts ||		// This survey has no questions
	Editing))		// I am editing
      /***** Put button to add a new question in this survey *****/
      Svy_PutButtonToCreateNewQuestion ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End frame *****/
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/***************** Put icon to add a new question to survey ******************/
/*****************************************************************************/

static void Svy_PutIconToAddNewQuestion (void)
  {
   extern const char *Txt_New_question;

   /***** Put form to create a new question *****/
   Lay_PutContextualLink (ActEdiOneSvyQst,Svy_PutParams,
                          "plus64x64.png",
                          Txt_New_question,NULL,
		          NULL);
  }

/*****************************************************************************/
/**************** Put button to add a new question to survey *****************/
/*****************************************************************************/

static void Svy_PutButtonToCreateNewQuestion (void)
  {
   extern const char *Txt_New_question;

   Act_FormStart (ActEdiOneSvyQst);
   Svy_PutParams ();
   Lay_PutConfirmButton (Txt_New_question);
   Act_FormEnd ();
  }

/*****************************************************************************/
/****************** Write the heading of a survey question *******************/
/*****************************************************************************/

static void Svy_WriteQstStem (const char *Stem)
  {
   unsigned long LengthHeading;
   char *HeadingRigorousHTML;

   /* Convert the stem, that is in HTML, to rigorous HTML */
   LengthHeading = strlen (Stem) * Str_MAX_LENGTH_SPEC_CHAR_HTML;
   if ((HeadingRigorousHTML = malloc (LengthHeading+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store stem of question.");
   strcpy (HeadingRigorousHTML,Stem);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     HeadingRigorousHTML,LengthHeading,false);

   /* Write the stem */
   fprintf (Gbl.F.Out,"%s",HeadingRigorousHTML);

   /* Free memory allocated for the stem */
   free ((void *) HeadingRigorousHTML);
  }

/*****************************************************************************/
/************** Get and write the answers of a survey question ***************/
/*****************************************************************************/

static void Svy_WriteAnswersOfAQst (struct Survey *Svy,struct SurveyQuestion *SvyQst,bool PutFormAnswerSurvey)
  {
   unsigned NumAns,NumAnswers;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrsThisAnswer;
   char *Answer;
   size_t AnsLength;

   NumAnswers = Svy_GetAnswersQst (SvyQst->QstCod,&mysql_res);	// Result: AnsInd,NumUsrs,Answer

   /***** Write the answers *****/
   if (NumAnswers)
     {
      fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_5\">");
      for (NumAns = 0;
	   NumAns < NumAnswers;
	   NumAns++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get number of users who have marked this answer (row[1]) */
	 if (sscanf (row[1],"%u",&NumUsrsThisAnswer) != 1)
	    Lay_ShowErrorAndExit ("Error when getting number of users who have marked an answer.");

	 /* Convert the answer (row[2]), that is in HTML, to rigorous HTML */
	 AnsLength = strlen (row[2]) * Str_MAX_LENGTH_SPEC_CHAR_HTML;
	 if ((Answer = malloc (AnsLength+1)) == NULL)
	    Lay_ShowErrorAndExit ("Not enough memory to store answer.");
	 strcpy (Answer,row[2]);
	 Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			   Answer,AnsLength,false);

	 /* Selectors and label with the letter of the answer */
	 fprintf (Gbl.F.Out,"<tr>");

	 if (PutFormAnswerSurvey)
	   {
	    /* Write selector to choice this answer */
	    fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\">"
			       "<input type=\"");
	    if (SvyQst->AnswerType == Svy_ANS_UNIQUE_CHOICE)
	       fprintf (Gbl.F.Out,"radio\""
				  " onclick=\"selectUnselectRadio(this,this.form.Ans%010u,%u)\"",
			(unsigned) SvyQst->QstCod,NumAnswers);
	    else // SvyQst->AnswerType == Svy_ANS_MULTIPLE_CHOICE
	       fprintf (Gbl.F.Out,"checkbox\"");
	    fprintf (Gbl.F.Out," name=\"Ans%010u\" value=\"%u\" />"
			       "</td>",
		     (unsigned) SvyQst->QstCod,NumAns);
	   }

	 /* Write the number of option */
	 fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_TOP\" style=\"width:50px;\">"
			    "%u)"
			    "</td>",
		  NumAns + 1);

	 /* Write the text of the answer */
	 fprintf (Gbl.F.Out,"<td class=\"TEST_EDI LEFT_TOP\">%s</td>",
		  Answer);

	 /* Show stats of this answer */
	 if (Svy->Status.ICanViewResults)
	    Svy_DrawBarNumUsrs (NumUsrsThisAnswer,Svy->NumUsrs);

	 fprintf (Gbl.F.Out,"</tr>");

	 /* Free memory allocated for the answer */
	 free ((void *) Answer);
	}
      fprintf (Gbl.F.Out,"</table>");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Draw a bar with the percentage of answers *****************/
/*****************************************************************************/

#define Svy_MAX_BAR_WIDTH 125

static void Svy_DrawBarNumUsrs (unsigned NumUsrs,unsigned MaxUsrs)
  {
   extern const char *Txt_of_PART_OF_A_TOTAL;
   unsigned BarWidth = 0;

   /***** String with the number of users *****/
   if (MaxUsrs)
      sprintf (Gbl.Title,"%u&nbsp;(%u%%&nbsp;%s&nbsp;%u)",
               NumUsrs,
               (unsigned) ((((float) NumUsrs * 100.0) / (float) MaxUsrs) + 0.5),
               Txt_of_PART_OF_A_TOTAL,MaxUsrs);
   else
      sprintf (Gbl.Title,"0&nbsp;(0%%&nbsp;%s&nbsp;%u)",
               Txt_of_PART_OF_A_TOTAL,MaxUsrs);

   /***** Draw bar with a with proportional to the number of clicks *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_TOP\" style=\"width:%upx;\">",
            Svy_MAX_BAR_WIDTH + 125);
   if (NumUsrs && MaxUsrs)
      BarWidth = (unsigned) ((((float) NumUsrs * (float) Svy_MAX_BAR_WIDTH) /
	                       (float) MaxUsrs) + 0.5);
   if (BarWidth < 2)
      BarWidth = 2;
   fprintf (Gbl.F.Out,"<img src=\"%s/c1x16.gif\""
	              " alt=\"%s\" title=\"%s\""
                      " class=\"LEFT_TOP\""
	              " style=\"width:%upx; height:20px;\" />"
	              "&nbsp;",
      Gbl.Prefs.IconsURL,
      Gbl.Title,
      Gbl.Title,
      BarWidth);

   /***** Write the number of users *****/
   fprintf (Gbl.F.Out,"%s</td>"
	              "</tr>",
            Gbl.Title);
  }

/*****************************************************************************/
/********************* Put icon to remove one question ***********************/
/*****************************************************************************/

static void Svy_PutIconToRemoveOneQst (void)
  {
   extern const char *Txt_Remove;

   Lay_PutContextualLink (ActReqRemSvyQst,Svy_PutParamsRemoveOneQst,
                          "remove-on64x64.png",
                          Txt_Remove,NULL,
		          NULL);
  }

/*****************************************************************************/
/****************** Put parameter to remove one question *********************/
/*****************************************************************************/

static void Svy_PutParamsRemoveOneQst (void)
  {
   Svy_PutParamSvyCod (Gbl.Svys.SvyCodToEdit);
   Svy_PutParamQstCod (Gbl.Svys.SvyQstCodToEdit);
  }

/*****************************************************************************/
/********************** Request the removal of a question ********************/
/*****************************************************************************/

void Svy_RequestRemoveQst (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_question_X;
   extern const char *Txt_Remove_question;
   long SvyCod;
   struct SurveyQuestion SvyQst;

   /***** Get parameters from form *****/
   /* Get survey code */
   if ((SvyCod = Svy_GetParamSvyCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of survey is missing.");

   /* Get question code */
   if ((SvyQst.QstCod = Svy_GetParamQstCod ()) < 0)
      Lay_ShowErrorAndExit ("Wrong code of question.");

   /* Get question index */
   SvyQst.QstInd = Svy_GetQstIndFromQstCod (SvyQst.QstCod);

   /***** Start form *****/
   Act_FormStart (ActRemSvyQst);
   Svy_PutParamSvyCod (SvyCod);
   Svy_PutParamQstCod (SvyQst.QstCod);

   /***** Ask for confirmation of removing *****/
   sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_question_X,
	    (unsigned long) (SvyQst.QstInd + 1));
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);
   Lay_PutRemoveButton (Txt_Remove_question);

   /***** End form *****/
   Act_FormEnd ();

   /***** Show current survey *****/
   Svy_ShowOneSurvey (SvyCod,&SvyQst,true);
  }

/*****************************************************************************/
/****************************** Remove a question ****************************/
/*****************************************************************************/

void Svy_RemoveQst (void)
  {
   extern const char *Txt_Question_removed;
   char Query[512];
   long SvyCod;
   struct SurveyQuestion SvyQst;

   /***** Get parameters from form *****/
   /* Get survey code */
   if ((SvyCod = Svy_GetParamSvyCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of survey is missing.");

   /* Get question code */
   if ((SvyQst.QstCod = Svy_GetParamQstCod ()) < 0)
      Lay_ShowErrorAndExit ("Wrong code of question.");

   /* Get question index */
   SvyQst.QstInd = Svy_GetQstIndFromQstCod (SvyQst.QstCod);

   /***** Remove the question from all the tables *****/
   /* Remove answers from this test question */
   Svy_RemAnswersOfAQuestion (SvyQst.QstCod);

   /* Remove the question itself */
   sprintf (Query,"DELETE FROM svy_questions"
                  " WHERE QstCod='%ld'",
            SvyQst.QstCod);
   DB_QueryDELETE (Query,"can not remove a question");
   if (!mysql_affected_rows (&Gbl.mysql))
      Lay_ShowErrorAndExit ("The question to be removed does not exist.");

   /* Change index of questions greater than this */
   sprintf (Query,"UPDATE svy_questions SET QstInd=QstInd-1"
                  " WHERE SvyCod='%ld' AND QstInd>'%u'",
            SvyCod,SvyQst.QstInd);
   DB_QueryUPDATE (Query,"can not update indexes of questions");

   /***** Write message *****/
   sprintf (Gbl.Message,"%s",Txt_Question_removed);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show current survey *****/
   Svy_ShowOneSurvey (SvyCod,&SvyQst,true);
  }

/*****************************************************************************/
/************************ Receive answers of a survey ************************/
/*****************************************************************************/

void Svy_ReceiveSurveyAnswers (void)
  {
   extern const char *Txt_You_already_answered_this_survey_before;
   extern const char *Txt_Thanks_for_answering_the_survey;
   struct Survey Svy;
   struct SurveyQuestion SvyQst;

   /***** Get survey code *****/
   if ((Svy.SvyCod = Svy_GetParamSvyCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of survey is missing.");

   /***** Get data of the survey from database *****/
   Svy_GetDataOfSurveyByCod (&Svy);

   /***** Check if I have no answered this survey formerly *****/
   if (Svy.Status.IHaveAnswered)
      Lay_ShowAlert (Lay_WARNING,Txt_You_already_answered_this_survey_before);
   else
     {
      /***** Receive and store user's answers *****/
      Svy_ReceiveAndStoreUserAnswersToASurvey (Svy.SvyCod);
      Lay_ShowAlert (Lay_INFO,Txt_Thanks_for_answering_the_survey);
     }

   /***** Show current survey *****/
   Svy_ShowOneSurvey (Svy.SvyCod,&SvyQst,true);
  }

/*****************************************************************************/
/**************** Get and store user's answers to a survey *******************/
/*****************************************************************************/

static void Svy_ReceiveAndStoreUserAnswersToASurvey (long SvyCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQst;
   unsigned NumQsts;
   long QstCod;
   char ParamName[3+10+6+1];
   char StrAnswersIndexes[Svy_MAX_ANSWERS_PER_QUESTION*(10+1)];
   const char *Ptr;
   char UnsignedStr[10+1];
   unsigned AnsInd;

   /***** Get questions of this survey from database *****/
   sprintf (Query,"SELECT QstCod FROM svy_questions"
                  " WHERE SvyCod='%ld' ORDER BY QstCod",
            SvyCod);
   DB_QuerySELECT (Query,&mysql_res,"can not get questions of a survey");

   if ((NumQsts = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get surveys"))) // The survey has questions
     {
      /***** Get questions *****/
      for (NumQst = 0;
	   NumQst < NumQsts;
	   NumQst++)
        {
         /* Get next answer */
         row = mysql_fetch_row (mysql_res);

         /* Get question code (row[0]) */
         if ((QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Lay_ShowErrorAndExit ("Error: wrong question code.");

         /* Get possible parameter with the user's answer */
         sprintf (ParamName,"Ans%010u",(unsigned) QstCod);
         // Lay_ShowAlert (Lay_INFO,ParamName);
         Par_GetParMultiToText (ParamName,StrAnswersIndexes,Svy_MAX_ANSWERS_PER_QUESTION*(10+1));
         Ptr = StrAnswersIndexes;
         while (*Ptr)
           {
            Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,10);
            if (sscanf (UnsignedStr,"%u",&AnsInd) == 1)
               // Parameter exists, so user has marked this answer, so store it in database
               Svy_IncreaseAnswerInDB (QstCod,AnsInd);
           }
        }
     }
   else		// The survey has no questions and answers
      Lay_ShowErrorAndExit ("Error: this survey has no questions.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Register that you have answered this survey *****/
   Svy_RegisterIHaveAnsweredSvy (SvyCod);
  }

/*****************************************************************************/
/************ Increase number of users who have marked one answer ************/
/*****************************************************************************/

static void Svy_IncreaseAnswerInDB (long QstCod,unsigned AnsInd)
  {
   char Query[512];

   /***** Increase number of users who have selected the answer AnsInd in the question QstCod *****/
   sprintf (Query,"UPDATE svy_answers SET NumUsrs=NumUsrs+1"
                  " WHERE QstCod='%ld' AND AnsInd='%u'",
            QstCod,AnsInd);
   DB_QueryINSERT (Query,"can not register your answer to the survey");
  }

/*****************************************************************************/
/***************** Register that I have answered this survey *****************/
/*****************************************************************************/

static void Svy_RegisterIHaveAnsweredSvy (long SvyCod)
  {
   char Query[512];

   sprintf (Query,"INSERT INTO svy_users (SvyCod,UsrCod)"
                  " VALUES ('%ld','%ld')",
            SvyCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryINSERT (Query,"can not register that you have answered the survey");
  }

/*****************************************************************************/
/************** Register that you have answered this survey ******************/
/*****************************************************************************/

static bool Svy_CheckIfIHaveAnsweredSvy (long SvyCod)
  {
   char Query[512];

   /***** Get number of surveys with a field value from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM svy_users"
                  " WHERE SvyCod='%ld' AND UsrCod='%ld'",
            SvyCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if you have answered a survey") != 0);
  }

/*****************************************************************************/
/*************** Register that you have answered this survey *****************/
/*****************************************************************************/

static unsigned Svy_GetNumUsrsWhoHaveAnsweredSvy (long SvyCod)
  {
   char Query[512];

   /***** Get number of surveys with a field value from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM svy_users"
                  " WHERE SvyCod='%ld'",
            SvyCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of users who have answered a survey");
  }

/*****************************************************************************/
/******************** Get number of courses with surveys *********************/
/*****************************************************************************/
// Returns the number of courses with surveys for courses
// in this location (all the platform, current degree or current course)

unsigned Svy_GetNumCoursesWithCrsSurveys (Sco_Scope_t Scope)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with surveys from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(DISTINCT Cod)"
                        " FROM surveys"
                        " WHERE Scope='%s'",
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(DISTINCT surveys.Cod)"
                        " FROM institutions,centres,degrees,courses,surveys"
			" WHERE institutions.CtyCod='%ld'"
			" AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=surveys.Cod"
                        " AND surveys.Scope='%s'",
                  Gbl.CurrentIns.Ins.InsCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(DISTINCT surveys.Cod)"
                        " FROM centres,degrees,courses,surveys"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=surveys.Cod"
                        " AND surveys.Scope='%s'",
                  Gbl.CurrentIns.Ins.InsCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(DISTINCT surveys.Cod)"
                        " FROM degrees,courses,surveys"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=surveys.Cod"
                        " AND surveys.Scope='%s'",
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(DISTINCT surveys.Cod)"
                        " FROM courses,surveys"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=surveys.Cod"
                        " AND surveys.Scope='%s'",
                  Gbl.CurrentDeg.Deg.DegCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(DISTINCT Cod)"
                        " FROM surveys"
                        " WHERE Scope='%s' AND Cod='%ld'",
                  Sco_ScopeDB[Sco_SCOPE_CRS],
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of courses with surveys");

   /***** Get number of surveys *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumCourses) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of courses with surveys.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCourses;
  }

/*****************************************************************************/
/******************** Get number of surveys for courses **********************/
/*****************************************************************************/
// Returns the number of surveys for courses
// in this location (all the platform, current degree or current course)

unsigned Svy_GetNumCrsSurveys (Sco_Scope_t Scope,unsigned *NumNotif)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumSurveys;

   /***** Get number of surveys from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM surveys"
                        " WHERE Scope='%s'",
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(*),SUM(surveys.NumNotif)"
                        " FROM institutions,centres,degrees,courses,surveys"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=surveys.Cod"
                        " AND surveys.Scope='%s'",
                  Gbl.CurrentCty.Cty.CtyCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(*),SUM(surveys.NumNotif)"
                        " FROM centres,degrees,courses,surveys"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=surveys.Cod"
                        " AND surveys.Scope='%s'",
                  Gbl.CurrentIns.Ins.InsCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(*),SUM(surveys.NumNotif)"
                        " FROM degrees,courses,surveys"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=surveys.Cod"
                        " AND surveys.Scope='%s'",
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(*),SUM(surveys.NumNotif)"
                        " FROM courses,surveys"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=surveys.Cod"
                        " AND surveys.Scope='%s'",
                  Gbl.CurrentDeg.Deg.DegCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM surveys"
                        " WHERE surveys.Scope='%s'"
                        " AND CrsCod='%ld'",
                  Sco_ScopeDB[Sco_SCOPE_CRS],
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of surveys");

   /***** Get number of surveys *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumSurveys) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of surveys.");

   /***** Get number of notifications by email *****/
   if (row[1])
     {
      if (sscanf (row[1],"%u",NumNotif) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of notifications of surveys.");
     }
   else
      *NumNotif = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumSurveys;
  }

/*****************************************************************************/
/************ Get average number of questions per course survey **************/
/*****************************************************************************/

float Svy_GetNumQstsPerCrsSurvey (Sco_Scope_t Scope)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   float NumQstsPerSurvey;

   /***** Get number of questions per survey from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
                        " FROM surveys,svy_questions"
                        " WHERE surveys.Scope='%s'"
                        " AND surveys.SvyCod=svy_questions.SvyCod"
                        " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
                        " FROM institutions,centres,degrees,courses,surveys,svy_questions"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=surveys.Cod"
                        " AND surveys.Scope='%s'"
                        " AND surveys.SvyCod=svy_questions.SvyCod"
                        " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
                  Gbl.CurrentCty.Cty.CtyCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
                        " FROM centres,degrees,courses,surveys,svy_questions"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=surveys.Cod"
                        " AND surveys.Scope='%s'"
                        " AND surveys.SvyCod=svy_questions.SvyCod"
                        " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
                  Gbl.CurrentIns.Ins.InsCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
                        " FROM degrees,courses,surveys,svy_questions"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=surveys.Cod"
                        " AND surveys.Scope='%s'"
                        " AND surveys.SvyCod=svy_questions.SvyCod"
                        " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
                        " FROM courses,surveys,svy_questions"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=surveys.Cod"
                        " AND surveys.Scope='%s'"
                        " AND surveys.SvyCod=svy_questions.SvyCod"
                        " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
                  Gbl.CurrentDeg.Deg.DegCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
                        " FROM surveys,svy_questions"
                        " WHERE surveys.Scope='%s' AND surveys.Cod='%ld'"
                        " AND surveys.SvyCod=svy_questions.SvyCod"
                        " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
                  Sco_ScopeDB[Sco_SCOPE_CRS],Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of questions per survey");

   /***** Get number of courses *****/
   row = mysql_fetch_row (mysql_res);
   NumQstsPerSurvey = Str_GetFloatNumFromStr (row[0]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumQstsPerSurvey;
  }
