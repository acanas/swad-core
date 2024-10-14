// swad_timeline_user.c: social timeline users

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

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_alert.h"
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
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TmlUsr_GetAndShowSharersOrFavers (TmlUsr_FavSha_t FavSha,
                                              long Cod,long UsrCod,unsigned NumUsrs,
				              TmlUsr_HowManyUsrs_t HowManyUsrs);
static void TmlUsr_ShowNumSharersOrFavers (unsigned NumUsrs);
static void TmlUsr_ListSharersOrFavers (MYSQL_RES **mysql_res,
			                unsigned NumUsrs,unsigned NumFirstUsrs);
static unsigned TmlUsr_GetListFaversOrSharers (TmlUsr_FavSha_t FavSha,
                                               long Cod,long UsrCod,unsigned MaxUsrs,
                                               MYSQL_RES **mysql_res);

static void TmlUsr_PutDisabledIconFavSha (TmlUsr_FavSha_t FavSha,
                                          unsigned NumUsrs);

/*****************************************************************************/
/*************** Remove all content of a user from database ******************/
/*****************************************************************************/

void TmlUsr_RemoveUsrContent (long UsrCod)
  {
   /***** Remove favs for comments *****/
   /* Remove all favs made by this user to any comment */
   Tml_DB_RemoveAllFavsMadeByUsr (TmlUsr_FAV_UNF_COMM,UsrCod);

   /* Remove all favs to comments of this user */
   Tml_DB_RemoveAllFavsToPubsBy (TmlUsr_FAV_UNF_COMM,UsrCod);

   /* Remove all favs to all comments in all notes authored by this user */
   Tml_DB_RemoveAllFavsToAllCommsInAllNotesBy (UsrCod);

   /***** Remove favs for notes *****/
   /* Remove all favs made by this user to any note */
   Tml_DB_RemoveAllFavsMadeByUsr (TmlUsr_FAV_UNF_NOTE,UsrCod);

   /* Remove all favs to notes of this user */
   Tml_DB_RemoveAllFavsToPubsBy (TmlUsr_FAV_UNF_NOTE,UsrCod);

   /***** Remove comments *****/
   /* Remove all comments in all notes of this user */
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

static void TmlUsr_GetAndShowSharersOrFavers (TmlUsr_FavSha_t FavSha,
                                              long Cod,long UsrCod,unsigned NumUsrs,
				              TmlUsr_HowManyUsrs_t HowManyUsrs)
  {
   static const TmlFrm_Action_t Action[TmlUsr_NUM_FAV_SHA] =
     {
      [TmlUsr_FAV_UNF_NOTE] = TmlFrm_ALL_FAV_NOTE,
      [TmlUsr_FAV_UNF_COMM] = TmlFrm_ALL_FAV_COMM,
      [TmlUsr_SHA_UNS_NOTE] = TmlFrm_ALL_SHA_NOTE,
     };
   static const char *ParFormat[TmlUsr_NUM_FAV_SHA] =
     {
      [TmlUsr_FAV_UNF_NOTE] = "NotCod=%ld",
      [TmlUsr_FAV_UNF_COMM] = "PubCod=%ld",
      [TmlUsr_SHA_UNS_NOTE] = "NotCod=%ld",
     };
   struct Tml_Form Form =
     {
      .Action      = Action[FavSha],
      .ParFormat = ParFormat[FavSha],
      .ParCod    = Cod,
      .Icon        = "ellipsis-h.svg",
      .Color       = Ico_BLACK,
     };
   MYSQL_RES *mysql_res;
   unsigned NumFirstUsrs;

   /***** Get users who have faved/shared *****/
   if (NumUsrs)
      NumFirstUsrs =
      TmlUsr_GetListFaversOrSharers (FavSha,Cod,UsrCod,
                                      HowManyUsrs == TmlUsr_SHOW_FEW_USRS ? TmlUsr_DEF_USRS_SHOWN :
				                                            TmlUsr_MAX_USRS_SHOWN,
                                      &mysql_res);
   else
      NumFirstUsrs = 0;

   /***** Show users *****/
   /* Number of users */
   HTM_DIV_Begin ("class=\"Tml_NUM_USRS\"");
      TmlUsr_ShowNumSharersOrFavers (NumUsrs);
   HTM_DIV_End ();

   /* List users one by one */
   HTM_DIV_Begin ("class=\"Tml_USRS\"");
      TmlUsr_ListSharersOrFavers (&mysql_res,NumUsrs,NumFirstUsrs);
      if (NumFirstUsrs < NumUsrs)		// Not all are shown
	 /* Clickable ellipsis to show all users */
         TmlFrm_FormFavSha (&Form);
   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   if (NumUsrs)
      DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Show sharers or favouriters ************************/
/*****************************************************************************/

static void TmlUsr_ShowNumSharersOrFavers (unsigned NumUsrs)
  {
   /***** Show number of sharers or favers
          (users who have shared or marked this note as favourite) *****/
   HTM_TxtF ("&nbsp;%u",NumUsrs);
  }

static void TmlUsr_ListSharersOrFavers (MYSQL_RES **mysql_res,
			                unsigned NumUsrs,unsigned NumFirstUsrs)
  {
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC12x16",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE12x16",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO12x16",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR12x16",
     };
   unsigned NumUsr;
   struct Usr_Data UsrDat;

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
                                                   Usr_DONT_GET_ROLE_IN_CRS))
	{
	 /* Begin container */
	 HTM_DIV_Begin ("class=\"Tml_SHARER\"");

	    /* User's photo */
	    Pho_ShowUsrPhotoIfAllowed (&UsrDat,
	                               ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);

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

bool TmlUsr_CheckIfFavedSharedByUsr (TmlUsr_FavSha_t FavSha,long Cod,long UsrCod)
  {
   switch (FavSha)
     {
      case TmlUsr_FAV_UNF_NOTE:
      case TmlUsr_FAV_UNF_COMM:
	 return Tml_DB_CheckIfFavedByUsr (FavSha,Cod,UsrCod);
      case TmlUsr_SHA_UNS_NOTE:
	 return Tml_DB_CheckIfSharedByUsr (Cod,UsrCod);
      default:
         Err_ShowErrorAndExit ("Wrong fav/share action.");
     }

   return false;	// Not reached
  }

/*****************************************************************************/
/******* Get list of users who have marked a note/comment as favourite *******/
/*****************************************************************************/

static unsigned TmlUsr_GetListFaversOrSharers (TmlUsr_FavSha_t FavSha,
                                                long Cod,long UsrCod,unsigned MaxUsrs,
                                                MYSQL_RES **mysql_res)
  {
   switch (FavSha)
     {
      case TmlUsr_FAV_UNF_NOTE:
      case TmlUsr_FAV_UNF_COMM:
	 return Tml_DB_GetFavers (FavSha,Cod,UsrCod,MaxUsrs,mysql_res);
      case TmlUsr_SHA_UNS_NOTE:
	 return Tml_DB_GetSharers (Cod,UsrCod,MaxUsrs,mysql_res);
      default:
         Err_ShowErrorAndExit ("Wrong fav/share action.");
     }

   return 0;	// Not reached
  }

/*****************************************************************************/
/**************** Put icon to fav/unfav and list of favers *******************/
/*****************************************************************************/

void TmlUsr_PutIconFavSha (TmlUsr_FavSha_t FavSha,
                           long Cod,long UsrCod,unsigned NumUsrs,
                           TmlUsr_HowManyUsrs_t HowManyUsrs)
  {
   /***** Put form to fav/unfav or share/unshare this note/comment *****/
   /* Begin container */
   HTM_DIV_Begin ("class=\"Tml_ICO\"");

      /* Icon to fav/unfav or share/unshare this note/comment */
      if (Usr_ItsMe (UsrCod) == Usr_OTHER)	// I am not the author
	 TmlFrm_PutFormToFavUnfShaUns (FavSha,Cod);
      else
         // I can not fav/unfav or share/unshare
         TmlUsr_PutDisabledIconFavSha (FavSha,NumUsrs);

   /* End container */
   HTM_DIV_End ();

   /***** Show who have faved/shared this note/comment *****/
   TmlUsr_GetAndShowSharersOrFavers (FavSha,Cod,UsrCod,NumUsrs,HowManyUsrs);
  }

/*****************************************************************************/
/****************** Put disabled icon to mark as favourite *******************/
/*****************************************************************************/

static void TmlUsr_PutDisabledIconFavSha (TmlUsr_FavSha_t FavSha,
                                          unsigned NumUsrs)
  {
   extern const char *Txt_TIMELINE_Favourited_by_X_USERS;
   extern const char *Txt_TIMELINE_Not_favourited_by_anyone;
   extern const char *Txt_TIMELINE_Shared_by_X_USERS;
   extern const char *Txt_TIMELINE_Not_shared_by_anyone;
   static const struct
     {
      const char *Icon;
      struct
        {
         const char **WithUsrs;
         const char **WithoutUsrs;
        } Title;
     } Ico[TmlUsr_NUM_FAV_SHA] =
     {
      [TmlUsr_FAV_UNF_NOTE] =
	 {
	  .Icon = TmlFav_ICON_FAV,
	  .Title =
	    {
             .WithUsrs    = &Txt_TIMELINE_Favourited_by_X_USERS,
             .WithoutUsrs = &Txt_TIMELINE_Not_favourited_by_anyone,
	    },
	 },
      [TmlUsr_FAV_UNF_COMM] =
	 {
	  .Icon = TmlFav_ICON_FAV,
	  .Title =
	    {
             .WithUsrs    = &Txt_TIMELINE_Favourited_by_X_USERS,
             .WithoutUsrs = &Txt_TIMELINE_Not_favourited_by_anyone,
	    },
	 },
      [TmlUsr_SHA_UNS_NOTE] =
	 {
	  .Icon = TmlSha_ICON_SHARE,
	  .Title =
	    {
             .WithUsrs    = &Txt_TIMELINE_Shared_by_X_USERS,
             .WithoutUsrs = &Txt_TIMELINE_Not_shared_by_anyone,
	    },
	 },
     };
   char *Title;

   /***** Disabled icon to fav/share *****/
   if (NumUsrs)
     {
      if (asprintf (&Title,*Ico[FavSha].Title.WithUsrs,NumUsrs) < 0)
	 Err_NotEnoughMemoryExit ();
      Ico_PutDivIcon ("Tml_ICO_DISABLED",Ico[FavSha].Icon,Ico_BLACK,Title);
      free (Title);
     }
   else
      Ico_PutDivIcon ("Tml_ICO_DISABLED",Ico[FavSha].Icon,Ico_BLACK,
                      *Ico[FavSha].Title.WithoutUsrs);
  }

/*****************************************************************************/
/***************** Check if I can fav/share a note/comment *******************/
/*****************************************************************************/

Usr_Can_t TmlUsr_CheckIfICanFavSha (long Cod,long UsrCod)
  {
   extern const char *Txt_The_post_no_longer_exists;

   /***** Trivial check 1: note/comment code should be > 0 *****/
   if (Cod <= 0)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_The_post_no_longer_exists);
      return Usr_CAN_NOT;
     }

   /***** Trivial check 2: I must be logged
			   I can not fav/share my own notes/comments *****/
   if (!Gbl.Usrs.Me.Logged || Usr_ItsMe (UsrCod) == Usr_ME)
     {
      Err_NoPermission ();
      return Usr_CAN_NOT;
     }

   return Usr_CAN;
  }

/*****************************************************************************/
/***************** Check if I can fav/share a note/comment *******************/
/*****************************************************************************/

Usr_Can_t TmlUsr_CheckIfICanRemove (long Cod,long UsrCod)
  {
   extern const char *Txt_The_post_no_longer_exists;

   /***** Trivial check 1: note/comment code should be > 0 *****/
   if (Cod <= 0)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_The_post_no_longer_exists);
      return Usr_CAN_NOT;
     }

   /***** Trivial check 2: I must be logged
			   I can only remove my own notes/comments *****/
   if (!Gbl.Usrs.Me.Logged || Usr_ItsMe (UsrCod) == Usr_OTHER)
     {
      Err_NoPermission ();
      return Usr_CAN_NOT;
     }

   return Usr_CAN;
  }
