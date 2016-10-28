// swad_forum.h: forums

#ifndef _SWAD_FOR
#define _SWAD_FOR
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

#include "swad_centre.h"
#include "swad_degree.h"
#include "swad_institution.h"
#include "swad_pagination.h"
#include "swad_scope.h"
#include "swad_string.h"

/*****************************************************************************/
/************************ Public constants and types *************************/
/*****************************************************************************/

#define For_NUM_WHICH_FORUMS 2
typedef enum
  {
   For_ONLY_CURRENT_FORUMS	= 0,
   For_ALL_MY_FORUMS		= 1,
  } For_WhichForums_t;	// Which forums I want to see: all my forums or only the forums of current institution/degree/course.
#define For_DEFAULT_WHICH_FORUMS For_ONLY_CURRENT_FORUMS

#define For_NUM_TYPES_FORUM 12
typedef enum
  {
   For_FORUM_COURSE_USRS	=  0,
   For_FORUM_COURSE_TCHS	=  1,
   For_FORUM_DEGREE_USRS	=  2,
   For_FORUM_DEGREE_TCHS	=  3,
   For_FORUM_CENTRE_USRS	=  4,
   For_FORUM_CENTRE_TCHS	=  5,
   For_FORUM_INSTIT_USRS	=  6,
   For_FORUM_INSTIT_TCHS	=  7,
   For_FORUM_GLOBAL_USRS	=  8,
   For_FORUM_GLOBAL_TCHS	=  9,
   For_FORUM_SWAD_USRS		= 10,
   For_FORUM_SWAD_TCHS		= 11,
  } For_ForumType_t;	// Don't change numbers (used in database)

struct ForumThread
  {
   long ThrCod;
   long PstCod[2];
   long UsrCod[2];
   time_t WriteTime[2];
   bool Enabled[2];
   char Subject[Cns_MAX_BYTES_SUBJECT+1];
   unsigned NumPosts;
   unsigned NumUnreadPosts;	// Number of my unread posts in thread
   unsigned NumMyPosts;		// Number of posts written by me in thread
   unsigned NumWriters;
   unsigned NumReaders;
  };
typedef enum
  {
   For_FIRST_MSG = 0,
   For_LAST_MSG  = 1,
  } For_ForumOrderType_t;
#define For_DEFAULT_ORDER For_LAST_MSG

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void For_EnbPst (void);
void For_DisPst (void);

For_ForumType_t For_GetForumTypeOfAPost (long PstCod);

unsigned long For_GetNumPostsUsr (long UsrCod);
void For_DeleteThrFromReadThrs (long ThrCod);
void For_RemoveUsrFromReadThrs (long UsrCod);

void For_GetSummaryAndContentForumPst (char *SummaryStr,char **ContentStr,
                                       long PstCod,
                                       unsigned MaxChars,bool GetContent);

void For_PutAllHiddenParamsForum (void);
void For_SetForumTypeAndRestrictAccess (void);
void For_ShowForumList (void);
void For_SetForumName (For_ForumType_t ForumType,
                       struct Instit *Ins,
                       struct Centre *Ctr,
                       struct Degree *Deg,
                       struct Course *Crs,
                       char *ForumName,Txt_Language_t Language,bool UseHTMLEntities);
unsigned For_GetNumThrsWithNewPstsInForum (For_ForumType_t ForumType,unsigned NumThreads);
void For_ShowForumThrs (void);
unsigned For_GetNumTotalForumsOfType (For_ForumType_t ForumType,
                                      long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod);
unsigned For_GetNumTotalThrsInForumsOfType (For_ForumType_t ForumType,
                                            long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod);
unsigned For_GetNumThrsInForum (For_ForumType_t ForumType);
unsigned For_GetNumTotalPstsInForumsOfType (For_ForumType_t ForumType,
                                            long CtyCod,long InsCod,long CtrCod,long DegCod,long CrsCod,
                                            unsigned *NumUsrsToBeNotifiedByEMail);
unsigned For_GetNumPstsInForum (For_ForumType_t ForumType);
void For_ListForumThrs (long ThrCods[Pag_ITEMS_PER_PAGE],struct Pagination *PaginationThrs);
void For_GetThrData (struct ForumThread *Thr);
void For_ShowThrPsts (void);
void For_GetParamsForum (void);
void For_PutHiddenParamThrCod (long ThrCod);
void For_ShowForumLevel2 (long ThrCod);
void For_RecForumPst (void);
void For_DelPst (void);
void For_ReqDelThr (void);
void For_DelThr (void);
void For_CutThr (void);
void For_PasteThr (void);
long For_GetThrInMyClipboard (void);
bool For_CheckIfThrBelongsToForum (long ThrCod,For_ForumType_t ForumType);
void For_MoveThrToCurrentForum (long ThrCod);
void For_InsertThrInClipboard (long ThrCod);
void For_RemoveExpiredThrsClipboards (void);
void For_RemoveThrCodFromThrClipboard (long ThrCod);
void For_RemoveUsrFromThrClipboard (long UsrCod);

void For_RemoveForums (Sco_Scope_t Scope,long DegCod);

#endif
