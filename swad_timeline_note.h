// swad_timeline_note.h: social timeline notes

#ifndef _SWAD_TML_NOT
#define _SWAD_TML_NOT
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
/******************************** Public types *******************************/
/*****************************************************************************/

#define Tml_NOT_NUM_NOTE_TYPES	13
// If the numbers assigned to each event type change,
// it is necessary to change old numbers to new ones in database table tml_notes
typedef enum
  {
   TmlNot_UNKNOWN		=  0,
   /* Start tab */
   TmlNot_POST			= 10,	// Post written directly in timeline
   /* Institution tab */
   TmlNot_INS_DOC_PUB_FILE	=  1,	// Public file in documents of institution
   TmlNot_INS_SHA_PUB_FILE	=  2,	// Public file in shared files of institution
   /* Center tab */
   TmlNot_CTR_DOC_PUB_FILE	=  3,	// Public file in documents of center
   TmlNot_CTR_SHA_PUB_FILE	=  4,	// Public file in shared files of center
   /* Degree tab */
   TmlNot_DEG_DOC_PUB_FILE	=  5,	// Public file in documents of degree
   TmlNot_DEG_SHA_PUB_FILE	=  6,	// Public file in shared files of degree
   /* Course tab */
   TmlNot_CRS_DOC_PUB_FILE	=  7,	// Public file in documents of course
   TmlNot_CRS_SHA_PUB_FILE	=  8,	// Public file in shared files of course
   /* Assessment tab */
   TmlNot_CALL_FOR_EXAM		=  9,	// Call for exam in a course
   /* Users tab */
   /* Messages tab */
   TmlNot_NOTICE		= 12,	// A public notice in a course
   TmlNot_FORUM_POST		= 11,	// Post in global/swad forums
   /* Analytics tab */
   /* Profile tab */
  } TmlNot_Type_t;

struct TmlNot_Note
  {
   long NotCod;		// Unique code/identifier for each note
   TmlNot_Type_t Type;	// Timeline post, public file,
			// call for exam, notice, forum post...
   long UsrCod;		// Publisher
   long HieCod;		// Hierarchy code
			// (institution/center/degree/course)
   long Cod;		// Code of file, forum post,
			// notice, timeline post...
   Exi_Exist_t Exists;	// File, forum post, notice...
			// exists (not removed)
   time_t DateTimeUTC;	// Date-time of publication in UTC time
   unsigned NumShared;	// Number of times (users)
			// this note has been shared
   unsigned NumFavs;	// Number of times (users)
			// this note has been favourited
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void TmlNot_ShowHighlightedNote (struct Tml_Timeline *Timeline,
                                 struct TmlNot_Note *Not);

void TmlNot_CheckAndWriteNoteWithTopMsg (const struct Tml_Timeline *Timeline,
	                                 const struct TmlNot_Note *Not,
                                         Tml_TopMessage_t TopMessage,
                                         long PublisherCod);
void TmlNot_ShowAuthorPhoto (struct Usr_Data *UsrDat);
void TmlNot_WriteAuthorName (const struct Usr_Data *UsrDat,
                             const char *Class);

void TmlNot_GetNoteSummary (const struct TmlNot_Note *Not,
                            char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1]);

void TmlNot_StoreAndPublishNote (TmlNot_Type_t NoteType,long Cod);
void TmlNot_StoreAndPublishNoteInternal (TmlNot_Type_t NoteType,long Cod,
                                         struct TmlPub_Publication *Pub);
void TmlNot_MarkNoteOneFileAsUnavailable (const char *Path);
void TmlNot_MarkNotesChildrenOfFolderAsUnavailable (const char *Path);

void TmlNot_ReqRemNoteUsr (void);
void TmlNot_ReqRemNoteGbl (void);
void TmlNot_RemoveNoteUsr (void);
void TmlNot_RemoveNoteGbl (void);

void TmlNot_GetNoteDataByCod (struct TmlNot_Note *Not);

#endif
