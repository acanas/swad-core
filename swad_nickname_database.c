// swad_nickname_database.c: Users' nicknames operations with database

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

#include "swad_database.h"
#include "swad_nickname_database.h"

/*****************************************************************************/
/******************* Update user's nickname in database **********************/
/*****************************************************************************/

void Nck_DB_UpdateNick (long UsrCod,const char *NewNickname)
  {
   DB_QueryREPLACE ("can not update nickname",
		    "REPLACE INTO usr_nicknames"
		    " (UsrCod,Nickname,CreatTime)"
		    " VALUES"
		    " (%ld,'%s',NOW())",
                    UsrCod,
                    NewNickname);
  }

/*****************************************************************************/
/************** Get user's code of a user from his/her nickname **************/
/*****************************************************************************/

long Nck_DB_GetUsrCodFromNickname (const char *NickWithoutArr)
  {
   return DB_QuerySELECTCode ("can not get user's code",
			      "SELECT usr_nicknames.UsrCod"
			       " FROM usr_nicknames,"
				     "usr_data"
			      " WHERE usr_nicknames.Nickname='%s'"
				" AND usr_nicknames.UsrCod=usr_data.UsrCod",
			      NickWithoutArr);
  }

/*****************************************************************************/
/************* Get nickname of a user from his/her user's code ***************/
/*****************************************************************************/

void Nck_DB_GetNicknameFromUsrCod (long UsrCod,
                                   char NickWithoutArr[Nck_MAX_BYTES_NICK_WITHOUT_ARROBA + 1])
  {
   /***** Get current (last updated) user's nickname from database *****/
   DB_QuerySELECTString (NickWithoutArr,Nck_MAX_BYTES_NICK_WITHOUT_ARROBA,
                         "can not get nickname",
		         "SELECT Nickname"
		          " FROM usr_nicknames"
		         " WHERE UsrCod=%ld"
		      " ORDER BY CreatTime DESC"
		         " LIMIT 1",
		         UsrCod);
  }

/*****************************************************************************/
/***************************** Get my nicknames ******************************/
/*****************************************************************************/

unsigned Nck_DB_GetUsrNicknames (MYSQL_RES **mysql_res,long UsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get nicknames of a user",
		   "SELECT Nickname"	// row[0]
		    " FROM usr_nicknames"
		   " WHERE UsrCod=%ld"
		" ORDER BY CreatTime DESC",
		   UsrCod);
  }

/*****************************************************************************/
/*************** Check if nickname matches any of the nicknames **************/
/*****************************************************************************/

bool Nck_DB_CheckIfNickMatchesAnyNick (const char *NickWithoutArr)
  {
   return
   DB_QueryEXISTS ("can not check if nickname already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM usr_nicknames"
		    " WHERE Nickname='%s')",
		   NickWithoutArr) == Exi_EXISTS;
  }

/*****************************************************************************/
/************ Check if nickname matches any of a user's nicknames ************/
/*****************************************************************************/

bool Nck_DB_CheckIfNickMatchesAnyUsrNick (long UsrCod,const char *NickWithoutArr)
  {
   return
   DB_QueryEXISTS ("can not check if nickname already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM usr_nicknames"
		    " WHERE UsrCod=%ld"
		      " AND Nickname='%s')",
		   UsrCod,
		   NickWithoutArr) == Exi_EXISTS;
  }

/*****************************************************************************/
/********* Check if nickname matches any of other user's nicknames ***********/
/*****************************************************************************/

bool Nck_DB_CheckIfNickMatchesAnyOtherUsrsNicks (long UsrCod,const char *NickWithoutArr)
  {
   return
   DB_QueryEXISTS ("can not check if nickname already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM usr_nicknames"
		    " WHERE Nickname='%s'"
		      " AND UsrCod<>%ld)",
		   NickWithoutArr,
		   UsrCod) == Exi_EXISTS;	// A nickname of another user is the same that user's nickname
  }

/*****************************************************************************/
/********************** Remove a nickname from database **********************/
/*****************************************************************************/

void Nck_DB_RemoveNickname (long UsrCod,const char *Nickname)
  {
   DB_QueryREPLACE ("can not remove a nickname",
		    "DELETE FROM usr_nicknames"
		    " WHERE UsrCod=%ld"
		      " AND Nickname='%s'",
                    UsrCod,
                    Nickname);
  }

/*****************************************************************************/
/************************** Remove user's nicknames **************************/
/*****************************************************************************/

void Nck_DB_RemoveUsrNicknames (long UsrCod)
  {
   DB_QueryDELETE ("can not remove user's nicknames",
		   "DELETE FROM usr_nicknames"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }
