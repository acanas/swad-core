// swad_timeline_favourite.c: social timeline favourites

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
#include "swad_notification_database.h"
#include "swad_parameter_code.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_favourite.h"
#include "swad_timeline_notification.h"
#include "swad_timeline_publication.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TmlFav_FavNote (struct TmlNot_Note *Not);
static void TmlFav_UnfNote (struct TmlNot_Note *Not);

static void TmlFav_FavComm (struct TmlCom_Comment *Com);
static void TmlFav_UnfComm (struct TmlCom_Comment *Com);

/*****************************************************************************/
/************************** Show all favers of a note ************************/
/*****************************************************************************/

void TmlFav_ShowAllFaversNoteUsr (void)
  {
   __attribute__((unused)) Exi_Exist_t UsrExists;

   /***** Get user whom profile is displayed *****/
   UsrExists = Usr_GetParOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show all favers *****/
   TmlFav_ShowAllFaversNoteGbl ();
  }

void TmlFav_ShowAllFaversNoteGbl (void)
  {
   struct TmlNot_Note Not;

   /***** Get data of note *****/
   Not.NotCod = ParCod_GetAndCheckPar (ParCod_Not);
   TmlNot_GetNoteDataByCod (&Not);

   /***** Write HTML inside DIV with form to fav/unfav *****/
   TmlUsr_PutIconFavSha (TmlUsr_FAV_UNF_NOTE,
                         Not.NotCod,Not.UsrCod,Not.NumFavs,
                         TmlUsr_SHOW_ALL_USRS);
  }

/*****************************************************************************/
/********************** Mark/unmark a note as favourite **********************/
/*****************************************************************************/

void TmlFav_FavNoteUsr (void)
  {
   __attribute__((unused)) Exi_Exist_t UsrExists;

   /***** Get user whom profile is displayed *****/
   UsrExists = Usr_GetParOtherUsrCodEncryptedAndGetUsrData ();

   /***** Mark note as favourite *****/
   TmlFav_FavNoteGbl ();
  }

void TmlFav_FavNoteGbl (void)
  {
   struct TmlNot_Note Not;

   /***** Mark note as favourite *****/
   TmlFav_FavNote (&Not);

   /***** Write HTML inside DIV with form to unfav *****/
   TmlUsr_PutIconFavSha (TmlUsr_FAV_UNF_NOTE,
                         Not.NotCod,Not.UsrCod,Not.NumFavs,
                         TmlUsr_SHOW_FEW_USRS);
  }

void TmlFav_UnfNoteUsr (void)
  {
   __attribute__((unused)) Exi_Exist_t UsrExists;

   /***** Get user whom profile is displayed *****/
   UsrExists = Usr_GetParOtherUsrCodEncryptedAndGetUsrData ();

   /***** Unfav a note previously marked as favourite *****/
   TmlFav_UnfNoteGbl ();
  }

void TmlFav_UnfNoteGbl (void)
  {
   struct TmlNot_Note Not;

   /***** Stop marking as favourite a previously favourited note *****/
   TmlFav_UnfNote (&Not);

   /***** Write HTML inside DIV with form to fav *****/
   TmlUsr_PutIconFavSha (TmlUsr_FAV_UNF_NOTE,
                         Not.NotCod,Not.UsrCod,Not.NumFavs,
                         TmlUsr_SHOW_FEW_USRS);
  }

