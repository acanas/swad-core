// swad_password.c: Users' passwords

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_enrolment.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_ID_database.h"
#include "swad_mail_database.h"
#include "swad_password.h"
#include "swad_password_database.h"
#include "swad_parameter.h"
#include "swad_session_database.h"
#include "swad_user.h"
#include "swad_user_database.h"

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
/***************************** Private variables *****************************/
/*****************************************************************************/

const char *Pwd_PASSWORD_SECTION_ID = "password_section";

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Pwd_CheckAndUpdateNewPwd (struct Usr_Data *UsrDat);

static void Pwd_PutLinkToSendNewPasswdPars (void *UsrIdLogin);

static void Pwd_CreateANewPassword (char PlainPassword[Pwd_MAX_BYTES_PLAIN_PASSWORD + 1]);

static Exi_Exist_t Pwd_CheckIfPasswdExistsAsUsrIDorName (const char *PlainPassword);

/*****************************************************************************/
/************* Get parameter with my plain password from a form **************/
/*****************************************************************************/

void Pwd_GetParUsrPwdLogin (void)
  {
   /***** Get plain password from form *****/
   Par_GetParText ("UsrPwd",Gbl.Usrs.Me.LoginPlainPassword,
                     Pwd_MAX_BYTES_PLAIN_PASSWORD);

   /***** Encrypt password *****/
   Cry_EncryptSHA512Base64 (Gbl.Usrs.Me.LoginPlainPassword,Gbl.Usrs.Me.LoginEncryptedPassword);
  }

/*****************************************************************************/
/**** Check if login password is the same as current password in database ****/
/*****************************************************************************/
// Returns Err_SUCCESS if there's no current password in database, or if login password is the same
// Returns Err_ERROR if there's a current password in database and is not the same as the login password

Err_SuccessOrError_t Pwd_CheckCurrentPassword (void)
  {
   /***** If there's no current password in database *****/
   if (!Gbl.Usrs.Me.UsrDat.Password[0])
      return Err_SUCCESS;

   /***** Check is passwords are the same *****/
   return strcmp (Gbl.Usrs.Me.LoginEncryptedPassword,
		  Gbl.Usrs.Me.UsrDat.Password) ? Err_ERROR :
						 Err_SUCCESS;
  }

/*****************************************************************************/
/**** Check if login password is the same as current password in database ****/
/*****************************************************************************/
// Returns true if there's pending password in database and login password is the same
// Returns false if there's no a pending password in database, or if pending password is not the same as the login password

bool Pwd_CheckPendingPassword (void)
  {
   /***** Get pending password from database *****/
   Pwd_DB_GetPendingPassword ();

   return (Gbl.Usrs.Me.PendingPassword[0] ?
           !strcmp (Gbl.Usrs.Me.LoginEncryptedPassword,Gbl.Usrs.Me.PendingPassword) :
           false);
  }

/*****************************************************************************/
/************ Assign my pending password to my current password **************/
/*****************************************************************************/

void Pwd_AssignMyPendingPasswordToMyCurrentPassword (void)
  {
   /***** Update my current password in database *****/
   Pwd_DB_AssignMyPendingPasswordToMyCurrentPassword ();

   /***** Update my current password *****/
   Str_Copy (Gbl.Usrs.Me.UsrDat.Password,Gbl.Usrs.Me.PendingPassword,
             sizeof (Gbl.Usrs.Me.UsrDat.Password) - 1);

   /***** Remove my pending password from database
          since it is not longer necessary *****/
   Pwd_DB_RemoveMyPendingPassword ();
  }

/*****************************************************************************/
/*********************** Update my password in database **********************/
/*****************************************************************************/

void Pwd_UpdateMyPwd (void)
  {
   extern const char *Txt_You_have_not_entered_your_password_correctly;
   char PlainPassword[Pwd_MAX_BYTES_PLAIN_PASSWORD + 1];

   /***** Get plain password from form *****/
   Par_GetParText ("UsrPwd",PlainPassword,Pwd_MAX_BYTES_PLAIN_PASSWORD);

   /***** Encrypt password *****/
   Cry_EncryptSHA512Base64 (PlainPassword,Gbl.Usrs.Me.LoginEncryptedPassword);

   /***** Check current password *****/
   switch (Pwd_CheckCurrentPassword ())
     {
      case Err_SUCCESS:
	 /***** Check and update new password *****/
	 Pwd_CheckAndUpdateNewPwd (&Gbl.Usrs.Me.UsrDat);
	 break;
      case Err_ERROR:
      default:
	 Ale_CreateAlert (Ale_WARNING,Pwd_PASSWORD_SECTION_ID,
			  Txt_You_have_not_entered_your_password_correctly);
	 break;
     }
  }

