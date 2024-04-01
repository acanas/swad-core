// swad_timeline_comment.c: social timeline comments

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
#include <linux/limits.h>	// For PATH_MAX
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_error.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_message.h"
#include "swad_notification_database.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"
#include "swad_profile.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_form.h"
#include "swad_timeline_note.h"
#include "swad_timeline_publication.h"

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

#define TmlCom_NUM_VISIBLE_COMMENTS	3	// Maximum number of comments visible before expanding them

#define TmlCom_NUM_CONTRACT_EXPAND	2
typedef enum
  {
   TmlCom_CONTRACT = 0,
   TmlCom_EXPAND   = 1,
  } TmlCom_ContractExpand_t;

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TmlCom_ShowAuthorPhoto (struct Usr_Data *UsrDat);
static void TmlCom_PutFormToWriteNewComm (const struct Tml_Timeline *Timeline,
	                                  long NotCod);

static unsigned TmlCom_WriteHiddenComms (struct Tml_Timeline *Timeline,
                                         long NotCod,
				         char IdComms[Frm_MAX_BYTES_ID + 1],
					 unsigned NumInitialCommsToGet);
static void TmlCom_ListComms (const struct Tml_Timeline *Timeline,
                              unsigned NumComms,MYSQL_RES *mysql_res);
static void TmlCom_WriteOneCommInList (const struct Tml_Timeline *Timeline,
                                       MYSQL_RES *mysql_res);
static void TmlCom_LinkToShowComms (TmlCom_ContractExpand_t ConExp,
                                    const char IdComms[Frm_MAX_BYTES_ID + 1],
                                    unsigned NumComms);
static void TmlCom_PutIconToToggleComms (const char *UniqueId,
                                         const char *Icon,const char *Text);
static void TmlCom_CheckAndWriteComm (const struct Tml_Timeline *Timeline,
	                              struct TmlCom_Comment *Com);
static void TmlCom_WriteComm (const struct Tml_Timeline *Timeline,
	                      struct TmlCom_Comment *Com);
static void TmlCom_WriteAuthorTimeAndContent (struct TmlCom_Comment *Com,
                                              const struct Usr_Data *UsrDat);
static void TmlCom_WriteAuthorName (const struct Usr_Data *UsrDat);
static void TmlCom_WriteContent (struct TmlCom_Comment *Com);
static void TmlCom_WriteButtons (const struct Tml_Timeline *Timeline,
	                         const struct TmlCom_Comment *Com,
                                 const struct Usr_Data *UsrDat);

static void TmlCom_PutFormToRemoveComm (const struct Tml_Timeline *Timeline,
	                                long PubCod);

static long TmlCom_ReceiveComm (void);

static void TmlCom_ReqRemComm (struct Tml_Timeline *Timeline);
static void TmlCom_PutParsRemoveComm (void *Timeline);
static void TmlCom_RemoveComm (void);

static void TmlCom_GetCommDataFromRow (MYSQL_RES *mysql_res,
                                       struct TmlCom_Comment *Com);

static void TmlCom_ResetComm (struct TmlCom_Comment *Com);

/*****************************************************************************/
/********* Put an icon to toggle on/off the form to comment a note ***********/
/*****************************************************************************/

