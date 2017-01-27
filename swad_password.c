// swad_password.c: Users' passwords

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#include <stdlib.h>		// For system, getenv, etc.
#include <string.h>		// For string functions
#include <sys/wait.h>		// For the macro WEXITSTATUS
#include <unistd.h>		// For unlink

#include "swad_database.h"
#include "swad_enrollment.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_password.h"
#include "swad_parameter.h"
#include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

// If there are X users with the same password,
// it means than the password is trivial
// and another user can not change his/her password to this
#define Pwd_MAX_OTHER_USERS_USING_THE_SAME_PASSWORD 2

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Pwd_PutLinkToSendNewPasswdParams (void);

static void Pwd_CreateANewPassword (char PlainPassword[Pwd_MAX_LENGTH_PLAIN_PASSWORD+1]);

static bool Pwd_CheckIfPasswdIsUsrIDorName (const char *PlainPassword);
static unsigned Pwd_GetNumOtherUsrsWhoUseThisPassword (const char *EncryptedPassword,long UsrCod);

/*****************************************************************************/
/************* Get parameter with my plain password from a form **************/
/*****************************************************************************/

void Pwd_GetParamUsrPwdLogin (void)
  {
   /***** Get plain password from form *****/
   Par_GetParToText ("UsrPwd",Gbl.Usrs.Me.LoginPlainPassword,Pwd_MAX_LENGTH_PLAIN_PASSWORD);

   /***** Encrypt password *****/
   Cry_EncryptSHA512Base64 (Gbl.Usrs.Me.LoginPlainPassword,Gbl.Usrs.Me.LoginEncryptedPassword);
  }

/*****************************************************************************/
/**** Check if login password is the same as current password in database ****/
/*****************************************************************************/
// Returns true if there's no current password in database, or if login password is the same
// Returns false if there's a current password in database and is not the same as the login password

bool Pwd_CheckCurrentPassword (void)
  {
   return (Gbl.Usrs.Me.UsrDat.Password[0] ?
           !strcmp (Gbl.Usrs.Me.LoginEncryptedPassword,Gbl.Usrs.Me.UsrDat.Password) : true);
  }

/*****************************************************************************/
/**** Check if login password is the same as current password in database ****/
/*****************************************************************************/
// Returns true if there's pending password in database and login password is the same
// Returns false if there's no a pending password in database, or if pending password is not the same as the login password

