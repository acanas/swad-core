// swad_forum.c: forums

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
#include <linux/stddef.h>	// For NULL
#include <malloc.h>		// For malloc
#include <mysql/mysql.h>	// To access MySQL databases
#include <stdio.h>		// For asprintf
#include <string.h>
#include <time.h>		// For time_t

#include "swad_action.h"
#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_layout.h"
#include "swad_logo.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_profile.h"
#include "swad_role.h"
#include "swad_timeline.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*********************** Private constants and types *************************/
/*****************************************************************************/

#define For_FORUM_MAX_LEVELS 4

const unsigned PermissionThreadDeletion[For_NUM_TYPES_FORUM] =
  {
   0x3F0,	// For_FORUM_COURSE_USRS
   0x3F0,	// For_FORUM_COURSE_TCHS
   0x3E0,	// For_FORUM_DEGREE_USRS
   0x3E0,	// For_FORUM_DEGREE_TCHS
   0x3C0,	// For_FORUM_CENTRE_USRS
   0x3C0,	// For_FORUM_CENTRE_TCHS
   0x380,	// For_FORUM_INSTIT_USRS
   0x380,	// For_FORUM_INSTIT_TCHS
   0x300,	// For_FORUM_GLOBAL_USRS
   0x300,	// For_FORUM_GLOBAL_TCHS
   0x300,	// For_FORUM__SWAD__USRS
   0x300,	// For_FORUM__SWAD__TCHS
   0x000,	// For_FORUM_UNKNOWN
  };

const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM] =
  {
   ActSeeForCrsUsr,
   ActSeeForCrsTch,
   ActSeeForDegUsr,
   ActSeeForDegTch,
   ActSeeForCtrUsr,
   ActSeeForCtrTch,
   ActSeeForInsUsr,
   ActSeeForInsTch,
   ActSeeForGenUsr,
   ActSeeForGenTch,
   ActSeeForSWAUsr,
   ActSeeForSWATch,
   ActSeeFor,		// For_FORUM_UNKNOWN
  };
const Act_Action_t For_ActionsSeePstFor[For_NUM_TYPES_FORUM] =
  {
   ActSeePstForCrsUsr,
   ActSeePstForCrsTch,
   ActSeePstForDegUsr,
   ActSeePstForDegTch,
   ActSeePstForCtrUsr,
   ActSeePstForCtrTch,
   ActSeePstForInsUsr,
   ActSeePstForInsTch,
   ActSeePstForGenUsr,
   ActSeePstForGenTch,
   ActSeePstForSWAUsr,
   ActSeePstForSWATch,
   ActSeeFor,		// For_FORUM_UNKNOWN
  };
const Act_Action_t For_ActionsRecThrFor[For_NUM_TYPES_FORUM] =
  {
   ActRcvThrForCrsUsr,
   ActRcvThrForCrsTch,
   ActRcvThrForDegUsr,
   ActRcvThrForDegTch,
   ActRcvThrForCtrUsr,
   ActRcvThrForCtrTch,
   ActRcvThrForInsUsr,
   ActRcvThrForInsTch,
   ActRcvThrForGenUsr,
   ActRcvThrForGenTch,
   ActRcvThrForSWAUsr,
   ActRcvThrForSWATch,
   ActSeeFor,		// For_FORUM_UNKNOWN
  };
const Act_Action_t For_ActionsRecRepFor[For_NUM_TYPES_FORUM] =
  {
   ActRcvRepForCrsUsr,
   ActRcvRepForCrsTch,
   ActRcvRepForDegUsr,
   ActRcvRepForDegTch,
   ActRcvRepForCtrUsr,
   ActRcvRepForCtrTch,
   ActRcvRepForInsUsr,
   ActRcvRepForInsTch,
   ActRcvRepForGenUsr,
   ActRcvRepForGenTch,
   ActRcvRepForSWAUsr,
   ActRcvRepForSWATch,
   ActSeeFor,		// For_FORUM_UNKNOWN
  };
const Act_Action_t For_ActionsReqDelThr[For_NUM_TYPES_FORUM] =
  {
   ActReqDelThrCrsUsr,
   ActReqDelThrCrsTch,
   ActReqDelThrDegUsr,
   ActReqDelThrDegTch,
   ActReqDelThrCtrUsr,
   ActReqDelThrCtrTch,
   ActReqDelThrInsUsr,
   ActReqDelThrInsTch,
   ActReqDelThrGenUsr,
   ActReqDelThrGenTch,
   ActReqDelThrSWAUsr,
   ActReqDelThrSWATch,
   ActSeeFor,		// For_FORUM_UNKNOWN
  };
const Act_Action_t For_ActionsDelThrFor[For_NUM_TYPES_FORUM] =
  {
   ActDelThrForCrsUsr,
   ActDelThrForCrsTch,
   ActDelThrForDegUsr,
   ActDelThrForDegTch,
   ActDelThrForCtrUsr,
   ActDelThrForCtrTch,
   ActDelThrForInsUsr,
   ActDelThrForInsTch,
   ActDelThrForGenUsr,
   ActDelThrForGenTch,
   ActDelThrForSWAUsr,
   ActDelThrForSWATch,
   ActSeeFor,		// For_FORUM_UNKNOWN
  };
const Act_Action_t For_ActionsCutThrFor[For_NUM_TYPES_FORUM] =
  {
   ActCutThrForCrsUsr,
   ActCutThrForCrsTch,
   ActCutThrForDegUsr,
   ActCutThrForDegTch,
   ActCutThrForCtrUsr,
   ActCutThrForCtrTch,
   ActCutThrForInsUsr,
   ActCutThrForInsTch,
   ActCutThrForGenUsr,
   ActCutThrForGenTch,
   ActCutThrForSWAUsr,
   ActCutThrForSWATch,
   ActSeeFor,		// For_FORUM_UNKNOWN
  };
const Act_Action_t For_ActionsPasThrFor[For_NUM_TYPES_FORUM] =
  {
   ActPasThrForCrsUsr,
   ActPasThrForCrsTch,
   ActPasThrForDegUsr,
   ActPasThrForDegTch,
   ActPasThrForCtrUsr,
   ActPasThrForCtrTch,
   ActPasThrForInsUsr,
   ActPasThrForInsTch,
   ActPasThrForGenUsr,
   ActPasThrForGenTch,
   ActPasThrForSWAUsr,
   ActPasThrForSWATch,
   ActSeeFor,		// For_FORUM_UNKNOWN
  };
const Act_Action_t For_ActionsDelPstFor[For_NUM_TYPES_FORUM] =
  {
   ActDelPstForCrsUsr,
   ActDelPstForCrsTch,
   ActDelPstForDegUsr,
   ActDelPstForDegTch,
   ActDelPstForCtrUsr,
   ActDelPstForCtrTch,
   ActDelPstForInsUsr,
   ActDelPstForInsTch,
   ActDelPstForGenUsr,
   ActDelPstForGenTch,
   ActDelPstForSWAUsr,
   ActDelPstForSWATch,
   ActSeeFor,		// For_FORUM_UNKNOWN
  };
const Act_Action_t For_ActionsEnbPstFor[For_NUM_TYPES_FORUM] =
  {
   ActEnbPstForCrsUsr,
   ActEnbPstForCrsTch,
   ActEnbPstForDegUsr,
   ActEnbPstForDegTch,
   ActEnbPstForCtrUsr,
   ActEnbPstForCtrTch,
   ActEnbPstForInsUsr,
   ActEnbPstForInsTch,
   ActEnbPstForGenUsr,
   ActEnbPstForGenTch,
   ActEnbPstForSWAUsr,
   ActEnbPstForSWATch,
   ActSeeFor,		// For_FORUM_UNKNOWN
  };
const Act_Action_t For_ActionsDisPstFor[For_NUM_TYPES_FORUM] =
  {
   ActDisPstForCrsUsr,
   ActDisPstForCrsTch,
   ActDisPstForDegUsr,
   ActDisPstForDegTch,
   ActDisPstForCtrUsr,
   ActDisPstForCtrTch,
   ActDisPstForInsUsr,
   ActDisPstForInsTch,
   ActDisPstForGenUsr,
   ActDisPstForGenTch,
   ActDisPstForSWAUsr,
   ActDisPstForSWATch,
   ActSeeFor,		// For_FORUM_UNKNOWN
  };

// Links to go to <section>
#define For_REMOVE_THREAD_SECTION_ID	"remove_thread"
#define For_FORUM_THREADS_SECTION_ID	"forum_threads"
#define For_NEW_THREAD_SECTION_ID	"new_thread"
#define For_FORUM_POSTS_SECTION_ID	"thread_posts"
#define For_NEW_POST_SECTION_ID		"new_post"

// Forum images will be saved with:
// - maximum width of For_IMAGE_SAVED_MAX_HEIGHT
// - maximum height of For_IMAGE_SAVED_MAX_HEIGHT
// - maintaining the original aspect ratio (aspect ratio recommended: 3:2)
#define For_IMAGE_SAVED_MAX_WIDTH	768
#define For_IMAGE_SAVED_MAX_HEIGHT	512
#define For_IMAGE_SAVED_QUALITY		 75	// 1 to 100

/*****************************************************************************/
/***************************** Private prototypes ***************************/
/*****************************************************************************/

static bool For_GetIfForumPstExists (long PstCod);

static bool For_GetIfPstIsEnabled (long PstCod);
static void For_DeletePstFromDisabledPstTable (long PstCod);
static void For_InsertPstIntoBannedPstTable (long PstCod);

static long For_InsertForumPst (long ThrCod,long UsrCod,
                                const char *Subject,const char *Content,
                                struct Media *Media);
static bool For_RemoveForumPst (long PstCod,long MedCod);
static unsigned For_NumPstsInThrWithPstCod (long PstCod,long *ThrCod);

static long For_InsertForumThread (long FirstPstCod);
static void For_RemoveThreadOnly (long ThrCod);
static void For_RemoveThreadAndItsPsts (long ThrCod);
static void For_GetThrSubject (long ThrCod,char Subject[Cns_MAX_BYTES_SUBJECT + 1]);

static void For_UpdateThrFirstAndLastPst (long ThrCod,long FirstPstCod,long LastPstCod);
static void For_UpdateThrLastPst (long ThrCod,long LastPstCod);
static long For_GetLastPstCod (long ThrCod);

static void For_UpdateThrReadTime (long ThrCod,
                                   time_t CreatTimeUTCOfTheMostRecentPostRead);
static unsigned For_GetNumOfReadersOfThr (long ThrCod);
static unsigned For_GetNumOfWritersInThr (long ThrCod);
static unsigned For_GetNumPstsInThr (long ThrCod);
static unsigned For_GetNumMyPstInThr (long ThrCod);
static time_t For_GetThrReadTime (long ThrCod);
static void For_DeleteThrFromReadThrs (long ThrCod);
static void For_ShowPostsOfAThread (Ale_AlertType_t AlertType,const char *Message);
static void For_PutIconNewPost (void);
static void For_PutAllHiddenParamsNewPost (void);

static void For_ShowAForumPost (unsigned PstNum,long PstCod,
                                bool LastPst,char LastSubject[Cns_MAX_BYTES_SUBJECT + 1],
                                bool NewPst,bool ICanModerateForum);
static void For_GetPstData (long PstCod,long *UsrCod,time_t *CreatTimeUTC,
                            char Subject[Cns_MAX_BYTES_SUBJECT + 1],
                            char Content[Cns_MAX_BYTES_LONG_TEXT + 1],
                            struct Media *Media);
static void For_WriteNumberOfPosts (long UsrCod);

static void For_PutParamForumSet (For_ForumSet_t ForumSet);
static void For_PutParamForumOrder (For_Order_t Order);
static void For_PutParamForumLocation (long Location);
static void For_PutHiddenParamThrCod (long ThrCod);
static void For_PutHiddenParamPstCod (long PstCod);

static void For_ShowForumList (void);
static void For_PutIconsForums (void);
static void For_PutFormWhichForums (void);

static void For_WriteLinksToGblForums (bool IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS]);
static void For_WriteLinksToPlatformForums (bool IsLastForum,
                                            bool IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS]);
static long For_WriteLinksToInsForums (long InsCod,bool IsLastIns,
                                       bool IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS]);
static long For_WriteLinksToCtrForums (long CtrCod,bool IsLastCtr,
                                       bool IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS]);
static long For_WriteLinksToDegForums (long DegCod,bool IsLastDeg,
                                       bool IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS]);
static long For_WriteLinksToCrsForums (long CrsCod,bool IsLastCrs,
                                       bool IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS]);
static void For_WriteLinkToForum (struct Forum *Forum,
                                  bool Highlight,bool ShowNumOfPosts,
                                  unsigned Level,
                                  bool IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS]);
static unsigned For_GetNumThrsWithNewPstsInForum (struct Forum *Forum,
                                                  unsigned NumThreads);
static unsigned For_GetNumOfThreadsInForumNewerThan (struct Forum *Forum,
                                                     const char *Time);
static unsigned For_GetNumOfUnreadPostsInThr (long ThrCod,unsigned NumPostsInThr);
static unsigned For_GetNumOfPostsInThrNewerThan (long ThrCod,const char *Time);

static void For_WriteNumThrsAndPsts (unsigned NumThrs,unsigned NumThrsWithNewPosts,unsigned NumPosts);
static void For_WriteNumberOfThrs (unsigned NumThrs,unsigned NumThrsWithNewPosts);
static void For_ShowForumThreadsHighlightingOneThread (long ThrCodHighlighted,
                                                       Ale_AlertType_t AlertType,const char *Message);
static void For_PutIconNewThread (void);
static void For_PutAllHiddenParamsNewThread (void);
static unsigned For_GetNumThrsInForum (struct Forum *Forum);
static unsigned For_GetNumPstsInForum (struct Forum *Forum);
static void For_ListForumThrs (long ThrCods[Pag_ITEMS_PER_PAGE],
                               long ThrCodHighlighted,
                               struct Pagination *PaginationThrs);
static void For_GetThrData (struct ForumThread *Thr);

static void For_GetParamsForum (void);
static void For_SetForumType (void);
static void For_RestrictAccess (void);

static void For_WriteFormForumPst (bool IsReply,const char *Subject);

static void For_UpdateNumUsrsNotifiedByEMailAboutPost (long PstCod,unsigned NumUsrsToBeNotifiedByEMail);

static void For_PutAllHiddenParamsRemThread (void);

static bool For_CheckIfICanMoveThreads (void);
static long For_GetThrInMyClipboard (void);
static bool For_CheckIfThrBelongsToForum (long ThrCod,struct Forum *Forum);
static void For_MoveThrToCurrentForum (long ThrCod);
static void For_InsertThrInClipboard (long ThrCod);
static void For_RemoveExpiredThrsClipboards (void);
static void For_RemoveThrCodFromThrClipboard (long ThrCod);

/*****************************************************************************/
/****************************** Enable a forum post **************************/
/*****************************************************************************/

void For_EnablePost (void)
  {
   extern const char *Txt_FORUM_Post_unbanned;

   /***** Get parameters related to forum *****/
   For_GetParamsForum ();

   /***** Delete post from table of disabled posts *****/
   For_DeletePstFromDisabledPstTable (Gbl.Forum.ForumSelected.PstCod);

   /***** Show forum list again *****/
   For_ShowForumList ();

   /***** Show threads again *****/
   For_ShowForumThreadsHighlightingOneThread (Gbl.Forum.ForumSelected.ThrCod,
                                              Ale_SUCCESS,NULL);

   /***** Show the posts again *****/
   For_ShowPostsOfAThread (Ale_SUCCESS,Txt_FORUM_Post_unbanned);
  }

/*****************************************************************************/
/***************************** Disable a forum post **************************/
/*****************************************************************************/

void For_DisablePost (void)
  {
   extern const char *Txt_FORUM_Post_banned;

   /***** Get parameters related to forum *****/
   For_GetParamsForum ();

   /***** Check if post really exists, if it has not been removed *****/
   if (For_GetIfForumPstExists (Gbl.Forum.ForumSelected.PstCod))
     {
      /***** Insert post into table of banned posts *****/
      For_InsertPstIntoBannedPstTable (Gbl.Forum.ForumSelected.PstCod);

      /***** Show forum list again *****/
      For_ShowForumList ();

      /***** Show threads again *****/
      For_ShowForumThreadsHighlightingOneThread (Gbl.Forum.ForumSelected.ThrCod,
						 Ale_SUCCESS,NULL);

      /***** Show the posts again *****/
      For_ShowPostsOfAThread (Ale_SUCCESS,Txt_FORUM_Post_banned);
     }
   else
      Lay_ShowErrorAndExit ("The post to be banned no longer exists.");
  }

/*****************************************************************************/
/******************** Get if a forum post exists in database *****************/
/*****************************************************************************/

static bool For_GetIfForumPstExists (long PstCod)
  {
   /***** Get if a forum post exists from database *****/
   return (DB_QueryCOUNT ("can not check if a post of a forum already existed",
			  "SELECT COUNT(*) FROM forum_post WHERE PstCod=%ld",
			  PstCod) != 0);	// Post exists if it appears in table of forum posts
  }

/*****************************************************************************/
/*********************** Get if a forum post is enabled **********************/
/*****************************************************************************/

static bool For_GetIfPstIsEnabled (long PstCod)
  {
   if (PstCod > 0)
      /***** Get if post is disabled from database *****/
      return (DB_QueryCOUNT ("can not check if a post of a forum is disabled",
			     "SELECT COUNT(*) FROM forum_disabled_post"
			     " WHERE PstCod=%ld",
			     PstCod) == 0);	// Post is enabled if it does not appear in table of disabled posts
   else
      return false;
  }

/*****************************************************************************/
/****************** Delete post from table of disabled posts *****************/
/*****************************************************************************/

static void For_DeletePstFromDisabledPstTable (long PstCod)
  {
   /***** Remove post from disabled posts table *****/
   DB_QueryDELETE ("can not unban a post of a forum",
		   "DELETE FROM forum_disabled_post WHERE PstCod=%ld",
		   PstCod);
  }

/*****************************************************************************/
/****************** Insert post into table of banned posts *******************/
/*****************************************************************************/

