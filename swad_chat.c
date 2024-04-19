// swad_chat.c: chat

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

#include <stddef.h>		// For NULL
#include <stdbool.h>		// For boolean type
#include <string.h>

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_chat.h"
#include "swad_chat_database.h"
#include "swad_config.h"
#include "swad_course_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_logo.h"
#include "swad_parameter.h"
#include "swad_string.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Cht_CHAT_MAX_LEVELS 3

#define Cht_MAX_CHARS_ROOM_CODE	16	// 16, maximum number of chars of the code of a chat room
#define Cht_MAX_BYTES_ROOM_CODE	Cht_MAX_CHARS_ROOM_CODE	// 16

#define Cht_MAX_CHARS_ROOM_SHRT_NAME	(128 - 1)	// 127, maximum number of chars of the short name of a chat room
#define Cht_MAX_BYTES_ROOM_SHRT_NAME	((Cht_MAX_CHARS_ROOM_SHRT_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Cht_MAX_CHARS_ROOM_FULL_NAME	(256 - 1)	// 255, maximum number of chars of the full name of a chat room
#define Cht_MAX_BYTES_ROOM_FULL_NAME	((Cht_MAX_CHARS_ROOM_FULL_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 4095

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Cht_WriteLinkToChat1 (const char *RoomCode,const char *RoomShrtName,const char *RoomFullName,
                                  unsigned Level,Lay_LastItem_t IsLastItemInLevel[1 + Cht_CHAT_MAX_LEVELS]);
static void Cht_WriteLinkToChat2 (const char *RoomCode,const char *RoomFullName);

/*****************************************************************************/
/****************** List available whiteboard/chat rooms *********************/
/*****************************************************************************/

void Cht_ShowChatRooms (void)
  {
   extern const char *Txt_To_use_chat_you_must_have_installed_the_software_X_and_add_Y_;
   extern const char *Txt_Unfortunately_Firefox_and_Chrome_no_longer_allow_Java_to_run_;

   /***** Help message about software needed to use the whiteboard/chat *****/
   Ale_ShowAlert (Ale_INFO,Txt_To_use_chat_you_must_have_installed_the_software_X_and_add_Y_,
                  Cfg_JAVA_URL,Cfg_JAVA_NAME,
                  Cfg_PLATFORM_SERVER);

   /***** List available chat rooms *****/
   Cht_ShowListOfAvailableChatRooms ();

   Ale_ShowAlert (Ale_WARNING,Txt_Unfortunately_Firefox_and_Chrome_no_longer_allow_Java_to_run_);

   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      Cht_ShowListOfChatRoomsWithUsrs ();
  }

/*****************************************************************************/
/*********************** Show list of available chat rooms *******************/
/*****************************************************************************/

