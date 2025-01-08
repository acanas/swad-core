// swad_call_for_exam_database.h: calls for exams operations with database

#ifndef _SWAD_CFE_DB
#define _SWAD_CFE_DB
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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

#include "swad_call_for_exam.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

long Cfe_DB_CreateCallForExam (const struct Cfe_CallForExam *CallForExam);

unsigned Cfe_DB_GetCallsForExamsInCurrentCrs (MYSQL_RES **mysql_res);
unsigned Cfe_DB_GetVisibleCallsForExamsInCurrentCrs (MYSQL_RES **mysql_res);
unsigned Cfe_DB_GetFutureCallsForExamsInCurrentCrs (MYSQL_RES **mysql_res);
unsigned Cfe_DB_GetCallForExamDataByCod (MYSQL_RES **mysql_res,long ExaCod);

void Cfe_DB_ModifyCallForExam (const struct Cfe_CallForExam *CallForExam,
                               long ExaCod);
void Cfe_DB_UpdateNumUsrsNotifiedByEMailAboutCallForExam (long ExaCod,
                                                          unsigned NumUsrsToBeNotifiedByEMail);

void Cfe_DB_HideCallForExam (long ExaCod);
void Cfe_DB_UnhideCallForExam (long ExaCod);

void Cfe_DB_MarkACallForExamAsDeleted (long ExaCod);
void Cfe_DB_MarkCallForExamsInCrsAsDeleted (long CrsCod);

#endif
