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
#include "swad_timeline_share.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

#define TL_ICON_SHARE		"share-alt.svg"
#define TL_ICON_SHARED		"share-alt-green.svg"

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

static void TL_Sha_PutDisabledIconShare (unsigned NumShared);

static void TL_Sha_PutFormToShaNote (long ParamCod);
static void TL_Sha_PutFormToUnsNote (long ParamCod);

static void TL_Sha_ShaNote (struct TL_Note *Not);
static void TL_Sha_UnsNote (struct TL_Note *Not);

static bool TL_Sha_CheckIfNoteIsSharedByUsr (long NotCod,long UsrCod);

static void TL_Sha_ShowUsrsWhoHaveSharedNote (const struct TL_Note *Not,
					      TL_HowManyUsrs_t HowManyUsrs);

/*****************************************************************************/
/*********************** Put disabled icon to share **************************/
/*****************************************************************************/

static void TL_Sha_PutDisabledIconShare (unsigned NumShared)
  {
   extern const char *Txt_TIMELINE_NOTE_Shared_by_X_USERS;
   extern const char *Txt_TIMELINE_NOTE_Not_shared_by_anyone;

   /***** Disabled icon to share *****/
   if (NumShared)
     {
      Ico_PutDivIcon ("TL_ICO_DISABLED",TL_ICON_SHARE,
		      Str_BuildStringLong (Txt_TIMELINE_NOTE_Shared_by_X_USERS,
					   (long) NumShared));
      Str_FreeString ();
     }
   else
      Ico_PutDivIcon ("TL_ICO_DISABLED",TL_ICON_SHARE,
		      Txt_TIMELINE_NOTE_Not_shared_by_anyone);
  }

/*****************************************************************************/
/*********************** Form to share/unshare note **************************/
/*****************************************************************************/

static void TL_Sha_PutFormToShaNote (long ParamCod)
  {
   extern const char *Txt_Share;

   /***** Form and icon to mark note as favourite *****/
   TL_FormFavSha (ActShaTL_NotGbl,ActShaTL_NotUsr,
                  "NotCod=%ld",ParamCod,
	          TL_ICON_SHARE,Txt_Share);
  }

static void TL_Sha_PutFormToUnsNote (long ParamCod)
  {
   extern const char *Txt_TIMELINE_NOTE_Shared;

   /***** Form and icon to mark note as favourite *****/
   TL_FormFavSha (ActUnsTL_NotGbl,ActUnsTL_NotUsr,
                  "NotCod=%ld",ParamCod,
	          TL_ICON_SHARED,Txt_TIMELINE_NOTE_Shared);
  }

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
   struct TL_Note Not;

   /***** Get data of note *****/
   Not.NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (&Not);

   /***** Write HTML inside DIV with form to share/unshare *****/
   TL_Sha_PutFormToShaUnsNote (&Not,TL_SHOW_ALL_USRS);
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
   struct TL_Note Not;

   /***** Share note *****/
   TL_Sha_ShaNote (&Not);

   /***** Write HTML inside DIV with form to unshare *****/
   TL_Sha_PutFormToShaUnsNote (&Not,TL_SHOW_FEW_USRS);
  }

