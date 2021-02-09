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
#include "swad_timeline_favourite.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

#define TL_ICON_FAV		"heart.svg"
#define TL_ICON_FAVED		"heart-red.svg"

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

static void TL_Fav_PutDisabledIconFav (unsigned NumFavs);

static void TL_Fav_PutFormToFavNote (long ParamCod);
static void TL_Fav_PutFormToUnfNote (long ParamCod);

static void TL_Fav_PutFormToFavComment (long ParamCod);
static void TL_Fav_PutFormToUnfComment (long ParamCod);

static void TL_Fav_FavNote (struct TL_Note *SocNot);
static void TL_Fav_UnfNote (struct TL_Note *SocNot);

static void TL_Fav_FavComment (struct TL_Comment *SocCom);
static void TL_Fav_UnfComment (struct TL_Comment *SocCom);

static bool TL_Fav_CheckIfNoteIsFavedByUsr (long NotCod,long UsrCod);
static bool TL_Fav_CheckIfCommIsFavedByUsr (long PubCod,long UsrCod);

static void TL_Fav_ShowUsrsWhoHaveMarkedNoteAsFav (const struct TL_Note *SocNot,
					           TL_HowManyUsrs_t HowManyUsrs);
static void TL_Fav_ShowUsrsWhoHaveMarkedCommAsFav (const struct TL_Comment *SocCom,
					           TL_HowManyUsrs_t HowManyUsrs);

/*****************************************************************************/
/****************** Put disabled icon to mark as favourite *******************/
/*****************************************************************************/

static void TL_Fav_PutDisabledIconFav (unsigned NumFavs)
  {
   extern const char *Txt_TIMELINE_NOTE_Favourited_by_X_USERS;
   extern const char *Txt_TIMELINE_NOTE_Not_favourited_by_anyone;

   /***** Disabled icon to mark as favourite *****/
   if (NumFavs)
     {
      Ico_PutDivIcon ("TL_ICO_DISABLED",TL_ICON_FAV,
		      Str_BuildStringLong (Txt_TIMELINE_NOTE_Favourited_by_X_USERS,
					   (long) NumFavs));
      Str_FreeString ();
     }
   else
      Ico_PutDivIcon ("TL_ICO_DISABLED",TL_ICON_FAV,
		      Txt_TIMELINE_NOTE_Not_favourited_by_anyone);
  }

/*****************************************************************************/
/************************** Form to fav/unfav note ***************************/
/*****************************************************************************/

static void TL_Fav_PutFormToFavNote (long ParamCod)
  {
   extern const char *Txt_Mark_as_favourite;

   /***** Form and icon to mark note as favourite *****/
   TL_FormFavSha (ActFavSocNotGbl,ActFavSocNotUsr,
                  "NotCod=%ld",ParamCod,
	          TL_ICON_FAV,Txt_Mark_as_favourite);
  }

static void TL_Fav_PutFormToUnfNote (long ParamCod)
  {
   extern const char *Txt_TIMELINE_NOTE_Favourite;

   /***** Form and icon to unfav (remove mark as favourite) note *****/
   TL_FormFavSha (ActUnfSocNotGbl,ActUnfSocNotUsr,
                  "NotCod=%ld",ParamCod,
	          TL_ICON_FAVED,Txt_TIMELINE_NOTE_Favourite);
  }

/*****************************************************************************/
/************************** Form to fav/unfav comment ************************/
/*****************************************************************************/

static void TL_Fav_PutFormToFavComment (long ParamCod)
  {
   extern const char *Txt_Mark_as_favourite;

   /***** Form and icon to mark comment as favourite *****/
   TL_FormFavSha (ActFavSocComGbl,ActFavSocComUsr,
                  "PubCod=%ld",ParamCod,
	          TL_ICON_FAV,Txt_Mark_as_favourite);
  }

