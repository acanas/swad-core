// swad_timeline_favourite.c: social timeline favourites

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
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_favourite.h"
#include "swad_timeline_form.h"
#include "swad_timeline_notification.h"
#include "swad_timeline_publication.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

#define TL_Fav_ICON_FAV		"heart.svg"
#define TL_Fav_ICON_FAVED	"heart-red.svg"

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

static void TL_Fav_FavNote (struct TL_Not_Note *Not);
static void TL_Fav_UnfNote (struct TL_Not_Note *Not);

static void TL_Fav_FavComment (struct TL_Com_Comment *Com);
static void TL_Fav_UnfComment (struct TL_Com_Comment *Com);

static void TL_Fav_PutDisabledIconFav (unsigned NumFavs);
static void TL_Fav_PutFormToFavUnfNote (long NotCod);
static void TL_Fav_PutFormToFavUnfComm (long PubCod);

static void TL_Fav_ShowUsrsWhoHaveMarkedNoteAsFav (const struct TL_Not_Note *Not,
					           TL_Usr_HowManyUsrs_t HowManyUsrs);
static void TL_Fav_ShowUsrsWhoHaveMarkedCommAsFav (const struct TL_Com_Comment *Com,
					           TL_Usr_HowManyUsrs_t HowManyUsrs);

/*****************************************************************************/
/********************** Mark/unmark a note as favourite **********************/
/*****************************************************************************/

void TL_Fav_ShowAllFaversNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show all favers *****/
   TL_Fav_ShowAllFaversNoteGbl ();
  }

void TL_Fav_ShowAllFaversNoteGbl (void)
  {
   struct TL_Not_Note Not;

   /***** Get data of note *****/
   Not.NotCod = TL_Not_GetParamNotCod ();
   TL_Not_GetDataOfNoteByCod (&Not);

   /***** Write HTML inside DIV with form to fav/unfav *****/
   TL_Fav_PutIconToFavUnfNote (&Not,TL_Usr_SHOW_ALL_USRS);
  }

void TL_Fav_FavNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Mark note as favourite *****/
   TL_Fav_FavNoteGbl ();
  }

void TL_Fav_FavNoteGbl (void)
  {
   struct TL_Not_Note Not;

   /***** Mark note as favourite *****/
   TL_Fav_FavNote (&Not);

   /***** Write HTML inside DIV with form to unfav *****/
   TL_Fav_PutIconToFavUnfNote (&Not,TL_Usr_SHOW_FEW_USRS);
  }

void TL_Fav_UnfNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Unfav a note previously marked as favourite *****/
   TL_Fav_UnfNoteGbl ();
  }

void TL_Fav_UnfNoteGbl (void)
  {
   struct TL_Not_Note Not;

   /***** Stop marking as favourite a previously favourited note *****/
   TL_Fav_UnfNote (&Not);

   /***** Write HTML inside DIV with form to fav *****/
   TL_Fav_PutIconToFavUnfNote (&Not,TL_Usr_SHOW_FEW_USRS);
  }

void TL_Fav_PutIconToFavUnfNote (const struct TL_Not_Note *Not,
                                 TL_Usr_HowManyUsrs_t HowManyUsrs)
  {
   /***** Put form to fav/unfav this note *****/
   HTM_DIV_Begin ("class=\"TL_ICO\"");
   if (Not->Unavailable ||		// Unavailable notes can not be favourited
       Usr_ItsMe (Not->UsrCod))		// I am the author
      /* Put disabled icon */
      TL_Fav_PutDisabledIconFav (Not->NumFavs);
   else					// Available and I am not the author
      TL_Fav_PutFormToFavUnfNote (Not->NotCod);
   HTM_DIV_End ();

   /***** Show who have marked this note as favourite *****/
   TL_Fav_ShowUsrsWhoHaveMarkedNoteAsFav (Not,HowManyUsrs);
  }

