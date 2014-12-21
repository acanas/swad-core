// swad_survey2.c: surveys

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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
static void Svy_PutFormToSelectWhichGroupsToShow (void);
static void Svy_ShowOneSurvey (long SvyCod,struct SurveyQuestion *SvyQst,bool ShowOnlyThisSvyComplete);
static void Svy_WriteAuthor (struct Survey *Svy);
static void Svy_WriteStatus (struct Survey *Svy);
static void Svy_GetParamSvyOrderType (void);
static void Svy_PutFormToCreateNewSvy (void);
static void Svy_PutFormsToRemEditOneSvy (long SvyCod,bool Visible);
static void Svy_GetSurveyTxtFromDB (long SvyCod,char *Txt);
static void Svy_PutParamSvyCod (long SvyCod);
static long Svy_GetParamSvyCod (void);
static bool Svy_CheckIfSimilarSurveyExists (struct Survey *Svy);
static bool Svy_SetDefaultAndAllowedForEdition (void);
static void Svy_ShowLstGrpsToEditSurvey (long SvyCod);
static void Svy_UpdateNumUsrsNotifiedByEMailAboutSurvey (long SvyCod,unsigned NumUsrsToBeNotifiedByEMail);
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
static void Svy_WriteParamEditQst (struct SurveyQuestion *SvyQst);
static void Svy_WriteQstStem (const char *Stem,const char *TextStyle);
static void Svy_WriteAnswersOfAQst (struct Survey *Svy,struct SurveyQuestion *SvyQst);
static void Svy_DrawBarNumUsrs (unsigned NumUsrs,unsigned MaxUsrs);
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
   extern const char *Txt_ASG_ATT_OR_SVY_HELP_ORDER[2];
   extern const char *Txt_ASG_ATT_OR_SVY_ORDER[2];
   extern const char *Txt_Survey;
   extern const char *Txt_Status;
   extern const char *Txt_No_surveys;
   tSvysOrderType Order;
   struct Pagination Pagination;
   unsigned NumSvy;

   /***** Put link (form) to create new survey *****/
   if (Svy_CheckIfICanCreateSvy ())
      Svy_PutFormToCreateNewSvy ();

   /***** Get number of groups in current course *****/
   if (!Gbl.CurrentCrs.Grps.NumGrps)
      Gbl.CurrentCrs.Grps.WhichGrps = Grp_ALL_GROUPS;

   /***** Get list of surveys *****/
   Svy_GetListSurveys ();

   /***** Select whether show only my groups or all groups *****/
   if (Gbl.CurrentCrs.Grps.NumGrps)
      Svy_PutFormToSelectWhichGroupsToShow ();

   if (Gbl.Svys.Num)	// There are surveys in current course
     {
      /***** Compute variables related to pagination *****/
      Pagination.NumItems = Gbl.Svys.Num;
      Pagination.CurrentPage = (int) Gbl.Pag.CurrentPage;
      Pag_CalculatePagination (&Pagination);
      Gbl.Pag.CurrentPage = (unsigned) Pagination.CurrentPage;

      /***** Write links to pages *****/
      if (Pagination.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (Pag_SURVEYS,0,&Pagination);

      /***** Table head *****/
      Lay_StartRoundFrameTable10 (NULL,2,NULL);
      fprintf (Gbl.F.Out,"<tr>");
      for (Order = Svy_ORDER_BY_START_DATE;
	   Order <= Svy_ORDER_BY_END_DATE;
	   Order++)
        {
         fprintf (Gbl.F.Out,"<th align=\"center\" class=\"TIT_TBL\">");
         Act_FormStart (ActSeeAllSvy);
         Grp_PutParamWhichGrps ();
         Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
         Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
         Act_LinkFormSubmit (Txt_ASG_ATT_OR_SVY_HELP_ORDER[Order],"TIT_TBL");
         if (Order == Gbl.Svys.SelectedOrderType)
            fprintf (Gbl.F.Out,"<u>");
         fprintf (Gbl.F.Out,"%s",Txt_ASG_ATT_OR_SVY_ORDER[Order]);
         if (Order == Gbl.Svys.SelectedOrderType)
            fprintf (Gbl.F.Out,"</u>");
         fprintf (Gbl.F.Out,"</a>"
                            "</form>"
                            "</th>");
        }
      fprintf (Gbl.F.Out,"<th align=\"center\" class=\"TIT_TBL\">%s</th>"
                         "<th align=\"center\" class=\"TIT_TBL\">%s</th>"
                         "</tr>",
               Txt_Survey,
               Txt_Status);

      /***** Write all the surveys *****/
      for (NumSvy = Pagination.FirstItemVisible;
           NumSvy <= Pagination.LastItemVisible;
           NumSvy++)
         Svy_ShowOneSurvey (Gbl.Svys.LstSvyCods[NumSvy-1],SvyQst,false);

      /***** Table end *****/
      Lay_EndRoundFrameTable10 ();

      /***** Write again links to pages *****/
      if (Pagination.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (Pag_SURVEYS,0,&Pagination);
     }
   else	// There are no surveys in current course
      Lay_ShowAlert (Lay_INFO,Txt_No_surveys);

   /***** Free list of surveys *****/
   Svy_FreeListSurveys ();
  }

/*****************************************************************************/
/***************** Put form to select which groups to show *******************/
/*****************************************************************************/

static bool Svy_CheckIfICanCreateSvy (void)
  {
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_TEACHER:
         return (Gbl.CurrentCrs.Crs.CrsCod > 0);
      case Rol_ROLE_DEG_ADMIN:
         return (Gbl.CurrentDeg.Deg.DegCod > 0);
      case Rol_ROLE_SUPERUSER:
         return true;
      default:
         return false;
     }
   return false;
  }

/*****************************************************************************/
/***************** Put form to select which groups to show *******************/
/*****************************************************************************/

