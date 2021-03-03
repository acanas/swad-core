// swad_timeline_share.c: social timeline shared

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
#include "swad_timeline_form.h"
#include "swad_timeline_notification.h"
#include "swad_timeline_publication.h"
#include "swad_timeline_share.h"

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

static void TL_Sha_ShaNote (struct TL_Not_Note *Not);
static void TL_Sha_UnsNote (struct TL_Not_Note *Not);

/*****************************************************************************/
/******************************** Share a note *******************************/
/*****************************************************************************/

void TL_Sha_ShowAllSharersNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show all sharers *****/
   TL_Sha_ShowAllSharersNoteGbl ();
  }

void TL_Sha_ShowAllSharersNoteGbl (void)
  {
   struct TL_Not_Note Not;

   /***** Get data of note *****/
   Not.NotCod = TL_Not_GetParamNotCod ();
   TL_Not_GetDataOfNoteByCod (&Not);

   /***** Write HTML inside DIV with form to share/unshare *****/
   TL_Usr_PutIconFavSha (TL_Usr_SHA_UNS_NOTE,
	                 Not.NotCod,Not.UsrCod,Not.NumShared,
	                 TL_Usr_SHOW_ALL_USRS);
  }

void TL_Sha_ShaNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Share note *****/
   TL_Sha_ShaNoteGbl ();
  }

void TL_Sha_ShaNoteGbl (void)
  {
   struct TL_Not_Note Not;

   /***** Share note *****/
   TL_Sha_ShaNote (&Not);

   /***** Write HTML inside DIV with form to unshare *****/
   TL_Usr_PutIconFavSha (TL_Usr_SHA_UNS_NOTE,
	                 Not.NotCod,Not.UsrCod,Not.NumShared,
	                 TL_Usr_SHOW_FEW_USRS);
  }

static void TL_Sha_ShaNote (struct TL_Not_Note *Not)
  {
   struct TL_Pub_Publication Pub;
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = TL_Not_GetParamNotCod ();
   TL_Not_GetDataOfNoteByCod (Not);

   /***** Do some checks *****/
   if (!TL_Usr_CheckICanFavSha (Not->NotCod,Not->UsrCod))
      return;

   /***** Trivial check: Is note already shared by me? *****/
   if (TL_Usr_CheckIfFavedSharedByUsr (TL_Usr_SHA_UNS_NOTE,Not->NotCod,
                                       Gbl.Usrs.Me.UsrDat.UsrCod))
      return;

   /***** Share (publish note in timeline) *****/
   Pub.NotCod       = Not->NotCod;
   Pub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Pub.PubType      = TL_Pub_SHARED_NOTE;
   TL_Pub_PublishPubInTimeline (&Pub);	// Set Pub.PubCod

   /***** Update number of times this note is shared *****/
   Not->NumShared = TL_DB_GetNumSharers (Not->NotCod,Not->UsrCod);

   /***** Create notification about shared post
	  for the author of the post *****/
   OriginalPubCod = TL_DB_GetPubCodOfOriginalNote (Not->NotCod);
   if (OriginalPubCod > 0)
      TL_Ntf_CreateNotifToAuthor (Not->UsrCod,OriginalPubCod,
				  Ntf_EVENT_TIMELINE_SHARE);
  }

/*****************************************************************************/
/******************** Unshare a previously shared note ***********************/
/*****************************************************************************/

void TL_Sha_UnsNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Unshare note *****/
   TL_Sha_UnsNoteGbl ();
  }

void TL_Sha_UnsNoteGbl (void)
  {
   struct TL_Not_Note Not;

   /***** Unshare note *****/
   TL_Sha_UnsNote (&Not);

   /***** Write HTML inside DIV with form to share *****/
   TL_Usr_PutIconFavSha (TL_Usr_SHA_UNS_NOTE,
	                 Not.NotCod,Not.UsrCod,Not.NumShared,
	                 TL_Usr_SHOW_FEW_USRS);
  }

static void TL_Sha_UnsNote (struct TL_Not_Note *Not)
  {
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = TL_Not_GetParamNotCod ();
   TL_Not_GetDataOfNoteByCod (Not);

   /***** Do some checks *****/
   if (!TL_Usr_CheckICanFavSha (Not->NotCod,Not->UsrCod))
      return;

   /***** Delete publication from database *****/
   TL_DB_RemoveSharedPub (Not->NotCod);

   /***** Update number of times this note is shared *****/
   Not->NumShared = TL_DB_GetNumSharers (Not->NotCod,Not->UsrCod);

   /***** Mark possible notifications on this note as removed *****/
   OriginalPubCod = TL_DB_GetPubCodOfOriginalNote (Not->NotCod);
   if (OriginalPubCod > 0)
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_SHARE,OriginalPubCod);
  }
