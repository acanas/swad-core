// swad_nickname.c: Users' nicknames

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

#include <string.h>		// For string functions

#include "swad_account.h"
#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_nickname_database.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

const char *Nck_NICKNAME_SECTION_ID = "nickname_section";

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Nck_ShowFormChangeUsrNickname (Usr_MeOrOther_t MeOrOther,
                                           bool IMustFillNickname);
static void Nck_PutParsRemoveMyNick (void *Nick);
static void Nck_PutParsRemoveOtherNick (void *Nick);

static void Nck_ChangeUsrNick (struct Usr_Data *UsrDat);

/*****************************************************************************/
/********* Check whether a nickname (with initial arroba) if valid ***********/
/*****************************************************************************/

bool Nck_CheckIfNickWithArrIsValid (const char *NickWithArr)
  {
   char CopyOfNick[Nck_MAX_BYTES_NICK_WITH_ARROBA + 1];
   unsigned Length;
   const char *Ptr;

   /***** A nickname must start by '@' *****/
   if (NickWithArr[0] != '@')        // It's not a nickname
      return false;

   /***** Make a copy of nickname *****/
   Str_Copy (CopyOfNick,NickWithArr,sizeof (CopyOfNick) - 1);
   Str_RemoveLeadingArrobas (CopyOfNick);
   Length = strlen (CopyOfNick);	// Leading arrobas already removed

   /***** A nick (without arroba) must have a number of characters
          Nck_MIN_CHARS_NICK_WITHOUT_ARROBA <= Length <= Nck_MAX_CHARS_NICK_WITHOUT_ARROBA *****/
   if (Length < Nck_MIN_CHARS_NICK_WITHOUT_ARROBA ||
       Length > Nck_MAX_CHARS_NICK_WITHOUT_ARROBA)
      return false;

   /***** A nick can have digits, letters and '_'  *****/
   for (Ptr = CopyOfNick;		// Leading arrobas already removed
        *Ptr;
        Ptr++)
      if (!Str_ChIsAlphaNum (*Ptr))
         return false;

   return true;
  }

/*****************************************************************************/
/************** Get user's code of a user from his/her nickname **************/
/*****************************************************************************/
// Nickname may have leading '@'
// Returns true if nickname found in database

long Nck_GetUsrCodFromNickname (const char *NickWithArr)
  {
   char CopyOfNick[Nck_MAX_BYTES_NICK_WITH_ARROBA + 1];

   /***** Trivial check 1: nickname should be not null *****/
   if (!NickWithArr)
      return -1L;

   /***** Trivial check 2: nickname should be not empty *****/
   if (!NickWithArr[0])
      return -1L;

   /***** Remove leading arrobas *****/
   Str_Copy (CopyOfNick,NickWithArr,sizeof (CopyOfNick) - 1);
   Str_RemoveLeadingArrobas (CopyOfNick);

   /***** Get user's code from database *****/
   return Nck_DB_GetUsrCodFromNickname (CopyOfNick);
  }

/*****************************************************************************/
/******************* Put form to request the new nickname ********************/
/*****************************************************************************/

