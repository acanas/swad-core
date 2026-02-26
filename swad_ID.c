// swad_ID.c: Users' IDs

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <ctype.h>		// For isalnum, isdigit, etc.
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For exit, system, malloc, free, rand, etc.
#include <string.h>		// For string functions

#include "swad_account.h"
#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_ID_database.h"
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

static const char *ID_Class[ID_NUM_CONFIRMED] =
  {
   [ID_NOT_CONFIRMED] = "USR_ID_NC",
   [ID_CONFIRMED    ] = "USR_ID_C",
  };

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

const char *ID_ID_SECTION_ID = "id_section";

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static Err_SuccessOrError_t ID_CheckIfUsrIDIsValidUsingMinDigits (const char *UsrID,
								  unsigned MinDigits);

static void ID_PutLinkToConfirmID (const struct Usr_Data *UsrDat,unsigned NumID,
                                   const char *Anchor);

static void ID_ShowFormChangeUsrID (Usr_MeOrOther_t MeOrOther);

static void ID_PutParsRemoveMyID (void *ID);
static void ID_PutParsRemoveOtherID (void *ID);

static void ID_RemoveUsrID (const struct Usr_Data *UsrDat,Usr_MeOrOther_t MeOrOther);
static void ID_ChangeUsrID (const struct Usr_Data *UsrDat,Usr_MeOrOther_t MeOrOther);

/*****************************************************************************/
/********************** Get list of IDs of a user ****************************/
/*****************************************************************************/

void ID_GetListIDsFromUsrCod (struct Usr_Data *UsrDat)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumIDs;
   unsigned NumID;

   /***** Initialize list of IDs to an empty list *****/
   ID_FreeListIDs (UsrDat);

   if (UsrDat->UsrCod > 0)
     {
      /***** Get user's IDs from database *****/
      // First the confirmed  (Confirmed == 'Y')
      // Then the unconfirmed (Confirmed == 'N')
      if ((NumIDs = ID_DB_GetIDsFromUsrCod (&mysql_res,UsrDat->UsrCod)))
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
                      sizeof (UsrDat->IDs.List[NumID].ID) - 1);

            /* Get if ID is confirmed from row[1] */
            UsrDat->IDs.List[NumID].Confirmed = row[1][0] == 'Y' ? ID_CONFIRMED :
        							   ID_NOT_CONFIRMED;
	   }
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/***************** Free memory allocated for list of IDs *********************/
/*****************************************************************************/

void ID_ReallocateListIDs (struct Usr_Data *UsrDat,unsigned NumIDs)
  {
   unsigned NumID;

   /***** Free list of IDs if used *****/
   ID_FreeListIDs (UsrDat);

   /***** Assign number of IDs in list *****/
   UsrDat->IDs.Num = NumIDs;

   /***** Allocate space for the list *****/
   if ((UsrDat->IDs.List = malloc (NumIDs * sizeof (*UsrDat->IDs.List))) == NULL)
      Err_NotEnoughMemoryExit ();

   for (NumID = 0;
	NumID < NumIDs;
	NumID++)
     {
      UsrDat->IDs.List[NumID].Confirmed = ID_NOT_CONFIRMED;
      UsrDat->IDs.List[NumID].ID[0] = '\0';
     }
  }

/*****************************************************************************/
/***************** Free memory allocated for list of IDs *********************/
/*****************************************************************************/

void ID_FreeListIDs (struct Usr_Data *UsrDat)
  {
   /***** Free list *****/
   if (UsrDat->IDs.Num && UsrDat->IDs.List)
      free (UsrDat->IDs.List);

   /***** Reset list *****/
   UsrDat->IDs.List = NULL;
   UsrDat->IDs.Num = 0;
  }

/*****************************************************************************/
/***************** Get list of user codes from user's IDs ********************/
/*****************************************************************************/
// Returns the number of users with any of these IDs
// The list of users' codes is allocated inside this function and should be freed by caller

