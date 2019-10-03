// swad_nickname.c: Users' nicknames

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_table.h"
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

const char *Nck_NICKNAME_SECTION_ID = "nickname_section";

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Nck_ShowFormChangeUsrNickname (const struct UsrData *UsrDat,bool ItsMe,
                                           bool IMustFillNickname);

static void Nck_RemoveNicknameFromDB (long UsrCod,const char *Nickname);

static void Nck_UpdateUsrNick (struct UsrData *UsrDat);

/*****************************************************************************/
/********* Check whether a nickname (with initial arroba) if valid ***********/
/*****************************************************************************/

bool Nck_CheckIfNickWithArrobaIsValid (const char *NicknameWithArroba)
  {
   char NicknameWithoutArroba[Nck_MAX_BYTES_NICKNAME_FROM_FORM + 1];
   unsigned Length;
   const char *Ptr;

   /***** A nickname must start by '@' *****/
   if (NicknameWithArroba[0] != '@')        // It's not a nickname
      return false;

   /***** Make a copy of nickname *****/
   Str_Copy (NicknameWithoutArroba,NicknameWithArroba,
             Nck_MAX_BYTES_NICKNAME_FROM_FORM);
   Str_RemoveLeadingArrobas (NicknameWithoutArroba);
   Length = strlen (NicknameWithoutArroba);

   /***** A nick (without arroba) must have a number of characters
          Nck_MIN_BYTES_NICKNAME_WITHOUT_ARROBA <= Length <= Nck_MAX_BYTES_NICKNAME_WITHOUT_ARROBA *****/
   if (Length < Nck_MIN_BYTES_NICKNAME_WITHOUT_ARROBA ||
       Length > Nck_MAX_BYTES_NICKNAME_WITHOUT_ARROBA)
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

bool Nck_GetNicknameFromUsrCod (long UsrCod,
                                char Nickname[Nck_MAX_BYTES_NICKNAME_WITHOUT_ARROBA + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool Found;

   /***** Get current (last updated) user's nickname from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get nickname",
		       "SELECT Nickname FROM usr_nicknames"
		       " WHERE UsrCod=%ld ORDER BY CreatTime DESC LIMIT 1",
		       UsrCod))
     {
      /* Get nickname */
      row = mysql_fetch_row (mysql_res);
      Str_Copy (Nickname,row[0],
                Nck_MAX_BYTES_NICKNAME_WITHOUT_ARROBA);
      Found = true;
     }
   else
     {
      Nickname[0] = '\0';
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
   char NicknameWithoutArroba[Nck_MAX_BYTES_NICKNAME_FROM_FORM + 1];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long UsrCod = -1L;

   if (Nickname)
      if (Nickname[0])
	{
	 /***** Make a copy without possible starting arrobas *****/
	 Str_Copy (NicknameWithoutArroba,Nickname,
	           Nck_MAX_BYTES_NICKNAME_FROM_FORM);
	 Str_RemoveLeadingArrobas (NicknameWithoutArroba);

	 /***** Get user's code from database *****/
	 /* Check if user code from table usr_nicknames is also in table usr_data */
	 if (DB_QuerySELECT (&mysql_res,"can not get user's code",
			     "SELECT usr_nicknames.UsrCod"
			     " FROM usr_nicknames,usr_data"
			     " WHERE usr_nicknames.Nickname='%s'"
			     " AND usr_nicknames.UsrCod=usr_data.UsrCod",
			     NicknameWithoutArroba))
	   {
	    /* Get row */
	    row = mysql_fetch_row (mysql_res);

	    /* Get user's code */
	    UsrCod = Str_ConvertStrCodToLongCod (row[0]);
	   }

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);
	}

   return UsrCod;
  }

/*****************************************************************************/
/*********************** Show form to change my nickname *********************/
/*****************************************************************************/

void Nck_ShowFormChangeMyNickname (bool IMustFillNickname)
  {
   Nck_ShowFormChangeUsrNickname (&Gbl.Usrs.Me.UsrDat,
				  true,		// ItsMe
				  IMustFillNickname);
  }

/*****************************************************************************/
/*********************** Show form to change my nickname *********************/
/*****************************************************************************/

void Nck_ShowFormChangeOtherUsrNickname (void)
  {
   Nck_ShowFormChangeUsrNickname (&Gbl.Usrs.Other.UsrDat,
				  false,	// ItsMe
				  false);	// IMustFillNickname
  }

/*****************************************************************************/
/*********************** Show form to change my nickname *********************/
/*****************************************************************************/

static void Nck_ShowFormChangeUsrNickname (const struct UsrData *UsrDat,bool ItsMe,
                                           bool IMustFillNickname)
  {
   extern const char *Hlp_PROFILE_Account;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Nickname;
   extern const char *Txt_Before_going_to_any_other_option_you_must_fill_your_nickname;
   extern const char *Txt_Current_nickname;
   extern const char *Txt_Other_nicknames;
   extern const char *Txt_Use_this_nickname;
   extern const char *Txt_New_nickname;
   extern const char *Txt_Change_nickname;
   extern const char *Txt_Save_changes;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char StrRecordWidth[10 + 1];
   unsigned NumNicks;
   unsigned NumNick;
   Act_Action_t NextAction;

   /***** Start section *****/
   Lay_StartSection (Nck_NICKNAME_SECTION_ID);

   /***** Get my nicknames *****/
   NumNicks =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get nicknames of a user",
			      "SELECT Nickname FROM usr_nicknames"
			      " WHERE UsrCod=%ld"
			      " ORDER BY CreatTime DESC",
			      UsrDat->UsrCod);

   /***** Start box *****/
   snprintf (StrRecordWidth,sizeof (StrRecordWidth),
	     "%upx",
	     Rec_RECORD_WIDTH);
   Box_StartBox (StrRecordWidth,Txt_Nickname,Acc_PutLinkToRemoveMyAccount,
                 Hlp_PROFILE_Account,Box_NOT_CLOSABLE);

   /***** Show possible alerts *****/
   Ale_ShowAlerts (Nck_NICKNAME_SECTION_ID);

   /***** Help message *****/
   if (IMustFillNickname)
      Ale_ShowAlert (Ale_WARNING,Txt_Before_going_to_any_other_option_you_must_fill_your_nickname);

   /***** Start table *****/
   Tbl_StartTableWidePadding (2);

   /***** List nicknames *****/
   for (NumNick = 1;
	NumNick <= NumNicks;
	NumNick++)
     {
      /* Get nickname */
      row = mysql_fetch_row (mysql_res);

      if (NumNick == 1)
	 /* The first nickname is the current one */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"REC_C1_BOT RIGHT_TOP\">"
			    "<label for=\"Nick\" class=\"%s\">"
			    "%s:"
			    "</label>"
			    "</td>"
			    "<td class=\"REC_C2_BOT LEFT_TOP USR_ID\">",
		  The_ClassFormInBox[Gbl.Prefs.Theme],
		  Txt_Current_nickname);
      else	// NumNick >= 2
	{
	 Tbl_StartRow ();
	 if (NumNick == 2)
	    fprintf (Gbl.F.Out,"<td rowspan=\"%u\""
			       " class=\"REC_C1_BOT RIGHT_TOP\">"
			       "<label for=\"Nick\" class=\"%s\">"
			       "%s:"
			       "</label>"
			       "</td>",
		     NumNicks - 1,
		     The_ClassFormInBox[Gbl.Prefs.Theme],
		     Txt_Other_nicknames);
	 fprintf (Gbl.F.Out,"<td class=\"REC_C2_BOT LEFT_TOP DAT\">");

	 /* Form to remove old nickname */
	 if (ItsMe)
	    Frm_StartFormAnchor (ActRemMyNck,Nck_NICKNAME_SECTION_ID);
	 else
	   {
	    switch (UsrDat->Roles.InCurrentCrs.Role)
	      {
	       case Rol_STD:
		  NextAction = ActRemOldNicStd;
		  break;
	       case Rol_NET:
	       case Rol_TCH:
		  NextAction = ActRemOldNicTch;
		  break;
	       default:	// Guest, user or admin
		  NextAction = ActRemOldNicOth;
		  break;
	      }
	    Frm_StartFormAnchor (NextAction,Nck_NICKNAME_SECTION_ID);
	    Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	   }
	 fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"Nick\""
	                    " value=\"%s\" />",
		  row[0]);
	 Ico_PutIconRemove ();
	 Frm_EndForm ();
	}

      /* Nickname */
      fprintf (Gbl.F.Out,"@%s",row[0]);

      /* Link to QR code */
      if (NumNick == 1 && UsrDat->Nickname[0])
	 QR_PutLinkToPrintQRCode (ActPrnUsrQR,Usr_PutParamMyUsrCodEncrypted);

      /* Form to change the nickname */
      if (NumNick > 1)
	{
         fprintf (Gbl.F.Out,"<br />");
	 if (ItsMe)
	    Frm_StartFormAnchor (ActChgMyNck,Nck_NICKNAME_SECTION_ID);
	 else
	   {
	    switch (UsrDat->Roles.InCurrentCrs.Role)
	      {
	       case Rol_STD:
		  NextAction = ActChgNicStd;
		  break;
	       case Rol_NET:
	       case Rol_TCH:
		  NextAction = ActChgNicTch;
		  break;
	       default:	// Guest, user or admin
		  NextAction = ActChgNicOth;
		  break;
	      }
	    Frm_StartFormAnchor (NextAction,Nck_NICKNAME_SECTION_ID);
	    Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	   }
	 fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"NewNick\""
	                    " value=\"@%s\" />",
		  row[0]);	// Nickname
	 Btn_PutConfirmButtonInline (Txt_Use_this_nickname);
	 Frm_EndForm ();
	}

      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }

   /***** Form to enter new nickname *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"REC_C1_BOT RIGHT_TOP\">"
                      "<label for=\"NewNick\" class=\"%s\">"
                      "%s:"
                      "</label>"
                      "</td>"
                      "<td class=\"REC_C2_BOT LEFT_TOP DAT\">",
            The_ClassFormInBox[Gbl.Prefs.Theme],
            NumNicks ? Txt_New_nickname :	// A new nickname
        	       Txt_Nickname);		// The first nickname
   if (ItsMe)
      Frm_StartFormAnchor (ActChgMyNck,Nck_NICKNAME_SECTION_ID);
   else
     {
      switch (UsrDat->Roles.InCurrentCrs.Role)
	{
	 case Rol_STD:
	    NextAction = ActChgNicStd;
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	    NextAction = ActChgNicTch;
	    break;
	 default:	// Guest, user or admin
	    NextAction = ActChgNicOth;
	    break;
	}
      Frm_StartFormAnchor (NextAction,Nck_NICKNAME_SECTION_ID);
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
     }
   fprintf (Gbl.F.Out,"<input type=\"text\" id=\"NewNick\" name=\"NewNick\""
	              " size=\"18\" maxlength=\"%u\" value=\"@%s\" />"
	              "<br />",
            1 + Nck_MAX_CHARS_NICKNAME_WITHOUT_ARROBA,
            Gbl.Usrs.Me.UsrDat.Nickname);
   Btn_PutCreateButtonInline (NumNicks ? Txt_Change_nickname :	// I already have a nickname
        	                         Txt_Save_changes);	// I have no nickname yet);
   Frm_EndForm ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** End table and box *****/
   Box_EndBoxTable ();

   /***** End section *****/
   Lay_EndSection ();
  }

