// swad_timeline_form.c: social timeline forms

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_action.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_timeline.h"
#include "swad_timeline_favourite.h"
#include "swad_timeline_form.h"
#include "swad_timeline_share.h"

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

#define Tml_Frm_ICON_ELLIPSIS	"ellipsis-h.svg"

const Act_Action_t Tml_Frm_ActionGbl[Tml_Frm_NUM_ACTIONS] =
  {
   [Tml_Frm_RECEIVE_POST] = ActRcvPstGblTL,
   [Tml_Frm_RECEIVE_COMM] = ActRcvComGblTL,
   [Tml_Frm_REQ_REM_NOTE] = ActReqRemPubGblTL,
   [Tml_Frm_REQ_REM_COMM] = ActReqRemComGblTL,
   [Tml_Frm_REM_NOTE    ] = ActRemPubGblTL,
   [Tml_Frm_REM_COMM    ] = ActRemComGblTL,
   [Tml_Frm_SHA_NOTE    ] = ActShaNotGblTL,
   [Tml_Frm_UNS_NOTE    ] = ActUnsNotGblTL,
   [Tml_Frm_FAV_NOTE    ] = ActFavNotGblTL,
   [Tml_Frm_FAV_COMM    ] = ActFavComGblTL,
   [Tml_Frm_UNF_NOTE    ] = ActUnfNotGblTL,
   [Tml_Frm_UNF_COMM    ] = ActUnfComGblTL,
   [Tml_Frm_ALL_SHA_NOTE] = ActAllShaNotGblTL,
   [Tml_Frm_ALL_FAV_NOTE] = ActAllFavNotGblTL,
   [Tml_Frm_ALL_FAV_COMM] = ActAllFavComGblTL,
   [Tml_Frm_SHO_HID_COMM] = ActShoHidComGblTL,
  };
const Act_Action_t Tml_Frm_ActionUsr[Tml_Frm_NUM_ACTIONS] =
  {
   [Tml_Frm_RECEIVE_POST] = ActRcvPstUsrTL,
   [Tml_Frm_RECEIVE_COMM] = ActRcvComUsrTL,
   [Tml_Frm_REQ_REM_NOTE] = ActReqRemPubUsrTL,
   [Tml_Frm_REQ_REM_COMM] = ActReqRemComUsrTL,
   [Tml_Frm_REM_NOTE    ] = ActRemPubUsrTL,
   [Tml_Frm_REM_COMM    ] = ActRemComUsrTL,
   [Tml_Frm_SHA_NOTE    ] = ActShaNotUsrTL,
   [Tml_Frm_UNS_NOTE    ] = ActUnsNotUsrTL,
   [Tml_Frm_FAV_NOTE    ] = ActFavNotUsrTL,
   [Tml_Frm_FAV_COMM    ] = ActFavComUsrTL,
   [Tml_Frm_UNF_NOTE    ] = ActUnfNotUsrTL,
   [Tml_Frm_UNF_COMM    ] = ActUnfComUsrTL,
   [Tml_Frm_ALL_SHA_NOTE] = ActAllShaNotUsrTL,
   [Tml_Frm_ALL_FAV_NOTE] = ActAllFavNotUsrTL,
   [Tml_Frm_ALL_FAV_COMM] = ActAllFavComUsrTL,
   [Tml_Frm_SHO_HID_COMM] = ActShoHidComUsrTL,
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************** Form to fav/unfav or share/unshare *********************/
/*****************************************************************************/

void Tml_Frm_PutFormToFavUnfShaUns (Tml_Usr_FavSha_t FavSha,long Cod)
  {
   extern const char *Txt_TIMELINE_Mark_as_favourite;
   extern const char *Txt_TIMELINE_Favourite;
   extern const char *Txt_TIMELINE_Share;
   extern const char *Txt_TIMELINE_Shared;
   struct TL_Form Form[Tml_Usr_NUM_FAV_SHA][2] =
     {
      [Tml_Usr_FAV_UNF_NOTE] =
	{
	 [false] = // I have not faved ==> fav
	   {
	    .Action      = Tml_Frm_FAV_NOTE,
	    .ParamFormat = "NotCod=%ld",
	    .ParamCod    = Cod,
	    .Icon        = Tml_Fav_ICON_FAV,
	    .Title       = Txt_TIMELINE_Mark_as_favourite,
	   },
	 [true] = // I have faved ==> unfav
	   {
	    .Action      = Tml_Frm_UNF_NOTE,
	    .ParamFormat = "NotCod=%ld",
	    .ParamCod    = Cod,
	    .Icon        = Tml_Fav_ICON_FAVED,
	    .Title       = Txt_TIMELINE_Favourite,
	   },
	},
      [Tml_Usr_FAV_UNF_COMM] =
	{
	 [false] = // I have not faved ==> fav
	   {
	    .Action      = Tml_Frm_FAV_COMM,
	    .ParamFormat = "PubCod=%ld",
	    .ParamCod    = Cod,
	    .Icon        = Tml_Fav_ICON_FAV,
	    .Title       = Txt_TIMELINE_Mark_as_favourite,
	   },
	 [true] = // I have faved ==> unfav
	   {
	    .Action      = Tml_Frm_UNF_COMM,
	    .ParamFormat = "PubCod=%ld",
	    .ParamCod    = Cod,
	    .Icon        = Tml_Fav_ICON_FAVED,
	    .Title       = Txt_TIMELINE_Favourite,
	   },
	},
      [Tml_Usr_SHA_UNS_NOTE] =
	{
	 [false] = // I have not shared ==> share
	   {
	    .Action      = Tml_Frm_SHA_NOTE,
	    .ParamFormat = "NotCod=%ld",
	    .ParamCod    = Cod,
	    .Icon        = Tml_Sha_ICON_SHARE,
	    .Title       = Txt_TIMELINE_Share,
	   },
	 [true] = // I have shared ==> unshare
	   {
	    .Action      = Tml_Frm_UNS_NOTE,
	    .ParamFormat = "NotCod=%ld",
	    .ParamCod    = Cod,
	    .Icon        = Tml_Sha_ICON_SHARED,
	    .Title       = Txt_TIMELINE_Shared,
	   },
	},
     };
   bool FavedShared;

   /***** Form and icon to fav/unfav note *****/
   FavedShared = Tml_Usr_CheckIfFavedSharedByUsr (FavSha,Cod,Gbl.Usrs.Me.UsrDat.UsrCod);
   Tml_Frm_FormFavSha (&Form[FavSha][FavedShared]);
  }

/*****************************************************************************/
/***************** Begin a form in global or user timeline *******************/
/*****************************************************************************/

void Tml_Frm_BeginForm (const struct Tml_Timeline *Timeline,
                        Tml_Frm_Action_t Action)
  {
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      /***** Begin form in user timeline *****/
      Frm_BeginFormAnchor (Tml_Frm_ActionUsr[Action],"timeline");
      Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
     }
   else
     {
      /***** Begin form in global timeline *****/
      Frm_BeginForm (Tml_Frm_ActionGbl[Action]);
      Usr_PutHiddenParamWho (Timeline->Who);
     }
  }

