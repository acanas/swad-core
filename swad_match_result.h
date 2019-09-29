// swad_match_result.h: matches results in games using remote control

#ifndef _SWAD_MCR
#define _SWAD_MCR
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

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void McR_PutFormToViewMchResults (Act_Action_t Action);
void McR_SelDatesToSeeMyMchResults (void);
void McR_ShowMyMchResults (void);
void McR_SelUsrsToViewUsrsMchResults (void);
void McR_ShowUsrsMchResults (void);
void McR_ShowOneMchResult (void);
void McR_GetMatchResultQuestionsFromDB (long MchCod,long UsrCod,
					unsigned *NumQsts,unsigned *NumQstsNotBlank);

#endif
