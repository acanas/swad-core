// swad_scope.h: scope (platform, center, degree, course...)

#ifndef _SWAD_SCO
#define _SWAD_SCO
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

#include <stdbool.h>		// For boolean type

#include "swad_hierarchy_type.h"
#include "swad_HTML.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Sco_PutSelectorScope (const char *ParName,HTM_Attributes_t Attributes);
void Sco_PutParCurrentScope (void *Level);
void Sco_PutParScope (const char *ParName,Hie_Level_t Level);
void Sco_GetScope (const char *ParName,Hie_Level_t DefaultScope);
void Sco_AdjustScope (Hie_Level_t DefaultScope);

void Sco_SetAllowedScopesForListingGuests (void);
void Sco_SetAllowedScopesForListingStudents (void);

Hie_Level_t Sco_GetScopeFromUnsignedStr (const char *UnsignedStr);
Hie_Level_t Hie_GetLevelFromDBStr (const char *LevelDBStr);
const char *Hie_GetDBStrFromLevel (Hie_Level_t Level);

long Hie_GetCurrentCod (void);

#endif
