// swad_timeline.h: social timeline

#ifndef _SWAD_TL
#define _SWAD_TL
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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
/********************************** Headers **********************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define TL_TIMELINE_SECTION_ID	"timeline"

#define TL_ICON_ELLIPSIS	"ellipsis-h.svg"

#define TL_DEF_USRS_SHOWN	5	// Default maximum number of users shown who have share/fav a note
#define TL_MAX_USRS_SHOWN	1000	// Top     maximum number of users shown who have share/fav a note

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define TL_NUM_NOTE_TYPES	13
// If the numbers assigned to each event type change,
// it is necessary to change old numbers to new ones in database table tl_notes
typedef enum
  {
   TL_NOTE_UNKNOWN		=  0,
   /* Start tab */
   TL_NOTE_POST			= 10,	// Post written directly in timeline
   /* Institution tab */
   TL_NOTE_INS_DOC_PUB_FILE	=  1,	// Public file in documents of institution
   TL_NOTE_INS_SHA_PUB_FILE	=  2,	// Public file in shared files of institution
   /* Centre tab */
   TL_NOTE_CTR_DOC_PUB_FILE	=  3,	// Public file in documents of centre
   TL_NOTE_CTR_SHA_PUB_FILE	=  4,	// Public file in shared files of centre
   /* Degree tab */
   TL_NOTE_DEG_DOC_PUB_FILE	=  5,	// Public file in documents of degree
   TL_NOTE_DEG_SHA_PUB_FILE	=  6,	// Public file in shared files of degree
   /* Course tab */
   TL_NOTE_CRS_DOC_PUB_FILE	=  7,	// Public file in documents of course
   TL_NOTE_CRS_SHA_PUB_FILE	=  8,	// Public file in shared files of course
   /* Assessment tab */
   TL_NOTE_EXAM_ANNOUNCEMENT	=  9,	// Exam announcement in a course
   /* Users tab */
   /* Messages tab */
   TL_NOTE_NOTICE		= 12,	// A public notice in a course
   TL_NOTE_FORUM_POST		= 11,	// Post in global/swad forums
   /* Analytics tab */
   /* Profile tab */
  } TL_NoteType_t;

#define TL_NUM_TOP_MESSAGES (1 + 6)
typedef enum
  {
   TL_TOP_MESSAGE_NONE		= 0,
   TL_TOP_MESSAGE_COMMENTED	= 1,
   TL_TOP_MESSAGE_FAVED		= 2,
   TL_TOP_MESSAGE_UNFAVED	= 3,
   TL_TOP_MESSAGE_SHARED	= 4,
   TL_TOP_MESSAGE_UNSHARED	= 5,
   TL_TOP_MESSAGE_MENTIONED	= 6,
  } TL_TopMessage_t;

typedef enum
  {
   TL_TIMELINE_USR,	// Show the timeline of a user
   TL_TIMELINE_GBL,	// Show the timeline of the users follwed by me
  } TL_UsrOrGbl_t;

#define TL_NUM_WHAT_TO_GET 3
typedef enum
  {
   TL_GET_ONLY_NEW_PUBS,	// New publications are retrieved via AJAX
				// automatically from time to time
   TL_GET_RECENT_TIMELINE,	// Recent timeline is shown when the user clicks on action menu,...
				// or after editing timeline
   TL_GET_ONLY_OLD_PUBS,	// Old publications are retrieved via AJAX
				// when the user clicks on link at bottom of timeline
  } TL_WhatToGet_t;

struct TL_Timeline
  {
   TL_UsrOrGbl_t UsrOrGbl;
   Usr_Who_t Who;
   TL_WhatToGet_t WhatToGet;
   long NotCod;		// Used as parameter about social note to be edited, removed...
   long PubCod;		// Used as parameter about social publishing to be edited, removed...
  };

typedef enum
  {
   TL_SHOW_FEW_USRS,	// Show a few first favers/sharers
   TL_SHOW_ALL_USRS,	// Show all favers/sharers
  } TL_HowManyUsrs_t;

#define TL_NUM_PUB_TYPES	4
// If the numbers assigned to each event type change,
// it is necessary to change old numbers to new ones in database table tl_notes
typedef enum
  {
   TL_PUB_UNKNOWN		= 0,
   TL_PUB_ORIGINAL_NOTE		= 1,
   TL_PUB_SHARED_NOTE		= 2,
   TL_PUB_COMMENT_TO_NOTE	= 3,
  } TL_PubType_t;

