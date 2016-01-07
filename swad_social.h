// swad_social.c: social networking (timeline)

#ifndef _SWAD_SOC
#define _SWAD_SOC
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
/********************************** Headers **********************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define Soc_NUM_SOCIAL_NOTES	13
// If the numbers assigned to each event type change,
// it is necessary to change old numbers to new ones in database table social_notes

typedef enum
  {
   Soc_NOTE_UNKNOWN		=  0,

   /* Institution tab */
   Soc_NOTE_INS_DOC_PUB_FILE	=  1,
   Soc_NOTE_INS_SHA_PUB_FILE	=  2,

   /* Centre tab */
   Soc_NOTE_CTR_DOC_PUB_FILE	=  3,
   Soc_NOTE_CTR_SHA_PUB_FILE	=  4,

   /* Degree tab */
   Soc_NOTE_DEG_DOC_PUB_FILE	=  5,
   Soc_NOTE_DEG_SHA_PUB_FILE	=  6,

   /* Course tab */
   Soc_NOTE_CRS_DOC_PUB_FILE	=  7,
   Soc_NOTE_CRS_SHA_PUB_FILE	=  8,

   /* Assessment tab */
   Soc_NOTE_EXAM_ANNOUNCEMENT	=  9,

   /* Users tab */

   /* Social tab */
   Soc_NOTE_SOCIAL_POST		= 10,
   Soc_NOTE_FORUM_POST		= 11,

   /* Messages tab */
   Soc_NOTE_NOTICE		= 12,

   /* Statistics tab */

   /* Profile tab */

  } Soc_NoteType_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Soc_ShowTimelineUsr (void);
void Soc_ShowTimelineGbl (void);

void Soc_StoreAndPublishSocialNote (Soc_NoteType_t NoteType,long Cod);
void Soc_MarkSocialNoteAsUnavailableUsingNotCod (long NotCod);
void Soc_MarkSocialNoteAsUnavailableUsingNoteTypeAndCod (Soc_NoteType_t NoteType,long Cod);
void Soc_MarkSocialNoteOneFileAsRemoved (const char *Path);
void Soc_MarkSocialNotesChildrenOfFolderAsUnavailable (const char *Path);

void Soc_FormSocialPostGbl (void);
void Soc_FormSocialPostUsr (void);
void Soc_ReceiveSocialPostGbl (void);
void Soc_ReceiveSocialPostUsr (void);

void Soc_ReceiveCommentGbl (void);
void Soc_ReceiveCommentUsr (void);

void Soc_ShareSocialNoteGbl (void);
void Soc_ShareSocialNoteUsr (void);
void Soc_UnshareSocialPubGbl (void);
void Soc_UnshareSocialPubUsr (void);

void Soc_RequestRemSocialPubGbl (void);
void Soc_RequestRemSocialPubUsr (void);
void Soc_RemoveSocialNotGbl (void);
void Soc_RemoveSocialNotUsr (void);

#endif