bool Pwd_CheckPendingPassword (void)
  {
   char Query[256+Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get pending password from database *****/
   sprintf (Query,"SELECT PendingPassword FROM pending_passwd"
                  " WHERE UsrCod='%ld'",
            Gbl.Usrs.Me.UsrDat.UsrCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get pending password"))
     {
      /* Get encrypted pending password */
      row = mysql_fetch_row (mysql_res);
      Str_Copy (Gbl.Usrs.Me.PendingPassword,row[0],
                Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64);
     }
   else
      Gbl.Usrs.Me.PendingPassword[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return (Gbl.Usrs.Me.PendingPassword[0] ?
           !strcmp (Gbl.Usrs.Me.LoginEncryptedPassword,Gbl.Usrs.Me.PendingPassword) :
           false);
  }

/*****************************************************************************/
/************ Assign my pending password to my current password **************/
/*****************************************************************************/

void Pwd_AssignMyPendingPasswordToMyCurrentPassword (void)
  {
   char Query[128 + Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64];

   /***** Update my current password in database *****/
   sprintf (Query,"UPDATE usr_data SET Password='%s'"
	          " WHERE UsrCod='%ld'",
            Gbl.Usrs.Me.PendingPassword,
            Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update your password");

   /***** Update my current password *****/
   Str_Copy (Gbl.Usrs.Me.UsrDat.Password,Gbl.Usrs.Me.PendingPassword,
             Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64);
  }

/*****************************************************************************/
/*********************** Change my password in database **********************/
/*****************************************************************************/

void Pwd_ActChgMyPwd1 (void)
  {
   extern const char *Txt_You_have_not_written_twice_the_same_new_password;
   extern const char *Txt_Your_password_has_been_changed_successfully;
   extern const char *Txt_You_have_not_entered_your_password_correctly;
   char PlainPassword[Pwd_MAX_LENGTH_PLAIN_PASSWORD+1];
   char NewPlainPassword[2][Pwd_MAX_LENGTH_PLAIN_PASSWORD+1];
   char NewEncryptedPassword[Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64+1];

   /***** Get plain password from form *****/
   Par_GetParToText ("UsrPwd",PlainPassword,Pwd_MAX_LENGTH_PLAIN_PASSWORD);

   /***** Encrypt password *****/
   Cry_EncryptSHA512Base64 (PlainPassword,Gbl.Usrs.Me.LoginEncryptedPassword);

   Gbl.Usrs.Error = true;
   if (Pwd_CheckCurrentPassword ())
     {
      Par_GetParToText ("Paswd1",NewPlainPassword[0],Pwd_MAX_LENGTH_PLAIN_PASSWORD);
      Par_GetParToText ("Paswd2",NewPlainPassword[1],Pwd_MAX_LENGTH_PLAIN_PASSWORD);

      /***** Check if I have written twice the same password *****/
      if (strcmp (NewPlainPassword[0],NewPlainPassword[1]))
         // Passwords don't match
         sprintf (Gbl.Message,"%s",Txt_You_have_not_written_twice_the_same_new_password);
      else
        {
         Str_ChangeFormat (Str_FROM_FORM,Str_TO_TEXT,
                           NewPlainPassword[0],Pwd_MAX_LENGTH_PLAIN_PASSWORD,true);
         Cry_EncryptSHA512Base64 (NewPlainPassword[0],NewEncryptedPassword);
         if (Pwd_SlowCheckIfPasswordIsGood (NewPlainPassword[0],NewEncryptedPassword,Gbl.Usrs.Me.UsrDat.UsrCod))        // New password is good?
           {
            Str_Copy (Gbl.Usrs.Me.UsrDat.Password,NewEncryptedPassword,
                      Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64);
            Ses_UpdateSessionDataInDB ();
            Enr_UpdateUsrData (&Gbl.Usrs.Me.UsrDat);
            sprintf (Gbl.Message,"%s",Txt_Your_password_has_been_changed_successfully);
            Gbl.Usrs.Error = false;
           }
        }
     }
   else
      sprintf (Gbl.Message,"%s",Txt_You_have_not_entered_your_password_correctly);
  }

void Pwd_ActChgMyPwd2 (void)
  {
   /***** Write error message when updating password *****/
   Lay_ShowAlert (Gbl.Usrs.Error ? Lay_WARNING :
	                           Lay_INFO,
	          Gbl.Message);

   /***** Retry? *****/
   if (Gbl.Usrs.Error)
      Pwd_ShowFormChgPwd ();
  }

/*****************************************************************************/
/*************** Show form to send a new password by email *******************/
/*****************************************************************************/

void Pwd_PutLinkToSendNewPasswd (void)
  {
   extern const char *Txt_Forgotten_password;

   Lay_PutContextualLink (ActReqSndNewPwd,Pwd_PutLinkToSendNewPasswdParams,
                          "key64x64.gif",
                          Txt_Forgotten_password,Txt_Forgotten_password,
                          NULL);
  }

static void Pwd_PutLinkToSendNewPasswdParams (void)
  {
   Par_PutHiddenParamString ("UsrId",Gbl.Usrs.Me.UsrIdLogin);
  }

/*****************************************************************************/
/*************** Show form to send a new password by email *******************/
/*****************************************************************************/

void Pwd_ShowFormSendNewPwd (void)
  {
   extern const char *Hlp_PROFILE_Password;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_If_you_have_forgotten_your_password_;
   extern const char *Txt_Forgotten_password;
   extern const char *Txt_nick_email_or_ID;
   extern const char *Txt_Get_a_new_password;

   /***** Start of form *****/
   Act_FormStart (ActSndNewPwd);

   /***** Start frame *****/
   Lay_StartRoundFrame (NULL,Txt_Forgotten_password,NULL,Hlp_PROFILE_Password);

   /***** Help text *****/
   Lay_ShowAlert (Lay_INFO,Txt_If_you_have_forgotten_your_password_);

   /***** User's ID/nickname *****/
   fprintf (Gbl.F.Out,"<label class=\"%s\">"
	              "%s:&nbsp;"
                      "<input type=\"text\" name=\"UsrId\""
                      " size=\"8\" maxlength=\"%u\" value=\"%s\" />"
                      "</label>",
            The_ClassForm[Gbl.Prefs.Theme],Txt_nick_email_or_ID,
            Usr_MAX_LENGTH_USR_LOGIN,Gbl.Usrs.Me.UsrIdLogin);

   /***** Send button and end frame *****/
   Lay_EndRoundFrameWithButton (Lay_CONFIRM_BUTTON,Txt_Get_a_new_password);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/*********************** Send a new password by email ************************/
/*****************************************************************************/

void Pwd_ChkIdLoginAndSendNewPwd (void)
  {
   extern const char *Txt_You_must_enter_your_nick_email_or_ID;
   extern const char *Txt_There_was_a_problem_sending_an_email_automatically;
   extern const char *Txt_If_you_have_written_your_ID_nickname_or_email_correctly_;
   extern const char *Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email;
   struct ListUsrCods ListUsrCods;
   char NewRandomPlainPassword[Pwd_MAX_LENGTH_PLAIN_PASSWORD+1];
   int ReturnCode;

   /***** Check if user's ID or nickname is not empty *****/
   if (!Gbl.Usrs.Me.UsrIdLogin[0])
     {
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_enter_your_nick_email_or_ID);
      Pwd_ShowFormSendNewPwd ();
      return;
     }

   /***** Reset default list of users' codes *****/
   ListUsrCods.NumUsrs = 0;
   ListUsrCods.Lst = NULL;

   /***** Check if user exists *****/
   /* Check if user has typed his user's ID or his nickname */
   if (Nck_CheckIfNickWithArrobaIsValid (Gbl.Usrs.Me.UsrIdLogin))	// 1: It's a nickname
     {
      if ((Gbl.Usrs.Me.UsrDat.UsrCod = Nck_GetUsrCodFromNickname (Gbl.Usrs.Me.UsrIdLogin)) > 0)
	{
         /* Get user's data */
	 ListUsrCods.NumUsrs = 1;
	 Usr_AllocateListUsrCods (&ListUsrCods);
	 ListUsrCods.Lst[0] = Gbl.Usrs.Other.UsrDat.UsrCod;
	}
     }
   else if (Mai_CheckIfEmailIsValid (Gbl.Usrs.Me.UsrIdLogin))		// 2: It's an email
     {
      if ((Gbl.Usrs.Me.UsrDat.UsrCod = Mai_GetUsrCodFromEmail (Gbl.Usrs.Me.UsrIdLogin)) > 0)
	{
         /* Get user's data */
	 ListUsrCods.NumUsrs = 1;
	 Usr_AllocateListUsrCods (&ListUsrCods);
	 ListUsrCods.Lst[0] = Gbl.Usrs.Other.UsrDat.UsrCod;
        }
     }
   else									// 3: It's not a nickname nor email
     {
      // Users' IDs are always stored internally in capitals and without leading zeros
      Str_RemoveLeadingZeros (Gbl.Usrs.Me.UsrIdLogin);
      if (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Me.UsrIdLogin))
	{
	 /***** Allocate space for the list *****/
	 ID_ReallocateListIDs (&Gbl.Usrs.Me.UsrDat,1);

	 // User has typed a user's ID
	 Str_Copy (Gbl.Usrs.Me.UsrDat.IDs.List[0].ID,Gbl.Usrs.Me.UsrIdLogin,
	           ID_MAX_LENGTH_USR_ID);
         Str_ConvertToUpperText (Gbl.Usrs.Me.UsrDat.IDs.List[0].ID);

	 /* Get users' codes for this ID */
	 if (!ID_GetListUsrCodsFromUsrID (&Gbl.Usrs.Me.UsrDat,NULL,&ListUsrCods,true))	// Only confirmed IDs
	    // If no users found with confirmed IDs, try to get all users (confirmed or not)
	    ID_GetListUsrCodsFromUsrID (&Gbl.Usrs.Me.UsrDat,NULL,&ListUsrCods,false);	// All users (confirmed or not)
	}
     }

   /***** Send a new password via email when user exists *****/
   if (ListUsrCods.NumUsrs)
     {
      if (ListUsrCods.NumUsrs == 1)
	{
	 Usr_GetUsrDataFromUsrCod (&Gbl.Usrs.Me.UsrDat);	// Get my data

	 if (Gbl.Usrs.Me.UsrDat.Email[0])
	    switch ((ReturnCode = Pwd_SendNewPasswordByEmail (NewRandomPlainPassword)))
	      {
	       case 0: // Message sent successfully
		  Pwd_SetMyPendingPassword (NewRandomPlainPassword);
		  Lay_ShowAlert (Lay_INFO,Txt_If_you_have_written_your_ID_nickname_or_email_correctly_);
		  break;
	       case 1:
		  Lay_ShowAlert (Lay_WARNING,Txt_There_was_a_problem_sending_an_email_automatically);
		  break;
	       default:
		  sprintf (Gbl.Message,"Internal error: an email message has not been sent successfully."
				       " Error code returned by the script: %d",
			   ReturnCode);
		  Lay_ShowAlert (Lay_ERROR,Gbl.Message);
		  break;
	      }
	 else	// I have no email address
	    /***** Help message *****/
	    Lay_ShowAlert (Lay_INFO,Txt_If_you_have_written_your_ID_nickname_or_email_correctly_);
	}
      else	// ListUsrCods.NumUsrs > 1
	{
	 /***** Help message *****/
	 // TODO: This message allows to know if a ID exists in database (when no unique). This should be hidden!
	 sprintf (Gbl.Message,Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email,
		  Gbl.Usrs.Me.UsrIdLogin);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);

	 Pwd_ShowFormSendNewPwd ();
	}

      /***** Free list of users' codes *****/
      Usr_FreeListUsrCods (&ListUsrCods);
     }
   else        // ListUsrCods.NumUsrs == 0 ==> user does not exist
      /***** Help message *****/
      Lay_ShowAlert (Lay_INFO,Txt_If_you_have_written_your_ID_nickname_or_email_correctly_);
  }

/*****************************************************************************/
/*********************** Send a new password by email ************************/
/*****************************************************************************/
// Gbl.Usrs.Me.UsrDat must be filled
// Return code returned by command

int Pwd_SendNewPasswordByEmail (char NewRandomPlainPassword[Pwd_MAX_LENGTH_PLAIN_PASSWORD+1])
  {
   extern const char *Txt_The_following_password_has_been_assigned_to_you_to_log_in_X_NO_HTML;
   extern const char *Txt_New_password_NO_HTML[1+Txt_NUM_LANGUAGES];
   char Command[2048]; // Command to execute for sending an email
   int ReturnCode;

   /***** Create temporary file for mail content *****/
   Mai_CreateFileNameMail ();

   /***** Create a new random password *****/
   Pwd_CreateANewPassword (NewRandomPlainPassword);

   /***** Write mail content into file and close file *****/
   /* Welcome note */
   Mai_WriteWelcomeNoteEMail (&Gbl.Usrs.Me.UsrDat);

   /* Message body */
   fprintf (Gbl.Msg.FileMail,Txt_The_following_password_has_been_assigned_to_you_to_log_in_X_NO_HTML,
	    Cfg_PLATFORM_SHORT_NAME,NewRandomPlainPassword,
	    Cfg_URL_SWAD_CGI,(unsigned) (Cfg_TIME_TO_DELETE_OLD_PENDING_PASSWORDS/(24L*60L*60L)),
	    Gbl.Usrs.Me.UsrDat.Email);

   /* Footer note */
   Mai_WriteFootNoteEMail (Gbl.Prefs.Language);

   fclose (Gbl.Msg.FileMail);

   /***** Call the script to send an email *****/
   sprintf (Command,"%s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"[%s] %s\" \"%s\"",
	    Cfg_COMMAND_SEND_AUTOMATIC_EMAIL,
	    Cfg_AUTOMATIC_EMAIL_SMTP_SERVER,
	    Cfg_AUTOMATIC_EMAIL_SMTP_PORT,
	    Cfg_AUTOMATIC_EMAIL_FROM,
            Gbl.Config.SMTPPassword,
	    Gbl.Usrs.Me.UsrDat.Email,
	    Cfg_PLATFORM_SHORT_NAME,
	    Txt_New_password_NO_HTML[Gbl.Usrs.Me.UsrDat.Prefs.Language],
	    Gbl.Msg.FileNameMail);
   ReturnCode = system (Command);
   if (ReturnCode == -1)
      Lay_ShowErrorAndExit ("Error when running script to send email.");

   /***** Remove temporary file *****/
   unlink (Gbl.Msg.FileNameMail);

   /***** Write message depending on return code *****/
   return WEXITSTATUS (ReturnCode);
  }

/*****************************************************************************/
/*********************** Create a new random password ************************/
/*****************************************************************************/

static void Pwd_CreateANewPassword (char PlainPassword[Pwd_MAX_LENGTH_PLAIN_PASSWORD+1])
  {
   Str_CreateRandomAlphanumStr (PlainPassword,Pwd_MIN_LENGTH_PLAIN_PASSWORD);
  }

/*****************************************************************************/
/*************************** Set my pending password *************************/
/*****************************************************************************/

void Pwd_SetMyPendingPassword (char PlainPassword[Pwd_MAX_LENGTH_PLAIN_PASSWORD+1])
  {
   char Query[512];

   /***** Encrypt my pending password *****/
   Cry_EncryptSHA512Base64 (PlainPassword,Gbl.Usrs.Me.PendingPassword);

   /***** Remove expired pending passwords from database *****/
   sprintf (Query,"DELETE FROM pending_passwd"
                  " WHERE DateAndTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-'%lu')",
            Cfg_TIME_TO_DELETE_OLD_PENDING_PASSWORDS);
   DB_QueryDELETE (Query,"can not remove expired pending passwords");

   /***** Update my current password in database *****/
   sprintf (Query,"REPLACE INTO pending_passwd (UsrCod,PendingPassword,DateAndTime)"
                  " VALUES ('%ld','%s',NOW())",
            Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Me.PendingPassword);
   DB_QueryREPLACE (Query,"can not create pending password");
  }

/*****************************************************************************/
/******************************** Update password ****************************/
/*****************************************************************************/

void Pwd_UpdateOtherPwd1 (void)
  {
   extern const char *Txt_You_have_not_written_twice_the_same_new_password;
   extern const char *Txt_The_X_password_has_been_changed_successfully;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char NewPlainPassword[2][Pwd_MAX_LENGTH_PLAIN_PASSWORD+1];
   char NewEncryptedPassword[Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64+1];

   Gbl.Usrs.Error = true;

   /***** Get other user's code from form and get user's data *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Usr_AsAdminICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	{
	 Par_GetParToText ("Paswd1",NewPlainPassword[0],Pwd_MAX_LENGTH_PLAIN_PASSWORD);
	 Par_GetParToText ("Paswd2",NewPlainPassword[1],Pwd_MAX_LENGTH_PLAIN_PASSWORD);

	 if (strcmp (NewPlainPassword[0],NewPlainPassword[1]))
	    // Paswords don't match
	    sprintf (Gbl.Message,"%s",Txt_You_have_not_written_twice_the_same_new_password);
	 else
	   {
	    Str_ChangeFormat (Str_FROM_FORM,Str_TO_TEXT,
			      NewPlainPassword[0],Pwd_MAX_LENGTH_PLAIN_PASSWORD,true);
	    Cry_EncryptSHA512Base64 (NewPlainPassword[0],NewEncryptedPassword);
	    if (Pwd_SlowCheckIfPasswordIsGood (NewPlainPassword[0],NewEncryptedPassword,Gbl.Usrs.Other.UsrDat.UsrCod))        // Good password
	      {
	       /* Update other user's data */
	       Str_Copy (Gbl.Usrs.Other.UsrDat.Password,NewEncryptedPassword,
	                 Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64);
	       Enr_UpdateUsrData (&Gbl.Usrs.Other.UsrDat);

	       sprintf (Gbl.Message,Txt_The_X_password_has_been_changed_successfully,
			Gbl.Usrs.Other.UsrDat.FullName);
	       Gbl.Usrs.Error = false;
	      }
	   }
	}
      else
	 sprintf (Gbl.Message,"%s",Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else		// User not found
     sprintf (Gbl.Message,"%s",Txt_User_not_found_or_you_do_not_have_permission_);
  }

