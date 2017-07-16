// swad_remote_control.h: remote control

#ifndef _SWAD_RMT
#define _SWAD_RMT
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Rmt_MAX_CHARS_SURVEY_TITLE	(128 - 1)	// 127
#define Rmt_MAX_BYTES_SURVEY_TITLE	((Rmt_MAX_CHARS_SURVEY_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Rmt_NUM_DATES 2
typedef enum
  {
   Rmt_START_TIME = 0,
   Rmt_END_TIME   = 1,
  } Rmt_StartOrEndTime_t;

struct Game
  {
   long GamCod;
   Sco_Scope_t Scope;
   long Cod;		// Country, institution, centre, degree or course code
   unsigned Roles;	// Example: if game can be made by students, Roles == (1 << Rol_STD)
   long UsrCod;
   char Title[Rmt_MAX_BYTES_SURVEY_TITLE + 1];
   time_t TimeUTC[Rmt_NUM_DATES];
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

#define Rmt_NUM_ORDERS 2
typedef enum
  {
   Rmt_ORDER_BY_START_DATE = 0,
   Rmt_ORDER_BY_END_DATE   = 1,
  } Rmt_Order_t;
#define Rmt_ORDER_DEFAULT Rmt_ORDER_BY_START_DATE

#define Rmt_NUM_ANS_TYPES	2
typedef enum
  {
   Rmt_ANS_UNIQUE_CHOICE   = 0,
   Rmt_ANS_MULTIPLE_CHOICE = 1,
  } Rmt_AnswerType_t;
#define Rmt_ANSWER_TYPE_DEFAULT Rmt_ANS_UNIQUE_CHOICE

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Rmt_SeeAllGames (void);
void Rmt_SeeOneGame (void);
void Rmt_PutHiddenParamGameOrder (void);
void Rmt_RequestCreatOrEditGame (void);
void Rmt_GetListGames (void);
void Rmt_GetDataOfGameByCod (struct Game *Gam);
void Rmt_GetDataOfGameByFolder (struct Game *Gam);
void Rmt_FreeListGames (void);
void Rmt_GetNotifGame (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                       char **ContentStr,
                       long GamCod,bool GetContent);
void Rmt_PutParamGameCod (long GamCod);
long Rmt_GetParamGameCod (void);
void Rmt_AskRemGame (void);
void Rmt_RemoveGame (void);
void Rmt_AskResetGame (void);
void Rmt_ResetGame (void);
void Rmt_HideGame (void);
void Rmt_UnhideGame (void);
void Rmt_RecFormGame (void);
bool Rmt_CheckIfGamIsAssociatedToGrp (long GamCod,long GrpCod);
void Rmt_RemoveGroup (long GrpCod);
void Rmt_RemoveGroupsOfType (long GrpTypCod);
void Rmt_RemoveGames (Sco_Scope_t Scope,long Cod);

void Rmt_RequestEditQuestion (void);
void Rmt_ReceiveQst (void);

void Rmt_AddTstQuestionsToGame (void);

void Rmt_RequestRemoveQst (void);
void Rmt_RemoveQst (void);

void Rmt_ReceiveGameAnswers (void);

unsigned Rmt_GetNumCoursesWithCrsGames (Sco_Scope_t Scope);
unsigned Rmt_GetNumCrsGames (Sco_Scope_t Scope,unsigned *NumNotif);
float Rmt_GetNumQstsPerCrsGame (Sco_Scope_t Scope);

#endif
