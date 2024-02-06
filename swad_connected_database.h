// swad_connected_database.h: connected users operations with database

#ifndef _SWAD_CON_DB
#define _SWAD_CON_DB
/*
    SWAD (Shared Workspace At a Distance),
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_role.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

unsigned Con_DB_GetConnectedUsrsTotal (Rol_Role_t Role);
unsigned Con_DB_GetNumConnectedFromCurrentLocation (MYSQL_RES **mysql_res,Rol_Role_t Role);
unsigned Con_DB_GetConnectedFromCurrentLocation (MYSQL_RES **mysql_res,Rol_Role_t Role);

void Con_DB_UpdateMeInConnectedList (void);

void Con_DB_RemoveOldConnected (void);
void Con_DB_RemoveUsrFromConnected (long UsrCod);

#endif
