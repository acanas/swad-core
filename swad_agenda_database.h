// swad_agenda_database.h: user's agenda (personal organizer) operations with database

#ifndef _SWAD_AGD_DB
#define _SWAD_AGD_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_agenda.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

unsigned Agd_DB_GetListEvents (MYSQL_RES **mysql_res,
                               const struct Agd_Agenda *Agenda,
                               Agd_AgendaType_t AgendaType);

unsigned Agd_DB_GetEventDataByCod (MYSQL_RES **mysql_res,
                                   const struct Agd_Event *AgdEvent);
void Agd_DB_GetEventTxt (const struct Agd_Event *AgdEvent,
                         char Txt[Cns_MAX_BYTES_TEXT + 1]);

void Agd_DB_RemoveEvent (const struct Agd_Event *AgdEvent);

long Agd_DB_CreateEvent (const struct Agd_Event *AgdEvent,const char *Txt);
void Agd_DB_UpdateEvent (const struct Agd_Event *AgdEvent,const char *Txt);
void Agd_DB_HideOrUnhideEvent (long AgdCod,long UsrCod,
			       HidVis_HiddenOrVisible_t HiddenOrVisible);
void Agd_DB_MakeEventPublic (const struct Agd_Event *AgdEvent);
void Agd_DB_MakeEventPrivate (const struct Agd_Event *AgdEvent);

void Agd_DB_RemoveUsrEvents (long UsrCod);

unsigned Agd_DB_GetNumEventsFromUsr (long UsrCod);

unsigned Agd_DB_GetNumUsrsWithEvents (Hie_Level_t Level);

unsigned Agd_DB_GetNumEvents (Hie_Level_t Level);

#endif
