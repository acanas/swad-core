// swad_timeline_note.h: social timeline notes

#ifndef _SWAD_TL_NOT
#define _SWAD_TL_NOT
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include "swad_timeline.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define TL_NOT_NUM_NOTE_TYPES	13
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
  } TL_Not_NoteType_t;

struct TL_Not_Note
  {
   long NotCod;			// Unique code/identifier for each note
   TL_Not_NoteType_t NoteType;	// Timeline post, public file, exam announcement, notice, forum post...
   long UsrCod;			// Publisher
   long HieCod;			// Hierarchy code (institution/centre/degree/course)
   long Cod;			// Code of file, forum post, notice, timeline post...
   bool Unavailable;		// File, forum post, notice,... unavailable (removed)
   time_t DateTimeUTC;		// Date-time of publication in UTC time
   unsigned NumShared;		// Number of times (users) this note has been shared
   unsigned NumFavs;		// Number of times (users) this note has been favourited
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void TL_Not_ShowHighlightedNote (struct TL_Timeline *Timeline,
                                 struct TL_Not_Note *Not);

void TL_Not_InsertNoteInJustRetrievedNotes (long NotCod);
void TL_Not_InsertNoteInVisibleTimeline (long NotCod);

void TL_Not_WriteNoteInList (const struct TL_Timeline *Timeline,
	                     const struct TL_Not_Note *Not,
                             TL_TopMessage_t TopMessage,
                             long PublisherCod,			// Who did the action (publication, commenting, faving, sharing, mentioning)
                             TL_Highlight_t Highlight,		// Highlight note
                             TL_ShowAlone_t ShowNoteAlone);	// Note is shown alone, not in a list
void TL_Not_WriteAuthorName (const struct UsrData *UsrDat);

void TL_Not_GetNoteSummary (const struct TL_Not_Note *Not,
                            char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1]);

void TL_Not_StoreAndPublishNote (TL_Not_NoteType_t NoteType,long Cod);
void TL_Not_StoreAndPublishNoteInternal (TL_Not_NoteType_t NoteType,long Cod,struct TL_Pub_Publication *Pub);
void TL_Not_MarkNoteAsUnavailable (TL_Not_NoteType_t NoteType,long Cod);
void TL_Not_MarkNoteOneFileAsUnavailable (const char *Path);
void TL_Not_MarkNotesChildrenOfFolderAsUnavailable (const char *Path);

void TL_Not_PutHiddenParamNotCod (long NotCod);
long TL_Not_GetParamNotCod (void);

void TL_Not_RequestRemNoteUsr (void);
void TL_Not_RequestRemNoteGbl (void);
void TL_Not_RemoveNoteUsr (void);
void TL_Not_RemoveNoteGbl (void);

long TL_Not_GetPubCodOfOriginalNote (long NotCod);

void TL_Not_AddNotesJustRetrievedToVisibleTimelineThisSession (void);

void TL_Not_GetDataOfNoteByCod (struct TL_Not_Note *Not);

void TL_Not_ClearOldTimelinesNotesFromDB (void);
void TL_Not_ClearTimelineNotesThisSessionFromDB (void);

void TL_Not_CreateTmpTablesWithNotesAlreadyRetrieved (const struct TL_Timeline *Timeline);
void TL_Not_DropTmpTableJustRetrievedNotes (void);
void TL_Not_DropTmpTableVisibleTimeline (void);

#endif
