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
/**************************** Get link to thread *****************************/
/*****************************************************************************/

void ForRsc_GetLinkToThread (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_Course_forum;
   struct For_Forums Forums;
   struct For_Thread Thr;

   /***** Reset forum *****/
   For_ResetForums (&Forums);

   /***** Get parameters related to forums *****/
   For_GetParsForums (&Forums);
   Thr.ThrCod = Forums.Thread.Current;

   /***** Get thread subject *****/
   if (Thr.ThrCod > 0)
      For_GetThreadData (&Thr);

   /***** Copy link to thread into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_FORUM_THREAD,Thr.ThrCod);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Thr.ThrCod > 0 ? Thr.Subject :
   				   Txt_Course_forum);

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
