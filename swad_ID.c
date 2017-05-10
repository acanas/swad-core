// swad_ID.c: Users' IDs

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

#include <ctype.h>		// For isalnum, isdigit, etc.
#include <stdbool.h>		// For boolean type
#include <stdlib.h>		// For exit, system, malloc, free, rand, etc.
#include <string.h>		// For string functions

#include "swad_account.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_ID.h"
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

#define ID_MAX_IDS_PER_USER	3	// Maximum number of IDs per user

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static bool ID_CheckIfUsrIDIsValidUsingMinDigits (const char *UsrID,unsigned MinDigits);

static void ID_PutLinkToConfirmID (struct UsrData *UsrDat,unsigned NumID,
                                   const char *Anchor);

static void ID_RemoveUsrID (const struct UsrData *UsrDat,bool ItsMe);
static bool ID_CheckIfConfirmed (long UsrCod,const char *UsrID);
static void ID_RemoveUsrIDFromDB (long UsrCod,const char *UsrID);
static void ID_NewUsrID (const struct UsrData *UsrDat,bool ItsMe);
static void ID_InsertANewUsrIDInDB (long UsrCod,const char *NewID,bool Confirmed);

/*****************************************************************************/
/********************** Get list of IDs of a user ****************************/
/*****************************************************************************/

