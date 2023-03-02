// swad_rubric.h: assessment rubrics

#ifndef _SWAD_RUB
#define _SWAD_RUB
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_string.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Rub_MAX_CHARS_TITLE	(128 - 1)	// 127
#define Rub_MAX_BYTES_TITLE	((Rub_MAX_CHARS_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Rub_AFTER_LAST_CRITERION	((unsigned)((1UL << 31) - 1))	// 2^31 - 1, don't change this number because it is used in database

struct Rub_Rubric
  {
   long RubCod;			// Rubric code
   long CrsCod;			// Course code
   long UsrCod;			// Author code
   char Title[Rub_MAX_BYTES_TITLE + 1];
   unsigned NumCriteria;		// Number of criteria in the rubric
  };

struct Rub_Rubrics
  {
   bool LstIsRead;		// Is the list already read from database...
				// ...or it needs to be read?
   unsigned Num;		// Total number of rubrics
   long *Lst;			// List of rubric codes
   unsigned CurrentPage;
   struct Rub_Rubric Rubric;	// Selected/current rubric
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Rub_ResetRubrics (struct Rub_Rubrics *Rubrics);
void Rub_ResetRubric (struct Rub_Rubric *Rubric);

void Rub_SeeAllRubrics (void);
void Rub_ListAllRubrics (struct Rub_Rubrics *Rubrics);
bool Rub_CheckIfICanEditRubrics (void);
void Rub_SeeOneRubric (void);
void Rub_ShowOnlyOneRubric (struct Rub_Rubrics *Rubrics,
			    bool ListRubricDims);
void Rub_ShowOnlyOneRubricBegin (struct Rub_Rubrics *Rubrics,
			         bool ListRubricDims);
void Rub_ShowOnlyOneRubricEnd (void);

void Rub_SetCurrentRubCod (long GamCod);
void Rub_PutParams (void *Rubrics);
void Rub_PutParamRubCod (long RubCod);
long Rub_GetParamRubCod (void);
long Rub_GetParams (struct Rub_Rubrics *Rubrics);

void Rub_GetListRubrics (struct Rub_Rubrics *Rubrics);
void Rub_GetDataOfRubricByCod (struct Rub_Rubric *Rubric);
void Rub_GetDataOfRubricByFolder (struct Rub_Rubric *Rubric);
void Rub_FreeListRubrics (struct Rub_Rubrics *Rubrics);

void Rub_AskRemRubric (void);
void Rub_RemoveRubric (void);
void Rub_RemoveCrsRubrics (long CrsCod);

void Rub_ListRubric (void);

void Rub_RequestCreatOrEditRubric (void);

void Rub_ReceiveFormRubric (void);

//-------------------------------- Figures ------------------------------------
void Rub_GetAndShowRubricsStats (void);

#endif
