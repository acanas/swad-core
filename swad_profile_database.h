// swad_profile_database.h: user's public profile, operations with database

#ifndef _SWAD_PRF_DB
#define _SWAD_PRF_DB
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

#include "swad_profile.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Prf_DB_CreateUsrFigures (long UsrCod,const struct Prf_UsrFigures *UsrFigures,
                              Usr_MeOrOther_t MeOrOther);
void Prf_DB_UpdateFirstClickTimeUsr (long UsrCod,time_t FirstClickTimeUTC);
void Prf_DB_UpdateNumClicksUsr (long UsrCod,int NumClicks);
void Prf_DB_UpdateNumTimelinePubsUsr (long UsrCod,int NumTimelinePubs);
void Prf_DB_UpdateNumFileViewsUsr (long UsrCod,int NumFileViews);
void Prf_DB_UpdateNumForumPostsUsr (long UsrCod,int NumForumPosts);
void Prf_DB_UpdateNumMessagesSentUsr (long UsrCod,int NumMessagesSent);
void Prf_DB_IncrementNumClicksUsr (long UsrCod);
void Prf_DB_IncrementNumTimelinePubsUsr (long UsrCod);
void Prf_DB_IncrementNumFileViewsUsr (long UsrCod);
void Prf_DB_IncrementNumForPstUsr (long UsrCod);
void Prf_DB_IncrementNumMsgSntUsr (long UsrCod);

unsigned Prf_DB_GetUsrRankingFigure (long UsrCod,const char *FldName);
unsigned Prf_DB_GetRankingFigure (MYSQL_RES **mysql_res,
				  Hie_Level_t HieLvl,const char *FldName);
unsigned Prf_DB_GetRankingClicksPerDay (MYSQL_RES **mysql_res,Hie_Level_t HieLvl);
unsigned Prf_DB_GetNumUsrsWithFigure (const char *FldName);
unsigned Prf_DB_GetRankingNumClicksPerDay (long UsrCod);
unsigned Prf_DB_GetNumUsrsWithNumClicksPerDay (void);
Exi_Exist_t Prf_DB_CheckIfUsrFiguresExists (long UsrCod);
Exi_Exist_t Prf_DB_GetUsrFigures (MYSQL_RES **mysql_res,long UsrCod);
bool Prf_DB_CheckIfUsrBanned (long UsrCod);

void Prf_DB_RemoveUsrFigures (long UsrCod);
void Prf_DB_RemoveUsrFromBanned (long UsrCod);

#endif
