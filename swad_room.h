// swad_room.h: classrooms, laboratories, offices or other rooms in a centre

#ifndef _SWAD_ROO
#define _SWAD_ROO
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

#include "swad_building.h"
#include "swad_string.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Roo_MAX_CHARS_SHRT_NAME	(32 - 1)	// 31
#define Roo_MAX_BYTES_SHRT_NAME	((Roo_MAX_CHARS_SHRT_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 511

#define Roo_MAX_CHARS_FULL_NAME	(128 - 1)	// 127
#define Roo_MAX_BYTES_FULL_NAME	((Roo_MAX_CHARS_FULL_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Roo_MAX_CAPACITY	10000	// If capacity of a room is greater than this, it is considered infinite
#define Roo_UNLIMITED_CAPACITY	INT_MAX	// This number can be stored in database as an integer...
					// ...and means that a room has no limited capacity

typedef enum
  {
   Roo_ADMINISTRATION =  0,	// "user-tie.svg"					// "Administración"
   Roo_AUDITORIUM     =  1,	// "landmark.svg" "university.svg" "theater-masks.svg"	// "Auditorio"
   Roo_CAFETERIA      =  2,	// "coffee.svg" "mug-hot.svg"				// "Cafetería"
   Roo_CANTEEN        =  3,	// "utensils.svg"					// "Comedor"
   Roo_CLASSROOM      =  4,	// "chalkboard-teacher.svg" "chalkboard.svg"		// "Aula"
   Roo_CONCIERGE      =  5,	// "concierge-bell" "user-cog.svg" "house-user.svg" 	// "Conserjería/Portería"
   Roo_CORRIDOR       =  6,	// "walking.svg" "map-signs.svg" "door-open.svg"	// "Pasillo"
   Roo_GYM            =  7,	// "dumbbell.svg"					// "Gimnasio"
   Roo_HALL           =  8,	// "map-signs.svg" "archway.svg" "hotel.svg" "concierge-bell.svg" "door-open.svg" "street-view.svg"	// "Vestíbulo"
   Roo_KINDERGARTEN   =  9,	// "child.svg" "baby.svg" "baby-carriage.svg"		// "Guardería"
   Roo_LABORATORY     = 10,	// "flask.svg" "microscope.svg"				// "Laboratorio"
   Roo_LIBRARY        = 11,	// "book.svg" "book-reader.svg"				// "Biblioteca"
   Roo_OFFICE         = 12,	// "house-user.svg" "door-open.svg"			// "Oficina/Despacho"
   Roo_OUTDOORS       = 13,	// "cloud-sun.svg"					// "Exteriores"
   Roo_PARKING        = 14,	// "car.svg"						// "Parking"
   Roo_PAVILION       = 15,	// "volleyball-ball.svg" "running.svg"			// "Pabellón"
   Roo_ROOM           = 16,	//							// "Sala"
   Roo_SECRETARIAT    = 17,	// "stamp.svg" "pen-alt.svg" "file-alt.svg" "user-tie.svg"	// "Secretaría"
   Roo_SEMINAR        = 18,	// "chalkboard-teacher.svg" "chalkboard.svg"		// "Seminario"
   Roo_SHOP           = 19,	// "shopping-cart.svg" "shopping-basket.svg" "store-alt.svg" shopping-bag	// "Tienda"
   Roo_STORE          = 20,	// "daily.svg" "boxes.svg" "box-open.svg" "box.svg"	// "Almacén"
   Roo_TOILETS        = 21,	// "toilet.svg"						// "Aseos"
   Roo_VIRTUAL        = 22,	// "laptop-house.svg" "cloud.svg"			// "Virtual"
   Roo_YARD           = 23,	// "canadian-mapple-leaf.svg"				// "Patio/Jardín"
  } Roo_RoomType_t;

struct Roo_Room
  {
   long RooCod;						// Room code
   long CtrCod;						// Centre code
   long BldCod;						// Building code
   char BldShrtName[Bld_MAX_BYTES_SHRT_NAME + 1];	// Building short name
   int  Floor;						// Room floor
   char ShrtName[Roo_MAX_BYTES_SHRT_NAME + 1];		// Room short name
   char FullName[Roo_MAX_BYTES_FULL_NAME + 1];		// Room full name
   unsigned Capacity;		// Room seating capacity
				// (maximum people who fit in the room)
  };

#define Roo_NUM_ORDERS 6
typedef enum
  {
   Roo_ORDER_BY_BUILDING  = 0,
   Roo_ORDER_BY_FLOOR     = 1,
   Roo_ORDER_BY_TYPE      = 2,
   Roo_ORDER_BY_SHRT_NAME = 3,
   Roo_ORDER_BY_FULL_NAME = 4,
   Roo_ORDER_BY_CAPACITY  = 5,
  } Roo_Order_t;
#define Roo_ORDER_DEFAULT Roo_ORDER_BY_BUILDING

/***** Get all data or only short name *****/
typedef enum
  {
   Roo_ALL_DATA,
   Roo_ONLY_SHRT_NAME,
  } Roo_WhichData_t;

struct Roo_Rooms
  {
   unsigned Num;		// Number of rooms
   struct Roo_Room *Lst;	// List of rooms
   Roo_Order_t SelectedOrder;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Roo_ResetRooms (struct Roo_Rooms *Rooms);

void Roo_SeeRooms (void);
void Roo_EditRooms (void);
void Roo_PutIconToViewRooms (void);
void Roo_GetListRooms (struct Roo_Rooms *Rooms,
                       Roo_WhichData_t WhichData);
void Roo_FreeListRooms (struct Roo_Rooms *Rooms);

void Roo_GetListRoomsInThisCtr (void);

long Roo_GetParamRooCod (void);

void Roo_RemoveRoom (void);
void Roo_RemoveAllRoomsInCtr (long CtrCod);
void Roo_ChangeBuilding (void);
void Roo_ChangeFloor (void);
void Roo_RenameRoomShort (void);
void Roo_RenameRoomFull (void);
void Roo_ChangeCapacity (void);
void Roo_ContEditAfterChgRoom (void);

void Roo_RecFormNewRoom (void);

#endif
