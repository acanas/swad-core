// swad_timeline_form.c: social timeline forms

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_timeline.h"
#include "swad_timeline_favourite.h"
#include "swad_timeline_form.h"
#include "swad_timeline_share.h"

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

Act_Action_t TmlFrm_ActionGbl[TmlFrm_NUM_ACTIONS] =
  {
   [TmlFrm_RECEIVE_POST] = ActRcvPstGblTL,
   [TmlFrm_RECEIVE_COMM] = ActRcvComGblTL,
   [TmlFrm_REQ_REM_NOTE] = ActReqRemPubGblTL,
   [TmlFrm_REQ_REM_COMM] = ActReqRemComGblTL,
   [TmlFrm_REM_NOTE    ] = ActRemPubGblTL,
   [TmlFrm_REM_COMM    ] = ActRemComGblTL,
   [TmlFrm_SHA_NOTE    ] = ActShaNotGblTL,
   [TmlFrm_UNS_NOTE    ] = ActUnsNotGblTL,
   [TmlFrm_FAV_NOTE    ] = ActFavNotGblTL,
   [TmlFrm_FAV_COMM    ] = ActFavComGblTL,
   [TmlFrm_UNF_NOTE    ] = ActUnfNotGblTL,
   [TmlFrm_UNF_COMM    ] = ActUnfComGblTL,
   [TmlFrm_ALL_SHA_NOTE] = ActAllShaNotGblTL,
   [TmlFrm_ALL_FAV_NOTE] = ActAllFavNotGblTL,
   [TmlFrm_ALL_FAV_COMM] = ActAllFavComGblTL,
   [TmlFrm_SHO_HID_COMM] = ActShoHidComGblTL,
  };
Act_Action_t TmlFrm_ActionUsr[TmlFrm_NUM_ACTIONS] =
  {
   [TmlFrm_RECEIVE_POST] = ActRcvPstUsrTL,
   [TmlFrm_RECEIVE_COMM] = ActRcvComUsrTL,
   [TmlFrm_REQ_REM_NOTE] = ActReqRemPubUsrTL,
   [TmlFrm_REQ_REM_COMM] = ActReqRemComUsrTL,
   [TmlFrm_REM_NOTE    ] = ActRemPubUsrTL,
   [TmlFrm_REM_COMM    ] = ActRemComUsrTL,
   [TmlFrm_SHA_NOTE    ] = ActShaNotUsrTL,
   [TmlFrm_UNS_NOTE    ] = ActUnsNotUsrTL,
   [TmlFrm_FAV_NOTE    ] = ActFavNotUsrTL,
   [TmlFrm_FAV_COMM    ] = ActFavComUsrTL,
   [TmlFrm_UNF_NOTE    ] = ActUnfNotUsrTL,
   [TmlFrm_UNF_COMM    ] = ActUnfComUsrTL,
   [TmlFrm_ALL_SHA_NOTE] = ActAllShaNotUsrTL,
   [TmlFrm_ALL_FAV_NOTE] = ActAllFavNotUsrTL,
   [TmlFrm_ALL_FAV_COMM] = ActAllFavComUsrTL,
   [TmlFrm_SHO_HID_COMM] = ActShoHidComUsrTL,
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************** Form to fav/unfav or share/unshare *********************/
/*****************************************************************************/

void TmlFrm_PutFormToFavUnfShaUns (TmlUsr_FavSha_t FavSha,long Cod)
  {
   struct Tml_Form Form[TmlUsr_NUM_FAV_SHA][2] =
     {
      [TmlUsr_FAV_UNF_NOTE] =
	{
	 [false] = // I have not faved ==> fav
	   {
	    .Action    = TmlFrm_FAV_NOTE,
	    .ParFormat = "NotCod=%ld",
	    .ParCod    = Cod,
	    .Icon      = TmlFav_ICON_FAV,
	    .Color     = Ico_BLACK,
	   },
	 [true] = // I have faved ==> unfav
	   {
	    .Action    = TmlFrm_UNF_NOTE,
	    .ParFormat = "NotCod=%ld",
	    .ParCod    = Cod,
	    .Icon      = TmlFav_ICON_FAV,
	    .Color     = Ico_RED,
	   },
	},
      [TmlUsr_FAV_UNF_COMM] =
	{
	 [false] = // I have not faved ==> fav
	   {
	    .Action    = TmlFrm_FAV_COMM,
	    .ParFormat = "PubCod=%ld",
	    .ParCod    = Cod,
	    .Icon      = TmlFav_ICON_FAV,
	    .Color     = Ico_BLACK,
	   },
	 [true] = // I have faved ==> unfav
	   {
	    .Action    = TmlFrm_UNF_COMM,
	    .ParFormat = "PubCod=%ld",
	    .ParCod    = Cod,
	    .Icon      = TmlFav_ICON_FAV,
	    .Color     = Ico_RED,
	   },
	},
      [TmlUsr_SHA_UNS_NOTE] =
	{
	 [false] = // I have not shared ==> share
	   {
	    .Action    = TmlFrm_SHA_NOTE,
	    .ParFormat = "NotCod=%ld",
	    .ParCod    = Cod,
	    .Icon      = TmlSha_ICON_SHARE,
	    .Color     = Ico_BLACK,
	   },
	 [true] = // I have shared ==> unshare
	   {
	    .Action    = TmlFrm_UNS_NOTE,
	    .ParFormat = "NotCod=%ld",
	    .ParCod    = Cod,
	    .Icon      = TmlSha_ICON_SHARE,
	    .Color     = Ico_GREEN,
	   },
	},
     };
   bool FavedShared;

   /***** Form and icon to fav/unfav note *****/
   FavedShared = TmlUsr_CheckIfFavedSharedByUsr (FavSha,Cod,Gbl.Usrs.Me.UsrDat.UsrCod);
   TmlFrm_FormFavSha (&Form[FavSha][FavedShared]);
  }

/*****************************************************************************/
/***************** Begin a form in global or user timeline *******************/
/*****************************************************************************/

void TmlFrm_BeginForm (const struct Tml_Timeline *Timeline,
                       TmlFrm_Action_t Action)
  {
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      /***** Begin form in user timeline *****/
      Frm_BeginFormAnchor (TmlFrm_ActionUsr[Action],"timeline");
	 Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
     }
   else
     {
      /***** Begin form in global timeline *****/
      Frm_BeginForm (TmlFrm_ActionGbl[Action]);
	 Usr_PutParWho (Timeline->Who);
     }
  }

