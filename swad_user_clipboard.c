// swad_user_clipboard.c: user clipboard

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <stdlib.h>		// For free

#include "swad_box.h"
#include "swad_cryptography.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_user.h"
#include "swad_user_clipboard.h"
#include "swad_user_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void UsrClp_AddUsrToClipboard (void);
static void UsrClp_OverwriteUsrClipboard (void);
static void UsrClp_AddUsrsToClipboard (void);
static void UsrClp_OverwriteUsrsClipboard (void);

static void UsrClp_ShowClipboard (Rol_Role_t Role);
static void UsrClp_PutIconsClipboard (void *Args);

static void UsrClp_GetUsrsLst (void);

static void UsrClp_RemoveClipboard (void);

/*****************************************************************************/
/************************ Add one user to clipboard **************************/
/*****************************************************************************/

void UsrClp_AddOthToClipboard (void)
  {
   UsrClp_AddUsrToClipboard ();
   UsrClp_ShowClipboard (Rol_GST);
  }

void UsrClp_AddStdToClipboard (void)
  {
   UsrClp_AddUsrToClipboard ();
   UsrClp_ShowClipboard (Rol_STD);
  }

void UsrClp_AddTchToClipboard (void)
  {
   UsrClp_AddUsrToClipboard ();
   UsrClp_ShowClipboard (Rol_TCH);
  }

static void UsrClp_AddUsrToClipboard (void)
  {
   /***** Remove old clipboards (from all users) *****/
   Usr_DB_RemoveExpiredClipboards ();   // Someone must do this work. Let's do it whenever a user click in copy

    /***** Get user to be copied *****/
   switch (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
     {
      case Exi_EXISTS:
	 /***** Add user to clipboard *****/
	 Usr_DB_CopyToClipboard (Gbl.Usrs.Other.UsrDat.UsrCod);
	 break;
      case Exi_DOES_NOT_EXIST:
      default:
	 Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	 break;
     }
  }

/*****************************************************************************/
/********************* Overwrite clipboard with one user *********************/
/*****************************************************************************/

void UsrClp_OverwriteOthClipboard (void)
  {
   UsrClp_OverwriteUsrClipboard ();
   UsrClp_ShowClipboard (Rol_GST);
  }

void UsrClp_OverwriteStdClipboard (void)
  {
   UsrClp_OverwriteUsrClipboard ();
   UsrClp_ShowClipboard (Rol_STD);
  }

void UsrClp_OverwriteTchClipboard (void)
  {
   UsrClp_OverwriteUsrClipboard ();
   UsrClp_ShowClipboard (Rol_TCH);
  }

static void UsrClp_OverwriteUsrClipboard (void)
  {
   /***** Remove my clipboard *****/
   Usr_DB_RemoveMyClipboard ();

   /***** Add user to clipboard *****/
   UsrClp_AddUsrToClipboard ();
  }

/*****************************************************************************/
/********************* Add selected users to clipboard ***********************/
/*****************************************************************************/

void UsrClp_AddGstsToClipboard (void)
  {
   UsrClp_AddUsrsToClipboard ();
   UsrClp_ShowClipboardGsts ();
  }

void UsrClp_AddStdsToClipboard (void)
  {
   UsrClp_AddUsrsToClipboard ();
   UsrClp_ShowClipboardStds ();
  }

void UsrClp_AddTchsToClipboard (void)
  {
   UsrClp_AddUsrsToClipboard ();
   UsrClp_ShowClipboardTchs ();
  }

static void UsrClp_AddUsrsToClipboard (void)
  {
   const char *Ptr;
   struct Usr_Data UsrDat;

   /***** Remove old clipboards (from all users) *****/
   Usr_DB_RemoveExpiredClipboards ();   // Someone must do this work. Let's do it whenever a user click in copy

   /***** Get list of selected users if not already got *****/
   Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected,
					  Usr_GET_LIST_ALL_USRS);

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** List the records *****/
   for (Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
        *Ptr;
       )
     {
      Par_GetNextStrUntilSeparParMult (&Ptr,UsrDat.EnUsrCod,
                                       Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,	// Get student's data from database
                                                   Usr_DONT_GET_PREFS,
                                                   Usr_GET_ROLE_IN_CRS) == Exi_EXISTS)
	 /* Add user to clipboard */
	 Usr_DB_CopyToClipboard (UsrDat.UsrCod);
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
  }

/*****************************************************************************/
/********************* Add selected users to clipboard ***********************/
/*****************************************************************************/

void UsrClp_OverwriteGstsClipboard (void)
  {
   UsrClp_OverwriteUsrsClipboard ();
   UsrClp_ShowClipboardGsts ();
  }

void UsrClp_OverwriteStdsClipboard (void)
  {
   UsrClp_OverwriteUsrsClipboard ();
   UsrClp_ShowClipboardStds ();
  }

void UsrClp_OverwriteTchsClipboard (void)
  {
   UsrClp_OverwriteUsrsClipboard ();
   UsrClp_ShowClipboardTchs ();
  }

