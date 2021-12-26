// swad_timeline_post.c: social timeline posts

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Ca�as Vargas

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

#include "swad_database.h"
#include "swad_global.h"
#include "swad_info.h"
#include "swad_media.h"
#include "swad_message.h"
#include "swad_profile.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_form.h"
#include "swad_timeline_note.h"
#include "swad_timeline_post.h"
#include "swad_timeline_publication.h"

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

#define Tml_Pst_MAX_CHARS_IN_POST	1000	// Maximum number of characters in a post

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Tml_Pst_GetPostContent (long PstCod,struct Tml_Pst_Content *Content);
static void Tml_Pst_ShowPostContent (struct Tml_Pst_Content *Content);

static void Tml_Pst_PutFormToWriteNewPost (struct Tml_Timeline *Timeline);

static long Tml_Pst_ReceivePost (void);

/*****************************************************************************/
/***************** Get from database and write public post *******************/
/*****************************************************************************/

void Tml_Pst_GetAndWritePost (long PstCod)
  {
   struct Tml_Pst_Content Content;

   /***** Initialize media *****/
   Med_MediaConstructor (&Content.Media);

   /***** Get post content from database *****/
   Tml_Pst_GetPostContent (PstCod,&Content);

   /***** Show post content *****/
   Tml_Pst_ShowPostContent (&Content);

   /***** Free media *****/
   Med_MediaDestructor (&Content.Media);
  }

/*****************************************************************************/
/****************************** Get post content *****************************/
/*****************************************************************************/
// Media must be initialized before calling this function

