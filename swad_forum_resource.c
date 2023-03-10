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
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_forum_database.h"
#include "swad_forum_resource.h"
#include "swad_global.h"
#include "swad_program_database.h"

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
   Prg_DB_CopyToClipboard (PrgRsc_FORUM_THREAD,Forums.Thread.Current);

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
/***************** Write thread subject in course program ********************/
/*****************************************************************************/

void ForRsc_WriteThreadInCrsProgram (long ThrCod,bool PutFormToGo,
                                     const char *Icon,const char *IconTitle)
  {
   extern const char *Txt_Actions[ActLst_NUM_ACTIONS];
   Act_Action_t NextAction;
   struct For_Forums Forums;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];

   /***** Get thread subject *****/
   ForRsc_GetTitleFromThrCod (ThrCod,Subject,sizeof (Subject) - 1);

   /***** Begin form to go to survey *****/
   if (PutFormToGo)
     {
      /***** Set forum and thread *****/
      For_ResetForums (&Forums);
      Forums.Forum.Type = For_FORUM_COURSE_USRS;
      Forums.Forum.Location = Gbl.Hierarchy.Crs.CrsCod;
      Forums.Thread.Current =
      Forums.Thread.Selected = ThrCod;
      // TODO: In the listing of threads, the page is always the first.
      //       The page should be that corresponding to the selected thread.
      NextAction = (ThrCod > 0)	? ActSeePstForCrsUsr :	// Thread specified
				  ActSeeForCrsUsr;	// All threads
      Frm_BeginFormAnchor (NextAction,ThrCod > 0 ? For_FORUM_POSTS_SECTION_ID :
					           For_FORUM_THREADS_SECTION_ID);
	 For_PutParsNewPost (&Forums);
	 HTM_BUTTON_Submit_Begin (Txt_Actions[NextAction],
	                          "class=\"LM BT_LINK PRG_LNK_%s\"",
	                          The_GetSuffix ());
     }

   /***** Icon depending on type ******/
   if (PutFormToGo)
      Ico_PutIconLink (Icon,Ico_BLACK,NextAction);
   else
      Ico_PutIconOn (Icon,Ico_BLACK,IconTitle);

   /***** Write Name of the course and date of exam *****/
   HTM_Txt (Subject);

   /***** End form to download file *****/
   if (PutFormToGo)
     {
      /* End form */
         HTM_BUTTON_End ();

      Frm_EndForm ();
     }
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
