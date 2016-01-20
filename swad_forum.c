// swad_forum.c: forums

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL
#include <malloc.h>		// For malloc
#include <mysql/mysql.h>	// To access MySQL databases
#include <string.h>
#include <time.h>		// For time_t

#include "swad_action.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_layout.h"
#include "swad_logo.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_profile.h"
#include "swad_social.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;
extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];

/*****************************************************************************/
/*********************** Private constants and types *************************/
/*****************************************************************************/

#define For_FORUM_MAX_LEVELS 4

const unsigned PermissionThreadDeletion[For_NUM_TYPES_FORUM] =
  {
   0x1F0,	// For_FORUM_COURSE_USRS
   0x1F0,	// For_FORUM_COURSE_TCHS
   0x1E0,	// For_FORUM_DEGREE_USRS
   0x1E0,	// For_FORUM_DEGREE_TCHS
   0x1C0,	// For_FORUM_CENTRE_USRS
   0x1C0,	// For_FORUM_CENTRE_TCHS
   0x180,	// For_FORUM_INSTITUTION_USRS
   0x180,	// For_FORUM_INSTITUTION_TCHS
   0x100,	// For_FORUM_GLOBAL_USRS
   0x100,	// For_FORUM_GLOBAL_TCHS
   0x100,	// For_FORUM_SWAD_USRS
   0x100,	// For_FORUM_SWAD_TCHS
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
  };

/*****************************************************************************/
/***************************** Private prototypes ***************************/
/*****************************************************************************/

static void For_UpdateThrReadTime (long ThrCod,time_t ReadTimeUTC);
static unsigned For_GetNumOfReadersOfThr (long ThrCod);
static unsigned For_GetNumOfWritersInThr (long ThrCod);
static unsigned For_GetNumPstsInThr (long ThrCod);
static unsigned For_GetNumMyPstInThr (long ThrCod);
static time_t For_GetThrReadTime (long ThrCod);
static void For_ShowThreadPosts (long ThrCod,char *LastSubject);
static void For_PutParamWhichForum (void);
static void For_PutParamForumOrder (void);
static void For_PutFormWhichForums (void);
static void For_WriteLinkToTopLevelOfForums (void);
static void For_PutParamsForumInsDegCrs (void);
static void For_WriteLinksToGblForums (bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS]);
static void For_WriteLinksToPlatformForums (bool IsLastForum,bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS]);
static long For_WriteLinksToInsForums (long InsCod,bool IsLastIns,bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS]);
static long For_WriteLinksToCtrForums (long CtrCod,bool IsLastCtr,bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS]);
static long For_WriteLinksToDegForums (long DegCod,bool IsLastDeg,bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS]);
static long For_WriteLinksToCrsForums (long CrsCod,bool IsLastCrs,bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS]);
static void For_WriteLinkToAForum (For_ForumType_t ForumType,bool ShowNumOfPosts,
                                   unsigned Level,bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS]);
static void For_WriteLinkToForum (For_ForumType_t ForumType,Act_Action_t NextAct,const char *Icon,const char *ForumName,bool ShowNumOfPosts,
                                  unsigned Level,bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS]);
static unsigned For_GetNumOfThreadsInForumNewerThan (For_ForumType_t ForumType,const char *Time);
static unsigned For_GetNumOfUnreadPostsInThr (long ThrCod,unsigned NumPostsInThr);
static unsigned For_GetNumOfPostsInThrNewerThan (long ThrCod,const char *Time);

static void For_WriteFormForumPst (bool IsReply,long ThrCod,const char *Subject);

static void For_UpdateNumUsrsNotifiedByEMailAboutPost (long PstCod,unsigned NumUsrsToBeNotifiedByEMail);
static void For_WriteNumberOfThrs (unsigned NumThrs,unsigned NumThrsWithNewPosts);
static void For_WriteNumThrsAndPsts (unsigned NumThrs,unsigned NumThrsWithNewPosts,unsigned NumPosts);
static void For_WriteThrSubject (long ThrCod);
static long For_GetParamThrCod (void);
static void For_PutHiddenParamPstCod (long PstCod);
static long For_GetParamPstCod (void);
static void For_ShowAForumPost (struct ForumThread *Thr,unsigned PstNum,long PstCod,
                                bool LastPst,char *LastSubject,
                                bool NewPst,bool ICanModerateForum);
static void For_GetPstData (long PstCod,long *UsrCod,time_t *CreatTimeUTC,
                            char *Subject, char *Content);
static void For_WriteNumberOfPosts (For_ForumType_t ForumType,long UsrCod);

/*****************************************************************************/
/****************************** Enable a forum post **************************/
/*****************************************************************************/

void For_EnbPst (void)
  {
   extern const char *Txt_Post_unbanned;
   long PstCod,ThrCod;

   /***** Get order type, degree and course of the forum *****/
   For_GetParamsForum ();

   /***** Set forum type *****/
   For_SetForumTypeAndRestrictAccess ();

   /***** Get the post code to unban *****/
   PstCod = For_GetParamPstCod ();

   /***** Get the thread number *****/
   ThrCod = For_GetParamThrCod ();

   /***** Delete post from table of disabled posts *****/
   For_DeletePstFromDisabledPstTable (PstCod);
   Lay_ShowAlert (Lay_SUCCESS,Txt_Post_unbanned);

   /***** Show the posts again *****/
   For_ShowForumLevel2 (ThrCod);
  }

/*****************************************************************************/
/***************************** Disable a forum post **************************/
/*****************************************************************************/

void For_DisPst (void)
  {
   extern const char *Txt_Post_banned;
   long PstCod,ThrCod;

   /***** Get order type, degree and course of the forum *****/
   For_GetParamsForum ();

   /***** Set forum type *****/
   For_SetForumTypeAndRestrictAccess ();

   /***** Get the post code to ban *****/
   PstCod = For_GetParamPstCod ();

   /***** Get the thread number *****/
   ThrCod = For_GetParamThrCod ();

   /***** Check if post really exists, if it has not been removed *****/
   if (For_GetIfForumPstExists (PstCod))
     {
      /***** Insert post into table of banned posts *****/
      For_InsertPstIntoBannedPstTable (PstCod);
      Lay_ShowAlert (Lay_SUCCESS,Txt_Post_banned);
     }
   else
      Lay_ShowErrorAndExit ("The post to be banned no longer exists.");

   /***** Show the posts again *****/
   For_ShowForumLevel2 (ThrCod);
  }

/*****************************************************************************/
/******************** Get if a forum post exists in database *****************/
/*****************************************************************************/

