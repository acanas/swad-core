// swad_duplicate.c: duplicate users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_account.h"
#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_duplicate.h"
#include "swad_duplicate_database.h"
#include "swad_enrolment.h"
#include "swad_enrolment_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_layout.h"
#include "swad_parameter.h"
#include "swad_profile.h"
#include "swad_role_type.h"
#include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Dup_ListSimilarUsrs (void);

static void Dup_PutButtonToViewSimilarUsrs (const struct Usr_Data *UsrDat);
static void Dup_PutButtonToEliminateUsrAccount (const struct Usr_Data *UsrDat);
static void Dup_PutButtonToRemoveFromListOfDupUsrs (const struct Usr_Data *UsrDat);

/*****************************************************************************/
/******************** Report a user as possible duplicate ********************/
/*****************************************************************************/

void Dup_ReportUsrAsPossibleDuplicate (void)
  {
   extern const char *Txt_Thank_you_for_reporting_a_possible_duplicate_user;

   /***** Get user to be reported as possible duplicate *****/
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
     {
      /* Check if it's allowed to me to report users as possible duplicatedr */
      if (Gbl.Usrs.Me.Role.Logged >= Rol_TCH &&
	  Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod) == Usr_OTHER)
	{
	 /***** Insert possible duplicate into database *****/
         Dup_DB_AddUsrToDuplicated (Gbl.Usrs.Other.UsrDat.UsrCod);

         /***** Show feedback message *****/
         Ale_ShowAlert (Ale_SUCCESS,Txt_Thank_you_for_reporting_a_possible_duplicate_user);
	}
      else
         Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/************ Put a link (form) to list possible duplicate users *************/
/*****************************************************************************/

void Dup_PutLinkToListDupUsrs (void)
  {
   extern const char *Txt_Duplicate_USERS;

   /***** Put form to remove old users *****/
   Lay_PutContextualLinkIconText (ActLstDupUsr,NULL,
                                  NULL,NULL,
				  "user-friends.svg",Ico_BLACK,
				  Txt_Duplicate_USERS,NULL);
  }

/*****************************************************************************/
/*********************** List possible duplicate users ***********************/
/*****************************************************************************/

void Dup_ListDuplicateUsrs (void)
  {
   extern const char *Hlp_USERS_Duplicates_possibly_duplicate_users;
   extern const char *Txt_Possibly_duplicate_users;
   extern const char *Txt_Informants;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct Usr_Data UsrDat;
   unsigned NumInformants;

   /***** Begin box with list of possible duplicate users *****/
   Box_BoxBegin (Txt_Possibly_duplicate_users,NULL,NULL,
                 Hlp_USERS_Duplicates_possibly_duplicate_users,Box_NOT_CLOSABLE);

      /***** List possible duplicated users *****/
      if ((NumUsrs = Dup_DB_GetListDuplicateUsrs (&mysql_res)))
	{
	 /***** Initialize field names *****/
	 Usr_SetUsrDatMainFieldNames ();

	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&UsrDat);

	 /***** Begin table *****/
	 HTM_TABLE_BeginCenterPadding (2);

	    /***** Heading row with column names *****/
	    Gbl.Usrs.Listing.WithPhotos = true;
	    Usr_WriteHeaderFieldsUsrDat (false);	// Columns for the data

	    /***** List users *****/
	    for (NumUsr = 0, The_ResetRowColor ();
		 NumUsr < NumUsrs;
		 NumUsr++)
	      {
	       row = mysql_fetch_row (mysql_res);

	       /* Get user code (row[0]) */
	       UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
	       if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							    Usr_DONT_GET_PREFS,
							    Usr_DONT_GET_ROLE_IN_CRS))
		 {
		  /* Get if user has accepted all his/her courses */
		  if (Enr_DB_GetNumCrssOfUsr (UsrDat.UsrCod) != 0)
		     UsrDat.Accepted = (Enr_DB_GetNumCrssOfUsrNotAccepted (UsrDat.UsrCod) == 0);
		  else
		     UsrDat.Accepted = false;

		  /* Write data of this user */
		  Usr_WriteRowUsrMainData (NumUsrs - NumUsr,&UsrDat,false,Rol_UNK,
					   &Gbl.Usrs.Selected);

		  HTM_TR_Begin (NULL);

		     HTM_TD_Begin ("colspan=\"2\" class=\"%s\"",
		                   The_GetColorRows ());
		     HTM_TD_End ();

		     HTM_TD_Begin ("colspan=\"%u\" class=\"LM DAT_%s %s\"",
				   Usr_NUM_MAIN_FIELDS_DATA_USR - 2,
				   The_GetSuffix (),
				   The_GetColorRows ());

			/* Write number of informants (row[1]) if greater than 1 */
			if (sscanf (row[1],"%u",&NumInformants) != 1)
			   Err_ShowErrorAndExit ("Wrong number of informants.");
			if (NumInformants > 1)
			  {
			   HTM_TxtColonNBSP (Txt_Informants);
			   HTM_Unsigned (NumInformants);
			   HTM_BR ();
			  }

			/* Button to view users similar to this */
			Dup_PutButtonToViewSimilarUsrs (&UsrDat);

			/* Button to remove from list of possible duplicate users */
			Dup_PutButtonToRemoveFromListOfDupUsrs (&UsrDat);

		     HTM_TD_End ();

		  HTM_TR_End ();

		  The_ChangeRowColor ();
		 }
	       else        // User does not exists ==>
			   // remove user from table of possible duplicate users
		  Dup_DB_RemoveUsrFromDuplicated (UsrDat.UsrCod);
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);
	}
      else	// There are no duplicated users
	 /***** Show warning indicating no users found *****/
	 Usr_ShowWarningNoUsersFound (Rol_UNK);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* List similar users to a given one *********************/
