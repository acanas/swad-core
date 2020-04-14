// swad_room.c: classrooms, laboratories, offices or other rooms in a centre

/*
    SWAD (Shared Workspace At a Distance),
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

#include <malloc.h>		// For calloc, free
#include <stddef.h>		// For NULL
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_room.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static struct Roo_Room *Roo_EditingRoom = NULL;	// Static variable to keep the room being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static Roo_Order_t Roo_GetParamRoomOrder (void);
static bool Roo_CheckIfICanCreateRooms (void);
static void Roo_PutIconsListingRooms (__attribute__((unused)) void *Args);
static void Roo_PutIconToEditRooms (void);
static void Roo_PutIconsEditingRooms (__attribute__((unused)) void *Args);

static void Roo_EditRoomsInternal (void);

static void Roo_ListRoomsForEdition (const struct Roo_Rooms *Rooms);
static void Roo_PutParamRooCod (long RooCod);

static void Roo_RenameRoom (Cns_ShrtOrFullName_t ShrtOrFullName);
static bool Roo_CheckIfRoomNameExists (const char *FieldName,const char *Name,long RooCod);
static void Roo_UpdateRoomNameDB (long RooCod,const char *FieldName,const char *NewRoomName);

static void Roo_WriteCapacity (char Str[Cns_MAX_DECIMAL_DIGITS_UINT + 1],unsigned Capacity);

static void Roo_PutFormToCreateRoom (void);
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
/**************************** List all the rooms *****************************/
/*****************************************************************************/

void Roo_SeeRooms (void)
  {
   extern const char *Hlp_CENTRE_Rooms;
   extern const char *Txt_Rooms;
   extern const char *Txt_ROOMS_HELP_ORDER[Roo_NUM_ORDERS];
   extern const char *Txt_ROOMS_ORDER[Roo_NUM_ORDERS];
   extern const char *Txt_New_room;
   struct Roo_Rooms Rooms;
   Roo_Order_t Order;
   unsigned NumRoom;
   unsigned RowEvenOdd;
   char StrCapacity[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Ctr.CtrCod <= 0)		// No centre selected
      return;

   /***** Reset rooms context *****/
   Roo_ResetRooms (&Rooms);

   /***** Get parameter with the type of order in the list of rooms *****/
   Rooms.SelectedOrder = Roo_GetParamRoomOrder ();

   /***** Get list of rooms *****/
   Roo_GetListRooms (&Rooms,Roo_ALL_DATA);

   /***** Table head *****/
   Box_BoxBegin (NULL,Txt_Rooms,
                 Roo_PutIconsListingRooms,NULL,
		 Hlp_CENTRE_Rooms,Box_NOT_CLOSABLE);
   HTM_TABLE_BeginWideMarginPadding (2);
   HTM_TR_Begin (NULL);
   for (Order  = (Roo_Order_t) 0;
	Order <= (Roo_Order_t) (Roo_NUM_ORDERS - 1);
	Order++)
     {
      HTM_TH_Begin (1,1,"LM");
      Frm_StartForm (ActSeeRoo);
      Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
      HTM_BUTTON_SUBMIT_Begin (Txt_ROOMS_HELP_ORDER[Order],"BT_LINK TIT_TBL",NULL);
      if (Order == Rooms.SelectedOrder)
	 HTM_U_Begin ();
      HTM_Txt (Txt_ROOMS_ORDER[Order]);
      if (Order == Rooms.SelectedOrder)
	 HTM_U_End ();
      HTM_BUTTON_End ();
      Frm_EndForm ();
      HTM_TH_End ();
     }
   HTM_TR_End ();

   /***** Write list of rooms *****/
   for (NumRoom = 0, RowEvenOdd = 1;
	NumRoom < Rooms.Num;
	NumRoom++, RowEvenOdd = 1 - RowEvenOdd)
     {
      HTM_TR_Begin (NULL);

      /* Short name */
      HTM_TD_Begin ("class=\"DAT LM %s\"",Gbl.ColorRows[RowEvenOdd]);
      HTM_Txt (Rooms.Lst[NumRoom].ShrtName);
      HTM_TD_End ();

      /* Full name */
      HTM_TD_Begin ("class=\"DAT LM %s\"",Gbl.ColorRows[RowEvenOdd]);
      HTM_Txt (Rooms.Lst[NumRoom].FullName);
      HTM_TD_End ();

      /* Capacity */
      HTM_TD_Begin ("class=\"DAT RM %s\"",Gbl.ColorRows[RowEvenOdd]);
      Roo_WriteCapacity (StrCapacity,Rooms.Lst[NumRoom].Capacity);
      HTM_Txt (StrCapacity);
      HTM_TD_End ();

      /* Location */
      HTM_TD_Begin ("class=\"DAT LM %s\"",Gbl.ColorRows[RowEvenOdd]);
      HTM_Txt (Rooms.Lst[NumRoom].Location);
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Button to create room *****/
   if (Roo_CheckIfICanCreateRooms ())
     {
      Frm_StartForm (ActEdiRoo);
      Btn_PutConfirmButton (Txt_New_room);
      Frm_EndForm ();
     }

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of rooms *****/
   Roo_FreeListRooms (&Rooms);
  }

/*****************************************************************************/
/*********** Get parameter with the type or order in list of rooms ***********/
/*****************************************************************************/

static Roo_Order_t Roo_GetParamRoomOrder (void)
  {
   return (Roo_Order_t) Par_GetParToUnsignedLong ("Order",
						  0,
						  Roo_NUM_ORDERS - 1,
						  (unsigned long) Roo_ORDER_DEFAULT);
  }

/*****************************************************************************/
/*********************** Check if I can create rooms *************************/
/*****************************************************************************/

static bool Roo_CheckIfICanCreateRooms (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM);
  }