void Nck_PutFormToGetNewNickname (const char *NewNickWithoutArr)
  {
   extern const char *Txt_Nickname;
   extern const char *Txt_HELP_nickname;
   char NewNickWithArr[Nck_MAX_BYTES_NICK_WITH_ARROBA + 1];

   /***** Nickname *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RM","NewNick",Txt_Nickname);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LM\"");
	 if (NewNickWithoutArr[0])
	    snprintf (NewNickWithArr,sizeof (NewNickWithArr),"@%s",
		      NewNickWithoutArr);
	 else
	    NewNickWithArr[0] = '\0';
	 HTM_INPUT_TEXT ("NewNick",1 + Nck_MAX_CHARS_NICK_WITHOUT_ARROBA,
			 NewNickWithArr,
			 HTM_REQUIRED,
			 "id=\"NewNick\" size=\"16\" placeholder=\"%s\""
			 " class=\"Frm_C2_INPUT INPUT_%s\"",
			 Txt_HELP_nickname,The_GetSuffix ());
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/*********************** Show form to change my nickname *********************/
/*****************************************************************************/

void Nck_ShowFormChangeMyNickname (bool IMustFillNickname)
  {
   Nck_ShowFormChangeUsrNickname (Usr_ME,
				  IMustFillNickname);
  }

/*****************************************************************************/
/*********************** Show form to change my nickname *********************/
/*****************************************************************************/

void Nck_ShowFormChangeOtherUsrNickname (void)
  {
   Nck_ShowFormChangeUsrNickname (Usr_OTHER,
				  false);	// IMustFillNickname
  }

/*****************************************************************************/
/*********************** Show form to change my nickname *********************/
/*****************************************************************************/

static void Nck_ShowFormChangeUsrNickname (Usr_MeOrOther_t MeOrOther,
                                           bool IMustFillNickname)
  {
   extern const char *Hlp_PROFILE_Account;
   extern const char *Txt_Nickname;
   extern const char *Txt_Before_going_to_any_other_option_you_must_fill_your_nickname;
   extern const char *Txt_Current_nickname;
   extern const char *Txt_Other_nicknames;
   extern const char *Txt_Use_this_nickname;
   extern const char *Txt_New_nickname;
   extern const char *Txt_Change_nickname;
   extern const char *Txt_Save_changes;
   extern struct Usr_Data *Usr_UsrDat[Usr_NUM_ME_OR_OTHER];
   static const struct
     {
      Act_Action_t Remove;
      Act_Action_t Change;
     } NextAction[Rol_NUM_ROLES] =
     {
      [Rol_UNK	  ] = {ActRemNicOth,ActChgNicOth},
      [Rol_GST	  ] = {ActRemNicOth,ActChgNicOth},
      [Rol_USR	  ] = {ActRemNicOth,ActChgNicOth},
      [Rol_STD	  ] = {ActRemNicStd,ActChgNicStd},
      [Rol_NET	  ] = {ActRemNicTch,ActChgNicTch},
      [Rol_TCH	  ] = {ActRemNicTch,ActChgNicTch},
      [Rol_DEG_ADM] = {ActRemNicOth,ActChgNicOth},
      [Rol_CTR_ADM] = {ActRemNicOth,ActChgNicOth},
      [Rol_INS_ADM] = {ActRemNicOth,ActChgNicOth},
      [Rol_SYS_ADM] = {ActRemNicOth,ActChgNicOth},
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumNicks;
   unsigned NumNick;
   char NickWithArr[Nck_MAX_BYTES_NICK_WITH_ARROBA + 1];
   static void (*FuncParsRemove[Usr_NUM_ME_OR_OTHER]) (void *ID) =
     {
      [Usr_ME   ] = Nck_PutParsRemoveMyNick,
      [Usr_OTHER] = Nck_PutParsRemoveOtherNick
     };
   struct
     {
      Act_Action_t Remove;
      Act_Action_t Change;
     } ActNck[Rol_NUM_ROLES] =
     {
      [Usr_ME   ] = {.Remove = ActRemMyNck,
	             .Change = ActChgMyNck},
      [Usr_OTHER] = {.Remove = NextAction[Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs].Remove,
	             .Change = NextAction[Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs].Change}
     };

   /***** Begin section *****/
   HTM_SECTION_Begin (Nck_NICKNAME_SECTION_ID);

      /***** Get my nicknames *****/
      NumNicks = Nck_DB_GetUsrNicknames (&mysql_res,Usr_UsrDat[MeOrOther]->UsrCod);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Nickname,Acc_PutLinkToRemoveMyAccount,NULL,
		    Hlp_PROFILE_Account,Box_NOT_CLOSABLE);

	 /***** Show possible alerts *****/
	 Ale_ShowAlerts (Nck_NICKNAME_SECTION_ID);

	 /***** Help message *****/
	 if (IMustFillNickname)
	    Ale_ShowAlert (Ale_WARNING,Txt_Before_going_to_any_other_option_you_must_fill_your_nickname);

	 /***** Begin table *****/
	 HTM_TABLE_BeginPadding (2);

	    /***** List nicknames *****/
	    for (NumNick  = 1;
		 NumNick <= NumNicks;
		 NumNick++)
	      {
	       /* Get nickname */
	       row = mysql_fetch_row (mysql_res);

	       if (NumNick == 1)
		 {
		  /* The first nickname is the current one */
		  HTM_TR_Begin (NULL);

		     /* Label */
		     Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_Current_nickname);

		     /* Data */
		     HTM_TD_Begin ("class=\"Frm_C2 LT DAT_STRONG_%s\"",
				   The_GetSuffix ());
		 }
	       else	// NumNick >= 2
		 {
		  if (NumNick == 2)
		    {
		     HTM_TR_Begin (NULL);

			/* Label */
			Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_Other_nicknames);

			/* Data */
			HTM_TD_Begin ("class=\"Frm_C2 LT DAT_%s\"",
				      The_GetSuffix ());
		    }

		  /* Form to remove old nickname */
		  Ico_PutContextualIconToRemove (ActNck[MeOrOther].Remove,Nck_NICKNAME_SECTION_ID,
						 FuncParsRemove[MeOrOther],row[0]);
		 }

	       /* Nickname */
	       HTM_TxtF ("@%s",row[0]);

	       /* Link to QR code */
	       if (NumNick == 1 && Usr_UsrDat[MeOrOther]->Nickname[0])
		  QR_PutLinkToPrintQRCode (ActPrnUsrQR,
					   Usr_PutParOtherUsrCodEncrypted,Usr_UsrDat[MeOrOther]->EnUsrCod);

	       /* Form to change the nickname */
	       if (NumNick > 1)
		 {
		  HTM_BR ();
		  Frm_BeginFormAnchor (ActNck[MeOrOther].Change,Nck_NICKNAME_SECTION_ID);
		     if (MeOrOther == Usr_OTHER)
			Usr_PutParUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
		     snprintf (NickWithArr,sizeof (NickWithArr),"@%s",row[0]);
		     Par_PutParString (NULL,"NewNick",NickWithArr);	// Nickname
			Btn_PutConfirmButtonInline (Txt_Use_this_nickname);
		  Frm_EndForm ();
		 }

	       if (NumNick == 1 ||
		   NumNick == NumNicks)
		 {
		     HTM_TD_End ();
		  HTM_TR_End ();
		 }
	       else
		  HTM_BR ();
	      }

	    /***** Form to enter new nickname *****/
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("Frm_C1 RT","NewNick",
				NumNicks ? Txt_New_nickname :	// A new nickname
					   Txt_Nickname);	// The first nickname

	       /* Data */
	       HTM_TD_Begin ("class=\"Frm_C2 LT DAT_%s\"",The_GetSuffix ());
		  Frm_BeginFormAnchor (ActNck[MeOrOther].Change,Nck_NICKNAME_SECTION_ID);
		     if (MeOrOther == Usr_OTHER)
			Usr_PutParUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
		     snprintf (NickWithArr,sizeof (NickWithArr),"@%s",
			       Usr_UsrDat[MeOrOther]->Nickname);
		     HTM_INPUT_TEXT ("NewNick",1 + Nck_MAX_CHARS_NICK_WITHOUT_ARROBA,
				     NickWithArr,
				     HTM_NO_ATTR,
				     "id=\"NewNick\""
				     " class=\"Frm_C2_INPUT INPUT_%s\""
				     " size=\"16\"",The_GetSuffix ());
		     HTM_BR ();
		     Btn_PutCreateButtonInline (NumNicks ? Txt_Change_nickname :	// I already have a nickname
							   Txt_Save_changes);	// I have no nickname yet);
		  Frm_EndForm ();
	       HTM_TD_End ();

	    HTM_TR_End ();

      /***** End table and box *****/
      Box_BoxTableEnd ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

