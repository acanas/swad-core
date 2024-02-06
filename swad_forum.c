// swad_forum.c: forums

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <malloc.h>		// For malloc
#include <mysql/mysql.h>	// To access MySQL databases
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <string.h>
#include <time.h>		// For time_t

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_center_database.h"
#include "swad_config.h"
#include "swad_course_database.h"
#include "swad_database.h"
#include "swad_degree_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_forum_database.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_HTML.h"
#include "swad_layout.h"
#include "swad_logo.h"
#include "swad_message.h"
#include "swad_notification.h"
#include "swad_notification_database.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_profile.h"
#include "swad_profile_database.h"
#include "swad_program_database.h"
#include "swad_role.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************ Public constants and types *************************/
/*****************************************************************************/

const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM] =
  {
   [For_FORUM_COURSE_USRS] = ActSeeForCrsUsr,
   [For_FORUM_COURSE_TCHS] = ActSeeForCrsTch,
   [For_FORUM_DEGREE_USRS] = ActSeeForDegUsr,
   [For_FORUM_DEGREE_TCHS] = ActSeeForDegTch,
   [For_FORUM_CENTER_USRS] = ActSeeForCtrUsr,
   [For_FORUM_CENTER_TCHS] = ActSeeForCtrTch,
   [For_FORUM_INSTIT_USRS] = ActSeeForInsUsr,
   [For_FORUM_INSTIT_TCHS] = ActSeeForInsTch,
   [For_FORUM_GLOBAL_USRS] = ActSeeForGenUsr,
   [For_FORUM_GLOBAL_TCHS] = ActSeeForGenTch,
   [For_FORUM__SWAD__USRS] = ActSeeForSWAUsr,
   [For_FORUM__SWAD__TCHS] = ActSeeForSWATch,
   [For_FORUM_UNKNOWN    ] = ActSeeFor,
  };

const Act_Action_t For_ActionsSeePstFor[For_NUM_TYPES_FORUM] =
  {
   [For_FORUM_COURSE_USRS] = ActSeePstForCrsUsr,
   [For_FORUM_COURSE_TCHS] = ActSeePstForCrsTch,
   [For_FORUM_DEGREE_USRS] = ActSeePstForDegUsr,
   [For_FORUM_DEGREE_TCHS] = ActSeePstForDegTch,
   [For_FORUM_CENTER_USRS] = ActSeePstForCtrUsr,
   [For_FORUM_CENTER_TCHS] = ActSeePstForCtrTch,
   [For_FORUM_INSTIT_USRS] = ActSeePstForInsUsr,
   [For_FORUM_INSTIT_TCHS] = ActSeePstForInsTch,
   [For_FORUM_GLOBAL_USRS] = ActSeePstForGenUsr,
   [For_FORUM_GLOBAL_TCHS] = ActSeePstForGenTch,
   [For_FORUM__SWAD__USRS] = ActSeePstForSWAUsr,
   [For_FORUM__SWAD__TCHS] = ActSeePstForSWATch,
   [For_FORUM_UNKNOWN    ] = ActSeeFor,
  };

const Act_Action_t For_ActionsRecThrFor[For_NUM_TYPES_FORUM] =
  {
   [For_FORUM_COURSE_USRS] = ActRcvThrForCrsUsr,
   [For_FORUM_COURSE_TCHS] = ActRcvThrForCrsTch,
   [For_FORUM_DEGREE_USRS] = ActRcvThrForDegUsr,
   [For_FORUM_DEGREE_TCHS] = ActRcvThrForDegTch,
   [For_FORUM_CENTER_USRS] = ActRcvThrForCtrUsr,
   [For_FORUM_CENTER_TCHS] = ActRcvThrForCtrTch,
   [For_FORUM_INSTIT_USRS] = ActRcvThrForInsUsr,
   [For_FORUM_INSTIT_TCHS] = ActRcvThrForInsTch,
   [For_FORUM_GLOBAL_USRS] = ActRcvThrForGenUsr,
   [For_FORUM_GLOBAL_TCHS] = ActRcvThrForGenTch,
   [For_FORUM__SWAD__USRS] = ActRcvThrForSWAUsr,
   [For_FORUM__SWAD__TCHS] = ActRcvThrForSWATch,
   [For_FORUM_UNKNOWN    ] = ActSeeFor,
  };

const Act_Action_t For_ActionsRecRepFor[For_NUM_TYPES_FORUM] =
  {
   [For_FORUM_COURSE_USRS] = ActRcvRepForCrsUsr,
   [For_FORUM_COURSE_TCHS] = ActRcvRepForCrsTch,
   [For_FORUM_DEGREE_USRS] = ActRcvRepForDegUsr,
   [For_FORUM_DEGREE_TCHS] = ActRcvRepForDegTch,
   [For_FORUM_CENTER_USRS] = ActRcvRepForCtrUsr,
   [For_FORUM_CENTER_TCHS] = ActRcvRepForCtrTch,
   [For_FORUM_INSTIT_USRS] = ActRcvRepForInsUsr,
   [For_FORUM_INSTIT_TCHS] = ActRcvRepForInsTch,
   [For_FORUM_GLOBAL_USRS] = ActRcvRepForGenUsr,
   [For_FORUM_GLOBAL_TCHS] = ActRcvRepForGenTch,
   [For_FORUM__SWAD__USRS] = ActRcvRepForSWAUsr,
   [For_FORUM__SWAD__TCHS] = ActRcvRepForSWATch,
   [For_FORUM_UNKNOWN    ] = ActSeeFor,
  };

const Act_Action_t For_ActionsReqDelThr[For_NUM_TYPES_FORUM] =
  {
   [For_FORUM_COURSE_USRS] = ActReqDelThrCrsUsr,
   [For_FORUM_COURSE_TCHS] = ActReqDelThrCrsTch,
   [For_FORUM_DEGREE_USRS] = ActReqDelThrDegUsr,
   [For_FORUM_DEGREE_TCHS] = ActReqDelThrDegTch,
   [For_FORUM_CENTER_USRS] = ActReqDelThrCtrUsr,
   [For_FORUM_CENTER_TCHS] = ActReqDelThrCtrTch,
   [For_FORUM_INSTIT_USRS] = ActReqDelThrInsUsr,
   [For_FORUM_INSTIT_TCHS] = ActReqDelThrInsTch,
   [For_FORUM_GLOBAL_USRS] = ActReqDelThrGenUsr,
   [For_FORUM_GLOBAL_TCHS] = ActReqDelThrGenTch,
   [For_FORUM__SWAD__USRS] = ActReqDelThrSWAUsr,
   [For_FORUM__SWAD__TCHS] = ActReqDelThrSWATch,
   [For_FORUM_UNKNOWN    ] = ActSeeFor,
  };

const Act_Action_t For_ActionsDelThrFor[For_NUM_TYPES_FORUM] =
  {
   [For_FORUM_COURSE_USRS] = ActDelThrForCrsUsr,
   [For_FORUM_COURSE_TCHS] = ActDelThrForCrsTch,
   [For_FORUM_DEGREE_USRS] = ActDelThrForDegUsr,
   [For_FORUM_DEGREE_TCHS] = ActDelThrForDegTch,
   [For_FORUM_CENTER_USRS] = ActDelThrForCtrUsr,
   [For_FORUM_CENTER_TCHS] = ActDelThrForCtrTch,
   [For_FORUM_INSTIT_USRS] = ActDelThrForInsUsr,
   [For_FORUM_INSTIT_TCHS] = ActDelThrForInsTch,
   [For_FORUM_GLOBAL_USRS] = ActDelThrForGenUsr,
   [For_FORUM_GLOBAL_TCHS] = ActDelThrForGenTch,
   [For_FORUM__SWAD__USRS] = ActDelThrForSWAUsr,
   [For_FORUM__SWAD__TCHS] = ActDelThrForSWATch,
   [For_FORUM_UNKNOWN    ] = ActSeeFor,
  };

const Act_Action_t For_ActionsCutThrFor[For_NUM_TYPES_FORUM] =
  {
   [For_FORUM_COURSE_USRS] = ActCutThrForCrsUsr,
   [For_FORUM_COURSE_TCHS] = ActCutThrForCrsTch,
   [For_FORUM_DEGREE_USRS] = ActCutThrForDegUsr,
   [For_FORUM_DEGREE_TCHS] = ActCutThrForDegTch,
   [For_FORUM_CENTER_USRS] = ActCutThrForCtrUsr,
   [For_FORUM_CENTER_TCHS] = ActCutThrForCtrTch,
   [For_FORUM_INSTIT_USRS] = ActCutThrForInsUsr,
   [For_FORUM_INSTIT_TCHS] = ActCutThrForInsTch,
   [For_FORUM_GLOBAL_USRS] = ActCutThrForGenUsr,
   [For_FORUM_GLOBAL_TCHS] = ActCutThrForGenTch,
   [For_FORUM__SWAD__USRS] = ActCutThrForSWAUsr,
   [For_FORUM__SWAD__TCHS] = ActCutThrForSWATch,
   [For_FORUM_UNKNOWN    ] = ActSeeFor,
  };

const Act_Action_t For_ActionsPasThrFor[For_NUM_TYPES_FORUM] =
  {
   [For_FORUM_COURSE_USRS] = ActPasThrForCrsUsr,
   [For_FORUM_COURSE_TCHS] = ActPasThrForCrsTch,
   [For_FORUM_DEGREE_USRS] = ActPasThrForDegUsr,
   [For_FORUM_DEGREE_TCHS] = ActPasThrForDegTch,
   [For_FORUM_CENTER_USRS] = ActPasThrForCtrUsr,
   [For_FORUM_CENTER_TCHS] = ActPasThrForCtrTch,
   [For_FORUM_INSTIT_USRS] = ActPasThrForInsUsr,
   [For_FORUM_INSTIT_TCHS] = ActPasThrForInsTch,
   [For_FORUM_GLOBAL_USRS] = ActPasThrForGenUsr,
   [For_FORUM_GLOBAL_TCHS] = ActPasThrForGenTch,
   [For_FORUM__SWAD__USRS] = ActPasThrForSWAUsr,
   [For_FORUM__SWAD__TCHS] = ActPasThrForSWATch,
   [For_FORUM_UNKNOWN    ] = ActSeeFor,
  };

const Act_Action_t For_ActionsDelPstFor[For_NUM_TYPES_FORUM] =
  {
   [For_FORUM_COURSE_USRS] = ActDelPstForCrsUsr,
   [For_FORUM_COURSE_TCHS] = ActDelPstForCrsTch,
   [For_FORUM_DEGREE_USRS] = ActDelPstForDegUsr,
   [For_FORUM_DEGREE_TCHS] = ActDelPstForDegTch,
   [For_FORUM_CENTER_USRS] = ActDelPstForCtrUsr,
   [For_FORUM_CENTER_TCHS] = ActDelPstForCtrTch,
   [For_FORUM_INSTIT_USRS] = ActDelPstForInsUsr,
   [For_FORUM_INSTIT_TCHS] = ActDelPstForInsTch,
   [For_FORUM_GLOBAL_USRS] = ActDelPstForGenUsr,
   [For_FORUM_GLOBAL_TCHS] = ActDelPstForGenTch,
   [For_FORUM__SWAD__USRS] = ActDelPstForSWAUsr,
   [For_FORUM__SWAD__TCHS] = ActDelPstForSWATch,
   [For_FORUM_UNKNOWN    ] = ActSeeFor,
  };

const Act_Action_t For_ActionsEnbPstFor[For_NUM_TYPES_FORUM] =
  {
   [For_FORUM_COURSE_USRS] = ActEnbPstForCrsUsr,
   [For_FORUM_COURSE_TCHS] = ActEnbPstForCrsTch,
   [For_FORUM_DEGREE_USRS] = ActEnbPstForDegUsr,
   [For_FORUM_DEGREE_TCHS] = ActEnbPstForDegTch,
   [For_FORUM_CENTER_USRS] = ActEnbPstForCtrUsr,
   [For_FORUM_CENTER_TCHS] = ActEnbPstForCtrTch,
   [For_FORUM_INSTIT_USRS] = ActEnbPstForInsUsr,
   [For_FORUM_INSTIT_TCHS] = ActEnbPstForInsTch,
   [For_FORUM_GLOBAL_USRS] = ActEnbPstForGenUsr,
   [For_FORUM_GLOBAL_TCHS] = ActEnbPstForGenTch,
   [For_FORUM__SWAD__USRS] = ActEnbPstForSWAUsr,
   [For_FORUM__SWAD__TCHS] = ActEnbPstForSWATch,
   [For_FORUM_UNKNOWN    ] = ActSeeFor,
  };

const Act_Action_t For_ActionsDisPstFor[For_NUM_TYPES_FORUM] =
  {
   [For_FORUM_COURSE_USRS] = ActDisPstForCrsUsr,
   [For_FORUM_COURSE_TCHS] = ActDisPstForCrsTch,
   [For_FORUM_DEGREE_USRS] = ActDisPstForDegUsr,
   [For_FORUM_DEGREE_TCHS] = ActDisPstForDegTch,
   [For_FORUM_CENTER_USRS] = ActDisPstForCtrUsr,
   [For_FORUM_CENTER_TCHS] = ActDisPstForCtrTch,
   [For_FORUM_INSTIT_USRS] = ActDisPstForInsUsr,
   [For_FORUM_INSTIT_TCHS] = ActDisPstForInsTch,
   [For_FORUM_GLOBAL_USRS] = ActDisPstForGenUsr,
   [For_FORUM_GLOBAL_TCHS] = ActDisPstForGenTch,
   [For_FORUM__SWAD__USRS] = ActDisPstForSWAUsr,
   [For_FORUM__SWAD__TCHS] = ActDisPstForSWATch,
   [For_FORUM_UNKNOWN    ] = ActSeeFor,
  };

/*****************************************************************************/
/*********************** Private constants and types *************************/
/*****************************************************************************/

#define For_FORUM_MAX_LEVELS 5

static const unsigned PermissionThreadDeletion[For_NUM_TYPES_FORUM] =
  {
   [For_FORUM_COURSE_USRS] = 0x3F0,
   [For_FORUM_COURSE_TCHS] = 0x3F0,
   [For_FORUM_DEGREE_USRS] = 0x3E0,
   [For_FORUM_DEGREE_TCHS] = 0x3E0,
   [For_FORUM_CENTER_USRS] = 0x3C0,
   [For_FORUM_CENTER_TCHS] = 0x3C0,
   [For_FORUM_INSTIT_USRS] = 0x380,
   [For_FORUM_INSTIT_TCHS] = 0x380,
   [For_FORUM_GLOBAL_USRS] = 0x300,
   [For_FORUM_GLOBAL_TCHS] = 0x300,
   [For_FORUM__SWAD__USRS] = 0x300,
   [For_FORUM__SWAD__TCHS] = 0x300,
   [For_FORUM_UNKNOWN    ] = 0x000,
  };

// Forum images will be saved with:
// - maximum width of For_IMAGE_SAVED_MAX_HEIGHT
// - maximum height of For_IMAGE_SAVED_MAX_HEIGHT
// - maintaining the original aspect ratio (aspect ratio recommended: 3:2)
#define For_IMAGE_SAVED_MAX_WIDTH	768
#define For_IMAGE_SAVED_MAX_HEIGHT	768
#define For_IMAGE_SAVED_QUALITY		 90	// 1 to 100

struct For_FiguresForum
  {
   unsigned NumForums;
   unsigned NumThreads;
   unsigned NumPosts;
   unsigned NumUsrsToBeNotifiedByEMail;
  };

/*****************************************************************************/
/****************************** Private prototypes ***************************/
/*****************************************************************************/

static long For_InsertForumPst (long ThrCod,long UsrCod,
                                const char *Subject,const char *Content,
                                struct Med_Media *Media);
static bool For_RemoveForumPst (long PstCod,long MedCod);
static unsigned For_NumPstsInThrWithPstCod (long PstCod,long *ThrCod);

static void For_RemoveThreadOnly (long ThrCod);
static void For_RemoveThreadAndItsPsts (long ThrCod);

static time_t For_GetThrReadTime (long ThrCod);

static void For_PutIconsOneThread (void *Forums);

static void For_PutParsNewPost (void *Forums);

static void For_ShowAForumPost (struct For_Forums *Forums,
	                        unsigned PstNum,
                                bool LastPst,char LastSubject[Cns_MAX_BYTES_SUBJECT + 1],
                                bool NewPst,bool ICanModerateForum);
static void For_GetPstData (long PstCod,long *UsrCod,time_t *CreatTimeUTC,
                            char Subject[Cns_MAX_BYTES_SUBJECT   + 1],
                            char Content[Cns_MAX_BYTES_LONG_TEXT + 1],
                            struct Med_Media *Media);
static void For_WriteNumberOfPosts (const struct For_Forums *Forums,long UsrCod);

static void For_PutParsForum (void *Forums);

static void For_PutParForumSet (For_ForumSet_t ForumSet);

static void For_PutIconsForums (__attribute__((unused)) void *Args);
static void For_PutFormWhichForums (const struct For_Forums *Forums);

static void For_WriteLinksToGblForums (const struct For_Forums *Forums,
                                       Lay_LastItem_t IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS]);
static void For_WriteLinksToPlatformForums (const struct For_Forums *Forums,
					    bool IsLastForum,
                                            Lay_LastItem_t IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS]);
static long For_WriteLinksToInsForums (const struct For_Forums *Forums,
				       long InsCod,bool IsLastIns,
                                       Lay_LastItem_t IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS]);
static long For_WriteLinksToCtrForums (const struct For_Forums *Forums,
				       long CtrCod,bool IsLastCtr,
                                       Lay_LastItem_t IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS]);
