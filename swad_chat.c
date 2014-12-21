// swad_chat.c: chat

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL
#include <stdio.h>		// For sprintf
#include <string.h>

#include "swad_bool.h"
#include "swad_chat.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_string.h"
#include "swad_text.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Cht_CHAT_MAX_LEVELS 3

#define MAX_LENGTH_ROOM_CODE	    16	// Maximum length of the code of a chat room
#define MAX_LENGTH_ROOM_SHORT_NAME 128	// Maximum length of the short name of a chat room
#define MAX_LENGTH_ROOM_FULL_NAME  256	// Maximum length of the full name of a chat room

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Cht_WriteLinkToChat (const char *Icon,const char *RoomCode,const char *RoomShortName,const char *RoomFullName,
                                 unsigned Level,bool IsLastItemInLevel[1+Cht_CHAT_MAX_LEVELS]);

/*****************************************************************************/
/****************** List available whiteboard/chat rooms *********************/
/*****************************************************************************/

void Cht_ShowChatRooms (void)
  {
   extern const char *Txt_To_use_chat_you_must_have_installed_the_software_X_and_add_Y_;

   /***** Help message about software needed to use the whiteboard/chat *****/
   sprintf (Gbl.Message,Txt_To_use_chat_you_must_have_installed_the_software_X_and_add_Y_,
            Cfg_JAVA_URL,Cfg_JAVA_NAME,
            Cfg_PLATFORM_SERVER);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);

   /***** List available chat rooms *****/
   Cht_ShowListOfAvailableChatRooms ();

   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
      Cht_ShowListOfChatRoomsWithUsrs ();
  }

/*****************************************************************************/
/*********************** Show list of available chat rooms *******************/
/*****************************************************************************/