static void UsrClp_OverwriteUsrsClipboard (void)
  {
   /***** Remove my clipboard *****/
   Usr_DB_RemoveMyClipboard ();

   /***** Add users to clipboard *****/
   UsrClp_AddUsrsToClipboard ();
  }

/*****************************************************************************/
/**************************** Show user clipboard ****************************/
/*****************************************************************************/

void UsrClp_ShowClipboardGsts (void)
  {
   UsrClp_ShowClipboard (Rol_GST);
   Usr_ListGuests ();
  }

void UsrClp_ShowClipboardStds (void)
  {
   UsrClp_ShowClipboard (Rol_STD);
   Usr_ListStudents ();
  }

void UsrClp_ShowClipboardTchs (void)
  {
   UsrClp_ShowClipboard (Rol_TCH);
   Usr_ListTeachers ();
  }

static void UsrClp_ShowClipboard (Rol_Role_t Role)
  {
   extern const char *Hlp_USERS_Clipboard;
   extern const char *Txt_Clipboard;

   Box_BoxBegin (Txt_Clipboard,UsrClp_PutIconsClipboard,&Role,
		 Hlp_USERS_Clipboard,Box_CLOSABLE);
      UsrClp_ListUsrsInMyClipboard (Frm_DONT_PUT_FORM,
				    Lay_SHOW);	// Show even if empty
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************* Put contextual icons when showing user clipboard **************/
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

void UsrClp_ListUsrsInMyClipboard (Frm_PutForm_t PutForm,Lay_Show_t ShowWhenEmpty)
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
   unsigned NumUsr;
   struct Usr_Data UsrDat;

   /***** Get users in clipboard *****/
   UsrClp_GetUsrsLst ();

   /***** Checkbox to selected/unselect all users *****/
   if (ShowWhenEmpty == Lay_SHOW || Gbl.Usrs.LstUsrs[Rol_UNK].NumUsrs)
     {
      if (PutForm == Frm_PUT_FORM && Gbl.Usrs.LstUsrs[Rol_UNK].NumUsrs)
        {
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_INPUT_CHECKBOX (Usr_NameSelUnsel[Rol_UNK],
				Gbl.Usrs.LstUsrs[Rol_UNK].NumUsrs ? HTM_NO_ATTR :
								    HTM_DISABLED,
				"value=\"\""
				" onclick=\"togglecheckChildren(this,'%s')\"",
				Usr_ParUsrCod[Rol_UNK]);
	    Usr_WriteNumUsrsInList (Rol_UNK);
	 HTM_LABEL_End ();
        }
      else
        {
	 HTM_SPAN_Begin ("class=\"DAT_%s\"",The_GetSuffix ());
	    Usr_WriteNumUsrsInList (Rol_UNK);
	 HTM_SPAN_End ();
	}
     }

   /***** List users in clipboard *****/
   if (Gbl.Usrs.LstUsrs[Rol_UNK].NumUsrs)
     {
      HTM_DIV_Begin ("class=\"UsrClp_USRS\"");

	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&UsrDat);

	 /***** List users *****/
	 for (NumUsr = 0;
	      NumUsr < Gbl.Usrs.LstUsrs[Rol_UNK].NumUsrs;
	      NumUsr++)
	   {
	    /* Copy user's basic data from list */
	    Usr_CopyBasicUsrDataFromList (&UsrDat,&Gbl.Usrs.LstUsrs[Rol_UNK].Lst[NumUsr]);

	    /* Begin container */
	    HTM_DIV_Begin ("class=\"UsrClp_USR\"");

	       /* Check box to select this user */
	       if (PutForm == Frm_PUT_FORM)
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

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);

      HTM_DIV_End ();
     }
  }

/*****************************************************************************/
/************************ Get list with data of guests ***********************/
/*****************************************************************************/

static void UsrClp_GetUsrsLst (void)
  {
   char *Query = NULL;

   /***** Build query *****/
   Usr_DB_BuildQueryToGetUsrsInMyClipboard (&Query);

   /***** Get list of students from database *****/
   Usr_GetListUsrsFromQuery (Query,Hie_SYS,Rol_UNK);

   /***** Free query string *****/
   free (Query);
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
   UsrClp_RemoveClipboard ();
   UsrClp_ShowClipboardGsts ();
  }

void UsrClp_RemoveClipboardStds (void)
  {
   UsrClp_RemoveClipboard ();
   UsrClp_ShowClipboardStds ();
  }

void UsrClp_RemoveClipboardTchs (void)
  {
   UsrClp_RemoveClipboard ();
   UsrClp_ShowClipboardTchs ();
  }

static void UsrClp_RemoveClipboard (void)
  {
   extern const char *Txt_Clipboard_removed;

   /***** Remove user clipboard *****/
   Usr_DB_RemoveMyClipboard ();
   Ale_ShowAlert (Ale_SUCCESS,Txt_Clipboard_removed);
  }
