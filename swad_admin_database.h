// swad_admin_database.h: administrators operations with database

#ifndef _SWAD_ADM_DB
#define _SWAD_ADM_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#include "swad_hierarchy.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Adm_DB_InsertAdmin (long UsrCod,HieLvl_Level_t Level);

unsigned Adm_DB_GetAdmsCurrentScopeExceptMe (MYSQL_RES **mysql_res);
bool Adm_DB_CheckIfUsrIsAdm (long UsrCod,HieLvl_Level_t Level);
bool Adm_DB_CheckIfUsrIsSuperuser (long UsrCod);

void Adm_DB_RemAdmin (long UsrCod,HieLvl_Level_t Level);
void Adm_DB_RemUsrAsAdmin (long UsrCod);
void Adm_DB_RemAdmins (HieLvl_Level_t Level,long Cod);

#endif
