// swad_attendance_database.h: control of attendance operations with database

#ifndef _SWAD_ATT_DB
#define _SWAD_ATT_DB
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include "swad_attendance.h"
#include "swad_date.h"
#include "swad_group.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

unsigned Att_DB_GetListAttEventsMyGrps (MYSQL_RES **mysql_res,
                                        Dat_StartEndTime_t SelectedOrder,
                                        Att_OrderNewestOldest_t OrderNewestOldest);
unsigned Att_DB_GetListAttEventsAllGrps (MYSQL_RES **mysql_res,
                                         Dat_StartEndTime_t SelectedOrder,
                                         Att_OrderNewestOldest_t OrderNewestOldest);
unsigned Att_DB_GetDataOfAllAttEvents (MYSQL_RES **mysql_res,long CrsCod);
unsigned Att_DB_GetDataOfAttEventByCod (MYSQL_RES **mysql_res,long AttCod);
void Att_DB_GetAttEventDescription (long AttCod,char Description[Cns_MAX_BYTES_TEXT + 1]);

bool Att_DB_CheckIfSimilarAttEventExists (const char *Field,const char *Value,long AttCod);

long Att_DB_CreateAttEvent (const struct Att_Event *Event,const char *Description);
void Att_DB_UpdateAttEvent (const struct Att_Event *Event,const char *Description);
void Att_DB_HideOrUnhideAttEvent (long AttCod,bool Hide);

void Att_DB_CreateGroup (long AttCod,long GrpCod);
unsigned Att_DB_GetGrpCodsAssociatedToEvent (MYSQL_RES **mysql_res,long AttCod);
unsigned Att_DB_GetGroupsAssociatedToEvent (MYSQL_RES **mysql_res,long AttCod);
void Att_DB_RemoveGroup (long GrpCod);
void Att_DB_RemoveGroupsOfType (long GrpTypCod);
void Att_DB_RemoveGrpsAssociatedToAnAttEvent (long AttCod);

unsigned Att_DB_GetNumStdsTotalWhoAreInAttEvent (long AttCod);
unsigned Att_DB_GetNumStdsFromListWhoAreInAttEvent (long AttCod,const char *SubQueryUsrs);
bool Att_DB_CheckIfUsrIsInTableAttUsr (long AttCod,long UsrCod,bool *Present);
unsigned Att_DB_GetPresentAndComments (MYSQL_RES **mysql_res,long AttCod,long UsrCod);
unsigned Att_DB_GetListUsrsInAttEvent (MYSQL_RES **mysql_res,
                                       long AttCod,bool AttEventIsAsociatedToGrps);
void Att_DB_RegUsrInAttEventChangingComments (long AttCod,long UsrCod,
                                              bool Present,
                                              const char *CommentStd,
                                              const char *CommentTch);
void Att_DB_SetUsrAsPresent (long AttCod,long UsrCod);
void Att_DB_SetUsrsAsPresent (long AttCod,const char *ListUsrs,bool SetOthersAsAbsent);
void Att_DB_RemoveUsrFromAttEvent (long AttCod,long UsrCod);
void Att_DB_RemoveUsrsAbsentWithoutCommentsFromAttEvent (long AttCod);

void Att_DB_RemoveAllUsrsFromAnAttEvent (long AttCod);
void Att_DB_RemoveUsrFromAllAttEvents (long UsrCod);
void Att_DB_RemoveUsrFromCrsAttEvents (long UsrCod,long CrsCod);
void Att_DB_RemoveAttEventFromCurrentCrs (long AttCod);

void Att_DB_RemoveUsrsFromCrsAttEvents (long CrsCod);
void Att_DB_RemoveGrpsAssociatedToCrsAttEvents (long CrsCod);
void Att_DB_RemoveCrsAttEvents (long CrsCod);

unsigned Att_DB_GetNumAttEventsInCrs (long CrsCod);
unsigned Att_DB_GetNumCoursesWithAttEvents (HieLvl_Level_t Scope);

unsigned Att_DB_GetNumAttEvents (MYSQL_RES **mysql_res,HieLvl_Level_t Scope);

#endif