static void TL_Fav_FavNote (struct TL_Not_Note *Not)
  {
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = TL_Not_GetParamNotCod ();
   TL_Not_GetDataOfNoteByCod (Not);

   if (Not->NotCod > 0)
     {
      if (Gbl.Usrs.Me.Logged &&		// I am logged...
	  !Usr_ItsMe (Not->UsrCod))	// ...but I am not the author
	 if (!TL_DB_CheckIfNoteIsFavedByUsr (Not->NotCod,
					      Gbl.Usrs.Me.UsrDat.UsrCod))	// I have not yet favourited the note
	   {
	    /***** Mark note as favourite in database *****/
	    TL_DB_MarkNoteAsFav (Not->NotCod);

	    /***** Update number of times this note is favourited *****/
	    TL_Fav_GetNumTimesANoteHasBeenFav (Not);

	    /***** Create notification about favourite post
		   for the author of the post *****/
	    OriginalPubCod = TL_DB_GetPubCodOfOriginalNote (Not->NotCod);
	    if (OriginalPubCod > 0)
	       TL_Ntf_CreateNotifToAuthor (Not->UsrCod,OriginalPubCod,
	                                   Ntf_EVENT_TIMELINE_FAV);
	   }
     }
  }

static void TL_Fav_UnfNote (struct TL_Not_Note *Not)
  {
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = TL_Not_GetParamNotCod ();
   TL_Not_GetDataOfNoteByCod (Not);

   if (Not->NotCod > 0)
      if (Not->NumFavs &&
	  Gbl.Usrs.Me.Logged &&		// I am logged...
	  !Usr_ItsMe (Not->UsrCod))	// ...but I am not the author
	 if (TL_DB_CheckIfNoteIsFavedByUsr (Not->NotCod,
					     Gbl.Usrs.Me.UsrDat.UsrCod))	// I have favourited the note
	   {
	    /***** Delete the mark as favourite from database *****/
	    TL_DB_UnmarkNoteAsFav (Not->NotCod);

	    /***** Update number of times this note is favourited *****/
	    TL_Fav_GetNumTimesANoteHasBeenFav (Not);

            /***** Mark possible notifications on this note as removed *****/
	    OriginalPubCod = TL_DB_GetPubCodOfOriginalNote (Not->NotCod);
	    if (OriginalPubCod > 0)
	       Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV,OriginalPubCod);
	   }
  }

/*****************************************************************************/
/********************* Mark/unmark a comment as favourite ********************/
/*****************************************************************************/

void TL_Fav_ShowAllFaversComUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show all favers *****/
   TL_Fav_ShowAllFaversComGbl ();
  }

void TL_Fav_ShowAllFaversComGbl (void)
  {
   struct TL_Com_Comment Com;

   /***** Get data of comment *****/
   Med_MediaConstructor (&Com.Content.Media);
   Com.PubCod = TL_Pub_GetParamPubCod ();
   TL_Com_GetDataOfCommByCod (&Com);
   Med_MediaDestructor (&Com.Content.Media);

   /***** Write HTML inside DIV with form to fav/unfav *****/
   TL_Fav_PutIconToFavUnfComment (&Com,TL_Usr_SHOW_ALL_USRS);
  }

void TL_Fav_FavCommentUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Mark comment as favourite *****/
   TL_Fav_FavCommentGbl ();
  }

void TL_Fav_FavCommentGbl (void)
  {
   struct TL_Com_Comment Com;

   /***** Mark comment as favourite *****/
   TL_Fav_FavComment (&Com);

   /***** Write HTML inside DIV with form to unfav *****/
   TL_Fav_PutIconToFavUnfComment (&Com,TL_Usr_SHOW_FEW_USRS);
  }

void TL_Fav_UnfCommentUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Unfav a comment previously marked as favourite *****/
   TL_Fav_UnfCommentGbl ();
  }

void TL_Fav_UnfCommentGbl (void)
  {
   struct TL_Com_Comment Com;

   /***** Stop marking as favourite a previously favourited comment *****/
   TL_Fav_UnfComment (&Com);

   /***** Write HTML inside DIV with form to fav *****/
   TL_Fav_PutIconToFavUnfComment (&Com,TL_Usr_SHOW_FEW_USRS);
  }