static long For_WriteLinksToDegForums (const struct For_Forums *Forums,
				       long DegCod,bool IsLastDeg,
                                       Lay_LastItem_t IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS]);
static long For_WriteLinksToCrsForums (const struct For_Forums *Forums,
				       long CrsCod,bool IsLastCrs,
                                       Lay_LastItem_t IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS]);
static void For_WriteLinkToForum (const struct For_Forums *Forums,
	                          const struct For_Forum *Forum,
                                  Lay_Highlight_t Highlight,
                                  unsigned Level,
                                  Lay_LastItem_t IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS]);
static unsigned For_GetNumThrsWithNewPstsInForum (const struct For_Forum *Forum,
                                                  unsigned NumThreads);
static unsigned For_GetNumOfUnreadPostsInThr (long ThrCod,unsigned NumPostsInThr);

static void For_WriteNumberOfThrs (unsigned NumThrs);

static void For_PutIconsThreads (void *Forums);
static void For_PutParsNewThread (void *Forums);
static void For_ListForumThrs (struct For_Forums *Forums,
	                       long ThrCods[Pag_ITEMS_PER_PAGE],
                               long ThrCodHighlighted,
                               struct Pag_Pagination *PaginationThrs);

static void For_SetForumType (struct For_Forums *Forums);
static void For_RestrictAccess (const struct For_Forums *Forums);

static void For_WriteFormForumPst (struct For_Forums *Forums,
                                   bool IsReply,const char *Subject);

static void For_PutParsRemThread (void *Forums);

static bool For_CheckIfICanMoveThreads (void);
static void For_InsertThrInClipboard (long ThrCod);

static void For_ShowStatOfAForumType (For_ForumType_t ForumType,
				      long HieCod[Hie_NUM_LEVELS],
                                      struct For_FiguresForum *FiguresForum);
static void For_WriteForumTitleAndStats (For_ForumType_t ForumType,
					 long HieCod[Hie_NUM_LEVELS],
                                         const char *Icon,struct For_FiguresForum *FiguresForum,
                                         const char *ForumName1,const char *ForumName2);
static void For_WriteForumTotalStats (struct For_FiguresForum *FiguresForum);

/*****************************************************************************/
/********************************** Reset forum ******************************/
/*****************************************************************************/

void For_ResetForums (struct For_Forums *Forums)
  {
   Forums->ForumSet        = For_DEFAULT_FORUM_SET;
   Forums->ThreadsOrder    = For_DEFAULT_ORDER;
   Forums->CurrentPageThrs = 0;
   Forums->CurrentPagePsts = 0;

   Forums->Forum.Type      = For_FORUM_UNKNOWN;
   Forums->Forum.HieCod  = -1L;

   Forums->Thread.Selected =
   Forums->Thread.Current  =
   Forums->Thread.ToMove   = -1L;

   Forums->PstCod          = -1L;
  }

/*****************************************************************************/
/****************************** Enable a forum post **************************/
/*****************************************************************************/

void For_EnablePost (void)
  {
   extern const char *Txt_FORUM_Post_unbanned;
   struct For_Forums Forums;

   /***** Reset forum *****/
   For_ResetForums (&Forums);

   /***** Get parameters related to forum *****/
   For_GetParsForums (&Forums);

   /***** Delete post from table of disabled posts *****/
   For_DB_RemovePstFromDisabled (Forums.PstCod);

   /***** Show forum list again *****/
   For_ShowForumList (&Forums);

   /***** Show threads again *****/
   For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_SUCCESS,NULL);

   /***** Show the posts again *****/
   For_ShowPostsOfAThread (&Forums,Ale_SUCCESS,Txt_FORUM_Post_unbanned);
  }

/*****************************************************************************/
/***************************** Disable a forum post **************************/
/*****************************************************************************/

void For_DisablePost (void)
  {
   extern const char *Txt_FORUM_Post_banned;
   struct For_Forums Forums;

   /***** Get parameters related to forums *****/
   For_GetParsForums (&Forums);

   /***** Check if post really exists, if it has not been removed *****/
   if (For_DB_CheckIfForumPstExists (Forums.PstCod))
     {
      /***** Insert post into table of banned posts *****/
      For_DB_InsertPstIntoDisabled (Forums.PstCod);

      /***** Show forum list again *****/
      For_ShowForumList (&Forums);

      /***** Show threads again *****/
      For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_SUCCESS,NULL);

      /***** Show the posts again *****/
      For_ShowPostsOfAThread (&Forums,Ale_SUCCESS,Txt_FORUM_Post_banned);
     }
   else
      Err_WrongPostExit ();
  }

/*****************************************************************************/
/************** Insert a post new in the table of posts of forums ************/
/*****************************************************************************/

static long For_InsertForumPst (long ThrCod,long UsrCod,
                                const char *Subject,const char *Content,
                                struct Med_Media *Media)
  {
   /***** Store media in filesystem and database *****/
   Med_RemoveKeepOrStoreMedia (-1L,Media);

   /***** Insert forum post in the database *****/
   return For_DB_InsertForumPst (ThrCod,UsrCod,Subject,Content,Media->MedCod);
  }

/*****************************************************************************/
/***************** Delete a post from the forum post table *******************/
/*****************************************************************************/
// Return true if the post thread is deleted

static bool For_RemoveForumPst (long PstCod,long MedCod)
  {
   long ThrCod;
   bool ThreadDeleted = false;

   /***** Remove media file attached to forum post *****/
   Med_RemoveMedia (MedCod);

   /***** If the post is the only one in its thread, delete that thread *****/
   if (For_NumPstsInThrWithPstCod (PstCod,&ThrCod) < 2)
     {
      For_RemoveThreadOnly (ThrCod);
      ThreadDeleted = true;
     }

   /***** Delete the post from the table of disabled forum posts *****/
   For_DB_RemovePstFromDisabled (PstCod);

   /***** Delete post from forum post table *****/
   For_DB_RemovePst (PstCod);

   /***** Update the last post of the thread *****/
   if (!ThreadDeleted)
      For_DB_UpdateThrLastPst (ThrCod,For_DB_GetThrLastPst (ThrCod));

   return ThreadDeleted;
  }

/*****************************************************************************/
/*********** Get the number of posts in the thread than holds a post *********/
/*****************************************************************************/

static unsigned For_NumPstsInThrWithPstCod (long PstCod,long *ThrCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumPsts;

   /***** Initialize default values *****/
   *ThrCod = -1L;
   NumPsts = 0;

   /***** Trivial check: PstCod should be > 0 *****/
   if (PstCod <= 0)
      return NumPsts;

   /***** Get number of posts in the thread that holds a post from database *****/
   if (For_DB_GetThreadAndNumPostsGivenPstCod (&mysql_res,PstCod) == 1)	// Result should have one row
     {
      row = mysql_fetch_row (mysql_res);
      /*
      row[0]: ThrCod
      row[1]: COUNT(PstCod)
      */
      if (sscanf (row[0],"%ld",ThrCod) != 1)
	 Err_ShowErrorAndExit ("Error when getting number of posts in a thread.");
      if (sscanf (row[1],"%u",&NumPsts) != 1)
	 Err_ShowErrorAndExit ("Error when getting number of posts in a thread.");
     }
   DB_FreeMySQLResult (&mysql_res);

   return NumPsts;
  }

/*****************************************************************************/
/*************** Delete a thread from the forum thread table *****************/
/*****************************************************************************/

static void For_RemoveThreadOnly (long ThrCod)
  {
   /***** Indicate that this thread has not been read by anyone *****/
   For_DB_RemoveThrFromReadThrs (ThrCod);

   /***** Remove thread code from thread clipboard *****/
   For_DB_RemoveThrFromClipboard (ThrCod);

   /***** Delete thread from forum thread table *****/
   For_DB_RemoveThread (ThrCod);
  }

/*****************************************************************************/
/*************** Delete a thread from the forum thread table *****************/
/*****************************************************************************/

static void For_RemoveThreadAndItsPsts (long ThrCod)
  {
   /***** Delete disabled posts in thread *****/
   For_DB_RemoveDisabledPstsInThread (ThrCod);

   /***** Delete thread posts *****/
   For_DB_RemoveThreadPsts (ThrCod);

   /***** Delete thread from forum thread table *****/
   For_RemoveThreadOnly (ThrCod);
  }

/*****************************************************************************/
/*********** Get thread, forum type and hierarchy code of a post *************/
/*****************************************************************************/
// Return thread to which the post belongs

long For_GetThreadForumTypeAndHieCodOfAPost (long PstCod,struct For_Forum *Forum)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;
   long ThrCod;

   /***** Set defaults *****/
   Forum->Type = For_FORUM_UNKNOWN;
   Forum->HieCod = -1L;
   ThrCod = -1L;

   /***** Check if there is a row with forum type *****/
   if (For_DB_GetThreadForumTypeAndHieCodOfAPost (&mysql_res,PstCod))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get forum thread (row[0]) */
      if (sscanf (row[0],"%ld",&ThrCod) != 1)
	 Err_WrongThreadExit ();

      /* Get forum type (row[1]) */
      if (sscanf (row[1],"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < For_NUM_TYPES_FORUM)
	    Forum->Type = (For_ForumType_t) UnsignedNum;

      /* Get forum hierarchy code (row[2]) */
      if (sscanf (row[2],"%ld",&Forum->HieCod) != 1)
	 Err_WrongForumExit ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return ThrCod;
  }

/*****************************************************************************/
/*************** Get forum type and hierarchy code of a post *****************/
/*****************************************************************************/

void For_GetForumTypeAndHieCodOfAThread (long ThrCod,struct For_Forum *Forum)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;

   /***** Set defaults *****/
   Forum->Type = For_FORUM_UNKNOWN;
   Forum->HieCod = -1L;

   /***** Check if there is a row with forum type *****/
   if (For_DB_GetForumTypeAndHieCodOfAThread (&mysql_res,ThrCod))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get forum type (row[0]) */
      if (sscanf (row[0],"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < For_NUM_TYPES_FORUM)
	    Forum->Type = (For_ForumType_t) UnsignedNum;

      /* Get forum hierarchy code (row[1]) */
      if (sscanf (row[1],"%ld",&Forum->HieCod) != 1)
	 Err_WrongForumExit ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Get thread read time for the current user ****************/
/*****************************************************************************/

static time_t For_GetThrReadTime (long ThrCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   time_t ReadTimeUTC;

   /***** Get read time of a thread from database *****/
   if (For_DB_GetThrReadTime (&mysql_res,ThrCod))
     {
      /***** There is a row ==> get read time *****/
      row = mysql_fetch_row (mysql_res);

      ReadTimeUTC = Dat_GetUNIXTimeFromStr (row[0]);
     }
   else
      ReadTimeUTC = (time_t) 0;	// If there is no row for this thread and current user,
				// then current user has not read this thread

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return ReadTimeUTC;
  }

/*****************************************************************************/
/************************ Show posts in a thread *****************************/
/*****************************************************************************/

void For_ShowPostsOfAThread (struct For_Forums *Forums,
			     Ale_AlertType_t AlertType,const char *Message)
  {
   extern const char *Hlp_COMMUNICATION_Forums_posts;
   extern const char *Txt_Thread;
   struct For_Thread Thread;
   char LastSubject[Cns_MAX_BYTES_SUBJECT + 1];
   char FrameTitle[128 + Cns_MAX_BYTES_SUBJECT];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRow;
   unsigned NumPst = 0;		// Initialized to avoid warning
   unsigned NumPsts;
   time_t ReadTimeUTC;		// Read time of thread for the current user
   time_t CreatTimeUTC;		// Creation time of post
   struct Pag_Pagination PaginationPsts;
   bool NewPst = false;
   bool ICanModerateForum = false;

   /***** Get data of the thread *****/
   Thread.ThrCod =
   Forums->Thread.Current =
   Forums->Thread.Selected;
   For_GetThreadData (&Thread);

   /***** Get if there is a thread ready to be moved *****/
   if (For_CheckIfICanMoveThreads ())
      Forums->Thread.ToMove = For_DB_GetThrInMyClipboard ();

   /***** Get thread read time for the current user *****/
   ReadTimeUTC = For_GetThrReadTime (Thread.ThrCod);

   /***** Show alert after action *****/
   HTM_SECTION_Begin (For_FORUM_POSTS_SECTION_ID);

      Ale_ShowAlerts (For_FORUM_POSTS_SECTION_ID);	// Possible pending alerts
      if (Message)
	 if (Message[0])
	    Ale_ShowAlert (AlertType,Message);

      /***** Begin box *****/
      snprintf (FrameTitle,sizeof (FrameTitle),"%s: %s",
		Txt_Thread,Thread.Subject);
      Box_BoxBegin (FrameTitle,For_PutIconsOneThread,Forums,
		    Hlp_COMMUNICATION_Forums_posts,Box_NOT_CLOSABLE);

	 /***** Get posts of a thread from database *****/
	 LastSubject[0] = '\0';
	 if ((NumPsts = For_DB_GetPostsOfAThread (&mysql_res,Thread.ThrCod)))		// If there are posts...
	   {
	    /***** Check if I can moderate posts in forum *****/
	    switch (Forums->Forum.Type)
	      {
	       case For_FORUM_GLOBAL_USRS:
	       case For_FORUM_GLOBAL_TCHS:
	       case For_FORUM__SWAD__USRS:
	       case For_FORUM__SWAD__TCHS:
		  ICanModerateForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);
		  break;
	       case For_FORUM_INSTIT_USRS:
	       case For_FORUM_INSTIT_TCHS:
		  ICanModerateForum = (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM);
		  break;
	       case For_FORUM_CENTER_USRS:
	       case For_FORUM_CENTER_TCHS:
		  ICanModerateForum = (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM);
		  break;
	       case For_FORUM_DEGREE_USRS:
	       case For_FORUM_DEGREE_TCHS:
	       case For_FORUM_COURSE_TCHS:
		  ICanModerateForum = (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM);
		  break;
	       case For_FORUM_COURSE_USRS:
		  ICanModerateForum = (Gbl.Usrs.Me.Role.Logged >= Rol_TCH);
		  break;
	       default:
		  ICanModerateForum = false;
		  break;
	      }

	    /***** Compute variables related to pagination *****/
	    PaginationPsts.NumItems = NumPsts;
	    PaginationPsts.CurrentPage = (int) Forums->CurrentPagePsts;
	    Pag_CalculatePagination (&PaginationPsts);
	    PaginationPsts.Anchor = For_FORUM_POSTS_SECTION_ID;
	    Forums->CurrentPagePsts = (unsigned) PaginationPsts.CurrentPage;

	    /***** Write links to pages *****/
	    Pag_WriteLinksToPagesCentered (Pag_POSTS_FORUM,&PaginationPsts,
					   Forums,Thread.ThrCod);

	    /***** Begin table *****/
	    HTM_TABLE_BeginWidePadding (2);

	       /***** Show posts from this page, the author and the date of last reply *****/
	       mysql_data_seek (mysql_res,(my_ulonglong) (PaginationPsts.FirstItemVisible - 1));
	       for (NumRow  = PaginationPsts.FirstItemVisible;
		    NumRow <= PaginationPsts.LastItemVisible;
		    NumRow++)
		 {
		  row = mysql_fetch_row (mysql_res);

		  if (sscanf (row[0],"%ld",&Forums->PstCod) != 1)
		     Err_WrongPostExit ();

		  CreatTimeUTC = Dat_GetUNIXTimeFromStr (row[1]);

		  NumPst = NumRow;
		  NewPst = (CreatTimeUTC > ReadTimeUTC);

		  if (NewPst && NumRow == PaginationPsts.LastItemVisible)
		     /* Update for_read table indicating that this thread page and previous ones
			have been read and have no new posts for the current user
			(even if any previous pages have been no read actually).
			Note that database is not updated with the current time,
			but with the creation time of the most recent post
			in this page of threads. */
		     For_DB_UpdateThrReadTime (Thread.ThrCod,CreatTimeUTC);

		  /* Show post */
		  For_ShowAForumPost (Forums,NumPst,
				      (NumRow == NumPsts),LastSubject,
				      NewPst,ICanModerateForum);

		  /* Mark possible notification as seen */
		  switch (Forums->Forum.Type)
		    {
		     case For_FORUM_COURSE_TCHS:
		     case For_FORUM_COURSE_USRS:
			Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_FORUM_POST_COURSE,
			                                Forums->PstCod);
			break;
		     default:
			break;
		    }
		  if (Thread.NumMyPosts)
		     Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_FORUM_REPLY,
		                                     Forums->PstCod);
		 }

	    /***** End table *****/
	    HTM_TABLE_End ();

	    /***** Write again links to pages *****/
	    Pag_WriteLinksToPagesCentered (Pag_POSTS_FORUM,&PaginationPsts,
					   Forums,Thread.ThrCod);
	   }

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);

	 /***** Form to write a new post in the thread *****/
	 HTM_SECTION_Begin (For_NEW_POST_SECTION_ID);
	    For_WriteFormForumPst (Forums,true,LastSubject);
	 HTM_SECTION_End ();

      /***** End box *****/
      Box_BoxEnd ();

   HTM_SECTION_End ();
  }

/*****************************************************************************/
/*********************** Put icon to write a new post ************************/
/*****************************************************************************/

static void For_PutIconsOneThread (void *Forums)
  {
   if (Forums)
     {
      /***** Put icon to write a new post *****/
      Ico_PutContextualIconToAdd (For_ActionsSeePstFor[((struct For_Forums *) Forums)->Forum.Type],
				  For_NEW_POST_SECTION_ID,
				  For_PutParsNewPost,Forums);

      /***** Put icon to get resource link *****/
      if (((struct For_Forums *) Forums)->Forum.Type == For_FORUM_COURSE_USRS &&
          Rsc_CheckIfICanGetLink ())
	 Ico_PutContextualIconToGetLink (ActReqLnkForCrsUsr,NULL,
					 For_PutParsNewPost,Forums);
     }
  }

