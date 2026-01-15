// swad_timetable_resource.c: link to course timetable as program resource

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include "swad_alert.h"
#include "swad_resource_database.h"
#include "swad_timetable.h"

/*****************************************************************************/
/********************** Get link to course timetable *************************/
/*****************************************************************************/

void TmtRsc_GetLinkToCrsTimeTable (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_TIMETABLE_TYPES[Tmt_NUM_TIMETABLE_TYPES];

   /***** Copy link to course timetable into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_TIMETABLE,-1L);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Txt_TIMETABLE_TYPES[Tmt_COURSE_TIMETABLE]);

   /***** Show course timetable again *****/
   Tmt_ShowCrsTimeTable ();
  }
