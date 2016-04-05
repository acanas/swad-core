// swad_survey.h: surveys

#ifndef _SWAD_SVY
#define _SWAD_SVY
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

#include "swad_date.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Svy_MAX_LENGTH_SURVEY_TITLE	255

#define Svy_NUM_DATES 2
typedef enum
  {
   Svy_START_TIME = 0,
   Svy_END_TIME   = 1,
  } Svy_StartOrEndTime_t;

struct Survey
  {
   long SvyCod;
   long DegCod;
   long CrsCod;
   unsigned Roles;	// Example: if survey can be made by students and teachers, Roles == (1 << Rol_ROLE_STUDENT) | (1 << Rol_ROLE_TEACHER)
   long UsrCod;
   time_t TimeUTC[Svy_NUM_DATES];
   char Title[Svy_MAX_LENGTH_SURVEY_TITLE+1];
   unsigned NumQsts;	// Number of questions in the survey
   unsigned NumUsrs;	// Number of distinct users who have already answered the survey
   struct
     {
      bool Visible;		// Survey is not hidden
      bool Open;		// Start date <= now <= end date
      bool IAmLoggedWithAValidRoleToAnswer;	// I am logged with a valid role to answer this survey
      bool IBelongToDegCrsGrps;	// I can answer this survey (it is associated to no groups or (if associated to groups) I belong to any of the groups
      bool IHaveAnswered;	// I have already answered this survey
      bool ICanAnswer;
      bool ICanViewResults;
      bool ICanEdit;
     } Status;
  };

typedef enum
  {
   Svy_ORDER_BY_START_DATE = 0,
   Svy_ORDER_BY_END_DATE   = 1,
  } tSvysOrderType;

#define Svy_NUM_ANS_TYPES	2

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Svy_SeeAllSurveys (void);
void Svy_SeeOneSurvey (void);
void Svy_PutHiddenParamSvyOrderType (void);
void Svy_RequestCreatOrEditSvy (void);
void Svy_GetListSurveys (void);
void Svy_GetDataOfSurveyByCod (struct Survey *Svy);
void Svy_GetDataOfSurveyByFolder (struct Survey *Svy);
void Svy_FreeListSurveys (void);
void Svy_GetNotifSurvey (char *SummaryStr,char **ContentStr,long SvyCod,unsigned MaxChars,bool GetContent);
void Svy_AskRemSurvey (void);
void Svy_RemoveSurvey (void);
void Svy_AskResetSurvey (void);
void Svy_ResetSurvey (void);
void Svy_HideSurvey (void);
void Svy_UnhideSurvey (void);
void Svy_RecFormSurvey (void);
bool Svy_CheckIfSvyIsAssociatedToGrp (long SvyCod,long GrpCod);
void Svy_RemoveGroup (long GrpCod);
void Svy_RemoveGroupsOfType (long GrpTypCod);
void Svy_RemoveDegSurveys (long DegCod);
void Svy_RemoveCrsSurveys (long CrsCod);

void Svy_RequestEditQuestion (void);
void Svy_ReceiveQst (void);

void Svy_RequestRemoveQst (void);
void Svy_RemoveQst (void);

void Svy_ReceiveSurveyAnswers (void);

unsigned Svy_GetNumCoursesWithSurveys (Sco_Scope_t Scope);
unsigned Svy_GetNumSurveys (Sco_Scope_t Scope,unsigned *NumNotif);
float Svy_GetNumQstsPerSurvey (Sco_Scope_t Scope);

#endif
