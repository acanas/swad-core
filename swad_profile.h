// swad_profile.h: user's public profile

#ifndef _SWAD_PRF
#define _SWAD_PRF
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <time.h>		// For time

#include "swad_user.h"

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

struct Prf_UsrFigures
  {
   time_t FirstClickTimeUTC;	//  0 ==> unknown first click time of user never logged
   int NumDays;			// -1 ==> not applicable
   int NumClicks;		// -1 ==> unknown number of clicks
   int NumTimelinePubs;		// -1 ==> unknown number of timeline publications
   int NumFileViews;		// -1 ==> unknown number of file views
   int NumForumPosts;		// -1 ==> unknown number of forum posts
   int NumMessagesSent;		// -1 ==> unknown number of messages sent
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Prf_SeeSocialProfiles (void);

char *Prf_GetURLPublicProfile (char URL[WWW_MAX_BYTES_WWW + 1],
                               const char *NickWithoutArr);
void Prf_PutLinkMyPublicProfile (void);
void Prf_PutLinkReqAnotherUsrProfile (void);
void Prf_ReqUserProfile (void);
void Prf_GetUsrDatAndShowUserProfile (void);

Err_SuccessOrError_t Prf_ShowUsrProfile (struct Usr_Data *UsrDat);
void Prf_ChangeBasicProfileVis (void);
void Prf_ChangeExtendedProfileVis (void);

void Prf_ShowDetailsUserProfile (const struct Usr_Data *UsrDat);
void Prf_GetUsrFigures (long UsrCod,struct Prf_UsrFigures *UsrFigures);
void Prf_CalculateFigures (void);
bool Prf_GetAndStoreAllUsrFigures (long UsrCod,struct Prf_UsrFigures *UsrFigures);

void Prf_CreateNewUsrFigures (long UsrCod,Usr_MeOrOther_t MeOrOther);

void Prf_GetAndShowRankingClicks (Hie_Level_t HieLvl);
void Prf_GetAndShowRankingTimelinePubs (Hie_Level_t HieLvl);
void Prf_GetAndShowRankingFileViews (Hie_Level_t HieLvl);
void Prf_GetAndShowRankingForPsts (Hie_Level_t HieLvl);
void Prf_GetAndShowRankingMsgsSnt (Hie_Level_t HieLvl);
void Prf_ShowRankingFigure (MYSQL_RES **mysql_res,unsigned NumUsrs);
void Prf_GetAndShowRankingClicksPerDay (Hie_Level_t HieLvl);

#endif