/*****************************************************************************/
/********************* Update another user's password ************************/
/*****************************************************************************/

void Pwd_UpdateOtherUsrPwd (void)
  {
   /***** Get other user's code from form and get user's data *****/
   switch (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
     {
      case Exi_EXISTS:
	 switch (Usr_CheckIfICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	   {
	    case Usr_CAN:
	       /***** Check and update password *****/
	       Pwd_CheckAndUpdateNewPwd (&Gbl.Usrs.Other.UsrDat);
	       break;
	    case Usr_CAN_NOT:
	    default:
	       Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	       break;
	   }
	 break;
      case Exi_DOES_NOT_EXIST:	// User not found
      default:
	 Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	 break;
     }
  }

/*****************************************************************************/
/********************* Check and update new password *************************/
/*****************************************************************************/

static void Pwd_CheckAndUpdateNewPwd (struct Usr_Data *UsrDat)
  {
   extern const char *Txt_You_have_not_written_twice_the_same_new_password;
   extern const char *Txt_The_password_has_been_changed_successfully;
   char NewPlainPassword[2][Pwd_MAX_BYTES_PLAIN_PASSWORD + 1];
   char NewEncryptedPassword[Pwd_BYTES_ENCRYPTED_PASSWORD + 1];

   Par_GetParText ("Paswd1",NewPlainPassword[0],Pwd_MAX_BYTES_PLAIN_PASSWORD);
   Par_GetParText ("Paswd2",NewPlainPassword[1],Pwd_MAX_BYTES_PLAIN_PASSWORD);

   /***** Check if I have written twice the same password *****/
   if (strcmp (NewPlainPassword[0],
               NewPlainPassword[1]))
      // Passwords don't match
      Ale_CreateAlert (Ale_WARNING,Pwd_PASSWORD_SECTION_ID,
	               Txt_You_have_not_written_twice_the_same_new_password);
   else
     {
      Cry_EncryptSHA512Base64 (NewPlainPassword[0],NewEncryptedPassword);
      if (Pwd_SlowCheckIfPasswordIsGood (NewPlainPassword[0],
					 NewEncryptedPassword,
					 UsrDat->UsrCod))        // New password is good?
	{
         /* Update user's data */
	 Str_Copy (UsrDat->Password,NewEncryptedPassword,
		   sizeof (UsrDat->Password) - 1);
	 Ses_DB_UpdateSession ();
	 Enr_UpdateUsrData (UsrDat);

	 Ale_CreateAlert (Ale_SUCCESS,Pwd_PASSWORD_SECTION_ID,
	                  Txt_The_password_has_been_changed_successfully);
	}
     }
  }

/*****************************************************************************/
/*************** Show form to send a new password by email *******************/
/*****************************************************************************/

void Pwd_PutLinkToSendNewPasswd (void)
  {
   extern const char *Txt_Forgotten_password;

   Lay_PutContextualLinkIconText (ActReqSndNewPwd,NULL,
				  Pwd_PutLinkToSendNewPasswdPars,Gbl.Usrs.Me.UsrIdLogin,
				  "key.svg",Ico_BLACK,
				  Txt_Forgotten_password,NULL);
  }

static void Pwd_PutLinkToSendNewPasswdPars (void *UsrIdLogin)
  {
   if (UsrIdLogin)
      Par_PutParString (NULL,"UsrId",(char *) Gbl.Usrs.Me.UsrIdLogin);
  }

/*****************************************************************************/
/*************** Show form to send a new password by email *******************/
/*****************************************************************************/

void Pwd_ShowFormSendNewPwd (void)
  {
   extern const char *Hlp_PROFILE_Password;
   extern const char *Txt_If_you_have_forgotten_your_password_;
   extern const char *Txt_Forgotten_password;
   extern const char *Txt_nick_email_or_ID;

   /***** Begin form *****/
   Frm_BeginForm (ActSndNewPwd);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Forgotten_password,NULL,NULL,
		    Hlp_PROFILE_Password,Box_NOT_CLOSABLE);

	 /***** Help text *****/
	 Ale_ShowAlert (Ale_INFO,Txt_If_you_have_forgotten_your_password_);

	 /***** User's ID/nickname *****/
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_Txt (Txt_nick_email_or_ID); HTM_Colon (); HTM_NBSP ();
	    HTM_INPUT_TEXT ("UsrId",Cns_MAX_CHARS_EMAIL_ADDRESS,Gbl.Usrs.Me.UsrIdLogin,
			    HTM_REQUIRED,
			    "size=\"8\" class=\"INPUT_%s\"",The_GetSuffix ());
	 HTM_LABEL_End ();

      /***** Send button and end box *****/
      Box_BoxWithButtonEnd (Btn_CONTINUE);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/*********************** Send a new password by email ************************/
/*****************************************************************************/

void Pwd_ChkIdLoginAndSendNewPwd (void)
  {
   extern const char *Txt_You_must_enter_your_nick_email_or_ID;
   extern const char *Txt_There_was_a_problem_sending_an_email_automatically;
   extern const char *Txt_If_you_have_written_your_ID_nickname_or_email_correctly_;
   struct Usr_ListUsrCods ListUsrCods;
   unsigned NumUsr;
   char NewRandomPlainPassword[Pwd_MAX_BYTES_PLAIN_PASSWORD + 1];
   int ReturnCode;
   char ErrorTxt[256];

   /***** Check if user's ID or nickname is not empty *****/
   if (!Gbl.Usrs.Me.UsrIdLogin[0])
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_enter_your_nick_email_or_ID);
      Pwd_ShowFormSendNewPwd ();
      return;
     }

   /***** Reset default list of users' codes *****/
   ListUsrCods.NumUsrs = 0;
   ListUsrCods.Lst = NULL;

   /***** Check if user exists *****/
   /* Check if user has typed his user's ID or his nickname */
   switch (Nck_CheckIfNickWithArrIsValid (Gbl.Usrs.Me.UsrIdLogin))
     {
      case Err_SUCCESS:	// 1: It's a nickname
	 if ((Gbl.Usrs.Me.UsrDat.UsrCod = Nck_GetUsrCodFromNickname (Gbl.Usrs.Me.UsrIdLogin)) > 0)
	   {
	    /* Get user's data */
	    ListUsrCods.NumUsrs = 1;
	    Usr_AllocateListUsrCods (&ListUsrCods);
	    ListUsrCods.Lst[0] = Gbl.Usrs.Me.UsrDat.UsrCod;
	   }
	 break;
      case Err_ERROR:
      default:
	 switch (Mai_CheckIfEmailIsValid (Gbl.Usrs.Me.UsrIdLogin))
	   {
	    case Err_SUCCESS:	// 2: It's an email
	       if ((Gbl.Usrs.Me.UsrDat.UsrCod = Mai_DB_GetUsrCodFromEmail (Gbl.Usrs.Me.UsrIdLogin)) > 0)
		 {
		  /* Get user's data */
		  ListUsrCods.NumUsrs = 1;
		  Usr_AllocateListUsrCods (&ListUsrCods);
		  ListUsrCods.Lst[0] = Gbl.Usrs.Me.UsrDat.UsrCod;
		 }
	       break;
	    case Err_ERROR:	// 3: It's not a nickname nor email
	    default:
	       // Users' IDs are always stored internally in capitals and without leading zeros
	       Str_RemoveLeadingZeros (Gbl.Usrs.Me.UsrIdLogin);
	       if (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Me.UsrIdLogin) == Err_SUCCESS)
		 {
		  /***** Allocate space for the list *****/
		  ID_ReallocateListIDs (&Gbl.Usrs.Me.UsrDat,1);

		  // User has typed a user's ID
		  Str_Copy (Gbl.Usrs.Me.UsrDat.IDs.List[0].ID,Gbl.Usrs.Me.UsrIdLogin,
			    sizeof (Gbl.Usrs.Me.UsrDat.IDs.List[0].ID) - 1);
		  Str_ConvertToUpperText (Gbl.Usrs.Me.UsrDat.IDs.List[0].ID);

		  /* Get users' codes for this ID */
		  if (!ID_GetListUsrCodsFromUsrID (&Gbl.Usrs.Me.UsrDat,NULL,&ListUsrCods,true))	// Only confirmed IDs
		     // If no users found with confirmed IDs, try to get all users (confirmed or not)
		     ID_GetListUsrCodsFromUsrID (&Gbl.Usrs.Me.UsrDat,NULL,&ListUsrCods,false);	// All users (confirmed or not)
		 }
	       break;
	   }
	 break;
     }

   /***** Send a new password via email when user exists *****/
   for (NumUsr = 0;
	NumUsr < ListUsrCods.NumUsrs;
	NumUsr++)
     {
      Gbl.Usrs.Me.UsrDat.UsrCod = ListUsrCods.Lst[NumUsr];
      Usr_GetUsrDataFromUsrCod (&Gbl.Usrs.Me.UsrDat,	// Get my data
                                Usr_DONT_GET_PREFS,
                                Usr_DONT_GET_ROLE_IN_CRS);

      if (Gbl.Usrs.Me.UsrDat.Email[0])
	 switch ((ReturnCode = Pwd_SendNewPasswordByEmail (NewRandomPlainPassword)))
	   {
	    case 0: // Message sent successfully
	       Pwd_SetMyPendingPassword (NewRandomPlainPassword);
	       break;
	    case 1:
	       Err_ShowErrorAndExit (Txt_There_was_a_problem_sending_an_email_automatically);
	       break;
	    default:
	       snprintf (ErrorTxt,sizeof (ErrorTxt),
			 "Internal error: an email message has not been sent successfully."
			 " Error code returned by the script: %d",
			 ReturnCode);
	       Err_ShowErrorAndExit (ErrorTxt);
	       break;
	   }
     }

   /***** Free list of users' codes *****/
   Usr_FreeListUsrCods (&ListUsrCods);

   /***** Help message *****/
   Ale_ShowAlert (Ale_INFO,Txt_If_you_have_written_your_ID_nickname_or_email_correctly_);

   /**** Show forms to login / create account again *****/
   Usr_WriteLandingPage ();
  }