static void TL_Fav_PutFormToUnfComment (long ParamCod)
  {
   extern const char *Txt_TIMELINE_NOTE_Favourite;

   /***** Form and icon to unfav (remove mark as favourite) comment *****/
   TL_FormFavSha (ActUnfSocComGbl,ActUnfSocComUsr,
                  "PubCod=%ld",ParamCod,
	          TL_ICON_FAVED,Txt_TIMELINE_NOTE_Favourite);
  }

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
   struct TL_Note SocNot;

   /***** Get data of note *****/
   SocNot.NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (&SocNot);

   /***** Write HTML inside DIV with form to fav/unfav *****/
   TL_Fav_PutFormToFavUnfNote (&SocNot,TL_SHOW_ALL_USRS);
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
   struct TL_Note SocNot;

   /***** Mark note as favourite *****/
   TL_Fav_FavNote (&SocNot);

   /***** Write HTML inside DIV with form to unfav *****/
   TL_Fav_PutFormToFavUnfNote (&SocNot,TL_SHOW_FEW_USRS);
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
   struct TL_Note SocNot;

   /***** Stop marking as favourite a previously favourited note *****/
   TL_Fav_UnfNote (&SocNot);

   /***** Write HTML inside DIV with form to fav *****/
   TL_Fav_PutFormToFavUnfNote (&SocNot,TL_SHOW_FEW_USRS);
  }

void TL_Fav_PutFormToFavUnfNote (const struct TL_Note *SocNot,
                                 TL_HowManyUsrs_t HowManyUsrs)
  {
   bool IAmTheAuthor;
   bool IAmAFaverOfThisSocNot;

   /***** Put form to fav/unfav this note *****/
   HTM_DIV_Begin ("class=\"TL_ICO\"");
   IAmTheAuthor = Usr_ItsMe (SocNot->UsrCod);
   if (SocNot->Unavailable ||		// Unavailable notes can not be favourited
       IAmTheAuthor)			// I am the author
      /* Put disabled icon */
      TL_Fav_PutDisabledIconFav (SocNot->NumFavs);
   else					// Available and I am not the author
     {
      /* Put icon to fav/unfav */
      IAmAFaverOfThisSocNot = TL_Fav_CheckIfNoteIsFavedByUsr (SocNot->NotCod,
							      Gbl.Usrs.Me.UsrDat.UsrCod);
      if (IAmAFaverOfThisSocNot)	// I have favourited this note
	 TL_Fav_PutFormToUnfNote (SocNot->NotCod);
      else				// I am not a faver of this note
	 TL_Fav_PutFormToFavNote (SocNot->NotCod);
     }
   HTM_DIV_End ();

   /***** Show who have marked this note as favourite *****/
   TL_Fav_ShowUsrsWhoHaveMarkedNoteAsFav (SocNot,HowManyUsrs);
  }

static void TL_Fav_FavNote (struct TL_Note *SocNot)
  {
   bool ItsMe;
   long OriginalPubCod;

   /***** Get data of note *****/
   SocNot->NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (SocNot);

   if (SocNot->NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot->UsrCod);
      if (Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
	 if (!TL_Fav_CheckIfNoteIsFavedByUsr (SocNot->NotCod,
					  Gbl.Usrs.Me.UsrDat.UsrCod))	// I have not yet favourited the note
	   {
	    /***** Mark as favourite in database *****/
	    DB_QueryINSERT ("can not favourite note",
			    "INSERT IGNORE INTO tl_notes_fav"
			    " (NotCod,UsrCod,TimeFav)"
			    " VALUES"
			    " (%ld,%ld,NOW())",
			    SocNot->NotCod,
			    Gbl.Usrs.Me.UsrDat.UsrCod);

	    /***** Update number of times this note is favourited *****/
	    TL_Fav_GetNumTimesANoteHasBeenFav (SocNot);

	    /***** Create notification about favourite post
		   for the author of the post *****/
	    OriginalPubCod = TL_GetPubCodOfOriginalNote (SocNot->NotCod);
	    if (OriginalPubCod > 0)
	       TL_CreateNotifToAuthor (SocNot->UsrCod,OriginalPubCod,Ntf_EVENT_TIMELINE_FAV);
	   }
     }
  }

