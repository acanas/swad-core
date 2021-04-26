// swad_timeline_user.c: social timeline users

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

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <stdio.h>		// For asprintf

#include "swad_error.h"
#include "swad_global.h"
#include "swad_photo.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_favourite.h"
#include "swad_timeline_form.h"
#include "swad_timeline_share.h"
#include "swad_timeline_user.h"

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

static void Tml_Usr_GetAndShowSharersOrFavers (Tml_Usr_FavSha_t FavSha,
                                               long Cod,long UsrCod,unsigned NumUsrs,
				               Tml_Usr_HowManyUsrs_t HowManyUsrs);
static void Tml_Usr_ShowNumSharersOrFavers (unsigned NumUsrs);
static void Tml_Usr_ListSharersOrFavers (MYSQL_RES **mysql_res,
			                 unsigned NumUsrs,unsigned NumFirstUsrs);
static unsigned Tml_Usr_GetListFaversOrSharers (Tml_Usr_FavSha_t FavSha,
                                                long Cod,long UsrCod,unsigned MaxUsrs,
                                                MYSQL_RES **mysql_res);

static void Tml_Usr_PutDisabledIconFavSha (Tml_Usr_FavSha_t FavSha,
                                           unsigned NumUsrs);

/*****************************************************************************/
/************* Remove all the content of a user from database ****************/
/*****************************************************************************/

void Tml_Usr_RemoveUsrContent (long UsrCod)
  {
   /***** Remove favs for comments *****/
   /* Remove all favs made by this user to any comment */
   Tml_DB_RemoveAllFavsMadeByUsr (Tml_Usr_FAV_UNF_COMM,UsrCod);

   /* Remove all favs to comments of this user */
   Tml_DB_RemoveAllFavsToPubsBy (Tml_Usr_FAV_UNF_COMM,UsrCod);

   /* Remove all favs to all comments in all notes authored by this user */
   Tml_DB_RemoveAllFavsToAllCommsInAllNotesBy (UsrCod);

   /***** Remove favs for notes *****/
   /* Remove all favs made by this user to any note */
   Tml_DB_RemoveAllFavsMadeByUsr (Tml_Usr_FAV_UNF_NOTE,UsrCod);

   /* Remove all favs to notes of this user */
   Tml_DB_RemoveAllFavsToPubsBy (Tml_Usr_FAV_UNF_NOTE,UsrCod);

   /***** Remove comments *****/
   /* Remove all comments in all the notes of this user */
   Tml_DB_RemoveAllCommsInAllNotesOf (UsrCod);

   /* Remove all comments made by this user in any note */
   Tml_DB_RemoveAllCommsMadeBy (UsrCod);

   /***** Remove posts *****/
   /* Remove all posts of the user */
   Tml_DB_RemoveAllPostsUsr (UsrCod);

   /***** Remove publications *****/
   /* Remove all publications (original, shared notes, comments)
      published by any user
      and related to notes authored by the user */
   Tml_DB_RemoveAllPubsPublishedByAnyUsrOfNotesAuthoredBy (UsrCod);

   /* Remove all publications published by the user */
   Tml_DB_RemoveAllPubsPublishedBy (UsrCod);

   /***** Remove notes *****/
   /* Remove all notes of the user */
   Tml_DB_RemoveAllNotesUsr (UsrCod);
  }

/*****************************************************************************/
/************** Show users who have faved/shared a note/comment **************/
/*****************************************************************************/

