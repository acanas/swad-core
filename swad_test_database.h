// swad_test_database.h: self-assessment tests, operations with database

#ifndef _SWAD_TST_DB
#define _SWAD_TST_DB
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

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//----------------------- User's settings in course ---------------------------
void Tst_DB_IncreaseNumMyPrints (void);
void Tst_DB_UpdateLastAccTst (unsigned NumQsts);

Exi_Exist_t Tst_DB_GetDateNextTstAllowed (MYSQL_RES **mysql_res);
unsigned Tst_DB_GetNumPrintsGeneratedByMe (MYSQL_RES **mysql_res);

//--------------------------- Test configuration ------------------------------
void Tst_DB_SaveConfig (void);
Exi_Exist_t Tst_DB_GetConfig (MYSQL_RES **mysql_res,long HieCod);
Exi_Exist_t Tst_DB_GetPluggableFromConfig (MYSQL_RES **mysql_res);

void Tst_DB_RemoveTstConfig (long HieCod);

//------------------------------- Test prints ---------------------------------
long Tst_DB_CreatePrint (unsigned NumQsts);
void Tst_DB_UpdatePrint (const struct TstPrn_Print *Print);

unsigned Tst_DB_GetPrintsBetweenDates (MYSQL_RES **mysql_res,
                                       const char *From,
                                       const char *To);
unsigned Tst_DB_GetUsrPrintsInCurrentCrs (MYSQL_RES **mysql_res,long UsrCod);
Exi_Exist_t Tst_DB_GetPrintDataByPrnCod (MYSQL_RES **mysql_res,long PrnCod);

void Tst_DB_RemovePrintsMadeByUsrInAllCrss (long UsrCod);
void Tst_DB_RemovePrintsMadeByUsrInCrs (long UsrCod,long HieCod);
void Tst_DB_RemovePrintsMadeByInCrs (long HieCod);

//-------------------------- Test print questions -----------------------------
void Tst_DB_StoreOneQstOfPrint (const struct TstPrn_Print *Print,unsigned QstInd);

unsigned Tst_DB_GetTagsPresentInAPrint (MYSQL_RES **mysql_res,long PrnCod);
unsigned Tst_DB_GetPrintQuestions (MYSQL_RES **mysql_res,long PrnCod);

void Tst_DB_RemovePrintQuestionsMadeByUsrInAllCrss (long UsrCod);
void Tst_DB_RemovePrintQuestionsMadeByUsrInCrs (long UsrCod,long HieCod);
void Tst_DB_RemovePrintQuestionsMadeInCrs (long HieCod);

#endif
