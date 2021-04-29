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

static void Tml_Sha_ShaNote (struct Tml_Not_Note *Not);
static void Tml_Sha_UnsNote (struct Tml_Not_Note *Not);

/*****************************************************************************/
/****************************** Show all sharers *****************************/
/*****************************************************************************/

void Tml_Sha_ShowAllSharersNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show all sharers *****/
   Tml_Sha_ShowAllSharersNoteGbl ();
  }

void Tml_Sha_ShowAllSharersNoteGbl (void)
  {
   struct Tml_Not_Note Not;

   /***** Get data of note *****/
   Not.NotCod = Tml_Not_GetParamNotCod ();
   Tml_Not_GetDataOfNoteByCod (&Not);

   /***** Write HTML inside DIV with form to share/unshare *****/
   Tml_Usr_PutIconFavSha (Tml_Usr_SHA_UNS_NOTE,
	                  Not.NotCod,Not.UsrCod,Not.NumShared,
	                  Tml_Usr_SHOW_ALL_USRS);
  }

/*****************************************************************************/
/******************************** Share a note *******************************/
/*****************************************************************************/

void Tml_Sha_ShaNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Share note *****/
   Tml_Sha_ShaNoteGbl ();
  }

void Tml_Sha_ShaNoteGbl (void)
  {
   struct Tml_Not_Note Not;

   /***** Share note *****/
   Tml_Sha_ShaNote (&Not);

   /***** Write HTML inside DIV with form to unshare *****/
   Tml_Usr_PutIconFavSha (Tml_Usr_SHA_UNS_NOTE,
	                  Not.NotCod,Not.UsrCod,Not.NumShared,
	                  Tml_Usr_SHOW_FEW_USRS);
  }

static void Tml_Sha_ShaNote (struct Tml_Not_Note *Not)
  {
   struct Tml_Pub_Publication Pub;
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = Tml_Not_GetParamNotCod ();
   Tml_Not_GetDataOfNoteByCod (Not);

   /***** Do some checks *****/
   if (!Tml_Usr_CheckIfICanFavSha (Not->NotCod,Not->UsrCod))
      return;

   /***** Trivial check: Is note already shared by me? *****/
   if (Tml_Usr_CheckIfFavedSharedByUsr (Tml_Usr_SHA_UNS_NOTE,Not->NotCod,
                                        Gbl.Usrs.Me.UsrDat.UsrCod))
      return;

   /***** Share (publish note in timeline) *****/
   Pub.NotCod       = Not->NotCod;
   Pub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Pub.PubType      = Tml_Pub_SHARED_NOTE;
   Tml_Pub_PublishPubInTimeline (&Pub);	// Set Pub.PubCod

   /***** Update number of times this note is shared *****/
   Not->NumShared = Tml_DB_GetNumSharers (Not->NotCod,Not->UsrCod);

   /***** Create notification about shared post
	  for the author of the post *****/
   OriginalPubCod = Tml_DB_GetPubCodOfOriginalNote (Not->NotCod);
   if (OriginalPubCod > 0)
      Tml_Ntf_CreateNotifToAuthor (Not->UsrCod,OriginalPubCod,Ntf_EVENT_TL_SHARE);
  }

/*****************************************************************************/
/******************** Unshare a previously shared note ***********************/
/*****************************************************************************/

void Tml_Sha_UnsNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Unshare note *****/
   Tml_Sha_UnsNoteGbl ();
  }

void Tml_Sha_UnsNoteGbl (void)
  {
   struct Tml_Not_Note Not;

   /***** Unshare note *****/
   Tml_Sha_UnsNote (&Not);

   /***** Write HTML inside DIV with form to share *****/
   Tml_Usr_PutIconFavSha (Tml_Usr_SHA_UNS_NOTE,
	                  Not.NotCod,Not.UsrCod,Not.NumShared,
	                  Tml_Usr_SHOW_FEW_USRS);
  }

static void Tml_Sha_UnsNote (struct Tml_Not_Note *Not)
  {
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = Tml_Not_GetParamNotCod ();
   Tml_Not_GetDataOfNoteByCod (Not);

   /***** Do some checks *****/
   if (!Tml_Usr_CheckIfICanFavSha (Not->NotCod,Not->UsrCod))
      return;

   /***** Delete publication from database *****/
   Tml_DB_RemoveSharedPub (Not->NotCod);

   /***** Update number of times this note is shared *****/
   Not->NumShared = Tml_DB_GetNumSharers (Not->NotCod,Not->UsrCod);

   /***** Mark possible notifications on this note as removed *****/
   OriginalPubCod = Tml_DB_GetPubCodOfOriginalNote (Not->NotCod);
   if (OriginalPubCod > 0)
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TL_SHARE,OriginalPubCod);
  }
