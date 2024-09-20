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

#include "swad_box.h"
#include "swad_cryptography.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_user.h"
#include "swad_user_clipboard.h"
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

static void UsrClp_ShowClipboard (Rol_Role_t Role);
static void UsrClp_PutIconsClipboard (void *Args);

/*****************************************************************************/
/******************** Copy selected users to clipboard ***********************/
/*****************************************************************************/

void UsrClp_CopyGstsToClipboard (void)
  {
   UsrClp_CopyUsrsToClipboard ();
   UsrClp_ShowClipboardGsts ();
  }

void UsrClp_CopyStdsToClipboard (void)
  {
   UsrClp_CopyUsrsToClipboard ();
   UsrClp_ShowClipboardStds ();
  }

void UsrClp_CopyTchsToClipboard (void)
  {
   UsrClp_CopyUsrsToClipboard ();
   UsrClp_ShowClipboardTchs ();
  }

static void UsrClp_CopyUsrsToClipboard (void)
  {
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
	 /* Add user to clipboard */
	 Usr_DB_CopyToClipboard (UsrDat.UsrCod);
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
  }

/*****************************************************************************/
/**************************** Show user clipboard ****************************/
/*****************************************************************************/

void UsrClp_ShowClipboardGsts (void)
  {
   UsrClp_ShowClipboard (Rol_GST);
   Usr_SeeGuests ();
  }

void UsrClp_ShowClipboardStds (void)
  {
   UsrClp_ShowClipboard (Rol_STD);
   Usr_SeeStudents ();
  }

void UsrClp_ShowClipboardTchs (void)
  {
   UsrClp_ShowClipboard (Rol_TCH);
   Usr_SeeTeachers ();
  }

static void UsrClp_ShowClipboard (Rol_Role_t Role)
  {
   extern const char *Hlp_USERS_Clipboard;
   extern const char *Txt_User_clipboard;

   Box_BoxBegin (Txt_User_clipboard,UsrClp_PutIconsClipboard,&Role,
		 Hlp_USERS_Clipboard,Box_CLOSABLE);
      UsrClp_ListUsrsInMyClipboard ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************* Put contextual icons when showing user clipboard *************/
/*****************************************************************************/

static void UsrClp_PutIconsClipboard (void *Args)
  {
   static Act_Action_t NextActions[Rol_NUM_ROLES] =
     {
      [Rol_UNK	  ] = ActUnk,
      [Rol_GST	  ] = ActRemClpGst,
      [Rol_USR	  ] = ActUnk,
      [Rol_STD	  ] = ActRemClpStd,
      [Rol_NET	  ] = ActUnk,
      [Rol_TCH	  ] = ActRemClpTch,
      [Rol_DEG_ADM] = ActUnk,
      [Rol_CTR_ADM] = ActUnk,
      [Rol_INS_ADM] = ActUnk,
      [Rol_SYS_ADM] = ActUnk,
     };
   static Act_Action_t NextAction;

   /***** Put icon to remove resource clipboard in program *****/
   if (Args)
      if (Usr_DB_GetNumUsrsInMyClipboard ())	// Only if there are users
        {
	 NextAction = NextActions[*((Rol_Role_t *) Args)];
	 if (NextAction != ActUnk)
	    Ico_PutContextualIconToRemove (NextAction,NULL,
					   NULL,NULL);
        }
  }

/*****************************************************************************/
/************************* Show users in my clipboard ************************/
/*****************************************************************************/

void UsrClp_ListUsrsInMyClipboard (void)
  {
   extern const char *Usr_NameSelUnsel[Rol_NUM_ROLES];
   extern const char *Usr_ParUsrCod[Rol_NUM_ROLES];
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC12x16",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE12x16",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO12x16",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR12x16",
     };
   MYSQL_RES *mysql_res;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct Usr_Data UsrDat;

   /***** Get and show users in clipboard *****/
   NumUsrs = Usr_DB_GetUsrsInMyClipboard (&mysql_res);

   /***** Checkbox to selected/unselect all users *****/
   HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
      HTM_INPUT_CHECKBOX (Usr_NameSelUnsel[Rol_UNK],
			  NumUsrs ? HTM_NO_ATTR :
				    HTM_DISABLED,
			  "value=\"\""
			  " onclick=\"togglecheckChildren(this,'%s')\"",
			  Usr_ParUsrCod[Rol_UNK]);

      HTM_TxtF ("%u usuarios",NumUsrs);	// TODO: Need translation!!!!!
   HTM_LABEL_End ();

   /***** List users in clipboard *****/
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

		  /* Check box to select this user */
		  HTM_INPUT_CHECKBOX (Usr_ParUsrCod[Rol_UNK],
				      HTM_NO_ATTR,
				      "value=\"%s\" onclick=\"checkParent(this,'%s')\"",
				      UsrDat.EnUsrCod,Usr_NameSelUnsel[Rol_UNK]);

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

/*****************************************************************************/
/********************* Put icon to view user clipboard ***********************/
/*****************************************************************************/

void UsrClp_PutIconToViewClipboardGsts (void)
  {
   Ico_PutContextualIconToViewClipboard (ActSeeCliGst,NULL,NULL,NULL);
  }

void UsrClp_PutIconToViewClipboardStds (void)
  {
   Ico_PutContextualIconToViewClipboard (ActSeeCliStd,NULL,NULL,NULL);
  }

void UsrClp_PutIconToViewClipboardTchs (void)
  {
   Ico_PutContextualIconToViewClipboard (ActSeeCliTch,NULL,NULL,NULL);
  }

/*****************************************************************************/
/************************** Remove user clipboard ****************************/
/*****************************************************************************/

void UsrClp_RemoveClipboardGsts (void)
  {
   Usr_DB_RemoveMyClipboard ();
   UsrClp_ShowClipboardGsts ();
  }

void UsrClp_RemoveClipboardStds (void)
  {
   Usr_DB_RemoveMyClipboard ();
   UsrClp_ShowClipboardStds ();
  }

void UsrClp_RemoveClipboardTchs (void)
  {
   Usr_DB_RemoveMyClipboard ();
   UsrClp_ShowClipboardTchs ();
  }