struct TL_Publication
  {
   long PubCod;
   long NotCod;
   long PublisherCod;		// Sharer or writer of the publication
   TL_PubType_t PubType;
   time_t DateTimeUTC;
   TL_TopMessage_t TopMessage;	// Used to show feedback on the action made
  };

struct TL_PostContent
  {
   char Txt[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Media Media;
  };

struct TL_Note
  {
   long NotCod;			// Unique code/identifier for each note
   TL_NoteType_t NoteType;	// Timeline post, public file, exam announcement, notice, forum post...
   long UsrCod;			// Publisher
   long HieCod;			// Hierarchy code (institution/centre/degree/course)
   long Cod;			// Code of file, forum post, notice, timeline post...
   bool Unavailable;		// File, forum post, notice,... unavailable (removed)
   time_t DateTimeUTC;		// Date-time of publication in UTC time
   unsigned NumShared;		// Number of times (users) this note has been shared
   unsigned NumFavs;		// Number of times (users) this note has been favourited
  };

struct TL_Comment
  {
   long PubCod;			// Unique code/identifier for each publication
   long UsrCod;			// Publisher
   long NotCod;			// Note code to which this comment belongs
   time_t DateTimeUTC;		// Date-time of publication in UTC time
   unsigned NumFavs;		// Number of times (users) this comment has been favourited
   struct TL_PostContent Content;
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void TL_ResetTimeline (struct TL_Timeline *Timeline);
void TL_ShowTimelineGbl (void);

void TL_ShowTimelineUsr (struct TL_Timeline *Timeline);

void TL_RefreshNewTimelineGbl (void);

void TL_RefreshOldTimelineGbl (void);
void TL_RefreshOldTimelineUsr (void);

void TL_MarkMyNotifAsSeen (void);

void TL_GetParamWho (void);
Usr_Who_t TL_GetGlobalWho (void);

void TL_StoreAndPublishNote (TL_NoteType_t NoteType,long Cod);
void TL_MarkNoteAsUnavailable (TL_NoteType_t NoteType,long Cod);
void TL_MarkNoteOneFileAsUnavailable (const char *Path);
void TL_MarkNotesChildrenOfFolderAsUnavailable (const char *Path);

void TL_PublishNoteInTimeline (struct TL_Publication *SocPub);

void TL_ReceivePostUsr (void);
void TL_ReceivePostGbl (void);

void TL_ShowHiddenCommentsUsr (void);
void TL_ShowHiddenCommentsGbl (void);

void TL_PutHiddenParamPubCod (long PubCod);
long TL_GetParamNotCod (void);
long TL_GetParamPubCod (void);

void TL_ReceiveCommentUsr (void);
void TL_ReceiveCommentGbl (void);

void TL_CreateNotifToAuthor (long AuthorCod,long PubCod,
                             Ntf_NotifyEvent_t NotifyEvent);

void TL_RequestRemNoteUsr (void);
void TL_RequestRemNoteGbl (void);
void TL_RemoveNoteUsr (void);
void TL_RemoveNoteGbl (void);

long TL_GetPubCodOfOriginalNote (long NotCod);

void TL_RequestRemComUsr (void);
void TL_RequestRemComGbl (void);
void TL_RemoveComUsr (void);
void TL_RemoveComGbl (void);

void TL_RemoveUsrContent (long UsrCod);

void TL_ShowNumSharersOrFavers (unsigned NumUsrs);
void TL_ShowSharersOrFavers (MYSQL_RES **mysql_res,
			     unsigned NumUsrs,unsigned NumFirstUsrs);
void TL_PutFormToSeeAllFaversSharers (Act_Action_t ActionGbl,Act_Action_t ActionUsr,
		                      const char *ParamFormat,long ParamCod,
                                      TL_HowManyUsrs_t HowManyUsrs);
void TL_FormFavSha (Act_Action_t ActionGbl,Act_Action_t ActionUsr,
		    const char *ParamFormat,long ParamCod,
		    const char *Icon,const char *Title);

void TL_GetDataOfNoteByCod (struct TL_Note *SocNot);
void TL_GetDataOfCommByCod (struct TL_Comment *SocCom);

void TL_ClearOldTimelinesDB (void);

void TL_GetNotifPublication (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                             char **ContentStr,
                             long PubCod,bool GetContent);

unsigned long TL_GetNumPubsUsr (long UsrCod);

#endif
