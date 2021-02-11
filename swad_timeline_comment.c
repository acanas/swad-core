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

#include "swad_database.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_message.h"
#include "swad_photo.h"
#include "swad_profile.h"
#include "swad_timeline.h"
#include "swad_timeline_favourite.h"
#include "swad_timeline_note.h"
#include "swad_timeline_publication.h"
#include "swad_timeline_share.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

#define TL_COM_NUM_VISIBLE_COMMENTS	3	// Maximum number of comments visible before expanding them

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

static void TL_Com_FormToShowHiddenComments (Act_Action_t ActionGbl,Act_Action_t ActionUsr,
			                     long NotCod,
					     char IdComments[Frm_MAX_BYTES_ID + 1],
					     unsigned NumInitialComments);
static unsigned TL_Com_WriteHiddenComments (struct TL_Timeline *Timeline,
                                            long NotCod,
				            char IdComments[Frm_MAX_BYTES_ID + 1],
					    unsigned NumInitialCommentsToGet);
static void TL_Com_WriteOneCommentInList (struct TL_Timeline *Timeline,
                                          MYSQL_RES *mysql_res);
static void TL_Com_LinkToShowOnlyLatestComments (const char IdComments[Frm_MAX_BYTES_ID + 1]);
static void TL_Com_LinkToShowPreviousComments (const char IdComments[Frm_MAX_BYTES_ID + 1],
				               unsigned NumInitialComments);
static void TL_Com_PutIconToToggleComments (const char *UniqueId,
                                            const char *Icon,const char *Text);
static void TL_Com_WriteComment (struct TL_Timeline *Timeline,
	                         struct TL_Com_Comment *Com,
                                 TL_ShowAlone_t ShowCommentAlone);	// Comment is shown alone, not in a list
static void TL_Com_WriteAuthorComment (struct UsrData *UsrDat);

static void TL_Com_PutFormToRemoveComment (const struct TL_Timeline *Timeline,
	                                   long PubCod);

static long TL_Com_ReceiveComment (void);

static void TL_Com_RequestRemovalComment (struct TL_Timeline *Timeline);
static void TL_Com_PutParamsRemoveComment (void *Timeline);
static void TL_Com_RemoveComment (void);

static void TL_Com_GetDataOfCommentFromRow (MYSQL_ROW row,struct TL_Com_Comment *Com);

static void TL_Com_ResetComment (struct TL_Com_Comment *Com);

/*****************************************************************************/
/********* Put an icon to toggle on/off the form to comment a note ***********/
/*****************************************************************************/