static void For_InsertPstIntoBannedPstTable (long PstCod)
  {
   /***** Remove post from banned posts table *****/
   DB_QueryREPLACE ("can not ban a post of a forum",
		    "REPLACE INTO forum_disabled_post"
		    " (PstCod,UsrCod,DisableTime)"
		    " VALUES"
		    " (%ld,%ld,NOW())",
                    PstCod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************** Insert a post new in the table of posts of forums ************/
/*****************************************************************************/

static long For_InsertForumPst (long ThrCod,long UsrCod,
                                const char *Subject,const char *Content,
                                struct Media *Media)
  {
   long PstCod;

   /***** Store media in filesystem and database *****/
   Med_RemoveKeepOrStoreMedia (-1L,Media);

   /***** Insert forum post in the database *****/
   PstCod =
   DB_QueryINSERTandReturnCode ("can not create a new post in a forum",
				"INSERT INTO forum_post"
				" (ThrCod,UsrCod,CreatTime,ModifTime,NumNotif,"
				"Subject,Content,MedCod)"
				" VALUES"
				" (%ld,%ld,NOW(),NOW(),0,"
				"'%s','%s',%ld)",
				ThrCod,UsrCod,
				Subject,Content,Media->MedCod);

   return PstCod;
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

   /***** Delete post from forum post table *****/
   DB_QueryDELETE ("can not remove a post from a forum",
		   "DELETE FROM forum_post WHERE PstCod=%ld",
		   PstCod);

   /***** Delete the post from the table of disabled forum posts *****/
   For_DeletePstFromDisabledPstTable (PstCod);

   /***** Update the last post of the thread *****/
   if (!ThreadDeleted)
      For_UpdateThrLastPst (ThrCod,For_GetLastPstCod (ThrCod));

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

   /***** Get number of posts in the thread that holds a post from database *****/
   DB_QuerySELECT (&mysql_res,"can not get number of posts"
			      " in a thread of a forum",
		   "SELECT COUNT(PstCod),ThrCod FROM forum_post"
		   " WHERE ThrCod IN"
		   " (SELECT ThrCod FROM forum_post"
		   " WHERE PstCod=%ld) GROUP BY ThrCod;",
		   PstCod);

   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumPsts) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of posts in a thread of a forum.");
   if (sscanf (row[1],"%ld",ThrCod) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of posts in a thread of a forum.");
   DB_FreeMySQLResult (&mysql_res);

   return NumPsts;
  }

/*****************************************************************************/
/*************** Insert a new thread in table of forum threads ***************/
/*****************************************************************************/
// Returns the code of the new inserted thread

static long For_InsertForumThread (long FirstPstCod)
  {
   /***** Insert new thread in the database *****/
   return
   DB_QueryINSERTandReturnCode ("can not create a new thread in a forum",
				"INSERT INTO forum_thread"
				" (ForumType,Location,FirstPstCod,LastPstCod)"
				" VALUES"
				" (%u,%ld,%ld,%ld)",
				(unsigned) Gbl.Forum.ForumSelected.Type,
				Gbl.Forum.ForumSelected.Location,
				FirstPstCod,FirstPstCod);
  }

/*****************************************************************************/
/*************** Delete a thread from the forum thread table *****************/
/*****************************************************************************/

static void For_RemoveThreadOnly (long ThrCod)
  {
   /***** Indicate that this thread has not been read by anyone *****/
   For_DeleteThrFromReadThrs (ThrCod);

   /***** Remove thread code from thread clipboard *****/
   For_RemoveThrCodFromThrClipboard (ThrCod);

   /***** Delete thread from forum thread table *****/
   DB_QueryDELETE ("can not remove a thread from a forum",
		   "DELETE FROM forum_thread WHERE ThrCod=%ld",
		   ThrCod);
  }

/*****************************************************************************/
/*************** Delete a thread from the forum thread table *****************/
/*****************************************************************************/

static void For_RemoveThreadAndItsPsts (long ThrCod)
  {
   /***** Delete banned posts in thread *****/
   DB_QueryDELETE ("can not unban the posts of a thread of a forum",
		   "DELETE forum_disabled_post"
		   " FROM forum_post,forum_disabled_post"
		   " WHERE forum_post.ThrCod=%ld"
		   " AND forum_post.PstCod=forum_disabled_post.PstCod",
                   ThrCod);

   /***** Delete thread posts *****/
   DB_QueryDELETE ("can not remove the posts of a thread of a forum",
		   "DELETE FROM forum_post WHERE ThrCod=%ld",
		   ThrCod);

   /***** Delete thread from forum thread table *****/
   For_RemoveThreadOnly (ThrCod);
  }

/*****************************************************************************/
/********************* Get the thread subject from a thread ******************/
/*****************************************************************************/

static void For_GetThrSubject (long ThrCod,char Subject[Cns_MAX_BYTES_SUBJECT + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get subject of a thread from database *****/
   DB_QuerySELECT (&mysql_res,"can not get the subject"
			      " of a thread of a forum",
		   "SELECT forum_post.Subject FROM forum_thread,forum_post"
		   " WHERE forum_thread.ThrCod=%ld"
		   " AND forum_thread.FirstPstCod=forum_post.PstCod",
		   ThrCod);

   /***** Write the subject of the thread *****/
   row = mysql_fetch_row (mysql_res);
   Str_Copy (Subject,row[0],
             Cns_MAX_BYTES_SUBJECT);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************** Get the forum type and location of a post *******************/
/*****************************************************************************/

void For_GetForumTypeAndLocationOfAPost (long PstCod,struct Forum *Forum)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;
   long LongNum;

   /***** Set default forum type and location *****/
   Forum->Type = For_FORUM_UNKNOWN;
   Forum->Location = -1L;

   /***** Check if there is a row with forum type *****/
   if (DB_QuerySELECT (&mysql_res,"can not get forum type and location",
		       "SELECT forum_thread.ForumType,forum_thread.Location"
		       " FROM forum_post,forum_thread"
		       " WHERE forum_post.PstCod=%ld"
		       " AND forum_post.ThrCod=forum_thread.ThrCod",
		       PstCod))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get forum type (row[0]) */
      if (sscanf (row[0],"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < For_NUM_TYPES_FORUM)
	    Forum->Type = (For_ForumType_t) UnsignedNum;

      /* Get forum location (row[1]) */
      if (sscanf (row[1],"%ld",&LongNum) == 1)
         Forum->Location = LongNum;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********* Modify the codes of the first and last posts of a thread **********/
/*****************************************************************************/

static void For_UpdateThrFirstAndLastPst (long ThrCod,long FirstPstCod,long LastPstCod)
  {
   /***** Update the code of the first and last posts of a thread *****/
   DB_QueryUPDATE ("can not update a thread of a forum",
		   "UPDATE forum_thread SET FirstPstCod=%ld,LastPstCod=%ld"
		   " WHERE ThrCod=%ld",
                   FirstPstCod,LastPstCod,ThrCod);
  }

/*****************************************************************************/
/************** Modify the code of the last post of a thread *****************/
/*****************************************************************************/

static void For_UpdateThrLastPst (long ThrCod,long LastPstCod)
  {
   /***** Update the code of the last post of a thread *****/
   DB_QueryUPDATE ("can not update a thread of a forum",
		   "UPDATE forum_thread SET LastPstCod=%ld"
		   " WHERE ThrCod=%ld",
                   LastPstCod,ThrCod);
  }

/*****************************************************************************/
/**************** Get the code of the last post of a thread ******************/
/*****************************************************************************/

static long For_GetLastPstCod (long ThrCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long LastPstCod;

   /***** Get the code of the last post of a thread from database *****/
   DB_QuerySELECT (&mysql_res,"can not get the most recent post"
			      " of a thread of a forum",
		   "SELECT PstCod FROM forum_post"
		   " WHERE ThrCod=%ld ORDER BY CreatTime DESC LIMIT 1",
                   ThrCod);

   /***** Write the subject of the thread *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%ld",&LastPstCod) != 1)
      Lay_ShowErrorAndExit ("Error when getting the most recent post of a thread of a forum.");

   return LastPstCod;
  }

/*****************************************************************************/
/************* Update read date of a thread for the current user *************/
/*****************************************************************************/
// Update forum_thr_read table indicating that this thread page and previous ones
// have been read and have no new posts for the current user
// (even if any previous pages have been no read actually)
// Note that database is not updated with the current time,
// but with the creation time of the most recent post in this thread read by me.

static void For_UpdateThrReadTime (long ThrCod,
                                   time_t CreatTimeUTCOfTheMostRecentPostRead)
  {
   /***** Insert or replace pair ThrCod-UsrCod in forum_thr_read *****/
   DB_QueryREPLACE ("can not update the status of reading"
		    " of a thread of a forum",
		    "REPLACE INTO forum_thr_read"
		    " (ThrCod,UsrCod,ReadTime)"
		    " VALUES"
		    " (%ld,%ld,FROM_UNIXTIME(%ld))",
	            ThrCod,Gbl.Usrs.Me.UsrDat.UsrCod,
	            (long) CreatTimeUTCOfTheMostRecentPostRead);
  }

/*****************************************************************************/
/**************** Get number of users that have read a thread ****************/
/*****************************************************************************/

static unsigned For_GetNumOfReadersOfThr (long ThrCod)
  {
   /***** Get number of distinct readers of a thread from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get the number of readers"
	                     " of a thread of a forum",
			     "SELECT COUNT(*) FROM forum_thr_read"
			     " WHERE ThrCod=%ld",
			     ThrCod);
  }

/*****************************************************************************/
/********** Get number of users that have write posts in a thread ************/
/*****************************************************************************/

static unsigned For_GetNumOfWritersInThr (long ThrCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumWriters;

   /***** Get number of distinct writers in a thread from database *****/
   DB_QuerySELECT (&mysql_res,"can not get the number of writers"
	                      " in a thread of a forum",
		   "SELECT COUNT(DISTINCT UsrCod) FROM forum_post"
		   " WHERE ThrCod=%ld",
                   ThrCod);

   /* Get row with number of writers */
   row = mysql_fetch_row (mysql_res);

   /* Get number of writers (row[0]) */
   if (sscanf (row[0],"%u",&NumWriters) != 1)
      Lay_ShowErrorAndExit ("Error when getting the number of writers in a thread of a forum.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumWriters;
  }

/*****************************************************************************/
/********************** Get number of posts in a thread **********************/
/*****************************************************************************/

static unsigned For_GetNumPstsInThr (long ThrCod)
  {
   /***** Get number of posts in a thread from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get the number of posts"
	                     " in a thread of a forum",
			     "SELECT COUNT(*) FROM forum_post"
			     " WHERE ThrCod=%ld",
			     ThrCod);
  }

/*****************************************************************************/
/************** Get whether there are posts of mine in a thread **************/
/*****************************************************************************/

static unsigned For_GetNumMyPstInThr (long ThrCod)
  {
   /***** Get if I have write posts in a thread from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not check if you have written"
	                     " posts in a thead of a forum",
			     "SELECT COUNT(*) FROM forum_post"
			     " WHERE ThrCod=%ld AND UsrCod=%ld",
			     ThrCod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*********************** Get number of posts from a user *********************/
/*****************************************************************************/

unsigned long For_GetNumPostsUsr (long UsrCod)
  {
   /***** Get number of posts from a user from database *****/
   return DB_QueryCOUNT ("can not get number of forum posts from a user",
			 "SELECT COUNT(*) FROM forum_post"
			 " WHERE UsrCod=%ld",
			 UsrCod);
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
   if (DB_QuerySELECT (&mysql_res,"can not get date of reading"
				  " of a thread of a forum",
		       "SELECT UNIX_TIMESTAMP(ReadTime)"
		       " FROM forum_thr_read"
		       " WHERE ThrCod=%ld AND UsrCod=%ld",
		       ThrCod,Gbl.Usrs.Me.UsrDat.UsrCod))
     {
      /***** There is a row ==> get read time *****/
      row = mysql_fetch_row (mysql_res);

      ReadTimeUTC = Dat_GetUNIXTimeFromStr (row[0]);
     }
   else
      ReadTimeUTC = (time_t) 0;	// If there is no row for this thread and current user, then current user has not read this thread

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return ReadTimeUTC;
  }

/*****************************************************************************/
/********************* Delete thread read status for a thread ****************/
/*****************************************************************************/

static void For_DeleteThrFromReadThrs (long ThrCod)
  {
   /***** Delete pairs ThrCod-UsrCod in forum_thr_read for a thread *****/
   DB_QueryDELETE ("can not remove the status of reading"
		   " of a thread of a forum",
		   "DELETE FROM forum_thr_read WHERE ThrCod=%ld",
		   ThrCod);
  }

/*****************************************************************************/
/********************** Delete thread read status for an user ****************/
/*****************************************************************************/

void For_RemoveUsrFromReadThrs (long UsrCod)
  {
   /***** Delete pairs ThrCod-UsrCod in forum_thr_read for a user *****/
   DB_QueryDELETE ("can not remove the status of reading by a user"
		   " of all the threads of a forum",
		   "DELETE FROM forum_thr_read WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/************************ Show posts in a thread *****************************/
/*****************************************************************************/

static void For_ShowPostsOfAThread (Ale_AlertType_t AlertType,const char *Message)
  {
   extern const char *Hlp_MESSAGES_Forums_posts;
   extern const char *Txt_Thread;
   struct ForumThread Thr;
   char LastSubject[Cns_MAX_BYTES_SUBJECT + 1];
   char FrameTitle[128 + Cns_MAX_BYTES_SUBJECT];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;
   unsigned NumPst = 0;		// Initialized to avoid warning
   unsigned NumPsts;
   time_t ReadTimeUTC;		// Read time of thread for the current user
   time_t CreatTimeUTC;		// Creation time of post
   struct Pagination PaginationPsts;
   long PstCod;
   bool NewPst = false;
   bool ICanModerateForum = false;

   /***** Get data of the thread *****/
   Thr.ThrCod = Gbl.Forum.ForumSelected.ThrCod;
   For_GetThrData (&Thr);

   /***** Get if there is a thread ready to be moved *****/
   if (For_CheckIfICanMoveThreads ())
      Gbl.Forum.ThreadToMove = For_GetThrInMyClipboard ();

   /***** Get thread read time for the current user *****/
   ReadTimeUTC = For_GetThrReadTime (Gbl.Forum.ForumSelected.ThrCod);

   /***** Show alert after action *****/
   HTM_SECTION_Begin (For_FORUM_POSTS_SECTION_ID);
   Ale_ShowAlerts (For_FORUM_POSTS_SECTION_ID);	// Possible pending alerts
   if (Message)
      if (Message[0])
         Ale_ShowAlert (AlertType,Message);

   /***** Begin box *****/
   snprintf (FrameTitle,sizeof (FrameTitle),
	     "%s: %s",
	     Txt_Thread,Thr.Subject);
   Box_BoxBegin (NULL,FrameTitle,For_PutIconNewPost,
                 Hlp_MESSAGES_Forums_posts,Box_NOT_CLOSABLE);

   /***** Get posts of a thread from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get posts of a thread",
			     "SELECT PstCod,UNIX_TIMESTAMP(CreatTime)"
			     " FROM forum_post"
			     " WHERE ThrCod=%ld ORDER BY PstCod",
			     Gbl.Forum.ForumSelected.ThrCod);

   NumPsts = (unsigned) NumRows;
   LastSubject[0] = '\0';
   if (NumPsts)		// If there are posts...
     {
      /***** Check if I can moderate posts in forum *****/
      switch (Gbl.Forum.ForumSelected.Type)
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
         case For_FORUM_CENTRE_USRS:
         case For_FORUM_CENTRE_TCHS:
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
      PaginationPsts.CurrentPage = (int) Gbl.Forum.CurrentPagePsts;
      Pag_CalculatePagination (&PaginationPsts);
      PaginationPsts.Anchor = For_FORUM_POSTS_SECTION_ID;
      Gbl.Forum.CurrentPagePsts = (unsigned) PaginationPsts.CurrentPage;

      /***** Write links to pages *****/
      if (PaginationPsts.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (Pag_POSTS_FORUM,
                                        Gbl.Forum.ForumSelected.ThrCod,
                                        &PaginationPsts);

      /***** Begin table *****/
      HTM_TABLE_BeginWidePadding (2);

      /***** Show posts from this page, the author and the date of last reply *****/
      mysql_data_seek (mysql_res,(my_ulonglong) (PaginationPsts.FirstItemVisible - 1));
      for (NumRow = PaginationPsts.FirstItemVisible;
           NumRow <= PaginationPsts.LastItemVisible;
           NumRow++)
        {
         row = mysql_fetch_row (mysql_res);

         if (sscanf (row[0],"%ld",&PstCod) != 1)
            Lay_ShowErrorAndExit ("Wrong code of post.");

         CreatTimeUTC = Dat_GetUNIXTimeFromStr (row[1]);

         NumPst = (unsigned) NumRow;
         NewPst = (CreatTimeUTC > ReadTimeUTC);

         if (NewPst && NumRow == PaginationPsts.LastItemVisible)
            /* Update forum_thr_read table indicating that this thread page and previous ones
               have been read and have no new posts for the current user
               (even if any previous pages have been no read actually).
               Note that database is not updated with the current time,
               but with the creation time of the most recent post
               in this page of threads. */
            For_UpdateThrReadTime (Gbl.Forum.ForumSelected.ThrCod,
                                   CreatTimeUTC);

         /* Show post */
         For_ShowAForumPost (NumPst,PstCod,
                             (NumRow == NumRows),LastSubject,
                             NewPst,ICanModerateForum);

         /* Mark possible notification as seen */
         switch (Gbl.Forum.ForumSelected.Type)
           {
            case For_FORUM_COURSE_TCHS:
            case For_FORUM_COURSE_USRS:
               Ntf_MarkNotifAsSeen (Ntf_EVENT_FORUM_POST_COURSE,
           	                    PstCod,Gbl.Hierarchy.Crs.CrsCod,
           	                    Gbl.Usrs.Me.UsrDat.UsrCod);
               break;
            default:
               break;
           }
         if (Thr.NumMyPosts)
            Ntf_MarkNotifAsSeen (Ntf_EVENT_FORUM_REPLY,
        	                 PstCod,-1L,
        	                 Gbl.Usrs.Me.UsrDat.UsrCod);
        }

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Write again links to pages *****/
      if (PaginationPsts.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (Pag_POSTS_FORUM,
                                        Gbl.Forum.ForumSelected.ThrCod,
                                        &PaginationPsts);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Form to write a new post in the thread *****/
   HTM_SECTION_Begin (For_NEW_POST_SECTION_ID);
   For_WriteFormForumPst (true,LastSubject);
   HTM_SECTION_End ();

   /***** End box *****/
   Box_BoxEnd ();
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/*********************** Put icon to write a new post ************************/
/*****************************************************************************/

static void For_PutIconNewPost (void)
  {
   extern const char *Txt_New_post;

   Ico_PutContextualIconToAdd (For_ActionsSeePstFor[Gbl.Forum.ForumSelected.Type],
			       For_NEW_POST_SECTION_ID,
			       For_PutAllHiddenParamsNewPost,
			       Txt_New_post);
  }

static void For_PutAllHiddenParamsNewPost (void)
  {
   For_PutAllHiddenParamsForum (Gbl.Forum.CurrentPageThrs,	// Page of threads = current
                                UINT_MAX,			// Page of posts   = last
                                Gbl.Forum.ForumSet,
                                Gbl.Forum.ThreadsOrder,
                                Gbl.Forum.ForumSelected.Location,
                                Gbl.Forum.ForumSelected.ThrCod,
                                -1L);
  }

/*****************************************************************************/
/**************************** Show a post from forum *************************/
/*****************************************************************************/

static void For_ShowAForumPost (unsigned PstNum,long PstCod,
                                bool LastPst,char LastSubject[Cns_MAX_BYTES_SUBJECT + 1],
                                bool NewPst,bool ICanModerateForum)
  {
   extern const char *Txt_MSG_New;
   extern const char *Txt_MSG_Open;
   extern const char *Txt_no_subject;
   extern const char *Txt_FORUM_Post_X_allowed;
   extern const char *Txt_FORUM_Post_banned;
   extern const char *Txt_FORUM_Post_X_banned;
   extern const char *Txt_FORUM_Post_X_allowed_Click_to_ban_it;
   extern const char *Txt_FORUM_Post_X_banned_Click_to_unban_it;
   extern const char *Txt_This_post_has_been_banned_probably_for_not_satisfy_the_rules_of_the_forums;
   struct UsrData UsrDat;
   time_t CreatTimeUTC;	// Creation time of a post
   char OriginalContent[Cns_MAX_BYTES_LONG_TEXT + 1];
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Content[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Media Media;
   bool Enabled;
   bool ItsMe;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Initialize image *****/
   Med_MediaConstructor (&Media);

   /***** Check if post is enabled *****/
   Enabled = For_GetIfPstIsEnabled (PstCod);

   /***** Get data of post *****/
   For_GetPstData (PstCod,&UsrDat.UsrCod,&CreatTimeUTC,
                   Subject,OriginalContent,&Media);

   if (Enabled)
      /* Return this subject as last subject */
      Str_Copy (LastSubject,Subject,
                Cns_MAX_BYTES_SUBJECT);

   HTM_TR_Begin (NULL);

   /***** Put an icon with post status *****/
   HTM_TD_Begin ("class=\"CONTEXT_COL %s\"",
	         NewPst ? "MSG_TIT_BG_NEW" :
			  "MSG_TIT_BG");
   Ico_PutIcon (NewPst ? "envelope.svg" :
        	         "envelope-open-text.svg",
		NewPst ? Txt_MSG_New :
        	         Txt_MSG_Open,
	        "ICO16x16");
   HTM_TD_End ();

   /***** Write post number *****/
   Msg_WriteMsgNumber ((unsigned long) PstNum,NewPst);

   /***** Write date *****/
   Msg_WriteMsgDate (CreatTimeUTC,NewPst ? "MSG_TIT_BG_NEW" :
	                                   "MSG_TIT_BG");

   /***** Write subject *****/
   HTM_TD_Begin ("class=\"%s LT\"",NewPst ? "MSG_TIT_BG_NEW" :
        					  "MSG_TIT_BG");
   if (Enabled)
     {
      if (Subject[0])
         fprintf (Gbl.F.Out,"%s",Subject);
      else
         fprintf (Gbl.F.Out,"[%s]",Txt_no_subject);
     }
   else
      fprintf (Gbl.F.Out,"[%s]",Txt_FORUM_Post_banned);
   HTM_TD_End ();
   HTM_TR_End ();

   /***** Form to ban/unban post *****/
   HTM_TR_Begin (NULL);
   HTM_TD_Begin ("class=\"CONTEXT_COL\"");
   if (ICanModerateForum)
     {
      Frm_StartFormAnchor (Enabled ? For_ActionsDisPstFor[Gbl.Forum.ForumSelected.Type] :
				     For_ActionsEnbPstFor[Gbl.Forum.ForumSelected.Type],
			   For_FORUM_POSTS_SECTION_ID);
      For_PutAllHiddenParamsForum (Gbl.Forum.CurrentPageThrs,	// Page of threads = current
                                   Gbl.Forum.CurrentPagePsts,	// Page of posts   = current
                                   Gbl.Forum.ForumSet,
				   Gbl.Forum.ThreadsOrder,
				   Gbl.Forum.ForumSelected.Location,
				   Gbl.Forum.ForumSelected.ThrCod,
				   PstCod);

      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        Enabled ? Txt_FORUM_Post_X_allowed_Click_to_ban_it :
			  Txt_FORUM_Post_X_banned_Click_to_unban_it,
	        PstNum);
      Ico_PutIconLink (Enabled ? "eye.svg" :
			         "eye-slash.svg",
	               Gbl.Title);
      Frm_EndForm ();
     }
   else
     {
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        Enabled ? Txt_FORUM_Post_X_allowed :
			  Txt_FORUM_Post_X_banned,
	        PstNum);
      fprintf (Gbl.F.Out,"<span title=\"%s\">",Gbl.Title);	// TODO: Remove?
      Ico_PutIcon (Enabled ? "eye.svg" :
			     "eye-slash.svg",
	           Gbl.Title,"ICO_HIDDEN ICO16x16");
      fprintf (Gbl.F.Out,"</span>");				// TODO: Remove?
     }

   /***** Form to remove post *****/
   if (LastPst)
     {
      ItsMe = Usr_ItsMe (UsrDat.UsrCod);
      if (ItsMe)
	{
	 // Post can be removed if post is the last (without answers) and it's mine
	 if (PstNum == 1)	// First and unique post in thread
	    Frm_StartFormAnchor (For_ActionsDelPstFor[Gbl.Forum.ForumSelected.Type],
				 For_FORUM_THREADS_SECTION_ID);
	 else		// Last of several posts in thread
	    Frm_StartFormAnchor (For_ActionsDelPstFor[Gbl.Forum.ForumSelected.Type],
				 For_FORUM_POSTS_SECTION_ID);
	 For_PutAllHiddenParamsForum (Gbl.Forum.CurrentPageThrs,	// Page of threads = current
				      Gbl.Forum.CurrentPagePsts,	// Page of posts   = current
				      Gbl.Forum.ForumSet,
				      Gbl.Forum.ThreadsOrder,
				      Gbl.Forum.ForumSelected.Location,
				      Gbl.Forum.ForumSelected.ThrCod,
				      PstCod);
	 Ico_PutIconRemove ();
	 Frm_EndForm ();
	}
     }
   HTM_TD_End ();

   /***** Write author *****/
   Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);
   HTM_TD_Begin ("colspan=\"2\" class=\"AUTHOR_TXT LT\" style=\"width:150px;\"");
   Msg_WriteMsgAuthor (&UsrDat,Enabled,NULL);
   if (Enabled)
      /* Write number of posts from this user */
      For_WriteNumberOfPosts (UsrDat.UsrCod);
   HTM_TD_End ();

   /***** Write post content *****/
   HTM_TD_Begin ("class=\"MSG_TXT LT\"");
   if (Enabled)
     {
      Str_Copy (Content,OriginalContent,
                Cns_MAX_BYTES_LONG_TEXT);
      Msg_WriteMsgContent (Content,Cns_MAX_BYTES_LONG_TEXT,true,false);

      /***** Show image *****/
      Med_ShowMedia (&Media,"FOR_IMG_CONTAINER","FOR_IMG");
     }
   else
      fprintf (Gbl.F.Out,"%s",Txt_This_post_has_been_banned_probably_for_not_satisfy_the_rules_of_the_forums);
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
                            char Subject[Cns_MAX_BYTES_SUBJECT + 1],
                            char Content[Cns_MAX_BYTES_LONG_TEXT + 1],
                            struct Media *Media)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;

   /***** Get data of a post from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get data of a post",
			     "SELECT UsrCod,"				// row[0]
			            "UNIX_TIMESTAMP(CreatTime),"	// row[1]
			            "Subject,"				// row[2]
			            "Content,"				// row[3]
			            "MedCod"				// row[4]
			     " FROM forum_post WHERE PstCod=%ld",
			     PstCod);

   /***** Result should have a unique row *****/
   if (NumRows != 1)
      Lay_ShowErrorAndExit ("Internal error in database when getting data of a post.");

   /***** Get number of rows *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get author code (row[1]) *****/
   *UsrCod = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get creation time (row[1]) *****/
   *CreatTimeUTC = Dat_GetUNIXTimeFromStr (row[1]);

   /***** Get subject (row[2]) *****/
   Str_Copy (Subject,row[2],
             Cns_MAX_BYTES_SUBJECT);

   /***** Get location (row[3]) *****/
   Str_Copy (Content,row[3],
             Cns_MAX_BYTES_LONG_TEXT);

   /***** Get media (row[4]) *****/
   Media->MedCod = Str_ConvertStrCodToLongCod (row[4]);
   Med_GetMediaDataByCod (Media);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Get summary and content for a forum post ******************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error

void For_GetSummaryAndContentForumPst (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                       char **ContentStr,
                                       long PstCod,bool GetContent)
  {
   char *Query = NULL;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   size_t Length;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Get subject of message from database *****/
   if (asprintf (&Query,"SELECT Subject,Content FROM forum_post"
                        " WHERE PstCod=%ld",PstCod) < 0)
      Lay_NotEnoughMemoryExit ();
   if (!mysql_query (&Gbl.mysql,Query))
      if ((mysql_res = mysql_store_result (&Gbl.mysql)) != NULL)
        {
         /***** Result should have a unique row *****/
         if (mysql_num_rows (mysql_res) == 1)
           {
            /***** Get subject and content of the message *****/
            row = mysql_fetch_row (mysql_res);

            /***** Copy subject *****/
            // TODO: Do only direct copy when Subject will be VARCHAR(255)
            if (strlen (row[0]) > Ntf_MAX_BYTES_SUMMARY)
              {
               strncpy (SummaryStr,row[0],
			Ntf_MAX_BYTES_SUMMARY);
               SummaryStr[Ntf_MAX_BYTES_SUMMARY] = '\0';
              }
            else
	       Str_Copy (SummaryStr,row[0],
			 Ntf_MAX_BYTES_SUMMARY);

            /***** Copy content *****/
            if (GetContent)
              {
               Length = strlen (row[1]);

               if ((*ContentStr = (char *) malloc (Length + 1)) == NULL)
                  Lay_ShowErrorAndExit ("Error allocating memory for notification content.");

               if (Length)
                  Str_Copy (*ContentStr,row[1],
                            Length);
               else
        	  **ContentStr = '\0';
              }
           }
         mysql_free_result (mysql_res);
        }
  }

/*****************************************************************************/
/*************** Write number of posts in a forum of an user *****************/
/*****************************************************************************/

static void For_WriteNumberOfPosts (long UsrCod)
  {
   extern const char *Txt_FORUM_post;
   extern const char *Txt_FORUM_posts;
   char SubQuery[256];
   unsigned NumPsts;

   /***** Star table cell *****/
   HTM_DIV_Begin ("class=\"AUTHOR_TXT LT\"");

   /***** Get number of posts from database *****/
   if (Gbl.Forum.ForumSelected.Location > 0)
      sprintf (SubQuery," AND forum_thread.Location=%ld",
               Gbl.Forum.ForumSelected.Location);
   else
      SubQuery[0] = '\0';
   NumPsts =
   (unsigned) DB_QueryCOUNT ("can not get the number of posts of a user"
			     " in a forum",
			     "SELECT COUNT(*) FROM forum_post,forum_thread"
			     " WHERE forum_post.UsrCod=%ld"
			     " AND forum_post.ThrCod=forum_thread.ThrCod"
			     " AND forum_thread.ForumType=%u%s",
			     UsrCod,
			     (unsigned) Gbl.Forum.ForumSelected.Type,SubQuery);

   /***** Write number of threads and number of posts *****/
   if (NumPsts == 1)
      fprintf (Gbl.F.Out,"[1 %s]",Txt_FORUM_post);
   else
      fprintf (Gbl.F.Out,"[%u %s]",NumPsts,Txt_FORUM_posts);

   /***** End table cell *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************ Put all the hidden parameters related to forums ****************/
/*****************************************************************************/

void For_PutAllHiddenParamsForum (unsigned NumPageThreads,
                                  unsigned NumPagePosts,
                                  For_ForumSet_t ForumSet,
                                  For_Order_t Order,
                                  long Location,
                                  long ThrCod,
                                  long PstCod)
  {
   Pag_PutHiddenParamPagNum (Pag_THREADS_FORUM,NumPageThreads);
   Pag_PutHiddenParamPagNum (Pag_POSTS_FORUM,NumPagePosts);
   For_PutParamForumSet (ForumSet);
   For_PutParamForumOrder (Order);
   For_PutParamForumLocation (Location);
   For_PutHiddenParamThrCod (ThrCod);
   For_PutHiddenParamPstCod (PstCod);
  }

/*****************************************************************************/
/********* Put a hidden parameter with set of forums I want to see ***********/
/*****************************************************************************/

static void For_PutParamForumSet (For_ForumSet_t ForumSet)
  {
   Par_PutHiddenParamUnsigned ("ForumSet",(unsigned) ForumSet);
  }

/*****************************************************************************/
/******** Put a hidden parameter with the order criterium for forums *********/
/*****************************************************************************/

static void For_PutParamForumOrder (For_Order_t Order)
  {
   Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
  }

/*****************************************************************************/
/************** Put hidden parameter with code of            *****************/
/************** forum institution, centre, degree and course *****************/
/*****************************************************************************/

static void For_PutParamForumLocation (long Location)
  {
   if (Location > 0)
      /***** Put a hidden parameter with the
             institution, centre, degree or course of the forum *****/
      Par_PutHiddenParamLong ("Location",Location);
  }

/*****************************************************************************/
/************ Write a form parameter to specify a thread code ****************/
/*****************************************************************************/

static void For_PutHiddenParamThrCod (long ThrCod)
  {
   if (ThrCod > 0)
      Par_PutHiddenParamLong ("ThrCod",ThrCod);
  }

/*****************************************************************************/
/************* Write a form parameter to specify a post code *****************/
/*****************************************************************************/

static void For_PutHiddenParamPstCod (long PstCod)
  {
   if (PstCod > 0)
      Par_PutHiddenParamLong ("PstCod",PstCod);
  }

/*****************************************************************************/
/************************** Show list of available forums ********************/
/*****************************************************************************/

static void For_ShowForumList (void)
  {
   extern const char *Hlp_MESSAGES_Forums;
   extern const char *Txt_Forums;
   bool IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS];
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
      Gbl.Forum.ThreadToMove = For_GetThrInMyClipboard ();

   /***** Fill the list with the institutions I belong to *****/
   Usr_GetMyInstits ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Forums,For_PutIconsForums,
                 Hlp_MESSAGES_Forums,Box_NOT_CLOSABLE);

   /***** Put a form to select which forums *****/
   For_PutFormWhichForums ();

   /***** Start list *****/
   HTM_UL_Begin ("class=\"LIST_TREE\"");

   /***** Links to global forums *****/
   For_WriteLinksToGblForums (IsLastItemInLevel);
   switch (Gbl.Forum.ForumSet)
     {
      case For_ONLY_CURRENT_FORUMS:
	 if (Gbl.Hierarchy.Ins.InsCod > 0)
	   {
	    if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
	       ICanSeeInsForum = true;
	    else
	       ICanSeeInsForum = Usr_CheckIfIBelongToIns (Gbl.Hierarchy.Ins.InsCod);
	   }
	 else
	    ICanSeeInsForum = false;

         /***** Links to forums about the platform *****/
         For_WriteLinksToPlatformForums (!ICanSeeInsForum,IsLastItemInLevel);

         if (ICanSeeInsForum)
           {
            if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
	       ICanSeeCtrForum = true;
	    else
	       ICanSeeCtrForum = Usr_CheckIfIBelongToCtr (Gbl.Hierarchy.Ctr.CtrCod);

	    /***** Links to forums of current institution *****/
	    if (For_WriteLinksToInsForums (Gbl.Hierarchy.Ins.InsCod,
	                                   true,
	                                   IsLastItemInLevel) > 0)
               if (ICanSeeCtrForum)
        	 {
        	  if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
		     ICanSeeDegForum = true;
		  else
		     ICanSeeDegForum = Usr_CheckIfIBelongToDeg (Gbl.Hierarchy.Deg.DegCod);

		  /***** Links to forums of current centre *****/
		  if (For_WriteLinksToCtrForums (Gbl.Hierarchy.Ctr.CtrCod,
		                                 true,
		                                 IsLastItemInLevel) > 0)
		     if (ICanSeeDegForum)
			/***** Links to forums of current degree *****/
			if (For_WriteLinksToDegForums (Gbl.Hierarchy.Deg.DegCod,
			                               true,
			                               IsLastItemInLevel) > 0)
			   if (Gbl.Usrs.Me.IBelongToCurrentCrs ||
			       Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
			      /***** Links to forums of current degree *****/
			      For_WriteLinksToCrsForums (Gbl.Hierarchy.Crs.CrsCod,
			                                 true,
			                                 IsLastItemInLevel);
        	 }
           }
         break;
      case For_ALL_MY_FORUMS:
         /***** Links to forums about the platform *****/
         For_WriteLinksToPlatformForums ((Gbl.Usrs.Me.MyInss.Num == 0),IsLastItemInLevel);

         /***** Links to forums of users from my institutions, the degrees in each institution and the courses in each degree *****/
         for (NumMyIns = 0;
              NumMyIns < Gbl.Usrs.Me.MyInss.Num;
              NumMyIns++)
           {
            InsCod = Gbl.Usrs.Me.MyInss.Inss[NumMyIns].InsCod;

            /* Links to forums of this institution */
            For_WriteLinksToInsForums (InsCod,
                                       (NumMyIns == Gbl.Usrs.Me.MyInss.Num - 1),
                                       IsLastItemInLevel);

            /* Get my centres in this institution from database */
            if ((NumCtrs = Usr_GetCtrsFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,
                                               InsCod,
                                               &mysql_resCtr)) > 0) // Centres found in this institution
               for (NumCtr = 0;
        	    NumCtr < NumCtrs;
        	    NumCtr++)
                 {
                  /* Get next centre */
                  row = mysql_fetch_row (mysql_resCtr);
                  CtrCod = Str_ConvertStrCodToLongCod (row[0]);

                  /* Links to forums of this centre */
                  if (For_WriteLinksToCtrForums (CtrCod,
                                                 (NumCtr == NumCtrs - 1),
                                                 IsLastItemInLevel) > 0)
                    {
		     /* Get my degrees in this institution from database */
		     if ((NumDegs = Usr_GetDegsFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,
		                                        CtrCod,
		                                        &mysql_resDeg)) > 0) // Degrees found in this centre
			for (NumDeg = 0;
			     NumDeg < NumDegs;
			     NumDeg++)
			  {
			   /* Get next degree */
			   row = mysql_fetch_row (mysql_resDeg);
                           DegCod = Str_ConvertStrCodToLongCod (row[0]);

			   /* Links to forums of this degree */
			   if (For_WriteLinksToDegForums (DegCod,
			                                  (NumDeg == NumDegs - 1),
			                                  IsLastItemInLevel) > 0)
			     {
			      /* Get my courses in this degree from database */
			      if ((NumCrss = Usr_GetCrssFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,
			                                         DegCod,
			                                         &mysql_resCrs)) > 0) // Courses found in this degree
				 for (NumCrs = 0;
				      NumCrs < NumCrss;
				      NumCrs++)
				   {
				    /* Get next course */
				    row = mysql_fetch_row (mysql_resCrs);
                                    CrsCod = Str_ConvertStrCodToLongCod (row[0]);

				    /* Links to forums of this course */
				    For_WriteLinksToCrsForums (CrsCod,
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

static void For_PutIconsForums (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_FORUMS;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/*************** Put form to select which forums I want to see ***************/
/*****************************************************************************/

static void For_PutFormWhichForums (void)
  {
   extern const char *Txt_FORUM_WHICH_FORUM[For_NUM_FORUM_SETS];
   For_ForumSet_t ForumSet;

   /***** Form to select which forums I want to see:
          - all my forums
          - only the forums of current institution/degree/course *****/
   Frm_StartForm (ActSeeFor);
   For_PutParamForumOrder (Gbl.Forum.ThreadsOrder);
   HTM_DIV_Begin ("class=\"SEL_BELOW_TITLE\"");
   HTM_UL_Begin (NULL);

   for (ForumSet = (For_ForumSet_t) 0;
	ForumSet < For_NUM_FORUM_SETS;
	ForumSet++)
     {
      HTM_LI_Begin (NULL);
      fprintf (Gbl.F.Out,"<label>"
                         "<input type=\"radio\" name=\"ForumSet\""
                         " value=\"%u\"",
               (unsigned) ForumSet);
      if (ForumSet == Gbl.Forum.ForumSet)
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," onclick=\"document.getElementById('%s').submit();\" />"
	                 "%s"
                         "</label>",
               Gbl.Form.Id,Txt_FORUM_WHICH_FORUM[ForumSet]);
      HTM_LI_End ();
     }
   HTM_UL_End ();
   HTM_DIV_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************* Write links to global forums **********************/
/*****************************************************************************/

static void For_WriteLinksToGblForums (bool IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS])
  {
   bool Highlight;
   bool ICanSeeTeacherForum;
   struct Forum Forum;

   /***** Can I see teachers's forums? *****/
   Rol_GetRolesInAllCrssIfNotYetGot (&Gbl.Usrs.Me.UsrDat);
   ICanSeeTeacherForum = Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
	                 (Gbl.Usrs.Me.UsrDat.Roles.InCrss & ((1 << Rol_NET) |
	                                                     (1 << Rol_TCH)));

   /***** Link to forum global *****/
   Forum.Type = For_FORUM_GLOBAL_USRS;
   Forum.Location = -1L;
   Highlight = (Gbl.Forum.ForumSelected.Type == For_FORUM_GLOBAL_USRS);
   IsLastItemInLevel[1] = false;
   For_WriteLinkToForum (&Forum,Highlight,false,0,IsLastItemInLevel);

   /***** Link to forum of teachers global *****/
   Rol_GetRolesInAllCrssIfNotYetGot (&Gbl.Usrs.Me.UsrDat);
   if (ICanSeeTeacherForum)
     {
      Forum.Type = For_FORUM_GLOBAL_TCHS;
      Forum.Location = -1L;
      Highlight = (Gbl.Forum.ForumSelected.Type == For_FORUM_GLOBAL_TCHS);
      IsLastItemInLevel[1] = false;
      For_WriteLinkToForum (&Forum,Highlight,false,0,IsLastItemInLevel);
     }
  }

/*****************************************************************************/
/****************** Write links to forums about the platform *****************/
/*****************************************************************************/

static void For_WriteLinksToPlatformForums (bool IsLastForum,
                                            bool IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS])
  {
   bool Highlight;
   bool ICanSeeTeacherForum;
   struct Forum Forum;

   /***** Can I see teachers's forums? *****/
   Rol_GetRolesInAllCrssIfNotYetGot (&Gbl.Usrs.Me.UsrDat);
   ICanSeeTeacherForum = Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
	                 (Gbl.Usrs.Me.UsrDat.Roles.InCrss & ((1 << Rol_NET) |
	                                                     (1 << Rol_TCH)));

   /***** Link to forum of users about the platform *****/
   Forum.Type = For_FORUM__SWAD__USRS;
   Forum.Location = -1L;
   Highlight = (Gbl.Forum.ForumSelected.Type == For_FORUM__SWAD__USRS);
   IsLastItemInLevel[1] = (IsLastForum && !ICanSeeTeacherForum);
   For_WriteLinkToForum (&Forum,Highlight,false,0,IsLastItemInLevel);

   /***** Link to forum of teachers about the platform *****/
   if (ICanSeeTeacherForum)
     {
      Forum.Type = For_FORUM__SWAD__TCHS;
      Forum.Location = -1L;
      Highlight = (Gbl.Forum.ForumSelected.Type == For_FORUM__SWAD__TCHS);
      IsLastItemInLevel[1] = IsLastForum;
      For_WriteLinkToForum (&Forum,Highlight,false,0,IsLastItemInLevel);
     }
  }

/*****************************************************************************/
/********************** Write links to institution forums ********************/
/*****************************************************************************/
// Returns institution code

static long For_WriteLinksToInsForums (long InsCod,bool IsLastIns,
                                       bool IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS])
  {
   bool Highlight;
   Rol_Role_t MaxRoleInIns;
   bool ICanSeeTeacherForum;
   struct Forum Forum;

   if (InsCod > 0)
     {
      MaxRoleInIns = Rol_GetMyMaxRoleInIns (InsCod);
      ICanSeeTeacherForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
	                     MaxRoleInIns == Rol_NET ||
	                     MaxRoleInIns == Rol_TCH);

      /***** Link to the forum of users from this institution *****/
      Forum.Type = For_FORUM_INSTIT_USRS;
      Forum.Location = InsCod;
      Highlight = (Gbl.Forum.ForumSelected.Type == For_FORUM_INSTIT_USRS &&
	           Gbl.Forum.ForumSelected.Location == InsCod);
      IsLastItemInLevel[1] = (IsLastIns && !ICanSeeTeacherForum);
      For_WriteLinkToForum (&Forum,Highlight,false,1,IsLastItemInLevel);

      /***** Link to forum of teachers from this institution *****/
      if (ICanSeeTeacherForum)
        {
	 Forum.Type = For_FORUM_INSTIT_TCHS;
	 Forum.Location = InsCod;
	 Highlight = (Gbl.Forum.ForumSelected.Type == For_FORUM_INSTIT_TCHS &&
		      Gbl.Forum.ForumSelected.Location == InsCod);
         IsLastItemInLevel[1] = IsLastIns;
         For_WriteLinkToForum (&Forum,Highlight,false,1,IsLastItemInLevel);
        }
     }
   return InsCod;
  }

/*****************************************************************************/
/************************ Write links to centre forums ***********************/
/*****************************************************************************/
// Returns centre code

static long For_WriteLinksToCtrForums (long CtrCod,bool IsLastCtr,
                                       bool IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS])
  {
   bool Highlight;
   Rol_Role_t MaxRoleInCtr;
   bool ICanSeeTeacherForum;
   struct Forum Forum;

   if (CtrCod > 0)
     {
      MaxRoleInCtr = Rol_GetMyMaxRoleInCtr (CtrCod);
      ICanSeeTeacherForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
	                     MaxRoleInCtr == Rol_NET ||
	                     MaxRoleInCtr == Rol_TCH);

      /***** Link to the forum of users from this centre *****/
      Forum.Type = For_FORUM_CENTRE_USRS;
      Forum.Location = CtrCod;
      Highlight = (Gbl.Forum.ForumSelected.Type == For_FORUM_CENTRE_USRS &&
	           Gbl.Forum.ForumSelected.Location == CtrCod);
      IsLastItemInLevel[2] = (IsLastCtr && !ICanSeeTeacherForum);
      For_WriteLinkToForum (&Forum,Highlight,false,2,IsLastItemInLevel);

      /***** Link to forum of teachers from this centre *****/
      if (ICanSeeTeacherForum)
        {
	 Forum.Type = For_FORUM_CENTRE_TCHS;
	 Forum.Location = CtrCod;
	 Highlight = (Gbl.Forum.ForumSelected.Type == For_FORUM_CENTRE_TCHS &&
		      Gbl.Forum.ForumSelected.Location == CtrCod);
         IsLastItemInLevel[2] = IsLastCtr;
         For_WriteLinkToForum (&Forum,Highlight,false,2,IsLastItemInLevel);
        }
     }
   return CtrCod;
  }

/*****************************************************************************/
/************************ Write links to degree forums ***********************/
/*****************************************************************************/
// Returns degree code

static long For_WriteLinksToDegForums (long DegCod,bool IsLastDeg,
                                       bool IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS])
  {
   bool Highlight;
   Rol_Role_t MaxRoleInDeg;
   bool ICanSeeTeacherForum;
   struct Forum Forum;

   if (DegCod > 0)
     {
      MaxRoleInDeg = Rol_GetMyMaxRoleInDeg (DegCod);
      ICanSeeTeacherForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
	                     MaxRoleInDeg == Rol_NET ||
	                     MaxRoleInDeg == Rol_TCH);

      /***** Link to the forum of users from this degree *****/
      Forum.Type = For_FORUM_DEGREE_USRS;
      Forum.Location = DegCod;
      Highlight = (Gbl.Forum.ForumSelected.Type == For_FORUM_DEGREE_USRS &&
	           Gbl.Forum.ForumSelected.Location == DegCod);
      IsLastItemInLevel[3] = (IsLastDeg && !ICanSeeTeacherForum);
      For_WriteLinkToForum (&Forum,Highlight,false,3,IsLastItemInLevel);

      /***** Link to forum of teachers from this degree *****/
      if (ICanSeeTeacherForum)
        {
	 Forum.Type = For_FORUM_DEGREE_TCHS;
	 Forum.Location = DegCod;
	 Highlight = (Gbl.Forum.ForumSelected.Type == For_FORUM_DEGREE_TCHS &&
		      Gbl.Forum.ForumSelected.Location == DegCod);
	 IsLastItemInLevel[3] = IsLastDeg;
         For_WriteLinkToForum (&Forum,Highlight,false,3,IsLastItemInLevel);
        }
     }
   return DegCod;
  }

/*****************************************************************************/
/************************ Write links to course forums ***********************/
/*****************************************************************************/
// Returns course code

static long For_WriteLinksToCrsForums (long CrsCod,bool IsLastCrs,
                                       bool IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS])
  {
   bool Highlight;
   Rol_Role_t MyRoleInCrs;
   bool ICanSeeTeacherForum;
   struct Forum Forum;

   if (CrsCod > 0)
     {
      MyRoleInCrs = Rol_GetMyRoleInCrs (CrsCod);
      ICanSeeTeacherForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
	                     MyRoleInCrs == Rol_NET ||
	                     MyRoleInCrs == Rol_TCH);

      /***** Link to the forum of users from this course *****/
      Forum.Type = For_FORUM_COURSE_USRS;
      Forum.Location = CrsCod;
      Highlight = (Gbl.Forum.ForumSelected.Type == For_FORUM_COURSE_USRS &&
	           Gbl.Forum.ForumSelected.Location == CrsCod);
      IsLastItemInLevel[4] = (IsLastCrs && !ICanSeeTeacherForum);
      For_WriteLinkToForum (&Forum,Highlight,false,4,IsLastItemInLevel);

      /***** Link to forum of teachers from this course *****/
      if (ICanSeeTeacherForum)
        {
	 Forum.Type = For_FORUM_COURSE_TCHS;
	 Forum.Location = CrsCod;
	 Highlight = (Gbl.Forum.ForumSelected.Type == For_FORUM_COURSE_TCHS &&
		      Gbl.Forum.ForumSelected.Location == CrsCod);
         IsLastItemInLevel[4] = IsLastCrs;
         For_WriteLinkToForum (&Forum,Highlight,false,4,IsLastItemInLevel);
        }
     }
   return CrsCod;
  }

/*****************************************************************************/
/********************** Write title and link to a forum **********************/
/*****************************************************************************/

static void For_WriteLinkToForum (struct Forum *Forum,
                                   bool Highlight,bool ShowNumOfPosts,
                                   unsigned Level,
                                   bool IsLastItemInLevel[1 + For_FORUM_MAX_LEVELS])
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *The_ClassFormInBoxBold[The_NUM_THEMES];
   extern const char *Txt_Copy_not_allowed;
   unsigned NumThrs;
   unsigned NumThrsWithNewPosts;
   unsigned NumPosts;
   char ActTxt[Act_MAX_BYTES_ACTION_TXT + 1];
   const char *Style;
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];

   /***** Get number of threads and number of posts *****/
   NumThrs = For_GetNumThrsInForum (Forum);
   NumThrsWithNewPosts = For_GetNumThrsWithNewPstsInForum (Forum,NumThrs);
   Style = (NumThrsWithNewPosts ? The_ClassFormInBoxBold[Gbl.Prefs.Theme] :
	                          The_ClassFormInBox[Gbl.Prefs.Theme]);

   /***** Start row *****/
   HTM_LI_Begin (Highlight ? "class=\"LIGHT_BLUE\"" :
			     NULL);

   /***** Indent forum title *****/
   Lay_IndentDependingOnLevel (Level,IsLastItemInLevel);

   /***** Write paste button used to move a thread in clipboard to this forum *****/
   if (Gbl.Forum.ThreadToMove >= 0) // If I have permission to paste threads and there is a thread ready to be pasted...
     {
      /* Check if thread to move is yet in current forum */
      if (For_CheckIfThrBelongsToForum (Gbl.Forum.ThreadToMove,Forum))
         Ico_PutIcon ("paste.svg",Txt_Copy_not_allowed,"CONTEXT_OPT ICO_HIDDEN ICO16x16");
      else
        {
         Frm_StartFormAnchor (For_ActionsPasThrFor[Forum->Type],
                              For_FORUM_THREADS_SECTION_ID);
	 For_PutAllHiddenParamsForum (1,	// Page of threads = first
                                      1,	// Page of posts   = first
                                      Gbl.Forum.ForumSet,
				      Gbl.Forum.ThreadsOrder,
				      Forum->Location,
				      Gbl.Forum.ThreadToMove,
				      -1L);
         Ico_PutIconPaste ();
         Frm_EndForm ();
        }
     }

   /***** Write link to forum *****/
   Frm_StartFormAnchor (For_ActionsSeeFor[Forum->Type],
                        For_FORUM_THREADS_SECTION_ID);
   For_PutAllHiddenParamsForum (1,	// Page of threads = first
                                1,	// Page of posts   = first
                                Gbl.Forum.ForumSet,
                                Gbl.Forum.ThreadsOrder,
                                Forum->Location,
                                -1L,
                                -1L);
   Frm_LinkFormSubmit (Act_GetActionTextFromDB (Act_GetActCod (For_ActionsSeeFor[Forum->Type]),ActTxt),
                       Style,NULL);
   For_SetForumName (Forum,ForumName,Gbl.Prefs.Language,true);
   switch (Forum->Type)
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM_GLOBAL_TCHS:
         Ico_PutIcon ("comments.svg",ForumName,"ICO16x16");
         break;
      case For_FORUM__SWAD__USRS:
      case For_FORUM__SWAD__TCHS:
         Ico_PutIcon ("swad64x64.png",ForumName,"ICO16x16");
         break;
      case For_FORUM_INSTIT_USRS:
      case For_FORUM_INSTIT_TCHS:
         Log_DrawLogo (Hie_INS,Forum->Location,ForumName,16,NULL,true);
         break;
      case For_FORUM_CENTRE_USRS:
      case For_FORUM_CENTRE_TCHS:
         Log_DrawLogo (Hie_CTR,Forum->Location,ForumName,16,NULL,true);
         break;
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         Log_DrawLogo (Hie_DEG,Forum->Location,ForumName,16,NULL,true);
         break;
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
         Ico_PutIcon ("list-ol.svg",ForumName,"ICO16x16");
         break;
      default:
         break;
     }
   fprintf (Gbl.F.Out,"&nbsp;%s",ForumName);

   /***** Write total number of threads and posts in this forum *****/
   if (ShowNumOfPosts)
     {
      if ((NumPosts = For_GetNumPstsInForum (Forum)))
         For_WriteNumThrsAndPsts (NumThrs,NumThrsWithNewPosts,NumPosts);
     }
   else
      if (NumThrs)
         For_WriteNumberOfThrs (NumThrs,NumThrsWithNewPosts);

   /***** End row *****/
   Frm_LinkFormEnd ();
   Frm_EndForm ();

   /***** Put link to register students *****/
   if (Forum->Type == For_FORUM_COURSE_USRS)
      Enr_PutButtonInlineToRegisterStds (Forum->Location);

   HTM_LI_End ();
  }

/*****************************************************************************/
/************************** Set the name of a forum **************************/
/*****************************************************************************/

void For_SetForumName (struct Forum *Forum,
                       char ForumName[For_MAX_BYTES_FORUM_NAME + 1],
                       Lan_Language_t Language,bool UseHTMLEntities)
  {
   extern const char *Txt_General;
   extern const char *Txt_General_NO_HTML[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_only_teachers;
   extern const char *Txt_only_teachers_NO_HTML[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_Unknown_FORUM;
   struct Instit Ins;
   struct Centre Ctr;
   struct Degree Deg;
   struct Course Crs;

   switch (Forum->Type)
     {
      case For_FORUM_GLOBAL_USRS:
         Str_Copy (ForumName,UseHTMLEntities ? Txt_General :
                                               Txt_General_NO_HTML[Language],
                   For_MAX_BYTES_FORUM_NAME);
         break;
      case For_FORUM_GLOBAL_TCHS:
         snprintf (ForumName,For_MAX_BYTES_FORUM_NAME + 1,
                   "%s%s",
                   UseHTMLEntities ? Txt_General :
                                     Txt_General_NO_HTML[Language],
                   UseHTMLEntities ? Txt_only_teachers :
                                     Txt_only_teachers_NO_HTML[Language]);
         break;
      case For_FORUM__SWAD__USRS:
         Str_Copy (ForumName,Cfg_PLATFORM_SHORT_NAME,
                   For_MAX_BYTES_FORUM_NAME);
         break;
      case For_FORUM__SWAD__TCHS:
         snprintf (ForumName,For_MAX_BYTES_FORUM_NAME + 1,
                   "%s%s",Cfg_PLATFORM_SHORT_NAME,
                   UseHTMLEntities ? Txt_only_teachers :
                                     Txt_only_teachers_NO_HTML[Language]);
         break;
      case For_FORUM_INSTIT_USRS:
	 Ins.InsCod = Forum->Location;
	 if (!Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA))
	    Lay_ShowErrorAndExit ("Institution not found.");
         Str_Copy (ForumName,Ins.ShrtName,
                   For_MAX_BYTES_FORUM_NAME);
         break;
      case For_FORUM_INSTIT_TCHS:
	 Ins.InsCod = Forum->Location;
	 if (!Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA))
	    Lay_ShowErrorAndExit ("Institution not found.");
         snprintf (ForumName,For_MAX_BYTES_FORUM_NAME + 1,
                   "%s%s",Ins.ShrtName,
                   UseHTMLEntities ? Txt_only_teachers :
                                     Txt_only_teachers_NO_HTML[Language]);
         break;
      case For_FORUM_CENTRE_USRS:
	 Ctr.CtrCod = Forum->Location;
	 if (!Ctr_GetDataOfCentreByCod (&Ctr))
	    Lay_ShowErrorAndExit ("Centre not found.");
         Str_Copy (ForumName,Ctr.ShrtName,
                   For_MAX_BYTES_FORUM_NAME);
         break;
      case For_FORUM_CENTRE_TCHS:
	 Ctr.CtrCod = Forum->Location;
	 if (!Ctr_GetDataOfCentreByCod (&Ctr))
	    Lay_ShowErrorAndExit ("Centre not found.");
         snprintf (ForumName,For_MAX_BYTES_FORUM_NAME + 1,
                   "%s%s",Ctr.ShrtName,
                   UseHTMLEntities ? Txt_only_teachers :
                                     Txt_only_teachers_NO_HTML[Language]);
         break;
      case For_FORUM_DEGREE_USRS:
	 Deg.DegCod = Forum->Location;
	 if (!Deg_GetDataOfDegreeByCod (&Deg))
	    Lay_ShowErrorAndExit ("Degree not found.");
         Str_Copy (ForumName,Deg.ShrtName,
                   For_MAX_BYTES_FORUM_NAME);
         break;
      case For_FORUM_DEGREE_TCHS:
	 Deg.DegCod = Forum->Location;
	 if (!Deg_GetDataOfDegreeByCod (&Deg))
	    Lay_ShowErrorAndExit ("Degree not found.");
         snprintf (ForumName,For_MAX_BYTES_FORUM_NAME + 1,
                   "%s%s",Deg.ShrtName,
                   UseHTMLEntities ? Txt_only_teachers :
                                     Txt_only_teachers_NO_HTML[Language]);
         break;
      case For_FORUM_COURSE_USRS:
	 Crs.CrsCod = Forum->Location;
	 if (!Crs_GetDataOfCourseByCod (&Crs))
	    Lay_ShowErrorAndExit ("Course not found.");
         Str_Copy (ForumName,Crs.ShrtName,
                   For_MAX_BYTES_FORUM_NAME);
         break;
      case For_FORUM_COURSE_TCHS:
	 Crs.CrsCod = Forum->Location;
	 if (!Crs_GetDataOfCourseByCod (&Crs))
	    Lay_ShowErrorAndExit ("Course not found.");
         snprintf (ForumName,For_MAX_BYTES_FORUM_NAME + 1,
                   "%s%s",Crs.ShrtName,
                   UseHTMLEntities ? Txt_only_teachers :
                                     Txt_only_teachers_NO_HTML[Language]);
         break;
      default:
         Str_Copy (ForumName,Txt_Unknown_FORUM,
                   For_MAX_BYTES_FORUM_NAME);
     }
  }

/*****************************************************************************/
/***** Get number of threads with new posts since my last read of a forum ****/
/*****************************************************************************/

static unsigned For_GetNumThrsWithNewPstsInForum (struct Forum *Forum,
                                                  unsigned NumThreads)
  {
   char SubQuery[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumThrsWithNewPosts = NumThreads;	// By default, all the threads are new to me

   /***** Get last time I read this forum from database *****/
   if (Forum->Location > 0)
      sprintf (SubQuery," AND forum_thread.Location=%ld",Forum->Location);
   else
      SubQuery[0] = '\0';
   NumRows = DB_QuerySELECT (&mysql_res,"can not get the date of reading of a forum",
			     "SELECT MAX(forum_thr_read.ReadTime)"
			     " FROM forum_thr_read,forum_thread"
			     " WHERE forum_thr_read.UsrCod=%ld"
			     " AND forum_thr_read.ThrCod=forum_thread.ThrCod"
			     " AND forum_thread.ForumType=%u%s",
			     Gbl.Usrs.Me.UsrDat.UsrCod,
			     (unsigned) Forum->Type,SubQuery);

   if (NumRows)
     {
      /***** Get number of threads with a last message modify time > newest read time (row[0]) *****/
      row = mysql_fetch_row (mysql_res);
      NumThrsWithNewPosts = For_GetNumOfThreadsInForumNewerThan (Forum,row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumThrsWithNewPosts;
  }

/*****************************************************************************/
/**** Get number of threads in forum with a modify time > a specified time ***/
/*****************************************************************************/

static unsigned For_GetNumOfThreadsInForumNewerThan (struct Forum *Forum,
                                                     const char *Time)
  {
   char SubQuery[256];

   /***** Get number of threads with a last message modify time
          > specified time from database *****/
   if (Forum->Location > 0)
      sprintf (SubQuery," AND forum_thread.Location=%ld",Forum->Location);
   else
      SubQuery[0] = '\0';
   return
   (unsigned) DB_QueryCOUNT ("can not check if there are new posts in a forum",
			     "SELECT COUNT(*) FROM forum_thread,forum_post"
			     " WHERE forum_thread.ForumType=%u%s"
			     " AND forum_thread.LastPstCod=forum_post.PstCod"
			     " AND forum_post.ModifTime>'%s'",
			     (unsigned) Forum->Type,SubQuery,Time);
  }

/*****************************************************************************/
/** Get number of unread posts in a thread since my last read of that thread */
/*****************************************************************************/

static unsigned For_GetNumOfUnreadPostsInThr (long ThrCod,unsigned NumPostsInThr)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumUnreadPosts = NumPostsInThr;	// By default, all the posts are unread by me

   /***** Get last time I read this thread from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get the date of reading"
					" of a thread",
			     "SELECT ReadTime FROM forum_thr_read"
			     " WHERE ThrCod=%ld AND UsrCod=%ld",
			     ThrCod,Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Get if last time I read this thread exists in database *****/
   if (NumRows)
     {
      /***** Get the number of posts in thread with a modify time > newest read time for me (row[0]) *****/
      row = mysql_fetch_row (mysql_res);
      NumUnreadPosts = For_GetNumOfPostsInThrNewerThan (ThrCod,row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumUnreadPosts;
  }

/*****************************************************************************/
/**** Get number of posts in thread with a modify time > a specified time ****/
/*****************************************************************************/

static unsigned For_GetNumOfPostsInThrNewerThan (long ThrCod,const char *Time)
  {
   /***** Get the number of posts in thread with a modify time > a specified time from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not check if there are new posts"
			     " in a thread of a forum",
			     "SELECT COUNT(*) FROM forum_post"
			     " WHERE ThrCod=%ld AND ModifTime>'%s'",
			     ThrCod,Time);
  }

/*****************************************************************************/
/*************** Get and write total number of threads and posts *************/
/*****************************************************************************/

static void For_WriteNumThrsAndPsts (unsigned NumThrs,unsigned NumThrsWithNewPosts,unsigned NumPosts)
  {
   extern const char *Txt_thread;
   extern const char *Txt_threads;
   extern const char *Txt_FORUM_post;
   extern const char *Txt_FORUM_posts;
   extern const char *Txt_with_new_posts;

   /***** Write number of threads and number of posts *****/
   fprintf (Gbl.F.Out," [");
   if (NumThrs == 1)
     {
      fprintf (Gbl.F.Out,"1 %s",Txt_thread);
      if (NumThrsWithNewPosts)
         fprintf (Gbl.F.Out,", 1 %s",Txt_with_new_posts);
      fprintf (Gbl.F.Out,"; ");
      if (NumPosts == 1)
         fprintf (Gbl.F.Out,"1 %s",Txt_FORUM_post);
      else
         fprintf (Gbl.F.Out,"%u %s",NumPosts,Txt_FORUM_posts);
     }
   else
     {
      fprintf (Gbl.F.Out,"%u %s",NumThrs,Txt_threads);
      if (NumThrsWithNewPosts)
         fprintf (Gbl.F.Out,", %u %s",NumThrsWithNewPosts,Txt_with_new_posts);
      fprintf (Gbl.F.Out,"; %u %s",NumPosts,Txt_FORUM_posts);
     }
   fprintf (Gbl.F.Out,"]");
  }

/*****************************************************************************/
/************** Get and write total number of threads and posts **************/
/*****************************************************************************/

static void For_WriteNumberOfThrs (unsigned NumThrs,unsigned NumThrsWithNewPosts)
  {
   extern const char *Txt_thread;
   extern const char *Txt_threads;
   extern const char *Txt_with_new_posts;

   /***** Write number of threads and number of posts *****/
   fprintf (Gbl.F.Out," [");
   if (NumThrs == 1)
      fprintf (Gbl.F.Out,"1 %s",Txt_thread);
   else
      fprintf (Gbl.F.Out,"%u %s",NumThrs,Txt_threads);
   if (NumThrsWithNewPosts)
      fprintf (Gbl.F.Out,", %u %s",NumThrsWithNewPosts,Txt_with_new_posts);
   fprintf (Gbl.F.Out,"]");
  }

/*****************************************************************************/
/********************** Show available threads of a forum ********************/
/*****************************************************************************/

void For_ShowForumTheads (void)
  {
   /***** Get parameters related to forum *****/
   For_GetParamsForum ();

   /***** Show forum list again *****/
   For_ShowForumList ();

   /***** Show forum threads with no one highlighted *****/
   For_ShowForumThreadsHighlightingOneThread (-1L,Ale_SUCCESS,NULL);
  }

/*****************************************************************************/
/********** Show available threads of a forum highlighting a thread **********/
/*****************************************************************************/

static void For_ShowForumThreadsHighlightingOneThread (long ThrCodHighlighted,
                                                       Ale_AlertType_t AlertType,const char *Message)
  {
   extern const char *Hlp_MESSAGES_Forums_threads;
   extern const char *Txt_Forum;
   extern const char *Txt_MSG_Subject;
   extern const char *Txt_FORUM_THREAD_HELP_ORDER[2];
   extern const char *Txt_FORUM_THREAD_ORDER[2];
   extern const char *Txt_No_BR_msgs;
   extern const char *Txt_Unread_BR_msgs;
   extern const char *Txt_WriBRters;
   extern const char *Txt_ReaBRders;
   char SubQuery[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char FrameTitle[128 + For_MAX_BYTES_FORUM_NAME];
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];
   unsigned NumThr;
   unsigned NumThrs;
   unsigned NumThrInScreen;	// From 0 to Pag_ITEMS_PER_PAGE-1
   For_Order_t Order;
   long ThrCods[Pag_ITEMS_PER_PAGE];
   struct Pagination PaginationThrs;

   /***** Set forum name *****/
   For_SetForumName (&Gbl.Forum.ForumSelected,
	             ForumName,Gbl.Prefs.Language,true);

   /***** Get threads of a forum from database *****/
   if (Gbl.Forum.ForumSelected.Location > 0)
      sprintf (SubQuery," AND forum_thread.Location=%ld",
	       Gbl.Forum.ForumSelected.Location);
   else
      SubQuery[0] = '\0';
   switch (Gbl.Forum.ThreadsOrder)
     {
      case For_FIRST_MSG:
         NumThrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get thread of a forum",
					      "SELECT forum_thread.ThrCod"
					      " FROM forum_thread,forum_post"
					      " WHERE forum_thread.ForumType=%u%s"
					      " AND forum_thread.FirstPstCod=forum_post.PstCod"
					      " ORDER BY forum_post.CreatTime DESC",
					      (unsigned) Gbl.Forum.ForumSelected.Type,SubQuery);
         break;
      case For_LAST_MSG:
         NumThrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get thread of a forum",
					      "SELECT forum_thread.ThrCod"
					      " FROM forum_thread,forum_post"
					      " WHERE forum_thread.ForumType=%u%s"
					      " AND forum_thread.LastPstCod=forum_post.PstCod"
					      " ORDER BY forum_post.CreatTime DESC",
					      (unsigned) Gbl.Forum.ForumSelected.Type,SubQuery);
         break;
      default:	// Impossible
	 return;
     }

   /***** Compute variables related to pagination of threads *****/
   PaginationThrs.NumItems = NumThrs;
   PaginationThrs.CurrentPage = (int) Gbl.Forum.CurrentPageThrs;
   Pag_CalculatePagination (&PaginationThrs);
   PaginationThrs.Anchor = For_FORUM_THREADS_SECTION_ID;
   Gbl.Forum.CurrentPageThrs = (unsigned) PaginationThrs.CurrentPage;

   /***** Fill the list of threads for current page *****/
   mysql_data_seek (mysql_res,(my_ulonglong) (PaginationThrs.FirstItemVisible - 1));
   for (NumThr = PaginationThrs.FirstItemVisible, NumThrInScreen = 0;
        NumThr <= PaginationThrs.LastItemVisible;
        NumThr++, NumThrInScreen++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get thread code(row[0]) */
      if ((ThrCods[NumThrInScreen] = Str_ConvertStrCodToLongCod (row[0])) < 0)
         Lay_ShowErrorAndExit ("Error when getting thread of a forum.");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Show alert after action *****/
   HTM_SECTION_Begin (For_FORUM_THREADS_SECTION_ID);
   if (Message)
      if (Message[0])
         Ale_ShowAlert (AlertType,Message);

   /***** Begin box for threads of this forum *****/
   snprintf (FrameTitle,sizeof (FrameTitle),
	     "%s: %s",
	     Txt_Forum,ForumName);
   Box_BoxBegin (NULL,FrameTitle,For_PutIconNewThread,
		 Hlp_MESSAGES_Forums_threads,Box_NOT_CLOSABLE);

   /***** List the threads *****/
   if (NumThrs)
     {
      /***** Write links to all the pages in the listing of threads *****/
      if (PaginationThrs.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (Pag_THREADS_FORUM,
                                        0,
                                        &PaginationThrs);

      /***** Heading row *****/
      HTM_TABLE_BeginWideMarginPadding (2);
      HTM_TR_Begin (NULL);

      HTM_TH (1,1,"BT",NULL);
      HTM_TH (1,1,"CONTEXT_COL",NULL);	// Column for contextual icons
      HTM_TH (1,1,"LM",Txt_MSG_Subject);

      for (Order = For_FIRST_MSG;
	   Order <= For_LAST_MSG;
	   Order++)
	{
	 HTM_TH_Begin (1,2,"CM");

         Frm_StartFormAnchor (For_ActionsSeeFor[Gbl.Forum.ForumSelected.Type],
                              For_FORUM_THREADS_SECTION_ID);
	 For_PutAllHiddenParamsForum (Gbl.Forum.CurrentPageThrs,	// Page of threads = current
                                      1,				// Page of posts   = first
                                      Gbl.Forum.ForumSet,
				      Order,
				      Gbl.Forum.ForumSelected.Location,
				      -1L,
				      -1L);
	 Frm_LinkFormSubmit (Txt_FORUM_THREAD_HELP_ORDER[Order],"TIT_TBL",NULL);
         if (Order == Gbl.Forum.ThreadsOrder)
            fprintf (Gbl.F.Out,"<u>");
	 fprintf (Gbl.F.Out,"%s",Txt_FORUM_THREAD_ORDER[Order]);
         if (Order == Gbl.Forum.ThreadsOrder)
            fprintf (Gbl.F.Out,"</u>");
         Frm_LinkFormEnd ();
         Frm_EndForm ();

         HTM_TH_End ();
	}

      HTM_TH (1,1,"RM",Txt_No_BR_msgs);
      HTM_TH (1,1,"RM",Txt_Unread_BR_msgs);
      HTM_TH (1,1,"RM",Txt_WriBRters);
      HTM_TH (1,1,"RM",Txt_ReaBRders);

      HTM_TR_End ();

      /***** List the threads *****/
      For_ListForumThrs (ThrCods,ThrCodHighlighted,&PaginationThrs);

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Write links to all the pages in the listing of threads *****/
      if (PaginationThrs.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (Pag_THREADS_FORUM,
                                        0,
                                        &PaginationThrs);
     }

   /***** Put a form to write the first post of a new thread *****/
   HTM_SECTION_Begin (For_NEW_THREAD_SECTION_ID);
   For_WriteFormForumPst (false,NULL);
   HTM_SECTION_End ();

   /***** End box with threads of this forum ****/
   Box_BoxEnd ();
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/********************** Put icon to write a new thread ***********************/
/*****************************************************************************/

static void For_PutIconNewThread (void)
  {
   extern const char *Txt_New_thread;

   Ico_PutContextualIconToAdd (For_ActionsSeeFor[Gbl.Forum.ForumSelected.Type],
                               For_NEW_THREAD_SECTION_ID,
			       For_PutAllHiddenParamsNewThread,
			       Txt_New_thread);
  }

static void For_PutAllHiddenParamsNewThread (void)
  {
   For_PutAllHiddenParamsForum (1,	// Page of threads = first
                                1,	// Page of posts = first
                                Gbl.Forum.ForumSet,
                                Gbl.Forum.ThreadsOrder,
                                Gbl.Forum.ForumSelected.Location,
                                -1L,
                                -1L);
  }

/*****************************************************************************/
/********************** Get number of forums of a type ***********************/
/*****************************************************************************/

unsigned For_GetNumTotalForumsOfType (For_ForumType_t ForumType,
                                      long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumForums;

   /***** Get number of forums of a type from database *****/
   switch (ForumType)
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM_GLOBAL_TCHS:
      case For_FORUM__SWAD__USRS:
      case For_FORUM__SWAD__TCHS:
         return 1;	// Only one forum
      case For_FORUM_INSTIT_USRS:
      case For_FORUM_INSTIT_TCHS:
         if (InsCod > 0)	// InsCod > 0 ==> 0 <= number of institutions forums for an institution <= 1
            DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT Location)"
			    " FROM forum_thread"
			    " WHERE ForumType=%u"
			    " AND Location=%ld",
                            (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of institution forums for a country
	    DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT forum_thread.Location)"
			    " FROM forum_thread,institutions"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=institutions.InsCod"
			    " AND institutions.CtyCod=%ld",
		            (unsigned) ForumType,CtyCod);
         else			// CtyCod <= 0 ==> Number of institutions forums for the whole platform
            DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT Location)"
			    " FROM forum_thread"
			    " WHERE ForumType=%u",
                            (unsigned) ForumType);
	 break;
      case For_FORUM_CENTRE_USRS:
      case For_FORUM_CENTRE_TCHS:
         if (CtrCod > 0)	// CtrCod > 0 ==> 0 <= number of centre forums for a centre <= 1
            DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT Location)"
			    " FROM forum_thread"
			    " WHERE ForumType=%u"
			    " AND Location=%ld",
                            (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of centre forums for an institution
	    DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT forum_thread.Location)"
			    " FROM forum_thread,centres"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=centres.CtrCod"
			    " AND centres.InsCod=%ld",
		            (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of centre forums for a country
	    DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT forum_thread.Location)"
			    " FROM forum_thread,centres,institutions"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=centres.CtrCod"
			    " AND centres.InsCod=institutions.InsCod"
			    " AND institutions.CtyCod=%ld",
		            (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of centre forums for the whole platform
            DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT Location)"
			    " FROM forum_thread"
			    " WHERE ForumType=%u",
                            (unsigned) ForumType);
	 break;
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         if (DegCod > 0)	// DegCod > 0 ==> 0 <= number of degree forums for a degree <= 1
            DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT Location)"
			    " FROM forum_thread"
			    " WHERE ForumType=%u"
			    " AND Location=%ld",
                            (unsigned) ForumType,DegCod);
         else if (CtrCod > 0)	// DegCod <= 0 && CtrCod > 0 ==> Number of degree forums for a centre
	    DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT forum_thread.Location)"
			    " FROM forum_thread,degrees"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=degrees.DegCod"
			    " AND degrees.CtrCod=%ld",
		            (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of degree forums for an institution
	    DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT forum_thread.Location)"
			    " FROM forum_thread,degrees,centres"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=degrees.DegCod"
			    " AND degrees.CtrCod=centres.CtrCod"
			    " AND centres.InsCod=%ld",
		            (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of degree forums for a country
	    DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT forum_thread.Location)"
			    " FROM forum_thread,degrees,centres,institutions"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=degrees.DegCod"
			    " AND degrees.CtrCod=centres.CtrCod"
			    " AND centres.InsCod=institutions.InsCod"
			    " AND institutions.CtyCod=%ld",
		            (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of degree forums for the whole platform
	    DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT Location)"
			    " FROM forum_thread"
			    " WHERE ForumType=%u",
		            (unsigned) ForumType);
         break;
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
         if (CrsCod > 0)	// CrsCod > 0 ==> 0 <= number of course forums for a course <= 1
            DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT Location)"
			    " FROM forum_thread"
			    " WHERE ForumType=%u"
			    " AND Location=%ld",
                            (unsigned) ForumType,CrsCod);
         else if (DegCod > 0)	// CrsCod <= 0 && DegCod > 0 ==> Number of course forums for a degree
	    DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT forum_thread.Location)"
			    " FROM forum_thread,courses"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=courses.CrsCod"
			    " AND courses.DegCod=%ld",
		            (unsigned) ForumType,DegCod);
         else if (CtrCod > 0)	// DegCod <= 0 && CtrCod > 0 ==> Number of course forums for a centre
	    DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT forum_thread.Location)"
			    " FROM forum_thread,courses,degrees"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=courses.CrsCod"
			    " AND courses.DegCod=degrees.DegCod"
			    " AND degrees.CtrCod=%ld",
		            (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of course forums for an institution
	    DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT forum_thread.Location)"
			    " FROM forum_thread,courses,degrees,centres"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=courses.CrsCod"
			    " AND courses.DegCod=degrees.DegCod"
			    " AND degrees.CtrCod=centres.CtrCod"
			    " AND centres.InsCod=%ld",
		            (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of course forums for a country
	    DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT forum_thread.Location)"
			    " FROM forum_thread,courses,degrees,centres,institutions"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=courses.CrsCod"
			    " AND courses.DegCod=degrees.DegCod"
			    " AND degrees.CtrCod=centres.CtrCod"
			    " AND centres.InsCod=institutions.InsCod"
			    " AND institutions.CtyCod=%ld",
		            (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of course forums for the whole platform
	    DB_QuerySELECT (&mysql_res,"can not get number of forums of a type",
			    "SELECT COUNT(DISTINCT Location)"
			    " FROM forum_thread"
			    " WHERE ForumType=%u",
			    (unsigned) ForumType);
         break;
      default:
	 return 0;
     }

   /* Get row with number of threads and number of posts */
   row = mysql_fetch_row (mysql_res);

   /* Get number of threads (row[0]) */
   if (sscanf (row[0],"%u",&NumForums) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of forums of a type.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumForums;
  }

/*****************************************************************************/
/*********** Get total number of threads in forums of this type **************/
/*****************************************************************************/

unsigned For_GetNumTotalThrsInForumsOfType (For_ForumType_t ForumType,
                                            long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod)
  {
   unsigned NumThrs;

   /***** Get total number of threads in forums of this type from database *****/
   switch (ForumType)
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM_GLOBAL_TCHS:
      case For_FORUM__SWAD__USRS:
      case For_FORUM__SWAD__TCHS:
         // Total number of threads in forums of this type
         NumThrs =
	 (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				   " in forums of a type",
				   "SELECT COUNT(*)"
				   " FROM forum_thread"
				   " WHERE ForumType=%u",
				   (unsigned) ForumType);
         break;
      case For_FORUM_INSTIT_USRS:
      case For_FORUM_INSTIT_TCHS:
         if (InsCod > 0)	// InsCod > 0 ==> Number of threads in institution forums for an institution
            NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread"
				      " WHERE ForumType=%u"
				      " AND Location=%ld",
				      (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of threads in institution forums for a country
	    NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread,institutions"
				      " WHERE forum_thread.ForumType=%u"
				      " AND forum_thread.Location=institutions.InsCod"
				      " AND institutions.CtyCod=%ld",
				      (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of threads in institution forums for the whole platform
            NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread"
				      " WHERE ForumType=%u",
				      (unsigned) ForumType);
	 break;
      case For_FORUM_CENTRE_USRS:
      case For_FORUM_CENTRE_TCHS:
         if (CtrCod > 0)	// CtrCod > 0 ==> 0 <= Number of threads in centre forums for a centre <= 1
            NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread"
				      " WHERE ForumType=%u"
				      " AND Location=%ld",
				      (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of threads in centre forums for an institution
            NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread,centres"
				      " WHERE forum_thread.ForumType=%u"
				      " AND forum_thread.Location=centres.CtrCod"
				      " AND centres.InsCod=%ld",
				      (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of threads in centre forums for a country
	    NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread,centres,institutions"
				      " WHERE forum_thread.ForumType=%u"
				      " AND forum_thread.Location=centres.CtrCod"
				      " AND centres.InsCod=institutions.InsCod"
				      " AND institutions.CtyCod=%ld",
				      (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of threads in centre forums for the whole platform
            NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread"
				      " WHERE ForumType=%u",
				      (unsigned) ForumType);
	 break;
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         if (DegCod > 0)	// DegCod > 0 ==> Number of threads in degree forums for a degree
            NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread"
				      " WHERE ForumType=%u"
				      " AND Location=%ld",
				      (unsigned) ForumType,DegCod);
         else if (CtrCod > 0)	// DegCod <= 0 && CtrCod > 0 ==> Number of threads in degree forums for a centre
            NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread,degrees"
				      " WHERE forum_thread.ForumType=%u"
				      " AND forum_thread.Location=degrees.DegCod"
				      " AND degrees.CtrCod=%ld",
				      (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of threads in degree forums for an institution
            NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread,degrees,centres"
				      " WHERE forum_thread.ForumType=%u"
				      " AND forum_thread.Location=degrees.DegCod"
				      " AND degrees.CtrCod=centres.CtrCod"
				      " AND centres.InsCod=%ld",
				      (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of threads in degree forums for a country
	    NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread,degrees,centres,institutions"
				      " WHERE forum_thread.ForumType=%u"
				      " AND forum_thread.Location=degrees.DegCod"
				      " AND degrees.CtrCod=centres.CtrCod"
				      " AND centres.InsCod=institutions.InsCod"
				      " AND institutions.CtyCod=%ld",
				      (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of threads in degree forums for the whole platform
            NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread"
				      " WHERE ForumType=%u",
				      (unsigned) ForumType);
         break;
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
         if (CrsCod > 0)	// CrsCod > 0 ==> 0 <= Number of threads in course forums for a course
            NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread"
				      " WHERE ForumType=%u"
				      " AND Location=%ld",
				      (unsigned) ForumType,CrsCod);
         else if (DegCod > 0)	// CrsCod <= 0 && DegCod > 0 ==> Number of threads in course forums for a degree
	    NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread,courses"
				      " WHERE forum_thread.ForumType=%u"
				      " AND forum_thread.Location=courses.CrsCod"
				      " AND courses.DegCod=%ld",
				      (unsigned) ForumType,DegCod);
         else if (CtrCod > 0)	// DegCod <= 0 && CtrCod > 0 ==> Number of threads in course forums for a centre
	    NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread,courses,degrees"
				      " WHERE forum_thread.ForumType=%u"
				      " AND forum_thread.Location=courses.CrsCod"
				      " AND courses.DegCod=degrees.DegCod"
				      " AND degrees.CtrCod=%ld",
				      (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of threads in course forums for an institution
            NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread,courses,degrees,centres"
				      " WHERE forum_thread.ForumType=%u"
				      " AND forum_thread.Location=courses.CrsCod"
				      " AND courses.DegCod=degrees.DegCod"
				      " AND degrees.CtrCod=centres.CtrCod"
				      " AND centres.InsCod=%ld",
				      (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of threads in course forums for a country
	    NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread,courses,degrees,centres,institutions"
				      " WHERE forum_thread.ForumType=%u"
				      " AND forum_thread.Location=courses.CrsCod"
				      " AND courses.DegCod=degrees.DegCod"
				      " AND degrees.CtrCod=centres.CtrCod"
				      " AND centres.InsCod=institutions.InsCod"
				      " AND institutions.CtyCod=%ld",
				      (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of threads in course forums for the whole platform
	    NumThrs =
	    (unsigned) DB_QueryCOUNT ("can not get the number of threads"
				      " in forums of a type",
				      "SELECT COUNT(*)"
				      " FROM forum_thread"
				      " WHERE ForumType=%u",
				      (unsigned) ForumType);
         break;
      default:
	 NumThrs = 0;
         break;
     }

   return NumThrs;
  }

/*****************************************************************************/
/******************* Get number of threads in a forum ************************/
/*****************************************************************************/

static unsigned For_GetNumThrsInForum (struct Forum *Forum)
  {
   char SubQuery[256];

   /***** Get number of threads in a forum from database *****/
   if (Forum->Location > 0)
      sprintf (SubQuery," AND Location=%ld",Forum->Location);
   else
      SubQuery[0] = '\0';
   return
   (unsigned) DB_QueryCOUNT ("can not get number of threads in a forum",
			     "SELECT COUNT(*) FROM forum_thread"
			     " WHERE ForumType=%u%s",
			     (unsigned) Forum->Type,SubQuery);
  }

/*****************************************************************************/
/************** Get total number of posts in forums of a type ****************/
/*****************************************************************************/

unsigned For_GetNumTotalPstsInForumsOfType (For_ForumType_t ForumType,
                                            long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod,
                                            unsigned *NumUsrsToBeNotifiedByEMail)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumPosts;

   /***** Get total number of posts in forums of this type from database *****/
   switch (ForumType)
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM_GLOBAL_TCHS:
      case For_FORUM__SWAD__USRS:
      case For_FORUM__SWAD__TCHS:
         // Total number of posts in forums of this type
         DB_QuerySELECT (&mysql_res,"can not get the total number"
				    " of forums of a type",
			 "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			 " FROM forum_thread,forum_post "
			 " WHERE forum_thread.ForumType=%u"
			 " AND forum_thread.ThrCod=forum_post.ThrCod",
                         (unsigned) ForumType);
         break;
      case For_FORUM_INSTIT_USRS:	case For_FORUM_INSTIT_TCHS:
         if (InsCod > 0)	// InsCod > 0 ==> Number of posts in institutions forums for an institution
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,forum_post"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=%ld"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
                            (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of posts in institutions forums for a country
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,institutions,forum_post"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=institutions.InsCod"
			    " AND institutions.CtyCod=%ld"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
                            (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of posts in institution forums for the whole platform
	    DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,forum_post "
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
		            (unsigned) ForumType);
         break;
      case For_FORUM_CENTRE_USRS:
      case For_FORUM_CENTRE_TCHS:
         if (CtrCod > 0)	// CtrCod > 0 ==> Number of posts in centre forums for a centre
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,forum_post"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=%ld"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
                            (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of posts in centre forums for an institution
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,centres,forum_post"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=centres.CtrCod"
			    " AND centres.InsCod=%ld"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
                            (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of posts in centre forums for a country
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,centres,institutions,forum_post"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=centres.CtrCod"
			    " AND centres.InsCod=institutions.InsCod"
			    " AND institutions.CtyCod=%ld"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
                            (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of posts in centre forums for the whole platform
	    DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,forum_post "
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
		            (unsigned) ForumType);
         break;
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         if (DegCod > 0)	// DegCod > 0 ==> Number of posts in degree forums for a degree
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,forum_post "
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=%ld"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
                            (unsigned) ForumType,DegCod);
         else if (CtrCod > 0)	// DegCod <= 0 && CtrCod > 0 ==> Number of posts in degree forums for a centre
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,degrees,forum_post "
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=degrees.DegCod"
			    " AND degrees.CtrCod=%ld"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
                            (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of posts in degree forums for an institution
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,degrees,centres,forum_post"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=degrees.DegCod"
			    " AND degrees.CtrCod=centres.CtrCod"
			    " AND centres.InsCod=%ld"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
                            (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of posts in degree forums for a country
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,degrees,centres,institutions,forum_post"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=degrees.DegCod"
			    " AND degrees.CtrCod=centres.CtrCod"
			    " AND centres.InsCod=institutions.InsCod"
			    " AND institutions.CtyCod=%ld"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
                            (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of posts in degree forums for the whole platform
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,forum_post "
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
                            (unsigned) ForumType);
         break;
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
         if (CrsCod > 0)	// CrsCod > 0 ==> 0 <= number of posts in course forums for a course
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,forum_post "
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=%ld"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
                            (unsigned) ForumType,CrsCod);
         else if (DegCod > 0)	// CrsCod <= 0 && DegCod > 0 ==> Number of posts in course forums for a degree
	    DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,courses,forum_post "
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=courses.CrsCod"
			    " AND courses.DegCod=%ld"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
		            (unsigned) ForumType,DegCod);
         else if (CtrCod > 0)	// DegCod <= 0 && CtrCod > 0 ==> Number of posts in course forums for a centre
	    DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,courses,degrees,forum_post "
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=courses.CrsCod"
			    " AND courses.DegCod=degrees.DegCod"
			    " AND degrees.CtrCod=%ld"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
		            (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of posts in course forums for an institution
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,courses,degrees,centres,forum_post"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=courses.CrsCod"
			    " AND courses.DegCod=degrees.DegCod"
			    " AND degrees.CtrCod=centres.CtrCod"
			    " AND centres.InsCod=%ld"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
                            (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of posts in course forums for a country
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,courses,degrees,centres,institutions,forum_post"
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.Location=courses.CrsCod"
			    " AND courses.DegCod=degrees.DegCod"
			    " AND degrees.CtrCod=centres.CtrCod"
			    " AND centres.InsCod=institutions.InsCod"
			    " AND institutions.CtyCod=%ld"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
                            (unsigned) ForumType,CtyCod);
         else			// CrsCod <= 0 && DegCod <= 0 && CtrCod <= 0 ==> Number of posts in course forums for the whole platform
	    DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),SUM(forum_post.NumNotif)"
			    " FROM forum_thread,forum_post "
			    " WHERE forum_thread.ForumType=%u"
			    " AND forum_thread.ThrCod=forum_post.ThrCod",
		            (unsigned) ForumType);
         break;
      default:
	 return 0;
     }

   /* Get row with number of posts */
   row = mysql_fetch_row (mysql_res);

   /* Get number of posts (row[0]) */
   if (sscanf (row[0],"%u",&NumPosts) != 1)
      Lay_ShowErrorAndExit ("Error when getting the total number of forums of a type.");

   /* Get number of users notified (row[1]) */
   if (row[1])
     {
      if (sscanf (row[1],"%u",NumUsrsToBeNotifiedByEMail) != 1)
         Lay_ShowErrorAndExit ("Error when getting the total number of forums of a type.");
     }
   else
      *NumUsrsToBeNotifiedByEMail = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumPosts;
  }

/*****************************************************************************/
/********************* Get number of posts in a forum ************************/
/*****************************************************************************/

static unsigned For_GetNumPstsInForum (struct Forum *Forum)
  {
   char SubQuery[256];

   /***** Get number of posts in a forum from database *****/
   if (Forum->Location > 0)
      sprintf (SubQuery," AND forum_thread.Location=%ld",Forum->Location);
   else
      SubQuery[0] = '\0';
   return
   (unsigned) DB_QueryCOUNT ("can not get the number of posts in a forum",
			     "SELECT COUNT(*) FROM forum_thread,forum_post "
			     " WHERE forum_thread.ForumType=%u%s"
			     " AND forum_thread.ThrCod=forum_post.ThrCod",
			     (unsigned) Forum->Type,SubQuery);
  }

/*****************************************************************************/
/************************ List the threads of a forum ************************/
/*****************************************************************************/

static void For_ListForumThrs (long ThrCods[Pag_ITEMS_PER_PAGE],
                               long ThrCodHighlighted,
                               struct Pagination *PaginationThrs)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *The_ClassFormInBoxBold[The_NUM_THEMES];
   extern const char *Txt_Thread_with_posts_from_you;
   extern const char *Txt_There_are_new_posts;
   extern const char *Txt_No_new_posts;
   unsigned NumThr;
   unsigned NumThrInScreen;	// From 0 to Pag_ITEMS_PER_PAGE-1
   unsigned UniqueId;
   char *Id;
   struct ForumThread Thr;
   struct UsrData UsrDat;
   For_Order_t Order;
   time_t TimeUTC;
   struct Pagination PaginationPsts;
   const char *Style;
   long ThreadInMyClipboard = -1L;
   unsigned Column;
   const char *BgColor;
   bool ICanMoveThreads;

   /***** Get if there is a thread ready to be moved *****/
   if ((ICanMoveThreads = For_CheckIfICanMoveThreads ()))
      ThreadInMyClipboard = For_GetThrInMyClipboard ();

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   for (NumThr = PaginationThrs->FirstItemVisible, NumThrInScreen = 0, UniqueId = 0, Gbl.RowEvenOdd = 0;
        NumThr <= PaginationThrs->LastItemVisible;
        NumThr++, NumThrInScreen++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
     {
      /***** Get the data of this thread *****/
      Thr.ThrCod = ThrCods[NumThrInScreen];
      For_GetThrData (&Thr);
      Style = (Thr.NumUnreadPosts ? "AUTHOR_TXT_NEW" :
	                            "AUTHOR_TXT");
      BgColor =  (Thr.ThrCod == ThreadInMyClipboard) ? "LIGHT_GREEN" :
	        ((Thr.ThrCod == ThrCodHighlighted)   ? "LIGHT_BLUE" :
                                                       Gbl.ColorRows[Gbl.RowEvenOdd]);

      /***** Show my photo if I have any posts in this thread *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"BT %s\"",BgColor);
      if (Thr.NumMyPosts)
         HTM_IMG (Gbl.Usrs.Me.PhotoURL[0] ? Gbl.Usrs.Me.PhotoURL :
                                            Cfg_URL_ICON_PUBLIC,
		  Gbl.Usrs.Me.PhotoURL[0] ? NULL :
			                    "usr_bl.jpg",
		  Txt_Thread_with_posts_from_you,
	          "class=\"PHOTO15x20\"");
      HTM_TD_End ();

      /***** Put an icon with thread status *****/
      HTM_TD_Begin ("class=\"CONTEXT_COL %s\"",BgColor);
      Ico_PutIcon (Thr.NumUnreadPosts ? "envelope.svg" :
        	                        "envelope-open-text.svg",
		   Thr.NumUnreadPosts ? Txt_There_are_new_posts :
                                        Txt_No_new_posts,
		   "ICO16x16");

      /***** Put button to remove the thread *****/
      if (PermissionThreadDeletion[Gbl.Forum.ForumSelected.Type] &
	  (1 << Gbl.Usrs.Me.Role.Logged)) // If I have permission to remove thread in this forum...
        {
         fprintf (Gbl.F.Out,"<br />");
         Frm_StartFormAnchor (For_ActionsReqDelThr[Gbl.Forum.ForumSelected.Type],
                              For_REMOVE_THREAD_SECTION_ID);
	 For_PutAllHiddenParamsForum (Gbl.Forum.CurrentPageThrs,	// Page of threads = current
                                      1,				// Page of posts   = first
                                      Gbl.Forum.ForumSet,
				      Gbl.Forum.ThreadsOrder,
				      Gbl.Forum.ForumSelected.Location,
				      Thr.ThrCod,
				      -1L);
         Ico_PutIconRemove ();
         Frm_EndForm ();
        }

      /***** Put button to cut the thread for moving it to another forum *****/
      if (ICanMoveThreads)
        {
         fprintf (Gbl.F.Out,"<br />");
         Frm_StartFormAnchor (For_ActionsCutThrFor[Gbl.Forum.ForumSelected.Type],
                              For_FORUM_THREADS_SECTION_ID);
	 For_PutAllHiddenParamsForum (Gbl.Forum.CurrentPageThrs,	// Page of threads = current
                                      1,				// Page of posts   = first
                                      Gbl.Forum.ForumSet,
				      Gbl.Forum.ThreadsOrder,
				      Gbl.Forum.ForumSelected.Location,
				      Thr.ThrCod,
				      -1L);
         Ico_PutIconCut ();
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
                             Thr.ThrCod,
                             &PaginationPsts,
                             Thr.Enabled[For_FIRST_MSG],
                             Thr.Subject,
                             Thr.NumUnreadPosts ? The_ClassFormInBoxBold[Gbl.Prefs.Theme] :
                        	                  The_ClassFormInBox[Gbl.Prefs.Theme],
                             true);
      HTM_TD_End ();

      /***** Write the authors and date-times of first and last posts *****/
      for (Order = For_FIRST_MSG;
	   Order <= For_LAST_MSG;
	   Order++)
        {
         if (Order == For_FIRST_MSG || Thr.NumPosts > 1)	// Don't write twice the same author when thread has only one thread
           {
            /* Write the author of first or last message */
            UsrDat.UsrCod = Thr.UsrCod[Order];
            Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);
	    HTM_TD_Begin ("class=\"%s LT %s\"",Style,BgColor);
            Msg_WriteMsgAuthor (&UsrDat,Thr.Enabled[Order],BgColor);
	    HTM_TD_End ();

            /* Write the date of first or last message (it's in YYYYMMDDHHMMSS format) */
            TimeUTC = Thr.WriteTime[Order];
	    UniqueId++;
	    if (asprintf (&Id,"thr_date_%u",UniqueId) < 0)
	       Lay_NotEnoughMemoryExit ();
            HTM_TD_Begin ("id=\"%s\" class=\"%s LT %s\"",Id,Style,BgColor);
	    Dat_WriteLocalDateHMSFromUTC (Id,TimeUTC,
					  Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
					  true,true,false,0x6);
            HTM_TD_End ();
            free ((void *) Id);
           }
         else
            for (Column = 1;
        	 Column <= 2;
        	 Column++)
              {
               HTM_TD_Begin ("class=\"%s LT %s\"",Style,BgColor);
               HTM_TD_End ();
              }
        }

      /***** Write number of posts in this thread *****/
      HTM_TD_Begin ("class=\"%s RT %s\"",Style,BgColor);
      fprintf (Gbl.F.Out,"%u&nbsp;",Thr.NumPosts);
      HTM_TD_End ();

      /***** Write number of new posts in this thread *****/
      HTM_TD_Begin ("class=\"%s RT %s\"",Style,BgColor);
      fprintf (Gbl.F.Out,"%u&nbsp;",Thr.NumUnreadPosts);
      HTM_TD_End ();

      /***** Write number of users who have write posts in this thread *****/
      HTM_TD_Begin ("class=\"%s RT %s\"",Style,BgColor);
      fprintf (Gbl.F.Out,"%u&nbsp;",Thr.NumWriters);
      HTM_TD_End ();

      /***** Write number of users who have read this thread *****/
      HTM_TD_Begin ("class=\"%s RT %s\"",Style,BgColor);
      fprintf (Gbl.F.Out,"%u&nbsp;",Thr.NumReaders);
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/***************************** Get data of a thread **************************/
/*****************************************************************************/

static void For_GetThrData (struct ForumThread *Thr)
  {
   extern const char *Txt_no_subject;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   For_Order_t Order;

   /***** Get data of a thread from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get data"
					" of a thread of a forum",
			     "SELECT m0.PstCod,m1.PstCod,m0.UsrCod,m1.UsrCod,"
			     "UNIX_TIMESTAMP(m0.CreatTime),"
			     "UNIX_TIMESTAMP(m1.CreatTime),"
			     "m0.Subject"
			     " FROM forum_thread,forum_post AS m0,forum_post AS m1"
			     " WHERE forum_thread.ThrCod=%ld"
			     " AND forum_thread.FirstPstCod=m0.PstCod"
			     " AND forum_thread.LastPstCod=m1.PstCod",
			     Thr->ThrCod);

   /***** The result of the query should have one row *****/
   if (NumRows != 1)
      Lay_ShowErrorAndExit ("Error when getting data of a thread of a forum.");
   row = mysql_fetch_row (mysql_res);

   /***** Get the code of the first post in this thread (row[0]) *****/
   Thr->PstCod[For_FIRST_MSG] = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get the code of the last  post in this thread (row[1]) *****/
   Thr->PstCod[For_LAST_MSG ] = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get the code of the first message in this thread (row[0]) *****/
   if (sscanf (row[0],"%ld",&(Thr->PstCod[For_FIRST_MSG])) != 1)
      Lay_ShowErrorAndExit ("Wrong code of post.");

   /***** Get the code of the last message in this thread (row[1]) *****/
   if (sscanf (row[1],"%ld",&(Thr->PstCod[For_LAST_MSG])) != 1)
      Lay_ShowErrorAndExit ("Wrong code of post.");

   /***** Get the author of the first post in this thread (row[2]) *****/
   Thr->UsrCod[For_FIRST_MSG] = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get the author of the last  post in this thread (row[3]) *****/
   Thr->UsrCod[For_LAST_MSG ] = Str_ConvertStrCodToLongCod (row[3]);

   /***** Get the date of the first post in this thread (row[4]) *****/
   Thr->WriteTime[For_FIRST_MSG] = Dat_GetUNIXTimeFromStr (row[4]);

   /***** Get the date of the last  post in this thread (row[5]) *****/
   Thr->WriteTime[For_LAST_MSG ] = Dat_GetUNIXTimeFromStr (row[5]);

   /***** Get the subject of this thread (row[6]) *****/
   Str_Copy (Thr->Subject,row[6],
             Cns_MAX_BYTES_SUBJECT);
   if (!Thr->Subject[0])
      snprintf (Thr->Subject,sizeof (Thr->Subject),
	        "[%s]",
		Txt_no_subject);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Get if first or last message are enabled *****/
   for (Order = For_FIRST_MSG;
	Order <= For_LAST_MSG;
	Order++)
      Thr->Enabled[Order] = For_GetIfPstIsEnabled (Thr->PstCod[Order]);
      // Thr->Enabled[Order] = true;

   /***** Get number of posts in this thread *****/
   Thr->NumPosts = For_GetNumPstsInThr (Thr->ThrCod);

   /***** Get number of unread (by me) posts in this thread *****/
   Thr->NumUnreadPosts = For_GetNumOfUnreadPostsInThr (Thr->ThrCod,Thr->NumPosts);

   /***** Get number of posts that I have written in this thread *****/
   Thr->NumMyPosts = For_GetNumMyPstInThr (Thr->ThrCod);

   /***** Get number of users who have write posts in this thread *****/
   Thr->NumWriters = For_GetNumOfWritersInThr (Thr->ThrCod);

   /***** Get number of users who have read this thread *****/
   Thr->NumReaders = For_GetNumOfReadersOfThr (Thr->ThrCod);
  }

/*****************************************************************************/
/**************** Show posts of a thread in a discussion forum ***************/
/*****************************************************************************/

void For_ShowThreadPosts (void)
  {
   /***** Get parameters related to forum *****/
   For_GetParamsForum ();

   /***** Show forum list again *****/
   For_ShowForumList ();

   /***** Show threads again *****/
   For_ShowForumThreadsHighlightingOneThread (Gbl.Forum.ForumSelected.ThrCod,
                                              Ale_SUCCESS,NULL);

   /***** Show the posts of that thread *****/
   For_ShowPostsOfAThread (Ale_SUCCESS,NULL);
  }

/*****************************************************************************/
/********************* Get parameters related to a forum *********************/
/*****************************************************************************/

static void For_GetParamsForum (void)
  {
   /***** Set forum type *****/
   For_SetForumType ();

   /***** Get parameter with code of course, degree, centre or institution *****/
   switch (Gbl.Forum.ForumSelected.Type)
     {
      case For_FORUM_INSTIT_USRS:
      case For_FORUM_INSTIT_TCHS:
      case For_FORUM_CENTRE_USRS:
      case For_FORUM_CENTRE_TCHS:
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
	 Gbl.Forum.ForumSelected.Location = Par_GetParToLong ("Location");
         break;
      default:
	 Gbl.Forum.ForumSelected.Location = -1L;
         break;
     }

   /***** Get optional parameter with code of a selected thread *****/
   Gbl.Forum.ForumSelected.ThrCod = Par_GetParToLong ("ThrCod");

   /***** Get optional parameter with code of a selected post *****/
   Gbl.Forum.ForumSelected.PstCod = Par_GetParToLong ("PstCod");

   /***** Get which forums I want to see *****/
   Gbl.Forum.ForumSet = (For_ForumSet_t)
			Par_GetParToUnsignedLong ("ForumSet",
						  0,
						  For_NUM_FORUM_SETS - 1,
						  (unsigned long) For_DEFAULT_FORUM_SET);

   /***** Get order type *****/
   Gbl.Forum.ThreadsOrder = (For_Order_t)
			     Par_GetParToUnsignedLong ("Order",
						       0,
						       For_NUM_ORDERS - 1,
						       (unsigned long) For_DEFAULT_ORDER);

   /***** Get optional page numbers for threads and posts *****/
   Gbl.Forum.CurrentPageThrs = Pag_GetParamPagNum (Pag_THREADS_FORUM);
   Gbl.Forum.CurrentPagePsts = Pag_GetParamPagNum (Pag_POSTS_FORUM);

   /***** Restrict access to forum *****/
   For_RestrictAccess ();
  }

/*****************************************************************************/
/***************************** Set the type of forum *************************/
/*****************************************************************************/

static void For_SetForumType (void)
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
         Gbl.Forum.ForumSelected.Type = For_FORUM_GLOBAL_USRS;
         break;
      case ActSeeForGenTch:	case ActSeePstForGenTch:
      case ActRcvThrForGenTch:	case ActRcvRepForGenTch:
      case ActReqDelThrGenTch:	case ActDelThrForGenTch:
      case ActCutThrForGenTch:	case ActPasThrForGenTch:
      case ActDelPstForGenTch:
      case ActEnbPstForGenTch:	case ActDisPstForGenTch:
         Gbl.Forum.ForumSelected.Type = For_FORUM_GLOBAL_TCHS;
         break;
      case ActSeeForSWAUsr:	case ActSeePstForSWAUsr:
      case ActRcvThrForSWAUsr:	case ActRcvRepForSWAUsr:
      case ActReqDelThrSWAUsr:	case ActDelThrForSWAUsr:
      case ActCutThrForSWAUsr:	case ActPasThrForSWAUsr:
      case ActDelPstForSWAUsr:
      case ActEnbPstForSWAUsr:	case ActDisPstForSWAUsr:
         Gbl.Forum.ForumSelected.Type = For_FORUM__SWAD__USRS;
         break;
      case ActSeeForSWATch:	case ActSeePstForSWATch:
      case ActRcvThrForSWATch:	case ActRcvRepForSWATch:
      case ActReqDelThrSWATch:	case ActDelThrForSWATch:
      case ActCutThrForSWATch:	case ActPasThrForSWATch:
      case ActDelPstForSWATch:
      case ActEnbPstForSWATch:	case ActDisPstForSWATch:
         Gbl.Forum.ForumSelected.Type = For_FORUM__SWAD__TCHS;
         break;
      case ActSeeForInsUsr:	case ActSeePstForInsUsr:
      case ActRcvThrForInsUsr:	case ActRcvRepForInsUsr:
      case ActReqDelThrInsUsr:	case ActDelThrForInsUsr:
      case ActCutThrForInsUsr:	case ActPasThrForInsUsr:
      case ActDelPstForInsUsr:
      case ActEnbPstForInsUsr:	case ActDisPstForInsUsr:
         Gbl.Forum.ForumSelected.Type = For_FORUM_INSTIT_USRS;
         break;
      case ActSeeForInsTch:	case ActSeePstForInsTch:
      case ActRcvThrForInsTch:	case ActRcvRepForInsTch:
      case ActReqDelThrInsTch:	case ActDelThrForInsTch:
      case ActCutThrForInsTch:	case ActPasThrForInsTch:
      case ActDelPstForInsTch:
      case ActEnbPstForInsTch:	case ActDisPstForInsTch:
	 Gbl.Forum.ForumSelected.Type = For_FORUM_INSTIT_TCHS;
	 break;
      case ActSeeForCtrUsr:	case ActSeePstForCtrUsr:
      case ActRcvThrForCtrUsr:	case ActRcvRepForCtrUsr:
      case ActReqDelThrCtrUsr:	case ActDelThrForCtrUsr:
      case ActCutThrForCtrUsr:	case ActPasThrForCtrUsr:
      case ActDelPstForCtrUsr:
      case ActEnbPstForCtrUsr:	case ActDisPstForCtrUsr:
         Gbl.Forum.ForumSelected.Type = For_FORUM_CENTRE_USRS;
         break;
      case ActSeeForCtrTch:	case ActSeePstForCtrTch:
      case ActRcvThrForCtrTch:	case ActRcvRepForCtrTch:
      case ActReqDelThrCtrTch:	case ActDelThrForCtrTch:
      case ActCutThrForCtrTch:	case ActPasThrForCtrTch:
      case ActDelPstForCtrTch:
      case ActEnbPstForCtrTch:	case ActDisPstForCtrTch:
	 Gbl.Forum.ForumSelected.Type = For_FORUM_CENTRE_TCHS;
	 break;
      case ActSeeForDegUsr:	case ActSeePstForDegUsr:
      case ActRcvThrForDegUsr:	case ActRcvRepForDegUsr:
      case ActReqDelThrDegUsr:	case ActDelThrForDegUsr:
      case ActCutThrForDegUsr:	case ActPasThrForDegUsr:
      case ActDelPstForDegUsr:
      case ActEnbPstForDegUsr:	case ActDisPstForDegUsr:
         Gbl.Forum.ForumSelected.Type = For_FORUM_DEGREE_USRS;
         break;
      case ActSeeForDegTch:	case ActSeePstForDegTch:
      case ActRcvThrForDegTch:	case ActRcvRepForDegTch:
      case ActReqDelThrDegTch:	case ActDelThrForDegTch:
      case ActCutThrForDegTch:	case ActPasThrForDegTch:
      case ActDelPstForDegTch:
      case ActEnbPstForDegTch:	case ActDisPstForDegTch:
	 Gbl.Forum.ForumSelected.Type = For_FORUM_DEGREE_TCHS;
	 break;
      case ActSeeForCrsUsr:	case ActSeePstForCrsUsr:
      case ActRcvThrForCrsUsr:	case ActRcvRepForCrsUsr:
      case ActReqDelThrCrsUsr:	case ActDelThrForCrsUsr:
      case ActCutThrForCrsUsr:	case ActPasThrForCrsUsr:
      case ActDelPstForCrsUsr:
      case ActEnbPstForCrsUsr:	case ActDisPstForCrsUsr:
         Gbl.Forum.ForumSelected.Type = For_FORUM_COURSE_USRS;
         break;
      case ActSeeForCrsTch:	case ActSeePstForCrsTch:
      case ActRcvThrForCrsTch:	case ActRcvRepForCrsTch:
      case ActReqDelThrCrsTch:	case ActDelThrForCrsTch:
      case ActCutThrForCrsTch:	case ActPasThrForCrsTch:
      case ActDelPstForCrsTch:
      case ActEnbPstForCrsTch:	case ActDisPstForCrsTch:
         Gbl.Forum.ForumSelected.Type = For_FORUM_COURSE_TCHS;
         break;
      default:
	 Gbl.Forum.ForumSelected.Type = For_FORUM_UNKNOWN;
     }
  }

/*****************************************************************************/
/************************** Restrict access to forum *************************/
/*****************************************************************************/

static void For_RestrictAccess (void)
  {
   Rol_Role_t MaxRole;
   bool ICanSeeForum;

   /***** Restrict access *****/
   switch (Gbl.Forum.ForumSelected.Type)
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM__SWAD__USRS:
         ICanSeeForum = true;
         break;
      case For_FORUM_GLOBAL_TCHS:
      case For_FORUM__SWAD__TCHS:
         Rol_GetRolesInAllCrssIfNotYetGot (&Gbl.Usrs.Me.UsrDat);
         ICanSeeForum = (Gbl.Usrs.Me.UsrDat.Roles.InCrss & ((1 << Rol_NET) |
                                                            (1 << Rol_TCH)));
         break;
      case For_FORUM_INSTIT_USRS:
	 MaxRole = Rol_GetMyMaxRoleInIns (Gbl.Forum.ForumSelected.Location);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole == Rol_STD ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      case For_FORUM_INSTIT_TCHS:
	 MaxRole = Rol_GetMyMaxRoleInIns (Gbl.Forum.ForumSelected.Location);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      case For_FORUM_CENTRE_USRS:
	 MaxRole = Rol_GetMyMaxRoleInCtr (Gbl.Forum.ForumSelected.Location);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole >= Rol_STD ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      case For_FORUM_CENTRE_TCHS:
	 MaxRole = Rol_GetMyMaxRoleInCtr (Gbl.Forum.ForumSelected.Location);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      case For_FORUM_DEGREE_USRS:
	 MaxRole = Rol_GetMyMaxRoleInDeg (Gbl.Forum.ForumSelected.Location);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole >= Rol_STD ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      case For_FORUM_DEGREE_TCHS:
	 MaxRole = Rol_GetMyMaxRoleInDeg (Gbl.Forum.ForumSelected.Location);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      case For_FORUM_COURSE_USRS:
	 MaxRole = Rol_GetMyRoleInCrs (Gbl.Forum.ForumSelected.Location);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole >= Rol_STD ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      case For_FORUM_COURSE_TCHS:
	 MaxRole = Rol_GetMyRoleInCrs (Gbl.Forum.ForumSelected.Location);
         ICanSeeForum = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||
                         MaxRole == Rol_NET ||
                         MaxRole == Rol_TCH);
         break;
      default:
	 ICanSeeForum = false;
	 break;
     }
   if (!ICanSeeForum)
      Lay_NoPermissionExit ();
  }

/*****************************************************************************/
/********************** Show an area to write a message **********************/
/*****************************************************************************/

static void For_WriteFormForumPst (bool IsReply,const char *Subject)
  {
   extern const char *Hlp_MESSAGES_Forums_new_post;
   extern const char *Hlp_MESSAGES_Forums_new_thread;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_New_post;
   extern const char *Txt_New_thread;
   extern const char *Txt_MSG_Subject;
   extern const char *Txt_MSG_Content;
   extern const char *Txt_Send;

   /***** Begin box *****/
   if (IsReply)
      Box_BoxBegin (NULL,Txt_New_post,NULL,
		    Hlp_MESSAGES_Forums_new_post,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,Txt_New_thread,NULL,
		    Hlp_MESSAGES_Forums_new_thread,Box_NOT_CLOSABLE);

   /***** Begin form *****/
   if (IsReply)	// Form to write a reply to a post of an existing thread
     {
      Frm_StartFormAnchor (For_ActionsRecRepFor[Gbl.Forum.ForumSelected.Type],
                           For_FORUM_POSTS_SECTION_ID);
      For_PutAllHiddenParamsNewPost ();
     }
   else		// Form to write the first post of a new thread
     {
      Frm_StartFormAnchor (For_ActionsRecThrFor[Gbl.Forum.ForumSelected.Type],
                           For_FORUM_POSTS_SECTION_ID);
      For_PutAllHiddenParamsNewThread ();
     }

   /***** Subject and content *****/
   HTM_TABLE_BeginCenterPadding (2);

   // If writing a reply to a message of an existing thread ==> write subject
   /* Subject */
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   fprintf (Gbl.F.Out,"<label for=\"Subject\" class=\"%s\">%s:</label>",
	    The_ClassFormInBox[Gbl.Prefs.Theme],Txt_MSG_Subject);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LM\"");
   fprintf (Gbl.F.Out,"<input type=\"text\" id=\"Subject\" name=\"Subject\""
		      " class=\"MSG_SUBJECT\""
		      " maxlength=\"%u\" value=\"%s\""
		      " required=\"required\" />",
	    Cns_MAX_CHARS_SUBJECT,
	    IsReply ? Subject :
		      "");
   HTM_TD_End ();

   HTM_TR_End ();

   /* Content */
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RT\"");
   fprintf (Gbl.F.Out,"<label for=\"Content\" class=\"%s\">%s:</label>",
            The_ClassFormInBox[Gbl.Prefs.Theme],Txt_MSG_Content);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LT\"");
   HTM_TEXTAREA_Begin ("id=\"Content\" name=\"Content\" class=\"MSG_CONTENT\" rows=\"10\"");
   HTM_TEXTAREA_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   HTM_TABLE_End ();

   /***** Help for text editor *****/
   Lay_HelpPlainEditor ();

   /***** Attached image (optional) *****/
   Med_PutMediaUploader (-1,"FOR_MED_INPUT");

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
   bool IsReply = false;
   long PstCod = 0;
   unsigned NumUsrsToBeNotifiedByEMail;
   struct TL_Publication SocPub;
   char Content[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Media Media;

   /***** Get parameters related to forum *****/
   For_GetParamsForum ();

   /***** Get the code of the thread y the número of page *****/
   if (Gbl.Action.Act == ActRcvRepForCrsUsr || Gbl.Action.Act == ActRcvRepForCrsTch ||
       Gbl.Action.Act == ActRcvRepForDegUsr || Gbl.Action.Act == ActRcvRepForDegTch ||
       Gbl.Action.Act == ActRcvRepForCtrUsr || Gbl.Action.Act == ActRcvRepForCtrTch ||
       Gbl.Action.Act == ActRcvRepForInsUsr || Gbl.Action.Act == ActRcvRepForInsTch ||
       Gbl.Action.Act == ActRcvRepForGenUsr || Gbl.Action.Act == ActRcvRepForGenTch ||
       Gbl.Action.Act == ActRcvRepForSWAUsr || Gbl.Action.Act == ActRcvRepForSWATch)
      IsReply = true;

   /***** Get message subject *****/
   Par_GetParToHTML ("Subject",Gbl.Msg.Subject,Cns_MAX_BYTES_SUBJECT);

   /***** Get message body *****/
   Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_LONG_TEXT,
                              Str_TO_RIGOROUS_HTML,false);

   /***** Initialize media *****/
   Med_MediaConstructor (&Media);

   /***** Get attached media *****/
   Media.Width   = For_IMAGE_SAVED_MAX_WIDTH;
   Media.Height  = For_IMAGE_SAVED_MAX_HEIGHT;
   Media.Quality = For_IMAGE_SAVED_QUALITY;
   Med_GetMediaFromForm (-1,&Media,NULL,
	                 For_FORUM_POSTS_SECTION_ID);	// Alerts will be shown later in posts section

   /***** Create a new message *****/
   if (IsReply)	// This post is a reply to another posts in the thread
     {
      // Gbl.Forum.ForumSelected.ThrCod has been received from form

      /***** Create last message of the thread *****/
      PstCod = For_InsertForumPst (Gbl.Forum.ForumSelected.ThrCod,Gbl.Usrs.Me.UsrDat.UsrCod,
                                   Gbl.Msg.Subject,Content,&Media);

      /***** Modify last message of the thread *****/
      For_UpdateThrLastPst (Gbl.Forum.ForumSelected.ThrCod,PstCod);
     }
   else			// This post is the first of a new thread
     {
      /***** Create new thread with unknown first and last message codes *****/
      Gbl.Forum.ForumSelected.ThrCod = For_InsertForumThread (-1L);

      /***** Create first (and last) message of the thread *****/
      PstCod = For_InsertForumPst (Gbl.Forum.ForumSelected.ThrCod,Gbl.Usrs.Me.UsrDat.UsrCod,
                                   Gbl.Msg.Subject,Content,&Media);

      /***** Update first and last posts of new thread *****/
      For_UpdateThrFirstAndLastPst (Gbl.Forum.ForumSelected.ThrCod,PstCod,PstCod);
     }

   /***** Free media *****/
   Med_MediaDestructor (&Media);

   /***** Increment number of forum posts in my user's figures *****/
   Prf_IncrementNumForPstUsr (Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Notify the new post to users in course *****/
   switch (Gbl.Forum.ForumSelected.Type)
     {
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
	 if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_FORUM_POST_COURSE,PstCod)))
	    For_UpdateNumUsrsNotifiedByEMailAboutPost (PstCod,NumUsrsToBeNotifiedByEMail);
	 break;
      default:
	 break;
     }

   /***** Notify the new post to previous writers in this thread *****/
   if (IsReply)
      if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_FORUM_REPLY,PstCod)))
         For_UpdateNumUsrsNotifiedByEMailAboutPost (PstCod,NumUsrsToBeNotifiedByEMail);

   /***** Insert forum post into public social activity *****/
   switch (Gbl.Forum.ForumSelected.Type)	// Only if forum is public for any logged user
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM__SWAD__USRS:
         TL_StoreAndPublishNote (TL_NOTE_FORUM_POST,PstCod,&SocPub);
         break;
      default:
	 break;
     }

   /***** Show forum list again *****/
   For_ShowForumList ();

   /***** Show threads again *****/
   For_ShowForumThreadsHighlightingOneThread (Gbl.Forum.ForumSelected.ThrCod,
                                              Ale_SUCCESS,NULL);

   /***** Show again the posts of this thread of the forum *****/
   For_ShowPostsOfAThread (Ale_SUCCESS,Txt_FORUM_Post_sent);
  }

