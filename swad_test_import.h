// swad_test_import.h: import and export self-assessment tests using XML files

#ifndef _SWAD_TSI
#define _SWAD_TSI
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void TsI_PutFormToExportQuestions (const struct Tst_Tags *Tags,
                                   const struct Tst_AnswerTypes *AnswerTypes,
                                   Tst_QuestionsOrder_t SelectedOrder);
bool TsI_GetCreateXMLParamFromForm (void);
void TsI_PutFormToImportQuestions (void);
void TsI_CreateXML (unsigned long NumRows,MYSQL_RES *mysql_res);
void TsI_ShowFormImportQstsFromXML (void);
void TsI_ImportQstsFromXML (void);

#endif
