// swad_test_database.h: self-assessment tests, operations with database

#ifndef _SWAD_TST_DB
#define _SWAD_TST_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

// #include "swad_exam.h"
// #include "swad_game.h"
// #include "swad_media.h"
// #include "swad_question.h"
// #include "swad_question_type.h"
// #include "swad_test_config.h"
// #include "swad_test_print.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//----------------------- User's settings in course ---------------------------
void Tst_DB_IncreaseNumMyPrints (void);
void Tst_DB_UpdateLastAccTst (unsigned NumQsts);

unsigned Tst_DB_GetDateNextTstAllowed (MYSQL_RES **mysql_res);
unsigned Tst_DB_GetNumPrintsGeneratedByMe (MYSQL_RES **mysql_res);

//---------------------------- Test questions ---------------------------------
void Tst_DB_UpdateQstScore (const struct TstPrn_PrintedQuestion *PrintedQuestion);

unsigned Tst_DB_GetQuestionsForNewTest (MYSQL_RES **mysql_res,
                                        const struct Qst_Questions *Questions);

//--------------------------- Test configuration ------------------------------
void Tst_DB_SaveConfig (void);
unsigned Tst_DB_GetConfig (MYSQL_RES **mysql_res,long CrsCod);
unsigned Tst_DB_GetPluggableFromConfig (MYSQL_RES **mysql_res);

void Tst_DB_RemoveTstConfig (long CrsCod);

//------------------------------- Test prints ---------------------------------
long Tst_DB_CreatePrint (unsigned NumQsts);
void Tst_DB_UpdatePrint (const struct TstPrn_Print *Print);

#endif
