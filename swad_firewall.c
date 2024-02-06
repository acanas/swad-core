// swad_firewall.c: firewall to mitigate denial of service attacks

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
#include "swad_firewall_database.h"

/*****************************************************************************/
/****************************** Private prototypes ***************************/
/*****************************************************************************/

static void Fir_WriteHTML (const char *Title,const char *H1);

/*****************************************************************************/
/*************************** Check if IP is banned ***************************/
/*****************************************************************************/

void Fir_CheckFirewallAndExitIfBanned (void)
  {
   unsigned NumCurrentBans;

   /***** Get number of current bans from database *****/
   NumCurrentBans = Fir_DB_GetNumBansIP ();

   /***** Exit with status 403 if banned *****/
   /* RFC 6585 suggests "403 Forbidden", according to
      https://stackoverflow.com/questions/7447283/proper-http-status-to-return-for-hacking-attempts
      https://tools.ietf.org/html/rfc2616#section-10.4.4 */
   if (NumCurrentBans)
     {
      /* Return status 403 Forbidden */
      fprintf (stdout,"Content-Type: text/html; charset=windows-1252\n"
	              "Status: 403\r\n\r\n");
      Fir_WriteHTML ("Forbidden","You are temporarily banned");

      /* Close database connection and exit */
      DB_CloseDBConnection ();
      exit (0);
     }
  }

/*****************************************************************************/
/**************** Check if too many connections from this IP *****************/
/*****************************************************************************/

void Fir_CheckFirewallAndExitIfTooManyRequests (void)
  {
   unsigned NumClicks;

   /***** Get number of clicks from database *****/
   NumClicks = Fir_DB_GetNumClicksFromLog ();

   /***** Exit with status 429 if too many connections *****/
   /* RFC 6585 suggests "429 Too Many Requests", according to
      https://stackoverflow.com/questions/46664695/whats-the-correct-http-response-code-to-return-for-denial-of-service-dos-atta
      https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/429 */
   if (NumClicks > Fw_MAX_CLICKS_IN_INTERVAL)
     {
      /* Ban this IP */
      Fir_DB_BanIP ();

      /* Return status 429 Too Many Requests */
      fprintf (stdout,"Content-Type: text/html; charset=windows-1252\n"
                      "Retry-After: %lu\n"
	              "Status: 429\r\n\r\n",
	       (unsigned long) Fw_TIME_BANNED);
      Fir_WriteHTML ("Too Many Requests","Please stop that");

      /* Close database connection and exit */
      DB_CloseDBConnection ();
      exit (0);
     }
  }

/*****************************************************************************/
/********************************* Ban an IP *********************************/
/*****************************************************************************/

static void Fir_WriteHTML (const char *Title,const char *H1)
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