void TL_Com_PutIconToToggleComment (const char UniqueId[Frm_MAX_BYTES_ID + 1])
  {
   extern const char *Txt_Comment;

   /***** Link to toggle on/off the form to comment a note *****/
   HTM_DIV_Begin ("id=\"%s_ico\" class=\"TL_ICO_COM_OFF\"",UniqueId);
   HTM_A_Begin ("href=\"\" onclick=\"toggleNewComment ('%s');return false;\"",
                UniqueId);
   Ico_PutIcon ("comment-regular.svg",Txt_Comment,"CONTEXT_ICO_16x16");
   HTM_A_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Put an icon to toggle on/off the form to comment a note **********/
/*****************************************************************************/

void TL_Com_PutIconCommentDisabled (void)
  {
   extern const char *Txt_Comment;

   /***** Disabled icon to comment a note *****/
   HTM_DIV_Begin ("class=\"TL_ICO_COM_OFF TL_ICO_DISABLED\"");
   Ico_PutIcon ("edit.svg",Txt_Comment,"ICO16x16");
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************************* Form to comment a note ****************************/
/*****************************************************************************/

void TL_Com_PutHiddenFormToWriteNewComment (const struct TL_Timeline *Timeline,
	                                    long NotCod,
                                            const char IdNewComment[Frm_MAX_BYTES_ID + 1])
  {
   extern const char *Txt_New_TIMELINE_comment;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX + 1];

   /***** Start container *****/
   HTM_DIV_Begin ("id=\"%s\" class=\"TL_FORM_NEW_COM TL_RIGHT_WIDTH\""
		  " style=\"display:none;\"",
	          IdNewComment);

   /***** Left: write author's photo (my photo) *****/
   HTM_DIV_Begin ("class=\"TL_COM_PHOTO\"");
   ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&Gbl.Usrs.Me.UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (&Gbl.Usrs.Me.UsrDat,ShowPhoto ? PhotoURL :
					             NULL,
		     "PHOTO30x40",Pho_ZOOM,true);	// Use unique id
   HTM_DIV_End ();

   /***** Right: form to write the comment *****/
   /* Start right container */
   HTM_DIV_Begin ("class=\"TL_COM_CONT TL_COMM_WIDTH\"");

   /* Begin form to write the post */
   TL_FormStart (Timeline,ActRcvTL_ComGbl,
                          ActRcvTL_ComUsr);
   TL_Not_PutHiddenParamNotCod (NotCod);

   /* Textarea and button */
   TL_Pst_PutTextarea (Txt_New_TIMELINE_comment,
	           "TL_COM_TEXTAREA TL_COMM_WIDTH");

   /* End form */
   Frm_EndForm ();

   /* End right container */
   HTM_DIV_End ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Get number of comments in a note **********************/
/*****************************************************************************/

unsigned long TL_Com_GetNumCommentsInNote (long NotCod)
  {
   return DB_QueryCOUNT ("can not get number of comments in a note",
			 "SELECT COUNT(*) FROM tl_pubs"
			 " WHERE NotCod=%ld AND PubType=%u",
			 NotCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/*********************** Write comments in a note ****************************/
/*****************************************************************************/

void TL_Com_WriteCommentsInNote (struct TL_Timeline *Timeline,
				 const struct TL_Not_Note *Not,
				 unsigned NumComments)
  {
   MYSQL_RES *mysql_res;
   unsigned NumInitialComments;
   unsigned NumFinalCommentsToGet;
   unsigned NumFinalCommentsGot;
   unsigned NumCom;
   char IdComments[Frm_MAX_BYTES_ID + 1];

   /***** Compute how many initial comments will be hidden
          and how many final comments will be visible *****/
   // Never hide only one comment
   // So, the number of comments initially hidden must be 0 or >= 2
   if (NumComments <= TL_COM_NUM_VISIBLE_COMMENTS + 1)
     {
      NumInitialComments    = 0;
      NumFinalCommentsToGet = NumComments;
     }
   else
     {
      NumInitialComments    = NumComments - TL_COM_NUM_VISIBLE_COMMENTS;
      NumFinalCommentsToGet = TL_COM_NUM_VISIBLE_COMMENTS;
     }

   /***** Get last comments of this note from database *****/
   NumFinalCommentsGot = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get comments",
			      "SELECT * FROM "
			      "("
			      "SELECT tl_pubs.PubCod,"		// row[0]
				     "tl_pubs.PublisherCod,"	// row[1]
				     "tl_pubs.NotCod,"		// row[2]
				     "UNIX_TIMESTAMP("
				     "tl_pubs.TimePublish),"	// row[3]
				     "tl_comments.Txt,"		// row[4]
				     "tl_comments.MedCod"	// row[5]
			      " FROM tl_pubs,tl_comments"
			      " WHERE tl_pubs.NotCod=%ld"
			      " AND tl_pubs.PubType=%u"
			      " AND tl_pubs.PubCod=tl_comments.PubCod"
			      " ORDER BY tl_pubs.PubCod DESC LIMIT %u"
			      ") AS comments"
			      " ORDER BY PubCod",
			      Not->NotCod,(unsigned) TL_Pub_COMMENT_TO_NOTE,
			      NumFinalCommentsToGet);

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
   if (NumInitialComments)
     {
      /***** Create unique id for list of hidden comments *****/
      Frm_SetUniqueId (IdComments);

      /***** Link (initially hidden) to show only the latest comments *****/
      TL_Com_LinkToShowOnlyLatestComments (IdComments);

      /***** Div which content will be updated via AJAX *****/
      HTM_DIV_Begin ("id=\"%s\" class=\"TL_RIGHT_WIDTH\"",IdComments);
      TL_Com_FormToShowHiddenComments (ActShoHidTL_ComGbl,ActShoHidTL_ComUsr,
				   Not->NotCod,
				   IdComments,
				   NumInitialComments);
      HTM_DIV_End ();
     }

   /***** List final visible comments *****/
   if (NumFinalCommentsGot)
     {
      HTM_UL_Begin ("class=\"TL_LIST\"");
      for (NumCom = 0;
	   NumCom < NumFinalCommentsGot;
	   NumCom++)
	 TL_Com_WriteOneCommentInList (Timeline,mysql_res);
      HTM_UL_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********** Form to show hidden coments in global or user timeline ***********/
/*****************************************************************************/

static void TL_Com_FormToShowHiddenComments (Act_Action_t ActionGbl,Act_Action_t ActionUsr,
			                     long NotCod,
					     char IdComments[Frm_MAX_BYTES_ID + 1],
					     unsigned NumInitialComments)
  {
   extern const char *The_ClassFormLinkInBox[The_NUM_THEMES];
   extern const char *Txt_See_the_previous_X_COMMENTS;
   char *OnSubmit;

   HTM_DIV_Begin ("id=\"exp_%s\" class=\"TL_EXPAND_COM TL_RIGHT_WIDTH\"",
		  IdComments);

   /***** Form and icon-text to show hidden comments *****/
   /* Begin form */
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      if (asprintf (&OnSubmit,"toggleComments('%s');"
	                      "updateDivHiddenComments(this,"
			      "'act=%ld&ses=%s&NotCod=%ld&IdComments=%s&NumHidCom=%u&OtherUsrCod=%s');"
			      " return false;",	// return false is necessary to not submit form
		    IdComments,
		    Act_GetActCod (ActionUsr),
		    Gbl.Session.Id,
		    NotCod,
		    IdComments,
		    NumInitialComments,
		    Gbl.Usrs.Other.UsrDat.EncryptedUsrCod) < 0)
	 Lay_NotEnoughMemoryExit ();
      Frm_StartFormUniqueAnchorOnSubmit (ActUnk,"timeline",OnSubmit);
     }
   else
     {
      if (asprintf (&OnSubmit,"toggleComments('%s');"
	                      "updateDivHiddenComments(this,"
			      "'act=%ld&ses=%s&NotCod=%ld&IdComments=%s&NumHidCom=%u');"
			      " return false;",	// return false is necessary to not submit form
		    IdComments,
		    Act_GetActCod (ActionGbl),
		    Gbl.Session.Id,
		    NotCod,
		    IdComments,
		    NumInitialComments) < 0)
	 Lay_NotEnoughMemoryExit ();
      Frm_StartFormUniqueAnchorOnSubmit (ActUnk,NULL,OnSubmit);
     }

   /* Put icon and text with link to show the first hidden comments */
   HTM_BUTTON_SUBMIT_Begin (NULL,The_ClassFormLinkInBox[Gbl.Prefs.Theme],NULL);
   Ico_PutIconTextLink ("angle-up.svg",
			Str_BuildStringLong (Txt_See_the_previous_X_COMMENTS,
					     (long) NumInitialComments));
   Str_FreeString ();
   HTM_BUTTON_End ();

   /* End form */
   Frm_EndForm ();

   /* Free allocated memory */
   free (OnSubmit);

   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************** Write hidden comments via AJAX ***********************/
/*****************************************************************************/

void TL_Com_ShowHiddenCommentsUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show hidden comments *****/
   TL_Com_ShowHiddenCommentsGbl ();
  }

void TL_Com_ShowHiddenCommentsGbl (void)
  {
   struct TL_Timeline Timeline;
   long NotCod;
   char IdComments[Frm_MAX_BYTES_ID + 1];
   unsigned NumInitialCommentsToGet;
   unsigned NumInitialCommentsGot;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get parameters *****/
   /* Get note code */
   NotCod = TL_Not_GetParamNotCod ();

   /* Get identifier */
   Par_GetParToText ("IdComments",IdComments,Frm_MAX_BYTES_ID);

   /* Get number of comments to get */
   NumInitialCommentsToGet = (unsigned) Par_GetParToLong ("NumHidCom");

   /***** Write HTML inside DIV with hidden comments *****/
   NumInitialCommentsGot = TL_Com_WriteHiddenComments (&Timeline,
                                                   NotCod,IdComments,NumInitialCommentsToGet);

   /***** Link to show the first comments *****/
   TL_Com_LinkToShowPreviousComments (IdComments,NumInitialCommentsGot);
  }

/*****************************************************************************/
/**************************** Write hidden comments **************************/
/*****************************************************************************/
// Returns the number of comments got

static unsigned TL_Com_WriteHiddenComments (struct TL_Timeline *Timeline,
                                            long NotCod,
				            char IdComments[Frm_MAX_BYTES_ID + 1],
					    unsigned NumInitialCommentsToGet)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumInitialCommentsGot;
   unsigned long NumCom;

   /***** Get comments of this note from database *****/
   NumInitialCommentsGot = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get comments",
		   "SELECT tl_pubs.PubCod,"		// row[0]
			  "tl_pubs.PublisherCod,"	// row[1]
			  "tl_pubs.NotCod,"		// row[2]
			  "UNIX_TIMESTAMP("
			  "tl_pubs.TimePublish),"	// row[3]
			  "tl_comments.Txt,"		// row[4]
			  "tl_comments.MedCod"		// row[5]
		   " FROM tl_pubs,tl_comments"
		   " WHERE tl_pubs.NotCod=%ld"
		   " AND tl_pubs.PubType=%u"
		   " AND tl_pubs.PubCod=tl_comments.PubCod"
		   " ORDER BY tl_pubs.PubCod"
		   " LIMIT %lu",
		   NotCod,(unsigned) TL_Pub_COMMENT_TO_NOTE,
		   NumInitialCommentsToGet);

   /***** List with comments *****/
   HTM_UL_Begin ("id=\"com_%s\" class=\"TL_LIST\"",IdComments);
   for (NumCom = 0;
	NumCom < NumInitialCommentsGot;
	NumCom++)
      TL_Com_WriteOneCommentInList (Timeline,mysql_res);
   HTM_UL_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumInitialCommentsGot;
  }

/*****************************************************************************/
/************************* Write a comment in list ***************************/
/*****************************************************************************/

static void TL_Com_WriteOneCommentInList (struct TL_Timeline *Timeline,
                                          MYSQL_RES *mysql_res)
  {
   MYSQL_ROW row;
   struct TL_Com_Comment Com;

   /***** Initialize image *****/
   Med_MediaConstructor (&Com.Content.Media);

   /***** Get data of comment *****/
   row = mysql_fetch_row (mysql_res);
   TL_Com_GetDataOfCommentFromRow (row,&Com);

   /***** Write comment *****/
   TL_Com_WriteComment (Timeline,&Com,
		        TL_DONT_SHOW_ALONE);

   /***** Free image *****/
   Med_MediaDestructor (&Com.Content.Media);
  }

/*****************************************************************************/
/****************** Link to show only the latest comments ********************/
/*****************************************************************************/

static void TL_Com_LinkToShowOnlyLatestComments (const char IdComments[Frm_MAX_BYTES_ID + 1])
  {
   extern const char *Txt_See_only_the_latest_COMMENTS;

   /***** Icon and text to show only the latest comments ****/
   HTM_DIV_Begin ("id=\"con_%s\" class=\"TL_EXPAND_COM TL_RIGHT_WIDTH\""
		  " style=\"display:none;\"",	// Hidden
		  IdComments);
   TL_Com_PutIconToToggleComments (IdComments,"angle-down.svg",
			           Txt_See_only_the_latest_COMMENTS);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Link to show the first comments ***********************/
/*****************************************************************************/

static void TL_Com_LinkToShowPreviousComments (const char IdComments[Frm_MAX_BYTES_ID + 1],
				               unsigned NumInitialComments)
  {
   extern const char *Txt_See_the_previous_X_COMMENTS;

   /***** Icon and text to show only the latest comments ****/
   HTM_DIV_Begin ("id=\"exp_%s\" class=\"TL_EXPAND_COM TL_RIGHT_WIDTH\""
	          " style=\"display:none;\"",	// Hidden
		  IdComments);
   TL_Com_PutIconToToggleComments (IdComments,"angle-up.svg",
			           Str_BuildStringLong (Txt_See_the_previous_X_COMMENTS,
						        (long) NumInitialComments));
   Str_FreeString ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Put an icon to toggle on/off comments in a publication ***********/
/*****************************************************************************/

static void TL_Com_PutIconToToggleComments (const char *UniqueId,
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
/******************************** Write comment ******************************/
/*****************************************************************************/

static void TL_Com_WriteComment (struct TL_Timeline *Timeline,
	                         struct TL_Com_Comment *Com,
                                 TL_ShowAlone_t ShowCommentAlone)	// Comment is shown alone, not in a list
  {
   struct UsrData AuthorDat;
   bool IAmTheAuthor;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX + 1];
   static unsigned NumDiv = 0;	// Used to create unique div id for fav

   NumDiv++;

   if (ShowCommentAlone == TL_SHOW_ALONE)
     {
      Box_BoxBegin (NULL,NULL,
                    NULL,NULL,
                    NULL,Box_NOT_CLOSABLE);

      HTM_DIV_Begin ("class=\"TL_LEFT_PHOTO\"");
      HTM_DIV_End ();

      HTM_DIV_Begin ("class=\"TL_RIGHT_CONT TL_RIGHT_WIDTH\"");
      HTM_UL_Begin ("class=\"LIST_LEFT\"");
     }

   /***** Start list item *****/
   HTM_LI_Begin (ShowCommentAlone == TL_SHOW_ALONE ? NULL :
	                                             "class=\"TL_COM\"");

   if (Com->PubCod <= 0 ||
       Com->NotCod <= 0 ||
       Com->UsrCod <= 0)
      Ale_ShowAlert (Ale_ERROR,"Error in comment.");
   else
     {
      /***** Get author's data *****/
      Usr_UsrDataConstructor (&AuthorDat);
      AuthorDat.UsrCod = Com->UsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&AuthorDat,Usr_DONT_GET_PREFS);
      IAmTheAuthor = Usr_ItsMe (AuthorDat.UsrCod);

      /***** Left: write author's photo *****/
      HTM_DIV_Begin ("class=\"TL_COM_PHOTO\"");
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&AuthorDat,PhotoURL);
      Pho_ShowUsrPhoto (&AuthorDat,ShowPhoto ? PhotoURL :
					       NULL,
			"PHOTO30x40",Pho_ZOOM,true);	// Use unique id
      HTM_DIV_End ();

      /***** Right: author's name, time, content, image and buttons *****/
      HTM_DIV_Begin ("class=\"TL_COM_CONT TL_COMM_WIDTH\"");

      /* Write author's full name and nickname */
      TL_Com_WriteAuthorComment (&AuthorDat);

      /* Write date and time */
      TL_WriteDateTime (Com->DateTimeUTC);

      /* Write content of the comment */
      if (Com->Content.Txt[0])
	{
	 HTM_DIV_Begin ("class=\"TL_TXT\"");
	 Msg_WriteMsgContent (Com->Content.Txt,Cns_MAX_BYTES_LONG_TEXT,true,false);
	 HTM_DIV_End ();
	}

      /* Show image */
      Med_ShowMedia (&Com->Content.Media,"TL_COM_MED_CONT TL_COMM_WIDTH",
	                                 "TL_COM_MED TL_COMM_WIDTH");

      /* Start foot container */
      HTM_DIV_Begin ("class=\"TL_FOOT TL_COMM_WIDTH\"");

      /* Fav zone */
      HTM_DIV_Begin ("id=\"fav_com_%s_%u\" class=\"TL_FAV_COM TL_FAV_WIDTH\"",
	             Gbl.UniqueNameEncrypted,NumDiv);
      TL_Fav_PutFormToFavUnfComment (Com,TL_Usr_SHOW_FEW_USRS);
      HTM_DIV_End ();

      /* Put icon to remove this comment */
      HTM_DIV_Begin ("class=\"TL_REM\"");
      if (IAmTheAuthor &&
	  ShowCommentAlone == TL_DONT_SHOW_ALONE)
	 TL_Com_PutFormToRemoveComment (Timeline,Com->PubCod);
      HTM_DIV_End ();

      /* End foot container */
      HTM_DIV_End ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&AuthorDat);
     }

   /***** End list item *****/
   HTM_LI_End ();

   if (ShowCommentAlone == TL_SHOW_ALONE)
     {
      HTM_UL_End ();
      HTM_DIV_End ();
      Box_BoxEnd ();
     }
  }

/*****************************************************************************/
/********* Write name and nickname of author of a comment to a note **********/
/*****************************************************************************/

static void TL_Com_WriteAuthorComment (struct UsrData *UsrDat)
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;
   bool ItsMe = Usr_ItsMe (UsrDat->UsrCod);

   /***** Show user's name inside form to go to user's public profile *****/
   Frm_StartFormUnique (ActSeeOthPubPrf);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   HTM_BUTTON_SUBMIT_Begin (ItsMe ? Txt_My_public_profile :
			            Txt_Another_user_s_profile,
	                    "BT_LINK TL_COM_AUTHOR TL_COMM_AUTHOR_WIDTH DAT_BOLD",NULL);
   HTM_Txt (UsrDat->FullName);
   HTM_BUTTON_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************* Form to remove comment ****************************/
/*****************************************************************************/

static void TL_Com_PutFormToRemoveComment (const struct TL_Timeline *Timeline,
	                                   long PubCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove publication *****/
   TL_FormStart (Timeline,ActReqRemTL_ComGbl,
                          ActReqRemTL_ComUsr);
   TL_Pub_PutHiddenParamPubCod (PubCod);
   Ico_PutIconLink ("trash.svg",Txt_Remove);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************************* Comment a note ******************************/
/*****************************************************************************/

void TL_Com_ReceiveCommentUsr (void)
  {
   struct TL_Timeline Timeline;
   long NotCod;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Receive comment in a note
          and write updated timeline after commenting (user) *****/
   NotCod = TL_Com_ReceiveComment ();
   TL_ShowTimelineUsrHighlightingNot (&Timeline,NotCod);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_Com_ReceiveCommentGbl (void)
  {
   struct TL_Timeline Timeline;
   long NotCod;

   /***** Initialize timeline *****/
   TL_InitTimelineGbl (&Timeline);

   /***** Receive comment in a note *****/
   NotCod = TL_Com_ReceiveComment ();

   /***** Write updated timeline after commenting (global) *****/
   TL_ShowTimelineGblHighlightingNot (&Timeline,NotCod);
  }

static long TL_Com_ReceiveComment (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   struct TL_Pst_PostContent Content;
   struct TL_Not_Note Not;
   struct TL_Pub_Publication Pub;

   /***** Get data of note *****/
   Not.NotCod = TL_Not_GetParamNotCod ();
   TL_Not_GetDataOfNoteByCod (&Not);

   if (Not.NotCod > 0)
     {
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
	 DB_QueryINSERT ("can not store comment content",
			 "INSERT INTO tl_comments"
	                 " (PubCod,Txt,MedCod)"
			 " VALUES"
			 " (%ld,'%s',%ld)",
			 Pub.PubCod,
			 Content.Txt,
			 Content.Media.MedCod);

	 /***** Store notifications about the new comment *****/
	 Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_TIMELINE_COMMENT,Pub.PubCod);

	 /***** Analyze content and store notifications about mentions *****/
	 Str_AnalyzeTxtAndStoreNotifyEventToMentionedUsrs (Pub.PubCod,Content.Txt);
	}

      /***** Free image *****/
      Med_MediaDestructor (&Content.Media);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);

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

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Request the removal of comment in note *****/
   TL_Com_RequestRemovalComment (&Timeline);

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
   TL_Com_RequestRemovalComment (&Timeline);

   /***** Write timeline again (global) *****/
   TL_ShowNoteAndTimelineGbl (&Timeline);
  }

static void TL_Com_RequestRemovalComment (struct TL_Timeline *Timeline)
  {
   extern const char *Txt_The_comment_no_longer_exists;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_comment;
   extern const char *Txt_Remove;
   struct TL_Com_Comment Com;
   bool ItsMe;

   /***** Initialize image *****/
   Med_MediaConstructor (&Com.Content.Media);

   /***** Get data of comment *****/
   Com.PubCod = TL_Pub_GetParamPubCod ();
   TL_Com_GetDataOfCommByCod (&Com);

   if (Com.PubCod > 0)
     {
      ItsMe = Usr_ItsMe (Com.UsrCod);
      if (ItsMe)	// I am the author of this comment
	{
	 /***** Show question and button to remove comment *****/
	 /* Start alert */
	 Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_following_comment);

	 /* Show comment */
	 TL_Com_WriteComment (Timeline,&Com,
			      TL_SHOW_ALONE);

	 /* End alert */
	 Timeline->PubCod = Com.PubCod;	// Publication to be removed
	 if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	    Ale_ShowAlertAndButton2 (ActRemTL_ComUsr,"timeline",NULL,
	                             TL_Com_PutParamsRemoveComment,Timeline,
				     Btn_REMOVE_BUTTON,Txt_Remove);
	 else
	    Ale_ShowAlertAndButton2 (ActRemTL_ComGbl,NULL,NULL,
	                             TL_Com_PutParamsRemoveComment,Timeline,
				     Btn_REMOVE_BUTTON,Txt_Remove);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_comment_no_longer_exists);

   /***** Free image *****/
   Med_MediaDestructor (&Com.Content.Media);
  }

/*****************************************************************************/
/******************** Put parameters to remove a comment *********************/
/*****************************************************************************/

static void TL_Com_PutParamsRemoveComment (void *Timeline)
  {
   if (Timeline)
     {
      if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	 Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
      else
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

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Remove a comment *****/
   TL_Com_RemoveComment ();

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
   TL_Com_RemoveComment ();

   /***** Write updated timeline after removing (global) *****/
   TL_ShowNoteAndTimelineGbl (&Timeline);
  }

static void TL_Com_RemoveComment (void)
  {
   extern const char *Txt_The_comment_no_longer_exists;
   extern const char *Txt_Comment_removed;
   struct TL_Com_Comment Com;
   bool ItsMe;

   /***** Initialize image *****/
   Med_MediaConstructor (&Com.Content.Media);

   /***** Get data of comment *****/
   Com.PubCod = TL_Pub_GetParamPubCod ();
   TL_Com_GetDataOfCommByCod (&Com);

   if (Com.PubCod > 0)
     {
      ItsMe = Usr_ItsMe (Com.UsrCod);
      if (ItsMe)	// I am the author of this comment
	{
	 /***** Remove media associated to comment
	        and delete comment from database *****/
	 TL_Com_RemoveCommentMediaAndDBEntries (Com.PubCod);

	 /***** Reset fields of comment *****/
	 TL_Com_ResetComment (&Com);

	 /***** Message of success *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_Comment_removed);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_comment_no_longer_exists);

   /***** Free image *****/
   Med_MediaDestructor (&Com.Content.Media);
  }

/*****************************************************************************/
/*************** Remove comment media and database entries *******************/
/*****************************************************************************/

void TL_Com_RemoveCommentMediaAndDBEntries (long PubCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long MedCod;

   /***** Remove media associated to comment *****/
   if (DB_QuerySELECT (&mysql_res,"can not get media",
		       "SELECT MedCod"	// row[0]
		       " FROM tl_comments"
		       " WHERE PubCod=%ld",
		       PubCod) == 1)   // Result should have a unique row
     {
      /* Get media code */
      row = mysql_fetch_row (mysql_res);
      MedCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Remove media */
      Med_RemoveMedia (MedCod);
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Mark possible notifications on this comment as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_COMMENT,PubCod);
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV    ,PubCod);
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_MENTION,PubCod);

   /***** Remove favs for this comment *****/
   DB_QueryDELETE ("can not remove favs for comment",
		   "DELETE FROM tl_comments_fav"
		   " WHERE PubCod=%ld",
		   PubCod);

   /***** Remove content of this comment *****/
   DB_QueryDELETE ("can not remove a comment",
		   "DELETE FROM tl_comments"
		   " WHERE PubCod=%ld",
		   PubCod);

   /***** Remove this comment *****/
   DB_QueryDELETE ("can not remove a comment",
		   "DELETE FROM tl_pubs"
	           " WHERE PubCod=%ld"
	           " AND PublisherCod=%ld"	// Extra check: I am the author
	           " AND PubType=%u",		// Extra check: it's a comment
		   PubCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) TL_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/******************* Get data of comment using its code **********************/
/*****************************************************************************/

void TL_Com_GetDataOfCommByCod (struct TL_Com_Comment *Com)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (Com->PubCod > 0)
     {
      /***** Get data of comment from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get data of comment",
			  "SELECT tl_pubs.PubCod,"			// row[0]
				 "tl_pubs.PublisherCod,"		// row[1]
				 "tl_pubs.NotCod,"			// row[2]
				 "UNIX_TIMESTAMP(tl_pubs.TimePublish),"	// row[3]
				 "tl_comments.Txt,"			// row[4]
				 "tl_comments.MedCod"			// row[5]
			  " FROM tl_pubs,tl_comments"
			  " WHERE tl_pubs.PubCod=%ld"
			  " AND tl_pubs.PubType=%u"
			  " AND tl_pubs.PubCod=tl_comments.PubCod",
			  Com->PubCod,(unsigned) TL_Pub_COMMENT_TO_NOTE))
	{
	 /***** Get data of comment *****/
	 row = mysql_fetch_row (mysql_res);
	 TL_Com_GetDataOfCommentFromRow (row,Com);
	}
      else
	 /***** Reset fields of comment *****/
	 TL_Com_ResetComment (Com);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /***** Reset fields of comment *****/
      TL_Com_ResetComment (Com);
  }

/*****************************************************************************/
/********************** Get data of comment from row *************************/
/*****************************************************************************/

static void TL_Com_GetDataOfCommentFromRow (MYSQL_ROW row,struct TL_Com_Comment *Com)
  {
   /*
   row[0]: PubCod
   row[1]: PublisherCod
   row[2]: NotCod
   row[3]: TimePublish
   row[4]: Txt
   row[5]: MedCod
   */
   /***** Get code of comment (row[0]) *****/
   Com->PubCod      = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get (from) user code (row[1]) *****/
   Com->UsrCod      = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get code of note (row[2]) *****/
   Com->NotCod      = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get time of the note (row[3]) *****/
   Com->DateTimeUTC = Dat_GetUNIXTimeFromStr (row[3]);

   /***** Get text content (row[4]) *****/
   Str_Copy (Com->Content.Txt,row[4],
             Cns_MAX_BYTES_LONG_TEXT);

   /***** Get number of times this comment has been favourited *****/
   TL_Fav_GetNumTimesACommHasBeenFav (Com);

   /***** Get media content (row[5]) *****/
   Com->Content.Media.MedCod = Str_ConvertStrCodToLongCod (row[5]);
   Med_GetMediaDataByCod (&Com->Content.Media);
  }

/*****************************************************************************/
/************************** Reset fields of comment **************************/
/*****************************************************************************/

static void TL_Com_ResetComment (struct TL_Com_Comment *Com)
  {
   Com->PubCod      = -1L;
   Com->UsrCod      = -1L;
   Com->NotCod      = -1L;
   Com->DateTimeUTC = (time_t) 0;
   Com->Content.Txt[0]  = '\0';
  }
