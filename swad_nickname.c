// swad_nickname.c: Users' nicknames

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#include <string.h>		// For string functions

#include "swad_account.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_user.h"

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

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Nck_RemoveNicknameFromDB (const char *Nickname);

/*****************************************************************************/
/********* Check whether a nickname (with initial arroba) if valid ***********/
/*****************************************************************************/

bool Nck_CheckIfNickWithArrobaIsValid (const char *NicknameWithArroba)
  {
   char NicknameWithoutArroba[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA+1];
   unsigned Length;
   const char *Ptr;

   /***** A nickname must start by '@' *****/
   if (NicknameWithArroba[0] != '@')        // It's not a nickname
      return false;

   /***** Make a copy of nickname *****/
   strncpy (NicknameWithoutArroba,NicknameWithArroba,Nck_MAX_BYTES_NICKNAME_WITH_ARROBA);
   NicknameWithoutArroba[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA] = '\0';
   Str_RemoveLeadingArrobas (NicknameWithoutArroba);
   Length = strlen (NicknameWithoutArroba);

   /***** A nick (without arroba) must have a number of characters
          Nck_MIN_LENGTH_NICKNAME_WITHOUT_ARROBA <= Length <= Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA *****/
   if (Length < Nck_MIN_LENGTH_NICKNAME_WITHOUT_ARROBA ||
       Length > Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA)
      return false;

   /***** A nick can have digits, letters and '_'  *****/
   for (Ptr = NicknameWithoutArroba;
        *Ptr;
        Ptr++)
      if (!((*Ptr >= 'a' && *Ptr <= 'z') ||
            (*Ptr >= 'A' && *Ptr <= 'Z') ||
            (*Ptr >= '0' && *Ptr <= '9') ||
            (*Ptr == '_')))
         return false;

   return true;
  }

/*****************************************************************************/
/************* Get nickname of a user from his/her user's code ***************/
/*****************************************************************************/

