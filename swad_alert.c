// swad_alert.c: alerts

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

#define _GNU_SOURCE 		// For vasprintf
#include <linux/stddef.h>	// For NULL
#include <stdarg.h>		// For va_start, va_end
#include <stdio.h>		// For FILE, fprintf, vasprintf
#include <stdlib.h>		// For free

#include "swad_alert.h"
#include "swad_form.h"
#include "swad_global.h"

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

static const char *Ale_AlertIcons[Ale_NUM_ALERT_TYPES] =
  {
   NULL,		// Ale_NONE
   "clipboard.svg",	// Ale_CLIPBOARD
   "info-circle.svg",	// Ale_INFO
   "check-circle.svg",	// Ale_SUCCESS
   "question64x64.gif",	// Ale_QUESTION		animated gif
   "warning64x64.gif",	// Ale_WARNING		animated gif
   "error64x64.gif",	// Ale_ERROR		animated gif
  };

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

   if (Gbl.Alerts.Num + 1 > Ale_MAX_ALERTS)
      Lay_ShowErrorAndExit ("Too many alerts.");

   Gbl.Alerts.Num++;

   Gbl.Alerts.List[Gbl.Alerts.Num - 1].Type = Type;

   Gbl.Alerts.List[Gbl.Alerts.Num - 1].Section = NULL;
   if (Section)
      if (Section[0])
         Gbl.Alerts.List[Gbl.Alerts.Num - 1].Section = Section;

   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Gbl.Alerts.List[Gbl.Alerts.Num - 1].Text,fmt,ap);
   va_end (ap);

   if (NumBytesPrinted < 0)	// If memory allocation wasn't possible,
				// or some other error occurs,
				// vasprintf will return -1
      Lay_NotEnoughMemoryExit ();
  }

/*****************************************************************************/
/***************** Get current number of delayed alerts **********************/
/*****************************************************************************/

unsigned Ale_GetNumAlerts (void)
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
	 Gbl.Alerts.List[i].Section = NULL;

	 /***** Free memory allocated for text *****/
	 if (Gbl.Alerts.List[i].Text)
	   {
	    free ((void *) Gbl.Alerts.List[i].Text);
	    Gbl.Alerts.List[i].Text = NULL;
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

   for (i = 0;
	i < Gbl.Alerts.Num;
	i++)
      if (Gbl.Alerts.List[i].Type != Ale_NONE)
        {
	 ShowAlert = Section ? (Gbl.Alerts.List[i].Section == Section) :
		               true;

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
      if (NumBytesPrinted < 0)	// If memory allocation wasn't possible,
				// or some other error occurs,
				// vasprintf will return -1
	 Lay_NotEnoughMemoryExit ();

      /***** Show alert *****/
      Ale_ShowFixAlert (AlertType,Txt);

      /***** Free text *****/
      free ((void *) Txt);
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
			       NULL,Btn_NO_BUTTON,NULL);
     }
  }

/*****************************************************************************/
/**************** Show the more recent alert with a button *******************/
/*****************************************************************************/

void Ale_ShowLastAlertAndButton (Act_Action_t NextAction,const char *Anchor,
                                 const char *OnSubmit,void (*FuncParams) (),
				 Btn_Button_t Button,const char *TxtButton)
  {
   /***** Show last alert and then reset it *****/
   Ale_ShowLastAlertAndButton1 ();

   /***** Show button *****/
   Ale_ShowAlertAndButton2 (NextAction,Anchor,OnSubmit,
                            FuncParams,Button,TxtButton);
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

void Ale_ShowAlertAndButton (Act_Action_t NextAction,const char *Anchor,
                             const char *OnSubmit,void (*FuncParams) (),
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
   if (NumBytesPrinted < 0)	// If memory allocation wasn't possible,
				// or some other error occurs,
				// vasprintf will return -1
      Lay_NotEnoughMemoryExit ();

   /****** Print fix alert and button ******/
   Ale_ShowFixAlertAndButton1 (AlertType,Txt);
   Ale_ShowAlertAndButton2 (NextAction,Anchor,OnSubmit,
                            FuncParams,Button,TxtButton);

   /***** Free text *****/
   free ((void *) Txt);
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
   if (NumBytesPrinted < 0)	// If memory allocation wasn't possible,
				// or some other error occurs,
				// vasprintf will return -1
      Lay_NotEnoughMemoryExit ();

   /****** Print start of fix alert and button ******/
   Ale_ShowFixAlertAndButton1 (AlertType,Txt);

   /***** Free text *****/
   free ((void *) Txt);
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
      false,	// Ale_NONE
      true,	// Ale_CLIPBOARD
      true,	// Ale_INFO
      true,	// Ale_SUCCESS
      true,	// Ale_QUESTION
      true,	// Ale_WARNING
      true,	// Ale_ERROR
    };

   /****** If start of page is not written yet, do it now ******/
   if (!Gbl.Layout.HTMLStartWritten)
      Lay_WriteStartOfPage ();

   /***** Start box *****/
   fprintf (Gbl.F.Out,"<div");
   if (AlertClosable[AlertType])
     {
      /* Create unique id for alert */
      Frm_SetUniqueId (IdAlert);
      fprintf (Gbl.F.Out," id=\"%s\"",IdAlert);
     }
   fprintf (Gbl.F.Out," class=\"CENTER_MIDDLE\">"
	              "<div class=\"ALERT\">");

   /***** Icon to close the alert *****/
   if (AlertClosable[AlertType])
      fprintf (Gbl.F.Out,"<div class=\"ALERT_CLOSE\">"
			 "<a href=\"\""
			 " onclick=\"toggleDisplay('%s');return false;\" />"
			 "<img src=\"%s/close.svg\""
			 " alt=\"%s\" title=\"%s\""
			 " class=\"ICO16x16\" />"
			 "</a>"
			 "</div>",
	       IdAlert,
	       Gbl.Prefs.URLIcons,
	       Txt_Close,Txt_Close);

   /***** Write message *****/
   fprintf (Gbl.F.Out,"<div class=\"ALERT_TXT\"");
   if (AlertType != Ale_NONE)
      fprintf (Gbl.F.Out," style=\"background-image:url('%s/%s');\"",
	       Gbl.Prefs.URLIcons,Ale_AlertIcons[AlertType]);
   fprintf (Gbl.F.Out,">%s"
	              "</div>",
            Txt);
  }

/*****************************************************************************/
/*************** Show the second part of an alert with a button **************/
/*****************************************************************************/

void Ale_ShowAlertAndButton2 (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit,
                              void (*FuncParams) (),
                              Btn_Button_t Button,const char *TxtButton)
  {
   /***** Optional button *****/
   if (NextAction != ActUnk &&
       Button != Btn_NO_BUTTON &&
       TxtButton)
      if (TxtButton[0])
	{
         /* Start form */
         Frm_StartFormAnchorOnSubmit (NextAction,Anchor,OnSubmit);
	 if (FuncParams)
	    FuncParams ();

         /* Put button *****/
	 Btn_PutButton (Button,TxtButton);

         /* End form */
	 Frm_EndForm ();
	}

   /***** End box *****/
   fprintf (Gbl.F.Out,"</div>"
	              "</div>");
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
