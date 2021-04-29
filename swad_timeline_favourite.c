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

static void Tml_Fav_FavNote (struct Tml_Not_Note *Not);
static void Tml_Fav_UnfNote (struct Tml_Not_Note *Not);

static void Tml_Fav_FavComm (struct Tml_Com_Comment *Com);
static void Tml_Fav_UnfComm (struct Tml_Com_Comment *Com);

/*****************************************************************************/
/************************** Show all favers of a note ************************/
/*****************************************************************************/

void Tml_Fav_ShowAllFaversNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show all favers *****/
   Tml_Fav_ShowAllFaversNoteGbl ();
  }

void Tml_Fav_ShowAllFaversNoteGbl (void)
  {
   struct Tml_Not_Note Not;

   /***** Get data of note *****/
   Not.NotCod = Tml_Not_GetParamNotCod ();
   Tml_Not_GetDataOfNoteByCod (&Not);

   /***** Write HTML inside DIV with form to fav/unfav *****/
   Tml_Usr_PutIconFavSha (Tml_Usr_FAV_UNF_NOTE,
                          Not.NotCod,Not.UsrCod,Not.NumFavs,
                          Tml_Usr_SHOW_ALL_USRS);
  }

/*****************************************************************************/
/********************** Mark/unmark a note as favourite **********************/
/*****************************************************************************/

void Tml_Fav_FavNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Mark note as favourite *****/
   Tml_Fav_FavNoteGbl ();
  }

void Tml_Fav_FavNoteGbl (void)
  {
   struct Tml_Not_Note Not;

   /***** Mark note as favourite *****/
   Tml_Fav_FavNote (&Not);

   /***** Write HTML inside DIV with form to unfav *****/
   Tml_Usr_PutIconFavSha (Tml_Usr_FAV_UNF_NOTE,
                          Not.NotCod,Not.UsrCod,Not.NumFavs,
                          Tml_Usr_SHOW_FEW_USRS);
  }

void Tml_Fav_UnfNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Unfav a note previously marked as favourite *****/
   Tml_Fav_UnfNoteGbl ();
  }

void Tml_Fav_UnfNoteGbl (void)
  {
   struct Tml_Not_Note Not;

   /***** Stop marking as favourite a previously favourited note *****/
   Tml_Fav_UnfNote (&Not);

   /***** Write HTML inside DIV with form to fav *****/
   Tml_Usr_PutIconFavSha (Tml_Usr_FAV_UNF_NOTE,
                          Not.NotCod,Not.UsrCod,Not.NumFavs,
                          Tml_Usr_SHOW_FEW_USRS);
  }

static void Tml_Fav_FavNote (struct Tml_Not_Note *Not)
  {
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = Tml_Not_GetParamNotCod ();
   Tml_Not_GetDataOfNoteByCod (Not);

   /***** Do some checks *****/
   if (!Tml_Usr_CheckIfICanFavSha (Not->NotCod,Not->UsrCod))
      return;

   /***** Trivial check: Have I faved this note? *****/
   if (Tml_Usr_CheckIfFavedSharedByUsr (Tml_Usr_FAV_UNF_NOTE,Not->NotCod,
                                        Gbl.Usrs.Me.UsrDat.UsrCod))
      return;

   /***** Mark note as favourite in database *****/
   Tml_DB_MarkAsFav (Tml_Usr_FAV_UNF_NOTE,Not->NotCod);

   /***** Update number of times this note is favourited *****/
   Not->NumFavs = Tml_DB_GetNumFavers (Tml_Usr_FAV_UNF_NOTE,
				       Not->NotCod,Not->UsrCod);

   /***** Create notification about favourite post
	  for the author of the post *****/
   OriginalPubCod = Tml_DB_GetPubCodOfOriginalNote (Not->NotCod);
   if (OriginalPubCod > 0)
      Tml_Ntf_CreateNotifToAuthor (Not->UsrCod,OriginalPubCod,Ntf_EVENT_TL_FAV);
  }

static void Tml_Fav_UnfNote (struct Tml_Not_Note *Not)
  {
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = Tml_Not_GetParamNotCod ();
   Tml_Not_GetDataOfNoteByCod (Not);

   /***** Do some checks *****/
   if (!Tml_Usr_CheckIfICanFavSha (Not->NotCod,Not->UsrCod))
      return;

   /***** Trivial check: Have I faved this note? *****/
   if (!Tml_Usr_CheckIfFavedSharedByUsr (Tml_Usr_FAV_UNF_NOTE,Not->NotCod,
                                         Gbl.Usrs.Me.UsrDat.UsrCod))
      return;

   /***** Delete the mark as favourite from database *****/
   Tml_DB_UnmarkAsFav (Tml_Usr_FAV_UNF_NOTE,Not->NotCod);

   /***** Update number of times this note is favourited *****/
   Not->NumFavs = Tml_DB_GetNumFavers (Tml_Usr_FAV_UNF_NOTE,
			               Not->NotCod,Not->UsrCod);

   /***** Mark possible notifications on this note as removed *****/
   OriginalPubCod = Tml_DB_GetPubCodOfOriginalNote (Not->NotCod);
   if (OriginalPubCod > 0)
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TL_FAV,OriginalPubCod);
  }

/*****************************************************************************/
/************************** Show all favers of a note ************************/
/*****************************************************************************/