bool For_GetIfForumPstExists (long PstCod)
  {
   char Query[512];

   /***** Get if a forum post exists from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM forum_post"
	          " WHERE PstCod='%ld'",
	    PstCod);
   return (DB_QueryCOUNT (Query,"can not check if a post of a forum already existed") != 0);	// Post exists if it appears in table of forum posts
  }

/*****************************************************************************/
/*********************** Get if a forum post is enabled **********************/
/*****************************************************************************/

bool For_GetIfPstIsEnabled (long PstCod)
  {
   char Query[512];

   /***** Get if post is disabled from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM forum_disabled_post"
                  " WHERE PstCod='%ld'",
            PstCod);
   return (DB_QueryCOUNT (Query,"can not check if a post of a forum is disabled") == 0);	// Post is enabled if it does not appear in table of disabled posts
  }

/*****************************************************************************/
/****************** Delete post from table of disabled posts *****************/
/*****************************************************************************/

void For_DeletePstFromDisabledPstTable (long PstCod)
  {
   char Query[512];

   /***** Remove post from disabled posts table *****/
   sprintf (Query,"DELETE FROM forum_disabled_post"
                  " WHERE PstCod='%ld'",
            PstCod);
   DB_QueryDELETE (Query,"can not unban a post of a forum");
  }

/*****************************************************************************/
/****************** Insert post into table of banned posts *******************/
/*****************************************************************************/

void For_InsertPstIntoBannedPstTable (long PstCod)
  {
   char Query[512];

   /***** Remove post from banned posts table *****/
   sprintf (Query,"REPLACE INTO forum_disabled_post"
	          " (PstCod,UsrCod,DisableTime)"
                  " VALUES ('%ld','%ld',NOW())",
            PstCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryREPLACE (Query,"can not ban a post of a forum");
  }

/*****************************************************************************/
/************** Insert a post new in the table of posts of forums ************/
/*****************************************************************************/

long For_InsertForumPst (long ThrCod,long UsrCod,const char *Subject,const char *Content)
  {
   char Query[256+Cns_MAX_BYTES_SUBJECT+Cns_MAX_BYTES_LONG_TEXT];

   /***** Insert forum post in the database *****/
   sprintf (Query,"INSERT INTO forum_post"
	          " (ThrCod,UsrCod,CreatTime,ModifTime,NumNotif,Subject,Content)"
                  " VALUES ('%ld','%ld',NOW(),NOW(),'0','%s','%s')",
            ThrCod,UsrCod,Subject,Content);
   return DB_QueryINSERTandReturnCode (Query,"can not create a new post in a forum");
  }

/*****************************************************************************/
/***************** Delete a post from the forum post table *******************/
/*****************************************************************************/
// Return true if the post thread is deleted

bool For_RemoveForumPst (long PstCod)
  {
   char Query[512];
   long ThrCod;
   bool ThreadDeleted = false;

   /***** If the post is the only one in its thread, delete that thread *****/
   if (For_NumPstsInThrWithPstCod (PstCod,&ThrCod) < 2)
     {
      For_RemoveThreadOnly (ThrCod);
      ThreadDeleted = true;
     }

   /***** Delete post from forum post table *****/
   sprintf (Query,"DELETE FROM forum_post WHERE PstCod='%ld'",
            PstCod);
   DB_QueryDELETE (Query,"can not remove a post from a forum");

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

unsigned For_NumPstsInThrWithPstCod (long PstCod,long *ThrCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumPsts;

   /***** Get number of posts in the thread that holds a post from database *****/
   sprintf (Query,"SELECT COUNT(PstCod),ThrCod FROM forum_post"
                  " WHERE ThrCod IN"
                  " (SELECT ThrCod FROM forum_post"
                  " WHERE PstCod='%ld') GROUP BY ThrCod;",
            PstCod);
   DB_QuerySELECT (Query,&mysql_res,"can not get number of posts in a thread of a forum");

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

long For_InsertForumThread (For_ForumType_t ForumType,long FirstPstCod)
  {
   char Query[512];

   /***** Insert new thread in the database *****/
   switch (ForumType)
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM_GLOBAL_TCHS:
      case For_FORUM_SWAD_USRS:
      case For_FORUM_SWAD_TCHS:
         sprintf (Query,"INSERT INTO forum_thread"
                        " (ForumType,Location,FirstPstCod,LastPstCod)"
                        " VALUES ('%u','-1','%ld','%ld')",
                  (unsigned) ForumType,FirstPstCod,FirstPstCod);
         break;
      case For_FORUM_INSTITUTION_USRS:
      case For_FORUM_INSTITUTION_TCHS:
         sprintf (Query,"INSERT INTO forum_thread"
                        " (ForumType,Location,FirstPstCod,LastPstCod)"
                        " VALUES ('%u','%ld','%ld','%ld')",
                  (unsigned) ForumType,Gbl.Forum.Ins.InsCod,FirstPstCod,FirstPstCod);
         break;
      case For_FORUM_CENTRE_USRS:
      case For_FORUM_CENTRE_TCHS:
         sprintf (Query,"INSERT INTO forum_thread"
                        " (ForumType,Location,FirstPstCod,LastPstCod)"
                        " VALUES ('%u','%ld','%ld','%ld')",
                  (unsigned) ForumType,Gbl.Forum.Ctr.CtrCod,FirstPstCod,FirstPstCod);
         break;
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         sprintf (Query,"INSERT INTO forum_thread"
                        " (ForumType,Location,FirstPstCod,LastPstCod)"
                        " VALUES ('%u','%ld','%ld','%ld')",
                  (unsigned) ForumType,Gbl.Forum.Deg.DegCod,FirstPstCod,FirstPstCod);
         break;
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
         sprintf (Query,"INSERT INTO forum_thread"
                        " (ForumType,Location,FirstPstCod,LastPstCod)"
                        " VALUES ('%u','%ld','%ld','%ld')",
                  (unsigned) ForumType,Gbl.Forum.Crs.CrsCod,FirstPstCod,FirstPstCod);
         break;
     }
   return DB_QueryINSERTandReturnCode (Query,"can not create a new thread in a forum");
  }

/*****************************************************************************/
/*************** Delete a thread from the forum thread table *****************/
/*****************************************************************************/

void For_RemoveThreadOnly (long ThrCod)
  {
   char Query[512];

   /***** Indicate that this thread has not been read by anyone *****/
   For_DeleteThrFromReadThrs (ThrCod);

   /***** Remove thread code from thread clipboard *****/
   For_RemoveThrCodFromThrClipboard (ThrCod);

   /***** Delete thread from forum thread table *****/
   sprintf (Query,"DELETE FROM forum_thread WHERE ThrCod='%ld'",
            ThrCod);
   DB_QueryDELETE (Query,"can not remove a thread from a forum");
  }

/*****************************************************************************/
/*************** Delete a thread from the forum thread table *****************/
/*****************************************************************************/

void For_RemoveThreadAndItsPsts (long ThrCod)
  {
   char Query[512];

   /***** Delete banned posts in thread *****/
   sprintf (Query,"DELETE forum_disabled_post"
	          " FROM forum_post,forum_disabled_post"
                  " WHERE forum_post.ThrCod='%ld'"
                  " AND forum_post.PstCod=forum_disabled_post.PstCod",
            ThrCod);
   DB_QueryDELETE (Query,"can not unban the posts of a thread of a forum");

   /***** Delete thread posts *****/
   sprintf (Query,"DELETE FROM forum_post WHERE ThrCod='%ld'",
            ThrCod);
   DB_QueryDELETE (Query,"can not remove the posts of a thread of a forum");

   /***** Delete thread from forum thread table *****/
   For_RemoveThreadOnly (ThrCod);
  }

/*****************************************************************************/
/********************* Get the thread subject from a thread ******************/
/*****************************************************************************/

void For_GetThrSubject (long ThrCod,char *Subject,size_t MaxSize)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get subject of a thread from database *****/
   sprintf (Query,"SELECT forum_post.Subject FROM forum_thread,forum_post"
                  " WHERE forum_thread.ThrCod='%ld'"
                  " AND forum_thread.FirstPstCod=forum_post.PstCod",
            ThrCod);
   DB_QuerySELECT (Query,&mysql_res,"can not get the subject of a thread of a forum");

   /***** Write the subject of the thread *****/
   row = mysql_fetch_row (mysql_res);
   strncpy (Subject,row[0],MaxSize);
   Subject[MaxSize] = '\0';
   Str_LimitLengthHTMLStr (Subject,20);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************** Get the forum type of a post *********************/
/*****************************************************************************/

For_ForumType_t For_GetForumTypeOfAPost (long PstCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   For_ForumType_t ForumType = (For_ForumType_t) 0;
   unsigned UnsignedNum;

   /***** Get forum type of a forum from database *****/
   sprintf (Query,"SELECT forum_thread.ForumType"
	          " FROM forum_post,forum_thread"
                  " WHERE forum_post.PstCod='%ld'"
                  " AND forum_post.ThrCod=forum_thread.ThrCod",
            PstCod);

   /***** Check if there is a row with forum type *****/
   if (DB_QuerySELECT (Query,&mysql_res,"can not get the forum type of a post"))
     {
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%u",&UnsignedNum) != 1)
         Lay_ShowErrorAndExit ("Wrong type of forum.");
      if (UnsignedNum >= For_NUM_TYPES_FORUM)
         Lay_ShowErrorAndExit ("Wrong type of forum.");
      ForumType = (For_ForumType_t) UnsignedNum;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return ForumType;
  }

/*****************************************************************************/
/********* Modify the codes of the first and last posts of a thread **********/
/*****************************************************************************/

void For_UpdateThrFirstAndLastPst (long ThrCod,long FirstPstCod,long LastPstCod)
  {
   char Query[512];

   /***** Update the code of the first and last posts of a thread *****/
   sprintf (Query,"UPDATE forum_thread SET FirstPstCod='%ld',LastPstCod='%ld'"
                  " WHERE ThrCod='%ld'",
            FirstPstCod,LastPstCod,ThrCod);
   DB_QueryUPDATE (Query,"can not update a thread of a forum");
  }

/*****************************************************************************/
/************** Modify the code of the last post of a thread *****************/
/*****************************************************************************/

void For_UpdateThrLastPst (long ThrCod,long LastPstCod)
  {
   char Query[512];

   /***** Update the code of the last post of a thread *****/
   sprintf (Query,"UPDATE forum_thread SET LastPstCod='%ld' WHERE ThrCod='%ld'",
            LastPstCod,ThrCod);
   DB_QueryUPDATE (Query,"can not update a thread of a forum");
  }

/*****************************************************************************/
/**************** Get the code of the last post of a thread ******************/
/*****************************************************************************/

long For_GetLastPstCod (long ThrCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long LastPstCod;

   /***** Get the code of the last post of a thread from database *****/
   sprintf (Query,"SELECT PstCod FROM forum_post"
                  " WHERE ThrCod='%ld' ORDER BY CreatTime DESC LIMIT 1",
            ThrCod);
   DB_QuerySELECT (Query,&mysql_res,"can not get the most recent post of a thread of a forum");

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

static void For_UpdateThrReadTime (long ThrCod,time_t ReadTimeUTC)
  {
   char Query[256];

   /***** Insert or replace pair ThrCod-UsrCod in forum_thr_read *****/
   sprintf (Query,"REPLACE INTO forum_thr_read (ThrCod,UsrCod,ReadTime)"
                  " VALUES ('%ld','%ld',FROM_UNIXTIME('%ld'))",
            ThrCod,Gbl.Usrs.Me.UsrDat.UsrCod,(long) ReadTimeUTC);
   DB_QueryREPLACE (Query,"can not update the status of reading of a thread of a forum");
  }

/*****************************************************************************/
/**************** Get number of users that have read a thread ****************/
/*****************************************************************************/

static unsigned For_GetNumOfReadersOfThr (long ThrCod)
  {
   char Query[512];

   /***** Get number of distinct readers of a thread from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM forum_thr_read WHERE ThrCod='%ld'",
            ThrCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of readers of a thread of a forum");
  }

/*****************************************************************************/
/********** Get number of users that have write posts in a thread ************/
/*****************************************************************************/

static unsigned For_GetNumOfWritersInThr (long ThrCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumWriters;

   /***** Get number of distinct writers in a thread from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT UsrCod) FROM forum_post"
                  " WHERE ThrCod='%ld'",ThrCod);
   DB_QuerySELECT (Query,&mysql_res,"can not get the number of writers in a thread of a forum");

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
   char Query[512];

   /***** Get number of posts in a thread from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM forum_post"
	          " WHERE ThrCod='%ld'",
            ThrCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of posts in a thread of a forum");
  }

/*****************************************************************************/
/************** Get whether there are posts of mine in a thread **************/
/*****************************************************************************/

static unsigned For_GetNumMyPstInThr (long ThrCod)
  {
   char Query[128];

   /***** Get if I have write posts in a thread from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM forum_post"
                  " WHERE ThrCod='%ld' AND UsrCod='%ld'",
            ThrCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not check if you have written post in a thead of a forum");
  }

/*****************************************************************************/
/*********************** Get number of posts from a user *********************/
/*****************************************************************************/

unsigned long For_GetNumPostsUsr (long UsrCod)
  {
   char Query[128];

   /***** Get number of posts from a user from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM forum_post WHERE UsrCod='%ld'",
            UsrCod);
   return DB_QueryCOUNT (Query,"can not number of posts from a user");
  }

/*****************************************************************************/
/****************** Get thread read time for the current user ****************/
/*****************************************************************************/

static time_t For_GetThrReadTime (long ThrCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   time_t ReadTimeUTC;

   /***** Get read time of a thread from database *****/
   sprintf (Query,"SELECT UNIX_TIMESTAMP(ReadTime)"
                  " FROM forum_thr_read"
                  " WHERE ThrCod='%ld' AND UsrCod='%ld'",
            ThrCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get date of reading of a thread of a forum"))
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

void For_DeleteThrFromReadThrs (long ThrCod)
  {
   char Query[128];

   /***** Delete pairs ThrCod-UsrCod in forum_thr_read for a thread *****/
   sprintf (Query,"DELETE FROM forum_thr_read WHERE ThrCod='%ld'",
            ThrCod);
   DB_QueryDELETE (Query,"can not remove the status of reading of a thread of a forum");
  }

/*****************************************************************************/
/********************** Delete thread read status for an user ****************/
/*****************************************************************************/

void For_RemoveUsrFromReadThrs (long UsrCod)
  {
   char Query[128];

   /***** Delete pairs ThrCod-UsrCod in forum_thr_read for a user *****/
   sprintf (Query,"DELETE FROM forum_thr_read WHERE UsrCod='%ld'",
            UsrCod);
   DB_QueryDELETE (Query,"can not remove the status of reading by a user of all the threads of a forum");
  }

/*****************************************************************************/
/****************************** Show forum posts *****************************/
/*****************************************************************************/

static void For_ShowThreadPosts (long ThrCod,char *LastSubject)
  {
   extern const char *Txt_Thread;
   extern const char *Txt_There_are_new_posts;
   extern const char *Txt_No_new_posts;
   extern const char *Txt_Messages;
   bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS];
   struct ForumThread Thr;
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow,NumRows;
   unsigned NumPst = 0;		// Initialized to avoid warning
   unsigned NumPsts;
   time_t ReadTimeUTC;		// Read time of thread for the current user
   time_t CreatTimeUTC;		// Creation time of post
   struct Pagination Pagination;
   long PstCod;
   bool NewPst = false;
   bool ICanModerateForum = false;
   bool ICanMoveThreads = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);	// If I have permission to move threads...

   /***** Get data of the thread *****/
   Thr.ThrCod = ThrCod;
   For_GetThrData (&Thr);

   /***** Get the page number *****/
   Pag_GetParamPagNum (Pag_POSTS_FORUM);

   /***** Write title *****/
   /* Get if there is a thread ready to be moved */
   if (ICanMoveThreads)
      Gbl.Forum.ThreadToMove = For_GetThrInMyClipboard ();

   /* Get thread read time for the current user */
   ReadTimeUTC = For_GetThrReadTime (ThrCod);

   /* Table start */
   Lay_StartRoundFrame (NULL,Txt_Thread);

   /* Put a form to select which forums */
   For_PutFormWhichForums ();

   /* Write a link to top level of forums */
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\""
	              " style=\"display:inline-block;\">"
                      "<ul class=\"LIST_LEFT\">");
   For_WriteLinkToTopLevelOfForums ();

   /* Write a link to current forum */
   IsLastItemInLevel[1] = true;
   For_WriteLinkToAForum (Gbl.Forum.ForumType,true,1,IsLastItemInLevel);

   /* Write thread title */
   fprintf (Gbl.F.Out,"<li class=\"DAT\" style=\"height:25px;\">");
   IsLastItemInLevel[2] = true;
   Lay_IndentDependingOnLevel (2,IsLastItemInLevel);

   fprintf (Gbl.F.Out,"<img src=\"%s/%s16x16.gif\""
	              " alt=\"%s\" title=\"%s\""
                      " class=\"ICON20x20\" /> ",
            Gbl.Prefs.IconsURL,
            Thr.NumUnreadPosts ? "msg-unread" :
        	                 "msg-open",
            Thr.NumUnreadPosts ? Txt_There_are_new_posts :
        	                 Txt_No_new_posts,
	    Thr.NumUnreadPosts ? Txt_There_are_new_posts :
			         Txt_No_new_posts);
   For_WriteThrSubject (ThrCod);

   /***** Get posts of a thread from database *****/
   sprintf (Query,"SELECT PstCod,UNIX_TIMESTAMP(CreatTime)"
	          " FROM forum_post"
                  " WHERE ThrCod='%ld' ORDER BY PstCod",
            ThrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get posts of a thread");

   NumPsts = (unsigned) NumRows;

   /* Write number of posts and number of new posts */
   fprintf (Gbl.F.Out," ");
   Msg_WriteNumMsgs (NumPsts,0);

   fprintf (Gbl.F.Out,"</li>"
	              "</ul>"
	              "</div>");
   Lay_EndRoundFrame ();

   LastSubject[0] = '\0';
   if (NumPsts)		// If there are posts...
     {
      /***** Check if I can moderate posts in forum *****/
      switch (Gbl.Forum.ForumType)
        {
         case For_FORUM_SWAD_USRS:		case For_FORUM_SWAD_TCHS:
         case For_FORUM_GLOBAL_USRS:		case For_FORUM_GLOBAL_TCHS:
         case For_FORUM_CENTRE_USRS:		case For_FORUM_CENTRE_TCHS:
            ICanModerateForum = Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM;
            break;
         case For_FORUM_INSTITUTION_USRS:	case For_FORUM_INSTITUTION_TCHS:
            ICanModerateForum = Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM;
            break;
         case For_FORUM_DEGREE_USRS:		case For_FORUM_DEGREE_TCHS:
         					case For_FORUM_COURSE_TCHS:
            ICanModerateForum = (Gbl.Usrs.Me.LoggedRole == Rol_DEG_ADM ||
                                 Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);
            break;
         case For_FORUM_COURSE_USRS:
            ICanModerateForum = (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER ||
                                 Gbl.Usrs.Me.LoggedRole == Rol_DEG_ADM ||
                                 Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);
            break;
        }

      /***** Compute variables related to pagination *****/
      Pagination.NumItems = NumPsts;
      Pagination.CurrentPage = (int) Gbl.Pag.CurrentPage;
      Pag_CalculatePagination (&Pagination);
      Gbl.Pag.CurrentPage = (unsigned) Pagination.CurrentPage;

      /***** Write links to pages *****/
      if (Pagination.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (Pag_POSTS_FORUM,ThrCod,&Pagination);

      /***** Show posts from this page, the author and the date of last reply *****/
      Lay_StartRoundFrameTable (NULL,2,Txt_Messages);

      mysql_data_seek (mysql_res,(my_ulonglong) (Pagination.FirstItemVisible-1));
      for (NumRow = Pagination.FirstItemVisible;
           NumRow <= Pagination.LastItemVisible;
           NumRow++)
        {
         row = mysql_fetch_row (mysql_res);

         if (sscanf (row[0],"%ld",&PstCod) != 1)
            Lay_ShowErrorAndExit ("Wrong code of post.");

         CreatTimeUTC = Dat_GetUNIXTimeFromStr (row[1]);

         NumPst = (unsigned) NumRow;
         NewPst = (CreatTimeUTC > ReadTimeUTC);

         if (NewPst && NumRow == Pagination.LastItemVisible)
            /* Update forum_thr_read table indicating that this thread page and previous ones
               have been read and have no new posts for the current user
               (even if any previous pages have been no read actually) */
            For_UpdateThrReadTime (ThrCod,CreatTimeUTC);

         /* Show post */
         For_ShowAForumPost (&Thr,NumPst,PstCod,
                             (NumRow == NumRows),LastSubject,
                             NewPst,ICanModerateForum);

         /* Mark possible notification as seen */
         switch (Gbl.Forum.ForumType)
           {
            case For_FORUM_COURSE_TCHS:
            case For_FORUM_COURSE_USRS:
               Ntf_MarkNotifAsSeen (Ntf_EVENT_FORUM_POST_COURSE,
           	                    PstCod,Gbl.CurrentCrs.Crs.CrsCod,
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

      Lay_EndRoundFrameTable ();

      /***** Write again links to pages *****/
      if (Pagination.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (Pag_POSTS_FORUM,ThrCod,&Pagination);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************************** Show a post from forum *************************/
/*****************************************************************************/

static void For_ShowAForumPost (struct ForumThread *Thr,unsigned PstNum,long PstCod,
                                bool LastPst,char *LastSubject,
                                bool NewPst,bool ICanModerateForum)
  {
   extern const char *Txt_unread_MESSAGE;
   extern const char *Txt_MSG_Open;
   extern const char *Txt_no_subject;
   extern const char *Txt_Post_X_allowed;
   extern const char *Txt_Post_banned;
   extern const char *Txt_Post_X_banned;
   extern const char *Txt_Post_X_allowed_Click_to_ban_it;
   extern const char *Txt_Post_X_banned_Click_to_unban_it;
   extern const char *Txt_This_post_has_been_banned_probably_for_not_satisfy_the_rules_of_the_forums;
   struct UsrData UsrDat;
   time_t CreatTimeUTC;	// Creation time of a post
   char OriginalContent[Cns_MAX_BYTES_LONG_TEXT+1];
   char Subject[Cns_MAX_BYTES_SUBJECT+1];
   char Content[Cns_MAX_BYTES_LONG_TEXT+1];
   bool Enabled;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Check if post is enabled *****/
   Enabled = For_GetIfPstIsEnabled (PstCod);

   /***** Get data of post *****/
   For_GetPstData (PstCod,&UsrDat.UsrCod,&CreatTimeUTC,Subject,OriginalContent);
   if (Enabled)
     {
      /* Return this subject as last subject */
      strncpy (LastSubject,Subject,Cns_MAX_BYTES_SUBJECT);
      LastSubject[Cns_MAX_BYTES_SUBJECT] = '\0';
     }

   /***** Put an icon with post status *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s CENTER_TOP\" style=\"width:30px;\">"
                      "<img src=\"%s/%s16x16.gif\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICON20x20\" />"
                      "</td>",
            NewPst ? "MSG_TIT_BG_NEW" :
        	     "MSG_TIT_BG",
            Gbl.Prefs.IconsURL,
            NewPst ? "msg-unread" :
        	     "msg-open",
            NewPst ? Txt_unread_MESSAGE :
        	     Txt_MSG_Open,
            NewPst ? Txt_unread_MESSAGE :
        	     Txt_MSG_Open);

   /***** Write post number *****/
   Msg_WriteMsgNumber ((unsigned long) PstNum,NewPst);

   /***** Write date *****/
   Msg_WriteMsgDate (CreatTimeUTC,NewPst ? "MSG_TIT_BG_NEW" :
	                                   "MSG_TIT_BG");

   /***** Write subject *****/
   fprintf (Gbl.F.Out,"<td class=\"%s LEFT_TOP\">",
            NewPst ? "MSG_TIT_BG_NEW" :
        	     "MSG_TIT_BG");
   if (Enabled)
     {
      if (Subject[0])
         fprintf (Gbl.F.Out,"%s",Subject);
      else
         fprintf (Gbl.F.Out,"[%s]",Txt_no_subject);
     }
   else
      fprintf (Gbl.F.Out,"[%s]",Txt_Post_banned);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Form to remove post *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"CENTER_TOP\" style=\"width:22px;\">");
   if (LastPst && Gbl.Usrs.Me.UsrDat.UsrCod == UsrDat.UsrCod)
      // Post can be removed if post is the last (without answers) and it's mine
     {
      Act_FormStart (For_ActionsDelPstFor[Gbl.Forum.ForumType]);
      Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
      For_PutHiddenParamPstCod (PstCod);
      For_PutAllHiddenParamsForum ();
      For_PutHiddenParamThrCod (Thr->ThrCod);
      Lay_PutIconRemove ();
      Act_FormEnd ();
     }

   /***** Form to ban/unban post *****/
   else
     {
      if (ICanModerateForum)
        {
         Act_FormStart (Enabled ? For_ActionsDisPstFor[Gbl.Forum.ForumType] :
                                  For_ActionsEnbPstFor[Gbl.Forum.ForumType]);
         Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
         For_PutHiddenParamPstCod (PstCod);
         For_PutAllHiddenParamsForum ();
         For_PutHiddenParamThrCod (Thr->ThrCod);

         sprintf (Gbl.Title,Enabled ? Txt_Post_X_allowed_Click_to_ban_it :
                                      Txt_Post_X_banned_Click_to_unban_it,
                  PstNum);
         fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s-on64x64.png\""
                            " alt=\"%s\" title=\"%s\""
                            " class=\"ICON20x20\" />",
                  Gbl.Prefs.IconsURL,
                  Enabled ? "eye" :
                	    "eye-slash",
                  Gbl.Title,
                  Gbl.Title);
         Act_FormEnd ();
        }
      else
        {
         sprintf (Gbl.Title,Enabled ? Txt_Post_X_allowed :
                                      Txt_Post_X_banned,
                  PstNum);
         fprintf (Gbl.F.Out,"<span title=\"%s\">"
                            "<img src=\"%s/%s-off64x64.png\""
                            " alt=\"%s\" title=\"%s\""
                            " class=\"ICON20x20\" />"
                            "</span>",
                  Gbl.Title,
                  Gbl.Prefs.IconsURL,
                  Enabled ? "eye" :
                	    "eye-slash",
                  Gbl.Title,
                  Gbl.Title);
        }
     }

   /***** Write author or destinatary, and form to reply (in case of received post) *****/
   fprintf (Gbl.F.Out,"</td>"
	              "<td colspan=\"2\" class=\"LEFT_TOP\""
	              " style=\"width:150px;\">"
                      "<table class=\"CELLS_PAD_2\" style=\"width:150px;\">");

   /* Write author */
   fprintf (Gbl.F.Out,"<tr>");
   Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);
   Msg_WriteMsgAuthor (&UsrDat,125,16,"MSG_AUT",Enabled,NULL);
   fprintf (Gbl.F.Out,"</tr>");
   if (Enabled)
     {
      /* Write number of posts from this user */
      fprintf (Gbl.F.Out,"<tr>");
      For_WriteNumberOfPosts (Gbl.Forum.ForumType,UsrDat.UsrCod);
      fprintf (Gbl.F.Out,"</tr>");
     }

   /***** Write post content *****/
   fprintf (Gbl.F.Out,"</table>"
	              "</td>"
	              "<td class=\"MSG_TXT LEFT_TOP\">");
   if (Enabled)
     {
      strncpy (Content,OriginalContent,Cns_MAX_BYTES_LONG_TEXT);
      Content[Cns_MAX_BYTES_LONG_TEXT] = '\0';
      Msg_WriteMsgContent (Content,Cns_MAX_BYTES_LONG_TEXT,true,false);
     }
   else
      fprintf (Gbl.F.Out,"%s",Txt_This_post_has_been_banned_probably_for_not_satisfy_the_rules_of_the_forums);
   fprintf (Gbl.F.Out,"<br />&nbsp;"
	              "</td>"
	              "</tr>");

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/*************************** Get data of a forum post ************************/
/*****************************************************************************/

static void For_GetPstData (long PstCod,long *UsrCod,time_t *CreatTimeUTC,
                            char *Subject, char *Content)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;

   /***** Get data of a post from database *****/
   sprintf (Query,"SELECT UsrCod,UNIX_TIMESTAMP(CreatTime),Subject,Content"
                  " FROM forum_post WHERE PstCod='%ld'",
            PstCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get data of a post");

   /***** Result should have a unique row *****/
   if (NumRows != 1)
      Lay_ShowErrorAndExit ("Internal error in database when getting data of a post.");

   /***** Get number of rows *****/
   row = mysql_fetch_row (mysql_res);

   /****** Get author code (row[1]) *****/
   *UsrCod = Str_ConvertStrCodToLongCod (row[0]);

   /****** Get creation time (row[1]) *****/
   *CreatTimeUTC = Dat_GetUNIXTimeFromStr (row[1]);

   /****** Get subject (row[2]) *****/
   strcpy (Subject,row[2]);

   /****** Get location (row[3]) *****/
   strcpy (Content,row[3]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Get summary and content for a forum post ******************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error

void For_GetSummaryAndContentForumPst (char *SummaryStr,char **ContentStr,
                                       long PstCod,
                                       unsigned MaxChars,bool GetContent)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Get subject of message from database *****/
   sprintf (Query,"SELECT Subject,Content FROM forum_post"
                  " WHERE PstCod='%ld'",PstCod);
   if (!mysql_query (&Gbl.mysql,Query))
      if ((mysql_res = mysql_store_result (&Gbl.mysql)) != NULL)
        {
         /***** Result should have a unique row *****/
         if (mysql_num_rows (mysql_res) == 1)
           {
            /***** Get subject and content of the message *****/
            row = mysql_fetch_row (mysql_res);

            /***** Copy subject *****/
            strcpy (SummaryStr,row[0]);
            if (MaxChars)
               Str_LimitLengthHTMLStr (SummaryStr,MaxChars);

            /***** Copy content *****/
            if (GetContent)
              {
               if ((*ContentStr = (char *) malloc (strlen (row[1])+1)) == NULL)
                  Lay_ShowErrorAndExit ("Error allocating memory for notification content.");
               strcpy (*ContentStr,row[1]);
              }
           }
         mysql_free_result (mysql_res);
        }
  }

/*****************************************************************************/
/*************** Write number of posts in a forum of an user *****************/
/*****************************************************************************/

static void For_WriteNumberOfPosts (For_ForumType_t ForumType,long UsrCod)
  {
   extern const char *Txt_post;
   extern const char *Txt_posts;
   char SubQuery[256];
   char Query[1024];
   unsigned NumPsts;

   /***** Star table cell *****/
   fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"MSG_AUT CENTER_TOP\""
	              " style=\"width:150px;\">");

   /***** Get number of posts from database *****/
   switch (ForumType)
     {
      case For_FORUM_INSTITUTION_USRS:
      case For_FORUM_INSTITUTION_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Ins.InsCod);
         break;
      case For_FORUM_CENTRE_USRS:
      case For_FORUM_CENTRE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Ctr.CtrCod);
         break;
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Deg.DegCod);
         break;
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Crs.CrsCod);
         break;
      default:
         SubQuery[0] = '\0';
         break;
     }
   sprintf (Query,"SELECT COUNT(*) FROM forum_post,forum_thread"
                  " WHERE forum_post.UsrCod='%ld'"
                  " AND forum_post.ThrCod=forum_thread.ThrCod"
                  " AND forum_thread.ForumType='%u'%s",
            UsrCod,(unsigned) ForumType,SubQuery);
   NumPsts = (unsigned) DB_QueryCOUNT (Query,"can not get the number of posts of a user in a forum");

   /***** Write number of threads and number of posts *****/
   if (NumPsts == 1)
      fprintf (Gbl.F.Out,"[1 %s]",Txt_post);
   else
      fprintf (Gbl.F.Out,"[%u %s]",NumPsts,Txt_posts);

   /***** End table cell *****/
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/************ Put all the hidden parameters related to forums ****************/
/*****************************************************************************/

void For_PutAllHiddenParamsForum (void)
  {
   For_PutParamWhichForum ();
   For_PutParamForumOrder ();
   For_PutParamsForumInsDegCrs ();
  }

/*****************************************************************************/
/*********** Put a hidden parameter with which forum I want to see ***********/
/*****************************************************************************/

static void For_PutParamWhichForum (void)
  {
   Par_PutHiddenParamUnsigned ("WhichForum",(unsigned) Gbl.Forum.WhichForums);
  }

/*****************************************************************************/
/******** Put a hidden parameter with the order criterium for forums *********/
/*****************************************************************************/

static void For_PutParamForumOrder (void)
  {
   Par_PutHiddenParamUnsigned ("Order",(unsigned) Gbl.Forum.SelectedOrderType);
  }

/*****************************************************************************/
/************** Put hidden parameters related to             *****************/
/************** forum institution, centre, degree and course *****************/
/*****************************************************************************/

static void For_PutParamsForumInsDegCrs (void)
  {
   /***** Put a hidden parameter with the institution of the forum *****/
   if (Gbl.Forum.Ins.InsCod > 0)
      Par_PutHiddenParamLong ("ForInsCod",Gbl.Forum.Ins.InsCod);

   /***** Put a hidden parameter with the centre of the forum *****/
   if (Gbl.Forum.Ctr.CtrCod > 0)
      Par_PutHiddenParamLong ("ForCtrCod",Gbl.Forum.Ctr.CtrCod);

   /***** Put a hidden parameter with the degree of the forum *****/
   if (Gbl.Forum.Deg.DegCod > 0)
      Par_PutHiddenParamLong ("ForDegCod",Gbl.Forum.Deg.DegCod);

   /***** Put a hidden parameter with the course of the forum *****/
   if (Gbl.Forum.Crs.CrsCod > 0)
      Par_PutHiddenParamLong ("ForCrsCod",Gbl.Forum.Crs.CrsCod);
  }

/*****************************************************************************/
/***************************** Set the type of forum *************************/
/*****************************************************************************/

void For_SetForumTypeAndRestrictAccess (void)
  {
   extern const char *Txt_You_dont_have_permission_to_access_to_this_forum;
   bool ICanSeeForum = false;

   switch (Gbl.Action.Act)
     {
      case ActSeeForSWAUsr:	case ActSeePstForSWAUsr:
      case ActRcvThrForSWAUsr:	case ActRcvRepForSWAUsr:
      case ActReqDelThrSWAUsr:	case ActDelThrForSWAUsr:
      case ActCutThrForSWAUsr:	case ActPasThrForSWAUsr:
      case ActDelPstForSWAUsr:
      case ActEnbPstForSWAUsr:	case ActDisPstForSWAUsr:
         Gbl.Forum.ForumType = For_FORUM_SWAD_USRS;
         Gbl.Forum.Level = 1;
         break;
      case ActSeeForSWATch:	case ActSeePstForSWATch:
      case ActRcvThrForSWATch:	case ActRcvRepForSWATch:
      case ActReqDelThrSWATch:	case ActDelThrForSWATch:
      case ActCutThrForSWATch:	case ActPasThrForSWATch:
      case ActDelPstForSWATch:
      case ActEnbPstForSWATch:	case ActDisPstForSWATch:
         Gbl.Forum.ForumType = For_FORUM_SWAD_TCHS;
         Gbl.Forum.Level = 1;
         break;
      case ActSeeForGenUsr:	case ActSeePstForGenUsr:
      case ActRcvThrForGenUsr:	case ActRcvRepForGenUsr:
      case ActReqDelThrGenUsr:	case ActDelThrForGenUsr:
      case ActCutThrForGenUsr:	case ActPasThrForGenUsr:
      case ActDelPstForGenUsr:
      case ActEnbPstForGenUsr:	case ActDisPstForGenUsr:
         Gbl.Forum.ForumType = For_FORUM_GLOBAL_USRS;
         Gbl.Forum.Level = 1;
         break;
      case ActSeeForGenTch:	case ActSeePstForGenTch:
      case ActRcvThrForGenTch:	case ActRcvRepForGenTch:
      case ActReqDelThrGenTch:	case ActDelThrForGenTch:
      case ActCutThrForGenTch:	case ActPasThrForGenTch:
      case ActDelPstForGenTch:
      case ActEnbPstForGenTch:	case ActDisPstForGenTch:
         Gbl.Forum.ForumType = For_FORUM_GLOBAL_TCHS;
         Gbl.Forum.Level = 1;
         break;
      case ActSeeForInsUsr:	case ActSeePstForInsUsr:
      case ActRcvThrForInsUsr:	case ActRcvRepForInsUsr:
      case ActReqDelThrInsUsr:	case ActDelThrForInsUsr:
      case ActCutThrForInsUsr:	case ActPasThrForInsUsr:
      case ActDelPstForInsUsr:
      case ActEnbPstForInsUsr:	case ActDisPstForInsUsr:
         Gbl.Forum.ForumType = For_FORUM_INSTITUTION_USRS;
         Gbl.Forum.Level = 1;
         break;
      case ActSeeForInsTch:	case ActSeePstForInsTch:
      case ActRcvThrForInsTch:	case ActRcvRepForInsTch:
      case ActReqDelThrInsTch:	case ActDelThrForInsTch:
      case ActCutThrForInsTch:	case ActPasThrForInsTch:
      case ActDelPstForInsTch:
      case ActEnbPstForInsTch:	case ActDisPstForInsTch:
	 Gbl.Forum.ForumType = For_FORUM_INSTITUTION_TCHS;
         Gbl.Forum.Level = 1;
	 break;
      case ActSeeForCtrUsr:	case ActSeePstForCtrUsr:
      case ActRcvThrForCtrUsr:	case ActRcvRepForCtrUsr:
      case ActReqDelThrCtrUsr:	case ActDelThrForCtrUsr:
      case ActCutThrForCtrUsr:	case ActPasThrForCtrUsr:
      case ActDelPstForCtrUsr:
      case ActEnbPstForCtrUsr:	case ActDisPstForCtrUsr:
         Gbl.Forum.ForumType = For_FORUM_CENTRE_USRS;
         Gbl.Forum.Level = 1;
         break;
      case ActSeeForCtrTch:	case ActSeePstForCtrTch:
      case ActRcvThrForCtrTch:	case ActRcvRepForCtrTch:
      case ActReqDelThrCtrTch:	case ActDelThrForCtrTch:
      case ActCutThrForCtrTch:	case ActPasThrForCtrTch:
      case ActDelPstForCtrTch:
      case ActEnbPstForCtrTch:	case ActDisPstForCtrTch:
	 Gbl.Forum.ForumType = For_FORUM_CENTRE_TCHS;
         Gbl.Forum.Level = 1;
	 break;
      case ActSeeForDegUsr:	case ActSeePstForDegUsr:
      case ActRcvThrForDegUsr:	case ActRcvRepForDegUsr:
      case ActReqDelThrDegUsr:	case ActDelThrForDegUsr:
      case ActCutThrForDegUsr:	case ActPasThrForDegUsr:
      case ActDelPstForDegUsr:
      case ActEnbPstForDegUsr:	case ActDisPstForDegUsr:
         Gbl.Forum.ForumType = For_FORUM_DEGREE_USRS;
         Gbl.Forum.Level = 2;
         break;
      case ActSeeForDegTch:	case ActSeePstForDegTch:
      case ActRcvThrForDegTch:	case ActRcvRepForDegTch:
      case ActReqDelThrDegTch:	case ActDelThrForDegTch:
      case ActCutThrForDegTch:	case ActPasThrForDegTch:
      case ActDelPstForDegTch:
      case ActEnbPstForDegTch:	case ActDisPstForDegTch:
	 Gbl.Forum.ForumType = For_FORUM_DEGREE_TCHS;
         Gbl.Forum.Level = 2;
	 break;
      case ActSeeForCrsUsr:	case ActSeePstForCrsUsr:
      case ActRcvThrForCrsUsr:	case ActRcvRepForCrsUsr:
      case ActReqDelThrCrsUsr:	case ActDelThrForCrsUsr:
      case ActCutThrForCrsUsr:	case ActPasThrForCrsUsr:
      case ActDelPstForCrsUsr:
      case ActEnbPstForCrsUsr:	case ActDisPstForCrsUsr:
         Gbl.Forum.ForumType = For_FORUM_COURSE_USRS;
         Gbl.Forum.Level = 3;
         break;
      case ActSeeForCrsTch:	case ActSeePstForCrsTch:
      case ActRcvThrForCrsTch:	case ActRcvRepForCrsTch:
      case ActReqDelThrCrsTch:	case ActDelThrForCrsTch:
      case ActCutThrForCrsTch:	case ActPasThrForCrsTch:
      case ActDelPstForCrsTch:
      case ActEnbPstForCrsTch:	case ActDisPstForCrsTch:
         Gbl.Forum.ForumType = For_FORUM_COURSE_TCHS;
         Gbl.Forum.Level = 3;
         break;
     }

   /***** Restrict access *****/
   switch (Gbl.Forum.ForumType)
     {
      case For_FORUM_COURSE_USRS:
         ICanSeeForum = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ||
                         Rol_GetMyRoleInCrs (Gbl.Forum.Crs.CrsCod) >= Rol_STUDENT);
         break;
      case For_FORUM_COURSE_TCHS:
         ICanSeeForum = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ||
                         Rol_GetMyRoleInCrs (Gbl.Forum.Crs.CrsCod) >= Rol_TEACHER);
         break;
      case For_FORUM_DEGREE_USRS:
         ICanSeeForum = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ||
                         Rol_GetMyMaxRoleInDeg (Gbl.Forum.Deg.DegCod) >= Rol_STUDENT);
         break;
      case For_FORUM_DEGREE_TCHS:
         ICanSeeForum = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ||
                         Rol_GetMyMaxRoleInDeg (Gbl.Forum.Deg.DegCod) >= Rol_TEACHER);
         break;
      case For_FORUM_CENTRE_USRS:
         ICanSeeForum = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ||
                         Rol_GetMyMaxRoleInCtr (Gbl.Forum.Ctr.CtrCod) >= Rol_STUDENT);
         break;
      case For_FORUM_CENTRE_TCHS:
         ICanSeeForum = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ||
                         Rol_GetMyMaxRoleInCtr (Gbl.Forum.Ctr.CtrCod) >= Rol_TEACHER);
         break;
      case For_FORUM_INSTITUTION_USRS:
         ICanSeeForum = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ||
                         Rol_GetMyMaxRoleInIns (Gbl.Forum.Ins.InsCod) >= Rol_STUDENT);
         break;
      case For_FORUM_INSTITUTION_TCHS:
         ICanSeeForum = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ||
                         Rol_GetMyMaxRoleInIns (Gbl.Forum.Ins.InsCod) >= Rol_TEACHER);
         break;
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM_SWAD_USRS:
         ICanSeeForum = true;
         break;
      case For_FORUM_GLOBAL_TCHS:
      case For_FORUM_SWAD_TCHS:
         ICanSeeForum = (Gbl.Usrs.Me.UsrDat.Roles >= (1 << Rol_TEACHER));
         break;
     }
   if (!ICanSeeForum)
      Lay_ShowErrorAndExit (Txt_You_dont_have_permission_to_access_to_this_forum);
  }

/*****************************************************************************/
/************************** Show list of available forums ********************/
/*****************************************************************************/

void For_ShowForumList (void)
  {
   extern const char *Txt_Forums;
   bool ICanMoveThreads = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);	// If I have permission to move threads...
   bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS];
   unsigned NumMyIns;
   MYSQL_RES *mysql_resCtr;
   MYSQL_RES *mysql_resDeg;
   MYSQL_RES *mysql_resCrs;
   MYSQL_ROW row;
   unsigned NumCtr,NumCtrs;
   unsigned NumDeg,NumDegs;
   unsigned NumCrs,NumCrss;
   bool ICanSeeInsForum;
   bool ICanSeeCtrForum;
   bool ICanSeeDegForum;

   /***** Get if there is a thread ready to be moved *****/
   if (ICanMoveThreads)
      Gbl.Forum.ThreadToMove = For_GetThrInMyClipboard ();

   /***** Get order type, degree and course of the forum *****/
   For_GetParamsForum ();

   /***** Fill the list with the institutions I belong to *****/
   Usr_GetMyInstitutions ();

   /***** Table start *****/
   Lay_StartRoundFrame (NULL,Txt_Forums);

   /***** Put a form to select which forums *****/
   For_PutFormWhichForums ();

   /***** Write a link to top level of forums *****/
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\""
	              " style=\"display:inline-block;\">"
                      "<ul class=\"LIST_LEFT\">");
   For_WriteLinkToTopLevelOfForums ();

   /***** Links to global forums *****/
   For_WriteLinksToGblForums (IsLastItemInLevel);
   switch (Gbl.Forum.WhichForums)
     {
      case For_ONLY_CURRENT_FORUMS:
	 if (Gbl.Usrs.Me.LoggedRole >= Rol_DEG_ADM)
	    ICanSeeInsForum = true;
	 else
            ICanSeeInsForum = Usr_CheckIfIBelongToIns (Gbl.CurrentIns.Ins.InsCod);

         /***** Links to forums about the platform *****/
         For_WriteLinksToPlatformForums (!ICanSeeInsForum,IsLastItemInLevel);

         if (ICanSeeInsForum)
           {
            if (Gbl.Usrs.Me.LoggedRole >= Rol_DEG_ADM)
	       ICanSeeCtrForum = true;
	    else
	       ICanSeeCtrForum = Usr_CheckIfIBelongToCtr (Gbl.CurrentCtr.Ctr.CtrCod);

	    /***** Links to forums of current institution *****/
	    if (For_WriteLinksToInsForums (Gbl.CurrentIns.Ins.InsCod,true,IsLastItemInLevel) > 0)
               if (ICanSeeCtrForum)
        	 {
        	  if (Gbl.Usrs.Me.LoggedRole >= Rol_DEG_ADM)
		     ICanSeeDegForum = true;
		  else
		     ICanSeeDegForum = Usr_CheckIfIBelongToDeg (Gbl.CurrentDeg.Deg.DegCod);

		  /***** Links to forums of current centre *****/
		  if (For_WriteLinksToCtrForums (Gbl.CurrentCtr.Ctr.CtrCod,true,IsLastItemInLevel) > 0)
		     if (ICanSeeDegForum)
			/***** Links to forums of current degree *****/
			if (For_WriteLinksToDegForums (Gbl.CurrentDeg.Deg.DegCod,true,IsLastItemInLevel) > 0)
			   if (Gbl.Usrs.Me.IBelongToCurrentCrs ||
			       Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
			      /***** Links to forums of current degree *****/
			      For_WriteLinksToCrsForums (Gbl.CurrentCrs.Crs.CrsCod,true,IsLastItemInLevel);
        	 }
           }
         break;
      case For_ALL_MY_FORUMS:
         /***** Links to forums about the platform *****/
         For_WriteLinksToPlatformForums ((Gbl.Usrs.Me.MyInstitutions.Num == 0),IsLastItemInLevel);

         /***** Links to forums of users from my institutions, the degrees in each institution and the courses in each degree *****/
         for (NumMyIns = 0;
              NumMyIns < Gbl.Usrs.Me.MyInstitutions.Num;
              NumMyIns++)
           {
            /* Links to forums of this institution */
            For_WriteLinksToInsForums (Gbl.Usrs.Me.MyInstitutions.Inss[NumMyIns].InsCod,(NumMyIns == Gbl.Usrs.Me.MyInstitutions.Num-1),IsLastItemInLevel);

            /* Get my centres in this institution from database */
            if ((NumCtrs = Usr_GetCtrsFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Forum.Ins.InsCod,&mysql_resCtr)) > 0) // Centres found in this institution
               for (NumCtr = 0;
        	    NumCtr < NumCtrs;
        	    NumCtr++)
                 {
                  /* Get next centre */
                  row = mysql_fetch_row (mysql_resCtr);

                  /* Links to forums of this centre */
                  if (For_WriteLinksToCtrForums (Str_ConvertStrCodToLongCod (row[0]),(NumCtr == NumCtrs-1),IsLastItemInLevel) > 0)
                    {
		     /* Get my degrees in this institution from database */
		     if ((NumDegs = Usr_GetDegsFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Forum.Ctr.CtrCod,&mysql_resDeg)) > 0) // Degrees found in this centre
			for (NumDeg = 0;
			     NumDeg < NumDegs;
			     NumDeg++)
			  {
			   /* Get next degree */
			   row = mysql_fetch_row (mysql_resDeg);

			   /* Links to forums of this degree */
			   if (For_WriteLinksToDegForums (Str_ConvertStrCodToLongCod (row[0]),(NumDeg == NumDegs-1),IsLastItemInLevel) > 0)
			     {
			      /* Get my courses in this degree from database */
			      if ((NumCrss = Usr_GetCrssFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Forum.Deg.DegCod,&mysql_resCrs)) > 0) // Courses found in this degree
				 for (NumCrs = 0;
				      NumCrs < NumCrss;
				      NumCrs++)
				   {
				    /* Get next course */
				    row = mysql_fetch_row (mysql_resCrs);

				    /* Links to forums of this course */
				    For_WriteLinksToCrsForums (Str_ConvertStrCodToLongCod (row[0]),(NumCrs == NumCrss-1),IsLastItemInLevel);
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

   /***** End table *****/
   fprintf (Gbl.F.Out,"</ul>"
	              "</div>");
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/**************** Write title and link to top level of forums ****************/
/*****************************************************************************/

static void For_PutFormWhichForums (void)
  {
   extern const char *Txt_FORUM_WHICH_FORUM[For_NUM_WHICH_FORUMS];
   For_WhichForums_t WhichForums;

   /***** Form to select which forums I want to see
          (all my forums or only the forums of current institution/degree/course) *****/
   Act_FormStart (ActSeeFor);
   For_PutParamForumOrder ();
   For_PutParamsForumInsDegCrs ();
   fprintf (Gbl.F.Out,"<div style=\"margin:12px 0;\">"
	              "<ul class=\"LIST_CENTER\">");

   for (WhichForums = (For_WhichForums_t) 0;
	WhichForums < For_NUM_WHICH_FORUMS;
	WhichForums++)
     {
      fprintf (Gbl.F.Out,"<li class=\"DAT LEFT_MIDDLE\""
	                 " style=\"display:inline;\">"
                         "<input type=\"radio\" name=\"WhichForum\" value=\"%u\"",
               (unsigned) WhichForums);
      if (WhichForums == Gbl.Forum.WhichForums)
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," onclick=\"document.getElementById('%s').submit();\" />"
	                 "%s"
                         "</li>",
               Gbl.Form.Id,Txt_FORUM_WHICH_FORUM[WhichForums]);
     }
   fprintf (Gbl.F.Out,"</ul>"
	              "</div>");
   Act_FormEnd ();
  }

/*****************************************************************************/
/**************** Write title and link to top level of forums ****************/
/*****************************************************************************/

static void For_WriteLinkToTopLevelOfForums (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Forums;

   fprintf (Gbl.F.Out,"<li style=\"height:25px;\">");
   Act_FormStart (ActSeeFor);
   For_PutAllHiddenParamsForum ();
   Act_LinkFormSubmit (Txt_Forums,The_ClassForm[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"<img src=\"%s/forum64x64.gif\""
	              " alt=\"%s\" title=\"%s\""
	              " class=\"ICON20x20\" />"
                      "&nbsp;%s"
                      "</a>",
            Gbl.Prefs.IconsURL,
            Txt_Forums,Txt_Forums,
            Txt_Forums);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</li>");
  }

/*****************************************************************************/
/************************* Write links to global forums **********************/
/*****************************************************************************/

static void For_WriteLinksToGblForums (bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS])
  {
   /***** Link to forum global *****/
   IsLastItemInLevel[1] = false;
   For_WriteLinkToAForum (For_FORUM_GLOBAL_USRS,false,1,IsLastItemInLevel);

   /***** Link to forum of teachers global *****/
   if (Gbl.Usrs.Me.UsrDat.Roles >= (1 << Rol_TEACHER))
     {
      IsLastItemInLevel[1] = false;
      For_WriteLinkToAForum (For_FORUM_GLOBAL_TCHS,false,1,IsLastItemInLevel);
     }
  }

/*****************************************************************************/
/****************** Write links to forums about the platform *****************/
/*****************************************************************************/

static void For_WriteLinksToPlatformForums (bool IsLastForum,bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS])
  {
   bool ICanSeeTeacherForum = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ||
	                       Gbl.Usrs.Me.UsrDat.Roles >= (1 << Rol_TEACHER));

   /***** Link to forum of users about the platform *****/
   IsLastItemInLevel[1] = (IsLastForum && !ICanSeeTeacherForum);
   For_WriteLinkToAForum (For_FORUM_SWAD_USRS,false,1,IsLastItemInLevel);

   /***** Link to forum of teachers about the platform *****/
   if (ICanSeeTeacherForum)
     {
      IsLastItemInLevel[1] = IsLastForum;
      For_WriteLinkToAForum (For_FORUM_SWAD_TCHS,false,1,IsLastItemInLevel);
     }
  }

/*****************************************************************************/
/********************** Write links to institution forums ********************/
/*****************************************************************************/
// Returns institution code

static long For_WriteLinksToInsForums (long InsCod,bool IsLastIns,bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS])
  {
   bool ICanSeeTeacherForum;

   if (InsCod > 0)
     {
      ICanSeeTeacherForum = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ||
	                     Rol_GetMyMaxRoleInIns (InsCod) >= Rol_TEACHER);

      /***** Get data of this institution *****/
      Gbl.Forum.Ins.InsCod = InsCod;
      if (!Ins_GetDataOfInstitutionByCod (&Gbl.Forum.Ins,Ins_GET_BASIC_DATA))
         Lay_ShowErrorAndExit ("Institution not found.");

      /***** Link to the forum of users from this institution *****/
      IsLastItemInLevel[1] = (IsLastIns && !ICanSeeTeacherForum);
      For_WriteLinkToAForum (For_FORUM_INSTITUTION_USRS,false,1,IsLastItemInLevel);

      /***** Link to forum of teachers from this institution *****/
      if (ICanSeeTeacherForum)
        {
         IsLastItemInLevel[1] = IsLastIns;
         For_WriteLinkToAForum (For_FORUM_INSTITUTION_TCHS,false,1,IsLastItemInLevel);
        }
     }
   return InsCod;
  }

/*****************************************************************************/
/************************ Write links to centre forums ***********************/
/*****************************************************************************/
// Returns centre code

static long For_WriteLinksToCtrForums (long CtrCod,bool IsLastCtr,bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS])
  {
   bool ICanSeeTeacherForum;

   if (CtrCod > 0)
     {
      ICanSeeTeacherForum = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ||
	                     Rol_GetMyMaxRoleInCtr (CtrCod) >= Rol_TEACHER);

      /***** Get data of this centre *****/
      Gbl.Forum.Ctr.CtrCod = CtrCod;
      if (!Ctr_GetDataOfCentreByCod (&Gbl.Forum.Ctr))
         Lay_ShowErrorAndExit ("Centre not found.");

      /***** Link to the forum of users from this centre *****/
      IsLastItemInLevel[2] = (IsLastCtr && !ICanSeeTeacherForum);
      For_WriteLinkToAForum (For_FORUM_CENTRE_USRS,false,2,IsLastItemInLevel);

      /***** Link to forum of teachers from this centre *****/
      if (ICanSeeTeacherForum)
        {
         IsLastItemInLevel[2] = IsLastCtr;
         For_WriteLinkToAForum (For_FORUM_CENTRE_TCHS,false,2,IsLastItemInLevel);
        }
     }
   return CtrCod;
  }

