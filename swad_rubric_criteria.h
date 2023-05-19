// swad_rubric_criteria.h: criteria in assessment rubrics

#ifndef _SWAD_RUB_CRI
#define _SWAD_RUB_CRI
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

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void RubCri_GetCriterionDataByCod (struct RubCri_Criterion *Criterion);

void RubCri_ReceiveFormCriterion (void);
void RubCri_ChangeTitle (void);
void RubCri_ChangeMinValue (void);
void RubCri_ChangeMaxValue (void);
void RubCri_ChangeWeight (void);

void RubCri_ListCriteriaForSeeing (const struct Rub_Rubrics *Rubrics);
void RubCri_ListCriteriaForEdition (struct Rub_Rubrics *Rubrics);
void RubCri_ListCriteriaInProject (struct Prj_Projects *Projects,long RubCod,
				   bool ICanFill);

void Rub_PushRubCod (struct Rub_Node **TOS,long RubCod);
void Rub_PopRubCod (struct Rub_Node **TOS);
bool Rub_FindRubCodInStack (const struct Rub_Node *TOS,long RubCod);

double RubCri_GetParScore (void);

void RubCri_GetCriterionDataFromRow (MYSQL_RES *mysql_res,
                                     struct RubCri_Criterion *Criterion);

void RubCri_ResetCriterion (struct RubCri_Criterion *Criterion);

void RubCri_ReqRemCriterion (void);
void RubCri_RemoveCriterion (void);

void RubCri_MoveUpCriterion (void);
void RubCri_MoveDownCriterion (void);

void RubCri_ChangeLink (void);

#endif
