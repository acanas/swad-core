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

static void TL_Fav_FavComm (struct TL_Com_Comment *Com);
static void TL_Fav_UnfComm (struct TL_Com_Comment *Com);

static void TL_Fav_PutDisabledIconFav (unsigned NumFavs);
static void TL_Fav_PutFormToFavUnf (TL_Fav_WhatToFav_t WhatToFav,long Cod);

static void TL_Fav_ShowUsrsWhoHaveMarkedAsFav (TL_Fav_WhatToFav_t WhatToFav,
                                               long Cod,long UsrCod,
                                               unsigned NumFavs,
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
   TL_Fav_PutIconToFavUnf (TL_Fav_NOTE,Not.NotCod,Not.UsrCod,Not.NumFavs,
                           TL_Usr_SHOW_ALL_USRS);
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
   TL_Fav_PutIconToFavUnf (TL_Fav_NOTE,Not.NotCod,Not.UsrCod,Not.NumFavs,
                           TL_Usr_SHOW_FEW_USRS);
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
   TL_Fav_PutIconToFavUnf (TL_Fav_NOTE,Not.NotCod,Not.UsrCod,Not.NumFavs,
                           TL_Usr_SHOW_FEW_USRS);
  }

static void TL_Fav_FavNote (struct TL_Not_Note *Not)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = TL_Not_GetParamNotCod ();
   TL_Not_GetDataOfNoteByCod (Not);

   /***** Trivial check 1: note code should be > 0 *****/
   if (Not->NotCod <= 0)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);
      return;
     }

   /***** Trivial check 2: I must be logged *****/
   if (!Gbl.Usrs.Me.Logged)
     {
      Ale_ShowAlert (Ale_ERROR,"You are not logged.");
      return;
     }

   /***** Trivial check 3: The author can not fav his/her own notes *****/
   if (Usr_ItsMe (Not->UsrCod))
     {
      Ale_ShowAlert (Ale_ERROR,"You can not fav/unfav your own posts.");
      return;
     }

   /***** Trivial check 4: Have I faved this note? *****/
   if (TL_DB_CheckIfFavedByUsr (TL_Fav_NOTE,Not->NotCod,
				Gbl.Usrs.Me.UsrDat.UsrCod))
      // Don't show error message
      return;

   /***** Mark note as favourite in database *****/
   TL_DB_MarkAsFav (TL_Fav_NOTE,Not->NotCod);

   /***** Update number of times this note is favourited *****/
   Not->NumFavs = TL_DB_GetNumTimesHasBeenFav (TL_Fav_NOTE,
					       Not->NotCod,Not->UsrCod);

   /***** Create notification about favourite post
	  for the author of the post *****/
   OriginalPubCod = TL_DB_GetPubCodOfOriginalNote (Not->NotCod);
   if (OriginalPubCod > 0)
      TL_Ntf_CreateNotifToAuthor (Not->UsrCod,OriginalPubCod,
				  Ntf_EVENT_TIMELINE_FAV);
  }

static void TL_Fav_UnfNote (struct TL_Not_Note *Not)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = TL_Not_GetParamNotCod ();
   TL_Not_GetDataOfNoteByCod (Not);

   /***** Trivial check 1: note code should be > 0 *****/
   if (Not->NotCod <= 0)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);
      return;
     }

   /***** Trivial check 2: I must be logged *****/
   if (!Gbl.Usrs.Me.Logged)
     {
      Ale_ShowAlert (Ale_ERROR,"You are not logged.");
      return;
     }

   /***** Trivial check 3: The author can not unfav his/her own notes *****/
   if (Usr_ItsMe (Not->UsrCod))
     {
      Ale_ShowAlert (Ale_ERROR,"You can not fav/unfav your own posts.");
      return;
     }

   /***** Trivial check 4: Have I faved this note? *****/
   if (!TL_DB_CheckIfFavedByUsr (TL_Fav_NOTE,Not->NotCod,
				 Gbl.Usrs.Me.UsrDat.UsrCod))
      // Don't show error message
      return;

   /***** Delete the mark as favourite from database *****/
   TL_DB_UnmarkAsFav (TL_Fav_NOTE,Not->NotCod);

   /***** Update number of times this note is favourited *****/
   Not->NumFavs = TL_DB_GetNumTimesHasBeenFav (TL_Fav_NOTE,
					       Not->NotCod,Not->UsrCod);

   /***** Mark possible notifications on this note as removed *****/
   OriginalPubCod = TL_DB_GetPubCodOfOriginalNote (Not->NotCod);
   if (OriginalPubCod > 0)
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV,OriginalPubCod);
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
   TL_Fav_PutIconToFavUnf (TL_Fav_COMM,Com.PubCod,Com.UsrCod,Com.NumFavs,
                           TL_Usr_SHOW_ALL_USRS);
  }

