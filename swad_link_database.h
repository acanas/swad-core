// swad_link_database.h: institutional links, operations with database

#ifndef _SWAD_LNK_DB
#define _SWAD_LNK_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Ca�as Vargas

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

#include "swad_link.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Lnk_DB_CreateLink (const struct Link *Lnk);
void Lnk_DB_UpdateLnkName (long LnkCod,const char *FieldName,const char *NewLnkName);
void Lnk_DB_UpdateLnkWWW (long LnkCod,const char NewWWW[Cns_MAX_BYTES_WWW + 1]);

unsigned Lnk_DB_GetLinks (MYSQL_RES **mysql_res);
unsigned Lnk_DB_GetDataOfLinkByCod (MYSQL_RES **mysql_res,long LnkCod);
bool Lnk_DB_CheckIfLinkNameExists (const char *FieldName,const char *Name,long LnkCod);

void Lnk_DB_RemoveLink (long LnkCod);

#endif
