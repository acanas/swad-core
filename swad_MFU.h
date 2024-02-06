// swad_MFU.h: Most Frequently Used actions

#ifndef _SWAD_MFU
#define _SWAD_MFU
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
/****************************** Public constants *****************************/
/*****************************************************************************/

#define MFU_MIN_SCORE		  0.5
#define MFU_MAX_SCORE		100.0
#define MFU_INCREASE_FACTOR	  1.2
#define MFU_DECREASE_FACTOR	  0.99

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

struct MFU_ListMFUActions
  {
   unsigned NumActions;
   Act_Action_t *Actions;
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void MFU_AllocateMFUActions (struct MFU_ListMFUActions *ListMFUActions,unsigned MaxActionsShown);
void MFU_FreeMFUActions (struct MFU_ListMFUActions *ListMFUActions);
void MFU_GetMFUActions (struct MFU_ListMFUActions *ListMFUActions,unsigned MaxActionsShown);
Act_Action_t MFU_GetMyLastActionInCurrentTab (void);
void MFU_ShowMyMFUActions (void);
void MFU_WriteBigMFUActions (struct MFU_ListMFUActions *ListMFUActions);
void MFU_WriteSmallMFUActions (struct MFU_ListMFUActions *ListMFUActions);
void MFU_UpdateMFUActions (void);

#endif