void Tml_Fav_ShowAllFaversComUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show all favers *****/
   Tml_Fav_ShowAllFaversComGbl ();
  }

void Tml_Fav_ShowAllFaversComGbl (void)
  {
   struct Tml_Com_Comment Com;

   /***** Get data of comment *****/
   Med_MediaConstructor (&Com.Content.Media);
   Com.PubCod = Tml_Pub_GetParamPubCod ();
   Tml_Com_GetDataOfCommByCod (&Com);
   Med_MediaDestructor (&Com.Content.Media);

   /***** Write HTML inside DIV with form to fav/unfav *****/
   Tml_Usr_PutIconFavSha (Tml_Usr_FAV_UNF_COMM,
                          Com.PubCod,Com.UsrCod,Com.NumFavs,
                          Tml_Usr_SHOW_ALL_USRS);
  }

/*****************************************************************************/
/********************* Mark/unmark a comment as favourite ********************/
/*****************************************************************************/

void Tml_Fav_FavCommUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Mark comment as favourite *****/
   Tml_Fav_FavCommGbl ();
  }

void Tml_Fav_FavCommGbl (void)
  {
   struct Tml_Com_Comment Com;

   /***** Mark comment as favourite *****/
   Tml_Fav_FavComm (&Com);

   /***** Write HTML inside DIV with form to unfav *****/
   Tml_Usr_PutIconFavSha (Tml_Usr_FAV_UNF_COMM,
                          Com.PubCod,Com.UsrCod,Com.NumFavs,
                          Tml_Usr_SHOW_FEW_USRS);
  }

void Tml_Fav_UnfCommUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Unfav a comment previously marked as favourite *****/
   Tml_Fav_UnfCommGbl ();
  }

void Tml_Fav_UnfCommGbl (void)
  {
   struct Tml_Com_Comment Com;

   /***** Stop marking as favourite a previously favourited comment *****/
   Tml_Fav_UnfComm (&Com);

   /***** Write HTML inside DIV with form to fav *****/
   Tml_Usr_PutIconFavSha (Tml_Usr_FAV_UNF_COMM,
                          Com.PubCod,Com.UsrCod,Com.NumFavs,
                          Tml_Usr_SHOW_FEW_USRS);
  }

static void Tml_Fav_FavComm (struct Tml_Com_Comment *Com)
  {
   /***** Initialize image *****/
   Med_MediaConstructor (&Com->Content.Media);

   /***** Get data of comment *****/
   Com->PubCod = Tml_Pub_GetParamPubCod ();
   Tml_Com_GetDataOfCommByCod (Com);

   /***** Do some checks *****/
   if (!Tml_Usr_CheckIfICanFavSha (Com->PubCod,Com->UsrCod))
     {
      Med_MediaDestructor (&Com->Content.Media);
      return;
     }

   /***** Trivial check: Have I faved this comment? *****/
   if (Tml_Usr_CheckIfFavedSharedByUsr (Tml_Usr_FAV_UNF_COMM,Com->PubCod,
				        Gbl.Usrs.Me.UsrDat.UsrCod))
     {
      Med_MediaDestructor (&Com->Content.Media);
      return;
     }

   /***** Mark comment as favourite in database *****/
   Tml_DB_MarkAsFav (Tml_Usr_FAV_UNF_COMM,Com->PubCod);

   /***** Update number of times this comment is favourited *****/
   Com->NumFavs = Tml_DB_GetNumFavers (Tml_Usr_FAV_UNF_COMM,
				       Com->PubCod,Com->UsrCod);

   /***** Create notification about favourite post
	  for the author of the post *****/
   Tml_Ntf_CreateNotifToAuthor (Com->UsrCod,Com->PubCod,Ntf_EVENT_TL_FAV);

   /***** Free image *****/
   Med_MediaDestructor (&Com->Content.Media);
  }

static void Tml_Fav_UnfComm (struct Tml_Com_Comment *Com)
  {
   /***** Initialize image *****/
   Med_MediaConstructor (&Com->Content.Media);

   /***** Get data of comment *****/
   Com->PubCod = Tml_Pub_GetParamPubCod ();
   Tml_Com_GetDataOfCommByCod (Com);

   /***** Do some checks *****/
   if (!Tml_Usr_CheckIfICanFavSha (Com->PubCod,Com->UsrCod))
     {
      Med_MediaDestructor (&Com->Content.Media);
      return;
     }

   /***** Trivial check: Have I faved this comment? *****/
   if (!Tml_Usr_CheckIfFavedSharedByUsr (Tml_Usr_FAV_UNF_COMM,Com->PubCod,
				         Gbl.Usrs.Me.UsrDat.UsrCod))
     {
      Med_MediaDestructor (&Com->Content.Media);
      return;
     }

   /***** Delete the mark as favourite from database *****/
   Tml_DB_UnmarkAsFav (Tml_Usr_FAV_UNF_COMM,Com->PubCod);

   /***** Update number of times this comment is favourited *****/
   Com->NumFavs = Tml_DB_GetNumFavers (Tml_Usr_FAV_UNF_COMM,
				       Com->PubCod,Com->UsrCod);

   /***** Mark possible notifications on this comment as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_TL_FAV,Com->PubCod);

   /***** Free image *****/
   Med_MediaDestructor (&Com->Content.Media);
  }
