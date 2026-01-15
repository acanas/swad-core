// swad_game_database.h: games using remote control, operations with database

#ifndef _SWAD_GAM_DB
#define _SWAD_GAM_DB
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_game.h"
#include "swad_hierarchy_type.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//--------------------------------- Games -------------------------------------
long Gam_DB_CreateGame (const struct Gam_Game *Game,const char *Txt);
void Gam_DB_UpdateGame (const struct Gam_Game *Game,const char *Txt);
void Gam_DB_HideOrUnhideGame (long GamCod,
			      HidVis_HiddenOrVisible_t HiddenOrVisible);

unsigned Gam_DB_GetListGames (MYSQL_RES **mysql_res,Gam_Order_t SelectedOrder);
unsigned Gam_DB_GetListAvailableGames (MYSQL_RES **mysql_res);
Exi_Exist_t Gam_DB_GetGameDataByCod (MYSQL_RES **mysql_res,long GamCod);
void Gam_DB_GetGameTitle (long GamCod,char *Title,size_t TitleSize);
void Gam_DB_GetGameTxt (long GamCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);
Exi_Exist_t Gam_DB_CheckIfSimilarGameExists (const struct Gam_Game *Game);
unsigned Gam_DB_GetNumCoursesWithGames (Hie_Level_t HieLvl);
unsigned Gam_DB_GetNumGames (Hie_Level_t HieLvl);

void Gam_DB_RemoveGame (long GamCod);
void Gam_DB_RemoveCrsGames (long HieCod);

//---------------------------- Game questions ---------------------------------
void Gam_DB_InsertQstInGame (long GamCod,unsigned QstInd,long QstCod);
void Gam_DB_UpdateIndexesOfQstsGreaterThan (long GamCod,unsigned QstInd);
void Gam_DB_UpdateQstIndex (long QstInd,long GamCod,long QstCod);
void Gam_DB_LockTable (void);

unsigned Gam_DB_GetNumQstsGame (long GamCod);
unsigned Gam_DB_GetGameQuestionsBasic (MYSQL_RES **mysql_res,long GamCod);
unsigned Gam_DB_GetGameQuestionsFull (MYSQL_RES **mysql_res,long GamCod);
long Gam_DB_GetQstCodFromQstInd (long GamCod,unsigned QstInd);
unsigned Gam_DB_GetQstIndFromQstCod (long GamCod,long QstCod);
unsigned Gam_DB_GetMaxQuestionIndexInGame (long GamCod);
unsigned Gam_DB_GetPrevQuestionIndexInGame (long GamCod,unsigned QstInd);
unsigned Gam_DB_GetNextQuestionIndexInGame (long GamCod,unsigned QstInd);
double Gam_DB_GetNumQstsPerGame (Hie_Level_t HieLvl);
unsigned Gam_DB_GetTstTagsPresentInAGame (MYSQL_RES **mysql_res,long GamCod);
unsigned Gam_DB_GetNumAnswersOfQstsInGame (MYSQL_RES **mysql_res,long GamCod);

void Gam_DB_RemoveQstFromGame (long GamCod,unsigned QstInd);
void Gam_DB_RemoveGameQsts (long GamCod);
void Gam_DB_RemoveCrsGameQsts (long HieCod);

#endif
