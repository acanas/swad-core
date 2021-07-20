// swad_exam_database.h: exams operations with database

#ifndef _SWAD_EXA_DB
#define _SWAD_EXA_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Ca�as Vargas

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_exam_log.h"
#include "swad_exam_print.h"
#include "swad_exam_session.h"
#include "swad_test_type.h"

/*****************************************************************************/
/************************* Public types and constants ************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

unsigned Exa_DB_GetExamSets (MYSQL_RES **mysql_res,long ExaCod);
unsigned Exa_DB_GetSomeQstsFromSetToPrint (MYSQL_RES **mysql_res,
                                           long SetCod,unsigned NumQstsToPrint);
unsigned Exa_DB_GetValidityAndTypeOfQuestion (MYSQL_RES **mysql_res,long QstCod);
unsigned Exa_DB_GetQstAnswersTextFromSet (MYSQL_RES **mysql_res,long QstCod);
unsigned Exa_DB_GetQstAnswersCorrFromSet (MYSQL_RES **mysql_res,long QstCod);

long Exa_DB_CreateSession (const struct ExaSes_Session *Session);
void Exa_DB_UpdateSession (const struct ExaSes_Session *Session);
unsigned Exa_DB_GetNumSessionsInExam (long ExaCod);
unsigned Exa_DB_GetNumOpenSessionsInExam (long ExaCod);
unsigned Exa_DB_GetSessions (MYSQL_RES **mysql_res,long ExaCod);
unsigned Exa_DB_GetDataOfSessionByCod (MYSQL_RES **mysql_res,long SesCod);
void Exa_DB_ToggleVisResultsSesUsr (const struct ExaSes_Session *Session);
void Exa_DB_HideUnhideSession (const struct ExaSes_Session *Session,bool Hide);
void Exa_DB_RemoveSessionFromAllTables (long SesCod);
void Exa_DB_RemoveSessionsInExamFromAllTables (long ExaCod);
void Exa_DB_RemoveSessionInCourseFromAllTables (long CrsCod);
void Exa_DB_RemoveUsrFromSessionTablesInCrs (long UsrCod,long CrsCod);

void Exa_DB_CreateGrpAssociatedToSession (long SesCod,long GrpCod);
unsigned Exa_DB_GetGrpsAssociatedToSession (MYSQL_RES **mysql_res,long SesCod);
bool Exa_DB_CheckIfICanListThisSessionBasedOnGrps (long SesCod);
void Exa_DB_RemoveAllGrpsAssociatedToSession (long SesCod);
void Exa_DB_RemoveGroupsOfType (long GrpTypCod);
void Exa_DB_RemoveGrpAssociatedToExamSessions (long GrpCod);

long Exa_DB_CreatePrint (const struct ExaPrn_Print *Print);
void Exa_DB_UpdatePrint (const struct ExaPrn_Print *Print);
unsigned Exa_DB_GetDataOfPrintByPrnCod (MYSQL_RES **mysql_res,long PrnCod);
unsigned Exa_DB_GetDataOfPrintBySesCodAndUsrCod (MYSQL_RES **mysql_res,
                                                 long SesCod,long UsrCod);
void Exa_DB_RemovePrintsMadeByUsrInAllCrss (long UsrCod);
void Exa_DB_RemovePrintsMadeByUsrInCrs (long UsrCod,long CrsCod);
void Exa_DB_RemovePrintsInCrs (long CrsCod);

void Exa_DB_StoreOneQstOfPrint (const struct ExaPrn_Print *Print,
                                unsigned QstInd);
unsigned Exa_DB_GetPrintQuestions (MYSQL_RES **mysql_res,long PrnCod);
void Exa_DB_GetAnswersFromQstInPrint (long PrnCod,long QstCod,
                                      char StrAnswers[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1]);
unsigned Exa_DB_GetNumQstsNotBlankInPrint (long PrnCod);
double Exa_DB_ComputeTotalScoreOfPrint (long PrnCod);
void Exa_DB_RemovePrintQuestionsMadeByUsrInAllCrss (long UsrCod);
void Exa_DB_RemovePrintsQuestionsMadeByUsrInCrs (long UsrCod,long CrsCod);
void Exa_DB_RemovePrintQuestionsInCrs (long CrsCod);

bool Exa_DB_CheckIfSessionIsTheSameAsTheLast (long PrnCod);
bool Exa_DB_CheckIfUserAgentIsTheSameAsTheLast (long PrnCod,const char *UserAgentDB);
void Exa_DB_LogAccess (long LogCod,long PrnCod,ExaLog_Action_t Action);
void Exa_DB_LogSession (long LogCod,long PrnCod);
void Exa_DB_LogUserAgent (long LogCod,long PrnCod,const char *UserAgentDB);
unsigned Exa_DB_GetExamLog (MYSQL_RES **mysql_res,long PrnCod);

unsigned Exa_DB_GetAllUsrsWhoHaveMadeExam (MYSQL_RES **mysql_res,long ExaCod);
unsigned Exa_DB_GetAllUsrsWhoHaveMadeSession (MYSQL_RES **mysql_res,long SesCod);
unsigned Exa_DB_GetResults (MYSQL_RES **mysql_res,
			    Usr_MeOrOther_t MeOrOther,
			    long SesCod,	// <= 0 ==> any
			    long ExaCod,	// <= 0 ==> any
			    const char *ExamsSelectedCommas);

#endif