static void TmlFav_FavNote (struct TmlNot_Note *Not)
  {
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = ParCod_GetAndCheckPar (ParCod_Not);
   TmlNot_GetNoteDataByCod (Not);

   /***** Do some checks *****/
   if (TmlUsr_CheckIfICanFavSha (Not->NotCod,Not->UsrCod) == Usr_CAN_NOT)
      return;

   /***** Trivial check: Have I faved this note? *****/
   if (TmlUsr_CheckIfFavedSharedByUsr (TmlUsr_FAV_UNF_NOTE,Not->NotCod,
                                       Gbl.Usrs.Me.UsrDat.UsrCod))
      return;

   /***** Mark note as favourite in database *****/
   Tml_DB_MarkAsFav (TmlUsr_FAV_UNF_NOTE,Not->NotCod);

   /***** Update number of times this note is favourited *****/
   Not->NumFavs = Tml_DB_GetNumFavers (TmlUsr_FAV_UNF_NOTE,
				       Not->NotCod,Not->UsrCod);

   /***** Create notification about favourite post
	  for the author of the post *****/
   if ((OriginalPubCod = Tml_DB_GetPubCodOfOriginalNote (Not->NotCod)) > 0)
      TmlNtf_CreateNotifToAuthor (Not->UsrCod,OriginalPubCod,Ntf_EVENT_TML_FAV);
  }

static void TmlFav_UnfNote (struct TmlNot_Note *Not)
  {
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = ParCod_GetAndCheckPar (ParCod_Not);
   TmlNot_GetNoteDataByCod (Not);

   /***** Do some checks *****/
   if (TmlUsr_CheckIfICanFavSha (Not->NotCod,Not->UsrCod) == Usr_CAN_NOT)
      return;

   /***** Trivial check: Have I faved this note? *****/
   if (!TmlUsr_CheckIfFavedSharedByUsr (TmlUsr_FAV_UNF_NOTE,Not->NotCod,
                                         Gbl.Usrs.Me.UsrDat.UsrCod))
      return;

   /***** Delete the mark as favourite from database *****/
   Tml_DB_UnmarkAsFav (TmlUsr_FAV_UNF_NOTE,Not->NotCod);

   /***** Update number of times this note is favourited *****/
   Not->NumFavs = Tml_DB_GetNumFavers (TmlUsr_FAV_UNF_NOTE,
			               Not->NotCod,Not->UsrCod);

   /***** Mark possible notifications on this note as removed *****/
   if ((OriginalPubCod = Tml_DB_GetPubCodOfOriginalNote (Not->NotCod)) > 0)
      Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_TML_FAV,OriginalPubCod);
  }

/*****************************************************************************/
/************************** Show all favers of a note ************************/
/*****************************************************************************/

void TmlFav_ShowAllFaversComUsr (void)
  {
   __attribute__((unused)) Exi_Exist_t UsrExists;

   /***** Get user whom profile is displayed *****/
   UsrExists = Usr_GetParOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show all favers *****/
   TmlFav_ShowAllFaversComGbl ();
  }

void TmlFav_ShowAllFaversComGbl (void)
  {
   struct TmlCom_Comment Com;

   /***** Get data of comment *****/
   Med_MediaConstructor (&Com.Content.Media);
   Com.PubCod = ParCod_GetAndCheckPar (ParCod_Pub);
   TmlCom_GetCommDataByCod (&Com);
   Med_MediaDestructor (&Com.Content.Media);

   /***** Write HTML inside DIV with form to fav/unfav *****/
   TmlUsr_PutIconFavSha (TmlUsr_FAV_UNF_COMM,
                         Com.PubCod,Com.UsrCod,Com.NumFavs,
                         TmlUsr_SHOW_ALL_USRS);
  }

/*****************************************************************************/
/********************* Mark/unmark a comment as favourite ********************/
/*****************************************************************************/

void TmlFav_FavCommUsr (void)
  {
   __attribute__((unused)) Exi_Exist_t UsrExists;

   /***** Get user whom profile is displayed *****/
   UsrExists = Usr_GetParOtherUsrCodEncryptedAndGetUsrData ();

   /***** Mark comment as favourite *****/
   TmlFav_FavCommGbl ();
  }

void TmlFav_FavCommGbl (void)
  {
   struct TmlCom_Comment Com;

   /***** Mark comment as favourite *****/
   TmlFav_FavComm (&Com);

   /***** Write HTML inside DIV with form to unfav *****/
   TmlUsr_PutIconFavSha (TmlUsr_FAV_UNF_COMM,
                         Com.PubCod,Com.UsrCod,Com.NumFavs,
                         TmlUsr_SHOW_FEW_USRS);
  }