/*****************************************************************************/
/*********************** Send a new password by email ************************/
/*****************************************************************************/
// Gbl.Usrs.Me.UsrDat must be filled
// Return code returned by command

int Pwd_SendNewPasswordByEmail (char NewRandomPlainPassword[Pwd_MAX_BYTES_PLAIN_PASSWORD + 1])
  {
   extern const char *Txt_The_following_password_has_been_assigned_to_you_to_log_in_X_NO_HTML;
   extern const char *Txt_New_password_NO_HTML[1 + Lan_NUM_LANGUAGES];
   char FileNameMail[PATH_MAX + 1];
   FILE *FileMail;
   Lan_Language_t ToUsrLanguage;
   int ReturnCode;

   /***** Create temporary file for mail content *****/
   Mai_CreateFileNameMail (FileNameMail,&FileMail);

   /***** Create a new random password *****/
   Pwd_CreateANewPassword (NewRandomPlainPassword);

   /***** If I have no language, set language to current language *****/
   ToUsrLanguage = Gbl.Usrs.Me.UsrDat.Prefs.Language;
   if (ToUsrLanguage == Lan_LANGUAGE_UNKNOWN)
      ToUsrLanguage = Gbl.Prefs.Language;

   /***** Write mail content into file and close file *****/
   /* Welcome note */
   Mai_WriteWelcomeNoteEMail (FileMail,&Gbl.Usrs.Me.UsrDat,ToUsrLanguage);

   /* Message body */
   fprintf (FileMail,Txt_The_following_password_has_been_assigned_to_you_to_log_in_X_NO_HTML,
	    Cfg_PLATFORM_SHORT_NAME,NewRandomPlainPassword,Cfg_URL_SWAD_CGI,
	    (unsigned) (Cfg_TIME_TO_DELETE_OLD_PENDING_PASSWORDS / (24L * 60L * 60L)),
	    Gbl.Usrs.Me.UsrDat.Email);

   /* Footer note */
   Mai_WriteFootNoteEMail (FileMail,ToUsrLanguage);

   fclose (FileMail);

   /***** Call the script to send an email *****/
   ReturnCode = Mai_SendMailMsg (FileNameMail,
                                 Txt_New_password_NO_HTML[ToUsrLanguage],
                                 Gbl.Usrs.Me.UsrDat.Email);

   /***** Remove temporary file *****/
   unlink (FileNameMail);

   /***** Write message depending on return code *****/
   return ReturnCode;
  }

