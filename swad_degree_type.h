// swad_degree_type.h: degree types

#ifndef _SWAD_DT
#define _SWAD_DT
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Deg_MAX_LENGTH_DEGREE_TYPE_NAME	32

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct DegreeType
  {
   long DegTypCod;						// Degree type code
   char DegTypName[Deg_MAX_LENGTH_DEGREE_TYPE_NAME + 1];	// Degree type name
   unsigned NumDegs;						// Number of degrees of this type
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void DT_WriteSelectorDegreeTypes (void);
void DT_SeeDegreeTypes (void);
void DT_ReqEditDegreeTypes (void);

void DT_GetListDegreeTypes (void);
void DT_FreeListDegreeTypes (void);

void DT_RecFormNewDegreeType (void);

void DT_RemoveDegreeType (void);

long DT_GetParamOtherDegTypCod (void);

bool DT_GetDataOfDegreeTypeByCod (struct DegreeType *DegTyp);
void DT_RenameDegreeType (void);
void DT_ChangeDegreeType (void);

#endif
