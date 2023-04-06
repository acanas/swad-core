// swad_forum_resource.c: links to course forum threads as program resources
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_forum.h"
#include "swad_forum_database.h"
#include "swad_forum_resource.h"
#include "swad_global.h"
#include "swad_parameter_code.h"
#include "swad_resource_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Get link to thread *****************************/
/*****************************************************************************/

void ForRsc_GetLinkToThread (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   struct For_Forums Forums;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];

   /***** Reset forum *****/
   For_ResetForums (&Forums);

   /***** Get parameters related to forums *****/
   For_GetParsForums (&Forums);

   /***** Get thread subject *****/
   ForRsc_GetTitleFromThrCod (Forums.Thread.Current,Subject,sizeof (Subject) - 1);

   /***** Copy link to thread into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_FORUM_THREAD,Forums.Thread.Current);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Subject);

   /***** Show forum list again *****/
   For_ShowForumList (&Forums);

   /***** Show threads again *****/
   For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_SUCCESS,NULL);

   /***** Show the posts of that thread *****/
   For_ShowPostsOfAThread (&Forums,Ale_SUCCESS,NULL);
  }

/*****************************************************************************/
/************** Build/free anchor string given a thread code *****************/
/*****************************************************************************/

void ForRsc_SetAnchorStr (long ThrCod,char **Anchor)
  {
   *Anchor = (ThrCod > 0) ? For_FORUM_POSTS_SECTION_ID :
			    For_FORUM_THREADS_SECTION_ID;
  }

void ForRsc_FreeAnchorStr (char **Anchor)
  {
   *Anchor = NULL;
  }

/*****************************************************************************/
/********************* Get survey title from survey code *********************/
/*****************************************************************************/
// The trailing null character is not counted in TitleSize

void ForRsc_GetTitleFromThrCod (long ThrCod,char *Title,size_t TitleSize)
  {
   extern const char *Txt_Forum;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];

   if (ThrCod > 0)
     {
      /***** Get thread subject *****/
      For_DB_GetThreadSubject (ThrCod,Subject);
      Str_Copy (Title,Subject,TitleSize);
     }
   else
      snprintf (Title,TitleSize + 1,"%s %s",
                Txt_Forum,Gbl.Hierarchy.Crs.ShrtName);
  }
