// swad_question.h: test/exam/game questions

#ifndef _SWAD_QST
#define _SWAD_QST
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include <time.h>		// For time_t

#include "swad_exam.h"
#include "swad_game.h"
#include "swad_question_type.h"
#include "swad_media.h"
#include "swad_string.h"
#include "swad_tag_type.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Qst_MAX_CHARS_ANSWER_OR_FEEDBACK	(1024 - 1)	// 1023
#define Qst_MAX_BYTES_ANSWER_OR_FEEDBACK	((Qst_MAX_CHARS_ANSWER_OR_FEEDBACK + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 16383

// Test images will be saved with:
// - maximum width of Tst_IMAGE_SAVED_MAX_HEIGHT
// - maximum height of Tst_IMAGE_SAVED_MAX_HEIGHT
// - maintaining the original aspect ratio (aspect ratio recommended: 3:2)
#define Qst_IMAGE_SAVED_MAX_WIDTH	768
#define Qst_IMAGE_SAVED_MAX_HEIGHT	768
#define Qst_IMAGE_SAVED_QUALITY		 90	// 1 to 100

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Qst_Constructor (struct Qst_Questions *Qsts);
void Qst_Destructor (struct Qst_Questions *Qsts);

void Qst_ReqEditQsts (void);
void Qst_ShowFormRequestEditQsts (struct Qst_Questions *Qsts);
void Qst_ShowFormAnswerTypes (const struct Qst_AnswerTypes *AnswerTypes);
void Qst_RequestSelectQstsForExamSet (struct Exa_Exams *Exams);
void Qst_RequestSelectQstsForGame (struct Gam_Games *Games);
void Qst_ShowFormRequestSelectQstsForExamSet (struct Exa_Exams *Exams,
                                              struct Qst_Questions *Qsts);
void Qst_ShowFormRequestSelectQstsForGame (struct Gam_Games *Games,
                                           struct Qst_Questions *Qsts);

void Qst_PutIconsRequestBankQsts (__attribute__((unused)) void *Args);
void Qst_PutIconsEditBankQsts (void *Qsts);

void Qst_WriteAnswerType (Qst_AnswerType_t AnswerType,ExaSet_Validity_t Validity);
void Qst_WriteQstStem (const char *Stem,const char *ClassStem,
		       HidVis_HiddenOrVisible_t HiddenOrVisible);
void Qst_WriteQstFeedback (const char *Feedback,const char *ClassFeedback);

void Qst_ListQuestionsToEdit (void);
void Qst_ListQuestionsToSelectForExamSet (struct Exa_Exams *Exams);
void Qst_ListQuestionsToSelectForGame (struct Gam_Games *Games);

void Qst_ListOneOrMoreQstsForEdition (struct Qst_Questions *Qsts,
                                      MYSQL_RES *mysql_res);
void Qst_WriteHeadingRowQuestionsForEdition (struct Qst_Questions *Qsts);
void Qst_WriteQuestionListing (struct Qst_Questions *Qsts,unsigned QstInd);

void Qst_ListOneOrMoreQstsForSelectionForExamSet (struct Exa_Exams *Exams,
						  unsigned NumQsts,
                                                  MYSQL_RES *mysql_res);
void Qst_ListOneOrMoreQstsForSelectionForGame (struct Gam_Games *Games,
					       unsigned NumQsts,
                                               MYSQL_RES *mysql_res);
void Qst_PutCheckboxToSelectAllQuestions (void);
void Qst_WriteQuestionRowForSelection (unsigned QstInd,struct Qst_Question *Qst);

void Qst_PutParsEditQst (void *Qsts);

void Qst_WriteAnswers (struct Qst_Question *Qst,
                       const char *ClassTxt,const char *ClassFeedback);

void Qst_ListOneQstToEdit (struct Qst_Questions *Qsts);

//-----------------------------------------------------------------------------

void Qst_WriteParQstCod (unsigned NumQst,long QstCod);

void Qst_CheckIfNumberOfAnswersIsOne (const struct Qst_Question *Qst);

void Qst_ChangeFormatOptionsText (struct Qst_Question *Qst);
void Qst_ChangeFormatOptionsFeedback (struct Qst_Question *Qst);

Qst_AnswerType_t Qst_ConvertFromStrAnsTypDBToAnsTyp (const char *StrAnsTypeDB);
Qst_AnswerType_t Qst_ConvertFromUnsignedStrToAnsTyp (const char *UnsignedStr);

