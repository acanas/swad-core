// swad_rubric_resource.h: links to rubrics as resources

#ifndef _SWAD_RUB_RSC
#define _SWAD_RUB_RSC
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Ca�as Vargas

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <stddef.h>		// For size_t

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void RubRsc_GetLinkToRubric (void);

void RubRsc_ViewResourceClipboard (void);
void RubRsc_RemoveResourceClipboard (void);

#endif