static void Nck_PutParsRemoveMyNick (void *Nick)
  {
   if (Nick)
      Par_PutParString (NULL,"Nick",Nick);
  }

static void Nck_PutParsRemoveOtherNick (void *Nick)
  {
   if (Nick)
     {
      Usr_PutParUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
      Par_PutParString (NULL,"Nick",Nick);
     }
  }

/*****************************************************************************/
/***************************** Remove my nickname ****************************/
/*****************************************************************************/

void Nck_RemoveMyNick (void)
  {
   extern const char *Txt_Nickname_X_removed;
   extern const char *Txt_You_can_not_delete_your_current_nickname;
   char NickWithoutArr[Nck_MAX_BYTES_NICK_WITHOUT_ARROBA + 1];

   /***** Get nickname from form *****/
   Par_GetParText ("Nick",NickWithoutArr,
	             Nck_MAX_BYTES_NICK_WITHOUT_ARROBA);

   if (strcasecmp (NickWithoutArr,Gbl.Usrs.Me.UsrDat.Nickname))	// Only if not my current nickname
     {
      /***** Remove one of my old nicknames *****/
      Nck_DB_RemoveNickname (Gbl.Usrs.Me.UsrDat.UsrCod,NickWithoutArr);

      /***** Show message *****/
      Ale_CreateAlert (Ale_SUCCESS,Nck_NICKNAME_SECTION_ID,
	               Txt_Nickname_X_removed,
		       NickWithoutArr);
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
   char NickWithoutArr[Nck_MAX_BYTES_NICK_WITHOUT_ARROBA + 1];

   /***** Get user whose nick must be removed *****/
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
      switch (Usr_CheckIfICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	{
	 case Usr_CAN:
	    /***** Get nickname from form *****/
	    Par_GetParText ("Nick",NickWithoutArr,
			      Nck_MAX_BYTES_NICK_WITHOUT_ARROBA);

	    /***** Remove one of the old nicknames *****/
	    Nck_DB_RemoveNickname (Gbl.Usrs.Other.UsrDat.UsrCod,NickWithoutArr);

	    /***** Show message *****/
	    Ale_CreateAlert (Ale_SUCCESS,Nck_NICKNAME_SECTION_ID,
			     Txt_Nickname_X_removed,
			     NickWithoutArr);

	    /***** Show user's account again *****/
	    Acc_ShowFormChgOtherUsrAccount ();
	    break;
	 case Usr_CAN_NOT:
	 default:
	    Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	}
   else		// User not found
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/***************************** Update my nickname ****************************/
/*****************************************************************************/

void Nck_UpdateMyNick (void)
  {
   /***** Update my nickname *****/
   Nck_ChangeUsrNick (&Gbl.Usrs.Me.UsrDat);

   /***** Show my account again *****/
   Acc_ShowFormChgMyAccount ();
  }

/*****************************************************************************/
/*********************** Change another user's nickname **********************/
/*****************************************************************************/

void Nck_ChangeOtherUsrNick (void)
  {
   /***** Get user whose nick must be changed *****/
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
      switch (Usr_CheckIfICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	{
	 case Usr_CAN:
	    /***** Update user's nickname *****/
	    Nck_ChangeUsrNick (&Gbl.Usrs.Other.UsrDat);

	    /***** Show user's account again *****/
	    Acc_ShowFormChgOtherUsrAccount ();
	    break;
	 case Usr_CAN_NOT:
	 default:
	    Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	}
   else		// User not found
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/*************************** Change user's nickname **************************/
/*****************************************************************************/

static void Nck_ChangeUsrNick (struct Usr_Data *UsrDat)
  {
   extern const char *Txt_The_nickname_matches_the_one_you_had_previously_registered;
   extern const char *Txt_The_nickname_had_been_registered_by_another_user;
   extern const char *Txt_The_nickname_has_been_registered_successfully;
   extern const char *Txt_The_nickname_is_not_valid_;
   char NewNick[Nck_MAX_BYTES_NICK_WITH_ARROBA + 1];

   /***** Get new nickname from form *****/
   Par_GetParText ("NewNick",NewNick,sizeof (NewNick) - 1);

   if (Nck_CheckIfNickWithArrIsValid (NewNick))        // If new nickname is valid
     {
      /***** Remove arrobas at the beginning *****/
      Str_RemoveLeadingArrobas (NewNick);

      /***** Check if new nickname exists in database *****/
      if (!strcmp (UsrDat->Nickname,NewNick))		// User's nickname match exactly the new nickname
         Ale_CreateAlert (Ale_WARNING,Nck_NICKNAME_SECTION_ID,
                          Txt_The_nickname_matches_the_one_you_had_previously_registered);
      else if (strcasecmp (UsrDat->Nickname,NewNick))	// User's nickname does not match, not even case insensitive, the new nickname
        {
         /***** Check if the new nickname matches any of my old nicknames *****/
         if (!Nck_DB_CheckIfNickMatchesAnyUsrNick (UsrDat->UsrCod,NewNick))		// No matches
            /***** Check if the new nickname matches any of the nicknames of other users *****/
            if (Nck_DB_CheckIfNickMatchesAnyOtherUsrsNicks (UsrDat->UsrCod,NewNick))	// A nickname of another user is the same that user's nickname
               Ale_CreateAlert (Ale_WARNING,Nck_NICKNAME_SECTION_ID,
        	                Txt_The_nickname_had_been_registered_by_another_user);
        }
      if (Ale_GetNumAlerts () == 0)	// No problems
        {
         // Now we know the new nickname is not already in database
	 // and is diffent to the current one
         Nck_DB_UpdateNick (UsrDat->UsrCod,NewNick);	// Leading arrobas already removed
         Str_Copy (UsrDat->Nickname,NewNick,sizeof (UsrDat->Nickname) - 1);

         Ale_CreateAlert (Ale_SUCCESS,Nck_NICKNAME_SECTION_ID,
                          Txt_The_nickname_has_been_registered_successfully);
        }
     }
   else        // New nickname is not valid
      Ale_CreateAlert (Ale_WARNING,Nck_NICKNAME_SECTION_ID,
	               Txt_The_nickname_is_not_valid_,
		       Nck_MIN_CHARS_NICK_WITHOUT_ARROBA,
		       Nck_MAX_CHARS_NICK_WITHOUT_ARROBA);
  }
