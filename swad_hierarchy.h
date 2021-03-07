// swad_hierarchy.h: hierarchy (system, institution, center, degree, course)

#ifndef _SWAD_HIE
#define _SWAD_HIE
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include "swad_center.h"
#include "swad_country.h"
#include "swad_course.h"
#include "swad_degree.h"
#include "swad_institution.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct Hie_Hierarchy
  {
   struct Cty_Countr Cty;
   struct Ins_Instit Ins;
   struct Ctr_Center Ctr;
   struct Deg_Degree Deg;
   struct Crs_Course Crs;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Hie_SeePending (void);

void Hie_WriteMenuHierarchy (void);
void Hie_WriteHierarchyInBreadcrumb (void);
void Hie_WriteBigNameCtyInsCtrDegCrs (void);

void Hie_SetHierarchyFromUsrLastHierarchy (void);
void Hie_InitHierarchy (void);
void Hie_ResetHierarchy (void);

void Hie_GetAndWriteInsCtrDegAdminBy (long UsrCod,unsigned ColSpan);

char *Hie_BuildGoToMsg (const char *Where);
void Hie_FreeGoToMsg (void);

#endif
