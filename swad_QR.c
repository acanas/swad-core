// swad_QR.c: QR codes

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_action.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_parameter.h"
#include "swad_QR.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define QR_CODE_SIZE	((6 + 25 + 6) * 8)
#define QR_DEFAULT_TYPE QR_ID

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************** Private global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************** Put a link to a print view of a QR code *******************/
/*****************************************************************************/

void QR_PutLinkToPrintQRCode (Act_Action_t Action,void (*FuncParams) (void))
  {
   extern const char *Txt_QR_code;

   /***** Link to print QR *****/
   Lay_PutContextualLinkOnlyIcon (Action,NULL,FuncParams,
				  "qrcode.svg",
				  Txt_QR_code);
  }

/*****************************************************************************/
/************************* Put parameter QR string ***************************/
/*****************************************************************************/

void QR_PutParamQRString (void)
  {
   Par_PutHiddenParamString (NULL,"QRString",Gbl.QR.Str);
  }

/*****************************************************************************/
/******************************* Show a QR code ******************************/
/*****************************************************************************/

void QR_PrintQRCode (void)
  {
   char QRString[Cns_MAX_BYTES_WWW + 1];

   /***** Get QR string *****/
   Par_GetParToText ("QRString",QRString,Cns_MAX_BYTES_WWW);

   /***** Show QR code *****/
   QR_ImageQRCode (QRString);
  }

/*****************************************************************************/
/******************** Write an QR (image) based on a string ******************/
/*****************************************************************************/

void QR_ImageQRCode (const char *QRString)
  {
   char *URL;

   HTM_DIV_Begin ("class=\"CM\" style=\"margin:0 auto; width:%upx;\"",
		  QR_CODE_SIZE);

   if (asprintf (&URL,"https://chart.googleapis.com/chart?cht=qr&amp;chs=%ux%u&amp;chl=%s",
		 QR_CODE_SIZE,QR_CODE_SIZE,QRString) < 0)
      Lay_NotEnoughMemoryExit ();
   HTM_IMG (URL,NULL,QRString,
	    "style=\"width:%upx;height:%upx;border:1px dashed silver;\"",
	    QR_CODE_SIZE,QR_CODE_SIZE);
   free (URL);

   HTM_DIV_End ();
  }

/*****************************************************************************/
/*************** Show QR code with direct link (shortcut URL) ****************/
/*****************************************************************************/

void QR_LinkTo (unsigned Size,const char *ParamName,long Cod)
  {
   extern const char *Txt_Shortcut;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   char *URL;

   /***** Show QR code with direct link to the current centre *****/
   if (asprintf (&URL,"https://chart.googleapis.com/chart?cht=qr&amp;chs=%ux%u&amp;chl=%s/%s?%s=%ld",
		 Size,Size,
                 Cfg_URL_SWAD_CGI,
                 Lan_STR_LANG_ID[Gbl.Prefs.Language],ParamName,Cod) < 0)
      Lay_NotEnoughMemoryExit ();
   HTM_IMG (URL,NULL,Txt_Shortcut,
	    "style=\"width:%upx;height:%upx;\"",Size,Size);
   free (URL);
  }

/*****************************************************************************/
/*********** Show QR code with direct link to an exam announcement ***********/
/*****************************************************************************/

void QR_ExamAnnnouncement (void)
  {
   extern const char *Txt_Link_to_announcement_of_exam;
   char *URL;

   /***** Show QR code with direct link to the exam announcement *****/
   HTM_DIV_Begin ("class=\"CM\"");

   if (asprintf (&URL,"https://chart.googleapis.com/chart?cht=qr&amp;chs=%ux%u&amp;chl=%s/?crs=%ld%%26act=%ld",
		 300,300,
                 Cfg_URL_SWAD_CGI,Gbl.Hierarchy.Crs.CrsCod,
		 Act_GetActCod (ActSeeAllExaAnn)) < 0)
      Lay_NotEnoughMemoryExit ();
   HTM_IMG (URL,NULL,Txt_Link_to_announcement_of_exam,
	    "style=\"width:250px;height:250px;\"");
   free (URL);

   HTM_DIV_End ();
  }