void Pwd_UpdateOtherPwd2 (void)
  {
   /***** Write message when updating the password *****/
   if (Gbl.Message[0])
      Lay_ShowAlert (Gbl.Usrs.Error ? Lay_WARNING :
	                              Lay_INFO,
	             Gbl.Message);

   if (Gbl.Usrs.Error)
     {
      /***** Try again *****/
      Gbl.Usrs.Error = false;
      Pwd_ShowFormOthPwd ();
     }
  }

/*****************************************************************************/
/************************ Check if a password is good ************************/
/*****************************************************************************/

bool Pwd_SlowCheckIfPasswordIsGood (const char *PlainPassword,
                                    const char *EncryptedPassword,
                                    long UsrCod)
  {
   extern const char *Txt_The_password_is_too_trivial_;

   /***** Check if password seems good by making fast checks *****/
   if (!Pwd_FastCheckIfPasswordSeemsGood (PlainPassword))
      return false;

   /***** Check if password is found in user's ID, first name or surnames of anybody *****/
   if (Pwd_CheckIfPasswdIsUsrIDorName (PlainPassword))        // PlainPassword is a user's ID, name or surname
     {
      sprintf (Gbl.Message,"%s",Txt_The_password_is_too_trivial_);
      return false;
     }

   /***** Check if password is used by too many other users *****/
   if (Pwd_GetNumOtherUsrsWhoUseThisPassword (EncryptedPassword,UsrCod) >
       Pwd_MAX_OTHER_USERS_USING_THE_SAME_PASSWORD)
     {
      sprintf (Gbl.Message,"%s",Txt_The_password_is_too_trivial_);
      return false;
     }

   return true;
  }

