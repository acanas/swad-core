// swad_firewall_database.c: firewall to mitigate denial of service attacks, operations with database

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

#include <stdlib.h>	// For exit

#include "swad_database.h"
#include "swad_firewall.h"
#include "swad_global.h"
#include "swad_parameter.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/********************* Log access into firewall recent log *******************/
/*****************************************************************************/

void Fir_DB_LogAccess (void)
  {
   DB_QueryINSERT ("can not log access into firewall_log",
		   "INSERT INTO fir_log"
		   " (ClickTime,IP,UsrCod)"
		   " VALUES"
		   " (NOW(),'%s',%ld)",
		   Par_GetIP (),
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************* Get number of clicks from database ********************/
/*****************************************************************************/

unsigned Fir_DB_GetNumClicksFromLog (void)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not check firewall log",
		  "SELECT COUNT(*)"
		   " FROM fir_log"
		  " WHERE IP='%s'"
		    " AND UsrCod=%ld"
		    " AND ClickTime>FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
		  Par_GetIP (),
		  Gbl.Usrs.Me.UsrDat.UsrCod,
		  Fw_CHECK_INTERVAL);
  }

/*****************************************************************************/
/********************** Remove old clicks from firewall **********************/
/*****************************************************************************/

void Fir_DB_PurgeFirewallLog (void)
  {
   DB_QueryDELETE ("can not purge firewall log",
		   "DELETE LOW_PRIORITY FROM fir_log"
		   " WHERE ClickTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   (unsigned long) Fw_TIME_TO_DELETE_OLD_CLICKS);
  }

/*****************************************************************************/
/********************************* Ban an IP *********************************/
/*****************************************************************************/

void Fir_DB_BanIP (void)
  {
   DB_QueryINSERT ("can not ban IP",
		   "INSERT INTO fir_banned"
		   " (IP,BanTime,UnbanTime)"
		   " VALUES"
		   " ('%s',NOW(),FROM_UNIXTIME(UNIX_TIMESTAMP()+%lu))",
		   Par_GetIP (),
		   (unsigned long) Fw_TIME_BANNED);
  }

/*****************************************************************************/
/***************** Get number of current bans from database ******************/
/*****************************************************************************/

unsigned Fir_DB_GetNumBansIP (void)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not check firewall log",
		  "SELECT COUNT(*)"
		   " FROM fir_banned"
		  " WHERE IP='%s'"
		    " AND UnbanTime>NOW()",
		  Par_GetIP ());
  }
