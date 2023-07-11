// swad_question_database.h: test/exam/game questions, operations with database

#ifndef _SWAD_QST_DB
#define _SWAD_QST_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/***************************** Public prototypes *****************************/
/*****************************************************************************/

long Qst_DB_CreateQst (const struct Qst_Question *Question);
void Qst_DB_UpdateQst (const struct Qst_Question *Question);
void Qst_DB_UpdateQstScore (long QstCod,bool AnswerIsNotBlank,double Score);
void Qst_DB_UpdateQstShuffle (long QstCod,bool Shuffle);
//-----------------------------------------------------------------------------
void Qst_DB_CreateIntAnswer (struct Qst_Question *Question);
void Qst_DB_CreateFltAnswer (struct Qst_Question *Question);
void Qst_DB_CreateTF_Answer (struct Qst_Question *Question);
void Qst_DB_CreateChoAnswer (struct Qst_Question *Question);
//-----------------------------------------------------------------------------

unsigned Qst_DB_GetQsts (MYSQL_RES **mysql_res,
                         const struct Qst_Questions *Questions);
unsigned Qst_DB_GetQstsForNewTestPrint (MYSQL_RES **mysql_res,
                                        const struct Qst_Questions *Questions);
unsigned Qst_DB_GetTrivialQst (MYSQL_RES **mysql_res,
                               char DegreesStr[API_MAX_BYTES_DEGREES_STR + 1],
                               float lowerScore,float upperScore);
unsigned Qst_DB_GetNumQstsInCrs (long CrsCod);
unsigned Qst_DB_GetNumQsts (MYSQL_RES **mysql_res,
                            HieLvl_Level_t Level,Qst_AnswerType_t AnsType);
unsigned Qst_DB_GetNumCrssWithQsts (HieLvl_Level_t Level,
                                    Qst_AnswerType_t AnsType);
unsigned Qst_DB_GetNumCrssWithPluggableQsts (HieLvl_Level_t Level,
                                             Qst_AnswerType_t AnsType);

unsigned Qst_DB_GetRecentQuestions (MYSQL_RES **mysql_res,
                                    long CrsCod,time_t BeginTime);
unsigned Qst_DB_GetRecentAnswers (MYSQL_RES **mysql_res,
                                  long CrsCod,time_t BeginTime);

unsigned Qst_DB_GetQstDataByCod (MYSQL_RES **mysql_res,long QstCod);
Qst_AnswerType_t Qst_DB_GetQstAnswerType (long QstCod);
long Qst_DB_GetQstMedCod (long CrsCod,long QstCod);
unsigned Qst_DB_GetQstCodFromTypeAnsStem (MYSQL_RES **mysql_res,
                                          const struct Qst_Question *Question);

unsigned Qst_DB_GetNumAnswersQst (long QstCod);
unsigned Qst_DB_GetAnswersData (MYSQL_RES **mysql_res,long QstCod,bool Shuffle);
unsigned Qst_DB_GetTextOfAnswers (MYSQL_RES **mysql_res,long QstCod);
unsigned Qst_DB_GetQstAnswersCorr (MYSQL_RES **mysql_res,long QstCod);
unsigned Qst_DB_GetShuffledAnswersIndexes (MYSQL_RES **mysql_res,
                                           const struct Qst_Question *Question);
unsigned Qst_DB_GetMedCodsFromStemsOfQstsInCrs (MYSQL_RES **mysql_res,long CrsCod);
unsigned Qst_DB_GetMedCodsFromAnssOfQstsInCrs (MYSQL_RES **mysql_res,long CrsCod);
unsigned Qst_DB_GetMedCodFromStemOfQst (MYSQL_RES **mysql_res,long CrsCod,long QstCod);
unsigned Qst_DB_GetMedCodsFromAnssOfQst (MYSQL_RES **mysql_res,long CrsCod,long QstCod);
long Qst_DB_GetMedCodFromAnsOfQst (long QstCod,unsigned AnsInd);

void Qst_DB_RemoveQstsInCrs (long CrsCod);
void Qst_DB_RemoveQst (long CrsCod,long QstCod);
void Qst_DB_RemAnssFromQstsInCrs (long CrsCod);
void Qst_DB_RemAnsFromQst (long QstCod);

#endif
