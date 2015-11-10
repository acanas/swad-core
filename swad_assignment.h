// swad_assignment.h: assignments

#ifndef _SWAD_ASG
#define _SWAD_ASG
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#include "swad_date.h"
#include "swad_user.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Asg_MAX_LENGTH_ASSIGNMENT_TITLE	255

#define Asg_MAX_LENGTH_FOLDER 32

#define Asg_NUM_TYPES_SEND_WORK 2
typedef enum
  {
   Asg_DO_NOT_SEND_WORK = 0,
   Asg_SEND_WORK        = 1,
  } Asg_SendWork_t;

#define Asg_NUM_DATES 2
typedef enum
  {
   Asg_START_TIME = 0,
   Asg_END_TIME   = 1,
  } Asg_StartOrEndTime_t;

struct Assignment
  {
   long AsgCod;
   bool Hidden;
   long UsrCod;
   time_t TimeUTC[Asg_NUM_DATES];
   bool Open;
   char Title[Asg_MAX_LENGTH_ASSIGNMENT_TITLE+1];
   Asg_SendWork_t SendWork;
   char Folder[Asg_MAX_LENGTH_FOLDER+1];
   bool IBelongToCrsOrGrps;	// I can do this assignment
				// (it is associated to no groups
				// or, if associated to groups,
				// I belong to any of the groups)
  };

typedef enum
  {
   Asg_ORDER_BY_START_DATE = 0,
   Asg_ORDER_BY_END_DATE   = 1,
  } tAsgsOrderType;

#define Asg_DEFAULT_ORDER_TYPE Asg_ORDER_BY_START_DATE

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Asg_SeeAssignments (void);
void Asg_PutHiddenParamAsgOrderType (void);
void Asg_RequestCreatOrEditAsg (void);
void Asg_GetListAssignments (void);
void Asg_GetDataOfAssignmentByCod (struct Assignment *Asg);
void Asg_GetDataOfAssignmentByFolder (struct Assignment *Asg);
void Asg_FreeListAssignments (void);

void Asg_GetNotifAssignment (char *SummaryStr,char **ContentStr,long AsgCod,unsigned MaxChars,bool GetContent);

long Asg_GetParamAsgCod (void);
void Asg_AskRemAssignment (void);
void Asg_RemoveAssignment (void);
void Asg_HideAssignment (void);
void Asg_ShowAssignment (void);
void Asg_RecFormAssignment (void);
bool Asg_CheckIfAsgIsAssociatedToGrp (long AsgCod,long GrpCod);
void Asg_RemoveGroup (long GrpCod);
void Asg_RemoveGroupsOfType (long GrpTypCod);
void Asg_RemoveCrsAssignments (long CrsCod);
unsigned Asg_GetNumAssignmentsInCrs(long CrsCod);

unsigned Asg_GetNumCoursesWithAssignments (Sco_Scope_t Scope);
unsigned Asg_GetNumAssignments (Sco_Scope_t Scope,unsigned *NumNotif);

#endif