static void For_PutParsNewPost (void *Forums)
  {
   if (Forums)
      For_PutAllParsForum (((struct For_Forums *) Forums)->CurrentPageThrs,	// Page of threads = current
			   UINT_MAX,						// Page of posts   = last
			   ((struct For_Forums *) Forums)->ForumSet,
			   ((struct For_Forums *) Forums)->ThreadsOrder,
			   ((struct For_Forums *) Forums)->Forum.HieCod,
			   ((struct For_Forums *) Forums)->Thread.Current,
			   -1L);
  }

/*****************************************************************************/
/**************************** Show a post from forum *************************/
/*****************************************************************************/

static void For_ShowAForumPost (struct For_Forums *Forums,
	                        unsigned PstNum,
                                bool LastPst,char LastSubject[Cns_MAX_BYTES_SUBJECT + 1],
                                bool NewPst,bool ICanModerateForum)
  {
   extern const char *Txt_MSG_New;
   extern const char *Txt_MSG_Open;
   extern const char *Txt_no_subject;
   extern const char *Txt_FORUM_Post_X_allowed;
   extern const char *Txt_FORUM_Post_banned;
   extern const char *Txt_FORUM_Post_X_banned;
   extern const char *Txt_This_post_has_been_banned_probably_for_not_satisfy_the_rules_of_the_forums;
   static const char *Icon[Cns_NUM_DISABLED_ENABLED] =
     {
      [Cns_DISABLED] = "eye-slash.svg",
      [Cns_ENABLED ] = "eye.svg",
     };
   static Ico_Color_t Color[Cns_NUM_DISABLED_ENABLED] =
     {
      [Cns_DISABLED] = Ico_RED,
      [Cns_ENABLED ] = Ico_GREEN,
     };
   static const char **TxtAllowedBanned[Cns_NUM_DISABLED_ENABLED] =
     {
      [Cns_DISABLED] = &Txt_FORUM_Post_X_banned,
      [Cns_ENABLED ] = &Txt_FORUM_Post_X_allowed,
     };
   struct Usr_Data UsrDat;
   time_t CreatTimeUTC;	// Creation time of a post
   char OriginalContent[Cns_MAX_BYTES_LONG_TEXT + 1];
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Content[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Med_Media Media;
   Cns_DisabledOrEnabled_t DisabledOrEnabled;
   char *Title;
   Act_Action_t NextAction;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Initialize image *****/
   Med_MediaConstructor (&Media);

   /***** Check if post is enabled *****/
   DisabledOrEnabled = For_DB_GetIfPstIsDisabledOrEnabled (Forums->PstCod);

   /***** Get data of post *****/
   For_GetPstData (Forums->PstCod,&UsrDat.UsrCod,&CreatTimeUTC,
                   Subject,OriginalContent,&Media);

   if (DisabledOrEnabled == Cns_ENABLED)
      /* Return this subject as last subject */
      Str_Copy (LastSubject,Subject,Cns_MAX_BYTES_SUBJECT);

   HTM_TR_Begin (NULL);

      /***** Put an icon with post status *****/
      HTM_TD_Begin ("class=\"CONTEXT_COL %s_%s\"",
		    NewPst ? "MSG_BG_NEW" :
			     "MSG_BG",
		    The_GetSuffix ());
	 Ico_PutIcon (NewPst ? "envelope.svg" :
			       "envelope-open-text.svg",
		      Ico_BLACK,
		      NewPst ? Txt_MSG_New :
			       Txt_MSG_Open,
		      "ICO16x16");
      HTM_TD_End ();

      /***** Write post number *****/
      Msg_WriteMsgNumber ((unsigned long) PstNum,NewPst);

      /***** Write date *****/
      Msg_WriteMsgDate (CreatTimeUTC,
                        NewPst ? "MSG_TIT_NEW" :
				 "MSG_TIT",
                        NewPst ? "MSG_BG_NEW" :
				 "MSG_BG");
      /***** Write subject *****/
      HTM_TD_Begin ("class=\"LT %s_%s %s_%s\"",
		    NewPst ? "MSG_TIT_NEW" :
			     "MSG_TIT",
	  	    The_GetSuffix (),
                    NewPst ? "MSG_BG_NEW" :
	  	             "MSG_BG",
	  	    The_GetSuffix ());
         switch (DisabledOrEnabled)
           {
            case Cns_DISABLED:
               HTM_TxtF ("[%s]",Txt_FORUM_Post_banned);
               break;
            case Cns_ENABLED:
	       if (Subject[0])
		  HTM_Txt (Subject);
	       else
		  HTM_TxtF ("[%s]",Txt_no_subject);
               break;
           }
      HTM_TD_End ();
   HTM_TR_End ();

   HTM_TR_Begin (NULL);

      /***** Form to ban/unban post *****/
      HTM_TD_Begin ("class=\"CONTEXT_COL\"");

	 if (ICanModerateForum)
	   {
	    NextAction = DisabledOrEnabled == Cns_ENABLED ? For_ActionsDisPstFor[Forums->Forum.Type] :
					                    For_ActionsEnbPstFor[Forums->Forum.Type];
	    Frm_BeginFormAnchor (NextAction,For_FORUM_POSTS_SECTION_ID);
	       For_PutParsForum (Forums);
	       Ico_PutIconLink (Icon[DisabledOrEnabled],Color[DisabledOrEnabled],NextAction);
	    Frm_EndForm ();
	   }
	 else
	   {
	    if (asprintf (&Title,*TxtAllowedBanned[DisabledOrEnabled],PstNum) < 0)
	       Err_NotEnoughMemoryExit ();
	    Ico_PutIcon (Icon[DisabledOrEnabled],Color[DisabledOrEnabled],Title,
			 "ICO_HIDDEN ICO16x16");
	    free (Title);
	   }

	 /***** Form to remove post *****/
	 if (LastPst)
	    if (Usr_ItsMe (UsrDat.UsrCod) == Usr_ME)
	       // Post can be removed if post is the last (without answers) and it's mine
	       Ico_PutContextualIconToRemove (For_ActionsDelPstFor[Forums->Forum.Type],
					      PstNum == 1 ? For_FORUM_THREADS_SECTION_ID : 	// First and unique post in thread
							    For_FORUM_POSTS_SECTION_ID,	// Last of several posts in thread
					      For_PutParsForum,Forums);

      HTM_TD_End ();

      /***** Write author *****/
      HTM_TD_Begin ("colspan=\"2\" class=\"LT MSG_AUT_%s\" style=\"width:150px;\"",
                    The_GetSuffix ());
	 Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
						  Usr_DONT_GET_PREFS,
						  Usr_DONT_GET_ROLE_IN_CRS);
         Usr_WriteAuthor (&UsrDat,DisabledOrEnabled);
	 if (DisabledOrEnabled == Cns_ENABLED)
	    /* Write number of posts from this user */
	    For_WriteNumberOfPosts (Forums,UsrDat.UsrCod);
      HTM_TD_End ();

      /***** Write post content *****/
      HTM_TD_Begin ("class=\"LT MSG_TXT_%s\"",The_GetSuffix ());
         switch (DisabledOrEnabled)
           {
            case Cns_DISABLED:
	       HTM_Txt (Txt_This_post_has_been_banned_probably_for_not_satisfy_the_rules_of_the_forums);
               break;
            case Cns_ENABLED:
	       Str_Copy (Content,OriginalContent,sizeof (Content) - 1);
	       Msg_WriteMsgContent (Content,true,false);
   	       Med_ShowMedia (&Media,"FOR_IMG_CONT","FOR_IMG");
               break;
           }
      HTM_TD_End ();
   HTM_TR_End ();

   /***** Free image *****/
   Med_MediaDestructor (&Media);

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/*************************** Get data of a forum post ************************/
/*****************************************************************************/

static void For_GetPstData (long PstCod,long *UsrCod,time_t *CreatTimeUTC,
                            char Subject[Cns_MAX_BYTES_SUBJECT   + 1],
                            char Content[Cns_MAX_BYTES_LONG_TEXT + 1],
                            struct Med_Media *Media)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get data of a post from database *****/
   if (For_DB_GetPstData (&mysql_res,PstCod) != 1)
      Err_WrongPostExit ();

   /***** Get number of rows *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get author code (row[1]) *****/
   *UsrCod = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get creation time (row[1]) *****/
   *CreatTimeUTC = Dat_GetUNIXTimeFromStr (row[1]);

   /***** Get subject (row[2]) and content (row[3]) *****/
   Str_Copy (Subject,row[2],Cns_MAX_BYTES_SUBJECT  );
   Str_Copy (Content,row[3],Cns_MAX_BYTES_LONG_TEXT);

   /***** Get media (row[4]) *****/
   Media->MedCod = Str_ConvertStrCodToLongCod (row[4]);
   Med_GetMediaDataByCod (Media);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Get summary and content for a forum post ******************/
/*****************************************************************************/

void For_GetSummaryAndContentForumPst (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                       char **ContentStr,
                                       long PstCod,Ntf_GetContent_t GetContent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   size_t Length;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Get post subject and content from database *****/
   if (For_DB_GetPstSubjectAndContent (&mysql_res,PstCod) == 1)
     {
      /***** Get subject and content of the post *****/
      row = mysql_fetch_row (mysql_res);

      /***** Copy subject *****/
      Str_Copy (SummaryStr,row[0],Ntf_MAX_BYTES_SUMMARY);

      /***** Copy content *****/
      if (GetContent == Ntf_GET_CONTENT)
	{
	 Length = strlen (row[1]);

	 if ((*ContentStr = malloc (Length + 1)) == NULL)
            Err_NotEnoughMemoryExit ();

	 if (Length)
	    Str_Copy (*ContentStr,row[1],Length);
	 else
	    **ContentStr = '\0';
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************** Write number of posts in a forum of an user *****************/
/*****************************************************************************/

static void For_WriteNumberOfPosts (const struct For_Forums *Forums,long UsrCod)
  {
   extern const char *Txt_FORUM_post;
   extern const char *Txt_FORUM_posts;
   unsigned NumPsts;

   /***** Get number of posts from database *****/
   NumPsts = For_DB_GetNumPstsOfUsrInForum (&Forums->Forum,UsrCod);

   /***** Write number of posts *****/
   HTM_DIV_Begin ("class=\"LT MSG_AUT_%s\"",The_GetSuffix ());
      HTM_TxtF ("[%u %s]",NumPsts,NumPsts == 1 ? Txt_FORUM_post :
						 Txt_FORUM_posts);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************** Put all hidden parameters related to forums ******************/
/*****************************************************************************/

static void For_PutParsForum (void *Forums)
  {
   if (Forums)
      For_PutAllParsForum (((struct For_Forums *) Forums)->CurrentPageThrs,	// Page of threads = current
                           ((struct For_Forums *) Forums)->CurrentPagePsts,	// Page of posts   = current
                           ((struct For_Forums *) Forums)->ForumSet,
			   ((struct For_Forums *) Forums)->ThreadsOrder,
			   ((struct For_Forums *) Forums)->Forum.HieCod,
			   ((struct For_Forums *) Forums)->Thread.Current,
			   ((struct For_Forums *) Forums)->PstCod);
  }

void For_PutAllParsForum (unsigned NumPageThreads,
                          unsigned NumPagePosts,
                          For_ForumSet_t ForumSet,
                          Dat_StartEndTime_t Order,
                          long HieCod,
                          long ThrCod,
                          long PstCod)
  {
   Pag_PutParPagNum (Pag_THREADS_FORUM,NumPageThreads);
   Pag_PutParPagNum (Pag_POSTS_FORUM,NumPagePosts);
   For_PutParForumSet (ForumSet);
   Par_PutParOrder ((unsigned) Order);
   ParCod_PutPar (ParCod_OthHie,HieCod);
   ParCod_PutPar (ParCod_Thr,ThrCod);
   ParCod_PutPar (ParCod_Pst,PstCod);
  }

/*****************************************************************************/
/********* Put a hidden parameter with set of forums I want to see ***********/
/*****************************************************************************/

static void For_PutParForumSet (For_ForumSet_t ForumSet)
  {
   Par_PutParUnsigned (NULL,"ForumSet",(unsigned) ForumSet);
  }

/*****************************************************************************/
/************************** Show list of available forums ********************/
/*****************************************************************************/

void For_ShowForumList (struct For_Forums *Forums)
  {
   extern const char *Hlp_COMMUNICATION_Forums;
   extern const char *Txt_Forums;
   Lay_LastItem_t IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS];
   MYSQL_RES *mysql_resCtr;
   MYSQL_RES *mysql_resDeg;
   MYSQL_RES *mysql_resCrs;
   MYSQL_ROW row;
   long InsCod;
   long CtrCod;
   long DegCod;
   long CrsCod;
   unsigned NumMyIns;
   unsigned NumCtr;
   unsigned NumCtrs;
   unsigned NumDeg;
   unsigned NumDegs;
   unsigned NumCrs;
   unsigned NumCrss;
   bool ICanSeeInsForum;
   bool ICanSeeCtrForum;
   bool ICanSeeDegForum;

   /***** Get if there is a thread ready to be moved *****/
   if (For_CheckIfICanMoveThreads ())
      Forums->Thread.ToMove = For_DB_GetThrInMyClipboard ();

   /***** Fill the list with the institutions I belong to *****/
   Hie_GetMyHierarchy (Hie_INS);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Forums,For_PutIconsForums,NULL,
                 Hlp_COMMUNICATION_Forums,Box_NOT_CLOSABLE);

      /***** Put a form to select which forums *****/
      For_PutFormWhichForums (Forums);

      /***** Begin list *****/
      HTM_UL_Begin ("class=\"LIST_TREE\"");

	 /***** Links to global forums *****/
	 For_WriteLinksToGblForums (Forums,IsLastItemInLevel);
	 switch (Forums->ForumSet)
	   {
	    case For_ONLY_CURRENT_FORUMS:
	       if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)
		 {
		  if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
		     ICanSeeInsForum = true;
		  else
		     ICanSeeInsForum = Hie_CheckIfIBelongTo (Hie_INS,Gbl.Hierarchy.Node[Hie_INS].HieCod);
		 }
	       else
		  ICanSeeInsForum = false;

	       /***** Links to forums about the platform *****/
	       For_WriteLinksToPlatformForums (Forums,true,IsLastItemInLevel);

	       if (ICanSeeInsForum)
		 {
		  if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
		     ICanSeeCtrForum = true;
		  else
		     ICanSeeCtrForum = Hie_CheckIfIBelongTo (Hie_CTR,Gbl.Hierarchy.Node[Hie_CTR].HieCod);

		  /***** Links to forums of current institution *****/
		  if (For_WriteLinksToInsForums (Forums,Gbl.Hierarchy.Node[Hie_INS].HieCod,
						 true,
						 IsLastItemInLevel) > 0)
		     if (ICanSeeCtrForum)
		       {
			if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
			   ICanSeeDegForum = true;
			else
			   ICanSeeDegForum = Hie_CheckIfIBelongTo (Hie_DEG,Gbl.Hierarchy.Node[Hie_DEG].HieCod);

			/***** Links to forums of current center *****/
			if (For_WriteLinksToCtrForums (Forums,
						       Gbl.Hierarchy.Node[Hie_CTR].HieCod,
						       true,
						       IsLastItemInLevel) > 0)
			   if (ICanSeeDegForum)
			      /***** Links to forums of current degree *****/
			      if (For_WriteLinksToDegForums (Forums,
							     Gbl.Hierarchy.Node[Hie_DEG].HieCod,
							     true,
							     IsLastItemInLevel) > 0)
				 if (Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS] ||
				     Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
				    /***** Links to forums of current degree *****/
				    For_WriteLinksToCrsForums (Forums,
							       Gbl.Hierarchy.Node[Hie_CRS].HieCod,
							       true,
							       IsLastItemInLevel);
		       }
		 }
	       break;
	    case For_ALL_MY_FORUMS:
	       /***** Links to forums about the platform *****/
	       For_WriteLinksToPlatformForums (Forums,true,IsLastItemInLevel);

	       /***** Links to forums of users from my institutions, the degrees in each institution and the courses in each degree *****/
	       for (NumMyIns = 0;
		    NumMyIns < Gbl.Usrs.Me.Hierarchy[Hie_INS].Num;
		    NumMyIns++)
		 {
		  InsCod = Gbl.Usrs.Me.Hierarchy[Hie_INS].Nodes[NumMyIns].HieCod;

		  /* Links to forums of this institution */
		  For_WriteLinksToInsForums (Forums,
					     InsCod,
					     (NumMyIns == Gbl.Usrs.Me.Hierarchy[Hie_INS].Num - 1),
					     IsLastItemInLevel);

		  /* Get my centers in this institution from database */
		  NumCtrs = Ctr_DB_GetMyCtrs (&mysql_resCtr,InsCod);
		  for (NumCtr = 0;
		       NumCtr < NumCtrs;
		       NumCtr++)
		    {
		     /* Get next center */
		     row = mysql_fetch_row (mysql_resCtr);
		     CtrCod = Str_ConvertStrCodToLongCod (row[0]);

		     /* Links to forums of this center */
		     if (For_WriteLinksToCtrForums (Forums,
						    CtrCod,
						    (NumCtr == NumCtrs - 1),
						    IsLastItemInLevel) > 0)
		       {
			/* Get my degrees in this institution from database */
			NumDegs = Deg_DB_GetMyDegs (&mysql_resDeg,CtrCod);
			for (NumDeg = 0;
			     NumDeg < NumDegs;
			     NumDeg++)
			  {
			   /* Get next degree */
			   row = mysql_fetch_row (mysql_resDeg);
			   DegCod = Str_ConvertStrCodToLongCod (row[0]);

			   /* Links to forums of this degree */
			   if (For_WriteLinksToDegForums (Forums,
							  DegCod,
							  (NumDeg == NumDegs - 1),
							  IsLastItemInLevel) > 0)
			     {
			      /* Get my courses in this degree from database */
			      NumCrss = Crs_DB_GetCrssFromUsr (&mysql_resCrs,
							       Gbl.Usrs.Me.UsrDat.UsrCod,
							       DegCod);
			      for (NumCrs = 0;
				   NumCrs < NumCrss;
				   NumCrs++)
				{
				 /* Get next course */
				 row = mysql_fetch_row (mysql_resCrs);
				 CrsCod = Str_ConvertStrCodToLongCod (row[0]);

				 /* Links to forums of this course */
				 For_WriteLinksToCrsForums (Forums,
							    CrsCod,
							    (NumCrs == NumCrss - 1),
							    IsLastItemInLevel);
				}

			      /* Free structure that stores the query result */
			      DB_FreeMySQLResult (&mysql_resCrs);
			     }
			  }

			/* Free structure that stores the query result */
			DB_FreeMySQLResult (&mysql_resDeg);
		       }
		    }

		  /* Free structure that stores the query result */
		  DB_FreeMySQLResult (&mysql_resCtr);
		 }
	       break;
	    default:
	       break;
	   }

      /***** End list *****/
      HTM_UL_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************** Put contextual icons in forums ***********************/
/*****************************************************************************/

static void For_PutIconsForums (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_FORUMS);
  }

/*****************************************************************************/
/*************** Put form to select which forums I want to see ***************/
/*****************************************************************************/

static void For_PutFormWhichForums (const struct For_Forums *Forums)
  {
   extern const char *Txt_FORUM_WHICH_FORUM[For_NUM_FORUM_SETS];
   For_ForumSet_t ForumSet;

   /***** Form to select which forums I want to see:
          - all my forums
          - only the forums of current institution/degree/course *****/
   Frm_BeginForm (ActSeeFor);
      Par_PutParOrder ((unsigned) Forums->ThreadsOrder);
      HTM_DIV_Begin ("class=\"SEL_BELOW_TITLE DAT_%s\"",The_GetSuffix ());
	 HTM_UL_Begin (NULL);

	    for (ForumSet  = (For_ForumSet_t) 0;
		 ForumSet <= (For_ForumSet_t) (For_NUM_FORUM_SETS - 1);
		 ForumSet++)
	      {
	       HTM_LI_Begin (NULL);
		  HTM_LABEL_Begin (NULL);
		     HTM_INPUT_RADIO ("ForumSet",HTM_SUBMIT_ON_CLICK,
				      "value=\"%u\"%s",
				      (unsigned) ForumSet,
				      (ForumSet == Forums->ForumSet) ? " checked=\"checked\"" :
								       "");
		     HTM_Txt (Txt_FORUM_WHICH_FORUM[ForumSet]);
		  HTM_LABEL_End ();
	       HTM_LI_End ();
	      }

	 HTM_UL_End ();
      HTM_DIV_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************* Write links to global forums **********************/
/*****************************************************************************/

static void For_WriteLinksToGblForums (const struct For_Forums *Forums,
                                       Lay_LastItem_t IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS])
  {
   Lay_Highlight_t Highlight;
   bool ICanSeeTeacherForum;
   struct For_Forum Forum;

   /***** Can I see teachers's forums? *****/
   Rol_GetRolesInAllCrss (&Gbl.Usrs.Me.UsrDat);
   ICanSeeTeacherForum = Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
	                 (Gbl.Usrs.Me.UsrDat.Roles.InCrss & ((1 << Rol_NET) |
	                                                     (1 << Rol_TCH)));

   /***** Link to forum global *****/
   Forum.Type = For_FORUM_GLOBAL_USRS;
   Forum.HieCod = -1L;
   Highlight = (Forums->Forum.Type == For_FORUM_GLOBAL_USRS) ? Lay_HIGHLIGHT :
							       Lay_NO_HIGHLIGHT;
   IsLastItemInLevel[1] = Lay_NO_LAST_ITEM;
   For_WriteLinkToForum (Forums,&Forum,Highlight,1,IsLastItemInLevel);

   /***** Link to forum of teachers global *****/
   Rol_GetRolesInAllCrss (&Gbl.Usrs.Me.UsrDat);
   if (ICanSeeTeacherForum)
     {
      Forum.Type = For_FORUM_GLOBAL_TCHS;
      Forum.HieCod = -1L;
      Highlight = (Forums->Forum.Type == For_FORUM_GLOBAL_TCHS) ? Lay_HIGHLIGHT :
								  Lay_NO_HIGHLIGHT;
      IsLastItemInLevel[1] = Lay_NO_LAST_ITEM;
      For_WriteLinkToForum (Forums,&Forum,Highlight,1,IsLastItemInLevel);
     }
  }

/*****************************************************************************/
/****************** Write links to forums about the platform *****************/
/*****************************************************************************/

static void For_WriteLinksToPlatformForums (const struct For_Forums *Forums,
					    bool IsLastForum,
                                            Lay_LastItem_t IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS])
  {
   Lay_Highlight_t Highlight;
   bool ICanSeeTeacherForum;
   struct For_Forum Forum;

   /***** Can I see teachers's forums? *****/
   Rol_GetRolesInAllCrss (&Gbl.Usrs.Me.UsrDat);
   ICanSeeTeacherForum = Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
	                 (Gbl.Usrs.Me.UsrDat.Roles.InCrss & ((1 << Rol_NET) |
	                                                     (1 << Rol_TCH)));

   /***** Link to forum of users about the platform *****/
   Forum.Type = For_FORUM__SWAD__USRS;
   Forum.HieCod = -1L;
   Highlight = (Forums->Forum.Type == For_FORUM__SWAD__USRS) ? Lay_HIGHLIGHT :
							       Lay_NO_HIGHLIGHT;
   IsLastItemInLevel[1] = (IsLastForum && !ICanSeeTeacherForum) ? Lay_LAST_ITEM :
								  Lay_NO_LAST_ITEM;
   For_WriteLinkToForum (Forums,&Forum,Highlight,1,IsLastItemInLevel);

   /***** Link to forum of teachers about the platform *****/
   if (ICanSeeTeacherForum)
     {
      Forum.Type = For_FORUM__SWAD__TCHS;
      Forum.HieCod = -1L;
      Highlight = (Forums->Forum.Type == For_FORUM__SWAD__TCHS) ? Lay_HIGHLIGHT :
								  Lay_NO_HIGHLIGHT;
      IsLastItemInLevel[1] = IsLastForum ? Lay_LAST_ITEM :
					   Lay_NO_LAST_ITEM;
      For_WriteLinkToForum (Forums,&Forum,Highlight,1,IsLastItemInLevel);
     }
  }

/*****************************************************************************/
/********************** Write links to institution forums ********************/
/*****************************************************************************/
// Returns institution code

static long For_WriteLinksToInsForums (const struct For_Forums *Forums,
				       long InsCod,bool IsLastIns,
                                       Lay_LastItem_t IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS])
  {
   Lay_Highlight_t Highlight;
   Rol_Role_t MaxRoleInIns;
   bool ICanSeeTeacherForum;
   struct For_Forum Forum;

   if (InsCod > 0)
     {
      MaxRoleInIns = Rol_GetMyMaxRoleIn (Hie_INS,InsCod);
      ICanSeeTeacherForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
	                     MaxRoleInIns == Rol_NET ||
	                     MaxRoleInIns == Rol_TCH);

      /***** Link to the forum of users from this institution *****/
      Forum.Type = For_FORUM_INSTIT_USRS;
      Forum.HieCod = InsCod;
      Highlight = (Forums->Forum.Type == For_FORUM_INSTIT_USRS &&
	           Forums->Forum.HieCod == InsCod) ? Lay_HIGHLIGHT :
						     Lay_NO_HIGHLIGHT;
      IsLastItemInLevel[2] = (IsLastIns && !ICanSeeTeacherForum) ? Lay_LAST_ITEM :
								   Lay_NO_LAST_ITEM;
      For_WriteLinkToForum (Forums,&Forum,Highlight,2,IsLastItemInLevel);

      /***** Link to forum of teachers from this institution *****/
      if (ICanSeeTeacherForum)
        {
	 Forum.Type = For_FORUM_INSTIT_TCHS;
	 Forum.HieCod = InsCod;
	 Highlight = (Forums->Forum.Type == For_FORUM_INSTIT_TCHS &&
		      Forums->Forum.HieCod == InsCod) ? Lay_HIGHLIGHT :
							Lay_NO_HIGHLIGHT;
         IsLastItemInLevel[2] = IsLastIns ? Lay_LAST_ITEM :
					    Lay_NO_LAST_ITEM;
         For_WriteLinkToForum (Forums,&Forum,Highlight,2,IsLastItemInLevel);
        }
     }
   return InsCod;
  }

