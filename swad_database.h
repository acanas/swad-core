// swad_database.h: database

#ifndef _SWAD_DB
#define _SWAD_DB
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_role.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void DB_CreateTablesIfNotExist (void);
void DB_OpenDBConnection (void);
bool DB_CheckIfDatabaseIsOpen (void);
void DB_CloseDBConnection (void);

void DB_BuildQuery (char **Query,const char *fmt,...);

unsigned long DB_QuerySELECT (MYSQL_RES **mysql_res,const char *MsgError,
                              const char *fmt,...);
long DB_QuerySELECTCode (const char *MsgError,
                         const char *fmt,...);
unsigned DB_QuerySELECTUnsigned (const char *MsgError,
                                 const char *fmt,...);
double DB_QuerySELECTDouble (const char *MsgError,
                             const char *fmt,...);
Rol_Role_t DB_QuerySELECTRole (const char *MsgError,
                               const char *fmt,...);
void DB_QuerySELECTString (char *Str,size_t StrSize,const char *MsgError,
                           const char *fmt,...);

long DB_GetNextCode (MYSQL_RES *mysql_res);
unsigned long DB_GetNumRowsTable (const char *Table);
unsigned long DB_QueryCOUNT (const char *MsgError,const char *fmt,...);
bool DB_QueryEXISTS (const char *MsgError,const char *fmt,...);

void DB_QueryINSERT (const char *MsgError,const char *fmt,...);

long DB_QueryINSERTandReturnCode (const char *MsgError,const char *fmt,...);

void DB_QueryREPLACE (const char *MsgError,const char *fmt,...);

void DB_QueryUPDATE (const char *MsgError,const char *fmt,...);

void DB_QueryDELETE (const char *MsgError,const char *fmt,...);

void DB_CreateTmpTable (const char *fmt,...);
void DB_DropTmpTable (const char *Table);

void DB_Query (const char *MsgError,const char *fmt,...);

void DB_FreeMySQLResult (MYSQL_RES **mysql_res);
void DB_ExitOnMySQLError (const char *Message);

void DB_SetThereAreLockedTables (void);
void DB_UnlockTables (void);

#endif
