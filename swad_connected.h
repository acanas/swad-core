// swad_connected.h: connected users

#ifndef _SWAD_CON
#define _SWAD_CON
/*
    SWAD (Shared Workspace At a Distance),
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Con_MIN_TIME_TO_REFRESH_CONNECTED_IN_MS	(Cfg_MIN_TIME_TO_REFRESH_CONNECTED * 1000L)	// Refresh period of connected users in miliseconds
#define Con_MAX_TIME_TO_REFRESH_CONNECTED_IN_MS	(Cfg_MAX_TIME_TO_REFRESH_CONNECTED * 1000L)	// Refresh period of connected users in miliseconds

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

struct Con_ConnectedUsrs
  {
   unsigned NumUsrs;
   Usr_Sex_t Sex;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Con_SetTimeToRefresh (unsigned NumSessions);
void Con_RefreshConnected (void);

void Con_ShowConnectedUsrs (void);
void Con_ShowGlobalConnectedUsrs (void);
void Con_ComputeConnectedUsrsBelongingToCurrentCrs (void);
void Con_ShowConnectedUsrsBelongingToCurrentCrs (void);

void Con_WriteScriptClockConnected (void);

#endif
