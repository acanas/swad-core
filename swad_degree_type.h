// swad_degree_type.h: degree types

#ifndef _SWAD_DEG_TYP
#define _SWAD_DEG_TYP
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_scope.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define DegTyp_MAX_CHARS_DEGREE_TYPE_NAME	(32 - 1)	// 31
#define DegTyp_MAX_BYTES_DEGREE_TYPE_NAME	((DegTyp_MAX_CHARS_DEGREE_TYPE_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 511

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct DegTyp_DegreeType
  {
   long DegTypCod;						// Degree type code
   char DegTypName[DegTyp_MAX_BYTES_DEGREE_TYPE_NAME + 1];	// Degree type name
   unsigned NumDegs;						// Number of degrees of this type
  };

struct DegTyp_DegTypes
  {
   unsigned Num;			// Number of degree types
   struct DegTyp_DegreeType *Lst;	// List of degree types
  };

#define DegTyp_NUM_ORDERS 2
typedef enum
  {
   DegTyp_ORDER_BY_DEGREE_TYPE = 0,
   DegTyp_ORDER_BY_NUM_DEGREES = 1,
  } DegTyp_Order_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void DegTyp_WriteSelectorDegreeTypes (long SelectedDegTypCod);

void DegTyp_SeeDegreeTypesInDegTab (void);
void DegTyp_SeeDegreeTypesInStaTab (void);
void DegTyp_GetAndEditDegreeTypes (void);
void DegTyp_EditDegreeTypes (const struct DegTyp_DegTypes *DegTypes);

void DegTyp_PutIconToViewDegreeTypes (void);

Usr_ICan_t DegTyp_CheckIfICanCreateDegreeTypes (void);

void DegTyp_GetListDegreeTypes (struct DegTyp_DegTypes *DegTypes,
                                Hie_Level_t Level,DegTyp_Order_t Order);
void DegTyp_FreeListDegreeTypes (struct DegTyp_DegTypes *DegTypes);

void DegTyp_ReceiveNewDegreeType (void);

void DegTyp_RemoveDegreeType (void);

bool DegTyp_GetDegreeTypeDataByCod (struct DegTyp_DegreeType *DegTyp);
void DegTyp_RenameDegreeType (void);

void DegTyp_ContEditAfterChgDegTyp (void);

//-------------------------------- Figures ------------------------------------
void DegTyp_GetAndShowDegreeTypesStats (void);

#endif
