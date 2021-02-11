// swad_timeline_post.c: social timeline posts

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

#include "swad_database.h"
#include "swad_global.h"
#include "swad_info.h"
#include "swad_media.h"
#include "swad_message.h"
#include "swad_profile.h"
#include "swad_timeline.h"
#include "swad_timeline_note.h"
#include "swad_timeline_post.h"
#include "swad_timeline_publication.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

#define TL_Pst_MAX_CHARS_IN_POST	1000	// Maximum number of characters in a post

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

static long TL_Pst_ReceivePost (void);

/*****************************************************************************/
/***************** Get from database and write public post *******************/
/*****************************************************************************/

void TL_Pst_GetAndWritePost (long PstCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   struct TL_Pst_PostContent Content;

   /***** Initialize image *****/
   Med_MediaConstructor (&Content.Media);

   /***** Get post from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get the content"
					" of a post",
			     "SELECT Txt,"		// row[0]
			            "MedCod"		// row[1]
			     " FROM tl_posts"
			     " WHERE PstCod=%ld",
			     PstCod);

   /***** Result should have a unique row *****/
   if (NumRows == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /****** Get content (row[0]) *****/
      Str_Copy (Content.Txt,row[0],
                Cns_MAX_BYTES_LONG_TEXT);

      /***** Get media (row[1]) *****/
      Content.Media.MedCod = Str_ConvertStrCodToLongCod (row[1]);
      Med_GetMediaDataByCod (&Content.Media);
     }
   else
      Content.Txt[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Write content *****/
   if (Content.Txt[0])
     {
      HTM_DIV_Begin ("class=\"TL_TXT\"");
      Msg_WriteMsgContent (Content.Txt,Cns_MAX_BYTES_LONG_TEXT,true,false);
      HTM_DIV_End ();
     }

   /***** Show image *****/
   Med_ShowMedia (&Content.Media,"TL_PST_MED_CONT TL_RIGHT_WIDTH",
	                         "TL_PST_MED TL_RIGHT_WIDTH");

   /***** Free image *****/
   Med_MediaDestructor (&Content.Media);
  }

/*****************************************************************************/
/********************** Form to write a new publication **********************/
/*****************************************************************************/

void TL_Pst_PutFormToWriteNewPost (struct TL_Timeline *Timeline)
  {
   extern const char *Txt_New_TIMELINE_post;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];

   /***** Start list *****/
   HTM_UL_Begin ("class=\"TL_LIST\"");
   HTM_LI_Begin ("class=\"TL_WIDTH\"");

   /***** Left: write author's photo (my photo) *****/
   HTM_DIV_Begin ("class=\"TL_LEFT_PHOTO\"");
   ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&Gbl.Usrs.Me.UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (&Gbl.Usrs.Me.UsrDat,ShowPhoto ? PhotoURL :
						     NULL,
		     "PHOTO45x60",Pho_ZOOM,false);
   HTM_DIV_End ();

   /***** Right: author's name, time, textarea *****/
   HTM_DIV_Begin ("class=\"TL_RIGHT_CONT TL_RIGHT_WIDTH\"");

   /* Author name */
   TL_Not_WriteAuthorNote (&Gbl.Usrs.Me.UsrDat);

   /* Form to write the post */
   HTM_DIV_Begin ("class=\"TL_FORM_NEW_PST TL_RIGHT_WIDTH\"");
   TL_FormStart (Timeline,ActRcvTL_PstGbl,
                          ActRcvTL_PstUsr);
   TL_Pst_PutTextarea (Txt_New_TIMELINE_post,"TL_PST_TEXTAREA TL_RIGHT_WIDTH");
   Frm_EndForm ();
   HTM_DIV_End ();

   HTM_DIV_End ();

   /***** End list *****/
   HTM_LI_End ();
   HTM_UL_End ();
  }

/*****************************************************************************/
/*** Put textarea and button inside a form to submit a new post or comment ***/
/*****************************************************************************/

void TL_Pst_PutTextarea (const char *Placeholder,const char *ClassTextArea)
  {
   extern const char *Txt_Post;
   char IdDivImgButton[Frm_MAX_BYTES_ID + 1];

   /***** Set unique id for the hidden div *****/
   Frm_SetUniqueId (IdDivImgButton);

   /***** Textarea to write the content *****/
   HTM_TEXTAREA_Begin ("name=\"Txt\" rows=\"1\" maxlength=\"%u\""
                       " placeholder=\"%s&hellip;\" class=\"%s\""
	               " onfocus=\"expandTextarea(this,'%s','6');\"",
		       TL_Pst_MAX_CHARS_IN_POST,
		       Placeholder,ClassTextArea,
		       IdDivImgButton);
   HTM_TEXTAREA_End ();

   /***** Start concealable div *****/
   HTM_DIV_Begin ("id=\"%s\" style=\"display:none;\"",IdDivImgButton);

   /***** Help on editor *****/
   Lay_HelpPlainEditor ();

   /***** Attached image (optional) *****/
   Med_PutMediaUploader (-1,"TL_MED_INPUT_WIDTH");

   /***** Submit button *****/
   HTM_BUTTON_SUBMIT_Begin (NULL,"BT_SUBMIT_INLINE BT_CREATE",NULL);
   HTM_Txt (Txt_Post);
   HTM_BUTTON_End ();

   /***** End hidden div *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************* Receive and store a new public post *********************/
/*****************************************************************************/

void TL_Pst_ReceivePostUsr (void)
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

   /***** Receive and store post, and
          write updated timeline after publication (user) *****/
   NotCod = TL_Pst_ReceivePost ();
   TL_ShowTimelineUsrHighlightingNot (&Timeline,NotCod);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_Pst_ReceivePostGbl (void)
  {
   struct TL_Timeline Timeline;
   long NotCod;

   /***** Initialize timeline *****/
   TL_InitTimelineGbl (&Timeline);

   /***** Receive and store post *****/
   NotCod = TL_Pst_ReceivePost ();

   /***** Write updated timeline after publication (global) *****/
   TL_ShowTimelineGblHighlightingNot (&Timeline,NotCod);
  }

// Returns the code of the note just created
static long TL_Pst_ReceivePost (void)
  {
   struct TL_Pst_PostContent Content;
   long PstCod;
   struct TL_Pub_Publication Pub;

   /***** Get the content of the new post *****/
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

   if (Content.Txt[0] ||		// Text not empty
      Content.Media.Status == Med_PROCESSED)	// A media is attached
     {
      /***** Store media in filesystem and database *****/
      Med_RemoveKeepOrStoreMedia (-1L,&Content.Media);

      /***** Publish *****/
      /* Insert post content in the database */
      PstCod =
      DB_QueryINSERTandReturnCode ("can not create post",
				   "INSERT INTO tl_posts"
				   " (Txt,MedCod)"
				   " VALUES"
				   " ('%s',%ld)",
				   Content.Txt,
				   Content.Media.MedCod);

      /* Insert post in notes */
      TL_Not_StoreAndPublishNoteInternal (TL_NOTE_POST,PstCod,&Pub);

      /***** Analyze content and store notifications about mentions *****/
      Str_AnalyzeTxtAndStoreNotifyEventToMentionedUsrs (Pub.PubCod,Content.Txt);
     }
   else	// Text and image are empty
      Pub.NotCod = -1L;

   /***** Free image *****/
   Med_MediaDestructor (&Content.Media);

   return Pub.NotCod;
  }