void Qst_ShowFormEditOneQst (void);
void Qst_PutFormEditOneQst (struct Qst_Question *Qst);

void Qst_QstConstructor (struct Qst_Question *Qst);
void Qst_QstDestructor (struct Qst_Question *Qst);

Err_SuccessOrError_t Qst_AllocateTextChoiceAnswer (struct Qst_Question *Qst,
						   unsigned NumOpt);
void Qst_FreeTextChoiceAnswers (struct Qst_Question *Qst);
void Qst_FreeTextChoiceAnswer (struct Qst_Question *Qst,unsigned NumOpt);

void Qst_ResetMediaOfQuestion (struct Qst_Question *Qst);
void Qst_FreeMediaOfQuestion (struct Qst_Question *Qst);

Exi_Exist_t Qst_GetQstDataByCod (struct Qst_Question *Qst);
void Qst_GetQstOptionsByCod (MYSQL_RES **mysql_res,struct Qst_Question *Qst);
Qst_Shuffle_t Qst_GetShuffleFromYN (char Ch);
Qst_Shuffle_t Qst_GetParShuffle (void);
Qst_WrongOrCorrect_t Qst_GetCorrectFromYN (char Ch);
long Qst_GetMedCodFromDB (long HieCod,long QstCod,int NumOpt);
void Qst_GetMediaFromDB (long HieCod,long QstCod,int NumOpt,
                         struct Med_Media *Media);
void Qst_ReceiveQst (void);
void Qst_GetQstFromForm (struct Qst_Question *Qst);
Err_SuccessOrError_t Qst_CheckIfQstFormatIsCorrectAndCountNumOptions (struct Qst_Question *Qst);

Exi_Exist_t Qst_CheckIfQuestionExistsInDB (struct Qst_Question *Qst);

void Qst_MoveMediaToDefinitiveDirectories (struct Qst_Question *Qst);

void Qst_ReqRemSelectedQsts (void);
void Qst_RemoveSelectedQsts (void);
void Qst_PutIconToRemoveOneQst (void *QstCod);
void Qst_ReqRemOneQst (void);
void Qst_PutParsRemoveOnlyThisQst (void *QstCod);
void Qst_RemoveOneQst (void);
void Qst_RemoveOneQstFromDB (long HieCod,long QstCod);

void Qst_ChangeShuffleQst (void);

void Qst_PutParQstCod (void *QstCod);

void Qst_InsertOrUpdateQstTagsAnsIntoDB (struct Qst_Question *Qst);
void Qst_InsertOrUpdateQstIntoDB (struct Qst_Question *Qst);
void Qst_InsertAnswersIntoDB (struct Qst_Question *Qst);

unsigned Qst_CountNumAnswerTypesInList (const struct Qst_AnswerTypes *AnswerTypes);

void Qst_RemoveCrsQsts (long HieCod);
void Qst_RemoveMediaFromStemOfQst (long HieCod,long QstCod);
void Qst_RemoveMediaFromAllAnsOfQst (long HieCod,long QstCod);
void Qst_RemoveAllMedFilesFromStemOfAllQstsInCrs (long HieCod);
void Qst_RemoveAllMedFilesFromAnsOfAllQstsInCrs (long HieCod);

unsigned Qst_GetNumQuestions (Hie_Level_t HieLvl,Qst_AnswerType_t AnsType,
                              struct Qst_Stats *Stats);

void Qst_GetTestStats (Hie_Level_t HieLvl,Qst_AnswerType_t AnsType,struct Qst_Stats *Stats);

//-----------------------------------------------------------------------------

void Qst_GetIndexesFromStr (const char StrIndexesOneQst[Qst_MAX_BYTES_INDEXES_ONE_QST + 1],	// 0 1 2 3, 3 0 2 1, etc.
			    unsigned Indexes[Qst_MAX_OPTS_PER_QST]);
void Qst_GetAnswersFromStr (const char StrAnswersOneQst[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1],
			    HTM_Attributes_t UsrAnswers[Qst_MAX_OPTS_PER_QST]);

void Qst_ComputeAnswerScore (const char *Table,
			     struct Qst_PrintedQuestion *PrintedQst,
			     struct Qst_Question *Qst);

void Qst_ComputeAndShowGrade (unsigned NumQsts,double Score,double MaxGrade);
double Qst_ComputeGrade (unsigned NumQsts,double Score,double MaxGrade);

#endif
