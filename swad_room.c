// swad_room.c: classrooms, laboratories, offices or other rooms in a center

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <malloc.h>		// For calloc, free
#include <stddef.h>		// For NULL
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_building.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_group_database.h"
#include "swad_HTML.h"
#include "swad_MAC.h"
#include "swad_parameter_code.h"
#include "swad_room.h"
#include "swad_room_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const char *Roo_TypesIcons[Roo_NUM_TYPES] =
  {
   [Roo_NO_TYPE       ] = "question.svg",
   [Roo_ADMINISTRATION] = "user-tie.svg",
   [Roo_AUDITORIUM    ] = "landmark.svg",
   [Roo_CAFETERIA     ] = "coffee.svg",
   [Roo_CANTEEN       ] = "utensils.svg",
   [Roo_CLASSROOM     ] = "chalkboard-teacher.svg",
   [Roo_CONCIERGE     ] = "concierge-bell.svg",
   [Roo_CORRIDOR      ] = "walking.svg",
   [Roo_GYM           ] = "dumbbell.svg",
   [Roo_HALL          ] = "map-signs.svg",
   [Roo_KINDERGARTEN  ] = "child.svg",
   [Roo_LABORATORY    ] = "flask.svg",
   [Roo_LIBRARY       ] = "book.svg",
   [Roo_OFFICE        ] = "door-closed.svg",
   [Roo_OUTDOORS      ] = "cloud-sun.svg",
   [Roo_PARKING       ] = "car.svg",
   [Roo_PAVILION      ] = "volleyball-ball.svg",
   [Roo_ROOM          ] = "door-open.svg",
   [Roo_SECRETARIAT   ] = "stamp.svg",
   [Roo_SEMINAR       ] = "chalkboard.svg",
   [Roo_SHOP          ] = "shopping-cart.svg",
   [Roo_STORE         ] = "dolly.svg",
   [Roo_TOILETS       ] = "toilet.svg",
   [Roo_VIRTUAL       ] = "laptop-house.svg",
   [Roo_YARD          ] = "tree.svg",
  };

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static struct Roo_Room *Roo_EditingRoom = NULL;	// Static variable to keep the room being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Roo_GetAndListMACAddresses (long RooCod);
static void Roo_GetAndEditMACAddresses (long RooCod,const char *Anchor);

static Roo_Order_t Roo_GetParRoomOrder (void);
static Usr_Can_t Roo_CheckIfICanCreateRooms (void);
static void Roo_PutIconsListingRooms (__attribute__((unused)) void *Args);
static void Roo_PutIconToEditRooms (void);
static void Roo_PutIconsEditingRooms (__attribute__((unused)) void *Args);

static void Roo_GetRoomDataByCod (struct Roo_Room *Room);
static void Roo_GetRoomDataFromRow (MYSQL_RES *mysql_res,
                                    struct Roo_Room *Room,
                                    Roo_WhichData_t WhichData);
static void Roo_GetBldShrtName (struct Roo_Room *Room,const char *BldShrtNameFromDB);
static Roo_RoomType_t Roo_GetTypeFromString (const char *Str);

static void Roo_EditRoomsInternal (void);

static void Roo_ListRoomsForEdition (const struct Bld_Buildings *Buildings,
                                     const struct Roo_Rooms *Rooms);
static void Roo_PutParRooCod (void *RooCod);
static void Roo_PutSelectorBuilding (long BldCod,
                                     const struct Bld_Buildings *Buildings,
                                     HTM_SubmitOnChange_t SubmitOnChange);
static void Roo_PutSelectorType (Roo_RoomType_t RoomType,
                                 HTM_SubmitOnChange_t SubmitOnChange);
static int Roo_GetParFloor (void);
static Roo_RoomType_t Roo_GetParType (void);

static void Roo_RenameRoom (Nam_ShrtOrFullName_t ShrtOrFull);

static void Roo_WriteCapacity (char Str[Cns_MAX_DECIMAL_DIGITS_UINT + 1],unsigned Capacity);

static void Roo_PutFormToCreateRoom (const struct Bld_Buildings *Buildings);
static void Roo_PutHeadRooms (void);
static void Roo_CreateRoom (struct Roo_Room *Room);

static void Roo_EditingRoomConstructor (void);
static void Roo_EditingRoomDestructor (void);

/*****************************************************************************/
/**************************** Reset rooms context ****************************/
/*****************************************************************************/

void Roo_ResetRooms (struct Roo_Rooms *Rooms)
  {
   Rooms->Num           = 0;	// Number of rooms
   Rooms->Lst           = NULL;	// List of rooms
   Rooms->SelectedOrder = Roo_ORDER_DEFAULT;
  }

/*****************************************************************************/
/****************************** List all rooms *******************************/
/*****************************************************************************/

