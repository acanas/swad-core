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

/*****************************************************************************/
/************* Remove all the content of a user from database ****************/
/*****************************************************************************/

void TL_Usr_RemoveUsrContent (long UsrCod)
  {
   /***** Remove favs for comments *****/
   /* Remove all favs made by this user to any comment */
   TL_DB_RemoveAllFavsMadeByUsr (TL_Usr_FAV_UNF_COMM,UsrCod);

   /* Remove all favs to comments of this user */
   TL_DB_RemoveAllFavsToPubsBy (TL_Usr_FAV_UNF_COMM,UsrCod);

   /* Remove all favs to all comments in all notes authored by this user */
   TL_DB_RemoveAllFavsToAllCommsInAllNotesBy (UsrCod);

   /***** Remove favs for notes *****/
   /* Remove all favs made by this user to any note */
   TL_DB_RemoveAllFavsMadeByUsr (TL_Usr_FAV_UNF_NOTE,UsrCod);

   /* Remove all favs to notes of this user */
   TL_DB_RemoveAllFavsToPubsBy (TL_Usr_FAV_UNF_NOTE,UsrCod);

   /***** Remove comments *****/
   /* Remove all comments in all the notes of this user */
   TL_DB_RemoveAllCommsInAllNotesOf (UsrCod);

   /* Remove all comments made by this user in any note */
   TL_DB_RemoveAllCommsMadeBy (UsrCod);

   /***** Remove posts *****/
   /* Remove all posts of the user */
   TL_DB_RemoveAllPostsUsr (UsrCod);

   /***** Remove publications *****/
   /* Remove all publications (original, shared notes, comments)
      published by any user
      and related to notes authored by the user */
   TL_DB_RemoveAllPubsPublishedByAnyUsrOfNotesAuthoredBy (UsrCod);

   /* Remove all publications published by the user */
   TL_DB_RemoveAllPubsPublishedBy (UsrCod);

   /***** Remove notes *****/
   /* Remove all notes of the user */
   TL_DB_RemoveAllNotesUsr (UsrCod);
  }

/*****************************************************************************/
/************** Show users who have faved/shared a note/comment **************/
/*****************************************************************************/

void TL_Usr_GetAndShowSharersOrFavers (TL_Usr_FavSha_t FavSha,
                                       long Cod,long UsrCod,unsigned NumUsrs,
				       TL_Usr_HowManyUsrs_t HowManyUsrs)
  {
   static const TL_Frm_Action_t Action[TL_Usr_NUM_FAV_SHA] =
     {
      [TL_Usr_FAV_UNF_NOTE] = TL_Frm_ALL_FAV_NOTE,
      [TL_Usr_FAV_UNF_COMM] = TL_Frm_ALL_FAV_COMM,
      [TL_Usr_SHA_UNS_NOTE] = TL_Frm_ALL_SHA_NOTE,
     };
   static const char *ParamFormat[TL_Usr_NUM_FAV_SHA] =
     {
      [TL_Usr_FAV_UNF_NOTE] = "NotCod=%ld",
      [TL_Usr_FAV_UNF_COMM] = "PubCod=%ld",
      [TL_Usr_SHA_UNS_NOTE] = "NotCod=%ld",
     };
   MYSQL_RES *mysql_res;
   unsigned NumFirstUsrs;

   /***** Get users who have faved/shared *****/
   if (NumUsrs)
      NumFirstUsrs =
      TL_Usr_GetListFaversOrSharers (FavSha,Cod,UsrCod,
                                     HowManyUsrs == TL_Usr_SHOW_FEW_USRS ? TL_Usr_DEF_USRS_SHOWN :
				                                           TL_Usr_MAX_USRS_SHOWN,
                                     &mysql_res);
   else
      NumFirstUsrs = 0;

   /***** Show users *****/
   /* Number of users */
   HTM_DIV_Begin ("class=\"TL_NUM_USRS\"");
      TL_Usr_ShowNumSharersOrFavers (NumUsrs);
   HTM_DIV_End ();

   /* List users one by one */
   HTM_DIV_Begin ("class=\"TL_USRS\"");
      TL_Usr_ListSharersOrFavers (&mysql_res,NumUsrs,NumFirstUsrs);
      if (NumFirstUsrs < NumUsrs)		// Not all are shown
	 /* Clickable ellipsis to show all users */
	 TL_Frm_PutFormToSeeAllFaversSharers (Action[FavSha],
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

void TL_Usr_ShowNumSharersOrFavers (unsigned NumUsrs)
  {
   /***** Show number of sharers or favers
          (users who have shared or marked this note as favourite) *****/
   HTM_TxtF ("&nbsp;%u",NumUsrs);
  }

void TL_Usr_ListSharersOrFavers (MYSQL_RES **mysql_res,
			         unsigned NumUsrs,unsigned NumFirstUsrs)
  {
   MYSQL_ROW row;
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
      row = mysql_fetch_row (*mysql_res);
      UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /***** Get user's data and show user's photo *****/
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS))
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
/***************** Check if I can fav/share a note/comment *******************/
/*****************************************************************************/

bool TL_Usr_CheckICanFavSha (long Cod,long UsrCod)
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
      Lay_NoPermissionExit ();
      return false;	// Not reached
     }

   return true;
  }