static void Svy_PutFormToSelectWhichGroupsToShow (void)
  {
   fprintf (Gbl.F.Out,"<div align=\"center\">");
   Act_FormStart (ActSeeAllSvy);
   Svy_PutHiddenParamSvyOrderType ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   Grp_ShowSelectorWhichGrps ();
   fprintf (Gbl.F.Out,"</form>"
	              "</div>");
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
   Lay_StartRoundFrameTable10 (NULL,2,NULL);
   Svy_ShowOneSurvey (Svy.SvyCod,&SvyQst,true);
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/****************************** Show one survey ******************************/
/*****************************************************************************/

static void Svy_ShowOneSurvey (long SvyCod,struct SurveyQuestion *SvyQst,bool ShowOnlyThisSvyComplete)
  {
   extern const char *Txt_View_survey;
   extern const char *Txt_No_of_questions;
   extern const char *Txt_No_of_users;
   extern const char *Txt_Scope;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;
   extern const char *Txt_Users;
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Answer_survey;
   extern const char *Txt_View_survey_results;
   struct Survey Svy;
   Rol_Role_t Role;
   bool RolesSelected;
   char Txt[Cns_MAX_BYTES_TEXT+1];

   /***** Get data of this survey *****/
   Svy.SvyCod = SvyCod;
   Svy_GetDataOfSurveyByCod (&Svy);

   /***** Start date/time *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"left\" valign=\"top\" class=\"%s\"",
            Svy.Status.Visible ? (Svy.Status.Open ? "DATE_GREEN" :
        	                                    "DATE_RED") :
                                 (Svy.Status.Open ? "DATE_GREEN_LIGHT" :
                                                    "DATE_RED_LIGHT"));
   if (!ShowOnlyThisSvyComplete)
      fprintf (Gbl.F.Out," bgcolor=\"%s\"",Gbl.ColorRows[Gbl.RowEvenOdd]);
   fprintf (Gbl.F.Out,">%02u/%02u/%02u<br />%02u:%02u h</td>",
            Svy.DateTimes[Svy_START_TIME].Date.Day,
            Svy.DateTimes[Svy_START_TIME].Date.Month,
	    Svy.DateTimes[Svy_START_TIME].Date.Year % 100,
	    Svy.DateTimes[Svy_START_TIME].Time.Hour,
	    Svy.DateTimes[Svy_START_TIME].Time.Minute);

   /***** End date/time *****/
   fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"top\" class=\"%s\"",
            Svy.Status.Visible ? (Svy.Status.Open ? "DATE_GREEN" :
        	                                    "DATE_RED") :
                                 (Svy.Status.Open ? "DATE_GREEN_LIGHT" :
                                                    "DATE_RED_LIGHT"));
   if (!ShowOnlyThisSvyComplete)
      fprintf (Gbl.F.Out," bgcolor=\"%s\"",Gbl.ColorRows[Gbl.RowEvenOdd]);
   fprintf (Gbl.F.Out,">%02u/%02u/%02u<br />%02u:%02u h</td>",
            Svy.DateTimes[Svy_END_TIME  ].Date.Day,
            Svy.DateTimes[Svy_END_TIME  ].Date.Month,
            Svy.DateTimes[Svy_END_TIME  ].Date.Year % 100,
	    Svy.DateTimes[Svy_END_TIME  ].Time.Hour,
	    Svy.DateTimes[Svy_END_TIME  ].Time.Minute);

   /***** Survey title *****/
   fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"top\"");
   if (!ShowOnlyThisSvyComplete)
      fprintf (Gbl.F.Out," bgcolor=\"%s\"",Gbl.ColorRows[Gbl.RowEvenOdd]);
   fprintf (Gbl.F.Out,">");

   /* Put form to view survey */
   Act_FormStart (ActSeeOneSvy);
   Svy_PutParamSvyCod (SvyCod);
   Svy_PutHiddenParamSvyOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   Act_LinkFormSubmit (Txt_View_survey,
                       Svy.Status.Visible ? "ASG_TITLE" :
	                                    "ASG_TITLE_LIGHT");
   fprintf (Gbl.F.Out,"%s</a>"
	              "</form>",
            Svy.Title);

   /* Number of questions and number of distinct users who have already answered this survey */
   fprintf (Gbl.F.Out,"<p class=\"%s\">%s: %u; %s: %u</p></td>",
            Svy.Status.Visible ? "ASG_GRP" :
        	                 "ASG_GRP_LIGHT",
            Txt_No_of_questions,
            Svy.NumQsts,
            Txt_No_of_users,
            Svy.NumUsrs);

   /***** Status of the survey *****/
   fprintf (Gbl.F.Out,"<td rowspan=\"2\" align=\"left\" valign=\"top\"");
   if (!ShowOnlyThisSvyComplete)
      fprintf (Gbl.F.Out," bgcolor=\"%s\"",Gbl.ColorRows[Gbl.RowEvenOdd]);
   fprintf (Gbl.F.Out,">");
   Svy_WriteStatus (&Svy);

   if (!ShowOnlyThisSvyComplete)
     {
      /* Possible button to answer this survey */
      if (Svy.Status.ICanAnswer)
        {
         Act_FormStart (ActSeeOneSvy);
         Svy_PutParamSvyCod (Svy.SvyCod);
         Svy_PutHiddenParamSvyOrderType ();
         Grp_PutParamWhichGrps ();
         Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
         Lay_PutSendButton (Txt_Answer_survey);
         fprintf (Gbl.F.Out,"</form>");
        }
      /* Possible button to see the result of the survey */
      else if (Svy.Status.ICanViewResults)
        {
         Act_FormStart (ActSeeOneSvy);
         Svy_PutParamSvyCod (Svy.SvyCod);
         Svy_PutHiddenParamSvyOrderType ();
         Grp_PutParamWhichGrps ();
         Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
         Lay_PutSendButton (Txt_View_survey_results);
         fprintf (Gbl.F.Out,"</form>");
        }
     }

   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Write second row of data of this survey *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" align=\"left\" valign=\"top\"");
   if (!ShowOnlyThisSvyComplete)
      fprintf (Gbl.F.Out," bgcolor=\"%s\"",Gbl.ColorRows[Gbl.RowEvenOdd]);
   fprintf (Gbl.F.Out,">");

   /* Author of the survey */
   Svy_WriteAuthor (&Svy);

   /* Forms to remove/edit this survey */
   if (Svy.Status.ICanEdit)
      Svy_PutFormsToRemEditOneSvy (Svy.SvyCod,Svy.Status.Visible);

   fprintf (Gbl.F.Out,"</td>"
                      "<td align=\"left\" valign=\"top\"");
   if (!ShowOnlyThisSvyComplete)
      fprintf (Gbl.F.Out," bgcolor=\"%s\"",Gbl.ColorRows[Gbl.RowEvenOdd]);
   fprintf (Gbl.F.Out,">");

   /* Scope of the survey */
   fprintf (Gbl.F.Out,"<p class=\"%s\">%s: ",
            Svy.Status.Visible ? "ASG_GRP" :
        	                 "ASG_GRP_LIGHT",
            Txt_Scope);
   if (Svy.CrsCod > 0)
      fprintf (Gbl.F.Out,"%s %s",
               Txt_Course,Gbl.CurrentCrs.Crs.ShortName);
   else if (Svy.DegCod > 0)
      fprintf (Gbl.F.Out,"%s %s",
               Txt_Degree,Gbl.CurrentDeg.Deg.ShortName);
   else
      fprintf (Gbl.F.Out,"%s",Cfg_PLATFORM_SHORT_NAME);
   fprintf (Gbl.F.Out,"</p>");

   /* Users' roles who can answer the survey */
   fprintf (Gbl.F.Out,"<p class=\"%s\">%s:",
            Svy.Status.Visible ? "ASG_GRP" :
        	                 "ASG_GRP_LIGHT",
            Txt_Users);
   for (Role = Rol_ROLE_STUDENT, RolesSelected = false;
	Role <= Rol_ROLE_TEACHER;
	Role++)
     {
      if (RolesSelected)
         fprintf (Gbl.F.Out,",");
      else
         RolesSelected = true;
      fprintf (Gbl.F.Out,"<input type=\"checkbox\" name=\"Roles\" value=\"%u\" disabled=\"disabled\"",
               (unsigned) Role);
      if (Svy.Roles & (1 << Role))
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," />%s",Txt_ROLES_PLURAL_abc[Role][Usr_SEX_UNKNOWN]);
     }
   fprintf (Gbl.F.Out,"</p>");

   /* Groups whose users can answer this survey */
   if (Svy.CrsCod > 0)
      if (Gbl.CurrentCrs.Grps.NumGrps)
         Svy_GetAndWriteNamesOfGrpsAssociatedToSvy (&Svy);

   /* Text of the survey */
   Svy_GetSurveyTxtFromDB (Svy.SvyCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML
   Str_InsertLinkInURLs (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   fprintf (Gbl.F.Out,"<p align=\"justify\" class=\"%s\">"
                      "<br />%s<br />&nbsp;</p></td>"
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
   if (Svy.CrsCod > 0)	// Only course surveys are notified
      Ntf_SetNotifAsSeen (Ntf_EVENT_SURVEY,
	                  SvyCod,
	                  Gbl.Usrs.Me.UsrDat.UsrCod);
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
   Pho_ShowUsrPhoto (&UsrDat,
                     ShowPhoto ? PhotoURL :
                	         NULL,
                     12,16,true);

   /***** Write name *****/
   strcpy (FirstName,UsrDat.FirstName);
   strcpy (Surnames,UsrDat.Surname1);
   if (UsrDat.Surname2[0])
     {
      strcat (Surnames," ");
      strcat (Surnames,UsrDat.Surname2);
     }
   Str_LimitLengthHTMLStr (FirstName,9);
   Str_LimitLengthHTMLStr (Surnames,9);
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
   extern const char *Txt_SURVEY_You_belong_to_degree_coruse_or_groups;
   extern const char *Txt_SURVEY_You_dont_belong_to_degree_coruse_or_groups;
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
   if (Svy->Status.IBelongToDegCrsGrps)
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Svy->Status.Visible ? "STATUS_GREEN" :
        	                     "STATUS_GREEN_LIGHT",
               Txt_SURVEY_You_belong_to_degree_coruse_or_groups);
   else
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Svy->Status.Visible ? "STATUS_RED" :
        	                     "STATUS_RED_LIGHT",
               Txt_SURVEY_You_dont_belong_to_degree_coruse_or_groups);

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

   Par_GetParToText ("Order",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      Gbl.Svys.SelectedOrderType = (tSvysOrderType) UnsignedNum;
   else
      Gbl.Svys.SelectedOrderType = Svy_ORDER_BY_START_DATE;
  }

/*****************************************************************************/
/***** Put a hidden parameter with the type of order in list of surveys ******/
/*****************************************************************************/

void Svy_PutHiddenParamSvyOrderType (void)
  {
   Par_PutHiddenParamUnsigned ("Order",(unsigned) Gbl.Svys.SelectedOrderType);
  }

/*****************************************************************************/
/********************* Put a link (form) to edit surveys *********************/
/*****************************************************************************/

static void Svy_PutFormToCreateNewSvy (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_New_survey;

   fprintf (Gbl.F.Out,"<div align=\"center\">");
   Act_FormStart (ActFrmNewSvy);
   Svy_PutHiddenParamSvyOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   Act_LinkFormSubmit (Txt_New_survey,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("new",Txt_New_survey,Txt_New_survey);
   fprintf (Gbl.F.Out,"</form>"
	              "</div>");
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

   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_4\">"
                      "<tr>");

   /***** Put form to remove survey *****/
   fprintf (Gbl.F.Out,"<td align=\"left\">");
   Act_FormStart (ActReqRemSvy);
   Svy_PutParamSvyCod (SvyCod);
   Svy_PutHiddenParamSvyOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/delon16x16.gif\""
	              " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />"
                      "</form>"
                      "</td>",
            Gbl.Prefs.IconsURL,
            Txt_Remove,
            Txt_Remove);

   /***** Put form to reset survey *****/
   fprintf (Gbl.F.Out,"<td align=\"left\">");
   Act_FormStart (ActReqRstSvy);
   Svy_PutParamSvyCod (SvyCod);
   Svy_PutHiddenParamSvyOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/reset16x16.gif\""
	              " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />"
                      "</form>"
                      "</td>",
            Gbl.Prefs.IconsURL,
            Txt_Reset,
            Txt_Reset);

   /***** Put form to hide/show survey *****/
   fprintf (Gbl.F.Out,"<td align=\"left\">");
   Act_FormStart (Visible ? ActHidSvy :
	                    ActShoSvy);
   Svy_PutParamSvyCod (SvyCod);
   Svy_PutHiddenParamSvyOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   if (Visible)
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/visible_on16x16.gif\""
			 " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />",
	       Gbl.Prefs.IconsURL,
	       Txt_Hide,Txt_Hide);
   else
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/hidden_on16x16.gif\""
			 " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />",
	       Gbl.Prefs.IconsURL,
	       Txt_Show,Txt_Show);
   fprintf (Gbl.F.Out,"</form>"
                      "</td>");

   /***** Put form to edit survey *****/
   fprintf (Gbl.F.Out,"<td align=\"left\">");
   Act_FormStart (ActEdiOneSvy);
   Svy_PutParamSvyCod (SvyCod);
   Svy_PutHiddenParamSvyOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/edit16x16.gif\""
	              " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />"
                      "</form>"
                      "</td>",
            Gbl.Prefs.IconsURL,
            Txt_Edit,
            Txt_Edit);

   fprintf (Gbl.F.Out,"</tr>"
                      "</table>");
  }

