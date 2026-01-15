// swad_admin.c: administrators

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include <stdlib.h>		// For free

#include "swad_action_list.h"
#include "swad_admin.h"
#include "swad_admin_database.h"
#include "swad_alert.h"
#include "swad_global.h"
#include "swad_user_database.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

Hie_Level_t Adm_LevelAdmin[Rol_NUM_ROLES] =
  {
   [Rol_UNK	] = Hie_UNK,
   [Rol_GST	] = Hie_UNK,
   [Rol_USR	] = Hie_UNK,
   [Rol_STD	] = Hie_UNK,
   [Rol_NET	] = Hie_UNK,
   [Rol_TCH	] = Hie_CRS,
   [Rol_DEG_ADM	] = Hie_DEG,
   [Rol_CTR_ADM	] = Hie_CTR,
   [Rol_INS_ADM	] = Hie_INS,
   [Rol_SYS_ADM	] = Hie_SYS,
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Adm_AddAdm (Hie_Level_t HieLvl);
static void Adm_RegisterAdmin (struct Usr_Data *UsrDat,Hie_Level_t HieLvl);

static void Adm_ReqRemOrRemAdm (Enr_ReqDelOrDelUsr_t ReqDelOrDelUsr,
                                Hie_Level_t HieLvl);
static void Adm_AskIfRemAdm (Usr_MeOrOther_t MeOrOther,Hie_Level_t HieLvl);
static void Adm_EffectivelyRemAdm (struct Usr_Data *UsrDat,Hie_Level_t HieLvl);

/*****************************************************************************/
/**************** Check if I can admin another user's account ****************/
/*****************************************************************************/

Usr_Can_t Adm_CheckIfICanAdminOtherUsrs (void)
  {
   return Gbl.Usrs.Me.Role.Logged >= Rol_TCH ? Usr_CAN :
					       Usr_CAN_NOT;
  }

/*****************************************************************************/
/**** Ask if really wanted to add an administrator to current institution ****/
/*****************************************************************************/

void Adm_ReqAddAdm (Hie_Level_t HieLvl)
  {
   extern const char *Txt_THE_USER_X_is_already_an_administrator_of_Y;
   extern const char *Txt_Do_you_really_want_to_enrol_the_following_user_as_an_administrator_of_X;
   static Act_Action_t Actions[Hie_NUM_LEVELS] =
     {
      [Hie_UNK] = ActUnk,
      [Hie_SYS] = ActUnk,
      [Hie_CTY] = ActUnk,
      [Hie_INS] = ActNewAdmIns,
      [Hie_CTR] = ActNewAdmCtr,
      [Hie_DEG] = ActNewAdmDeg,
      [Hie_CRS] = ActUnk,
     };
   Usr_Can_t ICanRegister;

   if (Gbl.Hierarchy.Node[HieLvl].HieCod > 0)
      /***** Get user's identificator of the user to register as admin *****/
      switch (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
	{
	 case Exi_EXISTS:
	    /* Check if I am allowed to register user as administrator in institution/center/degree */
	    ICanRegister = (HieLvl == Hie_DEG && Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM) ||
			   (HieLvl == Hie_CTR && Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM) ||
			   (HieLvl == Hie_INS && Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM) ? Usr_CAN :
											   Usr_CAN_NOT;
	    switch (ICanRegister)
	      {
	       case Usr_CAN:
		  switch (Adm_DB_CheckIfUsrExistsAsAdm (Gbl.Usrs.Other.UsrDat.UsrCod,HieLvl))
		    {
		     case Exi_EXISTS:	// User is already an administrator of current institution/center/degree
			Ale_ShowAlert (Ale_INFO,Txt_THE_USER_X_is_already_an_administrator_of_Y,
				       Gbl.Usrs.Other.UsrDat.FullName,
				       Gbl.Hierarchy.Node[HieLvl].FullName);
			Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);
			break;
		     case Exi_DOES_NOT_EXIST:
		     default:
			/***** Show question and button to register user as administrator *****/
			/* Begin alert */
			Ale_ShowAlertAndButtonBegin (Ale_QUESTION,Txt_Do_you_really_want_to_enrol_the_following_user_as_an_administrator_of_X,
						     Gbl.Hierarchy.Node[HieLvl].FullName);

			/* Show user's record */
			Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

			/* End alert */
			Ale_ShowAlertAndButtonEnd (Actions[HieLvl],NULL,NULL,
						   Usr_PutParOtherUsrCodEncrypted,Gbl.Usrs.Other.UsrDat.EnUsrCod,
						   Btn_ENROL);
			break;
		    }
		  break;
	       case Usr_CAN_NOT:
	       default:
		  Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
		  break;
	      }
	    break;
	 case Exi_DOES_NOT_EXIST:
	 default:
	    Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	}
  }

/*****************************************************************************/
/*************** Add an administrator to current institution *****************/
/*****************************************************************************/

void Adm_AddAdmToIns (void)
  {
   Adm_AddAdm (Hie_INS);
  }

/*****************************************************************************/
/******************* Add an administrator to current center ******************/
/*****************************************************************************/

void Adm_AddAdmToCtr (void)
  {
   Adm_AddAdm (Hie_CTR);
  }

/*****************************************************************************/
/******************* Add an administrator to current degree ******************/
/*****************************************************************************/

void Adm_AddAdmToDeg (void)
  {
   Adm_AddAdm (Hie_DEG);
  }

/*****************************************************************************/
/******************** Get list with data of administrators *******************/
/*****************************************************************************/

void Adm_GetAdmsLst (Hie_Level_t HieLvl)
  {
   char *Query = NULL;

   /***** Build query *****/
   Usr_DB_BuildQueryToGetAdmsLst (HieLvl,&Query);

   /***** Get list of administrators from database *****/
   Usr_GetListUsrsFromQuery (Query,HieLvl,Rol_DEG_ADM);

   /***** Free query string *****/
   free (Query);
  }

/*****************************************************************************/
/******************* Add an administrator in a given level *******************/
/*****************************************************************************/

static void Adm_AddAdm (Hie_Level_t HieLvl)
  {
   Usr_Can_t ICanRegister;

   if (Gbl.Hierarchy.Node[HieLvl].HieCod > 0)
      /***** Get plain user's ID of the user to add/modify *****/
      switch (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
	{
	 case Exi_EXISTS:
	    /* Check if I am allowed to register user as administrator in institution/center/degree */
	    ICanRegister = (HieLvl == Hie_DEG && Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM) ||
			   (HieLvl == Hie_CTR && Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM) ||
			   (HieLvl == Hie_INS && Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM) ? Usr_CAN :
											   Usr_CAN_NOT;
	    switch (ICanRegister)
	      {
	       case Usr_CAN:
		  /***** Register administrator in current institution/center/degree in database *****/
		  Adm_RegisterAdmin (&Gbl.Usrs.Other.UsrDat,HieLvl);

		  /***** Show user's record *****/
		  Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);
		  break;
	       case Usr_CAN_NOT:
	       default:
		  Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
		  break;
	     }
	    break;
	 case Exi_DOES_NOT_EXIST:
	 default:
	    Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	}
  }

/*****************************************************************************/
/**************** Register administrator in current institution **************/
/*****************************************************************************/

static void Adm_RegisterAdmin (struct Usr_Data *UsrDat,Hie_Level_t HieLvl)
  {
   extern const char *Txt_THE_USER_X_is_already_an_administrator_of_Y;
   extern const char *Txt_THE_USER_X_has_been_enroled_as_administrator_of_Y;

   /***** Check if user was and administrator of current institution/center/degree *****/
   switch (Adm_DB_CheckIfUsrExistsAsAdm (UsrDat->UsrCod,HieLvl))
     {
      case Exi_EXISTS:
	 Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_is_already_an_administrator_of_Y,
			UsrDat->FullName,Gbl.Hierarchy.Node[HieLvl].FullName);
	 break;
      case Exi_DOES_NOT_EXIST:	// User was not administrator of current institution/center/degree
      default:
	 /***** Insert or replace administrator in current institution/center/degree *****/
	 Adm_DB_InsertAdmin (UsrDat->UsrCod,HieLvl);

	 Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_been_enroled_as_administrator_of_Y,
			UsrDat->FullName,Gbl.Hierarchy.Node[HieLvl].FullName);
	 break;
     }
  }

