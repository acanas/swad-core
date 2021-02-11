// swad_timeline_user.c: social timeline users

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
#include "swad_global.h"
#include "swad_photo.h"
#include "swad_timeline.h"
#include "swad_timeline_note.h"
#include "swad_timeline_publication.h"
#include "swad_timeline_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

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

/*****************************************************************************/
/************* Remove all the content of a user from database ****************/
/*****************************************************************************/

void TL_Usr_RemoveUsrContent (long UsrCod)
  {
   /***** Remove favs for comments *****/
   /* Remove all favs made by this user in any comment */
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM tl_comments_fav"
		   " WHERE UsrCod=%ld",
		   UsrCod);

   /* Remove all favs for all comments of this user */
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM tl_comments_fav"
	           " USING tl_pubs,tl_comments_fav"
	           " WHERE tl_pubs.PublisherCod=%ld"	// Author of the comment
                   " AND tl_pubs.PubType=%u"
	           " AND tl_pubs.PubCod=tl_comments_fav.PubCod",
		   UsrCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);

   /* Remove all favs for all comments in all the notes of the user */
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tl_comments_fav"
	           " USING tl_notes,tl_pubs,tl_comments_fav"
	           " WHERE tl_notes.UsrCod=%ld"	// Author of the note
	           " AND tl_notes.NotCod=tl_pubs.NotCod"
                   " AND tl_pubs.PubType=%u"
	           " AND tl_pubs.PubCod=tl_comments_fav.PubCod",
		   UsrCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);

   /***** Remove favs for notes *****/
   /* Remove all favs made by this user in any note */
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM tl_notes_fav"
		   " WHERE UsrCod=%ld",
		   UsrCod);

   /* Remove all favs for all notes of this user */
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM tl_notes_fav"
	           " USING tl_notes,tl_notes_fav"
	           " WHERE tl_notes.UsrCod=%ld"	// Author of the note
	           " AND tl_notes.NotCod=tl_notes_fav.NotCod",
		   UsrCod);

   /***** Remove comments *****/
   /* Remove content of all the comments in all the notes of the user */
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tl_comments"
	           " USING tl_notes,tl_pubs,tl_comments"
	           " WHERE tl_notes.UsrCod=%ld"
		   " AND tl_notes.NotCod=tl_pubs.NotCod"
                   " AND tl_pubs.PubType=%u"
	           " AND tl_pubs.PubCod=tl_comments.PubCod",
		   UsrCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);

   /* Remove all the comments from any user in any note of the user */
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tl_pubs"
	           " USING tl_notes,tl_pubs"
	           " WHERE tl_notes.UsrCod=%ld"
		   " AND tl_notes.NotCod=tl_pubs.NotCod"
                   " AND tl_pubs.PubType=%u",
		   UsrCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);

   /* Remove content of all the comments of the user in any note */
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tl_comments"
	           " USING tl_pubs,tl_comments"
	           " WHERE tl_pubs.PublisherCod=%ld"
	           " AND tl_pubs.PubType=%u"
	           " AND tl_pubs.PubCod=tl_comments.PubCod",
		   UsrCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);

   /***** Remove all the posts of the user *****/
   DB_QueryDELETE ("can not remove posts",
		   "DELETE FROM tl_posts"
		   " WHERE PstCod IN"
		   " (SELECT Cod FROM tl_notes"
	           " WHERE UsrCod=%ld AND NoteType=%u)",
		   UsrCod,(unsigned) TL_NOTE_POST);

   /***** Remove all the publications of any user authored by the user *****/
   DB_QueryDELETE ("can not remove publications",
		   "DELETE FROM tl_pubs"
                   " USING tl_notes,tl_pubs"
	           " WHERE tl_notes.UsrCod=%ld"
                   " AND tl_notes.NotCod=tl_pubs.NotCod",
		   UsrCod);

   /***** Remove all the publications of the user *****/
   DB_QueryDELETE ("can not remove publications",
		   "DELETE FROM tl_pubs"
		   " WHERE PublisherCod=%ld",
		   UsrCod);

   /***** Remove all the notes of the user *****/
   DB_QueryDELETE ("can not remove notes",
		   "DELETE FROM tl_notes"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/************************ Show sharers or favouriters ************************/
/*****************************************************************************/

void TL_Usr_ShowNumSharersOrFavers (unsigned NumUsrs)
  {
   /***** Show number of sharers or favers
          (users who have shared or marked this note as favourite) *****/
   HTM_TxtF ("&nbsp;%u",NumUsrs);
  }

void TL_Usr_ShowSharersOrFavers (MYSQL_RES **mysql_res,
			         unsigned NumUsrs,unsigned NumFirstUsrs)
  {
   MYSQL_ROW row;
   unsigned NumUsr;
   unsigned NumUsrsShown = 0;
   struct UsrData UsrDat;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];

   if (NumUsrs)
     {
      /***** A list of users has been got from database *****/
      if (NumFirstUsrs)
	{
	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&UsrDat);

	 /***** List users *****/
	 for (NumUsr = 0;
	      NumUsr < NumFirstUsrs;
	      NumUsr++)
	   {
	    /***** Get user *****/
	    row = mysql_fetch_row (*mysql_res);

	    /* Get user's code (row[0]) */
	    UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	    /***** Get user's data and show user's photo *****/
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS))
	      {
               HTM_DIV_Begin ("class=\"TL_SHARER\"");
	       ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL);
	       Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
	                                             NULL,
	                         "PHOTO12x16",Pho_ZOOM,true);	// Use unique id
               HTM_DIV_End ();

               NumUsrsShown++;
              }
	   }

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);
	}
     }
  }

