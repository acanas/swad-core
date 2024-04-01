// swad_admin.h: administrators

#ifndef _SWAD_ADM
#define _SWAD_ADM
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_admin.h"
#include "swad_enrolment.h"
#include "swad_hierarchy.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

Usr_ICan_t Adm_CheckIfICanAdminOtherUsrs (void);

void Adm_ReqAddAdm (Hie_Level_t Level);

void Adm_AddAdmToIns (void);
void Adm_AddAdmToCtr (void);
void Adm_AddAdmToDeg (void);

void Adm_GetAdmsLst (Hie_Level_t Level);

void Adm_ReqRemAdmOfIns (void);
void Adm_ReqRemAdmOfCtr (void);
void Adm_ReqRemAdmOfDeg (void);

void Adm_RemAdmIns (void);
void Adm_RemAdmCtr (void);
void Adm_RemAdmDeg (void);

#endif