void TL_Fav_FavCommUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Mark comment as favourite *****/
   TL_Fav_FavCommGbl ();
  }

void TL_Fav_FavCommGbl (void)
  {
   struct TL_Com_Comment Com;

   /***** Mark comment as favourite *****/
   TL_Fav_FavComm (&Com);

   /***** Write HTML inside DIV with form to unfav *****/
   TL_Fav_PutIconToFavUnf (TL_Fav_COMM,Com.PubCod,Com.UsrCod,Com.NumFavs,
                           TL_Usr_SHOW_FEW_USRS);
  }

void TL_Fav_UnfCommUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Unfav a comment previously marked as favourite *****/
   TL_Fav_UnfCommGbl ();
  }

void TL_Fav_UnfCommGbl (void)
  {
   struct TL_Com_Comment Com;

   /***** Stop marking as favourite a previously favourited comment *****/
   TL_Fav_UnfComm (&Com);

   /***** Write HTML inside DIV with form to fav *****/
   TL_Fav_PutIconToFavUnf (TL_Fav_COMM,Com.PubCod,Com.UsrCod,Com.NumFavs,
                           TL_Usr_SHOW_FEW_USRS);
  }

static void TL_Fav_FavComm (struct TL_Com_Comment *Com)
  {
   extern const char *Txt_The_comment_no_longer_exists;

   /***** Initialize image *****/
   Med_MediaConstructor (&Com->Content.Media);

   /***** Get data of comment *****/
   Com->PubCod = TL_Pub_GetParamPubCod ();
   TL_Com_GetDataOfCommByCod (Com);

   /***** Trivial check 1: publication code should be > 0 *****/
   if (Com->PubCod <= 0)
     {
      Med_MediaDestructor (&Com->Content.Media);
      Ale_ShowAlert (Ale_WARNING,Txt_The_comment_no_longer_exists);
      return;
     }

   /***** Trivial check 2: I must be logged *****/
   if (!Gbl.Usrs.Me.Logged)
     {
      Med_MediaDestructor (&Com->Content.Media);
      Ale_ShowAlert (Ale_ERROR,"You are not logged.");
      return;
     }

   /***** Trivial check 3: The author can not fav his/her own comments *****/
   if (Usr_ItsMe (Com->UsrCod))
     {
      Med_MediaDestructor (&Com->Content.Media);
      Ale_ShowAlert (Ale_ERROR,"You can not fav/unfav your own comments.");
      return;
     }

   /***** Trivial check 4: Have I faved this comment? *****/
   if (TL_DB_CheckIfFavedByUsr (TL_Fav_COMM,Com->PubCod,
				Gbl.Usrs.Me.UsrDat.UsrCod))
     {
      Med_MediaDestructor (&Com->Content.Media);
      // Don't show error message
      return;
     }

   /***** Mark comment as favourite in database *****/
   TL_DB_MarkAsFav (TL_Fav_COMM,Com->PubCod);

   /***** Update number of times this comment is favourited *****/
   Com->NumFavs = TL_DB_GetNumTimesHasBeenFav (TL_Fav_COMM,
					       Com->PubCod,Com->UsrCod);

   /***** Create notification about favourite post
	  for the author of the post *****/
   TL_Ntf_CreateNotifToAuthor (Com->UsrCod,Com->PubCod,
			       Ntf_EVENT_TIMELINE_FAV);

   /***** Free image *****/
   Med_MediaDestructor (&Com->Content.Media);
  }

