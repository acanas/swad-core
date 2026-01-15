// swad_cookie.h:  user's preferences about cookies

#ifndef _SWAD_COO
#define _SWAD_COO
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_hierarchy_type.h"

/*****************************************************************************/
/************************* Public types and constants ************************/
/*****************************************************************************/

#define Coo_COOKIES_ID	"cookies"

#define Coo_NUM_REFUSE_ACCEPT 2
typedef enum
  {
   Coo_REFUSE,
   Coo_ACCEPT,
  } Coo_RefuseAccept_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

Coo_RefuseAccept_t Coo_GetAcceptFromYN (char Ch);

void Coo_EditMyPrefsOnCookies (void);
void Coo_ChangeMyPrefsCookies (void);

//-------------------------------- Figures ------------------------------------
void Coo_GetAndShowNumUsrsPerCookies (Hie_Level_t HieLvl);

#endif
