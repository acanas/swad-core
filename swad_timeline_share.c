// swad_timeline_share.c: social timeline shared

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

static void TL_Sha_PutFormToShaNote (const struct TL_Note *SocNot);
static void TL_Sha_PutFormToUnsNote (const struct TL_Note *SocNot);

static void TL_Sha_ShaNote (struct TL_Note *SocNot);
static void TL_Sha_UnsNote (struct TL_Note *SocNot);

static bool TL_Sha_CheckIfNoteIsSharedByUsr (long NotCod,long UsrCod);

static void TL_Sha_ShowUsrsWhoHaveSharedNote (const struct TL_Note *SocNot,
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

void TL_Sha_PutFormToSeeAllSharersNote (const struct TL_Note *SocNot,
                                        TL_HowManyUsrs_t HowManyUsrs)
  {
   extern const char *Txt_View_all_USERS;
   char ParamCod[7 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   switch (HowManyUsrs)
     {
      case TL_SHOW_FEW_USRS:
	 /***** Form and icon to mark note as favourite *****/
	 sprintf (ParamCod,"NotCod=%ld",SocNot->NotCod);
	 TL_FormFavSha (ActAllShaSocNotGbl,ActAllShaSocNotUsr,ParamCod,
			TL_ICON_ELLIPSIS,Txt_View_all_USERS);
	 break;
      case TL_SHOW_ALL_USRS:
	 Ico_PutIconOff (TL_ICON_ELLIPSIS,Txt_View_all_USERS);
	 break;
     }
  }

static void TL_Sha_PutFormToShaNote (const struct TL_Note *SocNot)
  {
   extern const char *Txt_Share;
   char ParamCod[7 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Form and icon to mark note as favourite *****/
   sprintf (ParamCod,"NotCod=%ld",SocNot->NotCod);
   TL_FormFavSha (ActShaSocNotGbl,ActShaSocNotUsr,ParamCod,
	          TL_ICON_SHARE,Txt_Share);
  }

static void TL_Sha_PutFormToUnsNote (const struct TL_Note *SocNot)
  {
   extern const char *Txt_TIMELINE_NOTE_Shared;
   char ParamCod[7 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Form and icon to mark note as favourite *****/
   sprintf (ParamCod,"NotCod=%ld",SocNot->NotCod);
   TL_FormFavSha (ActUnsSocNotGbl,ActUnsSocNotUsr,ParamCod,
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
   struct TL_Note SocNot;

   /***** Get data of note *****/
   SocNot.NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (&SocNot);

   /***** Write HTML inside DIV with form to share/unshare *****/
   TL_Sha_PutFormToShaUnsNote (&SocNot,TL_SHOW_ALL_USRS);
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
   struct TL_Note SocNot;

   /***** Share note *****/
   TL_Sha_ShaNote (&SocNot);

   /***** Write HTML inside DIV with form to unshare *****/
   TL_Sha_PutFormToShaUnsNote (&SocNot,TL_SHOW_FEW_USRS);
  }

static void TL_Sha_ShaNote (struct TL_Note *SocNot)
  {
   // extern const char *Txt_The_original_post_no_longer_exists;
   struct TL_Publication SocPub;
   bool ItsMe;
   long OriginalPubCod;

   /***** Get data of note *****/
   SocNot->NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (SocNot);

   if (SocNot->NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot->UsrCod);
      if (Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
         if (!TL_Sha_CheckIfNoteIsSharedByUsr (SocNot->NotCod,
					    Gbl.Usrs.Me.UsrDat.UsrCod))	// Not yet shared by me
	   {
	    /***** Share (publish note in timeline) *****/
	    SocPub.NotCod       = SocNot->NotCod;
	    SocPub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
	    SocPub.PubType      = TL_PUB_SHARED_NOTE;
	    TL_PublishNoteInTimeline (&SocPub);	// Set SocPub.PubCod

	    /* Update number of times this note is shared */
	    TL_Sha_UpdateNumTimesANoteHasBeenShared (SocNot);

	    /**** Create notification about shared post
		  for the author of the post ***/
	    OriginalPubCod = TL_GetPubCodOfOriginalNote (SocNot->NotCod);
	    if (OriginalPubCod > 0)
	       TL_CreateNotifToAuthor (SocNot->UsrCod,OriginalPubCod,Ntf_EVENT_TIMELINE_SHARE);
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
   struct TL_Note SocNot;

   /***** Unshare note *****/
   TL_Sha_UnsNote (&SocNot);

   /***** Write HTML inside DIV with form to share *****/
   TL_Sha_PutFormToShaUnsNote (&SocNot,TL_SHOW_FEW_USRS);
  }

static void TL_Sha_UnsNote (struct TL_Note *SocNot)
  {
   // extern const char *Txt_The_original_post_no_longer_exists;
   long OriginalPubCod;
   bool ItsMe;

   /***** Get data of note *****/
   SocNot->NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (SocNot);

   if (SocNot->NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot->UsrCod);
      if (SocNot->NumShared &&
	  Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
	 if (TL_Sha_CheckIfNoteIsSharedByUsr (SocNot->NotCod,
					   Gbl.Usrs.Me.UsrDat.UsrCod))	// I am a sharer
	   {
	    /***** Delete publication from database *****/
	    DB_QueryDELETE ("can not remove a publication",
			    "DELETE FROM tl_pubs"
	                    " WHERE NotCod=%ld"
	                    " AND PublisherCod=%ld"
	                    " AND PubType=%u",
			    SocNot->NotCod,
			    Gbl.Usrs.Me.UsrDat.UsrCod,
			    (unsigned) TL_PUB_SHARED_NOTE);

	    /***** Update number of times this note is shared *****/
	    TL_Sha_UpdateNumTimesANoteHasBeenShared (SocNot);

            /***** Mark possible notifications on this note as removed *****/
	    OriginalPubCod = TL_GetPubCodOfOriginalNote (SocNot->NotCod);
	    if (OriginalPubCod > 0)
	       Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_SHARE,OriginalPubCod);
	   }
     }
  }

void TL_Sha_PutFormToShaUnsNote (const struct TL_Note *SocNot,
                                 TL_HowManyUsrs_t HowManyUsrs)
  {
   bool IAmTheAuthor;
   bool IAmASharerOfThisSocNot;

   /***** Put form to share/unshare this note *****/
   HTM_DIV_Begin ("class=\"TL_ICO\"");
   IAmTheAuthor = Usr_ItsMe (SocNot->UsrCod);
   if (SocNot->Unavailable ||		// Unavailable notes can not be shared
       IAmTheAuthor)			// I am the author
      /* Put disabled icon */
      TL_Sha_PutDisabledIconShare (SocNot->NumShared);
   else					// Available and I am not the author
     {
      /* Put icon to share/unshare */
      IAmASharerOfThisSocNot = TL_Sha_CheckIfNoteIsSharedByUsr (SocNot->NotCod,
							    Gbl.Usrs.Me.UsrDat.UsrCod);
      if (IAmASharerOfThisSocNot)	// I have shared this note
	 TL_Sha_PutFormToUnsNote (SocNot);
      else				// I have not shared this note
	 TL_Sha_PutFormToShaNote (SocNot);
     }
   HTM_DIV_End ();

   /***** Show who have shared this note *****/
   TL_Sha_ShowUsrsWhoHaveSharedNote (SocNot,HowManyUsrs);
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

void TL_Sha_UpdateNumTimesANoteHasBeenShared (struct TL_Note *SocNot)
  {
   /***** Get number of times (users) this note has been shared *****/
   SocNot->NumShared =
   (unsigned) DB_QueryCOUNT ("can not get number of times"
			     " a note has been shared",
			     "SELECT COUNT(*) FROM tl_pubs"
			     " WHERE NotCod=%ld"
			     " AND PublisherCod<>%ld"
			     " AND PubType=%u",
			     SocNot->NotCod,
			     SocNot->UsrCod,	// The author
			     (unsigned) TL_PUB_SHARED_NOTE);
  }

/*****************************************************************************/
/******************* Show users who have shared this note ********************/
/*****************************************************************************/

static void TL_Sha_ShowUsrsWhoHaveSharedNote (const struct TL_Note *SocNot,
					      TL_HowManyUsrs_t HowManyUsrs)
  {
   MYSQL_RES *mysql_res;
   unsigned NumFirstUsrs;

   /***** Get users who have shared this note *****/
   if (SocNot->NumShared)
      NumFirstUsrs =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get users",
				 "SELECT PublisherCod FROM tl_pubs"
				 " WHERE NotCod=%ld"
				 " AND PublisherCod<>%ld"
				 " AND PubType=%u"
				 " ORDER BY PubCod LIMIT %u",
				 SocNot->NotCod,
				 SocNot->UsrCod,
				 (unsigned) TL_PUB_SHARED_NOTE,
				 HowManyUsrs == TL_SHOW_FEW_USRS ? TL_DEF_USRS_SHOWN :
				                                   TL_MAX_USRS_SHOWN);
   else
      NumFirstUsrs = 0;

   /***** Show users *****/
   /* Number of users */
   HTM_DIV_Begin ("class=\"TL_NUM_USRS\"");
   TL_ShowNumSharersOrFavers (SocNot->NumShared);
   HTM_DIV_End ();

   /* List users one by one */
   HTM_DIV_Begin ("class=\"TL_USRS\"");
   TL_ShowSharersOrFavers (&mysql_res,SocNot->NumShared,NumFirstUsrs);
   if (NumFirstUsrs < SocNot->NumShared)
      /* Clickable ellipsis to show all users */
      TL_Sha_PutFormToSeeAllSharersNote (SocNot,HowManyUsrs);
   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   if (SocNot->NumShared)
      DB_FreeMySQLResult (&mysql_res);
  }