static void TL_Fav_UnfComm (struct TL_Com_Comment *Com)
  {
   extern const char *Txt_The_comment_no_longer_exists;

   /***** Initialize image *****/
   Med_MediaConstructor (&Com->Content.Media);

   /***** Get data of comment *****/
   Com->PubCod = TL_Pub_GetParamPubCod ();
   TL_Com_GetDataOfCommByCod (Com);

   /***** Trivial check 1: publication code should be > 0 *****/
   if (Com->PubCod <= 0)
     {
      Med_MediaDestructor (&Com->Content.Media);
      Ale_ShowAlert (Ale_WARNING,Txt_The_comment_no_longer_exists);
      return;
     }

   /***** Trivial check 2: I must be logged *****/
   if (!Gbl.Usrs.Me.Logged)
     {
      Med_MediaDestructor (&Com->Content.Media);
      Ale_ShowAlert (Ale_ERROR,"You are not logged.");
      return;
     }

   /***** Trivial check 3: The author can not fav its own notes *****/
   if (Usr_ItsMe (Com->UsrCod))
     {
      Med_MediaDestructor (&Com->Content.Media);
      Ale_ShowAlert (Ale_ERROR,"You can not fav/unfav your own comments.");
      return;
     }

   /***** Trivial check 4: Have I faved this comment? *****/
   if (!TL_DB_CheckIfFavedByUsr (TL_Fav_COMM,Com->PubCod,
				 Gbl.Usrs.Me.UsrDat.UsrCod))
     {
      Med_MediaDestructor (&Com->Content.Media);
      // Don't show error message
      return;
     }

   /***** Delete the mark as favourite from database *****/
   TL_DB_UnmarkAsFav (TL_Fav_COMM,Com->PubCod);

   /***** Update number of times this comment is favourited *****/
   Com->NumFavs = TL_DB_GetNumTimesHasBeenFav (TL_Fav_COMM,
					       Com->PubCod,Com->UsrCod);

   /***** Mark possible notifications on this comment as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV,Com->PubCod);

   /***** Free image *****/
   Med_MediaDestructor (&Com->Content.Media);
  }

/*****************************************************************************/
/**************** Put icon to fav/unfav and list of favers *******************/
/*****************************************************************************/