/*****************************************************************************/
/************* Check if a user has faved/shared a note/comment ***************/
/*****************************************************************************/

bool TL_Usr_CheckIfFavedSharedByUsr (TL_Usr_FavSha_t FavSha,long Cod,long UsrCod)
  {
   switch (FavSha)
     {
      case TL_Usr_FAV_UNF_NOTE:
      case TL_Usr_FAV_UNF_COMM:
	 return TL_DB_CheckIfFavedByUsr (FavSha,Cod,UsrCod);
      case TL_Usr_SHA_UNS_NOTE:
	 return TL_DB_CheckIfSharedByUsr (Cod,UsrCod);
      default:
         Lay_ShowErrorAndExit ("Wrong fav/share action.");
     }

   return false;	// Not reached
  }

/*****************************************************************************/
/******* Get list of users who have marked a note/comment as favourite *******/
/*****************************************************************************/

unsigned TL_Usr_GetListFaversOrSharers (TL_Usr_FavSha_t FavSha,
                                        long Cod,long UsrCod,unsigned MaxUsrs,
                                        MYSQL_RES **mysql_res)
  {
   switch (FavSha)
     {
      case TL_Usr_FAV_UNF_NOTE:
      case TL_Usr_FAV_UNF_COMM:
	 return TL_DB_GetFavers (FavSha,Cod,UsrCod,MaxUsrs,mysql_res);
      case TL_Usr_SHA_UNS_NOTE:
	 return TL_DB_GetSharers (Cod,UsrCod,MaxUsrs,mysql_res);
      default:
         Lay_ShowErrorAndExit ("Wrong fav/share action.");
     }

   return 0;	// Not reached
  }

/*****************************************************************************/
/**************** Put icon to fav/unfav and list of favers *******************/
/*****************************************************************************/

void TL_Usr_PutIconFavSha (TL_Usr_FavSha_t FavSha,
                           long Cod,long UsrCod,unsigned NumUsrs,
                           TL_Usr_HowManyUsrs_t HowManyUsrs)
  {
   /***** Put form to fav/unfav or share/unshare this note/comment *****/
   /* Begin container */
   HTM_DIV_Begin ("class=\"TL_ICO\"");

      /* Icon to fav/unfav or share/unshare this note/comment */
      if (Usr_ItsMe (UsrCod))	// I am the author ==> I can not fav/unfav or share/unshare
         TL_Usr_PutDisabledIconFavSha (FavSha,NumUsrs);
      else			// I am not the author
	 TL_Frm_PutFormToFavUnfShaUns (FavSha,Cod);

   /* End container */
   HTM_DIV_End ();

   /***** Show who have faved/shared this note/comment *****/
   TL_Usr_GetAndShowSharersOrFavers (FavSha,Cod,UsrCod,NumUsrs,HowManyUsrs);
  }

/*****************************************************************************/
/****************** Put disabled icon to mark as favourite *******************/
/*****************************************************************************/

void TL_Usr_PutDisabledIconFavSha (TL_Usr_FavSha_t FavSha,unsigned NumUsrs)
  {
   extern const char *Txt_TIMELINE_Favourited_by_X_USERS;
   extern const char *Txt_TIMELINE_Not_favourited_by_anyone;
   extern const char *Txt_TIMELINE_Shared_by_X_USERS;
   extern const char *Txt_TIMELINE_Not_shared_by_anyone;
   static const char *Icon[TL_Usr_NUM_FAV_SHA] =
     {
      [TL_Usr_FAV_UNF_NOTE] = TL_Fav_ICON_FAV,
      [TL_Usr_FAV_UNF_COMM] = TL_Fav_ICON_FAV,
      [TL_Usr_SHA_UNS_NOTE] = TL_Sha_ICON_SHARE,
     };
   const char *TitleWithUsrs[TL_Usr_NUM_FAV_SHA] =
     {
      [TL_Usr_FAV_UNF_NOTE] = Txt_TIMELINE_Favourited_by_X_USERS,
      [TL_Usr_FAV_UNF_COMM] = Txt_TIMELINE_Favourited_by_X_USERS,
      [TL_Usr_SHA_UNS_NOTE] = Txt_TIMELINE_Shared_by_X_USERS,
     };
   const char *TitleWithoutUsrs[TL_Usr_NUM_FAV_SHA] =
     {
      [TL_Usr_FAV_UNF_NOTE] = Txt_TIMELINE_Not_favourited_by_anyone,
      [TL_Usr_FAV_UNF_COMM] = Txt_TIMELINE_Not_favourited_by_anyone,
      [TL_Usr_SHA_UNS_NOTE] = Txt_TIMELINE_Not_shared_by_anyone,
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
