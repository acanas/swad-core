// swad_attendance_resource.c: links to attendance events as program resources
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2022 Antonio Cañas Vargas

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

#include "swad_alert.h"
#include "swad_attendance.h"
#include "swad_attendance_database.h"
#include "swad_attendance_resource.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_program_database.h"

/*****************************************************************************/
/************************ Get link to attendance event ***********************/
/*****************************************************************************/

void AttRsc_GetLinkToEvent (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   long AttCod;
   char Title[Att_MAX_BYTES_ATTENDANCE_EVENT_TITLE + 1];

   /***** Get attendance event code *****/
   if ((AttCod = Att_GetParamAttCod ()) < 0)
      Err_WrongEventExit ();

   /***** Get attendance event title *****/
   AttRsc_GetTitleFromAttCod (AttCod,Title,sizeof (Title) - 1);

   /***** Copy link to attendance event into resource clipboard *****/
   Prg_DB_CopyToClipboard (PrgRsc_ATTENDANCE_EVENT,AttCod);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Title);

   /***** Show attendance events again *****/
   Att_SeeAttEvents ();
  }

/*****************************************************************************/
/**************** Write attendance event in course program *******************/
/*****************************************************************************/

void AttRsc_WriteAttEventInCrsProgram (long AttCod,bool PutFormToGo,
                                       const char *Icon,const char *IconTitle)
  {
   extern const char *Txt_Actions[Act_NUM_ACTIONS];
   Act_Action_t NextAction;
   char Title[Att_MAX_BYTES_ATTENDANCE_EVENT_TITLE + 1];

   /***** Get game title *****/
   AttRsc_GetTitleFromAttCod (AttCod,Title,sizeof (Title) - 1);

   /***** Begin form to go to game *****/
   if (PutFormToGo)
     {
      NextAction = (AttCod > 0)	? ActSeeOneAtt :	// Attendance events specified
				  ActSeeAtt;		// All attendance events
      Frm_BeginForm (NextAction);
	 Att_PutParamAttCod (AttCod);
	 Att_PutParamsCodGrps (AttCod);
	 HTM_BUTTON_Submit_Begin (Txt_Actions[NextAction],
	                          "class=\"LM BT_LINK PRG_LNK_%s\"",
	                          The_GetSuffix ());
     }

   /***** Icon depending on type ******/
   if (PutFormToGo)
      Ico_PutIconLink (Icon,Ico_BLACK,NextAction);
   else
      Ico_PutIconOn (Icon,Ico_BLACK,IconTitle);

   /***** Write attendance event title *****/
   HTM_Txt (Title);

   /***** End form to download file *****/
   if (PutFormToGo)
     {
      /* End form */
         HTM_BUTTON_End ();

      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/*************** Get attendance event title from game code *******************/
/*****************************************************************************/

void AttRsc_GetTitleFromAttCod (long AttCod,char *Title,size_t TitleSize)
  {
   extern const char *Txt_Control_of_class_attendance;
   char TitleFromDB[Att_MAX_BYTES_ATTENDANCE_EVENT_TITLE + 1];

   if (AttCod > 0)
     {
      /***** Get attendance event title *****/
      Att_DB_GetAttEventTitle (AttCod,TitleFromDB);
      Str_Copy (Title,TitleFromDB,TitleSize);
     }
   else
      /***** Generic title for all attendance events *****/
      Str_Copy (Title,Txt_Control_of_class_attendance,TitleSize);
  }
