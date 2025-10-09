// swad_record_database.h: users' record cards, operations with database

#ifndef _SWAD_REC_DB
#define _SWAD_REC_DB
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

#include "swad_record.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Rec_DB_CreateField (long HieCod,const struct RecordField *Field);
void Rec_DB_CreateFieldContent (long FldCod,long UsrCod,const char *Text);
void Rec_DB_UpdateFieldTxt (long FldCod,long UsrCod,const char *Text);
void Rec_DB_UpdateFieldName (long FldCod,const char NewFieldName[Rec_MAX_BYTES_NAME_FIELD + 1]);
void Rec_DB_UpdateFieldNumLines (long FldCod,unsigned NewNumLines);
void Rec_DB_UpdateFieldVisibility (long FldCod,Rec_VisibilityRecordFields_t NewVisibility);

unsigned Rec_DB_CountNumRecordsWithFieldContent (long FldCod);
unsigned Rec_DB_GetAllFieldsInCrs (MYSQL_RES **mysql_res,long HieCod);
Exi_Exist_t Rec_DB_GetFieldByCod (MYSQL_RES **mysql_res,long HieCod,long FldCod);
Exi_Exist_t Rec_DB_GetFieldTxtFromUsrRecord (MYSQL_RES **mysql_res,
                                             long FldCod,long UsrCod);

void Rec_DB_RemoveFieldContentFromAllUsrsRecords (long FldCod);
void Rec_DB_RemoveFieldContentFromUsrRecord (long FldCod,long UsrCod);
void Rec_DB_RemoveAllFieldContentsFromUsrRecordInCrs (long UsrCod,long HieCod);
void Rec_DB_RemoveAllFieldContentsFromUsrRecords (long UsrCod);
void Rec_DB_RemoveAllFieldContentsInCrs (long HieCod);
void Rec_DB_RemoveField (long FldCod);
void Rec_DB_RemoveAllFieldsInCrs (long HieCod);

#endif