static void TL_Fav_UnfNote (struct TL_Note *SocNot)
  {
   long OriginalPubCod;
   bool ItsMe;

   /***** Get data of note *****/
   SocNot->NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (SocNot);

   if (SocNot->NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot->UsrCod);
      if (SocNot->NumFavs &&
	  Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
	 if (TL_Fav_CheckIfNoteIsFavedByUsr (SocNot->NotCod,
					  Gbl.Usrs.Me.UsrDat.UsrCod))	// I have favourited the note
	   {
	    /***** Delete the mark as favourite from database *****/
	    DB_QueryDELETE ("can not unfavourite note",
			    "DELETE FROM tl_notes_fav"
			    " WHERE NotCod=%ld AND UsrCod=%ld",
			    SocNot->NotCod,
			    Gbl.Usrs.Me.UsrDat.UsrCod);

	    /***** Update number of times this note is favourited *****/
	    TL_Fav_GetNumTimesANoteHasBeenFav (SocNot);

            /***** Mark possible notifications on this note as removed *****/
	    OriginalPubCod = TL_GetPubCodOfOriginalNote (SocNot->NotCod);
	    if (OriginalPubCod > 0)
	       Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV,OriginalPubCod);
	   }
     }
  }

/*****************************************************************************/
/********************* Mark/unmark a comment as favourite ************************/
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
   struct TL_Comment SocCom;

   /***** Get data of comment *****/
   Med_MediaConstructor (&SocCom.Content.Media);
   SocCom.PubCod = TL_GetParamPubCod ();
   TL_GetDataOfCommByCod (&SocCom);
   Med_MediaDestructor (&SocCom.Content.Media);

   /***** Write HTML inside DIV with form to fav/unfav *****/
   TL_Fav_PutFormToFavUnfComment (&SocCom,TL_SHOW_ALL_USRS);
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
   struct TL_Comment SocCom;

   /***** Mark comment as favourite *****/
   TL_Fav_FavComment (&SocCom);

   /***** Write HTML inside DIV with form to unfav *****/
   TL_Fav_PutFormToFavUnfComment (&SocCom,TL_SHOW_FEW_USRS);
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
   struct TL_Comment SocCom;

   /***** Stop marking as favourite a previously favourited comment *****/
   TL_Fav_UnfComment (&SocCom);

   /***** Write HTML inside DIV with form to fav *****/
   TL_Fav_PutFormToFavUnfComment (&SocCom,TL_SHOW_FEW_USRS);
  }

void TL_Fav_PutFormToFavUnfComment (const struct TL_Comment *SocCom,
                                    TL_HowManyUsrs_t HowManyUsrs)
  {
   bool IAmTheAuthor;
   bool IAmAFaverOfThisSocCom;

   /***** Put form to fav/unfav this comment *****/
   HTM_DIV_Begin ("class=\"TL_ICO\"");
   IAmTheAuthor = Usr_ItsMe (SocCom->UsrCod);
   if (IAmTheAuthor)			// I am the author
      /* Put disabled icon */
      TL_Fav_PutDisabledIconFav (SocCom->NumFavs);
   else				// I am not the author
     {
      /* Put icon to mark this comment as favourite */
      IAmAFaverOfThisSocCom = TL_Fav_CheckIfCommIsFavedByUsr (SocCom->PubCod,
							      Gbl.Usrs.Me.UsrDat.UsrCod);
      if (IAmAFaverOfThisSocCom)	// I have favourited this comment
	 /* Put icon to unfav this publication and list of users */
	 TL_Fav_PutFormToUnfComment (SocCom->PubCod);
      else				// I am not a favouriter
	 /* Put icon to fav this publication and list of users */
	 TL_Fav_PutFormToFavComment (SocCom->PubCod);
     }
   HTM_DIV_End ();

   /***** Show who have marked this comment as favourite *****/
   TL_Fav_ShowUsrsWhoHaveMarkedCommAsFav (SocCom,HowManyUsrs);
  }