void TL_Fav_PutIconToFavUnfComment (const struct TL_Com_Comment *Com,
                                    TL_Usr_HowManyUsrs_t HowManyUsrs)
  {
   /***** Put form to fav/unfav this comment *****/
   HTM_DIV_Begin ("class=\"TL_ICO\"");
   if (Usr_ItsMe (Com->UsrCod))			// I am the author
      /* Put disabled icon */
      TL_Fav_PutDisabledIconFav (Com->NumFavs);
   else				// I am not the author
      TL_Fav_PutFormToFavUnfComm (Com->PubCod);
   HTM_DIV_End ();

   /***** Show who have marked this comment as favourite *****/
   TL_Fav_ShowUsrsWhoHaveMarkedCommAsFav (Com,HowManyUsrs);
  }

static void TL_Fav_FavComment (struct TL_Com_Comment *Com)
  {
   /***** Initialize image *****/
   Med_MediaConstructor (&Com->Content.Media);

   /***** Get data of comment *****/
   Com->PubCod = TL_Pub_GetParamPubCod ();
   TL_Com_GetDataOfCommByCod (Com);

   if (Com->PubCod > 0)
      if (!Usr_ItsMe (Com->UsrCod))	// I am not the author
	 if (!TL_DB_CheckIfCommIsFavedByUsr (Com->PubCod,
					      Gbl.Usrs.Me.UsrDat.UsrCod)) // I have not yet favourited the comment
	   {
	    /***** Mark comment as favourite in database *****/
	    TL_DB_MarkCommAsFav (Com->PubCod);

	    /* Update number of times this comment is favourited */
	    TL_Fav_GetNumTimesACommHasBeenFav (Com);

	    /**** Create notification about favourite post
		  for the author of the post ***/
	    TL_Ntf_CreateNotifToAuthor (Com->UsrCod,Com->PubCod,
	                                Ntf_EVENT_TIMELINE_FAV);
	   }

   /***** Free image *****/
   Med_MediaDestructor (&Com->Content.Media);
  }

static void TL_Fav_UnfComment (struct TL_Com_Comment *Com)
  {
   /***** Initialize image *****/
   Med_MediaConstructor (&Com->Content.Media);

   /***** Get data of comment *****/
   Com->PubCod = TL_Pub_GetParamPubCod ();
   TL_Com_GetDataOfCommByCod (Com);

   if (Com->PubCod > 0)
      if (Com->NumFavs &&
	  !Usr_ItsMe (Com->UsrCod))	// I am not the author
	 if (TL_DB_CheckIfCommIsFavedByUsr (Com->PubCod,
					     Gbl.Usrs.Me.UsrDat.UsrCod))	// I have favourited the comment
	   {
	    /***** Delete the mark as favourite from database *****/
	    TL_DB_UnmarkCommAsFav (Com->PubCod);

	    /***** Update number of times this comment is favourited *****/
	    TL_Fav_GetNumTimesACommHasBeenFav (Com);

            /***** Mark possible notifications on this comment as removed *****/
            Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV,Com->PubCod);
	   }

   /***** Free image *****/
   Med_MediaDestructor (&Com->Content.Media);
  }


/*****************************************************************************/
/****************** Put disabled icon to mark as favourite *******************/
/*****************************************************************************/

static void TL_Fav_PutDisabledIconFav (unsigned NumFavs)
  {
   extern const char *Txt_TIMELINE_Favourited_by_X_USERS;
   extern const char *Txt_TIMELINE_Not_favourited_by_anyone;

   /***** Disabled icon to mark as favourite *****/
   if (NumFavs)
     {
      Ico_PutDivIcon ("TL_ICO_DISABLED",TL_Fav_ICON_FAV,
		      Str_BuildStringLong (Txt_TIMELINE_Favourited_by_X_USERS,
					   (long) NumFavs));
      Str_FreeString ();
     }
   else
      Ico_PutDivIcon ("TL_ICO_DISABLED",TL_Fav_ICON_FAV,
		      Txt_TIMELINE_Not_favourited_by_anyone);
  }

