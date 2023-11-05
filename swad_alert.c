// swad_alert.c: alerts

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For vasprintf
#include <stdarg.h>		// For va_start, va_end
#include <stddef.h>		// For NULL
#include <stdio.h>		// For FILE, vasprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

static struct
  {
   size_t Num;		// Number of alert
   struct
     {
      Ale_AlertType_t Type;
      char *Text;	// Message to be displayed
      char *Section;	// Where to display the alert
     } List[Ale_MAX_ALERTS];
  } Ale_Alerts =	// Alert message created in a function and printed in a subsequent function
  {
   .Num = 0,		// No pending alerts to be shown
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ale_ResetLastAlert (void);
static void Ale_ResetAlert (size_t NumAlert);

static void Ale_ShowFixAlert (Ale_AlertType_t AlertType,const char *Txt);

static void Ale_ShowFixAlertAndButtonBegin (Ale_AlertType_t AlertType,const char *Txt);

/*****************************************************************************/
/**************************** Create a new alert *****************************/
/*****************************************************************************/

void Ale_CreateAlert (Ale_AlertType_t Type,const char *Section,
                      const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   size_t i;

   if (Ale_Alerts.Num + 1 > Ale_MAX_ALERTS)
      Err_ShowErrorAndExit ("Too many alerts.");

   i = Ale_Alerts.Num;
   Ale_Alerts.Num++;

   Ale_Alerts.List[i].Type = Type;

   Ale_Alerts.List[i].Section = NULL;
   if (Section)
      if (Section[0])
	 if (asprintf (&Ale_Alerts.List[i].Section,"%s",
	               Section) < 0)
	    Err_NotEnoughMemoryExit ();

   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Ale_Alerts.List[i].Text,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();
  }

/*****************************************************************************/
/***************** Get current number of delayed alerts **********************/
/*****************************************************************************/

size_t Ale_GetNumAlerts (void)
  {
   return Ale_Alerts.Num;
  }

/*****************************************************************************/
/********************** Get type of last delayed alert ***********************/
/*****************************************************************************/

Ale_AlertType_t Ale_GetTypeOfLastAlert (void)
  {
   return Ale_Alerts.Num ? Ale_Alerts.List[Ale_Alerts.Num - 1].Type :
			   Ale_NONE;
  }

/*****************************************************************************/
/********************** Get text of last delayed alert ***********************/
/*****************************************************************************/

const char *Ale_GetTextOfLastAlert (void)
  {
   return Ale_Alerts.Num ? Ale_Alerts.List[Ale_Alerts.Num - 1].Text :
			   NULL;
  }

/*****************************************************************************/
/***************************** Reset all alerts ******************************/
/*****************************************************************************/

void Ale_ResetAllAlerts (void)
  {
   size_t NumAlert;

   for (NumAlert = 0;
	NumAlert < Ale_Alerts.Num;
	NumAlert++)
      Ale_ResetAlert (NumAlert);
  }

/*****************************************************************************/
/************************* Reset more recent alert ***************************/
/*****************************************************************************/

static void Ale_ResetLastAlert (void)
  {
   if (Ale_Alerts.Num)	// There are pending alerts not shown
      Ale_ResetAlert (Ale_Alerts.Num - 1);	// Reset the last one
  }

/*****************************************************************************/
/********************* Reset one alert given its index ***********************/
/*****************************************************************************/

static void Ale_ResetAlert (size_t NumAlert)
  {
   bool NoMoreAlertsPending;
   size_t i;

   if (NumAlert < Ale_Alerts.Num)
      if (Ale_Alerts.List[NumAlert].Type != Ale_NONE)
	{
	 /***** Reset alert *****/
	 Ale_Alerts.List[NumAlert].Type = Ale_NONE;	// Reset alert

	 /***** Free memory allocated for text *****/
	 if (Ale_Alerts.List[NumAlert].Text)
	   {
	    free (Ale_Alerts.List[NumAlert].Text);
	    Ale_Alerts.List[NumAlert].Text = NULL;
	   }

	 /***** Free memory allocated for section *****/
	 if (Ale_Alerts.List[NumAlert].Section)
	   {
	    free (Ale_Alerts.List[NumAlert].Section);
	    Ale_Alerts.List[NumAlert].Section = NULL;
	   }
	}

   /***** Set number of alerts to 0
          if there are no more alerts
          pending to be shown *****/
   NoMoreAlertsPending = true;
   for (i = 0;
	NoMoreAlertsPending && i < Ale_Alerts.Num;
	i++)
      if (Ale_Alerts.List[i].Type != Ale_NONE)
	 NoMoreAlertsPending = false;

   if (NoMoreAlertsPending)
      Ale_Alerts.Num = 0;
  }

/*****************************************************************************/
/************************ Show alert messages and exit ***********************/
/*****************************************************************************/

void Ale_ShowAlertsAndExit ()
  {
   Ale_ShowAlerts (NULL);
   Err_ShowErrorAndExit (NULL);
  }

/*****************************************************************************/
/****** Show several alert messages stored in vector of delayed alerts *******/
/*****************************************************************************/
// If Section == NULL ==> show all alerts
// If Section != NULL ==> shown only the alerts assigned to Section

void Ale_ShowAlerts (const char *Section)
  {
   size_t NumAlerts = Ale_GetNumAlerts ();
   size_t NumAlert;
   bool ShowAlert;

   for (NumAlert = 0;
	NumAlert < NumAlerts;
	NumAlert++)
      if (Ale_Alerts.List[NumAlert].Type != Ale_NONE)
        {
	 if (Section)
	    ShowAlert = (bool) !strcmp (Ale_Alerts.List[NumAlert].Section,Section);
	 else
	    ShowAlert = true;

	 if (ShowAlert)
	   {
	    Ale_ShowFixAlert (Ale_Alerts.List[NumAlert].Type,
			      Ale_Alerts.List[NumAlert].Text);
	    Ale_ResetAlert (NumAlert);
	   }
        }
  }

/*****************************************************************************/
/****************** Show one formatted-text alert message ********************/
/*****************************************************************************/

void Ale_ShowAlert (Ale_AlertType_t AlertType,const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Txt;

   if (AlertType != Ale_NONE)
     {
      /***** Print format and list of variables into text *****/
      va_start (ap,fmt);
      NumBytesPrinted = vasprintf (&Txt,fmt,ap);
      va_end (ap);
      if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	 Err_NotEnoughMemoryExit ();

      /***** Show alert *****/
      Ale_ShowFixAlert (AlertType,Txt);

      /***** Free text *****/
      free (Txt);
     }
  }

/*****************************************************************************/
/********************** Show one fix-text alert message **********************/
/*****************************************************************************/

static void Ale_ShowFixAlert (Ale_AlertType_t AlertType,const char *Txt)
  {
   if (AlertType != Ale_NONE)
     {
      /****** Print fix alert and button ******/
      Ale_ShowFixAlertAndButtonBegin (AlertType,Txt);
      Ale_ShowAlertAndButtonEnd (ActUnk,NULL,NULL,
			       NULL,NULL,
			       Btn_NO_BUTTON,NULL);
     }
  }

/*****************************************************************************/
/**************** Show the more recent alert with a button *******************/
/*****************************************************************************/

void Ale_ShowLastAlertAndButton (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit,
                                 void (*FuncPars) (void *Args),void *Args,
				 Btn_Button_t Button,const char *TxtButton)
  {
   /***** Show last alert and then reset it *****/
   Ale_ShowLastAlertAndButtonBegin ();

   /***** Show button *****/
   Ale_ShowAlertAndButtonEnd (NextAction,Anchor,OnSubmit,
                              FuncPars,Args,
                              Button,TxtButton);
  }

/*****************************************************************************/
/********** Show the first part of more recent alert with a button ***********/
/*****************************************************************************/

void Ale_ShowLastAlertAndButtonBegin (void)
  {
   /***** Show last alert *****/
   Ale_ShowFixAlertAndButtonBegin (Ale_GetTypeOfLastAlert (),
				   Ale_GetTextOfLastAlert ());

   /***** Reset last alert *****/
   Ale_ResetLastAlert ();
  }

/*****************************************************************************/
/************** Show an alert with a button to confirm removal ***************/
/*****************************************************************************/

void Ale_ShowAlertRemove (Act_Action_t NextAction,const char *Anchor,
                          void (*FuncPars) (void *Args),void *Args,
			  const char *Txt,const char *Title)
  {
   extern const char *Txt_Remove;

   Ale_ShowAlertAndButton (NextAction,Anchor,NULL,
                           FuncPars,Args,
			   Btn_REMOVE_BUTTON,Txt_Remove,
			   Ale_QUESTION,Txt,
			   Title);
  }

/*****************************************************************************/
/*********************** Show an alert with a button *************************/
/*****************************************************************************/

void Ale_ShowAlertAndButton (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit,
                             void (*FuncPars) (void *Args),void *Args,
                             Btn_Button_t Button,const char *TxtButton,
			     Ale_AlertType_t AlertType,const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Txt;

   /***** Print format and list of variables into text *****/
   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Txt,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /****** Print fix alert and button ******/
   Ale_ShowFixAlertAndButtonBegin (AlertType,Txt);
   Ale_ShowAlertAndButtonEnd (NextAction,Anchor,OnSubmit,
                              FuncPars,Args,
                              Button,TxtButton);

   /***** Free text *****/
   free (Txt);
  }

/*****************************************************************************/
/******** Show the first part of a formatted-text alert with a button ********/
/*****************************************************************************/

void Ale_ShowAlertAndButtonBegin (Ale_AlertType_t AlertType,const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Txt;

   /***** Print format and list of variables into text *****/
   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Txt,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /****** Print start of fix alert and button ******/
   Ale_ShowFixAlertAndButtonBegin (AlertType,Txt);

   /***** Free text *****/
   free (Txt);
  }

/*****************************************************************************/
/*********** Show the first part of a fix-text alert with a button ***********/
/*****************************************************************************/

static void Ale_ShowFixAlertAndButtonBegin (Ale_AlertType_t AlertType,const char *Txt)
  {
   extern const char *Txt_Close;
   char IdAlert[Frm_MAX_BYTES_ID + 1];
   static const bool AlertClosable[Ale_NUM_ALERT_TYPES] =
     {
      [Ale_NONE     ] = false,
      [Ale_CLIPBOARD] = true,
      [Ale_INFO     ] = true,
      [Ale_SUCCESS  ] = true,
      [Ale_QUESTION ] = true,
      [Ale_WARNING  ] = true,
      [Ale_ERROR    ] = true,
    };
   static const char *Ale_AlertIcons[Ale_NUM_ALERT_TYPES] =
     {
      [Ale_NONE     ] = NULL,
      [Ale_CLIPBOARD] = "clipboard.svg",
      [Ale_INFO     ] = "info-circle-blue.svg",
      [Ale_SUCCESS  ] = "check-circle-green.svg",
      [Ale_QUESTION ] = "question64x64.gif",	// animated gif
      [Ale_WARNING  ] = "warning64x64.gif",	// animated gif
      [Ale_ERROR    ] = "error64x64.gif",	// animated gif
     };

   /****** If start of page is not written yet, do it now ******/
   if (!Gbl.Layout.HTMLStartWritten)
      Lay_WriteStartOfPage ();

   /***** Begin container *****/
   if (AlertClosable[AlertType])
     {
      /* Create unique id for alert */
      Frm_SetUniqueId (IdAlert);
      HTM_DIV_Begin ("id=\"%s\" class=\"CM\"",IdAlert);
     }
   else
      HTM_DIV_Begin ("class=\"CM\"");

   /***** Begin box *****/
   HTM_DIV_Begin ("class=\"ALERT ALERT_BG_%s\"",The_GetSuffix ());

      /***** Icon to close the alert *****/
      if (AlertClosable[AlertType])
	{
	 HTM_DIV_Begin ("class=\"ALERT_CLOSE\"");
	    HTM_A_Begin ("href=\"\" onclick=\"toggleDisplay('%s');return false;\" /",
			 IdAlert);
	       Ico_PutIcon ("times.svg",Ico_BLACK,Txt_Close,"ICO16x16");
	    HTM_A_End ();
	 HTM_DIV_End ();
	}

      /***** Write message *****/
      if (AlertType == Ale_NONE)
	 HTM_DIV_Begin ("class=\"ALERT_TXT ALERT_TXT_%s\"",
	                The_GetSuffix ());
      else
	 HTM_DIV_Begin ("class=\"ALERT_TXT ALERT_TXT_%s\""
		        " style=\"background-image:url('%s/%s');\"",
	                The_GetSuffix (),
			Cfg_URL_ICON_PUBLIC,Ale_AlertIcons[AlertType]);
      HTM_Txt (Txt);
      HTM_DIV_End ();
  }

/*****************************************************************************/
/*************** Show the second part of an alert with a button **************/
/*****************************************************************************/

void Ale_ShowAlertAndButtonEnd (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit,
                                void (*FuncPars) (void *Args),void *Args,
                                Btn_Button_t Button,const char *TxtButton)
  {
      /***** Optional button *****/
      if (NextAction != ActUnk &&
	  Button != Btn_NO_BUTTON &&
	  TxtButton)
	 if (TxtButton[0])
	   {
	    /* Begin form */
	    Frm_BeginFormAnchorOnSubmit (NextAction,Anchor,OnSubmit);
	       if (FuncPars)
		  FuncPars (Args);

	       /* Put button */
	       Btn_PutButton (Button,TxtButton);

	    /* End form */
	    Frm_EndForm ();
	   }

      /***** End box *****/
      HTM_DIV_End ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**************** Create alert when a form field is empty ********************/
/*****************************************************************************/

void Ale_ShowAlertYouCanNotLeaveFieldEmpty (void)
  {
   extern const char *Txt_You_can_not_leave_the_field_empty;

   Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_the_field_empty);
  }

/*****************************************************************************/
/*** Show alert when user not found or no permission to perform an action ****/
/*****************************************************************************/

void Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/** Create alert when user not found or no permission to perform an action ***/
/*****************************************************************************/

void Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   Ale_CreateAlert (Ale_WARNING,NULL,
		    Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/**************** Create alert when a form field is empty ********************/
/*****************************************************************************/

void Ale_CreateAlertYouCanNotLeaveFieldEmpty (void)
  {
   extern const char *Txt_You_can_not_leave_the_field_empty;

   Ale_CreateAlert (Ale_WARNING,NULL,Txt_You_can_not_leave_the_field_empty);
  }

/*****************************************************************************/
/********************* Create alert when title is empty **********************/
/*****************************************************************************/

void Ale_CreateAlertYouMustSpecifyTheTitle (void)
  {
   extern const char *Txt_You_must_specify_the_title;

   Ale_CreateAlert (Ale_WARNING,NULL,
		    Txt_You_must_specify_the_title);
  }

/*****************************************************************************/
/****************** Create alert when web address is empty *******************/
/*****************************************************************************/

void Ale_CreateAlertYouMustSpecifyTheWebAddress (void)
  {
   extern const char *Txt_You_must_specify_the_web_address;

   Ale_CreateAlert (Ale_WARNING,NULL,
		    Txt_You_must_specify_the_web_address);
  }

/*****************************************************************************/
/************ Create alert when short name or full name are empty ************/
/*****************************************************************************/

void Ale_CreateAlertYouMustSpecifyTheName (void)
  {
   extern const char *Txt_You_must_specify_the_name;

   Ale_CreateAlert (Ale_WARNING,NULL,
		    Txt_You_must_specify_the_name);
  }

/*****************************************************************************/
/************ Create alert when short name or full name are empty ************/
/*****************************************************************************/

void Ale_CreateAlertYouMustSpecifyShrtNameAndFullName (void)
  {
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name;

   Ale_CreateAlert (Ale_WARNING,NULL,
		    Txt_You_must_specify_the_short_name_and_the_full_name);
  }
