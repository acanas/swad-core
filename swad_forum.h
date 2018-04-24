// swad_forum.h: forums

#ifndef _SWAD_FOR
#define _SWAD_FOR
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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

#include "swad_centre.h"
#include "swad_degree.h"
#include "swad_institution.h"
#include "swad_notification.h"
#include "swad_pagination.h"
#include "swad_scope.h"
#include "swad_string.h"

/*****************************************************************************/
/************************ Public constants and types *************************/
/*****************************************************************************/

#define For_NUM_FORUM_SETS 2
typedef enum
  {
   For_ONLY_CURRENT_FORUMS	= 0,
   For_ALL_MY_FORUMS		= 1,
  } For_ForumSet_t;	// Which set of forums I want to see: all my forums or only the forums of current institution/degree/course.
#define For_DEFAULT_FORUM_SET For_ONLY_CURRENT_FORUMS

#define For_NUM_TYPES_FORUM 13
typedef enum
  {
   For_FORUM_COURSE_USRS	=  0,	// TODO: Change to 11 (also in database)
   For_FORUM_COURSE_TCHS	=  1,	// TODO: Change to 12 (also in database)
   For_FORUM_DEGREE_USRS	=  2,	// TODO: Change to  9 (also in database)
   For_FORUM_DEGREE_TCHS	=  3,	// TODO: Change to 10 (also in database)
   For_FORUM_CENTRE_USRS	=  4,	// TODO: Change to  7 (also in database)
   For_FORUM_CENTRE_TCHS	=  5,	// TODO: Change to  8 (also in database)
   For_FORUM_INSTIT_USRS	=  6,	// TODO: Change to  5 (also in database)
   For_FORUM_INSTIT_TCHS	=  7,	// TODO: Change to  6 (also in database)
   For_FORUM_GLOBAL_USRS	=  8,	// TODO: Change to  1 (also in database)
   For_FORUM_GLOBAL_TCHS	=  9,	// TODO: Change to  2 (also in database)
   For_FORUM__SWAD__USRS	= 10,	// TODO: Change to  3 (also in database)
   For_FORUM__SWAD__TCHS	= 11,	// TODO: Change to  4 (also in database)
   For_FORUM_UNKNOWN		= 12,	// TODO: Change to  0 (also in database)
  } For_ForumType_t;	// Don't change numbers (used in database)

struct Forum
  {
   For_ForumType_t Type;	// Type of forum
   long Location;		// Code of institution, centre, degree or course
   long ThrCod;			// Optional thread code
   long PstCod;			// Optional post code
  };

struct ForumThread
  {
   long ThrCod;
   long PstCod[2];
   long UsrCod[2];
   time_t WriteTime[2];
   bool Enabled[2];
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   unsigned NumPosts;
   unsigned NumUnreadPosts;	// Number of my unread posts in thread
   unsigned NumMyPosts;		// Number of posts written by me in thread
   unsigned NumWriters;
   unsigned NumReaders;
  };

#define For_NUM_ORDERS 2
typedef enum
  {
   For_FIRST_MSG = 0,
   For_LAST_MSG  = 1,
  } For_Order_t;
#define For_DEFAULT_ORDER For_LAST_MSG

#define For_MAX_BYTES_FORUM_NAME (512 - 1)

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void For_EnablePost (void);
void For_DisablePost (void);

void For_GetForumTypeAndLocationOfAPost (long PstCod,struct Forum *Forum);

unsigned long For_GetNumPostsUsr (long UsrCod);
void For_RemoveUsrFromReadThrs (long UsrCod);

void For_GetSummaryAndContentForumPst (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                       char **ContentStr,
                                       long PstCod,bool GetContent);

void For_PutAllHiddenParamsForum (unsigned NumPageThreads,
                                  unsigned NumPagePosts,
                                  For_ForumSet_t ForumSet,
                                  For_Order_t Order,
                                  long Location,
                                  long ThrCod,
                                  long PstCod);

void For_SetForumName (struct Forum *Forum,
                       char ForumName[For_MAX_BYTES_FORUM_NAME + 1],
                       Txt_Language_t Language,bool UseHTMLEntities);

void For_ShowForumTheads (void);
unsigned For_GetNumTotalForumsOfType (For_ForumType_t ForumType,
                                      long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod);
unsigned For_GetNumTotalThrsInForumsOfType (For_ForumType_t ForumType,
                                            long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod);
unsigned For_GetNumTotalPstsInForumsOfType (For_ForumType_t ForumType,
                                            long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod,
                                            unsigned *NumUsrsToBeNotifiedByEMail);

void For_ShowThreadPosts (void);
void For_ReceiveForumPost (void);
void For_RemovePost (void);
void For_RequestRemoveThread (void);
void For_RemoveThread (void);
void For_CutThread (void);
void For_PasteThread (void);
void For_RemoveUsrFromThrClipboard (long UsrCod);

void For_RemoveForums (Sco_Scope_t Scope,long ForumLocation);

#endif
