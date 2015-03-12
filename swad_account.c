// swad_account.c: user's account

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <string.h>		// For string functions

#include "swad_account.h"
#include "swad_announcement.h"
#include "swad_database.h"
#include "swad_enrollment.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_notification.h"
#include "swad_parameter.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

typedef enum
  {
   Acc_REQUEST_REMOVE_USR,
   Acc_REMOVE_USR,
  } Acc_ReqDelOrDelUsr_t;

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Acc_ShowFormRequestNewAccountWithParams (const char *NewNicknameWithoutArroba,
                                                     const char *NewEmail);
static bool Acc_GetParamsNewAccount (char *NewNicknameWithoutArroba,
                                     char *NewEmail,
                                     char *NewEncryptedPassword);
static void Acc_CreateNewEncryptedUsrCod (struct UsrData *UsrDat);

static void Acc_PutLinkToRemoveMyAccount (void);
static void Acc_PrintAccountSeparator (void);

static void Acc_ReqDelOrDelUsrGbl (Acc_ReqDelOrDelUsr_t ReqDelOrDelUsr);

static void Acc_RemoveUsrBriefcase (struct UsrData *UsrDat);
static void Acc_RemoveUsr (struct UsrData *UsrDat);

/*****************************************************************************/
/******** Show form to change my account or to create a new account **********/
/*****************************************************************************/

void Acc_ShowFormAccount (void)
  {
   if (Gbl.Usrs.Me.Logged)
      Acc_ShowFormChangeMyAccount ();
   else
      Acc_ShowFormRequestNewAccountWithParams ("","");
  }

/*****************************************************************************/
/********************* Show form to create a new account *********************/
/*****************************************************************************/