/*****************************************************************************/
/************************ Write links to degree forums ***********************/
/*****************************************************************************/
// Returns degree code

static long For_WriteLinksToDegForums (long DegCod,bool IsLastDeg,bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS])
  {
   bool ICanSeeTeacherForum;

   if (DegCod > 0)
     {
      ICanSeeTeacherForum = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ||
	                     Rol_GetMyMaxRoleInDeg (DegCod) >= Rol_TEACHER);

      /***** Get data of this degree *****/
      Gbl.Forum.Deg.DegCod = DegCod;
      if (!Deg_GetDataOfDegreeByCod (&Gbl.Forum.Deg))
         Lay_ShowErrorAndExit ("Degree not found.");

      /***** Link to the forum of users from this degree *****/
      IsLastItemInLevel[3] = (IsLastDeg && !ICanSeeTeacherForum);
      For_WriteLinkToAForum (For_FORUM_DEGREE_USRS,false,3,IsLastItemInLevel);

      /***** Link to forum of teachers from this degree *****/
      if (ICanSeeTeacherForum)
        {
         IsLastItemInLevel[3] = IsLastDeg;
         For_WriteLinkToAForum (For_FORUM_DEGREE_TCHS,false,3,IsLastItemInLevel);
        }
     }
   return DegCod;
  }

