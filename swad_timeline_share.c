// swad_timeline_share.c: social timeline shared

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
#include "swad_timeline_notification.h"
#include "swad_timeline_publication.h"
#include "swad_timeline_share.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TmlSha_ShaNote (struct TmlNot_Note *Not);
static void TmlSha_UnsNote (struct TmlNot_Note *Not);

/*****************************************************************************/
/****************************** Show all sharers *****************************/
/*****************************************************************************/

void TmlSha_ShowAllSharersNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show all sharers *****/
   TmlSha_ShowAllSharersNoteGbl ();
  }

void TmlSha_ShowAllSharersNoteGbl (void)
  {
   struct TmlNot_Note Not;

   /***** Get data of note *****/
   Not.NotCod = ParCod_GetAndCheckPar (ParCod_Not);
   TmlNot_GetNoteDataByCod (&Not);

   /***** Write HTML inside DIV with form to share/unshare *****/
   TmlUsr_PutIconFavSha (TmlUsr_SHA_UNS_NOTE,
	                 Not.NotCod,Not.UsrCod,Not.NumShared,
	                 TmlUsr_SHOW_ALL_USRS);
  }

/*****************************************************************************/
/******************************** Share a note *******************************/
/*****************************************************************************/

void TmlSha_ShaNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParOtherUsrCodEncryptedAndGetUsrData ();

   /***** Share note *****/
   TmlSha_ShaNoteGbl ();
  }

void TmlSha_ShaNoteGbl (void)
  {
   struct TmlNot_Note Not;

   /***** Share note *****/
   TmlSha_ShaNote (&Not);

   /***** Write HTML inside DIV with form to unshare *****/
   TmlUsr_PutIconFavSha (TmlUsr_SHA_UNS_NOTE,
	                 Not.NotCod,Not.UsrCod,Not.NumShared,
	                 TmlUsr_SHOW_FEW_USRS);
  }

static void TmlSha_ShaNote (struct TmlNot_Note *Not)
  {
   struct TmlPub_Publication Pub;
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = ParCod_GetAndCheckPar (ParCod_Not);
   TmlNot_GetNoteDataByCod (Not);

   /***** Do some checks *****/
   if (!TmlUsr_CheckIfICanFavSha (Not->NotCod,Not->UsrCod))
      return;

   /***** Trivial check: Is note already shared by me? *****/
   if (TmlUsr_CheckIfFavedSharedByUsr (TmlUsr_SHA_UNS_NOTE,Not->NotCod,
                                        Gbl.Usrs.Me.UsrDat.UsrCod))
      return;

   /***** Share (publish note in timeline) *****/
   Pub.NotCod       = Not->NotCod;
   Pub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Pub.Type         = TmlPub_SHARED_NOTE;
   TmlPub_PublishPubInTimeline (&Pub);	// Set Pub.PubCod

   /***** Update number of times this note is shared *****/
   Not->NumShared = Tml_DB_GetNumSharers (Not->NotCod,Not->UsrCod);

   /***** Create notification about shared post
	  for the author of the post *****/
   if ((OriginalPubCod = Tml_DB_GetPubCodOfOriginalNote (Not->NotCod)) > 0)
      TmlNtf_CreateNotifToAuthor (Not->UsrCod,OriginalPubCod,Ntf_EVENT_TML_SHARE);
  }

/*****************************************************************************/
/******************** Unshare a previously shared note ***********************/
/*****************************************************************************/

void TmlSha_UnsNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParOtherUsrCodEncryptedAndGetUsrData ();

   /***** Unshare note *****/
   TmlSha_UnsNoteGbl ();
  }

void TmlSha_UnsNoteGbl (void)
  {
   struct TmlNot_Note Not;

   /***** Unshare note *****/
   TmlSha_UnsNote (&Not);

   /***** Write HTML inside DIV with form to share *****/
   TmlUsr_PutIconFavSha (TmlUsr_SHA_UNS_NOTE,
	                 Not.NotCod,Not.UsrCod,Not.NumShared,
	                 TmlUsr_SHOW_FEW_USRS);
  }

static void TmlSha_UnsNote (struct TmlNot_Note *Not)
  {
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = ParCod_GetAndCheckPar (ParCod_Not);
   TmlNot_GetNoteDataByCod (Not);

   /***** Do some checks *****/
   if (!TmlUsr_CheckIfICanFavSha (Not->NotCod,Not->UsrCod))
      return;

   /***** Delete publication from database *****/
   Tml_DB_RemoveSharedPub (Not->NotCod);

   /***** Update number of times this note is shared *****/
   Not->NumShared = Tml_DB_GetNumSharers (Not->NotCod,Not->UsrCod);

   /***** Mark possible notifications on this note as removed *****/
   if ((OriginalPubCod = Tml_DB_GetPubCodOfOriginalNote (Not->NotCod)) > 0)
      Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_TML_SHARE,OriginalPubCod);
  }
