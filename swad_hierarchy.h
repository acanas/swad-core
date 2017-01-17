// swad_hierarchy.h: hierarchy (system, institution, centre, degree, course)

#ifndef _SWAD_HIE
#define _SWAD_HIE
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

#define Hie_MAX_LENGTH_LOCATION_SHORT_NAME	 32
#define Hie_MAX_LENGTH_LOCATION_SHORT_NAME_SPEC_CHAR (Hie_MAX_LENGTH_LOCATION_SHORT_NAME * Str_MAX_LENGTH_SPEC_CHAR_HTML)

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Hie_WriteMenuHierarchy (void);
void Hie_WriteHierarchyBreadcrumb (void);
void Hie_WriteBigNameCtyInsCtrDegCrs (void);
void Hie_InitHierarchy (void);

void Hie_GetAndWriteInsCtrDegAdminBy (long UsrCod,unsigned ColSpan);

#endif
