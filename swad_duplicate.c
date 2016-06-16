// swad_duplicate.c: duplicate users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

// #include <linux/stddef.h>	// For NULL
// #include <stdlib.h>		// For exit, system, malloc, free, rand, etc.
// #include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_duplicate.h"
#include "swad_global.h"
#include "swad_layout.h"
#include "swad_role.h"
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

/*****************************************************************************/
/******************** Report a user as possible duplicate ********************/
/*****************************************************************************/

void Dup_ReportUsrAsPossibleDuplicate (void)
  {
   extern const char *Txt_Thank_you_for_reporting_a_possible_duplicate_user;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char Query[256];
   bool ItsMe;

   /***** Get user to be reported as possible duplicate *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      /* Check if it's allowed to me to report users as possible duplicatedr */
      ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);
      if (!ItsMe && Gbl.Usrs.Me.LoggedRole >= Rol_TEACHER)
	{
	 /***** Insert possible duplicate into database *****/
         sprintf (Query,"REPLACE INTO usr_duplicated (UsrCod,InformerCod,InformTime)"
                        " VALUES ('%ld','%ld',NOW())",
                  Gbl.Usrs.Other.UsrDat.UsrCod,
                  Gbl.Usrs.Me.UsrDat.UsrCod);
         DB_QueryINSERT (Query,"can not report duplicate");

         /***** Show feedback message *****/
         Lay_ShowAlert (Lay_SUCCESS,Txt_Thank_you_for_reporting_a_possible_duplicate_user);
	}
      else
         Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/************ Put a link (form) to list possible duplicate users *************/
/*****************************************************************************/

void Dup_PutLinkToListDupUsrs (void)
  {
   extern const char *Txt_Duplicate_USERS;

   /***** Put form to remove old users *****/
   Lay_PutContextualLink (ActLstDupUsr,NULL,"usrs64x64.gif",
                          Txt_Duplicate_USERS,Txt_Duplicate_USERS);
  }

/*****************************************************************************/
/*********************** List possible duplicate users ***********************/
/*****************************************************************************/

void Dup_ListDuplicateUsrs (void)
  {
   extern const char *Txt_Possibly_duplicate_users;
   extern const char *Txt_Informants;
   extern const char *Txt_Similar_users;
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct UsrData UsrDat;
   unsigned NumInformants;

   /***** Start frame with list of possible duplicate users *****/
   Lay_StartRoundFrame (NULL,Txt_Possibly_duplicate_users,NULL);

   /***** Build query *****/
   sprintf (Query,"SELECT UsrCod,COUNT(*) AS N,MIN(UNIX_TIMESTAMP(InformTime)) AS T"
	          " FROM usr_duplicated"
		  " GROUP BY UsrCod"
		  " ORDER BY N DESC,T DESC");
   NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get requests for enrollment");

   /***** List possible duplicated users *****/
   if (NumUsrs)
     {
      /***** Initialize field names *****/
      Usr_SetUsrDatMainFieldNames ();

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Start table *****/
      fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\">");

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
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))
           {
            if (Usr_GetNumCrssOfUsr (UsrDat.UsrCod) != 0)
               UsrDat.Accepted = (Usr_GetNumCrssOfUsrNotAccepted (UsrDat.UsrCod) == 0);
            else
               UsrDat.Accepted = false;

            /* Write data of this user */
            Usr_WriteRowUsrMainData (NumUsrs - NumUsr,&UsrDat,false);

            /* Write number of informants (row[1]) if greater than 1 */
	    if (sscanf (row[1],"%u",&NumInformants) != 1)
	       Lay_ShowErrorAndExit ("Wrong number of informers.");
            if (NumInformants > 1)
	       fprintf (Gbl.F.Out,"<tr>"
				  "<td colspan=\"2\" class=\"COLOR%u\"></td>"
				  "<td colspan=\"%u\""
				  " class=\"DAT LEFT_MIDDLE COLOR%u\">"
				  "%s: %u"
				  "</td>"
				  "</tr>",
	                Gbl.RowEvenOdd,
			Usr_NUM_MAIN_FIELDS_DATA_USR-2,
	                Gbl.RowEvenOdd,
			Txt_Informants,
			NumInformants);

            /* Write link to view users similar to this */
	    fprintf (Gbl.F.Out,"<tr>"
			       "<td colspan=\"2\" class=\"COLOR%u\"></td>"
			       "<td colspan=\"%u\""
			       " class=\"DAT LEFT_MIDDLE COLOR%u\">",
	             Gbl.RowEvenOdd,
		     Usr_NUM_MAIN_FIELDS_DATA_USR-2,
	             Gbl.RowEvenOdd);
	    Act_FormStart (ActLstSimUsr);
            Usr_PutParamUsrCodEncrypted (UsrDat.EncryptedUsrCod);
	    Lay_PutConfirmButtonInline (Txt_Similar_users);
	    Act_FormEnd ();
	    fprintf (Gbl.F.Out,"</td>"
			       "</tr>");

	    /* Write all the courses this user belongs to */
	    Crs_GetAndWriteCrssOfAUsr (&UsrDat,Rol_TEACHER);
	    Crs_GetAndWriteCrssOfAUsr (&UsrDat,Rol_STUDENT);

	    Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
           }
         else        // User does not exists ==>
                     // remove user from table of possible duplicate users
            Dup_RemoveUsrFromDuplicated (UsrDat.UsrCod);
        }

      /***** End table *****/
      fprintf (Gbl.F.Out,"</table>");

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
   else	// There are no users
      Lay_ShowAlert (Lay_INFO,Txt_No_users_found[Rol_UNKNOWN]);

   /***** End frame *****/
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/********************* List similar users to a given one *********************/
/*****************************************************************************/

