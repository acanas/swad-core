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
   TL_Fav_PutIconToFavUnf (TL_Usr_FAV_UNF_NOTE,
                           Not.NotCod,Not.UsrCod,Not.NumFavs,
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
   TL_Fav_PutIconToFavUnf (TL_Usr_FAV_UNF_NOTE,
                           Not.NotCod,Not.UsrCod,Not.NumFavs,
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
   TL_Fav_PutIconToFavUnf (TL_Usr_FAV_UNF_NOTE,
                           Not.NotCod,Not.UsrCod,Not.NumFavs,
                           TL_Usr_SHOW_FEW_USRS);
  }

static void TL_Fav_FavNote (struct TL_Not_Note *Not)
  {
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = TL_Not_GetParamNotCod ();
   TL_Not_GetDataOfNoteByCod (Not);

   /***** Do some checks *****/
   if (!TL_Usr_CheckICanFavSha (Not->NotCod,Not->UsrCod))
      return;

   /***** Trivial check: Have I faved this note? *****/
   if (TL_Usr_CheckIfFavedSharedByUsr (TL_Usr_FAV_UNF_NOTE,Not->NotCod,
                                       Gbl.Usrs.Me.UsrDat.UsrCod))
      return;

   /***** Mark note as favourite in database *****/
   TL_DB_MarkAsFav (TL_Usr_FAV_UNF_NOTE,Not->NotCod);

   /***** Update number of times this note is favourited *****/
   Not->NumFavs = TL_DB_GetNumFavers (TL_Usr_FAV_UNF_NOTE,
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
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = TL_Not_GetParamNotCod ();
   TL_Not_GetDataOfNoteByCod (Not);

   /***** Do some checks *****/
   if (!TL_Usr_CheckICanFavSha (Not->NotCod,Not->UsrCod))
      return;

   /***** Trivial check: Have I faved this note? *****/
   if (!TL_Usr_CheckIfFavedSharedByUsr (TL_Usr_FAV_UNF_NOTE,Not->NotCod,
                                        Gbl.Usrs.Me.UsrDat.UsrCod))
      return;

   /***** Delete the mark as favourite from database *****/
   TL_DB_UnmarkAsFav (TL_Usr_FAV_UNF_NOTE,Not->NotCod);

   /***** Update number of times this note is favourited *****/
   Not->NumFavs = TL_DB_GetNumFavers (TL_Usr_FAV_UNF_NOTE,
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
   TL_Fav_PutIconToFavUnf (TL_Usr_FAV_UNF_COMM,
                           Com.PubCod,Com.UsrCod,Com.NumFavs,
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
   TL_Fav_PutIconToFavUnf (TL_Usr_FAV_UNF_COMM,
                           Com.PubCod,Com.UsrCod,Com.NumFavs,
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
   TL_Fav_PutIconToFavUnf (TL_Usr_FAV_UNF_COMM,
                           Com.PubCod,Com.UsrCod,Com.NumFavs,
                           TL_Usr_SHOW_FEW_USRS);
  }

static void TL_Fav_FavComm (struct TL_Com_Comment *Com)
  {
   /***** Initialize image *****/
   Med_MediaConstructor (&Com->Content.Media);

   /***** Get data of comment *****/
   Com->PubCod = TL_Pub_GetParamPubCod ();
   TL_Com_GetDataOfCommByCod (Com);

   /***** Do some checks *****/
   if (!TL_Usr_CheckICanFavSha (Com->PubCod,Com->UsrCod))
     {
      Med_MediaDestructor (&Com->Content.Media);
      return;
     }

   /***** Trivial check: Have I faved this comment? *****/
   if (TL_Usr_CheckIfFavedSharedByUsr (TL_Usr_FAV_UNF_COMM,Com->PubCod,
				       Gbl.Usrs.Me.UsrDat.UsrCod))
     {
      Med_MediaDestructor (&Com->Content.Media);
      return;
     }

   /***** Mark comment as favourite in database *****/
   TL_DB_MarkAsFav (TL_Usr_FAV_UNF_COMM,Com->PubCod);

   /***** Update number of times this comment is favourited *****/
   Com->NumFavs = TL_DB_GetNumFavers (TL_Usr_FAV_UNF_COMM,
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
   /***** Initialize image *****/
   Med_MediaConstructor (&Com->Content.Media);

   /***** Get data of comment *****/
   Com->PubCod = TL_Pub_GetParamPubCod ();
   TL_Com_GetDataOfCommByCod (Com);

   /***** Do some checks *****/
   if (!TL_Usr_CheckICanFavSha (Com->PubCod,Com->UsrCod))
     {
      Med_MediaDestructor (&Com->Content.Media);
      return;
     }

   /***** Trivial check: Have I faved this comment? *****/
   if (!TL_Usr_CheckIfFavedSharedByUsr (TL_Usr_FAV_UNF_COMM,Com->PubCod,
				        Gbl.Usrs.Me.UsrDat.UsrCod))
     {
      Med_MediaDestructor (&Com->Content.Media);
      return;
     }

   /***** Delete the mark as favourite from database *****/
   TL_DB_UnmarkAsFav (TL_Usr_FAV_UNF_COMM,Com->PubCod);

   /***** Update number of times this comment is favourited *****/
   Com->NumFavs = TL_DB_GetNumFavers (TL_Usr_FAV_UNF_COMM,
				      Com->PubCod,Com->UsrCod);

   /***** Mark possible notifications on this comment as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV,Com->PubCod);

   /***** Free image *****/
   Med_MediaDestructor (&Com->Content.Media);
  }

/*****************************************************************************/
/**************** Put icon to fav/unfav and list of favers *******************/
/*****************************************************************************/

void TL_Fav_PutIconToFavUnf (TL_Usr_FavSha_t FavSha,
                             long Cod,long UsrCod,unsigned NumUsrs,
                             TL_Usr_HowManyUsrs_t HowManyUsrs)
  {
   /***** Put form to fav/unfav this comment *****/
   /* Begin container */
   HTM_DIV_Begin ("class=\"TL_ICO\"");

      /* Icon to fav/unfav */
      if (Usr_ItsMe (UsrCod))	// I am the author ==> I can not fav/unfav
         TL_Usr_PutDisabledIconFavSha (FavSha,NumUsrs);
      else			// I am not the author
	 TL_Frm_PutFormToFavUnfShaUns (FavSha,Cod);

   /* End container */
   HTM_DIV_End ();

   /***** Show who have faved this note/comment *****/
   TL_Usr_GetAndShowSharersOrFavers (FavSha,Cod,UsrCod,NumUsrs,HowManyUsrs);
  }
