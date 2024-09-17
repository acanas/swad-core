// swad_user_clipboard.c: user clipboard

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_cryptography.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_user.h"
#include "swad_user_database.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************** Public global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void UsrClp_CopyUsrsToClipboard (void);
static void UsrClp_ListUsrsInMyClipboard (void);

/*****************************************************************************/
/******************** Copy selected users to clipboard ***********************/
/*****************************************************************************/

void UsrClp_CopyGstsToClipboard (void)
  {
   UsrClp_CopyUsrsToClipboard ();
  }

void UsrClp_CopyStdsToClipboard (void)
  {
   UsrClp_CopyUsrsToClipboard ();
  }

void UsrClp_CopyTchsToClipboard (void)
  {
   UsrClp_CopyUsrsToClipboard ();
  }

static void UsrClp_CopyUsrsToClipboard (void)
  {
   unsigned NumUsr = 0;
   const char *Ptr;
   struct Usr_Data UsrDat;

   /***** Remove my clipboard *****/
   Usr_DB_RemoveMyClipboard ();

   /***** Remove old clipboards (from all users) *****/
   Usr_DB_RemoveExpiredClipboards ();   // Someone must do this work. Let's do it whenever a user click in copy

   /***** Get list of selected users if not already got *****/
   Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** List the records *****/
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParMult (&Ptr,UsrDat.EnUsrCod,
                                       Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,	// Get student's data from database
                                                   Usr_DONT_GET_PREFS,
                                                   Usr_GET_ROLE_IN_CRS))
	{
	 /* Check if this user has accepted
	    his/her inscription in the current course */
	 UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&UsrDat);

	 /* Add user to clipboard */
	 Usr_DB_CopyToClipboard (UsrDat.UsrCod);

	 NumUsr++;
	}
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

   /***** Show message *****/
   Ale_ShowAlert (NumUsr ? Ale_SUCCESS :
			   Ale_WARNING,
		  "Usuarios copiados en el portapapeles: %u",NumUsr);

   UsrClp_ListUsrsInMyClipboard ();
  }

/*****************************************************************************/
/************************* Show users in my clipboard ************************/
/*****************************************************************************/

static void UsrClp_ListUsrsInMyClipboard (void)
  {
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC12x16",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE12x16",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO12x16",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR12x16",
     };
   unsigned NumUsrs;
   unsigned NumUsr;
   struct Usr_Data UsrDat;
   MYSQL_RES *mysql_res;

   /***** Get users who have faved/shared *****/
   NumUsrs = Usr_DB_GetUsrsInMyClipboard (&mysql_res);

   if (NumUsrs)
     {
      HTM_DIV_Begin ("class=\"UsrClp_USRS\"");

	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&UsrDat);

	 /***** List users *****/
	 for (NumUsr = 0;
	      NumUsr < NumUsrs;
	      NumUsr++)
	   {
	    /***** Get user's code *****/
	    UsrDat.UsrCod = DB_GetNextCode (mysql_res);

	    /***** Get user's data and show user's photo *****/
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							 Usr_DONT_GET_PREFS,
							 Usr_DONT_GET_ROLE_IN_CRS))
	      {
	       /* Begin container */
	       HTM_DIV_Begin ("class=\"UsrClp_USR\"");

		  /* User's photo */
		  Pho_ShowUsrPhotoIfAllowed (&UsrDat,
					     ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);

	       /* End container */
	       HTM_DIV_End ();
	      }
	   }

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);

      HTM_DIV_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