/*****************************************************************************/
/************************ Write links to center forums ***********************/
/*****************************************************************************/
// Returns center code

static long For_WriteLinksToCtrForums (const struct For_Forums *Forums,
				       long CtrCod,bool IsLastCtr,
                                       Lay_LastItem_t IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS])
  {
   Lay_Highlight_t Highlight;
   Rol_Role_t MaxRoleInCtr;
   bool ICanSeeTeacherForum;
   struct For_Forum Forum;

   if (CtrCod > 0)
     {
      MaxRoleInCtr = Rol_GetMyMaxRoleIn (Hie_CTR,CtrCod);
      ICanSeeTeacherForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
	                     MaxRoleInCtr == Rol_NET ||
	                     MaxRoleInCtr == Rol_TCH);

      /***** Link to the forum of users from this center *****/
      Forum.Type = For_FORUM_CENTER_USRS;
      Forum.HieCod = CtrCod;
      Highlight = (Forums->Forum.Type == For_FORUM_CENTER_USRS &&
	           Forums->Forum.HieCod == CtrCod) ? Lay_HIGHLIGHT :
						     Lay_NO_HIGHLIGHT;
      IsLastItemInLevel[3] = (IsLastCtr && !ICanSeeTeacherForum) ? Lay_LAST_ITEM :
								   Lay_NO_LAST_ITEM;
      For_WriteLinkToForum (Forums,&Forum,Highlight,3,IsLastItemInLevel);

      /***** Link to forum of teachers from this center *****/
      if (ICanSeeTeacherForum)
        {
	 Forum.Type = For_FORUM_CENTER_TCHS;
	 Forum.HieCod = CtrCod;
	 Highlight = (Forums->Forum.Type == For_FORUM_CENTER_TCHS &&
		      Forums->Forum.HieCod == CtrCod) ? Lay_HIGHLIGHT :
							Lay_NO_HIGHLIGHT;
         IsLastItemInLevel[3] = IsLastCtr ? Lay_LAST_ITEM :
					    Lay_NO_LAST_ITEM;
         For_WriteLinkToForum (Forums,&Forum,Highlight,3,IsLastItemInLevel);
        }
     }
   return CtrCod;
  }

/*****************************************************************************/
/************************ Write links to degree forums ***********************/
/*****************************************************************************/
// Returns degree code

static long For_WriteLinksToDegForums (const struct For_Forums *Forums,
				       long DegCod,bool IsLastDeg,
                                       Lay_LastItem_t IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS])
  {
   Lay_Highlight_t Highlight;
   Rol_Role_t MaxRoleInDeg;
   bool ICanSeeTeacherForum;
   struct For_Forum Forum;

   if (DegCod > 0)
     {
      MaxRoleInDeg = Rol_GetMyMaxRoleIn (Hie_DEG,DegCod);
      ICanSeeTeacherForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
	                     MaxRoleInDeg == Rol_NET ||
	                     MaxRoleInDeg == Rol_TCH);

      /***** Link to the forum of users from this degree *****/
      Forum.Type = For_FORUM_DEGREE_USRS;
      Forum.HieCod = DegCod;
      Highlight = (Forums->Forum.Type == For_FORUM_DEGREE_USRS &&
	           Forums->Forum.HieCod == DegCod) ? Lay_HIGHLIGHT :
						     Lay_NO_HIGHLIGHT;
      IsLastItemInLevel[4] = (IsLastDeg && !ICanSeeTeacherForum) ? Lay_LAST_ITEM :
								   Lay_NO_LAST_ITEM;
      For_WriteLinkToForum (Forums,&Forum,Highlight,4,IsLastItemInLevel);

      /***** Link to forum of teachers from this degree *****/
      if (ICanSeeTeacherForum)
        {
	 Forum.Type = For_FORUM_DEGREE_TCHS;
	 Forum.HieCod = DegCod;
	 Highlight = (Forums->Forum.Type == For_FORUM_DEGREE_TCHS &&
		      Forums->Forum.HieCod == DegCod) ? Lay_HIGHLIGHT :
							Lay_NO_HIGHLIGHT;
	 IsLastItemInLevel[4] = IsLastDeg ? Lay_LAST_ITEM :
					    Lay_NO_LAST_ITEM;
         For_WriteLinkToForum (Forums,&Forum,Highlight,4,IsLastItemInLevel);
        }
     }
   return DegCod;
  }

/*****************************************************************************/
/************************ Write links to course forums ***********************/
/*****************************************************************************/
// Returns course code

static long For_WriteLinksToCrsForums (const struct For_Forums *Forums,
				       long CrsCod,bool IsLastCrs,
                                       Lay_LastItem_t IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS])
  {
   Lay_Highlight_t Highlight;
   Rol_Role_t MyRoleInCrs;
   bool ICanSeeTeacherForum;
   struct For_Forum Forum;

   if (CrsCod > 0)
     {
      MyRoleInCrs = Rol_GetMyRoleInCrs (CrsCod);
      ICanSeeTeacherForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
	                     MyRoleInCrs == Rol_NET ||
	                     MyRoleInCrs == Rol_TCH);

      /***** Link to the forum of users from this course *****/
      Forum.Type = For_FORUM_COURSE_USRS;
      Forum.HieCod = CrsCod;
      Highlight = (Forums->Forum.Type == For_FORUM_COURSE_USRS &&
	           Forums->Forum.HieCod == CrsCod) ? Lay_HIGHLIGHT :
						     Lay_NO_HIGHLIGHT;
      IsLastItemInLevel[5] = (IsLastCrs && !ICanSeeTeacherForum) ? Lay_LAST_ITEM :
								   Lay_NO_LAST_ITEM;
      For_WriteLinkToForum (Forums,&Forum,Highlight,5,IsLastItemInLevel);

      /***** Link to forum of teachers from this course *****/
      if (ICanSeeTeacherForum)
        {
	 Forum.Type = For_FORUM_COURSE_TCHS;
	 Forum.HieCod = CrsCod;
	 Highlight = (Forums->Forum.Type == For_FORUM_COURSE_TCHS &&
		      Forums->Forum.HieCod == CrsCod) ? Lay_HIGHLIGHT :
							Lay_NO_HIGHLIGHT;
         IsLastItemInLevel[5] = IsLastCrs ? Lay_LAST_ITEM :
					    Lay_NO_LAST_ITEM;
         For_WriteLinkToForum (Forums,&Forum,Highlight,5,IsLastItemInLevel);
        }
     }
   return CrsCod;
  }

/*****************************************************************************/
/********************** Write title and link to a forum **********************/
/*****************************************************************************/

