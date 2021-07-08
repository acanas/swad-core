// swad_profile.h: user's public profile

#ifndef _SWAD_PRF
#define _SWAD_PRF
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

struct UsrFigures
  {
   time_t FirstClickTimeUTC;	//  0 ==> unknown first click time of user never logged
   int NumDays;			// -1 ==> not applicable
   int NumClicks;		// -1 ==> unknown number of clicks
   int NumSocPub;		// -1 ==> unknown number of social publications
   int NumFileViews;		// -1 ==> unknown number of file views
   int NumForPst;		// -1 ==> unknown number of forum posts
   int NumMsgSnt;		// -1 ==> unknown number of messages sent
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Prf_SeeSocialProfiles (void);

char *Prf_GetURLPublicProfile (char URL[Cns_MAX_BYTES_WWW + 1],
                               const char *NickWithoutArr);
void Prf_PutLinkMyPublicProfile (void);
void Prf_PutLinkRequestAnotherUserProfile (void);
void Prf_RequestUserProfile (void);
void Prf_GetUsrDatAndShowUserProfile (void);

bool Prf_ShowUserProfile (struct UsrData *UsrDat);
void Prf_ChangeBasicProfileVis (void);
void Prf_ChangeExtendedProfileVis (void);

void Prf_ShowDetailsUserProfile (const struct UsrData *UsrDat);
void Prf_GetUsrFigures (long UsrCod,struct UsrFigures *UsrFigures);
void Prf_CalculateFigures (void);
bool Prf_GetAndStoreAllUsrFigures (long UsrCod,struct UsrFigures *UsrFigures);

void Prf_CreateNewUsrFigures (long UsrCod,bool CreatingMyOwnAccount);
void Prf_DB_RemoveUsrFigures (long UsrCod);
void Prf_DB_IncrementNumClicksUsr (long UsrCod);
void Prf_DB_IncrementNumPubsUsr (long UsrCod);
void Prf_DB_IncrementNumFileViewsUsr (long UsrCod);
void Prf_DB_IncrementNumForPstUsr (long UsrCod);
void Prf_DB_IncrementNumMsgSntUsr (long UsrCod);

void Prf_GetAndShowRankingClicks (void);
void Prf_GetAndShowRankingSocPub (void);
void Prf_GetAndShowRankingFileViews (void);
void Prf_GetAndShowRankingForPst (void);
void Prf_GetAndShowRankingMsgSnt (void);
void Prf_ShowRankingFigure (MYSQL_RES **mysql_res,unsigned NumUsrs);
void Prf_GetAndShowRankingClicksPerDay (void);

#endif