void TmlFrm_EndForm (void)
  {
   Frm_EndForm ();
  }

/*****************************************************************************/
/******* Form to fav/unfav or share/unshare in global or user timeline *******/
/*****************************************************************************/

void TmlFrm_FormFavSha (const struct Tml_Form *Form)
  {
   char *OnSubmit;
   char ParStr[7 + Cns_MAX_DIGITS_LONG + 1];

   /***** Create parameter string *****/
   sprintf (ParStr,Form->ParFormat,Form->ParCod);

   /*
    ___________________________________________________________________________
   | div which content will be updated (parent of parent of form)              |
   |  _____________________   _______   _____________________________________  |
   | | div (parent of form)| | div   | | div for users                       | |
   | |  _________________  | | for   | |  ______   ______   ______   ______  | |
   | | |    this form    | | | num.  | | |      | |      | |      | | form | | |
   | | |  _____________  | | | of    | | | user | | user | | user | |  to  | | |
   | | | |   fav icon  | | | | users | | |   1  | |   2  | |   3  | | show | | |
   | | | |_____________| | | |       | | |      | |      | |      | |  all | | |
   | | |_________________| | |       | | |______| |______| |______| |______| | |
   | |_____________________| |_______| |_____________________________________| |
   |___________________________________________________________________________|
   */

   /***** Form and icon to mark note as favourite *****/
   /* Form with icon */
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      if (asprintf (&OnSubmit,"updateDivFaversSharers(this,"
			      "'act=%ld&ses=%s&%s&OtherUsrCod=%s');"
			      " return false;",	// return false is necessary to not submit form
		    Act_GetActCod (TmlFrm_ActionUsr[Form->Action]),
		    Gbl.Session.Id,
		    ParStr,
		    Gbl.Usrs.Other.UsrDat.EnUsrCod) < 0)
	 Err_NotEnoughMemoryExit ();
      Frm_BeginFormAnchorOnSubmit (ActUnk,"timeline",OnSubmit);
      free (OnSubmit);
	 Ico_PutIconLink (Form->Icon,Form->Color,
			  TmlFrm_ActionUsr[Form->Action]);
     }
   else
     {
      if (asprintf (&OnSubmit,"updateDivFaversSharers(this,"
			      "'act=%ld&ses=%s&%s');"
			      " return false;",	// return false is necessary to not submit form
		    Act_GetActCod (TmlFrm_ActionGbl[Form->Action]),
		    Gbl.Session.Id,
		    ParStr) < 0)
	 Err_NotEnoughMemoryExit ();
      Frm_BeginFormAnchorOnSubmit (ActUnk,NULL,OnSubmit);
      free (OnSubmit);
	 Ico_PutIconLink (Form->Icon,Form->Color,
			  TmlFrm_ActionGbl[Form->Action]);
     }
   Frm_EndForm ();
  }