/*****************************************************************************/
/***************************** Remove my nickname ****************************/
/*****************************************************************************/

void Nck_RemoveMyNick (void)
  {
   extern const char *Txt_Nickname_X_removed;
   extern const char *Txt_You_can_not_delete_your_current_nickname;
   char NicknameWithoutArroba[Nck_MAX_BYTES_NICKNAME_WITHOUT_ARROBA + 1];

   /***** Get nickname from form *****/
   Par_GetParToText ("Nick",NicknameWithoutArroba,
	             Nck_MAX_BYTES_NICKNAME_WITHOUT_ARROBA);

   if (strcasecmp (NicknameWithoutArroba,Gbl.Usrs.Me.UsrDat.Nickname))	// Only if not my current nickname
     {
      /***** Remove one of my old nicknames *****/
      Nck_RemoveNicknameFromDB (Gbl.Usrs.Me.UsrDat.UsrCod,NicknameWithoutArroba);

      /***** Show message *****/
      Ale_CreateAlert (Ale_SUCCESS,Nck_NICKNAME_SECTION_ID,
	               Txt_Nickname_X_removed,
		       NicknameWithoutArroba);
     }
   else
      Ale_CreateAlert (Ale_WARNING,Nck_NICKNAME_SECTION_ID,
	               Txt_You_can_not_delete_your_current_nickname);

   /***** Show my account again *****/
   Acc_ShowFormChgMyAccount ();
  }

