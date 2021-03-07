// swad_timeline_note.h: social timeline notes

#ifndef _SWAD_TML_NOT
#define _SWAD_TML_NOT
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
// it is necessary to change old numbers to new ones in database table tml_notes
typedef enum
  {
   TL_NOTE_UNKNOWN		=  0,
   /* Start tab */
   TL_NOTE_POST			= 10,	// Post written directly in timeline
   /* Institution tab */
   TL_NOTE_INS_DOC_PUB_FILE	=  1,	// Public file in documents of institution
   TL_NOTE_INS_SHA_PUB_FILE	=  2,	// Public file in shared files of institution
   /* Center tab */
   TL_NOTE_CTR_DOC_PUB_FILE	=  3,	// Public file in documents of center
   TL_NOTE_CTR_SHA_PUB_FILE	=  4,	// Public file in shared files of center
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
  } Tml_Not_NoteType_t;

struct Tml_Not_Note
  {
   long NotCod;			// Unique code/identifier for each note
   Tml_Not_NoteType_t NoteType;	// Timeline post, public file, exam announcement, notice, forum post...
   long UsrCod;			// Publisher
   long HieCod;			// Hierarchy code (institution/center/degree/course)
   long Cod;			// Code of file, forum post, notice, timeline post...
   bool Unavailable;		// File, forum post, notice,... unavailable (removed)
   time_t DateTimeUTC;		// Date-time of publication in UTC time
   unsigned NumShared;		// Number of times (users) this note has been shared
   unsigned NumFavs;		// Number of times (users) this note has been favourited
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Tml_Not_ShowHighlightedNote (struct Tml_Timeline *Timeline,
                                  struct Tml_Not_Note *Not);

void Tml_Not_CheckAndWriteNoteWithTopMsg (const struct Tml_Timeline *Timeline,
	                                  const struct Tml_Not_Note *Not,
                                          Tml_TopMessage_t TopMessage,
                                          long PublisherCod);
void Tml_Not_ShowAuthorPhoto (struct UsrData *UsrDat,bool FormUnique);
void Tml_Not_WriteAuthorName (const struct UsrData *UsrDat);

void Tml_Not_GetNoteSummary (const struct Tml_Not_Note *Not,
                             char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1]);

void Tml_Not_StoreAndPublishNote (Tml_Not_NoteType_t NoteType,long Cod);
void Tml_Not_StoreAndPublishNoteInternal (Tml_Not_NoteType_t NoteType,long Cod,
                                          struct Tml_Pub_Publication *Pub);
void Tml_Not_MarkNoteOneFileAsUnavailable (const char *Path);
void Tml_Not_MarkNotesChildrenOfFolderAsUnavailable (const char *Path);

void Tml_Not_PutHiddenParamNotCod (long NotCod);
long Tml_Not_GetParamNotCod (void);

void Tml_Not_RequestRemNoteUsr (void);
void Tml_Not_RequestRemNoteGbl (void);
void Tml_Not_RemoveNoteUsr (void);
void Tml_Not_RemoveNoteGbl (void);

void Tml_Not_GetDataOfNoteByCod (struct Tml_Not_Note *Not);

#endif