static void For_WriteLinkToForum (const struct For_Forums *Forums,
	                          const struct For_Forum *Forum,
                                  Lay_Highlight_t Highlight,
                                  unsigned Level,
                                  Lay_LastItem_t IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS])
  {
   extern const char *Lay_HighlightClass[Lay_NUM_HIGHLIGHT];
   extern const char *Txt_Copy_not_allowed;
   unsigned NumThrs;
   unsigned NumThrsWithNewPosts;
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];
   Hie_Level_t Levels[For_NUM_TYPES_FORUM] =
     {
      [For_FORUM_GLOBAL_USRS] = Hie_SYS,
      [For_FORUM_GLOBAL_TCHS] = Hie_SYS,
      [For_FORUM__SWAD__USRS] = Hie_SYS,
      [For_FORUM__SWAD__TCHS] = Hie_SYS,
      [For_FORUM_INSTIT_USRS] = Hie_INS,
      [For_FORUM_INSTIT_TCHS] = Hie_INS,
      [For_FORUM_CENTER_USRS] = Hie_CTR,
      [For_FORUM_CENTER_TCHS] = Hie_CTR,
      [For_FORUM_DEGREE_USRS] = Hie_DEG,
      [For_FORUM_DEGREE_TCHS] = Hie_DEG,
      [For_FORUM_COURSE_USRS] = Hie_CRS,
      [For_FORUM_COURSE_TCHS] = Hie_CRS,
     };
   struct Hie_Node Node;

   /***** Get number of threads and number of posts *****/
   NumThrs = For_DB_GetNumThrsInForum (Forum);
   NumThrsWithNewPosts = For_GetNumThrsWithNewPstsInForum (Forum,NumThrs);

   /***** Begin row *****/
   HTM_LI_Begin (Lay_HighlightClass[Highlight]);

      /***** Indent forum title *****/
      Lay_IndentDependingOnLevel (Level,IsLastItemInLevel,
				  Lay_HORIZONTAL_LINE_AT_RIGHT);

      /***** Write paste button used to move a thread in clipboard to this forum *****/
      if (Forums->Thread.ToMove >= 0) // If I have permission to paste threads and there is a thread ready to be pasted...
	{
	 /* Check if thread to move is yet in current forum */
	 if (For_DB_CheckIfThrBelongsToForum (Forums->Thread.ToMove,Forum))
	    Ico_PutIcon ("paste.svg",Ico_BLACK,
			 Txt_Copy_not_allowed,"CONTEXT_OPT ICO_HIDDEN ICO16x16");
	 else
	   {
	    Frm_BeginFormAnchor (For_ActionsPasThrFor[Forum->Type],
				 For_FORUM_THREADS_SECTION_ID);
	       For_PutAllParsForum (1,	// Page of threads = first
				    1,	// Page of posts   = first
				    Forums->ForumSet,
				    Forums->ThreadsOrder,
				    Forum->HieCod,
				    Forums->Thread.ToMove,
				    -1L);
	       Ico_PutIconPaste (For_ActionsPasThrFor[Forum->Type]);
	    Frm_EndForm ();
	   }
	}

      /***** Write link to forum *****/
      Frm_BeginFormAnchor (For_ActionsSeeFor[Forum->Type],
			   For_FORUM_THREADS_SECTION_ID);
	 For_PutAllParsForum (1,	// Page of threads = first
			      1,	// Page of posts   = first
			      Forums->ForumSet,
			      Forums->ThreadsOrder,
			      Forum->HieCod,
			      -1L,
			      -1L);

	 HTM_BUTTON_Submit_Begin (Act_GetActionText (For_ActionsSeeFor[Forum->Type]),
				  NumThrsWithNewPosts ? "class=\"BT_LINK FORM_IN_%s BOLD\"" :
	                                                "class=\"BT_LINK FORM_IN_%s\"",
	                          The_GetSuffix ());

	    For_SetForumName (Forum,ForumName,Gbl.Prefs.Language,true);
	    switch (Forum->Type)
	      {
	       case For_FORUM_GLOBAL_USRS:
	       case For_FORUM_GLOBAL_TCHS:
		  Ico_PutIcon ("comments.svg",Ico_BLACK,ForumName,
			       "ICO16x16");
		  break;
	       case For_FORUM__SWAD__USRS:
	       case For_FORUM__SWAD__TCHS:
		  Ico_PutIcon ("swad64x64.png",Ico_UNCHANGED,ForumName,
			       "ICO16x16");
		  break;
	       case For_FORUM_INSTIT_USRS:
	       case For_FORUM_INSTIT_TCHS:
	       case For_FORUM_CENTER_USRS:
	       case For_FORUM_CENTER_TCHS:
	       case For_FORUM_DEGREE_USRS:
	       case For_FORUM_DEGREE_TCHS:
	       case For_FORUM_COURSE_USRS:
	       case For_FORUM_COURSE_TCHS:
		  Node.HieCod = Forum->HieCod;
		  Str_Copy (Node.ShrtName,ForumName,sizeof (Node.ShrtName) - 1);
		  Lgo_DrawLogo (Levels[Forum->Type],&Node,"ICO16x16");
		  break;
	       default:
		  break;
	      }
	    HTM_NBSPTxt (ForumName);

	    /***** Write total number of threads and posts in this forum *****/
	    if (NumThrs)
	       For_WriteNumberOfThrs (NumThrs);

	 /***** End row *****/
	 HTM_BUTTON_End ();

      Frm_EndForm ();

   HTM_LI_End ();

   /***** Put link to register students *****/
   if (Forum->Type == For_FORUM_COURSE_USRS)
       Enr_PutButtonInlineToRegisterStds (Forum->HieCod,
                                          5,IsLastItemInLevel,
					  Highlight);
  }

/*****************************************************************************/
/************************** Set the name of a forum **************************/
/*****************************************************************************/

void For_SetForumName (const struct For_Forum *Forum,
                       char ForumName[For_MAX_BYTES_FORUM_NAME + 1],
                       Lan_Language_t Language,bool UseHTMLEntities)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_General;
   extern const char *Txt_General_NO_HTML[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_only_teachers;
   extern const char *Txt_only_teachers_NO_HTML[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_Unknown_FORUM;
   struct Hie_Node Hie[Hie_NUM_LEVELS];

   switch (Forum->Type)
     {
      case For_FORUM_GLOBAL_USRS:
         Str_Copy (ForumName,UseHTMLEntities ? Txt_General :
                                               Txt_General_NO_HTML[Language],
                   For_MAX_BYTES_FORUM_NAME);
         break;
      case For_FORUM_GLOBAL_TCHS:
         snprintf (ForumName,For_MAX_BYTES_FORUM_NAME + 1,"%s%s",
                   UseHTMLEntities ? Txt_General :
                                     Txt_General_NO_HTML[Language],
                   UseHTMLEntities ? Txt_only_teachers :
                                     Txt_only_teachers_NO_HTML[Language]);
         break;
      case For_FORUM__SWAD__USRS:
         Str_Copy (ForumName,Cfg_PLATFORM_SHORT_NAME,For_MAX_BYTES_FORUM_NAME);
         break;
      case For_FORUM__SWAD__TCHS:
         snprintf (ForumName,For_MAX_BYTES_FORUM_NAME + 1,"%s%s",
                   Cfg_PLATFORM_SHORT_NAME,
                   UseHTMLEntities ? Txt_only_teachers :
                                     Txt_only_teachers_NO_HTML[Language]);
         break;
      case For_FORUM_INSTIT_USRS:
	 Hie[Hie_INS].HieCod = Forum->HieCod;
	 if (!Hie_GetDataByCod[Hie_INS] (&Hie[Hie_INS]))
	    Err_WrongInstitExit ();
         Str_Copy (ForumName,Hie[Hie_INS].ShrtName,For_MAX_BYTES_FORUM_NAME);
         break;
      case For_FORUM_INSTIT_TCHS:
	 Hie[Hie_INS].HieCod = Forum->HieCod;
	 if (!Hie_GetDataByCod[Hie_INS] (&Hie[Hie_INS]))
	    Err_WrongInstitExit ();
         snprintf (ForumName,For_MAX_BYTES_FORUM_NAME + 1,"%s%s",
		   Hie[Hie_INS].ShrtName,
                   UseHTMLEntities ? Txt_only_teachers :
                                     Txt_only_teachers_NO_HTML[Language]);
         break;
      case For_FORUM_CENTER_USRS:
	 Hie[Hie_CTR].HieCod = Forum->HieCod;
	 if (!Hie_GetDataByCod[Hie_CTR] (&Hie[Hie_CTR]))
	    Err_WrongCenterExit ();
         Str_Copy (ForumName,Hie[Hie_CTR].ShrtName,For_MAX_BYTES_FORUM_NAME);
         break;
      case For_FORUM_CENTER_TCHS:
	 Hie[Hie_CTR].HieCod = Forum->HieCod;
	 if (!Hie_GetDataByCod[Hie_CTR] (&Hie[Hie_CTR]))
	    Err_WrongCenterExit ();
         snprintf (ForumName,For_MAX_BYTES_FORUM_NAME + 1,"%s%s",
		   Hie[Hie_CTR].ShrtName,
                   UseHTMLEntities ? Txt_only_teachers :
                                     Txt_only_teachers_NO_HTML[Language]);
         break;
      case For_FORUM_DEGREE_USRS:
	 Hie[Hie_DEG].HieCod = Forum->HieCod;
	 if (!Hie_GetDataByCod[Hie_DEG] (&Hie[Hie_DEG]))
	    Err_WrongDegreeExit ();
         Str_Copy (ForumName,Hie[Hie_DEG].ShrtName,For_MAX_BYTES_FORUM_NAME);
         break;
      case For_FORUM_DEGREE_TCHS:
	 Hie[Hie_DEG].HieCod = Forum->HieCod;
	 if (!Hie_GetDataByCod[Hie_DEG] (&Hie[Hie_DEG]))
	    Err_WrongDegreeExit ();
         snprintf (ForumName,For_MAX_BYTES_FORUM_NAME + 1,"%s%s",
		   Hie[Hie_DEG].ShrtName,
                   UseHTMLEntities ? Txt_only_teachers :
                                     Txt_only_teachers_NO_HTML[Language]);
         break;
      case For_FORUM_COURSE_USRS:
	 Hie[Hie_CRS].HieCod = Forum->HieCod;
	 if (!Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]))
	    Err_WrongCourseExit ();
         Str_Copy (ForumName,Hie[Hie_CRS].ShrtName,For_MAX_BYTES_FORUM_NAME);
         break;
      case For_FORUM_COURSE_TCHS:
	 Hie[Hie_CRS].HieCod = Forum->HieCod;
	 if (!Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]))
	    Err_WrongCourseExit ();
         snprintf (ForumName,For_MAX_BYTES_FORUM_NAME + 1,"%s%s",
		   Hie[Hie_CRS].ShrtName,
                   UseHTMLEntities ? Txt_only_teachers :
                                     Txt_only_teachers_NO_HTML[Language]);
         break;
      default:
         Str_Copy (ForumName,Txt_Unknown_FORUM,For_MAX_BYTES_FORUM_NAME);
     }
  }

/*****************************************************************************/
/***** Get number of threads with new posts since my last read of a forum ****/
/*****************************************************************************/

static unsigned For_GetNumThrsWithNewPstsInForum (const struct For_Forum *Forum,
                                                  unsigned NumThreads)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumThrsWithNewPosts = NumThreads;	// By default, all threads are new to me

   /***** Get last time I read this forum from database *****/
   if (For_DB_GetLastTimeIReadForum (&mysql_res,Forum))
     {
      /***** Get number of threads with a last message modify time > newest read time (row[0]) *****/
      row = mysql_fetch_row (mysql_res);
      NumThrsWithNewPosts = For_DB_GetNumThrsInForumNewerThan (Forum,row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumThrsWithNewPosts;
  }

/*****************************************************************************/
/** Get number of unread posts in a thread since my last read of that thread */
/*****************************************************************************/

static unsigned For_GetNumOfUnreadPostsInThr (long ThrCod,unsigned NumPostsInThr)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUnreadPosts = NumPostsInThr;	// By default, all posts are unread by me

   /***** Get last time I read this thread from database *****/
   if (For_DB_GetLastTimeIReadThread (&mysql_res,ThrCod))
     {
      /***** Get the number of posts in thread with a modify time > newest read time for me (row[0]) *****/
      row = mysql_fetch_row (mysql_res);
      NumUnreadPosts = For_DB_GetNumPstsInThrNewerThan (ThrCod,row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumUnreadPosts;
  }

/*****************************************************************************/
/************** Get and write total number of threads and posts **************/
/*****************************************************************************/

static void For_WriteNumberOfThrs (unsigned NumThrs)
  {
   extern const char *Txt_thread;
   extern const char *Txt_threads;

   /***** Write number of threads and number of posts *****/
   HTM_TxtF (" [%u&nbsp;%s]",NumThrs,NumThrs == 1 ? Txt_thread :
			                            Txt_threads);
  }

/*****************************************************************************/
/********************** Show available threads of a forum ********************/
/*****************************************************************************/

void For_ShowForumTheads (void)
  {
   struct For_Forums Forums;

   /***** Reset forum *****/
   For_ResetForums (&Forums);

   /***** Get parameters related to forums *****/
   For_GetParsForums (&Forums);

   /***** Show forum list again *****/
   For_ShowForumList (&Forums);

   /***** Show forum threads with no one highlighted *****/
   For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_SUCCESS,NULL);
  }

/*****************************************************************************/
/********** Show available threads of a forum highlighting a thread **********/
/*****************************************************************************/

void For_ShowForumThreadsHighlightingOneThread (struct For_Forums *Forums,
                                                Ale_AlertType_t AlertType,const char *Message)
  {
   extern const char *Hlp_COMMUNICATION_Forums_threads;
   extern const char *Txt_Forum;
   extern const char *Txt_MSG_Subject;
   extern const char *Txt_FORUM_THREAD_HELP_ORDER[Dat_NUM_START_END_TIME];
   extern const char *Txt_FORUM_THREAD_ORDER[Dat_NUM_START_END_TIME];
   extern const char *Txt_Number_BR_msgs;
   extern const char *Txt_Unread_BR_msgs;
   extern const char *Txt_WriBRters;
   extern const char *Txt_ReaBRders;
   MYSQL_RES *mysql_res;
   char FrameTitle[128 + For_MAX_BYTES_FORUM_NAME];
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];
   unsigned NumThr;
   unsigned NumThrs;
   unsigned NumThrInScreen;	// From 0 to Pag_ITEMS_PER_PAGE-1
   Dat_StartEndTime_t Order;
   long ThrCods[Pag_ITEMS_PER_PAGE];
   struct Pag_Pagination PaginationThrs;

   /***** Set forum name *****/
   For_SetForumName (&Forums->Forum,
	             ForumName,Gbl.Prefs.Language,true);

   /***** Get threads of a forum from database *****/
   NumThrs = For_DB_GetForumThreads (&mysql_res,Forums);

   /***** Compute variables related to pagination of threads *****/
   PaginationThrs.NumItems = NumThrs;
   PaginationThrs.CurrentPage = (int) Forums->CurrentPageThrs;
   Pag_CalculatePagination (&PaginationThrs);
   PaginationThrs.Anchor = For_FORUM_THREADS_SECTION_ID;
   Forums->CurrentPageThrs = (unsigned) PaginationThrs.CurrentPage;

   /***** Fill the list of threads for current page *****/
   mysql_data_seek (mysql_res,(my_ulonglong) (PaginationThrs.FirstItemVisible - 1));
   for (NumThr  = PaginationThrs.FirstItemVisible, NumThrInScreen = 0;
        NumThr <= PaginationThrs.LastItemVisible;
        NumThr++, NumThrInScreen++)
      /* Get thread code(row[0]) */
      if ((ThrCods[NumThrInScreen] = DB_GetNextCode (mysql_res)) < 0)
         Err_ShowErrorAndExit ("Error when getting thread of a forum.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Show alert after action *****/
   HTM_SECTION_Begin (For_FORUM_THREADS_SECTION_ID);

      if (Message)
	 if (Message[0])
	    Ale_ShowAlert (AlertType,Message);

      /***** Begin box for threads of this forum *****/
      snprintf (FrameTitle,sizeof (FrameTitle),"%s: %s",Txt_Forum,ForumName);
      Box_BoxBegin (FrameTitle,For_PutIconsThreads,Forums,
		    Hlp_COMMUNICATION_Forums_threads,Box_NOT_CLOSABLE);

	 /***** List the threads *****/
	 if (NumThrs)
	   {
	    /***** Write links to all pages in the listing of threads *****/
	    Pag_WriteLinksToPagesCentered (Pag_THREADS_FORUM,&PaginationThrs,
					   Forums,-1L);

	    /***** Heading row *****/
	    HTM_TABLE_Begin ("TBL_SCROLL");
	       HTM_TR_Begin (NULL);

		  HTM_TH_Span (NULL      ,HTM_HEAD_CENTER,1,1,"BT");
		  HTM_TH_Span (NULL      ,HTM_HEAD_CENTER,1,1,"CONTEXT_COL");	// Column for contextual icons
                  HTM_TH (Txt_MSG_Subject,HTM_HEAD_LEFT  );

		  for (Order  = (Dat_StartEndTime_t) 0;
		       Order <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
		       Order++)
		    {
		     HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,2,NULL);

			Frm_BeginFormAnchor (For_ActionsSeeFor[Forums->Forum.Type],
					     For_FORUM_THREADS_SECTION_ID);
			   For_PutAllParsForum (Forums->CurrentPageThrs,	// Page of threads = current
					        1,				// Page of posts   = first
					        Forums->ForumSet,
					        Order,
				                Forums->Forum.HieCod,
					        -1L,
					        -1L);
			   HTM_BUTTON_Submit_Begin (Txt_FORUM_THREAD_HELP_ORDER[Order],
			                            "class=\"BT_LINK\"");
			      if (Order == Forums->ThreadsOrder)
				 HTM_U_Begin ();
			      HTM_Txt (Txt_FORUM_THREAD_ORDER[Order]);
			      if (Order == Forums->ThreadsOrder)
				 HTM_U_End ();
			   HTM_BUTTON_End ();
			Frm_EndForm ();

		     HTM_TH_End ();
		    }

	          HTM_TH (Txt_Number_BR_msgs,HTM_HEAD_RIGHT);
	          HTM_TH (Txt_Unread_BR_msgs,HTM_HEAD_RIGHT);
	          HTM_TH (Txt_WriBRters     ,HTM_HEAD_RIGHT);
	          HTM_TH (Txt_ReaBRders     ,HTM_HEAD_RIGHT);

	       HTM_TR_End ();

	       /***** List the threads *****/
	       For_ListForumThrs (Forums,ThrCods,Forums->Thread.Current,&PaginationThrs);

	    /***** End table *****/
	    HTM_TABLE_End ();

	    /***** Write links to all pages in the listing of threads *****/
	    Pag_WriteLinksToPagesCentered (Pag_THREADS_FORUM,&PaginationThrs,
					   Forums,-1L);
	   }

	 /***** Put a form to write the first post of a new thread *****/
	 HTM_SECTION_Begin (For_NEW_THREAD_SECTION_ID);
	    For_WriteFormForumPst (Forums,false,NULL);
	 HTM_SECTION_End ();

      /***** End box with threads of this forum ****/
      Box_BoxEnd ();

   HTM_SECTION_End ();
  }

/*****************************************************************************/
/***************** Put icons in list of threads of a forum *******************/
/*****************************************************************************/

