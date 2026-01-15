// swad_follow_database.h: user's followers and followed operations with database

#ifndef _SWAD_FOL_DB
#define _SWAD_FOL_DB
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_follow.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

unsigned Fol_DB_GetUsrsToFollow (unsigned MaxUsrsToShow,
				 Fol_WhichUsersSuggestToFollowThem_t WhichUsersSuggestToFollowThem,
				 MYSQL_RES **mysql_res);

Fol_IsFollower_t Fol_DB_CheckUsrIsFollowerOf (long FollowerCod,long FollowedCod);

unsigned Fol_DB_GetNumFollowing (long UsrCod);
unsigned Fol_DB_GetNumFollowers (long UsrCod);

unsigned Fol_DB_GetListFollowing (MYSQL_RES **mysql_res,long UsrCod);
unsigned Fol_DB_GetListFollowers (MYSQL_RES **mysql_res,long UsrCod);

unsigned Fol_DB_GetNumFollowinFollowers (Hie_Level_t HieLvl,Fol_Follow_t Fol);
double Fol_DB_GetNumFollowedPerFollower (Hie_Level_t HieLvl,Fol_Follow_t Fol);

void Fol_DB_FollowUsr (long UsrCod);
void Fol_DB_UnfollowUsr (long UsrCod);

unsigned Fol_DB_GetRankingFollowers (MYSQL_RES **mysql_res,Hie_Level_t HieLvl);

void Fol_DB_RemoveUsrFromUsrFollow (long UsrCod);

void Fol_DB_CreateTmpTableMeAndUsrsIFollow (void);
void Fol_DB_DropTmpTableMeAndUsrsIFollow (void);

#endif