/*****************************************************************************/

void Dup_GetUsrCodAndListSimilarUsrs (void)
  {
   /***** Get user to be removed from list of possible duplicates *****/
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
      Dup_ListSimilarUsrs ();
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

static void Dup_ListSimilarUsrs (void)
  {
   extern const char *Hlp_USERS_Duplicates_similar_users;
   extern const char *Txt_Similar_users;
   struct Usr_Data UsrDat;
   MYSQL_RES *mysql_res;
   unsigned NumUsrs;
   unsigned NumUsr;

   /***** Begin box with list of possible duplicate users *****/
   Box_BoxBegin (Txt_Similar_users,NULL,NULL,
                 Hlp_USERS_Duplicates_similar_users,Box_NOT_CLOSABLE);

   /***** List possible similar users *****/
   if ((NumUsrs = Dup_DB_GetUsrsSimilarTo (&mysql_res,&Gbl.Usrs.Other.UsrDat)))
     {
      /***** Initialize field names *****/
      Usr_SetUsrDatMainFieldNames ();

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Begin table *****/
      HTM_TABLE_BeginCenterPadding (5);

	 /***** Heading row with column names *****/
	 Gbl.Usrs.Listing.WithPhotos = true;
	 Usr_WriteHeaderFieldsUsrDat (false);	// Columns for the data

	 /***** List users *****/
	 for (NumUsr = 0, The_ResetRowColor ();
	      NumUsr < NumUsrs;
	      NumUsr++)
	   {
	    /* Get user code */
	    UsrDat.UsrCod = DB_GetNextCode (mysql_res);
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							 Usr_DONT_GET_PREFS,
							 Usr_DONT_GET_ROLE_IN_CRS))
	      {
	       /* Get if user has accepted all his/her courses */
	       if (Enr_DB_GetNumCrssOfUsr (UsrDat.UsrCod) != 0)
		  UsrDat.Accepted = (Enr_DB_GetNumCrssOfUsrNotAccepted (UsrDat.UsrCod) == 0);
	       else
		  UsrDat.Accepted = false;

	       /***** Write data of this user *****/
	       Usr_WriteRowUsrMainData (NumUsrs - NumUsr,&UsrDat,false,Rol_UNK,
					&Gbl.Usrs.Selected);

	       /***** Write user's profile and user's courses *****/
	       HTM_TR_Begin (NULL);

		  HTM_TD_Begin ("colspan=\"2\" class=\"%s\"",
		                The_GetColorRows ());
		  HTM_TD_End ();

		  HTM_TD_Begin ("colspan=\"%u\" class=\"%s\"",
				Usr_NUM_MAIN_FIELDS_DATA_USR-2,
				The_GetColorRows ());
		     /* Show details of user's profile */
		     Prf_ShowDetailsUserProfile (&UsrDat);
		     /* Write all courses this user belongs to */
		     Crs_GetAndWriteCrssOfAUsr (&UsrDat,Rol_TCH);
		     Crs_GetAndWriteCrssOfAUsr (&UsrDat,Rol_NET);
		     Crs_GetAndWriteCrssOfAUsr (&UsrDat,Rol_STD);
		  HTM_TD_End ();

	       HTM_TR_End ();

	       /***** Buttons to remove user / mark as not dubplicated *****/
	       HTM_TR_Begin (NULL);

		  HTM_TD_Begin ("colspan=\"2\" class=\"%s\"",
		                The_GetColorRows ());
		  HTM_TD_End ();

		  HTM_TD_Begin ("colspan=\"%u\" class=\"LT %s\"",
				Usr_NUM_MAIN_FIELDS_DATA_USR-2,
				The_GetColorRows ());
		     /* Button to remove this user */
		     if (Acc_CheckIfICanEliminateAccount (UsrDat.UsrCod))
			Dup_PutButtonToEliminateUsrAccount (&UsrDat);
		     /* Button to remove from list of possible duplicate users */
		     if (Dup_DB_CheckIfUsrIsDup (UsrDat.UsrCod))
			Dup_PutButtonToRemoveFromListOfDupUsrs (&UsrDat);
		  HTM_TD_End ();

	       HTM_TR_End ();

	       The_ChangeRowColor ();
	      }
	   }

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
   else	// There are no similar users
      /***** Show warning indicating no users found *****/
      Usr_ShowWarningNoUsersFound (Rol_UNK);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* Put button to view similar users **********************/
/*****************************************************************************/

static void Dup_PutButtonToViewSimilarUsrs (const struct Usr_Data *UsrDat)
  {
   extern const char *Txt_Similar_users;

   Frm_BeginForm (ActLstSimUsr);
      Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
      Btn_PutConfirmButtonInline (Txt_Similar_users);
   Frm_EndForm ();
  }

/*****************************************************************************/
/********* Put button to request the elimination of a user's account *********/
/*****************************************************************************/

static void Dup_PutButtonToEliminateUsrAccount (const struct Usr_Data *UsrDat)
  {
   extern const char *Txt_Eliminate_user_account;

   Frm_BeginForm (ActUpdOth);
      Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
      Par_PutParUnsigned (NULL,"RegRemAction",(unsigned) Enr_ELIMINATE_ONE_USR_FROM_PLATFORM);
      Btn_PutRemoveButtonInline (Txt_Eliminate_user_account);
   Frm_EndForm ();
  }

/*****************************************************************************/
/****** Put button to remove user from list of possible duplicate users ******/
/*****************************************************************************/

static void Dup_PutButtonToRemoveFromListOfDupUsrs (const struct Usr_Data *UsrDat)
  {
   extern const char *Txt_Not_duplicated;

   Frm_BeginForm (ActRemDupUsr);
      Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
      Btn_PutConfirmButtonInline (Txt_Not_duplicated);
   Frm_EndForm ();
  }

/*****************************************************************************/
/*********** Remove user from list of possible duplicate users ***************/
/*****************************************************************************/

void Dup_RemoveUsrFromListDupUsrs (void)
  {
   /***** Get user to be removed from list of possible duplicates *****/
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
     {
      /* Remove entry from database */
      Dup_DB_RemoveUsrFromDuplicated (Gbl.Usrs.Other.UsrDat.UsrCod);

      /* Show list of possible duplicated users again */
      Dup_ListDuplicateUsrs ();
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }
