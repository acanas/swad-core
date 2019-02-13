// swad_firewall.c: firewall to mitigate mitigate denial of service attacks

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Fw_CHECK_INTERVAL		((time_t)(10UL))	// Check clicks in the last 10 seconds
#define Fw_MAX_CLICKS_IN_INTERVAL	30			// Maximum of 30 clicks allowed in 10 seconds

#define Fw_TIME_TO_DELETE_OLD_CLICKS	Fw_CHECK_INTERVAL	// Remove clicks older than these seconds

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

/*****************************************************************************/
/************************** Log access into firewall *************************/
/*****************************************************************************/

void FW_LogAccess (void)
  {
   /***** Log access in firewall recent log *****/
   DB_QueryINSERT ("can not log access into firewall",
		   "INSERT INTO firewall (ClickTime,IP) VALUES (NOW(),'%s')",
		   Gbl.IP);
  }

/*****************************************************************************/
/************************** Log access into firewall *************************/
/*****************************************************************************/

void FW_CheckFirewallAndExitIfTooManyRequests (void)
  {
   unsigned long NumClicks;

   /***** Get number of clicks from database *****/
   NumClicks = DB_QueryCOUNT ("can not check firewall",
		              "SELECT COUNT(*) FROM firewall"
			      " WHERE IP='%s'"
			      " AND ClickTime>FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
			      Gbl.IP,
                              Fw_CHECK_INTERVAL);

   /***** Exit with status 429 if too many connections *****/
   /* RFC 6585 suggests "429 Too Many Requests", according to
      https://stackoverflow.com/questions/46664695/whats-the-correct-http-response-code-to-return-for-denial-of-service-dos-atta
      https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/429 */
   if (NumClicks > Fw_MAX_CLICKS_IN_INTERVAL)
     {
      /* Return status 429 Too Many Requests */
      fprintf (stdout,"Content-Type: text/html; charset=windows-1252\n"
                      "Retry-After: 3600\n"
	              "Status: 429\r\n\r\n"
                      "<html>"
                      "<head>"
                      "<title>Too Many Requests</title>"
                      "</head>"
                      "<body>"
                      "<h1>Please stop that</h1>"
                      "</body>"
                      "</html>\n");

      /* Close database connection and exit */
      DB_CloseDBConnection ();
      exit (0);
     }
  }

/*****************************************************************************/
/********************** Remove old clicks from firewall **********************/
/*****************************************************************************/

void FW_PurgeFirewall (void)
  {
   /***** Remove old clicks *****/
   DB_QueryDELETE ("can not purge firewall",
		   "DELETE LOW_PRIORITY FROM firewall"
		   " WHERE ClickTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Fw_TIME_TO_DELETE_OLD_CLICKS);
  }
