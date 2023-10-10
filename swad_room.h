// swad_room.h: classrooms, laboratories, offices or other rooms in a center

#ifndef _SWAD_ROO
#define _SWAD_ROO
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_building.h"
#include "swad_string.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Roo_MAX_CAPACITY	10000	// If capacity of a room is greater than this, it is considered infinite
#define Roo_UNLIMITED_CAPACITY	INT_MAX	// This number can be stored in database as an integer...
					// ...and means that a room has no limited capacity

#define Roo_NUM_TYPES 25
typedef enum
  {
   Roo_NO_TYPE        =  0,	//							// "Sin tipo"
   Roo_ADMINISTRATION =  1,	// "user-tie.svg"					// "Administración"
   Roo_AUDITORIUM     =  2,	// "landmark.svg" "university.svg" "theater-masks.svg"	// "Auditorio"
   Roo_CAFETERIA      =  3,	// "coffee.svg" "mug-hot.svg"				// "Cafetería"
   Roo_CANTEEN        =  4,	// "utensils.svg"					// "Comedor"
   Roo_CLASSROOM      =  5,	// "chalkboard-teacher.svg" "chalkboard.svg"		// "Aula"
   Roo_CONCIERGE      =  6,	// "concierge-bell" "user-cog.svg" "house-user.svg" 	// "Conserjería/Portería"
   Roo_CORRIDOR       =  7,	// "walking.svg" "map-signs.svg" "door-open.svg"	// "Pasillo"
   Roo_GYM            =  8,	// "dumbbell.svg"					// "Gimnasio"
   Roo_HALL           =  9,	// "map-signs.svg" "archway.svg" "hotel.svg" "concierge-bell.svg" "door-open.svg" "street-view.svg"	// "Vestíbulo"
   Roo_KINDERGARTEN   = 10,	// "child.svg" "baby.svg" "baby-carriage.svg"		// "Guardería"
   Roo_LABORATORY     = 11,	// "flask.svg" "microscope.svg"				// "Laboratorio"
   Roo_LIBRARY        = 12,	// "book.svg" "book-reader.svg"				// "Biblioteca"
   Roo_OFFICE         = 13,	// "door-closed.svg" "house-user.svg" "door-open.svg"	// "Despacho/Oficina"
   Roo_OUTDOORS       = 14,	// "cloud-sun.svg"					// "Exteriores"
   Roo_PARKING        = 15,	// "car.svg"						// "Aparcamiento"
   Roo_PAVILION       = 16,	// "volleyball-ball.svg" "running.svg"			// "Pabellón"
   Roo_ROOM           = 17,	// "door-open.svg" "chalkboard.svg"					// "Sala"
   Roo_SECRETARIAT    = 18,	// "stamp.svg" "pen-alt.svg" "file-alt.svg" "user-tie.svg"	// "Secretaría"
   Roo_SEMINAR        = 19,	// "chalkboard-teacher.svg" "chalkboard.svg"		// "Seminario"
   Roo_SHOP           = 20,	// "shopping-cart.svg" "shopping-basket.svg" "store-alt.svg" "shopping-bag.svg"	// "Tienda"
   Roo_STORE          = 21,	// "dolly.svg" "boxes.svg" "box-open.svg" "box.svg"	// "Almacén"
   Roo_TOILETS        = 22,	// "toilet.svg"						// "Aseos"
   Roo_VIRTUAL        = 23,	// "laptop-house.svg" "cloud.svg"			// "Virtual"
   Roo_YARD           = 24,	// "tree.svg" "leaf.svg" "canadian-mapple-leaf.svg"	// "Patio/Jardín"
  } Roo_RoomType_t;

struct Roo_Room
  {
   long RooCod;						// Room code
   long CtrCod;						// Center code
   long BldCod;						// Building code
   char BldShrtName[Nam_MAX_BYTES_SHRT_NAME + 1];	// Building short name
   int  Floor;						// Room floor
   Roo_RoomType_t Type;					// Room type
   char ShrtName[Nam_MAX_BYTES_SHRT_NAME + 1];		// Room short name
   char FullName[Nam_MAX_BYTES_FULL_NAME + 1];		// Room full name
   unsigned Capacity;		// Room seating capacity
				// (maximum people who fit in the room)
   unsigned long long MACnum;
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
void Roo_ChangeMAC (void);
void Roo_EditRooms (void);
void Roo_GetListRooms (struct Roo_Rooms *Rooms,Roo_WhichData_t WhichData);
void Roo_FreeListRooms (struct Roo_Rooms *Rooms);

void Roo_GetListRoomsInThisCtr (void);

void Roo_RemoveRoom (void);
void Roo_ChangeBuilding (void);
void Roo_ChangeFloor (void);
void Roo_ChangeType (void);
void Roo_RenameRoomShort (void);
void Roo_RenameRoomFull (void);
void Roo_ChangeCapacity (void);
void Roo_ContEditAfterChgRoom (void);

void Roo_ReceiveFormNewRoom (void);

#endif
