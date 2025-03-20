// swad.c: main

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <stddef.h>		// For NULL
#include <stdlib.h>		// For exit
#include <string.h>
#include <unistd.h>		// For sleep

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_announcement.h"
#include "swad_config.h"
#include "swad_connected_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_firewall.h"
#include "swad_firewall_database.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_type.h"
#include "swad_MFU.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_session.h"
#include "swad_session_database.h"
#include "swad_setting.h"
#include "swad_user.h"

/*****************************************************************************/
/******************************** Constants **********************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

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
		      "<body><br><br><br><br>"
		      "<h1 class=\"CM\">"
		      "%s est&aacute; parado un momento por mantenimiento."
		      "</h1>"
		      "<h2 class=\"CM\">"
		      "Intente acceder pasados unos minutos, por favor."
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
      Par_CreateListOfPars ();
      Par_GetMainPars ();

      /***** Kick out banned IPs *****/
      Fir_CheckFirewallAndExitIfBanned ();

      /**** Initialize current country, institution, center, degree and course *****/
      Hie_InitHierarchy ();

      if (!Gbl.WebService.IsWebService)
	{
	 /***** Create file for HTML output *****/
	 Fil_CreateFileForHTMLOutput ();

	 /***** Remove old (expired) sessions *****/
	 Ses_DB_RemoveExpiredSessions ();

	 /***** Remove old users from connected list *****/
	 Con_DB_RemoveOldConnected ();

	 /***** Get number of sessions *****/
	 switch (Act_GetBrowserTab (Gbl.Action.Act))
	   {
	    case Act_1ST:
	    case Act_REF:
	       Ses_GetNumSessions ();
               break;
	    default:
	       break;
	   }

	 /***** Check user and get user's data *****/
	 Usr_ChkUsrAndGetUsrData ();
	}

      /***** Mitigate automatized attacks from the same IP-user *****/
      // If this execution is web service, no user is logged at this moment...
      // ...so only IP is checked and it could be banned...
      // ...if many users use the web service from the same IP
      Fir_DB_LogAccess ();
      Fir_CheckFirewallAndExitIfTooManyRequests ();

      /***** Check if the user have permission to execute the action *****/
      if (Act_CheckIfICanExecuteAction (Gbl.Action.Act) == Usr_CAN_NOT)
	 Err_NoPermissionExit ();

      /***** Update most frequently used actions *****/
      MFU_UpdateMFUActions ();

      /***** Execute a function depending on the action *****/
      FunctionPriori = Act_GetFunctionPriori (Gbl.Action.Act);
      if (FunctionPriori != NULL)
	  FunctionPriori ();

      if (Act_GetBrowserTab (Gbl.Action.Act) == Act_204)
	 /***** Write HTTP Status 204 No Content *****/
	 Lay_WriteHTTPStatus204NoContent ();

      /***** Begin writing HTML output *****/
      Lay_WriteStartOfPage ();

      /***** If I am been redirected from another action... *****/
      switch (Gbl.Action.Original)
	{
	 case ActLogIn:
	 case ActLogInNew:
	 case ActLogInLan:
	    Usr_WelcomeUsr ();
	    break;
	 case ActAnnSee:
	    Ann_MarkAnnouncementAsSeen ();
	    break;
	 default:
	    break;
	}

      /***** Make a processing or other depending on the action *****/
      FunctionPosteriori = Act_GetFunctionPosteriori (Gbl.Action.Act);
      if (FunctionPosteriori != NULL)
	  FunctionPosteriori ();

      /* When updating a small zone via AJAX, all output is already done */
      switch (Act_GetBrowserTab (Gbl.Action.Act))
        {
         case Act_AJA:
         case Act_REF:
         case Act_SVC:
	    /* All the output is made, so don't write anymore */
	    Gbl.Layout.DivsEndWritten =
            Gbl.Layout.HTMLEndWritten = true;
	    break;
         default:
            break;
        }
     }

   /***** Cleanup and exit *****/
   Err_ShowErrorAndExit (NULL);

   return 0; // Control don't reach this point. Used to avoid warning.
  }