static void TL_Sha_ShaNote (struct TL_Note *Not)
  {
   // extern const char *Txt_The_original_post_no_longer_exists;
   struct TL_Publication Pub;
   bool ItsMe;
   long OriginalPubCod;

   /***** Get data of note *****/
   Not->NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (Not);

   if (Not->NotCod > 0)
     {
      ItsMe = Usr_ItsMe (Not->UsrCod);
      if (Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
         if (!TL_Sha_CheckIfNoteIsSharedByUsr (Not->NotCod,
					    Gbl.Usrs.Me.UsrDat.UsrCod))	// Not yet shared by me
	   {
	    /***** Share (publish note in timeline) *****/
	    Pub.NotCod       = Not->NotCod;
	    Pub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
	    Pub.PubType      = TL_PUB_SHARED_NOTE;
	    TL_PublishNoteInTimeline (&Pub);	// Set Pub.PubCod

	    /* Update number of times this note is shared */
	    TL_Sha_UpdateNumTimesANoteHasBeenShared (Not);

	    /**** Create notification about shared post
		  for the author of the post ***/
	    OriginalPubCod = TL_GetPubCodOfOriginalNote (Not->NotCod);
	    if (OriginalPubCod > 0)
	       TL_CreateNotifToAuthor (Not->UsrCod,OriginalPubCod,Ntf_EVENT_TIMELINE_SHARE);
	   }
     }
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
   struct TL_Note Not;

   /***** Unshare note *****/
   TL_Sha_UnsNote (&Not);

   /***** Write HTML inside DIV with form to share *****/
   TL_Sha_PutFormToShaUnsNote (&Not,TL_SHOW_FEW_USRS);
  }

static void TL_Sha_UnsNote (struct TL_Note *Not)
  {
   // extern const char *Txt_The_original_post_no_longer_exists;
   long OriginalPubCod;
   bool ItsMe;

   /***** Get data of note *****/
   Not->NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (Not);

   if (Not->NotCod > 0)
     {
      ItsMe = Usr_ItsMe (Not->UsrCod);
      if (Not->NumShared &&
	  Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
	 if (TL_Sha_CheckIfNoteIsSharedByUsr (Not->NotCod,
					   Gbl.Usrs.Me.UsrDat.UsrCod))	// I am a sharer
	   {
	    /***** Delete publication from database *****/
	    DB_QueryDELETE ("can not remove a publication",
			    "DELETE FROM tl_pubs"
	                    " WHERE NotCod=%ld"
	                    " AND PublisherCod=%ld"
	                    " AND PubType=%u",
			    Not->NotCod,
			    Gbl.Usrs.Me.UsrDat.UsrCod,
			    (unsigned) TL_PUB_SHARED_NOTE);

	    /***** Update number of times this note is shared *****/
	    TL_Sha_UpdateNumTimesANoteHasBeenShared (Not);

            /***** Mark possible notifications on this note as removed *****/
	    OriginalPubCod = TL_GetPubCodOfOriginalNote (Not->NotCod);
	    if (OriginalPubCod > 0)
	       Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_SHARE,OriginalPubCod);
	   }
     }
  }

void TL_Sha_PutFormToShaUnsNote (const struct TL_Note *Not,
                                 TL_HowManyUsrs_t HowManyUsrs)
  {
   bool IAmTheAuthor;
   bool IAmASharerOfThisNot;

   /***** Put form to share/unshare this note *****/
   HTM_DIV_Begin ("class=\"TL_ICO\"");
   IAmTheAuthor = Usr_ItsMe (Not->UsrCod);
   if (Not->Unavailable ||		// Unavailable notes can not be shared
       IAmTheAuthor)			// I am the author
      /* Put disabled icon */
      TL_Sha_PutDisabledIconShare (Not->NumShared);
   else					// Available and I am not the author
     {
      /* Put icon to share/unshare */
      IAmASharerOfThisNot = TL_Sha_CheckIfNoteIsSharedByUsr (Not->NotCod,
							    Gbl.Usrs.Me.UsrDat.UsrCod);
      if (IAmASharerOfThisNot)	// I have shared this note
	 TL_Sha_PutFormToUnsNote (Not->NotCod);
      else				// I have not shared this note
	 TL_Sha_PutFormToShaNote (Not->NotCod);
     }
   HTM_DIV_End ();

   /***** Show who have shared this note *****/
   TL_Sha_ShowUsrsWhoHaveSharedNote (Not,HowManyUsrs);
  }

/*****************************************************************************/
/****************** Check if a user has published a note *********************/
/*****************************************************************************/

static bool TL_Sha_CheckIfNoteIsSharedByUsr (long NotCod,long UsrCod)
  {
   return (DB_QueryCOUNT ("can not check if a user has shared a note",
			  "SELECT COUNT(*) FROM tl_pubs"
			  " WHERE NotCod=%ld"
			  " AND PublisherCod=%ld"
			  " AND PubType=%u",
			  NotCod,
			  UsrCod,
			  (unsigned) TL_PUB_SHARED_NOTE) != 0);
  }

/*****************************************************************************/
/********** Get number of times a note has been shared in timeline ***********/
/*****************************************************************************/

void TL_Sha_UpdateNumTimesANoteHasBeenShared (struct TL_Note *Not)
  {
   /***** Get number of times (users) this note has been shared *****/
   Not->NumShared =
   (unsigned) DB_QueryCOUNT ("can not get number of times"
			     " a note has been shared",
			     "SELECT COUNT(*) FROM tl_pubs"
			     " WHERE NotCod=%ld"
			     " AND PublisherCod<>%ld"
			     " AND PubType=%u",
			     Not->NotCod,
			     Not->UsrCod,	// The author
			     (unsigned) TL_PUB_SHARED_NOTE);
  }

/*****************************************************************************/
/******************* Show users who have shared this note ********************/
/*****************************************************************************/

static void TL_Sha_ShowUsrsWhoHaveSharedNote (const struct TL_Note *Not,
					      TL_HowManyUsrs_t HowManyUsrs)
  {
   MYSQL_RES *mysql_res;
   unsigned NumFirstUsrs;

   /***** Get users who have shared this note *****/
   if (Not->NumShared)
      NumFirstUsrs =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get users",
				 "SELECT PublisherCod FROM tl_pubs"
				 " WHERE NotCod=%ld"
				 " AND PublisherCod<>%ld"
				 " AND PubType=%u"
				 " ORDER BY PubCod LIMIT %u",
				 Not->NotCod,
				 Not->UsrCod,
				 (unsigned) TL_PUB_SHARED_NOTE,
				 HowManyUsrs == TL_SHOW_FEW_USRS ? TL_DEF_USRS_SHOWN :
				                                   TL_MAX_USRS_SHOWN);
   else
      NumFirstUsrs = 0;

   /***** Show users *****/
   /* Number of users */
   HTM_DIV_Begin ("class=\"TL_NUM_USRS\"");
   TL_ShowNumSharersOrFavers (Not->NumShared);
   HTM_DIV_End ();

   /* List users one by one */
   HTM_DIV_Begin ("class=\"TL_USRS\"");
   TL_ShowSharersOrFavers (&mysql_res,Not->NumShared,NumFirstUsrs);
   if (NumFirstUsrs < Not->NumShared)
      /* Clickable ellipsis to show all users */
      TL_PutFormToSeeAllFaversSharers (ActAllShaTL_NotGbl,ActAllShaTL_NotUsr,
		                       "NotCod=%ld",Not->NotCod,
                                       HowManyUsrs);
   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   if (Not->NumShared)
      DB_FreeMySQLResult (&mysql_res);
  }