/*****************************************************************************/
/************************ Write links to course forums ***********************/
/*****************************************************************************/
// Returns course code

static long For_WriteLinksToCrsForums (long CrsCod,bool IsLastCrs,bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS])
  {
   bool ICanSeeTeacherForum;

   if (CrsCod > 0)
     {
      ICanSeeTeacherForum = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ||
	                     Rol_GetMyRoleInCrs (CrsCod) >= Rol_TEACHER);

      /***** Get data of this course *****/
      Gbl.Forum.Crs.CrsCod = CrsCod;
      if (!Crs_GetDataOfCourseByCod (&Gbl.Forum.Crs))
         Lay_ShowErrorAndExit ("Course not found.");

      /***** Link to the forum of users from this course *****/
      IsLastItemInLevel[4] = (IsLastCrs && !ICanSeeTeacherForum);
      For_WriteLinkToAForum (For_FORUM_COURSE_USRS,false,4,IsLastItemInLevel);

      /***** Link to forum of teachers from this course *****/
      if (ICanSeeTeacherForum)
        {
         IsLastItemInLevel[4] = IsLastCrs;
         For_WriteLinkToAForum (For_FORUM_COURSE_TCHS,false,4,IsLastItemInLevel);
        }
     }
   return CrsCod;
  }

/*****************************************************************************/
/********************** Write title and link to a forum **********************/
/*****************************************************************************/

static void For_WriteLinkToAForum (For_ForumType_t ForumType,bool ShowNumOfPosts,
                                   unsigned Level,bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS])
  {
   char Icon[512];
   char ForumName[512];

   For_SetForumName (ForumType,
	             &Gbl.Forum.Ins,
	             &Gbl.Forum.Ctr,
	             &Gbl.Forum.Deg,
	             &Gbl.Forum.Crs,
	             ForumName,Gbl.Prefs.Language,true);

   switch (ForumType)
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM_GLOBAL_TCHS:
         sprintf (Icon,"<img src=\"%s/forum64x64.gif\""
                       " alt=\"%s\" title=\"%s\""
                       " class=\"ICON20x20\" />",
                  Gbl.Prefs.IconsURL,
                  ForumName,ForumName);
         break;
      case For_FORUM_SWAD_USRS:
      case For_FORUM_SWAD_TCHS:
         sprintf (Icon,"<img src=\"%s/swad64x64.gif\""
                       " alt=\"%s\" title=\"%s\""
                       " class=\"ICON20x20\" />",
                  Gbl.Prefs.IconsURL,
                  ForumName,ForumName);
         break;
      case For_FORUM_INSTITUTION_USRS:
      case For_FORUM_INSTITUTION_TCHS:
      case For_FORUM_CENTRE_USRS:
      case For_FORUM_CENTRE_TCHS:
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         break;
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
         sprintf (Icon,"<img src=\"%s/dot64x64.png\""
                       " alt=\"%s\" title=\"%s\""
                       " class=\"ICON20x20\" />",
                  Gbl.Prefs.IconsURL,
                  ForumName,ForumName);
         break;
     }

   For_WriteLinkToForum (ForumType,For_ActionsSeeFor[ForumType],Icon,ForumName,ShowNumOfPosts,
                         Level,IsLastItemInLevel);
  }

/*****************************************************************************/
/************************** Set the name of a forum **************************/
/*****************************************************************************/

void For_SetForumName (For_ForumType_t ForumType,
                       struct Institution *Ins,
                       struct Centre *Ctr,
                       struct Degree *Deg,
                       struct Course *Crs,
                       char *ForumName,Txt_Language_t Language,bool UseHTMLEntities)
  {
   extern const char *Txt_General;
   extern const char *Txt_General_NO_HTML[1+Txt_NUM_LANGUAGES];
   extern const char *Txt_only_teachers;
   extern const char *Txt_only_teachers_NO_HTML[1+Txt_NUM_LANGUAGES];

   switch (ForumType)
     {
      case For_FORUM_COURSE_USRS:
         strcpy (ForumName,Crs->ShortName);
         break;
      case For_FORUM_COURSE_TCHS:
         sprintf (ForumName,"%s%s",Crs->ShortName,
                  UseHTMLEntities ? Txt_only_teachers :
                                    Txt_only_teachers_NO_HTML[Language]);
         break;
      case For_FORUM_DEGREE_USRS:
         strcpy (ForumName,Deg->ShortName);
         break;
      case For_FORUM_DEGREE_TCHS:
         sprintf (ForumName,"%s%s",Deg->ShortName,
                  UseHTMLEntities ? Txt_only_teachers :
                                    Txt_only_teachers_NO_HTML[Language]);
         break;
      case For_FORUM_CENTRE_USRS:
         strcpy (ForumName,Ctr->ShortName);
         break;
      case For_FORUM_CENTRE_TCHS:
         sprintf (ForumName,"%s%s",Ctr->ShortName,
                  UseHTMLEntities ? Txt_only_teachers :
                                    Txt_only_teachers_NO_HTML[Language]);
         break;
      case For_FORUM_INSTITUTION_USRS:
         strcpy (ForumName,Ins->ShortName);
         break;
      case For_FORUM_INSTITUTION_TCHS:
         sprintf (ForumName,"%s%s",Ins->ShortName,
                  UseHTMLEntities ? Txt_only_teachers :
                                    Txt_only_teachers_NO_HTML[Language]);
         break;
      case For_FORUM_GLOBAL_USRS:
         strcpy (ForumName,UseHTMLEntities ? Txt_General :
                                             Txt_General_NO_HTML[Language]);
         break;
      case For_FORUM_GLOBAL_TCHS:
         sprintf (ForumName,"%s%s",
                  UseHTMLEntities ? Txt_General :
                                    Txt_General_NO_HTML[Language],
                  UseHTMLEntities ? Txt_only_teachers :
                                    Txt_only_teachers_NO_HTML[Language]);
         break;
      case For_FORUM_SWAD_USRS:
         strcpy (ForumName,Cfg_PLATFORM_SHORT_NAME);
         break;
      case For_FORUM_SWAD_TCHS:
         sprintf (ForumName,"%s%s",Cfg_PLATFORM_SHORT_NAME,
                  UseHTMLEntities ? Txt_only_teachers :
                                    Txt_only_teachers_NO_HTML[Language]);
         break;
     }
  }

/*****************************************************************************/
/*********************** Write title and link to a forum *********************/
/*****************************************************************************/

static void For_WriteLinkToForum (For_ForumType_t ForumType,Act_Action_t NextAct,const char *Icon,const char *ForumName,bool ShowNumOfPosts,
                                  unsigned Level,bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS])
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_Copy_not_allowed;
   extern const char *Txt_Paste_thread;
   unsigned NumThrs;
   unsigned NumThrsWithNewPosts;
   unsigned NumPosts;
   const char *Style;
   char ActTxt[Act_MAX_LENGTH_ACTION_TXT+1];

   /***** Get number of threads and number of posts *****/
   NumThrs = For_GetNumThrsInForum (ForumType);
   NumThrsWithNewPosts = For_GetNumThrsWithNewPstsInForum (ForumType,NumThrs);
   Style = (NumThrsWithNewPosts ? The_ClassFormBold[Gbl.Prefs.Theme] :
	                          The_ClassForm[Gbl.Prefs.Theme]);

   /***** Start row *****/
   fprintf (Gbl.F.Out,"<li style=\"height:25px;\">");

   /***** Indent forum title *****/
   Lay_IndentDependingOnLevel (Level,IsLastItemInLevel);

   /***** Write paste button used to move a thread in clipboard to this forum *****/
   if (Gbl.Forum.ThreadToMove >= 0) // If I have permission to paste threads and there is a thread ready to be pasted...
     {
      /* Check if thread to move is yet in current forum */
      if (For_CheckIfThrBelongsToForum (Gbl.Forum.ThreadToMove,ForumType))
         fprintf (Gbl.F.Out,"<img src=\"%s/paste_off16x16.gif\""
                            " alt=\"%s\" title=\"%s\""
                            " class=\"ICON20x20\" />",
                  Gbl.Prefs.IconsURL,
                  Txt_Copy_not_allowed,Txt_Copy_not_allowed);
      else
        {
         Act_FormStart (For_ActionsPasThrFor[ForumType]);
         Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
         For_PutAllHiddenParamsForum ();
         For_PutHiddenParamThrCod (Gbl.Forum.ThreadToMove);
         fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/paste_on16x16.gif\""
                            " alt=\"%s\" title=\"%s\""
                            " class=\"ICON20x20\" />",
                  Gbl.Prefs.IconsURL,
                  Txt_Paste_thread,
                  Txt_Paste_thread);
         Act_FormEnd ();
        }
     }

   /***** Write link to forum *****/
   Act_FormStart (NextAct);
   For_PutAllHiddenParamsForum ();
   Act_LinkFormSubmit (Act_GetActionTextFromDB (Act_Actions[NextAct].ActCod,ActTxt),Style);
   switch (ForumType)
     {
      case For_FORUM_INSTITUTION_USRS:
      case For_FORUM_INSTITUTION_TCHS:
         Log_DrawLogo (Sco_SCOPE_INS,Gbl.Forum.Ins.InsCod,ForumName,20,NULL,true);
         break;
      case For_FORUM_CENTRE_USRS:
      case For_FORUM_CENTRE_TCHS:
         Log_DrawLogo (Sco_SCOPE_CTR,Gbl.Forum.Ctr.CtrCod,ForumName,20,NULL,true);
         break;
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         Log_DrawLogo (Sco_SCOPE_DEG,Gbl.Forum.Deg.DegCod,ForumName,20,NULL,true);
         break;
      default:
         fprintf (Gbl.F.Out,"%s",Icon);
         break;
     }
   fprintf (Gbl.F.Out,"&nbsp;%s",ForumName);

   /***** Write total number of threads and posts in this forum *****/
   if (ShowNumOfPosts)
     {
      if ((NumPosts = For_GetNumPstsInForum (ForumType)))
         For_WriteNumThrsAndPsts (NumThrs,NumThrsWithNewPosts,NumPosts);
     }
   else
      if (NumThrs)
         For_WriteNumberOfThrs (NumThrs,NumThrsWithNewPosts);

   fprintf (Gbl.F.Out,"</a>");
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</li>");
  }

