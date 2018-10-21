// swad_database.h: database

#ifndef _SWAD_DB
#define _SWAD_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void DB_CreateTablesIfNotExist (void);
void DB_OpenDBConnection (void);
void DB_CloseDBConnection (void);

unsigned long DB_QuerySELECT_free (const char *Query,MYSQL_RES **mysql_res,const char *MsgError);
unsigned long DB_QuerySELECT (const char *Query,MYSQL_RES **mysql_res,const char *MsgError);

unsigned long DB_QueryCOUNT_free (const char *Query,const char *MsgError);
unsigned long DB_QueryCOUNT (const char *Query,const char *MsgError);

void DB_QueryINSERT_free (const char *Query,const char *MsgError);
void DB_QueryINSERT (const char *Query,const char *MsgError);

long DB_QueryINSERTandReturnCode_free (const char *Query,const char *MsgError);
long DB_QueryINSERTandReturnCode (const char *Query,const char *MsgError);

void DB_QueryREPLACE_free (const char *Query,const char *MsgError);
void DB_QueryREPLACE (const char *Query,const char *MsgError);

void DB_QueryUPDATE_free (const char *Query,const char *MsgError);
void DB_QueryUPDATE (const char *Query,const char *MsgError);

void DB_QueryDELETE_free (const char *Query,const char *MsgError);
void DB_QueryDELETE (const char *Query,const char *MsgError);

void DB_Query_free (const char *Query,const char *MsgError);
void DB_Query (const char *Query,const char *MsgError);
void DB_FreeMySQLResult (MYSQL_RES **mysql_res);
void DB_ExitOnMySQLError (const char *Message);

#endif