/*****************************************************************************/
/***** Check if a password is a user's ID, a first name or a surname *********/
/*****************************************************************************/

static bool Pwd_CheckIfPasswdIsUsrIDorName (const char *PlainPassword)
  {
   char Query[128+3*Pwd_MAX_LENGTH_PLAIN_PASSWORD];
   bool Found;

   /***** Get if password is found in user's ID from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_IDs WHERE UsrID='%s'",
            PlainPassword);
   Found = (DB_QueryCOUNT (Query,"can not check if a password matches a user's ID") != 0);

   /***** Get if password is found in first name or surnames of anybody, from database *****/
   if (!Found)
     {
      sprintf (Query,"SELECT COUNT(*) FROM usr_data"
		     " WHERE FirstName='%s' OR Surname1='%s' OR Surname2='%s'",
	       PlainPassword,PlainPassword,PlainPassword);
      Found = (DB_QueryCOUNT (Query,"can not check if a password matches a first name or a surname") != 0);
     }

   return Found;
  }

/*****************************************************************************/
/************** Get the number of users who use yet a password ***************/
/*****************************************************************************/

static unsigned Pwd_GetNumOtherUsrsWhoUseThisPassword (const char *EncryptedPassword,long UsrCod)
  {
   char Query[512];

   /***** Get number of other users who use a password from database *****/
   /* Query database */
   if (UsrCod > 0)
      sprintf (Query,"SELECT COUNT(*) FROM usr_data"
		     " WHERE Password='%s' AND UsrCod<>'%ld'",
	       EncryptedPassword,UsrCod);
   else
      sprintf (Query,"SELECT COUNT(*) FROM usr_data"
		     " WHERE Password='%s'",
	       EncryptedPassword);
   return (unsigned) DB_QueryCOUNT (Query,"can not check if a password is trivial");
  }