static void For_PutIconsThreads (void *Forums)
  {
   if (Forums)
     {
      /***** Put icon to write a new thread *****/
      Ico_PutContextualIconToAdd (For_ActionsSeeFor[((struct For_Forums *) Forums)->Forum.Type],
				  For_NEW_THREAD_SECTION_ID,
				  For_PutParsNewThread,Forums);

      /***** Put icon to get resource link *****/
      if (((struct For_Forums *) Forums)->Forum.Type == For_FORUM_COURSE_USRS &&
          Rsc_CheckIfICanGetLink ())
	 Ico_PutContextualIconToGetLink (ActReqLnkForCrsUsr,NULL,
					 For_PutParsNewPost,Forums);
     }
  }

static void For_PutParsNewThread (void *Forums)
  {
   if (Forums)
      For_PutAllParsForum (1,	// Page of threads = first
			   1,	// Page of posts = first
			   ((struct For_Forums *)Forums)->ForumSet,
			   ((struct For_Forums *)Forums)->ThreadsOrder,
			   ((struct For_Forums *)Forums)->Forum.HieCod,
			   -1L,
			   -1L);
  }

/*****************************************************************************/
/************************ List the threads of a forum ************************/
/*****************************************************************************/

static void For_ListForumThrs (struct For_Forums *Forums,
	                       long ThrCods[Pag_ITEMS_PER_PAGE],
                               long ThrCodHighlighted,
                               struct Pag_Pagination *PaginationThrs)
  {
   extern const char *Txt_Thread_with_posts_from_you;
   extern const char *Txt_There_are_new_posts;
   extern const char *Txt_No_new_posts;
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC15x20",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE15x20",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO15x20",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR15x20",
     };
   unsigned NumThr;
   unsigned NumThrInScreen;	// From 0 to Pag_ITEMS_PER_PAGE-1
   struct For_Thread Thr;
   unsigned UniqueId;
   char *Id;
   struct Usr_Data UsrDat;
   Dat_StartEndTime_t Order;
   time_t TimeUTC;
   struct Pag_Pagination PaginationPsts;
   const char *Class;
   long ThreadInMyClipboard = -1L;
   unsigned Column;
   const char *BgColor;
   bool ICanMoveThreads;

   /***** Get if there is a thread ready to be moved *****/
   if ((ICanMoveThreads = For_CheckIfICanMoveThreads ()))
      ThreadInMyClipboard = For_DB_GetThrInMyClipboard ();

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** List threads *****/
   for (NumThr  = PaginationThrs->FirstItemVisible, NumThrInScreen = 0, UniqueId = 0, The_ResetRowColor ();
        NumThr <= PaginationThrs->LastItemVisible;
        NumThr++, NumThrInScreen++, The_ChangeRowColor ())
     {
      /***** Get the data of this thread *****/
      Thr.ThrCod = ThrCods[NumThrInScreen];
      For_GetThreadData (&Thr);
      Forums->Thread.Current = Thr.ThrCod;
      Class = (Thr.NumUnreadPosts ? "MSG_AUT_NEW" :
	                            "MSG_AUT");
      BgColor =  (Thr.ThrCod == ThreadInMyClipboard) ? "LIGHT_GREEN" :
	        ((Thr.ThrCod == ThrCodHighlighted)   ? "BG_HIGHLIGHT" :
                                                       The_GetColorRows ());

      HTM_TR_Begin (NULL);

         /***** Show my photo if I have any posts in this thread *****/
	 HTM_TD_Begin ("class=\"BT %s\"",BgColor);
	    if (Thr.NumMyPosts)
	       HTM_IMG (Gbl.Usrs.Me.PhotoURL[0] ? Gbl.Usrs.Me.PhotoURL :
						  Cfg_URL_ICON_PUBLIC,
			Gbl.Usrs.Me.PhotoURL[0] ? NULL :
						  "usr_bl.jpg",
			Txt_Thread_with_posts_from_you,
			"class=\"%s\"",ClassPhoto[Gbl.Prefs.PhotoShape]);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"CONTEXT_COL %s\"",BgColor);
	    /***** Put an icon with thread status *****/
	    if (Thr.NumUnreadPosts)
	       Ico_PutIcon ("envelope.svg",Ico_BLACK,
			    Txt_There_are_new_posts,"ICO16x16");
	    else
	       Ico_PutIcon ("envelope-open-text.svg",Ico_BLACK,
			    Txt_No_new_posts,"ICO16x16");

	    /***** Put button to remove the thread *****/
	    if (PermissionThreadDeletion[Forums->Forum.Type] &
		(1 << Gbl.Usrs.Me.Role.Logged)) // If I have permission to remove thread in this forum...
	      {
	       HTM_BR ();
	       Ico_PutContextualIconToRemove (For_ActionsReqDelThr[Forums->Forum.Type],For_REMOVE_THREAD_SECTION_ID,
					      For_PutParsForum,Forums);
	      }

	    /***** Put button to cut the thread for moving it to another forum *****/
	    if (ICanMoveThreads)
	      {
	       HTM_BR ();
	       Frm_BeginFormAnchor (For_ActionsCutThrFor[Forums->Forum.Type],
				    For_FORUM_THREADS_SECTION_ID);
		  For_PutAllParsForum (Forums->CurrentPageThrs,	// Page of threads = current
				       1,			// Page of posts   = first
				       Forums->ForumSet,
				       Forums->ThreadsOrder,
				       Forums->Forum.HieCod,
				       Thr.ThrCod,
				       -1L);
		  Ico_PutIconCut (For_ActionsCutThrFor[Forums->Forum.Type]);
	       Frm_EndForm ();
	      }

	 HTM_TD_End ();

	 /***** Write subject and links to thread pages *****/
	 HTM_TD_Begin ("class=\"LT %s\"",BgColor);
	    PaginationPsts.NumItems = Thr.NumPosts;
	    PaginationPsts.CurrentPage = 1;	// First page
	    Pag_CalculatePagination (&PaginationPsts);
	    PaginationPsts.Anchor = For_FORUM_POSTS_SECTION_ID;
	    Pag_WriteLinksToPages (Pag_POSTS_FORUM,
				   &PaginationPsts,
				   Forums,Thr.ThrCod,
				   Thr.Enabled[Dat_STR_TIME],
				   Thr.Subject,
				   Thr.NumUnreadPosts ? "BOLD PAG_TXT" :
							"PAG_TXT",
				   true);
	 HTM_TD_End ();

	 /***** Write the authors and date-times of first and last posts *****/
	 for (Order  = (Dat_StartEndTime_t) 0;
	      Order <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	      Order++)
	   {
	    if (Order == Dat_STR_TIME || Thr.NumPosts > 1)	// Don't write twice the same author when thread has only one thread
	      {
	       /* Write the author of first or last message */
	       UsrDat.UsrCod = Thr.UsrCod[Order];
	       Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							Usr_DONT_GET_PREFS,
							Usr_DONT_GET_ROLE_IN_CRS);
	       HTM_TD_Begin ("class=\"LT %s_%s %s\"",
	                     Class,The_GetSuffix (),BgColor);
		  Usr_WriteAuthor (&UsrDat,Thr.Enabled[Order]);
	       HTM_TD_End ();

	       /* Write the date of first or last message (it's in YYYYMMDDHHMMSS format) */
	       TimeUTC = Thr.WriteTime[Order];
	       UniqueId++;
	       if (asprintf (&Id,"thr_date_%u",UniqueId) < 0)
		  Err_NotEnoughMemoryExit ();
	       HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s %s\"",
	                     Id,Class,The_GetSuffix (),BgColor);
		  Dat_WriteLocalDateHMSFromUTC (Id,TimeUTC,
						Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
						true,true,false,0x6);
	       HTM_TD_End ();
	       free (Id);
	      }
	    else
	       for (Column  = 1;
		    Column <= 2;
		    Column++)
		 {
		  HTM_TD_Begin ("class=\"LT %s_%s %s\"",
		                Class,The_GetSuffix (),BgColor);
		  HTM_TD_End ();
		 }
	   }

	 /***** Write number of posts in this thread *****/
	 HTM_TD_Begin ("class=\"RT %s_%s %s\"",Class,The_GetSuffix (),BgColor);
	    HTM_TxtF ("%u&nbsp;",Thr.NumPosts);
	 HTM_TD_End ();

	 /***** Write number of new posts in this thread *****/
	 HTM_TD_Begin ("class=\"RT %s_%s %s\"",Class,The_GetSuffix (),BgColor);
	    HTM_TxtF ("%u&nbsp;",Thr.NumUnreadPosts);
	 HTM_TD_End ();

	 /***** Write number of users who have write posts in this thread *****/
	 HTM_TD_Begin ("class=\"RT %s_%s %s\"",Class,The_GetSuffix (),BgColor);
	    HTM_TxtF ("%u&nbsp;",Thr.NumWriters);
	 HTM_TD_End ();

	 /***** Write number of users who have read this thread *****/
	 HTM_TD_Begin ("class=\"RT %s_%s %s\"",Class,The_GetSuffix (),BgColor);
	    HTM_TxtF ("%u&nbsp;",Thr.NumReaders);
	 HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/***************************** Get data of a thread **************************/
/*****************************************************************************/

void For_GetThreadData (struct For_Thread *Thr)
  {
   extern const char *Txt_no_subject;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Dat_StartEndTime_t Order;

   /***** Get data of a thread from database *****/
   if (For_DB_GetThreadData (&mysql_res,Thr->ThrCod) != 1)
      Err_WrongThreadExit ();
   row = mysql_fetch_row (mysql_res);

   /***** Get the codes of the first (row[0])
          and the last post (row[1]) in this thread *****/
   Thr->PstCod[Dat_STR_TIME] = Str_ConvertStrCodToLongCod (row[0]);
   Thr->PstCod[Dat_END_TIME] = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get the code of the first message (row[0])
          and the last message (row[1]) in this thread *****/
   if (sscanf (row[0],"%ld",&(Thr->PstCod[Dat_STR_TIME])) != 1)
      Err_WrongPostExit ();
   if (sscanf (row[1],"%ld",&(Thr->PstCod[Dat_END_TIME])) != 1)
      Err_WrongPostExit ();

   /***** Get the author of the first post in this thread (row[2])
          and the author of the last  post in this thread (row[3]) *****/
   Thr->UsrCod[Dat_STR_TIME] = Str_ConvertStrCodToLongCod (row[2]);
   Thr->UsrCod[Dat_END_TIME] = Str_ConvertStrCodToLongCod (row[3]);

   /***** Get the date of the first post in this thread (row[4])
          and the date of the last  post in this thread (row[5]) *****/
   Thr->WriteTime[Dat_STR_TIME] = Dat_GetUNIXTimeFromStr (row[4]);
   Thr->WriteTime[Dat_END_TIME] = Dat_GetUNIXTimeFromStr (row[5]);

   /***** Get the subject of this thread (row[6]) *****/
   Str_Copy (Thr->Subject,row[6],sizeof (Thr->Subject) - 1);
   if (!Thr->Subject[0])
      snprintf (Thr->Subject,sizeof (Thr->Subject),"[%s]",Txt_no_subject);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Get if first or last message are enabled *****/
   for (Order  = (Dat_StartEndTime_t) 0;
	Order <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	Order++)
      Thr->Enabled[Order] = For_DB_GetIfPstIsDisabledOrEnabled (Thr->PstCod[Order]);

   /***** Get number of posts in this thread *****/
   Thr->NumPosts = For_DB_GetNumPstsInThr (Thr->ThrCod);

   /***** Get number of unread (by me) posts in this thread *****/
   Thr->NumUnreadPosts = For_GetNumOfUnreadPostsInThr (Thr->ThrCod,Thr->NumPosts);

   /***** Get number of posts that I have written in this thread *****/
   Thr->NumMyPosts = For_DB_GetNumMyPstsInThr (Thr->ThrCod);

   /***** Get number of users who have write posts in this thread *****/
   Thr->NumWriters = For_DB_GetNumOfWritersInThr (Thr->ThrCod);

   /***** Get number of users who have read this thread *****/
   Thr->NumReaders = For_DB_GetNumReadersOfThr (Thr->ThrCod);
  }

/*****************************************************************************/
/**************** Show posts of a thread in a discussion forum ***************/
/*****************************************************************************/

void For_ShowThreadPosts (void)
  {
   struct For_Forums Forums;

   /***** Reset forum *****/
   For_ResetForums (&Forums);

   /***** Get parameters related to forums *****/
   For_GetParsForums (&Forums);

   /***** Show forum list again *****/
   For_ShowForumList (&Forums);

   /***** Show threads again *****/
   For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_SUCCESS,NULL);

   /***** Show the posts of that thread *****/
   For_ShowPostsOfAThread (&Forums,Ale_SUCCESS,NULL);
  }

/*****************************************************************************/
/********************* Get parameters related to a forum *********************/
/*****************************************************************************/

void For_GetParsForums (struct For_Forums *Forums)
  {
   static Hie_Level_t Level[For_NUM_TYPES_FORUM] =
     {
      [For_FORUM_COURSE_USRS] = Hie_CRS,
      [For_FORUM_COURSE_TCHS] = Hie_CRS,
      [For_FORUM_DEGREE_USRS] = Hie_DEG,
      [For_FORUM_DEGREE_TCHS] = Hie_DEG,
      [For_FORUM_CENTER_USRS] = Hie_CTR,
      [For_FORUM_CENTER_TCHS] = Hie_CTR,
      [For_FORUM_INSTIT_USRS] = Hie_INS,
      [For_FORUM_INSTIT_TCHS] = Hie_INS,
      [For_FORUM_GLOBAL_USRS] = Hie_SYS,
      [For_FORUM_GLOBAL_TCHS] = Hie_SYS,
      [For_FORUM__SWAD__USRS] = Hie_SYS,
      [For_FORUM__SWAD__TCHS] = Hie_SYS,
      [For_FORUM_UNKNOWN    ] = Hie_SYS,
     };

   /***** Set forum type *****/
   For_SetForumType (Forums);

   /***** Get parameter with code of institution, center, degree or course *****/
   if (Level[Forums->Forum.Type] == Hie_SYS)
      Forums->Forum.HieCod = -1L;
   else
     {
      if ((Forums->Forum.HieCod = ParCod_GetPar (ParCod_OthHie)) <= 0)
	 // If no institution specified ==> go to current institution forum
	 Forums->Forum.HieCod = Gbl.Hierarchy.Node[Level[Forums->Forum.Type]].HieCod;
     }

   /***** Get which forums I want to see *****/
   Forums->ForumSet = (For_ForumSet_t)
		      Par_GetParUnsignedLong ("ForumSet",
					      0,
					      For_NUM_FORUM_SETS - 1,
					      (unsigned long) For_DEFAULT_FORUM_SET);

   /***** Get order type *****/
   Forums->ThreadsOrder = (Dat_StartEndTime_t)
			  Par_GetParUnsignedLong ("Order",
						  0,
						  Dat_NUM_START_END_TIME - 1,
						  (unsigned long) For_DEFAULT_ORDER);

   /***** Get optional page numbers for threads and posts *****/
   Forums->CurrentPageThrs = Pag_GetParPagNum (Pag_THREADS_FORUM);
   Forums->CurrentPagePsts = Pag_GetParPagNum (Pag_POSTS_FORUM);

   /***** Get optional parameter with code of a selected thread *****/
   Forums->Thread.Current  =
   Forums->Thread.Selected = ParCod_GetPar (ParCod_Thr);

   /***** Get optional parameter with code of a selected post *****/
   Forums->PstCod = ParCod_GetPar (ParCod_Pst);

   /***** If post or thread are specified, get other params from them *****/
   if (Forums->PstCod > 0)	// If post specified ==> get other parameters in function of post
      // Get thread, type and hierarchy code from database
      Forums->Thread.Selected =
      Forums->Thread.Current  = For_GetThreadForumTypeAndHieCodOfAPost (Forums->PstCod,&Forums->Forum);
   else if (Forums->Thread.Current > 0)	// If thread specified ==> get other parameters in function of thread
      // Get type and hierarchy code from database
      For_GetForumTypeAndHieCodOfAThread (Forums->Thread.Current,&Forums->Forum);

   /***** Restrict access to forum *****/
   For_RestrictAccess (Forums);
  }

/*****************************************************************************/
/***************************** Set the type of forum *************************/
/*****************************************************************************/

