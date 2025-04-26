// swad_exam_database.h: exams operations with database

#ifndef _SWAD_EXA_DB
#define _SWAD_EXA_DB
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_exam_log.h"
#include "swad_exam_print.h"
#include "swad_exam_session.h"
#include "swad_exam_set.h"
#include "swad_question_type.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//---------------------------- Exams ------------------------------------------
long Exa_DB_CreateExam (const struct Exa_Exam *Exam,const char *Txt);
void Exa_DB_UpdateExam (const struct Exa_Exam *Exam,const char *Txt);
void Exa_DB_HideOrUnhideExam (long ExaCod,
			      HidVis_HiddenOrVisible_t HiddenOrVisible);
unsigned Exa_DB_GetListExams (MYSQL_RES **mysql_res,Exa_Order_t SelectedOrder);
unsigned Exa_DB_GetExamDataByCod (MYSQL_RES **mysql_res,long ExaCod);
unsigned Exa_DB_GetExamStartEnd (MYSQL_RES **mysql_res,long ExaCod);
void Exa_DB_GetExamTitle (long ExaCod,char *Title,size_t TitleSize);
void Exa_DB_GetExamTxt (long ExaCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);
bool Exa_DB_CheckIfSimilarExamExists (long CrsCod,long ExaCod,const char *Title);
unsigned Exa_DB_GetNumCoursesWithExams (Hie_Level_t HieLvl);
unsigned Exa_DB_GetNumExams (Hie_Level_t HieLvl);
double Exa_DB_GetNumQstsPerCrsExam (Hie_Level_t HieLvl);
void Exa_DB_RemoveExam (long ExaCod);
void Exa_DB_RemoveAllExamsFromCrs (long CrsCod);

//--------------------------------- Sets --------------------------------------
long Exa_DB_CreateSet (const struct ExaSet_Set *Set);
void Exa_DB_UpdateSetTitle (long SetCod,long ExaCod,
                            const char NewTitle[ExaSet_MAX_BYTES_TITLE + 1]);
void Exa_DB_UpdateNumQstsToExam (long SetCod,long ExaCod,unsigned NumQstsToPrint);
void Exa_DB_UpdateSetIndexesInExamGreaterThan (long ExaCod,unsigned SetInd);
void Exa_DB_UpdateSetIndex (long SetInd,long SetCod,long ExaCod);
void Exa_DB_LockTables (void);
unsigned Exa_DB_GetNumSetsExam (long ExaCod);
unsigned Exa_DB_GetNumQstsExam (long ExaCod);
unsigned Exa_DB_GetExamSets (MYSQL_RES **mysql_res,long ExaCod);
unsigned Exa_DB_GetSetDataByCod (MYSQL_RES **mysql_res,long SetCod);
bool Exa_DB_CheckIfSimilarSetExists (const struct ExaSet_Set *Set,
                                     const char Title[ExaSet_MAX_BYTES_TITLE + 1]);
unsigned Exa_DB_GetSetIndFromSetCod (const struct ExaSet_Set *Set);
long Exa_DB_GetSetCodFromSetInd (long ExaCod,unsigned SetInd);
unsigned Exa_DB_GetMaxSetIndexInExam (long ExaCod);
unsigned Exa_DB_GetPrevSetIndexInExam (long ExaCod,unsigned SetInd);
unsigned Exa_DB_GetNextSetIndexInExam (long ExaCod,unsigned SetInd);
void Exa_DB_RemoveSetFromExam (const struct ExaSet_Set *Set);
void Exa_DB_RemoveAllSetsFromExam (long ExaCod);
void Exa_DB_RemoveAllSetsFromCrs (long CrsCod);

//------------------------------ Set questions --------------------------------
long Exa_DB_AddQuestionToSet (long SetCod,const struct Qst_Question *Question,long MedCod);
void Exa_DB_ChangeValidityQst (long QstCod,long SetCod,long ExaCod,long CrsCod,
                               ExaSet_Validity_t Validity);
