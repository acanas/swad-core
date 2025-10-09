// swad_assignment.h: assignments

#ifndef _SWAD_ASG
#define _SWAD_ASG
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Ca�as Vargas

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

#include "swad_browser.h"
#include "swad_date.h"
#include "swad_notification.h"
#include "swad_user.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Asg_MAX_CHARS_ASSIGNMENT_TITLE	(128 - 1)	// 127
#define Asg_MAX_BYTES_ASSIGNMENT_TITLE	((Asg_MAX_CHARS_ASSIGNMENT_TITLE + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Asg_NUM_TYPES_SEND_WORK 2
typedef enum
  {
   Asg_DONT_SEND_WORK = 0,
   Asg_SEND_WORK      = 1,
  } Asg_SendWork_t;

struct Asg_Assignment
  {
   long AsgCod;
   HidVis_HiddenOrVisible_t Hidden;
   long UsrCod;
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   CloOpe_ClosedOrOpen_t ClosedOrOpen;
   char Title[Asg_MAX_BYTES_ASSIGNMENT_TITLE + 1];
   Asg_SendWork_t SendWork;
   char Folder[Brw_MAX_BYTES_FOLDER + 1];
   long RubCod;
   Usr_Can_t ICanDo;	// I can do this assignment
			// (it is associated to no groups
			// or, if associated to groups,
			// I belong to any of the groups)
  };

struct Asg_Assignments
  {
   bool LstIsRead;		// Is the list already read from database...
				// ...or it needs to be read?
   unsigned Num;		// Number of assignments
   long *LstAsgCods;		// List of assigment codes
   Dat_StartEndTime_t SelectedOrder;
   unsigned CurrentPage;
   struct Asg_Assignment Asg;	// Selected/current assignment
  };

#define Asg_ORDER_DEFAULT Dat_STR_TIME

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Asg_SetAssignment (const struct Asg_Assignment *Asg);
const struct Asg_Assignment *Asg_GetAssigment (void);

void Asg_ResetAssignments (struct Asg_Assignments *Assignments);

void Asg_SeeAssignments (void);
void Asg_ShowAllAssignments (struct Asg_Assignments *Assignments);
void Asg_SeeOneAssignment (void);
void Asg_PrintOneAssignment (void);

void Asg_ShowOneAssignmentInBox (struct Asg_Assignments *Assignments);

Dat_StartEndTime_t Asg_GetParAsgOrder (void);

void Asg_ReqCreatOrEditAsg (void);
void Asg_GetAssignmentDataByCod (struct Asg_Assignment *Asg);
void Asg_GetAssignmentDataByFolder (const char Folder[Brw_MAX_BYTES_FOLDER + 1]);

void Asg_GetNotifAssignment (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                             char **ContentStr,
                             long AsgCod,Ntf_GetContent_t GetContent);

void Asg_ReqRemAssignment (void);
void Asg_RemoveAssignment (void);
void Asg_HideAssignment (void);
void Asg_UnhideAssignment (void);
void Asg_ReceiveAssignment (void);
void Asg_RemoveCrsAssignments (long HieCod);

void Asg_WriteDatesAssignment (void);
Usr_Can_t Asg_CheckIfICanCreateIntoAssigment (void);
void Asg_SetFolder (unsigned Level,char Folder[Brw_MAX_BYTES_FOLDER + 1]);

unsigned Asg_GetNumAssignments (Hie_Level_t HieLvl,unsigned *NumNotif);

//-------------------------------- Figures ------------------------------------
void Asg_GetAndShowAssignmentsStats (Hie_Level_t HieLvl);

#endif