/*****************************************************************************/
/********************** Check if a password seems good ***********************/
/*****************************************************************************/

bool Pwd_FastCheckIfPasswordSeemsGood (const char *PlainPassword)
  {
   extern const char *Txt_The_password_must_be_at_least_X_characters;
   extern const char *Txt_The_password_can_not_contain_spaces;
   extern const char *Txt_The_password_can_not_consist_only_of_digits;
   unsigned LengthPassword = strlen (PlainPassword),i;
   bool ItsANumber;

   /***** Check length of password *****/
   if (LengthPassword < Pwd_MIN_LENGTH_PLAIN_PASSWORD)	// PlainPassword too short
     {
      sprintf (Gbl.Message,Txt_The_password_must_be_at_least_X_characters,
               Pwd_MIN_LENGTH_PLAIN_PASSWORD);
      return false;
     }

   /***** Check spaces in password *****/
   if (strchr (PlainPassword,(int) ' ') != NULL)        // PlainPassword with spaces
     {
      sprintf (Gbl.Message,"%s",Txt_The_password_can_not_contain_spaces);
      return false;
     }

   /***** Check if password is a number *****/
   for (i = 0, ItsANumber = true;
        i < LengthPassword && ItsANumber;
        i++)
      if (PlainPassword[i] < '0' || PlainPassword[i] > '9')
         ItsANumber = false;
   if (ItsANumber)
     {
      sprintf (Gbl.Message,"%s",Txt_The_password_can_not_consist_only_of_digits);
      return false;
     }

   return true;
  }