/*****************************************************************************/
/************************** Form to fav/unfav note ***************************/
/*****************************************************************************/

static void TL_Fav_PutFormToFavUnfNote (long NotCod)
  {
   extern const char *Txt_TIMELINE_Favourite;
   extern const char *Txt_TIMELINE_Mark_as_favourite;
   struct TL_Form Form[2] =
     {
      [false] = // I have not faved ==> fav
        {
         .Action      = TL_Frm_FAV_NOTE,
         .ParamFormat = "NotCod=%ld",
         .ParamCod    = NotCod,
         .Icon        = TL_Fav_ICON_FAV,
         .Title       = Txt_TIMELINE_Mark_as_favourite,
        },
      [true] = // I have faved ==> unfav
	{
	 .Action      = TL_Frm_UNF_NOTE,
	 .ParamFormat = "NotCod=%ld",
	 .ParamCod    = NotCod,
	 .Icon        = TL_Fav_ICON_FAVED,
	 .Title       = Txt_TIMELINE_Favourite,
	},
     };

   /***** Form and icon to fav/unfav note *****/
   TL_Frm_FormFavSha (&Form[TL_DB_CheckIfNoteIsFavedByUsr (NotCod,Gbl.Usrs.Me.UsrDat.UsrCod)]);
  }

/*****************************************************************************/
/************************** Form to fav/unfav comment ************************/
/*****************************************************************************/

static void TL_Fav_PutFormToFavUnfComm (long PubCod)
  {
   extern const char *Txt_TIMELINE_Favourite;
   extern const char *Txt_TIMELINE_Mark_as_favourite;
   struct TL_Form Form[2] =
     {
      [false] = // I have not faved ==> fav
        {
         .Action      = TL_Frm_FAV_COMM,
         .ParamFormat = "PubCod=%ld",
         .ParamCod    = PubCod,
         .Icon        = TL_Fav_ICON_FAV,
         .Title       = Txt_TIMELINE_Mark_as_favourite,
        },
      [true] = // I have faved ==> unfav
	{
	 .Action      = TL_Frm_UNF_COMM,
	 .ParamFormat = "PubCod=%ld",
	 .ParamCod    = PubCod,
	 .Icon        = TL_Fav_ICON_FAVED,
	 .Title       = Txt_TIMELINE_Favourite,
	},
     };

   /***** Form and icon to fav/unfav *****/
   TL_Frm_FormFavSha (&Form[TL_DB_CheckIfNoteIsFavedByUsr (PubCod,Gbl.Usrs.Me.UsrDat.UsrCod)]);
  }

/*****************************************************************************/
/*************** Get number of times a note has been favourited **************/
/*****************************************************************************/

void TL_Fav_GetNumTimesANoteHasBeenFav (struct TL_Not_Note *Not)
  {
   /***** Get number of times (users) this note has been favourited *****/
   Not->NumFavs =
   (unsigned) DB_QueryCOUNT ("can not get number of times"
			     " a note has been favourited",
			     "SELECT COUNT(*) FROM tl_notes_fav"
			     " WHERE NotCod=%ld"
			     " AND UsrCod<>%ld",	// Extra check
			     Not->NotCod,
			     Not->UsrCod);		// The author
  }

/*****************************************************************************/
/************ Get number of times a comment has been favourited **************/
/*****************************************************************************/

void TL_Fav_GetNumTimesACommHasBeenFav (struct TL_Com_Comment *Com)
  {
   /***** Get number of times (users) this comment has been favourited *****/
   Com->NumFavs =
   (unsigned) DB_QueryCOUNT ("can not get number of times"
			     " a comment has been favourited",
			     "SELECT COUNT(*) FROM tl_comments_fav"
			     " WHERE PubCod=%ld"
			     " AND UsrCod<>%ld",	// Extra check
			     Com->PubCod,
			     Com->UsrCod);		// The author
  }