void Roo_SeeRooms (void)
  {
   extern const char *Hlp_CENTER_Rooms;
   extern const char *Txt_Rooms;
   extern const char *Txt_ROOMS_HELP_ORDER[Roo_NUM_ORDERS];
   extern const char *Txt_ROOMS_ORDER[Roo_NUM_ORDERS];
   extern const char *Txt_MAC_address;
   extern const char *Txt_ROOM_TYPES[Roo_NUM_TYPES];
   struct Roo_Rooms Rooms;
   Roo_Order_t Order;
   unsigned NumRoom;
   char StrCapacity[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Node[Hie_CTR].HieCod <= 0)		// No center selected
      return;

   /***** Reset rooms context *****/
   Roo_ResetRooms (&Rooms);

   /***** Get parameter with the type of order in the list of rooms *****/
   Rooms.SelectedOrder = Roo_GetParRoomOrder ();

   /***** Get list of rooms *****/
   Roo_GetListRooms (&Rooms,Roo_ALL_DATA);

   /***** Table head *****/
   Box_BoxBegin (Txt_Rooms,Roo_PutIconsListingRooms,NULL,
		 Hlp_CENTER_Rooms,Box_NOT_CLOSABLE);
      HTM_TABLE_BeginWideMarginPadding (2);
	 HTM_TR_Begin (NULL);

	 /* Columns visible by all */
	 for (Order  = (Roo_Order_t) 0;
	      Order <= (Roo_Order_t) (Roo_NUM_ORDERS - 1);
	      Order++)
	   {
            HTM_TH_Begin (HTM_HEAD_LEFT);
	       Frm_BeginForm (ActSeeRoo);
		  Par_PutParUnsigned (NULL,"Order",(unsigned) Order);
		  HTM_BUTTON_Submit_Begin (Txt_ROOMS_HELP_ORDER[Order],
		                           "class=\"BT_LINK\"");
		     if (Order == Rooms.SelectedOrder)
			HTM_U_Begin ();
		     HTM_Txt (Txt_ROOMS_ORDER[Order]);
		     if (Order == Rooms.SelectedOrder)
			HTM_U_End ();
		  HTM_BUTTON_End ();
	       Frm_EndForm ();
	    HTM_TH_End ();
	   }

	 /* Column visible by admins */
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_CTR_ADM:
	    case Rol_INS_ADM:
	    case Rol_SYS_ADM:
	       HTM_TH (Txt_MAC_address,HTM_HEAD_LEFT);
	       break;
	    default:
	       break;
	   }

	 HTM_TR_End ();

	 /***** Write list of rooms *****/
	 for (NumRoom = 0, The_ResetRowColor ();
	      NumRoom < Rooms.Num;
	      NumRoom++, The_ChangeRowColor ())
	   {
	    HTM_TR_Begin (NULL);

	       /* Building short name */
	       HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
	                     The_GetSuffix (),
	                     The_GetColorRows ());
		  HTM_Txt (Rooms.Lst[NumRoom].BldShrtName);
	       HTM_TD_End ();

	       /* Floor */
	       HTM_TD_Begin ("class=\"RT DAT_%s %s\"",
	                     The_GetSuffix (),
	                     The_GetColorRows ());
		  HTM_Int (Rooms.Lst[NumRoom].Floor);
	       HTM_TD_End ();

	       /* Type */
	       HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
	                     The_GetSuffix (),
	                     The_GetColorRows ());
		  Ico_PutIconOn (Roo_TypesIcons[Rooms.Lst[NumRoom].Type],Ico_BLACK,
		                 Txt_ROOM_TYPES[Rooms.Lst[NumRoom].Type]);
	       HTM_TD_End ();

	       /* Short name */
	       HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
	                     The_GetSuffix (),
	                     The_GetColorRows ());
		  HTM_Txt (Rooms.Lst[NumRoom].ShrtName);
	       HTM_TD_End ();

	       /* Full name */
	       HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
	                     The_GetSuffix (),
	                     The_GetColorRows ());
		  HTM_Txt (Rooms.Lst[NumRoom].FullName);
	       HTM_TD_End ();

	       /* Capacity */
	       HTM_TD_Begin ("class=\"RT DAT_%s %s\"",
	                     The_GetSuffix (),
	                     The_GetColorRows ());
		  Roo_WriteCapacity (StrCapacity,Rooms.Lst[NumRoom].Capacity);
		  HTM_Txt (StrCapacity);
	       HTM_TD_End ();

	       /* Column visible by admins */
	       switch (Gbl.Usrs.Me.Role.Logged)
		 {
		  case Rol_CTR_ADM:
		  case Rol_INS_ADM:
		  case Rol_SYS_ADM:
		     HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
		                   The_GetSuffix (),
		                   The_GetColorRows ());
			Roo_GetAndListMACAddresses (Rooms.Lst[NumRoom].RooCod);
		     HTM_TD_End ();
		     break;
		  default:
		     break;
		 }

	    HTM_TR_End ();
	   }

      /***** End table *****/
      HTM_TABLE_End ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of rooms *****/
   Roo_FreeListRooms (&Rooms);
  }

/*****************************************************************************/
/************ Get and list the MAC addresses associated to a room ************/
/*****************************************************************************/

static void Roo_GetAndListMACAddresses (long RooCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumMACs;

   /***** Get MAC addresses from database *****/
   NumMACs = Roo_DB_GetMACAddresses (&mysql_res,RooCod);

   /***** Write the MAC addresses *****/
   MAC_ListMACAddresses (NumMACs,&mysql_res);
  }

/*****************************************************************************/
/****** Get and list for edition the MAC addresses associated to a room ******/
/*****************************************************************************/

static void Roo_GetAndEditMACAddresses (long RooCod,const char *Anchor)
  {
   MYSQL_RES *mysql_res;
   unsigned NumMACs;

   /***** Get MAC addresses from database *****/
   NumMACs = Roo_DB_GetMACAddresses (&mysql_res,RooCod);

   /***** Write the MAC addresses *****/
   MAC_EditMACAddresses (RooCod,Anchor,NumMACs,&mysql_res);
  }

/*****************************************************************************/
/**************************** Change MAC of a room ***************************/
/*****************************************************************************/

void Roo_ChangeMAC (void)
  {
   unsigned long long OldMACnum;
   unsigned long long NewMACnum;

   /***** Room constructor *****/
   Roo_EditingRoomConstructor ();

   /***** Get parameters from form *****/
   /* Get room code */
   Roo_EditingRoom->RooCod = ParCod_GetAndCheckPar (ParCod_Roo);

   /* Get the old MAC address of the room */
   OldMACnum = MAC_GetMACnumFromForm ("MAC");

   /* Get the new MAC address of the room */
   NewMACnum = MAC_GetMACnumFromForm ("NewMAC");

   /***** Get data of the room from database *****/
   Roo_GetRoomDataByCod (Roo_EditingRoom);

   /***** Check if the new MAC is different from the old MAC *****/
   if (OldMACnum)
      Roo_DB_RemoveMACAddress (Roo_EditingRoom->RooCod,OldMACnum);
   if (NewMACnum)
      /***** Update the table of rooms-MACs changing the old MAC for the new one *****/
      Roo_DB_UpdateMACAddress (Roo_EditingRoom->RooCod,NewMACnum);

   Roo_EditingRoom->MACnum = NewMACnum;
  }

