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

/*****************************************************************************/
/******************************** Reset alert ********************************/
/*****************************************************************************/

void Ale_ResetAlert (void)
  {
   Gbl.Alert.Type = Ale_NONE;	// Reset alert
   Gbl.Alert.Section = NULL;
   Gbl.Alert.Txt[0] = '\0';
  }

/*****************************************************************************/
/*********************** Show a write-pending alert **************************/
/*****************************************************************************/
// Gbl.Alert.Type must be Ale_NONE or any type of alert
// If Gbl.Alert.Type != Ale_NONE ==> Gbl.Alert.Txt must hold the message

void Ale_ShowPendingAlert (void)
  {
   /***** Anything to show? *****/
   if (Gbl.Alert.Type != Ale_NONE)
      /***** Show alert *****/
      Ale_ShowA_old (Gbl.Alert.Type,Gbl.Alert.Txt);

   Ale_ResetAlert ();
  }

/*****************************************************************************/
/******************** Show an alert message to the user **********************/
/*****************************************************************************/

void Ale_ShowA_fmt (Ale_AlertType_t AlertType,
                    const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Txt;

   if (AlertType != Ale_NONE)
     {
      va_start (ap,fmt);
      NumBytesPrinted = vasprintf (&Txt,fmt,ap);
      va_end (ap);

      if (NumBytesPrinted < 0)	// If memory allocation wasn't possible,
				   // or some other error occurs,
				   // vasprintf will return -1
	 Lay_NotEnoughMemoryExit ();

      Ale_ShowAlertAndButton (AlertType,Txt,
                              ActUnk,NULL,NULL,NULL,Btn_NO_BUTTON,NULL);

      free ((void *) Txt);
     }
  }

void Ale_ShowA_new (Ale_AlertType_t AlertType,const char *Txt)
  {
   if (AlertType != Ale_NONE)
      Ale_ShowAlertAndButton (AlertType,Txt,
                              ActUnk,NULL,NULL,NULL,Btn_NO_BUTTON,NULL);
  }

void Ale_ShowA_old (Ale_AlertType_t AlertType,const char *Txt)
  {
   if (AlertType != Ale_NONE)
      Ale_ShowAlertAndButton (AlertType,Txt,
                              ActUnk,NULL,NULL,NULL,Btn_NO_BUTTON,NULL);
  }

void Ale_ShowAlertAndButton (Ale_AlertType_t AlertType,const char *Txt,
                             Act_Action_t NextAction,const char *Anchor,const char *OnSubmit,
                             void (*FuncParams) (),
                             Btn_Button_t Button,const char *TxtButton)
  {
   Ale_ShowAlertAndButton1 (AlertType,Txt);
   Ale_ShowAlertAndButton2 (NextAction,Anchor,OnSubmit,
                            FuncParams,Button,TxtButton);
  }

void Ale_ShowAlertAndButton1 (Ale_AlertType_t AlertType,const char *Txt)
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
   fprintf (Gbl.F.Out,">");
   if (Txt)
      fprintf (Gbl.F.Out,"%s",Txt);
   fprintf (Gbl.F.Out,"</div>");
  }

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
	 Frm_StartFormAnchor (NextAction,Anchor);
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