/*****************************************************************************/
/*********************** Create a new random password ************************/
/*****************************************************************************/

static void Pwd_CreateANewPassword (char PlainPassword[Pwd_MAX_BYTES_PLAIN_PASSWORD + 1])
  {
   Str_CreateRandomAlphanumStr (PlainPassword,Pwd_MIN_CHARS_PLAIN_PASSWORD);
  }

/*****************************************************************************/
/*************************** Set my pending password *************************/
/*****************************************************************************/

void Pwd_SetMyPendingPassword (char PlainPassword[Pwd_MAX_BYTES_PLAIN_PASSWORD + 1])
  {
   /***** Encrypt my pending password *****/
   Cry_EncryptSHA512Base64 (PlainPassword,Gbl.Usrs.Me.PendingPassword);

   /***** Remove expired pending passwords from database *****/
   Pwd_DB_RemoveExpiredPendingPassword ();

   /***** Update my current pending password in database *****/
   Pwd_DB_UpdateMyPendingPassword ();
  }

/*****************************************************************************/
/************************ Check if a password is good ************************/
/*****************************************************************************/

bool Pwd_SlowCheckIfPasswordIsGood (const char PlainPassword[Pwd_MAX_BYTES_PLAIN_PASSWORD + 1],
                                    const char EncryptedPassword[Pwd_BYTES_ENCRYPTED_PASSWORD + 1],
                                    long UsrCod)
  {
   extern const char *Txt_The_password_is_too_trivial_;

   /***** Check if password seems good by making fast checks *****/
   if (!Pwd_FastCheckIfPasswordSeemsGood (PlainPassword))
      return false;

   /***** Check if password is found in user's ID, first name or surnames of anybody *****/
   if (Pwd_CheckIfPasswdExistsAsUsrIDorName (PlainPassword) == Exi_EXISTS)        // PlainPassword is a user's ID, name or surname
     {
      Ale_CreateAlert (Ale_WARNING,Pwd_PASSWORD_SECTION_ID,
	               Txt_The_password_is_too_trivial_);
      return false;
     }

   /***** Check if password is used by too many other users *****/
   if (Pwd_DB_GetNumOtherUsrsWhoUseThisPassword (EncryptedPassword,UsrCod) >
       Pwd_MAX_OTHER_USERS_USING_THE_SAME_PASSWORD)
     {
      Ale_CreateAlert (Ale_WARNING,Pwd_PASSWORD_SECTION_ID,
		       Txt_The_password_is_too_trivial_);
      return false;
     }

   return true;
  }

