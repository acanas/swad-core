// swad_match_database.h: matches in games using remote control, operations woth database

#ifndef _SWAD_MCH_DB
#define _SWAD_MCH_DB
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

// #include "swad_game.h"
#include "swad_match.h"
// #include "swad_match_print.h"
// #include "swad_scope.h"
// #include "swad_test.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//------------------------------- Matches -------------------------------------
long Mch_DB_CreateMatch (long GamCod,char Title[Mch_MAX_BYTES_TITLE + 1]);
void Mch_DB_UpdateMatchStatus (const struct Mch_Match *Match);
void Mch_DB_UpdateMatchTitle (const struct Mch_Match *Match);

Mch_Showing_t Mch_DB_GetShowingFromStr (const char *Str);
unsigned Mch_DB_GetNumMchsInGame (long GamCod);
unsigned Mch_DB_GetNumUnfinishedMchsInGame (long GamCod);

void Mch_DB_RemoveMatchFromTable (long MchCod,const char *TableName);
void Mch_DB_RemoveMatchesInGameFromMainTable (long GamCod);
void Mch_DB_RemoveMatchesInGameFromOtherTable (long GamCod,const char *TableName);
void Mch_DB_RemoveMatchesInCrsFromMainTable (long CrsCod);
void Mch_DB_RemoveMatchesInCrsFromOtherTable (long CrsCod,const char *TableName);
void Mch_DB_RemoveMatchesMadeByUsrFromTable (long UsrCod,const char *TableName);
void Mch_DB_RemoveMatchesMadeByUsrInCrsFromTable (long UsrCod,long CrsCod,
                                                  const char *TableName);

//---------------------------------Groups -------------------------------------
void Mch_DB_AssociateGroupToMatch (long MchCod,long GrpCod);

void Mch_DB_RemoveGroup (long GrpCod);
void Mch_DB_RemoveGroupsOfType (long GrpTypCod);

//-------------------------------- Answers ------------------------------------
void Mch_DB_RemAnswersOfAQuestion (long GamCod,unsigned QstInd);

//----------------------------- Answers indexes -------------------------------
void Mch_DB_CreateQstIndexes (long MchCod,unsigned QstInd,
                              const char StrAnswersOneQst[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1]);
void Mch_DB_GetIndexes (long MchCod,unsigned QstInd,
		        char StrIndexesOneQst[Tst_MAX_BYTES_INDEXES_ONE_QST + 1]);

//----------------------------- Elapsed times ---------------------------------
void Mch_DB_UpdateElapsedTimeInQuestion (long MchCod,long QstInd);

#endif
