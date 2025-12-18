// swad_question_database.h: test/exam/game questions, operations with database

#ifndef _SWAD_QST_DB
#define _SWAD_QST_DB
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

#include "swad_test_print.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

long Qst_DB_CreateQst (const struct Qst_Question *Qst);
void Qst_DB_UpdateQst (const struct Qst_Question *Qst);
void Qst_DB_UpdateQstScore (const struct TstPrn_Print *Print,unsigned QstInd);
void Qst_DB_UpdateQstShuffle (long QstCod,Qst_Shuffle_t Shuffle);
//-----------------------------------------------------------------------------
void Qst_DB_CreateIntAnswer (struct Qst_Question *Qst);
void Qst_DB_CreateFltAnswer (struct Qst_Question *Qst);
void Qst_DB_CreateTF_Answer (struct Qst_Question *Qst);
void Qst_DB_CreateChoAnswer (struct Qst_Question *Qst);
//-----------------------------------------------------------------------------

unsigned Qst_DB_GetQsts (MYSQL_RES **mysql_res,const struct Qst_Questions *Qsts);
unsigned Qst_DB_GetQstsForNewTestPrint (MYSQL_RES **mysql_res,
                                        const struct Qst_Questions *Qsts);
unsigned Qst_DB_GetTrivialQst (MYSQL_RES **mysql_res,
                               char DegreesStr[API_MAX_BYTES_DEGREES_STR + 1],
                               float lowerScore,float upperScore);
unsigned Qst_DB_GetNumQstsInCrs (long HieCod);
unsigned Qst_DB_GetNumQsts (MYSQL_RES **mysql_res,
                            Hie_Level_t HieLvl,Qst_AnswerType_t AnsType);
unsigned Qst_DB_GetNumCrssWithQsts (Hie_Level_t HieLvl,Qst_AnswerType_t AnsType);
unsigned Qst_DB_GetNumCrssWithPluggableQsts (Hie_Level_t HieLvl,
                                             Qst_AnswerType_t AnsType);

unsigned Qst_DB_GetRecentQuestions (MYSQL_RES **mysql_res,
                                    long HieCod,time_t BeginTime);
unsigned Qst_DB_GetRecentAnswers (MYSQL_RES **mysql_res,
                                  long HieCod,time_t BeginTime);

Exi_Exist_t Qst_DB_GetQstDataByCod (MYSQL_RES **mysql_res,long QstCod);
Qst_AnswerType_t Qst_DB_GetQstAnswerType (long QstCod);
long Qst_DB_GetQstMedCod (long HieCod,long QstCod);
unsigned Qst_DB_GetQstCodsFromTypeAnsStem (MYSQL_RES **mysql_res,
                                           const struct Qst_Question *Qst);

unsigned Qst_DB_GetNumAnswersQst (long QstCod);
unsigned Qst_DB_GetAnswersData (MYSQL_RES **mysql_res,long QstCod,
			        Qst_Shuffle_t Shuffle);
unsigned Qst_DB_GetTextOfAnswers (MYSQL_RES **mysql_res,
				  const char *Table,long QstCod);
unsigned Qst_DB_GetQstAnswersCorr (MYSQL_RES **mysql_res,
				   const char *Table,long QstCod);
unsigned Qst_DB_GetShuffledAnswersIndexes (MYSQL_RES **mysql_res,
                                           const struct Qst_Question *Qst);
unsigned Qst_DB_GetMedCodsFromStemsOfQstsInCrs (MYSQL_RES **mysql_res,long HieCod);
unsigned Qst_DB_GetMedCodsFromAnssOfQstsInCrs (MYSQL_RES **mysql_res,long HieCod);
unsigned Qst_DB_GetMedCodFromStemOfQst (MYSQL_RES **mysql_res,long HieCod,long QstCod);
unsigned Qst_DB_GetMedCodsFromAnssOfQst (MYSQL_RES **mysql_res,long HieCod,long QstCod);
long Qst_DB_GetMedCodFromAnsOfQst (long QstCod,unsigned AnsInd);

void Qst_DB_RemoveQstsInCrs (long HieCod);
void Qst_DB_RemoveQst (long HieCod,long QstCod);
void Qst_DB_RemAnssFromQstsInCrs (long HieCod);
void Qst_DB_RemAnsFromQst (long QstCod);

#endif