static void Acc_ShowFormRequestNewAccountWithParams (const char *NewNicknameWithoutArroba,
                                                     const char *NewEmail)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Log_in;
   extern const char *Txt_Nickname;
   extern const char *Txt_Email;
   extern const char *Txt_New_on_PLATFORM_Sign_up;
   extern const char *Txt_Create_account;

   /***** Link to log in *****/
   fprintf (Gbl.F.Out,"<div style=\"text-align:center; margin-bottom:20px;\">");
   Act_FormStart (ActFrmLogIn);
   Act_LinkFormSubmit (Txt_Log_in,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("login",Txt_Log_in,Txt_Log_in);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");

   /***** Form to enter some data of the new user *****/
   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
   Act_FormStart (ActCreUsrAcc);
   sprintf (Gbl.Title,Txt_New_on_PLATFORM_Sign_up,Cfg_PLATFORM_SHORT_NAME);
   Lay_StartRoundFrameTable10 (NULL,2,Gbl.Title);

   /***** Nickname *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s\" style=\"text-align:right;\">"
                      "%s: "
                      "</td>"
	              "<td style=\"text-align:left;\">"
                      "<input type=\"text\" name=\"NewNick\""
                      " size=\"16\" maxlength=\"%u\" value=\"@%s\" />"
                      "</td>"
                      "</tr>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Nickname,
            1+Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA,
            NewNicknameWithoutArroba);

   /***** E-mail *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s\" style=\"text-align:right;\">"
                      "%s: "
                      "</td>"
	              "<td style=\"text-align:left;\">"
                      "<input type=\"text\" name=\"NewEmail\""
                      " size=\"16\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>"
                      "</tr>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Email,
            Cns_MAX_BYTES_STRING,
            NewEmail);

   /***** Password *****/
   Pwd_PutFormToGetNewPasswordTwice ();

   /***** Send button and form end *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" style=\"text-align:center;\">"
                      "<input type=\"submit\" value=\"%s\" />"
                      "</td>"
	              "</tr>",
	              Txt_Create_account);
   Lay_EndRoundFrameTable10 ();
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/*********************** Show form to change my account **********************/
/*****************************************************************************/

void Acc_ShowFormChangeMyAccount (void)
  {
   extern const char *Txt_Before_going_to_any_other_option_you_must_fill_your_nickname;
   extern const char *Txt_Before_going_to_any_other_option_you_must_fill_your_email_address;
   extern const char *Txt_Before_going_to_any_other_option_you_must_fill_your_ID;
   extern const char *Txt_User_account;

   /***** Get current user's nickname and e-mail address
          It's necessary because current nickname or e-mail could be just updated *****/
   Nck_GetNicknameFromUsrCod (Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Me.UsrDat.Nickname);
   Mai_GetEmailFromUsrCod (Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Me.UsrDat.Email,&(Gbl.Usrs.Me.UsrDat.EmailConfirmed));

   /***** Check nickname and e-mail *****/
   if (!Gbl.Usrs.Me.UsrDat.Nickname[0])
      Lay_ShowAlert (Lay_WARNING,Txt_Before_going_to_any_other_option_you_must_fill_your_nickname);
   else if (!Gbl.Usrs.Me.UsrDat.Email[0])
      Lay_ShowAlert (Lay_WARNING,Txt_Before_going_to_any_other_option_you_must_fill_your_email_address);
   else if (!Gbl.Usrs.Me.UsrDat.IDs.Num)
      Lay_ShowAlert (Lay_WARNING,Txt_Before_going_to_any_other_option_you_must_fill_your_ID);

   /***** Put links to change my password and to remove my account*****/
   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
   Pwd_PutLinkToChangeUsrPassword (&Gbl.Usrs.Me.UsrDat);
   if (Acc_CheckIfICanEliminateAccount (true))	// ItsMe = true
      Acc_PutLinkToRemoveMyAccount ();
   fprintf (Gbl.F.Out,"</div>");

   /***** Start table *****/
   Lay_StartRoundFrameTable10 ("80%",2,Txt_User_account);

   /***** Nickname *****/
   Nck_ShowFormChangeUsrNickname ();

   /***** Separator *****/
   Acc_PrintAccountSeparator ();

   /***** E-mail *****/
   Mai_ShowFormChangeUsrEmail ();

   /***** Separator *****/
   Acc_PrintAccountSeparator ();

   /***** User's ID *****/
   ID_ShowFormChangeUsrID (&Gbl.Usrs.Me.UsrDat,true);

   /***** End of table *****/
   Lay_EndRoundFrameTable10 ();
  }


/*****************************************************************************/
/******** Put a link to the action used to request user's password ***********/
/*****************************************************************************/

static void Acc_PutLinkToRemoveMyAccount (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Remove_account;

   Act_FormStart (ActUpdOthUsrDat);
   Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Me.UsrDat.EncryptedUsrCod);
   Par_PutHiddenParamUnsigned ("RegRemAction",(unsigned) Enr_ELIMINATE_ONE_USR_FROM_PLATFORM);
   Act_LinkFormSubmit (Txt_Remove_account,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("delon",Txt_Remove_account,Txt_Remove_account);
   Act_FormEnd ();
  }

/*****************************************************************************/
/******* Draw a separator between different parts of new account form ********/
/*****************************************************************************/

static void Acc_PrintAccountSeparator (void)
  {
   extern const char *The_ClassSeparator[The_NUM_THEMES];

   /***** Separator *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td colspan=\"2\" style=\"text-align:center; vertical-align:middle;\">"
		      "<hr class=\"%s\" />"
		      "</td>"
		      "</tr>",
	    The_ClassSeparator[Gbl.Prefs.Theme]);
  }

/*****************************************************************************/
/*************** Create new user account with an ID and login ****************/
/*****************************************************************************/
// Return true if no error and user can be logged in
// Return false on error

bool Acc_CreateNewAccountAndLogIn (void)
  {
   char NewNicknameWithoutArroba[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA+1];
   char NewEmail[Cns_MAX_BYTES_STRING+1];
   char NewEncryptedPassword[Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64+1];

   if (Acc_GetParamsNewAccount (NewNicknameWithoutArroba,NewEmail,NewEncryptedPassword))
     {
      /***** User's has no ID *****/
      Gbl.Usrs.Me.UsrDat.IDs.Num = 0;
      Gbl.Usrs.Me.UsrDat.IDs.List = NULL;

      /***** Set password to the password typed by the user *****/
      strcpy (Gbl.Usrs.Me.UsrDat.Password,NewEncryptedPassword);

      /***** User does not exist in the platform, so create him/her! *****/
      Acc_CreateNewUsr (&Gbl.Usrs.Me.UsrDat);

      /***** Save nickname *****/
      Nck_UpdateMyNick (NewNicknameWithoutArroba);
      strcpy (Gbl.Usrs.Me.UsrDat.Nickname,NewNicknameWithoutArroba);

      /***** Save e-mail *****/
      if (Mai_UpdateEmailInDB (&Gbl.Usrs.Me.UsrDat,NewEmail))
	{
	 /* E-mail updated sucessfully */
	 strcpy (Gbl.Usrs.Me.UsrDat.Email,NewEmail);
	 Gbl.Usrs.Me.UsrDat.EmailConfirmed = false;
	}

      return true;
     }
   else
     {
      /***** Show form again ******/
      Acc_ShowFormRequestNewAccountWithParams (NewNicknameWithoutArroba,NewEmail);
      return false;
     }
  }

/*****************************************************************************/
/************* Get parameters for the creation of a new account **************/
/*****************************************************************************/
// Return false on error

static bool Acc_GetParamsNewAccount (char *NewNicknameWithoutArroba,
                                     char *NewEmail,
                                     char *NewEncryptedPassword)
  {
   extern const char *Txt_The_nickname_X_had_been_registered_by_another_user;
   extern const char *Txt_The_nickname_entered_X_is_not_valid_;
   extern const char *Txt_The_email_address_X_had_been_registered_by_another_user;
   extern const char *Txt_The_email_address_entered_X_is_not_valid;
   extern const char *Txt_You_have_not_written_twice_the_same_new_password;
   char Query[1024];
   char NewNicknameWithArroba[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA+1];
   char NewPlainPassword[2][Pwd_MAX_LENGTH_PLAIN_PASSWORD+1];
   bool Error = false;

   /***** Step 1/3: Get new nickname from form *****/
   Par_GetParToText ("NewNick",NewNicknameWithArroba,Nck_MAX_BYTES_NICKNAME_WITH_ARROBA);

   if (Nck_CheckIfNickWithArrobaIsValid (NewNicknameWithArroba))        // If new nickname is valid
     {
      /***** Remove arrobas at the beginning *****/
      strncpy (NewNicknameWithoutArroba,NewNicknameWithArroba,Nck_MAX_BYTES_NICKNAME_WITH_ARROBA);
      NewNicknameWithoutArroba[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA] = '\0';
      Str_RemoveLeadingArrobas (NewNicknameWithoutArroba);

      /***** Check if the new nickname matches any of the nicknames of other users *****/
      sprintf (Query,"SELECT COUNT(*) FROM usr_nicknames"
		     " WHERE Nickname='%s' AND UsrCod<>'%ld'",
	       NewNicknameWithoutArroba,Gbl.Usrs.Me.UsrDat.UsrCod);
      if (DB_QueryCOUNT (Query,"can not check if nickname already existed"))        // A nickname of another user is the same that this nickname
	{
	 Error = true;
	 sprintf (Gbl.Message,Txt_The_nickname_X_had_been_registered_by_another_user,
		  NewNicknameWithoutArroba);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	}
     }
   else        // New nickname is not valid
     {
      Error = true;
      sprintf (Gbl.Message,Txt_The_nickname_entered_X_is_not_valid_,
               NewNicknameWithArroba,
               Nck_MIN_LENGTH_NICKNAME_WITHOUT_ARROBA,
               Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }

   /***** Step 2/3: Get new e-mail from form *****/
   Par_GetParToText ("NewEmail",NewEmail,Cns_MAX_BYTES_STRING);

   if (Mai_CheckIfEmailIsValid (NewEmail))	// New e-mail is valid
     {
      /***** Check if the new e-mail matches any of the confirmed e-mails of other users *****/
      sprintf (Query,"SELECT COUNT(*) FROM usr_emails"
		     " WHERE E_mail='%s' AND Confirmed='Y'",
	       NewEmail);
      if (DB_QueryCOUNT (Query,"can not check if e-mail already existed"))	// An e-mail of another user is the same that my e-mail
	{
	 Error = true;
	 sprintf (Gbl.Message,Txt_The_email_address_X_had_been_registered_by_another_user,
		  NewEmail);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	}
     }
   else	// New e-mail is not valid
     {
      Error = true;
      sprintf (Gbl.Message,Txt_The_email_address_entered_X_is_not_valid,
               NewEmail);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }

   /***** Step 3/3: Get new user's ID from form *****/
   Par_GetParToText ("Paswd1",NewPlainPassword[0],Pwd_MAX_LENGTH_PLAIN_PASSWORD);
   Par_GetParToText ("Paswd2",NewPlainPassword[1],Pwd_MAX_LENGTH_PLAIN_PASSWORD);
   if (strcmp (NewPlainPassword[0],NewPlainPassword[1]))
     {
      // Passwords don't match
      Error = true;
      Lay_ShowAlert (Lay_WARNING,Txt_You_have_not_written_twice_the_same_new_password);
     }
   else
     {
      Str_ChangeFormat (Str_FROM_FORM,Str_TO_TEXT,
                        NewPlainPassword[0],Pwd_MAX_LENGTH_PLAIN_PASSWORD,true);
      Cry_EncryptSHA512Base64 (NewPlainPassword[0],NewEncryptedPassword);
      if (!Pwd_SlowCheckIfPasswordIsGood (NewPlainPassword[0],NewEncryptedPassword,-1L))        // New password is good?
	{
	 Error = true;
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);	// Error message is set in Usr_SlowCheckIfPasswordIsGood
	}
     }

   return !Error;
  }

/*****************************************************************************/
/****************************** Create new user ******************************/
/*****************************************************************************/
// UsrDat->UsrCod must be <= 0
// UsrDat->UsrDat.IDs must contain a list of IDs for the new user

void Acc_CreateNewUsr (struct UsrData *UsrDat)
  {
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];
   char Query[2048];
   char PathRelUsr[PATH_MAX+1];
   unsigned NumID;

   /***** Check if user's code is initialized *****/
   if (UsrDat->UsrCod > 0)
      Lay_ShowErrorAndExit ("Can not create new user.");

   /***** Create encrypted user's code *****/
   Acc_CreateNewEncryptedUsrCod (UsrDat);

   /***** Filter some user's data before inserting */
   Enr_FilterUsrDat (UsrDat);

   /***** Insert new user in database *****/
   /* Insert user's data */
   sprintf (Query,"INSERT INTO usr_data (EncryptedUsrCod,Password,Surname1,Surname2,FirstName,Sex,"
		  "Layout,Theme,IconSet,Language,PhotoVisibility,ProfileVisibility,"
		  "CtyCod,"
		  "LocalAddress,LocalPhone,FamilyAddress,FamilyPhone,OriginPlace,Birthday,Comments,"
		  "Menu,SideCols,NotifNtfEvents,EmailNtfEvents)"
		  " VALUES ('%s','%s','%s','%s','%s','%s',"
		  "'%u','%s','%s','%s','%s','%s',"
		  "'%ld',"
		  "'%s','%s','%s','%s','%s','%04u-%02u-%02u','%s',"
		  "'%u','%u','-1','0')",
	    UsrDat->EncryptedUsrCod,
	    UsrDat->Password,
	    UsrDat->Surname1,UsrDat->Surname2,UsrDat->FirstName,
	    Usr_StringsSexDB[UsrDat->Sex],
	    (unsigned) UsrDat->Prefs.Layout,
	    The_ThemeId[UsrDat->Prefs.Theme],
	    Ico_IconSetId[UsrDat->Prefs.IconSet],
	    Txt_STR_LANG_ID[UsrDat->Prefs.Language],
            Pri_VisibilityDB[UsrDat->PhotoVisibility],
            Pri_VisibilityDB[UsrDat->ProfileVisibility],
	    UsrDat->CtyCod,
	    UsrDat->LocalAddress ,UsrDat->LocalPhone,
	    UsrDat->FamilyAddress,UsrDat->FamilyPhone,
	    UsrDat->OriginPlace,
	    UsrDat->Birthday.Year,UsrDat->Birthday.Month,UsrDat->Birthday.Day,
	    UsrDat->Comments ? UsrDat->Comments :
		               "",
            Mnu_MENU_DEFAULT,
            Cfg_DEFAULT_COLUMNS);
   UsrDat->UsrCod = DB_QueryINSERTandReturnCode (Query,"can not create user");

   /* Insert user's IDs as confirmed */
   for (NumID = 0;
	NumID < UsrDat->IDs.Num;
	NumID++)
     {
      Str_ConvertToUpperText (UsrDat->IDs.List[NumID].ID);
      sprintf (Query,"INSERT INTO usr_IDs (UsrCod,UsrID,CreatTime,Confirmed)"
		     " VALUES ('%ld','%s',NOW(),'%c')",
	       UsrDat->UsrCod,
	       UsrDat->IDs.List[NumID].ID,
	       UsrDat->IDs.List[NumID].Confirmed ? 'Y' :
		                                   'N');
      DB_QueryINSERT (Query,"can not store user's ID when creating user");
     }

   /***** Create directory for the user, if not exists *****/
   Usr_ConstructPathUsr (UsrDat->UsrCod,PathRelUsr);
   Fil_CreateDirIfNotExists (PathRelUsr);
  }

/*****************************************************************************/
/******************** Create a new encrypted user's code *********************/
/*****************************************************************************/

#define LENGTH_RANDOM_STR 32
#define MAX_TRY 10

static void Acc_CreateNewEncryptedUsrCod (struct UsrData *UsrDat)
  {
   char RandomStr[LENGTH_RANDOM_STR+1];
   unsigned NumTry;

   for (NumTry = 0;
        NumTry < MAX_TRY;
        NumTry++)
     {
      Str_CreateRandomAlphanumStr (RandomStr,LENGTH_RANDOM_STR);
      Cry_EncryptSHA256Base64 (RandomStr,UsrDat->EncryptedUsrCod);
      if (!Usr_ChkIfEncryptedUsrCodExists (UsrDat->EncryptedUsrCod))
          break;
     }
   if (NumTry == MAX_TRY)
      Lay_ShowErrorAndExit ("Can not create a new encrypted user's code.");
   }

/*****************************************************************************/
/***************** Message after creation of a new account *******************/
/*****************************************************************************/

void Acc_AfterCreationNewAccount (void)
  {
   extern const char *Txt_New_account_created;

   if (Gbl.Usrs.Me.Logged)	// If account has been created without problem, I am logged
     {
      /***** Show message of success *****/
      Lay_ShowAlert (Lay_SUCCESS,Txt_New_account_created);

      /***** Show form with account data *****/
      Acc_ShowFormChangeMyAccount ();
     }
  }

/*****************************************************************************/
/********* Request definite removing of a user from the platform *************/
/*****************************************************************************/

void Acc_ReqRemUsrGbl (void)
  {
   Acc_ReqDelOrDelUsrGbl (Acc_REQUEST_REMOVE_USR);
  }

/*****************************************************************************/
/************** Definite removing of a user from the platform ****************/
/*****************************************************************************/

void Acc_RemUsrGbl (void)
  {
   Acc_ReqDelOrDelUsrGbl (Acc_REMOVE_USR);
  }

/*****************************************************************************/
/**************************** Removing of a user *****************************/
/*****************************************************************************/

static void Acc_ReqDelOrDelUsrGbl (Acc_ReqDelOrDelUsr_t ReqDelOrDelUsr)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   bool ItsMe;

   /***** Get user to be removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);
      if (Acc_CheckIfICanEliminateAccount (ItsMe))
         switch (ReqDelOrDelUsr)
           {
            case Acc_REQUEST_REMOVE_USR:	// Ask if eliminate completely the user from the platform
               Acc_AskIfCompletelyEliminateAccount (ItsMe);
               break;
            case Acc_REMOVE_USR:		// Eliminate completely the user from the platform
               if (Pwd_GetConfirmationOnDangerousAction ())
        	 {
                  Acc_CompletelyEliminateAccount (&Gbl.Usrs.Other.UsrDat,Cns_VERBOSE);

                  /***** Move unused contents of messages to table of deleted contents of messages *****/
                  Msg_MoveUnusedMsgsContentToDeleted ();
        	 }
               break;
           }
      else
         Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/******** Check if I can eliminate completely another user's account *********/
