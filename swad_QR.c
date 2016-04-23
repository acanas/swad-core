// swad_QR.c: QR codes

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#include <string.h>	// For strncpy...

#include "swad_action.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_parameter.h"
#include "swad_QR.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal constants ****************************/
/*****************************************************************************/

#define QR_CODE_SIZE	((6+25+6)*8)
#define QR_DEFAULT_TYPE QR_ID

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

static void QR_PutParamQRString (void);

static void QR_ImageQRCode (const char *QRString);

/*****************************************************************************/
/***************** Put a link to a print view of a QR code *******************/
/*****************************************************************************/

void QR_PutLinkToPrintQRCode (const char *Nickname,bool PrintText)
  {
   extern const char *Txt_QR_code;

   /***** Link to print QR *****/
   Gbl.Usrs.NicknameForQR = Nickname;
   Lay_PutContextualLink (ActPrnUsrQR,QR_PutParamQRString,"qr64x64.gif",
                          Txt_QR_code,PrintText ? Txt_QR_code :
	                                          NULL);
  }

/*****************************************************************************/
/************************* Put parameter QR string ***************************/
/*****************************************************************************/

static void QR_PutParamQRString (void)
  {
   char NicknameWithArroba[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA+1];

   sprintf (NicknameWithArroba,"@%s",Gbl.Usrs.NicknameForQR);
   Par_PutHiddenParamString ("QRString",NicknameWithArroba);
  }

/*****************************************************************************/
/******************************* Show a QR code ******************************/
/*****************************************************************************/

void QR_PrintQRCode (void)
  {
   char QRString[Cns_MAX_BYTES_STRING+1];

   /***** Get QR string *****/
   Par_GetParToText ("QRString",QRString,Cns_MAX_BYTES_STRING);

   /***** Show QR code *****/
   QR_ImageQRCode (QRString);
  }

/*****************************************************************************/
/******************** Write an QR (image) based on a string ******************/
/*****************************************************************************/

static void QR_ImageQRCode (const char *QRString)
  {
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\" style=\"width:%upx;\">"
                      "<img src=\"https://chart.googleapis.com/chart?cht=qr&amp;chs=%ux%u&amp;chl=%s\""
                      " alt=\"%s\" title=\"%s\""
                      " style=\"width:%upx; height:%upx;"
                      " border:1px dashed silver;\" />"
                      "<br />"
                      "<span class=\"DAT\">%s</span>"
                      "</div>",
            QR_CODE_SIZE,
            QR_CODE_SIZE,QR_CODE_SIZE,
            QRString,
            QRString,
            QRString,
            QR_CODE_SIZE,QR_CODE_SIZE,
            QRString);
  }

/*****************************************************************************/
/*************** Show QR code with direct link (shortcut URL) ****************/
/*****************************************************************************/

void QR_LinkTo (unsigned Size,const char *ParamStr,long Cod)
  {
   extern const char *Txt_Shortcut;
   extern const char *Txt_STR_LANG_ID[1+Txt_NUM_LANGUAGES];

   /***** Show QR code with direct link to the current centre *****/
   fprintf (Gbl.F.Out,"<img src=\"https://chart.googleapis.com/chart?cht=qr&amp;chs=%ux%u&amp;chl=%s/%s?%s=%ld\""
                      " alt=\"%s\" title=\"%s\""
                      " style=\"width:%upx; height:%upx;\" />",
            Size,Size,
            Cfg_HTTPS_URL_SWAD_CGI,
            Txt_STR_LANG_ID[Gbl.Prefs.Language],ParamStr,Cod,
            Txt_Shortcut,
            Txt_Shortcut,
            Size,Size);
  }

/*****************************************************************************/
/*********** Show QR code with direct link to an exam announcement ***********/
/*****************************************************************************/

void QR_ExamAnnnouncement (void)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   extern const char *Txt_Link_to_announcement_of_exam;

   /***** Show QR code with direct link to the exam announcement *****/
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">"
                      "<img src=\"https://chart.googleapis.com/chart?cht=qr&amp;chs=%ux%u&amp;chl=%s/?crs=%ld%%26act=%ld\""
                      " alt=\"%s\" title=\"%s\""
                      " style=\"width:250px; height:250px;\" />"
                      "</div>",
            300,300,
            Cfg_HTTPS_URL_SWAD_CGI,Gbl.CurrentCrs.Crs.CrsCod,Act_Actions[ActSeeExaAnn].ActCod,
            Txt_Link_to_announcement_of_exam,
            Txt_Link_to_announcement_of_exam);
  }
