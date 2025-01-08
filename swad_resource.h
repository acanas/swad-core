// swad_resource.h: resources for course program and rubrics

#ifndef _SWAD_RSC
#define _SWAD_RSC
/*
    SWAD (Shared Workspace At a Distance),
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_form.h"
#include "swad_HTML.h"
#include "swad_resource_type.h"
#include "swad_string.h"
#include "swad_tree.h"
#include "swad_user.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Rsc_ResetSpcFields (struct Tre_Node *Node);

void Rsc_PutIconToViewClipboard (void);
void Rsc_ShowClipboard (void);
void Rsc_ShowClipboardToChangeLink (const struct Rsc_Link *CurrentLink,
				    HTM_Attributes_t Attributes);
void Rsc_WriteLinkName (const struct Rsc_Link *Link,Frm_PutForm_t PutFormToGo);

void Rsc_GetLinkDataFromRow (MYSQL_RES *mysql_res,struct Rsc_Link *Link);
Rsc_Type_t Rsc_GetTypeFromString (const char *Str);

bool Rsc_GetParLink (struct Rsc_Link *Link);

Usr_Can_t Rsc_CheckIfICanGetLink (void);

#endif
