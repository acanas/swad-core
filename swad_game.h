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

#define Gam_MAX_CHARS_SURVEY_TITLE	(128 - 1)	// 127
#define Gam_MAX_BYTES_SURVEY_TITLE	((Gam_MAX_CHARS_SURVEY_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Gam_NUM_DATES 2
typedef enum
  {
   Gam_START_TIME = 0,
   Gam_END_TIME   = 1,
  } Gam_StartOrEndTime_t;

struct Game
  {
   long GamCod;
   Sco_Scope_t Scope;
   long Cod;		// Country, institution, centre, degree or course code
   unsigned Roles;	// Example: if game can be made by students, Roles == (1 << Rol_STD)
   long UsrCod;
   char Title[Gam_MAX_BYTES_SURVEY_TITLE + 1];
   time_t TimeUTC[Gam_NUM_DATES];
   unsigned NumQsts;	// Number of questions in the game
   unsigned NumUsrs;	// Number of distinct users who have already answered the game
   struct
     {
      bool Visible;		// Game is not hidden
      bool Open;		// Start date <= now <= end date
      bool IAmLoggedWithAValidRoleToAnswer;	// I am logged with a valid role to answer this game
      bool IBelongToScope;	// I belong to the scope of this game
      bool IHaveAnswered;	// I have already answered this game
      bool ICanAnswer;
      bool ICanViewResults;
      bool ICanEdit;
     } Status;
  };

#define Gam_NUM_ORDERS 2
typedef enum
  {
   Gam_ORDER_BY_START_DATE = 0,
   Gam_ORDER_BY_END_DATE   = 1,
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
bool Gam_CheckIfGamIsAssociatedToGrp (long GamCod,long GrpCod);
void Gam_RemoveGroup (long GrpCod);
void Gam_RemoveGroupsOfType (long GrpTypCod);
void Gam_RemoveGames (Sco_Scope_t Scope,long Cod);

void Gam_RequestNewQuestion (void);

void Gam_GetAndDrawBarNumUsrsWhoAnswered (struct Game *Game,long QstCod,unsigned AnsInd);

void Gam_AddTstQuestionsToGame (void);

void Gam_RequestRemoveQst (void);
void Gam_RemoveQst (void);

void Gam_MoveUpQst (void);
void Gam_MoveDownQst (void);

void Gam_PlayGame (void);

void Gam_ReceiveGameAnswers (void);
void Gam_PlayGameNextQuestion (void);
void Gam_PlayGameShowAnswers (void);

unsigned Gam_GetNumCoursesWithGames (Sco_Scope_t Scope);
unsigned Gam_GetNumGames (Sco_Scope_t Scope);
float Gam_GetNumQstsPerCrsGame (Sco_Scope_t Scope);

#endif