static void For_SetForumType (struct For_Forums *Forums)
  {
   switch (Gbl.Action.Act)
     {
      case ActSeeFor:
      case ActSeeForGenUsr:	case ActSeePstForGenUsr:
      case ActRcvThrForGenUsr:	case ActRcvRepForGenUsr:
      case ActReqDelThrGenUsr:	case ActDelThrForGenUsr:
      case ActCutThrForGenUsr:	case ActPasThrForGenUsr:
      case ActDelPstForGenUsr:
      case ActEnbPstForGenUsr:	case ActDisPstForGenUsr:
         Forums->Forum.Type = For_FORUM_GLOBAL_USRS;
         break;
      case ActSeeForGenTch:	case ActSeePstForGenTch:
      case ActRcvThrForGenTch:	case ActRcvRepForGenTch:
      case ActReqDelThrGenTch:	case ActDelThrForGenTch:
      case ActCutThrForGenTch:	case ActPasThrForGenTch:
      case ActDelPstForGenTch:
      case ActEnbPstForGenTch:	case ActDisPstForGenTch:
         Forums->Forum.Type = For_FORUM_GLOBAL_TCHS;
         break;
      case ActSeeForSWAUsr:	case ActSeePstForSWAUsr:
      case ActRcvThrForSWAUsr:	case ActRcvRepForSWAUsr:
      case ActReqDelThrSWAUsr:	case ActDelThrForSWAUsr:
      case ActCutThrForSWAUsr:	case ActPasThrForSWAUsr:
      case ActDelPstForSWAUsr:
      case ActEnbPstForSWAUsr:	case ActDisPstForSWAUsr:
         Forums->Forum.Type = For_FORUM__SWAD__USRS;
         break;
      case ActSeeForSWATch:	case ActSeePstForSWATch:
      case ActRcvThrForSWATch:	case ActRcvRepForSWATch:
      case ActReqDelThrSWATch:	case ActDelThrForSWATch:
      case ActCutThrForSWATch:	case ActPasThrForSWATch:
      case ActDelPstForSWATch:
      case ActEnbPstForSWATch:	case ActDisPstForSWATch:
         Forums->Forum.Type = For_FORUM__SWAD__TCHS;
         break;
      case ActSeeForInsUsr:	case ActSeePstForInsUsr:
      case ActRcvThrForInsUsr:	case ActRcvRepForInsUsr:
      case ActReqDelThrInsUsr:	case ActDelThrForInsUsr:
      case ActCutThrForInsUsr:	case ActPasThrForInsUsr:
      case ActDelPstForInsUsr:
      case ActEnbPstForInsUsr:	case ActDisPstForInsUsr:
         Forums->Forum.Type = For_FORUM_INSTIT_USRS;
         break;
      case ActSeeForInsTch:	case ActSeePstForInsTch:
      case ActRcvThrForInsTch:	case ActRcvRepForInsTch:
      case ActReqDelThrInsTch:	case ActDelThrForInsTch:
      case ActCutThrForInsTch:	case ActPasThrForInsTch:
      case ActDelPstForInsTch:
      case ActEnbPstForInsTch:	case ActDisPstForInsTch:
	 Forums->Forum.Type = For_FORUM_INSTIT_TCHS;
	 break;
      case ActSeeForCtrUsr:	case ActSeePstForCtrUsr:
      case ActRcvThrForCtrUsr:	case ActRcvRepForCtrUsr:
      case ActReqDelThrCtrUsr:	case ActDelThrForCtrUsr:
      case ActCutThrForCtrUsr:	case ActPasThrForCtrUsr:
      case ActDelPstForCtrUsr:
      case ActEnbPstForCtrUsr:	case ActDisPstForCtrUsr:
         Forums->Forum.Type = For_FORUM_CENTER_USRS;
         break;
      case ActSeeForCtrTch:	case ActSeePstForCtrTch:
      case ActRcvThrForCtrTch:	case ActRcvRepForCtrTch:
      case ActReqDelThrCtrTch:	case ActDelThrForCtrTch:
      case ActCutThrForCtrTch:	case ActPasThrForCtrTch:
      case ActDelPstForCtrTch:
      case ActEnbPstForCtrTch:	case ActDisPstForCtrTch:
	 Forums->Forum.Type = For_FORUM_CENTER_TCHS;
	 break;
      case ActSeeForDegUsr:	case ActSeePstForDegUsr:
      case ActRcvThrForDegUsr:	case ActRcvRepForDegUsr:
      case ActReqDelThrDegUsr:	case ActDelThrForDegUsr:
      case ActCutThrForDegUsr:	case ActPasThrForDegUsr:
      case ActDelPstForDegUsr:
      case ActEnbPstForDegUsr:	case ActDisPstForDegUsr:
         Forums->Forum.Type = For_FORUM_DEGREE_USRS;
         break;
      case ActSeeForDegTch:	case ActSeePstForDegTch:
      case ActRcvThrForDegTch:	case ActRcvRepForDegTch:
      case ActReqDelThrDegTch:	case ActDelThrForDegTch:
      case ActCutThrForDegTch:	case ActPasThrForDegTch:
      case ActDelPstForDegTch:
      case ActEnbPstForDegTch:	case ActDisPstForDegTch:
	 Forums->Forum.Type = For_FORUM_DEGREE_TCHS;
	 break;
      case ActSeeForCrsUsr:	case ActSeePstForCrsUsr:
      case ActRcvThrForCrsUsr:	case ActRcvRepForCrsUsr:
      case ActReqDelThrCrsUsr:	case ActDelThrForCrsUsr:
      case ActCutThrForCrsUsr:	case ActPasThrForCrsUsr:
      case ActDelPstForCrsUsr:
      case ActEnbPstForCrsUsr:	case ActDisPstForCrsUsr:
      case ActReqLnkForCrsUsr:
         Forums->Forum.Type = For_FORUM_COURSE_USRS;
         break;
      case ActSeeForCrsTch:	case ActSeePstForCrsTch:
      case ActRcvThrForCrsTch:	case ActRcvRepForCrsTch:
      case ActReqDelThrCrsTch:	case ActDelThrForCrsTch:
      case ActCutThrForCrsTch:	case ActPasThrForCrsTch:
      case ActDelPstForCrsTch:
      case ActEnbPstForCrsTch:	case ActDisPstForCrsTch:
         Forums->Forum.Type = For_FORUM_COURSE_TCHS;
         break;
      default:
	 Forums->Forum.Type = For_FORUM_UNKNOWN;
     }
  }

/*****************************************************************************/
/************************** Restrict access to forum *************************/
/*****************************************************************************/

static void For_RestrictAccess (const struct For_Forums *Forums)
  {
   Rol_Role_t MaxRole;
   bool ICanSeeForum;

   /***** Restrict access *****/
   switch (Forums->Forum.Type)
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM__SWAD__USRS:
         ICanSeeForum = true;
         break;
      case For_FORUM_GLOBAL_TCHS:
      case For_FORUM__SWAD__TCHS:
         Rol_GetRolesInAllCrss (&Gbl.Usrs.Me.UsrDat);
         ICanSeeForum = (Gbl.Usrs.Me.UsrDat.Roles.InCrss & ((1 << Rol_NET) |
                                                            (1 << Rol_TCH)));
         break;
      case For_FORUM_INSTIT_USRS:
	 MaxRole = Rol_GetMyMaxRoleIn (Hie_INS,Forums->Forum.HieCod);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole == Rol_STD ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      case For_FORUM_INSTIT_TCHS:
	 MaxRole = Rol_GetMyMaxRoleIn (Hie_INS,Forums->Forum.HieCod);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      case For_FORUM_CENTER_USRS:
	 MaxRole = Rol_GetMyMaxRoleIn (Hie_CTR,Forums->Forum.HieCod);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole >= Rol_STD ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      case For_FORUM_CENTER_TCHS:
	 MaxRole = Rol_GetMyMaxRoleIn (Hie_CTR,Forums->Forum.HieCod);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      case For_FORUM_DEGREE_USRS:
	 MaxRole = Rol_GetMyMaxRoleIn (Hie_DEG,Forums->Forum.HieCod);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole >= Rol_STD ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      case For_FORUM_DEGREE_TCHS:
	 MaxRole = Rol_GetMyMaxRoleIn (Hie_DEG,Forums->Forum.HieCod);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      case For_FORUM_COURSE_USRS:
	 MaxRole = Rol_GetMyRoleInCrs (Forums->Forum.HieCod);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole >= Rol_STD ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      case For_FORUM_COURSE_TCHS:
	 MaxRole = Rol_GetMyRoleInCrs (Forums->Forum.HieCod);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      default:
	 ICanSeeForum = false;
	 break;
     }
   if (!ICanSeeForum)
      Err_NoPermissionExit ();
  }

/*****************************************************************************/
/********************** Show an area to write a message **********************/
/*****************************************************************************/

static void For_WriteFormForumPst (struct For_Forums *Forums,
                                   bool IsReply,const char *Subject)
  {
   extern const char *Hlp_COMMUNICATION_Forums_new_post;
   extern const char *Hlp_COMMUNICATION_Forums_new_thread;
   extern const char *Txt_Post;
   extern const char *Txt_Thread;
   extern const char *Txt_MSG_Subject;
   extern const char *Txt_MSG_Content;
   extern const char *Txt_Send;
   char *ClassInput;

   /***** Begin box *****/
   Box_BoxBegin (IsReply ? Txt_Post :
			   Txt_Thread,
		 NULL,NULL,
		 IsReply ? Hlp_COMMUNICATION_Forums_new_post :
			   Hlp_COMMUNICATION_Forums_new_thread,
		 Box_NOT_CLOSABLE);

      /***** Begin form *****/
      if (IsReply)	// Form to write a reply to a post of an existing thread
	{
	 Frm_BeginFormAnchor (For_ActionsRecRepFor[Forums->Forum.Type],
			      For_FORUM_POSTS_SECTION_ID);
	    For_PutParsNewPost (Forums);
	}
      else		// Form to write the first post of a new thread
	{
	 Frm_BeginFormAnchor (For_ActionsRecThrFor[Forums->Forum.Type],
			      For_FORUM_POSTS_SECTION_ID);
	    For_PutParsNewThread (Forums);
	}

	 /***** Subject and content *****/
	 HTM_TABLE_BeginCenterPadding (2);

	    // If writing a reply to a message of an existing thread ==> write subject
	    /* Subject */
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("Frm_C1 RT","Subject",Txt_MSG_Subject);

	       /* Data */
	       HTM_TD_Begin ("class=\"Frm_C2 LT\"");
		  HTM_INPUT_TEXT ("Subject",Cns_MAX_CHARS_SUBJECT,
				  IsReply ? Subject :
					    "",
				  HTM_DONT_SUBMIT_ON_CHANGE,
				  "id=\"Subject\""
				  " class=\"Frm_C2_INPUT INPUT_%s\""
				  " required=\"required\"",
				  The_GetSuffix ());
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /* Content */
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("Frm_C1 RT","Content",Txt_MSG_Content);

	       /* Data */
	       HTM_TD_Begin ("class=\"Frm_C2 LT\"");
		  HTM_TEXTAREA_Begin ("id=\"Content\" name=\"Content\""
				      " class=\"Frm_C2_INPUT INPUT_%s\""
				      " rows=\"10\"",
				      The_GetSuffix ());
		  HTM_TEXTAREA_End ();
	       HTM_TD_End ();

	    HTM_TR_End ();

	 HTM_TABLE_End ();

	 /***** Help for text editor *****/
	 Lay_HelpPlainEditor ();

	 /***** Attached image (optional) *****/
	 if (asprintf (&ClassInput,"FOR_MED_INPUT INPUT_%s",
		       The_GetSuffix ()) < 0)
	    Err_NotEnoughMemoryExit ();
	 Med_PutMediaUploader (-1,ClassInput);
	 free (ClassInput);

	 /***** Send button *****/
	 Btn_PutCreateButton (Txt_Send);

      /***** End form *****/
      Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************************** Receive message of a forum ***********************/
/*****************************************************************************/

void For_ReceiveForumPost (void)
  {
   extern const char *Txt_FORUM_Post_sent;
   struct For_Forums Forums;
   bool IsReply = false;
   long PstCod = 0;
   unsigned NumUsrsToBeNotifiedByEMail;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Content[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Med_Media Media;

   /***** Reset forum *****/
   For_ResetForums (&Forums);

   /***** Get parameters related to forum *****/
   For_GetParsForums (&Forums);

   /***** Get the code of the thread y the número of page *****/
   if (Gbl.Action.Act == ActRcvRepForCrsUsr || Gbl.Action.Act == ActRcvRepForCrsTch ||
       Gbl.Action.Act == ActRcvRepForDegUsr || Gbl.Action.Act == ActRcvRepForDegTch ||
       Gbl.Action.Act == ActRcvRepForCtrUsr || Gbl.Action.Act == ActRcvRepForCtrTch ||
       Gbl.Action.Act == ActRcvRepForInsUsr || Gbl.Action.Act == ActRcvRepForInsTch ||
       Gbl.Action.Act == ActRcvRepForGenUsr || Gbl.Action.Act == ActRcvRepForGenTch ||
       Gbl.Action.Act == ActRcvRepForSWAUsr || Gbl.Action.Act == ActRcvRepForSWATch)
      IsReply = true;

   /***** Get message subject *****/
   Par_GetParHTML ("Subject",Subject,Cns_MAX_BYTES_SUBJECT);

   /***** Get message body *****/
   Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_LONG_TEXT,
                              Str_TO_RIGOROUS_HTML,Str_DONT_REMOVE_SPACES);

   /***** Initialize media *****/
   Med_MediaConstructor (&Media);

   /***** Get attached media *****/
   Media.Width   = For_IMAGE_SAVED_MAX_WIDTH;
   Media.Height  = For_IMAGE_SAVED_MAX_HEIGHT;
   Media.Quality = For_IMAGE_SAVED_QUALITY;
   Med_GetMediaFromForm (-1L,-1L,-1,&Media,NULL,
	                 For_FORUM_POSTS_SECTION_ID);	// Alerts will be shown later in posts section

   /***** Create a new message *****/
   if (IsReply)	// This post is a reply to another posts in the thread
     {
      // Forums.ThrCod has been received from form

      /***** Create last message of the thread *****/
      PstCod = For_InsertForumPst (Forums.Thread.Current,Gbl.Usrs.Me.UsrDat.UsrCod,
                                   Subject,Content,&Media);

      /***** Modify last message of the thread *****/
      For_DB_UpdateThrLastPst (Forums.Thread.Current,PstCod);
     }
   else			// This post is the first of a new thread
     {
      /***** Create new thread with unknown first and last message codes *****/
      Forums.Thread.Current  =
      Forums.Thread.Selected = For_DB_InsertForumThread (&Forums,-1L);

      /***** Create first (and last) message of the thread *****/
      PstCod = For_InsertForumPst (Forums.Thread.Current,Gbl.Usrs.Me.UsrDat.UsrCod,
                                   Subject,Content,&Media);

      /***** Update first and last posts of new thread *****/
      For_DB_UpdateThrFirstAndLastPst (Forums.Thread.Current,PstCod,PstCod);
     }

   /***** Free media *****/
   Med_MediaDestructor (&Media);

   /***** Increment number of forum posts in my user's figures *****/
   Prf_DB_IncrementNumForPstUsr (Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Notify the new post to users in course *****/
   switch (Forums.Forum.Type)
     {
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
	 if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_FORUM_POST_COURSE,PstCod)))
	    For_DB_UpdateNumUsrsNotifiedByEMailAboutPost (PstCod,NumUsrsToBeNotifiedByEMail);
	 break;
      default:
	 break;
     }

   /***** Notify the new post to previous writers in this thread *****/
   if (IsReply)
      if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_FORUM_REPLY,PstCod)))
         For_DB_UpdateNumUsrsNotifiedByEMailAboutPost (PstCod,NumUsrsToBeNotifiedByEMail);

   /***** Insert forum post into public social activity *****/
   switch (Forums.Forum.Type)	// Only if forum is public for any logged user
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM__SWAD__USRS:
         TmlNot_StoreAndPublishNote (TmlNot_FORUM_POST,PstCod);
         break;
      default:
	 break;
     }

   /***** Show forum list again *****/
   For_ShowForumList (&Forums);

   /***** Show threads again *****/
   For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_SUCCESS,NULL);

   /***** Show again the posts of this thread of the forum *****/
   For_ShowPostsOfAThread (&Forums,Ale_SUCCESS,Txt_FORUM_Post_sent);
  }

/*****************************************************************************/
/***************************** Delete a forum post ***************************/
/*****************************************************************************/

void For_RemovePost (void)
  {
   extern const char *Txt_FORUM_Post_and_thread_removed;
   extern const char *Txt_FORUM_Post_removed;
   struct For_Forums Forums;
   struct Usr_Data UsrDat;
   time_t CreatTimeUTC;	// Creation time of a message
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char OriginalContent[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Med_Media Media;
   bool ThreadDeleted = false;

   /***** Reset forum *****/
   For_ResetForums (&Forums);

   /***** Get parameters related to forum *****/
   For_GetParsForums (&Forums);

   /***** Initialize image *****/
   Med_MediaConstructor (&Media);

   /***** Get forum post data *****/
   For_GetPstData (Forums.PstCod,&UsrDat.UsrCod,&CreatTimeUTC,
                   Subject,OriginalContent,&Media);

   /***** Check if I can remove the post *****/
   /* Check if the message really exists, if it has not been removed */
   if (!For_DB_CheckIfForumPstExists (Forums.PstCod))
      Err_WrongPostExit ();

   /* Check if I am the author of the message */
   if (Usr_ItsMe (UsrDat.UsrCod) == Usr_OTHER)
      Err_NoPermissionExit ();

   /* Check if the message is the last message in the thread */
   if (Forums.PstCod != For_DB_GetThrLastPst (Forums.Thread.Current))
      Err_NoPermissionExit ();

   /***** Remove the post *****/
   ThreadDeleted = For_RemoveForumPst (Forums.PstCod,Media.MedCod);

   /***** Free image *****/
   Med_MediaDestructor (&Media);

   /***** Mark possible notifications as removed *****/
   Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_FORUM_POST_COURSE,Forums.PstCod);
   Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_FORUM_REPLY      ,Forums.PstCod);

   /***** Mark possible social note as unavailable *****/
   switch (Forums.Forum.Type)	// Only if forum is public for any logged user
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM__SWAD__USRS:
         Tml_DB_MarkNoteAsUnavailable (TmlNot_FORUM_POST,Forums.PstCod);
         break;
      default:
	 break;
     }

   /***** Show forum list again *****/
   For_ShowForumList (&Forums);

   if (ThreadDeleted)
      /***** Show the remaining threads *****/
      For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_SUCCESS,Txt_FORUM_Post_and_thread_removed);
   else
     {
      /***** Show threads again *****/
      For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_SUCCESS,NULL);

      /***** Show the remaining posts *****/
      For_ShowPostsOfAThread (&Forums,Ale_SUCCESS,Txt_FORUM_Post_removed);
     }
  }

/*****************************************************************************/
/***************** Request the removing of an existing thread ****************/
/*****************************************************************************/

