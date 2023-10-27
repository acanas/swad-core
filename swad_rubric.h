// swad_rubric.h: assessment rubrics

#ifndef _SWAD_RUB
#define _SWAD_RUB
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_project.h"
#include "swad_rubric_type.h"
#include "swad_string.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define RubCri_AFTER_LAST_CRITERION	((unsigned)((1UL << 31) - 1))	// 2^31 - 1, don't change this number because it is used in database

#define Rub_NUM_CHECK_RUB_COD 2
typedef enum
  {
   Rub_DONT_CHECK_RUB_COD = 0,
   Rub_CHECK_RUB_COD      = 1,
  } Rub_CheckRubCod_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Rub_ResetRubrics (struct Rub_Rubrics *Rubrics);
void Rub_RubricConstructor (struct Rub_Rubric *Rubric);
void Rub_RubricDestructor (struct Rub_Rubric *Rubric);

void Rub_SeeAllRubrics (void);
void Rub_ListAllRubrics (struct Rub_Rubrics *Rubrics);
bool Rub_CheckIfICanEditRubrics (void);
bool Rub_CheckIfEditable (void);
void Rub_SeeOneRubric (void);
void Rub_ShowOnlyOneRubric (struct Rub_Rubrics *Rubrics);
void Rub_ShowRubricMainData (struct Rub_Rubrics *Rubrics,
                             bool ShowOnlyThisRubric);

void Rub_SetCurrentRubCod (long GamCod);
void Rub_PutPars (void *Rubrics);
void Rub_GetPars (struct Rub_Rubrics *Rubrics,Rub_CheckRubCod_t CheckRubCod);

void Rub_GetListRubrics (struct Rub_Rubrics *Rubrics);
void Rub_GetRubricDataByCod (struct Rub_Rubric *Rubric);
void Rub_FreeListRubrics (struct Rub_Rubrics *Rubrics);

void Rub_AskRemRubric (void);
void Rub_RemoveRubric (void);
void Rub_RemoveCrsRubrics (long CrsCod);

void Rub_ReqCreatOrEditRubric (void);
void Rub_PutFormsOneRubric (struct Rub_Rubrics *Rubrics,
			    Rub_ExistingNewRubric_t ExistingNewRubric);

void Rub_ReceiveFormRubric (void);

//-------------------------------- Figures ------------------------------------
void Rub_GetAndShowRubricsStats (void);

#endif
