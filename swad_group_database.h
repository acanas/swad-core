// swad_group_database.h: types of groups and groups operations with database

#ifndef _SWAD_GRP_DB
#define _SWAD_GRP_DB
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

// #include "swad_info.h"
// #include "swad_room.h"
// #include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Grp_DB_LockTables (void);
void Grp_DB_UnlockTables (void);

bool Grp_DB_CheckIfAssociatedToGrp (const char *Table,const char *Field,
                                    long Cod,long GrpCod);
bool Grp_DB_CheckIfAssociatedToGrps (const char *Table,const char *Field,long Cod);

unsigned Grp_DB_CountNumGrpsInCurrentCrs (void);
unsigned Grp_DB_CountNumGrpsInThisCrsOfType (long GrpTypCod);
unsigned Grp_DB_GetGrpsOfType (MYSQL_RES **mysql_res,long GrpTypCod);

#endif
