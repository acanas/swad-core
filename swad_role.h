// swad_role.h: user's roles

#ifndef _SWAD_ROL
#define _SWAD_ROL
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

#include "swad_HTML.h"
#include "swad_user.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Rol_SetMyRoles (void);

unsigned Rol_GetNumAvailableRoles (void);
Rol_Role_t Rol_GetMaxRoleInCrss (unsigned Roles);
Rol_Role_t Rol_GetMyMaxRoleIn (Hie_Level_t Level,long HieCod);

void Rol_FlushCacheMyRoleInCurrentCrs (void);
Rol_Role_t Rol_GetMyRoleInCrs (long CrsCod);
void Rol_FlushCacheRoleUsrInCrs (void);
Rol_Role_t Rol_GetRoleUsrInCrs (long UsrCod,long CrsCod);
void Rol_GetRolesInAllCrss (struct Usr_Data *UsrDat);

Rol_Role_t Rol_ConvertUnsignedStrToRole (const char *UnsignedStr);
unsigned Rol_ConvertUnsignedStrToRoles (const char *UnsignedStr);

void Rol_PutFormToChangeMyRole (const char *ClassSelect);
void Rol_ChangeMyRole (void);

void Rol_WriteSelectorRoles (unsigned RolesAllowed,unsigned RolesSelected,
                             HTM_Attributes_t Attributes);
void Rol_PutParRoles (unsigned Roles);
unsigned Rol_GetSelectedRoles (void);

#endif
