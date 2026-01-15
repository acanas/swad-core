// swad_user_resource.c: links to users as program resources

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

#include "swad_global.h"
#include "swad_resource_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Get link to teacher ****************************/
/*****************************************************************************/

void UsrRsc_GetLinkToTch (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   /***** Get the selected teacher *****/
   Usr_GetParOtherUsrCodEncryptedAndGetListIDs ();

   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      /***** Copy link to one teacher and show teacher's record *****/
      switch (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,	// Get teacher's data from database
						       Usr_DONT_GET_PREFS,
						       Usr_GET_ROLE_IN_CRS))
	{
	 case Exi_EXISTS:
	    switch (Usr_CheckIfICanViewRecordTch (&Gbl.Usrs.Other.UsrDat))
	      {
	       case Usr_CAN:
		  /* Copy link to teacher into resource clipboard */
		  Rsc_DB_CopyToClipboard (Rsc_TEACHER,Gbl.Usrs.Other.UsrDat.UsrCod);

		  /* Write success message */
		  Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
				 Gbl.Usrs.Other.UsrDat.FullName);

		  /* Show teacher's record */
		  Rec_ShowRecordOneTchCrs ();
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
   else
     {
      /***** Copy link to teachers and show teachers *****/
      /* Copy link to teachers into resource clipboard */
      Rsc_DB_CopyToClipboard (Rsc_TEACHER,-1L);

      /* Write success message */
      Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
		     Txt_ROLES_PLURAL_Abc[Rol_TCH][Usr_SEX_ALL]);

      /* Show teachers */
      Usr_ListTeachers ();
     }
  }
