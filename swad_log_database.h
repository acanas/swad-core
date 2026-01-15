// swad_log_database.h: access log, operations with database

#ifndef _SWAD_LOG_DB
#define _SWAD_LOG_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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
/***************************** Public prototypes *****************************/
/*****************************************************************************/

long Log_DB_LogAccessInHistoricalLog (long ActCod,Rol_Role_t RoleToStore);
void Log_DB_LogAccessInRecentLog (long LogCod,long ActCod,Rol_Role_t RoleToStore);
void Log_DB_LogComments (long LogCod,const char *CommentsDB);
void Log_DB_LogSearchString (long LogCod,const struct Sch_Search *Search);
void Log_DB_LogAPI (long LogCod);
void Log_DB_LogBanner (long LogCod,long BanCodClicked);

unsigned Log_DB_GetLastClicks (MYSQL_RES **mysql_res);
Exi_Exist_t Log_DB_GetUsrFirstClick (MYSQL_RES **mysql_res,long UsrCod);
unsigned Log_DB_GetUsrNumClicks (long UsrCod);
unsigned Log_DB_GetMyClicksGroupedByAction (MYSQL_RES **mysql_res,
                                            time_t FirstClickTimeUTC,
                                            unsigned MaxActions);
Exi_Exist_t Log_DB_GetMyMaxHitsPerYear (MYSQL_RES **mysql_res,
                                        time_t FirstClickTimeUTC);
unsigned Log_DB_GetMyCrssAndHitsPerCrs (MYSQL_RES **mysql_res,Rol_Role_t Role);
unsigned Log_DB_GetMyHistoricCrss (MYSQL_RES **mysql_res,
                                   Rol_Role_t Role,unsigned MinClicksCrs);
unsigned Log_DB_GetMyHitsPerYear (MYSQL_RES **mysql_res,
                                  long HieCod,Rol_Role_t Role,
                                  time_t FirstClickTimeUTC);
void Log_DB_GetLogComments (long LogCod,char Comments[Cns_MAX_BYTES_TEXT + 1]);

void Log_DB_RemoveOldEntriesRecentLog (void);

#endif
