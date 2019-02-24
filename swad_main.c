// swad.c: main

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For exit
#include <string.h>
#include <unistd.h>		// For sleep

#include "swad_action.h"
#include "swad_config.h"
#include "swad_connected.h"
#include "swad_database.h"
#include "swad_firewall.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_MFU.h"
#include "swad_parameter.h"
#include "swad_preference.h"
#include "swad_notification.h"

/*****************************************************************************/
/******************************** Constants **********************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Internal types ******************************/
/*****************************************************************************/

/*****************************************************************************/
/************************ Internal global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Main function ********************************/
/*****************************************************************************/

int main (void)
  {
   void (*FunctionPriori) (void);
   void (*FunctionPosteriori) (void);

   /*
    "touch swad.lock" in CGI directory if you want to disable SWAD
    "rm swad.lock" in CGI directory if you want to enable SWAD
   */
   if (Fil_CheckIfPathExists ("./swad.lock"))
     {
      fprintf (stdout,"Content-type: text/html; charset=windows-1252\r\n"
		      "Status: 503 Service Temporarily Unavailable\r\n\r\n"
		      "<html lang=\"es\">"
		      "<head><title>%s</title></head>"
		      "<body><br /><br /><br /><br />"
		      "<h1 class=\"CENTER_MIDDLE\">"
		      "%s est&aacute; parado por mantenimiento durante unos minutos."
		      "</h1>"
		      "<h2 class=\"CENTER_MIDDLE\">"
		      "Intente acceder m&aacute;s tarde, por favor."
		      "</h2>"
		      "</body>"
		      "</html>",
	       Cfg_PLATFORM_SHORT_NAME,
	       Cfg_PLATFORM_SHORT_NAME);
      exit (0);
     }

   /***** Initialize global variables *****/
   Gbl_InitializeGlobals ();
   Cfg_GetConfigFromFile ();

   /***** Open database connection *****/
   DB_OpenDBConnection ();

   /***** Read parameters *****/
   if (Par_GetQueryString ())
     {
      /***** Get parameters *****/
      Par_CreateListOfParams ();
      Par_GetMainParameters ();

      /***** Mitigate DoS attacks *****/
      FW_CheckFirewallAndExitIfBanned ();
      FW_LogAccess ();
      FW_CheckFirewallAndExitIfTooManyRequests ();

      Hie_InitHierarchy ();
      if (!Gbl.WebService.IsWebService)
	{
	 /***** Create file for HTML output *****/
	 Fil_CreateFileForHTMLOutput ();

	 /***** Remove old (expired) sessions *****/
	 Ses_RemoveExpiredSessions ();

	 /***** Remove old users from connected list *****/
	 Con_RemoveOldConnected ();

	 /***** Get number of sessions *****/
	 if (Act_GetBrowserTab (Gbl.Action.Act) == Act_BRW_1ST_TAB)
	    Ses_GetNumSessions ();

	 /***** Check user and get user's data *****/
	 Usr_ChkUsrAndGetUsrData ();
	}

      /***** Check if the user have permission to execute the action *****/
      if (!Act_CheckIfIHavePermissionToExecuteAction (Gbl.Action.Act))
	 Lay_NoPermissionExit ();

      /***** Update most frequently used actions *****/
      MFU_UpdateMFUActions ();

      /***** Execute a function depending on the action *****/
      FunctionPriori = Act_GetFunctionPriori (Gbl.Action.Act);
      if (FunctionPriori != NULL)
	  FunctionPriori ();

      if (Act_GetBrowserTab (Gbl.Action.Act) == Act_204_NO_CONT)
	 /***** Write HTTP Status 204 No Content *****/
	 Lay_WriteHTTPStatus204NoContent ();

      /***** Start writing HTML output *****/
      Lay_WriteStartOfPage ();

      /***** Make a processing or other depending on the action *****/
      FunctionPosteriori = Act_GetFunctionPosteriori (Gbl.Action.Act);
      if (FunctionPosteriori != NULL)
	  FunctionPosteriori ();
     }

   /***** Cleanup and exit *****/
   Lay_ShowErrorAndExit (NULL);

   return 0; // Control don't reach this point. Used to avoid warning.
  }
