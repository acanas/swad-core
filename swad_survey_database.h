// swad_survey_database.h: surveys, operations with database

#ifndef _SWAD_SVY_DB
#define _SWAD_SVY_DB
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_constant.h"
#include "swad_date.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//-------------------------------- Surveys ------------------------------------
void Svy_DB_UpdateNumUsrsNotifiedByEMailAboutSurvey (long SvyCod,
                                                     unsigned NumUsrsToBeNotifiedByEMail);
unsigned Svy_DB_GetListSurveys (MYSQL_RES **mysql_res,
                                unsigned ScopesAllowed,
                                unsigned HiddenAllowed,
                                Dat_StartEndTime_t SelectedOrder);
unsigned Svy_DB_GetDataOfSurveyByCod (MYSQL_RES **mysql_res,long SvyCod);
unsigned Svy_DB_GetSurveyTitleAndText (MYSQL_RES **mysql_res,long SvyCod);
void Svy_DB_GetSurveyTxt (long SvyCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);
bool Svy_DB_CheckIfSimilarSurveyExists (const struct Svy_Survey *Svy);
unsigned Svy_DB_GetNumCrssWithCrsSurveys (HieLvl_Level_t Scope);
unsigned Svy_DB_GetNumCrsSurveys (MYSQL_RES **mysql_res,HieLvl_Level_t Scope);
double Svy_DB_GetNumQstsPerCrsSurvey (HieLvl_Level_t Scope);
unsigned Svy_DB_GetUsrsFromSurveyExceptMe (MYSQL_RES **mysql_res,long SvyCod);

void Svy_DB_RemoveSvy (long SvyCod);

//---------------------------- Surveys groups ---------------------------------
bool Svy_DB_CheckIfICanDoThisSurveyBasedOnGrps (long SvyCod);

void Svy_DB_RemoveGrpsAssociatedToSurvey (long SvyCod);
void Svy_DB_RemoveGroupsOfType (long GrpTypCod);
void Svy_DB_RemoveGroup (long GrpCod);

//--------------------------- Surveys questions -------------------------------
void Svy_DB_ChangeIndexesQsts (long SvyCod,unsigned QstInd);

unsigned Svy_DB_GetNumQstsSvy (long SvyCod);
unsigned Svy_DB_GetSurveyQstsCodes (MYSQL_RES **mysql_res,long SvyCod);
unsigned Svy_DB_GetSurveyQsts (MYSQL_RES **mysql_res,long SvyCod);
unsigned Svy_DB_GetQstIndFromQstCod (long QstCod);

void Svy_DB_RemoveQst (long QstCod);
void Svy_DB_RemoveQstsSvy (long SvyCod);

//---------------------------- Surveys answers --------------------------------
void Svy_DB_ResetAnswersSvy (long SvyCod);
void Svy_DB_IncreaseAnswer (long QstCod,unsigned AnsInd);

bool Svy_DB_CheckIfAnswerExists (long QstCod,unsigned AnsInd);
unsigned Svy_DB_GetAnswersQst (MYSQL_RES **mysql_res,long QstCod);

void Svy_DB_RemoveAnswersQst (long QstCod);
void Svy_DB_RemoveAnswersSvy (long SvyCod);

//--------------------- Users who have answered surveys -----------------------
void Svy_DB_RegisterIHaveAnsweredSvy (long SvyCod);

bool Svy_DB_CheckIfIHaveAnsweredSvy (long SvyCod);
unsigned Svy_DB_GetNumUsrsWhoHaveAnsweredSvy (long SvyCod);

void Svy_DB_RemoveUsrsWhoHaveAnsweredSvy (long SvyCod);

#endif