/*****************************************************************************/
/*********************** Get list of all the surveys *************************/
/*****************************************************************************/

void Svy_GetListSurveys (void)
  {
   char HiddenSubQuery[256];
   char OrderBySubQuery[256];
   char Query[2048];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumSvy;

   if (Gbl.Svys.LstIsRead)
      Svy_FreeListSurveys ();

   /***** Get list of surveys from database *****/
   switch (Gbl.Svys.SelectedOrderType)
     {
      case Svy_ORDER_BY_START_DATE:
         sprintf (OrderBySubQuery,"StartTime DESC,EndTime DESC,Title DESC");
         break;
      case Svy_ORDER_BY_END_DATE:
         sprintf (OrderBySubQuery,"EndTime DESC,StartTime DESC,Title DESC");
         break;
     }

   if (Gbl.CurrentDeg.Deg.DegCod < 0)	// If no degree selected
     {
      switch (Gbl.Usrs.Me.LoggedRole)
        {
         case Rol_ROLE_SUPERUSER:
            HiddenSubQuery[0] = '\0';			// Show all surveys, visible or hidden
            break;
         default:
            sprintf (HiddenSubQuery," AND Hidden='N'");	// Show only visible surveys
            break;
        }
      sprintf (Query,"SELECT SvyCod"
                     " FROM surveys"
                     " WHERE DegCod='-1' AND CrsCod='-1'%s"
                     " ORDER BY %s",
               HiddenSubQuery,
               OrderBySubQuery);
     }
   else if ((Gbl.CurrentDeg.Deg.DegCod > 0 && Gbl.CurrentCrs.Crs.CrsCod < 0) ||		// If degree selected, but no course selected
             Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADMIN)				// or if I am a degree administrator
     {
      switch (Gbl.Usrs.Me.LoggedRole)
        {
         case Rol_ROLE_DEG_ADMIN:
         case Rol_ROLE_SUPERUSER:
            HiddenSubQuery[0] = '\0';			// Show all surveys, visible or hidden
            break;
         default:
            sprintf (HiddenSubQuery," AND Hidden='N'");	// Show only visible surveys
            break;
        }
      sprintf (Query,"SELECT SvyCod"
                     " FROM surveys"
                     " WHERE ((DegCod='-1' AND CrsCod='-1')"
                     " OR (DegCod='%ld' AND CrsCod='-1'))%s"
                     " ORDER BY %s",
               Gbl.CurrentDeg.Deg.DegCod,
               HiddenSubQuery,
               OrderBySubQuery);
     }
   else	if (Gbl.CurrentCrs.Crs.CrsCod > 0 &&
            Gbl.Usrs.Me.LoggedRole != Rol_ROLE_DEG_ADMIN)
     {
      switch (Gbl.Usrs.Me.LoggedRole)
        {
         case Rol_ROLE_TEACHER:
         case Rol_ROLE_SUPERUSER:
            HiddenSubQuery[0] = '\0';			// Show all surveys, visible or hidden
            break;
         default:
            sprintf (HiddenSubQuery," AND Hidden='N'");	// Show only visible surveys
            break;
        }
      if (Gbl.CurrentCrs.Grps.WhichGrps == Grp_ONLY_MY_GROUPS)
         sprintf (Query,"SELECT SvyCod"
                        " FROM surveys"
                        " WHERE ((DegCod='-1' AND CrsCod='-1')"
                        " OR (DegCod='%ld' AND CrsCod='-1')"
                        " OR (CrsCod='%ld'"
                        " AND (SvyCod NOT IN (SELECT SvyCod FROM svy_grp) OR"
                        " SvyCod IN (SELECT svy_grp.SvyCod FROM svy_grp,crs_grp_usr"
                        " WHERE crs_grp_usr.UsrCod='%ld' AND svy_grp.GrpCod=crs_grp_usr.GrpCod))))%s"
                        " ORDER BY %s",
                  Gbl.CurrentDeg.Deg.DegCod,
                  Gbl.CurrentCrs.Crs.CrsCod,
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  HiddenSubQuery,OrderBySubQuery);
      else	// Gbl.CurrentCrs.Grps.WhichGrps == Grp_ALL_GROUPS
         sprintf (Query,"SELECT SvyCod"
                        " FROM surveys"
                        " WHERE ((DegCod='-1' AND CrsCod='-1')"
                        " OR (DegCod='%ld' AND CrsCod='-1')"
                        " OR CrsCod='%ld')%s"
                        " ORDER BY %s",
                  Gbl.CurrentDeg.Deg.DegCod,
                  Gbl.CurrentCrs.Crs.CrsCod,
                  HiddenSubQuery,OrderBySubQuery);
     }
   else
      Lay_ShowErrorAndExit ("Can not get list of surveys.");

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
/********************* Get survey data using its code ************************/
/*****************************************************************************/

void Svy_GetDataOfSurveyByCod (struct Survey *Svy)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Build query *****/
   sprintf (Query,"SELECT SvyCod,DegCod,CrsCod,Hidden,Roles,UsrCod,"
                  "DATE_FORMAT(StartTime,'%%Y%%m%%d%%H%%i%%S'),"
                  "DATE_FORMAT(EndTime,'%%Y%%m%%d%%H%%i%%S'),"
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

      /* Get code of the degree (row[1]) */
      Svy->DegCod = Str_ConvertStrCodToLongCod (row[1]);
      if (Svy->DegCod > 0)
         if (Svy->DegCod != Gbl.CurrentDeg.Deg.DegCod)
            Lay_ShowErrorAndExit ("Wrong survey degree.");

      /* Get code of the course (row[2]) */
      Svy->CrsCod = Str_ConvertStrCodToLongCod (row[2]);
      if (Svy->CrsCod > 0)
         if (Svy->CrsCod != Gbl.CurrentCrs.Crs.CrsCod)
            Lay_ShowErrorAndExit ("Wrong survey course.");

      /* Get whether the survey is hidden (row[3]) */
      Svy->Status.Visible = (Str_ConvertToUpperLetter (row[3][0]) == 'N');

      /* Get roles (row[4]) */
      if (sscanf (row[4],"%u",&Svy->Roles) != 1)
      	 Lay_ShowErrorAndExit ("Error when reading roles of survey.");

      /* Get author of the survey (row[5]) */
      Svy->UsrCod = Str_ConvertStrCodToLongCod (row[5]);

      /* Get start date (row[6] holds the start date in YYYYMMDDHHMMSS format) */
      if (!(Dat_GetDateTimeFromYYYYMMDDHHMMSS (&(Svy->DateTimes[Svy_START_TIME]),row[6])))
	 Lay_ShowErrorAndExit ("Error when reading start date of survey.");

      /* Get end date (row[7] holds the end date in YYYYMMDDHHMMSS format) */
      if (!(Dat_GetDateTimeFromYYYYMMDDHHMMSS (&(Svy->DateTimes[Svy_END_TIME]),row[7])))
	 Lay_ShowErrorAndExit ("Error when reading end date of survey.");

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
      if (Svy->DegCod < 0 && Svy->CrsCod < 0)
         Svy->Status.IBelongToDegCrsGrps = Gbl.Usrs.Me.Logged;
      else if (Svy->DegCod > 0 && Svy->CrsCod < 0)
         Svy->Status.IBelongToDegCrsGrps = Usr_CheckIfIBelongToDeg (Svy->DegCod);
      else if (Svy->CrsCod > 0)
         Svy->Status.IBelongToDegCrsGrps = Usr_CheckIfIBelongToCrs (Svy->CrsCod) &&
                                           Svy_CheckIfICanDoThisSurveyBasedOnGrps (Svy->SvyCod);
      else
         Lay_ShowErrorAndExit ("Wrong survey scope.");

      /* Have I answered this survey? */
      Svy->Status.IHaveAnswered = Svy_CheckIfIHaveAnsweredSvy (Svy->SvyCod);

      /* Can I answer survey? */
      Svy->Status.ICanAnswer = (Svy->NumQsts != 0) &&
                                Svy->Status.Visible &&
                                Svy->Status.Open &&
                                Svy->Status.IAmLoggedWithAValidRoleToAnswer &&
                                Svy->Status.IBelongToDegCrsGrps &&
                               !Svy->Status.IHaveAnswered;

      /* Can I view results of the survey?
         Can I edit survey? */
      switch (Gbl.Usrs.Me.LoggedRole)
        {
         case Rol_ROLE_STUDENT:
            Svy->Status.ICanViewResults = (Svy->NumQsts != 0) &&
                                          Svy->Status.Visible &&
                                          Svy->Status.Open &&
                                          Svy->Status.IAmLoggedWithAValidRoleToAnswer &&
                                          Svy->Status.IBelongToDegCrsGrps &&
                                          Svy->Status.IHaveAnswered;
            Svy->Status.ICanEdit = false;
            break;
         case Rol_ROLE_TEACHER:
            Svy->Status.ICanViewResults = (Svy->NumQsts != 0) &&
                                          !Svy->Status.ICanAnswer;
            Svy->Status.ICanEdit = Svy->CrsCod > 0 &&
                                   Svy->Status.IBelongToDegCrsGrps;
            break;
         case Rol_ROLE_DEG_ADMIN:
            Svy->Status.ICanViewResults = false;
            Svy->Status.ICanEdit = Svy->DegCod > 0 &&
                                   Svy->CrsCod < 0 &&
                                   Svy->Status.IBelongToDegCrsGrps;
            break;
         case Rol_ROLE_SUPERUSER:
            Svy->Status.ICanViewResults = (Svy->NumQsts != 0);
            Svy->Status.ICanEdit = true;
            break;
         default:
            Svy->Status.ICanViewResults = false;
            Svy->Status.ICanEdit = false;
            break;
        }
     }
   else
     {
      /* Initialize to empty survey */
      Svy->SvyCod = -1L;
      Svy->Roles = 0;
      Svy->DateTimes[Svy_START_TIME].Date.Day   =
      Svy->DateTimes[Svy_START_TIME].Date.Month =
      Svy->DateTimes[Svy_START_TIME].Date.Year  = 0;
      Svy->DateTimes[Svy_END_TIME].Date.Day   =
      Svy->DateTimes[Svy_END_TIME].Date.Month =
      Svy->DateTimes[Svy_END_TIME].Date.Year  = 0;
      Svy->Title[0] = '\0';
      Svy->NumQsts = 0;
      Svy->NumUsrs = 0;
      Svy->Status.Visible = true;
      Svy->Status.Open = false;
      Svy->Status.IAmLoggedWithAValidRoleToAnswer = false;
      Svy->Status.IBelongToDegCrsGrps = false;
      Svy->Status.IHaveAnswered = false;
      Svy->Status.ICanAnswer = false;
      Svy->Status.ICanViewResults = false;
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
   extern const char *Txt_Start_date;
   extern const char *Txt_End_date;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct DateTime DateTimes[Asg_NUM_DATES];

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Build query *****/
   sprintf (Query,"SELECT Title,"
                  "DATE_FORMAT(StartTime,'%%Y%%m%%d%%H%%i%%S'),"
                  "DATE_FORMAT(EndTime,'%%Y%%m%%d%%H%%i%%S'),"
                  "Txt"
                  " FROM surveys"
                  " WHERE SvyCod='%ld'",
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

               /* Get start date (row[1] holds the start date in YYYYMMDDHHMMSS format) */
               if (!(Dat_GetDateTimeFromYYYYMMDDHHMMSS (&DateTimes[Asg_START_TIME],row[1])))
	          Lay_ShowErrorAndExit ("Error when reading start date of assignment.");

               /* Get end date (row[2] holds the end date in YYYYMMDDHHMMSS format) */
               if (!(Dat_GetDateTimeFromYYYYMMDDHHMMSS (&DateTimes[Asg_END_TIME  ],row[2])))
	          Lay_ShowErrorAndExit ("Error when reading end date of assignment.");

               sprintf (*ContentStr,"%s: %02u/%02u/%04u %02u:%02u<br />%s: %02u/%02u/%04u %02u:%02u<br />%s",
                        Txt_Start_date,
                        DateTimes[Asg_START_TIME].Date.Day,
                        DateTimes[Asg_START_TIME].Date.Month,
                        DateTimes[Asg_START_TIME].Date.Year,
                        DateTimes[Asg_START_TIME].Time.Hour,
                        DateTimes[Asg_START_TIME].Time.Minute,
                        Txt_End_date,
                        DateTimes[Asg_END_TIME].Date.Day,
                        DateTimes[Asg_END_TIME].Date.Month,
                        DateTimes[Asg_END_TIME].Date.Year,
                        DateTimes[Asg_END_TIME].Time.Hour,
                        DateTimes[Asg_END_TIME].Time.Minute,
                        row[3]);
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

long Svy_GetParamSvyCod (void)
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

   /***** Button of confirmation of removing *****/
   Act_FormStart (ActRemSvy);
   Svy_PutParamSvyCod (Svy.SvyCod);
   Svy_PutHiddenParamSvyOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);

   /***** Ask for confirmation of removing *****/
   sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_survey_X,
            Svy.Title);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);
   fprintf (Gbl.F.Out,"<div align=\"center\"><input type=\"submit\" value=\"%s\" /></div>"
                      "</form>",
            Txt_Remove_survey);

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
   Ntf_SetNotifAsRemoved (Ntf_EVENT_SURVEY,Svy.SvyCod);

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
   extern const char *Txt_Reset_survey;
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

   /***** Button of confirmation of reset *****/
   Act_FormStart (ActRstSvy);
   Svy_PutParamSvyCod (Svy.SvyCod);
   Svy_PutHiddenParamSvyOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);

   /***** Ask for confirmation of reset *****/
   sprintf (Gbl.Message,Txt_Do_you_really_want_to_reset_the_survey_X,
            Svy.Title);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);
   fprintf (Gbl.F.Out,"<div align=\"center\"><input type=\"submit\" value=\"%s\" /></div></form>",
            Txt_Reset_survey);

   /***** Show surveys again *****/
   Svy_ListAllSurveys (&SvyQst);
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
   char Query[512];

   /***** Get number of surveys with a field value from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM surveys"
                  " WHERE DegCod='%ld' AND CrsCod='%ld' AND Title='%s' AND SvyCod<>'%ld'",
            Svy->DegCod,Svy->CrsCod,Svy->Title,Svy->SvyCod);
   return (DB_QueryCOUNT (Query,"can not get similar surveys") != 0);
  }

/*****************************************************************************/
/********************* Put a form to create a new survey *********************/
/*****************************************************************************/