void TmlFav_UnfCommUsr (void)
  {
   __attribute__((unused)) Exi_Exist_t UsrExists;

   /***** Get user whom profile is displayed *****/
   UsrExists = Usr_GetParOtherUsrCodEncryptedAndGetUsrData ();

   /***** Unfav a comment previously marked as favourite *****/
   TmlFav_UnfCommGbl ();
  }

void TmlFav_UnfCommGbl (void)
  {
   struct TmlCom_Comment Com;

   /***** Stop marking as favourite a previously favourited comment *****/
   TmlFav_UnfComm (&Com);

   /***** Write HTML inside DIV with form to fav *****/
   TmlUsr_PutIconFavSha (TmlUsr_FAV_UNF_COMM,
                         Com.PubCod,Com.UsrCod,Com.NumFavs,
                         TmlUsr_SHOW_FEW_USRS);
  }

static void TmlFav_FavComm (struct TmlCom_Comment *Com)
  {
   /***** Initialize image *****/
   Med_MediaConstructor (&Com->Content.Media);

   /***** Get data of comment *****/
   Com->PubCod = ParCod_GetAndCheckPar (ParCod_Pub);
   TmlCom_GetCommDataByCod (Com);

   /***** Do some checks *****/
   if (TmlUsr_CheckIfICanFavSha (Com->PubCod,Com->UsrCod) == Usr_CAN_NOT)
     {
      Med_MediaDestructor (&Com->Content.Media);
      return;
     }

   /***** Trivial check: Have I faved this comment? *****/
   if (TmlUsr_CheckIfFavedSharedByUsr (TmlUsr_FAV_UNF_COMM,Com->PubCod,
				        Gbl.Usrs.Me.UsrDat.UsrCod))
     {
      Med_MediaDestructor (&Com->Content.Media);
      return;
     }

   /***** Mark comment as favourite in database *****/
   Tml_DB_MarkAsFav (TmlUsr_FAV_UNF_COMM,Com->PubCod);

   /***** Update number of times this comment is favourited *****/
   Com->NumFavs = Tml_DB_GetNumFavers (TmlUsr_FAV_UNF_COMM,
				       Com->PubCod,Com->UsrCod);

   /***** Create notification about favourite post
	  for the author of the post *****/
   TmlNtf_CreateNotifToAuthor (Com->UsrCod,Com->PubCod,Ntf_EVENT_TML_FAV);

   /***** Free image *****/
   Med_MediaDestructor (&Com->Content.Media);
  }

static void TmlFav_UnfComm (struct TmlCom_Comment *Com)
  {
   /***** Initialize image *****/
   Med_MediaConstructor (&Com->Content.Media);

   /***** Get data of comment *****/
   Com->PubCod = ParCod_GetAndCheckPar (ParCod_Pub);
   TmlCom_GetCommDataByCod (Com);

   /***** Do some checks *****/
   if (TmlUsr_CheckIfICanFavSha (Com->PubCod,Com->UsrCod) == Usr_CAN_NOT)
     {
      Med_MediaDestructor (&Com->Content.Media);
      return;
     }

   /***** Trivial check: Have I faved this comment? *****/
   if (!TmlUsr_CheckIfFavedSharedByUsr (TmlUsr_FAV_UNF_COMM,Com->PubCod,
				        Gbl.Usrs.Me.UsrDat.UsrCod))
     {
      Med_MediaDestructor (&Com->Content.Media);
      return;
     }

   /***** Delete the mark as favourite from database *****/
   Tml_DB_UnmarkAsFav (TmlUsr_FAV_UNF_COMM,Com->PubCod);

   /***** Update number of times this comment is favourited *****/
   Com->NumFavs = Tml_DB_GetNumFavers (TmlUsr_FAV_UNF_COMM,
				       Com->PubCod,Com->UsrCod);

   /***** Mark possible notifications on this comment as removed *****/
   Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_TML_FAV,Com->PubCod);

   /***** Free image *****/
   Med_MediaDestructor (&Com->Content.Media);
  }