/*****************************************************************************/
/********************* Remove another user's nickname ************************/
/*****************************************************************************/

void Nck_RemoveOtherUsrNick (void)
  {
   extern const char *Txt_Nickname_X_removed;
   char NicknameWithoutArroba[Nck_MAX_BYTES_NICKNAME_WITHOUT_ARROBA + 1];

   /***** Get user whose nick must be removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Usr_ICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	{
	 /***** Get nickname from form *****/
	 Par_GetParToText ("Nick",NicknameWithoutArroba,
			   Nck_MAX_BYTES_NICKNAME_WITHOUT_ARROBA);

	 /***** Remove one of the old nicknames *****/
	 Nck_RemoveNicknameFromDB (Gbl.Usrs.Other.UsrDat.UsrCod,NicknameWithoutArroba);

	 /***** Show message *****/
	 Ale_CreateAlert (Ale_SUCCESS,Nck_NICKNAME_SECTION_ID,
	                  Txt_Nickname_X_removed,
		          NicknameWithoutArroba);

	 /***** Show user's account again *****/
	 Acc_ShowFormChgOtherUsrAccount ();
	}
      else
	 Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
     }
   else		// User not found
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/********************** Remove a nickname from database **********************/
/*****************************************************************************/

static void Nck_RemoveNicknameFromDB (long UsrCod,const char *Nickname)
  {
   /***** Remove a nickname *****/
   DB_QueryREPLACE ("can not remove a nickname",
		    "DELETE FROM usr_nicknames"
		    " WHERE UsrCod=%ld AND Nickname='%s'",
                    UsrCod,Nickname);
  }