void TL_Fav_PutIconToFavUnf (TL_Fav_WhatToFav_t WhatToFav,
                             long Cod,long UsrCod,unsigned NumFavs,
                             TL_Usr_HowManyUsrs_t HowManyUsrs)
  {
   /***** Put form to fav/unfav this comment *****/
   /* Begin container */
   HTM_DIV_Begin ("class=\"TL_ICO\"");

      /* Icon to fav/unfav */
      if (Usr_ItsMe (UsrCod))	// I am the author ==> I can not fav/unfav
	 TL_Fav_PutDisabledIconFav (NumFavs);
      else			// I am not the author
	 TL_Fav_PutFormToFavUnf (WhatToFav,Cod);

   /* End container */
   HTM_DIV_End ();

   /***** Show who have marked this comment as favourite *****/
   TL_Fav_ShowUsrsWhoHaveMarkedAsFav (WhatToFav,Cod,UsrCod,NumFavs,HowManyUsrs);
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

static void TL_Fav_PutFormToFavUnf (TL_Fav_WhatToFav_t WhatToFav,long Cod)
  {
   extern const char *Txt_TIMELINE_Favourite;
   extern const char *Txt_TIMELINE_Mark_as_favourite;
   struct TL_Form Form[TL_Fav_NUM_WHAT_TO_FAV][2] =
     {
      [TL_Fav_NOTE] =
	{
	 [false] = // I have not faved ==> fav
	   {
	    .Action      = TL_Frm_FAV_NOTE,
	    .ParamFormat = "NotCod=%ld",
	    .ParamCod    = Cod,
	    .Icon        = TL_Fav_ICON_FAV,
	    .Title       = Txt_TIMELINE_Mark_as_favourite,
	   },
	 [true] = // I have faved ==> unfav
	   {
	    .Action      = TL_Frm_UNF_NOTE,
	    .ParamFormat = "NotCod=%ld",
	    .ParamCod    = Cod,
	    .Icon        = TL_Fav_ICON_FAVED,
	    .Title       = Txt_TIMELINE_Favourite,
	   },
	},
      [TL_Fav_COMM] =
	{
	 [false] = // I have not faved ==> fav
	   {
	    .Action      = TL_Frm_FAV_COMM,
	    .ParamFormat = "PubCod=%ld",
	    .ParamCod    = Cod,
	    .Icon        = TL_Fav_ICON_FAV,
	    .Title       = Txt_TIMELINE_Mark_as_favourite,
	   },
	 [true] = // I have faved ==> unfav
	   {
	    .Action      = TL_Frm_UNF_COMM,
	    .ParamFormat = "PubCod=%ld",
	    .ParamCod    = Cod,
	    .Icon        = TL_Fav_ICON_FAVED,
	    .Title       = Txt_TIMELINE_Favourite,
	   },
	},
     };

   /***** Form and icon to fav/unfav note *****/
   TL_Frm_FormFavSha (&Form[WhatToFav][TL_DB_CheckIfFavedByUsr (WhatToFav,Cod,Gbl.Usrs.Me.UsrDat.UsrCod)]);
  }

/*****************************************************************************/
/********** Show users who have marked a note/comment as favourite ***********/
/*****************************************************************************/

static void TL_Fav_ShowUsrsWhoHaveMarkedAsFav (TL_Fav_WhatToFav_t WhatToFav,
                                               long Cod,long UsrCod,
                                               unsigned NumFavs,
					       TL_Usr_HowManyUsrs_t HowManyUsrs)
  {
   static const TL_Frm_Action_t Action[TL_Fav_NUM_WHAT_TO_FAV] =
     {
      [TL_Fav_NOTE] = TL_Frm_ALL_FAV_NOTE,
      [TL_Fav_COMM] = TL_Frm_ALL_FAV_COMM,
     };
   static const char *ParamFormat[TL_Fav_NUM_WHAT_TO_FAV] =
     {
      [TL_Fav_NOTE] = "NotCod=%ld",
      [TL_Fav_COMM] = "PubCod=%ld",
     };
   MYSQL_RES *mysql_res;
   unsigned NumFirstUsrs;

   /***** Get users who have marked this note as favourite *****/
   if (NumFavs)
      /***** Get list of users from database *****/
      NumFirstUsrs =
      TL_DB_GetListUsrsHaveFaved (WhatToFav,Cod,UsrCod,
                                  HowManyUsrs == TL_Usr_SHOW_FEW_USRS ? TL_Usr_DEF_USRS_SHOWN :
				                                        TL_Usr_MAX_USRS_SHOWN,
                                  &mysql_res);
   else
      NumFirstUsrs = 0;

   /***** Show users *****/
   /* Number of users */
   HTM_DIV_Begin ("class=\"TL_NUM_USRS\"");
      TL_Usr_ShowNumSharersOrFavers (NumFavs);
   HTM_DIV_End ();

   /* List users one by one */
   HTM_DIV_Begin ("class=\"TL_USRS\"");
      TL_Usr_ShowSharersOrFavers (&mysql_res,NumFavs,NumFirstUsrs);
      if (NumFirstUsrs < NumFavs)		// Not all are shown
	 /* Clickable ellipsis to show all users */
	 TL_Frm_PutFormToSeeAllFaversSharers (Action[WhatToFav],
					      ParamFormat[WhatToFav],Cod,
					      HowManyUsrs);
   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   if (NumFavs)
      DB_FreeMySQLResult (&mysql_res);
  }
