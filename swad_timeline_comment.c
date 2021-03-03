// swad_timeline_comment.c: social timeline comments

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
#include <linux/limits.h>	// For PATH_MAX
#include <stdio.h>		// For asprintf

#include "swad_forum.h"
#include "swad_global.h"
#include "swad_message.h"
#include "swad_photo.h"
#include "swad_profile.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_favourite.h"
#include "swad_timeline_form.h"
#include "swad_timeline_note.h"
#include "swad_timeline_publication.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

#define TL_Com_NUM_VISIBLE_COMMENTS	3	// Maximum number of comments visible before expanding them

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TL_Com_ShowAuthorPhoto (struct UsrData *UsrDat);
static void TL_Com_PutFormToWriteNewComm (const struct TL_Timeline *Timeline,
	                                  long NotCod);

static unsigned TL_Com_WriteHiddenComms (struct TL_Timeline *Timeline,
                                         long NotCod,
				         char IdComms[Frm_MAX_BYTES_ID + 1],
					 unsigned NumInitialCommsToGet);
static void TL_Com_ListComms (const struct TL_Timeline *Timeline,
			      MYSQL_RES *mysql_res,unsigned NumComms);
static void TL_Com_WriteOneCommInList (const struct TL_Timeline *Timeline,
                                       MYSQL_RES *mysql_res);
static void TL_Com_LinkToShowOnlyLatestComms (const char IdComms[Frm_MAX_BYTES_ID + 1]);
static void TL_Com_LinkToShowPreviousComms (const char IdComms[Frm_MAX_BYTES_ID + 1],
				            unsigned NumInitialComms);
static void TL_Com_PutIconToToggleComms (const char *UniqueId,
                                         const char *Icon,const char *Text);
static void TL_Com_CheckAndWriteComm (const struct TL_Timeline *Timeline,
	                              struct TL_Com_Comment *Com);
static void TL_Com_WriteComm (const struct TL_Timeline *Timeline,
	                      struct TL_Com_Comment *Com);
static void TL_Com_WriteAuthorTimeAndContent (struct TL_Com_Comment *Com,
                                              const struct UsrData *UsrDat);
static void TL_Com_WriteAuthorName (const struct UsrData *UsrDat);
static void TL_Com_WriteContent (struct TL_Com_Comment *Com);
static void TL_Com_WriteButtons (const struct TL_Timeline *Timeline,
	                         const struct TL_Com_Comment *Com,
                                 const struct UsrData *UsrDat);

static void TL_Com_PutFormToRemoveComm (const struct TL_Timeline *Timeline,
	                                long PubCod);

static long TL_Com_ReceiveComm (void);

static void TL_Com_RequestRemovalComm (struct TL_Timeline *Timeline);
static void TL_Com_PutParamsRemoveComm (void *Timeline);
static void TL_Com_RemoveComm (void);

static void TL_Com_GetDataOfCommFromRow (MYSQL_ROW row,
                                         struct TL_Com_Comment *Com);

static void TL_Com_ResetComm (struct TL_Com_Comment *Com);

/*****************************************************************************/
/********* Put an icon to toggle on/off the form to comment a note ***********/
/*****************************************************************************/