void Svy_RequestCreatOrEditSvy (void)
  {
   extern const char *Txt_New_survey;
   extern const char *Txt_Scope;
   extern const char *Txt_Edit_survey;
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Start_date;
   extern const char *Txt_End_date;
   extern const char *Txt_Title;
   extern const char *Txt_Description;
   extern const char *Txt_Users;
   extern const char *Txt_Create_survey;
   extern const char *Txt_Modify_survey;
   struct Survey Svy;
   struct SurveyQuestion SvyQst;
   bool ItsANewSurvey;
   Svy_StartOrEndTime_t StartOrEndTime;
   const char *NameSelectYear  [Svy_NUM_DATES] = {"StartYear"  ,"EndYear"  };
   const char *NameSelectMonth [Svy_NUM_DATES] = {"StartMonth" ,"EndMonth" };
   const char *NameSelectDay   [Svy_NUM_DATES] = {"StartDay"   ,"EndDay"   };
   const char *NameSelectHour  [Svy_NUM_DATES] = {"StartHour"  ,"EndHour"  };
   const char *NameSelectMinute[Svy_NUM_DATES] = {"StartMinute","EndMinute"};
   const char *Dates[Svy_NUM_DATES] = {Txt_Start_date,Txt_End_date};
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
      Svy.DateTimes[Svy_START_TIME].Date.Year   = Gbl.Now.Date.Year;
      Svy.DateTimes[Svy_START_TIME].Date.Month  = Gbl.Now.Date.Month;
      Svy.DateTimes[Svy_START_TIME].Date.Day    = Gbl.Now.Date.Day;
      Svy.DateTimes[Svy_START_TIME].Time.Hour   = Gbl.Now.Time.Hour;
      Svy.DateTimes[Svy_START_TIME].Time.Minute = Gbl.Now.Time.Minute;
      Svy.DateTimes[Svy_START_TIME].Time.Second = Gbl.Now.Time.Second;
      Svy.DateTimes[Svy_END_TIME  ].Date.Year   = Gbl.Now.Date.Year;
      Svy.DateTimes[Svy_END_TIME  ].Date.Month  = Gbl.Now.Date.Month;
      Svy.DateTimes[Svy_END_TIME  ].Date.Day    = Gbl.Now.Date.Day;
      Svy.DateTimes[Svy_END_TIME  ].Time.Hour   = 23;
      Svy.DateTimes[Svy_END_TIME  ].Time.Minute = 59;
      Svy.DateTimes[Svy_END_TIME  ].Time.Second = 59;
      Svy.Title[0] = '\0';
      Svy.Roles = (1 << Rol_ROLE_STUDENT);
      Svy.NumQsts = 0;
      Svy.NumUsrs = 0;
      Svy.Status.Visible = true;
      Svy.Status.Open = true;
      Svy.Status.IAmLoggedWithAValidRoleToAnswer = false;
      Svy.Status.IBelongToDegCrsGrps = false;
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

   /***** Form start *****/
   if (ItsANewSurvey)
      Act_FormStart (ActNewSvy);
   else
     {
      Act_FormStart (ActChgSvy);
      Svy_PutParamSvyCod (Svy.SvyCod);
     }
   Svy_PutHiddenParamSvyOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);

   /***** Table start *****/
   Lay_StartRoundFrameTable10 (NULL,2,
                               ItsANewSurvey ? Txt_New_survey :
                                               Txt_Edit_survey);

   /***** Survey for anywhere, degree or course? *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"right\" valign=\"middle\" class=\"TIT_TBL\">%s:</td>"
                      "<td align=\"left\" valign=\"middle\">",
            Txt_Scope);
   if (!Svy_SetDefaultAndAllowedForEdition ())
      Lay_ShowErrorAndExit ("You don't have permission to edit surveys here.");
   Sco_GetScope ();
   Sco_PutSelectorScope (false);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Survey title *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"right\" valign=\"top\" class=\"TIT_TBL\">%s:</td>"
                      "<td align=\"left\" valign=\"top\">"
                      "<input type=\"text\" name=\"Title\" size=\"80\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>"
                      "</tr>",
            Txt_Title,
            Svy_MAX_LENGTH_SURVEY_TITLE,Svy.Title);

   /***** Survey start and end dates *****/
   for (StartOrEndTime = Svy_START_TIME;
	StartOrEndTime <= Svy_END_TIME;
	StartOrEndTime++)
     {
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td align=\"right\" valign=\"top\" class=\"TIT_TBL\">%s:</td>"
                         "<td align=\"left\" valign=\"top\">"
                         "<table class=\"CELLS_PAD_2\">"
                         "<tr>"
                         "<td align=\"left\" valign=\"top\">",
               Dates[StartOrEndTime]);
      Dat_WriteFormDate (Gbl.Now.Date.Year-1,
	                 Gbl.Now.Date.Year+1,
                         NameSelectDay  [StartOrEndTime],
                         NameSelectMonth[StartOrEndTime],
                         NameSelectYear [StartOrEndTime],
                         &(Svy.DateTimes[StartOrEndTime].Date),
                         false,false);
      fprintf (Gbl.F.Out,"</td>"
                         "<td align=\"left\" valign=\"top\">");
      Dat_WriteFormHourMinute (NameSelectHour  [StartOrEndTime],
                               NameSelectMinute[StartOrEndTime],
		               &(Svy.DateTimes[StartOrEndTime].Time),
                               false,false);
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>"
	                 "</table>"
	                 "</td>"
	                 "</tr>");
     }

   /***** Survey text *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"right\" valign=\"top\" class=\"TIT_TBL\">%s:</td>"
                      "<td align=\"left\" valign=\"top\">"
                      "<textarea name=\"Txt\" cols=\"60\" rows=\"10\">",
            Txt_Description);
   if (!ItsANewSurvey)
      fprintf (Gbl.F.Out,"%s",Txt);
   fprintf (Gbl.F.Out,"</textarea>"
                      "</td>"
                      "</tr>");

   /***** Users' roles who can answer the survey *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"right\" valign=\"top\" class=\"TIT_TBL\">%s:</td>"
                      "<td align=\"left\" valign=\"middle\" class=\"DAT\">",
            Txt_Users);
   Rol_WriteSelectorRoles (Svy.Roles);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Groups *****/
   Svy_ShowLstGrpsToEditSurvey (Svy.SvyCod);

   /***** Table end *****/
   Lay_EndRoundFrameTable10 ();

   /***** Button to create/modify survey *****/
   Lay_PutSendButton (ItsANewSurvey ? Txt_Create_survey :
                                      Txt_Modify_survey);

   /***** Form end *****/
   fprintf (Gbl.F.Out,"</form>");

   /***** Show questions of the survey ready to be edited *****/
   if (!ItsANewSurvey)
      Svy_ListSvyQuestions (&Svy,&SvyQst);
  }

