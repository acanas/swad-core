// swad_test_results.h: test results

#ifndef _SWAD_TSR
#define _SWAD_TSR
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

#include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define TsR_SCORE_MAX	10	// Maximum score of a test (10 in Spain). Must be unsigned! // TODO: Make this configurable by teachers

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void TsR_SelUsrsToViewUsrsTstResults (void);
void TsR_SelDatesToSeeMyTstResults (void);
void TsR_ShowMyTstResults (void);
long TsR_CreateTestResultInDB (bool AllowTeachers);
void TsR_StoreScoreOfTestResultInDB (long TstCod,
                                     unsigned NumQstsNotBlank,double Score);
void TsR_GetUsrsAndShowTstResults (void);
void TsR_ShowOneTstResult (void);
void TsR_ShowTestResult (struct UsrData *UsrDat,
			 unsigned NumQsts,time_t TstTimeUTC,
			 unsigned Visibility);
void TsR_StoreOneTestResultQstInDB (long TstCod,long QstCod,unsigned NumQst,double Score);
void TsR_RemoveTestResultsMadeByUsrInAllCrss (long UsrCod);
void TsR_RemoveTestResultsMadeByUsrInCrs (long UsrCod,long CrsCod);
void TsR_RemoveCrsTestResults (long CrsCod);

#endif
