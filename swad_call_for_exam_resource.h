// swad_call_for_exam_resource.h: links to calls for exams as program resources

#ifndef _SWAD_CFE_RSC
#define _SWAD_CFE_RSC
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_form.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void CfeRsc_GetLinkToCallForExam (void);
void CfeRsc_GetCallForExamTitle (long ExaCod,char *Title,size_t TitleSize);

#endif