unsigned Exa_DB_GetNumQstsInSet (long SetCod);
unsigned Exa_DB_GetQstsFromSet (MYSQL_RES **mysql_res,long SetCod);
unsigned Exa_DB_GetSomeQstsFromSetToPrint (MYSQL_RES **mysql_res,
                                           long SetCod,unsigned NumQstsToPrint);
unsigned Exa_DB_GetQstDataByCod (MYSQL_RES **mysql_res,long QstCod);
unsigned Exa_DB_GetValidityAndAnswerType (MYSQL_RES **mysql_res,long QstCod);
unsigned Exa_DB_GetAnswerType (MYSQL_RES **mysql_res,long QstCod);
unsigned Exa_DB_GetMediaFromStemOfQst (MYSQL_RES **mysql_res,long QstCod,long SetCod);
unsigned Exa_DB_GetAllMediaFomStemOfAllQstsFromExam (MYSQL_RES **mysql_res,long ExaCod);
unsigned Exa_DB_GetAllMediaFomStemOfAllQstsFromCrs (MYSQL_RES **mysql_res,long CrsCod);
void Exa_DB_RemoveSetQuestion (long QstCod,long SetCod);
void Exa_DB_RemoveAllSetQuestionsFromSet (const struct ExaSet_Set *Set);
void Exa_DB_RemoveAllSetQuestionsFromExam (long ExaCod);
void Exa_DB_RemoveAllSetQuestionsFromCrs (long CrsCod);

//--------------------------- Set question answers ----------------------------
void Exa_DB_AddAnsToQstInSet (long QstCod,unsigned AnsInd,
                              const char *Answer,const char *Feedback,
                              long MedCod,Qst_WrongOrCorrect_t WrongOrCorrect);
unsigned Exa_DB_GetQstAnswersFromSet (MYSQL_RES **mysql_res,long QstCod,
				      Qst_Shuffle_t Shuffle);
unsigned Exa_DB_GetQstAnswersTextFromSet (MYSQL_RES **mysql_res,long QstCod);
unsigned Exa_DB_GetQstAnswersCorrFromSet (MYSQL_RES **mysql_res,long QstCod);
unsigned Exa_DB_GetMediaFromAllAnsOfQst (MYSQL_RES **mysql_res,long QstCod,long SetCod);
unsigned Exa_DB_GetAllMediaFromAnsOfAllQstsFromExam (MYSQL_RES **mysql_res,long ExaCod);
unsigned Exa_DB_GetAllMediaFromAnsOfAllQstsFromCrs (MYSQL_RES **mysql_res,long CrsCod);
void Exa_DB_RemoveAllSetAnswersFromExam (long ExaCod);
void Exa_DB_RemoveAllSetAnswersFromCrs (long CrsCod);

//-------------------------------- Sessions -----------------------------------
long Exa_DB_CreateSession (const struct ExaSes_Session *Session);
void Exa_DB_UpdateSession (const struct ExaSes_Session *Session);
unsigned Exa_DB_GetNumSessionsInExam (long ExaCod);
unsigned Exa_DB_GetNumOpenSessionsInExam (long ExaCod);
unsigned Exa_DB_GetSessions (MYSQL_RES **mysql_res,long ExaCod);
unsigned Exa_DB_GetSessionDataByCod (MYSQL_RES **mysql_res,long SesCod);
void Exa_DB_GetSessionTitle (long SesCod,char Title[ExaSes_MAX_BYTES_TITLE + 1]);
void Exa_DB_ToggleVisResultsSesUsr (const struct ExaSes_Session *Session);
void Exa_DB_HideUnhideSession (const struct ExaSes_Session *Session,
			       HidVis_HiddenOrVisible_t HiddenOrVisible);
void Exa_DB_RemoveSessionFromAllTables (long SesCod);
void Exa_DB_RemoveAllSessionsFromExam (long ExaCod);
void Exa_DB_RemoveAllSessionsFromCrs (long CrsCod);
void Exa_DB_RemoveUsrFromSessionTablesInCrs (long UsrCod,long CrsCod);