void TmlCom_PutIconToToggleComm (const struct Tml_Timeline *Timeline,
                                 const char UniqueId[Frm_MAX_BYTES_ID + 1])
  {
   /***** Begin container *****/
   HTM_DIV_Begin ("id=\"%s_ico\" class=\"Tml_ICO_COM_OFF\"",UniqueId);

      /* Begin anchor */
      HTM_A_Begin ("href=\"\" onclick=\"toggleNewComment ('%s');"
	                               "return false;\"",
		   UniqueId);

         /* Icon to toggle comment */
	 Ico_PutIcon ("comment-regular.svg",Ico_BLACK,
	              Act_GetActionText (Timeline->UsrOrGbl == TmlUsr_TIMELINE_GBL ? ActRcvComGblTL :
	        	                                                             ActRcvComUsrTL),
	              "CONTEXT_ICO16x16");

      /* End anchor */
      HTM_A_End ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Put an icon to toggle on/off the form to comment a note **********/
/*****************************************************************************/

void TmlCom_PutIconCommDisabled (void)
  {
   extern const char *Txt_not_available;

   /***** Disabled icon to comment a note *****/
   /* Begin container */
   HTM_DIV_Begin ("class=\"Tml_ICO_COM_OFF Tml_ICO_DISABLED\"");

      /* Disabled icon */
      Ico_PutIcon ("comment-regular.svg",Ico_BLACK,Txt_not_available,"ICO16x16");

   /* End container */
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************************* Form to comment a note ****************************/
/*****************************************************************************/

void TmlCom_PutPhotoAndFormToWriteNewComm (const struct Tml_Timeline *Timeline,
	                                   long NotCod,
                                           const char IdNewComm[Frm_MAX_BYTES_ID + 1])
  {
   /***** Begin container *****/
   HTM_DIV_Begin ("id=\"%s\" class=\"Tml_FORM_NEW_COM Tml_RIGHT_WIDTH\""
		  " style=\"display:none;\"",
	          IdNewComm);

      /***** Left: write author's photo (my photo) *****/
      TmlCom_ShowAuthorPhoto (&Gbl.Usrs.Me.UsrDat);

      /***** Right: form to write the comment *****/
      TmlCom_PutFormToWriteNewComm (Timeline,NotCod);

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Show photo of author of a comment *********************/
/*****************************************************************************/

static void TmlCom_ShowAuthorPhoto (struct Usr_Data *UsrDat)
  {
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC30x40",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE30x40",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO30x40",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR30x40",
     };

   /***** Show author's photo *****/
   /* Begin container */
   HTM_DIV_Begin ("class=\"Tml_COM_PHOTO\"");

      /* Author's photo */
      Pho_ShowUsrPhotoIfAllowed (UsrDat,
                                 ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);

   /* End container */
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**************************** Form with textarea *****************************/
/*****************************************************************************/

static void TmlCom_PutFormToWriteNewComm (const struct Tml_Timeline *Timeline,
	                                  long NotCod)
  {
   extern const char *Txt_New_TIMELINE_comment;
   char *ClassTextArea;

   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"Tml_COM_CONT Tml_COM_WIDTH\"");

      /***** Begin form to write the post *****/
      TmlFrm_BeginForm (Timeline,TmlFrm_RECEIVE_COMM);
	 ParCod_PutPar (ParCod_Not,NotCod);

	 /***** Textarea and button *****/
	 if (asprintf (&ClassTextArea,"Tml_COM_TEXTAREA Tml_COM_WIDTH INPUT_%s",
	               The_GetSuffix ()) < 0)
	    Err_NotEnoughMemoryExit ();
	 TmlPst_PutTextarea (Txt_New_TIMELINE_comment,ClassTextArea);
	 free (ClassTextArea);

      /***** End form *****/
      TmlFrm_EndForm ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*********************** Write comments in a note ****************************/
/*****************************************************************************/

void TmlCom_WriteCommsInNote (const struct Tml_Timeline *Timeline,
			      const struct TmlNot_Note *Not)
  {
   MYSQL_RES *mysql_res;
   unsigned NumComms;
   unsigned NumInitialComms;
   unsigned NumFinalCommsToGet;
   unsigned NumFinalComms;
   char IdComms[Frm_MAX_BYTES_ID + 1];

   /***** Get number of comments in note *****/
   NumComms = Tml_DB_GetNumCommsInNote (Not->NotCod);

   /***** Trivial check: if no comments ==> nothing to do *****/
   if (!NumComms)
      return;

   /***** Compute how many initial comments will be hidden
          and how many final comments will be visible *****/
   // Never hide only one comment
   // So, the number of comments initially hidden must be 0 or >= 2
   if (NumComms <= TmlCom_NUM_VISIBLE_COMMENTS + 1)
     {
      NumInitialComms    = 0;
      NumFinalCommsToGet = NumComms;
     }
   else
     {
      NumInitialComms    = NumComms - TmlCom_NUM_VISIBLE_COMMENTS;
      NumFinalCommsToGet = TmlCom_NUM_VISIBLE_COMMENTS;
     }

   /***** Get final comments of this note from database *****/
   NumFinalComms = Tml_DB_GetFinalComms (Not->NotCod,NumFinalCommsToGet,&mysql_res);
   /*
      Before clicking "See prev..."    -->    After clicking "See prev..."
    _________________________________       _________________________________
   |           div con_<id>          |     |           div con_<id>          |
   |            (hidden)             |     |            (visible)            |
   |  _____________________________  |     |  _____________________________  |
   | |    v See only the latest    | |     | |    v See only the latest    | |
   | |_________(contract)__________| |     | |_________(contract)__________| |
   |_________________________________|     |_________________________________|
    _________________________________       _________________________________
   |            div <id>             |     |        div <id> updated         |
   |          which content          |     |  _____________________________  |
   |    will be updated via AJAX     |     | |         ul com_<id>         | |
   |   (parent of parent of form)    |     | |  _________________________  | |
   |                                 |     | | |     li (comment 1)      | | |
   |                                 |     | | |_________________________| | |
   |                                 |     | | |           ...           | | |
   |                                 |     | | |_________________________| | |
   |                                 |     | | |     li (comment n)      | | |
   |                                 | --> | | |_________________________| | |
   |                                 |     | |_____________________________| |
   |  _____________________________  |     |  _____________________________  |
   | |        div exp_<id>         | |     | |         div exp_<id>        | |
   | |          (visible)          | |     | |          (hidden)           | |
   | |  _________________________  | |     | |                             | |
   | | |          form           | | |     | |                             | |
   | | |  _____________________  | | |     | |    _____________________    | |
   | | | | ^ See prev.comments | | | |     | |   | ^ See prev.comments |   | |
   | | | |_______(expand)______| | | |     | |   |_______(expand)______|   | |
   | | |_________________________| | |     | |                             | |
   | |_____________________________| |     | |_____________________________| |
   |_________________________________|     |_________________________________|
    _________________________________       _________________________________
   |                ul               |     |                ul               |
   |    _________________________    |     |    _________________________    |
   |   |     li (comment n+1)    |   |     |   |     li (comment n+1)    |   |
   |   |_________________________|   |     |   |_________________________|   |
   |   |           ...           |   |     |   |           ...           |   |
   |   |_________________________|   |     |   |_________________________|   |
   |   |     li (comment m)      |   |     |   |     li (comment m)      |   |
   |   |_________________________|   |     |   |_________________________|   |
   |_________________________________|     |_________________________________|
   */
   /***** Link to show initial hidden comments *****/
   if (NumInitialComms)
     {
      /***** Create unique id for list of hidden comments *****/
      Frm_SetUniqueId (IdComms);

      /***** Link (initially hidden) to show only the latest comments *****/
      TmlCom_LinkToShowComms (TmlCom_CONTRACT,IdComms,NumFinalComms);

      /***** Div with form to show hidden coments,
             which content will be updated via AJAX *****/
      TmlFrm_FormToShowHiddenComms (Not->NotCod,IdComms,NumInitialComms);
     }

   /***** List final visible comments *****/
   if (NumFinalComms)
     {
      HTM_UL_Begin ("class=\"Tml_LIST\"");	// Never hidden, always visible
	 TmlCom_ListComms (Timeline,NumFinalComms,mysql_res);
      HTM_UL_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Write hidden comments via AJAX ***********************/
/*****************************************************************************/

void TmlCom_ShowHiddenCommsUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show hidden comments *****/
   TmlCom_ShowHiddenCommsGbl ();
  }

void TmlCom_ShowHiddenCommsGbl (void)
  {
   struct Tml_Timeline Timeline;
   long NotCod;
   char IdComms[Frm_MAX_BYTES_ID + 1];
   unsigned NumInitialCommsToGet;
   unsigned NumInitialCommsGot;

   /***** Reset timeline context *****/
   Tml_ResetTimeline (&Timeline);

   /***** Get parameters *****/
   /* Get note code, identifier and number of comments to get */
   NotCod = ParCod_GetPar (ParCod_Not);
   Par_GetParText ("IdComments",IdComms,Frm_MAX_BYTES_ID);
   NumInitialCommsToGet = (unsigned) Par_GetParLong ("NumHidCom");

   /***** Write HTML inside DIV with hidden comments *****/
   NumInitialCommsGot =
   TmlCom_WriteHiddenComms (&Timeline,NotCod,IdComms,NumInitialCommsToGet);

   /***** Link to show the first comments *****/
   TmlCom_LinkToShowComms (TmlCom_EXPAND,IdComms,NumInitialCommsGot);
  }

/*****************************************************************************/
/**************************** Write hidden comments **************************/
/*****************************************************************************/
// Returns the number of comments got

static unsigned TmlCom_WriteHiddenComms (struct Tml_Timeline *Timeline,
                                         long NotCod,
				         char IdComms[Frm_MAX_BYTES_ID + 1],
					 unsigned NumInitialCommsToGet)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumInitialCommsGot;

   /***** Get comments of this note from database *****/
   NumInitialCommsGot = Tml_DB_GetInitialComms (NotCod,
				                NumInitialCommsToGet,
				                &mysql_res);

   /***** List comments *****/
   HTM_UL_Begin ("id=\"com_%s\" class=\"Tml_LIST\"",IdComms);
      TmlCom_ListComms (Timeline,NumInitialCommsGot,mysql_res);
   HTM_UL_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumInitialCommsGot;
  }

/*****************************************************************************/
/*********************** Write comments in a note ****************************/
/*****************************************************************************/

static void TmlCom_ListComms (const struct Tml_Timeline *Timeline,
                              unsigned NumComms,MYSQL_RES *mysql_res)
  {
   unsigned NumCom;

   /***** List comments one by one *****/
   for (NumCom = 0;
	NumCom < NumComms;
	NumCom++)
      TmlCom_WriteOneCommInList (Timeline,mysql_res);
  }

/*****************************************************************************/
/************************* Write a comment in list ***************************/
/*****************************************************************************/

static void TmlCom_WriteOneCommInList (const struct Tml_Timeline *Timeline,
                                       MYSQL_RES *mysql_res)
  {
   struct TmlCom_Comment Com;

   /***** Initialize image *****/
   Med_MediaConstructor (&Com.Content.Media);

   /***** Get data of comment *****/
   TmlCom_GetCommDataFromRow (mysql_res,&Com);

   /***** Write comment *****/
   HTM_LI_Begin ("class=\"Tml_COM\"");
      TmlCom_CheckAndWriteComm (Timeline,&Com);
   HTM_LI_End ();

   /***** Free image *****/
   Med_MediaDestructor (&Com.Content.Media);
  }

/*****************************************************************************/
/**** Link to "show only the latest comments" / "show the first comments" ****/
/*****************************************************************************/

static void TmlCom_LinkToShowComms (TmlCom_ContractExpand_t ConExp,
                                    const char IdComms[Frm_MAX_BYTES_ID + 1],
                                    unsigned NumComms)
  {
   extern const char *Txt_See_only_the_latest_X_COMMENTS;
   extern const char *Txt_See_the_previous_X_COMMENTS;
   static const struct
     {
      const char *Id;
      const char *Icon;
      const char **Text;
     } Link[TmlCom_NUM_CONTRACT_EXPAND] =
     {
	[TmlCom_CONTRACT] =
	  {
	   .Id   = "con",
	   .Icon = "angle-down.svg",
	   .Text = &Txt_See_only_the_latest_X_COMMENTS,
	  },
	[TmlCom_EXPAND] =
	  {
	   .Id   = "exp",
	   .Icon = "angle-up.svg",
	   .Text = &Txt_See_the_previous_X_COMMENTS,
	  },
     };
   char *Text;

   /***** Link (icon and text) to show comments ****/
   /* Begin container */
   HTM_DIV_Begin ("id=\"%s_%s\" class=\"Tml_EXPAND_COM Tml_RIGHT_WIDTH\""
		  " style=\"display:none;\"",	// Hidden
		  Link[ConExp].Id,IdComms);

      /* Icon and text */
      if (asprintf (&Text,*Link[ConExp].Text,NumComms) < 0)
	 Err_NotEnoughMemoryExit ();
      TmlCom_PutIconToToggleComms (IdComms,Link[ConExp].Icon,Text);
      free (Text);

   /* End container */
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Put an icon to toggle on/off comments in a publication ***********/
/*****************************************************************************/

static void TmlCom_PutIconToToggleComms (const char *UniqueId,
                                         const char *Icon,const char *Text)
  {
   /***** Link to toggle on/off some divs *****/
   HTM_BUTTON_Begin (Text,
                     "class=\"BT_LINK FORM_IN_%s\""
                     " onclick=\"toggleComments('%s');return false;\"",
                     The_GetSuffix (),UniqueId);
      Ico_PutIconTextLink (Icon,Ico_BLACK,Text);
   HTM_BUTTON_End ();
  }

/*****************************************************************************/
/************************** Check and write comment **************************/
/*****************************************************************************/

static void TmlCom_CheckAndWriteComm (const struct Tml_Timeline *Timeline,
	                              struct TmlCom_Comment *Com)
  {
   /*
    ___________________________________________
   | _____  |                      |           | \              \
   ||     | | Author's name        | Date-time |  |              |
   ||Auth.| |______________________|___________|  |              |
   ||photo| |                                  |  |  author's    |
   ||_____| |                                  |   > name, time  |
   |        |             Comment              |  |  and content  > comment
   |        |             content              |  |              |
   |        |                                  |  |              |
   |        |__________________________________| /               |
   |        |                           |      | \               |
   |        | Favs                      |Remove|   > buttons     |
   |________|___________________________|______| /              /
   */

   /***** Trivial check: codes *****/
   if (Com->PubCod <= 0 ||
       Com->NotCod <= 0 ||
       Com->UsrCod <= 0)
     {
      Ale_ShowAlert (Ale_ERROR,"Error in comment.");
      return;
     }

   /***** Write comment *****/
   TmlCom_WriteComm (Timeline,Com);
  }

/*****************************************************************************/
/******************************** Write comment ******************************/
/*****************************************************************************/

static void TmlCom_WriteComm (const struct Tml_Timeline *Timeline,
	                      struct TmlCom_Comment *Com)
  {
   struct Usr_Data UsrDat;	// Author of the comment

   /***** Get author's data *****/
   Usr_UsrDataConstructor (&UsrDat);
   UsrDat.UsrCod = Com->UsrCod;
   Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
                                            Usr_DONT_GET_PREFS,
                                            Usr_DONT_GET_ROLE_IN_CRS);

   /***** Left: author's photo *****/
   TmlCom_ShowAuthorPhoto (&UsrDat);

   /***** Right: author's name, time, content, and buttons *****/
   /* Begin container */
   HTM_DIV_Begin ("class=\"Tml_COM_CONT Tml_COM_WIDTH\"");

      /* Right top: author's name, time, and content */
      TmlCom_WriteAuthorTimeAndContent (Com,&UsrDat);

      /* Right bottom: buttons */
      TmlCom_WriteButtons (Timeline,Com,&UsrDat);

   /* End container */
   HTM_DIV_End ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/**** Write top right part of a note: author's name, time and note content ***/
/*****************************************************************************/

static void TmlCom_WriteAuthorTimeAndContent (struct TmlCom_Comment *Com,
                                              const struct Usr_Data *UsrDat)	// Author
  {
   /***** Write author's full name and nickname *****/
   TmlCom_WriteAuthorName (UsrDat);

   /***** Write date and time *****/
   Tml_WriteDateTime (Com->DateTimeUTC);

   /***** Write content of the comment *****/
   TmlCom_WriteContent (Com);
  }

/*****************************************************************************/
/*************** Write name of author of a comment to a note *****************/
/*****************************************************************************/

static void TmlCom_WriteAuthorName (const struct Usr_Data *UsrDat)	// Author
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;
   const char *Title[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_My_public_profile,
      [Usr_OTHER] = Txt_Another_user_s_profile,
     };

   /***** Show user's name inside form to go to user's public profile *****/
   /* Begin form */
   Frm_BeginForm (ActSeeOthPubPrf);
      Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);

      /* Author's name */
      HTM_BUTTON_Submit_Begin (Title[Usr_ItsMe (UsrDat->UsrCod)],
			       "class=\"Tml_COM_AUTHOR Tml_COM_AUTHOR_WIDTH BT_LINK DAT_%s BOLD\"",
                               The_GetSuffix ());
	 HTM_Txt (UsrDat->FullName);
      HTM_BUTTON_End ();

   /* End form */
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************* Write content of comment **************************/
/*****************************************************************************/

static void TmlCom_WriteContent (struct TmlCom_Comment *Com)
  {
   /***** Write content of the comment *****/
   if (Com->Content.Txt[0])
     {
      HTM_DIV_Begin ("class=\"Tml_TXT Tml_TXT_%s\"",
                     The_GetSuffix ());
	 Msg_WriteMsgContent (Com->Content.Txt,true,false);
      HTM_DIV_End ();
     }

   /***** Show image *****/
   Med_ShowMedia (&Com->Content.Media,"Tml_COM_MED_CONT Tml_COM_WIDTH",
				      "Tml_COM_MED Tml_COM_WIDTH");
  }

/*****************************************************************************/
/********************* Write bottom part of a comment ************************/
/*****************************************************************************/

static void TmlCom_WriteButtons (const struct Tml_Timeline *Timeline,
	                         const struct TmlCom_Comment *Com,
                                 const struct Usr_Data *UsrDat)	// Author
  {
   static unsigned NumDiv = 0;	// Used to create unique div id for fav

   NumDiv++;

   /***** Begin buttons container *****/
   HTM_DIV_Begin ("class=\"Tml_FOOT Tml_COM_WIDTH\"");

      /***** Foot column 1: fav zone *****/
      HTM_DIV_Begin ("id=\"fav_com_%s_%u\" class=\"Tml_FAV_COM Tml_FAV_WIDTH\"",
		     Cry_GetUniqueNameEncrypted (),NumDiv);
	 TmlUsr_PutIconFavSha (TmlUsr_FAV_UNF_COMM,
	                       Com->PubCod,Com->UsrCod,Com->NumFavs,
			       TmlUsr_SHOW_FEW_USRS);
      HTM_DIV_End ();

      /***** Foot column 2: icon to remove this comment *****/
      HTM_DIV_Begin ("class=\"Tml_REM\"");
	 if (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)	// I am the author
	    TmlCom_PutFormToRemoveComm (Timeline,Com->PubCod);
      HTM_DIV_End ();

   /***** End buttons container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************************* Form to remove comment ****************************/
/*****************************************************************************/

static void TmlCom_PutFormToRemoveComm (const struct Tml_Timeline *Timeline,
	                                long PubCod)
  {
   extern Act_Action_t TmlFrm_ActionUsr[TmlFrm_NUM_ACTIONS];
   extern Act_Action_t TmlFrm_ActionGbl[TmlFrm_NUM_ACTIONS];

   /***** Form to remove publication *****/
   /* Begin form */
   TmlFrm_BeginForm (Timeline,TmlFrm_REQ_REM_COMM);
      ParCod_PutPar (ParCod_Pub,PubCod);

      /* Icon to remove */
      Ico_PutIconLink ("trash.svg",Ico_RED,
                       Gbl.Usrs.Other.UsrDat.UsrCod > 0 ? TmlFrm_ActionUsr[TmlFrm_REQ_REM_COMM] :
                		                          TmlFrm_ActionGbl[TmlFrm_REQ_REM_COMM]);

   /* End form */
   TmlFrm_EndForm ();
  }

/*****************************************************************************/
/******************************* Comment a note ******************************/
/*****************************************************************************/

void TmlCom_ReceiveCommUsr (void)
  {
   struct Tml_Timeline Timeline;
   long NotCod;

   /***** Reset timeline context *****/
   Tml_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUsrProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Begin section *****/
   HTM_SECTION_Begin (Tml_TIMELINE_SECTION_ID);

      /***** Receive comment in a note
	     and write updated timeline after commenting (user) *****/
      NotCod = TmlCom_ReceiveComm ();
      Tml_ShowTimelineUsrHighlighting (&Timeline,NotCod);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TmlCom_ReceiveCommGbl (void)
  {
   struct Tml_Timeline Timeline;
   long NotCod;

   /***** Initialize timeline *****/
   Tml_InitTimelineGbl (&Timeline);

   /***** Receive comment in a note *****/
   NotCod = TmlCom_ReceiveComm ();

   /***** Write updated timeline after commenting (global) *****/
   Tml_ShowTimelineGblHighlighting (&Timeline,NotCod);
  }

static long TmlCom_ReceiveComm (void)
  {
   extern const char *Txt_The_post_no_longer_exists;
   struct TmlPst_Content Content;
   struct TmlNot_Note Not;
   struct TmlPub_Publication Pub;

   /***** Get data of note *****/
   Not.NotCod = ParCod_GetAndCheckPar (ParCod_Not);
   TmlNot_GetNoteDataByCod (&Not);

   /***** Trivial check: note code *****/
   if (Not.NotCod <= 0)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_The_post_no_longer_exists);
      return -1L;
     }

   /***** Get the content of the comment *****/
   Par_GetParAndChangeFormat ("Txt",Content.Txt,Cns_MAX_BYTES_LONG_TEXT,
			      Str_TO_RIGOROUS_HTML,Str_REMOVE_SPACES);

   /***** Initialize image *****/
   Med_MediaConstructor (&Content.Media);

   /***** Get attached image (action, file and title) *****/
   Content.Media.Width   = Tml_IMAGE_SAVED_MAX_WIDTH;
   Content.Media.Height  = Tml_IMAGE_SAVED_MAX_HEIGHT;
   Content.Media.Quality = Tml_IMAGE_SAVED_QUALITY;
   Med_GetMediaFromForm (-1L,-1L,-1,&Content.Media,NULL,NULL);
   Ale_ShowAlerts (NULL);

   if (Content.Txt[0] ||			// Text not empty
       Content.Media.Status == Med_PROCESSED)	// A media is attached
     {
      /***** Store media in filesystem and database *****/
      Med_RemoveKeepOrStoreMedia (-1L,&Content.Media);

      /***** Publish *****/
      /* Insert into publications */
      Pub.NotCod       = Not.NotCod;
      Pub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
      Pub.Type         = TmlPub_COMMENT_TO_NOTE;
      TmlPub_PublishPubInTimeline (&Pub);	// Set Pub.PubCod

      /* Insert comment content in the database */
      Tml_DB_InsertCommContent (Pub.PubCod,&Content);

      /***** Store notifications about the new comment *****/
      Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_TML_COMMENT,Pub.PubCod);

      /***** Analyze content and store notifications about mentions *****/
      Str_AnalyzeTxtAndStoreNotifyEventToMentionedUsrs (Pub.PubCod,Content.Txt);
     }

   /***** Free image *****/
   Med_MediaDestructor (&Content.Media);

   return Not.NotCod;
  }

/*****************************************************************************/
/**************** Request the removal of a comment in a note *****************/
/*****************************************************************************/

void TmlCom_ReqRemComUsr (void)
  {
   struct Tml_Timeline Timeline;

   /***** Reset timeline context *****/
   Tml_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUsrProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Begin section *****/
   HTM_SECTION_Begin (Tml_TIMELINE_SECTION_ID);

      /***** Request the removal of comment in note *****/
      TmlCom_ReqRemComm (&Timeline);

      /***** Write timeline again (user) *****/
      Tml_ShowTimelineUsr (&Timeline);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TmlCom_ReqRemComGbl (void)
  {
   struct Tml_Timeline Timeline;

   /***** Initialize timeline *****/
   Tml_InitTimelineGbl (&Timeline);

   /***** Request the removal of comment in note *****/
   TmlCom_ReqRemComm (&Timeline);

   /***** Write timeline again (global) *****/
   Tml_ShowNoteAndTimelineGbl (&Timeline);
  }

static void TmlCom_ReqRemComm (struct Tml_Timeline *Timeline)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_following_comment;
   struct TmlCom_Comment Com;

   /***** Initialize media *****/
   Med_MediaConstructor (&Com.Content.Media);

   /***** Get data of comment *****/
   Com.PubCod = ParCod_GetAndCheckPar (ParCod_Pub);
   TmlCom_GetCommDataByCod (&Com);

   /***** Do some checks *****/
   if (TmlUsr_CheckIfICanRemove (Com.PubCod,Com.UsrCod) == Usr_I_CAN_NOT)
     {
      Med_MediaDestructor (&Com.Content.Media);
      return;
     }

   /***** Show question and button to remove comment *****/
   /* Begin alert */
   TmlFrm_BeginAlertRemove (Txt_Do_you_really_want_to_remove_the_following_comment);

      /* Begin box for the comment */
      Box_BoxBegin (NULL,NULL,NULL,NULL,Box_NOT_CLOSABLE);

         /* Indent the comment */
	 HTM_DIV_Begin ("class=\"Tml_LEFT_PHOTO\"");
	 HTM_DIV_End ();

	 /* Show the comment */
	 HTM_DIV_Begin ("class=\"Tml_RIGHT_CONT Tml_RIGHT_WIDTH\"");
	    TmlCom_CheckAndWriteComm (Timeline,&Com);
	 HTM_DIV_End ();

      /* End box */
      Box_BoxEnd ();

   /* End alert */
   Timeline->PubCod = Com.PubCod;	// Publication to be removed
   TmlFrm_EndAlertRemove (Timeline,TmlFrm_REM_COMM,TmlCom_PutParsRemoveComm);

   /***** Free media *****/
   Med_MediaDestructor (&Com.Content.Media);
  }

/*****************************************************************************/
/******************** Put parameters to remove a comment *********************/
/*****************************************************************************/

static void TmlCom_PutParsRemoveComm (void *Timeline)
  {
   if (Timeline)
     {
      if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)	// User's timeline
	 Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
      else					// Global timeline
	 Usr_PutParWho (((struct Tml_Timeline *) Timeline)->Who);
      ParCod_PutPar (ParCod_Pub,((struct Tml_Timeline *) Timeline)->PubCod);
     }
  }

/*****************************************************************************/
/***************************** Remove a comment ******************************/
/*****************************************************************************/

void TmlCom_RemoveComUsr (void)
  {
   struct Tml_Timeline Timeline;

   /***** Reset timeline context *****/
   Tml_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUsrProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Begin section *****/
   HTM_SECTION_Begin (Tml_TIMELINE_SECTION_ID);

      /***** Remove a comment *****/
      TmlCom_RemoveComm ();

      /***** Write updated timeline after removing (user) *****/
      Tml_ShowTimelineUsr (&Timeline);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TmlCom_RemoveComGbl (void)
  {
   struct Tml_Timeline Timeline;

   /***** Initialize timeline *****/
   Tml_InitTimelineGbl (&Timeline);

   /***** Remove a comment *****/
   TmlCom_RemoveComm ();

   /***** Write updated timeline after removing (global) *****/
   Tml_ShowNoteAndTimelineGbl (&Timeline);
  }

static void TmlCom_RemoveComm (void)
  {
   extern const char *Txt_The_post_no_longer_exists;
   extern const char *Txt_Comment_removed;
   struct TmlCom_Comment Com;

   /***** Initialize media *****/
   Med_MediaConstructor (&Com.Content.Media);

   /***** Get data of comment *****/
   Com.PubCod = ParCod_GetAndCheckPar (ParCod_Pub);
   TmlCom_GetCommDataByCod (&Com);

   /***** Trivial check 1: publication code *****/
   if (Com.PubCod <= 0)
     {
      Med_MediaDestructor (&Com.Content.Media);
      Ale_ShowAlert (Ale_WARNING,Txt_The_post_no_longer_exists);
      return;
     }

   /***** Trivial check 2: only if I am the author of this comment? *****/
   if (Usr_ItsMe (Com.UsrCod) == Usr_OTHER)
     {
      Med_MediaDestructor (&Com.Content.Media);
      Err_NoPermission ();
      return;
     }

   /***** Remove media associated to comment
	  and delete comment from database *****/
   TmlCom_RemoveCommMediaAndDBEntries (Com.PubCod);

   /***** Free media *****/
   Med_MediaDestructor (&Com.Content.Media);

   /***** Reset fields of comment *****/
   TmlCom_ResetComm (&Com);

   /***** Message of success *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Comment_removed);
  }

/*****************************************************************************/
/*************** Remove comment media and database entries *******************/
/*****************************************************************************/

void TmlCom_RemoveCommMediaAndDBEntries (long PubCod)
  {
   /***** Remove media associated to comment *****/
   Med_RemoveMedia (Tml_DB_GetMedCodFromComm (PubCod));

   /***** Mark possible notifications on this comment as removed *****/
   Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_TML_COMMENT,PubCod);
   Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_TML_FAV    ,PubCod);
   Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_TML_MENTION,PubCod);

   /***** Remove favs for this comment *****/
   Tml_DB_RemoveCommFavs (PubCod);

   /***** Remove content of this comment *****/
   Tml_DB_RemoveCommContent (PubCod);

   /***** Remove this comment publication *****/
   Tml_DB_RemoveCommPub (PubCod);
  }