/*****************************************************************************/
/******* Ask for remove of an administrator from current institution *********/
/*****************************************************************************/

void Adm_ReqRemAdmOfIns (void)
  {
   Adm_ReqRemOrRemAdm (Enr_REQUEST_REMOVE_USR,Hie_INS);
  }

/*****************************************************************************/
/********* Ask for remove of an administrator from current center ************/
/*****************************************************************************/

void Adm_ReqRemAdmOfCtr (void)
  {
   Adm_ReqRemOrRemAdm (Enr_REQUEST_REMOVE_USR,Hie_CTR);
  }

/*****************************************************************************/
/********* Ask for remove of an administrator from current degree ************/
/*****************************************************************************/

void Adm_ReqRemAdmOfDeg (void)
  {
   Adm_ReqRemOrRemAdm (Enr_REQUEST_REMOVE_USR,Hie_DEG);
  }

/*****************************************************************************/
/************ Remove an administrator from current institution ***************/
/*****************************************************************************/

void Adm_RemAdmIns (void)
  {
   Adm_ReqRemOrRemAdm (Enr_REMOVE_USR,Hie_INS);
  }

/*****************************************************************************/
/*************** Remove an administrator from current center *****************/
/*****************************************************************************/

void Adm_RemAdmCtr (void)
  {
   Adm_ReqRemOrRemAdm (Enr_REMOVE_USR,Hie_CTR);
  }

