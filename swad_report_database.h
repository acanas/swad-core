// swad_report_database.h: report on my use of the platform, operations with database

#ifndef _SWAD_REP_DB
#define _SWAD_REP_DB
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_cryptography.h"
#include "swad_report.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Rep_DB_CreateNewReport (long UsrCod,const struct Rep_Report *Report,
                             const char UniqueNameEncrypted[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1]);

unsigned Rep_DB_GetUsrReportsFiles (MYSQL_RES **mysql_res,long UsrCod);

void Rep_DB_RemoveUsrReports (long UsrCod);

#endif