/*****************************************************************************/

bool Acc_CheckIfICanEliminateAccount (bool ItsMe)
  {
   // A user logged as superuser can eliminate any user except her/him
   // Other users only can eliminate themselves
   return (( ItsMe &&								// It's me
	    (Gbl.Usrs.Me.AvailableRoles & (1 << Rol_ROLE_SYS_ADM)) == 0)	// I can not be superuser
	   ||
           (!ItsMe &&								// It's not me
             Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM));			// I am logged as superuser
  }

/*****************************************************************************/
/*********** Ask if really wanted to eliminate completely a user *************/
/*****************************************************************************/

void Acc_AskIfCompletelyEliminateAccount (bool ItsMe)
  {
   extern const char *Txt_Do_you_really_want_to_completely_eliminate_your_user_account;
   extern const char *Txt_Do_you_really_want_to_completely_eliminate_the_following_user;
   extern const char *Txt_Completely_eliminate_me;
   extern const char *Txt_Completely_eliminate_user;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   if (Usr_ChkIfUsrCodExists (Gbl.Usrs.Other.UsrDat.UsrCod))
     {
      Lay_ShowAlert (Lay_WARNING,ItsMe ? Txt_Do_you_really_want_to_completely_eliminate_your_user_account :
				         Txt_Do_you_really_want_to_completely_eliminate_the_following_user);

      Rec_ShowCommonRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

      Act_FormStart (ActRemUsrGbl);
      Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);

      /* Ask for consent on dangerous actions */
      Pwd_AskForConfirmationOnDangerousAction ();

      fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">"
	                 "<input type=\"submit\" value=\"%s\" />"
	                 "</div>",
               ItsMe ? Txt_Completely_eliminate_me :
                       Txt_Completely_eliminate_user);
      Act_FormEnd ();
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/************* Remove completely a user from the whole platform **************/
/*****************************************************************************/

