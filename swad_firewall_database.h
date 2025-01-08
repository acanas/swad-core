// swad_firewall_database.h: firewall to mitigate denial of service attacks, operations with database

#ifndef _SWAD_FW_DB
#define _SWAD_FW_DB
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
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Fir_DB_LogAccess (void);
unsigned Fir_DB_GetNumClicksFromLog (void);
void Fir_DB_PurgeFirewallLog (void);

void Fir_DB_BanIP (void);
unsigned Fir_DB_GetNumBansIP (void);

#endif
