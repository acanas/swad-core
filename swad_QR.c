// swad_QR.c: QR codes

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_parameter.h"
#include "swad_QR.h"

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define QR_CODE_SIZE	((6 + 25 + 6) * 8)
#define QR_DEFAULT_TYPE QR_ID

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************** Put a link to a print view of a QR code *******************/
/*****************************************************************************/

void QR_PutLinkToPrintQRCode (Act_Action_t Action,
                              void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (Action,NULL,
                                  FuncPars,Args,
				  "qrcode.svg",Ico_BLACK);
  }

/*****************************************************************************/
/************************* Put parameter QR string ***************************/
/*****************************************************************************/

void QR_PutParQRString (void *QRString)
  {
   Par_PutParString (NULL,"QRString",QRString);
  }

/*****************************************************************************/
/******************************* Show a QR code ******************************/
/*****************************************************************************/

void QR_PrintQRCode (void)
  {
   char QRString[WWW_MAX_BYTES_WWW + 1];

   /***** Get QR string *****/
   Par_GetParText ("QRString",QRString,WWW_MAX_BYTES_WWW);

   /***** Show QR code *****/
   QR_ImageQRCode (QRString);
  }

/*****************************************************************************/
/**************************** Show a user QR code ****************************/
/*****************************************************************************/

void QR_PrintUsrQRCode (void)
  {
   char NewNickWithArr[Nck_MAX_BYTES_NICK_WITH_ARROBA + 1];

   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
     {
      /***** Begin box *****/
      Box_BoxBegin (Gbl.Usrs.Other.UsrDat.FullName,NULL,NULL,
                    NULL,Box_NOT_CLOSABLE);

	 /***** Show QR code *****/
	 if (Gbl.Usrs.Other.UsrDat.Nickname[0])
	   {
	    snprintf (NewNickWithArr,sizeof (NewNickWithArr),"@%s",
		      Gbl.Usrs.Other.UsrDat.Nickname);
	    QR_ImageQRCode (NewNickWithArr);
	   }

      /***** End box *****/
      Box_BoxEnd ();
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
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
	 Err_NotEnoughMemoryExit ();
      HTM_IMG (URL,NULL,QRString,
	       "style=\"width:%upx;height:%upx;border:1px dashed silver;\"",
	       QR_CODE_SIZE,QR_CODE_SIZE);
      free (URL);

   HTM_DIV_End ();
  }

/*****************************************************************************/
/*************** Show QR code with direct link (shortcut URL) ****************/
/*****************************************************************************/

void QR_LinkTo (unsigned Size,ParCod_Param_t ParCode,long Cod)
  {
   extern const char *Par_CodeStr[Par_NUM_PAR_COD];
   extern const char *Txt_Shortcut;
   char *URL;

   /***** Show QR code with link *****/
   if (ParCode == ParCod_None)
     {
      if (asprintf (&URL,"https://chart.googleapis.com/"
	                 "chart?cht=qr&amp;chs=%ux%u&amp;chl=%s/",
		    Size,Size,Cfg_URL_SWAD_CGI) < 0)
	 Err_NotEnoughMemoryExit ();
     }
   else
     {
      if (asprintf (&URL,"https://chart.googleapis.com/"
	                 "chart?cht=qr&amp;chs=%ux%u&amp;chl=%s/?%s=%ld",
		    Size,Size,Cfg_URL_SWAD_CGI,Par_CodeStr[ParCode],Cod) < 0)
	 Err_NotEnoughMemoryExit ();
     }

   HTM_IMG (URL,NULL,Txt_Shortcut,"style=\"width:%upx;height:%upx;\"",Size,Size);
   free (URL);
  }

/*****************************************************************************/
/*********** Show QR code with direct link to an exam announcement ***********/
/*****************************************************************************/

void QR_ExamAnnnouncement (void)
  {
   extern const char *Txt_Link_to_call_for_exam;
   char *URL;

   /***** Show QR code with direct link to the exam announcement *****/
   HTM_DIV_Begin ("class=\"CM\"");

      if (asprintf (&URL,"https://chart.googleapis.com/chart?cht=qr&amp;chs=%ux%u&amp;chl=%s/?crs=%ld%%26act=%ld",
		    300,300,
		    Cfg_URL_SWAD_CGI,Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		    Act_GetActCod (ActSeeAllCfe)) < 0)
	 Err_NotEnoughMemoryExit ();
      HTM_IMG (URL,NULL,Txt_Link_to_call_for_exam,
	       "style=\"width:250px;height:250px;\"");
      free (URL);

   HTM_DIV_End ();
  }
