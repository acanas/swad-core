// swad_rubric_criteria.h: criteria in assessment rubrics

#ifndef _SWAD_RUB_CRI
#define _SWAD_RUB_CRI
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2022 Antonio Cañas Vargas

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
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Rub_MAX_CHARS_CRITERION_TITLE	(128 - 1)	// 127
#define Rub_MAX_BYTES_CRITERION_TITLE	((Rub_MAX_CHARS_CRITERION_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Rub_AFTER_LAST_CRITERION	((unsigned)((1UL << 31) - 1))	// 2^31 - 1, don't change this number because it is used in database

struct Rub_Criterion
  {
   long RubCod;			// Rubric code
   long CriCod;			// Course code
   char Title[Rub_MAX_BYTES_CRITERION_TITLE + 1];
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void RubCri_CriterionConstructor (struct Rub_Criterion *Criterion);
void RubCri_CriterionDestructor (struct Rub_Criterion *Criterion);

void RubCri_PutParCriCod (void *CriCod);

bool RubCri_GetCriterionDataFromDB (struct Rub_Criterion *Criterion);

void RubCri_ListCriterionForEdition (struct Rub_Criterion *Criterion,
                                     unsigned CriInd,bool CriterionExists,
                                     const char *Anchor);
void RubCri_WriteNumCriterion (unsigned NumDim,const char *Class);
void RubCri_WriteCriterionTitle (const char *Title,const char *ClassTitle,bool Visible);

void RubCri_PutIconToAddNewCriterion (void *Rubrics);
void RubCri_PutButtonToAddNewCriterion (struct Rub_Rubrics *Rubrics);

void RubCri_PutParCriInd (unsigned CriInd);
unsigned RubCri_GetParCriInd (void);

void RubCri_ListRubricCriteria (struct Rub_Rubrics *Rubrics);

void RubCri_RequestCreatOrEditCri (void);

#endif