/*****************************************************************************/
/***************************** Update my nickname ****************************/
/*****************************************************************************/

void Nck_UpdateMyNick (void)
  {
   /***** Update my nickname *****/
   Nck_UpdateUsrNick (&Gbl.Usrs.Me.UsrDat);

   /***** Show my account again *****/
   Acc_ShowFormChgMyAccount ();
  }

/*****************************************************************************/
/*********************** Update another user's nickname **********************/
/*****************************************************************************/

void Nck_UpdateOtherUsrNick (void)
  {
   /***** Get user whose nick must be changed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Usr_ICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	{
	 /***** Update my nickname *****/
	 Nck_UpdateUsrNick (&Gbl.Usrs.Other.UsrDat);

	 /***** Show user's account again *****/
	 Acc_ShowFormChgOtherUsrAccount ();
	}
      else
	 Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
     }
   else		// User not found
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/*************************** Update user's nickname **************************/
/*****************************************************************************/

static void Nck_UpdateUsrNick (struct UsrData *UsrDat)
  {
   extern const char *Txt_The_nickname_X_matches_the_one_you_had_previously_registered;
   extern const char *Txt_The_nickname_X_had_been_registered_by_another_user;
   extern const char *Txt_The_nickname_X_has_been_registered_successfully;
   extern const char *Txt_The_nickname_entered_X_is_not_valid_;
   char NewNicknameWithArroba[Nck_MAX_BYTES_NICKNAME_FROM_FORM + 1];
   char NewNicknameWithoutArroba[Nck_MAX_BYTES_NICKNAME_FROM_FORM + 1];

   /***** Get new nickname from form *****/
   Par_GetParToText ("NewNick",NewNicknameWithArroba,
		     Nck_MAX_BYTES_NICKNAME_FROM_FORM);
   if (Nck_CheckIfNickWithArrobaIsValid (NewNicknameWithArroba))        // If new nickname is valid
     {
      /***** Remove arrobas at the beginning *****/
      Str_Copy (NewNicknameWithoutArroba,NewNicknameWithArroba,
                Nck_MAX_BYTES_NICKNAME_FROM_FORM);
      Str_RemoveLeadingArrobas (NewNicknameWithoutArroba);

      /***** Check if new nickname exists in database *****/
      if (!strcmp (UsrDat->Nickname,NewNicknameWithoutArroba))		// User's nickname match exactly the new nickname
         Ale_CreateAlert (Ale_WARNING,Nck_NICKNAME_SECTION_ID,
                          Txt_The_nickname_X_matches_the_one_you_had_previously_registered,
                          NewNicknameWithoutArroba);
      else if (strcasecmp (UsrDat->Nickname,NewNicknameWithoutArroba))	// User's nickname does not match, not even case insensitive, the new nickname
        {
         /***** Check if the new nickname matches any of my old nicknames *****/
         if (!DB_QueryCOUNT ("can not check if nickname already existed",
			     "SELECT COUNT(*) FROM usr_nicknames"
			     " WHERE UsrCod=%ld AND Nickname='%s'",
			     UsrDat->UsrCod,NewNicknameWithoutArroba))	// No matches
            /***** Check if the new nickname matches any of the nicknames of other users *****/
            if (DB_QueryCOUNT ("can not check if nickname already existed",
        		       "SELECT COUNT(*) FROM usr_nicknames"
			       " WHERE Nickname='%s' AND UsrCod<>%ld",
			       NewNicknameWithoutArroba,UsrDat->UsrCod))	// A nickname of another user is the same that user's nickname
               Ale_CreateAlert (Ale_WARNING,Nck_NICKNAME_SECTION_ID,
        	                Txt_The_nickname_X_had_been_registered_by_another_user,
                                NewNicknameWithoutArroba);
        }
      if (Ale_GetNumAlerts () == 0)	// No problems
        {
         // Now we know the new nickname is not already in database
	 // and is diffent to the current one
         Nck_UpdateNickInDB (UsrDat->UsrCod,NewNicknameWithoutArroba);
         Str_Copy (UsrDat->Nickname,NewNicknameWithoutArroba,
                   Nck_MAX_BYTES_NICKNAME_WITHOUT_ARROBA);

         Ale_CreateAlert (Ale_SUCCESS,Nck_NICKNAME_SECTION_ID,
                          Txt_The_nickname_X_has_been_registered_successfully,
                          NewNicknameWithoutArroba);
        }
     }
   else        // New nickname is not valid
      Ale_CreateAlert (Ale_WARNING,Nck_NICKNAME_SECTION_ID,
	               Txt_The_nickname_entered_X_is_not_valid_,
		       NewNicknameWithArroba,
		       Nck_MIN_CHARS_NICKNAME_WITHOUT_ARROBA,
		       Nck_MAX_CHARS_NICKNAME_WITHOUT_ARROBA);
  }

/*****************************************************************************/
/******************* Update user's nickname in database **********************/
/*****************************************************************************/

void Nck_UpdateNickInDB (long UsrCod,const char *NewNickname)
  {
   /***** Update user's nickname in database *****/
   DB_QueryREPLACE ("can not update nickname",
		    "REPLACE INTO usr_nicknames"
		    " (UsrCod,Nickname,CreatTime)"
		    " VALUES"
		    " (%ld,'%s',NOW())",
                    UsrCod,NewNickname);
  }
