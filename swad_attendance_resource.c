// swad_attendance_resource.c: links to attendance events as program resources
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_attendance.h"
#include "swad_attendance_database.h"
#include "swad_attendance_resource.h"
#include "swad_error.h"
#include "swad_parameter_code.h"
#include "swad_resource_database.h"

/*****************************************************************************/
/************************ Get link to attendance event ***********************/
/*****************************************************************************/

void AttRsc_GetLinkToEvent (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_Control_of_class_attendance;
   struct Att_Event Event;
   __attribute__((unused)) Exi_Exist_t AttExists;

   /***** Get attendance event code *****/
   Event.AttCod = ParCod_GetPar (ParCod_Att);

   /***** Get attendance event data *****/
   if (Event.AttCod > 0)
      AttExists = Att_GetEventDataByCod (&Event);

   /***** Copy link to attendance event into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_ATT_EVENT,Event.AttCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
                  Event.AttCod > 0 ? Event.Title :
                	             Txt_Control_of_class_attendance);

   /***** Show attendance events again *****/
   Att_ShowEvents ();
  }
