// swad_duplicate.c: duplicate users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
#include "swad_box.h"
#include "swad_database.h"
#include "swad_duplicate.h"
#include "swad_enrolment.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_layout.h"
#include "swad_profile.h"
#include "swad_role_type.h"
#include "swad_table.h"
#include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

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

static void Dup_ListSimilarUsrs (void);

static bool Dup_CheckIfUsrIsDup (long UsrCod);

static void Dup_PutButtonToViewSimilarUsrs (const struct UsrData *UsrDat);
static void Dup_PutButtonToEliminateUsrAccount (const struct UsrData *UsrDat);
static void Dup_PutButtonToRemoveFromListOfDupUsrs (const struct UsrData *UsrDat);

/*****************************************************************************/
/******************** Report a user as possible duplicate ********************/
/*****************************************************************************/

void Dup_ReportUsrAsPossibleDuplicate (void)
  {
   extern const char *Txt_Thank_you_for_reporting_a_possible_duplicate_user;
   bool ItsMe;

   /***** Get user to be reported as possible duplicate *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      /* Check if it's allowed to me to report users as possible duplicatedr */
      ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);
      if (!ItsMe && Gbl.Usrs.Me.Role.Logged >= Rol_TCH)
	{
	 /***** Insert possible duplicate into database *****/
         DB_QueryINSERT ("can not report duplicate",
			 "REPLACE INTO usr_duplicated"
			 " (UsrCod,InformerCod,InformTime)"
			 " VALUES"
			 " (%ld,%ld,NOW())",
                         Gbl.Usrs.Other.UsrDat.UsrCod,
                         Gbl.Usrs.Me.UsrDat.UsrCod);

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
   Lay_PutContextualLinkIconText (ActLstDupUsr,NULL,NULL,
				  "user-friends.svg",
				  Txt_Duplicate_USERS);
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
   struct UsrData UsrDat;
   unsigned NumInformants;

   /***** Start box with list of possible duplicate users *****/
   Box_StartBox (NULL,Txt_Possibly_duplicate_users,NULL,
                 Hlp_USERS_Duplicates_possibly_duplicate_users,Box_NOT_CLOSABLE);

   /***** Make query *****/
   NumUsrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get possibly"
						   " duplicate users",
					"SELECT UsrCod,COUNT(*) AS N,UNIX_TIMESTAMP(MIN(InformTime)) AS T"
				        " FROM usr_duplicated"
				        " GROUP BY UsrCod"
				        " ORDER BY N DESC,T DESC");

   /***** List possible duplicated users *****/
   if (NumUsrs)
     {
      /***** Initialize field names *****/
      Usr_SetUsrDatMainFieldNames ();

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Start table *****/
      Tbl_StartTableCenterPadding (2);

      /***** Heading row with column names *****/
      Gbl.Usrs.Listing.WithPhotos = true;
      Usr_WriteHeaderFieldsUsrDat (false);	// Columns for the data

      /***** List users *****/
      for (NumUsr = 0, Gbl.RowEvenOdd = 0;
           NumUsr < NumUsrs;
           NumUsr++)
        {
         row = mysql_fetch_row (mysql_res);

         /* Get user code (row[0]) */
         UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS))
           {
            /* Get if user has accepted all his/her courses */
            if (Usr_GetNumCrssOfUsr (UsrDat.UsrCod) != 0)
               UsrDat.Accepted = (Usr_GetNumCrssOfUsrNotAccepted (UsrDat.UsrCod) == 0);
            else
               UsrDat.Accepted = false;

            /* Write data of this user */
            Usr_WriteRowUsrMainData (NumUsrs - NumUsr,&UsrDat,false,Rol_UNK);

	    Tbl_StartRow ();

            fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"COLOR%u\">",
	             Gbl.RowEvenOdd);
            Tbl_EndCell ();

	    fprintf (Gbl.F.Out,"<td colspan=\"%u\""
			       " class=\"DAT LEFT_MIDDLE COLOR%u\">",
		     Usr_NUM_MAIN_FIELDS_DATA_USR-2,
	             Gbl.RowEvenOdd);

            /* Write number of informants (row[1]) if greater than 1 */
	    if (sscanf (row[1],"%u",&NumInformants) != 1)
	       Lay_ShowErrorAndExit ("Wrong number of informants.");
            if (NumInformants > 1)
	       fprintf (Gbl.F.Out,"%s: %u<br />",
			Txt_Informants,
			NumInformants);

            /* Button to view users similar to this */
	    Dup_PutButtonToViewSimilarUsrs (&UsrDat);

	    /* Button to remove from list of possible duplicate users */
	    Dup_PutButtonToRemoveFromListOfDupUsrs (&UsrDat);

	    Tbl_EndCell ();

	    Tbl_EndRow ();

	    Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
           }
         else        // User does not exists ==>
                     // remove user from table of possible duplicate users
            Dup_RemoveUsrFromDuplicated (UsrDat.UsrCod);
        }

      /***** End table *****/
      Tbl_EndTable ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
   else	// There are no duplicated users
      /***** Show warning indicating no users found *****/
      Usr_ShowWarningNoUsersFound (Rol_UNK);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/********************* List similar users to a given one *********************/
