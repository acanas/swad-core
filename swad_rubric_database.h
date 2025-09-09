// swad_rubric_database.h: assessment rubrics, operations with database

#ifndef _SWAD_RUB_DB
#define _SWAD_RUB_DB
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_game.h"
#include "swad_hierarchy_type.h"
#include "swad_rubric_type.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//-------------------------------- Rubrics ------------------------------------
long Rub_DB_CreateRubric (const struct Rub_Rubric *Rubric);
void Rub_DB_UpdateRubric (const struct Rub_Rubric *Rubric);

unsigned Rub_DB_GetListRubrics (MYSQL_RES **mysql_res);
Exi_Exist_t Rub_DB_GetRubricDataByCod (MYSQL_RES **mysql_res,long RubCod);
void Rub_DB_GetRubricTitle (long RubCod,char *Title,size_t TitleSize);
void Rub_DB_GetRubricTxt (struct Rub_Rubric *Rubric);
Exi_Exist_t Rub_DB_CheckIfSimilarRubricExists (const struct Rub_Rubric *Rubric);
unsigned Rub_DB_GetNumCoursesWithRubrics (Hie_Level_t HieLvl);
unsigned Rub_DB_GetNumRubrics (Hie_Level_t HieLvl);

void Rub_DB_RemoveRubric (long RubCod);
void Rub_DB_RemoveCrsRubrics (long CrsCod);

//---------------------------- Rubric criteria --------------------------------
long Rub_DB_CreateCriterion (const struct RubCri_Criterion *Criterion);
void Rub_DB_UpdateCriterionTitle (const struct RubCri_Criterion *Criterion);
void Rub_DB_UpdateCriterionLink (const struct RubCri_Criterion *Criterion);
void Rub_DB_UpdateCriterionValue (const struct RubCri_Criterion *Criterion,
                                  RubCri_ValueRange_t ValueRange);
void Rub_DB_UpdateCriterionWeight (const struct RubCri_Criterion *Criterion);
void Rub_DB_UpdateCriteriaIndexesInRubricGreaterThan (const struct RubCri_Criterion *Criterion);
void Rub_DB_UpdateCriterionIndex (long CriInd,long CriCod,long RubCod);
void Rub_DB_LockTable (void);

unsigned Rub_DB_GetNumCriteriaInRubric (long RubCod);
long Rub_DB_GetRubCodFromCriCod (long CriCod);
long Rub_DB_GetCriCodFromCriInd (long RubCod,unsigned QstInd);
unsigned Rub_DB_GetCriIndFromCriCod (long RubCod,long CriCod);
unsigned Rub_DB_GetMaxCriterionIndexInRubric (long RubCod);
unsigned Rub_DB_GetPrevCriterionIndexInRubric (long RubCod,unsigned QstInd);
unsigned Rub_DB_GetNextCriterionIndexInRubric (long RubCod,unsigned QstInd);
unsigned Rub_DB_GetCriteria (MYSQL_RES **mysql_res,long RubCod);
Exi_Exist_t Rub_DB_GetCriterionDataByCod (MYSQL_RES **mysql_res,long CriCod);
Exi_Exist_t Rub_DB_CheckIfSimilarCriterionExists (const struct RubCri_Criterion *Criterion,
						  const char Title[RubCri_MAX_BYTES_TITLE + 1]);

double Rub_DB_GetNumCriteriaPerRubric (Hie_Level_t HieLvl);

void Rub_DB_RemoveCriterionFromRubric (const struct RubCri_Criterion *Criterion);
void Rub_DB_RemoveRubricCriteria (long RubCod);
void Rub_DB_RemoveCrsRubricCriteria (long CrsCod);

//----------------------------- Rubric scores ---------------------------------
void Rub_DB_UpdateScore (Rsc_Type_t RscType,long RscCod,long UsrCod,long CriCod,
                         double Score);

double Rub_DB_GetScore (Rsc_Type_t RscType,long RscCod,long UsrCod,long CriCod);

#endif