/*****************************************************************************/
/*************** Remove an administrator from current degree *****************/
/*****************************************************************************/

void Adm_RemAdmDeg (void)
  {
   Adm_ReqRemOrRemAdm (Enr_REMOVE_USR,Hie_DEG);
  }

/*****************************************************************************/
/***************** Remove an admin from current institution ******************/
/*****************************************************************************/

static void Adm_ReqRemOrRemAdm (Enr_ReqDelOrDelUsr_t ReqDelOrDelUsr,
                                Hie_Level_t HieLvl)
  {
   extern const char *Txt_THE_USER_X_is_not_an_administrator_of_Y;
   Usr_MeOrOther_t MeOrOther;
   Usr_Can_t ICanRemove;

   if (Gbl.Hierarchy.Node[HieLvl].HieCod > 0)
      /***** Get user to be removed *****/
      switch (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
	{
	 case Exi_EXISTS:
	    /* Check if it's forbidden to remove an administrator */
	    MeOrOther = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);
	    ICanRemove = MeOrOther == Usr_ME ||
			 (HieLvl == Hie_DEG && Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM) ||
			 (HieLvl == Hie_CTR && Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM) ||
			 (HieLvl == Hie_INS && Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM) ? Usr_CAN :
											 Usr_CAN_NOT;
	    switch (ICanRemove)
	      {
	       case Usr_CAN:
		  /* Check if the other user is an admin of the current institution/center/degree */
		  switch (Adm_DB_CheckIfUsrExistsAsAdm (Gbl.Usrs.Other.UsrDat.UsrCod,HieLvl))
		    {
		     case Exi_EXISTS:		// The other user is an administrator
						// of current institution/center/degree
						// ==> ask for removing or remove her/him
			switch (ReqDelOrDelUsr)
			  {
			   case Enr_REQUEST_REMOVE_USR:     // Ask if remove administrator from current institution
			      Adm_AskIfRemAdm (MeOrOther,HieLvl);
			      break;
			   case Enr_REMOVE_USR:             // Remove administrator from current institution
			      Adm_EffectivelyRemAdm (&Gbl.Usrs.Other.UsrDat,HieLvl);
			      break;
			  }
			break;
		     case Exi_DOES_NOT_EXIST:	// The other user is not an administrator
		     default:			// of current institution
			Ale_ShowAlert (Ale_WARNING,Txt_THE_USER_X_is_not_an_administrator_of_Y,
				       Gbl.Usrs.Other.UsrDat.FullName,
				       Gbl.Hierarchy.Node[HieLvl].FullName);
			break;
		    }
		  break;
	       case Usr_CAN_NOT:
	       default:
		  Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
		  break;
	      }
	    break;
	 case Exi_DOES_NOT_EXIST:
	 default:
	    Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	}
  }