/*****************************************************************************/
/***** Check if a password is a user's ID, a first name or a surname *********/
/*****************************************************************************/

static Exi_Exist_t Pwd_CheckIfPasswdExistsAsUsrIDorName (const char *PlainPassword)
  {
   /***** Check if password is found in user's ID *****/
   if (ID_DB_FindStrInUsrsIDs (PlainPassword) == Exi_EXISTS)
      return Exi_EXISTS;	// Found

   /***** Check if password is found in first name or surnames of anybody *****/
   return Usr_DB_FindStrInUsrsNames (PlainPassword);
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
   if (LengthPassword < Pwd_MIN_BYTES_PLAIN_PASSWORD)	// PlainPassword too short
     {
      Ale_CreateAlert (Ale_WARNING,Pwd_PASSWORD_SECTION_ID,
		       Txt_The_password_must_be_at_least_X_characters,
		       Pwd_MIN_CHARS_PLAIN_PASSWORD);
      return false;
     }

   /***** Check spaces in password *****/
   if (strchr (PlainPassword,(int) ' ') != NULL)        // PlainPassword with spaces
     {
      Ale_CreateAlert (Ale_WARNING,Pwd_PASSWORD_SECTION_ID,
	               Txt_The_password_can_not_contain_spaces);
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
      Ale_CreateAlert (Ale_WARNING,Pwd_PASSWORD_SECTION_ID,
		       Txt_The_password_can_not_consist_only_of_digits);
      return false;
     }

   return true;
  }

/*****************************************************************************/
/********************** Show form for changing my password *******************/
/*****************************************************************************/