/*****************************************************************************/
/********* Update number of users notified in table of forum posts **********/
/*****************************************************************************/

static void For_UpdateNumUsrsNotifiedByEMailAboutPost (long PstCod,unsigned NumUsrsToBeNotifiedByEMail)
  {
   /***** Update number of users notified *****/
   DB_QueryUPDATE ("can not update the number of notifications of a post",
		   "UPDATE forum_post SET NumNotif=NumNotif+%u"
		   " WHERE PstCod=%ld",
                   NumUsrsToBeNotifiedByEMail,PstCod);
  }

/*****************************************************************************/
/***************************** Delete a forum post ***************************/
/*****************************************************************************/

void For_RemovePost (void)
  {
   extern const char *Txt_FORUM_Post_and_thread_removed;
   extern const char *Txt_FORUM_Post_removed;
   struct UsrData UsrDat;
   time_t CreatTimeUTC;	// Creation time of a message
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char OriginalContent[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Media Media;
   bool ItsMe;
   bool ThreadDeleted = false;

   /***** Get parameters related to forum *****/
   For_GetParamsForum ();

   /***** Initialize image *****/
   Med_MediaConstructor (&Media);

   /***** Get forum post data *****/
   For_GetPstData (Gbl.Forum.ForumSelected.PstCod,&UsrDat.UsrCod,&CreatTimeUTC,
                   Subject,OriginalContent,&Media);

   /***** Check if I can remove the post *****/
   /* Check if the message really exists, if it has not been removed */
   if (!For_GetIfForumPstExists (Gbl.Forum.ForumSelected.PstCod))
      Lay_ShowErrorAndExit ("The post to remove no longer exists.");

   /* Check if I am the author of the message */
   ItsMe = Usr_ItsMe (UsrDat.UsrCod);
   if (!ItsMe)
      Lay_NoPermissionExit ();

   /* Check if the message is the last message in the thread */
   if (Gbl.Forum.ForumSelected.PstCod != For_GetLastPstCod (Gbl.Forum.ForumSelected.ThrCod))
      Lay_NoPermissionExit ();

   /***** Remove the post *****/
   ThreadDeleted = For_RemoveForumPst (Gbl.Forum.ForumSelected.PstCod,Media.MedCod);

   /***** Free image *****/
   Med_MediaDestructor (&Media);

   /***** Mark possible notifications as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_FORUM_POST_COURSE,Gbl.Forum.ForumSelected.PstCod);
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_FORUM_REPLY,Gbl.Forum.ForumSelected.PstCod);

   /***** Mark possible social note as unavailable *****/
   switch (Gbl.Forum.ForumSelected.Type)	// Only if forum is public for any logged user
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM__SWAD__USRS:
         TL_MarkNoteAsUnavailableUsingNoteTypeAndCod (TL_NOTE_FORUM_POST,Gbl.Forum.ForumSelected.PstCod);
         break;
      default:
	 break;
     }

   /***** Show forum list again *****/
   For_ShowForumList ();

   if (ThreadDeleted)
      /***** Show the remaining threads *****/
      For_ShowForumThreadsHighlightingOneThread (Gbl.Forum.ForumSelected.ThrCod,
                                                 Ale_SUCCESS,Txt_FORUM_Post_and_thread_removed);
   else
     {
      /***** Show threads again *****/
      For_ShowForumThreadsHighlightingOneThread (Gbl.Forum.ForumSelected.ThrCod,
						 Ale_SUCCESS,NULL);
      /***** Show the remaining posts *****/
      For_ShowPostsOfAThread (Ale_SUCCESS,Txt_FORUM_Post_removed);
     }
  }