/*****************************************************************************/
/******************* Get data of comment using its code **********************/
/*****************************************************************************/

void TmlCom_GetCommDataByCod (struct TmlCom_Comment *Com)
  {
   MYSQL_RES *mysql_res;

   /***** Trivial check: publication code should be > 0 *****/
   if (Com->PubCod <= 0)
     {
      /***** Reset fields of comment *****/
      TmlCom_ResetComm (Com);
      return;
     }

   /***** Get data of comment from database *****/
   if (Tml_DB_GetCommDataByCod (Com->PubCod,&mysql_res))
      TmlCom_GetCommDataFromRow (mysql_res,Com);
   else
      /* Reset fields of comment */
      TmlCom_ResetComm (Com);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Get data of comment from row *************************/
/*****************************************************************************/

static void TmlCom_GetCommDataFromRow (MYSQL_RES *mysql_res,
                                       struct TmlCom_Comment *Com)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);
   /*
   row[0]: PubCod
   row[1]: PublisherCod]
   row[2]: NotCod
   row[3]: TimePublish
   row[4]: Txt
   row[5]: MedCod
   */
   /***** Get code of comment (row[0]), publisher code (row[1])
          and note code (row[2) *****/
   Com->PubCod = Str_ConvertStrCodToLongCod (row[0]);
   Com->UsrCod = Str_ConvertStrCodToLongCod (row[1]);
   Com->NotCod = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get date-time of the note (row[3]) *****/
   Com->DateTimeUTC = Dat_GetUNIXTimeFromStr (row[3]);

   /***** Get text content (row[4]) and media content (row[5]) *****/
   Str_Copy (Com->Content.Txt,row[4],sizeof (Com->Content.Txt) - 1);
   Com->Content.Media.MedCod = Str_ConvertStrCodToLongCod (row[5]);
   Med_GetMediaDataByCod (&Com->Content.Media);

   /***** Get number of times this comment has been favourited *****/
   Com->NumFavs = Tml_DB_GetNumFavers (TmlUsr_FAV_UNF_COMM,
                                       Com->PubCod,Com->UsrCod);
  }

/*****************************************************************************/
/************************** Reset fields of comment **************************/
/*****************************************************************************/

static void TmlCom_ResetComm (struct TmlCom_Comment *Com)
  {
   Com->PubCod         =
   Com->UsrCod         =
   Com->NotCod         = -1L;
   Com->DateTimeUTC    = (time_t) 0;
   Com->Content.Txt[0] = '\0';
   Com->NumFavs        = 0;
  }
