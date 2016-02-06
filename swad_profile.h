// swad_profile.h: user's public profile

#ifndef _SWAD_PRF
#define _SWAD_PRF
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Prf_SeeSocialProfiles (void);

char *Prf_GetURLPublicProfile (char *URL,const char *NicknameWithoutArroba);
void Prf_PutLinkRequestUserProfile (void);
void Prf_RequestUserProfile (void);
void Prf_GetUsrDatAndShowUserProfile (void);

bool Prf_ShowUserProfile (void);
void Prf_ChangeProfileVisibility (void);
void Prf_CalculateFirstClickTime (void);
void Prf_CalculateNumClicks (void);
void Prf_CalculateNumFileViews (void);
void Prf_CalculateNumForPst (void);
void Prf_CalculateNumMsgSnt (void);

void Prf_CreateNewUsrFigures (long UsrCod);
void Prf_RemoveUsrFigures (long UsrCod);
void Prf_IncrementNumClicksUsr (long UsrCod);
void Prf_IncrementNumFileViewsUsr (long UsrCod);
void Prf_IncrementNumForPstUsr (long UsrCod);
void Prf_IncrementNumMsgSntUsr (long UsrCod);

void Prf_GetAndShowRankingClicks (void);
void Prf_GetAndShowRankingFileViews (void);
void Prf_GetAndShowRankingForPst (void);
void Prf_GetAndShowRankingMsgSnt (void);
void Prf_ShowRankingFigure (const char *Query);
void Prf_GetAndShowRankingClicksPerDay (void);
void Prf_ShowUsrInRanking (const struct UsrData *UsrDat,unsigned Rank);

#endif