/*****************************************************************************/
/****************** Put contextual icons in list of rooms ********************/
/*****************************************************************************/

static void Roo_PutIconsListingRooms (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit rooms *****/
   if (Roo_CheckIfICanCreateRooms ())
      Roo_PutIconToEditRooms ();
  }

/*****************************************************************************/
/********************** Put a link (form) to edit rooms **********************/
/*****************************************************************************/

static void Roo_PutIconToEditRooms (void)
  {
   Ico_PutContextualIconToEdit (ActEdiRoo,NULL,
                                NULL,NULL);
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
   extern const char *Hlp_CENTRE_Rooms_edit;
   extern const char *Txt_Rooms;
   struct Roo_Rooms Rooms;

   /***** Reset rooms context *****/
   Roo_ResetRooms (&Rooms);

   /***** Get list of rooms *****/
   Roo_GetListRooms (&Rooms,Roo_ALL_DATA);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Rooms,
                 Roo_PutIconsEditingRooms,NULL,
                 Hlp_CENTRE_Rooms_edit,Box_NOT_CLOSABLE);

   /***** Put a form to create a new room *****/
   Roo_PutFormToCreateRoom ();

   /***** Forms to edit current rooms *****/
   if (Rooms.Num)
      Roo_ListRoomsForEdition (&Rooms);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of rooms *****/
   Roo_FreeListRooms (&Rooms);
  }

/*****************************************************************************/
/***************** Put contextual icons in edition of rooms ******************/
/*****************************************************************************/

static void Roo_PutIconsEditingRooms (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view rooms *****/
   Roo_PutIconToViewRooms ();
  }

/*****************************************************************************/
/************************** Put icon to view rooms ***************************/
/*****************************************************************************/

void Roo_PutIconToViewRooms (void)
  {
   extern const char *Txt_Rooms;

   Lay_PutContextualLinkOnlyIcon (ActSeeRoo,NULL,
                                  NULL,NULL,
                                  "classroom.svg",
                                  Txt_Rooms);
  }

/*****************************************************************************/
/**************************** List all the rooms *****************************/
/*****************************************************************************/

