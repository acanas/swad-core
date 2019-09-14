// swad_match.h: matches in games using remote control

#ifndef _SWAD_MCH
#define _SWAD_MCH
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

#define Mch_NEW_MATCH_SECTION_ID	"new_match"

#define Mch_AFTER_LAST_QUESTION	((unsigned)((1UL << 31) - 1))	// 2^31 - 1, don't change this number because it is used in database to indicate that a match is finished

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Mch_ListMatches (struct Game *Game,bool PutFormNewMatch);

void Mch_RequestRemoveMatchTch (void);
void Mch_RemoveMatchTch (void);

void Mch_CreateNewMatchTch (void);
void Mch_RequestStartResumeMatchTch (void);
void Mch_PauseMatchTch (void);
void Mch_ResumeMatchTch (void);
void Mch_ShowResultsQstMatchTch (void);
void Mch_BackMatchTch (void);
void Mch_ForwardMatchTch (void);

void Mch_ShowFinishedMatchResults (void);

void Mch_GetMatchBeingPlayed (void);
void Mch_ShowMatchToMeAsStd (void);
void Mch_RefreshMatchTch (void);
void Mch_RefreshMatchStd (void);

void Mch_ReceiveQstAnsFromStd (void);

void Mch_GetAndDrawBarNumUsrsWhoAnswered (long MchCod,unsigned QstInd,unsigned AnsInd,unsigned NumUsrs);

#endif