/*****************************************************************************/
/*** Set default and allowed location ranges depending on logged user type ***/
/*****************************************************************************/
// Return true if user can edit surveys in current location

static bool Svy_SetDefaultAndAllowedForEdition (void)
  {
   Gbl.Scope.Default = Sco_SCOPE_NONE;
   Gbl.Scope.Allowed = 0;

   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_TEACHER:
         if (Gbl.CurrentCrs.Crs.CrsCod > 0)
           {
            Gbl.Scope.Default = Sco_SCOPE_COURSE;
            Gbl.Scope.Allowed = 1 << Sco_SCOPE_COURSE;
            return true;
           }
         return false;
       case Rol_ROLE_DEG_ADMIN:
         if (Gbl.CurrentDeg.Deg.DegCod > 0)
           {
            Gbl.Scope.Default = Sco_SCOPE_DEGREE;
            Gbl.Scope.Allowed = 1 << Sco_SCOPE_DEGREE;
            return true;
           }
         return false;
      case Rol_ROLE_SUPERUSER:
         Gbl.Scope.Default = Sco_SCOPE_PLATFORM;
         Gbl.Scope.Allowed = 1 << Sco_SCOPE_PLATFORM    |
	                     // 1 << Sco_SCOPE_COUNTRY     |	// TODO: Add this scope
	                     // 1 << Sco_SCOPE_INSTITUTION |	// TODO: Add this scope
	                     // 1 << Sco_SCOPE_CENTRE      |	// TODO: Add this scope
                             1 << Sco_SCOPE_DEGREE      |
                             1 << Sco_SCOPE_COURSE;
         return true;
      default:
         return false;
     }
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

   if (Gbl.CurrentCrs.Grps.GrpTypes.Num)
     {
      /***** Start table *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td align=\"right\" valign=\"top\" class=\"TIT_TBL\">%s:</td>"
                         "<td align=\"left\" valign=\"top\">",
               Txt_Groups);
      Lay_StartRoundFrameTable10 (NULL,0,NULL);

      /***** First row: checkbox to select the whole course *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td colspan=\"7\" align=\"left\" valign=\"middle\" class=\"DAT\">"
                         "<input type=\"checkbox\" id=\"WholeCrs\" name=\"WholeCrs\" value=\"Y\"");
      if (!Svy_CheckIfSvyIsAssociatedToGrps (SvyCod))
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," onclick=\"uncheckChildren(this,'GrpCods')\" />%s %s</td>"
	                 "</tr>",
               Txt_The_whole_course,Gbl.CurrentCrs.Crs.ShortName);

      /***** List the groups for each group type *****/
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
         if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
            Grp_ListGrpsToEditAsgAttOrSvy (&Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],SvyCod,Grp_SURVEY);

      /***** End table *****/
      Lay_EndRoundFrameTable10 ();
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
   struct Survey OldSvy,NewSvy;
   struct SurveyQuestion SvyQst;
   bool ItsANewSurvey;
   bool NewSurveyIsCorrect = true;
   unsigned NumUsrsToBeNotifiedByEMail;
   char Txt[Cns_MAX_BYTES_TEXT+1];

   /***** Get the code of the survey *****/
   ItsANewSurvey = ((NewSvy.SvyCod = Svy_GetParamSvyCod ()) == -1L);

   if (!ItsANewSurvey)
     {
      /* Get data of the old (current) survey from database */
      OldSvy.SvyCod = NewSvy.SvyCod;
      Svy_GetDataOfSurveyByCod (&OldSvy);
      if (!OldSvy.Status.ICanEdit)
         Lay_ShowErrorAndExit ("You can not update this survey.");
     }

   /***** Get scope *****/
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_PLATFORM    |
	               // 1 << Sco_SCOPE_COUNTRY     |	// TODO: Add this scope
	               // 1 << Sco_SCOPE_INSTITUTION |	// TODO: Add this scope
	               // 1 << Sco_SCOPE_CENTRE      |	// TODO: Add this scope
                       1 << Sco_SCOPE_DEGREE      |
                       1 << Sco_SCOPE_COURSE;
   Gbl.Scope.Default = Sco_SCOPE_PLATFORM;
   Sco_GetScope ();

   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_PLATFORM:
         NewSvy.DegCod = -1L;
         NewSvy.CrsCod = -1L;
         break;
      case Sco_SCOPE_DEGREE:
         if (Gbl.CurrentDeg.Deg.DegCod > 0)
           {
            NewSvy.DegCod = Gbl.CurrentDeg.Deg.DegCod;
            NewSvy.CrsCod = -1L;
           }
         else
            Lay_ShowErrorAndExit ("Wrong survey location.");
         break;
      case Sco_SCOPE_COURSE:
         if (Gbl.CurrentCrs.Crs.CrsCod > 0)
           {
            NewSvy.DegCod = -1L;	// DegCod doen't mind when CrsCod > 0
            NewSvy.CrsCod = Gbl.CurrentCrs.Crs.CrsCod;
           }
         else
            Lay_ShowErrorAndExit ("Wrong survey location.");
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }

   /***** Get start date *****/
   Dat_GetDateFromForm ("StartDay","StartMonth","StartYear",
                        &(NewSvy.DateTimes[Svy_START_TIME].Date.Day),
                        &(NewSvy.DateTimes[Svy_START_TIME].Date.Month),
                        &(NewSvy.DateTimes[Svy_START_TIME].Date.Year));
   Dat_GetHourMinuteFromForm ("StartHour","StartMinute",
                              &(NewSvy.DateTimes[Svy_START_TIME].Time.Hour),
                              &(NewSvy.DateTimes[Svy_START_TIME].Time.Minute));

   /***** Get end date *****/
   Dat_GetDateFromForm ("EndDay","EndMonth","EndYear",
                        &(NewSvy.DateTimes[Svy_END_TIME].Date.Day),
                        &(NewSvy.DateTimes[Svy_END_TIME].Date.Month),
                        &(NewSvy.DateTimes[Svy_END_TIME].Date.Year));
   Dat_GetHourMinuteFromForm ("EndHour","EndMinute",
                              &(NewSvy.DateTimes[Svy_END_TIME].Time.Hour),
                              &(NewSvy.DateTimes[Svy_END_TIME].Time.Minute));

   /***** Get survey title *****/
   Par_GetParToText ("Title",NewSvy.Title,Svy_MAX_LENGTH_SURVEY_TITLE);

   /***** Get survey text and insert links *****/
   Par_GetParToHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (NewSvy.DateTimes[Svy_START_TIME].Date.Day   == 0 ||
       NewSvy.DateTimes[Svy_START_TIME].Date.Month == 0 ||
       NewSvy.DateTimes[Svy_START_TIME].Date.Year  == 0)
     {
      NewSvy.DateTimes[Svy_START_TIME].Date.Year   = Gbl.Now.Date.Year;
      NewSvy.DateTimes[Svy_START_TIME].Date.Month  = Gbl.Now.Date.Month;
      NewSvy.DateTimes[Svy_START_TIME].Date.Day    = Gbl.Now.Date.Day;
      NewSvy.DateTimes[Svy_START_TIME].Time.Hour   = Gbl.Now.Time.Hour;
      NewSvy.DateTimes[Svy_START_TIME].Time.Minute = Gbl.Now.Time.Minute;
      NewSvy.DateTimes[Svy_START_TIME].Time.Second = Gbl.Now.Time.Second;
     }
   if (NewSvy.DateTimes[Svy_END_TIME].Date.Day   == 0 ||
       NewSvy.DateTimes[Svy_END_TIME].Date.Month == 0 ||
       NewSvy.DateTimes[Svy_END_TIME].Date.Year  == 0)
     {
      NewSvy.DateTimes[Svy_END_TIME].Date.Year   = Gbl.Now.Date.Year;
      NewSvy.DateTimes[Svy_END_TIME].Date.Month  = Gbl.Now.Date.Month;
      NewSvy.DateTimes[Svy_END_TIME].Date.Day    = Gbl.Now.Date.Day;
      NewSvy.DateTimes[Svy_END_TIME].Time.Hour   = 23;
      NewSvy.DateTimes[Svy_END_TIME].Time.Minute = 59;
     }
   NewSvy.DateTimes[Svy_END_TIME].Time.Second = 59;

   /***** Get users who can answer this survey *****/
   Rol_GetSelectedRoles (&(NewSvy.Roles));

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

   /***** Notify by e-mail about the new assignment *****/
   if (NewSvy.CrsCod > 0)	// Notify only the surveys for a course, not for a degree or global
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