/*****************************************************************************/
/***** Get number of threads with new posts since my last read of a forum ****/
/*****************************************************************************/

unsigned For_GetNumThrsWithNewPstsInForum (For_ForumType_t ForumType,unsigned NumThreads)
  {
   char SubQuery[256];
   char Query[2048];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumThrsWithNewPosts = NumThreads;	// By default, all the threads are new to me

   /***** Get last time I read this forum from database *****/
   switch (ForumType)
     {
      case For_FORUM_INSTITUTION_USRS:
      case For_FORUM_INSTITUTION_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Ins.InsCod);
         break;
      case For_FORUM_CENTRE_USRS:
      case For_FORUM_CENTRE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Ctr.CtrCod);
         break;
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Deg.DegCod);
         break;
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Crs.CrsCod);
         break;
      default:
         SubQuery[0] = '\0';
         break;
     }
   sprintf (Query,"SELECT MAX(forum_thr_read.ReadTime)"
	          " FROM forum_thr_read,forum_thread"
                  " WHERE forum_thr_read.UsrCod='%ld'"
                  " AND forum_thr_read.ThrCod=forum_thread.ThrCod"
                  " AND forum_thread.ForumType='%u'%s",
                  Gbl.Usrs.Me.UsrDat.UsrCod,(unsigned) ForumType,SubQuery);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get the date of reading of a forum");

   if (NumRows)
     {
      /***** Get number of threads with a last message modify time > newest read time (row[0]) *****/
      row = mysql_fetch_row (mysql_res);
      NumThrsWithNewPosts = For_GetNumOfThreadsInForumNewerThan (ForumType,row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumThrsWithNewPosts;
  }

/*****************************************************************************/
/**** Get number of threads in forum with a modify time > a specified time ***/
/*****************************************************************************/

static unsigned For_GetNumOfThreadsInForumNewerThan (For_ForumType_t ForumType,const char *Time)
  {
   char SubQuery[256];
   char Query[2048];

   /***** Get number of threads with a last message modify time
          > specified time from database *****/
   switch (ForumType)
     {
      case For_FORUM_INSTITUTION_USRS:
      case For_FORUM_INSTITUTION_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Ins.InsCod);
         break;
      case For_FORUM_CENTRE_USRS:
      case For_FORUM_CENTRE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Ctr.CtrCod);
         break;
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Deg.DegCod);
         break;
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Crs.CrsCod);
         break;
      default:
         SubQuery[0] = '\0';
         break;
     }
   sprintf (Query,"SELECT COUNT(*) FROM forum_thread,forum_post"
	          " WHERE forum_thread.ForumType='%u'%s"
                  " AND forum_thread.LastPstCod=forum_post.PstCod"
                  " AND forum_post.ModifTime>'%s'",
                  (unsigned) ForumType,SubQuery,Time);
   return (unsigned) DB_QueryCOUNT (Query,"can not check if there are new posts in a forum");
  }

/*****************************************************************************/
/** Get number of unread posts in a thread since my last read of that thread */
/*****************************************************************************/