void Tml_Frm_EndForm (void)
  {
   Frm_EndForm ();
  }

/*****************************************************************************/
/********************* Form to show all favers/sharers ***********************/
/*****************************************************************************/

void Tml_Frm_PutFormToSeeAllFaversSharers (Tml_Frm_Action_t Action,
		                           const char *ParamFormat,long ParamCod,
                                           Tml_Usr_HowManyUsrs_t HowManyUsrs)
  {
   extern const char *Txt_View_all_USERS;
   struct TL_Form Form =
     {
      .Action      = Action,
      .ParamFormat = ParamFormat,
      .ParamCod    = ParamCod,
      .Icon        = Tml_Frm_ICON_ELLIPSIS,
      .Title       = Txt_View_all_USERS,
     };

   switch (HowManyUsrs)
     {
      case Tml_Usr_SHOW_FEW_USRS:
	 /***** Form and icon to view all users *****/
	 Tml_Frm_FormFavSha (&Form);
	 break;
      case Tml_Usr_SHOW_ALL_USRS:
	 /***** Disabled icon *****/
         Ico_PutIconOff (Tml_Frm_ICON_ELLIPSIS,Txt_View_all_USERS);
	 break;
     }
  }

/*****************************************************************************/
/******* Form to fav/unfav or share/unshare in global or user timeline *******/
/*****************************************************************************/

void Tml_Frm_FormFavSha (const struct TL_Form *Form)
  {
   char *OnSubmit;
   char ParamStr[7 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Create parameter string *****/
   sprintf (ParamStr,Form->ParamFormat,Form->ParamCod);

   /*
   +---------------------------------------------------------------------------+
   | div which content will be updated (parent of parent of form)              |
   | +---------------------+ +-------+ +-------------------------------------+ |
   | | div (parent of form)| | div   | | div for users                       | |
   | | +-----------------+ | | for   | | +------+ +------+ +------+ +------+ | |
   | | |    this form    | | | num.  | | |      | |      | |      | | form | | |
   | | | +-------------+ | | | of    | | | user | | user | | user | |  to  | | |
   | | | |   fav icon  | | | | users | | |   1  | |   2  | |   3  | | show | | |
   | | | +-------------+ | | |       | | |      | |      | |      | |  all | | |
   | | +-----------------+ | |       | | +------+ +------+ +------+ +------+ | |
   | +---------------------+ +-------+ +-------------------------------------+ |
   +---------------------------------------------------------------------------+
   */

   /***** Form and icon to mark note as favourite *****/
   /* Form with icon */
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      if (asprintf (&OnSubmit,"updateDivFaversSharers(this,"
			      "'act=%ld&ses=%s&%s&OtherUsrCod=%s');"
			      " return false;",	// return false is necessary to not submit form
		    Act_GetActCod (Tml_Frm_ActionUsr[Form->Action]),
		    Gbl.Session.Id,
		    ParamStr,
		    Gbl.Usrs.Other.UsrDat.EnUsrCod) < 0)
	 Err_NotEnoughMemoryExit ();
      Frm_BeginFormUniqueAnchorOnSubmit (ActUnk,"timeline",OnSubmit);
     }
   else
     {
      if (asprintf (&OnSubmit,"updateDivFaversSharers(this,"
			      "'act=%ld&ses=%s&%s');"
			      " return false;",	// return false is necessary to not submit form
		    Act_GetActCod (Tml_Frm_ActionGbl[Form->Action]),
		    Gbl.Session.Id,
		    ParamStr) < 0)
	 Err_NotEnoughMemoryExit ();
      Frm_BeginFormUniqueAnchorOnSubmit (ActUnk,NULL,OnSubmit);
     }
   Ico_PutIconLink (Form->Icon,Form->Title);
   Frm_EndForm ();

   /* Free allocated memory */
   free (OnSubmit);
  }