static void Tml_Pst_GetPostContent (long PstCod,struct Tml_Pst_Content *Content)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get post from database *****/
   if (Tml_DB_GetPostByCod (PstCod,&mysql_res) == 1)
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);
      /*
      row[0]	Txt
      row[1]	MedCod
      */
      /* Get content (row[0]) */
      Str_Copy (Content->Txt,row[0],sizeof (Content->Txt) - 1);

      /* Get media (row[1]) */
      Content->Media.MedCod = Str_ConvertStrCodToLongCod (row[1]);
      Med_GetMediaDataByCod (&Content->Media);
     }
   else
      Content->Txt[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************************** Get post content *****************************/
/*****************************************************************************/

static void Tml_Pst_ShowPostContent (struct Tml_Pst_Content *Content)
  {
   extern const char *The_TmlTxtColor[The_NUM_THEMES];

   /***** Write content text *****/
   if (Content->Txt[0])
     {
      HTM_DIV_Begin ("class=\"Tml_TXT %s\"",The_TmlTxtColor[Gbl.Prefs.Theme]);
	 Msg_WriteMsgContent (Content->Txt,true,false);
      HTM_DIV_End ();
     }

   /***** Show media *****/
   Med_ShowMedia (&Content->Media,"Tml_PST_MED_CONT Tml_RIGHT_WIDTH",
	                          "Tml_PST_MED Tml_RIGHT_WIDTH");
  }

/*****************************************************************************/
/********************** Form to write a new publication **********************/
/*****************************************************************************/

void Tml_Pst_PutPhotoAndFormToWriteNewPost (struct Tml_Timeline *Timeline)
  {
   /***** Begin list *****/
   HTM_UL_Begin ("class=\"Tml_LIST\"");

      /***** Begin list item *****/
      HTM_LI_Begin ("class=\"Tml_WIDTH\"");

	 /***** Left: write author's photo (my photo) *****/
         Tml_Not_ShowAuthorPhoto (&Gbl.Usrs.Me.UsrDat,false);	// Don't use unique id

	 /***** Right: author's name, time, textarea *****/
         Tml_Pst_PutFormToWriteNewPost (Timeline);

      /***** End list item *****/
      HTM_LI_End ();

   /***** End list *****/
   HTM_UL_End ();
  }

/*****************************************************************************/
/**************** Author's name, time, and form with textarea ****************/
/*****************************************************************************/

static void Tml_Pst_PutFormToWriteNewPost (struct Tml_Timeline *Timeline)
  {
   extern const char *The_ClassDatStrong[The_NUM_THEMES];
   extern const char *The_ClassInput[The_NUM_THEMES];
   extern const char *Txt_New_TIMELINE_post;

   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"Tml_RIGHT_CONT Tml_RIGHT_WIDTH\"");

      /***** Author name *****/
      Tml_Not_WriteAuthorName (&Gbl.Usrs.Me.UsrDat,
                               Str_BuildString ("Tml_RIGHT_AUTHOR Tml_RIGHT_AUTHOR_WIDTH BT_LINK %s BOLD",
                                                The_ClassDatStrong[Gbl.Prefs.Theme]));
      Str_FreeStrings ();

      /***** Form to write the post *****/
      /* Begin container */
      HTM_DIV_Begin ("class=\"Tml_FORM_NEW_PST Tml_RIGHT_WIDTH\"");

         /* Form with textarea */
	 Tml_Frm_BeginForm (Timeline,Tml_Frm_RECEIVE_POST);
	    Tml_Pst_PutTextarea (Txt_New_TIMELINE_post,
				 Str_BuildString ("Tml_COM_TEXTAREA Tml_COMM_WIDTH %s",
			                          The_ClassInput[Gbl.Prefs.Theme]));
	    Str_FreeStrings ();
	 Tml_Frm_EndForm ();

      /* End container */
      HTM_DIV_End ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*** Put textarea and button inside a form to submit a new post or comment ***/
/*****************************************************************************/

void Tml_Pst_PutTextarea (const char *Placeholder,const char *ClassTextArea)
  {
   extern const char *The_ClassInput[The_NUM_THEMES];
   extern const char *Txt_Post;
   char IdDivImgButton[Frm_MAX_BYTES_ID + 1];

   /***** Set unique id for the hidden div *****/
   Frm_SetUniqueId (IdDivImgButton);

   /***** Textarea to write the content *****/
   HTM_TEXTAREA_Begin ("name=\"Txt\" rows=\"1\" maxlength=\"%u\""
                       " placeholder=\"%s&hellip;\" class=\"%s\""
	               " onfocus=\"expandTextarea(this,'%s','6');\"",
		       Tml_Pst_MAX_CHARS_IN_POST,
		       Placeholder,ClassTextArea,
		       IdDivImgButton);
   HTM_TEXTAREA_End ();

   /***** Begin hidden div *****/
   HTM_DIV_Begin ("id=\"%s\" style=\"display:none;\"",IdDivImgButton);

      /***** Help on editor *****/
      Lay_HelpPlainEditor ();

      /***** Attached image (optional) *****/
      Med_PutMediaUploader (-1,Str_BuildString ("Tml_MED_INPUT_WIDTH %s",
                                                The_ClassInput[Gbl.Prefs.Theme]));
      Str_FreeStrings ();

      /***** Submit button *****/
      HTM_BUTTON_OnSubmit_Begin (NULL,"BT_SUBMIT_INLINE BT_CREATE",NULL);
	 HTM_Txt (Txt_Post);
      HTM_BUTTON_End ();

   /***** End hidden div *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************* Receive and store a new public post *********************/
/*****************************************************************************/

void Tml_Pst_ReceivePostUsr (void)
  {
   struct Tml_Timeline Timeline;
   long NotCod;

   /***** Reset timeline context *****/
   Tml_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Begin section *****/
   HTM_SECTION_Begin (Tml_TIMELINE_SECTION_ID);

      /***** Receive and store post, and
	     write updated timeline after publication (user) *****/
      NotCod = Tml_Pst_ReceivePost ();
      Tml_ShowTimelineUsrHighlighting (&Timeline,NotCod);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void Tml_Pst_ReceivePostGbl (void)
  {
   struct Tml_Timeline Timeline;
   long NotCod;

   /***** Initialize timeline *****/
   Tml_InitTimelineGbl (&Timeline);

   /***** Receive and store post *****/
   NotCod = Tml_Pst_ReceivePost ();

   /***** Write updated timeline after publication (global) *****/
   Tml_ShowTimelineGblHighlighting (&Timeline,NotCod);
  }

// Returns the code of the note just created
static long Tml_Pst_ReceivePost (void)
  {
   struct Tml_Pst_Content Content;
   long PstCod;
   struct Tml_Pub_Publication Pub;

   /***** Get the content of the new post *****/
   Par_GetParAndChangeFormat ("Txt",Content.Txt,Cns_MAX_BYTES_LONG_TEXT,
                              Str_TO_RIGOROUS_HTML,true);

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
      /* Insert post content in the database */
      PstCod = Tml_DB_CreateNewPost (&Content);

      /* Insert post in notes */
      Tml_Not_StoreAndPublishNoteInternal (Tml_NOTE_POST,PstCod,&Pub);

      /***** Analyze content and store notifications about mentions *****/
      Str_AnalyzeTxtAndStoreNotifyEventToMentionedUsrs (Pub.PubCod,Content.Txt);
     }
   else	// Text and image are empty
      Pub.NotCod = -1L;

   /***** Free image *****/
   Med_MediaDestructor (&Content.Media);

   return Pub.NotCod;
  }