void ID_GetListIDsFromUsrCod (struct UsrData *UsrDat)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumIDs;
   unsigned NumID;

   /***** Initialize list of IDs to an empty list *****/
   ID_FreeListIDs (UsrDat);

   if (UsrDat->UsrCod > 0)
     {
      /***** Get user's IDs from database *****/
      // First the confirmed (Confirmed == 'Y')
      // Then the unconfirmed (Confirmed == 'N')
      sprintf (Query,"SELECT UsrID,Confirmed FROM usr_IDs"
	             " WHERE UsrCod=%ld"
	             " ORDER BY Confirmed DESC,UsrID",
               UsrDat->UsrCod);
      if ((NumIDs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get user's IDs")))
	{
	 /***** Allocate space for the list *****/
         ID_ReallocateListIDs (UsrDat,NumIDs);

         /***** Get list of IDs *****/
	 for (NumID = 0;
	      NumID < NumIDs;
	      NumID++)
	   {
            row = mysql_fetch_row (mysql_res);

	    /* Get ID from row[0] */
            Str_Copy (UsrDat->IDs.List[NumID].ID,row[0],
                      ID_MAX_BYTES_USR_ID);

            /* Get if ID is confirmed from row[1] */
            UsrDat->IDs.List[NumID].Confirmed = (row[1][0] == 'Y');
	   }
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/***************** Free memory allocated for list of IDs *********************/
/*****************************************************************************/

void ID_ReallocateListIDs (struct UsrData *UsrDat,unsigned NumIDs)
  {
   /***** Free list of IDs if used *****/
   ID_FreeListIDs (UsrDat);

   /***** Assign number of IDs in list *****/
   UsrDat->IDs.Num = NumIDs;

   /***** Allocate space for the list *****/
   if ((UsrDat->IDs.List = (struct ListIDs *) malloc (NumIDs * sizeof (struct ListIDs))) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store list of user's IDs.");
  }

/*****************************************************************************/
/***************** Free memory allocated for list of IDs *********************/
/*****************************************************************************/

void ID_FreeListIDs (struct UsrData *UsrDat)
  {
   /***** Free list *****/
   if (UsrDat->IDs.Num && UsrDat->IDs.List)
      free ((void *) UsrDat->IDs.List);

   /***** Reset list *****/
   UsrDat->IDs.List = NULL;
   UsrDat->IDs.Num = 0;
  }

/*****************************************************************************/
/***************** Get list of user codes from user's IDs ********************/
/*****************************************************************************/
// Returns the number of users with any of these IDs
// The list of users' codes is allocated inside this function and should be freed by caller

unsigned ID_GetListUsrCodsFromUsrID (struct UsrData *UsrDat,
                                     const char *EncryptedPassword,	// If NULL or empty ==> do not check password
                                     struct ListUsrCods *ListUsrCods,
                                     bool OnlyConfirmedIDs)
  {
   char SubQuery[256];
   char *Query;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   size_t MaxLength;
   unsigned NumID;
   unsigned NumUsr;
   bool CheckPassword = false;

   if (UsrDat->IDs.Num)
     {
      if (EncryptedPassword)
	 if (EncryptedPassword[0])
	    CheckPassword = true;

      /***** Allocate memory for query string *****/
      MaxLength = 512 + UsrDat->IDs.Num * (1 + ID_MAX_BYTES_USR_ID + 1) - 1;
      if ((Query = (char *) malloc (MaxLength + 1)) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store list of user's IDs.");

      /***** Get user's code(s) from database *****/
      Str_Copy (Query,CheckPassword ? "SELECT DISTINCT(usr_IDs.UsrCod) FROM usr_IDs,usr_data"
				      " WHERE usr_IDs.UsrID IN (" :
				      "SELECT DISTINCT(UsrCod) FROM usr_IDs"
				      " WHERE UsrID IN (",
		MaxLength);
      for (NumID = 0;
	   NumID < UsrDat->IDs.Num;
	   NumID++)
	{
	 if (NumID)
	    Str_Concat (Query,",",
	                MaxLength);
	 sprintf (SubQuery,"'%s'",UsrDat->IDs.List[NumID].ID);

	 Str_Concat (Query,SubQuery,
	             MaxLength);
	}
      Str_Concat (Query,")",
                  MaxLength);

      if (CheckPassword)
	{
	 if (OnlyConfirmedIDs)
	    Str_Concat (Query," AND usr_IDs.Confirmed='Y'",
	                MaxLength);

	 // Get user's code if I have written the correct password
	 // or if password in database is empty (new user)
	 sprintf (SubQuery," AND usr_IDs.UsrCod=usr_data.UsrCod"
			   " AND (usr_data.Password='%s' OR usr_data.Password='')",
		  EncryptedPassword);
	 Str_Concat (Query,SubQuery,
	             MaxLength);
	}
      else if (OnlyConfirmedIDs)
	 Str_Concat (Query," AND Confirmed='Y'",
	             MaxLength);

      ListUsrCods->NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get user's codes");

      /***** Free memory for query string *****/
      free ((void *) Query);

      if (ListUsrCods->NumUsrs)
        {
	 /***** Allocate space for the list of users' codes *****/
	 Usr_AllocateListUsrCods (ListUsrCods);
	 // The list should be freed by caller

	 /***** Fill the list *****/
	 for (NumUsr = 0;
	      NumUsr < ListUsrCods->NumUsrs;
	      NumUsr++)
	   {
	    /***** Get user's code *****/
	    row = mysql_fetch_row (mysql_res);
	    ListUsrCods->Lst[NumUsr] = Str_ConvertStrCodToLongCod (row[0]);
	   }
	 UsrDat->UsrCod = ListUsrCods->Lst[0];	// The first user found
        }
      else	// ListUsrCods->NumUsrs == 0
        {
         ListUsrCods->Lst = NULL;
         UsrDat->UsrCod = -1L;
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else	// List of user's IDs is empty
     {
      ListUsrCods->NumUsrs = 0;
      ListUsrCods->Lst = NULL;
      UsrDat->UsrCod = -1L;
     }

   return ListUsrCods->NumUsrs;
  }

/*****************************************************************************/
/******* Put hidden parameter with the plain user's ID of other user *********/
/*****************************************************************************/

void ID_PutParamOtherUsrIDPlain (void)
  {
   Par_PutHiddenParamString ("OtherUsrID",
	                     (Gbl.Usrs.Other.UsrDat.IDs.Num &&
	                      Gbl.Usrs.Other.UsrDat.IDs.List) ? Gbl.Usrs.Other.UsrDat.IDs.List[0].ID :
	                                                        "");
  }

/*****************************************************************************/
/********* Get parameter plain user's ID of other user from a form ***********/
/*****************************************************************************/

void ID_GetParamOtherUsrIDPlain (void)
  {
   /***** Allocate space for the list *****/
   ID_ReallocateListIDs (&Gbl.Usrs.Other.UsrDat,1);

   /***** Get parameter *****/
   Par_GetParToText ("OtherUsrID",Gbl.Usrs.Other.UsrDat.IDs.List[0].ID,
                     ID_MAX_BYTES_USR_ID);
   // Users' IDs are always stored internally in capitals and without leading zeros
   Str_RemoveLeadingZeros (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID);
   Str_ConvertToUpperText (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID);

   Gbl.Usrs.Other.UsrDat.IDs.List[0].Confirmed = true;
  }

/*****************************************************************************/
/****** Check whether a user's ID without the ending letter is valid *********/
/*****************************************************************************/
// Returns true if the user's ID string is valid, or false if not
// A valid user's ID must...:
// 1. Must be ID_MIN_BYTES_USR_ID <= characters <= ID_MAX_BYTES_USR_ID.
// 2. All characters must be digits or letters
// 3. Must have a minimum number of digits

// Wrapper function to avoid passing extra parameters
bool ID_CheckIfUsrIDIsValid (const char *UsrID)
  {
   if (UsrID)
      if (UsrID[0])
         return ID_CheckIfUsrIDIsValidUsingMinDigits (UsrID,ID_MIN_DIGITS_USR_ID);

   return false;
  }

// Wrapper function to avoid passing extra parameters
bool ID_CheckIfUsrIDSeemsAValidID (const char *UsrID)
  {
   if (UsrID)
      if (UsrID[0])
         return ID_CheckIfUsrIDIsValidUsingMinDigits (UsrID,ID_MIN_DIGITS_AUTOMATIC_DETECT_USR_ID);

   return false;
  }

static bool ID_CheckIfUsrIDIsValidUsingMinDigits (const char *UsrID,unsigned MinDigits)
  {
   const char *Ptr;
   unsigned NumDigits = 0;
   unsigned Length;

   /***** Check length *****/
   if (!UsrID)
      return false;
   if (!UsrID[0])
      return false;
   Length = strlen (UsrID);
   if (Length < ID_MIN_BYTES_USR_ID ||
       Length > ID_MAX_BYTES_USR_ID)
      return false;			// 1. Must be ID_MIN_BYTES_USR_ID <= characters <= ID_MAX_BYTES_USR_ID

   /**** Loop through user's ID *****/
   for (Ptr = UsrID;
        *Ptr;
        Ptr++)
      if (isdigit ((int) *Ptr))                        // If character is digit
         NumDigits++;
      else if (!((*Ptr >= 'A' && *Ptr <= 'Z') ||
                 (*Ptr >= 'a' && *Ptr <= 'z')))        // If character is not alpha
         return false;			// 2. All characters must be digits or letters

   return (NumDigits >= MinDigits);	// 3. Must have MinDigits digits at least
  }

/*****************************************************************************/
/*************************** Write list of user's ID *************************/
/*****************************************************************************/

void ID_WriteUsrIDs (struct UsrData *UsrDat,const char *Anchor)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   unsigned NumID;
   bool ICanSeeUsrID;
   bool ICanConfirmUsrID;

   ICanSeeUsrID = ID_ICanSeeOtherUsrIDs (UsrDat);
   ICanConfirmUsrID = ICanSeeUsrID &&
	              (UsrDat->UsrCod != Gbl.Usrs.Me.UsrDat.UsrCod) &&			// Not me
	              !Gbl.Form.Inside &&						// Not inside another form
                      Act_Actions[Gbl.Action.Act].BrowserWindow == Act_THIS_WINDOW;	// Only in main window

   for (NumID = 0;
	NumID < UsrDat->IDs.Num;
	NumID++)
     {
      if (NumID)
	 fprintf (Gbl.F.Out,"<br />");

      fprintf (Gbl.F.Out,"<span class=\"%s\">",
	       UsrDat->IDs.List[NumID].Confirmed ? "USR_ID_C" :
						   "USR_ID_NC");
      if (ICanSeeUsrID)
	 fprintf (Gbl.F.Out,"%s",UsrDat->IDs.List[NumID].ID);
      else
	 fprintf (Gbl.F.Out,"********");
      fprintf (Gbl.F.Out,"</span>");

      if (ICanConfirmUsrID &&
	  !UsrDat->IDs.List[NumID].Confirmed)
	 ID_PutLinkToConfirmID (UsrDat,NumID,Anchor);
     }
  }

/*****************************************************************************/
/***************** Check if I can see another user's IDs *********************/
/*****************************************************************************/

bool ID_ICanSeeOtherUsrIDs (const struct UsrData *UsrDat)
  {
   if (UsrDat->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// It's me
      return true;

   /***** Check if I have permission to see another user's IDs *****/
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_TEACHER:
	 /* Check 1: I can see the IDs of users who do not exist in database */
         if (UsrDat->UsrCod <= 0)	// User does not exist (when creating a new user)
            return true;

	 /* Check 2: I can see the IDs of confirmed students */
         if (UsrDat->RoleInCurrentCrsDB == Rol_STUDENT &&	// A student
	     UsrDat->Accepted)					// who accepted registration
            return true;

         /* Check 3: I can see the IDs of users with user's data empty */
         // This check is made to not view simultaneously:
         // - an ID
         // - a name or an email
         if (!UsrDat->Password[0] &&	// User has no password (never logged)
	     !UsrDat->Surname1[0] &&	// and who has no surname 1 (nobody filled user's surname 1)
	     !UsrDat->Surname2[0] &&	// and who has no surname 2 (nobody filled user's surname 2)
	     !UsrDat->FirstName[0] &&	// and who has no first name (nobody filled user's first name)
             !UsrDat->Email[0])		// and who has no email (nobody filled user's email)
            return true;

         return false;
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
         return Usr_ICanEditOtherUsr (UsrDat);
      default:
	 return false;
     }
  }

/*****************************************************************************/
/****************** Put a link to confirm of another user's ID ***************/
/*****************************************************************************/

static void ID_PutLinkToConfirmID (struct UsrData *UsrDat,unsigned NumID,
                                   const char *Anchor)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_Confirm_ID;

   /***** Start form *****/
   Act_FormStartAnchor ( UsrDat->RoleInCurrentCrsDB == Rol_STUDENT ? ActCnfID_Std :
		        (UsrDat->RoleInCurrentCrsDB == Rol_TEACHER ? ActCnfID_Tch :
							             ActCnfID_Oth),
                        Anchor);
   if (Gbl.Action.Original != ActUnk)
     {
      Par_PutHiddenParamLong ("OriginalActCod",
                              Act_Actions[Gbl.Action.Original].ActCod);	// Original action, used to know where we came from
      switch (Gbl.Action.Original)
	{
	 case ActSeeRecSevGst:
	    Usr_PutHiddenParUsrCodAll (ActCnfID_Oth,Gbl.Usrs.Select.All);
	    break;
	 case ActSeeRecSevStd:
	    Usr_PutHiddenParUsrCodAll (ActCnfID_Std,Gbl.Usrs.Select.All);
	    break;
	 case ActSeeRecSevTch:
	    Usr_PutHiddenParUsrCodAll (ActCnfID_Tch,Gbl.Usrs.Select.All);
	    break;
	}
     }
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"UsrID\" value=\"%s\" />",
	    UsrDat->IDs.List[NumID].ID);

   /***** Put link *****/
   Lay_PutIconLink ("ok_on16x16.gif",Txt_Confirm_ID,Txt_Confirm_ID,
                    The_ClassFormBold[Gbl.Prefs.Theme],NULL);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/*********** Put a link to the action used to request user's IDs *************/
/*****************************************************************************/

void ID_PutLinkToChangeUsrIDs (void)
  {
   extern const char *Txt_Change_IDs;

   /***** Link for changing the password *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// It's me
      Lay_PutContextualLink (ActFrmMyAcc,NULL,NULL,
			     "arroba64x64.gif",
			     Txt_Change_IDs,Txt_Change_IDs,
                             NULL);
   else									// Not me
      Lay_PutContextualLink ( Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB == Rol_STUDENT ? ActFrmIDsStd :
	                     (Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB == Rol_TEACHER ? ActFrmIDsTch :
	                	                                                        ActFrmIDsOth),	// Guest, visitor or admin
                             NULL,Usr_PutParamOtherUsrCodEncrypted,
			     "arroba64x64.gif",
			     Txt_Change_IDs,Txt_Change_IDs,
                             NULL);
  }

/*****************************************************************************/
/************* Show form to the change of IDs of another user ****************/
/*****************************************************************************/

void ID_ShowFormOthIDs (void)
  {
   extern const char *Txt_ID;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get user whose password must be changed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Usr_ICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	{
	 /***** Start frame *****/
         Lay_StartRoundFrame (NULL,Txt_ID,NULL,NULL);

	 /***** Show user's record *****/
	 Rec_ShowSharedUsrRecord (Rec_SHA_RECORD_LIST,
	                          &Gbl.Usrs.Other.UsrDat,NULL);

	 /***** Form with the user's ID *****/
         Lay_StartTableWide (2);
         ID_ShowFormChangeUsrID (&Gbl.Usrs.Other.UsrDat,
                                 (Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod));	// It's me?
         Lay_EndTable ();

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
/*********************** Show form to change my user's ID ********************/
/*****************************************************************************/

void ID_ShowFormChangeUsrID (const struct UsrData *UsrDat,bool ItsMe)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_ID;
   extern const char *Txt_ID_X_confirmed;
   extern const char *Txt_ID_X_not_confirmed;
   extern const char *Txt_Another_ID;
   extern const char *Txt_Add_this_ID;
   extern const char *Txt_If_there_are_multiple_versions_of_the_ID_;
   extern const char *Txt_The_ID_is_used_in_order_to_facilitate_;
   unsigned NumID;

   /***** List existing user's IDs *****/
   for (NumID = 0;
	NumID < UsrDat->IDs.Num;
	NumID++)
     {
      if (NumID == 0)
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_TOP\">"
			    "%s:"
			    "</td>"
			    "<td class=\"LEFT_TOP\">",
		  The_ClassForm[Gbl.Prefs.Theme],Txt_ID);
      else	// NumID >= 1
         fprintf (Gbl.F.Out,"<br />");

      if (UsrDat->IDs.Num > 1)	// I have two or more IDs
	{
	 if (ItsMe && UsrDat->IDs.List[NumID].Confirmed)	// I can not remove my confirmed IDs
            /* Put disabled icon to remove user's ID */
            Lay_PutIconRemovalNotAllowed ();
	 else							// I can remove
	   {
	    /* Form to remove user's ID */
	    if (ItsMe)
	       Act_FormStart (ActRemID_Me);
	    else
	      {
	       Act_FormStart ( UsrDat->RoleInCurrentCrsDB == Rol_STUDENT ? ActRemID_Std :
	                      (UsrDat->RoleInCurrentCrsDB == Rol_TEACHER ? ActRemID_Tch :
	                	                                           ActRemID_Oth));	// Guest, visitor or admin
	       Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	      }
	    fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"UsrID\" value=\"%s\" />",
		     UsrDat->IDs.List[NumID].ID);
	    Lay_PutIconRemove ();
	    Act_FormEnd ();
	   }
	}

      /* User's ID */
      sprintf (Gbl.Title,
               UsrDat->IDs.List[NumID].Confirmed ? Txt_ID_X_confirmed :
                                                   Txt_ID_X_not_confirmed,
               UsrDat->IDs.List[NumID].ID);
      fprintf (Gbl.F.Out,"<span class=\"USR_ID %s\" title=\"%s\">%s</span>",
               UsrDat->IDs.List[NumID].Confirmed ? "USR_ID_C" :
                                                   "USR_ID_NC",
               Gbl.Title,
               UsrDat->IDs.List[NumID].ID);

      /* ID confirmed? */
      if (UsrDat->IDs.List[NumID].Confirmed)
	{
	 sprintf (Gbl.Title,Txt_ID_X_confirmed,UsrDat->IDs.List[NumID].ID);
	 fprintf (Gbl.F.Out,"<img src=\"%s/ok_green16x16.gif\""
			    " alt=\"%s\" title=\"%s\""
			    " class=\"ICO20x20\" />",
		  Gbl.Prefs.IconsURL,
		  Gbl.Title,Gbl.Title);
	}

      if (NumID == UsrDat->IDs.Num - 1)
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");
     }

   /***** Form to enter new user's ID *****/
   if (UsrDat->IDs.Num < ID_MAX_IDS_PER_USER)
     {
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"RIGHT_MIDDLE\">"
			 "<label for=\"NewID\" class=\"%s\">%s:</label>"
			 "</td>"
			 "<td class=\"LEFT_MIDDLE\">",
	       The_ClassForm[Gbl.Prefs.Theme],
	       UsrDat->IDs.Num ? Txt_Another_ID :	// A new user's ID
		                 Txt_ID);		// The first user's ID
      if (ItsMe)
	 Act_FormStart (ActNewIDMe);
      else
	{
	 Act_FormStart ( UsrDat->RoleInCurrentCrsDB == Rol_STUDENT ? ActNewID_Std :
	                (UsrDat->RoleInCurrentCrsDB == Rol_TEACHER ? ActNewID_Tch :
	                	                                     ActNewID_Oth));	// Guest, visitor or admin
	 Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	}
      fprintf (Gbl.F.Out,"<div class=\"FORM_ACCOUNT\">"
                         "<input type=\"text\" id=\"NewID\" name=\"NewID\""
	                 " size=\"18\" maxlength=\"%u\" value=\"%s\" />"
	                 "</div>",
	       ID_MAX_BYTES_USR_ID,
	       UsrDat->IDs.Num ? UsrDat->IDs.List[UsrDat->IDs.Num - 1].ID :
		                 "");	// Show the most recent ID
      Lay_PutCreateButtonInline (Txt_Add_this_ID);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>"
			 "<tr>");
     }

   /***** Write help text *****/
   fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"DAT CENTER_MIDDLE\">");
   if (ItsMe)
      fprintf (Gbl.F.Out,"%s ",
               Txt_The_ID_is_used_in_order_to_facilitate_);
   fprintf (Gbl.F.Out,"%s"
		      "</td>"
		      "</tr>",
            Txt_If_there_are_multiple_versions_of_the_ID_);
  }

/*****************************************************************************/
/********************** Remove one of my user's IDs **************************/
/*****************************************************************************/

void ID_RemoveMyUsrID (void)
  {
   /***** Remove user's ID *****/
   ID_RemoveUsrID (&Gbl.Usrs.Me.UsrDat,true);	// It's me

   /***** Update list of IDs *****/
   ID_GetListIDsFromUsrCod (&Gbl.Usrs.Me.UsrDat);

   /***** Show my account again *****/
   Acc_ShowFormChangeMyAccount ();
  }

/*****************************************************************************/
/**************** Remove one of the user's IDs of another user ***************/
/*****************************************************************************/

void ID_RemoveOtherUsrID (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get other user's code from form and get user's data *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      /***** Remove user's ID *****/
      ID_RemoveUsrID (&Gbl.Usrs.Other.UsrDat,
                      (Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod));	// It's me?

      /***** Update list of IDs *****/
      ID_GetListIDsFromUsrCod (&Gbl.Usrs.Other.UsrDat);

      /***** Show user's record *****/
      Rec_ShowSharedUsrRecord (Rec_SHA_RECORD_LIST,
                               &Gbl.Usrs.Other.UsrDat,NULL);
     }
   else		// User not found
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/***************************** Remove user's ID ******************************/
/*****************************************************************************/

static void ID_RemoveUsrID (const struct UsrData *UsrDat,bool ItsMe)
  {
   extern const char *Txt_ID_X_removed;
   extern const char *Txt_You_can_not_delete_this_ID;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char UsrID[ID_MAX_BYTES_USR_ID + 1];
   bool ICanRemove;

   if (Usr_ICanEditOtherUsr (UsrDat))
     {
      /***** Get user's ID from form *****/
      Par_GetParToText ("UsrID",UsrID,ID_MAX_BYTES_USR_ID);
      // Users' IDs are always stored internally in capitals and without leading zeros
      Str_RemoveLeadingZeros (UsrID);
      Str_ConvertToUpperText (UsrID);

      if (UsrDat->IDs.Num < 2)	// One unique ID
	 ICanRemove = false;
      else if (ItsMe)
	 // I can remove my ID only if it is not confirmed
	 ICanRemove = !ID_CheckIfConfirmed (UsrDat->UsrCod,UsrID);
      else
	 ICanRemove = true;

      if (ICanRemove)
	{
	 /***** Remove one of the user's IDs *****/
	 ID_RemoveUsrIDFromDB (UsrDat->UsrCod,UsrID);

	 /***** Show message *****/
	 sprintf (Gbl.Message,Txt_ID_X_removed,UsrID);
	 Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
	}
      else
	 Lay_ShowAlert (Lay_WARNING,Txt_You_can_not_delete_this_ID);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/************************ Check if an ID is confirmed ************************/
/*****************************************************************************/

static bool ID_CheckIfConfirmed (long UsrCod,const char *UsrID)
  {
   char Query[128];

   /***** Get if ID is confirmed from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_IDs"
		  " WHERE UsrCod=%ld AND UsrID='%s' AND Confirmed='Y'",
	    UsrCod,UsrID);
   return (DB_QueryCOUNT (Query,"can not check if ID is confirmed") != 0);
  }

/*****************************************************************************/
/**************** Remove one of my user's IDs from database ******************/
/*****************************************************************************/

static void ID_RemoveUsrIDFromDB (long UsrCod,const char *UsrID)
  {
   char Query[256 + ID_MAX_BYTES_USR_ID];

   /***** Remove one of my user's IDs *****/
   sprintf (Query,"DELETE FROM usr_IDs"
                  " WHERE UsrCod=%ld AND UsrID='%s'",
            UsrCod,UsrID);
   DB_QueryREPLACE (Query,"can not remove a user's ID");
  }

/*****************************************************************************/
/************************* New user's ID for me ******************************/
/*****************************************************************************/

void ID_NewMyUsrID (void)
  {
   /***** Remove user's ID *****/
   ID_NewUsrID (&Gbl.Usrs.Me.UsrDat,true);	// It's me

   /***** Update list of IDs *****/
   ID_GetListIDsFromUsrCod (&Gbl.Usrs.Me.UsrDat);

   /***** Show my account again *****/
   Acc_ShowFormChangeMyAccount ();
  }

/*****************************************************************************/
/********************* New user's ID for another user ************************/
/*****************************************************************************/

void ID_NewOtherUsrID (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get other user's code from form and get user's data *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      /***** New user's ID *****/
      ID_NewUsrID (&Gbl.Usrs.Other.UsrDat,
                   (Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod));	// It's me?

      /***** Update list of IDs *****/
      ID_GetListIDsFromUsrCod (&Gbl.Usrs.Other.UsrDat);

      /***** Show user's record *****/
      Rec_ShowSharedUsrRecord (Rec_SHA_RECORD_LIST,
                               &Gbl.Usrs.Other.UsrDat,NULL);
     }
   else		// User not found
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/***************************** New user's ID *********************************/
/*****************************************************************************/

static void ID_NewUsrID (const struct UsrData *UsrDat,bool ItsMe)
  {
   extern const char *Txt_The_ID_X_matches_one_of_the_existing;
   extern const char *Txt_The_ID_X_has_been_confirmed;
   extern const char *Txt_A_user_can_not_have_more_than_X_IDs;
   extern const char *Txt_The_ID_X_has_been_registered_successfully;
   extern const char *Txt_The_ID_X_is_not_valid;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char NewID[ID_MAX_BYTES_USR_ID + 1];
   unsigned NumID;
   bool AlreadyExists;
   unsigned NumIDFound = 0;	// Initialized to avoid warning
   bool Error = false;

   if (Usr_ICanEditOtherUsr (UsrDat))
     {
      /***** Get new user's ID from form *****/
      Par_GetParToText ("NewID",NewID,ID_MAX_BYTES_USR_ID);
      // Users' IDs are always stored internally in capitals and without leading zeros
      Str_RemoveLeadingZeros (NewID);
      Str_ConvertToUpperText (NewID);

      if (ID_CheckIfUsrIDIsValid (NewID))        // If new ID is valid
	{
	 /***** Check if the new ID matches any of the old IDs *****/
	 for (NumID = 0, AlreadyExists = false;
	      NumID < UsrDat->IDs.Num && !AlreadyExists;
	      NumID++)
	    if (!strcasecmp (UsrDat->IDs.List[NumID].ID,NewID))
	      {
	       AlreadyExists = true;
	       NumIDFound = NumID;
	      }

	 if (AlreadyExists)	// This new ID was already associated to this user
	   {
	    if (ItsMe || UsrDat->IDs.List[NumIDFound].Confirmed)
	      {
	       Error = true;
	       sprintf (Gbl.Message,Txt_The_ID_X_matches_one_of_the_existing,
			NewID);
	      }
	    else	// It's not me && !Confirmed
	      {
	       /***** Mark this ID as confirmed *****/
	       ID_ConfirmUsrID (UsrDat,NewID);
	       sprintf (Gbl.Message,Txt_The_ID_X_has_been_confirmed,
			NewID);
	      }
	   }
	 else if (UsrDat->IDs.Num >= ID_MAX_IDS_PER_USER)
	   {
	    Error = true;
	    sprintf (Gbl.Message,Txt_A_user_can_not_have_more_than_X_IDs,
		     ID_MAX_IDS_PER_USER);
	   }
	 else	// OK ==> add this new ID to my list of IDs
	   {
	    /***** Save this new ID *****/
	    // It's me ==> ID not confirmed
	    // It's not me ==> ID confirmed
	    ID_InsertANewUsrIDInDB (UsrDat->UsrCod,NewID,!ItsMe);

	    sprintf (Gbl.Message,Txt_The_ID_X_has_been_registered_successfully,
		     NewID);
	   }
	}
      else        // New ID is not valid
	{
	 Error = true;
	 sprintf (Gbl.Message,Txt_The_ID_X_is_not_valid,NewID);
	}

      /***** Show message *****/
      Lay_ShowAlert (Error ? Lay_WARNING :
			     Lay_SUCCESS,
		     Gbl.Message);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/******************* Insert a new ID for me in database **********************/
/*****************************************************************************/

static void ID_InsertANewUsrIDInDB (long UsrCod,const char *NewID,bool Confirmed)
  {
   char Query[256 + ID_MAX_BYTES_USR_ID];

   /***** Update my nickname in database *****/
   sprintf (Query,"INSERT INTO usr_IDs"
                  " (UsrCod,UsrID,CreatTime,Confirmed)"
                  " VALUES"
                  " (%ld,'%s',NOW(),'%c')",
            UsrCod,NewID,
            Confirmed ? 'Y' :
        	        'N');
   DB_QueryINSERT (Query,"can not insert a new ID");
  }

/*****************************************************************************/
/************************ Confirm another user's ID **************************/
/*****************************************************************************/

void ID_ConfirmOtherUsrID (void)
  {
   extern const char *Txt_ID_X_had_already_been_confirmed;
   extern const char *Txt_The_ID_X_has_been_confirmed;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   long OriginalActCod;
   char UsrID[ID_MAX_BYTES_USR_ID + 1];
   bool ICanConfirm;
   bool Found;
   unsigned NumID;
   unsigned NumIDFound = 0;	// Initialized to avoid warning

   /***** Initialize alert type and message *****/
   Gbl.AlertType = Lay_NONE;	// Do not show alert

   /***** Get where we came from *****/
   OriginalActCod = Par_GetParToLong ("OriginalActCod");
   Gbl.Action.Original = Act_GetActionFromActCod (OriginalActCod);

   /***** Get other user's code from form and get user's data *****/
   ICanConfirm = false;
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
      if (Gbl.Usrs.Other.UsrDat.UsrCod != Gbl.Usrs.Me.UsrDat.UsrCod)	// Not me
	 if (ID_ICanSeeOtherUsrIDs (&Gbl.Usrs.Other.UsrDat))
	    ICanConfirm = true;

   if (ICanConfirm)
     {
      /***** Get user's ID from form *****/
      Par_GetParToText ("UsrID",UsrID,ID_MAX_BYTES_USR_ID);
      // Users' IDs are always stored internally in capitals and without leading zeros
      Str_RemoveLeadingZeros (UsrID);
      Str_ConvertToUpperText (UsrID);

      for (NumID = 0, Found = false;
	   NumID < Gbl.Usrs.Other.UsrDat.IDs.Num && !Found;
	   NumID++)
	 if (!strcasecmp (UsrID,Gbl.Usrs.Other.UsrDat.IDs.List[NumID].ID))
	   {
	    Found = true;
	    NumIDFound = NumID;
	   }

      if (Found)	// Found
	{
	 if (Gbl.Usrs.Other.UsrDat.IDs.List[NumIDFound].Confirmed)
	   {
	    /***** ID found and already confirmed *****/
            Gbl.AlertType = Lay_INFO;
	    sprintf (Gbl.Message,Txt_ID_X_had_already_been_confirmed,
		     Gbl.Usrs.Other.UsrDat.IDs.List[NumIDFound].ID);
	   }
	 else
	   {
	    /***** Mark this ID as confirmed *****/
	    ID_ConfirmUsrID (&Gbl.Usrs.Other.UsrDat,
			     Gbl.Usrs.Other.UsrDat.IDs.List[NumIDFound].ID);
	    Gbl.Usrs.Other.UsrDat.IDs.List[NumIDFound].Confirmed = true;

	    /***** Write success message *****/
	    Gbl.AlertType = Lay_SUCCESS;
	    sprintf (Gbl.Message,Txt_The_ID_X_has_been_confirmed,
		     Gbl.Usrs.Other.UsrDat.IDs.List[NumIDFound].ID);
	   }
	}
      else	// User's ID not found
	{
	 Gbl.AlertType = Lay_WARNING;
         sprintf (Gbl.Message,"%s",Txt_User_not_found_or_you_do_not_have_permission_);
	}
     }
   else	// I can not confirm
     {
      Gbl.AlertType = Lay_WARNING;
      sprintf (Gbl.Message,"%s",Txt_User_not_found_or_you_do_not_have_permission_);
     }

   /***** Show one or multiple records *****/
   switch (Gbl.Action.Original)
     {
      case ActSeeRecSevGst:
	 /* Show multiple records of guests again (including the updated one) */
	 Rec_ListRecordsGstsShow ();
	 break;
      case ActSeeRecSevStd:
	 /* Show multiple records of students again (including the updated one) */
	 Rec_ListRecordsStdsShow ();
	 break;
      case ActSeeRecSevTch:
	 /* Show multiple records of teachers again (including the updated one) */
	 Rec_ListRecordsTchsShow ();
	 break;
      default:
	 /* Show optional alert */
	 Lay_ShowPendingAlert ();

	 /* Show only the updated record of this user */
	 Rec_ShowSharedUsrRecord (Rec_SHA_RECORD_LIST,
				  &Gbl.Usrs.Other.UsrDat,NULL);
	 break;
     }
  }

/*****************************************************************************/
/*********************** Set a user's ID as confirmed ************************/
/*****************************************************************************/

void ID_ConfirmUsrID (const struct UsrData *UsrDat,const char *UsrID)
  {
   char Query[256 + ID_MAX_BYTES_USR_ID];

   /***** Update database *****/
   sprintf (Query,"UPDATE usr_IDs SET Confirmed='Y'"
                  " WHERE UsrCod=%ld AND UsrID='%s' AND Confirmed<>'Y'",
            UsrDat->UsrCod,UsrID);
   DB_QueryINSERT (Query,"can not confirm a user's ID");
  }