bool Nck_GetNicknameFromUsrCod (long UsrCod,char *Nickname)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool Found;

   /***** Get current (last updated) user's nickname from database *****/
   sprintf (Query,"SELECT Nickname FROM usr_nicknames"
	          " WHERE UsrCod='%ld' ORDER BY CreatTime DESC LIMIT 1",
	    UsrCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get nickname"))
     {
      /* Get nickname */
      row = mysql_fetch_row (mysql_res);
      strncpy (Nickname,row[0],Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA);
      Nickname[Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA] = '\0';
      Found = true;
     }
   else
     {
      *Nickname = '\0';
      Found = false;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Found;
  }

/*****************************************************************************/
/************** Get user's code of a user from his/her nickname **************/
/*****************************************************************************/
// Nickname may have leading '@'
// Returns true if nickname found in database

long Nck_GetUsrCodFromNickname (const char *Nickname)
  {
   char NicknameWithoutArroba[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA + 1];
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long UsrCod = -1L;

   /***** Make a copy without possible starting arrobas *****/
   strncpy (NicknameWithoutArroba,Nickname,Nck_MAX_BYTES_NICKNAME_WITH_ARROBA);
   NicknameWithoutArroba[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA] = '\0';
   Str_RemoveLeadingArrobas (NicknameWithoutArroba);

   /***** Get user's code from database *****/
   /* Check if user code from table usr_nicknames is also in table usr_data */
   sprintf (Query,"SELECT usr_nicknames.UsrCod FROM usr_nicknames,usr_data"
                  " WHERE usr_nicknames.Nickname='%s'"
                  " AND usr_nicknames.UsrCod=usr_data.UsrCod",
            NicknameWithoutArroba);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get user's code"))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get user's code */
      UsrCod = Str_ConvertStrCodToLongCod (row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return UsrCod;
  }

/*****************************************************************************/
/*********************** Show form to change my nickname *********************/
/*****************************************************************************/

void Nck_ShowFormChangeUsrNickname (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Current_nickname;
   extern const char *Txt_Other_nicknames;
   extern const char *Txt_Remove_nickname_X;
   extern const char *Txt_Use_this_nickname;
   extern const char *Txt_New_nickname;
   extern const char *Txt_Nickname;
   extern const char *Txt_Change_nickname;
   extern const char *Txt_Save;
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumNicks;
   unsigned NumNick;

   /***** Get my nicknames *****/
   sprintf (Query,"SELECT Nickname FROM usr_nicknames"
                  " WHERE UsrCod='%ld'"
                  " ORDER BY CreatTime DESC",
            Gbl.Usrs.Me.UsrDat.UsrCod);
   NumNicks = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get nicknames of a user");

   /***** List my nicknames *****/
   for (NumNick = 1;
	NumNick <= NumNicks;
	NumNick++)
     {
      /* Get nickname */
      row = mysql_fetch_row (mysql_res);

      if (NumNick == 1)
	 /* The first nickname is the current one */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"width:40%%;"
			    " text-align:right; vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td style=\"width:60%%;"
			    " text-align:left; vertical-align:middle;\">",
		  The_ClassFormul[Gbl.Prefs.Theme],Txt_Current_nickname);
      else	// NumNick >= 2
	{
	 if (NumNick == 2)
	    fprintf (Gbl.F.Out,"<tr>"
			       "<td class=\"%s\" style=\"width:40%%;"
			       " text-align:right; vertical-align:top;\">"
			       "%s:"
			       "</td>"
			       "<td style=\"width:60%%;"
			       " text-align:left; vertical-align:top;\">",
		     The_ClassFormul[Gbl.Prefs.Theme],Txt_Other_nicknames);
	 else	// NumNick >= 3
	    fprintf (Gbl.F.Out,"<br />");

	 /* Form to remove old nickname */
	 Act_FormStart (ActRemOldNic);
	 sprintf (Gbl.Title,Txt_Remove_nickname_X,
		  row[0]);
	 fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"Nick\" value=\"%s\" />"
			    "<input type=\"image\" src=\"%s/delon16x16.gif\""
			    " alt=\"%s\" title=\"%s\" class=\"ICON16x16\""
			    " style=\"margin-right:2px;\" />",
		  row[0],
		  Gbl.Prefs.IconsURL,
		  Gbl.Title,
		  Gbl.Title);
	 Act_FormEnd ();
	}

      /* Nickname */
      fprintf (Gbl.F.Out,"<span class=\"%s\">@%s</span>",
	       NumNick == 1 ? "USR_ID" :
		              "DAT",
	       row[0]);

      /* Link to QR code */
      if (NumNick == 1 && Gbl.Usrs.Me.UsrDat.Nickname[0])
         QR_PutLinkToPrintQRCode (QR_NICKNAME,&Gbl.Usrs.Me.UsrDat,false);

      /* Form to change the nickname */
      if (NumNick > 1)
	{
	 Act_FormStart (ActChgNic);
	 fprintf (Gbl.F.Out,"&nbsp;"
	                    "<input type=\"hidden\" name=\"NewNick\" value=\"@%s\" />"
			    "<input type=\"submit\" value=\"%s\" />",
		  row[0],	// Nickname
		  Txt_Use_this_nickname);
	 Act_FormEnd ();
	}

      if (NumNick == 1 ||
	  NumNick == NumNicks)
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");
     }

   /***** Form to enter new nickname *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s\" style=\"width:40%%;"
                      " text-align:right; vertical-align:middle;\">"
                      "%s:"
                      "</td>"
                      "<td style=\"width:60%%;"
                      " text-align:left; vertical-align:middle;\">",
            The_ClassFormul[Gbl.Prefs.Theme],
            NumNicks ? Txt_New_nickname :	// A new nickname
        	       Txt_Nickname);		// The first nickname
   Act_FormStart (ActChgNic);
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"NewNick\" size=\"16\" maxlength=\"%u\" value=\"@%s\" />"
	              "<input type=\"submit\" value=\"%s\" />",
            1 + Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA,
            Gbl.Usrs.Me.UsrDat.Nickname,
            NumNicks ? Txt_Change_nickname :	// I already have a nickname
        	       Txt_Save);		// I have no nickname yet
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
  }

/*****************************************************************************/
/******************************* Remove nickname *****************************/
/*****************************************************************************/

void Nck_RemoveNick (void)
  {
   extern const char *Txt_Nickname_X_removed;
   extern const char *Txt_You_can_not_delete_your_current_nickname;
   char NicknameWithoutArroba[Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA+1];

   /***** Get new nickname from form *****/
   Par_GetParToText ("Nick",NicknameWithoutArroba,Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA);

   if (strcasecmp (NicknameWithoutArroba,Gbl.Usrs.Me.UsrDat.Nickname))	// Only if not my current nickname
     {
      /***** Remove one of my old nicknames *****/
      Nck_RemoveNicknameFromDB (NicknameWithoutArroba);

      /***** Show message *****/
      sprintf (Gbl.Message,Txt_Nickname_X_removed,NicknameWithoutArroba);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_You_can_not_delete_your_current_nickname);

   /***** Show my account again *****/
   Acc_ShowFormChangeMyAccount ();
  }

/*****************************************************************************/
/******************* Remove an old nickname from database ********************/
/*****************************************************************************/

static void Nck_RemoveNicknameFromDB (const char *Nickname)
  {
   char Query[256];

   /***** Remove an old nickname *****/
   sprintf (Query,"DELETE FROM usr_nicknames"
                  " WHERE UsrCod='%ld' AND Nickname='%s'",
            Gbl.Usrs.Me.UsrDat.UsrCod,Nickname);
   DB_QueryREPLACE (Query,"can not remove an old nickname");
  }

/*****************************************************************************/
/******************************* Update nickname *****************************/
/*****************************************************************************/

void Nck_UpdateNick (void)
  {
   extern const char *Txt_The_nickname_X_matches_the_one_you_had_previously_registered;
   extern const char *Txt_The_nickname_X_had_been_registered_by_another_user;
   extern const char *Txt_Your_nickname_X_has_been_registered_successfully;
   extern const char *Txt_The_nickname_entered_X_is_not_valid_;
   char Query[1024];
   char NewNicknameWithArroba[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA+1];
   char NewNicknameWithoutArroba[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA+1];
   bool Error = false;

   /***** Get new nickname from form *****/
   Par_GetParToText ("NewNick",NewNicknameWithArroba,Nck_MAX_BYTES_NICKNAME_WITH_ARROBA);

   if (Nck_CheckIfNickWithArrobaIsValid (NewNicknameWithArroba))        // If new nickname is valid
     {
      /***** Remove arrobas at the beginning *****/
      strncpy (NewNicknameWithoutArroba,NewNicknameWithArroba,Nck_MAX_BYTES_NICKNAME_WITH_ARROBA);
      NewNicknameWithoutArroba[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA] = '\0';
      Str_RemoveLeadingArrobas (NewNicknameWithoutArroba);

      /***** Check if new nickname exists in database *****/
      if (!strcmp (Gbl.Usrs.Me.UsrDat.Nickname,NewNicknameWithoutArroba)) // My nickname match exactly the new nickname
        {
         Error = true;
         sprintf (Gbl.Message,Txt_The_nickname_X_matches_the_one_you_had_previously_registered,
                  NewNicknameWithoutArroba);
        }
      else if (strcasecmp (Gbl.Usrs.Me.UsrDat.Nickname,NewNicknameWithoutArroba)) // My nickname does not match, not even case insensitive, the new nickname
        {
         /***** Check if the new nickname matches any of my old nicknames *****/
         sprintf (Query,"SELECT COUNT(*) FROM usr_nicknames"
                        " WHERE UsrCod='%ld' AND Nickname='%s'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,NewNicknameWithoutArroba);
         if (!DB_QueryCOUNT (Query,"can not check if nickname already existed"))        // No matches
           {
            /***** Check if the new nickname matches any of the nicknames of other users *****/
            sprintf (Query,"SELECT COUNT(*) FROM usr_nicknames"
                           " WHERE Nickname='%s' AND UsrCod<>'%ld'",
                     NewNicknameWithoutArroba,Gbl.Usrs.Me.UsrDat.UsrCod);
            if (DB_QueryCOUNT (Query,"can not check if nickname already existed"))        // A nickname of another user is the same that my nickname
              {
               Error = true;
               sprintf (Gbl.Message,Txt_The_nickname_X_had_been_registered_by_another_user,
                        NewNicknameWithoutArroba);
              }
           }
        }
      if (!Error)
        {
         // Now we know the new nickname is not already in database and is diffent to the current one
         Nck_UpdateMyNick (NewNicknameWithoutArroba);
         strcpy (Gbl.Usrs.Me.UsrDat.Nickname,NewNicknameWithoutArroba);

         sprintf (Gbl.Message,Txt_Your_nickname_X_has_been_registered_successfully,
                  NewNicknameWithoutArroba);
        }
     }
   else        // New nickname is not valid
     {
      Error = true;
      sprintf (Gbl.Message,Txt_The_nickname_entered_X_is_not_valid_,
               NewNicknameWithArroba,
               Nck_MIN_LENGTH_NICKNAME_WITHOUT_ARROBA,
               Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA);
     }

   /***** Show message *****/
   Lay_ShowAlert (Error ? Lay_WARNING :
	                  Lay_SUCCESS,
	          Gbl.Message);

   /***** Show my account again *****/
   Acc_ShowFormChangeMyAccount ();
  }

/*****************************************************************************/
/********************** Update my nickname in database ***********************/
/*****************************************************************************/

void Nck_UpdateMyNick (const char *NewNickname)
  {
   char Query[512];

   /***** Update my nickname in database *****/
   sprintf (Query,"REPLACE INTO usr_nicknames"
                  " (UsrCod,Nickname,CreatTime) VALUES ('%ld','%s',NOW())",
            Gbl.Usrs.Me.UsrDat.UsrCod,NewNickname);
   DB_QueryREPLACE (Query,"can not update your nickname");
  }
