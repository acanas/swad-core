// swad_assignment_database.h: assignments operations with database

#ifndef _SWAD_ASG_DB
#define _SWAD_ASG_DB
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#include "swad_assignment.h"
#include "swad_browser.h"
#include "swad_group.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

unsigned Asg_DB_GetListAssignmentsMyGrps (MYSQL_RES **mysql_res,
                                          Dat_StartEndTime_t SelectedOrder);
unsigned Asg_DB_GetListAssignmentsAllGrps (MYSQL_RES **mysql_res,
                                           Dat_StartEndTime_t SelectedOrder);

unsigned Asg_DB_GetAssignmentDataByCod (MYSQL_RES **mysql_res,long AsgCod);
unsigned Asg_DB_GetAssignmentDataByFolder (MYSQL_RES **mysql_res,
                                           const char Folder[Brw_MAX_BYTES_FOLDER + 1]);
unsigned Asg_DB_GetAssignmentTitleAndTxtByCod (MYSQL_RES **mysql_res,long AsgCod);
void Asg_DB_GetAssignmentTitleByCod (long AsgCod,char *Title,size_t TitleSize);
void Asg_DB_GetAssignmentTxtByCod (long AsgCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);

bool Asg_DB_CheckIfSimilarAssignmentExists (const char *Field,const char *Value,
                                            long AsgCod);

long Asg_DB_CreateAssignment (const struct Asg_Assignment *Asg,const char *Txt);
void Asg_DB_UpdateAssignment (const struct Asg_Assignment *Asg,const char *Txt);
void Asg_DB_HideOrUnhideAssignment (long AsgCod,bool Hide);

void Asg_DB_RemoveAssignment (long AsgCod);

bool Asg_DB_CheckIfICanDoAssignment (long AsgCod);

unsigned Asg_DB_GetGrps (MYSQL_RES **mysql_res,long AsgCod);
void Asg_DB_CreateGroup (long AsgCod,long GrpCod);
void Asg_DB_RemoveGroup (long GrpCod);
void Asg_DB_RemoveGroupsOfType (long GrpTypCod);
void Asg_DB_RemoveGrpsAssociatedToAnAssignment (long AsgCod);

void Asg_DB_RemoveGrpsAssociatedToAsgsInCrs (long CrsCod);
void Asg_DB_RemoveCrsAssignments (long CrsCod);

void Asg_DB_UpdateNumUsrsNotifiedByEMailAboutAssignment (long AsgCod,
                                                         unsigned NumUsrsToBeNotifiedByEMail);
unsigned Asg_DB_GetUsrsFromAssignmentExceptMe (MYSQL_RES **mysql_res,long AsgCod);

unsigned Asg_DB_GetNumCoursesWithAssignments (HieLvl_Level_t Level);

unsigned Asg_DB_GetNumAssignments (MYSQL_RES **mysql_res,HieLvl_Level_t Level);

unsigned Asg_DB_GetNumAssignmentsInCrs (long CrsCod);

#endif