/*****************************************************************************/
/********************** Show form for changing my password *******************/
/*****************************************************************************/

void Pwd_ShowFormChgPwd (void)
  {
   extern const char *Hlp_PROFILE_Password;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Before_going_to_any_other_option_you_must_create_your_password;
   extern const char *Txt_Your_password_is_not_secure_enough;
   extern const char *Txt_Your_password_must_be_at_least_X_characters_and_can_not_contain_spaces_;
   extern const char *Txt_Password;
   extern const char *Txt_Current_password;
   extern const char *Txt_Change_password;
   extern const char *Txt_Set_password;
   bool IHaveAPasswordInDB = (bool) Gbl.Usrs.Me.UsrDat.Password[0];

   /***** Help message *****/
   if (!IHaveAPasswordInDB) // If I don't have a password in database...
      Lay_ShowAlert (Lay_WARNING,Txt_Before_going_to_any_other_option_you_must_create_your_password);
   else if (Gbl.Usrs.Me.LoginPlainPassword[0])
     {
      if (!Pwd_FastCheckIfPasswordSeemsGood (Gbl.Usrs.Me.LoginPlainPassword))
         Lay_ShowAlert (Lay_WARNING,Txt_Your_password_is_not_secure_enough);
     }

   /***** Start form *****/
   Act_FormStart (ActChgPwd);

   /***** Start frame *****/
   Lay_StartRoundFrameTable (NULL,Txt_Password,NULL,Hlp_PROFILE_Password,2);

   /* Current password */
   if (IHaveAPasswordInDB) // If I have a password in database...
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"RIGHT_MIDDLE\">"
	                 "<label for=\"UsrPwd\" class=\"%s\">%s:</label>"
	                 "</td>"
                         "<td class=\"LEFT_MIDDLE\">"
                         "<input type=\"password\""
                         " id=\"UsrPwd\" name=\"UsrPwd\""
                         " size=\"18\" maxlength=\"%u\""
                         " autocomplete=\"off\" required=\"required\" />"
                         "</td>"
                         "</tr>",
               The_ClassForm[Gbl.Prefs.Theme],
               Txt_Current_password,
               Pwd_MAX_LENGTH_PLAIN_PASSWORD);

   /* Help message */
   fprintf (Gbl.F.Out,"<tr>"
		      "<td colspan=\"2\">");
   sprintf (Gbl.Message,Txt_Your_password_must_be_at_least_X_characters_and_can_not_contain_spaces_,
            Pwd_MIN_LENGTH_PLAIN_PASSWORD);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   /* New password */
   Pwd_PutFormToGetNewPasswordTwice ();

   /***** Send button and end form *****/
   Lay_EndRoundFrameTableWithButton (Lay_CONFIRM_BUTTON,
                                     IHaveAPasswordInDB ? Txt_Change_password :
	                                                  Txt_Set_password);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/**************** Put form to request the new password once ******************/
