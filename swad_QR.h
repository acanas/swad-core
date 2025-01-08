// swad_QR.h: QR codes

#ifndef _SWAD_QR
#define _SWAD_QR
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_parameter_code.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void QR_PutLinkToPrintQRCode (Act_Action_t Action,
                              void (*FuncPars) (void *Args),void *Args);
void QR_PutParQRString (void *QRString);

void QR_PrintQRCode (void);
void QR_PrintUsrQRCode (void);
void QR_ImageQRCode (const char *QRString);

void QR_LinkTo (unsigned Size,ParCod_Param_t ParCode,long Cod);
void QR_ExamAnnnouncement (void);

#endif
