// swad_session.h: sessions

#ifndef _SWAD_SES
#define _SWAD_SES
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_action.h"
#include "swad_pagination.h"

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   Ses_NOT_OPEN,	// When no user logged or session is not yet open
   Ses_OPEN,		// When user is logged and session is open and not expired
   Ses_EXPIRED,		// When session has expired
   Ses_CLOSED,		// When user has closed session
  } Ses_Status_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Ses_GetNumSessions (void);
void Ses_CreateSession (void);
void Ses_CloseSession (void);

bool Ses_GetSessionData (void);

//---------------------------- Session parameters -----------------------------
void Ses_InsertParInDB (const char *ParName,const char *ParValue);

#endif