static void TL_Fav_FavComment (struct TL_Comment *SocCom)
  {
   bool IAmTheAuthor;

   /***** Initialize image *****/
   Med_MediaConstructor (&SocCom->Content.Media);

   /***** Get data of comment *****/
   SocCom->PubCod = TL_GetParamPubCod ();
   TL_GetDataOfCommByCod (SocCom);

   if (SocCom->PubCod > 0)
     {
      IAmTheAuthor = Usr_ItsMe (SocCom->UsrCod);
      if (!IAmTheAuthor)	// I am not the author
	 if (!TL_Fav_CheckIfCommIsFavedByUsr (SocCom->PubCod,
					      Gbl.Usrs.Me.UsrDat.UsrCod)) // I have not yet favourited the comment
	   {
	    /***** Mark as favourite in database *****/
	    DB_QueryINSERT ("can not favourite comment",
			    "INSERT IGNORE INTO tl_comments_fav"
			    " (PubCod,UsrCod,TimeFav)"
			    " VALUES"
			    " (%ld,%ld,NOW())",
			    SocCom->PubCod,
			    Gbl.Usrs.Me.UsrDat.UsrCod);

	    /* Update number of times this comment is favourited */
	    TL_Fav_GetNumTimesACommHasBeenFav (SocCom);

	    /**** Create notification about favourite post
		  for the author of the post ***/
	    TL_CreateNotifToAuthor (SocCom->UsrCod,SocCom->PubCod,Ntf_EVENT_TIMELINE_FAV);
	   }
     }

   /***** Free image *****/
   Med_MediaDestructor (&SocCom->Content.Media);
  }

static void TL_Fav_UnfComment (struct TL_Comment *SocCom)
  {
   bool IAmTheAuthor;

   /***** Initialize image *****/
   Med_MediaConstructor (&SocCom->Content.Media);

   /***** Get data of comment *****/
   SocCom->PubCod = TL_GetParamPubCod ();
   TL_GetDataOfCommByCod (SocCom);

   if (SocCom->PubCod > 0)
     {
      IAmTheAuthor = Usr_ItsMe (SocCom->UsrCod);
      if (SocCom->NumFavs &&
	  !IAmTheAuthor)	// I am not the author
	 if (TL_Fav_CheckIfCommIsFavedByUsr (SocCom->PubCod,
					     Gbl.Usrs.Me.UsrDat.UsrCod))	// I have favourited the comment
	   {
	    /***** Delete the mark as favourite from database *****/
	    DB_QueryDELETE ("can not unfavourite comment",
			    "DELETE FROM tl_comments_fav"
			    " WHERE PubCod=%ld AND UsrCod=%ld",
			    SocCom->PubCod,
			    Gbl.Usrs.Me.UsrDat.UsrCod);

	    /***** Update number of times this comment is favourited *****/
	    TL_Fav_GetNumTimesACommHasBeenFav (SocCom);

            /***** Mark possible notifications on this comment as removed *****/
            Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV,SocCom->PubCod);
	   }
     }

   /***** Free image *****/
   Med_MediaDestructor (&SocCom->Content.Media);
  }

/*****************************************************************************/
/****************** Check if a user has favourited a note ********************/
/*****************************************************************************/

static bool TL_Fav_CheckIfNoteIsFavedByUsr (long NotCod,long UsrCod)
  {
   return (DB_QueryCOUNT ("can not check if a user"
			  " has favourited a note",
			  "SELECT COUNT(*) FROM tl_notes_fav"
			  " WHERE NotCod=%ld AND UsrCod=%ld",
			  NotCod,UsrCod) != 0);
  }

/*****************************************************************************/
/**************** Check if a user has favourited a comment *******************/
/*****************************************************************************/

static bool TL_Fav_CheckIfCommIsFavedByUsr (long PubCod,long UsrCod)
  {
   return (DB_QueryCOUNT ("can not check if a user"
			  " has favourited a comment",
			  "SELECT COUNT(*) FROM tl_comments_fav"
			  " WHERE PubCod=%ld AND UsrCod=%ld",
			  PubCod,UsrCod) != 0);
  }

/*****************************************************************************/
/*************** Get number of times a note has been favourited **************/
/*****************************************************************************/

void TL_Fav_GetNumTimesANoteHasBeenFav (struct TL_Note *SocNot)
  {
   /***** Get number of times (users) this note has been favourited *****/
   SocNot->NumFavs =
   (unsigned) DB_QueryCOUNT ("can not get number of times"
			     " a note has been favourited",
			     "SELECT COUNT(*) FROM tl_notes_fav"
			     " WHERE NotCod=%ld"
			     " AND UsrCod<>%ld",	// Extra check
			     SocNot->NotCod,
			     SocNot->UsrCod);		// The author
  }

/*****************************************************************************/
/************ Get number of times a comment has been favourited **************/
/*****************************************************************************/