void Cht_ShowListOfAvailableChatRooms (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Chat_rooms;
   extern const char *Txt_General;
   extern const char *Txt_SEX_PLURAL_Abc[Usr_NUM_SEXS];
   extern const char *Txt_SEX_PLURAL_abc[Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Students_ABBREVIATION;
   extern const char *Txt_Teachers_ABBREVIATION;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;
   bool IsLastItemInLevel[1+Cht_CHAT_MAX_LEVELS];
   unsigned NumMyDeg;
   struct Degree Deg;
   struct Course Crs;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow,NumRows;
   char Icon[512];
   char ThisRoomCode     [MAX_LENGTH_ROOM_CODE      +1];
   char ThisRoomShortName[MAX_LENGTH_ROOM_SHORT_NAME+1];
   char ThisRoomFullName [MAX_LENGTH_ROOM_FULL_NAME +1];

   /***** Fill the list with the degrees I belong to *****/ 
   Usr_GetMyDegrees ();

   /***** Table start *****/
   Lay_StartRoundFrameTable10 (NULL,0,Txt_Chat_rooms);
   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"left\" class=\"%s\">",
	    The_ClassFormul[Gbl.Prefs.Theme]);

   /***** Title of top level *****/
   fprintf (Gbl.F.Out,"<img src=\"%s/chat16x16.gif\""
	              " class=\"ICON16x16\" style=\"vertical-align:top;\" />"
                      " %s</td>"
                      "</tr>",
            Gbl.Prefs.IconsURL,Txt_Chat_rooms);

   /***** Link to chat available for all the users *****/
   sprintf (Icon,"<img src=\"%s/chat16x16.gif\""
	         " class=\"ICON16x16\" style=\"vertical-align:top;\" />",
            Gbl.Prefs.IconsURL);

   IsLastItemInLevel[1] = (Gbl.Usrs.Me.LoggedRole != Rol_ROLE_STUDENT &&
                           Gbl.Usrs.Me.LoggedRole != Rol_ROLE_TEACHER &&
                           !Gbl.Usrs.Me.MyDegrees.Num);
   sprintf (ThisRoomFullName,"%s (%s)",Txt_General,Txt_SEX_PLURAL_abc[Usr_SEX_ALL]);
   Cht_WriteLinkToChat (Icon,"GBL_USR",Txt_SEX_PLURAL_Abc[Usr_SEX_ALL],ThisRoomFullName,1,IsLastItemInLevel);

   IsLastItemInLevel[1] = !Gbl.Usrs.Me.MyDegrees.Num;
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_STUDENT:
         sprintf (ThisRoomFullName,"%s (%s)",Txt_General,Txt_ROLES_PLURAL_abc[Rol_ROLE_STUDENT][Usr_SEX_ALL]);
         Cht_WriteLinkToChat (Icon,"GBL_STD",Txt_Students_ABBREVIATION,ThisRoomFullName,1,IsLastItemInLevel);
         break;
      case Rol_ROLE_TEACHER:
         sprintf (ThisRoomFullName,"%s (%s)",Txt_General,Txt_ROLES_PLURAL_abc[Rol_ROLE_TEACHER][Usr_SEX_ALL]);
         Cht_WriteLinkToChat (Icon,"GBL_TCH",Txt_Teachers_ABBREVIATION,ThisRoomFullName,1,IsLastItemInLevel);
         break;
      default:
         break;
     }

   /***** Link to chat of users from my degrees *****/
   for (NumMyDeg = 0;
	NumMyDeg < Gbl.Usrs.Me.MyDegrees.Num;
	NumMyDeg++)
     {
      /* Get data of this degree */
      Deg.DegCod = Gbl.Usrs.Me.MyDegrees.Degs[NumMyDeg].DegCod;
      if (!Deg_GetDataOfDegreeByCod (&Deg))
         Lay_ShowErrorAndExit ("Degree not found.");

      /* Link to the room of this degree */
      IsLastItemInLevel[1] = (NumMyDeg == Gbl.Usrs.Me.MyDegrees.Num-1);
      sprintf (Icon,"<img src=\"%s/%s/%s16x16.gif\""
	            " class=\"ICON16x16\" style=\"vertical-align:top;\" />",
               Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_DEGREES,Deg.Logo);
      sprintf (ThisRoomCode,"DEG_%ld",Deg.DegCod);
      sprintf (ThisRoomShortName,"%s",Deg.ShortName);
      sprintf (ThisRoomFullName,"%s %s",Txt_Degree,Deg.ShortName);
      Cht_WriteLinkToChat (Icon,ThisRoomCode,ThisRoomShortName,ThisRoomFullName,1,IsLastItemInLevel);

      /* Get my courses in this degree from database */
      if ((NumRows = Usr_GetCrssFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Deg.DegCod,&mysql_res)) > 0) // Courses found in this degree
         for (NumRow = 0;
              NumRow < NumRows;
              NumRow++)
	   {
	    /* Get next course */
	    row = mysql_fetch_row (mysql_res);

            /* Get course code */
	    if ((Crs.CrsCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
	      {
               /* Get data of this course */
               Crs_GetDataOfCourseByCod (&Crs);

               /* Link to the room of this course */
               IsLastItemInLevel[2] = (NumRow == NumRows-1);
               sprintf (Icon,"<img src=\"%s/dot16x16.gif\""
        	             " class=\"ICON16x16\" style=\"vertical-align:top;\" />",
                        Gbl.Prefs.IconsURL);
               sprintf (ThisRoomCode,"CRS_%ld",Crs.CrsCod);
               sprintf (ThisRoomShortName,"%s",Crs.ShortName);
               sprintf (ThisRoomFullName,"%s %s",Txt_Course,Crs.ShortName);
               Cht_WriteLinkToChat (Icon,ThisRoomCode,ThisRoomShortName,ThisRoomFullName,2,IsLastItemInLevel);
	      }
	   }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** End table *****/
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/********************** Show list of chat rooms with users *******************/
/*****************************************************************************/

void Cht_ShowListOfChatRoomsWithUsrs (void)
  {
   extern const char *Txt_Rooms_with_users;
   extern const char *Txt_CHAT_Room_code;
   extern const char *Txt_No_of_users;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow,NumRows;

   /***** Get chat rooms with connected users from database *****/
   sprintf (Query,"SELECT RoomCode,NumUsrs FROM chat"
                  " WHERE NumUsrs>'0' ORDER BY NumUsrs DESC,RoomCode");
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get chat rooms with connected users");

   if (NumRows > 0) // If not empty chat rooms found
     {
      /***** Table start *****/
      Lay_WriteTitle (Txt_Rooms_with_users);
      Lay_StartRoundFrameTable10 (NULL,0,NULL);
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td align=\"center\" bgcolor=\"%s\" class=\"TIT_TBL\">%s&nbsp;</td>"
                         "<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">%s</td>"
                         "</tr>",
               VERY_LIGHT_BLUE,Txt_CHAT_Room_code,
               VERY_LIGHT_BLUE,Txt_No_of_users);

      /***** Loop over chat rooms *****/
      for (NumRow = 0;
	   NumRow < NumRows;
	   NumRow++)
	{
	 /* Get next chat room */
	 row = mysql_fetch_row (mysql_res);
         fprintf (Gbl.F.Out,"<tr>"
                            "<td align=\"left\" class=\"DAT\">%s</td>"
                            "<td align=\"right\" class=\"DAT\">%s</td>"
                            "</tr>",
                  row[0],row[1]);
        }

      /***** End table *****/
      Lay_EndRoundFrameTable10 ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Write title and link to a chat room ********************/
/*****************************************************************************/

static void Cht_WriteLinkToChat (const char *Icon,const char *RoomCode,const char *RoomShortName,const char *RoomFullName,
                                 unsigned Level,bool IsLastItemInLevel[1+Cht_CHAT_MAX_LEVELS])
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *The_ClassFormulB[The_NUM_THEMES];
   extern const char *Txt_connected_PLURAL;
   extern const char *Txt_connected_SINGULAR;
   int NumUsrsInRoom = Cht_GetNumUsrsInChatRoom (RoomCode);

   sprintf (Gbl.Chat.WindowName,"%s_%s",RoomCode,Gbl.UniqueNameEncrypted);

   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"left\" valign=\"top\">"
                      "<table cellpadding=\"0\">"
                      "<tr>");
   Msg_IndentDependingOnLevel (Level,IsLastItemInLevel);

   fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"middle\">");
   Act_FormStart (ActCht);
   Cht_WriteParamsRoomCodeAndNames (RoomCode,RoomShortName,RoomFullName);
   Act_LinkFormSubmit (RoomFullName,(NumUsrsInRoom > 0) ? The_ClassFormulB[Gbl.Prefs.Theme] :
                                                          The_ClassFormul[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s&nbsp;%s",Icon,RoomFullName);
   if (NumUsrsInRoom > 1)
      fprintf (Gbl.F.Out," [%d %s]",
               NumUsrsInRoom,Txt_connected_PLURAL);
   else if (NumUsrsInRoom == 1)
      fprintf (Gbl.F.Out," [1 %s]",
               Txt_connected_SINGULAR);
   fprintf (Gbl.F.Out,"</a>"
	              "</form>"
	              "</td>"
	              "</tr>"
	              "</table>"
	              "</td>"
	              "</tr>");
  }

/*****************************************************************************/
/*** Write parameters with code and names (short and full) of a chat room ****/
/*****************************************************************************/

void Cht_WriteParamsRoomCodeAndNames (const char *RoomCode,const char *RoomShortName,const char *RoomFullName)
  {
   Par_PutHiddenParamString ("RoomCode",RoomCode);
   Par_PutHiddenParamString ("RoomShortName",RoomShortName);
   Par_PutHiddenParamString ("RoomFullName",RoomFullName);
  }

/*****************************************************************************/
/*************** Get number of users connected to a chat room ****************/
/*****************************************************************************/

int Cht_GetNumUsrsInChatRoom (const char *RoomCode)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   int NumUsrs = -1;	// -1 ==> room is not in database. We can not know the number of usrs connected

   /***** Get number of users connected to chat rooms from database *****/
   sprintf (Query,"SELECT NumUsrs FROM chat WHERE RoomCode='%s'",RoomCode);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get number of users connected to a chat room");

   /***** Check number of rows of the result *****/
   if (NumRows == 1)
     {
      /* Get number of users connected to the chat room */
      row = mysql_fetch_row (mysql_res);
      if (row[0])
         if (sscanf (row[0],"%d",&NumUsrs) == 1)
            if (NumUsrs < 0)
	       NumUsrs = -1;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumUsrs;
  }

/*****************************************************************************/
/******************************* Enter a chat room ***************************/
/*****************************************************************************/

void Cht_OpenChatWindow (void)
  {
   extern const char *Txt_SEX_PLURAL_Abc[Usr_NUM_SEXS];
   extern const char *Txt_SEX_PLURAL_abc[Usr_NUM_SEXS];
   extern const char *Txt_General;
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Students_ABBREVIATION;
   extern const char *Txt_Teachers_ABBREVIATION;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;
   char RoomCode     [MAX_LENGTH_ROOM_CODE      +1];
   char RoomShortName[MAX_LENGTH_ROOM_SHORT_NAME+1];
   char RoomFullName [MAX_LENGTH_ROOM_FULL_NAME +1];
   char UsrName[3*(Usr_MAX_BYTES_NAME+1)];
   unsigned NumMyDeg;
   unsigned NumMyCrs;
   struct Degree Deg;
   struct Course Crs;
   char ThisRoomCode     [MAX_LENGTH_ROOM_CODE      +1];
   char ThisRoomShortName[MAX_LENGTH_ROOM_SHORT_NAME+1];
   char ThisRoomFullName [MAX_LENGTH_ROOM_FULL_NAME +1];
   char ListRoomCodes     [(3+Deg_MAX_DEGREES_PER_USR+Crs_MAX_COURSES_PER_USR)*MAX_LENGTH_ROOM_CODE      +1];
   char ListRoomShortNames[(3+Deg_MAX_DEGREES_PER_USR+Crs_MAX_COURSES_PER_USR)*MAX_LENGTH_ROOM_SHORT_NAME+1];
   char ListRoomFullNames [(3+Deg_MAX_DEGREES_PER_USR+Crs_MAX_COURSES_PER_USR)*MAX_LENGTH_ROOM_FULL_NAME +1];
   FILE *FileChat;

   /***** Get the code and the nombre of the room *****/
   Par_GetParToText ("RoomCode",RoomCode,MAX_LENGTH_ROOM_CODE);

   Par_GetParToText ("RoomShortName",RoomShortName,MAX_LENGTH_ROOM_SHORT_NAME);

   Par_GetParToText ("RoomFullName",RoomFullName,MAX_LENGTH_ROOM_FULL_NAME);

   if (!RoomCode[0] || !RoomShortName[0] || !RoomFullName[0])
      Lay_ShowErrorAndExit ("Wrong code or name of chat room.");
   if (strcspn (RoomCode," \t\n\r") != strlen (RoomCode)) // If RoomCode contiene espacios
      Lay_ShowErrorAndExit ("Wrong code of chat room.");

   /***** Fill the lists with the degrees and courses I belong to *****/ 
   Usr_GetMyDegrees ();
   Usr_GetMyCourses ();

   /***** Build my user's name *****/
   strcpy (UsrName,Gbl.Usrs.Me.UsrDat.Surname1);
   if (Gbl.Usrs.Me.UsrDat.Surname2[0])
     {
      strcat (UsrName," ");
      strcat (UsrName,Gbl.Usrs.Me.UsrDat.Surname2);
     }
   strcat (UsrName,", ");
   strcat (UsrName,Gbl.Usrs.Me.UsrDat.FirstName);

   /***** Build the lists of available rooms *****/
   sprintf (ListRoomCodes,"#%s",RoomCode);
   strcpy (ListRoomShortNames,RoomShortName);
   strcpy (ListRoomFullNames ,RoomFullName);
   if (strcmp (RoomCode,"GBL_USR"))
     {
      strcat (ListRoomCodes,"|#GBL_USR");
      sprintf (RoomShortName,"|%s",Txt_SEX_PLURAL_Abc[Usr_SEX_ALL]);
      strcat (ListRoomShortNames,RoomShortName);
      sprintf (RoomFullName,"|%s (%s)",Txt_General,Txt_SEX_PLURAL_abc[Usr_SEX_ALL]);
      strcat (ListRoomFullNames,RoomFullName);
     }
   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_STUDENT)
      if (strcmp (RoomCode,"GBL_STD"))
        {
         strcat (ListRoomCodes,"|#GBL_STD");
         sprintf (RoomShortName,"|%s",Txt_Students_ABBREVIATION);
         strcat (ListRoomShortNames,RoomShortName);
         sprintf (RoomFullName,"|%s (%s)",Txt_General,Txt_ROLES_PLURAL_abc[Rol_ROLE_STUDENT][Usr_SEX_ALL]);
         strcat (ListRoomFullNames,RoomFullName);
        }
   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER)
      if (strcmp (RoomCode,"GBL_TCH"))
        {
         strcat (ListRoomCodes,"|#GBL_TCH");
         sprintf (RoomShortName,"|%s",Txt_Teachers_ABBREVIATION);
         strcat (ListRoomShortNames,RoomShortName);
         sprintf (RoomFullName,"|%s (%s)",Txt_General,Txt_ROLES_PLURAL_abc[Rol_ROLE_TEACHER][Usr_SEX_ALL]);
         strcat (ListRoomFullNames,RoomFullName);
        }
   for (NumMyDeg = 0;
	NumMyDeg < Gbl.Usrs.Me.MyDegrees.Num;
	NumMyDeg++)
     {
      sprintf (ThisRoomCode,"DEG_%ld",Gbl.Usrs.Me.MyDegrees.Degs[NumMyDeg].DegCod);
      if (strcmp (RoomCode,ThisRoomCode))
        {
         strcat (ListRoomCodes,"|#");
         strcat (ListRoomCodes,ThisRoomCode);

         /* Get data of this degree */
         Deg.DegCod = Gbl.Usrs.Me.MyDegrees.Degs[NumMyDeg].DegCod;
         Deg_GetDataOfDegreeByCod (&Deg);

         sprintf (ThisRoomShortName,"%s",Deg.ShortName);
         strcat (ListRoomShortNames,"|");
         strcat (ListRoomShortNames,ThisRoomShortName);

         sprintf (ThisRoomFullName,"%s %s",Txt_Degree,Deg.ShortName);
         strcat (ListRoomFullNames,"|");
         strcat (ListRoomFullNames,ThisRoomFullName);
        }
     }
   for (NumMyCrs = 0;
	NumMyCrs < Gbl.Usrs.Me.MyCourses.Num;
	NumMyCrs++)
     {
      sprintf (ThisRoomCode,"CRS_%ld",Gbl.Usrs.Me.MyCourses.Crss[NumMyCrs].CrsCod);
      if (strcmp (RoomCode,ThisRoomCode))
        {
         strcat (ListRoomCodes,"|#");
         strcat (ListRoomCodes,ThisRoomCode);

         /* Get data of this course */
         Crs.CrsCod = Gbl.Usrs.Me.MyCourses.Crss[NumMyCrs].CrsCod;
         Crs_GetDataOfCourseByCod (&Crs);

         sprintf (ThisRoomShortName,"%s",Crs.ShortName);
         strcat (ListRoomShortNames,"|");
         strcat (ListRoomShortNames,ThisRoomShortName);

         sprintf (ThisRoomFullName,"%s %s",Txt_Course,Crs.ShortName);
         strcat (ListRoomFullNames,"|");
         strcat (ListRoomFullNames,ThisRoomFullName);
        }
     }

   /***** Open index.html file with the HTML page for the chat *****/
   if ((FileChat = fopen (Cfg_PATH_AND_FILE_REL_CHAT_PRIVATE,"rb")) == NULL)
      Lay_ShowErrorAndExit ("Can not open chat.");

   /***** Start writing the index.html file to the output *****/
   fprintf (Gbl.F.Out,"Content-type: text/html; charset=windows-1252\n\n");	// Two carriage returns at the end of the line are mandatory!

   Gbl.Layout.HTMLStartWritten = true;

   /***** Copy index.html file until the end of the applet code *****/
   Str_WriteUntilStrFoundInFileIncludingStr (Gbl.F.Out,FileChat,"<applet",Str_NO_SKIP_HTML_COMMENTS);
   Str_WriteUntilStrFoundInFileIncludingStr (Gbl.F.Out,FileChat,">",Str_NO_SKIP_HTML_COMMENTS);

   /***** Write parameters *****/
   fprintf (Gbl.F.Out,"\n<param name=\"nick\" value=\"N%s\">",
	    Gbl.Session.Id);
   fprintf (Gbl.F.Out,"\n<param name=\"realname\" value=\"%s\">",
	    UsrName);
   fprintf (Gbl.F.Out,"\n<param name=\"host\" value=\"%s\">",
	    Gbl.IP);
   fprintf (Gbl.F.Out,"\n<param name=\"server_name\" value=\"%s\">",
	    Cfg_PLATFORM_SERVER);
   fprintf (Gbl.F.Out,"\n<param name=\"port\" value=\"5000\">");
   fprintf (Gbl.F.Out,"\n<param name=\"image_bl\" value=\"%s/%s/usr_bl.jpg\">",
	    Cfg_HTTP_URL_SWAD_PUBLIC,Cfg_FOLDER_PUBLIC_ICON);
   fprintf (Gbl.F.Out,"\n<param name=\"image_url\" value=\"%s/%s/%s.jpg\">",
	    Cfg_HTTP_URL_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,Gbl.Usrs.Me.UsrDat.Photo);
   fprintf (Gbl.F.Out,"\n<param name=\"channel_name\" value=\"%s\">",
	    ListRoomCodes);
   fprintf (Gbl.F.Out,"\n<param name=\"tab\" value=\"%s\">",
	    ListRoomShortNames);
   fprintf (Gbl.F.Out,"\n<param name=\"topic\" value=\"%s\">",
	    ListRoomFullNames);

   /***** Copy index.html file until the end *****/
   Str_WriteUntilStrFoundInFileIncludingStr (Gbl.F.Out,FileChat,"</html>",Str_NO_SKIP_HTML_COMMENTS);

   /***** Close index.html file *****/
   fclose (FileChat);

   Gbl.Layout.TablEndWritten = Gbl.Layout.HTMLEndWritten = true;
  }
