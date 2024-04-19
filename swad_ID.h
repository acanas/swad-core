// swad_ID.h: Users' IDs

#ifndef _SWAD_ID
#define _SWAD_ID
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_user.h"

/*****************************************************************************/
/************************* Public types and constants ************************/
/*****************************************************************************/

#define ID_MIN_DIGITS_AUTOMATIC_DETECT_USR_ID	 6	// Used for detect automatically users' IDs inside a text

#define ID_MIN_DIGITS_USR_ID			 1	// A user's ID must have at least these digits

#define ID_MIN_CHARS_USR_ID			 5	// This number MUST be >= 3
#define ID_MIN_BYTES_USR_ID			ID_MIN_CHARS_USR_ID

#define ID_MAX_CHARS_USR_ID			16
#define ID_MAX_BYTES_USR_ID			ID_MAX_CHARS_USR_ID

#define ID_MAX_BYTES_LIST_USRS_IDS	(ID_MAX_BYTES_USR_ID * Cfg_MAX_USRS_IN_LIST * 10)

struct ListIDs
  {
   bool Confirmed;
   char ID[ID_MAX_BYTES_USR_ID + 1];
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void ID_GetListIDsFromUsrCod (struct Usr_Data *UsrDat);
void ID_ReallocateListIDs (struct Usr_Data *UsrDat,unsigned NumIDs);
void ID_FreeListIDs (struct Usr_Data *UsrDat);
unsigned ID_GetListUsrCodsFromUsrID (struct Usr_Data *UsrDat,
                                     const char *EncryptedPassword,	// If NULL or empty ==> do not check password
                                     struct Usr_ListUsrCods *ListUsrCods,
                                     bool OnlyConfirmedIDs);

void ID_PutParOtherUsrIDPlain (void);
void ID_GetParOtherUsrIDPlain (void);

bool ID_CheckIfUsrIDIsValid (const char *UsrID);
bool ID_CheckIfUsrIDSeemsAValidID (const char *UsrID);

void ID_WriteUsrIDs (struct Usr_Data *UsrDat,const char *Anchor);
Usr_Can_t ID_ICanSeeOtherUsrIDs (const struct Usr_Data *UsrDat);

void ID_ShowFormChangeMyID (bool IShouldFillInID);
void ID_ShowFormChangeOtherUsrID (void);

void ID_RemoveMyUsrID (void);
void ID_RemoveOtherUsrID (void);
void ID_NewMyUsrID (void);
void ID_ChangeOtherUsrID (void);

void ID_ConfirmOtherUsrID (void);

#endif