void Roo_GetListRooms (struct Roo_Rooms *Rooms,
                       Roo_WhichData_t WhichData)
  {
   static const char *OrderBySubQuery[Roo_NUM_ORDERS] =
     {
      [Roo_ORDER_BY_SHRT_NAME] = "ShortName",
      [Roo_ORDER_BY_FULL_NAME] = "FullName",
      [Roo_ORDER_BY_CAPACITY ] = "Capacity DESC,ShortName",
      [Roo_ORDER_BY_LOCATION ] = "Location,ShortName",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumRoom;
   struct Roo_Room *Room;

   /***** Get rooms from database *****/
   switch (WhichData)
     {
      case Roo_ALL_DATA:
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get rooms",
				   "SELECT RooCod,"
					  "ShortName,"
					  "FullName,"
					  "Capacity,"
					  "Location"
				   " FROM rooms"
				   " WHERE CtrCod=%ld"
				   " ORDER BY %s",
				   Gbl.Hierarchy.Ctr.CtrCod,
				   OrderBySubQuery[Rooms->SelectedOrder]);
	 break;
      case Roo_ONLY_SHRT_NAME:
      default:
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get rooms",
				   "SELECT RooCod,"
					  "ShortName"
				   " FROM rooms"
				   " WHERE CtrCod=%ld"
				   " ORDER BY ShortName",
				   Gbl.Hierarchy.Ctr.CtrCod);
	 break;
     }

   /***** Count number of rows in result *****/
   if (NumRows) // Rooms found...
     {
      Rooms->Num = (unsigned) NumRows;

      /***** Create list with courses in centre *****/
      if ((Rooms->Lst = (struct Roo_Room *)
	                     calloc (NumRows,
	                             sizeof (struct Roo_Room))) == NULL)
          Lay_NotEnoughMemoryExit ();

      /***** Get the rooms *****/
      for (NumRoom = 0;
	   NumRoom < Rooms->Num;
	   NumRoom++)
        {
         Room = &Rooms->Lst[NumRoom];

         /* Get next room */
         row = mysql_fetch_row (mysql_res);

         /* Get room code (row[0]) */
         if ((Room->RooCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of room.");

         /* Get the short name of the room (row[1]) */
         Str_Copy (Room->ShrtName,row[1],
                   Roo_MAX_BYTES_SHRT_NAME);

         if (WhichData == Roo_ALL_DATA)
           {
	    /* Get the full name of the room (row[2]) */
	    Str_Copy (Room->FullName,row[2],
		      Roo_MAX_BYTES_FULL_NAME);

	    /* Get seating capacity in this room (row[3]) */
	    if (sscanf (row[3],"%u",&Room->Capacity) != 1)
	       Room->Capacity = Roo_UNLIMITED_CAPACITY;

	    /* Get the full name of the room (row[4]) */
	    Str_Copy (Room->Location,row[4],
		      Roo_MAX_BYTES_LOCATION);
           }
        }
     }
   else
      Rooms->Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************************** Get room full name ****************************/
/*****************************************************************************/

void Roo_GetDataOfRoomByCod (struct Roo_Room *Room)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Clear data *****/
   Room->ShrtName[0] = '\0';
   Room->FullName[0] = '\0';
   Room->Capacity = Roo_UNLIMITED_CAPACITY;
   Room->Location[0] = '\0';

   /***** Check if room code is correct *****/
   if (Room->RooCod > 0)
     {
      /***** Get data of a room from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get data of a room",
			        "SELECT ShortName,"
				       "FullName,"
				       "Capacity,"
				       "Location"
				" FROM rooms"
				" WHERE RooCod=%ld",
				Room->RooCod);

      /***** Count number of rows in result *****/
      if (NumRows) // Room found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get the short name of the room (row[0]) */
         Str_Copy (Room->ShrtName,row[0],
                   Roo_MAX_BYTES_SHRT_NAME);

         /* Get the full name of the room (row[1]) */
         Str_Copy (Room->FullName,row[1],
                   Roo_MAX_BYTES_FULL_NAME);

         /* Get seating capacity in this room (row[2]) */
         if (sscanf (row[2],"%u",&Room->Capacity) != 1)
            Room->Capacity = Roo_UNLIMITED_CAPACITY;

         /* Get the location of the room (row[3]) */
         Str_Copy (Room->Location,row[3],
                   Roo_MAX_BYTES_LOCATION);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
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
/*************************** List all the rooms ******************************/
/*****************************************************************************/

static void Roo_ListRoomsForEdition (const struct Roo_Rooms *Rooms)
  {
   unsigned NumCla;
   struct Roo_Room *Room;
   char StrCapacity[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Write heading *****/
   HTM_TABLE_BeginWidePadding (2);
   Roo_PutHeadRooms ();

   /***** Write all the rooms *****/
   for (NumCla = 0;
	NumCla < Rooms->Num;
	NumCla++)
     {
      Room = &Rooms->Lst[NumCla];

      HTM_TR_Begin (NULL);

      /* Put icon to remove room */
      HTM_TD_Begin ("class=\"BM\"");
      Frm_StartForm (ActRemRoo);
      Roo_PutParamRooCod (Room->RooCod);
      Ico_PutIconRemove ();
      Frm_EndForm ();
      HTM_TD_End ();

      /* Room code */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_Long (Room->RooCod);
      HTM_TD_End ();

      /* Room short name */
      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartForm (ActRenRooSho);
      Roo_PutParamRooCod (Room->RooCod);
      HTM_INPUT_TEXT ("ShortName",Roo_MAX_CHARS_SHRT_NAME,Room->ShrtName,true,
		      "size=\"10\" class=\"INPUT_SHORT_NAME\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Room full name */
      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartForm (ActRenRooFul);
      Roo_PutParamRooCod (Room->RooCod);
      HTM_INPUT_TEXT ("FullName",Roo_MAX_CHARS_FULL_NAME,Room->FullName,true,
		      "size=\"20\" class=\"INPUT_FULL_NAME\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Seating capacity */
      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartForm (ActChgRooMaxUsr);
      Roo_PutParamRooCod (Room->RooCod);
      Roo_WriteCapacity (StrCapacity,Room->Capacity);
      HTM_INPUT_TEXT ("Capacity",Cns_MAX_DECIMAL_DIGITS_UINT,StrCapacity,true,
		      "size=\"3\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Room location */
      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartForm (ActRenRooLoc);
      Roo_PutParamRooCod (Room->RooCod);
      HTM_INPUT_TEXT ("Location",Roo_MAX_CHARS_LOCATION,Room->Location,true,
		      "size=\"15\" class=\"INPUT_FULL_NAME\"");
      Frm_EndForm ();
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********************* Write parameter with code of room *********************/
/*****************************************************************************/

static void Roo_PutParamRooCod (long RooCod)
  {
   Par_PutHiddenParamLong (NULL,"RooCod",RooCod);
  }

/*****************************************************************************/
/********************** Get parameter with code of room **********************/
/*****************************************************************************/

long Roo_GetParamRooCod (void)
  {
   /***** Get code of room *****/
   return Par_GetParToLong ("RooCod");
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
   if ((Roo_EditingRoom->RooCod = Roo_GetParamRooCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of room is missing.");

   /***** Get data of the room from database *****/
   Roo_GetDataOfRoomByCod (Roo_EditingRoom);

   /***** Update groups assigned to this room *****/
   DB_QueryUPDATE ("can not update room in groups",
		   "UPDATE crs_grp SET RooCod=0"	// 0 means another room
		   " WHERE RooCod=%ld",
		   Roo_EditingRoom->RooCod);

   /***** Remove room *****/
   DB_QueryDELETE ("can not remove a room",
		   "DELETE FROM rooms WHERE RooCod=%ld",
		   Roo_EditingRoom->RooCod);

   /***** Create message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_Room_X_removed,
	            Roo_EditingRoom->FullName);
  }

/*****************************************************************************/
/********************** Remove all rooms in a centre *************************/
/*****************************************************************************/

void Roo_RemoveAllRoomsInCtr (long CtrCod)
  {
   /***** Remove all rooms in centre *****/
   DB_QueryDELETE ("can not remove rooms",
		   "DELETE FROM rooms"
                   " WHERE CtrCod=%ld",
		   CtrCod);
  }

/*****************************************************************************/
/********************** Change the short name of a room **********************/
/*****************************************************************************/

void Roo_RenameRoomShort (void)
  {
   /***** Room constructor *****/
   Roo_EditingRoomConstructor ();

   /***** Rename room *****/
   Roo_RenameRoom (Cns_SHRT_NAME);
  }

/*****************************************************************************/
/********************** Change the full name of a room ***********************/
/*****************************************************************************/

void Roo_RenameRoomFull (void)
  {
   /***** Room constructor *****/
   Roo_EditingRoomConstructor ();

   /***** Rename room *****/
   Roo_RenameRoom (Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************* Change the name of a room *************************/
/*****************************************************************************/

static void Roo_RenameRoom (Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_The_room_X_already_exists;
   extern const char *Txt_The_room_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_room_X_has_not_changed;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentClaName = NULL;		// Initialized to avoid warning
   char NewClaName[Roo_MAX_BYTES_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Roo_MAX_BYTES_SHRT_NAME;
         CurrentClaName = Roo_EditingRoom->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Roo_MAX_BYTES_FULL_NAME;
         CurrentClaName = Roo_EditingRoom->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the room */
   if ((Roo_EditingRoom->RooCod = Roo_GetParamRooCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of room is missing.");

   /* Get the new name for the room */
   Par_GetParToText (ParamName,NewClaName,MaxBytes);

   /***** Get from the database the old names of the room *****/
   Roo_GetDataOfRoomByCod (Roo_EditingRoom);

   /***** Check if new name is empty *****/
   if (NewClaName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentClaName,NewClaName))	// Different names
        {
         /***** If room was in database... *****/
         if (Roo_CheckIfRoomNameExists (ParamName,NewClaName,Roo_EditingRoom->RooCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_room_X_already_exists,
                             NewClaName);
         else
           {
            /* Update the table changing old name by new name */
            Roo_UpdateRoomNameDB (Roo_EditingRoom->RooCod,FieldName,NewClaName);

            /* Write message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_room_X_has_been_renamed_as_Y,
                             CurrentClaName,NewClaName);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_of_the_room_X_has_not_changed,
                          CurrentClaName);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update room name *****/
   Str_Copy (CurrentClaName,NewClaName,
             MaxBytes);
  }

/*****************************************************************************/
/********************** Check if the name of room exists *********************/
/*****************************************************************************/

static bool Roo_CheckIfRoomNameExists (const char *FieldName,const char *Name,long RooCod)
  {
   /***** Get number of rooms with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of a room"
			  " already existed",
			  "SELECT COUNT(*) FROM rooms"
			  " WHERE CtrCod=%ld"
			  " AND %s='%s' AND RooCod<>%ld",
			  Gbl.Hierarchy.Ctr.CtrCod,
			  FieldName,Name,RooCod) != 0);
  }

/*****************************************************************************/
/******************** Update room name in table of rooms *********************/
/*****************************************************************************/

static void Roo_UpdateRoomNameDB (long RooCod,const char *FieldName,const char *NewRoomName)
  {
   /***** Update room changing old name by new name */
   DB_QueryUPDATE ("can not update the name of a room",
		   "UPDATE rooms SET %s='%s' WHERE RooCod=%ld",
		   FieldName,NewRoomName,RooCod);
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
   /* Get the code of the room */
   if ((Roo_EditingRoom->RooCod = Roo_GetParamRooCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of room is missing.");

   /* Get the seating capacity of the room */
   NewCapacity = (unsigned)
	          Par_GetParToUnsignedLong ("Capacity",
                                            0,
                                            Roo_MAX_CAPACITY,
                                            Roo_UNLIMITED_CAPACITY);

   /***** Get data of the room from database *****/
   Roo_GetDataOfRoomByCod (Roo_EditingRoom);

   /***** Check if the old capacity equals the new one
          (this happens when return is pressed without changes) *****/
   if (Roo_EditingRoom->Capacity == NewCapacity)
      /***** Message to show no changes made *****/
      Ale_CreateAlert (Ale_INFO,NULL,
	               Txt_The_capacity_of_room_X_has_not_changed,
		       Roo_EditingRoom->FullName);
   else
     {
      /***** Update the table of groups changing the old capacity to the new *****/
      DB_QueryUPDATE ("can not update the capacity of a room",
		      "UPDATE rooms SET Capacity=%u WHERE RooCod=%ld",
                      NewCapacity,Roo_EditingRoom->RooCod);
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
      snprintf (Str,Cns_MAX_DECIMAL_DIGITS_UINT + 1,
		"%u",
		Capacity);
   else
      Str[0] = '\0';
  }

/*****************************************************************************/
/********************** Change the location of a room ************************/
/*****************************************************************************/

void Roo_ChangeRoomLocation (void)
  {
   extern const char *Txt_The_location_of_the_room_X_has_changed_to_Y;
   extern const char *Txt_The_location_of_the_room_X_has_not_changed;
   char NewLocation[Roo_MAX_BYTES_FULL_NAME + 1];

   /***** Room constructor *****/
   Roo_EditingRoomConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the room */
   if ((Roo_EditingRoom->RooCod = Roo_GetParamRooCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of room is missing.");

   /* Get the new location for the room */
   Par_GetParToText ("Location",NewLocation,Roo_MAX_BYTES_LOCATION);

   /***** Get from the database the old location of the room *****/
   Roo_GetDataOfRoomByCod (Roo_EditingRoom);

   /***** Check if old and new locations are the same
	  (this happens when return is pressed without changes) *****/
   if (strcmp (Roo_EditingRoom->Location,NewLocation))	// Different locations
     {
      /* Update the table changing old name by new name */
      Roo_UpdateRoomNameDB (Roo_EditingRoom->RooCod,"Location",NewLocation);
      Str_Copy (Roo_EditingRoom->Location,NewLocation,
		Roo_MAX_BYTES_LOCATION);

      /* Write message to show the change made */
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_location_of_the_room_X_has_changed_to_Y,
		       Roo_EditingRoom->FullName,NewLocation);
     }
   else	// The same location
      Ale_CreateAlert (Ale_INFO,NULL,
	               Txt_The_location_of_the_room_X_has_not_changed,
		       Roo_EditingRoom->FullName);
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

static void Roo_PutFormToCreateRoom (void)
  {
   extern const char *Txt_New_room;
   extern const char *Txt_Create_room;
   char StrCapacity[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Begin form *****/
   Frm_StartForm (ActNewRoo);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_New_room,
                      NULL,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Roo_PutHeadRooms ();

   HTM_TR_Begin (NULL);

   /***** Column to remove room, disabled here *****/
   HTM_TD_Begin ("class=\"BM\"");
   HTM_TD_End ();

   /***** Room code *****/
   HTM_TD_Begin ("class=\"CODE\"");
   HTM_TD_End ();

   /***** Room short name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("ShortName",Roo_MAX_CHARS_SHRT_NAME,Roo_EditingRoom->ShrtName,false,
		   "size=\"10\" class=\"INPUT_SHORT_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Room full name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("FullName",Roo_MAX_CHARS_FULL_NAME,Roo_EditingRoom->FullName,false,
		   "size=\"20\" class=\"INPUT_FULL_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Seating capacity *****/
   HTM_TD_Begin ("class=\"LM\"");
   Roo_WriteCapacity (StrCapacity,Roo_EditingRoom->Capacity);
   HTM_INPUT_TEXT ("Capacity",Cns_MAX_DECIMAL_DIGITS_UINT,StrCapacity,false,
		   "size=\"3\"");
   HTM_TD_End ();

   /***** Room location *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("Location",Roo_MAX_CHARS_LOCATION,Roo_EditingRoom->Location,false,
		   "size=\"15\" class=\"INPUT_FULL_NAME\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_room);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Write header with fields of a room *********************/
/*****************************************************************************/

static void Roo_PutHeadRooms (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Short_name;
   extern const char *Txt_Full_name;
   extern const char *Txt_Capacity_OF_A_ROOM;
   extern const char *Txt_Location;

   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"BM",NULL);
   HTM_TH (1,1,"RM",Txt_Code);
   HTM_TH (1,1,"LM",Txt_Short_name);
   HTM_TH (1,1,"LM",Txt_Full_name);
   HTM_TH (1,1,"LM",Txt_Capacity_OF_A_ROOM);
   HTM_TH (1,1,"LM",Txt_Location);

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Receive form to create a new room ***********************/
/*****************************************************************************/

void Roo_RecFormNewRoom (void)
  {
   extern const char *Txt_The_room_X_already_exists;
   extern const char *Txt_Created_new_room_X;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_room;

   /***** Room constructor *****/
   Roo_EditingRoomConstructor ();

   /***** Get parameters from form *****/
   /* Get room short name */
   Par_GetParToText ("ShortName",Roo_EditingRoom->ShrtName,Roo_MAX_BYTES_SHRT_NAME);

   /* Get room full name */
   Par_GetParToText ("FullName",Roo_EditingRoom->FullName,Roo_MAX_BYTES_FULL_NAME);

   /* Get seating capacity */
   Roo_EditingRoom->Capacity = (unsigned)
	                      Par_GetParToUnsignedLong ("Capacity",
                                                        0,
                                                        Roo_MAX_CAPACITY,
                                                        Roo_UNLIMITED_CAPACITY);

   /* Get room location */
   Par_GetParToText ("Location",Roo_EditingRoom->Location,Roo_MAX_BYTES_LOCATION);

   if (Roo_EditingRoom->ShrtName[0] &&
       Roo_EditingRoom->FullName[0])	// If there's a room name
     {
      /***** If name of room was in database... *****/
      if (Roo_CheckIfRoomNameExists ("ShortName",Roo_EditingRoom->ShrtName,-1L))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_room_X_already_exists,
                          Roo_EditingRoom->ShrtName);
      else if (Roo_CheckIfRoomNameExists ("FullName",Roo_EditingRoom->FullName,-1L))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_room_X_already_exists,
                          Roo_EditingRoom->FullName);
      else	// Add new room to database
        {
         Roo_CreateRoom (Roo_EditingRoom);
	 Ale_CreateAlert (Ale_SUCCESS,NULL,
	                  Txt_Created_new_room_X,
			  Roo_EditingRoom->FullName);
        }
     }
   else	// If there is not a room name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_room);
  }

/*****************************************************************************/
/**************************** Create a new room ******************************/
/*****************************************************************************/

static void Roo_CreateRoom (struct Roo_Room *Room)
  {
   /***** Create a new room *****/
   DB_QueryINSERT ("can not create room",
		   "INSERT INTO rooms"
		   " (CtrCod,ShortName,FullName,Capacity,Location)"
		   " VALUES"
		   " (%ld,'%s','%s',%u,'%s')",
                   Gbl.Hierarchy.Ctr.CtrCod,
		   Room->ShrtName,Room->FullName,Room->Capacity,Room->Location);
  }

/*****************************************************************************/
/************************** Room constructor/destructor **********************/
/*****************************************************************************/

static void Roo_EditingRoomConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Roo_EditingRoom != NULL)
      Lay_ShowErrorAndExit ("Error initializing room.");

   /***** Allocate memory for room *****/
   if ((Roo_EditingRoom = (struct Roo_Room *) malloc (sizeof (struct Roo_Room))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for room.");

   /***** Reset room *****/
   Roo_EditingRoom->RooCod      = -1L;
   Roo_EditingRoom->InsCod      = -1L;
   Roo_EditingRoom->ShrtName[0] = '\0';
   Roo_EditingRoom->FullName[0] = '\0';
   Roo_EditingRoom->Capacity    = Roo_UNLIMITED_CAPACITY;
   Roo_EditingRoom->Location[0] = '\0';
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