void TL_Fav_GetNumTimesACommHasBeenFav (struct TL_Comment *SocCom)
  {
   /***** Get number of times (users) this comment has been favourited *****/
   SocCom->NumFavs =
   (unsigned) DB_QueryCOUNT ("can not get number of times"
			     " a comment has been favourited",
			     "SELECT COUNT(*) FROM tl_comments_fav"
			     " WHERE PubCod=%ld"
			     " AND UsrCod<>%ld",	// Extra check
			     SocCom->PubCod,
			     SocCom->UsrCod);		// The author
  }

/*****************************************************************************/
/************ Show users who have marked this note as favourite **************/
/*****************************************************************************/

static void TL_Fav_ShowUsrsWhoHaveMarkedNoteAsFav (const struct TL_Note *SocNot,
					           TL_HowManyUsrs_t HowManyUsrs)
  {
   MYSQL_RES *mysql_res;
   unsigned NumFirstUsrs;

   /***** Get users who have marked this note as favourite *****/
   if (SocNot->NumFavs)
      /***** Get list of users from database *****/
      NumFirstUsrs =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get users",
				 "SELECT UsrCod FROM tl_notes_fav"
				 " WHERE NotCod=%ld"
				 " AND UsrCod<>%ld"	// Extra check
				 " ORDER BY FavCod LIMIT %u",
				 SocNot->NotCod,
				 SocNot->UsrCod,
				 HowManyUsrs == TL_SHOW_FEW_USRS ? TL_DEF_USRS_SHOWN :
				                                   TL_MAX_USRS_SHOWN);
   else
      NumFirstUsrs = 0;

   /***** Show users *****/
   /* Number of users */
   HTM_DIV_Begin ("class=\"TL_NUM_USRS\"");
   TL_ShowNumSharersOrFavers (SocNot->NumFavs);
   HTM_DIV_End ();

   /* List users one by one */
   HTM_DIV_Begin ("class=\"TL_USRS\"");
   TL_ShowSharersOrFavers (&mysql_res,SocNot->NumFavs,NumFirstUsrs);
   if (NumFirstUsrs < SocNot->NumFavs)		// Not all are shown
      /* Clickable ellipsis to show all users */
      TL_PutFormToSeeAllFaversSharers (ActAllFavSocNotGbl,ActAllFavSocNotUsr,
                                       "NotCod=%ld",SocNot->NotCod,
                                       HowManyUsrs);
   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   if (SocNot->NumFavs)
      DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Show users who have marked this note as favourite **************/
/*****************************************************************************/

static void TL_Fav_ShowUsrsWhoHaveMarkedCommAsFav (const struct TL_Comment *SocCom,
					           TL_HowManyUsrs_t HowManyUsrs)
  {
   MYSQL_RES *mysql_res;
   unsigned NumFirstUsrs;

   /***** Get users who have marked this comment as favourite *****/
   if (SocCom->NumFavs)
      /***** Get list of users from database *****/
      NumFirstUsrs =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get users",
				 "SELECT UsrCod FROM tl_comments_fav"
				 " WHERE PubCod=%ld"
				 " AND UsrCod<>%ld"	// Extra check
				 " ORDER BY FavCod LIMIT %u",
				 SocCom->PubCod,
				 SocCom->UsrCod,
				 HowManyUsrs == TL_SHOW_FEW_USRS ? TL_DEF_USRS_SHOWN :
				                                   TL_MAX_USRS_SHOWN);
   else
      NumFirstUsrs = 0;

   /***** Show users *****/
   /* Number of users */
   HTM_DIV_Begin ("class=\"TL_NUM_USRS\"");
   TL_ShowNumSharersOrFavers (SocCom->NumFavs);
   HTM_DIV_End ();

   /* List users one by one */
   HTM_DIV_Begin ("class=\"TL_USRS\"");
   TL_ShowSharersOrFavers (&mysql_res,SocCom->NumFavs,NumFirstUsrs);
   if (NumFirstUsrs < SocCom->NumFavs)
      /* Clickable ellipsis to show all users */
      TL_PutFormToSeeAllFaversSharers (ActAllFavSocComGbl,ActAllFavSocComUsr,
                                       "PubCod=%ld",SocCom->PubCod,
                                       HowManyUsrs);
   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   if (SocCom->NumFavs)
      DB_FreeMySQLResult (&mysql_res);
  }
