// swad_game.h: games using remote control

#ifndef _SWAD_GAM
#define _SWAD_GAM
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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

#include "swad_scope.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Gam_MAX_CHARS_TITLE	(128 - 1)	// 127
#define Gam_MAX_BYTES_TITLE	((Gam_MAX_CHARS_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Gam_NUM_DATES 2
typedef enum
  {
   Gam_START_TIME = 0,
   Gam_END_TIME   = 1,
  } Gam_StartOrEndTime_t;

struct Game
  {
   long GamCod;		// Game code
   long UsrCod;		// Author code
   char Title[Gam_MAX_BYTES_TITLE + 1];
   time_t TimeUTC[Gam_NUM_DATES];
   unsigned NumQsts;	// Number of questions in the game
   struct
     {
      bool Visible;		// Game is not hidden
      bool ICanViewResults;
      bool ICanEdit;
     } Status;
  };

#define Gam_NUM_ORDERS 3
typedef enum
  {
   Gam_ORDER_BY_START_DATE = 0,
   Gam_ORDER_BY_END_DATE   = 1,
   Gam_ORDER_BY_TITLE      = 2,
  } Gam_Order_t;
#define Gam_ORDER_DEFAULT Gam_ORDER_BY_START_DATE

#define Gam_NUM_ANS_TYPES	2
typedef enum
  {
   Gam_ANS_UNIQUE_CHOICE   = 0,
   Gam_ANS_MULTIPLE_CHOICE = 1,
  } Gam_AnswerType_t;
#define Gam_ANSWER_TYPE_DEFAULT Gam_ANS_UNIQUE_CHOICE

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Gam_SeeAllGames (void);
void Gam_SeeOneGame (void);
void Gam_PutHiddenParamGameOrder (void);
void Gam_RequestCreatOrEditGame (void);
void Gam_GetListGames (void);
void Gam_GetDataOfGameByCod (struct Game *Gam);
void Gam_GetDataOfGameByFolder (struct Game *Gam);
void Gam_FreeListGames (void);

void Gam_PutParamGameCod (long GamCod);
long Gam_GetParamGameCod (void);
void Gam_AskRemGame (void);
void Gam_RemoveGame (void);
void Gam_AskResetGame (void);
void Gam_ResetGame (void);
void Gam_HideGame (void);
void Gam_UnhideGame (void);
void Gam_RecFormGame (void);
bool Gam_CheckIfMatchIsAssociatedToGrp (long MchCod,long GrpCod);
void Gam_RemoveGroup (long GrpCod);
void Gam_RemoveGroupsOfType (long GrpTypCod);
void Gam_RemoveGames (Hie_Level_t Scope,long Cod);

void Gam_RequestNewQuestion (void);

void Gam_GetAndDrawBarNumUsrsWhoAnswered (long GamCod,unsigned QstInd,unsigned AnsInd,unsigned NumUsrs);

void Gam_AddTstQuestionsToGame (void);

void Gam_RequestRemoveQst (void);
void Gam_RemoveQst (void);

void Gam_MoveUpQst (void);
void Gam_MoveDownQst (void);

void Gam_RequestRemoveMatchTch (void);
void Gam_RemoveMatchTch (void);

void Gam_RequestNewMatchTch (void);

void Gam_CreateNewMatchTch (void);
void Gam_RequestStartResumeMatchTch (void);
void Gam_PauseMatchTch (void);
void Gam_ResumeMatchTch (void);
void Gam_ShowStemQstMatchTch (void);
void Gam_ShowAnssQstMatchTch (void);
void Gam_ShowRessQstMatchTch (void);
void Gam_PrevQstMatchTch (void);
void Gam_NextQstMatchTch (void);
void Gam_CurrQstMatchTch (void);

void Gam_ShowFinishedMatchResults (void);

void Gam_GetMatchBeingPlayed (void);
void Gam_ShowMatchToMeAsStd (void);
void Gam_RefreshMatchTch (void);
void Gam_RefreshMatchStd (void);

void Gam_ReceiveQstAnsFromStd (void);

unsigned Gam_GetNumCoursesWithGames (Hie_Level_t Scope);
unsigned Gam_GetNumGames (Hie_Level_t Scope);
float Gam_GetNumQstsPerCrsGame (Hie_Level_t Scope);

#endif