/*****************************************************************************/
/***************** Request the removing of an existing thread ****************/
/*****************************************************************************/

void For_RequestRemoveThread (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_entire_thread_X;
   extern const char *Txt_Do_you_really_want_to_remove_the_entire_thread;
   extern const char *Txt_Remove_thread;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];

   /***** Get parameters related to forum *****/
   For_GetParamsForum ();

   /***** Get subject of the thread to delete *****/
   For_GetThrSubject (Gbl.Forum.ForumSelected.ThrCod,Subject);

   /***** Show forum list again *****/
   For_ShowForumList ();

   /***** Show question and button to remove the thread *****/
   HTM_SECTION_Begin (For_REMOVE_THREAD_SECTION_ID);
   if (Subject[0])
      Ale_ShowAlertAndButton (For_ActionsDelThrFor[Gbl.Forum.ForumSelected.Type],
			      For_FORUM_THREADS_SECTION_ID,NULL,
			      For_PutAllHiddenParamsRemThread,
			      Btn_REMOVE_BUTTON,Txt_Remove_thread,
			      Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_entire_thread_X,
                              Subject);
   else
      Ale_ShowAlertAndButton (For_ActionsDelThrFor[Gbl.Forum.ForumSelected.Type],
			      For_FORUM_THREADS_SECTION_ID,NULL,
			      For_PutAllHiddenParamsRemThread,
			      Btn_REMOVE_BUTTON,Txt_Remove_thread,
			      Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_entire_thread);
   HTM_SECTION_End ();

   /***** Show the threads again *****/
   For_ShowForumThreadsHighlightingOneThread (Gbl.Forum.ForumSelected.ThrCod,
					      Ale_SUCCESS,NULL);
  }

