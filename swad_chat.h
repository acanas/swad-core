// swad_chat.h: chat

#ifndef _SWAD_CHT
#define _SWAD_CHT
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
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Cht_ShowChatRooms (void);
void Cht_ShowListOfAvailableChatRooms (void);
void Cht_ShowListOfChatRoomsWithUsrs (void);
void Cht_WriteParsRoomCodeAndNames (const char *RoomCode,const char *RoomShortName,const char *RoomFullName);
void Cht_OpenChatWindow (void);

#endif