static void Svy_UpdateNumUsrsNotifiedByEMailAboutSurvey (long SvyCod,unsigned NumUsrsToBeNotifiedByEMail)
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
   extern const char *Txt_Created_new_survey_X;
   char Query[1024+Cns_MAX_BYTES_TEXT];

   /***** Create a new survey *****/
   sprintf (Query,"INSERT INTO surveys (DegCod,CrsCod,Hidden,Roles,UsrCod,StartTime,EndTime,Title,Txt)"
                  " VALUES ('%ld','%ld','N','%u','%ld','%04u%02u%02u%02u%02u%02u','%04u%02u%02u%02u%02u%02u','%s','%s')",
            Svy->DegCod,
            Svy->CrsCod,
            Svy->Roles,
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Svy->DateTimes[Svy_START_TIME].Date.Year,
            Svy->DateTimes[Svy_START_TIME].Date.Month,
            Svy->DateTimes[Svy_START_TIME].Date.Day,
            Svy->DateTimes[Svy_START_TIME].Time.Hour,
            Svy->DateTimes[Svy_START_TIME].Time.Minute,
            Svy->DateTimes[Svy_START_TIME].Time.Second,
            Svy->DateTimes[Svy_END_TIME].Date.Year,
            Svy->DateTimes[Svy_END_TIME].Date.Month,
            Svy->DateTimes[Svy_END_TIME].Date.Day,
            Svy->DateTimes[Svy_END_TIME].Time.Hour,
            Svy->DateTimes[Svy_END_TIME].Time.Minute,
            Svy->DateTimes[Svy_END_TIME].Time.Second,
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
   extern const char *Txt_The_survey_has_been_modified;
   char Query[1024+Cns_MAX_BYTES_TEXT];

   /***** Update the data of the survey *****/
   sprintf (Query,"UPDATE surveys SET DegCod='%ld',CrsCod='%ld',Roles='%u',StartTime='%04u%02u%02u%02u%02u%02u',EndTime='%04u%02u%02u%02u%02u%02u',Title='%s',Txt='%s'"
                  " WHERE SvyCod='%ld'",
            Svy->DegCod,Svy->CrsCod,
            Svy->Roles,
            Svy->DateTimes[Svy_START_TIME].Date.Year,
            Svy->DateTimes[Svy_START_TIME].Date.Month,
            Svy->DateTimes[Svy_START_TIME].Date.Day,
            Svy->DateTimes[Svy_START_TIME].Time.Hour,
            Svy->DateTimes[Svy_START_TIME].Time.Minute,
            Svy->DateTimes[Svy_START_TIME].Time.Second,
            Svy->DateTimes[Svy_END_TIME  ].Date.Year,
            Svy->DateTimes[Svy_END_TIME  ].Date.Month,
            Svy->DateTimes[Svy_END_TIME  ].Date.Day,
            Svy->DateTimes[Svy_END_TIME  ].Time.Hour,
            Svy->DateTimes[Svy_END_TIME  ].Time.Minute,
            Svy->DateTimes[Svy_END_TIME  ].Time.Second,
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
   sprintf (Query,"DELETE FROM svy_grp WHERE GrpCod='%ld'",GrpCod);
   DB_QueryDELETE (Query,"can not remove group from the associations between surveys and groups");
  }

/*****************************************************************************/
/*************** Remove groups of one type from all the surveys **************/
/*****************************************************************************/

void Svy_RemoveGroupsOfType (long GrpTypCod)
  {
   char Query[256];

   /***** Remove group from all the surveys *****/
   sprintf (Query,"DELETE FROM svy_grp USING crs_grp,svy_grp"
                  " WHERE crs_grp.GrpTypCod='%ld' AND crs_grp.GrpCod=svy_grp.GrpCod",
            GrpTypCod);
   DB_QueryDELETE (Query,"can not remove groups of a type from the associations between surveys and groups");
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
   fprintf (Gbl.F.Out,"<p class=\"%s\">%s: ",
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
               Txt_The_whole_course,Gbl.CurrentCrs.Crs.ShortName);

   fprintf (Gbl.F.Out,"</p>");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Remove all the surveys of a degree ********************/
/*****************************************************************************/

void Svy_RemoveDegSurveys (long DegCod)
  {
   char Query[512];

   /***** Remove all the users in degree surveys
          (not including surveys of courses in degree) *****/
   sprintf (Query,"DELETE FROM svy_users USING surveys,svy_users"
                  " WHERE surveys.DegCod='%ld' AND surveys.CrsCod='-1'"
                  " AND surveys.SvyCod=svy_users.SvyCod",
            DegCod);
   DB_QueryDELETE (Query,"can not remove users who are answered surveys in a degree");

   /***** Remove all the answers in degree surveys
          (not including surveys of courses in degree) *****/
   sprintf (Query,"DELETE FROM svy_answers USING surveys,svy_questions,svy_answers"
                  " WHERE surveys.DegCod='%ld' AND surveys.CrsCod='-1'"
                  " AND surveys.SvyCod=svy_questions.SvyCod"
                  " AND svy_questions.QstCod=svy_answers.QstCod",
            DegCod);
   DB_QueryDELETE (Query,"can not remove answers of surveys in a degree");

   /***** Remove all the questions in this survey
          (not including surveys of courses in degree) *****/
   sprintf (Query,"DELETE FROM svy_questions USING surveys,svy_questions"
                  " WHERE surveys.DegCod='%ld' AND surveys.CrsCod='-1'"
                  " AND surveys.SvyCod=svy_questions.SvyCod",
            DegCod);
   DB_QueryDELETE (Query,"can not remove questions of surveys in a degree");

   /***** Remove degree surveys
          (not including surveys of courses in degree) *****/
   sprintf (Query,"DELETE FROM surveys"
                  " WHERE surveys.DegCod='%ld' AND surveys.CrsCod='-1'",
            DegCod);
   DB_QueryDELETE (Query,"can not remove all the surveys of a course");
  }

/*****************************************************************************/
/********************* Remove all the surveys of a course ********************/
/*****************************************************************************/

void Svy_RemoveCrsSurveys (long CrsCod)
  {
   char Query[512];

   /***** Remove all the users in course surveys *****/
   sprintf (Query,"DELETE FROM svy_users USING surveys,svy_users"
                  " WHERE surveys.CrsCod='%ld'"
                  " AND surveys.SvyCod=svy_users.SvyCod",
            CrsCod);
   DB_QueryDELETE (Query,"can not remove users who are answered surveys in a course");

   /***** Remove all the answers in course surveys *****/
   sprintf (Query,"DELETE FROM svy_answers USING surveys,svy_questions,svy_answers"
                  " WHERE surveys.CrsCod='%ld'"
                  " AND surveys.SvyCod=svy_questions.SvyCod"
                  " AND svy_questions.QstCod=svy_answers.QstCod",
            CrsCod);
   DB_QueryDELETE (Query,"can not remove answers of surveys in a course");

   /***** Remove all the questions in course surveys *****/
   sprintf (Query,"DELETE FROM svy_questions USING surveys,svy_questions"
                  " WHERE surveys.CrsCod='%ld'"
                  " AND surveys.SvyCod=svy_questions.SvyCod",
            CrsCod);
   DB_QueryDELETE (Query,"can not remove questions of surveys in a course");

   /***** Remove groups *****/
   sprintf (Query,"DELETE FROM svy_grp USING surveys,svy_grp"
                  " WHERE surveys.CrsCod='%ld' AND surveys.SvyCod=svy_grp.SvyCod",
            CrsCod);
   DB_QueryDELETE (Query,"can not remove all the groups associated to surveys of a course");

   /***** Remove course surveys *****/
   sprintf (Query,"DELETE FROM surveys WHERE CrsCod='%ld'",CrsCod);
   DB_QueryDELETE (Query,"can not remove all the surveys of a course");
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
                  " WHERE crs_grp_usr.UsrCod='%ld' AND svy_grp.GrpCod=crs_grp_usr.GrpCod))",
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
   Lay_StartRoundFrameTable10 (NULL,2,NULL);
   Svy_ShowOneSurvey (SvyCod,&SvyQst,true);
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/******************* Show form to edit one survey question *******************/
/*****************************************************************************/

static void Svy_ShowFormEditOneQst (long SvyCod,struct SurveyQuestion *SvyQst,char *Txt)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Question;
   extern const char *Txt_New_question;
   extern const char *Txt_Stem;
   extern const char *Txt_Type;
   extern const char *Txt_SVY_STR_ANSWER_TYPES[Svy_NUM_ANS_TYPES];
   extern const char *Txt_Send;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAns,NumAnswers = 0;
   Svy_AnswerType_t AnsType;

   if (Gbl.CurrentAct == ActEdiOneSvyQst) // If no receiving the question, but editing a new or existing question
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

   /***** Start form *****/
   Act_FormStart (ActRcvSvyQst);
   Svy_PutParamSvyCod (SvyCod);
   if (SvyQst->QstCod > 0)	// If the question already has assigned a code
      Svy_PutParamQstCod (SvyQst->QstCod);

   fprintf (Gbl.F.Out,"<div align=\"center\">");

   /***** Show message *****/
   if (SvyQst->QstCod > 0)	// If the question already has assigned a code
     {
      sprintf (Gbl.Message,"%s %u",
               Txt_Question,SvyQst->QstInd + 1);	// Question index may be 0, 1, 2, 3,...
      Lay_WriteTitle (Gbl.Message);
     }
   else
      Lay_WriteTitle (Txt_New_question);

   fprintf (Gbl.F.Out,"<table>");

   /***** Stem *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"right\" valign=\"top\" class=\"%s\">%s:</td>"
                      "<td align=\"left\" valign=\"top\">"
                      "<textarea name=\"Txt\" cols=\"60\" rows=\"4\">"
	              "%s"
                      "</textarea>"
                      "</td>"
                      "</tr>",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Stem,
	    Txt);

   /***** Type of answer *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"right\" valign=\"top\" class=\"%s\">%s:</td>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Type);
   fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"top\" class=\"%s\">",
            The_ClassFormul[Gbl.Prefs.Theme]);
   for (AnsType = (Svy_AnswerType_t) 0;
	AnsType < Svy_NUM_ANS_TYPES;
	AnsType++)
     {
      fprintf (Gbl.F.Out,"<input type=\"radio\" name=\"AnswerType\" value=\"%u\"",
               (unsigned) AnsType);
      if (AnsType == SvyQst->AnswerType)
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," />%s<br />",
               Txt_SVY_STR_ANSWER_TYPES[AnsType]);
     }
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Answers *****/
   /* Unique or multiple choice answers */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"left\"></td>"
                      "<td align=\"left\" valign=\"top\">"
                      "<table class=\"CELLS_PAD_2\">");
   for (NumAns = 0;
	NumAns < Svy_MAX_ANSWERS_PER_QUESTION;
	NumAns++)
     {
      /* Label with the number of the answer */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td align=\"right\" valign=\"top\" class=\"%s\">%u)</td>",
               The_ClassFormul[Gbl.Prefs.Theme],NumAns+1);

      /* Answer text */
      fprintf (Gbl.F.Out,"<td align=\"right\" valign=\"top\">"
                         "<textarea name=\"AnsStr%u\" cols=\"50\" rows=\"1\">",
               NumAns);
      if (SvyQst->AnsChoice[NumAns].Text)
         fprintf (Gbl.F.Out,"%s",SvyQst->AnsChoice[NumAns].Text);
      fprintf (Gbl.F.Out,"</textarea>"
	                 "</td>"
	                 "</tr>");
     }

   /***** Send and undo buttons *****/
   fprintf (Gbl.F.Out,"</table></tr></table></div>");
   Lay_PutSendButton (Txt_Send);
   fprintf (Gbl.F.Out,"</form>");

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
      Lay_ShowErrorAndExit ("Error when getting answers of a question.");

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
      Str_ReplaceSeveralSpacesForOne (SvyQst.AnsChoice[NumAns].Text);	// Join several spaces into one in answer
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
   Lay_StartRoundFrameTable10 (NULL,2,NULL);
   Svy_ShowOneSurvey (SvyCod,&SvyQst,true);
   Lay_EndRoundFrameTable10 ();
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
   char Query[512];
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
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Survey_questions;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Type;
   extern const char *Txt_Question;
   extern const char *Txt_SVY_STR_ANSWER_TYPES[Svy_NUM_ANS_TYPES];
   extern const char *Txt_This_survey_has_no_questions;
   extern const char *Txt_Send_survey;
   extern const char *Txt_Remove_question;
   extern const char *Txt_Edit_question;
   extern const char *Txt_New_question;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQsts;
   unsigned NumQst;

   /***** Get data of questions from database *****/
   sprintf (Query,"SELECT QstCod,QstInd,AnsType,Stem"
                  " FROM svy_questions WHERE SvyCod='%ld' ORDER BY QstInd",
            Svy->SvyCod);
   NumQsts = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get data of a question");

   if (Svy->Status.ICanAnswer)
     {
      /***** Start form to send answers to survey *****/
      Act_FormStart (ActAnsSvy);
      Svy_PutParamSvyCod (Svy->SvyCod);
     }
   Lay_StartRoundFrameTable10 (NULL,0,NULL);

   /***** Heading title *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"center\" class=\"DAT\">");
   Lay_WriteTitle (Txt_Survey_questions);
   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_4\">");

   if (NumQsts)
     {
      /***** Write the heading *****/
      fprintf (Gbl.F.Out,"<tr>");
      if (Svy->Status.ICanEdit)
         fprintf (Gbl.F.Out,"<td align=\"left\" colspan=\"2\"></td>");
      fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"top\" class=\"TIT_TBL\">%s</td>"
                         "<td align=\"center\" valign=\"top\" class=\"TIT_TBL\">%s</td>"
                         "<td align=\"left\" valign=\"top\" class=\"TIT_TBL\">%s</td>"
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
            fprintf (Gbl.F.Out,"<td class=\"BT%d\">",Gbl.RowEvenOdd);
            Act_FormStart (ActRemSvyQst);
            Svy_PutParamSvyCod (Svy->SvyCod);
            Svy_PutParamQstCod (SvyQst->QstCod);
            Sta_WriteParamsDatesSeeAccesses ();
            Svy_WriteParamEditQst (SvyQst);
            fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/delon16x16.gif\""
        	               " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />"
                               "</form>"
                               "</td>",
                     Gbl.Prefs.IconsURL,
                     Txt_Remove_question,
                     Txt_Remove_question);

            /* Write icon to edit the question */
            fprintf (Gbl.F.Out,"<td class=\"BT%d\">",Gbl.RowEvenOdd);
            Act_FormStart (ActEdiOneSvyQst);
            Svy_PutParamSvyCod (Svy->SvyCod);
            Svy_PutParamQstCod (SvyQst->QstCod);
            fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/edit16x16.gif\""
        	               " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />"
        	               "</form>"
        	               "</td>",
                     Gbl.Prefs.IconsURL,
                     Txt_Edit_question,
                     Txt_Edit_question);
           }

         /* Write index of question inside survey (row[1]) */
         if (sscanf (row[1],"%u",&(SvyQst->QstInd)) != 1)
            Lay_ShowErrorAndExit ("Error: wrong question index.");
         fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"top\" bgcolor=\"%s\" class=\"DAT_SMALL\">%u</td>",
                  Gbl.ColorRows[Gbl.RowEvenOdd],SvyQst->QstInd+1);

         /* Write the question type (row[2]) */
         SvyQst->AnswerType = Svy_ConvertFromStrAnsTypDBToAnsTyp (row[2]);
         fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"top\" bgcolor=\"%s\" class=\"DAT_SMALL\">%s",
	          Gbl.ColorRows[Gbl.RowEvenOdd],
                  Txt_SVY_STR_ANSWER_TYPES[SvyQst->AnswerType]);

         /* Write the stem (row[3]) */
         Svy_WriteQstStem (row[3],"TEST_EDI");

         /* Write the answers of this question */
         Svy_WriteAnswersOfAQst (Svy,SvyQst);
         fprintf (Gbl.F.Out,"</td>"
                            "</tr>");
        }
     }
   else	// This survey has no questions
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td align=\"center\" class=\"ASG_GRP\">(%s)</td>"
	                 "</tr>",
               Txt_This_survey_has_no_questions);

   fprintf (Gbl.F.Out,"</table>"
	              "</td>"
	              "</tr>");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (Svy->Status.ICanAnswer)
     {
      /***** Table end *****/
      Lay_EndRoundFrameTable10 ();

      /***** Button to create/modify survey *****/
      Lay_PutSendButton (Txt_Send_survey);

      /***** Form end *****/
      fprintf (Gbl.F.Out,"</form>");
     }
   else
     {
      if (Svy->Status.ICanEdit)
        {
         /***** Put form to add a new question in this survey *****/
         fprintf (Gbl.F.Out,"<tr>"
	                    "<td align=\"center\">");
         Act_FormStart (ActEdiOneSvyQst);
         Svy_PutParamSvyCod (Svy->SvyCod);
         Svy_PutHiddenParamSvyOrderType ();
         Grp_PutParamWhichGrps ();
         Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
         Act_LinkFormSubmit (Txt_New_question,The_ClassFormul[Gbl.Prefs.Theme]);
         Lay_PutSendIcon ("new",Txt_New_question,Txt_New_question);
         fprintf (Gbl.F.Out,"</form>"
                            "</td>"
                            "</tr>");
        }

      /***** Table end *****/
      Lay_EndRoundFrameTable10 ();
     }
  }