/*****************************************************************************/

void Dup_GetUsrCodAndListSimilarUsrs (void)
  {
   /***** Get user to be removed from list of possible duplicates *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
      Dup_ListSimilarUsrs ();
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

static void Dup_ListSimilarUsrs (void)
  {
   extern const char *Hlp_USERS_Duplicates_similar_users;
   extern const char *Txt_Similar_users;
   struct UsrData UsrDat;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;

   /***** Start box with list of possible duplicate users *****/
   Box_StartBox (NULL,Txt_Similar_users,NULL,
                 Hlp_USERS_Duplicates_similar_users,Box_NOT_CLOSABLE);

   /***** Make query *****/
   if (Gbl.Usrs.Other.UsrDat.Surname1[0] &&
       Gbl.Usrs.Other.UsrDat.FirstName[0])	// Name and surname 1 not empty
      NumUsrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get similar users",
					   "SELECT DISTINCT UsrCod FROM"
					   "(SELECT DISTINCT UsrCod FROM usr_IDs"
					   " WHERE UsrID IN (SELECT UsrID FROM usr_IDs WHERE UsrCod=%ld)"
					   " UNION"
					   " SELECT UsrCod FROM usr_data"
					   " WHERE Surname1='%s' AND Surname2='%s' AND FirstName='%s')"
					   " AS U",
					   Gbl.Usrs.Other.UsrDat.UsrCod,
					   Gbl.Usrs.Other.UsrDat.Surname1,
					   Gbl.Usrs.Other.UsrDat.Surname2,
					   Gbl.Usrs.Other.UsrDat.FirstName);
   else
      NumUsrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get similar users",
					   "SELECT DISTINCT UsrCod FROM usr_IDs"
					   " WHERE UsrID IN (SELECT UsrID FROM usr_IDs WHERE UsrCod=%ld)",
					   Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** List possible similar users *****/
   if (NumUsrs)
     {
      /***** Initialize field names *****/
      Usr_SetUsrDatMainFieldNames ();

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Start table *****/
      Tbl_StartTableCenterPadding (2);

      /***** Heading row with column names *****/
      Gbl.Usrs.Listing.WithPhotos = true;
      Usr_WriteHeaderFieldsUsrDat (false);	// Columns for the data

      /***** List users *****/
      for (NumUsr = 0, Gbl.RowEvenOdd = 0;
           NumUsr < NumUsrs;
           NumUsr++)
        {
         row = mysql_fetch_row (mysql_res);

         /* Get user code (row[0]) */
         UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS))
           {
            /* Get if user has accepted all his/her courses */
            if (Usr_GetNumCrssOfUsr (UsrDat.UsrCod) != 0)
               UsrDat.Accepted = (Usr_GetNumCrssOfUsrNotAccepted (UsrDat.UsrCod) == 0);
            else
               UsrDat.Accepted = false;

            /* Write data of this user */
            Usr_WriteRowUsrMainData (NumUsrs - NumUsr,&UsrDat,false,Rol_UNK);

	    Tbl_StartRow ();

            fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"COLOR%u\">",
		     Gbl.RowEvenOdd);
            Tbl_EndCell ();

	    fprintf (Gbl.F.Out,"<td colspan=\"%u\" class=\"COLOR%u\">",
		     Usr_NUM_MAIN_FIELDS_DATA_USR-2,
		     Gbl.RowEvenOdd);

	    /* Show details of user's profile */
            Prf_ShowDetailsUserProfile (&UsrDat);

	    /* Write all the courses this user belongs to */
	    Crs_GetAndWriteCrssOfAUsr (&UsrDat,Rol_TCH);
	    Crs_GetAndWriteCrssOfAUsr (&UsrDat,Rol_NET);
	    Crs_GetAndWriteCrssOfAUsr (&UsrDat,Rol_STD);

	    Tbl_EndCell ();
	    Tbl_EndRow ();

	    Tbl_StartRow ();

            fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"COLOR%u\">",
		     Gbl.RowEvenOdd);
            Tbl_EndCell ();

	    fprintf (Gbl.F.Out,"<td colspan=\"%u\" class=\"LEFT_TOP COLOR%u\""
			       " style=\"padding-bottom:20px;\">",
		     Usr_NUM_MAIN_FIELDS_DATA_USR-2,
		     Gbl.RowEvenOdd);

	    /* Button to remove this user */
	    if (Acc_CheckIfICanEliminateAccount (UsrDat.UsrCod))
	       Dup_PutButtonToEliminateUsrAccount (&UsrDat);

	    /* Button to remove from list of possible duplicate users */
	    if (Dup_CheckIfUsrIsDup (UsrDat.UsrCod))
	       Dup_PutButtonToRemoveFromListOfDupUsrs (&UsrDat);

	    Tbl_EndCell ();

	    Tbl_EndRow ();

	    Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
           }
        }

      /***** End table *****/
      Tbl_EndTable ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
   else	// There are no similar users
      /***** Show warning indicating no users found *****/
      Usr_ShowWarningNoUsersFound (Rol_UNK);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/********** Check if a user is in list of possible duplicate users ***********/