/*****************************************************************************/

void Pwd_PutFormToGetNewPasswordOnce (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Password;
   extern const char *Txt_HELP_password;

   sprintf (Gbl.Message,Txt_HELP_password,Pwd_MIN_LENGTH_PLAIN_PASSWORD);
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_MIDDLE\">"
	              "<label for=\"Passwd\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"password\" id=\"Passwd\" name=\"Paswd\""
                      " size=\"18\" maxlength=\"%u\" placeholder=\"%s\""
                      " required=\"required\" />"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Password,
            Pwd_MAX_LENGTH_PLAIN_PASSWORD,
            Gbl.Message);
  }

/*****************************************************************************/
/**************** Put form to request the new password twice *****************/
/*****************************************************************************/

void Pwd_PutFormToGetNewPasswordTwice (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_New_password;
   extern const char *Txt_HELP_password;
   extern const char *Txt_Retype_new_password;

   sprintf (Gbl.Message,Txt_HELP_password,Pwd_MIN_LENGTH_PLAIN_PASSWORD);
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_MIDDLE\">"
	              "<label for=\"Paswd1\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"password\" id=\"Paswd1\" name=\"Paswd1\""
                      " size=\"18\" maxlength=\"%u\""
                      " placeholder=\"%s\" required=\"required\" />"
                      "</td>"
                      "</tr>"
                      "<tr>"
                      "<td class=\"RIGHT_MIDDLE\">"
	              "<label for=\"Paswd2\" class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"password\" id=\"Paswd2\" name=\"Paswd2\""
                      " size=\"18\" maxlength=\"%u\""
                      " placeholder=\"%s\" required=\"required\" />"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_New_password,
            Pwd_MAX_LENGTH_PLAIN_PASSWORD,
            Gbl.Message,
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Retype_new_password,
            Pwd_MAX_LENGTH_PLAIN_PASSWORD,
            Gbl.Message);
  }

/*****************************************************************************/
/********** Show form to the change of password of another user **************/
/*****************************************************************************/