void Cht_ShowListOfAvailableChatRooms (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_Chat_rooms;
   extern const char *Txt_General;
   extern const char *Txt_SEX_PLURAL_Abc[Usr_NUM_SEXS];
   extern const char *Txt_SEX_PLURAL_abc[Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   Lay_LastItem_t IsLastItemInLevel[1 + Cht_CHAT_MAX_LEVELS];
   unsigned NumMyDeg;
   struct Hie_Node Deg;
   struct Hie_Node Crs;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   char ThisRoomCode    [Cht_MAX_BYTES_ROOM_CODE + 1];
   char ThisRoomShrtName[Cht_MAX_BYTES_ROOM_SHRT_NAME + 1];
   char ThisRoomFullName[Cht_MAX_BYTES_ROOM_FULL_NAME + 1];

   /***** Fill the list with the degrees I belong to *****/ 
   Hie_GetMyHierarchy (Hie_DEG);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Chat_rooms,NULL,NULL,NULL,Box_NOT_CLOSABLE);

      /***** Begin list *****/
      HTM_UL_Begin ("class=\"LIST_TREE\"");

	 /***** Link to chat available for all users *****/
	 IsLastItemInLevel[1] = (Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS] == Usr_BELONG ||
				 Gbl.Usrs.Me.Hierarchy[Hie_DEG].Num) ? Lay_NO_LAST_ITEM :
								       Lay_LAST_ITEM;
	 snprintf (ThisRoomFullName,sizeof (ThisRoomFullName),"%s (%s)",
		   Txt_General,Txt_SEX_PLURAL_abc[Usr_SEX_ALL]);
	 Cht_WriteLinkToChat1 ("GBL_USR",Txt_SEX_PLURAL_Abc[Usr_SEX_ALL],
			       ThisRoomFullName,1,IsLastItemInLevel);
	    Ico_PutIcon ("comments.svg",Ico_BLACK,ThisRoomFullName,"ICO16x16");
	 Cht_WriteLinkToChat2 ("GBL_USR",ThisRoomFullName);

	 IsLastItemInLevel[1] = Gbl.Usrs.Me.Hierarchy[Hie_DEG].Num ? Lay_NO_LAST_ITEM :
								     Lay_LAST_ITEM;
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_STD:
	       snprintf (ThisRoomFullName,sizeof (ThisRoomFullName),"%s (%s)",
			 Txt_General,Txt_ROLES_PLURAL_abc[Rol_STD][Usr_SEX_ALL]);
	       Cht_WriteLinkToChat1 ("GBL_STD",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD],
				     ThisRoomFullName,1,IsLastItemInLevel);
		  Ico_PutIcon ("comments.svg",Ico_BLACK,ThisRoomFullName,"ICO16x16");
	       Cht_WriteLinkToChat2 ("GBL_STD",ThisRoomFullName);
	       break;
	    case Rol_NET:
	    case Rol_TCH:
	       snprintf (ThisRoomFullName,sizeof (ThisRoomFullName),"%s (%s)",
			 Txt_General,Txt_ROLES_PLURAL_abc[Rol_TCH][Usr_SEX_ALL]);
	       Cht_WriteLinkToChat1 ("GBL_TCH",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH],
				     ThisRoomFullName,1,IsLastItemInLevel);
		  Ico_PutIcon ("comments.svg",Ico_BLACK,ThisRoomFullName,"ICO16x16");
	       Cht_WriteLinkToChat2 ("GBL_TCH",ThisRoomFullName);
	       break;
	    default:
	       break;
	   }

	 /***** Link to chat of users from my degrees *****/
	 for (NumMyDeg = 0;
	      NumMyDeg < Gbl.Usrs.Me.Hierarchy[Hie_DEG].Num;
	      NumMyDeg++)
	   {
	    /* Get data of this degree */
	    Deg.HieCod = Gbl.Usrs.Me.Hierarchy[Hie_DEG].Nodes[NumMyDeg].HieCod;
	    if (!Hie_GetDataByCod[Hie_DEG] (&Deg))
	       Err_WrongDegreeExit ();

	    /* Link to the room of this degree */
	    IsLastItemInLevel[1] = (NumMyDeg == Gbl.Usrs.Me.Hierarchy[Hie_DEG].Num - 1) ? Lay_LAST_ITEM :
											  Lay_NO_LAST_ITEM;
	    snprintf (ThisRoomCode,sizeof (ThisRoomCode),"DEG_%ld",Deg.HieCod);
	    snprintf (ThisRoomShrtName,sizeof (ThisRoomShrtName),"%s",
		      Deg.ShrtName);
	    snprintf (ThisRoomFullName,sizeof (ThisRoomFullName),"%s %s",
		      Txt_HIERARCHY_SINGUL_Abc[Hie_DEG],Deg.ShrtName);
	    Cht_WriteLinkToChat1 (ThisRoomCode,ThisRoomShrtName,
				  ThisRoomFullName,1,IsLastItemInLevel);
	       Lgo_DrawLogo (Hie_DEG,&Deg,"ICO16x16");
	    Cht_WriteLinkToChat2 (ThisRoomCode,ThisRoomFullName);

	    /* Get my courses in this degree from database */
	    NumCrss = Crs_DB_GetCrssFromUsr (&mysql_res,
	                                     Gbl.Usrs.Me.UsrDat.UsrCod,
	                                     Deg.HieCod);
	    for (NumCrs = 0;
		 NumCrs < NumCrss;
		 NumCrs++)
	      {
	       /* Get next course */
	       row = mysql_fetch_row (mysql_res);

	       /* Get course code */
	       if ((Crs.HieCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
		 {
		  /* Get data of this course */
		  Hie_GetDataByCod[Hie_CRS] (&Crs);

		  /* Link to the room of this course */
		  IsLastItemInLevel[2] = (NumCrs == NumCrss - 1) ? Lay_LAST_ITEM :
								   Lay_NO_LAST_ITEM;
		  snprintf (ThisRoomCode,sizeof (ThisRoomCode),"CRS_%ld",
			    Crs.HieCod);
		  snprintf (ThisRoomShrtName,sizeof (ThisRoomShrtName),"%s",
			    Crs.ShrtName);
		  snprintf (ThisRoomFullName,sizeof (ThisRoomFullName),"%s %s",
			    Txt_HIERARCHY_SINGUL_Abc[Hie_CRS],Crs.ShrtName);
		  Cht_WriteLinkToChat1 (ThisRoomCode,ThisRoomShrtName,
				        ThisRoomFullName,2,IsLastItemInLevel);
		     Lgo_DrawLogo (Hie_CRS,&Crs,"ICO16x16");
		  Cht_WriteLinkToChat2 (ThisRoomCode,ThisRoomFullName);
		 }
	      }

	    /***** Free structure that stores the query result *****/
	    DB_FreeMySQLResult (&mysql_res);
	   }

      /***** End list *****/
      HTM_UL_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************** Show list of chat rooms with users *******************/
/*****************************************************************************/

void Cht_ShowListOfChatRoomsWithUsrs (void)
  {
   extern const char *Txt_Rooms_with_users;
   extern const char *Txt_CHAT_Room_code;
   extern const char *Txt_Number_of_users;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRooms;
   unsigned NumRoom;

   /***** Get chat rooms with connected users from database *****/
   NumRooms = Cht_DB_GetListOfChatRoomsWithUsrs (&mysql_res);
   if (NumRooms) // If not empty chat rooms found
     {
      /***** Begin box and table *****/
      Box_BoxTableBegin (Txt_Rooms_with_users,NULL,NULL,
			 NULL,Box_NOT_CLOSABLE,2);

	 /***** Write heading *****/
	 HTM_TR_Begin (NULL);
	    HTM_TH (Txt_CHAT_Room_code ,HTM_HEAD_CENTER);
	    HTM_TH (Txt_Number_of_users,HTM_HEAD_LEFT  );
	 HTM_TR_End ();

	 /***** Loop over chat rooms *****/
	 for (NumRoom = 0;
	      NumRoom < NumRooms;
	      NumRoom++)
	   {
	    /* Get next chat room */
	    row = mysql_fetch_row (mysql_res);

	    HTM_TR_Begin (NULL);
	       HTM_TD_Txt_Left  (row[0]);
	       HTM_TD_Txt_Right (row[1]);
	    HTM_TR_End ();
	   }

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Write title and link to a chat room ********************/
/*****************************************************************************/

static void Cht_WriteLinkToChat1 (const char *RoomCode,const char *RoomShrtName,const char *RoomFullName,
                                  unsigned Level,Lay_LastItem_t IsLastItemInLevel[1 + Cht_CHAT_MAX_LEVELS])
  {
   /***** Begin list item *****/
   HTM_LI_Begin (NULL);

      Lay_IndentDependingOnLevel (Level,IsLastItemInLevel,Lay_HORIZONTAL_LINE_AT_RIGHT);

      /***** Begin form *****/
      Frm_BeginForm (ActCht);

	 Cht_WriteParsRoomCodeAndNames (RoomCode,RoomShrtName,RoomFullName);

	 /* Begin button */
	 HTM_BUTTON_Submit_Begin (RoomFullName,
	                          "class=\"BT_LINK FORM_IN_%s\"",
	                          The_GetSuffix ());
  }

static void Cht_WriteLinkToChat2 (const char *RoomCode,const char *RoomFullName)
  {
   extern const char *Txt_connected_PLURAL;
   extern const char *Txt_connected_SINGULAR;
   unsigned NumUsrsInRoom = Cht_DB_GetNumUsrsInChatRoom (RoomCode);

            /* Begin strong if room has users */
	    if (NumUsrsInRoom)
	       HTM_STRONG_Begin ();

	    /* Room name and users */
	    HTM_NBSPTxt (RoomFullName);
	    if (NumUsrsInRoom > 1)
	       HTM_TxtF (" [%d %s]",NumUsrsInRoom,Txt_connected_PLURAL);
	    else if (NumUsrsInRoom == 1)
	       HTM_TxtF (" [1 %s]",Txt_connected_SINGULAR);

            /* End strong if room has users */
	    if (NumUsrsInRoom)
	       HTM_STRONG_End ();

	 /* End button */
	 HTM_BUTTON_End ();

      /***** End form *****/
      Frm_EndForm ();

   /***** End list item *****/
   HTM_LI_End ();
  }

/*****************************************************************************/
/*** Write parameters with code and names (short and full) of a chat room ****/
/*****************************************************************************/

void Cht_WriteParsRoomCodeAndNames (const char *RoomCode,const char *RoomShrtName,const char *RoomFullName)
  {
   Par_PutParString (NULL,"RoomCode",RoomCode);
   Par_PutParString (NULL,"RoomShrtName",RoomShrtName);
   Par_PutParString (NULL,"RoomFullName",RoomFullName);
  }

/*****************************************************************************/
/******************************* Enter a chat room ***************************/
/*****************************************************************************/

// TODO: Max. bytes should be allocated dinamycally depending on number of degrees and courses
#define Cht_MAX_BYTES_ROOM_CODES      (100 * Cht_MAX_BYTES_ROOM_CODE)
#define Cht_MAX_BYTES_ROOM_SHRT_NAMES (100 * Cht_MAX_BYTES_ROOM_SHRT_NAME)
#define Cht_MAX_BYTES_ROOM_FULL_NAMES (100 * Cht_MAX_BYTES_ROOM_FULL_NAME)

void Cht_OpenChatWindow (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_SEX_PLURAL_Abc[Usr_NUM_SEXS];
   extern const char *Txt_SEX_PLURAL_abc[Usr_NUM_SEXS];
   extern const char *Txt_General;
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   char RoomCode[Cht_MAX_BYTES_ROOM_CODE + 1];
   char RoomShrtName[Cht_MAX_BYTES_ROOM_SHRT_NAME + 1];
   char RoomFullName [Cht_MAX_BYTES_ROOM_FULL_NAME + 1];
   char UsrName[Usr_MAX_BYTES_FULL_NAME + 1];
   unsigned NumMyDeg;
   unsigned NumMyCrs;
   struct Hie_Node Deg;
   struct Hie_Node Crs;
   char ThisRoomCode[Cht_MAX_BYTES_ROOM_CODE + 1];
   char ThisRoomShortName[Cht_MAX_BYTES_ROOM_SHRT_NAME + 1];
   char ThisRoomFullName [Cht_MAX_BYTES_ROOM_FULL_NAME + 1];
   char ListRoomCodes    [Cht_MAX_BYTES_ROOM_CODES      + 1];
   char ListRoomShrtNames[Cht_MAX_BYTES_ROOM_SHRT_NAMES + 1];
   char ListRoomFullNames[Cht_MAX_BYTES_ROOM_FULL_NAMES + 1];
   FILE *FileChat;
   FILE *FileOut = Fil_GetOutputFile ();

   /***** Get the code and the nombre of the room *****/
   Par_GetParText ("RoomCode",RoomCode,Cht_MAX_BYTES_ROOM_CODE);

   Par_GetParText ("RoomShrtName",RoomShrtName,Cht_MAX_BYTES_ROOM_SHRT_NAME);

   Par_GetParText ("RoomFullName",RoomFullName,Cht_MAX_BYTES_ROOM_FULL_NAME);

   if (!RoomCode[0] || !RoomShrtName[0] || !RoomFullName[0])
      Err_ShowErrorAndExit ("Wrong code or name of chat room.");
   if (strcspn (RoomCode," \t\n\r") != strlen (RoomCode)) // If RoomCode contiene espacios
      Err_ShowErrorAndExit ("Wrong code of chat room.");

   /***** Fill the lists with the degrees and courses I belong to *****/ 
   Hie_GetMyHierarchy (Hie_DEG);
   Hie_GetMyHierarchy (Hie_CRS);

   /***** Build my user's name *****/
   Str_Copy (UsrName,Gbl.Usrs.Me.UsrDat.Surname1,sizeof (UsrName) - 1);
   if (Gbl.Usrs.Me.UsrDat.Surname2[0])
     {
      Str_Concat (UsrName," ",sizeof (UsrName) - 1);
      Str_Concat (UsrName,Gbl.Usrs.Me.UsrDat.Surname2,sizeof (UsrName) - 1);
     }
   Str_Concat (UsrName,", ",sizeof (UsrName) - 1);
   Str_Concat (UsrName,Gbl.Usrs.Me.UsrDat.FrstName,sizeof (UsrName) - 1);

   /***** Build the lists of available rooms *****/
   snprintf (ListRoomCodes,sizeof (ListRoomCodes),"#%s",RoomCode);
   Str_Copy (ListRoomShrtNames,RoomShrtName,sizeof (ListRoomShrtNames) - 1);
   Str_Copy (ListRoomFullNames,RoomFullName,sizeof (ListRoomFullNames) - 1);

   if (strcmp (RoomCode,"GBL_USR"))
     {
      Str_Concat (ListRoomCodes,"|#GBL_USR",sizeof (ListRoomCodes) - 1);

      snprintf (RoomShrtName,sizeof (RoomShrtName),"|%s",
		Txt_SEX_PLURAL_Abc[Usr_SEX_ALL]);
      Str_Concat (ListRoomShrtNames,RoomShrtName,sizeof (ListRoomShrtNames) - 1);

      snprintf (RoomFullName,sizeof (RoomFullName),"|%s (%s)",
                Txt_General,Txt_SEX_PLURAL_abc[Usr_SEX_ALL]);
      Str_Concat (ListRoomFullNames,RoomFullName,sizeof (ListRoomFullNames) - 1);
     }

   if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
      if (strcmp (RoomCode,"GBL_STD"))
        {
         Str_Concat (ListRoomCodes,"|#GBL_STD",sizeof (ListRoomCodes) - 1);

         snprintf (RoomShrtName,sizeof (RoomShrtName),"|%s",
		   Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
         Str_Concat (ListRoomShrtNames,RoomShrtName,sizeof (ListRoomShrtNames) - 1);

         snprintf (RoomFullName,sizeof (RoomFullName),"|%s (%s)",
		   Txt_General,Txt_ROLES_PLURAL_abc[Rol_STD][Usr_SEX_ALL]);
	 Str_Concat (ListRoomFullNames,RoomFullName,sizeof (ListRoomFullNames) - 1);
        }

   if (Gbl.Usrs.Me.Role.Logged == Rol_NET ||
       Gbl.Usrs.Me.Role.Logged == Rol_TCH)
      if (strcmp (RoomCode,"GBL_TCH"))
        {
         Str_Concat (ListRoomCodes,"|#GBL_TCH",sizeof (ListRoomCodes) - 1);

         snprintf (RoomShrtName,sizeof (RoomShrtName),"|%s",
		   Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
         Str_Concat (ListRoomShrtNames,RoomShrtName,sizeof (ListRoomShrtNames) - 1);

         snprintf (RoomFullName,sizeof (RoomFullName),"|%s (%s)",
                   Txt_General,Txt_ROLES_PLURAL_abc[Rol_TCH][Usr_SEX_ALL]);
	 Str_Concat (ListRoomFullNames,RoomFullName,sizeof (ListRoomFullNames) - 1);
        }

   for (NumMyDeg = 0;
	NumMyDeg < Gbl.Usrs.Me.Hierarchy[Hie_DEG].Num;
	NumMyDeg++)
     {
      snprintf (ThisRoomCode,sizeof (ThisRoomCode),"DEG_%ld",
		Gbl.Usrs.Me.Hierarchy[Hie_DEG].Nodes[NumMyDeg].HieCod);
      if (strcmp (RoomCode,ThisRoomCode))
        {
         Str_Concat (ListRoomCodes,"|#",sizeof (ListRoomCodes) - 1);
         Str_Concat (ListRoomCodes,ThisRoomCode,sizeof (ListRoomCodes) - 1);

         /* Get data of this degree */
         Deg.HieCod = Gbl.Usrs.Me.Hierarchy[Hie_DEG].Nodes[NumMyDeg].HieCod;
         Hie_GetDataByCod[Hie_DEG] (&Deg);

         snprintf (ThisRoomShortName,sizeof (ThisRoomShortName),"%s",
		   Deg.ShrtName);
         Str_Concat (ListRoomShrtNames,"|",sizeof (ListRoomShrtNames) - 1);
         Str_Concat (ListRoomShrtNames,ThisRoomShortName,sizeof (ListRoomShrtNames) - 1);

         snprintf (ThisRoomFullName,sizeof (ThisRoomFullName),"%s %s",
		   Txt_HIERARCHY_SINGUL_Abc[Hie_DEG],Deg.ShrtName);
         Str_Concat (ListRoomFullNames,"|",sizeof (ListRoomFullNames) - 1);
         Str_Concat (ListRoomFullNames,ThisRoomFullName,sizeof (ListRoomFullNames) - 1);
        }
     }

   for (NumMyCrs = 0;
	NumMyCrs < Gbl.Usrs.Me.Hierarchy[Hie_CRS].Num;
	NumMyCrs++)
     {
      snprintf (ThisRoomCode,sizeof (ThisRoomCode),"CRS_%ld",
		Gbl.Usrs.Me.Hierarchy[Hie_CRS].Nodes[NumMyCrs].HieCod);
      if (strcmp (RoomCode,ThisRoomCode))
        {
         Str_Concat (ListRoomCodes,"|#",sizeof (ListRoomCodes) - 1);
         Str_Concat (ListRoomCodes,ThisRoomCode,sizeof (ListRoomCodes) - 1);

         /* Get data of this course */
         Crs.HieCod = Gbl.Usrs.Me.Hierarchy[Hie_CRS].Nodes[NumMyCrs].HieCod;
         Hie_GetDataByCod[Hie_CRS] (&Crs);

         snprintf (ThisRoomShortName,sizeof (ThisRoomShortName),
                   "%s",
		   Crs.ShrtName);
         Str_Concat (ListRoomShrtNames,"|",sizeof (ListRoomShrtNames) - 1);
         Str_Concat (ListRoomShrtNames,ThisRoomShortName,sizeof (ListRoomShrtNames) - 1);

         snprintf (ThisRoomFullName,sizeof (ThisRoomFullName),"%s %s",
		   Txt_HIERARCHY_SINGUL_Abc[Hie_CRS],Crs.ShrtName);
         Str_Concat (ListRoomFullNames,"|",sizeof (ListRoomFullNames) - 1);
         Str_Concat (ListRoomFullNames,ThisRoomFullName,sizeof (ListRoomFullNames) - 1);
        }
     }

   /***** Open index.html file with the HTML page for the chat *****/
   if ((FileChat = fopen (Cfg_PATH_AND_FILE_REL_CHAT_PRIVATE,"rb")) == NULL)
      Err_ShowErrorAndExit ("Can not open chat.");

   /***** Begin writing the index.html file to the output *****/
   HTM_Txt ("Content-type: text/html; charset=windows-1252\n\n");	// Two carriage returns at the end of the line are mandatory!

   Gbl.Layout.HTMLStartWritten = true;

   /***** Copy index.html file until the end of the applet code *****/
   Str_WriteUntilStrFoundInFileIncludingStr (FileOut,FileChat,"<applet",
                                             Str_NO_SKIP_HTML_COMMENTS);
   Str_WriteUntilStrFoundInFileIncludingStr (FileOut,FileChat,">",
                                             Str_NO_SKIP_HTML_COMMENTS);

   /***** Write parameters *****/
   HTM_PARAM ("nick","N%s",Gbl.Session.Id);
   HTM_PARAM ("realname","%s",UsrName);
   HTM_PARAM ("host","%s",Par_GetIP ());
   HTM_PARAM ("server_name","%s",Cfg_PLATFORM_SERVER);
   HTM_PARAM ("port","%u",5000);
   HTM_PARAM ("image_bl","%s/usr_bl.jpg",Cfg_URL_ICON_PUBLIC);
   HTM_PARAM ("image_url","%s/%s.jpg",Cfg_URL_PHOTO_PUBLIC,Gbl.Usrs.Me.UsrDat.Photo);
   HTM_PARAM ("channel_name","%s",ListRoomCodes);
   HTM_PARAM ("tab","%s",ListRoomShrtNames);
   HTM_PARAM ("topic","%s",ListRoomFullNames);

   /***** Copy index.html file until the end *****/
   Str_WriteUntilStrFoundInFileIncludingStr (FileOut,FileChat,"</html>",
                                             Str_NO_SKIP_HTML_COMMENTS);

   /***** Close index.html file *****/
   fclose (FileChat);

   Gbl.Layout.DivsEndWritten = Gbl.Layout.HTMLEndWritten = true;
  }
