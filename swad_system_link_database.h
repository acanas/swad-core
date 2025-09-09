// swad_system_link_database.h: system links, operations with database

#ifndef _SWAD_SYS_LNK_DB
#define _SWAD_SYS_LNK_DB
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_system_link.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void SysLnk_DB_CreateLink (const struct SysLnk_Link *Lnk);
void SysLnk_DB_UpdateLnkName (long LnkCod,const char *FldName,const char *NewLnkName);
void SysLnk_DB_UpdateLnkWWW (long LnkCod,const char NewWWW[WWW_MAX_BYTES_WWW + 1]);

unsigned SysLnk_DB_GetLinks (MYSQL_RES **mysql_res);
Exi_Exist_t SysLnk_DB_GetLinkDataByCod (MYSQL_RES **mysql_res,long LnkCod);
Exi_Exist_t SysLnk_DB_CheckIfLinkNameExists (const char *FldName,const char *Name,long Cod,
					     __attribute__((unused)) long PrtCod,
					     __attribute__((unused)) unsigned Year);

void SysLnk_DB_RemoveLink (long LnkCod);

#endif