// TODO: Write the code of this function

void Dup_ListSimilarUsrs (void)
  {
   extern const char *Txt_Possibly_duplicate_users;
   extern const char *Txt_Informants;
   extern const char *Txt_Similar_users;
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct UsrData UsrDat;
   unsigned NumInformants;

   /***** Start frame with list of possible duplicate users *****/
   Lay_StartRoundFrame (NULL,Txt_Possibly_duplicate_users,NULL);

   /***** Build query *****/
   sprintf (Query,"SELECT UsrCod,COUNT(*) AS N,MIN(UNIX_TIMESTAMP(InformTime)) AS T"
	          " FROM usr_duplicated"
		  " GROUP BY UsrCod"
		  " ORDER BY N DESC,T DESC");
   NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get requests for enrollment");

   /***** List possible duplicated users *****/
   if (NumUsrs)
     {
      /***** Initialize field names *****/
      Usr_SetUsrDatMainFieldNames ();

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Start table *****/
      fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\">");

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
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))
           {
            if (Usr_GetNumCrssOfUsr (UsrDat.UsrCod) != 0)
               UsrDat.Accepted = (Usr_GetNumCrssOfUsrNotAccepted (UsrDat.UsrCod) == 0);
            else
               UsrDat.Accepted = false;

            /* Write data of this user */
            Usr_WriteRowUsrMainData (NumUsrs - NumUsr,&UsrDat,false);

            /* Write number of informants (row[1]) if greater than 1 */
	    if (sscanf (row[1],"%u",&NumInformants) != 1)
	       Lay_ShowErrorAndExit ("Wrong number of informers.");
            if (NumInformants > 1)
	       fprintf (Gbl.F.Out,"<tr>"
				  "<td colspan=\"2\" class=\"COLOR%u\"></td>"
				  "<td colspan=\"%u\""
				  " class=\"DAT LEFT_MIDDLE COLOR%u\">"
				  "%s: %u"
				  "</td>"
				  "</tr>",
	                Gbl.RowEvenOdd,
			Usr_NUM_MAIN_FIELDS_DATA_USR-2,
	                Gbl.RowEvenOdd,
			Txt_Informants,
			NumInformants);

            /* Write link to view users similar to this */
	    fprintf (Gbl.F.Out,"<tr>"
			       "<td colspan=\"2\" class=\"COLOR%u\"></td>"
			       "<td colspan=\"%u\""
			       " class=\"DAT LEFT_MIDDLE COLOR%u\">",
	             Gbl.RowEvenOdd,
		     Usr_NUM_MAIN_FIELDS_DATA_USR-2,
	             Gbl.RowEvenOdd);
	    Act_FormStart (ActLstSimUsr);
            Usr_PutParamUsrCodEncrypted (UsrDat.EncryptedUsrCod);
	    Lay_PutConfirmButtonInline (Txt_Similar_users);
	    Act_FormEnd ();
	    fprintf (Gbl.F.Out,"</td>"
			       "</tr>");

	    /* Write all the courses this user belongs to */
	    Crs_GetAndWriteCrssOfAUsr (&UsrDat,Rol_TEACHER);
	    Crs_GetAndWriteCrssOfAUsr (&UsrDat,Rol_STUDENT);

	    Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
           }
         else        // User does not exists ==>
                     // remove user from table of possible duplicate users
            Dup_RemoveUsrFromDuplicated (UsrDat.UsrCod);
        }

      /***** End table *****/
      fprintf (Gbl.F.Out,"</table>");

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
   else	// There are no users
      Lay_ShowAlert (Lay_INFO,Txt_No_users_found[Rol_UNKNOWN]);

   /***** End frame *****/
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/********************* Remove a request for enrollment ***********************/
/*****************************************************************************/

void Dup_RemoveUsrFromDuplicated (long UsrCod)
  {
   char Query[128];

   /***** Remove enrollment request *****/
   sprintf (Query,"DELETE FROM usr_duplicated WHERE UsrCod='%ld'",
            UsrCod);
   DB_QueryDELETE (Query,"can not remove a user from possible duplicates");
  }
