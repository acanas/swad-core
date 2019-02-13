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

#include <stdlib.h>	// For exit

#include "swad_database.h"
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/* The maximum number of clicks in the interval
   should be large enough to prevent an IP from being banned
   due to automatic refresh when the user is viewing the last clicks. */
#define Fw_CHECK_INTERVAL		((time_t)(10UL))	// Check clicks in the last 10 seconds
#define Fw_MAX_CLICKS_IN_INTERVAL	30			// Maximum of 30 clicks allowed in 10 seconds

#define Fw_TIME_BANNED			((time_t)(60UL*60UL))	// Ban IP for 1 hour

#define Fw_TIME_TO_DELETE_OLD_CLICKS	Fw_CHECK_INTERVAL	// Remove clicks older than these seconds

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

static void FW_BanIP (void);

static void FW_WriteHTML (const char *Title,const char *H1);

/*****************************************************************************/
/************************** Log access into firewall *************************/
/*****************************************************************************/

void FW_LogAccess (void)
  {
   /***** Log access in firewall recent log *****/
   DB_QueryINSERT ("can not log access into firewall_log",
		   "INSERT INTO firewall_log"
		   " (ClickTime,IP)"
		   " VALUES"
		   " (NOW(),'%s')",
		   Gbl.IP);
  }

/*****************************************************************************/
/********************** Remove old clicks from firewall **********************/
/*****************************************************************************/

void FW_PurgeFirewall (void)
  {
   /***** Remove old clicks *****/
   DB_QueryDELETE ("can not purge firewall log",
		   "DELETE LOW_PRIORITY FROM firewall_log"
		   " WHERE ClickTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   (unsigned long) Fw_TIME_TO_DELETE_OLD_CLICKS);
  }

/*****************************************************************************/
/*************************** Check if IP is banned ***************************/
/*****************************************************************************/

void FW_CheckFirewallAndExitIfBanned (void)
  {
   unsigned long NumCurrentBans;

   /***** Get number of current bans from database *****/
   NumCurrentBans = DB_QueryCOUNT ("can not check firewall log",
		                   "SELECT COUNT(*) FROM firewall_banned"
			           " WHERE IP='%s' AND UnbanTime>NOW()",
			           Gbl.IP);

   /***** Exit with status 403 if banned *****/
   /* RFC 6585 suggests "429 Too Many Requests", according to
      https://stackoverflow.com/questions/7447283/proper-http-status-to-return-for-hacking-attempts
      https://tools.ietf.org/html/rfc2616#section-10.4.4 */
   if (NumCurrentBans)
     {
      /* Return status 403 Forbidden */
      fprintf (stdout,"Content-Type: text/html; charset=windows-1252\n"
	              "Status: 403\r\n\r\n");
      FW_WriteHTML ("Forbidden","You are temporarily banned");

      /* Close database connection and exit */
      DB_CloseDBConnection ();
      exit (0);
     }
  }

/*****************************************************************************/
/**************** Check if too many connections from this IP *****************/
/*****************************************************************************/

void FW_CheckFirewallAndExitIfTooManyRequests (void)
  {
   unsigned long NumClicks;

   /***** Get number of clicks from database *****/
   NumClicks = DB_QueryCOUNT ("can not check firewall log",
		              "SELECT COUNT(*) FROM firewall_log"
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
      /* Ban this IP */
      FW_BanIP ();

      /* Return status 429 Too Many Requests */
      fprintf (stdout,"Content-Type: text/html; charset=windows-1252\n"
                      "Retry-After: %lu\n"
	              "Status: 429\r\n\r\n",
	       (unsigned long) Fw_TIME_BANNED);
      FW_WriteHTML ("Too Many Requests","Please stop that");

      /* Close database connection and exit */
      DB_CloseDBConnection ();
      exit (0);
     }
  }

/*****************************************************************************/
/********************************* Ban an IP *********************************/
/*****************************************************************************/

static void FW_BanIP (void)
  {
   /***** Insert IP into table of banned IPs *****/
   DB_QueryINSERT ("can not ban IP",
		   "INSERT INTO firewall_banned"
		   " (IP,BanTime,UnbanTime)"
		   " VALUES"
		   " ('%s',NOW(),FROM_UNIXTIME(UNIX_TIMESTAMP()+%lu))",
		   Gbl.IP,(unsigned long) Fw_TIME_BANNED);
  }

/*****************************************************************************/
/********************************* Ban an IP *********************************/
/*****************************************************************************/

static void FW_WriteHTML (const char *Title,const char *H1)
  {
   fprintf (stdout,"<html>"
		   "<head>"
		   "<title>%s</title>"
		   "</head>"
		   "<body>"
		   "<h1>%s</h1>"
		   "</body>"
		   "</html>\n",
	    Title,H1);
  }
