// swad_role_database.h: user's roles, operations with database

#ifndef _SWAD_ROL_DB
#define _SWAD_ROL_DB
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

#include "swad_role_type.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Rol_DB_UpdateUsrRoleInCrs (long CrsCod,long UsrCod,Rol_Role_t NewRole);

Rol_Role_t Rol_DB_GetRoleUsrInCrs (long UsrCod,long CrsCod);
Rol_Role_t Rol_DB_GetMaxRoleUsrInCrss (long UsrCod);
unsigned Rol_DB_GetRolesInAllCrss (MYSQL_RES **mysql_res,long UsrCod);
Rol_Role_t Rol_DB_GetRequestedRole (long CrsCod,long UsrCod);

#endif
