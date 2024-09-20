// swad_user_clipboard.h: user clipboard

#ifndef _SWAD_USR_CLP
#define _SWAD_USR_CLP
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_form.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void UsrClp_CopyGstsToClipboard (void);
void UsrClp_CopyStdsToClipboard (void);
void UsrClp_CopyTchsToClipboard (void);

void UsrClp_ShowClipboardGsts (void);
void UsrClp_ShowClipboardStds (void);
void UsrClp_ShowClipboardTchs (void);

void UsrClp_ListUsrsInMyClipboard (Frm_PutForm_t PutForm,bool ShowWhenEmpty);

void UsrClp_PutIconToViewClipboardGsts (void);
void UsrClp_PutIconToViewClipboardStds (void);
void UsrClp_PutIconToViewClipboardTchs (void);

void UsrClp_RemoveClipboardGsts (void);
void UsrClp_RemoveClipboardStds (void);
void UsrClp_RemoveClipboardTchs (void);

#endif
