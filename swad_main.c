// swad.c: main

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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
#include "swad_degree.h"
#include "swad_global.h"
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
extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];

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

int main (int argc, char *argv[])
  {
   extern const char *Txt_You_dont_have_permission_to_perform_this_action;

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
		      "%s est&aacute; parado por mantenimiento durante pocos minutos"
		      "</h1>"
		      "<h2 class=\"CENTER_MIDDLE\">"
		      "Intente acceder m&aacute;s tarde, por favor."
		      "</h2>"
		      "</body>"
		      "</html>",
	       Cfg_PLATFORM_PAGE_TITLE,
	       Cfg_PLATFORM_SHORT_NAME);
      exit (0);
     }

   if (argc > 1)
     {
      fprintf (stdout,"Call %s without parameters",argv[0]);
      return -1;
     }

   /***** Initialize global variables *****/
   Gbl_InitializeGlobals ();

   /***** Get database password and SMTP password from config file *****/
   Cfg_GetConfigFromFile ();

   /***** Open database connection *****/
   DB_OpenDBConnection ();

   /***** Read parameters *****/
   if (Par_GetQueryString ())
     {
      Par_GetMainParameters ();
      Deg_InitCurrentCourse ();

      if (!Gbl.WebService.IsWebService)
	{
	 /***** Check if clicks are made from the same IP too quickly *****/
	 // Disable this call. Don't check if too fast...
	 // ...because uploading files using Dropzone.js will trigger error
	 // Sta_ExitIfTooFast ();

	 /***** Create file for HTML output *****/
	 Fil_CreateFileForHTMLOutput ();

	 /***** Remove old (expired) sessions *****/
	 Ses_RemoveExpiredSessions ();

	 /***** Remove old users from connected list *****/
	 Con_RemoveOldConnected ();

	 /***** Get number of sessions *****/
	 if (Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW)
	    Ses_GetNumSessions ();

	 /***** Check user and get user's data *****/
	 Usr_ChkUsrAndGetUsrData ();
	}

      /***** Check if the user have permission to execute the action *****/
      if (!Act_CheckIfIHavePermissionToExecuteAction (Gbl.CurrentAct))
	 Lay_ShowErrorAndExit (Txt_You_dont_have_permission_to_perform_this_action);

      /***** Update most frequently used actions *****/
      Act_UpdateMFUActions ();

      /***** Execute a function depending on the action *****/
      if (Act_Actions[Gbl.CurrentAct].FunctionPriori != NULL)
	 Act_Actions[Gbl.CurrentAct].FunctionPriori ();

      /***** Start writing HTML output *****/
      Lay_WriteStartOfPage ();

      /***** Make a processing or other depending on the action *****/
      if (Act_Actions[Gbl.CurrentAct].FunctionPosteriori != NULL)
	 Act_Actions[Gbl.CurrentAct].FunctionPosteriori ();
     }

   /***** Cleanup and exit *****/
   Lay_ShowErrorAndExit (NULL);

   return 0; // Control don't reach this point. Used to avoid warning.
  }