/*****************************************************************************/
/************ Show users who have marked this note as favourite **************/
/*****************************************************************************/

static void TL_Fav_ShowUsrsWhoHaveMarkedNoteAsFav (const struct TL_Not_Note *Not,
					           TL_Usr_HowManyUsrs_t HowManyUsrs)
  {
   MYSQL_RES *mysql_res;
   unsigned NumFirstUsrs;

   /***** Get users who have marked this note as favourite *****/
   if (Not->NumFavs)
      /***** Get list of users from database *****/
      NumFirstUsrs =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get users",
				 "SELECT UsrCod FROM tl_notes_fav"
				 " WHERE NotCod=%ld"
				 " AND UsrCod<>%ld"	// Extra check
				 " ORDER BY FavCod LIMIT %u",
				 Not->NotCod,
				 Not->UsrCod,
				 HowManyUsrs == TL_Usr_SHOW_FEW_USRS ? TL_Usr_DEF_USRS_SHOWN :
				                                       TL_Usr_MAX_USRS_SHOWN);
   else
      NumFirstUsrs = 0;

   /***** Show users *****/
   /* Number of users */
   HTM_DIV_Begin ("class=\"TL_NUM_USRS\"");
   TL_Usr_ShowNumSharersOrFavers (Not->NumFavs);
   HTM_DIV_End ();

   /* List users one by one */
   HTM_DIV_Begin ("class=\"TL_USRS\"");
   TL_Usr_ShowSharersOrFavers (&mysql_res,Not->NumFavs,NumFirstUsrs);
   if (NumFirstUsrs < Not->NumFavs)		// Not all are shown
      /* Clickable ellipsis to show all users */
      TL_Frm_PutFormToSeeAllFaversSharers (TL_Frm_ALL_FAV_NOTE,
                                           "NotCod=%ld",Not->NotCod,
                                           HowManyUsrs);
   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   if (Not->NumFavs)
      DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Show users who have marked this note as favourite **************/
/*****************************************************************************/

static void TL_Fav_ShowUsrsWhoHaveMarkedCommAsFav (const struct TL_Com_Comment *Com,
					           TL_Usr_HowManyUsrs_t HowManyUsrs)
  {
   MYSQL_RES *mysql_res;
   unsigned NumFirstUsrs;

   /***** Get users who have marked this comment as favourite *****/
   if (Com->NumFavs)
      /***** Get list of users from database *****/
      NumFirstUsrs =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get users",
				 "SELECT UsrCod FROM tl_comments_fav"
				 " WHERE PubCod=%ld"
				 " AND UsrCod<>%ld"	// Extra check
				 " ORDER BY FavCod LIMIT %u",
				 Com->PubCod,
				 Com->UsrCod,
				 HowManyUsrs == TL_Usr_SHOW_FEW_USRS ? TL_Usr_DEF_USRS_SHOWN :
				                                       TL_Usr_MAX_USRS_SHOWN);
   else
      NumFirstUsrs = 0;

   /***** Show users *****/
   /* Number of users */
   HTM_DIV_Begin ("class=\"TL_NUM_USRS\"");
   TL_Usr_ShowNumSharersOrFavers (Com->NumFavs);
   HTM_DIV_End ();

   /* List users one by one */
   HTM_DIV_Begin ("class=\"TL_USRS\"");
   TL_Usr_ShowSharersOrFavers (&mysql_res,Com->NumFavs,NumFirstUsrs);
   if (NumFirstUsrs < Com->NumFavs)
      /* Clickable ellipsis to show all users */
      TL_Frm_PutFormToSeeAllFaversSharers (TL_Frm_ALL_FAV_COMM,
                                           "PubCod=%ld",Com->PubCod,
                                           HowManyUsrs);
   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   if (Com->NumFavs)
      DB_FreeMySQLResult (&mysql_res);
  }