/*****************************************************************************/
/** Ask if really wanted to remove an administrator from current institution */
/*****************************************************************************/

static void Adm_AskIfRemAdm (Usr_MeOrOther_t MeOrOther,Hie_Level_t HieLvl)
  {
   extern const char *Txt_Do_you_really_want_to_be_removed_as_an_administrator_of_X;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_user_as_an_administrator_of_X;
   static Act_Action_t ActRemAdm[Hie_NUM_LEVELS] =
     {
      [Hie_UNK] = ActUnk,
      [Hie_SYS] = ActUnk,
      [Hie_CTY] = ActUnk,
      [Hie_INS] = ActRemAdmIns,
      [Hie_CTR] = ActRemAdmCtr,
      [Hie_DEG] = ActRemAdmDeg,
      [Hie_CRS] = ActUnk,
     };
   const char *Question[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_Do_you_really_want_to_be_removed_as_an_administrator_of_X,
      [Usr_OTHER] = Txt_Do_you_really_want_to_remove_the_following_user_as_an_administrator_of_X,
     };

   switch (Usr_DB_ChkIfUsrCodExists (Gbl.Usrs.Other.UsrDat.UsrCod))
     {
      case Exi_EXISTS:
	 /***** Show question and button to remove user as administrator *****/
	 /* Begin alert */
	 Ale_ShowAlertAndButtonBegin (Ale_QUESTION,Question[MeOrOther],
				      Gbl.Hierarchy.Node[HieLvl].FullName);

	 /* Show user's record */
	 Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

	 /* End alert */
	 Ale_ShowAlertAndButtonEnd (ActRemAdm[HieLvl],NULL,NULL,
				    Usr_PutParOtherUsrCodEncrypted,Gbl.Usrs.Other.UsrDat.EnUsrCod,
				    Btn_REMOVE);
	 break;
      case Exi_DOES_NOT_EXIST:
      default:
	 Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	 break;
     }
  }

/*****************************************************************************/
/**** Remove an administrator from current institution, center or degree *****/
/*****************************************************************************/

static void Adm_EffectivelyRemAdm (struct Usr_Data *UsrDat,Hie_Level_t HieLvl)
  {
   extern const char *Txt_THE_USER_X_has_been_removed_as_administrator_of_Y;
   extern const char *Txt_THE_USER_X_is_not_an_administrator_of_Y;

   switch (Adm_DB_CheckIfUsrExistsAsAdm (UsrDat->UsrCod,HieLvl))
     {
      case Exi_EXISTS:		// User is administrator of current institution/center/degree
	 /***** Remove user as administrator of institution, center or degree *****/
	 Adm_DB_RemAdmin (UsrDat->UsrCod,HieLvl);

	 Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_been_removed_as_administrator_of_Y,
			UsrDat->FullName,Gbl.Hierarchy.Node[HieLvl].FullName);
	 break;
      case Exi_DOES_NOT_EXIST:	// User is not an administrator of the current institution/center/degree
      default:
	 Ale_ShowAlert (Ale_ERROR,Txt_THE_USER_X_is_not_an_administrator_of_Y,
			UsrDat->FullName,Gbl.Hierarchy.Node[HieLvl].FullName);
	 break;
     }
  }
