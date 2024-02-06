// swad_room_database.h: classrooms, laboratories, offices or other rooms in a center, operations with database

#ifndef _SWAD_ROO_DB
#define _SWAD_ROO_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/***************************** Public prototypes *****************************/
/*****************************************************************************/

long Roo_DB_CreateRoom (long CtrCod,const struct Roo_Room *Room);
void Roo_DB_CreateMACAddress (long RooCod,unsigned long long MACnum);
void Roo_DB_UpdateMACAddress (long RooCod,unsigned long long NewMACnum);
void Roo_DB_UpdateRoomBuilding (long RooCod,long NewBldCod);
void Roo_DB_UpdateRoomFloor (long RooCod,int NewFloor);
void Roo_DB_UpdateRoomType (long RooCod,Roo_RoomType_t NewType);
void Roo_DB_UpdateRoomName (long RooCod,
                            const char *FldName,const char *NewRoomName);
void Roo_DB_UpdateRoomCapacity (long RooCod,unsigned NewCapacity);

unsigned Roo_DB_GetListRooms (MYSQL_RES **mysql_res,
                              long CtrCod,
                              Roo_WhichData_t WhichData,
                              Roo_Order_t SelectedOrder);
bool Roo_DB_CheckIfRoomNameExists (const char *FldName,const char *Name,
				   long Cod,long PrtCod,
				   __attribute__((unused)) unsigned Year);
unsigned Roo_DB_GetRoomDataByCod (MYSQL_RES **mysql_res,long RooCod);
unsigned Roo_DB_GetMACAddresses (MYSQL_RES **mysql_res,long RooCod);

void Roo_DB_RemoveRoom (long RooCod);
void Roo_DB_RemoveMACAddress (long RooCod,unsigned long long MACnum);
void Roo_DB_RemoveBuildingFromRooms (long BldCod);
void Roo_DB_RemoveAllRoomsInCtr (long CtrCod);

//--------------------------------- Check in ----------------------------------
long Roo_DB_CheckIn (long RooCod);

bool Roo_DB_CheckIfICanSeeUsrLocation (long UsrCod);
unsigned Roo_DB_GetUsrLastLocation (MYSQL_RES **mysql_res,long UsrCod);
unsigned Roo_DB_GetLocationByMAC (MYSQL_RES **mysql_res,unsigned long long MACnum);

#endif