void For_ReqRemThread (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_entire_thread_X;
   struct For_Forums Forums;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];

   /***** Reset forum *****/
   For_ResetForums (&Forums);

   /***** Get parameters related to forum *****/
   For_GetParsForums (&Forums);

   /***** Get subject of the thread to delete *****/
   For_DB_GetThrSubject (Forums.Thread.Current,Subject);

   /***** Show forum list again *****/
   For_ShowForumList (&Forums);

   /***** Show question and button to remove the thread *****/
   HTM_SECTION_Begin (For_REMOVE_THREAD_SECTION_ID);
      Ale_ShowAlertRemove (For_ActionsDelThrFor[Forums.Forum.Type],
			   For_FORUM_THREADS_SECTION_ID,
			   For_PutParsRemThread,&Forums,
			   Txt_Do_you_really_want_to_remove_the_entire_thread_X,
			   Subject);
   HTM_SECTION_End ();

   /***** Show the threads again *****/
   For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_SUCCESS,NULL);
  }

static void For_PutParsRemThread (void *Forums)
  {
   if (Forums)
      For_PutAllParsForum (((struct For_Forums *) Forums)->CurrentPageThrs,	// Page of threads = current
			   1,				// Page of posts   = first
			   ((struct For_Forums *) Forums)->ForumSet,
			   ((struct For_Forums *) Forums)->ThreadsOrder,
			   ((struct For_Forums *) Forums)->Forum.HieCod,
			   ((struct For_Forums *) Forums)->Thread.Current,
			   -1L);
  }

/*****************************************************************************/
/*************************** Remove an existing thread ***********************/
/*****************************************************************************/

void For_RemoveThread (void)
  {
   extern const char *Txt_Thread_X_removed;
   extern const char *Txt_Thread_removed;
   struct For_Forums Forums;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Message[256 + Cns_MAX_BYTES_SUBJECT + 1];

   /***** Reset forum *****/
   For_ResetForums (&Forums);

   /***** Get parameters related to forum *****/
   For_GetParsForums (&Forums);

   /***** Check if I have permission to remove thread in this forum *****/
   if (!(PermissionThreadDeletion[Forums.Forum.Type] &
         (1 << Gbl.Usrs.Me.Role.Logged)))
      Err_NoPermissionExit ();

   /***** Get subject of thread to delete *****/
   For_DB_GetThrSubject (Forums.Thread.Current,Subject);

   /***** Remove the thread and all its posts *****/
   For_RemoveThreadAndItsPsts (Forums.Thread.Current);

   /***** Show forum list again *****/
   For_ShowForumList (&Forums);

   /***** Show the threads again *****/
   if (Subject[0])
     {
      snprintf (Message,sizeof (Message),
		Txt_Thread_X_removed,Subject);
      For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_SUCCESS,Message);
     }
   else
      For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_SUCCESS,Txt_Thread_removed);
  }

/*****************************************************************************/
/*************** Cut a thread to move it to another forum ********************/
/*****************************************************************************/

void For_CutThread (void)
  {
   extern const char *Txt_Thread_X_marked_to_be_moved;
   extern const char *Txt_Thread_marked_to_be_moved;
   struct For_Forums Forums;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Message[256 + Cns_MAX_BYTES_SUBJECT + 1];

   /***** Reset forum *****/
   For_ResetForums (&Forums);

   /***** Get parameters related to forum *****/
   For_GetParsForums (&Forums);

   /***** Get subject of thread to cut *****/
   For_DB_GetThrSubject (Forums.Thread.Current,Subject);

   /***** Mark the thread as cut *****/
   For_InsertThrInClipboard (Forums.Thread.Current);

   /***** Show forum list again *****/
   For_ShowForumList (&Forums);

   /***** Show the threads again *****/
   if (Subject[0])
     {
      snprintf (Message,sizeof (Message),
                Txt_Thread_X_marked_to_be_moved,Subject);
      For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_SUCCESS,Message);
     }
   else
      For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_SUCCESS,Txt_Thread_marked_to_be_moved);

  }

/*****************************************************************************/
/************* Paste the thread in clipboard into current forum **************/
/*****************************************************************************/

void For_PasteThread (void)
  {
   extern const char *Txt_The_thread_X_is_already_in_this_forum;
   extern const char *Txt_The_thread_is_already_in_this_forum;
   extern const char *Txt_Thread_X_moved_to_this_forum;
   extern const char *Txt_Thread_moved_to_this_forum;
   struct For_Forums Forums;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Message[256 + Cns_MAX_BYTES_SUBJECT + 1];

   /***** Reset forum *****/
   For_ResetForums (&Forums);

   /***** Get parameters related to forum *****/
   For_GetParsForums (&Forums);

   /***** Get subject of thread to paste *****/
   For_DB_GetThrSubject (Forums.Thread.Current,Subject);

   /***** Check if paste (move) the thread to current forum has sense *****/
   if (For_DB_CheckIfThrBelongsToForum (Forums.Thread.Current,&Forums.Forum))
     {
      /***** Show forum list again *****/
      For_ShowForumList (&Forums);

      /***** Show the threads again *****/
      if (Subject[0])
	{
         snprintf (Message,sizeof (Message),
                   Txt_The_thread_X_is_already_in_this_forum,Subject);
         For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_WARNING,Message);
        }
      else
         For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_WARNING,Txt_The_thread_is_already_in_this_forum);
     }
   else
     {
      /***** Paste (move) the thread to current forum *****/
      For_DB_MoveThrToCurrentForum (&Forums);

      /***** Show forum list again *****/
      For_ShowForumList (&Forums);

      /***** Show the threads again *****/
      if (Subject[0])
	{
         snprintf (Message,sizeof (Message),
	           Txt_Thread_X_moved_to_this_forum,Subject);
         For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_SUCCESS,Message);
	}
      else
         For_ShowForumThreadsHighlightingOneThread (&Forums,Ale_SUCCESS,Txt_Thread_moved_to_this_forum);
     }
  }

/*****************************************************************************/
/*********************** Check if I can move threads *************************/
/*****************************************************************************/

static bool For_CheckIfICanMoveThreads (void)
  {
   return (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);	// If I have permission to move threads...
  }

/*****************************************************************************/
/********************* Insert thread in thread clipboard ********************/
/*****************************************************************************/

static void For_InsertThrInClipboard (long ThrCod)
  {
   /***** Remove expired thread clipboards *****/
   For_DB_RemoveExpiredClipboards ();

   /***** Add thread to my clipboard *****/
   For_DB_InsertThrInMyClipboard (ThrCod);
  }

/*****************************************************************************/
/************************** Show figures about forums ************************/
/*****************************************************************************/

void For_GetAndShowForumStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_forums;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Scope;
   extern const char *Txt_Forums;
   extern const char *Txt_Number_of_forums;
   extern const char *Txt_Number_of_threads;
   extern const char *Txt_Number_of_posts;
   extern const char *Txt_Number_of_BR_notifications;
   extern const char *Txt_Number_of_threads_BR_per_forum;
   extern const char *Txt_Number_of_posts_BR_per_thread;
   extern const char *Txt_Number_of_posts_BR_per_forum;
   struct For_FiguresForum FiguresForum;
   long HieCod[Hie_NUM_LEVELS] =
     {
      [Hie_UNK] = -1L,
      [Hie_SYS] = -1L,
      [Hie_CTY] = -1L,
      [Hie_INS] = -1L,
      [Hie_CTR] = -1L,
      [Hie_DEG] = -1L,
      [Hie_CRS] = -1L,
     };

   HieCod[Gbl.Scope.Current] = Gbl.Hierarchy.Node[Gbl.Scope.Current].HieCod;

   /***** Reset total stats *****/
   FiguresForum.NumForums  = 0;
   FiguresForum.NumThreads = 0;
   FiguresForum.NumPosts   = 0;
   FiguresForum.NumUsrsToBeNotifiedByEMail = 0;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_FORUMS],NULL,NULL,
                      Hlp_ANALYTICS_Figures_forums,Box_NOT_CLOSABLE,2);

      /***** Write table heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH_Span_Begin (HTM_HEAD_CENTER,1,1,"BT");
	    Ico_PutIcon ("comments.svg",Ico_BLACK,Txt_Scope,"ICOx16");
	 HTM_TH_End ();
	 HTM_TH (Txt_Forums                        ,HTM_HEAD_LEFT);
	 HTM_TH (Txt_Number_of_forums              ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_threads             ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_posts               ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_BR_notifications    ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_threads_BR_per_forum,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_posts_BR_per_thread ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_posts_BR_per_forum  ,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Write a row for each type of forum *****/
      switch (Gbl.Scope.Current)
	{
	 case Hie_SYS:
	    For_ShowStatOfAForumType (For_FORUM_GLOBAL_USRS,HieCod,&FiguresForum);
	    For_ShowStatOfAForumType (For_FORUM_GLOBAL_TCHS,HieCod,&FiguresForum);
	    For_ShowStatOfAForumType (For_FORUM__SWAD__USRS,HieCod,&FiguresForum);
	    For_ShowStatOfAForumType (For_FORUM__SWAD__TCHS,HieCod,&FiguresForum);
	    /* falls through */
	    /* no break */
	 case Hie_CTY:
	 case Hie_INS:
	    For_ShowStatOfAForumType (For_FORUM_INSTIT_USRS,HieCod,&FiguresForum);
	    For_ShowStatOfAForumType (For_FORUM_INSTIT_TCHS,HieCod,&FiguresForum);
	    /* falls through */
	    /* no break */
	 case Hie_CTR:
	    For_ShowStatOfAForumType (For_FORUM_CENTER_USRS,HieCod,&FiguresForum);
	    For_ShowStatOfAForumType (For_FORUM_CENTER_TCHS,HieCod,&FiguresForum);
	    /* falls through */
	    /* no break */
	 case Hie_DEG:
	    For_ShowStatOfAForumType (For_FORUM_DEGREE_USRS,HieCod,&FiguresForum);
	    For_ShowStatOfAForumType (For_FORUM_DEGREE_TCHS,HieCod,&FiguresForum);
	    /* falls through */
	    /* no break */
	 case Hie_CRS:
	    For_ShowStatOfAForumType (For_FORUM_COURSE_USRS,HieCod,&FiguresForum);
	    For_ShowStatOfAForumType (For_FORUM_COURSE_TCHS,HieCod,&FiguresForum);
	    break;
	 default:
	    Err_WrongHierarchyLevelExit ();
	    break;
	}

      For_WriteForumTotalStats (&FiguresForum);

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/************************* Show stats of a forum type ************************/
/*****************************************************************************/

static void For_ShowStatOfAForumType (For_ForumType_t ForumType,
				      long HieCod[Hie_NUM_LEVELS],
                                      struct For_FiguresForum *FiguresForum)
  {
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_General;
   extern const char *Txt_only_teachers;
   static const char *PlatformShortName = Cfg_PLATFORM_SHORT_NAME;
   static const char *EmptyName = "";
   static const struct
     {
      const char *Icon;
      const char **ForumName1;
      const char **ForumName2;
     } Pars[For_NUM_TYPES_FORUM] =
     {
      [For_FORUM_COURSE_USRS] = {"chalkboard-teacher.svg",&Txt_HIERARCHY_PLURAL_Abc[Hie_CRS],&EmptyName        },
      [For_FORUM_COURSE_TCHS] = {"chalkboard-teacher.svg",&Txt_HIERARCHY_PLURAL_Abc[Hie_CRS],&Txt_only_teachers},
      [For_FORUM_DEGREE_USRS] = {"graduation-cap.svg"    ,&Txt_HIERARCHY_PLURAL_Abc[Hie_DEG],&EmptyName        },
      [For_FORUM_DEGREE_TCHS] = {"graduation-cap.svg"    ,&Txt_HIERARCHY_PLURAL_Abc[Hie_DEG],&Txt_only_teachers},
      [For_FORUM_CENTER_USRS] = {"building.svg"          ,&Txt_HIERARCHY_PLURAL_Abc[Hie_CTR],&EmptyName        },
      [For_FORUM_CENTER_TCHS] = {"building.svg"          ,&Txt_HIERARCHY_PLURAL_Abc[Hie_CTR],&Txt_only_teachers},
      [For_FORUM_INSTIT_USRS] = {"university.svg"        ,&Txt_HIERARCHY_PLURAL_Abc[Hie_INS],&EmptyName        },
      [For_FORUM_INSTIT_TCHS] = {"university.svg"        ,&Txt_HIERARCHY_PLURAL_Abc[Hie_INS],&Txt_only_teachers},
      [For_FORUM_GLOBAL_USRS] = {"comments.svg"          ,&Txt_General      		    ,&EmptyName        },
      [For_FORUM_GLOBAL_TCHS] = {"comments.svg"          ,&Txt_General      		    ,&Txt_only_teachers},
      [For_FORUM__SWAD__USRS] = {"swad64x64.png"         ,&PlatformShortName		    ,&EmptyName        },
      [For_FORUM__SWAD__TCHS] = {"swad64x64.png"         ,&PlatformShortName		    ,&Txt_only_teachers},
     };

   if (Pars[ForumType].Icon)
      For_WriteForumTitleAndStats (ForumType,HieCod,
				    Pars[ForumType].Icon,FiguresForum,
				   *Pars[ForumType].ForumName1,
				   *Pars[ForumType].ForumName2);
  }

/*****************************************************************************/
/******************* Write title and stats of a forum type *******************/
/*****************************************************************************/

static void For_WriteForumTitleAndStats (For_ForumType_t ForumType,
					 long HieCod[Hie_NUM_LEVELS],
                                         const char *Icon,struct For_FiguresForum *FiguresForum,
                                         const char *ForumName1,const char *ForumName2)
  {
   unsigned NumForums;
   unsigned NumThreads;
   unsigned NumPosts;
   unsigned NumUsrsToBeNotifiedByEMail;
   double NumThrsPerForum;
   double NumPostsPerThread;
   double NumPostsPerForum;
   char *ForumName;

   /***** Compute number of forums, number of threads and number of posts *****/
   NumForums  = For_DB_GetNumTotalForumsOfType       (ForumType,HieCod);
   NumThreads = For_DB_GetNumTotalThrsInForumsOfType (ForumType,HieCod);
   NumPosts   = For_DB_GetNumTotalPstsInForumsOfType (ForumType,HieCod,&NumUsrsToBeNotifiedByEMail);

   /***** Compute number of threads per forum, number of posts per forum and number of posts per thread *****/
   NumThrsPerForum = (NumForums ? (double) NumThreads / (double) NumForums :
	                          0.0);
   NumPostsPerThread = (NumThreads ? (double) NumPosts / (double) NumThreads :
	                             0.0);
   NumPostsPerForum = (NumForums ? (double) NumPosts / (double) NumForums :
	                           0.0);

   /***** Update total stats *****/
   FiguresForum->NumForums                  += NumForums;
   FiguresForum->NumThreads                 += NumThreads;
   FiguresForum->NumPosts                   += NumPosts;
   FiguresForum->NumUsrsToBeNotifiedByEMail += NumUsrsToBeNotifiedByEMail;

   /***** Write forum name and stats *****/
   HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"BT\"");
	 if (asprintf (&ForumName,"%s%s",
		       ForumName1,ForumName2) < 0)
	    Err_NotEnoughMemoryExit ();
	 Ico_PutIcon (Icon,Ico_BLACK,ForumName,"ICOx16");
	 free (ForumName);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
	 HTM_Txt (ForumName1);
	 HTM_Txt (ForumName2);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RT DAT_%s\"",The_GetSuffix ());
      HTM_Unsigned (NumForums);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RT DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (NumThreads);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RT DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (NumPosts);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RT DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (NumUsrsToBeNotifiedByEMail);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RT DAT_%s\"",The_GetSuffix ());
	 HTM_Double2Decimals (NumThrsPerForum);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RT DAT_%s\"",The_GetSuffix ());
	 HTM_Double2Decimals (NumPostsPerThread);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RT DAT_%s\"",The_GetSuffix ());
	 HTM_Double2Decimals (NumPostsPerForum);
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Write title and stats of a forum type *******************/
/*****************************************************************************/

static void For_WriteForumTotalStats (struct For_FiguresForum *FiguresForum)
  {
   extern const char *Txt_Total;
   double NumThrsPerForum;
   double NumPostsPerThread;
   double NumPostsPerForum;

   /***** Compute number of threads per forum, number of posts per forum and number of posts per thread *****/
   NumThrsPerForum   = (FiguresForum->NumForums  ? (double) FiguresForum->NumThreads /
	                                           (double) FiguresForum->NumForums :
	                                           0.0);
   NumPostsPerThread = (FiguresForum->NumThreads ? (double) FiguresForum->NumPosts /
	                                           (double) FiguresForum->NumThreads :
	                                           0.0);
   NumPostsPerForum  = (FiguresForum->NumForums  ? (double) FiguresForum->NumPosts /
	                                           (double) FiguresForum->NumForums :
	                                           0.0);

   /***** Write forum name and stats *****/
   HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_STRONG_%s LINE_TOP\" style=\"width:20px;\"",
                    The_GetSuffix ());
      HTM_TD_End ();

      HTM_TD_LINE_TOP_Txt (Txt_Total);
      HTM_TD_LINE_TOP_Unsigned (FiguresForum->NumForums);
      HTM_TD_LINE_TOP_Unsigned (FiguresForum->NumThreads);
      HTM_TD_LINE_TOP_Unsigned (FiguresForum->NumPosts);
      HTM_TD_LINE_TOP_Unsigned (FiguresForum->NumUsrsToBeNotifiedByEMail);
      HTM_TD_LINE_TOP_Double2Decimals (NumThrsPerForum);
      HTM_TD_LINE_TOP_Double2Decimals (NumPostsPerThread);
      HTM_TD_LINE_TOP_Double2Decimals (NumPostsPerForum);

   HTM_TR_End ();
  }