static void For_PutAllHiddenParamsRemThread (void)
  {
   For_PutAllHiddenParamsForum (Gbl.Forum.CurrentPageThrs,	// Page of threads = current
                                1,				// Page of posts   = first
                                Gbl.Forum.ForumSet,
				Gbl.Forum.ThreadsOrder,
				Gbl.Forum.ForumSelected.Location,
				Gbl.Forum.ForumSelected.ThrCod,
				-1L);
  }

/*****************************************************************************/
/*************************** Remove an existing thread ***********************/
/*****************************************************************************/

void For_RemoveThread (void)
  {
   extern const char *Txt_Thread_X_removed;
   extern const char *Txt_Thread_removed;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Message[256 + Cns_MAX_BYTES_SUBJECT + 1];

   /***** Get parameters related to forum *****/
   For_GetParamsForum ();

   if (PermissionThreadDeletion[Gbl.Forum.ForumSelected.Type] &
       (1 << Gbl.Usrs.Me.Role.Logged)) // If I have permission to remove thread in this forum...
     {
      /***** Get subject of thread to delete *****/
      For_GetThrSubject (Gbl.Forum.ForumSelected.ThrCod,Subject);

      /***** Remove the thread and all its posts *****/
      For_RemoveThreadAndItsPsts (Gbl.Forum.ForumSelected.ThrCod);

      /***** Show forum list again *****/
      For_ShowForumList ();

      /***** Show the threads again *****/
      if (Subject[0])
      	{
         snprintf (Message,sizeof (Message),
	           Txt_Thread_X_removed,Subject);
         For_ShowForumThreadsHighlightingOneThread (Gbl.Forum.ForumSelected.ThrCod,
	                                            Ale_SUCCESS,Message);
      	}
      else
	 For_ShowForumThreadsHighlightingOneThread (Gbl.Forum.ForumSelected.ThrCod,
	                                            Ale_SUCCESS,Txt_Thread_removed);
     }
   else
      Lay_NoPermissionExit ();
  }

