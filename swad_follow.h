// swad_follow.h: user's followers and followed

#ifndef _SWAD_FOL
#define _SWAD_FOL
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_notification.h"

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

typedef enum
  {
   Fol_SUGGEST_ONLY_USERS_WITH_PHOTO,
   Fol_SUGGEST_ANY_USER,
  } Fol_WhichUsersSuggestToFollowThem_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Fol_PutLinkWhoToFollow (void);
void Fol_SuggestUsrsToFollowOnMainZone (void);
void Fol_SuggestUsrsToFollowOnRightColumn (void);

void Fol_FlushCacheFollow (void);
void Fol_GetNumFollow (long UsrCod,
                       unsigned *NumFollowing,unsigned *NumFollowers);
unsigned Fol_GetNumFollowers (long UsrCod);
void Fol_ShowFollowingAndFollowers (const struct Usr_Data *UsrDat,
                                    unsigned NumFollowing,unsigned NumFollowers,
                                    bool UsrFollowsMe,bool IFollowUsr);
void Fol_ListFollowing (void);
void Fol_ListFollowers (void);

void Fol_FollowUsr1 (void);
void Fol_FollowUsr2 (void);
void Fol_UnfollowUsr1 (void);
void Fol_UnfollowUsr2 (void);

void Fol_ReqFollowStds (void);
void Fol_ReqFollowTchs (void);
void Fol_ReqUnfollowStds (void);
void Fol_ReqUnfollowTchs (void);
void Fol_FollowUsrs (void);
void Fol_UnfollowUsrs (void);

void Fol_GetAndShowRankingFollowers (void);

void Fol_GetNotifFollower (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                           char **ContentStr);

void Fol_RemoveUsrFromUsrFollow (long UsrCod);

//-------------------------------- Figures ------------------------------------
void Fol_GetAndShowFollowStats (void);

#endif
