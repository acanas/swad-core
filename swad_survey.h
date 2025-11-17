// swad_survey.h: surveys

#ifndef _SWAD_SVY
#define _SWAD_SVY
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_date.h"
#include "swad_deny_allow.h"
#include "swad_hierarchy_type.h"
#include "swad_notification.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Svy_MAX_CHARS_SURVEY_TITLE	(128 - 1)	// 127
#define Svy_MAX_BYTES_SURVEY_TITLE	((Svy_MAX_CHARS_SURVEY_TITLE + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Svy_ORDER_DEFAULT Dat_STR_TIME

#define Svy_NUM_I_HAVE_ANSWERED 2
typedef enum
  {
   Svy_I_DONT_HAVE_ANSWERED,
   Svy_I_HAVE_ANSWERED,
  } Svy_IHaveAnswered_t;

struct Svy_Survey
  {
   long SvyCod;
   Hie_Level_t HieLvl;
   long HieCod;		// Country, institution, center, degree or course code
   unsigned Roles;	// Example: if survey can be made by students, Roles == (1 << Rol_STD)
   long UsrCod;
   char Title[Svy_MAX_BYTES_SURVEY_TITLE + 1];
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   unsigned NumQsts;	// Number of questions in the survey
   unsigned NumUsrs;	// Number of distinct users who have already answered the survey
   struct
     {
      HidVis_HiddenOrVisible_t Hidden;	// Survey is hidden or visible?
      CloOpe_ClosedOrOpen_t ClosedOrOpen;	// Start date <= now <= end date
      Err_SuccessOrError_t ValidRoleToAnswer;	// I am logged with a valid role to answer this survey
      Usr_Belong_t IBelongToScope;		// I belong to the scope of this survey
      Svy_IHaveAnswered_t IHaveAnswered;	// I have already answered this survey
      Usr_Can_t ICanAnswer;
      Usr_Can_t ICanViewResults;
      Usr_Can_t ICanViewComments;
      Usr_Can_t ICanEdit;
     } Status;
  };

struct Svy_Surveys
  {
   Cac_Status_t LstReadStatus;	// Is the list already read from database...
				// ...or it needs to be read?
   unsigned Num;		// Number of surveys
   long *LstSvyCods;		// List of survey codes
   Dat_StartEndTime_t SelectedOrder;
   unsigned CurrentPage;
   struct Svy_Survey Svy;	// Selected/current survey
   long QstCod;
  };

#define Svy_NUM_ANS_TYPES	2
typedef enum
  {
   Svy_ANS_UNIQUE_CHOICE   = 0,
   Svy_ANS_MULTIPLE_CHOICE = 1,
  } Svy_AnswerType_t;
#define Svy_ANSWER_TYPE_DEFAULT Svy_ANS_UNIQUE_CHOICE

#define Svy_MAX_ANSWERS_PER_QUESTION	10
struct Svy_Question	// Must be initialized to 0 before using it
  {
   long QstCod;
   unsigned QstInd;
   Svy_AnswerType_t AnswerType;
   struct
     {
      char *Text;
     } AnsChoice[Svy_MAX_ANSWERS_PER_QUESTION];
   DenAll_DenyOrAllow_t AllowCommentsByStds;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Svy_ResetSurveys (struct Svy_Surveys *Surveys);

void Svy_SeeAllSurveys (void);
void Svy_ListAllSurveys (struct Svy_Surveys *Surveys);
void Svy_SeeOneSurvey (void);
void Svy_PutParSvyOrder (Dat_StartEndTime_t SelectedOrder);
void Svy_ReqCreatOrEditSvy (void);
void Svy_GetSurveyDataByCod (struct Svy_Survey *Svy);
void Svy_GetSurveyDataByFolder (struct Svy_Survey *Svy);
void Svy_FreeListSurveys (struct Svy_Surveys *Surveys);
void Svy_GetNotifSurvey (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                         char **ContentStr,
                         long SvyCod,Ntf_GetContent_t GetContent);
void Svy_AskRemSurvey (void);
void Svy_RemoveSurvey (void);
void Svy_AskResetSurvey (void);
void Svy_ResetSurvey (void);
void Svy_HideSurvey (void);
void Svy_UnhideSurvey (void);
void Svy_ReceiveSurvey (void);
void Svy_RemoveSurveys (Hie_Level_t HieLvl,long HieCod);

void Svy_ReqEditQuestion (void);
void Svy_ReceiveQst (void);

void Svy_ReqRemQst (void);
void Svy_RemoveQst (void);

void Svy_ReceiveSurveyAnswers (void);

unsigned Svy_GetNumCrsSurveys (Hie_Level_t HieLvl,unsigned *NumNotif);

//-------------------------------- Figures ------------------------------------
void Svy_GetAndShowSurveysStats (Hie_Level_t HieLvl);

#endif
