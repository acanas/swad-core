// swad_alert.c: alerts

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include "swad_alert.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ale_ResetLastAlert (void);
static void Ale_ResetAlert (size_t i);

static void Ale_ShowFixAlert (Ale_AlertType_t AlertType,const char *Txt);

static void Ale_ShowFixAlertAndButton1 (Ale_AlertType_t AlertType,const char *Txt);

/*****************************************************************************/
/**************************** Create a new alert *****************************/
/*****************************************************************************/

void Ale_CreateAlert (Ale_AlertType_t Type,const char *Section,
                      const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   size_t i;

   if (Gbl.Alerts.Num + 1 > Ale_MAX_ALERTS)
      Lay_ShowErrorAndExit ("Too many alerts.");

   i = Gbl.Alerts.Num;
   Gbl.Alerts.Num++;

   Gbl.Alerts.List[i].Type = Type;

   Gbl.Alerts.List[i].Section = NULL;
   if (Section)
      if (Section[0])
	 if (asprintf (&Gbl.Alerts.List[i].Section,"%s",
	               Section) < 0)
	    Lay_NotEnoughMemoryExit ();

   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Gbl.Alerts.List[i].Text,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Lay_NotEnoughMemoryExit ();
  }

/*****************************************************************************/
/***************** Get current number of delayed alerts **********************/
/*****************************************************************************/

size_t Ale_GetNumAlerts (void)
  {
   return Gbl.Alerts.Num;
  }

/*****************************************************************************/
/********************** Get type of last delayed alert ***********************/
/*****************************************************************************/

Ale_AlertType_t Ale_GetTypeOfLastAlert (void)
  {
   return Gbl.Alerts.Num ? Gbl.Alerts.List[Gbl.Alerts.Num - 1].Type :
			   Ale_NONE;
  }

/*****************************************************************************/
/********************** Get text of last delayed alert ***********************/
/*****************************************************************************/

const char *Ale_GetTextOfLastAlert (void)
  {
   return Gbl.Alerts.Num ? Gbl.Alerts.List[Gbl.Alerts.Num - 1].Text :
			   NULL;
  }

/*****************************************************************************/
/***************************** Reset all alerts ******************************/
/*****************************************************************************/

void Ale_ResetAllAlerts (void)
  {
   size_t i;

   for (i = 0;
	i < Gbl.Alerts.Num;
	i++)
      Ale_ResetAlert (i);
  }

/*****************************************************************************/
/************************* Reset more recent alert ***************************/
/*****************************************************************************/

static void Ale_ResetLastAlert (void)
  {
   if (Gbl.Alerts.Num)	// There are pending alerts no shown
      Ale_ResetAlert (Gbl.Alerts.Num - 1);	// Reset the last one
  }

/*****************************************************************************/
/********************* Reset one alert given its index ***********************/
/*****************************************************************************/

static void Ale_ResetAlert (size_t i)
  {
   bool NoMoreAlertsPending;
   size_t j;

   if (i < Gbl.Alerts.Num)
      if (Gbl.Alerts.List[i].Type != Ale_NONE)
	{
	 /***** Reset i-esim alert *****/
	 Gbl.Alerts.List[i].Type = Ale_NONE;	// Reset alert

	 /***** Free memory allocated for text *****/
	 if (Gbl.Alerts.List[i].Text)
	   {
	    free (Gbl.Alerts.List[i].Text);
	    Gbl.Alerts.List[i].Text = NULL;
	   }

	 /***** Free memory allocated for section *****/
	 if (Gbl.Alerts.List[i].Section)
	   {
	    free (Gbl.Alerts.List[i].Section);
	    Gbl.Alerts.List[i].Section = NULL;
	   }
	}

   /***** Set number of alerts to 0
          if there are no more alerts
          pending to be shown *****/
   NoMoreAlertsPending = true;
   for (j = 0;
	NoMoreAlertsPending && j < Gbl.Alerts.Num;
	j++)
      if (Gbl.Alerts.List[j].Type != Ale_NONE)
	 NoMoreAlertsPending = false;

   if (NoMoreAlertsPending)
      Gbl.Alerts.Num = 0;
  }

/*****************************************************************************/
/************************ Show alert messages and exit ***********************/
/*****************************************************************************/

void Ale_ShowAlertsAndExit ()
  {
   Ale_ShowAlerts (NULL);
   Lay_ShowErrorAndExit (NULL);
  }

/*****************************************************************************/
/****** Show several alert messages stored in vector of delayed alerts *******/
/*****************************************************************************/
// If Section == NULL ==> show all alerts
// If Section != NULL ==> shown only the alerts assigned to Section