/*****************************************************************************/
/********************* Form to show all favers/sharers ***********************/
/*****************************************************************************/

void TL_Usr_PutFormToSeeAllFaversSharers (Act_Action_t ActionGbl,
                                          Act_Action_t ActionUsr,
		                          const char *ParamFormat,long ParamCod,
                                          TL_Usr_HowManyUsrs_t HowManyUsrs)
  {
   extern const char *Txt_View_all_USERS;

   switch (HowManyUsrs)
     {
      case TL_Usr_SHOW_FEW_USRS:
	 /***** Form and icon to mark note as favourite *****/
	 TL_Usr_FormFavSha (ActionGbl,
	                    ActionUsr,
	                    ParamFormat,ParamCod,
			    TL_Usr_ICON_ELLIPSIS,Txt_View_all_USERS);
	 break;
      case TL_Usr_SHOW_ALL_USRS:
         Ico_PutIconOff (TL_Usr_ICON_ELLIPSIS,Txt_View_all_USERS);
	 break;
     }
  }

/*****************************************************************************/
/******* Form to fav/unfav or share/unshare in global or user timeline *******/
/*****************************************************************************/

void TL_Usr_FormFavSha (Act_Action_t ActionGbl,
                        Act_Action_t ActionUsr,
		        const char *ParamFormat,long ParamCod,
		        const char *Icon,const char *Title)
  {
   char *OnSubmit;
   char ParamStr[7 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Create parameter string *****/
   sprintf (ParamStr,ParamFormat,ParamCod);

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
		    Act_GetActCod (ActionUsr),
		    Gbl.Session.Id,
		    ParamStr,
		    Gbl.Usrs.Other.UsrDat.EncryptedUsrCod) < 0)
	 Lay_NotEnoughMemoryExit ();
      Frm_StartFormUniqueAnchorOnSubmit (ActUnk,"timeline",OnSubmit);
     }
   else
     {
      if (asprintf (&OnSubmit,"updateDivFaversSharers(this,"
			      "'act=%ld&ses=%s&%s');"
			      " return false;",	// return false is necessary to not submit form
		    Act_GetActCod (ActionGbl),
		    Gbl.Session.Id,
		    ParamStr) < 0)
	 Lay_NotEnoughMemoryExit ();
      Frm_StartFormUniqueAnchorOnSubmit (ActUnk,NULL,OnSubmit);
     }
   Ico_PutIconLink (Icon,Title);
   Frm_EndForm ();

   /* Free allocated memory */
   free (OnSubmit);
  }
