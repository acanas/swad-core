// swad_rubric_type.h: definition of types for rubrics

#ifndef _SWAD_RUB_TYP
#define _SWAD_RUB_TYP
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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

#include <stdbool.h>		// For boolean type

#include "swad_date.h"
#include "swad_hierarchy_type.h"
#include "swad_program_resource.h"
#include "swad_string.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Rub_MAX_CHARS_TITLE	(128 - 1)	// 127
#define Rub_MAX_BYTES_TITLE	((Rub_MAX_CHARS_TITLE + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 2047

#define RubCri_MAX_CHARS_TITLE	(256 - 1)	// 255
#define RubCri_MAX_BYTES_TITLE	((RubCri_MAX_CHARS_TITLE + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 4095

#define RubCri_NUM_VALUES 2
typedef enum
  {
   RubCri_MIN = 0,
   RubCri_MAX = 1,
  } RubCri_ValueRange_t;

struct Rub_Rubric
  {
   long RubCod;			// Rubric code
   long HieCod;			// Course code
   long UsrCod;			// Author code
   char Title[Rub_MAX_BYTES_TITLE + 1];	// Title
   char *Txt;			// Text (dynamically allocated)
  };

struct RubCri_Criterion
  {
   long RubCod;		// Rubric code
   long CriCod;		// Criterion code
   unsigned CriInd;	// Criterion index (position in the rubric)
   struct Rsc_Link Link;
   double Values[RubCri_NUM_VALUES];
   double Weight;	// Relative weight (from 0.0 to 1.0)
   char Title[RubCri_MAX_BYTES_TITLE + 1];	// Title of the criterion
  };

struct Rub_Rubrics
  {
   bool LstIsRead;		// Is the list already read from database...
				// ...or it needs to be read?
   unsigned Num;		// Total number of rubrics
   long *Lst;			// List of rubric codes
   unsigned CurrentPage;
   struct Rub_Rubric Rubric;		// Selected/current rubric
   struct RubCri_Criterion Criterion;	// Selected/current criterion
  };

/* Stack node to check is a rubric is recursive */
struct Rub_Node
  {
   long RubCod;
   struct Rub_Node *Prev;
  };

#endif
