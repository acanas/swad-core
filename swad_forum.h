// swad_forum.h: forums

#ifndef _SWAD_FOR
#define _SWAD_FOR
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include "swad_alert.h"
#include "swad_date.h"
#include "swad_language.h"
#include "swad_notification.h"

/*****************************************************************************/
/************************ Public constants and types *************************/
/*****************************************************************************/

#define For_NUM_DISABLED 2
typedef enum
  {
   For_DISABLED,
   For_ENABLED,
  } For_Disabled_t;

#define For_NUM_FORUM_SETS 2
typedef enum
  {
   For_ONLY_CURRENT_FORUMS	= 0,
   For_ALL_MY_FORUMS		= 1,
  } For_ForumSet_t;	// Which set of forums I want to see: all my forums or only the forums of current institution/degree/course.
#define For_DEFAULT_FORUM_SET For_ONLY_CURRENT_FORUMS

#define For_NUM_USE_HTML_ENTITIES 2
typedef enum
  {
   For_DONT_USE_HTML_ENTITIES,
   For_USE_HTML_ENTITIES,
  } For_UseHTMLEntities_t;

#define For_NUM_TYPES_FORUM 13
typedef enum
  {
   For_FORUM_COURSE_USRS	=  0,	// TODO: Change to 11 (also in database)
   For_FORUM_COURSE_TCHS	=  1,	// TODO: Change to 12 (also in database)
   For_FORUM_DEGREE_USRS	=  2,	// TODO: Change to  9 (also in database)
   For_FORUM_DEGREE_TCHS	=  3,	// TODO: Change to 10 (also in database)
   For_FORUM_CENTER_USRS	=  4,	// TODO: Change to  7 (also in database)
   For_FORUM_CENTER_TCHS	=  5,	// TODO: Change to  8 (also in database)
   For_FORUM_INSTIT_USRS	=  6,	// TODO: Change to  5 (also in database)
   For_FORUM_INSTIT_TCHS	=  7,	// TODO: Change to  6 (also in database)
   For_FORUM_GLOBAL_USRS	=  8,	// TODO: Change to  1 (also in database)
   For_FORUM_GLOBAL_TCHS	=  9,	// TODO: Change to  2 (also in database)
   For_FORUM__SWAD__USRS	= 10,	// TODO: Change to  3 (also in database)
   For_FORUM__SWAD__TCHS	= 11,	// TODO: Change to  4 (also in database)
   For_FORUM_UNKNOWN		= 12,	// TODO: Change to  0 (also in database)
  } For_ForumType_t;	// Don't change numbers (used in database)

struct For_Forum
  {
   For_ForumType_t Type;	// Type of forum
   long HieCod;			// Hierachy code (code of institution, center, degree or course)
  };

struct For_Thread
  {
   long ThrCod;			// Thread code
   long PstCod[Dat_NUM_START_END_TIME];
   long UsrCod[Dat_NUM_START_END_TIME];
   time_t WriteTime[Dat_NUM_START_END_TIME];
   For_Disabled_t Disabled[Dat_NUM_START_END_TIME];
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   unsigned NumPosts;
   unsigned NumUnreadPosts;	// Number of my unread posts in thread
   unsigned NumMyPosts;		// Number of posts written by me in thread
   unsigned NumWriters;
   unsigned NumReaders;
  };

struct For_Post
  {
   long PstCod;			// Post code
  };

struct For_Forums
  {
   For_ForumSet_t ForumSet;
   Dat_StartEndTime_t ThreadsOrder;
   unsigned CurrentPageThrs;
   unsigned CurrentPagePsts;
   struct For_Forum Forum;	// Forum selected
   struct
     {
      long Selected;		// Thread selected
      long Current;		// Current thread
      long ToMove;		// Thread to move
     } Thread;
   long PstCod;			// Post selected
  };

#define For_DEFAULT_ORDER Dat_END_TIME

#define For_MAX_BYTES_FORUM_NAME (512 - 1)

// Links to go to <section>
#define For_REMOVE_THREAD_SECTION_ID	"remove_thread"
#define For_FORUM_THREADS_SECTION_ID	"forum_threads"
#define For_NEW_THREAD_SECTION_ID	"new_thread"
#define For_FORUM_POSTS_SECTION_ID	"thread_posts"
#define For_NEW_POST_SECTION_ID		"new_post"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void For_ResetForums (struct For_Forums *Forums);

void For_EnablePost (void);
void For_DisablePost (void);

long For_GetThreadForumTypeAndHieCodOfAPost (long PstCod,struct For_Forum *Forum);
void For_GetForumTypeAndHieCodOfAThread (long ThrCod,struct For_Forum *Forum);

void For_ShowPostsOfAThread (struct For_Forums *Forums,
			     Ale_AlertType_t AlertType,const char *Message);

void For_GetSummaryAndContentForumPst (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                       char **ContentStr,
                                       long PstCod,Ntf_GetContent_t GetContent);

void For_PutAllParsForum (unsigned NumPageThreads,
                          unsigned NumPagePosts,
                          For_ForumSet_t ForumSet,
                          Dat_StartEndTime_t Order,
                          long HieCod,
                          long ThrCod,
                          long PstCod);
void For_ShowForumList (struct For_Forums *Forums);

void For_SetForumName (const struct For_Forum *Forum,
                       char ForumName[For_MAX_BYTES_FORUM_NAME + 1],
                       Lan_Language_t Language,For_UseHTMLEntities_t UseHTMLEntities);

void For_ShowForumTheads (void);
void For_ShowForumThreadsHighlightingOneThread (struct For_Forums *Forums,
                                                Ale_AlertType_t AlertType,const char *Message);

void For_GetThreadData (struct For_Thread *Thr);

void For_ShowThreadPosts (void);

void For_GetParsForums (struct For_Forums *Forums);

void For_ReceiveForumPost (void);
void For_RemovePost (void);
void For_ReqRemThread (void);
void For_RemoveThread (void);
void For_CutThread (void);
void For_PasteThread (void);

//-------------------------------- Figures ------------------------------------
void For_GetAndShowForumStats (Hie_Level_t HieLvl);

#endif