void Acc_CompletelyEliminateAccount (struct UsrData *UsrDat,
                                     Cns_QuietOrVerbose_t QuietOrVerbose)
  {
   extern const char *Txt_THE_USER_X_has_been_removed_from_all_his_her_courses;
   extern const char *Txt_THE_USER_X_has_been_removed_as_administrator;
   extern const char *Txt_Messages_of_THE_USER_X_have_been_deleted;
   extern const char *Txt_Virtual_pendrive_of_THE_USER_X_has_been_removed;
   extern const char *Txt_Photo_of_THE_USER_X_has_been_removed;
   extern const char *Txt_Record_card_of_THE_USER_X_has_been_removed;
   char Query[1024];
   bool PhotoRemoved = false;

   /***** Remove the works zones of the user in all courses *****/
   Brw_RemoveUsrWorksInAllCrss (UsrDat,QuietOrVerbose);        // Make this before of removing the user from the courses

   /***** Remove the fields of course record in all courses *****/
   Rec_RemoveFieldsCrsRecordAll (UsrDat->UsrCod,QuietOrVerbose);

   /***** Remove user from all the attendance events *****/
   Att_RemoveUsrFromAllAttEvents (UsrDat->UsrCod);

   /***** Remove user from all the groups of all courses *****/
   Grp_RemUsrFromAllGrps (UsrDat,QuietOrVerbose);

   /***** Remove user's requests for inscription *****/
   sprintf (Query,"DELETE FROM crs_usr_requests"
                  " WHERE UsrCod='%ld'",
            UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove user's requests for inscription");

   /***** Remove user from the table of courses and users *****/
   sprintf (Query,"DELETE FROM crs_usr"
                  " WHERE UsrCod='%ld'",
            UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove a user from all courses");

   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_THE_USER_X_has_been_removed_from_all_his_her_courses,
               UsrDat->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Remove user as administrator of any degree *****/
   sprintf (Query,"DELETE FROM admin"
                  " WHERE UsrCod='%ld'",
            UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove a user as administrator");

   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_THE_USER_X_has_been_removed_as_administrator,
               UsrDat->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Remove user's clipboard in forums *****/
   For_RemoveUsrFromThrClipboard (UsrDat->UsrCod);

   /***** Remove some files of the user's from database *****/
   Brw_RemoveUsrFilesFromDB (UsrDat->UsrCod);

   /***** Remove the file tree of a user *****/
   Acc_RemoveUsrBriefcase (UsrDat);
   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_Virtual_pendrive_of_THE_USER_X_has_been_removed,
               UsrDat->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Remove exams made by user in all courses *****/
   Tst_RemoveExamsMadeByUsrInAllCrss (UsrDat->UsrCod);

   /***** Remove user's notifications *****/
   Ntf_RemoveUsrNtfs (UsrDat->UsrCod);

   /***** Delete user's messages sent and received *****/
   Gbl.Msg.FilterContent[0] = '\0';
   Msg_DelAllRecAndSntMsgsUsr (UsrDat->UsrCod);
   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_Messages_of_THE_USER_X_have_been_deleted,
               UsrDat->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Remove user from table of banned users *****/
   Msg_RemoveUsrFromBanned (UsrDat->UsrCod);

   /***** Delete thread read status for this user *****/
   For_RemoveUsrFromReadThrs (UsrDat->UsrCod);

   /***** Remove user from table of seen announcements *****/
   Ann_RemoveUsrFromSeenAnnouncements (UsrDat->UsrCod);

   /***** Remove user from table of connected users *****/
   sprintf (Query,"DELETE FROM connected WHERE UsrCod='%ld'",
            UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove a user from table of connected users");

   /***** Remove all sessions of this user *****/
   sprintf (Query,"DELETE FROM sessions WHERE UsrCod='%ld'",
            UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove sessions of a user");

   /***** Remove user's figures *****/
   Usr_RemoveUsrFigures (UsrDat->UsrCod);

   /***** Remove the user from the list of users without photo *****/
   Pho_RemoveUsrFromTableClicksWithoutPhoto (UsrDat->UsrCod);

   /***** Remove user's photo *****/
   PhotoRemoved = Pho_RemovePhoto (UsrDat);
   if (PhotoRemoved && QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_Photo_of_THE_USER_X_has_been_removed,
               UsrDat->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Remove user *****/
   Acc_RemoveUsr (UsrDat);
   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_Record_card_of_THE_USER_X_has_been_removed,
               UsrDat->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
  }

/*****************************************************************************/
/********************** Remove the briefcase of a user ***********************/
/*****************************************************************************/

static void Acc_RemoveUsrBriefcase (struct UsrData *UsrDat)
  {
   char PathRelUsr[PATH_MAX+1];

   /***** Remove files of the user's briefcase from disc *****/
   Usr_ConstructPathUsr (UsrDat->UsrCod,PathRelUsr);
   Brw_RemoveTree (PathRelUsr);
  }

/*****************************************************************************/
/************************ Remove a user from database ************************/
/*****************************************************************************/

static void Acc_RemoveUsr (struct UsrData *UsrDat)
  {
   char Query[128];

   /***** Remove user's webs / social networks *****/
   sprintf (Query,"DELETE FROM usr_webs WHERE UsrCod='%ld'",
	    UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove user's webs / social networks");

   /***** Remove user's nicknames *****/
   sprintf (Query,"DELETE FROM usr_nicknames WHERE UsrCod='%ld'",
	    UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove user's nicknames");

   /***** Remove user's e-mails *****/
   sprintf (Query,"DELETE FROM pending_emails WHERE UsrCod='%ld'",
	    UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove pending user's e-mails");

   sprintf (Query,"DELETE FROM usr_emails WHERE UsrCod='%ld'",
	    UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove user's e-mails");

   /***** Remove user's IDs *****/
   sprintf (Query,"DELETE FROM usr_IDs WHERE UsrCod='%ld'",
	    UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove user's IDs");

   /***** Remove user's last data *****/
   sprintf (Query,"DELETE FROM usr_last WHERE UsrCod='%ld'",
	    UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove user's last data");

   /***** Remove user's data  *****/
   sprintf (Query,"DELETE FROM usr_data WHERE UsrCod='%ld'",
	    UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove user's data");
  }
