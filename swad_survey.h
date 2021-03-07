// swad_survey.h: surveys

#ifndef _SWAD_SVY
#define _SWAD_SVY
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Svy_MAX_CHARS_SURVEY_TITLE	(128 - 1)	// 127
#define Svy_MAX_BYTES_SURVEY_TITLE	((Svy_MAX_CHARS_SURVEY_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Svy_NUM_DATES 2
typedef enum
  {
   Svy_START_TIME = 0,
   Svy_END_TIME   = 1,
  } Svy_StartOrEndTime_t;
#define Svy_ORDER_DEFAULT Dat_START_TIME

struct Svy_Surveys
  {
   bool LstIsRead;	// Is the list already read from database, or it needs to be read?
   unsigned Num;	// Number of surveys
   long *LstSvyCods;	// List of survey codes
   Dat_StartEndTime_t SelectedOrder;
   unsigned CurrentPage;
   long SvyCod;
   long QstCod;
  };

struct Svy_Survey
  {
   long SvyCod;
   Hie_Lvl_Level_t Scope;
   long Cod;		// Country, institution, center, degree or course code
   unsigned Roles;	// Example: if survey can be made by students, Roles == (1 << Rol_STD)
   long UsrCod;
   char Title[Svy_MAX_BYTES_SURVEY_TITLE + 1];
   time_t TimeUTC[Svy_NUM_DATES];
   unsigned NumQsts;	// Number of questions in the survey
   unsigned NumUsrs;	// Number of distinct users who have already answered the survey
   struct
     {
      bool Visible;		// Survey is not hidden
      bool Open;		// Start date <= now <= end date
      bool IAmLoggedWithAValidRoleToAnswer;	// I am logged with a valid role to answer this survey
      bool IBelongToScope;	// I belong to the scope of this survey
      bool IHaveAnswered;	// I have already answered this survey
      bool ICanAnswer;
      bool ICanViewResults;
      bool ICanEdit;
     } Status;
  };

#define Svy_NUM_ANS_TYPES	2
typedef enum
  {
   Svy_ANS_UNIQUE_CHOICE   = 0,
   Svy_ANS_MULTIPLE_CHOICE = 1,
  } Svy_AnswerType_t;
#define Svy_ANSWER_TYPE_DEFAULT Svy_ANS_UNIQUE_CHOICE

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Svy_SeeAllSurveys (void);
void Svy_SeeOneSurvey (void);
void Svy_PutHiddenParamSvyOrder (Dat_StartEndTime_t SelectedOrder);
void Svy_RequestCreatOrEditSvy (void);
void Svy_GetDataOfSurveyByCod (struct Svy_Survey *Svy);
void Svy_GetDataOfSurveyByFolder (struct Svy_Survey *Svy);
void Svy_FreeListSurveys (struct Svy_Surveys *Surveys);
void Svy_GetNotifSurvey (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                         char **ContentStr,
                         long SvyCod,bool GetContent);
void Svy_AskRemSurvey (void);
void Svy_RemoveSurvey (void);
void Svy_AskResetSurvey (void);
void Svy_ResetSurvey (void);
void Svy_HideSurvey (void);
void Svy_UnhideSurvey (void);
void Svy_ReceiveFormSurvey (void);
void Svy_RemoveGroup (long GrpCod);
void Svy_RemoveGroupsOfType (long GrpTypCod);
void Svy_RemoveSurveys (Hie_Lvl_Level_t Scope,long Cod);

void Svy_RequestEditQuestion (void);
void Svy_ReceiveQst (void);

void Svy_RequestRemoveQst (void);
void Svy_RemoveQst (void);

void Svy_ReceiveSurveyAnswers (void);

unsigned Svy_GetNumCoursesWithCrsSurveys (Hie_Lvl_Level_t Scope);
unsigned Svy_GetNumCrsSurveys (Hie_Lvl_Level_t Scope,unsigned *NumNotif);
double Svy_GetNumQstsPerCrsSurvey (Hie_Lvl_Level_t Scope);

#endif