/*****************************************************************************/
/********* Write hidden parameters for edition of survey questions ***********/
/*****************************************************************************/

static void Svy_WriteParamEditQst (struct SurveyQuestion *SvyQst)
  {
   Par_PutHiddenParamChar ("AllAnsTypes",
                           SvyQst->AllAnsTypes ? 'Y' :
                        	                 'N');
   Par_PutHiddenParamString ("AnswerType",SvyQst->ListAnsTypes);
  }

/*****************************************************************************/
/****************** Write the heading of a survey question *******************/
/*****************************************************************************/

static void Svy_WriteQstStem (const char *Stem,const char *TextStyle)
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
   fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"top\" bgcolor=\"%s\" class=\"%s\">"
                      "<p align=\"justify\"><tt>%s</tt></p>",
	    Gbl.ColorRows[Gbl.RowEvenOdd],TextStyle,HeadingRigorousHTML);

   /* Free memory allocated for the stem */
   free ((void *) HeadingRigorousHTML);
  }

/*****************************************************************************/
/************** Get and write the answers of a survey question ***************/
/*****************************************************************************/

static void Svy_WriteAnswersOfAQst (struct Survey *Svy,struct SurveyQuestion *SvyQst)
  {
   unsigned NumAns,NumAnswers;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrsThisAnswer;
   char *Answer;
   size_t AnsLength;

   NumAnswers = Svy_GetAnswersQst (SvyQst->QstCod,&mysql_res);	// Result: AnsInd,NumUsrs,Answer

   /***** Write the answers *****/
   fprintf (Gbl.F.Out,"<table width=\"100%%\" class=\"CELLS_PAD_4\">");
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

      if (Svy->Status.ICanAnswer)
        {
         /* Write selector to choice this answer */
         fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"top\">");
         fprintf (Gbl.F.Out,"<input type=\"");
         if (SvyQst->AnswerType == Svy_ANS_UNIQUE_CHOICE)
            fprintf (Gbl.F.Out,"radio\" onclick=\"selectUnselectRadio(this,this.form.Ans%010u,%u)\"",
                     (unsigned) SvyQst->QstCod,NumAnswers);
         else // SvyQst->AnswerType == Svy_ANS_MULTIPLE_CHOICE
            fprintf (Gbl.F.Out,"checkbox\"");
         fprintf (Gbl.F.Out," name=\"Ans%010u\" value=\"%u\" /></td>",
                  (unsigned) SvyQst->QstCod,NumAns);
        }

      /* Write the number of option */
      fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"top\" width=\"40\" class=\"DAT\">%u)</td>",
               NumAns+1);

      /* Write the text of the answer */
      fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"top\" class=\"TEST_EDI\"><tt>%s</tt></td>",
               Answer);

      /* Show stats of this answer */
      if (Svy->Status.ICanViewResults)
         Svy_DrawBarNumUsrs (NumUsrsThisAnswer,Svy->NumUsrs);

      fprintf (Gbl.F.Out,"</tr>");

      /* Free memory allocated for the answer */
      free ((void *) Answer);
     }
   fprintf (Gbl.F.Out,"</table>");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Draw a bar with the percentage of answers *****************/