/*****************************************************************************/
/*********** Get parameter with the type or order in list of rooms ***********/
/*****************************************************************************/

static Roo_Order_t Roo_GetParRoomOrder (void)
  {
   return (Roo_Order_t) Par_GetParUnsignedLong ("Order",
						  0,
						  Roo_NUM_ORDERS - 1,
						  (unsigned long) Roo_ORDER_DEFAULT);
  }

/*****************************************************************************/
/*********************** Check if I can create rooms *************************/
/*****************************************************************************/

static Usr_Can_t Roo_CheckIfICanCreateRooms (void)
  {
   return (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM) ? Usr_CAN :
						     Usr_CAN_NOT;
  }

/*****************************************************************************/
/****************** Put contextual icons in list of rooms ********************/
/*****************************************************************************/

static void Roo_PutIconsListingRooms (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit rooms *****/
   if (Roo_CheckIfICanCreateRooms () == Usr_CAN)
      Roo_PutIconToEditRooms ();
  }

/*****************************************************************************/
/********************** Put a link (form) to edit rooms **********************/
/*****************************************************************************/

static void Roo_PutIconToEditRooms (void)
  {
   Ico_PutContextualIconToEdit (ActEdiRoo,NULL,NULL,NULL);
  }

/*****************************************************************************/
/*************************** Put forms to edit rooms *************************/
/*****************************************************************************/

void Roo_EditRooms (void)
  {
   /***** Room constructor *****/
   Roo_EditingRoomConstructor ();

   /***** Edit rooms *****/
   Roo_EditRoomsInternal ();

   /***** Room destructor *****/
   Roo_EditingRoomDestructor ();
  }

static void Roo_EditRoomsInternal (void)
  {
   extern const char *Hlp_CENTER_Rooms_edit;
   extern const char *Txt_Rooms;
   struct Bld_Buildings Buildings;
   struct Roo_Rooms Rooms;

   /***** Reset context *****/
   Bld_ResetBuildings (&Buildings);
   Roo_ResetRooms (&Rooms);

   /***** Get lists of buildings and rooms *****/
   Bld_GetListBuildings (&Buildings,Bld_ALL_DATA);
   Roo_GetListRooms (&Rooms,Roo_ALL_DATA);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Rooms,Roo_PutIconsEditingRooms,NULL,
                 Hlp_CENTER_Rooms_edit,Box_NOT_CLOSABLE);

      /***** Put a form to create a new room *****/
      Roo_PutFormToCreateRoom (&Buildings);

      /***** Forms to edit current rooms *****/
      if (Rooms.Num)
	 Roo_ListRoomsForEdition (&Buildings,&Rooms);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free lists of rooms and buildings *****/
   Roo_FreeListRooms (&Rooms);
   Bld_FreeListBuildings (&Buildings);
  }

/*****************************************************************************/
/***************** Put contextual icons in edition of rooms ******************/
/*****************************************************************************/

static void Roo_PutIconsEditingRooms (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view rooms *****/
   Ico_PutContextualIconToView (ActSeeRoo,NULL,NULL,NULL);
  }

/*****************************************************************************/
/****************************** List all rooms *******************************/
/*****************************************************************************/