/*****************************************************************************/
/*************** Cut a thread to move it to another forum ********************/
/*****************************************************************************/

void For_CutThread (void)
  {
   extern const char *Txt_Thread_X_marked_to_be_moved;
   extern const char *Txt_Thread_marked_to_be_moved;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Message[256 + Cns_MAX_BYTES_SUBJECT + 1];

   /***** Get parameters related to forum *****/
   For_GetParamsForum ();

   /***** Get subject of thread to cut *****/
   For_GetThrSubject (Gbl.Forum.ForumSelected.ThrCod,Subject);

   /***** Mark the thread as cut *****/
   For_InsertThrInClipboard (Gbl.Forum.ForumSelected.ThrCod);

   /***** Show forum list again *****/
   For_ShowForumList ();

   /***** Show the threads again *****/
   if (Subject[0])
     {
      snprintf (Message,sizeof (Message),
	        Txt_Thread_X_marked_to_be_moved,Subject);
      For_ShowForumThreadsHighlightingOneThread (Gbl.Forum.ForumSelected.ThrCod,
						 Ale_SUCCESS,Message);
     }
   else
      For_ShowForumThreadsHighlightingOneThread (Gbl.Forum.ForumSelected.ThrCod,
						 Ale_SUCCESS,Txt_Thread_marked_to_be_moved);

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
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Message[256 + Cns_MAX_BYTES_SUBJECT + 1];

   /***** Get parameters related to forum *****/
   For_GetParamsForum ();

   /***** Get subject of thread to paste *****/
   For_GetThrSubject (Gbl.Forum.ForumSelected.ThrCod,Subject);

   /***** Check if paste (move) the thread to current forum has sense *****/
   if (For_CheckIfThrBelongsToForum (Gbl.Forum.ForumSelected.ThrCod,&Gbl.Forum.ForumSelected))
     {
      /***** Show forum list again *****/
      For_ShowForumList ();

      /***** Show the threads again *****/
      if (Subject[0])
	{
         snprintf (Message,sizeof (Message),
	           Txt_The_thread_X_is_already_in_this_forum,
                   Subject);
         For_ShowForumThreadsHighlightingOneThread (Gbl.Forum.ForumSelected.ThrCod,
						    Ale_WARNING,Message);
        }
      else
         For_ShowForumThreadsHighlightingOneThread (Gbl.Forum.ForumSelected.ThrCod,
						    Ale_WARNING,Txt_The_thread_is_already_in_this_forum);
     }
   else
     {
      /***** Paste (move) the thread to current forum *****/
      For_MoveThrToCurrentForum (Gbl.Forum.ForumSelected.ThrCod);

      /***** Show forum list again *****/
      For_ShowForumList ();

      /***** Show the threads again *****/
      if (Subject[0])
	{
         snprintf (Message,sizeof (Message),
	           Txt_Thread_X_moved_to_this_forum,
                   Subject);
         For_ShowForumThreadsHighlightingOneThread (Gbl.Forum.ForumSelected.ThrCod,
						    Ale_SUCCESS,Message);
	}
      else
         For_ShowForumThreadsHighlightingOneThread (Gbl.Forum.ForumSelected.ThrCod,
						    Ale_SUCCESS,Txt_Thread_moved_to_this_forum);
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
/**************** Get if there is a thread ready to be moved *****************/
/*****************************************************************************/

static long For_GetThrInMyClipboard (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   long ThrCod = -1L;;

   /***** Get if there is a thread ready to move in my clipboard from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not check if there is"
					" any thread ready to be moved",
	                     "SELECT ThrCod FROM forum_thr_clip"
	                     " WHERE UsrCod=%ld",
			     Gbl.Usrs.Me.UsrDat.UsrCod);

   if (NumRows == 1)
     {
      /* Get thread code */
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%ld",&ThrCod) != 1)
         Lay_ShowErrorAndExit ("Error when checking if there is any thread ready to be moved.");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return ThrCod;
  }

/*****************************************************************************/
/***************** Get if a thread belongs to current forum ******************/
/*****************************************************************************/

static bool For_CheckIfThrBelongsToForum (long ThrCod,struct Forum *Forum)
  {
   char SubQuery[256];

   /***** Get if a thread belong to current forum from database *****/
   if (Forum->Location > 0)
      sprintf (SubQuery," AND forum_thread.Location=%ld",Forum->Location);
   else
      SubQuery[0] = '\0';
   return (DB_QueryCOUNT ("can not get if a thread belong to current forum",
			  "SELECT COUNT(*) FROM forum_thread"
			  " WHERE ThrCod=%ld AND ForumType=%u%s",
			  ThrCod,(unsigned) Forum->Type,SubQuery) != 0);
  }

/*****************************************************************************/
/************************ Move a thread to current forum *********************/
/*****************************************************************************/

static void For_MoveThrToCurrentForum (long ThrCod)
  {
   /***** Move a thread to current forum *****/
   switch (Gbl.Forum.ForumSelected.Type)
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM_GLOBAL_TCHS:
      case For_FORUM__SWAD__USRS:
      case For_FORUM__SWAD__TCHS:
         DB_QueryUPDATE ("can not move a thread to current forum",
			 "UPDATE forum_thread"
			 " SET ForumType=%u,Location=-1"
			 " WHERE ThrCod=%ld",
                         (unsigned) Gbl.Forum.ForumSelected.Type,
                         ThrCod);
         break;
      case For_FORUM_INSTIT_USRS:
      case For_FORUM_INSTIT_TCHS:
         DB_QueryUPDATE ("can not move a thread to current forum",
			 "UPDATE forum_thread"
			 " SET ForumType=%u,Location=%ld"
			 " WHERE ThrCod=%ld",
		         (unsigned) Gbl.Forum.ForumSelected.Type,
		         Gbl.Forum.ForumSelected.Location,
		         ThrCod);
         break;
      case For_FORUM_CENTRE_USRS:
      case For_FORUM_CENTRE_TCHS:
         DB_QueryUPDATE ("can not move a thread to current forum",
			 "UPDATE forum_thread"
			 " SET ForumType=%u,Location=%ld"
			 " WHERE ThrCod=%ld",
                         (unsigned) Gbl.Forum.ForumSelected.Type,
                         Gbl.Forum.ForumSelected.Location,
                         ThrCod);
         break;
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         DB_QueryUPDATE ("can not move a thread to current forum",
			 "UPDATE forum_thread"
			 " SET ForumType=%u,Location=%ld"
			 " WHERE ThrCod=%ld",
		         (unsigned) Gbl.Forum.ForumSelected.Type,
		         Gbl.Forum.ForumSelected.Location,
		         ThrCod);
         break;
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
         DB_QueryUPDATE ("can not move a thread to current forum",
			 "UPDATE forum_thread"
			 " SET ForumType=%u,Location=%ld"
			 " WHERE ThrCod=%ld",
		         (unsigned) Gbl.Forum.ForumSelected.Type,
		         Gbl.Forum.ForumSelected.Location,
		         ThrCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong forum.");
	 break;
     }
  }

/*****************************************************************************/
/********************* Insert thread in thread clipboard ********************/
/*****************************************************************************/

static void For_InsertThrInClipboard (long ThrCod)
  {
   /***** Remove expired thread clipboards *****/
   For_RemoveExpiredThrsClipboards ();

   /***** Add thread to my clipboard *****/
   DB_QueryREPLACE ("can not add thread to clipboard",
		    "REPLACE INTO forum_thr_clip"
		    " (ThrCod,UsrCod)"
		    " VALUES"
		    " (%ld,%ld)",
                    ThrCod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************* Remove expired thread clipboards (from all users) *************/
/*****************************************************************************/

static void For_RemoveExpiredThrsClipboards (void)
  {
   /***** Remove all expired clipboards *****/
   DB_QueryDELETE ("can not remove old threads from clipboards",
		   "DELETE LOW_PRIORITY FROM forum_thr_clip"
		   " WHERE TimeInsert<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Cfg_TIME_TO_DELETE_THREAD_CLIPBOARD);
  }

/*****************************************************************************/
/**************** Remove thread code from thread clipboard *******************/
/*****************************************************************************/

static void For_RemoveThrCodFromThrClipboard (long ThrCod)
  {
   /***** Remove thread from thread clipboard *****/
   DB_QueryDELETE ("can not remove a thread from clipboard",
		   "DELETE FROM forum_thr_clip WHERE ThrCod=%ld",
		   ThrCod);
  }

/*****************************************************************************/
/********************* Remove thread clipboard of a user *********************/
/*****************************************************************************/

void For_RemoveUsrFromThrClipboard (long UsrCod)
  {
   /***** Remove clipboard of specified user *****/
   DB_QueryDELETE ("can not remove a thread from the clipboard of a user",
		   "DELETE FROM forum_thr_clip WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/********** Remove all the threads and posts in forums of a degree ***********/
/*****************************************************************************/

void For_RemoveForums (Hie_Level_t Scope,long ForumLocation)
  {
   static const struct
     {
      For_ForumType_t Usrs;
      For_ForumType_t Tchs;
     } ForumType[Hie_NUM_LEVELS] =
     {
	{For_FORUM_GLOBAL_USRS,For_FORUM_GLOBAL_TCHS},	// Hie_UNK	// No forums for this scope
	{For_FORUM_GLOBAL_USRS,For_FORUM_GLOBAL_TCHS},	// Hie_SYS	// Not removable
	{For_FORUM_GLOBAL_USRS,For_FORUM_GLOBAL_TCHS},	// Hie_CTY	// No forums for this scope
	{For_FORUM_INSTIT_USRS,For_FORUM_INSTIT_TCHS},	// Hie_INS
	{For_FORUM_CENTRE_USRS,For_FORUM_CENTRE_TCHS},	// Hie_CTR
	{For_FORUM_DEGREE_USRS,For_FORUM_DEGREE_TCHS},	// Hie_DEG
	{For_FORUM_COURSE_USRS,For_FORUM_COURSE_TCHS},	// Hie_CRS
     };

   /***** Remove disabled posts *****/
   DB_QueryDELETE ("can not remove the disabled posts in forums",
		   "DELETE FROM forum_disabled_post"
		   " USING forum_thread,forum_post,forum_disabled_post"
		   " WHERE"
		   " (forum_thread.ForumType=%u"
		   " OR"
		   " forum_thread.ForumType=%u)"
		   " AND forum_thread.Location=%ld"
		   " AND forum_thread.ThrCod=forum_post.ThrCod"
		   " AND forum_post.PstCod=forum_disabled_post.PstCod",
	           ForumType[Scope].Usrs,
	           ForumType[Scope].Tchs,
	           ForumLocation);

   /***** Remove posts *****/
   DB_QueryDELETE ("can not remove posts in forums",
		   "DELETE FROM forum_post"
		   " USING forum_thread,forum_post"
		   " WHERE"
		   " (forum_thread.ForumType=%u"
		   " OR"
		   " forum_thread.ForumType=%u)"
		   " AND forum_thread.Location=%ld"
		   " AND forum_thread.ThrCod=forum_post.ThrCod",
	           ForumType[Scope].Usrs,
	           ForumType[Scope].Tchs,
	           ForumLocation);

   /***** Remove threads read *****/
   DB_QueryDELETE ("can not remove read threads in forums",
		   "DELETE FROM forum_thr_read"
		   " USING forum_thread,forum_thr_read"
		   " WHERE"
		   " (forum_thread.ForumType=%u"
		   " OR"
		   " forum_thread.ForumType=%u)"
		   " AND forum_thread.Location=%ld"
		   " AND forum_thread.ThrCod=forum_thr_read.ThrCod",
	           ForumType[Scope].Usrs,
	           ForumType[Scope].Tchs,
	           ForumLocation);

   /***** Remove threads *****/
   DB_QueryDELETE ("can not remove threads in forums",
		   "DELETE FROM forum_thread"
		   " WHERE"
		   " (forum_thread.ForumType=%u"
		   " OR"
		   " forum_thread.ForumType=%u)"
		   " AND Location=%ld",
	           ForumType[Scope].Usrs,
	           ForumType[Scope].Tchs,
	           ForumLocation);
  }