//--------------------------------- Groups ------------------------------------
void Exa_DB_CreateGrpAssociatedToSes (long SesCod,long GrpCod);
unsigned Exa_DB_GetGrpCodsAssociatedToSes (MYSQL_RES **mysql_res,long SesCod);
unsigned Exa_DB_GetGrpsAssociatedToSes (MYSQL_RES **mysql_res,long SesCod);
Usr_Can_t Exa_DB_CheckIfICanListThisSessionBasedOnGrps (long SesCod);
void Exa_DB_RemoveAllGrpsFromExa (long ExaCod);
void Exa_DB_RemoveAllGrpsFromCrs (long CrsCod);
void Exa_DB_RemoveAllGrpsFromSes (long SesCod);
void Exa_DB_RemoveAllGrpsOfType (long GrpTypCod);
void Exa_DB_RemoveGroup (long GrpCod);

//---------------------------------- Prints -----------------------------------
long Exa_DB_CreatePrint (const struct ExaPrn_Print *Print,bool Start);
void Exa_DB_UpdatePrint (const struct ExaPrn_Print *Print);
unsigned Exa_DB_GetPrintDataByPrnCod (MYSQL_RES **mysql_res,long PrnCod);
unsigned Exa_DB_GetPrintDataBySesCodAndUsrCod (MYSQL_RES **mysql_res,
                                               long SesCod,long UsrCod);
void Exa_DB_RemoveAllPrintsMadeByUsrInAllCrss (long UsrCod);
void Exa_DB_RemoveAllPrintsMadeByUsrInCrs (long UsrCod,long CrsCod);
void Exa_DB_RemoveAllPrintsFromSes (long SesCod);
void Exa_DB_RemoveAllPrintsFromExa (long ExaCod);
void Exa_DB_RemoveAllPrintsFromCrs (long CrsCod);

//------------------------------- Print questions -----------------------------
void Exa_DB_StoreOneQstOfPrint (const struct ExaPrn_Print *Print,
                                unsigned QstInd);
unsigned Exa_DB_GetPrintQuestions (MYSQL_RES **mysql_res,long PrnCod);
void Exa_DB_GetAnswersFromQstInPrint (long PrnCod,long QstCod,
                                      char StrAnswers[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1]);
unsigned Exa_DB_GetNumQstsNotBlankInPrint (long PrnCod);
double Exa_DB_ComputeTotalScoreOfPrint (long PrnCod);
void Exa_DB_RemovePrintQstsMadeByUsrInAllCrss (long UsrCod);
void Exa_DB_RemovePrintQstsMadeByUsrInCrs (long UsrCod,long CrsCod);
void Exa_DB_RemovePrintQstsFromSes (long SesCod);
void Exa_DB_RemovePrintQstsFromExa (long ExaCod);
void Exa_DB_RemovePrintQstsFromCrs (long CrsCod);

//---------------------------------- Logs -------------------------------------
bool Exa_DB_CheckIfSessionIsTheSameAsTheLast (long PrnCod);
bool Exa_DB_CheckIfUserAgentIsTheSameAsTheLast (long PrnCod,const char *UserAgentDB);
void Exa_DB_LogAccess (long LogCod,long PrnCod,ExaLog_Action_t Action);
void Exa_DB_LogSession (long LogCod,long PrnCod);
void Exa_DB_LogUserAgent (long LogCod,long PrnCod,const char *UserAgentDB);
unsigned Exa_DB_GetExamLog (MYSQL_RES **mysql_res,long PrnCod);

//--------------------------------- Results -----------------------------------
unsigned Exa_DB_GetAllUsrsWhoHaveMadeExam (MYSQL_RES **mysql_res,long ExaCod);
unsigned Exa_DB_GetAllUsrsWhoHaveMadeSession (MYSQL_RES **mysql_res,long SesCod);
unsigned Exa_DB_GetNumPrintsInSession (long SesCod);
unsigned Exa_DB_GetResults (MYSQL_RES **mysql_res,
			    Usr_MeOrOther_t MeOrOther,long UsrCod,
			    long SesCod,	// <= 0 ==> any
			    long ExaCod,	// <= 0 ==> any
			    const char *ExamsSelectedCommas);

#endif