static void Tml_Usr_GetAndShowSharersOrFavers (Tml_Usr_FavSha_t FavSha,
                                               long Cod,long UsrCod,unsigned NumUsrs,
				               Tml_Usr_HowManyUsrs_t HowManyUsrs)
  {
   static const Tml_Frm_Action_t Action[Tml_Usr_NUM_FAV_SHA] =
     {
      [Tml_Usr_FAV_UNF_NOTE] = Tml_Frm_ALL_FAV_NOTE,
      [Tml_Usr_FAV_UNF_COMM] = Tml_Frm_ALL_FAV_COMM,
      [Tml_Usr_SHA_UNS_NOTE] = Tml_Frm_ALL_SHA_NOTE,
     };
   static const char *ParamFormat[Tml_Usr_NUM_FAV_SHA] =
     {
      [Tml_Usr_FAV_UNF_NOTE] = "NotCod=%ld",
      [Tml_Usr_FAV_UNF_COMM] = "PubCod=%ld",
      [Tml_Usr_SHA_UNS_NOTE] = "NotCod=%ld",
     };
   MYSQL_RES *mysql_res;
   unsigned NumFirstUsrs;

   /***** Get users who have faved/shared *****/
   if (NumUsrs)
      NumFirstUsrs =
      Tml_Usr_GetListFaversOrSharers (FavSha,Cod,UsrCod,
                                      HowManyUsrs == Tml_Usr_SHOW_FEW_USRS ? Tml_Usr_DEF_USRS_SHOWN :
				                                             Tml_Usr_MAX_USRS_SHOWN,
                                      &mysql_res);
   else
      NumFirstUsrs = 0;

   /***** Show users *****/
   /* Number of users */
   HTM_DIV_Begin ("class=\"Tml_NUM_USRS\"");
      Tml_Usr_ShowNumSharersOrFavers (NumUsrs);
   HTM_DIV_End ();

   /* List users one by one */
   HTM_DIV_Begin ("class=\"TL_USRS\"");
      Tml_Usr_ListSharersOrFavers (&mysql_res,NumUsrs,NumFirstUsrs);
      if (NumFirstUsrs < NumUsrs)		// Not all are shown
	 /* Clickable ellipsis to show all users */
	 Tml_Frm_PutFormToSeeAllFaversSharers (Action[FavSha],
					       ParamFormat[FavSha],Cod,
					       HowManyUsrs);
   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   if (NumUsrs)
      DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Show sharers or favouriters ************************/
/*****************************************************************************/

static void Tml_Usr_ShowNumSharersOrFavers (unsigned NumUsrs)
  {
   /***** Show number of sharers or favers
          (users who have shared or marked this note as favourite) *****/
   HTM_TxtF ("&nbsp;%u",NumUsrs);
  }

static void Tml_Usr_ListSharersOrFavers (MYSQL_RES **mysql_res,
			                 unsigned NumUsrs,unsigned NumFirstUsrs)
  {
   unsigned NumUsr;
   struct UsrData UsrDat;

   /***** Trivial check 1: are there favers or sharers? *****/
   if (!NumUsrs)
      return;

   /***** Trivial check 1: number of favers or sharers to show *****/
   if (!NumFirstUsrs)
      return;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** List users *****/
   for (NumUsr = 0;
	NumUsr < NumFirstUsrs;
	NumUsr++)
     {
      /***** Get user's code *****/
      UsrDat.UsrCod = DB_GetNextCode (*mysql_res);

      /***** Get user's data and show user's photo *****/
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
                                                   Usr_DONT_GET_PREFS,
                                                   Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
	{
	 /* Begin container */
	 HTM_DIV_Begin ("class=\"TL_SHARER\"");

	    /* User's photo */
	    Pho_ShowUsrPhotoIfAllowed (&UsrDat,"PHOTO12x16",Pho_ZOOM,true);	// Use unique id

	 /* End container */
	 HTM_DIV_End ();
	}
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************* Check if a user has faved/shared a note/comment ***************/
/*****************************************************************************/

bool Tml_Usr_CheckIfFavedSharedByUsr (Tml_Usr_FavSha_t FavSha,long Cod,long UsrCod)
  {
   switch (FavSha)
     {
      case Tml_Usr_FAV_UNF_NOTE:
      case Tml_Usr_FAV_UNF_COMM:
	 return Tml_DB_CheckIfFavedByUsr (FavSha,Cod,UsrCod);
      case Tml_Usr_SHA_UNS_NOTE:
	 return Tml_DB_CheckIfSharedByUsr (Cod,UsrCod);
      default:
         Err_ShowErrorAndExit ("Wrong fav/share action.");
     }

   return false;	// Not reached
  }

/*****************************************************************************/
/******* Get list of users who have marked a note/comment as favourite *******/
/*****************************************************************************/

static unsigned Tml_Usr_GetListFaversOrSharers (Tml_Usr_FavSha_t FavSha,
                                                long Cod,long UsrCod,unsigned MaxUsrs,
                                                MYSQL_RES **mysql_res)
  {
   switch (FavSha)
     {
      case Tml_Usr_FAV_UNF_NOTE:
      case Tml_Usr_FAV_UNF_COMM:
	 return Tml_DB_GetFavers (FavSha,Cod,UsrCod,MaxUsrs,mysql_res);
      case Tml_Usr_SHA_UNS_NOTE:
	 return Tml_DB_GetSharers (Cod,UsrCod,MaxUsrs,mysql_res);
      default:
         Err_ShowErrorAndExit ("Wrong fav/share action.");
     }

   return 0;	// Not reached
  }

/*****************************************************************************/
/**************** Put icon to fav/unfav and list of favers *******************/
/*****************************************************************************/

void Tml_Usr_PutIconFavSha (Tml_Usr_FavSha_t FavSha,
                            long Cod,long UsrCod,unsigned NumUsrs,
                            Tml_Usr_HowManyUsrs_t HowManyUsrs)
  {
   /***** Put form to fav/unfav or share/unshare this note/comment *****/
   /* Begin container */
   HTM_DIV_Begin ("class=\"TL_ICO\"");

      /* Icon to fav/unfav or share/unshare this note/comment */
      if (Usr_ItsMe (UsrCod))	// I am the author ==> I can not fav/unfav or share/unshare
         Tml_Usr_PutDisabledIconFavSha (FavSha,NumUsrs);
      else			// I am not the author
	 Tml_Frm_PutFormToFavUnfShaUns (FavSha,Cod);

   /* End container */
   HTM_DIV_End ();

   /***** Show who have faved/shared this note/comment *****/
   Tml_Usr_GetAndShowSharersOrFavers (FavSha,Cod,UsrCod,NumUsrs,HowManyUsrs);
  }

/*****************************************************************************/
/****************** Put disabled icon to mark as favourite *******************/
/*****************************************************************************/

static void Tml_Usr_PutDisabledIconFavSha (Tml_Usr_FavSha_t FavSha,
                                           unsigned NumUsrs)
  {
   extern const char *Txt_TIMELINE_Favourited_by_X_USERS;
   extern const char *Txt_TIMELINE_Not_favourited_by_anyone;
   extern const char *Txt_TIMELINE_Shared_by_X_USERS;
   extern const char *Txt_TIMELINE_Not_shared_by_anyone;
   static const char *Icon[Tml_Usr_NUM_FAV_SHA] =
     {
      [Tml_Usr_FAV_UNF_NOTE] = Tml_Fav_ICON_FAV,
      [Tml_Usr_FAV_UNF_COMM] = Tml_Fav_ICON_FAV,
      [Tml_Usr_SHA_UNS_NOTE] = Tml_Sha_ICON_SHARE,
     };
   const char *TitleWithUsrs[Tml_Usr_NUM_FAV_SHA] =
     {
      [Tml_Usr_FAV_UNF_NOTE] = Txt_TIMELINE_Favourited_by_X_USERS,
      [Tml_Usr_FAV_UNF_COMM] = Txt_TIMELINE_Favourited_by_X_USERS,
      [Tml_Usr_SHA_UNS_NOTE] = Txt_TIMELINE_Shared_by_X_USERS,
     };
   const char *TitleWithoutUsrs[Tml_Usr_NUM_FAV_SHA] =
     {
      [Tml_Usr_FAV_UNF_NOTE] = Txt_TIMELINE_Not_favourited_by_anyone,
      [Tml_Usr_FAV_UNF_COMM] = Txt_TIMELINE_Not_favourited_by_anyone,
      [Tml_Usr_SHA_UNS_NOTE] = Txt_TIMELINE_Not_shared_by_anyone,
     };

   /***** Disabled icon to fav/share *****/
   if (NumUsrs)
     {
      Ico_PutDivIcon ("TL_ICO_DISABLED",Icon[FavSha],
		      Str_BuildStringLong (TitleWithUsrs[FavSha],(long) NumUsrs));
      Str_FreeString ();
     }
   else
      Ico_PutDivIcon ("TL_ICO_DISABLED",Icon[FavSha],TitleWithoutUsrs[FavSha]);
  }

/*****************************************************************************/
/***************** Check if I can fav/share a note/comment *******************/
/*****************************************************************************/

bool Tml_Usr_CheckIfICanFavSha (long Cod,long UsrCod)
  {
   extern const char *Txt_The_post_no_longer_exists;

   /***** Trivial check 1: note/comment code should be > 0 *****/
   if (Cod <= 0)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_The_post_no_longer_exists);
      return false;
     }

   /***** Trivial check 2: I must be logged
			   I can not fav/share my own notes/comments *****/
   if (!Gbl.Usrs.Me.Logged || Usr_ItsMe (UsrCod))
     {
      Err_NoPermissionExit ();
      return false;	// Not reached
     }

   return true;
  }

/*****************************************************************************/
/***************** Check if I can fav/share a note/comment *******************/
/*****************************************************************************/

bool Tml_Usr_CheckIfICanRemove (long Cod,long UsrCod)
  {
   extern const char *Txt_The_post_no_longer_exists;

   /***** Trivial check 1: note/comment code should be > 0 *****/
   if (Cod <= 0)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_The_post_no_longer_exists);
      return false;
     }

   /***** Trivial check 2: I must be logged
			   I can only remove my own notes/comments *****/
   if (!Gbl.Usrs.Me.Logged || !Usr_ItsMe (UsrCod))
     {
      Err_NoPermissionExit ();
      return false;	// Not reached
     }

   return true;
  }
