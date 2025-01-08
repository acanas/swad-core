// swad_chat_database.c: chat operations with database

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_chat.h"
#include "swad_chat_database.h"
#include "swad_database.h"

/*****************************************************************************/
/********************** Get list of chat rooms with users ********************/
/*****************************************************************************/

unsigned Cht_DB_GetListOfChatRoomsWithUsrs (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get chat rooms with connected users",
		   "SELECT RoomCode,"		// row[0]
			  "NumUsrs"		// row[1]
		    " FROM cht_rooms"
		   " WHERE NumUsrs>0"
		" ORDER BY NumUsrs DESC,"
			  "RoomCode");
  }

/*****************************************************************************/
/*************** Get number of users connected to a chat room ****************/
/*****************************************************************************/

unsigned Cht_DB_GetNumUsrsInChatRoom (const char *RoomCode)
  {
   return DB_QuerySELECTUnsigned ("can not get number of users"
				  " connected to a chat room",
				  "SELECT NumUsrs"
				   " FROM cht_rooms"
				  " WHERE RoomCode='%s'",
				  RoomCode);
  }
