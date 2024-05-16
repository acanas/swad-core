// swad_attendance_database.h: control of attendance operations with database

#ifndef _SWAD_ATT_DB
#define _SWAD_ATT_DB
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

unsigned Att_DB_GetListEventsMyGrps (MYSQL_RES **mysql_res,
                                     Dat_StartEndTime_t SelectedOrder,
                                     Att_OrderNewestOldest_t OrderNewestOldest);
unsigned Att_DB_GetListEventsAllGrps (MYSQL_RES **mysql_res,
                                      Dat_StartEndTime_t SelectedOrder,
                                      Att_OrderNewestOldest_t OrderNewestOldest);
unsigned Att_DB_GetAllEventsData (MYSQL_RES **mysql_res,long CrsCod);
unsigned Att_DB_GetEventDataByCod (MYSQL_RES **mysql_res,long AttCod);
void Att_DB_GetEventTitle (long AttCod,char *Title,size_t TitleSize);
void Att_DB_GetEventDescription (long AttCod,char Description[Cns_MAX_BYTES_TEXT + 1]);

bool Att_DB_CheckIfSimilarEventExists (const char *Field,const char *Value,long AttCod);

long Att_DB_CreateEvent (const struct Att_Event *Event,const char *Description);
void Att_DB_UpdateEvent (const struct Att_Event *Event,const char *Description);
void Att_DB_HideOrUnhideEvent (long AttCod,
			       HidVis_HiddenOrVisible_t HiddenOrVisible);

void Att_DB_CreateGroup (long AttCod,long GrpCod);
unsigned Att_DB_GetGrpCodsAssociatedToEvent (MYSQL_RES **mysql_res,long AttCod);
unsigned Att_DB_GetGroupsAssociatedToEvent (MYSQL_RES **mysql_res,long AttCod);
void Att_DB_RemoveGroup (long GrpCod);
void Att_DB_RemoveGroupsOfType (long GrpTypCod);
void Att_DB_RemoveGrpsAssociatedToAnEvent (long AttCod);

unsigned Att_DB_GetNumStdsTotalWhoAreInEvent (long AttCod);
unsigned Att_DB_GetNumStdsFromListWhoAreInEvent (long AttCod,const char *SubQueryUsrs);
bool Att_DB_CheckIfUsrIsInTableAttUsr (long AttCod,long UsrCod,
				       Att_Present_t *Presente);
unsigned Att_DB_GetPresentAndComments (MYSQL_RES **mysql_res,long AttCod,long UsrCod);
unsigned Att_DB_GetListUsrsInEvent (MYSQL_RES **mysql_res,
                                    long AttCod,bool AttEventIsAsociatedToGrps);
void Att_DB_RegUsrInEventChangingComments (long AttCod,long UsrCod,
                                           Att_Present_t Present,
                                           const char *CommentStd,
                                           const char *CommentTch);
void Att_DB_SetUsrAsPresent (long AttCod,long UsrCod);
void Att_DB_SetUsrsAsPresent (long AttCod,const char *ListUsrs,bool SetOthersAsAbsent);
void Att_DB_RemoveUsrFromEvent (long AttCod,long UsrCod);
void Att_DB_RemoveUsrsAbsentWithoutCommentsFromEvent (long AttCod);

void Att_DB_RemoveAllUsrsFromAnEvent (long AttCod);
void Att_DB_RemoveUsrFromAllEvents (long UsrCod);
void Att_DB_RemoveUsrFromCrsEvents (long UsrCod,long CrsCod);
void Att_DB_RemoveEventFromCurrentCrs (long AttCod);

void Att_DB_RemoveUsrsFromCrsEvents (long HieCod);
void Att_DB_RemoveGrpsAssociatedToCrsEvents (long HieCod);
void Att_DB_RemoveCrsEvents (long HieCod);

unsigned Att_DB_GetNumEventsInCrs (long HieCod);
unsigned Att_DB_GetNumCoursesWithEvents (Hie_Level_t Level);

unsigned Att_DB_GetNumEvents (MYSQL_RES **mysql_res,Hie_Level_t Level);

#endif