/*****************************************************************************/

#define Svy_MAX_BAR_WIDTH 100

static void Svy_DrawBarNumUsrs (unsigned NumUsrs,unsigned MaxUsrs)
  {
   extern const char *Txt_of_PART_OF_A_TOTAL;
   unsigned BarWidth = 0;

   /***** Draw bar with a with proportional to the number of clicks *****/
   fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"top\" width=\"%u\" class=\"DAT\">",
            Svy_MAX_BAR_WIDTH+100);
   if (NumUsrs && MaxUsrs)
      BarWidth = (unsigned) ((((float) NumUsrs * (float) Svy_MAX_BAR_WIDTH) / (float) MaxUsrs) + 0.5);
   if (BarWidth < 2)
      BarWidth = 2;
   fprintf (Gbl.F.Out,"<img src=\"%s/c1x16.gif\" width=\"%u\" height=\"16\" style=\"vertical-align:top;\" alt=\"\" />&nbsp;",
      Gbl.Prefs.IconsURL,BarWidth);

   /***** Write the number of users *****/
   if (MaxUsrs)
      fprintf (Gbl.F.Out,"%u&nbsp;(%u",
               NumUsrs,(unsigned) ((((float) NumUsrs * 100.0) / (float) MaxUsrs) + 0.5));
   else
      fprintf (Gbl.F.Out,"0&nbsp;(0");
   fprintf (Gbl.F.Out,"%%&nbsp;%s&nbsp;%u)</td>"
	              "</tr>",
            Txt_of_PART_OF_A_TOTAL,MaxUsrs);
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
   Lay_StartRoundFrameTable10 (NULL,2,NULL);
   Svy_ShowOneSurvey (SvyCod,&SvyQst,true);
   Lay_EndRoundFrameTable10 ();
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
   Lay_StartRoundFrameTable10 (NULL,2,NULL);
   Svy_ShowOneSurvey (Svy.SvyCod,&SvyQst,true);
   Lay_EndRoundFrameTable10 ();
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

unsigned Svy_GetNumCoursesWithSurveys (Sco_Scope_t Scope)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with surveys from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_PLATFORM:
         sprintf (Query,"SELECT COUNT(DISTINCT (CrsCod))"
                        " FROM surveys"
                        " WHERE CrsCod>'0'");
         break;
      case Sco_SCOPE_INSTITUTION:
         sprintf (Query,"SELECT COUNT(DISTINCT (surveys.CrsCod))"
                        " FROM centres,degrees,courses,surveys"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=surveys.CrsCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CENTRE:
         sprintf (Query,"SELECT COUNT(DISTINCT (surveys.CrsCod))"
                        " FROM degrees,courses,surveys"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=surveys.CrsCod",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEGREE:
         sprintf (Query,"SELECT COUNT(DISTINCT (surveys.CrsCod))"
                        " FROM courses,surveys"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=surveys.CrsCod",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_COURSE:
         sprintf (Query,"SELECT COUNT(DISTINCT (CrsCod))"
                        " FROM surveys"
                        " WHERE CrsCod='%ld'",
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
/*************************** Get number of surveys ***************************/
/*****************************************************************************/
// Returns the number of surveys for courses
// in this location (all the platform, current degree or current course)

unsigned Svy_GetNumSurveys (Sco_Scope_t Scope,unsigned *NumNotif)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumSurveys;

   /***** Get number of surveys from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_PLATFORM:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM surveys"
                        " WHERE CrsCod>'0'");
         break;
      case Sco_SCOPE_INSTITUTION:
         sprintf (Query,"SELECT COUNT(*),SUM(surveys.NumNotif)"
                        " FROM centres,degrees,courses,surveys"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=surveys.CrsCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CENTRE:
         sprintf (Query,"SELECT COUNT(*),SUM(surveys.NumNotif)"
                        " FROM degrees,courses,surveys"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=surveys.CrsCod",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEGREE:
         sprintf (Query,"SELECT COUNT(*),SUM(surveys.NumNotif)"
                        " FROM courses,surveys"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=surveys.CrsCod",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_COURSE:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM surveys"
                        " WHERE CrsCod='%ld'",
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

   /***** Get number of notifications by e-mail *****/
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
/*************** Get average number of questions per survey ******************/
/*****************************************************************************/

float Svy_GetNumQstsPerSurvey (Sco_Scope_t Scope)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   float NumQstsPerSurvey;

   /***** Get number of courses per user from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_PLATFORM:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
                        " FROM surveys,svy_questions"
                        " WHERE surveys.CrsCod>'0'"
                        " AND surveys.SvyCod=svy_questions.SvyCod"
                        " GROUP BY svy_questions.SvyCod) AS NumQstsTable");
         break;
      case Sco_SCOPE_INSTITUTION:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
                        " FROM centres,degrees,courses,surveys,svy_questions"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=surveys.CrsCod"
                        " AND surveys.SvyCod=svy_questions.SvyCod"
                        " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CENTRE:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
                        " FROM degrees,courses,surveys,svy_questions"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=surveys.CrsCod"
                        " AND surveys.SvyCod=svy_questions.SvyCod"
                        " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEGREE:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
                        " FROM courses,surveys,svy_questions"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=surveys.CrsCod"
                        " AND surveys.SvyCod=svy_questions.SvyCod"
                        " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_COURSE:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
                        " FROM surveys,svy_questions"
                        " WHERE surveys.CrsCod='%ld'"
                        " AND surveys.SvyCod=svy_questions.SvyCod"
                        " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
                  Gbl.CurrentCrs.Crs.CrsCod);
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
