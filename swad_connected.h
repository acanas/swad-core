// swad_connected.h: connected users

#ifndef _SWAD_CON
#define _SWAD_CON
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

typedef enum
  {
   Con_SHOW_ON_MAIN_ZONE    = 0,
   Con_SHOW_ON_RIGHT_COLUMN = 1,
  } Con_WhereToShow_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Con_ShowConnectedUsrs (void);
void Con_ShowLastClicks (void);
void Con_GetAndShowLastClicks (void);
void Con_ShowGlobalConnectedUsrs (void);
void Con_ComputeConnectedUsrsBelongingToCurrentCrs (void);
void Con_ShowConnectedUsrsBelongingToScope (void);
void Con_UpdateMeInConnectedList (void);
void Con_RemoveOldConnected (void);

void Con_WriteScriptClockConnected (void);

#endif