/*****************************************************************************/
/********** Form to show hidden coments in global or user timeline ***********/
/*****************************************************************************/

void TmlFrm_FormToShowHiddenComms (long NotCod,
				   char IdComms[Frm_MAX_BYTES_ID + 1],
				   unsigned NumInitialComms)
  {
   extern const char *Txt_See_the_previous_X_COMMENTS;
   char *OnSubmit;
   const char *Anchor;
   char *Text;

   /***** Begin container which content will be updated via AJAX *****/
   HTM_DIV_Begin ("id=\"%s\" class=\"Tml_RIGHT_WIDTH\"",IdComms);

      /***** Begin container *****/
      HTM_DIV_Begin ("id=\"exp_%s\" class=\"Tml_EXPAND_COM Tml_RIGHT_WIDTH\"",
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
			  Act_GetActCod (TmlFrm_ActionUsr[TmlFrm_SHO_HID_COMM]),
			  Gbl.Session.Id,
			  NotCod,
			  IdComms,
			  NumInitialComms,
			  Gbl.Usrs.Other.UsrDat.EnUsrCod) < 0)
	       Err_NotEnoughMemoryExit ();
	    Anchor = "timeline";
	   }
	 else
	   {
	    if (asprintf (&OnSubmit,"toggleComments('%s');"
				    "updateDivHiddenComments(this,"
				    "'act=%ld&ses=%s&NotCod=%ld&IdComments=%s&NumHidCom=%u');"
				    " return false;",	// return false is necessary to not submit form
			  IdComms,
			  Act_GetActCod (TmlFrm_ActionGbl[TmlFrm_SHO_HID_COMM]),
			  Gbl.Session.Id,
			  NotCod,
			  IdComms,
			  NumInitialComms) < 0)
	       Err_NotEnoughMemoryExit ();
	    Anchor = NULL;
	   }
	 Frm_BeginFormAnchorOnSubmit (ActUnk,Anchor,OnSubmit);
	 free (OnSubmit);

	    /* Put icon and text with link to show the first hidden comments */
	    HTM_BUTTON_Submit_Begin (NULL,
	                             "class=\"BT_LINK FORM_IN_%s\"",
	                             The_GetSuffix ());
	       if (asprintf (&Text,Txt_See_the_previous_X_COMMENTS,NumInitialComms) < 0)
		  Err_NotEnoughMemoryExit ();
	       Ico_PutIconTextLink ("angle-up.svg",Ico_BLACK,Text);
	       free (Text);
	    HTM_BUTTON_End ();

	 /* End form */
	 Frm_EndForm ();

      /***** End container *****/
      HTM_DIV_End ();

   /***** Begin container which content will be updated via AJAX *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************** Form to remove note / comment ************************/
/*****************************************************************************/

void TmlFrm_BeginAlertRemove (const char *QuestionTxt)
  {
   Ale_ShowAlertAndButtonBegin (Ale_QUESTION,QuestionTxt);
  }

void TmlFrm_EndAlertRemove (struct Tml_Timeline *Timeline,
                            TmlFrm_Action_t Action,
                            void (*FuncPars) (void *Args))
  {
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
      Ale_ShowAlertAndButtonEnd (TmlFrm_ActionUsr[Action],"timeline",NULL,
			         FuncPars,Timeline,
			         Btn_REMOVE_BUTTON,Act_GetActionText (TmlFrm_ActionUsr[Action]));
   else
      Ale_ShowAlertAndButtonEnd (TmlFrm_ActionGbl[Action],NULL,NULL,
			         FuncPars,Timeline,
			         Btn_REMOVE_BUTTON,Act_GetActionText (TmlFrm_ActionGbl[Action]));
  }