/*****************************************************************************/

static bool Dup_CheckIfUsrIsDup (long UsrCod)
  {
   return (DB_QueryCOUNT ("can not if user is in list"
			  " of possible duplicate users",
			  "SELECT COUNT(*) FROM usr_duplicated"
			  " WHERE UsrCod=%ld",
			  UsrCod) != 0);
  }

/*****************************************************************************/
/********************* Put button to view similar users **********************/
/*****************************************************************************/

static void Dup_PutButtonToViewSimilarUsrs (const struct UsrData *UsrDat)
  {
   extern const char *Txt_Similar_users;

   Frm_StartForm (ActLstSimUsr);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Btn_PutConfirmButtonInline (Txt_Similar_users);
   Frm_EndForm ();
  }

/*****************************************************************************/
/********* Put button to request the elimination of a user's account *********/
/*****************************************************************************/

static void Dup_PutButtonToEliminateUsrAccount (const struct UsrData *UsrDat)
  {
   extern const char *Txt_Eliminate_user_account;

   Frm_StartForm (ActUpdOth);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Par_PutHiddenParamUnsigned ("RegRemAction",(unsigned) Enr_ELIMINATE_ONE_USR_FROM_PLATFORM);
   Btn_PutRemoveButtonInline (Txt_Eliminate_user_account);
   Frm_EndForm ();
  }

/*****************************************************************************/
/****** Put button to remove user from list of possible duplicate users ******/
/*****************************************************************************/

static void Dup_PutButtonToRemoveFromListOfDupUsrs (const struct UsrData *UsrDat)
  {
   extern const char *Txt_Not_duplicated;

   Frm_StartForm (ActRemDupUsr);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Btn_PutConfirmButtonInline (Txt_Not_duplicated);
   Frm_EndForm ();
  }

/*****************************************************************************/
/*********** Remove user from list of possible duplicate users ***************/
/*****************************************************************************/

void Dup_RemoveUsrFromListDupUsrs (void)
  {
   /***** Get user to be removed from list of possible duplicates *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      /* Remove entry from database */
      Dup_RemoveUsrFromDuplicated (Gbl.Usrs.Other.UsrDat.UsrCod);

      /* Show list of similar users again */
      // Dup_ListSimilarUsrsInternal ();

      /* Show list of possible duplicated users again */
      Dup_ListDuplicateUsrs ();
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/******* Remove user from list of possible duplicate users in database *******/
/*****************************************************************************/

void Dup_RemoveUsrFromDuplicated (long UsrCod)
  {
   /***** Remove user from list of duplicated users *****/
   DB_QueryDELETE ("can not remove a user from possible duplicates",
		   "DELETE FROM usr_duplicated WHERE UsrCod=%ld",UsrCod);
  }
