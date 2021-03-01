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
   TL_DB_RemoveAllFavsMadeByUsr (TL_Fav_COMM,UsrCod);

   /* Remove all favs to comments of this user */
   TL_DB_RemoveAllFavsToPubsBy (TL_Fav_COMM,UsrCod);

   /* Remove all favs to all comments in all notes authored by this user */
   TL_DB_RemoveAllFavsToAllCommsInAllNotesBy (UsrCod);

   /***** Remove favs for notes *****/
   /* Remove all favs made by this user to any note */
   TL_DB_RemoveAllFavsMadeByUsr (TL_Fav_NOTE,UsrCod);

   /* Remove all favs to notes of this user */
   TL_DB_RemoveAllFavsToPubsBy (TL_Fav_NOTE,UsrCod);

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
/************************ Show sharers or favouriters ************************/
/*****************************************************************************/

void TL_Usr_ShowNumSharersOrFavers (unsigned NumUsrs)
  {
   /***** Show number of sharers or favers
          (users who have shared or marked this note as favourite) *****/
   HTM_TxtF ("&nbsp;%u",NumUsrs);
  }

void TL_Usr_ShowSharersOrFavers (MYSQL_RES **mysql_res,
			         unsigned NumUsrs,unsigned NumFirstUsrs)
  {
   MYSQL_ROW row;
   unsigned NumUsr;
   unsigned NumUsrsShown = 0;
   struct UsrData UsrDat;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];

   if (NumUsrs)
     {
      /***** A list of users has been got from database *****/
      if (NumFirstUsrs)
	{
	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&UsrDat);

	 /***** List users *****/
	 for (NumUsr = 0;
	      NumUsr < NumFirstUsrs;
	      NumUsr++)
	   {
	    /***** Get user *****/
	    /* Get row */
	    row = mysql_fetch_row (*mysql_res);

	    /* Get user's code (row[0]) */
	    UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	    /***** Get user's data and show user's photo *****/
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS))
	      {
	       /* Begin container */
               HTM_DIV_Begin ("class=\"TL_SHARER\"");

                  /* User's photo */
		  ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL);
		  Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
							NULL,
				    "PHOTO12x16",Pho_ZOOM,true);	// Use unique id

	       /* End container */
               HTM_DIV_End ();

               NumUsrsShown++;
              }
	   }

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);
	}
     }
  }