void Roo_GetListRooms (struct Roo_Rooms *Rooms,Roo_WhichData_t WhichData)
  {
   MYSQL_RES *mysql_res;
   unsigned NumRoom;

   /***** Get rooms from database *****/
   if ((Rooms->Num = Roo_DB_GetListRooms (&mysql_res,Gbl.Hierarchy.Node[Hie_CTR].HieCod,
                                          WhichData,Rooms->SelectedOrder))) // Rooms found...
     {
      /***** Create list with courses in center *****/
      if ((Rooms->Lst = calloc ((size_t) Rooms->Num,
                                sizeof (*Rooms->Lst))) == NULL)
          Err_NotEnoughMemoryExit ();

      /***** Get the rooms *****/
      for (NumRoom = 0;
	   NumRoom < Rooms->Num;
	   NumRoom++)
         Roo_GetRoomDataFromRow (mysql_res,&Rooms->Lst[NumRoom],WhichData);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Get room data from database using the room code ***************/
/*****************************************************************************/

static void Roo_GetRoomDataByCod (struct Roo_Room *Room)
  {
   MYSQL_RES *mysql_res;

   /***** Trivial check *****/
   if (Room->RooCod <= 0)
      return;

   /***** Get data of a room from database *****/
   if (Roo_DB_GetRoomDataByCod (&mysql_res,Room->RooCod)) // Room found...
      Roo_GetRoomDataFromRow (mysql_res,Room,Roo_ALL_DATA);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Get room data from database row ***********************/
/*****************************************************************************/

static void Roo_GetRoomDataFromRow (MYSQL_RES *mysql_res,
                                    struct Roo_Room *Room,
                                    Roo_WhichData_t WhichData)
  {
   MYSQL_ROW row;

   /***** Get row *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get room code (row[0]) *****/
   if ((Room->RooCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongRoomExit ();

   switch (WhichData)
     {
      case Roo_ALL_DATA:
	 /***** Get building code (row[1]) *****/
	 Room->BldCod = Str_ConvertStrCodToLongCod (row[1]);

	 /***** Get the short name of the building (row[2]) *****/
	 Roo_GetBldShrtName (Room,row[2]);

	 /***** Get floor (row[3]) *****/
	 Room->Floor = Str_ConvertStrCodToLongCod (row[3]);

	 /***** Get type (row[4]) *****/
	 Room->Type = Roo_GetTypeFromString (row[4]);

	 /***** Get the short (row[5]) and full (row[6]) names of the room *****/
	 Str_Copy (Room->ShrtName,row[5],sizeof (Room->ShrtName) - 1);
	 Str_Copy (Room->FullName,row[6],sizeof (Room->FullName) - 1);

	 /***** Get seating capacity in this room (row[7]) *****/
	 if (sscanf (row[7],"%u",&Room->Capacity) != 1)
	    Room->Capacity = Roo_UNLIMITED_CAPACITY;
	 break;
      case Roo_ONLY_SHRT_NAME:
      default:
	 /***** Get the short name of the room (row[1]) *****/
	 Str_Copy (Room->ShrtName,row[1],sizeof (Room->ShrtName) - 1);
	 break;
     }
  }

/*****************************************************************************/
/****************** Get building short name from database ********************/
/*****************************************************************************/

static void Roo_GetBldShrtName (struct Roo_Room *Room,const char *BldShrtNameFromDB)
  {
   extern const char *Txt_No_assigned_building;
   extern const char *Txt_Another_building;

   if (Room->BldCod < 0)
      Str_Copy (Room->BldShrtName,Txt_No_assigned_building,
		sizeof (Room->BldShrtName) - 1);
   else if (Room->BldCod == 0)
      Str_Copy (Room->BldShrtName,Txt_Another_building,
		sizeof (Room->BldShrtName) - 1);
   else	// Room->BldCod > 0
     {
      Room->BldShrtName[0] = '\0';
      if (BldShrtNameFromDB)
	 Str_Copy (Room->BldShrtName,BldShrtNameFromDB,
		   sizeof (Room->BldShrtName) - 1);
     }
  }

/*****************************************************************************/
/********************** Convert from string to type **************************/
/*****************************************************************************/

static Roo_RoomType_t Roo_GetTypeFromString (const char *Str)
  {
   extern const char *Roo_TypesDB[Roo_NUM_TYPES];
   Roo_RoomType_t Type;

   /***** Compare string with all string types *****/
   for (Type  = (Roo_RoomType_t) 0;
	Type <= (Roo_RoomType_t) (Roo_NUM_TYPES - 1);
	Type++)
      if (!strcmp (Roo_TypesDB[Type],Str))
	 return Type;

   return Roo_NO_TYPE;
  }

/*****************************************************************************/
/**************************** Free list of rooms *****************************/
/*****************************************************************************/

void Roo_FreeListRooms (struct Roo_Rooms *Rooms)
  {
   if (Rooms->Lst)
     {
      /***** Free memory used by the list of rooms in institution *****/
      free (Rooms->Lst);
      Rooms->Lst = NULL;
      Rooms->Num = 0;
     }
  }

/*****************************************************************************/
/***************************** List all rooms ********************************/
/*****************************************************************************/

static void Roo_ListRoomsForEdition (const struct Bld_Buildings *Buildings,
                                     const struct Roo_Rooms *Rooms)
  {
   static Act_Action_t ActionRename[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = ActRenRooSho,
      [Nam_FULL_NAME] = ActRenRooFul,
     };
   unsigned NumRoom;
   struct Roo_Room *Room;
   char *Anchor = NULL;
   char StrCapacity[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   HTM_TABLE_Begin ("TBL_SCROLL");

      /***** Write heading *****/
      Roo_PutHeadRooms ();

      /***** Write all rooms *****/
      for (NumRoom = 0;
	   NumRoom < Rooms->Num;
	   NumRoom++)
	{
	 Room = &Rooms->Lst[NumRoom];

	 /* Build anchor string */
	 Frm_SetAnchorStr (Room->RooCod,&Anchor);

	 HTM_TR_Begin (NULL);

	    /* Put icon to remove room */
	    HTM_TD_Begin ("class=\"BT\"");
	       Ico_PutContextualIconToRemove (ActRemRoo,NULL,
					      Roo_PutParRooCod,&Room->RooCod);
	    HTM_TD_End ();

	    /* Room code */
	    HTM_TD_Begin ("class=\"CODE DAT_%s\"",The_GetSuffix ());
	       HTM_ARTICLE_Begin (Anchor);
		  HTM_Long (Room->RooCod);
	       HTM_ARTICLE_End ();
	    HTM_TD_End ();

	    /* Building */
	    HTM_TD_Begin ("class=\"CT\"");
	       Frm_BeginFormAnchor (ActChgRooBld,Anchor);
		  ParCod_PutPar (ParCod_Roo,Room->RooCod);
		  Roo_PutSelectorBuilding (Room->BldCod,Buildings,
					   HTM_SUBMIT_ON_CHANGE);
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Floor */
	    HTM_TD_Begin ("class=\"LT\"");
	       Frm_BeginFormAnchor (ActChgRooFlo,Anchor);
		  ParCod_PutPar (ParCod_Roo,Room->RooCod);
		  HTM_INPUT_LONG ("Floor",(long) INT_MIN,(long) INT_MAX,(long) Room->Floor,
				  HTM_ENABLED,HTM_SUBMIT_ON_CHANGE,
				  "class=\"INPUT_LONG INPUT_%s\"",
				  The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Room type */
	    HTM_TD_Begin ("class=\"CT\"");
	       Frm_BeginFormAnchor (ActChgRooTyp,Anchor);
		  ParCod_PutPar (ParCod_Roo,Room->RooCod);
		  Roo_PutSelectorType (Room->Type,HTM_SUBMIT_ON_CHANGE);
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Room short name and full name */
	    Names[Nam_SHRT_NAME] = Room->ShrtName;
	    Names[Nam_FULL_NAME] = Room->FullName;
	    Nam_ExistingShortAndFullNames (ActionRename,
					   ParCod_Roo,Room->RooCod,
					   Names,
					   Frm_PUT_FORM);

	    /* Seating capacity */
	    HTM_TD_Begin ("class=\"LT\"");
	       Frm_BeginFormAnchor (ActChgRooMaxUsr,Anchor);
		  ParCod_PutPar (ParCod_Roo,Room->RooCod);
		  Roo_WriteCapacity (StrCapacity,Room->Capacity);
		  HTM_INPUT_TEXT ("Capacity",Cns_MAX_DECIMAL_DIGITS_UINT,StrCapacity,
				  HTM_ENABLED,HTM_SUBMIT_ON_CHANGE,
				  "size=\"3\" class=\"INPUT_%s\"",
				  The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* MAC addresses */
	    HTM_TD_Begin ("class=\"LT\"");
	       Roo_GetAndEditMACAddresses (Room->RooCod,Anchor);
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********************* Write parameter with code of room *********************/
/*****************************************************************************/

static void Roo_PutParRooCod (void *RooCod)
  {
   if (RooCod)
      ParCod_PutPar (ParCod_Roo,*((long *) RooCod));
  }

/*****************************************************************************/
/********** Put selector of the building to which the room belongs ***********/
/*****************************************************************************/

static void Roo_PutSelectorBuilding (long BldCod,
                                     const struct Bld_Buildings *Buildings,
                                     HTM_SubmitOnChange_t SubmitOnChange)
  {

   extern const char *Txt_No_assigned_building;
   extern const char *Txt_Another_building;
   unsigned NumBld;
   const struct Bld_Building *Bld;

   /***** Begin selector *****/
   HTM_SELECT_Begin (HTM_ENABLED,SubmitOnChange,NULL,
		     "name=\"BldCod\" class=\"BLD_SEL INPUT_%s\"",
		     The_GetSuffix ());

      /***** Option for no assigned building *****/
      HTM_OPTION (HTM_Type_STRING,"-1",
		  BldCod < 0 ? HTM_OPTION_SELECTED :
			       HTM_OPTION_UNSELECTED,
		  HTM_ENABLED,
		  "%s",Txt_No_assigned_building);

      /***** Option for another room *****/
      HTM_OPTION (HTM_Type_STRING,"0",
		  BldCod == 0 ? HTM_OPTION_SELECTED :
			        HTM_OPTION_UNSELECTED,
		  HTM_ENABLED,
		  "%s",Txt_Another_building);

      /***** Options for buildings *****/
      for (NumBld = 0;
	   NumBld < Buildings->Num;
	   NumBld++)
	{
	 Bld = &Buildings->Lst[NumBld];
	 HTM_OPTION (HTM_Type_LONG,&Bld->BldCod,
		     BldCod == Bld->BldCod ? HTM_OPTION_SELECTED :
					     HTM_OPTION_UNSELECTED,
		     HTM_ENABLED,
		     "%s",Bld->ShrtName);
	}

   /***** End selector *****/
   HTM_SELECT_End ();
  }

/*****************************************************************************/
/************************ Put selector of room type **************************/
/*****************************************************************************/

static void Roo_PutSelectorType (Roo_RoomType_t RoomType,
                                 HTM_SubmitOnChange_t SubmitOnChange)
  {
   extern const char *Txt_ROOM_TYPES[Roo_NUM_TYPES];
   Roo_RoomType_t Type;

   /***** Begin selector *****/
   HTM_SELECT_Begin (HTM_ENABLED,SubmitOnChange,NULL,
		     "name=\"Type\" class=\"ROOM_TYPE_SEL INPUT_%s\"",
		     The_GetSuffix ());

      /***** Options for types *****/
      for (Type  = (Roo_RoomType_t) 0;
	   Type <= (Roo_RoomType_t) (Roo_NUM_TYPES - 1);
	   Type++)
	 HTM_OPTION (HTM_Type_UNSIGNED,&Type,
		     Type == RoomType ? HTM_OPTION_SELECTED :
					HTM_OPTION_UNSELECTED,
		     HTM_ENABLED,
		     "%s",Txt_ROOM_TYPES[Type]);

   /***** End selector *****/
   HTM_SELECT_End ();
  }

/*****************************************************************************/
/************************** Get parameter with floor *************************/
/*****************************************************************************/

static int Roo_GetParFloor (void)
  {
   return (int) Par_GetParLong ("Floor");
  }

/*****************************************************************************/
/************************** Get parameter with type **************************/
/*****************************************************************************/

static Roo_RoomType_t Roo_GetParType (void)
  {
   long TypeLong;

   /***** Get type string *****/
   TypeLong = Par_GetParLong ("Type");
   if (TypeLong >= 0 && TypeLong < Roo_NUM_TYPES)
      return (Roo_RoomType_t) TypeLong;	// Correc type

   /***** Default type when no type or wrong type *****/
   return Roo_NO_TYPE;
  }

/*****************************************************************************/
/******************************** Remove a room ******************************/
/*****************************************************************************/

void Roo_RemoveRoom (void)
  {
   extern const char *Txt_Room_X_removed;

   /***** Room constructor *****/
   Roo_EditingRoomConstructor ();

   /***** Get room code *****/
   Roo_EditingRoom->RooCod = ParCod_GetAndCheckPar (ParCod_Roo);

   /***** Get data of the room from database *****/
   Roo_GetRoomDataByCod (Roo_EditingRoom);

   /***** Update groups assigned to this room *****/
   Grp_DB_ResetRoomInGrps (Roo_EditingRoom->RooCod);

   /***** Remove room *****/
   Roo_DB_RemoveRoom (Roo_EditingRoom->RooCod);

   /***** Create message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_Room_X_removed,
	            Roo_EditingRoom->FullName);
  }

/*****************************************************************************/
/********************* Change sitting capacity of a room *********************/
/*****************************************************************************/

void Roo_ChangeBuilding (void)
  {
   extern const char *Txt_The_building_of_room_X_has_not_changed;
   extern const char *Txt_The_building_of_room_X_is_now_Y;
   long NewBldCod;

   /***** Room constructor *****/
   Roo_EditingRoomConstructor ();

   /***** Get parameters from form *****/
   /* Get room code */
   Roo_EditingRoom->RooCod = ParCod_GetAndCheckPar (ParCod_Roo);

   /* Get room building */
   NewBldCod = ParCod_GetAndCheckPar (ParCod_Bld);

   /***** Get data of the room from database *****/
   Roo_GetRoomDataByCod (Roo_EditingRoom);

   /***** Check if the old building equals the new one
          (this happens when return is pressed without changes) *****/
   if (NewBldCod < 0)
      NewBldCod = -1L;
   if (NewBldCod == Roo_EditingRoom->BldCod)
      /***** Message to show no changes made *****/
      Ale_CreateAlert (Ale_INFO,NULL,
	               Txt_The_building_of_room_X_has_not_changed,
		       Roo_EditingRoom->FullName);
   else
     {
      /***** Update the table of rooms changing the old building for the new one *****/
      Roo_DB_UpdateRoomBuilding (Roo_EditingRoom->RooCod,NewBldCod);

      /***** Get updated data of the room from database *****/
      Roo_GetRoomDataByCod (Roo_EditingRoom);

      /***** Message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
		       Txt_The_building_of_room_X_is_now_Y,
		       Roo_EditingRoom->FullName,Roo_EditingRoom->BldShrtName);
     }
  }

/*****************************************************************************/
/************************ Change the floor of a room *************************/
/*****************************************************************************/

void Roo_ChangeFloor (void)
  {
   extern const char *Txt_The_floor_of_room_X_has_not_changed;
   extern const char *Txt_The_floor_of_room_X_is_now_Y;
   int NewFloor;

   /***** Room constructor *****/
   Roo_EditingRoomConstructor ();

   /***** Get parameters from form *****/
   /* Get room code */
   Roo_EditingRoom->RooCod = ParCod_GetAndCheckPar (ParCod_Roo);

   /* Get room floor */
   NewFloor = Roo_GetParFloor ();

   /***** Get data of the room from database *****/
   Roo_GetRoomDataByCod (Roo_EditingRoom);

   /***** Check if the old floor equals the new one
          (this happens when return is pressed without changes) *****/
   if (NewFloor == Roo_EditingRoom->Floor)
      /***** Message to show no changes made *****/
      Ale_CreateAlert (Ale_INFO,NULL,
	               Txt_The_floor_of_room_X_has_not_changed,
		       Roo_EditingRoom->FullName);
   else
     {
      /***** Update the table of rooms changing the old floor for the new one *****/
      Roo_DB_UpdateRoomFloor (Roo_EditingRoom->RooCod,NewFloor);

      /***** Get updated data of the room from database *****/
      Roo_GetRoomDataByCod (Roo_EditingRoom);

      /***** Message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
		       Txt_The_floor_of_room_X_is_now_Y,
		       Roo_EditingRoom->FullName,Roo_EditingRoom->Floor);
     }
  }

/*****************************************************************************/
/************************* Change the type of a room *************************/
/*****************************************************************************/

void Roo_ChangeType (void)
  {
   extern const char *Txt_The_type_of_room_X_has_not_changed;
   extern const char *Txt_The_type_of_room_X_is_now_Y;
   extern const char *Txt_ROOM_TYPES[Roo_NUM_TYPES];
   Roo_RoomType_t NewType;

   /***** Room constructor *****/
   Roo_EditingRoomConstructor ();

   /***** Get parameters from form *****/
   /* Get room code */
   Roo_EditingRoom->RooCod = ParCod_GetAndCheckPar (ParCod_Roo);

   /* Get room type */
   NewType = Roo_GetParType ();

   /***** Get data of the room from database *****/
   Roo_GetRoomDataByCod (Roo_EditingRoom);

   /***** Check if the old type equals the new one
          (this happens when return is pressed without changes) *****/
   if ((unsigned) NewType >= Roo_NUM_TYPES)
      NewType = Roo_NO_TYPE;
   if (NewType == Roo_EditingRoom->Type)
      /***** Message to show no changes made *****/
      Ale_CreateAlert (Ale_INFO,NULL,
	               Txt_The_type_of_room_X_has_not_changed,
		       Roo_EditingRoom->FullName);
   else
     {
      /***** Update the table of rooms changing the old type for the new one *****/
      Roo_DB_UpdateRoomType (Roo_EditingRoom->RooCod,NewType);

      /***** Get updated data of the room from database *****/
      Roo_GetRoomDataByCod (Roo_EditingRoom);

      /***** Message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
		       Txt_The_type_of_room_X_is_now_Y,
		       Roo_EditingRoom->FullName,Txt_ROOM_TYPES[Roo_EditingRoom->Type]);
     }
  }

/*****************************************************************************/
/********************** Change the short name of a room **********************/
/*****************************************************************************/

void Roo_RenameRoomShrt (void)
  {
   /***** Room constructor *****/
   Roo_EditingRoomConstructor ();

   /***** Rename room *****/
   Roo_RenameRoom (Nam_SHRT_NAME);
  }

/*****************************************************************************/
/********************** Change the full name of a room ***********************/
/*****************************************************************************/

void Roo_RenameRoomFull (void)
  {
   /***** Room constructor *****/
   Roo_EditingRoomConstructor ();

   /***** Rename room *****/
   Roo_RenameRoom (Nam_FULL_NAME);
  }

/*****************************************************************************/
/************************* Change the name of a room *************************/
/*****************************************************************************/

static void Roo_RenameRoom (Nam_ShrtOrFullName_t ShrtOrFull)
  {
   extern const char *Nam_Fields[Nam_NUM_SHRT_FULL_NAMES];
   extern unsigned Nam_MaxBytes[Nam_NUM_SHRT_FULL_NAMES];
   extern const char *Txt_X_already_exists;
   extern const char *Txt_The_room_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   char *CurrentName[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = Roo_EditingRoom->ShrtName,
      [Nam_FULL_NAME] = Roo_EditingRoom->FullName,
     };
   char NewName[Nam_MAX_BYTES_FULL_NAME + 1];

   /***** Get parameters from form *****/
   /* Get room code */
   Roo_EditingRoom->RooCod = ParCod_GetAndCheckPar (ParCod_Roo);

   /* Get the new name for the room */
   Nam_GetParShrtOrFullName (ShrtOrFull,NewName);

   /***** Get from the database the old names of the room *****/
   Roo_GetRoomDataByCod (Roo_EditingRoom);

   /***** Check if new name is empty *****/
   if (NewName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentName[ShrtOrFull],NewName))	// Different names
        {
         /***** If room was in database... *****/
         if (Roo_DB_CheckIfRoomNameExists (Nam_Fields[ShrtOrFull],
                                           NewName,Roo_EditingRoom->RooCod,
                                           Gbl.Hierarchy.Node[Hie_CTR].HieCod,
                                           0))	// Unused
            Ale_CreateAlert (Ale_WARNING,NULL,Txt_X_already_exists,
                             NewName);
         else
           {
            /* Update the table changing old name by new name */
            Roo_DB_UpdateRoomName (Roo_EditingRoom->RooCod,
        			   Nam_Fields[ShrtOrFull],NewName);

            /* Write message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_room_X_has_been_renamed_as_Y,
                             CurrentName[ShrtOrFull],NewName);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_X_has_not_changed,
                          CurrentName[ShrtOrFull]);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update room name *****/
   Str_Copy (CurrentName[ShrtOrFull],NewName,
	     Nam_MaxBytes[ShrtOrFull]);
  }

/*****************************************************************************/
/********************* Change sitting capacity of a room *********************/
/*****************************************************************************/

void Roo_ChangeCapacity (void)
  {
   extern const char *Txt_The_capacity_of_room_X_has_not_changed;
   extern const char *Txt_The_room_X_does_not_have_a_limited_capacity_now;
   extern const char *Txt_The_capacity_of_room_X_is_now_Y;
   unsigned NewCapacity;

   /***** Room constructor *****/
   Roo_EditingRoomConstructor ();

   /***** Get parameters from form *****/
   /* Get room code */
   Roo_EditingRoom->RooCod = ParCod_GetAndCheckPar (ParCod_Roo);

   /* Get the seating capacity of the room */
   NewCapacity = (unsigned)
	          Par_GetParUnsignedLong ("Capacity",
                                            0,
                                            Roo_MAX_CAPACITY,
                                            Roo_UNLIMITED_CAPACITY);

   /***** Get data of the room from database *****/
   Roo_GetRoomDataByCod (Roo_EditingRoom);

   /***** Check if the old capacity equals the new one
          (this happens when return is pressed without changes) *****/
   if (NewCapacity == Roo_EditingRoom->Capacity)
      /***** Message to show no changes made *****/
      Ale_CreateAlert (Ale_INFO,NULL,
	               Txt_The_capacity_of_room_X_has_not_changed,
		       Roo_EditingRoom->FullName);
   else
     {
      /***** Update the table of rooms changing the old capacity for the new one *****/
      Roo_DB_UpdateRoomCapacity (Roo_EditingRoom->RooCod,NewCapacity);
      Roo_EditingRoom->Capacity = NewCapacity;

      /***** Message to show the change made *****/
      if (NewCapacity > Grp_MAX_STUDENTS_IN_A_GROUP)
         Ale_CreateAlert (Ale_SUCCESS,NULL,
                          Txt_The_room_X_does_not_have_a_limited_capacity_now,
                          Roo_EditingRoom->FullName);
      else
         Ale_CreateAlert (Ale_SUCCESS,NULL,
                          Txt_The_capacity_of_room_X_is_now_Y,
                          Roo_EditingRoom->FullName,NewCapacity);
     }
  }

/*****************************************************************************/
/********************* Write seating capacity of a room **********************/
/*****************************************************************************/

static void Roo_WriteCapacity (char Str[Cns_MAX_DECIMAL_DIGITS_UINT + 1],unsigned Capacity)
  {
   if (Capacity <= Roo_MAX_CAPACITY)
      snprintf (Str,Cns_MAX_DECIMAL_DIGITS_UINT + 1,"%u",Capacity);
   else
      Str[0] = '\0';
  }

/*****************************************************************************/
/********* Show alerts after changing a room and continue editing ************/
/*****************************************************************************/

void Roo_ContEditAfterChgRoom (void)
  {
   /***** Write message to show the change made *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Roo_EditRoomsInternal ();

   /***** Room destructor *****/
   Roo_EditingRoomDestructor ();
  }

/*****************************************************************************/
/********************** Put a form to create a new room **********************/
/*****************************************************************************/

static void Roo_PutFormToCreateRoom (const struct Bld_Buildings *Buildings)
  {
   char StrCapacity[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   char MACstr[MAC_LENGTH_MAC_ADDRESS + 1];	// MAC address in xx:xx:xx:xx:xx:xx format
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Begin form to create *****/
   Frm_BeginFormTable (ActNewRoo,NULL,NULL,NULL,"TBL_SCROLL");

      /***** Write heading *****/
      Roo_PutHeadRooms ();

      HTM_TR_Begin (NULL);

	 /***** Column to remove room, disabled here *****/
	 HTM_TD_Begin ("class=\"BM\"");
	 HTM_TD_End ();

	 /***** Room code *****/
	 HTM_TD_Begin ("class=\"CODE\"");
	 HTM_TD_End ();

	 /***** Building *****/
	 HTM_TD_Begin ("class=\"CM\"");
	    Roo_PutSelectorBuilding (Roo_EditingRoom->BldCod,Buildings,
				     HTM_DONT_SUBMIT_ON_CHANGE);
	 HTM_TD_End ();

	 /***** Floor *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_LONG ("Floor",(long) INT_MIN,(long) INT_MAX,(long) Roo_EditingRoom->Floor,
			    HTM_ENABLED,HTM_DONT_SUBMIT_ON_CHANGE,
			    "class=\"INPUT_LONG INPUT_%s\"",The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Room type *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    Roo_PutSelectorType (Roo_EditingRoom->Type,
				 HTM_DONT_SUBMIT_ON_CHANGE);
	 HTM_TD_End ();

	 /***** Room short name and full name *****/
	 Names[Nam_SHRT_NAME] = Roo_EditingRoom->ShrtName;
	 Names[Nam_FULL_NAME] = Roo_EditingRoom->FullName;
	 Nam_NewShortAndFullNames (Names);

	 /***** Seating capacity *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    Roo_WriteCapacity (StrCapacity,Roo_EditingRoom->Capacity);
	    HTM_INPUT_TEXT ("Capacity",Cns_MAX_DECIMAL_DIGITS_UINT,StrCapacity,
			    HTM_ENABLED,HTM_DONT_SUBMIT_ON_CHANGE,
			    "size=\"3\" class=\"INPUT_%s\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

	 /***** MAC address *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    MAC_MACnumToMACstr (Roo_EditingRoom->MACnum,MACstr);
	    HTM_INPUT_TEXT ("MAC",MAC_LENGTH_MAC_ADDRESS,MACstr,
			    HTM_ENABLED,HTM_DONT_SUBMIT_ON_CHANGE,
			    "size=\"8\" class=\"INPUT_%s\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE_BUTTON);
  }

/*****************************************************************************/
/******************** Write header with fields of a room *********************/
/*****************************************************************************/

static void Roo_PutHeadRooms (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Building;
   extern const char *Txt_Floor;
   extern const char *Txt_Type;
   extern const char *Txt_Short_name;
   extern const char *Txt_Full_name;
   extern const char *Txt_Capacity_OF_A_ROOM;
   extern const char *Txt_MAC_address;

   HTM_TR_Begin (NULL);
      HTM_TH_Span (NULL             ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH (Txt_Code              ,HTM_HEAD_CENTER);
      HTM_TH (Txt_Building          ,HTM_HEAD_CENTER);
      HTM_TH (Txt_Floor             ,HTM_HEAD_LEFT  );
      HTM_TH (Txt_Type              ,HTM_HEAD_LEFT  );
      HTM_TH (Txt_Short_name        ,HTM_HEAD_LEFT  );
      HTM_TH (Txt_Full_name         ,HTM_HEAD_LEFT  );
      HTM_TH (Txt_Capacity_OF_A_ROOM,HTM_HEAD_LEFT  );
      HTM_TH (Txt_MAC_address       ,HTM_HEAD_LEFT  );
   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Receive form to create a new room ***********************/
/*****************************************************************************/

void Roo_ReceiveNewRoom (void)
  {
   extern const char *Txt_Created_new_room_X;
   char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Room constructor *****/
   Roo_EditingRoomConstructor ();

   /***** Get parameters from form *****/
   /* Get room building, floor and type */
   Roo_EditingRoom->BldCod = ParCod_GetAndCheckPar (ParCod_Bld);
   Roo_EditingRoom->Floor  = Roo_GetParFloor ();
   Roo_EditingRoom->Type   = Roo_GetParType ();

   /* Get room short name and full name */
   Names[Nam_SHRT_NAME] = Roo_EditingRoom->ShrtName;
   Names[Nam_FULL_NAME] = Roo_EditingRoom->FullName;
   Nam_GetParsShrtAndFullName (Names);

   /* Get seating capacity */
   Roo_EditingRoom->Capacity = (unsigned)
	                       Par_GetParUnsignedLong ("Capacity",
                                                       0,
                                                       Roo_MAX_CAPACITY,
                                                       Roo_UNLIMITED_CAPACITY);

   /* Get MAC address */
   Roo_EditingRoom->MACnum = MAC_GetMACnumFromForm ("MAC");

   if (Roo_EditingRoom->ShrtName[0] &&
       Roo_EditingRoom->FullName[0])	// If there's a room name
     {
      /***** If name of room was not in database... *****/
      if (!Nam_CheckIfNameExists (Roo_DB_CheckIfRoomNameExists,
				  (const char **) Names,
				  -1L,
				  Gbl.Hierarchy.Node[Hie_CTR].HieCod,
				  0))	// Unused
        {
         Roo_CreateRoom (Roo_EditingRoom);
	 Ale_CreateAlert (Ale_SUCCESS,NULL,Txt_Created_new_room_X,
			  Names[Nam_FULL_NAME]);
        }
     }
   else	// If there is not a room name
      Ale_CreateAlertYouMustSpecifyShrtNameAndFullName ();
  }

/*****************************************************************************/
/**************************** Create a new room ******************************/
/*****************************************************************************/

static void Roo_CreateRoom (struct Roo_Room *Room)
  {
   /***** Correct wrong room type *****/
   if ((unsigned) Room->Type >= Roo_NUM_TYPES)
      Room->Type = Roo_NO_TYPE;

   /***** Create a new room *****/
   Room->RooCod = Roo_DB_CreateRoom (Gbl.Hierarchy.Node[Hie_CTR].HieCod,Room);

   /***** Create MAC address *****/
   if (Room->MACnum)
      Roo_DB_CreateMACAddress (Room->RooCod,Room->MACnum);
  }

/*****************************************************************************/
/************************** Room constructor/destructor **********************/
/*****************************************************************************/

static void Roo_EditingRoomConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Roo_EditingRoom != NULL)
      Err_WrongRoomExit ();

   /***** Allocate memory for room *****/
   if ((Roo_EditingRoom = malloc (sizeof (*Roo_EditingRoom))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Reset room *****/
   Roo_EditingRoom->RooCod         = -1L;
   Roo_EditingRoom->CtrCod         = -1L;
   Roo_EditingRoom->BldCod         = -1L;
   Roo_EditingRoom->BldShrtName[0] = '\0';
   Roo_EditingRoom->Floor          = 0;
   Roo_EditingRoom->ShrtName[0]    = '\0';
   Roo_EditingRoom->FullName[0]    = '\0';
   Roo_EditingRoom->Capacity       = Roo_UNLIMITED_CAPACITY;
   Roo_EditingRoom->MACnum         = 0ULL;
  }

static void Roo_EditingRoomDestructor (void)
  {
   /***** Free memory used for room *****/
   if (Roo_EditingRoom != NULL)
     {
      free (Roo_EditingRoom);
      Roo_EditingRoom = NULL;
     }
  }