void Pwd_ShowFormChgMyPwd (void)
  {
   extern const char *Hlp_PROFILE_Password;
   extern const char *Txt_Before_going_to_any_other_option_you_must_create_your_password;
   extern const char *Txt_Your_password_is_not_secure_enough;
   extern const char *Txt_Your_password_must_be_at_least_X_characters_and_can_not_contain_spaces_;
   extern const char *Txt_Password;
   extern const char *Txt_Current_password;
   Exi_Exist_t MyPasswordInDBExists = Gbl.Usrs.Me.UsrDat.Password[0] ? Exi_EXISTS :
								       Exi_DOES_NOT_EXIST;

   /***** Begin section *****/
   HTM_SECTION_Begin (Pwd_PASSWORD_SECTION_ID);

      /***** Begin form *****/
      Frm_BeginFormAnchor (ActChgMyPwd,Pwd_PASSWORD_SECTION_ID);

	 /***** Begin box *****/
	 Box_BoxBegin (Txt_Password,NULL,NULL,
		       Hlp_PROFILE_Password,Box_NOT_CLOSABLE);

	    /***** Show possible alerts *****/
	    Ale_ShowAlerts (Pwd_PASSWORD_SECTION_ID);

	    /***** Help message *****/
	    switch (MyPasswordInDBExists)
	      {
	       case Exi_EXISTS:
		  if (Gbl.Usrs.Me.LoginPlainPassword[0])
		     if (!Pwd_FastCheckIfPasswordSeemsGood (Gbl.Usrs.Me.LoginPlainPassword))
			Ale_ShowAlert (Ale_WARNING,Txt_Your_password_is_not_secure_enough);
		  break;
	       case Exi_DOES_NOT_EXIST:	// If I don't have a password in database...
	       default:
		  Ale_ShowAlert (Ale_WARNING,Txt_Before_going_to_any_other_option_you_must_create_your_password);
		  break;
	      }

	    /***** Begin table *****/
	    HTM_TABLE_BeginWidePadding (2);

	    /***** Current password *****/
	    if (MyPasswordInDBExists == Exi_EXISTS) // If I have a password in database...
	      {
	       HTM_TR_Begin (NULL);

		  /* Label */
		  Frm_LabelColumn ("Frm_C1 RM","UsrPwd",Txt_Current_password);

		  /* Data */
		  HTM_TD_Begin ("class=\"Frm_C2 LM\"");
		     HTM_INPUT_PASSWORD ("UsrPwd",NULL,"off",
					 HTM_REQUIRED,
					 "id=\"UsrPwd\""
					 " class=\"Frm_C2_INPUT INPUT_%s\"",
					 The_GetSuffix ());
		  HTM_TD_End ();

	       HTM_TR_End ();
	      }

	    /***** Help message *****/
	    HTM_TR_Begin (NULL);
	       HTM_TD_Begin ("colspan=\"2\"");
		  Ale_ShowAlert (Ale_INFO,Txt_Your_password_must_be_at_least_X_characters_and_can_not_contain_spaces_,
				 Pwd_MIN_CHARS_PLAIN_PASSWORD);
	       HTM_TD_End ();
	    HTM_TR_End ();

	    /***** New password *****/
	    Pwd_PutFormToGetNewPasswordTwice ();

	 /***** End table, send button and end box *****/
	 Box_BoxTableWithButtonEnd (MyPasswordInDBExists == Exi_EXISTS ? Btn_SAVE_CHANGES :
									 Btn_CREATE);

      /***** End form *****/
      Frm_EndForm ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/**************** Put form to request the new password once ******************/
/*****************************************************************************/

void Pwd_PutFormToGetNewPasswordOnce (void)
  {
   extern const char *Txt_Password;
   extern const char *Txt_HELP_password;

   /***** Password *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RM","Paswd",Txt_Password);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LM\"");
	 HTM_INPUT_PASSWORD ("Paswd",Txt_HELP_password,NULL,
			     HTM_REQUIRED,
			     "id=\"Paswd\" class=\"REC_C2_BOT_INPUT INPUT_%s\"",
			     The_GetSuffix ());

      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/**************** Put form to request the new password twice *****************/
/*****************************************************************************/

void Pwd_PutFormToGetNewPasswordTwice (void)
  {
   extern const char *Txt_New_password;
   extern const char *Txt_Retype_new_password;
   extern const char *Txt_HELP_password;

   /***** 1st password *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RM","Paswd1",Txt_New_password);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LM\"");
	 HTM_INPUT_PASSWORD ("Paswd1",Txt_HELP_password,NULL,
			     HTM_REQUIRED,
			     "id=\"Paswd1\" class=\"Frm_C2_INPUT INPUT_%s\"",
			     The_GetSuffix ());
      HTM_TD_End ();

   HTM_TR_End ();

   /***** 2nd password *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RM","Paswd2",Txt_Retype_new_password);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LM\"");
	 HTM_INPUT_PASSWORD ("Paswd2",Txt_HELP_password,NULL,
			     HTM_REQUIRED,
			     "id=\"Paswd2\" class=\"Frm_C2_INPUT INPUT_%s\"",
			     The_GetSuffix ());
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/********** Show form to the change of password of another user **************/
/*****************************************************************************/

void Pwd_ShowFormChgOtherUsrPwd (void)
  {
   extern const char *Txt_Password;
   static Act_Action_t NextAction[Rol_NUM_ROLES] =
     {
      [Rol_UNK	  ] = ActChgPwdOth,
      [Rol_GST	  ] = ActChgPwdOth,
      [Rol_USR	  ] = ActChgPwdOth,
      [Rol_STD	  ] = ActChgPwdStd,
      [Rol_NET	  ] = ActChgPwdTch,
      [Rol_TCH	  ] = ActChgPwdTch,
      [Rol_DEG_ADM] = ActChgPwdOth,
      [Rol_CTR_ADM] = ActChgPwdOth,
      [Rol_INS_ADM] = ActChgPwdOth,
      [Rol_SYS_ADM] = ActChgPwdOth,
     };

   /***** Begin section *****/
   HTM_SECTION_Begin (Pwd_PASSWORD_SECTION_ID);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Password,NULL,NULL,NULL,Box_NOT_CLOSABLE);

	 /***** Show possible alerts *****/
	 Ale_ShowAlerts (Pwd_PASSWORD_SECTION_ID);

	 /***** Form to change password *****/
	 /* Begin form */
	 Frm_BeginFormAnchor (NextAction[Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs],Pwd_PASSWORD_SECTION_ID);
	    Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);

	    /* New password */
	    HTM_TABLE_BeginCenterPadding (2);
	       Pwd_PutFormToGetNewPasswordTwice ();
	    HTM_TABLE_End ();

	    Btn_PutButton (Btn_CHANGE,NULL);

	 /* End form */
	 Frm_EndForm ();

      /***** End box *****/
      Box_BoxEnd ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/***************** Ask for confirmation on dangerous actions *****************/
/*****************************************************************************/

void Pwd_AskForConfirmationOnDangerousAction (void)
  {
   extern const char *Txt_I_understand_that_this_action_can_not_be_undone;
   extern const char *Txt_For_security_enter_your_password;

   HTM_DIV_Begin ("class=\"CM\" style=\"margin:12px;\"");

      /***** Checkbox *****/
      HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	 HTM_INPUT_CHECKBOX ("Consent",
			     HTM_NO_ATTR,
			     "value=\"Y\"");
	 HTM_Txt (Txt_I_understand_that_this_action_can_not_be_undone);
      HTM_LABEL_End ();

      HTM_BR ();

      /***** Password *****/
      HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	 HTM_Txt (Txt_For_security_enter_your_password);
	 HTM_Colon (); HTM_NBSP ();
	 HTM_INPUT_PASSWORD ("OthUsrPwd",NULL,"off",
			     HTM_REQUIRED,
			     "class=\"INPUT_%s\"",The_GetSuffix ());
      HTM_LABEL_End ();

   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************** Get confirmation on dangerous actions ******************/
/*****************************************************************************/
// Returns true if consent have been checked and my password is correct

bool Pwd_GetConfirmationOnDangerousAction (void)
  {
   extern const char *Txt_You_have_not_confirmed_the_action;
   extern const char *Txt_You_have_not_entered_your_password_correctly;
   char PlainPassword[Pwd_MAX_BYTES_PLAIN_PASSWORD + 1];
   char EncryptedPassword[Pwd_BYTES_ENCRYPTED_PASSWORD + 1];

   /***** Get if consent has been done *****/
   if (!Par_GetParBool ("Consent"))
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_have_not_confirmed_the_action);
      return false;
     }

   /***** Get my password *****/
   /* Get plain password from form */
   Par_GetParText ("OthUsrPwd",PlainPassword,Pwd_MAX_BYTES_PLAIN_PASSWORD);

   /* Encrypt password */
   Cry_EncryptSHA512Base64 (PlainPassword,EncryptedPassword);

   /* Compare passwords */
   if (strcmp (Gbl.Usrs.Me.LoginEncryptedPassword,EncryptedPassword))
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_have_not_entered_your_password_correctly);
      return false;
     }

   return true;
  }
