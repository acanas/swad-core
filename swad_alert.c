// swad_alert.c: alerts

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL

#include "swad_alert.h"
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
   "copy_on16x16.gif",	// Ale_CLIPBOARD
   "info64x64.png",	// Ale_INFO
   "success64x64.png",	// Ale_SUCCESS
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
/*********************** Show a write-pending alert **************************/
/*****************************************************************************/
// Gbl.Alert.Type must be Ale_NONE or any type of alert
// If Gbl.Alert.Type != Ale_NONE ==> Gbl.Alert.Txt must hold the message

void Ale_ShowPendingAlert (void)
  {
   /***** Anything to show? *****/
   if (Gbl.Alert.Type != Ale_NONE)
      /***** Show alert *****/
      Ale_ShowAlert (Gbl.Alert.Type,Gbl.Alert.Txt);

   // Do not be tempted to restore the value of Gbl.Alert.Type to Ale_NONE here,
   // since it can still be used after calling this function.
  }

/*****************************************************************************/
/******************** Show an alert message to the user **********************/
/*****************************************************************************/

void Ale_ShowAlert (Ale_AlertType_t AlertType,const char *Txt)
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
   char IdAlert[Act_MAX_BYTES_ID];
   static const bool AlertClosable[Ale_NUM_ALERT_TYPES] =
     {
      false,	// Ale_NONE
      false,	// Ale_CLIPBOARD
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
      Act_SetUniqueId (IdAlert);
      fprintf (Gbl.F.Out," id=\"%s\"",IdAlert);
     }
   fprintf (Gbl.F.Out," class=\"CENTER_MIDDLE\">"
	              "<div class=\"ALERT\">");

   /***** Icon to close the alert *****/
   if (AlertClosable[AlertType])
      fprintf (Gbl.F.Out,"<div class=\"ALERT_CLOSE\">"
			 "<a href=\"\""
			 " onclick=\"toggleDisplay('%s');return false;\" />"
			 "<img src=\"%s/close64x64.png\""
			 " alt=\"%s\" title=\"%s\""
			 " class=\"ICO20x20\" />"
			 "</a>"
			 "</div>",
	       IdAlert,
	       Gbl.Prefs.IconsURL,
	       Txt_Close,Txt_Close);

   /***** Write message *****/
   fprintf (Gbl.F.Out,"<div class=\"ALERT_TXT\"");
   if (AlertType != Ale_NONE)
      fprintf (Gbl.F.Out," style=\"background-image:url('%s/%s');\"",
	       Gbl.Prefs.IconsURL,Ale_AlertIcons[AlertType]);
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
	 Act_StartFormAnchor (NextAction,Anchor);
         Act_StartFormAnchorOnSubmit (NextAction,Anchor,OnSubmit);
	 if (FuncParams)
	    FuncParams ();

         /* Put button *****/
	 Btn_PutButton (Button,TxtButton);

         /* End form */
	 Act_EndForm ();
	}

   /***** End box *****/
   fprintf (Gbl.F.Out,"</div>"
	              "</div>");
  }