unsigned ID_GetListUsrCodsFromUsrID (struct Usr_Data *UsrDat,
                                     const char *EncryptedPassword,	// If NULL or empty ==> do not check password
                                     struct Usr_ListUsrCods *ListUsrCods,
                                     ID_OnlyConfirmed_t OnlyConfirmedIDs)
  {
   MYSQL_RES *mysql_res;
   unsigned NumUsr;

   if (UsrDat->IDs.Num)
     {
      if ((ListUsrCods->NumUsrs = ID_DB_GetUsrCodsFromUsrID (&mysql_res,
                                                             UsrDat,
                                                             EncryptedPassword,
                                                             OnlyConfirmedIDs)))
        {
	 /***** Allocate space for the list of users' codes *****/
	 Usr_AllocateListUsrCods (ListUsrCods);
	 // The list should be freed by caller

	 /***** Fill the list *****/
	 for (NumUsr = 0;
	      NumUsr < ListUsrCods->NumUsrs;
	      NumUsr++)
	    /* Get user's code */
	    ListUsrCods->Lst[NumUsr] = DB_GetNextCode (mysql_res);
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

void ID_PutParOtherUsrIDPlain (void)
  {
   Par_PutParString (NULL,"OtherUsrID",
	             Gbl.Usrs.Other.UsrDat.IDs.Num &&
	             Gbl.Usrs.Other.UsrDat.IDs.List ? Gbl.Usrs.Other.UsrDat.IDs.List[0].ID :
	                                              "");
  }

/*****************************************************************************/
/********* Get parameter plain user's ID of other user from a form ***********/
/*****************************************************************************/

void ID_GetParOtherUsrIDPlain (void)
  {
   /***** Allocate space for the list *****/
   ID_ReallocateListIDs (&Gbl.Usrs.Other.UsrDat,1);

   /***** Get parameter *****/
   Par_GetParText ("OtherUsrID",Gbl.Usrs.Other.UsrDat.IDs.List[0].ID,
                     ID_MAX_BYTES_USR_ID);
   // Users' IDs are always stored internally in capitals and without leading zeros
   Str_RemoveLeadingZeros (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID);
   Str_ConvertToUpperText (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID);

   Gbl.Usrs.Other.UsrDat.IDs.List[0].Confirmed = ID_CONFIRMED;
  }

/*****************************************************************************/
/****** Check whether a user's ID without the ending letter is valid *********/
/*****************************************************************************/
// Returns Err_success if the user's ID string is valid, or Err_ERROR if not
// A valid user's ID must...:
// 1. Must be ID_MIN_BYTES_USR_ID <= characters <= ID_MAX_BYTES_USR_ID.
// 2. All characters must be digits or letters
// 3. Must have a minimum number of digits

// Wrapper function to avoid passing extra parameters
Err_SuccessOrError_t ID_CheckIfUsrIDIsValid (const char *UsrID)
  {
   if (UsrID)
      if (UsrID[0])
         return ID_CheckIfUsrIDIsValidUsingMinDigits (UsrID,ID_MIN_DIGITS_USR_ID);

   return Err_ERROR;
  }

// Wrapper function to avoid passing extra parameters
Err_SuccessOrError_t ID_CheckIfUsrIDSeemsAValidID (const char *UsrID)
  {
   if (UsrID)
      if (UsrID[0])
         return ID_CheckIfUsrIDIsValidUsingMinDigits (UsrID,ID_MIN_DIGITS_AUTOMATIC_DETECT_USR_ID);

   return Err_ERROR;
  }

static Err_SuccessOrError_t ID_CheckIfUsrIDIsValidUsingMinDigits (const char *UsrID,
								  unsigned MinDigits)
  {
   const char *Ptr;
   unsigned NumDigits = 0;
   unsigned Length;

   /***** Check length *****/
   if (!UsrID)
      return Err_ERROR;
   if (!UsrID[0])
      return Err_ERROR;
   Length = strlen (UsrID);
   if (Length < ID_MIN_BYTES_USR_ID ||
       Length > ID_MAX_BYTES_USR_ID)
      return Err_ERROR;					// 1. Must be ID_MIN_BYTES_USR_ID <= characters <= ID_MAX_BYTES_USR_ID

   /**** Loop through user's ID *****/
   for (Ptr = UsrID;
        *Ptr;
        Ptr++)
      if (isdigit ((int) *Ptr))			// If character is digit
         NumDigits++;
      else if (!((*Ptr >= 'A' && *Ptr <= 'Z') ||
                 (*Ptr >= 'a' && *Ptr <= 'z')))	// If character is not alpha
         return Err_ERROR;				// 2. All characters must be digits or letters

   return NumDigits >= MinDigits ? Err_SUCCESS :	// 3. Must have MinDigits digits at least
				   Err_ERROR;
  }

/*****************************************************************************/
/*************************** Write list of user's ID *************************/
/*****************************************************************************/

void ID_WriteUsrIDs (const struct Usr_Data *UsrDat,const char *Anchor)
  {
   unsigned NumID;
   Usr_Can_t ICanSeeUsrID = ID_ICanSeeOtherUsrIDs (UsrDat);
   Usr_Can_t ICanConfirmUsrID = ICanSeeUsrID == Usr_CAN &&
				Usr_ItsMe (UsrDat->UsrCod) == Usr_OTHER &&			// Not me
				Frm_CheckIfInside () == Frm_OUTSIDE_FORM &&			// Not inside another form
				Act_GetBrowserTab (Gbl.Action.Act) == Act_1ST ? Usr_CAN :	// Only in main browser tab
										Usr_CAN_NOT;
   for (NumID = 0;
	NumID < UsrDat->IDs.Num;
	NumID++)
     {
      /* If not the first ID ==> new line */
      if (NumID)
	 HTM_BR ();

      /* Write this ID */
      HTM_SPAN_Begin ("class=\"%s_%s\"",
	              ID_Class[UsrDat->IDs.List[NumID].Confirmed],
	              The_GetSuffix ());
	 switch (ICanSeeUsrID)
	   {
	    case Usr_CAN:
	       HTM_Txt (UsrDat->IDs.List[NumID].ID);
	       break;
	    case Usr_CAN_NOT:
	    default:
	       HTM_Txt ("********");
	       break;
	   }
      HTM_SPAN_End ();

      /* Put link to confirm ID? */
      if (ICanConfirmUsrID == Usr_CAN &&
	  UsrDat->IDs.List[NumID].Confirmed == ID_NOT_CONFIRMED)
	 ID_PutLinkToConfirmID (UsrDat,NumID,Anchor);
     }
  }

/*****************************************************************************/
/***************** Check if I can see another user's IDs *********************/
/*****************************************************************************/

Usr_Can_t ID_ICanSeeOtherUsrIDs (const struct Usr_Data *UsrDat)
  {
   /***** Fast check: It's me? *****/
   if (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)
      return Usr_CAN;

   /***** Check if I have permission to see another user's IDs *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
      case Rol_TCH:
	 /* Check 1: I can see the IDs of users who do not exist in database */
         if (UsrDat->UsrCod <= 0)	// User does not exist (when creating a new user)
            return Usr_CAN;

	 /* Check 2: I can see the IDs of confirmed students */
         if (UsrDat->Roles.InCurrentCrs == Rol_STD &&	// A student...
	     UsrDat->Accepted == Usr_HAS_ACCEPTED)	// ...who accepted registration
            return Usr_CAN;

         /* Check 3: I can see the IDs of users with user's data empty */
         // This check is made to not view simultaneously:
         // - an ID
         // - a name or an email
         if (!UsrDat->Password[0] &&	// User has no password (never logged)
	     !UsrDat->Surname1[0] &&	// and who has no surname 1 (nobody filled user's surname 1)
	     !UsrDat->Surname2[0] &&	// and who has no surname 2 (nobody filled user's surname 2)
	     !UsrDat->FrstName[0] &&	// and who has no first name (nobody filled user's first name)
             !UsrDat->Email[0])		// and who has no email (nobody filled user's email)
            return Usr_CAN;

         return Usr_CAN_NOT;
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
         return Usr_CheckIfICanEditOtherUsr (UsrDat);
      default:
	 return Usr_CAN_NOT;
     }
  }

/*****************************************************************************/
/****************** Put a link to confirm another user's ID ******************/
/*****************************************************************************/

static void ID_PutLinkToConfirmID (const struct Usr_Data *UsrDat,unsigned NumID,
                                   const char *Anchor)
  {
   extern const char *Txt_Confirm;
   static Act_Action_t NextAction[Rol_NUM_ROLES] =
     {
      [Rol_UNK	  ] = ActCnfID_Oth,
      [Rol_GST	  ] = ActCnfID_Oth,
      [Rol_USR    ] = ActCnfID_Oth,
      [Rol_STD	  ] = ActCnfID_Std,
      [Rol_NET	  ] = ActCnfID_Tch,
      [Rol_TCH	  ] = ActCnfID_Tch,
      [Rol_DEG_ADM] = ActCnfID_Oth,
      [Rol_CTR_ADM] = ActCnfID_Oth,
      [Rol_INS_ADM] = ActCnfID_Oth,
      [Rol_SYS_ADM] = ActCnfID_Oth,
     };

   /***** Begin form *****/
   Frm_BeginFormAnchor (NextAction[UsrDat->Roles.InCurrentCrs],Anchor);
      if (Gbl.Action.Original != ActUnk)
	{
	 ParCod_PutPar (ParCod_OrgAct,Act_GetActCod (Gbl.Action.Original));	// Original action, used to know where we came from
	 switch (Gbl.Action.Original)
	   {
	    case ActSeeRecSevGst:
	    case ActSeeRecSevStd:
	    case ActSeeRecSevTch:
	       Usr_PutParSelectedUsrsCods (&Gbl.Usrs.Selected);
	       break;
	    default:
	       break;
	   }
	}
      Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
      Par_PutParString (NULL,"UsrID",UsrDat->IDs.List[NumID].ID);

      /***** Put link *****/
      HTM_BUTTON_Submit_Begin (Txt_Confirm,NULL,
                               "class=\"BT_LINK FORM_OUT_%s BOLD\"",
                               The_GetSuffix ());
	 Ico_PutIconTextLink ("check.svg",Ico_BLACK,Txt_Confirm);
      HTM_BUTTON_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/*********************** Show form to change my user's ID ********************/
/*****************************************************************************/

void ID_ShowFormChangeMyID (void)
  {
   extern const char *Hlp_PROFILE_Account;
   extern const char *Txt_ID_identity_number;

   /***** Begin section *****/
   HTM_SECTION_Begin (ID_ID_SECTION_ID);

      /***** Begin box *****/
      Box_BoxBegin (Txt_ID_identity_number,Acc_PutLinkToRemoveMyAccount,NULL,
		    Hlp_PROFILE_Account,Box_NOT_CLOSABLE);

	 /***** Show form to change ID *****/
	 ID_ShowFormChangeUsrID (Usr_ME);

      /***** End box *****/
      Box_BoxEnd ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/*********************** Show form to change my user's ID ********************/
/*****************************************************************************/

void ID_ShowFormChangeOtherUsrID (void)
  {
   extern const char *Hlp_PROFILE_Account;
   extern const char *Txt_ID_identity_number;

   /***** Begin section *****/
   HTM_SECTION_Begin (ID_ID_SECTION_ID);

      /***** Begin box *****/
      Box_BoxBegin (Txt_ID_identity_number,NULL,NULL,
		    Hlp_PROFILE_Account,Box_NOT_CLOSABLE);

	 /***** Show form to change ID *****/
	 ID_ShowFormChangeUsrID (Usr_OTHER);

      /***** End box *****/
      Box_BoxEnd ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/*********************** Show form to change my user's ID ********************/
/*****************************************************************************/

static void ID_ShowFormChangeUsrID (Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Hlp_PROFILE_Account;
   extern const char *Txt_Please_fill_in_your_ID;
   extern const char *Txt_ID_X_confirmed;
   extern const char *Txt_ID_X_not_confirmed;
   extern const char *Txt_ID_identity_number;
   extern const char *Txt_Another_ID;
   extern const char *Txt_The_ID_is_used_in_order_to_facilitate_;
   extern struct Usr_Data *Usr_UsrDat[Usr_NUM_ME_OR_OTHER];
   unsigned NumID;
   char *Title;
   static struct
     {
      Act_Action_t Remove;
      Act_Action_t Change;
     } NextAction[Rol_NUM_ROLES] =
     {
      [Rol_UNK	  ] = {ActRemID_Oth,ActChgID_Oth},
      [Rol_GST	  ] = {ActRemID_Oth,ActChgID_Oth},
      [Rol_USR    ] = {ActRemID_Oth,ActChgID_Oth},
      [Rol_STD	  ] = {ActRemID_Std,ActChgID_Std},
      [Rol_NET	  ] = {ActRemID_Tch,ActChgID_Tch},
      [Rol_TCH	  ] = {ActRemID_Tch,ActChgID_Tch},
      [Rol_DEG_ADM] = {ActRemID_Oth,ActChgID_Oth},
      [Rol_CTR_ADM] = {ActRemID_Oth,ActChgID_Oth},
      [Rol_INS_ADM] = {ActRemID_Oth,ActChgID_Oth},
      [Rol_SYS_ADM] = {ActRemID_Oth,ActChgID_Oth},
     };
   static void (*FuncParsRemove[Usr_NUM_ME_OR_OTHER]) (void *ID) =
     {
      [Usr_ME   ] = ID_PutParsRemoveMyID,
      [Usr_OTHER] = ID_PutParsRemoveOtherID
     };
   struct
     {
      Act_Action_t Remove;
      Act_Action_t Change;
     } ActID[Rol_NUM_ROLES] =
     {
      [Usr_ME   ] = {.Remove = ActRemMyID,
	             .Change = ActChgMyID},
      [Usr_OTHER] = {.Remove = NextAction[Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs].Remove,
	             .Change = NextAction[Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs].Change}
     };
    static const char **TitleFmt[ID_NUM_CONFIRMED] =
     {
      [ID_NOT_CONFIRMED] = &Txt_ID_X_not_confirmed,
      [ID_CONFIRMED    ] = &Txt_ID_X_confirmed,
     };

   /***** Show possible alerts *****/
   Ale_ShowAlerts (ID_ID_SECTION_ID);

   /***** Help message *****/
   if (MeOrOther == Usr_ME && Gbl.Usrs.Me.UsrDat.IDs.Num == 0)
      Ale_ShowAlert (Ale_WARNING,Txt_Please_fill_in_your_ID);

   /***** Begin table *****/
   HTM_TABLE_BeginCenterPadding (2);

      /***** List existing user's IDs *****/
      for (NumID = 0;
	   NumID < Usr_UsrDat[MeOrOther]->IDs.Num;
	   NumID++)
	{
	 if (NumID == 0)
	   {
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_ID_identity_number);

	       /* Data */
	       HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
	   }
	 else	// NumID >= 1
	    HTM_BR ();

	 if (Usr_UsrDat[MeOrOther]->IDs.Num > 1)	// I have two or more IDs
	   {
	    if (MeOrOther == Usr_ME &&
		Usr_UsrDat[MeOrOther]->IDs.List[NumID].Confirmed == ID_CONFIRMED)	// I can not remove my confirmed IDs
	       /* Put disabled icon to remove user's ID */
	       Ico_PutIconRemovalNotAllowed ();
	    else						// I can remove
	       /* Form to remove user's ID */
	       Ico_PutContextualIconToRemove (ActID[MeOrOther].Remove,ID_ID_SECTION_ID,
					      FuncParsRemove[MeOrOther],
					      Usr_UsrDat[MeOrOther]->IDs.List[NumID].ID);
	   }

	    /* User's ID */
	    if (asprintf (&Title,
		          *TitleFmt[Usr_UsrDat[MeOrOther]->IDs.List[NumID].Confirmed],
			  Usr_UsrDat[MeOrOther]->IDs.List[NumID].ID) < 0)
	       Err_NotEnoughMemoryExit ();
	    HTM_SPAN_Begin ("class=\"%s\" title=\"%s\"",
			    ID_Class[Usr_UsrDat[MeOrOther]->IDs.List[NumID].Confirmed],
			    Title);
	    free (Title);
	       HTM_Txt (Usr_UsrDat[MeOrOther]->IDs.List[NumID].ID);
	       if (Usr_UsrDat[MeOrOther]->IDs.List[NumID].Confirmed == ID_CONFIRMED)
	          HTM_Txt ("&check;");
	    HTM_SPAN_End ();

	 if (NumID == Usr_UsrDat[MeOrOther]->IDs.Num - 1)
	   {
	       HTM_TD_End ();
	    HTM_TR_End ();
	   }
	}

      if (Usr_UsrDat[MeOrOther]->IDs.Num < ID_MAX_IDS_PER_USER)
	{
	 /***** Write help text *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("colspan=\"2\" class=\"CM DAT_%s\"",The_GetSuffix ());
	       Ale_ShowAlert (Ale_INFO,Txt_The_ID_is_used_in_order_to_facilitate_);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Form to enter new user's ID *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT","NewID",
			     Usr_UsrDat[MeOrOther]->IDs.Num ? Txt_Another_ID :		// A new user's ID
					                      Txt_ID_identity_number);	// The first user's ID

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LT DAT_%s\"",The_GetSuffix ());
	       Frm_BeginFormAnchor (ActID[MeOrOther].Change,ID_ID_SECTION_ID);
		  if (MeOrOther == Usr_OTHER)
		     Usr_PutParUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
		  HTM_INPUT_TEXT ("NewID",ID_MAX_BYTES_USR_ID,
				  Usr_UsrDat[MeOrOther]->IDs.Num ? Usr_UsrDat[MeOrOther]->IDs.List[Usr_UsrDat[MeOrOther]->IDs.Num - 1].ID :
						                   "",	// Show the most recent ID
				  HTM_NO_ATTR,
				  "id=\"NewID\" class=\"Frm_C2_INPUT INPUT_%s\""
				  " size=\"16\"",The_GetSuffix ());
		  HTM_BR ();
		  Btn_PutButtonInline (Btn_CREATE);
	       Frm_EndForm ();
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

static void ID_PutParsRemoveMyID (void *ID)
  {
   if (ID)
      Par_PutParString (NULL,"UsrID",(char *) ID);
  }

static void ID_PutParsRemoveOtherID (void *ID)
  {
   if (ID)
     {
      Usr_PutParUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
      Par_PutParString (NULL,"UsrID",(char *) ID);
     }
  }

/*****************************************************************************/
/********************** Remove one of my user's IDs **************************/
/*****************************************************************************/

void ID_RemoveMyUsrID (void)
  {
   /***** Remove user's ID *****/
   ID_RemoveUsrID (&Gbl.Usrs.Me.UsrDat,Usr_ME);

   /***** Update list of IDs *****/
   ID_GetListIDsFromUsrCod (&Gbl.Usrs.Me.UsrDat);

   /***** Show my account again *****/
   Acc_ShowFormChgMyAccount ();
  }

/*****************************************************************************/
/**************** Remove one of the user's IDs of another user ***************/
/*****************************************************************************/

void ID_RemoveOtherUsrID (void)
  {
   /***** Get other user's code from form and get user's data *****/
   switch (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
     {
      case Exi_EXISTS:
	 switch (Usr_CheckIfICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	   {
	    case Usr_CAN:
	       /***** Remove user's ID *****/
	       ID_RemoveUsrID (&Gbl.Usrs.Other.UsrDat,
			       Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod));

	       /***** Update list of IDs *****/
	       ID_GetListIDsFromUsrCod (&Gbl.Usrs.Other.UsrDat);

	       /***** Show form again *****/
	       Acc_ShowFormChgOtherUsrAccount ();
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
/***************************** Remove user's ID ******************************/
/*****************************************************************************/

static void ID_RemoveUsrID (const struct Usr_Data *UsrDat,Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_ID_X_removed;
   extern const char *Txt_You_can_not_delete_this_ID;
   char UsrID[ID_MAX_BYTES_USR_ID + 1];
   Usr_Can_t ICanRemove = Usr_CAN_NOT;

   switch (Usr_CheckIfICanEditOtherUsr (UsrDat))
     {
      case Usr_CAN:
	 /***** Get user's ID from form *****/
	 Par_GetParText ("UsrID",UsrID,ID_MAX_BYTES_USR_ID);
	 // Users' IDs are always stored internally in capitals and without leading zeros
	 Str_RemoveLeadingZeros (UsrID);
	 Str_ConvertToUpperText (UsrID);

	 if (UsrDat->IDs.Num >= 2)
	    switch (MeOrOther)
	      {
	       case Usr_ME:
		  // I can remove my ID only if it is not confirmed
		  ICanRemove = ID_DB_CheckIfConfirmed (UsrDat->UsrCod,UsrID) ? Usr_CAN_NOT :
									       Usr_CAN;
		  break;
	       case Usr_OTHER:
		  ICanRemove = Usr_CAN;
		  break;
	      }

	 switch (ICanRemove)
	   {
	    case Usr_CAN:
	       /***** Remove one of the user's IDs *****/
	       ID_DB_RemoveUsrID (UsrDat->UsrCod,UsrID);

	       /***** Show message *****/
	       Ale_CreateAlert (Ale_SUCCESS,ID_ID_SECTION_ID,
				Txt_ID_X_removed,UsrID);
	       break;
	    case Usr_CAN_NOT:
	    default:
	       Ale_CreateAlert (Ale_WARNING,ID_ID_SECTION_ID,
				Txt_You_can_not_delete_this_ID);
	       break;
	   }
	 break;
       case Usr_CAN_NOT:
       default:
	 Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	 break;
    }
  }

/*****************************************************************************/
/************************* New user's ID for me ******************************/
/*****************************************************************************/

void ID_NewMyUsrID (void)
  {
   /***** New user's ID *****/
   ID_ChangeUsrID (&Gbl.Usrs.Me.UsrDat,Usr_ME);

   /***** Update list of IDs *****/
   ID_GetListIDsFromUsrCod (&Gbl.Usrs.Me.UsrDat);

   /***** Show my account again *****/
   Acc_ShowFormChgMyAccount ();
  }

/*****************************************************************************/
/************************* Change another user's ID **************************/
/*****************************************************************************/

void ID_ChangeOtherUsrID (void)
  {
   /***** Get other user's code from form and get user's data *****/
   switch (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
     {
      case Exi_EXISTS:
	 switch (Usr_CheckIfICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	   {
	    case Usr_CAN:
	       /***** Change user's ID *****/
	       ID_ChangeUsrID (&Gbl.Usrs.Other.UsrDat,
			       Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod));

	       /***** Update list of IDs *****/
	       ID_GetListIDsFromUsrCod (&Gbl.Usrs.Other.UsrDat);

	       /***** Show form again *****/
	       Acc_ShowFormChgOtherUsrAccount ();
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
/***************************** Change user's ID ******************************/
/*****************************************************************************/

static void ID_ChangeUsrID (const struct Usr_Data *UsrDat,Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_The_ID_X_matches_one_of_the_existing;
   extern const char *Txt_The_ID_X_has_been_confirmed;
   extern const char *Txt_A_user_can_not_have_more_than_X_IDs;
   extern const char *Txt_The_ID_X_has_been_registered_successfully;
   extern const char *Txt_The_ID_X_is_not_valid;
   static ID_Confirmed_t Confirmed[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = ID_NOT_CONFIRMED,	// It's me ==> ID not confirmed
      [Usr_OTHER] = ID_CONFIRMED,	// Not me  ==> ID confirmed
     };
   char NewID[ID_MAX_BYTES_USR_ID + 1];
   unsigned NumID;
   Exi_Exist_t AlreadyExists;
   unsigned NumIDFound = 0;	// Initialized to avoid warning

   switch (Usr_CheckIfICanEditOtherUsr (UsrDat))
     {
      case Usr_CAN:
	 /***** Get new user's ID from form *****/
	 Par_GetParText ("NewID",NewID,ID_MAX_BYTES_USR_ID);
	 // Users' IDs are always stored internally in capitals and without leading zeros
	 Str_RemoveLeadingZeros (NewID);
	 Str_ConvertToUpperText (NewID);

	 switch (ID_CheckIfUsrIDIsValid (NewID))
	   {
	    case Err_SUCCESS:	// If new ID is valid
	       /***** Check if the new ID matches any of the old IDs *****/
	       for (NumID = 0, AlreadyExists = Exi_DOES_NOT_EXIST;
		    NumID < UsrDat->IDs.Num && AlreadyExists == Exi_DOES_NOT_EXIST;
		    NumID++)
		  if (!strcasecmp (UsrDat->IDs.List[NumID].ID,NewID))
		    {
		     AlreadyExists = Exi_EXISTS;
		     NumIDFound = NumID;
		    }

	       switch (AlreadyExists)
		 {
		  case Exi_EXISTS:
		     if (MeOrOther == Usr_ME || UsrDat->IDs.List[NumIDFound].Confirmed == ID_CONFIRMED)
			Ale_CreateAlert (Ale_WARNING,ID_ID_SECTION_ID,
					 Txt_The_ID_X_matches_one_of_the_existing,
					 NewID);
		     else	// It's not me && ID is not confirmed
		       {
			/***** Mark this ID as confirmed *****/
			ID_DB_ConfirmUsrID (UsrDat->UsrCod,NewID);

			Ale_CreateAlert (Ale_SUCCESS,ID_ID_SECTION_ID,
					 Txt_The_ID_X_has_been_confirmed,NewID);
		       }
		     break;
		  case Exi_DOES_NOT_EXIST:
		  default:
		     if (UsrDat->IDs.Num >= ID_MAX_IDS_PER_USER)
			Ale_CreateAlert (Ale_WARNING,ID_ID_SECTION_ID,
					 Txt_A_user_can_not_have_more_than_X_IDs,
					 ID_MAX_IDS_PER_USER);
		     else	// OK ==> add this new ID to my list of IDs
		       {
			/***** Save this new ID *****/
			ID_DB_InsertANewUsrID (UsrDat->UsrCod,NewID,
					       Confirmed[MeOrOther]);

			Ale_CreateAlert (Ale_SUCCESS,ID_ID_SECTION_ID,
					 Txt_The_ID_X_has_been_registered_successfully,
					 NewID);
		       }
		     break;
		 }
	       break;
	    case Err_ERROR:	// New ID is not valid
	    default:
	       Ale_CreateAlert (Ale_WARNING,ID_ID_SECTION_ID,
				Txt_The_ID_X_is_not_valid,NewID);
	       break;
	   }
	 break;
      case Usr_CAN_NOT:
      default:
	 Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	 break;
     }
  }

/*****************************************************************************/
/************************ Confirm another user's ID **************************/
/*****************************************************************************/

void ID_ConfirmOtherUsrID (void)
  {
   extern const char *Txt_ID_X_had_already_been_confirmed;
   extern const char *Txt_The_ID_X_has_been_confirmed;
   char UsrID[ID_MAX_BYTES_USR_ID + 1];
   Usr_Can_t ICanConfirm;
   Exi_Exist_t IDExists;
   unsigned NumID;
   unsigned NumIDFound = 0;	// Initialized to avoid warning

   /***** Get where we came from *****/
   Gbl.Action.Original = Act_GetActionFromActCod (ParCod_GetPar (ParCod_OrgAct));

   /***** Get other user's code from form and get user's data *****/
   ICanConfirm = Usr_CAN_NOT;
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData () == Exi_EXISTS)
      if (Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod) == Usr_OTHER)	// Not me
        {
	 /* If user is a student in current course,
	    check if he/she has accepted */
	 if (Gbl.Hierarchy.HieLvl == Hie_CRS)
	    if (Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs == Rol_STD)
	       Gbl.Usrs.Other.UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);

	 if (ID_ICanSeeOtherUsrIDs (&Gbl.Usrs.Other.UsrDat) == Usr_CAN)
	    ICanConfirm = Usr_CAN;
        }

   switch (ICanConfirm)
     {
      case Usr_CAN:
	 /***** Get user's ID from form *****/
	 Par_GetParText ("UsrID",UsrID,ID_MAX_BYTES_USR_ID);
	 // Users' IDs are always stored internally in capitals and without leading zeros
	 Str_RemoveLeadingZeros (UsrID);
	 Str_ConvertToUpperText (UsrID);

	 for (NumID = 0, IDExists = Exi_DOES_NOT_EXIST;
	      NumID < Gbl.Usrs.Other.UsrDat.IDs.Num && IDExists == Exi_DOES_NOT_EXIST;
	      NumID++)
	    if (!strcasecmp (UsrID,Gbl.Usrs.Other.UsrDat.IDs.List[NumID].ID))
	      {
	       IDExists = Exi_EXISTS;
	       NumIDFound = NumID;
	      }

	 switch (IDExists)
	   {
	    case Exi_EXISTS:
	       switch (Gbl.Usrs.Other.UsrDat.IDs.List[NumIDFound].Confirmed)
		 {
		  case ID_CONFIRMED:
		     /***** ID found and already confirmed *****/
		     Ale_CreateAlert (Ale_INFO,ID_ID_SECTION_ID,
				      Txt_ID_X_had_already_been_confirmed,
				      Gbl.Usrs.Other.UsrDat.IDs.List[NumIDFound].ID);
		     break;
		  case ID_NOT_CONFIRMED:
		  default:
		     /***** Mark this ID as confirmed *****/
		     ID_DB_ConfirmUsrID (Gbl.Usrs.Other.UsrDat.UsrCod,
					 Gbl.Usrs.Other.UsrDat.IDs.List[NumIDFound].ID);
		     Gbl.Usrs.Other.UsrDat.IDs.List[NumIDFound].Confirmed = ID_CONFIRMED;

		     /***** Write success message *****/
		     Ale_CreateAlert (Ale_SUCCESS,ID_ID_SECTION_ID,
				      Txt_The_ID_X_has_been_confirmed,
				      Gbl.Usrs.Other.UsrDat.IDs.List[NumIDFound].ID);
		     break;
		 }
	       break;
	    case Exi_DOES_NOT_EXIST:	// User's ID not found
	    default:
	       Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	       break;
	   }
	 break;
      case Usr_CAN_NOT:
      default:
	 Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	 break;
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
	 Ale_ShowAlerts (NULL);

	 /* Show only the updated record of this user */
	 Rec_ShowSharedUsrRecord (Rec_SHA_RECORD_LIST,
				  &Gbl.Usrs.Other.UsrDat,NULL);
	 break;
     }
  }