void Ale_ShowAlerts (const char *Section)
  {
   size_t i;
   bool ShowAlert;
   size_t NumAlerts = Ale_GetNumAlerts ();

   for (i = 0;
	i < NumAlerts;
	i++)
      if (Gbl.Alerts.List[i].Type != Ale_NONE)
        {
	 if (Section)
	    ShowAlert = (bool) !strcmp (Gbl.Alerts.List[i].Section,Section);
	 else
	    ShowAlert = true;

	 if (ShowAlert)
	   {
	    Ale_ShowFixAlert (Gbl.Alerts.List[i].Type,
			      Gbl.Alerts.List[i].Text);
	    Ale_ResetAlert (i);
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
	 Lay_NotEnoughMemoryExit ();

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
      Ale_ShowFixAlertAndButton1 (AlertType,Txt);
      Ale_ShowAlertAndButton2 (ActUnk,NULL,NULL,
			       NULL,NULL,
			       Btn_NO_BUTTON,NULL);
     }
  }

/*****************************************************************************/
/**************** Show the more recent alert with a button *******************/
/*****************************************************************************/

void Ale_ShowLastAlertAndButton (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit,
                                 void (*FuncParams) (void *Args),void *Args,
				 Btn_Button_t Button,const char *TxtButton)
  {
   /***** Show last alert and then reset it *****/
   Ale_ShowLastAlertAndButton1 ();

   /***** Show button *****/
   Ale_ShowAlertAndButton2 (NextAction,Anchor,OnSubmit,
                            FuncParams,Args,
                            Button,TxtButton);
  }

/*****************************************************************************/
/********** Show the first part of more recent alert with a button ***********/
/*****************************************************************************/

void Ale_ShowLastAlertAndButton1 (void)
  {
   /***** Show last alert *****/
   Ale_ShowFixAlertAndButton1 (Ale_GetTypeOfLastAlert (),Ale_GetTextOfLastAlert ());

   /***** Reset last alert *****/
   Ale_ResetLastAlert ();
  }

/*****************************************************************************/
/*********************** Show an alert with a button *************************/
/*****************************************************************************/

void Ale_ShowAlertAndButton (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit,
                             void (*FuncParams) (void *Args),void *Args,
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
      Lay_NotEnoughMemoryExit ();

   /****** Print fix alert and button ******/
   Ale_ShowFixAlertAndButton1 (AlertType,Txt);
   Ale_ShowAlertAndButton2 (NextAction,Anchor,OnSubmit,
                            FuncParams,Args,
                            Button,TxtButton);

   /***** Free text *****/
   free (Txt);
  }

/*****************************************************************************/
/******** Show the first part of a formatted-text alert with a button ********/
/*****************************************************************************/

void Ale_ShowAlertAndButton1 (Ale_AlertType_t AlertType,const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Txt;

   /***** Print format and list of variables into text *****/
   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Txt,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Lay_NotEnoughMemoryExit ();

   /****** Print start of fix alert and button ******/
   Ale_ShowFixAlertAndButton1 (AlertType,Txt);

   /***** Free text *****/
   free (Txt);
  }

/*****************************************************************************/
/*********** Show the first part of a fix-text alert with a button ***********/
/*****************************************************************************/

static void Ale_ShowFixAlertAndButton1 (Ale_AlertType_t AlertType,const char *Txt)
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
      [Ale_INFO     ] = "info-circle.svg",
      [Ale_SUCCESS  ] = "check-circle.svg",
      [Ale_QUESTION ] = "question64x64.gif",	// animated gif
      [Ale_WARNING  ] = "warning64x64.gif",	// animated gif
      [Ale_ERROR    ] = "error64x64.gif",	// animated gif
     };

   /****** If start of page is not written yet, do it now ******/
   if (!Gbl.Layout.HTMLStartWritten)
      Lay_WriteStartOfPage ();

   /***** Begin box *****/
   if (AlertClosable[AlertType])
     {
      /* Create unique id for alert */
      Frm_SetUniqueId (IdAlert);
      HTM_DIV_Begin ("id=\"%s\" class=\"CM\"",IdAlert);
     }
   else
      HTM_DIV_Begin ("class=\"CM\"");
   HTM_DIV_Begin ("class=\"ALERT\"");

   /***** Icon to close the alert *****/
   if (AlertClosable[AlertType])
     {
      HTM_DIV_Begin ("class=\"ALERT_CLOSE\"");
      HTM_A_Begin ("href=\"\" onclick=\"toggleDisplay('%s');return false;\" /",
	           IdAlert);
      Ico_PutIcon ("times.svg",Txt_Close,"ICO16x16");
      HTM_A_End ();
      HTM_DIV_End ();
     }

   /***** Write message *****/
   if (AlertType == Ale_NONE)
      HTM_DIV_Begin ("class=\"ALERT_TXT\"");
   else
      HTM_DIV_Begin ("class=\"ALERT_TXT\" style=\"background-image:url('%s/%s');\"",
	             Cfg_URL_ICON_PUBLIC,Ale_AlertIcons[AlertType]);
   HTM_Txt (Txt);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*************** Show the second part of an alert with a button **************/
/*****************************************************************************/

void Ale_ShowAlertAndButton2 (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit,
                              void (*FuncParams) (void *Args),void *Args,
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
	 if (FuncParams)
	    FuncParams (Args);

         /* Put button *****/
	 Btn_PutButton (Button,TxtButton);

         /* End form */
	 Frm_EndForm ();
	}

   /***** End box *****/
   HTM_DIV_End ();
   HTM_DIV_End ();
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
/*** Show alert when user not found or no permission to perform an action ****/
/*****************************************************************************/

void Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
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
/**************** Create alert when a form field is empty ********************/
/*****************************************************************************/

void Ale_ShowAlertYouCanNotLeaveFieldEmpty (void)
  {
   extern const char *Txt_You_can_not_leave_the_field_empty;

   Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_the_field_empty);
  }