void Pwd_ShowFormOthPwd (void)
  {
   extern const char *Txt_Password;
   extern const char *Txt_Change_password;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get user whose password must be changed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Usr_AsAdminICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	{
	 /***** Start frame *****/
         Lay_StartRoundFrame (NULL,Txt_Password,NULL,NULL);

	 /***** Show user's record *****/
	 Rec_ShowSharedUsrRecord (Rec_SHA_RECORD_LIST,&Gbl.Usrs.Other.UsrDat);

	 /***** Form to change password *****/
	 /* Start form */
	 Act_FormStart ( Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB == Rol_STUDENT ? ActChgPwdStd :
	                (Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB == Rol_TEACHER ? ActChgPwdTch :
	                	                                                   ActChgPwdOth));
	 Usr_PutParamOtherUsrCodEncrypted ();

	 /* New password */
	 fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\" style=\"margin:0 auto;\">");
         Pwd_PutFormToGetNewPasswordTwice ();
	 fprintf (Gbl.F.Out,"</table>");

	 /* End form */
	 Lay_PutConfirmButton (Txt_Change_password);
	 Act_FormEnd ();

         /***** End frame *****/
         Lay_EndRoundFrame ();
	}
      else
	 Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else		// User not found
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/********* Put a link to the action used to change user's password ***********/
/*****************************************************************************/

void Pwd_PutLinkToChangeMyPassword (void)
  {
   extern const char *Txt_Change_password;

   /***** Link for changing the password *****/
   Lay_PutContextualLink (ActFrmChgMyPwd,NULL,
                          "key64x64.gif",
                          Txt_Change_password,Txt_Change_password,
                          NULL);
  }

/*****************************************************************************/
/********* Put a link to the action used to change user's password ***********/
/*****************************************************************************/

void Pwd_PutLinkToChangeOtherUsrPassword (void)
  {
   extern const char *Txt_Change_password;

   /***** Link for changing the password *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// It's me
      Pwd_PutLinkToChangeMyPassword ();
   else									// Not me
      Lay_PutContextualLink ( Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB == Rol_STUDENT ? ActFrmPwdStd :
	                     (Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB == Rol_TEACHER ? ActFrmPwdTch :
	                	                                                        ActFrmPwdOth),
                             Usr_PutParamOtherUsrCodEncrypted,
                             "key64x64.gif",
                             Txt_Change_password,Txt_Change_password,
                             NULL);
  }

/*****************************************************************************/
/***************** Ask for confirmation on dangerous actions *****************/
/*****************************************************************************/

void Pwd_AskForConfirmationOnDangerousAction (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_I_understand_that_this_action_can_not_be_undone;
   extern const char *Txt_For_security_enter_your_password;

   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\" style=\"margin:12px;\">"
                      "<label class=\"%s\">"
		      "<input type=\"checkbox\" name=\"Consent\" value=\"Y\" />"
		      "%s"
                      "</label>"
		      "<br />"
                      "<label class=\"%s\">"
		      "%s:&nbsp;"
		      "<input type=\"password\" name=\"OthUsrPwd\""
		      " size=\"16\" maxlength=\"%u\""
		      " autocomplete=\"off\" required=\"required\" />"
		      "</label>"
		      "</div>",
            The_ClassForm[Gbl.Prefs.Theme],
	    Txt_I_understand_that_this_action_can_not_be_undone,
            The_ClassForm[Gbl.Prefs.Theme],
	    Txt_For_security_enter_your_password,
	    Pwd_MAX_LENGTH_PLAIN_PASSWORD);
  }

/*****************************************************************************/
/******************** Get confirmation on dangerous actions ******************/
/*****************************************************************************/
// Returns true if consent have been checked and my password is correct

bool Pwd_GetConfirmationOnDangerousAction (void)
  {
   extern const char *Txt_You_have_not_confirmed_the_action;
   extern const char *Txt_You_have_not_entered_your_password_correctly;
   char YN[1+1];
   char PlainPassword[Pwd_MAX_LENGTH_PLAIN_PASSWORD+1];
   char EncryptedPassword[Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64+1];

   /***** Get if consent has been done *****/
   Par_GetParToText ("Consent",YN,1);
   if (Str_ConvertToUpperLetter (YN[0]) != 'Y')
     {
      Lay_ShowAlert (Lay_WARNING,Txt_You_have_not_confirmed_the_action);
      return false;
     }

   /***** Get my password *****/
   /* Get plain password from form */
   Par_GetParToText ("OthUsrPwd",PlainPassword,Pwd_MAX_LENGTH_PLAIN_PASSWORD);

   /* Encrypt password */
   Cry_EncryptSHA512Base64 (PlainPassword,EncryptedPassword);

   /* Compare passwords */
   if (strcmp (Gbl.Usrs.Me.LoginEncryptedPassword,EncryptedPassword))
     {
      Lay_ShowAlert (Lay_WARNING,Txt_You_have_not_entered_your_password_correctly);
      return false;
     }

   return true;
  }