/*****************************************************************************/
/********** Form to show hidden coments in global or user timeline ***********/
/*****************************************************************************/

void Tml_Frm_FormToShowHiddenComms (long NotCod,
				    char IdComms[Frm_MAX_BYTES_ID + 1],
				    unsigned NumInitialComms)
  {
   extern const char *The_ClassFormLinkInBox[The_NUM_THEMES];
   extern const char *Txt_See_the_previous_X_COMMENTS;
   char *OnSubmit;

   /***** Begin container which content will be updated via AJAX *****/
   HTM_DIV_Begin ("id=\"%s\" class=\"TL_RIGHT_WIDTH\"",IdComms);

      /***** Begin container *****/
      HTM_DIV_Begin ("id=\"exp_%s\" class=\"TL_EXPAND_COM TL_RIGHT_WIDTH\"",
		     IdComms);

	 /***** Form and icon-text to show hidden comments *****/
	 /* Begin form */
	 if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	   {
	    if (asprintf (&OnSubmit,"toggleComments('%s');"
				    "updateDivHiddenComments(this,"
				    "'act=%ld&ses=%s&NotCod=%ld&IdComments=%s&NumHidCom=%u&OtherUsrCod=%s');"
				    " return false;",	// return false is necessary to not submit form
			  IdComms,
			  Act_GetActCod (Tml_Frm_ActionUsr[Tml_Frm_SHO_HID_COMM]),
			  Gbl.Session.Id,
			  NotCod,
			  IdComms,
			  NumInitialComms,
			  Gbl.Usrs.Other.UsrDat.EnUsrCod) < 0)
	       Err_NotEnoughMemoryExit ();
	    Frm_BeginFormUniqueAnchorOnSubmit (ActUnk,"timeline",OnSubmit);
	   }
	 else
	   {
	    if (asprintf (&OnSubmit,"toggleComments('%s');"
				    "updateDivHiddenComments(this,"
				    "'act=%ld&ses=%s&NotCod=%ld&IdComments=%s&NumHidCom=%u');"
				    " return false;",	// return false is necessary to not submit form
			  IdComms,
			  Act_GetActCod (Tml_Frm_ActionGbl[Tml_Frm_SHO_HID_COMM]),
			  Gbl.Session.Id,
			  NotCod,
			  IdComms,
			  NumInitialComms) < 0)
	       Err_NotEnoughMemoryExit ();
	    Frm_BeginFormUniqueAnchorOnSubmit (ActUnk,NULL,OnSubmit);
	   }

	 /* Put icon and text with link to show the first hidden comments */
	 HTM_BUTTON_SUBMIT_Begin (NULL,The_ClassFormLinkInBox[Gbl.Prefs.Theme],NULL);
	    Ico_PutIconTextLink ("angle-up.svg",
				 Str_BuildStringLong (Txt_See_the_previous_X_COMMENTS,
						      (long) NumInitialComms));
	    Str_FreeString ();
	 HTM_BUTTON_End ();

	 /* End form */
	 Frm_EndForm ();

	 /* Free allocated memory */
	 free (OnSubmit);

      /***** End container *****/
      HTM_DIV_End ();

   /***** Begin container which content will be updated via AJAX *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************** Form to remove note / comment ************************/
/*****************************************************************************/

void Tml_Frm_BeginAlertRemove (const char *QuestionTxt)
  {
   Ale_ShowAlertAndButton1 (Ale_QUESTION,QuestionTxt);
  }

void Tml_Frm_EndAlertRemove (struct Tml_Timeline *Timeline,
                             Tml_Frm_Action_t Action,
                             void (*FuncParams) (void *Args))
  {
   extern const char *Txt_Remove;

   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
      Ale_ShowAlertAndButton2 (Tml_Frm_ActionUsr[Action],"timeline",NULL,
			       FuncParams,Timeline,
			       Btn_REMOVE_BUTTON,Txt_Remove);
   else
      Ale_ShowAlertAndButton2 (Tml_Frm_ActionGbl[Action],NULL,NULL,
			       FuncParams,Timeline,
			       Btn_REMOVE_BUTTON,Txt_Remove);
  }