void TL_Com_PutIconToToggleComm (const char UniqueId[Frm_MAX_BYTES_ID + 1])
  {
   extern const char *Txt_Comment;

   /***** Link to toggle on/off the form to comment a note *****/
   /* Begin container */
   HTM_DIV_Begin ("id=\"%s_ico\" class=\"TL_ICO_COM_OFF\"",UniqueId);

      /* Begin anchor */
      HTM_A_Begin ("href=\"\" onclick=\"toggleNewComment ('%s');"
	                               "return false;\"",
		   UniqueId);

         /* Icon to toggle comment */
	 Ico_PutIcon ("comment-regular.svg",Txt_Comment,"CONTEXT_ICO_16x16");

      /* End anchor */
      HTM_A_End ();

   /* End container */
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Put an icon to toggle on/off the form to comment a note **********/
/*****************************************************************************/

void TL_Com_PutIconCommDisabled (void)
  {
   extern const char *Txt_Comment;

   /***** Disabled icon to comment a note *****/
   /* Begin container */
   HTM_DIV_Begin ("class=\"TL_ICO_COM_OFF TL_ICO_DISABLED\"");

      /* Disabled icon */
      Ico_PutIcon ("edit.svg",Txt_Comment,"ICO16x16");

   /* End container */
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************************* Form to comment a note ****************************/
/*****************************************************************************/

void TL_Com_PutPhotoAndFormToWriteNewComm (const struct TL_Timeline *Timeline,
	                                   long NotCod,
                                           const char IdNewComm[Frm_MAX_BYTES_ID + 1])
  {
   /***** Begin container *****/
   HTM_DIV_Begin ("id=\"%s\" class=\"TL_FORM_NEW_COM TL_RIGHT_WIDTH\""
		  " style=\"display:none;\"",
	          IdNewComm);

      /***** Left: write author's photo (my photo) *****/
      TL_Com_ShowAuthorPhoto (&Gbl.Usrs.Me.UsrDat);

      /***** Right: form to write the comment *****/
      TL_Com_PutFormToWriteNewComm (Timeline,NotCod);

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Show photo of author of a comment *********************/
/*****************************************************************************/

static void TL_Com_ShowAuthorPhoto (struct UsrData *UsrDat)
  {
   /***** Show author's photo *****/
   /* Begin container */
   HTM_DIV_Begin ("class=\"TL_COM_PHOTO\"");

      /* Author's photo */
      Pho_ShowUsrPhotoIfAllowed (UsrDat,"PHOTO30x40",Pho_ZOOM,true);	// Use unique id

   /* End container */
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**************************** Form with textarea *****************************/
/*****************************************************************************/

static void TL_Com_PutFormToWriteNewComm (const struct TL_Timeline *Timeline,
	                                  long NotCod)
  {
   extern const char *Txt_New_TIMELINE_comment;

   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"TL_COM_CONT TL_COMM_WIDTH\"");

      /***** Begin form to write the post *****/
      TL_Frm_BeginForm (Timeline,TL_Frm_RECEIVE_COMM);
      TL_Not_PutHiddenParamNotCod (NotCod);

	 /***** Textarea and button *****/
	 TL_Pst_PutTextarea (Txt_New_TIMELINE_comment,
			     "TL_COM_TEXTAREA TL_COMM_WIDTH");

      /***** End form *****/
      TL_Frm_EndForm ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*********************** Write comments in a note ****************************/
/*****************************************************************************/

void TL_Com_WriteCommsInNote (const struct TL_Timeline *Timeline,
			      const struct TL_Not_Note *Not)
  {
   MYSQL_RES *mysql_res;
   unsigned NumComms;
   unsigned NumInitialComms;
   unsigned NumFinalCommsToGet;
   unsigned NumFinalCommsGot;
   char IdComms[Frm_MAX_BYTES_ID + 1];

   /***** Get number of comments in note *****/
   NumComms = TL_DB_GetNumCommsInNote (Not->NotCod);

   /***** Trivial check: if no comments ==> nothing to do *****/
   if (!NumComms)
      return;

   /***** Compute how many initial comments will be hidden
          and how many final comments will be visible *****/
   // Never hide only one comment
   // So, the number of comments initially hidden must be 0 or >= 2
   if (NumComms <= TL_Com_NUM_VISIBLE_COMMENTS + 1)
     {
      NumInitialComms    = 0;
      NumFinalCommsToGet = NumComms;
     }
   else
     {
      NumInitialComms    = NumComms - TL_Com_NUM_VISIBLE_COMMENTS;
      NumFinalCommsToGet = TL_Com_NUM_VISIBLE_COMMENTS;
     }

   /***** Get final comments of this note from database *****/
   NumFinalCommsGot = TL_DB_GetFinalComms (Not->NotCod,NumFinalCommsToGet,
				           &mysql_res);
   /*
      Before clicking "See prev..."    -->    After clicking "See prev..."
    _________________________________       _________________________________
   |           div con_<id>          |     |           div con_<id>          |
   |            (hidden)             |     |            (visible)            |
   |  _____________________________  |     |  _____________________________  |
   | |    v See only the latest    | |     | |    v See only the latest    | |
   | |_____________________________| |     | |_____________________________| |
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
   | |  _________________________  | |     | |          (hidden)           | |
   | | |          form           | | |     | |                             | |
   | | |  _____________________  | | |     | |    _____________________    | |
   | | | | ^ See prev.comments | | | |     | |   | ^ See prev.comments |   | |
   | | | |_____________________| | | |     | |   |_____________________|   | |
   | | |_________________________| | |     | |                             | |
   | |_____________________________| |     | |_____________________________| |
   |_________________________________|     |_________________________________|
    _________________________________       _________________________________
   |           ul com_<id>           |     |           ul com_<id>           |
   |    _________________________    |     |    _________________________    |
   |   |     li (comment 1)      |   |     |   |     li (comment 1)      |   |
   |   |_________________________|   |     |   |_________________________|   |
   |   |           ...           |   |     |   |           ...           |   |
   |   |_________________________|   |     |   |_________________________|   |
   |   |     li (comment n)      |   |     |   |     li (comment n)      |   |
   |   |_________________________|   |     |   |_________________________|   |
   |_________________________________|     |_________________________________|
   */
   /***** Link to show initial hidden comments *****/
   if (NumInitialComms)
     {
      /***** Create unique id for list of hidden comments *****/
      Frm_SetUniqueId (IdComms);

      /***** Link (initially hidden) to show only the latest comments *****/
      TL_Com_LinkToShowOnlyLatestComms (IdComms);

      /***** Div which content will be updated via AJAX *****/
      HTM_DIV_Begin ("id=\"%s\" class=\"TL_RIGHT_WIDTH\"",IdComms);
	 TL_Frm_FormToShowHiddenComms (Not->NotCod,IdComms,NumInitialComms);
      HTM_DIV_End ();
     }

   /***** List final visible comments *****/
   if (NumFinalCommsGot)
     {
      HTM_UL_Begin ("class=\"TL_LIST\"");
	 TL_Com_ListComms (Timeline,mysql_res,NumFinalCommsGot);
      HTM_UL_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Write hidden comments via AJAX ***********************/
/*****************************************************************************/

void TL_Com_ShowHiddenCommsUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show hidden comments *****/
   TL_Com_ShowHiddenCommsGbl ();
  }

void TL_Com_ShowHiddenCommsGbl (void)
  {
   struct TL_Timeline Timeline;
   long NotCod;
   char IdComms[Frm_MAX_BYTES_ID + 1];
   unsigned NumInitialCommsToGet;
   unsigned NumInitialCommsGot;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get parameters *****/
   /* Get note code, identifier and number of comments to get */
   NotCod = TL_Not_GetParamNotCod ();
   Par_GetParToText ("IdComments",IdComms,Frm_MAX_BYTES_ID);
   NumInitialCommsToGet = (unsigned) Par_GetParToLong ("NumHidCom");

   /***** Write HTML inside DIV with hidden comments *****/
   NumInitialCommsGot =
   TL_Com_WriteHiddenComms (&Timeline,NotCod,IdComms,NumInitialCommsToGet);

   /***** Link to show the first comments *****/
   TL_Com_LinkToShowPreviousComms (IdComms,NumInitialCommsGot);
  }

/*****************************************************************************/
/**************************** Write hidden comments **************************/
/*****************************************************************************/
// Returns the number of comments got

static unsigned TL_Com_WriteHiddenComms (struct TL_Timeline *Timeline,
                                         long NotCod,
				         char IdComms[Frm_MAX_BYTES_ID + 1],
					 unsigned NumInitialCommsToGet)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumInitialCommsGot;

   /***** Get comments of this note from database *****/
   NumInitialCommsGot = TL_DB_GetInitialComms (NotCod,
				               NumInitialCommsToGet,
				               &mysql_res);

   /***** List comments *****/
   HTM_UL_Begin ("id=\"com_%s\" class=\"TL_LIST\"",IdComms);
      TL_Com_ListComms (Timeline,mysql_res,NumInitialCommsGot);
   HTM_UL_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumInitialCommsGot;
  }

/*****************************************************************************/
/*********************** Write comments in a note ****************************/
/*****************************************************************************/

static void TL_Com_ListComms (const struct TL_Timeline *Timeline,
			      MYSQL_RES *mysql_res,unsigned NumComms)
  {
   unsigned NumCom;

   /***** List comments one by one *****/
   for (NumCom = 0;
	NumCom < NumComms;
	NumCom++)
      TL_Com_WriteOneCommInList (Timeline,mysql_res);
  }

/*****************************************************************************/
/************************* Write a comment in list ***************************/
/*****************************************************************************/

static void TL_Com_WriteOneCommInList (const struct TL_Timeline *Timeline,
                                       MYSQL_RES *mysql_res)
  {
   MYSQL_ROW row;
   struct TL_Com_Comment Com;

   /***** Initialize image *****/
   Med_MediaConstructor (&Com.Content.Media);

   /***** Get data of comment *****/
   row = mysql_fetch_row (mysql_res);
   TL_Com_GetDataOfCommFromRow (row,&Com);

   /***** Write comment *****/
   HTM_LI_Begin ("class=\"TL_COM\"");
      TL_Com_CheckAndWriteComm (Timeline,&Com);
   HTM_LI_End ();

   /***** Free image *****/
   Med_MediaDestructor (&Com.Content.Media);
  }

/*****************************************************************************/
/****************** Link to show only the latest comments ********************/
/*****************************************************************************/

static void TL_Com_LinkToShowOnlyLatestComms (const char IdComms[Frm_MAX_BYTES_ID + 1])
  {
   extern const char *Txt_See_only_the_latest_COMMENTS;

   /***** Icon and text to show only the latest comments ****/
   HTM_DIV_Begin ("id=\"con_%s\" class=\"TL_EXPAND_COM TL_RIGHT_WIDTH\""
		  " style=\"display:none;\"",	// Hidden
		  IdComms);
      TL_Com_PutIconToToggleComms (IdComms,"angle-down.svg",
				   Txt_See_only_the_latest_COMMENTS);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Link to show the first comments ***********************/
/*****************************************************************************/

static void TL_Com_LinkToShowPreviousComms (const char IdComms[Frm_MAX_BYTES_ID + 1],
				            unsigned NumInitialComms)
  {
   extern const char *Txt_See_the_previous_X_COMMENTS;

   /***** Icon and text to show only the latest comments ****/
   HTM_DIV_Begin ("id=\"exp_%s\" class=\"TL_EXPAND_COM TL_RIGHT_WIDTH\""
	          " style=\"display:none;\"",	// Hidden
		  IdComms);
      TL_Com_PutIconToToggleComms (IdComms,"angle-up.svg",
				   Str_BuildStringLong (Txt_See_the_previous_X_COMMENTS,
							(long) NumInitialComms));
      Str_FreeString ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Put an icon to toggle on/off comments in a publication ***********/
/*****************************************************************************/

static void TL_Com_PutIconToToggleComms (const char *UniqueId,
                                         const char *Icon,const char *Text)
  {
   extern const char *The_ClassFormLinkInBox[The_NUM_THEMES];
   char *OnClick;

   if (asprintf (&OnClick,"toggleComments('%s')",UniqueId) < 0)
      Lay_NotEnoughMemoryExit ();

   /***** Link to toggle on/off some divs *****/
   HTM_BUTTON_BUTTON_Begin (Text,The_ClassFormLinkInBox[Gbl.Prefs.Theme],OnClick);
      Ico_PutIconTextLink (Icon,Text);
   HTM_BUTTON_End ();

   free (OnClick);
  }

/*****************************************************************************/
/************************** Check and write comment **************************/
/*****************************************************************************/

static void TL_Com_CheckAndWriteComm (const struct TL_Timeline *Timeline,
	                              struct TL_Com_Comment *Com)
  {
   /*__________________________________________
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
   TL_Com_WriteComm (Timeline,Com);
  }

/*****************************************************************************/
/******************************** Write comment ******************************/
/*****************************************************************************/

static void TL_Com_WriteComm (const struct TL_Timeline *Timeline,
	                      struct TL_Com_Comment *Com)
  {
   struct UsrData UsrDat;	// Author of the comment

   /***** Get author's data *****/
   Usr_UsrDataConstructor (&UsrDat);
   UsrDat.UsrCod = Com->UsrCod;
   Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);

   /***** Left: author's photo *****/
   TL_Com_ShowAuthorPhoto (&UsrDat);

   /***** Right: author's name, time, content, and buttons *****/
   /* Begin container */
   HTM_DIV_Begin ("class=\"TL_COM_CONT TL_COMM_WIDTH\"");

      /* Right top: author's name, time, and content */
      TL_Com_WriteAuthorTimeAndContent (Com,&UsrDat);

      /* Right bottom: buttons */
      TL_Com_WriteButtons (Timeline,Com,&UsrDat);

   /* End container */
   HTM_DIV_End ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/**** Write top right part of a note: author's name, time and note content ***/
/*****************************************************************************/

static void TL_Com_WriteAuthorTimeAndContent (struct TL_Com_Comment *Com,
                                              const struct UsrData *UsrDat)	// Author
  {
   /***** Write author's full name and nickname *****/
   TL_Com_WriteAuthorName (UsrDat);

   /***** Write date and time *****/
   TL_WriteDateTime (Com->DateTimeUTC);

   /***** Write content of the comment *****/
   TL_Com_WriteContent (Com);
  }

/*****************************************************************************/
/*************** Write name of author of a comment to a note *****************/
/*****************************************************************************/

static void TL_Com_WriteAuthorName (const struct UsrData *UsrDat)	// Author
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;

   /***** Show user's name inside form to go to user's public profile *****/
   /* Begin form */
   Frm_BeginFormUnique (ActSeeOthPubPrf);
   Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);

      /* Author's name */
      HTM_BUTTON_SUBMIT_Begin (Usr_ItsMe (UsrDat->UsrCod) ? Txt_My_public_profile :
							    Txt_Another_user_s_profile,
			       "BT_LINK TL_COM_AUTHOR TL_COMM_AUTHOR_WIDTH DAT_BOLD",NULL);
	 HTM_Txt (UsrDat->FullName);
      HTM_BUTTON_End ();

   /* End form */
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************* Write content of comment **************************/
/*****************************************************************************/

static void TL_Com_WriteContent (struct TL_Com_Comment *Com)
  {
   /***** Write content of the comment *****/
   if (Com->Content.Txt[0])
     {
      HTM_DIV_Begin ("class=\"TL_TXT\"");
	 Msg_WriteMsgContent (Com->Content.Txt,Cns_MAX_BYTES_LONG_TEXT,true,false);
      HTM_DIV_End ();
     }

   /***** Show image *****/
   Med_ShowMedia (&Com->Content.Media,"TL_COM_MED_CONT TL_COMM_WIDTH",
				      "TL_COM_MED TL_COMM_WIDTH");
  }

/*****************************************************************************/
/********************* Write bottom part of a comment ************************/
/*****************************************************************************/

static void TL_Com_WriteButtons (const struct TL_Timeline *Timeline,
	                         const struct TL_Com_Comment *Com,
                                 const struct UsrData *UsrDat)	// Author
  {
   static unsigned NumDiv = 0;	// Used to create unique div id for fav

   NumDiv++;

   /***** Begin buttons container *****/
   HTM_DIV_Begin ("class=\"TL_FOOT TL_COMM_WIDTH\"");

      /***** Foot column 1: fav zone *****/
      HTM_DIV_Begin ("id=\"fav_com_%s_%u\" class=\"TL_FAV_COM TL_FAV_WIDTH\"",
		     Gbl.UniqueNameEncrypted,NumDiv);
	 TL_Usr_PutIconFavSha (TL_Usr_FAV_UNF_COMM,
	                       Com->PubCod,Com->UsrCod,Com->NumFavs,
			       TL_Usr_SHOW_FEW_USRS);

      HTM_DIV_End ();

      /***** Foot column 2: icon to remove this comment *****/
      HTM_DIV_Begin ("class=\"TL_REM\"");
      if (Usr_ItsMe (UsrDat->UsrCod))	// I am the author
	 TL_Com_PutFormToRemoveComm (Timeline,Com->PubCod);
      HTM_DIV_End ();

   /***** End buttons container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************************* Form to remove comment ****************************/
/*****************************************************************************/

static void TL_Com_PutFormToRemoveComm (const struct TL_Timeline *Timeline,
	                                   long PubCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove publication *****/
   /* Begin form */
   TL_Frm_BeginForm (Timeline,TL_Frm_REQ_REM_COMM);
   TL_Pub_PutHiddenParamPubCod (PubCod);

      /* Icon to remove */
      Ico_PutIconLink ("trash.svg",Txt_Remove);

   /* End form */
   TL_Frm_EndForm ();
  }

/*****************************************************************************/
/******************************* Comment a note ******************************/
/*****************************************************************************/

void TL_Com_ReceiveCommUsr (void)
  {
   struct TL_Timeline Timeline;
   long NotCod;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Begin section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

      /***** Receive comment in a note
	     and write updated timeline after commenting (user) *****/
      NotCod = TL_Com_ReceiveComm ();
      TL_ShowTimelineUsrHighlighting (&Timeline,NotCod);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_Com_ReceiveCommGbl (void)
  {
   struct TL_Timeline Timeline;
   long NotCod;

   /***** Initialize timeline *****/
   TL_InitTimelineGbl (&Timeline);

   /***** Receive comment in a note *****/
   NotCod = TL_Com_ReceiveComm ();

   /***** Write updated timeline after commenting (global) *****/
   TL_ShowTimelineGblHighlighting (&Timeline,NotCod);
  }

static long TL_Com_ReceiveComm (void)
  {
   extern const char *Txt_The_post_no_longer_exists;
   struct TL_Pst_PostContent Content;
   struct TL_Not_Note Not;
   struct TL_Pub_Publication Pub;

   /***** Get data of note *****/
   Not.NotCod = TL_Not_GetParamNotCod ();
   TL_Not_GetDataOfNoteByCod (&Not);

   /***** Trivial check: note code *****/
   if (Not.NotCod <= 0)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_The_post_no_longer_exists);
      return -1L;
     }

   /***** Get the content of the comment *****/
   Par_GetParAndChangeFormat ("Txt",Content.Txt,Cns_MAX_BYTES_LONG_TEXT,
			      Str_TO_RIGOROUS_HTML,true);

   /***** Initialize image *****/
   Med_MediaConstructor (&Content.Media);

   /***** Get attached image (action, file and title) *****/
   Content.Media.Width   = TL_IMAGE_SAVED_MAX_WIDTH;
   Content.Media.Height  = TL_IMAGE_SAVED_MAX_HEIGHT;
   Content.Media.Quality = TL_IMAGE_SAVED_QUALITY;
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
      Pub.PubType      = TL_Pub_COMMENT_TO_NOTE;
      TL_Pub_PublishPubInTimeline (&Pub);	// Set Pub.PubCod

      /* Insert comment content in the database */
      TL_DB_InsertCommContent (Pub.PubCod,&Content);

      /***** Store notifications about the new comment *****/
      Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_TL_COMMENT,Pub.PubCod);

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

void TL_Com_RequestRemComUsr (void)
  {
   struct TL_Timeline Timeline;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Begin section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

      /***** Request the removal of comment in note *****/
      TL_Com_RequestRemovalComm (&Timeline);

      /***** Write timeline again (user) *****/
      TL_ShowTimelineUsr (&Timeline);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_Com_RequestRemComGbl (void)
  {
   struct TL_Timeline Timeline;

   /***** Initialize timeline *****/
   TL_InitTimelineGbl (&Timeline);

   /***** Request the removal of comment in note *****/
   TL_Com_RequestRemovalComm (&Timeline);

   /***** Write timeline again (global) *****/
   TL_ShowNoteAndTimelineGbl (&Timeline);
  }

static void TL_Com_RequestRemovalComm (struct TL_Timeline *Timeline)
  {
   extern const char *Txt_The_post_no_longer_exists;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_comment;
   struct TL_Com_Comment Com;

   /***** Initialize media *****/
   Med_MediaConstructor (&Com.Content.Media);

   /***** Get data of comment *****/
   Com.PubCod = TL_Pub_GetParamPubCod ();
   TL_Com_GetDataOfCommByCod (&Com);

   /***** Trivial check 1: publication code *****/
   if (Com.PubCod <= 0)
     {
      Med_MediaDestructor (&Com.Content.Media);
      Ale_ShowAlert (Ale_WARNING,Txt_The_post_no_longer_exists);
      return;
     }

   /***** Trivial check 2: only if I am the author of this comment *****/
   if (!Usr_ItsMe (Com.UsrCod))
     {
      Med_MediaDestructor (&Com.Content.Media);
      Ale_ShowAlert (Ale_ERROR,"You are not the author.");
      return;
     }

   /***** Show question and button to remove comment *****/
   /* Begin alert */
   TL_Frm_BeginAlertRemove (Txt_Do_you_really_want_to_remove_the_following_comment);

      /* Begin box for the comment */
      Box_BoxBegin (NULL,NULL,
		    NULL,NULL,
		    NULL,Box_NOT_CLOSABLE);

         /* Indent the comment */
	 HTM_DIV_Begin ("class=\"TL_LEFT_PHOTO\"");
	 HTM_DIV_End ();

	 /* Show the comment */
	 HTM_DIV_Begin ("class=\"TL_RIGHT_CONT TL_RIGHT_WIDTH\"");
	    TL_Com_CheckAndWriteComm (Timeline,&Com);
	 HTM_DIV_End ();

      /* End box */
      Box_BoxEnd ();

   /* End alert */
   Timeline->PubCod = Com.PubCod;	// Publication to be removed
   TL_Frm_EndAlertRemove (Timeline,TL_Frm_REM_COMM,
			  TL_Com_PutParamsRemoveComm);

   /***** Free media *****/
   Med_MediaDestructor (&Com.Content.Media);
  }

/*****************************************************************************/
/******************** Put parameters to remove a comment *********************/
/*****************************************************************************/

static void TL_Com_PutParamsRemoveComm (void *Timeline)
  {
   if (Timeline)
     {
      if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)	// User's timeline
	 Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
      else					// Global timeline
	 Usr_PutHiddenParamWho (((struct TL_Timeline *) Timeline)->Who);
      TL_Pub_PutHiddenParamPubCod (((struct TL_Timeline *) Timeline)->PubCod);
     }
  }

/*****************************************************************************/
/***************************** Remove a comment ******************************/
/*****************************************************************************/

void TL_Com_RemoveComUsr (void)
  {
   struct TL_Timeline Timeline;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Begin section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

      /***** Remove a comment *****/
      TL_Com_RemoveComm ();

      /***** Write updated timeline after removing (user) *****/
      TL_ShowTimelineUsr (&Timeline);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_Com_RemoveComGbl (void)
  {
   struct TL_Timeline Timeline;

   /***** Initialize timeline *****/
   TL_InitTimelineGbl (&Timeline);

   /***** Remove a comment *****/
   TL_Com_RemoveComm ();

   /***** Write updated timeline after removing (global) *****/
   TL_ShowNoteAndTimelineGbl (&Timeline);
  }

static void TL_Com_RemoveComm (void)
  {
   extern const char *Txt_The_post_no_longer_exists;
   extern const char *Txt_Comment_removed;
   struct TL_Com_Comment Com;

   /***** Initialize media *****/
   Med_MediaConstructor (&Com.Content.Media);

   /***** Get data of comment *****/
   Com.PubCod = TL_Pub_GetParamPubCod ();
   TL_Com_GetDataOfCommByCod (&Com);

   /***** Trivial check 1: publication code *****/
   if (Com.PubCod <= 0)
     {
      Med_MediaDestructor (&Com.Content.Media);
      Ale_ShowAlert (Ale_WARNING,Txt_The_post_no_longer_exists);
      return;
     }

   /***** Trivial check 2: only if I am the author of this comment *****/
   if (!Usr_ItsMe (Com.UsrCod))
     {
      Med_MediaDestructor (&Com.Content.Media);
      Ale_ShowAlert (Ale_ERROR,"You are not the author.");
      return;
     }

   /***** Remove media associated to comment
	  and delete comment from database *****/
   TL_Com_RemoveCommMediaAndDBEntries (Com.PubCod);

   /***** Free media *****/
   Med_MediaDestructor (&Com.Content.Media);

   /***** Reset fields of comment *****/
   TL_Com_ResetComm (&Com);

   /***** Message of success *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Comment_removed);
  }

/*****************************************************************************/
/*************** Remove comment media and database entries *******************/
/*****************************************************************************/

void TL_Com_RemoveCommMediaAndDBEntries (long PubCod)
  {
   /***** Remove media associated to comment *****/
   Med_RemoveMedia (TL_DB_GetMedCodFromComm (PubCod));

   /***** Mark possible notifications on this comment as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_TL_COMMENT,PubCod);
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_TL_FAV    ,PubCod);
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_TL_MENTION,PubCod);

   /***** Remove favs for this comment *****/
   TL_DB_RemoveCommFavs (PubCod);

   /***** Remove content of this comment *****/
   TL_DB_RemoveCommContent (PubCod);

   /***** Remove this comment publication *****/
   TL_DB_RemoveCommPub (PubCod);
  }

/*****************************************************************************/
/******************* Get data of comment using its code **********************/
/*****************************************************************************/

void TL_Com_GetDataOfCommByCod (struct TL_Com_Comment *Com)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Trivial check: publication code should be > 0 *****/
   if (Com->PubCod <= 0)
     {
      /***** Reset fields of comment *****/
      TL_Com_ResetComm (Com);
      return;
     }

   /***** Get data of comment from database *****/
   if (TL_DB_GetDataOfCommByCod (Com->PubCod,&mysql_res))
     {
      /* Get data of comment */
      row = mysql_fetch_row (mysql_res);
      TL_Com_GetDataOfCommFromRow (row,Com);
     }
   else
      /* Reset fields of comment */
      TL_Com_ResetComm (Com);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Get data of comment from row *************************/
/*****************************************************************************/

static void TL_Com_GetDataOfCommFromRow (MYSQL_ROW row,
                                         struct TL_Com_Comment *Com)
  {
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
   Com->NumFavs = TL_DB_GetNumFavers (TL_Usr_FAV_UNF_COMM,
                                      Com->PubCod,Com->UsrCod);
  }

/*****************************************************************************/
/************************** Reset fields of comment **************************/
/*****************************************************************************/

static void TL_Com_ResetComm (struct TL_Com_Comment *Com)
  {
   Com->PubCod         =
   Com->UsrCod         =
   Com->NotCod         = -1L;
   Com->DateTimeUTC    = (time_t) 0;
   Com->Content.Txt[0] = '\0';
   Com->NumFavs        = 0;
  }
