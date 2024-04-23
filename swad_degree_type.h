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

struct DegTyp_DegType
  {
   long DegTypCod;						// Degree type code
   char DegTypName[DegTyp_MAX_BYTES_DEGREE_TYPE_NAME + 1];	// Degree type name
   unsigned NumDegs;						// Number of degrees of this type
  };

struct DegTyp_DegTypes
  {
   unsigned Num;		// Number of degree types
   struct DegTyp_DegType *Lst;	// List of degree types
  };

#define DegTyp_NUM_ORDERS 2
typedef enum
  {
   DegTyp_ORDER_BY_DEG_TYPE = 0,
   DegTyp_ORDER_BY_NUM_DEGS = 1,
  } DegTyp_Order_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void DegTyp_WriteSelectorDegTypes (long SelectedDegTypCod);

void DegTyp_SeeDegTypesInDegTab (void);
void DegTyp_SeeDegTypesInStaTab (void);
void DegTyp_GetAndEditDegTypes (void);
void DegTyp_EditDegTypes (const struct DegTyp_DegTypes *DegTypes);

void DegTyp_PutIconToViewDegTypes (void);

Usr_Can_t DegTyp_CheckIfICanCreateDegTypes (void);

void DegTyp_GetListDegTypes (struct DegTyp_DegTypes *DegTypes,
                             Hie_Level_t Level,DegTyp_Order_t Order);
void DegTyp_FreeListDegTypes (struct DegTyp_DegTypes *DegTypes);

void DegTyp_ReceiveNewDegTyp (void);

void DegTyp_RemoveDegTyp (void);

bool DegTyp_GetDegTypeDataByCod (struct DegTyp_DegType *DegTyp);
void DegTyp_RenameDegTyp (void);

void DegTyp_ContEditAfterChgDegTyp (void);

//-------------------------------- Figures ------------------------------------
void DegTyp_GetAndShowDegTypesStats (void);

#endif