static unsigned For_GetNumOfUnreadPostsInThr (long ThrCod,unsigned NumPostsInThr)
  {
   char Query[2048];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumUnreadPosts = NumPostsInThr;	// By default, all the posts are unread by me

   /***** Get last time I read this thread from database *****/
   sprintf (Query,"SELECT ReadTime FROM forum_thr_read"
                  " WHERE ThrCod='%ld' AND UsrCod='%ld'",
            ThrCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get the date of reading of a thread");

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
   char Query[2048];

   /***** Get the number of posts in thread with a modify time > a specified time from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM forum_post"
                  " WHERE ThrCod='%ld' AND ModifTime>'%s'",
            ThrCod,Time);
   return (unsigned) DB_QueryCOUNT (Query,"can not check if there are new posts in a thread of a forum");
  }

/*****************************************************************************/
/********************** Show available threads of a forum ********************/
/*****************************************************************************/

void For_ShowForumThrs (void)
  {
   extern const char *Txt_Forum;
   extern const char *Txt_Threads;
   extern const char *Txt_MSG_Subject;
   extern const char *Txt_FORUM_THREAD_HELP_ORDER[2];
   extern const char *Txt_FORUM_THREAD_ORDER[2];
   extern const char *Txt_No_BR_msgs;
   extern const char *Txt_Unread_BR_msgs;
   extern const char *Txt_WriBRters;
   extern const char *Txt_ReaBRders;
   bool IsLastItemInLevel[1+For_FORUM_MAX_LEVELS];
   char SubQuery[256];
   char Query[2048];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumThr,NumThrs;
   unsigned NumThrInScreen;	// From 0 to Pag_ITEMS_PER_PAGE-1
   For_ForumOrderType_t Order;
   long ThrCods[Pag_ITEMS_PER_PAGE];
   struct Pagination PaginationThrs;
   bool ICanMoveThreads = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);	// If I have permission to move threads...

   /***** Get if there is a thread ready to be moved *****/
   if (ICanMoveThreads)
      Gbl.Forum.ThreadToMove = For_GetThrInMyClipboard ();

   /***** Get order type, degree and course of the forum *****/
   For_GetParamsForum ();

   /***** Set forum type *****/
   For_SetForumTypeAndRestrictAccess ();

   /***** Get page number *****/
   Pag_GetParamPagNum (Pag_THREADS_FORUM);

   /***** Get threads of a forum from database *****/
   switch (Gbl.Forum.ForumType)
     {
      case For_FORUM_INSTITUTION_USRS:
      case For_FORUM_INSTITUTION_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Ins.InsCod);
         break;
      case For_FORUM_CENTRE_USRS:
      case For_FORUM_CENTRE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Ctr.CtrCod);
         break;
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Deg.DegCod);
         break;
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",
                  Gbl.Forum.Crs.CrsCod);
         break;
      default:
      	 SubQuery[0] = '\0';
      	 break;
     }
   switch (Gbl.Forum.SelectedOrderType)
     {
      case For_FIRST_MSG:
         sprintf (Query,"SELECT forum_thread.ThrCod FROM forum_thread,forum_post"
                        " WHERE forum_thread.ForumType='%u'%s"
                        " AND forum_thread.FirstPstCod=forum_post.PstCod"
                        " ORDER BY forum_post.CreatTime DESC",
                  (unsigned) Gbl.Forum.ForumType,SubQuery);
         break;
      case For_LAST_MSG:
         sprintf (Query,"SELECT forum_thread.ThrCod FROM forum_thread,forum_post"
                        " WHERE forum_thread.ForumType='%u'%s"
                        " AND forum_thread.LastPstCod=forum_post.PstCod"
                        " ORDER BY forum_post.CreatTime DESC",
                  (unsigned) Gbl.Forum.ForumType,SubQuery);
         break;
     }
   NumThrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get thread of a forum");

   /***** Compute variables related to pagination of threads *****/
   PaginationThrs.NumItems = NumThrs;
   PaginationThrs.CurrentPage = (int) Gbl.Pag.CurrentPage;
   Pag_CalculatePagination (&PaginationThrs);
   Gbl.Pag.CurrentPage = (unsigned) PaginationThrs.CurrentPage;

   /***** Fill the list of threads for current page *****/
   mysql_data_seek (mysql_res,(my_ulonglong) (PaginationThrs.FirstItemVisible-1));
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

   /***** Header whith the name of this forum, the number of threads, and the total number of posts *****/
   /* Table start */
   Lay_StartRoundFrame (NULL,Txt_Forum);

   /* Put a form to select which forums */
   For_PutFormWhichForums ();

   /* Write a link to top level of forums */
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\""
	              " style=\"display:inline-block;\">"
                      "<ul class=\"LIST_LEFT\">");
   For_WriteLinkToTopLevelOfForums ();

   /* Write a link to current forum */
   IsLastItemInLevel[1] = true;
   For_WriteLinkToAForum (Gbl.Forum.ForumType,true,1,IsLastItemInLevel);

   /* End table */
   fprintf (Gbl.F.Out,"</ul>"
	              "</div>");
   Lay_EndRoundFrame ();

   /***** List the threads *****/
   if (NumThrs)
     {
      /***** Write links to all the pages in the listing of threads *****/
      if (PaginationThrs.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (Pag_THREADS_FORUM,0,&PaginationThrs);

      /***** Start table *****/
      Lay_StartRoundFrameTable (NULL,2,Txt_Threads);

      /***** Heading row *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<th class=\"LEFT_MIDDLE\""
	                 " style=\"width:18px;\">"
	                 "</th>"
                         "<th class=\"LEFT_MIDDLE\""
                         " style=\"width:22px;\">"
                         "</th>"
                         "<th class=\"LEFT_MIDDLE\">"
                         "%s"
                         "</th>",
               Txt_MSG_Subject);
      for (Order = For_FIRST_MSG;
	   Order <= For_LAST_MSG;
	   Order++)
	{
	 fprintf (Gbl.F.Out,"<th colspan=\"3\" class=\"CENTER_MIDDLE\">");
         Act_FormStart (For_ActionsSeeFor[Gbl.Forum.ForumType]);
         Pag_PutHiddenParamPagNum (PaginationThrs.CurrentPage);
         For_PutParamWhichForum ();
         For_PutParamsForumInsDegCrs ();
         Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Act_LinkFormSubmit (Txt_FORUM_THREAD_HELP_ORDER[Order],"TIT_TBL");
         if (Order == Gbl.Forum.SelectedOrderType)
            fprintf (Gbl.F.Out,"<u>");
	 fprintf (Gbl.F.Out,"%s",Txt_FORUM_THREAD_ORDER[Order]);
         if (Order == Gbl.Forum.SelectedOrderType)
            fprintf (Gbl.F.Out,"</u>");
         fprintf (Gbl.F.Out,"</a>");
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</th>");
	}
      fprintf (Gbl.F.Out,"<th class=\"RIGHT_MIDDLE\">"
	                 "%s"
	                 "</th>"
                         "<th class=\"RIGHT_MIDDLE\">"
                         "%s"
                         "</th>"
                         "<th class=\"RIGHT_MIDDLE\">"
                         "%s"
                         "</th>"
                         "<th class=\"RIGHT_MIDDLE\">"
                         "%s"
                         "</th>"
                         "</tr>",
               Txt_No_BR_msgs,
               Txt_Unread_BR_msgs,
               Txt_WriBRters,
               Txt_ReaBRders);

      /***** List the threads *****/
      For_ListForumThrs (ThrCods,&PaginationThrs);

      /***** End table *****/
      Lay_EndRoundFrameTable ();

      /***** Write links to all the pages in the listing of threads *****/
      if (PaginationThrs.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (Pag_THREADS_FORUM,0,&PaginationThrs);
     }

   /***** Put a form to write the first message of a new thread *****/
   For_WriteFormForumPst (false,-1,NULL);
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
/*************** Get and write total number of threads and posts *************/
/*****************************************************************************/

static void For_WriteNumThrsAndPsts (unsigned NumThrs,unsigned NumThrsWithNewPosts,unsigned NumPosts)
  {
   extern const char *Txt_thread;
   extern const char *Txt_threads;
   extern const char *Txt_post;
   extern const char *Txt_posts;
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
         fprintf (Gbl.F.Out,"1 %s",Txt_post);
      else
         fprintf (Gbl.F.Out,"%u %s",NumPosts,Txt_posts);
     }
   else
     {
      fprintf (Gbl.F.Out,"%u %s",NumThrs,Txt_threads);
      if (NumThrsWithNewPosts)
         fprintf (Gbl.F.Out,", %u %s",NumThrsWithNewPosts,Txt_with_new_posts);
      fprintf (Gbl.F.Out,"; %u %s",NumPosts,Txt_posts);
     }
   fprintf (Gbl.F.Out,"]");
  }

/*****************************************************************************/
/********************** Get number of forums of a type ***********************/
/*****************************************************************************/

unsigned For_GetNumTotalForumsOfType (For_ForumType_t ForumType,
                                      long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumForums;

   /***** Get number of forums of a type from database *****/
   switch (ForumType)
     {
      case For_FORUM_SWAD_USRS:		case For_FORUM_SWAD_TCHS:
      case For_FORUM_GLOBAL_USRS:	case For_FORUM_GLOBAL_TCHS:
         return 1;	// Only one forum
      case For_FORUM_INSTITUTION_USRS:	case For_FORUM_INSTITUTION_TCHS:
         if (InsCod > 0)	// InsCod > 0 ==> 0 <= number of institutions forums for an institution <= 1
            sprintf (Query,"SELECT COUNT(DISTINCT Location)"
        	           " FROM forum_thread"
        	           " WHERE ForumType='%u'"
        	           " AND Location='%ld'",
                     (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of institution forums for a country
	    sprintf (Query,"SELECT COUNT(DISTINCT forum_thread.Location)"
			   " FROM forum_thread,institutions"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=institutions.InsCod"
			   " AND institutions.CtyCod='%ld'",
		     (unsigned) ForumType,CtyCod);
         else			// CtyCod <= 0 ==> Number of institutions forums for the whole platform
            sprintf (Query,"SELECT COUNT(DISTINCT Location)"
        	           " FROM forum_thread"
        	           " WHERE ForumType='%u'",
                     (unsigned) ForumType);
	 break;
      case For_FORUM_CENTRE_USRS:	case For_FORUM_CENTRE_TCHS:
         if (CtrCod > 0)	// CtrCod > 0 ==> 0 <= number of centre forums for a centre <= 1
            sprintf (Query,"SELECT COUNT(DISTINCT Location)"
        	           " FROM forum_thread"
        	           " WHERE ForumType='%u'"
        	           " AND Location='%ld'",
                     (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of centre forums for an institution
	    sprintf (Query,"SELECT COUNT(DISTINCT forum_thread.Location)"
			   " FROM forum_thread,centres"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=centres.CtrCod"
			   " AND centres.InsCod='%ld'",
		     (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of centre forums for a country
	    sprintf (Query,"SELECT COUNT(DISTINCT forum_thread.Location)"
			   " FROM forum_thread,centres,institutions"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=centres.CtrCod"
	                   " AND centres.InsCod=institutions.InsCod"
			   " AND institutions.CtyCod='%ld'",
		     (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of centre forums for the whole platform
            sprintf (Query,"SELECT COUNT(DISTINCT Location)"
        	           " FROM forum_thread"
        	           " WHERE ForumType='%u'",
                     (unsigned) ForumType);
	 break;
      case For_FORUM_DEGREE_USRS:	case For_FORUM_DEGREE_TCHS:
         if (DegCod > 0)	// DegCod > 0 ==> 0 <= number of degree forums for a degree <= 1
            sprintf (Query,"SELECT COUNT(DISTINCT Location)"
        	           " FROM forum_thread"
        	           " WHERE ForumType='%u'"
        	           " AND Location='%ld'",
                     (unsigned) ForumType,DegCod);
         else if (CtrCod > 0)	// DegCod <= 0 && CtrCod > 0 ==> Number of degree forums for a centre
	    sprintf (Query,"SELECT COUNT(DISTINCT forum_thread.Location)"
			   " FROM forum_thread,degrees"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=degrees.DegCod"
			   " AND degrees.CtrCod='%ld'",
		     (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of degree forums for an institution
	    sprintf (Query,"SELECT COUNT(DISTINCT forum_thread.Location)"
			   " FROM forum_thread,degrees,centres"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=degrees.DegCod"
			   " AND degrees.CtrCod=centres.CtrCod"
			   " AND centres.InsCod='%ld'",
		     (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of degree forums for a country
	    sprintf (Query,"SELECT COUNT(DISTINCT forum_thread.Location)"
			   " FROM forum_thread,degrees,centres,institutions"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=degrees.DegCod"
	                   " AND degrees.CtrCod=centres.CtrCod"
	                   " AND centres.InsCod=institutions.InsCod"
			   " AND institutions.CtyCod='%ld'",
		     (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of degree forums for the whole platform
	    sprintf (Query,"SELECT COUNT(DISTINCT Location)"
			   " FROM forum_thread"
			   " WHERE ForumType='%u'",
		     (unsigned) ForumType);
         break;
      case For_FORUM_COURSE_USRS:	case For_FORUM_COURSE_TCHS:
         if (CrsCod > 0)	// CrsCod > 0 ==> 0 <= number of course forums for a course <= 1
           sprintf (Query,"SELECT COUNT(DISTINCT Location)"
        	          " FROM forum_thread"
        	          " WHERE ForumType='%u'"
        	          " AND Location='%ld'",
                     (unsigned) ForumType,CrsCod);
         else if (DegCod > 0)	// CrsCod <= 0 && DegCod > 0 ==> Number of course forums for a degree
	    sprintf (Query,"SELECT COUNT(DISTINCT forum_thread.Location)"
			   " FROM forum_thread,courses"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=courses.CrsCod"
			   " AND courses.DegCod='%ld'",
		     (unsigned) ForumType,DegCod);
         else if (CtrCod > 0)	// DegCod <= 0 && CtrCod > 0 ==> Number of course forums for a centre
	    sprintf (Query,"SELECT COUNT(DISTINCT forum_thread.Location)"
			   " FROM forum_thread,courses,degrees"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=courses.CrsCod"
			   " AND courses.DegCod=degrees.DegCod"
			   " AND degrees.CtrCod='%ld'",
		     (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of course forums for an institution
	    sprintf (Query,"SELECT COUNT(DISTINCT forum_thread.Location)"
			   " FROM forum_thread,courses,degrees,centres"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=courses.CrsCod"
			   " AND courses.DegCod=degrees.DegCod"
			   " AND degrees.CtrCod=centres.CtrCod"
			   " AND centres.InsCod='%ld'",
		     (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of course forums for a country
	    sprintf (Query,"SELECT COUNT(DISTINCT forum_thread.Location)"
			   " FROM forum_thread,courses,degrees,centres,institutions"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=courses.CrsCod"
			   " AND courses.DegCod=degrees.DegCod"
	                   " AND degrees.CtrCod=centres.CtrCod"
	                   " AND centres.InsCod=institutions.InsCod"
			   " AND institutions.CtyCod='%ld'",
		     (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of course forums for the whole platform
	    sprintf (Query,"SELECT COUNT(DISTINCT Location)"
			   " FROM forum_thread"
			   " WHERE ForumType='%u'",
		     (unsigned) ForumType);
         break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of forums of a type");

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
   char Query[512];

   /***** Get total number of threads in forums of this type from database *****/
   switch (ForumType)
     {
      case For_FORUM_SWAD_USRS:		case For_FORUM_SWAD_TCHS:
      case For_FORUM_GLOBAL_USRS:	case For_FORUM_GLOBAL_TCHS:
         // Total number of threads in forums of this type
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM forum_thread"
                        " WHERE ForumType='%u'",
                  (unsigned) ForumType);
         break;
      case For_FORUM_INSTITUTION_USRS:	case For_FORUM_INSTITUTION_TCHS:
         if (InsCod > 0)	// InsCod > 0 ==> Number of threads in institution forums for an institution
            sprintf (Query,"SELECT COUNT(*)"
        	           " FROM forum_thread"
        	           " WHERE ForumType='%u'"
        	           " AND Location='%ld'",
                     (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of threads in institution forums for a country
	    sprintf (Query,"SELECT COUNT(*)"
			   " FROM forum_thread,institutions"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=institutions.InsCod"
			   " AND institutions.CtyCod='%ld'",
		     (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of threads in institution forums for the whole platform
            sprintf (Query,"SELECT COUNT(*)"
        	           " FROM forum_thread"
        	           " WHERE ForumType='%u'",
                     (unsigned) ForumType);
	 break;
      case For_FORUM_CENTRE_USRS:	case For_FORUM_CENTRE_TCHS:
         if (CtrCod > 0)	// CtrCod > 0 ==> 0 <= Number of threads in centre forums for a centre <= 1
            sprintf (Query,"SELECT COUNT(*)"
        	           " FROM forum_thread"
        	           " WHERE ForumType='%u'"
        	           " AND Location='%ld'",
                     (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of threads in centre forums for an institution
            sprintf (Query,"SELECT COUNT(*)"
        	           " FROM forum_thread,centres"
        	           " WHERE forum_thread.ForumType='%u'"
        	           " AND forum_thread.Location=centres.CtrCod"
        	           " AND centres.InsCod='%ld'",
                     (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of threads in centre forums for a country
	    sprintf (Query,"SELECT COUNT(*)"
			   " FROM forum_thread,centres,institutions"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=centres.CtrCod"
	                   " AND centres.InsCod=institutions.InsCod"
			   " AND institutions.CtyCod='%ld'",
		     (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of threads in centre forums for the whole platform
            sprintf (Query,"SELECT COUNT(*)"
        	           " FROM forum_thread"
        	           " WHERE ForumType='%u'",
                     (unsigned) ForumType);
	 break;
      case For_FORUM_DEGREE_USRS:	case For_FORUM_DEGREE_TCHS:
         if (DegCod > 0)	// DegCod > 0 ==> Number of threads in degree forums for a degree
            sprintf (Query,"SELECT COUNT(*)"
        	           " FROM forum_thread"
        	           " WHERE ForumType='%u'"
        	           " AND Location='%ld'",
                     (unsigned) ForumType,DegCod);
         else if (CtrCod > 0)	// DegCod <= 0 && CtrCod > 0 ==> Number of threads in degree forums for a centre
            sprintf (Query,"SELECT COUNT(*)"
        	           " FROM forum_thread,degrees"
        	           " WHERE forum_thread.ForumType='%u'"
        	           " AND forum_thread.Location=degrees.DegCod"
        	           " AND degrees.CtrCod='%ld'",
                     (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of threads in degree forums for an institution
            sprintf (Query,"SELECT COUNT(*)"
        	           " FROM forum_thread,degrees,centres"
        	           " WHERE forum_thread.ForumType='%u'"
        	           " AND forum_thread.Location=degrees.DegCod"
        	           " AND degrees.CtrCod=centres.CtrCod"
        	           " AND centres.InsCod='%ld'",
                     (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of threads in degree forums for a country
	    sprintf (Query,"SELECT COUNT(*)"
			   " FROM forum_thread,degrees,centres,institutions"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=degrees.DegCod"
	                   " AND degrees.CtrCod=centres.CtrCod"
	                   " AND centres.InsCod=institutions.InsCod"
			   " AND institutions.CtyCod='%ld'",
		     (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of threads in degree forums for the whole platform
            sprintf (Query,"SELECT COUNT(*)"
        	           " FROM forum_thread"
        	           " WHERE ForumType='%u'",
                     (unsigned) ForumType);
         break;
      case For_FORUM_COURSE_USRS:	case For_FORUM_COURSE_TCHS:
         if (CrsCod > 0)	// CrsCod > 0 ==> 0 <= Number of threads in course forums for a course
            sprintf (Query,"SELECT COUNT(*)"
        	           " FROM forum_thread"
        	           " WHERE ForumType='%u'"
        	           " AND Location='%ld'",
                     (unsigned) ForumType,CrsCod);
         else if (DegCod > 0)	// CrsCod <= 0 && DegCod > 0 ==> Number of threads in course forums for a degree
	    sprintf (Query,"SELECT COUNT(*)"
			   " FROM forum_thread,courses"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=courses.CrsCod"
			   " AND courses.DegCod='%ld'",
		     (unsigned) ForumType,DegCod);
         else if (CtrCod > 0)	// DegCod <= 0 && CtrCod > 0 ==> Number of threads in course forums for a centre
	    sprintf (Query,"SELECT COUNT(*)"
			   " FROM forum_thread,courses,degrees"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=courses.CrsCod"
			   " AND courses.DegCod=degrees.DegCod"
			   " AND degrees.CtrCod='%ld'",
		     (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of threads in course forums for an institution
            sprintf (Query,"SELECT COUNT(*)"
        	           " FROM forum_thread,courses,degrees,centres"
        	           " WHERE forum_thread.ForumType='%u'"
        	           " AND forum_thread.Location=courses.CrsCod"
        	           " AND courses.DegCod=degrees.DegCod"
        	           " AND degrees.CtrCod=centres.CtrCod"
        	           " AND centres.InsCod='%ld'",
                     (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of threads in course forums for a country
	    sprintf (Query,"SELECT COUNT(*)"
			   " FROM forum_thread,courses,degrees,centres,institutions"
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=courses.CrsCod"
			   " AND courses.DegCod=degrees.DegCod"
	                   " AND degrees.CtrCod=centres.CtrCod"
	                   " AND centres.InsCod=institutions.InsCod"
			   " AND institutions.CtyCod='%ld'",
		     (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of threads in course forums for the whole platform
	    sprintf (Query,"SELECT COUNT(*)"
			   " FROM forum_thread"
			   " WHERE ForumType='%u'",
		     (unsigned) ForumType);
         break;
     }
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of threads in forums of a type");
  }

/*****************************************************************************/
/******************* Get number of threads in a forum ************************/
/*****************************************************************************/

unsigned For_GetNumThrsInForum (For_ForumType_t ForumType)
  {
   char SubQuery[256];
   char Query[1024];

   /***** Get number of threads in a forum from database *****/
   switch (ForumType)
     {
      case For_FORUM_INSTITUTION_USRS:	case For_FORUM_INSTITUTION_TCHS:
         sprintf (SubQuery," AND Location='%ld'",Gbl.Forum.Ins.InsCod);
         break;
      case For_FORUM_CENTRE_USRS:	case For_FORUM_CENTRE_TCHS:
         sprintf (SubQuery," AND Location='%ld'",Gbl.Forum.Ctr.CtrCod);
         break;
      case For_FORUM_DEGREE_USRS:	case For_FORUM_DEGREE_TCHS:
         sprintf (SubQuery," AND Location='%ld'",Gbl.Forum.Deg.DegCod);
         break;
      case For_FORUM_COURSE_USRS:	case For_FORUM_COURSE_TCHS:
         sprintf (SubQuery," AND Location='%ld'",Gbl.Forum.Crs.CrsCod);
         break;
      default:
         SubQuery[0] = '\0';
         break;
     }
   sprintf (Query,"SELECT COUNT(*) FROM forum_thread WHERE ForumType='%u'%s",
            (unsigned) ForumType,SubQuery);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of threads in a forum");
  }

/*****************************************************************************/
/************** Get total number of posts in forums of a type ****************/
/*****************************************************************************/

unsigned For_GetNumTotalPstsInForumsOfType (For_ForumType_t ForumType,
                                            long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod,
                                            unsigned *NumUsrsToBeNotifiedByEMail)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumPosts;

   /***** Get total number of posts in forums of this type from database *****/
   switch (ForumType)
     {
      case For_FORUM_SWAD_USRS:		case For_FORUM_SWAD_TCHS:
      case For_FORUM_GLOBAL_USRS:	case For_FORUM_GLOBAL_TCHS:
         // Total number of posts in forums of this type
         sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
                        " FROM forum_thread,forum_post "
                        " WHERE forum_thread.ForumType='%u'"
                        " AND forum_thread.ThrCod=forum_post.ThrCod",
                  (unsigned) ForumType);
         break;
      case For_FORUM_INSTITUTION_USRS:	case For_FORUM_INSTITUTION_TCHS:
         if (InsCod > 0)	// InsCod > 0 ==> Number of posts in institutions forums for an institution
            sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
        	           " FROM forum_thread,forum_post"
        	           " WHERE forum_thread.ForumType='%u'"
        	           " AND forum_thread.Location='%ld'"
        	           " AND forum_thread.ThrCod=forum_post.ThrCod",
                     (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of posts in institutions forums for a country
            sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
        	           " FROM forum_thread,institutions,forum_post"
        	           " WHERE forum_thread.ForumType='%u'"
        	           " AND forum_thread.Location=institutions.InsCod"
                           " AND institutions.CtyCod='%ld'"
        	           " AND forum_thread.ThrCod=forum_post.ThrCod",
                     (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of posts in institution forums for the whole platform
	    sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
			   " FROM forum_thread,forum_post "
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.ThrCod=forum_post.ThrCod",
		     (unsigned) ForumType);
         break;
      case For_FORUM_CENTRE_USRS:	case For_FORUM_CENTRE_TCHS:
         if (CtrCod > 0)	// CtrCod > 0 ==> Number of posts in centre forums for a centre
            sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
        	           " FROM forum_thread,forum_post"
        	           " WHERE forum_thread.ForumType='%u'"
        	           " AND forum_thread.Location='%ld'"
        	           " AND forum_thread.ThrCod=forum_post.ThrCod",
                     (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of posts in centre forums for an institution
            sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
        	           " FROM forum_thread,centres,forum_post"
        	           " WHERE forum_thread.ForumType='%u'"
        	           " AND forum_thread.Location=centres.CtrCod"
        	           " AND centres.InsCod='%ld'"
        	           " AND forum_thread.ThrCod=forum_post.ThrCod",
                     (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of posts in centre forums for a country
            sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
        	           " FROM forum_thread,centres,institutions,forum_post"
        	           " WHERE forum_thread.ForumType='%u'"
        	           " AND forum_thread.Location=centres.CtrCod"
                           " AND centres.InsCod=institutions.InsCod"
        	           " AND institutions.CtyCod='%ld'"
        	           " AND forum_thread.ThrCod=forum_post.ThrCod",
                     (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of posts in centre forums for the whole platform
	    sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
			   " FROM forum_thread,forum_post "
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.ThrCod=forum_post.ThrCod",
		     (unsigned) ForumType);
         break;
      case For_FORUM_DEGREE_USRS:	case For_FORUM_DEGREE_TCHS:
         if (DegCod > 0)	// DegCod > 0 ==> Number of posts in degree forums for a degree
            sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
        	           " FROM forum_thread,forum_post "
                           " WHERE forum_thread.ForumType='%u'"
                           " AND forum_thread.Location='%ld'"
                           " AND forum_thread.ThrCod=forum_post.ThrCod",
                     (unsigned) ForumType,DegCod);
         else if (CtrCod > 0)	// DegCod <= 0 && CtrCod > 0 ==> Number of posts in degree forums for a centre
            sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
        	           " FROM forum_thread,degrees,forum_post "
                           " WHERE forum_thread.ForumType='%u'"
                           " AND forum_thread.Location=degrees.DegCod"
                           " AND degrees.CtrCod='%ld'"
                           " AND forum_thread.ThrCod=forum_post.ThrCod",
                     (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of posts in degree forums for an institution
            sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
        	           " FROM forum_thread,degrees,centres,forum_post"
        	           " WHERE forum_thread.ForumType='%u'"
        	           " AND forum_thread.Location=degrees.DegCod"
        	           " AND degrees.CtrCod=centres.CtrCod"
        	           " AND centres.InsCod='%ld'"
        	           " AND forum_thread.ThrCod=forum_post.ThrCod",
                     (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of posts in degree forums for a country
            sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
        	           " FROM forum_thread,degrees,centres,institutions,forum_post"
        	           " WHERE forum_thread.ForumType='%u'"
        	           " AND forum_thread.Location=degrees.DegCod"
        	           " AND degrees.CtrCod=centres.CtrCod"
                           " AND centres.InsCod=institutions.InsCod"
        	           " AND institutions.CtyCod='%ld'"
        	           " AND forum_thread.ThrCod=forum_post.ThrCod",
                     (unsigned) ForumType,CtyCod);
         else			// InsCod <= 0 ==> Number of posts in degree forums for the whole platform
            sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
        	           " FROM forum_thread,forum_post "
                           " WHERE forum_thread.ForumType='%u'"
                           " AND forum_thread.ThrCod=forum_post.ThrCod",
                     (unsigned) ForumType);
         break;
      case For_FORUM_COURSE_USRS:	case For_FORUM_COURSE_TCHS:
         if (CrsCod > 0)	// CrsCod > 0 ==> 0 <= number of posts in course forums for a course
            sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
        	           " FROM forum_thread,forum_post "
                           " WHERE forum_thread.ForumType='%u'"
                           " AND forum_thread.Location='%ld'"
                           " AND forum_thread.ThrCod=forum_post.ThrCod",
                     (unsigned) ForumType,CrsCod);
         else if (DegCod > 0)	// CrsCod <= 0 && DegCod > 0 ==> Number of posts in course forums for a degree
	    sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
		           " FROM forum_thread,courses,forum_post "
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=courses.CrsCod"
			   " AND courses.DegCod='%ld'"
			   " AND forum_thread.ThrCod=forum_post.ThrCod",
		     (unsigned) ForumType,DegCod);
         else if (CtrCod > 0)	// DegCod <= 0 && CtrCod > 0 ==> Number of posts in course forums for a centre
	    sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
		           " FROM forum_thread,courses,degrees,forum_post "
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.Location=courses.CrsCod"
			   " AND courses.DegCod=degrees.DegCod"
			   " AND degrees.CtrCod='%ld'"
			   " AND forum_thread.ThrCod=forum_post.ThrCod",
		     (unsigned) ForumType,CtrCod);
         else if (InsCod > 0)	// CtrCod <= 0 && InsCod > 0 ==> Number of posts in course forums for an institution
            sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
        	           " FROM forum_thread,courses,degrees,centres,forum_post"
        	           " WHERE forum_thread.ForumType='%u'"
        	           " AND forum_thread.Location=courses.CrsCod"
        	           " AND courses.DegCod=degrees.DegCod"
        	           " AND degrees.CtrCod=centres.CtrCod"
        	           " AND centres.InsCod='%ld'"
        	           " AND forum_thread.ThrCod=forum_post.ThrCod",
                     (unsigned) ForumType,InsCod);
         else if (CtyCod > 0)	// InsCod <= 0 && CtyCod > 0 ==> Number of posts in course forums for a country
            sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
        	           " FROM forum_thread,courses,degrees,centres,institutions,forum_post"
        	           " WHERE forum_thread.ForumType='%u'"
        	           " AND forum_thread.Location=courses.CrsCod"
        	           " AND courses.DegCod=degrees.DegCod"
        	           " AND degrees.CtrCod=centres.CtrCod"
                           " AND centres.InsCod=institutions.InsCod"
        	           " AND institutions.CtyCod='%ld'"
        	           " AND forum_thread.ThrCod=forum_post.ThrCod",
                     (unsigned) ForumType,CtyCod);
         else			// CrsCod <= 0 && DegCod <= 0 && CtrCod <= 0 ==> Number of posts in course forums for the whole platform
	    sprintf (Query,"SELECT COUNT(*),SUM(forum_post.NumNotif)"
		           " FROM forum_thread,forum_post "
			   " WHERE forum_thread.ForumType='%u'"
			   " AND forum_thread.ThrCod=forum_post.ThrCod",
		     (unsigned) ForumType);
         break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get the total number of forums of a type");

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

unsigned For_GetNumPstsInForum (For_ForumType_t ForumType)
  {
   char SubQuery[256];
   char Query[1024];

   /***** Get number of posts in a forum from database *****/
   switch (ForumType)
     {
      case For_FORUM_INSTITUTION_USRS:	case For_FORUM_INSTITUTION_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",Gbl.Forum.Ins.InsCod);
         break;
      case For_FORUM_CENTRE_USRS:	case For_FORUM_CENTRE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",Gbl.Forum.Ins.InsCod);
         break;
      case For_FORUM_DEGREE_USRS:	case For_FORUM_DEGREE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",Gbl.Forum.Deg.DegCod);
         break;
      case For_FORUM_COURSE_USRS:	case For_FORUM_COURSE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",Gbl.Forum.Crs.CrsCod);
         break;
      default:
         SubQuery[0] = '\0';
         break;
     }
   sprintf (Query,"SELECT COUNT(*) FROM forum_thread,forum_post "
                  " WHERE forum_thread.ForumType='%u'%s AND forum_thread.ThrCod=forum_post.ThrCod",
            (unsigned) ForumType,SubQuery);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of posts in a forum");
  }

/*****************************************************************************/
/************************ List the threads of a forum ************************/
/*****************************************************************************/

void For_ListForumThrs (long ThrCods[Pag_ITEMS_PER_PAGE],struct Pagination *PaginationThrs)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_You_have_written_1_post_in_this_thread;
   extern const char *Txt_You_have_written_X_posts_in_this_thread;
   extern const char *Txt_Thread_with_posts_from_you;
   extern const char *Txt_There_are_new_posts;
   extern const char *Txt_No_new_posts;
   extern const char *Txt_Move_thread;
   extern const char *Txt_Today;
   unsigned NumThr;
   unsigned NumThrInScreen;	// From 0 to Pag_ITEMS_PER_PAGE-1
   unsigned UniqueId;
   struct ForumThread Thr;
   struct UsrData UsrDat;
   For_ForumOrderType_t Order;
   time_t TimeUTC;
   struct Pagination PaginationPsts;
   const char *Style;
   bool ICanMoveThreads = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);	// If I have permission to move threads...
   long ThreadInMyClipboard = -1L;
   unsigned Column;
   const char *BgColor;

   /***** Get if there is a thread ready to be moved *****/
   if (ICanMoveThreads)
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
      Style = (Thr.NumUnreadPosts ? "MSG_AUT_NEW" :
	                            "MSG_AUT");
      BgColor = (Thr.ThrCod == ThreadInMyClipboard) ? "LIGHT_GREEN" :
                                                      Gbl.ColorRows[Gbl.RowEvenOdd];

      /***** Show my photo if I have any posts in this thread *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"RIGHT_TOP %s\" style=\"width:18px;\">",
               BgColor);
      if (Thr.NumMyPosts)
        {
         fprintf (Gbl.F.Out,"<img src=\"");
         if (Gbl.Usrs.Me.PhotoURL[0])	// If I have photo
            fprintf (Gbl.F.Out,"%s",
                     Gbl.Usrs.Me.PhotoURL);
         else
            fprintf (Gbl.F.Out,"%s/usr_bl.jpg",
                     Gbl.Prefs.IconsURL);
         fprintf (Gbl.F.Out,"\" alt=\"%s\" title=\"",
                  Txt_Thread_with_posts_from_you);
         if (Thr.NumMyPosts == 1)
            fprintf (Gbl.F.Out,"%s",Txt_You_have_written_1_post_in_this_thread);
         else
            fprintf (Gbl.F.Out,Txt_You_have_written_X_posts_in_this_thread,
                     Thr.NumMyPosts);
         fprintf (Gbl.F.Out,"\" class=\"PHOTO15x20\" />");
        }
      fprintf (Gbl.F.Out,"</td>");

      /***** Put an icon with thread status *****/
      fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP %s\" style=\"width:30px;\">"
                         "<img src=\"%s/%s16x16.gif\""
                         " alt=\"%s\" title=\"%s\""
	                 " class=\"ICON20x20\" />",
               BgColor,
	       Gbl.Prefs.IconsURL,
               Thr.NumUnreadPosts ? "msg-unread" :
        	                    "msg-open",
               Thr.NumUnreadPosts ? Txt_There_are_new_posts :
                                    Txt_No_new_posts,
               Thr.NumUnreadPosts ? Txt_There_are_new_posts :
                                    Txt_No_new_posts);
      if (PermissionThreadDeletion[Gbl.Forum.ForumType] &
	  (1 << Gbl.Usrs.Me.LoggedRole)) // If I have permission to remove thread in this forum...
        {
         /* Put button to remove the thread */
         fprintf (Gbl.F.Out,"<br />");
         Act_FormStart (For_ActionsReqDelThr[Gbl.Forum.ForumType]);
         For_PutAllHiddenParamsForum ();
         For_PutHiddenParamThrCod (Thr.ThrCod);
         Lay_PutIconRemove ();
         Act_FormEnd ();
        }

      if (ICanMoveThreads)
        {
         /* Put button to cut the thread for moving it to another forum */
         fprintf (Gbl.F.Out,"<br />");
         Act_FormStart (For_ActionsCutThrFor[Gbl.Forum.ForumType]);
         Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
         For_PutAllHiddenParamsForum ();
         For_PutHiddenParamThrCod (Thr.ThrCod);
         fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/cut16x16.gif\""
                            " alt=\"%s\" title=\"%s\""
                            " class=\"ICON20x20\" />",
                  Gbl.Prefs.IconsURL,
                  Txt_Move_thread,
                  Txt_Move_thread);
         Act_FormEnd ();
        }

      fprintf (Gbl.F.Out,"</td>");

      /***** Write subject and links to thread pages *****/
      fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP %s\">",BgColor);
      PaginationPsts.NumItems = Thr.NumPosts;
      PaginationPsts.CurrentPage = 1;	// First page
      Pag_CalculatePagination (&PaginationPsts);
      Str_LimitLengthHTMLStr (Thr.Subject,80);
      Pag_WriteLinksToPages (Pag_POSTS_FORUM,Thr.ThrCod,&PaginationPsts,
                             Thr.Enabled[For_FIRST_MSG],
                             Thr.Subject,
                             Thr.NumUnreadPosts ? The_ClassFormBold[Gbl.Prefs.Theme] :
                        	                  The_ClassForm[Gbl.Prefs.Theme],
                             true);
      fprintf (Gbl.F.Out,"</td>");

      /***** Write the authors and date-times of first and last posts *****/
      for (Order = For_FIRST_MSG;
	   Order <= For_LAST_MSG;
	   Order++)
        {
         if (Order == For_FIRST_MSG || Thr.NumPosts > 1)	// Don't write twice the same author when thread has only one thread
           {
            /* Write the author of first or last message */
            UsrDat.UsrCod = Thr.UsrCod[Order];
            Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);
            Msg_WriteMsgAuthor (&UsrDat,68,9,Style,Thr.Enabled[Order],BgColor);

            /* Write the date of first or last message (it's in YYYYMMDDHHMMSS format) */
            TimeUTC = Thr.WriteTime[Order];
	    UniqueId++;
            fprintf (Gbl.F.Out,"<td id=\"date_%u\" class=\"%s LEFT_TOP %s\">"
                               "<script type=\"text/javascript\">"
			       "writeLocalDateHMSFromUTC('date_%u',%ld,'<br />','%s');"
			       "</script>"
			       "</td>",
                     UniqueId,Style,BgColor,
		     UniqueId,(long) TimeUTC,Txt_Today);
           }
         else
            for (Column = 1;
        	 Column <= 3;
        	 Column++)
               fprintf (Gbl.F.Out,"<td class=\"%s LEFT_TOP %s\">"
        	                  "</td>",
                        Style,BgColor);
        }

      /***** Write number of posts in this thread *****/
      fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_TOP %s\">"
	                 "%u&nbsp;"
	                 "</td>",
               Style,BgColor,
               Thr.NumPosts);

      /***** Write number of new posts in this thread *****/
      fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_TOP %s\">"
	                 "%u&nbsp;"
	                 "</td>",
               Style,BgColor,
               Thr.NumUnreadPosts);

      /***** Write number of users who have write posts in this thread *****/
      fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_TOP %s\">"
	                 "%u&nbsp;"
	                 "</td>",
               Style,BgColor,
               Thr.NumWriters);

      /***** Write number of users who have read this thread *****/
      fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_TOP %s\">"
	                 "%u&nbsp;"
	                 "</td>"
	                 "</tr>",
               Style,BgColor,
               Thr.NumReaders);
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/***************************** Get data of a thread **************************/
/*****************************************************************************/

void For_GetThrData (struct ForumThread *Thr)
  {
   extern const char *Txt_no_subject;
   char Query[2048];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   For_ForumOrderType_t Order;

   /***** Get data of a thread from database *****/
   sprintf (Query,"SELECT m0.PstCod,m1.PstCod,m0.UsrCod,m1.UsrCod,"
                  "UNIX_TIMESTAMP(m0.CreatTime),"
                  "UNIX_TIMESTAMP(m1.CreatTime),"
                  "m0.Subject"
                  " FROM forum_thread,forum_post AS m0,forum_post AS m1"
                  " WHERE forum_thread.ThrCod='%ld'"
                  " AND forum_thread.FirstPstCod=m0.PstCod"
                  " AND forum_thread.LastPstCod=m1.PstCod",
            Thr->ThrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get data of a thread of a forum");

   /***** The result of the query should have one row *****/
   if (NumRows != 1)
      Lay_ShowErrorAndExit ("Error when getting data of a thread of a forum.");
   row = mysql_fetch_row (mysql_res);

   /***** Get the code of the first message in this thread (row[0]) *****/
   if (sscanf (row[0],"%ld",&(Thr->PstCod[For_FIRST_MSG])) != 1)
      Lay_ShowErrorAndExit ("Wrong code of post.");

   /***** Get the code of the last message in this thread (row[1]) *****/
   if (sscanf (row[1],"%ld",&(Thr->PstCod[For_LAST_MSG])) != 1)
      Lay_ShowErrorAndExit ("Wrong code of post.");

   /***** Get the author of the first message in this thread (row[2]) *****/
   Thr->UsrCod[For_FIRST_MSG] = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get the author of the last message in this thread (row[3]) *****/
   Thr->UsrCod[For_LAST_MSG] = Str_ConvertStrCodToLongCod (row[3]);

   /***** Get the date of the first message in this thread (row[4]) *****/
   Thr->WriteTime[For_FIRST_MSG] = Dat_GetUNIXTimeFromStr (row[4]);

   /***** Get the date of the last  message in this thread (row[5]) *****/
   Thr->WriteTime[For_LAST_MSG ] = Dat_GetUNIXTimeFromStr (row[5]);

   /***** Get the subject of this thread (row[6]) *****/
   strncpy (Thr->Subject,row[6],Cns_MAX_BYTES_SUBJECT);
   Thr->Subject[Cns_MAX_BYTES_SUBJECT] = '\0';
   if (!Thr->Subject[0])
      sprintf (Thr->Subject,"[%s]",Txt_no_subject);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Get if first or last message are enabled *****/
   for (Order = For_FIRST_MSG;
	Order <= For_LAST_MSG;
	Order++)
      Thr->Enabled[Order] = For_GetIfPstIsEnabled (Thr->PstCod[Order]);

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
/************* Write the subject of the first message of a thread ************/
/*****************************************************************************/

static void For_WriteThrSubject (long ThrCod)
  {
   extern const char *Txt_no_subject;
   extern const char *Txt_first_message_not_allowed;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   long FirstPstCod;

   /***** Get subject of a thread from database *****/
   sprintf (Query,"SELECT forum_post.PstCod,forum_post.Subject FROM forum_thread,forum_post"
                  " WHERE forum_thread.ThrCod='%ld' AND forum_thread.FirstPstCod=forum_post.PstCod",
            ThrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get the subject of a thread");

   /***** The result of the query should have only one row *****/
   if (NumRows != 1)
      Lay_ShowErrorAndExit ("Error when getting the subject of a thread.");
   row = mysql_fetch_row (mysql_res);

   /***** Get message code *****/
   if (sscanf (row[0],"%ld",&FirstPstCod) != 1)
      Lay_ShowErrorAndExit ("Error when getting the subject of a thread.");

   /***** Write the subject of the thread *****/
   if (For_GetIfPstIsEnabled (FirstPstCod))
     {
      if (row[1][0])
         fprintf (Gbl.F.Out,"%s",row[1]);
      else
         fprintf (Gbl.F.Out,"[%s]",Txt_no_subject);
     }
   else
      fprintf (Gbl.F.Out,"[%s]",Txt_first_message_not_allowed);

   /***** Free the structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************** Show posts of a thread in a discussion forum ***************/
/*****************************************************************************/

void For_ShowThrPsts (void)
  {
   long ThrCod;

   /***** Get order type, degree and course of the forum *****/
   For_GetParamsForum ();

   /***** Get the identifier of the thread that user want to show *****/
   ThrCod = For_GetParamThrCod ();

   /***** Show the posts of that thread *****/
   For_ShowForumLevel2 (ThrCod);
  }

/*****************************************************************************/
/********************* Get parameters related to a forum *********************/
/*****************************************************************************/

void For_GetParamsForum (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;
   char LongStr[1+10+1];

   /***** Get which forums I want to see *****/
   Par_GetParToText ("WhichForum",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      Gbl.Forum.WhichForums = (For_ForumOrderType_t) UnsignedNum;
   else
      Gbl.Forum.WhichForums = For_DEFAULT_WHICH_FORUMS;

   /***** Get order type *****/
   Par_GetParToText ("Order",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      Gbl.Forum.SelectedOrderType = (For_ForumOrderType_t) UnsignedNum;
   else
      Gbl.Forum.SelectedOrderType = For_DEFAULT_ORDER;

   /***** Get parameter with code of institution *****/
   Par_GetParToText ("ForInsCod",LongStr,1+10);
   Gbl.Forum.Ins.InsCod = Str_ConvertStrCodToLongCod (LongStr);
   Ins_GetDataOfInstitutionByCod (&Gbl.Forum.Ins,Ins_GET_BASIC_DATA);

   /***** Get parameter with code of institution *****/
   Par_GetParToText ("ForCtrCod",LongStr,1+10);
   Gbl.Forum.Ctr.CtrCod = Str_ConvertStrCodToLongCod (LongStr);
   Ctr_GetDataOfCentreByCod (&Gbl.Forum.Ctr);

   /***** Get parameter with code of degree *****/
   Par_GetParToText ("ForDegCod",LongStr,1+10);
   Gbl.Forum.Deg.DegCod = Str_ConvertStrCodToLongCod (LongStr);
   Deg_GetDataOfDegreeByCod (&Gbl.Forum.Deg);

   /***** Get parameter with code of course *****/
   Par_GetParToText ("ForCrsCod",LongStr,1+10);
   Gbl.Forum.Crs.CrsCod = Str_ConvertStrCodToLongCod (LongStr);
   Crs_GetDataOfCourseByCod (&Gbl.Forum.Crs);
  }

/*****************************************************************************/
/************ Write a form parameter to specify a thread code ****************/
/*****************************************************************************/

void For_PutHiddenParamThrCod (long ThrCod)
  {
   Par_PutHiddenParamLong ("ThrCod",ThrCod);
  }

/*****************************************************************************/
/********* Get the value of the parameter with the code of a thread **********/
/*****************************************************************************/

static long For_GetParamThrCod (void)
  {
   char StrThrCod[1+10+1];	// String that holds the thread code
   long ThrCod;

   /* Get thread code */
   Par_GetParToText ("ThrCod",StrThrCod,1+10);
   if (sscanf (StrThrCod,"%ld",&ThrCod) != 1)
      Lay_ShowErrorAndExit ("Wrong thread code.");

   return ThrCod;
  }

/*****************************************************************************/
/************* Write a form parameter to specify a post code *****************/
/*****************************************************************************/

static void For_PutHiddenParamPstCod (long PstCod)
  {
   Par_PutHiddenParamLong ("PstCod",PstCod);
  }

/*****************************************************************************/
/********** Get the value of the parameter with the code of a post ***********/
/*****************************************************************************/

static long For_GetParamPstCod (void)
  {
   char StrPstCod[1+10+1];	// String that holds the post code
   long PstCod;

   /* Get post code */
   Par_GetParToText ("PstCod",StrPstCod,1+10);
   if (sscanf (StrPstCod,"%ld",&PstCod) != 1)
      Lay_ShowErrorAndExit ("Wrong post code.");

   return PstCod;
  }

/*****************************************************************************/
/******************** Show posts of a thread of the forum ********************/
/*****************************************************************************/

void For_ShowForumLevel2 (long ThrCod)
  {
   char Subject[Cns_MAX_BYTES_SUBJECT+1];

   /***** Get order type, degree and course of the forum *****/
   For_GetParamsForum ();

   /***** Set forum type *****/
   For_SetForumTypeAndRestrictAccess ();

   /***** Show the posts of this thread *****/
   For_ShowThreadPosts (ThrCod,Subject);

   /***** Form to write a new message in the thread *****/
   For_WriteFormForumPst (true,ThrCod,Subject);
  }

/*****************************************************************************/
/********************** Show an area to write a message **********************/
/*****************************************************************************/

static void For_WriteFormForumPst (bool IsReply,long ThrCod,const char *Subject)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_New_message;
   extern const char *Txt_New_thread;
   extern const char *Txt_MSG_Subject;
   extern const char *Txt_MSG_Message;
   extern const char *Txt_Send_message;

   if (IsReply)	// Form to write a reply to a message of an existing thread
     {
      Act_FormStart (For_ActionsRecRepFor[Gbl.Forum.ForumType]);
      For_PutHiddenParamThrCod (ThrCod);
      Pag_PutHiddenParamPagNum (0);
     }
   else		// Form to write the first message of a new thread
      Act_FormStart (For_ActionsRecThrFor[Gbl.Forum.ForumType]);
   For_PutAllHiddenParamsForum ();

   /***** Start frame *****/
   Lay_StartRoundFrameTable (NULL,2,IsReply ? Txt_New_message :
        	                              Txt_New_thread);
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s: "
	              "</td>"
                      "<td class=\"LEFT_TOP\">"
                      "<textarea name=\"Subject\" cols=\"72\" rows=\"2\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_MSG_Subject);
   if (IsReply)	// If writing a reply to a message of an existing thread
      fprintf (Gbl.F.Out,"%s",Subject);
   fprintf (Gbl.F.Out,"</textarea>"
	              "</td>"
	              "</tr>"
	              "<tr>"
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s: "
	              "</td>"
                      "<td class=\"LEFT_TOP\">"
                      "<textarea name=\"Content\" cols=\"72\" rows=\"15\">"
                      "</textarea>"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_MSG_Message);

   /***** Help for text editor *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td colspan=\"2\">");
   Lay_HelpPlainEditor ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Send button and end frame *****/
   Lay_EndRoundFrameTableWithButton (Lay_CREATE_BUTTON,Txt_Send_message);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/************************** Receive message of a forum ***********************/
/*****************************************************************************/

void For_RecForumPst (void)
  {
   extern const char *Txt_Post_sent;
   extern const char *Txt_Do_not_reload_this_page_because_the_post_will_be_sent_again_;
   bool PstIsAReply = false;
   long ThrCod = 0;
   long PstCod = 0;
   unsigned NumUsrsToBeNotifiedByEMail;
   char Content[Cns_MAX_BYTES_LONG_TEXT+1];

   /***** Get order type, degree and course of the forum *****/
   For_GetParamsForum ();

   /***** Set forum type *****/
   For_SetForumTypeAndRestrictAccess ();

   /***** Get the code of the thread y the número of page *****/
   if (Gbl.Action.Act == ActRcvRepForCrsUsr || Gbl.Action.Act == ActRcvRepForCrsTch ||
       Gbl.Action.Act == ActRcvRepForDegUsr || Gbl.Action.Act == ActRcvRepForDegTch ||
       Gbl.Action.Act == ActRcvRepForCtrUsr || Gbl.Action.Act == ActRcvRepForCtrTch ||
       Gbl.Action.Act == ActRcvRepForInsUsr || Gbl.Action.Act == ActRcvRepForInsTch ||
       Gbl.Action.Act == ActRcvRepForGenUsr || Gbl.Action.Act == ActRcvRepForGenTch ||
       Gbl.Action.Act == ActRcvRepForSWAUsr || Gbl.Action.Act == ActRcvRepForSWATch)
     {
      PstIsAReply = true;

      /* Get the code of the thread */
      ThrCod = For_GetParamThrCod ();
     }

   /***** Get page number *****/
   Pag_GetParamPagNum (Pag_POSTS_FORUM);

   /***** Get message subject *****/
   Par_GetParToHTML ("Subject",Gbl.Msg.Subject,Cns_MAX_BYTES_SUBJECT);

   /***** Get message body *****/
   Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_LONG_TEXT,
                              Str_TO_RIGOROUS_HTML,false);

   /***** Create a new message *****/
   if (PstIsAReply)
     {
      // ThrCod has been received from form

      /***** Create last message of the thread *****/
      PstCod = For_InsertForumPst (ThrCod,Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Msg.Subject,Content);

      /***** Modify last message of the thread *****/
      For_UpdateThrLastPst (ThrCod,PstCod);
     }
   else	// New thread
     {
      /***** Create new thread with unknown first and last message codes *****/
      ThrCod = For_InsertForumThread (Gbl.Forum.ForumType,-1L);

      /***** Create first (and last) message of the thread *****/
      PstCod = For_InsertForumPst (ThrCod,Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Msg.Subject,Content);

      /***** Update first and last posts of new thread *****/
      For_UpdateThrFirstAndLastPst (ThrCod,PstCod,PstCod);
     }

   /***** Increment number of forum posts in my user's figures *****/
   Prf_IncrementNumForPstUsr (Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Write message of success *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_Post_sent);
   Lay_ShowAlert (Lay_INFO,Txt_Do_not_reload_this_page_because_the_post_will_be_sent_again_);

   /***** Notify the new post to users in course *****/
   switch (Gbl.Forum.ForumType)
     {
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
	 if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_FORUM_POST_COURSE,PstCod)))
	    For_UpdateNumUsrsNotifiedByEMailAboutPost (PstCod,NumUsrsToBeNotifiedByEMail);
	 Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);
	 break;
      default:
	 break;
     }

   /***** Notify the new post to previous writers in this thread *****/
   if (PstIsAReply)
     {
      if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_FORUM_REPLY,PstCod)))
         For_UpdateNumUsrsNotifiedByEMailAboutPost (PstCod,NumUsrsToBeNotifiedByEMail);
      Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);
     }

   /***** Insert forum post into public social activity *****/
   switch (Gbl.Forum.ForumType)	// Only if forum is public for any logged user
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM_SWAD_USRS:
         Soc_StoreAndPublishSocialNote (Soc_NOTE_FORUM_POST,PstCod);
         break;
      default:
	 break;
     }

   /***** Show again the posts of this thread of the forum *****/
   For_ShowForumLevel2 (ThrCod);
  }

/*****************************************************************************/
/********* Update number of users notified in table of forum posts **********/
/*****************************************************************************/

static void For_UpdateNumUsrsNotifiedByEMailAboutPost (long PstCod,unsigned NumUsrsToBeNotifiedByEMail)
  {
   char Query[512];

   /***** Update number of users notified *****/
   sprintf (Query,"UPDATE forum_post SET NumNotif=NumNotif+'%u'"
	          " WHERE PstCod='%ld'",
            NumUsrsToBeNotifiedByEMail,PstCod);
   DB_QueryUPDATE (Query,"can not update the number of notifications of a post");
  }

/*****************************************************************************/
/***************************** Delete a forum post ***************************/
/*****************************************************************************/

void For_DelPst (void)
  {
   extern const char *Txt_Post_and_thread_removed;
   extern const char *Txt_Post_removed;
   long PstCod,ThrCod;
   struct UsrData UsrDat;
   time_t CreatTimeUTC;	// Creation time of a message
   char Subject[Cns_MAX_BYTES_SUBJECT+1];
   char OriginalContent[Cns_MAX_BYTES_TEXT+1];
   bool ThreadDeleted = false;

   /***** Get the code of the message to delete *****/
   PstCod = For_GetParamPstCod ();

   /***** Delete the message *****/
   /* Get order type, degree and course of the forum */
   For_GetParamsForum ();
   /* Set forum type */
   For_SetForumTypeAndRestrictAccess ();
   /* Get the thread number */
   ThrCod = For_GetParamThrCod ();

   /* Check if the message really exists, if it has not been removed */
   if (!For_GetIfForumPstExists (PstCod))
      Lay_ShowErrorAndExit ("The post to remove no longer exists.");

   /* Check if I am the author of the message */
   For_GetPstData (PstCod,&UsrDat.UsrCod,&CreatTimeUTC,Subject,OriginalContent);
   if (Gbl.Usrs.Me.UsrDat.UsrCod != UsrDat.UsrCod)
      Lay_ShowErrorAndExit ("You can not remove post because you aren't the author.");

   /* Check if the message is the last message in the thread */
   if (PstCod != For_GetLastPstCod (ThrCod))
      Lay_ShowErrorAndExit ("You can not remove post because it is not the last of the thread.");

   /* Remove the message */
   ThreadDeleted = For_RemoveForumPst (PstCod);

   /***** Mark possible notifications as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_FORUM_POST_COURSE,PstCod);
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_FORUM_REPLY,PstCod);

   /***** Mark possible social note as unavailable *****/
   switch (Gbl.Forum.ForumType)	// Only if forum is public for any logged user
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM_SWAD_USRS:
         Soc_MarkSocialNoteAsUnavailableUsingNoteTypeAndCod (Soc_NOTE_FORUM_POST,PstCod);
         break;
      default:
	 break;
     }

   if (ThreadDeleted)
     {
      Lay_ShowAlert (Lay_SUCCESS,Txt_Post_and_thread_removed);

      /* Show the remaining threads */
      For_ShowForumThrs ();
     }
   else
     {
      Lay_ShowAlert (Lay_SUCCESS,Txt_Post_removed);

      /* Show the remaining posts */
      For_ShowForumLevel2 (ThrCod);
     }
  }

/*****************************************************************************/
/**************** Solicitar the eliminación of a existing thread **************/
/*****************************************************************************/

void For_ReqDelThr (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_entire_thread_X;
   extern const char *Txt_Do_you_really_want_to_remove_the_entire_thread;
   extern const char *Txt_Remove_thread;
   long ThrCod;
   char Subject[Cns_MAX_BYTES_SUBJECT+1];

   /***** Get order type, degree and course of the forum *****/
   For_GetParamsForum ();

   /***** Initialize the type of forum *****/
   For_SetForumTypeAndRestrictAccess ();

   /***** Get the identifier of the thread to remove *****/
   ThrCod = For_GetParamThrCod ();

   /***** Get subject of thread to delete *****/
   For_GetThrSubject (ThrCod,Subject,Cns_MAX_BYTES_SUBJECT);

   /***** Request confirmation to remove the thread *****/
   if (Subject[0])
      sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_entire_thread_X,
               Subject);
   else
      sprintf (Gbl.Message,"%s",Txt_Do_you_really_want_to_remove_the_entire_thread);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);

   Act_FormStart (For_ActionsDelThrFor[Gbl.Forum.ForumType]);
   For_PutAllHiddenParamsForum ();
   For_PutHiddenParamThrCod (ThrCod);
   Lay_PutRemoveButton (Txt_Remove_thread);
   Act_FormEnd ();
  }

/*****************************************************************************/
/*************************** Remove an existing thread ***********************/
/*****************************************************************************/

void For_DelThr (void)
  {
   extern const char *Txt_Thread_X_removed;
   extern const char *Txt_Thread_removed;
   long ThrCod;
   char Subject[Cns_MAX_BYTES_SUBJECT+1];

   /***** Get order type, degree and course of the forum *****/
   For_GetParamsForum ();

   /***** Set forum type *****/
   For_SetForumTypeAndRestrictAccess ();

   if (PermissionThreadDeletion[Gbl.Forum.ForumType] & (1 << Gbl.Usrs.Me.LoggedRole)) // If I have permission to remove thread in this forum...
     {
      /***** Get code of thread to delete *****/
      ThrCod = For_GetParamThrCod ();

      /***** Get subject of thread to delete *****/
      For_GetThrSubject (ThrCod,Subject,Cns_MAX_BYTES_SUBJECT);

      /***** Remove the thread and all its posts *****/
      For_RemoveThreadAndItsPsts (ThrCod);

      /***** Write message confirming the deletion *****/
      if (Subject[0])
      	{
         sprintf (Gbl.Message,Txt_Thread_X_removed,
                  Subject);
         Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
        }
      else
         Lay_ShowAlert (Lay_SUCCESS,Txt_Thread_removed);
     }
   else
      Lay_ShowErrorAndExit ("You can not remove threads in this forum.");

   /***** Show the threads again *****/
   For_ShowForumThrs ();
  }

/*****************************************************************************/
/*************** Cut a thread to move it to another forum ********************/
/*****************************************************************************/

void For_CutThr (void)
  {
   extern const char *Txt_Thread_X_marked_to_be_moved;
   extern const char *Txt_Thread_marked_to_be_moved;
   long ThrCod;
   char Subject[Cns_MAX_BYTES_SUBJECT+1];

   /***** Get order type, degree and course of the forum *****/
   For_GetParamsForum ();

   /***** Set forum type *****/
   For_SetForumTypeAndRestrictAccess ();

   /***** Get code of thread to cut *****/
   ThrCod = For_GetParamThrCod ();

   /***** Get subject of thread to cut *****/
   For_GetThrSubject (ThrCod,Subject,Cns_MAX_BYTES_SUBJECT);

   /***** Mark the thread as cut *****/
   For_InsertThrInClipboard (ThrCod);

   /***** Write message confirming that thread has been marked to move it *****/
   if (Subject[0])
      sprintf (Gbl.Message,Txt_Thread_X_marked_to_be_moved,
               Subject);
   else
      sprintf (Gbl.Message,"%s",Txt_Thread_marked_to_be_moved);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show the threads again *****/
   For_ShowForumThrs ();
  }

/*****************************************************************************/
/************* Paste the thread in clipboard into current forum **************/
/*****************************************************************************/

void For_PasteThr (void)
  {
   extern const char *Txt_The_thread_X_is_already_in_this_forum;
   extern const char *Txt_The_thread_is_already_in_this_forum;
   extern const char *Txt_Thread_X_moved_to_this_forum;
   extern const char *Txt_Thread_moved_to_this_forum;
   long ThrCod;
   char Subject[Cns_MAX_BYTES_SUBJECT+1];

   /***** Get order type, degree and course of the forum *****/
   For_GetParamsForum ();

   /***** Set forum type *****/
   For_SetForumTypeAndRestrictAccess ();

   /***** Get code of thread to paste *****/
   ThrCod = For_GetParamThrCod ();

   /***** Get subject of thread to paste *****/
   For_GetThrSubject (ThrCod,Subject,Cns_MAX_BYTES_SUBJECT);

   /***** Paste (move) the thread to current forum *****/
   if (For_CheckIfThrBelongsToForum (ThrCod,Gbl.Forum.ForumType))
     {
      if (Subject[0])
         sprintf (Gbl.Message,Txt_The_thread_X_is_already_in_this_forum,
                  Subject);
      else
         sprintf (Gbl.Message,"%s",Txt_The_thread_is_already_in_this_forum);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
   else
     {
      For_MoveThrToCurrentForum (ThrCod);
      if (Subject[0])
         sprintf (Gbl.Message,Txt_Thread_X_moved_to_this_forum,
                  Subject);
      else
         sprintf (Gbl.Message,"%s",Txt_Thread_moved_to_this_forum);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Show the threads again *****/
   For_ShowForumThrs ();
  }

/*****************************************************************************/
/**************** Get if there is a thread ready to be moved ****************/
/*****************************************************************************/

long For_GetThrInMyClipboard (void)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   long ThrCod = -1L;;

   /***** Get if there is a thread ready to move in my clipboard from database *****/
   sprintf (Query,"SELECT ThrCod FROM forum_thr_clip WHERE UsrCod='%ld'",
            Gbl.Usrs.Me.UsrDat.UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not check if there is any thread ready to be moved");

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
/**************** Get if a thread belongs to current forum ******************/
/*****************************************************************************/

bool For_CheckIfThrBelongsToForum (long ThrCod,For_ForumType_t ForumType)
  {
   char SubQuery[256];
   char Query[512];

   /***** Get if a thread belong to current forum from database *****/
   switch (ForumType)
     {
      case For_FORUM_INSTITUTION_USRS:	case For_FORUM_INSTITUTION_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",Gbl.Forum.Ins.InsCod);
         break;
      case For_FORUM_CENTRE_USRS:	case For_FORUM_CENTRE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",Gbl.Forum.Ins.InsCod);
         break;
      case For_FORUM_DEGREE_USRS:	case For_FORUM_DEGREE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",Gbl.Forum.Deg.DegCod);
         break;
      case For_FORUM_COURSE_USRS:	case For_FORUM_COURSE_TCHS:
         sprintf (SubQuery," AND forum_thread.Location='%ld'",Gbl.Forum.Crs.CrsCod);
         break;
      default:
         SubQuery[0] = '\0';
         break;
     }
   sprintf (Query,"SELECT COUNT(*) FROM forum_thread WHERE ThrCod='%ld' AND ForumType='%u'%s",
            ThrCod,(unsigned) ForumType,SubQuery);
   return (DB_QueryCOUNT (Query,"can not get if a thread belong to current forum") != 0);
  }

/*****************************************************************************/
/************************ Move a thread to current forum *********************/
/*****************************************************************************/

void For_MoveThrToCurrentForum (long ThrCod)
  {
   char Query[512];

   /***** Move a thread to current forum *****/
   switch (Gbl.Forum.ForumType)
     {
      case For_FORUM_SWAD_USRS:		case For_FORUM_SWAD_TCHS:
      case For_FORUM_GLOBAL_USRS:	case For_FORUM_GLOBAL_TCHS:
         sprintf (Query,"UPDATE forum_thread SET ForumType='%u',Location='-1' WHERE ThrCod='%ld'",
                  (unsigned) Gbl.Forum.ForumType,ThrCod);
         break;
      case For_FORUM_INSTITUTION_USRS:	case For_FORUM_INSTITUTION_TCHS:
         sprintf (Query,"UPDATE forum_thread SET ForumType='%u',Location='%ld' WHERE ThrCod='%ld'",
                  (unsigned) Gbl.Forum.ForumType,Gbl.Forum.Ins.InsCod,ThrCod);
         break;
      case For_FORUM_CENTRE_USRS:	case For_FORUM_CENTRE_TCHS:
         sprintf (Query,"UPDATE forum_thread SET ForumType='%u',Location='%ld' WHERE ThrCod='%ld'",
                  (unsigned) Gbl.Forum.ForumType,Gbl.Forum.Ctr.CtrCod,ThrCod);
         break;
      case For_FORUM_DEGREE_USRS:	case For_FORUM_DEGREE_TCHS:
         sprintf (Query,"UPDATE forum_thread SET ForumType='%u',Location='%ld' WHERE ThrCod='%ld'",
                  (unsigned) Gbl.Forum.ForumType,Gbl.Forum.Deg.DegCod,ThrCod);
         break;
      case For_FORUM_COURSE_USRS:	case For_FORUM_COURSE_TCHS:
         sprintf (Query,"UPDATE forum_thread SET ForumType='%u',Location='%ld' WHERE ThrCod='%ld'",
                  (unsigned) Gbl.Forum.ForumType,Gbl.Forum.Crs.CrsCod,ThrCod);
         break;
     }
   DB_QueryUPDATE (Query,"can not move a thread to current forum");
  }

/*****************************************************************************/
/********************* Insert thread in thread clipboard ********************/
/*****************************************************************************/

void For_InsertThrInClipboard (long ThrCod)
  {
   char Query[512];

   /***** Remove expired thread clipboards *****/
   For_RemoveExpiredThrsClipboards ();

   /***** Add thread to my clipboard *****/
   sprintf (Query,"REPLACE INTO forum_thr_clip (ThrCod,UsrCod)"
                  " VALUES ('%ld','%ld')",
            ThrCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryREPLACE (Query,"can not add thread to clipboard");
  }

/*****************************************************************************/
/************* Remove expired thread clipboards (from all users) *************/
/*****************************************************************************/

void For_RemoveExpiredThrsClipboards (void)
  {
   char Query[512];

   /***** Remove all expired clipboards *****/
   sprintf (Query,"DELETE LOW_PRIORITY FROM forum_thr_clip"
                  " WHERE TimeInsert<FROM_UNIXTIME(UNIX_TIMESTAMP()-'%lu')",
            Cfg_TIME_TO_DELETE_THREAD_CLIPBOARD);
   DB_QueryDELETE (Query,"can not remove old threads from clipboards");
  }

/*****************************************************************************/
/**************** Remove thread code from thread clipboard *******************/
/*****************************************************************************/

void For_RemoveThrCodFromThrClipboard (long ThrCod)
  {
   char Query[512];

   /***** Remove thread from thread clipboard *****/
   sprintf (Query,"DELETE FROM forum_thr_clip WHERE ThrCod='%ld'",ThrCod);
   DB_QueryDELETE (Query,"can not remove a thread from clipboard");
  }

/*****************************************************************************/
/********************* Remove thread clipboard of a user *********************/
/*****************************************************************************/

void For_RemoveUsrFromThrClipboard (long UsrCod)
  {
   char Query[512];

   /***** Remove clipboard of specified user *****/
   sprintf (Query,"DELETE FROM forum_thr_clip WHERE UsrCod='%ld'",UsrCod);
   DB_QueryDELETE (Query,"can not remove a thread from the clipboard of a user");
  }
